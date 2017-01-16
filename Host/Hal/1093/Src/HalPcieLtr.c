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
//! \file
//! @brief Generic Host I/F High level driver API codes
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
// PKG
#include "BuildPkg.h"

// HAL
#include "HalPcieLink.h"

// APL

// Code Section
#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING

#if (HOST_IF_POWER_MANAGEMENT == FEATURE_SUPPORTED)

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
/// @brief LTR ID
typedef enum
{
    cLtrActive,                 ///< Command outstanding
    cLtrSemiActive,             ///< Command empty
    cLtrIdle,                   ///< Timer expired after command empty
    cLtrReserved,               ///< reserved for later
    cLtrEnd
} LtrId_t;


//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static Error_t SendLtrMessage(LtrId_t ltrId);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
static LtrId_t currentLtrId = cLtrActive;       // LTR ID of the latest sending
static uint32_t ltrIdleTimer = 0;               // LTR Idle timer, 1ms resolution timer

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/**
    Notification of command start

    @return nothing
**/
//-----------------------------------------------------------------------------
void HalPcieLtr_CommandActive(void)
{
#if (HOST_IF_POWER_MANAGEMENT == FEATURE_SUPPORTED)
    // Send Active LTR
    SendLtrMessage(cLtrActive);

    ltrIdleTimer = 0;
#endif
}

//-----------------------------------------------------------------------------
/**
    Command status polling. This function is called every 1msec

    @return nothing
**/
//-----------------------------------------------------------------------------
void HalPcieLtr_IdleStatusPolling(bool isIdle)
{
    if (isIdle)             // NVMe command idle
    {
        ltrIdleTimer++;

        if (ltrIdleTimer == 2)      // 2ms LTR for SemiActive
        {
            SendLtrMessage(cLtrSemiActive);
        }
        else if (ltrIdleTimer == 5) // 5ms LTR for Idle
        {
            SendLtrMessage(cLtrIdle);
        }
    }
    else
    {
        ltrIdleTimer = 0;
    }
}

//-----------------------------------------------------------------------------
/**
    Notify if the latest LTR is IdleLTR

    @return true if the latest LTR is IdleLTR
**/
//-----------------------------------------------------------------------------
bool HalPcieLtr_IsLtrIdle(void)
{
    return (currentLtrId == cLtrIdle);
}

//-----------------------------------------------------------------------------
/**
    Change LTR value

    @param[in]  LtrId
**/
//-----------------------------------------------------------------------------
static Error_t SendLtrMessage(LtrId_t ltrId)
{
    LtrVectorSel_t ltrVectorId;

    // exit if Ltr Id is not changed.
    if (currentLtrId == ltrId)  return cEcNoError;

    switch(ltrId)
    {
        case cLtrActive:        ltrVectorId = cLtrVector0; break;
        case cLtrSemiActive:    ltrVectorId = cLtrVector3; break;
        case cLtrIdle:          ltrVectorId = cLtrVector1; break;
        default:
            return cEcError;
    }

    currentLtrId = ltrId;
    HalPcieLink_RequestToSendLtrMessage(ltrVectorId);

    return cEcNoError;
}
#endif
