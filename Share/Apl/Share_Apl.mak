#------------------------------------------------------------------------------
# Include path
#------------------------------------------------------------------------------
INCLUDE_DIR  += -IShare/Apl/
INCLUDE_DIR  += -IShare/Apl/Inc
INCLUDE_DIR  += -IShare/Apl/Tcg/
#------------------------------------------------------------------------------
# Object file list
#------------------------------------------------------------------------------
SHARE_APL_OBJS=

#------------------------------------------------------------------------------
# Include component make file
#------------------------------------------------------------------------------
include Share/Apl/Boot/Boot.mak                       # build Boot component
include Share/Apl/CmdSensor/CmdSensor.mak             # build CmdSensor component
include Share/Apl/NamingRule/NamingRule.mak           # build NamingRule component
include Share/Apl/Debug/Debug.mak                     # build Debug component
include Share/Apl/IDS/IDS.mak                         # build IDS component
include Share/Apl/Ipc/Ipc.mak                         # build Ipc component
include Share/Apl/Kernel/Kernel.mak                   # build Kernel component
include Share/Apl/Lib/Lib.mak                         # build Library component
include Share/Apl/Monitor/Monitor.mak                 # build Monitor component
include Share/Apl/SysInfo/SysInfo.mak                 # build Sysinfo
include Share/Apl/Tcg/Tcg.mak                         # build Tcg utilities
include Share/Apl/Utility/Utility.mak                 # build Firmware utilities
include Share/Apl/UncTable/UncTable.mak               # build Uncorrectable table
include Share/Apl/Purge/Purge.mak                     # build purge
include Share/Apl/PLP/PLP.mak                         # build PLP component
include Share/Apl/SmBus/SmBus.mak                     # build SmBus component

SHARE_APL_OBJS += $(BOOT_OBJS)
SHARE_APL_OBJS += $(CMDSENSOR_OBJS)
SHARE_APL_OBJS += $(NAMINGRULE_OBJS)
SHARE_APL_OBJS += $(DEBUG_OBJS)
SHARE_APL_OBJS += $(IDS_OBJS)
SHARE_APL_OBJS += $(IPC_OBJS)
SHARE_APL_OBJS += $(KERNEL_OBJS)
SHARE_APL_OBJS += $(LIB_OBJS)
SHARE_APL_OBJS += $(MONITOR_OBJS)
SHARE_APL_OBJS += $(SYSINFO_OBJS)
SHARE_APL_OBJS += $(TCG_OBJS)
SHARE_APL_OBJS += $(UTILITY_OBJS)
SHARE_APL_OBJS += $(UNCTABLE_OBJS)
SHARE_APL_OBJS += $(PURGE_OBJS)
SHARE_APL_OBJS += $(PLP_OBJS)
SHARE_APL_OBJS += $(SMBUS_OBJS)