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
//! @brief Pure HAL API definition for FIO(Flash Input/Output) HW control
//!
//=============================================================================
#include "AplDesc.h"

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
#define MAX_READRETRY_ROW       0x0E  //Read Retry Voltage Max Level
#define MAX_READRETRY_COL       0x04
#define MAX_READRETRY_SLC       0x00

#if (READ_LEVEL_CHANGE == ENABLE)
    #if (_GENERATION_ == GENERATION_EP2)
        #define L_VALUE  0x00
        #define A_VALUE  0xFC
        #define B_VALUE  0xFB
        #define C_VALUE  0xFC
    #else
        #define L_VALUE  0x00
        #define A_VALUE  0xFC
        #define B_VALUE  0xFC
        #define C_VALUE  0xFC
    #endif
#endif

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
