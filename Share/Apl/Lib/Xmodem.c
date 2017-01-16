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
//! @brief XModem data receive protocol handling code
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "CharIo.h"
#include "MemIo.h"
#include "Xmodem.h"
#include "HalTimer.h"
#include "HalUart.h"

#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define XMODEM_SOH                   1   ///< Start Of Header
#define XMODEM_STX                   2   ///< Start Of Header (XModem 1k), not used for now 1-30-15
#define XMODEM_EOT                   4   ///< End Of Transmission
#define XMODEM_ACK                   6   ///< Acknowledge (positive)
#define XMODEM_DLE                  16   ///< Data Link Escape
#define XMODEM_XON                  17   ///< transmit ON
#define XMODEM_XOFF                 19   ///< transmit OFF
#define XMODEM_NAK                  21   ///< Negative Acknowledge 0x15
#define XMODEM_SYN                  22   ///< Synchronous idle
#define XMODEM_CAN                  24   ///< Cancel
#define XMODEM_BLOCK_SIZE           128     ///< XMODEM block size (128 Bytes)
#define XMODEM_128B_PACKET_HEADER   3       ///< SOH ,SEQ, SEQ cmpl, 128 data byte, CSUM
#define XMODEM_128B_PACKET_CKSUM    1       ///< SOH ,SEQ, SEQ cmpl, 128 data byte, CSUM
#define XMODEM_128B_PACKET_SIZE     132     ///< SOH ,SEQ, SEQ cmpl, 128 data byte, CSUM
#define XMODEM_SOH_TIMEOUT_MSEC     2000 ///< SOH waiting timeout
#define XMODEM_DATA_TIMEOUT_MSEC    5000 ///< DATA watiting timeout
#define XMODEM_ACK_TIMEOUT_MSEC     10000 ///< DATA watiting timeout

/// @brief   XModem state machine states
typedef enum
{
    cRxWaitSohNak,        ///< Receiver: Wait SOH from transmitter (sending NAK)
    cRxWaitSohAck,        ///< Receiver: Wait SOH from transmitter (sending ACK)
    cRxReceivePacket,     ///< Receiver: Read one packet
    cRxCancel,            ///< Receiver: Cancel receiving
    cRxEnd,               ///< Receiver: End of receiving (good end)
    cTxWaitAckNack,       ///< Transmitter: Wait NAK or ACK from receiver
    cTxSendPacket,        ///< Transmitter: Send one packet
    cTxCancel,            ///< Transmitter: Cancel transmitt
    cTxEnd                ///< Transmitter: End of transfer (good end)
} XmodemState_t;

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static bool GetData(uint8_t* pInChar, uint32_t timeOutMsec);
static uint8_t SendAndRecevieControlChar(uint8_t controlChar);
static uint8_t ReceiveOneBlock(uint8_t* pBuffer, uint32_t* pExpectedSeqNum);
static Error_t SendOneBlock (uint8_t sequence, uint32_t ptr, uint32_t endPtr);


//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------

static uint8_t xModemBuffer[140];

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
    Get a byte data with timeout.\n

    @param[out] pData           data
    @param[in]  timeOutMsec     timeout in milli-seconds unit

    @return TRUE if received data, FALSE if timed out
**/
//-----------------------------------------------------------------------------
bool GetData(uint8_t* pInChar, uint32_t timeOutMsec)
{
    HwTimerTick_t tick;

    // Take a snapshot of the timer.
    tick = HalTimer_ReadTick();

    do
    {
        if (InKey(pInChar)) return (TRUE);

    } while (HalTimer_GetElapsedTimeMsec(tick)<timeOutMsec);

    return(FALSE);
}

//-----------------------------------------------------------------------------
/**
    Send and receive control character.\n

    @param[in]  controlChar     control character to send

    @return if SOH, EOT and CAN were received, pass it on.  If not, return control char sent (argument 0).
**/
//-----------------------------------------------------------------------------
uint8_t SendAndRecevieControlChar(uint8_t controlChar)
{
    uint8_t rxControlChar=controlChar;

    PutChar(controlChar);

    GetData(&rxControlChar, XMODEM_SOH_TIMEOUT_MSEC);   // 2000

    switch(rxControlChar)
    {
    case XMODEM_SOH:
    case XMODEM_EOT:
    case XMODEM_CAN:
    case XMODEM_NAK:
        return(rxControlChar);
    }

    return(controlChar);
}

//-----------------------------------------------------------------------------
/**
    Receive one data packet(128 bytes).\n

    Don't check block number.

    @param[out]  pBuffer    buffer pointer to store data
    @param[out]  pOffset    buffer offset to store data

    @return next control character to send
**/
//-----------------------------------------------------------------------------
uint8_t ReceiveOneBlock(uint8_t* pBuffer, uint32_t* pOffset)
{
    uint32_t i;
    uint8_t  data;
    uint8_t  checkSum = 0;
    uint8_t  blockNumber;    // sequence number [SEQ]
    uint8_t  blockNumber1;   // [^SEQ]
    uint8_t  sendersCheckSum;
    uint8_t  expectedBlockNumber;

    expectedBlockNumber = ((*pOffset)/XMODEM_BLOCK_SIZE) + 1;
    pBuffer = pBuffer + (*pOffset);

    if (!GetData(&blockNumber, XMODEM_DATA_TIMEOUT_MSEC)) return XMODEM_CAN;
    if (!GetData(&blockNumber1, XMODEM_DATA_TIMEOUT_MSEC)) return XMODEM_CAN;

    for ( i=0; i<XMODEM_BLOCK_SIZE; i++ )
    {
        if (!GetData(&data, XMODEM_DATA_TIMEOUT_MSEC)) return XMODEM_CAN;
        checkSum += data;
        *pBuffer++ = data;
    }

    if (!GetData(&sendersCheckSum, XMODEM_DATA_TIMEOUT_MSEC)) return XMODEM_CAN;

    if ((sendersCheckSum != checkSum) || (blockNumber+blockNumber1)!=0xFF) // || (expectedBlockNumber!=blockNumber))
    {
        M_PRINTF(" %X %X %X\n", expectedBlockNumber, blockNumber, blockNumber1);
        return(XMODEM_NAK);
    }

    *pOffset += XMODEM_BLOCK_SIZE;

    return(XMODEM_ACK);
}
//-----------------------------------------------------------------------------
/**
    Receive data by using X-Modem 128 Bytes protocol.\n

    @param[in]  pBuffer     buffer pointer
    @param[in]  size        buffer size in byte count(shall be larger than 128)
    @param[out] rxSize      size of received data in byte count

    @return cEcError in case of cancellation, cEcNoError for OK.
**/
//-----------------------------------------------------------------------------
Error_t XmodemReceive(uint8_t* pBuffer, uint32_t size, uint32_t* pReceivedSize)
{
    uint8_t  controlChar = XMODEM_NAK;

    *pReceivedSize = 0;

    while (TRUE)
    {
        switch(controlChar)
        {
        case XMODEM_ACK:
        case XMODEM_NAK:
            controlChar = SendAndRecevieControlChar(controlChar);
            break;
        case XMODEM_SOH:
            controlChar = ReceiveOneBlock(pBuffer, pReceivedSize);
            break;
        case XMODEM_CAN:
            PutChar(XMODEM_CAN);
            PutChar(XMODEM_CAN);
            PutChar(XMODEM_CAN);
            return(cEcError);
        case XMODEM_EOT:
            PutChar(XMODEM_ACK);
            return(cEcNoError);
        }
    }
}

void ChottoMatteKudasai (void)
{
    // make sure UART is not in transition
    while (1)
    {
        if (HalUart_TestRxReady(0) && HalUart_TestTxReady(0))
            break;
    }
}

Error_t XmodemSendExit (Error_t rc)
{
    PutChar(XMODEM_CAN);
    PutChar(XMODEM_CAN);
    return rc;
}

//-----------------------------------------------------------------------------
/**
    Send data by using X-Modem 128 Bytes protocol.\n

    @param[in]  pBuffer     buffer pointer
    @param[in]  size        buffer size in byte count(shall be larger than 128)

    @return cEcError in case of cancellation, cEcNoError for OK.
**/
//-----------------------------------------------------------------------------
// see:  http://wiki.synchro.net/ref:xmodem
Error_t XmodemSend(uint8_t* pBuffer, uint32_t size)
{
    uint8_t   sequence = 1;   // starting at 1
    uint16_t  retry = 300, xmitRetry = 10;
    uint8_t   controlChar = XMODEM_ACK;
    uint32_t  ptr = (uint32_t)pBuffer;
    uint32_t  endPtr = ptr + size;
    Error_t   rc;

    // initialization:

    while (TRUE)
    {
        // try every 100 ms with 300 retries
        HalTimer_DelayMsec(100);
        controlChar = SendAndRecevieControlChar(XMODEM_ACK);
        {
            if (controlChar == XMODEM_NAK)
            {
                break;
            }
            if (controlChar == XMODEM_CAN)
            {
                return XmodemSendExit (cEcUartSendInitAbort);
            }
        }

        // timeout or not NAK
        retry --;
        if (0==retry)
        {
            return XmodemSendExit (cEcUartSendInitRetry);
        }
    }

    // data phase
    while (TRUE)
    {
        // Prepare then send a 132-byte packet
        rc = SendOneBlock (sequence, ptr, endPtr);
        if (rc == cEcNoError)
        {
            sequence ++;
            ptr += XMODEM_BLOCK_SIZE;
            // handling EOT
            if (ptr >= endPtr)
            {
                retry = 10;
                while (TRUE)
                {
                    uint8_t txCC;

                    while (1)
                    {
                        if (HalUart_TestTxReady(0))
                            break;
                    }
                    PutChar(XMODEM_EOT);

                    while (1)
                    {
                        if (HalUart_TestRxReady(0))
                            break;
                    }
                    GetData(&txCC, XMODEM_ACK_TIMEOUT_MSEC);

                    //M_PRINTF("finalACK %d\n", txCC);
                    if (txCC == XMODEM_ACK)
                    {
                        return rc;  // completed
                    }
                    else
                    //txCC = SendAndRecevieControlChar(XMODEM_EOT);
                    {
                        retry --;
                        if (retry == 0)
                        {
                            return XmodemSendExit (cEcUartSendEOT);
                        }
                    }
                }
            }
            xmitRetry = 10; // restore retry count for every good packet sent
            continue;
        }

        xmitRetry --;
        if (0 == xmitRetry)
        {
            return XmodemSendExit (cEcUartSendPacketRetry);
        }
        // retry the same packet.
    }
}

Error_t SendOneBlock (uint8_t sequence, uint32_t ptr, uint32_t endPtr)
{
    uint8_t     checksum = 0;
    uint8_t     controlChar;

    //MEM_AllocBuffer(sizeof(OnfiParamPage_t), 4);
    xModemBuffer[0] = XMODEM_SOH;
    xModemBuffer[1] = sequence;
    xModemBuffer[2] = 0xFF - sequence;

    MEM_COPY((void*)&(xModemBuffer[3]), (void *)ptr, XMODEM_BLOCK_SIZE);

    // for now, just sending incremental pattern
    // send and compute checksum loop
    {
        uint16_t i;

        for (i = 0; i < XMODEM_128B_PACKET_SIZE-1; i++)
        {
            if ((i >= 3) && (i <= 130)) // data packet
            {
                // if inc pattern xModemBuffer[i] = i - 3 + sequence;
                checksum += xModemBuffer[i];
            }
            PutChar(xModemBuffer[i]);
        }
        PutChar(checksum);
    }

    // do this get, the character seems to be lost
    while (1)
    {
        if (HalUart_TestRxReady(0))
            break;
    }
    GetData(&controlChar, XMODEM_ACK_TIMEOUT_MSEC);


    // if you do this, seeing 06 and 15 from host; controlChar = SendAndRecevieControlChar(0);
    {
        if (controlChar == XMODEM_ACK)
        {
            return cEcNoError;
        }
    }
    M_PRINTF("\nTBAD %d\n", controlChar);
    return cEcUartSendPacketFail;
}

