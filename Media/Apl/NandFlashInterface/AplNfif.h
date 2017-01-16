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
//! @brief NAND Flash I/O High Level Driver API definition
//!
//=============================================================================
//-----------------------------------------------------------------------------
//  include header files:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
#define NUM_OF_INIT_FUNC        8

typedef enum {
    NFIF_INIT_COLDBOOT = 0,
    NFIF_INIT_WAKEUP,
    NFIF_INIT_MODE_COUNT
}NfIfInitMode_t;

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
Error_t AplNfif_ResetTargets(uint32_t devicesPerCh, uint32_t numChannels, uint32_t command);
Error_t AplNfif_OneTimeInit(InitBootMode_t initMode);
void    AplNfif_DumpInfo(void);

//-----------------------------------------------------------------------------
//  Inline Function
//-----------------------------------------------------------------------------
