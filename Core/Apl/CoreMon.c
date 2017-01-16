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
//! @brief Service call message handler
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"

#include "HalHwCheck.h"

#include "CoreMsg.h"
#include "Heap.h"
#include "Cache.h"

#include "Ftl.h"
#include "Gc.h"
#include "Core.h"
#include "IDS.h"
#include "Aging.h"
#include "PLP.h"
#include "CoreDbg.h"
#include "Purge.h"
#include "SmBus.h"
#include "SysInfo.h"

#include "NormSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
static const uint32_t cMsgWaitTurnAroundTimeUs = 128;   ///< waiting time for new message

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static void CoreMon_IpcCmdInIsr(uint32_t param);
static void CoreMon_HandleResetEvent(void);
static bool CoreMon_WaitForNewCommands(uint32_t timeOutUsec);
static void CoreMon_ProcessMessages(void);
static void CoreMon_HandleMsgInEvent(void);
static void CoreMon_ServiceHandler(Msg_t* pMsg);
static void CoreMon_HandleSysMsg(Msg_t* pMsg);
static void CoreMon_HandleDbgMsg(Msg_t* pMsg);
static void CoreMon_Complete(Msg_t* pMsg);
static void CoreMon_DumpInfo(void);
static void CoreMon_ServiceLoop(void);

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief service message handler decoding table
typedef struct
{
    uint8_t  startOpCode;           ///< start msg code
    uint8_t  endOpCode;             ///< end msg code
    void (*pFunc)(Msg_t*);          ///< function pointer to service message
} SvcMsgTbl_t;

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
static const SvcMsgTbl_t sCoreMsgTable[] =
{
    { cSfnSysBegin,             cSfnSysEnd,             CoreMon_HandleSysMsg       },
    { cSfnCoreDbgMsgBegin,      cSfnCoreDbgMsgEnd,      CoreMon_HandleDbgMsg       },
    { 0,                        0,                      NULL                       }
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
//-----------------------------------------------------------------------------
/**
    IPC command in Interrupt service routine

    @param[in]  param   parameter
**/
//-----------------------------------------------------------------------------
static void CoreMon_IpcCmdInIsr(uint32_t param)
{
    Kernel_SetEventFlags(EVENT_OBJ_COREMON, EVENT_COREMON_MSG_IN);
}

//-----------------------------------------------------------------------------
/**
    IPC message in event handler.
**/
//-----------------------------------------------------------------------------
static void CoreMon_HandleMsgInEvent(void)
{
    HalIrq_EnDisable(cIrqIpcCommandIn, DISABLE);

    do
    {
        CoreMon_ProcessMessages();
    } while (CoreMon_WaitForNewCommands(cMsgWaitTurnAroundTimeUs)); // do not exit immediatley, check new message for a while to boost QD1 performance

    HalIrq_EnDisable(cIrqIpcCommandIn, ENABLE);
}

//-----------------------------------------------------------------------------
/**
    Process many IPC messages
**/
//-----------------------------------------------------------------------------
static void CoreMon_ProcessMessages(void)
{
    Msg_t* pMsg;

    while ((pMsg=Ipc_PeekMsg(cH2cUartQueue, true))!=NULL)
    {
        CoreMon_ServiceHandler(pMsg);
    }
}

//-----------------------------------------------------------------------------
/**
    Waits for new commands with time out

    @param[in]  timeOutUsec  time out

    @return true if there are new commands
**/
//-----------------------------------------------------------------------------
static bool CoreMon_WaitForNewCommands(uint32_t timeOutUsec)
{
    HwTimerTick_t tick;

    tick = HalTimer_ReadTick();

    while (Ipc_IsEmpty(cH2cUartQueue))
    {
        if (HalTimer_GetElapsedTimeUsec(tick)>timeOutUsec)
        {
            return(false);
        }
    }

    return(true);
}

//-----------------------------------------------------------------------------
/**
    Handle all service messages

    @param[in]  pMsg    message pointer
**/
//-----------------------------------------------------------------------------
static void CoreMon_ServiceHandler(Msg_t* pMsg)
{
    const SvcMsgTbl_t* pTable = sCoreMsgTable;

    pMsg->error = cEcNoError;

    while (pTable->pFunc != NULL)
    {
        if (pTable->startOpCode <= pMsg->hdr.b.opCode && pMsg->hdr.b.opCode <= pTable->endOpCode)
        {
            pTable->pFunc(pMsg);
            return;
        }
        pTable++;
    }

    pMsg->error = cEcInvalidCommand;
    E_PRINTF(pMsg->error, "SERVICE MSG ERROR FN:%4X\n", pMsg->hdr.b.opCode);
    Debug_DumpMemU32((uint32_t)pMsg, sizeof(Msg_t));

    CoreMon_Complete(pMsg);
}

//-----------------------------------------------------------------------------
/**
    Handle system service messages

    @param[in]  pMcmdQ    media command queue pointer
**/
//-----------------------------------------------------------------------------
static void CoreMon_HandleSysMsg(Msg_t* pMsg)
{
    switch (pMsg->hdr.b.opCode)
    {
        case cSfnDumpInfo:
            CoreMon_DumpInfo();
            break;
        case cSfnWakeup:
            break;
        default:
            pMsg->error = cEcInvalidCommand;
    }

    CoreMon_Complete(pMsg);
}

//-----------------------------------------------------------------------------
/**
    Handle core debug messages

    @param[in]  pMsg    media command queue pointer
**/
//-----------------------------------------------------------------------------
static void CoreMon_HandleDbgMsg(Msg_t* pMsg)
{
    MsgSeq_t* pMsgRw;
    pMsgRw = (MsgSeq_t*)pMsg;
    switch (pMsg->hdr.b.opCode)
    {
        case cSfnCacheDisplay:
            Cache_DumpInfo(pMsg->param[0]);
            break;
        case cSfnHMSGDisplay:
            CMSG_DumpInfo();
            break;
        case cSfnHeapDisplay:
            Heap_Dump();
            break;
        case cSfnFTLDisplay:
            FTL_DumpInfo();
            break;
        case cSfnRMPDisplay:
            FTL_DumpRemap(FALSE);
            break;
        case cSfnL2PDisplay:
            FTL_DumpL2P(pMsg->param[0], pMsg->param[1]);
            break;
        case cSfnVACDisplay:
            FTL_DumpVAC();
            break;
        case cSfnTRIMBMPDisplay:
            FTL_DumpTrimInfo();
            break;
        case cSfnFTLflushDummy:
            if ((gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].bank != 0) || (gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].page > 0))
            {
                U16 page, dmpage;

                page = gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].page - 1;
                dmpage = (page % 2) ? (page + 3) : (page + 2);
                dmpage = (dmpage > gNfInfo.pagesPerBlock) ? gNfInfo.pagesPerBlock : dmpage;

                D_PRINTF("Flush dummy\tBN:%d BK:0x%4X PG:%3X -> %3X\n", gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].bank, gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].block, page, dmpage);
                //FTL_FlushDummyCloseBlock(FTL_BLOCK_HEADER_HOST, 0, dmpage, FALSE);
                FTL_FlushDummyCloseBlock(FTL_BLOCK_HEADER_HOST, 0, dmpage);
                D_PRINTF("Flush Done\tBN:%d BK:0x%4X PG:%3X\n", gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].bank, gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].block, gFtlCurrHeader[FTL_BLOCK_HEADER_HOST][0].page);
            }
            break;
        case cSfnGcDisplay:
            #if(ENABLE_GC == TRUE)
            GC_DumpInfo();
            #endif
            break;
        case cSfnGcP2lDisplay:
            #if(ENABLE_GC == TRUE && DBG_GC_LP == TRUE)
            GC_P2lDumpInfo(pMsg->param[0],pMsg->param[1],pMsg->param[2]);
            #endif
            break;
        case cSfnGcTest:
            #if(ENABLE_GC == TRUE && DBG_GC_FUNCTION)
            GC_Test(pMsg->param[0],pMsg->param[1],pMsg->param[2]);
            #endif
            break;
        case csfnPreformat:
            Purge_Center(PURGE_CORE_FTL);
            break;
        case csfnSeqRW:
            CoreDbg_Center(pMsgRw->opcode, pMsgRw->plane, pMsgRw->paa, pMsgRw->pairblk[0], pMsgRw->pairblk[1], pMsgRw->pairblk[2]);
            break;
        case cSfnAging:
            Aging_Center(pMsg->param[0], pMsg->param[1], pMsg->param[2]);
            break;
        case cSfnHwCheck:
            pMsg->param[3] = HalHwCheck_Main((HalHwCheckComponent_t)pMsg->param[0], pMsg->param[1], pMsg->param[2]);
            break;
        case cSfnSmBus:
            #if (CO_SUPPORT_SMBUS == TRUE)
            AplSmBus_Center(pMsg->param[0], (U8 *)pMsg->param[1], pMsg->param[2], pMsg->param[3]);
            #endif
            break;
        case cSfnSIClear:
            Purge_Center(PURGE_SI_FTL | PURGE_SI_NVEC | PURGE_SI_UNCT | PURGE_SI_NVME | PURGE_SI_FTLRMP | PURGE_SI_TCG | PURGE_SI_LOGINFO | PURGE_SI_GLIST);
            break;
        case cSfnHoldCpu2:
            CoreMon_Complete(pMsg);
            while (smHoldCpuX);
            D_PRINTF("[CMON] Unlock CPU2\n");
            return;
        default:
            pMsg->error = cEcInvalidCommand;
    }

    CoreMon_Complete(pMsg);
}

//-----------------------------------------------------------------------------
/**
    Media complete service.

    @param[in]  pCcmdQ   core command queue pointer.
**/
//-----------------------------------------------------------------------------
static void CoreMon_Complete(Msg_t* pMsg)
{
    Ipc_CompleteMsg(pMsg, IPC_NO_TIMEOUT);
}

//-----------------------------------------------------------------------------
/**
    Dump media information.
**/
//-----------------------------------------------------------------------------
static void CoreMon_DumpInfo(void)
{
    Kernel_DumpInfo();

    M_PRINTF("\n[CORE]\n");

    M_PRINTF("[ITCM] ");
    MemAlloc_DumpAo(&itcmAllocatorAo);

    M_PRINTF("[DTCM] ");
    MemAlloc_DumpAo(&dtcmAllocatorAo);

    CMSG_DumpInfo();

    M_PRINTF("FreePrp[%3d]\n", gCoreFreePrpCnt);
}

//-----------------------------------------------------------------------------
/**
    Core monitor service task startup
**/
//-----------------------------------------------------------------------------
void CoreMon_StartTask(void)
{
    HalIrq_RegisterHandler(cIrqIpcCommandIn, CoreMon_IpcCmdInIsr, ENABLE);

    Kernel_InitEventFlags(EVENT_OBJ_COREMON, KERNEL_NO_SPINLOCK);

    CoreMon_ServiceLoop();
}

//-----------------------------------------------------------------------------
/**
    Core Mon service loop
**/
//-----------------------------------------------------------------------------
static void CoreMon_ServiceLoop(void)
{
    KernelEventFlags_t events;

    D_PRINTF("[Core] MonTask\n");

    while (TRUE)
    {
        events = Kernel_WaitEventFlags(EVENT_OBJ_COREMON, EVENT_COREMON_MSG_IN, KERNEL_NO_TIMEOUT);

        if (events & EVENT_COREMON_MSG_IN)
        {
            CoreMon_HandleMsgInEvent();
        }
    }
}
