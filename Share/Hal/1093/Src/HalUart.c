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
//! @brief UART I/F code for ALTA SoC
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "ClockTree.h"
#include "HalUart.h"
#include "HalCpu.h"
#include "HalClock.h"
#include "RegClockPLL.h"
#include "RegPeripheral.h"
#include "HalGpio.h"

#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//
//  VARIABLE DECLARATION WITH INITIAL VALUE DOESN'T WORK !!!
//  UNINTIALIZED VARIABLE DECLARATION ONLY !!!
//-----------------------------------------------------------------------------
#pragma arm section rwdata = "data_fast_ui"
#pragma arm section zidata = "data_fast_ui"

static UartPowerMode_t currentUartPowerMode;
//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    return UART clock frequency\n

    @return  Uart source clock frequency in MHZ unit
**/
//-----------------------------------------------------------------------------
__inline uint32_t GetUartSourceClockFreqMhz(void)
{
#if UART_CLOCK_SRC==SRC_CPU
    if (rClockPll.clkSelectioCtrl.b.CLK_SEL_PLLB_OSC == 0)      // 0:PLL
    {
        return HalClock_GetFrequency(cClockTbgB)/2;
    }
    else                                                        // 1:OSC
    {
        return HalClock_GetFrequency(cClockOsc)/2;
    }
#elif UART_CLOCK_SRC==SRC_UART
    return(UART_CLOCK_FREQ_MHZ);
#else
    #error "not supported UART clock source !!"
#endif
}

#if _CPU_==CPU_ID0    // CPU0 only
//-----------------------------------------------------------------------------
/**
    One time Initialization of UART HW.\n
     - set baud rate to 115200\n
     - enable UART output pin\n
     - enable UART periperal Rx,Tx interrupt\n

    @param[in]  initMode    intialization mode

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalUart_OneTimeInit(InitBootMode_t initMode)
{
    if (initMode != cInitBootCpu0Boot)
    {
        // enable UART0 interrupts
        rUart0.uartCtrl.b.RX_RDY_INT_EN = 1;
        //rUart0.uartCtrl.b.TX_EMPTY_INT_EN = 1;

        // enable UART1 interrupts
        rUart1.uartCtrl.b.RX_RDY_INT_EN = 1;
        //rUart1.uartCtrl.b.TX_EMPTY_INT_EN = 1;
    }

    currentUartPowerMode = cUartPmActive;

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    H/W block level power management API

    @param[in]  powerMode        targeting pm mode to enter
    @return error code
**/
//-----------------------------------------------------------------------------
Error_t HalUart_SetPowerMode(UartPowerMode_t powerMode)
{
    // no set up if power mode is not changed.
    if (currentUartPowerMode == powerMode) return cEcNoError;

    switch(powerMode)
    {
    case cUartPmDeepSleep:
        break;
    case cUartPmActive:
        HalUart_OneTimeInit(cInitBootDeepPowerDown);
        HalUart_SetBaudRate(0, UART_BAUD_RATE);             // Set Uart Clock for PLLB PLL
        break;
    default:
        break;
    }

    currentUartPowerMode = powerMode;

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Set UART baud rate

    @param[in]  portNum     UART port number
    @param[in]  baudRate    baud rate

    @return actual set baud rate
**/
//-----------------------------------------------------------------------------
uint32_t HalUart_SetBaudRate(uint32_t portNum, uint32_t baudRate)
{
    volatile Uart_t* pUart;
    uint32_t dummy;

    pUart = (portNum==0) ? &rUart0 : &rUart1;

    pUart->uartCtrl.b.TXFIFO_RST=1;
    pUart->uartCtrl.b.TXFIFO_RST=0;
    while (pUart->uartStat.b.TX_EMPTY==0); // wait for TX FIFO empty
    HalCpu_DelayUsec(200); // wait for last character sent, without this TX hung, need more investigation
    //HalGpio_AssertPin(13);
    //HalCpu_DelayLoop(0x10000000); // 0x10000000 will cause pwr on device disappear
    //HalCpu_DelayLoop(0x100000); // 0x100000 about ms/ms ok
    //HalCpu_DelayLoop(0x80000); // 0x80000 about 9ms/50ms ok
    //HalCpu_DelayLoop(0x40000); // 0x40000 about 5ms/26ms ok/ng
    //HalCpu_DelayLoop(0x20000); // 0x20000 about 2ms/13ms ng
    //HalCpu_DelayLoop(0x10000); // 0x10000 about 1ms ng
    //HalGpio_DeassertPin(13);

    pUart->progOvsmplStack1.b.BAUD_RATE_DIV = (GetUartSourceClockFreqMhz()*62500)/baudRate + 1;

    return(baudRate);
}
#endif

//-----------------------------------------------------------------------------
/**
    Test Tx FIFO ready or not

    @param[in]  portNum     UART port number

    @return TRUE if Tx FIFO is ready
**/
//-----------------------------------------------------------------------------
bool HalUart_TestTxReady(uint32_t portNum)
{
    volatile Uart_t* pUart;

    pUart = (portNum==0) ? &rUart0 : &rUart1;

    return pUart->uartStat.b.TX_READY;
}

bool HalUart_TestTxEmpty(uint32_t portNum)
{
    volatile Uart_t* pUart;

    pUart = (portNum==0) ? &rUart0 : &rUart1;

    return pUart->uartStat.b.TX_EMPTY;
}

//-----------------------------------------------------------------------------
/**
    Transmit 1 byte data to UART Tx FIFO.\n
    This code doesn't check Tx FIFO ready or not\n

    @param[in]  portNum     UART port number
    @param[in]  data    byte data to be sent
**/
//-----------------------------------------------------------------------------
void HalUart_Transmit(uint32_t portNum, uint8_t data)
{
    volatile Uart_t* pUart;

    pUart = (portNum==0) ? &rUart0 : &rUart1;

    pUart->uartTransmitHolding.all = data;
}

//-----------------------------------------------------------------------------
/**
    Test Rx FIFO ready or not

    @param[in]  portNum     UART port number

    @return TRUE if Rx FIFO is ready
**/
//-----------------------------------------------------------------------------
bool HalUart_TestRxReady(uint32_t portNum)
{
    volatile Uart_t* pUart;

    pUart = (portNum==0) ? &rUart0 : &rUart1;

    return pUart->uartStat.b.RX_READY;
}

//-----------------------------------------------------------------------------
/**
    Receive 1 byte data from UART Rx FIFO.\n
    This code doesn't check RX FIFO ready\n

    @param[in]  portNum     UART port number

    @return byte data
**/
//-----------------------------------------------------------------------------
uint8_t HalUart_Receive(uint32_t portNum)
{
    volatile Uart_t* pUart;

    pUart = (portNum==0) ? &rUart0 : &rUart1;

    return pUart->uartReceiverBuf.b.UART_RCVR_BFR;
}

//-----------------------------------------------------------------------------
/**
   Reset UART FIFO.

    @param[in]  portNum     UART port number
**/
//-----------------------------------------------------------------------------
void HalUart_ResetFIFO(uint32_t portNum)
{
    volatile Uart_t* pUart;

    pUart = (portNum==0) ? &rUart0 : &rUart1;

    pUart->uartCtrl.b.TXFIFO_RST=1;
    pUart->uartCtrl.b.TXFIFO_RST=0;
    while (pUart->uartStat.b.TX_EMPTY==0); // wait for TX FIFO empty
    HalCpu_DelayUsec(25); // wait for last character sent, without this TX hung, need more investigation
}
