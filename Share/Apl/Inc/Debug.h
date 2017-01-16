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
//! @brief <b> System debugging API & MACRO  </b>
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Constants definitions: debugging features
//-----------------------------------------------------------------------------
// debugging features
#define ENABLE_CAP_CONTROL     FALSE

// debug messages control
#define DBG_BE_SEQ_ABORT       FALSE
#define DBG_GC_VAC             FALSE

// Uart dynamic baudrate
#if CHK_CODE_TYPE(CODE_MAIN)
    #define DBG_UART_DYNAMIC_BAUDRATE
#endif

//#define _ALEXDEBUG_
//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
/// @brief Debug system state level
#define DBG_BOOT_UP             0       ///< Boot up process, debug module not ready
#define DBG_SHARE_READY         4       ///< shared module is ready, debug module is ready

typedef enum
{
    DBGCPU_0   = 0,
    DBGCPU_1   = 1,
    DBGCPU_2   = 2,
    DBGCPU_SM  = 3,
    DBGCPU_CNT,
}tDbgFlag;

/// @brief Share debug message dump flags
#define DBG_SHARE_MEM_ALLOC         (1<<0)  ///< Dump memory allocation information
#define DBG_SHARE_MEM_FREE          (1<<1)  ///< Dump memory free information

/// @brief Host debug message dump flags
#define DBG_DUMP_HOST_CDB           (1<<0)  ///< Dump host CDB fetch information
#define DBG_DUMP_HOST_XFER          (1<<1)  ///< Dump host data Xfer information
#define DBG_DUMP_HOST_EOC           (1<<2)  ///< Dump host EoC
#define DBG_DUMP_HOST_ERROR         (1<<3)  ///< Dump host SDB if error
#define DBG_DUMP_HOST_REQ_COMP      (1<<4)  ///< Dump host request completion
#define DBG_DUMP_HOST_NONRW         (1<<5)  ///< Dump host commands other than Read or Write
#define DBG_DUMP_HOST_IO_INFO       (1<<6)  ///< Dump host IO command information
#define DBG_DUMP_DECODE_TIME        (1<<7)  ///< Dump decode command time
#define DBG_DUMP_XFER_TIME          (1<<8)  ///< Dump xfer HAL layer setup time
#define DBG_DUMP_XDONE_TIME         (1<<9)  ///< Dump xfer done handle time
#define DBG_DUMP_FLUSH_OP           (1<<10) ///< Dump Flush operation flow
#define DBG_DUMP_SRAM_BITMAP        (1<<11) ///< Dump SRAM Bitmap
#define DBG_DUMP_ALL_INFO_RST       (1<<12) ///< Dump All information in reset
#define DBG_DUMP_TRIM_MSG           (1<<13) ///< Dump TRIM DBG message
#define DBG_DUMP_ERRHANDLE          (1<<14) ///< Dump Error Hnalde
#define DBG_DUMP_HOST_ADMIN_INFO    (1<<15) ///< Dump host Admin command information
#define DBG_DUMP_ABORT_INFO         (1<<16) ///< Dump Abort command information

/// @brief Media Debug flags
#define DBG_DUMP_MEDIA_MSG          (1<<0)  ///< Dump media message
#define DBG_DUMP_MEDIA_ADDR         (1<<1)  ///< Dump FTL address conversion
#define DBG_DUMP_MEDIA_STATE        (1<<2)  ///< Dump media task state
#define DBG_DUMP_MEDIA_DONE         (1<<3)  ///< Dump media done message
#define DBG_DUMP_MEDIA_SEQ          (1<<4)  ///< Dump media sequencer FIFO data
#define DBG_DUMP_MEDIA_FTL_ISSUEJOB (1<<5)  ///< Dump FTL Issue Job list.
#define DBG_DUMP_MEDIA_DESC_PNA     (1<<6)  ///< Dump PNA in Sequencer descriptor
#define DBG_DUMP_MEDIA_FTL_DM       (1<<7)  ///< Dump FTL defect management
#define DBG_DUMP_MEDIA_FTL_MAP      (1<<8)  ///< Dump FTL address mapping
#define DBG_DUMP_MEDIA_FTL_META     (1<<9)  ///< Dump FTL META data
#define DBG_DUMP_MEDIA_FTL_IF       (1<<10) ///< Dump FTL interface
#define DBG_DUMP_MEDIA_FTL_WERU     (1<<11) ///< Dump FTL WERU
#define DBG_DUMP_MEDIA_FTL_UTIL     (1<<12) ///< Dump FTL Utility
#define DBG_DUMP_MEDIA_ADDRESS      (1<<13) ///< Dump media address management info
#define DBG_DUMP_MEDIA_MOH          (1<<14) ///< Dump Media MOH
#define DBG_DUMP_MEDIA_MIO          (1<<15) ///< Dump Media MIO
#define DBG_MEDIA_CMD_FLOW          (1<<16) ///< Dump media message

#define DBG_DUMP_MEDIA_NAND_CH      (1<<16) ///< Dump NandCharacterization debug messages

/// @brief Core Debug Flags
#define DBG_DUMP_CORE_MSG           (1<<0)  ///< Dump Core message
#define DBG_DUMP_CORE_STATE         (1<<1)  ///< Dump Core states
#define DBG_DUMP_CORE_DONE          (1<<2)  ///< Dump Core done message
#define DBG_DUMP_CACHE_INFO         (1<<3)  ///< Dump Core cache entry/LL information
#define DBG_DUMP_FIOJOB_INFO        (1<<4)  ///< Dump Core FIO/job entry/LL information
#define DBG_DUMP_GCOP_INFO          (1<<5)  ///< Dump Core GC OP information
#define DBG_DUMP_GCBLK_INFO         (1<<6)  ///< Dump Core GC OP information
#define DBG_DUMP_CS_INFO            (1<<7)  ///< Dump Core GC OP information
#define DBG_DUMP_MSG_INFO           (1<<8)  ///< Dump Core host msg information
#define DBG_AUXFUN_CORE_MSG_EN      (1<<0)  ///< Set Enable Re-arrange msg funtion

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define M_PRINTF(F, args...)        Debug_Printf(F, ##args)         ///< Monitor message print out
#define D_PRINTF(F, args...)        Debug_DbgPrintf(F, ##args)      ///< Debug message print out
//#define D_PRINTF(F,args...)         Debug_FakePrintf(F, ##args)     ///< Debug message print out
#define E_PRINTF(E, F, args...)     Debug_ErrPrintf(E, F, ##args)   ///< Error message print out

#ifdef DBG_UART_DYNAMIC_BAUDRATE
    #if ((_CPU_== CPU_ID0) || (_CPU_ == CPU_ID1))
        #define DEBUG_UART_GET_STATUS()     {if (smUartDynamic.Lock) return;}
    #else
        #define DEBUG_UART_GET_STATUS()
    #endif
    #define DEBUG_UART_SET_BUSY(s)          smUartDynamic.Printing[_CPU_] = s
#else
    #define DEBUG_UART_GET_STATUS(s)
    #define DEBUG_UART_SET_BUSY(s)
#endif

#ifdef _ALEXDEBUG_
    #define zBLACK                      "\033[7;30m"
    #define zRED                        "\033[7;31m"
    #define zGREEN                      "\033[7;32m"
    #define zYELLOW                     "\033[7;33m"
    #define zBLUE                       "\033[7;34m"
    #define zMAGENTA                    "\033[7;35m"
    #define zCYAN                       "\033[7;36m"
    #define zWHITE                      "\033[7;37m"
    #define zNONE                       "\033[m"
    #define alexprn_red(F,args...)      Debug_DbgPrintf(zRED##F##zNONE, ##args)    ///< alex debug message print out
    #define alexprn_cya(F,args...)      Debug_DbgPrintf(zCYAN##F##zNONE, ##args)    ///< alex debug message print out
    #define alexprn_mge(F,args...)      Debug_DbgPrintf(zMAGENTA##F##zNONE, ##args)  ///< alex debug message print out
    #define alexprn1(a)                 alexprn_red(#a" <%x>\n",(U32)##a)
    #define alexprn2(a,b)               alexprn_red(#a" <%x> "#b" <%x>\n",(U32)##a,(U32)##b)
    #define alexprn3(a,b,c)             alexprn_red(#a" <%x> "#b" <%x> "#c" <%x>\n",(U32)##a,(U32)##b,(U32)##c)
    #define alexprn4(a,b,c,d)           alexprn_red(#a" <%x> "#b" <%x> "#c" <%x> "#d" <%x>\n",(U32)##a,(U32)##b,(U32)##c,(U32)##d)
    #define A_HERE                      alexprn_cya("%s %s %d\n",__MODULE__, __FUNCTION__, __LINE__)
    #define A_LANDMARK(a)               #a == "NULL" ? alexprn_mge("%s %s %d\n",__MODULE__, __FUNCTION__, __LINE__) : \
                                                    alexprn_mge("%s %s %d -> "#a" <%x>\n",__MODULE__, __FUNCTION__, __LINE__, (U32)##a)
    #define alexdump(a)                 {                                                        \
                                            U16 x,y;                                             \
                                            for(x=0; x<16; x++){                                 \
                                                D_PRINTF("%2X: ", x*16);                         \
                                                for(y=0; y<16; y++){                             \
                                                    D_PRINTF("%2X ",(U8)(*((U8 *)(a)+x*16+y)));  \
                                                    if(y==7) D_PRINTF("- ");                     \
                                                }                                                \
                                                D_PRINTF("\n");                                  \
                                            }                                                    \
                                        }
#else
    #define alexprn(F,args...)          {}    ///< nothing to do
    #define alexprn1(a)                 {}
    #define alexprn2(a,b)               {}
    #define alexprn3(a,b,c)             {}
    #define alexprn4(a,b,c,d)           {}
    #define A_HERE                      {}
    #define A_LANDMARK(a)               {}
    #define alexdump(a)                 {}
#endif

#define HALT()                      { Debug_Halt(__FUNCTION__, __FILE__, __LINE__, __return_address()); }
#define HALT1(X)                    { Debug_Halt((X), __FILE__, __LINE__, __return_address()); }
#define ASSERT(X)                   { if (!(X)) Debug_AssertFail(#X, __FILE__, __LINE__, __return_address()); }
#define ASSERT2(X,Y)                { if (!(X)) Debug_AssertFail(#X, __FILE__, Y, __return_address()); }

// Debug dump control macro functions.
#define BEGIN_SHARE_DBG_DUMP(X)     if ((smDebugFlags[DBGCPU_SM])&(X)) {        ///< Share Dump
#define END_SHARE_DBG_DUMP          }

#define BEGIN_HOST_DBG_DUMP(X)      if ((smDebugFlags[DBGCPU_0])&(X)) {        ///< HOST Dump
#define END_HOST_DBG_DUMP           }

#define BEGIN_MEDIA_DBG_DUMP(X)     if ((smDebugFlags[DBGCPU_1])&(X)) {        ///< MEDIA Dump
#define END_MEDIA_DBG_DUMP          }

#define BEGIN_CORE_DBG_DUMP(X)      if ((smDebugFlags[DBGCPU_2])&(X)) {        ///< CORE Dump
#define END_CORE_DBG_DUMP           }

#define CS_PRINTF(F,args...)        {                                         \
                                        BEGIN_CORE_DBG_DUMP(DBG_DUMP_CS_INFO) \
                                        Debug_Printf(F, ##args)               \
                                        END_CORE_DBG_DUMP                     \
                                    }
// Debug dump configuration macro functions.
#define SET_SHARE_DEBUG_FLAG(X)     (smDebugFlags[DBGCPU_SM] |= (X))
#define SET_CPU_0_DEBUG_FLAG(X)     (smDebugFlags[DBGCPU_0]  |= (X))
#define SET_CPU_1_DEBUG_FLAG(X)     (smDebugFlags[DBGCPU_1]  |= (X))
#define SET_CPU_2_DEBUG_FLAG(X)     (smDebugFlags[DBGCPU_2]  |= (X))

#define CLR_SHARE_DEBUG_FLAG(X)     (smDebugFlags[DBGCPU_SM] &= (~X))
#define CLR_CPU_0_DEBUG_FLAG(X)     (smDebugFlags[DBGCPU_0]  &= (~X))
#define CLR_CPU_1_DEBUG_FLAG(X)     (smDebugFlags[DBGCPU_1]  &= (~X))
#define CLR_CPU_2_DEBUG_FLAG(X)     (smDebugFlags[DBGCPU_2]  &= (~X))

#define CLR_CPU_DEBUG_FLAG(X)       (smDebugFlags[_CPU_]     &= (~X))

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef struct
{
    uint32_t    Lock;
    uint32_t    Printing[3/*SOC_NUM_OF_CPU*/];
} UartDynamic_t;

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern volatile U32             smDebugFlags[];      ///< debug Monitor dump flags for Core, Host and Media
extern volatile UartDynamic_t   smUartDynamic;

extern void* stdOut;            ///< normal messeage output
extern void* stdDbg;            ///< debug  messeage output
extern void* stdErr;            ///< error  messeage output

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t Debug_OneTimeInit(InitBootMode_t initMode);

void    Debug_Halt(Cstr_t funcName, Cstr_t fileName, uint32_t auxInfo, uint32_t callerAddress);
void    Debug_AssertFail(Cstr_t assertTest, Cstr_t fileName, uint32_t auxInfo, uint32_t callerAddress );
void    Debug_Printf(Cstr_t fmtStr, ...);
void    Debug_DbgPrintf(Cstr_t fmtStr, ...);
void    Debug_FakePrintf(Cstr_t fmtStr, ...);
void    Debug_ErrPrintf(Error_t error, Cstr_t fmtStr, ...);

void    Debug_DumpMemU8(uint32_t address, uint32_t dumpByteSize);
void    Debug_DumpMemU16(uint32_t address, uint32_t dumpByteSize);
void    Debug_DumpMemU32(uint32_t address, uint32_t dumpByteSize);
void    Debug_DumpMemU32PerLine(uint32_t address, uint32_t dumpByteSize, uint32_t bytesPerLine);
void    Debug_DumpCore(bool halt);

void    Debug_DumpVersion(void);
