#!../../bin/linuxRT_glibc-x86_64/apci1710App

< envPaths

## Register all support components
dbLoadDatabase("$(TOP)/dbd/apci1710.dbd",0,0)
apci1710_registerRecordDeviceDriver(pdbbase) 

# EVR macros
epicsEnvSet(IOC_NAME,"VIOC:B34:EV20")
epicsEnvSet(EVR_DEV1,"EVR:B34:EV20")
epicsEnvSet(UNIT,"EV20")
epicsEnvSet(FAC,"SYS0")
epicsEnvSet(CRD,"0")
# System Location:
epicsEnvSet("LOCA","B34")

# Counter macros
epicsEnvSet("COUNTER_PREFIX","WIRE:B34:")
epicsEnvSet("COUNTER_PORT","apci1710")

# ASYN Database
dbLoadRecords("$(ASYN)/db/asynRecord.db","P=${COUNTER_PREFIX},R=asyn1,PORT=${COUNTER_PORT},ADDR=0,IMAX=80,OMAX=80")

# Counter Database
dbLoadRecords("$(TOP)/db/APCI1710Counter.db","P=${COUNTER_PREFIX},R=WS01:,PORT=${COUNTER_PORT},ADDR=0,ESLO=2,EOFF=0,BSA_FLNK='',EGU=um")
dbLoadRecords("$(TOP)/db/APCI1710Counter.db","P=${COUNTER_PREFIX},R=WS02:,PORT=${COUNTER_PORT},ADDR=1,ESLO=2,EOFF=0,BSA_FLNK='',EGU=um")
dbLoadRecords("$(TOP)/db/APCI1710Counter.db","P=${COUNTER_PREFIX},R=WS03:,PORT=${COUNTER_PORT},ADDR=2,ESLO=2,EOFF=0,BSA_FLNK='',EGU=um")
dbLoadRecords("$(TOP)/db/APCI1710Counter.db","P=${COUNTER_PREFIX},R=WS04:,PORT=${COUNTER_PORT},ADDR=3,ESLO=2,EOFF=0,BSA_FLNK='',EGU=um")

# Timing Databases
dbLoadRecords("$(TOP)/db/Pattern.db","IOC=${IOC_NAME},SYS=${FAC}")
dbLoadRecords("$(TOP)/db/EvrPci.db","EVR=${EVR_DEV1},CRD=${CRD},SYS=${FAC}")

# NOTE: If uncomment PCI-trig.db then we need to provide a device name (DEV)
#dbLoadRecords("$(TOP)/db/PCI-trig.db","IOC=${IOC_NAME},LOCA=${LOCA},UNIT=${UNIT},SYS=${FAC}")

# BSA Data Source Simulator Databases
#dbLoadRecords("db/bsaSimulator.db","DEVICE=${COUNTER_PREFIX}")

# BSA Database for each data source from above
#dbLoadRecords("db/Bsa.db","DEVICE=${COUNTER_PREFIX}, ATRB=WS01:RTPOSN")
#dbLoadRecords("db/Bsa.db","DEVICE=${COUNTER_PREFIX}, ATRB=WS02:RTPOSN")
#dbLoadRecords("db/Bsa.db","DEVICE=${COUNTER_PREFIX}, ATRB=WS03:RTPOSN")
#dbLoadRecords("db/Bsa.db","DEVICE=${COUNTER_PREFIX}, ATRB=WS04:RTPOSN")

eevrmaConfigure(0, "/dev/vevr0")

APCI1710Config("apci1710", 0)

iocInit()

epicsThreadSleep(2.0)

# Setup Real-time priorities after iocInit for
# driver threads
system("/bin/su root -c $(TOP)/iocBoot/apci1710-ioc/rtPrioritySetup.cmd")
