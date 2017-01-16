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
//! @brief Memory Input/Output codes through CPU not SoC's HW.
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "MemIo.h"

#include "PerfSection.h"   // CODE AND DATA MEMORY MAPPING

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
    Read a byte(uint8_t) data from memory.\n
    Guarantee for unaligned address access.

    @param[in]  address     CPU's address

    @return     data value
**/
//-----------------------------------------------------------------------------
uint32_t MemReadU8(uint32_t address)
{
    return MEM_U8(address);
}

//-----------------------------------------------------------------------------
/**
    Read a word(uint16_t) data from memory.\n
    Guarantee for unaligned address access.

    @param[in]  address     CPU's address

    @return     data value
**/
//-----------------------------------------------------------------------------
uint32_t MemReadU16(uint32_t address)
{
    // aligned, use native read
    if (TEST_U16_ALIGNED(address)) return MEM_U16(address);

    return PACKED_U16(address);
}

//-----------------------------------------------------------------------------
/**
    Read a double word(uint32_t) data from memory.\n
    Guarantee for unaligned address access.

    @param[in]  address     CPU's address

    @return     data value
**/
//-----------------------------------------------------------------------------
uint32_t MemReadU32(uint32_t address)
{
    // aligned, use native read
    if (TEST_U32_ALIGNED(address)) return MEM_U32(address);

    return PACKED_U32(address);
}

//-----------------------------------------------------------------------------
/**
    Write a byte(uint8_t) data to memory.\n
    Guarantee for unaligned address access.

    @param[in]  address     CPU's address
    @param[in]  data        uint8_t data value to be written

    @return     data value
**/
//-----------------------------------------------------------------------------
void MemWriteU8(uint32_t address, uint32_t data)
{
     MEM_U8(address) = data;
}

//-----------------------------------------------------------------------------
/**
    Write a word(uint16_t) data to memory.\n
    Guarantee for unaligned address access.

    @param[in]  address     CPU's address
    @param[in]  data        uint16_t data value to be written

    @return     data value
**/
//-----------------------------------------------------------------------------
void MemWriteU16(uint32_t address, uint32_t data)
{
    // aligned, use native write
    if (TEST_U16_ALIGNED(address))
    {
        MEM_U16(address) = data;
    }
    else
    {
        PACKED_U16(address) = data;
    }
}

//-----------------------------------------------------------------------------
/**
    Write a double word(uint32_t) data to memory.\n
    Guarantee for unaligned address access.

    @param[in]  address     CPU's address
    @param[in]  data        uint32_t data value to be written

    @return     data value
**/
//-----------------------------------------------------------------------------
void MemWriteU32(uint32_t address, uint32_t data)
{
    // aligned, use native write
    if (TEST_U32_ALIGNED(address))
    {
        MEM_U32(address) = data;
    }
    else
    {
        PACKED_U32(address) = data;
    }
}

//-----------------------------------------------------------------------------
/**
    Fill a memory region with 32bit data pattern.\n
    This routine is used to fill a memory before C-Library initialization.\n
    Do not use any stack !!!

    @param[in]  pDest    destination memory pointer
    @param[in]  data     32bit data value to be fill
    @param[in]  byteSize byte size to fill (size shall be multiple of 4)

    @return     increased memory pointer

    @see MemRead, MEM_U8, REG_U8, MEM_U16, REG_U16, MEM_U32, REG_U32
    @callgraph
**/
//-----------------------------------------------------------------------------
__asm void* MemFillU32(void* pDest, uint32_t data, uint32_t byteSize)
{
    PRESERVE8

    LSRS     r2,r2,#2   // count /=4, convert to DWord unit
    B        %F0
1
    STM      r0!,{r1}
0
    SUBS     r2,r2,#1
    BCS      %B1

    BX       lr
}

//-----------------------------------------------------------------------------
/**
    Fill a memory region with incremental 32bit data pattern.\n

    This routine is used to fill a memory before C-Library initialization.\n
    Do not use any stack !!!

    @param[in]  pDest    destination memory pointer
    @param[in]  data     32bit data value to be fill
    @param[in]  byteSize byte size to fill (size shall be multiple of 4)
    @param[in]  delta    incremental vlaue

    @return     increased memory pointer

    @see MemRead, MEM_U8, REG_U8, MEM_U16, REG_U16, MEM_U32, REG_U32
    @callgraph
**/
//-----------------------------------------------------------------------------
__asm void* MemFillU32Inc(void* pDest, uint32_t data, uint32_t byteSize, uint32_t delta)
{
    PRESERVE8

    LSRS     r2,r2,#2   // count /=4, convert to DWord unit
    B        %F0
1
    STM      r0!,{r1}
    ADDS     r1,r1,r3   //
0
    SUBS     r2,r2,#1
    BCS      %B1

    BX       lr
}

//-----------------------------------------------------------------------------
/**
    sum of byte memories

    @param[in]  pData   memory pointer
    @param[in]  size    byte size

    @return     sum of byte memories
**/
//-----------------------------------------------------------------------------
uint32_t MemSumU8(const uint8_t* pData, uint32_t size)
{
    uint32_t sum = 0;

    while (size-- > 0)
    {
        sum += *pData++;
    }

    return sum;
}

//-----------------------------------------------------------------------------
/**
    Swap endian format (little to big or big to little endian)

    @param[in]  pDest       destination buffer pointer
    @param[in]  pSrc        source buffer pointer
    @param[in]  wordSize    word size of buffer

**/
//-----------------------------------------------------------------------------
void MemSwapU16(uint16_t* pDestData, const uint16_t* pSrcData, uint32_t wordSize)
{
    uint16_t value;

    while (wordSize-- > 0)
    {
        value = *pSrcData++;
        *pDestData++ = ((value&0xFF)<<8) | ((value>>8)&0xFF);
    }
}

//-----------------------------------------------------------------------------
/**
    Swaps and copies bytes between memories\n

    @param[in]  pDest   destination buffer pointer
    @param[in]  pSrc    source buffer pointer
    @param[in]  size    byte size of buffer

**/
//-----------------------------------------------------------------------------
void MemSwapCopy(void* pDest, void* pSrc, uint32_t byteSize)
{
    MEM_COPY(pDest, pSrc, byteSize);
    MemSwapU16((uint16_t *)pDest, (uint16_t *)pSrc, byteSize/2);
}

//-----------------------------------------------------------------------------
/**
    copies dwords between memories\n

    @param[out] pDest   destination buffer pointer
    @param[in]  pSrc    source buffer pointer
    @param[in]  size    byte size of buffer (shall be multiple of 4)

**/
//-----------------------------------------------------------------------------
void MemCopyU32(uint32_t* pDest, uint32_t* pSrc, uint32_t byteSize)
{
    byteSize /= 4; // Convert to DWord Size

    while (byteSize-- > 0)
    {
        *pDest++ = *pSrc++;
    }
}

//-----------------------------------------------------------------------------
/**
    Fast copy alinged 3 uint32_t data.

    Both source and target memory should support burst mode unless will fail.

    @param[out] pDest   destination buffer pointer
    @param[in]  pSrc    source buffer pointer
**/
//-----------------------------------------------------------------------------
__asm void* MemCopy2U32(uint32_t* pDest, uint32_t* pSrc)
{
    PRESERVE8

    PUSH    {r4}

    LDMIA   r1!, {r2-r3}
    STMIA   r0!, {r2-r3}

    POP     {r4}

    BX      LR
}

//-----------------------------------------------------------------------------
/**
    Fast copy alinged 3 uint32_t data.

    Both source and target memory should support burst mode unless will fail.

    @param[out] pDest   destination buffer pointer
    @param[in]  pSrc    source buffer pointer
**/
//-----------------------------------------------------------------------------
__asm void* MemCopy3U32(uint32_t* pDest, uint32_t* pSrc)
{
    PRESERVE8

    PUSH    {r4}

    LDMIA   r1!, {r2-r4}
    STMIA   r0!, {r2-r4}

    POP     {r4}

    BX      LR
}

//-----------------------------------------------------------------------------
/**
    Fast copy alinged 4 uint32_t data.

    Both source and target memory should support burst mode unless will fail.

    @param[out] pDest   destination buffer pointer
    @param[in]  pSrc    source buffer pointer
**/
//-----------------------------------------------------------------------------
__asm void* MemCopy4U32(uint32_t* pDest, uint32_t* pSrc)
{
    PRESERVE8

    PUSH    {r4-r5}

    LDMIA   r1!, {r2-r5}
    STMIA   r0!, {r2-r5}

    POP     {r4-r5}

    BX      LR
}

//-----------------------------------------------------------------------------
/**
    Fast copy alinged 5 uint32_t data.

    Both source and target memory should support burst mode unless will fail.

    @param[out] pDest   destination buffer pointer
    @param[in]  pSrc    source buffer pointer
**/
//-----------------------------------------------------------------------------
__asm void* MemCopy5U32(uint32_t* pDest, uint32_t* pSrc)
{
    PRESERVE8

    PUSH    {r4-r6}

    LDMIA   r1!, {r2-r6}
    STMIA   r0!, {r2-r6}

    POP     {r4-r6}

    BX      LR
}

//-----------------------------------------------------------------------------
/**
    Fast copy alinged 6 uint32_t data.

    Both source and target memory should support burst mode unless will fail.

    @param[out] pDest   destination buffer pointer
    @param[in]  pSrc    source buffer pointer
**/
//-----------------------------------------------------------------------------
__asm void* MemCopy6U32(uint32_t* pDest, uint32_t* pSrc)
{
    PRESERVE8

    PUSH    {r4-r7}

    LDMIA   r1!, {r2-r7}
    STMIA   r0!, {r2-r7}

    POP     {r4-r7}

    BX      LR
}

//-----------------------------------------------------------------------------
/**
    Fast copy alinged 7 uint32_t data.

    Both source and target memory should support burst mode unless will fail.

    @param[out] pDest   destination buffer pointer
    @param[in]  pSrc    source buffer pointer
**/
//-----------------------------------------------------------------------------
__asm void* MemCopy7U32(uint32_t* pDest, uint32_t* pSrc)
{
    PRESERVE8

    PUSH    {r4-r8}

    LDMIA   r1!, {r2-r8}
    STMIA   r0!, {r2-r8}

    POP     {r4-r8}

    BX      LR
}

//-----------------------------------------------------------------------------
/**
    Fast copy alinged 8 uint32_t data

    Both source and target memory should support burst mode unless will fail.

    @param[out] pDest   destination buffer pointer
    @param[in]  pSrc    source buffer pointer
**/
//-----------------------------------------------------------------------------
__asm void* MemCopy8U32(uint32_t* pDest, uint32_t* pSrc)
{
    PRESERVE8

    PUSH    {r4-r9}

    LDMIA   r1!, {r2-r9}
    STMIA   r0!, {r2-r9}

    POP     {r4-r9}

    BX      LR
}

//-----------------------------------------------------------------------------
/**
    Fast copy alinged 9 uint32_t data.

    Both source and target memory should support burst mode unless will fail.

    @param[out] pDest   destination buffer pointer
    @param[in]  pSrc    source buffer pointer
**/
//-----------------------------------------------------------------------------
__asm void* MemCopy9U32(uint32_t* pDest, uint32_t* pSrc)
{
    PRESERVE8

    PUSH    {r4-r10}

    LDMIA   r1!, {r2-r10}
    STMIA   r0!, {r2-r10}

    POP     {r4-r10}

    BX      LR
}

//-----------------------------------------------------------------------------
/**
    Fast copy alinged 10 uint32_t data.

    Both source and target memory should support burst mode unless will fail.

    @param[out] pDest   destination buffer pointer
    @param[in]  pSrc    source buffer pointer
**/
//-----------------------------------------------------------------------------
__asm void* MemCopy10U32(uint32_t* pDest, uint32_t* pSrc)
{
    PRESERVE8

    PUSH    {r4-r11}

    LDMIA   r1!, {r2-r11}
    STMIA   r0!, {r2-r11}

    POP     {r4-r11}

    BX      LR
}

//-----------------------------------------------------------------------------
/**
    Fast copy alinged 12 uint32_t data.

    Both source and target memory should support burst mode unless will fail.

    @param[out] pDest   destination buffer pointer
    @param[in]  pSrc    source buffer pointer
**/
//-----------------------------------------------------------------------------
__asm void* MemCopy12U32(uint32_t* pDest, uint32_t* pSrc)
{
    PRESERVE8

    PUSH    {r4-r7}

    LDMIA   r1!, {r2-r7}
    STMIA   r0!, {r2-r7}
    LDMIA   r1!, {r2-r7}
    STMIA   r0!, {r2-r7}

    POP     {r4-r7}

    BX      LR
}

//-----------------------------------------------------------------------------
/**
    Fast copy alinged 16 uint32_t data

    Both source and target memory should support burst mode unless will fail.

    @param[out] pDest   destination buffer pointer
    @param[in]  pSrc    source buffer pointer
**/
//-----------------------------------------------------------------------------
__asm void* MemCopy16U32(uint32_t* pDest, uint32_t* pSrc)
{
    PRESERVE8

    PUSH    {r4-r9}

    LDMIA   r1!, {r2-r9}
    STMIA   r0!, {r2-r9}
    LDMIA   r1!, {r2-r9}
    STMIA   r0!, {r2-r9}

    POP     {r4-r9}

    BX      LR
}

//-----------------------------------------------------------------------------
/**
    Fast copy alinged 32 uint32_t data

    Both source and target memory should support burst mode unless will fail.

    @param[out] pDest   destination buffer pointer
    @param[in]  pSrc    source buffer pointer
**/
//-----------------------------------------------------------------------------
__asm void* MemCopy32U32(uint32_t* pDest, uint32_t* pSrc)
{
    PRESERVE8

    PUSH    {r4-r9}

    LDMIA   r1!, {r2-r9}
    STMIA   r0!, {r2-r9}
    LDMIA   r1!, {r2-r9}
    STMIA   r0!, {r2-r9}
    LDMIA   r1!, {r2-r9}
    STMIA   r0!, {r2-r9}
    LDMIA   r1!, {r2-r9}
    STMIA   r0!, {r2-r9}

    POP     {r4-r9}

    BX      LR
}

#if CHK_CODE_TYPE(CODE_MAIN)
//-----------------------------------------------------------------------------
/**
    Fast copy alinged N 8U32 data

    Both source and target memory should support burst mode unless will fail.

    @param[out] pDest   destination buffer pointer
    @param[in]  pSrc    source buffer pointer
    @param[in]  count   number of bytes to be copied
**/
//-----------------------------------------------------------------------------
__asm PVOID MemCopyN8U32(U32* pDest, U32* pSrc, U32 bytes)
{
    PRESERVE8

    PUSH    {r4-r10}

LDM8U32loop

    LDMIA   r1!, {r3 - r10}
    STMIA   r0!, {r3 - r10}

    SUBS    r2, r2, #32
    BGT     LDM8U32loop

    POP     {r4-r10}

    BX      LR
}

//-----------------------------------------------------------------------------
/**
    Fast copy alinged N U32 data

    Both source and target memory should support burst mode unless will fail.

    @param[out] pDest   destination buffer pointer
    @param[in]  pSrc    source buffer pointer
    @param[in]  count   number of bytes to be copied
**/
//-----------------------------------------------------------------------------
__asm PVOID MemCopyNU32(U32* pDest, U32* pSrc, U32 bytes)
{
    PRESERVE8

    PUSH    {r4-r10}

    CMP     r2, #32
    BLT     LDM4Bloop

LDM32Bloop

    LDMIA   r1!, {r3 - r10}
    STMIA   r0!, {r3 - r10}

    SUB     r2, r2, #32
    CMP     r2, #32
    BGE     LDM32Bloop

    CMP     r2, #0
    BEQ     LDMexit

LDM4Bloop
    LDR     r3, [r1], #4
    STR     r3, [r0], #4
    SUBS    r2, r2,   #4
    BGT     LDM4Bloop

LDMexit

    POP     {r4-r10}

    BX      LR
}
#endif

#include "FastSection.h"
#if (_CPU_ == CPU_ID1)// CPU1 only
const unsigned short int TableForCRC16[256] =  {  // 8005

0x0000,0x8005,0x800f,0x000a,0x801b,0x001e,0x0014,0x8011,0x8033,0x0036,0x003c,0x8039,0x0028,0x802d,0x8027,0x0022,
0x8063,0x0066,0x006c,0x8069,0x0078,0x807d,0x8077,0x0072,0x0050,0x8055,0x805f,0x005a,0x804b,0x004e,0x0044,0x8041,
0x80c3,0x00c6,0x00cc,0x80c9,0x00d8,0x80dd,0x80d7,0x00d2,0x00f0,0x80f5,0x80ff,0x00fa,0x80eb,0x00ee,0x00e4,0x80e1,
0x00a0,0x80a5,0x80af,0x00aa,0x80bb,0x00be,0x00b4,0x80b1,0x8093,0x0096,0x009c,0x8099,0x0088,0x808d,0x8087,0x0082,
0x8183,0x0186,0x018c,0x8189,0x0198,0x819d,0x8197,0x0192,0x01b0,0x81b5,0x81bf,0x01ba,0x81ab,0x01ae,0x01a4,0x81a1,
0x01e0,0x81e5,0x81ef,0x01ea,0x81fb,0x01fe,0x01f4,0x81f1,0x81d3,0x01d6,0x01dc,0x81d9,0x01c8,0x81cd,0x81c7,0x01c2,
0x0140,0x8145,0x814f,0x014a,0x815b,0x015e,0x0154,0x8151,0x8173,0x0176,0x017c,0x8179,0x0168,0x816d,0x8167,0x0162,
0x8123,0x0126,0x012c,0x8129,0x0138,0x813d,0x8137,0x0132,0x0110,0x8115,0x811f,0x011a,0x810b,0x010e,0x0104,0x8101,
0x8303,0x0306,0x030c,0x8309,0x0318,0x831d,0x8317,0x0312,0x0330,0x8335,0x833f,0x033a,0x832b,0x032e,0x0324,0x8321,
0x0360,0x8365,0x836f,0x036a,0x837b,0x037e,0x0374,0x8371,0x8353,0x0356,0x035c,0x8359,0x0348,0x834d,0x8347,0x0342,
0x03c0,0x83c5,0x83cf,0x03ca,0x83db,0x03de,0x03d4,0x83d1,0x83f3,0x03f6,0x03fc,0x83f9,0x03e8,0x83ed,0x83e7,0x03e2,
0x83a3,0x03a6,0x03ac,0x83a9,0x03b8,0x83bd,0x83b7,0x03b2,0x0390,0x8395,0x839f,0x039a,0x838b,0x038e,0x0384,0x8381,
0x0280,0x8285,0x828f,0x028a,0x829b,0x029e,0x0294,0x8291,0x82b3,0x02b6,0x02bc,0x82b9,0x02a8,0x82ad,0x82a7,0x02a2,
0x82e3,0x02e6,0x02ec,0x82e9,0x02f8,0x82fd,0x82f7,0x02f2,0x02d0,0x82d5,0x82df,0x02da,0x82cb,0x02ce,0x02c4,0x82c1,
0x8243,0x0246,0x024c,0x8249,0x0258,0x825d,0x8257,0x0252,0x0270,0x8275,0x827f,0x027a,0x826b,0x026e,0x0264,0x8261,
0x0220,0x8225,0x822f,0x022a,0x823b,0x023e,0x0234,0x8231,0x8213,0x0216,0x021c,0x8219,0x0208,0x820d,0x8207,0x0202,

        };

//-----------------------------------------------------------------------------
/**
    NFIF_Reverse_2byte - Reverse 2 byte.

    @param[in]  data

**/
//-----------------------------------------------------------------------------
static __inline void Reverse_2byte(U16 *data)
{
    U16 temp_data, idx;
    U8 trans_table[16] = {8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7};

    temp_data = *data;
    *data = 0;

    for(idx = 0; idx < 16; idx++)
        *data |= (temp_data >> idx & 0x01) << trans_table[idx];
}

U16 CRC_Calculate(U16 crc, const void* data, U16 len)
{
    U16 idx;
    U8 *p = (unsigned char *) data;

    for (idx = 0; idx < len; idx++)
    {
        crc = ((crc << 8) ^ TableForCRC16[(crc >> 8) & 0xFF])^p[idx];
    }

    crc = ((crc << 8) ^ TableForCRC16[(crc >> 8) & 0xFF])^0x00;
    crc = ((crc << 8) ^ TableForCRC16[(crc >> 8) & 0xFF])^0x00;
    Reverse_2byte(&crc);
    return crc;
}
#endif

#include "PerfSection.h"
#if ((_CPU_ == CPU_ID2) || (_CPU_ == CPU_ID0))// CPU2 & CPU0 only
static const unsigned int crc_tab[] = {
    0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
    0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
    0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
    0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
    0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
    0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
    0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
    0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
    0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
    0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
    0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
    0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
    0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
    0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
    0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
    0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
    0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
    0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
    0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
    0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
    0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
    0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
    0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
    0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
    0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
    0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
    0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
    0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
    0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
    0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
    0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
    0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
    0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
    0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
    0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
    0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
    0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
    0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
    0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
    0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
    0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
    0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
    0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
    0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
    0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
    0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
    0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
    0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
    0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
    0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
    0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
    0x2d02ef8dL
};

U32 MemCrcU32(U32 crc, const void* data, U32 len)
{
    U32 i;
    unsigned char *p = (unsigned char *) data;

    for (i = 0;  i < len;  i ++)
    {
        crc = crc_tab[(crc ^ p[i]) & 0xff] ^ (crc >> 8);
    }

    return crc;
}
#endif
