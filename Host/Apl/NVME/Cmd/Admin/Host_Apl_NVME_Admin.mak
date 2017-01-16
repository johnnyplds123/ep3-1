#------------------------------------------------------------------------------
# Include path
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Object file list
#------------------------------------------------------------------------------
HOST_APL_NVME_ADMIN_OBJS=
HOST_APL_NVME_ADMIN_OBJS += $(OBJ_DIR)/NvmeAbort.o
HOST_APL_NVME_ADMIN_OBJS += $(OBJ_DIR)/NvmeAsynchronousEventRequest.o
HOST_APL_NVME_ADMIN_OBJS += $(OBJ_DIR)/NvmeCreateIoCompletionQueue.o
HOST_APL_NVME_ADMIN_OBJS += $(OBJ_DIR)/NvmeCreateIoSubmissionQueue.o
HOST_APL_NVME_ADMIN_OBJS += $(OBJ_DIR)/NvmeDeleteIoCompletionQueue.o
HOST_APL_NVME_ADMIN_OBJS += $(OBJ_DIR)/NvmeDeleteIoSubmissionQueue.o
HOST_APL_NVME_ADMIN_OBJS += $(OBJ_DIR)/NvmeGetFeatures.o
HOST_APL_NVME_ADMIN_OBJS += $(OBJ_DIR)/NvmeGetLogPage.o
HOST_APL_NVME_ADMIN_OBJS += $(OBJ_DIR)/NvmeIdentify.o
HOST_APL_NVME_ADMIN_OBJS += $(OBJ_DIR)/NvmeSetFeatures.o
HOST_APL_NVME_ADMIN_OBJS += $(OBJ_DIR)/NvmeDirective.o
HOST_APL_NVME_ADMIN_OBJS += $(OBJ_DIR)/NvmeNamespaceManagment.o
ifneq ($(_MEDIA_),DRAM)
    HOST_APL_NVME_ADMIN_OBJS += $(OBJ_DIR)/NvmeFirmwareActivate.o
    HOST_APL_NVME_ADMIN_OBJS += $(OBJ_DIR)/NvmeFirmwareImageDownload.o
    HOST_APL_NVME_ADMIN_OBJS += $(OBJ_DIR)/NvmeSecurityReceive.o
    HOST_APL_NVME_ADMIN_OBJS += $(OBJ_DIR)/NvmeSecuritySend.o
    HOST_APL_NVME_ADMIN_OBJS += $(OBJ_DIR)/NvmeFormat.o
    HOST_APL_NVME_ADMIN_OBJS += $(OBJ_DIR)/NvmeVendorCmd.o
    HOST_APL_NVME_ADMIN_OBJS += $(OBJ_DIR)/NvmeDeviceSelfTest.o
else
    ifeq ($(_FW_TYPE_),FW_DNLD)
        HOST_APL_NVME_ADMIN_OBJS += $(OBJ_DIR)/NvmeFirmwareActivate.o
        HOST_APL_NVME_ADMIN_OBJS += $(OBJ_DIR)/NvmeFirmwareImageDownload.o
    endif
endif
