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
//! \file
//! @brief DMAC(Direct Memory Access Controller) API codes
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "MemAlloc.h"
#include "HalDmac.h"
#include "RegDmac.h"

#include "HalTimer.h"

#if _CPU_!=CPU_ID1
#include "BcmClient.h"
#endif

#if _TCG_!=TCG_NONE
#include "TcgCommon.h"
#endif

#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define DMAC_BCRC_SIZE  4
#define DMAC_LBA_SIZE   8
#define DMAC_MPECC_SIZE 12
#define DMAC_MPECC_PAD  4

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define GET_AXI_ADDR_FROM_PTR(X)    ((uint32_t)(X))      ///< convert pointer to AXI address

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief SSD structure for DMAC
typedef struct
{
    uint32_t nextIndex;
    uint32_t startAddr;
    uint32_t endAddr;
} DmacSsd_t;

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
#if _CPU_!=CPU_ID1
Error_t HAL_MRE_SecCopy(U32 destAddr, U32 srcAddr, U32 byteSize, U32 range);
#endif
//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------
#if _TCG_!=TCG_NONE
  #if _CPU_ == CPU_ID0
    // ...
  #elif _CPU_ == CPU_ID2
    extern sEnabledLockingTable *pLockingRangeTable;
  #endif
#endif

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    One-time initialization of DMAC HW\n

    @param[in]  initMode        initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalDmac_OneTimeInit(InitBootMode_t initMode)
{
    return (cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Wait for DMAC operation done\n

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalDmac_WaitDone(void)
{
    // DmacStat_t status;
    DmacStat_t status;

    // init error bits
    status.all = 0;
    status.b.DMAC_CMP_FAIL                = 1;         ///<BIT [1] DMAC Compare Fail.
    status.b.SDRAM_TST_FAIL               = 1;         ///<BIT [2] SDRAM Test Fail.
    status.b.CRC_ERR                      = 3;         ///<BIT [5:4] CRC Error.
    status.b.REF_TAG_MISCMP               = 1;         ///<BIT [8] Reference Tag Miscompare.
    status.b.APP_TAG_MISCMP               = 1;         ///<BIT [9] Application Tag Miscompare.
    status.b.GRD_MISCMP                   = 1;         ///<BIT [10] Guard Miscompare.
    status.b.RD_CH_1_MPCC_ERR             = 1;         ///<BIT [11] Read Channel 1 MPECC Error.
    status.b.RD_CH_1_LBA_ERR              = 1;         ///<BIT [12] Read Channel 1 LBA Error.
    status.b.RD_CH_2_MPCC_ERR             = 1;         ///<BIT [13] Read Channel 2 MPECC Error.
    status.b.RD_CH_2_LBA_ERR              = 1;         ///<BIT [14] Read Channel 2 LBA Error.
    status.b.END_TO_END_FAIL              = 1;         ///<BIT [15] End-to-End Fail.
    status.b.AXI_BRDG_WR_TMT_ERR          = 1;         ///<BIT [16] AXI Bridge Write Timeout Error.
    status.b.AXI_BRDG_WR_FIFO_ERR         = 1;         ///<BIT [17] AXI Bridge Write FIFO Error.
    status.b.AXI_BRDG_ABNRML_RESP_RCVD    = 1;         ///<BIT [18] AXI Bridge Abnormal Response Received.
    status.b.AXI_BRDG_RD_TMT_ERR          = 1;         ///<BIT [20] AXI Bridge Read Timeout Error.
    status.b.AXI_BRDG_RD_ABNRML_RESP_RCVD = 1;         ///<BIT [21] AXI Bridge Read Abnormal Response Received.
    status.b.AXI_BRDG_RD_BRST_LEN_ERR     = 1;         ///<BIT [22] AXI Bridge Read Burst Length Error.
    status.b.WR_CH_FIFO_CRC_ERR           = 1;         ///<BIT [24] Write Channel FIFO CRC Error.
    status.b.RD_CH_1_FIFO_CRC_ERR         = 1;         ///<BIT [25] Read Channel 1 FIFO CRC Error.
    status.b.RD_CH_2_FIFO_CRC_ERR         = 1;         ///<BIT [26] Read Channel 2 FIFO CRC Error.
    status.b.SSD_PAR_ERR_DET              = 1;         ///<BIT [27] Sub Segment Descriptor Parity Error Detected.

    // wait for DMA complete
    while (rDmac.dmacStat.b.OP_CMPLT==0);

    // any error?
    status.all = status.all & rDmac.dmacStat.all;

    if (status.all==0)          return (cEcNoError);
    if (status.b.DMAC_CMP_FAIL) return (cEcDmacCompare);

    return (cEcDmacGeneral);
}


//-----------------------------------------------------------------------------
/**
    DMAC setup for simple linear memory operation

    @param[in]  byteSize    byte size to operation (must be multiple of 4)

    @return error code
**/
//-----------------------------------------------------------------------------
static void SetupDmacForMemoryOp(uint32_t buf1Addr, uint32_t buf2Addr, uint32_t buf3Addr, uint32_t byteSize)
{
    Cfg0_t cfg0;

    // setup buffer 1
    rDmac.LNKD_LIST_1_SUB_SEG_DSCRPTR_TBL_BASE_ADDR = buf1Addr;                   // buffer 1 address
    rDmac.LNKD_LIST_1_STRT_IDX_OR_ADDR              = buf1Addr;                   // buffer 1 reload address
    rDmac.LNKD_LIST_1_SSD_STRT_LBA                  = buf1Addr + byteSize - 4;    // buffer 1 limit address

    // setup buffer 2
    rDmac.LNKD_LIST_2_SUB_SEG_DSCRPTR_TBL_BASE_ADDR = buf2Addr;                   // buffer 2 address
    rDmac.LNKD_LIST_2_SUB_SEG_DSCRPTR_START_IX_ADDR_OR_RLD_ADDR = buf2Addr;       // buffer 2 reload address
    rDmac.LNKD_LIST_2_SSD_STRT_LBA                  = buf2Addr + byteSize - 4;    // buffer 2 limit address

    // setup buffer 3
    rDmac.LNKD_LIST_3_SUB_SEG_DSCRPTR_TBL_BASE_ADDR = buf3Addr;                   // buffer 3 address
    rDmac.SUB_SEG_DSCRPTR_3_STRT_ADDR_OR_IDX        = buf3Addr;                   // buffer 3 reload address
    rDmac.LNKD_LIST_3_SSD_STRT_LBA                  = buf3Addr + byteSize - 4;    // buffer 3 limit address

    // setup size
    rDmac.blkSize.all = byteSize;
    rDmac.BLK_CNT = 1;

    cfg0.all = 0;
    cfg0.b.DATA_FMT = 0;             // Data Format, 0 = Data Only
    cfg0.b.LNKD_LIST_PAR_DIS = 1;    // Linked List Parity Disable.
    cfg0.b.RD_CH_1_FXD_ADDR_MD = 1;  // Read Channel 1 Fixed Address Mode.
    cfg0.b.RD_CH_2_FXD_ADDR_MD = 1;  // Read Channel 2 Fixed Address Mode
    cfg0.b.WR_CH_FXD_ADDR_MD  = 1;   // Write Channel Fixed Address Mode.

    // setup command and configuration
    rDmac.cfg0.all = cfg0.all;
    rDmac.cmd23.all = 0;
}

//-----------------------------------------------------------------------------
/**
    fill a memory region with 32bit pattern\n

    @param[in]  destAddr    destination address (must be 4-byte aligned)
    @param[in]  pattern     32bit pattern to fill
    @param[in]  byteSize    byte size to fill (must be multiple of 4)

    @return error code

    @remark Caller must check DMAC done because this code just starts DMAC.
**/
//-----------------------------------------------------------------------------
Error_t HalDmac_StartFill(uint32_t destAddr, uint32_t pattern, uint32_t byteSize)
{
    Cmd01_t cmd01;    /// DMAC command 0

    if (!TEST_U32_ALIGNED(destAddr)) return cEcNotAligned;   // not aligned
    if (!TEST_U32_ALIGNED(byteSize)) return cEcNotAligned;   // not aligned

    // setup 32bit pattern
    rDmac.WR_BUF_LGCL_BLK_ADDR_0 = pattern;

    SetupDmacForMemoryOp(0, 0, destAddr, byteSize); // buffer 3 is destination

    // setup command0 register value
    cmd01.all = 0;
    cmd01.b.SDRAM_TST_WR_EN = 1;         // SDRAM Test Write Enable.
    cmd01.b.SDRAM_TST_LBA_INC_DIS = 1;   // SDRAM Test LBA Increment Disable
    cmd01.b.SDRAM_TST_PTRN = 0;          // SDRAM Test Pattern, 0 = Data (Sequential or Fix) without CRC.
    cmd01.b.SDRAM_TST_LBA_CRC_FMT = 2;   // SDRAM Test LBA/CRC Format, 2 = The XOR CRC accumulator is seeded with FFFFFFFFh.
    cmd01.b.SDRAM_TST_EN = 1;            // SDRAM Test Enable
    cmd01.b.DIS_NEXT_ADDR_CMP = 1;       // Disable Next Address Compare.
    cmd01.b.IGNR_MISCMP = 1;             // Ignore Miscompare.
    cmd01.b.FUNC_SEL = 4;                // Function Select, 4 = DMA
    cmd01.b.DMAC_SDRAM_TST_DMA_EN = 1;   // DMAC/SDRAM Test DMA Enable.
    cmd01.b.DMAC_SDRAM_TST_STRT_CMD = 1; // DMAC/SDRAM Test Start Command.

    // Start DMA controller
    rDmac.cmd01.all = cmd01.all;

    return (cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    fill a memory region with pseudo random pattern\n

    @param[in]  destAddr    destination address (must be 4-byte aligned)
    @param[in]  seed        32bit random seed
    @param[in]  byteSize    byte size to fill (must be multiple of 4)

    @return error code

    @remark Caller must check DMAC done because this code just starts DMAC.
**/
//-----------------------------------------------------------------------------
Error_t HalDmac_StartRandomFill(uint32_t destAddr, uint32_t seed, uint32_t byteSize)
{
    Cmd01_t cmd01;    /// DMAC command 0

    if (!TEST_U32_ALIGNED(destAddr)) return cEcNotAligned;   // not aligned
    if (!TEST_U32_ALIGNED(byteSize)) return cEcNotAligned;   // not aligned

    // setup seed
    rDmac.WR_BUF_LGCL_BLK_ADDR_0 = seed;

    SetupDmacForMemoryOp(0, 0, destAddr, byteSize); // buffer 3 is destination

    // setup command0 register value
    cmd01.all = 0;
    cmd01.b.SDRAM_TST_WR_EN = 1;         // SDRAM Test Write Enable.
    cmd01.b.SDRAM_TST_LBA_INC_DIS = 0;   // SDRAM Test LBA Increment Enable
    cmd01.b.SDRAM_TST_PTRN = 2;          // SDRAM Test Pattern, 2=Random data (From CRC accumulator) without CRC.
    cmd01.b.SDRAM_TST_LBA_CRC_FMT = 0;   // SDRAM Test LBA/CRC Format, 0=LBA seed.
    cmd01.b.SDRAM_TST_EN = 1;            // SDRAM Test Enable
    cmd01.b.DIS_NEXT_ADDR_CMP = 1;       // Disable Next Address Compare.
    cmd01.b.IGNR_MISCMP = 1;             // Ignore Miscompare.
    cmd01.b.FUNC_SEL = 0;                // Function Select, 0=XOR
    cmd01.b.DMAC_SDRAM_TST_DMA_EN = 1;   // DMAC/SDRAM Test DMA Enable.
    cmd01.b.DMAC_SDRAM_TST_STRT_CMD = 1; // DMAC/SDRAM Test Start Command.

    // Start DMA controller
    rDmac.cmd01.all = cmd01.all;

    return (cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    copies n bytes from memory area src to memory area dest\n

    @param[in]  destAddr     destination address (must be 4-byte aligned)
    @param[in]  srcAddr      source address (must be 4-byte aligned)
    @param[in]  byteSize     byte size to copy (must be multiple of 4)

    @return error code

    @remark Caller must check DMAC done because this code just starts DMAC.
**/
//-----------------------------------------------------------------------------
Error_t HalDmac_StartCopy(uint32_t destAddr, uint32_t srcAddr, uint32_t byteSize)
{
    Cmd01_t cmd01;    /// DMAC command 01

    if (!TEST_U32_ALIGNED(destAddr)) return cEcNotAligned;   // not aligned
    if (!TEST_U32_ALIGNED(srcAddr))  return cEcNotAligned;   // not aligned
    if (!TEST_U32_ALIGNED(byteSize)) return cEcNotAligned;   // not aligned

    SetupDmacForMemoryOp(srcAddr, 0, destAddr, byteSize); // buffer1=source, buffer3=destination

    // setup command0 register value
    cmd01.all = 0;
    cmd01.b.SDRAM_TST_WR_EN = 0;         // SDRAM Test Write Disable.
    cmd01.b.SDRAM_TST_LBA_INC_DIS = 1;   // SDRAM Test LBA Increment Disable
    cmd01.b.SDRAM_TST_PTRN = 0;          // SDRAM Test Pattern, 0 = Data (Sequential or Fix) without CRC.
    cmd01.b.SDRAM_TST_LBA_CRC_FMT = 2;   // SDRAM Test LBA/CRC Format, 2 = The XOR CRC accumulator is seeded with FFFFFFFFh.
    cmd01.b.SDRAM_TST_EN = 0;            // SDRAM Test Disable
    cmd01.b.DIS_NEXT_ADDR_CMP = 1;       // Disable Next Address Compare.
    cmd01.b.IGNR_MISCMP = 1;             // Ignore Miscompare.
    cmd01.b.FUNC_SEL = 4;                // Function Select, 4 = DMA
    cmd01.b.DMAC_SDRAM_TST_DMA_EN = 1;   // DMAC/SDRAM Test DMA Enable.
    cmd01.b.DMAC_SDRAM_TST_STRT_CMD = 1; // DMAC/SDRAM Test Start Command.

    // Start DMA controller
    rDmac.cmd01.all = cmd01.all;

    return (cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    compares n bytes of two regions of memory\n

    @param[in]  addr1      first address to compare (must be 4-byte aligned)
    @param[in]  addr2      second address to compare (must be 4-byte aligned)
    @param[in]  byteSize   byte size to copy (must be multiple of 4)

    @return error code

    @remark Caller must check DMAC done because this code just starts DMAC.
**/
//-----------------------------------------------------------------------------
Error_t HalDmac_StartCompare(uint32_t addr1, uint32_t addr2, uint32_t byteSize)
{
    Cmd01_t cmd01;    /// DMAC command 0

    if (!TEST_U32_ALIGNED(addr1))    return cEcNotAligned;   // not aligned
    if (!TEST_U32_ALIGNED(addr2))    return cEcNotAligned;   // not aligned
    if (!TEST_U32_ALIGNED(byteSize)) return cEcNotAligned;   // not aligned

    SetupDmacForMemoryOp(addr1, addr2, 0, byteSize);

    // setup command0 register value
    cmd01.all = 0;
    cmd01.b.SDRAM_TST_WR_EN = 0;         // SDRAM Test Write Disable.
    cmd01.b.SDRAM_TST_LBA_INC_DIS = 0;   // SDRAM Test LBA Increment Disable
    cmd01.b.SDRAM_TST_PTRN = 0;          // SDRAM Test Pattern, 0 = Data (Sequential or Fix) without CRC.
    cmd01.b.SDRAM_TST_LBA_CRC_FMT = 2;   // SDRAM Test LBA/CRC Format, 2 = The XOR CRC accumulator is seeded with FFFFFFFFh.
    cmd01.b.SDRAM_TST_EN = 0;            // SDRAM Test Disable
    cmd01.b.DIS_NEXT_ADDR_CMP = 1;       // Disable Next Address Compare.
    cmd01.b.IGNR_MISCMP = 0;             // Miscompare.
    cmd01.b.FUNC_SEL = 3;                // Function Select, 3 = compare
    cmd01.b.DMAC_SDRAM_TST_DMA_EN = 1;   // DMAC/SDRAM Test DMA Enable.
    cmd01.b.DMAC_SDRAM_TST_STRT_CMD = 1; // DMAC/SDRAM Test Start Command.

    // Start DMA controller
    rDmac.cmd01.all = cmd01.all;

    return (cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Search 32bit pattern in specified memory region\n

    find memory address i which satisfy following formular.

        (pattern & (~mask)) == (MEM[i] & (~mask))

    @param[in]  addr       memory address to search(must be 4-byte aligned)
    @param[in]  pattern    32bit pattern to search
    @param[in]  mask       32bit mask bits
    @param[in]  byteSize   byte size to search (must be multiple of 4)

    @return error code

    @remark Caller must check DMAC done because this code just starts DMAC.
**/
//-----------------------------------------------------------------------------
Error_t HalDmac_StartSearch(uint32_t addr, uint32_t pattern, uint32_t mask, uint32_t byteSize)
{
    if (!TEST_U32_ALIGNED(addr))     return cEcNotAligned;     // not aligned
    if (!TEST_U32_ALIGNED(byteSize)) return cEcNotAligned;     // not aligned

    return (cEcError);
}

//-----------------------------------------------------------------------------
/**
    count number of 0 bits in the source data

    @param[in]  srcAddr      source address (must be 4-byte aligned)
    @param[in]  byteSize     byte size to count (must be multiple of 4)

    @return error code

    @remark Caller must check DMAC done because this code just starts DMAC.
**/
//-----------------------------------------------------------------------------
Error_t HalDmac_StartCount0(uint32_t srcAddr, uint32_t byteSize)
{
    Cmd01_t cmd01;    /// DMAC command 0

    if (!TEST_U32_ALIGNED(srcAddr)) return cEcNotAligned;   // not aligned
    if (!TEST_U32_ALIGNED(byteSize)) return cEcNotAligned;  // not aligned

    SetupDmacForMemoryOp(srcAddr, 0, srcAddr, byteSize); // buffer1=source, buffer3=destination

    rDmac.patSearchAndZeroCntCtrl.b.ZERO_COUNT_EN = 1;
    rDmac.patSearchAndZeroCntCtrl.b.ZERO_COUNT_SEL = 1;

    // setup command0 register value
    cmd01.all = 0;
    cmd01.b.SDRAM_TST_WR_EN = 0;         // SDRAM Test Write Disable.
    cmd01.b.SDRAM_TST_LBA_INC_DIS = 1;   // SDRAM Test LBA Increment Disable
    cmd01.b.SDRAM_TST_PTRN = 0;          // SDRAM Test Pattern, 0 = Data (Sequential or Fix) without CRC.
    cmd01.b.SDRAM_TST_LBA_CRC_FMT = 2;   // SDRAM Test LBA/CRC Format, 2 = The XOR CRC accumulator is seeded with FFFFFFFFh.
    cmd01.b.SDRAM_TST_EN = 0;            // SDRAM Test Disable
    cmd01.b.DIS_NEXT_ADDR_CMP = 1;       // Disable Next Address Compare.
    cmd01.b.IGNR_MISCMP = 1;             // Ignore Miscompare.
    cmd01.b.FUNC_SEL = 4;                // Function Select, 4 = DMA
    cmd01.b.DMAC_SDRAM_TST_DMA_EN = 1;   // DMAC/SDRAM Test DMA Enable.
    cmd01.b.DMAC_SDRAM_TST_STRT_CMD = 1; // DMAC/SDRAM Test Start Command.

    // Start DMA controller
    rDmac.cmd01.all = cmd01.all;

    return (cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    DMAC setup for SSD memory operation (Scatter/Gather)

    @param[in]  byteSize    byte size to operation (must be multiple of 4)

    @return error code
**/
//-----------------------------------------------------------------------------
static void SetupDmacForSsdMemoryOp(uint32_t rd1SsdBase, uint32_t rd1SsdIndex, uint32_t wr1SsdBase, uint32_t wr1SsdIndex, uint32_t byteSize)
{
    Cfg0_t cfg0;

    // setup buffer 1
    rDmac.LNKD_LIST_1_SUB_SEG_DSCRPTR_TBL_BASE_ADDR = rd1SsdBase;                 // buffer 1 SSD base address
    rDmac.LNKD_LIST_1_STRT_IDX_OR_ADDR              = rd1SsdIndex;                // buffer 1 SSD start index

    // setup buffer 3
    rDmac.LNKD_LIST_3_SUB_SEG_DSCRPTR_TBL_BASE_ADDR = wr1SsdBase;                 // buffer 3 reload address
    rDmac.SUB_SEG_DSCRPTR_3_STRT_ADDR_OR_IDX        = wr1SsdIndex;                // buffer 3 SSD start index

    // setup size
    rDmac.blkSize.all = byteSize;
    rDmac.BLK_CNT = 1;

    cfg0.all = 0;
    cfg0.b.DATA_FMT = 0;             // Data Format, 0 = Data Only
    cfg0.b.LNKD_LIST_PAR_DIS = 1;    // Linked List Parity Disable.
    cfg0.b.RD_CH_1_FXD_ADDR_MD = 0;  // Read Channel 1 SSD Address Mode.
    cfg0.b.RD_CH_2_FXD_ADDR_MD = 0;  // Read Channel 2 SSD Address Mode
    cfg0.b.WR_CH_FXD_ADDR_MD  = 0;   // Write Channel SSD Address Mode.

    // setup command and configuration
    rDmac.cfg0.all = cfg0.all;
    rDmac.cmd23.all = 0;
}

//-----------------------------------------------------------------------------
/**
    copies n bytes from memory area src to memory area dest\n

    @param[in]  destSsdBase  destination SSD base address
    @param[in]  destSsdIndex destination SSD index
    @param[in]  srcSsdBase   source SSD base address
    @param[in]  srcSsdIndex  source SSD index
    @param[in]  byteSize     byte size to copy (must be multiple of 4)

    @return error code

    @remark Caller must check DMAC done because this code just starts DMAC.
**/
//-----------------------------------------------------------------------------
Error_t HalDmac_StartCopySsd(uint32_t destSsdBase, uint32_t destSsdIndex, uint32_t srcSsdBase, uint32_t srcSsdIndex, uint32_t byteSize)
{
    Cmd01_t cmd01;    /// DMAC command 0

    if (!TEST_U32_ALIGNED(byteSize)) return cEcNotAligned;   // not aligned

    SetupDmacForSsdMemoryOp(srcSsdBase, srcSsdIndex, destSsdBase, destSsdIndex, byteSize);

    // setup command0 register value
    cmd01.all = 0;
    cmd01.b.SDRAM_TST_WR_EN = 0;         // SDRAM Test Write Disable.
    cmd01.b.SDRAM_TST_LBA_INC_DIS = 1;   // SDRAM Test LBA Increment Disable
    cmd01.b.SDRAM_TST_PTRN = 0;          // SDRAM Test Pattern, 0 = Data (Sequential or Fix) without CRC.
    cmd01.b.SDRAM_TST_LBA_CRC_FMT = 2;   // SDRAM Test LBA/CRC Format, 2 = The XOR CRC accumulator is seeded with FFFFFFFFh.
    cmd01.b.SDRAM_TST_EN = 0;            // SDRAM Test Disable
    cmd01.b.DIS_NEXT_ADDR_CMP = 1;       // Disable Next Address Compare.
    cmd01.b.IGNR_MISCMP = 1;             // Ignore Miscompare.
    cmd01.b.FUNC_SEL = 4;                // Function Select, 4 = DMA
    cmd01.b.DMAC_SDRAM_TST_DMA_EN = 1;   // DMAC/SDRAM Test DMA Enable.
    cmd01.b.DMAC_SDRAM_TST_STRT_CMD = 1; // DMAC/SDRAM Test Start Command.

    // Start DMA controller
    rDmac.cmd01.all = cmd01.all;

    return (cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    DMAC setup for simple linear memory operation with DPP information generation

    @param[in]  byteSize    byte size to operation (must be multiple of 4)

    @return error code
**/
//-----------------------------------------------------------------------------
static void SetupDmacForMemoryOpWithDpp(uint32_t buf1Addr, uint32_t buf2Addr, uint32_t buf3Addr, uint32_t blockSize, uint32_t blockCount)
{
    Cfg0_t cfg0;
    Cfg1_t cfg1;

    // setup buffer 1
    rDmac.LNKD_LIST_1_SUB_SEG_DSCRPTR_TBL_BASE_ADDR = buf1Addr;                   // buffer 1 address
    rDmac.LNKD_LIST_1_STRT_IDX_OR_ADDR              = buf1Addr;                   // buffer 1 reload address
    rDmac.LNKD_LIST_1_SSD_STRT_LBA                  = buf1Addr + blockSize - 4;   // buffer 1 limit address

    // setup buffer 2
    rDmac.LNKD_LIST_2_SUB_SEG_DSCRPTR_TBL_BASE_ADDR = buf2Addr;                   // buffer 2 address
    rDmac.LNKD_LIST_2_SUB_SEG_DSCRPTR_START_IX_ADDR_OR_RLD_ADDR = buf2Addr;       // buffer 2 reload address
    rDmac.LNKD_LIST_2_SSD_STRT_LBA                  = buf2Addr + blockSize - 4;   // buffer 2 limit address

    // setup buffer 3
    rDmac.LNKD_LIST_3_SUB_SEG_DSCRPTR_TBL_BASE_ADDR = buf3Addr;                   // buffer 3 address
    rDmac.SUB_SEG_DSCRPTR_3_STRT_ADDR_OR_IDX        = buf3Addr;                   // buffer 3 reload address
    rDmac.LNKD_LIST_3_SSD_STRT_LBA                  = buf3Addr + (blockSize + DMAC_BCRC_SIZE + DMAC_LBA_SIZE + DMAC_MPECC_SIZE + DMAC_MPECC_PAD)*blockCount - 4;       //(blockSize - 4;    // buffer 3 limit address

    // setup size
    rDmac.blkSize.all = blockSize;
    rDmac.BLK_CNT = blockCount;

    cfg0.all = 0;
    cfg0.b.DATA_FMT = 0;             // Data Format, 0 = Data Only
    cfg0.b.LNKD_LIST_PAR_DIS = 1;    // Linked List Parity Disable.
    cfg0.b.RD_CH_1_FXD_ADDR_MD = 1;  // Read Channel 1 Fixed Address Mode.
    cfg0.b.RD_CH_2_FXD_ADDR_MD = 1;  // Read Channel 2 Fixed Address Mode
    cfg0.b.WR_CH_FXD_ADDR_MD  = 1;   // Write Channel Fixed Address Mode.
    cfg0.b.WR_CH_MPCC_EN  = 1;       // MPECC enable
    cfg0.b.WR_CH_LBA_EN  = 1;        // HLBA enable

    cfg1.all = 0;
    cfg1.b.USE_WR_DMAC_DATA_FMT_EN = 1; // CRC enable
    cfg1.b.WR_CH_DATA_FMT = 1;          // CRC seeded w/ LBA

    // setup command and configuration
    rDmac.cfg0.all = cfg0.all;
    rDmac.cfg1.all = cfg1.all;
    rDmac.cmd23.all = 0;
}


//-----------------------------------------------------------------------------
/**
    copies N blocks from memory area src to memory area dest with BCRC/HBLA/MPECC\n

    @param[in]  destAddr     destination address (must be 4-byte aligned)
    @param[in]  srcAddr      source address (must be 4-byte aligned)
    @param[in]  hlba         host LBA
    @param[in]  blockSize    byte size of source block
    @param[in]  blockCount   block count to copy

    @return error code

    @remark Caller must check DMAC done because this code just starts DMAC.
**/
//-----------------------------------------------------------------------------
Error_t HalDmac_StartCopyDpp(uint32_t destAddr, uint32_t srcAddr, uint32_t hlba, uint32_t blockSize, uint32_t blockCount)
{
    Cmd01_t cmd01;    /// DMAC command 0

    if (!TEST_U32_ALIGNED(destAddr)) return cEcNotAligned;   // not aligned
    if (!TEST_U32_ALIGNED(srcAddr))  return cEcNotAligned;   // not aligned
    if (!TEST_U32_ALIGNED(blockSize)) return cEcNotAligned;  // not aligned

    SetupDmacForMemoryOpWithDpp(srcAddr, 0, destAddr, blockSize, blockCount); // buffer1=source, buffer3=destination

    // setup LBA
    rDmac.WR_BUF_LGCL_BLK_ADDR_0 = hlba;

    // setup command0 register value
    cmd01.all = 0;
    cmd01.b.SDRAM_TST_WR_EN = 0;         // SDRAM Test Write Disable.
    cmd01.b.SDRAM_TST_LBA_INC_DIS = 1;   // SDRAM Test LBA Increment Disable
    cmd01.b.SDRAM_TST_PTRN = 0;          // SDRAM Test Pattern, 0 = Data (Sequential or Fix) without CRC.
    cmd01.b.SDRAM_TST_LBA_CRC_FMT = 2;   // SDRAM Test LBA/CRC Format, 2 = The XOR CRC accumulator is seeded with FFFFFFFFh.
    cmd01.b.SDRAM_TST_EN = 0;            // SDRAM Test Disable
    cmd01.b.DIS_NEXT_ADDR_CMP = 1;       // Disable Next Address Compare.
    cmd01.b.IGNR_MISCMP = 1;             // Ignore Miscompare.
    cmd01.b.FUNC_SEL = 4;                // Function Select, 4 = DMA
    cmd01.b.DMAC_SDRAM_TST_DMA_EN = 1;   // DMAC/SDRAM Test DMA Enable.
    cmd01.b.DMAC_SDRAM_TST_STRT_CMD = 1; // DMAC/SDRAM Test Start Command.

    // Start DMA controller
    rDmac.cmd01.all = cmd01.all;

    return (cEcNoError);
}

#if CHK_CODE_TYPE(CODE_MAIN)
//-----------------------------------------------------------------------------
/**
    fill memory block with 32bit pattern + BCRC + HLBA + MPECC\n

    @param[in]  destAddr    destination address (must be 4-byte aligned)
    @param[in]  pattern     32bit pattern to fill
    @param[in]  hlba        host LBA
    @param[in]  blockSize   byte size of block
    @param[in]  blockCount  block count to fill

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalDmac_FillPatternWithDpp(uint32_t destAddr, uint32_t pattern, uint32_t hlba, uint32_t blockSize, uint32_t blockCount)
{
    uint8_t* tempFillBuffer;

    tempFillBuffer = (uint8_t *)MEM_AllocBuffer(blockSize, 4);

    // step 1. Generate pattern in temporay buffer
    HalDmac_StartFill((uint32_t)tempFillBuffer, pattern, blockSize);
    HalDmac_WaitDone(); // check error

    // step 2. copy one block at a time
    blockCount = 1;

    while (blockCount--)
    {
        HalDmac_StartCopyDpp(destAddr, (uint32_t)tempFillBuffer, hlba, blockSize, 1);
        HalDmac_WaitDone(); // check error
        hlba++;
        destAddr += (blockSize +  DMAC_BCRC_SIZE + DMAC_LBA_SIZE + DMAC_MPECC_SIZE);
    }

    MEM_FreeBuffer(tempFillBuffer);

    return(cEcNoError);
}
#endif

///====================================================================
/// following is DMAC MRE AES subroutines
///====================================================================
//-----------------------------------------------------------------------------
/**
    copies n bytes from memory area src to memory area dest\n

    @param[in]  destAddr     destination address (must be 4-byte aligned)
    @param[in]  srcAddr      source address (must be 4-byte aligned)
    @param[in]  byteSize     byte size to copy (must be multiple of 4)

    @return error code

    @remark Caller must check DMAC done because this code just starts DMAC.
**/
//-----------------------------------------------------------------------------
#if _CPU_!=CPU_ID1 && CO_SUPPORT_BCM
Error_t HAL_MRE_SecCopy(uint32_t destAddr, uint32_t srcAddr, uint32_t byteSize, uint32_t range)
{
    Cmd01_t cmd01;      /// DMAC command 01
    uint32_t    TimeOutCnt;

    if (!TEST_U32_ALIGNED(destAddr)) return cEcNotAligned;   // not aligned
    if (!TEST_U32_ALIGNED(srcAddr))  return cEcNotAligned;   // not aligned
    //if (!TEST_U32_ALIGNED(byteSize)) return cEcNotAligned;   // not aligned
    if (byteSize & 0x000001FF) return cEcNotAligned;   // not aligned at 512 length

    SetupDmacForMemoryOp(srcAddr, 0, destAddr, byteSize); // buffer1=source, buffer3=destination

    // Re-setup size for MRE sending to Host, 512 byte per LBA
    rDmac.blkSize.all = 512;
    rDmac.BLK_CNT = byteSize/512;

    // setup command0 register value
    cmd01.all = 0;
    cmd01.b.SDRAM_TST_WR_EN = 0;         // SDRAM Test Write Disable.
    cmd01.b.SDRAM_TST_LBA_INC_DIS = 1;   // SDRAM Test LBA Increment Disable
    cmd01.b.SDRAM_TST_PTRN = 0;          // SDRAM Test Pattern, 0 = Data (Sequential or Fix) without CRC.
    cmd01.b.SDRAM_TST_LBA_CRC_FMT = 2;   // SDRAM Test LBA/CRC Format, 2 = The XOR CRC accumulator is seeded with FFFFFFFFh.
    cmd01.b.SDRAM_TST_EN = 0;            // SDRAM Test Disable
    cmd01.b.DIS_NEXT_ADDR_CMP = 1;       // Disable Next Address Compare.
    cmd01.b.IGNR_MISCMP = 1;             // Ignore Miscompare.
    cmd01.b.FUNC_SEL = 4;                // Function Select, 4 = DMA
    cmd01.b.DMAC_SDRAM_TST_DMA_EN = 1;   // DMAC/SDRAM Test DMA Enable.
    cmd01.b.DMAC_SDRAM_TST_STRT_CMD = 1; // DMAC/SDRAM Test Start Command.

    //MRE setting:
    HAL_MRE_AesInit(range);

    // Start DMA controller
    rDmac.cmd01.all = cmd01.all;

    HalTimer_DelayUsec(10);
    TimeOutCnt = 1000000;             // 1 second
    while(HAL_MRE_Wr_AesBusy()){      // busy ?
        if(!(--TimeOutCnt)){
            HAL_MRE_Wr_AesReset();
            HalTimer_DelayUsec(10);
            return (cEcTimeOut);
        }
        HalTimer_DelayUsec(1);
    }

    return (cEcNoError);
}
#endif

#if (_TCG_!=TCG_NONE) && (_CPU_!=CPU_ID1)
#include "PerfSection.h"
/**********************************************************************
* NAME:     HAL_MRE_GetRangeNo
*
*           check Range No of start LBA
***********************************************************************/
U16 HAL_MRE_GetRangeNo(U32 lba)
{
    U32 i;

#if _CPU_ == CPU_ID0
    for (i = 0; i <= LOCKING_RANGE_CNT; i++){
        if (mLockingRangeTable[i].rangeNo){
            if ((lba >= mLockingRangeTable[i].rangeStart) && (lba <= mLockingRangeTable[i].rangeEnd)){
                return mLockingRangeTable[i].rangeNo;
            }
        }
    }

    return 0;   // Global range , Key[0]
#elif _CPU_ == CPU_ID2
    for (i = 0; i <= LOCKING_RANGE_CNT; i++){
        if (pLockingRangeTable[i].rangeNo){
            if ((lba >= pLockingRangeTable[i].rangeStart) && (lba <= pLockingRangeTable[i].rangeEnd)){
                return pLockingRangeTable[i].rangeNo;
            }
        }
    }

    return 0;   // Global range , Key[0]
#endif
}
#endif
