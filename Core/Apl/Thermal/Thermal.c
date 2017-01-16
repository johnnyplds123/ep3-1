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

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"

#include "RegPeripheral.h"
#include "RegClockPLL.h"
#include "HalTempSensor.h"
#include "HalI2c.h"

#include "SysInfo.h"
#include "AplSch.h"
#include "Thermal.h"

#include "FastSection.h"

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define UART_BAUDRATE_SET_COUNT     5

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static void Thermal_ThrottleSetClockDivider(U32 clkSourceDivider);
static void Thermal_InitI2cVars(void);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
#ifdef DBG_UART_DYNAMIC_BAUDRATE
const tTT_UART_SET gTTUartSet[UART_BAUDRATE_SET_COUNT] =
                    {{1,  6, 45, 45, 44, 44},
                     {2,  3, 45, 45, 44, 44},
                     {4,  2, 33, 36, 33, 33},
                     {8,  1, 32, 32, 36, 38},
                     {20, 1, 12, 13, 13, 13}};
#endif

tTEMP_MT_TH gCurrMtTh;
tTT_MT      gTTState;

// original setup for CPU & ECU divider count
         U32        gTTClkDivider_B;
volatile PllCtrl_t* gTTPllCtrl_B;

I2cDevice_t         gTemperatureI2cDevice;
I2cReadData12Bit_t  gTemperatureI2cReadData;
U8                  gTemperatureI2cWritedata;

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
    AplThermal_OneTimeInit

    @param[in]  initMode : power on initiailization mode
**/
//-----------------------------------------------------------------------------
void AplThermal_OneTimeInit(InitBootMode_t initMode)
{
    Thermal_InitI2cVars();

    // Interrupt init
    HalTimer_SetTemperatureTimerInterruptFrequency(TT_TICK_PER_SEC);
    HalIrq_RegisterHandler(cIrqTimerThermalThrottle, HalTimer_ThermalThrottleTimer, THERMAL_THROTTLE);

    // Variables init
    smThermalTempUpdate = FALSE;
    gTTState            = TT_MT_0;

    // Set temperature throttle MT threshold
    gCurrMtTh.mt1_th = TT_DEGREE_MT_1;
    gCurrMtTh.mt2_th = TT_DEGREE_MT_2;

    // PLL divider record
    gTTPllCtrl_B    = HalClock_GetPllRegisterBase(cClockTbgB);
    gTTClkDivider_B = gTTPllCtrl_B->ctrl1.b.PLL_CLKOUT_SE_DIV_SEL;
}

//-----------------------------------------------------------------------------
/**
    Thermal Throttle Entrance
**/
//-----------------------------------------------------------------------------
void AplThermal_ThrottleEntry(void)
{
    tTT_MT  nextState;
    U32     divider;
    U32     currTemperature;

    #if (THERAML_TEMPERATURE_FROM_SOC == TRUE)
    currTemperature = HalTempSensor_GetTemperature();
    smcThermalSource[cThermalSourceSoc] = currTemperature;
    #else
    currTemperature = AplThermal_GetI2cTemperature();
    smcThermalSource[cThermalSourceI2c] = currTemperature;
    #endif

    #if (THERMAL_THROTTLE_ADAPTIVE_MODE == ENABLE)
    {
        if      (currTemperature <  TT_DEGREE_MT_0) smFioInfo.reqTMLevel = 0;
        else if (currTemperature >= TT_DEGREE_MT_2) smFioInfo.reqTMLevel = TT_ADAPTIVE_LEVEL;
        else
        {
            U8 tmpTemperature = (currTemperature - TT_DEGREE_MT_0) / TT_ADAPTIVE_LEVEL * TT_ADAPTIVE_LEVEL + TT_DEGREE_MT_0;
            if (tmpTemperature == currTemperature)
            {
                smFioInfo.reqTMLevel = (currTemperature - TT_DEGREE_MT_0) / TT_ADAPTIVE_LEVEL;
            }
        }
    }
    #endif

    nextState = gTTState;
    switch (gTTState)
    {
    case TT_MT_0:
        if      (currTemperature >= gCurrMtTh.mt2_th)                      nextState = TT_MT_2;
        else if (currTemperature >= gCurrMtTh.mt1_th)                      nextState = TT_MT_1;
        break;
    case TT_MT_1:
        if      (currTemperature >= gCurrMtTh.mt2_th)                      nextState = TT_MT_2;
        else if (currTemperature < (gCurrMtTh.mt1_th - TT_DEGREE_OFFSET))  nextState = TT_MT_0;
        break;
    case TT_MT_2:
        if      (currTemperature <= (gCurrMtTh.mt1_th - TT_DEGREE_OFFSET)) nextState = TT_MT_0;
        else if (currTemperature <= (gCurrMtTh.mt2_th - TT_DEGREE_OFFSET)) nextState = TT_MT_1;
        break;
    default:
        ASSERT(0);
    }

    // Set parameter
    if      (nextState == TT_MT_2)  divider = 20;  // CPU 25MHz ECU 50MHz
    else if (nextState == TT_MT_1)  divider = 2;   // CPU 250MHz
    else                            divider = 1;   // CPU 500MHz ECU 1000MHz

    TT_PROTECT_START
    #if (THERMAL_THROTTLE == ENABLE)
    if (nextState != gTTState)
    {
        Thermal_ThrottleSetClockDivider(divider);
        gTTState = nextState;
    }
    #endif
    smThermalTempUpdate = FALSE;
    TT_PROTECT_END
}

//-----------------------------------------------------------------------------
/**
    Thermal Throttle exception handler
**/
//-----------------------------------------------------------------------------
void AplThermal_ThrottleException(void)
{
    #if (THERMAL_THROTTLE == ENABLE)
    if (gTTState != TT_MT_0)
    {
        TT_PROTECT_START;
        Thermal_ThrottleSetClockDivider(1);
        gTTState = TT_MT_0;
        TT_PROTECT_END;
    }
    #endif
}

//-----------------------------------------------------------------------------
/**
    Thermal Throttle Clock throttling

    @param[in]  clkSourceDivider : clock divider
**/
//-----------------------------------------------------------------------------
static void Thermal_ThrottleSetClockDivider(U32 clkSourceDivider)
{
    #ifdef DBG_UART_DYNAMIC_BAUDRATE
    HwTimerTick_t timeStamp;
    U8            uartBRId;

    // Info CPU0/1 print lock
    smUartDynamic.Lock = TRUE;

    timeStamp = HalTimer_ReadTick();
    // Waiting for CPU0/1
    while ((smUartDynamic.Printing[CPU_ID0] || smUartDynamic.Printing[CPU_ID1]) &&
           (HalTimer_GetElapsedTimeMsec(timeStamp) < 5000));

    M_PRINTF("[THML] SwitchPllB|%d\n", clkSourceDivider);

    // (((32 FIFO + 1 shift) * 10bits * 1000000us) / 921600) + 1 = 359
    HalCpu_DelayUsec(359);

    // Reset UART FIFO and wait for shift register or CPU halt
    rUart0.uartCtrl.b.TXFIFO_RST = 1;   // Reset UART FIFO
    rUart0.uartCtrl.b.TXFIFO_RST = 0;

    timeStamp = HalTimer_ReadTick();
    // Waiting TX holding register empty
    while((rUart0.uartStat.b.TX_EMPTY == 0) && (HalTimer_GetElapsedTimeMsec(timeStamp) < 5000));

    // follow SDK 200us * (115200/921600) = 25us, wait for TX shift-register empty
    HalCpu_DelayUsec(25);
    #endif

    rClockPll.clkSelectioCtrl.b.CLK_SEL_PLLB_OSC = 1;
    HalCpu_DelayUsec(2);

    gTTPllCtrl_B->ctrl1.b.PLL_BYPASS_EN         = 1;                                    HalCpu_DelayUsec(2);
    gTTPllCtrl_B->ctrl3.b.PLL_RST               = 1;                                    HalCpu_DelayUsec(2);
    gTTPllCtrl_B->ctrl1.b.PLL_CLKOUT_SE_DIV_SEL = gTTClkDivider_B * clkSourceDivider;   HalCpu_DelayUsec(2);
    gTTPllCtrl_B->ctrl3.b.PLL_RST               = 0;                                    HalCpu_DelayUsec(100);
    gTTPllCtrl_B->ctrl1.b.PLL_BYPASS_EN         = 0;                                    HalCpu_DelayUsec(2);

    rClockPll.clkSelectioCtrl.b.CLK_SEL_PLLB_OSC = 0;
    HalCpu_DelayUsec(2);

    // Re-initial tickPerUsec
    HalTimer_SetCounterPrecision(TIMER_TICK_PER_USEC);

    // Re-initial CPU2 PeriodTimer
    HalTimer_SetTemperatureTimerInterruptFrequency(TT_TICK_PER_SEC);

    // Re-initial Smart Timer
    HalTimer_SetSmartTimerInterruptFrequency(SMARTCHECK_TICK_PER_SEC);

    // Re-initial CPU0 PeriodTimer
    HalTimer_SetPeriodicInterruptFrequency(KERNEL_TICK_PER_SEC);

    #ifdef DBG_UART_DYNAMIC_BAUDRATE
    for (uartBRId = 0; uartBRId < UART_BAUDRATE_SET_COUNT; uartBRId++)
    {
        if (gTTUartSet[uartBRId].ClkDividerSource == clkSourceDivider)
        {
            break;
        }
    }
    if (uartBRId == UART_BAUDRATE_SET_COUNT)
    {
        ASSERT(0);
    }

    rUart0.progOvsmplStack1.b.BAUD_RATE_DIV = gTTUartSet[uartBRId].ClkDividerBaudrate;
    rUart0.progOvsmplStack1.b.POSSR_ENT_1   = gTTUartSet[uartBRId].OverSampleSet[0];
    rUart0.progOvsmplStack1.b.POSSR_ENT_2   = gTTUartSet[uartBRId].OverSampleSet[1];
    rUart0.progOvsmplStack2.b.POSSR_ENT_3   = gTTUartSet[uartBRId].OverSampleSet[2];
    rUart0.progOvsmplStack2.b.POSSR_ENT_4   = gTTUartSet[uartBRId].OverSampleSet[3];

    // restore print lock flag
    smUartDynamic.Lock = FALSE;
    #endif
}

//-----------------------------------------------------------------------------
/**
    Thermal_InitI2cVars
**/
//-----------------------------------------------------------------------------
static void Thermal_InitI2cVars(void)
{
    S8 tempertureDegC;

    gTemperatureI2cDevice.port         = I2cPortAccessOrder[I2C_ORDER_PRIMARY];
    gTemperatureI2cDevice.slaveAddress = I2C_ADDRESS_TI_TMP102;
    gTemperatureI2cWritedata           = I2C_POINTER_REGISTER_TEMPERATURE;

    MEM_CLR(&gTemperatureI2cReadData.all[0], sizeof(I2cReadData12Bit_t));
    tempertureDegC = AplThermal_GetI2cTemperature();

    // should have a define to saperate Basic Management
    ///<
    #if (_GENERATION_ == GENERATION_EP3)
    if (tempertureDegC == (S8)S8_MIN)
    {
        D_PRINTF("[THML] I2c|UseSecondaryPort\n");
        gTemperatureI2cDevice.port = I2cPortAccessOrder[I2C_ORDER_SECONDARY];
        MEM_CLR(&gTemperatureI2cReadData.all[0], sizeof(I2cReadData12Bit_t));
        tempertureDegC = AplThermal_GetI2cTemperature();
    }
    #else
    // todo
    #endif
    ///>

    D_PRINTF("[THML] I2c|%03dDegC\n", tempertureDegC);
    smcThermalSource[cThermalSourceI2c] = tempertureDegC;
}

//-----------------------------------------------------------------------------
/**
    Thermal_GetI2cTemperature
**/
//-----------------------------------------------------------------------------
S8 AplThermal_GetI2cTemperature(void)
{
    Error_t error;
    U16 tempertureHex;
    S8  tempertureDegC;

    error = HalI2c_Read(&gTemperatureI2cDevice,
                        &gTemperatureI2cWritedata,       sizeof(gTemperatureI2cWritedata),
                        &gTemperatureI2cReadData.all[0], sizeof(gTemperatureI2cReadData));

    if (error)
    {
        D_PRINTF("[THML] ErrTemp.\n");
        return (S8)S8_MIN; //No error hanlde implemented yet
    }

    tempertureHex  = ((gTemperatureI2cReadData.b.bHighByte << 8) |
                       gTemperatureI2cReadData.all[1]) >> 4;
    tempertureDegC = (U8)(tempertureHex >> 4);
    if (gTemperatureI2cReadData.b.bSign) tempertureDegC |= B_7; //handle the sign bit

    return tempertureDegC;
}

/* End of File */
