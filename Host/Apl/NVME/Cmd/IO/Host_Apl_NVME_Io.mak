#------------------------------------------------------------------------------
# Include path
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Object file list
#------------------------------------------------------------------------------
HOST_APL_NVME_IO_OBJS=
HOST_APL_NVME_IO_OBJS += $(OBJ_DIR)/NvmeFlush.o
HOST_APL_NVME_IO_OBJS += $(OBJ_DIR)/NvmeRead.o
HOST_APL_NVME_IO_OBJS += $(OBJ_DIR)/NvmeWrite.o

ifneq ($(_MEDIA_),DRAM)
    HOST_APL_NVME_IO_OBJS += $(OBJ_DIR)/NvmeCompare.o
    HOST_APL_NVME_IO_OBJS += $(OBJ_DIR)/NvmeDatasetManagement.o
    HOST_APL_NVME_IO_OBJS += $(OBJ_DIR)/NvmeReservation.o
    HOST_APL_NVME_IO_OBJS += $(OBJ_DIR)/NvmeWriteUncorrectable.o
    HOST_APL_NVME_IO_OBJS += $(OBJ_DIR)/NvmeWriteZeroes.o
endif