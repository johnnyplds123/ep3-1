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
//! \file
//! @brief API codes for getting LDPC encoding and decoding schedulers
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "RegEcu.h"

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define NUMBER_OF_LDPC_CORE         (1)         ///<- Up to hardware spec.
#define SIZE_TABLE_COL              (2)         ///< ITR scheduler size table: the # of columns

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

/// @brief LDPC Scheduler Buffer manager.
typedef struct
{
    uint8_t  pSchName[5];                        ///< The name of Scheduler.
    uint16_t nEnable;                            ///< Enable or Disable to download.
    uint32_t pEncBuf;                            ///< Encoder Scheduler buffer pointer.
    uint32_t nEncSize;                           ///< Encoder Scheduler buffer size.
    uint32_t pDecBuf;                            ///< Decoder Scheduler buffer pointer.
    uint32_t nDecSize;                           ///< Decoder Scheduler buffer size.
    uint16_t nParitySize;                        ///< Parity size
} tLDPC_BUFFER_MANAGER;
COMPILE_ASSERT((sizeof(tLDPC_BUFFER_MANAGER) & 0x3) == 0, "please align to 4 bytes!");

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern  tLDPC_BUFFER_MANAGER gLdpcSchManager[NUMBER_OF_LDPC_CORE];

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t HalEcu_DownloadLdpc(void);

