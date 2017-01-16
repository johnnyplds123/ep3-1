#pragma once
// ============================================================================
//
//     Copyright (c) Marvell Corporation 2000-2015 -  All rights reserved
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
//! @file BitArray.h
//! @brief A generic Bit Array implementation
//!
//=============================================================================
/***********************************************************************************
*                                                                                  *
* Dependencies                                                                     *
*                                                                                  *
***********************************************************************************/
#pragma once

#include "Mathematics.h"


/***********************************************************************************
*                                                                                  *
* Public Data Type Definitions                                                     *
*                                                                                  *
***********************************************************************************/
/// structure that holds the state information of a bit array
typedef struct BitArray_t
{
    uint32_t* pArrayBuffer;         ///< start address of the bit array
    uint32_t numberOfEntries;       ///< number of entries
    uint32_t mask;                  ///< contains entrySizeInBits set bits ( for efficiency reasons )
    uint8_t entrySizeInBits;        ///< number of bits per entry. Must be smaller then 33
}BitArray_t;

/// enum used as parameter for the SearchNextBitInArray function
typedef enum BitToSearch_t
{
    cFindFirstResetBit,
    cfindFirstSetBit
}BitToSearch_t;

/***********************************************************************************
*                                                                                  *
* Public Function Declarations                                                     *
*                                                                                  *
***********************************************************************************/

/**
 *  Initializes a BitArray_t structure
 *
 *  @param[in] pArrayBuffer start address of bit array
 *  @param[in] pBitArray: pointer to the BitArray_t structure to be initialized
 *  @param[in] numberOfEntries: number of entries of the bit array
 *  @param[in] entrySizeInBits: number of bits in one bit array entry
 *
 *  @returns   nothing
 */
void BitArray_Initialize(uint32_t* const pArrayBuffer, BitArray_t* const pBitArray, uint32_t numberOfEntries, uint8_t entrySizeInBits);

/**
 *  Insert a value in a bit array
 *
 *  @param[in] pBitArray: pointer to the BitArray_t structure used to identify the array
 *  @param[in] index: index into the bitarray
 *  @param[in] value: value to insert in to the bitarray at index. When the value is equal or larger than 2^entrySizeInBits, only the lower bits will get saved
 *
 *  @returns   nothing
 */
static __inline void BitArray_Insert(BitArray_t const * const pBitArray, uint32_t index, uint32_t value);

/**
 *  Retrieves a value from a bit array
 *
 *  @param[in] pBitArray: pointer to the BitArray_t structure used to identify the array
 *  @param[in] index: index into the bitarray
 *
 *  @returns   the contents at index
**/
static __inline uint32_t BitArray_Get(BitArray_t const * const pBitArray, uint32_t index);

/**
 * Searches the next bit that is set or reset in an array ( searching from the least significant bit upwards )
 *
 * @param[in] array to search in
 * @param[in] array size in number of bits ( does not have to be a multiple of 8 )
 * @param[in] bit index to start the search at
 * @param[in] whether to search for a set bit of a reset bit
 *
 * @return The next bit position that is zero or set. If there is no bit found, numberOfBitsInBuffer is returned
 */
uint32_t SearchNextBitInArray(const uint32_t buffer[], const uint32_t numberOfBitsInBuffer, const uint32_t bitIndexToStart, const BitToSearch_t bitToSearch);

/**
 * Check whether a bit indicated by its bitIndex is set in an array
 *
 * @param[in] array to search in
 * @param[in] index of the bit to check
 *
 * @return true if it is set
 */
static __inline bool BitArray_IsBitSetInArray(uint32_t bitArray[], uint32_t bitIndex);

/**
 * Set a bit, indicated by its bitIndex, to 1 in an array
 *
 * @param[in] array to set the bit in
 * @param[in] index of the bit to set
 *
 * @return nothing
 */
static __inline void BitArray_SetBitInArray(uint32_t bitArray[], uint32_t bitIndex);

/**
 * Set a bit, indicated by its bitIndex, to 0 in an array
 *
 * @param[in] array to reset the bit in
 * @param[in] index of the bit to reset
 *
 * @return nothing
 */
static __inline void BitArray_ResetBitInArray(uint32_t bbbMask[], uint32_t bitIndex);
//-----------------------------------------------------------------------------
//  Inline functions
//-----------------------------------------------------------------------------


static __inline uint32_t BitArray_Get(BitArray_t const * const pBitArray, uint32_t index)
{
    uint32_t returnValue;
    uint32_t bitNumber;
    uint32_t startWordIndex;
    uint32_t startWordOffset;
    uint32_t valueStoredInFirstWord;
    int32_t remainingBitsInNextWord;

    bitNumber = index * pBitArray->entrySizeInBits;

    startWordIndex = bitNumber / (sizeof(uint32_t)*8);
    startWordOffset = bitNumber % (sizeof(uint32_t)*8);

    valueStoredInFirstWord = pBitArray->pArrayBuffer[startWordIndex];

    remainingBitsInNextWord = (int32_t)(((int32_t)startWordOffset + (int32_t)pBitArray->entrySizeInBits) - ((int32_t)sizeof(uint32_t)*8));

    ASSERT(index < pBitArray->numberOfEntries);

    if (remainingBitsInNextWord <= 0)
    {
        const uint32_t remainingBitsInThisWord = (uint32_t)(-remainingBitsInNextWord);

        valueStoredInFirstWord = valueStoredInFirstWord << remainingBitsInThisWord;
        returnValue = valueStoredInFirstWord >> (remainingBitsInThisWord + startWordOffset);
    }
    else
    {
        uint32_t valueStoredInNextWord = pBitArray->pArrayBuffer[startWordIndex+1];

        valueStoredInFirstWord = valueStoredInFirstWord >> startWordOffset;
        valueStoredInNextWord = valueStoredInNextWord << (32-remainingBitsInNextWord);
        valueStoredInNextWord = valueStoredInNextWord >> (32-pBitArray->entrySizeInBits);

        returnValue = valueStoredInNextWord | valueStoredInFirstWord;
    }
    return returnValue;
}

static __inline void BitArray_Insert(BitArray_t const * const pBitArray, uint32_t index, uint32_t value)
{
    uint32_t bitNumber;
    uint32_t startWordIndex;
    uint32_t startWordOffset;
    uint32_t valueToBeStoredInFirstWord;
    int32_t remainingBitsInNextWord;

    bitNumber = index * pBitArray->entrySizeInBits;

    startWordIndex = bitNumber / (sizeof(uint32_t)*8);
    startWordOffset = bitNumber % (sizeof(uint32_t)*8);

    valueToBeStoredInFirstWord = pBitArray->pArrayBuffer[startWordIndex];

    remainingBitsInNextWord = (int32_t)(((int32_t)startWordOffset + (int32_t)pBitArray->entrySizeInBits) - ((int32_t)sizeof(uint32_t)*8));

    ASSERT(index < pBitArray->numberOfEntries);

    value = value & (pBitArray->mask);

    valueToBeStoredInFirstWord &= (~(pBitArray->mask << startWordOffset));
    valueToBeStoredInFirstWord |= (value << startWordOffset);
    pBitArray->pArrayBuffer[startWordIndex] = valueToBeStoredInFirstWord;

    if (remainingBitsInNextWord > 0)
    {
        uint32_t valueToBeStoredInSecondWord = pBitArray->pArrayBuffer[startWordIndex + 1];

        valueToBeStoredInSecondWord &= (~(pBitArray->mask >> ( pBitArray->entrySizeInBits - remainingBitsInNextWord)));
        valueToBeStoredInSecondWord |= ( value >> ( pBitArray->entrySizeInBits - remainingBitsInNextWord));
        pBitArray->pArrayBuffer[startWordIndex + 1] = valueToBeStoredInSecondWord;
    }
}

static __inline bool BitArray_IsBitSetInArray(uint32_t bitArray[], uint32_t bitIndex)
{
    const uint16_t cIndex = bitIndex / 32;
    const uint16_t cOffset = bitIndex % 32;
    const uint32_t cBitMask = BIT(cOffset);

    return ((bitArray[cIndex] & cBitMask) == cBitMask);
}

static __inline void BitArray_ResetBitInArray(uint32_t bitArray[], uint32_t bitIndex)
{
    const uint16_t cIndex = bitIndex / 32;
    const uint16_t cOffset = bitIndex % 32;
    const uint32_t cBitMask = BIT(cOffset);

    bitArray[cIndex] &= (~cBitMask);
}

static __inline void BitArray_SetBitInArray(uint32_t bitArray[], uint32_t bitIndex)
{
    const uint16_t cIndex = bitIndex / 32;
    const uint16_t cOffset = bitIndex % 32;
    const uint32_t cBitMask = BIT(cOffset);

    bitArray[cIndex] |= cBitMask;
}

