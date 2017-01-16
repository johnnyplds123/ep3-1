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
//! @brief CPU control API codes
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "ClockTree.h"
#include "HalSys.h"
#include "HalCpu.h"
#include "HalIrq.h"
#include "HalClock.h"
#include "HalDram.h"
#include "HalPm.h"
#include "HalTimer.h"
#include "HalUart.h"
#include "HalSflash.h"
#include "HalIpc.h"
#include "HalGpio.h"
#include "HalDmac.h"
#include "HalPciePhy.h"
#include "HalPcieLink.h"
#include "HalI2c.h"
#include "HalTempSensor.h"

#include "RegApb.h"
#include "RegPciePhy.h"
#include "RegClockPLL.h"
#include "SharedVars.h"
#include "RegAvs.h"

#include "MemIo.h"

#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//
//  VARIABLE DECLARATION WITH INITIAL VALUE DOESN'T WORK!!!
//  UNINITIALZED VARIABLE DECLARATION ONLY !!!
//-----------------------------------------------------------------------------
#pragma arm section rodata = "data_perf_ui"
#pragma arm section rwdata = "data_perf_ui"
#pragma arm section zidata = "data_perf_ui"

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------
//extern uint32_t sIrqCount;         ///< to count how many IRQs happened
//extern uint32_t sIrqServiceCount;  ///< to count how many IRQs are serviced
//extern uint32_t sFiqCount;         ///< to count how many FIQs happened

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
    Get a boot mode

    @param  void

    @return boot mode
**/
//-----------------------------------------------------------------------------
InitBootMode_t Hal_GetInitBootMode(void)
{
#if _CPU_==CPU_ID0    // CPU0 only
    InitBootMode_t initBootMode = cInitBootCold;

    if (!rPciePhy.laneStat1.b.PM_TXDCLK_PCLK_EN)
    {
        initBootMode = cInitBootOffLine;
    }
    else if (rClockPll.WSIGN == 0xAB)
    {
        return cInitBootDeepPowerDown;
    }
    else if (MemReadU32(SHARED_TCM_OTF_FLG_ADDR) == REBOOT_FLAG)
    {
        initBootMode = cInitBootFwUpdated;
    }
    else
    {
        initBootMode = cInitBootCold;
    }

    smInitBootMode = initBootMode;
    return(initBootMode);
#else
    return((InitBootMode_t)smInitBootMode);
#endif
}

//-----------------------------------------------------------------------------
/**
    Get a SoC revision number

    @return revision number
**/
//-----------------------------------------------------------------------------
uint32_t Hal_GetRevision(void)
{
    return(rApb.dbgCtrl1.b.C_REV);
}

#if _CPU_==CPU_ID0 && CHK_CODE_TYPE(CODE_MAIN)
//-----------------------------------------------------------------------------
/**
    Dump HW information
**/
//-----------------------------------------------------------------------------
void Hal_DumpInfo(void)
{
    M_PRINTF("\n [CLK] OSC=%d\n"
             "TBGA|%4d, MC5|%4d\n"
             "TBGB|%4d, CPU|%4d\n"
             "TBGC|%4d, NFC|%4d\n"
             "TBGD|%4d, ECC|%4d\n",
             HalClock_GetFrequency(cClockOsc),
             HalClock_GetFrequency(cClockTbgA), HalClock_GetFrequency(cClockTbgA)/4,
             HalClock_GetFrequency(cClockTbgB), HalClock_GetFrequency(cClockTbgB)/1,
             HalClock_GetFrequency(cClockTbgC), HalClock_GetFrequency(cClockTbgC)/2,
             HalClock_GetFrequency(cClockTbgD), HalClock_GetFrequency(cClockTbgD)/1);

    //M_PRINTF("\n[INTERRUPT]\n"
    //         "FIQ=%10u, IRQ=%10u, %10u\n", sFiqCount, sIrqCount, sIrqServiceCount);

    M_PRINTF(" [AVS] %x\n", rAvs.vddLimitCtrl.all);
}
#endif

//-----------------------------------------------------------------------------
/**
    HAL pre-one time initialization before main() called.

    @param[in]  initMode        initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t Hal_PreOneTimeInit(InitBootMode_t initMode)
{
#if _CPU_==CPU_ID0    // CPU0 only

    HalGpio_OneTimeInit(initMode);      // GPIO pin initialization
    HalUart_OneTimeInit(initMode);      // UART
    HalClock_OneTimeInit(initMode);     // CLOCK tree
    HalCpu_OneTimeInit(initMode);       // CPU
    HalTimer_OneTimeInit(initMode);     // TIMER

    HalSflash_OneTimeInit(initMode);    // Serial flash
    HalDram_OneTimeInit(initMode);      // DRAM

#elif _CPU_==CPU_ID2 // CPU2 only

    HalI2c_OneTimeInit(initMode);       // I2C
    HalCpu_OneTimeInit(initMode);       // CPU2
    HalSflash_OneTimeInit(initMode);    // Serial flash

#else                // CPU1 only

    HalCpu_OneTimeInit(initMode);       // CPU1

#endif
    return(cEcNoError);
}

#if _CPU_==CPU_ID0    // CPU0 only
//-----------------------------------------------------------------------------
/**
    HAL one time initialization after main() called. (CPU1 is not started yet)

    @param[in]  initMode        initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t Hal_OneTimeInit(InitBootMode_t initMode)
{
    #if CHK_CODE_TYPE(CODE_MAIN)
    HalIrq_OneTimeInit(initMode);
    HalIpc_OneTimeInit(initMode);

    #if (CO_SUPPORT_DEVICE_POWER_MANAGEMENT == TRUE)
    HalPm_OneTimeInit(initMode);
    #endif

    HalTempSensor_OneTimeInit();
    #endif

    return(cEcNoError);
}
#endif
