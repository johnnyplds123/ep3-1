// ============================================================================
//
//     Copyright (c) Marvell Corporation 2000-2013  -  All rights reserved
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
//! \file
//! @brief I2C I/F code for ALTA SoC
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharedVars.h"

#include "RegI2cIf.h"
#include "HalCpu.h"
#include "HalTimer.h"
#include "HalI2c.h"

#include "MemIo.h"

#include "NormSection.h"
//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
const I2cPort_t I2cPortAccessOrder[I2C_NUMBER_OF_PORTS] =
{
    #if (_GENERATION_ == GENERATION_EP3)
    I2C_PORT_1, I2C_PORT_0
    #else
    I2C_PORT_0, I2C_PORT_1
    #endif
};

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//
//  VARIABLE DECLARATION WITH INITIAL VALUE DOESN'T WORK !!!
//  UNINTIALIZED VARIABLE DECLARATION ONLY !!!
//-----------------------------------------------------------------------------
#pragma arm section rwdata = "data_fast_ui"
#pragma arm section zidata = "data_fast_ui"

// CPU2 only
#if _CPU_==CPU_ID2

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------
static  void    I2c_ConfigPort(I2cPort_t port, I2cMode_t mode);
static  Error_t I2c_WaitWriteXferDone(I2cDevice_t const* pDevice);
static  Error_t I2c_WaitReadXferDone (I2cDevice_t const* pDevice);

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  HalI2c_OneTimeInit
//-----------------------------------------------------------------------------
Error_t HalI2c_OneTimeInit(InitBootMode_t initMode)
{
    I2c_ConfigPort(I2C_PORT_0, I2C_MODE_MASTER);
    I2c_ConfigPort(I2C_PORT_1, I2C_MODE_MASTER);

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Write data to I2C slave.\n

    @param[in]  pDevice         I2C address locator
    @param[in]  data            pointer to data to be sent
    @param[in]  length          #bytes to be sent
    @param[in]  waitTransfer    Wait until transfer completed

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalI2c_Write(I2cDevice_t const* pDevice, U8 const* data, U8 length, BOOL waitTransfer)
{
    Error_t error = cEcNoError;
    volatile I2c_t* pI2c = (pDevice->port == I2C_PORT_0) ? &rI2c0 : &rI2c1;
    U8 index;

    // check if TX FIFO is empty and no Master activity
    if ((pI2c->i2cStat.b.TFE == 1) && (pI2c->i2cStat.b.MST_ACTIVITY == 0))
    {
        pI2c->i2cEn.b.I2C_EN      = 0;                      // Disable I2C
        pI2c->i2cTgtAddr.b.IC_TAR = pDevice->slaveAddress;  // Set target slave address
        pI2c->i2cEn.b.I2C_EN      = 1;                      // Enable I2C

        BEGIN_CS1;
        for (index = 0; index < length; index++)
        {
            pI2c->i2cRxTxDataBufAndCmd.all = I2C_CMD_WRITE_FLAG | data[index];
        }
        END_CS1;

        if (waitTransfer)
        {
            error = I2c_WaitWriteXferDone(pDevice);
        }
    }
    else
    {
        error = cEcI2cGeneral;
    }
    return error;
}

//-----------------------------------------------------------------------------
/**
    Read data from I2C slave.\n

    @param[in]  pDevice         I2C address locator
    @param[in]  inputData       pointer to data to be sent before reading
    @param[in]  inputlength     #bytes to be sent before reading
    @param[in]  readData        pointer to store the data which is read
    @param[in]  readlength      #bytes to read

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalI2c_Read(I2cDevice_t const* pDevice, U8 *inputData, U8 inputLength, U8 *readData, U8 readLength)
{
    Error_t error = cEcNoError;
    volatile I2c_t* pI2c = (pDevice->port == I2C_PORT_0) ? &rI2c0 : &rI2c1;
    U32 cnt  = 0;
    U8 index = 0;

    // check if TX FIFO is empty and no Master activity
    if ((pI2c->i2cStat.b.TFE == 1) && (pI2c->i2cStat.b.MST_ACTIVITY == 0))
    {
        pI2c->i2cEn.b.I2C_EN = 0;                           // Disable I2C
        pI2c->i2cTgtAddr.b.IC_TAR = pDevice->slaveAddress;  // Set target slave address
        pI2c->i2cEn.b.I2C_EN = 1;                           // Enable I2C

        while(pI2c->i2cStat.b.RFNE)
        {
            // poll data untill RX FIFO is empty
            pI2c->i2cRxTxDataBufAndCmd.all;
            cnt++;
            if (cnt > MAX_NR_LOOPS)
            {
                D_PRINTF("[I2C ] ErrTimeout|R\n");
                break;
            }
        }

        // write bytes to slave
        if (inputLength > 0)
        {
            BEGIN_CS1;
            for (index = 0; index < inputLength; index++)
            {
                pI2c->i2cRxTxDataBufAndCmd.all = I2C_CMD_WRITE_FLAG | inputData[index];
            }
            END_CS1;

            error = I2c_WaitWriteXferDone(pDevice);
        }

        // read bytes from slave
        if ((error == cEcNoError) && (readLength > 0))
        {
            BEGIN_CS1;
            for (index = 0; index < readLength; index++)
            {
                pI2c->i2cRxTxDataBufAndCmd.all = I2C_CMD_READ_FLAG;
            }
            END_CS1;

            error = I2c_WaitReadXferDone(pDevice);

            if (pI2c->i2cRxFifoLvl.b.RX_F_LR == readLength)
            {
                for (index = 0; index < readLength; index++)
                {
                    readData[index] = pI2c->i2cRxTxDataBufAndCmd.all;
                }
            }
            else
            {
                error = cEcI2cGeneral;
            }
        }
    }
    else
    {
        error = cEcI2cGeneral;
    }
    return error;
}

//-----------------------------------------------------------------------------
/**
    Configure I2C port
     - set baud rate to 400 kbps

    @param[in] portNum: I2C port number

    @return nothing
**/
//-----------------------------------------------------------------------------
static void I2c_ConfigPort(I2cPort_t port, I2cMode_t mode)
{
    volatile I2c_t* pI2c = (port == I2C_PORT_0) ? &rI2c0 : &rI2c1;

    pI2c->i2cEn.b.I2C_EN = 0;                   // Disable I2C

    //pI2c->i2cFastSpeedClkSclHighCnt.b.IC_FS_SCL_HCNT = 96;  // 400Kbps (see SoC spec for complete calculation)
    //pI2c->i2cFastSpeedClkSclLowCnt.b.IC_FS_SCL_LCNT  = 145;
    pI2c->i2cStandardSpeedClkSclHighCnt.b.IC_SS_SCL_HCNT = 0x3E8;
    pI2c->i2cStandardSpeedClkSclLowCnt.b.IC_SS_SCL_LCNT = 0x497;

    pI2c->i2cCtrl.b.SPEED_IC            = 1;    // Configure I2C for "FAST" speed, 400Kbps, (to use i2cFastSpdHighCnt i2cFastSpdLowCnt registers for SCL low and high time)
    pI2c->i2cCtrl.b.MSTR_MODE           = mode; // Enable I2C to operate in Master mode.
    pI2c->i2cCtrl.b.IC_SLAVE_DIS        = mode; // Disable Slave I2C operatation.
    pI2c->i2cCtrl.b.IC_RESTART_EN       = 1;    // Enable issuing Restart when in Master mode.
    pI2c->i2cCtrl.b.IC_10B_ADDR_MSTR    = 0;    // Configure for 7 bit address for Master mode
    pI2c->i2cCtrl.b.IC_10B_ADDR_SLAVE   = 0;    // Configure for 7 bit address for Slave mode
    pI2c->i2cSdaHold.b.IC_SDA_HOLD      = 50;   // set 200ns@MT0 data hold time to meet thermal ic spec

    pI2c->i2cEn.b.I2C_EN = 1;                   // Enable I2C

    pI2c->clrTxAbrtIntr.b.CLR_TX_ABRT;          // Read to clear TX_ABRT flag, which prevents Tx FIFO activity.
}

//-----------------------------------------------------------------------------
/**
     I2c_ConfigPortSwitch

    @param[in] portNum: I2C port number

    @return Ori. I2C Port setting
**/
//-----------------------------------------------------------------------------
I2cMode_t I2c_ConfigPortSwitch(I2cPort_t port, I2cMode_t mode)
{
    volatile I2c_t* pI2c = (port == I2C_PORT_0) ? &rI2c0 : &rI2c1;
    I2cMode_t oriI2cMode = (I2cMode_t) pI2c->i2cCtrl.b.IC_SLAVE_DIS;

    I2c_ConfigPort(port, mode);

    return oriI2cMode;
}
//-----------------------------------------------------------------------------
/**
    Wait for the transfer completion in Write

    @param[in]  pDevice     I2C address locator

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t I2c_WaitWriteXferDone(I2cDevice_t const* pDevice)
{
    Error_t error = cEcNoError;
    U32 maxNrLoops = MAX_NR_LOOPS;
    volatile I2c_t* pI2c = (pDevice->port == I2C_PORT_0) ? &rI2c0 : &rI2c1;

    // Wait here until I2C Tx FIFO flag shows empty and no Activity
    while ((pI2c->i2cStat.b.TFE == 0) || (pI2c->i2cStat.b.ACT_STAT))
    {
        maxNrLoops--;
        if (pI2c->i2cRawIntrStat.b.TX_ABRT)
        {
            pI2c->clrTxAbrtIntr.b.CLR_TX_ABRT;  // clear the abort by reading the register
            error = cEcI2cGeneral;              // Write Abort Error
            break;
        }

        if (maxNrLoops == 0)
        {
            D_PRINTF("[I2C ] ErrTimeout|W\n");
            error = cEcI2cTimeout;
            break;
        }
    }

    if (pI2c->i2cRawIntrStat.b.TX_ABRT)
    {
        pI2c->clrTxAbrtIntr.b.CLR_TX_ABRT;  // clear the abort by reading the register
        error = cEcI2cGeneral;              // Write Abort Error
    }
    return error;
}

//-----------------------------------------------------------------------------
/**
    Wait for the transfer completion in Read

    @param[in]  pDevice     I2C address locator

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t I2c_WaitReadXferDone(I2cDevice_t const* pDevice)
{
    Error_t error = cEcNoError;
    U32 maxNrLoops = MAX_NR_LOOPS;
    volatile I2c_t* pI2c = (pDevice->port==I2C_PORT_0) ? &rI2c0 : &rI2c1;

    // Wait here until I2C Rx FIFO flag shows not empty and no Activity
    while ((pI2c->i2cStat.b.RFNE == 0) || (pI2c->i2cStat.b.ACT_STAT))
    {
        maxNrLoops--;
        if (maxNrLoops == 0)
        {
            error = cEcI2cTimeout;
            D_PRINTF("[I2C ] ErrTimeout|X\n");
            break;
        }
    }
    return error;
}
#endif

/* End of File */
