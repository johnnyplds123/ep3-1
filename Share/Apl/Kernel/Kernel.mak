#------------------------------------------------------------------------------
# Include path
#------------------------------------------------------------------------------
INCLUDE_DIR  += -IShare/Apl/Kernel/

#------------------------------------------------------------------------------
# Object file list
#------------------------------------------------------------------------------
KERNEL_OBJS  =

#KERNEL_OBJS += Share/Apl/Lib/cpu$(_CPU_)/Kernel.o
#KERNEL_OBJS += Share/Apl/Lib/cpu$(_CPU_)/KernelIdleTask.o
#KERNEL_OBJS += Share/Apl/Lib/cpu$(_CPU_)/KernelEvent.o
#KERNEL_OBJS += Share/Apl/Lib/cpu$(_CPU_)/KernelMutex.o
#KERNEL_OBJS += Share/Apl/Lib/cpu$(_CPU_)/KernelSemaphore.o
#KERNEL_OBJS += Share/Apl/Lib/cpu$(_CPU_)/KernelDlQueue.o
#KERNEL_OBJS += Share/Apl/Lib/cpu$(_CPU_)/KernelMessage.o

KERNEL_OBJS += $(OBJ_DIR)/Kernel.o
KERNEL_OBJS += $(OBJ_DIR)/KernelIdleTask.o
KERNEL_OBJS += $(OBJ_DIR)/KernelEvent.o
KERNEL_OBJS += $(OBJ_DIR)/KernelMutex.o
KERNEL_OBJS += $(OBJ_DIR)/KernelSemaphore.o
KERNEL_OBJS += $(OBJ_DIR)/KernelDlQueue.o
KERNEL_OBJS += $(OBJ_DIR)/KernelMessage.o


