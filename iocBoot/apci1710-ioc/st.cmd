#!../../bin/linuxRT_glibc-x86_64/apci1710App

< envPaths

## Register all support components
dbLoadDatabase("../../dbd/apci1710.dbd",0,0)
apci1710_registerRecordDeviceDriver(pdbbase) 

dbLoadTemplate("APCI1710.substitutions")

APCI1710Config("apci1710", 0)

iocInit()
