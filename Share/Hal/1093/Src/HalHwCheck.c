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

//-----------------------------------------------------------------------------
//  This is HalHwCheck.c
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"

#include "RegApb.h"
#include "HalGpio.h"
#include "HalIrq.h"
#include "HalIrqSrc.h"

#include "MemIo.h"
#include "PLP.h"

#include "HalHwCheck.h"

#include "NormSection.h"
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
    HalHwCheck_Main

    @param[in] component  mode  parameter
**/
//-----------------------------------------------------------------------------
U32 HalHwCheck_Main(HalHwCheckComponent_t component, U32 op, U32 parameter)
{
    #if (_CPU_== CPU_ID0)
    MsgService_t msg;

    MSG_SET_SERVICE_INIT(msg, cSfnHwCheck);

    msg.param[0] = component;
    msg.param[1] = op;
    msg.param[2] = parameter;

    Ipc_SendWaitMsg(cH2cUartQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);

    return msg.param[3];

    #elif (_CPU_ == CPU_ID2)
    U32 outPut = 0;

    switch (component)
    {
        case HAL_HWCHECK_COMPONENT_PLP:
            #if (CO_SUPPORT_PLP == TRUE)
            outPut = AplPlp_HwCheck(op, parameter);
            #endif
            break;
        default:
            break;
    }
    return outPut;
    #endif
}

/* End of File */

