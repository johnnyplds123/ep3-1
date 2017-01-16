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

//-----------------------------------------------------------------------------
//  This is PLP.h
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Definitions: feature/function switch
//-----------------------------------------------------------------------------
#if (_GENERATION_ == GENERATION_EP3)
    #define PLP_CAPACITANCE_FROM_I2C    TRUE
#else
    #define PLP_CAPACITANCE_FROM_I2C    FALSE
#endif

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
/// @brief PLP
#define PLP_CHARGE                  TRUE
#define PLP_DISCHARGE               FALSE
#define PLP_TIMEOUT_VALUE_US        1000000 // 1s
#define PLP_CHARGE_TIMEOUT_MS       1000
#define PLP_DISCHARGE_TIMEOUT_MS    300

/// @brief PLP HW Check
#define PLP_HWCHECK_MODE_CHARGE     0x01
#define PLP_HWCHECK_MODE_C_TEST     0x02
#define PLP_HWCHECK_MODE_TEST       0x03

#define PLP_HWCHECK_OK              1
#define PLP_HWCHECK_FAIL            0

#define PLP_HWCHECK_FILTER_IC_DISCHARGE_PULSE_TH    100000  // 100ms

/// @brief PLP GPIO
#define PLP_TESTPIN_NO_C_TEST   4
#define PLP_TESTPIN_NO_ENCON    12
#define PLP_TESTPIN_NO_SPG      1
#define PLP_TESTPIN_NO_PLOSS    11
#define PLP_TESTPIN_NO_HEALTH   6
#define PLP_TESTPIN_OUTPUT      (BIT(PLP_TESTPIN_NO_C_TEST) | BIT(PLP_TESTPIN_NO_ENCON))

//-----------------------------------------------------------------------------
//  Private data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef struct
{
    U32 DisChargeTime;
    U32 ChargeTime;
} CapInfo_t;

//-----------------------------------------------------------------------------
//  Public data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#if (CO_SUPPORT_PLP == TRUE)
    #define PLP_GET_STATUS()         smPlpFlag[_CPU_]
    #define PLP_SET_STATUS(CPUID, S) {BEGIN_CS1; smPlpFlag[CPUID] = S; END_CS1;}
#else
    #define PLP_GET_STATUS()         PLP_CHARGE
    #define PLP_SET_STATUS(CPUID, S)
#endif

#define PLP_GPIO_READ_INTPUT_SPG            (HalGpio_ReadPin(PLP_TESTPIN_NO_SPG))       //((HAL_GPIO_ReadAllPin() & BIT(1)) >> 1)
#define PLP_GPIO_READ_INTPUT_C_HEALTH       (HalGpio_ReadPin(PLP_TESTPIN_NO_HEALTH))    //((HAL_GPIO_ReadAllPin() & BIT(6)) >> 6)
#define PLP_GPIO_READ_INTPUT_PLOSS          (HalGpio_ReadPin(PLP_TESTPIN_NO_PLOSS))     //(PLOSS# Power Loss)
#define PLP_GPIO_READ_OUTPUT_ENCON          (HalGpio_ReadOutPin(PLP_TESTPIN_NO_ENCON))  //((HAL_GPIO_ReadAllOutPin() & BIT(12)) >> 12)
#define PLP_GPIO_READ_OUTPUT_C_TEST         (HalGpio_ReadOutPin(PLP_TESTPIN_NO_C_TEST)) //((HAL_GPIO_ReadAllOutPin() & BIT(4)) >> 4)

#define PLP_GPIO_WRITE_OUTPUT_ENCON(value)  (HalGpio_WritePin(PLP_TESTPIN_NO_ENCON, value))
#define PLP_GPIO_WRITE_OUTPUT_C_TEST(value) (HalGpio_WritePin(PLP_TESTPIN_NO_C_TEST, value))

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t AplPlp_OneTimeInit(InitBootMode_t initMode);
U32     AplPlp_HwCheck(U32 mode, U32 parameter);

/* End of File */

