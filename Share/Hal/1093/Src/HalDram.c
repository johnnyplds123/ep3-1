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
//! @brief Pure HAL API definition for DRAM H/W control
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
// Package
#include "BuildPkg.h"
#include "SharePkg.h"

// Hardware Abstract Layer(HAL)
#include "HalCpu.h"
#include "HalTimer.h"
#include "HalRegConfig.h"
#include "RegDdrMc.h"
#include "RegPeripheral.h"
#include "RegAvs.h"

// Software Layer
#include "MemIo.h"
#include "MemTest.h"

// Self header
#include "HalMc5DdrInit.h"
#include "HalDram.h"

#include "FastSection.h"

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
//  Data declaration: Private or Public
//
//  VARIABLE DECLARATION WITH INITIAL VALUE DOESN'T WORK !!!
//  UNINTIALIZED VARIABLE DECLARATION ONLY !!!
//-----------------------------------------------------------------------------
#pragma arm section rwdata = "data_fast_ui"
#pragma arm section zidata = "data_fast_ui"
DramInfo_t  halDramInfo;

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
Error_t Dram_DdrOneTimeInit(void);
Error_t Dram_DdrRecovery(void);
Error_t Dram_DdrSelfRefresh(SelfRefreshMode_t PowDownMode);

void    Dram_MiscSetAvsVoltage(void);
U32     Dram_MiscGetOtpFuse(void);

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    HAL DRAM One-time Initialization
**/
//-----------------------------------------------------------------------------
Error_t HalDram_OneTimeInit(InitBootMode_t initMode)
{
    Error_t       error;
    HwTimerTick_t timeTick;

    timeTick = HalTimer_ReadTick();

    MEM_CLR((PVOID)&halDramInfo, sizeof(halDramInfo));
    halDramInfo.clockFreq  = TBG_A_FREQ_MHZ >> 1;
    halDramInfo.size       = (U32)_DRAM_SIZE_ * MBYTE;
    halDramInfo.type       = _DRAM_;
    halDramInfo.mode       = cDramPmActive;
    halDramInfo.dataWidth  = DRAM_DATA_WIDTH_IN_BIT;
    halDramInfo.numberOfCs = DRAM_NUMBER_OF_CS;
    D_PRINTF_DRAM("[DRAM] %d|%d|0x%2X|%d|%d|%d\n", halDramInfo.clockFreq, halDramInfo.size >> 20,
                                                   halDramInfo.type,      halDramInfo.mode,
                                                   halDramInfo.dataWidth, halDramInfo.numberOfCs);
    // AVS
    Dram_MiscSetAvsVoltage();
    HalTimer_DelayUsec(100);

    // handle DRAM_NONE
    #if (_DRAM_ == DRAM_NONE)
        return cEcNoError;

    // DDR init routine
    #elif ((_DRAM_ & DRAM_TYPE_MASK) == DRAM_DDR || \
           (_DRAM_ & DRAM_TYPE_MASK) == DRAM_LPDDR)
        error = Dram_DdrOneTimeInit();

        #if (HW_TYPE != HW_TYPE_EMUL)
            timeTick = HalTimer_ReadTick();
            MemTestRandU32((U32 *)DRAM_TEST_START_ADDRESS0, DRAM_TEST_SIZE, timeTick, NULL, 4);
            timeTick = HalTimer_ReadTick();
            MemTestRandU32((U32 *)DRAM_TEST_START_ADDRESS1, DRAM_TEST_SIZE, timeTick, NULL, 4);
        #endif

        D_PRINTF_DRAM("[DRAM] InitUs %u\n", HalTimer_GetElapsedTimeUsec(timeTick));

        return (error);
    #else
        #error "Invalid DRAM TYPE"
    #endif
}

//-----------------------------------------------------------------------------
/**
    DRAM Recovery
**/
//-----------------------------------------------------------------------------
Error_t HalDram_Recovery(void)
{
    Error_t error;
    U32     i;

    halDramInfo.mode = cDramPmActive;

    // AVS
    Dram_MiscSetAvsVoltage();
    for (i = 0; i < 1000; i++); // delay
    //HalTimer_DelayUsec(100);
    // 151022ben: ^^^this line casue PS4 hang at wake

    // handle DRAM_NONE
    #if (_DRAM_ == DRAM_NONE)
        return cEcNoError;

    // DDR recovery routine
    #elif ((_DRAM_ & DRAM_TYPE_MASK) == DRAM_DDR || \
           (_DRAM_ & DRAM_TYPE_MASK) == DRAM_LPDDR)
        error = Dram_DdrRecovery();

        return (error);
    #else
        #error "Invalid DRAM_TYPE"
    #endif
}

//-----------------------------------------------------------------------------
/**
    DRAM hardware block level power management API
**/
//-----------------------------------------------------------------------------
Error_t HalDram_SetPowerMode(DramPowerMode_t powerMode)
{
    // return if mode is not changed
    if (halDramInfo.mode == powerMode) return cEcNoError;

    switch(powerMode)
    {
    case cDramPmDeepSleep:          ///< Target Mode
        // Drain the buffer to DDR
        rDdrMc.ddrmcUserInitiateCmd0.b.WCB_DRAIN_REQ = 1;   // 0xF1000020[1]
        while (rDdrMc.memControllStat.b.WCB_EMPTY == 0);    // 0xF1000004[3]

        switch(halDramInfo.mode)    ///< Current Mode
        {
        case cDramPmActive:
            Dram_DdrSelfRefresh(cModeEnter);
            break;
        case cDramPmSelfRefresh:
            break;
        }
        break;

    case cDramPmSelfRefresh:        ///< Target Mode
        switch(halDramInfo.mode)    ///< Current Mode
        {
        case cDramPmDeepSleep:
            break;
        case cDramPmActive:
            Dram_DdrSelfRefresh(cModeEnter);
            break;
        }
        break;

    case cDramPmActive:             ///< Target Mode
        switch(halDramInfo.mode)    ///< Current Mode
        {
        case cDramPmDeepSleep:
            HalDram_Recovery();
            break;
        case cDramPmSelfRefresh:
            Dram_DdrSelfRefresh(cModeExit);
            break;
        }
        break;

    default:
        break;
    }

    halDramInfo.mode = powerMode;

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    DDR DRAM One-time Initialization
**/
//-----------------------------------------------------------------------------
Error_t Dram_DdrOneTimeInit(void)
{
    HalMc5_OneTimeInit(TBG_A_FREQ_MHZ);

    // Set the DDR Controller Register Port Priority (0xF1000084), bit[3:2] to 0x3.
    // This will set Port1, the NVMe Controller, to Critical priority.
    rDdrMc.portPriorityCtrl.b.PRIORITY_P1 = 3;

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    DDR DRAM Recovery
**/
//-----------------------------------------------------------------------------
Error_t Dram_DdrRecovery(void)
{
    HalMc5_Recovery(TBG_A_FREQ_MHZ);

    // Set the DDR Controller Register Port Priority (0xF1000084), bit[3:2] to 0x3.
    // This will set Port1, the NVMe Controller, to Critical priority.
    rDdrMc.portPriorityCtrl.b.PRIORITY_P1 = 3;

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    DDR DRAM Self-Refresh enter and exit
**/
//-----------------------------------------------------------------------------
Error_t Dram_DdrSelfRefresh(SelfRefreshMode_t PowDownMode)
{
    HwTimerTick_t           tick;
    DdrmcUserInitiateCmd0_t ddrmcUserInitiateCmd0;

    ddrmcUserInitiateCmd0.all = 0;

    tick = HalTimer_ReadTick();

    if (PowDownMode == cModeEnter)
    {
        // Drain the buffer to DDR
        rDdrMc.ddrmcUserInitiateCmd0.b.WCB_DRAIN_REQ = 1;   // 0xF1000020[1]
        while (rDdrMc.memControllStat.b.WCB_EMPTY == 0)     // 0xF1000004[3]
        {
            if(HalTimer_GetElapsedTimeUsec(tick) > 1 * USEC2SEC)    // 1sec
            {
                return cEcDramNotReady;
            }
        }

        ddrmcUserInitiateCmd0.b.DDRMC_CH0_CMD0 = 1;     // 0xF1000020[28] DDRMC Channel 0 Command 0.
        ddrmcUserInitiateCmd0.b.DDRMC_CS1_CMD0 = 1;     // 0xF1000020[25] DDRMC Chip Select 1 Command 0.
        ddrmcUserInitiateCmd0.b.DDRMC_CS0_CMD0 = 1;     // 0xF1000020[24] DDRMC Chip Select 0 Command 0.
        ddrmcUserInitiateCmd0.b.SR_REQ         = 1;     // 0xF1000020[7:6] Self-Refresh Request.

        // Enter Self Refresh 0xF1000020
        rDdrMc.ddrmcUserInitiateCmd0 = ddrmcUserInitiateCmd0;

        while (rDdrMc.dramStat.all != 0x00000055);      // F1000008h Waiting until self refresh is done
        {
            if(HalTimer_GetElapsedTimeUsec(tick) > 2 * USEC2SEC)    // 2sec
            {
                return cEcDramGeneral;
            }
        }
    }
    else
    {
        ddrmcUserInitiateCmd0.b.DDRMC_CH0_CMD0 = 1;     // 0xF1000020[28] DDRMC Channel 0 Command 0.
        ddrmcUserInitiateCmd0.b.DDRMC_CS1_CMD0 = 1;     // 0xF1000020[25] DDRMC Chip Select 1 Command 0.
        ddrmcUserInitiateCmd0.b.DDRMC_CS0_CMD0 = 1;     // 0xF1000020[24] DDRMC Chip Select 0 Command 0.
        ddrmcUserInitiateCmd0.b.SR_REQ         = 2;     // 0xF1000020[7:6] Self-Refresh Request.

        // Exit Self Refresh 0xF1000020
        rDdrMc.ddrmcUserInitiateCmd0 = ddrmcUserInitiateCmd0;

        while (rDdrMc.dramStat.all != 0x00000011);      // F1000008h Waiting until self refresh is clear

        HalMc5_DllResetAndUpdate();
    }

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    AVS Operation
**/
//-----------------------------------------------------------------------------
void Dram_MiscSetAvsVoltage(void)
{
    #ifdef SUPPORT_OTP_FUSE
    U32           otpFuse;
    HwTimerTick_t timeTick;

    timeTick = HalTimer_ReadTick();

    otpFuse = Dram_MiscGetOtpFuse();
    D_PRINTF_DRAM("-[OTP] %x|%u\n", otpFuse, HalTimer_GetElapsedTimeUsec(timeTick));
    #endif

    // DDR Clock Speed
    #if (TBG_A_FREQ_MHZ == 1600)
        #if ((_DRAM_ & DRAM_TYPE_MASK) == DRAM_DDR)
            /*
            rAvs.vddLimitCtrl.all = AVS_VDD_096V; // set VDD to 0.96V
            */
            rAvs.vddLimitCtrl.all = AVS_VDD_090V; // set VDD to 0.90V
        #else // LPDDR
            rAvs.vddLimitCtrl.all = AVS_VDD_090V; // set VDD to 0.90V
        #endif
    #else
        rAvs.vddLimitCtrl.all = AVS_VDD_090V; // set VDD to 0.90V
    #endif
}

//-----------------------------------------------------------------------------
/**
    OTP Fuse Operation
**/
//-----------------------------------------------------------------------------
U32 Dram_MiscGetOtpFuse(void)
{
    U32 otpFuse;

    //OTP read
    *(volatile U32*)0xA0003810  = (*(volatile U32*)0xA0003810 & ~0x0000003F) | 0x20;
    *(volatile U32*)0xA0003800 |= 0x00000004;
    *(volatile U32*)0xA0003800 |= 0x00000001;

    while(*(volatile U32*)0xA0003804 & 0x00000001)
    {
    }

    *(volatile U32*)0xA0003804 |= 0x00000001;
    otpFuse = *(volatile U32*)0xA0003814;

    return (otpFuse);
}

/* End of File */

