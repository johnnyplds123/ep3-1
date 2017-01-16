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
//! @brief Register configuration code
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "HalTimer.h"
#include "HalCpu.h"
#include "HalRegConfig.h"
#include "RegPcieCore.h"

#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define GET_PARAM32(X,I)        MAKE_U32(X[I+1], X[I])

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------

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
**/
//-----------------------------------------------------------------------------
static bool TestValueU16(uint32_t address, uint32_t testValue, uint32_t bitMask)
{
    return ((REG_U16(address)&bitMask) == testValue);
}

//-----------------------------------------------------------------------------
/**
    compare register with bit mask and wait for desired value set.

    @param[in]  address         register address
    @param[in]  testValue       test value
    @param[in]  bitMask         bit mask value
    @param[in]  timeOutUsec     timeout in Usec unit (no timeout if RCI_NO_TIMEOUT)

    @return     cEcNoError for no error, else timed out
**/
//-----------------------------------------------------------------------------
Error_t HalRci_WaitValueU16(uint32_t address, uint16_t testValue, uint16_t bitMask, uint32_t timeOutUsec)
{
    return HalRci_WaitEvent(TestValueU16, address, testValue, bitMask, timeOutUsec);
}

//-----------------------------------------------------------------------------
/**
**/
//-----------------------------------------------------------------------------
static bool TestValueU32(uint32_t address, uint32_t testValue, uint32_t bitMask)
{
    return ((REG_U32(address)&bitMask) == testValue);
}


//-----------------------------------------------------------------------------
/**
    compare register with bit mask and wait for desired value set.

    @param[in]  address         register address
    @param[in]  testValue       test value
    @param[in]  bitMask         bit mask value
    @param[in]  timeOutUsec     timeout in Usec unit (no timeout if RCI_NO_TIMEOUT)

    @return     cEcNoError for no error, else timed out
**/
//-----------------------------------------------------------------------------
Error_t HalRci_WaitValueU32(uint32_t address, uint32_t testValue, uint32_t bitMask, uint32_t timeOutUsec)
{
    return HalRci_WaitEvent(TestValueU32, address, testValue, bitMask, timeOutUsec);
}

//-----------------------------------------------------------------------------
/**
    check whether the event is done by a true-return and wait for desired status.

    @param[in]  pFuncBool       pointer to function which returns true or false
    @param[in]  param1          parameter 1 for the given function
    @param[in]  param2          parameter 2 for the given function
    @param[in]  param3          parameter 3 for the given function
    @param[in]  timeOutUsec     timeout in Usec unit (no timeout if RCI_NO_TIMEOUT)

    @return     cEcNoError for no error, else timed out
**/
//-----------------------------------------------------------------------------
Error_t HalRci_WaitEvent(bool (*pFuncBool)(uint32_t, uint32_t, uint32_t), uint32_t param1, uint32_t param2, uint32_t param3, uint32_t timeOutUsec)
{
    HwTimerTick_t tick;
    uint32_t elapsedTimeUsec;
    static const uint32_t timerSlice=1000;

    // test first and return if true
    if (pFuncBool(param1, param2, param3))
    {
        return cEcNoError;
    }

    tick = HalTimer_ReadTick();

    while ( !pFuncBool(param1, param2, param3) )
    {
        if (timeOutUsec!=RCI_NO_TIMEOUT)
        {
            elapsedTimeUsec = HalTimer_GetElapsedTimeUsec(tick);

            if (elapsedTimeUsec>timeOutUsec)
            {
                E_PRINTF(cEcTimeOut, "TIME_OUT:%X %X (*%8X)(%X, %X, %X)\n", tick, HalTimer_ReadTick(), (uint32_t)pFuncBool, param1, param2, param3);

                return (cEcTimeOut);
            }

            if (elapsedTimeUsec>timerSlice) // re-read timer to prevent timer overflow (some SoCs have 8 or 16bit counter)
            {
                tick = HalTimer_ReadTick();
                timeOutUsec -= elapsedTimeUsec;
            }
        }
    }

    return(cEcNoError);
}


