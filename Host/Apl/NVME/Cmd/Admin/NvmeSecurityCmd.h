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

// REG

// HAL

// APL
#include "NvmeAdminCommands.h"

// Code Section
//-----------------------------------------------------------------------------
//  Constants definitions:
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

typedef enum
{
    cSECURITY_INFO  = 0x00,  ///< Security protocol information
    cTCG_PID_01     = 0x01,
    cTCG_PID_02     = 0x02,

    cNVME_PID_EA    = 0xEA,

    cIEEE1667_PID   = 0xEE,
    cATA_SECURITY   = 0xEF,  ///< ATA Device Server Password Security
} SecuritySendCmdSECP_t;

typedef enum
{
    cSATA_CMD_SecurityInfo    = 0x0000,
    cSATA_CMD_SetPassword     = 0x0001,
    cSATA_CMD_Unlock          = 0x0002,
    cSATA_CMD_ErasePrepare    = 0x0003,
    cSATA_CMD_EraseUnit       = 0x0004,
    cSATA_CMD_FreezeLock      = 0x0005,
    cSATA_CMD_DisablePassword = 0x0006,
} SecurityCmdSPSP_t;

// Security State define
#define SCU_SEC0                            0   ///< SEC0 : Power Down / No PW
#define SCU_SEC1                            1   ///< SEC1 : Power On / No PW / Unlock / No Frozen
#define SCU_SEC2                            2   ///< SEC2 : Power On / No PW / Unlock / Frozen
#define SCU_SEC3                            3   ///< SEC3 : Power Down PW
#define SCU_SEC4                            4   ///< SEC4 : Power On / PW / Lock / No Frozen
#define SCU_SEC5                            5   ///< SEC5 : Power On / PW / Unlock / No Frozen
#define SCU_SEC6                            6   ///< SEC6 : Power On / PW / Unlock / Frozen
#define SEC_PW_SIZE                         16  ///< 16 Word = 32 Bytes
#define SEC_PW_MAX_RETRY_CNT                5   ///< Max PW retry count
#define MAX_LEVEL                           1   ///< PW level
#define HIGH_LEVEL                          0   ///< PW level
#define MASTER_PW                           1   ///< PW Type
#define USER_PW                             0   ///< PW Type
#define SEC_ERASE_TIME                      1   // 3
#define SEC_ENH_ERASE_TIME                  1   // 3


#define NVME_SECURITY_SEND                  0x81
#define NVME_SECURITY_RECEIVE               0x82
//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef  struct
{
    AdminCommandCommon_t DW9_0;    ///< DW9..0

    uint32_t NSSF:8;          ///< NVMe Security Specific Field (DW10 bits[07:00])
    uint32_t SPSP:16;         ///< SP Specific (DW10 bits[23:08])
    uint32_t SECP:8;          ///< Security Protocol (DW10 bits[31:24])

    uint32_t AL;              ///< Allocation Length (DW11)
    uint32_t reserved12[4];   ///< DW15..12

} NvmeAdminSecurityReceiveCommand_t;
COMPILE_ASSERT(sizeof(NvmeAdminSecurityReceiveCommand_t) == 64, "NVMe command size shall be 64");


typedef  struct
{
    AdminCommandCommon_t DW9_0;    ///< DW9..0

    uint32_t NSSF:8;          ///< NVMe Security Specific Field (DW10 bits[07:00])
    uint32_t SPSP:16;         ///< SP Specific (DW10 bits[23:08])
    uint32_t SECP:8;          ///< Security Protocol (DW10 bits[31:24])

    uint32_t TL;              ///< Transfer Length (DW11)
    uint32_t reserved12[4];   ///< DW15..12

} NvmeAdminSecuritySendCommand_t;
COMPILE_ASSERT(sizeof(NvmeAdminSecuritySendCommand_t) == 64, "NVMe command size shall be 64");

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
extern void SATA_Security_Init(InitBootMode_t initMode);
extern void MediaEraseAllRequestDone(HcmdQ_t* pHcmdQ);
extern Error_t Nvme_CMD_CheckSecurity(uint8_t Admin, uint8_t CmdCode);

