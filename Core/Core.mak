#------------------------------------------------------------------------------
# Include path
#------------------------------------------------------------------------------
INCLUDE_DIR  += -ICore

#------------------------------------------------------------------------------
# Object file list
#------------------------------------------------------------------------------
CORE_OBJS   =

#------------------------------------------------------------------------------
# Include component make file
#------------------------------------------------------------------------------
include Core/Apl/Core_Apl.mak                    # build core application layer component
include Core/Hal/Core_Hal.mak                    # build core hardware layer component

CORE_OBJS += $(CORE_APL_OBJS)
CORE_OBJS += $(CORE_HAL_OBJS)


