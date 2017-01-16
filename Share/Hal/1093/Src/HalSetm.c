// ============================================================================
//
//     Copyright (c) Marvell Corporation 2000-2015  -  All rights reserved
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
//! @brief SETM I/F code for Eldora Soc
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"
#include "RegSetm3Phy.h"
#include "HalSetm.h"


#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

#if _CPU_==CPU_ID0    // CPU0 only
//-----------------------------------------------------------------------------
/**
    H/W block level power management API

    @param[in]  powerMode        targeting pm mode to enter
    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalSetm_SetPowerMode(SetmPowerMode_t powerMode)
{
    PowerAndPllCtrlRegister_t powerAndPllCtrlRegister = rSetm3Phy.powerAndPllCtrlRegister;

    switch(powerMode)
    {
    case cSetmPmDeepSleep:
        powerAndPllCtrlRegister.b.PU_TX_B  = 0;
        powerAndPllCtrlRegister.b.PU_TX_A  = 0;
        powerAndPllCtrlRegister.b.PU_PLL   = 0;
        powerAndPllCtrlRegister.b.PU_IVREF = 0;
        break;
    case cSetmPmActive:
        powerAndPllCtrlRegister.b.PU_TX_B  = 1;
        powerAndPllCtrlRegister.b.PU_TX_A  = 1;
        powerAndPllCtrlRegister.b.PU_PLL   = 1;
        powerAndPllCtrlRegister.b.PU_IVREF = 1;
        break;
    default:
        break;
    }

    rSetm3Phy.powerAndPllCtrlRegister = powerAndPllCtrlRegister;

    return cEcNoError;
}
#endif


