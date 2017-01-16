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
//! @brief Exception handlers and debug functions\n
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"

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
//  VARIABLE DECLARATION WITH INITIAL VALUE DOESN'T WORK !!!
//  UNINTIALIZED VARIABLE DECLARATION ONLY !!!
//-----------------------------------------------------------------------------
#pragma arm section rwdata = "data_fast_ui"
#pragma arm section zidata = "data_fast_ui"

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    Software Interrupt exception hanlder.\n
      - Print exception message and looping forever.\n

    @param[in]  exceptionAddress    exception address
    @param[in]  stackPointer        current stack pointer
    @param[in]  swiNumber           SWI number

**/
//-----------------------------------------------------------------------------
void Debug_Swi(uint32_t exceptionAddress, uint32_t stackPointer, uint32_t swiNumber)
{
    E_PRINTF(cEcCpuSwi, "SWI @%X sp=%X swi#=%X\n",exceptionAddress,stackPointer,swiNumber);

    Debug_DumpCore(TRUE);
}

//-----------------------------------------------------------------------------
/**
    Prefetch abort exception hanlder.\n
      - Print exception message and looping forever.\n

    @param[in]  exceptionAddress    exception address
    @param[in]  stackPointer        current stack pointer
    @param[in]  reserved            reserved

**/
//-----------------------------------------------------------------------------
void Debug_PrefetchAbort(uint32_t exceptionAddress, uint32_t stackPointer, uint32_t reserved)
{
    E_PRINTF(cEcCpuPabort, "P-ABORT @%X sp=%X\n",exceptionAddress,stackPointer);

    Debug_DumpCore(TRUE);
}

//-----------------------------------------------------------------------------
/**
    Data abort exception hanlder.\n
      - Print exception message and looping forever.\n

    @param[in]  exceptionAddress    exception address
    @param[in]  stackPointer        current stack pointer
    @param[in]  faultStatus         data fault status

**/
//-----------------------------------------------------------------------------
void Debug_DataAbort(uint32_t exceptionAddress, uint32_t stackPointer, uint32_t faultStatus)
{
    E_PRINTF(cEcCpuDabort, "D-ABORT @%X sp=%X df=%X\n",exceptionAddress,stackPointer, faultStatus);

    Debug_DumpCore(TRUE);
}

//-----------------------------------------------------------------------------
/**
    Undefined Instruction exception hanlder.\n
      - Print exception message and looping forever.\n

    @param[in]  exceptionAddress    exception address
    @param[in]  stackPointer        current stack pointer
    @param[in]  reserved            reserved (shall be 0)

**/
//-----------------------------------------------------------------------------
void Debug_UndefInst(uint32_t exceptionAddress, uint32_t stackPointer, uint32_t reserved)
{
    E_PRINTF(cEcCpuUndefInst, "UNDEF INST @%X sp=%X\n",exceptionAddress,stackPointer);

    Debug_DumpCore(TRUE);
}

//-----------------------------------------------------------------------------
/**
    HALT program and dump information.

    @param[in]  funcName        Halted function name
    @param[in]  fileName        Halted file name
    @param[in]  auxInfo         auxiliary information
    @param[in]  callerAddress   caller's address

**/
//-----------------------------------------------------------------------------
void Debug_Halt(Cstr_t funcName, Cstr_t fileName, uint32_t auxInfo, uint32_t callerAddress)
{
    E_PRINTF(cEcHalt, "FW HALTED at %s()\n", funcName);
    M_PRINTF("PC=%X, LR=%X, FILE=\"%s\" AUX=%U\n\n", __return_address(), callerAddress, fileName, auxInfo);

    Debug_DumpCore(TRUE);
}

//-----------------------------------------------------------------------------
/**
    Assertion failed, dump information.

    @param[in]  assertTest      assertion test equation string
    @param[in]  fileName        Halted file name
    @param[in]  auxInfo         auxiliary information
    @param[in]  callerAddress   caller's address

**/
//-----------------------------------------------------------------------------
void Debug_AssertFail(Cstr_t assertTest, Cstr_t fileName, uint32_t auxInfo, uint32_t callerAddress )
{

    E_PRINTF(cEcAssertFail ,"ASSERTION FAILED at \"%s\"\n", assertTest);
    M_PRINTF("PC=%X, LR=%X, FILE=\"%s\" AUX=%U\n\n", __return_address(), callerAddress, fileName, auxInfo);

    Debug_DumpCore(TRUE);
}
