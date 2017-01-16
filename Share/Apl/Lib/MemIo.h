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
//! @brief  Header file for Memory I/O
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define MEM_SET(dest, ch, n)            memset(dest, ch, n)
#define MEM_CLR(dest, n)                MEM_SET(dest, 0, n)
#define MEM_COPY(dest, src, n)          memcpy(dest, src, n)
#define MEM_CMP(src1, src2, n)          memcmp(src1, src2, n)

#define RANDOM(R)                       (rand()%(R))        ///< get random number beteen 0 to R-1
#define SRAND(S)                        (srand(S))          ///< set random seed value

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
uint32_t    MemReadU8(uint32_t address);
uint32_t    MemReadU16(uint32_t address);
uint32_t    MemReadU32(uint32_t address);
void        MemWriteU8(uint32_t address, uint32_t data);
void        MemWriteU16(uint32_t address, uint32_t data);
void        MemWriteU32(uint32_t address, uint32_t data);

void*       MemFillU32(void* pDest, uint32_t data, uint32_t byteSize);
void*       MemFillU32Inc(void* pDest, uint32_t data, uint32_t byteSize, uint32_t delta);

uint32_t    MemSumU8(const uint8_t* pBuffer, uint32_t byteSize);
void        MemSwapU16(uint16_t* pDestData, const uint16_t* pSrcData, uint32_t wordSize);
void        MemSwapCopy(void* pDest, void* pSrc, uint32_t byteSize);
void        MemCopyU32(uint32_t* pDest, uint32_t* pSrc, uint32_t byteSize);

void*       MemCopy2U32(uint32_t* pDest, uint32_t* pSrc);
void*       MemCopy3U32(uint32_t* pDest, uint32_t* pSrc);
void*       MemCopy4U32(uint32_t* pDest, uint32_t* pSrc);
void*       MemCopy5U32(uint32_t* pDest, uint32_t* pSrc);
void*       MemCopy6U32(uint32_t* pDest, uint32_t* pSrc);
void*       MemCopy7U32(uint32_t* pDest, uint32_t* pSrc);
void*       MemCopy8U32(uint32_t* pDest, uint32_t* pSrc);
void*       MemCopy9U32(uint32_t* pDest, uint32_t* pSrc);
void*       MemCopy10U32(uint32_t* pDest, uint32_t* pSrc);
void*       MemCopy16U32(uint32_t* pDest, uint32_t* pSrc);
void*       MemCopy32U32(uint32_t* pDest, uint32_t* pSrc);

#if CHK_CODE_TYPE(CODE_MAIN)
    PVOID MemCopyN8U32(U32* pDest, U32* pSrc, U32 bytes);
    PVOID MemCopyNU32(U32* pDest, U32* pSrc, U32 bytes);
#endif

U32   MemCrcU32(U32 crc, const void* data, U32 len);
U16   CRC_Calculate(U16 crc, const void* data, U16 len);
