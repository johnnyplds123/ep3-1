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
#include "HalCpu.h"
#include "Kernel.h"
#include "KernelUser.h"
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
    Variable size memory allocation.\n

    param[in]   pAllocator      variablie size allocator structure pointer
    param[in]   size            byte size of memory to allocate
    param[in]   align           address align (minimum 4)

    @return  memory pointer (HALT if out of memory)
**/
//-----------------------------------------------------------------------------
void* Mem_AllocVs(AllocatorVs_t* pAllocator, uint32_t size, uint32_t align, Cstr_t funcName)
{
    void* pBuffer;

    BEGIN_MULTI_CS1(cSpinLockMemAlloc)

    pBuffer = AllocVs(pAllocator, size, align, __return_address());

    MemAlloc_CheckVS();

    END_MULTI_CS1(cSpinLockMemAlloc);

    D_PRINTF_MALOC("AlocVs :%1X.%X, %X..%X, %s\n", HalCpu_GetId(), pBuffer, (uint32_t)pBuffer + size, size, funcName);

    ASSERT(pBuffer != NULL);

    return pBuffer;
}

//-----------------------------------------------------------------------------
/**
    Free a memory block which was previously allocated\n

    param[in]   pAllocator      variablie size allocator structure pointer
    param[in]   size            byte size of memory to allocate
    param[in]   align           address align

    @return number of printed out characters
**/
//-----------------------------------------------------------------------------
void Mem_FreeVs(AllocatorVs_t* pAllocator, void* pBuffer)
{
    BEGIN_MULTI_CS1(cSpinLockMemAlloc)

    FreeVs(pAllocator, pBuffer);

    END_MULTI_CS1(cSpinLockMemAlloc);

    D_PRINTF_MALOC("FreeVs :%1X:%X\n", HalCpu_GetId(), pBuffer);
}

//-----------------------------------------------------------------------------
/**
    Allocate Memory block

    param[in]   pAllocator  Alloc only allocator structure pointer
    param[in]   size        byte size of memory to allocate
    param[in]   align       address align

    @return  memory pointer (NULL if out of memory)
**/
//-----------------------------------------------------------------------------
void* Mem_AllocAo(AllocatorAo_t* pAllocator, uint32_t size, uint32_t align, Cstr_t funcName)
{
    void* pBuffer;

    BEGIN_MULTI_CS1(cSpinLockMemAlloc)

    pBuffer = AllocAo(pAllocator, size, align);

    END_MULTI_CS1(cSpinLockMemAlloc);

    D_PRINTF_MALOC("AlocAo :%1X.%X, %X..%X, %s\n", HalCpu_GetId(), pBuffer, (uint32_t)pBuffer + size, size, funcName);

    ASSERT(pBuffer != NULL);

    return pBuffer;
}

//-----------------------------------------------------------------------------
/**
    Allocate memory block,
      allocate memory from pAllocAo1 first, if failed and then pAllocAo2

    param[in]   pAllocAo1   Alloc only allocator structure pointer
    param[in]   pAllocAo2   Alloc only allocator structure pointer
    param[in]   size        byte size of memory to allocate
    param[in]   align       address align

    @return  memory pointer (NULL if out of memory)
**/
//-----------------------------------------------------------------------------
void* Mem_AllocAo2(AllocatorAo_t* pAllocAo1, AllocatorAo_t* pAllocAo2, uint32_t size, uint32_t align, Cstr_t funcName)
{
    void* pBuffer;

    BEGIN_MULTI_CS1(cSpinLockMemAlloc)
    pBuffer = AllocAo(pAllocAo1, size, align);

    if (pBuffer == NULL && pAllocAo2 != NULL)
    {
        pBuffer = AllocAo(pAllocAo2, size, align);
    }

    if (pBuffer == NULL)
    {
        pBuffer = AllocVs(&smDramAllocatorVs, size, align, __return_address());
    }

    END_MULTI_CS1(cSpinLockMemAlloc);

    D_PRINTF_MALOC("AlocAo2:%1X.%X, %X..%X, %s\n", HalCpu_GetId(), pBuffer, (uint32_t)pBuffer + size, size, funcName);

    ASSERT(pBuffer != NULL);

    return pBuffer;
}
