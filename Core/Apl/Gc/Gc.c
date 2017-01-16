//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"
#include "Fio.h"
#include "Cache.h"
#include "Core.h"
#include "ErrHandle.h"
#include "SysInfo.h"
#if (THERAML_TEMPERATURE_FROM_SOC == TRUE)  // EP2 || EP3
  #include "HalTempSensor.h"
#else
  #include "Thermal.h"                      // CX2
#endif
#include "Gc.h"

#include "PerfSection.h"

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static __inline U16      gc_getVicBlk(U8 ftlPool, U8 type);
static __inline tGC_MODE gc_getMode(void);
static __inline U32      gc_getBlkCache(U32 ValidCnt);
static __inline void     gc_setDtcmRmp(U16 block, U8 curP2lIdx);
static __inline void     gc_completeP2L(U8 curP2lIdx);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
__align(16) tGC_GEN_INFO            gGCInfo;
__align(16) tGC_PING_PONG_BUF_INFO  gGCppBuf[GC_PING_PONG_NUM];
__align(16) tGC_AUX                 gGCAux;

#if (GC_REMAP_MODE == TRUE)
tGC_REMAP_INFO gGCremapInfo;
#endif

#if (DBG_GC_LP == TRUE)
U32* gdwGCTestBuf;
#endif
U32* gdwGCLastPageBuf;

U8   gHstWeight_LoOP[16] = {0, 1, 2, 3, 5, 6, 7, 8, 9, 13, 17, 20, 27, 30, 30, 30};
U8   gHstWeight_HiOP[16] = {0, 2, 4, 8,10,12,13,14,15, 16, 17, 20, 27, 30, 30, 30};

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
#if (CHK_FW_TYPE(FW_AGING) == FALSE)

    // Ping Pong Buffer FSM
    void (*gCbfGC_PP_FSM[GC_PP_ST_CNT]) (U8 curP2lIdx) =
    {
        GC_PingPong_Idle,             // GC_PP_ST_IDLE
        GC_PingPong_Blk_Select,       // GC_PP_ST_BLK_SELECT
        GC_PingPong_P2L_Read,         // GC_PP_ST_P2L_READ
        GC_PingPong_P2L_Wait,         // GC_PP_ST_P2L_WAIT
        GC_PingPong_P2L_Done,         // GC_PP_ST_P2L_DONE
        GC_PingPong_Recycle,          // GC_PP_ST_RECYCLE
        GC_PingPong_Recycle_Done,     // GC_PP_ST_RECYCLE_DONE
        //GC_PingPong_Abort,          // GC_PP_ST_ABORT
    };

    // GC Mode Select
    #define GC_AllocTH_IDLE         GC_AllocTH_FORCE
    #define GC_AllocTH_STATIC_WL    GC_AllocTH_FORCE
    #define GC_AllocTH_EMGR         GC_AllocTH_SHUTTLE
    #define GC_AllocTH_AUX          GC_AllocTH_SHUTTLE

    void (*gCbfGC_Set_TH[GC_ST_CNT]) (U8 curP2lIdx) =
    {
        GC_AllocTH_NON,                 // GC_ST_NON
        GC_AllocTH_FORCE,               // GC_ST_FORCE
        GC_AllocTH_EMGR,                // GC_ST_EMGR
        GC_AllocTH_STATIC_WL,           // GC_ST_STATIC_WL
        GC_AllocTH_SHUTTLE,             // GC_ST_SHUTTLE
        GC_AllocTH_IDLE,                // GC_ST_IDLE
        GC_AllocTH_LOCK,                // GC_ST_LOCK
        GC_AllocTH_AUX,                 // GC_ST_AUX        .Exception mode
    };

//-------------------------------------------------------------------
// Function     : U16 gc_getVicBlk(U8 bFtlPool, U8 type)
// Description  :
// Input        : U8 bFtlPool, U8 type
// return       : U16
//-------------------------------------------------------------------
static __inline U16 gc_getVicBlk(U8 ftlPool, U8 type)
{
    U32 ComCnt;
    U32 VicCnt;
    U16 ComBlk;
    U16 VicBlk;
    U16 TailBlk;

    VicBlk  = ComBlk = gFtlBlkListInfo.head[ftlPool];
    TailBlk = gFtlBlkListInfo.tail[ftlPool];

    // Find first block meet not unsavebock and old data block.
    while (ComBlk != TailBlk)
    {
        if (!gFtlTrimInfo.trimBlocks[ComBlk] && (gFtlSNTable[ComBlk] < gFtlMgr.TableSN))
        {
            VicBlk = ComBlk;
            break;
        }
        ComBlk = gFtlBlkListTbl[ComBlk].block;
    }

    while (ComBlk != TailBlk)
    {
        if (!(gFtlTrimInfo.trimBlocks[ComBlk]) && (gFtlSNTable[ComBlk] < gFtlMgr.TableSN))
        {
            if (type == PICK_LOW_VC)
            {
                if (gFtlBlkListTbl[ComBlk].vs.b.vac < gFtlBlkListTbl[VicBlk].vs.b.vac)
                    VicBlk = ComBlk;
            }
            else
            {
                if (gFtlBlkListTbl[ComBlk].eraseCnt < gFtlBlkListTbl[VicBlk].eraseCnt)
                    VicBlk = ComBlk;
            }
        }
        ComBlk = gFtlBlkListTbl[ComBlk].block;
    }

    if (gFtlTrimInfo.trimBlocks[VicBlk])
        D_PRINTF("Err VicB[%4x]UNSAVE[%4x]SN[%x]TBSN[%x]\n", VicBlk, gFtlTrimInfo.trimBlocks[VicBlk], gFtlSNTable[VicBlk], gFtlMgr.TableSN);

    return (VicBlk);
}

//-------------------------------------------------------------------
// Function     : static __inline U32 gc_getMode(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static __inline tGC_MODE gc_getMode(void)
{
    tGC_MODE mode;

    if (gGCInfo.state.b.readOnly)
        mode = GC_ST_NON;
    else if (gGCInfo.mode.b.lockGC)
        mode = GC_ST_LOCK;
    else if (gGCInfo.mode.b.emgrGC)
        mode = GC_ST_EMGR;
    else if (gGCInfo.shuttleCnt)
    {
        mode = GC_ST_SHUTTLE;
        gGCInfo.shuttleCnt--;
    }
    else if (gGCInfo.mode.b.staticWL)
        mode = GC_ST_STATIC_WL;
    else if (gGCInfo.mode.b.idleGC)
    {
        // TODO: IDLE GC Entry point and do or no do.................
        //mode = GC_ST_IDLE;
        mode = GC_ST_NON;
    }
    else if (gGCInfo.mode.b.forceGC)
        mode = GC_ST_FORCE;
    else
        mode = GC_ST_NON;

    return (mode);
}

//-------------------------------------------------------------------
// Function     : U32 gc_getBlkCache(U32 ValidCnt)
// Description  :
// Input        : U32 ValidCnt
// return       : U32
//-------------------------------------------------------------------
static __inline U32 gc_getBlkCache(U32 ValidCnt)
{
    U32 opCache = ((GC_FTL_SBK_VALID_CNT - ValidCnt) * CACHE_RW_COUNT / GC_FTL_SBK_VALID_CNT);

    if (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_FREE] >= GC_BLKCNT_ACTIVE)
    {
        if (opCache > GC_BLK_OPRATIO_TO_CACHE(30))
            opCache += GC_BLK_OPRATIO_TO_CACHE(gHstWeight_HiOP[(gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_FREE] - GC_BLKCNT_ACTIVE)]);
        else
            opCache += GC_BLK_OPRATIO_TO_CACHE(gHstWeight_LoOP[(gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_FREE] - GC_BLKCNT_ACTIVE)]);
    }
    else
    {
        if (opCache > GC_BLK_OPRATIO_TO_CACHE(gHstWeight_HiOP[(GC_BLKCNT_ACTIVE - gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_FREE])]))
            opCache -= GC_BLK_OPRATIO_TO_CACHE(gHstWeight_HiOP[(GC_BLKCNT_ACTIVE - gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_FREE])]);
        else
            opCache = GC_BLK_OPRATIO_TO_CACHE(1);
    }

    // Only judge max cache cnt but maybe to consider min cache cnt need equal to  CACHE_FLUSH_ONE_CE_CNT(2% OP)
    if (opCache > GC_TOP_CACHE_CNT)         // 90%
        opCache = GC_TOP_CACHE_CNT;
    else if (opCache < GC_LOW_CACHE_CNT)
        opCache = GC_LOW_CACHE_CNT;         // 14% For EP Model

    return (opCache);
}

//-------------------------------------------------------------------
// Function     : void gc_setDtcmRmp(U16 block, U8 curP2lIdx)
// Description  :
// Input        : U16 block, U8 curP2lIdx
// return       : N/A
//-------------------------------------------------------------------
static __inline void gc_setDtcmRmp(U16 block, U8 curP2lIdx)
{
    tPAA paap;
    U16  idx;
    U8   lun, ce, ch, plane;

    paap.all32   = 0;
    paap.b.block = block;

    for (lun = 0; lun < GC_NF_LUN_PER_DEVICE; lun++)
    {
        for (ce = 0; ce < GC_NF_DEVICE_PER_CHANNEL; ce++)
        {
            for (plane = 0; plane < GC_NF_PLANE_PER_LUN; plane++)
            {
                for (ch = 0; ch < GC_NF_NUM_CHANNELS; ch++)
                {
                    paap.b.lun   = lun;
                    paap.b.ce    = ce;
                    paap.b.plane = plane;
                    paap.b.ch    = ch;
                    if (block != GC_Get_RmpBlk(paap))
                    {
                        idx = GET_RMP_IDX(paap.b.plane, paap.b.ce, paap.b.lun);
                        gGCppBuf[curP2lIdx].blk_Info.rmpBlk[idx] |= (1 << paap.b.ch);
                    }
                }
            }
        }
    }
}

//-------------------------------------------------------------------
// Function     : U32 gc_checkLAA(tPAA phyPaa, U8 curP2lIdx, U16 phyBlk)
// Description  :
// Input        : tPAA phyPaa, U8 curP2lIdx, U16 phyBlk
// return       : U32
//-------------------------------------------------------------------
__inline U32 gc_checkLAA(tPAA phyPaa, U8 curP2lIdx, U16 phyBlk)
{
    U16 cacheIdx;
    U32 laa = GC_Get_P2L(phyPaa, curP2lIdx);

    if (laa != DWORD_MASK)
    {
        phyPaa.b.block = phyBlk;  // Change from logical 2 physical blk number

        if (gFtlL2PTbl[laa].all32 == phyPaa.all32)
        {
            cacheIdx = Cache_Search(laa);

            if (cacheIdx)
            {
                if ((smCacheMgr.cacheEntry[cacheIdx].pool == SSD_POOL_FREE) && (smCacheMgr.cacheEntry[cacheIdx].status == 0))
                {
                    Cache_Delete(cacheIdx);
                    smCacheMgr.cacheEntry[cacheIdx].bitmap = CACHE_BMP_ALL_INVALID;
                    smCacheMgr.cacheEntry[cacheIdx].laa    = DWORD_MASK;
                    return (laa);
                }
                else if (smCacheMgr.cacheEntry[cacheIdx].pool == SSD_POOL_FILLUP)
                {
                    smCacheMgr.forceFillup = TRUE;
                }
                else if (smCacheMgr.cacheEntry[cacheIdx].pool == SSD_POOL_FREE)
                {
                    if (smCacheMgr.cacheEntry[cacheIdx].status & (CACHE_S_SEQ_READ | CACHE_S_XFER_READ))
                    {
                        smCacheMgr.cacheEntry[cacheIdx].pool = SSD_POOL_GC;
                    }
                    else if (smCacheMgr.cacheEntry[cacheIdx].status & CACHE_S_XFER_WRITE)
                    {
                        smCacheMgr.forceFillup = TRUE;
                    }
                }
            }
            else
            {
                return (laa);
            }
        }
    }
    return (DWORD_MASK);
}

// -------------------------------------------------------------------
// Function     : void gc_completeP2L(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static __inline void gc_completeP2L(U8 curP2lIdx)
{
    if (gGCppBuf[curP2lIdx].mode_Info.mode == GC_ST_SHUTTLE)  // Err or special case....
    {
        if (!gGCInfo.shuttleCnt && gGCppBuf[GC_PING_PONG_NUM - curP2lIdx].mode_Info.mode != GC_ST_SHUTTLE)
            GC_MODE_CLEAN(GC_SHUTTLE);

        if (gGCInfo.mode.b.bootGC)
        {
            gGCInfo.boot.blkcnt--;
            if (!gGCInfo.boot.blkcnt)
                GC_MODE_CLEAN(GC_BOOT);

            gFtlBlkListTbl[gGCppBuf[curP2lIdx].blk_Info.blk].vs.b.vac = 0;
            GC_FreeBlk(gGCppBuf[curP2lIdx].blk_Info.blk);
        }
        else
        {
            D_PRINTF_GC("SHUTTLE BK[%4x]DONE\n", gGCppBuf[curP2lIdx].blk_Info.blk);
        }
    }

    if ((gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_USER] <= GC_WL_REQ_BK_CNT))
    {
        GC_MODE_CLEAN(GC_STATIC_WL);
    }

    if ((gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_GCD] > (GC_PING_PONG_NUM << 1)))
    {
        if (smCacheMgr.ssdCnt[SSD_POOL_GC] && smCacheMgr.ssdCnt[SSD_POOL_GC] < GC_REQ_CACHE_ENTRY)
        {
            Cache_MergePool(SSD_POOL_GC, SSD_POOL_WRITE);
        }

        GC_ReleaseGcd();
    }

    // Debug Use: Check Vac
    if ((gGCppBuf[curP2lIdx].recyc_Info.recycPaaCnt > gGCppBuf[curP2lIdx].blk_Info.validCnt) && (gGCppBuf[curP2lIdx].mode_Info.type == GC_LP))
    {
        FTL_DumpInfo();
        D_PRINTF("Err Vac GC[%08x] FTL[%08x] BLK[%04x]\n", gGCppBuf[curP2lIdx].recyc_Info.recycPaaCnt, gGCppBuf[curP2lIdx].blk_Info.validCnt, gGCppBuf[curP2lIdx].blk_Info.blk);
        D_PRINTF("SN[%x] TBSN[%x]\n", gFtlSNTable[gGCppBuf[curP2lIdx].blk_Info.blk], gFtlMgr.TableSN);
        FTL_DumpVAC();
        ASSERT(FALSE);
    }
}

//-------------------------------------------------------------------
// Function     : void GC_PingPong_Recycle_Done(U8 curP2lIdx)
// Description  :
// Input        : Blk Select state
// return       : N/A
//-------------------------------------------------------------------
void GC_PingPong_Recycle_Done(U8 curP2lIdx)
{
    gGCppBuf[curP2lIdx].fsm_Info.curState = GC_PP_ST_RECYCLE_DONE;

    gc_completeP2L(curP2lIdx);

    if (gGCInfo.state.b.stopAtPor)
    {
        GC_STATE_SETTING(GC_STATE_HALT);
    }

    // Inital GCppBuf var...
    MEM_CLR(&gGCppBuf[curP2lIdx], sizeof(tGC_PING_PONG_BUF_INFO));
    gGCppBuf[curP2lIdx].blk_Info.blk = WORD_MASK;

    // ------- fsm --------------------------------------------------
    gGCppBuf[curP2lIdx].fsm_Info.nxtState = GC_PP_ST_IDLE;

    // ------- fsm --------------------------------------------------
}

//-------------------------------------------------------------------
// Function     : void GC_PingPong_Recycle(U8 curP2lIdx)
// Description  :
// Input        : Blk Select state
// return       : N/A
//-------------------------------------------------------------------
void GC_PingPong_Recycle(U8 curP2lIdx)
{
    U8 gcDone;
    gGCppBuf[curP2lIdx].fsm_Info.curState = GC_PP_ST_RECYCLE;

    if ((smCacheMgr.ssdCnt[SSD_POOL_FREE] >= GC_ONE_SHOT_PAA_CNT) && smFioInfo.jobCnt[FIO_JOB_POOL_FREE])  // checking FIO && SSD
    {
        if (gGCppBuf[curP2lIdx].qos_Info.gcCacheCnt < gGCppBuf[curP2lIdx].qos_Info.gcCacheTH || gGCInfo.state.b.stopAtPor) //// checking QoS checking GC mode: those modes requires complete block recycled/regardless of QoS
        {
            gcDone = (gGCppBuf[curP2lIdx].mode_Info.type == GC_AUX) ? gc_issueJob_Aux(curP2lIdx): gc_issueJob(curP2lIdx);
            if (gcDone)
            {
                // ------- fsm --------------------------------------------------
                gGCppBuf[curP2lIdx].fsm_Info.nxtState = GC_PP_ST_RECYCLE_DONE;

                // ------- fsm --------------------------------------------------
            }
        }
        else    // Checking GC state need to idle or not....
        {
            if ((gGCppBuf[curP2lIdx].mode_Info.mode < GC_ST_SHUTTLE) && !(gGCInfo.shuttleCnt))
            {
                GC_STATE_SETTING(GC_STATE_IDLE);
            }
        }
    }
}

//-------------------------------------------------------------------
// Function     : void GC_PingPong_P2L_Done(U8 curP2lIdx)
// Description  :
// Input        : Blk Select state
// return       : N/A
//-------------------------------------------------------------------
void GC_PingPong_P2L_Done(U8 curP2lIdx)
{
    gGCppBuf[curP2lIdx].fsm_Info.curState = GC_PP_ST_P2L_DONE;

    if (gGCppBuf[gGCInfo.curPpPtr].fsm_Info.nxtState != GC_PP_ST_RECYCLE)            // Change next pp idx for recycle
    {
        gGCInfo.curPpPtr = curP2lIdx;

        if (gGCppBuf[curP2lIdx].lp_Info.errorCnt || (GC_MODE_CHECK_BOOT_BLK(curP2lIdx)))  // Boot GC mode GC THCNT must be GC_FTL_SBK_VALID_CNT
        {
            // Abnormal AUX GC : Changing mode Exception
            gGCppBuf[curP2lIdx].mode_Info.type = GC_AUX;

            // ------- Qos Info -----------------------------------------
            if (gGCppBuf[curP2lIdx].mode_Info.mode != GC_ST_LOCK )
                gCbfGC_Set_TH[GC_ST_AUX](curP2lIdx);
        }

        // ------- fsm --------------------------------------------------
        gGCppBuf[curP2lIdx].fsm_Info.nxtState = GC_PP_ST_RECYCLE;

        // ------- fsm --------------------------------------------------
        gGCppBuf[curP2lIdx].blk_Info.opRatio = GC_RECYC_BLK_OPRATIO(gGCppBuf[curP2lIdx].blk_Info.validCnt);

        BEGIN_CORE_DBG_DUMP(DBG_DUMP_GCBLK_INFO)
        D_PRINTF_GC("M[%2x]S[%2x]AX[%d]I[%d]B[%4x]VC[%6x]EC[%4x]OP[%d]\n",
                    GC_MODE_CHECK,
                    GC_STATE_CHECK,
                    gGCppBuf[curP2lIdx].mode_Info.type,
                    curP2lIdx,
                    gGCppBuf[curP2lIdx].blk_Info.blk,
                    gFtlBlkListTbl[gGCppBuf[curP2lIdx].blk_Info.blk].vs.b.vac,
                    gFtlBlkListTbl[gGCppBuf[curP2lIdx].blk_Info.blk].eraseCnt,
                    gGCppBuf[curP2lIdx].blk_Info.opRatio);
        END_CORE_DBG_DUMP
    }
}

//-------------------------------------------------------------------
// Function     : void GC_PingPong_P2L_Wait(U8 curP2lIdx)
// Description  :
// Input        : Blk Select state
// return       : N/A
//-------------------------------------------------------------------
void GC_PingPong_P2L_Wait(U8 curP2lIdx)
{
    gGCppBuf[curP2lIdx].fsm_Info.curState = GC_PP_ST_P2L_WAIT;

    if (gGCppBuf[curP2lIdx].lp_Info.doneCnt == GC_NF_LUN_PER_CH)
    {
        // ------- fsm --------------------------------------------------
        gGCppBuf[curP2lIdx].fsm_Info.nxtState = GC_PP_ST_P2L_DONE;

        // ------- fsm --------------------------------------------------
    }
}

//-------------------------------------------------------------------
// Function     : void GC_PingPong_P2L_Read(U8 curP2lIdx)
// Description  :
// Input        : Blk Select state
// return       : N/A
//-------------------------------------------------------------------
void GC_PingPong_P2L_Read(U8 curP2lIdx)
{
    gGCppBuf[curP2lIdx].fsm_Info.curState = GC_PP_ST_P2L_READ;

    if (GC_MODE_CHECK_BOOT_BLK(curP2lIdx))
    {
        // ------- fsm --------------------------------------------------
        gGCppBuf[curP2lIdx].fsm_Info.nxtState = GC_PP_ST_P2L_DONE;

        // ------- fsm --------------------------------------------------
    }
    else if (gGCppBuf[curP2lIdx].lp_Info.issueCnt != GC_NF_LUN_PER_CH)
    {
        if (smCacheMgr.ssdCnt[SSD_POOL_OTHER] >= GC_REQ_SSD_COUNT_LP && smFioInfo.jobCnt[FIO_JOB_POOL_FREE])
        {
            if(gGCppBuf[curP2lIdx].lp_Info.issueCnt == 0 && gFtlBlkListTbl[gGCppBuf[curP2lIdx].blk_Info.blk].vs.b.bad) // For ErrH Case
                Core_JobWait();

            gc_issueJobLP(curP2lIdx);
            gGCppBuf[curP2lIdx].lp_Info.issueCnt++;  // Next loop issueCnt
        }
    }
    else
    {
        // ------- fsm --------------------------------------------------
        gGCppBuf[curP2lIdx].fsm_Info.nxtState = GC_PP_ST_P2L_WAIT;

        // ------- fsm --------------------------------------------------
    }
}

//-------------------------------------------------------------------
// Function     : void GC_PingPong_Blk_Select(U8 curP2lIdx)
// Description  : Fill blk Info, qos Info,
// Input        : Blk Select state
// return       : N/A
//-------------------------------------------------------------------
void GC_PingPong_Blk_Select(U8 curP2lIdx)
{
    gGCppBuf[curP2lIdx].fsm_Info.curState = GC_PP_ST_BLK_SELECT;

    // ------- blk Info ---------------------------------------------
    switch(gGCppBuf[curP2lIdx].mode_Info.mode)
    {
        case GC_ST_SHUTTLE:
        {
            gGCppBuf[curP2lIdx].blk_Info.blk = gGCppBuf[curP2lIdx].recyc_Info.curPaa.b.block = FTL_BlockPopHead(FTL_BLOCK_POOL_GC);
            break;
        }
        case GC_ST_LOCK:
        case GC_ST_EMGR:
        case GC_ST_IDLE:
        case GC_ST_FORCE:
        {
            gGCppBuf[curP2lIdx].blk_Info.blk = gGCppBuf[curP2lIdx].recyc_Info.curPaa.b.block = gc_getVicBlk(FTL_BLOCK_POOL_USER, PICK_LOW_VC);
            FTL_BlockPopList(FTL_BLOCK_POOL_USER, gGCppBuf[curP2lIdx].blk_Info.blk);
            break;
        }
        case GC_ST_STATIC_WL:
        {
            gGCppBuf[curP2lIdx].blk_Info.blk = gGCppBuf[curP2lIdx].recyc_Info.curPaa.b.block = gc_getVicBlk(FTL_BLOCK_POOL_USER, PICK_LOW_EC);
            FTL_BlockPopList(FTL_BLOCK_POOL_USER, gGCppBuf[curP2lIdx].blk_Info.blk);
            break;
        }
        default:
        {
            D_PRINTF_GC("[%x]\n",gGCppBuf[curP2lIdx].mode_Info.mode);
            ASSERT(FALSE);
            break;
        }
    }
    FTL_BlockPushList(FTL_BLOCK_POOL_GCD,  gGCppBuf[curP2lIdx].blk_Info.blk, FTL_SORT_NONE);

    gGCppBuf[curP2lIdx].blk_Info.validCnt = gFtlBlkListTbl[gGCppBuf[curP2lIdx].blk_Info.blk].vs.b.vac;

    if (gFtlRemap.rmpBlkBMP[gGCppBuf[curP2lIdx].blk_Info.blk])
    {
        gc_setDtcmRmp(gGCppBuf[curP2lIdx].blk_Info.blk, curP2lIdx);
    }

    // ------- Qos Info ---------------------------------------------
    gCbfGC_Set_TH[gGCppBuf[curP2lIdx].mode_Info.mode](curP2lIdx);

    // ------- fsm --------------------------------------------------
    gGCppBuf[curP2lIdx].fsm_Info.nxtState = GC_PP_ST_P2L_READ;

    // ------- fsm --------------------------------------------------
}

//-------------------------------------------------------------------
// Function     : void GC_PingPong_Idle(U8 curP2lIdx)
// Description  :
// Input        : U8 curP2lIdx
// return       : N/A
//-------------------------------------------------------------------
void GC_PingPong_Idle(U8 curP2lIdx)
{
    gGCppBuf[curP2lIdx].fsm_Info.curState = GC_PP_ST_IDLE;
    gGCppBuf[curP2lIdx].mode_Info.mode    = gc_getMode();

    // ------- fsm --------------------------------------------------
    if (gGCppBuf[curP2lIdx].mode_Info.mode != GC_ST_NON)              // GCmode hadn't been selected  EXP: moving a block
    {                                                                 // to another under clean drive thus only 1 PP buffer
        gGCppBuf[curP2lIdx].fsm_Info.nxtState = GC_PP_ST_BLK_SELECT;  // would need to be activated, the other keeps at idle.
    }

    // ------- fsm --------------------------------------------------
}

//-------------------------------------------------------------------
// Function     : void GC_AllocTH_NON(U8 curP2lIdx)
// Description  :
// Input        : U8 curP2lIdx
// return       : N/A
//-------------------------------------------------------------------
void GC_AllocTH_NON(U8 curP2lIdx)
{
    ASSERT(FALSE);
}

//-------------------------------------------------------------------
// Function     : void GC_AllocTH_LOCK(U8 curP2lIdx)
// Description  :
// Input        : U8 curP2lIdx
// return       : N/A
//-------------------------------------------------------------------
void GC_AllocTH_LOCK(U8 curP2lIdx)
{
    gGCppBuf[curP2lIdx].qos_Info.hostCacheTH  = GC_REQ_CACHE_ENTRY;
    gGCppBuf[curP2lIdx].qos_Info.gcCacheTH    = GC_FTL_SBK_VALID_CNT;
}

//-------------------------------------------------------------------
// Function     : void GC_AllocTH_FORCE(U8 curP2lIdx)
// Description  :
// Input        : U8 curP2lIdx
// return       : N/A
//-------------------------------------------------------------------
void GC_AllocTH_FORCE(U8 curP2lIdx)
{
    gGCppBuf[curP2lIdx].qos_Info.hostCacheTH  = gc_getBlkCache(gGCppBuf[curP2lIdx].blk_Info.validCnt);
    gGCppBuf[curP2lIdx].qos_Info.gcCacheTH    = (CACHE_RW_COUNT - gGCppBuf[curP2lIdx].qos_Info.hostCacheTH);
}

//-------------------------------------------------------------------
// Function     : void GC_AllocTH_SHUTTLE(U8 curP2lIdx)
// Description  :
// Input        : U8 curP2lIdx
// return       : N/A
//-------------------------------------------------------------------
void GC_AllocTH_SHUTTLE(U8 curP2lIdx)
{
    gGCppBuf[curP2lIdx].qos_Info.hostCacheTH  = (GC_FTL_SBK_VALID_CNT / gGCInfo.op.emgrRatio);
    gGCppBuf[curP2lIdx].qos_Info.gcCacheTH    = (GC_FTL_SBK_VALID_CNT);
}

//-------------------------------------------------------------------
// Function     : void GC_main(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void GC_main(void)
{
#if (FTL_PARTIAL_LOAD_L2P != FALSE)
    if (gFtlPorPbtInfo.finishBuild != FALSE)
#endif
    {
        gCbfGC_PP_FSM[gGCppBuf[gGCInfo.nxtPpPtr].fsm_Info.nxtState](gGCInfo.nxtPpPtr);

        if (gGCppBuf[gGCInfo.nxtPpPtr].fsm_Info.nxtState != GC_PP_ST_RECYCLE ||
            gGCppBuf[(GC_PING_PONG_RECYC_CNT - gGCInfo.nxtPpPtr)].fsm_Info.nxtState != GC_PP_ST_P2L_DONE)
        {
            gGCInfo.nxtPpPtr = (GC_PING_PONG_RECYC_CNT - gGCInfo.nxtPpPtr);
        }
    }
}

//-------------------------------------------------------------------
// Function     : void GC_HostWrite(MsgHostIO_t* hMsg)
// Description  :
// Input        : MsgHostIO_t* hMsg
// return       : U32
//-------------------------------------------------------------------
U32 GC_HostWrite(MsgHostIO_t* hMsg)
{
    if (GC_QoS_Host(hMsg->bufDesc.ssdCnt))
    {
        return TRUE;
    }
    
    GC_COUNT_HOST_WRITE (gGCInfo.curPpPtr, hMsg->bufDesc.ssdCnt);

    return FALSE;
}

//-------------------------------------------------------------------
// Function     : void GC_HostTrim(MsgHostIO_t* hMsg)
// Description  :
// Input        : MsgHostIO_t* hMsg
// return       : U32
//-------------------------------------------------------------------
U32 GC_HostTrim(MsgTrim_t* hMsg)
{
    GC_STATE_SETTING(GC_STATE_HALT);
    return FALSE;
}

//-------------------------------------------------------------------
// Function     : U32 GC_HostFlush(MsgHostIO_t* hMsg)
// Description  :
// Input        : MsgHostIO_t* hMsg
// return       : U32
//-------------------------------------------------------------------
U32 GC_HostFlush(Msg_t* hMsg)
{
    U16 dummyCnt = 0;

    if (smCacheMgr.forceFlush  == FALSE) dummyCnt += smCacheMgr.ssdCnt[SSD_POOL_WRITE];
    if (smCacheMgr.forceFillup == FALSE) dummyCnt += smCacheMgr.ssdCnt[SSD_POOL_FILLUP];

    dummyCnt = (dummyCnt % GC_REQ_CACHE_ENTRY);

    if (dummyCnt) dummyCnt = GC_REQ_CACHE_ENTRY - dummyCnt;

    if (GC_QoS_Host(dummyCnt))
    {
        return TRUE;
    }

    return FALSE;
}

//-------------------------------------------------------------------
// Function     : U32 GC_HostFormat(MsgHostIO_t* hMsg)
// Description  :
// Input        : MsgHostIO_t* hMsg
// return       : U32
//-------------------------------------------------------------------
U32 GC_HostFormat(MsgHostIO_t* hMsg)
{
    GC_STATE_SETTING(GC_STATE_HALT);
    return FALSE;
}

//-------------------------------------------------------------------
// Function     : U32 GC_HostShutDown(MsgHostIO_t* hMsg)
// Description  :
// Input        : MsgHostIO_t* hMsg
// return       : U32
//-------------------------------------------------------------------
U32 GC_HostShutDown(MsgHostIO_t* hMsg)
{
    U32 status = FALSE;
    U16 ppIdx;

    for (ppIdx = 0; ppIdx < GC_PING_PONG_NUM; ppIdx++)
    {
        // -------------------------------------------------------------------------//
        // Checking PING_PONG nxtstate is GC_PP_ST_RECYCLE and GC_PP_ST_P2L_DONE need recycle done last block..
        // -------------------------------------------------------------------------//
        if ((gGCppBuf[ppIdx].fsm_Info.nxtState == GC_PP_ST_RECYCLE) &&
            (gGCppBuf[(GC_PING_PONG_RECYC_CNT - ppIdx)].fsm_Info.nxtState == GC_PP_ST_P2L_DONE))
        {
            GC_STATE_SETTING(GC_STATE_STOPATPOR);
            //D_PRINTF("PORS pp[%d] blk[%4x] st[%x]\n", ppIdx, gGCppBuf[ppIdx].blk_Info.blk, gGCppBuf[ppIdx].fsm_Info.curState);
            status = TRUE;
            break;
        }
    }
    return status;
}

//-------------------------------------------------------------------
// Function     : void GC_ModeAssert(U16 pool, U16 block)
// Description  :
// Input        : U16 pool, U16 block
// return       : N/A
//-------------------------------------------------------------------
void GC_ModeAssert(U16 pool, U16 block)
{
    // (GC_LOCK|GC_FORCE|GC_EMGR|GC_SHUTTLE|GC_IDLE|GC_STATIC_WL)
    // ----- DiskLock
    if (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_FREE] < GC_BLKCNT_DISKFULL)
    {
        D_PRINTF_GC("GC_WARN: DISK FULL\n");
        GC_MODE_SETTING(GC_LOCK | GC_EMGR | GC_FORCE);

    }
    else if (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_FREE] <= GC_BLKCNT_EMER)
    {
        GC_MODE_SETTING(GC_EMGR | GC_FORCE);
    }
    else if (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_FREE] <= GC_BLKCNT_ACTIVE)
    {
        GC_MODE_SETTING(GC_FORCE);
    }

    if (pool == FTL_BLOCK_POOL_FREE)
    {
        // ----- WL check must at pop free block
        if ((gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_USER] > GC_WL_REQ_BK_CNT) &&
            (gFtlBlkListTbl[block].eraseCnt > (GC_EC_AVG_CNT + GC_WL_ENABLE_TH)))
        {
            GC_MODE_SETTING(GC_STATIC_WL);
        }
        else
        {
            GC_MODE_CLEAN(GC_STATIC_WL);
        }

        // If exit GC mode need transfer GC cache to write
        if (!(GC_MODE_CHECK) && smCacheMgr.ssdCnt[SSD_POOL_GC])
        {
            Cache_MergePool(SSD_POOL_GC, SSD_POOL_WRITE);
        }

        #if (DBG_BLKLOG == TRUE)
        BEGIN_CORE_DBG_DUMP(DBG_DUMP_GCBLK_INFO)
        D_PRINTF("[BLK ] FB[%4x]UB[%4x]B[%4x]LK[%1X]EC[%2d]T[%d]\n",
                gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_FREE],
                gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_USER],
                block,
                gGCInfo.state.b.readOnly,
                gFtlBlkListTbl[block].eraseCnt,
                #if (THERAML_TEMPERATURE_FROM_SOC == TRUE)  // EP
                    HalTempSensor_GetTemperature()
                #else
                    AplThermal_GetI2cTemperature()
                #endif
                );
        END_CORE_DBG_DUMP
    }
    #endif
}

//-------------------------------------------------------------------
// Function     : void GC_ModeDeassert(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void GC_ModeDeassert(void)
{
    // (GC_LOCK|GC_FORCE|GC_EMGR|GC_SHUTTLE|GC_IDLE|GC_STATIC_WL)
    if (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_FREE] >= GC_BLKCNT_DEACTIVE)
    {
        GC_MODE_CLEAN(GC_LOCK | GC_EMGR | GC_FORCE);
    }
    else if (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_FREE] > GC_BLKCNT_EMER)
    {
        GC_MODE_CLEAN(GC_LOCK | GC_EMGR);
    }
    else if (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_FREE] > GC_BLKCNT_DISKFULL)
    {
        GC_MODE_CLEAN(GC_LOCK);
    }
 }

//-------------------------------------------------------------------
// Function     : void GC_PushBlk(U16 blk, U16 pool, U16 header)
// Description  :
// Input        : U16 blk, U16 pool, U16 header
// return       : N/A
//-------------------------------------------------------------------
void GC_PushBlk(U16 blk, U16 pool, U16 header)
{
    if (header == FALSE)
    {
        FTL_BlockPopList(pool, blk);
    }

    FTL_BlockPushList(FTL_BLOCK_POOL_GC, blk, FTL_SORT_NONE);
    GC_MODE_SETTING(GC_SHUTTLE);
    gGCInfo.shuttleCnt++;
}

//-------------------------------------------------------------------
// Function     : void GC_FreeBlk(U16 blk)
// Description  :
// Input        : U16 blk
// return       : N/A
//-------------------------------------------------------------------
void GC_FreeBlk(U16 blk)
{
    U16 status = TRUE;

    if (gFtlBlkListTbl[blk].vs.b.bad)
    {
    #if (GC_REMAP_MODE == TRUE)
        status = GC_Replance_BadBlk(blk);
    #else
        status = FALSE;
    #endif
    }

    FTL_BlockPopList(gFtlBlkListTbl[blk].vs.b.status, blk);
    gFtlBlkListTbl[blk].vs.b.rdd = FALSE;

    if (status)
    {
        gFtlBlkListTbl[blk].vs.b.bad = FALSE;
        FTL_BlockPushList(FTL_BLOCK_POOL_FREE, blk, FTL_SORT_BY_EC);
    }
    else
    {
        FTL_BlockPushList(FTL_BLOCK_POOL_BAD, blk, FTL_SORT_BY_EC);
        GC_OverProvion();
    }
}

//-------------------------------------------------------------------
// Function     : void GC_ReleaseGcd(void)
// Description  : Release Blk in GCD Pool  (NOTICE: CHECK VAC)
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void GC_ReleaseGcd(void)
{
    U16 blk, nextBlk, idx, blkcnt;

    blkcnt = gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_GCD];
    blk    = gFtlBlkListInfo.head[FTL_BLOCK_POOL_GCD];

    for (idx = 0; idx < blkcnt; idx++)
    {
        nextBlk = gFtlBlkListTbl[blk].block;

        if ((gFtlSNTable[blk] < gFtlMgr.TableSN)
         && (!gFtlBlkListTbl[blk].vs.b.vac)
         && (blk != gGCppBuf[0].blk_Info.blk)
         && (blk != gGCppBuf[1].blk_Info.blk))
        {
            GC_FreeBlk(blk);
        }
        blk = nextBlk;
    }
}

#if (GC_REMAP_MODE == TRUE)
#include "NormSection.h"
//-------------------------------------------------------------------
// Function     : void GC_Sort_Remap_Info(U16 hitcnt)
// Description  :
// Input        : U16 hitcnt
// return       : N/A
//-------------------------------------------------------------------
void GC_Sort_Remap_Info(U16 hitcnt)
{
    U16 idx;
    ASSERT(gGCremapInfo.curRemapCnt > 0)
    gGCremapInfo.curRemapCnt--;

    for (idx = hitcnt; idx < gGCremapInfo.curRemapCnt; idx++)
    {
        MemCopyNU32((U32*)&gGCremapInfo.array[idx], (U32*)&gGCremapInfo.array[idx + 1], sizeof(tGC_NODE));
        D_PRINTF_GC("ReSort REMP idx[%x]<-[%x]addr[%8x]<-[%8x] blk[%4x]\n",
                     idx, idx + 1,
                     &gGCremapInfo.array[idx], &gGCremapInfo.array[idx + 1],
                     gGCremapInfo.array[idx + 1].block);
    }
    MEM_CLR(&gGCremapInfo.array[gGCremapInfo.curRemapCnt], sizeof(tGC_NODE));
    gGCremapInfo.array[gGCremapInfo.curRemapCnt].block = WORD_MASK;
}

//-------------------------------------------------------------------
// Function     : U32 GC_Get_Remap_Info(U16 blk)
// Description  :
// Input        : U16 blk
// return       : U32
//-------------------------------------------------------------------
U32 GC_Get_Remap_Info(U16 blk)
{
    U16 idx;

    for (idx = 0; idx < gGCremapInfo.curRemapCnt; idx++)
    {
       if ((gGCremapInfo.array[idx].block == blk) && (gGCremapInfo.array[idx].block != WORD_MASK))
       {
           return (idx);
       }
    }
    return (DWORD_MASK);
}

//-------------------------------------------------------------------
// Function     : void GC_Set_Remap_Blk(U16 blk)
// Description  :
// Input        : U16 blk
// return       : N/A
//-------------------------------------------------------------------
void GC_Set_Remap_Blk(U16 blk)
{
    U16 idx;
    U16 Flag = TRUE;

    if (gGCremapInfo.curRemapCnt < GC_REMAP_SIZE && (gFtlBlkListTbl[blk].vs.b.bad == TRUE) && (gFtlBlkListTbl[blk].vs.b.rdd == FALSE))
    {
        for (idx = 0; idx < gGCremapInfo.curRemapCnt; idx++)
        {
            if (gGCremapInfo.array[idx].block == blk)
            {
                Flag = FALSE;
                break;
            }
        }

        if (Flag)
        {
            gGCremapInfo.array[gGCremapInfo.curRemapCnt].block = blk;
            gGCremapInfo.curRemapCnt++;
            D_PRINTF_GC("PUH ONE BLK[%4x] RMPCNT[%d]\n", blk, gGCremapInfo.curRemapCnt);
        }
    }
}

#include "PerfSection.h"
//-------------------------------------------------------------------
// Function     : void GC_Set_Remap_Bmp(tPAA phyPaa)
// Description  :
// Input        : tPAA phyPaa
// return       : N/A
//-------------------------------------------------------------------
void GC_Set_Remap_Bmp(tPAA phyPaa)
{
    U16 idx;
    U16 remap_idx = GET_RMP_IDX(phyPaa.b.plane, phyPaa.b.ce, phyPaa.b.lun);

    for (idx = 0; idx < GC_REMAP_SIZE; idx++)
    {
        if ((gGCremapInfo.array[idx].block == GC_Get_RmpBlk(phyPaa)) && (gGCremapInfo.array[idx].block != WORD_MASK))
        {
            if (!(gGCremapInfo.array[idx].bmp[remap_idx] & (1 << phyPaa.b.ch)))
            {
                gGCremapInfo.array[idx].bmp[remap_idx] |= (1 << phyPaa.b.ch);
            }
        }
    }
}

//-------------------------------------------------------------------
// Function     : BOOL GC_Remap_PhyPaa(U32 idx, U16 blk)
// Description  :
// Input        : U32 idx, U16 blk
// return       : BOOL
//-------------------------------------------------------------------
BOOL GC_Remap_PhyPaa(U32 idx, U16 blk)
{
   tPAA paap;
   U16  remap_idx;
   U8   lun, ce, ch, plane, die;

   paap.all32 = 0;

   for (lun = 0; lun < GC_NF_LUN_PER_DEVICE; lun++)
   {
       for (ce = 0; ce < GC_NF_DEVICE_PER_CHANNEL; ce++)
       {
           for (plane = 0; plane < GC_NF_PLANE_PER_LUN; plane++)
           {
               for (ch = 0; ch < GC_NF_NUM_CHANNELS; ch++)
               {
                   paap.b.lun   = lun;
                   paap.b.ce    = ce;
                   paap.b.plane = plane;
                   paap.b.ch    = ch;
                   remap_idx = GET_RMP_IDX(paap.b.plane, paap.b.ce, paap.b.lun);
                   die       = FTL_GET_DIE_NUM(paap.b.lun, paap.b.ce, paap.b.ch);

                   if (gGCremapInfo.array[idx].bmp[remap_idx] & (1 << paap.b.ch))
                   {
                       //D_PRINTF_GC("match ch[%d] ce[%d] pl[%d] ln[%d] blk[%4x]\n", paap.b.ch,
                                    //paap.b.ce, paap.b.plane, paap.b.lun, blk);
                       gGCremapInfo.array[idx].bmp[remap_idx] &= (~(1 << paap.b.ch));

                       if (FTL_RemapBadBlock(die, paap.b.plane, blk) == FALSE)
                       {
                           return (FALSE);
                       }
                    }
                }
            }
        }
    }
    return (TRUE);
}

//-------------------------------------------------------------------
// Function     : U16 GC_Replance_BadBlk(U16 blk)
// Description  :
// Input        : U16 blk
// return       : N/A
//-------------------------------------------------------------------
U16 GC_Replance_BadBlk(U16 blk)
{
   U32 idx    = GC_Get_Remap_Info(blk);
   U16 status = TRUE;

   if (idx != DWORD_MASK)
   {
       if (GC_Remap_PhyPaa(idx, blk) == TRUE)
       {
           FTL_DumpRemap(TRUE);
           SyIn_Synchronize(SI_AREA_FTL, SYSINFO_WRITE, SI_SYNC_BY_FTL);
           GC_Sort_Remap_Info(idx);
       }
       else
       {
           status = FALSE;
       }
   }

   return (status);
}
#endif

//-------------------------------------------------------------------
// Function     : void GC_EarlyMoveBlk(tPAA paa)
// Description  :Check and move block to prevent read retension issue
// Input        :@param[in]  paa : physical address
// return       : N/A
//-------------------------------------------------------------------
#if (CO_SUPPORT_EARLY_MOVE_BLK == TRUE)
void GC_EarlyMoveBlk(tPAA paa)
{
    U32 ReCnt = EARLY_MOVE_MIN_CNT;
    U16 headflag = FALSE;
    U16 logBlk, currdie, idx, tempBlk;
    U8  lun, ce, plane, ch;

    if (GC_EC_AVG_CNT < 2000)
    {
        ReCnt = (EARLY_MOVE_BASE / 10 *((MAXECCNT - ((U32)GC_EC_AVG_CNT)) * 10 / MAXECCNT));
    }
    currdie = FTL_GET_DIE_NUM(paa.b.lun, paa.b.ce, paa.b.ch);
    idx     = FTL_GET_SBLK_IDX(paa.b.plane, paa.b.block, currdie);

    if ((++gFtlHeadReadCnt[idx]) >= ReCnt)
    {
        gFtlHeadReadCnt[idx] = 0;

        if (gFtlRemap.rmpBlkBMP[paa.b.block])
        {
            logBlk = FTL_GetRemapBlock(paa.b.ch, paa.b.ce, paa.b.lun, paa.b.plane, paa.b.block);
        }
        else
        {
            logBlk = paa.b.block;
        }

        if (gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].block == logBlk)
        {
            D_PRINTF_GC("Push B[%4x]TH[%8d]BCNT[%3U]M[%x]\n", logBlk, ReCnt, gNfInfo.blocksPerLun, GC_MODE_CHECK);

            if (gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].block == logBlk)
            {
                gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].block = WORD_MASK;
                gFtlBlkListTbl[logBlk].vs.b.vac = 0;
                headflag = TRUE;
            }

            GC_PushBlk(logBlk, gFtlBlkListTbl[logBlk].vs.b.status, headflag);

            gFtlBlkListTbl[logBlk].vs.b.rdd = TRUE;
            GC_STATE_RESET_ALL();

            // Clean All read cnt.
            for (lun = 0; lun < GC_NF_LUN_PER_DEVICE; lun++)
            {
                for (ce = 0; ce < GC_NF_DEVICE_PER_CHANNEL; ce++)
                {
                    for (plane = 0; plane < GC_NF_PLANE_PER_LUN; plane++)
                    {
                        for (ch = 0; ch < GC_NF_NUM_CHANNELS; ch++)
                        {
                            currdie = FTL_GET_DIE_NUM(lun, ce, ch);
                            tempBlk = FTL_GetRemapBlock(ch, ce, lun, plane, logBlk);
                            idx     = FTL_GET_SBLK_IDX(plane, tempBlk, currdie);

                            //D_PRINTF_GC("Clean ch[%d]ce[%d]lun[%d]pl[%d]idx[%d]BLK[%d]\n", ch, ce, lun, plane, idx, tempBlk);
                            gFtlHeadReadCnt[idx] = 0;
                        }
                    }
                }
            }
        }
    }
}
#endif

#include "NormSection.h"
//-------------------------------------------------------------------
// Function     : void GC_Boot(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void GC_Boot(void)
{
    U32 timer = HalTimer_ReadTick();
    U16 bootblk;

    if (gGCInfo.boot.blkcnt > 0)
    {
        bootblk = gFtlBlkListInfo.head[FTL_BLOCK_POOL_GC];
        GC_MODE_SETTING(GC_BOOT);
    }
    else
    {
        bootblk = WORD_MASK;
    }

    D_PRINTF_GC("BootStart FG[%1d] BKC[%d] M[%2x]\n",
                 gGCInfo.boot.flag,
                 gGCInfo.boot.blkcnt,
                 GC_MODE_CHECK);

    while (TRUE)
    {
        if ((smCacheMgr.ssdCnt[SSD_POOL_WRITE] >= (GC_REQ_CACHE_ENTRY)) ||
            (smCacheMgr.ssdCnt[SSD_POOL_GC]    >= (GC_REQ_CACHE_ENTRY)) ||
            (smCacheMgr.forceFlush) || (gFtlMgr.forceErase))
        {
            Core_ProcForceFlush();
        }

        if (GC_MODE_CHECK_BOOT_ACTIVE)
        {
            GC_main();
        }
        else
        {
            if (smFioInfo.jobCnt[FIO_JOB_POOL_FREE] == FIO_MAX_JOB) // All job complete
            {
                if ((smCacheMgr.ssdCnt[SSD_POOL_FREE] != CACHE_RW_COUNT))
                {
                    smCacheMgr.forceFlush = TRUE;
                }
                else                                                // All cache back
                {
                    smCacheMgr.forceFlush = FALSE;

                    if (!FTL_CURRENT_HEADER_NEW(FTL_BLOCK_HEADER_HOST, 0))
            	    {
                        FTL_FlushDummyCloseBlock(FTL_BLOCK_HEADER_HOST, 0, gNfInfo.pagesPerBlock);
            	    }
                    break;
                }
            }
        }

        while (IPC_MsgQFastPeek(cM2cComQueue))
        {
            Core_JobWait();
        }
    }
    if (bootblk != WORD_MASK)
    {
        FTL_EraseBankBlocks(bootblk); // Open blk need to erase twice
    }
    gGCInfo.boot.flag   = FALSE;
    gFtlPbtInfo.pageCnt = 0;

    #if (FTL_SPOR_FLUSH_DUMMY == TRUE)
	gFtlMgr.flushDummy = TRUE;
    #endif
    gGCInfo.boot.scantime_sec += (HalTimer_GetElapsedTimeMsec(timer) / 1000);
    D_PRINTF_GC("BootExit T[%d]ms Total T[%2d]s\n", HalTimer_GetElapsedTimeMsec(timer), gGCInfo.boot.scantime_sec);
}

//-------------------------------------------------------------------
// Function     : void GC_OverProvion(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void GC_OverProvion(void)
{
    U8 pool, head, group;

    // Always Initial Variable
    gGCInfo.op.totBlkCnt   = 0;
    gGCInfo.op.totHBlkCnt  = 0;
    gGCInfo.op.totHBlkCnt  = ((GC_NF_USER_CAPACITY * 1000 * 1000) / (GC_NF_DATA_BYTE_PER_PAGE * GC_NF_PAGE_PER_BLK * GC_NF_PLANE_PER_LUN * GC_NF_TOT_LUN / 1000));

    for (pool = FTL_BLOCK_POOL_USER; pool < FTL_BLOCK_POOL_BAD; pool++)
    {
        gGCInfo.op.totBlkCnt += gFtlBlkListInfo.blkCnt[pool];
    }

    // Include header blk
    for (head = FTL_BLOCK_HEADER_GC; head <= FTL_BLOCK_HEADER_TABLE; head++)
    {
        for (group = 0; group < FTL_GROUP_NUM; group++)
        {
            if (gFtlCurrHeader[head][group].block != WORD_MASK)
                gGCInfo.op.totBlkCnt++;
        }
    }

    if ((gGCInfo.op.totBlkCnt <= (gGCInfo.op.totHBlkCnt + GC_BLKCNT_DEACTIVE)) ||
        (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_FREE] <= GC_BLKCNT_READONLY ))
    {
        GC_MODE_CLEAN(GC_LOCK|GC_FORCE|GC_EMGR|GC_SHUTTLE|GC_BOOT|GC_IDLE|GC_STATIC_WL);
        GC_STATE_SETTING(GC_STATE_READONLY);
        D_PRINTF_GC("WARNY: DISK LOCK RONLY\n");
    }
    else
    {
        gGCInfo.op.opBlkCnt  = (gGCInfo.op.totBlkCnt - gGCInfo.op.totHBlkCnt);
        gGCInfo.op.opRatio   = (((gGCInfo.op.opBlkCnt * 100)) / gGCInfo.op.totHBlkCnt);
        gGCInfo.op.emgrRatio = ((((100 / gGCInfo.op.opRatio) << 1) + GC_EMGR_RATIO_WEIGHT) > GC_MAX_EMGR_RATIO) ? GC_MAX_EMGR_RATIO : ((100 / gGCInfo.op.opRatio) + GC_EMGR_RATIO_WEIGHT);

        smAvailableSpare = 100 - (gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_BAD] * 100 / (gGCInfo.op.totBlkCnt + gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_BAD] - gGCInfo.op.totHBlkCnt));

        gFtlTrimRatio = gGCInfo.op.opBlkCnt;//(GC_BLKCNT_DEACTIVE * FTL_QBT_BLOCK_RELEASE) << 1;

        D_PRINTF_GC("[GC Info]\n");
        D_PRINTF_GC("TotSBk[%2d]HostBlk[%2d]OpCnt[%2d]OpRatio[%2d]EmRatio[%2d]TrimRatio[%4d]SkpTrmBC[%d]\n",
        gGCInfo.op.totBlkCnt, gGCInfo.op.totHBlkCnt, gGCInfo.op.opBlkCnt,
        gGCInfo.op.opRatio, gGCInfo.op.emgrRatio, gFtlTrimRatio, FTL_SKIP_TRIM_BLOCKS_COUNT);
    }
}

//-------------------------------------------------------------------
// Function     : void GC_GlobalVarInit(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void GC_GlobalVarInit(void)
{
    U8  idx;

    MEM_SET(&gGCInfo,  0x00, sizeof(tGC_GEN_INFO));
    MEM_SET(&gGCppBuf, 0x00, sizeof(tGC_PING_PONG_BUF_INFO) * GC_PING_PONG_NUM);
    MEM_SET(&gGCAux,   0x00, sizeof(tGC_AUX));
    #if (GC_REMAP_MODE == TRUE)
      MEM_CLR(gGCremapInfo.array, sizeof(tGC_NODE) * GC_REMAP_SIZE);
      for (idx = 0; idx < GC_REMAP_SIZE; idx++)
      {
          gGCremapInfo.array[idx].block = WORD_MASK;
          gGCremapInfo.curRemapCnt = 0;
      }
    #endif

    gGCInfo.shuttleCnt = gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_GC];
    GC_OverProvion();
}

//-------------------------------------------------------------------
// Function     : void GC_OneTimeInit(InitBootMode_t initMode)
// Description  :
// Input        : InitBootMode_t initMode
// return       : N/A
//-------------------------------------------------------------------
void GC_OneTimeInit(InitBootMode_t initMode)
{
    // ---------GC LP Buf Init-------------
    gdwGCLastPageBuf = (U32*)MEM_AllocBuffer(GC_FTL_LP_SIZE * FTL_GROUP_NUM * GC_PING_PONG_NUM, 512);

    #if (GC_REMAP_MODE == TRUE)
        gGCremapInfo.array = (tGC_NODE*)MEM_AllocBuffer(sizeof(tGC_NODE) * GC_REMAP_SIZE , 512);
    #endif

    #if (DBG_GC_LP == TRUE)
      gdwGCTestBuf = (U32*)MEM_AllocBuffer(GC_FTL_LP_SIZE * FTL_GROUP_NUM , 16);
    #endif

    // -----------GC Var Init--------------
    GC_GlobalVarInit();

    // -----------GC BOOT------------------
    MemCopyNU32((U32*)&gGCInfo.boot, (U32*)&gFtlSporInfo, sizeof(tFTL_SPOR_INFO));

    GC_ModeAssert(WORD_MASK, WORD_MASK);
    if (gGCInfo.boot.flag || GC_MODE_CHECK_BOOT_ACTIVE)
    {
        gFtlPowerOn = TRUE;
        GC_Boot();
        gFtlPowerOn = FALSE;
    }
    if (gFtlTrimInfo.trimBlockcnt > (FTL_PBT_PERIOD_BLK * FTL_QBT_BLOCK_COUNT))
    {
        FTL_SaveFakeQBT();
        FTL_CheckUserBlocks();
    }
    gFtlTrimInfo.trimBlockcnt = 0;
}

//-------------------------------------------------------------------
// Function     : void GC_DumpInfo(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void GC_DumpInfo(void)
{
    U8  idx;
    U32 NANDCap = (U32)(((U64)gNfInfo.totalPages * GC_NF_DATA_BYTE_PER_PAGE) / GBYTE);

    D_PRINTF_GC("-Info-\n");
    D_PRINTF_GC("M[%2x]S[%2x] GCTH[%4x/%4x/%4x] LK[%2x] PPCUR/NXT[%1d/%1d]\n",
                GC_MODE_CHECK,
                GC_STATE_CHECK,
                GC_BLKCNT_ACTIVE,
                GC_BLKCNT_DEACTIVE,
                GC_BLKCNT_DISKFULL,
                gGCInfo.state.b.readOnly,
                gGCInfo.curPpPtr,
                gGCInfo.nxtPpPtr
                );
    D_PRINTF_GC("NANDCapBlk[%d] HostCap[%04d]GiB\n",
                LBU2GBYTE(IDEMA_GBYTE2LBU(GC_NF_USER_CAPACITY))*1024/(NANDCap*1024/*MB*//(GC_NF_PAGE_PER_BLK * GC_NF_PLANE_PER_LUN)/*Blk*/),
                LBU2GBYTE(IDEMA_GBYTE2LBU(GC_NF_USER_CAPACITY))
                );
    D_PRINTF_GC("TotBlk[%2d]TotHBlk[%2d]TabBlk[%d]OpCnt[%2d]OpRatio[%2d]EmRatio[%2d]L2PNUM[%x]\n\n",
                gGCInfo.op.totBlkCnt,
                gGCInfo.op.totHBlkCnt,
                gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_QBT] + gFtlBlkListInfo.blkCnt[FTL_BLOCK_POOL_PBT],
                gGCInfo.op.opBlkCnt,
                gGCInfo.op.opRatio,
                gGCInfo.op.emgrRatio,
                L2P_NUM
                );

    for (idx = 0; idx < GC_PING_PONG_NUM; idx++)
    {
        D_PRINTF("========PP IDX :%2x========\n",idx);
        D_PRINTF(" Mode/Type :%2x/%2x\n", gGCppBuf[idx].mode_Info.mode, gGCppBuf[idx].mode_Info.type);
        D_PRINTF(" Block     :%4x\n", gGCppBuf[idx].blk_Info.blk);
        D_PRINTF(" Valid Cnt :%8x\n", gGCppBuf[idx].blk_Info.validCnt);
        D_PRINTF(" BLK OP    :%4d\n", gGCppBuf[idx].blk_Info.opRatio);
        D_PRINTF(" CUR/NXT ST    :%2x/%2x\n", gGCppBuf[idx].fsm_Info.curState, gGCppBuf[idx].fsm_Info.nxtState);
        D_PRINTF(" LP BM I/D/E   :%2x/%2x/%2x\n", gGCppBuf[idx].lp_Info.issueCnt, gGCppBuf[idx].lp_Info.doneCnt, gGCppBuf[idx].lp_Info.errorCnt);
        D_PRINTF(" QoS  Host TH  :%8x\n", gGCppBuf[idx].qos_Info.hostCacheTH);
        D_PRINTF(" QoS  Host Cnt :%8x\n", gGCppBuf[idx].qos_Info.hostCacheCnt);
        D_PRINTF(" QoS  GC   TH  :%8x\n", gGCppBuf[idx].qos_Info.gcCacheTH);
        D_PRINTF(" QoS  GC   Cnt :%8x\n", gGCppBuf[idx].qos_Info.gcCacheCnt);
        D_PRINTF(" Recyc Paa     :%8x\n", gGCppBuf[idx].recyc_Info.curPaa);
        D_PRINTF(" Recyc Paa Cnt :%8x\n", gGCppBuf[idx].recyc_Info.recycPaaCnt);
    }

    D_PRINTF("========GC_AUX:Issue/DoneCNT[%d|%d]========\n", gGCAux.issuecnt, gGCAux.donecnt);
    for (idx =0; idx < GC_MAX_AUX_JOB;idx++)
    {
        D_PRINTF(" GC_AUX[%d] F/T[%3d|%3d]\n",idx, gGCAux.pJobbuf[idx]->fid, gGCAux.pJobbuf[idx]->tid);

    }
}

//-------------------------------------------------------------------
// Function     : void GC_Test(U32 arg0, U32 arg1, U32 arg2)
// Description  :
// Input        : U32 arg0, U32 arg1, U32 arg2
// return       : N/A
//-------------------------------------------------------------------
void GC_Test(U32 arg0, U32 arg1, U32 arg2)
{
    #if (DBG_GC_FUNCTION == TRUE)
    // UART Testing Funtion (GCTEST arg0 arg1 arg2)
    D_PRINTF_GC("TEST FUNCTION arg0[%4x] arg1[%4x] arg2[%4x]\n", arg0, arg1, arg2);


    U32 blk    = arg1;
    U16 pool   = gFtlBlkListTbl[blk].vs.b.status;
    U16 header = arg2;
    switch(arg0)
    {
        case 0 :
                GC_PushBlk(blk, pool, header);
                break;
        default: D_PRINTF_GC("No Gc Testing\n");break;
    }

    D_PRINTF("TEST DONE\n");
    #endif
}

#endif
