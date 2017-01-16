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
//! @brief MEDIA PARTITION Service call message handler
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"

#include "HalNfif.h"
#include "HalFlc.h"
#include "HalEcu.h"
#include "HalSeq.h"
#include "Fio.h"
#include "AplNfif.h"
#include "AplDesc.h"
#include "AplSeq.h"
#include "AplReadRetry.h"

#include "Media.h"

#include "NormSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
static const uint32_t cMsgWaitTurnAroundTimeUs = 2;   ///< waiting time for new UART message

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

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

/// @brief Vendor command Read Flash ID
typedef struct
{
    uint8_t  ch;                     ///< ch
    uint8_t  ce;                     ///< ce
    uint8_t  lun;                    ///< lun
    uint8_t  reserved1[1];
    uint8_t  ID[8];
    uint8_t  reserved2[4];
} SvcMsgVscFlashID_t;

/// @brief Vendor command Get Feature
typedef struct
{
    uint8_t  ch;                     ///< ch
    uint8_t  ce;                     ///< ce
    uint8_t  lun;                    ///
    uint8_t  param[4];               ///
    uint8_t  reserved;               ///
} SvcMsgVscFeature_t;

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static void MediaMon_HandleMsgInEvent(void);
static void MediaMon_ProcessMessages(void);
static bool MediaMon_WaitForNewCommands(uint32_t timeOutUsec);
static void MediaMon_Complete(Msg_t* pMsg);
static void MediaMon_ServiceHandler(Msg_t* pMsg);

static void MediaMon_HandleSysMsg(Msg_t* pMsg);
static void MediaMon_HandleNfifMsg(Msg_t* pMsg);
static void MediaMon_HandleVscMsg(Msg_t* pMsg);
static void MediaMon_HandleDbgMsg(Msg_t* pMsg);

static void MediaMon_DumpInfo(U32 param);
static void MediaMon_ServiceLoop(void);

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
static const SvcMsgTbl_t sMediaMsgTable[] =
{
    { cSfnSysBegin,             cSfnSysEnd,             MediaMon_HandleSysMsg      },
    { cSfnNfimMsgBegin,         cSfnNfimMsgEnd,         MediaMon_HandleNfifMsg     },
    { cSfnVscMsgBegin,          cSfnVscMsgEnd,          MediaMon_HandleVscMsg      },
    { cSfnMediaDbgMsgBegin,     cSfnMediaDbgMsgEnd,     MediaMon_HandleDbgMsg      },
    { 0,                        0,                      NULL                       }
};

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------
extern Error_t Pkg_Configure(NfCfgInfo_t *pCfgInfo);
extern Error_t Pkg_ScanDefect(void);

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
    IPC command in Interrupt service routine

    @param[in]  param   parameter
**/
//-----------------------------------------------------------------------------
static void MediaMon_IpcCmdInIsr(uint32_t param)
{
    Kernel_SetEventFlags(EVENT_OBJ_MEDIAMON, EVENT_MEDIAMON_MSG_IN);
}

//-----------------------------------------------------------------------------
/**
    IPC message in event handler.
**/
//-----------------------------------------------------------------------------
static void MediaMon_HandleMsgInEvent(void)
{
    HalIrq_EnDisable(cIrqIpcCommandIn, DISABLE);

    do
    {
        MediaMon_ProcessMessages();
    } while (MediaMon_WaitForNewCommands(cMsgWaitTurnAroundTimeUs)); // do not exit immediatley, check new message for a while to boost QD1 performance

    HalIrq_EnDisable(cIrqIpcCommandIn, ENABLE);
}

//-----------------------------------------------------------------------------
/**
    Process many IPC messages
**/
//-----------------------------------------------------------------------------
static void MediaMon_ProcessMessages(void)
{
    Msg_t* pMsg;

    // Let's first check the Core to Media Queue (frequent)
    while ((pMsg=Ipc_PeekMsg(cC2mOtherQueue, true))!=NULL)
    {
        MediaMon_ServiceHandler(pMsg);
    }

    // Let's also check the Host to Media Queue (unfrequent)
    while ((pMsg=Ipc_PeekMsg(cH2mReqQueue, true))!=NULL)
    {
        MediaMon_ServiceHandler(pMsg);
    }
}

//-----------------------------------------------------------------------------
/**
    Waits for new commands with time out

    @param[in]  timeOutUsec  time out

    @return true if there are new commands
**/
//-----------------------------------------------------------------------------
static bool MediaMon_WaitForNewCommands(uint32_t timeOutUsec)
{
    HwTimerTick_t tick;

    tick = HalTimer_ReadTick();

    while (Ipc_IsEmpty(cC2mOtherQueue) && Ipc_IsEmpty(cH2mReqQueue))
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
    Media complete service.

    @param[in]  pMsg    message pointer
**/
//-----------------------------------------------------------------------------
static void MediaMon_Complete(Msg_t* pMsg)
{
    Ipc_CompleteMsg(pMsg, IPC_NO_TIMEOUT);

    if (pMsg->hdr.b.nc == FALSE)
    {
        Ipc_PutMsg(cM2hComQueue, pMsg, IPC_NO_TIMEOUT);
    }
}

//-----------------------------------------------------------------------------
/**
    Handle all service messages

    @param[in]  pMsg    message pointer
**/
//-----------------------------------------------------------------------------
static void MediaMon_ServiceHandler(Msg_t* pMsg)
{
    const SvcMsgTbl_t* pTable = sMediaMsgTable;

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

    MediaMon_Complete(pMsg);
}

//-----------------------------------------------------------------------------
/**
    Handle NFIF service messages

    @param[in]  pMsg    message pointer
**/
//-----------------------------------------------------------------------------
static void MediaMon_HandleNfifMsg(Msg_t* pMsg)
{
    tPAA  paa;
    Pna_t pna;
    U32 *pCurBuffer;
    MsgPhyRwe_t* pMsgRwe;
    SvcMsgVscFlashID_t* flashId;
    SvcMsgVscFeature_t* feature;

    U16 idx;
    U8  ch, ce, lun;

    pMsgRwe    = (MsgPhyRwe_t*)pMsg;
    pCurBuffer = (U32*)pMsgRwe->pBuffer;

    paa.all32 = pMsgRwe->paa.all32;
    pna.ch  = paa.b.ch;
    pna.dev = paa.b.ce;

    switch (pMsgRwe->hdr.b.opCode)
    {
        case cSfnNfimReadId:
            flashId = (SvcMsgVscFlashID_t *)pMsgRwe->pBuffer;
            for (ch = 0; ch < gNfInfo.numChannels; ch++)
            {
                for (ce = 0; ce < gNfInfo.devicesPerChannel; ce++)
                {
                    for (lun = 0; lun < gNfInfo.lunsPerDevice; lun++)
                    {
                        pna.ch       = ch;
                        pna.dev      = ce;
                        pna.row      = lun << gNfInfo.lunLsbPosition;
                        flashId->ch  = ch;
                        flashId->ce  = ce;
                        flashId->lun = lun;
                        if (pMsgRwe->param[0] == 0) HalNfif_ReadId(&pna, &flashId->ID[0], sizeof(flashId->ID), DIRECT);
                        else if (pMsgRwe->param[0] == 3) HalNfif_ReadId(&pna, &flashId->ID[0], sizeof(flashId->ID), TEST);
                        flashId++;
                    }
                }
            }
            break;
        case cSfnNfimReadPp:
            pna.row = (pMsgRwe->param[0] & BYTE_MASK);
            HalNfif_ReadParameterPage(&pna, pMsgRwe->pBuffer, pMsgRwe->byteCount, DIRECT);
            break;
        case cSfnNfimRead:
            pna.row = ((paa.b.lun << gNfInfo.lunLsbPosition) | (paa.b.block << gNfInfo.pairedBlkLsbPos) | (paa.b.plane << gNfInfo.planeLsbPosition/*8*/) | (paa.b.page / gNfInfo.bitsPerCell));
            pna.col = 0;
            if (pMsgRwe->param[0] != 0xA2)
            {
                pMsgRwe->param[0] = paa.b.page % gNfInfo.pagesPerProgUnit;
            }
            HalNfif_ReadPage(&pna, pMsgRwe->pBuffer, gNfInfo.dataBytesPerFullPage, DIRECT, pMsgRwe->param[0]);
            break;
        case cSfnNfimWrite:
            pna.row = ((paa.b.lun << gNfInfo.lunLsbPosition) | (paa.b.block << gNfInfo.pairedBlkLsbPos) | (paa.b.plane << gNfInfo.planeLsbPosition/*8*/) | (paa.b.page / gNfInfo.bitsPerCell));
            pna.col = 0;
            for (idx = 0; idx < (gNfInfo.dataBytesPerFullPage / gEcuInfo.auPerPage); idx++)
            {
                pCurBuffer[idx] = (U32)idx;
            }
            if (pMsgRwe->param[0] != 0xA2)
            {
                pMsgRwe->param[0] = paa.b.page % gNfInfo.pagesPerProgUnit;
            }
            HalNfif_WritePage(&pna, pMsgRwe->pBuffer, gNfInfo.dataBytesPerFullPage, DIRECT, pMsgRwe->param[0]);
            break;
        case cSfnNfimScanDefect:
            Pkg_ScanDefect();
            break;
        case cSfnNfimGetf:
            pna.row = pMsgRwe->param[0];
            feature = (SvcMsgVscFeature_t *)pMsgRwe->pBuffer;
            for (ch = 0; ch < gNfInfo.numChannels; ch++)
            {
                for (ce = 0; ce < gNfInfo.devicesPerChannel; ce++)
                {
                    pna.ch  = ch;
                    pna.dev = ce;
                    feature->ch  = ch;
                    feature->ce  = ce;
                    HalNfif_GetFeature(&pna, (uint8_t *)&pMsgRwe->param[1], DIRECT);
                    MEM_COPY((uint8_t *)&feature->param[0], (uint8_t *)&pMsgRwe->param[1], 4);
                    feature++;
                }
            }
            break;
        case cSfnNfimSetf:
            pna.row = pMsgRwe->param[0];
            for (ch = 0; ch < gNfInfo.numChannels; ch++)
            {
                for (ce = 0; ce < gNfInfo.devicesPerChannel; ce++)
                {
                    pna.ch  = ch;
                    pna.dev = ce;
                    HalNfif_SetFeature(&pna, (uint8_t *)&pMsgRwe->param[1], DIRECT);
                }
            }
            break;
        case csfnNfimGetVoltageOffset:
            feature = (SvcMsgVscFeature_t *)pMsgRwe->pBuffer;
            for (ch = 0; ch < gNfInfo.numChannels; ch++)
            {
                for (ce = 0; ce < gNfInfo.devicesPerChannel; ce++)
                {
                    for (lun = 0; lun < gNfInfo.lunsPerDevice; lun++)
                    {
                        pna.ch  = ch;
                        pna.dev = ce;
                        pna.row = lun << gNfInfo.lunLsbPosition;
                        feature->ch = ch;
                        feature->ce = ce;
                        feature->lun = lun;
                        HalNfif_GetVoltageOffset(&pna, (U8)pMsgRwe->param[0], (uint8_t *)&pMsgRwe->param[1], DIRECT);
                        MEM_COPY((uint8_t *)&feature->param[0], (uint8_t *)&pMsgRwe->param[1], 4);
                        feature++;
                    }
                }
            }
            break;
        case csfnNfimSetVoltageOffset:
            for (ch = 0; ch < gNfInfo.numChannels; ch++)
            {
                for (ce = 0; ce < gNfInfo.devicesPerChannel; ce++)
                {
                    for (lun = 0; lun < gNfInfo.lunsPerDevice; lun++)
                    {
                        pna.ch  = ch;
                        pna.dev = ce;
                        pna.row = lun << gNfInfo.lunLsbPosition;
                        HalNfif_SetVoltageOffset(&pna, (U8)pMsgRwe->param[0] ,(uint8_t *)&pMsgRwe->param[1], DIRECT);
                    }
                }
            }
            break;
        case cSfnNfimReset:
            for (ch = 0; ch < gNfInfo.numChannels; ch++)
            {
                for (ce = 0; ce < gNfInfo.devicesPerChannel; ce++)
                {
                    pna.ch  = ch;
                    pna.dev = ce;
                    HalNfif_Reset(&pna, FC_RESET, DIRECT);
                }
            }
            HalTimer_DelayUsec(NF_tRST_USEC);
            pna.ch--;
            pna.dev--;
            HalNfif_WaitForStatusSet(&pna, FC_STATUS_RDY, 1000, DIRECT);
            break;
        case cSfnNfimPkgcfg:
            gCfgInfo.reqIfData = (NfIfData_t)pMsgRwe->param[0];
            gCfgInfo.reqXferMode = (uint8_t)pMsgRwe->param[1];
            Pkg_Configure(&gCfgInfo);
            break;
        case cSfnHoldCpu1:
            MediaMon_Complete(pMsg);
            while (smHoldCpuX);
            D_PRINTF("[MMON] Unlock CPU1\n");
            return;
        default:
            pMsgRwe->error = cEcInvalidCommand;
            break;
    }

    MediaMon_Complete(pMsg);
}

//-----------------------------------------------------------------------------
/**
    Handle system service messages

    @param[in]  pMsg    message pointer
**/
//-----------------------------------------------------------------------------
static void MediaMon_HandleSysMsg(Msg_t* pMsg)
{
    switch (pMsg->hdr.b.opCode)
    {
        case cSfnDumpInfo:
            MediaMon_DumpInfo(pMsg->param[0]);
            break;
        case cSfnWakeup:
            break;
        default:
            pMsg->error = cEcInvalidCommand;
    }

    MediaMon_Complete(pMsg);
}

//-----------------------------------------------------------------------------
/**
    Handle media debug messages

    @param[in]  pMsg    message pointer
**/
//-----------------------------------------------------------------------------
static void MediaMon_HandleDbgMsg(Msg_t* pMsg)
{
    switch (pMsg->hdr.b.opCode)
    {
        case cSfnDumpFioInfo:
            FIO_DumpInfo(pMsg->param[0]);
            break;
        case cSfnDumpRRInfo:
            AplReadRetry_DumpInfo(pMsg->param[0], pMsg->param[1]);
            break;
        default:
            pMsg->error = cEcInvalidCommand;
    }

    MediaMon_Complete(pMsg);
}

//-----------------------------------------------------------------------------
/**
    Handle VSC message

    @param[in]  pMsg    message pointer
**/
//-----------------------------------------------------------------------------
static void MediaMon_HandleVscMsg(Msg_t* pMsg)
{
    SvcMsgVscFlashID_t* VscID;
    MsgPhyRwe_t*        pMsgRwe;
    Pna_t pna;
    U8    ch;
    U8    ce;

    pMsgRwe = (MsgPhyRwe_t*)pMsg;
    VscID   = (SvcMsgVscFlashID_t *)pMsgRwe->pBuffer;

    switch (pMsgRwe->hdr.b.opCode)
    {
        case cSfnVscReadIdAll:
            if(gCfgInfo.ifType == NF_JEDEC)
            {
                D_PRINTF("Media READ FLASHID\n");
                gCfgInfo.ifType = NF_JEDEC;
                for(ch = 0; ch < gNfInfo.numChannels; ch++)
                {
                    for(ce = 0; ce < gNfInfo.devicesPerChannel; ce++)
                    {
                        pna.ch  = ch;
                        pna.dev = ce;
                        pna.row = 0;
                        pna.col = 0;
                        VscID->ch = ch;
                        VscID->ce = ce;
                        HalNfif_ReadId(&pna, &VscID->ID[0], 12, DIRECT);
                        MEM_CLR(&VscID->reserved2[0], 6);
                        VscID++;
                    }
                }
            }
            break;
        default:
            pMsgRwe->error = cEcInvalidCommand;
            break;
    }

    MediaMon_Complete(pMsg);
}

//-----------------------------------------------------------------------------
/**
    Dump media information.
**/
//-----------------------------------------------------------------------------
static void MediaMon_DumpInfo(U32 param)
{
    Kernel_DumpInfo();

    M_PRINTF("[MEDIA]\n");

    M_PRINTF("[ITCM] ");
    MemAlloc_DumpAo(&itcmAllocatorAo);

    M_PRINTF("[DTCM] ");
    MemAlloc_DumpAo(&dtcmAllocatorAo);

    M_PRINTF("[STCM] ");
    MemAlloc_DumpAo(&smStcmAllocatorAo);

    M_PRINTF("[SPM]  ");
    MemAlloc_DumpAo(&smSspmAllocatorAo);

    M_PRINTF("\n");

    if (param)
    {
        AplNfif_DumpInfo();
        AplSeq_DumpInfo();
        HalNfif_DumpInfo();
        HalEcu_DumpInfo();
        HalSeq_DumpInfo();
        HalSeq_DumpGenDescriptor();
    }
}

//-----------------------------------------------------------------------------
/**
    NAND media service task startup (CPU1)
**/
//-----------------------------------------------------------------------------
void MediaMon_StartTask(void)
{
    HalIrq_RegisterHandler(cIrqIpcCommandIn, MediaMon_IpcCmdInIsr,  ENABLE);

    Kernel_InitEventFlags(EVENT_OBJ_MEDIAMON, KERNEL_NO_SPINLOCK);

    MediaMon_ServiceLoop();
}

//-----------------------------------------------------------------------------
/**
    NAND Media service loop\n
      fetch message from message queue\n
      Handle message code\n
      Poll status and Notify\n
**/
//-----------------------------------------------------------------------------
static void MediaMon_ServiceLoop(void)
{
    KernelEventFlags_t events;

    D_PRINTF("[MEDA] MonTask\n");

    while (TRUE)
    {
        events = Kernel_WaitEventFlags(EVENT_OBJ_MEDIAMON, EVENT_MEDIAMON_MSG_IN, KERNEL_NO_TIMEOUT);

        if (events & EVENT_MEDIAMON_MSG_IN)
        {
            MediaMon_HandleMsgInEvent();
        }
    }
}
