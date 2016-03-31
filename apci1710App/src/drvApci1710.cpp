/* drvApci1710.cpp
 *
 * Driver for ADDI-DATA APCI1710 incremental counter using asynPortDriver base class
 *
 * This version implements simple counters
 *
 * Chris Ford
 * March 30, 2016
*/

#include <stdio.h>
#include <limits.h>

#include <iocsh.h>
#include <epicsExport.h>
#include <epicsThread.h>
#include <asynPortDriver.h>

static const char *driverName = "APCI1710";

// Counter parameters
#define counterCountsString       "COUNTER_VALUE"
#define counterResetString        "COUNTER_RESET"

#define NUM_COUNTERS    4   // Number counters on APCI1710
#define MAX_SIGNALS     NUM_COUNTERS
#define DEFAULT_POLL_TIME 0.01 

/* counter interface */
int ctr_init(int boardNum);
int ctr_reset(int boardNum, int adrs);
int ctr_in(int boardNum, int adrs, unsigned long *count);

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

protected:
  // Counter parameters
  int counterCounts_;
  #define FIRST_APCI1710_PARAM  counterCounts_

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
  createParam(counterResetString,              asynParamInt32, &counterReset_);

  ctr_init(0);    // initialize counter interface

  /* Start the thread to poll digital inputs and do callbacks to
   * device support */
  epicsThreadCreate("APCI1710Poller",
                    epicsThreadPriorityLow,
                    epicsThreadGetStackSize(epicsThreadStackMedium),
                    (EPICSTHREADFUNC)pollerThreadC,
                    this);
}

/* ----------------------------------------------------------------------------------- */

#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h"

const char* procname[NUM_COUNTERS] = { "/proc/driver/apci1710ctr0/counter",
                                       "/proc/driver/apci1710ctr1/counter",
                                       "/proc/driver/apci1710ctr2/counter",
                                       "/proc/driver/apci1710ctr3/counter" };

static int readFd[NUM_COUNTERS];
static int writeFd[NUM_COUNTERS];

/*
 * ctr_init - initialize counter interface
 *
 * Note: ctr_init() must be called once, before any calls to
 * ctr_reset() or ctr_in().
 */
int ctr_init(int boardNum)
{
  int ii;

  for (ii = 0; ii < NUM_COUNTERS; ii++) {
    readFd[ii]  = open(procname[ii], O_RDONLY);
    writeFd[ii] = open(procname[ii], O_WRONLY);
  }
  return 0;
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

  if ((adrs < NUM_COUNTERS) && (writeFd[adrs])) {
    if ((lseek(writeFd[adrs], 0, SEEK_SET) != (off_t)-1) &&
        (write(writeFd[adrs], "0", 1) == 1)) {
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

  if ((adrs < NUM_COUNTERS) && (readFd[adrs])) {
    if ((lseek(readFd[adrs], 0, SEEK_SET) != (off_t)-1)) {
      if (read(readFd[adrs], (void *)buf, sizeof(buf)) > 0) {
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
  unsigned int pulseid;

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
      }

      setIntegerParam(i, counterCounts_, countVal);
    }
    
    for (i=0; i<MAX_SIGNALS; i++) {
      callParamCallbacks(i);
    }
    unlock();
    epicsThreadSleep(pollTime_);
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
