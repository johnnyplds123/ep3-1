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
//! @brief Generic Host I/F High level driver API codes
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
// PKG
#include "BuildPkg.h"
#include "SharePkg.h"
#include "HostPkg.h"

// REG
#include "RegPcie.h"
#include "RegNvmePkg.h"

// HAL
#include "HalPcieLtr.h"
#include "HalNvmeXfer.h"
#include "HalRegConfig.h"
#include "HalPm.h"

// APL
#include "NvmeCmds.h"
#include "HldNvme.h"
#include "NvmeStatusCodes.h"

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

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
static volatile uint64_t controllerBusyTimeUsec[2]; // 0=Admin, 1=IO

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------
extern NvmeHwIoCmdDw0_t         ghwIocmdDw0_ids[NVME_NUM_OF_NAMESPACES];
extern NvmeHwIoCmdDw1_t         ghwIocmdDw1_ids[NVME_NUM_OF_NAMESPACES];
extern NvmeHwIoCmdDw0_t         ghwIocmdDw0_ssd[NVME_NUM_OF_NAMESPACES];
extern NvmeHwIoCmdDw1_t         ghwIocmdDw1_ssd[NVME_NUM_OF_NAMESPACES];
extern NvmeCdb_t                sNvmeCdb;                               ///< NVMe CDB(command data block) for last command
extern NvmeSdb_t                sNvmeSdb[MAX_HCMD_Q_COUNT];             ///< NVMe SDB(status data block) per command

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    Create I/O completion queue

    @param[in]  prp                 Physical Region Page
    @param[in]  cqId                completion queue Id
    @param[in]  qSize0              queue size (number of entries), zero based value
    @param[in]  contiguous          physically contiguous memory
    @param[in]  interruptEnable     interrupt enable
    @param[in]  interruptVector     interrupr vector

    @notes  assume that a fid is always 0. it needs to support function id later.

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HldNvme_CreateIoCompletionQueue(uint64_t prp, uint32_t cqId, uint32_t qSize0, uint32_t contiguous, uint32_t interruptEnable, uint32_t interruptVector)
{
    if (cqId == cAdminQueueId || cqId>HW_MAX_IO_QUEUES || IS_CQ_CREATED(cqId))
    {
        return MK_NVME_ERROR_CODE(cInvalidQueueIdentifier, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
    }

    if (qSize0 > rNvmeController.controller[0].CAP.b.MQES || !qSize0) // qSize0 is zero based value
    {
        return MK_NVME_ERROR_CODE(cMaximumQueueSizeExceeded, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
    }

    if (interruptVector >= HW_MAX_INT_VECTORS) // use constant instead of reading PCIe register (this cause hung without PCIe clock, PCIe ASPM L1.x CLK_REQ)
    {
        return MK_NVME_ERROR_CODE(cInvalidInterruptVector, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
    }

    // TODO: Support function id later
    HalNvme_CreateIoCq(prp, cqId, qSize0, contiguous, interruptEnable, interruptVector);

    CREATE_CQ(cqId);

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Create I/O submission queue

    @param[in]  prp                 Physical Region Page
    @param[in]  sqId                submission queue Id
    @param[in]  qSize0              queue size (number of entries), zero based value
    @param[in]  contiguous          physically contiguous memory
    @param[in]  priority            priority
    @param[in]  cqId                completion queue Id

    @notes  assume that a fid is always 0. it needs to support function id later.

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HldNvme_CreateIoSubmissionQueue(uint64_t prp, uint32_t sqId, uint32_t qSize0, uint32_t contiguous, uint32_t priority, uint32_t cqId)
{
    if (sqId == cAdminQueueId || sqId>HW_MAX_IO_QUEUES || IS_SQ_CREATED(sqId)) // error if CQ id is invalid or already allocated.
    {
        return MK_NVME_ERROR_CODE(cInvalidQueueIdentifier, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
    }

    if (cqId == cAdminQueueId || cqId>HW_MAX_IO_QUEUES || !IS_CQ_CREATED(cqId)) // error if assoicated CQ was not allocated
    {
        return MK_NVME_ERROR_CODE(cCompletionQueueInvalid, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
    }

    if (qSize0 > rNvmeController.controller[0].CAP.b.MQES || !qSize0) // qSize0 is zero based value
    {
        return MK_NVME_ERROR_CODE(cMaximumQueueSizeExceeded, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
    }

    // TODO: Support function id later
    HalNvme_CreateIoSq(prp, sqId, qSize0, contiguous, priority, cqId);

    CREATE_SQ(sqId);

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Delete I/O completion queue

    @param[in]  cqId          completion queue Indentifier to be deleted

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HldNvme_DeleteIoCompletionQueue(NvmeQueueId_t cqId)
{
    uint32_t sqId;


    // Traverse all submission queues to check whether all associated queues of a given cqId are deleted.
    for(sqId = 1; sqId <= HW_MAX_IO_QUEUES; sqId++)  // sqId 0 is for admin queue.
    {   // check associated submission queue is deleted
        if((rNvmeController.queueStatus[sqId].sq.b.cqId == cqId) && IS_SQ_CREATED(sqId))
        {
            return MK_NVME_ERROR_CODE(cInvalidQueueDeletion, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
        }
    }

    if (cqId==cAdminQueueId || cqId>HW_MAX_IO_QUEUES || !IS_CQ_CREATED(cqId))
    {
        return MK_NVME_ERROR_CODE(cInvalidQueueIdentifier, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
    }

    DELETE_CQ(cqId);

    HalNvme_DeleteIoQueue(TRUE, cqId);

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Delete I/O submission queue

    @param[in]  sqId          submission queue Indentifier to be deleted

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HldNvme_DeleteIoSubmissionQueue(NvmeQueueId_t sqId)
{
    if (sqId == cAdminQueueId || sqId>HW_MAX_IO_QUEUES || !IS_SQ_CREATED(sqId))
    {
        return MK_NVME_ERROR_CODE(cInvalidQueueIdentifier, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
    }

    DELETE_SQ(sqId);

    // The command causes all commands submitted to the indicated SQ that are still in progress to be aborted.
    // The controller may posst individual completion status of Command Aborted due to SQ deletion.
    // Commands that are not able to be aborted should be completed with appropriate completion status.
    // TODO: It requires first to delete all the commands that are submitted to the SQ.
    HalNvme_DeleteIoQueue(FALSE, sqId);

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Abort command

    @param[in]  fId           function Indentifier to be aborted.
    @param[in]  cmdId         command Indentifier to be aborted.
    @param[in]  sqId          submission queue Indentifier to be aborted

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HldNvme_AbortCommand(NvmeFunctionId_t fId, NvmeCmdId_t cmdId, NvmeQueueId_t sqId)
{
    MsgHostIO_t*    pHcmdMsg;
	NvmeSdb_t*      pSdb;
    HcmdQ_t*        pHcmdQ;
    HcmdStateId_t   state;
    uint16_t        index;
	uint8_t         poolTempCnt;
	uint8_t         abortFlag = FALSE;


    if ((sqId > HW_MAX_IO_QUEUES) || (!IS_SQ_CREATED(sqId)))
    {
        return(cEcError);
    }

    if (sqId == 0)
    {   //Admin command
        for (state = cHcsAsyncWait; state <= cHcsAsyncReport; state ++)
        {
            poolTempCnt = gHostIfMgr.stateList[state].count;
            while (poolTempCnt != 0)
            {
                pHcmdQ = Host_PopFromHead(state);

                if (pHcmdQ == NULL)
                {
                    break;
                }

                pSdb = (NvmeSdb_t*)NvmeCmd_GetSdbPointer(pHcmdQ);

                if (cmdId == pSdb->CID)
                {
                    pHcmdQ->error = MK_NVME_ERROR_CODE(cCommandAbortRequested, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
                    NvmeCmd_Complete(pHcmdQ);
                    return(cEcNoError);
                }
                else
                {
                    Host_PushToTail(state, pHcmdQ);
                }
                poolTempCnt--;
            }
        }
        #if 0   // Ignore Admin CMD abort
        for(index = 0; index < HW_MAX_ADMIN_CMD_INDEX; index++)
        {
            if(HalNvme_IsAdminCommandAlive(index, fId, sqId, cmdId))
            {
                rNvmeHwAdmCmd[index].AB = 1;    //AB bit set
                return(cEcNoError);
            }
        }
        #endif
    }
    else
    {   //I/O command
        for (index = 0; index < HW_MAX_IO_CMD_INDEX; index++)
        {
            if ( (GET_IO_FID_FROM_INDEX(index)  == fId)  &&
                 (GET_IO_SQID_FROM_INDEX(index) == sqId) &&
                 (GET_IO_CID_FROM_INDEX(index)  == cmdId) )
            {
                //rNvmeHwIoCmd[index].dw0.b.AB = 1;   // AB bit set
                pHcmdQ = &gHcmdQ[index];
                pHcmdMsg = (MsgHostIO_t *)GetHcmdMsgPtr(pHcmdQ);

                if ((pHcmdMsg->hdr.b.status <= cMsgExecuted) && (pHcmdMsg->hdr.b.status >= cMsgPosted))
                {
                    abortFlag = TRUE;
                    if ((pHcmdMsg->hdr.b.opCode == cMcRead) &&
                        ((pHcmdMsg->bufDesc.css == CACHE_R_IDS) || (pHcmdMsg->hdr.b.status != cMsgExecuted)))
                    {
                        abortFlag = FALSE;
                    }

                    if (abortFlag)
                    {
                        gHostIfMgr.info.currAbortCnt++;
                        pHcmdMsg->hdr.b.forceAbort = TRUE;
                        return(cEcNoError);
                    }
                }
            }
        }
    }

    return(cEcError);
}

//-----------------------------------------------------------------------------
/**
    Set NVMe Host LBA format

    @param[in] index    HLBA format index
    @param[in] code     format control code (see NvmeHlbaFormatControl_t)
    @param[in] param    parameter
    @param[in] pParam   parameter pointer (additional or output)

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HldNvme_SetHlbaFormat(uint32_t index, NvmeHlbaFormatControl_t code, uint32_t param, uint32_t *pParam)
{
    LbaFmt_t lbaFmt;

    if (HW_MAX_LBA_FORMATS<=index)
    {
        return(MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry));
    }

    lbaFmt.all = HalNvme_GetLbaFormat(index);

    switch (code)
    {
        case cNvmeFormatDataSize:
            lbaFmt.b.DATA_SIZE = LOG2(param) - 9;
            lbaFmt.b.AU_RATIO  = LOG2(AU_SIZE/param);
            break;
        case cNvmeFormatMetaSize:
            lbaFmt.b.META_SIZE = param;
            break;
        case cNvmeFormatHlbaSize:
            lbaFmt.b.HLBA_SIZE = param;
            break;
        case cNvmeFormatHlba:
            lbaFmt.b.HLBA_EN = param;
            break;
        case cNvmeFormatMpecc:
            lbaFmt.b.MPECC_EN = param;
            break;
        case cNvmeFormatBcrc:
            lbaFmt.b.BCRC_EN = param;
            break;
        case cNvmeFormatAes:
            lbaFmt.b.AES_EN = param;
            break;
        default:
            return(MK_NVME_ERROR_CODE(cInvalidCommandOpcode, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry));
    }

    HalNvme_SetLbaFormat(lbaFmt.all, index);

    return(cEcError);
}

//-----------------------------------------------------------------------------
/**
    Set NVMe Namespace format (all parameters are in NVMe spec. as-is)

    @param[in] nsId     namespace id
    @param[in] lbaf     LBA Format
    @param[in] pil      Protection Information Location
    @param[in] pi       Protection Information
    @param[in] ms       Meta data Settings

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HldNvme_SetNamespace(uint32_t nsid0)
{
    if (nsid0 >= NVME_NUM_OF_NAMESPACES)
    {
        return (MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry));
    }
    else
    {
        uint8_t lbaf = gNvmeIfMgr.ns[nsid0].LBAF;

        ghwIocmdDw0_ids[nsid0].b.LBAF = gNvmeIfMgr.ns[nsid0].LBAF;
        ghwIocmdDw0_ids[nsid0].b.MS   = gNvmeIfMgr.ns[nsid0].MS;
        ghwIocmdDw0_ids[nsid0].b.PI   = gNvmeIfMgr.ns[nsid0].PI;
        ghwIocmdDw0_ids[nsid0].b.PIL  = gNvmeIfMgr.ns[nsid0].PIL;

        ghwIocmdDw0_ssd[nsid0].b.LBAF = gNvmeIfMgr.ns[nsid0].LBAF;
        ghwIocmdDw0_ssd[nsid0].b.MS   = gNvmeIfMgr.ns[nsid0].MS;
        ghwIocmdDw0_ssd[nsid0].b.PI   = gNvmeIfMgr.ns[nsid0].PI;
        ghwIocmdDw0_ssd[nsid0].b.PIL  = gNvmeIfMgr.ns[nsid0].PIL;

        gNvmeIfMgr.ns[nsid0].NSZE = (gNvmeIfMgr.ns[nsid0].NSZE << gNvmeIfMgr.ns[nsid0].hbu_shift) >> cLBAFTable[lbaf].hbu_shift;

        gNvmeIfMgr.ns[nsid0].hbu_shift = cLBAFTable[lbaf].hbu_shift;

        if (nsid0)   gNvmeIfMgr.ns[nsid0].mbu_start = gNvmeIfMgr.ns[nsid0 - 1].mbu_end;
        else         gNvmeIfMgr.ns[nsid0].mbu_start = 0;

        gNvmeIfMgr.ns[nsid0].mbu_end = gNvmeIfMgr.ns[nsid0].mbu_start +
                                     ((gNvmeIfMgr.ns[nsid0].NSZE + cLBAFTable[lbaf].mbu_mask) >> cLBAFTable[lbaf].mbu_shift);

    }

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Return associated NVMe id

    @param[in]  hldCmdId    command id is allocated by HLD
    @param[out] pNvmeSqId   NVMe submission queue Id
    @param[out] pNvmeCmdId  NVMe command Id

    @return error code if not exist
**/
//-----------------------------------------------------------------------------
Error_t HldNvme_GetId(uint32_t hldCmdId, NvmeQueueId_t* pNvmeSqId, NvmeCmdId_t* pNvmeCmdId)
{
    uint16_t cmdIndex;

    cmdIndex = GET_COMMAND_INDEX(hldCmdId);

    if (IS_ADMIN_COMMAND_ID(hldCmdId))
    {   // Admin Queue
        if (cmdIndex >= HW_MAX_ADMIN_CMD_INDEX)
        {
            return(cEcError);
        }

        *pNvmeSqId  = GET_ADM_SQID_FROM_INDEX(cmdIndex);
        *pNvmeCmdId = GET_ADM_CID_FROM_INDEX(cmdIndex);
    }
    else
    {   // I/O Queue
        if (cmdIndex >= HW_MAX_IO_CMD_INDEX)
        {
            return(cEcError);
        }

        *pNvmeSqId  = GET_IO_SQID_FROM_INDEX(cmdIndex);
        *pNvmeCmdId = GET_IO_CID_FROM_INDEX(cmdIndex);
    }

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Monitor controller busy and count time
**/
//-----------------------------------------------------------------------------
void HldNvme_CountControllerBusyTime(void)
{
    uint32_t adminCmdActive;
    uint32_t ioCmdActive;

    if (!NvmeHba.ready) return;
    adminCmdActive = rNvmeDataMngr.dataManagerCtrl0.b.DATM_ACMD_CABM;

    ioCmdActive = rNvmeBitmapSram.bitmapSramCmdIndexAvailablBitmapCntr.b.CIABM_CNTR != HW_MAX_IO_CMD_INDEX;

    if (adminCmdActive!=0)
    {
        controllerBusyTimeUsec[cNvmeAdminCmd]++;
    }
    if (ioCmdActive!=0)
    {
        controllerBusyTimeUsec[cNvmeIoCmd]++;
    }

#if (HOST_IF_POWER_MANAGEMENT == FEATURE_SUPPORTED)   // enable PCIe PM
    if (adminCmdActive==0 && ioCmdActive==0)
    {
        HalPcieLtr_IdleStatusPolling(true);
    }
    else
    {
        HalPcieLtr_IdleStatusPolling(false);
    }
#endif
    // TODO: Add code here
}

//-----------------------------------------------------------------------------
/**
    Return controller busy time in usec unit (for NVMe SMART/Health information)

    @param[in]  cmdType     command type

    @return controller busy time in usec unit
**/
//-----------------------------------------------------------------------------
uint64_t HldNvme_GetControllerBusyTime(NvmeCmdType_t cmdType)
{
    return controllerBusyTimeUsec[cmdType];
}

//-----------------------------------------------------------------------------
/**
    Get NVMe CDB pointer

    @return CDB pointer
**/
//-----------------------------------------------------------------------------
NvmeCdb_t* NvmeCmd_GetCdbPointer(HcmdQ_t* pHcmdQ)
{
    return (&sNvmeCdb);
}

//-----------------------------------------------------------------------------
/**
    Get NVMe SDB pointer

    @return SDB pointer
**/
//-----------------------------------------------------------------------------
NvmeSdb_t* NvmeCmd_GetSdbPointer(HcmdQ_t* pHcmdQ)
{
    return &sNvmeSdb[pHcmdQ->index];
}
