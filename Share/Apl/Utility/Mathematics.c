// ============================================================================
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
//! @file
//! @brief source file containing basic mathematical functions
//!
//=============================================================================

#pragma once

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"

#include "Mathematics.h"
//#include "mdebug.h"

static uint8_t mCrc8(uint8_t inCrc, uint8_t inData);

#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING`

uint8_t CalculateCrc(uint8_t const * pData, uint16_t dataBufferLength)
{
    uint8_t i;
    uint8_t crc=0;

    for (i=0;i<dataBufferLength;i++)
    {
        crc=mCrc8(crc,*pData++);
    }
    return crc;
}

uint8_t mCrc8(uint8_t inCrc, uint8_t inData )
{
    uint8_t   i;
    uint8_t   data;

    data = inCrc ^ inData;

    for ( i = 0; i < 8; i++ )
    {
        if (( data & 0x80 ) != 0 )
        {
            data = ((data<<1)& 0xFF);
            data ^= 0x07;
        }
        else
        {
            data = ((data<<1)& 0xFF);
        }
    }

    return data;
}

uint32_t CalculateHighestCommonDenominator(uint32_t value1, uint32_t value2)
{
    //Debug_AssertMessage(((value1 != 0) && (value2 != 0)),("CalculateHighestCommonDenominator error: either %d or %d is zero", value1, value2));

    while (value1 != value2)
    {
        if (value1 > value2)
        {
            value1 -= value2;
        }
        else
        {
            value2 -= value1;
        }
    }
    return value1;
}

uint32_t CalculateLeastCommonMultiple(uint32_t value1, uint32_t value2)
{
    return (value1 * value2) / CalculateHighestCommonDenominator(value1, value2);
}
