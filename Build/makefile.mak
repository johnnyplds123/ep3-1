#  ============================================================================
#
#      Copyright (c) Marvell Corporation 2000-2012  -  All rights reserved
#
#   This computer program contains confidential and proprietary information,
#   and  may NOT  be reproduced or transmitted, in whole or in part,  in any
#   form,  or by any means electronic, mechanical, photo-optical, or  other-
#   wise, and  may NOT  be translated  into  another  language  without  the
#   express written permission from Marvell Corporation.
#
#  ============================================================================
#  =      C O M P A N Y      P R O P R I E T A R Y      M A T E R I A L       =
#  ============================================================================

#------------------------------------------------------------------------------
#  BUILD TOOLs Path (3rd party tools)
#------------------------------------------------------------------------------
COMPILE         = armcc
ASSEMBLE        = armasm
LINK            = armlink
ARCHIVE         = armar
FROMELF         = fromelf

#------------------------------------------------------------------------------
# BUILD parameter
#------------------------------------------------------------------------------
CPU_ID          = cpu$(_CPU_)
ifeq ($(_CODE_TYPE_), CODE_BOOT)
	MAKE_TARGET = boot
else
	MAKE_TARGET = $(CPU_ID)
endif

#------------------------------------------------------------------------------
# BUILD input files
#------------------------------------------------------------------------------
BUILD_DIR       = Build
MAKE_FILE       = $(BUILD_DIR)/$(MAKE_TARGET)make.mak
SF_FILE         = $(BUILD_DIR)/$(_SOC_)/$(MAKE_TARGET)link.sf

#------------------------------------------------------------------------------
# Output directory
#------------------------------------------------------------------------------
OUT_DIR         = $(_OUT_)
OBJ_DIR         = $(OUT_DIR)/obj_$(MAKE_TARGET)
BIN_DIR         = $(OUT_DIR)/bin

DOS_OUT_DIR     = $(subst /,\\,$(OUT_DIR))
DOS_OBJ_DIR     = $(subst /,\\,$(OBJ_DIR))
DOS_BIN_DIR     = $(subst /,\\,$(BIN_DIR))

#------------------------------------------------------------------------------
# Output files
#------------------------------------------------------------------------------
OUT_AXF         = $(BIN_DIR)/$(MAKE_TARGET).axf
OUT_LIB         = $(BIN_DIR)/$(MAKE_TARGET).a
OUT_SYM         = $(BIN_DIR)/$(MAKE_TARGET).txt
OUT_LST         = $(BIN_DIR)/$(MAKE_TARGET).lst
OUT_HEX         = $(BIN_DIR)/$(MAKE_TARGET).hex

#------------------------------------------------------------------------------
# Intermediate output files
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Build Target
#------------------------------------------------------------------------------
all: banner creat_dir $(OUT_AXF) $(OUT_HEX)

#------------------------------------------------------------------------------
# Build Banner
#------------------------------------------------------------------------------
banner:
	$(info --------------------------------------------------------------------------------)
	$(info [BUILD] - $(MAKE_FILE) SOC=$(_SOC_) REV=$(_SOC_REV_) CPU=$(_CPU_) $(_CODE_TYPE_) $(_FW_TYPE_))
ifeq ($(_MEDIA_),NAND)
	$(info [BUILD] - $(_HOST_).$(_MEDIA_).$(_DRAM_) $(_VENDOR_):$(_NAND_) TCG=$(_TCG_) $(_CUSTOMER_) $(_FW_REV_) $(_GENERATION_) OP=$(_OP_))
else
	$(info [BUILD] - $(_HOST_).$(_MEDIA_).$(_DRAM_) TCG=$(_TCG_) $(_CUSTOMER_) $(_FW_REV_) $(_GENERATION_) OP=$(_OP_))
endif
	$(info --------------------------------------------------------------------------------)

#------------------------------------------------------------------------------
# Create directories
#------------------------------------------------------------------------------
creat_dir: $(BIN_DIR) $(OBJ_DIR)

$(BIN_DIR):
ifeq ($(OS),Windows_NT)
	@mkdir $(DOS_BIN_DIR)
else
	@mkdir $(BIN_DIR)
endif

$(OBJ_DIR):
ifeq ($(OS),Windows_NT)
	@mkdir $(DOS_OBJ_DIR)
else
	@mkdir $(OBJ_DIR)
endif

#------------------------------------------------------------------------------
# Include path for C Compiler
#------------------------------------------------------------------------------
INCLUDE_DIR = -IBuild/

#------------------------------------------------------------------------------
# Build objects
#------------------------------------------------------------------------------
BUILD_OBJS  =

#------------------------------------------------------------------------------
# Include component make file
#------------------------------------------------------------------------------
include $(MAKE_FILE)

#------------------------------------------------------------------------------
# Dependency list
#------------------------------------------------------------------------------
-include ${BUILD_OBJS:.o=.d}

#------------------------------------------------------------------------------
# source search path for MAKE tool
#------------------------------------------------------------------------------
vpath %.h $(subst / ,:,$(subst -I,,$(INCLUDE_DIR)))
vpath %.c $(subst / ,:,$(subst -I,,$(INCLUDE_DIR)))

#------------------------------------------------------------------------------
# C Compiler option flags
#------------------------------------------------------------------------------
C_DEFINES   = -D_CPU_=$(_CPU_) \
              -D_SOC_=$(_SOC_) \
              -D_SOC_REV_=$(_SOC_REV_) \
              -D_SHARE_=SHARE_$(_SHARE_) \
              -D_HOST_=HOST_$(_HOST_) \
              -D_MEDIA_=MEDIA_$(_MEDIA_) \
              -D_CODE_TYPE_=$(_CODE_TYPE_) \
              -D_FW_TYPE_=$(_FW_TYPE_)  \
              -D_TCG_=TCG_$(_TCG_) \
              -D_OPROM_=OPROM_$(_OPROM_) \
              -D_NAND_=NAND_$(_NAND_) \
              -D_VENDOR_=VENDOR_$(_VENDOR_) \
              -D_DRAM_=DRAM_$(_DRAM_) \
              -D_DRAM_SIZE_=$(_DRAM_SIZE_) \
              -D_GENERATION_=GENERATION_$(_GENERATION_) \
              -D_CUSTOMER_=CUSTOMER_$(_CUSTOMER_) \
              -D_OP_=OP_$(_OP_) \
              -D_FW_REV_="\"$(_FW_REV_)"\" \
              -D_FW_REV_SUB_="\"$(_FW_REV_SUB_)"\" \
              -D_MODEL_NAME_="\"$(_MODEL_NAME_)"\" \
              -D_SRC_REV_="\"$(_SRC_REV_)"\" \
              -D_BLD_STR_="\"$(_HOST_).$(_MEDIA_).$(_VENDOR_).$(_NAND_).$(_DRAM_).$(_GENERATION_).$(_CUSTOMER_).$(_FW_TYPE)"\"

C_INCLUDE   = $(INCLUDE_DIR)

C_ALIGN     = --pointer_alignment=4 --min_array_alignment=8
C_OPTIMIZE  = --asm --interleave -c -O3 --no_inlinemax --bss_threshold=0 \
              --apcs /interwork --unsigned_chars --forceinline --C99 --remarks

C_DEBUG     = -g

C_WARNING   = --diag_suppress 177,1764,1786,2530,1301,1297,2815,826,3142\
              --diag_error    193         #3017

C_DEP       = --no_depend_system_headers --depend=${OBJ_DIR}/${notdir ${@:.o=.d}}

C_FLAGS     = $(C_OPTIMIZE) $(C_DEBUG) $(C_INCLUDE) $(C_DEFINES) $(C_WARNING) \
              $(C_ALIGN) $(C_CORE) $(C_DEP)

#------------------------------------------------------------------------------
# Linker Option flags
#------------------------------------------------------------------------------
ifeq ($(_CODE_TYPE_), CODE_BOOT)
	LINK_INFO   = --entry $(_BOOT_ADDR_) --map --symbols  --xref \
	              --xref --info sizes --info totals --info unused --callgraph
else
	LINK_INFO   = --entry 0x0 --map --symbols  --xref \
	              --xref --info sizes --info totals --info unused --callgraph
endif

LINK_DIAG   = --diag_suppress L6314W,L6329W

LINK_SYM    = --symdefs $(OUT_SYM)
LINK_LST    = --list $(OUT_LST)

LINK_FLAGS  = $(LINK_INFO) $(LINK_DIAG) $(C_CORE) $(LINK_SYM) $(LINK_LST)

#------------------------------------------------------------------------------
# Link object file and make target axf
#------------------------------------------------------------------------------
$(OUT_AXF) : $(BUILD_OBJS) $(SF_FILE) $(MAKE_FILE) $(BLD_LIBS)
	$(info Linking objects and create $(OUT_AXF))
	@$(LINK) $(BUILD_OBJS) $(BLD_LIBS) $(LINK_FLAGS) --scatter $(SF_FILE) -o $(OUT_AXF)

#------------------------------------------------------------------------------
# Extract binary code image from axf
#------------------------------------------------------------------------------
$(OUT_HEX) : $(OUT_AXF)
	@$(FROMELF) --i32combined $(OUT_AXF) --output $(OUT_HEX)

#------------------------------------------------------------------------------
# Compile c source
#------------------------------------------------------------------------------
$(OBJ_DIR)/%.o  : %.c
ifeq ($(_MUTE_), DISABLE)
	$(info Compiling - $<)
endif
	@$(COMPILE) $(C_FLAGS) $< -o $@
