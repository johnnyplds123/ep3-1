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
#include "NvmeSecurityCmd.h"
#include "NvmeDeviceSelfTest.h"
#include "NvmeDirective.h"
#if _TCG_!=TCG_NONE
#include "TcgCommon.h"
#endif
#include "SysInfo.h"
#include "UncTable.h"
#include "Purge.h"

// Code Section
#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
typedef enum
{
    cNoErase = 0,
    cDataErase,
    cCryptographicErase,
} SecuritySelect_t;

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief NVMe Admin Command structure - NVM Command Specific - Format NVM
typedef  struct
{
    AdminCommandCommon_t DW9_0;    ///< DW9..0

    uint32_t LBAF:4;          ///< LBA Format (DW10 bits[03:00])
    uint32_t MS:1;            ///< Meta data Settings (DW10 bits[04])
    uint32_t PI:3;            ///< Protection Information (DW10 bits[07:05])
    uint32_t PIL:1;           ///< Protection Information Location (DW10 bits[08])
    uint32_t SES:3;           ///< Secure Erase Settings (DW10 bits[11:09])
    uint32_t reserved10:20;   ///< Secure Erase Settings (DW10 bits[31:12])

    uint32_t reserved11[5]; ///< DW15..11

} NvmeAdminFormatCommand_t;
COMPILE_ASSERT(sizeof(NvmeAdminFormatCommand_t) == 64, "NVMe command size shall be 64");

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
void Trim_All(HcmdQ_t* pHcmdQ, uint32_t nsId0);

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
    Format - Admin 80h

    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
void NvmeCmd_Format(HcmdQ_t* pHcmdQ)
{
    uint32_t nsId;
    uint32_t endNsId;
    NvmeAdminFormatCommand_t* pCdb;

    pCdb = (NvmeAdminFormatCommand_t *)NvmeCmd_GetCdbPointer(pHcmdQ);
    nsId = pCdb->DW9_0.NSID; // 1 based value,

    BEGIN_HOST_DBG_DUMP(DBG_DUMP_HOST_NONRW)
        D_PRINTF("%s NSID:%X LBAF:%1X MS:%1X PI:%1X PIL:%1X SEC:%1X\n ", __FUNCTION__, pCdb->DW9_0.NSID, pCdb->LBAF, pCdb->MS, pCdb->PI, pCdb->PIL, pCdb->SES);
    END_HOST_DBG_DUMP

    if ((nsId == 0 || nsId > NVME_NUM_OF_NAMESPACES || !gNvmeIfMgr.ns[nsId - 1].Attach) && (nsId != cNvmeGlobalNamespaceId))
    {
        pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidNamespaceOrFormat, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
        gNvmeIfMgr.lastErrorBitOffset  = 0;
        gNvmeIfMgr.lastErrorByteOffset = ERR_DW1_OFFSET; // DW1, Namespaces

        return;
    }

    #if _TCG_!= TCG_NONE
    if (mTcgStatus&TCG_ACTIVATED)
    {
        D_PRINTF("TCG Activated!!\n");
        pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFormat, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
        gNvmeIfMgr.lastErrorBitOffset  = 0;
        gNvmeIfMgr.lastErrorByteOffset = 0;
        return;
    }
    #endif

    #if CO_SUPPORT_ATA_SECURITY
    // invalid security state
    if (gNvmeIfMgr.SecurityMgr.SecurityState == SCU_SEC4 ||  // locked state
        gNvmeIfMgr.SecurityMgr.SecurityState == SCU_SEC2 ||  // unlocked/frozen state
        gNvmeIfMgr.SecurityMgr.SecurityState == SCU_SEC6)    // unlocked/frozen state
    {
        D_PRINTF("SecurityState=%2x\n",gNvmeIfMgr.SecurityMgr.SecurityState);
        pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFormat, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
        gNvmeIfMgr.lastErrorBitOffset  = 0;
        gNvmeIfMgr.lastErrorByteOffset = 0;
        return;
    }
    #endif

    #if CO_SUPPORT_DEVICE_SELF_TEST
    DST_Completion(cDSTAbortFormat);
    #endif

    if (nsId == cNvmeGlobalNamespaceId)
    {
        nsId = 1;
        endNsId = NVME_NUM_OF_NAMESPACES;
    }
    else
    {
        endNsId = nsId;
    }

    for ( ; nsId<=endNsId; nsId++ )
    {
        // TODO:
        // security erase media
        switch (pCdb->SES)
        {
            case cNoErase:
                break;

            case cDataErase:
                break;

            #if _TCG_!= TCG_NONE
            case cCryptographicErase:
                TcgUpdateGlobalKey();
                break;
            #endif

            default:
                pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFormat, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
                gNvmeIfMgr.lastErrorBitOffset  = 1;
                gNvmeIfMgr.lastErrorByteOffset = ERR_DW10_OFFSET + 1; // DW10, SES
                break;
        }

        // Format media
        if ((pCdb->LBAF >= NVME_NUM_OF_LBAF) || pCdb->PI)
        {
            pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFormat, cCommandSpecificStatus, cNoMoreStatusInformation, cDoNotRetry);
            gNvmeIfMgr.lastErrorBitOffset  = 0;
            gNvmeIfMgr.lastErrorByteOffset = ERR_DW10_OFFSET; // DW10, LBAF
        }

        if (pHcmdQ->error == cEcNoError)
        {
            uint32_t nsId0 = nsId - 1;

            gNvmeIfMgr.ns[nsId0].LBAF = pCdb->LBAF;
            HldNvme_SetNamespace(nsId0);

            #if (SUP_DIRE_STREAMS == ENABLE)
            MEM_CLR(&gDirectiveMgr.streamInfo[nsId0], sizeof(StreamInfo_t));
            #endif

            //if ((pCdb->SES == cDataErase)
            //    || ((pCdb->SES == cNoErase) && (pCdb->DW9_0.NSID == cNvmeGlobalNamespaceId))  //IOL 1.2.1 Test 1.6 issue
            //   )
            {
                pHcmdQ->attr.all |= ATTR_OVERLAPPED;

                Trim_All(pHcmdQ, nsId0);

                if (gNvmeIfMgr.numOfNamespacesCreated > 1)
                {
                    UNCT_CHECK_DO_DELETE(gNvmeIfMgr.ns[nsId0].mbu_start << HBU_PER_MBU_SFT, (gNvmeIfMgr.ns[nsId0].mbu_end - gNvmeIfMgr.ns[nsId0].mbu_start) << HBU_PER_MBU_SFT, ENABLE);
                }
                else
                {
                    Purge_Center(PURGE_CORE_UNCT);
                }
            }

            smSysInfo->d.NVMeData.d.Namespace.numOfNamespacesAttached = gNvmeIfMgr.numOfNamespacesAttached;
            smSysInfo->d.NVMeData.d.Namespace.numOfNamespacesCreated  = gNvmeIfMgr.numOfNamespacesCreated;
            MEM_COPY(smSysInfo->d.NVMeData.d.Namespace.ns, gNvmeIfMgr.ns, sizeof(NvmeIfMgrNamespace_t) * (HW_MAX_NAMESPACES));
            SyIn_Synchronize(SI_AREA_NVME, SYSINFO_WRITE, SI_SYNC_BY_NVME);
        }
    }
}

//-----------------------------------------------------------------------------
/**
    @param[in]  pHcmdQ   Host command queue pointer
    @param[in]  nsid     namespace id (zero base)
**/
//-----------------------------------------------------------------------------
void Trim_All(HcmdQ_t* pHcmdQ, uint32_t nsId0)
{
    MsgFormat_t* pFormatMsg;
    NvmeAdminFormatCommand_t* pCdb;

    Host_AdminCmdOverlapReq();
    pFormatMsg = (MsgFormat_t*) GetHcmdMsgPtr(pHcmdQ);
    MSG_SET_HOST_INIT(pFormatMsg, cMcFormat);
    pFormatMsg->laaStart = gNvmeIfMgr.ns[nsId0].mbu_start;
    pFormatMsg->laaEnd   = gNvmeIfMgr.ns[nsId0].mbu_end; // (last laa is not included)

    if (NvmeCmd_GetCdbPointer(pHcmdQ)->OPC == cFormatNvm)
    {
        pCdb            = (NvmeAdminFormatCommand_t *)NvmeCmd_GetCdbPointer(pHcmdQ);
        pFormatMsg->SES = pCdb->SES;
    }
    else
    {
        pFormatMsg->SES = 0;
    }

    Host_MediaReq();
    IPC_SendMsgQ(cH2cReqQueue, (Msg_t*) pFormatMsg);
}
