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
//! @brief Pure HAL API definition for UART HW control
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
typedef enum
{
    cUartPmActive=1,        ///< Active mode - highest power consumption state
    cUartPmDeepSleep=15     ///< Deep sleep mode - Lowest power consumption state
} UartPowerMode_t;

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
Error_t  HalUart_OneTimeInit(InitBootMode_t initMode);
Error_t  HalUart_SetPowerMode(UartPowerMode_t powerMode);
uint32_t HalUart_SetBaudRate(uint32_t portNum, uint32_t baudRate);

bool     HalUart_TestRxReady(uint32_t portNum);
bool     HalUart_TestTxReady(uint32_t portNum);
bool     HalUart_TestTxEmpty(uint32_t portNum);

void     HalUart_Transmit(uint32_t portNum, uint8_t data);
uint8_t  HalUart_Receive(uint32_t portNum);
void     HalUart_ResetFIFO(uint32_t portNum);
