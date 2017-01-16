#pragma once

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------

//#define Rlevel_DEBUG
//#define EraseSet_DEBUG

void    TSB_1Z_MLC_RlevelChange     (U8 Lvalue, U8 Avalue, U8 Bvalue, U8 Cvalue, U8  ch, U8  ce, U8  lun, BOOL display);
void    TSB_1Z_MLC_EraseSetting     (U8  ch, U8  ce, U8  lun);
void    TSB_1Z_MLC_CCT3Setting      (U8  ch, U8  ce, U8  lun);
void    TSB_1Z_MLC_Die_Reset        (U8  ch, U8  ce, U8  lun, BOOL display);
void    TSB_1Z_MLC_EraseLoop_Relax  (U8  ch, U8  ce, U8  lun);

