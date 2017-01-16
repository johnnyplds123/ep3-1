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
//! @brief
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
// PKG
#include "BuildPkg.h"
#include "SharePkg.h"
#include "HostPkg.h"
#include "NvmeCmdPkg.h"

// REG

// HAL
#include "HalCpu.h"
#include "HalNvmeXfer.h"

// APL
#include "HldNvme.h"
#include "Cache.h"
#include "SysInfo.h"
#include "UncTable.h"

// Code Section
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
/// @brief NVMe I/O Command structure - Dataset Management
typedef struct
{
    IoCommandCommon_t DW9_0;    ///< DW9..0

    uint32_t NR:8;          ///< Number of Ranges (DW10[7:0])
    uint32_t reserved10:24; ///< Reserved (DW10[31:8])

    uint32_t IDR:1;         ///< Attribute - Integral Dataset for Read (DW11[0])
    uint32_t IDW:1;         ///< Attribute - Integral Dataset for Write (DW11[1])
    uint32_t AD:1;          ///< Attribute - Deallocate (DW11[2])
    uint32_t reserved11:29; ///< Reserved (DW11[31:3])

    uint32_t reserved12[4]; ///< Dataset Management (DW15..12)

} NvmeIoDatasetManagementCommand_t;
COMPILE_ASSERT(sizeof(NvmeIoDatasetManagementCommand_t)==64, "NVMe command size shall be 64");

/// @brief Dataset Management Range Definition
typedef struct
{
    struct
    {
        uint32_t AF:4;             ///< Access Frequency  (Bit[03:00])
        uint32_t AL:2;             ///< Access Latency    (Bit[05:04])
        uint32_t reserved6:2;      ///< Reserved          (Bit[07:06])
        uint32_t SR:1;             ///< Sequential Read Range (Bit[08])
        uint32_t SW:1;             ///< Sequential Write Range (Bit[09])
        uint32_t WP:1;             ///< Write Prepare (Bit[10])
        uint32_t reserved11:13;    ///< Reserved (Bit[23:11])
        uint32_t CommandAccessSize:8;    ///< Command Access Size in single Read/Write command (Bit[31:24])

    } ContextAttributes;

    uint32_t nb;
    uint64_t lba;

} NvmeIoDatasetManagementRangeFmt_t;
COMPILE_ASSERT(sizeof(NvmeIoDatasetManagementRangeFmt_t)==16, "Dataset Management Range size shall be 16");

/// @brief Dataset Management Info
typedef struct
{
    U32 NSID;

    struct
    {
        uint32_t NR:8;          ///< Number of Ranges (DW10[7:0])
        uint32_t reserved10:24; ///< Reserved (DW10[31:8]
    } DW10;

    struct
    {
        uint32_t IDR:1;         ///< Attribute - Integral Dataset for Read (DW11[0])
        uint32_t IDW:1;         ///< Attribute - Integral Dataset for Write (DW11[1])
        uint32_t AD:1;          ///< Attribute - Deallocate (DW11[2])
        uint32_t reserved11:29; ///< Reserved (DW11[31:3])
    } DW11;
} NvmeIoDatasetManagementInfo_t;

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static __inline uint16_t NvmeDataSetParsebuf(uint8_t* pSrcBuf, uint8_t* pDestBuf, uint32_t entryCnt, HcmdQ_t* pHcmdQ, uint32_t nsId0);
void NvmeCmd_HandleTrimEvent(HcmdQ_t* pHcmdQ);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
NvmeIoDatasetManagementInfo_t   gTrimInfo[HW_MAX_IO_CMD_INDEX];

//-----------------------------------------------------------------------------
//  Imported data prototype
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function proto-tpye
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**


    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_DatasetManagement(HcmdQ_t* pHcmdQ)
{
    NvmeIoDatasetManagementInfo_t*    pTrimInfo;
    NvmeIoDatasetManagementCommand_t* pCdb;

    pTrimInfo = &gTrimInfo[pHcmdQ->cmdId];
    MEM_CLR(pTrimInfo, sizeof(NvmeIoDatasetManagementInfo_t));

    pCdb = (NvmeIoDatasetManagementCommand_t *)NvmeCmd_GetCdbPointer(pHcmdQ);
    pTrimInfo->NSID     = pCdb->DW9_0.NSID;
    pTrimInfo->DW11.AD  = pCdb->AD;
    pTrimInfo->DW11.IDR = pCdb->IDR;
    pTrimInfo->DW11.IDW = pCdb->IDW;
    pTrimInfo->DW10.NR  = pCdb->NR;

    SetHcmdHandler(pHcmdQ, NvmeCmd_HandleTrimEvent);

    Host_PushToTail(cHcsCmdWait, pHcmdQ);
}

//-----------------------------------------------------------------------------
/**
    Trim Event handler.
**/
//-----------------------------------------------------------------------------
void NvmeCmd_HandleTrimEvent(HcmdQ_t* pHcmdQ)
{
    MsgTrim_t* pTrimMsg;
    NvmeIoDatasetManagementInfo_t* pTrimInfo;
    uint32_t transferSize;
    uint8_t* pTrimBuf;
    uint8_t* pRangeBuf;
    uint16_t entryCnt;

    pTrimInfo = &gTrimInfo[pHcmdQ->cmdId];

    pHcmdQ->bufDesc.ssdIndex  = SSD_IF_START;
    pHcmdQ->bufDesc.flowCtrl  = cXferNonStreaming;
    pHcmdQ->attr.b.xferNotify = TRUE;
    pHcmdQ->fptr              = NULL;

    pRangeBuf = (uint8_t *)NvmeCmd_GetScratchBufferPointer();
    pTrimBuf  = (uint8_t *)NvmeCmd_GetTrimBufferPointer(pHcmdQ->cmdId);

    BEGIN_HOST_DBG_DUMP(DBG_DUMP_TRIM_MSG)
        D_PRINTF("CID[%03d]DataBuff[%08x]TrimBuff[%08x]\n", pHcmdQ->cmdId, pRangeBuf, pTrimBuf);
        D_PRINTF("%s, IDR[%02x], IDW[%02x], AD[%02x], NR=%3u\n", __FUNCTION__, pTrimInfo->DW11.IDR, pTrimInfo->DW11.IDW, pTrimInfo->DW11.AD, pTrimInfo->DW10.NR+1);
    END_HOST_DBG_DUMP

    BEGIN_CS1
    if (gNvmeIfMgr.state == cNvmeIfReady)
    {
        AplHost_TransferOneNonMedia(pHcmdQ, (pTrimInfo->DW10.NR + 1) * 0x10);
    }
    END_CS1

    while (TRUE)
    {
        if (pHcmdQ->attr.b.xferNotify == FALSE)
        {
            break;
        }

        if (gNvmeIfMgr.state != cNvmeIfReady)
        {
            pHcmdQ->error = MK_NVME_ERROR_CODE(cDataTransferError, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
            NvmeCmd_Complete(pHcmdQ);
            return;
        }
    }

    // Trim Operation
    if (pTrimInfo->DW11.AD)
    {
        entryCnt = NvmeDataSetParsebuf(pRangeBuf, pTrimBuf, pTrimInfo->DW10.NR + 1, pHcmdQ, pTrimInfo->NSID - 1);

        if (entryCnt && pHcmdQ->error == cEcNoError)
        {
            pTrimMsg = (MsgTrim_t*) GetHcmdMsgPtr(pHcmdQ);
            MSG_SET_HOST_INIT(pTrimMsg, cMcTrim);
            pTrimMsg->pBuffer  = pTrimBuf;
            pTrimMsg->entryCnt = entryCnt;
            MEM_CLR(pTrimMsg->param, sizeof(pTrimMsg->param));

            Host_MediaReq();
            IPC_SendMsgQ(cH2cReqQueue, (Msg_t*) pTrimMsg);
            return;
        }
    }

    NvmeCmd_Complete(pHcmdQ);
}

//-----------------------------------------------------------------------------
/**
    Rearrange data set management buffer.

    @param[in]  pSrcBuf    Original data set management buffer pointer
    @param[in]  pDestBuf   Re-arrange buffer pointer
    @param[in]  entryCnt   Data set management entry count
    @param[in]  pHcmdQ     Host command queue pointer
    @param[in]  nsId0      namespace id (zero base)
**/
//-----------------------------------------------------------------------------
static __inline uint16_t NvmeDataSetParsebuf(uint8_t* pSrcBuf, uint8_t* pDestBuf, uint32_t entryCnt, HcmdQ_t* pHcmdQ, uint32_t nsId0)
{
    NvmeIoDatasetManagementRangeFmt_t* pNvmeDataSet;
    FioTrimFmt_t*                      pFioDataSet;
    uint64_t newLen;
    uint64_t newLba;
    uint16_t newIdx;
    uint16_t currIdx;
    uint16_t lbaf;

    lbaf         = gNvmeIfMgr.ns[nsId0].LBAF;
    pNvmeDataSet = (NvmeIoDatasetManagementRangeFmt_t*)&gFastDataSetBuf[0];
    pFioDataSet  = (FioTrimFmt_t*)                     &gFastDataSetBuf[cNvmeDataSetBufSize / 2];

    MEM_COPY(pNvmeDataSet, pSrcBuf, entryCnt * sizeof(NvmeIoDatasetManagementRangeFmt_t));

    newIdx  = 0;
    newLen  = 0;
    currIdx = 0;

    while (currIdx < entryCnt)
    {
        if (pNvmeDataSet[currIdx].nb == 0) break;     // or continue?

        if ((pNvmeDataSet[currIdx].lba + pNvmeDataSet[currIdx].nb) > gNvmeIfMgr.ns[nsId0].NSZE)
        {
            pHcmdQ->error = MK_NVME_ERROR_CODE(cLbaOutOfRange, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
            gNvmeIfMgr.lastErrorBitOffset  = 0;
            gNvmeIfMgr.lastErrorByteOffset = (pNvmeDataSet[currIdx].lba >= gNvmeIfMgr.ns[nsId0].NSZE) ? ERR_DW10_OFFSET : ERR_DW12_OFFSET; // error parameter, SLBA or NLB ?
            return 0;
        }

        if (currIdx == 0)
        {
            newLba = pNvmeDataSet[currIdx].lba;
            newLen = pNvmeDataSet[currIdx].nb;
        }
        else if ((newLba + newLen) == pNvmeDataSet[currIdx].lba)
        {
            newLen += pNvmeDataSet[currIdx].nb;
        }
        else
        {
            pFioDataSet[newIdx].laaStart = ((newLba + cLBAFTable[lbaf].mbu_mask) >> cLBAFTable[lbaf].mbu_shift) + gNvmeIfMgr.ns[nsId0].mbu_start;
            pFioDataSet[newIdx].laaEnd   = ((newLba + newLen)                    >> cLBAFTable[lbaf].mbu_shift) + gNvmeIfMgr.ns[nsId0].mbu_start;

            UNCT_CHECK_DO_DELETE((newLba << cLBAFTable[lbaf].hbu_shift) + (gNvmeIfMgr.ns[nsId0].mbu_start << HBU_PER_MBU_SFT), newLen << cLBAFTable[lbaf].hbu_shift, DISABLE);

            newIdx++;

            newLba = pNvmeDataSet[currIdx].lba;
            newLen = pNvmeDataSet[currIdx].nb;
        }

        currIdx++;
    }

    pFioDataSet[newIdx].laaStart = ((newLba + cLBAFTable[lbaf].mbu_mask) >> cLBAFTable[lbaf].mbu_shift) + gNvmeIfMgr.ns[nsId0].mbu_start;
    pFioDataSet[newIdx].laaEnd   = ((newLba + newLen)                    >> cLBAFTable[lbaf].mbu_shift) + gNvmeIfMgr.ns[nsId0].mbu_start;
    newIdx++;

    UNCT_CHECK_DO_DELETE((newLba << cLBAFTable[lbaf].hbu_shift) + (gNvmeIfMgr.ns[nsId0].mbu_start << HBU_PER_MBU_SFT), newLen << cLBAFTable[lbaf].hbu_shift, ENABLE);

    MEM_COPY((PVOID)pDestBuf, (PVOID)pFioDataSet, sizeof(FioTrimFmt_t) * newIdx);

    ASM_DMB();

    return newIdx;
}
