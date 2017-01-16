#pragma once
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
//! @brief Firmware loader and burner.
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
typedef enum
{
    FW_MAIN1_VALID = 0,
    FW_MAIN2_VALID,
    FW_MAIN_BK_VALID
}fwValid_t;

typedef enum
{
    FW_NO_ERR = 0x0,
    FW_HEAD_ERR,
    FW_HEAD_BK_ERR,
    FW_MIAN1_ERR,
    FW_MIAN2_ERR,
    FW_MAIN_BK_ERR,
    FW_CHECK_NUM,
    FW_UNEXPECT_NUM = 0xFF
}fwError_t;

enum
{
    UPDATE_FW_UART = 0xD0,
    UPDATE_FW_NVME,
    UPDATE_FW_NUM
};

#define FW_BK_MECH       ENABLE  //Fw Backup Mechanism
#define FW_MAIN_BK_NUM   (NVME_NUM_OF_FW_SLOTS + 1)
#define FW_ROTATION_NUM  2

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

// @brief Download Firmware use region
#define DFW_CHECK_FWHEADER(P)   (*((uint32_t *)(uint8_t *)(P + DFW_FW_CSS_SIZE)) != DFW_FW_HEADRER_TAG)    ? B_0 : 0
#define DFW_CHECK_BHEADER(P)    (P[DFW_FW_HEAD_SIZE] != DFW_HEADER_ID)                                     ? B_1 : 0
#define DFW_CHECK_DRAM(P)       (P[DFW_FW_CSS_SIZE + 16] != _DRAM_   && P[DFW_FW_CSS_SIZE + 16] != 0xFF)   ? B_2 : 0
#define DFW_CHECK_VENDOR(P)     (P[DFW_FW_CSS_SIZE + 17] != _VENDOR_ && P[DFW_FW_CSS_SIZE + 17] != 0xFF)   ? B_3 : 0
#define DFW_CHECK_NAND(P)       (P[DFW_FW_CSS_SIZE + 18] != _NAND_   && P[DFW_FW_CSS_SIZE + 18] != 0xFF)   ? B_4 : 0
#define DFW_CHECK_CRC(R,C)      (R != C) ? B_5 : 0

// Do not or all error status, just do all macro if any first error it will be return
#define DFW_GET_ERROR_ST(P,R,C) DFW_CHECK_FWHEADER(P) | DFW_CHECK_BHEADER(P) | DFW_CHECK_DRAM(P) | \
                                DFW_CHECK_VENDOR(P)   | DFW_CHECK_NAND(P)    | DFW_CHECK_CRC(R,C)

#define DFW_GET_FWNUM(M)        ((((M).tag >> 16 & 0xFF) - 0x30) * 10 + ((M).tag >> 24 & 0xFF) - 0x30)

#define DFW_PRINTF(F,args...)   D_PRINTF("[DFW ] "F, ##args)         ///< VSC message print out
//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef struct
{
    uint32_t  address;
    uint32_t  size;
    uint8_t   select;
    uint8_t   reserved[15];
    uint8_t*  prevslot;
    uint8_t*  nextslot;
}DfwInfo_t;

/// @brief Downloadable Firmware header structure
typedef struct
{
    uint8_t  id;              ///< identifier (0x5A=MASK ROM Loader, 0xC5=Firmware Loader)
    uint8_t  cpu;             ///< target CPU
    uint16_t resereved02;
    uint32_t srcBlockSize;   ///< code size + 1
    uint32_t packetSize;     ///< code size
    uint32_t srcAddress;     ///< source address, copy from
    uint32_t destAddress;    ///< destination address, copy to
    uint32_t execAddress;    ///< after copy, execution address
    uint8_t  reserved24[7];
    uint8_t  checkSum;
} DfwBlkHeader_t;

typedef struct
{
    uint8_t     id;              ///< identifier
    uint8_t     reserved1[7];
    uint8_t     fw_revision[8];
    uint32_t    historyNum;
    uint32_t    mfw_crc;
    uint8_t     reserved2[8];
} DfwImageTag_t;
COMPILE_ASSERT(sizeof(DfwImageTag_t)== 32, "shall be 32 bytes size");

typedef struct
{
    uint8_t   id;              ///< identifier
    uint8_t   select;          ///< FW select
    uint8_t   reserved[30];
} DfwSelectTag_t;
COMPILE_ASSERT(sizeof(DfwSelectTag_t)== 32, "shall be 32 bytes size");

typedef struct
{
    uint32_t  tag;
    uint16_t  version;
    uint8_t   sec_cnt;
    uint8_t   reserved[24];
    uint8_t   checkSum;
} DfwDescript_t;
COMPILE_ASSERT(sizeof(DfwDescript_t)== 32, "shall be 32 bytes size");

typedef struct
{
    uint32_t  tag;
    uint32_t  offset;
    uint32_t  size;
    uint32_t  update;
} DfwModuleInfo_t;

//----------------------------------------------------
// FW Header(   0~1023 byte): CSS(bin)->Selection(SPI)
// FW Header(1024~4095 byte): FW Module Section
//----------------------------------------------------
typedef struct
{
    DfwSelectTag_t DfwSelectTag;
    DfwDescript_t  DfwDescript;
} DfwHeader_t;
//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern DfwInfo_t           gLdFw_FwInfo[FW_MAIN_BK_NUM];
extern DfwImageTag_t       gLdFw_ImgTag[FW_MAIN_BK_NUM];
extern DfwModuleInfo_t     gLdFw_module;

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t AplLoadFirmware_OneTimeInit(InitBootMode_t initMode);
Error_t AplLoadFirmware_SaveToSPI(uint32_t fw_addr, uint32_t fw_size, uint8_t* pBuffer, uint16_t byteMode, uint32_t retry);
void    AplLoadFirmware_GetFwInfo(uint32_t sflashAddress, uint8_t fwSetValid, DfwInfo_t* pfwInfo);
