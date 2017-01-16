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

// HAL

// APL
#include "HldNvme.h"
#include "NvmeSecurityCmd.h"
#if _TCG_!=TCG_NONE
#include "TcgCommon.h"
#endif
#include "SysInfo.h"
#include "Cache.h"
#include "UncTable.h"
#include "Purge.h"

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
/// @brief NVMe Admin Command structure - Security Send command - NVM Command Set Specific

typedef struct
{
    uint8_t  SecLevel:1;                 // 0 = High, 1 = Maximum
    uint8_t  reserved0:7;
    uint8_t  Identify:1;                 // 0 = User PW, 1 = Master PW
    uint8_t  reserved1:7;
    uint16_t PW[SEC_PW_SIZE];            // Byte 2~33
    uint16_t RevCode;
} tSATA_PW_DATA;

/// @brief Format message structure
typedef struct
{
    MsgHdr_t    hdr;        ///< message common header
    Error_t     error;      ///< error code
    uint8_t     Erase;
    uint32_t    param[4];
} tMSG_EraseUnit;
COMPILE_ASSERT(sizeof(MsgFormat_t)<=sizeof(Msg_t), "shall be less than Msg_t size");

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
void SATA_Security_Init(InitBootMode_t initMode);
static void ATA_SecuritySend(tSATA_PW_DATA* pBuff, HcmdQ_t* pHcmdQ);
void MediaEraseAllRequest(HcmdQ_t* pHcmdQ);
void MediaEraseAllRequestDone(HcmdQ_t* pHcmdQ);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
tSATA_PW_DATA* pPwData;
Error_t        gErrorSecutitySend;

//-----------------------------------------------------------------------------
//  Imported data prototype
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function proto-tpye
//-----------------------------------------------------------------------------
#if _TCG_!=TCG_NONE
extern void Nvme2Sata_TrustedSend(HcmdQ_t* pHcmdQ);

extern void TcgPreformatAndInit(U8 IsTcgInit);
extern void TcgKeepGroupDataBridge(U32 GrpNum);
extern void TcgEepOperatedBridge(U32 Metholdtype);
extern void TcgEepBackupBridge(void);
extern void TcgEepRestoreBridge(void);
extern void TcgPsidBackupBridge(void);
extern void TcgPsidRestoreBridge(void);
#endif

extern void Trim_All(HcmdQ_t* pHcmdQ, uint32_t nsId);

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    Security Send - Admin 81h

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


void NvmeCmd_SecuritySend(HcmdQ_t* pHcmdQ)
{
    NvmeAdminSecuritySendCommand_t* pCdb;
    uint32_t transferSize;

    pCdb = (NvmeAdminSecuritySendCommand_t *)NvmeCmd_GetCdbPointer(pHcmdQ);

    BEGIN_HOST_DBG_DUMP(DBG_DUMP_HOST_NONRW)
        D_PRINTF("%s SPSP=%4X SECP=%2X TL=%X\n", __FUNCTION__, pCdb->SPSP, pCdb->SECP, pCdb->TL);
    END_HOST_DBG_DUMP

    switch(pCdb->SECP)
    {
        case cNVME_PID_EA:
            // TODO:
            break;

    #if _TCG_!=TCG_NONE
        case cTCG_PID_01:
        case cTCG_PID_02:
        case cIEEE1667_PID:
            Nvme2Sata_TrustedSend(pHcmdQ);
            break;
    #endif
    #if CO_SUPPORT_ATA_SECURITY
        case cATA_SECURITY:
        {
            void* pBuff;

            pBuff= (void *)NvmeCmd_GetScratchBufferPointer();
            transferSize = (pCdb->TL);
            // check size
            if (transferSize > cNvmeScratchBufferSize)
            {
                pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
                gNvmeIfMgr.lastErrorBitOffset = 0;
                gNvmeIfMgr.lastErrorByteOffset = ERR_DW11_OFFSET; // DW11
                return;
            }
            
            pHcmdQ->bufDesc.ssdIndex = SSD_IF_START;
            AplHost_TransferWholeData(pHcmdQ, pBuff, transferSize, TRUE);

            if (pHcmdQ->error != cEcNoError) return;

	    	ATA_SecuritySend((tSATA_PW_DATA *)pBuff, pHcmdQ);
            break;
        }
    #endif
        default:
            pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
            gNvmeIfMgr.lastErrorBitOffset = 0;
            gNvmeIfMgr.lastErrorByteOffset = ERR_DW10_OFFSET + 3; // DW10, SECP
            break;
    }

}

//-----------------------------------------------------------------------------
/**
    Security frozen state command check

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t SATA_SecurityFrozen_CmdCheck(void)
{
#if _TCG_ != TCG_NONE
    if(mTcgStatus&TCG_ACTIVATED) {
        return cEcInvalidCommand;
    }
    else
#endif
    {
        if (gNvmeIfMgr.SecurityMgr.SecurityState == SCU_SEC2 ||
            gNvmeIfMgr.SecurityMgr.SecurityState == SCU_SEC6)
        {
            return cEcSecurityFrozen;
        }
        else
        {
            return cEcNoError;
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Security information Initial

**/
//-----------------------------------------------------------------------------
void SATA_Security_Init(InitBootMode_t initMode)
{
    // If EEPROM ATA not exist
    if ((smSysInfo->d.Header.d.SubHeader.d.NVMe.d.dwValidTag != SI_TAG_NVME) ||
        (smSysInfo->d.NVMeData.d.SCU_CMD.TAG != 0x5402))
    {
        smSysInfo->d.NVMeData.d.SCU_CMD.TAG = 0x5402;
        // Reset All Security Info
        MEM_CLR(&smSysInfo->d.NVMeData.d.SCU_CMD.MasterPw, sizeof(smSysInfo->d.NVMeData.d.SCU_CMD.MasterPw));
        MEM_CLR(&smSysInfo->d.NVMeData.d.SCU_CMD.UserPw, sizeof(smSysInfo->d.NVMeData.d.SCU_CMD.UserPw));
        smSysInfo->d.NVMeData.d.SCU_CMD.MasterRv = 0xFEFF;  //0xFFFE  // 0;  //LSB MSB
        smSysInfo->d.NVMeData.d.SCU_CMD.Level = HIGH_LEVEL;
        smSysInfo->d.NVMeData.d.SCU_CMD.PwEnable = FALSE;
    }

    gNvmeIfMgr.SecurityMgr.ErasePrepare = FALSE;
    gNvmeIfMgr.SecurityMgr.PasswordCnt = SEC_PW_MAX_RETRY_CNT;
    gNvmeIfMgr.SecurityMgr.SecurityState = SCU_SEC1;
    gNvmeIfMgr.SecurityMgr.RetryExpired = FALSE;

    if (smSysInfo->d.NVMeData.d.SCU_CMD.PwEnable == TRUE)
    {
        if (initMode == cInitBootFwUpdated){
            gNvmeIfMgr.SecurityMgr.SecurityState = SCU_SEC5;
        }else{
            gNvmeIfMgr.SecurityMgr.SecurityState = SCU_SEC4;
        }
    }

    D_PRINTF("SecurityState=%2x\n",gNvmeIfMgr.SecurityMgr.SecurityState);

}

//-----------------------------------------------------------------------------
/**
    Dump security info

    @param[in]  type : dump type
**/
//-----------------------------------------------------------------------------
/*void SATA_Security_InfoDump(U8 AllData)
{
    U8 idx;

    D_PRINTF("--Host\n");
    D_PRINTF("SecLevel = %02x  Identify = %02x\n", pPwData->SecLevel, pPwData->Identify);
    for (idx = 0; idx < SEC_PW_SIZE; idx ++)
    {
        D_PRINTF("PW[%d] = %x\n", idx, pPwData->PW[idx]);
    }
    D_PRINTF("RevCode = %x\n", pPwData->RevCode);

    if (AllData == TRUE)
    {
        D_PRINTF("--Device\n");
        D_PRINTF("Addr = %x\n", pPwData);
        D_PRINTF("Size = %x\n", sizeof(tSATA_PW_DATA));
        D_PRINTF("PwEnable = %x\n", smSysInfo->d.NVMeData.d.SCU_CMD.PwEnable);
        D_PRINTF("Size MPW = %d\n", sizeof(smSysInfo->d.NVMeData.d.SCU_CMD.MasterPw));
        D_PRINTF("Size UPW = %d\n", sizeof(smSysInfo->d.NVMeData.d.SCU_CMD.UserPw));
        D_PRINTF("Maddr = %x, Uaddr = %x\n", smSysInfo->d.NVMeData.d.SCU_CMD.MasterPw, smSysInfo->d.NVMeData.d.SCU_CMD.UserPw);
        D_PRINTF("State = %x\n", gNvmeIfMgr.SecurityMgr.SecurityState);
        D_PRINTF("Prepare = %x\n", gNvmeIfMgr.SecurityMgr.ErasePrepare);
        D_PRINTF("Cnt = %x\n", gNvmeIfMgr.SecurityMgr.PasswordCnt);
        D_PRINTF("Retry Over = %x\n", gNvmeIfMgr.SecurityMgr.RetryExpired);
        D_PRINTF("U-RevCode = %x\n", smSysInfo->d.NVMeData.d.SCU_CMD.MasterRv);
        D_PRINTF("Level = %x\n", smSysInfo->d.NVMeData.d.SCU_CMD.Level);

        for (idx = 0; idx < SEC_PW_SIZE; idx ++)
        {
            D_PRINTF("M-PW[%d] = %x\n", idx, smSysInfo->d.NVMeData.d.SCU_CMD.MasterPw[idx]);
        }

        for (idx = 0; idx < SEC_PW_SIZE; idx ++)
        {
            D_PRINTF("U-PW[%d] = %x\n", idx, smSysInfo->d.NVMeData.d.SCU_CMD.UserPw[idx]);
        }
    }
}*/

//-----------------------------------------------------------------------------
/**
    Dump security info

    @param[in]  MasterType     MASTER_PW or USER_PW
    @param[in]  UnlockCase     TRUE (Lock CMD case), FALSE (Other case)

    @return TRUE(same) FALSE(different)
**/
//-----------------------------------------------------------------------------
BOOL SATA_Security_PwCompare(BOOL MasterType, BOOL UnlockCase)
{
    U8 idx, result;
    result = TRUE;

    for (idx = 0; idx < SEC_PW_SIZE; idx ++)
    {
        if (MasterType)
        {
            if (pPwData->PW[idx] != smSysInfo->d.NVMeData.d.SCU_CMD.MasterPw[idx])
            {
                result = FALSE;
            }
        }
        else
        {
            if (pPwData->PW[idx] != smSysInfo->d.NVMeData.d.SCU_CMD.UserPw[idx])
            {
                result = FALSE;
            }
        }
        if (result != TRUE)
        {
            break;
        }
    }

    if ((UnlockCase) && (gNvmeIfMgr.SecurityMgr.SecurityState == SCU_SEC4))  //SEC1 Step 11
    {
        if (gNvmeIfMgr.SecurityMgr.PasswordCnt != 0)
        {
            gNvmeIfMgr.SecurityMgr.PasswordCnt --;
        }

        if (gNvmeIfMgr.SecurityMgr.PasswordCnt == 0)
        {
            gNvmeIfMgr.SecurityMgr.RetryExpired = TRUE;
        }
    }
    return result;
}

//-----------------------------------------------------------------------------
/**
    Security state set

    @param[in]  state

**/
//-----------------------------------------------------------------------------
//void SATA_Security_StateSet(U8 state)
//{
//    gSataIfMgr.SecurityMgr.SecurityState = state;
//}

//-----------------------------------------------------------------------------
/**
    Media Trim Request

    @param[in]  pHcmdMsg     Host command queue pointer
**/
//-----------------------------------------------------------------------------
void MediaEraseAllRequest(HcmdQ_t* pHcmdQ)
{
    uint32_t nsId0;

#if _TCG_ != TCG_NONE
    TcgKeepGroupDataBridge(4);
    TcgEepOperatedBridge(0);
    TcgEepBackupBridge();
    TcgPsidBackupBridge();
#endif

    pHcmdQ->attr.all |= ATTR_OVERLAPPED;

#if _TCG_ != TCG_NONE
    TcgEepRestoreBridge();
    TcgEepOperatedBridge(2);
    TcgPsidRestoreBridge();
    TcgPreformatAndInit(TRUE);
#endif

    for (nsId0 = 0; nsId0 < NVME_NUM_OF_NAMESPACES; nsId0++)
    {
        if (gNvmeIfMgr.ns[nsId0].Create)
        {
            Trim_All(pHcmdQ, nsId0);
        }
    }

    Purge_Center(PURGE_CORE_UNCT);
}

//-----------------------------------------------------------------------------
/**
    Media Trim request done.

    @param[in]  pHcmdQ     Host command queue pointer
**/
//-----------------------------------------------------------------------------
void MediaEraseAllRequestDone(HcmdQ_t* pHcmdQ)
{
    uint8_t CmdCode;

    CmdCode = NvmeCmd_GetCdbPointer(pHcmdQ)->OPC;

    if (CmdCode == cSecuritySend)
    {
        gNvmeIfMgr.SecurityMgr.SecurityState = SCU_SEC1;
        MEM_CLR(smSysInfo->d.NVMeData.d.SCU_CMD.UserPw, sizeof(smSysInfo->d.NVMeData.d.SCU_CMD.UserPw));
        smSysInfo->d.NVMeData.d.SCU_CMD.PwEnable = FALSE;
        SyIn_Synchronize(SI_AREA_NVME, SYSINFO_WRITE, SI_SYNC_BY_NVME);
    }
}

//-----------------------------------------------------------------------------
/**
    Set Pass word command handler (0xF1)

    @param[in]  pHcmdQ     Host command queue pointer
**/
//-----------------------------------------------------------------------------
static Error_t SATA_CMD_SetPassword(HcmdQ_t* pHcmdQ)
{
    //U8 idx;
    U8 store = FALSE;
    //U32 XferErr = FALSE;


    gErrorSecutitySend = SATA_SecurityFrozen_CmdCheck();

    if (gErrorSecutitySend == cEcNoError)
    {

        //if (XferErr == FALSE)
        {
            if (pPwData->Identify == TRUE)                // Master Password
            {
                D_PRINTF("Master Password\n");

                MEM_COPY(smSysInfo->d.NVMeData.d.SCU_CMD.MasterPw, pPwData->PW, sizeof(smSysInfo->d.NVMeData.d.SCU_CMD.MasterPw));
                if ((pPwData->RevCode > 0x0000) && (pPwData->RevCode < 0xFFFF))
                {
                    smSysInfo->d.NVMeData.d.SCU_CMD.MasterRv = pPwData->RevCode;
                }
                store = TRUE;

                #if (_CUSTOMER_ == CUSTOMER_HP)
                if ((pPwData->RevCode == 0x0000)||(pPwData->RevCode == 0xFFFF))  //HP special feature
                {
                    gErrorSecutitySend = cEcInvalidCommand;
                    store              = FALSE;
                }
                #endif
            }
            else                                                        // User Password
            {
                D_PRINTF("User Password\n");

                MEM_COPY(smSysInfo->d.NVMeData.d.SCU_CMD.UserPw, pPwData->PW, sizeof(smSysInfo->d.NVMeData.d.SCU_CMD.UserPw));
                smSysInfo->d.NVMeData.d.SCU_CMD.Level = (pPwData->SecLevel) ? MAX_LEVEL : HIGH_LEVEL;
                smSysInfo->d.NVMeData.d.SCU_CMD.PwEnable = TRUE;
                gNvmeIfMgr.SecurityMgr.SecurityState = SCU_SEC5;
                store = TRUE;
            }
            D_PRINTF("SecurityState=%x\n",gNvmeIfMgr.SecurityMgr.SecurityState);
        }
    }

    if (store == TRUE)
    {
        SyIn_Synchronize(SI_AREA_NVME, SYSINFO_WRITE, SI_SYNC_BY_NVME);
    }

    return gErrorSecutitySend;
}

//-----------------------------------------------------------------------------
/**
    Security unlock command handler (0xF2)

    @param[in]  pHcmdQ     Host command queue pointer
**/
//-----------------------------------------------------------------------------
static Error_t SATA_CMD_Unlock(HcmdQ_t* pHcmdQ)
{
    U8 errorcode = 0;
    //U32 XferErr = FALSE;


    gErrorSecutitySend = SATA_SecurityFrozen_CmdCheck();
/*#if TCG_SW
    if (error==cEcInvalidCommand){
        return;
    }
#endif*/

    if (gNvmeIfMgr.SecurityMgr.RetryExpired == TRUE)
    {
        gErrorSecutitySend = cEcSecurityLock;
        errorcode         |= B_1;
    }

    if (gErrorSecutitySend == cEcNoError)
    {

        //if (XferErr == FALSE)
        {
            if (pPwData->Identify)                // Check Master PW
            {
                if (SATA_Security_PwCompare(MASTER_PW, TRUE) == FALSE)
                {
                    gErrorSecutitySend = cEcSecurityLock;
                    errorcode         |= B_2;
                }

                if (smSysInfo->d.NVMeData.d.SCU_CMD.PwEnable == TRUE)  //Drive shall return OK when master key used  //SEC1 Step 12
                {
                    if (smSysInfo->d.NVMeData.d.SCU_CMD.Level == MAX_LEVEL)
                    {
                        gErrorSecutitySend = cEcSecurityLock;
                        errorcode         |= B_3;
                    }
                }
            }
            else                                                // Check User PW
            {
                if (gNvmeIfMgr.SecurityMgr.SecurityState == SCU_SEC1)  //SEC1 Step 11
                {
                    gErrorSecutitySend = cEcSecurityLock;
                    errorcode         |= B_0;
                }

                if (SATA_Security_PwCompare(USER_PW, TRUE) == FALSE)
                {
                    gErrorSecutitySend = cEcSecurityLock;
                    errorcode         |= B_4;
                }
            }
        }
    }

    //SATA_CMD_OpErrDump(errorcode);

    if (gErrorSecutitySend == cEcNoError)
    {
        gNvmeIfMgr.SecurityMgr.SecurityState = SCU_SEC5;

        D_PRINTF("SecurityState=%x\n",gNvmeIfMgr.SecurityMgr.SecurityState);

    }else{
        D_PRINTF("errorcode=%x\n",errorcode);
    }
    return gErrorSecutitySend;
}

//-----------------------------------------------------------------------------
/**
    Erase prepare command handler (0xF3)

    @param[in]  pHcmdQ     Host command queue pointer
**/
//-----------------------------------------------------------------------------
static Error_t SATA_CMD_ErasePrepare()
{
    gErrorSecutitySend = SATA_SecurityFrozen_CmdCheck();

    #if (_CUSTOMER_ == CUSTOMER_HP)
    if (gNvmeIfMgr.SecurityMgr.SecurityState == SCU_SEC1)  //HP special feature
    {
        gErrorSecutitySend = cEcInvalidCommand;
    }
    #endif

    //if (error == cEcNoError)
    //{
    //    gSataIfMgr.SecurityMgr.ErasePrepare = TRUE;
    //}
    return gErrorSecutitySend;
}

//-----------------------------------------------------------------------------
/**
    Security erase unit command handler (0xF4)

    @param[in]  pHcmdQ     Host command queue pointer
**/
//-----------------------------------------------------------------------------
static Error_t SATA_CMD_EraseUnit(HcmdQ_t* pHcmdQ)
{
    U8 eraseall = FALSE;
    U8 errorcode = 0;
    U32 XferErr = FALSE;

    gErrorSecutitySend = SATA_SecurityFrozen_CmdCheck();

    // Check Erase prepare
    if (gNvmeIfMgr.SecurityMgr.ErasePrepare == FALSE)
    {
        gErrorSecutitySend = cEcSecurityLock;
        errorcode         |= B_0;
    }

    if (gNvmeIfMgr.LastNvmeCmd != cSecuritySend)
    {
        gErrorSecutitySend = cEcSecurityLock;
        errorcode         |= B_5;
    }

    if (gNvmeIfMgr.SecurityMgr.RetryExpired == TRUE)  //Drive shall return aborted when SEC_COUNT_EXPIRED
    {
        gErrorSecutitySend = cEcSecurityLock;
        errorcode         |= B_6;
    }

    if (gErrorSecutitySend == cEcNoError)
    {

        if (XferErr == FALSE)
        {
            if (pPwData->Identify)                // Check Master PW
            {
                if (SATA_Security_PwCompare(MASTER_PW, FALSE) == FALSE)
                {
                    gErrorSecutitySend = cEcSecurityLock;
                    errorcode         |= B_1;
                }
            }
            else                                                // Check User PW
            {
                if (gNvmeIfMgr.SecurityMgr.SecurityState == SCU_SEC1)
                {
                    gErrorSecutitySend = cEcSecurityLock;
                    errorcode         |= B_2;
                }

                if (SATA_Security_PwCompare(USER_PW, FALSE) == FALSE)
                {
                    gErrorSecutitySend = cEcSecurityLock;
                    errorcode         |= B_3;
                }
            }

            if (gErrorSecutitySend == cEcNoError)
            {
                eraseall = TRUE;
            }
        }
        else
        {
            gErrorSecutitySend = cEcInvalidCommand;
            errorcode |= B_4;
        }
    }

    if (eraseall)
    {
#if _TCG_!=TCG_NONE
        // If the drive is a Self-Encrypting Device (SED) and if EN_ER = 1
        if (pPwData->SecLevel)
        {
            TcgUpdateGlobalKey();

            gNvmeIfMgr.SecurityMgr.SecurityState = SCU_SEC1;
            MEM_CLR(smSysInfo->d.NVMeData.d.SCU_CMD.UserPw, sizeof(smSysInfo->d.NVMeData.d.SCU_CMD.UserPw));
            smSysInfo->d.NVMeData.d.SCU_CMD.PwEnable = FALSE;
            SyIn_Synchronize(SI_AREA_NVME, SYSINFO_WRITE, SI_SYNC_BY_NVME);
        }
        else
#endif
        {
            MediaEraseAllRequest(pHcmdQ);
        }
    }
    else
    {
        //SATA_CMD_OpErrDump(errorcode);

        D_PRINTF("errorcode=%x\n",errorcode);

    }
    return gErrorSecutitySend;
}

//-----------------------------------------------------------------------------
/**
    Freeze lock command handler (0xF5)

    @param[in]  pHcmdQ     Host command queue pointer
**/
//-----------------------------------------------------------------------------
static Error_t SATA_CMD_FreezeLock()
{
    U8 state;

/*#if TCG_SW
    if(mTcgStatus&TCG_ACTIVATED){
        gErrorSecutitySend=cEcInvalidCommand;
        return;
    }
#endif*/
    state = gNvmeIfMgr.SecurityMgr.SecurityState;
    switch (state)
    {
        case SCU_SEC1:
            state = SCU_SEC2;
            break;
        case SCU_SEC5:
            state = SCU_SEC6;
            break;
        case SCU_SEC2:
        case SCU_SEC6:
            break;
        default :
            break;
    }
    gNvmeIfMgr.SecurityMgr.SecurityState = state;


    D_PRINTF("SecurityState=%x\n",gNvmeIfMgr.SecurityMgr.SecurityState);
    return gErrorSecutitySend;

}

//-----------------------------------------------------------------------------
/**
    Disable pass word command handler (0xF6)

    @param[in]  pHcmdQ     Host command queue pointer
**/
//-----------------------------------------------------------------------------
static Error_t SATA_CMD_DisablePassword(HcmdQ_t* pHcmdQ)
{
    U8 store = FALSE;
    U8 errorcode = 0;
    //U32 XferErr = FALSE;

    gErrorSecutitySend = SATA_SecurityFrozen_CmdCheck();
    if (gErrorSecutitySend == cEcNoError)
    {

        //if (XferErr == FALSE)
        {
            if (pPwData->Identify)                // Check Master PW
            {
                if (smSysInfo->d.NVMeData.d.SCU_CMD.PwEnable == TRUE)  //Drive shall return OK when master key used  //SEC1 Step 5
                {
                    if (smSysInfo->d.NVMeData.d.SCU_CMD.Level == MAX_LEVEL)
                    {
                        gErrorSecutitySend = cEcSecurityLock;
                        errorcode         |= B_0;
                    }
                }

                if (SATA_Security_PwCompare(MASTER_PW, FALSE) == FALSE)
                {
                    gErrorSecutitySend = cEcSecurityLock;
                    errorcode         |= B_1;
                }
            }
            else                                                // Check User PW
            {
                if (smSysInfo->d.NVMeData.d.SCU_CMD.PwEnable == FALSE)
                {
                    gErrorSecutitySend = cEcSecurityLock;
                    errorcode         |= B_2;
                }

                if (SATA_Security_PwCompare(USER_PW, FALSE) == FALSE)
                {
                    gErrorSecutitySend = cEcSecurityLock;
                    errorcode         |= B_3;
                }
            }

            if (gErrorSecutitySend == cEcNoError)
            {
                gNvmeIfMgr.SecurityMgr.SecurityState= SCU_SEC1;
                D_PRINTF("SecurityState=%02x, Identify=%02x, \n",gNvmeIfMgr.SecurityMgr.SecurityState,pPwData->Identify);

                // For User PW case
                //if (pPwData->ControlWord.b.Identify == 0)  //SEC5 Step 2
                {
                    MEM_CLR(smSysInfo->d.NVMeData.d.SCU_CMD.UserPw, sizeof(smSysInfo->d.NVMeData.d.SCU_CMD.UserPw));
                    smSysInfo->d.NVMeData.d.SCU_CMD.PwEnable = FALSE;
                    store = TRUE;
                }
            }
        }
    }

    //SATA_CMD_OpErrDump(errorcode);
    if (gErrorSecutitySend){
        D_PRINTF("errorcode=%x\n",errorcode);
    }

    if (store == TRUE)
    {
        SyIn_Synchronize(SI_AREA_NVME, SYSINFO_WRITE, SI_SYNC_BY_NVME);
    }

    return gErrorSecutitySend;
}

//-----------------------------------------------------------------------------
/**
    ATA_SecuritySend

    @param[in]
**/
//-----------------------------------------------------------------------------
static void ATA_SecuritySend(tSATA_PW_DATA* pBuff, HcmdQ_t* pHcmdQ)
{
    NvmeAdminSecuritySendCommand_t* pCdb;

    pCdb = (NvmeAdminSecuritySendCommand_t *)NvmeCmd_GetCdbPointer(pHcmdQ);
    pPwData = (tSATA_PW_DATA *)pBuff;

    gErrorSecutitySend = cEcNoError;

    if ((pCdb->SPSP == cSATA_CMD_SetPassword) || (pCdb->SPSP == cSATA_CMD_Unlock) ||
        (pCdb->SPSP == cSATA_CMD_EraseUnit) || (pCdb->SPSP == cSATA_CMD_DisablePassword))
    {
        if (pCdb->TL != 0x24)
        {
            pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
            gNvmeIfMgr.lastErrorBitOffset = 0;
            gNvmeIfMgr.lastErrorByteOffset = ERR_DW11_OFFSET;
            return;
        }
    }

    switch(pCdb->SPSP)
    {
        case cSATA_CMD_SetPassword:
            gErrorSecutitySend = SATA_CMD_SetPassword(pHcmdQ);
            break;
        case cSATA_CMD_Unlock:
            gErrorSecutitySend = SATA_CMD_Unlock(pHcmdQ);
            break;
        case cSATA_CMD_ErasePrepare:
            gErrorSecutitySend = SATA_CMD_ErasePrepare();
            break;
        case cSATA_CMD_EraseUnit:
            gErrorSecutitySend = SATA_CMD_EraseUnit(pHcmdQ);
            break;
        case cSATA_CMD_FreezeLock:
            gErrorSecutitySend = SATA_CMD_FreezeLock();
            break;
         case cSATA_CMD_DisablePassword:
            gErrorSecutitySend = SATA_CMD_DisablePassword(pHcmdQ);
            break;
        default:
            gErrorSecutitySend = cEcInvalidCommand;
            break;
    }

    gNvmeIfMgr.SecurityMgr.ErasePrepare = FALSE;
    if (gErrorSecutitySend)
    {
        D_PRINTF("ATA_SecuritySend  error=%08x\n", gErrorSecutitySend);

        pHcmdQ->error = MK_NVME_ERROR_CODE(cInvalidFieldIncommand, cGenericCommandStatus, cNoMoreStatusInformation, cDoNotRetry);
        gNvmeIfMgr.lastErrorBitOffset = 0;
        gNvmeIfMgr.lastErrorByteOffset = ERR_DW10_OFFSET + 1;
    }
    else if (pCdb->SPSP == cSATA_CMD_ErasePrepare)
    {
        gNvmeIfMgr.SecurityMgr.ErasePrepare = TRUE;
    }

}
