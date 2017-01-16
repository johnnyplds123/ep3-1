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
//! @brief NAND Flash I/O High Level Driver API definition
//!
//=============================================================================
//-----------------------------------------------------------------------------
//  include header files:
//-----------------------------------------------------------------------------
#include "Fio.h"

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
#define CMSG_EXT_MSG_COUNT 8

typedef enum
{
    CMSG_EXT_IDX_SYSINFO = 0,
    CMSG_EXT_IDX_SYSINFO1,
    CMSG_EXT_IDX_ZERO_CACHE,
    CMSG_EXT_IDX_MAX
}tCMSG_EXT_IDX;
COMPILE_ASSERT(CMSG_EXT_IDX_MAX < CMSG_EXT_MSG_COUNT, "CMSG_EXT_IDX_MAX > CMSG_EXT_MSG_COUNT");

typedef enum
{
    CMSG_POOL_WAIT = 0,
    CMSG_POOL_COUNT,
}tCMSG_POOL;

typedef enum
{
    CMSG_CMD_COMPLETE=0,
    CMSG_CMD_POSTPONE,
    CMSG_CMD_HALT,
    CMSG_CMD_LOOKAHEAD,
}tCMSG_CMD_STATUS;

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef struct
{
    Msg_t* head[CMSG_POOL_COUNT];
    Msg_t* tail[CMSG_POOL_COUNT];
    U32    msgCnt[CMSG_POOL_COUNT];
}tCMSG_INFO;

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern tCMSG_INFO gCoreMsgInfo;

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t CMSG_OneTimeInit(InitBootMode_t initMode);
Msg_t*  CMSG_GetExtendMsg(uint32_t opCode);
void    CMSG_DumpInfo(void);

//-----------------------------------------------------------------------------
//  Inline function
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    push core msg into tail

    @param[in]  msg pool
    @param[in]  pointer of msg
**/
//-----------------------------------------------------------------------------
__inline void CMSG_PushTail(tCMSG_POOL pool, Msg_t* pMsg)
{
    if (gCoreMsgInfo.tail[pool] == NULL)
    {
        gCoreMsgInfo.head[pool] = gCoreMsgInfo.tail[pool] = pMsg;
    }
    else
    {
        gCoreMsgInfo.tail[pool]->next = pMsg;
        gCoreMsgInfo.tail[pool]       = pMsg;
    }

    gCoreMsgInfo.msgCnt[pool]++;
}

//-----------------------------------------------------------------------------
/**
    push core msg into head

    @param[in]  msg pool
    @param[in]  pointer of msg
**/
//-----------------------------------------------------------------------------
__inline void CMSG_PushHead(tCMSG_POOL pool, Msg_t* pMsg)
{
    if (gCoreMsgInfo.head[pool] == NULL)
    {
        gCoreMsgInfo.head[pool] = gCoreMsgInfo.tail[pool] = pMsg;
    }
    else
    {
        pMsg->next              = gCoreMsgInfo.head[pool];
        gCoreMsgInfo.head[pool] = pMsg;
    }

    gCoreMsgInfo.msgCnt[pool]++;
}

//-----------------------------------------------------------------------------
/**
    pop core msg from head

    @param[in]  msg pool

    @return pointer of msg
**/
//-----------------------------------------------------------------------------
__inline Msg_t* CMSG_PopHead(tCMSG_POOL pool)
{
    Msg_t* pMsg = NULL;

    if (gCoreMsgInfo.msgCnt[pool])
    {
        pMsg = gCoreMsgInfo.head[pool];

        if (pMsg == gCoreMsgInfo.tail[pool])
        {
            gCoreMsgInfo.head[pool] = gCoreMsgInfo.tail[pool] = NULL;
        }
        else
        {
            gCoreMsgInfo.head[pool] = pMsg->next;
        }

        pMsg->next = NULL;

        gCoreMsgInfo.msgCnt[pool]--;
    }

    return pMsg;
}
