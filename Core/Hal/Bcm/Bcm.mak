#------------------------------------------------------------------------------
# Include path
#------------------------------------------------------------------------------
INCLUDE_DIR += -ICore/Hal/Bcm

#------------------------------------------------------------------------------
# Object file list
#------------------------------------------------------------------------------
BCM_OBJS=

BCM_OBJS += $(OBJ_DIR)/BcmClientMain.o
BCM_OBJS += $(OBJ_DIR)/BcmClientFunctions.o
BCM_OBJS += $(OBJ_DIR)/BcmClientUtilities.o
BCM_OBJS += $(OBJ_DIR)/BcmClientMaes.o

