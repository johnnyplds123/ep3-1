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
#define CPU0_UART_OUT_PORT  0       ///< UART output port number for CPU0
#define CPU1_UART_OUT_PORT  0       ///< UART output port number for CPU1

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

void*               stdOut;     ///< standard normal message output stream pointer
void*               stdDbg;     ///< standard debug message output stream pointer
void*               stdErr;     ///< standard error message output stream pointer
static StrOutUart_t uartOut;

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
    One-time initialization of Debug module.\n
      - set standard, debug and Error output stream to UART.
      - set all debug option variables.

    @param[in]  initMode        initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t Debug_OneTimeInit(InitBootMode_t initMode)
{
    #if (((_CPU_ == CPU_ID0) && CHK_CODE_TYPE(CODE_BOOT)) || CHK_FW_TYPE(FW_DNLD))
    MEM_CLR((void*)&smUartDynamic, sizeof(smUartDynamic));
    CLR_SHARE_DEBUG_FLAG(0xFFFFFFFF);
    #endif

    CLR_CPU_DEBUG_FLAG(0xFFFFFFFF);

    #if (_CPU_ == CPU_ID2)
    SET_CPU_2_DEBUG_FLAG(DBG_DUMP_GCBLK_INFO);
    #endif

    uartOut.fptr    = PutCharUart;
    uartOut.portNum = (HalCpu_GetId() == CPU_ID0) ? CPU0_UART_OUT_PORT : CPU1_UART_OUT_PORT;
    stdOut = &uartOut;

    stdDbg = stdOut;
    stdErr = stdOut;

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Dump memory data as 8bit hexa format.

    @param[in]  address         data address
    @param[in]  dumpByteSize    byte size to dump
**/
//-----------------------------------------------------------------------------
void Debug_DumpMemU8(uint32_t address, uint32_t dumpByteSize)
{
    PutData('M', address, dumpByteSize/sizeof(uint8_t), sizeof(uint8_t), MemReadU8);
}

//-----------------------------------------------------------------------------
/**
    Dump memory data as 16bit hexa format.

    @param[in]  address         data address
    @param[in]  dumpByteSize    byte size to dump
**/
//-----------------------------------------------------------------------------
void Debug_DumpMemU16(uint32_t address, uint32_t dumpByteSize)
{
    PutData('M', address, dumpByteSize/sizeof(uint16_t), sizeof(uint16_t), MemReadU16);
}

//-----------------------------------------------------------------------------
/**
    Dump memory data as 32bit hexa format.

    @param[in]  address         data address
    @param[in]  dumpByteSize    byte size to dump
**/
//-----------------------------------------------------------------------------
void Debug_DumpMemU32(uint32_t address, uint32_t dumpByteSize)
{
    PutData('M', address, dumpByteSize/sizeof(uint32_t), sizeof(uint32_t), MemReadU32);
}

//-----------------------------------------------------------------------------
/**
    Dump memory data as 32bit hexa format with bytes per line.

    @param[in]  address         data address
    @param[in]  dumpByteSize    byte size to dump
    @param[in]  bytesPerLine    byte size to dump per line
**/
//-----------------------------------------------------------------------------
void Debug_DumpMemU32PerLine(uint32_t address, uint32_t dumpByteSize, uint32_t bytesPerLine)
{
    PutDataEx('M', address, dumpByteSize/sizeof(uint32_t), sizeof(uint32_t), MemReadU32, 0, bytesPerLine);
}

//-----------------------------------------------------------------------------
/**
    Dump core information and halt a firmware execution.

    @param[in]  halt    halt if TRUE, else return to caller after dump information

**/
//-----------------------------------------------------------------------------
void Debug_DumpCore(bool halt)
{
    uint32_t tos;
    static const uint32_t dumpDwCount=64;

    tos = __current_sp();
    D_PRINTF("[CURRENT CPU=%1X, MODE=%2X, SP=%X]\n", HalCpu_GetId(), HalCpu_GetMode(), tos);
    PutData('X', tos - dumpDwCount*4, dumpDwCount*2, 4, MemReadU32);

#if 0
    tos = HalCpu_GetStackPointer(MODE_IRQ|I_BIT|F_BIT);
    D_PRINTF("[IRQ_SP=%X]\n", tos);
    PutData('X', tos - 32*4, 64, 4, MemReadU32);

    tos = HalCpu_GetStackPointer(MODE_FIQ|I_BIT|F_BIT);
    D_PRINTF("[FIQ_SP=%X]\n", HalCpu_GetId(), HalCpu_GetMode(), tos);
    PutData('X', tos - 32*4, 64, 4, MemReadU32);

    tos = HalCpu_GetStackPointer(MODE_SYS|I_BIT|F_BIT);
    D_PRINTF("[SYS_SP=%X]\n", HalCpu_GetId(), HalCpu_GetMode(), tos);
    PutData('X', tos - 32*4, 64, 4, MemReadU32);
#endif

    while(halt);
}

//-----------------------------------------------------------------------------
/**
    Print formatted output to standard output stream with resource sharing mechanism.\n

    @param[in] fmtStr     format control string
    @param[in] ...        variable arguments, depends on format
**/
//-----------------------------------------------------------------------------
void Debug_Printf(Cstr_t fmtStr, ...)
{
    va_list     varArg;
    uint32_t    spinLockId;

    DEBUG_UART_GET_STATUS();
    DEBUG_UART_SET_BUSY(TRUE);

    va_start(varArg, fmtStr);

    // use spin lock in SYS MODE only to prevent dead lock
    spinLockId = (HalCpu_GetMode()==MODE_SYS) ? cSpinLockPrintf : SPIN_LOCK_NULL;
    BEGIN_MULTI_CS1(spinLockId);
    PutFormatVarFile(stdOut, fmtStr, varArg);
    END_MULTI_CS1(spinLockId);

    DEBUG_UART_SET_BUSY(FALSE);
}

//-----------------------------------------------------------------------------
/**
    Print formatted output to debug stream with resource sharing mechanism.\n

    @param[in] error      error code
    @param[in] fmtStr     format control string
    @param[in] ...        variable arguments, depends on format
**/
//-----------------------------------------------------------------------------
void Debug_DbgPrintf(Cstr_t fmtStr, ...)
{
    va_list     varArg;
    uint32_t    spinLockId;

    DEBUG_UART_GET_STATUS();
    DEBUG_UART_SET_BUSY(TRUE);

    va_start(varArg, fmtStr);

    // use spin lock in SYS MODE only to prevent dead lock
    spinLockId = (HalCpu_GetMode()==MODE_SYS) ? cSpinLockPrintf : SPIN_LOCK_NULL;
    BEGIN_MULTI_CS1(spinLockId);
    PutFormatVarFile(stdDbg, fmtStr, varArg);
    END_MULTI_CS1(spinLockId);

    DEBUG_UART_SET_BUSY(FALSE);
}

void Debug_FakePrintf(Cstr_t fmtStr, ...){ }

//-----------------------------------------------------------------------------
/**
    Print formatted output to error stream with resource sharing mechanism.\n

    @param[in] error      error code
    @param[in] fmtStr     format control string
    @param[in] ...        variable arguments, depends on format
**/
//-----------------------------------------------------------------------------
void Debug_ErrPrintf(Error_t error, Cstr_t fmtStr, ...)
{
    va_list     varArg;
    uint32_t    spinLockId;

    // do not print message if no error
    if (error == cEcNoError) return;

    va_start(varArg, fmtStr);

    // use spin lock in SYS MODE only to prevent dead lock
    spinLockId = (HalCpu_GetMode()==MODE_SYS) ? cSpinLockPrintf : SPIN_LOCK_NULL;
    BEGIN_MULTI_CS1(spinLockId);
    PutFormatFile(stdErr, "\n[CPU%d] <ERROR:%X> ", _CPU_, error);
    PutFormatVarFile(stdErr, fmtStr, varArg);
    END_MULTI_CS1(spinLockId);
}

#if _CPU_==CPU_ID0    // CPU0 only
//-----------------------------------------------------------------------------
/**
    Debug dump firmware version and build information
**/
//-----------------------------------------------------------------------------
void Debug_DumpVersion(void)
{
    M_PRINTF("[VERS]");
    M_PRINTF(" SoC|%s(%4u)|R%2X %s\n",
                SOC_NICK_NAME, SOC_NAME, Hal_GetRevision(), _BLD_STR_);
    M_PRINTF("[VERS]");
    M_PRINTF(" FW|%s Time|%s Date|%s\n",
                _SRC_REV_, __TIME__, __DATE__);
}
#endif
