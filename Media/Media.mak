#------------------------------------------------------------------------------
# Include path
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Object file list
#------------------------------------------------------------------------------
MEDIA_OBJS=

#------------------------------------------------------------------------------
# Include component make file
#------------------------------------------------------------------------------
include Media/Apl/Media_Apl.mak                    # build Media application layer component
include Media/Hal/Media_Hal.mak                    # build Media hardware abstraction layer component

MEDIA_OBJS += $(MEDIA_APL_OBJS)
MEDIA_OBJS += $(MEDIA_HAL_OBJS)

