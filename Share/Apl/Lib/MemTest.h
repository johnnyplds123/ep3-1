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
//! @brief  Header file for Memory test
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
// default memory test function
//#define MemTest(A, S, E, M)     MemTestRandU32((U32*)A, S, 0x12345678, E, M)

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief Memory test result
typedef struct
{
    U32 errorAddress;   ///< error address
    U32 writePattern;   ///< written pattern (orignal pattern)
    U32 readPattern;    ///< read back pattern (errored pattern)
} MemTestResult_t;

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
U32 MemTestAddrU32(U32* pMem, U32 testSize, U32 pattern, MemTestResult_t errLog[], U32 maxLog);
U32 MemTestRandU32(U32* pMem, U32 testSize, U32 pattern, MemTestResult_t errLog[], U32 maxLog);
U32 MemTestRandU32NoMsg(U32* pMem, U32 testSize, U32 seed);
U32 MemTestMarchU32(U32* pMem, U32 testSize, U32 pattern, MemTestResult_t errLog[], U32 maxLog);
U32 MemTestWrRdU32(U32* pMem, U32 testSize, U32 pattern, MemTestResult_t errLog[], U32 maxLog);
U32 MemTestRandMaskNoMsg(U32* pMem, U32 testSize, U32 seed, U32 mask);
/* End of File */

