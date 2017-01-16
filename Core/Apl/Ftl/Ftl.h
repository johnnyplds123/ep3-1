#pragma once
// ============================================================================
//
// ==        Copyright (c) LITEON Corporation  -  All rights reserved        ==
/*
    This computer program contains confidential and proprietary information,
    and may NOT be reproduced or transmitted, in whole or in part, in any form,
    or by any means electronic, mechanical, photo-optical, or otherwise, and
    may NOT be translated into another language without the express written
    permission from LITEON Corporation.
*/
// ==        Copyright (c) LITEON Corporation  -  All rights reserved        ==
//
//=============================================================================
//
//! @file
//! @brief Flash Translation Layer Manager API header
//!
//=============================================================================
//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "Fio.h"
#include "TcgCommon.h"

#if (_NAND_ == NAND_MLC)
    #include "Ftl_MLC.h"
#else
    #include "Ftl_3DTLCG2.h"
#endif

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
/// @brief: FTL Feature
#if CHK_FW_TYPE(FW_AGING)
    #define FTL_EBT_BUILD               FALSE
    #define FTL_NBT_BUILD               FALSE
    #define FTL_QBT_BUILD               FALSE
    #define FTL_PBT_BUILD               FALSE
    #define FTL_FAKE_QBT				FALSE
    #define FTL_P2L_LAST_PAGE           FALSE
    #define FTL_IDLE_FLUSH_DUMMY        FALSE
    #define FTL_CLOSE_HEADER_BLOCK      FALSE  //don't close open block have problem.
    #define FTL_SHUT_DOWN_CLOSE_BLK     FALSE
    #define FTL_SPOR_FLUSH_DUMMY		FALSE
    #define FTL_PARTIAL_LOAD_L2P		FALSE // Not ready
    #define FTL_TRIM_UPDATE             FALSE
#else
    #if (_NAND_ == NAND_MLC)
    #define FTL_EBT_BUILD               TRUE
    #define FTL_NBT_BUILD               TRUE
    #define FTL_QBT_BUILD               TRUE
    #define FTL_PBT_BUILD               TRUE
    #define FTL_FAKE_QBT				TRUE
    #define FTL_P2L_LAST_PAGE           TRUE
    #define FTL_IDLE_FLUSH_DUMMY        FALSE
    #define FTL_CLOSE_HEADER_BLOCK      TRUE  //don't close open block have problem.
    #define FTL_SHUT_DOWN_CLOSE_BLK     TRUE
    #define FTL_SPOR_FLUSH_DUMMY		FALSE
    #define FTL_PARTIAL_LOAD_L2P		FALSE // Not ready
    #define FTL_TRIM_UPDATE             TRUE
    #else
    #define FTL_EBT_BUILD               TRUE
    #define FTL_NBT_BUILD               TRUE
    #define FTL_QBT_BUILD               TRUE
    #define FTL_PBT_BUILD               TRUE
    #define FTL_FAKE_QBT				TRUE
    #define FTL_P2L_LAST_PAGE           TRUE
    #define FTL_IDLE_FLUSH_DUMMY        FALSE
    #define FTL_CLOSE_HEADER_BLOCK      TRUE  //don't close open block have problem.
    #define FTL_SHUT_DOWN_CLOSE_BLK     TRUE
    #define FTL_SPOR_FLUSH_DUMMY		FALSE
    #define FTL_PARTIAL_LOAD_L2P		FALSE
    #define FTL_TRIM_UPDATE             TRUE
    #endif
#endif
#define FTL_FUSH_SIZE               (16 * KBYTE)

//! @brief: FTL Debug
#if CHK_FW_TYPE(FW_AGING)
	#define FTL_DUMP_SBLK               FALSE
	#define FTL_DUMP_FTL                FALSE
	#define FTL_DUMP_BKLS               FALSE
	#define FTL_DUMP_INFO               FALSE
	#define FTL_DBG                     FALSE
	#define FTL_DRAM_CHEK               FALSE
	#define FTL_LP_P2L_CHEK             FALSE
	#define FTL_VAC_CHEK                FALSE
	#define FTL_SELF_TEST_SPOR          FALSE
#else
	#define FTL_DUMP_SBLK               FALSE
	#define FTL_DUMP_FTL                TRUE
	#define FTL_DUMP_BKLS               TRUE
	#define FTL_DUMP_INFO               TRUE
	#define FTL_DBG                     TRUE
	#define FTL_DRAM_CHEK               FALSE
	#define FTL_LP_P2L_CHEK             FALSE
	#define FTL_VAC_CHEK                FALSE
	#define FTL_SELF_TEST_SPOR          FALSE

#endif
//! @brief: FTL tag
#define FTL_BAD_BLOCK_TAG           ( 0xAADD )
#define FTL_LAST_PAGE_TAG           ( 0x4C415354 )
#define FTL_POR_PAGE_TAG            ( 0x504F5254 )
#define FTL_QBT_TABLE_TAG           ( 0x51425400 )
#define FTL_PBT_TABLE_TAG           ( 0x50425400 )
#define FTL_FQBT_TABLE_TAG          ( 0x46514200 )
#define FTL_GC_HEADER_TAG           ( 0x47434800 )
#define FTL_TABLE_TAG_MASK          ( 0x000000FF )

//! @brief: FTL meta info
#define FTL_META_HEADER_BITS        (LOG2(FTL_BLOCK_HEADER_MAX) + 1) // 2
#define FTL_META_HEADER_SHIFT       (32 - FTL_META_HEADER_BITS) // 30
#define FTL_META_BOOTGC_WRITE_BITS  (1)
#define FTL_META_BOOTGC_WRITE_SHIFT (FTL_META_HEADER_SHIFT - FTL_META_BOOTGC_WRITE_BITS) // 29
#define FTL_META_BOOTGC_WRITE_MUSK  (FTL_META_BOOTGC_WRITE_BITS) // 0x01
#define FTL_META_SN_BITS            (FTL_META_BOOTGC_WRITE_SHIFT) // 29
#define FTL_META_SN_MASK            (~(((1 << (FTL_META_HEADER_BITS + FTL_META_BOOTGC_WRITE_BITS)) - 1) << FTL_META_SN_BITS)) // ~(0xE0000000) = 0x1FFFFFFF

//! @brief: FTL L2P info
#define L2P_NUM_RAW                 ((IDEMA_GBYTE2LBU(gNfInfo.userCapacity) + HBU_PER_MBU_MASK) >> HBU_PER_MBU_SFT)

#define L2P_EXT_SPACER_START        L2P_NUM_RAW
#define L2P_EXT_NUM_SPACER          1

#define L2P_EXT_ZERO_START          (L2P_EXT_SPACER_START + L2P_EXT_NUM_SPACER)     // intend to add additional one laa for separation
#define L2P_EXT_NUM_ZERO            (LOCKING_RANGE_CNT + 1)                         // locking range count + 1 global range

#define L2P_EXT_NUM_USED            (L2P_EXT_NUM_SPACER + L2P_EXT_NUM_ZERO)
#define L2P_EXT_NUM_TOTAL           16384                                           // intend to reserve additional 16384 laas

COMPILE_ASSERT(L2P_EXT_NUM_USED <= L2P_EXT_NUM_TOTAL, "overflow!");

#if (_NAND_ == NAND_MLC)
    #define L2P_NUM                 ((((L2P_NUM_RAW + L2P_EXT_NUM_TOTAL) >> 17) + 1) << 17) //align with 256KB
#else
    #define L2P_NUM                 ((((L2P_NUM_RAW + L2P_EXT_NUM_TOTAL) >> 16) + 1) << 16) //align with 128KB
#endif
#define L2P_MASK                    ((U32)(L2P_NUM - 1))

//! @brief: FTL Quick Build Table
#define FTL_SUPER_BLOCK_SIZE        (gNfInfo.dataBytesPerPage * gNfInfo.pagesPerBlock * gNfInfo.planesPerLun * gNfInfo.totalLun) // 1TB device : 16KB*256*4*64 = 1GB
#define FTL_QBT_BLOCK_COUNT         ((L2P_NUM * 4)/(gNfInfo.dataBytesPerPage * gNfInfo.pagesPerBlock * gNfInfo.planesPerLun * gNfInfo.totalLun) + 1) // Total L2P bytes / bytes of a super block, store table after first round
#define FTL_QBT_BLOCK_RELEASE       (FTL_QBT_BLOCK_COUNT * 2)
#define FTL_QBT_SIZE_PER_FLUSH      (gNfInfo.numChannels * gNfInfo.dataBytesPerPage * gNfInfo.planesPerLun * gNfInfo.pagesPerProgUnit) // 512KB

#define FTL_MAX_QBT_CNT             ( FTL_QBT_BLOCK_RELEASE )

//! @brief: FTL support sysinfo block count
#define FTL_MAX_SYSINFO_CNT         ( 1 )

//@brief: FTL NAND info
#define FTL_DIE_SHIFT               ( gFtlNandInfo.die_shift )

#define FTL_FRAG_NUM                ( gEcuInfo.auPerPage )
#define FTL_FRAG_SHIFT              ( gFtlNandInfo.frag_shift )
#define FTL_FRAG_MASK               ( gFtlNandInfo.frag_mask )

#define FTL_PLANE_NUM               ( gNfInfo.planesPerLun )
#define FTL_PLANE_SHIFT             ( gFtlNandInfo.plane_shift )
#define FTL_PLANE_MASK              ( gFtlNandInfo.plane_mask )

#define FTL_CH_NUM                  ( gNfInfo.numChannels )
#define FTL_CH_SHIFT                ( gFtlNandInfo.ch_shift )
#define FTL_CH_MASK                 ( gFtlNandInfo.ch_mask )

#define FTL_CE_NUM                  ( gNfInfo.devicesPerChannel )
#define FTL_CE_SHIFT                ( gFtlNandInfo.ce_shift )
#define FTL_CE_MASK                 ( gFtlNandInfo.ce_mask )

#define FTL_LUN_NUM                 ( gNfInfo.lunsPerDevice )
#define FTL_LUN_SHIFT               ( gFtlNandInfo.lun_shift )
#define FTL_LUN_MASK                ( gFtlNandInfo.lun_mask )

#define FTL_BANK_NUM                ( gFtlNandInfo.bank_num )
#define FTL_BANK_SHIFT              ( gFtlNandInfo.bank_shift )
#define FTL_BANK_BIT_MASK           ( gFtlNandInfo.bank_bits_mask )

#define FTL_BITSPERCELL             (gNfInfo.bitsPerCell)

//! @brief: FTL support block count
#define FTL_SUPPORT_MAX_BLK         ( gNfInfo.blocksPerPlane )

//! @brief: FTL partial build table each blocks
#if (_GENERATION_ == GENERATION_CX2)
    #define FTL_PBT_PERIOD_BLK_MAX_COUNT        ( 16 )
#else
	#define FTL_PBT_PERIOD_BLK_MAX_COUNT        ( 32 )
#endif

#define FTL_PBT_PERIOD_BLK                      (FTL_PBT_PERIOD_BLK_MAX_COUNT / (MAX_SUPPORTED_PLANES >> FTL_PLANE_SHIFT))
#define FTL_PBT_PERIOD_PAGE                     (FTL_PBT_PERIOD_BLK / FTL_QBT_BLOCK_COUNT)

//! @brief: FTL last page info
#define LP_OFFSET_BLKTBL                        ( 0 )
#define LP_SIZE_BLKTBL                          ( sizeof(tFTL_BLOCK_NODE) * FTL_SUPPORT_MAX_BLK) // 12 * 1069 = 12828 Bytes

#define LP_OFFSET_BLKINFO                       ( LP_OFFSET_BLKTBL + LP_SIZE_BLKTBL ) // 12828 Bytes
#define LP_SIZE_BLKINFO                         ( sizeof(tFTL_BLOCK_INFO) ) // 44 Bytes

#define LP_OFFSET_HEADINFO                      ( LP_OFFSET_BLKINFO + LP_SIZE_BLKINFO ) // 12828 + 44 = 12872 Bytes
#define LP_SIZE_HEADINFO                        ( sizeof(tFTL_HEADER_INFO) * FTL_BLOCK_HEADER_MAX * FTL_GROUP_NUM) // 12 * 3 * 1 = 36 Bytes

#define LP_OFFSET_MGRVAR                        ( LP_OFFSET_HEADINFO + LP_SIZE_HEADINFO ) // 12872 + 36 = 12908 Bytes
#define LP_SIZE_MGRVAR                          ( sizeof(tFTL_MANAGER) ) // 32Bytes

#define LP_OFFSET_SERIAL_NUMBER                 ( LP_OFFSET_MGRVAR + LP_SIZE_MGRVAR ) // 12908 + 32 = 12940 Bytes
#define LP_SIZE_SERIAL_NUMBER                   ( sizeof(U32) * FTL_SUPPORT_MAX_BLK ) // 4 * 1069 = 4276Bytes

#define LP_SIZE_MAX                             ( LP_SIZE_BLKTBL + LP_SIZE_BLKINFO + LP_SIZE_HEADINFO + LP_SIZE_MGRVAR + LP_SIZE_SERIAL_NUMBER ) // 12940 + 4276 = 17216Bytes

#define LP_P2L_PP_SIZE                          (2) // Ping-Pong buffer size

//! @brief: FTL table info
#define FTL_GROUP_NUM                           ( MAX_SUPPORTED_FTL_GROUP )
#define FTL_LAA2GROUP_SHIFT                     ( 30 - 12 + LOG2(gNfInfo.userCapacity) - LOG2(FTL_GROUP_NUM) )//( 24 )
#define FTL_GROUP_MASK                          ((U32)( FTL_GROUP_NUM - 1 ))

//! @brief: FTL protect block push list
#define FTL_INSERT_PROTECT_AREA                 ( FTL_BLOCK_HEADER_MAX * FTL_GROUP_NUM * 2 )

//! @brief: FTL NBT4-1 get sn/header of status unknown block retry pages
#define FTL_NBT_GET_SN_HEADER_RPAGES            ( 2 )

//! @brief: FTL Max VAC
#define FTL_MAX_VAC                             (gFtlNandInfo.max_valid_count)

//! @brief: Change PBT period flush pages after trim
#define FTL_TRIM_PBT_PERIOD                     ( 2 )

//! @brief: FTL normal build timeout when spor
#if (_GENERATION_ == GENERATION_CX2)
    #define FTL_SPOR_TIMEOUT                    (10000)
#else
    #define FTL_SPOR_TIMEOUT                    (20000)
#endif

//! @brief: FTL update Trim L2P and VAC
#define FTL_LAA_BANK_PER_PAGE                   (FTL_QBT_SIZE_PER_FLUSH >> (LOG2(sizeof(U32))))
#define FTL_LAA_PER_PAGE                        (FTL_LAA_BANK_PER_PAGE << FTL_BANK_SHIFT)

//! @brief: FTL scan vac of blocks after NBT4-2
#define FTL_SCAN_BLOCK_VAC_TIME                 (FTL_SPOR_TIMEOUT - 3500) // reserve 3 sec for boot GC and other functions

//! @brief: Protect lower page of current header block when power on
#define FTL_PROTECT_CURRENT_HEADER_PAGE_BLOCKS  ( 1 + 1 ) // block count of Boot GC + First Current Block

//! @brief: Partial Build after POR
#define FTL_POR_PBT_ZONE_NUM                    (8)
#define FTL_POR_PBT_LAA_ZONE_CNT                ((gNfInfo.userCapacity << 18) >> LOG2(FTL_POR_PBT_ZONE_NUM))  // Capacity(1024) * GB / 4KB / 8 = 64M  LAA / zone

//! @brief: FTL max SSD table usage
#define FTL_MAX_CACHE_SSD_USAGE                 (FTL_FLUSH_BLT_P2L_PER_PAGE * MAX_SUPPORTED_DEVICES * MAX_SUPPORTED_LUNS)

//! @brief: FTL support direct trim update laa size threshold
#define FTL_DIRECT_TRIM_LAA_COUNT               (U32)(64 * (GBYTE / AU_SIZE))

//! @brief: FTL skip smaller VAC blocks for GC
#define FTL_SKIP_TRIM_BLOCKS_COUNT              (gGCInfo.op.opBlkCnt >> FTL_QBT_BLOCK_COUNT)

//! @brief: FTL limitation time of trim update
#define FTL_TRIM_UPDATE_TIMEOUT                 (75)//(40)

//-----------------------------------------------------------------------------
//  Structure definitions:
//-----------------------------------------------------------------------------
//! @brief: FTL block pools
enum
{
    FTL_BLOCK_POOL_USER=0,
    FTL_BLOCK_POOL_QBT,//=(FTL_BLOCK_POOL_USER+FTL_GROUP_NUM),
    FTL_BLOCK_POOL_PBT,
    FTL_BLOCK_POOL_GC,
    FTL_BLOCK_POOL_GCD,
    FTL_BLOCK_POOL_EMPTY,
    FTL_BLOCK_POOL_FREE,
    FTL_BLOCK_POOL_BAD,
    FTL_BLOCK_POOL_MAX, // 8
};

//! @brief: FTL block header
enum
{
    FTL_BLOCK_HEADER_GC=0,
    FTL_BLOCK_HEADER_HOST,
    FTL_BLOCK_HEADER_TABLE,
    FTL_BLOCK_HEADER_MAX,
};

//! @brief: FTL scan L2P stage
enum
{
    FTL_SCAN_L2P_STAGE_GC=0,
    FTL_SCAN_L2P_STAGE_HOST,
    FTL_SCAN_L2P_STAGE_HEADER,
    FTL_SCAN_L2P_STAGE_MAX,
};

//! @brief: FTL initial variables
enum
{
    FTL_INITIAL_PREFORMAT=0,
    FTL_INITIAL_POWER_ON,
    FTL_INITIAL_MAX,
};

//! @brief: FTL build table type
enum
{
    FTL_BUILD_TYPE_NONE=0,
    FTL_BUILD_TYPE_QBT,
    FTL_BUILD_TYPE_PBT,
    FTL_BUILD_TYPE_MAX,
};

//! @brief: FTL scan page with aux or not
enum
{
    FTL_SCAN_PAGE_NONE=0,
    FTL_SCAN_PAGE_AUX,
    FTL_SCAN_PAGE_MAX,
};

//@ brief: FTL block list sorting rule
enum
{
    FTL_SORT_NONE=0,
    FTL_SORT_BY_EC,
    FTL_SORT_BY_SN,
    FTL_SORT_BY_HEAD,
    FTL_SORT_MAX,
};

//! @brief: FTL bank status
enum
{
    FTL_BANK_STATUS_NONE=0,
    FTL_BANK_STATUS_RETRY,
    FTL_BANK_STATUS_EMPTY,
    FTL_BANK_STATUS_MAX,
};

//! @brief: FTL scan SN type
enum
{
    FTL_SN_TYPE_EMPTY=0,
    FTL_SN_TYPE_USER,
    FTL_SN_TYPE_PBT,
    FTL_SN_TYPE_QBT,
    FTL_SN_TYPE_FQBT,
    FTL_SN_TYPE_OPENHOST,
    FTL_SN_TYPE_BOOTGC,
    FTL_SN_TYPE_MAX,
};

//! @brief: FTL self test SPOR type
enum
{
    FTL_SELF_TEST_T0=0, // un-close user block and no save QBT
    FTL_SELF_TEST_T1,   // close user block and un-close QBT
    FTL_SELF_TEST_MAX,  // close user block and close QBT
};

//! brief: FTL build table states
typedef enum
{
    FTL_BUILD_TABLE_ERASE = 0,
    FTL_BUILD_TABLE_NORMAL,
    FTL_BUILD_TABLE_READ_SN,
    FTL_BUILD_TABLE_READ_BLIST,
    FTL_BUILD_TABLE_REBUILD_BLIST,
    FTL_BUILD_TABLE_READ_L2P,
    FTL_BUILD_TABLE_READ_P2L,
    FTL_BUILD_TABLE_READ_OPENBK,
    FTL_BUILD_TABLE_COUNT,
    FTL_BUILD_TABLE_NULL = 0x7FFFFFFF
}tFTL_BUILD_TABLE_STATES;

//! @brief: FTL open block empty type
enum
{
    FTL_OPEN_BLOCK_BANK_EMPTY=0,        // bank 0 empty
    FTL_OPEN_BLOCK_BANK_PAGE_EMPTY,     // bank 0 and page 0 empty
    FTL_OPEN_BLOCK_EMPTY_MAX,
};

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define FTL_GET_DIE_NUM(lun,ce,ch)              ((lun << gNfInfo.bitsChDev) | (ce << FTL_CH_SHIFT) | ch)
#define FTL_GET_SBLK_IDX(plane,blk,die)         (((U32)(plane * gNfInfo.blocksPerPlane + blk) << FTL_DIE_SHIFT) | die)
#define FTL_GET_LP_PP_IDX(lun, ce, hd, gp)      (gFtlMgr.LastPageBufIdx[(lun << FTL_CE_SHIFT) | ce][hd][gp])

#define FTL_CURRENT_HEADER_NEW(header, group)   ((gFtlCurrHeader[header][group].block == WORD_MASK) || ((gFtlCurrHeader[header][group].page == gNfInfo.pagesPerBlock) && (gFtlCurrHeader[header][group].bank == 0)))
#define FTL_GET_CURRENT_BANK(header, group)     (gFtlCurrHeader[header][group].bank)

#if (FTL_P2L_LAST_PAGE == TRUE)
    #define FTL_IS_LAST_PAGE(header, group)     (gFtlCurrHeader[header][group].page == FTL_NAND_LAST_PAGE)
#else
    #define FTL_IS_LAST_PAGE(header, group)     (FALSE)
#endif

#if (FTL_PBT_BUILD == TRUE)
    #define FTL_IS_PBT_PAGE()                   (gFtlPbtInfo.pageCnt >= FTL_PBT_PERIOD_BLK)
#else
    #define FTL_IS_PBT_PAGE()                   (FALSE)
#endif

#define FTL_GET_PBT_PG_BN_IDX(ce,lun,page)      ((((((gFtlPbtInfo.loopCnt-1) % FTL_QBT_BLOCK_COUNT) * (gNfInfo.pagesPerBlock / gNfInfo.pagesPerProgUnit)) + (page / gNfInfo.pagesPerProgUnit)) << FTL_BANK_SHIFT) + (lun << FTL_CE_SHIFT) + ce)

#if (FTL_DBG == TRUE)
    #define D_PRINTF_FTL(F, args...)            D_PRINTF("FTL:"F, ##args)
#else
    #define D_PRINTF_FTL(F, args...)            {}
#endif

//! @brief: Trim Info Macro
#define FTL_TRIM_SET_BMP(laa)                    (gFtlTrimInfo.trimBMP[laa >> 5] |= BIT(laa & 31))
#define FTL_TRIM_CHK_BMP(laa)                   ( gFtlTrimInfo.trimBMPcnt ? (gFtlTrimInfo.trimBMP[laa >> 5] & BIT(laa & 31)) : 0)
#define FTL_TRIM_CLR_BMP(laa)                   { if (gFtlTrimInfo.trimBMP[laa >> 5] & BIT(laa & 31)) {gFtlTrimInfo.trimBMP[laa >> 5] &= ~BIT(laa & 31); gFtlTrimInfo.trimBMPcnt--;} }
#define FTL_TRIM_SET_PBT_PG(bkpg)               (gFtlTrimInfo.trimPbtPG[bkpg >> 5] |=  BIT(bkpg & 31))
#define FTL_TRIM_CHK_PBT_PG(bkpg)               (gFtlTrimInfo.trimPbtPG[bkpg >> 5] &   BIT(bkpg & 31))
#define FTL_TRIM_CLR_PBT_PG(bkpg)               (gFtlTrimInfo.trimPbtPG[bkpg >> 5] &= ~BIT(bkpg & 31))

//! @brief: Set serail number to Job
#define FTL_JOB_SN_SET(header, bootGC, sn)      ((sn & FTL_META_SN_MASK) | (bootGC << FTL_META_BOOTGC_WRITE_SHIFT) | (header << FTL_META_HEADER_SHIFT))

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
//! @brief: FTL nand information
typedef struct
{
    U32 max_valid_count;
    U32 bank_bits_mask;

    U8  ch_shift;
    U8  ch_mask;

    U8  ce_shift;
    U8  ce_mask;

    U8  lun_shift;
    U8  lun_mask;

    U8  bank_num;
    U8  bank_shift;
    U8  bank_mask;

    U8  die_shift;

    U8  plane_shift;
    U8  plane_mask;
    U8  frag_shift;
    U8  frag_mask;

    U8  pages_shift;
    U8  reserve[1];
} tFTL_NAND_INFO; // 4 + 4 + 1*15 + 1*1 = 24
COMPILE_ASSERT(sizeof(tFTL_NAND_INFO)==24, "size must be 24 bytes");

//! @brief: FTL valid count status
typedef union
{
    U32                 all32;
    struct {
        U32             vac      : 24;//Trim + Over Write
        U32             status   : 4;
        U32             host     : 1;
        U32             bad      : 1;
        U32             rdd      : 1; //for Rdd and only move data no markbad case....
        U32             reserve  : 1;
    } b;
} tFTL_VALID_STATUS;
COMPILE_ASSERT(sizeof(tFTL_VALID_STATUS)==4, "size must be 4 bytes");

//! @brief: Ftl Block List Table
typedef struct
{
    tFTL_VALID_STATUS   vs;
    U16                 eraseCnt;
    U16                 block;
}tFTL_BLOCK_NODE; // 4 + 2 + 2 = 8 Bytes
COMPILE_ASSERT(sizeof(tFTL_BLOCK_NODE)==8, "size must be 8 bytes");

//@brief: Ftl Block List Info
typedef struct
{
    U16                 head[FTL_BLOCK_POOL_MAX];
    U16                 tail[FTL_BLOCK_POOL_MAX];
    U16                 blkCnt[FTL_BLOCK_POOL_MAX];
    U8                  reserve[4];
}tFTL_BLOCK_INFO; // 2*8 + 2*8 + 2*8 + 4 = 52 Bytes
COMPILE_ASSERT((sizeof(tFTL_BLOCK_INFO)%4) == 0, "size must be 52 bytes");

//! @brief: Ftl Current Header
typedef struct
{
    U32                 sn;
    U16                 bank;
    U16                 block;
    U16                 page;
    U8                  reserve[2];
}tFTL_HEADER_INFO; // 4 + 2 + 2 + 2 + 2 = 12 Bytes
COMPILE_ASSERT(sizeof(tFTL_HEADER_INFO)==12, "size must be 12 bytes");

//! @brief: Ftl Maneger
typedef struct
{
    U32                 SerialNumber;
    U32                 TableSN;
    U32                 AllEraseCnt;
    U16                 forceErase;
    U16                 QbtBackup;
    U8                  LastPageBufIdx[MAX_SUPPORTED_CHANNELS][FTL_SCAN_L2P_STAGE_HEADER][FTL_GROUP_NUM];//max_ce
    U8                  flushDummy;
    U8                  ReleaseBlk;
    U8                  ReleaseGCBlk;
    U8                  reserve[1];
}tFTL_MANAGER; // 4 + 4 + 4 + 2 + 2 + 1*8*2*1 + 1 + 1 + 1 + 1 = 36 Bytes
COMPILE_ASSERT((sizeof(tFTL_MANAGER)%4) == 0, "size must be 36 bytes");

//! @brief: FTL Quick Build Table Info
typedef struct
{
    U32             sn;
    U16             block;
    U8              reserved[2];
}tFTL_QBT_INFO; // 4 + 2 + 2 = 8 Bytes
COMPILE_ASSERT(sizeof(tFTL_QBT_INFO)==8, "size must be 8 bytes");

//! @brief: FTL Partial Build Table Info
typedef struct
{
    U32             sn;
    U16             block;
    U16             loopCnt;
    U16             pageCnt;
    U8              reserve[2];
}tFTL_PBT_INFO; // 4 + 2 + 2 + 2 + 2 = 12 Bytes
COMPILE_ASSERT(sizeof(tFTL_PBT_INFO)==12, "size must be 12 bytes");

typedef struct
{
    U16*            rmpSuperBlkTbl;
    U8*             rmpBlkBMP;
}tFTL_RMP_TABLE; // 4 + 4 = 8 Bytes
COMPILE_ASSERT(sizeof(tFTL_RMP_TABLE)==8, "size must be 8 bytes");

typedef struct
{
    //U32*            p2lBuf;
    U32*            lastPageBufFtlP2L;
    U8*             lastPageBufFtlInfo;
}tFTL_LAST_PAGE_BUFF; // 4 + 4 + 4 = 12 Bytes
COMPILE_ASSERT((sizeof(tFTL_LAST_PAGE_BUFF) & 0x03) == 0, "size must be 12 bytes");

typedef struct
{
    U32*            trimBMP;            // bitmap for trim LAA
    U32*            trimPbtPG;          // update L2P per PBT page
    U8*             trimBlocks;         // trim LAA in #block and can't move before PBT close

    U32             trimBMPcnt;         // count of trim LAA
    U32             trimBlockcnt;       // count of unsave blocks
    U16             trimSkipBlockcnt;   // count of skipping trim blocks
    U8              trimUpdate;         // update L2P with trim bitmap
    U8              reserve[5];
}tFTL_TRIM_INFO; // 4 + 4 + 4 + 4 + 4 + 2 + 1 + 1*5 = 28 Bytes
COMPILE_ASSERT(sizeof(tFTL_TRIM_INFO)==28, "size must be 28 bytes");

typedef struct
{
    U8              existQBT;
    U8              existPBT;
    U8              existFQBT;
    U8              reserve;
}tFTL_NBT_TYPE;
COMPILE_ASSERT(sizeof(tFTL_NBT_TYPE)==4, "size must be 4 bytes");

typedef struct
{
    U8*             zoneBMP;
    U8              finishBuild;
    U8              zoneCnt;
    U8              reserve[2];
}tFTL_POR_PBT_INFO; // 4 + 1 + 1 + 1*2 = 8 Bytes
COMPILE_ASSERT(sizeof(tFTL_POR_PBT_INFO)==8, "size must be 8 bytes");

typedef struct
{
    tFTL_BUILD_TABLE_STATES buildState;        // build table state
    tFTL_BUILD_TABLE_STATES prevState;         // previous table state
    U16                     readBlistBlock;    // read block list
    U8                      reserve[2];
}tFTL_BUILD_TABLE_INFO; // 4 + 4 + 2 + 1*2 = 12 Bytes
COMPILE_ASSERT(sizeof(tFTL_BUILD_TABLE_INFO)==12, "size must be 12 bytes");

typedef struct
{
    U16     openBlkCount;
    U8      flag;
    U8      scantime_sec;
}tFTL_SPOR_INFO; //2 + 1 + 1 = 4 Bytes
COMPILE_ASSERT(sizeof(tFTL_SPOR_INFO)==4, "size must be 4 bytes");

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

extern tFTL_BLOCK_INFO      gFtlBlkListInfo; // 44B
extern tFTL_HEADER_INFO     gFtlCurrHeader[FTL_BLOCK_HEADER_MAX][FTL_GROUP_NUM]; // 12 * 3 * 1 = 36B
extern tFTL_MANAGER         gFtlMgr;        // 32B
extern tFTL_PBT_INFO        gFtlPbtInfo;    // 12B
extern tFTL_RMP_TABLE       gFtlRemap;      // 8B
extern tFTL_LAST_PAGE_BUFF  gFtlLastPageBuf;// 12B
extern tFTL_TRIM_INFO       gFtlTrimInfo;   // 24B
extern tFTL_NBT_TYPE        gFtlNbtType;    // 4B
extern tFTL_POR_PBT_INFO    gFtlPorPbtInfo; // 12B
extern tFTL_NAND_INFO       gFtlNandInfo;   // 16B
extern tFTL_SPOR_INFO       gFtlSporInfo;   // 4B

extern tFIO_JOB**           gFtlBTJob;//max_ce*max_plane
extern tFTL_BLOCK_NODE*     gFtlBlkListTbl;
extern tFTL_BLOCK_NODE*     gFtlPBTBlkListTbl;
extern tFTL_QBT_INFO*       gFtlQbtInfo;
extern tSSD*                gFtlBtSSDHead;
extern tSSD*                gFtlBtSSDTail;
extern tPAA*                gFtlL2PTbl;
extern U32*                 gFtlFastVacBuf;
extern U32*                 gFtlQbtSn;
extern U32*                 gFtlSNTable;
extern U32*                 gFtlHeadReadCnt;
extern U16*                 gFtlFreeIdx;
extern U8*                  gFtlNbtEmptyBlk;
extern U8*                  gFtlLastPageP2LIdx;

extern U32                  gFtlLastPageP2LSize;
extern U32                  gFtlTrimRatio;
extern U8                   gFtlPowerOn;
extern U32                  gExtLaaStart;

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
void     FTL_CheckUserBlocks(void);
void     FTL_SaveFakeQBT(void);
void     FTL_PurgeCurrentHeader2Free(void);
void     FTL_SetExternLAA(void);
U8       FTL_RemapBadBlock(U8 die, U8 plane, U16 logBlk);
void     FTL_DumpInfo(void);
void     FTL_DumpRemap(U8 buildBitmap);
void     FTL_DumpL2P(U32 laa, U32 count);
void     FTL_DumpVAC(void);
void     FTL_DumpTrimInfo(void);
Error_t  FTL_OneTimeInit(InitBootMode_t initMode);
void     FTL_ClearHeaderBlock(U8 header,U8 group);
U16      FTL_GetRemapBlock(U8 ch, U8 ce, U8 lun, U8 plane, U16 block);
U16      FTL_BlockPopHead(U16 pool);
void     FTL_BlockPopList(U16 pool, U16 block);
void     FTL_BlockPushList(U16 pool, U16 block, U16 sortRule);
void     FTL_BlockPopPushList(U16 pool, U16 block, U16 sortRule);
void     FTL_PurgePool2Free(U16 pool);
void     FTL_PurgePbt2Free(void);
U8       FTL_QuickOrPartialBuildL2P(U32 buildType, U8 zone);
U8       FTL_DecreaseVC(tPAA paa);
void     FTL_ReleaseEmptyPool(void);
void     FTL_CopyFtlInfoLastPageBuf(void);
void     FTL_LastPageUpdate(tFIO_JOB* pJob);
void     FTL_DirectTrimUpdate(U32 laaS,U32 laaE);
void     FTL_PbtTrimUpdate(U16 page, U8 lun, U8 ce);
void     FTL_PbtPageUpdate(tFIO_JOB* pJob);
void     FTL_EraseUpdate(tFIO_JOB* pJob);
U8       FTL_BuildL2PZone(U8 zoneStart, U8 zoneEnd);
void     FTL_SaveQuickBuildTable(void);
void     FTL_EraseBankBlocks(U16 block);
void     FTL_FlushDummyCloseBlock(U8 header, U8 group, U16 dmpage);
void     FTL_Preformat(void);

//-----------------------------------------------------------------------------
//  Inline function reference
//-----------------------------------------------------------------------------
//-------------------------------------------------------------------
// Function     : __inline U16 FTL_GetRemapSuperBlock(U8 ch, U8 ce, U8 lun, U8 plane, U16 block)
// Description  : Get remap block
// Input        : U8 ch, U8 ce, U8 lun, U8 plane, U16 block
// return       : U16 block
//-------------------------------------------------------------------
static __inline U16 FTL_FastGetRMPBlk(U8 ch, U8 ce, U8 lun, U8 plane, U16 block)
{
    return (gFtlRemap.rmpBlkBMP[block]) ? FTL_GetRemapBlock(ch, ce, lun, plane, block) : block;
}

//-------------------------------------------------------------------
// Function     : tPAA FTL_PhyAddrSearch(U32 laa)
// Description  :
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static __inline tPAA FTL_PhyAddrSearch(U32 laa)
{
    if (laa > L2P_NUM)
    {
        D_PRINTF("L[%x]P[%x]\n", laa, gFtlL2PTbl[laa].all32);
        ASSERT(FALSE);
    }

    if (FTL_TRIM_CHK_BMP(laa))
    {
        tPAA dummyPaa;
        dummyPaa.all32 = DWORD_MASK;
        return dummyPaa;
    }
    else
    {
        return (gFtlL2PTbl[laa]);
    }
}

//-------------------------------------------------------------------
// Function     : __inline void FTL_L2PBurstCopy(tPAA* src, U32 laa, U16 cnt)
// Description  : burst copy L2P table
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
__inline void FTL_L2PBurstCopy(U32* desc, U32 startLaa, U16 cnt)
{
    //D_PRINTF("PAS_B:[%x]CH[%d]CE[%d]LN[%d]PN[%d]BK[%4x]PG[%2x]FG[%d] CNT[%d]\n", startLaa, gFtlL2PTbl[startLaa].b.ch, gFtlL2PTbl[startLaa].b.ce, gFtlL2PTbl[startLaa].b.lun, gFtlL2PTbl[startLaa].b.plane, gFtlL2PTbl[startLaa].b.block, gFtlL2PTbl[startLaa].b.page, gFtlL2PTbl[startLaa].b.frag, cnt);
    MemCopyNU32(desc, (U32*)&gFtlL2PTbl[startLaa], sizeof(tPAA) * cnt);
}

//-------------------------------------------------------------------
// Function     : __inline void FTL_PorPbtLaaZone(U32 laaS, U32 laaE)
// Description  : burst copy L2P table
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static __inline void FTL_PorPbtLaaZone(U32 laaS, U32 laaE)
{
    U8  zoneS, zoneE;

    zoneS = laaS / FTL_POR_PBT_LAA_ZONE_CNT;
    zoneE = (laaE / FTL_POR_PBT_LAA_ZONE_CNT) + 1;

    if (zoneE > FTL_POR_PBT_ZONE_NUM) zoneE = FTL_POR_PBT_ZONE_NUM;

    D_PRINTF_FTL("BuildL2P LAA[%x-%x]Zone[%d-%d]\n", laaS, laaE, zoneS, zoneE);

    FTL_BuildL2PZone(zoneS, zoneE);
}

//-------------------------------------------------------------------
// Function     : __inline U8 FTL_TestSpor(U32 sn)
// Description  : self test type
// Input        : N/A
// return       : N/A
//-------------------------------------------------------------------
static __inline U8 FTL_TestSpor(U32 sn)
{
#if (FTL_SELF_TEST_SPOR == TRUE)
    return (sn % FTL_SELF_TEST_MAX);
#else
    return (FTL_SELF_TEST_MAX);
#endif
}
