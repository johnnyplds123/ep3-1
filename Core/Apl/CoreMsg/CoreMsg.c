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
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"
#include "Fio.h"
#include "CoreMsg.h"

#include "FastSection.h"
//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
tCMSG_INFO gCoreMsgInfo;
Msg_t      gCoreExtendMsg[CMSG_EXT_MSG_COUNT];

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
    Initial core msg

    @param[in]  initMode

    @return cEcNoError
**/
//-----------------------------------------------------------------------------
Error_t CMSG_OneTimeInit(InitBootMode_t initMode)
{
    U32 idx;

    MEM_CLR(&gCoreMsgInfo,  sizeof(tCMSG_INFO));
    MEM_CLR(gCoreExtendMsg, sizeof(gCoreExtendMsg));

    for (idx = 0; idx < CMSG_EXT_MSG_COUNT; idx++)
    {
        gCoreExtendMsg[idx].hdr.b.status = cMsgCompleted;
    }

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    get extend core msg
**/
//-----------------------------------------------------------------------------
Msg_t* CMSG_GetExtendMsg(uint32_t opCode)
{
    Msg_t *pMsg = NULL;

    switch (opCode)
    {
        case cMcSysInfo:
            if (gCoreExtendMsg[CMSG_EXT_IDX_SYSINFO].hdr.b.status != cMsgExecuted)
            {
                pMsg = &gCoreExtendMsg[CMSG_EXT_IDX_SYSINFO];
            }
            else
            {
                pMsg = &gCoreExtendMsg[CMSG_EXT_IDX_SYSINFO1];
            }

            if (pMsg->hdr.b.status != cMsgCompleted)    return pMsg;
            break;
        case cMcResetCache:
            if (gCoreExtendMsg[CMSG_EXT_IDX_ZERO_CACHE].hdr.b.status == cMsgCompleted)
            {
                pMsg = &gCoreExtendMsg[CMSG_EXT_IDX_ZERO_CACHE];
            }
            else
            {
                return NULL;
            }
            break;
        default:
            ASSERT(0);
            break;
    }

    MSG_SET_EXTEND_INIT(pMsg, opCode, TRUE);
    pMsg->hdr.b.prty = MSG_MAX_PRIORITY_NUM;

    return pMsg;
}

//-----------------------------------------------------------------------------
/**
    Dump core msg information
**/
//-----------------------------------------------------------------------------
void CMSG_DumpInfo(void)
{
    Msg_t* pMsg;
    U16 idx;
    U16 pool;

    D_PRINTF("\n[CORE MSG]\n");

    for (pool = 0; pool < CMSG_POOL_COUNT; pool++)
    {
        D_PRINTF("P[%d] C[%d]\n", pool, gCoreMsgInfo.msgCnt[pool]);

        pMsg = gCoreMsgInfo.head[pool];
        for (idx = 0; idx < gCoreMsgInfo.msgCnt[pool]; idx++)
        {
            D_PRINTF("%2d: OP[%d] ST[%02X] PRI[%d]\n", idx, pMsg->hdr.b.opCode, pMsg->hdr.b.status, pMsg->hdr.b.prty);
            pMsg = pMsg->next;
        }
    }
}
