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
#include "AplDesc_TSB_3DTLCG2_Addons.h"
#include "AplDesc_TSB_3DTLCG2.h"

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

#define SET_RW_DESC_ROW_FROM_PAA(P)             ((P.b.lun << gNfInfo.lunLsbPosition/*21*/) | (P.b.block << gNfInfo.pairedBlkLsbPos) | (P.b.plane << gNfInfo.planeLsbPosition/*8*/)| (P.b.page / gNfInfo.bitsPerCell))

#if (READ_LEVEL_CHANGE == ENABLE)
    #define GET_RETRY_VOLTAGE_STAGE_89(V)       (((U8)(gVoltageStageList_89[V][3]) << 24) | ((U8)(gVoltageStageList_89[V][2]) << 16) | ((U8)(gVoltageStageList_89[V][1]) << 8) | ((U8)(gVoltageStageList_89[V][0])))
    #define GET_RETRY_VOLTAGE_STAGE_8A(V)       (((U8)(gVoltageStageList_8A[V][3]) << 24) | ((U8)(gVoltageStageList_8A[V][2]) << 16) | ((U8)(gVoltageStageList_8A[V][1]) << 8) | ((U8)(gVoltageStageList_8A[V][0])))
    #define GET_RETRY_VOLTAGE_STAGE_8B(V)       ((U8)gVoltageStageList_8B[V])
#else
    #define GET_RETRY_VOLTAGE_STAGE_89(V)       (((U8)(gVoltageStageList_89[V][3]) << 24) | ((U8)(gVoltageStageList_89[V][2]) << 16) | ((U8)(gVoltageStageList_89[V][1]) << 8) | ((U8)(gVoltageStageList_89[V][0])))
    #define GET_RETRY_VOLTAGE_STAGE_8A(V)       (((U8)(gVoltageStageList_8A[V][3]) << 24) | ((U8)(gVoltageStageList_8A[V][2]) << 16) | ((U8)(gVoltageStageList_8A[V][1]) << 8) | ((U8)(gVoltageStageList_8A[V][0])))
    #define GET_RETRY_VOLTAGE_STAGE_8B(V)       ((U8)gVoltageStageList_8B[V])
#endif

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static U16 DescGeneric_3DTLCG2_Read(tFIO_JOB* pJob);
static U16 DescGeneric_3DTLCG2_FastRead(tFIO_JOB *pJob);
static U16 DescGeneric_3DTLCG2_FullSeqRead(tFIO_JOB *pJob);
static U16 DescGeneric_3DTLCG2_SlcRead(tFIO_JOB *pJob);
static U16 DescGeneric_3DTLCG2_SlcTouch(tFIO_JOB *pJob);
static U16 DescGeneric_3DTLCG2_IdsPageRead(tFIO_JOB *pJob);
static U16 DescGeneric_3DTLCG2_IdsRead(tFIO_JOB *pJob);
static U16 DescGeneric_3DTLCG2_PreOpen(tFIO_JOB* pJob);
static U16 DescGeneric_3DTLCG2_ReadDistribution(tFIO_JOB* pJob);
static U16 DescGeneric_3DTLCG2_RR(tFIO_JOB *pJob);
static U16 DescGeneric_3DTLCG2_Write(tFIO_JOB* pJob);
static U16 DescGeneric_3DTLCG2_Erase(tFIO_JOB* pJob);

static __inline U16  DescGeneric_3DTLCG2_FullSeqRing(tFIO_JOB* pJob, U16 die, tPAA paa, U16 descOffset);
static __inline U16  DescGeneric_3DTLCG2_FullSeqOpen(tFIO_JOB* pJob, U16 actDie, tPAA paa, U16 descOffset);
static __inline U16  DescGeneric_3DTLCG2_OpenPage(tFIO_JOB* pJob, U16 actDie, tPAA paa, U16 descOffset);
static __inline U16  DescGeneric_3DTLCG2_SetFeature(tFIO_JOB *pJob, tPAA paa, U16 descOffset);
static __inline void DescRegular_3DTLCG2_Polling(tFIO_JOB *pJob);
static __inline void DescRegular_3DTLCG2_InitTemplate(void);
static __inline void DescGeneric_3DTLCG2_InitTemplate(void);

static __inline void DescGeneric_3DTLCG2_HD_Retry_CW(tFIO_JOB* pJob, U8 fg_cw, U8 fg_fd, U8 fg_JU, tPAA paa);
static __inline void DescGeneric_3DTLCG2_SD_Retry_CW(tFIO_JOB* pJob, U8 fg_cw, tPAA paa);

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
U16 (*gCbfProcDesc[FIO_JOB_CMD_COUNT])(tFIO_JOB* pJob) =
{
    DescGeneric_3DTLCG2_Read,
    DescGeneric_3DTLCG2_FastRead,
    DescGeneric_3DTLCG2_SlcRead,
    DescGeneric_3DTLCG2_SlcTouch,
    DescGeneric_3DTLCG2_IdsPageRead,
    DescGeneric_3DTLCG2_IdsRead,
    DescGeneric_3DTLCG2_PreOpen,
	DescGeneric_3DTLCG2_RR,
    DescGeneric_3DTLCG2_ReadDistribution,
    DescGeneric_3DTLCG2_Write,
    DescGeneric_3DTLCG2_Erase,
};

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
SeqGenericDescOr_t     gTplGenericErase;
SeqGenericDescOr_t     gTplGenericWrite;
SeqGenericDescOr_t     gTplGenericRead;
SeqGenericDescOr_t     gTplGenericOpenRow;
SeqGenericDescOr_t     gTplGenericFastRead;
SeqGenericDescOr_t     gTplGenericFullSeqRead;
SeqGenericDescOr_t     gTplGenericFullSeqRing;
SeqGenericDescOr_t     gTplGenericFullSeqOpen;
SeqGenericDescOr_t     gTplGenericVoltageShift;
SeqGenericDescOr_t     gTplGenericReadRetry;
SeqGenericDescOr_t     gTplGenericPreOpen;
SeqGenericDescOr_t     gTplGenericIdsRead;
SeqGenericDescOr_t     gTplGenericIdsPageRead;
SeqGenericDescOr_t     gTplGenericSlcRead;
SeqGenericDescOr_t     gTplGenericSlcTouch;

SeqGenericDescOr_t     gTplGenericCmdAddrData;
SeqGenericDescOr_t     gTplGenericDisRead;
SeqGenericDescOr_t     gTplGenericPreDisRead;
SeqGenericDescOr_t     gTplGenericEnterTestMode;
SeqGenericDescOr_t     gTplGenericExitTestMode;
SeqGenericDescOr_t     gTplGenericRandomCacheRead;
SeqGenericDescOr_t     gTplGenericSoftBitXor;
SeqGenericFlexDescOr_t gTplGenericFlexDescOr;

SeqRegularDescOr_t     gTplRegularPolling;

U32 gTogglePage[MAX_SUPPORTED_CHANNELS * MAX_SUPPORTED_DEVICES * MAX_SUPPORTED_LUNS][3];

U8  gVoltageStageList_89[MAX_READRETRY_ROW][MAX_READRETRY_COL] = {
	{0x03, 0x04, 0x0A, 0x07}, {0xFD, 0x03, 0xF7, 0xF9}, {0xFA, 0xFF, 0xFA, 0xF4}, {0x01, 0xFC, 0xF4, 0xF0},
	{0x03, 0x08, 0x0B, 0x02}, {0x09, 0x0C, 0x0B, 0x03}, {0x0D, 0x0C, 0x0B, 0x08}, {0x05, 0x06, 0x01, 0xF6},
	{0x05, 0x05, 0x00, 0xF3}, {0x04, 0x04, 0x00, 0xF4}, {0xFA, 0x02, 0xF3, 0xEE}, {0xF7, 0x03, 0xF9, 0xF4},
	{0x04, 0xF9, 0xF3, 0xEE}, {0xFD, 0x00, 0xFB, 0xF7}, {0xF9, 0x00, 0xF6, 0xF1}, {0x00, 0xFD, 0xFA, 0xFA},
	{0x06, 0xF9, 0xF6, 0xF6}, {0x03, 0xFC, 0xF7, 0xF5}, {0xFE, 0x06, 0xF3, 0xF3}, {0x13, 0x06, 0x05, 0xFF},
	{0x0B, 0x07, 0x08, 0x02},
};

U8  gVoltageStageList_8A[MAX_READRETRY_ROW][MAX_READRETRY_COL] = {
	{0x03, 0x07, 0x08, 0x10}, {0xFF, 0xFD, 0xF7, 0x20}, {0x04, 0x01, 0xFB, 0x30}, {0xFB, 0xFB, 0xF7, 0xC0},
	{0x0C, 0x07, 0x06, 0xD0}, {0x0F, 0x08, 0x09, 0xE0}, {0x0D, 0x08, 0x0F, 0xF0}, {0x08, 0x04, 0xFB, 0x00},
	{0x04, 0x00, 0xF8, 0x00}, {0x03, 0x03, 0xFA, 0x00}, {0xFE, 0xFE, 0xF3, 0x00}, {0x01, 0xFA, 0xF5, 0x00},
	{0xFB, 0xF8, 0xF4, 0x00}, {0xFD, 0x00, 0xF9, 0x00}, {0xFD, 0xF9, 0xFA, 0x00}, {0x01, 0xFB, 0xFA, 0x00},
	{0x04, 0xFE, 0xF4, 0x00}, {0x02, 0x00, 0xF7, 0x00}, {0xF8, 0xFE, 0xF6, 0x00}, {0x10, 0x04, 0x10, 0x00},
	{0x0F, 0x0C, 0x04, 0x00},
};

U8  gVoltageStageList_8B[MAX_READRETRY_SLC] =
      { 0x10, 0x20, 0xF0, 0xE0, 0x30, 0xD0, 0xC0};

U8  gPlane2FailByte[5] = {0, 0x3, 0x7, 0, 0x1F};
U8  gSlc2FailByte      = 0x4;
U8  gFeatureCmdTLC[3]  = {0x89, 0x8A, 0x89};
U8  gFeatureCmdSLC[3]  = {0x8B, 0x8B, 0x8B};

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
#include "FastSection.h"
//-----------------------------------------------------------------------------
/**
    One-time initialization of Descriptor.\n

    @param[in]  initMode            initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t AplDesc_OneTimeInit(InitBootMode_t initMode)
{
    DescGeneric_3DTLCG2_InitTemplate();
    DescRegular_3DTLCG2_InitTemplate();

    MEM_SET(gOpenRow, BYTE_MASK, sizeof(gOpenRow));
    MEM_CLR(&gOpenPgMap[0][0], sizeof(gOpenPgMap));
    MEM_CLR(&gOpenPgCnt[0], sizeof(gOpenPgCnt));

    return cEcNoError;
}

#include "PerfSection.h"
//-----------------------------------------------------------------------------
/**
    General descriptor for erase operation

    @param[in]  pJob            Flash I/O Job pointer

    @return descriptor count
**/
//-----------------------------------------------------------------------------
static U16 DescGeneric_3DTLCG2_Erase(tFIO_JOB* pJob)
{
    tPAA paa;
    U32  actDie;
    U16  cnt;
    U8   endPlane;

    if (pJob->flag & FIO_F_SLC) MEM_SET(gOpenRow, BYTE_MASK, sizeof(gOpenRow));

    endPlane = pJob->paa[0].b.plane + pJob->plane - 1;

    gTplGenericErase.field.TRACK_ID  = pJob->tid;
    gTplGenericErase.field.PREFIX_3B = 0;

    for (cnt = 0; cnt < pJob->paaCnt; cnt++)
    {
        paa = pJob->paa[cnt];
        actDie = GET_DIE_NUM_FROM_PAA(paa.b.plane, paa.b.lun, paa.b.ce, paa.b.ch);
        if (pJob->plane == FIO_CMD_1PLANE) endPlane = paa.b.plane + pJob->plane - 1;

        gOpenRow[actDie] = DWORD_MASK;
        gTplGenericErase.field.DEV            = paa.b.ce;
        gTplGenericErase.field.CH             = paa.b.ch;
        gTplGenericErase.field.OPTION         = (paa.b.plane == endPlane) ? SEQ_OPTION_000002 : SEQ_OPTION_800802;
        gTplGenericErase.field.SCMD           = (paa.b.lun == 0) ? FC_READ_STATUS_BYTE2 : FC_READ_STATUS_BYTE3;

        gTplGenericErase.field.FD             = (pJob->descCnt) ? 0: 1;
        gTplGenericErase.field.LD             = 0;

        gTplGenericErase.field.START_ROW_ADDR = (paa.b.lun   << gNfInfo.lunLsbPosition)  |
                                                (paa.b.block << gNfInfo.pairedBlkLsbPos) |
                                                (paa.b.plane << gNfInfo.planeLsbPosition);

        if (pJob->flag & FIO_F_SLC)
        {
            if ((paa.b.plane == 0) || (pJob->plane == FIO_CMD_1PLANE))
            {
                gTplGenericErase.field.PREFIX_3B = FC_PREFIX_SLC_BYTE;
                gTplGenericErase.field.OPTION    = (paa.b.plane == endPlane) ? SEQ_OPTION_00812A : SEQ_OPTION_808802;
            }
            else
            {
                gTplGenericErase.field.PREFIX_3B = 0;
                gTplGenericErase.field.OPTION    = SEQ_OPTION_00012A;
            }

            gTplGenericErase.field.FAIL          = (paa.b.plane == endPlane)   ? gSlc2FailByte: 0;
            gTplGenericErase.field.LD            = (cnt == (pJob->paaCnt - 1)) ? 1 : 0;
        }

        HalSeq_PushDescriptors((PVOID)&gTplGenericErase, sizeof(SeqGenericDescOr_t));
        pJob->descCnt++;
    }

    if (!(pJob->flag & FIO_F_SLC))
    {
        DescRegular_3DTLCG2_Polling(pJob);//Regular Polling 78h
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
static U16 DescGeneric_3DTLCG2_Write(tFIO_JOB* pJob)
{
    tPAA paa;
    U32 actDie;
    U16 cnt;
    U16 totalCnt;
    U16 cntOffset;
    U8  endPlane;
    U8  lmu;

    if (pJob->flag & FIO_F_SLC) MEM_SET(gOpenRow, BYTE_MASK, sizeof(gOpenRow));

    totalCnt  = (pJob->paaCnt >> 2) - 1; // 16K
    cntOffset = 4; // 16K

    endPlane = pJob->paa[0].b.plane + pJob->plane - 1;

    gTplGenericWrite.field.AU_NUM         = 3; // 16K
    gTplGenericWrite.field.TRACK_ID       = pJob->tid;
    gTplGenericWrite.field.AU_SEG         = pJob->segNum;
    gTplGenericWrite.field.START_SSD_PTR  = pJob->ssdIdx[pJob->ssdList];
    gTplGenericWrite.field.XF_COUNT       = (gEcuAuFormatList[pJob->auFmt].xferSizePerAu << 2);
    gTplGenericWrite.field.AU_FORMAT      = pJob->auFmt;

    for (cnt = 0; cnt < pJob->paaCnt; cnt+= cntOffset)
    {
        paa = pJob->paa[cnt];
        lmu = (pJob->flag & FIO_F_SLC) ? 0 : (paa.b.page % gNfInfo.bitsPerCell);
        if (pJob->plane == FIO_CMD_1PLANE) endPlane = paa.b.plane + pJob->plane - 1;

        actDie = GET_DIE_NUM_FROM_PAA(paa.b.plane, paa.b.lun, paa.b.ce, paa.b.ch);
        gOpenRow[actDie] = DWORD_MASK;
        gTplGenericWrite.field.JU             = cnt ? 0 : 1;
        gTplGenericWrite.field.FD             = (pJob->descCnt) ? 0 : 1;
        gTplGenericWrite.field.LD             = 0;
        gTplGenericWrite.field.DEV            = paa.b.ce;
        gTplGenericWrite.field.CH             = paa.b.ch;
        gTplGenericWrite.field.OPTION         = SEQ_OPTION_008100; // PCNT52 1B Prefix
        gTplGenericWrite.field.CMD1           = FC_PROG_BYTE0;
        gTplGenericWrite.field.CMD2           = FC_PROG_BYTE2;
        gTplGenericWrite.field.FAIL           = 0;
        gTplGenericWrite.field.START_ROW_ADDR = SET_RW_DESC_ROW_FROM_PAA(paa);
        gTplGenericWrite.field.SCMD           = (paa.b.lun == 0) ? FC_READ_STATUS_BYTE2 : FC_READ_STATUS_BYTE3;
        gTplGenericWrite.field.PREFIX_3B      = (pJob->flag & FIO_F_SLC) ? FC_PREFIX_SLC_BYTE : (lmu + 1);

        // Normal model
        if ((paa.b.plane == endPlane) && ((lmu == 2) || (pJob->flag & FIO_F_SLC)))
        {
            gTplGenericWrite.field.CMD2 = FC_PROG_BYTE1;
            if (pJob->flag & FIO_F_SLC)
            {

                gTplGenericWrite.field.OPTION = (pJob->plane == FIO_CMD_1PLANE) ? SEQ_OPTION_008118 : SEQ_OPTION_008108; //WCNT 21 PCNT52 1B Prefix
                gTplGenericWrite.field.FAIL   = gSlc2FailByte ;
                gTplGenericWrite.field.LD     = (pJob->descCnt == totalCnt) ? 1 : 0;
            }
            else
            {
                gTplGenericWrite.field.OPTION = SEQ_OPTION_008000;//8000 No wcnt and Polling ;
            }
        }
        else if (paa.b.plane == endPlane)
        {
            gTplGenericWrite.field.OPTION = SEQ_OPTION_008100; // PCNT52 1B Prefix
            gTplGenericWrite.field.CMD2   = FC_PROG_BYTE5 ; //0x1A
        }

        HalSeq_PushDescriptors((PVOID)&gTplGenericWrite, sizeof(SeqGenericDescOr_t));
        pJob->descCnt++;
    }

    HalSeq_PushAuxInserts((PVOID)pJob->aux, gEcuAuFormatList[pJob->auFmt].auxSize * pJob->paaCnt);

    if (!(pJob->flag & FIO_F_SLC))
    {
        DescRegular_3DTLCG2_Polling(pJob);//Regular Polling 78h
    }

    return pJob->descCnt;
}

//-----------------------------------------------------------------------------
/**
    General descriptor for read operation

    @param[in]  pJob            Flash I/O Job pointer

    @return descriptor count
**/
//-----------------------------------------------------------------------------
static U16 DescGeneric_3DTLCG2_Read(tFIO_JOB* pJob)
{
    tPAA paa;
    U32  actDie;
    U16  xferSize;
    U16  descOffset = 0;
    U16  totalCnt;
    U16  cnt;

    DESC_RST_OPEN_PG_BMP(pJob->tid);

    if (pJob->flag & FIO_F_PAGE_READ)
    {
        totalCnt = (pJob->paaCnt >> 2);
        xferSize = gEcuAuFormatList[pJob->auFmt].xferSizePerAu << 2;
        gTplGenericRead.field.AU_NUM = 3;
    }
    else
    {
        totalCnt = pJob->paaCnt;
        xferSize = gEcuAuFormatList[pJob->auFmt].xferSizePerAu;
        gTplGenericRead.field.AU_NUM = 0;
    }

    gTplGenericRead.field.TRACK_ID      = pJob->tid;
    gTplGenericRead.field.AU_SEG        = pJob->segNum;
    gTplGenericRead.field.START_SSD_PTR = pJob->ssdIdx[pJob->ssdList];
    gTplGenericRead.field.AU_FORMAT     = pJob->auFmt;
    gTplGenericRead.field.XF_COUNT      = xferSize;
    gTplGenericRead.field.LE            = (pJob->flag & FIO_F_AUX) ? 1 : 0;

    for (cnt = 0; cnt < totalCnt; cnt++)
    {
        APLDESC_ALLOC_MULTI_TRK(descOffset, MAX_DESC_PER_TRK - MAX_SUPPORTED_PLANES, pJob, gTplGenericRead, cnt);

        paa = (pJob->flag & FIO_F_PAGE_READ) ? (pJob->paa[cnt << 2]) :(pJob->paa[cnt]);

        actDie = GET_DIE_NUM_FROM_PAA(paa.b.plane, paa.b.lun, paa.b.ce, paa.b.ch);

        if (gOpenRow[actDie] != paa.rough.row)
        {
            descOffset = DescGeneric_3DTLCG2_OpenPage(pJob, actDie, paa, descOffset);
        }

        gTplGenericRead.field.PREFIX_3B      = (paa.b.page % gNfInfo.bitsPerCell) + 1;
        gTplGenericRead.field.JU             = cnt ? 0 : 1;
        gTplGenericRead.field.DEV            = paa.b.ce;
        gTplGenericRead.field.CH             = paa.b.ch;
        gTplGenericRead.field.FD             = descOffset ? 0: 1;
        gTplGenericRead.field.LD             = ((cnt == (totalCnt - 1)) || CHK_OFFSET_OVER_TH(descOffset, (MAX_DESC_PER_TRK - MAX_SUPPORTED_PLANES))) ? 1 : 0;

        gTplGenericRead.field.START_COL_ADDR = (pJob->flag & FIO_F_PAGE_READ) ? 0 : (xferSize * paa.b.frag);
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
static __inline U16 DescGeneric_3DTLCG2_OpenPage(tFIO_JOB* pJob, U16 actDie, tPAA paa, U16 descOffset)
{

    gTplGenericOpenRow.field.TRACK_ID  = pJob->tid;
    gTplGenericOpenRow.field.AU_FORMAT = pJob->auFmt;
    gTplGenericOpenRow.field.CH        = paa.b.ch;
    gTplGenericOpenRow.field.DEV       = paa.b.ce;
    gTplGenericOpenRow.field.LD        = 0;
    gTplGenericOpenRow.field.PREFIX_3B = (paa.b.page % gNfInfo.bitsPerCell) + 1;

    if (paa.b.plane)
    {
        gTplGenericOpenRow.field.FD     = descOffset ? 0: 1;
        gTplGenericOpenRow.field.OPTION = SEQ_OPTION_028148;
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
            gTplGenericOpenRow.field.OPTION = (planeOffset == (pJob->plane - 1))? SEQ_OPTION_028148: SEQ_OPTION_828100;
            gTplGenericOpenRow.field.CMD2   = (planeOffset == (pJob->plane - 1))? FC_READ_BYTE1    : FC_READ_MP_JEDEC;
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
#include "FastSection.h"
//-----------------------------------------------------------------------------
/**
    General descriptor for pre-open operation

    @param[in]  pJob            Flash I/O Job pointer

    @return descriptor count
**/
//-----------------------------------------------------------------------------
static U16 DescGeneric_3DTLCG2_PreOpen(tFIO_JOB* pJob)
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

        for (planeOffset = 0; planeOffset < pJob->plane; planeOffset++)
        {
            paa.b.plane = planeOffset;

            actDie = GET_DIE_NUM_FROM_PAA(paa.b.plane, paa.b.lun, paa.b.ce, paa.b.ch);

            gTplGenericPreOpen.field.PREFIX_3B = (paa.b.page % gNfInfo.bitsPerCell)+ 1;
            gTplGenericPreOpen.field.FD        = pJob->descCnt ? 0: 1;
            gTplGenericPreOpen.field.LD        = (pJob->descCnt == totalCnt) ? 1 : 0;
            gTplGenericPreOpen.field.OPTION    = (planeOffset == (pJob->plane - 1))? SEQ_OPTION_028148: SEQ_OPTION_828100;
            gTplGenericPreOpen.field.CMD2      = (planeOffset == (pJob->plane - 1))? FC_READ_BYTE1     : FC_READ_MP_JEDEC;
            gTplGenericPreOpen.field.SCMD      = (paa.b.lun == 0)? FC_READ_STATUS_BYTE2 : FC_READ_STATUS_BYTE3;

            gTplGenericPreOpen.field.START_ROW_ADDR = SET_RW_DESC_ROW_FROM_PAA(paa);
            gOpenRow[actDie] = paa.rough.row;

            HalSeq_PushDescriptors((PVOID)&gTplGenericPreOpen, sizeof(SeqGenericDescOr_t));

            pJob->descCnt++;
        }
    }

    return pJob->descCnt;
}
#include "PerfSection.h"
//-----------------------------------------------------------------------------
/**
    General descriptor for fast-read operation

    @param[in]  pJob            Flash I/O Job pointer

    @return descriptor count
**/
//-----------------------------------------------------------------------------
static U16 DescGeneric_3DTLCG2_IdsRead(tFIO_JOB *pJob)
{
    tPAA paa;
    U32 actDie = 0;
    U32 xferSize;
    U32 cnt;

    DESC_RST_OPEN_PG_BMP(pJob->tid);

    xferSize = gEcuAuFormatList[pJob->auFmt].xferSizePerAu;

    gTplGenericIdsRead.field.TRACK_ID        = pJob->tid;
    gTplGenericIdsRead.field.XF_COUNT        = xferSize;
    gTplGenericIdsRead.field.AU_SEG          = pJob->segNum;
    gTplGenericIdsRead.field.HCMD_INDEX_HWLL = pJob->pMsgHost->attrInfo.b.cmdId;

    for (cnt = 0; cnt < pJob->paaCnt; cnt++)
    {
        paa = pJob->paa[cnt];

        actDie = GET_DIE_NUM_FROM_PAA(paa.b.plane, paa.b.lun, paa.b.ce, paa.b.ch);

        if (gOpenRow[actDie] == paa.rough.row)
        {
            gTplGenericIdsRead.field.OPTION = SEQ_OPTION_320C03;
        }
        else if ((pJob->plane == gNfInfo.planesPerLun) && (paa.b.plane == 0))
        {
            DescGeneric_3DTLCG2_OpenPage(pJob, actDie, paa, pJob->descCnt);
            gTplGenericIdsRead.field.OPTION = SEQ_OPTION_320C03;
        }
        else
        {
            gTplGenericIdsRead.field.OPTION = SEQ_OPTION_328148;
            gOpenRow[actDie] = paa.rough.row;
        }
        gTplGenericIdsRead.field.PREFIX_3B       = (paa.b.page % gNfInfo.bitsPerCell) + 1;
        gTplGenericIdsRead.field.FD              = (pJob->descCnt) ? 0 : 1;
        gTplGenericIdsRead.field.LD              = (cnt == (pJob->paaCnt - 1)) ? 1 : 0;
        gTplGenericIdsRead.field.DEV             = paa.b.ce;
        gTplGenericIdsRead.field.CH              = paa.b.ch;
        gTplGenericIdsRead.field.SCMD            = (paa.b.lun == 0) ? FC_READ_STATUS_BYTE2 : FC_READ_STATUS_BYTE3;
        gTplGenericIdsRead.field.START_COL_ADDR  = (paa.b.frag * xferSize);
        gTplGenericIdsRead.field.START_ROW_ADDR  = SET_RW_DESC_ROW_FROM_PAA(paa);

        gTplGenericIdsRead.field.AU_INDEX = cnt + pJob->paaOffset;

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
static U16 DescGeneric_3DTLCG2_IdsPageRead(tFIO_JOB *pJob)
{
    tPAA paa;
    U32  actDie = 0;
    U32  cnt;
    U32  totalCnt;

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
            gTplGenericIdsPageRead.field.OPTION = SEQ_OPTION_320C03;
        }
        else
        {
            if (paa.b.plane == 0)
            {
                DescGeneric_3DTLCG2_OpenPage(pJob, actDie, paa, pJob->descCnt);
                gTplGenericIdsPageRead.field.OPTION = SEQ_OPTION_320C03;
            }
            else
            {
                gTplGenericIdsPageRead.field.OPTION = SEQ_OPTION_328148;
                gOpenRow[actDie] = paa.rough.row;
            }
        }

        gTplGenericIdsPageRead.field.PREFIX_3B      = (paa.b.page % gNfInfo.bitsPerCell) + 1;
        gTplGenericIdsPageRead.field.FD             = (pJob->descCnt) ? 0 : 1;
        gTplGenericIdsPageRead.field.LD             = (cnt == (totalCnt - 1)) ? 1 : 0;
        gTplGenericIdsPageRead.field.DEV            = paa.b.ce;
        gTplGenericIdsPageRead.field.CH             = paa.b.ch;
        gTplGenericIdsPageRead.field.SCMD           = (paa.b.lun == 0) ? FC_READ_STATUS_BYTE2 : FC_READ_STATUS_BYTE3;
        gTplGenericIdsPageRead.field.AU_INDEX       = pJob->paaOffset + (cnt << 2);
        gTplGenericIdsPageRead.field.START_ROW_ADDR = SET_RW_DESC_ROW_FROM_PAA(paa);

        HalSeq_PushDescriptors((PVOID)&gTplGenericIdsPageRead, sizeof(SeqGenericDescOr_t));
        pJob->descCnt++;
    }

    return pJob->descCnt;
}
#include "FastSection.h"
//-----------------------------------------------------------------------------
/**
    General descriptor for fast-read operation

    @param[in]  pJob            Flash I/O Job pointer

    @return descriptor count
**/
//-----------------------------------------------------------------------------
static __inline U16 DescGeneric_3DTLCG2_FullSeqOpen(tFIO_JOB* pJob, U16 actDie, tPAA paa, U16 descOffset)
{
    gTplGenericFullSeqOpen.field.TRACK_ID  = pJob->tid;
    gTplGenericFullSeqOpen.field.AU_FORMAT = pJob->auFmt;
    gTplGenericFullSeqOpen.field.CH        = paa.b.ch;
    gTplGenericFullSeqOpen.field.DEV       = paa.b.ce;
    gTplGenericFullSeqOpen.field.LD        = 0;

    if (paa.b.plane)
    {
        gTplGenericFullSeqOpen.field.FD        = descOffset ? 0: 1;
        gTplGenericFullSeqOpen.field.OPTION    = SEQ_OPTION_028148;
        gTplGenericFullSeqOpen.field.CMD2      = FC_READ_BYTE1;
        gTplGenericFullSeqOpen.field.SCMD      = (paa.b.lun == 0)? FC_READ_STATUS_BYTE2 : FC_READ_STATUS_BYTE3;
        gTplGenericFullSeqOpen.field.PREFIX_3B = 0x50;
        gTplGenericFullSeqOpen.field.START_ROW_ADDR = SET_RW_DESC_ROW_FROM_PAA(paa);
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

            gTplGenericFullSeqOpen.field.FD        = descOffset ? 0: 1;
            gTplGenericFullSeqOpen.field.OPTION    = (planeOffset == (pJob->plane - 1))? SEQ_OPTION_020148: SEQ_OPTION_828100;
            gTplGenericFullSeqOpen.field.CMD2      = (planeOffset == (pJob->plane - 1))? FC_READ_BYTE1    : FC_READ_MP_JEDEC;
            gTplGenericFullSeqOpen.field.PREFIX_3B = (planeOffset == (pJob->plane - 1))? 0 : 0x50;
            gTplGenericFullSeqOpen.field.SCMD      = (paa.b.lun == 0)? FC_READ_STATUS_BYTE2 : FC_READ_STATUS_BYTE3;

            gTplGenericFullSeqOpen.field.START_ROW_ADDR = SET_RW_DESC_ROW_FROM_PAA(paa);
            gOpenRow[actDie] = paa.rough.row;

            DESC_SET_OPEN_PG_BMP(pJob->tid, descOffset);
            HalSeq_PushDescriptors((PVOID)&gTplGenericFullSeqOpen, sizeof(SeqGenericDescOr_t));
            descOffset++;
            pJob->descCnt++;
        }
    }

    return descOffset;
}
//-----------------------------------------------------------------------------
/**
    General descriptor for fast-read operation

    @param[in]  pJob            Flash I/O Job pointer

    @return descriptor count
**/
//-----------------------------------------------------------------------------
static __inline U16 DescGeneric_3DTLCG2_FullSeqRing(tFIO_JOB* pJob, U16 die, tPAA paa, U16 descOffset)
{
    U32  temp;
    U8   cnt = 0;

    gTplGenericFullSeqRing.field.TRACK_ID  = pJob->tid;
    gTplGenericFullSeqRing.field.OPTION    = SEQ_OPTION_0008C2;
    gTplGenericFullSeqRing.field.CMD1      = 0x3F;
    gTplGenericFullSeqRing.field.LD        = 0;
    gTplGenericFullSeqRing.field.DEV       = paa.b.ce;
    gTplGenericFullSeqRing.field.CH        = paa.b.ch;

    if (gTogglePage[die][1] == paa.rough.row)
    {
        gTplGenericFullSeqRing.field.FD     = descOffset ? 0 : 1;
        gTplGenericFullSeqRing.field.SCMD   = (paa.b.lun == 0) ? FC_READ_STATUS_BYTE2 : FC_READ_STATUS_BYTE3;
        gTplGenericFullSeqRing.field.START_ROW_ADDR  = SET_RW_DESC_ROW_FROM_PAA(paa);

        temp = gTogglePage[die][0];
        gTogglePage[die][0] = gTogglePage[die][1];
        gTogglePage[die][1] = gTogglePage[die][2];
        gTogglePage[die][2] = temp;
        DESC_SET_OPEN_PG_BMP(pJob->tid, descOffset);
        HalSeq_PushDescriptors((PVOID)&gTplGenericFullSeqRing, sizeof(SeqGenericDescOr_t));
        descOffset++;
        pJob->descCnt++;
    }
    else
    {
        for (cnt = 0; cnt < 2; cnt++)
        {
            gTplGenericFullSeqRing.field.FD     = descOffset ? 0 : 1;
            gTplGenericFullSeqRing.field.DEV    = paa.b.ce;
            gTplGenericFullSeqRing.field.CH     = paa.b.ch;
            gTplGenericFullSeqRing.field.START_ROW_ADDR  = SET_RW_DESC_ROW_FROM_PAA(paa);
            gTplGenericFullSeqRing.field.SCMD   = (paa.b.lun == 0) ? FC_READ_STATUS_BYTE2 : FC_READ_STATUS_BYTE3;
            temp = gTogglePage[die][0];
            gTogglePage[die][0] = gTogglePage[die][1];
            gTogglePage[die][1] = gTogglePage[die][2];
            gTogglePage[die][2] = temp;
            DESC_SET_OPEN_PG_BMP(pJob->tid, descOffset);
            HalSeq_PushDescriptors((PVOID)&gTplGenericFullSeqRing, sizeof(SeqGenericDescOr_t));
            descOffset++;
            pJob->descCnt++;
        }
    }

    return descOffset;
}
//-----------------------------------------------------------------------------
/**
    General descriptor for fast-read operation

    @param[in]  pJob            Flash I/O Job pointer

    @return descriptor count
**/
//-----------------------------------------------------------------------------
static U16 DescGeneric_3DTLCG2_FullSeqRead(tFIO_JOB *pJob)
{
    tPAA paa;
    tPAA tmpPaa;
    U32  actDie;
    U16  xferSize;
    U16  totalCnt;
    U16  die;
    U16  cnt;
    U16  descOffset = 0;
    U8   lmu;

    DESC_RST_OPEN_PG_BMP(pJob->tid);

    totalCnt      = (pJob->paaCnt >> 2);
    xferSize      = gEcuAuFormatList[pJob->auFmt].xferSizePerAu << 2;

    gTplGenericFullSeqRead.field.LE            = (pJob->flag & FIO_F_AUX) ? 1 : 0;
    gTplGenericFullSeqRead.field.START_SSD_PTR = pJob->ssdIdx[pJob->ssdList];
    gTplGenericFullSeqRead.field.TRACK_ID      = pJob->tid;
    gTplGenericFullSeqRead.field.AU_SEG        = pJob->segNum;
    gTplGenericFullSeqRead.field.AU_FORMAT     = pJob->auFmt;
    gTplGenericFullSeqRead.field.XF_COUNT      = xferSize;

    for (cnt = 0; cnt < totalCnt; cnt++)
    {
        APLDESC_ALLOC_MULTI_TRK(descOffset, MAX_DESC_PER_TRK - MAX_SUPPORTED_PLANES, pJob, gTplGenericFullSeqRead, cnt);

        paa = pJob->paa[cnt << 2];
        lmu = paa.b.page % 3;
        actDie = GET_DIE_NUM_FROM_PAA(paa.b.plane, paa.b.lun, paa.b.ce, paa.b.ch);
        die    = GET_DIE_NUM(paa.b.lun, paa.b.ce, paa.b.ch);

        if (gTogglePage[die][0] == paa.rough.row || gTogglePage[die][1] == paa.rough.row || gTogglePage[die][2] == paa.rough.row)
        {
            if (gTogglePage[die][0] != paa.rough.row)
            {
                descOffset = DescGeneric_3DTLCG2_FullSeqRing(pJob, die, paa, descOffset);
            }
        }
        else
        {
            descOffset = DescGeneric_3DTLCG2_FullSeqOpen(pJob, actDie, paa, descOffset);
            tmpPaa = paa;
            for (lmu = 0; lmu < gNfInfo.pagesPerProgUnit; lmu++)
            {
                tmpPaa.b.page = tmpPaa.b.page + lmu;
                gTogglePage[die][lmu] = tmpPaa.rough.row;
            }
        }
        gTplGenericFullSeqRead.field.OPTION          = SEQ_OPTION_320C03;
        gTplGenericFullSeqRead.field.PREFIX_3B       = (paa.b.page % gNfInfo.bitsPerCell) + 1;
        gTplGenericFullSeqRead.field.FD              = descOffset ? 0: 1;
        gTplGenericFullSeqRead.field.JU              = cnt ? 0 : 1;
        gTplGenericFullSeqRead.field.LD              = ((cnt == (totalCnt - 1)) || CHK_OFFSET_OVER_TH(descOffset, (MAX_DESC_PER_TRK - MAX_SUPPORTED_PLANES))) ? 1 : 0;
        gTplGenericFullSeqRead.field.DEV             = paa.b.ce;
        gTplGenericFullSeqRead.field.CH              = paa.b.ch;
        gTplGenericFullSeqRead.field.SCMD            = (paa.b.lun == 0) ? FC_READ_STATUS_BYTE2 : FC_READ_STATUS_BYTE3;
        gTplGenericFullSeqRead.field.START_COL_ADDR  = 0;
        gTplGenericFullSeqRead.field.START_ROW_ADDR  = SET_RW_DESC_ROW_FROM_PAA(paa);

        HalSeq_PushDescriptors((PVOID)&gTplGenericFullSeqRead, sizeof(SeqGenericDescOr_t));
        descOffset++;
        pJob->descCnt++;
    }

    return pJob->descCnt;
}
#include "PerfSection.h"
//-----------------------------------------------------------------------------
/**
    General descriptor for fast-read operation

    @param[in]  pJob            Flash I/O Job pointer

    @return descriptor count
**/
//-----------------------------------------------------------------------------
static U16 DescGeneric_3DTLCG2_FastRead(tFIO_JOB *pJob)
{
    tPAA paa;
    U32  actDie;
    U16  xferSize;
    U16  descOffset = 0;
    U16  cnt;

    #if CHK_FW_TYPE(FW_AGING)
    MEM_SET(gOpenRow, BYTE_MASK, sizeof(gOpenRow));
    #endif

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
            gTplGenericFastRead.field.BM     = 0;
            gTplGenericFastRead.field.OPTION = SEQ_OPTION_320C03;
        }
        else
        {
            if (pJob->flag & FIO_F_NANDTIME)
            {
                gTplGenericFastRead.field.BM     = 1;
                gTplGenericFastRead.field.OPTION = SEQ_OPTION_328100;
            }
            else
            {
                gTplGenericFastRead.field.BM     = 0;
                gTplGenericFastRead.field.OPTION = SEQ_OPTION_328148;
            }
            gOpenRow[actDie] = paa.rough.row;
        }
        gTplGenericFastRead.field.PREFIX_3B       = (paa.b.page % gNfInfo.bitsPerCell) + 1;
        gTplGenericFastRead.field.FD              = (descOffset) ? 0 : 1;
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
    General descriptor for SlcTouch operation

    @param[in]  pJob            Flash I/O Job pointer
    @param[in]  actDie          active die
    @param[in]  paa             paa

    @return descriptor count
**/
//-----------------------------------------------------------------------------
static U16 DescGeneric_3DTLCG2_SlcTouch(tFIO_JOB* pJob)
{
    tPAA paa;
    U16  cnt;
    U8   planeOffset;

    MEM_SET(gOpenRow, BYTE_MASK, sizeof(gOpenRow));

    gTplGenericSlcTouch.field.TRACK_ID  = pJob->tid;
    gTplGenericSlcTouch.field.AU_FORMAT = pJob->auFmt;

    for (cnt = 0; cnt < pJob->paaCnt; cnt++)
    {
        paa = pJob->paa[cnt];

        gTplGenericSlcTouch.field.CH   = paa.b.ch;
        gTplGenericSlcTouch.field.DEV  = paa.b.ce;
        gTplGenericSlcTouch.field.SCMD = (paa.b.lun == 0)? FC_READ_STATUS_BYTE2 : FC_READ_STATUS_BYTE3;

        for (planeOffset = 0; planeOffset < pJob->plane; planeOffset++)
        {
            paa.b.plane = planeOffset;

            gTplGenericSlcTouch.field.START_ROW_ADDR = SET_RW_DESC_ROW_FROM_PAA(paa);

            if (planeOffset == 0)
            {
                gTplGenericSlcTouch.field.CMD2      = FC_READ_MP_JEDEC;
                gTplGenericSlcTouch.field.PREFIX_3B = FC_PREFIX_SLC_BYTE;
                gTplGenericSlcTouch.field.OPTION    = SEQ_OPTION_828100;
                gTplGenericSlcTouch.field.FD        = pJob->descCnt ? 0: 1;
                gTplGenericSlcTouch.field.LD        = 0;
            }
            else if (planeOffset == pJob->plane - 1)
            {
                gTplGenericSlcTouch.field.CMD2      = FC_READ_BYTE1;
                gTplGenericSlcTouch.field.PREFIX_3B = 0;
                gTplGenericSlcTouch.field.OPTION    = SEQ_OPTION_020148;
                gTplGenericSlcTouch.field.FD        = 0;
                gTplGenericSlcTouch.field.LD        = (cnt == pJob->paaCnt - 1) ? 1 : 0;
            }
            else
            {
                gTplGenericSlcTouch.field.CMD2      = FC_READ_MP_JEDEC;
                gTplGenericSlcTouch.field.PREFIX_3B = 0;
                gTplGenericSlcTouch.field.OPTION    = SEQ_OPTION_820100;
                gTplGenericSlcTouch.field.FD        = 0;
                gTplGenericSlcTouch.field.LD        = 0;
            }

            HalSeq_PushDescriptors((PVOID)&gTplGenericSlcTouch, sizeof(SeqGenericDescOr_t));

            pJob->descCnt++;
        }
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
static U16 DescGeneric_3DTLCG2_SlcRead(tFIO_JOB *pJob)
{
    tPAA paa;
    U16 xferSize;
    U16 totalCnt;
    U16 cnt;

    MEM_SET(gOpenRow, BYTE_MASK, sizeof(gOpenRow));

    xferSize = gEcuAuFormatList[pJob->auFmt].xferSizePerAu << 2;
    totalCnt = (pJob->paaCnt >> 2); // 16K

    gTplGenericSlcRead.field.LE              = (pJob->flag & FIO_F_AUX) ? 1 : 0;
    gTplGenericSlcRead.field.TRACK_ID        = pJob->tid;
    gTplGenericSlcRead.field.AU_FORMAT       = pJob->auFmt;
    gTplGenericSlcRead.field.START_COL_ADDR  = 0;
    gTplGenericSlcRead.field.OPTION          = SEQ_OPTION_328148;
    gTplGenericSlcRead.field.XF_COUNT        = xferSize;
    gTplGenericSlcRead.field.AU_SEG          = pJob->segNum;
    gTplGenericSlcRead.field.START_SSD_PTR   = pJob->ssdIdx[pJob->ssdList];

    for (cnt = 0; cnt < totalCnt; cnt++)
    {
        paa = pJob->paa[cnt << 2];

        gTplGenericSlcRead.field.FD              = (pJob->descCnt) ? 0 : 1;
        gTplGenericSlcRead.field.LD              = (cnt == (totalCnt - 1)) ? 1 : 0;
        gTplGenericSlcRead.field.DEV             = paa.b.ce;
        gTplGenericSlcRead.field.CH              = paa.b.ch;
        gTplGenericSlcRead.field.SCMD            = (paa.b.lun == 0) ? FC_READ_STATUS_BYTE2 : FC_READ_STATUS_BYTE3;
        gTplGenericSlcRead.field.START_ROW_ADDR  = SET_RW_DESC_ROW_FROM_PAA(paa);
        gTplGenericSlcRead.field.JU              = cnt ? 0 : 1;

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
static U16 DescGeneric_3DTLCG2_RR(tFIO_JOB *pJob)
{
    tPAA vth;
    tPAA paa;
    U16  xferSize;
    U16  cnt;
    U16  descOffset = 0;
    U16  paaCnt = pJob->paaCnt / 2;
    U16  actDie;
    U8*  pFeatureCmd;
    U8   lmu;

    if (pJob->flag & FIO_F_SLC) pFeatureCmd = gFeatureCmdSLC;
    else                        pFeatureCmd = gFeatureCmdTLC;

    xferSize = gEcuAuFormatList[pJob->auFmt].xferSizePerAu;

    if (pJob->css == CACHE_R_IDS)
    {
        gTplGenericReadRetry.field.HWLL            = 1;
        gTplGenericReadRetry.field.HCMD_INDEX_HWLL = pJob->pMsgHost->attrInfo.b.cmdId;
    }
    else
    {
        gTplGenericReadRetry.field.PRP_CNT         = 0;
        gTplGenericReadRetry.field.HWLL            = 0;
        gTplGenericReadRetry.field.HCMD_INDEX_HWLL = 0;
        gTplGenericReadRetry.field.AU_INDEX        = 0;
        gTplGenericReadRetry.field.START_SSD_PTR   = pJob->ssdIdx[pJob->ssdList];
    }

    gTplGenericReadRetry.field.AU_SEG    = pJob->segNum;
    gTplGenericReadRetry.field.TRACK_ID  = pJob->tid;
    gTplGenericReadRetry.field.LE        = (pJob->flag & FIO_F_AUX) ? 1 : 0;
    gTplGenericReadRetry.field.AU_FORMAT = pJob->auFmt;
    gTplGenericReadRetry.field.XF_COUNT  = xferSize;

    for (cnt = 0; cnt < paaCnt; cnt++)
    {
        vth = pJob->paa[cnt * 2];
        paa = pJob->paa[cnt * 2 + 1];

        // TODO:: Clean multiple plane openrow
        actDie = GET_DIE_NUM_FROM_PAA(paa.b.plane, paa.b.lun, paa.b.ce, paa.b.ch);
        gOpenRow[actDie] = DWORD_MASK;

        //---------------------------------------------------------------------------------------
        APLDESC_ALLOC_MULTI_TRK(descOffset, (MAX_DESC_PER_TRK - 1), pJob, gTplGenericVoltageShift, cnt);

        lmu = paa.b.page % gNfInfo.bitsPerCell;

        gTplGenericVoltageShift.field.TRACK_ID       = pJob->tid;
        gTplGenericVoltageShift.field.DEV            = paa.b.ce;
        gTplGenericVoltageShift.field.CH             = paa.b.ch;
        gTplGenericVoltageShift.field.FD             = descOffset ? 0 : 1;
        gTplGenericVoltageShift.field.LD             = 0;
        gTplGenericVoltageShift.field.START_COL_ADDR = (paa.b.lun | pFeatureCmd[lmu] << 8);
        gTplGenericVoltageShift.field.SCMD           = (paa.b.lun == 0) ? FC_READ_STATUS_BYTE2 : FC_READ_STATUS_BYTE3;
        gTplGenericVoltageShift.field.START_ROW_ADDR = SET_RW_DESC_ROW_FROM_PAA(paa);

        if (pFeatureCmd[lmu] == 0x89)      gTplGenericVoltageShift.field.SET_PARAM = GET_RETRY_VOLTAGE_STAGE_89(vth.rr.voltage);
        else if (pFeatureCmd[lmu] == 0x8A) gTplGenericVoltageShift.field.SET_PARAM = GET_RETRY_VOLTAGE_STAGE_8A(vth.rr.voltage);
        else if (pFeatureCmd[lmu] == 0x8B) gTplGenericVoltageShift.field.SET_PARAM = GET_RETRY_VOLTAGE_STAGE_8B(vth.rr.voltage);

        HalSeq_PushDescriptors((PVOID)&gTplGenericVoltageShift, sizeof(SeqGenericDescOr_t));
        descOffset++;
        pJob->descCnt++;

        //---------------------------------------------------------------------------------------
        gTplGenericReadRetry.field.TRACK_ID       = pJob->tid;
        gTplGenericReadRetry.field.PREFIX_3B      = (pJob->flag & FIO_F_SLC) ? FC_PREFIX_SLC_BYTE : lmu + 1;
        gTplGenericReadRetry.field.FD             = 0;
        gTplGenericReadRetry.field.LD             = 0;
        gTplGenericReadRetry.field.DEV            = paa.b.ce;
        gTplGenericReadRetry.field.CH             = paa.b.ch;
        gTplGenericReadRetry.field.SCMD           = (paa.b.lun == 0) ? FC_READ_STATUS_BYTE2 : FC_READ_STATUS_BYTE3;
        gTplGenericReadRetry.field.START_COL_ADDR = paa.b.frag * xferSize;
        gTplGenericReadRetry.field.START_ROW_ADDR = SET_RW_DESC_ROW_FROM_PAA(paa);

        if (cnt && pJob->paa[(cnt - 1) * 2].rough2.row == paa.rough2.row)
        {
            gTplGenericReadRetry.field.OPTION = SEQ_OPTION_320C03;
        }
        else
        {
            gTplGenericReadRetry.field.OPTION = SEQ_OPTION_328148;
        }

        if (pJob->css == CACHE_R_IDS) gTplGenericReadRetry.field.AU_INDEX   = pJob->paaOffset + vth.rr.srcIdx;
        else                          gTplGenericReadRetry.field.JU         = cnt ? 0 : 1;

        HalSeq_PushDescriptors((PVOID)&gTplGenericReadRetry, sizeof(SeqGenericDescOr_t));
        descOffset++;
        pJob->descCnt++;

        //---------------------------------------------------------------------------------------
        gTplGenericVoltageShift.field.FD = 0;
        gTplGenericVoltageShift.field.LD = ((cnt == paaCnt - 1) || CHK_OFFSET_OVER_TH(descOffset, (MAX_DESC_PER_TRK - 1))) ? 1 : 0;

        if (pFeatureCmd[lmu] == 0x89)      gTplGenericVoltageShift.field.SET_PARAM  = ((U32)G_VALUE << 24) | ((U32)E_VALUE << 16) | ((U32)C_VALUE << 8) | (U32)A_VALUE;
        else if (pFeatureCmd[lmu] == 0x8A) gTplGenericVoltageShift.field.SET_PARAM  = ((U32)F_VALUE << 16) | ((U32)D_VALUE << 8)  | (U32)B_VALUE;
        else if (pFeatureCmd[lmu] == 0x8B) gTplGenericVoltageShift.field.SET_PARAM  = L_VALUE;

        HalSeq_PushDescriptors((PVOID)&gTplGenericVoltageShift, sizeof(SeqGenericDescOr_t));
        descOffset++;
        pJob->descCnt++;
    }

    return pJob->descCnt;
}

#include "FastSection.h"
//-----------------------------------------------------------------------------
/**
    General descriptor for PreDis operation

    @param[in]  pJob            Flash I/O Job pointer
    @param[in]  actDie          active die
    @param[in]  paa             paa

    @return descriptor count
**/
//-----------------------------------------------------------------------------
static U16 DescGeneric_3DTLCG2_PreDis(tFIO_JOB* pJob)
{
    tPAA paa;
    U16  idx;

    paa = pJob->paa[0];

    gTplGenericEnterTestMode.field.TRACK_ID       = pJob->tid;
    gTplGenericEnterTestMode.field.AU_SEG         = pJob->segNum;
    gTplGenericEnterTestMode.field.START_SSD_PTR  = pJob->ssdIdx[pJob->ssdList];
    gTplGenericEnterTestMode.field.AU_FORMAT      = pJob->auFmt;
    gTplGenericEnterTestMode.field.XF_COUNT       = 0;
    gTplGenericEnterTestMode.field.AU_NUM         = 3;
    gTplGenericEnterTestMode.field.JU             = 0;
    gTplGenericEnterTestMode.field.DEV            = paa.b.ce;
    gTplGenericEnterTestMode.field.CH             = paa.b.ch;
    gTplGenericEnterTestMode.field.FD             = 1;
    gTplGenericEnterTestMode.field.LD             = 0;
    gTplGenericEnterTestMode.field.START_ROW_ADDR = SET_RW_DESC_ROW_FROM_PAA(paa);
    AplDesc_TSB_3DTLCG2_Addons_EnterTextMode(&gTplGenericEnterTestMode);

    HalSeq_PushDescriptors((PVOID)&gTplGenericEnterTestMode, sizeof(SeqGenericDescOr_t));
    pJob->descCnt++;

    gTplGenericCmdAddrData.field.TRACK_ID       = pJob->tid;
    gTplGenericCmdAddrData.field.AU_SEG         = pJob->segNum;
    gTplGenericCmdAddrData.field.START_SSD_PTR  = pJob->ssdIdx[pJob->ssdList];
    gTplGenericCmdAddrData.field.AU_FORMAT      = pJob->auFmt;
    gTplGenericCmdAddrData.field.XF_COUNT       = 0;
    gTplGenericCmdAddrData.field.AU_NUM         = 0;      // 16K
    gTplGenericCmdAddrData.field.JU             = 0;
    gTplGenericCmdAddrData.field.DEV            = paa.b.ce;
    gTplGenericCmdAddrData.field.CH             = paa.b.ch;
    gTplGenericCmdAddrData.field.FD             = 0;
    gTplGenericCmdAddrData.field.LD             = 0;
    gTplGenericCmdAddrData.field.START_ROW_ADDR = SET_RW_DESC_ROW_FROM_PAA(paa);

    for(idx = 0; idx < 4; idx++)
    {
        AplDesc_TSB_3DTLCG2_Addons_CmdAddrData(&gTplGenericCmdAddrData, idx);

        HalSeq_PushDescriptors((PVOID)&gTplGenericCmdAddrData, sizeof(SeqGenericDescOr_t));
        pJob->descCnt++;
    }

    gTplGenericPreDisRead.field.START_SSD_PTR   = pJob->ssdIdx[pJob->ssdList];
    gTplGenericPreDisRead.field.TRACK_ID        = pJob->tid;
    gTplGenericPreDisRead.field.AU_SEG          = pJob->segNum;
    gTplGenericPreDisRead.field.AU_FORMAT       = pJob->auFmt;
    gTplGenericPreDisRead.field.FD              = 0 ;
    gTplGenericPreDisRead.field.JU              = 0;
    gTplGenericPreDisRead.field.LD              = 1;
    gTplGenericPreDisRead.field.DEV             = paa.b.ce;
    gTplGenericPreDisRead.field.CH              = paa.b.ch;
    gTplGenericPreDisRead.field.SCMD            = (paa.b.lun == 0) ? FC_READ_STATUS_BYTE2 : FC_READ_STATUS_BYTE3;
    gTplGenericPreDisRead.field.START_COL_ADDR  = 0x00;
    gTplGenericPreDisRead.field.START_ROW_ADDR  = SET_RW_DESC_ROW_FROM_PAA(paa);
    gTplGenericPreDisRead.field.XF_COUNT        =  0;
    AplDesc_TSB_3DTLCG2_Addons_PreDis(&gTplGenericPreDisRead);

    HalSeq_PushDescriptors((PVOID)&gTplGenericPreDisRead, sizeof(SeqGenericDescOr_t));
    pJob->descCnt++;
    return pJob->descCnt;
}

#include "PerfSection.h"
//-----------------------------------------------------------------------------
/**
    General descriptor for DisToggle operation

    @param[in]  pJob            Flash I/O Job pointer
    @param[in]  actDie          active die
    @param[in]  paa             paa

    @return descriptor count
**/
//-----------------------------------------------------------------------------
static U16 DescGeneric_3DTLCG2_DisToggle(tFIO_JOB* pJob)
{
    tPAA paa;
    U16 xferSize;
    U16 descOffset = 1;
    U16 cnt;

    xferSize = gEcuAuFormatList[pJob->auFmt].xferSizePerAu << 2;

    APLDESC_ALLOC_MULTI_TRK(descOffset, 1, pJob, gTplGenericDisRead, 0);

    AplDesc_TSB_3DTLCG2_Addons_DisToggle(&gTplGenericDisRead);
    gTplGenericDisRead.field.START_SSD_PTR  = pJob->ssdIdx[pJob->ssdList];
    gTplGenericDisRead.field.TRACK_ID       = pJob->tid;
    gTplGenericDisRead.field.AU_SEG         = pJob->segNum;
    gTplGenericDisRead.field.AU_FORMAT      = pJob->auFmt;
    gTplGenericDisRead.field.START_COL_ADDR = 0;
    gTplGenericDisRead.field.XF_COUNT       = xferSize;

    paa = pJob->paa[0];

    descOffset = 0;
    for (cnt = 0; cnt < READ_DISTRIBUTION_TIMES; cnt++)
    {
        APLDESC_ALLOC_MULTI_TRK(descOffset, MAX_DESC_PER_TRK, pJob, gTplGenericDisRead, 0);

        gTplGenericDisRead.field.FD              = (descOffset == 0) ? 1 : 0;
        gTplGenericDisRead.field.JU              = cnt ? 0 : 1;
        gTplGenericDisRead.field.LD              = ((cnt == (READ_DISTRIBUTION_TIMES - 1)) || CHK_OFFSET_OVER_TH(descOffset, MAX_DESC_PER_TRK)) ? 1 : 0;
        gTplGenericDisRead.field.DEV             = paa.b.ce;
        gTplGenericDisRead.field.CH              = paa.b.ch;
        gTplGenericDisRead.field.SCMD            = (paa.b.lun == 0) ? FC_READ_STATUS_BYTE2 : FC_READ_STATUS_BYTE3;
        gTplGenericDisRead.field.START_ROW_ADDR  =  SET_RW_DESC_ROW_FROM_PAA(paa);

        HalSeq_PushDescriptors((PVOID)&gTplGenericDisRead, sizeof(SeqGenericDescOr_t));
        descOffset++;
        pJob->descCnt++;
    }
    return pJob->descCnt;
}

//-----------------------------------------------------------------------------
/**
    General descriptor for ExitTestMode operation

    @param[in]  pJob            Flash I/O Job pointer
    @param[in]  actDie          active die
    @param[in]  paa             paa

    @return descriptor count
**/
//-----------------------------------------------------------------------------
static U16 DescGeneric_3DTLCG2_ExitTestMode(tFIO_JOB* pJob)
{
    tPAA paa;
    U16 descOffset = 1;
    paa = pJob->paa[0];

    APLDESC_ALLOC_MULTI_TRK(descOffset, 1, pJob, gTplGenericExitTestMode, 0);

    gTplGenericExitTestMode.field.TRACK_ID       = pJob->tid;
    gTplGenericExitTestMode.field.AU_SEG         = pJob->segNum;
    gTplGenericExitTestMode.field.START_SSD_PTR  = pJob->ssdIdx[pJob->ssdList];
    gTplGenericExitTestMode.field.AU_FORMAT      = pJob->auFmt;
    gTplGenericExitTestMode.field.XF_COUNT       = 0;
    gTplGenericExitTestMode.field.AU_NUM         = 0;
    gTplGenericExitTestMode.field.JU             = 0;
    gTplGenericExitTestMode.field.DEV            = paa.b.ce;
    gTplGenericExitTestMode.field.CH             = paa.b.ch;
    gTplGenericExitTestMode.field.FD             = 1;
    gTplGenericExitTestMode.field.LD             = 1;
    gTplGenericExitTestMode.field.START_COL_ADDR = 0;
    gTplGenericExitTestMode.field.START_ROW_ADDR = SET_RW_DESC_ROW_FROM_PAA(paa);

    HalSeq_PushDescriptors((PVOID)&gTplGenericExitTestMode, sizeof(SeqGenericDescOr_t));
    pJob->descCnt++;
    return pJob->descCnt;
}

#include "FastSection.h"
//-----------------------------------------------------------------------------
/**
    General descriptor for Read Distribution operation

    @param[in]  pJob            Flash I/O Job pointer
    @param[in]  actDie          active die
    @param[in]  paa             paa

    @return descriptor count
**/
//-----------------------------------------------------------------------------
static U16 DescGeneric_3DTLCG2_ReadDistribution(tFIO_JOB* pJob)
{
    DescGeneric_3DTLCG2_PreDis(pJob);
    DescGeneric_3DTLCG2_DisToggle(pJob);
    DescGeneric_3DTLCG2_ExitTestMode(pJob);
    return pJob->descCnt;
}

//-----------------------------------------------------------------------------
/**
    General descriptor for two bit decode operation

    @param[in]  pJob            Flash I/O Job pointer

    @return descriptor count
**/
//-----------------------------------------------------------------------------
static U16 DescGeneric_3DTLCG2_NormalTwoBitDecode(tFIO_JOB *pJob)
{
    tPAA paa;
    U16  cnt;
    U8   fd, ju;

    for(cnt = 0; cnt < pJob->paaCnt; cnt++)
    {
        paa = pJob->paa[cnt];
        fd = cnt ?  0 : 1;
        ju = cnt ?  0 : 1;

        DescGeneric_3DTLCG2_HD_Retry_CW(pJob, 0, fd, ju, paa);  // pjob, n-th CW, FD, JU

        /* CW0 SD 5C C5 + SOFT READ LEVEL SET */
        DescGeneric_3DTLCG2_SD_Retry_CW(pJob, 0, paa);  //pjob, n-th CW

        /* CW 1 SD  5C C5 + SOFT READ LEVEL SET */
        DescGeneric_3DTLCG2_HD_Retry_CW(pJob, 1, 0, 0, paa); // pjob, n-th CW, FD, JU

        DescGeneric_3DTLCG2_SD_Retry_CW(pJob, 1, paa);   //pjob, n-th CW
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
static __inline void DescGeneric_3DTLCG2_HD_Retry_CW(tFIO_JOB* pJob, U8 fg_cw, U8 fg_fd, U8 fg_JU, tPAA paa)
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
static __inline void DescGeneric_3DTLCG2_SD_Retry_CW(tFIO_JOB* pJob, U8 fg_cw, tPAA paa)
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

#include "PerfSection.h"
//-----------------------------------------------------------------------------
/**
// Function     : static __inline U16 DescGeneric_3DTLCG2_SetFeature(tFIO_JOB *pJob)
// Description  :
// Input        :
// return       :
**/
//-----------------------------------------------------------------------------
static __inline U16 DescGeneric_3DTLCG2_SetFeature(tFIO_JOB* pJob, tPAA paa, U16 descOffset)
{
    U8* pFeatureCmd;
    U8  lmu;

    if (pJob->flag & FIO_F_SLC) pFeatureCmd = gFeatureCmdSLC;
    else                        pFeatureCmd = gFeatureCmdTLC;

    lmu = paa.b.page % gNfInfo.bitsPerCell;

    gTplGenericVoltageShift.field.TRACK_ID = pJob->tid;
    gTplGenericVoltageShift.field.DEV      = paa.b.ce;
    gTplGenericVoltageShift.field.CH       = paa.b.ch;

    gTplGenericVoltageShift.field.FD             = descOffset ? 0 : 1;
    gTplGenericVoltageShift.field.LD             = 0;
    gTplGenericVoltageShift.field.START_COL_ADDR = (paa.b.lun | pFeatureCmd[lmu] << 8);
    gTplGenericVoltageShift.field.SCMD           = (paa.b.lun == 0) ? FC_READ_STATUS_BYTE2 : FC_READ_STATUS_BYTE3;

    gTplGenericVoltageShift.field.START_ROW_ADDR = SET_RW_DESC_ROW_FROM_PAA(paa);

    if (pFeatureCmd[lmu] == 0x89)      gTplGenericVoltageShift.field.SET_PARAM  = ((U32)G_VALUE << 24) | ((U32)E_VALUE << 16) | ((U32)C_VALUE << 8) | (U32)A_VALUE;
    else if (pFeatureCmd[lmu] == 0x8A) gTplGenericVoltageShift.field.SET_PARAM  = ((U32)F_VALUE << 16) | ((U32)D_VALUE << 8)  | (U32)B_VALUE;
    else if (pFeatureCmd[lmu] == 0x8B) gTplGenericVoltageShift.field.SET_PARAM  = L_VALUE;

    DESC_SET_OPEN_PG_BMP(pJob->tid, descOffset);
    HalSeq_PushDescriptors((PVOID)&gTplGenericVoltageShift, sizeof(SeqGenericDescOr_t));
    descOffset++;
    pJob->descCnt++;

    return descOffset;
}
//=============================================================================
//-------------------------------------------------------------------
// Function     : U16 HldDesc_RegularDescPolling(tFIO_JOB *pJob)
// Description  :
// Input        : tHLD_SEQ_TRK_NODE * pTrkNode, U8 ReadRetry
// return       : N/A
//-------------------------------------------------------------------
static __inline void DescRegular_3DTLCG2_Polling(tFIO_JOB *pJob)
{
    tPAA paa;
    U16 cnt;
    U16 totalCnt = 0;
    U8  cntoffset = 0;
    U8  plane = 0;

    totalCnt  = pJob->paaCnt;

    if (pJob->cmd == FIO_JOB_CMD_WRITE)
    {
        //totalCnt  = pJob->paaCnt;
        totalCnt  = totalCnt / gNfInfo.bitsPerCell;
        cntoffset = 4 * pJob->plane;
    }
    else if (pJob->cmd == FIO_JOB_CMD_ERASE)
    {
        cntoffset = pJob->plane;
    }

    gTplRegularPolling.base.field.TRACK_ID   = pJob->tid;
    gTplRegularPolling.base.field.MP_NUM     = pJob->plane - 1;
    gTplRegularPolling.base.field.BM         = (pJob->flag & FIO_F_NANDTIME) ? 1 : 0;
    gTplRegularPolling.base.field.CMD_ENH2_0 = (pJob->flag & FIO_F_NANDTIME) ? 0 : 5;

    for (cnt = 0; cnt < totalCnt; cnt+=cntoffset)
    {
        paa = pJob->paa[cnt];
        if (pJob->plane == FIO_CMD_1PLANE)
        {
            gTplRegularPolling.START_ROW_ADDR[plane]   = SET_RW_DESC_ROW_FROM_PAA(paa);
            gTplRegularPolling.START_ROW_ADDR[plane+1] = 0;
        }
        else
        {
            ///Only Support Two Plane
            for (plane = 0; plane < pJob->plane; plane++)
            {
                paa.b.plane = plane;
                gTplRegularPolling.START_ROW_ADDR[plane] = SET_RW_DESC_ROW_FROM_PAA(paa);
            }
        }

        gTplRegularPolling.base.field.LD         = (cnt == (totalCnt - cntoffset)) ? 1 : 0;
        gTplRegularPolling.base.field.DEV        = paa.b.ce;
        gTplRegularPolling.base.field.CH         = paa.b.ch;

        pJob->descCnt++;
        HalSeq_PushDescriptors((PVOID)&gTplRegularPolling, sizeof(SeqRegularDescOr_t));
    }
}

#include "FastSection.h"
//-----------------------------------------------------------------------------
/**
    General descriptor template

    @param[in] N/A

    @return N/A
**/
//-----------------------------------------------------------------------------
static __inline void DescGeneric_3DTLCG2_InitTemplate(void)
{
    // Flex Generic
    MEM_CLR(&gTplGenericFlexDescOr,sizeof(SeqGenericFlexDescOr_t));
    gTplGenericFlexDescOr.field.DF       = DESC_FMT_GENERIC;
    gTplGenericFlexDescOr.field.DES_TYPE = DESC_CMD_READ_STATUS;
    gTplGenericFlexDescOr.field.DV       = 1;
    gTplGenericFlexDescOr.field.OR1      = 1;
    gTplGenericFlexDescOr.field.PA       = 1;

    // Erase
    MEM_CLR(&gTplGenericErase, sizeof(SeqGenericDescOr_t));
    gTplGenericErase.field.DF          = DESC_FMT_GENERIC;
    gTplGenericErase.field.DES_TYPE    = DESC_CMD_MLUN | DESC_CMD_PROG;
    gTplGenericErase.field.DV          = 1;
    gTplGenericErase.field.OR1         = 1;
    gTplGenericErase.field.CMD1        = FC_ERASE_BYTE0;
    gTplGenericErase.field.CMD2        = FC_ERASE_BYTE1;
    gTplGenericErase.field.READY       = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericErase.field.RAID_OFFSET = 7;

    // Write
    MEM_CLR(&gTplGenericWrite, sizeof(SeqGenericDescOr_t));
    gTplGenericWrite.field.DF             = DESC_FMT_GENERIC;
    gTplGenericWrite.field.DES_TYPE       = DESC_CMD_MLUN | DESC_CMD_PROG;
    gTplGenericWrite.field.DV             = 1;
    gTplGenericWrite.field.OR1            = 1;
    gTplGenericWrite.field.READY          = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericWrite.field.START_COL_ADDR = 0;
    gTplGenericWrite.field.RAID_OFFSET    = 7;

   // OpenRow
    MEM_CLR(&gTplGenericOpenRow, sizeof(SeqGenericDescOr_t));
    gTplGenericOpenRow.field.DF          = DESC_FMT_GENERIC;
    gTplGenericOpenRow.field.DES_TYPE    = DESC_CMD_MLUN | DESC_CMD_READ;
    gTplGenericOpenRow.field.DV          = 1;
    gTplGenericOpenRow.field.OR1         = 1;
    gTplGenericOpenRow.field.AU_NUM      = 0;
    gTplGenericOpenRow.field.CMD1        = FC_READ_BYTE0;
    gTplGenericOpenRow.field.READY       = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericOpenRow.field.RAID_OFFSET = 7;

    // Read
    MEM_CLR(&gTplGenericRead, sizeof(SeqGenericDescOr_t));
    gTplGenericRead.field.DF          = DESC_FMT_GENERIC;
    gTplGenericRead.field.DES_TYPE    = DESC_CMD_MLUN | DESC_CMD_READ;
    gTplGenericRead.field.DV          = 1;
    gTplGenericRead.field.OR1         = 1;
    gTplGenericRead.field.AU_NUM      = 0;
    gTplGenericRead.field.READY       = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericRead.field.CMD1        = FC_READ_BYTE4_JEDEC;
    gTplGenericRead.field.CMD2        = FC_READ_BYTE5;
    gTplGenericRead.field.OPTION      = SEQ_OPTION_320C03;
    gTplGenericRead.field.RAID_OFFSET = 7;

    // Fast Read
    MEM_CLR(&gTplGenericFastRead, sizeof(SeqGenericDescOr_t));
    gTplGenericFastRead.field.DF          = DESC_FMT_GENERIC;
    gTplGenericFastRead.field.DES_TYPE    = DESC_CMD_MLUN | DESC_CMD_READ;
    gTplGenericFastRead.field.DV          = 1;
    gTplGenericFastRead.field.OR1         = 1;
    gTplGenericFastRead.field.READY       = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericFastRead.field.CMD1        = FC_READ_BYTE0;
    gTplGenericFastRead.field.CMD2        = FC_READ_BYTE1;
    gTplGenericFastRead.field.OPTION      = SEQ_OPTION_3280C8;
    gTplGenericFastRead.field.AU_NUM      = 0;
    gTplGenericFastRead.field.RAID_OFFSET = 7;

    // Full Seq Open
    MEM_CLR(&gTplGenericFullSeqOpen, sizeof(SeqGenericDescOr_t));
    gTplGenericFullSeqOpen.field.DF          = DESC_FMT_GENERIC;
    gTplGenericFullSeqOpen.field.DES_TYPE    = DESC_CMD_MLUN | DESC_CMD_READ;
    gTplGenericFullSeqOpen.field.DV          = 1;
    gTplGenericFullSeqOpen.field.OR1         = 1;
    gTplGenericFullSeqOpen.field.READY       = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericFullSeqOpen.field.CMD1        = FC_READ_BYTE0;
    gTplGenericFullSeqOpen.field.CMD2        = FC_READ_BYTE1;
    gTplGenericFullSeqOpen.field.AU_NUM      = 0;
    gTplGenericFullSeqOpen.field.RAID_OFFSET = 7;
    gTplGenericFullSeqOpen.field.PREFIX_3B   = 0x50;

    // Full Seq Read
    MEM_CLR(&gTplGenericFullSeqRing, sizeof(SeqGenericDescOr_t));
    gTplGenericFullSeqRing.field.DF          = DESC_FMT_GENERIC;
    gTplGenericFullSeqRing.field.DES_TYPE    = DESC_CMD_MLUN | DESC_CMD_READ;
    gTplGenericFullSeqRing.field.DV          = 1;
    gTplGenericFullSeqRing.field.OR1         = 1;
    gTplGenericFullSeqRing.field.READY       = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericFullSeqRing.field.AU_NUM      = 0;
    gTplGenericFullSeqRing.field.RAID_OFFSET = 7;

    // Full Seq Read
    MEM_CLR(&gTplGenericFullSeqRead, sizeof(SeqGenericDescOr_t));
    gTplGenericFullSeqRead.field.DF          = DESC_FMT_GENERIC;
    gTplGenericFullSeqRead.field.DES_TYPE    = DESC_CMD_MLUN | DESC_CMD_READ;
    gTplGenericFullSeqRead.field.DV          = 1;
    gTplGenericFullSeqRead.field.OR1         = 1;
    gTplGenericFullSeqRead.field.READY       = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericFullSeqRead.field.AU_NUM      = 3;
    gTplGenericFullSeqRead.field.RAID_OFFSET = 7;

    // Slc Touch
     MEM_CLR(&gTplGenericSlcTouch, sizeof(SeqGenericDescOr_t));
     gTplGenericSlcTouch.field.DF          = DESC_FMT_GENERIC;
     gTplGenericSlcTouch.field.DES_TYPE    = DESC_CMD_MLUN | DESC_CMD_READ;
     gTplGenericSlcTouch.field.DV          = 1;
     gTplGenericSlcTouch.field.OR1         = 1;
     gTplGenericSlcTouch.field.AU_NUM      = 0;
     gTplGenericSlcTouch.field.CMD1        = FC_READ_BYTE0;
     gTplGenericSlcTouch.field.READY       = FC_STATUS_ENH_READY_BIT_JEDEC;
     gTplGenericSlcTouch.field.RAID_OFFSET = 7;

    // Slc Read
    MEM_CLR(&gTplGenericSlcRead, sizeof(SeqGenericDescOr_t));
    gTplGenericSlcRead.field.DF          = DESC_FMT_GENERIC;
    gTplGenericSlcRead.field.DES_TYPE    = DESC_CMD_MLUN | DESC_CMD_READ;
    gTplGenericSlcRead.field.DV          = 1;
    gTplGenericSlcRead.field.OR1         = 1;
    gTplGenericSlcRead.field.READY       = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericSlcRead.field.PREFIX_3B   = FC_PREFIX_SLC_BYTE;
    gTplGenericSlcRead.field.CMD1        = FC_READ_BYTE0;
    gTplGenericSlcRead.field.CMD2        = FC_READ_BYTE1;
    gTplGenericSlcRead.field.AU_NUM      = 3;
    gTplGenericSlcRead.field.RAID_OFFSET = 7;

    // Voltage Shift
    MEM_CLR(&gTplGenericVoltageShift, sizeof(SeqGenericDescOr_t));
    gTplGenericVoltageShift.field.DF          = DESC_FMT_GENERIC;
    gTplGenericVoltageShift.field.DES_TYPE    = DESC_CMD_MLUN | DESC_CMD_PROG;
    gTplGenericVoltageShift.field.DV          = 1;
    gTplGenericVoltageShift.field.OR1         = 1;
    gTplGenericVoltageShift.field.PA          = 1;
    gTplGenericVoltageShift.field.XF_COUNT    = 8;
    gTplGenericVoltageShift.field.READY       = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericVoltageShift.field.OPTION      = SEQ_OPTION_020859;//SEQ_OPTION_0208D9;
    gTplGenericVoltageShift.field.CMD1        = 0xD5;
    gTplGenericVoltageShift.field.RAID_OFFSET = 7;

    // Read Retry
    MEM_CLR(&gTplGenericReadRetry, sizeof(SeqGenericDescOr_t));
    gTplGenericReadRetry.field.DF          = DESC_FMT_GENERIC;
    gTplGenericReadRetry.field.DES_TYPE    = DESC_CMD_MLUN | DESC_CMD_READ;
    gTplGenericReadRetry.field.DV          = 1;
    gTplGenericReadRetry.field.OR1         = 1;
    gTplGenericReadRetry.field.READY       = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericReadRetry.field.CMD1        = FC_READ_BYTE0;
    gTplGenericReadRetry.field.CMD2        = FC_READ_BYTE1;
    gTplGenericReadRetry.field.AU_NUM      = 0;
    gTplGenericReadRetry.field.OPTION      = SEQ_OPTION_148148;
    gTplGenericReadRetry.field.RAID_OFFSET = 7;

    // PreOpen
    MEM_CLR(&gTplGenericPreOpen, sizeof(SeqGenericDescOr_t));
    gTplGenericPreOpen.field.DF          = DESC_FMT_GENERIC;
    gTplGenericPreOpen.field.DES_TYPE    = DESC_CMD_MLUN | DESC_CMD_READ;
    gTplGenericPreOpen.field.DV          = 1;
    gTplGenericPreOpen.field.OR1         = 1;
    gTplGenericPreOpen.field.AU_NUM      = 0;
    gTplGenericPreOpen.field.CMD1        = FC_READ_BYTE0;
    gTplGenericPreOpen.field.READY       = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericPreOpen.field.HWLL        = 0;
    gTplGenericPreOpen.field.RAID_OFFSET = 7;

    // IDS Read
    MEM_CLR(&gTplGenericIdsRead, sizeof(SeqGenericDescOr_t));
    gTplGenericIdsRead.field.AU_NUM      = 0;
    gTplGenericIdsRead.field.DF          = DESC_FMT_GENERIC;
    gTplGenericIdsRead.field.DES_TYPE    = DESC_CMD_MLUN | DESC_CMD_READ;
    gTplGenericIdsRead.field.DV          = 1;
    gTplGenericIdsRead.field.OR1         = 1;
    gTplGenericIdsRead.field.READY       = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericIdsRead.field.CMD1        = FC_READ_BYTE0;
    gTplGenericIdsRead.field.CMD2        = FC_READ_BYTE1;
    gTplGenericIdsRead.field.OPTION      = SEQ_OPTION_328148;
    gTplGenericIdsRead.field.HWLL        = 1;
    gTplGenericIdsRead.field.RAID_OFFSET = 7;

    // IDS Page Read
    MEM_CLR(&gTplGenericIdsPageRead, sizeof(SeqGenericDescOr_t));
    gTplGenericIdsPageRead.field.DF          = DESC_FMT_GENERIC;
    gTplGenericIdsPageRead.field.DES_TYPE    = DESC_CMD_MLUN | DESC_CMD_READ;
    gTplGenericIdsPageRead.field.DV          = 1;
    gTplGenericIdsPageRead.field.OR1         = 1;
    gTplGenericIdsPageRead.field.AU_NUM      = 3;
    gTplGenericIdsPageRead.field.READY       = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericIdsPageRead.field.CMD1        = FC_READ_BYTE0;
    gTplGenericIdsPageRead.field.CMD2        = FC_READ_BYTE1;
    gTplGenericIdsPageRead.field.OPTION      = SEQ_OPTION_328148;
    gTplGenericIdsPageRead.field.HWLL        = 1;
    gTplGenericIdsPageRead.field.RAID_OFFSET = 7;

    //Random Cache Read
    MEM_CLR(&gTplGenericRandomCacheRead, sizeof(SeqGenericDescOr_t));
    gTplGenericRandomCacheRead.field.DF          = DESC_FMT_GENERIC;
    gTplGenericRandomCacheRead.field.DES_TYPE    = DESC_CMD_MLUN | DESC_CMD_READ;
    gTplGenericRandomCacheRead.field.DV          = 1;
    gTplGenericRandomCacheRead.field.OR1         = 1;
    gTplGenericRandomCacheRead.field.AU_NUM      = 0;
    gTplGenericRandomCacheRead.field.READY       = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericRandomCacheRead.field.CMD1        = FC_READ_BYTE4_JEDEC;
    gTplGenericRandomCacheRead.field.CMD2        = FC_READ_BYTE5;
    gTplGenericRandomCacheRead.field.OPTION      = SEQ_OPTION_140C03;
    gTplGenericRandomCacheRead.field.RAID_OFFSET = 7;

    //EnterTestMode
    MEM_CLR(&gTplGenericEnterTestMode, sizeof(SeqGenericDescOr_t));
    gTplGenericEnterTestMode.field.AU_NUM    = 0;
    gTplGenericEnterTestMode.field.DES_TYPE  = DESC_CMD_MLUN| DESC_CMD_READ;
    gTplGenericEnterTestMode.field.DF        = DESC_FMT_GENERIC;
    gTplGenericEnterTestMode.field.READY     = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericEnterTestMode.field.DV        = 1;
    gTplGenericEnterTestMode.field.OR1       = 1;

    // ExitTestMode
    MEM_CLR(&gTplGenericExitTestMode, sizeof(SeqGenericDescOr_t));
    gTplGenericExitTestMode.field.DF          = DESC_FMT_GENERIC;
    gTplGenericExitTestMode.field.DES_TYPE    = DESC_CMD_MLUN | DESC_CMD_READ;
    gTplGenericExitTestMode.field.DV          = 1;
    gTplGenericExitTestMode.field.OR1         = 1;
    gTplGenericExitTestMode.field.AU_NUM      = 0;
    gTplGenericExitTestMode.field.READY       = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericExitTestMode.field.CMD1        = FC_RESET;
    gTplGenericExitTestMode.field.CMD2        = 0x00;
    gTplGenericExitTestMode.field.OPTION      = SEQ_OPTION_208CB;
    gTplGenericExitTestMode.field.SCMD        = FC_READ_STATUS_BYTE0;
    gTplGenericExitTestMode.field.RAID_OFFSET = 7;

    // gTplGenericSoftBitXor
    MEM_CLR(&gTplGenericSoftBitXor, sizeof(SeqGenericDescOr_t));
    gTplGenericSoftBitXor.field.DF          = DESC_FMT_GENERIC;
    gTplGenericSoftBitXor.field.DES_TYPE    = DESC_CMD_MLUN | DESC_CMD_READ;
    gTplGenericSoftBitXor.field.DV          = 1;
    gTplGenericSoftBitXor.field.OR1         = 1;
    gTplGenericSoftBitXor.field.READY       = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericSoftBitXor.field.CMD1        = FC_READ_BYTE0;
    gTplGenericSoftBitXor.field.CMD2        = FC_READ_BYTE1;
    gTplGenericSoftBitXor.field.SCMD        = FC_READ_STATUS_BYTE0;
    gTplGenericSoftBitXor.field.OPTION      = SEQ_OPTION_0280C8;
    gTplGenericSoftBitXor.field.AU_NUM      = 0;
    gTplGenericSoftBitXor.field.PREFIX_3B   = 0xC2;
    gTplGenericSoftBitXor.field.RAID_OFFSET = 7;

    // CmdAddrData
    MEM_CLR(&gTplGenericCmdAddrData, sizeof(SeqGenericDescOr_t));
    gTplGenericCmdAddrData.field.AU_NUM    = 0;
    gTplGenericCmdAddrData.field.DES_TYPE  = DESC_CMD_MLUN | DESC_CMD_READ;
    gTplGenericCmdAddrData.field.DF        = DESC_FMT_GENERIC;
    gTplGenericCmdAddrData.field.READY     = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericCmdAddrData.field.DV        = 1;
    gTplGenericCmdAddrData.field.OR1       = 1;

    // PreDisRead
    MEM_CLR(&gTplGenericPreDisRead, sizeof(SeqGenericDescOr_t));
    gTplGenericPreDisRead.field.DES_TYPE    = DESC_CMD_MLUN | DESC_CMD_READ;
    gTplGenericPreDisRead.field.DF          = DESC_FMT_GENERIC;
    gTplGenericPreDisRead.field.READY       = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericPreDisRead.field.DV          = 1;
    gTplGenericPreDisRead.field.AU_NUM      = 0;
    gTplGenericPreDisRead.field.OR1         = 1;
    gTplGenericPreDisRead.field.RAID_OFFSET = 7;

    //DisRead
    MEM_CLR(&gTplGenericDisRead, sizeof(SeqGenericDescOr_t));
    gTplGenericDisRead.field.DES_TYPE    = DESC_CMD_MLUN | DESC_CMD_READ;
    gTplGenericDisRead.field.DF          = DESC_FMT_GENERIC;
    gTplGenericDisRead.field.READY       = FC_STATUS_ENH_READY_BIT_JEDEC;
    gTplGenericDisRead.field.DV          = 1;
    gTplGenericDisRead.field.AU_NUM      = 3;
    gTplGenericDisRead.field.OR1         = 1;
    gTplGenericDisRead.field.RAID_OFFSET = 7;
}

//-------------------------------------------------------------------
// Function     : static void DESC_InitRegularTemplate(void)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static __inline void DescRegular_3DTLCG2_InitTemplate(void)
{
    MEM_CLR(&gTplRegularPolling, sizeof(SeqRegularDescOr_t));

    gTplRegularPolling.base.field.DF          = DESC_FMT_REGULAR;
    gTplRegularPolling.base.field.DES_TYPE    = DESC_CMD_MLUN | DESC_CMD_MP_READ_STATUS;
    gTplRegularPolling.base.field.DV          = 1;
    gTplRegularPolling.base.field.OR1         = 1;
    gTplRegularPolling.base.field.DES_RDSR    = 1;
    gTplRegularPolling.base.field.SA          = 1;
    gTplRegularPolling.base.field.RR          = 1;
}
