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
INCLUDE_DIR  += -IShare/Apl/Inc
INCLUDE_DIR  += -IShare/Apl/Tcg
INCLUDE_DIR  += -IShare/Apl/Boot
INCLUDE_DIR  += -IShare/Apl/CmdSensor
INCLUDE_DIR  += -IShare/Apl/NamingRule
INCLUDE_DIR  += -IShare/Apl/Lib
INCLUDE_DIR  += -IShare/Apl/Debug
INCLUDE_DIR  += -IShare/Apl/Kernel
INCLUDE_DIR  += -IShare/Apl/Ipc
INCLUDE_DIR  += -IShare/Apl/Monitor
INCLUDE_DIR  += -IShare/Apl/Utility
INCLUDE_DIR  += -IShare/Hal
INCLUDE_DIR  += -IShare/Hal/Inc
INCLUDE_DIR  += -IShare/Hal/$(_SOC_)/Registers
INCLUDE_DIR  += -IShare/Hal/$(_SOC_)/Src
INCLUDE_DIR  += -IShare/Hal/$(_SOC_)

INCLUDE_DIR  += -IHost/Hal/Inc
INCLUDE_DIR  += -IHost/Hal/$(_SOC_)
INCLUDE_DIR  += -IHost/Hal/$(_SOC_)/Src
INCLUDE_DIR  += -IHost/Hal/$(_SOC_)/Src/$(_HOST_)
INCLUDE_DIR  += -IHost/Hal/$(_SOC_)/Registers
INCLUDE_DIR  += -IHost/Apl/$(_HOST_)
INCLUDE_DIR  += -IHost/Apl/

INCLUDE_DIR  += -IMedia/Hal/Includes
INCLUDE_DIR  += -IMedia/Hal/$(_SOC_)/Src

INCLUDE_DIR  += -ICore/Apl/Fio

#------------------------------------------------------------------------------
# Include component make file
#------------------------------------------------------------------------------
BOOT_OBJS  =
BOOT_OBJS += $(OBJ_DIR)/Boot.o
BOOT_OBJS += $(OBJ_DIR)/LoadFirmware.o
BOOT_OBJS += $(OBJ_DIR)/CharIo.o
BOOT_OBJS += $(OBJ_DIR)/MemIo.o
BOOT_OBJS += $(OBJ_DIR)/MemTest.o
BOOT_OBJS += $(OBJ_DIR)/SharedVars.o
BOOT_OBJS += $(OBJ_DIR)/Debug.o
BOOT_OBJS += $(OBJ_DIR)/DebugException.o
BOOT_OBJS += $(OBJ_DIR)/HalRegConfig.o
BOOT_OBJS += $(OBJ_DIR)/RegShare.o
BOOT_OBJS += $(OBJ_DIR)/HalSys.o
BOOT_OBJS += $(OBJ_DIR)/HalClock.o
BOOT_OBJS += $(OBJ_DIR)/HalCpu.o
BOOT_OBJS += $(OBJ_DIR)/HalUart.o
BOOT_OBJS += $(OBJ_DIR)/HalTimer.o
BOOT_OBJS += $(OBJ_DIR)/HalGpio.o
BOOT_OBJS += $(OBJ_DIR)/HalDram.o
BOOT_OBJS += $(OBJ_DIR)/HalMc5DdrInit.o
BOOT_OBJS += $(OBJ_DIR)/HalDdrDll.o
BOOT_OBJS += $(OBJ_DIR)/HalSflash.o
BOOT_OBJS += $(OBJ_DIR)/RegHost.o
BOOT_OBJS += $(OBJ_DIR)/HalPciePhy.o
BOOT_OBJS += $(OBJ_DIR)/HalPcieLink.o
BUILD_OBJS += $(BOOT_OBJS)
