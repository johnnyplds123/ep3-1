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
//
//! @file
//! @brief SmBus code
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"
#include "WorkArounds.h"

#include "RegI2cIf.h"
#include "RegPcie.h"
#include "RegPciePhy.h"
#include "RegPcieCore.h"
#include "RegNvmeController.h"

#include "HalCpu.h"
#if (_CPU_ == CPU_ID0)
#include "HalPcieLink.h"
#endif

#include "MemIo.h"
#include "PLP.h"
#include "SysInfo.h"

#include "SmBus.h"

#if (SMBUS_CODE_ON_DRAM == FALSE)
#include "FastSection.h"
#else
#include "NormSection.h"
#endif
//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function proto-type definitions:
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//
//  VARIABLE DECLARATION WITH INITIAL VALUE DOESN'T WORK !!!
//  UNINTIALIZED VARIABLE DECLARATION ONLY !!!
//----------------------------------------------------------------------------
#if (CO_SUPPORT_SMBUS == TRUE && _CPU_ != CPU_ID1)

#if (_CPU_ == CPU_ID0)
static   VpdPayload_t*      gpVpdPayload;
#else

// VPD
static   VpdPayload_t       gVpdPayload;
static   I2cDevice_t        gVpdI2cDevice;

// Basic Management
volatile I2c_t*             gpBmI2c;
static   BasicManagement_t  gBasicManagement;
static   U8                 gSmBusIndex = 0;
static   U8                 gVpdCompareBuffer[SMBUS_VPD_COMPARE_SIZE];
#endif

//-----------------------------------------------------------------------------
//  Imported data proto-type without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function proto-type without header include
//-----------------------------------------------------------------------------
#if (_CPU_ == CPU_ID2)
static void SmBus_IrqHandler(U32 parameter);
static void SmBus_IrqEnableBm(void);
static U8   SmBus_Crc8(U8* bdata, U8 len);
#endif
//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
    Smbus Vpd One Time Init

    @param[in] nothing

**/
//-----------------------------------------------------------------------------
Error_t AplSmbus_OneTimeInitVpd(InitBootMode_t initMode)
{
#if (_CPU_ == CPU_ID2)
    I2C_DEVICE_LOCATION(gVpdI2cDevice, SMBUS_VPD_PORT, SMBUS_VPD_ADDRESS);
    MEM_SET(&gVpdPayload, 0x00, sizeof(VpdPayload_t));

    #if (SMBUS_CODE_ON_DRAM == FALSE)
    smSmBusVpdInfo = ((U32)&gVpdPayload | AXI_B0TCM_CPU2_ADDR);
    #else
    smSmBusVpdInfo = (U32)&gVpdPayload;
    #endif

#else
    gpVpdPayload                     = (VpdPayload_t*)smSmBusVpdInfo;
    gpVpdPayload->d.MaxLinkSpeed     = PCIE_LIMIT_SPEED;
    gpVpdPayload->d.MaxLinkWidth     = PCIE_LIMIT_WIDTH;

    gpVpdPayload->d.PCIeClassCode[0] = NVME_CLASS_CODE >> 8  & 0xFF;
    gpVpdPayload->d.PCIeClassCode[1] = NVME_CLASS_CODE >> 16 & 0xFF;
    gpVpdPayload->d.PCIeClassCode[2] = NVME_CLASS_CODE >> 24 & 0xFF;

    MEM_COPY(gpVpdPayload->d.NVMeMN, LiteOnModelName, sizeof(gpVpdPayload->d.NVMeMN));
#endif

    return cEcNoError;
}

#if (_CPU_ == CPU_ID2)
//-----------------------------------------------------------------------------
/**
    Smbus Basic Managerment One Time Init

    @param[in] nothing

**/
//-----------------------------------------------------------------------------
Error_t AplSmbus_OneTimeInitBm(InitBootMode_t initMode)
{
    gpBmI2c = (SMBUS_BM_PORT == I2C_PORT_0) ? &rI2c0 : &rI2c1;

    AplSmBus_BasicVariableInit(TRUE);
    SmBus_IrqEnableBm();

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    SmBus Vpd Write

    @param[in] nothing

**/
//-----------------------------------------------------------------------------
Error_t AplSmBus_VpdWrite(I2cDevice_t* i2c, U8* data, U16 offset, U16 len)
{
    I2cMode_t  oriI2cMode;
    Error_t    error;
    U16        cnt;
    U16        wdata;

    error = cEcNoError;

    oriI2cMode = I2c_ConfigPortSwitch(SMBUS_VPD_PORT, I2C_MODE_MASTER);
    SMBUS_VPD_WRITE_PROTECT(SMBUS_VPD_WRITE_EN_PIN, DISABLE);

    for (cnt = 0; cnt < len; cnt++)
    {
        wdata = (offset + cnt) | data[cnt] << 8;
        error = HalI2c_Write(i2c, (U8*)&wdata, 2, TRUE);
        HalTimer_DelayMsec(10);
    }

    SMBUS_VPD_WRITE_PROTECT(SMBUS_VPD_WRITE_EN_PIN, ENABLE);
    I2c_ConfigPortSwitch(SMBUS_VPD_PORT, oriI2cMode);

    return error;
}

//-----------------------------------------------------------------------------
/**
    SmBus Vpd Read

    @param[in] nothing

**/
//-----------------------------------------------------------------------------
Error_t AplSmBus_VpdRead(I2cDevice_t* i2c, U8* data, U16 offset, U16 len)
{
    I2cMode_t oriI2cMode;
    Error_t   error;
    U16       cnt;
    U8        wdata;

    error = cEcNoError;

    oriI2cMode = I2c_ConfigPortSwitch(SMBUS_VPD_PORT, I2C_MODE_MASTER);

    for (cnt = 0; cnt < len; cnt++)
    {
        wdata = offset + cnt;
        error = HalI2c_Read(i2c, &wdata, 1, &data[cnt], 1);
    }

    I2c_ConfigPortSwitch(SMBUS_VPD_PORT, oriI2cMode);

    return error;
}

//-----------------------------------------------------------------------------
/**
    AplSmBus_VpdInject

    @param[in] nothing

**/
//-----------------------------------------------------------------------------
Error_t AplSmBus_VpdInject(I2cDevice_t* port)
{
    Error_t  error;
    U32      size;
    U8       errLoop;

    error   = cEcNoError;
    errLoop = 0;

    MEM_SET(&gVpdCompareBuffer, 0x00, SMBUS_VPD_COMPARE_SIZE);

    gVpdPayload.d.PCIeVId[0] = (U8)(PCI_VENDOR_ID);
    gVpdPayload.d.PCIeVId[1] = (U8)(PCI_VENDOR_ID >> 8);
    MEM_COPY(&gVpdPayload.d.NVMeSN[0],
             &smSysInfo->d.MPInfo.d.SerialNumberDrive[0], sizeof(gVpdPayload.d.NVMeSN));

    gVpdPayload.d.PowerInit   = 8;
    gVpdPayload.d.PowerMaxReq = 8;

    //VPD Write
    do
    {
        error = AplSmBus_VpdWrite(port, (U8*)&gVpdPayload, 0, SMBUS_VPD_PAYLOAD_SIZE);
        if (error == cEcNoError) break;
        errLoop++;
    } while (errLoop < SMBUS_VPD_ERROR_RETRY);

    //VPD Read & Compare
    if (error == cEcNoError)
    {
        error = AplSmBus_VpdRead(port, gVpdCompareBuffer, 0, SMBUS_VPD_COMPARE_SIZE);
        // todo error handle

        if(!MEM_CMP(gVpdCompareBuffer, (U8*)&gVpdPayload, SMBUS_VPD_COMPARE_SIZE))
        {
            D_PRINTF("[SBUS] VpdPass\n");
            error = cEcNoError;
        }
        else
        {
            D_PRINTF("[SBUS] VpdFail\n");
            // todo error handle
            error = cEcNoError;
        }
    }
    return error;
}

//-----------------------------------------------------------------------------
/**
    AplSmBus_BasicVariableInit
**/
//-----------------------------------------------------------------------------
void AplSmBus_BasicVariableInit(U8 oneTimeInitFalg)
{
    U16 smBusTemp;
    U8  smBusCrc[32];
    U8  smBusCrcSize;

    if (oneTimeInitFalg)
    {
        MEM_CLR(&gBasicManagement, sizeof(BasicManagement_t));
        MEM_CLR(&smBusCrc[0],      32);

        smBusCrcSize = sizeof(BmCmd8_t) - 1;

        // Smbus Crc slave address, command code, second slave address
        smBusCrc[0] = SMBUS_BM_ADDRESS << 1;
        smBusCrc[1] = SMBUS_CMD_CODE_08;
        smBusCrc[2] = SMBUS_BM_ADDRESS << 1;

        //CMD 8
        gBasicManagement.d.Cmd8.d.Length     = SMBUS_BM_LENGTH_BASIC_CMD10;
        gBasicManagement.d.Cmd8.d.PCIeVId[0] = (U8)(PCI_VENDOR_ID);
        gBasicManagement.d.Cmd8.d.PCIeVId[1] = (U8)(PCI_VENDOR_ID >> 8);

        MEM_COPY(&gBasicManagement.d.Cmd8.d.NVMeSN[0],
                 &smSysInfo->d.MPInfo.d.SerialNumberDrive[0],
                 sizeof(gBasicManagement.d.Cmd8.d.NVMeSN));
        MEM_COPY(&smBusCrc[SMBUS_BM_CRC_START_BYTE],
                 &gBasicManagement.d.Cmd8.all[0],
                 smBusCrcSize);

        gBasicManagement.d.Cmd8.d.PEC = SmBus_Crc8(&smBusCrc[0], (smBusCrcSize + SMBUS_BM_CRC_START_BYTE));
    }

    //CMD 0
    gBasicManagement.d.Cmd0.d.Length = SMBUS_BM_LENGTH_BASIC_CMD0;
    gBasicManagement.d.Cmd0.d.SFLGS.b.Bit0_2 = 0x07;

    if (rPciePhy.laneStat1.b.PM_TXDCLK_PCLK_EN)
    {
        gBasicManagement.d.Cmd0.d.SFLGS.b.PCIeLinkActive = (rPcieCore.pciecoreGlobal0.b.SMLH_LINK_UP & rPcieCore.pciecoreGlobal0.b.RDLH_LINK_UP);
        gBasicManagement.d.Cmd0.d.SFLGS.b.PoweredUp      = rNvmeController.controller[0].CSTS.b.RDY;
    }
    else
    {
        gBasicManagement.d.Cmd0.d.SFLGS.b.PCIeLinkActive = 0;
        gBasicManagement.d.Cmd0.d.SFLGS.b.PoweredUp      = 0;
    }

    smBusTemp    = smcThermalSource[cThermalSourceSoc] - SMBUS_BM_TEMP_GP_VAR;
    smBusCrcSize = sizeof(BmCmd0_t) - 1;

    gBasicManagement.d.Cmd0.d.SFLGS.b.ResetNotRequired = 1;
    gBasicManagement.d.Cmd0.d.SFLGS.b.DriveFunctional  = 1;
    gBasicManagement.d.Cmd0.d.SFLGS.b.Arbitration      = SMBUS_READ_COMPLETE;
    gBasicManagement.d.Cmd0.d.SmartWarnings.all        = 0x1F;
    gBasicManagement.d.Cmd0.d.CTemp                    = (smBusTemp > 127) ? 0x7F : smBusTemp;
    gBasicManagement.d.Cmd0.d.PDLU                     = smSysInfo->d.LogInfo.d.SmrLog.percentageUsed;

    MEM_COPY(&smBusCrc[SMBUS_BM_CRC_START_BYTE], gBasicManagement.d.Cmd0.all, smBusCrcSize);
    gBasicManagement.d.Cmd0.d.PEC = SmBus_Crc8(&smBusCrc[0], (smBusCrcSize + SMBUS_BM_CRC_START_BYTE));
}

//-----------------------------------------------------------------------------
/**
    AplSmBus_Center
**/
//-----------------------------------------------------------------------------
Error_t AplSmBus_Center(U8 OpCode, U8* pBuffer, U32 Offset, U32 Len)
{
    Error_t error = cEcNoError;

    switch (OpCode)
    {
        case SMBUS_CENTER_VPD_INJECT:
            error = AplSmBus_VpdInject((I2cDevice_t*)&gVpdI2cDevice);
            break;
        case SMBUS_CENTER_VPD_READ:
            error = AplSmBus_VpdRead((I2cDevice_t*)&gVpdI2cDevice, pBuffer, Offset, Len);
            break;
        case SMBUS_CENTER_VPD_WRITE:
            error = AplSmBus_VpdWrite((I2cDevice_t*)&gVpdI2cDevice, pBuffer, Offset, Len);
            break;
        default:
            error = cEcError;
            break;
    }
    return error;
}

//-----------------------------------------------------------------------------
/**
    SmBus_IrqHandler
**/
//-----------------------------------------------------------------------------
static void SmBus_IrqHandler(U32 parameter)
{
    if (gpBmI2c->i2cIntrStat.b.R_TX_ABRT)
    {
        // clear the abort by reading the register
        gpBmI2c->clrTxAbrtIntr.b.CLR_TX_ABRT;
    }

    // host xfer data
    if (gpBmI2c->i2cStat.b.RFNE && gpBmI2c->i2cRxFifoLvl.b.RX_F_LR)
    {
        gSmBusIndex = gpBmI2c->i2cRxTxDataBufAndCmd.all;
        if (gSmBusIndex == SMBUS_BM_READ_START_BYTE)
        {
            gBasicManagement.d.Cmd0.d.SFLGS.b.Arbitration = SMBUS_READ_PROCESS;
        }
    }

    if (gSmBusIndex == 0)
    {
        AplSmBus_BasicVariableInit(FALSE);
    }

    if (!gpBmI2c->i2cStat.b.TFE)
    {
        // clear RD_REQ
        gpBmI2c->clrRdReqIntr.b.CLR_RD_REQ;
    }

    // host request data
    if (gpBmI2c->i2cIntrStat.b.R_RD_REQ && gpBmI2c->i2cStat.b.TFE)
    {
        if (gpBmI2c->i2cStat.b.TFE)
        {
            U32  smBusBasicLen;
            for (smBusBasicLen = 0; smBusBasicLen < SMBUS_BM_ISR_PUT_MAX_CNT; smBusBasicLen++)
            {
                gpBmI2c->clrTxAbrtIntr.b.CLR_TX_ABRT;

                // clear RD_REQ
                gpBmI2c->clrRdReqIntr.b.CLR_RD_REQ;
                if (gSmBusIndex < SMBUS_BM_LENGTH_BASIC)
                {
                    gpBmI2c->i2cRxTxDataBufAndCmd.all = I2C_CMD_WRITE_FLAG | gBasicManagement.all[gSmBusIndex];
                }
                else
                {
                    gpBmI2c->i2cRxTxDataBufAndCmd.all = I2C_CMD_WRITE_FLAG | 0x00;
                }

                gSmBusIndex++;

                if (gSmBusIndex == SMBUS_BM_LENGTH_COMPLETE)
                {
                    gBasicManagement.d.Cmd0.d.SFLGS.b.Arbitration = SMBUS_READ_COMPLETE;
                }
            }
        }
    }

    if (gpBmI2c->i2cIntrStat.b.R_RX_DONE)
    {
        gpBmI2c->clrRxDoneIntr.b.CLR_RX_DONE;
    }

    if (gpBmI2c->i2cIntrStat.b.R_TX_OVER)
    {
        gpBmI2c->clrTxOverIntr.b.CLR_TX_OVER;
    }
}

//-----------------------------------------------------------------------------
/**
    SmBus_IrqEnableBm
**/
//-----------------------------------------------------------------------------
static void SmBus_IrqEnableBm(void)
{
    volatile I2c_t* pI2cClose = (SMBUS_BM_PORT == I2C_PORT_0) ? &rI2c1 : &rI2c0;

    pI2cClose->i2cIntrMsk.all = 0;

    gpBmI2c->i2cEn.b.I2C_EN             = 0;
    gpBmI2c->i2cIntrMsk.all             = 0;

    gpBmI2c->i2cIntrMsk.b.M_RD_REQ      = 1;
    gpBmI2c->i2cIntrMsk.b.M_RX_DONE     = 1;
    gpBmI2c->i2cIntrMsk.b.M_RX_UNDER    = 1;
    gpBmI2c->i2cIntrMsk.b.M_TX_OVER     = 1;
    gpBmI2c->i2cIntrMsk.b.M_TX_ABRT     = 1;
    gpBmI2c->i2cIntrMsk.b.M_GEN_CALL    = 1;

    gpBmI2c->i2cSlaveAddr.b.IC_SAR      = SMBUS_BM_ADDRESS;
    gpBmI2c->i2cEn.b.I2C_EN             = 1;

    HalIrq_RegisterHandler(cIrqSmBusBasic, SmBus_IrqHandler, ENABLE);
    I2c_ConfigPortSwitch(SMBUS_BM_PORT, I2C_MODE_SLAVE);
}

//-----------------------------------------------------------------------------
/**
    Smbus CRC by 1 byte

    Return CRC-8 of the data, using x^8 + x^2 + x + 1 polynomial.
    A table-based algorithm would be faster,
    but for only few bytes, it isn't worth for the code size.
**/
//-----------------------------------------------------------------------------
static U8 SmBus_Crc8(U8* data, U8 len)
{
    U32  crc = 0;
    U8   cnt;
    U8   shift;

    for (cnt = len; cnt; cnt--, data++)
    {
        crc ^= (*data << 8);
        for (shift = 8; shift; shift--)
        {
            if (crc & 0x8000)
            {
                crc ^= (0x1070 << 3);
            }
            crc <<= 1;
        }
    }
    return (U8)(crc >> 8);
}
#endif
#endif

/* End of File */
