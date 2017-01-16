#------------------------------------------------------------------------------
# Include path
#------------------------------------------------------------------------------
INCLUDE_DIR  += -IShare/Apl/Lib/
INCLUDE_DIR  += -ICore/Hal/Bcm/
#------------------------------------------------------------------------------
# Object file list
#------------------------------------------------------------------------------
LIB_OBJS  =

LIB_OBJS += $(OBJ_DIR)/CharIo.o
LIB_OBJS += $(OBJ_DIR)/MemIo.o
LIB_OBJS += $(OBJ_DIR)/MemTest.o
LIB_OBJS += $(OBJ_DIR)/MemAlloc.o
LIB_OBJS += $(OBJ_DIR)/MemAllocAo.o
LIB_OBJS += $(OBJ_DIR)/MemAllocVs.o
LIB_OBJS += $(OBJ_DIR)/Xmodem.o
LIB_OBJS += $(OBJ_DIR)/CirBuf.o
LIB_OBJS += $(OBJ_DIR)/LinkList.o
LIB_OBJS += $(OBJ_DIR)/LinkListOffset.o
LIB_OBJS += $(OBJ_DIR)/AdaptiveVoltageScaling.o
