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
//! @brief Header file for Serial montior.
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
#define MONBUFFER_SIZE (128 * KBYTE)
//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
// monitor command handler function prototype
typedef Error_t  (*MonCmdFunc_t)(Cstr_t , uint32_t, uint32_t*);

/// @brief monitor command decoding table
typedef struct
{
    char* cmdStr;           ///< command string
    char* helpStr;          ///< command help string
    uint8_t  minArgc;       ///< minimum argument count
    uint8_t  autoDec;       ///< automatic decode argument (number only)
    uint8_t  exeTimePrint;  ///< print command execution time
    MonCmdFunc_t pFunc;     ///< function pointer to service command
} MonCmdTbl_t;

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern void* gMonBuffer;     ///< generic test buffer

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
void    Mon_StartTask(void);
void    Mon_UartRxIsr(uint32_t reserved);
void    Mon_PrintCmdList(void);
void    Mon_PrintPrompt(void);
Error_t Mon_RegisterCmdList(const MonCmdTbl_t* pTable);

