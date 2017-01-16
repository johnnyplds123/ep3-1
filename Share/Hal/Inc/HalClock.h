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
//! @brief Pure HAL API definition for Clock and TBG(Time Based Generator) H/W
//!
//=============================================================================
#include "RegClockPLL.h"

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
/// @brief Clock TBG/PLL ID definition
typedef enum
{
    cClockOsc  = 0,         ///< Oscillator clock
    cClockTbgA = 1,         ///< TBG A
    cClockTbgB = 2,         ///< TBG B
    cClockTbgC = 3,         ///< TBG C
    cClockTbgD = 4,         ///< TBG D
} ClockId_t;

/// @brief Clock power mode definition
typedef enum
{
    cClockPmActive            =  1, ///< Active mode - highest power consumption state
    cClockPmPartialSleep      =  2, ///< Partial Sleep mode - change clk source into Osc except MC5 clock
    cClockPmSleep             =  3, ///< Sleep mode - change clk source into Osc
    cClockPmDeepSleepAnalogOn = 14, ///< Deep sleep mode - Analog block is turned on
    cClockPmDeepSleep         = 15  ///< Deep sleep mode - Lowest power consumption state
} ClockPowerMode_t;

/// @brief Operational Clock power mode
typedef enum
{
    cClockPmHighestActive     =  0, ///< Active mode - highest power consumption state
    cClockPmHighActive,             ///< Active mode - high power consumption state
    cClockPmMediumActive,           ///< Active mode - medium power consumption state
    cClockPmLowActive,              ///< Active mode - low power consumption state
} OperationalClockPowerMode_t;

/// @brief DDR Frequency Index on the table
typedef enum
{
    cDdrTbgType = 0,                ///< TBG type position
    cDdrFrequencyTbg,               ///< TBG position
    cDdrFrequencyBm,                ///< BM position
    cNumberOfDdrFrequencies         ///< a Number of teh table
} DdrFrequencyIndex_t;

/// @brief TGB clock frequency definition
typedef enum TbgClockFrequency_t
{
    cTbgClockFrequency500Mhz  =  500,
    cTbgClockFrequency800Mhz  =  800,
    cTbgClockFrequency1000Mhz = 1000,
    cTbgClockFrequency1066Mhz = 1066,
    cTbgClockFrequency1200Mhz = 1200,
    cTbgClockFrequency1333Mhz = 1333,
    cTbgClockFrequency1400Mhz = 1400,
    cTbgClockFrequency1600Mhz = 1600
} TbgClockFrequency_t;

/// @brief BM clock frequency definition
typedef enum BmClockFrequency_t
{
    cBmClockFrequency125Mhz = 125,
    cBmClockFrequency200Mhz = 200,
    cBmClockFrequency250Mhz = 250,
    cBmClockFrequency266Mhz = 266,
    cBmClockFrequency300Mhz = 300,
    cBmClockFrequency333Mhz = 333,
    cBmClockFrequency350Mhz = 350,
    cBmClockFrequency400Mhz = 400
} BmClockFrequency_t;

COMPILE_ASSERT(TBG_B_FREQ_MHZ == 500, "Need to sync PM delay setting: HalClock_PllBlockOff");
//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern volatile ClockPowerMode_t currentClockPowerMode;

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t HalClock_OneTimeInit(InitBootMode_t initMode);

uint32_t HalClock_SetFrequency(ClockId_t clockId, uint32_t freqMhz);
uint32_t HalClock_GetFrequency(ClockId_t clockId);
bool     HalClock_TurnOnOff(ClockId_t clockId, bool onOff);
uint32_t HalClock_SetSsc(ClockId_t clockId, uint32_t sscAmount);
bool     HalClock_SscOnOff(ClockId_t clockId, bool onOff);

uint32_t HalClock_GetOscFrequency(void);
Error_t  HalClock_SetPowerMode(ClockPowerMode_t powerMode);
Error_t  HalClock_SetOperationalPowerMode(OperationalClockPowerMode_t powerMode);

uint16_t HalClock_SetEccClock(uint16_t freqMhz);
volatile PllCtrl_t* HalClock_GetPllRegisterBase(ClockId_t clockId);


