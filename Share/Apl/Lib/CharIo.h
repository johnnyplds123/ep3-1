#pragma once
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
//! @brief Header file for Character I/O
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
#define ASC_ESCAPE          '\e'
#define ASC_BACKSPACE       '\b'
#define ASC_LINEFEED        '\n'
#define ASC_RETURN          '\r'
#define ASC_NULL            '\0'

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define IS_DIGIT(X)         ('0'<=(X) && (X)<='9')   ///< is decimal digit
#define IS_HEX_DIGIT(X)     (AscToNum(X)<16)         ///< is hexa digit
#define IS_UPPER(X)         (('A'<=(X))&&((X)<='z')) ///< is uppercase character
#define IS_LOWER(X)         (('a'<=(X))&&((X)<='z')) ///< is lowercase character

#define PutChar(value)                  PutCharFile(stdOut, value)
#define PutNewLine()                    PutNewLineFile(stdOut)
#define PutStr(pStr)                    PutStrFile(stdOut, pStr)
#define PutStrn(pStr, width)            PutStrnFile(stdOut, pStr, width)
#define PutHex(value, width)            PutHexFile(stdOut, value, width)
#define PutSdec(value, width)           PutSdecFile(stdOut, value, width)
#define PutUdec(value, width)           PutUdecFile(stdOut, value, width)

#define PutData(ch,addr,cnt,size,fptr)                  PutDataFile(stdOut, ch, addr, cnt, size, fptr, 0,      16)
#define PutDataEx(ch,addr,cnt,size,fptr,offset,bytes)   PutDataFile(stdOut, ch, addr, cnt, size, fptr, offset, bytes)

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

/// @brief character stream output (generic)
typedef struct
{
    void (*fptr)(void* stream, uint8_t ch);
} StrOut_t;

/// @brief character stream output to UART
typedef struct tagSTR_OUT_UART StrOutUart_t;
struct tagSTR_OUT_UART
{
    void (*fptr)(StrOutUart_t* pUartStr, uint8_t ch);
    uint8_t portNum;
};

/// @brief character stream output to memory buffer
typedef struct tagSTR_OUT_MEM StrOutMem_t;
struct tagSTR_OUT_MEM
{
    void (*fptr)(StrOutMem_t* pMemStr, uint8_t ch); ///< output function
    uint8_t* pBuffer;                        ///< buffer pointer
    uint32_t size;                           ///< buffer byte size
    uint32_t length;                         ///< output length
};

/// @brief character stream output to STDIO using semi-hosting
typedef struct tagSTR_OUT_ARM StrOutArm_t;
struct tagSTR_OUT_ARM
{
    void (*fptr)(StrOutArm_t* pArmStr, uint8_t ch); ///< output function
};

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
uint8_t NumToAsc(uint8_t value);
uint8_t AscToNum(uint8_t asc);
uint8_t ToUpper(uint8_t asc);

Cstr_t SkipChar(Cstr_t pStr, uint8_t ch);
Cstr_t StrToNum(Cstr_t pStr, uint32_t* pNum);
Cstr_t StrIcmpMove(Cstr_t pStr, Cstr_t pPatternStr);

bool InKey(uint8_t* pInKey);
uint8_t   GetChar(void);

void PutCharUart(StrOutUart_t* pUartStr, uint8_t ch);
void BenUartOutChar(uint8_t value);
void PutCharArm(StrOutArm_t* pArmStr, uint8_t value);
void PutCharMem(StrOutMem_t* pMemStr, uint8_t ch);

void PutCharFile(void* stream,uint32_t value);
void PutNewLineFile(void* stream);
uint32_t  PutStrFile(void* stream, Cstr_t pStr);
uint32_t  PutStrnFile(void* stream, Cstr_t pStr, uint32_t width);

uint32_t  PutHexFile(void* stream, uint32_t value, uint32_t width);
uint32_t  PutSdecFile(void* stream, int32_t value, uint32_t width);
uint32_t  PutUdecFile(void* stream, uint32_t value, uint32_t width);

uint32_t  PutFormatFile(void* stream, Cstr_t fmtStr, ...);
uint32_t  PutFormatVarFile(void* stream, Cstr_t fmtStr, va_list varArg);
void PutDataFile(void* stream, uint8_t ch, uint32_t address, uint32_t dumpCount, uint32_t itemSize, uint32_t (*ReadData)(uint32_t), uint32_t prtAddrOffset, uint32_t bytesPerLine);

uint32_t  MemPrintf(void* pBuffer, uint32_t size, Cstr_t fmtStr, ...);

