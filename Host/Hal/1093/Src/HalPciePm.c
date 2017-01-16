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
//! @brief PCIe power management event handler
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
// PKG
#include "BuildPkg.h"

// REG
#include "RegPcie.h"

// HAL
#include "HalPciePm.h"
#include "HalPcieLink.h"

// APL

// Code Section
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
//  Imported function prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    Request hanlder to manage PCIe power management event PME_TURN_OFF.\n

    @param[in]  void
**/
//-----------------------------------------------------------------------------
void HalPm_HandlePcieTurnOff(void)
{
}

//-----------------------------------------------------------------------------
/**
    Request hanlder to manage PCIe power management event Active State NAK.\n

    @param[in]  void
**/
//-----------------------------------------------------------------------------
void HalPm_HandlePcieAsNak(void)
{
    D_PRINTF("[PM_AS_NAK]\n");
}

//-----------------------------------------------------------------------------
/**
    Request hanlder to manage PCIe power management event PME.\n

    @param[in]  void
**/
//-----------------------------------------------------------------------------
void HalPm_HandlePciePME(void)
{
    D_PRINTF("[PME]\n");
}

//-----------------------------------------------------------------------------
/**
    Request hanlder to manage PCIe power management event PME_TO_ACK.\n

    @param[in]  void
**/
//-----------------------------------------------------------------------------
void HalPm_HandlePciePmeToAck(void)
{
    D_PRINTF("[PM_ACK_TO]\n");
}

void HalPm_EnablePmeDetect(void)
{
}

