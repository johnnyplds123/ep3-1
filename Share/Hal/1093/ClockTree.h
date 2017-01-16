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
//! @brief Clock tree and frequency defintions and precompile value constraints
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Clock check macro
//-----------------------------------------------------------------------------
#define TEST_FREQ_ERROR(X,Y,P) (((100*(X))/(Y))>(100+P))


//-----------------------------------------------------------------------------
//  TBG select types
//-----------------------------------------------------------------------------
#define OSC_SEL     0   ///< Oscillator source select
#define TBG_SEL     1   ///< TBG source select

#define PLL_CLOCK   0   ///< PLL Clock is enabled.
#define PLL_BYPASS  1   ///< PLL Bypass is enabled.

//-----------------------------------------------------------------------------
//  Clock definition and validation - TBG_A, MC5, DDR=2xMC5
//-----------------------------------------------------------------------------
#define MC5_CLOCK_SET_FREQ_MHZ      MC5_CLOCK_FREQ_MHZ
#if ((TBG_A_FREQ_MHZ/4)!=(MC5_CLOCK_FREQ_MHZ))
    #error "Invalid TBG_A clock frequency, TBG_A must be 4xMC5_CLOCK_FREQ_MHZ !!!"
#endif

//-----------------------------------------------------------------------------
//  Clock definition and validation - TBG_B, CPU
//-----------------------------------------------------------------------------
#define CPU_CLOCK_SET_FREQ_MHZ      CPU_CLOCK_FREQ_MHZ
#if (TBG_B_FREQ_MHZ!=CPU_CLOCK_FREQ_MHZ)
    #error "Invalid TBG_B clock frequency, must equal to CPU !!!"
#endif

//-----------------------------------------------------------------------------
//  Clock definition and validation - TBG_C, NAND FLASH
//-----------------------------------------------------------------------------
#define NFLASH_CLOCK_SET_FREQ_MHZ   NFLASH_CLOCK_FREQ_MHZ
#if (TBG_C_FREQ_MHZ!=(NFLASH_CLOCK_SET_FREQ_MHZ*2))
    #error "Invalid TBG_C clock frequency, must be 2xNFLASH_CLOCK_SET_FREQ_MHZ !!!"
#endif

//-----------------------------------------------------------------------------
//  Clock definition and validation - TBG_D, ECC
//-----------------------------------------------------------------------------
#define ECU_CLOCK_SET_FREQ_MHZ      ECU_CLOCK_FREQ_MHZ
#if ((TBG_D_FREQ_MHZ/2)!=ECU_CLOCK_SET_FREQ_MHZ)
    #error "Invalid TBG_D clock frequency, must equal to ECU !!!"
#endif

//-----------------------------------------------------------------------------
//  Clock definition and validation - Serial Flash (SPI)
//-----------------------------------------------------------------------------
#if SFLASH_CLOCK_SRC==SRC_CPU
#define SFLASH_CLOCK_SOURCE_FREQ_MHZ    (SFLASH_CLOCK_FREQ_MHZ)
#else
    #error "Invalid serial flash clock source, source must be SRC_RAB !!!"
#endif

#ifdef SFLASH_CLOCK_DIV
#define SFLASH_SET_BAUD_RATE    (SFLASH_CLOCK_SOURCE_FREQ_MHZ/SFLASH_CLOCK_DIV)
#define SFLASH_BAUD_RATE        SFLASH_SET_BAUD_RATE
#else
#define SFLASH_CLOCK_DIV        (SFLASH_CLOCK_SOURCE_FREQ_MHZ/SFLASH_BAUD_RATE)
#define SFLASH_SET_BAUD_RATE    (SFLASH_CLOCK_SOURCE_FREQ_MHZ/SFLASH_CLOCK_DIV)
#endif

#if !(1<=SFLASH_CLOCK_DIV && SFLASH_CLOCK_DIV<=30)
    #error "Serial Flash clock divider shall be between 1 to 30 !!!"
#endif

#if ((100*SFLASH_SET_BAUD_RATE)/SFLASH_BAUD_RATE)>101
    #error "Can't make target Serial Flash baud rate less 1% difference"
#endif

