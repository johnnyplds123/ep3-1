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
//! @brief Pure HAL API definition for Timer HW control
//!
//=============================================================================
#if _CPU_==CPU_ID2    // CPU2 only
#include "RegClockPLL.h"
#endif
//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
#define SMARTCHECK_TICK_PER_SEC         1
#define PWRONHR_TICK_SCAL               SMARTCHECK_TICK_PER_SEC * 60 * 20    // 20 minutes

/// @brief Timer power mode definition
typedef enum
{
    cTimerPmActive=1,           ///< Active mode - highest power consumption state
    cTimerPmDeepSleep=15        ///< Deep sleep mode - Lowest power consumption state
} TimerPowerMode_t;

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef uint32_t HwTimerTick_t;     ///< data type for hw timer tick

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t  HalTimer_OneTimeInit(InitBootMode_t initMode);
Error_t  HalTimer_SetPowerMode(TimerPowerMode_t powerMode);

uint32_t HalTimer_SetPeriodicInterruptFrequency(uint32_t hertz);
uint32_t HalTimer_SetSmartTimerInterruptFrequency(uint32_t hertz);

#if _CPU_==CPU_ID2    // CPU2 only
uint32_t HalTimer_SetTemperatureTimerInterruptFrequency(uint32_t hertz);
void     HalTimer_ThermalThrottleTimer(uint32_t usec);
#endif

void     HalTimer_SetOneShotTimer(uint32_t usec);
void     HalTimer_SmartCheckTimer(uint32_t usec);
HwTimerTick_t HalTimer_ReadTick(void);

void     HalTimer_DelayMsec(uint32_t milliSecToDelay);
void     HalTimer_DelayUsec(uint32_t microSecToDelay);

uint32_t HalTimer_GetElapsedTimeMsec(HwTimerTick_t lastTick);
uint32_t HalTimer_GetElapsedTimeUsec(HwTimerTick_t lastTick);
uint32_t HalTimer_SetCounterPrecision(uint32_t tickPerUsec);


//-----------------------------------------------------------------------------
/**
    Compute time difference in micro-second unit.\n

    @param[in]  startTick  start Tick
    @param[in]  endTick    end Tick

    @return   time difference (usec)
**/
//-----------------------------------------------------------------------------
__inline uint32_t HalTimer_GetTimeDiffUsec(HwTimerTick_t startTick, HwTimerTick_t endTick)
{
#ifdef TIMER_TICK_PER_USEC // timer resoultion is higher than usec
    return ((uint32_t)(startTick-endTick)/TIMER_TICK_PER_USEC);
#else
    return ((uint32_t)(startTick-endTick)*TIMER_USEC_PER_TICK);
#endif
}
