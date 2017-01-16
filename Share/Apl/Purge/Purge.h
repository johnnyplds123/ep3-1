#pragma once
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
//
//! @file
//! @brief description for example code
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  include header files:
//-----------------------------------------------------------------------------
#include "SysTypes.h"
#include "BuildCommon.h"
//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------

//Purge BitMap
#define PURGE_SI_MISC          B_0
#define PURGE_SI_PLSTCNT       B_1
#define PURGE_SI_TCG           B_2
#define PURGE_SI_MPINFO        B_3
#define PURGE_SI_NVME          B_4
#define PURGE_SI_LOGINFO       B_5
#define PURGE_SI_AGING         B_6
#define PURGE_SI_GLIST         B_7
#define PURGE_SI_UNCT          B_8
#define PURGE_SI_NVEC          B_9
#define PURGE_SI_FTL           B_10    //A
#define PURGE_SI_ECNT          B_11
#define PURGE_SI_PLIST1        B_12
#define PURGE_SI_PLIST2        B_13
#define PURGE_SI_FTLRMP        B_14
#define PURGE_SI_AGINGEI       B_15    //F

#define PURGE_LOGINFO_ERR      B_17
#define PURGE_LOGINFO_SMART    B_18

#define PURGE_NOR_FW           B_24
#define PURGE_NOR_HWINFO       B_25
#define PURGE_NOR_QUICKIDX     B_26    //1A
#define PURGE_NOR_SYINBU       B_27
#define PURGE_NOR_TCG          B_28
#define PURGE_NOR_FTLOVERRIDE  B_29

#define PURGE_CORE_FTL         B_32    //20
#define PURGE_CORE_UNCT        B_33

#define PURGE_NAND_SYSTEM_W    B_34
#define PURGE_NAND_SYSTEM_WO   B_35

#define PURGE_MASK_NOR         (PURGE_NOR_FW         | \
                                PURGE_NOR_HWINFO     | \
                                PURGE_NOR_QUICKIDX   | \
                                PURGE_NOR_SYINBU     | \
                                PURGE_NOR_TCG        | \
                                PURGE_NOR_FTLOVERRIDE)

#define PURGE_MASK_CORE         PURGE_CORE_FTL
#define PURGE_MASK_NAND        (PURGE_NAND_SYSTEM_W | PURGE_NAND_SYSTEM_WO)
#define PURGE_MASK_CPU0        (PURGE_MASK_NOR | PURGE_CORE_UNCT)

#define PURGE_PKG_CLEAR_DISK    PURGE_MASK_NOR
#define PURGE_PKG_CLEAR_SI      PURGE_NOR_QUICKIDX

#define PURGE_NOR_SPLITCNT    6
#define PURGE_NOR_START       24
//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief Purge error
typedef enum {
    PURGE_ERR_NON    = 0,
    PURGE_ERR_FAIL,
    PURGE_ERR_NULL   = 0x7FFFFFFF
} tPG_ERR;

typedef struct {
    U16     dwStartSector; //  4B
    U16     dwSectorCnt;   //  4B
} tPG_NORTABLE;

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------

tPG_ERR Purge_Center(U64 PurgeBitMap);

//-----------------------------------------------------------------------------
//  Inline function reference
//-----------------------------------------------------------------------------


/* End of File */
