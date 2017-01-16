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
//! @brief I2C Registers
//! 
//! Doc Version: STANDARD
//! Doc Date: 03/18/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  I2C Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xF0108000 : I2C Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t MSTR_MODE           : 1;         ///<BIT [0] Master Mode.
        uint32_t SPEED_IC            : 2;         ///<BIT [2:1] I2C Speed.
        uint32_t IC_10B_ADDR_SLAVE   : 1;         ///<BIT [3] I2C 10-bit Address Slave.
        uint32_t IC_10B_ADDR_MSTR    : 1;         ///<BIT [4] I2C 10-bit Address Master.
        uint32_t IC_RESTART_EN       : 1;         ///<BIT [5] I2C Restart Enable.
        uint32_t IC_SLAVE_DIS        : 1;         ///<BIT [6] I2C Slave Disable.
        uint32_t RF0108000_31_7_RSVD :25;         ///<BIT [31:7] Reserved.
    } b;    
} I2cCtrl_t;

///  @brief 0xF0108004 : I2C Target Address
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IC_TAR               :10;         ///<BIT [9:0] I2C Target.
        uint32_t GC_OR_START          : 1;         ///<BIT [10] General Call Or START.
        uint32_t SPECIAL              : 1;         ///<BIT [11] Special.
        uint32_t RF0108004_31_12_RSVD :20;         ///<BIT [31:12] Reserved.
    } b;    
} I2cTgtAddr_t;

///  @brief 0xF0108008 : I2C Slave Address
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IC_SAR               :10;         ///<BIT [9:0] IC Slave Address.
        uint32_t RF0108008_31_10_RSVD :22;         ///<BIT [31:10] Reserved.
    } b;    
} I2cSlaveAddr_t;

///  @brief 0xF010800C : I2C High Speed Master Mode Code Address
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IC_HS_MAR           : 3;         ///<BIT [2:0] I2C HS (High-Speed) Mode Master Code Address.
        uint32_t RF010800C_31_3_RSVD :29;         ///<BIT [31:3] Reserved.
    } b;    
} I2cHighSpeedMstrModeCmemAddr_t;

///  @brief 0xF0108010 : I2C Rx/Tx Data Buffer and Command
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DAT                 : 8;         ///<BIT [7:0] Data.
        uint32_t CMD                 : 1;         ///<BIT [8] Read or Write Command.
        uint32_t RF0108010_31_9_RSVD :23;         ///<BIT [31:9] Reserved.
    } b;    
} I2cRxTxDataBufAndCmd_t;

///  @brief 0xF0108014 : I2C Standard Speed Clock SCL High Count
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IC_SS_SCL_HCNT       :16;         ///<BIT [15:0] IC SCL High-speed Clock Count.
        uint32_t RF0108014_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} I2cStandardSpeedClkSclHighCnt_t;

///  @brief 0xF0108018 : I2C Standard Speed Clock SCL Low Count
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IC_SS_SCL_LCNT       :16;         ///<BIT [15:0] IC SCL Low-speed Clock Count.
        uint32_t RF0108018_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} I2cStandardSpeedClkSclLowCnt_t;

///  @brief 0xF010801C : I2C Fast Speed Clock SCL High Count
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IC_FS_SCL_HCNT       :16;         ///<BIT [15:0] IC SCL Fast-speed Clock Count.
        uint32_t RF010801C_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} I2cFastSpeedClkSclHighCnt_t;

///  @brief 0xF0108020 : I2C Fast Speed Clock SCL Low Count
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IC_FS_SCL_LCNT       :16;         ///<BIT [15:0] IC Fast-speed Clock SCL Low Count.
        uint32_t RF0108020_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} I2cFastSpeedClkSclLowCnt_t;

///  @brief 0xF0108024 : I2C High Speed Clock SCL High Count
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IC_HS_SCL_HCNT       :16;         ///<BIT [15:0] IC High-speed Clock SCL High Count.
        uint32_t RF0108024_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} I2cHighSpeedClkSclHighCnt_t;

///  @brief 0xF0108028 : I2C High Speed Clock SCL Low Count
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IC_HS_SCL_LCNT       :16;         ///<BIT [15:0] IC High-speed Clock SCL Low Count.
        uint32_t RF0108028_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} I2cHighSpeedClkSclLowCnt_t;

///  @brief 0xF010802C : I2C Interrupt Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t R_RX_UNDER           : 1;         ///<BIT [0] Receive Under.
        uint32_t R_RX_OVER            : 1;         ///<BIT [1] Receive Over.
        uint32_t R_RX_FULL            : 1;         ///<BIT [2] Receive Full.
        uint32_t R_TX_OVER            : 1;         ///<BIT [3] Transmit Over.
        uint32_t R_TX_EMPTY           : 1;         ///<BIT [4] Transmit Empty.
        uint32_t R_RD_REQ             : 1;         ///<BIT [5] Read Request (RD_REQ).
        uint32_t R_TX_ABRT            : 1;         ///<BIT [6] Transmit Abort.
        uint32_t R_RX_DONE            : 1;         ///<BIT [7] Receive Done.
        uint32_t R_ACTIVITY           : 1;         ///<BIT [8] I2C Activity Capture.
        uint32_t R_STOP_DET           : 1;         ///<BIT [9] Stop Condition Indicator.
        uint32_t R_START_DET          : 1;         ///<BIT [10] Start or Restart Indicator.
        uint32_t R_GEN_CALL           : 1;         ///<BIT [11] General Call Receipt.
        uint32_t RF010802C_31_12_RSVD :20;         ///<BIT [31:12] Reserved.
    } b;    
} I2cIntrStat_t;

///  @brief 0xF0108030 : I2C Interrupt Mask
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t M_RX_UNDER           : 1;         ///<BIT [0] Mask Receive Under.
        uint32_t M_RX_OVER            : 1;         ///<BIT [1] Mask Receive Over.
        uint32_t M_RX_FULL            : 1;         ///<BIT [2] Mask Receive Full.
        uint32_t M_TX_OVER            : 1;         ///<BIT [3] Mask Transmit Over.
        uint32_t M_TX_EMPTY           : 1;         ///<BIT [4] Mask Transmit Empty.
        uint32_t M_RD_REQ             : 1;         ///<BIT [5] Mask Read Request (RD_REQ).
        uint32_t M_TX_ABRT            : 1;         ///<BIT [6] Mask Transmit Abort.
        uint32_t M_RX_DONE            : 1;         ///<BIT [7] Mask Receive Done.
        uint32_t M_ACTIVITY           : 1;         ///<BIT [8] Mask I2C Activity Capture.
        uint32_t M_STOP_DET           : 1;         ///<BIT [9] Mask Stop Condition Indicator.
        uint32_t M_START_DET          : 1;         ///<BIT [10] Mask Start or Restart Indicator.
        uint32_t M_GEN_CALL           : 1;         ///<BIT [11] Mask General Call Receipt.
        uint32_t RF0108030_31_12_RSVD :20;         ///<BIT [31:12] Reserved.
    } b;    
} I2cIntrMsk_t;

///  @brief 0xF0108034 : I2C Raw Interrupt Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RX_UNDER             : 1;         ///<BIT [0] Raw Receive Under.
        uint32_t RX_OVER              : 1;         ///<BIT [1] Raw Receive Over.
        uint32_t RX_FULL              : 1;         ///<BIT [2] Raw Receive Full.
        uint32_t TX_OVER              : 1;         ///<BIT [3] Raw Transmit Over.
        uint32_t TX_EMPTY             : 1;         ///<BIT [4] Raw Transmit Empty.
        uint32_t RD_REQ               : 1;         ///<BIT [5] Raw Read Request.
        uint32_t TX_ABRT              : 1;         ///<BIT [6] Raw Transmit Abort.
        uint32_t RX_DONE              : 1;         ///<BIT [7] Raw Receive Done.
        uint32_t ACT_CAP              : 1;         ///<BIT [8] Raw I2C Activity Capture.
        uint32_t STOP_DET             : 1;         ///<BIT [9] Raw Stop Condition Indicator.
        uint32_t START_DET            : 1;         ///<BIT [10] Raw Start or Restart Indicator.
        uint32_t GEN_CALL             : 1;         ///<BIT [11] General Call Received and Acknowledged.
        uint32_t RF0108034_31_12_RSVD :20;         ///<BIT [31:12] Reserved.
    } b;    
} I2cRawIntrStat_t;

///  @brief 0xF0108038 : I2C Receive FIFO Threshold
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RX_TL               : 8;         ///<BIT [7:0] Receive FIFO Threshold Level.
        uint32_t RF0108038_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;    
} I2cRxFifoThr_t;

///  @brief 0xF010803C : I2C Transmit FIFO Threshold
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t TX_TL               : 8;         ///<BIT [7:0] Transmit FIFO Threshold Level.
        uint32_t RF010803C_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;    
} I2cTxFifoThr_t;

///  @brief 0xF0108040 : Clear Combined and Individual Interrupt
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CLR_INTR            : 1;         ///<BIT [0] Clear Interrupts.
        uint32_t RF0108040_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } b;    
} ClrCombinedAndIndividuIntr_t;

///  @brief 0xF0108044 : Clear RX_UNDER Interrupt
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CLR_RX_UNDER        : 1;         ///<BIT [0] Clear Receive Under.
        uint32_t RF0108044_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } b;    
} ClrRxUnderIntr_t;

///  @brief 0xF0108048 : Clear RX_OVER Interrupt
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CLR_RX_OVER         : 1;         ///<BIT [0] Clear Receive Over.
        uint32_t RF0108048_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } b;    
} ClrRxOverIntr_t;

///  @brief 0xF010804C : Clear TX_OVER Interrupt
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CLR_TX_OVER         : 1;         ///<BIT [0] Clear Transmit Over.
        uint32_t RF010804C_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } b;    
} ClrTxOverIntr_t;

///  @brief 0xF0108050 : Clear RD_REQ Interrupt
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CLR_RD_REQ          : 1;         ///<BIT [0] Clear Read Request.
        uint32_t RF0108050_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } b;    
} ClrRdReqIntr_t;

///  @brief 0xF0108054 : Clear TX_ABRT Interrupt
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CLR_TX_ABRT         : 1;         ///<BIT [0] Clear Transmit Abort.
        uint32_t RF0108054_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } b;    
} ClrTxAbrtIntr_t;

///  @brief 0xF0108058 : Clear RX_DONE Interrupt
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CLR_RX_DONE         : 1;         ///<BIT [0] Clear Receive Done.
        uint32_t RF0108058_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } b;    
} ClrRxDoneIntr_t;

///  @brief 0xF010805C : Clear ACTIVITY Interrupt
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CLR_ACTIVITY        : 1;         ///<BIT [0] Clear Activity.
        uint32_t RF010805C_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } b;    
} ClrActivityIntr_t;

///  @brief 0xF0108060 : Clear STOP_DET Interrupt
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CLR_STOP_DET        : 1;         ///<BIT [0] Clear Stop Condition Indicator.
        uint32_t RF0108060_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } b;    
} ClrStopDetIntr_t;

///  @brief 0xF0108064 : Clear START_DET Interrupt
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CLR_START_DET       : 1;         ///<BIT [0] Clear Start or Restart Indicator.
        uint32_t RF0108064_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } b;    
} ClrStartDetIntr_t;

///  @brief 0xF0108068 : Clear GEN_CALL Interrupt
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t CLR_GEN_CALL        : 1;         ///<BIT [0] Clear General Call Receipt.
        uint32_t RF0108068_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } b;    
} ClrGenCallIntr_t;

///  @brief 0xF010806C : I2C Enable
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t I2C_EN              : 1;         ///<BIT [0] I2C Enable.
        uint32_t RF010806C_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } b;    
} I2cEn_t;

///  @brief 0xF0108070 : I2C Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ACT_STAT            : 1;         ///<BIT [0] I2C Activity Status.
        uint32_t TFNF                : 1;         ///<BIT [1] Transmit FIFO Not Full.
        uint32_t TFE                 : 1;         ///<BIT [2] Transmit FIFO Completely Empty.
        uint32_t RFNE                : 1;         ///<BIT [3] Receive FIFO Not Empty.
        uint32_t RFF                 : 1;         ///<BIT [4] Receive FIFO Completely Full.
        uint32_t MST_ACTIVITY        : 1;         ///<BIT [5] Master Activity.
        uint32_t SLV_ACTIVITY        : 1;         ///<BIT [6] Slave Activity.
        uint32_t RF0108070_31_7_RSVD :25;         ///<BIT [31:7] Reserved.
    } b;    
} I2cStat_t;

///  @brief 0xF0108074 : I2C Transmit FIFO Level
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t TX_F_LR             : 4;         ///<BIT [3:0] Transmit FIFO Level.
        uint32_t RF0108074_31_4_RSVD :28;         ///<BIT [31:4] Reserved.
    } b;    
} I2cTxFifoLvl_t;

///  @brief 0xF0108078 : I2C Receive FIFO Level
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RX_F_LR             : 4;         ///<BIT [3:0] Receive FIFO Level.
        uint32_t RF0108078_31_4_RSVD :28;         ///<BIT [31:4] Reserved.
    } b;    
} I2cRxFifoLvl_t;

///  @brief 0xF010807C : I2C SDA Hold Time Length Register
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IC_SDA_HOLD          :16;         ///Sets the required SDA hold time in units of ic_clk period.
        uint32_t RF010807C_31_16_RSVD :16;        ///<BIT [31:16] Reserved.
    } b;    
} I2cSdaHold_t;
///  @brief 0xF0108080 : I2C Transmit Abort Source
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ABRT_7B_ADDR_NOACK    : 1;         ///<BIT [0] Abort 7-bit Address Mode Address Sent Not Acknowledged.
        uint32_t ABRT_10ADDR1_NOACK    : 1;         ///<BIT [1] Abort 10-bit Address Mode First Byte Not Acknowledged.
        uint32_t ABRT_10ADDR2_NOACK    : 1;         ///<BIT [2] Abort 10-bit Address Mode Second Byte Not Acknowledged.
        uint32_t ABRT_TXDATA_NOACK     : 1;         ///<BIT [3] Abort Transmit Data Not Acknowledged.
        uint32_t ABRT_GCALL_NOACK      : 1;         ///<BIT [4] Abort General Call Not Acknowledged.
        uint32_t ABRT_GCALL_READ       : 1;         ///<BIT [5] Abort General Call Read.
        uint32_t ABRT_HS_ACKDET        : 1;         ///<BIT [6] Abort High Speed Acknowledge.
        uint32_t ABRT_SBYTE_ACKDET     : 1;         ///<BIT [7] Abort Start Byte Acknowledge.
        uint32_t ABRT_HS_NO_RSTRT      : 1;         ///<BIT [8] Abort High Speed No Restart.
        uint32_t ABRT_SBYTE_NO_RSTRT   : 1;         ///<BIT [9] Abort Start Byte No Restart.
        uint32_t ABRT_10B_RD_NO_RSTRT  : 1;         ///<BIT [10] Abort 10-bit Read No Restart.
        uint32_t ABRT_MASTER_DIS       : 1;         ///<BIT [11] Abort Master Disabled.
        uint32_t ARB_LOST              : 1;         ///<BIT [12] Arbitration Lost.
        uint32_t ABRT_SLV_FLUSH_TXFIFO : 1;         ///<BIT [13] Abort Slave Flush Transmit FIFO.
        uint32_t ABRT_SLV_ARB_LOST     : 1;         ///<BIT [14] Abort Slave Arbitration Lost.
        uint32_t ABRT_SLV_RD_INTX      : 1;         ///<BIT [15] Abort Slave Request for Data Transmit.
        uint32_t RF0108080_31_16_RSVD  :16;         ///<BIT [31:16] Reserved.
    } b;
} I2cTxAbortSrc_t;

///  @brief 0xF0108094 : I2C SDA Setup
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SDA_SETUP           : 8;         ///<BIT [7:0] SDA Setup.
        uint32_t RF0108094_31_8_RSVD :24;         ///<BIT [31:8] Reserved.
    } b;    
} I2cSdaSetup_t;

///  @brief 0xF0108098 : I2C ACK General Call
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ACK_GEN_CALL        : 1;         ///<BIT [0] Acknowledge General Call.
        uint32_t RF0108098_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } b;    
} I2cAckGeneralCall_t;

///  @brief 0xF010809C : I2C Enable Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t IC_EN_STATUS            : 1;         ///<BIT [0] IC Enable Status.
        uint32_t SLV_DISABLED_WHILE_BUSY : 1;         ///<BIT [1] Slave Disabled While Busy (Transmit, Receive).
        uint32_t SLV_RX_DATA_LOST        : 1;         ///<BIT [2] Slave Receive Data Lost.
        uint32_t RF010809C_31_3_RSVD     :29;         ///<BIT [31:3] Reserved.
    } b;    
} I2cEnStat_t;

///  @brief 0xF01080F4 : Component Parameter 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t APB_DATA_WIDTH       : 2;         ///<BIT [1:0] APB_DATA_WIDTH Value.
        uint32_t MAX_SPEED_MODE       : 2;         ///<BIT [3:2] IC_MAX_SPEED_MODE Value.
        uint32_t HC_COUNT_VALUES      : 1;         ///<BIT [4] IC_HC_COUNT VALUES Value.
        uint32_t INTR_IO              : 1;         ///<BIT [5] IC_INTR_IO Value.
        uint32_t HAS_DMA              : 1;         ///<BIT [6] IC_HAS_DMA Value.
        uint32_t ADD_ENCODED_PARAMS   : 1;         ///<BIT [7] IC_ADD_ENCODED_PARAMS Value.
        uint32_t RX_BUFFER_DEPTH      : 8;         ///<BIT [15:8] IC_RX_BUFFER_DEPTH Value.
        uint32_t TX_BUFFER_DEPTH      : 8;         ///<BIT [23:16] IC_TX_BUFFER_DEPTH Value.
        uint32_t RF01080F4_31_24_RSVD : 8;         ///<BIT [31:24] Reserved.
    } b;    
} ComponenParam1_t;


/// @brief 0xF0108000-0xF0108100 : I2c_t MODULE
typedef struct
{
    I2cCtrl_t i2cCtrl;                                                 ///< 0xF0108000 : I2C Control
    I2cTgtAddr_t i2cTgtAddr;                                           ///< 0xF0108004 : I2C Target Address
    I2cSlaveAddr_t i2cSlaveAddr;                                       ///< 0xF0108008 : I2C Slave Address
    I2cHighSpeedMstrModeCmemAddr_t i2cHighSpeedMstrModeCmemAddr;       ///< 0xF010800C : I2C High Speed Master Mode Code Address
    I2cRxTxDataBufAndCmd_t i2cRxTxDataBufAndCmd;                       ///< 0xF0108010 : I2C Rx/Tx Data Buffer and Command
    I2cStandardSpeedClkSclHighCnt_t i2cStandardSpeedClkSclHighCnt;     ///< 0xF0108014 : I2C Standard Speed Clock SCL High Count
    I2cStandardSpeedClkSclLowCnt_t i2cStandardSpeedClkSclLowCnt;       ///< 0xF0108018 : I2C Standard Speed Clock SCL Low Count
    I2cFastSpeedClkSclHighCnt_t i2cFastSpeedClkSclHighCnt;             ///< 0xF010801C : I2C Fast Speed Clock SCL High Count
    I2cFastSpeedClkSclLowCnt_t i2cFastSpeedClkSclLowCnt;               ///< 0xF0108020 : I2C Fast Speed Clock SCL Low Count
    I2cHighSpeedClkSclHighCnt_t i2cHighSpeedClkSclHighCnt;             ///< 0xF0108024 : I2C High Speed Clock SCL High Count
    I2cHighSpeedClkSclLowCnt_t i2cHighSpeedClkSclLowCnt;               ///< 0xF0108028 : I2C High Speed Clock SCL Low Count
    I2cIntrStat_t i2cIntrStat;                                         ///< 0xF010802C : I2C Interrupt Status
    I2cIntrMsk_t i2cIntrMsk;                                           ///< 0xF0108030 : I2C Interrupt Mask
    I2cRawIntrStat_t i2cRawIntrStat;                                   ///< 0xF0108034 : I2C Raw Interrupt Status
    I2cRxFifoThr_t i2cRxFifoThr;                                       ///< 0xF0108038 : I2C Receive FIFO Threshold
    I2cTxFifoThr_t i2cTxFifoThr;                                       ///< 0xF010803C : I2C Transmit FIFO Threshold
    ClrCombinedAndIndividuIntr_t clrCombinedAndIndividuIntr;           ///< 0xF0108040 : Clear Combined and Individual Interrupt
    ClrRxUnderIntr_t clrRxUnderIntr;                                   ///< 0xF0108044 : Clear RX_UNDER Interrupt
    ClrRxOverIntr_t clrRxOverIntr;                                     ///< 0xF0108048 : Clear RX_OVER Interrupt
    ClrTxOverIntr_t clrTxOverIntr;                                     ///< 0xF010804C : Clear TX_OVER Interrupt
    ClrRdReqIntr_t clrRdReqIntr;                                       ///< 0xF0108050 : Clear RD_REQ Interrupt
    ClrTxAbrtIntr_t clrTxAbrtIntr;                                     ///< 0xF0108054 : Clear TX_ABRT Interrupt
    ClrRxDoneIntr_t clrRxDoneIntr;                                     ///< 0xF0108058 : Clear RX_DONE Interrupt
    ClrActivityIntr_t clrActivityIntr;                                 ///< 0xF010805C : Clear ACTIVITY Interrupt
    ClrStopDetIntr_t clrStopDetIntr;                                   ///< 0xF0108060 : Clear STOP_DET Interrupt
    ClrStartDetIntr_t clrStartDetIntr;                                 ///< 0xF0108064 : Clear START_DET Interrupt
    ClrGenCallIntr_t clrGenCallIntr;                                   ///< 0xF0108068 : Clear GEN_CALL Interrupt
    I2cEn_t i2cEn;                                                     ///< 0xF010806C : I2C Enable
    I2cStat_t i2cStat;                                                 ///< 0xF0108070 : I2C Status
    I2cTxFifoLvl_t i2cTxFifoLvl;                                       ///< 0xF0108074 : I2C Transmit FIFO Level
    I2cRxFifoLvl_t i2cRxFifoLvl;                                       ///< 0xF0108078 : I2C Receive FIFO Level
    I2cSdaHold_t i2cSdaHold;										   ///< 0xF010807C - I2C SDA Data Hold Time
    I2cTxAbortSrc_t i2cTxAbortSrc;                                     ///< 0xF0108080 : I2C Transmit Abort Source
    uint32_t RF0108084_31_0_RSVD;                                      ///< 0xF0108084 : Reserved
    uint8_t rsvd_RF0108088[12];                                        ///< 0xF0108088 - 0xF0108093 : Reserved.
    I2cSdaSetup_t i2cSdaSetup;                                         ///< 0xF0108094 : I2C SDA Setup
    I2cAckGeneralCall_t i2cAckGeneralCall;                             ///< 0xF0108098 : I2C ACK General Call
    I2cEnStat_t i2cEnStat;                                             ///< 0xF010809C : I2C Enable Status
    uint8_t rsvd_RF01080A0[84];                                        ///< 0xF01080A0 - 0xF01080F3 : Reserved.
    ComponenParam1_t componenParam1;                                   ///< 0xF01080F4 : Component Parameter 1
    uint8_t rsvd_RF01080F8[4];                                         ///< 0xF01080F8 - 0xF01080FB : Reserved.
    uint32_t IC_COMP_TYPE;                                             ///< 0xF01080FC : I2C Component Type
} I2c_t;
COMPILE_ASSERT(offsetof(I2c_t,IC_COMP_TYPE)==0xF01080FC-0xF0108000,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile I2c_t rI2c0;                          ///< 0xF0108000-0xF0108100 : I2c_t MODULE 0
extern volatile I2c_t rI2c1;                          ///< 0xF0109000-0xF0109100 : I2c_t MODULE 1

//-----------------------------------------------------------------------------
//  Exported register reference (Non volatile type for optimization)
//-----------------------------------------------------------------------------
