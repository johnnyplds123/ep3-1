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
//! @brief IRQ Source Definition
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
#define VIC_MARVELL     0           ///< Marvell VIC
#define VIC_ARM         1           ///< ARM VIC
#define VIC_MODE        VIC_ARM     ///< VIC selection

/// @brief HW dependant IRQ Source Definition
typedef enum
{
    cIrqSourceFlash=0,          // FLASH_INT_N
    cIrqSourceMc5,              // MC5_INT
    cIrqSourceNvme,             // NVME_INT
    cIrqSourcePcieClkReqN,      // PCIE_INT_CLK_REQ_N

    cIrqSourcePcie=4,           // PCIE_INT
    cIrqSourceI2c,              // I2C_INT_01
    cIrqSourceDmaDone,          // DMA_DONE_INT
    cIrqSourceSdmaAutoFth,      // SDMA_INT_AUTO_FTH

    cIrqSourceSdmaHctUpdt=8,    // SDMA_INT_HCT_UPDT
    cIrqSourceSdmaPt0Updt,      // SDMA_INT_PT0_UPDT
    cIrqSourcePcieDma,          // PCIE_DMA_INT
    cIrqSourceComCpu2,          // COMRX_INT_CPU2 | COMTX_INT_CPU2

    cIrqSourceComCpu1=12,       // COMRX_INT_CPU1 | COMTX_INT_CPU1
    cIrqSourceComCpu0,          // COMRX_INT_CPU0 | COMTX_INT_CPU0
    cIrqSourcePcieDma1,         // PCIE_DMA_INT_1
    cIrqSourceWatchDog,         // WATCH_DOG_INT
    
    cIrqSourceIpc0=16,          // IPC_INT_0
    cIrqSourceIpc1,             // IPC_INT_1
    cIrqSourceIpc2,             // IPC_INT_2
    cIrqSourcePcieDma2,         // PCIE_DMA_INT_2

    cIrqSourceUartRx=20,        // UART_RX_INT
    cIrqSourceUartTx,           // UART_TX_INT
    cIrqSourcePcieClkReq,       // PCIE_INT_CLK_REQ
    cIrqSourceSdp12,            // SDP12_INT

    cIrqSourceTimer=24,         // TIMER_INT
    cIrqSourceReserved=25,
    cIrqSourceTestpin10,        // TESTPIN10_INT
    cIrqSourceTestpin11,        // TESTPIN11_INT

    cIrqSourceTsen=28,          // TSEN_INT
    cIrqSourcePcieVRstN,        // PCIE PCIE reset
    cIrqSourceUart2,            // UART_RX_INT_2ND | UART_TX_INT_2ND
    cIrqSourceAon               // AON_INT

} IrqSource_t;


/// IPC Descriptor assignment for periodic timer interrupt emulation 

static const uint32_t cIpcDescPeriodicTimerCpu1 = 30;
static const uint32_t cIpcDescPeriodicTimerCpu2 = 31;

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#if (VIC_MODE==VIC_ARM)
#define VIC_READ_STATUS(cpu)                (rVintc##cpu.irqStatus)
#define VIC_READ_RAW_STATUS(cpu)            (rVintc##cpu.rawIntr)
#define VIC_CLEAR_STATUS(cpu, bits)         (rVintc##cpu.activeVectAddr = 0)
#define VIC_SERVICED(cpu)                   (rVintc##cpu.activeVectAddr = 0)
#define VIC_ENABLE(cpu, bits)               (rVintc##cpu.intEnable = (bits))
#define VIC_DISABLE(cpu, bits)              (rVintc##cpu.intEnableClear = (bits))
#define VIC_GET_ENABLE_STATUS(cpu)          rVintc##cpu.intEnable
#define VIC_IS_ENABLED(cpu, bits)           ((rVintc##cpu.intEnable & (bits)) ? ENABLE : DISABLE)
#define VIC_SET_HANDLER(cpu, id, handler)   (rVintc##cpu.vectAddr[id] = (uint32_t)(handler))
#define VIC_GET_HANDLER(cpu, id)            rVintc##cpu.vectAddr[id]
#define VIC_GET_ACTIVE_HANDLER(cpu)         rVintc##cpu.activeVectAddr
#define VIC_ASSERT(cpu, bits)               (rVintc##cpu.softInt = bits)
#define VIC_DEASSERT(cpu, bits)             (rVintc##cpu.softIntClear = bits)
#else
#define VIC_READ_STATUS(cpu)                (rIntc##cpu.IRQ_SOURCE_IRP)
#define VIC_READ_RAW_STATUS(cpu)            (rIntc##cpu.IRQ_RAW_SOURCE)
#define VIC_CLEAR_STATUS(cpu, bits)         (rIntc##cpu.CLR_IRQ_IRS = (bits))
#define VIC_SERVICED(cpu)                   
#define VIC_ENABLE(cpu, bits)               (rIntc##cpu.IRQ_INT_MSK_CLR = (bits))
#define VIC_DISABLE(cpu, bits)              (rIntc##cpu.IRQ_INT_MSK_SET = (bits))
#define VIC_GET_ENABLE_STATUS(cpu)          (~(rIntc##cpu.IRQ_INT_MSK_SET))         //@@ need to check
#define VIC_IS_ENABLED(cpu, bits)           ((rIntc##cpu.IRQ_INT_MSK_SET & (bits)) ? DISABLE : ENABLE)
#define VIC_SET_HANDLER(cpu, id, handler)   (rIntc##cpu.intrControllIrqVector[id] = (uint32_t)(handler))
#define VIC_GET_HANDLER(cpu, id)            rIntc##cpu.intrControllIrqVector[id]
#define VIC_GET_ACTIVE_HANDLER(cpu)         rIntc##cpu.IC_IRQ_INT_BASE
#define VIC_ASSERT(cpu, bits)               
#define VIC_DEASSERT(cpu, bits)             
#endif

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------


