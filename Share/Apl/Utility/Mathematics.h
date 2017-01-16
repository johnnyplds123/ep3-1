// ============================================================================
//
//     Copyright (c) Marvell Corporation 2013 -  All rights reserved
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
//! @file
//! @brief Mathematic interface
//!
//=============================================================================

#pragma once

//    #include "mGlobals.h"

    /***********************************************************************************
     *                                                                                 *
     *  ARM Compiler Intrinsics                                                        *
     *                                                                                 *
     ***********************************************************************************/
    unsigned char __clz(unsigned int val);
    unsigned int __ror(unsigned int val, unsigned int shift);

    /**
     *  round up V(alue) to the next M(ultiple) boundary
     *
     *  @param[in] value        Value
     *  @param[in] multiple     Multiple
     *
     *  @returns   Value' rounded up to the next multiple of 'Multiple'
    **/
    static __inline uint32_t RoundUpToNextMultipleBoundary(uint32_t value, uint32_t multiple)
    {
        return (multiple * ((value + multiple - 1) / multiple));
    }

    /**
     *  round down V(alue) to the previous M(ultiple) boundary
     *
     *  @param[in] value        Value
     *  @param[in] multiple     Multiple
     *
     *  @returns   Value' rounded down to the previous multiple of 'Multiple'
    **/
    static __inline uint32_t RoundDownToPreviousMultipleBoundary(uint32_t value, uint32_t multiple)
    {
        return (multiple * (value / multiple));

    }

    /**
     *  round up V(alue) to the next M(ultiple) boundary
     *
     *  @param[in] value        Value
     *  @param[in] multiple     Multiple
     *
     *  @returns   Value' rounded up to the next multiple of 'Multiple'
    **/
    static __inline uint64_t RoundUp64BitsToNextMultipleBoundary(uint64_t value, uint64_t multiple)
    {
        return (multiple * ((value + multiple - 1) / multiple));
    }


    /**
     *  Count the number of set bits
     *
     *  @param[in] value        Value
     *
     *  @returns   The number of bits set.
    **/
    static __inline int CountNumberOfOnes(uint32_t value)
    {
        value = value - ((value >> 1) & 0x55555555);
        value = (value & 0x33333333) + ((value >> 2) & 0x33333333);
        return (((value + (value >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
    }

    /**
     *  Count the number of leading zeros.
     *
     *  @param[in] value        Value
     *
     *  @returns   The number of leading zeros.
    **/
    static __inline uint8_t CountLeadingZeros(uint32_t value)
    {
        return __clz(value);
    }

    /**
     *  Calculates the minimum required number of bits to represent the given value
     *
     *  @param[in] value        Value
     *
     *  @returns   minimum number of bits required
    **/
    static __inline uint32_t CalculateMinimumNumberOfRequiredBits(uint32_t value)
    {
        return (32 - CountLeadingZeros(value));
    }


    /**
     *  Get the number of elements of an array
     *
     *  @param[in] array        The array
     *
     *  @returns   The number of elements
    **/
    #define GetNumberOfArrayElements(array) (sizeof(array) / sizeof(array[0]))

    /**
     *  Calculates an 8 bit CRC number from a given data buffer
     *
     *  @param[in] pData             The data buffer
     *  @param[in] dataBufferLength  The data buffer length
     *
     *  @returns   The Crc number calculated

    **/
    uint8_t CalculateCrc(uint8_t const * pData, uint16_t dataBufferLength);

        /**
     *  Converts a Qx value to an unsigned long value.
     *
     *  @param[in] x      Qx factor
     *  @param[in] value  Qx Value
     *
     *  @returns   U32 value
    **/
    #define ConvertQxToU32(x, value) ( (uint32_t)(value) / (1 << x) )

    /**
     *  Converts an unsigned long value to a Qx value.
     *
     *  @param[in] x      Qx factor
     *  @param[in] value  U32 Value
     *
     *  @returns   Qx value
    **/
    #define ConvertU32ToQx(x, value) ( (uint32_t)(value) * (1 << x) )

    /**
     *  Increment a circular value.
     *
     *  @param[in] value    value, to increase by one
     *  @param[in] maximum  maximum value, will wrap at this value
     *
     *  @returns   Incremented value
    **/
    #define CircularIncrement(value, maximum)   \
    value++;                                    \
    if (value == maximum)                       \
    {                                           \
        value = 0;                              \
    }                                           \

    /**
     *  Calculates the highest common denominator of two values
     *
     *  @param[in] value 1
     *  @param[in] value 2
     *
     *  @returns   highest common denominator
    **/
    uint32_t CalculateHighestCommonDenominator(uint32_t value1, uint32_t value2);

    /**
     *  Calculates least common multiple
     *
     *  @param[in] value 1
     *  @param[in] value 2
     *
     *  @returns   least common multiple
    **/
    uint32_t CalculateLeastCommonMultiple(uint32_t value1, uint32_t value2);

    /**
     *  Sets a specified number of the least significant bits to 1
     *
     *  @param[in] number of bits to set
     *
     *  @returns   the bitmask
    **/
    //lint -e648 : Overflow in computing constant for operation: 'unsigned shift left'
    #define BitMask(numberOfBits)          ((numberOfBits > 31) ? 0xffffffff : ((1UL << (numberOfBits))-1))

    /**
     *  Count the number of trailing Ones.
     *
     *  @param[in] value        Value
     *
     *  @returns   The number of trailing ones (or: the bitposition of the first least significant bit that is reset)
    **/
    static __inline uint8_t CountTrailingOnes(uint32_t value)
    {
#if (__TARGET_ARCH_ARM != 5)
        uint32_t reversed = __rbit(value);
        return __clz(~reversed);
#else
        return value;
#endif
    }

    /**
     *  Count the number of trailing Ones.
     *
     *  @param[in] value        Value
     *
     *  @returns   The number of trailing zeros (or: the bitposition of the first least significant bit that is set)
    **/
    static __inline uint8_t CountTrailingZeros(uint32_t value)
    {
#if (__TARGET_ARCH_ARM != 5)
        uint32_t reversed = __rbit(value);
        return __clz(reversed);
#else
        return value;
#endif
    }


