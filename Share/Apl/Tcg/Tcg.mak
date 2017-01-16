#------------------------------------------------------------------------------
# Include path
#------------------------------------------------------------------------------
INCLUDE_DIR  += -IShare/Apl/Tcg/

#------------------------------------------------------------------------------
# Object file list
#------------------------------------------------------------------------------
TCG_OBJS  =

ifneq ($(_TCG_),NONE)
    ifeq ($(_CPU_),0)
        TCG_OBJS += $(OBJ_DIR)/Tcg.o
        TCG_OBJS += $(OBJ_DIR)/TcgTbl.o
    else ifeq ($(_CPU_),2)
        TCG_OBJS += $(OBJ_DIR)/TcgNfIf.o
        TCG_OBJS += $(OBJ_DIR)/TcgNfRdWr.o
    endif
endif
