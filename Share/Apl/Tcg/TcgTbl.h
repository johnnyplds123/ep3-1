// ============================================================================
//
//     Copyright (c) LITEON Corporation  -  All rights reserved
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
//! @brief Inter Processor Communication code
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define TCG_CFG_BLK_CNT_PER_GRP     128                         //128 bolcks for L9 , 128 blocks is for M6/M3
#define TCG_MBR_CELLS               TCG_CFG_BLK_CNT_PER_GRP     // Alta+ = 128 blocks , M3/M6=128 blocks

#define SP_LOCKING_IDX  1           //UID_SP_Locking row index

#define CPIN_SID_IDX    0           //CPIN_SID row index
#define CPIN_MSID_IDX   1           //CPIN_MSID row index
#if _TCG_ != TCG_PYRITE
  #define CPIN_PSID_IDX 3           //CPIN_PSID row index
#else
  #define CPIN_PSID_IDX 2
#endif
#define LCK_CPIN_ADMIN1_IDX     0   //Lck CPIN_Admin1_IDX
#define SI_LOCKING_RANGE_CNT    8   //total locking range: 9 (=1+8, including global range)



//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef struct
{
    U8     SMBRDone;
    U8     DummyZZ;
    U8     RdLocked[SI_LOCKING_RANGE_CNT+1];  // 8+1 ranges
    U8     WrLocked[SI_LOCKING_RANGE_CNT+1];  // 8+1 ranges
} TcgBkDevSlpDef;

typedef struct
{
    U32         TCG_EEPROM_PROG_IDX;                    // 0x0000
    U8          TCG_DEV_TYPE;                           // 0x0004
    U8          TCG_RSV_ZONE[11];                       // 0x0005
    char        TcgDefect_ID[16];                       // 0x0010
    U8          TcgG4NorCellDft[TCG_MBR_CELLS];         // 0x0020
    U8          TcgG5NorCellDft[TCG_MBR_CELLS];         // 0x00A0
    char        TcgErasedCnt_ID[16];                    // 0x0120
    U16         TcgG4NorBlkErasedCnt[TCG_MBR_CELLS];    // 0x0130
    U16         TcgG5NorBlkErasedCnt[TCG_MBR_CELLS];    // 0x0230
    char        Tcg_m_ID_Text[4];                       // 0x0330
    U32         m_ID;                                   // 0x0334
    char        Tcg_m_Ver_Text[4];                      // 0x0338
    U32         m_Ver;                                  // 0x033C
    char        Tcg_c_ID_Text[4];                       // 0x0340
    U32         c_ID;                                   // 0x0344
    char        Tcg_c_Ver_Text[4];                      // 0x0348
    U32         c_Ver;                                  // 0x034C
    char        Preformat_ID[16];                       // 0x0350
    TcgBkDevSlpDef  TcgBkDevSlpVar;                     // 0x0360
    U8          Tcg_E2P_RSV[2*KBYTE-0x374];             // 0x0374
}tSYSINFO_TCG;



//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Public function prototype definitions:
//-----------------------------------------------------------------------------






