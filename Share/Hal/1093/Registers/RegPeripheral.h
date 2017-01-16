#pragma once
// ============================================================================
//
//     Copyright (c) Marvell Corporation 2000-2015  -  All rights reserved     
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
//! @brief UART Registers
//! 
//! Doc Version: STANDARD
//! Doc Date: 03/18/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  UART Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xF0116000 : Watchdog Timer Compare
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t WDT_CMP              :16;         ///<BIT [15:0] Watchdog Timer Compare.
        uint32_t RF0116000_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} WdgTmrCompare_t;

///  @brief 0xF0116004 : Watchdog Timer Counter
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t WDT_CNT              :16;         ///<BIT [15:0] Watchdog Timer Counter.
        uint32_t RF0116004_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} WdgTmrCntr_t;

///  @brief 0xF0116008 : Watchdog Timer Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t EN_WDT_REBOOT        : 1;         ///<BIT [0] Enable Watchdog Timer Reboot.
        uint32_t EN_WDT_TO_INTR       : 1;         ///<BIT [1] Enable Watchdog Timer Time-out Interrupt.
        uint32_t EN_WDT_ALM_INTR      : 1;         ///<BIT [2] Enable Watchdog Timer Alarm Interrupt.
        uint32_t PR_SCL_SET           : 3;         ///<BIT [5:3] Watchdog Timer Prescale Set.
        uint32_t RF0116008_6_RSVD     : 1;         ///<BIT [6] Reserved.
        uint32_t EN_WDT               : 1;         ///<BIT [7] Enable Watchdog Timer.
        uint32_t RF0116008_15_8_RSVD  : 8;         ///<BIT [15:8] Reserved.
        uint32_t RF0116008_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} WdgTmrCtrl_t;

///  @brief 0xF011600C : Watchdog Timer Interrupt Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t WDT_ALM_INTR         : 1;         ///<BIT [0] Watchdog Timer Alarm Interrupt.
        uint32_t WDT_TO_INTR          : 1;         ///<BIT [1] Watchdog Timer Time-out Interrupt.
        uint32_t WDT_RBTD             : 1;         ///<BIT [2] Watchdog Timer Rebooted.
        uint32_t RF011600C_15_3_RSVD  :13;         ///<BIT [15:3] Reserved.
        uint32_t RF011600C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} WdgTmrIntrStat_t;

///  @brief 0xF1010000 : UART Receiver Buffer
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t UART_RCVR_BFR        : 8;         ///<BIT [7:0] UART Receiver Buffer.
        uint32_t RF1010000_11_8_RSVD  : 4;         ///<BIT [11:8] Reserved.
        uint32_t OVRUN_ERR_MIRR       : 1;         ///<BIT [12] Overrun Error Mirror.
        uint32_t PARITY_ERR_DET       : 1;         ///<BIT [13] Parity Error Detect.
        uint32_t FRAME_ERR_DET        : 1;         ///<BIT [14] Frame Error Detect.
        uint32_t BREAK_DET            : 1;         ///<BIT [15] Break Detect.
        uint32_t RF1010000_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} UartReceiverBuf_t;

///  @brief 0xF1010004 : UART Transmitter Holding
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t UART_TRANS_HLD      : 8;         ///<BIT [7:0] UART Transmitter Holding.
        uint32_t RF1010004_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;    
} UartTransmitHolding_t;

///  @brief 0xF1010008 : UART Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t OVRUN_ERR_INT_EN       : 1;         ///<BIT [0] Overrun Error Interrupt Enable.
        uint32_t PARITY_ERR_INT_EN      : 1;         ///<BIT [1] Parity Error Interrupt Enable.
        uint32_t FRAME_ERR_INT_EN       : 1;         ///<BIT [2] Frame Error Interrupt Enable.
        uint32_t BRK_DET_INT_EN         : 1;         ///<BIT [3] Break Detect Interrupt Enable.
        uint32_t RX_RDY_INT_EN          : 1;         ///<BIT [4] Rx Ready Interrupt Enable.
        uint32_t TX_RDY_INT_EN          : 1;         ///<BIT [5] Tx Ready Interrupt Enable.
        uint32_t TX_EMPTY_INT_EN        : 1;         ///<BIT [6] Tx Empty Interrupt Enable.
        uint32_t RXFIFO_HALFFULL_INT_EN : 1;         ///<BIT [7] Rx FIFO Half-Full Interrupt Enable.
        uint32_t TWO_STOP_BITS          : 1;         ///<BIT [8] Two Stop Bits.
        uint32_t PARITY_EN              : 1;         ///<BIT [9] Parity Enable.
        uint32_t SEND_BREAK_SEQ         : 1;         ///<BIT [10] Send Break Sequence.
        uint32_t LOOPBACK_RW_EN         : 1;         ///<BIT [11] Loop Back Read/Write Enable.
        uint32_t RF1010008_12_RSVD      : 1;         ///<BIT [12] Reserved.
        uint32_t ST_MIRROR_EN           : 1;         ///<BIT [13] Status Mirror Enable.
        uint32_t RXFIFO_RST             : 1;         ///<BIT [14] Rx FIFO Reset.
        uint32_t TXFIFO_RST             : 1;         ///<BIT [15] Tx FIFO Reset.
        uint32_t RX_HFULL_SEL           : 2;         ///<BIT [17:16] Receive FIFO Half Full Select.
        uint32_t TX_HFULL_SEL           : 2;         ///<BIT [19:18] Transmit FIFO Half Full Select.
        uint32_t RF1010008_31_20_RSVD   :12;         ///<BIT [31:20] Reserved.
    } b;    
} UartCtrl_t;

///  @brief 0xF101000C : UART Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t OVRUN_ERR            : 1;         ///<BIT [0] Overrun Error.
        uint32_t PARITY_ERR           : 1;         ///<BIT [1] Parity Error.
        uint32_t FRAME_ERR            : 1;         ///<BIT [2] Frame Error.
        uint32_t BRK_DET              : 1;         ///<BIT [3] Break Sequence Detect.
        uint32_t RX_READY             : 1;         ///<BIT [4] Rx Ready.
        uint32_t TX_READY             : 1;         ///<BIT [5] Tx Ready.
        uint32_t TX_EMPTY             : 1;         ///<BIT [6] Tx Empty.
        uint32_t RXFIFO_HALF_FULL     : 1;         ///<BIT [7] Rx FIFO Half Full.
        uint32_t RXFIFO_FULL          : 1;         ///<BIT [8] Rx FIFO Full.
        uint32_t RX_TOGGLED           : 1;         ///<BIT [9] Rx Toggled.
        uint32_t TX_FIFO_HFULL        : 1;         ///<BIT [10] Transmit FIFO Half Full.
        uint32_t TX_FIFO_FULL         : 1;         ///<BIT [11] Transmit FIFO Full.
        uint32_t RX_FIFO_EMPTY        : 1;         ///<BIT [12] Receive Empty.
        uint32_t RF101000C_14_13_RSVD : 2;         ///<BIT [14:13] Reserved.
        uint32_t RX_CLOCK_SELECT      : 1;         ///<BIT [15] Clock Select.
        uint32_t RF101000C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} UartStat_t;

///  @brief 0xF1010010 : Programmable Oversampling Stack 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t BAUD_RATE_DIV        :10;         ///<BIT [9:0] Baud Rate Divisor.
        uint32_t RF1010010_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
        uint32_t POSSR_ENT_1          : 6;         ///<BIT [21:16] Programmable Oversampling Stack Entry One.
        uint32_t RF1010010_23_22_RSVD : 2;         ///<BIT [23:22] Reserved.
        uint32_t POSSR_ENT_2          : 6;         ///<BIT [29:24] Programmable Oversampling Stack Entry Two.
        uint32_t RF1010010_31_30_RSVD : 2;         ///<BIT [31:30] Reserved.
    } b;    
} ProgOvsmplStack1_t;

///  @brief 0xF1010020 : Programmable Oversampling Stack 2
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t POSSR_ENT_3          : 6;         ///<BIT [5:0] Programmable Oversampling Stack Entry Three.
        uint32_t RF1010020_7_6_RSVD   : 2;         ///<BIT [7:6] Reserved.
        uint32_t POSSR_ENT_4          : 6;         ///<BIT [13:8] Programmable Oversampling Stack Entry Four.
        uint32_t RF1010020_31_14_RSVD :18;         ///<BIT [31:14] Reserved.
    } b;    
} ProgOvsmplStack2_t;


/// @brief 0xF0116000-0xF0116010 : WdTimer_t MODULE
typedef struct
{
    WdgTmrCompare_t wdgTmrCompare;       ///< 0xF0116000 : Watchdog Timer Compare
    WdgTmrCntr_t wdgTmrCntr;             ///< 0xF0116004 : Watchdog Timer Counter
    WdgTmrCtrl_t wdgTmrCtrl;             ///< 0xF0116008 : Watchdog Timer Control
    WdgTmrIntrStat_t wdgTmrIntrStat;     ///< 0xF011600C : Watchdog Timer Interrupt Status
} WdTimer_t;
COMPILE_ASSERT(offsetof(WdTimer_t,wdgTmrIntrStat)==0xF011600C-0xF0116000,"check register structure offset");


/// @brief 0xF1010000-0xF1010100 : Uart_t MODULE
typedef struct
{
    UartReceiverBuf_t uartReceiverBuf;             ///< 0xF1010000 : UART Receiver Buffer
    UartTransmitHolding_t uartTransmitHolding;     ///< 0xF1010004 : UART Transmitter Holding
    UartCtrl_t uartCtrl;                           ///< 0xF1010008 : UART Control
    UartStat_t uartStat;                           ///< 0xF101000C : UART Status
    ProgOvsmplStack1_t progOvsmplStack1;           ///< 0xF1010010 : Programmable Oversampling Stack 1
    uint8_t rsvd_RF1010014[12];                    ///< 0xF1010014 - 0xF101001F : Reserved.
    ProgOvsmplStack2_t progOvsmplStack2;           ///< 0xF1010020 : Programmable Oversampling Stack 2
} Uart_t;
COMPILE_ASSERT(offsetof(Uart_t,progOvsmplStack2)==0xF1010020-0xF1010000,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile WdTimer_t rWdTimer;                  ///< 0xF0116000-0xF0116010 : WdTimer_t MODULE
extern volatile Uart_t rUart0;                       ///< 0xF1010000-0xF1010100 : Uart_t MODULE 0
extern volatile Uart_t rUart1;                       ///< 0xF1010100-0xF1010200 : Uart_t MODULE 1

//-----------------------------------------------------------------------------
//  Exported register reference (Non volatile type for optimization)
//-----------------------------------------------------------------------------
