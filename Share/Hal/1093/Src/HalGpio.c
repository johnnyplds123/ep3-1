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
//! @brief IPC control API codes
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "RegApb.h"
#include "HalGpio.h"

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
static __inline void UpdatePinsFromShadow(uint32_t pinNo);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//
//  VARIABLE DECLARATION WITH INITIAL VALUE DOESN'T WORK !!!
//  UNINTIALIZED VARIABLE DECLARATION ONLY !!!
//-----------------------------------------------------------------------------
#pragma arm section rwdata = "data_fast_ui"
#pragma arm section zidata = "data_fast_ui"

static GpioPowerMode_t      currentGpioPowerMode;
static TestpinOutputEn_t    saveGpioReg;

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
    One time Initialization of GPIO HW.\n

    @param[in]  initMode        initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalGpio_OneTimeInit(InitBootMode_t initMode)
{
    if (initMode != cInitBootCpu0Boot)
    {
        uint32_t tmpReg = 0;

        HalGpio_OutputSetMux(BYTE_MASK);    // 0xFF for TESTPIN_OUT Group
        HalGpio_OutputDisable(WORD_MASK);   // Set All GPIO Input

        #if (_GENERATION_ == GENERATION_CX2)
        HalGpio_OutputEnable(B_4 | B_6);
        HalGpio_WritePin(4, ENABLE);
        HalGpio_WritePin(6, ENABLE);
        #endif
    }

    currentGpioPowerMode = cGpioPmActive;

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    H/W block level power management API

    @param[in]  powerMode        targeting pm mode to enter

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalGpio_SetPowerMode(GpioPowerMode_t powerMode)
{
    // no set up if power mode is not changed.
    if (currentGpioPowerMode == powerMode) return cEcNoError;

    switch(powerMode)
    {
    case cGpioPmDeepSleep:
        saveGpioReg.all = rApb.testpinOutputEn.all;     // save PIN_MUX_GRP_SEL and TESTPIN_OE
        break;
    case cGpioPmActive:
        rApb.testpinOutputEn.all = saveGpioReg.all;     // restore PIN_MUX_GRP_SEL and TESTPIN_OE
        break;
    default:
        break;
    }

    currentGpioPowerMode = powerMode;

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Read all power on latched Test pins' status

    @param  void

    @return latched test pins status (bit0=pin0, bit1=pin1, ... bit31=pin31)
**/
//-----------------------------------------------------------------------------
uint32_t HalGpio_ReadAllLatchedPin(void)
{
    return 0;
}

//-----------------------------------------------------------------------------
/**
    Read all test pins' status (real time).

    @return current test pins status (bit0=pin0, bit1=pin1, ... bit31=pin31)
**/
//-----------------------------------------------------------------------------
uint16_t HalGpio_ReadAllPin(void)
{
    return rApb.testpinIn.b.TESTPIN_IN;
}

//-----------------------------------------------------------------------------
/**
    Read test pin

    @return current test pins status
**/
//-----------------------------------------------------------------------------
uint16_t HalGpio_ReadPin(uint32_t pinNo)
{
    return (rApb.testpinIn.b.TESTPIN_IN & BIT(pinNo)) >> pinNo;
}

//-----------------------------------------------------------------------------
/**
    Read test out pin

    @return current test pins status
**/
//-----------------------------------------------------------------------------
uint16_t HalGpio_ReadOutPin(uint32_t pinNo)
{
    return (rApb.testpinOutput.b.TESTPIN_OUT & BIT(pinNo)) >> pinNo;
}

//-----------------------------------------------------------------------------
/**
    Write a high value to test pin.

    @param[in]  pinNo     GPIO pin number
**/
//-----------------------------------------------------------------------------
void HalGpio_AssertPin(uint32_t pinNo)
{
#if 0
    BIT_SET(gpioShadow, BIT(pinNo));

    UpdatePinsFromShadow(pinNo);
#else
    BIT_SET(rApb.testpinOutput.b.TESTPIN_OUT, BIT(pinNo));
#endif
}

//-----------------------------------------------------------------------------
/**
    Write a low value to test pin.

    @param[in]  pinNo     GPIO pin number
**/
//-----------------------------------------------------------------------------
void HalGpio_DeassertPin(uint32_t pinNo)
{
#if 0
    BIT_CLR(gpioShadow, BIT(pinNo));

    UpdatePinsFromShadow(pinNo);
#else
    BIT_CLR(rApb.testpinOutput.b.TESTPIN_OUT, BIT(pinNo));
#endif
}

//-----------------------------------------------------------------------------
/**
    Toggle a test pin.

    @param[in]  pinNo     GPIO pin number
**/
//-----------------------------------------------------------------------------
void HalGpio_TogglePin(uint32_t pinNo)
{
#if 0
    BIT_XOR( gpioShadow, BIT(pinNo) );

    UpdatePinsFromShadow(pinNo);
#else
    BIT_XOR(rApb.testpinOutput.b.TESTPIN_OUT, BIT(pinNo));
#endif
}

//-----------------------------------------------------------------------------
/**
    Write a high or low value to test pin.

    @param[in]  pinNo     GPIO pin number
    @param[in]  value     value to write

**/
//-----------------------------------------------------------------------------
void HalGpio_WritePin(uint32_t pinNo, uint32_t value)
{
    if (value == 0)
    {
        HalGpio_DeassertPin(pinNo);
    }
    else
    {
        HalGpio_AssertPin(pinNo);
    }
}

//-----------------------------------------------------------------------------
/**
    GPIO/Test pins output enable,

    @param[in]  pinBits     output enable pin bits

    @return error code
**/
//-----------------------------------------------------------------------------
void HalGpio_OutputEnable(uint32_t pinBits)
{
    //rApb.testpinOutputEn.b.PIN_MUX_GRP_SEL = 0x44;         // 0xFF for TESTPIN_OUT Group
    BIT_SET(rApb.testpinOutputEn.b.TESTPIN_OE, pinBits);    // BIT [15:0] Testpin Out.
}

//-----------------------------------------------------------------------------
/**
    GPIO/Test pins output disable,

    @param[in]  pinBits     output disable pin bits

**/
//-----------------------------------------------------------------------------
void HalGpio_OutputDisable(uint32_t pinBits)
{
    //rApb.testpinOutputEn.b.PIN_MUX_GRP_SEL = 0x0;          // 0x00 change to default
    BIT_CLR(rApb.testpinOutputEn.b.TESTPIN_OE, pinBits);    // BIT [15:0] Testpin Out.
}

//-----------------------------------------------------------------------------
/**
    GPIO/Test pins output disable,

    @param[in]  pinBits     output disable pin bits

**/
//-----------------------------------------------------------------------------
void HalGpio_OutputSetMux(uint8_t value)
{
    rApb.testpinOutputEn.b.PIN_MUX_GRP_SEL = value;         // 0x00 change to default
}

//-----------------------------------------------------------------------------
/**
    GPIO/Test pins output disable,

    @param[in]  pinBits     output disable pin bits

**/
//-----------------------------------------------------------------------------
#if 0
void UpdatePinsFromShadow(uint32_t pinNo)
{
    if(pinNo > 16)
    {
    }
    else
    {
        rApb.testpinOutput.b.TESTPIN_OUT = TO_U16(gpioShadow);
    }
}
#endif
