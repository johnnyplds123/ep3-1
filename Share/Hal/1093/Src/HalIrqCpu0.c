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
//! @brief CPU0 Interrupt Request HW control API codes
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "HalIrq.h"
#include "HalIrqSrc.h"
#include "HalTimer.h"

#include "RegApb.h"
#include "RegArmVic.h"
#include "RegIntController.h"

#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING
#include "HalGpio.h"

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief common struct to save both ARM and Marvell VIC contents
typedef struct
{
    uint32_t enableStatus;
    uint32_t vectorAddr[32];
} SaveVicContext_t;

/// @brief struct to save/restore irq status for the deep power down
typedef struct
{
    SaveVicContext_t vicIrq;
    uint32_t hardWareIrq[cIrqNumOfId];
} SaveIrqStatus_t;

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static void DefaultFwIrqCallBack(uint32_t param);
static void DefaultHwIrqHandler(IrqHandler_t irqHandler[]);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
IrqHandler_t sIrqHandler[cIrqNumOfId];
IrqHandler_t lastirq;

static IrqPowerMode_t currentIrqPowerMode;
static SaveIrqStatus_t saveIrqStatus;
//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------
extern void HalTimer_IrqHandle(IrqHandler_t irqHandler[]);
extern uint32_t HalTimer_IrqEnDisable(IrqId_t sIrqId, uint32_t state);
extern void HalIpc_IrqHandle(IrqHandler_t irqHandler[]);
extern uint32_t HalIpc_IrqEnDisable(IrqId_t sIrqId, uint32_t state);
extern uint32_t HalNvme_IrqEnDisable(IrqId_t sIrqId, uint32_t state);
extern uint32_t HalAhci_IrqEnDisable(IrqId_t sIrqId, uint32_t state);
extern void Mon_UartRxIsr(uint32_t reserved);

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    Disable hardware vectored interrupt.
    - Firmware will control
**/
//-----------------------------------------------------------------------------
__asm static void DisableHardwareVectoredInterrupt(void)
{
    MRC     p15, 0, r0, c1, c0, 0       // Read SCTLR (System Control Register)
    BIC     r0, r0, #0x1 << 24          // Disable VE bit - Firmware must read out a vector address
    MCR     p15, 0, r0, c1, c0, 0       // Write SCTLR

    BX  lr
}


//-----------------------------------------------------------------------------
/**
    Pre One-timie initialization of Interrupt HW\n
     - set all interrupt handler to default NULL handler\n
     - disable all HW interrupt.\n
     - Clear all pending HW interrupt status.\n

    @param[in]  initMode        initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalIrq_OneTimeInit(InitBootMode_t initMode)
{
    IrqId_t irqId;
    uint32_t irqBitNumber;

    DisableHardwareVectoredInterrupt();
    rApb.vectoredIntrControllCtrl.b.VIC_EN = VIC_MODE; // select VIC mode

    VIC_CLEAR_STATUS(0, U32_ALL_BIT_SET); // clear interrupts status
    VIC_DISABLE(0, U32_ALL_BIT_SET);      // Disable all interrupts

    // initailize HW independant IRQ handler
    for (irqId=cIrqBegin; irqId<cIrqNumOfId; irqId++ )
    {
        sIrqHandler[irqId] = DefaultFwIrqCallBack;
    }

    // initailize HW dependant IRQ handler
    for (irqBitNumber=0; irqBitNumber<32; irqBitNumber++)
    {
        VIC_SET_HANDLER(0, irqBitNumber, DefaultHwIrqHandler);
    }

    // register IRQ for internal shared module
    VIC_SET_HANDLER(0, cIrqSourceTimer, HalTimer_IrqHandle);    // Register timer irq handler
    VIC_SET_HANDLER(0, cIrqSourceIpc0, HalIpc_IrqHandle);       // Register IPC irq handler

    // enable IRQ for internal shared module
    VIC_ENABLE(0, BIT(cIrqSourceTimer) | BIT(cIrqSourceIpc0));

    currentIrqPowerMode = cIrqPmActive;

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    H/W block level power management API

    @param[in]  powerMode        targeting pm mode to enter
    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalIrq_SetPowerMode(IrqPowerMode_t powerMode)
{
    uint32_t index;
    IrqId_t irqId;

    // no set up if power mode is not changed.
    if (currentIrqPowerMode == powerMode) return cEcNoError;

    switch(powerMode)
    {
    case cIrqPmSleep:
    case cIrqPmDeepSleep:
        ///< Save VIC Irq status
        saveIrqStatus.vicIrq.enableStatus = VIC_GET_ENABLE_STATUS(0);
        for(index=0; index<32; index++)
        {
            saveIrqStatus.vicIrq.vectorAddr[index] = VIC_GET_HANDLER(0, index);
        }
        ///< Save HW Irq status
        for(irqId=cIrqBegin; irqId<cIrqNumOfId; irqId++)
        {
            saveIrqStatus.hardWareIrq[irqId] = HalIrq_EnDisable(irqId, DISABLE);
        }

        if (powerMode == cIrqPmSleep)
        {
            HalIrq_EnDisable(cIrqTimerOneshot,  ENABLE);
            HalIrq_EnDisable(cIrqUartRx,        ENABLE);
            HalIrq_EnDisable(cIrqHostShutdown,  ENABLE);
            HalIrq_EnDisable(cIrqHostCommandIn, ENABLE);
            HalIrq_EnDisable(cIrqHostReset,     ENABLE);
        }

        break;

    case cIrqPmActive:
        ///< Restore VIC Irq status
        for(index=0; index<32; index++)
        {
            VIC_SET_HANDLER(0, index, saveIrqStatus.vicIrq.vectorAddr[index]);
        }
        VIC_ENABLE(0, saveIrqStatus.vicIrq.enableStatus);

        DisableHardwareVectoredInterrupt();
        rApb.vectoredIntrControllCtrl.b.VIC_EN = VIC_MODE; // select VIC mode

        VIC_CLEAR_STATUS(0, U32_ALL_BIT_SET); // clear interrupts status

        ///< Restore HW Irq status
        for(irqId=cIrqBegin; irqId<cIrqNumOfId; irqId++)
        {
            HalIrq_EnDisable(irqId, saveIrqStatus.hardWareIrq[irqId]);
        }
        break;
    default:
        break;
    }

    currentIrqPowerMode = powerMode;

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Default interrupt handler (do nothing)

    @param[in]  param   interrupt dependant parameter
**/
//-----------------------------------------------------------------------------
static void DefaultFwIrqCallBack(uint32_t param)
{
}

//-----------------------------------------------------------------------------
/**
    Default interrupt handler (HALT)

    @param[in]  irqHandler   fw Irq callback handler
**/
//-----------------------------------------------------------------------------
static void DefaultHwIrqHandler(IrqHandler_t irqHandler[])
{
    E_PRINTF(cEcError, "Undefined HW IRQ handler ! %X, %X\n", irqHandler, VIC_READ_STATUS(0));
    HALT();
}

//-----------------------------------------------------------------------------
/**
    Register interrupt request service handler.

    @param[in]  irqId       Interrupt request Id
    @param[in]  irqHandler  Interrupt request Handler
    @param[in]  state       Interrupt state (ENABLE or DISABLE)

    @return previous IRQ handler
**/
//-----------------------------------------------------------------------------
IrqHandler_t HalIrq_RegisterHandler(IrqId_t irqId, IrqHandler_t irqHandler, uint32_t state)
{
    IrqHandler_t orgIrqHandler;

    if (irqId>=cIrqNumOfId) return(NULL);

    orgIrqHandler = sIrqHandler[irqId];
    sIrqHandler[irqId] = irqHandler;

    switch(irqId)
    {
    case cIrqUartRx:
        VIC_SET_HANDLER(0,  cIrqSourceUartRx, irqHandler);
        break;
    case cIrqUartTx:
        VIC_SET_HANDLER(0,  cIrqSourceUartTx, irqHandler);
        break;
    }

    HalIrq_EnDisable(irqId, state);

    return(orgIrqHandler);
}

//-----------------------------------------------------------------------------
/**
    Enable or disable HW IRQ.

    @param[in]  irqId  Interrupt request Id
    @param[in]  state  state, ENABLE or DISABLE

    @return previous HW IRQ state
**/
//-----------------------------------------------------------------------------
uint32_t HalIrq_EnDisable(IrqId_t irqId, uint32_t state)
{
    bool orgState = DISABLE;
    uint32_t irqBitNumber;

    switch(irqId)
    {
        case cIrqHostCommandIn: // enable both Admin and I/O
        case cIrqHostXferDone:
        case cIrqHostReset:
        case cIrqHostShutdown:
        case cIrqHostIfPm:
        case cIrqHostIfError:

#if CHK_CODE_TYPE(CODE_MAIN)
    #if _HOST_==HOST_NVME
            return HalNvme_IrqEnDisable(irqId, state);
    #else
            return DISABLE;
    #endif
#endif
        case cIrqUartRx:
            irqBitNumber = cIrqSourceUartRx;
            break;
        case cIrqUartTx:
            irqBitNumber = cIrqSourceUartTx;
            break;

        case cIrqIpcCommandIn: // FALL THROUGH
        case cIrqIpcStatusRead:
            return HalIpc_IrqEnDisable(irqId, state);

        case cIrqTimerPeriodic: // FALL THROUGH
        case cIrqTimerOneshot:
        case cIrqTimerSmartCheck:
            return HalTimer_IrqEnDisable(irqId, state);

        default:
            //D_PRINTF("No deal IrqId[%d]\n",irqId);
            return(DISABLE);
    }

    orgState = VIC_IS_ENABLED(0, BIT(irqBitNumber));

    if(state == ENABLE)
    {
        VIC_ENABLE(0, BIT(irqBitNumber));
    }
    else
    {
        VIC_DISABLE(0, BIT(irqBitNumber));
    }

    return(orgState);
}

#include "PerfSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
/**
    Interrupt Request handler for vectored interrupt.\n
    This code decodes interrupt and calls registered handler.\n

    @param  void
**/
//-----------------------------------------------------------------------------
uint32_t HalIrq_Handler(void)
{
    IrqHandler_t irqHandler;
    uint32_t kernSch = FALSE;

    while (VIC_READ_STATUS(0))
    {
        irqHandler = (IrqHandler_t)(VIC_GET_ACTIVE_HANDLER(0));
        irqHandler((uint32_t)sIrqHandler);
        VIC_SERVICED(0); // clear the respective interrupt

        if (irqHandler == (IrqHandler_t)Mon_UartRxIsr) kernSch = TRUE;
    }

    return kernSch;
}

//-----------------------------------------------------------------------------
/**
    Interrupt Request hanlder to emulate vectored interrupt.\n
    This code decodes interrupt and calls registered handler.\n

    @param  void
**/
//-----------------------------------------------------------------------------
void HalFiq_Handler(void)
{

}
