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
//! @brief NVMe Security receive command handler
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
#include "NvmeSecurityCmd.h"
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

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief NVMe Admin Command structure - Security Receive command - NVM Command Set Specific


typedef  struct
{
    uint8_t  reserve;
    uint8_t  Length;
    uint16_t EraseTimeRequire;
    uint16_t EnhEraseTimeRequire;
    uint16_t MasterPwdIdentifier;
    uint8_t  Maxset:1;
    uint8_t  reserve8:7;

    union
    {
        uint8_t all;                                ///<  128
        struct
        {
            uint8_t securitySupported:1;                ///<  BIT0
            uint8_t securityEnabled:1;                  ///<  BIT1
            uint8_t securityLocked:1;                   ///<  BIT2
            uint8_t securityFrozen:1;                   ///<  BIT3
            uint8_t securityCountExpired:1;             ///<  BIT4
            uint8_t enhancedSecurityEraseSupported:1;   ///<  BIT5
            uint8_t reserved0:2;                        ///<  BIT[7:6]
        } b;
    } securityStatus;       ///< 128
} ATA_SecurityReceive_t;

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
void Prepare_PID_00_List(U8 *pbuf);
static Error_t ATA_SecurityReceive(ATA_SecurityReceive_t* pBuff, NvmeAdminSecurityReceiveCommand_t* pCdb);
Error_t Nvme_CMD_CheckSecurity(uint8_t Admin, uint8_t CmdCode);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
const U8 SUPPORT_PID_00_LIST[] = { cSECURITY_INFO,
                               #if _TCG_ != TCG_NONE
                                   cTCG_PID_01,    cTCG_PID_02,  cIEEE1667_PID,
                               #endif
                                   cNVME_PID_EA,   cATA_SECURITY
                                 };
//-----------------------------------------------------------------------------
//  Imported data prototype
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function proto-tpye
//-----------------------------------------------------------------------------
#if _TCG_!=TCG_NONE
extern void Nvme2Sata_TrustedReceive(HcmdQ_t* pHcmdQ);
#endif

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    Security Receive - Admin 82h

    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------

        /** 1. According to ATA Command Set - 4 (ACS-4) page257

        Table 139 — TRUSTED RECEIVE SECURITY PROTOCOL field
        Value       Description
        -----------------------------
        00h         Return security protocol information (see 7.50.6)
        01h..06h    Reserved for TCG
        07h         Reserved for T10
        08h..1Fh    Reserved
        20h..21h    Reserved for T10
        22h..3Fh    Reserved
        40h..41h    Reserved for T10
        42h..E9h    Reserved
        EAh         Reserved for NVM Express
        EBh         Defined by SCSA (see 3.1.67)
        ECh         Reserved for JEDECR
        EDh         Reserved for SDCard
        EEh         Reserved for IEEE 1667
        EFh         Reserved for T10
        F0h..FFh    endor Specific
        &*/

        /** 2. According to SCSI Primary Command - 4 (SPC-4) page347

        Table 252 — SECURITY PROTOCOL field in SECURITY PROTOCOL IN command
        Code        Description                             Reference
        -------------------------------------------------------------
        00h         Security protocol information           7.6.1
        01h to 06h  Defined by the TCG                      3.1.176
        07h         CbCS                                    7.6.4
        08h to 1Fh  Reserved
        20h         Tape Data Encryption                    SSC-3
        21h         Data Encryption Configuration           ADC-3
        22h to 3Fh  Reserved
        40h         A Creation Capabilities                 7.6.2
        41h         KEv2-SCSI                               7.6.3
        42h to ECh  Reserved
        EDh         SD Card TrustedFlash specification      3.1.145
        EEh         Authentication in Host Attachments      IEEE 1667
                    of ransient Storage Devices
        EFh         ATA Device Server Password Security     SAT-2
        F0h to FFh  Vendor Specific
        &*/


void NvmeCmd_SecurityReceive(HcmdQ_t* pHcmdQ)
{
    NvmeAdminSecurityReceiveCommand_t* pCdb;
    uint32_t transferSize;
    void* pBuff;

    pCdb  = (NvmeAdminSecurityReceiveCommand_t *)NvmeCmd_GetCdbPointer(pHcmdQ);
    pBuff = (void *)NvmeCmd_GetScratchBufferPointer();
    transferSize = (pCdb->AL);
    MEM_CLR(pBuff, MIN(transferSize, cNvmeScratchBufferSize));

    BEGIN_HOST_DBG_DUMP(DBG_DUMP_HOST_NONRW)
        D_PRINTF("%s  SPSP=%4X SECP=%2X AL=%X\n", __FUNCTION__, pCdb->SPSP, pCdb->SECP, pCdb->AL);
    END_HOST_DBG_DUMP

    switch(pCdb->SECP)
    {
        case cSECURITY_INFO:
            Prepare_PID_00_List(pBuff);
            break;

        case cNVME_PID_EA:
            // TODO:
            break;

    #if _TCG_!=TCG_NONE
        case cTCG_PID_01:
        case cTCG_PID_02:
        case cIEEE1667_PID:
            //TODO: abort command if ATA Security Password is set!!
            Nvme2Sata_TrustedReceive(pHcmdQ);
            break;
    #endif
    #if CO_SUPPORT_ATA_SECURITY
        case cATA_SECURITY:
            pHcmdQ->error = ATA_SecurityReceive((ATA_SecurityReceive_t *)pBuff, pCdb);
            break;
    #endif
        default:
            pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
            gNvmeIfMgr.lastErrorBitOffset = 0;
            gNvmeIfMgr.lastErrorByteOffset = ERR_DW10_OFFSET + 3;
            break;
    }

    if ((pCdb->SECP != 0x01) && (pCdb->SECP != 0x02) && (pCdb->SECP != 0xEE))
    {
        if (pHcmdQ->error == cEcNoError)
        {
            pHcmdQ->bufDesc.ssdIndex = SSD_IF_START;
            AplHost_TransferWholeData(pHcmdQ, pBuff, transferSize, TRUE);
        }
    }
}

//-----------------------------------------------------------------------------
//    Prepare Supported Security Protocol list (PID = 0x00)
//    @param[in]  buf ptr
//    @param[out] none
//
//    ref SPC-4 page 467
//-----------------------------------------------------------------------------
void Prepare_PID_00_List(U8 *pbuf)
{
    U32 i;

    //[6~7] padload length
    pbuf += 6;
    *pbuf++ = 0;
    *pbuf++ = sizeof(SUPPORT_PID_00_LIST);

    //[8~n] support PID list
    for(i = 0; i < sizeof(SUPPORT_PID_00_LIST); i++){
        *pbuf++ = SUPPORT_PID_00_LIST[i];
    }
}

//-----------------------------------------------------------------------------
/**
    ATA_SecurityReceive

    @param[in]
**/
//-----------------------------------------------------------------------------
static Error_t ATA_SecurityReceive(ATA_SecurityReceive_t* pBuff, NvmeAdminSecurityReceiveCommand_t* pCdb)
{
    Error_t error;

    if (pCdb->SPSP != cSATA_CMD_SecurityInfo)
    {
        error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
        gNvmeIfMgr.lastErrorBitOffset = 0;
        gNvmeIfMgr.lastErrorByteOffset = ERR_DW10_OFFSET + 1;
        return (error);
    }
    //MEM_CLR(pBuff, cNvmeScratchBufferSize);

    pBuff->Length = 0x0E;
#if _TCG_!=TCG_NONE
    if(mTcgStatus&TCG_ACTIVATED) {
        //pBuff->EraseTimeRequire = 0;
        //pBuff->EnhEraseTimeRequire = 0;
        //pBuff->MasterPwdIdentifier = 0;
        //pBuff->Maxset = 0;
        //pBuff->securityStatus.all = 0;
    }
    else
#endif
    {
        pBuff->EraseTimeRequire = SEC_ERASE_TIME << 8;
        pBuff->EnhEraseTimeRequire = SEC_ENH_ERASE_TIME << 8;

        if (smSysInfo->d.NVMeData.d.SCU_CMD.MasterRv == 0)
            pBuff->MasterPwdIdentifier = 0xFEFF;  //0xFFFE  //LSB  MSB
        else
            pBuff->MasterPwdIdentifier = smSysInfo->d.NVMeData.d.SCU_CMD.MasterRv;

        if (smSysInfo->d.NVMeData.d.SCU_CMD.Level)
            pBuff->Maxset = 1;


        pBuff->securityStatus.all = 0;
        pBuff->securityStatus.b.securitySupported = 1;

        if (smSysInfo->d.NVMeData.d.SCU_CMD.PwEnable == TRUE)    // PW Enable check
            pBuff->securityStatus.b.securityEnabled = TRUE;

        if (gNvmeIfMgr.SecurityMgr.SecurityState == SCU_SEC4)    // Lock check
            pBuff->securityStatus.b.securityLocked = 1;

        if (gNvmeIfMgr.SecurityMgr.SecurityState == SCU_SEC2 ||  // Frozen check
            gNvmeIfMgr.SecurityMgr.SecurityState == SCU_SEC6)
            pBuff->securityStatus.b.securityFrozen = 1;

        if (gNvmeIfMgr.SecurityMgr.RetryExpired)
            pBuff->securityStatus.b.securityCountExpired = 1;    // Expire check


        pBuff->securityStatus.b.enhancedSecurityEraseSupported = 1;
    }

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Check Security execution condition.

    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
Error_t Nvme_CMD_CheckSecurity(uint8_t Admin, uint8_t CmdCode)
{
    Error_t error = cEcNoError;
#if _TCG_ == TCG_NONE
    if (gNvmeIfMgr.SecurityMgr.SecurityState != SCU_SEC4)
        return cEcNoError;
#endif
    if (Admin)
    {
        switch (CmdCode)
        {
            case cFirmwareActivate:
            case cFirmwareImageDownload:
            //case cFormatNvm:
#if _TCG_ != TCG_NONE
                if((gNvmeIfMgr.SecurityMgr.SecurityState == SCU_SEC4)
                  || mReadLockedStatus || mWriteLockedStatus )
#endif
                {
                    error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
                    gNvmeIfMgr.lastErrorBitOffset = 0;
                    gNvmeIfMgr.lastErrorByteOffset = 0;
                }
                break;
            default:
                break;
        }
    }
    else
    {
#if _TCG_ != TCG_NONE
        if (gNvmeIfMgr.SecurityMgr.SecurityState == SCU_SEC4)
#endif
        {
            //D_PRINTF("Security Lock\n");

            error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
            gNvmeIfMgr.lastErrorBitOffset = 0;
            gNvmeIfMgr.lastErrorByteOffset = 0;
        }
    }
    return (error);
}
