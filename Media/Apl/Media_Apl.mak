#------------------------------------------------------------------------------
# Include path
#------------------------------------------------------------------------------
INCLUDE_DIR  += -IMedia/Apl

#------------------------------------------------------------------------------
# Object file list
#------------------------------------------------------------------------------
MEDIA_APL_OBJS =

MEDIA_APL_OBJS += $(OBJ_DIR)/Media.o
MEDIA_APL_OBJS += $(OBJ_DIR)/MediaMon.o

#------------------------------------------------------------------------------
# Include component make file
#------------------------------------------------------------------------------
include Media/Apl/Descriptor/AplDesc.mak          # build apl descriptor
include Media/Apl/NandFlashInterface/AplNfif.mak  # build apl nand flash interface
include Media/Apl/Scheduler/AplSch.mak            # build apl scheduler
include Media/Apl/Sequencer/AplSeq.mak            # build apl sequencer
include Media/Apl/ReadRetry/AplReadRetry.mak      # build apl readretry
include Media/Apl/NandPackage/AplNandPkg.mak      # build Media application layer component

MEDIA_APL_OBJS += $(MEDIA_APL_NFIF_OBJS)
MEDIA_APL_OBJS += $(MEDIA_APL_DESC_OBJS)
MEDIA_APL_OBJS += $(MEDIA_APL_SCH_OBJS)
MEDIA_APL_OBJS += $(MEDIA_APL_SEQ_OBJS)
MEDIA_APL_OBJS += $(MEDIA_APL_READRETRY_OBJS)
MEDIA_APL_OBJS += $(MEDIA_APL_NANDPKG_OBJS)

