#pragma once
// ============================================================================
//
//     Copyright (c) Marvell Corporation 2000-2014  -  All rights reserved
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
//! @brief Pure HAL API definition for I2C HW control
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
// ref. TI TMP102/TMP103
#define I2C_ADDRESS_TI_TMP103A              0x70
#define I2C_ADDRESS_TI_TMP102               0x48

// TI use POINTER_REGISTER
#define I2C_POINTER_REGISTER_BYTE           0x00
#define I2C_POINTER_REGISTER_TEMPERATURE    (I2C_POINTER_REGISTER_BYTE|0x00)
#define I2C_POINTER_REGISTER_CONFIG         (I2C_POINTER_REGISTER_BYTE|0x01)
#define I2C_POINTER_REGISTER_T_LOW          (I2C_POINTER_REGISTER_BYTE|0x02)
#define I2C_POINTER_REGISTER_T_HIGH         (I2C_POINTER_REGISTER_BYTE|0x03)

// Temperature Application
#define I2C_ROOM_TEMPERATURE                25

#define I2C_CMD_DUMMY_DATA                  (0x00)
#define I2C_CMD_READ_FLAG                   (0x01 << 8)
#define I2C_CMD_WRITE_FLAG                  (0x00 << 8)

#define MAX_NR_LOOPS                        12000

//-----------------------------------------------------------------------------
//  Constants definitions: Silergy SYT664
//-----------------------------------------------------------------------------
#define I2C_ADDRESS_SILERGY_SYT664          0x5A

#define I2C_REGISTER_SILERGY_DCP            0x02    // DC-DC Converter Control Parameter
#define I2C_REGISTER_SILERGY_DCP_POWER_M    0x0E    // Boost Peak Current Mask 1110b
#define I2C_REGISTER_SILERGY_DCP_300MA      0x00
#define I2C_REGISTER_SILERGY_DCP_500MA      0x01
#define I2C_REGISTER_SILERGY_DCP_600MA      0x02

#define I2C_REGISTER_SILERGY_CAP_MEASURE    0x06    // Capacitance Measurement
#define I2C_REGISTER_SILERGY_CAP_MEASURE_E  BIT(5)  // Capacitance Measurement Enable

#define I2C_REGISTER_SILERGY_T_DISCHARGE_H  0x07    // Discharge time data TDISCHARGE High byte
#define I2C_REGISTER_SILERGY_T_DISCHARGE_L  0x08    // Discharge time data TDISCHARGE Low  byte

//-----------------------------------------------------------------------------
//  Constants definitions: ATMEL EEPROM 24C01D
//-----------------------------------------------------------------------------
#define I2C_ADDRESS_ATMEL_EE24C01D          0x53

//-----------------------------------------------------------------------------
//  Constants definitions: SMBus Basic Management Address
//-----------------------------------------------------------------------------
#define I2C_ADDRESS_BASIC                   0x6A

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief I2c Port definition
typedef enum I2cPort_t
{
    I2C_PORT_0 = 0,         ///< I2c Port 0
    I2C_PORT_1,             ///< I2c Port 1
    I2C_NUMBER_OF_PORTS,    ///< Number of I2c Ports
} I2cPort_t;

/// @brief I2c Mode definition
typedef enum I2cMode_t
{
    I2C_MODE_SLAVE = 0,         ///< I2c Mode Slave
    I2C_MODE_MASTER,            ///< I2c Mode Master
    I2C_NUMBER_OF_MODE,         ///< Number of I2c Mode
} I2cMode_t;

/// @brief I2c Mode definition
typedef enum I2cOrder_t
{
    I2C_ORDER_PRIMARY = 0,  ///< I2c Order Primary
    I2C_ORDER_SECONDARY,    ///< I2c Order Sencodary
    I2C_NUMBER_OF_ORDER,    ///< Number of I2c Order
} I2cOrder_t;

/// @brief Ports and Slave Addresses
typedef struct I2cDevice_t
{
    I2cPort_t port;         ///< I2c Port location
    U8 slaveAddress;        ///< I2c Slave Address
} I2cDevice_t;

//ref. TI TMP102/TMP103
typedef union
{
    U8 all[2];
    struct{
        U8 bHighByte    : 7;    ///< HighByte
        U8 bSign        : 1;
        U8 bPadding     : 4;
        U8 bLowByte     : 4;    ///< LowByte
    }b;
} I2cReadData12Bit_t;

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define I2C_DEVICE_LOCATION(D ,P, A)  {(D).port = P; (D).slaveAddress = A;}

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern const I2cPort_t I2cPortAccessOrder[];
//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t HalI2c_OneTimeInit(InitBootMode_t initMode);
Error_t HalI2c_Write(I2cDevice_t const* pDevice, U8 const* data, U8 length, BOOL waitTransfer);
Error_t HalI2c_Read (I2cDevice_t const* pDevice, U8 *inputData, U8 inputLength, U8 *readData, U8 readLength);

extern I2cMode_t I2c_ConfigPortSwitch(I2cPort_t port, I2cMode_t mode);
/* End of File */

