#------------------------------------------------------------------------------
# Include path
#------------------------------------------------------------------------------
INCLUDE_DIR += -IHost/Apl/
INCLUDE_DIR += -IHost/Apl/NVME
INCLUDE_DIR += -IHost/Apl/NVME/Cmd
INCLUDE_DIR += -IHost/Apl/NVME/Cmd/Admin
INCLUDE_DIR += -IHost/Apl/NVME/Cmd/IO

#------------------------------------------------------------------------------
# Object file list
#------------------------------------------------------------------------------
include Host/Apl/$(_HOST_)/Cmd/IO/Host_Apl_NVME_Io.mak
include Host/Apl/$(_HOST_)/Cmd/Admin/Host_Apl_NVME_Admin.mak

HOST_APL_NVME_OBJS=
HOST_APL_NVME_OBJS += $(OBJ_DIR)/NvmeService.o
HOST_APL_NVME_OBJS += $(OBJ_DIR)/HldHost.o
HOST_APL_NVME_OBJS += $(OBJ_DIR)/HldNvme.o
HOST_APL_NVME_OBJS += $(OBJ_DIR)/NvmeCmds.o
HOST_APL_NVME_OBJS += $(OBJ_DIR)/NvmeCmdTable.o
HOST_APL_NVME_OBJS += $(HOST_APL_NVME_ADMIN_OBJS)
HOST_APL_NVME_OBJS += $(HOST_APL_NVME_IO_OBJS)


