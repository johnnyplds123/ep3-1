#------------------------------------------------------------------------------
# Include path
#------------------------------------------------------------------------------
INCLUDE_DIR += -IHost/Hal/$(_SOC_)
INCLUDE_DIR += -IHost/Hal/$(_SOC_)/Src
INCLUDE_DIR += -IHost/Hal/$(_SOC_)/Src/$(_HOST_)
INCLUDE_DIR += -IHost/Hal/$(_SOC_)/Registers

#------------------------------------------------------------------------------
# Object file list
#------------------------------------------------------------------------------
HOST_HAL_OBJS += $(OBJ_DIR)/RegHost.o

HOST_HAL_OBJS += $(OBJ_DIR)/HalPciePhy.o
HOST_HAL_OBJS += $(OBJ_DIR)/HalPcieLink.o
HOST_HAL_OBJS += $(OBJ_DIR)/HalPcieIrq.o
HOST_HAL_OBJS += $(OBJ_DIR)/HalPciePm.o
HOST_HAL_OBJS += $(OBJ_DIR)/HalPcieErr.o
HOST_HAL_OBJS += $(OBJ_DIR)/HalPcieLtr.o

ifeq ($(_HOST_), NVME)
HOST_HAL_OBJS += $(OBJ_DIR)/HalNvmeXfer.o
HOST_HAL_OBJS += $(OBJ_DIR)/HalNvmeIrq.o
HOST_HAL_OBJS += $(OBJ_DIR)/HalNvmeError.o
HOST_HAL_OBJS += $(OBJ_DIR)/HalNvmeDebug.o
endif

ifeq ($(_HOST_), AHCI)
HOST_HAL_OBJS += $(OBJ_DIR)/HldHost.o
HOST_HAL_OBJS += $(OBJ_DIR)/HldSata.o
HOST_HAL_OBJS += $(OBJ_DIR)/HalDpp.o
HOST_HAL_OBJS += $(OBJ_DIR)/HalSecurity.o
HOST_HAL_OBJS += $(OBJ_DIR)/HalAhciXfer.o
HOST_HAL_OBJS += $(OBJ_DIR)/HalAhciIrq.o
endif

#------------------------------------------------------------------------------
# Include component make file
#------------------------------------------------------------------------------

