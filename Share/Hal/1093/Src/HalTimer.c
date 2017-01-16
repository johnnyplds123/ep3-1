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
//! \file
//! @brief TIMER HW control code for ALTA SoC
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
// PKG#include "BuildPkg.h"// REG#include "RegIpc.h"#include "RegApb.h"#include "RegArmVic.h"#include "RegClockPLL.h"#include "RegCoutTimers.h"// HAL#include "HalCpu.h"#include "HalIrq.h"#include "HalI2c.h"#include "HalClock.h"#include "HalIrqSrc.h"#include "HalTempSensor.h"#include "HalTimer.h"#include "HalMc5DdrInit.h"#include "ClockTree.h"#include "Kernel.h"#if _CPU_==CPU_ID2    // CPU2 only#include "Thermal.h"#endif// APL#include "SharedVars.h"#if ((_CPU_==CPU_ID0) && CHK_CODE_TYPE(CODE_MAIN))     // CPU0 Only#include "NvmeAdminCommands.h"#include "SysInfo.h"#include "HldNvme.h"#include "NvmeGetLogPage.h"#endif// Code Section#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define PERIODIC_TIMER_ID         0   ///< for all CPUs
#define FREE_RUN_TIMER_ID         1   ///< for all CPUs
#define ONESHOT_TIMER_ID          2   ///< for CPU0
#define SMARTCHECK_TIMER_ID       3   ///< for CPU0
#define TEMPERATURECHECK_TIMER_ID 4   ///< FOR CPU2

#define TIMER_DEBOUNCE_MARGIN   256             ///< debounce margin for reading timer counter


//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define MSEC2TICK(T)    ((HwTimerTick_t)((T)*TIMER_TICK_PER_USEC*1000))    ///< convert milli-second to hw timer tick unit
#define USEC2TICK(T)    ((HwTimerTick_t)((T)*TIMER_TICK_PER_USEC))         ///< convert micro-second to hw timer tick unit

#define TICK2MSEC(T)    ((uint32_t)(T)/TIMER_TICK_PER_USEC/1000)           ///< convert hw timer tick to milli-second unit
#define TICK2USEC(T)    ((uint32_t)(T)/TIMER_TICK_PER_USEC)                ///< convert hw timer tick to micro-second unit

#if (_SOC_REV_<0x0200)    // for R1.0 and R1.1
#define GET_TIMER_INTR_MASK(CPU)                (rCoutTimers.cpuIntrMsk[CPU].all)
#define SET_TIMER_INTR_MASK(CPU, VAL)           (rCoutTimers.cpuIntrMsk[CPU].all |= (VAL))
#define CLR_TIMER_INTR_MASK(CPU, VAL)           (rCoutTimers.cpuIntrMsk[CPU].all &= (~(VAL)))
#define GET_TIMER_INTR_PENDING_STATUS(CPU)      (rCoutTimers.intrPendingStat.all)
#define CLR_TIMER_INTR_PENDING_STATUS(CPU,VAL)  (rCoutTimers.intrPendingStat.all = (VAL))
#else
#define GET_TIMER_INTR_MASK(CPU)                (rCoutTimers.cpuIntrMsk[CPU].all)
#define SET_TIMER_INTR_MASK(CPU, VAL)           (rCoutTimers.cpuIntrMsk[CPU].all = (VAL))
#define CLR_TIMER_INTR_MASK(CPU, VAL)           (rCoutTimers.cpuIntrClrMsk[CPU].all = (VAL))
#define GET_TIMER_INTR_PENDING_STATUS(CPU)      (rCoutTimers.intrPendingStat[CPU].all)
#define CLR_TIMER_INTR_PENDING_STATUS(CPU,VAL)  (rCoutTimers.intrPendingStat[CPU].all = (VAL))
#endif

#define AGING_DRAM_TEST_TAG       0x4D415244
//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static uint32_t  GetElapsedTick(HwTimerTick_t lastTick);
static void      DelayTick(HwTimerTick_t tickToDelay);
static uint32_t  SetCounterPrecision(uint32_t tickPerUsec);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//
//  VARIABLE DECLARATION WITH INITIAL VALUE DOESN'T WORK !!!
//  UNINTIALIZED VARIABLE DECLARATION ONLY !!!
//-----------------------------------------------------------------------------
#pragma arm section rwdata = "data_fast_ui"
#pragma arm section zidata = "data_fast_ui"

#if (_CPU_==CPU_ID0) && CHK_CODE_TYPE(CODE_MAIN)
static TimerPowerMode_t currentTimerPowerMode;
#endif

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------
extern void HldNvme_CountControllerBusyTime(void);
extern void CriticalWarningUpdate(void);

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    return timer clock source frequency\n

    @return  timer clock source frequency
**/
//-----------------------------------------------------------------------------
__inline uint32_t GetTimerSourceClockFreqMhz(void)
{
#if TIMER_CLOCK_SRC==SRC_OSC
    return OSC_FREQ_MHZ;
#elif TIMER_CLOCK_SRC==SRC_CPU
    return HalClock_GetFrequency(cClockTbgB)/2;
#else
    #error "Invalid timer clock source"
#endif
}

#if _CPU_==CPU_ID0    // CPU0 only
//-----------------------------------------------------------------------------
/**
    One-time initialization of timer H/W.\n
      - disable all timer interrupt\n
      - set timer0 for 10ms peridoc interrupt.\n
      - set timer1 for precise time measurement.\n

    @param[in] initMode     initialization mode

    @return  error codes
    @see     TIMER_TICK_PER_USEC
**/
//-----------------------------------------------------------------------------
Error_t HalTimer_OneTimeInit(InitBootMode_t initMode)
{
    if (initMode != cInitBootCpu0Boot)
    {
        // setup timer2, disabled, one-shot, 2Mhz tick,
        rCoutTimers.cntr2Ctrl.all = 0;
        rCoutTimers.cntr2Ctrl.b.CNTR_2_CLK_PRE = GetTimerSourceClockFreqMhz()/2;

        SetCounterPrecision(TIMER_TICK_PER_USEC);

        // mask all interrupts
        rCoutTimers.cpuIntrMsk[CPU_ID0].all = 0x1F;
        rCoutTimers.cpuIntrMsk[CPU_ID1].all = 0x1F;
        rCoutTimers.cpuIntrMsk[CPU_ID2].all = 0x1F;

        // clear all pending status
        CLR_TIMER_INTR_PENDING_STATUS(0, 0x1F);
        CLR_TIMER_INTR_PENDING_STATUS(1, 0x1F);
        CLR_TIMER_INTR_PENDING_STATUS(2, 0x1F);
    }

    #if CHK_CODE_TYPE(CODE_MAIN)
    currentTimerPowerMode = cTimerPmActive;
    #endif

    return cEcNoError;
}
#endif

#if ((_CPU_==CPU_ID0) || (_CPU_==CPU_ID2))   // CPU0 only
#if CHK_CODE_TYPE(CODE_MAIN)
#if (_CPU_==CPU_ID0)
//-----------------------------------------------------------------------------
/**
    H/W block level power management API

    @param[in]  powerMode        targeting pm mode to enter
    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalTimer_SetPowerMode(TimerPowerMode_t powerMode)
{
    // no set up if power mode is not changed.
    if (currentTimerPowerMode == powerMode) return cEcNoError;

    switch(powerMode)
    {
    case cTimerPmDeepSleep:
        break;
    case cTimerPmActive:
        HalTimer_OneTimeInit(cInitBootDeepPowerDown);

        //@@ temporary:
        // if nessersary, it will be moved into Kernerl context save/restore to keep KERNEL_TICK_PER_SEC value
        HalTimer_SetPeriodicInterruptFrequency(KERNEL_TICK_PER_SEC);
        break;
    default:
        break;
    }

    currentTimerPowerMode = powerMode;

    return cEcNoError;
}
#endif

//-----------------------------------------------------------------------------
/**
    Set periodic timer interrupt frequency.\n

    @param[in]  frequency in hertz unit

    @return   actual set frequency

    @see HalIrq_EnDisableInterrupt, IRQ_PERIODIC

    @remark HW Timer0 is reserved for periodic interrupt.
**/
//-----------------------------------------------------------------------------
uint32_t HalTimer_SetPeriodicInterruptFrequency(uint32_t hertz)
{
    uint32_t preScaler;
    uint32_t counter;
    Cntr0Ctrl_t ctrl0;

    preScaler = GetTimerSourceClockFreqMhz()/2; // set 0.5 usec counter resolution

    counter = 2*MHZ/hertz - 1;

    // timer0 for all CPU0, CPU1 and CPU2 but only CPU0 can set period.

    ctrl0.all = 0;
    ctrl0.b.CNTR_0_EN = 0;
    ctrl0.b.CNTR_0_TRIGGER_SRC = 0;
    ctrl0.b.CNTR_0_CLK_PRE = preScaler;
    ctrl0.b.CNTR_0_RW = 1; // free run
    rCoutTimers.cntr0Ctrl.all = ctrl0.all;  // set register at a time (to save code space)

    rCoutTimers.CNTR_0_INIT_CURR_CNT = counter;

    rCoutTimers.cntr0Ctrl.b.CNTR_0_EN = 1;

    return(hertz);
}

uint32_t HalTimer_SetSmartTimerInterruptFrequency(uint32_t hertz)
{
    uint32_t preScaler;
    uint32_t counter;
    Cntr3Ctrl_t ctrl3;

#if (_CPU_==CPU_ID0)
    // Clear busy check flags
    gSmartIOTimerFlag = FALSE;
    gSmartTimerMil    = FALSE;
#endif

    preScaler = GetTimerSourceClockFreqMhz()/2; // set 0.5 usec counter resolution

    counter = 2*MHZ/hertz - 1;

    // timer0 for all CPU0, CPU1 and CPU2 but only CPU0 can set period.

    ctrl3.all = 0;
    ctrl3.b.CNTR_3_EN = 0;
    ctrl3.b.CNTR_3_TRIGGER_SRC = 0;
    ctrl3.b.CNTR_3_CLK_PRE = preScaler;
    ctrl3.b.CNTR_3_RW = 1; // free run
    rCoutTimers.cntr3Ctrl.all = ctrl3.all;  // set register at a time (to save code space)

    rCoutTimers.CNTR_3_INIT_CURR_CNT = counter;

    rCoutTimers.cntr3Ctrl.b.CNTR_3_EN = 1;

    return(hertz);
}
#endif
#endif

#if ((_CPU_==CPU_ID0) || (_CPU_==CPU_ID2))
//-----------------------------------------------------------------------------
/**
    Set and enable free running timer resolution to measure precise time and delay.

    @param[in]  tickPerUsec   timer tick per mirco-second

    @return   actual set tick per mirco-second

    @remark HW Timer1 is reserved for periodic interrupt.
**/
//-----------------------------------------------------------------------------
uint32_t SetCounterPrecision(uint32_t tickPerUsec)
{
    uint32_t divider;
    Cntr1Ctrl_t ctrl1;

    divider = (GetTimerSourceClockFreqMhz()/tickPerUsec);

    ctrl1.all = 0;
    ctrl1.b.CNTR_1_EN = 0;
    ctrl1.b.CNTR_1_TRIGGER_SRC = 0;
    ctrl1.b.CNTR_1_CLK_PRE = divider;
    ctrl1.b.CNTR_1_RW = 1; // free run
    rCoutTimers.cntr1Ctrl.all = ctrl1.all;  // set register at a time (to save code space)

    rCoutTimers.CNTR_1_INIT_CURR_CNT = U32_MAX;
    rCoutTimers.cntr1Ctrl.b.CNTR_1_EN = 1;

    tickPerUsec = GetTimerSourceClockFreqMhz()/divider;

    return(tickPerUsec);
}
#endif

#if _CPU_==CPU_ID2
uint32_t HalTimer_SetCounterPrecision(uint32_t tickPerUsec)
{
    return SetCounterPrecision(tickPerUsec);
}
#endif

#if CHK_CODE_TYPE(CODE_MAIN)
#if _CPU_==CPU_ID0    // CPU0 only
//-----------------------------------------------------------------------------
/**
    Set one shot timer time and assert interrupt when timer is expired.

    @param[in]  usec   time out value in mirco-second unit

**/
//-----------------------------------------------------------------------------
void HalTimer_SetOneShotTimer(uint32_t usec)
{
    rCoutTimers.cntr2Ctrl.b.CNTR_2_EN = 0;
    rCoutTimers.CNTR_2_INIT_CURR_CNT = usec * 2;
    rCoutTimers.cntr2Ctrl.b.CNTR_2_EN = 1;
}

void HalTimer_SmartCheckTimer(uint32_t usec)
{
    rCoutTimers.cntr3Ctrl.b.CNTR_3_EN = FALSE;

    #if CHK_FW_TYPE(FW_AGING)
    if (smAgingDramInfo.AgingDramTag == AGING_DRAM_TEST_TAG)
    {
        rApb.uartPinCtrl.b.UART1_TX_OUT ^= ENABLE;
    }
    #endif

    if(!(++gSmartTimerMil % SMARTCHECK_TICK_PER_SEC))
    {
        // IO Busy timer check (Unit: 1 Second)
        if(gSmartIOTimerFlag)
        {
            gGetLogIOBusyTime++;
        }

        // Power on Hours check (Unit: 20 Minutes)
        if(gSmartTimerMil == PWRONHR_TICK_SCAL)
        {
            gSmartTimerMil = 0;
            gGetLogPowerOnHours++;
        }

        // Critical Warning Update (Unit: 1 Second)
        CriticalWarningUpdate();
    }

    rCoutTimers.cntr3Ctrl.b.CNTR_3_EN = TRUE;
}
#endif
#endif

#if _CPU_ == CPU_ID2    // CPU2 only
uint32_t HalTimer_SetTemperatureTimerInterruptFrequency(uint32_t hertz)
{
    uint32_t preScaler;
    uint32_t counter;
    Cntr4Ctrl_t ctrl4;

    preScaler = GetTimerSourceClockFreqMhz()/2; // set 0.5 usec counter resolution

    counter = 2*MHZ/hertz - 1;

    // timer0 for all CPU0, CPU1 and CPU2 but only CPU0 can set period.
    ctrl4.all = 0;
    ctrl4.b.CNTR_4_EN = 0;
    ctrl4.b.CNTR_4_TRIG_SRC = 0;
    ctrl4.b.CNTR_4_CLK_PRE = preScaler;
    ctrl4.b.CNTR_4_RW = 1; // free run

    rCoutTimers.cntr4Ctrl.all = ctrl4.all;  // set register at a time (to save code space)
    rCoutTimers.CNTR_4_INIT_CURR_CNT = counter;

    rCoutTimers.cntr4Ctrl.b.CNTR_4_EN = 1;
    return(hertz);
}

//-----------------------------------------------------------------------------
/**
    Timer for thermal throttle

    @param[in]  usec
**/
//-----------------------------------------------------------------------------
void HalTimer_ThermalThrottleTimer(uint32_t usec)
{
    smThermalTempUpdate = TRUE;
}

#endif

//-----------------------------------------------------------------------------
/**
    Read a current timer counter tick value.

    @return   current timer counter tick
**/
//-----------------------------------------------------------------------------
HwTimerTick_t HalTimer_ReadTick(void)
{
    return(rCoutTimers.CNTR_1_INIT_CURR_CNT);
}


//-----------------------------------------------------------------------------
/**
    Compute elapsed time in HW timer tick unit.\n

    @param[in]  lastTick    last tick value from

    @return   elapsed time tick
**/
//-----------------------------------------------------------------------------
HwTimerTick_t GetElapsedTick(HwTimerTick_t lastTick)
{
    uint32_t tick;

    tick = lastTick - HalTimer_ReadTick();

    return (tick);
}

//-----------------------------------------------------------------------------
/**
    Delay for at least the given number of HW timer tick.\n

    @param[in]  tickToDelay     tick to delay

    @remark Active wait, task is not suspended.
**/
//-----------------------------------------------------------------------------
void DelayTick(HwTimerTick_t tickToDelay)
{
    uint32_t lastTick;

    if (tickToDelay)
    {
        lastTick = HalTimer_ReadTick();
        while (GetElapsedTick(lastTick) < tickToDelay);
    }
}


//-----------------------------------------------------------------------------
/**
    Delay for at least the given number of milliseconds by using HW timer.

    @param[in]  milliSecToDelay     time to delay in milli-seconds unit

    @remark Active wait, task is not suspended.
**/
//-----------------------------------------------------------------------------
void HalTimer_DelayMsec(uint32_t milliSecToDelay)
{
    while (milliSecToDelay--)
    {
        DelayTick(MSEC2TICK(1));
    }
}

//-----------------------------------------------------------------------------
/**
    Delay for at least the given number of micro-seconds by using HW timer.

    @param[in]  microSecToDelay     time to delay in micro-seconds unit

    @remark Active wait, task is not suspended.
**/
//-----------------------------------------------------------------------------
void HalTimer_DelayUsec(uint32_t microSecToDelay)
{
    DelayTick(USEC2TICK(microSecToDelay));
}

//-----------------------------------------------------------------------------
/**
    Compute elapsed time in millisecond unit by using HW timer.\n

    example) compute elapsed time from (1) to (2).\n\n

        lastTick = HalTimer_ReadTick();                        --- (1)\n
        ...\n
        elapsedTime = HalTimer_GetElapsedTimeMsec(lastTick)        --- (2)\n

    @param[in]  lastTick    a snap-shot taken before this function is called.

    @return   elapsed time (msec)
**/
//-----------------------------------------------------------------------------
uint32_t HalTimer_GetElapsedTimeMsec(HwTimerTick_t lastTick)
{
    return TICK2MSEC(GetElapsedTick(lastTick));
}

//-----------------------------------------------------------------------------
/**
    Compute elapsed time in micro-second unit by using HW timer.\n

    example) compute elapsed time from (1) to (2).\n\n

        lastTick = HalTimer_ReadTick();                        --- (1)\n
        ...\n
        elapsedTime = HalTimer_GetElapsedTimeUsec(lastTick)        --- (2)\n

    @param[in]  lastTick    a snap-shot taken before this function is called.

    @return   elapsed time (usec)
**/
//-----------------------------------------------------------------------------
uint32_t HalTimer_GetElapsedTimeUsec(HwTimerTick_t lastTick)
{
    return TICK2USEC(GetElapsedTick(lastTick));
}

//-----------------------------------------------------------------------------
/**
    Timer interrupt enable/disable.

    @param[in]  sIrqId       software IRQ ID.
    @param[in]  state        ENABLE or DISABLE IRQ

    @return     desired physical timer irq enable/disable state
**/
//-----------------------------------------------------------------------------
uint32_t HalTimer_IrqEnDisable(IrqId_t sIrqId, uint32_t state)
{
    uint32_t orgState;
    uint32_t bitMask;

    switch(sIrqId)
    {
    case cIrqTimerPeriodic:
        bitMask = BIT(PERIODIC_TIMER_ID);
        break;
#if _CPU_==CPU_ID0
    case cIrqTimerOneshot:
        bitMask = BIT(ONESHOT_TIMER_ID);
        break;
    case cIrqTimerSmartCheck:
        bitMask = BIT(SMARTCHECK_TIMER_ID);
        break;
#endif

#if _CPU_==CPU_ID2
    case cIrqTimerThermalThrottle:
        bitMask = BIT(TEMPERATURECHECK_TIMER_ID);
        break;
#endif

    default:
        return DISABLE;
    }

    orgState = (rCoutTimers.cpuIntrMsk[_CPU_].all & bitMask) ? DISABLE : ENABLE;

    if (state==DISABLE)
    {
        SET_TIMER_INTR_MASK(_CPU_, bitMask);
    }
    else
    {
        CLR_TIMER_INTR_MASK(_CPU_, bitMask);
    }

    return orgState;
}

//-----------------------------------------------------------------------------
/**
    Timer interrupt service routine.

    @param[in]  irqHandler  interrupt request handler table

    @remark     as a workaround, only CPU0 handles periodic interrupt and asserts VIC to others.
**/
//-----------------------------------------------------------------------------
void HalTimer_IrqHandle(IrqHandler_t irqHandler[])
{
    IntrPendingStat_t intPending;

    while ((intPending.all = GET_TIMER_INTR_PENDING_STATUS(_CPU_) & (~(GET_TIMER_INTR_MASK(_CPU_))))!=0)
    {
        CLR_TIMER_INTR_PENDING_STATUS(_CPU_, intPending.all); // clear all pending interrupt

        if (intPending.b.CNTR_0_INT_PEND)
        {
            irqHandler[cIrqTimerPeriodic](0);

#if (_HOST_==HOST_NVME) && (_CPU_==CPU_ID0)
    #if CHK_CODE_TYPE(CODE_MAIN)
        HldNvme_CountControllerBusyTime();  // call this routine every 1ms to count controller busy time
    #endif
#endif
        }

#if _CPU_==CPU_ID0
        if (intPending.b.CNTR_2_INT_PEND)
        {
            irqHandler[cIrqTimerOneshot](0);
        }
        if (intPending.b.CNTR_3_INT_PEND)
        {
            irqHandler[cIrqTimerSmartCheck](0);
        }
#endif

#if _CPU_==CPU_ID2
        if (intPending.b.CNTR_4_INT_PEND)
        {
            irqHandler[cIrqTimerThermalThrottle](0);
        }
#endif
    }
}
