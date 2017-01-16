#------------------------------------------------------------------------------
# Include path
#------------------------------------------------------------------------------
INCLUDE_DIR  += -ICore/Apl
INCLUDE_DIR  += -IMedia/Apl/Scheduler

#------------------------------------------------------------------------------
# Object file list
#------------------------------------------------------------------------------
CORE_APL_OBJS =

CORE_APL_OBJS += $(OBJ_DIR)/Core.o
CORE_APL_OBJS += $(OBJ_DIR)/CoreMon.o

#------------------------------------------------------------------------------
# Include component make file
#------------------------------------------------------------------------------
include Core/Apl/CoreMsg/CoreMsg.mak                  # build core msg component
include Core/Apl/CoreDbg/CoreDbg.mak                  # build core debug component
include Core/Apl/Ftl/Ftl.mak                          # build core Ftl layer component
include Core/Apl/Gc/Gc.mak                            # build core GC  layer component
include Core/Apl/ErrorHandle/ErrHandle.mak            # build core ErrorHandler component
include Core/Apl/Aging/Aging.mak                      # build core Aging component
include Core/Apl/Thermal/Thermal.mak                  # build core Thermal component

CORE_APL_OBJS += $(COREMSG_OBJS)
CORE_APL_OBJS += $(COREDBG_OBJS)
CORE_APL_OBJS += $(FTL_OBJS)
CORE_APL_OBJS += $(GC_OBJS)
CORE_APL_OBJS += $(ERH_OBJS)
CORE_APL_OBJS += $(AGING_OBJS)
CORE_APL_OBJS += $(THERMAL_OBJS)
