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
//! @brief cache code
//! Evan: Please make cache code as simple/clean as possible!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"

#if _CPU_==CPU_ID0
    #include "NvmeCmds.h"
#elif _CPU_==CPU_ID2
    #include "SysInfo.h"
    #include "ErrHandle.h"
    #include "Ftl.h"
    #include "Gc.h"
    #include "IDS.h"
    #include "Fio.h"
    #include "Core.h"
    #include "CmdSensor.h"
#endif

#include "Cache.h"

#include "PerfSection.h"   // CODE AND DATA MEMORY MAPPING
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
__DATA_PERF_SH U8*              smCacheRamDrive;    ///< Device Cache Ram Disk buffer
__DATA_PERF_SH tSSD*            smSSD;              ///< SSD Table
__DATA_PERF_SH U8*              smCache;            ///< Device Cache buffer
__DATA_PERF_SH CacheManager_t   smCacheMgr;         ///< Cache Manager
__DATA_PERF_SH U32*             smCacheCompare;     ///< Device cahce for NVMe compare cmd

/*####################################################################
##      ##    ###    ########  ##    ## #### ##    ##  ######   ####
##  ##  ##   ## ##   ##     ## ###   ##  ##  ###   ## ##    ##  ####
##  ##  ##  ##   ##  ##     ## ####  ##  ##  ####  ## ##        ####
##  ##  ## ##     ## ########  ## ## ##  ##  ## ## ## ##   ####  ##
##  ##  ## ######### ##   ##   ##  ####  ##  ##  #### ##    ##
##  ##  ## ##     ## ##    ##  ##   ###  ##  ##   ### ##    ##  ####
 ###  ###  ##     ## ##     ## ##    ## #### ##    ##  ######   ####
#####################################################################*/
// Change "gCbfCacheXferDone", "gCbfCacheTrkDone", "gCbfCacheTrkError" function pointer order at your own risk!

const U16  gSLaaBitMask[16] = { 0xFFFF & CACHE_BMP_ALL_VALID, 0xFFFE & CACHE_BMP_ALL_VALID,
                                0xFFFC & CACHE_BMP_ALL_VALID, 0xFFF8 & CACHE_BMP_ALL_VALID,
                                0xFFF0 & CACHE_BMP_ALL_VALID, 0xFFE0 & CACHE_BMP_ALL_VALID,
                                0xFFC0 & CACHE_BMP_ALL_VALID, 0xFF80 & CACHE_BMP_ALL_VALID,
                                0xFF00 & CACHE_BMP_ALL_VALID, 0xFE00 & CACHE_BMP_ALL_VALID,
                                0xFC00 & CACHE_BMP_ALL_VALID, 0xF800 & CACHE_BMP_ALL_VALID,
                                0xF000 & CACHE_BMP_ALL_VALID, 0xE000 & CACHE_BMP_ALL_VALID,
                                0xC000 & CACHE_BMP_ALL_VALID, 0x8000 & CACHE_BMP_ALL_VALID };

const U16  gELaaBitMask[16] = { 0x0001 & CACHE_BMP_ALL_VALID, 0x0003 & CACHE_BMP_ALL_VALID,
                                0x0007 & CACHE_BMP_ALL_VALID, 0x000F & CACHE_BMP_ALL_VALID,
                                0x001F & CACHE_BMP_ALL_VALID, 0x003F & CACHE_BMP_ALL_VALID,
                                0x007F & CACHE_BMP_ALL_VALID, 0x00FF & CACHE_BMP_ALL_VALID,
                                0x01FF & CACHE_BMP_ALL_VALID, 0x03FF & CACHE_BMP_ALL_VALID,
                                0x07FF & CACHE_BMP_ALL_VALID, 0x0FFF & CACHE_BMP_ALL_VALID,
                                0x1FFF & CACHE_BMP_ALL_VALID, 0x3FFF & CACHE_BMP_ALL_VALID,
                                0x7FFF & CACHE_BMP_ALL_VALID, 0xFFFF & CACHE_BMP_ALL_VALID };

#if _CPU_==CPU_ID0
    //--- Beware of CPU0 fast tcm buffer size, use it wisely!
    U8   gFastCpu0TcmBuf[FAST_CPU0_TCM_BUF_SIZE];
    U32  gFastCpu0TcmUsedSize;
    U8*  gFastDataSetBuf;

#elif _CPU_ ==CPU_ID1
    //--- Beware of CPU1 fast tcm buffer size, use it wisely!
    U8    gFastCpu1TcmBuf[FAST_CPU1_TCM_BUF_SIZE];
    U32   gFastCpu1TcmUsedSize;
    tPAA* gFastPaaBuf;

#elif _CPU_==CPU_ID2
    #define Cache_XferDone_ReadPartialHit   Cache_XferDone_ReadFullHit
    #define Cache_XferDone_WritePartialHit  Cache_XferDone_WriteNoHit
    #define Cache_XferDone_WriteFullHit     Cache_XferDone_WriteNoHit
    #define Cache_XferDone_ReadIDS          Cache_XferDone_ReadFullHitBlank

    void (*gCbfCacheXferDone[CACHE_STATUS_COUNT])(MsgCacheRelObj_t* pHcmdMsg) =
    {
        Cache_XferDone_ReadFullHit,         // CACHE_R_NO_HIT
        Cache_XferDone_ReadPartialHit,      // CACHE_R_PARTIAL_HIT
        Cache_XferDone_ReadFullHit,         // CACHE_R_FULL_HIT
        Cache_XferDone_ReadFullHitBlank,    // CACHE_R_FULL_HIT_BLANK
        Cache_XferDone_Exception,           // CACHE_R_FILLUP_HIT
        //------ 5
        Cache_XferDone_ReadIDS,             // CACHE_R_IDS
        Cache_XferDone_WriteNoHit,          // CACHE_W_NO_HIT
        Cache_XferDone_WritePartialHit,     // CACHE_W_PARTIAL_HIT
        Cache_XferDone_WriteFullHit,        // CACHE_W_FULL_HIT
        Cache_XferDone_WriteFua,            // CACHE_W_FUA
        //------ 10
        Cache_XferDone_Exception,           // CACHE_FLUSH
        Cache_XferDone_Exception,           // CACHE_FILLUP
        Cache_XferDone_Exception,           // CACHE_GCREADAUX
        Cache_XferDone_Exception,           // CACHE_GCREAD
        Cache_XferDone_Exception,           // CACHE_GCLASTPAGE
        //------ 15
        Cache_XferDone_Exception,           // CACHE_PREREAD
        Cache_XferDone_Exception,           // CACHE_PREOPEN
        Cache_XferDone_Exception,           // CACHE_LASTPAGE
        Cache_XferDone_Exception,           // CACHE_SCANPAGE
        Cache_XferDone_Exception,           // CACHE_TCG
        //------ 20
        Cache_XferDone_Exception,           // CACHE_SysInfo
        Cache_XferDone_Exception,           // CACHE_WRDUMMY
        Cache_XferDone_Auto,                // CACHE_AUTO
        Cache_XferDone_Abort,               // CACHE_ABORT
    };

    void (*gCbfCacheAlloc[CACHE_STATUS_COUNT])(MsgHostIO_t* pHcmdMsg) =
    {
        Cache_Alloc_ReadNoHit,              // CACHE_R_NO_HIT
        Cache_Alloc_ReadPartialHit,         // CACHE_R_PARTIAL_HIT
        Cache_Alloc_ReadFullHit,            // CACHE_R_FULL_HIT
        Cache_Alloc_ReadFullHitBlank,       // CACHE_R_FULL_HIT_BLANK
        Cache_Alloc_Exception,              // CACHE_R_FILLUP_HIT
        //------ 5
        Cache_Alloc_ReadIDS,                // CACHE_R_IDS
        Cache_Alloc_WriteNoHit,             // CACHE_W_NO_HIT
        Cache_Alloc_WritePartialHit,        // CACHE_W_PARTIAL_HIT
        Cache_Alloc_WriteFullHit,           // CACHE_W_FULL_HIT
        Cache_Alloc_Exception,              // CACHE_W_FUA
        //------ 10
        Cache_Alloc_Exception,              // CACHE_FLUSH
        Cache_Alloc_Exception,              // CACHE_FILLUP
        Cache_Alloc_Exception,              // CACHE_GCREADAUX
        Cache_Alloc_Exception,              // CACHE_GCREAD
        Cache_Alloc_Exception,              // CACHE_GCLASTPAGE
        //------ 15
        Cache_Alloc_PreRead,                // CACHE_PREREAD
        Cache_Alloc_PreRead,                // CACHE_PREOPEN
        Cache_Alloc_Exception,              // CACHE_LASTPAGE
        Cache_Alloc_Exception,              // CACHE_SCANPAGE
        Cache_Alloc_Exception,              // CACHE_TCG
        //------ 20
        Cache_Alloc_Exception,              // CACHE_SYSINFO
        Cache_Alloc_Exception,              // CACHE_WRDUMMY
        Cache_Alloc_Exception,              // CACHE_AUTO
        Cache_Alloc_Exception,              // CACHE_ABORT
    };

    U16 gCacheHashTable[CACHE_HASH_COUNT];

    //--- Beware of CPU2 fast tcm buffer size, use it wisely!
    U8    gFastCpu2TcmBuf[FAST_CPU2_TCM_BUF_SIZE];
    U32   gFastCpu2TcmUsedSize;
    tPAA* gFastPaaBuf;
    U32*  gFastLaaBuf;
    U32*  gFastAuxBuf;

    #define Cache_TrkDone_Fillup    Cache_TrkDone_ReadFillupHit
    #define Cache_TrkDone_LastPage  Cache_TrkDone_Auto
    #define Cache_TrkDone_TCG       Cache_TrkDone_Auto
    #define Cache_TrkDone_ScanPage  Cache_TrkDone_Auto
    #define Cache_TrkDone_WrDummy   Cache_TrkDone_Auto
    #define Cache_TrkDone_ReadIDS   Cache_TrkDone_DoNothing
    #define Cache_TrkDone_PreOpen   Cache_TrkDone_DoNothing
    #define Cache_TrkDone_WriteFua  Cache_TrkDone_DoNothing

    void (*gCbfCacheTrkDone[CACHE_STATUS_COUNT])(tFIO_JOB* pJob) =
    {
        Cache_TrkDone_ReadNoHit,            // CACHE_R_NO_HIT
        Cache_TrkDone_ReadPartialHit,       // CACHE_R_PARTIAL_HIT
        Cache_TrkDone_Exception,            // CACHE_R_FULL_HIT
        Cache_TrkDone_Exception,            // CACHE_R_FULL_HIT_BLANK
        Cache_TrkDone_ReadFillupHit,        // CACHE_R_FILLUP_HIT
        //------ 5
        Cache_TrkDone_ReadIDS,              // CACHE_R_IDS
        Cache_TrkDone_Exception,            // CACHE_W_NO_HIT
        Cache_TrkDone_Exception,            // CACHE_W_PARTIAL_HIT
        Cache_TrkDone_Exception,            // CACHE_W_FULL_HIT
        Cache_TrkDone_WriteFua,             // CACHE_W_FUA
        //------ 10
        Cache_TrkDone_Flush,                // CACHE_FLUSH
        Cache_TrkDone_Fillup,               // CACHE_FILLUP
        Cache_TrkDone_GCReadAux,            // CACHE_GCREADAUX
        Cache_TrkDone_GCRead,               // CACHE_GCREAD
        Cache_TrkDone_GCLastPage,           // CACHE_GCLASTPAGE
        //------ 15
        Cache_TrkDone_PreRead,              // CACHE_PREREAD
        Cache_TrkDone_PreOpen,              // CACHE_PREOPEN
        Cache_TrkDone_LastPage,             // CACHE_LASTPAGE
        Cache_TrkDone_ScanPage,             // CACHE_SCANPAGE
        Cache_TrkDone_TCG,                  // CACHE_TCG
        //------ 20
        Cache_TrkDone_SysInfo,              // CACHE_SYSINFO
        Cache_TrkDone_WrDummy,              // CACHE_WRDUMMY
        Cache_TrkDone_Auto,                 // CACHE_AUTO
        Cache_TrkDone_Exception,            // CACHE_ABORT
    };

    #define Cache_TrkError_ReadPartialHit   Cache_TrkError_ReadNoHit
    #define Cache_TrkError_ReadFillupHit    Cache_TrkError_ReadNoHit
    #define Cache_TrkError_PreRead          Cache_TrkError_ReadNoHit
    #define Cache_TrkError_Fillup           Cache_TrkError_ReadNoHit
    #define Cache_TrkError_GCReadAux        Cache_TrkError_ReadNoHit
    #define Cache_TrkError_GCRead           Cache_TrkError_ReadNoHit
    #define Cache_TrkError_GCLastPage       Cache_TrkError_ReadNoHit
    #define Cache_TrkError_LastPage         Cache_TrkError_Auto
    #define Cache_TrkError_ScanPage         Cache_TrkError_Auto
    #define Cache_TrkError_TCG              Cache_TrkError_Auto
    #define Cache_TrkError_WrDummy          Cache_TrkError_Auto
    #define Cache_TrkError_WriteFua         Cache_TrkError_Auto

    void (*gCbfCacheTrkError[CACHE_STATUS_COUNT])(tFIO_JOB* pJob) =
    {
        Cache_TrkError_ReadNoHit,            // CACHE_R_NO_HIT
        Cache_TrkError_ReadPartialHit,       // CACHE_R_PARTIAL_HIT
        Cache_TrkError_Exception,            // CACHE_R_FULL_HIT
        Cache_TrkError_Exception,            // CACHE_R_FULL_HIT_BLANK
        Cache_TrkError_ReadFillupHit,        // CACHE_R_FILLUP_HIT
        //------ 5
        Cache_TrkError_ReadIDS,              // CACHE_R_IDS
        Cache_TrkError_Exception,            // CACHE_W_NO_HIT
        Cache_TrkError_Exception,            // CACHE_W_PARTIAL_HIT
        Cache_TrkError_Exception,            // CACHE_W_FULL_HIT
        Cache_TrkError_WriteFua,             // CACHE_W_FUA
        //------ 10
        Cache_TrkError_Flush,                // CACHE_FLUSH
        Cache_TrkError_Fillup,               // CACHE_FILLUP
        Cache_TrkError_GCReadAux,            // CACE_GCREADAUX
        Cache_TrkError_GCRead,               // CACHE_GCREAD
        Cache_TrkError_GCLastPage,           // CACHE_GCLASTPAGE
        //------ 15
        Cache_TrkError_PreRead,              // CACHE_PREREAD
        Cache_TrkError_Exception,            // CACHE_PROPEN
        Cache_TrkError_LastPage,             // CACHE_LASTPAGE
        Cache_TrkError_ScanPage,             // CACHE_SCANPAGE
        Cache_TrkError_TCG,                  // CACHE_TCG
        //------ 20
        Cache_TrkError_SysInfo,              // CACHE_SYSINFO
        Cache_TrkError_WrDummy,              // CACHE_WRDUMMY
        Cache_TrkError_Auto,                 // CACHE_AUTO
        Cache_TrkError_Exception,            // CACHE_ABORT
    };
#endif

#if (_CPU_==CPU_ID1) || (_CPU_==CPU_ID2)
const U32 gBmp2GroupMap[0x100]=
{
       0x8,      0x71,     0x611,      0x62,     0x512,    0x5111,     0x521,      0x53,  // [00] ~ [07]
     0x413,    0x4121,   0x41111,    0x4112,     0x422,    0x4211,     0x431,      0x44,  // [08] ~ [0F]
     0x314,    0x3131,   0x31211,    0x3122,   0x31112,  0x311111,   0x31121,    0x3113,  // [10] ~ [17]
     0x323,    0x3221,   0x32111,    0x3212,     0x332,    0x3311,     0x341,      0x35,  // [18] ~ [1F]
     0x215,    0x2141,   0x21311,    0x2132,   0x21212,  0x212111,   0x21221,    0x2123,  // [20] ~ [27]
   0x21113,  0x211121, 0x2111111,  0x211112,   0x21122,  0x211211,   0x21131,    0x2114,  // [28] ~ [2F]
     0x224,    0x2231,   0x22211,    0x2222,   0x22112,  0x221111,   0x22121,    0x2213,  // [30] ~ [37]
     0x233,    0x2321,   0x23111,    0x2312,     0x242,    0x2411,     0x251,      0x26,  // [38] ~ [3F]
     0x116,    0x1151,   0x11411,    0x1142,   0x11312,  0x113111,   0x11321,    0x1133,  // [40] ~ [47]
   0x11213,  0x112121, 0x1121111,  0x112112,   0x11222,  0x112211,   0x11231,    0x1124,  // [48] ~ [4F]
   0x11114,  0x111131, 0x1111211,  0x111122, 0x1111112,0x11111111, 0x1111121,  0x111113,  // [50] ~ [57]
   0x11123,  0x111221, 0x1112111,  0x111212,   0x11132,  0x111311,   0x11141,    0x1115,  // [58] ~ [5F]
     0x125,    0x1241,   0x12311,    0x1232,   0x12212,  0x122111,   0x12221,    0x1223,  // [60] ~ [67]
   0x12113,  0x121121, 0x1211111,  0x121112,   0x12122,  0x121211,   0x12131,    0x1214,  // [68] ~ [6F]
     0x134,    0x1331,   0x13211,    0x1322,   0x13112,  0x131111,   0x13121,    0x1313,  // [70] ~ [77]
     0x143,    0x1421,   0x14111,    0x1412,     0x152,    0x1511,     0x161,      0x17,  // [78] ~ [7F]
      0x17,     0x161,    0x1511,     0x152,    0x1412,   0x14111,    0x1421,     0x143,  // [80] ~ [87]
    0x1313,   0x13121,  0x131111,   0x13112,    0x1322,   0x13211,    0x1331,     0x134,  // [88] ~ [8F]
    0x1214,   0x12131,  0x121211,   0x12122,  0x121112, 0x1211111,  0x121121,   0x12113,  // [90] ~ [97]
    0x1223,   0x12221,  0x122111,   0x12212,    0x1232,   0x12311,    0x1241,     0x125,  // [98] ~ [9F]
    0x1115,   0x11141,  0x111311,   0x11132,  0x111212, 0x1112111,  0x111221,   0x11123,  // [A0] ~ [A7]
  0x111113, 0x1111121,0x11111111, 0x1111112,  0x111122, 0x1111211,  0x111131,   0x11114,  // [A8] ~ [AF]
    0x1124,   0x11231,  0x112211,   0x11222,  0x112112, 0x1121111,  0x112121,   0x11213,  // [B0] ~ [B7]
    0x1133,   0x11321,  0x113111,   0x11312,    0x1142,   0x11411,    0x1151,     0x116,  // [B8] ~ [BF]
      0x26,     0x251,    0x2411,     0x242,    0x2312,   0x23111,    0x2321,     0x233,  // [C0] ~ [C7]
    0x2213,   0x22121,  0x221111,   0x22112,    0x2222,   0x22211,    0x2231,     0x224,  // [C8] ~ [CF]
    0x2114,   0x21131,  0x211211,   0x21122,  0x211112, 0x2111111,  0x211121,   0x21113,  // [D0] ~ [D7]
    0x2123,   0x21221,  0x212111,   0x21212,    0x2132,   0x21311,    0x2141,     0x215,  // [D8] ~ [DF]
      0x35,     0x341,    0x3311,     0x332,    0x3212,   0x32111,    0x3221,     0x323,  // [E0] ~ [E7]
    0x3113,   0x31121,  0x311111,   0x31112,    0x3122,   0x31211,    0x3131,     0x314,  // [E8] ~ [EF]
      0x44,     0x431,    0x4211,     0x422,    0x4112,   0x41111,    0x4121,     0x413,  // [F0] ~ [F7]
      0x53,     0x521,    0x5111,     0x512,      0x62,     0x611,      0x71,       0x8,  // [F8] ~ [FF]
};
#endif

//-----------------------------------------------------------------------------
//  Imported external variable
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
    Initial and allocate cache variables

    @param[in]  initMode

    @return cEcNoError
**/
//-----------------------------------------------------------------------------
Error_t Cache_OneTimeInit(InitBootMode_t initMode)
{
    #if (_CPU_==CPU_ID0)

        #if _MEDIA_==MEDIA_DRAM

            U16 ssdIdx;

            smCache         = MEM_AllocBuffer(CACHE_TOTAL_SIZE, 512);
            smSSD           = MEM_AllocSspm(NUM_OF_SSD * sizeof(tSSD), 64);
            smCacheCompare  = MEM_AllocBuffer(NVME_MAX_DATA_TRANSFER_SIZE + MBU_SIZE, 512);
            smCacheRamDrive = MEM_AllocBuffer(RAM_DISK_SIZE, 512);

            for (ssdIdx = 0; ssdIdx < HW_MAX_IO_CMD_INDEX; ssdIdx++)
            {
                smSSD[ssdIdx].endAddr     = 0xFFFFFFF0;
            }

            smSSD[SSD_IF_START].startAddr =   (U32)smCache                     >> SSD_ADDR_SHIFT;
            smSSD[SSD_IF_START].endAddr   = (((U32)smCache + CACHE_TOTAL_SIZE) >> SSD_ADDR_SHIFT) - 1;

        #else

            smCache         = MEM_AllocBuffer(CACHE_TOTAL_SIZE, 512);
            smSSD           = MEM_AllocSspm(NUM_OF_SSD * sizeof(tSSD), 64);
            smCacheCompare  = MEM_AllocBuffer(NVME_MAX_DATA_TRANSFER_SIZE + MBU_SIZE, 512);

        #endif

        gFastCpu0TcmUsedSize = 0;
        REG_CPU0_FAST_TCM(gFastDataSetBuf, cNvmeDataSetBufSize, U8);
        ASSERT(gFastCpu0TcmUsedSize <= FAST_CPU0_TCM_BUF_SIZE);

    #elif (_CPU_==CPU_ID1)
        gFastCpu1TcmUsedSize = 0;

        REG_CPU1_FAST_TCM(gFastPaaBuf, MAX_PAA_PER_JOB, tPAA);
        ASSERT(gFastCpu1TcmUsedSize <= FAST_CPU1_TCM_BUF_SIZE);

    #elif (_CPU_==CPU_ID2)

        Cache_Reset(CACHE_RST_MODE_FORCE);

        gFastCpu2TcmUsedSize = 0;

        REG_CPU2_FAST_TCM(gFastPaaBuf, MAX_PAA_PER_JOB, tPAA);
        REG_CPU2_FAST_TCM(gFastLaaBuf, MAX_PAA_PER_JOB,  U32);
        REG_CPU2_FAST_TCM(gFastAuxBuf, MAX_AUX_PER_JOB, U32);

        MEM_SET(gFastAuxBuf, BYTE_MASK, MAX_AUX_PER_JOB * sizeof(U32));

        ASSERT(gFastCpu2TcmUsedSize <= FAST_CPU2_TCM_BUF_SIZE);
        ASSERT((CACHE_FLUSH_THRESHOLD * 2 + NVME_MAX_DATA_TRANSFER_SIZE / MBU_SIZE) < CACHE_RW_COUNT);

    #endif

    return cEcNoError;
}

#if _CPU_==CPU_ID2
//-----------------------------------------------------------------------------
/**
    Reset all cache or RW cache and hash table only

    @param[in]  cache reset mode
**/
//-----------------------------------------------------------------------------
void Cache_Reset(U32 mode)
{
    if (mode == CACHE_RST_MODE_FORCE)
    {
        U16 idx;

        MEM_CLR(&smCacheMgr, sizeof(smCacheMgr));

        //--- init NULL SSD ----------------
        smSSD[SSD_NULL_START].prevIndex = smSSD[SSD_NULL_START].nextIndex = SSD_NULL_START;
        smCacheMgr.cacheEntry[SSD_NULL_START].laa = DWORD_MASK;

        smSSD[SSD_NULL_START].startAddr =  ((U32)smCache +  CACHE_NULL_START                     * MBU_SIZE) >> SSD_ADDR_SHIFT;
        smSSD[SSD_NULL_START].endAddr   = (((U32)smCache + (CACHE_NULL_START + CACHE_NULL_COUNT) * MBU_SIZE) >> SSD_ADDR_SHIFT) - 1;

        //--- init R/W SSD & Cache ----------
        for (idx = SSD_RW_START; idx <= SSD_RW_END; idx++)
        {
            if (idx == SSD_RW_START)
            {
                smSSD[idx].prevIndex = SSD_NULL_START;
                smSSD[idx].nextIndex = idx + 1;
            }
            else if (idx == SSD_RW_END)
            {
                smSSD[idx].prevIndex = idx - 1;
                smSSD[idx].nextIndex = SSD_NULL_START;
            }
            else
            {
                smSSD[idx].prevIndex = idx - 1;
                smSSD[idx].nextIndex = idx + 1;
            }

            smSSD[idx].startAddr =  ((U32)smCache +  idx      * MBU_SIZE) >> SSD_ADDR_SHIFT;
            smSSD[idx].endAddr   = (((U32)smCache + (idx + 1) * MBU_SIZE) >> SSD_ADDR_SHIFT) - 1;

            smCacheMgr.cacheEntry[idx].status = 0;
            smCacheMgr.cacheEntry[idx].pool   = SSD_POOL_FREE;
            smCacheMgr.cacheEntry[idx].laa    = DWORD_MASK;
        }

        smCacheMgr.head[SSD_POOL_FREE]   = &smSSD[SSD_RW_START];
        smCacheMgr.tail[SSD_POOL_FREE]   = &smSSD[SSD_RW_END];
        smCacheMgr.ssdCnt[SSD_POOL_FREE] = SSD_RW_COUNT;

        //--- init ZERO SSD & Cache -------
        for (idx = SSD_ZERO_START; idx <= CACHE_ZERO_END; idx++)
        {
            MEM_CLR(&smCache[idx * MBU_SIZE], MBU_SIZE);

            smSSD[idx].prevIndex = smSSD[idx].nextIndex = idx;
            smSSD[idx].startAddr =  ((U32)smCache +  idx      * MBU_SIZE) >> SSD_ADDR_SHIFT;
            smSSD[idx].endAddr   = (((U32)smCache + (idx + 1) * MBU_SIZE) >> SSD_ADDR_SHIFT) - 1;
            smCacheMgr.cacheEntry[idx].laa = DWORD_MASK;
        }

        //--- init Dummy SSD --------------
        smSSD[SSD_DUMMY_START].prevIndex = smSSD[SSD_DUMMY_START].nextIndex = SSD_DUMMY_START;
        smSSD[SSD_DUMMY_START].startAddr =  ((U32)smCache +  CACHE_DUMMY_START      * MBU_SIZE) >> SSD_ADDR_SHIFT;
        smSSD[SSD_DUMMY_START].endAddr   = (((U32)smCache + (CACHE_DUMMY_START + 1) * MBU_SIZE) >> SSD_ADDR_SHIFT) - 1;

        smCacheMgr.cacheEntry[SSD_DUMMY_START].laa = DWORD_MASK;

        for (idx = SSD_IF_START; idx <= SSD_IF_END; idx++)
        {
            smSSD[idx].prevIndex = idx - 1;
            smSSD[idx].nextIndex = idx + 1;
            smSSD[idx].startAddr =  ((U32)smCache + (idx)     * MBU_SIZE) >> SSD_ADDR_SHIFT;
            smSSD[idx].endAddr   = (((U32)smCache + (idx + 1) * MBU_SIZE) >> SSD_ADDR_SHIFT) - 1;
        }

        /*-------------------------------------------------
        - Initialization: SSD without Cache
        ---------------------------------------------------*/
        //--- init nvme operation SSD
        smSSD[SSD_IF_START].prevIndex  = SSD_IF_END;
        smSSD[SSD_IF_END].nextIndex    = SSD_IF_START;

        //--- init Compare SSD ---------
        smSSD[SSD_CMP_START].prevIndex = smSSD[SSD_CMP_START].nextIndex = SSD_CMP_START;
        smSSD[SSD_CMP_START].startAddr =  (U32)smCacheCompare                                            >> SSD_ADDR_SHIFT;
        smSSD[SSD_CMP_START].endAddr   = ((U32)smCacheCompare + (NVME_MAX_DATA_TRANSFER_SIZE + MBU_SIZE) >> SSD_ADDR_SHIFT) - 1;

        //--- init TCG SSD -------------
        smSSD[SSD_TCG_START].prevIndex = SSD_TCG_END;
        smSSD[SSD_TCG_END].nextIndex   = SSD_TCG_START;

        //--- init admin manual xfer SSD -------------
        smSSD[SSD_ADMIN_START].prevIndex = SSD_ADMIN_END;
        smSSD[SSD_ADMIN_END].nextIndex   = SSD_ADMIN_START;

        //--- init read retry SSD -------------
        MEM_CLR(&smSSD[SSD_RR_START], sizeof(tSSD) * SSD_RR_COUNT);
        for (idx = SSD_RR_START; idx < SSD_RR_END; idx++)
        {
            smSSD[idx].nextIndex = idx + 1;
        }
        smSSD[SSD_RR_END].nextIndex = SSD_NULL_START;

        smCacheMgr.head  [SSD_POOL_RR] = &smSSD[SSD_RR_START];
        smCacheMgr.tail  [SSD_POOL_RR] = &smSSD[SSD_RR_END];
        smCacheMgr.ssdCnt[SSD_POOL_RR] = SSD_RR_COUNT;

        //--- init OTHERS SSD -------------
        MEM_CLR(&smSSD[SSD_OTHER_START], sizeof(tSSD) * SSD_OTHER_COUNT);
        for (idx = SSD_OTHER_START; idx < SSD_OTHER_END; idx++)
        {
            smSSD[idx].nextIndex = idx + 1;
        }
        smSSD[SSD_OTHER_END].nextIndex = SSD_NULL_START;

        smCacheMgr.head  [SSD_POOL_OTHER] = &smSSD[SSD_OTHER_START];
        smCacheMgr.tail  [SSD_POOL_OTHER] = &smSSD[SSD_OTHER_END];
        smCacheMgr.ssdCnt[SSD_POOL_OTHER] = SSD_OTHER_COUNT;

        //--- init hash table ----------
        MEM_CLR(&gCacheHashTable, sizeof(gCacheHashTable));
    }
    else if (mode == CACHE_RST_MODE_QUICK)
    {
        if (smCacheMgr.ssdCnt[SSD_POOL_FREE] == CACHE_RW_COUNT)
        {
            U16 idx;

            //--- init R/W SSD & Cache ----------
            for (idx = SSD_RW_START; idx <= SSD_RW_END; idx++)
            {
                smCacheMgr.cacheEntry[idx].prev = smCacheMgr.cacheEntry[idx].next = 0;
                smCacheMgr.cacheEntry[idx].laa = DWORD_MASK;
            }

            //--- init hash table ----------
            MEM_CLR(&gCacheHashTable, sizeof(gCacheHashTable));
        }
        else
            ASSERT(0);  // violate caceh reset (quick mode) rule
    }
}

//-----------------------------------------------------------------------------
/**
    Cache allocation exception handler

    @param[in]  pHcmdMsg
**/
//-----------------------------------------------------------------------------
void Cache_Alloc_Exception(MsgHostIO_t* pHcmdMsg)
{
    D_PRINTF("CSS[%d]\n", pHcmdMsg->bufDesc.css);
    ASSERT(0);
}

//-----------------------------------------------------------------------------
/**
    Cache allocation: Auto

    @param[in]  pJob
**/
//-----------------------------------------------------------------------------
void Cache_Alloc_Auto(tFIO_JOB* pJob)
{
    tSSD *pHead;

    ASSERT(smCacheMgr.ssdCnt[SSD_POOL_OTHER]);
    ASSERT(pJob->pBuffer != NULL);

    pHead = smCacheMgr.head[SSD_POOL_OTHER];
    Cache_PopSSD_Other(SSD_POOL_OTHER, pHead, pHead, 1);

    pHead->startAddr = (U32)pJob->pBuffer;

    if (pJob->auFmt == AU_FORMAT_READ_TOTAL_AU)
    {
        pHead->endAddr = (U32)(pJob->pBuffer + 512 * (gEcuInfo.xferSizePerAu * gEcuInfo.auPerPage) - 1);
    }
    else
    {
        pHead->endAddr = (U32)(pJob->pBuffer + pJob->paaCnt * gEcuInfo.auMetaSize - 1);
    }

    pJob->ssdList = SSD_WO_CACHE;
    pJob->ssdIdx[SSD_WO_CACHE] = GET_SSD_INDEX_FROM_PTR(pHead);
    pJob->ssdCnt[SSD_WO_CACHE] = 1;
}

//-----------------------------------------------------------------------------
/**
    Cache allocation: TCG

    @param[in]  pJob
**/
//-----------------------------------------------------------------------------
void Cache_Alloc_TCG(tFIO_JOB* pJob)
{
    tSSD *pHead;
    tSSD *pTail;
    U16 cacheIdx;
    U16 idx;
    U8 *pBuf;

    pHead = pTail = smCacheMgr.head[SSD_POOL_OTHER];
    cacheIdx      = GET_SSD_INDEX_FROM_PTR(pHead);

    pJob->ssdList = SSD_WO_CACHE;
    pJob->ssdIdx[SSD_WO_CACHE] = cacheIdx;
    pJob->ssdCnt[SSD_WO_CACHE] = pJob->paaCnt;

    pBuf = pJob->pBuffer;

    for (idx = 0; idx < pJob->paaCnt; idx++)
    {
        smSSD[cacheIdx].startAddr =   (U32)pBuf                        >> SSD_ADDR_SHIFT;
        smSSD[cacheIdx].endAddr   = (((U32)pBuf + gEcuInfo.auMetaSize) >> SSD_ADDR_SHIFT) - 1;

        pBuf += gEcuInfo.auMetaSize;

        pTail    = GET_SSD_PTR_FROM_INDEX(cacheIdx);
        cacheIdx = GET_SSD_NEXT_INDEX_FROM_INDEX(cacheIdx);
    }

    Cache_PopSSD_Other(SSD_POOL_OTHER, pHead, pTail, pJob->paaCnt);
}

//-----------------------------------------------------------------------------
/**
    Cache allocation: Read cmd hit fillup cache

    @param[in]  cache index
**/
//-----------------------------------------------------------------------------
void Cache_Alloc_ReadFillupHit(U16 cacheIdx)
{
    tSSD* pHead;
    tSSD* pTail;
    tFIO_JOB* pJob;
    U16 otherIdx;
    U16 paaPerFillup = 0;
    U16 idx;

    //-----------------------------------------------------------------------------
    smFioInfo.fillupJobCnt++;

    //-----------------------------------------------------------------------------
    pHead = pTail = &smSSD[cacheIdx];
    Cache_PopSSD(SSD_POOL_FILLUP, pHead, pTail, 1);
    smCacheMgr.cacheEntry[cacheIdx].status |= CACHE_S_SEQ_FILLUP;

    //-----------------------------------------------------------------------------
    pHead    = smCacheMgr.head[SSD_POOL_OTHER];
    otherIdx = GET_SSD_INDEX_FROM_PTR(pHead);

    pJob = FIO_PopJob(FIO_JOB_POOL_FREE);
    pJob->cmd                  = FIO_JOB_CMD_READ_FAST;
    pJob->ssdIdx[SSD_W_CACHE]  = cacheIdx;
    pJob->ssdCnt[SSD_W_CACHE]  = 1;
    pJob->ssdList              = SSD_WO_CACHE;
    pJob->ssdIdx[SSD_WO_CACHE] = otherIdx;
    pJob->css                  = CACHE_R_FILLUP_HIT;

    //-----------------------------------------------------------------------------
    pJob->paa[0] = FTL_PhyAddrSearch(smCacheMgr.cacheEntry[cacheIdx].laa);
    if (pJob->paa[0].all32 == DWORD_MASK)
    {
        pJob->paa[0] = Cache_GetZeroPAA(smCacheMgr.cacheEntry[cacheIdx].laa);
    }

    //-----------------------------------------------------------------------------

    otherIdx = Cache_Alloc_FillupSSD(pJob, cacheIdx, otherIdx, &paaPerFillup);
    pTail    = GET_SSD_PTR_FROM_INDEX(otherIdx);

    Cache_PopSSD_Other(SSD_POOL_OTHER, pHead, pTail, pJob->ssdCnt[SSD_WO_CACHE]);

    #ifdef LIMIT_NUM_OF_SSD_PER_AU_WORKAROUND
    for (idx = pJob->paaCnt + 1; idx < pJob->paaCnt + paaPerFillup; idx++)
    {
        pJob->paa[idx] = pJob->paa[pJob->paaCnt];
    }
    #endif

    pJob->paaCnt              += paaPerFillup;
    pJob->poolNo               = FIO_GET_SCH_POOL_NO(pJob->paa[0]);
    pJob->flag                 = FIO_F_SCHEDULE;
    //-----------------------------------------------------------------------------

    Core_JobIssue(pJob);
}

//-----------------------------------------------------------------------------
/**
    Cache allocation: Fillup
**/
//-----------------------------------------------------------------------------
void Cache_Alloc_Fillup(void)
{
    tFIO_JOB* pJob;
    tSSD* pFHead;
    tSSD* pFTail;
    tSSD* pHead;
    tSSD* pTail;
    U16 otherIdx;
    U16 cacheIdx;
    U16 fillupCnt;
    U16 paaPerFillup;
    U16 idx;

    //-----------------------------------------------------------------------------
    smFioInfo.fillupJobCnt++;

    //-----------------------------------------------------------------------------
    pFHead   = smCacheMgr.head[SSD_POOL_FILLUP];
    cacheIdx = GET_SSD_INDEX_FROM_PTR(pFHead);

    pHead    = smCacheMgr.head[SSD_POOL_OTHER];
    otherIdx = GET_SSD_INDEX_FROM_PTR(pHead);

    pJob = FIO_PopJob(FIO_JOB_POOL_FREE);
    pJob->cmd                  = FIO_JOB_CMD_READ_FAST;
    pJob->ssdIdx[SSD_W_CACHE]  = cacheIdx;
    pJob->ssdList              = SSD_WO_CACHE;
    pJob->ssdIdx[SSD_WO_CACHE] = otherIdx;
    pJob->css                  = CACHE_FILLUP;

    //-----------------------------------------------------------------------------
    fillupCnt = smCacheMgr.ssdCnt[SSD_POOL_FILLUP];
    if (fillupCnt > FIO_JOB_MAX_FILLUP_CNT) fillupCnt = FIO_JOB_MAX_FILLUP_CNT;

    while ((smCacheMgr.ssdCnt[SSD_POOL_OTHER] - pJob->ssdCnt[SSD_WO_CACHE]) > MAX_SSD_PER_FILLUP && pJob->paaCnt < fillupCnt)
    {
        pJob->paa[pJob->paaCnt] = FTL_PhyAddrSearch(smCacheMgr.cacheEntry[cacheIdx].laa);

        if (pJob->paa[pJob->paaCnt].all32 == DWORD_MASK)
        {
            pJob->paa[pJob->paaCnt] = Cache_GetZeroPAA(smCacheMgr.cacheEntry[cacheIdx].laa);
        }

        smCacheMgr.cacheEntry[cacheIdx].status |= CACHE_S_SEQ_FILLUP;

        //-----------------------------------------------------------------------------
        paaPerFillup = 0;
        otherIdx     = Cache_Alloc_FillupSSD(pJob, cacheIdx, otherIdx, &paaPerFillup);
        pTail        = GET_SSD_PTR_FROM_INDEX(otherIdx);
        otherIdx     = pTail->nextIndex;

        #ifdef LIMIT_NUM_OF_SSD_PER_AU_WORKAROUND
        for (idx = pJob->paaCnt + 1; idx < pJob->paaCnt + paaPerFillup; idx++)
        {
            pJob->paa[idx] = pJob->paa[pJob->paaCnt];
        }
        #endif

        pJob->paaCnt += paaPerFillup;

        //-----------------------------------------------------------------------------
        pJob->ssdCnt[SSD_W_CACHE]++;
        pFTail   = &smSSD[cacheIdx];
        cacheIdx = pFTail->nextIndex;
    }

    Cache_PopSSD(SSD_POOL_FILLUP, pFHead, pFTail, pJob->ssdCnt[SSD_W_CACHE]);
    Cache_PopSSD_Other(SSD_POOL_OTHER, pHead, pTail, pJob->ssdCnt[SSD_WO_CACHE]);

    Core_JobIssue(pJob);
}

//-----------------------------------------------------------------------------
/**
    Cache allocation: Pre-Read

    @param[in]  pHcmdMsg
**/
//-----------------------------------------------------------------------------
void Cache_Alloc_PreRead(MsgHostIO_t* pHcmdMsg)
{
    #if 0
    tFIO_JOB* pJob;
    tSSD *pHead;
    tSSD *pTail;
    tPAA paa;
    U32  laa;
    U32  preReadSLAA;
    U32  preReadELAA;
    U16  cacheIdx;
    U16  hitCnt;

    //-------------------------------------------------------------
    preReadSLAA = CmdSensor.lastPreReadLAA;
    preReadELAA = preReadSLAA + CACHE_PREREAD_CNT;
    CmdSensor.lastPreReadLAA = preReadELAA;

    //-------------------------------------------------------------
    pHead    = smCacheMgr.head[SSD_POOL_FREE];
    cacheIdx = GET_SSD_INDEX_FROM_PTR(pHead);

    //-------------------------------------------------------------
    if (preReadSLAA >= L2P_NUM || preReadELAA >= L2P_NUM)
    {
        CmdSensor.lastPreReadLAA = DWORD_MASK;
        return;
    }

    FTL_L2PBurstCopy(gFastLaaBuf, preReadSLAA, preReadELAA - preReadSLAA);

    // aligement
    paa.all32 = gFastLaaBuf[preReadELAA - preReadSLAA - 1];
    if (paa.b.ch && paa.all32 != DWORD_MASK)
    {
        for (laa = preReadSLAA; laa < preReadELAA; laa++)
        {
            paa.all32 = gFastLaaBuf[laa - preReadSLAA];
            if (paa.b.ch == (gNfInfo.numChannels - 1) && paa.b.frag == (gEcuInfo.auPerPage - 1) && paa.b.plane == (gNfInfo.planesPerLun - 1))
            {
                preReadELAA = laa + 1;
                CmdSensor.lastPreReadLAA = preReadELAA;
                break;
            }
        }
    }

    hitCnt = 0;
    for (laa = preReadSLAA; laa < preReadELAA; laa++)
    {
        if (FTL_TRIM_CHK_BMP(laa))
        {
            gFastLaaBuf[laa - preReadSLAA] = DWORD_MASK;
        }

        if (gFastLaaBuf[laa - preReadSLAA] != DWORD_MASK && Cache_Search(laa) == 0)
        {
            // --- Reset L2C for Old laa -----------------------------
            if (smCacheMgr.cacheEntry[cacheIdx].laa != DWORD_MASK)
            {
                Cache_Delete(cacheIdx);
            }

            smCacheMgr.cacheEntry[cacheIdx].status = CACHE_S_SEQ_READ;
            smCacheMgr.cacheEntry[cacheIdx].laa    = laa;
            Cache_Insert(cacheIdx);

            //-----------------------------------------------------------------
            gFastPaaBuf[hitCnt].all32 = gFastLaaBuf[laa - preReadSLAA];
            if (gFastPaaBuf[hitCnt].all32 == DWORD_MASK)
            {
                gFastPaaBuf[hitCnt] = Cache_GetZeroPAA(laa);
            }

            //-----------------------------------------------------------------

            pTail = &smSSD[cacheIdx];
            cacheIdx = pTail->nextIndex;

            hitCnt++;
        }
    }

    //-------------------------------------------------------------
    if (hitCnt)
    {
        pJob = FIO_PopJob(FIO_JOB_POOL_FREE);
        MEM_COPY(pJob->paa, gFastPaaBuf, sizeof(tPAA) * hitCnt);

        pJob->ssdIdx[SSD_W_CACHE] = GET_SSD_INDEX_FROM_PTR(pHead);
        pJob->ssdCnt[SSD_W_CACHE] = hitCnt;
        pJob->paaCnt              = hitCnt;
        pJob->css                 = CACHE_PREREAD;
        pJob->cmd                 = FIO_JOB_CMD_READ_FAST;

        Cache_PopSSD(SSD_POOL_FREE, pHead, pTail, hitCnt);
        Core_JobIssue(pJob);
    }
    #endif
}

//-----------------------------------------------------------------------------
/**
    Cache allocation: Read cmd without cache hit

    @param[in]  pHcmdMsg
**/
//-----------------------------------------------------------------------------
void Cache_Alloc_ReadNoHit(MsgHostIO_t* pHcmdMsg)
{
    tSSD     *pHead;
    tSSD     *pTail;
    tFIO_JOB *pJob;
    U32      laa;
    U32      laaEnd;
    U16      cacheIdx;
    U16      cnt;

    pHead                      = smCacheMgr.head[SSD_POOL_FREE];
    cacheIdx                   = GET_SSD_INDEX_FROM_PTR(pHead);
    pHcmdMsg->bufDesc.ssdIndex = cacheIdx;
    laaEnd                     = pHcmdMsg->laa + pHcmdMsg->bufDesc.ssdCnt;

    pJob = FIO_PopJob(FIO_JOB_POOL_FREE);

    cnt = 0;

    FTL_L2PBurstCopy((U32*)gFastPaaBuf, pHcmdMsg->laa, pHcmdMsg->bufDesc.ssdCnt);

    for (laa = pHcmdMsg->laa; laa < laaEnd; laa++)
    {
        // --- Reset L2C for Old laa -----------------------------
        if (smCacheMgr.cacheEntry[cacheIdx].laa != DWORD_MASK)
        {
            Cache_Delete(cacheIdx);
        }

        smCacheMgr.cacheEntry[cacheIdx].status = CACHE_S_SEQ_READ;
        smCacheMgr.cacheEntry[cacheIdx].laa    = laa;
        Cache_Insert(cacheIdx);

        pTail    = &smSSD[cacheIdx];
        cacheIdx = pTail->nextIndex;

        if (FTL_TRIM_CHK_BMP(laa))
        {
            gFastPaaBuf[laa - pHcmdMsg->laa].all32 = DWORD_MASK;
        }

        //-----------------------------------------------------------------
        if (gFastPaaBuf[cnt].all32 == DWORD_MASK)
        {
            gFastPaaBuf[cnt] = Cache_GetZeroPAA(laa);
        }

        //-----------------------------------------------------------------
        cnt++;
    }

    MEM_COPY(pJob->paa, gFastPaaBuf, sizeof(tPAA) * cnt);
    Cache_PopSSD(SSD_POOL_FREE, pHead, pTail, cnt);

    // ---------------------------------------------------------------
    pJob->ssdIdx[SSD_W_CACHE] = pHcmdMsg->bufDesc.ssdIndex;
    pJob->ssdCnt[SSD_W_CACHE] = cnt;
    pJob->pMsgHost            = pHcmdMsg;
    pJob->paaCnt              = cnt;
    pJob->css                 = CACHE_R_NO_HIT;
    pJob->poolNo              = FIO_GET_SCH_POOL_NO(gFastPaaBuf[0]);
    pJob->flag                = FIO_F_SCHEDULE;
    pJob->cmd                 = FIO_JOB_CMD_READ_FAST;

    Core_JobIssue(pJob);
}

//-----------------------------------------------------------------------------
/**
    Cache allocation: IDS Read

    @param[in]  pHcmdMsg
**/
//-----------------------------------------------------------------------------
void Cache_Alloc_ReadIDS(MsgHostIO_t* pHcmdMsg)
{
    tFIO_JOB *pJob;
    U32  laa;
    U32  idx;
    U16  issuedSSDCnt = 0;

    laa = pHcmdMsg->laa;
    FTL_L2PBurstCopy((U32*)gFastPaaBuf, laa, pHcmdMsg->bufDesc.ssdCnt);

    for (idx = 0; idx < pHcmdMsg->bufDesc.ssdCnt; idx++)
    {
        if (gFastPaaBuf[idx].all32 == DWORD_MASK || FTL_TRIM_CHK_BMP(laa))
        {
            gFastPaaBuf[idx] = Cache_GetZeroPAA(laa + idx);
        }
        #if (CO_SUPPORT_EARLY_MOVE_BLK == TRUE) && (ENABLE_GC == TRUE)
        else
        {
            GC_EarlyMoveBlk(gFastPaaBuf[idx]);
        }
        #endif

        laa++;
    }

    do
    {
        pJob = FIO_PopJob(FIO_JOB_POOL_FREE);
        pJob->pMsgHost  = pHcmdMsg;
        pJob->cmd       = FIO_JOB_CMD_READ_IDS;
        pJob->css       = CACHE_R_IDS;
        pJob->paaOffset = issuedSSDCnt;

        #if _NAND_ == NAND_MLC
            pJob->plane = FIO_CMD_1PLANE;

            if (pHcmdMsg->bufDesc.ssdCnt > 1)
            {
                if (issuedSSDCnt == 0 && gFastPaaBuf[0].b.frag)
                {
                    pJob->paaCnt = gEcuInfo.auPerPage - gFastPaaBuf[0].b.frag;

                    if (pJob->paaCnt > pHcmdMsg->bufDesc.ssdCnt)
                    {
                        pJob->paaCnt = pHcmdMsg->bufDesc.ssdCnt;
                    }
                }
                else if ((pHcmdMsg->bufDesc.ssdCnt - issuedSSDCnt) >= IDS_MAX_PAA_PER_JOB)
                {
                    tPAA paa;

                    paa  = gFastPaaBuf[issuedSSDCnt];

                    if (smCmdSensor.rCmdMode == CMD_SEQU_READ && paa.b.plane == 0)
                    {
                        pJob->plane = FIO_CMD_4PLANE;
                    }

                    if (paa.b.frag == 0)
                    {
                        pJob->cmd = FIO_JOB_CMD_READ_IDS_PAGE;
                        for (idx = 1; idx < IDS_MAX_PAA_PER_JOB; idx++)
                        {
                            paa.b.frag = idx;
                            if (paa.all32 != gFastPaaBuf[issuedSSDCnt + idx].all32)
                            {
                                pJob->cmd = FIO_JOB_CMD_READ_IDS;
                                break;
                            }
                        }
                    }

                    pJob->paaCnt = IDS_MAX_PAA_PER_JOB;
                }
                else
                {
                    pJob->paaCnt = (pHcmdMsg->bufDesc.ssdCnt - issuedSSDCnt);
                }
            }
            else
            {
                pJob->paaCnt = pHcmdMsg->bufDesc.ssdCnt;
            }

            pJob->poolNo = FIO_GET_SCH_POOL_NO(gFastPaaBuf[issuedSSDCnt]);
        #elif _NAND_ == NAND_3DTLCG2
            if (pHcmdMsg->bufDesc.ssdCnt < IDS_MAX_PAA_PER_JOB) pJob->plane = FIO_CMD_1PLANE;
            else                                                pJob->plane = FIO_CMD_2PLANE;

            if (pHcmdMsg->bufDesc.ssdCnt > 1)
            {
                if (issuedSSDCnt == 0 && gFastPaaBuf[0].b.frag)
                {
                    pJob->paaCnt = gEcuInfo.auPerPage - gFastPaaBuf[0].b.frag;

                    if (pJob->paaCnt > pHcmdMsg->bufDesc.ssdCnt)
                    {
                        pJob->paaCnt = pHcmdMsg->bufDesc.ssdCnt;
                    }
                }
                else if ((pHcmdMsg->bufDesc.ssdCnt - issuedSSDCnt) >= IDS_MAX_PAA_PER_JOB)
                {
                    tPAA paa;
                    tPAA paa2;

                    paa  = gFastPaaBuf[issuedSSDCnt];
                    paa2 = gFastPaaBuf[issuedSSDCnt + (IDS_MAX_PAA_PER_JOB / 2)];

                    if (paa.b.frag == 0 && paa2.b.frag == 0)
                    {
                        pJob->cmd = FIO_JOB_CMD_READ_IDS_PAGE;
                        for (idx = 1; idx < (IDS_MAX_PAA_PER_JOB / 2); idx++)
                        {
                            paa.b.frag  = idx;
                            paa2.b.frag = idx;
                            if ((paa.all32  != gFastPaaBuf[issuedSSDCnt + idx].all32) ||
                                (paa2.all32 != gFastPaaBuf[issuedSSDCnt + (IDS_MAX_PAA_PER_JOB / 2) + idx].all32))
                            {
                                pJob->cmd = FIO_JOB_CMD_READ_IDS;
                                break;
                            }
                        }
                    }

                    pJob->paaCnt = IDS_MAX_PAA_PER_JOB;
                }
                else
                {
                    pJob->paaCnt = (pHcmdMsg->bufDesc.ssdCnt - issuedSSDCnt);
                }

                if (pHcmdMsg->bufDesc.ssdCnt < IDS_MAX_PAA_PER_JOB) pJob->poolNo = FIO_GET_SCH_POOL_NO(gFastPaaBuf[issuedSSDCnt]);
                else                                                pJob->poolNo = FIO_JOB_POOL_READ + FIO_R_POOL_CNT_NORMAL;
            }
            else
            {
                pJob->paaCnt = pHcmdMsg->bufDesc.ssdCnt;
                pJob->poolNo = FIO_GET_SCH_POOL_NO(gFastPaaBuf[issuedSSDCnt]);
            }
        #else
            #error "unsupported nand"
        #endif

        MEM_COPY(pJob->paa, gFastPaaBuf + issuedSSDCnt, sizeof(tPAA) * pJob->paaCnt);

        pJob->flag = FIO_F_SCHEDULE;

        Core_JobIssue(pJob);

        issuedSSDCnt += pJob->paaCnt;
    } while (issuedSSDCnt < pHcmdMsg->bufDesc.ssdCnt);

    Core_ProcHostXfer(pHcmdMsg);
}

//-----------------------------------------------------------------------------
/**
    Cache allocation: Read cmd with full cache hit

    @param[in]  pHcmdMsg
**/
//-----------------------------------------------------------------------------
void Cache_Alloc_ReadFullHit(MsgHostIO_t* pHcmdMsg)
{
    tSSD* pTmpTail = NULL;
    tSSD *pHead;
    tSSD *pTail;
    U32   laa, laaEnd;
    U16   nextCacheIdx;
    U16   hitCnt;
    U16   cacheIdx;

    cacheIdx                    = Cache_Search(pHcmdMsg->laa);
    nextCacheIdx                = cacheIdx;
    pHcmdMsg->bufDesc.ssdIndex  = cacheIdx;
    pHead                       = &smSSD[cacheIdx];
    laaEnd                      = pHcmdMsg->laa + pHcmdMsg->bufDesc.ssdCnt;

    hitCnt = 0;

    for (laa = pHcmdMsg->laa; laa < laaEnd; laa++)
    {
        cacheIdx = Cache_Search(laa);
        if (nextCacheIdx == cacheIdx)
        {
            hitCnt++;
            pTail = &smSSD[cacheIdx];
        }
        else
        {
            // --- Cache list discontinued, pop previous region and re-assign pHead/pTail and count
            Cache_PopSSD(smCacheMgr.cacheEntry[GET_SSD_INDEX_FROM_PTR(pHead)].pool, pHead, pTail, hitCnt);

            // --- concat previous tail to current head -----------------------------------------
            if (pTmpTail)
            {
                pTmpTail->nextIndex = GET_SSD_INDEX_FROM_PTR(pHead);
                pHead->prevIndex    = GET_SSD_INDEX_FROM_PTR(pTmpTail);
            }

            //-----------------------------------------------------------------------------------
            pTmpTail      = pTail;
            hitCnt        = 1;
            pHead = pTail = &smSSD[cacheIdx];
        }

        smCacheMgr.cacheEntry[cacheIdx].status |= CACHE_S_XFER_READ;

        // --- Check cache error bit ------------------------------
        if (smCacheMgr.cacheEntry[cacheIdx].status & CACHE_S_ERROR)
        {
            pHcmdMsg->error = cEcError;
        }

        nextCacheIdx = pTail->nextIndex;
    }

    //--- finalization ---------------------------------------------------------------------------
    Cache_PopSSD(smCacheMgr.cacheEntry[GET_SSD_INDEX_FROM_PTR(pHead)].pool, pHead, pTail, hitCnt);

    if (pTmpTail)
    {
        pTmpTail->nextIndex = GET_SSD_INDEX_FROM_PTR(pHead);
        pHead->prevIndex    = GET_SSD_INDEX_FROM_PTR(pTmpTail);
    }

    Core_ProcHostXfer(pHcmdMsg);
}

//-----------------------------------------------------------------------------
/**
    Cache allocation: Read cmd with partial cache hit

    @param[in]  pHcmdMsg
**/
//-----------------------------------------------------------------------------
void Cache_Alloc_ReadPartialHit(MsgHostIO_t* pHcmdMsg)
{
    tFIO_JOB* pJob;
    tSSD* pTmpTail     = NULL;
    tSSD *pHead;
    tSSD *pTail;
    U32   laa;
    U32   laaEnd;
    U16   nextCacheIdx = 0;
    U16   endOtherIdx  = 0;
    U16   cacheIdx;
    U16   freeIdx;
    U16   otherIdx;
    U16   hitCnt;

    freeIdx  = GET_SSD_INDEX_FROM_PTR(smCacheMgr.head[SSD_POOL_FREE]);
    otherIdx = GET_SSD_INDEX_FROM_PTR(smCacheMgr.head[SSD_POOL_OTHER]);

    pJob                       = FIO_PopJob(FIO_JOB_POOL_FREE);
    pJob->css                  = CACHE_R_PARTIAL_HIT;
    pJob->ssdList              = SSD_WO_CACHE;
    pJob->ssdIdx[SSD_WO_CACHE] = otherIdx;
    pJob->pMsgHost             = pHcmdMsg;

    // ----------------------------------------------------------------------------------
    FTL_L2PBurstCopy(gFastLaaBuf, pHcmdMsg->laa, pHcmdMsg->bufDesc.ssdCnt);

    hitCnt = 0;
    laa    = pHcmdMsg->laa;
    laaEnd = pHcmdMsg->laa + pHcmdMsg->bufDesc.ssdCnt;

    while (laa < laaEnd)
    {
        cacheIdx = Cache_Search(laa);
        if (cacheIdx == 0)
        {
            if (smCacheMgr.cacheEntry[freeIdx].laa != DWORD_MASK)
            {
                if (smCacheMgr.cacheEntry[freeIdx].laa >= pHcmdMsg->laa && smCacheMgr.cacheEntry[freeIdx].laa < laaEnd)
                {
                    freeIdx = smSSD[freeIdx].nextIndex;
                    continue;
                }

                Cache_Delete(freeIdx);
            }

            cacheIdx = freeIdx;

            smCacheMgr.cacheEntry[cacheIdx].laa    = laa;
            smCacheMgr.cacheEntry[cacheIdx].status = 0;
            Cache_Insert(cacheIdx);

            smSSD[otherIdx].prevIndex = cacheIdx;
            smSSD[otherIdx].startAddr = smSSD[cacheIdx].startAddr;
            smSSD[otherIdx].endAddr   = smSSD[cacheIdx].endAddr;

            // backup end other index... for Cache_PopSSD_Other()
            endOtherIdx = otherIdx;

            // ----------------------------------------------------------------------------------
            if (FTL_TRIM_CHK_BMP(laa))
            {
                gFastLaaBuf[laa - pHcmdMsg->laa] = DWORD_MASK;
            }

            gFastPaaBuf[pJob->paaCnt].all32 = gFastLaaBuf[laa - pHcmdMsg->laa];
            if (gFastPaaBuf[pJob->paaCnt].all32 == DWORD_MASK)
            {
                gFastPaaBuf[pJob->paaCnt] = Cache_GetZeroPAA(laa);
            }

            // --- update index & count -----------------------------
            pJob->ssdCnt[SSD_WO_CACHE]++;
            pJob->paaCnt++;

            freeIdx  = smSSD[freeIdx].nextIndex;
            otherIdx = smSSD[otherIdx].nextIndex;
        }

        if (hitCnt == 0)
        {
            hitCnt++;
            pHead = pTail               = GET_SSD_PTR_FROM_INDEX(cacheIdx);
            pHcmdMsg->bufDesc.ssdIndex  = cacheIdx;
        }
        else if (nextCacheIdx != cacheIdx)
        {
            // --- Cache list discontinuous, pop previous region and re-assign pHead/pTail and count
            Cache_PopSSD(smCacheMgr.cacheEntry[GET_SSD_INDEX_FROM_PTR(pHead)].pool, pHead, pTail, hitCnt);

            if (smCacheMgr.cacheEntry[GET_SSD_INDEX_FROM_PTR(pHead)].pool == SSD_POOL_FREE)
            {
                freeIdx = GET_SSD_INDEX_FROM_PTR(smCacheMgr.head[SSD_POOL_FREE]);
            }

            // --- concat previous tail to current head -----------------------------------------
            if (pTmpTail)
            {
                pTmpTail->nextIndex = GET_SSD_INDEX_FROM_PTR(pHead);
                pHead->prevIndex    = GET_SSD_INDEX_FROM_PTR(pTmpTail);
            }

            pTmpTail = pTail;

            // ----------------------------------------------------------------------------------
            hitCnt        = 1;
            pHead = pTail = GET_SSD_PTR_FROM_INDEX(cacheIdx);
        }
        else
        {
            hitCnt++;
            pTail = GET_SSD_PTR_FROM_INDEX(cacheIdx);
        }

        // --- Check cache error bit ------------------------------
        if (smCacheMgr.cacheEntry[cacheIdx].status & CACHE_S_ERROR)
        {
            pHcmdMsg->error = cEcError;
        }

        smCacheMgr.cacheEntry[cacheIdx].status |= CACHE_S_SEQ_READ;

        nextCacheIdx = smSSD[cacheIdx].nextIndex;
        laa++;
    }

    // --- Cache list discontinued, pop previous region and re-assign pHead/pTail and count
    Cache_PopSSD(smCacheMgr.cacheEntry[GET_SSD_INDEX_FROM_PTR(pHead)].pool, pHead, pTail, hitCnt);

    // --- concat previous tail to current head -----------------------------------------
    if (pTmpTail)
    {
        pTmpTail->nextIndex = GET_SSD_INDEX_FROM_PTR(pHead);
        pHead->prevIndex    = GET_SSD_INDEX_FROM_PTR(pTmpTail);
    }

    Cache_PopSSD_Other(SSD_POOL_OTHER, GET_SSD_PTR_FROM_INDEX(pJob->ssdIdx[SSD_WO_CACHE]), GET_SSD_PTR_FROM_INDEX(endOtherIdx), pJob->ssdCnt[SSD_WO_CACHE]);

    pJob->ssdIdx[SSD_W_CACHE] = pHcmdMsg->bufDesc.ssdIndex;
    pJob->ssdCnt[SSD_W_CACHE] = pHcmdMsg->bufDesc.ssdCnt;

    pJob->cmd = FIO_JOB_CMD_READ_FAST;

    MEM_COPY(pJob->paa, gFastPaaBuf, sizeof(tPAA)*pJob->paaCnt);

    pJob->poolNo = FIO_GET_SCH_POOL_NO(gFastPaaBuf[0]);
    pJob->flag   = FIO_F_SCHEDULE;

    Core_JobIssue(pJob);
}

//-----------------------------------------------------------------------------
/**
    Cache allocation: Read cmd with full cache hit blank

    @param[in]  pHcmdMsg
**/
//-----------------------------------------------------------------------------
void Cache_Alloc_ReadFullHitBlank(MsgHostIO_t* pHcmdMsg)
{
    #if _TCG_ == TCG_EDRV
        U16 cacheIdx;

        cacheIdx = SSD_ZERO_START + HAL_MRE_GetRangeNo(pHcmdMsg->laa << HBU_PER_MBU_SFT);

        pHcmdMsg->bufDesc.ssdIndex  = cacheIdx;
    #else

        pHcmdMsg->bufDesc.ssdIndex  = SSD_ZERO_START;
    #endif

    Core_ProcHostXfer(pHcmdMsg);
}

//-----------------------------------------------------------------------------
/**
    Cache allocation: write cmd without cache hit

    @param[in]  pHcmdMsg
**/
//-----------------------------------------------------------------------------
void Cache_Alloc_WriteNoHit(MsgHostIO_t* pHcmdMsg)
{
    tSSD *pHead;
    tSSD *pTail;
    U32  laa;
    U32  laaEnd;
    U16  cacheIdx;

    pHead = pTail               = smCacheMgr.head[SSD_POOL_FREE];
    cacheIdx                    = GET_SSD_INDEX_FROM_PTR(pHead);
    pHcmdMsg->bufDesc.ssdIndex  = cacheIdx;
    laaEnd                      = pHcmdMsg->laa + pHcmdMsg->bufDesc.ssdCnt;

    for (laa = pHcmdMsg->laa; laa < laaEnd; laa++)
    {
        // --- Reset L2C ------------------------------------------
        if (smCacheMgr.cacheEntry[cacheIdx].laa != DWORD_MASK)
        {
            Cache_Delete(cacheIdx);
        }

        smCacheMgr.cacheEntry[cacheIdx].status = CACHE_S_XFER_WRITE;
        smCacheMgr.cacheEntry[cacheIdx].bitmap = CACHE_BMP_ALL_INVALID;
        smCacheMgr.cacheEntry[cacheIdx].laa    = laa;
        Cache_Insert(cacheIdx);
        pTail                                  = &smSSD[cacheIdx];
        cacheIdx                               = pTail->nextIndex;
    }

    Cache_PopSSD(SSD_POOL_FREE, pHead, pTail, pHcmdMsg->bufDesc.ssdCnt);

    Core_ProcHostXfer(pHcmdMsg);
}

//-----------------------------------------------------------------------------
/**
    Cache allocation: write cmd with full cache hit

    @param[in]  pHcmdMsg
**/
//-----------------------------------------------------------------------------
void Cache_Alloc_WriteFullHit(MsgHostIO_t* pHcmdMsg)
{
    tSSD *pTmpTail = NULL;
    tSSD *pHead;
    tSSD *pTail;
    U32   laa;
    U32   laaEnd;
    U16   nextCacheIdx;
    U16   hitCnt;
    U16   cacheIdx;

    hitCnt                      = 0;
    cacheIdx                    = Cache_Search(pHcmdMsg->laa);
    nextCacheIdx                = cacheIdx;
    pHcmdMsg->bufDesc.ssdIndex  = cacheIdx;
    pHead = pTail               = &smSSD[cacheIdx];
    laaEnd                      = pHcmdMsg->laa + pHcmdMsg->bufDesc.ssdCnt;

    for (laa = pHcmdMsg->laa; laa < laaEnd; laa++)
    {
        cacheIdx = Cache_Search(laa);
        if (nextCacheIdx == cacheIdx)
        {
            hitCnt++;
            pTail = &smSSD[cacheIdx];
        }
        else
        {
            Cache_PopSSD(smCacheMgr.cacheEntry[GET_SSD_INDEX_FROM_PTR(pHead)].pool, pHead, pTail, hitCnt);

            if (pTmpTail)
            {
                pTmpTail->nextIndex = GET_SSD_INDEX_FROM_PTR(pHead);
                pHead->prevIndex    = GET_SSD_INDEX_FROM_PTR(pTmpTail);
            }

            //-----------------------------------------------------------------------------------

            pTmpTail      = pTail;
            hitCnt        = 1;
            pHead = pTail = &smSSD[cacheIdx];
        }

        smCacheMgr.cacheEntry[cacheIdx].status |= CACHE_S_XFER_WRITE;

        nextCacheIdx = pTail->nextIndex;
    }

    //--- finalization ---------------------------------------------------------------------------
    Cache_PopSSD(smCacheMgr.cacheEntry[GET_SSD_INDEX_FROM_PTR(pHead)].pool, pHead, pTail, hitCnt);

    if (pTmpTail)
    {
        pTmpTail->nextIndex = GET_SSD_INDEX_FROM_PTR(pHead);
        pHead->prevIndex    = GET_SSD_INDEX_FROM_PTR(pTmpTail);
    }

    Core_ProcHostXfer(pHcmdMsg);
}

//-----------------------------------------------------------------------------
/**
    Cache allocation: write cmd with partial cache hit

    @param[in]  pHcmdMsg
**/
//-----------------------------------------------------------------------------
void Cache_Alloc_WritePartialHit(MsgHostIO_t* pHcmdMsg)
{
    tSSD *pTmpTail     = NULL;
    tSSD *pHead;
    tSSD *pTail;
    U32   laa;
    U32   laaEnd;
    U16   hitCnt;
    U16   cacheIdx;
    U16   nextCacheIdx = 0;
    U16   freeIdx;

    freeIdx = GET_SSD_INDEX_FROM_PTR(smCacheMgr.head[SSD_POOL_FREE]);
    hitCnt  = 0;
    laa     = pHcmdMsg->laa;
    laaEnd  = pHcmdMsg->laa + pHcmdMsg->bufDesc.ssdCnt;

    while (laa < laaEnd)
    {
        cacheIdx = Cache_Search(laa);
        if (cacheIdx == 0)
        {
            if (smCacheMgr.cacheEntry[freeIdx].laa != DWORD_MASK)
            {
                if (smCacheMgr.cacheEntry[freeIdx].laa >= pHcmdMsg->laa && smCacheMgr.cacheEntry[freeIdx].laa < laaEnd)
                {
                    freeIdx = smSSD[freeIdx].nextIndex;
                    continue;
                }

                Cache_Delete(freeIdx);
            }

            cacheIdx = freeIdx;

            smCacheMgr.cacheEntry[cacheIdx].laa    = laa;
            smCacheMgr.cacheEntry[cacheIdx].status = 0;
            smCacheMgr.cacheEntry[cacheIdx].bitmap = CACHE_BMP_ALL_INVALID;
            Cache_Insert(cacheIdx);

            freeIdx  = smSSD[freeIdx].nextIndex;
        }

        if (hitCnt == 0)
        {
            hitCnt++;
            pHead = pTail              = GET_SSD_PTR_FROM_INDEX(cacheIdx);
            pHcmdMsg->bufDesc.ssdIndex = cacheIdx;
        }
        else if (nextCacheIdx != cacheIdx)
        {
            // --- Cache list discontinued, pop previous region and re-assign pHead/pTail and count
            Cache_PopSSD(smCacheMgr.cacheEntry[GET_SSD_INDEX_FROM_PTR(pHead)].pool, pHead, pTail, hitCnt);

            if (smCacheMgr.cacheEntry[GET_SSD_INDEX_FROM_PTR(pHead)].pool == SSD_POOL_FREE)
            {
                freeIdx = GET_SSD_INDEX_FROM_PTR(smCacheMgr.head[SSD_POOL_FREE]);
            }

            // --- concat previous tail to current head -----------------------------------------
            if (pTmpTail)
            {
                pTmpTail->nextIndex = GET_SSD_INDEX_FROM_PTR(pHead);
                pHead->prevIndex    = GET_SSD_INDEX_FROM_PTR(pTmpTail);
            }

            pTmpTail = pTail;

            // ----------------------------------------------------------------------------------
            hitCnt        = 1;
            pHead = pTail = GET_SSD_PTR_FROM_INDEX(cacheIdx);
        }
        else
        {
            hitCnt++;
            pTail = GET_SSD_PTR_FROM_INDEX(cacheIdx);
        }

        smCacheMgr.cacheEntry[cacheIdx].status |= CACHE_S_XFER_WRITE;

        nextCacheIdx = smSSD[cacheIdx].nextIndex;
        laa++;
    }

    // --- Cache list discontinued, pop previous region and re-assign pHead/pTail and count
    Cache_PopSSD(smCacheMgr.cacheEntry[GET_SSD_INDEX_FROM_PTR(pHead)].pool, pHead, pTail, hitCnt);

    // --- concat previous tail to current head -----------------------------------------
    if (pTmpTail)
    {
        pTmpTail->nextIndex = GET_SSD_INDEX_FROM_PTR(pHead);
        pHead->prevIndex    = GET_SSD_INDEX_FROM_PTR(pTmpTail);
    }

    Core_ProcHostXfer(pHcmdMsg);
}

//-----------------------------------------------------------------------------
/**
    Cache Transfer Done: exception handler

    @param[in]  pHcmdMsg
**/
//-----------------------------------------------------------------------------
void Cache_XferDone_Exception(MsgCacheRelObj_t* pHcmdMsg)
{
    D_PRINTF("CSS[%d]\n", pHcmdMsg->bufDesc.css);
    ASSERT(0);
}

//-----------------------------------------------------------------------------
/**
    Cache Transfer Done: Auto

    @param[in]  pHcmdMsg
**/
//-----------------------------------------------------------------------------
void Cache_XferDone_Auto(MsgCacheRelObj_t* pHcmdMsg)
{
    // Do nothing...
}

//-----------------------------------------------------------------------------
/**
    Cache Transfer Done: read cmd with full cache hit blank

    @param[in]  pHcmdMsg
**/
//-----------------------------------------------------------------------------
void Cache_XferDone_ReadFullHitBlank(MsgCacheRelObj_t* pHcmdMsg)
{
    CORE_FREE_PRP_CNT_ADD(pHcmdMsg->bufDesc.ssdCnt);
}

//-----------------------------------------------------------------------------
/**
    Cache Transfer Done: Read cmd with full cache hit

    @param[in]  pHcmdMsg
**/
//-----------------------------------------------------------------------------
void Cache_XferDone_ReadFullHit(MsgCacheRelObj_t* pHcmdMsg)
{
    tSSD *pHead    = NULL;
    tSSD *pTail    = NULL;
    U16   idx      = 0;
    U16   cnt      = 0;
    U16   pool;
    U16   cacheIdx;

    cacheIdx       = pHcmdMsg->bufDesc.ssdIndex;
    pHead = pTail  = &smSSD[cacheIdx];
    pool           = smCacheMgr.cacheEntry[cacheIdx].pool;

    CORE_FREE_PRP_CNT_ADD(pHcmdMsg->bufDesc.ssdCnt);

    while (idx < pHcmdMsg->bufDesc.ssdCnt)
    {
        if (pool == smCacheMgr.cacheEntry[cacheIdx].pool)
        {
            cnt++;
            pTail = &smSSD[cacheIdx];
        }
        else
        {
            Cache_PushSSD(pool, pHead, pTail, cnt);
            //---------------------------------------------------------------------------------
            pool          = smCacheMgr.cacheEntry[cacheIdx].pool;
            cnt           = 1;
            pHead = pTail = &smSSD[cacheIdx];
        }

        smCacheMgr.cacheEntry[cacheIdx].status &= ~CACHE_S_XFER_MASK;
        cacheIdx = pTail->nextIndex;
        idx++;
    }

    //--- finalization --------------------------------------------------------------------------
    Cache_PushSSD(pool, pHead, pTail, cnt);
}

//-----------------------------------------------------------------------------
/**
    Cache Transfer Done: write cmd without cache hit

    @param[in]  pHcmdMsg
**/
//-----------------------------------------------------------------------------
void Cache_XferDone_WriteNoHit(MsgCacheRelObj_t* pHcmdMsg)
{
    tSSD *pHead   = NULL;
    tSSD *pTail   = NULL;
    U16   idx     = 0;
    U16   cnt     = 0;
    U16   cacheIdx;

    cacheIdx      = pHcmdMsg->bufDesc.ssdIndex;
    pHead = pTail = &smSSD[cacheIdx];

    if (pHcmdMsg->bufDesc.laaStartBmp == CACHE_BMP_ALL_VALID && pHcmdMsg->bufDesc.laaEndBmp == CACHE_BMP_ALL_VALID)
    {
        #if _NAND_ == NAND_3DTLCG2
            U16 splitBit;

            splitBit = smCacheMgr.cacheEntry[cacheIdx].laa & WRITE_CACHE_SPLIT_MASK;

            while (idx++ < pHcmdMsg->bufDesc.ssdCnt)
            {
                if ((smCacheMgr.cacheEntry[cacheIdx].laa & WRITE_CACHE_SPLIT_MASK) != splitBit)
                {
                    Cache_PushSSD(SSD_POOL_WRITE + (splitBit >> WRITE_CACHE_SPLIT_SHIFT), pHead, pTail, cnt);

                    splitBit = smCacheMgr.cacheEntry[cacheIdx].laa & WRITE_CACHE_SPLIT_MASK;
                    cnt      = 0;
                    pHead    = &smSSD[cacheIdx];
                }

                smCacheMgr.cacheEntry[cacheIdx].status = 0;
                smCacheMgr.cacheEntry[cacheIdx].pool   = SSD_POOL_WRITE + (splitBit >> WRITE_CACHE_SPLIT_SHIFT);
                smCacheMgr.cacheEntry[cacheIdx].bitmap = CACHE_BMP_ALL_VALID;
                cnt++;
                pTail    = &smSSD[cacheIdx];
                cacheIdx = pTail->nextIndex;
            }

            if (cnt)    Cache_PushSSD(SSD_POOL_WRITE + (splitBit >> WRITE_CACHE_SPLIT_SHIFT) , pHead, pTail, cnt);

        #else
            while (cnt < pHcmdMsg->bufDesc.ssdCnt)
            {
                smCacheMgr.cacheEntry[cacheIdx].status = 0;
                smCacheMgr.cacheEntry[cacheIdx].pool   = SSD_POOL_WRITE;
                smCacheMgr.cacheEntry[cacheIdx].bitmap = CACHE_BMP_ALL_VALID;
                cnt++;
                pTail = &smSSD[cacheIdx];
                cacheIdx = pTail->nextIndex;
            }

            Cache_PushSSD(SSD_POOL_WRITE, pHead, pTail, cnt);
        #endif
    }
    else
    {
        while (idx < pHcmdMsg->bufDesc.ssdCnt)
        {
            if ((idx == 0) && pHcmdMsg->bufDesc.laaStartBmp != CACHE_BMP_ALL_VALID)
            {
                pHead = pTail = &smSSD[pTail->nextIndex];

                smCacheMgr.cacheEntry[cacheIdx].bitmap |= pHcmdMsg->bufDesc.laaStartBmp;
                smCacheMgr.cacheEntry[cacheIdx].status &= ~CACHE_S_XFER_MASK;

                if (smCacheMgr.cacheEntry[cacheIdx].bitmap == CACHE_BMP_ALL_VALID)
                {
                    smCacheMgr.cacheEntry[cacheIdx].pool   = SSD_POOL_WRITE;
                    Cache_PushSSD(SSD_POOL_WRITE, GET_SSD_PTR_FROM_INDEX(cacheIdx), GET_SSD_PTR_FROM_INDEX(cacheIdx), 1);
                }
                else
                {
                    smCacheMgr.cacheEntry[cacheIdx].pool   = SSD_POOL_FILLUP;
                    Cache_PushSSD(SSD_POOL_FILLUP, GET_SSD_PTR_FROM_INDEX(cacheIdx), GET_SSD_PTR_FROM_INDEX(cacheIdx), 1);
                }

                cnt = 0;
                cacheIdx = GET_SSD_INDEX_FROM_PTR(pTail);
            }
            else if ((idx == (pHcmdMsg->bufDesc.ssdCnt - 1)) && pHcmdMsg->bufDesc.laaEndBmp != CACHE_BMP_ALL_VALID)
            {
                if (cnt)  Cache_PushSSD(SSD_POOL_WRITE, pHead, pTail, cnt);

                pHead = pTail = &smSSD[cacheIdx];

                smCacheMgr.cacheEntry[cacheIdx].bitmap |= pHcmdMsg->bufDesc.laaEndBmp;
                smCacheMgr.cacheEntry[cacheIdx].status &= ~CACHE_S_XFER_MASK;

                if (smCacheMgr.cacheEntry[cacheIdx].bitmap == CACHE_BMP_ALL_VALID)
                {
                    smCacheMgr.cacheEntry[cacheIdx].pool   = SSD_POOL_WRITE;
                    Cache_PushSSD(SSD_POOL_WRITE, pHead, pTail, 1);
                }
                else
                {
                    smCacheMgr.cacheEntry[cacheIdx].pool   = SSD_POOL_FILLUP;
                    Cache_PushSSD(SSD_POOL_FILLUP, pHead, pTail, 1);
                }

                cnt = 0;
            }
            else
            {
                smCacheMgr.cacheEntry[cacheIdx].status = 0;
                smCacheMgr.cacheEntry[cacheIdx].pool   = SSD_POOL_WRITE;
                smCacheMgr.cacheEntry[cacheIdx].bitmap = CACHE_BMP_ALL_VALID;
                cnt++;
                pTail    = &smSSD[cacheIdx];
                cacheIdx = pTail->nextIndex;
            }

            idx++;
        }

        if (cnt)  Cache_PushSSD(SSD_POOL_WRITE, pHead, pTail, cnt);
    }
}

//-----------------------------------------------------------------------------
/**
    Cache Transfer Done: Fua Write

    @param[in]  pHcmdMsg
**/
//-----------------------------------------------------------------------------
void Cache_XferDone_WriteFua(MsgCacheRelObj_t* pHcmdMsg)
{
    U16 cacheIdx;
    U16 count = 0;

    cacheIdx = pHcmdMsg->bufDesc.ssdIndex;

    while (count < pHcmdMsg->bufDesc.ssdCnt)
    {
        smCacheMgr.cacheEntry[cacheIdx].status |= CACHE_S_SEQ_FUA;
        smCacheMgr.cacheEntry[cacheIdx].status &= ~CACHE_S_XFER_MASK;

        if (count == 0 && pHcmdMsg->bufDesc.laaStartBmp != CACHE_BMP_ALL_VALID)
        {
            smCacheMgr.cacheEntry[cacheIdx].bitmap |= pHcmdMsg->bufDesc.laaStartBmp;
            if (smCacheMgr.cacheEntry[cacheIdx].bitmap == CACHE_BMP_ALL_VALID)
            {
                smCacheMgr.cacheEntry[cacheIdx].pool = SSD_POOL_WRITE;
            }
        }
        else if (count == (pHcmdMsg->bufDesc.ssdCnt - 1) && pHcmdMsg->bufDesc.laaEndBmp != CACHE_BMP_ALL_VALID)
        {
            smCacheMgr.cacheEntry[cacheIdx].bitmap |= pHcmdMsg->bufDesc.laaEndBmp;

            if (smCacheMgr.cacheEntry[cacheIdx].bitmap == CACHE_BMP_ALL_VALID)
            {
                smCacheMgr.cacheEntry[cacheIdx].pool = SSD_POOL_WRITE;
            }
        }
        else
        {
            smCacheMgr.cacheEntry[cacheIdx].bitmap = CACHE_BMP_ALL_VALID;
        }

        cacheIdx = GET_SSD_NEXT_INDEX_FROM_INDEX(cacheIdx);
        count++;
    }
}

//-----------------------------------------------------------------------------
/**
    Cache Transfer Done: Abort

    @param[in]  pHcmdMsg
**/
//-----------------------------------------------------------------------------
void Cache_XferDone_Abort(MsgCacheRelObj_t* pHcmdMsg)
{
    if (pHcmdMsg->bufDesc.ssdCnt)
    {
        if ((pHcmdMsg->hdr.b.opCode == cMcRead) || (pHcmdMsg->hdr.b.opCode == cMcCompareCmd))
        {
            CORE_FREE_PRP_CNT_ADD(pHcmdMsg->bufDesc.ssdCnt);
        }

        if ((pHcmdMsg->bufDesc.cssTemp != CACHE_R_IDS) && (pHcmdMsg->bufDesc.cssTemp != CACHE_R_FULL_HIT_BLANK))
        {
            tSSD *pHead    = NULL;
            tSSD *pTail    = NULL;
            U16   idx      = 0;
            U16   cnt      = 0;
            U16   pool;
            U16   cacheIdx;

            cacheIdx      = pHcmdMsg->bufDesc.ssdIndex;
            pHead = pTail = &smSSD[cacheIdx];
            pool          = smCacheMgr.cacheEntry[cacheIdx].pool;

            while (idx < pHcmdMsg->bufDesc.ssdCnt)
            {
                if (pool == smCacheMgr.cacheEntry[cacheIdx].pool)
                {
                    cnt++;
                    pTail = &smSSD[cacheIdx];
                }
                else
                {
                    Cache_PushSSD(pool, pHead, pTail, cnt);
                    //---------------------------------------------------------------------------------
                    pool          = smCacheMgr.cacheEntry[cacheIdx].pool;
                    cnt           = 1;
                    pHead = pTail = &smSSD[cacheIdx];
                }

                smCacheMgr.cacheEntry[cacheIdx].status &= ~CACHE_S_XFER_MASK;
                cacheIdx = pTail->nextIndex;
                idx++;
            }

            //--- finalization -------------------------------------------------------------------------
            Cache_PushSSD(pool, pHead, pTail, cnt);
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Cache Track Done: Exception handler

    @param[in]  pJob
**/
//-----------------------------------------------------------------------------
void Cache_TrkDone_Exception(tFIO_JOB* pJob)
{
    D_PRINTF("CSS[%d]\n", pJob->css);
    ASSERT(0);
}

//-----------------------------------------------------------------------------
/**
    Cache Track Done: Auto

    @param[in]  pJob
**/
//-----------------------------------------------------------------------------
void Cache_TrkDone_Auto(tFIO_JOB* pJob)
{
    if (pJob->ssdCnt[SSD_WO_CACHE])
    {
        tSSD *pHead = NULL;
        tSSD *pTail = NULL;
        U32 cacheIdx;
        U16 idx;

        cacheIdx = pJob->ssdIdx[SSD_WO_CACHE];
        pHead    = GET_SSD_PTR_FROM_INDEX(cacheIdx);

        for (idx = 0; idx < pJob->ssdCnt[SSD_WO_CACHE]; idx++)
        {
            pTail    = &smSSD[cacheIdx];
            cacheIdx = pTail->nextIndex;
        }

        Cache_PushSSD_Other(SSD_POOL_OTHER, pHead, pTail, pJob->ssdCnt[SSD_WO_CACHE]);
    }
}


//-----------------------------------------------------------------------------
/**
    Cache Track Done: SysInfo

    @param[in]  pJob
**/
//-----------------------------------------------------------------------------
void Cache_TrkDone_SysInfo(tFIO_JOB* pJob)
{
    MsgSI_t *pHcmdMsg;

    Cache_TrkDone_Auto(pJob);

    if (pJob->cmd == FIO_JOB_CMD_ERASE)
    {
        SI_SET_ERASEBMP(SyIn_NAND_BankIdx(pJob->paa[0]));
        BIT_CLR(gSysInfoMgr.mgrBitMap, SI_S_ERASE);
    }
    else if (pJob->cmd == FIO_JOB_CMD_WRITE)
    {
        if (pJob->pMsgHost)
        {
            pHcmdMsg = (MsgSI_t*)pJob->pMsgHost;

            if (!(gSysInfoMgr.mgrBitMap & SI_S_WRITE_FAIL))
            {
                #ifdef SI_MS_NAND_QUICKSEARCH
                SyIn_NAND_QuickStore(pHcmdMsg->quickIdx);
                #endif
            }
            BIT_CLR(gSysInfoMgr.mgrBitMap, SI_S_WRITE);

            pHcmdMsg->area         = 0;
            pHcmdMsg->hdr.b.status = cMsgCompleted;
        }
    }
}
//-----------------------------------------------------------------------------
/**
    Cache Track Done: Read cmd without cache hit

    @param[in]  pJob
**/
//-----------------------------------------------------------------------------
void Cache_TrkDone_ReadNoHit(tFIO_JOB* pJob)
{
    U16 cacheIdx;
    U16 count = 0;

    cacheIdx = pJob->ssdIdx[SSD_W_CACHE];

    while (count < pJob->ssdCnt[SSD_W_CACHE])
    {
        smCacheMgr.cacheEntry[cacheIdx].status &= ~CACHE_S_SEQ_MASK;
        smCacheMgr.cacheEntry[cacheIdx].status |= CACHE_S_XFER_READ;
        smCacheMgr.cacheEntry[cacheIdx].bitmap  = CACHE_BMP_ALL_VALID;

        cacheIdx = GET_SSD_NEXT_INDEX_FROM_INDEX(cacheIdx);
        count++;
    }
    Core_ProcHostXfer(pJob->pMsgHost);
}

//-----------------------------------------------------------------------------
/**
    Cache Track Done: Read cmd with partial cache hit

    @param[in]  pJob
**/
//-----------------------------------------------------------------------------
void Cache_TrkDone_ReadPartialHit(tFIO_JOB* pJob)
{
    tSSD *pHead;
    tSSD *pTail;
    U32   cacheIdx;
    U16   count;

    cacheIdx = pJob->ssdIdx[SSD_W_CACHE];
    pHead    = GET_SSD_PTR_FROM_INDEX(cacheIdx);
    count    = 0;

    while (count < pJob->ssdCnt[SSD_W_CACHE])
    {
        smCacheMgr.cacheEntry[cacheIdx].status &= ~CACHE_S_SEQ_MASK;
        smCacheMgr.cacheEntry[cacheIdx].status |= CACHE_S_XFER_READ;
        smCacheMgr.cacheEntry[cacheIdx].bitmap  = CACHE_BMP_ALL_VALID;

        pTail    = &smSSD[cacheIdx];
        cacheIdx = pTail->nextIndex;
        count++;
    }

    cacheIdx = pJob->ssdIdx[SSD_WO_CACHE];
    pHead    = GET_SSD_PTR_FROM_INDEX(cacheIdx);
    count    = 0;

    while (count < pJob->ssdCnt[SSD_WO_CACHE])
    {
        pTail    = &smSSD[cacheIdx];
        cacheIdx = pTail->nextIndex;
        count++;
    }

    Cache_PushSSD_Other(SSD_POOL_OTHER, pHead, pTail, count);

    Core_ProcHostXfer(pJob->pMsgHost);
}

//-----------------------------------------------------------------------------
/**
    Cache Track Done: Read cmd with full cache hit

    @param[in]  pJob
**/
//-----------------------------------------------------------------------------
void Cache_TrkDone_ReadFillupHit(tFIO_JOB* pJob)
{
    tSSD *pHead;
    tSSD *pTail;
    U32  cacheIdx;
    U16  count;

    //-----------------------------------------------------------------------------
    smFioInfo.fillupJobCnt--;

    //-----------------------------------------------------------------------------
    cacheIdx = pJob->ssdIdx[SSD_W_CACHE];
    pHead    = GET_SSD_PTR_FROM_INDEX(cacheIdx);
    count    = 0;

    while (count < pJob->ssdCnt[SSD_W_CACHE])
    {
        smCacheMgr.cacheEntry[cacheIdx].status &= ~CACHE_S_SEQ_MASK;
        smCacheMgr.cacheEntry[cacheIdx].pool    = SSD_POOL_WRITE;
        smCacheMgr.cacheEntry[cacheIdx].bitmap  = CACHE_BMP_ALL_VALID;

        pTail    = &smSSD[cacheIdx];
        cacheIdx = pTail->nextIndex;
        count++;
    }

    Cache_PushSSD(SSD_POOL_WRITE, pHead, pTail, count);

    cacheIdx = pJob->ssdIdx[SSD_WO_CACHE];
    pHead    = GET_SSD_PTR_FROM_INDEX(cacheIdx);
    count    = 0;

    while (count < pJob->ssdCnt[SSD_WO_CACHE])
    {
        pTail    = &smSSD[cacheIdx];
        cacheIdx = pTail->nextIndex;
        count++;
    }

    Cache_PushSSD_Other(SSD_POOL_OTHER, pHead, pTail, count);
}

//-----------------------------------------------------------------------------
/**
    Cache Track Done: Flush

    @param[in]  pJob
**/
//-----------------------------------------------------------------------------
void Cache_TrkDone_Flush(tFIO_JOB* pJob)
{
    tSSD *pHead;
    tSSD *pTail;
    U32  cacheIdx;
    U16  count = 0;

    cacheIdx = pJob->ssdIdx[SSD_W_CACHE];
    pHead    = GET_SSD_PTR_FROM_INDEX(cacheIdx);

    while (count < pJob->ssdCnt[SSD_W_CACHE])
    {
        smCacheMgr.cacheEntry[cacheIdx].status = 0;
        smCacheMgr.cacheEntry[cacheIdx].pool   = SSD_POOL_FREE;

        pTail    = &smSSD[cacheIdx];
        cacheIdx = pTail->nextIndex;
        count++;
    }

    Cache_PushSSD(SSD_POOL_FREE, pHead, pTail, count);
}

//-----------------------------------------------------------------------------
/**
    Cache Track Done: GC ReadAux

    @param[in]  pJob
**/
//-----------------------------------------------------------------------------
void Cache_TrkDone_GCReadAux(tFIO_JOB* pJob)
{
    U32 cacheIdx;
    U16 idx = 0;

    MemCopyNU32((U32*)(gFastAuxBuf), (U32*)pJob->aux, sizeof(U32)  * pJob->ssdCnt[SSD_W_CACHE] * 2);

    cacheIdx = pJob->ssdIdx[SSD_W_CACHE];

    while (idx < pJob->ssdCnt[SSD_W_CACHE])
    {
        if (gFastAuxBuf[(idx << 1)] >= L2P_NUM && gFastAuxBuf[(idx << 1)] != DWORD_MASK)
        {
            D_PRINTF("ERR AuxLAA[%x]:[%x]\n", gFastAuxBuf[(idx << 1)], (idx << 1));
            smCacheMgr.cacheEntry[cacheIdx].laa = DWORD_MASK;
        }
        else
        {
            smCacheMgr.cacheEntry[cacheIdx].laa = gFastAuxBuf[(idx << 1)];
        }

        cacheIdx = GET_SSD_NEXT_INDEX_FROM_INDEX(cacheIdx);
        idx++;
    }

    gGCAux.donecnt++;

    if (gGCAux.donecnt >= (GC_MAX_AUX_JOB))
    {
        tSSD *pHead;
        tSSD *pTail;
        tPAA  paa;
        U16  jobIdx;
        U16  pool;
        U16  count;
        U16  hitCacheIdx;

        for (jobIdx = 0; jobIdx < GC_MAX_AUX_JOB; jobIdx++)
        {
            pJob     = gGCAux.pJobbuf[jobIdx];
            cacheIdx = pJob->ssdIdx[SSD_W_CACHE];
            pHead    = GET_SSD_PTR_FROM_INDEX(cacheIdx);
            pool     = SSD_POOL_NULL;
            count    = 0;
            idx      = 0;

            MemCopyNU32((U32*)(gFastPaaBuf), (U32*)pJob->paa, sizeof(tPAA) * pJob->ssdCnt[SSD_W_CACHE]);

            while (idx < pJob->ssdCnt[SSD_W_CACHE])
            {
                smCacheMgr.cacheEntry[cacheIdx].status &= ~CACHE_S_SEQ_MASK;

                if ((smCacheMgr.cacheEntry[cacheIdx].status & CACHE_S_ERROR) || (smCacheMgr.cacheEntry[cacheIdx].laa == DWORD_MASK))
                {
                    if (pool != SSD_POOL_FREE)
                    {
                        if (count)
                        {
                            Cache_PushSSD(pool, pHead, pTail, count);
                        }
                        count = 0;
                        pool  = SSD_POOL_FREE;
                        pHead = GET_SSD_PTR_FROM_INDEX(cacheIdx);
                    }

                    smCacheMgr.cacheEntry[cacheIdx].status = 0;
                    smCacheMgr.cacheEntry[cacheIdx].pool   = SSD_POOL_FREE;
                    smCacheMgr.cacheEntry[cacheIdx].bitmap = CACHE_BMP_ALL_INVALID;
                    smCacheMgr.cacheEntry[cacheIdx].laa    = DWORD_MASK;
                }
                else if (pJob->flag & FIO_F_BOOT)
                {
                    U32 cacheIdx_hit;

                    if (pool != SSD_POOL_GC)
                    {
                        if (count)
                        {
                            Cache_PushSSD(pool, pHead, pTail, count);
                        }
                        count = 0;
                        pool  = SSD_POOL_GC;
                        pHead = GET_SSD_PTR_FROM_INDEX(cacheIdx);
                    }

                    smCacheMgr.cacheEntry[cacheIdx].pool   = SSD_POOL_GC;
                    smCacheMgr.cacheEntry[cacheIdx].bitmap = CACHE_BMP_ALL_VALID;

                    cacheIdx_hit = Cache_Search(smCacheMgr.cacheEntry[cacheIdx].laa);
                    if (cacheIdx_hit)
                    {
                        Cache_Delete(cacheIdx_hit);
                        smCacheMgr.cacheEntry[cacheIdx_hit].bitmap  = CACHE_BMP_ALL_INVALID;
                        smCacheMgr.cacheEntry[cacheIdx_hit].laa     = DWORD_MASK;

                    }

                    Cache_Insert(cacheIdx);
                }
                else if ((Cache_Search(smCacheMgr.cacheEntry[cacheIdx].laa) == 0))  // cache no hit
                {
                    paa = gFtlL2PTbl[smCacheMgr.cacheEntry[cacheIdx].laa];

                    if ((paa.all32 == gFastPaaBuf[idx].all32))  // if read back has the same paa as FTL (valid)
                    {
                        if (pool != SSD_POOL_GC)
                        {
                            if (count)
                            {
                                Cache_PushSSD(pool, pHead, pTail, count);
                            }
                            count = 0;
                            pool  = SSD_POOL_GC;
                            pHead = GET_SSD_PTR_FROM_INDEX(cacheIdx);
                        }

                        smCacheMgr.cacheEntry[cacheIdx].pool   = SSD_POOL_GC;
                        smCacheMgr.cacheEntry[cacheIdx].bitmap = CACHE_BMP_ALL_VALID;

                        Cache_Insert(cacheIdx);
                    }
                    else  // if read back has different paa as FTL (invalid)
                    {
                        if (pool != SSD_POOL_FREE)
                        {
                            if (count)
                            {
                                Cache_PushSSD(pool, pHead, pTail, count);
                            }
                            count = 0;
                            pool  = SSD_POOL_FREE;
                            pHead = GET_SSD_PTR_FROM_INDEX(cacheIdx);
                        }

                        smCacheMgr.cacheEntry[cacheIdx].status = 0;
                        smCacheMgr.cacheEntry[cacheIdx].pool   = SSD_POOL_FREE;
                        smCacheMgr.cacheEntry[cacheIdx].bitmap = CACHE_BMP_ALL_INVALID;
                        smCacheMgr.cacheEntry[cacheIdx].laa    = DWORD_MASK;
                    }
                }
                else  // exist in cache (invalid)
                {
                    hitCacheIdx = Cache_Search(smCacheMgr.cacheEntry[cacheIdx].laa);

                    if (smCacheMgr.cacheEntry[hitCacheIdx].pool == SSD_POOL_FILLUP)
                    {
                        smCacheMgr.forceFillup = TRUE;
                    }
                    else if (smCacheMgr.cacheEntry[hitCacheIdx].pool == SSD_POOL_FREE)
                    {
                        if (smCacheMgr.cacheEntry[hitCacheIdx].status == 0)
                        {
                            smCacheMgr.cacheEntry[hitCacheIdx].pool = SSD_POOL_GC;
                            Cache_PopSSD(SSD_POOL_FREE, &smSSD[hitCacheIdx], &smSSD[hitCacheIdx], 1);
                            Cache_PushSSD(SSD_POOL_GC, &smSSD[hitCacheIdx], &smSSD[hitCacheIdx], 1);
                        }
                        else if (smCacheMgr.cacheEntry[hitCacheIdx].status & (CACHE_S_SEQ_READ | CACHE_S_XFER_READ))
                        {
                            smCacheMgr.cacheEntry[hitCacheIdx].pool = SSD_POOL_GC;
                        }
                        else if (smCacheMgr.cacheEntry[hitCacheIdx].status & CACHE_S_XFER_WRITE)
                        {
                            smCacheMgr.forceFillup = TRUE;
                        }
                    }

                    if (pool != SSD_POOL_FREE)
                    {
                        if (count)
                        {
                            Cache_PushSSD(pool, pHead, pTail, count);
                        }

                        count = 0;
                        pool  = SSD_POOL_FREE;
                        pHead = GET_SSD_PTR_FROM_INDEX(cacheIdx);
                    }

                    smCacheMgr.cacheEntry[cacheIdx].status = 0;
                    smCacheMgr.cacheEntry[cacheIdx].pool   = SSD_POOL_FREE;
                    smCacheMgr.cacheEntry[cacheIdx].bitmap = CACHE_BMP_ALL_INVALID;
                    smCacheMgr.cacheEntry[cacheIdx].laa    = DWORD_MASK;
                }

                pTail    = &smSSD[cacheIdx];
                cacheIdx = pTail->nextIndex;
                count++;
                idx++;
            }

            Cache_PushSSD(pool, pHead, pTail, count);
        }

        gGCAux.donecnt = 0;
    }
}

//-----------------------------------------------------------------------------
/**
    Cache Track Done: GC Read

    @param[in]  pJob
**/
//-----------------------------------------------------------------------------
void Cache_TrkDone_GCRead(tFIO_JOB* pJob)
{
    tSSD  *pHead;
    tSSD  *pTail;
    U32   cacheIdx;
    U16   count = 0;

    cacheIdx = pJob->ssdIdx[SSD_W_CACHE];
    pHead    = GET_SSD_PTR_FROM_INDEX(cacheIdx);

    while (count < pJob->ssdCnt[SSD_W_CACHE])
    {
        smCacheMgr.cacheEntry[cacheIdx].status &= ~CACHE_S_SEQ_MASK;
        smCacheMgr.cacheEntry[cacheIdx].pool    = SSD_POOL_GC;
        smCacheMgr.cacheEntry[cacheIdx].bitmap  = CACHE_BMP_ALL_VALID;

        pTail    = &smSSD[cacheIdx];
        cacheIdx = pTail->nextIndex;
        count++;
    }

    Cache_PushSSD(SSD_POOL_GC, pHead, pTail, count);
}

//-----------------------------------------------------------------------------
/**
    Cache Track Done: GC read last page

    @param[in]  pJob
**/
//-----------------------------------------------------------------------------
void Cache_TrkDone_GCLastPage(tFIO_JOB* pJob)
{
    tPAA paa;
    U8   p2lIdx;

    paa = pJob->paa[0];
    for (p2lIdx = 0; p2lIdx < GC_PING_PONG_NUM; p2lIdx++)
    {
        if (gGCppBuf[p2lIdx].blk_Info.blk == GC_Get_RmpBlk(paa))  {break;}
    }

    ASSERT(p2lIdx < GC_PING_PONG_NUM);

    if (pJob->status & FIO_ERR_RETRY_MASK)  //TODO : Need complete no/err last page
    {
        gGCppBuf[p2lIdx].lp_Info.errorCnt++;
    }

    gGCppBuf[p2lIdx].lp_Info.doneCnt++;
    Cache_TrkDone_Auto(pJob);
}

//-----------------------------------------------------------------------------
/**
    Cache Track Done: Pre-Read

    @param[in]  pJob
**/
//-----------------------------------------------------------------------------
void Cache_TrkDone_PreRead(tFIO_JOB* pJob)
{
    tSSD *pHead;
    tSSD *pTail;
    U16   idx = 0;
    U16   cnt = 0;
    U16   pool;
    U16   cacheIdx;

    cacheIdx       = pJob->ssdIdx[SSD_W_CACHE];
    pHead = pTail  = &smSSD[cacheIdx];
    pool           = smCacheMgr.cacheEntry[cacheIdx].pool;

    while (idx < pJob->ssdCnt[SSD_W_CACHE])
    {
        if (pool == smCacheMgr.cacheEntry[cacheIdx].pool)
        {
            cnt++;
            pTail = &smSSD[cacheIdx];
        }
        else
        {
            Cache_PushSSD(pool, pHead, pTail, cnt);
            //---------------------------------------------------------------------------------
            pool          = smCacheMgr.cacheEntry[cacheIdx].pool;
            cnt           = 1;
            pHead = pTail = &smSSD[cacheIdx];
        }

        smCacheMgr.cacheEntry[cacheIdx].status &= ~CACHE_S_SEQ_MASK;
        smCacheMgr.cacheEntry[cacheIdx].bitmap  = CACHE_BMP_ALL_VALID;
        cacheIdx = pTail->nextIndex;
        idx++;
    }

    //--- finalization --------------------------------------------------------------------------
    Cache_PushSSD(pool, pHead, pTail, cnt);
}

//-----------------------------------------------------------------------------
/**
    Cache Track Done: Do nothing

    @param[in]  pJob
**/
//-----------------------------------------------------------------------------
void Cache_TrkDone_DoNothing(tFIO_JOB* pJob)
{

}

//-----------------------------------------------------------------------------
/**
    Cache Track Error: Exception handler

    @param[in]  pJob
**/
//-----------------------------------------------------------------------------
void Cache_TrkError_Exception(tFIO_JOB* pJob)
{
    D_PRINTF("CSS[%d]\n", pJob->css);
    ASSERT(0);
}

//-----------------------------------------------------------------------------
/**
    Cache Track Error: Auto

    @param[in]  pJob
**/
//-----------------------------------------------------------------------------
void Cache_TrkError_Auto(tFIO_JOB* pJob)
{
    if ((pJob->flag & FIO_F_NO_MKERR) == 0)
    {
        ErrHandle_MarkDefect(pJob);
    }

    gCbfCacheTrkDone[pJob->css](pJob);
}

//-----------------------------------------------------------------------------
/**
    Cache Track Error: SysInfo

    @param[in]  pJob
**/
//-----------------------------------------------------------------------------
void Cache_TrkError_SysInfo(tFIO_JOB* pJob)
{
    MsgSI_t *pHcmdMsg;

    Cache_TrkDone_Auto(pJob);
    if (pJob->cmd != FIO_JOB_CMD_READ_SLC)
    {
        if (pJob->cmd == FIO_JOB_CMD_WRITE)
        {
            BIT_SET(gSysInfoMgr.mgrBitMap, SI_S_WRITE_FAIL);
            gSysInfoMgr.bBadBank[SyIn_NAND_BankIdx(pJob->paa[0])] = TRUE; //Mark BadBank
            SyIn_Synchronize(SI_AREA_ALL_MASK, SYSINFO_WRITE, NULL);
            if (pJob->pMsgHost)
            {
                pHcmdMsg = (MsgSI_t*)pJob->pMsgHost;

                pHcmdMsg->area         = 0;
                pHcmdMsg->hdr.b.status = cMsgCompleted;
                BIT_CLR(gSysInfoMgr.mgrBitMap, SI_S_WRITE);
            }
        }
        else if (pJob->cmd == FIO_JOB_CMD_ERASE)
        {
            BIT_CLR(gSysInfoMgr.mgrBitMap, SI_S_ERASE);
            gSysInfoMgr.bBadBank[SyIn_NAND_BankIdx(pJob->paa[0])] = TRUE;
        }
        MEM_COPY(&smSysInfo->d.MiscData.d.InfoMgr.d.SIBadBank, &gSysInfoMgr.bBadBank, (SI_NAND_MAX_CE * SI_NAND_MAX_LUN * SI_NAND_MAX_CHANNEL));
    }
}

//-----------------------------------------------------------------------------
/**
    Cache Track Error: IDS

    @param[in]  pJob
**/
//-----------------------------------------------------------------------------
void Cache_TrkError_ReadIDS(tFIO_JOB* pJob)
{
    // TODO:: really necesary to mark ids error ?
    #if 0
    if ((pJob->flag & FIO_F_NO_MKERR) == 0)
    {
        ErrHandle_MarkError(pJob);
        ErrHandle_MarkDefect(pJob);
    }
    #endif

    // TODO:: trigger IDS write back here?
    // TODO:: trigger early move here?
    pJob->pMsgHost->error = cEcIdsReadFail;

    IPC_SendMsgQ(cC2hComQueue, (Msg_t*)pJob->pMsgHost);

    gCbfCacheTrkDone[pJob->css](pJob);
}

//-----------------------------------------------------------------------------
/**
    Cache Track Error: Read not hit

    @param[in]  pJob
**/
//-----------------------------------------------------------------------------
void Cache_TrkError_ReadNoHit(tFIO_JOB* pJob)
{
    if ((pJob->flag & FIO_F_NO_MKERR) == 0)
    {
        ErrHandle_MarkError(pJob);
        ErrHandle_MarkDefect(pJob);
    }

    if (pJob->pMsgHost)
    {
        pJob->pMsgHost->error = cEcError;
    }

    gCbfCacheTrkDone[pJob->css](pJob);
}

//-----------------------------------------------------------------------------
/**
    Cache Track Error: Flush

    @param[in]  pJob
**/
//-----------------------------------------------------------------------------
void Cache_TrkError_Flush(tFIO_JOB* pJob)
{
    tSSD *pHead;
    tSSD *pTail;
    U32  cacheIdx;
    U16  count = 0;
    U16  pool  = 0;

    cacheIdx = pJob->ssdIdx[SSD_W_CACHE];
    pHead    = GET_SSD_PTR_FROM_INDEX(cacheIdx);
    pool     = smCacheMgr.cacheEntry[cacheIdx].pool;

    while (count < pJob->ssdCnt[SSD_W_CACHE])
    {
        smCacheMgr.cacheEntry[cacheIdx].status &= ~CACHE_S_SEQ_MASK;

        pTail = &smSSD[cacheIdx];
        cacheIdx = pTail->nextIndex;
        count++;
    }

    Cache_PushSSD(pool, pHead, pTail, count);

    if ((pJob->flag & FIO_F_NO_MKERR) == 0)
    {
        ErrHandle_MarkDefect(pJob);
    }
}

//-----------------------------------------------------------------------------
/**
    Pop SSD from specific pool

    @param[in]  SSD pool
    @param[in]  head pointer of SSD
    @param[in]  tail pointer of SSD
    @param[in]  SSD count
**/
//-----------------------------------------------------------------------------
void Cache_PopSSD(U16 pool, tSSD* pStartSSD, tSSD* pEndSSD, U16 Count)
{
    ASSERT(pool < SSD_POOL_OTHER);

    CACHE_BE_SSD_PROTECT_START
    ASSERT(smCacheMgr.ssdCnt[pool] >= Count);

    if (smCacheMgr.head[pool] == pStartSSD && smCacheMgr.tail[pool] == pEndSSD)
    {
        smCacheMgr.head[pool] = smCacheMgr.tail[pool] = NULL;
    }
    else if(smCacheMgr.head[pool] == pStartSSD)
    {
        smCacheMgr.head[pool]                 = &smSSD[pEndSSD->nextIndex];
        smCacheMgr.head[pool]->prevIndex      = SSD_NULL_START;
    }
    else if(smCacheMgr.tail[pool] == pEndSSD)
    {
        smCacheMgr.tail[pool]                 = &smSSD[pStartSSD->prevIndex];
        smCacheMgr.tail[pool]->nextIndex      = SSD_NULL_START;
    }
    else
    {
        smSSD[pStartSSD->prevIndex].nextIndex = pEndSSD->nextIndex;
        smSSD[pEndSSD->nextIndex].prevIndex   = pStartSSD->prevIndex;
    }

    pStartSSD->prevIndex = SSD_NULL_START;
    pEndSSD->nextIndex   = SSD_NULL_START;

    #if (_NAND_ == NAND_3DTLCG2)
        if ((pool > SSD_POOL_WRITE) && (pool < SSD_POOL_GC))
        {
            smCacheMgr.ssdCnt[SSD_POOL_WRITE] -= Count;
        }
    #endif

    smCacheMgr.ssdCnt[pool] -= Count;
    CACHE_BE_SSD_PROTECT_END
}

//-----------------------------------------------------------------------------
/**
    Pop SSD into specific pool

    @param[in]  SSD pool
    @param[in]  head pointer of SSD
    @param[in]  tail pointer of SSD
    @param[in]  SSD count
**/
//-----------------------------------------------------------------------------
void Cache_PushSSD(U16 pool, tSSD* pStartSSD, tSSD* pEndSSD, U16 Count)
{
    ASSERT(pool < SSD_POOL_OTHER);

    CACHE_BE_SSD_PROTECT_START
    if (smCacheMgr.head[pool])
    {
        smCacheMgr.tail[pool]->nextIndex = GET_SSD_INDEX_FROM_PTR(pStartSSD);
        pStartSSD->prevIndex             = GET_SSD_INDEX_FROM_PTR(smCacheMgr.tail[pool]);
        smCacheMgr.tail[pool]            = pEndSSD;
        pEndSSD->nextIndex               = SSD_NULL_START;
    }
    else
    {
        smCacheMgr.head[pool] = pStartSSD;
        smCacheMgr.tail[pool] = pEndSSD;
        pStartSSD->prevIndex  = SSD_NULL_START;
        pEndSSD->nextIndex    = SSD_NULL_START;
    }

    #if (_NAND_ == NAND_3DTLCG2)
        if ((pool > SSD_POOL_WRITE) && (pool < SSD_POOL_GC))
        {
            smCacheMgr.ssdCnt[SSD_POOL_WRITE] += Count;
        }
    #endif

    smCacheMgr.ssdCnt[pool] += Count;
    CACHE_BE_SSD_PROTECT_END
}

//-----------------------------------------------------------------------------
/**
    Generate or modify zero cache

    @param[in]  laa
    @param[in]  bitmap
**/
//-----------------------------------------------------------------------------
U32 Cache_WriteZero(U32 laa, U16 bitmap)
{
    tSSD* pSSD;
    U16   cacheIdx;
    U16   idx;

    cacheIdx = Cache_Search(laa);

    if (cacheIdx)
    {
        if (smCacheMgr.cacheEntry[cacheIdx].status & CACHE_S_ABORT_MASK)
        {
            return FALSE;
        }
        else if (smCacheMgr.cacheEntry[cacheIdx].pool == SSD_POOL_FREE)
        {
            pSSD = GET_SSD_PTR_FROM_INDEX(cacheIdx);

            smCacheMgr.cacheEntry[cacheIdx].status = 0;
            smCacheMgr.cacheEntry[cacheIdx].pool   = SSD_POOL_WRITE;

            Cache_PopSSD(SSD_POOL_FREE, pSSD, pSSD, 1);
            Cache_PushSSD(SSD_POOL_WRITE, pSSD, pSSD, 1);
        }
    }
    else
    {
        if (smCacheMgr.ssdCnt[SSD_POOL_FREE])
        {
            pSSD     = smCacheMgr.head[SSD_POOL_FREE];
            cacheIdx = GET_SSD_INDEX_FROM_PTR(pSSD);

            if (smCacheMgr.cacheEntry[cacheIdx].laa != DWORD_MASK)
            {
                Cache_Delete(cacheIdx);
            }

            smCacheMgr.cacheEntry[cacheIdx].status  = 0;
            smCacheMgr.cacheEntry[cacheIdx].pool    = SSD_POOL_FILLUP;
            smCacheMgr.cacheEntry[cacheIdx].laa     = laa;
            smCacheMgr.cacheEntry[cacheIdx].bitmap  = bitmap;
            Cache_Insert(cacheIdx);

            Cache_PopSSD(SSD_POOL_FREE, pSSD, pSSD, 1);
            Cache_PushSSD(SSD_POOL_FILLUP, pSSD, pSSD, 1);
        }
        else
        {
            return FALSE;
        }
    }

    for (idx = 0; idx < HBU_PER_MBU; idx++)
    {
        if (smCacheMgr.cacheEntry[cacheIdx].bitmap & BIT(idx))
        {
            MEM_COPY(&smCache[cacheIdx * MBU_SIZE + idx * HLBU_DATA_SIZE], &smCache[SSD_ZERO_START * MBU_SIZE], HLBU_DATA_SIZE);
        }
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
/**
    Get zero paa

    @param[in]  laa

    @return     paa address of zero pattern
**/
//-----------------------------------------------------------------------------
tPAA Cache_GetZeroPAA(U32 laa)
{
    tPAA paa;

    #if _TCG_ == TCG_EDRV
        paa = FTL_PhyAddrSearch(gExtLaaStart + HAL_MRE_GetRangeNo(laa << HBU_PER_MBU_SFT));
    #else
        paa = FTL_PhyAddrSearch(gExtLaaStart);
    #endif

    if (paa.all32 == DWORD_MASK) ASSERT(0);

    return paa;
}

//-----------------------------------------------------------------------------
/**
    Merge two cache pool

    @param[in]  poolFrom
    @param[in]  poolTo
**/
//-----------------------------------------------------------------------------
void Cache_MergePool(U16 poolFrom, U16 poolTo)
{
    if (smCacheMgr.ssdCnt[poolFrom])
    {
        tSSD *pHead;
        tSSD *pTail;
        U16   cacheIdx;
        U16   count = 0;

        pHead    = smCacheMgr.head[poolFrom];
        cacheIdx = GET_SSD_INDEX_FROM_PTR(pHead);

        while (count < smCacheMgr.ssdCnt[poolFrom])
        {
            smCacheMgr.cacheEntry[cacheIdx].status = 0;
            smCacheMgr.cacheEntry[cacheIdx].pool   = poolTo;
            pTail    = &smSSD[cacheIdx];
            cacheIdx = pTail->nextIndex;
            count++;
        }

        Cache_PopSSD(poolFrom, pHead, pTail, count);
        Cache_PushSSD(poolTo , pHead, pTail, count);
    }
}

#include "NormSection.h"
//-----------------------------------------------------------------------------
/**
    Dump and check cache information

    @param[in]  level: 0        => cache check only
                level: Non-Zero => cache check with F/R/W/FU cache info
                level: F        => cahhe check with all cache info
**/
//-----------------------------------------------------------------------------
void Cache_DumpInfo(U16 level)
{
    CSTR str[SSD_POOL_COUNT]   = {"FR", "FI", "W0", "W1", "W2", "W3", "GC", "OT",  "RR"};

    tSSD* pSSD;
    U16 cacheIdx;
    U16 list, idx, ssdCnt;
    U16 count, error;
    U16 seqBusy, xferBusy;

    #define CACHE_ERR_POOL_MISMATCH         B_0
    #define CACHE_ERR_COUNT_MISMATCH        B_1
    #define CACHE_ERR_SSD_TAIL_MISMATCH     B_2
    #define CACHE_ERR_SSD_HEAD_MISMATCH     B_3
    #define CACHE_ERR_SEQ_XFER_BUSY         B_4

    #define CACHE_DUMP_ALL                  0xA

    // ------------------------------------------------------
    for (list = 0; list < SSD_POOL_COUNT; list++)
    {
        count    = 0;
        error    = FALSE;
        seqBusy  = 0;
        xferBusy = 0;

        ssdCnt = smCacheMgr.ssdCnt[list];

        #if _NAND_ == NAND_3DTLCG2
            if (list == SSD_POOL_WRITE)
            {
                for (count = 1; count < WRITE_CACHE_SPLIT_POOL_CNT; count++)
                {
                    ssdCnt -= smCacheMgr.ssdCnt[SSD_POOL_WRITE + count];
                }
                count = 0;
            }
        #endif

        if (ssdCnt)
        {
            pSSD     = smCacheMgr.head[list];
            cacheIdx = GET_SSD_INDEX_FROM_PTR(pSSD);
            while (cacheIdx)
            {
                if (list < SSD_POOL_OTHER)
                {
                    if (smCacheMgr.cacheEntry[cacheIdx].status & CACHE_S_SEQ_MASK)  seqBusy++;
                    if (smCacheMgr.cacheEntry[cacheIdx].status & CACHE_S_XFER_MASK) xferBusy++;
                    if (smCacheMgr.cacheEntry[cacheIdx].pool  != list) error |= CACHE_ERR_POOL_MISMATCH;
                }

                pSSD     = &smSSD[cacheIdx];
                cacheIdx = pSSD->nextIndex;
                count++;

                if (count == ssdCnt) break;
            }

            if (seqBusy || xferBusy)            error |= CACHE_ERR_SEQ_XFER_BUSY;
            if (pSSD != smCacheMgr.tail[list])  error |= CACHE_ERR_SSD_TAIL_MISMATCH;
            if (count != ssdCnt)                error |= CACHE_ERR_COUNT_MISMATCH;

            if (list < SSD_POOL_OTHER)
            {
                count    = 0;
                pSSD     = smCacheMgr.tail[list];
                cacheIdx = GET_SSD_INDEX_FROM_PTR(pSSD);
                while (cacheIdx)
                {
                    pSSD     = &smSSD[cacheIdx];
                    cacheIdx = pSSD->prevIndex;
                    count++;

                    if (count == ssdCnt) break;
                }

                if (pSSD != smCacheMgr.head[list])      error |= CACHE_ERR_SSD_HEAD_MISMATCH;
            }
        }

        if (error)
        {
            M_PRINTF("\n\t%s :%5d SEQ[%4d] XF[%4d] => ERR[%04X]", str[list], ssdCnt, seqBusy, xferBusy, error);
        }
        else
        {
            M_PRINTF("\n\t%s :%5d SEQ[%4d] XF[%4d] => OK", str[list], ssdCnt, seqBusy, xferBusy);
        }
    }

    #if (_NAND_==NAND_3DTLCG2)
        M_PRINTF("\n\tTotalW :%5d\n", smCacheMgr.ssdCnt[SSD_POOL_WRITE]);
    #endif

    if (level)
    {
        // ------------------------------------------------------
        for (list = 0; list < SSD_POOL_OTHER; list++)
        {
            ssdCnt = smCacheMgr.ssdCnt[list];

            #if _NAND_ == NAND_3DTLCG2
                if (list == SSD_POOL_WRITE)
                {
                    for (count = 1; count < WRITE_CACHE_SPLIT_POOL_CNT; count++)
                    {
                        ssdCnt -= smCacheMgr.ssdCnt[SSD_POOL_WRITE + count];
                    }
                    count = 0;
                }
            #endif

            if (ssdCnt)
            {
                M_PRINTF("\n P[%s] H[%4d] T[%4d] C:%d\n", str[list], GET_SSD_INDEX_FROM_PTR(smCacheMgr.head[list]), GET_SSD_INDEX_FROM_PTR(smCacheMgr.tail[list]), ssdCnt);
            }
            else
            {
                M_PRINTF("\n P[%s] H[NULL] T[NULL] C:0\n", str[list]);
            }

            pSSD = smCacheMgr.head[list];
            for (idx = 0; idx < ssdCnt; idx++)
            {
                cacheIdx = GET_SSD_INDEX_FROM_PTR(pSSD);
                M_PRINTF("   %4d: [%4d] <-- [%4d] --> [%4d] : S[%04X] P[%2d] L[%08X] B[%04X]\n", idx + 1, pSSD->prevIndex, cacheIdx, pSSD->nextIndex, smCacheMgr.cacheEntry[cacheIdx].status, smCacheMgr.cacheEntry[cacheIdx].pool, smCacheMgr.cacheEntry[cacheIdx].laa, smCacheMgr.cacheEntry[cacheIdx].bitmap);
                pSSD = GET_SSD_PTR_FROM_INDEX(pSSD->nextIndex);
            }
        }

        if (level == CACHE_DUMP_ALL)
        {
            // ------------------------------------------------------
            M_PRINTF("Cache SSD: %d\n", CACHE_TOTAL_COUNT);
            for (idx = 0; idx < CACHE_TOTAL_COUNT; idx++)
            {
                pSSD = &smSSD[idx];
                M_PRINTF("   %4d: [%4d] <-- [%4d] --> [%4d] S[%04X] P[%02d] L[%08X] B[%04X] S[%08X] E[%08X]\n", idx, pSSD->prevIndex, idx, pSSD->nextIndex, smCacheMgr.cacheEntry[idx].status, smCacheMgr.cacheEntry[idx].pool, smCacheMgr.cacheEntry[idx].laa, smCacheMgr.cacheEntry[idx].bitmap, smSSD[idx].startAddr, smSSD[idx].endAddr);
            }

            M_PRINTF("Other SSD: H[%4d] T[%d] C[%4d]\n", GET_SSD_INDEX_FROM_PTR(smCacheMgr.head[SSD_POOL_OTHER]), GET_SSD_INDEX_FROM_PTR(smCacheMgr.tail[SSD_POOL_OTHER]), smCacheMgr.ssdCnt[SSD_POOL_OTHER]);
            pSSD     = smCacheMgr.head[SSD_POOL_OTHER];
            for (idx = 0; idx < smCacheMgr.ssdCnt[SSD_POOL_OTHER]; idx++)
            {
                cacheIdx = GET_SSD_INDEX_FROM_PTR(pSSD);
                M_PRINTF("   %4d: [%4d] <-- [%4d] --> [%4d] S[%08X] E[%08X]\n", idx + 1, smSSD[cacheIdx].prevIndex, cacheIdx, smSSD[cacheIdx].nextIndex, smSSD[cacheIdx].startAddr, smSSD[cacheIdx].endAddr);
                pSSD = GET_SSD_PTR_FROM_INDEX(pSSD->nextIndex);
            }

            M_PRINTF("RR SSD: H[%4d] T[%d] C[%4d]\n", GET_SSD_INDEX_FROM_PTR(smCacheMgr.head[SSD_POOL_RR]), GET_SSD_INDEX_FROM_PTR(smCacheMgr.tail[SSD_POOL_RR]), smCacheMgr.ssdCnt[SSD_POOL_RR]);
            pSSD     = smCacheMgr.head[SSD_POOL_RR];
            for (idx = 0; idx < smCacheMgr.ssdCnt[SSD_POOL_RR]; idx++)
            {
                cacheIdx = GET_SSD_INDEX_FROM_PTR(pSSD);
                M_PRINTF("   %4d: [%4d] <-- [%4d] --> [%4d] S[%08X] E[%08X]\n", idx + 1, smSSD[cacheIdx].prevIndex, cacheIdx, smSSD[cacheIdx].nextIndex, smSSD[cacheIdx].startAddr, smSSD[cacheIdx].endAddr);
                pSSD = GET_SSD_PTR_FROM_INDEX(pSSD->nextIndex);
            }
        }
    }
}
#endif

#if (_CPU_==CPU_ID1) || (_CPU_==CPU_ID2)
#include "PerfSection.h"
//-----------------------------------------------------------------------------
/**
    Cache allocation: Fillup SSDs

    @param[in]  pJob
    @param[in]  cache index
    @param[in]  next other cache index
    @param[in]  pointer of number of paa per fillup

    @return last other cache index
**/
//-----------------------------------------------------------------------------
U16 Cache_Alloc_FillupSSD(tFIO_JOB* pJob, U16 cacheIdx, U16 nextOtherIdx, U16* paaPerFillup)
{
    #ifdef LIMIT_NUM_OF_SSD_PER_AU_WORKAROUND
    U64 groupMap;
    U16 bitType;
    U16 currGroupBits;
    U16 otherIdx;
    U16 checkedBits;
    U16 usedSSDCnt;
    U16 tempBmp;
    U16 idx;

    tempBmp = smCacheMgr.cacheEntry[cacheIdx].bitmap;

    do {
        usedSSDCnt  = 0;
        checkedBits = 0;
        groupMap = Cache_Convert_Bitmap(tempBmp);
        bitType  = (tempBmp & B_0);

        do {
            usedSSDCnt++;
            currGroupBits = groupMap & CACHE_FIILUP_B2C_MASK;

            if (usedSSDCnt == MAX_SSD_PER_AU && (checkedBits + currGroupBits) < HBU_PER_MBU)
            {
                smSSD[nextOtherIdx].prevIndex = SSD_DUMMY_START;
                smSSD[nextOtherIdx].startAddr = smSSD[SSD_DUMMY_START].startAddr;
                smSSD[nextOtherIdx].endAddr   = smSSD[SSD_DUMMY_START].startAddr + (HLBU_DATA_SIZE >> SSD_ADDR_SHIFT) * (HBU_PER_MBU - checkedBits) - 1;
            }
            else if (bitType)
            {
                smSSD[nextOtherIdx].prevIndex = SSD_DUMMY_START;
                smSSD[nextOtherIdx].startAddr = smSSD[SSD_DUMMY_START].startAddr;
                smSSD[nextOtherIdx].endAddr   = smSSD[SSD_DUMMY_START].startAddr + (HLBU_DATA_SIZE >> SSD_ADDR_SHIFT) * currGroupBits - 1;
                bitType = FALSE;
            }
            else
            {
                smSSD[nextOtherIdx].prevIndex = cacheIdx;
                smSSD[nextOtherIdx].startAddr = smSSD[cacheIdx].startAddr + checkedBits                   * (HLBU_DATA_SIZE >> SSD_ADDR_SHIFT);
                smSSD[nextOtherIdx].endAddr   = smSSD[cacheIdx].startAddr + (checkedBits + currGroupBits) * (HLBU_DATA_SIZE >> SSD_ADDR_SHIFT) - 1;
                bitType = TRUE;

                // Update temporary bitmap
                for (idx = checkedBits; idx < (checkedBits + currGroupBits); idx++)
                {
                    BIT_SET(tempBmp, BIT(idx));
                }
            }

            otherIdx     = nextOtherIdx;
            nextOtherIdx = GET_SSD_NEXT_INDEX_FROM_INDEX(nextOtherIdx);
            pJob->ssdCnt[SSD_WO_CACHE]++;

            groupMap    >>= CACHE_FIILUP_B2C_SHIFT;
            checkedBits  += currGroupBits;
        } while ((usedSSDCnt < MAX_SSD_PER_AU) && (checkedBits < HBU_PER_MBU));

        (*paaPerFillup)++;

    } while (checkedBits < HBU_PER_MBU);

    return otherIdx;

    #else

    U64 groupMap;
    U16 bitType;
    U16 currGroupBits;
    U16 otherIdx;
    U16 checkedBits = 0;

    groupMap = Cache_Convert_Bitmap(smCacheMgr.cacheEntry[cacheIdx].bitmap);
    bitType  = (smCacheMgr.cacheEntry[cacheIdx].bitmap & B_0);

    do {
        currGroupBits = groupMap & CACHE_FIILUP_B2C_MASK;

        if (bitType)
        {
            smSSD[nextOtherIdx].prevIndex = SSD_DUMMY_START;
            smSSD[nextOtherIdx].startAddr = smSSD[SSD_DUMMY_START].startAddr;
            smSSD[nextOtherIdx].endAddr   = smSSD[SSD_DUMMY_START].startAddr + (HLBU_DATA_SIZE >> SSD_ADDR_SHIFT) * currGroupBits - 1;
            bitType = FALSE;
        }
        else
        {
            smSSD[nextOtherIdx].prevIndex = cacheIdx;
            smSSD[nextOtherIdx].startAddr = smSSD[cacheIdx].startAddr + checkedBits                   * (HLBU_DATA_SIZE >> SSD_ADDR_SHIFT);
            smSSD[nextOtherIdx].endAddr   = smSSD[cacheIdx].startAddr + (checkedBits + currGroupBits) * (HLBU_DATA_SIZE >> SSD_ADDR_SHIFT) - 1;
            bitType = TRUE;
        }

        otherIdx     = nextOtherIdx;
        nextOtherIdx = GET_SSD_NEXT_INDEX_FROM_INDEX(nextOtherIdx);
        pJob->ssdCnt[SSD_WO_CACHE]++;

        groupMap    >>= CACHE_FIILUP_B2C_SHIFT;
        checkedBits  += currGroupBits;
    } while (checkedBits < HBU_PER_MBU);

    (*paaPerFillup)++;

    return otherIdx;
    #endif
}

//-----------------------------------------------------------------------------
/**
    Convert cache bitmap to group map

    @param[in]  cache bitmap
    @return  cache group map
**/
//-----------------------------------------------------------------------------
U64 Cache_Convert_Bitmap(U16 bitmap)
{
    #if MBU_SIZE == (8 * KBYTE)
        U64 groupMap = 0, bitsCnt;
        U32 bitFlag, bit;
        U32 shiftNum;
        U32 i;

        bitFlag  = bitmap & 0x1;
        bitsCnt  = 0;
        shiftNum = 0;

        for (i = 0; i < HBU_PER_MBU; i++)
        {
            bit = (bitmap >> i) & 0x1;
            if (bit == bitFlag)
            {
                bitsCnt++;
            }
            else
            {
                groupMap |= (bitsCnt << shiftNum);

                bitFlag   = bit;
                bitsCnt   = 1;
                shiftNum += 4;
            }
        }

        groupMap |= (bitsCnt << shiftNum);

        return groupMap;

    #elif MBU_SIZE == (4* KBYTE)

        return gBmp2GroupMap[bitmap & CACHE_BMP_ALL_VALID];

    #endif
}

//-----------------------------------------------------------------------------
/**
    Pop other SSD from other pool

    @param[in]  SSD pool
    @param[in]  head pointer of SSD
    @param[in]  tail pointer of SSD
    @param[in]  SSD count
**/
//-----------------------------------------------------------------------------
void Cache_PopSSD_Other(U16 pool, tSSD* pHead, tSSD* pTail, U16 Count)
{
    ASSERT(smCacheMgr.ssdCnt[pool] >= Count);

    BEGIN_CS1
    if(smCacheMgr.tail[pool] == pTail)
    {
        smCacheMgr.head[pool] = smCacheMgr.tail[pool] = NULL;
    }
    else
    {
        smCacheMgr.head[pool] = &smSSD[pTail->nextIndex];
    }

    pTail->nextIndex = SSD_NULL_START;
    smCacheMgr.ssdCnt[pool] -= Count;
    END_CS1
}

//-----------------------------------------------------------------------------
/**
    Push other SSD into other pool

    @param[in]  SSD pool
    @param[in]  head pointer of SSD
    @param[in]  tail pointer of SSD
    @param[in]  SSD count
**/
//-----------------------------------------------------------------------------
void Cache_PushSSD_Other(U16 pool, tSSD* pHead, tSSD* pTail, U16 Count)
{
    ASSERT(smCacheMgr.ssdCnt[pool] < NUM_OF_SSD);

    BEGIN_CS1
    if (smCacheMgr.tail[pool])
    {
        smCacheMgr.tail[pool]->nextIndex = GET_SSD_INDEX_FROM_PTR(pHead);
        smCacheMgr.tail[pool]            = pTail;
    }
    else
    {
        smCacheMgr.head[pool] = pHead;
        smCacheMgr.tail[pool] = pTail;
    }

    pTail->nextIndex = SSD_NULL_START;
    smCacheMgr.ssdCnt[pool] += Count;
    END_CS1
}
#endif
