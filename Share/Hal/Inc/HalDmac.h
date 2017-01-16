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
//! @brief Pure HAL API definition for DMA H/W control
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------

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
Error_t HalDmac_OneTimeInit(InitBootMode_t initMode);
Error_t HalDmac_WaitDone(void);

Error_t HalDmac_StartFill(uint32_t destAddr, uint32_t pattern, uint32_t byteCount);
Error_t HalDmac_StartCopy(uint32_t destAddr, uint32_t srcAddr, uint32_t byteCount);
Error_t HalDmac_StartCompare(uint32_t addr1, uint32_t addr2, uint32_t byteCount);
Error_t HalDmac_StartSearch(uint32_t addr, uint32_t pattern, uint32_t mask, uint32_t byteCount);
Error_t HalDmac_StartCount0(uint32_t srcAddr, uint32_t byteSize);

U16     HAL_MRE_GetRangeNo(U32 lba);
Error_t HAL_MRE_SecCopy(uint32_t destAddr, uint32_t srcAddr, uint32_t byteSize, uint32_t range);

