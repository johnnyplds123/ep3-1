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
//! @brief  Memory allocation APIs
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define MEM_DEBUG                               FALSE

#if MEM_DEBUG == TRUE
    #define MEM_DUMP_FUNC                       __FUNCTION__
    #define D_PRINTF_MALOC(F, args...)          {D_PRINTF("MEM: "F, ##args);}
#else
    #define MEM_DUMP_FUNC                       NULL
    #define D_PRINTF_MALOC(F, args...)          {}
#endif

#if CHK_CODE_TYPE(CODE_MAIN)
    // 1 stage allocator,
    #define MEM_AllocItcm(size, align)          Mem_AllocAo(&itcmAllocatorAo,   size, align, MEM_DUMP_FUNC)
    #define MEM_AllocDtcm(size, align)          Mem_AllocAo(&dtcmAllocatorAo,   size, align, MEM_DUMP_FUNC)
    #define MEM_AllocStcm(size, align)          Mem_AllocAo(&smStcmAllocatorAo, size, align, MEM_DUMP_FUNC)
    #define MEM_AllocSspm(size, align)          Mem_AllocAo(&smSspmAllocatorAo, size, align, MEM_DUMP_FUNC)

    // 2 stage allocator, TCM and then DRAM
    #define MEM_AllocItcmDram(size, align)      Mem_AllocAo2(&itcmAllocatorAo,   NULL, size, align, MEM_DUMP_FUNC)
    #define MEM_AllocDtcmDram(size, align)      Mem_AllocAo2(&dtcmAllocatorAo,   NULL, size, align, MEM_DUMP_FUNC)
    #define MEM_AllocStcmDram(size, align)      Mem_AllocAo2(&smStcmAllocatorAo, NULL, size, align, MEM_DUMP_FUNC)
    #define MEM_AllocSspmDram(size, align)      Mem_AllocAo2(&smSspmAllocatorAo, NULL, size, align, MEM_DUMP_FUNC)

    // 3 stage allocator, TCM, SSPM and then DRAM
    #define MEM_AllocItcmSspmDram(size, align)  Mem_AllocAo2(&itcmAllocatorAo,   &smSspmAllocatorAo, size, align, MEM_DUMP_FUNC)
    #define MEM_AllocDtcmSspmDram(size, align)  Mem_AllocAo2(&dtcmAllocatorAo,   &smSspmAllocatorAo, size, align, MEM_DUMP_FUNC)
    #define MEM_AllocStcmSspmDram(size, align)  Mem_AllocAo2(&smStcmAllocatorAo, &smSspmAllocatorAo, size, align, MEM_DUMP_FUNC)

    #if _DRAM_==DRAM_NONE
        #define MEM_AllocBuffer(size, align)    Mem_AllocAo(&smSspmAllocatorAo, size, align, MEM_DUMP_FUNC)    // Use SPM for Buffer if DRAM is not installed
        #define MEM_FreeBuffer(pBuffer)
        #define MEM_GetBufferSize(pBuffer)      0
    #else
        #define MEM_AllocBuffer(size, align)    Mem_AllocVs(&smDramAllocatorVs, size, align, MEM_DUMP_FUNC)
        #define MEM_FreeBuffer(pBuffer)         Mem_FreeVs (&smDramAllocatorVs, pBuffer)
        #define MEM_GetBufferSize(pBuffer)      GetSizeVs(&smDramAllocatorVs, pBuffer)
    #endif
#endif

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief Alloc Only memory Allocator - good for one time allocation in initialization code
typedef struct
{
    uint32_t       count;       ///< allocated count
    uint32_t       size;        ///< total available size
    uint8_t*       pFreeBufBase;///< free buffer base pointer
    uint8_t*       pFreeBufEnd1;///< free buffer end pointer + 1
    uint8_t*       pFreeBuf;    ///< current free buffer pointer
} AllocatorAo_t;

/// @brief Variable size memory allocator
typedef struct
{
    LinkHeadTail_t allocList;   ///< allocated entry list
    uint8_t*       pFreeBufBase;///< free buffer base pointer
    uint8_t*       pFreeBufEnd1;///< free buffer end pointer
} AllocatorVs_t;

/// @brief memory allocator node
typedef struct
{
    DlinkEntry_t dlink;         ///< doubly linked list
    uint8_t*     pBufferEnd1;   ///< allocated buffer end pointer + 1
    uint32_t     debugInfo:24;  ///< debug information
    uint32_t     tag      : 8;  ///< allocation tag
} AllocNode_t;
COMPILE_ASSERT(sizeof(AllocNode_t)==16,"size shall be 16 bytes");

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern AllocatorAo_t itcmAllocatorAo;   ///< ITCM alloc only allocator
extern AllocatorAo_t dtcmAllocatorAo;   ///< DTCM alloc only allocator
extern AllocatorAo_t smStcmAllocatorAo; ///< Shared TCM alloc only allocator
extern AllocatorAo_t smSspmAllocatorAo; ///< Shared SPM alloc only allocator
extern AllocatorVs_t smDramAllocatorVs; ///< Shared DRAM variable size allocator

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
void     MemAlloc_InitAo(AllocatorAo_t* pAllocator, void* pFreeBufBase, void* pFreeBufEnd1, uint32_t size);
void     MemAlloc_DumpAo(AllocatorAo_t* pAllocator);
void*    AllocAo(AllocatorAo_t* pAllocator, uint32_t size, uint32_t align);

void     MemAlloc_InitVs(AllocatorVs_t* pAllocator, void* pFreeBufBase, void* pFreeBufEnd1);
void     MemAlloc_DumpVs(AllocatorVs_t* pAllocator);
void*    AllocVs(AllocatorVs_t* pAllocator, uint32_t size, uint32_t align, uint32_t debugInfo);
void     FreeVs(AllocatorVs_t* pAllocator, void* pBuffer);
uint32_t GetSizeVs(AllocatorVs_t* pAllocator, void* pBuffer);
void     MemAlloc_CheckVS(void);

void*    Mem_AllocVs(AllocatorVs_t* pAllocAo, uint32_t size, uint32_t align, Cstr_t funcName);
void     Mem_FreeVs(AllocatorVs_t* pAllocAo, void* pBuffer);
void*    Mem_AllocAo(AllocatorAo_t* pAllocAo, uint32_t size, uint32_t align, Cstr_t funcName);
void*    Mem_AllocAo2(AllocatorAo_t* pAllocAo1, AllocatorAo_t* pAllocAo2, uint32_t size, uint32_t align, Cstr_t funcName);
