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
//! @brief Inter Processor Communication code
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"

#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define CB_WRITE(Q,M)   CbWriteU32(Q,(uint32_t)M)  ///< circular buffer write
#define CB_READ(Q,M)    CbReadU32(Q,M,true)        ///< circular buffer read
#define CB_PEEK(Q,M,F)  CbReadU32(Q,M,F)

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------
extern CirBuf_t smMsgQueue[];   ///< message queue
extern Msg_t    smMsgEntry[];   ///< message entry

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
static const uint8_t msgQueueTargetCpu[] = // see MsgQueueId_t
{
    CPU_ID1,    // cH2mReqQueue
    CPU_ID0,    // cM2hComQueue
    CPU_ID2,    // cH2cReqQueue
    CPU_ID0,    // cC2hComQueue
    CPU_ID1,    // cC2mReqQueue
    CPU_ID2,    // cM2cComQueue
    CPU_ID2,    // cH2cCacheRelQueue
    CPU_ID2,    // cH2cUartQueue
    CPU_ID1,    // cC2mOtherQueue
};

#if _CPU_==CPU_ID0    // CPU0 only
//-----------------------------------------------------------------------------
/**
    One time initialization for IPC queue.
      - Init Host to Media queue
      - Init Media to Host queue

    @param[in]  initMode        initialization mode
**/
//-----------------------------------------------------------------------------
void Ipc_OneTimeInit(InitBootMode_t initMode)
{
    void* pMsgEntry;

    // allocate memory for Host to Media queue entries
    pMsgEntry = MEM_AllocStcm(sizeof(MsgHandle_t) * H2M_REQ_QUEUE_DEPTH, 4);
    CbInit(&smMsgQueue[cH2mReqQueue], pMsgEntry, H2M_REQ_QUEUE_DEPTH, sizeof(MsgHandle_t));

    // allocate memory for Media to Host completion queue entries
    pMsgEntry = MEM_AllocStcm(sizeof(MsgHandle_t) * M2H_COM_QUEUE_DEPTH, 4);
    CbInit(&smMsgQueue[cM2hComQueue], pMsgEntry, M2H_COM_QUEUE_DEPTH, sizeof(MsgHandle_t));

    // allocate memory for Host to Core queue entries
    pMsgEntry = MEM_AllocStcm(sizeof(MsgHandle_t) * H2C_REQ_QUEUE_DEPTH, 4);
    CbInit(&smMsgQueue[cH2cReqQueue], pMsgEntry, H2C_REQ_QUEUE_DEPTH, sizeof(MsgHandle_t));

    // allocate memory for Core to Host completion queue entries
    pMsgEntry = MEM_AllocStcm(sizeof(MsgHandle_t) * C2H_COM_QUEUE_DEPTH, 4);
    CbInit(&smMsgQueue[cC2hComQueue], pMsgEntry, C2H_COM_QUEUE_DEPTH, sizeof(MsgHandle_t));

    // allocate memory for Core to Media queue entries
    pMsgEntry = MEM_AllocStcm(sizeof(MsgHandle_t) * C2M_REQ_QUEUE_DEPTH, 4);
    CbInit(&smMsgQueue[cC2mReqQueue], pMsgEntry, C2M_REQ_QUEUE_DEPTH, sizeof(MsgHandle_t));

    // allocate memory for Media to Core completion queue entries
    pMsgEntry = MEM_AllocStcm(sizeof(MsgHandle_t) * M2C_COM_QUEUE_DEPTH, 4);
    CbInit(&smMsgQueue[cM2cComQueue], pMsgEntry, M2C_COM_QUEUE_DEPTH, sizeof(MsgHandle_t));

    pMsgEntry = MEM_AllocStcm(sizeof(MsgHandle_t) * H2C_CACHE_QUEUE_DEPTH, 4);
    CbInit(&smMsgQueue[cH2cCacheRelQueue], pMsgEntry, H2C_CACHE_QUEUE_DEPTH, sizeof(MsgHandle_t));

    pMsgEntry = MEM_AllocStcm(sizeof(MsgHandle_t) * H2C_UART_QUEUE_DEPTH, 4);
    CbInit(&smMsgQueue[cH2cUartQueue], pMsgEntry, H2C_UART_QUEUE_DEPTH, sizeof(MsgHandle_t));

    pMsgEntry = MEM_AllocStcm(sizeof(MsgHandle_t) * H2C_UART_QUEUE_DEPTH, 4);
    CbInit(&smMsgQueue[cC2mOtherQueue], pMsgEntry, H2C_UART_QUEUE_DEPTH, sizeof(MsgHandle_t));

    smCacheRelMsgIdx = 0;
}

//-----------------------------------------------------------------------------
/**
    Reset a extend messag from opCode

    @param[in]   opCode
    @return      message pointer
**/
//-----------------------------------------------------------------------------
Msg_t* Ipc_GetExtendMsg(uint32_t opCode)
{
    Msg_t *pMsg;

    switch (opCode)
    {
        case cMcIdleFlush:
            pMsg = &smHcmdMsg[HMSG_EXT_IDX_IDLE_FLUSH];
            if (pMsg->hdr.b.status == cMsgCompleted)
            {
                MSG_SET_EXTEND_INIT(pMsg, opCode, TRUE);
                return pMsg;
            }
            else
            {
                return NULL;
            }
        case cMcSysInfo:
            pMsg = &smHcmdMsg[HMSG_EXT_IDX_SYSINFO];

            break;
        case cMcSmartOp:
            pMsg = &smHcmdMsg[HMSG_EXT_IDX_SMART];

            break;
        case cMcVendor:
            pMsg = &smHcmdMsg[HMSG_EXT_IDX_VSC];

            break;
        case cMcTcg:
            pMsg = &smHcmdMsg[HMSG_EXT_IDX_TCG];
            MEM_CLR(pMsg, sizeof(pMsg));

            break;
        case cMcShutdown:
            pMsg = &smHcmdMsg[HMSG_EXT_IDX_SHUTDOWN];
            pMsg->hdr.b.prty = MSG_MAX_PRIORITY_NUM;

            return pMsg;
        case cMcHostSetFeature:
            pMsg = &smHcmdMsg[HMSG_EXT_IDX_SET_FEATURE];
            MEM_CLR(pMsg, sizeof(pMsg));
            break;
        case cMcPurge:
            pMsg = &smHcmdMsg[HMSG_EXT_IDX_PURGE];
            MEM_CLR(pMsg, sizeof(pMsg));
            break;
        case cMcHostReset:
            pMsg = &smHcmdMsg[HMSG_EXT_IDX_HOSTRESET];
            if (pMsg->hdr.b.status == cMsgCompleted)
            {
                MSG_SET_EXTEND_INIT(pMsg, opCode, FALSE);
                pMsg->hdr.b.prty = MSG_MAX_PRIORITY_NUM;
                return pMsg;
            }
            else
            {
				ASSERT(0);
                return NULL;
            }
        default:
            ASSERT(0);
            break;
    }

    MSG_SET_EXTEND_INIT(pMsg, opCode, TRUE);
    pMsg->hdr.b.prty = MSG_MAX_PRIORITY_NUM;

    return pMsg;
}
#endif

//-----------------------------------------------------------------------------
/**
    Return number of message count in IPC message queue

    @param[in]   msgQueueId  IPC message queue Id

    @return  message count in message queue
**/
//-----------------------------------------------------------------------------
uint32_t Ipc_GetCount(MsgQueueId_t msgQueueId)
{
    return CbGetCount(&smMsgQueue[msgQueueId]);
}

//-----------------------------------------------------------------------------
/**
    Return TRUE if IPC message queue is empty

    @param[in]   msgQueueId  IPC message queue Id

    @return  TRUE buffer empty, else not
**/
//-----------------------------------------------------------------------------
bool Ipc_IsEmpty(MsgQueueId_t msgQueueId)
{
    return CbIsEmpty(&smMsgQueue[msgQueueId]);
}

//-----------------------------------------------------------------------------
/**
    Return TRUE if IPC message queue is full

    @param[in]   msgQueueId  IPC message queue Id

    @return  TRUE buffer full, else not
**/
//-----------------------------------------------------------------------------
bool Ipc_IsFull(MsgQueueId_t msgQueueId)
{
    return CbIsFull(&smMsgQueue[msgQueueId]);
}

//-----------------------------------------------------------------------------
/**
    Get a message from IPC Queue.\n
    wait until message queue is filled.

    @param[in]   msgQueueId  message queue Id
    @param[in]   timeOutUsec time out in micro-second unit

    @return  message handle
**/
//-----------------------------------------------------------------------------
MsgHandle_t Ipc_GetMsg(MsgQueueId_t msgQueueId, uint32_t timeOutUsec)
{
    MsgHandle_t hMsg;

    //$todo need RTOS wait procedure for CPU1 when queue is empty
    while (CB_READ(&smMsgQueue[msgQueueId], &hMsg)!=cEcNoError);

    return(hMsg);
}

//-----------------------------------------------------------------------------
/**
    Peek a message from IPC Queue

    @param[in]   msgQueueId  message queue Id
    @param[in]   remove      Messages are removed from the queue

    @return  message handle if message peeking is success, else NULL
**/
//-----------------------------------------------------------------------------
MsgHandle_t Ipc_PeekMsg(MsgQueueId_t msgQueueId, bool remove)
{
    MsgHandle_t hMsg=NULL;

    CB_PEEK(&smMsgQueue[msgQueueId], &hMsg, remove);

    return (hMsg);
}

//-----------------------------------------------------------------------------
/**
    Put Message to IPC Queue.\n

    @param[in]   msgQueueId  message queue Id
    @param[in]   hMsg        message handle
    @param[in]   timeOutUsec time out in micro-second unit

    @return  cEcNoError if success, else error
**/
//-----------------------------------------------------------------------------
Error_t Ipc_PutMsg(MsgQueueId_t msgQueueId, MsgHandle_t hMsg, uint32_t timeOutUsec)
{
    uint32_t cpuId;

    //$todo need RTOS wait procedure for CPU0 when queue is full
    while (CB_WRITE(&smMsgQueue[msgQueueId],hMsg)!=cEcNoError);

    cpuId = msgQueueTargetCpu[msgQueueId];
    HalIpc_WriteCmdTo(cpuId, cIpcCmdMsgIn, 0, 0);

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Completion of requested message

    @param[in]   hMsg        message handle
    @param[in]   timeOutUsec time out in micro-second unit

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t Ipc_CompleteMsg(MsgHandle_t hMsg, uint32_t timeOutUsec)
{
    Msg_t* pMsg = MSG_GET_PTR_FROM_HANDLE(hMsg, Msg_t);

    pMsg->hdr.b.status = cMsgExecuted;

    return(cEcNoError);
}

#if _CPU_!=CPU_ID1    // CPU0 & CPU2 only
//-----------------------------------------------------------------------------
/**
    Wait for message completion

    @param[in]   hMsg        message handle
    @param[out]  pCompMsg    completion message pointer
    @param[in]   timeOutUsec time out in micro-second unit

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t Ipc_WaitCompletion(MsgHandle_t hMsg, Msg_t* pCompMsg, uint32_t timeOutUsec)
{
    Msg_t* pMsg = MSG_GET_PTR_FROM_HANDLE(hMsg, Msg_t);

    while(pMsg->hdr.b.status != cMsgExecuted); //wait for message execution done

    MEM_COPY(pCompMsg, pMsg, sizeof(Msg_t));

    Ipc_ReleaseSharedMem((void* *)pMsg);

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Send a message

    @param[in]   msgQueueId  message queue Id
    @param[in]   pReqMsg     message pointer to request
    @param[in]   timeOutUsec time out in micro-second unit

    @return  message handle
**/
//-----------------------------------------------------------------------------
MsgHandle_t Ipc_SendMsg(MsgQueueId_t msgQueueId, Msg_t* pReqMsg, uint32_t timeOutUsec)
{
    Msg_t* pMsg;

    pMsg = (Msg_t *)Ipc_AllocSharedMem(msgQueueId, sizeof(Msg_t));

    MEM_COPY(pMsg, pReqMsg, sizeof(Msg_t));

    pMsg->hdr.b.status = cMsgPosted;
    Ipc_PutMsg(msgQueueId, (MsgHandle_t)pMsg, timeOutUsec);

    return((MsgHandle_t)pMsg);
}

//-----------------------------------------------------------------------------
/**
    Send a message and wait for completion

    @param[in]   msgQueueId  message queue Id
    @param[in]   pReqMsg     message pointer to request
    @param[out]  pCompMsg    completion message pointer
    @param[in]   timeOutUsec time out in micro-second unit

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t Ipc_SendWaitMsg(MsgQueueId_t msgQueueId, Msg_t* pReqMsg, Msg_t* pCompMsg, uint32_t timeOutUsec)
{
    MsgHandle_t hMsg;

    hMsg = Ipc_SendMsg(msgQueueId, pReqMsg, timeOutUsec);
    return Ipc_WaitCompletion(hMsg, pCompMsg, timeOutUsec);
}

//-----------------------------------------------------------------------------
/**
    Alloc a message queue for IPC.

    @param[in]   size   byte size to alloc

    @return allocated message pointer
**/
//-----------------------------------------------------------------------------
void* Ipc_AllocSharedMem(MsgQueueId_t msgQueueId, uint32_t size)
{
    // Use static Buffer instead of dynamic allocation
    return &smMsgEntry[msgQueueId];
}

//-----------------------------------------------------------------------------
/**
    Release a message queue

    param[in]   pBuffer    message pointer
**/
//-----------------------------------------------------------------------------
void Ipc_ReleaseSharedMem(void* pBuffer)
{

}
#endif
