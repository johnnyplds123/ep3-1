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
//! @brief <b> Device data type, macro and variables </b>
//
//=============================================================================
#include "BuildCommon.h"

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Block definitions
//-----------------------------------------------------------------------------
/// @brief Initialize boot mode identifier
typedef enum
{
    cInitBootCold = 1,          ///< cold boot (power on)
    cInitBootCpu0Boot,          ///< cold boot for cpu0 boot code
    cInitBootFwUpdated,         ///< Firmware is updated (preserve all host state)
    cInitBootOffLine,           ///< exit form PCIe No link Mode
    cInitBootPowerDown,         ///< exit from Power Down mode
    cInitBootDeepPowerDown,     ///< exit from Deep Power Down mode
    cInitBootNA = 0x7FFFFFFF
} InitBootMode_t;

/// @brief reset mode identifier
typedef enum
{
    cResetHard = 1,             ///< hard reset
    cResetSoft,                 ///< soft reset
    cResetFwUpdate              ///< FW Update reset
} ResetMode_t;

/// @brief Device Power mode
typedef enum
{
    cDevicePmActive = 0,        ///< Max performance state (full power up)
    cDevicePmSleep,             ///< Basic power down mode 1
    cDevicePmDeepSleep,         ///< Lowest power consumption (turned most of HW blocks off)
    cDevicePmNone = 0x7FFFFFFF  ///< Lowest power consumption (turned most of HW blocks off)
} DevicePowerMode_t;

/// @brief host data transfer direction
typedef enum
{
    cXferD2h = 0,               ///< Device to host transfer (Host read, data in)
    cXferH2d = 1                ///< Host to device transfer (Host write, data out)
} XferDir_t;

/// @brief host data transfer flow control types
typedef enum
{
    cXferNonStreaming = 0,      ///< Non-streaming, for Whole data transfer
    cXferStreamingId1,          ///< Streaming ID 1
    cXferStreamingId2,          ///< Streaming ID 2
    cXferStreamingId3,          ///< Streaming ID 3
    cXferStreamingId4,          ///< Streaming ID 4
    cXferStreamingId5,          ///< Streaming ID 5
    cXferStreamingId6,          ///< Streaming ID 6
    cXferStreamingId7,          ///< Streaming ID 7
    cXferIds = 15               ///< Internal Data Storage
} XferFlowCtrl_t;

/// @brief Host data transfer start
typedef enum
{
    cHostStartXfer = 0,         ///< Host FW starts data transfer
    cHostNotStartXfer           ///< Host FW does not start data transfer
} XferStart_t;

typedef enum
{
    cThermalSourceSoc = 0,
    cThermalSourceI2c,
    cThermalSourceCount
} ThermalSource_t;

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define LBU_SIZE        512         ///< size of logical block

#define  BYTE2LBU(X)    ((X)>>9)    ///<  Byte unit to LBU unit conversion
#define KBYTE2LBU(X)    ((X)<<1)    ///< KByte unit to LBU unit conversion
#define MBYTE2LBU(X)    ((X)<<11)   ///< MByte unit to LBU unit conversion
#define GBYTE2LBU(X)    ((X)<<21)   ///< GByte unit to LBU unit conversion

#define LBU2BYTE(X)     ((X)<<9)    ///< LBU unit to  Byte unit conversion
#define LBU2KBYTE(X)    ((X)>>1)    ///< LBU unit to KByte unit conversion
#define LBU2MBYTE(X)    ((X)>>11)   ///< LBU unit to MByte unit conversion
#define LBU2GBYTE(X)    ((X)>>21)   ///< LBU unit to GByte unit conversion

#define NYBBLE_SHIFT    ( 4 )
#define BYTE_SHIFT      ( 8 )
#define WORD_SHIFT      ( 16 )
#define DWORD_SHIFT     ( 32 )
#define NYBBLE_MASK     ( 0xF )
#define BYTE_MASK       ( 0xFF )
#define WORD_MASK       ( 0xFFFF )
#define DWORD_MASK      ( 0xFFFFFFFF )

#define BITS_PER_BYTE   ( 8 )
#define BITS_PER_WORD   ( 16 )
#define BITS_PER_DWORD  ( 32 )
#define BIT2BYTE(X)     ((X) >> 3)    ///< Bit Count to Byte Count conversion

#define _BX_(X)         ((X) - (((X) >> 1) & 0x77777777) - \
                               (((X) >> 2) & 0x33333333) - \
                               (((X) >> 3) & 0x11111111))
#define BIT_COUNT(X)    (((_BX_(X) + (_BX_(X) >> 4)) & 0x0F0F0F0F) % 255)
#define CTZ(X)          ((X) ? (31 - (__clz((X) & -(X))) : (__clz((X) & -(X)))) // Count Trailing Zero
#define LOG2(X)         (31-__clz(X))                                           ///< Log 2

#define U64_TO_U32_H(X)     (U32)((X >> 32))
#define U64_TO_U32_L(X)     (U32)(X)

/// IDEMA Capacity formula
#define IDEMA_GBYTE2LBU(X) (97696368 + 1953504*((X) - 50))
#define IDEMA_LBU2GBYTE(X) ((((X)-97696368)/1953504) + 50)

/// @name Memory section types for code
/// @{
#define __CODE_PERF __attribute__((section("code_perf")))
#define __CODE_FAST __attribute__((section("code_fast")))
#define __CODE_SLOW __attribute__((section("code_slow")))
#define __CODE_NORM __attribute__((section("code_norm")))
#define __CODE_DRAM __attribute__((section("code_dram")))
/// @}

/// @name Memory section types for zero or value initialized data
/// @{
#define __DATA_PERF __attribute__((section("data_perf")))
#define __DATA_FAST __attribute__((section("data_fast")))
#define __DATA_NORM __attribute__((section("data_norm")))
/// @}

/// @name Memory section types for uninitialized data
/// @{
#define __DATA_PERF_UI  __attribute__((section("data_perf_ui"),zero_init))
#define __DATA_FAST_UI  __attribute__((section("data_fast_ui"),zero_init))
#define __DATA_NORM_UI  __attribute__((section("data_norm_ui"),zero_init))
/// @}

/// @name Memory section types for shared uninitialized data
/// @{
#define __DATA_PERF_SH  __attribute__((used)) __attribute__((section("data_perf_sh"),zero_init))
#define __DATA_FAST_SH  __attribute__((used)) __attribute__((section("data_fast_sh"),zero_init))
#define __DATA_NORM_SH  __attribute__((used)) __attribute__((section("data_norm_sh"),zero_init))
#define __DATA_SSPM_SH  __attribute__((used)) __attribute__((section("data_sspm_ui"),zero_init))
#define __DATA_DRAM_SH  __attribute__((used)) __attribute__((section("data_dram_ui"),zero_init))
/// @}

//-----------------------------------------------------------------------------
//  Data type definitions:
//-----------------------------------------------------------------------------
typedef uint64_t Lba_t;             ///< Logical Block Address (always 512B unit)
typedef uint64_t Hlba_t;            ///< Host Logical Block Address (HOST Block size)

/// @brief Buffer descriptor structure
#pragma anon_unions // unnamed union
typedef struct
{
    void*    pBuffer;               ///< buffer pointer (start address)
    uint16_t ssdIndex;              ///< SSD index
    union
    {
        uint16_t allFlags;          ///< all flags
        struct
        {
            uint16_t flowCtrl   :4; ///< XferFlowCtrl_t flow control (non-streaming, IDS, streamingId1..7)
            uint16_t callBack   :1; ///< buffer fill completion call back
            uint16_t xferSetup  :2; ///< data xfer (see XferSetup_t)
            uint16_t cacheAloc  :1; ///< Cache Allocate (0=Non cache, 1=Cache allocate by CoreMain)
        };
    };
} HcmdQBufDesc_t;
COMPILE_ASSERT(sizeof(HcmdQBufDesc_t) == 2 * 4, "shall be 8 bytes size");

/// @paa info
typedef union
{
    U32 all32;
#if _NAND_ == NAND_3DTLCG2
    struct{
        U32 frag    :2;
        U32 plane   :2;
        U32 ch      :3;
        U32 page    :10;
        U32 block   :11;
        U32 ce      :3;
        U32 lun     :1;
    }b;
#elif _NAND_ == NAND_MLC
    struct{
        U32 frag    :2;
        U32 plane   :2;
        U32 ch      :3;
        U32 page    :8;
        U32 block   :13;
        U32 ce      :3;
        U32 lun     :1;
    }b;
#endif
    struct{
        U32 allfrag :4;
        U32 ch      :3;
        U32 row     :21;
        U32 LunCE   :4;
    }rough;
    struct{
        U32 frag    :2;
        U32 row     :30;
    }rough2;
    struct{
        U32 srcIdx  :16;
        U32 voltage :16;
    }rr;
}tPAA;
COMPILE_ASSERT(sizeof(tPAA) == 1 * 4, "shall be 4 bytes size");

/// @brief Phyiscal NAND Flash address
typedef struct
{
    uint8_t  ch;                 ///< channel number
    uint8_t  dev;                ///< device number
    uint16_t col;                ///< column address
    uint32_t row;                ///< row address
} Pna_t;

/// @brief Logical NAND Flash address
typedef struct
{
    uint8_t  ch;                 ///< channel number
    uint8_t  lane;               ///< lane number (including CE# & LUN#)
    uint16_t col;                ///< column address
    uint32_t row;                ///< row address
} Lna_t;

typedef struct
{
    U32 AgingDramTag;
    U32 AgingDramStatus;
    U32 AgingL2PPoint;
    U32 AgingDramTestSize;
} tDRAMTEST_INFO;
COMPILE_ASSERT((sizeof(tDRAMTEST_INFO) & 0x3) == 0, "please align to 4 bytes!");

typedef struct
{
    U32  NfifTag;
    U32  JEDECFail;
    U32  CRCFail;
    U8   FailCh;
    U8   FailCe;
    U8   FailLun;
    U8   FailPlane;
} tNAND_INFO;
COMPILE_ASSERT((sizeof(tNAND_INFO) & 0x3) == 0, "please align to 4 bytes!");

/// @brief PM Device lock status
typedef enum {
    cPmDeviceLock = 0,
    cPmDeviceBusy,
    cPmDeviceNone = 0x7FFFFFFF,
} PmDeviceLock_t;

/// @brief Power Control
typedef struct
{
    uint32_t            ReqLock;
    uint32_t            WakeUpBusy;
    uint32_t            ContextAddr;
    uint32_t            ProtectAddr;
    uint32_t            WakeupAddr;
    PmDeviceLock_t      LockStatus;
    DevicePowerMode_t   PowerMode;
} PowerControl_t;

typedef struct
{
    U16 nextIndex;
    U16 prevIndex;
    U32 startAddr;
    U32 endAddr;
}tSSD;

/// @brief Address range information structure (8 bytes)
#pragma anon_unions // unnamed union
typedef struct
{
    union
    {
        Lba_t  lba;                 ///< LBA (fixed block size, always 512B unit)
        Hlba_t hlba;                ///< host LBA
    };
    union
    {
        uint32_t allFlags;          ///< all flags
        struct
        {
            uint32_t nb                 :17;    ///< number of block (unit size depends on address type)
            uint32_t nsId0              :3;     ///< name space identifier (zero based)
            uint32_t fua                :1;     ///< Force Unit Access
            uint32_t limitedRetry       :1;     ///< Limited Retry
            uint32_t accessFrequency    :4;     ///< DSM Access Frequency
            uint32_t accessLatency      :2;     ///< DSM Access Latency
            uint32_t sequentialRequest  :1;     ///< indicates part of sequential access
            uint32_t incompressible     :1;     ///< indicates data is not compressible
            uint32_t reserved           :2;     ///< reserved
        };
    };
	uint32_t  rsvd;
} AddressInfo_t;
COMPILE_ASSERT(sizeof(AddressInfo_t) == 16, "shall be 16 bytes size");

typedef struct
{
    U32 laaStart;
    U32 laaEnd;
} FioTrimFmt_t;

/// @brief Host command transfer attribute
typedef union
{
    uint32_t all;
    struct
    {
        uint32_t cmdId      :16;    ///< command Id (unique identifier number from HAL)
        XferDir_t direction :1;     ///< data transfer direction (0=D2H, 1=H2D)
        uint32_t autoEoC    :1;     ///< auto EoC
        uint32_t hostIrq    :1;     ///< host interrupt request in case of autoEoC
        uint32_t mode       :2;     ///< Transfer mode (Protocol specific)
        uint32_t chained    :1;     ///< Transfer FIFO chained (HLD specific aggregation)
        uint32_t streamId   :8;     ///< Stream ID
        uint32_t nsid       :2;     ///< Reserve
    } b;
} XferAttr_t;

/// @brief NVMe Name space
typedef struct
{
    uint64_t NSZE;                  ///< namespace size    (host block unit)
    uint64_t NCAP;                  ///< namespace capacity (host block size unit)
    uint32_t mbu_start;             ///< MBU start address (same as laa start)
    uint32_t mbu_end;               ///< MBU end address (same as laa end, valid range till mbu_end - 1)
    uint8_t  LBAF;                  ///< LBA Format
    uint8_t  Attach;                ///< indicate namespace attached
    uint8_t  Create;                ///< indicate namespace created
    uint8_t  MS         :1;         ///< Meta data Settings
    uint8_t  PI         :3;         ///< Protection Information
    uint8_t  PIL        :1;         ///< Protection Information Location
    uint8_t  rsvd       :3;
    uint32_t hbu_shift;             ///< copy from lba format table (for performance purpose)
} NvmeIfMgrNamespace_t;

/// @brief NVMe I/F protocol and command service manager
typedef struct
{
    uint16_t dataSize;              ///< user data size per host block (byte size)
    uint16_t metaSize;              ///< meta data size per host block (byte size)
    uint8_t  rp;                    ///< relative performance
    uint8_t  mbu_mask;              ///< lba per mbu mask (MBU / LBA - 1)
    uint8_t  mbu_shift;             ///< lba to mbu shift (LOG2(MBU / LBA))
    uint8_t  hbu_shift;             ///< lba to hbu shfit (LOG2(HBU / LBA))
} NvmeIfMgrLbaFormat_t;
