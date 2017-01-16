#------------------------------------------------------------------------------
# Include path
#------------------------------------------------------------------------------
INCLUDE_DIR += -IMedia/Hal/$(_SOC_)
INCLUDE_DIR += -IMedia/Hal/$(_SOC_)/Src
INCLUDE_DIR += -IMedia/Hal/$(_SOC_)/Src/Ecu
INCLUDE_DIR += -IMedia/Hal/$(_SOC_)/RegistersR2
INCLUDE_DIR += -IMedia/Hal/$(_SOC_)/Src/NfCtrl/$(_VENDOR_)

#------------------------------------------------------------------------------
# Object file list
#------------------------------------------------------------------------------
ifneq ($(_MEDIA_), NONE)
    MEDIA_HAL_OBJS += $(OBJ_DIR)/RegMedia.o
    MEDIA_HAL_OBJS += $(OBJ_DIR)/HalNfif.o
    MEDIA_HAL_OBJS += $(OBJ_DIR)/HalEcu.o
    MEDIA_HAL_OBJS += $(OBJ_DIR)/HalFlc.o
    MEDIA_HAL_OBJS += $(OBJ_DIR)/HalSeq.o
    MEDIA_HAL_OBJS += $(OBJ_DIR)/HalFlexible.o
    MEDIA_HAL_OBJS += $(OBJ_DIR)/HalLdpcLoadSch.o
    MEDIA_HAL_OBJS += $(OBJ_DIR)/HalLdpcSchDb.o
    MEDIA_HAL_OBJS += $(OBJ_DIR)/HalNfCtrl_$(_VENDOR_)_$(_NAND_).o
endif
#------------------------------------------------------------------------------
# Include component make file
#------------------------------------------------------------------------------

