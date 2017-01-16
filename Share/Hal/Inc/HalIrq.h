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
//! @brief Pure HAL API definition for Interrupt H/W control
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------

/// @brief Hardware Independant IRQ(Interrupt request) list
typedef enum
{
    cIrqBegin=0,
    cIrqPowerFault,       ///< Power fault IRQ

    cIrqFw,               ///< FW generated IRQ

    cIrqUartRx,           ///< UART transimit IRQ
    cIrqUartTx,           ///< UART receive IRQ

    cIrqTimerPeriodic,    ///< Periodic timer interrupt (reserved for system)
    cIrqTimerOneshot,     ///< Oneshot timer interrupt
    cIrqTimerSmartCheck,   ///< I/O Busy Check for SMART
    cIrqTimerThermalThrottle,   ///< I/O Busy Check for periodic Temperature Measurement

    cIrqIpcCommandIn,     ///< IPC command in  (opponent CPU writes IPC command register)
    cIrqIpcStatusRead,    ///< IPC status read (opponent CPU reads IPC command register)

    cIrqHostShutdown,     ///< Host shutdown
    cIrqHostReset,        ///< Host reset
    cIrqHostCommandIn,    ///< Host command in
    cIrqHostXferDone,     ///< Host transfer done
    cIrqHostIoXferDone,   ///< Host IO command transfer done
    cIrqHostIfPm,         ///< Host I/F power management
    cIrqHostIfError,      ///< Host I/F error

    cIrqMediaDone,        ///< Media done

    cIrqPLP,              ///< PLP interrupt (TEST_PIN 11)
    cIrqSmBusBasic,       ///< SmBus Basic Management

    cIrqNumOfId

} IrqId_t;

/// @brief HalIrq power mode
typedef enum
{
    cIrqPmActive=1,         ///< Active mode - highest power consumption state
    cIrqPmSleep,            ///< Sleep mode
    cIrqPmDeepSleep=15      ///< Deep sleep mode - Lowest power consumption state
} IrqPowerMode_t;

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef void (*IrqHandler_t)(uint32_t param);

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t HalIrq_OneTimeInit(InitBootMode_t initMode);

uint32_t HalIrq_Handler(void);
void HalFiq_Handler(void);

IrqHandler_t HalIrq_RegisterHandler(IrqId_t irqId, IrqHandler_t irqHandler, uint32_t state);
uint32_t     HalIrq_EnDisable(IrqId_t irqId, uint32_t state);
Error_t      HalIrq_SetPowerMode(IrqPowerMode_t powerMode);

