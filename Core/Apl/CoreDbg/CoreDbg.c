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
//! @brief SEQ(Sequencer) Read/Write API codes @ Core CPU2
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"

#include "RegEcu.h"

#include "Ftl.h"
#include "Fio.h"
#include "Cache.h"
#include "Core.h"
#include "Gc.h"
#include "Aging.h"
#include "SysInfo.h"

#include "CoreDbg.h"

#include "NormSection.h"
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
void Dbg_EraseAll(tPAA paa);
void Dbg_EraseBlock(U16 multiplane, tPAA paa, U16* block);
void Dbg_WriteBlock(U16 multiplane, tPAA paa, U16* block);
void Dbg_ReadBlock(U16 multiplane, tPAA paa, U16* block);
void Dbg_WritePage(U16 multiplane, tPAA paa, U16* block);
void Dbg_ReadPage(U16 multiplane, tPAA paa, U16* block);
void Dbg_Write(U16 planeNum, tPAA paa, U16* block);
void Dbg_Read(U16 planeNum, tPAA paa, U16* block);
void Dbg_EccScan(U16 planeNum, tPAA srcPaa, PVOID pBuffer);
void Dbg_SaveEraseCnt(void);
void Dbg_ScanDefect(void);
void Dbg_CalNandBusyTime(tPAA paa, U16* block);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
tCOREDBG_BUF gCoreDbgBuf;

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
    Fill Read buffers.\n

    @param[in]  pInfo               Information pointer

    @return error code
**/
//-----------------------------------------------------------------------------
void CoreDbg_OneTimeInit(InitBootMode_t initMode)
{
    #if _MEDIA_ >= MEDIA_NAND
    gCoreDbgBuf.wBuffer    = MEM_AllocBuffer(1 * MBYTE, 512);
    gCoreDbgBuf.rBuffer    = MEM_AllocBuffer(1 * MBYTE, 512);
    gCoreDbgBuf.eccScanTbl = MEM_AllocBuffer(gNfInfo.pagesPerBlock * gEcuInfo.auPerPage * gEcuInfo.cwPerAu * sizeof(U8), 8);
    #endif
}

//-------------------------------------------------------------------
// Function     : CoreDbg_Center(U16 opCode, U16 plane, tPAA paa, U16 pairblk1, U16 pairblk2, U16 pairblk3)
// Description  :
// Input        : opCode, plane, paa, pairblk1, pairblk2, pairblk3
// return       : N/A
//-------------------------------------------------------------------
void CoreDbg_Center(U16 opCode, U16 plane, tPAA paa, U16 pairblk1, U16 pairblk2, U16 pairblk3)
{
    U16 block[4];
    block[0] = paa.b.block;
    block[1] = pairblk1;
    block[2] = pairblk2;
    block[3] = pairblk3;

    switch (opCode)
    {
        case COREDBG_CMD_ERASE:
            Dbg_EraseBlock(plane, paa, block);
            break;
        case COREDBG_CMD_WRITE:
            Dbg_WritePage(plane, paa, block);
            break;
        case COREDBG_CMD_READ:
            Dbg_ReadPage(plane, paa, block);
            break;
        case COREDBG_CMD_ECCSCAN:
            Dbg_EccScan(plane, paa, gCoreDbgBuf.eccScanTbl);
            break;
        case COREDBG_CMD_ERASEALL:
            Dbg_EraseAll(paa);
            break;
        case COREDBG_CMD_WRITE_BLK:
            Dbg_WriteBlock(plane, paa, block);
            break;
        case COREDBG_CMD_READ_BLK:
            Dbg_ReadBlock(plane, paa, block);
            break;
        case COREDBG_CMD_SCANDEFECT:
            Dbg_ScanDefect();
            break;
        case COREDBG_CMD_NAND_BUSYTIME:
            Dbg_CalNandBusyTime(paa, block);
            break;
        default:
            break;
    }
}

//-----------------------------------------------------------------------------
/**
    Read pages with generic descriptor by using sequencer.\n

    @param[in]  pInfo               Information pointer

    @return error code
**/
//-----------------------------------------------------------------------------
void Dbg_EccScan(U16 planeNum, tPAA paa, PVOID pBuffer)
{
    volatile EcuDecStat2_t* ecuDecStat2   = (EcuDecStat2_t*)0xF200C054;
    volatile EcuDecStat3_t* ecuDecStat3   = (EcuDecStat3_t*)0xF200C056;
    volatile EcuDecStat4_t* ecuDecStat4   = (EcuDecStat4_t*)0xF200C058;
    volatile EcuDecStat21_t* ecuDecStat21 = (EcuDecStat21_t*)0xF200C192;

    tFIO_JOB* pJob = NULL;
    U16 page;
    U8  idx, cwxcnt, readUnit, cnt = 0;

    D_PRINTF("[ECC ] PAA[CH%1d|CE%1d|LN%1d|PN%1d|BK0x%3X]", paa.b.ch, paa.b.ce, paa.b.lun, paa.b.plane, paa.b.block);

    MEM_SET(gCoreDbgBuf.eccScanTbl, 0xFF, gNfInfo.pagesPerBlock * gEcuInfo.auPerPage * gEcuInfo.cwPerAu * sizeof(U8));

    readUnit = (planeNum == 2) ? gNfInfo.bitsPerCell : 1;

    for (page = 0; page < gNfInfo.pagesPerBlock; page+=readUnit)
    {
        cwxcnt = 0;
        paa.b.page = page;
        pJob = FIO_PopJob(FIO_JOB_POOL_FREE);
        pJob->css     = CACHE_AUTO;
        pJob->plane   = gNfInfo.planesPerLun;
        pJob->paaCnt  = gEcuInfo.auPerPage;
        pJob->flag    = FIO_F_NO_MKERR | FIO_F_NO_RR | FIO_F_MUTE;
        pJob->pBuffer = gCoreDbgBuf.rBuffer;

        if (planeNum == 2)
        {
            pJob->cmd = FIO_JOB_CMD_READ_SLC;
            pJob->flag |= FIO_F_SLC;
        }
        else
        {
            pJob->cmd = FIO_JOB_CMD_READ_FAST;
        }

        pJob->paa[0].all32 = 0;
        for (idx = 0; idx < pJob->paaCnt; idx++)
        {
            pJob->paa[idx].all32= paa.all32;
            pJob->paa[idx].b.frag = idx;
            pJob->paa[idx].b.page = page;
            pJob->paa[idx].b.plane = paa.b.plane;
            pJob->paa[idx].b.block = paa.b.block;
        }

        Core_JobIssue(pJob);

        Core_JobWait();

        cnt = ecuDecStat21->b.CUR_16HISTORY_READ_PTR;

        // ECU
        for (idx=0; idx < gEcuInfo.auPerPage; idx++)
        {
            ecuDecStat2->b.AU_DEC_STATUS_PTR = cnt - idx;

            for (cwxcnt = 0; cwxcnt < gEcuInfo.cwPerAu; cwxcnt++)
            {
                if (ecuDecStat3->all & 0x0040)      gCoreDbgBuf.eccScanTbl[page * gEcuInfo.auPerPage * gEcuInfo.cwPerAu + idx * gEcuInfo.cwPerAu + cwxcnt] = 0xE0; //Empty
                else if(ecuDecStat3->all & 0x0002)  gCoreDbgBuf.eccScanTbl[page * gEcuInfo.auPerPage * gEcuInfo.cwPerAu + idx * gEcuInfo.cwPerAu +  cwxcnt] = 0xF0; //Uncorrectable
                else
                {
                    ecuDecStat2->b.CW_DEC_STAT_PTR = cwxcnt;
                    gCoreDbgBuf.eccScanTbl[page * gEcuInfo.auPerPage * gEcuInfo.cwPerAu + idx * gEcuInfo.cwPerAu + cwxcnt] = ecuDecStat4->b.CWX_CORR_CNT;
                }
            }
        }
    }

    MEM_COPY(pBuffer, gCoreDbgBuf.eccScanTbl, gNfInfo.pagesPerBlock * gEcuInfo.auPerPage);

    for (page = 0; page < gNfInfo.pagesPerBlock; page++)
    {
        #if _NAND_ == NAND_3DTLCG2
        if(!(page % gNfInfo.bitsPerCell))   D_PRINTF("\n");
        #else
        if(!(page % gEcuInfo.auPerPage))    D_PRINTF("\n");
        #endif
        D_PRINTF("(%03X) ",page);
        for(idx=0; idx < gEcuInfo.auPerPage; idx++)
            for (cwxcnt = 0; cwxcnt < gEcuInfo.cwPerAu; cwxcnt++)
                D_PRINTF("%02X ", gCoreDbgBuf.eccScanTbl[page * gEcuInfo.auPerPage * gEcuInfo.cwPerAu + idx * gEcuInfo.cwPerAu + cwxcnt]);
    }

    D_PRINTF("\n");
}

//-------------------------------------------------------------------
// Function     : void CORESEQ_EraseAll(tPAA pPAA)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void Dbg_EraseAll(tPAA paa)
{
    tFIO_JOB* pJob = NULL;
    U16 blk, idx;
    U8  ch, ce, lun, plane, done;

    done = FALSE;
    blk  = paa.b.block;
    lun  = 0;
    ce   = 0;

    while (done == FALSE)
    {
        if (smFioInfo.jobCnt[FIO_JOB_POOL_FREE])
        {
            if ((blk & 0xFF) == 0) { D_PRINTF("Erase CE|%d LN|%d Blk|0x%4X\n", ce, lun, blk); }
            idx = 0;
            pJob = FIO_PopJob(FIO_JOB_POOL_FREE);
            pJob->cmd   = FIO_JOB_CMD_ERASE;
            pJob->plane = gNfInfo.planesPerLun;
            pJob->paaCnt= gNfInfo.numChannels * pJob->plane;
            pJob->flag  = FIO_F_NO_MKERR;

            #if _NAND_ == NAND_3DTLCG2
            for (ch = 0; ch < gNfInfo.numChannels; ch++)
            {
                for (plane = 0; plane < gNfInfo.planesPerLun; plane++)
                {
            #elif _NAND_ ==  NAND_MLC
            for (plane = 0; plane < gNfInfo.planesPerLun; plane++)
            {
                for (ch = 0; ch < gNfInfo.numChannels; ch++)
                {
            #endif
                    pJob->paa[idx].all32   = 0;
                    pJob->paa[idx].b.ch    = ch;
                    pJob->paa[idx].b.ce    = ce;
                    pJob->paa[idx].b.lun   = lun;
                    pJob->paa[idx].b.block = blk;
                    pJob->paa[idx].b.plane = plane;
                    idx++;
                }
            }
            Core_JobIssue(pJob);

            if (ce++ >= (gNfInfo.devicesPerChannel - 1))
            {
                ce = 0;
                if (lun++ >= (gNfInfo.lunsPerDevice - 1))
                {
                    lun = 0;
                    if (blk++ >= (gNfInfo.blocksPerPlane - 1))
                    {
                        done = TRUE;
                    }
                }
            }
        }
        else
        {
            HalTimer_DelayUsec(1);

            if (IPC_MsgQFastPeek(cM2cComQueue))
            {
                Core_ProcJobComplete();
            }
        }
        Core_JobWait();
    }
    D_PRINTF("EraseAll BlkNo: 0x%4X Done\n", blk);
}

//-------------------------------------------------------------------
// Function     : void Dbg_EraseBlock(U16 plane, tPAA paa, U16* block)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void Dbg_EraseBlock(U16 multiplane, tPAA paa, U16* block)
{
    tFIO_JOB* pJob;
    U16 cnt = 0;
    U8  plane;
    Core_JobWait();

    pJob = FIO_PopJob(FIO_JOB_POOL_FREE);
    pJob->cmd   = FIO_JOB_CMD_ERASE;
    pJob->plane = (multiplane) ? gNfInfo.planesPerLun : FIO_CMD_1PLANE;
    pJob->css   = CACHE_AUTO;
    pJob->flag  = FIO_F_NO_MKERR | FIO_F_NANDTIME;

    for (plane = 0; plane < pJob->plane; plane++)
    {
        gFastPaaBuf[cnt].all32   = 0;
        gFastPaaBuf[cnt].b.block = block[plane];
        gFastPaaBuf[cnt].b.ch    = paa.b.ch;
        gFastPaaBuf[cnt].b.ce    = paa.b.ce;
        gFastPaaBuf[cnt].b.lun   = paa.b.lun;
        gFastPaaBuf[cnt].b.plane = (multiplane) ? plane : paa.b.plane;
        cnt++;
    }
    pJob->paaCnt = cnt;
    MEM_COPY(pJob->paa, gFastPaaBuf, sizeof(tPAA)*pJob->paaCnt);

    Core_JobIssue(pJob);
    Core_JobWait();

    #if (CO_SUPPORT_NAND_BUSYTIME == TRUE)
    D_PRINTF("[E] PAA[CH%d|CE%d|LN%d|PN%d|BK0x%3x] BusyTime[%4d]\n", pJob->paa[0].b.ch, pJob->paa[0].b.ce, pJob->paa[0].b.lun, pJob->paa[0].b.plane, pJob->paa[0].b.block, smNandBusyTime);
    #endif
}

//-------------------------------------------------------------------
// Function     : void Dbg_WriteBlock(U16 multiplane, tPAA paa, U16* block)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void Dbg_WriteBlock(U16 multiplane, tPAA paa, U16* block)
{
    U32 *pCurBuffer;
    U32  idx;
    U16  cnt, page, pagecnt, planeNum;
    U8   done;

    pCurBuffer = (U32 *)&gCoreDbgBuf.wBuffer[0];
    done       = FALSE;
    planeNum   = (multiplane) ? gNfInfo.planesPerLun : FIO_CMD_1PLANE;
    page       = 0;
    pagecnt    = (paa.b.page > gNfInfo.pagesPerBlock) ? gNfInfo.pagesPerBlock : paa.b.page;

    D_PRINTF("Write %dPlane Page[0~0x%3x]\n", planeNum, paa.b.page);

    for (cnt = 0; cnt < 64; cnt++)
    {
        for(idx = 0; idx < 0x1000UL; idx++)
        {
            pCurBuffer[cnt*0x1000UL+idx] = (U32) 0x55AA55AA;
        }
    }

    while (done == FALSE)
    {
        if (smFioInfo.jobCnt[FIO_JOB_POOL_FREE])
        {
            paa.b.page  = page;

            Dbg_Write(planeNum, paa, block);

            page+=gNfInfo.pagesPerProgUnit;
            if (page > pagecnt)
            {
                done = TRUE;
            }

            Core_JobWait();
        }
        else
        {
            HalTimer_DelayUsec(1);

            if (IPC_MsgQFastPeek(cM2cComQueue))
            {
                Core_ProcJobComplete();
            }
        }
    }
    Core_JobWait();
}

//-------------------------------------------------------------------
// Function     : void Dbg_ReadBlock(U16 multiplane, tPAA paa, U16* block)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void Dbg_ReadBlock(U16 multiplane, tPAA paa, U16* block)
{
    U32 *pCurBuffer;
    U32  idx;
    U16  cnt, page, pagecnt, planeNum;
    U8   done;

    pCurBuffer = (U32 *)&gCoreDbgBuf.rBuffer[0];
    done       = FALSE;
    planeNum   = (multiplane) ? gNfInfo.planesPerLun : FIO_CMD_1PLANE;
    page       = 0;
    pagecnt    = (paa.b.page > gNfInfo.pagesPerBlock) ? gNfInfo.pagesPerBlock : paa.b.page;

    D_PRINTF("Read %dPlane Page[0~0x%3x]\n", planeNum, paa.b.page);

    for (cnt = 0; cnt < 64; cnt++)
    {
        for(idx = 0; idx < 0x1000UL; idx++)
        {
            pCurBuffer[cnt*0x1000UL+idx] = (U32) 0xDEADBEEF;
        }
    }

    while (done == FALSE)
    {
        if (smFioInfo.jobCnt[FIO_JOB_POOL_FREE])
        {
            paa.b.page = page;

            Dbg_Read(planeNum, paa, block);

            Core_JobWait();
            if (++page > pagecnt)
            {
                page = 0;
                done = TRUE;
            }
        }
        else
        {
            HalTimer_DelayUsec(1);

            if (IPC_MsgQFastPeek(cM2cComQueue))
            {
                Core_ProcJobComplete();
            }
        }
    }
    Core_JobWait();
}

//-------------------------------------------------------------------
// Function     : void Dbg_WritePage(U16 multiplane, tPAA paa, U16* block)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void Dbg_WritePage(U16 multiplane, tPAA paa, U16* block)
{
    U32 *pCurBuffer;
    U32  idx;
    U16  cnt, planeNum;

    planeNum = (multiplane) ? gNfInfo.planesPerLun : FIO_CMD_1PLANE;

    Core_JobWait();

    pCurBuffer = (U32 *)&gCoreDbgBuf.wBuffer[0];

    for (cnt = 0; cnt < 64; cnt++)
    {
        for(idx = 0; idx < 0x1000UL; idx++)
        {
            pCurBuffer[cnt*0x1000UL+idx] = (U32) idx;
        }
    }

    Dbg_Write(planeNum, paa, block);
}

//-------------------------------------------------------------------
// Function     : void Dbg_ReadPage(U16 multiplane, tPAA paa, U16* block)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void Dbg_ReadPage(U16 multiplane, tPAA paa, U16* block)
{
    U32 *pCurBuffer;
    U32  idx;
    U16  cnt, planeNum;

    planeNum = (multiplane) ? gNfInfo.planesPerLun : FIO_CMD_1PLANE;

    Core_JobWait();

    pCurBuffer = (U32 *)&gCoreDbgBuf.rBuffer[0];

    for (cnt = 0; cnt < 64; cnt++)
    {
        for(idx = 0; idx < 0x1000UL; idx++)
        {
            pCurBuffer[cnt*0x1000UL+idx] = (U32) 0xDEADBEEF;
        }
    }

    Dbg_Read(planeNum, paa, block);
}

//-------------------------------------------------------------------
// Function     : void Dbg_ScanDefect(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void Dbg_ScanDefect(void)
{
    MsgService_t msg;

    MEM_CLR(&msg, sizeof(msg));

    MSG_SET_SERVICE_INIT(msg, cSfnNfimScanDefect);

    Ipc_SendWaitMsg(cC2mOtherQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);

    smSysInfo->d.Header.d.SubHeader.d.PList1.d.dwValidTag     = SI_TAG_PLIST1;
    smSysInfo->d.Header.d.SubHeader.d.PlistCnt.d.dwValidTag   = SI_TAG_PLSTCNT;

    SyIn_Synchronize(SI_AREA_PLSTCNT | SI_AREA_PLIST1, SYSINFO_WRITE_FORCE, SI_SYNC_BY_SCANDEFECT);
	SyIn_Synchronize(SI_AREA_PLSTCNT | SI_AREA_PLIST1, SYSINFO_WRITE, SI_SYNC_BY_NOR);
}

//-------------------------------------------------------------------
// Function     : Dbg_CalNandBusyTime(tPAA paa, U16* block)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void Dbg_CalNandBusyTime(tPAA paa, U16* block)
{
    #if (CO_SUPPORT_NAND_BUSYTIME == TRUE)
    U32  busyTimeErase, busyTimeWrite, busyTimeRead, busyTimeWrite_Max, busyTimeRead_Max;
    U16  page;
    U8   type;

    busyTimeErase = busyTimeWrite = busyTimeRead = busyTimeWrite_Max = busyTimeRead_Max = 0;

    for (type = 0; type < 4; type++)
    {
        Core_JobWait();

        if (type == 0)
        {
            Dbg_EraseBlock(0, paa, block);
            Core_JobWait();
            busyTimeErase = smNandBusyTime;
        }
        else if (type == 1)
        {
            for (page = 0; page < gNfInfo.pagesPerBlock; page+=gNfInfo.pagesPerProgUnit)  //WL
            {
                paa.b.page = page;
                Dbg_WritePage(0, paa, block);
                Core_JobWait();
                busyTimeWrite += smNandBusyTime;
                if (smNandBusyTime >= busyTimeWrite_Max)
                {
                    busyTimeWrite_Max = smNandBusyTime;
                }
            }
            #if _NAND_ == NAND_3DTLCG2
            busyTimeWrite = (busyTimeWrite / gNfInfo.wlPerBlock);
            #elif _NAND_ ==  NAND_MLC
            busyTimeWrite = (busyTimeWrite / gNfInfo.pagesPerProgUnit);
            #endif
        }
        else if (type == 2)
        {
            for (page = 0; page < gNfInfo.pagesPerBlock; page++)  //Page0---->0
            {
                paa.b.page  = page;
                Dbg_ReadPage(0, paa, block);
                Core_JobWait();
                busyTimeRead += smNandBusyTime;
                if (smNandBusyTime >= busyTimeRead_Max)
                {
                    busyTimeRead_Max = smNandBusyTime;
                }
            }
            busyTimeRead = (busyTimeRead / gNfInfo.pagesPerBlock);
        }
        else if (type == 3)
        {
            Dbg_EraseBlock(0, paa, block);
            Core_JobWait();
        }
    }

    D_PRINTF("\nE[%4d] AVG_P[%4d] MAX_P[%4d] AVG_R[%4d] MAX_R[%4d] \n",
                busyTimeErase, busyTimeWrite, busyTimeWrite_Max, busyTimeRead, busyTimeRead_Max);
    #else
    D_PRINTF("CoreDbg Calculate Nand Busy Time Not Support\n");
    #endif
}

//-------------------------------------------------------------------
// Function     : Dbg_SaveEraseCnt(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void Dbg_SaveEraseCnt(void)
{
    D_PRINTF("Dbg_SaveEraseCnt \n");

    #if CHK_FW_TYPE(FW_AGING)
    if ((smSysInfo->d.NVEC.d.EraseCnt.d.EraseTag != SI_TAG_NVEC) && (smSysInfo->d.Header.d.MainHeader.d.dwTag == SI_TAG_SYSINFO)) //First Time
    {

        smSysInfo->d.NVEC.d.EraseCnt.d.EraseTag             = SI_TAG_NVEC;
        smSysInfo->d.NVEC.d.EraseCnt.d.AgingEraseCnt       += smSysInfo->d.AgingData.d.Aging_EraseCnt;
        smSysInfo->d.Header.d.SubHeader.d.NVEC.d.dwValidTag = SI_TAG_NVEC;
        SyIn_Synchronize(SI_AREA_NVEC, SYSINFO_WRITE, SI_SYNC_BY_SCANDEFECT);
    }
    else if((smSysInfo->d.NVEC.d.EraseCnt.d.EraseTag == SI_TAG_NVEC) && (smSysInfo->d.Header.d.MainHeader.d.dwTag == SI_TAG_SYSINFO))
    {
        smSysInfo->d.NVEC.d.EraseCnt.d.AgingEraseCnt       += smSysInfo->d.AgingData.d.Aging_EraseCnt;
        smSysInfo->d.Header.d.SubHeader.d.NVEC.d.dwValidTag = SI_TAG_NVEC;
        SyIn_Synchronize(SI_AREA_NVEC, SYSINFO_WRITE, SI_SYNC_BY_SCANDEFECT);
    }
    #else

    U32 AverEcCnt = 0, MaxEcCnt = 0;
    U16 blk;

    for (blk = 0; blk < FTL_SUPPORT_MAX_BLK; blk++)
    {
        AverEcCnt += gFtlBlkListTbl[blk].eraseCnt;

        if(MaxEcCnt < gFtlBlkListTbl[blk].eraseCnt)
            MaxEcCnt = gFtlBlkListTbl[blk].eraseCnt;
    }

    AverEcCnt = ( AverEcCnt / FTL_SUPPORT_MAX_BLK );

    if ((smSysInfo->d.NVEC.d.EraseCnt.d.EraseTag != SI_TAG_NVEC) && (smSysInfo->d.Header.d.MainHeader.d.dwTag == SI_TAG_SYSINFO)) //First Time
    {
        smSysInfo->d.NVEC.d.EraseCnt.d.EraseTag             = SI_TAG_NVEC;
        smSysInfo->d.NVEC.d.EraseCnt.d.AverEcCnt           += AverEcCnt;
        smSysInfo->d.NVEC.d.EraseCnt.d.MaxEcCnt            += MaxEcCnt;
        smSysInfo->d.NVEC.d.EraseCnt.d.TotalEcCnt          += gFtlMgr.AllEraseCnt;

        smSysInfo->d.NVEC.d.EraseCnt.d.TempAverEcCnt        = AverEcCnt;
        smSysInfo->d.NVEC.d.EraseCnt.d.TempMaxEcCnt         = MaxEcCnt;
        smSysInfo->d.NVEC.d.EraseCnt.d.TempTotalEcCnt       = gFtlMgr.AllEraseCnt;

        smSysInfo->d.Header.d.SubHeader.d.NVEC.d.dwValidTag = SI_TAG_NVEC;

        SyIn_Synchronize(SI_AREA_NVEC, SYSINFO_WRITE, SI_SYNC_BY_SCANDEFECT);
    }
    else if((smSysInfo->d.NVEC.d.EraseCnt.d.EraseTag == SI_TAG_NVEC) && (smSysInfo->d.Header.d.MainHeader.d.dwTag == SI_TAG_SYSINFO))
    {
        //D_PRINTF("TAG  eq SI_TAG_ECNT \n");
        gFtlMgr.AllEraseCnt                                -= smSysInfo->d.NVEC.d.EraseCnt.d.TempTotalEcCnt;
        MaxEcCnt                                           -= smSysInfo->d.NVEC.d.EraseCnt.d.TempMaxEcCnt;
        AverEcCnt                                          -= smSysInfo->d.NVEC.d.EraseCnt.d.TempAverEcCnt;

        smSysInfo->d.NVEC.d.EraseCnt.d.TotalEcCnt          += gFtlMgr.AllEraseCnt;
        smSysInfo->d.NVEC.d.EraseCnt.d.MaxEcCnt            += MaxEcCnt;
        smSysInfo->d.NVEC.d.EraseCnt.d.AverEcCnt           += AverEcCnt;

        smSysInfo->d.NVEC.d.EraseCnt.d.TempTotalEcCnt      += gFtlMgr.AllEraseCnt;
        smSysInfo->d.NVEC.d.EraseCnt.d.TempMaxEcCnt        += MaxEcCnt;
        smSysInfo->d.NVEC.d.EraseCnt.d.TempAverEcCnt       += AverEcCnt;


        smSysInfo->d.Header.d.SubHeader.d.NVEC.d.dwValidTag = SI_TAG_NVEC;

        SyIn_Synchronize(SI_AREA_NVEC, SYSINFO_WRITE, SI_SYNC_BY_SCANDEFECT);
    }
    #endif
}

//-------------------------------------------------------------------
// Function     : Dbg_Read(U16 planeNum, tPAA paa, U16* block)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void Dbg_Read(U16 planeNum, tPAA paa, U16* block)
{
    tFIO_JOB* pJob = NULL;
    U16  idx = 0;
    U8   plane, frag;

    for (plane = 0; plane < planeNum; plane++)
    {
        for(frag = 0; frag < gEcuInfo.auPerPage; frag++)
        {
            gFastPaaBuf[idx].all32   = 0;
            gFastPaaBuf[idx].b.block = block[plane];
            gFastPaaBuf[idx].b.ch    = paa.b.ch;
            gFastPaaBuf[idx].b.ce    = paa.b.ce;
            gFastPaaBuf[idx].b.lun   = paa.b.lun;
            gFastPaaBuf[idx].b.plane = (planeNum == 1) ? paa.b.plane : plane;
            gFastPaaBuf[idx].b.page  = paa.b.page;
            gFastPaaBuf[idx].b.frag  = frag;
            idx++;
        }
    }
    if (idx)
    {
        pJob = FIO_PopJob(FIO_JOB_POOL_FREE);
        pJob->cmd     = FIO_JOB_CMD_READ_FAST;
        pJob->plane   = planeNum;
        pJob->paaCnt  = idx;
        pJob->pBuffer = gCoreDbgBuf.rBuffer;
        pJob->css     = CACHE_AUTO;
        pJob->flag    = FIO_F_NO_MKERR | FIO_F_NO_RR | FIO_F_NANDTIME;

        MEM_COPY(pJob->paa, gFastPaaBuf, sizeof(tPAA)*pJob->paaCnt);

        Core_JobIssue(pJob);
    }
    Core_JobWait();

    #if (CO_SUPPORT_NAND_BUSYTIME == TRUE)
    D_PRINTF("[R] PAA[CH%d|CE%d|LN%d|PN%d|BK0x%3x|PG0x%3x] BusyTime[%4d]\n",paa.b.ch, paa.b.ce, paa.b.lun, paa.b.plane, paa.b.block, paa.b.page, smNandBusyTime);
    #endif
}

//-------------------------------------------------------------------
// Function     : Dbg_Write(U16 planeNum, tPAA paa, U16* block)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
void Dbg_Write(U16 planeNum, tPAA paa, U16* block)
{
    tFIO_JOB* pJob = NULL;
    U16  idx = 0;
    U8   plane, frag = 0;

    #if _NAND_ == NAND_3DTLCG2
    U8   lmu = 0;

    for (lmu = 0; lmu < gNfInfo.bitsPerCell; lmu++)
    {
        for (plane = 0; plane < planeNum; plane++)
        {
            for(frag = 0; frag < gEcuInfo.auPerPage; frag++)
            {
                gFastPaaBuf[idx].all32   = 0;
                gFastPaaBuf[idx].b.block = block[plane];
                gFastPaaBuf[idx].b.ch    = paa.b.ch;
                gFastPaaBuf[idx].b.ce    = paa.b.ce;
                gFastPaaBuf[idx].b.lun   = paa.b.lun;
                gFastPaaBuf[idx].b.page  = paa.b.page + lmu;
                gFastPaaBuf[idx].b.plane = (planeNum == 1) ? paa.b.plane : plane;
                gFastPaaBuf[idx].b.frag  = frag;
                idx++;
            }
        }
    }
    #elif _NAND_ ==  NAND_MLC
    for (plane = 0; plane < planeNum; plane++)
    {
        for(frag = 0; frag < gEcuInfo.auPerPage; frag++)
        {
            gFastPaaBuf[idx].all32   = 0;
            gFastPaaBuf[idx].b.block = block[plane];
            gFastPaaBuf[idx].b.ch    = paa.b.ch;
            gFastPaaBuf[idx].b.ce    = paa.b.ce;
            gFastPaaBuf[idx].b.lun   = paa.b.lun;
            gFastPaaBuf[idx].b.page  = paa.b.page;
            gFastPaaBuf[idx].b.plane = (planeNum == 1) ? paa.b.plane : plane;
            gFastPaaBuf[idx].b.frag  = frag;
            idx++;
        }
    }
    #endif
    if (idx)
    {
        pJob = FIO_PopJob(FIO_JOB_POOL_FREE);
        pJob->cmd     = FIO_JOB_CMD_WRITE;
        pJob->plane   = planeNum;
        pJob->paaCnt  = idx;
        pJob->pBuffer = gCoreDbgBuf.wBuffer;
        pJob->css     = CACHE_AUTO;
        pJob->flag    = FIO_F_NO_MKERR | FIO_F_NANDTIME;

        MEM_COPY(pJob->paa, gFastPaaBuf, sizeof(tPAA)*pJob->paaCnt);

        Core_JobIssue(pJob);
    }
    Core_JobWait();

    #if (CO_SUPPORT_NAND_BUSYTIME == TRUE)
    D_PRINTF("[W] PAA[CH%d|CE%d|LN%d|PN%d|BK0x%3x|PG0x%3x] BusyTime[%4d]\n", paa.b.ch, paa.b.ce, paa.b.lun, paa.b.plane, paa.b.block, paa.b.page, smNandBusyTime);
    #endif
}
