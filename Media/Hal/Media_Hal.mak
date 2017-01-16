#------------------------------------------------------------------------------
# Include path
#------------------------------------------------------------------------------
INCLUDE_DIR += -IMedia/Hal

#------------------------------------------------------------------------------
# Object file list
#------------------------------------------------------------------------------
MEDIA_HAL_OBJS=

#------------------------------------------------------------------------------
# Include component make file
#------------------------------------------------------------------------------

# compile for 1093 only, no HAL code for others
include Media/Hal/$(_SOC_)/Media_Hal_$(_SOC_).mak           # build SoC specific MEDIA HAL
