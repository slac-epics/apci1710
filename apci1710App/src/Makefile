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

# [see apci1710Include.dbd]
#
# include "base.dbd"
# include "asyn.dbd"
# include "apci1710Support.dbd"
# include "evrSupport.dbd"
# include "devMrfEr.dbd"

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
# ------------ begin EVR ------------
PROD_LIBS += evrSupport
PROD_LIBS += devMrfEr
#ifeq ($(T_A),linuxRT_glibc-x86_64)
  PROD_LIBS += drvMrf
  evrma_DIR = $(EVRMA_LIB)
  PROD_LIBS += evrma
#endif

#ifeq ($(T_A),linux-x86_64)
  PROD_LIBS += drvMrf
  evrma_DIR = $(EVRMA_LIB)
  PROD_LIBS += evrma
#endif

# ------------ end EVR --------------
PROD_LIBS += $(EPICS_BASE_IOC_LIBS)

#
include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
