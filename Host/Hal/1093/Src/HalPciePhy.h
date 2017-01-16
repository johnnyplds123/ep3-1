#pragma once
// ============================================================================
//
//     Copyright (c) Marvell Corporation 2000-2012  -  All rights reserved
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
//! @file
//! @brief Pure HAL API definition for Host Interface Phy layer HW control
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Compile options
//-----------------------------------------------------------------------------
#define CO_RESET_PCIE_PHY_ON_PERST      (DISABLE && (_SOC_REV_>=0x0210)) // this option is only applicable to R2.1 or above
#define CO_REBOOT_DEVICE_ON_PERST       (DISABLE && (_SOC_REV_>=0x0210)) // this option is only applicable to R2.1 or above
#define PCIE_FINE_TUNE_ACERR14          TRUE

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
/// @brief PCIePHY power mode definition
typedef enum
{
    cPciePhyPmActive=1,          ///< active mode - highest power consumption state
    cPciePhyPmDeepSleep=15       ///< sleep mode - lowest power consumption state
} PciePhyPowerMode_t;

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

#define TRY4_SETTING        TRUE
#define TX_AMP              FALSE
//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t HalPhy_OneTimeInit(InitBootMode_t initMode);
void HalPhy_Reset(ResetMode_t resetMode);
Error_t HalPhy_WaitForPerstRelease(uint32_t timeOutUsec);
void HalPhy_SetPowerMode(PciePhyPowerMode_t mode);

