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
//! \file
//! @brief Register configuration code Header file
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------

/// @brief register configuration op code definition
#define RCC_NO_OP                 0       ///< no operation
#define RCC_SET_BASE_OP           1       ///< set registger base address

#define RCC_READ_OP               2       ///< read register (16bit)
#define RCC_WRITE_OP              3       ///< write register (16bit)
#define RCC_BIT_SET_OP            4       ///< Bit set register (16bit)
#define RCC_BIT_CLR_OP            5       ///< Bit clear register (16bit)
#define RCC_FIELD_WRITE_OP        6       ///< Bit field write register (16bit)
#define RCC_WAIT_FOR_OP           7       ///< Wait for desired value set with bit mask (16bit)

#define RCC_READ32_OP             8       ///< read register (32bit)
#define RCC_WRITE32_OP            9       ///< write register (32bit)
#define RCC_BIT_SET32_OP          10      ///< Bit set register (32bit)
#define RCC_BIT_CLR32_OP          11      ///< Bit clear register (32bit)
#define RCC_FIELD_WRITE32_OP      12      ///< Bit field write register (32bit)
#define RCC_WAIT_FOR32_OP         13      ///< Wait for desired value set with bit mask (32bit)

#define RCC_DELAY_OP              14      ///< Delay Usecond
#define RCC_RETURN_OP             15      ///< return to caller

#define RCI_NO_TIMEOUT            U32_MAX ///< No time out

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define RCI_32BIT(V)                (((uint32_t)V)&0xFFFF), (((uint32_t)V)>>16)

/// @brief register configuration code instruction definition
#define RCI_SET_BASE(V)             RCC_SET_BASE_OP|((V)&0xFFF0), ((V)>>16)&0xFFFF

/// @brief 16bit register access instructions (12bit address offset xxxh)
#define RCI_READ(R)                 RCC_READ_OP|((R)<<4)
#define RCI_WRITE(R,V)              RCC_WRITE_OP|((R)<<4),       (V)
#define RCI_BIT_SET(R,V)            RCC_BIT_SET_OP|((R)<<4),     (V)
#define RCI_BIT_CLR(R,V)            RCC_BIT_CLR_OP|((R)<<4),     (V)
#define RCI_FIELD_WRITE(R,V,M)      RCC_FIELD_WRITE_OP|((R)<<4), (V), (M)
#define RCI_WAIT_FOR(R,V,M,T)       RCC_WAIT_FOR_OP|((R)<<4),    (V), (M), RCI_32BIT(T)

/// @brief 32bit register access instructions (12bit address offset xxxh)
#define RCI_READ32(R)               RCC_READ32_OP|((R)<<4)
#define RCI_WRITE32(R,V)            RCC_WRITE32_OP|((R)<<4),        RCI_32BIT(V)
#define RCI_BIT_SET32(R,V)          RCC_BIT_SET32_OP|((R)<<4),      RCI_32BIT(V)
#define RCI_BIT_CLR32(R,V)          RCC_BIT_CLR32_OP|((R)<<4),      RCI_32BIT(V)
#define RCI_FIELD_WRITE32(R,V,M)    RCC_FIELD_WRITE32_OP|((R)<<4),  RCI_32BIT(V), RCI_32BIT(M)
#define RCI_WAIT_FOR32(R,V,M,T)     RCC_WAIT_FOR32_OP|((R)<<4),     RCI_32BIT(V), RCI_32BIT(M), RCI_32BIT(T)

/// @brief delay instruction (max 268sec, 28bits)
#define RCI_DELAY_USEC(T)           RCC_DELAY_OP|((T)<<4), ((T)>>16)&0xFFFF

#define RCI_EXIT                    RCC_RETURN_OP


//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef uint16_t RegConfigInst_t;

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
uint32_t HalRci_Execute(const RegConfigInst_t* pInst, uint32_t baseAddr);
Error_t HalRci_WaitValueU16(uint32_t address, uint16_t testValue, uint16_t bitMask, uint32_t timeOutUsec);
Error_t HalRci_WaitValueU32(uint32_t address, uint32_t testValue, uint32_t bitMask, uint32_t timeOutUsec);
Error_t HalRci_WaitEvent(bool (*pFuncBool)(uint32_t, uint32_t, uint32_t), uint32_t param1, uint32_t param2, uint32_t param3, uint32_t timeOutUsec);

