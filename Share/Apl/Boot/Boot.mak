#------------------------------------------------------------------------------
# Include path
#------------------------------------------------------------------------------
INCLUDE_DIR  += -IShare/Apl/Boot

#------------------------------------------------------------------------------
# Object file list
#------------------------------------------------------------------------------
BOOT_OBJS  =

ifeq ($(_CPU_),0)
BOOT_OBJS += $(OBJ_DIR)/Cpu0Boot.o
BOOT_OBJS += $(OBJ_DIR)/Cpu0Main.o
BOOT_OBJS += $(OBJ_DIR)/LoadFirmware.o
endif

ifeq ($(_CPU_),1)
BOOT_OBJS += $(OBJ_DIR)/Cpu1Boot.o
BOOT_OBJS += $(OBJ_DIR)/Cpu1Main.o
endif

ifeq ($(_CPU_),2)
BOOT_OBJS += $(OBJ_DIR)/Cpu2Boot.o
BOOT_OBJS += $(OBJ_DIR)/Cpu2Main.o
endif
