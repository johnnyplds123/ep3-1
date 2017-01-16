#------------------------------------------------------------------------------
# Include path
#------------------------------------------------------------------------------
INCLUDE_DIR  += -IShare/Apl/Monitor/

#------------------------------------------------------------------------------
# Object file list
#------------------------------------------------------------------------------
MONITOR_OBJS  =

ifeq ($(_CPU_),0)
MONITOR_OBJS += $(OBJ_DIR)/Monitor.o
MONITOR_OBJS += $(OBJ_DIR)/MonCmd.o
    ifeq ($(_MEDIA_),NAND)
    MONITOR_OBJS += $(OBJ_DIR)/MonCmdMedia.o
    MONITOR_OBJS += $(OBJ_DIR)/MonCmdCore.o
    endif
endif

