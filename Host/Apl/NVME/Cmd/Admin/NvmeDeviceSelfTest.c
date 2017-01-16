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
//! @brief NVMe Format command handler
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
#include "HalTimer.h"

// APL
#include "HldNvme.h"
#include "NvmeDeviceSelfTest.h"
#include "SysInfo.h"
#include "NvmeGetLogPage.h"

// Code Section
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
/// @brief NVMe Admin Command structure - NVM Command Specific - Self Test
typedef  struct
{
    AdminCommandCommon_t DW9_0;    ///< DW9..0

    uint32_t STC:4;           ///< Self-test Code (DW10 bits[03:00])
    uint32_t reserved10:28;   ///< reserved (DW10 bits[31:4])

    uint32_t reserved11[5]; ///< DW15..11

} NvmeAdminSelfTestCommand_t;
COMPILE_ASSERT(sizeof(NvmeAdminSelfTestCommand_t) == 64, "NVMe command size shall be 64");

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
void NvmeDeviceSelfTestInit(InitBootMode_t initMode);
void DST_Operation(uint8_t STC);
void DST_CmdAbort(HcmdQ_t* pHcmdQ, uint8_t STC);
void DST_Completion(uint8_t Result);
void HandleSaveDSTLog(void);
void DSTOperationImmediate(void);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
uint8_t     gCurrDSTOperation;
uint8_t     gCurrDSTCompletion;
uint32_t    gCurrDSTTime;
uint8_t     gCurrDSTOperationImmed;

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
    Device Self-test initial

    @param[in]  initMode   Initial mode
**/
//-----------------------------------------------------------------------------
void NvmeDeviceSelfTestInit(InitBootMode_t initMode)
{
    gCurrDSTOperation = 0;
    gCurrDSTCompletion = 0;
    gCurrDSTTime = 0;
    gCurrDSTOperationImmed = 0;

    if (smSysInfo->d.LogInfo.d.DSTLog.Tag != 0x44535400)  //DST0
    {
        uint8_t idx;

        MEM_CLR((U8*)&smSysInfo->d.LogInfo.d.DSTLog, sizeof(tDST_LOG));
        D_PRINTF("DST init\n");

        for (idx = 0; idx < 20; idx++)
        {
           smSysInfo->d.LogInfo.d.DSTLog.DSTLogEntry[idx].DSTResult = cDSTEntryNotUsed;
        }
        smSysInfo->d.LogInfo.d.DSTLog.Tag = 0x44535400;  //DST0
    }

    if (smSysInfo->d.LogInfo.d.DSTLog.DSTResult.DSTCode)
    {
        if (smSysInfo->d.LogInfo.d.DSTLog.DSTResult.DSTCode == cDST_SHORT)
        {
            smSysInfo->d.LogInfo.d.DSTLog.DSTResult.DSTResult = cDSTAbort;
            HandleSaveDSTLog();
        }
        else
        {
            MsgSmart_t*  pMsg;

            DST_Operation(cDST_EXTENDED);

            pMsg = (MsgSmart_t*) Ipc_GetExtendMsg(cMcSmartOp);
            pMsg->subOp = cSmrSelfTestOp;
            pMsg->error         = cEcNoError;
            pMsg->type          = cDST_EXTENDED;

            IPC_SendMsgQ(cH2cReqQueue, (Msg_t*)pMsg);
            while (pMsg->hdr.b.status != cMsgCompleted);
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Device Self-test - Admin 14h

    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_DeviceSelfTest(HcmdQ_t* pHcmdQ)
{
    NvmeAdminSelfTestCommand_t* pCdb;
    MsgSmart_t* pMsg;

    pCdb = (NvmeAdminSelfTestCommand_t *)NvmeCmd_GetCdbPointer(pHcmdQ);

    BEGIN_HOST_DBG_DUMP(DBG_DUMP_HOST_NONRW)
        D_PRINTF("%s  STC=%1X\n ", __FUNCTION__,pCdb->STC);
    END_HOST_DBG_DUMP

    switch (pCdb->STC)  // Self-test Code
    {
        case cDST_SHORT:
        case cDST_EXTENDED:
            if (!gCurrDSTOperation)
            {
                pHcmdQ->attr.all |= ATTR_OVERLAPPED;
                DST_Operation(pCdb->STC);

                pMsg = (MsgSmart_t*) GetHcmdMsgPtr(pHcmdQ);
                MSG_SET_HOST_INIT(pMsg, cMcSmartOp);
                pMsg->subOp = cSmrSelfTestOp;
                pMsg->error = cEcNoError;
                pMsg->type  = pCdb->STC;

                Host_MediaReq();
                IPC_SendMsgQ(cH2cReqQueue, (Msg_t*)pMsg);
                SyIn_Synchronize(SI_AREA_SMART, SYSINFO_WRITE, SI_SYNC_BY_NVMELOG);
            }
            else
            {
                pHcmdQ->error = MK_NVME_ERROR_CODE(cDeviceSelftestInProgress, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
                gNvmeIfMgr.lastErrorBitOffset  = 0;
                gNvmeIfMgr.lastErrorByteOffset = ERR_DW10_OFFSET;
            }
            break;

        case cDST_ABORT:
            DST_CmdAbort(pHcmdQ, pCdb->STC);
            break;

        default:
            pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFormat, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
            gNvmeIfMgr.lastErrorBitOffset  = 0;
            gNvmeIfMgr.lastErrorByteOffset = ERR_DW10_OFFSET; // DW10, SES
            break;
    }
}

//-----------------------------------------------------------------------------
/**
    DST OPERATION - 1h 2h

    @param[in]
**/
//-----------------------------------------------------------------------------
void DST_Operation(uint8_t STC)
{
    gCurrDSTOperation = STC;
    smSysInfo->d.LogInfo.d.DSTLog.DSTResult.DSTCode = STC;
    smSysInfo->d.LogInfo.d.DSTLog.DSTResult.DSTResult = 0;
    smSysInfo->d.LogInfo.d.DSTLog.DSTResult.Segment = 0;
    gCurrDSTOperationImmed = 1;
    gCurrDSTCompletion = 1;
    gCurrDSTTime = HalTimer_ReadTick();
}

//-----------------------------------------------------------------------------
/**
    DST Cmd Abort

    @param[in]
**/
//-----------------------------------------------------------------------------
void DST_CmdAbort(HcmdQ_t* pHcmdQ, uint8_t STC)
{
    if (gCurrDSTOperation)
    {
        DST_Completion(cDSTAbort);
        pHcmdQ->error = MK_NVME_ERROR_CODE(cDeviceSelftestInProgress, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
        gNvmeIfMgr.lastErrorBitOffset  = 0;
        gNvmeIfMgr.lastErrorByteOffset = ERR_DW10_OFFSET;
    }
}

//-----------------------------------------------------------------------------
/**
    DST COMPLETION

    @param[in]
**/
//-----------------------------------------------------------------------------
void DST_Completion(uint8_t Result)
{
    if (gCurrDSTOperation)
    {
        if ((smSysInfo->d.LogInfo.d.DSTLog.DSTResult.DSTCode == cDST_SHORT) || (Result != cDSTAbortReset))
        {
            MsgSmart_t* pMsg;
            uint32_t TimStar;

            smSysInfo->d.LogInfo.d.DSTLog.DSTResult.DSTResult = Result;
            gCurrDSTOperationImmed = 0;

            D_PRINTF("DSTCompletion   Result=%1X\n",Result);

            pMsg = (MsgSmart_t*) Ipc_GetExtendMsg(cMcSmartOp);
            pMsg->subOp = cSmrSelfTestIni;

            IPC_SendMsgQ(cH2cReqQueue,  (Msg_t*)pMsg);
            TimStar = HalTimer_ReadTick();
            while (pMsg->hdr.b.status != cMsgCompleted && (HalTimer_GetElapsedTimeMsec(TimStar) < 10000));

            HandleSaveDSTLog();
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Handle Save DST Log

    @param[in]
**/
//-----------------------------------------------------------------------------
void HandleSaveDSTLog(void)
{
    uint8_t  DSTIdx;
    uint8_t  Valid = 0;

    smSysInfo->d.LogInfo.d.DSTLog.TotalDSTCnt++;

    for (DSTIdx = (20 - 1); DSTIdx > 0; DSTIdx--)
    {
        MEM_COPY((U8*)&smSysInfo->d.LogInfo.d.DSTLog.DSTLogEntry[DSTIdx], (U8*)&smSysInfo->d.LogInfo.d.DSTLog.DSTLogEntry[(DSTIdx - 1)], sizeof(tDST_LOG_ENTRY));
    }

    if (smSysInfo->d.LogInfo.d.DSTLog.DSTResult.NSID)           Valid |= 0x01;
    if (smSysInfo->d.LogInfo.d.DSTLog.DSTResult.FLBA[0])        Valid |= 0x02;
    if (smSysInfo->d.LogInfo.d.DSTLog.DSTResult.CodeType)       Valid |= 0x04;
    if (smSysInfo->d.LogInfo.d.DSTLog.DSTResult.StatusCode)     Valid |= 0x08;

    smSysInfo->d.LogInfo.d.DSTLog.DSTResult.Valid = Valid;
    smSysInfo->d.LogInfo.d.DSTLog.DSTResult.POH[0] = gGetLogPowerOnHours / 3;

    MEM_COPY((U8*)&smSysInfo->d.LogInfo.d.DSTLog.DSTLogEntry[0], (U8*)&smSysInfo->d.LogInfo.d.DSTLog.DSTResult, sizeof(tDST_LOG_ENTRY));

    MEM_CLR((U8*)&smSysInfo->d.LogInfo.d.DSTLog.DSTResult, sizeof(tDST_LOG_ENTRY));

    SyIn_Synchronize(SI_AREA_SMART, SYSINFO_WRITE, SI_SYNC_BY_NVMELOG);

    gCurrDSTOperation = 0;
    gCurrDSTCompletion = 0;
}

//-----------------------------------------------------------------------------
/**
    SMART Self Test handle

    @param[in]
**/
//-----------------------------------------------------------------------------
void DSTOperationImmediate()
{
    U32 CurrentTestTime;
    U32 TestTime = 0;
    U8 TimeOutFlag = FALSE;

    if (!gCurrDSTOperationImmed) return;

    CurrentTestTime = HalTimer_GetElapsedTimeMsec(gCurrDSTTime);

    if (gCurrDSTOperation == cDST_SHORT)
    {
        TestTime = (ShortDSTTime * 1000);
    }
    else if (gCurrDSTOperation == cDST_EXTENDED)
    {
        TestTime = (ExtenDSTTime * 1000);
    }

    if (CurrentTestTime >= TestTime)
    {
        TimeOutFlag = TRUE;
    }

    if (smSysInfo->d.LogInfo.d.DSTLog.DSTResult.Segment)
    {
        D_PRINTF("DSTOperationImmediate  Segment  %8X  \n",smSysInfo->d.LogInfo.d.DSTLog.DSTResult.Segment);

        DST_Completion(cDSTCompletedSegmentNum);
    }
    else if (smSysInfo->d.LogInfo.d.DSTLog.DSTErr)
    {
        DST_Completion(smSysInfo->d.LogInfo.d.DSTLog.DSTErr);
    }
    else if (TimeOutFlag)
    {
        //D_PRINTF("DSTOperation  TimeOut  %8X  \n",CurrentTestTime);
        DST_Completion(cDSTCompleted);
    }
    else
    {
        gCurrDSTCompletion = (100 * CurrentTestTime / TestTime);

        if (!gCurrDSTCompletion) gCurrDSTCompletion = 1;
    }
}
