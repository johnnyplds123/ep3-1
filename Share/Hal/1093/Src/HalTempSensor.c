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
//! @brief Temperature sensor from SoC.
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharedVars.h"

#include "RegTempSns.h"
#include "HalCpu.h"
#include "HalI2c.h"
#include "HalTimer.h"
#include "HalTempSensor.h"

#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
static const float  cTemperatureGain   = (0.381);
static const float  cTemperatureOffset = (-280);
//static const float cTemperatureOffset = (-282.5);

static const float  cTemperatureHiLimit  = (80);
static const float  cTemperatureLowLimit = (58);

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
//  Imported data prototype without header include
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

#if _CPU_ == CPU_ID0
//-----------------------------------------------------------------------------
/**
    temperature initialization + power bring up
**/
//-----------------------------------------------------------------------------
void HalTempSensor_OneTimeInit(void)
{
    float   temperature;
    uint8_t thermalIdx;

    // Initial thermal variable
    for (thermalIdx = 0; thermalIdx < cThermalSourceCount; thermalIdx++)
    {
        smcThermalSource[thermalIdx] = U16_MAX;
    }

    // 1. Write a 0h to TSEN_START (RF0115180h [29])
    rTempSensor.tempSensorCtrl0.b.TSEN_START = 0;

    // 2. Reset the temperature sensor by writing a
    //    0h to TSEN_EN (RF0115180h [30]) and writing a
    //    1h to RF0115180h.
    rTempSensor.tempSensorCtrl0.b.TSEN_EN = 0;

    // 3. Program the static control registers {which are these?} to the appropriate settings.
    temperature = (cTemperatureHiLimit - cTemperatureOffset) / cTemperatureGain;
    rTempSensor.tempSensorCtrl1.b.TSEN_2_HOT_INT_THR = (U16)temperature;

    // 4. Write a 0h to RF0115180h.
    //rTempSensor.tempSensorCtrl0.all = 0;
    rTempSensor.tempSensorCtrl0.b.TSEN_ISO_EN = 0;
    rTempSensor.tempSensorCtrl0.b.TSEN_RESET  = 0;

    // 0, 1 -- manual mode;
    // 2, 3 -- auto mode
    // Only support 0 and 2
    rTempSensor.tempSensorCtrl0.b.TSEN_CAL = 2;

    // 5. Write a 1h to TSEN_EN (RF0115180h [30]).
    rTempSensor.tempSensorCtrl0.b.TSEN_EN    = 1;
    // 6. Write a 1h to TSEN_START (RF0115180h [29]).
    rTempSensor.tempSensorCtrl0.b.TSEN_START = 1;
}
#endif

//-----------------------------------------------------------------------------
/**
    Power off SoC temperature sensor
    // if you need to power-On + initialize plz call HalTemp_OneTimeInit(void)
**/
//-----------------------------------------------------------------------------
void HalTempSensor_PowerOff(void)
{
   // stop test
   rTempSensor.tempSensorCtrl0.b.TSEN_START  = 0;
   rTempSensor.tempSensorCtrl0.b.TSEN_EN     = 0;

   // reset IP
   rTempSensor.tempSensorCtrl0.b.TSEN_ISO_EN = 1;
   rTempSensor.tempSensorCtrl0.b.TSEN_RESET  = 1;
}

//-----------------------------------------------------------------------------
/**
    Read SoC Temperature

    @return temperature in degree celsius
**/
//-----------------------------------------------------------------------------
U32 HalTempSensor_GetTemperature(void)
{
    float   value;
    U32     cnt;
    U16     dout;

    cnt = 0;

    // 7. Wait for TSEN_RDY (RF0115188h [31]) to be asserted, and retrieve data.
    while (!rTempSensor.tempSensorStat1.b.TSEN_SAMPL_DONE_PEND)
    {
        if (cnt > 0x10000)
        {
            D_PRINTF("[TMPS] ErrTimeout\n");
            ASSERT(FALSE);
        }

        HalTimer_DelayUsec(1);
        cnt++;
    }

    // 9. Retrieve data from TSEN_DATA_RDY (RF011518Ch [11:0]).
    dout = rTempSensor.tempSensorStat1.b.TSEN_DATA_RDY;

    // convert the 12-bit temperature reading to temperature in Celsius:
    // value(in Celsius) = Output data * Gain + Offset
    // where in default Gain = 0.381; Offset = -280
    value = ((float)dout * cTemperatureGain) + cTemperatureOffset;

    rTempSensor.tempSensorCtrl1.b.TSEN_SAMPL_DONE_PEND_CLR = 1;
    rTempSensor.tempSensorCtrl1.b.TSEN_SAMPL_DONE_PEND_CLR = 0;

    return (value);
}
