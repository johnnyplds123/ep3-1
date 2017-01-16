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
//! @brief NVMe Security send command handler
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
#include "RegNvmePkg.h"

// HAL

// APL
#include "HldNvme.h"
#include "SysInfo.h"
#include "Cache.h"
#include "UncTable.h"

// Code Section
//#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING
#include "NormSection.h"

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define NAMESPACE_TAG   0x4E414D01

typedef enum
{
    cCreate = 0,
    cDelete = 1,
} NamespaceMSelect_t;

typedef enum
{
    cAttach = 0,
    cDetach = 1,
} NamespaceASelect_t;

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief NVMe I/O Command structure - Namespace Management
typedef struct
{
    AdminCommandCommon_t DW9_0;   ///< DW9..0
    uint32_t SEL:4;               ///< Select (DW10[3:0])
    uint32_t reserved10:28;       ///< Reserved (DW10[31:4])
    uint32_t reserved11[5];       ///< Reserved (DW15..11)
} NvmeNamespaceManagementCommand_t;
COMPILE_ASSERT(sizeof(NvmeNamespaceManagementCommand_t)==64, "NVMe command size shall be 64");

/// @brief
typedef struct
{
    uint64_t NSZE;                ///< Namespace Size (NSZE) (bytes 7..0)
    uint64_t NCAP;                ///< Namespace Capacity (NCAP)  (bytes 15..8)
    uint8_t  Reserved16[10];      ///< Reserved (bytes 25..16)
    uint8_t  FLBAS;               ///< Formatted LBA Size (FLBAS)  (bytes 26)
    uint8_t  Reserved27[2];       ///< Reserved (bytes 28..27)
    uint8_t  DPS;                 ///< End-to-end Data Protection Type Settings (DPS) (bytes 29)
    uint8_t  NMIC;                ///< Namespace Multi-path I/O and Namespace Sharing Capabilities (NMIC) (bytes 30)
    uint8_t  reserved31[353];     ///< Reserved (bytes 384..31)
    uint32_t reserved384[160];    ///< Reserved (bytes 1023..384)
    uint32_t Vendor[768];         ///< Vendor specific (bytes 4095..1024)
} NamespaceManaageCreate_t;
COMPILE_ASSERT(sizeof(NamespaceManaageCreate_t) == 4096, " NamespaceManaageCreate_t size shall be 4096");

/// @brief
typedef struct
{
    uint32_t NamespaceList[1024];
} NamespaceAttach_t;
COMPILE_ASSERT(sizeof(NamespaceAttach_t) == 4096, " NamespaceManaageCreate_t size shall be 4096");

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static Error_t NamespaceManageCreate(HcmdQ_t* pHcmdQ, uint32_t nsId);
static Error_t NamespaceManageDelete(HcmdQ_t* pHcmdQ, uint32_t nsId);
static Error_t NamespaceAttach(NamespaceAttach_t* pBuff, uint32_t nsid);
static Error_t NamespaceDetach(NamespaceAttach_t* pBuff, uint32_t nsid);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
const NvmeIfMgrNamespace_t cNamespaceTable[HW_MAX_NAMESPACES] =
{
    //     NSZE        NCAP   mbu_start,  mbu_end,  LBAF Attach  Create     MS PI     PIL  Rsvd hbu_shift
    {   U64_MAX,    U64_MAX,          0,        0,     0,  TRUE,   TRUE,  TRUE, 0,  FALSE,    0,       0},
    {  0x300000,   0x300000,          0,        0,     0,  TRUE,   TRUE,  TRUE, 0,  FALSE,    0,       0},
    {         0,          0,          0,        0,     0, FALSE,  FALSE,  TRUE, 0,  FALSE,    0,       0},
    {         0,          0,          0,        0,     0, FALSE,  FALSE,  TRUE, 0,  FALSE,    0,       0}
};

//-----------------------------------------------------------------------------
//  Imported data prototype
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function proto-tpye
//-----------------------------------------------------------------------------
extern void Trim_All(HcmdQ_t* pHcmdQ, uint32_t nsId);

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    NamespaceManage - Init

**/
//--------------------------------------------------------------------------
void NvmeCmd_NvmespaceInit(InitBootMode_t initMode)
{
    uint32_t nsId0;

#if (CO_SUPPORT_NAMESPACE_MANAGEMENT == TRUE)
    if ((smSysInfo->d.Header.d.SubHeader.d.NVMe.d.dwValidTag != SI_TAG_NVME) ||
        (smSysInfo->d.NVMeData.d.Namespace.TAG != NAMESPACE_TAG))
    {
        MEM_CLR((U8*)&smSysInfo->d.NVMeData.d.Namespace, sizeof(tNAMESPACE));
        smSysInfo->d.NVMeData.d.Namespace.TAG = NAMESPACE_TAG;
#endif
        MEM_COPY(gNvmeIfMgr.ns, cNamespaceTable, sizeof(NvmeIfMgrNamespace_t) * (HW_MAX_NAMESPACES));

        gNvmeIfMgr.ns[HW_MAX_NAMESPACES].mbu_start = 0;

        #if (_MEDIA_==MEDIA_DRAM)
            gNvmeIfMgr.ns[HW_MAX_NAMESPACES].mbu_end = RAM_DRIVE_HBU_SIZE                     / HBU_PER_MBU;
        #else
            gNvmeIfMgr.ns[HW_MAX_NAMESPACES].mbu_end = IDEMA_GBYTE2LBU(gNfInfo.userCapacity) / HBU_PER_MBU;
        #endif

        for (nsId0 = 0; nsId0 < NVME_NUM_OF_NAMESPACES; nsId0++)
        {
            if (nsId0 == 0 && gNvmeIfMgr.ns[0].NSZE == U64_MAX)
            {
                #if (_MEDIA_==MEDIA_DRAM)
                    gNvmeIfMgr.ns[nsId0].NSZE = RAM_DRIVE_HBU_SIZE                     >> cLBAFTable[gNvmeIfMgr.ns[nsId0].LBAF].hbu_shift;
                #else
                    gNvmeIfMgr.ns[nsId0].NSZE = IDEMA_GBYTE2LBU(gNfInfo.userCapacity) >> cLBAFTable[gNvmeIfMgr.ns[nsId0].LBAF].hbu_shift;
                #endif

                gNvmeIfMgr.ns[nsId0].NCAP = gNvmeIfMgr.ns[nsId0].NSZE;
            }

            if (gNvmeIfMgr.ns[nsId0].Create)
            {
                gNvmeIfMgr.numOfNamespacesCreated++;

                if (gNvmeIfMgr.ns[nsId0].Attach)
                {
                    gNvmeIfMgr.numOfNamespacesAttached++;
                }
            }

            HldNvme_SetNamespace(nsId0);
            gNvmeIfMgr.ns[HW_MAX_NAMESPACES].mbu_start += (gNvmeIfMgr.ns[nsId0].mbu_end - gNvmeIfMgr.ns[nsId0].mbu_start);
        }

        D_PRINTF("[NVMe] Namespace Reset\n");

#if (CO_SUPPORT_NAMESPACE_MANAGEMENT == TRUE)
        smSysInfo->d.NVMeData.d.Namespace.numOfNamespacesAttached = gNvmeIfMgr.numOfNamespacesAttached;
        smSysInfo->d.NVMeData.d.Namespace.numOfNamespacesCreated  = gNvmeIfMgr.numOfNamespacesCreated;
        MEM_COPY(smSysInfo->d.NVMeData.d.Namespace.ns, gNvmeIfMgr.ns, sizeof(NvmeIfMgrNamespace_t) * (HW_MAX_NAMESPACES));
        smSysInfo->d.Header.d.SubHeader.d.NVMe.d.dwValidTag = SI_TAG_NVME;
        SyIn_Synchronize(SI_AREA_NVME, SYSINFO_WRITE, SI_SYNC_BY_NVME);
    }
    else
    {
        MEM_COPY(gNvmeIfMgr.ns, smSysInfo->d.NVMeData.d.Namespace.ns, sizeof(NvmeIfMgrNamespace_t) * (HW_MAX_NAMESPACES));
        gNvmeIfMgr.numOfNamespacesAttached = smSysInfo->d.NVMeData.d.Namespace.numOfNamespacesAttached;
        gNvmeIfMgr.numOfNamespacesCreated  = smSysInfo->d.NVMeData.d.Namespace.numOfNamespacesCreated;

        gNvmeIfMgr.ns[HW_MAX_NAMESPACES].mbu_start = 0;
        gNvmeIfMgr.ns[HW_MAX_NAMESPACES].mbu_end   = IDEMA_GBYTE2LBU(gNfInfo.userCapacity) / (MBU_SIZE / HLBU_DATA_SIZE);

        for (nsId0 = 0; nsId0 < NVME_NUM_OF_NAMESPACES; nsId0++)
        {
            if (gNvmeIfMgr.ns[nsId0].Create)
            {
                HldNvme_SetNamespace(nsId0);
                gNvmeIfMgr.ns[HW_MAX_NAMESPACES].mbu_start += (gNvmeIfMgr.ns[nsId0].mbu_end - gNvmeIfMgr.ns[nsId0].mbu_start);
            }
        }
    }
#endif
}

#if (CO_SUPPORT_NAMESPACE_MANAGEMENT == TRUE)
//-----------------------------------------------------------------------------
/**
    Namespace Management - Admin 0Dh

    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_NamespaceManagement(HcmdQ_t* pHcmdQ)
{
    NvmeNamespaceManagementCommand_t* pCdb;
    uint32_t nsId;

    pCdb = (NvmeNamespaceManagementCommand_t *)NvmeCmd_GetCdbPointer(pHcmdQ);
    nsId = pCdb->DW9_0.NSID;

    switch (pCdb->SEL)
    {
        case cCreate:
            pHcmdQ->error = NamespaceManageCreate(pHcmdQ, nsId);
            break;

        case cDelete:
            pHcmdQ->error = NamespaceManageDelete(pHcmdQ, nsId);
            break;

        default:
            pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
            gNvmeIfMgr.lastErrorBitOffset  = 0;
            gNvmeIfMgr.lastErrorByteOffset = ERR_DW10_OFFSET; // DW10, SEL
            break;
    }

    if (pHcmdQ->error == cEcNoError)
    {
        smSysInfo->d.NVMeData.d.Namespace.numOfNamespacesCreated  = gNvmeIfMgr.numOfNamespacesCreated;
        smSysInfo->d.NVMeData.d.Namespace.numOfNamespacesAttached = gNvmeIfMgr.numOfNamespacesAttached;
        MEM_COPY((&smSysInfo->d.NVMeData.d.Namespace.ns), (&gNvmeIfMgr.ns), sizeof(NvmeIfMgrNamespace_t) * (HW_MAX_NAMESPACES));
        SyIn_Synchronize(SI_AREA_NVME, SYSINFO_WRITE, SI_SYNC_BY_NVME);
    }
}

//-----------------------------------------------------------------------------
/**
    NamespaceManage - Create

**/
//--------------------------------------------------------------------------
static Error_t NamespaceManageCreate(HcmdQ_t* pHcmdQ, uint32_t nsId)
{
    Error_t error = cEcNoError;
    NamespaceManaageCreate_t* pNamespace;
    NvmeSdb_t* pSdb;
    void* pBuff;
    uint32_t valid_mbu_start;
    uint32_t valid_mbu_end;
    uint32_t idx;
    uint32_t nsId0;

    pSdb       = (NvmeSdb_t *)NvmeCmd_GetSdbPointer(pHcmdQ);
    pBuff      = (void *)NvmeCmd_GetScratchBufferPointer();
    pNamespace = (NamespaceManaageCreate_t *)pBuff;

    pHcmdQ->attr.b.xferDir   = cXferH2d;
    pHcmdQ->bufDesc.ssdIndex = SSD_IF_START;
    AplHost_TransferWholeData(pHcmdQ, pBuff, 4096, TRUE);

    if (gNvmeIfMgr.numOfNamespacesCreated == NVME_NUM_OF_NAMESPACES)
    {
        error = MK_NVME_ERROR_CODE(cNamespaceIdentifierUnavailable, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
    }
    else if (((pNamespace->FLBAS & 0x0F) > NVME_NUM_OF_LBAF) || pNamespace->DPS || pNamespace->NMIC)
    {
        error = MK_NVME_ERROR_CODE(cInvalidFormat, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
    }
    else
    {
        for (nsId0 = 0; nsId0 < NVME_NUM_OF_NAMESPACES; nsId0++)
        {
            if (gNvmeIfMgr.ns[nsId0].Create == 0)
            {
                break;
            }
        }

        valid_mbu_start  = 0;
        valid_mbu_end = gNvmeIfMgr.ns[HW_MAX_NAMESPACES].mbu_end;

        // Search backward
        idx = nsId0;
        while (idx-- > 0)
        {
            if (gNvmeIfMgr.ns[idx].Create)
            {
                valid_mbu_start = gNvmeIfMgr.ns[idx].mbu_end;
                break;
            }
        }

        // Search forward
        idx = nsId0;
        while (++idx < NVME_NUM_OF_NAMESPACES)
        {
            if (gNvmeIfMgr.ns[idx].Create)
            {
                valid_mbu_end = gNvmeIfMgr.ns[idx].mbu_start;
                break;
            }
        }

        // convert to mbu unit to check available space
        if ((pNamespace->NSZE >> cLBAFTable[pNamespace->FLBAS & 0xF].mbu_shift) > (valid_mbu_end - valid_mbu_start))
        {
            error = MK_NVME_ERROR_CODE(cNamespaceInsufficientCapacity, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
        }
        else
        {
            gNvmeIfMgr.ns[nsId0].NSZE   = pNamespace->NSZE;
            gNvmeIfMgr.ns[nsId0].NCAP   = pNamespace->NCAP;
            gNvmeIfMgr.ns[nsId0].LBAF   = pNamespace->FLBAS & 0xF;
            gNvmeIfMgr.ns[nsId0].MS     = 1;
            gNvmeIfMgr.ns[nsId0].PI     = 0;
            gNvmeIfMgr.ns[nsId0].PIL    = 0;
            gNvmeIfMgr.ns[nsId0].Create = 1;
            gNvmeIfMgr.ns[nsId0].Attach = 0;

            HldNvme_SetNamespace(nsId0);
            gNvmeIfMgr.ns[HW_MAX_NAMESPACES].mbu_start += (gNvmeIfMgr.ns[nsId0].mbu_end - gNvmeIfMgr.ns[nsId0].mbu_start);
            gNvmeIfMgr.numOfNamespacesCreated++;

            pSdb->DW0 = nsId0 + 1;

            D_PRINTF("Create nsid[%x] cap[%x]\n", nsId0, pNamespace->NCAP);

            pHcmdQ->attr.all |= ATTR_OVERLAPPED;

            Trim_All(pHcmdQ, nsId0);

            UNCT_CHECK_DO_DELETE(gNvmeIfMgr.ns[nsId0].mbu_start << HBU_PER_MBU_SFT, (gNvmeIfMgr.ns[nsId0].mbu_end - gNvmeIfMgr.ns[nsId0].mbu_start) << HBU_PER_MBU_SFT, ENABLE);
        }
    }

    return error;
}

//-----------------------------------------------------------------------------
/**
    NamespaceManage - Delete

**/
//--------------------------------------------------------------------------
static Error_t NamespaceManageDelete(HcmdQ_t* pHcmdQ, uint32_t nsId)
{
    Error_t error  = cEcNoError;
    uint32_t nsid0 = nsId - 1;

    if (!gNvmeIfMgr.ns[nsid0].Create)
    {
        error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
    }
    else
    {
        if (gNvmeIfMgr.ns[nsid0].Attach)
        {
            gNvmeIfMgr.numOfNamespacesAttached--;
        }

        gNvmeIfMgr.ns[HW_MAX_NAMESPACES].mbu_start -= (gNvmeIfMgr.ns[nsid0].mbu_end - gNvmeIfMgr.ns[nsid0].mbu_start);

        MEM_CLR(&gNvmeIfMgr.ns[nsid0], sizeof(NvmeIfMgrNamespace_t));

        gNvmeIfMgr.numOfNamespacesCreated--;

        D_PRINTF("Delete nsid[%x]\n", nsid0);
    }

    return error;
}

//-----------------------------------------------------------------------------
/**
    Namespace Attachment - Admin 15h

    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_NamespaceAttachment(HcmdQ_t* pHcmdQ)
{
    NvmeNamespaceManagementCommand_t* pCdb;
    void* pBuff;

    pCdb  = (NvmeNamespaceManagementCommand_t *)NvmeCmd_GetCdbPointer(pHcmdQ);
    pBuff = (void *)NvmeCmd_GetScratchBufferPointer();

    switch (pCdb->SEL)
    {
        case cAttach:
            pHcmdQ->error = NamespaceAttach((NamespaceAttach_t *)pBuff, pCdb->DW9_0.NSID);
            break;

        case cDetach:
            pHcmdQ->error = NamespaceDetach((NamespaceAttach_t *)pBuff, pCdb->DW9_0.NSID);
            break;

        default:
            pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
            gNvmeIfMgr.lastErrorBitOffset  = 0;
            gNvmeIfMgr.lastErrorByteOffset = ERR_DW10_OFFSET; // DW10, SEL
            break;
    }

    if (pHcmdQ->error == cEcNoError)
    {
        smSysInfo->d.NVMeData.d.Namespace.numOfNamespacesCreated  = gNvmeIfMgr.numOfNamespacesCreated;
        smSysInfo->d.NVMeData.d.Namespace.numOfNamespacesAttached = gNvmeIfMgr.numOfNamespacesAttached;
        MEM_COPY((&smSysInfo->d.NVMeData.d.Namespace.ns), (&gNvmeIfMgr.ns), sizeof(NvmeIfMgrNamespace_t) * (HW_MAX_NAMESPACES));
        SyIn_Synchronize(SI_AREA_NVME, SYSINFO_WRITE, SI_SYNC_BY_NVME);
    }
}

//-----------------------------------------------------------------------------
/**
    NamespaceManage - Attach
**/
//-----------------------------------------------------------------------------
static Error_t NamespaceAttach(NamespaceAttach_t* pBuff, uint32_t nsid)
{
    Error_t error = cEcNoError;

    if (!nsid || (nsid > NVME_NUM_OF_NAMESPACES) || !gNvmeIfMgr.ns[nsid - 1].Create)
    {
        error = MK_NVME_ERROR_CODE(cNamespaceIsPrivate, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
    }
    else if (gNvmeIfMgr.ns[nsid - 1].Attach)
    {
        error = MK_NVME_ERROR_CODE(cNamespaceAlreadyAttached, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
    }
    else
    {
        gNvmeIfMgr.ns[nsid - 1].Attach = 1;
        gNvmeIfMgr.numOfNamespacesAttached++;

        D_PRINTF("Attach nsid[%x]\n",nsid);
    }

    return(error);
}

//-----------------------------------------------------------------------------
/**
    NamespaceManage - Delete
**/
//-----------------------------------------------------------------------------
static Error_t NamespaceDetach(NamespaceAttach_t* pBuff, uint32_t nsid)
{
    Error_t error = cEcNoError;

    if (!nsid || (nsid > NVME_NUM_OF_NAMESPACES) || !gNvmeIfMgr.ns[nsid - 1].Attach || !gNvmeIfMgr.ns[nsid - 1].Create)
    {
        error = MK_NVME_ERROR_CODE(cNamespaceNotAttached, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
    }
    else
    {
        gNvmeIfMgr.ns[nsid - 1].Attach = 0;
        gNvmeIfMgr.numOfNamespacesAttached--;

        D_PRINTF("Detach nsid[%x]\n", nsid);
    }

    return(error);
}
#endif
