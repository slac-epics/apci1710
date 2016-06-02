/**
 * ----------------------------------------------------------------------------
 * File       : drvApci1710.cpp
 * Author     : Chris Ford, caf@slac.stanford.edu
 * Created    : 2016-03-30
 * Last update: 2016-05-06
 * ----------------------------------------------------------------------------
 * Description:
 * Driver for ADDI-DATA APCI1710 incremental counter using asynPortDriver
 * base class.  Implements simple (polled) counters plus real-time counters.
 * ----------------------------------------------------------------------------
 * This file is part of apci1710-asyn. It is subject to
 * the license terms in the LICENSE.txt file found in the top-level directory
 * of this distribution and at:
 *   https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html.
 * No part of apci1710-asyn, including this file, may be                       
 * copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE.txt file.
 * ----------------------------------------------------------------------------
**/

#include <stdio.h>
#include <limits.h>
#include "sys/ioctl.h"

#include <iocsh.h>
#include <epicsExport.h>
#include <epicsThread.h>
#include "epicsTime.h"
#include "evrTime.h"
#include <asynPortDriver.h>

#include "apci1710ctr_buf.h"
#include "apci1710ctr_ioctl.h"

static const char *driverName = "APCI1710";

// Counter parameters
#define counterCountsString       "COUNTER_VALUE"
#define counterRTCountsString     "RT_COUNTER_VALUE"
#define counterPositionString     "POSITION"
#define counterRTPositionString   "RT_POSITION"
#define counterResetString        "COUNTER_RESET"

#define NUM_COUNTERS    4   // Number counters on APCI1710
#define MAX_SIGNALS     NUM_COUNTERS
#define DEFAULT_POLL_TIME 0.1

/* counter interface */
int ctr_init(int boardNum);
int ctr_reset(int boardNum, int adrs);
int ctr_in(int boardNum, int adrs, unsigned long *count);

bool _verbose = false;

/** Class definition for the APCI1710 class
  */
class APCI1710 : public asynPortDriver {
public:
  APCI1710(const char *portName, int boardNum);

  /* These are the methods that we override from asynPortDriver */
  virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
  virtual void report(FILE *fp, int details);
  // These should be private but are called from C
  virtual void pollerThread(void);
  virtual void interruptThread(int adrs);

protected:
  // Counter parameters
  int counterCounts_;
  #define FIRST_APCI1710_PARAM  counterCounts_

  int counterPosition_;

  int counterRTCounts_;
  int counterRTPosition_;

  int counterReset_;
  #define LAST_APCI1710_PARAM   counterReset_

private:
  int boardNum_;
  double pollTime_;
  int forceCallback_;
};

#define NUM_PARAMS (&LAST_APCI1710_PARAM - &FIRST_APCI1710_PARAM + 1)

static void pollerThreadC(void * pPvt)
{
    APCI1710 *pAPCI1710 = (APCI1710 *)pPvt;
    pAPCI1710->pollerThread();
}

static void interrupt0ThreadC(void * pPvt)
{
    APCI1710 *pAPCI1710 = (APCI1710 *)pPvt;
    pAPCI1710->interruptThread(0);
}

static void interrupt1ThreadC(void * pPvt)
{
    APCI1710 *pAPCI1710 = (APCI1710 *)pPvt;
    pAPCI1710->interruptThread(1);
}

static void interrupt2ThreadC(void * pPvt)
{
    APCI1710 *pAPCI1710 = (APCI1710 *)pPvt;
    pAPCI1710->interruptThread(2);
}

static void interrupt3ThreadC(void * pPvt)
{
    APCI1710 *pAPCI1710 = (APCI1710 *)pPvt;
    pAPCI1710->interruptThread(3);
}

/** Constructor for the APCI1710 class
  */
APCI1710::APCI1710(const char *portName, int boardNum)
  : asynPortDriver(portName, MAX_SIGNALS, NUM_PARAMS,
      asynInt32Mask | asynDrvUserMask,  // Interfaces that we implement
      asynInt32Mask,                    // Interfaces that do callbacks
      ASYN_MULTIDEVICE | ASYN_CANBLOCK, 1, /* ASYN_CANBLOCK=1, ASYN_MULTIDEVICE=1, autoConnect=1 */
      0, 0),  /* Default priority and stack size */
    boardNum_(boardNum),
    pollTime_(DEFAULT_POLL_TIME),
    forceCallback_(1)
{
  // Counter parameters
  createParam(counterCountsString,             asynParamInt32, &counterCounts_);
  createParam(counterRTCountsString,           asynParamInt32, &counterRTCounts_);
  createParam(counterPositionString,           asynParamInt32, &counterPosition_);
  createParam(counterRTPositionString,         asynParamInt32, &counterRTPosition_);
  createParam(counterResetString,              asynParamInt32, &counterReset_);

  // initialize counter interface
  if (ctr_init(boardNum)) {
    // error
    printf("%s:%s, port %s, ERROR returned from ctr_init(%d)\n",
           driverName, __FUNCTION__, portName, boardNum);
  } else {
    // Start the thread to poll inputs and do callbacks to
    // device support
    epicsThreadCreate("APCI1710Poller",
                      epicsThreadPriorityLow,
                      epicsThreadGetStackSize(epicsThreadStackMedium),
                      (EPICSTHREADFUNC)pollerThreadC,
                      this);
    // Start threads to receive interrupts and do callbacks to
    // device support
    epicsThreadCreate("APCI1710Interrupt0",
                      epicsThreadPriorityHigh,
                      epicsThreadGetStackSize(epicsThreadStackMedium),
                      (EPICSTHREADFUNC)interrupt0ThreadC,
                      this);
    epicsThreadCreate("APCI1710Interrupt1",
                      epicsThreadPriorityHigh,
                      epicsThreadGetStackSize(epicsThreadStackMedium),
                      (EPICSTHREADFUNC)interrupt1ThreadC,
                      this);
    epicsThreadCreate("APCI1710Interrupt2",
                      epicsThreadPriorityHigh,
                      epicsThreadGetStackSize(epicsThreadStackMedium),
                      (EPICSTHREADFUNC)interrupt2ThreadC,
                      this);
    epicsThreadCreate("APCI1710Interrupt3",
                      epicsThreadPriorityHigh,
                      epicsThreadGetStackSize(epicsThreadStackMedium),
                      (EPICSTHREADFUNC)interrupt3ThreadC,
                      this);
  }
}

/* ----------------------------------------------------------------------------------- */

#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h"

const char* procName[NUM_COUNTERS] = { "/proc/driver/apci1710ctr0/counter",
                                       "/proc/driver/apci1710ctr1/counter",
                                       "/proc/driver/apci1710ctr2/counter",
                                       "/proc/driver/apci1710ctr3/counter" };

static int readProcFd[NUM_COUNTERS];
static int writeProcFd[NUM_COUNTERS];

const char* devName[NUM_COUNTERS] = { "/dev/apci1710ctr_0",
                                       "/dev/apci1710ctr_1",
                                       "/dev/apci1710ctr_2",
                                       "/dev/apci1710ctr_3" };

static int devFd[NUM_COUNTERS];

/*
 * ctr_init - initialize counter interface
 *
 * Note: ctr_init() must be called once, before any calls to
 * ctr_reset() or ctr_in().
 */
int ctr_init(int boardNum)
{
  int ii;
  int err = 0;

  for (ii = 0; ii < NUM_COUNTERS; ii++) {
    readProcFd[ii]  = open(procName[ii], O_RDONLY);
    if (readProcFd[ii] == -1) {
      perror("ctr_init open procName O_RDONLY");
      err ++;
    }
    writeProcFd[ii] = open(procName[ii], O_WRONLY);
    if (readProcFd[ii] == -1) {
      perror("ctr_init open procName O_WRONLY");
      err ++;
    }
    devFd[ii] = open(devName[ii], O_RDONLY);
    if (devFd[ii] == -1) {
      perror("ctr_init open devName O_RDONLY");
      err ++;
    }
  }
  return err;
}

/*
 * ctr_reset - reset counter value to zero
 *
 * Note: ctr_init() must be called once, before any calls to
 * ctr_reset() or ctr_in().
 */
int ctr_reset(int boardNum, int adrs)
{
  int rv = -1;

  if ((adrs < NUM_COUNTERS) && (writeProcFd[adrs])) {
    if ((lseek(writeProcFd[adrs], 0, SEEK_SET) != (off_t)-1) &&
        (write(writeProcFd[adrs], "0", 1) == 1)) {
      rv = 0; /* success */
    }
  }
  return rv;
}

/*
 * ctr_in - read counter value
 *
 * Note: ctr_init() must be called once, before any calls to
 * ctr_reset() or ctr_in().
 */
int ctr_in(int boardNum, int adrs, unsigned long *count)
{
  int rv = -1;
  ssize_t len;
  char buf[80];

  if ((adrs < NUM_COUNTERS) && (readProcFd[adrs])) {
    if ((lseek(readProcFd[adrs], 0, SEEK_SET) != (off_t)-1)) {
      rv = read(readProcFd[adrs], (void *)buf, sizeof(buf));
      if (rv == -1) {
        perror("ctr_in: read");
        epicsThreadSleep(1.0);    // avoid spinning too fast in error loop
      } else if (rv > 0) {
        len = sscanf(buf, "%ld", count);
        if (len == 1) {
          rv = 0;           /* success */
        }
      }
    }
  }
  return rv;
}

/* ----------------------------------------------------------------------------------- */

asynStatus APCI1710::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
  int addr;
  int function = pasynUser->reason;
  int status=0;
  static const char *functionName = "writeInt32";

  this->getAddress(pasynUser, &addr);
  setIntegerParam(addr, function, value);

  // Counter functions
  if (function == counterReset_) {
    // reset counter to zero
    status = ctr_reset(boardNum_, addr);
  }

  callParamCallbacks(addr);
  if (status == 0) {
    asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
             "%s:%s, port %s, wrote %d to address %d\n",
             driverName, functionName, this->portName, value, addr);
  } else {
    asynPrint(pasynUser, ASYN_TRACE_ERROR,
             "%s:%s, port %s, ERROR writing %d to address %d, status=%d\n",
             driverName, functionName, this->portName, value, addr, status);
  }
  return (status==0) ? asynSuccess : asynError;
}

void APCI1710::pollerThread()
{
  /* This function runs in a separate thread.  It waits for the poll time */
  static const char *functionName = "pollerThread";
  unsigned long countVal;
  int i;
  int status;

  while(1) {
    lock();

    // Read the counter inputs
    for (i=0; i<NUM_COUNTERS; i++) {
      status = ctr_in(boardNum_, i, &countVal);
      if (status) {
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
                  "%s:%s: ERROR calling ctr_in, status=%d\n", 
                  driverName, functionName, status);
        epicsThreadSleep(1.0);        // avoid spinning too fast in error loop
      } else {
        setIntegerParam(i, counterCounts_, countVal);
        setIntegerParam(i, counterPosition_, countVal);
      }
    }

    for (i=0; i<MAX_SIGNALS; i++) {
      callParamCallbacks(i);
    }

    unlock();
    epicsThreadSleep(pollTime_);
  }
}

void APCI1710::interruptThread(int adrs)
{
  /* This function runs in a separate thread.  */

  ssize_t rv;
  counterBuf_t buf;
  epicsTimeStamp time;

  rv = ioctl(devFd[adrs], APCI1710CTR_IOCINTENABLE, 0);  // enable latch interrupt
  if (rv) {
    asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, "interruptThread(%d): ioctl(APCI1710CTR_IOCINTENABLE) returned %zd\n", adrs, rv);
  }

  while(1) { 

    // read() can block
    rv = read(devFd[adrs], &buf, sizeof(buf));

    if (rv == sizeof(buf)) {
      evrTimeGet(&time, 0);         // get 120Hz resolution timestamp, timeslot 1 and timeslot 4
      if (setTimeStamp(&time) != asynSuccess) {
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, "interruptThread(%d): setTimeStamp() failed\n", adrs);
      }
      setIntegerParam(adrs, counterRTCounts_, buf.counter + 1);
      setIntegerParam(adrs, counterRTCounts_, buf.counter);
      setIntegerParam(adrs, counterRTPosition_, buf.counter + 1);
      setIntegerParam(adrs, counterRTPosition_, buf.counter);
      callParamCallbacks(adrs);
    } else {
      asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, "interruptThread(%d): read() returned %zd\n", adrs, rv);
      epicsThreadSleep(1.0);        // avoid spinning too fast in error loop
    }
  }
}

/* Report  parameters */
void APCI1710::report(FILE *fp, int details)
{

  fprintf(fp, "  Port: %s, board number=%d\n",
          this->portName, boardNum_);

  asynPortDriver::report(fp, details);
}

/** Configuration command, called directly or from iocsh */
extern "C" int APCI1710Config(const char *portName, int boardNum)
{
  APCI1710 *pAPCI1710 = new APCI1710(portName, boardNum);
  pAPCI1710 = NULL;  /* This is just to avoid compiler warnings */
  return(asynSuccess);
}

static const iocshArg configArg0 = { "Port name",      iocshArgString};
static const iocshArg configArg1 = { "Board number",      iocshArgInt};
static const iocshArg * const configArgs[] = {&configArg0,
                                              &configArg1};
static const iocshFuncDef configFuncDef = {"APCI1710Config", 2, configArgs};
static void configCallFunc(const iocshArgBuf *args)
{
  APCI1710Config(args[0].sval, args[1].ival);
}

void drvAPCI1710Register(void)
{
  iocshRegister(&configFuncDef,configCallFunc);
}

extern "C" {
epicsExportRegistrar(drvAPCI1710Register);
}
