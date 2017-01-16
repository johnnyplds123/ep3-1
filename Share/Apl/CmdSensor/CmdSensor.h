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
//! @brief Header file for Media sub-system
//!
//=============================================================================

#if _CPU_==CPU_ID0
    #include "HostBase.h"
#endif

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define READ_CMD_TOLERANCE      8
#define WRITE_CMD_TOLERANCE     8

typedef enum
{
    CMD_NONE=0,
    CMD_SEQU_READ,
    CMD_RAND_READ,
    CMD_SEQU_WRITE,
    CMD_RAND_WRITE,
    CMD_MODE_COUNT,
    CMD_MODE_NULL=0x7FFFFFFF
}tCMD_SENSOR_MODE;

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef struct
{
    U32  lastReadLBA;
    U32  lastWriteLBA;

    U32  accumSequR;
    U32  accumRandR;
    tCMD_SENSOR_MODE rCmdMode;

    U32  accumSequW;
    U32  accumRandW;
    tCMD_SENSOR_MODE wCmdMode;

    U32  lastPreReadLAA;
    U32  hitReadLAA;
}CmdSensor_t;

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern CmdSensor_t smCmdSensor;

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
void AplCmdSensor_OneTimeInit(InitBootMode_t initMode);
void AplCmdSensor_DumpInfo(void);

//-----------------------------------------------------------------------------
//  Inline Function
//-----------------------------------------------------------------------------

#if _CPU_==CPU_ID0
//-----------------------------------------------------------------------------
/**
    Detect read command sequence

    @param[in]  pHcmdQ      Host cmd
**/
//-----------------------------------------------------------------------------
__inline void AplCmdSensor_ReadCmdDetect(HcmdQ_t* pHcmdQ)
{
    if (smCmdSensor.lastReadLBA == pHcmdQ->addrInfo.lba)
    {
        if (smCmdSensor.accumSequR != READ_CMD_TOLERANCE)
        {
            smCmdSensor.rCmdMode = CMD_NONE;
            smCmdSensor.accumSequR++;
            if (smCmdSensor.accumSequR == READ_CMD_TOLERANCE)
            {
                smCmdSensor.rCmdMode = CMD_SEQU_READ;
            }
        }

        smCmdSensor.accumRandR = 0;
    }
    else
    {
        if (smCmdSensor.accumRandR != READ_CMD_TOLERANCE)
        {
            smCmdSensor.rCmdMode = CMD_NONE;
            smCmdSensor.accumRandR++;

            if (smCmdSensor.accumRandR == READ_CMD_TOLERANCE)
            {
                smCmdSensor.rCmdMode = CMD_RAND_READ;
            }
        }

        smCmdSensor.accumSequR = 0;
    }

    smCmdSensor.lastReadLBA = pHcmdQ->addrInfo.lba + pHcmdQ->addrInfo.nb;
}

//-----------------------------------------------------------------------------
/**
    Detect write command sequence

    @param[in]  pHcmdQ      Host cmd
**/
//-----------------------------------------------------------------------------
__inline void AplCmdSensor_WriteCmdDetect(HcmdQ_t* pHcmdQ)
{
    if (smCmdSensor.lastWriteLBA == pHcmdQ->addrInfo.lba)
    {
        if (smCmdSensor.accumSequW != WRITE_CMD_TOLERANCE)
        {
            smCmdSensor.wCmdMode = CMD_NONE;
            smCmdSensor.accumSequW++;
            if (smCmdSensor.accumSequW == WRITE_CMD_TOLERANCE)
            {
                smCmdSensor.wCmdMode = CMD_SEQU_WRITE;
            }
        }

        smCmdSensor.accumRandW = 0;
    }
    else
    {
        if (smCmdSensor.accumRandW != WRITE_CMD_TOLERANCE)
        {
            smCmdSensor.wCmdMode = CMD_NONE;
            smCmdSensor.accumRandW++;
            if (smCmdSensor.accumRandW == WRITE_CMD_TOLERANCE)
            {
                smCmdSensor.wCmdMode = CMD_RAND_WRITE;
            }
        }

        smCmdSensor.accumSequW = 0;
    }

    smCmdSensor.lastWriteLBA = pHcmdQ->addrInfo.lba + pHcmdQ->addrInfo.nb;
}
#endif
