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
//! @brief <b> System primitive data type, macro and constant definitions </b>
//
//  DO NOT USE ANY CONDITIONAL COMPILE OPTION !!
//
//=============================================================================

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
#define NONE        (0)                     ///< NONE, numeric value is 0

#define FALSE       (0)                     ///< FALSE, numeric value is 0
#define TRUE        (1)                     ///< TRUE,  numeric value is 1

#define OFF         (0)                     ///< OFF, numeric value is 0
#define ON          (1)                     ///<  ON, numeric value is 1

#define DISABLE     (0)                     ///< Disable
#define ENABLE      (1)                     ///< Enable

#define U8_MAX      (0xFF)                  ///< max value for uint8_t  data type
#define U16_MAX     (0xFFFF)                ///< max value for uint16_t data type
#define U32_MAX     (0xFFFFFFFF)            ///< max value for uint32_t data type
#define U64_MAX     (0xFFFFFFFFFFFFFFFFLL)  ///< max value for uint64_t data type

#define S8_MAX      ( 0x7F )
#define S8_MIN      ( S8_MAX + 1 )

#define KBYTE       (1<<10)                 ///< Kilo-bytes 1024
#define MBYTE       (1<<20)                 ///< mega-bytes 1048576
#define GBYTE       (1<<30)                 ///< giga-bytes 1073741824

#define KHZ         (1000)                  ///< kilo-hertz
#define MHZ         (1000000)               ///< mega-hertz
#define GHZ         (1000000000)            ///< giga-hertz

#ifndef NULL
#define NULL        ((void *)0)             ///< NULL pointer
#endif

#define U8_ALL_BIT_SET     (0xFF)                  ///< all bits are set.
#define U16_ALL_BIT_SET    (0xFFFF)                ///< all bits are set.
#define U32_ALL_BIT_SET    (0xFFFFFFFF)            ///< all bits are set.
#define U64_ALL_BIT_SET    (0xFFFFFFFFFFFFFFFFLL)  ///< all bits are set.

#define FEATURE_SUPPORTED               ENABLE
#define FEATURE_NOT_SUPPORTED           DISABLE

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

/// Macro realted memory access

#define MEM_U8(addr)            ( *( uint8_t* ) (addr) )    ///< memory I/O for uint8_t
#define MEM_S8(addr)            ( *( int8_t* ) (addr) )     ///< memory I/O for int8_t
#define MEM_U16(addr)           ( *( uint16_t* ) (addr) )   ///< memory I/O for uint16_t
#define MEM_S16(addr)           ( *( int16_t* ) (addr) )    ///< memory I/O for int16_t
#define MEM_U32(addr)           ( *( uint32_t* ) (addr) )   ///< memory I/O for uint32_t
#define MEM_S32(addr)           ( *( int32_t* ) (addr) )    ///< memory I/O for int32_t
#define MEM_U64(addr)           ( *( uint64_t* ) (addr) )   ///< memory I/O for uint64_t
#define MEM_S64(addr)           ( *( int64_t* ) (addr) )    ///< memory I/O for int64_t

/// Macro realted register access

#define REG_U8(addr)            ( *( volatile uint8_t* ) (addr) )   ///< memory mapped register I/O for uint8_t
#define REG_S8(addr)            ( *( volatile int8_t* ) (addr) )    ///< memory mapped register I/O for int8_t
#define REG_U16(addr)           ( *( volatile uint16_t* ) (addr) )  ///< memory mapped register I/O for uint16_t
#define REG_S16(addr)           ( *( volatile int16_t* ) (addr) )   ///< memory mapped register I/O for int16_t
#define REG_U32(addr)           ( *( volatile uint32_t* ) (addr) )  ///< memory mapped register I/O for uint32_t
#define REG_S32(addr)           ( *( volatile int32_t* ) (addr) )   ///< memory mapped register I/O for int32_t
#define REG_U64(addr)           ( *( volatile uint64_t* ) (addr) )  ///< memory mapped register I/O for uint64_t
#define REG_S64(addr)           ( *( volatile int64_t* ) (addr) )   ///< memory mapped register I/O for int64_t

// Register, Bit field write with mask,
#define REG_U8_FW(addr,v,mask)  (REG_U8(addr) = ((REG_U8(addr)&(~(mask)))|(v)))   ///< bit field write
#define REG_U16_FW(addr,v,mask) (REG_U16(addr) = ((REG_U16(addr)&(~(mask)))|(v))) ///< bit field write
#define REG_U32_FW(addr,v,mask) (REG_U32(addr) = ((REG_U32(addr)&(~(mask)))|(v))) ///< bit field write

// Register, Bit field write with bit positions, ADDR[bp1:bp0] = value
#define REG_U8_BW(addr, bp1, bp0, value)  REG_U8_FW((addr), ((value)<<(bp0)), (((0xFFU)>>(7-(bp1)))<<(bp0)))
#define REG_U16_BW(addr, bp1, bp0, value) REG_U16_FW((addr), ((value)<<(bp0)), (((0xFFFFU)>>(15-(bp1)))<<(bp0)))
#define REG_U32_BW(addr, bp1, bp0, value) REG_U32_FW((addr), ((value)<<(bp0)), (((0xFFFFFFFFU)>>(31-(bp1)))<<(bp0)))

/// Macro realted bit manipulation

#define B_0         0x00000001              ///< Bit value definition
#define B_1         0x00000002
#define B_2         0x00000004
#define B_3         0x00000008
#define B_4         0x00000010
#define B_5         0x00000020
#define B_6         0x00000040
#define B_7         0x00000080
#define B_8         0x00000100
#define B_9         0x00000200
#define B_10        0x00000400
#define B_11        0x00000800
#define B_12        0x00001000
#define B_13        0x00002000
#define B_14        0x00004000
#define B_15        0x00008000
#define B_16        0x00010000
#define B_17        0x00020000
#define B_18        0x00040000
#define B_19        0x00080000
#define B_20        0x00100000
#define B_21        0x00200000
#define B_22        0x00400000
#define B_23        0x00800000
#define B_24        0x01000000
#define B_25        0x02000000
#define B_26        0x04000000
#define B_27        0x08000000
#define B_28        0x10000000
#define B_29        0x20000000
#define B_30        0x40000000
#define B_31        0x80000000
#define B_32        0x0000000100000000
#define B_33        0x0000000200000000
#define B_34        0x0000000400000000
#define B_35        0x0000000800000000
#define B_36        0x0000001000000000
#define B_37        0x0000002000000000
#define B_38        0x0000004000000000
#define B_39        0x0000008000000000
#define B_40        0x0000010000000000
#define B_41        0x0000020000000000
#define B_42        0x0000040000000000
#define B_43        0x0000080000000000
#define B_44        0x0000100000000000
#define B_45        0x0000200000000000
#define B_46        0x0000400000000000
#define B_47        0x0000800000000000
#define B_48        0x0001000000000000
#define B_49        0x0002000000000000
#define B_50        0x0004000000000000
#define B_51        0x0008000000000000
#define B_52        0x0010000000000000
#define B_53        0x0020000000000000
#define B_54        0x0040000000000000
#define B_55        0x0080000000000000
#define B_56        0x0100000000000000
#define B_57        0x0200000000000000
#define B_58        0x0400000000000000
#define B_59        0x0800000000000000
#define B_60        0x1000000000000000
#define B_61        0x2000000000000000
#define B_62        0x4000000000000000
#define B_63        0x8000000000000000

#define BIT(pos)                (1U << (pos))                       ///< Convert bit position to value
#define BITS(H, L)              ((BIT((H)+1)-1) & ~(BIT((L))-1))    ///< Convert bit range to value
#define BITS_U32(H, L)          ((U32_ALL_BIT_SET>>(31-(H))) & (U32_ALL_BIT_SET << (L)))    // Convet bit range to value for U32 (fast?)

#define MAKE_U16(HIGH,LOW)      (((uint16_t)(HIGH)<<8)|(LOW))        ///< Make uint16_t with two uint8_t  HIGH and LOW
#define MAKE_U32(HIGH,LOW)      (((uint32_t)(HIGH)<<16)|(LOW))       ///< Make uint32_t with two uint16_t HIGH and LOW
#define MAKE_U64(HIGH,LOW)      (((uint64_t)(HIGH)<<32)|(LOW))       ///< Make uint64_t with two uint32_t HIGH and LOW

#define GET_B07_00(X)           ((uint8_t)(X))                       ///< Get lowest byte
#define GET_B15_08(X)           ((uint8_t)((X)>>8))
#define GET_B23_16(X)           ((uint8_t)((X)>>16))
#define GET_B31_24(X)           ((uint8_t)((X)>>24))
#define GET_B15_00(X)           ((uint16_t)(X))
#define GET_B23_08(X)           ((uint16_t)((X)>>8))
#define GET_B31_16(X)           ((uint16_t)((X)>>16))
#define GET_B31_00(X)           ((uint32_t)(X))                      ///< Get lower DWORD
#define GET_B63_32(X)           ((uint32_t)((X)>>32))                ///< Get upper DWORD

#define GET_BIT(X, BP)          (((X)>>(BP))&1)
#define GET_BITS(X, H, L)       (((X)>>(L))&(BIT(H-L+1)-1))
#define GET_BITS_RANGE(X,H,L)   ((X) & BITS(H, L))

#define BIT_SET(var, bits)      (var |= (bits))                     ///< Bits set
#define BIT_CLR(var, bits)      (var &= (~(bits)))                  ///< Bits clear
#define BIT_XOR(var, bits)      (var ^= (bits))                     ///< Bits XOR
#define BIT_FW(var,v,mask)      (var = (var&(~(mask)))|v)           ///< Bits field write

// Set one bit in bitmap array type uint8_t, uint16_t, uint32_t (LSB is bit position 0)
#define BA_SET_U8_L2M(BUFP, BITP)     ((BUFP)[(BITP)/8] |= ((uint8_t)1<<((BITP)%8)))
#define BA_CLR_U8_L2M(BUFP, BITP)     ((BUFP)[(BITP)/8] &= (~((uint8_t)1<<((BITP)%8))))
#define BA_SET_U16_L2M(BUFP, BITP)    ((BUFP)[(BITP)/16] |= ((uint16_t)1<<((BITP)%16)))
#define BA_CLR_U16_L2M(BUFP, BITP)    ((BUFP)[(BITP)/16] &= (~((uint16_t)1<<((BITP)%16))))
#define BA_SET_U32_L2M(BUFP, BITP)    ((BUFP)[(BITP)/32] |= ((uint32_t)1<<((BITP)%32)))
#define BA_CLR_U32_L2M(BUFP, BITP)    ((BUFP)[(BITP)/32] &= (~((uint32_t)1<<((BITP)%32))))
#define BA_WR1_U32_L2M(BUFP, BITP)    ((BUFP)[(BITP)/32] = ((uint32_t)1<<((BITP)%32)))

// test bit one bit in bitmap array type uint8_t, uint16_t, uint32_t (LSB is bit position 0)
#define BA_TESTB_U8_L2M(BUFP, BITP)   ((BUFP)[(BITP)/8] & (1<<((BITP)%8)))
#define BA_TESTB_U16_L2M(BUFP, BITP)  ((BUFP)[(BITP)/16] & (1<<((BITP)%16)))
#define BA_TESTB_U32_L2M(BUFP, BITP)  ((BUFP)[(BITP)/32] & (1<<((BITP)%32)))

// Clear one bit in bitmap array type uint8_t, uint16_t, uint32_t (MSB is bit position 0)
#define BA_SET_U8_M2L(BUFP, BITP)     ((BUFP)[(BITP)/8] |= (0x80>>(BITP)%8))
#define BA_CLR_U8_M2L(BUFP, BITP)     ((BUFP)[(BITP)/8] &= (~(0x80>>(BITP)%8)))
#define BA_SET_U16_M2L(BUFP, BITP)    ((BUFP)[(BITP)/16] |= (0x8000>>(BITP)%16))
#define BA_CLR_U16_M2L(BUFP, BITP)    ((BUFP)[(BITP)/16] &= (~(0x8000>>(BITP)%16)))
#define BA_SET_U32_M2L(BUFP, BITP)    ((BUFP)[(BITP)/32] |= (0x80000000>>(BITP)%32))
#define BA_CLR_U32_M2L(BUFP, BITP)    ((BUFP)[(BITP)/32] &= (~(0x80000000>>(BITP)%32)))
#define BA_WR1_U32_M2L(BUFP, BITP)    ((BUFP)[(BITP)/32] = (0x80000000>>(BITP)%32))

// test bit one bit in bitmap array type uint8_t, uint16_t, uint32_t (MSB is bit position 0)
#define BA_TESTB_U8_M2L(BUFP, BITP)   ((BUFP)[(BITP)/8] & (0x80>>(BITP)%8))
#define BA_TESTB_U16_M2L(BUFP, BITP)  ((BUFP)[(BITP)/16] & (0x8000>>(BITP)%16))
#define BA_TESTB_U32_M2L(BUFP, BITP)  ((BUFP)[(BITP)/32] & (0x80000000>>(BITP)%32))

/// MACRO related algined/unaligned

#define PACKED_U16(addr)        (((PackedU16_t *)addr)->word)   ///< Unaligned 16bit data I/O
#define PACKED_U32(addr)        (((PackedU32_t *)addr)->dword)  ///< unaligned 32bit data I/O

#define TO_U8(data)             ((U8)((data) & U8_MAX))         ///< convert to U8 type
#define TO_U16(data)            ((U16)((data) & U16_MAX))       ///< convert to U16 type
#define TO_U32(data)            ((U32)((data) & U32_MAX))       ///< convert to U32 type
#define TO_U64(data)            ((U64)(data))                   ///< convert to U64 type

#define TEST_U16_ALIGNED(X)     (((uint32_t)(X)&1)==0)              ///< Test uint16_t(word) aligned
#define TEST_U32_ALIGNED(X)     (((uint32_t)(X)&3)==0)              ///< Test uint32_t(dword) aligned

#define TEST_ALIGNED(X,A)       (((uint32_t)(X)&(A-1))==0)          ///< Test A size aligned (A shall be power of 2)

#define ALIGN_FLOOR(X,A)        ((uint32_t)(X)&(~(A-1)))            ///< the largest aligned value that is not greater than X
#define ALIGN_CEIL(X,A)         ALIGN_FLOOR((X)+(A-1),A)            ///< the smallest aligned value that is not less than x

#define CONVERT_UNIT_FLOOR(X,U) ((X)/(U))                           ///< unit conversion, largest value that is not greater than X
#define CONVERT_UNIT_CEIL(X,U)  (((X)+(U)-1)/(U))                   ///< unit conversion, smallest value that is not less than X

/// MACRO related MIN and MAX

#define MAX(x,y)                ((x)>(y) ? (x) : (y))               ///< maximum value of two
#define MIN(x,y)                ((x)<(y) ? (x) : (y))               ///< minimum value of two

/// MACRO related CLZ (Count Leading Zeros) and FFS (Find First Set)
#define __FFS(W,X)              ((W) - __clz((X) & (-(X))))       ///< Find First Set using CLZ (Count Leading Zeros), (1 to N, 0 is invalid)
#define FFS(X)                  (__FFS(32, (X)) - 1)              ///< Find First Set for uint32_t from LSB to MSB (0 to N-1, 0xFFFF is invalid)
#define FFS_RANGE(X,H,L)        (FFS(GET_BITS_RANGE((X),(H),(L))))///< Find First Set in bits range from LSB to MSB (0 to N-1, 0xFFFF is invalid)
#define FFS_RANGE_RVS(X,H,L)    (31 - __clz((X) & GET_BITS_RANGE((X),(H),(L)))) ///< Find First Set in bits range from MSB to LSB (N-1 to 0, 0xFFFF is invalid)

#define READ32(offset)          *(volatile uint32_t*)(offset)
#define WRITE32(offset , value) *(volatile uint32_t*)(offset) = value
#define READ16(offset)          (*(volatile uint16_t*)(offset))
#define WRITE16(offset , value) (*(volatile uint16_t*)(offset)) = value

#define LOG_1(n)                (((n) >= 2) ? 1 : 0)
#define LOG_2(n)                (((n) >= 1<< 2) ? (2  + LOG_1((n)>> 2)) : LOG_1(n))
#define LOG_4(n)                (((n) >= 1<< 4) ? (4  + LOG_2((n)>> 4)) : LOG_2(n))
#define LOG_8(n)                (((n) >= 1<< 8) ? (8  + LOG_4((n)>> 8)) : LOG_4(n))
#define LOG(n)                  (((n) >= 1<<16) ? (16 + LOG_8((n)>>16)) : LOG_8(n))

/// MACRO related structure size, offset

#define SIZEOF_MEMBER(s,m)      sizeof(((s *)0)->m)                 ///< size of the member in structure

#define STR_PTR_FROM_MEMBER(addr,s,m) ((s *)((uint8_t *)(addr) - offsetof(s,m)))

#define ARRAY_INDEX_FROM_PTR(A, P) (((uint32_t)(P)-(uint32_t)(A))/sizeof(A[0]))  ///< compute array index from pointer

#define COMPILE_ASSERT(exp,str) extern char __ct_[(exp) ? 1 : -1] ///< compile time assert

/**
 * The reinterpret_cast macro changes one data type into another. It should be used
 * to cast between incompatible pointer types.
 *
 *  @param Type       [in ] Pointer type
 *  @param Expression [in ] Expression to be casted.
 *
 */
#define REINTERPRET_CAST(Type, Expression)  (Type)(void*)(Expression)

#define CHECK_RETURN(x,EC)                  { if(x) return EC; }
#define CHECK_RETURN2(x,PC,EC)              { if(x) {PC; return EC;} }

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef const char*     Cstr_t;   ///< constant string pointer

/// @brief packed structure to access unaligned word(16bit) data
typedef __packed union
{
    uint16_t word;
    __packed struct
    {
        uint8_t low;
        uint8_t high;
    } byte;
} PackedU16_t;

/// @brief packed structure to access unaligned dword(32bit) data
typedef __packed union
{
    uint32_t dword;
    __packed struct
    {
        uint16_t low;
        uint16_t high;
    } word;
    __packed struct
    {
        uint8_t b0;
        uint8_t b1;
        uint8_t b2;
        uint8_t b3;
    } byte;
} PackedU32_t;

/// @brief union data structure for aligned word(16bit) data
typedef union
{
    uint16_t word;
    struct
    {
        uint8_t low;
        uint8_t high;
    } byte;
} UnionU16_t;

/// @brief union data structure for aligned dword(32bit) data
typedef union
{
    uint32_t dword;
    struct
    {
        uint16_t low;
        uint16_t high;
    } word;
    struct
    {
        uint8_t b0;
        uint8_t b1;
        uint8_t b2;
        uint8_t b3;
    } byte;
} UnionU32_t;

/// @brief union data structure for aligned qword(64bit) data
typedef union
{
    uint64_t qword;
    struct
    {
        uint32_t dw0;
        uint32_t dw1;
    } dword;
    struct
    {
        uint16_t w0;
        uint16_t w1;
        uint16_t w2;
        uint16_t w3;
    } word;
    struct
    {
        uint8_t b0;
        uint8_t b1;
        uint8_t b2;
        uint8_t b3;
        uint8_t b4;
        uint8_t b5;
        uint8_t b6;
        uint8_t b7;
    } byte;
} UnionU64_t;


/// @brief Doubly Linked List
typedef struct
{
    void* prev;     ///< prevoius pointer (tail pointer)
    void* next;     ///< next pointer (head pointer)
} DlinkEntry_t;

/// @brief Singly Linked List
typedef struct
{
    void* next;     ///< next pointer
} SlinkEntry_t;

/// @brief List head and tail
typedef struct
{
    void* tail;     ///< tail pointer
    void* head;     ///< head pointer
} LinkHeadTail_t;

/// @brief Doubly Linked List by 16 bits offset pointer (to reduce memory size)
typedef struct
{
    uint16_t prev; ///< low 16 bits offset of previous pointer
    uint16_t next; ///< low 16 bits offset of next pointer
} DlinkOffset_t;

/// @brief Singly Linked List by 16 bits offset pointer (to reduce memory size)
typedef struct
{
    uint16_t next; ///< low 16 bits offset of next pointer
} SlinkOffset_t;

/// @brief List head and tail by 16bit pointer (to reduce memory size)
typedef struct
{
    uint16_t tail;  ///< low 16 bits offset of tail pointer
    uint16_t head;  ///< low 16 bits offset of head pointer
} LinkHeadTailOffset_t;

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef unsigned int        BOOL;   ///< TRUE, FALSE or ENABLE, DISABLE or ON, OFF

typedef unsigned char       U8;     ///< unsigned  8 bit integer
typedef unsigned short      U16;    ///< unsigned 16 bit integer
typedef unsigned int        U32;    ///< unsigned 32 bit integer
typedef unsigned long long  U64;    ///< unsigned 64 bit integer, 8 byte data

typedef signed char         S8;     ///< signed  8 bit integer
typedef signed short        S16;    ///< signed 16 bit integer
typedef signed int          S32;    ///< signed 32 bit integer
typedef signed long long    S64;    ///< signed 64 bit integer

typedef float               F32;    ///< 32 bit floating point number
typedef double              F64;    ///< 64 bit floating point number

typedef void*               PVOID;  ///< void pointer

typedef const char*         CSTR;   ///< constant string pointer
