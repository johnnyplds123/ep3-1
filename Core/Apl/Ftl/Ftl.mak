#------------------------------------------------------------------------------
# Include path
#------------------------------------------------------------------------------
INCLUDE_DIR += -ICore/Apl/Ftl
INCLUDE_DIR += -ICore/Apl/Ftl/Ftl_$(_NAND_)

#------------------------------------------------------------------------------
# Object file list
#------------------------------------------------------------------------------
FTL_OBJS  =
FTL_OBJS += $(OBJ_DIR)/Ftl.o
FTL_OBJS += $(OBJ_DIR)/Ftl_$(_NAND_).o
