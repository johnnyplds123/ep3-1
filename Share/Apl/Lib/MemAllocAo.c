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
//! @brief Alloc only memory allocation codes
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "CirBuf.h"
#include "LinkList.h"
#include "MemAlloc.h"

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
    Intialize alloc only allocator sturcture

    param[in]   pAllocator      Alloc only allocator structure pointer
    param[in]   pFreeBufBase    free buffer base pointer
    param[in]   pFreeBufEnd1    free buffer end pointer + 1
**/
//-----------------------------------------------------------------------------
void MemAlloc_InitAo(AllocatorAo_t* pAllocator, void* pFreeBufBase, void* pFreeBufEnd1, uint32_t size)
{
    pAllocator->count        = 0;
    pAllocator->size         = size;
    pAllocator->pFreeBufBase = (uint8_t *)pFreeBufBase;
    pAllocator->pFreeBufEnd1 = (uint8_t *)pFreeBufEnd1;
    pAllocator->pFreeBuf     = (uint8_t *)pFreeBufBase;
}

//-----------------------------------------------------------------------------
/**
    Allocate Memory block, Good for one time memory allocation for array or table.

    param[in]   pAllocator  Alloc only allocator structure pointer
    param[in]   size        byte size of memory to allocate
    param[in]   align       address align (should be power of 2)

    @return  memory pointer (NULL if out of memory)
**/
//-----------------------------------------------------------------------------
void* AllocAo(AllocatorAo_t* pAllocator, uint32_t size, uint32_t align)
{
    uint8_t* pBuffer;

    pBuffer = (uint8_t*)ALIGN_CEIL((uint32_t)pAllocator->pFreeBuf, align); // make alignment

    if (pBuffer + size >= pAllocator->pFreeBufEnd1)
    {
        return (NULL); // not enough memory
    }

    pAllocator->pFreeBuf = pBuffer + size;    // move to next free pointer
    pAllocator->count++;

    return (pBuffer);
}

//-----------------------------------------------------------------------------
/**
    Dump Allocator information.\n

    param[in]   pAllocator      Alloc only allocator structure pointer

**/
//-----------------------------------------------------------------------------
void MemAlloc_DumpAo(AllocatorAo_t* pAllocator)
{
    M_PRINTF("ADDR:%X..%X TOTAL:%8u USED:%8u FREE:%8u (%3u%c) ALLOC:%u\n",
             pAllocator->pFreeBufBase, pAllocator->pFreeBufEnd1-1,
             pAllocator->size,
             pAllocator->size - (pAllocator->pFreeBufEnd1 - pAllocator->pFreeBuf),
             pAllocator->pFreeBufEnd1 - pAllocator->pFreeBuf,
             (pAllocator->pFreeBufEnd1 - pAllocator->pFreeBuf) * 100 / (pAllocator->size), 0x25,
             pAllocator->count);
}
