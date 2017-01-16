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
//! @brief System level pure HAL API definition
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
#define REBOOT_FLAG             0xAABBCCDD
//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern volatile U32 smInitBootMode;

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
InitBootMode_t Hal_GetInitBootMode(void);
Error_t Hal_PreOneTimeInit(InitBootMode_t initMode);
Error_t Hal_OneTimeInit(InitBootMode_t initMode);
uint32_t Hal_GetRevision(void);
void Hal_DumpInfo(void);
