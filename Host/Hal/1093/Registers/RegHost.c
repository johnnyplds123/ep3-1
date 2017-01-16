//============================================================================
//
//     Copyright (c) Marvell Corporation 2000-2013  -  All rights reserved
//
//  This computer program contains confidential and proprietary information,
//  and  may NOT  be reproduced or transmitted, in whole or in part,  in any
//  form,  or by any means electronic, mechanical, photo-optical, or  other-
//  wise, and  may NOT  be translated  into  another  language  without  the
//  express written permission from Marvell Corporation.
//
// ============================================================================
// =      C O M P A N Y      P R O P R I E T A R Y      M A T E R I A L       =
// ============================================================================
//
//! \file
//! @brief 9293 register declaration
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
// PKG
#include "BuildPkg.h"
#include "RegNvmePkg.h"

// REG
#include "RegPcie.h"
#include "RegPcieShadow.h"
#include "RegPcieCore.h"
#include "RegPciePhy.h"

// HAL

// APL

// Code Section

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define __AT(X) __attribute__((at(X))) __attribute__((zero_init)) ///< locate at specified address
#define __AS(X) __attribute__((alias(X)))                         ///< name, type alias

//-----------------------------------------------------------------------------
//  Register declaration: Private or Public
//-----------------------------------------------------------------------------

/// @brief SoC Host register declaration

// PCIe Register Access through APB (0xF000000)
volatile PciePf_t               rPciePf                                 __AT(0xF0000000);   // 32bit accessible only
volatile PcieShadow_t           rPcieShadow                             __AT(0xF0020010);   // 32bit accessible only
volatile PcieCore_t             rPcieCore                               __AT(0xF0080000);   // 32bit accessible only
volatile PciePhy_t              rPciePhy                                __AT(0xF00C0000);   // 32bit accessible only

// NVME Register Access through APB (0xF5000000)
volatile Nvme_t                 rNvme                                   __AT(0xF5000000);   // 32bit accessible only
volatile NvmeControllerSram_t   rNvmeController                         __AT(0xF5000180);   // 32bit accessible only
volatile NvmeCmdFetch_t         rNvmeCmdFetch                           __AT(0xF5010000);   // 32bit accessible only
volatile NvmeDataMngr_t         rNvmeDataMngr                           __AT(0xF5011000);   // 32bit accessible only
volatile NvmeHwll_t             rNvmeHwll                               __AT(0xF5012000);   // 32bit accessible only
volatile NvmePktMngr_t          rNvmePktMngr                            __AT(0xF5013000);   // 32bit accessible only
volatile NvmeQueueArbiter_t     rNvmeQueueArbiter                       __AT(0xF5014000);   // 32bit accessible only
volatile NvmeStatMngr_t         rNvmeStatMngr                           __AT(0xF5015000);   // 32bit accessible only
volatile NvmeBitmapSram_t       rNvmeBitmapSram                         __AT(0xF5016000);   // 32bit accessible only
volatile NvmeCmdSram_t          rNvmeCmdSram                            __AT(0xF5017000);   // 32bit accessible only
volatile NvmeCentralCtrl_t      rNvmeCentralCtrl                        __AT(0xF5018000);   // 32bit accessible only

// NVME SRAM Access through AHB (0xF5400000)
volatile NvmeFwIoCmd_t          rNvmeFwIoCmd[8]                         __AT(0xF5400000);   // 32bit accessible only
volatile NvmeFwAdmCmd_t         rNvmeFwAdmCmd[8]                        __AT(0xF5400800);   // 32bit accessible only
volatile NvmeHwIoCmd_t          rNvmeHwIoCmd[HW_MAX_IO_CMD_INDEX]       __AT(0xF5401000);   // 32bit accessible only
volatile NvmeHwAdmCmd_t         rNvmeHwAdmCmd[HW_MAX_ADMIN_CMD_INDEX]   __AT(0xF5409000);   // 32bit accessible only

volatile NvmeBitmapSramData_t   rNvmeBitmap                             __AT(0xF541E000);   // 32bit accessible only
volatile NvmeBitmapSramData_t   rNvmeBitmapW1s                          __AT(0xF541E400);   // Write 1 to Set, 32bit accessible only
volatile NvmeBitmapSramData_t   rNvmeBitmapW1c                          __AT(0xF541E800);   // Write 1 to Clear, 32bit accessible only

/// @brief non volatile register declaration for optimization

