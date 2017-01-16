// ============================================================================
//
//     Copyright (c) Marvell Corporation 2000-2015  -  All rights reserved
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
//! @brief source file containing Bit Array functions
//!
//=============================================================================

/***********************************************************************************
*                                                                                  *
* Dependencies                                                                     *
*                                                                                  *
***********************************************************************************/

#include "BuildPkg.h"
#include "BitArray.h"

#include "Mathematics.h"

#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING`

/***********************************************************************************
*                                                                                  *
* Public Function Declarations                                                    *
*                                                                                  *
***********************************************************************************/
void BitArray_Initialize(uint32_t* const pArrayBuffer, BitArray_t* const pBitArray, uint32_t numberOfEntries, uint8_t entrySizeInBits)
{
    pBitArray->numberOfEntries = numberOfEntries;
    pBitArray->entrySizeInBits = entrySizeInBits;
    pBitArray->pArrayBuffer = pArrayBuffer;
    pBitArray->mask = BitMask(entrySizeInBits);

    M_PRINTF("BA_Init bfr = 0x%x\n", pBitArray->pArrayBuffer);
    ASSERT(pBitArray->numberOfEntries < (0xFFFFFFFFUL / entrySizeInBits));
}

uint32_t SearchNextBitInArray(const uint32_t buffer[], const uint32_t numberOfBitsInBuffer, const uint32_t bitIndexToStart, const BitToSearch_t bitToSearch)
{
    const uint32_t cNumberOfBytesInBufferEntry = sizeof(uint32_t);
    const uint32_t cNumberOfBitsInBufferEntry = cNumberOfBytesInBufferEntry * 8;
    const uint32_t cBitOffsetToStart = bitIndexToStart % cNumberOfBitsInBufferEntry;
    uint32_t presentWord;
    uint32_t bufferIndex = bitIndexToStart / cNumberOfBitsInBufferEntry;
    uint32_t nextBit = bufferIndex * cNumberOfBitsInBufferEntry;

    if (bitIndexToStart < numberOfBitsInBuffer)
    {
        if (bitToSearch == cfindFirstSetBit)
        {
            presentWord = buffer[bufferIndex] & ( 0xFFFFFFFFUL << cBitOffsetToStart);
        }
        else
        {
            presentWord = buffer[bufferIndex] | ( 0xFFFFFFFFUL >> (cNumberOfBitsInBufferEntry - cBitOffsetToStart));
        }

        //M_PRINTF( cLogShareCommon, cLogDebug, ( "PresentWord %x ( compensated for BitIndexToStart )\n", presentWord));

        while (nextBit < numberOfBitsInBuffer)  // (bufferIndex < BufferSize32BitsWords)
        {
            if (bitToSearch == cfindFirstSetBit)
            {
                if (presentWord != 0x0)
                {
                    nextBit = nextBit + CountTrailingZeros(presentWord);
                    break;
                }
            }
            else
            {
                if (presentWord != 0xFFFFFFFF)
                {
                    nextBit = nextBit + CountTrailingOnes(presentWord);
                    break;
                }
            }

            nextBit += cNumberOfBitsInBufferEntry;
            bufferIndex++;
            presentWord = buffer[bufferIndex];
        }

        nextBit = MIN(nextBit, numberOfBitsInBuffer);
    }
    else
    {
        nextBit = numberOfBitsInBuffer;

    }
    M_PRINTF( "SearchNextBitInArray %x %x start %d bit to search = %d -> Found bit %d\n", buffer[1], buffer[0], bitIndexToStart, bitToSearch, nextBit);


    return nextBit;
}
