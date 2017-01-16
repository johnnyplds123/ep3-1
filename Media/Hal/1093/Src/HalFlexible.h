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
//! @brief Pure HAL API definition for FLC(Flash Link Controller) HW control
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
#define    MIN_EXECYC_RRT  20  //CLKSPD_200MHZ_2
#define    MIN_EXECYC_2B   20  // CLKSPD_200MHZ_2
#define    CLE_H         (0x01 << 0)
#define    CLE_L         (0x00 << 0)
#define    ALE_H         (0x01 << 1)
#define    ALE_L         (0x00 << 1)
#define    WEB_H         (0x01 << 2)
#define    WEB_L         (0x00 << 2)
#define    REB_H         (0x01 << 3)
#define    REB_L         (0x00 << 3)
#define    DQS_H         (0x01 << 4)
#define    DQS_L         (0x00 << 4)
#define    CEB_DES       (0x01 << 5)
#define    CEB_DIS       (0x00 << 5)
#define    DQ_DES        (0x01 << 6)
#define    DQ_TBL        (0x00 << 6)
#define    DQ(x)         ((x) << 7)
#define    DQ_IN         (0x01 << 11)
#define    DQ_OUT        (0x00 << 11)
#define    GOLATCH       (0x01 << 12)
#define    NOLATCH       (0x00 << 12)
#define    EXECYC(x)     ((x) << 13)
#define    END_H         (0x01 << 19)
#define    END_L         (0x00 << 19)

#define    NUMBER_OF_TBL_ELEMENTS(x)    ((sizeof(x))/(sizeof(x[0])))

#define    FD_H      (0x01 << 7)
#define    FD_L      (0x00 << 7)
#define    LD_H      (0x01 << 6)
#define    LD_L      (0x00 << 6)
#define    SDLVL_P   (0x00 << 5)
#define    SDLVL_N   (0x01 << 5)
#define    RDCMD_YES (0x00 << 4)
#define    RDCMD_NO  (0x01 << 4)
#define    EC_H      (0x01 << 3)
#define    EC_L      (0x00 << 3)
#define    BM_H      (0x01 << 2)
#define    BM_L      (0x00 << 2)

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
void HalFlexible_OneTimeInit(void);


