#------------------------------------------------------------------------------
# Include path
#------------------------------------------------------------------------------
INCLUDE_DIR += -Ihost/Hal
INCLUDE_DIR += -Ihost/Hal/Inc

#------------------------------------------------------------------------------
# Object file list
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# Include component make file
#------------------------------------------------------------------------------
include Host/Hal/$(_SOC_)/Host_Hal_$(_SOC_).mak           # build SoC specific HOST HAL

