#------------------------------------------------------------------------------
# Include path
#------------------------------------------------------------------------------
INCLUDE_DIR  += -ICore/Hal

#------------------------------------------------------------------------------
# Object file list
#------------------------------------------------------------------------------
CORE_HAL_OBJS =

#------------------------------------------------------------------------------
# Include component make file
#------------------------------------------------------------------------------
include Core/Hal/Bcm/Bcm.mak                  # build hostmsg component

CORE_HAL_OBJS += $(BCM_OBJS)
