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
//! @brief Variable size memory allocation codes
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "CirBuf.h"
#include "LinkList.h"
#include "HalCpu.h"
#include "MemAlloc.h"

#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define MEM_TAG_FREE            0x00            ///< free buffer tag
#define MEM_TAG_ALLOCATED       0x88            ///< allocated buffer tag

#define MEM_OFFSET_DBG_CPUID    22              ///< offset of cpuid in debugInfo

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
    Initialize variable size memory allocator structure.\n

    param[in]   pAllocator      Variable size memory allocator structure pointer
    param[in]   numNode         maximum number of memory block
    param[in]   pFreeBufBase    free buffer base pointer
    param[in]   pFreeBufEnd1    free buffer end pointer + 1

    @return  cEcNoError if success, else error
**/
//-----------------------------------------------------------------------------
void MemAlloc_InitVs(AllocatorVs_t* pAllocator, void* pFreeBufBase, void* pFreeBufEnd1)
{
    AllocNode_t* pNode;

    pNode = (AllocNode_t *)pFreeBufBase;

    pAllocator->pFreeBufBase = (uint8_t *)pFreeBufBase + sizeof(AllocNode_t) * 2;
    pAllocator->pFreeBufEnd1 = (uint8_t *)pFreeBufEnd1;

    pNode->tag         = MEM_TAG_FREE;
    pNode->debugInfo   = 0xDEAD;
    pNode->pBufferEnd1 = pAllocator->pFreeBufBase;

    DlInit(&pAllocator->allocList);

    // add guard node to speed the scanning process up.

    DlAddToHead(&pAllocator->allocList, &pNode->dlink);
}

//-----------------------------------------------------------------------------
/**
    Allocate a memory block\n.

    param[in]   pAllocator      Variable size memory allocator structure pointer
    param[in]   size            byte size of memory to allocate
    param[in]   align           address align (should be power of 2)
    param[in]   debugInfo       debug information

    @return  memory pointer (NULL if out of memory)
**/
//-----------------------------------------------------------------------------
void* AllocVs(AllocatorVs_t* pAllocator, uint32_t size, uint32_t align, uint32_t debugInfo)
{
    DlinkEntry_t* pScanEntry;
    AllocNode_t*  pScanNode;
    AllocNode_t*  pFreeNode;
    uint8_t*      pFreeBegin;
    uint8_t*      pFreeEnd1;

    pFreeEnd1 = pAllocator->pFreeBufEnd1;
    align = ALIGN_CEIL(align, 4); // should be multiple of 4byte align

    // backward scan to find free space.
    DL_FOR_EACH_BACKWARD(pScanEntry, &pAllocator->allocList)
    {
        pScanNode = STR_PTR_FROM_MEMBER(pScanEntry, AllocNode_t, dlink);

        pFreeBegin = pScanNode->pBufferEnd1 + sizeof(AllocNode_t);
        // make alignment
        pFreeBegin = (uint8_t*)ALIGN_CEIL((uint32_t)pFreeBegin, align);

        // test free space is enough or not
        if ((pFreeBegin<pFreeEnd1) && ((uint32_t)pFreeEnd1-(uint32_t)pFreeBegin>=size))
        {
            // create a node, and add to alloc list
            pFreeNode = (AllocNode_t *)(pFreeBegin - sizeof(AllocNode_t));
            pFreeNode->pBufferEnd1 = pFreeBegin + size;
            pFreeNode->debugInfo   = (HalCpu_GetId() << MEM_OFFSET_DBG_CPUID) | debugInfo;
            pFreeNode->tag         = MEM_TAG_ALLOCATED; // for debugging

            DlAddToNext(&pFreeNode->dlink, pScanEntry);

            return (pFreeBegin);
        }

        pFreeEnd1 = (uint8_t *)pScanNode;
    }

    return (NULL);
}

//-----------------------------------------------------------------------------
/**
    Free a memory block which was previously allocated\n

    param[in]   pAllocator      Variable size memory allocator structure pointer
    param[in]   pBuffer         previously allocated memory block address

    @return  cEcNoError if success, else error
**/
//-----------------------------------------------------------------------------
void FreeVs(AllocatorVs_t* pAllocator, void* pBuffer)
{
    AllocNode_t* pNode;

    pNode = (AllocNode_t *)((uint32_t)pBuffer - sizeof(AllocNode_t));

    ASSERT(pNode->tag == MEM_TAG_ALLOCATED);

    pNode->tag = MEM_TAG_FREE;

    DlRemove(&pNode->dlink);
}

//-----------------------------------------------------------------------------
/**
    Get allocated buffer size.

    param[in]   pAllocator      Variable size memory allocator structure pointer
    param[in]   pBuffer         previously allocated memory block address

    @return  allocated buffer size in byte count.
**/
//-----------------------------------------------------------------------------
uint32_t GetSizeVs(AllocatorVs_t* pAllocator, void* pBuffer)
{
    AllocNode_t* pNode;

    pNode = (AllocNode_t *)((uint32_t)pBuffer - sizeof(AllocNode_t));

    if (pNode->tag != MEM_TAG_ALLOCATED)
    {
        return(0);
    }

    return (uint32_t)pNode->pBufferEnd1 - (uint32_t)pNode - sizeof(AllocNode_t);
}

//-----------------------------------------------------------------------------
/**
    Dump variable size allocator information\n

    param[in]   pAllocator      Variable size memory allocator structure pointer

    @return  cEcNoError if success, else error
**/
//-----------------------------------------------------------------------------
void MemAlloc_DumpVs(AllocatorVs_t* pAllocator)
{
    DlinkEntry_t* pEntry;
    AllocNode_t* pNode;
    uint32_t totalAllocSize;
    uint32_t index;

    index = 0;
    totalAllocSize = 0;

    DL_FOR_EACH(pEntry, &pAllocator->allocList)
    {
        pNode = STR_PTR_FROM_MEMBER(pEntry, AllocNode_t, dlink);

        M_PRINTF("%4U:%X..%X, %X, TAG:%2X DBG:%6X\n", index++, pNode + 1, pNode->pBufferEnd1 - 1,
                  pNode->pBufferEnd1 - (uint8_t *)pNode - sizeof(AllocNode_t), pNode->tag, pNode->debugInfo);

        totalAllocSize += ((uint32_t)pNode->pBufferEnd1 - (uint32_t)pNode);
    }

    M_PRINTF("TOTAL USED:%U, FREE:%U Bytes\n", totalAllocSize, ((U32)_DRAM_SIZE_ * MBYTE) - totalAllocSize);
}

//-----------------------------------------------------------------------------
/**
    Debug memory allocator
**/
//-----------------------------------------------------------------------------
void MemAlloc_CheckVS(void)
{
    #if MEM_DEBUG == TRUE
    AllocatorVs_t* pAlloc = &smDramAllocatorVs;
    DlinkEntry_t* pEntry;
    AllocNode_t* pNode;
    U32 endAddr;

    DL_FOR_EACH(pEntry, &pAlloc->allocList)
    {
        pNode = STR_PTR_FROM_MEMBER(pEntry, AllocNode_t, dlink);
        endAddr = (U32)pNode->pBufferEnd1;

        if (endAddr < (U32)pAlloc->pFreeBufBase || endAddr > (U32)pAlloc->pFreeBufEnd1 || (pNode->tag != MEM_TAG_FREE && pNode->tag != MEM_TAG_ALLOCATED))
        {
            D_PRINTF("Check Fail: Base:%08X EndAddr:%08X Tag:%02X Dbg:%06X\n", pNode + 1, pNode->pBufferEnd1, pNode->tag, pNode->debugInfo);
            ASSERT(0);
        }
    }
    #endif
}
