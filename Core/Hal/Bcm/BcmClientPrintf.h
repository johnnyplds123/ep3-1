
#pragma once
// ============================================================================
//
//     Copyright (c) Marvell Corporation 2000-2014  -  All rights reserved
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
//! @brief Header file BcmClientPrintf.h
//!
//=============================================================================

#define TEST_END

#define simplePrintf    D_PRINTF    //cj added

#define PRINTF_TUBE_ENABLE  1    //0
// #define PRINTF_TUBE_ENABLE  1


#if (PRINTF_TUBE_ENABLE == 1)
    #define  printf_tube   simplePrintf

    __inline void write_tube_dword(U32 v)
    {
        simplePrintf("%x\n", v);
    }

    #define write_tube      printf_tube
#else
    __inline void printf_tube(char *pt, ...)
    {
        // Stab, do nothing.
    }

    __inline void write_tube_dword(U32 v)
    {
    }

    #define write_tube      printf_tube
#endif

#define bit(x)      (1 << (x))


// A simple printf fuction that prints string and hex number only.
// All numbers are treated as 4-byte width.
//void simplePrintf(char *fmt, ...);



