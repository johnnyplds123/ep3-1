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
//! @brief IPC control API codes
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "HalIrqSrc.h"
#include "HalIpc.h"
#include "HalCpu.h"
#include "HalIrq.h"
#include "HalRegConfig.h"
#include "RegIpc.h"

#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
/// @brief IPC command (see ROM IPC spec)
typedef enum
{
    cIpcCmdGo = 2
} IpcRomCommand;

/// @brief IPC status (see ROM IPC spec)
typedef enum
{
    cIpcStatusRequest = 0x55,
    cIpcStatusSuccess = 0x10,
    cIpcStatusFail    = 0x11
} IpcRomStatus;

#if _CPU_==CPU_ID0
#define OPPONENT_CPU_A  CPU_ID1
#define OPPONENT_CPU_B  CPU_ID2
#elif _CPU_==CPU_ID1
#define OPPONENT_CPU_A  CPU_ID0
#define OPPONENT_CPU_B  CPU_ID2
#elif _CPU_==CPU_ID2
#define OPPONENT_CPU_A  CPU_ID0
#define OPPONENT_CPU_B  CPU_ID1
#else
    #error "Invalid CPU Id"
#endif

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define GetBaseDescriptorNumber(source, target)     ipcDescOffset[source][target]
#define GetCmdDescriptorNumber(source, target)      ipcDescOffset[source][target]
#define GetStatusDescriptorNumber(source, target)   ipcDescOffset[source][target]
#define GetIpcDescriptorPointer(source, target)     ((IpcPacket_t *) &rIpc.DESC_REG[GetBaseDescriptorNumber(source,target)])

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

/// @brief IPC command parameter definition
typedef union
{
    uint32_t all;
    struct
    {
        uint32_t cpuId:2;   // CPU id  (HW layout)
        uint32_t cmd:14;    // command (FW layout)
        uint32_t wParam:16; // 16bit parameter (FW layout)
    } b;
} IpcCmdParam_t;

/// @brief IPC packet structure (FW layout)
typedef struct
{
    volatile IpcCmdParam_t cmdParam;    // command and word parameter
    volatile uint32_t lParam;           // long parameter
} IpcPacket_t;

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
static IpcPowerMode_t currentIpcPowerMode;

//-----------------------------------------------------------------------------
//
// Allocation of IPC registers (FW layout)
//
// - Descriptor Register 0:    Command and 16bit parameter from CPU0 to CPU1
// - Descriptor Register 1:    32bit Parameter from CPU0 to CPU1
//
// - Descriptor Register 2:    Command and 16bit parameter from CPU0 to CPU2
// - Descriptor Register 3:    32bit Parameter from CPU0 to CPU2
//
// - Descriptor Register 4:    Command and 16bit parameter from CPU1 to CPU0
// - Descriptor Register 5:    32bit Parameter from CPU1 to CPU0
//
// - Descriptor Register 6:    Command and 16bit parameter from CPU1 to CPU2
// - Descriptor Register 7:   32bit Parameter from CPU2 to CPU0
//
// - Descriptor Register 8:   Command and 16bit parameter from CPU2 to CPU0
// - Descriptor Register 9:   32bit Parameter from CPU1 to CPU2
//
// - Descriptor Register 10:   Command and 16bit parameter from CPU2 to CPU1
// - Descriptor Register 11:   32bit Parameter from CPU2 to CPU1
//
static const uint8_t ipcDescOffset[SOC_NUM_OF_CPU][SOC_NUM_OF_CPU]  =
{
    0*sizeof(IpcPacket_t)/4,    // CPU0 to CPU0 (N/A)
    0*sizeof(IpcPacket_t)/4,    // CPU0 to CPU1
    1*sizeof(IpcPacket_t)/4,    // CPU0 to CPU2
    2*sizeof(IpcPacket_t)/4,    // CPU1 to CPU0
    2*sizeof(IpcPacket_t)/4,    // CPU1 to CPU1 (N/A)
    3*sizeof(IpcPacket_t)/4,    // CPU1 to CPU2
    4*sizeof(IpcPacket_t)/4,    // CPU2 to CPU0
    5*sizeof(IpcPacket_t)/4,    // CPU2 to CPU1
    5*sizeof(IpcPacket_t)/4     // CPU2 to CPU2 (N/A)
};

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

#if _CPU_==CPU_ID0    // CPU0 only
//-----------------------------------------------------------------------------
/**
    One time Initialization of IPC HW.\n

    @param[in]  initMode        initialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalIpc_OneTimeInit(InitBootMode_t initMode)
{
    uint32_t i;

    // Clear registers for initialization:
    for(i=0; i<SOC_NUM_OF_CPU; i++)
    {
        rIpc.INT_PEND_CLR[i] = U32_ALL_BIT_SET; // interrupt pending
        rIpc.INT_MASK_SET[i] = U32_ALL_BIT_SET; // interrupt mask  on
        rIpc.INT_ENAB_SET[i] = U32_ALL_BIT_SET; // interrupt enable
    }

    // set signal mode for all descriptor
    for(i=0; i<HW_IPC_NUM_OF_DESCRIPTOR; i++)
    {
        rIpc.descriptCtrl[i].b.DESC_00_MODE = 0;
    }

    // set callback mode for command descriptor
    for(i=0; i<SOC_NUM_OF_CPU*(SOC_NUM_OF_CPU-1); i++)
    {
        rIpc.descriptCtrl[i*sizeof(IpcPacket_t)/4].b.DESC_00_MODE = 1;
    }

    currentIpcPowerMode = cIpcPmActive;

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Start CPU.

    @param[in]  cpuId    cpu Id number to start
**/
//-----------------------------------------------------------------------------
void HalIpc_StartCpu(uint32_t cpuId)
{
    HalIpc_JumpToAddress(cpuId, 0);
}

#include "PerfSection.h"
//-----------------------------------------------------------------------------
/**
    Start CPU from the jump address.

    @param[in]  cpuId    cpu Id number to start
    @param[in]  address  Jump address
**/
//-----------------------------------------------------------------------------
void HalIpc_JumpToAddress(uint32_t cpuId, uint32_t address)
{
    IpcPacket_t* pIpcPacket;

    switch(cpuId)
    {
    case CPU_ID1:
        rIpc.DESC_REG[8] = cIpcCmdGo;           // Go command
        rIpc.DESC_REG[9] = address;             // Jump address
        rIpc.DESC_REG[1] = cIpcStatusRequest;   // send command to CPU1
        while (rIpc.DESC_REG[3]==cIpcStatusSuccess); // wait ACK from CPU1
        break;
    case CPU_ID2:
        rIpc.DESC_REG[16] = cIpcCmdGo;          // Go command
        rIpc.DESC_REG[17] = address;            // Jump address
        rIpc.DESC_REG[2] = cIpcStatusRequest;   // send command to CPU2
        while (rIpc.DESC_REG[4]==cIpcStatusSuccess); // wait ACK from CPU2
        break;
    default:
        HALT();
    }
}
#include "FastSection.h"
//-----------------------------------------------------------------------------
/**
    H/W block level power management API

    @param[in]  powerMode        targeting pm mode to enter
    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalIpc_SetPowerMode(IpcPowerMode_t powerMode)
{
    uint32_t i;

    // no set up if power mode is not changed.
    if (currentIpcPowerMode == powerMode) return cEcNoError;

    switch(powerMode)
    {
    case cIpcPmActive:
        HalIpc_OneTimeInit(cInitBootDeepPowerDown);
        break;

    case cIpcPmDeepSleep:
        // Do nothing
        break;
    default:
        break;
    }

    currentIpcPowerMode = powerMode;

    return cEcNoError;
}
#endif

//-----------------------------------------------------------------------------
/**
    Read IPC command from CPU\n

    This will generate source CPU's IPC read status interrupt.\n

    @param[in]   cpuId       source CPU id
    @param[out]  pCmdParam   16 bit command[15:0]
    @param[out]  pwParam1    16 bit parameter pointer
    @param[out]  plParam2    32 bit parameter pointer
**/
//-----------------------------------------------------------------------------
void HalIpc_ReadCmdFrom(uint16_t cpuId, uint16_t* pCmd, uint16_t* pwParam, uint32_t* plParam)
{
    IpcPacket_t* pIpcPacket;
    IpcCmdParam_t cmdParam;

    pIpcPacket = GetIpcDescriptorPointer(cpuId, _CPU_);

    *plParam = pIpcPacket->lParam;
    cmdParam.all = pIpcPacket->cmdParam.all; // generate interrupt to source CPU

    *pwParam = cmdParam.b.wParam;
    *pCmd = cmdParam.b.cmd;
}

//-----------------------------------------------------------------------------
/**
    Write IPC command to CPU\n

    This will generate target CPU's IPC write interrupt.\n

    @param[in]  cpuId       target CPU id
    @param[in]  cmd         16 bit command
    @param[in]  wParam      16 bit additional parameter
    @param[in]  lParam      32 bit additional parameter
**/
//-----------------------------------------------------------------------------
void HalIpc_WriteCmdTo(uint16_t cpuId, uint16_t cmd, uint16_t wParam, uint32_t lParam)
{
    IpcPacket_t* pIpcPacket;
    IpcCmdParam_t cmdParam;

    pIpcPacket = GetIpcDescriptorPointer(_CPU_, cpuId);

    pIpcPacket->lParam = lParam;

    cmdParam.all = 0;
    cmdParam.b.cpuId = _CPU_;  // set submitter's CPU id for interrupt call back
    cmdParam.b.cmd = cmd;
    cmdParam.b.wParam = wParam;

    pIpcPacket->cmdParam.all = cmdParam.all; // generate interrupt to target CPU
}

//-----------------------------------------------------------------------------
/**
    Enable or disable IPC interrupt

    @param[in]  sIrqId       software IRQ ID.
    @param[in]  state        ENABLE or DISABLE IRQ

    @return previous IRQ state
**/
//-----------------------------------------------------------------------------
uint32_t HalIpc_IrqEnDisable(IrqId_t sIrqId, uint32_t state)
{
    bool orgState;
    uint32_t maskBits;

    switch(sIrqId)
    {
    case cIrqIpcCommandIn:
        maskBits = BIT(GetCmdDescriptorNumber(OPPONENT_CPU_A, _CPU_)) | BIT(GetCmdDescriptorNumber(OPPONENT_CPU_B, _CPU_));
        break;
    case cIrqIpcStatusRead:
        maskBits = BIT(GetStatusDescriptorNumber(_CPU_, OPPONENT_CPU_A)) | BIT(GetStatusDescriptorNumber(_CPU_, OPPONENT_CPU_B));
        break;
    default:
        return DISABLE;
    }

    orgState = ((rIpc.INT_MASK_CLR[_CPU_] & maskBits) == maskBits) ? DISABLE : ENABLE;

    if (state==ENABLE)
    {
        rIpc.INT_MASK_CLR[_CPU_] = maskBits;
    }
    else
    {
        rIpc.INT_MASK_SET[_CPU_] = maskBits;
    }

    return(orgState);
}

//-----------------------------------------------------------------------------
/**
    Handle IPC interrupt

    @param[in]  irqHandler  interrupt request handler table
**/
//-----------------------------------------------------------------------------
void HalIpc_IrqHandle(IrqHandler_t irqHandler[])
{
    uint32_t intPending;

    while ((intPending = (rIpc.INT_PEND_CLR[_CPU_] & (~rIpc.INT_MASK_CLR[_CPU_])))!=0)
    {
        rIpc.INT_PEND_CLR[_CPU_] = intPending; // clear interrupt pending

        if (intPending & BIT(GetCmdDescriptorNumber(OPPONENT_CPU_A, _CPU_)))
        {
            irqHandler[cIrqIpcCommandIn](OPPONENT_CPU_A);
        }

        if (intPending & BIT(GetCmdDescriptorNumber(OPPONENT_CPU_B, _CPU_)))
        {
            irqHandler[cIrqIpcCommandIn](OPPONENT_CPU_B);
        }

        if (intPending & BIT(GetStatusDescriptorNumber(_CPU_, OPPONENT_CPU_A)))
        {
            irqHandler[cIrqIpcStatusRead](OPPONENT_CPU_A);
        }

        if (intPending & BIT(GetStatusDescriptorNumber(_CPU_, OPPONENT_CPU_B)))
        {
            irqHandler[cIrqIpcStatusRead](OPPONENT_CPU_B);
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Check and Clear IPC pending interrupt
      In case that FW is polling IPC message without using the interrupt, pending bits are remained.
      Clear the pending bit so that CPU can stay WFI at power down mode.

    @return Pending interrupt status
**/
//-----------------------------------------------------------------------------
uint32_t HalIpc_ClearInterruptPending(void)
{
    uint32_t intPending = rIpc.INT_PEND_CLR[_CPU_];
    rIpc.INT_PEND_CLR[_CPU_] = intPending; // clear interrupt pending

    return intPending;
}

