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
#include "Cache.h"

// Code Section
#include "NormSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
/// @brief Get Features - Select
typedef enum
{
    cCurrent = 0,
    cDefault,
    cSaved,
    cSupprotedCapabilities,
} GetFeaturesSelect_t;


//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief NVMe Admin Command structure - Get Features
typedef  struct
{
    AdminCommandCommon_t DW9_0;    ///< DW9..0

    uint32_t FID:8;           ///< Feature Identifier (DW10 bits[7:0])
    uint32_t SEL:3;           ///< Select (DW10 bits[10:8])
    uint32_t reserved10:21;   ///< Reserved (DW10 bits[31:11])

    uint32_t DW11;            ///< DW11 Feature-specific

    uint32_t reserved12[4];   ///< DW15..12
} NvmeAdminGetFeaturesCommand_t;
COMPILE_ASSERT(sizeof(NvmeAdminGetFeaturesCommand_t)==64, "NVMe command size shall be 64");

/// @brief NVMe Admin Get Features - Select (SEL) - Supported capabilities CQ Dword0
typedef union
{
    uint32_t all;
    struct
    {
        uint32_t savable:1;            ///< indicates Feature Identifier is saveable bits[00]
        uint32_t namespaceSpecific:1;  ///< indicates Feature Identifier is namespace specific bits[01]
        uint32_t changeable:1;         ///< indicates Feature Identifier is changeable bits[02]
        uint32_t reserved3:29;         ///< Reserved bits[31:03]
    } b;
} GetFeaturesCapabilitiesCqDw0_t;
COMPILE_ASSERT(sizeof(GetFeaturesCapabilitiesCqDw0_t) == 4, "GetFeatures DW0 size shall be 4");

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
static Error_t GetCurrentValue(uint32_t fid, NvmeSdb_t* pSdb, NvmeAdminGetFeaturesCommand_t* pCdb);
static Error_t GetDefaultValue(uint32_t fid, NvmeSdb_t* pSdb, NvmeAdminGetFeaturesCommand_t* pCdb);
static Error_t GetSavedValue(uint32_t fid, NvmeSdb_t* pSdb, NvmeAdminGetFeaturesCommand_t* pCdb);
static Error_t GetSupportedCapabilities(uint32_t fid, NvmeSdb_t* pSdb);
static Error_t GetTempThreshold(NvmeSdb_t* pSdb, NvmeAdminGetFeaturesCommand_t* pCdb);
static void GetAutonomousPowerState(HcmdQ_t* pHcmdQ);
static void GetManagementTemp(HcmdQ_t* pHcmdQ);

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
    Get Features - Admin 10h

    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_GetFeatures(HcmdQ_t* pHcmdQ)
{
    NvmeAdminGetFeaturesCommand_t* pCdb;
    NvmeSdb_t* pSdb;
    uint32_t fid;

    pCdb = (NvmeAdminGetFeaturesCommand_t *)NvmeCmd_GetCdbPointer(pHcmdQ);
    pSdb = (NvmeSdb_t *)NvmeCmd_GetSdbPointer(pHcmdQ);

    BEGIN_HOST_DBG_DUMP(DBG_DUMP_HOST_NONRW)
        D_PRINTF("%s FID=%2X SEL=%1X\n", __FUNCTION__, pCdb->FID, pCdb->SEL);
    END_HOST_DBG_DUMP

    fid = pCdb->FID;

#if Feature_03_LBARange == DISABLE
    if (fid == cLbaRangeType) fid = 0;
#endif

#if (CO_SUPPORT_APST == FALSE)
    if (fid == cAutonomousPowerStateTransition) fid = 0;
#endif

    #if ((_CUSTOMER_ == CUSTOMER_DELL) || (_CUSTOMER_ == CUSTOMER_LENOVO))
    if (fid == cManagementTemperature)
    {
        GetManagementTemp(pHcmdQ);
        return;
    }
    #endif

    // check invalid FID
    if (fid==0 || cLastSupportedFid<fid)
    {
        pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
        gNvmeIfMgr.lastErrorBitOffset = 0;
        gNvmeIfMgr.lastErrorByteOffset = ERR_DW10_OFFSET; // DW10, FID
        return;
    }

    switch(pCdb->SEL)
    {
        case cCurrent:
            pHcmdQ->error = GetCurrentValue(fid, pSdb, pCdb);
            break;
        case cDefault:
            pHcmdQ->error = GetDefaultValue(fid, pSdb, pCdb);
            break;
        case cSaved:
            #if CO_SUPPORT_FEATURES_SELECT_SAVE
            pHcmdQ->error = GetSavedValue(fid, pSdb, pCdb);
            #else
            pHcmdQ->error = GetDefaultValue(fid, pSdb, pCdb);
            #endif
            break;
        case cSupprotedCapabilities:
            pHcmdQ->error = GetSupportedCapabilities(fid, pSdb);
            break;
        default:
            pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
            gNvmeIfMgr.lastErrorBitOffset = 0;
            gNvmeIfMgr.lastErrorByteOffset = ERR_DW10_OFFSET + 1; // DW10, SEL
            break;
    }

    if (pHcmdQ->error == cEcNoError)
    {
        if (fid == cTemperatureThreshold)
        {
            pHcmdQ->error = GetTempThreshold(pSdb, pCdb);
        }
        else if (fid == cAutonomousPowerStateTransition)
        {
            GetAutonomousPowerState(pHcmdQ);
        }
    }
    D_PRINTF("pCdb->DW11 = %X, pSdb->DW0 = %X\n", pCdb->DW11, pSdb->DW0);
}

//-----------------------------------------------------------------------------
/**
    Get Features - Current Value

    @param[in]  fid     feature Id
    @param[out] pSdb    pointer to NVMe SDB
**/
//-----------------------------------------------------------------------------
static Error_t GetCurrentValue(uint32_t fid, NvmeSdb_t* pSdb, NvmeAdminGetFeaturesCommand_t* pCdb)
{
    pSdb->DW0 = currentFeatureValue[fid];

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Get Features - Default Value

    @param[in]  fid     feature Id
    @param[out] pSdb    pointer to NVMe SDB
**/
//-----------------------------------------------------------------------------
static Error_t GetDefaultValue(uint32_t fid, NvmeSdb_t* pSdb, NvmeAdminGetFeaturesCommand_t* pCdb)
{
    pSdb->DW0 = defaultFeatureValue[fid];

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Get Features - Saved Value

    @param[in]  fid     feature Id
    @param[out] pSdb    pointer to NVMe SDB
**/
//-----------------------------------------------------------------------------
static Error_t GetSavedValue(uint32_t fid, NvmeSdb_t* pSdb, NvmeAdminGetFeaturesCommand_t* pCdb)
{
    pSdb->DW0 = smSysInfo->d.NVMeData.d.Feature.SaveFeature[fid];

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Get Features - Supported Capabilities

    @param[in]  fid     feature Id
    @param[out] pSdb    pointer to NVMe SDB
**/
//-----------------------------------------------------------------------------
static Error_t GetSupportedCapabilities(uint32_t fid, NvmeSdb_t* pSdb)
{
    GetFeaturesCapabilitiesCqDw0_t dw0;

    dw0.all = 0;
    dw0.b.savable = CO_SUPPORT_FEATURES_SELECT_SAVE;
    dw0.b.namespaceSpecific = 0;
    dw0.b.changeable = 1;

    pSdb->DW0 = dw0.all;

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    GetTempThreshold

    @param[in] pSdb    pointer to NVMe SDB
**/
//-----------------------------------------------------------------------------
static Error_t GetTempThreshold(NvmeSdb_t* pSdb, NvmeAdminGetFeaturesCommand_t* pCdb)
{
    Error_t error;
    Dw11TemperatureThreshold_t* pTemp;

    pTemp = (Dw11TemperatureThreshold_t*)(&pCdb->DW11);

    if (pTemp->TMPSEL > 0x01)
    {
        error = MK_NVME_ERROR_CODE(cFeatureNotChangeable, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
        gNvmeIfMgr.lastErrorBitOffset = 0;
        gNvmeIfMgr.lastErrorByteOffset = ERR_DW11_OFFSET + 2; // DW11, TMPSEL
        return (error);
    }

    pSdb->DW0 &= 0xffff0000;

    switch(pCdb->SEL)
    {
        case cCurrent:
            pSdb->DW0 |= gNvmeIfMgr.currTMPTH[pTemp->TMPSEL][pTemp->THSEL];
            break;

        case cDefault:
            pSdb->DW0 |= defaTMPTH[pTemp->TMPSEL][pTemp->THSEL];
            break;

        case cSaved:
            #if CO_SUPPORT_FEATURES_SELECT_SAVE
            pSdb->DW0 |= smSysInfo->d.NVMeData.d.Feature.SaveTMPTH[pTemp->TMPSEL][pTemp->THSEL];
            #else
            pSdb->DW0 |= defaTMPTH[pTemp->TMPSEL][pTemp->THSEL];
            #endif
            break;

        case cSupprotedCapabilities:
            GetSupportedCapabilities(cTemperatureThreshold, pSdb);
            break;

        default:
            break;

    }

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    GetAutonomousPowerState

    @param[in]  pHcmdQ
**/
//-----------------------------------------------------------------------------
static void GetAutonomousPowerState(HcmdQ_t* pHcmdQ)
{
    NvmeAdminGetFeaturesCommand_t* pCdb;
    uint32_t transferSize;
    void* pFeaBuffer;

    pCdb = (NvmeAdminGetFeaturesCommand_t *)NvmeCmd_GetCdbPointer(pHcmdQ);

    transferSize = 256;
    pFeaBuffer = (void *)NvmeCmd_GetScratchBufferPointer();
    MEM_CLR(pFeaBuffer, cNvmeScratchBufferSize);

    //D_PRINTF("TransferData --   cAutonomousPowerStateTransition\n");

    if (pCdb->SEL == cCurrent)
    {
        MEM_COPY(pFeaBuffer, gNvmeIfMgr.AutoPST, transferSize);

    }
    else if (pCdb->SEL == cSaved)
    {
        MEM_COPY(pFeaBuffer, smSysInfo->d.NVMeData.d.Feature.SaveAutoPST, transferSize);
    }

    pHcmdQ->attr.b.xferDir = cXferD2h;
    pHcmdQ->bufDesc.ssdIndex = SSD_IF_START;
    pHcmdQ->error = AplHost_TransferWholeData(pHcmdQ, pFeaBuffer, transferSize, TRUE);

}

//-----------------------------------------------------------------------------
/**
    GetManaTemperature

    @param[in]  pHcmdQ
**/
//-----------------------------------------------------------------------------
static void GetManagementTemp(HcmdQ_t* pHcmdQ)
{
    NvmeAdminGetFeaturesCommand_t* pCdb;
    NvmeSdb_t* pSdb;
    Dw11ManaTemperature_t* pTemp;

    pCdb = (NvmeAdminGetFeaturesCommand_t *)NvmeCmd_GetCdbPointer(pHcmdQ);
    pSdb = (NvmeSdb_t *)NvmeCmd_GetSdbPointer(pHcmdQ);
    pTemp = (Dw11ManaTemperature_t*)(&pSdb->DW0);


    switch(pCdb->SEL)
    {
        case cCurrent:
            pTemp->Heavy = gNvmeIfMgr.currHeavyTMPTH;
            pTemp->Light = gNvmeIfMgr.currLightTMPTH;
            break;

        case cDefault:
            pTemp->Heavy = TT_DEGREE_MT_2 + CEL2KELVIN_CONVERTION;
            pTemp->Light = TT_DEGREE_MT_1 + CEL2KELVIN_CONVERTION;
            break;

        case cSaved:
            #if CO_SUPPORT_FEATURES_SELECT_SAVE
            pTemp->Heavy = smSysInfo->d.NVMeData.d.Feature.SaveHeavyTMPTH;
            pTemp->Light = smSysInfo->d.NVMeData.d.Feature.SaveLightTMPTH;
            #else
            pTemp->Heavy = TT_DEGREE_MT_2 + CEL2KELVIN_CONVERTION;
            pTemp->Light = TT_DEGREE_MT_1 + CEL2KELVIN_CONVERTION;
            #endif
            break;

        case cSupprotedCapabilities:
            pHcmdQ->error = GetSupportedCapabilities(cManagementTemperature, pSdb);
            break;

        default:
            pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
            gNvmeIfMgr.lastErrorBitOffset = 0;
            gNvmeIfMgr.lastErrorByteOffset = ERR_DW10_OFFSET + 1; // DW10, SEL
            break;
    }
}
