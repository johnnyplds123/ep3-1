#------------------------------------------------------------------------------
# Include path
#------------------------------------------------------------------------------
INCLUDE_DIR  += -IShare

#------------------------------------------------------------------------------
# Object file list
#------------------------------------------------------------------------------
SHARE_OBJS=

#------------------------------------------------------------------------------
# Include component make file
#------------------------------------------------------------------------------
include Share/Apl/Share_Apl.mak                   # build share hardware abstraction layer component
include Share/Hal/Share_Hal.mak                   # build share hardware abstraction layer component

SHARE_OBJS += $(SHARE_APL_OBJS)
SHARE_OBJS += $(SHARE_HAL_OBJS)

