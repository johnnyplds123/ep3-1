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
//! @file
//! @brief Memory test code.
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "MemIo.h"
#include "HalCpu.h"
#include "SharedVars.h"
#include "MemTest.h"

#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define __P_RAND(X) ((X) + 1)

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief mjarch X test
typedef struct
{
    U32 *pMem;
    U32 testCount;
    U32 pattern0;
    U32 pattern1;
    U32 maxLog;
    U32 errorCount;
    MemTestResult_t* errLog;
} MemMarchTest_t;

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
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
    Print Pass or fail message

    @param[in]  errorCount  error count

    @return     error count
**/
//-----------------------------------------------------------------------------
static U32 PrintPassFail(U32 errorCount)
{
    if (errorCount == 0)
    {
        D_PRINTF(" - pass\n");
    }
    else
    {
        E_PRINTF(cEcMisCompare, " - test failed total %u error(s)\n", errorCount);
    }

    return (errorCount);
}

//-----------------------------------------------------------------------------
/**
    Print and log a mis-compare error

    @param[in]  pCurLog  error log buffer pointer
    @param[in]  pErrAddr error address pointer
    @param[in]  writeVal written value
    @param[in]  readVal  read value

    @return     error count
**/
//-----------------------------------------------------------------------------
static void PrintLogMiscompareError(MemTestResult_t* pCurLog, volatile U32 *pErrAddr, U32 writeVal, U32 readVal)
{
    //D_PRINTF("\nMiscompare%1X @%X, write=%X read=%X %X %X %X", HalCpu_GetId(), pErrAddr, writeVal, readVal, *pErrAddr, *pErrAddr, (writeVal ^ readVal));
    D_PRINTF("\n[MEMT] MisCom Cpu%d|@%X|W%X|R%X %X %X|B%X", HalCpu_GetId(), pErrAddr, writeVal, readVal, *pErrAddr, *pErrAddr, (writeVal ^ readVal));

    // add error information to error log
    if (pCurLog != NULL)
    {
        pCurLog->errorAddress = (U32)pErrAddr;
        pCurLog->writePattern = writeVal;
        pCurLog->readPattern  = readVal;
    }
}
//-----------------------------------------------------------------------------
/**
    Test memories with random pattern (32bit)

    @param[in]  address     start address of memory
    @param[in]  size        size of memory in byte count
    @param[in]  seed        random seed
    @param[out] errLog      error log result
    @param[in]  maxLog      maximum number of error log.

    @return     number of error
**/
//-----------------------------------------------------------------------------
U32 MemTestRandU32(U32* pMem, U32 testSize, U32 seed, MemTestResult_t errLog[], U32 maxLog)
{
    U32 count;
    U32 readVal;
    U32 writeVal;
    U32 errorCount = 0;
    volatile U32 *pCurAddr;

    D_PRINTF("[MEMT] Cpu%d|Rnd%X|[%X.%X)", HalCpu_GetId(), seed, pMem, (U32)pMem + testSize);

    count = testSize / sizeof(U32);
    pCurAddr = (volatile U32*)pMem;
    SRAND(seed);

    // Write address pattern forward.
    while (count-- > 0)
    {
        writeVal  = RANDOM(U32_MAX);
        *pCurAddr = writeVal;
        pCurAddr++;
    }

    count = testSize / sizeof(U32);
    pCurAddr = (volatile U32*)pMem;
    SRAND(seed);

    // Compare address pattern forward.
    while (count-- > 0)
    {
        writeVal = RANDOM(U32_MAX);
        readVal  = *pCurAddr;

        if (readVal != writeVal)
        {
            if (errorCount < maxLog)
            {
                PrintLogMiscompareError(errLog != NULL ? &errLog[errorCount] : NULL, pCurAddr, writeVal, readVal);
            }
            errorCount++;
        }
         pCurAddr++;
    }
    return PrintPassFail(errorCount);
}

#if CHK_CODE_TYPE(CODE_MAIN)
//-----------------------------------------------------------------------------
/**
    Test memories with random pattern (32bit)

    @param[in]  address     start address of memory
    @param[in]  size        size of memory in byte count
    @param[in]  seed        random seed

    @return     number of error
**/
//-----------------------------------------------------------------------------
U32 MemTestRandU32NoMsg(U32* pMem, U32 testSize, U32 seed)
{
    U32 count;
    U32 readVal;
    U32 writeVal;
    U32 errorCount = 0;
    volatile U32 *pCurAddr;

    count = testSize / sizeof(U32);
    pCurAddr = (volatile U32 *)pMem;
    SRAND(seed);

    // Write address pattern forward.
    while (count-- > 0)
    {
        writeVal  = RANDOM(U32_MAX);
        *pCurAddr = writeVal;
        pCurAddr++;
    }

    count = testSize / sizeof(U32);
    pCurAddr = (volatile U32 *)pMem;
    SRAND(seed);

    // Compare address pattern forward.
    while (count-- > 0)
    {
        writeVal = RANDOM(U32_MAX);
        readVal  = *pCurAddr;

        if (readVal != writeVal)
        {
            errorCount++;
        }
         pCurAddr++;
    }
    return errorCount;
}

//-----------------------------------------------------------------------------
/**
    Test memories with random pattern (Mask)

    @param[in]  address     start address of memory
    @param[in]  size        size of memory in byte count
    @param[in]  seed        random seed
    @param[in]  mask        bit mask

    @return     number of error
**/
//-----------------------------------------------------------------------------
U32 MemTestRandMaskNoMsg(U32* pMem, U32 testSize, U32 seed, U32 mask)
{
    U32 count;
    U32 readVal;
    U32 writeVal;
    U32 errorCount = 0;
    volatile U32 *pCurAddr;

    count = testSize / sizeof(U32);
    pCurAddr = (volatile U32 *)pMem;
    SRAND(seed);

    // Write address pattern forward.
    while (count-- > 0)
    {
        writeVal  = RANDOM(U32_MAX);
        *pCurAddr = writeVal;
        pCurAddr++;
    }

    count = testSize / sizeof(U32);
    pCurAddr = (volatile U32 *)pMem;
    SRAND(seed);

    // Compare address pattern forward.
    while (count-- > 0)
    {
        writeVal = RANDOM(U32_MAX);
        readVal  = *pCurAddr;
        if ((readVal & mask) != (writeVal & mask))
        {
            errorCount++;
        }

         pCurAddr++;
    }
    return errorCount;
}

//-----------------------------------------------------------------------------
/**
    Test memories with address pattern (32bit)

    @param[in]  address     start address of memory
    @param[in]  size        size of memory in byte count
    @param[in]  pattern     pattern to be XORed
    @param[out] errLog      error log result
    @param[in]  maxLog      maximum number of error log.

    @return     number of error
**/
//-----------------------------------------------------------------------------
U32 MemTestAddrU32(U32* pMem, U32 testSize, U32 pattern, MemTestResult_t errLog[], U32 maxLog)
{
    U32 count;
    U32 readVal;
    U32 writeVal;
    U32 errorCount=0;
    volatile U32 *pCurAddr;

    D_PRINTF("mem%1X inc:%X test[%X..%X)", HalCpu_GetId(), pattern, pMem, (U32)pMem + testSize);

    count = testSize / sizeof(U32);
    pCurAddr = (volatile U32*)pMem;

    // Write address pattern forward.
    while (count-- > 0)
    {
        *pCurAddr = (U32)pCurAddr ^ pattern;
        pCurAddr++;
    }

    count = testSize / sizeof(U32);
    pCurAddr = (volatile U32*)pMem;

    // Compare address pattern forward.
    while (count-- > 0)
    {
        writeVal =  (U32)pCurAddr ^ pattern;
        readVal  = *pCurAddr;

        if (readVal != writeVal)
        {
            if (errorCount < maxLog)
            {
                PrintLogMiscompareError(errLog != NULL ? &errLog[errorCount] : NULL, pCurAddr, writeVal, readVal);
            }
            errorCount++;
        }
        pCurAddr++;
    }

    return PrintPassFail(errorCount);
}

//-----------------------------------------------------------------------------
/**
    compare dword

    @param[in]  pMem        memory pointer
    @param[in]  writeData   written data be compared
    @param[out] pResult     march test result pointer
**/
//-----------------------------------------------------------------------------
static void MemCompareU32(volatile U32* pMem, U32 writeData, MemMarchTest_t* pResult)
{
    U32 readData;

    readData = *pMem;

    if (readData != writeData)
    {
        if (pResult->errorCount < pResult->maxLog)
        {
            PrintLogMiscompareError(pResult->errLog != NULL ? &pResult->errLog[pResult->errorCount] : NULL, pMem, writeData, readData);
        }
        pResult->errorCount++;
    }
}

//-----------------------------------------------------------------------------
/**
    Forward write (32bit)

    @param[in]  pMem        memory pointer
    @param[in]  count       test count
    @param[in]  pattern     pattern to be written
    @param[out] pResult     march test result pointer
**/
//-----------------------------------------------------------------------------
static void ForwardWriteU32(U32* pMem, U32 count, U32 pattern)
{
    while (count-- > 0)
    {
        *pMem = pattern;
        pMem++;
    }
}

//-----------------------------------------------------------------------------
/**
    Backward write (32bit)

    @param[in]  pMem        memory pointer
    @param[in]  count       test count
    @param[in]  pattern     pattern to be written
    @param[out] pResult     march test result pointer
**/
//-----------------------------------------------------------------------------
static void BackwardWriteU32(U32* pMem, U32 count, U32 pattern)
{
    pMem = pMem + count - 1;

    while (count-- > 0)
    {
        *pMem = pattern;
        pMem--;
    }
}

//-----------------------------------------------------------------------------
/**
    Forward compare (32bit)

    @param[in]  pMem        memory pointer
    @param[in]  count       test count
    @param[in]  pattern     pattern to be compared
    @param[out] pResult     march test result pointer
**/
//-----------------------------------------------------------------------------
static void ForwardCompareU32(U32* pMem, U32 count, U32 pattern, MemMarchTest_t* pResult)
{
    while (count-- > 0)
    {
        MemCompareU32(pMem, pattern, pResult);
        pMem++;
    }
}

//-----------------------------------------------------------------------------
/**
    Backward compare (32bit)

    @param[in]  pMem        memory pointer
    @param[in]  count       test count
    @param[in]  pattern     pattern to be compared
    @param[out] pResult     march test result pointer
**/
//-----------------------------------------------------------------------------
static void BackwardCompareU32(U32* pMem, U32 count, U32 pattern, MemMarchTest_t* pResult)
{
    pMem = pMem + count - 1;

    while (count-- > 0)
    {
        MemCompareU32(pMem, pattern, pResult);
        pMem--;
    }
}

//-----------------------------------------------------------------------------
/**
    Forward compare and write pattern (32bit)

    @param[in]  pMem        memory pointer
    @param[in]  count       test count
    @param[in]  pattern0    pattern to be compared
    @param[in]  pattern1    pattern to be written
    @param[out] pResult     march test result pointer
**/
//-----------------------------------------------------------------------------
static void ForwardCompareWriteU32(U32* pMem, U32 count, U32 pattern0, U32 pattern1, MemMarchTest_t* pResult)
{
    while (count-- > 0)
    {
        MemCompareU32(pMem, pattern0, pResult);
        *pMem = pattern1;
        pMem++;
    }
}

//-----------------------------------------------------------------------------
/**
    Backward compare and write pattern (32bit)

    @param[in]  pMem        memory pointer
    @param[in]  count       test count
    @param[in]  pattern0    pattern to be compared
    @param[in]  pattern1    pattern to be written
    @param[out] pResult     march test result pointer
**/
//-----------------------------------------------------------------------------
static void BackwardCompareWriteU32(U32* pMem, U32 count, U32 pattern0, U32 pattern1, MemMarchTest_t* pResult)
{
    pMem = pMem + count - 1;

    while (count-- > 0)
    {
        MemCompareU32(pMem, pattern0, pResult);
        *pMem = pattern1;
        pMem--;
    }
}

//-----------------------------------------------------------------------------
/**
    March X memory test. (32bit)

    @param[in]  pMem        memory pointer
    @param[in]  count       test count
    @param[in]  pattern0    pattern to be compared
    @param[in]  pattern1    pattern to be written
    @param[out] pResult     march test result pointer
**/
//-----------------------------------------------------------------------------
static void DoMarchTest(U32* pMem, U32 count, U32 pattern0, U32 pattern1, MemMarchTest_t* pResult)
{
    ForwardWriteU32(pMem, count, pattern0);
    ForwardCompareWriteU32(pMem, count, pattern0, pattern1, pResult);
    BackwardCompareWriteU32(pMem, count, pattern1, pattern0, pResult);
    BackwardCompareU32(pMem, count, pattern0, pResult);
}

//-----------------------------------------------------------------------------
/**
    March X memory test. (32bit)

    @param[in]  pMem        memory pointer
    @param[in]  testSize    test size of memory in byte count
    @param[in]  pattern     test pattern
    @param[out] errLog      error log array
    @param[in]  maxLog      maximum number of error log.

    @return     number of error
**/
//-----------------------------------------------------------------------------
U32 MemTestMarchU32(U32* pMem, U32 testSize, U32 pattern, MemTestResult_t errLog[], U32 maxLog)
{
    U32 count;
    MemMarchTest_t result;

    result.errLog = errLog;
    result.errorCount = 0;
    result.maxLog = maxLog;

    D_PRINTF("mem%1X mar:%X test[%X..%X)", HalCpu_GetId(), pattern, pMem, (U32)pMem+testSize);

    count = testSize / sizeof(U32);
    DoMarchTest(pMem, count, pattern, ~pattern, &result);

    return PrintPassFail(result.errorCount);
}

//-----------------------------------------------------------------------------
/**
    Write and Read compare memory test. (32bit)

    @param[in]  pMem        memory pointer
    @param[in]  testSize    test size of memory in byte count
    @param[in]  pattern     test pattern
    @param[out] errLog      error log array
    @param[in]  maxLog      maximum number of error log.

    @return     number of error
**/
//-----------------------------------------------------------------------------
U32 MemTestWrRdU32(U32* pMem, U32 testSize, U32 pattern, MemTestResult_t errLog[], U32 maxLog)
{
    U32 count;
    U32 readVal;
    U32 writeVal;
    U32 errorCount = 0;
    volatile U32 *pCurAddr;

    D_PRINTF("mem%1X wrc:%X test[%X..%X)", HalCpu_GetId(), pattern, pMem, (U32)pMem+testSize);

    count = testSize / sizeof(U32);
    pCurAddr = (volatile U32*)pMem;
    writeVal = pattern;

    // Compare address pattern forward.
    while (count-- > 0)
    {
        *pCurAddr = writeVal;
        readVal  = *pCurAddr;

        if (readVal != writeVal)
        {
            if (errorCount<maxLog)
            {
                PrintLogMiscompareError(errLog != NULL ? &errLog[errorCount] : NULL, pCurAddr, writeVal, readVal);
            }
            errorCount++;
        }
         pCurAddr++;
   }

    return PrintPassFail(errorCount);
}
#endif

/* End of File */

