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
#include "NvmeIdentify.h"
#if CO_SUPPORT_DEVICE_SELF_TEST
    #include "NvmeDeviceSelfTest.h"
#endif
#if (CO_SUPPORT_SECURITY) || (_TCG_ != TCG_NONE)
    #include "NvmeSecurityCmd.h"
#endif
#if _TCG_!=TCG_NONE
    #include "TcgCommon.h"
#endif
#include "SysInfo.h"
#include "Cache.h"

// Code Section
#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define MPS_10MW    0
#define MPS_100UW   1

typedef enum
{
    cIdentifyNamespace           = 0x00,
    cIdentifyControllerData      = 0x01,
    cIdentifyActiveNamespaceIds  = 0x02,

    //Namespace Management
    cIdentifyContainNamespaceIds = 0x10,
    cIdentifyContainNamespace    = 0x11,
    cIdentifyControlListAttach   = 0x12,
    cIdentifyControlList         = 0x13,
} NvmeIdentifyCns_t;

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief NVMe Admin Command structure - Identify
typedef  struct
{
    AdminCommandCommon_t DW9_0; ///< DW9..0

    uint32_t CNS:8;             ///< Controller or Namespace Structure (DW10 bits[7:0])
    uint32_t reserved10:8;      ///< Reserved (DW10 bits[15:8])
    uint32_t CNTID:16;          ///< Controller Identifier (DW10 bits[31:16])

    uint32_t reserved11[5];     ///< DW15..11

} NvmeAdminIdentifyCommand_t;
COMPILE_ASSERT(sizeof(NvmeAdminIdentifyCommand_t) == 64, "NVMe command size shall be 64");

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static Error_t IdentifyNamespace(NvmeIdentifyNamespace_t* pIdfy, uint32_t nsid, uint32_t* transferSize);
static Error_t IdentifyControllerData(NvmeIdentifyController_t* pIdfy, uint32_t* transferSize);
static Error_t IdentifyActiveNamespaceIds(NvmeIdentifyActiveNamespaceIds_t* pIdfy, uint32_t nsid, uint32_t* transferSize);

//Namespace Management
static Error_t IdentifyContainNamespaceIds(NvmeIdentifyContainNamespaceIds_t* pIdfy, uint32_t nsid, uint32_t* transferSize);
static Error_t IdentifyContainNamespace(NvmeIdentifyNamespace_t* pIdfy, uint32_t nsid, uint32_t* transferSize);
static Error_t IdentifyControlListAttach(NvmeIdentifyControlList_t* pIdfy, uint32_t nsid,  uint32_t cntid, uint32_t* transferSize);
static Error_t IdentifyControlList(NvmeIdentifyControlList_t* pIdfy, uint32_t nsid, uint32_t cntid, uint32_t* transferSize);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
const PowerStateDescriptor_t tPSD[NVME_NUM_OF_POWER_STATES] = {    // NVME_NUM_OF_POWER_STATES maximum is 5
    {800 , 0, MPS_10MW,  0,  0, PS_0_ENTER_US, PS_0_EXIT_US, 0, 0, 0, 0, 0, 0, 0},
#if (CO_SUPPORT_APST == TRUE)
    {400 , 0, MPS_10MW,  0,  0, PS_1_ENTER_US, PS_1_EXIT_US, 1, 0, 1, 0, 1, 0, 1},
    {210 , 0, MPS_10MW,  0,  0, PS_2_ENTER_US, PS_2_EXIT_US, 2, 0, 2, 0, 2, 0, 2},
    {1000, 0, MPS_100UW, 1,  0, PS_3_ENTER_US, PS_3_EXIT_US, 3, 0, 3, 0, 3, 0, 3},
    {100 , 0, MPS_100UW, 1,  0, PS_4_ENTER_US, PS_4_EXIT_US, 4, 0, 4, 0, 4, 0, 4}
#endif
};

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
    Identify command handler - Admin 06h

    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_Identify(HcmdQ_t* pHcmdQ)
{
    uint32_t transferSize;
    NvmeAdminIdentifyCommand_t* pCdb;
    void* pIdfy;

    pCdb  = (NvmeAdminIdentifyCommand_t *)NvmeCmd_GetCdbPointer(pHcmdQ);
    pIdfy = (void *)NvmeCmd_GetScratchBufferPointer();
    MEM_CLR(pIdfy, cNvmeScratchBufferSize);

    BEGIN_HOST_DBG_DUMP(DBG_DUMP_HOST_NONRW)
        D_PRINTF("%s CNS=%2X NSID:%X\n", __FUNCTION__, pCdb->CNS, pCdb->DW9_0.NSID);
    END_HOST_DBG_DUMP

    transferSize = 0;

    switch (pCdb->CNS)
    {
        case cIdentifyNamespace:
            pHcmdQ->error = IdentifyNamespace((NvmeIdentifyNamespace_t *)pIdfy, pCdb->DW9_0.NSID, &transferSize);
            break;
        case cIdentifyControllerData:
            pHcmdQ->error = IdentifyControllerData((NvmeIdentifyController_t *)pIdfy, &transferSize);
            break;
        case cIdentifyActiveNamespaceIds:
            pHcmdQ->error = IdentifyActiveNamespaceIds((NvmeIdentifyActiveNamespaceIds_t *)pIdfy, pCdb->DW9_0.NSID, &transferSize);
            break;
    #if CO_SUPPORT_NAMESPACE_MANAGEMENT
        case cIdentifyContainNamespaceIds:
            pHcmdQ->error = IdentifyContainNamespaceIds((NvmeIdentifyContainNamespaceIds_t *)pIdfy, pCdb->DW9_0.NSID, &transferSize);
            break;
        case cIdentifyContainNamespace:
            pHcmdQ->error = IdentifyContainNamespace((NvmeIdentifyNamespace_t *)pIdfy, pCdb->DW9_0.NSID, &transferSize);
            break;
        case cIdentifyControlListAttach:
            pHcmdQ->error = IdentifyControlListAttach((NvmeIdentifyControlList_t *)pIdfy, pCdb->DW9_0.NSID, pCdb->CNTID, &transferSize);
            break;
        case cIdentifyControlList:
            pHcmdQ->error = IdentifyControlList((NvmeIdentifyControlList_t *)pIdfy, pCdb->DW9_0.NSID, pCdb->CNTID, &transferSize);
            break;
    #endif
        default:
            pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
            gNvmeIfMgr.lastErrorBitOffset  = 0;       // bit0
            gNvmeIfMgr.lastErrorByteOffset = ERR_DW10_OFFSET;  // DW10, CNS
    }

    if (pHcmdQ->error == cEcNoError)
    {
        pHcmdQ->bufDesc.ssdIndex = SSD_IF_START;
        AplHost_TransferWholeData(pHcmdQ, pIdfy, transferSize, TRUE);
    }

    #if (CHK_FW_TYPE(FW_AGING))
    if(smSysInfo->d.AgingData.d.Aging_Mode == ENABLE || smSysInfo->d.AgingData.d.Aging_Signature == 0x4E494741)
    {
        D_PRINTF("Aging_Identify[%x]\n",smSysInfo->d.AgingData.d.Aging_Identify);

        smSysInfo->d.AgingData.d.Aging_Identify = ENABLE;
    }
    #endif
}

//-----------------------------------------------------------------------------
/**
    Identify - Namespace

    @param[in]  pIdfy   identify namespace pointer
    @param[in]  nsid    namespace id (one based number)

    @return transfer size in byte count, zero if any error
**/
//-----------------------------------------------------------------------------
static Error_t IdentifyNamespace(NvmeIdentifyNamespace_t* pIdfy, uint32_t nsid, uint32_t* transferSize)
{
    uint32_t lbafIndex;
    uint32_t nsid0;

    nsid0 = nsid - 1; // convert to zero based id

    if (nsid != cNvmeGlobalNamespaceId)
    {
        if ((nsid0 >= NVME_NUM_OF_NAMESPACES) || (!gNvmeIfMgr.ns[nsid0].Create))
        {
            gNvmeIfMgr.lastErrorBitOffset  = 0;
            gNvmeIfMgr.lastErrorByteOffset = ERR_DW1_OFFSET; // DW1, Namespaces
            return MK_NVME_ERROR_CODE(cInvalidNamespaceOrFormat, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
        }

        if (!gNvmeIfMgr.ns[nsid0].Attach)
        {
            *transferSize = sizeof(NvmeIdentifyNamespace_t);
            return cEcNoError;
        }
    }

    if (nsid == cNvmeGlobalNamespaceId)
    {
        pIdfy->NUSE                        = 0;
        pIdfy->NLBAF                       = NVME_NUM_OF_LBAF - 1;
        pIdfy->FLBAS_METADATA_LBA_END      = 1;
        pIdfy->FLBAS_SUPPORTED_COMBINATION = 0;

        // Meta data capabilities
        #if CO_SUPPORT_DATA_PROTECTION
        pIdfy->MC                         = 1;
        pIdfy->DPC_INFO_TYPE1             = 1;
        pIdfy->DPC_INFO_FIRST_EIGHT_BYTES = 0;
        pIdfy->DPC_INFO_LAST_EIGHT_BYTES  = 1;
        pIdfy->PI  = 0;
        pIdfy->PIL = 0;
        #endif
    }
    else
    {
        pIdfy->NUSE                        = 0;
        pIdfy->NLBAF                       = NVME_NUM_OF_LBAF - 1;           // Number of lba formats, 0 based
        pIdfy->FLBAS_METADATA_LBA_END      = gNvmeIfMgr.ns[nsid0].MS;
        pIdfy->FLBAS_SUPPORTED_COMBINATION = gNvmeIfMgr.ns[nsid0].LBAF;

        // Meta data capabilities
        #if CO_SUPPORT_DATA_PROTECTION
        pIdfy->MC                         = 1;                               // Support a seperate buffer and an extended data LBA
        pIdfy->DPC_INFO_TYPE1             = 1;
        pIdfy->DPC_INFO_FIRST_EIGHT_BYTES = 0;
        pIdfy->DPC_INFO_LAST_EIGHT_BYTES  = 1;
        pIdfy->PI  = gNvmeIfMgr.ns[nsid0].PI;
        pIdfy->PIL = gNvmeIfMgr.ns[nsid0].PIL;
        #endif

        // size and capacity
        pIdfy->NSZE = gNvmeIfMgr.ns[nsid0].NSZE;
        pIdfy->NCAP = gNvmeIfMgr.ns[nsid0].NCAP;
    }

    #if CO_SUPPORT_NAMESPACE_MANAGEMENT
    pIdfy->NVMCAP[0] = (uint64_t)(gNvmeIfMgr.ns[nsid0].NSZE) << (9 + gNvmeIfMgr.ns[nsid0].hbu_shift);
    #endif

    MEM_COPY(&pIdfy->EUI64, &smSysInfo->d.MPInfo.d.WorldWideName[0], sizeof(pIdfy->EUI64));

    // LBA format example
    for (lbafIndex = 0; lbafIndex < NVME_NUM_OF_LBAF; lbafIndex++)
    {
        // Each name space can use four lba formats supported by Data Manager.
        pIdfy->LBAF[lbafIndex].LBADS = LOG2(cLBAFTable[lbafIndex].dataSize);
        pIdfy->LBAF[lbafIndex].MS    = cLBAFTable[lbafIndex].metaSize;
        pIdfy->LBAF[lbafIndex].RP    = cLBAFTable[lbafIndex].rp;
    }

    *transferSize = sizeof(NvmeIdentifyNamespace_t);

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Identify - Controller data

    @param[in]  pIdfy   identify controller data pointer

    @return transfer size in byte count, zero if any error
**/
//-----------------------------------------------------------------------------
static Error_t IdentifyControllerData(NvmeIdentifyController_t* pIdfy, uint32_t* transferSize)
{
#if _TCG_!=TCG_NONE
    bool bCryptoErase = TRUE;
#endif
    uint8_t PwState;

    pIdfy->VID   = PCI_VENDOR_ID;
    pIdfy->SSVID = PCI_SUBSYS_VENDOR_ID;

    MEM_COPY(pIdfy->SN, &smSysInfo->d.MPInfo.d.SerialNumberDrive[0], sizeof(pIdfy->SN)); // needs space character padding

    //MEM_SET(pIdfy->MN, 0x20, sizeof(pIdfy->MN));
    //MEM_SET(pIdfy->FR, 0x20, sizeof(pIdfy->FR));
    MEM_COPY(pIdfy->MN, &LiteOnModelName[0],  sizeof(pIdfy->MN));
    MEM_COPY(pIdfy->FR, &LiteOnFWRevision[0], sizeof(pIdfy->FR));

    // handle 0x7A: 'z'
    if ((pIdfy->SN[0] < 0x20) || (pIdfy->SN[0] > 0x7A)) MEM_SET(pIdfy->SN, 0x20, sizeof(pIdfy->SN));

    pIdfy->RAB = 0;
    pIdfy->IEEE = NVME_IEEE_OUI_ID_LITEON;
    pIdfy->MDTS = LOG2(NVME_MAX_DATA_TRANSFER_SIZE) - 12;  // assume MPSMIN is 4KB(2^12)
    pIdfy->CNTLID = 1;  //NVME_PCIE_ID;

    pIdfy->VER = NVME_VER;
    pIdfy->RTD3R = NVME_RTD3R;
    pIdfy->RTD3E = NVME_RTD3E;

    // Controller identify CACS
    pIdfy->OACS_CTL_SECURITY_SEND_RECEIVE_SUPPORT   = CO_SUPPORT_SECURITY;
    pIdfy->OACS_CTL_FORMAT_NMV_SUPPORT              = CO_SUPPORT_FORMAT;
    pIdfy->OACS_CTL_FIRMWARE_DOWNLOAD_SUPPORT       = CO_SUPPORT_FW_DOWNLOAD;
    pIdfy->OACS_CTL_NAMESPACE                       = CO_SUPPORT_NAMESPACE_MANAGEMENT;
    pIdfy->OACS_CTL_DEVICE_SELF_TEST                = CO_SUPPORT_DEVICE_SELF_TEST;
    pIdfy->OACS_CTL_DIRECTIVES                      = CO_SUPPORT_DIRECTIVES;

    pIdfy->ACL                                      = NVME_ABORT_COMMAND_LIMIT - 1;    // 4 Abort Command Limit
    pIdfy->AERL                                     = NVME_ASYNC_EVENT_LIMIT - 1;      // 4 Asynchronous Event Request Limit
    pIdfy->FRMW_NUM_OF_DEVICE_SUPPORTED             = NVME_NUM_OF_FW_SLOTS;
    pIdfy->FRMW_WITHOUT_RESET_SUPPORTED             = 1;
    pIdfy->ELPE                                     = NVME_ERROR_LOG_ENTIRIES - 1;     // Error Log Page Entries
    pIdfy->NPSS                                     = NVME_NUM_OF_POWER_STATES - 1;    // 4 Power States Support

#if CO_SUPPORT_DEVICE_SELF_TEST
    pIdfy->EDSTT = ExtenDSTTime / 60;
    pIdfy->DSTO = 1;
#endif

    pIdfy->SQES_REQUIRED = 6;  // 64(2^6) bytes submission queue entry size
    pIdfy->SQES_MAXIMUM  = 6;  //
    pIdfy->CQES_REQUIRED = 4;  // 16(2^4) bytes completion queue entry size
    pIdfy->CQES_MAXIMUM  = 4;  //

    pIdfy->NN = gNvmeIfMgr.numOfNamespacesCreated;

    pIdfy->ONCS_COMPARE_SUPPORT              = CO_SUPPORT_COMPARE;
    pIdfy->ONCS_WRITE_UNCORRECTABLE_SUPPORT  = CO_SUPPORT_WRITE_UNCORRECTABLE;
    pIdfy->ONCS_DATASET_MANAGEMENT_SUPPORT   = CO_SUPPORT_DATASET_MANAGEMENT;
    pIdfy->ONCS_WRITE_ZEROES_SUPPORT         = CO_SUPPORT_WRITE_ZEROES;
    pIdfy->ONCS_FEATURES_SELECT_SAVE_SUPPORT = CO_SUPPORT_FEATURES_SELECT_SAVE;
    pIdfy->ONCS_RESERVATIONS_SUPPORT         = CO_SUPPORT_RESERVATIONS;

#if CO_SUPPORT_ATA_SECURITY
    if (smSysInfo->d.NVMeData.d.SCU_CMD.PwEnable == TRUE)
    {
        pIdfy->FNA_ERASE_TO_ALL_NAMESPACES = FALSE;
  #if _TCG_==TCG_NONE
        pIdfy->FNA_CRYPTOGRAPHIC_ERASE_SUPPORT = FALSE;
  #else
        bCryptoErase = FALSE;
  #endif
    }
#endif
#if _TCG_!=TCG_NONE
    if(mTcgStatus&TCG_ACTIVATED)
        bCryptoErase = FALSE;

    pIdfy->FNA_CRYPTOGRAPHIC_ERASE_SUPPORT = bCryptoErase;
#endif

    pIdfy->VWC = CO_SUPPORT_VOLATILE_WRITE_CACHE;
    pIdfy->AWUN = KBYTE2LBU(128) - 1; // 128KB Atomic Write Unit Normal
    pIdfy->AWUPF = KBYTE2LBU(4) - 1;  // 4KB Atomic Write Unit Power Fail

    pIdfy->LPA_SUP_SMART            = CO_SUPPORT_LOGPAGE_PER_NAMESPACE;
    pIdfy->LPA_SUP_COMMAND_EFFECTS  = CO_SUPPORT_LOGPAGE_COMMAND_EFFECTS;

    //IDFY 264 BIT(0)
    pIdfy->AVSCC  = CO_SUPPORT_VENDORCMD;

    pIdfy->APSTA  = CO_SUPPORT_APST;

    pIdfy->WCTEMP = NVME_WARNING_TEMPERATURE;
    pIdfy->CCTEMP = NVME_CRITICAL_TEMPERATURE;

    pIdfy->MTFA   = NVME_MTFA_LIMIT;

#if CO_SUPPORT_NAMESPACE_MANAGEMENT
    uint8_t nameSpecNum;
    for (nameSpecNum = 0; nameSpecNum < NVME_NUM_OF_NAMESPACES; nameSpecNum++)
    {
        if (gNvmeIfMgr.ns[nameSpecNum].Create)
        {
            pIdfy->TNVMCAP[0] += (uint64_t)gNvmeIfMgr.ns[nameSpecNum].NSZE << (9 + gNvmeIfMgr.ns[nameSpecNum].hbu_shift);  // Total NVM Capacity
        }
    }
    pIdfy->UNVMCAP[0] = ((uint64_t)IDEMA_GBYTE2LBU(gNfInfo.userCapacity) << 9) - pIdfy->TNVMCAP[0];
#endif

    pIdfy->NVSCC = NVME_NVSCC_USE_OPTION;

    for (PwState=0 ; PwState <NVME_NUM_OF_POWER_STATES ;PwState++)
    {
        pIdfy->PSD[PwState] = tPSD[PwState];
    }

#if _TCG_==TCG_NONE
    pIdfy->security_ATA_SEC = CO_SUPPORT_ATA_SECURITY;
    pIdfy->security_SED     = 0;  //SED
    pIdfy->security_TCG     = 0;
#else
    if (mTcgStatus & TCG_ACTIVATED)
        pIdfy->security_ATA_SEC = 0;
    else
        pIdfy->security_ATA_SEC = CO_SUPPORT_ATA_SECURITY;

    pIdfy->security_SED = 1;  //SED

    if ((gNvmeIfMgr.SecurityMgr.SecurityState != SCU_SEC1) &&
        (gNvmeIfMgr.SecurityMgr.SecurityState != SCU_SEC2))
        pIdfy->security_TCG = 0;
    else
        pIdfy->security_TCG = 1;  //TCG
#endif
    *transferSize = sizeof(NvmeIdentifyController_t);

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Identify - Active namespace Ids

    @param[in]  pIdfy   identify active namespace ids pointer
    @param[in]  nsid    namespace id

    @return transfer size in byte count, zero if any error
**/
//--------------------------------------------------------------------------
static Error_t IdentifyActiveNamespaceIds(NvmeIdentifyActiveNamespaceIds_t* pIdfy, uint32_t nsid, uint32_t* transferSize)
{
    uint16_t index = 0;

    if (nsid == 0) // the smallest namespace id is 1.
    {
        nsid = 1;
    }

    while (nsid <= NVME_NUM_OF_NAMESPACES)
    {
        if (gNvmeIfMgr.ns[nsid - 1].Attach)
        {
            pIdfy->ACTIVE_NSID[index++] = nsid;
        }

        nsid++;
    }

    *transferSize = sizeof(NvmeIdentifyActiveNamespaceIds_t);

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Identify - ContainNamespaceIds

    @param[in]  pIdfy   identify contain namespace ids pointer
    @param[in]  nsid    namespace id

    @return transfer size in byte count, zero if any error
**/
//--------------------------------------------------------------------------
static Error_t IdentifyContainNamespaceIds(NvmeIdentifyContainNamespaceIds_t* pIdfy, uint32_t nsid, uint32_t* transferSize)
{
    uint16_t index = 0;

    if (nsid == 0) // the smallest namespace id is 1.
    {
        nsid = 1;
    }

    while (nsid <= NVME_NUM_OF_NAMESPACES)
    {
        if (gNvmeIfMgr.ns[nsid - 1].Create)
        {
            pIdfy->CONTAIN_NSID[index++] = nsid;
        }

        nsid++;
    }

    *transferSize = sizeof(NvmeIdentifyContainNamespaceIds_t);

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Identify - ContainNamespace

    @param[in]  pIdfy   identify active namespace ids pointer
    @param[in]  nsid    namespace id

    @return transfer size in byte count, zero if any error
**/
//--------------------------------------------------------------------------
static Error_t IdentifyContainNamespace(NvmeIdentifyNamespace_t* pIdfy, uint32_t nsid, uint32_t* transferSize)
{
    uint32_t lbafIndex;
    uint32_t nsid0;

    nsid0 = nsid - 1; // convert to zero based id

    if (nsid0 >= NVME_NUM_OF_NAMESPACES || !gNvmeIfMgr.ns[nsid0].Create)
    {
        gNvmeIfMgr.lastErrorBitOffset  = 0;
        gNvmeIfMgr.lastErrorByteOffset = ERR_DW1_OFFSET; // DW1, Namespaces
        return MK_NVME_ERROR_CODE(cInvalidNamespaceOrFormat, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
    }

    lbafIndex = gNvmeIfMgr.ns[nsid0].LBAF;

    pIdfy->NUSE                        = 0;
    pIdfy->NLBAF                       = NVME_NUM_OF_LBAF - 1;            // Number of lba formats, 0 based
    pIdfy->FLBAS_METADATA_LBA_END      = gNvmeIfMgr.ns[nsid0].MS;
    pIdfy->FLBAS_SUPPORTED_COMBINATION = gNvmeIfMgr.ns[nsid0].LBAF;

    // Meta data capabilities
    #if CO_SUPPORT_DATA_PROTECTION
    pIdfy->MC                          = 1;                                // Support a seperate buffer and an extended data LBA
    pIdfy->DPC_INFO_TYPE1              = 1;
    pIdfy->DPC_INFO_FIRST_EIGHT_BYTES  = 0;
    pIdfy->DPC_INFO_LAST_EIGHT_BYTES   = 1;
    pIdfy->PI                          = gNvmeIfMgr.ns[nsid0].PI;
    pIdfy->PIL                         = gNvmeIfMgr.ns[nsid0].PIL;
    #endif

    // size and capacity
    pIdfy->NSZE = gNvmeIfMgr.ns[nsid0].NSZE;

    if (gNvmeIfMgr.ns[nsid0].Attach)  pIdfy->NCAP = gNvmeIfMgr.ns[nsid0].NCAP;

    pIdfy->NVMCAP[0] = (gNvmeIfMgr.ns[nsid0].mbu_end - gNvmeIfMgr.ns[nsid0].mbu_start) << (9 + gNvmeIfMgr.ns[nsid0].hbu_shift);

    // LBA format example
    for (lbafIndex = 0; lbafIndex < NVME_NUM_OF_LBAF; lbafIndex++)
    {
        pIdfy->LBAF[lbafIndex].LBADS = LOG2(cLBAFTable[lbafIndex].dataSize);
        pIdfy->LBAF[lbafIndex].MS    = cLBAFTable[lbafIndex].metaSize;
        pIdfy->LBAF[lbafIndex].RP    = cLBAFTable[lbafIndex].rp;
    }

    *transferSize = sizeof(NvmeIdentifyNamespace_t);

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Identify - ControlList

    @param[in]  pIdfy   identify active namespace ids pointer
    @param[in]  nsid    namespace id

    @return transfer size in byte count, zero if any error
**/
//--------------------------------------------------------------------------
static Error_t IdentifyControlListAttach(NvmeIdentifyControlList_t* pIdfy, uint32_t nsid, uint32_t cntid, uint32_t* transferSize)
{
    if (nsid && nsid <= NVME_NUM_OF_NAMESPACES)
    {
        if (cntid <= 1 && gNvmeIfMgr.ns[nsid].Attach)
        {
            pIdfy->NumId     = 1;
            pIdfy->CNTLID[0] = 1;//NVME_PCIE_ID;
        }

        *transferSize = sizeof(NvmeIdentifyControlList_t);
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
    Identify - ControlList NVM subsystem

    @param[in]  pIdfy   identify active namespace ids pointer
    @param[in]  nsid    namespace id

    @return transfer size in byte count, zero if any error
**/
//--------------------------------------------------------------------------
static Error_t IdentifyControlList(NvmeIdentifyControlList_t* pIdfy, uint32_t nsid, uint32_t cntid, uint32_t* transferSize)
{
    if (nsid && nsid <= NVME_NUM_OF_NAMESPACES)
    {
        if (cntid <= 1)
        {
            pIdfy->NumId     = 1;
            pIdfy->CNTLID[0] = 1;//NVME_PCIE_ID;
        }

        *transferSize = sizeof(NvmeIdentifyControlList_t);
    }
    else
    {
        gNvmeIfMgr.lastErrorBitOffset  = 0;
        gNvmeIfMgr.lastErrorByteOffset = ERR_DW1_OFFSET; // DW1, NSID
        return MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
    }

    return cEcNoError;
}
