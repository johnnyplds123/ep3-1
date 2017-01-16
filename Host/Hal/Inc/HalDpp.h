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
//! @brief Pure HAL API definition for DATA Path Protection (BCRC, MPECC and so on)
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
/// @brief DRAM mode definition
typedef enum
{
    cDppEccCombinedMode=0,         ///< Combined ECC mode (ECC are following the data field)
    cDppEccSeperatedMode,          ///< Seperated ECC mode (ECC are stored seperated location)
} DppEccStorageMode_t;

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t HalDpp_OneTimeInit(InitBootMode_t initMode);

void HalDpp_SetEccStorageMode(DppEccStorageMode_t mode);

void   HalDpp_SetEccBasePointer(void* pDataBuffer, void* pEccBuffer);
void*  HalDpp_SetProtectionRegion(void* pDataBuffer, uint32_t size);

Error_t HalDpp_SetHlbaSize(uint32_t hlbaSize);

bool HalDpp_EnDisableHostLba(bool enDisable);
bool HalDpp_EnDisableHostBcrc(bool enDisable);
bool HalDpp_EnDisableHostMpecc(bool enDisable);

