# Makefile

TOP = ../..
include $(TOP)/configure/CONFIG

#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================
#==================================================
# Build an IOC support library

# <name>.dbd will be created from <name>Include.dbd
DBD += apci1710.dbd
DBD += apci1710Support.dbd

LIBRARY_IOC = apci1710

LIB_SRCS += drvApci1710.cpp
LIB_LIBS += asyn 
LIB_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================
# build an ioc application

PROD_IOC = apci1710App

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
PROD_SRCS_DEFAULT += apci1710_registerRecordDeviceDriver.cpp apci1710Main.cpp

PROD_LIBS += apci1710
PROD_LIBS += asyn
PROD_LIBS += $(EPICS_BASE_IOC_LIBS)

#
include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
