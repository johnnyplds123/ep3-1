#------------------------------------------------------------------------------
# Include path
#------------------------------------------------------------------------------
INCLUDE_DIR  += -IMedia/Apl/Descriptor
INCLUDE_DIR  += -IMedia/Apl/Descriptor/$(_VENDOR_)

#------------------------------------------------------------------------------
# Object file list
#------------------------------------------------------------------------------
MEDIA_APL_DESC_OBJS  =

MEDIA_APL_DESC_OBJS += $(OBJ_DIR)/AplDesc.o
MEDIA_APL_DESC_OBJS += $(OBJ_DIR)/AplDesc_$(_VENDOR_)_$(_NAND_).o

ifeq ($(_VENDOR_), TSB)
    ifeq ($(_NAND_), 3DTLCG2)
        MEDIA_APL_DESC_OBJS += Media/Apl/Descriptor/$(_VENDOR_)/AplDesc_$(_VENDOR_)_$(_NAND_)_Addons.o
    endif
endif
