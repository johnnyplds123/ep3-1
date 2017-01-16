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
//! @brief 1093 register declaration
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------

#include "BuildPkg.h"

#include "RegDge.h"
#include "RegEcu.h"
#include "RegFlashLlc.h"
#include "RegHwReread.h"
#include "RegNandFlashIf.h"
#include "RegSequencer.h"

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define __AT(X) __attribute__((at(X))) __attribute__((zero_init)) ///< locate at specified address
#define __AS(X) __attribute__((alias(X)))                         ///< name, type alias

//-----------------------------------------------------------------------------
//  Register declaration: Private or Public
//-----------------------------------------------------------------------------

/// @brief SoC register declaration

// Nand Flash Controller Register Access through APB (0xF200B800)
volatile FlashLlc_t         rFlashLlc           __AT(0xF200B800);
volatile Fct_t              rFct                __AT(0xF200BA00);
volatile Ecu_t              rEcu                __AT(0xF200C000);
volatile Seq_t              rSeq                __AT(0xF200D000);
volatile NandFlashIf_t      rNandFlashIf        __AT(0xF200E000);
volatile Dge_t              rDge                __AT(0xF200E800);
//volatile Alu_t              rAlu                __AT(0x1C00E900);     ///< Currently it's in RegShare.c
volatile HwReread_t         rHwReread           __AT(0xF200FC00);

/// @brief non volatile register declaration for optimization
extern Dge_t                mDge                __AS("rDge");
extern Ecu_t                mEcu                __AS("rEcu");
extern FlashLlc_t           mFlashLlc           __AS("rFlashLlc");
extern HwReread_t           mHwReread           __AS("rHwReread");
extern NandFlashIf_t        mNandFlashIf        __AS("rNandFlashIf");
extern Seq_t                mSeq                __AS("rSeq");
extern Fct_t                mFct                __AS("rFct");

