#------------------------------------------------------------------------------
# Include path
#------------------------------------------------------------------------------
INCLUDE_DIR += -IShare/Hal
INCLUDE_DIR += -IShare/Hal/Inc
INCLUDE_DIR += -IHost/Hal/Inc
INCLUDE_DIR += -IMedia/Hal/Inc
INCLUDE_DIR += -IHost/Hal/$(_SOC_)/Src
INCLUDE_DIR += -IMedia/Hal/$(_SOC_)/Src

#------------------------------------------------------------------------------
# Object file list
#------------------------------------------------------------------------------
SHARE_HAL_OBJS =
SHARE_HAL_OBJS += $(OBJ_DIR)/HalRegConfig.o

#------------------------------------------------------------------------------
# Include component make file
#------------------------------------------------------------------------------
include Share/Hal/$(_SOC_)/Share_Hal_$(_SOC_).mak     # build SoC specific SHARE HAL
