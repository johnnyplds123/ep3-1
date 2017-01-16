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
//! @brief  Header file for Circular buffer management
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

/// @brief circular buffer structure
typedef struct
{
    uint8_t* pBegin;         ///< buffer begin
    uint8_t* pEnd;           ///< buffer end
    uint32_t numEntries;     ///< number of entries
    uint32_t entrySize;      ///< size per entry
    uint8_t* pRead;          ///< read pointer
    uint8_t* pWrite;         ///< write pointer
    volatile uint32_t readCount;      ///< read count
    volatile uint32_t writeCount;     ///< write count
} CirBuf_t;

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
void CbInit(CirBuf_t* pCb, void* pBuffer, uint32_t numEntries, uint32_t entrySize);
void CbReset(CirBuf_t* pCb);

bool CbIsEmpty(CirBuf_t* pCb);
bool CbIsFull(CirBuf_t* pCb);

Error_t CbWrite(CirBuf_t* pCb, void* pData);
Error_t CbRead (CirBuf_t* pCb, void* pData, bool remove);

Error_t CbWriteU64(CirBuf_t* pCb, void* pData);
Error_t CbReadU64 (CirBuf_t* pCb, void* pData, bool remove);

Error_t CbWriteU32(CirBuf_t* pCb, uint32_t data);
Error_t CbReadU32 (CirBuf_t* pCb, void* pData, bool remove);

Error_t CbWriteU16(CirBuf_t* pCb, uint16_t data);
Error_t CbReadU16 (CirBuf_t* pCb, void* pData, bool remove);

Error_t CbIncReadPointer(CirBuf_t* pCb);
Error_t CbIncWritePointer(CirBuf_t* pCb);

void* CbGetPointer(CirBuf_t* pCb, int32_t offset, uint8_t* pBase);

Error_t CbMoveWritePointer(CirBuf_t* pCb, int32_t delta);
Error_t CbMoveReadPointer(CirBuf_t* pCb, int32_t delta);

//-----------------------------------------------------------------------------
//  Exported function reference - inline
//-----------------------------------------------------------------------------
// USE MACRO instead of inline because ARM compiler doesn't generate real-inline function.
#if TRUE
#define CbGetCount(pCb) ((pCb)->writeCount - (pCb)->readCount)
#define CbIsEmpty(pCb)  ((pCb)->writeCount == (pCb)->readCount)
#define CbIsFull(pCb)   (((pCb)->writeCount - (pCb)->readCount)==(pCb)->numEntries)

#define CbGetCurrentWritePointer(pCb)   ((pCb)->pWrite)
#define CbGetCurrentReadPointer(pCb)    ((pCb)->pRead)
#define CbGetWritePointer(pCb, offset)  CbGetPointer(pCb, offset, (pCb)->pWrite)
#define CbGetWritePointer(pCb, offset)  CbGetPointer(pCb, offset, (pCb)->pWrite)
#define CbGetReadPointer(pCb, offset)   CbGetPointer(pCb, offset, (pCb)->pRead)
#else
//-----------------------------------------------------------------------------
/**
    return number of data element count in circular buffer

    param[in]   pCb         circular buffer pointer

    @return  data entry count in circular buffer
**/
//-----------------------------------------------------------------------------
__inline uint32_t CbGetCount(CirBuf_t* pCb)
{
    return (pCb->writeCount - pCb->readCount);
}

//-----------------------------------------------------------------------------
/**
    return TRUE if circular buffer is empty

    param[in]   pCb         circular buffer pointer

    @return  TRUE buffer empty, else not
**/
//-----------------------------------------------------------------------------
__inline bool CbIsEmpty(CirBuf_t* pCb)
{
    return (pCb->writeCount == pCb->readCount);
}

//-----------------------------------------------------------------------------
/**
    return TRUE if circular buffer is full

    param[in]   pCb         circular buffer pointer

    @return  TRUE buffer full, else not
**/
//-----------------------------------------------------------------------------
__inline bool CbIsFull(CirBuf_t* pCb)
{
    return (pCb->writeCount - pCb->readCount)==pCb->numEntries;
}

//-----------------------------------------------------------------------------
/**
    Get write pointer.

    @param[in]   pCb        circular buffer pointer
    @param[in]   offset     offset

    @return write pointer
**/
//-----------------------------------------------------------------------------
__inline void* CbGetWritePointer(CirBuf_t* pCb, int32_t offset)
{
    return (CbGetPointer(pCb, offset, pCb->pWrite));
}

//-----------------------------------------------------------------------------
/**
    Get read pointer.

    @param[in]   pCb        circular buffer pointer
    @param[in]   offset     offset

    @return always cEcNoError
**/
//-----------------------------------------------------------------------------
__inline void* CbGetReadPointer(CirBuf_t* pCb, int32_t offset)
{
    return (CbGetPointer(pCb, offset, pCb->pRead));
}
#endif


