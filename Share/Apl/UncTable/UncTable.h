#pragma once
// ============================================================================
//
// ==        Copyright (c) LITEON Corporation  -  All rights reserved        ==
/*
    This computer program contains confidential and proprietary information,
    and may NOT be reproduced or transmitted, in whole or in part, in any form,
    or by any means electronic, mechanical, photo-optical, or otherwise, and
    may NOT be translated into another language without the express written
    permission from LITEON Corporation.
*/
// ==        Copyright (c) LITEON Corporation  -  All rights reserved        ==
//
//=============================================================================
//
//! @brief header of ECCTable
//=============================================================================
//-----------------------------------------------------------------------------
//  Include
//-----------------------------------------------------------------------------
#include "SysInfoTypes.h"
#if (_CPU_ == CPU_ID0)
#include "NvmeCmds.h"
#endif
//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
#if CHK_FW_TYPE(FW_AGING)
    #define UNC_TABLE_ENABLE        DISABLE
#else
    #define UNC_TABLE_ENABLE        ENABLE
#endif

#if (_CPU_ == CPU_ID0)
#if (CO_SUPPORT_WRITE_UNCORRECTABLE == TRUE)
    #define UNCT_SUPPORT_CPU_0    TRUE
#else
    #define UNCT_SUPPORT_CPU_0    FALSE
#endif
#else
    #define UNCT_SUPPORT_CPU_0    FALSE
#endif

#define UNC_TBL_ENTRY_NUM       ((SI_PAYLOAD_SIZE_UNCT - sizeof(tUNCTblBrief)) / sizeof(tUNCTblNode))
#define UNCT_INDX_BIT_NUM       14
#define UNCT_TYPE_BIT_NUM       16 - UNCT_INDX_BIT_NUM
#define UNCT_INDX_MASK          (WORD_MASK >> UNCT_TYPE_BIT_NUM)
//-----------------------------------------------------------------------------
// Exclusive LBA: 0 ~ 0x1000 (LAA 0~0x200) for better compatibility
enum{
    UNCT_PROTECT_LAA_START = 0,
    UNCT_PROTECT_LAA_END   = 0x200
};

enum{
    UNCT_NO_ERROR = 0,
    UNCT_ERR_NO_FREE_ENTRY,
    UNCT_ERR_LBA_OVERFLOW,
    UNCT_ERR_PROTECTED_LBA
};

enum{
    UNC_TBL_POOL_FREE = 0,
    UNC_TBL_POOL_USED = 1,
    UNC_TBL_POOL_NUM  = 2
};

enum{
    UNCT_SYNC_DISABLE  = 0x1000,
    UNCT_SYNC_ENABLE   = 0x1001
};

typedef enum
{
    UNCT_NDTP_INIT  = 0x0,
    UNCT_NDTP_WUNC  = 0x1,
    UNCT_NDTP_MEDIA = 0x2
}tUNCTblNdType;

typedef struct _tUNCTblBrief
{
    U16 head[UNC_TBL_POOL_NUM];
    U16 tail[UNC_TBL_POOL_NUM];
    U16 count[UNC_TBL_POOL_NUM];
    U16 rsvd[UNC_TBL_POOL_NUM];
} tUNCTblBrief;

typedef struct _tUNCTblNode
{
    U32              laa;
    U16              bmp;
    U16              next: UNCT_INDX_BIT_NUM;
    tUNCTblNdType    type: UNCT_TYPE_BIT_NUM;
} tUNCTblNode;

typedef struct _tUNCTBL_Info
{
    tUNCTblBrief    brief;
    tUNCTblNode     node[UNC_TBL_ENTRY_NUM];
} tUNCTblInfo;
COMPILE_ASSERT(sizeof(tUNCTblInfo) == (SI_PAYLOAD_SIZE_UNCT), "tUNCTblInfo size overflow");

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#if (UNC_TABLE_ENABLE == ENABLE || UNCT_SUPPORT_CPU_0 == TRUE)
    #define UNCT_CHECK_DO_SEARCH(lb, ln)         UNCT_Search(lb, ln)
    #define UNCT_CHECK_DO_INSERT(lb, ln, tp)     UNCT_Insert(lb, ln, tp)
    #define UNCT_CHECK_DO_DELETE(lb, ln, up)     UNCT_Delete(lb, ln, up)
    #define UNCT_CHECK_DO_RESET()                UNCT_Reset()
#else
    #define UNCT_CHECK_DO_SEARCH(lb, ln)         FALSE
    #define UNCT_CHECK_DO_INSERT(lb, ln, tp)
    #define UNCT_CHECK_DO_DELETE(lb, ln, up)
    #define UNCT_CHECK_DO_RESET()                {}
#endif
// next index use 14 bit, work mask compare only 14 bit
#define UNCT_WMASK_CHECK(I) ((I & UNCT_INDX_MASK) != (WORD_MASK & UNCT_INDX_MASK)) ? TRUE : FALSE
//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern tUNCTblInfo*    pUncTblInfo;

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
#if (_CPU_ == CPU_ID0)
Error_t UNCT_OneTimeInit(InitBootMode_t initMode);
void    UNCT_Reset(void);
void    UNCT_Dump(void);
#endif

#if (_CPU_ == CPU_ID0 || _CPU_ == CPU_ID2)
U32     UNCT_Delete(U64 lba, U64 len, U8 update);
U32     UNCT_Search(U64 lba, U64 len);
U32     UNCT_Insert(U64 lba, U64 len, U8 type);
#endif
