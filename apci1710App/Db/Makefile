TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#----------------------------------------------------
#  Optimization of db files using dbst (DEFAULT: NO)
#DB_OPT = YES

#----------------------------------------------------
# Create and install (or just install)
# databases, templates, substitutions like this
DB += dbExample.db

# ========================================
# Install timing/event related databases
# ========================================
DB_INSTALLS += $(EVENT)/db/EvrPmc.db
DB_INSTALLS += $(EVENT)/db/EvrPci.db
DB_INSTALLS += $(EVENT)/db/Pattern.db
DB_INSTALLS += $(EVENT)/db/PCI-trig.db
DB_INSTALLS += $(EVENT)/db/PMC-trig.db
DB_INSTALLS += $(EVENT)/db/VME-trig.db
DB_INSTALLS += $(EVENT)/db/VME-TTB-trig.db
DB_INSTALLS += $(EVENT)/db/Bsa.db
#
# For event module BSA Test
DB += bsaSimulator.db

# For ADDI-DATA asyn Driver
DB += APCI1710Counter.db

# Install Common databases needed by All IOCs
# ========================================================
# Install databases from iocAdmin package
# These databases support IOC Health and Monitoring
# ========================================================
# HardIOC using RTEMS
DB_INSTALLS += $(IOCADMIN)/db/iocAdminRTEMS.db
# SoftIOC
DB_INSTALLS += $(IOCADMIN)/db/iocAdminSoft.db

DB_INSTALLS += $(IOCADMIN)/db/iocAdminScanMon.db
DB_INSTALLS += $(IOCADMIN)/db/iocScanMonSum.db
DB_INSTALLS += $(IOCADMIN)/db/iocScanMon.db
IOCRELEASE_DB += iocRelease.db

# Install Database to support reading the status of the autosave package:
DB_INSTALLS += $(AUTOSAVE)/db/save_restoreStatus.db

# =======================================================================

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

