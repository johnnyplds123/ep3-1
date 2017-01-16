// ============================================================================
//
// ==        Copyright (c) LITEON Corporation  -  All rights reserved        ==
//
//  This computer program contains confidential and proprietary information,
//  and may NOT be reproduced or transmitted, in whole or in part, in any form,
//  or by any means electronic, mechanical, photo-optical, or otherwise, and
//  may NOT be translated into another language without the express written
//  permission from LITEON Corporation.
//
// ==        Copyright (c) LITEON Corporation  -  All rights reserved        ==
//
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
#include "RegNvmeCmdFetch.h"
// HAL
#include "HldNvme.h"
#include "HalNvmeXfer.h"
// APL
#include "NvmeDirective.h"
// Code Section
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
NvmeDirectiveMgr_t  gDirectiveMgr;
uint32_t*           pCmdfBuf;
uint8_t             gCmdfPtr = 0;

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
    Directive Initial
**/
//-----------------------------------------------------------------------------
void Directive_WriteHandling(MsgHostIO_t* pHcmdMsg, uint32_t nsid0)
{
    uint32_t sqid    = 0;
    uint32_t opcode  = 0;
    uint32_t counter = 0;

    while (counter++ < HW_MAX_IO_CMD_INDEX)
    {
        sqid   = (*(pCmdfBuf + (gCmdfPtr * CMDF_DW_PER_CMD                  )) & CMDF_MASK_SQID);
        opcode = (*(pCmdfBuf + (gCmdfPtr * CMDF_DW_PER_CMD + CMDF_OFFSET_DW0)) & CMDF_MASK_OPC);

        if (sqid && (opcode == cWrite))
        {
            if (gDirectiveMgr.en_Stream)
            {
                uint32_t cmfDtype = 0;

                cmfDtype = (*(pCmdfBuf + (gCmdfPtr * CMDF_DW_PER_CMD + CMDF_OFFSET_DW12)) & CMDF_MASK_DTYPE) >> CMDF_SHIFT_DTYPE;

                if (cmfDtype == DIRE_TYPE_STREAMS)
                {
                    uint32_t idx;
                    uint16_t streamId = 0;

                    streamId = (*(pCmdfBuf + (gCmdfPtr * CMDF_DW_PER_CMD + CMDF_OFFSET_DW13)) & CMDF_MASK_DSPEC) >> CMDF_SHIFT_DSPEC;

                    for (idx = 0; idx < gDirectiveMgr.streamInfo[nsid0].openCnt; idx++)
                    {
                        if (gDirectiveMgr.streamInfo[nsid0].idTable[idx] == streamId)
                        {
                            break;
                        }
                    }

                    if (idx == gDirectiveMgr.streamInfo[nsid0].openCnt)
                    {
                        if (gDirectiveMgr.streamInfo[nsid0].openCnt < gDirectiveMgr.streamInfo[nsid0].allocCnt)
                        {
                            gDirectiveMgr.streamInfo[nsid0].idTable[idx] = streamId;
                            gDirectiveMgr.streamInfo[nsid0].openCnt++;
                        }
                        else
                        {
                            streamId = 0;
                        }
                    }

                    pHcmdMsg->attrInfo.b.nsid     = nsid0;
                    pHcmdMsg->attrInfo.b.streamId = streamId;

                    #if 0
                    if (gDirectiveMgr.StreamlookupTBL[streamId] != streamId)
                    {
                        gDirectiveMgr.StreamlookupTBL[streamId] = streamId;
                        gDirectiveMgr.streamInfo[nsid0].openCnt++;
                    }
                    #endif
                }
            }

            rNvmeCmdFetch.cmdFetcherAxiBufCntrDecrease.b.CMDF_BUF_CNTR_DEC = 1;
            gCmdfPtr++;
            break;
        }

        rNvmeCmdFetch.cmdFetcherAxiBufCntrDecrease.b.CMDF_BUF_CNTR_DEC = 1;
        gCmdfPtr++;
    }
}

#include "NormSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
/**
    Directive Initial
**/
//-----------------------------------------------------------------------------
void Directive_Init(InitBootMode_t initMode)
{
#if (SUP_DIRE_STREAMS == ENABLE)
    pCmdfBuf = (uint32_t*)MEM_AllocBuffer(HW_MAX_IO_CMD_INDEX * CMDF_SIZE_PER_CMD, 512);
    gCmdfPtr = 0;

    rNvmeCmdFetch.cmdFetcherCfg.b.CMDF_DEST_MD         = 3;
    rNvmeCmdFetch.CMDF_AXI_BUF_ADDR                    = (uint32_t)pCmdfBuf;
    rNvmeCmdFetch.cmdFetcherAxiBufSize.b.CMDF_BUF_SIZE = HW_MAX_IO_CMD_INDEX;
#endif

    MEM_CLR(&gDirectiveMgr, sizeof(gDirectiveMgr));
}

//-----------------------------------------------------------------------------
/**
    Directive Receive - Retrun Parameters
**/
//-----------------------------------------------------------------------------
Error_t Identify_OP_ReturnParameter(DirecIdentifyReturnPara_t* pReturnPara)
{
    // --- Directives Support
    pReturnPara->supp_Identify  = ENABLE;
    pReturnPara->supp_Streams   = SUP_DIRE_STREAMS;
    pReturnPara->supp_AdvBgCon  = SUP_DIRE_ADVBGCTRL;

    // --- Directives Enable
    pReturnPara->enab_Identify  = ENABLE;
    pReturnPara->enab_Streams   = gDirectiveMgr.en_Stream;
    pReturnPara->enab_AdvBgCon  = gDirectiveMgr.en_AdvBgCtrl;

    // It's not support in the latest Directive TP
    // --- Event Notification Support
    pReturnPara->evno_Identify  = DISABLE;
    pReturnPara->evno_Streams   = SUP_EVENTNOTI_STREAMS;
    pReturnPara->evno_AdvBgCon  = SUP_EVENTNOTI_ADVBGCTRL;

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Directive Receive - Event Notification
**/
//-----------------------------------------------------------------------------
Error_t Identify_OP_EventNotification(NvmeAdminDirectiveRecieveCommand_t* pCdb)
{
    // According to pCdb->event_DTYPE to provide asyc Event notification
    U8 errFlag = FALSE;

    switch(pCdb->event_DTYPE)
    {
        case DIRE_TYPE_IDENTIFY:
            errFlag = TRUE;
            break;
        case DIRE_TYPE_STREAMS:
            if ((SUP_EVENTNOTI_STREAMS & gDirectiveMgr.en_Stream) == DISABLE)
            {
                errFlag = TRUE;
            }
            break;
        case DIRE_TYPE_ADVBGCTRL:
            if ((SUP_EVENTNOTI_ADVBGCTRL & gDirectiveMgr.en_AdvBgCtrl) == DISABLE)
            {
                errFlag = TRUE;
            }
            break;
        default:
            errFlag = TRUE;
            break;
    }

    if (errFlag)
    {
        gNvmeIfMgr.lastErrorBitOffset   = 0;
        gNvmeIfMgr.lastErrorByteOffset  = ERR_DW12_OFFSET; // DW12, DTYPE
        return MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
    }
    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Directive Send - Enable Directive
**/
//-----------------------------------------------------------------------------
Error_t Identify_OP_EnableDirective(NvmeAdminDirectiveSendCommand_t* pCdb)
{
    U8 errFlag = FALSE;

    switch(pCdb->enable_DTYPE)
    {
        case DIRE_TYPE_IDENTIFY:
            if (pCdb->enable_ENDIR == DISABLE)
            {
                errFlag = TRUE;
            }
            break;
        case DIRE_TYPE_STREAMS:
            if(SUP_DIRE_STREAMS == DISABLE)
            {
                errFlag = TRUE;
            }
            else
            {
                gDirectiveMgr.en_Stream = pCdb->enable_ENDIR;
            }
            break;
        case DIRE_TYPE_ADVBGCTRL:
            if(SUP_DIRE_ADVBGCTRL == DISABLE)
            {
                errFlag = TRUE;
            }
            else
            {
                gDirectiveMgr.en_AdvBgCtrl = pCdb->enable_ENDIR;
            }
            break;
        default:
            errFlag = TRUE;
            break;
    }
    if (errFlag)
    {
        gNvmeIfMgr.lastErrorBitOffset   = 0;
        gNvmeIfMgr.lastErrorByteOffset  = ERR_DW12_OFFSET + 1; // DW12, Enable DTYPE
        return MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
    }
    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Directive Send - Event Clear
**/
//-----------------------------------------------------------------------------
Error_t Identify_OP_EventClear(NvmeAdminDirectiveSendCommand_t* pCdb)
{
    // Complete specific DTYPE Event Notification
    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Stream Receive - Retrun Parameters
**/
//-----------------------------------------------------------------------------
Error_t Stream_OP_ReturnParameter(StreamReturnPara_t* pReturnPara, uint32_t nsid)
{
    U32 nsidIdx;
    U32 tolAllocCnt = 0;
    U32 tolOpenCnt  = 0;

    pReturnPara->msl  = SUP_NUM_OF_STREAMID;

    for (nsidIdx = 0; nsidIdx < NVME_NUM_OF_NAMESPACES; nsidIdx++)
    {
        tolAllocCnt += gDirectiveMgr.streamInfo[nsidIdx].allocCnt;
    }
    pReturnPara->nssa = SUP_NUM_OF_STREAMID - tolAllocCnt;

    for (nsidIdx = 0; nsidIdx < NVME_NUM_OF_NAMESPACES; nsidIdx++)
    {
        tolOpenCnt += gDirectiveMgr.streamInfo[nsidIdx].openCnt;
    }
    pReturnPara->nsso = tolOpenCnt;

    if (nsid && (nsid <= NVME_NUM_OF_NAMESPACES) && gNvmeIfMgr.ns[nsid - 1].Attach)
    {
        pReturnPara->sws = (STREAM_SWS / HLBU_DATA_SIZE) >> gNvmeIfMgr.ns[nsid - 1].hbu_shift;
        pReturnPara->sgs = STREAM_SGS / STREAM_SWS;
        pReturnPara->nsa = gDirectiveMgr.streamInfo[nsid - 1].allocCnt;
        pReturnPara->nso = gDirectiveMgr.streamInfo[nsid - 1].openCnt;
    }
    else if (nsid == cNvmeGlobalNamespaceId)
    {
        pReturnPara->nsa = tolAllocCnt;
        pReturnPara->nso = tolOpenCnt;
    }

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Stream Receive - Get Status
**/
//-----------------------------------------------------------------------------
Error_t Stream_OP_GetStatus(U16* pReturnSta, uint32_t nsid)
{
    U32 idx;

    if (nsid && (nsid <= NVME_NUM_OF_NAMESPACES) && gNvmeIfMgr.ns[nsid - 1].Attach)
    {
        pReturnSta[0] = gDirectiveMgr.streamInfo[nsid - 1].openCnt;
        for (idx = 0; idx < gDirectiveMgr.streamInfo[nsid - 1].openCnt; idx++)
        {
            pReturnSta[(idx + 1)] = gDirectiveMgr.streamInfo[nsid - 1].idTable[idx];
        }
    }
    else if (nsid == cNvmeGlobalNamespaceId)
    {
        U32 dataIdx;
        U32 tolOpenCnt = 0;

        for (idx = 0; idx < NVME_NUM_OF_NAMESPACES; idx++)
        {
            tolOpenCnt += gDirectiveMgr.streamInfo[idx].openCnt;
        }

        pReturnSta[0] = tolOpenCnt;
        dataIdx       = 1;

        for (idx = 0; idx < NVME_NUM_OF_NAMESPACES; idx++)
        {
            for (idx = 0; idx < gDirectiveMgr.streamInfo[idx].openCnt; idx++)
            {
                pReturnSta[dataIdx] = gDirectiveMgr.streamInfo[idx].idTable[idx];
                dataIdx++;
            }
        }
    }
    else
    {
        gNvmeIfMgr.lastErrorBitOffset  = 0;
        gNvmeIfMgr.lastErrorByteOffset = ERR_DW1_OFFSET; // DW1, NSID
        return MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
    }

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Stream Receive - Get Status
**/
//-----------------------------------------------------------------------------
Error_t Stream_OP_AllocResource(NvmeAdminDirectiveRecieveCommand_t* pCdb, HcmdQ_t* pHcmdQ)
{
    NvmeSdb_t* pSdb;
    U32        nsidIdx;
    U32        tolAllocCnt = 0;

    pSdb = (NvmeSdb_t *)NvmeCmd_GetSdbPointer(pHcmdQ);

    if ((pCdb->DW9_0.NSID > NVME_NUM_OF_NAMESPACES) || (pCdb->DW9_0.NSID == 0) || !gNvmeIfMgr.ns[pCdb->DW9_0.NSID - 1].Attach)
    {
        gNvmeIfMgr.lastErrorBitOffset  = 0;
        gNvmeIfMgr.lastErrorByteOffset = ERR_DW1_OFFSET; // DW1, NSID
        return MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
    }

    if (gDirectiveMgr.streamInfo[pCdb->DW9_0.NSID - 1].allocCnt)
    {
        gNvmeIfMgr.lastErrorBitOffset  = 0;
        gNvmeIfMgr.lastErrorByteOffset = ERR_DW1_OFFSET; // DW1, NSID
        return MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
    }

    for (nsidIdx = 0; nsidIdx < NVME_NUM_OF_NAMESPACES; nsidIdx++)
    {
        tolAllocCnt += gDirectiveMgr.streamInfo[nsidIdx].allocCnt;
    }

    if (tolAllocCnt < SUP_NUM_OF_STREAMID)
    {
        if (pCdb->alRes_NSR > (SUP_NUM_OF_STREAMID - tolAllocCnt))
        {
            gDirectiveMgr.streamInfo[pCdb->DW9_0.NSID - 1].allocCnt = SUP_NUM_OF_STREAMID - tolAllocCnt;
        }
        else
        {
            gDirectiveMgr.streamInfo[pCdb->DW9_0.NSID - 1].allocCnt = pCdb->alRes_NSR;
        }
    }
    else
    {
        gNvmeIfMgr.lastErrorBitOffset  = 0;
        gNvmeIfMgr.lastErrorByteOffset = ERR_DW2_OFFSET; // DW2, NSR
        return MK_NVME_ERROR_CODE(cDirectiveResourceAllocFailed, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
    }

    pSdb->DW0 = gDirectiveMgr.streamInfo[pCdb->DW9_0.NSID - 1].allocCnt;

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Stream Send - Release Identifier
**/
//-----------------------------------------------------------------------------
Error_t Stream_OP_RelIdentifier(NvmeAdminDirectiveSendCommand_t* pCdb)
{
    U32 openIdx, remainCnt;

    if (pCdb->DW9_0.NSID && (pCdb->DW9_0.NSID <= NVME_NUM_OF_NAMESPACES) && gNvmeIfMgr.ns[pCdb->DW9_0.NSID - 1].Attach)
    {
        if (pCdb->DSPEC)
        {
            for (openIdx = 0; openIdx < gDirectiveMgr.streamInfo[pCdb->DW9_0.NSID - 1].openCnt; openIdx++)
            {
                if(gDirectiveMgr.streamInfo[pCdb->DW9_0.NSID - 1].idTable[openIdx] == pCdb->DSPEC)
                {
                    remainCnt = gDirectiveMgr.streamInfo[pCdb->DW9_0.NSID - 1].openCnt - (openIdx + 1);
                    while (remainCnt)
                    {
                        gDirectiveMgr.streamInfo[pCdb->DW9_0.NSID - 1].idTable[openIdx] = gDirectiveMgr.streamInfo[pCdb->DW9_0.NSID - 1].idTable[openIdx + 1];
                        remainCnt--;
                        openIdx++;
                    }
                    gDirectiveMgr.streamInfo[pCdb->DW9_0.NSID - 1].idTable[(gDirectiveMgr.streamInfo[pCdb->DW9_0.NSID - 1].openCnt - 1)] = 0;
                    gDirectiveMgr.streamInfo[pCdb->DW9_0.NSID - 1].openCnt--;
                    return cEcNoError;
                }
            }
        }
    }
    else
    {
        gNvmeIfMgr.lastErrorBitOffset  = 0;
        gNvmeIfMgr.lastErrorByteOffset = ERR_DW1_OFFSET; // DW1, NSID
        return MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
    }
    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Stream Send - Release Resources
**/
//-----------------------------------------------------------------------------
Error_t Stream_OP_RelResource(NvmeAdminDirectiveSendCommand_t* pCdb)
{
    U32 eNsid;
    U32 sNsid = pCdb->DW9_0.NSID;

    if (((sNsid > NVME_NUM_OF_NAMESPACES) && (sNsid != cNvmeGlobalNamespaceId)) || (sNsid == 0))
    {
        gNvmeIfMgr.lastErrorBitOffset  = 0;
        gNvmeIfMgr.lastErrorByteOffset = ERR_DW1_OFFSET; // DW1, NSID
        return MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
    }

    if (sNsid == cNvmeGlobalNamespaceId)
    {
        sNsid = 1;
        eNsid = NVME_NUM_OF_NAMESPACES;
    }
    else
    {
        eNsid = sNsid;
    }

    do {
        MEM_CLR(&gDirectiveMgr.streamInfo[sNsid - 1], sizeof(StreamInfo_t));
    } while (++sNsid <= eNsid);

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Advanced Background Ctrl Receive - Retrun Parameters
**/
//-----------------------------------------------------------------------------
Error_t AdvBgCtl_OP_ReturnParameter(AdvBgReturnPara_t* pReturnPara)
{
    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Advanced Background Ctrl Receive - Retrun Status
**/
//-----------------------------------------------------------------------------
Error_t AdvBgCtl_OP_ReturnStatus(AdvBgReturnSta_t* pReturnPara)
{
    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Advanced Background Ctrl Send - Set Parameter
**/
//-----------------------------------------------------------------------------
Error_t AdvBgCtl_OP_SetParameter(NvmeAdminDirectiveSendCommand_t* pCdb)
{
    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Advanced Background Ctrl Send - Start
**/
//-----------------------------------------------------------------------------
Error_t AdvBgCtl_OP_StartHandle(NvmeAdminDirectiveSendCommand_t* pCdb)
{
    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Advanced Background Ctrl Send - Stop
**/
//-----------------------------------------------------------------------------
Error_t AdvBgCtl_OP_StopHandle(NvmeAdminDirectiveSendCommand_t* pCdb)
{
    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Directive command handler

    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_DirectiveReceive(HcmdQ_t* pHcmdQ)
{
    NvmeAdminDirectiveRecieveCommand_t* pCdb;
    void*    pReturnPara;
    uint32_t transferSize;
    uint8_t  error_Code = cErr_Dire_NoErr;
    uint8_t  noNeedXfer = FALSE;

    pReturnPara = (void *)NvmeCmd_GetScratchBufferPointer();
    MEM_CLR(pReturnPara, cNvmeScratchBufferSize);

    pCdb = (NvmeAdminDirectiveRecieveCommand_t *)NvmeCmd_GetCdbPointer(pHcmdQ);
    transferSize = (pCdb->NUMD+1) * sizeof(uint32_t); // convert to 1 based byte size

    BEGIN_HOST_DBG_DUMP(DBG_DUMP_HOST_NONRW)
        D_PRINTF("%s Type[%02X] Op[%02X]\n", __FUNCTION__, pCdb->DTYPE, pCdb->DOPER);
    END_HOST_DBG_DUMP

    switch (pCdb->DTYPE)
    {
        case DIRE_TYPE_IDENTIFY:
            switch (pCdb->DOPER)
            {
                case IDENTIFY_OP_RETURNPARA:
                    pHcmdQ->error = Identify_OP_ReturnParameter((DirecIdentifyReturnPara_t*)pReturnPara);
                    break;

            // It's not support in the latest Directive TP now
            #if SUP_EVENTNOTI
                case IDENTIFY_OP_EVENTNOTIF:
                    pHcmdQ->error = Identify_OP_EventNotification(pCdb);
                    break;
            #endif

                default:
                    error_Code = cErr_Dire_Op;
                    break;
            }
            break;

        case DIRE_TYPE_STREAMS:
            if (gDirectiveMgr.en_Stream)
            {
                switch (pCdb->DOPER)
                {
                    case STREAM_OP_RETURNPARA:
                        pHcmdQ->error = Stream_OP_ReturnParameter((StreamReturnPara_t*)pReturnPara, pCdb->DW9_0.NSID);
                        break;
                    case STREAM_OP_GETSTA:
                        pHcmdQ->error = Stream_OP_GetStatus((U16*)pReturnPara, pCdb->DW9_0.NSID);
                        break;
                    case STREAM_OP_ALLOCRES:
                        pHcmdQ->error = Stream_OP_AllocResource(pCdb, pHcmdQ);
                        noNeedXfer = TRUE;                                          // No data need to xfer ?
                        break;
                    default:
                        error_Code = cErr_Dire_Op;
                        break;
                }
            }
            else
            {
                error_Code = cErr_Dire_Type;
            }
            break;

        case DIRE_TYPE_ADVBGCTRL:
            if (gDirectiveMgr.en_AdvBgCtrl)
            {
                switch (pCdb->DOPER)
                {
                    case ADVBGCTL_OP_RETURNPARA:
                        pHcmdQ->error = AdvBgCtl_OP_ReturnParameter((AdvBgReturnPara_t*)pReturnPara);
                        break;
                    case ADVBGCTL_OP_RETURNSTA:
                        pHcmdQ->error = AdvBgCtl_OP_ReturnStatus((AdvBgReturnSta_t*)pReturnPara);
                        break;
                    default:
                        error_Code = cErr_Dire_Op;
                        break;
                }
            }
            else
            {
                error_Code = cErr_Dire_Type;
            }
            break;

        default:
            error_Code = cErr_Dire_Type;
            break;
    }

    if (error_Code != cErr_Dire_NoErr)
    {
        if (error_Code == cErr_Dire_Type)
        {
            pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
            gNvmeIfMgr.lastErrorBitOffset = 0;
            gNvmeIfMgr.lastErrorByteOffset = ERR_DW11_OFFSET + 1; // DW11, DTYPE
        }
        else if (error_Code == cErr_Dire_Op)
        {
            pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
            gNvmeIfMgr.lastErrorBitOffset = 0;
            gNvmeIfMgr.lastErrorByteOffset = ERR_DW11_OFFSET; // DW11, DOPER
        }
    }

    if ((pHcmdQ->error == cEcNoError) && (noNeedXfer == FALSE))
    {
        pHcmdQ->bufDesc.ssdIndex = SSD_IF_START;
        AplHost_TransferWholeData_WithPRPList(pHcmdQ, pReturnPara, transferSize, TRUE);
    }
}

//-----------------------------------------------------------------------------
/**
    Directive command handler

    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_DirectiveSend(HcmdQ_t* pHcmdQ)
{
    NvmeAdminDirectiveSendCommand_t* pCdb;
    uint8_t error_Code = cErr_Dire_NoErr;

    pCdb = (NvmeAdminDirectiveSendCommand_t *)NvmeCmd_GetCdbPointer(pHcmdQ);

    BEGIN_HOST_DBG_DUMP(DBG_DUMP_HOST_NONRW)
        D_PRINTF("%s Type[%02X] Op[%02X]\n", __FUNCTION__, pCdb->DTYPE, pCdb->DOPER);
    END_HOST_DBG_DUMP

    switch (pCdb->DTYPE)
    {
        case DIRE_TYPE_IDENTIFY:
            switch (pCdb->DOPER)
            {
                case IDENTIFY_OP_ENABLEDIRE:
                    pHcmdQ->error = Identify_OP_EnableDirective(pCdb);
                    break;

            // It's not support in the latest Directive TP now
            #if SUP_EVENTNOTI
                case IDENTIFY_OP_EVENTCLR:
                    pHcmdQ->error = Identify_OP_EventClear(pCdb);
                    break;
            #endif

                default:
                    error_Code = cErr_Dire_Op;
                    break;
            }
            break;

        case DIRE_TYPE_STREAMS:
            if (gDirectiveMgr.en_Stream)
            {
                switch (pCdb->DOPER)
                {
                    case STREAM_OP_RELIDENT:
                        pHcmdQ->error = Stream_OP_RelIdentifier(pCdb);
                        break;
                    case STREAM_OP_RELRES:
                        pHcmdQ->error = Stream_OP_RelResource(pCdb);
                        break;
                    default:
                        error_Code = cErr_Dire_Op;
                        break;
                }
            }
            else
            {
                error_Code = cErr_Dire_Type;
            }
            break;

        case DIRE_TYPE_ADVBGCTRL:
            if (gDirectiveMgr.en_AdvBgCtrl)
            {
                switch (pCdb->DOPER)
                {
                    case ADVBGCTL_OP_SETPARA:
                        pHcmdQ->error = AdvBgCtl_OP_SetParameter(pCdb);
                        break;
                    case ADVBGCTL_OP_START:
                        pHcmdQ->error = AdvBgCtl_OP_StartHandle(pCdb);
                        break;
                    case ADVBGCTL_OP_STOP:
                        pHcmdQ->error = AdvBgCtl_OP_StopHandle(pCdb);
                        break;
                    default:
                        error_Code = cErr_Dire_Op;
                        break;
                }
            }
            else
            {
                error_Code = cErr_Dire_Type;
            }
            break;

        default:
            error_Code = cErr_Dire_Type;
            break;
    }

    if (error_Code != cErr_Dire_NoErr)
    {
        if (error_Code == cErr_Dire_Type)
        {
            pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
            gNvmeIfMgr.lastErrorBitOffset = 0;
            gNvmeIfMgr.lastErrorByteOffset = ERR_DW11_OFFSET + 1; // DW11, DTYPE
        }
        else if (error_Code == cErr_Dire_Op)
        {
            pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
            gNvmeIfMgr.lastErrorBitOffset = 0;
            gNvmeIfMgr.lastErrorByteOffset = ERR_DW11_OFFSET; // DW11, DOPER
        }
    }
}
