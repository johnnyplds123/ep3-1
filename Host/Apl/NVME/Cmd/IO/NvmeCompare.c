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
#include "HalNvmeXfer.h"

// APL
#include "HldNvme.h"
#include "SysInfo.h"
#include "NvmeGetLogPage.h"
#include "Cache.h"

// Code Section
#include "NormSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief NVMe I/O Command structure - Compare
typedef struct
{
    IoCommandCommon_t DW9_0;    ///< DW9..0

    uint64_t SLBA;          ///< Starting LBA (DW11..10)

    uint32_t NLB:16;        ///< Number of Logical Blocks (DW12[15:0])
    uint32_t reserved12:10; ///< Reserved (DW12[25:16])
    uint32_t PRINFO:4;      ///< Protection Information Field (DW12[29:26])
    uint32_t FUA:1;         ///< Force Unit Access (DW12[30])
    uint32_t LR:1;          ///< Limited Retry (DW12[31])

    uint32_t reserved13;    ///< DW13

    uint32_t EILBRT;        ///< Expected Initial Logical Block Reference Tag (DW14)
    uint32_t ELBAT:16;      ///< Expected Logical Block Application Tag (DW15[15:0])
    uint32_t ELBATM:16;     ///< Expected Logical Block Application Tag Mask (DW15[31:16])

} NvmeIoCompareCommand_t;
COMPILE_ASSERT(sizeof(NvmeIoCompareCommand_t)==64, "NVMe command size shall be 64");

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static void Compare_WriteXferDone(HcmdQ_t* pHcmdQ);
       void Compare_ReadMediaDone(HcmdQ_t* pHcmdQ);
static void Compare_Transfer(HcmdQ_t* pHcmdQ, uint8_t XferH2d);
static U8   Cache_Compare(HcmdQ_t* pHcmdQ);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported data prototype
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function proto-tpye
//-----------------------------------------------------------------------------
extern Error_t HalNvme_RequestDataTransferIoQuick(MsgHostIO_t* pHcmdMsg);

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
    NvmeCmd_Compare

    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_Compare(HcmdQ_t* pHcmdQ)
{
    AddressInfo_t* pAddrInfo;
    MsgHostIO_t*   pHcmdMsg;

    pAddrInfo = GetHcmdAddressInfoPtr(pHcmdQ);

    BEGIN_HOST_DBG_DUMP(DBG_DUMP_HOST_NONRW)
        D_PRINTF("%s  lba[%x], nb[%x]\n", __FUNCTION__, pAddrInfo->lba, pAddrInfo->nb);
    END_HOST_DBG_DUMP

    Compare_Transfer(pHcmdQ, cXferH2d);

    pHcmdMsg = (MsgHostIO_t *)GetHcmdMsgPtr(pHcmdQ);
    pHcmdMsg->bufDesc.ssdIndex = SSD_CMP_START;
    pHcmdQ->bufDesc.ssdIndex   = SSD_CMP_START;

    SetHcmdHandler(pHcmdQ, Compare_WriteXferDone);
    HalNvme_RequestDataTransferIoQuick(pHcmdMsg);
    pHcmdMsg->hdr.b.status = cMsgXfered;
    Host_IoXferReq();
}

//-----------------------------------------------------------------------------
/**
    Compare_WriteXferDone

    @param[in]  pHcmdQ     Host command queue pointer
**/
//-----------------------------------------------------------------------------
static void Compare_WriteXferDone(HcmdQ_t* pHcmdQ)
{
    MsgHostIO_t* pHcmdMsg;

    pHcmdMsg = (MsgHostIO_t *)GetHcmdMsgPtr(pHcmdQ);
    pHcmdMsg->bufDesc.ssdCnt = 0;

    // TODO: what if compare cmd xfer error?

    Compare_Transfer(pHcmdQ, cXferD2h);
}

//-----------------------------------------------------------------------------
/**
    Compare_ReadMediaDone

    @param[in]  pHcmdQ     Host command queue pointer
**/
//-----------------------------------------------------------------------------
void Compare_ReadMediaDone(HcmdQ_t* pHcmdQ)
{
    if (Cache_Compare(pHcmdQ))
    {
        pHcmdQ->error = MK_NVME_ERROR_CODE(cCompareFailure, cMediaErrors, cNoMoreStatusInformation, cDoNotRetry);
    }

    BEGIN_CS1
    NvmeCmd_ReturnCacheMsg(pHcmdQ);
    END_CS1
}

//-----------------------------------------------------------------------------
/**
    Compare_Transfer

    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
static void Compare_Transfer(HcmdQ_t* pHcmdQ, XferDir_t xferDir)
{
    MsgHostIO_t* pHcmdMsg;

    pHcmdMsg = (MsgHostIO_t *)GetHcmdMsgPtr(pHcmdQ);

    if (xferDir == cXferH2d)
    {
        XferAttr_t AttrInfo;
        uint32_t GroupId;
        uint32_t Bitmap;
        uint16_t cmdIndex;

        AttrInfo.all = 0;
        AttrInfo.b.cmdId     = GetHcmdId(pHcmdQ);
        AttrInfo.b.direction = xferDir;
        AttrInfo.b.autoEoC   = pHcmdQ->attr.b.autoEoC;

        cmdIndex = GET_COMMAND_INDEX(GetHcmdId(pHcmdQ));
        GroupId  = cmdIndex >> 5;
        Bitmap   = (1 << (cmdIndex & 0x1F));

        SET_IO_XFER_PARAM_FROM_INDEX(cmdIndex, (uint32_t) pHcmdQ);
        SET_IO_ATTR_FROM_INDEX(cmdIndex, AttrInfo.all);

        // Set a bitmap whether a command is transferable.
        NvmeHba.bitmapXferable[GroupId] |= Bitmap;

        // Set a Cabm fw bitmaps to check transfer done or command completion
        NvmeHba.bitmapFwCabm[GroupId] |= Bitmap;
        NvmeHba.bitmapFwCabmGroup |= BIT(GroupId);

        MSG_SET_HOST_INIT(pHcmdMsg, cMcCompareCmd);

        pHcmdMsg->attrInfo.all     = AttrInfo.all;
        pHcmdMsg->bufDesc.allFlags = 0;
        pHcmdMsg->bufDesc.ssdCnt   = 0;
    }
    else
    {
        uint16_t nsId0;
        uint16_t lbaf;

        nsId0 = pHcmdQ->addrInfo.nsId0;
        lbaf  = gNvmeIfMgr.ns[nsId0].LBAF;

        pHcmdMsg->laa            =  (pHcmdQ->addrInfo.lba)                                                   >> cLBAFTable[lbaf].mbu_shift;
        pHcmdMsg->bufDesc.ssdCnt = ((pHcmdQ->addrInfo.lba + pHcmdQ->addrInfo.nb + cLBAFTable[lbaf].mbu_mask) >> cLBAFTable[lbaf].mbu_shift) - pHcmdMsg->laa;

        pHcmdMsg->hdr.b.status = cMsgPosted;
        Host_MediaReq();
        IPC_SendMsgQ(cH2cReqQueue, (Msg_t*) pHcmdMsg);
    }
}

//-----------------------------------------------------------------------------
/**
    Cache_Compare

    @param[in]  pHcmdQ     Host command queue pointer
**/
//-----------------------------------------------------------------------------
static U8 Cache_Compare(HcmdQ_t* pHcmdQ)
{
    MsgHostIO_t* pHcmdMsg;
    AddressInfo_t* pAddrInfo;
    uint8_t* pWriteBuf;
    uint8_t* pReadBuf;
    uint32_t totalSize;
    uint32_t cmpSize;
    uint32_t byteOffset;
    uint16_t cacheIdx;
    uint16_t nsId0;
    uint16_t lbaf;

    nsId0 = pHcmdQ->addrInfo.nsId0;
    lbaf  = gNvmeIfMgr.ns[nsId0].LBAF;

    pHcmdMsg   = (MsgHostIO_t *)GetHcmdMsgPtr(pHcmdQ);
    pAddrInfo  = GetHcmdAddressInfoPtr(pHcmdQ);
    totalSize  = LBU2BYTE(pHcmdQ->addrInfo.nb);

    byteOffset = (pAddrInfo->lba & cLBAFTable[lbaf].mbu_mask) * HLBU_DATA_SIZE;
    cacheIdx   = pHcmdMsg->bufDesc.ssdIndex;

    //D_PRINTF("Lba[%X] Len[%X] | Laa[%X] Len[%X] Offset[%X]\n", pAddrInfo->laa, pAddrInfo->nb, pHcmdMsg->laa, pHcmdMsg->bufDesc.ssdCnt, byteOffset);

    pWriteBuf = (uint8_t*)((smSSD[SSD_CMP_START].startAddr << SSD_ADDR_SHIFT) + byteOffset);
    pReadBuf  = (uint8_t*)((smSSD[cacheIdx].startAddr      << SSD_ADDR_SHIFT) + byteOffset);

    if (pHcmdMsg->bufDesc.ssdCnt > 1)   cmpSize = MBU_SIZE - byteOffset;
    else                                cmpSize = totalSize;

    while (totalSize)
    {
        //D_PRINTF("WBuf[%x] RBuf[%x] CmpSize[%x]\n", (uint32_t)pWriteBuf, (uint32_t)pReadBuf, cmpSize);
        if (MEM_CMP(pWriteBuf, pReadBuf, cmpSize))
        {
            D_PRINTF("CmpErr: W[%X]  R[%X]\n", *((uint32_t*)&pWriteBuf[0]), *((uint32_t*)&pReadBuf[0]));
            return 1;
        }

        pWriteBuf += cmpSize;
        totalSize -= cmpSize;

        cacheIdx   = GET_SSD_NEXT_INDEX_FROM_INDEX(cacheIdx);
        pReadBuf   = (uint8_t*)(smSSD[cacheIdx].startAddr << SSD_ADDR_SHIFT);
        byteOffset = 0;

        cmpSize = ((totalSize >= MBU_SIZE) ? MBU_SIZE : totalSize);
    }

    return 0;
}
