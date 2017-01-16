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
#include "HalTempSensor.h"

// APL
#include "HldNvme.h"
#include "SysInfo.h"
#include "NvmeGetLogPage.h"
#if CO_SUPPORT_DEVICE_SELF_TEST
    #include "NvmeDeviceSelfTest.h"
#endif
#include "Cache.h"

// Code Section
#include "NormSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
static const uint16_t cMaxLogDataSize = 4096;

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static Error_t GetLogPageError(LogPageErrorInfoEntry_t* pLogPage, uint32_t* transferSize);
static Error_t GetLogPageSmartHealthInformation(LogPageSmartHealthInfoEntry_t* pLogPage, uint32_t nsid, uint32_t* transferSize);
static Error_t GetLogPageFirmwareSlotInformation(LogPageFirmwareSlotInfoEntry_t* pLogPage, uint32_t* transferSize);
static Error_t GetLogPageCommandEffectsLog(LogPageCommandEffectsEntry_t* pLogPage, uint32_t* transferSize);
static Error_t GetLogPageDeviceSelfTestLog(LogPageDeviceSelfTestEntry_t* pLogPage, uint32_t* transferSize);
static Error_t GetLogPageDellSmartHealthInfo(LogPageDellSmartHealthInfo_t* pLogPage, uint32_t nsid, uint32_t* transferSize);
static Error_t GetLogPageWcsSmartHealthInfo(LogPageWcsSmartHealthInfo_t* pLogPage, uint32_t nsid, uint32_t* transferSize);
static Error_t GetLogPageWcsDebuggingLog(LogPageWcsDebugLogHeader_t* pLogPage, uint8_t LogPageId, uint32_t* transferSize);
U8             CriticalWarningCheck(U16 Temp);
void           SmartBackupData(void);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
LogCritWarnTime_t gLogCritWarnTime[cLogCritWarnCount];  // in minutes
U64               gGetLogPowerOnHours;
U64               gGetLogIOBusyTime;
U32               gSmartIOTimerFlag;
U32               gSmartTimerMil;
U32               gWarnPeriodCnt;                       // in seconds
U32               gCritPeriodCnt;                       // in seconds
U8                gErrLogSysInfoUpdate = FALSE;
//-----------------------------------------------------------------------------
//  Imported data prototype
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function proto-tpye
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
#include "PerfSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
/**
    Handle temperature from Celsius to Kelvin

    @param[in]  temp   Temerature
**/
//-----------------------------------------------------------------------------
Error_t HandleTemperatureC2K(uint16_t* temp)
{
    if ((*temp != U16_MAX) && (*temp != S8_MIN))
    {
        if (*temp & B_7)  // Handle sign bit case
        {
            *temp &= (~B_7);
            if (*temp > CEL2KELVIN_CONVERTION)
            {
                *temp = 0;
            }
            else
            {
                *temp = CEL2KELVIN_CONVERTION - *temp;
            }
        }
        else
        {
            *temp += CEL2KELVIN_CONVERTION;
        }
    }
    else
    {
        return cEcError;
    }

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    CriticalWarningUpdate
**/
//-----------------------------------------------------------------------------
void CriticalWarningUpdate(void)
{
    U32 temperature;

    #if (THERAML_TEMPERATURE_FROM_SOC == TRUE)
    temperature = smcThermalSource[cThermalSourceSoc];
    #else
    temperature = smcThermalSource[cThermalSourceI2c];
    #endif

    // Handle sign bit
    if ((temperature != U16_MAX) && (temperature != S8_MIN))
    {
        if (temperature & B_7)  // Handle sign bit case
        {
            temperature = 0;
        }

        if (temperature >= TT_DEGREE_CRIT)
        {
            gCritPeriodCnt++;
            if (gCritPeriodCnt >= 60)
            {
                if (gLogCritWarnTime[cLogCritWarnCurr].critTime < 0xFFFFFFFF)
                {
                    gLogCritWarnTime[cLogCritWarnCurr].critTime++;
                }
                gCritPeriodCnt = 0;
            }

            if (gLogCritWarnTime[cLogCritWarnCurr].critTime > gLogCritWarnTime[cLogCritWarnPost].critTime)
            {
                gLogCritWarnTime[cLogCritWarnPost].critTime = gLogCritWarnTime[cLogCritWarnCurr].critTime;
            }
        }
        else if (temperature >= TT_DEGREE_WARN)
        {
            gWarnPeriodCnt++;
            if (gWarnPeriodCnt >= 60)
            {
                if (gLogCritWarnTime[cLogCritWarnCurr].warnTime < 0xFFFFFFFFFF)
                {
                    gLogCritWarnTime[cLogCritWarnCurr].warnTime++;
                }
                gWarnPeriodCnt = 0;
            }

            if (gLogCritWarnTime[cLogCritWarnCurr].warnTime > gLogCritWarnTime[cLogCritWarnPost].warnTime)
            {
                gLogCritWarnTime[cLogCritWarnPost].warnTime = gLogCritWarnTime[cLogCritWarnCurr].warnTime;
            }
        }
    }
}

#include "NormSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
/**
    Handle NVMe get log initial
**/
//-----------------------------------------------------------------------------
void HandleLogInfoInit(InitBootMode_t initMode)
{
    if (smSysInfo->d.Header.d.SubHeader.d.NVMeLog.d.dwValidTag != SI_TAG_LOGINFO)
    {
        MEM_CLR((U8*)&smSysInfo->d.LogInfo.all[0], sizeof(tSI_PAYLOAD_LOGINFO));
        smSysInfo->d.Header.d.SubHeader.d.NVMeLog.d.dwValidTag = SI_TAG_LOGINFO;
    }

    // - - - Prepare Frontend information
    gGetLogIOBusyTime               = smSysInfo->d.LogInfo.d.SmrLog.ctrlBusyTime;
    gGetLogPowerOnHours             = smSysInfo->d.LogInfo.d.SmrLog.pwrOnHrs;
    gNvmeIfMgr.dataUnitsRead        = smSysInfo->d.LogInfo.d.SmrLog.dataUniRd;
    gNvmeIfMgr.dataUnitsWritten     = smSysInfo->d.LogInfo.d.SmrLog.dataUniWr;
    gNvmeIfMgr.hostReadCommands     = smSysInfo->d.LogInfo.d.SmrLog.hostRdCmd;
    gNvmeIfMgr.hostWriteCommands    = smSysInfo->d.LogInfo.d.SmrLog.hostWrCmd;
    gNvmeIfMgr.UnalignCnt           = smSysInfo->d.LogInfo.d.SmrLog.WCSUnalignedIO;

    // For log
    gLogCritWarnTime[cLogCritWarnPost].warnTime = smSysInfo->d.LogInfo.d.SmrLog.warnTempTime;
    gLogCritWarnTime[cLogCritWarnPost].critTime = (U32)(smSysInfo->d.LogInfo.d.SmrLog.critTempTime);

    gLogCritWarnTime[cLogCritWarnCurr].warnTime = gLogCritWarnTime[cLogCritWarnPost].warnTime;
    gLogCritWarnTime[cLogCritWarnCurr].critTime = gLogCritWarnTime[cLogCritWarnPost].critTime;

    gWarnPeriodCnt = 0;
    gCritPeriodCnt = 0;

    // For async Event
    smAvailableSpare = U32_MAX;

#if CO_SUPPORT_DEVICE_SELF_TEST
    NvmeDeviceSelfTestInit(initMode);
#endif
}

//-----------------------------------------------------------------------------
/**
    Handle Error log backup

**/
//-----------------------------------------------------------------------------
void HandleSaveErrLog(HcmdQ_t* pHcmdQ)
{
    uint8_t  errIdx;
    uint32_t sftmp = 0;
    NvmeSdb_t* pSdb;

    smSysInfo->d.LogInfo.d.ErrLog.TotalErrCnt++;

    for(errIdx = (NVME_ERROR_LOG_ENTIRIES - 1); errIdx > 0; errIdx--)
    {
        MEM_COPY((U8*)&smSysInfo->d.LogInfo.d.ErrLog.ErrLogEntry[errIdx], (U8*)&smSysInfo->d.LogInfo.d.ErrLog.ErrLogEntry[(errIdx - 1)], sizeof(LogPageErrorInfoEntry_t));
    }

    pSdb    = NvmeCmd_GetSdbPointer(pHcmdQ);
    sftmp   = (pSdb->DNR << 15) | (pSdb->M << 14) | (pSdb->reserved << 12) | (pSdb->SCT << 9) | (pSdb->SC << 1) | pSdb->P;

    smSysInfo->d.LogInfo.d.ErrLog.ErrLogEntry[0].ErrCnt               = smSysInfo->d.LogInfo.d.ErrLog.TotalErrCnt;
    smSysInfo->d.LogInfo.d.ErrLog.ErrLogEntry[0].sqID                 = (uint16_t)pSdb->SQID;
    smSysInfo->d.LogInfo.d.ErrLog.ErrLogEntry[0].cmdID                = (uint16_t)pSdb->CID;
    smSysInfo->d.LogInfo.d.ErrLog.ErrLogEntry[0].StaField             = (uint16_t)sftmp;
    smSysInfo->d.LogInfo.d.ErrLog.ErrLogEntry[0].ParaErrLoc.ByteLoc   = gNvmeIfMgr.lastErrorByteOffset;
    smSysInfo->d.LogInfo.d.ErrLog.ErrLogEntry[0].ParaErrLoc.BitLoc    = gNvmeIfMgr.lastErrorBitOffset;
    smSysInfo->d.LogInfo.d.ErrLog.ErrLogEntry[0].lba                  = pHcmdQ->addrInfo.lba;
    smSysInfo->d.LogInfo.d.ErrLog.ErrLogEntry[0].nameSpace            = pHcmdQ->addrInfo.nsId0 + 1;
    smSysInfo->d.LogInfo.d.ErrLog.ErrLogEntry[0].VenSpeInfoAvi        = 0;
    smSysInfo->d.LogInfo.d.ErrLog.ErrLogEntry[0].cmdSpeInfo           = 0;

    SyIn_Synchronize(SI_AREA_SMART, SYSINFO_WRITE, SI_SYNC_BY_NVMELOG);
}

//-----------------------------------------------------------------------------
/**
    Handle Error log backup

**/
//-----------------------------------------------------------------------------
void HandleSaveNoSpecErrLog(void)
{
    uint8_t  errIdx;

    smSysInfo->d.LogInfo.d.ErrLog.TotalErrCnt++;

    for(errIdx = (NVME_ERROR_LOG_ENTIRIES - 1); errIdx > 0; errIdx--)
    {
        MEM_COPY((U8*)&smSysInfo->d.LogInfo.d.ErrLog.ErrLogEntry[errIdx], (U8*)&smSysInfo->d.LogInfo.d.ErrLog.ErrLogEntry[(errIdx - 1)], sizeof(LogPageErrorInfoEntry_t));
    }

    smSysInfo->d.LogInfo.d.ErrLog.ErrLogEntry[0].ErrCnt               = smSysInfo->d.LogInfo.d.ErrLog.TotalErrCnt;
    smSysInfo->d.LogInfo.d.ErrLog.ErrLogEntry[0].sqID                 = 0xFFFF;
    smSysInfo->d.LogInfo.d.ErrLog.ErrLogEntry[0].cmdID                = 0xFFFF;
    smSysInfo->d.LogInfo.d.ErrLog.ErrLogEntry[0].StaField             = 0;
    smSysInfo->d.LogInfo.d.ErrLog.ErrLogEntry[0].ParaErrLoc.ByteLoc   = 0xFFFF;
    smSysInfo->d.LogInfo.d.ErrLog.ErrLogEntry[0].ParaErrLoc.BitLoc    = 0xFFFF;
    smSysInfo->d.LogInfo.d.ErrLog.ErrLogEntry[0].lba                  = 0;
    smSysInfo->d.LogInfo.d.ErrLog.ErrLogEntry[0].nameSpace            = U32_MAX;
    smSysInfo->d.LogInfo.d.ErrLog.ErrLogEntry[0].VenSpeInfoAvi        = 0;
    smSysInfo->d.LogInfo.d.ErrLog.ErrLogEntry[0].cmdSpeInfo           = 0;
}

//-----------------------------------------------------------------------------
/**
    Handle Error log backup

**/
//-----------------------------------------------------------------------------
void HandleErrLogUpdateSysInfo(HcmdQ_t* pHcmdQ)
{
    SyIn_Synchronize(SI_AREA_SMART, SYSINFO_WRITE, SI_SYNC_BY_NVMELOG);
    gErrLogSysInfoUpdate = FALSE;
}

//-----------------------------------------------------------------------------
/**
    Backup SMART attribute datas
**/
//-----------------------------------------------------------------------------
void SmartBackupData(void)
{
    smSysInfo->d.LogInfo.d.SmrLog.ctrlBusyTime      = gGetLogIOBusyTime;
    smSysInfo->d.LogInfo.d.SmrLog.pwrOnHrs          = gGetLogPowerOnHours;
    smSysInfo->d.LogInfo.d.SmrLog.dataUniRd         = gNvmeIfMgr.dataUnitsRead;
    smSysInfo->d.LogInfo.d.SmrLog.dataUniWr         = gNvmeIfMgr.dataUnitsWritten;
    smSysInfo->d.LogInfo.d.SmrLog.hostRdCmd         = gNvmeIfMgr.hostReadCommands;
    smSysInfo->d.LogInfo.d.SmrLog.hostWrCmd         = gNvmeIfMgr.hostWriteCommands;
    smSysInfo->d.LogInfo.d.SmrLog.WCSUnalignedIO    = gNvmeIfMgr.UnalignCnt;
    smSysInfo->d.LogInfo.d.SmrLog.warnTempTime      = gLogCritWarnTime[cLogCritWarnCurr].warnTime;
    smSysInfo->d.LogInfo.d.SmrLog.critTempTime      = (U64)(gLogCritWarnTime[cLogCritWarnCurr].critTime);

    SyIn_Synchronize(SI_AREA_SMART, SYSINFO_WRITE, SI_SYNC_BY_NVMELOG);
}

//-----------------------------------------------------------------------------
/**


    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_GetLogPage(HcmdQ_t* pHcmdQ)
{
    uint32_t transferSize;
    NvmeAdminGetLogPageCommand_t* pCdb;
    NvmeCdb_t* pCmdCdb;
    void* pLogPage;

    pCdb = (NvmeAdminGetLogPageCommand_t *)NvmeCmd_GetCdbPointer(pHcmdQ);

    pLogPage = (void *)NvmeCmd_GetScratchBufferPointer();
    MEM_CLR(pLogPage, cNvmeScratchBufferSize);

    pCmdCdb = (NvmeCdb_t*) pCdb;

    BEGIN_HOST_DBG_DUMP(DBG_DUMP_HOST_NONRW)
        D_PRINTF("%s LID=%1X NUMD=%X NSID=%X\n", __FUNCTION__, pCdb->LID, pCdb->NUMD, pCmdCdb->NSID);
    END_HOST_DBG_DUMP

    transferSize = (pCdb->NUMD+1) * sizeof(uint32_t); // convert to 1 based byte size

    // check size
    if (transferSize > cMaxLogDataSize)
    {
        pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
        gNvmeIfMgr.lastErrorBitOffset  = 0;
        gNvmeIfMgr.lastErrorByteOffset = ERR_DW10_OFFSET + 2; // DW10, NUMD
        return;
    }

    switch (pCdb->LID)
    {
        case cLidErrorInformation:
            pHcmdQ->error = GetLogPageError((LogPageErrorInfoEntry_t *)pLogPage, &transferSize);
            break;
        case cLidSmartHealthInformation:
            break;
        case cLidFirmwareSlotInformation:
            pHcmdQ->error = GetLogPageFirmwareSlotInformation((LogPageFirmwareSlotInfoEntry_t*) pLogPage, &transferSize);
            break;
        case cLidCommandEffectsLog:
            pHcmdQ->error = GetLogPageCommandEffectsLog((LogPageCommandEffectsEntry_t*) pLogPage, &transferSize);
            break;
    #if CO_SUPPORT_DEVICE_SELF_TEST
        case cLidDeviceSelfTestLog:
            pHcmdQ->error = GetLogPageDeviceSelfTestLog((LogPageDeviceSelfTestEntry_t*) pLogPage, &transferSize);
            break;
    #endif
    #if (_CUSTOMER_ == CUSTOMER_DELL)
        case cLidDellSmartHealthInfo:
            break;
    #endif

    #if (_CUSTOMER_ == CUSTOMER_MICROSOFT)
        case cLidWCSSmartHealthInfo:
            break;
        case cLidWCSDebugC1Log:
        case cLidWCSDebugC2Log:
            pHcmdQ->error = GetLogPageWcsDebuggingLog((LogPageWcsDebugLogHeader_t*) pLogPage, pCdb->LID, &transferSize);
            break;
    #endif

        default:
            pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidLogPage, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
            gNvmeIfMgr.lastErrorBitOffset  = 0;
            gNvmeIfMgr.lastErrorByteOffset = ERR_DW10_OFFSET; // DW10, LID
            return;
    }

    if ((pCdb->LID == cLidSmartHealthInformation) ||
        (pCdb->LID == cLidDellSmartHealthInfo)    ||
        (pCdb->LID == cLidWCSSmartHealthInfo))
    {
        if (pCmdCdb->NSID != cNvmeGlobalNamespaceId)
        {
            gNvmeIfMgr.lastErrorBitOffset  = 0;
            gNvmeIfMgr.lastErrorByteOffset = ERR_DW1_OFFSET; // DW1, NSID;

            if (pCmdCdb->NSID == 0 || pCmdCdb->NSID > NVME_NUM_OF_NAMESPACES || !gNvmeIfMgr.ns[pCmdCdb->NSID - 1].Attach)
            {
                pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidNamespaceOrFormat, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
            }

            #if (CO_SUPPORT_LOGPAGE_PER_NAMESPACE == FALSE)
            pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
            #endif
        }

        if (pHcmdQ->error == cEcNoError)
        {
            MsgSmart_t*  pMsg;

            Host_AdminCmdOverlapReq();
            pHcmdQ->attr.all |= ATTR_OVERLAPPED;
            pMsg = (MsgSmart_t*) GetHcmdMsgPtr(pHcmdQ);
            MSG_SET_HOST_INIT(pMsg, cMcSmartOp);
            pMsg->subOp = cSmrAttri;
            Host_MediaReq();
            IPC_SendMsgQ(cH2cReqQueue,  (Msg_t*)pMsg);
        }

        return;
    }

    if (pHcmdQ->error == cEcNoError)
    {
        pHcmdQ->bufDesc.ssdIndex = SSD_IF_START;
        BIT_CLR(gNvmeIfMgr.asyncEventMask, BIT(pCdb->LID));
        AplHost_TransferWholeData(pHcmdQ, pLogPage, transferSize, TRUE);
    }
}

//-----------------------------------------------------------------------------
/**
    NvmeCmd_GetLogPageDone

    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_GetLogPageDone(HcmdQ_t* pHcmdQ)
{
    uint32_t transferSize;
    NvmeAdminGetLogPageCommand_t* pCdb;
    void* pLogPage;

    pCdb = (NvmeAdminGetLogPageCommand_t *)NvmeCmd_GetCdbPointer(pHcmdQ);

    transferSize = (pCdb->NUMD+1) * sizeof(uint32_t); // convert to 1 based byte size

    pLogPage = (void *)NvmeCmd_GetScratchBufferPointer();
    MEM_CLR(pLogPage, cNvmeScratchBufferSize);

    BEGIN_HOST_DBG_DUMP(DBG_DUMP_HOST_NONRW)
        D_PRINTF("%s LID=%1X NUMD=%X\n", __FUNCTION__, pCdb->LID, pCdb->NUMD);
    END_HOST_DBG_DUMP

    if (pCdb->LID == cLidSmartHealthInformation)
    {
        pHcmdQ->error = GetLogPageSmartHealthInformation((LogPageSmartHealthInfoEntry_t *)pLogPage, pCdb->DW9_0.NSID, &transferSize);
    }
    else if (pCdb->LID == cLidDellSmartHealthInfo)
    {
        pHcmdQ->error = GetLogPageDellSmartHealthInfo((LogPageDellSmartHealthInfo_t*) pLogPage, pCdb->DW9_0.NSID, &transferSize);
    }
    else if (pCdb->LID == cLidWCSSmartHealthInfo)
    {
        pHcmdQ->error = GetLogPageWcsSmartHealthInfo((LogPageWcsSmartHealthInfo_t*) pLogPage, pCdb->DW9_0.NSID, &transferSize);
    }

    if (pHcmdQ->error == cEcNoError)
    {
        pHcmdQ->bufDesc.ssdIndex = SSD_IF_START;
        BIT_CLR(gNvmeIfMgr.asyncEventMask, BIT(pCdb->LID));
        AplHost_TransferWholeData(pHcmdQ, pLogPage, transferSize, TRUE);
    }

BEGIN_HOST_DBG_DUMP(DBG_DUMP_HOST_NONRW)
    D_PRINTF("Smart Xfer Done\n");
END_HOST_DBG_DUMP

    NvmeCmd_Complete(pHcmdQ);
}

//-----------------------------------------------------------------------------
/**
    Get log page - error log

    @param[in]  pLogPage   log page buffer pointer
**/
//-----------------------------------------------------------------------------
static Error_t GetLogPageError(LogPageErrorInfoEntry_t* pLogPage, uint32_t* transferSize)
{
#if 1
    MEM_COPY((uint8_t*)&pLogPage[0], (uint8_t*)&smSysInfo->d.LogInfo.d.ErrLog.ErrLogEntry[0], sizeof(LogPageErrorInfoEntry_t) * NVME_ERROR_LOG_ENTIRIES);
#endif
    return (cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    CriticalWarningCheck
**/
//-----------------------------------------------------------------------------
U8 CriticalWarningCheck(U16 Temp)
{
    U8 CriticalWarning = 0;
    U8 i;

    #if 0
    if (AvaiSpareThreshold && (smSysInfo->d.LogInfo.d.SmrLog.availableSpare < AvaiSpareThreshold))
    {
        CriticalWarning |= cCriticalWarnSpare;  // Spare
    }
    #endif

    for (i = 0; i < 2; i++)
    {
        if (((Temp > gNvmeIfMgr.currTMPTH[i][0]) && (gNvmeIfMgr.currTMPTH[i][0])) ||
            ((Temp < gNvmeIfMgr.currTMPTH[i][1]) && (gNvmeIfMgr.currTMPTH[i][1])))
        {
            CriticalWarning |= cCriticalWarnTemperature;  // Temperature
        }
    }

    CriticalWarning |= smSysInfo->d.LogInfo.d.SmrLog.CriticalWarn;

    if (CriticalWarning)
    {
        D_PRINTF("CriticalWarning=%X\n", CriticalWarning);
        D_PRINTF("AvaiSpareThreshold=%X, availableSpare=%X\n", AvaiSpareThreshold, smSysInfo->d.LogInfo.d.SmrLog.availableSpare);
        D_PRINTF("currTMPTH[0][0]=%X, Temp=%X\n", gNvmeIfMgr.currTMPTH[0][0], Temp);
        D_PRINTF("currTMPTH[1][0]=%X, Temp=%X\n", gNvmeIfMgr.currTMPTH[1][0], Temp);
    }

    return(CriticalWarning);
}

//-----------------------------------------------------------------------------
/**
    Get log page - Smart Health Information

    @param[in]  pLogPage   log page buffer pointer
**/
//-----------------------------------------------------------------------------
static Error_t GetLogPageSmartHealthInformation(LogPageSmartHealthInfoEntry_t* pLogPage, uint32_t nsid, uint32_t* transferSize)
{
    uint16_t temp;
    uint8_t  CriticalWarning;

    // Celsius
    temp = smcThermalSource[cThermalSourceI2c];

    // Celsius to Kelvin
    if (HandleTemperatureC2K((uint16_t*)&temp) == cEcNoError)
    {
        pLogPage->temperature = temp;
    }
    else
    {
        pLogPage->temperature = CEL2KELVIN_CONVERTION;
    }

    CriticalWarning = CriticalWarningCheck(pLogPage->temperature);

    pLogPage->criticalWarningSpareSpace             = CriticalWarning >> 0;
    pLogPage->criticalWarningTemperature            = CriticalWarning >> 1;
    pLogPage->criticalWarningMediaInternalError     = CriticalWarning >> 2;
    pLogPage->criticalWarningReadOnlyMode           = CriticalWarning >> 3;
    pLogPage->criticalWarningVolatileFail           = CriticalWarning >> 4;

    pLogPage->availableSpareThreshold               = AvaiSpareThreshold;

    pLogPage->dataUnitsRead[0]                      = ((gNvmeIfMgr.dataUnitsRead) / 1000) * (HLBU_DATA_SIZE / 512);
    pLogPage->dataUnitsWritten[0]                   = ((gNvmeIfMgr.dataUnitsWritten) / 1000) * (HLBU_DATA_SIZE / 512);
    pLogPage->hostReadCommands[0]                   = gNvmeIfMgr.hostReadCommands;
    pLogPage->hostWriteCommands[0]                  = gNvmeIfMgr.hostWriteCommands;
    pLogPage->powerOnHours[0]                       = gGetLogPowerOnHours / 3;
    pLogPage->controllerBusyTime[0]                 = gGetLogIOBusyTime / 60;
    pLogPage->powerCycles[0]                        = smSysInfo->d.LogInfo.d.SmrLog.pwrCycCnt;
    pLogPage->unsafeShutdowns[0]                    = smSysInfo->d.LogInfo.d.SmrLog.unsafePwrCycCnt;
    pLogPage->numberofErrorInformationLogEntries[0] = smSysInfo->d.LogInfo.d.ErrLog.TotalErrCnt;

    // TODO:
    pLogPage->availableSpare    = smSysInfo->d.LogInfo.d.SmrLog.availableSpare;
    pLogPage->mediaErrors[0]    = smSysInfo->d.LogInfo.d.SmrLog.medAndDataErrCnt;
    pLogPage->percentageUsed    = smSysInfo->d.LogInfo.d.SmrLog.percentageUsed;

    pLogPage->WarningTempTime = (gLogCritWarnTime[cLogCritWarnCurr].warnTime > 0xFFFFFFFF)? 0xFFFFFFFF : (U32)gLogCritWarnTime[cLogCritWarnCurr].warnTime;
    pLogPage->CriticalTempTime = gLogCritWarnTime[cLogCritWarnCurr].critTime;

    pLogPage->TempSensor1       = pLogPage->temperature;

    return (cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Get log page - Firmware Slot Information

    @param[in]  pLogPage   log page buffer pointer
**/
//-----------------------------------------------------------------------------
static Error_t GetLogPageFirmwareSlotInformation(LogPageFirmwareSlotInfoEntry_t* pLogPage, uint32_t* transferSize)
{
    DfwDescript_t   fw_desc;
    DfwModuleInfo_t fw_module;
    DfwSelectTag_t  sel_info;
    DfwImageTag_t   Img_Tag;
    U32             FwAddress;
    U8              FwBlkCnt;
    U8              Fw_num;

    // Get Active Slot number
    pLogPage->AFI_SLOT_ACTIVE = smActiveFwSlot;

    // Get Next Active Slot number
    HalSflash_Copy(&sel_info, DFW_CURRENT_HEAD_OFFSET, sizeof(sel_info));
    if (sel_info.id == DFW_FW_SELECT_TAG)
    {
        pLogPage->AFI_SLOT_ACTIVE_NEXT = sel_info.select;
    }
    else
    {
        pLogPage->AFI_SLOT_ACTIVE_NEXT = 1;
    }

    HalSflash_Copy(&fw_desc, DFW_CURRENT_HEAD_OFFSET + DFW_FW_SELECT_SIZE, DFW_FW_DESC_SIZE);
    FwAddress = DFW_CURRENT_HEAD_OFFSET + DFW_FW_SELECT_SIZE + DFW_FW_DESC_SIZE;

    for (FwBlkCnt = 0; FwBlkCnt < fw_desc.sec_cnt; FwBlkCnt++)
    {
        HalSflash_Copy(&fw_module, FwAddress, sizeof(DfwModuleInfo_t));
        if ((fw_module.tag & 0xFFFF) == DFW_FW_MAFW_TAG)
        {
            Fw_num = DFW_GET_FWNUM(fw_module) - 1;
            if (Fw_num < NVME_NUM_OF_FW_SLOTS)
            {
                HalSflash_Copy(&Img_Tag, fw_module.offset, sizeof(Img_Tag));
                // Get each FW Revision
                if (Img_Tag.id == DFW_FW_IMAGE_TAG)
                {
                    MEM_COPY(pLogPage->FRS[Fw_num], Img_Tag.fw_revision, 8);
                    pLogPage->FRS[Fw_num][2] = gCapacityMap[_OP_][gCapacityIndex].CapacityChar;
                }
            }
        }
        FwAddress += sizeof(DfwModuleInfo_t);
    }
    return (cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Get log page - Command Effects Log

    @param[in]  pLogPage   log page buffer pointer
**/
//-----------------------------------------------------------------------------
static Error_t GetLogPageCommandEffectsLog(LogPageCommandEffectsEntry_t* pLogPage, uint32_t* transferSize)
{
    pLogPage->ACS[cDeleteIoSubmissionQueue].CSUPP  = 1;
    pLogPage->ACS[cCreateIoSubmissionQueue].CSUPP  = 1;
    pLogPage->ACS[cGetLogpage].CSUPP               = 1;
    pLogPage->ACS[cDeleteIoCompletionQueue].CSUPP  = 1;
    pLogPage->ACS[cCreateIoCompletionQueue].CSUPP  = 1;
    pLogPage->ACS[cIdentify].CSUPP                 = 1;
    pLogPage->ACS[cAbort].CSUPP                    = 1;
    pLogPage->ACS[cSetFeatures].CSUPP              = 1;
    pLogPage->ACS[cGetFeatures].CSUPP              = 1;
    pLogPage->ACS[cAsynchronousEventRequest].CSUPP = 1;

    pLogPage->ACS[cNamespaceManagement].CSUPP   = CO_SUPPORT_NAMESPACE_MANAGEMENT;
    pLogPage->ACS[cFirmwareActivate].CSUPP      = CO_SUPPORT_FW_DOWNLOAD;
    pLogPage->ACS[cFirmwareImageDownload].CSUPP = CO_SUPPORT_FW_DOWNLOAD;
    pLogPage->ACS[cNamespaceAttachment].CSUPP   = CO_SUPPORT_NAMESPACE_MANAGEMENT;
    pLogPage->ACS[cFormatNvm].CSUPP             = CO_SUPPORT_FORMAT;
    pLogPage->ACS[cSecuritySend].CSUPP          = CO_SUPPORT_SECURITY;
    pLogPage->ACS[cSecurityReceive].CSUPP       = CO_SUPPORT_SECURITY;

    pLogPage->IOCS[cFlush].CSUPP = 1;
    pLogPage->IOCS[cWrite].CSUPP = 1;
    pLogPage->IOCS[cRead].CSUPP  = 1;

    pLogPage->IOCS[cWriteUncorrectable].CSUPP   = CO_SUPPORT_WRITE_UNCORRECTABLE;
    pLogPage->IOCS[cCompare].CSUPP              = CO_SUPPORT_COMPARE;
    pLogPage->IOCS[cWriteZeroes].CSUPP          = CO_SUPPORT_WRITE_ZEROES;
    pLogPage->IOCS[cDatasetManagement].CSUPP    = CO_SUPPORT_DATASET_MANAGEMENT;
    pLogPage->IOCS[cReservationRegister].CSUPP  = CO_SUPPORT_RESERVATIONS;
    pLogPage->IOCS[cReservationReport].CSUPP    = CO_SUPPORT_RESERVATIONS;
    pLogPage->IOCS[cReservationAcquire].CSUPP   = CO_SUPPORT_RESERVATIONS;
    pLogPage->IOCS[cReservationRelease].CSUPP   = CO_SUPPORT_RESERVATIONS;

    return (cEcNoError);
}

#if CO_SUPPORT_DEVICE_SELF_TEST
//-----------------------------------------------------------------------------
/**
    Get log page - Command Effects Log

    @param[in]  pLogPage   log page buffer pointer
**/
//-----------------------------------------------------------------------------
static Error_t GetLogPageDeviceSelfTestLog(LogPageDeviceSelfTestEntry_t* pLogPage, uint32_t* transferSize)
{
    pLogPage->Operation  = gCurrDSTOperation;
    pLogPage->Completion = gCurrDSTCompletion;

    D_PRINTF("gCurrDSTCompletion=%2X\n",gCurrDSTCompletion);

    MEM_COPY((uint8_t*)&pLogPage->DSTResultData[0], (uint8_t*)&smSysInfo->d.LogInfo.d.DSTLog.DSTLogEntry[0], sizeof(tDST_LOG_ENTRY) * 20);

    return (cEcNoError);
}
#endif

//-----------------------------------------------------------------------------
/**
    Get log page - Dell SmartAttributes

    @param[in]  pLogPage   log page buffer pointer
**/
//-----------------------------------------------------------------------------
static Error_t GetLogPageDellSmartHealthInfo(LogPageDellSmartHealthInfo_t* pLogPage, uint32_t nsid, uint32_t* transferSize)
{
    pLogPage->ReAssignedSectorCount       = smSysInfo->d.LogInfo.d.SmrLog.ReAssignedSecCnt;
    pLogPage->ProgramFailCountWorst       = smSysInfo->d.LogInfo.d.SmrLog.PgmFailCnt_W;
    pLogPage->ProgramFailCountTotal       = smSysInfo->d.LogInfo.d.SmrLog.PgmFailCnt_T;
    pLogPage->EraseFailCountWorst         = smSysInfo->d.LogInfo.d.SmrLog.ErsFailCnt_W;
    pLogPage->EraseFailCountTotal         = smSysInfo->d.LogInfo.d.SmrLog.ErsFailCnt_T;
    pLogPage->WearLevelingCount           = smSysInfo->d.LogInfo.d.SmrLog.WearLvlCnt;
    pLogPage->UsedReservedBlockCountWorst = smSysInfo->d.LogInfo.d.SmrLog.UsedRsvBlkCnt_W;
    pLogPage->UsedReservedBlockCountTotal = smSysInfo->d.LogInfo.d.SmrLog.UsedRsvBlkCnt_T;
    pLogPage->ReservedBlockCount          = smSysInfo->d.LogInfo.d.SmrLog.RsvBlkCnt;

    return (cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Get log page - WCS SmartAttributes

    @param[in]  pLogPage   log page buffer pointer
**/
//-----------------------------------------------------------------------------
static Error_t GetLogPageWcsSmartHealthInfo(LogPageWcsSmartHealthInfo_t* pLogPage, uint32_t nsid, uint32_t* transferSize)
{
    U8  idx;

    pLogPage->MediaUnitsWritten[0]      = ((smSysInfo->d.LogInfo.d.SmrLog.WCSFlushCnt / 512) *
                                           (gNfInfo.dataBytesPerPage / 1000));
    pLogPage->CapacitorHealth           = 0;
    pLogPage->EccIterations[0]          = (U32)smSysInfo->d.GList.d.GListBrief.wCntRead;
    pLogPage->SupportedFeatures         = TRUE;

    for (idx = 0; idx < 5; idx++)
    {
        pLogPage->TemperatureThrottlingWarn[idx] = (gLogCritWarnTime[cLogCritWarnCurr].warnTime > 0xFFFFFFFFFF) ? 0xFF : (U8)(gLogCritWarnTime[cLogCritWarnCurr].warnTime >> (idx * 8));
    }

    for (idx = 0; idx < 3; idx++)
    {
        pLogPage->TemperatureThrottlingCrit[idx] = (gLogCritWarnTime[cLogCritWarnCurr].critTime > 0x00FFFFFF) ? 0xFF : (U8)(gLogCritWarnTime[cLogCritWarnCurr].critTime >> (idx * 8));
    }

    pLogPage->PowerConsumption          = 0xFF;
    pLogPage->WearRangeDelta            = smSysInfo->d.LogInfo.d.SmrLog.WCSWearRangeDelta;

    if (gNvmeIfMgr.UnalignCnt > 0xFFFFFFFFFFFF)
    {
        for (idx = 0; idx < 6; idx++)
        {
            pLogPage->UnalignedIO[idx] = 0xFF;
        }
    }
    else
    {
        for (idx = 0; idx < 6; idx++)
        {
            pLogPage->UnalignedIO[idx] = (U8)(gNvmeIfMgr.UnalignCnt >> (idx * 8));
        }
    }

    pLogPage->MappedLBAs = smSysInfo->d.LogInfo.d.SmrLog.WCSLbaMapCnt;

    if (smSysInfo->d.GList.d.GListBrief.wCntWrite > 0xFF)
    {
        pLogPage->ProgramFailCount = 0xFF;
    }
    else
    {
        pLogPage->ProgramFailCount = (U8)smSysInfo->d.GList.d.GListBrief.wCntWrite;
    }

    if (smSysInfo->d.GList.d.GListBrief.wCntErase > 0xFF)
    {
        pLogPage->EraseFailCount = 0xFF;
    }
    else
    {
        pLogPage->EraseFailCount = (U8)smSysInfo->d.GList.d.GListBrief.wCntErase;
    }

    return (cEcNoError);
}
//-----------------------------------------------------------------------------
/**
    Get log page - WCS Debugging Logs C1 and C2

    @param[in]  pLogPage   log page buffer pointer
**/
//-----------------------------------------------------------------------------
static Error_t GetLogPageWcsDebuggingLog(LogPageWcsDebugLogHeader_t* pLogPage, uint8_t LogPageId, uint32_t* transferSize)
{
    //Error case
    //Wait add

    //Non-Error case
    pLogPage->LogPageId = LogPageId;

    return (cEcNoError);
}
