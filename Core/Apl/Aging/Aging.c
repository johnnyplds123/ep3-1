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
//! \file
//! @brief FIO(Flash Input/Output) control API codes
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"

#include "RegEcu.h"
#include "RegApb.h"
#include "RegDdrMc.h"

#include "Fio.h"
#include "SysInfo.h"
#include "Cache.h"
#include "CoreDbg.h"
#include "Core.h"
#include "Ftl.h"
#include "Heap.h"
#include "Thermal.h"

#include "Aging.h"

#include "FastSection.h"
//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define AGING_GET_PNDIE(CH, CE, LN, PN)    (CH  | (CE << gNfInfo.bitsCh) | (LN << (gNfInfo.bitsChDev)) | (PN << (gNfInfo.bitsChDev + gNfInfo.bitsLun)))
#define AGING_GET_ERR_IDX(DIE, CNT)        (DIE | (CNT << (gNfInfo.bitsChDev + gNfInfo.bitsLun + gNfInfo.bitsPlane)))
#define AGING_GET_PBLK(BLK, PN)            ((BLK << gNfInfo.bitsPlane) + PN)
#define AGING_GET_FREE_IDX(PN, DIE)        (PN + gNfInfo.planesPerLun * DIE)

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function proto-type definitions:
//-----------------------------------------------------------------------------
void Aging_Util_BubbleSort(void);
void Aging_Util_ReBuildTable(void);
void Aging_Util_RandomBuffer(U8 *pBuf, U32 wdSector);
void Aging_JobIssue(tFIO_JOB* pJob);
void Aging_JobWait(void);
U8 Aging_Nand_EccScan(tPAA paa,U16 type, U16 cnt);

static void Aging_SaveInfo(void);
static tERROR_AGING Aging_MainInit(void);
static void Aging_LoopInit(U16 loop);
static void Aging_DramInit(void);
static void Aging_BufferInit(void);

static tERROR_AGING Aging_Dram_Check(void);
static tERROR_AGING Aging_Start(void);
static void Aging_End(void);

static void Aging_Dram_Tune(void);
static void Aging_Dram_SaveParameter(void);
static void Aging_Nand_SaveInfo(void);
static void Aging_Nand_ScanDefect(void);
static void Aging_Nand_Temperature(void);

static void Aging_ProcJobError(tFIO_JOB* pJob);
static void Aging_ProcJobComplete(void);

static void Aging_SOC_Temperature(void);
static tERROR_AGING Aging_SOC_TypeCheck(void);

static void Aging_ErrBlk_Record(tPAA errPaa, U16 cmd, U16 status, U8 planeCnt);
static void Aging_ErrBlk_Mark(tPAA errPaa, U16 status, U8 planeCnt);
static void Aging_ErrBlk_Save(tPAA errPaa, U16 phyBlk, U16 errType, U8 planeCnt);

static void Aging_MP_SaveInfo(void);
static void Aging_MP_CalcPlistCnt(void);
static void Aging_MP_CalcFailDieCnt(void);

static void Aging_SysInfoUpdate(U16 Loop,U8 Mode);
static void Aging_DumpInfo(U32 DisCnt, U32 param);

#if (AGING_LED == ENABLE)
    static void Aging_Led_Manage(void);
    static void Aging_Led_DisplayErrorCode(U8 ErrorCode);
    static void Aging_Led_BlinkPass(U8 DoPowerCycle,U8 PowerCyclesFinished);
    static void Aging_Led_BlinkFail(U8 ErrorHi, U8 ErrorLo, U8 LongHi, U8 LongLo);
#endif

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
U32   gdwAgEnFrag;
tPAA* gAgingFastPaa1Buf;
tPAA* gAgingFastPaa2Buf;
U16*  gwAgFreeIdx;
U16*  gwAgRemapTbl;
U8*   gbAgflagTbl;
U8*   gbAgBuffer;

tAGING_INFO     gAgingInfo;
tAGING_ERROR    gAgingError;
tAGING_LOOP     gAgingLoop;

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------
extern tERROR_AGING Aging_Operator(U16 loop);
extern tERROR_AGING Aging_Nand_FlashID(void);
extern void Aging_Nand_BusyTime(void);
extern void Aging_EraseAll(void);
//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
#if CHK_FW_TYPE(FW_AGING)
//-------------------------------------------------------------------
// Function     : void Aging_OneTimeInit(InitBootMode_t initMode)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void Aging_OneTimeInit(InitBootMode_t initMode)
{
    Aging_BufferInit();

    gAgingFastPaa1Buf = (tPAA*)gFastPaaBuf;
    gAgingFastPaa2Buf = (tPAA*)gFastLaaBuf;

    smSysInfo->d.AgingData.d.Aging_Identify = DISABLE;

    Aging_DramInit();

    if  ((smSysInfo->d.AgingData.d.Aging_Signature == AGING_SIGN) || (smSysInfo->d.AgingData.d.Aging_Mode == ENABLE))
    {
        gdwAgEnFrag = AGING_AGEN;
    }
    else
    {
        gdwAgEnFrag = DISABLE;
    }

    D_PRINTF_AG("TargetLoop|%d CurrentLoop|%d ErrorCode|%x Rerun|%d ErrPaaCnt|%d SpecialErr|%x\n",
        smSysInfo->d.AgingData.d.AgingLoop.LoopTarget,
        smSysInfo->d.AgingData.d.AgingLoop.LoopCurrent,
        smSysInfo->d.AgingData.d.Aging_ErrorCode,
        smSysInfo->d.AgingData.d.AgingRerun.ReRunCount,
        smSysInfo->d.AgingData.d.AgingErrInfo.ErrCnt,
        smSysInfo->d.AgingData.d.AgingErrInfo.ErrNonCWO);
}

//-------------------------------------------------------------------
// Function     : static tERROR_AGING Aging_MainInit(void)
// Description  :
// Input        : N/A
// return       : ErrorCode
//-------------------------------------------------------------------
static tERROR_AGING Aging_MainInit(void)
{
    U8    version[strlen(_FW_REV_)], cnt;
    U8    size_rev = strlen(_FW_REV_);

    MEM_CLR(&gAgingInfo.Tick, ((U32)&gAgingInfo.ReRunCnt - (U32)&gAgingInfo.Tick + sizeof(gAgingInfo.ReRunCnt)));

    //---Write_VERSION--------------------------------------------------
    MEM_COPY((char *)&smSysInfo->d.AgingData.d.Aging_ModelName,_SRC_REV_,4);
    MEM_COPY(smSysInfo->d.AgingData.d.Aging_SN, smSysInfo->d.MPInfo.d.SerialNumberPCBA, 16);
    MEM_COPY(smSysInfo->d.AgingEI.d.SN, smSysInfo->d.MPInfo.d.SerialNumberPCBA, 16);
    MEM_COPY(&version[0], _FW_REV_, strlen(_FW_REV_));
    smSysInfo->d.AgingData.d.Aging_VerNo = (version[size_rev - 1] - 0x30) | ((version[size_rev - 2] - 0x30) <<  8);
    //---Write_VERSION--------------------------------------------------

    #if (AGING_LED == ENABLE)
    Aging_Led_Manage();
    #endif

    smSysInfo->d.Header.d.SubHeader.d.Aging.d.dwValidTag   = SI_TAG_AGING;
    smSysInfo->d.Header.d.SubHeader.d.MPInfo.d.dwValidTag  = SI_TAG_MPINFO;
    smSysInfo->d.Header.d.SubHeader.d.AgingEI.d.dwValidTag = SI_TAG_AGINGEI;

    smSysInfo->d.AgingEI.d.ErrInfoTag 					   = AGING_ERIN;

    smSysInfo->d.AgingData.d.AgingSoc.SocTypeTag             = AGING_SOCT;
    smSysInfo->d.AgingData.d.AgingRerun.ReRunTag             = AGING_RERN;
    smSysInfo->d.AgingData.d.AgingLoop.LoopTag               = AGING_LOOP;
    smSysInfo->d.AgingData.d.AgingErrInfo.ErrInfoTag         = AGING_ERIN;
    smSysInfo->d.AgingData.d.AgingNandPP.NfifTag             = AGING_NFIF;
    smSysInfo->d.AgingData.d.AgingFlashID.FlashIDTag         = AGING_NFID;
    smSysInfo->d.AgingData.d.AgingDram.DramTag               = AGING_DRAM;
    smSysInfo->d.AgingData.d.AgingPdinfo.PDTag               = AGING_PDIF;
    smSysInfo->d.AgingData.d.AgingSbkCnt.SupperBlkTag        = AGING_SBLK;
    smSysInfo->d.AgingData.d.AgingTemperature.TemperatureTag = AGING_MOTP;
    smSysInfo->d.AgingData.d.AgingNandBusyTime.NBTimeTag     = AGING_NBTI;

    gAgingInfo.ErrorCode                               = ERR_NO_ERROR;
    smSysInfo->d.AgingData.d.Aging_ErrorCode           = ERR_NO_ERROR;
    smSysInfo->d.AgingData.d.Aging_Signature           = AGING_GOGO;
    smSysInfo->d.AgingData.d.AgingErrInfo.TotalDie     = gNfInfo.totalLun;

    if (smSysInfo->d.AgingData.d.AgingRerun.ReRunValidTag != AGING_RERN)
    {
        smSysInfo->d.AgingData.d.AgingRerun.ReRunValidTag = AGING_RERN;
        smSysInfo->d.AgingData.d.AgingRerun.ReRunCount    += ++gAgingInfo.ReRunCnt;
        smSysInfo->d.MPInfo.d.ReAgingCount                 = smSysInfo->d.AgingData.d.AgingRerun.ReRunCount;
    }

    if ((smSysInfo->d.AgingData.d.Aging_Mode == ENABLE) && (smSysInfo->d.AgingData.d.Aging_Identify == DISABLE))
    {
        if  ((smSysInfo->d.MPInfo.d.StationCode[0].FunctionBoard != 0x00) ||
           (smSysInfo->d.MPInfo.d.StationCode[1].FunctionBoard != 0x00))
        {
            gAgingInfo.ErrorCode                     = ERR_BF2_ERROR;
            smSysInfo->d.AgingData.d.Aging_ErrorCode = ERR_BF2_ERROR;
        }
        else if (smSysInfo->d.AgingData.d.AgingRerun.ReRunCount >= AGING_MAX_RERUN_COUNT)
        {
            gAgingInfo.ErrorCode                     = ERR_MAX_RERUN;
            smSysInfo->d.AgingData.d.Aging_ErrorCode = ERR_MAX_RERUN;
        }
    }
    return gAgingInfo.ErrorCode;
}

//-------------------------------------------------------------------
// Function     : static void Aging_BufferInit(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void Aging_BufferInit(void)
{
    gAgingError.ErrPaa    = (tPAA*)MEM_AllocBuffer(sizeof(tPAA) * gNfInfo.totalBlocks, 4);
    MEM_CLR(gAgingError.ErrPaa, sizeof(tPAA) * gNfInfo.totalBlocks);

    gAgingError.ErrPaaCnt =  (U32*)MEM_AllocBuffer(sizeof(U32)  * gNfInfo.planesPerLun * gNfInfo.totalLun, 4);
    MEM_CLR(gAgingError.ErrPaaCnt, sizeof(U32) * gNfInfo.planesPerLun * gNfInfo.totalLun);

    gwAgFreeIdx = (U16*)MEM_AllocBuffer(sizeof(U16) * gNfInfo.planesPerLun * gNfInfo.totalLun, 4);
    MEM_CLR(gwAgFreeIdx, sizeof(U16) * gNfInfo.planesPerLun * gNfInfo.totalLun);

    gwAgRemapTbl = (U16*)MEM_AllocBuffer(sizeof(U16) * gNfInfo.blocksPerPlane * gNfInfo.planesPerLun * gNfInfo.totalLun, 4);
    MEM_CLR(gwAgRemapTbl, sizeof(U16) * gNfInfo.blocksPerPlane * gNfInfo.planesPerLun * gNfInfo.totalLun);

    gbAgflagTbl  = (U8*)MEM_AllocBuffer(sizeof(U8) * gNfInfo.totalLun, 4);
    MEM_CLR(gbAgflagTbl, sizeof(U8) * gNfInfo.totalLun);

    gbAgBuffer   = (U8*)MEM_AllocBuffer(9 * MBYTE, 512);

    //L2P Table for DramTest
    gFtlL2PTbl   = (tPAA*)MEM_AllocBuffer(sizeof(tPAA)*L2P_NUM, 16);
    MEM_SET(gFtlL2PTbl, BYTE_MASK, sizeof(tPAA)*L2P_NUM);
}

//-------------------------------------------------------------------
// Function     : static void Aging_DramInit(void)
// Description  : Dram Test
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void Aging_DramInit(void)
{
    U32 tmpDram;

    tmpDram = smAgingDramInfo.AgingDramStatus;
    MEM_CLR((PVOID)&smAgingDramInfo, sizeof(tDRAMTEST_INFO));
    smAgingDramInfo.AgingDramStatus = tmpDram;
    smAgingDramInfo.AgingL2PPoint =  (U32)&gFtlL2PTbl[0];
    smAgingDramInfo.AgingDramTestSize = sizeof(tPAA)*L2P_NUM/sizeof(U32);
}

//-------------------------------------------------------------------
// Function     : static void Aging_LoopInit(U16 loop)
// Description  :
// Input        : loop
// return       : N/A
//-------------------------------------------------------------------
static void Aging_LoopInit(U16 loop)
{
    MEM_CLR(&gAgingLoop, sizeof(tAGING_LOOP));

    if (smSysInfo->d.AgingData.d.AgingLoop.LoopValidTag != AGING_LOOP)
    {
        MEM_CLR(smSysInfo->d.AgingData.d.AgingLoop.LoopDefectCnt, sizeof(tAG_COUNT));
        smSysInfo->d.AgingData.d.AgingLoop.LoopValidTag = AGING_LOOP;
        smSysInfo->d.AgingData.d.AgingLoop.LoopCurrent  = 0;
    }
    gAgingInfo.CurrLoopCnt = smSysInfo->d.AgingData.d.AgingLoop.LoopCurrent = loop;

    D_PRINTF_AG("CurrentLoop|%d ErrorCode|%x Rerun|%d\n",
        smSysInfo->d.AgingData.d.AgingLoop.LoopCurrent,
        smSysInfo->d.AgingData.d.Aging_ErrorCode,
        smSysInfo->d.AgingData.d.AgingRerun.ReRunCount);

    SyIn_Synchronize(SI_AREA_AGING | SI_AREA_MPINFO, SYSINFO_WRITE_FORCE, SI_SYNC_BY_AGING);
}

//-------------------------------------------------------------------
// Function     : void Aging_MainLoop(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void Aging_MainLoop(void)
{
    U16 loop;

    if (Aging_MainInit() != ERR_NO_ERROR)   goto LBL_EXIT_AGING;
    if (Aging_Start() != ERR_NO_ERROR)      goto LBL_EXIT_AGING;

    for (loop = smSysInfo->d.AgingData.d.AgingLoop.LoopCurrent; loop < smSysInfo->d.AgingData.d.AgingLoop.LoopTarget; loop++)
    {
        Aging_LoopInit(loop);

        if (Aging_Dram_Check() != ERR_NO_ERROR)     goto LBL_EXIT_AGING;
        if (Aging_Operator(loop) != ERR_NO_ERROR)   goto LBL_EXIT_AGING;

        Aging_SaveInfo();
        Aging_MP_SaveInfo();

        SyIn_Synchronize(SI_AREA_PLSTCNT | SI_AREA_PLIST2 | SI_AREA_AGING | SI_AREA_MPINFO | SI_AREA_AGINGEI, SYSINFO_WRITE_FORCE, SI_SYNC_BY_AGING);
    }

    LBL_EXIT_AGING:
        Aging_End();
}

//-------------------------------------------------------------------
// Function     : static tERROR_AGING Aging_Start(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static tERROR_AGING Aging_Start(void)
{
    Aging_Dram_Tune();

    if (Aging_SOC_TypeCheck() != ERR_NO_ERROR)  return gAgingInfo.ErrorCode;

    Aging_SOC_Temperature();

    Aging_Nand_SaveInfo();

    if (Aging_Nand_FlashID() != ERR_NO_ERROR)   return gAgingInfo.ErrorCode;

    if (gAgingInfo.ErrorCode == ERR_NO_ERROR)
    {
        Aging_Nand_ScanDefect();

        if (smSysInfo->d.Header.d.SubHeader.d.PList2.d.dwValidTag == SI_TAG_PLIST2)
        {
            Aging_MP_CalcPlistCnt();
            MEM_COPY(smSysInfo->d.AgingData.d.AgingSbkCnt.DefectDieMap, smSysInfo->d.AgingData.d.AgingSbkCnt.SbkDieMap, sizeof(smSysInfo->d.AgingData.d.AgingSbkCnt.SbkDieMap));

            if (gAgingInfo.ErrorCode == ERR_DEFECT_BY_CH)
                gAgingInfo.ErrorCode = ERR_SCAN_DEFECT;
            else if (gAgingInfo.ErrorCode == ERR_SUPBLK_FAIL)
                gAgingInfo.ErrorCode = ERR_ORISUPBLK_FAIL;
        }
        Aging_Nand_BusyTime();
    }

    SyIn_Synchronize(SI_AREA_PLSTCNT | SI_AREA_PLIST2 | SI_AREA_AGING | SI_AREA_MPINFO | SI_AREA_AGINGEI, SYSINFO_WRITE_FORCE, SI_SYNC_BY_AGING);

    return gAgingInfo.ErrorCode;
}

//-------------------------------------------------------------------
// Function     : static void Aging_End(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void Aging_End(void)
{
    Aging_Dram_Check();
    smAgingDramInfo.AgingDramTag = DWORD_MASK;
    HalTimer_DelayMsec(1000);

    switch (gAgingInfo.ErrorCode)
    {
        case ERR_NO_ERROR:
            smSysInfo->d.AgingData.d.Aging_Signature = AGING_PASS;   //0xPASS =0x50415353
            smSysInfo->d.AgingData.d.Aging_Mode      = DISABLE;
            break;
        case ERR_BF2_ERROR:
        case ERR_DATACOMPARE_FAIL:
        case ERR_FLASH_ID:
        case ERR_DRAMCAL_FAIL:
        case ERR_SOC_TYPE:
        case ERR_DRAMALL_FAIL:
        case ERR_SCAN_DEFECT:
        case ERR_ORISUPBLK_FAIL:
        case ERR_SIDATA_FAIL:
        case ERR_DRAMDLL_OVLM:
        case ERR_PLIST1_FAIL:
        case ERR_MAX_RERUN:
        case ERR_DRAM_HT_FAIL:
            smSysInfo->d.AgingData.d.Aging_Mode      = DISABLE;
            smSysInfo->d.AgingData.d.Aging_Signature = AGING_FAIL;
            break;
        case ERR_UNKNOWN_ERROR:
            if (smSysInfo->d.AgingData.d.AgingRerun.RecordError != ERR_UNKNOWN_ERROR)
            {
                smSysInfo->d.AgingData.d.AgingRerun.ReRunCount  = 0x00;
                smSysInfo->d.AgingData.d.AgingRerun.RecordError = ERR_UNKNOWN_ERROR;
                smSysInfo->d.AgingData.d.Aging_Signature        = AGING_SIGN;
                smSysInfo->d.AgingData.d.Aging_Mode             = ENABLE;
                gAgingInfo.ErrorCode                            = ERR_NO_ERROR;
                Aging_MainLoop();
            }
            break;
        default:
            smSysInfo->d.AgingData.d.Aging_Mode = (smSysInfo->d.AgingData.d.Aging_Mode == ENABLE) ? ENABLE : DISABLE;
            break;
    }

    if ((gAgingInfo.ErrorCode != ERR_SCAN_DEFECT) && (gAgingInfo.ErrorCode != ERR_ORISUPBLK_FAIL))
    {
        Aging_MP_CalcPlistCnt();
    }
    if (gAgingError.DefectCntTotal)
    {
        Aging_MP_CalcFailDieCnt();
    }

    smSysInfo->d.MPInfo.d.StationCode[0].FunctionAging = smSysInfo->d.AgingData.d.Aging_ErrorCode;
    smSysInfo->d.MPInfo.d.StationCode[1].FunctionAging = smSysInfo->d.AgingData.d.Aging_ErrorCode;
    smSysInfo->d.AgingData.d.AgingPdinfo.SOC_Temp  = (U32)HalTempSensor_GetTemperature();

    Aging_Nand_Temperature();
    Aging_Dram_SaveParameter();
    Aging_SaveInfo();
    Aging_MP_SaveInfo();

    SyIn_Synchronize(SI_AREA_PLSTCNT | SI_AREA_PLIST2 | SI_AREA_AGING | SI_AREA_MPINFO | SI_AREA_AGINGEI, SYSINFO_WRITE_FORCE, SI_SYNC_BY_AGING);
    SyIn_Synchronize(SI_AREA_PLSTCNT | SI_AREA_PLIST1 | SI_AREA_PLIST2 | SI_AREA_AGING, SYSINFO_WRITE, SI_SYNC_BY_NOR);

    D_PRINTF_AG("Loop|%d ErrorCode|%2x RecordError|%x ElapsedTime|%d Defect|%d ReAgingCount|%d\n",
            smSysInfo->d.AgingData.d.AgingLoop.LoopCurrent,
            smSysInfo->d.AgingData.d.Aging_ErrorCode,
            smSysInfo->d.AgingData.d.AgingRerun.RecordError,
            gAgingInfo.TotalTime,
            gAgingError.DefectCntTotal,
            smSysInfo->d.AgingData.d.AgingRerun.ReRunCount);

    D_PRINTF_AG("End\n");

    #if (AGING_LED == ENABLE)
    Aging_Led_DisplayErrorCode(gAgingInfo.ErrorCode);
    #endif
}

//-------------------------------------------------------------------
// Function     : static void Aging_SaveInfo()
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void Aging_SaveInfo(void)
{
    smSysInfo->d.AgingData.d.AgingLoop.LoopDefectCnt[gAgingInfo.CurrLoopCnt].Loop        = gAgingInfo.CurrLoopCnt;
    smSysInfo->d.AgingData.d.AgingLoop.LoopDefectCnt[gAgingInfo.CurrLoopCnt].BadBlockCnt = gAgingLoop.LoopDefectCnt;
    smSysInfo->d.AgingData.d.AgingLoop.LoopCurrent                                       = gAgingInfo.CurrLoopCnt;
    smSysInfo->d.AgingData.d.Aging_ErrorCode                                             = gAgingInfo.ErrorCode;
    smSysInfo->d.AgingData.d.Aging_ElapsedTime                                          += gAgingInfo.TotalTime;
    smSysInfo->d.AgingData.d.Aging_EraseCnt++;
}

//-------------------------------------------------------------------
// Function     : void Aging_JobIssue(tFIO_JOB* pJob)
// Description  :
// Input        : pJob
// return       : N/A
//-------------------------------------------------------------------
void Aging_JobIssue(tFIO_JOB* pJob)
{
    tPAA* HeapPaa;
    U32   timeout = 0;

    while (smFioInfo.jobCnt[FIO_JOB_POOL_FREE] <= (FIO_MAX_JOB - 32))
    {
        Aging_ProcJobComplete();

        HalTimer_DelayUsec(5);
        timeout++;
        if (timeout >= (FIO_TIMEOUT_US / 5))
        {
            smAgingDramInfo.AgingDramTag = DWORD_MASK;
            HalTimer_DelayMsec(1000);
            ASSERT(timeout < (FIO_TIMEOUT_US / 5));
        }
    }

    #if (FIO_JOB_HEAP_MEMORY == TRUE)
    HeapPaa = (tPAA*)Heap_Alloc(pJob->paaCnt + (pJob->flag & FIO_F_AUX) * 2 * pJob->paaCnt);

    timeout = 0;
    while (HeapPaa == NULL)
    {
        if (smFioInfo.jobCnt[FIO_JOB_POOL_FREE] < FIO_MAX_JOB)
        {
            Aging_ProcJobComplete();
        }

        HeapPaa = (tPAA*)Heap_Alloc(pJob->paaCnt + (pJob->flag & FIO_F_AUX) * 2 * pJob->paaCnt);

        HalTimer_DelayUsec(5);
        timeout++;
        ASSERT(timeout < (HEAP_ALLOC_TIMEOUT_US / 5));
    }
    #endif

    ASM_DMB();
    if (pJob->cmd < FIO_JOB_CMD_ERASE && pJob->css == CACHE_AUTO)
    {
        Cache_Alloc_Auto(pJob);
    }

    #if (FIO_JOB_HEAP_MEMORY == TRUE)
    MEM_COPY(HEAP_GET_PTR_CPU1_B0TCM(HeapPaa), pJob->paa, sizeof(tPAA) * pJob->paaCnt);
    pJob->paa = HeapPaa;
    #endif

    IPC_SendMsgQ(cC2mReqQueue, (Msg_t*)pJob);
    smFioInfo.jobIssueCnt++;
}

//-------------------------------------------------------------------
// Function     : void Aging_JobWait(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void Aging_JobWait(void)
{
    U32   timeout = 0;

    while (smFioInfo.jobCnt[FIO_JOB_POOL_FREE] != FIO_MAX_JOB)
    {
        Aging_ProcJobComplete();

        HalTimer_DelayUsec(5);
        timeout++;
        if (timeout >= (FIO_TIMEOUT_US / 5))
        {
            smAgingDramInfo.AgingDramTag = DWORD_MASK;
            HalTimer_DelayMsec(1000);
            ASSERT(timeout < (FIO_TIMEOUT_US / 5));
        }
    }
}

//-------------------------------------------------------------------
// Function     : static void Aging_ProcJobError(tFIO_JOB* pJob)
// Description  :
// Input        : pJob
// return       : N/A
//-------------------------------------------------------------------
static void Aging_ProcJobError(tFIO_JOB* pJob)
{
    tPAA errPaa;
    U16  cnt;

    for (cnt = 0; cnt < (pJob->paaCnt); cnt++)
    {
        if (FIO_CHK_ERR_BMP(pJob->fid, cnt))
        {
            errPaa = pJob->paa[cnt];
            if (pJob->css == CACHE_AUTO &&
                pJob->auFmt == 0        &&
                pJob->cmd != FIO_JOB_CMD_READ_SLC_TOUCH &&
                pJob->flag & FIO_F_NO_RR ||
                pJob->cmd == FIO_JOB_CMD_WRITE ||
                pJob->cmd == FIO_JOB_CMD_ERASE)
            {
                Aging_ErrBlk_Record(errPaa, pJob->cmd, pJob->status, pJob->plane);
            }
            FIO_CLR_ERR_BMP(pJob->fid, cnt);
        }
    }
}

//-------------------------------------------------------------------
// Function     : static void Aging_ProcJobComplete(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void Aging_ProcJobComplete(void)
{
    tFIO_JOB* pJob = NULL;

    if (IPC_MsgQFastPeek(cM2cComQueue))
    {
        pJob   = (tFIO_JOB*) IPC_GetMsgQ(cM2cComQueue);

        #if (FIO_JOB_HEAP_MEMORY == TRUE)
        Heap_Free((PVOID)pJob->paa);
        pJob->paa = &smFioInfo.pDramPaa[pJob->fid * MAX_PAA_PER_JOB];

        if (pJob->flag & FIO_F_AUX)
        {
            if (pJob->cmd < FIO_JOB_CMD_WRITE)
            {
                MEM_COPY(&smFioInfo.pDramAux[pJob->fid * MAX_AUX_PER_JOB], HEAP_GET_PTR_CPU1_B0TCM(pJob->aux), sizeof(tPAA) * pJob->paaCnt * 2);
            }

            pJob->aux = &smFioInfo.pDramAux[pJob->fid * MAX_AUX_PER_JOB];
        }
        #endif

        if (pJob->status != FIO_S_NO_ERR) Aging_ProcJobError(pJob);

        gCbfCacheTrkDone[pJob->css](pJob);
        FIO_PushJob(FIO_JOB_POOL_FREE, pJob);
        smFioInfo.jobDoneCnt++;
    }
}

//-------------------------------------------------------------------
// Function     : tSTATUS_AGING Aging_CheckIssueSBlk(U16 blk, U8 ce, U8 lun)
// Description  : Check Issue SupperBlock
// Input        : block, ce, lun
// return       : tSTATUS_AGING
//-------------------------------------------------------------------
tSTATUS_AGING Aging_CheckIssueSBlk(U16 block, U8 ce, U8 lun)
{
    U32 idx;
    U16 die, rmpblk;
    U8  ch, plane, flag1, flag2;

    MEM_CLR(gbAgflagTbl, sizeof(U8) * gNfInfo.totalLun);

    flag1 = flag2 = 0;

    // --- Chech Issue MultiPlane------------------------------------------------------------------------
    for (ch = 0; ch < gNfInfo.numChannels; ch++)
    {
        die = AGING_GET_DIE(lun, ce ,ch);
        for (plane = 0; plane < gNfInfo.planesPerLun; plane++)
        {
            idx     = AGING_GET_SBLK_IDX(plane, block, die);
            rmpblk  = gwAgRemapTbl[idx];

            if  (rmpblk == AGING_BAD_BLOCK)
            {
                gbAgflagTbl[die] += 1;
            }
        }
    }

    for (ch = 0; ch < gNfInfo.numChannels; ch++)
    {
        die = AGING_GET_DIE(lun, ce ,ch);
        if  (gbAgflagTbl[die] == gNfInfo.planesPerLun)
        {
            flag1++;
        }
        else if ((gbAgflagTbl[die] != 0) && (gbAgflagTbl[die] != gNfInfo.planesPerLun))
        {
            flag2++;
        }
    }

    if  (flag1 == gNfInfo.numChannels)
        return STATUS_NON_ISSUE;
    else if ((flag1 == 0) && (flag2 == 0))
        return STATUS_ISSUE_SBLK;
    else
        return STATUS_ISSUE_NON_SBLK;
}

//-------------------------------------------------------------------
// Function     : static void AGING_SOC_MonTemperature(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void Aging_SOC_Temperature(void)
{
    HwTimerTick_t tick;
    U32           waitTime, totalSOC, totalI2C;
    U16           cnt, minSOC, minI2C, maxSOC, maxI2C;

    cnt = waitTime = totalSOC = totalI2C = maxSOC = maxI2C  = 0;
    minSOC = minI2C = WORD_MASK;

    if (smSysInfo->d.AgingData.d.AgingTemperature.TemperatureIdleTag != AGING_TC_IDLE)
    {
        tick     = HalTimer_ReadTick();
        while(1)
        {
            waitTime = HalTimer_GetElapsedTimeMsec(tick);
            if (((waitTime % AGING_TEMPERATURE_TIME) == 0) && (waitTime != 0))
            {
                smSysInfo->d.AgingData.d.AgingTemperature.TemperatureIdle[cnt].SOC   = (U16)HalTempSensor_GetTemperature();
                smSysInfo->d.AgingData.d.AgingTemperature.TemperatureIdle[cnt].I2C   = (S16)AplThermal_GetI2cTemperature();

                totalSOC += smSysInfo->d.AgingData.d.AgingTemperature.TemperatureIdle[cnt].SOC;
                totalI2C += smSysInfo->d.AgingData.d.AgingTemperature.TemperatureIdle[cnt].I2C;

                if (minSOC > smSysInfo->d.AgingData.d.AgingTemperature.TemperatureIdle[cnt].SOC)
                    minSOC = smSysInfo->d.AgingData.d.AgingTemperature.TemperatureIdle[cnt].SOC;
                if (minI2C > smSysInfo->d.AgingData.d.AgingTemperature.TemperatureIdle[cnt].I2C)
                    minI2C = smSysInfo->d.AgingData.d.AgingTemperature.TemperatureIdle[cnt].I2C;

                if (maxSOC < smSysInfo->d.AgingData.d.AgingTemperature.TemperatureIdle[cnt].SOC)
                    maxSOC = smSysInfo->d.AgingData.d.AgingTemperature.TemperatureIdle[cnt].SOC;
                if (maxI2C < smSysInfo->d.AgingData.d.AgingTemperature.TemperatureIdle[cnt].I2C)
                    maxI2C = smSysInfo->d.AgingData.d.AgingTemperature.TemperatureIdle[cnt].I2C;

                cnt++;
                if (cnt == 10)
                {
                    smSysInfo->d.AgingData.d.AgingTemperature.TemperatureIdle_Avg.SOC = (U16)(totalSOC / cnt);
                    smSysInfo->d.AgingData.d.AgingTemperature.TemperatureIdle_Avg.I2C = (S16)(totalI2C / cnt);
                    smSysInfo->d.AgingData.d.AgingTemperature.TemperatureIdle_Min.SOC = minSOC;
                    smSysInfo->d.AgingData.d.AgingTemperature.TemperatureIdle_Min.I2C = minI2C;
                    smSysInfo->d.AgingData.d.AgingTemperature.TemperatureIdle_Max.SOC = maxSOC;
                    smSysInfo->d.AgingData.d.AgingTemperature.TemperatureIdle_Max.I2C = maxI2C;

                    D_PRINTF_AG("'Min' SOC|%x I2C|%x\n",smSysInfo->d.AgingData.d.AgingTemperature.TemperatureIdle_Min.SOC, smSysInfo->d.AgingData.d.AgingTemperature.TemperatureIdle_Min.I2C);
                    D_PRINTF_AG("'Max' SOC|%x I2C|%x\n",smSysInfo->d.AgingData.d.AgingTemperature.TemperatureIdle_Max.SOC, smSysInfo->d.AgingData.d.AgingTemperature.TemperatureIdle_Max.I2C);
                    D_PRINTF_AG("'Avg' SOC|%x I2C|%x\n",smSysInfo->d.AgingData.d.AgingTemperature.TemperatureIdle_Avg.SOC, smSysInfo->d.AgingData.d.AgingTemperature.TemperatureIdle_Avg.I2C);

                    if((smSysInfo->d.AgingData.d.AgingTemperature.TemperatureIdle[7].SOC >= AGING_TEMPERATURE_LIMIT) || (smSysInfo->d.AgingData.d.AgingTemperature.TemperatureIdle[8].SOC >= AGING_TEMPERATURE_LIMIT) || (smSysInfo->d.AgingData.d.AgingTemperature.TemperatureIdle[9].SOC >= AGING_TEMPERATURE_LIMIT))
                    {
                        smSysInfo->d.AgingData.d.AgingTemperature.TemperatureIdle_High = AGING_TC_HIGH;
                    }

                    smSysInfo->d.AgingData.d.AgingTemperature.TemperatureIdleTag = AGING_TC_IDLE;
                    break;
                }
            }
        }
    }
    SyIn_Synchronize(SI_AREA_AGING | SI_AREA_MPINFO, SYSINFO_WRITE_FORCE, SI_SYNC_BY_AGING);
}

//-------------------------------------------------------------------
// Function     : static tERROR_AGING Aging_SOC_TypeCheck(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static tERROR_AGING Aging_SOC_TypeCheck(void)
{
    if (smHwInfo.d.socType.tag == AGING_SOC_TYPE_TAG)
    {
        smSysInfo->d.AgingData.d.AgingSoc.AVS = smHwInfo.d.socType.avs;

        #if (AGING_CHECK_SOC_TYPE == TRUE)
        if (smHwInfo.d.socType.avs != AGING_SOC_TYPE_TT)    gAgingInfo.ErrorCode = ERR_SOC_TYPE;
        #endif
    }

    return gAgingInfo.ErrorCode;
}

//-------------------------------------------------------------------
// Function     : static void Aging_Nand_SaveInfo(void)
// Description  : Backup NAND Info to Aging
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void Aging_Nand_SaveInfo(void)
{
    smSysInfo->d.AgingData.d.AgingNandPP.NfifTag   = AGING_NFIF;
    smSysInfo->d.AgingData.d.AgingNandPP.JEDECFail = smAgingNandInfo.JEDECFail;
    smSysInfo->d.AgingData.d.AgingNandPP.CRCFail   = smAgingNandInfo.CRCFail;
    smSysInfo->d.AgingData.d.AgingNandPP.FailCh    = smAgingNandInfo.FailCh;
    smSysInfo->d.AgingData.d.AgingNandPP.FailCe    = smAgingNandInfo.FailCe;
    smSysInfo->d.AgingData.d.AgingNandPP.FailLun   = smAgingNandInfo.FailLun;
    smSysInfo->d.AgingData.d.AgingNandPP.FailPlane = smAgingNandInfo.FailPlane;
}

//-------------------------------------------------------------------
// Function     : static void AGING_SOC_MonTemperature(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void Aging_Nand_Temperature(void)
{
    U32 totalSOC_E, totalI2C_E, totalSOC_W, totalI2C_W, totalSOC_R, totalI2C_R;
    U16 minSOC_E, minI2C_E, maxSOC_E, maxI2C_E, avgSOC_E, avgI2C_E;
    U16 minSOC_W, minI2C_W, maxSOC_W, maxI2C_W, avgSOC_W, avgI2C_W;
    U16 minSOC_R, minI2C_R, maxSOC_R, maxI2C_R, avgSOC_R, avgI2C_R;
    U16 loop;

    totalSOC_E = totalI2C_E = totalSOC_W = totalI2C_W = totalSOC_R = totalI2C_R = 0;
    maxSOC_E = maxI2C_E = maxSOC_W = maxI2C_W = maxSOC_R = maxI2C_R = 0;
    minSOC_E = minI2C_E = minSOC_W = minI2C_W = minSOC_R = minI2C_R = WORD_MASK;

    for (loop = 0; loop < (gAgingInfo.CurrLoopCnt + 1); loop++)
    {
        totalSOC_E += smSysInfo->d.AgingData.d.AgingTemperature.TemperatureErase[loop].SOC;
        totalI2C_E += smSysInfo->d.AgingData.d.AgingTemperature.TemperatureErase[loop].I2C;
        totalSOC_W += smSysInfo->d.AgingData.d.AgingTemperature.TemperatureWrite[loop].SOC;
        totalI2C_W += smSysInfo->d.AgingData.d.AgingTemperature.TemperatureWrite[loop].I2C;
        totalSOC_R += smSysInfo->d.AgingData.d.AgingTemperature.TemperatureRead[loop].SOC;
        totalI2C_R += smSysInfo->d.AgingData.d.AgingTemperature.TemperatureRead[loop].I2C;

        smSysInfo->d.AgingData.d.AgingTemperature.TemperatureEraseTag = AGING_TC_ERASE;
        if (minSOC_E > smSysInfo->d.AgingData.d.AgingTemperature.TemperatureErase[loop].SOC)
            minSOC_E = smSysInfo->d.AgingData.d.AgingTemperature.TemperatureErase[loop].SOC;
        if (minI2C_E > smSysInfo->d.AgingData.d.AgingTemperature.TemperatureErase[loop].I2C)
            minI2C_E = smSysInfo->d.AgingData.d.AgingTemperature.TemperatureErase[loop].I2C;
        if (maxSOC_E < smSysInfo->d.AgingData.d.AgingTemperature.TemperatureErase[loop].SOC)
            maxSOC_E = smSysInfo->d.AgingData.d.AgingTemperature.TemperatureErase[loop].SOC;
        if (maxI2C_E < smSysInfo->d.AgingData.d.AgingTemperature.TemperatureErase[loop].I2C)
            maxI2C_E = smSysInfo->d.AgingData.d.AgingTemperature.TemperatureErase[loop].I2C;

        smSysInfo->d.AgingData.d.AgingTemperature.TemperatureWriteTag = AGING_TC_WRITE;
        if (minSOC_W > smSysInfo->d.AgingData.d.AgingTemperature.TemperatureWrite[loop].SOC)
            minSOC_W = smSysInfo->d.AgingData.d.AgingTemperature.TemperatureWrite[loop].SOC;
        if (minI2C_W > smSysInfo->d.AgingData.d.AgingTemperature.TemperatureWrite[loop].I2C)
            minI2C_W = smSysInfo->d.AgingData.d.AgingTemperature.TemperatureWrite[loop].I2C;
        if (maxSOC_W < smSysInfo->d.AgingData.d.AgingTemperature.TemperatureWrite[loop].SOC)
            maxSOC_W = smSysInfo->d.AgingData.d.AgingTemperature.TemperatureWrite[loop].SOC;
        if (maxI2C_W < smSysInfo->d.AgingData.d.AgingTemperature.TemperatureWrite[loop].I2C)
            maxI2C_W = smSysInfo->d.AgingData.d.AgingTemperature.TemperatureWrite[loop].I2C;

        smSysInfo->d.AgingData.d.AgingTemperature.TemperatureReadTag = AGING_TC_READ;
        if (minSOC_R > smSysInfo->d.AgingData.d.AgingTemperature.TemperatureRead[loop].SOC)
            minSOC_R = smSysInfo->d.AgingData.d.AgingTemperature.TemperatureRead[loop].SOC;
        if (minI2C_R > smSysInfo->d.AgingData.d.AgingTemperature.TemperatureRead[loop].I2C)
            minI2C_R = smSysInfo->d.AgingData.d.AgingTemperature.TemperatureRead[loop].I2C;
        if (maxSOC_R < smSysInfo->d.AgingData.d.AgingTemperature.TemperatureRead[loop].SOC)
            maxSOC_R = smSysInfo->d.AgingData.d.AgingTemperature.TemperatureRead[loop].SOC;
        if (maxI2C_R < smSysInfo->d.AgingData.d.AgingTemperature.TemperatureRead[loop].I2C)
            maxI2C_R = smSysInfo->d.AgingData.d.AgingTemperature.TemperatureRead[loop].I2C;
    }
    smSysInfo->d.AgingData.d.AgingTemperature.TemperatureErase_Max.SOC = maxSOC_E;
    smSysInfo->d.AgingData.d.AgingTemperature.TemperatureErase_Max.I2C = maxI2C_E;
    smSysInfo->d.AgingData.d.AgingTemperature.TemperatureErase_Min.SOC = minSOC_E;
    smSysInfo->d.AgingData.d.AgingTemperature.TemperatureErase_Min.I2C = minI2C_E;
    smSysInfo->d.AgingData.d.AgingTemperature.TemperatureErase_Avg.SOC = totalSOC_E / (gAgingInfo.CurrLoopCnt + 1);
    smSysInfo->d.AgingData.d.AgingTemperature.TemperatureErase_Avg.I2C = totalI2C_E / (gAgingInfo.CurrLoopCnt + 1);

    smSysInfo->d.AgingData.d.AgingTemperature.TemperatureWrite_Max.SOC = maxSOC_W;
    smSysInfo->d.AgingData.d.AgingTemperature.TemperatureWrite_Max.I2C = maxI2C_W;
    smSysInfo->d.AgingData.d.AgingTemperature.TemperatureWrite_Min.SOC = minSOC_W;
    smSysInfo->d.AgingData.d.AgingTemperature.TemperatureWrite_Min.I2C = minI2C_W;
    smSysInfo->d.AgingData.d.AgingTemperature.TemperatureWrite_Avg.SOC = totalSOC_W / (gAgingInfo.CurrLoopCnt + 1);
    smSysInfo->d.AgingData.d.AgingTemperature.TemperatureWrite_Avg.I2C = totalI2C_W / (gAgingInfo.CurrLoopCnt + 1);

    smSysInfo->d.AgingData.d.AgingTemperature.TemperatureRead_Max.SOC = maxSOC_R;
    smSysInfo->d.AgingData.d.AgingTemperature.TemperatureRead_Max.I2C = maxI2C_R;
    smSysInfo->d.AgingData.d.AgingTemperature.TemperatureRead_Min.SOC = minSOC_R;
    smSysInfo->d.AgingData.d.AgingTemperature.TemperatureRead_Min.I2C = minI2C_R;
    smSysInfo->d.AgingData.d.AgingTemperature.TemperatureRead_Avg.SOC = totalSOC_R / (gAgingInfo.CurrLoopCnt + 1);
    smSysInfo->d.AgingData.d.AgingTemperature.TemperatureRead_Avg.I2C = totalI2C_R / (gAgingInfo.CurrLoopCnt + 1);

    SyIn_Synchronize(SI_AREA_AGING, SYSINFO_WRITE_FORCE, SI_SYNC_BY_AGING);
}

//-------------------------------------------------------------------
// Function     : static void Aging_Nand_ScanDefect(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void Aging_Nand_ScanDefect(void)
{
    tPAA paa;

    paa.all32 = 0;
    paa.b.block = ERASE_START_BLK;

    if (smSysInfo->d.Header.d.SubHeader.d.PList1.d.dwValidTag != SI_TAG_PLIST1)
    {
        CoreDbg_Center(COREDBG_CMD_ERASEALL, 0, paa, 0, 0, 0);
        MEM_CLR(&smSysInfo->d.PList1, sizeof(tSI_PAYLOAD_PLIST));
        MEM_CLR(&smSysInfo->d.PList2, sizeof(tSI_PAYLOAD_PLIST));
        MEM_CLR(&smSysInfo->d.PlistCnt.d.PList1Cnt, sizeof(tSI_PLIST_CNT));
        MEM_CLR(&smSysInfo->d.PlistCnt.d.PList2Cnt, sizeof(tSI_PLIST_CNT));

        CoreDbg_Center(COREDBG_CMD_SCANDEFECT, 0, paa, 0, 0, 0);
    }

    if (smSysInfo->d.Header.d.SubHeader.d.PList1.d.dwValidTag     == SI_TAG_PLIST1 &&
        smSysInfo->d.Header.d.SubHeader.d.PlistCnt.d.dwValidTag   == SI_TAG_PLSTCNT)
    {
        if (smSysInfo->d.Header.d.SubHeader.d.PList2.d.dwValidTag != SI_TAG_PLIST2)
        {
            MEM_COPY(&smSysInfo->d.PlistCnt.d.PList2Cnt, &smSysInfo->d.PlistCnt.d.PList1Cnt, sizeof(tSI_PLIST_CNT));
            MEM_COPY(&smSysInfo->d.PList2, &smSysInfo->d.PList1, sizeof(tSI_PAYLOAD_PLIST));

            smSysInfo->d.Header.d.SubHeader.d.PList2.d.dwValidTag      = SI_TAG_PLIST2;
            smSysInfo->d.Header.d.SubHeader.d.PList2.d.dwDWord4.d.bCH  = SI_NAND_MAX_CHANNEL;
            smSysInfo->d.Header.d.SubHeader.d.PList2.d.dwDWord4.d.bCE  = SI_NAND_MAX_CE;
            smSysInfo->d.Header.d.SubHeader.d.PList2.d.dwDWord4.d.bLUN = SI_NAND_MAX_LUN;
            smSysInfo->d.Header.d.SubHeader.d.PList2.d.dwDWord4.d.bCnt = SI_PLIST_MAX_COUNT / SI_PLIST_MAX_COUNT_STEP;

            SyIn_Synchronize(SI_AREA_PLSTCNT | SI_AREA_PLIST2 | SI_AREA_AGING | SI_AREA_MPINFO, SYSINFO_WRITE_FORCE, SI_SYNC_BY_AGING);
        }
    }
    else
    {
        gAgingInfo.ErrorCode                     = ERR_SCAN_DEFECT;
        smSysInfo->d.AgingData.d.Aging_ErrorCode = ERR_SCAN_DEFECT;
    }
}

//-------------------------------------------------------------------
// Function     : U8 Aging_Nand_EccScan(tPAA paa,U16 type, U16 cnt)
// Description  :
// Input        : paa, type, cnt
// return       : 0 / 1
//-------------------------------------------------------------------
U8 Aging_Nand_EccScan(tPAA paa, U16 type, U16 cnt)
{
    volatile EcuDecStat2_t* ecuDecStat2   = (EcuDecStat2_t*)0xF200C054;
    volatile EcuDecStat3_t* ecuDecStat3   = (EcuDecStat3_t*)0xF200C056;
    volatile EcuDecStat4_t* ecuDecStat4   = (EcuDecStat4_t*)0xF200C058;
    volatile EcuDecStat21_t* ecuDecStat21 = (EcuDecStat21_t*)0xF200C192;

    tFIO_JOB* pJob = NULL;
    U16  read_ptr = 0;
    U8   aucnt, cwxcnt, totalcnt, frag, eccStatus, eccValue = 0, aumask = 0;

    Aging_JobWait();

    //ECC Scan//
    pJob = FIO_PopJob(FIO_JOB_POOL_FREE);
    pJob->css     = CACHE_AUTO;
    pJob->plane   = gNfInfo.planesPerLun;
    pJob->paaCnt  = gEcuInfo.auPerPage;
    pJob->pBuffer = gCoreDbgBuf.rBuffer;
    pJob->flag    = FIO_F_NO_MKERR | FIO_F_NO_RR | FIO_F_MUTE;
    if (paa.b.block < AGING_SLC_BLOCK)
    {
        pJob->cmd  = FIO_JOB_CMD_READ_SLC;
        pJob->flag|= FIO_F_SLC;
    }
    else
    {
        pJob->cmd  = FIO_JOB_CMD_READ_FAST;
    }
    for (frag = 0; frag < pJob->paaCnt; frag++)
    {
        pJob->paa[frag].all32   = 0;
        pJob->paa[frag].b.ch    = paa.b.ch;
        pJob->paa[frag].b.ce    = paa.b.ce;
        pJob->paa[frag].b.lun   = paa.b.lun;
        pJob->paa[frag].b.plane = paa.b.plane;
        pJob->paa[frag].b.block = paa.b.block;
        pJob->paa[frag].b.page  = paa.b.page;
        pJob->paa[frag].b.frag  = frag;
    }

    Aging_JobIssue(pJob);
    Aging_JobWait();

    if (paa.b.page == smSysInfo->d.AgingEI.d.Data[cnt].Page)  ///scan page == errpage
    {
        smSysInfo->d.AgingEI.d.Data[cnt].EccStatus = pJob->status;
    }

    read_ptr = (ecuDecStat21->b.CUR_16HISTORY_READ_PTR);

    //--- Read ECC------------------------------------------------------------------------
    for (aucnt = 0, totalcnt = 0, aumask = 0; aucnt < gEcuInfo.auPerPage; aucnt++)
    {
        ecuDecStat2->b.AU_DEC_STATUS_PTR = (16 + read_ptr - (gEcuInfo.auPerPage - aucnt - 1)) & BYTE_MASK;

        for (cwxcnt = 0; cwxcnt < gEcuInfo.cwPerAu; cwxcnt++)
        {
            if (ecuDecStat3->all & 0x0040)      eccValue = 0xE0;  //Empty                                                                                                                //Uncorrectable
            else if (ecuDecStat3->all & 0x0002) eccValue = 0xF0;  //Uncorrectable
            else
            {
                ecuDecStat2->b.CW_DEC_STAT_PTR = cwxcnt;
                eccValue = ecuDecStat4->b.CWX_CORR_CNT;
            }
            if (type == PROG_WL_OPEN || type == PROG_WL_SHORT || type == READ_WL_OPEN || type == READ_WL_SHORT || type == PROG_WL_LEAKAGE)
            {
                if (eccValue == 0xE0  || eccValue == 0xF0) aumask |= (1 << totalcnt);
            }
            else
            {
                if (paa.b.page == smSysInfo->d.AgingEI.d.Data[cnt].Page)  ///scan page == errpage
                {
                    smSysInfo->d.AgingEI.d.Data[cnt].EccCw[aucnt * gEcuInfo.cwPerAu + cwxcnt] = eccValue;
                }
                else
                {
                    if (type == PROGRAM)
                    {
                        if (eccValue == 0xE0 || eccValue == 0xF0)   /// Update Layer Status
                        {
                            smSysInfo->d.AgingEI.d.Data[cnt].Page = paa.b.page;
                            smSysInfo->d.AgingEI.d.Data[cnt].EccCw[aucnt * gEcuInfo.cwPerAu + cwxcnt] = eccValue;
                            smSysInfo->d.AgingEI.d.Data[cnt].EccStatus = pJob->status;;
                        }
                    }
                }
            }
            D_PRINTF("[ECC ] CW%d|%x AUMask|%x\n",(aucnt * gEcuInfo.cwPerAu + cwxcnt), eccValue, aumask);
            totalcnt++;
        }
    }
    return aumask;
}

//-------------------------------------------------------------------
// Function     : static tERROR_AGING Aging_Dram_Check(void)
// Description  : Dram test when AGING
// Input        :
// return       : gAgingInfo.ErrorCode
//-------------------------------------------------------------------
static tERROR_AGING Aging_Dram_Check(void)
{
    smAgingDramInfo.AgingDramTag = AGING_DRAM;
    #if 0
    if (smHwInfo.d.dramDll.tag == AGING_DRAM_DLL_OVERLIMIT_TAG)
    {
        gAgingInfo.ErrorCode = ERR_DRAMDLL_OVLM;
    }
    else if (smHwInfo.d.dramCal.tag == AGING_DRAM_CAL_INVAILD_TAG)
    {
        gAgingInfo.ErrorCode = ERR_DRAMCAL_FAIL;
    }
    #endif
    if (smAgingDramInfo.AgingDramStatus == AGING_ERDM)
    {
        smAgingDramInfo.AgingDramTag = DWORD_MASK;
        if (smSysInfo->d.AgingData.d.AgingDram.DramErrorTemp.SOC > AGING_DRAM_TEMP_LIMIT)
        {
            gAgingInfo.ErrorCode                                = ERR_DRAM_HT_FAIL;
            smSysInfo->d.AgingData.d.AgingDram.DramErrorCode    = ERR_DRAM_HT_FAIL;

        }
        else
        {
            gAgingInfo.ErrorCode                                = ERR_DRAM_FAIL;
            smSysInfo->d.AgingData.d.AgingDram.DramErrorCode    = ERR_DRAM_FAIL;
        }
    }
    return  gAgingInfo.ErrorCode;
}

//-------------------------------------------------------------------
// Function     : static void Aging_Dram_Tune(void)
// Description  : Tune Dram Parameter
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void Aging_Dram_Tune(void)
{
    smAgingDramParameterTag = AGING_DRAM_PARAMETER_VAILD;

    while(smAgingDramParameterTag != AGING_DRAM_PARAMETER_FINISH);

    smSysInfo->d.AgingData.d.AgingDram.DramOpFrequency = TBG_A_FREQ_MHZ;

    SyIn_Synchronize(SI_AREA_AGING, SYSINFO_WRITE_FORCE, SI_SYNC_BY_AGING);
}

//-------------------------------------------------------------------
// Function     : static void Aging_Dram_SaveParameter(void)
// Description  : Save Dram DLL and PadCalibration Parameter
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void Aging_Dram_SaveParameter(void)
{
    HalSflash_MiscHwInfo_Flush();

    //---Backup Dram DLL to Aging---------------------------------------
    smSysInfo->d.AgingData.d.AgingDram.DramDLLTag       = smHwInfo.d.dramDll.tag;
    MEM_COPY((PVOID)&smSysInfo->d.AgingData.d.AgingDram.DramDLLAvg.DramDllPositive[0],
            (PVOID)&smHwInfo.d.dramDll.dllPositive[0], 8);
    smSysInfo->d.AgingData.d.AgingDram.DramDLLCheckSum  = smHwInfo.d.dramDll.checkSum;

    //---Backup Dram PadCalibration to Aging-----------------------------
    smSysInfo->d.AgingData.d.AgingDram.DramCALTag        = smHwInfo.d.dramCal.tag;
    smSysInfo->d.AgingData.d.AgingDram.DramphyCalLsb     = smHwInfo.d.dramCal.phyCalLsb;
    smSysInfo->d.AgingData.d.AgingDram.DramphyCalMsb     = smHwInfo.d.dramCal.phyCalMsb;
    smSysInfo->d.AgingData.d.AgingDram.DramCALcheckSum   = smHwInfo.d.dramCal.checkSum;
}

//-------------------------------------------------------------------
// Function     : static void Aging_ErrBlk_Save(tPAA errPaa, U16 phyBlk, U8 errType, U8 planeCnt)
// Description  :
// Input        : errPaa, phyBlk, errType, planeCnt
// return       : N/A
//-------------------------------------------------------------------
static void Aging_ErrBlk_Save(tPAA errPaa, U16 phyBlk, U16 errType, U8 planeCnt)
{
    U32 idx;
    U16 die, pairBlk;
    U8  planeOffet, pairPlane = 0;

    die = AGING_GET_DIE(errPaa.b.lun, errPaa.b.ce ,errPaa.b.ch);

    for (planeOffet = 0; planeOffet < gNfInfo.planesPerLun; planeOffet++)
    {
        if (planeOffet != errPaa.b.plane)
        {
            idx = AGING_GET_SBLK_IDX(planeOffet, gAgingInfo.LogBlock, die);
            pairBlk = gwAgRemapTbl[idx];
            smSysInfo->d.AgingEI.d.Data[smSysInfo->d.AgingEI.d.ErrPaaCnt].PairBlk[pairPlane] = AGING_GET_PBLK(pairBlk, planeOffet);
            pairPlane++;
        }
    }
    smSysInfo->d.AgingEI.d.Data[smSysInfo->d.AgingEI.d.ErrPaaCnt].PlaneCnt        = planeCnt;
    smSysInfo->d.AgingEI.d.Data[smSysInfo->d.AgingEI.d.ErrPaaCnt].Loop            = gAgingInfo.CurrLoopCnt;
    smSysInfo->d.AgingEI.d.Data[smSysInfo->d.AgingEI.d.ErrPaaCnt].Type            = errType;
    smSysInfo->d.AgingEI.d.Data[smSysInfo->d.AgingEI.d.ErrPaaCnt].ErrType         = errType;
    smSysInfo->d.AgingEI.d.Data[smSysInfo->d.AgingEI.d.ErrPaaCnt].Ch              = errPaa.b.ch;
    smSysInfo->d.AgingEI.d.Data[smSysInfo->d.AgingEI.d.ErrPaaCnt].Ce              = errPaa.b.ce;
    smSysInfo->d.AgingEI.d.Data[smSysInfo->d.AgingEI.d.ErrPaaCnt].Lun             = errPaa.b.lun;
    smSysInfo->d.AgingEI.d.Data[smSysInfo->d.AgingEI.d.ErrPaaCnt].Block           = phyBlk;
    smSysInfo->d.AgingEI.d.Data[smSysInfo->d.AgingEI.d.ErrPaaCnt].Page            = errPaa.b.page;
    smSysInfo->d.AgingEI.d.Data[smSysInfo->d.AgingEI.d.ErrPaaCnt].LogBlock        = gAgingInfo.LogBlock;
    smSysInfo->d.AgingEI.d.Data[smSysInfo->d.AgingEI.d.ErrPaaCnt].Temperature.SOC = (U16)HalTempSensor_GetTemperature();
    smSysInfo->d.AgingEI.d.Data[smSysInfo->d.AgingEI.d.ErrPaaCnt].Temperature.I2C = (S16)AplThermal_GetI2cTemperature();
    smSysInfo->d.AgingEI.d.ErrPaaCnt++;
    smSysInfo->d.AgingData.d.AgingErrInfo.ErrCnt++;
    gAgingError.DefectCntTotal++;

    if (errType != READ_CWO)
    {
        smSysInfo->d.AgingEI.d.ErrNonCWO   = NO_CWOFAIL;
        smSysInfo->d.AgingData.d.AgingErrInfo.ErrNonCWO = NO_CWOFAIL;
    }
}

//-------------------------------------------------------------------
// Function     : static void Aging_ErrBlk_Mark(tPAA errPaa, U16 status, U8 planeCnt)
// Description  :
// Input        : errPaa, status, planeCnt
// return       : N/A
//-------------------------------------------------------------------
static void Aging_ErrBlk_Mark(tPAA errPaa, U16 status, U8 planeCnt)
{
    U16 phyBlk, defectCnt, errType = 0;

    phyBlk = AGING_GET_PBLK(errPaa.b.block, errPaa.b.plane);

    if (status & FIO_S_UNC_ERR)
    {
        gAgingLoop.LoopErrReadCnt++;
        errType = READ_UNC;
        smSysInfo->d.AgingEI.d.ErrTotalRead++;
        smSysInfo->d.AgingData.d.AgingErrInfo.ErrTotalRead++;
        smSysInfo->d.AgingData.d.AgingPdinfo.LastFail_UNC = gAgingInfo.CurrLoopCnt;
    }
    else if (status & FIO_S_BLANK_ERR)
    {
        gAgingLoop.LoopErrReadCnt++;
        errType = READ_BLANK;
        smSysInfo->d.AgingEI.d.ErrTotalRead++;
        smSysInfo->d.AgingData.d.AgingErrInfo.ErrTotalRead++;
        smSysInfo->d.AgingData.d.AgingPdinfo.LastFail_Blank = gAgingInfo.CurrLoopCnt;
    }
    else if (status & FIO_S_OVER_LMT)
    {
        gAgingLoop.LoopErrReadCnt++;
        errType = READ_CWO;
        smSysInfo->d.AgingEI.d.ErrTotalRead++;
        smSysInfo->d.AgingData.d.AgingErrInfo.ErrTotalRead++;
        smSysInfo->d.AgingData.d.AgingPdinfo.LastFail_Blank = gAgingInfo.CurrLoopCnt;
    }
    else if (status & FIO_S_PROG_ERR)
    {
        gAgingLoop.LoopErrWriteCnt++;
        errType = PROGRAM;
        smSysInfo->d.AgingEI.d.ErrTotalWrite++;
        smSysInfo->d.AgingData.d.AgingErrInfo.ErrTotalWrite++;
        smSysInfo->d.AgingData.d.AgingPdinfo.LastFail_Prog = gAgingInfo.CurrLoopCnt;
    }
    else if (status & FIO_S_ERASE_ERR)
    {
        gAgingLoop.LoopErrEraseCnt++;
        errType = ERASE;
        smSysInfo->d.AgingEI.d.ErrTotalErase++;
        smSysInfo->d.AgingData.d.AgingErrInfo.ErrTotalErase++;
        smSysInfo->d.AgingData.d.AgingPdinfo.LastFail_Erase = gAgingInfo.CurrLoopCnt;
    }
    else
    {
        errType = status;
    }

    if (smSysInfo->d.AgingEI.d.ErrPaaCnt < AGING_ERRDATA_SIZE) Aging_ErrBlk_Save(errPaa, phyBlk, errType, planeCnt);

    defectCnt = smSysInfo->d.PlistCnt.d.PList2Cnt.wCount[errPaa.b.ch][errPaa.b.ce][errPaa.b.lun];
    if (defectCnt < SI_PLIST_MAX_COUNT)
	{
		smSysInfo->d.PList2.wList[errPaa.b.ch][errPaa.b.ce][errPaa.b.lun][defectCnt] = phyBlk;
    }
	defectCnt++;
    smSysInfo->d.PlistCnt.d.PList2Cnt.wCount[errPaa.b.ch][errPaa.b.ce][errPaa.b.lun] = defectCnt;
    gAgingLoop.LoopDefectCnt++;

    if (defectCnt > (MAX_DEFECT_ALLOW * gNfInfo.oriPlanesPerLun))
    {
        gAgingInfo.ErrorCode = ERR_DEFECT_BY_CH;
    }

    #if (AGING_FINAL_LOOP_ERR == ENABLE)
    if  (gAgingInfo.CurrLoopCnt > (smSysInfo->d.AgingInfo.d.AgingLoop.LoopTarget)-5)
    {
        gAgingInfo.ErrorCode = ERR_FINALSTAGE_ERROR;
    }
    #endif
}

//-------------------------------------------------------------------
// Function     : static void Aging_ErrBlk_Record(tPAA errPaa, U16 cmd, U16 status, U8 planeCnt)
// Description  :
// Input        : errPAa, cmd, status, planeCnt
// return       : N/A
//-------------------------------------------------------------------
static void Aging_ErrBlk_Record(tPAA errPaa, U16 cmd, U16 status, U8 planeCnt)
{
    U32 entry, idx;
    U16 cnt;

    entry = AGING_GET_PNDIE(errPaa.b.ch, errPaa.b.ce, errPaa.b.lun, errPaa.b.plane);

    for (cnt = 0; cnt < gAgingError.ErrPaaCnt[entry]; cnt++)
    {
        idx = AGING_GET_ERR_IDX(entry, cnt);

        if ((gAgingError.ErrPaa[idx].b.ch    == errPaa.b.ch)    &&
            (gAgingError.ErrPaa[idx].b.ce    == errPaa.b.ce)    &&
            (gAgingError.ErrPaa[idx].b.lun   == errPaa.b.lun)   &&
            (gAgingError.ErrPaa[idx].b.plane == errPaa.b.plane) &&
            (gAgingError.ErrPaa[idx].b.block == errPaa.b.block))
        {
            return;
        }
    }

    idx = AGING_GET_ERR_IDX(entry, gAgingError.ErrPaaCnt[entry]);
    gAgingError.ErrPaa[idx].all32 = errPaa.all32;
    gAgingError.ErrPaaCnt[entry]++;

    D_PRINTF_AG("'Record Err' CMD|%d PAA[CH%d|CE%d|LN%d|PN%d|BK0x%3x]\n", cmd, errPaa.b.ch, errPaa.b.ce, errPaa.b.lun, errPaa.b.plane, errPaa.b.block);

    if  ((gAgingInfo.LogBlock < gNfInfo.blocksPerPlane) &&
         (errPaa.b.ch < gNfInfo.numChannels) &&
         (errPaa.b.ce < gNfInfo.devicesPerChannel) &&
         (errPaa.b.lun < gNfInfo.lunsPerDevice) &&
         (errPaa.b.plane < gNfInfo.planesPerLun)&&
         (errPaa.b.block < gNfInfo.blocksPerPlane) &&
         (errPaa.b.page < gNfInfo.pagesPerBlock))
    {
        Aging_ErrBlk_Mark(errPaa, status, planeCnt);
    }
    else
    {
        D_PRINTF_AG("'UnKnown Err' S|0x%1x CMD|%d PAA[CH%d|CE%d|LN%d|PN%d|BK0x%3x|LogBlock0x%3x|Page0x%3x]\n",
                     status, cmd, errPaa.b.ch, errPaa.b.ce, errPaa.b.lun, errPaa.b.plane, errPaa.b.block, gAgingInfo.LogBlock, errPaa.b.page);
        gAgingInfo.ErrorCode = ERR_UNKNOWN_ERROR;
    }
}

//-------------------------------------------------------------------
// Function     : static void Aging_Util_RemapBlk(U16 die, U16 block, U8 plane)
// Description  : remapBlk
// Input        : die, block, plane
// return       : N/A
//-------------------------------------------------------------------
static void Aging_Util_RemapBlk(U16 die, U16 block, U8 plane)
{
    U32 idx;
    U16 freeidx;

    freeidx = AGING_GET_FREE_IDX(plane, die);
    while (gwAgFreeIdx[freeidx] > block)
    {
        idx = AGING_GET_SBLK_IDX(plane, gwAgFreeIdx[freeidx], die);
        if (gwAgRemapTbl[idx]!= AGING_BAD_BLOCK)
        {
            ///Swap Remap Block to Bad Block
            idx = AGING_GET_SBLK_IDX(plane, block, die);
            gwAgRemapTbl[idx] = gwAgFreeIdx[freeidx];
            *(((U16*)smSysInfo->d.FtlRmpData.d.SBlkRmpInfo) + idx) = gwAgRemapTbl[idx];

            ///Swap Bad Block to Remap Block
            idx = AGING_GET_SBLK_IDX(plane, gwAgFreeIdx[freeidx], die);
            gwAgRemapTbl[idx] = AGING_BAD_BLOCK;
            *(((U16*)smSysInfo->d.FtlRmpData.d.SBlkRmpInfo) + idx) = block;
            gwAgFreeIdx[freeidx]--;
            break;
        }
        gwAgFreeIdx[freeidx]--;
    }
}

//-------------------------------------------------------------------
// Function     : void Aging_Util_BubbleSort(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void Aging_Util_BubbleSort(void)
{
    U16 die, cnt, temp, x;
    U8  ch, ce, lun;

    for (die = 0; die < gNfInfo.totalLun; die++)
    {
        ch = die & AGING_CH_MASK;
        ce = (die >> gNfInfo.bitsCh) & AGING_CE_MASK;
        lun = (die >> gNfInfo.bitsChDev) & AGING_LUN_MASK;
        if (smSysInfo->d.PlistCnt.d.PList2Cnt.wCount[ch][ce][lun] > 0)
        {
            for (cnt = 0; cnt < (smSysInfo->d.PlistCnt.d.PList2Cnt.wCount[ch][ce][lun]); cnt++)
            {
                for (x=(cnt+1); x < smSysInfo->d.PlistCnt.d.PList2Cnt.wCount[ch][ce][lun]; x++)
                {
                    if (smSysInfo->d.PList2.wList[ch][ce][lun][cnt]>smSysInfo->d.PList2.wList[ch][ce][lun][x])
                    {  //swap
                        temp = smSysInfo->d.PList2.wList[ch][ce][lun][x];
                        smSysInfo->d.PList2.wList[ch][ce][lun][x] = smSysInfo->d.PList2.wList[ch][ce][lun][cnt];
                        smSysInfo->d.PList2.wList[ch][ce][lun][cnt] = temp;
                    }
                }
            }
        }
    }
}

//-------------------------------------------------------------------
// Function     : void Aging_Util_ReBuildTable(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void Aging_Util_ReBuildTable(void)
{
    U32 idx;
    U16 cnt, freeidx, phyBlk, block, die;
    U8  ch, ce, lun, plane;

    MEM_CLR(gwAgRemapTbl, sizeof(U16) * gNfInfo.blocksPerPlane * gNfInfo.planesPerLun * gNfInfo.totalLun);

    MEM_CLR(gwAgFreeIdx, sizeof(U16) * gNfInfo.planesPerLun * gNfInfo.totalLun);

    //--- Build Free Idx Table------------------------------------------------------------------------
    for (die = 0; die < gNfInfo.totalLun; die++)
    {
        for (plane = 0; plane < gNfInfo.planesPerLun; plane++)
        {
            freeidx = AGING_GET_FREE_IDX(plane, die);
            gwAgFreeIdx[freeidx] = (gNfInfo.blocksPerPlane-1);
        }
    }

    //--- Build Blk Table------------------------------------------------------------------------
    for (die = 0; die < gNfInfo.totalLun; die++)
    {
        for (plane = 0; plane < gNfInfo.planesPerLun; plane++)
        {
            for (block = 0; block < gNfInfo.blocksPerPlane; block++)
            {
                idx = AGING_GET_SBLK_IDX(plane, block, die);
                gwAgRemapTbl[idx] = block;
            }
        }
    }
    /// Copy AgRemapTbl to Sysinfo
    MEM_COPY(smSysInfo->d.FtlRmpData.d.SBlkRmpInfo, (U8*)gwAgRemapTbl, sizeof(U16) * gNfInfo.blocksPerLun * gNfInfo.totalLun);
    //--- Mark Bad Block 0xFFFF------------------------------------------------------------------------
    for (die = 0; die < gNfInfo.totalLun; die++)
    {
        ch  = die & AGING_CH_MASK;
        ce  = (die >> gNfInfo.bitsCh) & AGING_CE_MASK;
        lun = (die >> gNfInfo.bitsChDev) & AGING_LUN_MASK;

        for (cnt = 0; cnt < smSysInfo->d.PlistCnt.d.PList2Cnt.wCount[ch][ce][lun]; cnt++)
        {
            phyBlk = smSysInfo->d.PList2.wList[ch][ce][lun][cnt];
            if (phyBlk < AGING_SYSINFO_CNT * gNfInfo.planesPerLun) continue;

            plane = phyBlk & AGING_PLANE_MASK;
            block = phyBlk >> gNfInfo.bitsPlane;
            idx = AGING_GET_SBLK_IDX(plane, block, die);
            gwAgRemapTbl[idx] = AGING_BAD_BLOCK;
        }
    }

    //--- Remap Bad Block------------------------------------------------------------------------
    for ( die = 0; die < gNfInfo.totalLun; die++)
    {
        ch  = die & AGING_CH_MASK;
        ce  = (die >> gNfInfo.bitsCh) & AGING_CE_MASK;
        lun = (die >> gNfInfo.bitsChDev) & AGING_LUN_MASK;

        for (cnt = 0; cnt < smSysInfo->d.PlistCnt.d.PList2Cnt.wCount[ch][ce][lun]; cnt++)
        {
            phyBlk = smSysInfo->d.PList2.wList[ch][ce][lun][cnt];
            if (phyBlk < AGING_SYSINFO_CNT * gNfInfo.planesPerLun) continue;

            plane = phyBlk & AGING_PLANE_MASK;
            block = phyBlk >> gNfInfo.bitsPlane;

            if ((block < gNfInfo.blocksPerPlane) && (plane < gNfInfo.planesPerLun))
            {
                Aging_Util_RemapBlk(die, block, plane);
            }
            else
            {
                gAgingInfo.ErrorCode = ERR_UNKNOWN_ERROR;
            }
        }

        #if 0
        D_PRINTF("Show Block MAP\n");
        for (block = 0; block < gNfInfo.blocksPerPlane; block++)
        {
            for (plane = 0; plane < gNfInfo.planesPerLun; plane++)
            {
                idx = AGING_GET_SBLK_IDX(plane, block, die);
                D_PRINTF("[%2d][%2d][%2d][%2d][%4d]:%6d\t",ch, ce, lun, plane, block, gwAgRemapTbl[idx]);
            }
            D_PRINTF("\n");
        }
        #endif
    }
    //--- Find Min Block Threshold------------------------------------------------------------------------
    gAgingInfo.MinFreeIdx = WORD_MASK;
    for (die = 0; die < gNfInfo.totalLun; die++)
    {
        for (plane = 0; plane < gNfInfo.planesPerLun; plane++)
        {
            freeidx = AGING_GET_FREE_IDX(plane, die);
            gAgingInfo.MinFreeIdx = MIN(gAgingInfo.MinFreeIdx, gwAgFreeIdx[freeidx]);
        }
    }
    D_PRINTF_AG("MinFreeIdx|0x%4x\n",gAgingInfo.MinFreeIdx);
}

//-------------------------------------------------------------------
// Function     : static void Aging_MP_CalcPlistCnt(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void Aging_MP_CalcPlistCnt(void)
{
    U32 maxT, totalPreDefect, totalDefect;
    U16 cnt ,die ,block, maxDefect, totalMaxT;
    U8  ch ,ce ,lun, plane;

    smSysInfo->d.AgingData.d.AgingErrInfo.ErrCntPlist1 = smSysInfo->d.AgingData.d.AgingErrInfo.ErrCntPlist2 = 0;
    maxT = totalPreDefect = totalDefect = maxDefect = 0;

    MEM_CLR(&smSysInfo->d.AgingData.d.AgingSbkCnt, sizeof(tAG_SBKCNT));
    smSysInfo->d.AgingData.d.AgingSbkCnt.SupperBlkTag  = AGING_SBLK;

    for (die = 0; die < gNfInfo.totalLun; die++)
    {
        ch  = die & AGING_CH_MASK;
        ce  = (die >> gNfInfo.bitsCh) & AGING_CE_MASK;
        lun = (die >> gNfInfo.bitsChDev) & AGING_LUN_MASK;

        if (smSysInfo->d.PlistCnt.d.PList2Cnt.wCount[ch][ce][lun] > (MAX_DEFECT_ALLOW * gNfInfo.oriPlanesPerLun))
        {
            gAgingInfo.ErrorCode = ERR_DEFECT_BY_CH;
        }

        smSysInfo->d.AgingData.d.AgingErrInfo.ErrCntPlist1 += smSysInfo->d.PlistCnt.d.PList1Cnt.wCount[ch][ce][lun];
        smSysInfo->d.AgingData.d.AgingErrInfo.ErrCntPlist2 += smSysInfo->d.PlistCnt.d.PList2Cnt.wCount[ch][ce][lun];

        totalPreDefect += smSysInfo->d.PlistCnt.d.PList1Cnt.wCount[ch][ce][lun];
        totalDefect    += smSysInfo->d.PlistCnt.d.PList2Cnt.wCount[ch][ce][lun];

        if (maxDefect < smSysInfo->d.PlistCnt.d.PList2Cnt.wCount[ch][ce][lun])
        {
            maxDefect  = smSysInfo->d.PlistCnt.d.PList2Cnt.wCount[ch][ce][lun];
        }

        if (smSysInfo->d.PlistCnt.d.PList2Cnt.wCount[ch][ce][lun] > 0)
        {
            for (cnt = 0; cnt < smSysInfo->d.PlistCnt.d.PList2Cnt.wCount[ch][ce][lun]; cnt++, maxT = 0)
            {
                block = smSysInfo->d.PList2.wList[ch][ce][lun][cnt];
                plane = block % gNfInfo.oriPlanesPerLun;
                smSysInfo->d.AgingData.d.AgingSbkCnt.DefectBlkCntT[plane][lun][ce][ch] += 1;
                if ((block / gNfInfo.oriPlanesPerLun) <= AGING_USER_BLK)
                {
                    smSysInfo->d.AgingData.d.AgingSbkCnt.DefectBlkCntU[plane][lun][ce][ch] += 1;
                }
                if (maxT <= smSysInfo->d.AgingData.d.AgingSbkCnt.DefectBlkCntT[plane][lun][ce][ch])
                {
                    maxT = smSysInfo->d.AgingData.d.AgingSbkCnt.DefectBlkCntT[plane][lun][ce][ch];

                    if (maxT >= totalMaxT)
                    {
                        totalMaxT = maxT;
                    }
                }
                if  (((gNfInfo.blocksPerLun / gNfInfo.oriPlanesPerLun)- maxT) < AGING_SUP_BLK)
                {
                    smSysInfo->d.AgingData.d.AgingSbkCnt.SbkDieMap[ch] |= BIT(ce);
                    smSysInfo->d.AgingData.d.AgingSbkCnt.SbkErrorCode   = ERR_SUPBLK_FAIL;

                    if (gAgingInfo.ErrorCode == ERR_NO_ERROR)
                    {
                        gAgingInfo.ErrorCode = ERR_SUPBLK_FAIL;
                    }
                }
            }
        }
    }

    smSysInfo->d.AgingData.d.AgingSbkCnt.SbkOpBlk               = (gNfInfo.blocksPerPlane - totalMaxT);
    smSysInfo->d.AgingData.d.AgingPdinfo.Avg_PreliminaryDefect  = (U16)(totalPreDefect / gNfInfo.totalLun);
    smSysInfo->d.AgingData.d.AgingPdinfo.Avg_TotalDefect        = (U16)(totalDefect    / gNfInfo.totalLun);
    smSysInfo->d.AgingData.d.AgingPdinfo.Avg_GrownDefect        = (U16)((totalDefect - totalPreDefect) / gNfInfo.totalLun);
    smSysInfo->d.AgingData.d.AgingPdinfo.Max_Defect             = maxDefect;

    if (smSysInfo->d.AgingData.d.AgingSbkCnt.SbkOpBlk > AGING_HOST_BLK)
        smSysInfo->d.AgingData.d.AgingSbkCnt.SbkOpRatio             = ((((gNfInfo.blocksPerPlane - totalMaxT) - AGING_HOST_BLK) * 100) / AGING_HOST_BLK);
    else
        smSysInfo->d.AgingData.d.AgingSbkCnt.SbkOpRatio             = 0;

}

//-------------------------------------------------------------------
// Function     : static void AGING_CalcFailDieCnt(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void Aging_MP_CalcFailDieCnt(void)
{
    U16 cnt, die;
    U8  ch, ce, lun;

    MEM_CLR(gbAgflagTbl, sizeof(U8) * gNfInfo.totalLun);

    for (cnt = 0; cnt < smSysInfo->d.AgingEI.d.ErrPaaCnt; cnt++)
    {
        ch  = smSysInfo->d.AgingEI.d.Data[cnt].Ch;
        ce  = smSysInfo->d.AgingEI.d.Data[cnt].Ce;
        lun = smSysInfo->d.AgingEI.d.Data[cnt].Lun;
        die = AGING_GET_DIE(lun, ce, ch);

        gbAgflagTbl[die]++;
    }

    for (die = 0; die < gNfInfo.totalLun; die++)
    {
        if (gbAgflagTbl[die])
        {
            smSysInfo->d.AgingData.d.AgingErrInfo.ErrDieCnt++;
        }
    }
}

//-------------------------------------------------------------------
// Function     : static void AGING_MP_SaveInfo(void)
// Description  : MP Info Description
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void Aging_MP_SaveInfo(void)
{
    volatile EcuCfg5_t_CPU2* EcuCfg5 = (EcuCfg5_t_CPU2*)0xF200C028;  // CAREFUL if ECU register is different, this has to change
    smSysInfo->d.MPInfo.d.AgingSignature            = SI_TAG_AGING;
    smSysInfo->d.MPInfo.d.AgingMode                 = smSysInfo->d.AgingData.d.Aging_Mode;
    smSysInfo->d.MPInfo.d.LoopCurrent               = smSysInfo->d.AgingData.d.AgingLoop.LoopCurrent;
    smSysInfo->d.MPInfo.d.LoopTarget                = smSysInfo->d.AgingData.d.AgingLoop.LoopTarget;
    smSysInfo->d.MPInfo.d.EccEnabled                = ENABLE;
    smSysInfo->d.MPInfo.d.EccThreshold              = EcuCfg5->b.THRSHD;
    smSysInfo->d.MPInfo.d.ErrorCode                 = smSysInfo->d.AgingData.d.Aging_ErrorCode;
    smSysInfo->d.MPInfo.d.ElapsedTime               = smSysInfo->d.AgingData.d.Aging_ElapsedTime;
    smSysInfo->d.MPInfo.d.NumberOfChannel           = gNfInfo.numChannels;
    smSysInfo->d.MPInfo.d.NumberOfDevice            = gNfInfo.totalLun;
    smSysInfo->d.MPInfo.d.NumberOfBlock             = gNfInfo.blocksPerLun;
    smSysInfo->d.MPInfo.d.NumberOfPage              = gNfInfo.pagesPerBlock;
    smSysInfo->d.MPInfo.d.TotalLoopWrite            = gAgingInfo.TotalLoopWrite;
    smSysInfo->d.MPInfo.d.TotalLoopRead             = gAgingInfo.TotalLoopRead;
    smSysInfo->d.MPInfo.d.TotalLoopErase            = gAgingInfo.TotalLoopErase;
    smSysInfo->d.MPInfo.d.TotalErrorWrite           = smSysInfo->d.AgingData.d.AgingErrInfo.ErrTotalWrite;
    smSysInfo->d.MPInfo.d.TotalErrorRead            = smSysInfo->d.AgingData.d.AgingErrInfo.ErrTotalRead;
    smSysInfo->d.MPInfo.d.TotalErrorErase           = smSysInfo->d.AgingData.d.AgingErrInfo.ErrTotalErase;
    smSysInfo->d.MPInfo.d.DefectCountPreliminary    = smSysInfo->d.AgingData.d.AgingErrInfo.ErrCntPlist1;
    smSysInfo->d.MPInfo.d.DefectCountTotal          = smSysInfo->d.AgingData.d.AgingErrInfo.ErrCntPlist2;
    smSysInfo->d.MPInfo.d.DefectCountGrown          = smSysInfo->d.AgingData.d.AgingErrInfo.ErrCntPlist2 - smSysInfo->d.AgingData.d.AgingErrInfo.ErrCntPlist1;
    smSysInfo->d.MPInfo.d.ReAgingCount              = smSysInfo->d.AgingData.d.AgingRerun.ReRunCount;
    smSysInfo->d.MPInfo.d.VersionNo                 = smSysInfo->d.AgingData.d.Aging_VerNo;
}

//-------------------------------------------------------------------
// Function     : static void AGING_SysInfoUpdate(void)
// Description  : uART Aging
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void Aging_SysInfoUpdate(U16 Loop,U8 Mode)
{
    smSysInfo->d.Header.d.SubHeader.d.Aging.d.dwValidTag       = SI_TAG_AGING;
    smSysInfo->d.Header.d.SubHeader.d.MPInfo.d.dwValidTag      = SI_TAG_MPINFO;
    smSysInfo->d.AgingData.d.Aging_ErrorCode                   = ERR_NO_EXE;

    if (Mode == 0)
    {
        MEM_CLR(smSysInfo->d.AgingData.all, sizeof(tSI_PAYLOAD_AGING));
        MEM_CLR(smSysInfo->d.AgingEI.all, sizeof(tSI_PAYLOAD_AGERRINFO));
        MEM_CLR(&smSysInfo->d.PlistCnt.d.PList2Cnt, sizeof(tSI_PLIST_CNT));
        smSysInfo->d.AgingData.d.Aging_Signature                   = AGING_SIGN;
        smSysInfo->d.Header.d.SubHeader.d.PList2.d.dwValidTag      = 0x00;
        smSysInfo->d.AgingData.d.AgingLoop.LoopTarget              = Loop;
        smSysInfo->d.AgingData.d.AgingRerun.ReRunCount             = 0x00;
        smSysInfo->d.MPInfo.d.AgingLED                             = 0x00;
    }
    else if (Mode == 1)
    {
        smSysInfo->d.AgingData.d.Aging_Signature                   = AGING_SIGN;
        smSysInfo->d.AgingData.d.AgingLoop.LoopTarget              = Loop;
        smSysInfo->d.AgingData.d.AgingRerun.ReRunCount             = 0x00;
    }

    D_PRINTF_AG("\nLoop|%d Mode|%d\n",smSysInfo->d.AgingData.d.AgingLoop.LoopTarget, Mode);

    SyIn_Synchronize(SI_AREA_AGING | SI_AREA_MPINFO | SI_AREA_AGINGEI, SYSINFO_WRITE_FORCE, SI_SYNC_BY_AGING);
}

#if (AGING_LED == ENABLE)
//-------------------------------------------------------------------
// Function     : static void Aging_Led_Manage(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static void Aging_Led_Manage(void)
{
    if(smSysInfo->d.MPInfo.d.AgingLED == AGING_LED_FLAG)
    {
        rApb.uartPinCtrl.b.UART1_DIS        = 1;
        rApb.uartPinCtrl.b.UART_CTRL_FW_RX1 = 1;
        rApb.uartPinCtrl.b.UART_CTRL_FW_TX1 = 1;
        rApb.uartPinCtrl.b.UART1_TX_OE      = 1;
        rApb.uartPinCtrl.b.UART1_RX_OE      = 1;
        rApb.uartPinCtrl.b.UART1_RX_OUT     = 0;
    }
}

//-------------------------------------------------------------------
// Function     : static void Aging_Led_DisplayErrorCode(U8 ErrorCode)
// Description  :
// Input        : bErrorCode
// return       : N/A
//-------------------------------------------------------------------
static void Aging_Led_DisplayErrorCode(U8 ErrorCode)
{
    D_PRINTF_AG("Show Led\n");

    HalTimer_DelayMsec(1000);

    rApb.uartPinCtrl.b.UART1_DIS        = 1;
    rApb.uartPinCtrl.b.UART_CTRL_FW_RX1 = 1;
    rApb.uartPinCtrl.b.UART_CTRL_FW_TX1 = 1;
    rApb.uartPinCtrl.b.UART1_TX_OE      = 1;
    rApb.uartPinCtrl.b.UART1_RX_OE      = 1;
    rApb.uartPinCtrl.b.UART1_RX_OUT     = 0;

    switch (ErrorCode)
    {
        case ERR_NO_ERROR: //0-0
            Aging_Led_BlinkPass(0,0);    break;
        case ERR_SCAN_DEFECT:
            Aging_Led_BlinkFail(1,1,mFALSE,mFALSE);    break;
        case ERR_FLASH_ID:
            Aging_Led_BlinkFail(1,2,mFALSE,mFALSE);    break;
        case ERR_SOC_TYPE:
            Aging_Led_BlinkFail(1,4,mFALSE,mFALSE);    break;
        case ERR_ERASE_FAILED:
            Aging_Led_BlinkFail(2,1,mFALSE,mFALSE);    break;
        case ERR_WRITE_FAILED:
            Aging_Led_BlinkFail(2,2,mFALSE,mFALSE);    break;
        case ERR_READ_FAILED:
            Aging_Led_BlinkFail(3,1,mFALSE,mFALSE);    break;
        case ERR_READ_EEPROM_FAILED:
            Aging_Led_BlinkFail(3,2,mFALSE,mFALSE);    break;
        case ERR_CHECKSUM_INVALID:
            Aging_Led_BlinkFail(3,3,mFALSE,mFALSE);    break;
        case ERR_BF2_ERROR:
            Aging_Led_BlinkFail(3,4,mFALSE,mFALSE);    break;
        case ERR_FINALSTAGE_ERROR:
            Aging_Led_BlinkFail(4,4,mFALSE,mFALSE);    break;
        case ERR_AGING_SETTING_ERROR:
            Aging_Led_BlinkFail(5,2,mFALSE,mFALSE);    break;
        case ERR_MEMORY_ERROR:
            Aging_Led_BlinkFail(5,3,mFALSE,mFALSE);    break;
        case ERR_INITIAL_E2_FAIL:
            Aging_Led_BlinkFail(5,4,mFALSE,mFALSE);    break;
        case ERR_DEFECT_ERROR:
            Aging_Led_BlinkFail(5,5,mFALSE,mFALSE);    break;
        case ERR_1st_PF_ECC_OK:
            Aging_Led_BlinkFail(6,1,mFALSE,mFALSE);    break;
        case ERR_2nd_PF_ECC_OK:
            Aging_Led_BlinkFail(6,2,mFALSE,mFALSE);    break;
        case ERR_MAX_RERUN:
            Aging_Led_BlinkFail(7,1,mFALSE,mFALSE);    break;
        case ERR_DEFECT_BY_CH:
            Aging_Led_BlinkFail(8,1,mFALSE,mFALSE);    break;
        case ERR_DRAM_FAIL:
            Aging_Led_BlinkFail(9,2,mFALSE,mFALSE);    break;
        case ERR_SUPBLK_FAIL:
            Aging_Led_BlinkFail(9,3,mFALSE,mFALSE);    break;
        case ERR_DATACOMPARE_FAIL:
            Aging_Led_BlinkFail(9,4,mFALSE,mFALSE);    break;
        case ERR_DRAMCAL_FAIL:
            Aging_Led_BlinkFail(9,5,mFALSE,mFALSE);    break;
        case ERR_DRAMALL_FAIL:
            Aging_Led_BlinkFail(9,6,mFALSE,mFALSE);    break;
        case ERR_ORISUPBLK_FAIL:
            Aging_Led_BlinkFail(9,7,mFALSE,mFALSE);    break;
        case ERR_SIDATA_FAIL:
            Aging_Led_BlinkFail(9,8,mFALSE,mFALSE);    break;
        case ERR_DRAMDLL_OVLM:
            Aging_Led_BlinkFail(9,9,mFALSE,mFALSE);    break;
        case ERR_PLIST1_FAIL:
            Aging_Led_BlinkFail(9,0xA,mFALSE,mFALSE);  break;
        case ERR_DRAM_HT_FAIL:
            Aging_Led_BlinkFail(9,0xB,mFALSE,mFALSE);  break;
        case ERR_UNKNOWN_ERROR:
        default:
            Aging_Led_BlinkFail(5,1,mFALSE,mFALSE);    break;
    }
}

//-------------------------------------------------------------------
// Function     : static void Aging_Led_BlinkPass(U8 DoPowerCycle,U8 PowerCyclesFinished)
// Description  :
// Input        : bDoPowerCycle, iPowerCyclesFinished
// return       : N/A
//-------------------------------------------------------------------
static void Aging_Led_BlinkPass(U8 DoPowerCycle,U8 PowerCyclesFinished)
{
    U8 i;
    while (1)
    {
        for (i = 0; i < 20; i++)
        {
            rApb.uartPinCtrl.b.UART1_TX_OUT = 1;
            HalTimer_DelayMsec(50);
            rApb.uartPinCtrl.b.UART1_TX_OUT = 0;
            HalTimer_DelayMsec(50);
        }
    }
}

//-------------------------------------------------------------------
// Function     : static void Aging_Led_BlinkFail(U8 ErrorHi, U8 ErrorLo, U8 LongHi, U8 LongLo)
// Description  :
// Input        : bErrorHi, bErrorLo, bLongHi, bLongLo
// return       : N/A
//-------------------------------------------------------------------
static void Aging_Led_BlinkFail(U8 ErrorHi, U8 ErrorLo, U8 LongHi, U8 LongLo)
{
    U8 i;
    while (1)
    {
        rApb.uartPinCtrl.b.UART1_RX_OUT = 0;

        for (i = 0 ; i < ErrorHi ; i++)
        {
            rApb.uartPinCtrl.b.UART1_TX_OUT = 1;

            HalTimer_DelayMsec(LongHi ? 1000 : 200);

            rApb.uartPinCtrl.b.UART1_TX_OUT = 0;

            HalTimer_DelayMsec(200);
        }

        HalTimer_DelayMsec(1000);

        for (i = 0 ; i < ErrorLo ; i++)
        {
            rApb.uartPinCtrl.b.UART1_TX_OUT = 1;

            HalTimer_DelayMsec(LongLo ? 1000 : 200);

            rApb.uartPinCtrl.b.UART1_TX_OUT = 0;

            HalTimer_DelayMsec(200);
        }
        HalTimer_DelayMsec(3000);
    }
}
#endif
#endif

#include "NormSection.h"
//-------------------------------------------------------------------
// Function     : static void Aging_DumpInfo(U32 DisCnt, U32 param)
// Description  : Dump Aging and Plist2 Info
// Input        : DisCnt, param
// return       : N/A
//-------------------------------------------------------------------
static void Aging_DumpInfo(U32 DisCnt, U32 param)
{
    U16 die = 0, loop = 0, cnt = 0, dump = 0;

    switch (param)
    {
        case 0: //Dump Aging
            D_PRINTF("AgingInfo\n");
            Debug_DumpMemU8((U32)&(smSysInfo->d.AgingData), (U32) sizeof(tSI_PAYLOAD_AGING));
            break;
        case 1: //Dump AgingErrInfo
            #if CHK_FW_TYPE(FW_AGING)
            D_PRINTF("AgingErrInfo_Data\n");
            Debug_DumpMemU8((U32)&smSysInfo->d.AgingEI, (U32)sizeof(tSI_PAYLOAD_AGERRINFO));
            #endif
            break;
        case 2: //Dump BusyTime
            D_PRINTF("AgingBusyTime\n");
            for (die = 0 ; die < gNfInfo.totalLun; die++)
            {
                D_PRINTF("DIE[%2d] = Erase[%4d]Read[%4d]Program[%4d] \n", die, smSysInfo->d.AgingData.d.AgingNandBusyTime.Erase[die], smSysInfo->d.AgingData.d.AgingNandBusyTime.Read[die], smSysInfo->d.AgingData.d.AgingNandBusyTime.Program[die]);
            }
            D_PRINTF("AVG_E[%4d] MAX_E[%4d] AVG_P[%4d] MAX_P[%4d] AVG_R[%4d] MAX_R[%4d] \n",
                        smSysInfo->d.AgingData.d.AgingPdinfo.TeAvg, smSysInfo->d.AgingData.d.AgingPdinfo.TeMax,
                        smSysInfo->d.AgingData.d.AgingPdinfo.TpAvg, smSysInfo->d.AgingData.d.AgingPdinfo.TpMax,
                        smSysInfo->d.AgingData.d.AgingPdinfo.TrAvg, smSysInfo->d.AgingData.d.AgingPdinfo.TrMax);
            break;
        case 3: //Dump Temperature
            D_PRINTF("AgingNandTemperature\n");
            for (loop = 0; loop < (smSysInfo->d.AgingData.d.AgingLoop.LoopCurrent + 1); loop++)
            {
                D_PRINTF("Loop[%d] E_SOC[%3d] E_I2C[%3d] W_SOC[%3d] W_I2C[%3d] R_SOC[%3d] R_I2C[%3d]\n",
                            loop,
                            smSysInfo->d.AgingData.d.AgingTemperature.TemperatureErase[loop].SOC,
                            smSysInfo->d.AgingData.d.AgingTemperature.TemperatureErase[loop].I2C,
                            smSysInfo->d.AgingData.d.AgingTemperature.TemperatureWrite[loop].SOC,
                            smSysInfo->d.AgingData.d.AgingTemperature.TemperatureWrite[loop].I2C,
                            smSysInfo->d.AgingData.d.AgingTemperature.TemperatureRead[loop].SOC,
                            smSysInfo->d.AgingData.d.AgingTemperature.TemperatureRead[loop].I2C);
            }
            D_PRINTF("E_SOC_MAX[%3d] E_SOC_AVG[%3d] E_SOC_MIN[%3d]\nW_SOC_MAX[%3d] W_SOC_AVG[%3d] W_SOC_MIN[%3d]\nR_SOC_MAX[%3d] R_SOC_AVG[%3d] R_SOC_MIN[%3d]\n",
                        smSysInfo->d.AgingData.d.AgingTemperature.TemperatureErase_Max.SOC,
                        smSysInfo->d.AgingData.d.AgingTemperature.TemperatureErase_Avg.SOC,
                        smSysInfo->d.AgingData.d.AgingTemperature.TemperatureErase_Min.SOC,
                        smSysInfo->d.AgingData.d.AgingTemperature.TemperatureWrite_Max.SOC,
                        smSysInfo->d.AgingData.d.AgingTemperature.TemperatureWrite_Avg.SOC,
                        smSysInfo->d.AgingData.d.AgingTemperature.TemperatureWrite_Min.SOC,
                        smSysInfo->d.AgingData.d.AgingTemperature.TemperatureRead_Max.SOC,
                        smSysInfo->d.AgingData.d.AgingTemperature.TemperatureRead_Avg.SOC,
                        smSysInfo->d.AgingData.d.AgingTemperature.TemperatureRead_Min.SOC);
            D_PRINTF("E_I2C_MAX[%3d] E_I2C_AVG[%3d] E_I2C_MIN[%3d]\nW_I2C_MAX[%3d] W_I2C_AVG[%3d] W_I2C_MIN[%3d]\nR_I2C_MAX[%3d] R_I2C_AVG[%3d] R_I2C_MIN[%3d]\n",
                        smSysInfo->d.AgingData.d.AgingTemperature.TemperatureErase_Max.I2C,
                        smSysInfo->d.AgingData.d.AgingTemperature.TemperatureErase_Avg.I2C,
                        smSysInfo->d.AgingData.d.AgingTemperature.TemperatureErase_Min.I2C,
                        smSysInfo->d.AgingData.d.AgingTemperature.TemperatureWrite_Max.I2C,
                        smSysInfo->d.AgingData.d.AgingTemperature.TemperatureWrite_Avg.I2C,
                        smSysInfo->d.AgingData.d.AgingTemperature.TemperatureWrite_Min.I2C,
                        smSysInfo->d.AgingData.d.AgingTemperature.TemperatureRead_Max.I2C,
                        smSysInfo->d.AgingData.d.AgingTemperature.TemperatureRead_Avg.I2C,
                        smSysInfo->d.AgingData.d.AgingTemperature.TemperatureRead_Min.I2C);
            break;
        case 4: //Dump Plist1 & Plist2
            D_PRINTF("Plist1\n");
            Debug_DumpMemU8((U32)&(smSysInfo->d.PList1), (U32) sizeof(tSI_PAYLOAD_PLIST));
            D_PRINTF("Plist2\n");
            Debug_DumpMemU8((U32)&(smSysInfo->d.PList2), (U32) sizeof(tSI_PAYLOAD_PLIST));
            break;
        case 5: //Dump MPInfo
            D_PRINTF("MPInfo\n");
            Debug_DumpMemU8((U32)&(smSysInfo->d.MPInfo), (U32) sizeof(tSI_PAYLOAD_MPINFO));
            break;
        case 6: //Dump DieCnt
            D_PRINTF("ErrDie:%d TotalDie:%d \n", smSysInfo->d.AgingData.d.AgingErrInfo.ErrDieCnt, smSysInfo->d.AgingData.d.AgingErrInfo.TotalDie);
            break;
        case 7:
            #if CHK_FW_TYPE(FW_AGING)
            D_PRINTF("SN[%s]\n",smSysInfo->d.AgingData.d.Aging_SN);
            for (dump = 0; dump < 2; dump++)
            {
                for (cnt = 0; cnt < smSysInfo->d.AgingEI.d.ErrPaaCnt; cnt++)
                {
                    if (dump == 0)
                    {
                        D_PRINTF("Cnt[%4d]Type[%4x]ErrType[%4x]Loop[%3d]PlaneCnt[%1d]Paa[%d.%d.%d.%3X.%3X.%3X]LogBlk[%3x]Ecc[%c]Toggle[%c]RetryCnt[%2d]\n",
                                cnt + 1,
                                smSysInfo->d.AgingEI.d.Data[cnt].Type,
                                smSysInfo->d.AgingEI.d.Data[cnt].ErrType,
                                smSysInfo->d.AgingEI.d.Data[cnt].Loop,
                                smSysInfo->d.AgingEI.d.Data[cnt].PlaneCnt,
                                smSysInfo->d.AgingEI.d.Data[cnt].Ch,
                                smSysInfo->d.AgingEI.d.Data[cnt].Ce,
                                smSysInfo->d.AgingEI.d.Data[cnt].Lun,
                                smSysInfo->d.AgingEI.d.Data[cnt].Block,
                                smSysInfo->d.AgingEI.d.Data[cnt].PairBlk[0],
                                smSysInfo->d.AgingEI.d.Data[cnt].Page,
                                smSysInfo->d.AgingEI.d.Data[cnt].LogBlock,
                                smSysInfo->d.AgingEI.d.Data[cnt].EccStatus,
                                smSysInfo->d.AgingEI.d.Data[cnt].ToggleStatus,
                                smSysInfo->d.AgingEI.d.Data[cnt].RetryCnt);
                    }
                    else if (dump == 1)
                    {
                        D_PRINTF("Cnt[%4d]Soc[%3d]I2C[%3d]EccCw[%2x %2x %2x %2x %2x %2x %2x %2x]ToggleCw[%2x %2x %2x %2x %2x %2x %2x %2x]\n",
                                cnt + 1,
                                smSysInfo->d.AgingEI.d.Data[cnt].Temperature.SOC,
                                smSysInfo->d.AgingEI.d.Data[cnt].Temperature.I2C,
                                smSysInfo->d.AgingEI.d.Data[cnt].EccCw[0],
                                smSysInfo->d.AgingEI.d.Data[cnt].EccCw[1],
                                smSysInfo->d.AgingEI.d.Data[cnt].EccCw[2],
                                smSysInfo->d.AgingEI.d.Data[cnt].EccCw[3],
                                smSysInfo->d.AgingEI.d.Data[cnt].EccCw[4],
                                smSysInfo->d.AgingEI.d.Data[cnt].EccCw[5],
                                smSysInfo->d.AgingEI.d.Data[cnt].EccCw[6],
                                smSysInfo->d.AgingEI.d.Data[cnt].EccCw[7],
                                smSysInfo->d.AgingEI.d.Data[cnt].ToggleCw[0],
                                smSysInfo->d.AgingEI.d.Data[cnt].ToggleCw[1],
                                smSysInfo->d.AgingEI.d.Data[cnt].ToggleCw[2],
                                smSysInfo->d.AgingEI.d.Data[cnt].ToggleCw[3],
                                smSysInfo->d.AgingEI.d.Data[cnt].ToggleCw[4],
                                smSysInfo->d.AgingEI.d.Data[cnt].ToggleCw[5],
                                smSysInfo->d.AgingEI.d.Data[cnt].ToggleCw[6],
                                smSysInfo->d.AgingEI.d.Data[cnt].ToggleCw[7]);
                    }
                }
            }
            #endif
            break;
        case 8:
            #if CHK_FW_TYPE(FW_AGING)
            D_PRINTF("Cnt[%4d]Paa[%d.%d.%d.%3X.%3X.%3X]\n",
                    DisCnt,
                    smSysInfo->d.AgingEI.d.Data[DisCnt - 1].Ch,
                    smSysInfo->d.AgingEI.d.Data[DisCnt - 1].Ce,
                    smSysInfo->d.AgingEI.d.Data[DisCnt - 1].Lun,
                    smSysInfo->d.AgingEI.d.Data[DisCnt - 1].Block,
                    smSysInfo->d.AgingEI.d.Data[DisCnt - 1].PairBlk[0],
                    smSysInfo->d.AgingEI.d.Data[DisCnt - 1].Page);
            for (cnt = 0; cnt < 512; cnt++)
            {
                D_PRINTF("%7d\n",smSysInfo->d.AgingEI.d.Data[DisCnt - 1].Distribution[cnt]);
            }
            #endif
            break;
        case 9:
            D_PRINTF("Nand Flash ID\n");
            Debug_DumpMemU8((uint32_t)&smSysInfo->d.AgingData.d.AgingFlashID, sizeof(tAGING_FLASHID) * gNfInfo.totalLun);
            break;
        case 10:
            D_PRINTF("Dram\n");
            Debug_DumpMemU8((uint32_t)&smSysInfo->d.AgingData.d.AgingDram, sizeof(tAG_DRAM));
            break;
        case 11:
            D_PRINTF("OP\n");
            D_PRINTF("Op[%4d]%[%2d]\n", smSysInfo->d.AgingData.d.AgingSbkCnt.SbkOpBlk, smSysInfo->d.AgingData.d.AgingSbkCnt.SbkOpRatio);
            break;
        default:
            break;
    }
}

//-------------------------------------------------------------------
// Function     : Aging_Center(U32 opcode, U32 loop, U32 mode)
// Description  : Dump Aging and Plist2 Info
// Input        : opcode, loop, mode
// return       : N/A
//-------------------------------------------------------------------
void Aging_Center(U32 opcode, U32 loop, U32 mode)
{
    switch (opcode)
    {
        case 0:
            Aging_DumpInfo(loop, mode);
            break;
        case 1:
            #if CHK_FW_TYPE(FW_AGING)
            Aging_SysInfoUpdate(loop, mode);
            #endif
            break;
        case 2:
            #if CHK_FW_TYPE(FW_AGING)
            Aging_Nand_BusyTime();
            #endif
            break;
        case 3:
            #if CHK_FW_TYPE(FW_AGING)
            Aging_Nand_FlashID();
            #endif
            break;
        default:
            D_PRINTF("Error [%d]\n", opcode);
    }
}
