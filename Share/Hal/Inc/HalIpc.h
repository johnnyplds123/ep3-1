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
//! @brief Pure HAL API definition  for IPC HW control
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
typedef enum
{
    cIpcPmActive=1,        ///< Active mode - highest power consumption state
    cIpcPmDeepSleep=15     ///< Deep sleep mode - Lowest power consumption state
} IpcPowerMode_t;

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t HalIpc_OneTimeInit(InitBootMode_t initMode);
void    HalIpc_OneTimeInitForStartCpu(void);
void    HalIpc_StartCpu(uint32_t cpuId);
void    HalIpc_JumpToAddress(uint32_t cpuId, uint32_t address);
Error_t HalIpc_SetPowerMode(IpcPowerMode_t powerMode);
void    HalIpc_ReadCmdFrom(uint16_t cpuId, uint16_t* pCmd, uint16_t* pwParam, uint32_t* plParam);
void    HalIpc_WriteCmdTo(uint16_t cpuId, uint16_t cmd, uint16_t wParam, uint32_t lParam);

