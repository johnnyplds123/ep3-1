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

// APL
#include "HldNvme.h"
#include "SysInfo.h"
#include "NvmeGetLogPage.h"

// Code Section
#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define GET_NVME_ASYNC_EVENT_INFO_FROM_ERROR(X) ((X)&0xFF)

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief NVMe Admin Command structure - Asynchronous Event Request
typedef  struct
{
    AdminCommandCommon_t DW9_0;    ///< DW9..0

    uint32_t reserved10[6];   ///< DW15..10

} NvmeAdminAsyncEventRequestCommand_t;
COMPILE_ASSERT(sizeof(NvmeAdminAsyncEventRequestCommand_t)==64, "NVMe command size shall be 64");

/// @brief Asynchronous Event Request Completion Queue Entry Dword 0
typedef struct
{
    uint32_t AsynchronousEventType:3;    ///< Asynchronous Event Type [02:00]
    uint32_t reserved3:5;                ///< Reserved [07:03]
    uint32_t AsynEventInformation:8;     ///< information regarding the asynchronous event [15:08]
    uint32_t AssociatedLogPage:8;        ///< Indicates the log page associated with the asynchronous event [23:16]
    uint32_t reserved24:8;               ///< Reserved [31:24]

} AsynchronousEventReqCompletionQDword0_t;
COMPILE_ASSERT(sizeof(AsynchronousEventReqCompletionQDword0_t) == 4, "NVMe DW0 size shall be 4");

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static void NvmeCmd_AsynchronousEventNotification(HcmdQ_t* pHcmdQ);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------

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
    Asynchronous Event Request - Admin 0Ch

    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_AsynchronousEventRequest(HcmdQ_t* pHcmdQ)
{
    BEGIN_HOST_DBG_DUMP(DBG_DUMP_HOST_NONRW)
        D_PRINTF("%s\n", __FUNCTION__);
    END_HOST_DBG_DUMP

    // push to asynchronous event command list
    if (gHostIfMgr.stateList[cHcsAsyncWait].count<NVME_ASYNC_EVENT_LIMIT)
    {
        if (gNvmeIfMgr.queuedError == cEcNoError)
        {
            Host_PushToTail(cHcsAsyncWait, pHcmdQ);
            SetHcmdHandler(pHcmdQ, NvmeCmd_AsynchronousEventNotification);
        }
        else // send error status notification immediately if error is queued.
        {
            pHcmdQ->error = gNvmeIfMgr.queuedError;
            gNvmeIfMgr.queuedError = cEcNoError;
            NvmeCmd_AsynchronousEventNotification(pHcmdQ);
        }
    }
    else
    {
        pHcmdQ->error = MK_NVME_ERROR_CODE(cAsynchronousEventRequestLimitExceeded, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
    }
}

//-----------------------------------------------------------------------------
/**
    Asynchronous Event notification

    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
static void NvmeCmd_AsynchronousEventNotification(HcmdQ_t* pHcmdQ)
{
    NvmeSdb_t* pSdb;
    AsynchronousEventReqCompletionQDword0_t* pDW0;

    pSdb = NvmeCmd_GetSdbPointer(pHcmdQ);
    pDW0 = (AsynchronousEventReqCompletionQDword0_t *)&pSdb->DW0;

    if (pHcmdQ->asycType == cErrorStatus)
    {
        pSdb->DW0 = 0;
        pDW0->AsynEventInformation  = GET_NVME_ASYNC_EVENT_INFO_FROM_ERROR(pHcmdQ->error);
        pDW0->AsynchronousEventType = pHcmdQ->asycType;
        pDW0->AssociatedLogPage     = cLidErrorInformation;
    }
    else
    {
        pSdb->DW0 = 0;
        pDW0->AsynEventInformation  = pHcmdQ->asycInfo;
        pDW0->AsynchronousEventType = pHcmdQ->asycType;

        if (pHcmdQ->asycType == cSmartHealthStatus)
        {
            pDW0->AssociatedLogPage = cLidSmartHealthInformation;
        }
        else if (pHcmdQ->asycType == cNotice)
        {
            if (pHcmdQ->asycInfo == cFirmwareActivationStarting)
            {
                 pDW0->AssociatedLogPage = cLidFirmwareSlotInformation;
            }
        }
    }

    // test if event notification is masked due to pending GetLog command
    if (GET_BIT(gNvmeIfMgr.asyncEventMask, pDW0->AssociatedLogPage) != 0)
    {
        Host_PushToTail(cHcsAsyncWait, pHcmdQ);
    }
    else
    {
        D_PRINTF("[NVMe] Async Event Notification\n");
        pHcmdQ->error = cEcNoError; // no error compeletion
        BIT_SET(gNvmeIfMgr.asyncEventMask, BIT(pDW0->AssociatedLogPage));
        NvmeCmd_Complete(pHcmdQ);
    }
}

//-----------------------------------------------------------------------------
/**
    Asynchronous Event Check

    @param[out]  U16   Asynchronous Event Type
**/
//-----------------------------------------------------------------------------
U16 NvmeCmd_AsynchronousEventCheck(void)
{
    //------------------------------------------------------------
    // Firmware Activataion (report it if device is updating firmware)
    //------------------------------------------------------------
    if (gNvmeIfMgr.AsyncEventEnable & cAsFirmwareActivationNotices)
    {
        if (gBootNewFwFlag & (BOOT_FW_NOW | BOOT_FW_AFT_RBT))
        {
            return NvmeCmd_ASY_RETURN_INFO_TYPE(cFirmwareActivationStarting, cNotice);
        }
    }

    //------------------------------------------------------------
    // Device Temperature (report it if over threshold)
    //------------------------------------------------------------
    if (gNvmeIfMgr.AsyncEventEnable & cCriticalWarnTemperature)
    {
        U16 temp;
        U8  i;

        temp = smcThermalSource[cThermalSourceI2c];
        if (HandleTemperatureC2K(&temp) == cEcNoError)
        {
            for (i = 0; i < 2; i++)
            {
                if (((temp > gNvmeIfMgr.currTMPTH[i][0]) && (gNvmeIfMgr.currTMPTH[i][0])) ||
                    ((temp < gNvmeIfMgr.currTMPTH[i][1]) && (gNvmeIfMgr.currTMPTH[i][1])))
                {
                    return NvmeCmd_ASY_RETURN_INFO_TYPE(cTemperatureAboveThreshold, cSmartHealthStatus);
                }
            }
        }
    }

    //------------------------------------------------------------
    // Available spared space (report it if below threshold)
    //------------------------------------------------------------
    if (gNvmeIfMgr.AsyncEventEnable & cCriticalWarnSpare)
    {
        #if (AvaiSpareThreshold != 0)
            if (smAvailableSpare != U32_MAX)
            {
                if (smAvailableSpare < AvaiSpareThreshold)
                {
                    return NvmeCmd_ASY_RETURN_INFO_TYPE(cSpareBelowThreshold, cSmartHealthStatus);
                }
            }
            else
            {
                if (smSysInfo->d.LogInfo.d.SmrLog.availableSpare < AvaiSpareThreshold)
                {
                    return NvmeCmd_ASY_RETURN_INFO_TYPE(cSpareBelowThreshold, cSmartHealthStatus);
                }
            }
        #endif
    }

    return FALSE;
}

//-----------------------------------------------------------------------------
/**
    Asynchronous Event Backup
**/
//-----------------------------------------------------------------------------
void Asynchronous_EventBackup(void)
{
    MEM_CLR(&gAsyEventBackup, sizeof(gAsyEventBackup));

    if (gHostIfMgr.stateList[cHcsAsyncWait].count != 0)
    {
        HcmdQ_t* pHcmdQ = NULL;
        U32 cmdindex    = 0;

        gAsyEventBackup.asyBackupCnt = gHostIfMgr.stateList[cHcsAsyncWait].count;

        while (gHostIfMgr.stateList[cHcsAsyncWait].count != 0)
        {
            pHcmdQ = Host_PopFromHead(cHcsAsyncWait);
            if (pHcmdQ == NULL)
            {
                break;
            }
            else
            {
                gAsyEventBackup.asyHcmdQ[cmdindex] = *pHcmdQ;
                cmdindex++;
            }
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Asynchronous Event restore
**/
//-----------------------------------------------------------------------------
void Asynchronous_EventRestore(void)
{
    if (gAsyEventBackup.asyBackupCnt)
    {
        HcmdQ_t* pHcmdQ = NULL;
        uint32_t idx    = 0;

        while (gAsyEventBackup.asyBackupCnt != 0)
        {
            pHcmdQ  = GetHcmdQ(gAsyEventBackup.asyHcmdQ[idx].cmdId);
            *pHcmdQ = gAsyEventBackup.asyHcmdQ[idx];
            Host_PushToTail(cHcsAsyncWait, pHcmdQ);

            idx++;
            gAsyEventBackup.asyBackupCnt--;
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Asynchronous Event Peek

    @param[out]  U16 Check AsyEvent and Push to Report pool
**/
//-----------------------------------------------------------------------------
U16 NvmeCmd_AsynchronousEventPeek(void)
{
    HcmdQ_t* pHcmdQ = NULL;
    U16      asyncEvent;

    asyncEvent = NvmeCmd_AsynchronousEventCheck();

    if (asyncEvent)
    {
        pHcmdQ = Host_PopFromHead(cHcsAsyncWait);
        if (pHcmdQ != NULL)
        {
            pHcmdQ->asycInfo = NvmeCmd_ASY_GET_INFO(asyncEvent);
            pHcmdQ->asycType = NvmeCmd_ASY_GET_TYPE(asyncEvent);
            Host_PushToTail(cHcsAsyncReport, pHcmdQ); // move to ready list to notify asynchronous Event
        }
        else
        {
            return FALSE;
        }
    }

    return asyncEvent;
}
/* End of File */
