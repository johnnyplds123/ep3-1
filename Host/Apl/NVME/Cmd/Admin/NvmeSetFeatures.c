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
#include "HalPm.h"
#include "HalTimer.h"
#include "HalNvmeXfer.h"

// APL
#include "HldNvme.h"
#include "NvmeIdentify.h"
#include "SysInfo.h"
#include "Cache.h"

// Code Section
#include "NormSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define APST_BUFFER_SIZE    0x100

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief NVMe Admin Command structure - Set Features
typedef  struct
{
    AdminCommandCommon_t DW9_0;    ///< DW9..0

    uint32_t FID:8;           ///< Feature Identifier (DW10 bits[7:0])
    uint32_t reserved10:23;   ///< Reserved (DW10 bits[30:8])
    uint32_t SV:1;            ///< Save (DW10 bits[31])

    uint32_t DW11;            ///< DW11 Feature-specific

    uint32_t reserved12[4];   ///< DW15..12

} NvmeAdminSetFeaturesCommand_t;
COMPILE_ASSERT(sizeof(NvmeAdminSetFeaturesCommand_t)==64, "NVMe command size shall be 64");

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static Error_t AllocQueues(Dw11NumberOfQueues_t* pNqn, Dw0NumberOfQueues_t* pNqa);
static Error_t SetPowerManagement(NvmeAdminSetFeaturesCommand_t* pCdb);
static Error_t SetTemperatureThreshold(NvmeAdminSetFeaturesCommand_t* pCdb);
static void SetAutonomousPowerStateTransition(HcmdQ_t* pHcmdQ);
static void SetManagementTemp(HcmdQ_t* pHcmdQ);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
uint32_t currentFeatureValue[cLastSupportedFid+1];
const uint32_t defaultFeatureValue[cLastSupportedFid+1] =
{
    0,
    (2<<24)|(2<<16)|(2<<8)|(3<<0),                      // FID=1,  Arbitration burst AB=3, LPW=2, MPW=2, HPW=2
    0,                                                  // FID=2,  Power management
    0,                                                  // FID=3,  LBA Range Type
    60 + CEL2KELVIN_CONVERTION,                         // FID=4,  Temperature threshold
    0,                                                  // FID=5,  Error Recovery
    1,                                                  // FID=6,  Volatile write cache
    MAKE_U32(HW_MAX_IO_QUEUES-1, HW_MAX_IO_QUEUES-1),   // FID=7,  Number of queues
    (0<<8)|(0<<0),                                      // FID=8,  Interrupt Coalescing
    (0<<16)|(0<<0),                                     // FID=9,  Interrupt Vector Configuration
    0,                                                  // FID=10, Write Atomicity
    0,                                                  // FID=11, Asynchornous Event Configuration
    0,                                                  // FID=12, Autonoumus Power State Transition
};

const uint16_t defaTMPTH[2][2] =
{
    NVME_WARNING_TEMPERATURE, 0, 0xFFFF, 0,  ///< default Temperature Threshold [0:Composite,1:Sensor 1][0:Over,1:Under]
};

#if (CO_SUPPORT_APST == TRUE)
const NvmeApstEntry_t defaAPST[NVME_NUM_OF_POWER_STATES] =
{
    {0, 3,  5 * KERNEL_TICK_PER_SEC, 0},
    {0, 4, 10 * KERNEL_TICK_PER_SEC, 0},
    {0, 0,                        0, 0},
    {0, 0,                        0, 0},
    {0, 0,                        0, 0}
};
#endif

uint8_t MT_SV;

//-----------------------------------------------------------------------------
//  Imported data prototype
//-----------------------------------------------------------------------------
extern PowerStateDescriptor_t tPSD[];

//-----------------------------------------------------------------------------
//  Imported function proto-tpye
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
    Set Features function initial

    @param[in]  initMode   Initial mode
**/
//-----------------------------------------------------------------------------
void NvmeSetFeatureInit(InitBootMode_t initMode)
{
    uint8_t i;

    MEM_COPY(currentFeatureValue, defaultFeatureValue, sizeof(currentFeatureValue));

    if (smSysInfo->d.NVMeData.d.Feature.Tag != SI_FEATURE_TAG)
    {
        MEM_COPY(smSysInfo->d.NVMeData.d.Feature.SaveFeature, defaultFeatureValue, sizeof(defaultFeatureValue));
        MEM_COPY(smSysInfo->d.NVMeData.d.Feature.SaveTMPTH, defaTMPTH, sizeof(defaTMPTH));
        MEM_CLR(smSysInfo->d.NVMeData.d.Feature.SaveAutoPST, sizeof(gNvmeIfMgr.AutoPST));
        smSysInfo->d.NVMeData.d.Feature.Tag = SI_FEATURE_TAG;
        smSysInfo->d.NVMeData.d.Feature.SaveHeavyTMPTH = TT_DEGREE_MT_2 + CEL2KELVIN_CONVERTION;
        smSysInfo->d.NVMeData.d.Feature.SaveLightTMPTH = TT_DEGREE_MT_1 + CEL2KELVIN_CONVERTION;

        D_PRINTF("[NVMe] SetFeatureInit\n");
    }

    #if CO_SUPPORT_FEATURES_SELECT_SAVE
    MEM_COPY(currentFeatureValue, smSysInfo->d.NVMeData.d.Feature.SaveFeature, sizeof(currentFeatureValue));
    #endif

    gNvmeIfMgr.writeCacheEnable  = (uint16_t)currentFeatureValue[cVolatileWriteCache];
    gNvmeIfMgr.errRecovery.DULBE = ((Dw11ErrorRecovery_t*) (&currentFeatureValue[cErrorRecovery]))->DULBE;
    gNvmeIfMgr.errRecovery.TLER  = ((Dw11ErrorRecovery_t*) (&currentFeatureValue[cErrorRecovery]))->TLER;
    gNvmeIfMgr.AsyncEventEnable  = currentFeatureValue[cAsynchronousEventConfiguration];

    MEM_COPY(gNvmeIfMgr.currTMPTH, defaTMPTH, sizeof(defaTMPTH));

    gNvmeIfMgr.currHeavyTMPTH = TT_DEGREE_MT_2 + CEL2KELVIN_CONVERTION;
    gNvmeIfMgr.currLightTMPTH = TT_DEGREE_MT_1 + CEL2KELVIN_CONVERTION;

    //currentFeatureValue[cPowerManagement] = 0;

    gNvmeIfMgr.PM_OperaState  = 0;
    if (tPSD[currentFeatureValue[cPowerManagement]].NOPS == 0)  gNvmeIfMgr.PM_OperaState = currentFeatureValue[cPowerManagement];

    #if (CO_SUPPORT_APST == TRUE)
    MEM_COPY(gNvmeIfMgr.AutoPST, defaAPST, sizeof(defaAPST));
    for (i = 0; i < NVME_NUM_OF_POWER_STATES; i++)
    {
        gPmInfo.apstTimerTbl[defaAPST[i].PS] = defaAPST[i].IdleMs;
    }
    #endif
}

//-----------------------------------------------------------------------------
/**
    Set Features - Admin 09h

    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_SetFeatures(HcmdQ_t* pHcmdQ)
{
    NvmeAdminSetFeaturesCommand_t* pCdb;
    NvmeSdb_t* pSdb;

    pCdb = (NvmeAdminSetFeaturesCommand_t *)NvmeCmd_GetCdbPointer(pHcmdQ);
    pSdb = (NvmeSdb_t *)NvmeCmd_GetSdbPointer(pHcmdQ);

    BEGIN_HOST_DBG_DUMP(DBG_DUMP_HOST_NONRW)
        D_PRINTF("%s FID=%2X SV=%1X DW11=%X\n", __FUNCTION__, pCdb->FID, pCdb->SV, pCdb->DW11);
    END_HOST_DBG_DUMP

#if !CO_SUPPORT_FEATURES_SELECT_SAVE
    if (pCdb->SV)
    {
        pHcmdQ->error = MK_NVME_ERROR_CODE(cFeatureIdentifierNotSaveable, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
        gNvmeIfMgr.lastErrorBitOffset = 7;
        gNvmeIfMgr.lastErrorByteOffset = ERR_DW10_OFFSET + 3; // DW10[31], SV
        return;
    }
#endif

    switch(pCdb->FID)
    {
        case cArbitration:
            pHcmdQ->error = HalNvme_SetCommandArbitration( ((Dw11Arbitration_t*) (&pCdb->DW11))->AB,
                                                           ((Dw11Arbitration_t*) (&pCdb->DW11))->LPW,
                                                           ((Dw11Arbitration_t*) (&pCdb->DW11))->MPW,
                                                           ((Dw11Arbitration_t*) (&pCdb->DW11))->HPW);
            break;

        case cPowerManagement:
            // TODO: Add code here to support this feature set
            pHcmdQ->error = SetPowerManagement(pCdb);
            break;

    #if Feature_03_LBARange
        case cLbaRangeType:
            // TODO: Add code here to support this feature set
            break;
    #endif

        case cTemperatureThreshold:
            pHcmdQ->error = SetTemperatureThreshold(pCdb);
            break;

        case cErrorRecovery:
            currentFeatureValue[cErrorRecovery] = pCdb->DW11;
            gNvmeIfMgr.errRecovery.DULBE = ((Dw11ErrorRecovery_t*) (&pCdb->DW11))->DULBE;
            gNvmeIfMgr.errRecovery.TLER  = ((Dw11ErrorRecovery_t*) (&pCdb->DW11))->TLER;
            break;

        case cVolatileWriteCache:
            currentFeatureValue[cVolatileWriteCache] = ((Dw11VolatileWriteCache_t*) (&pCdb->DW11))->WCE;
            gNvmeIfMgr.writeCacheEnable = currentFeatureValue[cVolatileWriteCache];
            break;

        case cNumberOfQueues:
            pHcmdQ->error = AllocQueues((Dw11NumberOfQueues_t*) (&pCdb->DW11) , (Dw0NumberOfQueues_t*) (&pSdb->DW0));
            break;

        case cInterruptCoalescing:
            pHcmdQ->error = HalNvme_SetInterruptCoalescing( ((Dw11InterruptCoalescing_t*) (&pCdb->DW11))->TIME * 100,
                                                                ((Dw11InterruptCoalescing_t*) (&pCdb->DW11))->THR);
            break;

        case cInterruptVectorConfiguration:
            pHcmdQ->error = HalNvme_SetInterruptConfiguration( ((Dw11InterruptConfiguration_t*) (&pCdb->DW11))->IV,
                                                               ((Dw11InterruptConfiguration_t*) (&pCdb->DW11))->CD);
            break;

        case cWriteAtomicity:
             // TODO: Add code here to support this feature set
           break;

        case cAsynchronousEventConfiguration:
            gNvmeIfMgr.AsyncEventEnable = (pCdb->DW11 & 0x031F);
            break;
        case cAutonomousPowerStateTransition:
            SetAutonomousPowerStateTransition(pHcmdQ);
            break;
        #if ((_CUSTOMER_ == CUSTOMER_DELL) || (_CUSTOMER_ == CUSTOMER_LENOVO))
        case cManagementTemperature:
            SetManagementTemp(pHcmdQ);
            return;
        #endif

        default:
            pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
            gNvmeIfMgr.lastErrorBitOffset = 0;
            gNvmeIfMgr.lastErrorByteOffset = ERR_DW10_OFFSET; // DW10, FID
            break;
    }

    if (pHcmdQ->error == cEcNoError)
    {
        if (pCdb->FID != cAutonomousPowerStateTransition)  // APST is complicated, APST is done in SetAutonomousPowerStateTransition()
        {
            currentFeatureValue[pCdb->FID] = pCdb->DW11;
        }

    #if CO_SUPPORT_FEATURES_SELECT_SAVE
        if (pCdb->SV)
        {
            smSysInfo->d.NVMeData.d.Feature.SaveFeature[pCdb->FID] = currentFeatureValue[pCdb->FID];
            SyIn_Synchronize(SI_AREA_NVME, SYSINFO_WRITE, SI_SYNC_BY_NVME);
        }
    #endif
    }
}

//-----------------------------------------------------------------------------
/**
    TemperatureThreshold

**/
//-----------------------------------------------------------------------------
static Error_t SetPowerManagement(NvmeAdminSetFeaturesCommand_t* pCdb)
{
    Error_t error;
    uint8_t PS;

    error = cEcNoError;

    PS = ((Dw11PowerManagement_t*) (&pCdb->DW11))->PS;

    if (PS > (NVME_NUM_OF_POWER_STATES - 1))
    {
        error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
        gNvmeIfMgr.lastErrorBitOffset  = 0;
        gNvmeIfMgr.lastErrorByteOffset = ERR_DW11_OFFSET + 2; // DW11, PS
    }
    else
    {
        gNvmeIfMgr.PM_OperaState = PS;

        if (tPSD[PS].NOPS == 0)
        {
            smThermalTempUpdate = TRUE;     // to change clock immediately
        }
    }
    return(error);
}

//-----------------------------------------------------------------------------
/**
    TemperatureThreshold

    @param[in]
**/
//-----------------------------------------------------------------------------
static Error_t SetTemperatureThreshold(NvmeAdminSetFeaturesCommand_t* pCdb)
{
    Error_t error;
    Dw11TemperatureThreshold_t* pTemp;
    uint8_t Sensor;

    error = cEcNoError;
    pTemp = (Dw11TemperatureThreshold_t*)(&pCdb->DW11);
    Sensor = pTemp->TMPSEL;

    if ((Sensor > 0x01) && (Sensor != 0x0F))
    {
        error = MK_NVME_ERROR_CODE(cFeatureNotChangeable, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
        gNvmeIfMgr.lastErrorBitOffset = 0;
        gNvmeIfMgr.lastErrorByteOffset = ERR_DW11_OFFSET + 2; // DW11, TMPSEL
        return (error);
    }
    if (Sensor == 0x0F)  Sensor = 1;

    gNvmeIfMgr.currTMPTH[Sensor][pTemp->THSEL] = pTemp->TMPTH;

#if CO_SUPPORT_FEATURES_SELECT_SAVE
    if (pCdb->SV)
    {
        smSysInfo->d.NVMeData.d.Feature.SaveTMPTH[Sensor][pTemp->THSEL] = gNvmeIfMgr.currTMPTH[Sensor][pTemp->THSEL];
    }
#endif
    return (error);
}

//-----------------------------------------------------------------------------
/**
    Allocate queues

    @param[in]  pNqn   number of queue requested
    @param[in]  pNqa   number of queue allocated

**/
//-----------------------------------------------------------------------------
static Error_t AllocQueues(Dw11NumberOfQueues_t* pNqn, Dw0NumberOfQueues_t* pNqa)
{
    Error_t error;

    error = cEcNoError;
    #if 0
    if(pNqn->NCQR >= U16_MAX || pNqn->NSQR >= U16_MAX)
    {
        error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
    }
    #endif
    pNqa->NCQA = MIN(HW_MAX_IO_QUEUES-1, pNqn->NCQR);
    pNqa->NSQA = MIN(HW_MAX_IO_QUEUES-1, pNqn->NSQR);

    pNqn->NCQR = pNqa->NCQA;
    pNqn->NSQR = pNqa->NSQA;

    return error;
}

//-----------------------------------------------------------------------------
/**
    AutonomousPowerStateTransition

    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
static void SetAutonomousPowerStateTransition(HcmdQ_t* pHcmdQ)
{
    NvmeAdminSetFeaturesCommand_t* pCdb;
    void*    pFeaBuffer;
    uint32_t    x;

    pCdb       = (NvmeAdminSetFeaturesCommand_t *)NvmeCmd_GetCdbPointer(pHcmdQ);
    pFeaBuffer = (void *)NvmeCmd_GetScratchBufferPointer();
    MEM_CLR(pFeaBuffer, cNvmeScratchBufferSize);

    M_PRINTF("\nAPST:%d\n", pCdb->DW11);

    currentFeatureValue[cAutonomousPowerStateTransition] = 0;

    if (pHcmdQ->error == cEcNoError)
    {
        NvmeApstEntry_t* pApstEntry = NULL;

        pHcmdQ->attr.b.xferDir   = cXferH2d;                  // Receive (H -> D)
        pHcmdQ->bufDesc.ssdIndex = SSD_IF_START;

        AplHost_TransferWholeData(pHcmdQ, pFeaBuffer, APST_BUFFER_SIZE, TRUE);

        if (pHcmdQ->error == cEcNoError)
        {
            pApstEntry = (NvmeApstEntry_t *)pFeaBuffer;
            for (x = 0; x < NVME_NUM_OF_POWER_STATES; x++)
            {
                if (pApstEntry[x].IdleMs)
                {
                    if ((pApstEntry[x].PS >= NVME_NUM_OF_POWER_STATES) || (tPSD[pApstEntry[x].PS].NOPS == 0))
                    {
                        pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
                        gNvmeIfMgr.lastErrorBitOffset = 0;
                        gNvmeIfMgr.lastErrorByteOffset = ERR_DW11_OFFSET; // DW11
                        break;
                    }
                }
            }

            //Autonomous Power State Transition
            //31:08  Idle Time Prior to Transition (ITPT):
            //07:03  Idle Transition Power State (ITPS):
            MEM_COPY(gNvmeIfMgr.AutoPST, pFeaBuffer, APST_BUFFER_SIZE);
            for (x = 0; x < NVME_NUM_OF_POWER_STATES; x++)
            {
                if (gNvmeIfMgr.AutoPST[x].IdleMs)
                {
                    if (gNvmeIfMgr.AutoPST[x].PS == 3)      gNvmeIfMgr.AutoPST[x].IdleMs = 10;
                    else if (gNvmeIfMgr.AutoPST[x].PS == 4) gNvmeIfMgr.AutoPST[x].IdleMs = 20;

                    M_PRINTF("%6d ms -> PS%d\n", gNvmeIfMgr.AutoPST[x].IdleMs, gNvmeIfMgr.AutoPST[x].PS);
                    gPmInfo.apstTimerTbl[gNvmeIfMgr.AutoPST[x].PS] = gNvmeIfMgr.AutoPST[x].IdleMs;
                }
            }

           #if (CO_SUPPORT_APST == TRUE)
            currentFeatureValue[cAutonomousPowerStateTransition] = pCdb->DW11;
           #endif

           #if CO_SUPPORT_FEATURES_SELECT_SAVE
            if (pCdb->SV)
            {
                MEM_COPY(smSysInfo->d.NVMeData.d.Feature.SaveAutoPST, pFeaBuffer, APST_BUFFER_SIZE);
            }
           #endif
        }
    }
}

//-----------------------------------------------------------------------------
/**
    SetManagementTemp

    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
static void SetManagementTemp(HcmdQ_t* pHcmdQ)
{
    NvmeAdminSetFeaturesCommand_t* pCdb;
    NvmeSdb_t* pSdb;
    Dw11ManaTemperature_t* pTemp;
    Msg_t* pMsg;

    pMsg                  = Ipc_GetExtendMsg(cMcHostSetFeature);
    pMsg->hdr.b.subOpCode = cSetFeaManagementTemperature;

    pCdb      = (NvmeAdminSetFeaturesCommand_t *)NvmeCmd_GetCdbPointer(pHcmdQ);
    pSdb      = (NvmeSdb_t *)NvmeCmd_GetSdbPointer(pHcmdQ);
    pTemp     = (Dw11ManaTemperature_t*)(&pCdb->DW11);
    pSdb->DW0 = 0;


#if (_CUSTOMER_ == CUSTOMER_DELL)
    if (pTemp->Heavy < (TT_DEGREE_MT_2 + CEL2KELVIN_CONVERTION))  // MT2
    {
        gNvmeIfMgr.currHeavyTMPTH = pTemp->Heavy;
        pMsg->param[0]            = pTemp->Heavy - CEL2KELVIN_CONVERTION;
        pSdb->DW0                |= B_1;
    }
    if (pTemp->Light < (TT_DEGREE_MT_1 + CEL2KELVIN_CONVERTION))  // MT1
    {
        gNvmeIfMgr.currLightTMPTH = pTemp->Light;
        pMsg->param[1]            = pTemp->Light - CEL2KELVIN_CONVERTION;
        pSdb->DW0                |= B_0;
    }
    if (pSdb->DW0 == 0) return;

#else  // Lenovo
    gNvmeIfMgr.currHeavyTMPTH = pTemp->Heavy;
    gNvmeIfMgr.currLightTMPTH = pTemp->Light;
    pMsg->param[0]            = pTemp->Heavy - CEL2KELVIN_CONVERTION;
    pMsg->param[1]            = pTemp->Light - CEL2KELVIN_CONVERTION;
#endif

    IPC_SendMsgQ(cH2cReqQueue, (Msg_t*) pMsg);

    // No need to wait down
    //while (pMsg->hdr.b.status != cMsgCompleted);

    #if CO_SUPPORT_FEATURES_SELECT_SAVE
    if (pCdb->SV)
    {
        smSysInfo->d.NVMeData.d.Feature.SaveHeavyTMPTH = gNvmeIfMgr.currHeavyTMPTH;
        smSysInfo->d.NVMeData.d.Feature.SaveLightTMPTH = gNvmeIfMgr.currLightTMPTH;
        SyIn_Synchronize(SI_AREA_NVME, SYSINFO_WRITE, SI_SYNC_BY_NVME);
    }
    #endif
}
