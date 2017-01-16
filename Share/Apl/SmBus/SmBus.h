#pragma once
// ============================================================================
//
// ==        Copyright (c) LITEON Corporation  -  All rights reserved        ==
/*
    This computer program contains confidential and proprietary information,
    and may NOT be reproduced or transmitted, in whole or in part, in any form,
    or by any means electronic, mechanical, photo-optical, or otherwise, and
    may NOT be translated into another language without the express written
    permission from LITEON Corporation.
*/
// ==        Copyright (c) LITEON Corporation  -  All rights reserved        ==
//
//=============================================================================

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define CO_SUPPORT_SMBUS            FALSE
#define SMBUS_CODE_ON_DRAM          FALSE

// SmBus VPD Info
#define SMBUS_VPD_PORT                  I2C_PORT_0
#define SMBUS_VPD_ADDRESS               I2C_ADDRESS_ATMEL_EE24C01D
#define SMBUS_VPD_PAYLOAD_SIZE          sizeof(VpdPayload_t)
#define SMBUS_VPD_COMPARE_SIZE          128
#define SMBUS_VPD_WRITE_EN_PIN          4
#define SMBUS_VPD_ERROR_RETRY           5

// SmBus Basic Managerment
#define SMBUS_BM_PORT                   I2C_PORT_0
#define SMBUS_BM_ADDRESS                I2C_ADDRESS_BASIC

#define SMBUS_BM_READ_START_BYTE        0xFF
#define SMBUS_BM_CRC_START_BYTE         3

#define SMBUS_BM_LENGTH_BASIC           0x20
#define SMBUS_BM_LENGTH_COMPLETE        0xFF
#define SMBUS_BM_LENGTH_BASIC_CMD0      6
#define SMBUS_BM_LENGTH_BASIC_CMD10     22

#define SMBUS_BM_ISR_PUT_MAX_CNT        1
#define SMBUS_BM_TEMP_GP_VAR            40

//-----------------------------------------------------------------------------
//  Private data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief SmBus VPD Operation
enum
{
    SMBUS_CENTER_VPD_INJECT = 0x00,
    SMBUS_CENTER_VPD_READ,
    SMBUS_CENTER_VPD_WRITE,
    SMBUS_CENTER_VPD_NUM
};
/// @brief SmBus Basic Management Command
enum
{
    SMBUS_CMD_CODE_00       =  0x00,
    SMBUS_CMD_CODE_08       =  0x08,
    SMBUS_CMD_CODE_VENDOR   =  0x20
};

/// @brief SmBus Slave Read Status
enum
{
    SMBUS_READ_PROCESS      =  0x0,
    SMBUS_READ_COMPLETE     =  0x1
};

// SmBus - VPD
typedef union
{
    U8 all[256];
    struct
    {
        U8 PCIeClassCode[3];        /// byte[2:0]
        U8 PCIeVId[2];              /// byte[4:3]
        U8 NVMeSN[20];              /// byte[24:5]
        U8 NVMeMN[40];              /// byte[64:25]
        U8 MaxLinkSpeed;            /// byte[65]
        U8 MaxLinkWidth;            /// byte[66]
        U8 Reserved67[2];           /// byte[68:67]
        U8 PowerInit;               /// byte[69]
        U8 Reserved70[2];           /// byte[71:70]
        U8 PowerMaxReq;             /// byte[72]
        U8 Reserved73[2];           /// byte[74:73]
        U8 CpListPoint[2];          /// byte[76:75]
        U8 Reserved77[179];         /// byte[255:77]
    } d;
}VpdPayload_t;

// SmBus - NVMe Basic Management Command
// Status Flags
typedef union
{
    U8 all;
    struct
    {
        U8 Bit0_2           : 3;
        U8 PCIeLinkActive   : 1;
        U8 ResetNotRequired : 1;
        U8 DriveFunctional  : 1;
        U8 PoweredUp        : 1;
        U8 Arbitration      : 1;
    } b;
}BmStatusFlags_t;

// SMART Warnings
typedef union
{
    U8 all;
    struct
    {
        U8 CriticalWarnSpare        : 1;
        U8 CriticalWarnTemperature  : 1;
        U8 CriticalWarnReliability  : 1;
        U8 CriticalWarnMedia        : 1;
        U8 CriticalWarnVolatile     : 1;
        U8 Bit5_7                   : 3;
    } b;
}BmSmartWarnings_t;

typedef union
{
    U8  all[8];
    struct
    {
        U8                  Length;
        BmStatusFlags_t     SFLGS;
        BmSmartWarnings_t   SmartWarnings;
        U8                  CTemp;
        U8                  PDLU;
        U8                  Reserved[2];
        U8                  PEC;
    } d;
}BmCmd0_t;

typedef union
{
    U8 all[24];
    struct
    {
        U8 Length;
        U8 PCIeVId[2];
        U8 NVMeSN[20];
        U8 PEC;
    } d;
}BmCmd8_t;

typedef union
{
    U8 all[32];
    struct
    {
        BmCmd0_t Cmd0;
        BmCmd8_t Cmd8;
    } d;
} BasicManagement_t;

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define SMBUS_VPD_WRITE_PROTECT(P, E)   {HalGpio_OutputEnable(BIT(P)); HalGpio_WritePin(P, E);}

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
extern Error_t  AplSmbus_OneTimeInitVpd(InitBootMode_t initMode);
extern Error_t  AplSmbus_OneTimeInitBm(InitBootMode_t initMode);
extern Error_t  AplSmBus_VpdInject(I2cDevice_t* port);
extern Error_t  AplSmBus_VpdWrite(I2cDevice_t* i2c, U8* data, U16 offset, U16 len);
extern Error_t  AplSmBus_VpdRead(I2cDevice_t* i2c, U8* data, U16 offset, U16 len);
extern Error_t  AplSmBus_Center(U8 OpCode, U8* pBuffer, U32 Offset, U32 Len);
extern void     AplSmBus_BasicVariableInit(U8 oneTimeInitFalg);

/* End of File */
