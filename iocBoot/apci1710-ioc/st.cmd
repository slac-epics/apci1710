#!../../bin/linuxRT_glibc-x86_64/apci1710App

< envPaths

## Register all support components
dbLoadDatabase("$(TOP)/dbd/apci1710.dbd",0,0)
apci1710_registerRecordDeviceDriver(pdbbase) 

dbLoadTemplate("APCI1710.substitutions")

epicsEnvSet(IOC_NAME,"VIOC:B34:EV20")
epicsEnvSet(EVR_DEV1,"EVR:B34:EV20")
epicsEnvSet(UNIT,"EV20")
epicsEnvSet(FAC,"SYS0")
epicsEnvSet(CRD,"0")
# System Location:
epicsEnvSet("LOCA","B34")

dbLoadRecords("$(TOP)/db/Pattern.db","IOC=${IOC_NAME},SYS=${FAC}")
dbLoadRecords("$(TOP)/db/EvrPci.db","EVR=${EVR_DEV1},CRD=${CRD},SYS=${FAC}")

# NOTE: If uncomment PCI-trig.db then we need to provide a device name (DEV)
#dbLoadRecords("$(TOP)/db/PCI-trig.db","IOC=${IOC_NAME},LOCA=${LOCA},UNIT=${UNIT},SYS=${FAC}")

# BSA Data Source Simulator Databases
#dbLoadRecords("db/bsaSimulator.db","DEVICE=LEPTON")

# BSA Database for each data source from above
#dbLoadRecords("db/Bsa.db","DEVICE=LEPTON, ATRB=COUNTER")
#dbLoadRecords("db/Bsa.db","DEVICE=LEPTON, ATRB=SINE")

eevrmaConfigure(0, "/dev/vevr0")

APCI1710Config("apci1710", 0)

iocInit()

epicsThreadSleep(2.0)

# Setup Real-time priorities after iocInit for
# driver threads
system("/bin/su root -c $(TOP)/iocBoot/apci1710-ioc/rtPrioritySetup.cmd")
