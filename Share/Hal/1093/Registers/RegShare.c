//============================================================================
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
//! \file
//! @brief 0 register declaration
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------

#include "BuildPkg.h"
#include "RegSpiBridge.h"
#include "RegSetm3.h"
#include "RegSetm3Phy.h"
#include "RegI2cIf.h"
#include "RegDmac.h"
#include "RegMRE.h"
#include "RegIntController.h"
#include "RegArmVic.h"
#include "RegClockPLL.h"
#include "RegIpc.h"
#include "RegAvs.h"
#include "RegApb.h"
#include "RegTempSns.h"
#include "RegPeripheral.h"
#include "RegCoutTimers.h"
#include "RegDdrMc.h"

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define __AT(X) __attribute__((at(X))) __attribute__((zero_init)) ///< locate at specified address
#define __AS(X) __attribute__((alias(X)))                         ///< name, type alias

//-----------------------------------------------------------------------------
//  Register declaration: Private or Public
//-----------------------------------------------------------------------------

/// @brief SoC register declaration

volatile SpiBridge_t         rSpiBridge         __AT(0x22000000);   // 32bit accessible only
volatile Setm3_t             rSetm3             __AT(0xD001B000);   // 32bit accessible only
volatile Setm3Phy_t          rSetm3Phy          __AT(0xD0020000);   // 32bit accessible only
volatile I2c_t               rI2c0              __AT(0xF0108000);   // 32bit accessible only
volatile I2c_t               rI2c1              __AT(0xF0109000);   // 32bit accessible only
volatile Dmac_t              rDmac              __AT(0xF010A000);   // 32bit accessible only
volatile MRE_t               rMRE               __AT(0xA0003800);   // 32bit accessible only
volatile Intc_t              rIntc0             __AT(0xF010C000);   // 32bit accessible only
volatile Intc_t              rIntc1             __AT(0xF010C400);   // 32bit accessible only
volatile Intc_t              rIntc2             __AT(0xF010C800);   // 32bit accessible only
volatile ClockPll_t          rClockPll          __AT(0xF0112000);   // 32bit accessible only
volatile Ipc_t               rIpc               __AT(0xF0114000);   // 32bit accessible only
volatile Avs_t               rAvs               __AT(0xF0114400);   // 32bit accessible only
volatile Apb_t               rApb               __AT(0xF0115000);   // 32bit accessible only
volatile TempSensor_t        rTempSensor        __AT(0xF0115180);   // 32bit accessible only
volatile Mbist_t             rMbist             __AT(0xF0115200);   // 32bit accessible only
volatile WdTimer_t           rWdTimer           __AT(0xF0116000);   // 32bit accessible only
volatile Vintc_t             rVintc0            __AT(0xF0120000);   // 32bit accessible only
volatile Vintc_t             rVintc1            __AT(0xF0122000);   // 32bit accessible only
volatile Vintc_t             rVintc2            __AT(0xF0124000);   // 32bit accessible only
volatile CoutTimers_t        rCoutTimers        __AT(0xF1020000);   // 32bit accessible only
volatile Uart_t              rUart0             __AT(0xF1010000);   // 32bit accessible only
volatile Uart_t              rUart1             __AT(0xF1010100);   // 32bit accessible only
volatile DdrMc_t             rDdrMc             __AT(0xF1000000);   // 32bit accessible only

/// @brief non volatile register declaration for optimization
