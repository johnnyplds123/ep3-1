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
//! @brief Header file for Cache API
//! Evan: Please make cache code as simple/clean as possible!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "Fio.h"

//-----------------------------------------------------------------------------
//  Constant definitions:
//-----------------------------------------------------------------------------
#define WRITE_CACHE_SPLIT_POOL_CNT  2
#define WRITE_CACHE_SPLIT_SHIFT     6   // 2^6 = 64 LAA = 256 KB
#define WRITE_CACHE_SPLIT_MASK      ((WRITE_CACHE_SPLIT_POOL_CNT - 1) << WRITE_CACHE_SPLIT_SHIFT)
#define WRITE_CACHE_POOL_MAX        4
COMPILE_ASSERT(WRITE_CACHE_SPLIT_POOL_CNT <= WRITE_CACHE_POOL_MAX, "pool overlimit");
//-----------------------------------------------------------------------------
/*####################################################################
##      ##    ###    ########  ##    ## #### ##    ##  ######   ####
##  ##  ##   ## ##   ##     ## ###   ##  ##  ###   ## ##    ##  ####
##  ##  ##  ##   ##  ##     ## ####  ##  ##  ####  ## ##        ####
##  ##  ## ##     ## ########  ## ## ##  ##  ## ## ## ##   ####  ##
##  ##  ## ######### ##   ##   ##  ####  ##  ##  #### ##    ##
##  ##  ## ##     ## ##    ##  ##   ###  ##  ##   ### ##    ##  ####
 ###  ###  ##     ## ##     ## ##    ## #### ##    ##  ######   ####
#####################################################################*/
// change enumeration order at your own risk!
enum
{
    CACHE_R_NO_HIT = 0,
    CACHE_R_PARTIAL_HIT,
    CACHE_R_FULL_HIT,
    CACHE_R_FULL_HIT_BLANK,
    CACHE_R_FILLUP_HIT,
    CACHE_R_IDS,                // 5

    CACHE_W_NO_HIT,
    CACHE_W_PARTIAL_HIT,
    CACHE_W_FULL_HIT,
    CACHE_W_FUA,

    CACHE_FLUSH,                // 10
    CACHE_FILLUP,
    CACHE_GCREADAUX,
    CACHE_GCREAD,
    CACHE_GCLASTPAGE,

    CACHE_PREREAD,              // 15
    CACHE_PREOPEN,
    CACHE_LASTPAGE,
    CACHE_SCANPAGE,
    CACHE_TCG,

    CACHE_SYSINFO,              // 20
    CACHE_WRDUMMY,
    CACHE_AUTO,
    CACHE_ABORT,
    CACHE_STATUS_COUNT
};

enum
{
    SSD_POOL_FREE=0,
    SSD_POOL_FILLUP,
    SSD_POOL_WRITE,
    SSD_POOL_GC=SSD_POOL_WRITE+WRITE_CACHE_POOL_MAX,
    SSD_POOL_OTHER,
    SSD_POOL_RR,
    SSD_POOL_COUNT,
    SSD_POOL_NULL
};

enum
{
    RST_CACHE_INIT = 0,
    RST_CACHE_CREATE,
    RST_CACHE_FLUSH
};

enum
{
    CACHE_RST_MODE_QUICK = 0,
    CACHE_RST_MODE_FORCE
};

//-----------------------------------------------------------------------------
#define CACHE_RW_SIZE           (8 * MBYTE)

#define CACHE_NULL_START        0
#define CACHE_NULL_COUNT        1
#define CACHE_NULL_END          (CACHE_NULL_START + CACHE_NULL_COUNT - 1)

#define CACHE_RW_START          (CACHE_NULL_END + 1)
#define CACHE_RW_COUNT          (CACHE_RW_SIZE / MBU_SIZE)
#define CACHE_RW_END            (CACHE_RW_START + CACHE_RW_COUNT - 1)

#define CACHE_ZERO_START        (CACHE_RW_END + 1)
#define CACHE_ZERO_COUNT        10
#define CACHE_ZERO_END          (CACHE_ZERO_START + CACHE_ZERO_COUNT - 1)

#define CACHE_DUMMY_START       (CACHE_ZERO_END + 1)
#define CACHE_DUMMY_COUNT       1
#define CACHE_DUMMY_END         (CACHE_DUMMY_START + CACHE_DUMMY_COUNT - 1)

#define CACHE_IF_START          (CACHE_DUMMY_END + 1)
#define CACHE_IF_COUNT          4
#define CACHE_IF_END            (CACHE_IF_START + CACHE_IF_COUNT - 1)

#define CACHE_TOTAL_COUNT       (CACHE_NULL_COUNT + CACHE_RW_COUNT + CACHE_ZERO_COUNT + CACHE_DUMMY_COUNT + CACHE_IF_COUNT)

#define CACHE_TOTAL_SIZE        (CACHE_TOTAL_COUNT * MBU_SIZE)

//-----------------------------------------------------------------------------
#if _MEDIA_ == MEDIA_DRAM
    #define RAM_DISK_SIZE       (RAM_DRIVE_BUFFER_SIZE)
#endif

#define FAST_CPU0_TCM_BUF_SIZE  (8 * KBYTE)
#define FAST_CPU1_TCM_BUF_SIZE  (1 * KBYTE)
#define FAST_CPU2_TCM_BUF_SIZE  (5 * KBYTE)

//-----------------------------------------------------------------------------
#define SSD_NULL_START          CACHE_NULL_START
#define SSD_NULL_COUNT          CACHE_NULL_COUNT
#define SSD_NULL_END            (SSD_NULL_START + SSD_NULL_COUNT - 1)

#define SSD_RW_START            (SSD_NULL_END + 1)
#define SSD_RW_COUNT            CACHE_RW_COUNT
#define SSD_RW_END              (SSD_RW_START + SSD_RW_COUNT - 1)

#define SSD_ZERO_START          (SSD_RW_END + 1)
#define SSD_ZERO_COUNT          CACHE_ZERO_COUNT
#define SSD_ZERO_END            (SSD_ZERO_START + SSD_ZERO_COUNT - 1)

#define SSD_DUMMY_START         (SSD_ZERO_END + 1)
#define SSD_DUMMY_COUNT         CACHE_DUMMY_COUNT
#define SSD_DUMMY_END           (SSD_DUMMY_START + SSD_DUMMY_COUNT - 1)

#define SSD_IF_START            (SSD_DUMMY_END + 1)
#define SSD_IF_COUNT            CACHE_IF_COUNT
#define SSD_IF_END              (SSD_IF_START + SSD_IF_COUNT - 1)

#define SSD_CMP_START           (SSD_IF_END + 1)
#define SSD_CMP_COUNT           1
#define SSD_CMP_END             (SSD_CMP_START + SSD_CMP_COUNT - 1)

#define SSD_TCG_START           (SSD_CMP_END + 1)
#define SSD_TCG_COUNT           1
#define SSD_TCG_END             (SSD_TCG_START + SSD_TCG_COUNT - 1)

#define SSD_ADMIN_START         (SSD_TCG_END + 1)
#define SSD_ADMIN_COUNT         1
#define SSD_ADMIN_END           (SSD_ADMIN_START + SSD_ADMIN_COUNT - 1)

#define SSD_RR_START            (SSD_ADMIN_END + 1)
#define SSD_RR_COUNT            384
#define SSD_RR_END              (SSD_RR_START + SSD_RR_COUNT - 1)

#define SSD_OTHER_START         (SSD_RR_END + 1)
#define SSD_OTHER_COUNT         (NUM_OF_SSD - SSD_NULL_COUNT - SSD_RW_COUNT - SSD_ZERO_COUNT - SSD_DUMMY_COUNT - SSD_IF_COUNT - SSD_CMP_COUNT - SSD_TCG_COUNT - SSD_ADMIN_COUNT - SSD_RR_COUNT)
#define SSD_OTHER_END           (SSD_OTHER_START + SSD_OTHER_COUNT - 1)

// For Eldora-Plus, left shifting 2 bits to meet 34-bits PAE setting
#if _SOC_ == 1093
    #define SSD_ADDR_SHIFT      0
#else
    #error "Unsupported SoC"
#endif

//-----------------------------------------------------------------------------
// Cache bitmap
#if MBU_SIZE == (4 * KBYTE)
    #define CACHE_BMP_ALL_VALID 0xFF
#elif MBU_SIZE == (8 * KBYTE)
    #define CACHE_BMP_ALL_VALID 0xFFFF
#endif

#define CACHE_BMP_ALL_INVALID   0

#define CACHE_FIILUP_B2C_SHIFT  4
#define CACHE_FIILUP_B2C_MASK   0xF

// Cache Backend Status
#define CACHE_S_SEQ_READ        B_0
#define CACHE_S_SEQ_WRITE       B_1
#define CACHE_S_SEQ_FILLUP      B_2
#define CACHE_S_SEQ_FUA         B_3

#define CACHE_S_SEQ_MASK        (CACHE_S_SEQ_READ | CACHE_S_SEQ_WRITE | CACHE_S_SEQ_FILLUP | CACHE_S_SEQ_FUA)

// Cache Frontend Status
#define CACHE_S_XFER_READ       B_4
#define CACHE_S_XFER_WRITE      B_5

#define CACHE_S_XFER_MASK       (CACHE_S_XFER_READ | CACHE_S_XFER_WRITE)

// Cache Abort MASK
#define CACHE_S_ERROR           (B_7)
#define CACHE_S_ABORT_MASK      (CACHE_S_XFER_MASK | CACHE_S_SEQ_MASK)

//-----------------------------------------------------------------------------
#define LIMIT_NUM_OF_SSD_PER_AU_WORKAROUND

#ifdef LIMIT_NUM_OF_SSD_PER_AU_WORKAROUND
    #define MAX_SSD_PER_AU          4

    #if MBU_SIZE == (4 * KBYTE)
        #define MAX_SSD_PER_FILLUP  12
    #elif MBU_SIZE == (8 * KBYTE)
        #define MAX_SSD_PER_FILLUP  24
    #endif

    #define MAX_PAA_PER_FILLUP      (MAX_SSD_PER_FILLUP / MAX_SSD_PER_AU)
#else
    #define MAX_SSD_PER_AU          HBU_PER_MBU
    #define MAX_SSD_PER_FILLUP      HBU_PER_MBU
    #define MAX_PAA_PER_FILLUP      MAX_SSD_PER_FILLUP
    #define MAX_PAA_PER_FILLUP      1
#endif

//-----------------------------------------------------------------------------
#if _CPU_==CPU_ID2
    #if (_NAND_ == NAND_3DTLCG2)
        #define CACHE_FLUSH_ONE_CE_CNT      (gNfInfo.numChannels * gEcuInfo.auPerPage * gNfInfo.planesPerLun * gNfInfo.pagesPerProgUnit)
    #else
        #define CACHE_FLUSH_ONE_CE_CNT      (gNfInfo.numChannels * gEcuInfo.auPerPage * gNfInfo.planesPerLun)
    #endif

    #define CACHE_PREREAD_CNT               (gNfInfo.numChannels * gEcuInfo.auPerPage)

    #if (_NAND_ == NAND_3DTLCG2)
        #define CACHE_FLUSH_THRESHOLD       (CACHE_FLUSH_ONE_CE_CNT * WRITE_CACHE_SPLIT_POOL_CNT)
        #define CACHE_MERGE_THRESHOLD       (CACHE_FLUSH_ONE_CE_CNT >> 1)
    #else
        #define CACHE_FLUSH_THRESHOLD       CACHE_FLUSH_ONE_CE_CNT
    #endif
    #define CACHE_FILLUP_THRESHOLD          (MAX_DESC_PER_TRK / (MAX_SSD_PER_FILLUP / MAX_SSD_PER_AU))
#endif

//-----------------------------------------------------------------------------
#define CACHE_HASH_SHIFT                    11
#define CACHE_HASH_COUNT                    (1 << CACHE_HASH_SHIFT)
#define CACHE_HASH_MASK                     (CACHE_HASH_COUNT - 1)

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define GET_SSD_PTR_FROM_INDEX(I)           (&smSSD[I])
#define GET_SSD_INDEX_FROM_PTR(P)           (P - smSSD)
#define GET_SSD_NEXT_INDEX_FROM_INDEX(I)    ((tSSD*)&smSSD[I])->nextIndex

#define GET_CACHE_HASH_CODE(L)              (L & CACHE_HASH_MASK)
//#define GET_CACHE_HASH_CODE(L)              ((L + (L >> CACHE_HASH_SHIFT)) & CACHE_HASH_MASK)

#define CACHE_FE_SSD_PROTECT_START          //BEGIN_CS1
#define CACHE_FE_SSD_PROTECT_END            //END_CS1

#define CACHE_BE_SSD_PROTECT_START          //BEGIN_CS1
#define CACHE_BE_SSD_PROTECT_END            //END_CS1

#if _CPU_==CPU_ID0
    #define REG_CPU0_FAST_TCM(V,C,T)        {V = (T*)&gFastCpu0TcmBuf[gFastCpu0TcmUsedSize];   gFastCpu0TcmUsedSize += (C * sizeof(T));}
#elif _CPU_==CPU_ID1
    #define REG_CPU1_FAST_TCM(V,C,T)        {V = (T*)&gFastCpu1TcmBuf[gFastCpu1TcmUsedSize];   gFastCpu1TcmUsedSize += (C * sizeof(T));}
#elif _CPU_==CPU_ID2
    #define REG_CPU2_FAST_TCM(V,C,T)        {V = (T*)&gFastCpu2TcmBuf[gFastCpu2TcmUsedSize];   gFastCpu2TcmUsedSize += (C * sizeof(T));}
#endif

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef struct
{
    U32 laa;        // logical allocaion address
    U16 next;       // next cache index for hash table
    U16 prev;       // previous cache index for hash table
    U16 bitmap;     // cache bitmap for fillup
    U8  status;     // cache status
    U8  pool;       // pool number
}CacheEntry_t;

typedef struct
{
    CacheEntry_t    cacheEntry[CACHE_TOTAL_COUNT];

    volatile U32    forceFlush;
    volatile U32    forceFillup;

    tSSD*           head[SSD_POOL_COUNT];
    tSSD*           tail[SSD_POOL_COUNT];
    volatile U32    ssdCnt[SSD_POOL_COUNT];
}CacheManager_t;

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern U8*             smCacheRamDrive;
extern U8*             smCache;
extern tSSD*           smSSD;
extern CacheManager_t  smCacheMgr;

extern const     U16   gSLaaBitMask[16];
extern const     U16   gELaaBitMask[16];

#if _CPU_==CPU_ID0
    extern       U8    gFastCpu0TcmBuf[FAST_CPU0_TCM_BUF_SIZE];
    extern       U8*   gFastDataSetBuf;
#elif _CPU_==CPU_ID1
    extern       U8    gFastCpu1TcmBuf[FAST_CPU1_TCM_BUF_SIZE];
    extern       tPAA* gFastPaaBuf;
#elif _CPU_==CPU_ID2
    extern       U8    gFastCpu2TcmBuf[FAST_CPU2_TCM_BUF_SIZE];
    extern       tPAA* gFastPaaBuf;
    extern       U32*  gFastLaaBuf;
    extern       U32*  gFastAuxBuf;
    extern       U16   gCacheHashTable[CACHE_HASH_COUNT];
    extern       void (*gCbfCacheXferDone[CACHE_STATUS_COUNT])(MsgCacheRelObj_t* pHcmdMsg);
    extern       void (*gCbfCacheAlloc[CACHE_STATUS_COUNT])(MsgHostIO_t * pHcmdMsg);
    extern       void (*gCbfCacheTrkDone[CACHE_STATUS_COUNT])(tFIO_JOB* pJob);
    extern       void (*gCbfCacheTrkError[CACHE_STATUS_COUNT])(tFIO_JOB* pJob);
#endif

#if (_CPU_==CPU_ID1) || (_CPU_==CPU_ID2)
    extern const U32   gBmp2GroupMap[0x100];
#endif

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t Cache_OneTimeInit(InitBootMode_t initMode);
void    Cache_PopSSD_Other(U16 pool, tSSD* pHead, tSSD* pTail, U16 Count);
void    Cache_PushSSD_Other(U16 pool, tSSD* pHead, tSSD* pTail, U16 Count);
U16     Cache_Alloc_FillupSSD(tFIO_JOB* pJob, U16 cacheIdx, U16 otherIdx, U16* paaPerFillup);
U64     Cache_Convert_Bitmap(U16 bitmap);

#if _CPU_==CPU_ID2
    void Cache_Reset(U32 mode);
    void Cache_DumpInfo(U16 level);

    //-------------------------------------------------------------
    void Cache_XferDone_Exception(MsgCacheRelObj_t* pHcmdMsg);
    void Cache_XferDone_Auto(MsgCacheRelObj_t* pHcmdMsg);
    void Cache_XferDone_ReadFullHitBlank(MsgCacheRelObj_t* pHcmdMsg);
    void Cache_XferDone_ReadFullHit(MsgCacheRelObj_t* pHcmdMsg);
    void Cache_XferDone_WriteNoHit(MsgCacheRelObj_t* pHcmdMsg);
    void Cache_XferDone_WriteFua(MsgCacheRelObj_t* pHcmdMsg);
    void Cache_XferDone_Abort(MsgCacheRelObj_t* pHcmdMsg);

    //-------------------------------------------------------------
    void Cache_Alloc_Exception(MsgHostIO_t* pHcmdMsg);
    void Cache_Alloc_Auto(tFIO_JOB* pJob);
    void Cache_Alloc_TCG(tFIO_JOB* pJob);
    void Cache_Alloc_ReadFillupHit(U16 cacheIdx);
    void Cache_Alloc_Fillup(void);
    void Cache_Alloc_ReadNoHit(MsgHostIO_t* pHcmdMsg);
    void Cache_Alloc_ReadIDS(MsgHostIO_t* pHcmdMsg);
    void Cache_Alloc_ReadFullHit(MsgHostIO_t* pHcmdMsg);
    void Cache_Alloc_ReadPartialHit(MsgHostIO_t* pHcmdMsg);
    void Cache_Alloc_ReadFullHitBlank(MsgHostIO_t* pHcmdMsg);
    void Cache_Alloc_PreRead(MsgHostIO_t* pHcmdMsg);
    void Cache_Alloc_WriteNoHit(MsgHostIO_t* pHcmdMsg);
    void Cache_Alloc_WriteFullHit(MsgHostIO_t* pHcmdMsg);
    void Cache_Alloc_WritePartialHit(MsgHostIO_t* pHcmdMsg);

    void Cache_TrkDone_Exception(tFIO_JOB* pJob);
    void Cache_TrkDone_Auto(tFIO_JOB* pJob);
    void Cache_TrkDone_SysInfo(tFIO_JOB* pJob);
    void Cache_TrkDone_ReadNoHit(tFIO_JOB* pJob);
    void Cache_TrkDone_ReadIDS(tFIO_JOB* pJob);
    void Cache_TrkDone_ReadPartialHit(tFIO_JOB* pJob);
    void Cache_TrkDone_ReadFillupHit(tFIO_JOB* pJob);
    void Cache_TrkDone_Flush(tFIO_JOB* pJob);
    void Cache_TrkDone_GCReadAux(tFIO_JOB* pJob);
    void Cache_TrkDone_GCRead(tFIO_JOB* pJob);
    void Cache_TrkDone_GCLastPage(tFIO_JOB * pJob);
    void Cache_TrkDone_PreRead(tFIO_JOB* pJob);
    void Cache_TrkDone_DoNothing(tFIO_JOB* pJob);

    //-------------------------------------------------------------
    void Cache_TrkError_Exception(tFIO_JOB* pJob);
    void Cache_TrkError_Auto(tFIO_JOB* pJob);
    void Cache_TrkError_ReadIDS(tFIO_JOB* pJob);
    void Cache_TrkError_ReadNoHit(tFIO_JOB* pJob);
    void Cache_TrkError_Flush(tFIO_JOB* pJob);
    void Cache_TrkError_SysInfo(tFIO_JOB* pJob);

    //-------------------------------------------------------------
    void Cache_PopSSD(U16 pool, tSSD* pStartSSD, tSSD* pEndSSD, U16 Count);
    void Cache_PushSSD(U16 pool, tSSD* pStartSSD, tSSD* pEndSSD, U16 Count);

    //-------------------------------------------------------------
    void Cache_MergePool(U16 poolFrom, U16 poolTo);
    U32  Cache_WriteZero(U32 laa, U16 bitmap);
    tPAA Cache_GetZeroPAA(U32 laa);
#endif

//-----------------------------------------------------------------------------
//  Inline Function
//-----------------------------------------------------------------------------

#if _CPU_==CPU_ID2
//-----------------------------------------------------------------------------
/**
    Search cache from hash lookup table

    @param[in]  laa

    @return cache index, 0 for no hit
**/
//-----------------------------------------------------------------------------
__inline U16 Cache_Search(U32 laa)
{
    U16 currIdx;

    currIdx = gCacheHashTable[GET_CACHE_HASH_CODE(laa)];
    while (currIdx)
    {
        if (smCacheMgr.cacheEntry[currIdx].laa == laa)
        {
            return currIdx;
        }

        currIdx = smCacheMgr.cacheEntry[currIdx].next;
    }

    return 0;
}

//-----------------------------------------------------------------------------
/**
    Insert cache into hash lookup table

    @param[in]  cache index
**/
//-----------------------------------------------------------------------------
__inline void Cache_Insert(U16 cacheIdx)
{
    U16 hashCode;

    hashCode = GET_CACHE_HASH_CODE(smCacheMgr.cacheEntry[cacheIdx].laa);

    if (gCacheHashTable[hashCode])
    {
        U16 headIdx = gCacheHashTable[hashCode];
        smCacheMgr.cacheEntry[headIdx].prev  = cacheIdx;
        smCacheMgr.cacheEntry[cacheIdx].next = headIdx;
    }

    gCacheHashTable[hashCode] = cacheIdx;
}

//-----------------------------------------------------------------------------
/**
    Delete cache from hash lookup table

    @param[in]  cache index
**/
//-----------------------------------------------------------------------------
__inline void Cache_Delete(U16 cacheIdx)
{
    U16 nextIdx;
    U16 prevIdx;

    nextIdx = smCacheMgr.cacheEntry[cacheIdx].next;
    prevIdx = smCacheMgr.cacheEntry[cacheIdx].prev;

    if (prevIdx == 0 && nextIdx == 0)
    {
        gCacheHashTable[GET_CACHE_HASH_CODE(smCacheMgr.cacheEntry[cacheIdx].laa)] = 0;
    }
    else if (prevIdx == 0)
    {
        gCacheHashTable[GET_CACHE_HASH_CODE(smCacheMgr.cacheEntry[cacheIdx].laa)] = nextIdx;
        smCacheMgr.cacheEntry[nextIdx].prev  = prevIdx;

        smCacheMgr.cacheEntry[cacheIdx].next = 0;
    }
    else if (nextIdx == 0)
    {
        smCacheMgr.cacheEntry[prevIdx].next  = nextIdx;
        smCacheMgr.cacheEntry[cacheIdx].prev = 0;
    }
    else
    {
        smCacheMgr.cacheEntry[nextIdx].prev = prevIdx;
        smCacheMgr.cacheEntry[prevIdx].next = nextIdx;

        smCacheMgr.cacheEntry[cacheIdx].prev = 0;
        smCacheMgr.cacheEntry[cacheIdx].next = 0;
    }
}
#endif
