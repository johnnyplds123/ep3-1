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
//! @brief AHCI I/F protocol service code
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"
#include "CmdSensor.h"

#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING
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
/**
    Command Sensor initialization

    @param[in]  initMode : init mode
**/
//-----------------------------------------------------------------------------
void AplCmdSensor_OneTimeInit(InitBootMode_t initMode)
{
    MEM_CLR(&smCmdSensor, sizeof(smCmdSensor));
    smCmdSensor.lastWriteLBA    = DWORD_MASK;
    smCmdSensor.lastReadLBA     = DWORD_MASK;
    smCmdSensor.hitReadLAA      = DWORD_MASK;
    smCmdSensor.lastPreReadLAA  = DWORD_MASK;
    smCmdSensor.hitReadLAA      = DWORD_MASK;
}

//-----------------------------------------------------------------------------
/**
    Dump Command Sensor Information
**/
//-----------------------------------------------------------------------------
void AplCmdSensor_DumpInfo(void)
{
    D_PRINTF("R/W Mode: %d/%d\n", smCmdSensor.rCmdMode, smCmdSensor.wCmdMode);
}
