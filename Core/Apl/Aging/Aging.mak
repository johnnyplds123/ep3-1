#------------------------------------------------------------------------------
# Include path
#------------------------------------------------------------------------------
INCLUDE_DIR  += -ICore/Apl/Aging/
INCLUDE_DIR  += -ICore/Apl/Aging/$(_NAND_)/
#------------------------------------------------------------------------------
# Object file list
#------------------------------------------------------------------------------
AGING_OBJS  =
AGING_OBJS += $(OBJ_DIR)/Aging.o
AGING_OBJS += $(OBJ_DIR)/Aging_$(_NAND_).o

ifeq ($(_NAND_), 3DTLCG2)
	AGING_OBJS += Core/Apl/Aging/$(_NAND_)/Aging_$(_NAND_)_Addons.o
endif

