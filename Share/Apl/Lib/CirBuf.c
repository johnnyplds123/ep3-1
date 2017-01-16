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
//! @brief  Circular buffer management code
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "MemIo.h"
#include "CirBuf.h"

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
    Initialize circular buffer structure.

    @param[in]   pCb         circular buffer pointer
    @param[in]   pBuffer     buffer pointer to store elements
    @param[in]   numEntries  number of entries
    @param[in]   entrySize   size of entry
**/
//-----------------------------------------------------------------------------
void CbInit(CirBuf_t* pCb, void* pBuffer, uint32_t numEntries, uint32_t entrySize)
{
    pCb->pBegin = (uint8_t *)pBuffer;
    pCb->pEnd = pCb->pBegin + numEntries*entrySize;

    pCb->numEntries = numEntries;
    pCb->entrySize = entrySize;

    CbReset(pCb);
}

//-----------------------------------------------------------------------------
/**
    Reset a circular buffer structure.\n
        Set read and write pointer to buffer begin.\n
        Clear read and write counter.\n

    @param[in]   pCb         circular buffer pointer
**/
//-----------------------------------------------------------------------------
void CbReset(CirBuf_t* pCb)
{
    pCb->pRead = pCb->pBegin;
    pCb->pWrite = pCb->pBegin;
    pCb->readCount = 0;
    pCb->writeCount = 0;
}

//-----------------------------------------------------------------------------
/**
    Get pointer with offset

    @param[in]   pCb        circular buffer pointer
    @param[in]   offset     offset
    @param[in]   pBase      base pointer

    @return data buffer pointer
**/
//-----------------------------------------------------------------------------
void* CbGetPointer(CirBuf_t* pCb, int32_t offset, uint8_t* pBase)
{
    pBase += (pCb->entrySize * offset);

    if (offset>0 && (pBase>=pCb->pEnd))
    {
        pBase = pCb->pBegin + (pBase - pCb->pEnd);
    }

    if (offset<0 && (pBase<pCb->pBegin))
    {
        pBase = pCb->pEnd + (pBase - pCb->pBegin);
    }

    return (pBase);
}

//-----------------------------------------------------------------------------
/**
    Move write pointer.

    @param[in]   pCb         circular buffer pointer
    @param[in]   delta

    @return always cEcNoError
**/
//-----------------------------------------------------------------------------
Error_t CbMoveWritePointer(CirBuf_t* pCb, int32_t delta)
{
    pCb->pWrite = CbGetWritePointer(pCb, delta);
    pCb->writeCount += delta;

    return (cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Move read pointer.

    @param[in]   pCb         circular buffer pointer
    @param[in]   delta

    @return always cEcNoError
**/
//-----------------------------------------------------------------------------
Error_t CbMoveReadPointer(CirBuf_t* pCb, int32_t delta)
{
    pCb->pRead = CbGetReadPointer(pCb, delta);
    pCb->readCount += delta;

    return (cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Increament write pointer.

    @param[in]   pCb         circular buffer pointer

    @return always cEcNoError
**/
//-----------------------------------------------------------------------------
Error_t CbIncWritePointer(CirBuf_t* pCb)
{
    pCb->pWrite += pCb->entrySize;

    if (pCb->pWrite>=pCb->pEnd) pCb->pWrite = pCb->pBegin;

    pCb->writeCount++;

    return (cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Increament read pointer.

    @param[in]   pCb         circular buffer pointer

    @return always cEcNoError
**/
//-----------------------------------------------------------------------------
Error_t CbIncReadPointer(CirBuf_t* pCb)
{
    pCb->pRead += pCb->entrySize;

    if (pCb->pRead>=pCb->pEnd) pCb->pRead = pCb->pBegin;

    pCb->readCount++;

    return (cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Write one element data to Circular Buffer

    @param[in]   pCb         circular buffer pointer
    @param[in]   pData       data pointer

    @return  cEcNoError if success, else error
**/
//-----------------------------------------------------------------------------
Error_t CbWrite(CirBuf_t* pCb, void* pData)
{
    if (CbIsFull(pCb)) return cEcError;

    MEM_COPY(pCb->pWrite, pData, pCb->entrySize);

    return (CbIncWritePointer(pCb));
}

//-----------------------------------------------------------------------------
/**
    Read one element data from Circular Buffer

    @param[in]   pCb         circular buffer pointer
    @param[out]  pData       data pointer
    @param[in]   remove      Messages are removed from the queue

    @return  cEcError if success, else no data because of empty
**/
//-----------------------------------------------------------------------------
Error_t CbRead(CirBuf_t* pCb, void* pData, bool remove)
{
    if (CbIsEmpty(pCb)) return cEcError;

    MEM_COPY(pData, pCb->pRead, pCb->entrySize);

    if (!remove) return cEcNoError;

    return (CbIncReadPointer(pCb));
}

//-----------------------------------------------------------------------------
/**
    Write uint64_t type data to Circular Buffer

    @param[in]   pCb         circular buffer pointer
    @param[in]   pData       data pointer

    @return  cEcNoError if success, else error
**/
//-----------------------------------------------------------------------------
Error_t CbWriteU64(CirBuf_t* pCb, void* pData)
{
    if (CbIsFull(pCb)) return cEcError;

    MEM_U64(pCb->pWrite) = MEM_U64(pData);

    return (CbIncWritePointer(pCb));
}

//-----------------------------------------------------------------------------
/**
    Read uint64_t type data from Circular Buffer

    @param[in]   pCb         circular buffer pointer
    @param[out]  pData       data pointer
    @param[in]   remove      Messages are removed from the queue

    @return  cEcError if success, else no data because of empty
**/
//-----------------------------------------------------------------------------
Error_t CbReadU64(CirBuf_t* pCb, void* pData, bool remove)
{
    if (CbIsEmpty(pCb)) return cEcError;

    MEM_U64(pData) = MEM_U64(pCb->pRead);

    if (!remove) return cEcNoError;

    return (CbIncReadPointer(pCb));
}

//-----------------------------------------------------------------------------
/**
    Write uint32_t type data to Circular Buffer

    @param[in]   pCb         circular buffer pointer
    @param[in]   pData       data pointer

    @return  cEcNoError if success, else error
**/
//-----------------------------------------------------------------------------
Error_t CbWriteU32(CirBuf_t* pCb, uint32_t data)
{
    if (CbIsFull(pCb)) return cEcError;

    MEM_U32(pCb->pWrite) = data;

    return (CbIncWritePointer(pCb));
}

//-----------------------------------------------------------------------------
/**
    Read uint32_t type data from Circular Buffer

    @param[in]   pCb         circular buffer pointer
    @param[out]  pData       data pointer
    @param[in]   remove      Messages are removed from the queue

    @return  cEcError if success, else no data because of empty
**/
//-----------------------------------------------------------------------------
Error_t CbReadU32(CirBuf_t* pCb, void* pData, bool remove)
{
    if (CbIsEmpty(pCb)) return cEcError;

    MEM_U32(pData) = MEM_U32(pCb->pRead);

    if (!remove) return cEcNoError;

    return (CbIncReadPointer(pCb));
}

//-----------------------------------------------------------------------------
/**
    Write uint16_t type data to Circular Buffer

    @param[in]   pCb         circular buffer pointer
    @param[in]   pData       data pointer

    @return  cEcNoError if success, else error
**/
//-----------------------------------------------------------------------------
Error_t CbWriteU16(CirBuf_t* pCb, uint16_t data)
{
    if (CbIsFull(pCb)) return cEcError;

    MEM_U16(pCb->pWrite) = data;

    return (CbIncWritePointer(pCb));
}

//-----------------------------------------------------------------------------
/**
    Read uint16_t type data from Circular Buffer

    @param[in]   pCb         circular buffer pointer
    @param[out]  pData       data pointer
    @param[in]   remove      Messages are removed from the queue

    @return  cEcError if success, else no data because of empty
**/
//-----------------------------------------------------------------------------
Error_t CbReadU16(CirBuf_t* pCb, void* pData, bool remove)
{
    if (CbIsEmpty(pCb)) return cEcError;

    MEM_U16(pData) = MEM_U16(pCb->pRead);

    if (!remove) return cEcNoError;

    return (CbIncReadPointer(pCb));
}


