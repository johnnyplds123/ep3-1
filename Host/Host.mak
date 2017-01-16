#------------------------------------------------------------------------------
# Include path
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Object file list
#------------------------------------------------------------------------------
HOST_OBJS=

#------------------------------------------------------------------------------
# Include component make file
#------------------------------------------------------------------------------
include Host/Apl/Host_Apl.mak                    # build host application layer component
include Host/Hal/Host_Hal.mak                    # build host hardware abstraction layer component

HOST_OBJS += $(HOST_APL_OBJS)
HOST_OBJS += $(HOST_HAL_OBJS)

