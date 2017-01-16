#------------------------------------------------------------------------------
# Include path
#------------------------------------------------------------------------------
INCLUDE_DIR += -IShare/Hal/$(_SOC_)
INCLUDE_DIR += -IShare/Hal/$(_SOC_)/Src
INCLUDE_DIR += -IShare/Hal/$(_SOC_)/Registers
INCLUDE_DIR += -IHost/Hal/$(_SOC_)/Registers

#------------------------------------------------------------------------------
# Object file list
#------------------------------------------------------------------------------
SHARE_HAL_OBJS += $(OBJ_DIR)/RegShare.o
SHARE_HAL_OBJS += $(OBJ_DIR)/HalSys.o
SHARE_HAL_OBJS += $(OBJ_DIR)/HalClock.o
SHARE_HAL_OBJS += $(OBJ_DIR)/HalCpu.o
SHARE_HAL_OBJS += $(OBJ_DIR)/HalUart.o
SHARE_HAL_OBJS += $(OBJ_DIR)/HalTimer.o
SHARE_HAL_OBJS += $(OBJ_DIR)/HalIpc.o
SHARE_HAL_OBJS += $(OBJ_DIR)/HalGpio.o
SHARE_HAL_OBJS += $(OBJ_DIR)/HalDmac.o
SHARE_HAL_OBJS += $(OBJ_DIR)/HalPm.o
SHARE_HAL_OBJS += $(OBJ_DIR)/HalTempSensor.o
SHARE_HAL_OBJS += $(OBJ_DIR)/HalSflash.o
SHARE_HAL_OBJS += $(OBJ_DIR)/HalI2c.o

ifeq ($(_CPU_),0)
SHARE_HAL_OBJS += $(OBJ_DIR)/HalSetm.o
SHARE_HAL_OBJS += $(OBJ_DIR)/HalIrqCpu0.o
SHARE_HAL_OBJS += $(OBJ_DIR)/HalDram.o
SHARE_HAL_OBJS += $(OBJ_DIR)/HalMc5DdrInit.o
SHARE_HAL_OBJS += $(OBJ_DIR)/HalDdrDll.o
SHARE_HAL_OBJS += $(OBJ_DIR)/HalHwCheck.o
endif

ifeq ($(_CPU_),1)
SHARE_HAL_OBJS += $(OBJ_DIR)/HalIrqCpu1.o
endif

ifeq ($(_CPU_),2)
SHARE_HAL_OBJS += $(OBJ_DIR)/HalIrqCpu2.o
SHARE_HAL_OBJS += $(OBJ_DIR)/HalHwCheck.o
endif

ifneq ($(_CPU_),0)
SHARE_HAL_OBJS += $(OBJ_DIR)/RegHost.o
endif

#------------------------------------------------------------------------------
# Include component make file
#------------------------------------------------------------------------------
