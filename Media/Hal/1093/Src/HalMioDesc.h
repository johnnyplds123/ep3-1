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
//! @brief Pure HAL API definition for MIO(Flash Input/Output) HW control
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

// Sequencer Option Value definitions
                    // CMD1(00h) + ADDR(COL+ROW) + CMD2(32h) + {WCNT61 + tPLBSY} + {SCMD(78h) + ROW}
                    // OPTION[2 : 0] = 0h (COL+ROW)
                    // OPTION[6 : 3] = 9h (WCNT61)
                    // OPTION[8 : 7] = 1h (PCNT51)
                    // OPTION[    9] = 1h (Issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 0h (Issue CMD2)
                    // OPTION[   12] = 0h (No Issue CMD(00h) before 1st CMD cycle)
                    // OPTION[14:13] = 0h (No status check & report first status)
                    // OPTION[16:15] = 0h (No issue ADDR)
                    // OPTION[   17] = 1h (No issue CMD(00h))
                    // OPTION[19:18] = 0h (No issue ADDR)
                    // OPTION[21:20] = 0h (No issue anything)
#define SEQ_OPTION_0202C8           0x0202C8

                    // CMD1(00h) + ADDR(COL+ROW) + CMD2(32h) + {WCNT61 + tPLBSY} + {SCMD(78h) + ROW}
                    // OPTION[2 : 0] = 0h (COL+ROW)
                    // OPTION[6 : 3] = 9h (WCNT61)
                    // OPTION[8 : 7] = 1h (PCNT51)
                    // OPTION[    9] = 1h (Issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 0h (Issue CMD2)
                    // OPTION[   12] = 0h (No Issue CMD(00h) before 1st CMD cycle)
                    // OPTION[14:13] = 0h (No status check & report first status)
                    // OPTION[16:15] = 0h (No issue ADDR)
                    // OPTION[   17] = 1h (No issue CMD(00h))
                    // OPTION[19:18] = 0h (No issue ADDR)
                    // OPTION[21:20] = 0h (No issue anything)
                    // OPTION[   23] = 1h (Chain with the next descriptor)
#define SEQ_OPTION_8202C8           0x8202C8
#define SEQ_OPTION_820288           0x820288
                    // CMD1(00h) + ADDR(COL+ROW) + CMD2(32h) + {WCNT61 + tPLBSY} + {SCMD(78h) + ROW}
                    // OPTION[2 : 0] = 0h (COL+ROW)
                    // OPTION[6 : 3] = 9h (WCNT61)
                    // OPTION[8 : 7] = 1h (PCNT51)
                    // OPTION[    9] = 0h (Issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 0h (Issue CMD2)
                    // OPTION[   12] = 0h (No Issue CMD(00h) before 1st CMD cycle)
                    // OPTION[14:13] = 0h (No status check & report first status)
                    // OPTION[16:15] = 0h (No issue ADDR)
                    // OPTION[   17] = 1h (No issue CMD(00h))
                    // OPTION[19:18] = 0h (No issue ADDR)
                    // OPTION[21:20] = 0h (No issue anything)
#define SEQ_OPTION_0200C8           0x0200C8
                    // CMD1(00h) + ADDR(COL+ROW) + CMD2(32h) + {WCNT61 + tPLBSY} + {SCMD(78h) + ROW}
                    // OPTION[2 : 0] = 0h (COL+ROW)
                    // OPTION[6 : 3] = 0h (WCNT61)
                    // OPTION[8 : 7] = 1h (PCNT51)
                    // OPTION[    9] = 0h (Issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 0h (Issue CMD2)
                    // OPTION[   12] = 0h (No Issue CMD(00h) before 1st CMD cycle)
                    // OPTION[14:13] = 0h (No status check & report first status)
                    // OPTION[16:15] = 0h (No issue ADDR)
                    // OPTION[   17] = 1h (No issue CMD(00h))
                    // OPTION[19:18] = 0h (No issue ADDR)
                    // OPTION[21:20] = 0h (No issue anything)
                    // OPTION[   23] = 1h (Chain with the next descriptor)
#define SEQ_OPTION_820080           0x820080
#define SEQ_OPTION_020080           0x020080
                    // CMD1(60h) + ADDR(ROW)
                    // OPTION[2 : 0] = 2h (ROW)
                    // OPTION[6 : 3] = 0h (No wait)
                    // OPTION[8 : 7] = 0h (No polling)
                    // OPTION[    9] = 0h (No issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 1h (No issue CMD2)
                    // OPTION[   12] = 0h (No Issue CMD(00h) before 1st CMD cycle)
                    // OPTION[14:13] = 0h (No status check & report first status)
                    // OPTION[16:15] = 0h (No issue ADDR)
                    // OPTION[   17] = 1h (No issue CMD(00h))
                    // OPTION[19:18] = 0h (No issue ADDR)
                    // OPTION[21:20] = 0h (No issue anything)
#define SEQ_OPTION_020802           0x020802
                    // CMD1(00h) + ADDR(COL+ROW) + CMD2(30h) + {WCNT11 + tR} + {SCMD(78h) + ROW}
                    // OPTION[2 : 0] = 0h (COL+ROW)
                    // OPTION[6 : 3] = 1h (WCNT11)
                    // OPTION[8 : 7] = 1h (PCNT51)
                    // OPTION[    9] = 1h (Issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 0h (Issue CMD2)
                    // OPTION[   12] = 0h (No Issue CMD(00h) before 1st CMD cycle)
                    // OPTION[14:13] = 0h (No status check & report first status)
                    // OPTION[16:15] = 0h (No issue ADDR)
                    // OPTION[   17] = 1h (No issue CMD(00h))
                    // OPTION[19:18] = 0h (No issue ADDR)
                    // OPTION[21:20] = 0h (No issue anything)
#define SEQ_OPTION_020288           0x020288
                    // CMD1(60h) + ADDR(ROW) + CMD2(30h) + {WCNT11 + tR} + SCMD(F1h/F2h)
                    // OPTION[2 : 0] = 2h (ROW)
                    // OPTION[6 : 3] = 1h (WCNT11)
                    // OPTION[8 : 7] = 1h (PCNT51)
                    // OPTION[    9] = 0h (No issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 0h (Issue CMD2)
                    // OPTION[   12] = 0h (No Issue CMD(00h) before 1st CMD cycle)
                    // OPTION[14:13] = 0h (No status check & report first status)
                    // OPTION[16:15] = 0h (No issue ADDR)
                    // OPTION[   17] = 1h (No issue CMD(00h))
                    // OPTION[19:18] = 0h (No issue ADDR)
                    // OPTION[21:20] = 0h (No issue anything)
#define SEQ_OPTION_020088           0x020088
                    // {CMD(05h) + ADDR(COL) + CMD(E0h)} + DIN
                    // OPTION[2 : 0] = 3h (No issue ADDR)
                    // OPTION[6 : 3] = 0h (No wait)
                    // OPTION[8 : 7] = 0h (No polling)
                    // OPTION[    9] = 0h (No issue ROW for SCMD)
                    // OPTION[   10] = 1h (No issue CMD1)
                    // OPTION[   11] = 1h (No issue CMD2)
                    // OPTION[   12] = 0h (No Issue CMD(00h) before 1st CMD cycle)
                    // OPTION[14:13] = 0h (No status check & report first status)
                    // OPTION[16:15] = 0h (No issue ADDR)
                    // OPTION[   17] = 1h (No issue CMD(00h))
                    // OPTION[19:18] = 0h (Issue COL+ROW)
                    // OPTION[21:20] = 1h (Issue 05h/COL/E0h)
#define SEQ_OPTION_120C03           0x120C03
                    // CMD(00h) + ADDR(COL+ROW) + {CMD(05h) + ADDR(COL) + CMD(E0h)} + DIN
                    // OPTION[2 : 0] = 3h (No issue ADDR)
                    // OPTION[6 : 3] = 0h (No wait)
                    // OPTION[8 : 7] = 0h (No polling)
                    // OPTION[    9] = 0h (No issue ROW for SCMD)
                    // OPTION[   10] = 1h (No issue CMD1)
                    // OPTION[   11] = 1h (No issue CMD2)
                    // OPTION[   12] = 0h (No Issue CMD(00h) before 1st CMD cycle)
                    // OPTION[14:13] = 0h (No status check & report first status)
                    // OPTION[16:15] = 0h (No issue ADDR)
                    // OPTION[   17] = 0h (Issue CMD(00h))
                    // OPTION[19:18] = 1h (Issue COL+ROW)
                    // OPTION[21:20] = 1h (Issue 05h/COL/E0h)
#define SEQ_OPTION_940C03           0x940C03
                    // {CMD(06h) + ADDR(COL+ROW) + CMD(E0h)} + DIN
                    // OPTION[2 : 0] = 3h (No issue ADDR)
                    // OPTION[6 : 3] = 0h (No wait)
                    // OPTION[8 : 7] = 0h (No polling)
                    // OPTION[    9] = 0h (No issue ROW for SCMD)
                    // OPTION[   10] = 1h (No issue CMD1)
                    // OPTION[   11] = 1h (No issue CMD2)
                    // OPTION[   12] = 0h (No Issue CMD(00h) before 1st CMD cycle)
                    // OPTION[14:13] = 0h (No status check & report first status)
                    // OPTION[16:15] = 0h (No issue ADDR)
                    // OPTION[   17] = 1h (No issue CMD(00h))
                    // OPTION[19:18] = 0h (No issue ADDR)
                    // OPTION[21:20] = 2h (Issue 06h/COL+ROW/E0h)
#define SEQ_OPTION_220C03           0x220C03
                    // CMD1(60h) + ADDR(ROW)
                    // OPTION[2 : 0] = 2h (ROW)
                    // OPTION[6 : 3] = 0h (No wait)
                    // OPTION[8 : 7] = 0h (No polling)
                    // OPTION[    9] = 0h (No issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 1h (No issue CMD2)
#define SEQ_OPTION_000842           0x000842
                    // CMD1(80h) + ADDR(COL+ROW)
                    // OPTION[2 : 0] = 0h (COL+ROW)
                    // OPTION[6 : 3] = 0h (No waiting)
                    // OPTION[8 : 7] = 0h (No polling)
                    // OPTION[    9] = 0h (No Issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 1h (No Issue CMD2)
#define SEQ_OPTION_400800           0x400800
                    // CMD1(85h) + TCWAW + ADDR(COL+ROW) + CMD2(11h) + {WCNT61 + tPLBSY}  + {SCMD(78h) + ROW}
                    // OPTION[2 : 0] = 0h (COL+ROW)
                    // OPTION[6 : 3] = 9h (WCNT61)
                    // OPTION[8 : 7] = 2h (PCNT52)
                    // OPTION[    9] = 1h (Issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 0h (Issue CMD2)
                    // OPTION[   22] = 1h (Wait based on param_tcwaw)
#define SEQ_OPTION_400118           0x400118
                    // CMD1(00h) + TCWAW + ADDR(COL+ROW) + CMD2(30h) + {WCNT11 + tR} + SCMD(F1h/F2h) + CMD(00h) + DIN
                    // OPTION[2 : 0] = 0h (COL+ROW)
                    // OPTION[6 : 3] = 1h (WCNT11)
                    // OPTION[8 : 7] = 1h (PCNT51)
                    // OPTION[    9] = 0h (No issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 0h (Issue CMD2)
                    // OPTION[   12] = 0h (No Issue CMD(00h) before 1st CMD cycle)
                    // OPTION[14:13] = 0h (No status check & report first status)
                    // OPTION[16:15] = 0h (No issue ADDR)
                    // OPTION[   17] = 0h (Issue CMD(00h))
                    // OPTION[19:18] = 0h (No issue ADDR)
                    // OPTION[21:20] = 0h (No issue anything)
                    // OPTION[   22] = 1h (Wait based on param_tcwaw)
#define SEQ_OPTION_400148           0x400148
                    // CMD1(60h) + TCWAW + ADDR(ROW) + CMD2(D0h) + {WCNT31 + tBER} + SCMD(F1/F2h)
                    // OPTION[2 : 0] = 0h (COL+ROW)
                    // OPTION[6 : 3] = 5h (WCNT31)
                    // OPTION[8 : 7] = 2h (PCNT52)
                    // OPTION[    9] = 0h (No issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 0h (Issue CMD2)
                    // OPTION[   22] = 1h (Wait based on param_tcwaw)
#define SEQ_OPTION_000800           0x000800
                    // CMD1(80h) + ADDR(COL+ROW) + CMD2(11h) + {WCNT61 + tPLBSY}  + {SCMD(78h) + ROW}
                    // OPTION[2 : 0] = 0h (COL+ROW)
                    // OPTION[6 : 3] = 9h (WCNT61)
                    // OPTION[8 : 7] = 2h (PCNT52)
                    // OPTION[    9] = 1h (Issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 0h (Issue CMD2)
#define SEQ_OPTION_000348           0x000348
                    // CMD1(85h) + ADDR(COL) + CMD2(11h) + {WCNT61 + tPLBSY} + {SCMD(78) + ROW}
                    // OPTION[2 : 0] = 1h (COL only)
                    // OPTION[6 : 3] = 9h (WCNT61)
                    // OPTION[8 : 7] = 2h (PCNT52)
                    // OPTION[    9] = 1h (Issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 0h (Issue CMD2)
#define SEQ_OPTION_000349           0x000349
                    // CMD1(60h) + ADDR(ROW) + CMD2(D1h) + {WCNT61 + tPLBSY} + {SCMD(78h) + ROW}
                    // OPTION[2 : 0] = 2h (ROW)
                    // OPTION[6 : 3] = 9h (WCNT61)
                    // OPTION[8 : 7] = 2h (PCNT52)
                    // OPTION[    9] = 1h (Issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 0h (Issue CMD2)
#define SEQ_OPTION_00034A           0x00034A
                    // CMD1(60h) + ADDR(ROW) + CMD2(D0h) + {WCNT31 + tBER} + {SCMD(78h) + ROW}
                    // OPTION[2 : 0] = 2h (ROW)
                    // OPTION[6 : 3] = 5h (WCNT31)
                    // OPTION[8 : 7] = 2h (PCNT52)
                    // OPTION[    9] = 1h (Issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 0h (Issue CMD2)
#define SEQ_OPTION_00032A           0x00032A
                    // CMD1(80h) + ADDR(COL+ROW) + CMD2(10h) + {WCNT21 + tPROG} + {SCMD(78) + ROW}
                    // OPTION[2 : 0] = 0h (COL+ROW)
                    // OPTION[6 : 3] = 3h (WCNT21)
                    // OPTION[8 : 7] = 2h (PCNT52)
                    // OPTION[    9] = 1h (Issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 0h (Issue CMD2)
#define SEQ_OPTION_000318           0x000318
                    // CMD1(85h) + ADDR(COL) + CMD2(10h) + {WCNT21 + tPROG} + {SCMD(78) + ROW}
                    // OPTION[2 : 0] = 1h (COL only)
                    // OPTION[6 : 3] = 3h (WCNT21)
                    // OPTION[8 : 7] = 2h (PCNT52)
                    // OPTION[    9] = 1h (Issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 0h (Issue CMD2)
#define SEQ_OPTION_000319           0x000319
                    // CMD1(00h) + ADDR(COL+ROW) + CMD2(30h) + {WCNT11 + tR} + {SCMD(78h) + ROW} + CMD(00h) + DIN
                    // OPTION[2 : 0] = 0h (COL+ROW)
                    // OPTION[6 : 3] = 1h (WCNT11)
                    // OPTION[8 : 7] = 1h (PCNT51)
                    // OPTION[    9] = 1h (Issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 0h (Issue CMD2)
                    // OPTION[   12] = 0h (No Issue CMD(00h) before 1st CMD cycle)
                    // OPTION[14:13] = 0h (No status check & report first status)
                    // OPTION[16:15] = 0h (No issue ADDR)
                    // OPTION[   17] = 0h (Issue CMD(00h))
                    // OPTION[19:18] = 0h (No issue ADDR)
                    // OPTION[21:20] = 0h (No issue anything)
#define SEQ_OPTION_000288           0x000288
                    // CMD1(80h) + ADDR(COL+ROW) + CMD2(11h) + {WCNT61 + tPLBSY} + SCMD(F1h/F2h)
                    // OPTION[2 : 0] = 0h (COL+ROW)
                    // OPTION[6 : 3] = 9h (WCNT61)
                    // OPTION[8 : 7] = 2h (PCNT52)
                    // OPTION[    9] = 0h (No issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 0h (Issue CMD2)
#define SEQ_OPTION_000100           0x000100
                    // CMD1(60h) + ADDR(ROW) + CMD2(D0h) + {WCNT31 + tBER} + SCMD(F1/F2h)
                    // OPTION[2 : 0] = 0h (COL+ROW)
                    // OPTION[6 : 3] = 0h (No waiting)
                    // OPTION[8 : 7] = 2h (PCNT52)
                    // OPTION[    9] = 0h (No issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 0h (Issue CMD2)
#define SEQ_OPTION_008108           0x008108
    //Prefix1B+  CMD1(80h) + ADDR(COL+ROW) + CMD2(1Ah) + {WCNT11 + tR} + SCMD(F1h/F2h) + CMD(00h) + DIN
                    // OPTION[2 : 0] = 0h (COL+ROW)
                    // OPTION[6 : 3] = 1h (WCNT11)
                    // OPTION[8 : 7] = 2h (PCNT52)
                    // OPTION[    9] = 0h (No issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 0h (Issue CMD2)
                    // OPTION[   12] = 0h (No Issue CMD(00h) before 1st CMD cycle)
                    // OPTION[14:13] = 0h (No status check & report first status)
                    // OPTION[16:15] = 1h (issue 1B prefix)
                    // OPTION[   17] = 0h (Issue CMD(00h))
                    // OPTION[19:18] = 0h (No issue ADDR)
                    // OPTION[21:20] = 0h (No issue anything)
#define SEQ_OPTION_008000           0x008000
    //Prefix1B+  CMD1(80h) + ADDR(COL+ROW) + CMD2(1Ah) + {WCNT11 + tR} + SCMD(F1h/F2h) + CMD(00h) + DIN
                    // OPTION[2 : 0] = 0h (COL+ROW)
                    // OPTION[6 : 3] = 0h (No WCNT)
                    // OPTION[8 : 7] = 0h (No PCNT)
                    // OPTION[    9] = 0h (No issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 0h (Issue CMD2)
                    // OPTION[   12] = 0h (No Issue CMD(00h) before 1st CMD cycle)
                    // OPTION[14:13] = 0h (No status check & report first status)
                    // OPTION[16:15] = 1h (issue 1B prefix)
                    // OPTION[   17] = 0h (Issue CMD(00h))
                    // OPTION[19:18] = 0h (No issue ADDR)
                    // OPTION[21:20] = 0h (No issue anything)
#define SEQ_OPTION_000002           0x000002
                    // CMD1(60h) +  ADDR(ROW) + CMD2(D0h)
#define SEQ_OPTION_800882           0x800882
                   //Chain Next Descriptor + CMD1(60h) +  ADDR(ROW) + CMD2(D0h) + PCNT51
#define SEQ_OPTION_00812A           0x00812A

#define SEQ_OPTION_00012A           0x00012A
                   //CMD1(60h) +  ADDR(ROW) + CMD2(D0h) + WCNT31 + PCNT52
#define SEQ_OPTION_00812A           0x00812A
                   // PREDIX(A2) + CMD1(60h) +  ADDR(ROW) + CMD2(D0h) + WCNT31 + PCNT52
#define SEQ_OPTION_808802           0x808802
                   //Chain Next Descriptor +  PREDIX(A2) + CMD1(60h) +  ADDR(ROW)

#define SEQ_OPTION_008118           0x008118
                    // CMD1(80h) + WCNT 21 PCNT52 1B Prefix
#define SEQ_OPTION_008100           0x008100
                    // CMD1(80h) + PCNT52 1B Prefix
#define SEQ_OPTION_008080           0x008080
                    // CMD1(60h) + ADDR(ROW) + CMD2(D0h) + {WCNT31 + tBER} + SCMD(F1/F2h)
                    // OPTION[2 : 0] = 0h (COL+ROW)
                    // OPTION[6 : 3] = 0h (No waiting)
                    // OPTION[8 : 7] = 1h (PCNT51)
                    // OPTION[    9] = 0h (No issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 0h (Issue CMD2)
                    // OPTION[   15] = 1h (Issue PreCMD)
#define SEQ_OPTION_800802           0x800802
#define SEQ_OPTION_000118           0x000118
                    // CMD1(00h) + ADDR(COL+ROW) + CMD2(30h) + {WCNT11 + tR} + SCMD(F1h/F2h) + CMD(00h) + DIN
                    // OPTION[2 : 0] = 0h (COL+ROW)
                    // OPTION[6 : 3] = 1h (WCNT11)
                    // OPTION[8 : 7] = 2h (PCNT52)
                    // OPTION[    9] = 0h (No issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 0h (Issue CMD2)
                    // OPTION[   12] = 0h (No Issue CMD(00h) before 1st CMD cycle)
                    // OPTION[14:13] = 0h (No status check & report first status)
                    // OPTION[16:15] = 0h (No issue ADDR)
                    // OPTION[   17] = 0h (Issue CMD(00h))
                    // OPTION[19:18] = 0h (No issue ADDR)
                    // OPTION[21:20] = 0h (No issue anything)
#define SEQ_OPTION_000088           0x000088
                    // CMD1(85h) + ADDR(COL) + CMD2(10h) + {WCNT21 + tPROG} + SCMD(F1h/F2h)
                    // OPTION[2 : 0] = 1h (COL only)
                    // OPTION[6 : 3] = 3h (WCNT21)
                    // OPTION[8 : 7] = 2h (PCNT52)
                    // OPTION[    9] = 0h (No Issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 0h (Issue CMD2)
#define SEQ_OPTION_000119           0x000119
                    // CMD1(EEh) + ADDR + DATA
                    // OPTION[2 : 0] = 4h (Issue the LSB of column address only)
                    // OPTION[6 : 3] = 8h (WCNT11)
                    // OPTION[8 : 7] = 0h (PCNT51)
                    // OPTION[    9] = 0h (No issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 1h (Do not issue CMD2)
                    // OPTION[   12] = 0h (Reserved)
                    // OPTION[14:13] = 0h (No status check & report first status)
                    // OPTION[16:15] = 0h (No Prefix)
                    // OPTION[   17] = 0h (Do not issue CMD(00h))   //@@
                    // OPTION[19:18] = 0h (No issue ADDR)
                    // OPTION[21:20] = 0h (No issue anything)
#define SEQ_OPTION_00084C           0x00084C

#define SEQ_OPTION_140C03           0x140C03
                    // {CMD(00h) + ADDR(00h+ROW) + CMD(05h) + ADDR(COL) + CMD(E0h)}
                    // OPTION[2 : 0] = 3h (No issue ADDR)
                    // OPTION[6 : 3] = 0h (No wait)
                    // OPTION[8 : 7] = 0h (No polling)
                    // OPTION[    9] = 0h (No issue ROW for SCMD)
                    // OPTION[   10] = 1h (No issue CMD1)
                    // OPTION[   11] = 1h (No issue CMD2)
                    // OPTION[   12] = 0h (No Issue CMD(00h) before 1st CMD cycle)
                    // OPTION[14:13] = 0h (No status check & report first status)
                    // OPTION[16:15] = 0h (No issue ADDR)
                    // OPTION[   17] = 0h (Issue CMD(00h))
                    // OPTION[19:18] = 2h (Issue ADDR(000h+ROW))
                    // OPTION[21:20] = 1h (Issue 05h/COL/E0h)
#define SEQ_OPTION_328100           0x328100
                    // Prefix 1B + NOWCNT + PCNT52 + Issue 05 + COL/ROW +E0
                    // OPTION[   17] = 1h (No Issue CMD(00h))

#define SEQ_OPTION_328148           0x328148
                    // Prefix 1B + WCNT61 + PCNT52 + Issue 05 + COL/ROW +E0
                    // OPTION[   17] = 1h (No Issue CMD(00h))
#define SEQ_OPTION_320C03           0x320C03
                    // OPTION 05 E0

                    // OPTION[2 : 0] = 3h No issue Addr between CMD1 CMD2
                    // OPTION[6 : 3] = 0h NO WCNT
                    // OPTION[8 : 7] = 0h NO polling
                    // OPTION[    9] = 0h (No issue ROW for SCMD)
                    // OPTION[   10] = 1h (NO issue CMD1)
                    // OPTION[   11] = 1h (NO issue CMD2)
                    // OPTION[   12] = 0h Reserved
                    // OPTION[14:13] = 0h **(No status check & report first status)
                    // OPTION[16:15] = 0h (NO  Prefix)
                    // OPTION[   17] = 1h (No Issue CMD(00h))
                    // OPTION[19:18] = 0h (NoIssue ADDR(00h+ROW))
                    // OPTION[21:20] = 3h ( Issue 05h/COLRow/E0h)
#define SEQ_OPTION_3280C8           0x3280C8
                    //  Prefix1B   00 30 SCMD 05 E0
                    // OPTION[2 : 0] = 0h (COL+ROW)
                    // OPTION[6 : 3] = 9h (WCNT61)
                    // OPTION[8 : 7] = 1h ( polling PCNT51 )
                    // OPTION[    9] = 0h (No issue ROW for SCMD)
                    // OPTION[   10] = 0h ( issue CMD1)
                    // OPTION[   11] = 0h ( issue CMD2)
                    // OPTION[   12] = 0h Reserved
                    // OPTION[14:13] = 0h **(No status check & report first status)
                    // OPTION[16:15] = 1h (1B Prefix)
                    // OPTION[   17] = 1h (No Issue CMD(00h))
                    // OPTION[19:18] = 0h (NoIssue ADDR(00h+ROW))
                    // OPTION[21:20] = 3h ( Issue 05h/COLRow/E0h)
#define SEQ_OPTION_0008C2           0x0008C2
                    // CMD1 + ROW + WCNT+PCNT
#define SEQ_OPTION_3208C3           0x3208C3
                    //    CMD1 + WCNT+PCNT + 05 COL RWO EO
                    // OPTION[2 : 0] = 3h no addr between cmd1 cmd2
                    // OPTION[6 : 3] = 1h (WCNT
                    // OPTION[8 : 7] = 1h ( polling PCNT51 )
                    // OPTION[    9] = 0h (No issue ROW for SCMD)
                    // OPTION[   10] = 0h ( issue CMD1)
                    // OPTION[   11] = 1h ( no issue CMD2)
                    // OPTION[   12] = 0h Reserved
                    // OPTION[14:13] = 0h **(No status check & report first status)
                    // OPTION[16:15] = 0h (no 1B Prefix)
                    // OPTION[   17] = 1h (No Issue CMD(00h))
                    // OPTION[19:18] = 0h (NoIssue ADDR(00h+ROW))
                    // OPTION[21:20] = 3h ( Issue 05h/COLRow/E0h)

#define SEQ_OPTION_140080           0x140080
                    // PCNT51 + NOWCNT
#define SEQ_OPTION_1400C8           0x1400C8
                    // PCNT51 + WCNT61
#define SEQ_OPTION_828080           0x828080
                    // Prefix1B 00 32 NO WCNT PCNT51
#define SEQ_OPTION_020148           0x020148
                    // 00 30 WCNT61 PCNT52
#define SEQ_OPTION_028148           0x028148
                    // Prefix1B 00 30 WCNT61 PCNT52
#define SEQ_OPTION_0280C8           0x0280C8
                    //  Prefix1B   00 30 SCMD

                    // OPTION[2 : 0] = 0h (COL+ROW)
                    // OPTION[6 : 3] = 9h (WCNT61)
                    // OPTION[8 : 7] = 1h ( polling PCNT51 )
                    // OPTION[    9] = 0h (No issue ROW for SCMD)
                    // OPTION[   10] = 0h ( issue CMD1)
                    // OPTION[   11] = 0h ( issue CMD2)
                    // OPTION[   12] = 0h Reserved
                    // OPTION[14:13] = 0h **(No status check & report first status)
                    // OPTION[16:15] = 1h (1B Prefix)
                    // OPTION[   17] = 1h (No Issue CMD(00h))
                    // OPTION[19:18] = 0h (NoIssue ADDR(00h+ROW))
                    // OPTION[21:20] = 0h (No Issue 05h/COL/E0h)
#define SEQ_OPTION_148148           0x148148
                   // PREFIX 1B + WCNT61 + PCNT52
                   // OPTION[19:18] = 1h (Issue ADDR(COL+ROW))
                   // OPTION[21:20] = 1h (Issue 05h/COL/E0h)
#define SEQ_OPTION_1480C8           0x1480C8
#define SEQ_OPTION_148C03           0x148C03

#define SEQ_OPTION_0208D9           0x0208D9
                    // WCNT71 + PCNT51
#define SEQ_OPTION_020859           0x020859
                    // PCNT1B + WVNT71

#define SEQ_OPTION_02A148           0x02A148

#define SEQ_OPTION_820100           0x820100

#define SEQ_OPTION_828100           0x828100
                    //Prefix 1B 00 32 PCNT52 23 Chain
                    // OPTION[   17] = 1h (No issue CMD(00h))
#define SEQ_OPTION_020881           0x020881
                    // CMD1(D5h) + ADDR(COL)
                    // OPTION[2 : 0] = 1h (COW)
                    // OPTION[6 : 3] = 0h (No WCNT)
                    // OPTION[8 : 7] = 1h (PCNT51)
                    // OPTION[    9] = 0h (No issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 1h (No issue CMD2)
                    // OPTION[   12] = 0h (No Issue CMD(00h) before 1st CMD cycle)
                    // OPTION[14:13] = 0h (No status check & report first status)
                    // OPTION[16:15] = 0h (No issue ADDR)
                    // OPTION[   17] = 1h (No issue CMD(00h))
                    // OPTION[19:18] = 0h (No issue ADDR)
                    // OPTION[21:20] = 0h (No issue anything)

#define SEQ_OPTION_0220C8           0x0220C8
                    // CMD1(00h) + ADDR(COL+ROW) + CMD2(32h) + {WCNT61 + PCNT51}
                    // OPTION[2 : 0] = 0h (COL+ROW)
                    // OPTION[6 : 3] = 9h (WCNT61)
                    // OPTION[8 : 7] = 1h (PCNT51)
                    // OPTION[    9] = 0h (No issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 0h (Issue CMD2)
                    // OPTION[   12] = 0h (No Issue CMD(00h) before 1st CMD cycle)
                    // OPTION[14:13] = 0h (No status check & report first status)
                    // OPTION[16:15] = 0h (No issue ADDR)
                    // OPTION[   17] = 1h (No issue CMD(00h))
                    // OPTION[19:18] = 0h (No issue ADDR)
                    // OPTION[21:20] = 0h (No issue anything)

#define SEQ_OPTION_022148           0X022148
                    // CMD1(00h) + ADDR(COL+ROW) + CMD2(32h) + {WCNT61 + tPLBSY}
                    // OPTION[2 : 0] = 0h (COL+ROW)
                    // OPTION[6 : 3] = 9h (WCNT61)
                    // OPTION[8 : 7] = 0h (No polling)s
                    // OPTION[    9] = 0h (No issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 0h (Issue CMD2)
                    // OPTION[   12] = 0h (No Issue CMD(00h) before 1st CMD cycle)
                    // OPTION[14:13] = 0h (No status check & report first status)
                    // OPTION[16:15] = 0h (No issue ADDR)
                    // OPTION[   17] = 1h (No issue CMD(00h))
                    // OPTION[19:18] = 0h (No issue ADDR)
                    // OPTION[21:20] = 0h (No issue anything)
#define SEQ_OPTION_820048           0x820048
                    // CMD1(00h) + ADDR(COL+ROW) + CMD2(32h) + {WCNT61 + tPLBSY} + {SCMD(78h) + ROW}
                    // OPTION[2 : 0] = 0h (COL+ROW)
                    // OPTION[6 : 3] = 9h (WCNT61)
                    // OPTION[8 : 7] = 1h (PCNT51)
                    // OPTION[    9] = 1h (Issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 0h (Issue CMD2)
                    // OPTION[   12] = 0h (No Issue CMD(00h) before 1st CMD cycle)
                    // OPTION[14:13] = 0h (No status check & report first status)
                    // OPTION[16:15] = 0h (No issue ADDR)
                    // OPTION[   17] = 1h (No issue CMD(00h))
                    // OPTION[19:18] = 0h (No issue ADDR)
                    // OPTION[21:20] = 0h (No issue anything)
#define SEQ_OPTION_020949           0x020949
                    // CMD1(D5h) + ADDR(COL)
                    // OPTION[6 : 3] = 9h (WCNT61)
                    // OPTION[8 : 7] = 2h (PCNT52)
                    // OPTION[    9] = 0h (No issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 1h (No issue CMD2)
                    // OPTION[   12] = 0h (No Issue CMD(00h) before 1st CMD cycle)
                    // OPTION[14:13] = 0h (No status check & report first status)
                    // OPTION[16:15] = 0h (No issue ADDR)
                    // OPTION[   17] = 1h (No issue CMD(00h))
                    // OPTION[19:18] = 0h (No issue ADDR)
                    // OPTION[21:20] = 0h (No issue anything)
#define SEQ_OPTION_020919           0x020919
                    // CMD1(D5h) + ADDR(COL)
                    // OPTION[2 : 0] = 1h (COW)
                    // OPTION[6 : 3] = 3h (WCNT21)
                    // OPTION[8 : 7] = 2h (PCNT52)
                    // OPTION[    9] = 0h (No issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 1h (No issue CMD2)
                    // OPTION[   12] = 0h (No Issue CMD(00h) before 1st CMD cycle)
                    // OPTION[14:13] = 0h (No status check & report first status)
                    // OPTION[16:15] = 0h (No issue ADDR)
                    // OPTION[   17] = 1h (No issue CMD(00h))
                    // OPTION[19:18] = 0h (No issue ADDR)
                    // OPTION[21:20] = 0h (No issue anything)
#define SEQ_OPTION_000803            0x000803
                    // CMD1(26h)
                    // OPTION[2 : 0] = 3h (No issue ADDR)
                    // OPTION[6 : 3] = 0h (No wait)
                    // OPTION[8 : 7] = 0h (No polling)
                    // OPTION[    9] = 0h (No issue ROW for SCMD)
                    // OPTION[   10] = 0h (Issue CMD1)
                    // OPTION[   11] = 1h (No issue CMD2)

#define SEQ_OPTION_208CB            0x0208CB
                    //  CMD(FF) + polling
                    // OPTION[2 : 0] = 3h (No issue ADDR)
                    // OPTION[6 : 3] = 9h (WCNT 61)
                    // OPTION[8 : 7] = 1h (PCNT51)
                    // OPTION[    9] = 0h (No issue ROW for SCMD)
                    // OPTION[   10] = 0h ( issue CMD1)
                    // OPTION[   11] = 1h (No issue CMD2)
                    // OPTION[   12] = 0h Reserved
                    // OPTION[14:13] = 0h **(No status check & report first status)
                    // OPTION[16:15] = 0h (No Prefix)
                    // OPTION[   17] = 1h (No Issue CMD(00h))
                    // OPTION[19:18] = 0h (NoIssue ADDR(00h+ROW))
                    // OPTION[21:20] = 0h (No Issue 05h/COL/E0h)
//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
