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
//! @brief Pure HAL API definition for GPIO and Test Pin H/W
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
/// @brief Gpio power mode definition
typedef enum
{
    cGpioPmActive=1,        ///< Active mode - highest power consumption state
    cGpioPmDeepSleep=15     ///< Deep sleep mode - Lowest power consumption state
} GpioPowerMode_t;

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t  HalGpio_OneTimeInit(InitBootMode_t initMode);
Error_t  HalGpio_SetPowerMode(GpioPowerMode_t powerMode);
uint32_t HalGpio_ReadAllLatchedPin(void);
uint16_t HalGpio_ReadAllPin(void);
uint16_t HalGpio_ReadPin(uint32_t pinNo);
uint16_t HalGpio_ReadOutPin(uint32_t pinNo);

void HalGpio_AssertPin(uint32_t pinNo);
void HalGpio_DeassertPin(uint32_t pinNo);
void HalGpio_TogglePin(uint32_t pinNo);
void HalGpio_WritePin(uint32_t pinNo, uint32_t value);
void HalGpio_OutputSetMux(uint8_t value);

void HalGpio_OutputEnable(uint32_t pinBits);
void HalGpio_OutputDisable(uint32_t pinBits);
