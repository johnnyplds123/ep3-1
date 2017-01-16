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
//! @brief Character input, output and manipulation.
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include <stdio.h>
#include "CharIo.h"
#include "HalUart.h"

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
static uint32_t PutVarFile(void* stream, uint8_t fmtChar, uint32_t value, uint32_t width);

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
    Convert number to hexa decimal ascii code.\n
    This code doesn't check the range of number.

    @param[in]  value    value to convert

    @return    ascii code
**/
//-----------------------------------------------------------------------------
uint8_t NumToAsc(uint8_t value)
{
    value &= 0xF;

    return (value>=10) ? (value-10+'A') : (value+'0');
}

//-----------------------------------------------------------------------------
/**
    Convert hexa decimal ascii code to number

    @param[in]  asc      ascii code value

    @return    16 if not hexa-decimal ascii, else converted number
**/
//-----------------------------------------------------------------------------
uint8_t AscToNum(uint8_t asc)
{
    if ('0'<=asc && asc<='9') return asc-'0';

    if ('A'<=asc && asc<='F') return asc-'A'+10;

    if ('a'<=asc && asc<='f') return asc-'a'+10;

    return (16);    // error
}

//-----------------------------------------------------------------------------
/**
    Convert lowercase character to uppercase.

    @param[in]  asc    ascii code to convert

    @return    uppercase character code
**/
//-----------------------------------------------------------------------------
uint8_t ToUpper(uint8_t asc)
{
    return IS_LOWER(asc) ? asc+'A'-'a' : asc;
}

//-----------------------------------------------------------------------------
/**
    Skip character in string

    @param[in]  pStr     pointer to string
    @param[in]  ch       character to skip

    @return    pointer to the first occurrence of not matched character in the string
**/
//-----------------------------------------------------------------------------
Cstr_t SkipChar(Cstr_t pStr, uint8_t ch)
{
    while(*pStr == ch) pStr++;

    return(pStr);
}

//-----------------------------------------------------------------------------
/**
    Convert hexa decimal string to number.\n
    This code doesn't check the error, stop convertion immediatly at the position of error\n.

    @param[in]  pStr     pointer to hexa decimal string
    @param[out] pNum     converted hexa decimal number

    @return    pointer to the first occurrence of non hexa decimal character in the string
**/
//-----------------------------------------------------------------------------
Cstr_t StrToNum(Cstr_t pStr, uint32_t* pNum)
{
    uint32_t num;
    uint32_t value = 0;

    while((num = AscToNum(*pStr)) < 16)
    {
        value = (value << 4) + num;
        pStr++;
    }

    *pNum = value;

    return pStr;
}

//-----------------------------------------------------------------------------
/**
    Compare string without case sensitivity and move string pointer if matched.

    @param[in]  pStr1   string to be scanned
    @param[in]  pStr2   string to compare

    @return     NULL if not matched
**/
//-----------------------------------------------------------------------------
Cstr_t StrIcmpMove(Cstr_t pStr1, Cstr_t pStr2)
{
    while (*pStr2!=ASC_NULL)
    {
        if (ToUpper(*pStr1++) != ToUpper(*pStr2++)) return(NULL);
    }

    return(pStr1);
}

//-----------------------------------------------------------------------------
/**
    return ascii key code from user input.

    @param[out] pInKey input character key code from user

    @return FALSE if a key has not been pressed, else pressed key code
**/
//-----------------------------------------------------------------------------
bool InKey(uint8_t* pInKey)
{
    if (!HalUart_TestRxReady(0)) return FALSE;

    *pInKey =  HalUart_Receive(0);

    return TRUE;
}

//-----------------------------------------------------------------------------
/**
    get ascii key code from user input, wait until key is pressed.

    @param  void

    @return pressed key code
**/
//-----------------------------------------------------------------------------
uint8_t GetChar(void)
{
    uint8_t code;

    while (!InKey(&code));    // wait for key input

    return code;
}

//-----------------------------------------------------------------------------
/**
    Print a character to UART.

    @param[in] stream     output stream pointer (UART)
    @param[in] value      character data to print out
**/
//-----------------------------------------------------------------------------
void PutCharUart(StrOutUart_t* pUartStr, uint8_t value)
{
    uint8_t portNum;

    portNum = pUartStr->portNum;

    while (!HalUart_TestTxReady(portNum));    // wait for Tx Ready

    HalUart_Transmit(portNum, value);
}
void BenUartOutChar(uint8_t value)
{
    while (!HalUart_TestTxReady(0));    // wait for Tx Ready
    HalUart_Transmit(0, value);
}

//-----------------------------------------------------------------------------
/**
    Print a character to memory buffer.

    @param[out] pMemStr   pointer to memory stream output
    @param[in]  value     character data to print out
**/
//-----------------------------------------------------------------------------
void PutCharMem(StrOutMem_t* pMemStr, uint8_t ch)
{
    if (pMemStr->length<pMemStr->size)
    {
        *(pMemStr->pBuffer+pMemStr->length) = ch;
    }
    pMemStr->length++;
}

//-----------------------------------------------------------------------------
/**
    Print a character to stream.

    @param[out] stream    pointer to stream output
    @param[in]  value     character data to print out
**/
//-----------------------------------------------------------------------------
void PutCharFile(void* stream, uint32_t value)
{
    if (stream==NULL) return;

    ((StrOut_t *)stream)->fptr(stream, value);
}

//-----------------------------------------------------------------------------
/**
    Print a string to stream.

    @param[out] stream    pointer to stream output
    @param[in]  pStr      pointer to string to print out

    @return number of printed out characters
**/
//-----------------------------------------------------------------------------
uint32_t PutStrFile(void* stream, Cstr_t pStr)
{
    uint32_t length=0;

    while (*pStr != '\0')
    {
        PutCharFile(stream, *pStr++);
        length++;
    }

    return(length);
}

//-----------------------------------------------------------------------------
/**
    Print a string to stream.\n

    @param[out] stream    pointer to stream output
    @param[in]  pStr      pointer to string to print out
    @param[in]  width     number of characters to be printed

    @return number of printed out characters
**/
//-----------------------------------------------------------------------------
uint32_t PutStrnFile(void* stream, Cstr_t pStr, uint32_t width)
{
    uint32_t length;

    length = PutStrFile(stream, pStr);

    for (; length<width; length++)
    {
        PutCharFile(stream, ' ');
    }

    return (length);
}

//-----------------------------------------------------------------------------
/**
    Print new line characters "\r\n" to stream.

    @param  void

    @see PutCharFile, PutHex, PutStr
    @callgraph
**/
//-----------------------------------------------------------------------------
void PutNewLineFile(void* stream)
{
    PutStrFile(stream, "\r\n");
}

//-----------------------------------------------------------------------------
/**
    Print data as hexa digit formatted to stream.\n

    @param[out] stream    pointer to stream output
    @param[in]  value     data value
    @param[in]  width     number of characters to be printed

    @return number of printed out characters
**/
//-----------------------------------------------------------------------------
uint32_t PutHexFile(void* stream, uint32_t value, uint32_t width)
{
    uint32_t bitIndex;
    uint8_t  hexChar;

    if (width == 0) width = 8;

    bitIndex = 4 * width; // convert to bit index from hexa width

    do
    {
        hexChar = NumToAsc(value >> (bitIndex - 4));
        PutCharFile(stream, hexChar);
        bitIndex -= 4;
    } while(bitIndex != 0);

    return(width);
}

//-----------------------------------------------------------------------------
/**
    Print data as decimal digit formatted to stream.

    @param[out] stream    pointer to stream output
    @param[in]  value     data value
    @param[in]  width     number of characters to be printed
    @param[in]  signChar  sign character

    @return number of printed out characters
**/
//-----------------------------------------------------------------------------
uint32_t PutDecFile(void* stream, uint32_t value, uint32_t width, uint8_t signChar)
{
    uint32_t digitValue;
    uint32_t quotient;
    uint32_t actualWidth;

    digitValue  = 1000000000;
    actualWidth = 10;

    // compute actual number of digit
    while (value < digitValue && digitValue > 1)
    {
        digitValue /= 10;
        actualWidth--;
    }

    // print space
    if (width > actualWidth) PutStrnFile(stream, "", width - actualWidth);

    // print sign character if necessary
    if (signChar) PutCharFile(stream, signChar);

    while (digitValue > 0)
    {
        quotient = value / digitValue;
        value    = value % digitValue;

        digitValue /= 10;

        PutCharFile(stream, '0' + quotient);
    }

    return width;
}


//-----------------------------------------------------------------------------
/**
    Print data as signed decimal digit formatted to stream.

    @param[out] stream    pointer to stream output
    @param[in]  value     data value
    @param[in]  width     number of characters to be printed

    @return number of printed out characters
**/
//-----------------------------------------------------------------------------
uint32_t PutSdecFile(void* stream, int32_t value, uint32_t width)
{
    uint32_t absValue;
    uint8_t signChar;

    if (value < 0)
    {
        signChar = '-';
        absValue = -value;
    }
    else
    {
        signChar = 0;
        absValue = +value;
    }

    return PutDecFile(stream, absValue, width, signChar);
}

//-----------------------------------------------------------------------------
/**
    Print variable data type to stream.

    @param[out] stream     pointer to stream output
    @param[in]  formatChar printed format character
    @param[in]  value      data value (real type depends on format character)
    @param[in]  width      width to print out

    @return number of characters to be printed
**/
//-----------------------------------------------------------------------------
static uint32_t PutVarFile(void* stream, uint8_t formatChar, uint32_t value, uint32_t width)
{
    switch(ToUpper(formatChar))
    {
    case 'S':
        return PutStrnFile(stream, (Cstr_t)value, width);
    case 'D':
        return PutSdecFile(stream, value, width);
    case 'U':
        return PutDecFile(stream, value, width, 0);
    case 'X':
        return PutHexFile(stream, value, width);
    case 'C':
        break;
    default:
        value = formatChar;
        break;
    }

    PutCharFile(stream, value);

    return(1);
}

//-----------------------------------------------------------------------------
/**
    Print formatted output to stream.\n
    very limited version of real printf in C-library.\n
    support \%s, \%d, \%u and \%x format only.\n
    may not properly dispay with width identifier such as \%3, \%09 and so on.

    @param[out] stream     pointer to stream output
    @param[in]  fmtStr     format control string
    @param[in]  ...        variable arguments, depends on format

    @return number of characters to be printed
**/
//-----------------------------------------------------------------------------
uint32_t PutFormatVarFile(void* stream, Cstr_t fmtStr, va_list varArg)
{
    uint32_t width;
    uint32_t value;
    uint32_t totalCharOut = 0;
    uint8_t  formatChar;

    while ((formatChar = *fmtStr++) != ASC_NULL)
    {
        switch(formatChar)
        {
        case '%':
            formatChar = *fmtStr++;
            width = 0;
            while (IS_DIGIT(formatChar))
            {
                width = width*10 + AscToNum(formatChar);
                formatChar = *fmtStr++;
            }

            value = va_arg(varArg, uint32_t);
            totalCharOut += PutVarFile(stream, formatChar, value, width);
            break;
        case ASC_LINEFEED:
            PutNewLineFile(stream);
            totalCharOut++;
            break;
        default:
            PutCharFile(stream, formatChar);
            totalCharOut++;
            break;
        }
    }

    return(totalCharOut);
}

//-----------------------------------------------------------------------------
/**
    Print formatted output to stream.\n
    very limited implementation of printf in C-library.\n
    support \%s, \%d, \%u and \%x format only.\n
    may not properly dispay with width identifier such as \%3, \%09 and so on.

    @param[out] stream     pointer to stream output
    @param[in]  fmtStr     format control string
    @param[in]  ...        variable arguments, depends on format

    @return number of characters to be printed
**/
//-----------------------------------------------------------------------------
uint32_t PutFormatFile(void* stream, Cstr_t fmtStr, ...)
{
    va_list varArg;

    va_start(varArg, fmtStr);

    return PutFormatVarFile(stream, fmtStr, varArg);
}

//-----------------------------------------------------------------------------
/**
    Put series of data to seial stream.\n

    @param[out] stream          pointer to stream output
    @param[in]  ch              address character
    @param[in]  address         data address
    @param[in]  count           number of data to print
    @param[in]  dataSize        byte size of data
    @param[in]  ReadData        Call back function pointer to read data
    @param[in]  offset          address offset
    @param[in]  bytesPerLine    bytes per line
**/
//-----------------------------------------------------------------------------
void PutDataFile(void* stream, uint8_t ch, uint32_t address, uint32_t count, uint32_t dataSize, uint32_t (*ReadData)(uint32_t), uint32_t offset, uint32_t bytesPerLine)
{
    uint32_t i;
    uint32_t itemPerLine;
    uint32_t value;
    char fmtStr[] = "%#X "; // format string to display Hex value, # is replaced by code

    itemPerLine = bytesPerLine / dataSize;  // bytes per line

    while (count > 0)
    {
        PutFormatFile(stream, "%c:%x ", ch, address);

        for (i = itemPerLine; count > 0 && i > 0; count--, i--)
        {
            value = ReadData(address+offset);

            fmtStr[1] = '0' + dataSize*2; // set width data in fomrat string
            PutFormatFile(stream, fmtStr,value);

            address += dataSize;
        }

        PutNewLineFile(stream);
    }
}

//-----------------------------------------------------------------------------
/**
    Print formatted output to buffer.\n
    very limited implementation of printf in C-library.\n
    support \%s, \%d, \%u and \%x format only.\n
    may not properly dispay with width identifier such as \%3, \%09 and so on.

    @param[in] pBuffer    pointer to buffer memory
    @param[in] size       buffer byte size
    @param[in] fmtStr     format control string
    @param[in] ...        variable arguments, depends on format

    @return number of characters to be printed
**/
//-----------------------------------------------------------------------------
uint32_t MemPrintf(void* pBuffer, uint32_t size, Cstr_t fmtStr, ...)
{
    StrOutMem_t memStr;
    va_list varArg;

    va_start(varArg, fmtStr);

    memStr.fptr = PutCharMem;
    memStr.pBuffer = pBuffer;
    memStr.size = size;
    memStr.length = 0;

    PutFormatVarFile(&memStr, fmtStr, varArg);

    return(memStr.length);
}

