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
#include "MediaPkg.h"

#include "Fio.h"
#include "SysInfo.h"
#include "TcgCommon.h"
#include "TcgNfIf.h"
#include "TcgNfRdWr.h"
#include "IDS.h"
#include "Cache.h"

#include "AplSeq.h"
#include "AplNfif.h"
#include "AplDesc_TSB_MLC.h"


#include "PerfSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define DIE_SHIFT                               ( LOG2(gNfInfo.totalLun) )
#define GET_DIE_NUM(lun,ce,ch)                  ((lun << gNfInfo.bitsChDev) | (ce << (gNfInfo.bitsCh)) | ch)
#define GET_SBLK_IDX(plane,blk,die)             (((U32)(plane * gNfInfo.blocksPerPlane + blk) << DIE_SHIFT) | die)
#define GET_DIE_NUM_FROM_PAA(PLANE,LUN,CE,CH)   ((PLANE << (gNfInfo.bitsLun + gNfInfo.bitsChDev)) | ((LUN << (gNfInfo.bitsChDev)) | (CE << gNfInfo.bitsCh) | CH))
#define SET_RW_DESC_ROW_FROM_PAA(P)             ((P.b.lun << gNfInfo.lunLsbPosition/*21*/) | (P.b.block << gNfInfo.pairedBlkLsbPos) | (P.b.plane << gNfInfo.planeLsbPosition/*8*/)| P.b.page)

#if (READ_LEVEL_CHANGE == ENABLE)
    #define GET_RETRY_VOLTAGE_STAGE(V)          (((U8)(gVoltageStageList[V][3] - C_VALUE) << 24) | ((U8)(gVoltageStageList[V][2] - B_VALUE) << 16) | ((U8)(gVoltageStageList[V][1] - A_VALUE) << 8) | ((U8)(gVoltageStageList[V][0] - L_VALUE)))
#else
    #define GET_RETRY_VOLTAGE_STAGE(V)          ((gVoltageStageList[V][3] << 24) | (gVoltageStageList[V][2] << 16) | (gVoltageStageList[V][1] << 8) | (gVoltageStageList[V][0]))
#endif

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static U16 DescGeneric_TSB_MLC_Read(tFIO_JOB* pJob);
static U16 DescGeneric_TSB_MLC_FastRead(tFIO_JOB *pJob);
static U16 DescGeneric_TSB_MLC_SlcRead(tFIO_JOB *pJob);
static U16 DescGeneric_TSB_MLC_IdsRead(tFIO_JOB *pJob);
static U16 DescGeneric_TSB_MLC_IdsPageRead(tFIO_JOB *pJob);
static U16 DescGeneric_TSB_MLC_PreOpen(tFIO_JOB* pJob);
static U16 DescGeneric_TSB_MLC_RR(tFIO_JOB *pJob);
static U16 DescGeneric_TSB_MLC_Write(tFIO_JOB* pJob);
static U16 DescGeneric_TSB_MLC_Erase(tFIO_JOB* pJob);
static U16 DescGeneric_TSB_MLC_Exception(tFIO_JOB* pJob);

static __inline U16  DescGeneric_TSB_MLC_OpenPage(tFIO_JOB* pJob, U16 actDie, tPAA paa, U16 descOffset);
static __inline void DescGeneric_TSB_MLC_HD_Retry_CW(tFIO_JOB* pJob, U8 fg_cw, U8 fg_fd, U8 fg_JU, tPAA paa);
static __inline void DescGeneric_TSB_MLC_SD_Retry_CW(tFIO_JOB* pJob, U8 fg_cw, tPAA paa);
static __inline void DescGeneric_TSB_MLC_InitTemplate(void);

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
U16 (*gCbfProcDesc[FIO_JOB_CMD_COUNT])(tFIO_JOB* pJob) =
{
    DescGeneric_TSB_MLC_Read,
    DescGeneric_TSB_MLC_FastRead,
    DescGeneric_TSB_MLC_SlcRead,
    DescGeneric_TSB_MLC_Exception,
    DescGeneric_TSB_MLC_IdsPageRead,
    DescGeneric_TSB_MLC_IdsRead,
    DescGeneric_TSB_MLC_PreOpen,
    DescGeneric_TSB_MLC_RR,
	DescGeneric_TSB_MLC_Exception,
    DescGeneric_TSB_MLC_Write,
    DescGeneric_TSB_MLC_Erase,
};

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
SeqGenericDescOr_t     gTplGenericErase;
SeqGenericDescOr_t     gTplGenericWrite;
SeqGenericDescOr_t     gTplGenericRead;
SeqGenericDescOr_t     gTplGenericOpenRow;
SeqGenericDescOr_t     gTplGenericFastRead;
SeqGenericDescOr_t     gTplGenericVoltageShift;
SeqGenericDescOr_t     gTplGenericReadRetry;
SeqGenericDescOr_t     gTplGenericPreOpen;
SeqGenericDescOr_t     gTplGenericIdsRead;
SeqGenericDescOr_t     gTplGenericIdsPageRead;
SeqGenericDescOr_t     gTplGenericSlcRead;

SeqGenericDescOr_t     gTplGenericExitTestMode;
SeqGenericDescOr_t     gTplGenericRandomCacheRead;
SeqGenericDescOr_t     gTplGenericSoftBitXor;
SeqGenericFlexDescOr_t gTplGenericFlexDescOr;

U8  gVoltageStageList[MAX_READRETRY_ROW][MAX_READRETRY_COL] = {{0x0C, 0x00, 0x00, 0x00}, {0x02, 0x04, 0x02, 0x00},
                                                               {0x7C, 0x00, 0x7C, 0x7C}, {0x7A, 0x00, 0x7A, 0x7A},
                                                               {0x78, 0x02, 0x78, 0x7A}, {0x7E, 0x7C, 0x7E, 0x7C},
                                                               {0x76, 0x04, 0x76, 0x02}, {0x04, 0x04, 0x04, 0x7A},
                                                               {0x06, 0x00, 0x06, 0x78}, {0x74, 0x04, 0x74, 0x78},
                                                               {0x03, 0x04, 0x7B, 0x76}, {0x08, 0x00, 0x7D, 0x76},
                                                               {0x0A, 0x7C, 0x7F, 0x76}, {0x0C, 0x0A, 0x01, 0x02}};

U8  gPlane2FailByte[5] = {0, 0x3, 0x7, 0, 0x1F};

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
    One-time initialization of Descriptor.\n

    @param[in]  initMode            initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t AplDesc_OneTimeInit(InitBootMode_t initMode)
{
    DescGeneric_TSB_MLC_InitTemplate();

    MEM_SET(gOpenRow, BYTE_MASK, sizeof(gOpenRow));
    MEM_CLR(&gOpenPgMap[0][0], sizeof(gOpenPgMap));
    MEM_CLR(&gOpenPgCnt[0], sizeof(gOpenPgCnt));

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    General descriptor for erase operation

    @param[in]  pJob            Flash I/O Job pointer

    @return descriptor count
**/
//-----------------------------------------------------------------------------
static U16 DescGeneric_TSB_MLC_Erase(tFIO_JOB* pJob)
{
    tPAA paa;
    U32  actDie;
    U16  cnt;
    U16  descOffset;
    U8   endPlane;

    if (pJob->flag & FIO_F_SLC) MEM_SET(gOpenRow, BYTE_MASK, sizeof(gOpenRow));

    endPlane = pJob->paa[0].b.plane + pJob->plane - 1;
    descOffset  = 0;

    gTplGenericErase.field.TRACK_ID = pJob->tid;
    gTplGenericErase.field.FAIL     = gPlane2FailByte[gNfInfo.oriPlanesPerLun];

    for (cnt = 0; cnt < pJob->paaCnt; cnt++)
    {
        APLDESC_ALLOC_MULTI_TRK(descOffset, MAX_DESC_PER_TRK, pJob, gTplGenericErase, cnt);

        paa = pJob->paa[cnt];
        actDie = GET_DIE_NUM_FROM_PAA(paa.b.plane, paa.b.lun, paa.b.ce, paa.b.ch);
        if (pJob->plane == FIO_CMD_1PLANE) endPlane = paa.b.plane + pJob->plane - 1;

        gOpenRow[actDie] = DWORD_MASK;
        gTplGenericErase.field.DEV            = paa.b.ce;
        gTplGenericErase.field.CH             = paa.b.ch;
        gTplGenericErase.field.SCMD           = (paa.b.lun == 0) ? FC_READ_STATUS_BYTE2 : FC_READ_STATUS_BYTE3;

        gTplGenericErase.field.FD             = (descOffset) ? 0: 1;
        gTplGenericErase.field.LD             = ((cnt == (pJob->paaCnt - 1)) || CHK_OFFSET_OVER_TH(descOffset, MAX_DESC_PER_TRK)) ? 1: 0;

        gTplGenericErase.field.START_ROW_ADDR = (paa.b.lun   << gNfInfo.lunLsbPosition)  |
                                                (paa.b.block << gNfInfo.pairedBlkLsbPos) |
                                                (paa.b.plane << gNfInfo.planeLsbPosition);

        if (paa.b.plane == endPlane)
        {
            gTplGenericErase.field.OPTION = SEQ_OPTION_00012A;
            gTplGenericErase.field.BM     = (pJob->flag & FIO_F_NANDTIME) ? 1 : 0;
        }
        else
        {
            gTplGenericErase.field.OPTION = SEQ_OPTION_800802;
            gTplGenericErase.field.BM     = 0;
        }

        if ((pJob->flag & FIO_F_SLC) && ((paa.b.plane == 0) || (pJob->plane == FIO_CMD_1PLANE)))
        {
            gTplGenericErase.field.PREFIX_3B = FC_PREFIX_SLC_BYTE;
            gTplGenericErase.field.OPTION    = SEQ_OPTION_808802;

            if (paa.b.plane == endPlane) gTplGenericErase.field.OPTION = SEQ_OPTION_00812A;
        }

        HalSeq_PushDescriptors((PVOID)&gTplGenericErase, sizeof(SeqGenericDescOr_t));
        descOffset++;
        pJob->descCnt++;
    }

    return pJob->descCnt;
}

//-----------------------------------------------------------------------------
/**
    General descriptor for write operation

    @param[in]  pJob            Flash I/O Job pointer

    @return descriptor count
**/
//-----------------------------------------------------------------------------
static U16 DescGeneric_TSB_MLC_Write(tFIO_JOB* pJob)
{
    tPAA paa;
    U32 actDie;
    U16 cnt;
    U16 totalCnt;
    U16 cntOffset;
    U16 descOffset = 0;
    U8  endPlane;
    U8  failByte;

    if (pJob->flag & FIO_F_SLC) MEM_SET(gOpenRow, BYTE_MASK, sizeof(gOpenRow));

    totalCnt  = (pJob->paaCnt >> 2) - 1; // 16K
    cntOffset = 4; // 16K

    endPlane  = pJob->paa[0].b.plane + pJob->plane - 1;
    failByte  = gPlane2FailByte[gNfInfo.oriPlanesPerLun];

    gTplGenericWrite.field.AU_NUM        = 3; // 16K
    gTplGenericWrite.field.TRACK_ID      = pJob->tid;
    gTplGenericWrite.field.AU_SEG        = pJob->segNum;
    gTplGenericWrite.field.START_SSD_PTR = pJob->ssdIdx[pJob->ssdList];
    gTplGenericWrite.field.XF_COUNT      = (gEcuAuFormatList[pJob->auFmt].xferSizePerAu << 2); // 16K;
    gTplGenericWrite.field.AU_FORMAT     = pJob->auFmt;

    for (cnt = 0; cnt < pJob->paaCnt; cnt += cntOffset)
    {
        APLDESC_ALLOC_MULTI_TRK(descOffset, MAX_DESC_PER_TRK, pJob, gTplGenericWrite, cnt);

        paa = pJob->paa[cnt];
        if (pJob->plane == FIO_CMD_1PLANE) endPlane = paa.b.plane + pJob->plane - 1;

        actDie = GET_DIE_NUM_FROM_PAA(paa.b.plane, paa.b.lun, paa.b.ce, paa.b.ch);
        gOpenRow[actDie] = DWORD_MASK;
        gTplGenericWrite.field.JU             = cnt ? 0 : 1;
        gTplGenericWrite.field.FD             = descOffset ? 0 : 1;
        gTplGenericWrite.field.LD             = ((pJob->descCnt == totalCnt) || CHK_OFFSET_OVER_TH(descOffset, MAX_DESC_PER_TRK)) ? 1 : 0;
        gTplGenericWrite.field.BM             = 0;
        gTplGenericWrite.field.DEV            = paa.b.ce;
        gTplGenericWrite.field.CH             = paa.b.ch;
        gTplGenericWrite.field.OPTION         = SEQ_OPTION_000100;
        gTplGenericWrite.field.CMD1           = FC_PROG_BYTE0;
        gTplGenericWrite.field.CMD2           = FC_PROG_BYTE2;
        gTplGenericWrite.field.FAIL           = 0;
        gTplGenericWrite.field.START_ROW_ADDR = SET_RW_DESC_ROW_FROM_PAA(paa);
        gTplGenericWrite.field.SCMD           = (paa.b.lun == 0) ? FC_READ_STATUS_BYTE2 : FC_READ_STATUS_BYTE3;

        if (pJob->flag & FIO_F_SLC)
        {
            gTplGenericWrite.field.PREFIX_3B = FC_PREFIX_SLC_BYTE;
            gTplGenericWrite.field.OPTION    = SEQ_OPTION_008100;
        }
                // Normal model
        if (paa.b.plane == endPlane)
        {
            gTplGenericWrite.field.OPTION = SEQ_OPTION_000118;
            gTplGenericWrite.field.CMD2   = FC_PROG_BYTE1;
            gTplGenericWrite.field.FAIL   = failByte;
            gTplGenericWrite.field.BM     = (pJob->flag & FIO_F_NANDTIME) ? 1 : 0;

            if (pJob->plane != FIO_CMD_1PLANE)  gTplGenericWrite.field.CMD1 = FC_PROG_BYTE3_JEDEC;
            if ((pJob->flag & FIO_F_SLC) && (pJob->plane == FIO_CMD_1PLANE))  gTplGenericWrite.field.OPTION = SEQ_OPTION_008118;
        }

        HalSeq_PushDescriptors((PVOID)&gTplGenericWrite, sizeof(SeqGenericDescOr_t));

        descOffset++;
        pJob->descCnt++;
    }

    HalSeq_PushAuxInserts((PVOID)pJob->aux, gEcuAuFormatList[pJob->auFmt].auxSize * pJob->paaCnt);

    return pJob->descCnt;
}

//-----------------------------------------------------------------------------
/**
    General descriptor for read operation

    @param[in]  pJob            Flash I/O Job pointer

    @return descriptor count
**/
//-----------------------------------------------------------------------------
static U16 DescGeneric_TSB_MLC_Read(tFIO_JOB* pJob)
{
    tPAA paa;
    U32  actDie;
    U16  xferSize;
    U16  descOffset = 0;
    U16  cnt;

    DESC_RST_OPEN_PG_BMP(pJob->tid);

    xferSize = gEcuAuFormatList[pJob->auFmt].xferSizePerAu;

    gTplGenericRead.field.TRACK_ID      = pJob->tid;
    gTplGenericRead.field.AU_SEG        = pJob->segNum;
    gTplGenericRead.field.START_SSD_PTR = pJob->ssdIdx[pJob->ssdList];
    gTplGenericRead.field.AU_FORMAT     = pJob->auFmt;
    gTplGenericRead.field.XF_COUNT      = xferSize;
    gTplGenericRead.field.LE            = (pJob->flag & FIO_F_AUX) ? 1 : 0;

    for (cnt = 0; cnt < pJob->paaCnt; cnt++)
    {
        APLDESC_ALLOC_MULTI_TRK(descOffset, MAX_DESC_PER_TRK - MAX_SUPPORTED_PLANES, pJob, gTplGenericRead, cnt);

        paa = pJob->paa[cnt];
        actDie = GET_DIE_NUM_FROM_PAA(paa.b.plane, paa.b.lun, paa.b.ce, paa.b.ch);

        if (gOpenRow[actDie] != paa.rough.row)
        {
            descOffset = DescGeneric_TSB_MLC_OpenPage(pJob, actDie, paa, descOffset);
        }

        gTplGenericRead.field.JU             = cnt ? 0 : 1;
        gTplGenericRead.field.DEV            = paa.b.ce;
        gTplGenericRead.field.CH             = paa.b.ch;
        gTplGenericRead.field.FD             = descOffset ? 0: 1;
        gTplGenericRead.field.LD             = ((cnt == (pJob->paaCnt - 1)) || CHK_OFFSET_OVER_TH(descOffset, (MAX_DESC_PER_TRK - MAX_SUPPORTED_PLANES))) ? 1 : 0;
        gTplGenericRead.field.START_COL_ADDR = xferSize * paa.b.frag;
        gTplGenericRead.field.START_ROW_ADDR = SET_RW_DESC_ROW_FROM_PAA(paa);

        HalSeq_PushDescriptors((PVOID)&gTplGenericRead, sizeof(SeqGenericDescOr_t));

        descOffset++;
        pJob->descCnt++;
    }

    return pJob->descCnt;
}
//-----------------------------------------------------------------------------
/**
    General descriptor for erase operation

    @param[in]  pJob            Flash I/O Job pointer
    @param[in]  actDie          active die
    @param[in]  paa             paa

    @return descriptor count
**/
//-----------------------------------------------------------------------------
static __inline U16 DescGeneric_TSB_MLC_OpenPage(tFIO_JOB* pJob, U16 actDie, tPAA paa, U16 descOffset)
{
    gTplGenericOpenRow.field.TRACK_ID  = pJob->tid;
    gTplGenericOpenRow.field.AU_FORMAT = pJob->auFmt;
    gTplGenericOpenRow.field.CH        = paa.b.ch;
    gTplGenericOpenRow.field.DEV       = paa.b.ce;
    gTplGenericOpenRow.field.LD        = 0;

    if (paa.b.plane)
    {
        gTplGenericOpenRow.field.FD     = descOffset ? 0: 1;
        gTplGenericOpenRow.field.OPTION = SEQ_OPTION_0220C8;
        gTplGenericOpenRow.field.CMD2   = FC_READ_BYTE1;
        gTplGenericOpenRow.field.SCMD   = (paa.b.lun == 0)? FC_READ_STATUS_BYTE2 : FC_READ_STATUS_BYTE3;

        gTplGenericOpenRow.field.START_ROW_ADDR = SET_RW_DESC_ROW_FROM_PAA(paa);
        gOpenRow[actDie] = paa.rough.row;

        DESC_SET_OPEN_PG_BMP(pJob->tid, descOffset);
        HalSeq_PushDescriptors((PVOID)&gTplGenericOpenRow, sizeof(SeqGenericDescOr_t));
        descOffset++;
        pJob->descCnt++;
    }
    else
    {
        U32  idx;
        U16  die;
        U16  logBlk;
        U16  rmpBlk;
        U8   planeOffset;

        die    = GET_DIE_NUM(paa.b.lun, paa.b.ce, paa.b.ch);
        idx    = GET_SBLK_IDX(paa.b.plane, paa.b.block, die);
        rmpBlk = *(((U16*)smSysInfo->d.FtlRmpData.d.SBlkRmpInfo) + idx);
        logBlk = (rmpBlk == WORD_MASK) ? paa.b.block : rmpBlk;

        for (planeOffset = 0; planeOffset < pJob->plane; planeOffset++)
        {
            if (planeOffset)
            {
                paa.b.plane = planeOffset;

                idx    = GET_SBLK_IDX(paa.b.plane, logBlk, die);
                rmpBlk = *(((U16*)smSysInfo->d.FtlRmpData.d.SBlkRmpInfo) + idx);
                paa.b.block = (rmpBlk == WORD_MASK) ? logBlk : rmpBlk;
                actDie      = GET_DIE_NUM_FROM_PAA(paa.b.plane, paa.b.lun, paa.b.ce, paa.b.ch);
            }
            gTplGenericOpenRow.field.FD     = descOffset ? 0: 1;
            gTplGenericOpenRow.field.OPTION = (planeOffset == (pJob->plane - 1))? SEQ_OPTION_0220C8 : SEQ_OPTION_820080;
            gTplGenericOpenRow.field.CMD2   = (planeOffset == (pJob->plane - 1))? FC_READ_BYTE1     : FC_READ_MP_JEDEC;
            gTplGenericOpenRow.field.SCMD   = (paa.b.lun == 0)? FC_READ_STATUS_BYTE2 : FC_READ_STATUS_BYTE3;

            gTplGenericOpenRow.field.START_ROW_ADDR = SET_RW_DESC_ROW_FROM_PAA(paa);
            gOpenRow[actDie] = paa.rough.row;

            DESC_SET_OPEN_PG_BMP(pJob->tid, descOffset);
            HalSeq_PushDescriptors((PVOID)&gTplGenericOpenRow, sizeof(SeqGenericDescOr_t));
            descOffset++;
            pJob->descCnt++;
        }
    }
    return descOffset;
}

//-----------------------------------------------------------------------------
/**
    General descriptor for pre-open operation

    @param[in]  pJob            Flash I/O Job pointer

    @return descriptor count
**/
//-----------------------------------------------------------------------------
static U16 DescGeneric_TSB_MLC_PreOpen(tFIO_JOB* pJob)
{
    tPAA paa;
    U32  actDie;
    U8   planeOffset;
    U8   cnt;
    U8   totalCnt = (pJob->plane * pJob->paaCnt - 1);

    gTplGenericPreOpen.field.TRACK_ID  = pJob->tid;
    gTplGenericPreOpen.field.AU_FORMAT = pJob->auFmt;

    for (cnt = 0; cnt < pJob->paaCnt; cnt++)
    {
        paa = pJob->paa[cnt];

        gTplGenericPreOpen.field.CH  = paa.b.ch;
        gTplGenericPreOpen.field.DEV = paa.b.ce;

        actDie = GET_DIE_NUM_FROM_PAA(paa.b.plane, paa.b.lun, paa.b.ce, paa.b.ch);

        for (planeOffset = 0; planeOffset < pJob->plane; planeOffset++)
        {
            if (planeOffset)
            {
                paa.b.plane = planeOffset;

                actDie = GET_DIE_NUM_FROM_PAA(paa.b.plane, paa.b.lun, paa.b.ce, paa.b.ch);
            }

            gTplGenericPreOpen.field.FD     = pJob->descCnt ? 0: 1;
            gTplGenericPreOpen.field.LD     = (pJob->descCnt == totalCnt) ? 1 : 0;
            gTplGenericPreOpen.field.OPTION = (planeOffset == (pJob->plane - 1))? SEQ_OPTION_0220C8 : SEQ_OPTION_820080;
            gTplGenericPreOpen.field.CMD2   = (planeOffset == (pJob->plane - 1))? FC_READ_BYTE1     : FC_READ_MP_JEDEC;
            gTplGenericPreOpen.field.SCMD   = (paa.b.lun == 0)? FC_READ_STATUS_BYTE2 : FC_READ_STATUS_BYTE3;

            gTplGenericPreOpen.field.START_ROW_ADDR = SET_RW_DESC_ROW_FROM_PAA(paa);
            gOpenRow[actDie] = paa.rough.row;

            HalSeq_PushDescriptors((PVOID)&gTplGenericPreOpen, sizeof(SeqGenericDescOr_t));

            pJob->descCnt++;
        }
    }

    return pJob->descCnt;
}

//-----------------------------------------------------------------------------
/**
    General descriptor for fast-read operation

    @param[in]  pJob            Flash I/O Job pointer

    @return descriptor count
**/
//-----------------------------------------------------------------------------
static U16 DescGeneric_TSB_MLC_IdsRead(tFIO_JOB *pJob)
{
    tPAA paa;
    U32  actDie;
    U32  xferSize;
    U32  cnt;

    xferSize  = gEcuAuFormatList[pJob->auFmt].xferSizePerAu;

    gTplGenericIdsRead.field.TRACK_ID        = pJob->tid;
    gTplGenericIdsRead.field.XF_COUNT        = xferSize;
    gTplGenericIdsRead.field.AU_SEG          = pJob->segNum;
    gTplGenericIdsRead.field.HCMD_INDEX_HWLL = pJob->pMsgHost->attrInfo.b.cmdId;

    for (cnt = 0; cnt < pJob->paaCnt; cnt++)
    {
        paa = pJob->paa[cnt];

        actDie = GET_DIE_NUM_FROM_PAA(paa.b.plane, paa.b.lun, paa.b.ce, paa.b.ch);

        if (gOpenRow[actDie] != paa.rough.row)
        {
            gTplGenericIdsRead.field.OPTION = SEQ_OPTION_1400C8;
            gOpenRow[actDie] = paa.rough.row;
        }
        else
        {
            gTplGenericIdsRead.field.OPTION = SEQ_OPTION_140C03;
        }

        gTplGenericIdsRead.field.FD             = (pJob->descCnt) ? 0 : 1;
        gTplGenericIdsRead.field.LD             = (cnt == (pJob->paaCnt - 1)) ? 1 : 0;
        gTplGenericIdsRead.field.DEV            = paa.b.ce;
        gTplGenericIdsRead.field.CH             = paa.b.ch;
        gTplGenericIdsRead.field.SCMD           = (paa.b.lun == 0) ? FC_READ_STATUS_BYTE2 : FC_READ_STATUS_BYTE3;
        gTplGenericIdsRead.field.AU_INDEX       = cnt + pJob->paaOffset;
        gTplGenericIdsRead.field.START_COL_ADDR = (paa.b.frag * xferSize);
        gTplGenericIdsRead.field.START_ROW_ADDR = SET_RW_DESC_ROW_FROM_PAA(paa);

        HalSeq_PushDescriptors((PVOID)&gTplGenericIdsRead, sizeof(SeqGenericDescOr_t));

        pJob->descCnt++;
    }

    return pJob->descCnt;
}

//-----------------------------------------------------------------------------
/**
    General descriptor for fast-read IDS operation

    @param[in]  pJob            Flash I/O Job pointer

    @return descriptor count
**/
//-----------------------------------------------------------------------------
static U16 DescGeneric_TSB_MLC_IdsPageRead(tFIO_JOB *pJob)
{
    tPAA paa;
    U32  actDie;
    U32  cnt;
    U32  totalCnt;
    U16  descOffset = 0;

    DESC_RST_OPEN_PG_BMP(pJob->tid);

    totalCnt = (pJob->paaCnt >> 2);

    gTplGenericIdsPageRead.field.TRACK_ID        = pJob->tid;
    gTplGenericIdsPageRead.field.XF_COUNT        = (gEcuAuFormatList[pJob->auFmt].xferSizePerAu << 2);
    gTplGenericIdsPageRead.field.AU_SEG          = pJob->segNum;
    gTplGenericIdsPageRead.field.HCMD_INDEX_HWLL = pJob->pMsgHost->attrInfo.b.cmdId;

    for (cnt = 0; cnt < totalCnt; cnt++)
    {
        paa = pJob->paa[cnt << 2];
        actDie = GET_DIE_NUM_FROM_PAA(paa.b.plane, paa.b.lun, paa.b.ce, paa.b.ch);

        if (gOpenRow[actDie] == paa.rough.row)
        {
            gTplGenericIdsPageRead.field.OPTION = SEQ_OPTION_140C03;
        }
        else
        {
            if (paa.b.plane == 0 && pJob->plane == gNfInfo.planesPerLun)
            {
                descOffset = DescGeneric_TSB_MLC_OpenPage(pJob, actDie, paa, descOffset);
                gTplGenericIdsPageRead.field.OPTION = SEQ_OPTION_140C03;
            }
            else
            {
                gTplGenericIdsPageRead.field.OPTION = SEQ_OPTION_1400C8;
                gOpenRow[actDie] = paa.rough.row;
            }
        }

        gTplGenericIdsPageRead.field.FD             = (pJob->descCnt) ? 0 : 1;
        gTplGenericIdsPageRead.field.LD             = (cnt == (totalCnt - 1)) ? 1 : 0;
        gTplGenericIdsPageRead.field.DEV            = paa.b.ce;
        gTplGenericIdsPageRead.field.CH             = paa.b.ch;
        gTplGenericIdsPageRead.field.SCMD           = (paa.b.lun == 0) ? FC_READ_STATUS_BYTE2 : FC_READ_STATUS_BYTE3;
        gTplGenericIdsPageRead.field.AU_INDEX       = pJob->paaOffset + (cnt << 2);
        gTplGenericIdsPageRead.field.START_ROW_ADDR = SET_RW_DESC_ROW_FROM_PAA(paa);

        HalSeq_PushDescriptors((PVOID)&gTplGenericIdsPageRead, sizeof(SeqGenericDescOr_t));
        descOffset++;
        pJob->descCnt++;
    }

    return pJob->descCnt;
}

//-----------------------------------------------------------------------------
/**
    General descriptor for fast-read operation

    @param[in]  pJob            Flash I/O Job pointer

    @return descriptor count
**/
//-----------------------------------------------------------------------------
static U16 DescGeneric_TSB_MLC_FastRead(tFIO_JOB *pJob)
{
    tPAA paa;
    U32  actDie;
    U16  xferSize;
    U16  descOffset = 0;
    U16  cnt;

    xferSize = gEcuAuFormatList[pJob->auFmt].xferSizePerAu;

    gTplGenericFastRead.field.LE            = (pJob->flag & FIO_F_AUX) ? 1 : 0;
    gTplGenericFastRead.field.START_SSD_PTR = pJob->ssdIdx[pJob->ssdList];
    gTplGenericFastRead.field.TRACK_ID      = pJob->tid;
    gTplGenericFastRead.field.AU_SEG        = pJob->segNum;
    gTplGenericFastRead.field.AU_FORMAT     = pJob->auFmt;
    gTplGenericFastRead.field.XF_COUNT      = xferSize;

    for (cnt = 0; cnt < pJob->paaCnt; cnt++)
    {
        APLDESC_ALLOC_MULTI_TRK(descOffset, MAX_DESC_PER_TRK, pJob, gTplGenericFastRead, cnt);
        paa = pJob->paa[cnt];

        actDie = GET_DIE_NUM_FROM_PAA(paa.b.plane, paa.b.lun, paa.b.ce, paa.b.ch);

        if (gOpenRow[actDie] == paa.rough.row)
        {
            gTplGenericFastRead.field.OPTION = SEQ_OPTION_140C03;
            gTplGenericFastRead.field.BM     = 0;
        }
        else
        {
            if (pJob->flag & FIO_F_NANDTIME)
            {
                gTplGenericFastRead.field.BM     = 1;
                gTplGenericFastRead.field.OPTION = SEQ_OPTION_140080;

            }
            else
            {
                gTplGenericFastRead.field.BM     = 0;
                gTplGenericFastRead.field.OPTION = SEQ_OPTION_1400C8;
            }
            gOpenRow[actDie] = paa.rough.row;
        }

        gTplGenericFastRead.field.FD              = descOffset ? 0 : 1;
        gTplGenericFastRead.field.JU              = cnt ? 0 : 1;
        gTplGenericFastRead.field.LD              = ((cnt == (pJob->paaCnt - 1)) || CHK_OFFSET_OVER_TH(descOffset, MAX_DESC_PER_TRK)) ? 1 : 0;
        gTplGenericFastRead.field.DEV             = paa.b.ce;
        gTplGenericFastRead.field.CH              = paa.b.ch;
        gTplGenericFastRead.field.SCMD            = (paa.b.lun == 0) ? FC_READ_STATUS_BYTE2 : FC_READ_STATUS_BYTE3;
        gTplGenericFastRead.field.START_COL_ADDR  = paa.b.frag * xferSize;
        gTplGenericFastRead.field.START_ROW_ADDR  = SET_RW_DESC_ROW_FROM_PAA(paa);

        HalSeq_PushDescriptors((PVOID)&gTplGenericFastRead, sizeof(SeqGenericDescOr_t));
        descOffset++;
        pJob->descCnt++;
    }

    return pJob->descCnt;
}

//-----------------------------------------------------------------------------
/**
    General descriptor for SLC fast-read operation

    @param[in]  pJob            Flash I/O Job pointer

    @return descriptor count
**/
//-----------------------------------------------------------------------------
static U16 DescGeneric_TSB_MLC_SlcRead(tFIO_JOB *pJob)
{
    tPAA paa;
    U16 xferSize;
    U16 totalCnt;
    U16 cnt;

    MEM_SET(gOpenRow, BYTE_MASK, sizeof(gOpenRow));

    xferSize = gEcuAuFormatList[pJob->auFmt].xferSizePerAu << 2;
    totalCnt  = (pJob->paaCnt >> 2);

    gTplGenericSlcRead.field.LE            = (pJob->flag & FIO_F_AUX) ? 1 : 0;
    gTplGenericSlcRead.field.START_SSD_PTR = pJob->ssdIdx[pJob->ssdList];
    gTplGenericSlcRead.field.TRACK_ID      = pJob->tid;
    gTplGenericSlcRead.field.AU_SEG        = pJob->segNum;
    gTplGenericSlcRead.field.AU_FORMAT     = pJob->auFmt;
    gTplGenericSlcRead.field.XF_COUNT      = xferSize;

    for (cnt = 0; cnt < totalCnt; cnt++)
    {
        paa = pJob->paa[cnt << 2];

        gTplGenericSlcRead.field.FD             = (pJob->descCnt) ? 0 : 1;
        gTplGenericSlcRead.field.JU             = cnt ? 0 : 1;
        gTplGenericSlcRead.field.LD             = (cnt == (totalCnt - 1)) ? 1 : 0;
        gTplGenericSlcRead.field.DEV            = paa.b.ce;
        gTplGenericSlcRead.field.CH             = paa.b.ch;
        gTplGenericSlcRead.field.SCMD           = (paa.b.lun == 0) ? FC_READ_STATUS_BYTE2 : FC_READ_STATUS_BYTE3;
        gTplGenericSlcRead.field.START_ROW_ADDR = SET_RW_DESC_ROW_FROM_PAA(paa);

        HalSeq_PushDescriptors((PVOID)&gTplGenericSlcRead, sizeof(SeqGenericDescOr_t));
        pJob->descCnt++;
    }

    return pJob->descCnt;
}

//-----------------------------------------------------------------------------
/**
    General descriptor for read retry IDS operation

    @param[in]  pJob            Flash I/O Job pointer

    @return descriptor count
**/
//-----------------------------------------------------------------------------
static U16 DescGeneric_TSB_MLC_RR(tFIO_JOB *pJob)
{
    tPAA vth;
    tPAA paa;
    U16  xferSize;
    U16  cnt;
    U16  descOffset = 0;
    U16  paaCnt = pJob->paaCnt / 2;
    U16  actDie;

    xferSize = gEcuAuFormatList[pJob->auFmt].xferSizePerAu;

    if (pJob->css == CACHE_R_IDS)
    {
        gTplGenericReadRetry.field.HWLL            = 1;
        gTplGenericReadRetry.field.HCMD_INDEX_HWLL = pJob->pMsgHost->attrInfo.b.cmdId;
    }
    else
    {
        gTplGenericReadRetry.field.HWLL            = 0;
        gTplGenericReadRetry.field.HCMD_INDEX_HWLL = 0;
        gTplGenericReadRetry.field.AU_INDEX        = 0;
        gTplGenericReadRetry.field.START_SSD_PTR   = pJob->ssdIdx[pJob->ssdList];
    }

    gTplGenericReadRetry.field.AU_SEG    = pJob->segNum;
    gTplGenericReadRetry.field.LE        = (pJob->flag & FIO_F_AUX) ? 1 : 0;
    gTplGenericReadRetry.field.AU_FORMAT = pJob->auFmt;
    gTplGenericReadRetry.field.XF_COUNT  = xferSize;

    for (cnt = 0; cnt < paaCnt; cnt++)
    {
        vth = pJob->paa[cnt * 2];
        paa = pJob->paa[cnt * 2 + 1];

        actDie = GET_DIE_NUM_FROM_PAA(paa.b.plane, paa.b.lun, paa.b.ce, paa.b.ch);
        gOpenRow[actDie] = DWORD_MASK;

        APLDESC_ALLOC_MULTI_TRK(descOffset, (MAX_DESC_PER_TRK - 1), pJob, gTplGenericVoltageShift, cnt);

        gTplGenericVoltageShift.field.TRACK_ID       = pJob->tid;
        gTplGenericVoltageShift.field.DEV            = paa.b.ce;
        gTplGenericVoltageShift.field.CH             = paa.b.ch;
        gTplGenericVoltageShift.field.FD             = descOffset ? 0 : 1;
        gTplGenericVoltageShift.field.LD             = 0;
        gTplGenericVoltageShift.field.START_COL_ADDR = (paa.b.lun | 0x89 << 8);
        gTplGenericVoltageShift.field.SET_PARAM      = GET_RETRY_VOLTAGE_STAGE(vth.rr.voltage);
        gTplGenericVoltageShift.field.SCMD           = (paa.b.lun == 0) ? FC_READ_STATUS_BYTE2 : FC_READ_STATUS_BYTE3;
        gTplGenericVoltageShift.field.START_ROW_ADDR = SET_RW_DESC_ROW_FROM_PAA(paa);

        HalSeq_PushDescriptors((PVOID)&gTplGenericVoltageShift, sizeof(SeqGenericDescOr_t));

        descOffset++;
        pJob->descCnt++;

        //-----------------------------------------------------------------------------
        gTplGenericReadRetry.field.TRACK_ID       = pJob->tid;
        gTplGenericReadRetry.field.FD             = 0;
        gTplGenericReadRetry.field.LD             = 0;
        gTplGenericReadRetry.field.DEV            = paa.b.ce;
        gTplGenericReadRetry.field.CH             = paa.b.ch;
        gTplGenericReadRetry.field.SCMD           = (paa.b.lun == 0) ? FC_READ_STATUS_BYTE2 : FC_READ_STATUS_BYTE3;
        gTplGenericReadRetry.field.START_COL_ADDR = paa.b.frag * xferSize;
        gTplGenericReadRetry.field.START_ROW_ADDR = SET_RW_DESC_ROW_FROM_PAA(paa);

        if (cnt && pJob->paa[(cnt - 1) * 2].rough2.row == paa.rough2.row)
        {
            gTplGenericReadRetry.field.OPTION = SEQ_OPTION_148C03;
        }
        else
        {
            gTplGenericReadRetry.field.OPTION = SEQ_OPTION_1480C8;
        }

        if (pJob->css == CACHE_R_IDS)   gTplGenericReadRetry.field.AU_INDEX = pJob->paaOffset + vth.rr.srcIdx;
        else                            gTplGenericReadRetry.field.JU = cnt ? 0 : 1;

        HalSeq_PushDescriptors((PVOID)&gTplGenericReadRetry, sizeof(SeqGenericDescOr_t));

        descOffset++;
        pJob->descCnt++;

        //-----------------------------------------------------------------------------
        gTplGenericVoltageShift.field.FD             = 0;
        gTplGenericVoltageShift.field.LD             = ((cnt == paaCnt - 1) || CHK_OFFSET_OVER_TH(descOffset, (MAX_DESC_PER_TRK - 1))) ? 1 : 0;
        gTplGenericVoltageShift.field.SET_PARAM      = 0x00;

        HalSeq_PushDescriptors((PVOID)&gTplGenericVoltageShift, sizeof(SeqGenericDescOr_t));

        descOffset++;
        pJob->descCnt++;
    }

    return pJob->descCnt;
}

//-----------------------------------------------------------------------------
/**
    General descriptor for no support operation

    @param[in]  pJob            Flash I/O Job pointer

    @return descriptor count
**/
//-----------------------------------------------------------------------------
static U16 DescGeneric_TSB_MLC_Exception(tFIO_JOB* pJob)
{
    D_PRINTF("Desc CMD(%d) Not Support!\n", pJob->cmd);
    ASSERT(0);
    return pJob->descCnt;
}

//-----------------------------------------------------------------------------
/**
    General descriptor for two bit decode operation

    @param[in]  pJob            Flash I/O Job pointer

    @return descriptor count
**/
//-----------------------------------------------------------------------------
static U16 DescGeneric_TSB_MLC_NormalTwoBitDecode(tFIO_JOB *pJob)
{
    tPAA paa;
    U16  cnt;
    U8   fd, ju;

    for (cnt = 0; cnt < pJob->paaCnt; cnt++)
    {
        paa = pJob->paa[cnt];
        fd = cnt ?  0 : 1;
        ju = cnt ?  0 : 1;

        DescGeneric_TSB_MLC_HD_Retry_CW(pJob, 0, fd, ju, paa);  // pjob, n-th CW, FD, JU

        /* CW0 SD 5C C5 + SOFT READ LEVEL SET */
        DescGeneric_TSB_MLC_SD_Retry_CW(pJob, 0, paa);  //pjob, n-th CW

        /* CW 1 SD  5C C5 + SOFT READ LEVEL SET */
        DescGeneric_TSB_MLC_HD_Retry_CW(pJob, 1, 0, 0, paa); // pjob, n-th CW, FD, JU

        DescGeneric_TSB_MLC_SD_Retry_CW(pJob, 1, paa);   //pjob, n-th CW
    }

    // ======== Final Set back HD retry level set========
    gTplGenericVoltageShift.field.TRACK_ID      = pJob->tid;

    for (cnt = 0; cnt < pJob->paaCnt; cnt++)
    {
        paa = pJob->paa[cnt];

        gTplGenericVoltageShift.field.DEV            = paa.b.ce;
        gTplGenericVoltageShift.field.CH             = paa.b.ch;

        gTplGenericVoltageShift.field.FD             = 0;
        gTplGenericVoltageShift.field.LD             = (cnt == pJob->paaCnt - 1) ? 1 : 0;;
        gTplGenericVoltageShift.field.OPTION         = SEQ_OPTION_020919;
        gTplGenericVoltageShift.field.CMD1           = 0xD5;
        gTplGenericVoltageShift.field.START_COL_ADDR = (paa.b.lun | 0x89 << 8);
        gTplGenericVoltageShift.field.SET_PARAM      = 0x00;
        gTplGenericVoltageShift.field.TWOBT0         = 1;
        gTplGenericVoltageShift.field.SCMD           = (paa.b.lun == 0) ? FC_READ_STATUS_BYTE2 : FC_READ_STATUS_BYTE3;

        HalSeq_PushDescriptors((PVOID)&gTplGenericVoltageShift, sizeof(SeqGenericDescOr_t));
        pJob->descCnt++;
    }

    gTplGenericVoltageShift.field.TWOBT0 = 0;

    // ======Final Set back HD retry level set end ======
    return pJob->descCnt;
}

//-----------------------------------------------------------------------------
/**
    General descriptor for CW retry operation
**/
//-----------------------------------------------------------------------------
static __inline void DescGeneric_TSB_MLC_HD_Retry_CW(tFIO_JOB* pJob, U8 fg_cw, U8 fg_fd, U8 fg_JU, tPAA paa)
{
    U16  xferSize;

    xferSize = gEcuAuFormatList[pJob->auFmt].xferSizePerAu;

    // ======== CW 0 HD retry level set========

    gTplGenericVoltageShift.field.TRACK_ID = pJob->tid;

    gTplGenericVoltageShift.field.DEV      = paa.b.ce;
    gTplGenericVoltageShift.field.CH       = paa.b.ch;

    gTplGenericVoltageShift.field.FD       = fg_fd ;
    gTplGenericVoltageShift.field.LD       = 0;

    gTplGenericVoltageShift.field.OPTION         = SEQ_OPTION_020919;//0x20801;
    gTplGenericVoltageShift.field.CMD1           = 0xD5;
    gTplGenericVoltageShift.field.START_COL_ADDR = (paa.b.lun | 0x89 << 8);
    gTplGenericVoltageShift.field.SET_PARAM      = 0x00;
    gTplGenericVoltageShift.field.TWOBT0         = 1;
    gTplGenericVoltageShift.field.SCMD           = (paa.b.lun == 0) ? FC_READ_STATUS_BYTE2 : FC_READ_STATUS_BYTE3;

    HalSeq_PushDescriptors((PVOID)&gTplGenericVoltageShift, sizeof(SeqGenericDescOr_t));
    pJob->descCnt++;

    gTplGenericVoltageShift.field.TWOBT0 = 0;

    // ======CW 0 HD retry level set end ======


    // ======CW 0 HD retry read ======
    gTplGenericReadRetry.field.TRACK_ID      = pJob->tid;
    gTplGenericReadRetry.field.LE            = (pJob->flag & FIO_F_AUX) ? 1 : 0;
    gTplGenericReadRetry.field.AU_SEG        = pJob->segNum;
    gTplGenericReadRetry.field.START_SSD_PTR = pJob->ssdIdx[pJob->ssdList];
    gTplGenericReadRetry.field.AU_FORMAT     = pJob->auFmt;
    gTplGenericReadRetry.field.XF_COUNT      = fg_cw ? gCwInfo.cw2size : gCwInfo.cw1size;

    gTplGenericReadRetry.field.FD              = 0;
    gTplGenericReadRetry.field.JU              = fg_JU;
    gTplGenericReadRetry.field.LD              = 0;
    gTplGenericReadRetry.field.DEV             = paa.b.ce;
    gTplGenericReadRetry.field.CH              = paa.b.ch;
    gTplGenericReadRetry.field.SCMD            = (paa.b.lun == 0) ? FC_READ_STATUS_BYTE2 : FC_READ_STATUS_BYTE3;
    gTplGenericReadRetry.field.START_COL_ADDR  = fg_cw ? paa.b.frag * xferSize + gCwInfo.cw1size : paa.b.frag * xferSize;
    gTplGenericReadRetry.field.START_ROW_ADDR  = SET_RW_DESC_ROW_FROM_PAA(paa);
    gTplGenericReadRetry.field.ZERO            = fg_cw ? 1 : 0;
    gTplGenericReadRetry.field.TWOBT0          = 1 ;

    HalSeq_PushDescriptors((PVOID)&gTplGenericReadRetry, sizeof(SeqGenericDescOr_t));
    pJob->descCnt++;

    gTplGenericReadRetry.field.ZERO   = 0;
    gTplGenericReadRetry.field.TWOBT0 = 0;

    // ======CW 0 HD retry read END ======
}

//-----------------------------------------------------------------------------
/**
    General descriptor for CW retry operation
**/
//-----------------------------------------------------------------------------
static __inline void DescGeneric_TSB_MLC_SD_Retry_CW(tFIO_JOB* pJob, U8 fg_cw, tPAA paa)
{
    U16  xferSize;

    xferSize = gEcuAuFormatList[pJob->auFmt].xferSizePerAu;

    gTplGenericFlexDescOr.field.TRACK_ID       = pJob->tid;

    gTplGenericFlexDescOr.field.START_CMD_PTR  = 0;
    gTplGenericFlexDescOr.field.DEV            = paa.b.ce;
    gTplGenericFlexDescOr.field.CH             = paa.b.ch;
    gTplGenericFlexDescOr.field.FD             = 0;
    gTplGenericFlexDescOr.field.LD             = 0;
    gTplGenericFlexDescOr.field.DQ0            = 0x00;
    gTplGenericFlexDescOr.field.DQ1            = 0;
    gTplGenericFlexDescOr.field.DQ2            = 0x7B;
    gTplGenericFlexDescOr.field.DQ3            = 5;
    gTplGenericFlexDescOr.field.DQ4            = 0x7B;
    gTplGenericFlexDescOr.field.DQ5            = 5;

    gTplGenericFlexDescOr.field.DQ6            = 0;
    gTplGenericFlexDescOr.field.DQ7            = 0;
    gTplGenericFlexDescOr.field.DQ8            = 0;
    gTplGenericFlexDescOr.field.DQ9            = 0;
    gTplGenericFlexDescOr.field.DQ10           = 0;
    gTplGenericFlexDescOr.field.DQ11           = 0;
    gTplGenericFlexDescOr.field.TWOBT0         = 1;

    HalSeq_PushDescriptors((PVOID)&gTplGenericFlexDescOr, sizeof(SeqGenericFlexDescOr_t));
    pJob->descCnt++;
    gTplGenericFlexDescOr.field.TWOBT0         = 0; //set back

    /* C2 Xor */
    gTplGenericSoftBitXor.field.TRACK_ID       = pJob->tid;
    gTplGenericSoftBitXor.field.AU_SEG         = pJob->segNum;
    gTplGenericSoftBitXor.field.START_SSD_PTR  = pJob->ssdIdx[pJob->ssdList];
    gTplGenericSoftBitXor.field.AU_FORMAT      = pJob->auFmt;
    gTplGenericSoftBitXor.field.XF_COUNT       = 0;
    gTplGenericSoftBitXor.field.AU_NUM         = 0;
    gTplGenericSoftBitXor.field.LE             = (pJob->flag & FIO_F_AUX) ? 1 : 0;

    gTplGenericSoftBitXor.field.JU             = 0;
    gTplGenericSoftBitXor.field.DEV            = paa.b.ce;
    gTplGenericSoftBitXor.field.CH             = paa.b.ch;
    gTplGenericSoftBitXor.field.FD             = 0;
    gTplGenericSoftBitXor.field.LD             = 0;
    gTplGenericSoftBitXor.field.TWOBT0         = 1;
    gTplGenericSoftBitXor.field.START_COL_ADDR = 0;
    gTplGenericSoftBitXor.field.START_ROW_ADDR = SET_RW_DESC_ROW_FROM_PAA(paa);

    HalSeq_PushDescriptors((PVOID)&gTplGenericSoftBitXor, sizeof(SeqGenericDescOr_t));
    pJob->descCnt++;

    gTplGenericSoftBitXor.field.TWOBT0         = 0; // set back

    /* CW 0 SD 00 05 E0 */
    gTplGenericRandomCacheRead.field.TRACK_ID      = pJob->tid;
    gTplGenericRandomCacheRead.field.AU_SEG        = pJob->segNum;
    gTplGenericRandomCacheRead.field.START_SSD_PTR = pJob->ssdIdx[pJob->ssdList];
    gTplGenericRandomCacheRead.field.AU_FORMAT     = pJob->auFmt;
    gTplGenericRandomCacheRead.field.XF_COUNT      = fg_cw ? gCwInfo.cw2size: gCwInfo.cw1size;
    gTplGenericRandomCacheRead.field.LE            = (pJob->flag & FIO_F_AUX) ? 1 : 0;
    gTplGenericRandomCacheRead.field.LC            = 0;

    gTplGenericRandomCacheRead.field.JU            = 0;
    gTplGenericRandomCacheRead.field.DEV           = paa.b.ce;
    gTplGenericRandomCacheRead.field.CH            = paa.b.ch;
    gTplGenericRandomCacheRead.field.FD            = 0;
    gTplGenericRandomCacheRead.field.LD            = 0;
    gTplGenericRandomCacheRead.field.ZERO          = 1;
    gTplGenericRandomCacheRead.field.TWOBT0        = 1;

    gTplGenericRandomCacheRead.field.START_COL_ADDR = fg_cw ? xferSize * paa.b.frag + gCwInfo.cw1size : xferSize * paa.b.frag;
    gTplGenericRandomCacheRead.field.START_ROW_ADDR = SET_RW_DESC_ROW_FROM_PAA(paa);

    HalSeq_PushDescriptors((PVOID)&gTplGenericRandomCacheRead, sizeof(SeqGenericDescOr_t));

    pJob->descCnt++;

    gTplGenericRandomCacheRead.field.ZERO        = 0;
    gTplGenericRandomCacheRead.field.TWOBT0      = 0;

     // 0xFF
    gTplGenericExitTestMode.field.TRACK_ID       = pJob->tid;
    gTplGenericExitTestMode.field.AU_SEG         = pJob->segNum;
    gTplGenericExitTestMode.field.START_SSD_PTR  = pJob->ssdIdx[pJob->ssdList];
    gTplGenericExitTestMode.field.AU_FORMAT      = pJob->auFmt;
    gTplGenericExitTestMode.field.XF_COUNT       = 0;
    gTplGenericExitTestMode.field.AU_NUM         = 0; // 4K
    gTplGenericExitTestMode.field.LE             = (pJob->flag & FIO_F_AUX) ? 1 : 0;

    gTplGenericExitTestMode.field.JU             = 0;
    gTplGenericExitTestMode.field.DEV            = paa.b.ce;
    gTplGenericExitTestMode.field.CH             = paa.b.ch;
    gTplGenericExitTestMode.field.FD             = 0;
    gTplGenericExitTestMode.field.LD             = 0;

    gTplGenericExitTestMode.field.START_COL_ADDR = 0;
    gTplGenericExitTestMode.field.START_ROW_ADDR = SET_RW_DESC_ROW_FROM_PAA(paa);

    HalSeq_PushDescriptors((PVOID)&gTplGenericExitTestMode, sizeof(SeqGenericDescOr_t));
    pJob->descCnt++;
    /* CW 0 SD 5C C5 + SOFT READ LEVEL SET end */
}

//=============================================================================
//-----------------------------------------------------------------------------
/**
    General descriptor template

    @param[in] N/A

    @return N/A
**/
//-----------------------------------------------------------------------------
static __inline void DescGeneric_TSB_MLC_InitTemplate(void)
{
    // Flex Generic
    MEM_CLR(&gTplGenericFlexDescOr,sizeof(SeqGenericFlexDescOr_t));
    gTplGenericFlexDescOr.field.DF       = DESC_FMT_GENERIC;
    gTplGenericFlexDescOr.field.DES_TYPE = DESC_CMD_READ_STATUS;
    gTplGenericFlexDescOr.field.OR1      = 1;
    gTplGenericFlexDescOr.field.DV       = 1;
    gTplGenericFlexDescOr.field.PA       = 1;

    // Erase
    MEM_CLR(&gTplGenericErase, sizeof(SeqGenericDescOr_t));
    gTplGenericErase.field.DF          = DESC_FMT_GENERIC;
    gTplGenericErase.field.DV          = 1;
    gTplGenericErase.field.DES_TYPE    = DESC_CMD_MLUN | DESC_CMD_PROG;
    gTplGenericErase.field.CMD1        = FC_ERASE_BYTE0;
    gTplGenericErase.field.CMD2        = FC_ERASE_BYTE1;
    gTplGenericErase.field.READY       = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericErase.field.OR1         = 1;
    gTplGenericErase.field.RAID_OFFSET = 7;

    // Write
    MEM_CLR(&gTplGenericWrite, sizeof(SeqGenericDescOr_t));
    gTplGenericWrite.field.DES_TYPE       = DESC_CMD_MLUN | DESC_CMD_PROG;
    gTplGenericWrite.field.DF             = DESC_FMT_GENERIC;
    gTplGenericWrite.field.READY          = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericWrite.field.DV             = 1;
    gTplGenericWrite.field.OR1            = 1;
    gTplGenericWrite.field.START_COL_ADDR = 0;
    gTplGenericWrite.field.RAID_OFFSET    = 7;

   // OpenRow
    MEM_CLR(&gTplGenericOpenRow, sizeof(SeqGenericDescOr_t));
    gTplGenericOpenRow.field.AU_NUM      = 0;
    gTplGenericOpenRow.field.CMD1        = FC_READ_BYTE0;
    gTplGenericOpenRow.field.DES_TYPE    = DESC_CMD_MLUN | DESC_CMD_READ;
    gTplGenericOpenRow.field.DF          = DESC_FMT_GENERIC;
    gTplGenericOpenRow.field.READY       = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericOpenRow.field.DV          = 1;
    gTplGenericOpenRow.field.OR1         = 1;
    gTplGenericOpenRow.field.RAID_OFFSET = 7;

    // Read
    MEM_CLR(&gTplGenericRead, sizeof(SeqGenericDescOr_t));
    gTplGenericRead.field.AU_NUM      = 0;
    gTplGenericRead.field.DES_TYPE    = DESC_CMD_MLUN | DESC_CMD_READ;
    gTplGenericRead.field.DF          = DESC_FMT_GENERIC;
    gTplGenericRead.field.READY       = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericRead.field.DV          = 1;
    gTplGenericRead.field.CMD1        = FC_READ_BYTE4_JEDEC;
    gTplGenericRead.field.CMD2        = FC_READ_BYTE5;
    gTplGenericRead.field.OPTION      = SEQ_OPTION_140C03;
    gTplGenericRead.field.OR1         = 1;
    gTplGenericRead.field.RAID_OFFSET = 7;

    // fast Read
    MEM_CLR(&gTplGenericFastRead, sizeof(SeqGenericDescOr_t));
    gTplGenericFastRead.field.DES_TYPE    = DESC_CMD_MLUN | DESC_CMD_READ;
    gTplGenericFastRead.field.DF          = DESC_FMT_GENERIC;
    gTplGenericFastRead.field.READY       = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericFastRead.field.DV          = 1;
    gTplGenericFastRead.field.CMD1        = FC_READ_BYTE0;
    gTplGenericFastRead.field.CMD2        = FC_READ_BYTE1;
    gTplGenericFastRead.field.OPTION      = SEQ_OPTION_1400C8;
    gTplGenericFastRead.field.AU_NUM      = 0;
    gTplGenericFastRead.field.OR1         = 1;
    gTplGenericFastRead.field.RAID_OFFSET = 7;

    // Slc Read
    MEM_CLR(&gTplGenericSlcRead, sizeof(SeqGenericDescOr_t));
    gTplGenericSlcRead.field.DES_TYPE    = DESC_CMD_MLUN | DESC_CMD_READ;
    gTplGenericSlcRead.field.DF          = DESC_FMT_GENERIC;
    gTplGenericSlcRead.field.READY       = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericSlcRead.field.DV          = 1;
    gTplGenericSlcRead.field.PREFIX_3B   = FC_PREFIX_SLC_BYTE;
    gTplGenericSlcRead.field.CMD1        = FC_READ_BYTE0;
    gTplGenericSlcRead.field.CMD2        = FC_READ_BYTE1;
    gTplGenericSlcRead.field.OPTION      = SEQ_OPTION_1480C8;
    gTplGenericSlcRead.field.AU_NUM      = 3;
    gTplGenericSlcRead.field.OR1         = 1;
    gTplGenericSlcRead.field.RAID_OFFSET = 7;

    // Voltage Shift
    MEM_CLR(&gTplGenericVoltageShift, sizeof(SeqGenericDescOr_t));
    gTplGenericVoltageShift.field.DES_TYPE    = DESC_CMD_PROG;
    gTplGenericVoltageShift.field.DF          = DESC_FMT_GENERIC;
    gTplGenericVoltageShift.field.DV          = 1;
    gTplGenericVoltageShift.field.PA          = 1;
    gTplGenericVoltageShift.field.XF_COUNT    = 8;
    gTplGenericVoltageShift.field.READY       = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericVoltageShift.field.OR1         = 1;
    gTplGenericVoltageShift.field.OPTION      = SEQ_OPTION_0208D9;
    gTplGenericVoltageShift.field.CMD1        = 0xD5;
    gTplGenericVoltageShift.field.RAID_OFFSET = 7;

    // Read Retry
    MEM_CLR(&gTplGenericReadRetry, sizeof(SeqGenericDescOr_t));
    gTplGenericReadRetry.field.PREFIX_3B   = 0x26;
    gTplGenericReadRetry.field.DES_TYPE    = DESC_CMD_READ;
    gTplGenericReadRetry.field.DF          = DESC_FMT_GENERIC;
    gTplGenericReadRetry.field.READY       = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericReadRetry.field.DV          = 1;
    gTplGenericReadRetry.field.CMD1        = FC_READ_BYTE0;
    gTplGenericReadRetry.field.CMD2        = FC_READ_BYTE1;
    gTplGenericReadRetry.field.AU_NUM      = 0;
    gTplGenericReadRetry.field.OR1         = 1;
    gTplGenericReadRetry.field.OPTION      = SEQ_OPTION_1480C8;
    gTplGenericReadRetry.field.RAID_OFFSET = 7;

    // PreOpen
    MEM_CLR(&gTplGenericPreOpen, sizeof(SeqGenericDescOr_t));
    gTplGenericPreOpen.field.AU_NUM      = 0;
    gTplGenericPreOpen.field.CMD1        = FC_READ_BYTE0;
    gTplGenericPreOpen.field.DES_TYPE    = DESC_CMD_MLUN | DESC_CMD_READ;
    gTplGenericPreOpen.field.DF          = DESC_FMT_GENERIC;
    gTplGenericPreOpen.field.READY       = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericPreOpen.field.DV          = 1;
    gTplGenericPreOpen.field.OR1         = 1;
    gTplGenericPreOpen.field.HWLL        = 0;
    gTplGenericPreOpen.field.RAID_OFFSET = 7;

    // IDS Read
    MEM_CLR(&gTplGenericIdsRead, sizeof(SeqGenericDescOr_t));
    gTplGenericIdsRead.field.AU_NUM      = 0;
    gTplGenericIdsRead.field.DES_TYPE    = DESC_CMD_MLUN | DESC_CMD_READ;
    gTplGenericIdsRead.field.READY       = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericIdsRead.field.DV          = 1;
    gTplGenericIdsRead.field.DF          = DESC_FMT_GENERIC;
    gTplGenericIdsRead.field.CMD1        = FC_READ_BYTE0;
    gTplGenericIdsRead.field.CMD2        = FC_READ_BYTE1;
    gTplGenericIdsRead.field.OPTION      = SEQ_OPTION_1400C8;
    gTplGenericIdsRead.field.OR1         = 1;
    gTplGenericIdsRead.field.HWLL        = 1;
    gTplGenericIdsRead.field.RAID_OFFSET = 7;

    // IDS Page Read
    MEM_CLR(&gTplGenericIdsPageRead, sizeof(SeqGenericDescOr_t));
    gTplGenericIdsPageRead.field.AU_NUM      = 3;
    gTplGenericIdsPageRead.field.DES_TYPE    = DESC_CMD_MLUN | DESC_CMD_READ;
    gTplGenericIdsPageRead.field.READY       = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericIdsPageRead.field.DV          = 1;
    gTplGenericIdsPageRead.field.DF          = DESC_FMT_GENERIC;
    gTplGenericIdsPageRead.field.CMD1        = FC_READ_BYTE0;
    gTplGenericIdsPageRead.field.CMD2        = FC_READ_BYTE1;
    gTplGenericIdsPageRead.field.OPTION      = SEQ_OPTION_1400C8;
    gTplGenericIdsPageRead.field.OR1         = 1;
    gTplGenericIdsPageRead.field.HWLL        = 1;
    gTplGenericIdsPageRead.field.RAID_OFFSET = 7;

    //Random Cache Read
    MEM_CLR(&gTplGenericRandomCacheRead, sizeof(SeqGenericDescOr_t));
    gTplGenericRandomCacheRead.field.AU_NUM      = 0;
    gTplGenericRandomCacheRead.field.DES_TYPE    = DESC_CMD_MLUN | DESC_CMD_READ;
    gTplGenericRandomCacheRead.field.DF          = DESC_FMT_GENERIC;
    gTplGenericRandomCacheRead.field.READY       = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericRandomCacheRead.field.DV          = 1;
    gTplGenericRandomCacheRead.field.CMD1        = FC_READ_BYTE4_JEDEC;
    gTplGenericRandomCacheRead.field.CMD2        = FC_READ_BYTE5;
    gTplGenericRandomCacheRead.field.OPTION      = SEQ_OPTION_140C03;
    gTplGenericRandomCacheRead.field.OR1         = 1;
    gTplGenericRandomCacheRead.field.RAID_OFFSET = 7;

    // ExitTestMode
    MEM_CLR(&gTplGenericExitTestMode, sizeof(SeqGenericDescOr_t));
    gTplGenericExitTestMode.field.AU_NUM      = 0;
    gTplGenericExitTestMode.field.DES_TYPE    = DESC_CMD_MLUN | DESC_CMD_READ;
    gTplGenericExitTestMode.field.DF          = DESC_FMT_GENERIC;
    gTplGenericExitTestMode.field.READY       = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericExitTestMode.field.DV          = 1;
    gTplGenericExitTestMode.field.CMD1        = FC_RESET;
    gTplGenericExitTestMode.field.CMD2        = 0x00;
    gTplGenericExitTestMode.field.OPTION      = SEQ_OPTION_208CB;
    gTplGenericExitTestMode.field.SCMD        = FC_READ_STATUS_BYTE0;
    gTplGenericExitTestMode.field.OR1         = 1;
    gTplGenericExitTestMode.field.RAID_OFFSET = 7;

    // gTplGenericSoftBitXor
    MEM_CLR(&gTplGenericSoftBitXor, sizeof(SeqGenericDescOr_t));
    gTplGenericSoftBitXor.field.DES_TYPE    = DESC_CMD_MLUN | DESC_CMD_READ;
    gTplGenericSoftBitXor.field.DF          = DESC_FMT_GENERIC;
    gTplGenericSoftBitXor.field.READY       = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericSoftBitXor.field.DV          = 1;
    gTplGenericSoftBitXor.field.CMD1        = FC_READ_BYTE0;
    gTplGenericSoftBitXor.field.CMD2        = FC_READ_BYTE1;
    gTplGenericSoftBitXor.field.SCMD        = FC_READ_STATUS_BYTE0;
    gTplGenericSoftBitXor.field.OPTION      = SEQ_OPTION_0280C8;
    gTplGenericSoftBitXor.field.AU_NUM      = 0;
    gTplGenericSoftBitXor.field.OR1         = 1;
    gTplGenericSoftBitXor.field.PREFIX_3B   = 0xC2;
    gTplGenericSoftBitXor.field.RAID_OFFSET = 7;
}
