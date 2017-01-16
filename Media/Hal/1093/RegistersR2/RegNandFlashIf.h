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
//! @brief NAND Flash Interface Registers
//! 
//! Doc Version: STANDARD
//! Doc Date: 05/11/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  NAND Flash Interface Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xF200BA00 : NAND Flash Controller Interrupt Status
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200BA00_0_RSVD    : 1;         ///<BIT [0] Reserved.
        uint16_t FLASH_INT_STAT      : 1;         ///<BIT [1] NAND Flash Interface Interrupt Status.
        uint16_t SEQ_INT_STAT        : 1;         ///<BIT [2] Sequencer interrupt Status.
        uint16_t FLC_INT_STAT        : 1;         ///<BIT [3] Flash Linked List Controller Interrupt Status.
        uint16_t ECU_INT_STAT        : 1;         ///<BIT [4] ECU to SoC CPU Interrupt Status.
        uint16_t RF200BA00_6_5_RSVD  : 2;         ///<BIT [6:5] Reserved.
        uint16_t DGE_INT_STAT        : 1;         ///<BIT [7] Descriptor Generation Engine Interrupt Status.
        uint16_t RF200BA00_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
    } b;    
} NandFlashControllIntrStat_t;

///  @brief 0xF200BA02 : NAND Flash Controller Interrupt Enable for SoC CPU
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200BA02_0_RSVD    : 1;         ///<BIT [0] Reserved.
        uint16_t FLASH_INT_EN        : 1;         ///<BIT [1] NAND Flash Interface Interrupt Enable.
        uint16_t SEQ_INT_EN          : 1;         ///<BIT [2] Sequencer interrupt Enable.
        uint16_t FLC_INT_EN          : 1;         ///<BIT [3] FLC Interrupt Enable.
        uint16_t RF200BA02_6_4_RSVD  : 3;         ///<BIT [6:4] Reserved.
        uint16_t DGE_INT_EN          : 1;         ///<BIT [7] Descriptor Generation Engine Interrupt Enable.
        uint16_t RF200BA02_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
    } b;    
} NandFlashControllIntrEnForSocCpu_t;

///  @brief 0xF200BA04 : NAND Flash Controller Test Bus Select
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t FCT_TEST_BUS_SEL    : 3;         ///<BIT [2:0] Flash Controller Top Bus Select.
        uint16_t RF200BA04_15_3_RSVD :13;         ///<BIT [15:3] Reserved.
    } b;    
} NandFlashControllTstBusSel_t;

///  @brief 0xF200BA08 : NAND Flash Interface Driver and Sequencer Revision ID
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t SEQ_BLK_ID    : 8;         ///<BIT [7:0] Sequencer Block ID.
        uint16_t NFLASH_BLK_ID : 8;         ///<BIT [15:8] NAND Flash Interface Revision ID.
    } b;    
} NandFlashInterfacDriverAndSeqRevisionId_t;

///  @brief 0xF200BA0A : ECC and Revision ID
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200BA0A_7_0_RSVD : 8;         ///<BIT [7:0] Reserved.
        uint16_t ECC_BLK_ID         : 8;         ///<BIT [15:8] ECC Revision ID.
    } b;    
} EccAndRevisionId_t;

///  @brief 0xF200BA1A : NAND Flash Controller ECU Interrupt Enable for SoC CPU
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ECU_INT_EN          : 4;         ///<BIT [3:0] ECU Interrupt Enable.
        uint16_t RF200BA1A_15_4_RSVD :12;         ///<BIT [15:4] Reserved.
    } b;    
} NandFlashControllEcuIntrEnForSoc_t;

///  @brief 0xF200E000 : Flash Interface Configuration
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t OP_MODE           : 2;         ///<BIT [1:0] NAND Flash Operation Mode.
        uint16_t FLASH_TYPE_L      : 2;         ///<BIT [3:2] NAND Flash Device Type Low.
        uint16_t FLASH_BW          : 1;         ///<BIT [4] NAND Flash Data-Bus Width.
        uint16_t RF200E000_5_RSVD  : 1;         ///<BIT [5] Reserved.
        uint16_t IF_RESET          : 1;         ///<BIT [6] Flash Interface Logic Reset.
        uint16_t PD_EN             : 1;         ///<BIT [7] Power-Down Enable.
        uint16_t FLASH_TYPE_H      : 1;         ///<BIT [8] NAND Flash Device Type High.
        uint16_t CLK_PD_EN         : 1;         ///<BIT [9] Clock Signal Power-Down Enable.
        uint16_t EXT_ROW_EN        : 1;         ///<BIT [10] Row Address Length.
        uint16_t ECE_EN            : 1;         ///<BIT [11] Encoded Chip-Enable Mode.
        uint16_t CLK_STOP_EN       : 1;         ///<BIT [12] Clock Stop Enable.
        uint16_t RF200E000_13_RSVD : 1;         ///<BIT [13] Reserved.
        uint16_t A15CE_EN          : 1;         ///<BIT [14] Fifteen Chip-Enable Mode.
        uint16_t RF200E000_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} FlashInterfacCfg_t;

///  @brief 0xF200E002 : Flash Direct/Indirect Access Configuration
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DEVICE_DIR_IND       : 8;         ///<BIT [7:0] Direct or Indirect Device Number.
        uint16_t CHANNEL_DIR_IND      : 4;         ///<BIT [11:8] Direct or Indirect Channel Number.
        uint16_t EN_AW_IND            : 1;         ///<BIT [12] Enable Auto-Wait Function in Indirect Mode.
        uint16_t RF200E002_15_13_RSVD : 3;         ///<BIT [15:13] Reserved.
    } b;    
} FlashDirectIndAccessCfg_t;

///  @brief 0xF200E004 : Flash Interface Timing Control 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t T1_SEQ_ASYN1         : 3;         ///<BIT [2:0] T1 Sequencer Mode Asynchronous/SDR Device Read Enable (RE_N) Lo.
        uint16_t T2_SEQ_ASYN1         : 3;         ///<BIT [5:3] T2 Sequencer Mode Asynchronous/SDR Device Read Enable (RE_N) Hi.
        uint16_t T3_SEQ_ASYN1         : 3;         ///<BIT [8:6] T3 Sequencer Mode Asynchronous/SDR Device Write Enable (WE_N) L.
        uint16_t T4_SEQ_ASYN1         : 3;         ///<BIT [11:9] T4 Sequencer Mode Asynchronous/SDR Device Write Enable (WE_N) H.
        uint16_t T5_SEQ_ASYN1         : 2;         ///<BIT [13:12] T5 Sequencer Mode Asynchronous/SDR Device Read Data Latch Timin.
        uint16_t RF200E004_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
    } b;    
} FlashInterfacTmCtrl1_t;

///  @brief 0xF200E006 : Flash Interface Timing Control 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t T1_SEQ_ASYN2         : 3;         ///<BIT [2:0] T1 Sequencer Mode Asynchronous/SDR Device Read Enable (RE_N) Lo.
        uint16_t T2_SEQ_ASYN2         : 3;         ///<BIT [5:3] T2 Sequencer Mode Asynchronous/SDR Device Read Enable (RE_N) Hi.
        uint16_t T3_SEQ_ASYN2         : 3;         ///<BIT [8:6] T3 Sequencer Mode Asynchronous/SDR Device Write Enable (WE_N) L.
        uint16_t T4_SEQ_ASYN2         : 3;         ///<BIT [11:9] T4 Sequencer Mode Asynchronous/SDR Device Write Enable (WE_N) H.
        uint16_t T5_SEQ_ASYN2         : 2;         ///<BIT [13:12] T5 Sequencer Mode Asynchronous/SDR Device Read Data Latch Timin.
        uint16_t RF200E006_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
    } b;    
} FlashInterfacTmCtrl2_t;

///  @brief 0xF200E008 : Flash Interface Timing Control 3 (Asynchronous Device and SDR Device)
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t T1_IND_ASYN          : 3;         ///<BIT [2:0] T1 Indirect Mode Asynchronous/SDR Device Read Enable (RE_N) Low.
        uint16_t T2_IND_ASYN          : 3;         ///<BIT [5:3] T2 Indirect Mode Asynchronous/SDR Device Read Enable (RE_N) Hig.
        uint16_t T3_IND_ASYN          : 3;         ///<BIT [8:6] T3 Indirect Mode Asynchronous/SDR Device Write Enable (WE_N) Lo.
        uint16_t T4_IND_ASYN          : 3;         ///<BIT [11:9] T4 Indirect Mode Asynchronous/SDR Device Write Enable (WE_N) Hi.
        uint16_t T5_IND_ASYN          : 2;         ///<BIT [13:12] T5 Indirect Mode Asynchronous/SDR Device Read Data Latch Timing.
        uint16_t RF200E008_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
    } b;    
} FlashInterfacTmCtrl3AsynchroDeviceAnd_t;

///  @brief 0xF200E00A : Flash Interface Timing Control 4 (Toggle 1/Toggle2/NV-DDR2 Device)
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t T1_SEQ_JDC           : 3;         ///<BIT [2:0] T1 Sequencer Mode Toggle1/Toggle2 and NV-DDR2 Device Read Enabl.
        uint16_t T2_SEQ_JDC           : 3;         ///<BIT [5:3] T2 Sequencer Mode Toggle1/Toggle2 and NV-DDR2 Device Read Enabl.
        uint16_t T3_SEQ_JDC           : 3;         ///<BIT [8:6] T3 Sequencer Mode Toggle1/Toggle2 and NV-DDR2 Device Write Enab.
        uint16_t T4_SEQ_JDC           : 3;         ///<BIT [11:9] T4 Sequencer Mode Toggle1/Toggle2 and NV-DDR2 Device Write Enab.
        uint16_t RF200E00A_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
    } b;    
} FlashInterfacTmCtrl4Toggle1Toggle2_t;

///  @brief 0xF200E00C : Flash Direct Access Control Port 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t CLE_CTRL  : 1;         ///<BIT [0] Command Latch Enable Control.
        uint16_t ALE_CTRL  : 1;         ///<BIT [1] Address Latch Enable Control.
        uint16_t RE_N_CTRL : 1;         ///<BIT [2] Read Enable Control.
        uint16_t WE_N_CTRL : 1;         ///<BIT [3] Write Enable Control.
        uint16_t WP_N_CTRL : 1;         ///<BIT [4] Write Protect Control.
        uint16_t DQS_OE    : 1;         ///<BIT [5] DQS Output Enable.
        uint16_t CE_N_CTRL : 1;         ///<BIT [6] Chip Enable Control.
        uint16_t OE_EN     : 1;         ///<BIT [7] Flash Data Bus[7:0] Output Mode Enable.
        uint16_t MON_CLE   : 1;         ///<BIT [8] Monitor Command Latch Enable.
        uint16_t MON_ALE   : 1;         ///<BIT [9] Monitor Address Latch Enable.
        uint16_t MON_RE_N  : 1;         ///<BIT [10] Monitor Read Enable.
        uint16_t MON_WE_N  : 1;         ///<BIT [11] Monitor Write Enable.
        uint16_t MON_WP_N  : 1;         ///<BIT [12] Monitor Write Protect.
        uint16_t DQS       : 1;         ///<BIT [13] DQS.
        uint16_t MON_CE_N  : 1;         ///<BIT [14] Monitor Chip Enable.
        uint16_t MON_DQS   : 1;         ///<BIT [15] Monitor DQS Signal.
    } b;    
} FlashDirectAccessCtrlPort1_t;

///  @brief 0xF200E010 : Flash Interface Data Latch Timing Control Channel 0 and Channel 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ASYNC_DLY_CTRL_CH0 : 7;         ///<BIT [6:0] Asynchronous/SDR Device Read Enable Delay Chain Control for Cha.
        uint16_t ES_CH0             : 1;         ///<BIT [7] Edge Selection for Channel 0.
        uint16_t ASYNC_DLY_CTRL_CH1 : 7;         ///<BIT [14:8] Asynchronous/SDR Device Read Enable Delay Chain Control for Cha.
        uint16_t ES_CH1             : 1;         ///<BIT [15] Edge Selection for Channel 1.
    } b;    
} FlashInterfacDataLatchTmCtrlChan0_t;

///  @brief 0xF200E012 : Flash Interface Data Latch Timing Control Channel 2 and Channel 3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ASYNC_DLY_CTRL_CH2 : 7;         ///<BIT [6:0] Asynchronous/SDR Device Read Enable Delay Chain Control for Cha.
        uint16_t ES_CH2             : 1;         ///<BIT [7] Edge Selection for Channel 2.
        uint16_t ASYNC_DLY_CTRL_CH3 : 7;         ///<BIT [14:8] Asynchronous/SDR Device Read Enable Delay Chain Control for Cha.
        uint16_t ES_CH3             : 1;         ///<BIT [15] Edge Selection for Channel 3.
    } b;    
} FlashInterfacDataLatchTmCtrlChan2_t;

///  @brief 0xF200E014 : Flash Interface Data Latch Timing Control Channel 4 and Channel 5
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ASYNC_DLY_CTRL_CH4 : 7;         ///<BIT [6:0] Asynchronous/SDR Device Read Enable Delay Chain Control for Cha.
        uint16_t ES_CH4             : 1;         ///<BIT [7] Edge Selection for Channel 4.
        uint16_t ASYNC_DLY_CTRL_CH5 : 7;         ///<BIT [14:8] Asynchronous/SDR Device Read Enable Delay Chain Control for Cha.
        uint16_t ES_CH5             : 1;         ///<BIT [15] Edge Selection for Channel 5.
    } b;    
} FlashInterfacDataLatchTmCtrlChan4_t;

///  @brief 0xF200E016 : Flash Interface Data Latch Timing Control Channel 6 and Channel 7
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ASYNC_DLY_CTRL_CH6 : 7;         ///<BIT [6:0] Asynchronous/SDR Device Read Enable Delay Chain Control for Cha.
        uint16_t ES_CH6             : 1;         ///<BIT [7] Edge Selection for Channel 6.
        uint16_t ASYNC_DLY_CTRL_CH7 : 7;         ///<BIT [14:8] Asynchronous/SDR Device Read Enable Delay Chain Control for Cha.
        uint16_t ES_CH7             : 1;         ///<BIT [15] Edge Selection for Channel 7.
    } b;    
} FlashInterfacDataLatchTmCtrlChan6_t;

///  @brief 0xF200E020 : Flash Indirect Access Configuration
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t CMD_NUM              : 4;         ///<BIT [3:0] Number of Transfer Command and Address Bytes.
        uint16_t IN_OUT               : 1;         ///<BIT [4] Input Or Output Mode.
        uint16_t CT                   : 1;         ///<BIT [5] Command Latch Enable.
        uint16_t FIFO_RESET           : 1;         ///<BIT [6] FIFO Pointer Reset.
        uint16_t START                : 1;         ///<BIT [7] Start Command Operation.
        uint16_t DUMMY_RBB            : 1;         ///<BIT [8] Dummy Ready or Busy Condition.
        uint16_t RF200E020_9_RSVD     : 1;         ///<BIT [9] Reserved.
        uint16_t RF200E020_10_RSVD    : 1;         ///<BIT [10] Reserved.
        uint16_t RF200E020_13_11_RSVD : 3;         ///<BIT [13:11] Reserved.
        uint16_t FIFO_STATUS          : 1;         ///<BIT [14] FIFO Status.
        uint16_t RF200E020_15_RSVD    : 1;         ///<BIT [15] Reserved.
    } b;    
} FlashIndAccessCfg_t;

///  @brief 0xF200E022 : Flash Indirect Access Command Port 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t BYTE0_IND : 8;         ///<BIT [7:0] Indirect Mode Byte 0.
        uint16_t BYTE1_IND : 8;         ///<BIT [15:8] Indirect Mode Byte 1.
    } b;    
} FlashIndAccessCmdPort1_t;

///  @brief 0xF200E024 : Flash Indirect Access Command Port 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t BYTE2_IND : 8;         ///<BIT [7:0] Indirect Mode Byte 2.
        uint16_t BYTE3_IND : 8;         ///<BIT [15:8] Indirect Mode Byte 3.
    } b;    
} FlashIndAccessCmdPort2_t;

///  @brief 0xF200E026 : Flash Indirect Access Command Port 3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t BYTE4_IND : 8;         ///<BIT [7:0] Indirect Mode Byte 4.
        uint16_t BYTE5_IND : 8;         ///<BIT [15:8] Indirect Mode Byte 5.
    } b;    
} FlashIndAccessCmdPort3_t;

///  @brief 0xF200E028 : Flash Indirect Access Command Port 4
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t BYTE6_IND : 8;         ///<BIT [7:0] Indirect Mode Byte 6.
        uint16_t BYTE7_IND : 8;         ///<BIT [15:8] Indirect Mode Byte 7.
    } b;    
} FlashIndAccessCmdPort4_t;

///  @brief 0xF200E030 : Flash Interface Timing Control 5
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t T7_IND_JDC          : 6;         ///<BIT [5:0] T7 Delay Setting for DQS/QS Signal in Indirect and Toggle1 Mode.
        uint16_t RF200E030_15_6_RSVD :10;         ///<BIT [15:6] Reserved.
    } b;    
} FlashInterfacTmCtrl5_t;

///  @brief 0xF200E032 : Flash Interface Timing Control 6
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t T8_SEQ : 4;         ///<BIT [3:0] T8 Sequencer tADL Parameter.
        uint16_t T9_SEQ : 4;         ///<BIT [7:4] T9 Sequencer tWHR Parameter.
        uint16_t T8_IND : 4;         ///<BIT [11:8] T8 Indirect tADL Parameter.
        uint16_t T9_IND : 4;         ///<BIT [15:12] T9 Indirect tWHR Parameter.
    } b;    
} FlashInterfacTmCtrl6_t;

///  @brief 0xF200E034 : Flash Interface Timing Control 7
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t TCS                  : 4;         ///<BIT [3:0] tCS Parameter.
        uint16_t TCEH                 : 4;         ///<BIT [7:4] tCEH Parameter.
        uint16_t TCWAW                : 6;         ///<BIT [13:8] tCWAW Parameter.
        uint16_t RF200E034_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
    } b;    
} FlashInterfacTmCtrl7_t;

///  @brief 0xF200E036 : Flash Interface Timing Control 8
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t TWPST  : 4;         ///<BIT [3:0] tWPST Parameter.
        uint16_t TWPSTH : 4;         ///<BIT [7:4] tWPSTH Parameter.
        uint16_t TRPST  : 4;         ///<BIT [11:8] tRPST Parameter.
        uint16_t TRPSTH : 4;         ///<BIT [15:12] tRPSTH Parameter.
    } b;    
} FlashInterfacTmCtrl8_t;

///  @brief 0xF200E038 : Flash Interface Timing Control 9
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t TWB   : 4;         ///<BIT [3:0] tWB Parameter.
        uint16_t TWPRE : 4;         ///<BIT [7:4] tWPRE Parameter.
        uint16_t TRPRE : 4;         ///<BIT [11:8] tRPRE Parameter.
        uint16_t TRHW  : 4;         ///<BIT [15:12] tRHW Parameter.
    } b;    
} FlashInterfacTmCtrl9_t;

///  @brief 0xF200E03E : Flash Sequencer Control 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DUMMY_COM          : 1;         ///<BIT [0] Dummy Command.
        uint16_t RF200E03E_2_1_RSVD : 2;         ///<BIT [2:1] Reserved.
        uint16_t SQRA_EN            : 1;         ///<BIT [3] Enable Register Access Mode.
        uint16_t ISEQ_SEL           : 1;         ///<BIT [4] Select Multi-LUN Read Sequence.
#if (_SOC_REV_ >= 0x0210)
        uint16_t RF200E03E_5_RSVD   : 1;         ///<BIT [5] Reserved.
        uint16_t DPP_STATUS         : 1;         ///<BIT [6] DPP Error Status.
#else
        uint16_t RF200E03E_6_5_RSVD : 2;         ///<BIT [6:5] Reserved.
#endif
        uint16_t SM_IDLE            : 1;         ///<BIT [7] Sequencer Module Idle Status.
        uint16_t ERR_STR_SEL        : 4;         ///<BIT [11:8] Error Status Channel Select.
        uint16_t ERR_STR            : 2;         ///<BIT [13:12] Error Status.
        uint16_t RF200E03E_14_RSVD  : 1;         ///<BIT [14] Reserved.
        uint16_t INT_EN             : 1;         ///<BIT [15] Interrupt Enable.
    } b;    
} FlashSeqCtrl2_t;

///  @brief 0xF200E040 : Flash Sequencer Control 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t FSEQ_EN              : 1;         ///<BIT [0] Flash Sequencer Logic Enable.
        uint16_t FSEQ_RESET           : 1;         ///<BIT [1] Flash Sequencer Logic Reset.
        uint16_t FAIL_BIT0            : 3;         ///<BIT [4:2] Fail Bit 0.
        uint16_t READY_BIT0           : 3;         ///<BIT [7:5] Ready Bit 0.
        uint16_t RF200E040_11_8_RSVD  : 4;         ///<BIT [11:8] Reserved.
        uint16_t RF200E040_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
    } b;    
} FlashSeqCtrl1_t;

///  @brief 0xF200E042 : Flash Sequencer Command Port 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RBYTE0 : 8;         ///<BIT [7:0] Read Byte 0.
        uint16_t RBYTE1 : 8;         ///<BIT [15:8] Read Byte 1.
    } b;    
} FlashSeqCmdPort1_t;

///  @brief 0xF200E044 : Flash Sequencer Command Port 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RBYTE2   : 8;         ///<BIT [7:0] Read Byte 2.
        uint16_t PREBYTE0 : 8;         ///<BIT [15:8] Pre-Command Byte 0.
    } b;    
} FlashSeqCmdPort2_t;

///  @brief 0xF200E046 : Flash Sequencer Command Port 3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PBYTE0 : 8;         ///<BIT [7:0] Program Byte 0.
        uint16_t PBYTE1 : 8;         ///<BIT [15:8] Program Byte 1.
    } b;    
} FlashSeqCmdPort3_t;

///  @brief 0xF200E048 : Flash Sequencer Command Port 4
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t EBYTE0 : 8;         ///<BIT [7:0] Erase Byte 0.
        uint16_t EBYTE1 : 8;         ///<BIT [15:8] Erase Byte 1.
    } b;    
} FlashSeqCmdPort4_t;

///  @brief 0xF200E04A : Flash Sequencer Command Port 5
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t SBYTE0     : 8;         ///<BIT [7:0] Status Byte 0.
        uint16_t ABYTE_ASYN : 8;         ///<BIT [15:8] Asynchronous Abort Byte.
    } b;    
} FlashSeqCmdPort5_t;

///  @brief 0xF200E04C : Flash Sequencer Command Port 6
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ABYTE_SYN : 8;         ///<BIT [7:0] NV-DDR Abort Byte.
        uint16_t PREBYTE1  : 8;         ///<BIT [15:8] Pre-Command Byte 1.
    } b;    
} FlashSeqCmdPort6_t;

///  @brief 0xF200E060 : Flash Sequencer Interface Timing Channel 0 Configuration
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t TIMING_SEL_CH0      : 8;         ///<BIT [7:0] T1 through T6 Timing for Channel 0.
        uint16_t RF200E060_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
    } b;    
} FlashSeqInterfacTmChan0Cfg_t;

///  @brief 0xF200E062 : Flash Sequencer Interface Timing Channel 1 Configuration
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t TIMING_SEL_CH1      : 8;         ///<BIT [7:0] T1 through T6 Timing for Channel 1.
        uint16_t RF200E062_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
    } b;    
} FlashSeqInterfacTmChan1Cfg_t;

///  @brief 0xF200E064 : Flash Sequencer Interface Timing Channel 2 Configuration
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t TIMING_SEL_CH2      : 8;         ///<BIT [7:0] T1 through T6 Timing for Channel 2.
        uint16_t RF200E064_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
    } b;    
} FlashSeqInterfacTmChan2Cfg_t;

///  @brief 0xF200E066 : Flash Sequencer Interface Timing Channel 3 Configuration
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t TIMING_SEL_CH3      : 8;         ///<BIT [7:0] T1 through T6 Timing for Channel 3.
        uint16_t RF200E066_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
    } b;    
} FlashSeqInterfacTmChan3Cfg_t;

///  @brief 0xF200E068 : Flash Sequencer Interface Timing Channel 4 Configuration
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t TIMING_SEL_CH4      : 8;         ///<BIT [7:0] T1 through T6 Timing for Channel 4.
        uint16_t RF200E068_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
    } b;    
} FlashSeqInterfacTmChan4Cfg_t;

///  @brief 0xF200E06A : Flash Sequencer Interface Timing Channel 5 Configuration
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t TIMING_SEL_CH5      : 8;         ///<BIT [7:0] T1 through T6 Timing for Channel 5.
        uint16_t RF200E06A_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
    } b;    
} FlashSeqInterfacTmChan5Cfg_t;

///  @brief 0xF200E06C : Flash Sequencer Interface Timing Channel 6 Configuration
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t TIMING_SEL_CH6      : 8;         ///<BIT [7:0] T1 through T6 Timing for Channel 6.
        uint16_t RF200E06C_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
    } b;    
} FlashSeqInterfacTmChan6Cfg_t;

///  @brief 0xF200E06E : Flash Sequencer Interface Timing Channel 7 Configuration
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t TIMING_SEL_CH7      : 8;         ///<BIT [7:0] T1 through T6 Timing for Channel 7.
        uint16_t RF200E06E_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
    } b;    
} FlashSeqInterfacTmChan7Cfg_t;

///  @brief 0xF200E070 : Flash Sequencer Interface Timing Control 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t WCNT12           : 7;         ///<BIT [6:0] Wait Counter 12.
        uint16_t RF200E070_7_RSVD : 1;         ///<BIT [7] Reserved.
        uint16_t WCNT11           : 6;         ///<BIT [13:8] Wait Counter 11.
        uint16_t DIV1             : 2;         ///<BIT [15:14] Flash Clock Divider 1.
    } b;    
} FlashSeqInterfacTmCtrl1_t;

///  @brief 0xF200E072 : Flash Sequencer Interface Timing Control 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t WCNT22           : 7;         ///<BIT [6:0] Wait Counter 22.
        uint16_t RF200E072_7_RSVD : 1;         ///<BIT [7] Reserved.
        uint16_t WCNT21           : 6;         ///<BIT [13:8] Wait Counter 21.
        uint16_t DIV2             : 2;         ///<BIT [15:14] Flash Clock Divider 2.
    } b;    
} FlashSeqInterfacTmCtrl2_t;

///  @brief 0xF200E074 : Flash Sequencer Interface Timing Control 3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t WCNT32           : 7;         ///<BIT [6:0] Wait Counter 32.
        uint16_t RF200E074_7_RSVD : 1;         ///<BIT [7] Reserved.
        uint16_t WCNT31           : 6;         ///<BIT [13:8] Wait Counter 31.
        uint16_t DIV3             : 2;         ///<BIT [15:14] Flash Clock Divider 3.
    } b;    
} FlashSeqInterfacTmCtrl3_t;

///  @brief 0xF200E076 : Flash Sequencer Interface Timing Control 4
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t WCNT42           : 7;         ///<BIT [6:0] Wait Counter 42.
        uint16_t RF200E076_7_RSVD : 1;         ///<BIT [7] Reserved.
        uint16_t WCNT41           : 6;         ///<BIT [13:8] Wait Counter 41.
        uint16_t DIV4             : 2;         ///<BIT [15:14] Flash Clock Divider 4.
    } b;    
} FlashSeqInterfacTmCtrl4_t;

///  @brief 0xF200E078 : Flash Sequencer Interface Timing Control 5
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PCNT52               : 7;         ///<BIT [6:0] Polling Counter 52.
        uint16_t RF200E078_7_RSVD     : 1;         ///<BIT [7] Reserved.
        uint16_t PCNT51               : 4;         ///<BIT [11:8] Polling Counter 51.
        uint16_t RF200E078_13_12_RSVD : 2;         ///<BIT [13:12] Reserved.
        uint16_t DIV5                 : 2;         ///<BIT [15:14] Flash Clock Divider 5.
    } b;    
} FlashSeqInterfacTmCtrl5_t;

///  @brief 0xF200E07A : Flash Sequencer Interface Timing Control 6
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t WCNT62           : 7;         ///<BIT [6:0] Wait Counter 62.
        uint16_t RF200E07A_7_RSVD : 1;         ///<BIT [7] Reserved.
        uint16_t WCNT61           : 6;         ///<BIT [13:8] Wait Counter 61.
        uint16_t DIV6             : 2;         ///<BIT [15:14] Flash Clock Divider 6.
    } b;    
} FlashSeqInterfacTmCtrl6_t;

///  @brief 0xF200E07C : Flash Sequencer Interface Timing Control 7
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t WCNT72           : 7;         ///<BIT [6:0] Wait Counter 72.
        uint16_t RF200E07C_7_RSVD : 1;         ///<BIT [7] Reserved.
        uint16_t WCNT71           : 6;         ///<BIT [13:8] Wait Counter 71.
        uint16_t DIV7             : 2;         ///<BIT [15:14] Flash Clock Divider 7.
    } b;    
} FlashSeqInterfacTmCtrl7_t;

///  @brief 0xF200E07E : Flash Sequencer Interface Timing Control 8
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t VMT_T3               : 5;         ///<BIT [4:0] Vendor Mode T3 Timing.
        uint16_t RF200E07E_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint16_t VMT_T4               : 5;         ///<BIT [12:8] Vendor Mode T4 Timing.
        uint16_t RF200E07E_15_13_RSVD : 3;         ///<BIT [15:13] Reserved.
    } b;    
} FlashSeqInterfacTmCtrl8_t;

///  @brief 0xF200E080 : Flash Sequencer Channel Engine Clock Control 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PD_CH0        : 1;         ///<BIT [0] Power Down Channel 0 Engine and EF FIFO.
        uint16_t PD_CH1        : 1;         ///<BIT [1] Power Down Channel 1 Engine and EF FIFO.
        uint16_t PD_CH2        : 1;         ///<BIT [2] Power Down Channel 2 Engine and EF FIFO.
        uint16_t PD_CH3        : 1;         ///<BIT [3] Power Down Channel 3 Engine and EF FIFO.
        uint16_t PD_CH4        : 1;         ///<BIT [4] Power Down Channel 4 Engine and EF FIFO.
        uint16_t PD_CH5        : 1;         ///<BIT [5] Power Down Channel 5 Engine and EF FIFO.
        uint16_t PD_CH6        : 1;         ///<BIT [6] Power Down Channel 6 Engine and EF FIFO.
        uint16_t PD_CH7        : 1;         ///<BIT [7] Power Down Channel 7 Engine and EF FIFO.
        uint16_t EXT_CE_EN_CH0 : 1;         ///<BIT [8] Extended Chip Enable Control 1 for Channel 0.
        uint16_t EXT_CE_EN_CH1 : 1;         ///<BIT [9] Extended Chip Enable Control 1 for Channel 1.
        uint16_t EXT_CE_EN_CH2 : 1;         ///<BIT [10] Extended Chip Enable Control 1 for Channel 2.
        uint16_t EXT_CE_EN_CH3 : 1;         ///<BIT [11] Extended Chip Enable Control 1 for Channel 3.
        uint16_t EXT_CE_EN_CH4 : 1;         ///<BIT [12] Extended Chip Enable Control 1 for Channel 4.
        uint16_t EXT_CE_EN_CH5 : 1;         ///<BIT [13] Extended Chip Enable Control 1 for Channel 5.
        uint16_t EXT_CE_EN_CH6 : 1;         ///<BIT [14] Extended Chip Enable Control 1 for Channel 6.
        uint16_t EXT_CE_EN_CH7 : 1;         ///<BIT [15] Extended Chip Enable Control 1 for Channel 7.
    } b;    
} FlashSeqChanEngineClkCtrl1_t;

///  @brief 0xF200E084 : Flash Sequencer Status Byte Control 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t SBYTE1            : 8;         ///<BIT [7:0] Status Byte 1.
        uint16_t FAIL_BIT1         : 3;         ///<BIT [10:8] Fail Bit 1.
        uint16_t RF200E084_11_RSVD : 1;         ///<BIT [11] Reserved.
        uint16_t READY_BIT1        : 3;         ///<BIT [14:12] Ready Bit 1.
        uint16_t RF200E084_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} FlashSeqStatByteCtrl1_t;

///  @brief 0xF200E086 : Flash Sequencer Status Byte Control 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t SBYTE2 : 8;         ///<BIT [7:0] Status Byte 2.
        uint16_t SBYTE3 : 8;         ///<BIT [15:8] Status Byte 3.
    } b;    
} FlashSeqStatByteCtrl2_t;

///  @brief 0xF200E088 : Flash Sequencer Status Byte Control 3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t FAIL_BIT2         : 3;         ///<BIT [2:0] Fail Bit 2.
        uint16_t RF200E088_3_RSVD  : 1;         ///<BIT [3] Reserved.
        uint16_t READY_BIT2        : 3;         ///<BIT [6:4] Ready Bit 2.
        uint16_t RF200E088_7_RSVD  : 1;         ///<BIT [7] Reserved.
        uint16_t FAIL_BIT3         : 3;         ///<BIT [10:8] Fail Bit 3.
        uint16_t RF200E088_11_RSVD : 1;         ///<BIT [11] Reserved.
        uint16_t FAIL_BIT4         : 3;         ///<BIT [14:12] Fail Bit 4.
        uint16_t RF200E088_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} FlashSeqStatByteCtrl3_t;

///  @brief 0xF200E08A : Flash Sequencer Status Byte Control 4
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t FAIL_BIT5        : 3;         ///<BIT [2:0] Fail Bit 5.
        uint16_t RF200E08A_3_RSVD : 1;         ///<BIT [3] Reserved.
        uint16_t FAIL_BIT6        : 3;         ///<BIT [6:4] Fail Bit 6.
        uint16_t RF200E08A_7_RSVD : 1;         ///<BIT [7] Reserved.
        uint16_t SBYTE4           : 8;         ///<BIT [15:8] Status Byte 4.
    } b;    
} FlashSeqStatByteCtrl4_t;

///  @brief 0xF200E090 : Flash Sequencer Flash Sequencer Command Port 7
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RBYTE3 : 8;         ///<BIT [7:0] Read Byte 3.
        uint16_t RBYTE4 : 8;         ///<BIT [15:8] Read Byte 4.
    } b;    
} FlashSeqFlashSeqCmdPort7_t;

///  @brief 0xF200E092 : Flash Sequencer Command Port 8
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RBYTE5 : 8;         ///<BIT [7:0] Read Byte 5.
        uint16_t RBYTE6 : 8;         ///<BIT [15:8] Read Byte 6.
    } b;    
} FlashSeqCmdPort8_t;

///  @brief 0xF200E094 : Flash Sequencer Command Port 9
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RBYTE7 : 8;         ///<BIT [7:0] Read Byte 7.
        uint16_t RBYTE8 : 8;         ///<BIT [15:8] Read Byte 8.
    } b;    
} FlashSeqCmdPort9_t;

///  @brief 0xF200E096 : Flash Sequencer Command Port 10
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PBYTE2 : 8;         ///<BIT [7:0] Program Byte 2.
        uint16_t PBYTE3 : 8;         ///<BIT [15:8] Program Byte 3.
    } b;    
} FlashSeqCmdPort10_t;

///  @brief 0xF200E098 : Flash Sequencer Command Port 11
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PBYTE4 : 8;         ///<BIT [7:0] Program Byte 4.
        uint16_t EBYTE2 : 8;         ///<BIT [15:8] Erase Byte 2.
    } b;    
} FlashSeqCmdPort11_t;

///  @brief 0xF200E09A : Flash Sequencer Command Port 12
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PBYTE5 : 8;         ///<BIT [7:0] Program Byte 5.
        uint16_t RBYTE9 : 8;         ///<BIT [15:8] Read Byte 9.
    } b;    
} FlashSeqCmdPort12_t;

///  @brief 0xF200E0A0 : Flash Sequencer Channel 0 Block Boundary Information
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t BLK_BNDR_SEL_CH0  : 8;         ///<BIT [7:0] Channel 0 Block Boundary Select.
        uint16_t BLK_BNDR1_CH0     : 3;         ///<BIT [10:8] Channel 0 Block Boundary Information 1.
        uint16_t RF200E0A0_11_RSVD : 1;         ///<BIT [11] Reserved.
        uint16_t BLK_BNDR2_CH0     : 3;         ///<BIT [14:12] Channel 0 Block Boundary Information 2.
        uint16_t RF200E0A0_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} FlashSeqChan0BlkBoundaryInformat_t;

///  @brief 0xF200E0A2 : Flash Sequencer Channel 1 Block Boundary Information
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t BLK_BNDR_SEL_CH1  : 8;         ///<BIT [7:0] Channel 1 Block Boundary Select.
        uint16_t BLK_BNDR1_CH1     : 3;         ///<BIT [10:8] Channel 1 Block Boundary Information 1.
        uint16_t RF200E0A2_11_RSVD : 1;         ///<BIT [11] Reserved.
        uint16_t BLK_BNDR2_CH1     : 3;         ///<BIT [14:12] Channel 1 Block Boundary Information 2.
        uint16_t RF200E0A2_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} FlashSeqChan1BlkBoundaryInformat_t;

///  @brief 0xF200E0A4 : Flash Sequencer Channel 2 Block Boundary Information
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t BLK_BNDR_SEL_CH2  : 8;         ///<BIT [7:0] Channel 2 Block Boundary Select.
        uint16_t BLK_BNDR1_CH2     : 3;         ///<BIT [10:8] Channel 2 Block Boundary Information 1.
        uint16_t RF200E0A4_11_RSVD : 1;         ///<BIT [11] Reserved.
        uint16_t BLK_BNDR2_CH2     : 3;         ///<BIT [14:12] Channel 2 Block Boundary Information 2.
        uint16_t RF200E0A4_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} FlashSeqChan2BlkBoundaryInformat_t;

///  @brief 0xF200E0A6 : Flash Sequencer Channel 3 Block Boundary Information
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t BLK_BNDR_SEL_CH3  : 8;         ///<BIT [7:0] Channel 3 Block Boundary Select.
        uint16_t BLK_BNDR1_CH3     : 3;         ///<BIT [10:8] Channel 3 Block Boundary Information 1.
        uint16_t RF200E0A6_11_RSVD : 1;         ///<BIT [11] Reserved.
        uint16_t BLK_BNDR2_CH3     : 3;         ///<BIT [14:12] Channel 3 Block Boundary Information 2.
        uint16_t RF200E0A6_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} FlashSeqChan3BlkBoundaryInformat_t;

///  @brief 0xF200E0A8 : Flash Sequencer Channel 4 Block Boundary Information
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t BLK_BNDR_SEL_CH4  : 8;         ///<BIT [7:0] Channel 4 Block Boundary Select.
        uint16_t BLK_BNDR1_CH4     : 3;         ///<BIT [10:8] Channel 4 Block Boundary Information 1.
        uint16_t RF200E0A8_11_RSVD : 1;         ///<BIT [11] Reserved.
        uint16_t BLK_BNDR2_CH4     : 3;         ///<BIT [14:12] Channel 4 Block Boundary Information 2.
        uint16_t RF200E0A8_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} FlashSeqChan4BlkBoundaryInformat_t;

///  @brief 0xF200E0AA : Flash Sequencer Channel 5 Block Boundary Information
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t BLK_BNDR_SEL_CH5  : 8;         ///<BIT [7:0] Channel 5 Block Boundary Select.
        uint16_t BLK_BNDR1_CH5     : 3;         ///<BIT [10:8] Channel 5 Block Boundary Information 1.
        uint16_t RF200E0AA_11_RSVD : 1;         ///<BIT [11] Reserved.
        uint16_t BLK_BNDR2_CH5     : 3;         ///<BIT [14:12] Channel 5 Block Boundary Information 2.
        uint16_t RF200E0AA_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} FlashSeqChan5BlkBoundaryInformat_t;

///  @brief 0xF200E0AC : Flash Sequencer Channel 6 Block Boundary Information
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t BLK_BNDR_SEL_CH6  : 8;         ///<BIT [7:0] Channel 6 Block Boundary Select.
        uint16_t BLK_BNDR1_CH6     : 3;         ///<BIT [10:8] Channel 6 Block Boundary Information 1.
        uint16_t RF200E0AC_11_RSVD : 1;         ///<BIT [11] Reserved.
        uint16_t BLK_BNDR2_CH6     : 3;         ///<BIT [14:12] Channel 6 Block Boundary Information 2.
        uint16_t RF200E0AC_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} FlashSeqChan6BlkBoundaryInformat_t;

///  @brief 0xF200E0AE : Flash Sequencer Channel 7 Block Boundary Information
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t BLK_BNDR_SEL_CH7  : 8;         ///<BIT [7:0] Channel 7 Block Boundary Select.
        uint16_t BLK_BNDR1_CH7     : 3;         ///<BIT [10:8] Channel 7 Block Boundary Information 1.
        uint16_t RF200E0AE_11_RSVD : 1;         ///<BIT [11] Reserved.
        uint16_t BLK_BNDR2_CH7     : 3;         ///<BIT [14:12] Channel 7 Block Boundary Information 2.
        uint16_t RF200E0AE_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} FlashSeqChan7BlkBoundaryInformat_t;

///  @brief 0xF200E0C2 : Flash Sequencer Status Port 3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200E0C2_7_0_RSVD  : 8;         ///<BIT [7:0] Reserved.
        uint16_t RF200E0C2_13_8_RSVD : 6;         ///<BIT [13:8] Reserved.
        uint16_t RF200E0C2_14_RSVD   : 1;         ///<BIT [14] Reserved.
        uint16_t RF200E0C2_15_RSVD   : 1;         ///<BIT [15] Reserved.
    } b;    
} FlashSeqStatPort3_t;

///  @brief 0xF200E0D0 : EF FIFO Clock Control Port
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200E0D0_1_0_RSVD   : 2;         ///<BIT [1:0] Reserved.
        uint16_t COL_MASK             : 1;         ///<BIT [2] Column Address Mask Select.
#if (_SOC_REV_ >= 0x0210)
        uint16_t RF200E0D0_10_3_RSVD  : 8;         ///<BIT [10:3] Reserved.
        uint16_t NO_ABORT_DES_CHAIN   : 1;         ///<BIT [11] Descriptor Chain Abort.
        uint16_t RF200E0D0_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
#else
        uint16_t RF200E0D0_8_3_RSVD   : 6;         ///<BIT [8:3] Reserved.
        uint16_t RF200E0D0_9_RSVD     : 1;         ///<BIT [9] Reserved.
        uint16_t RF200E0D0_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
#endif
    } b;    
} EfFifoClkCtrlPort_t;

///  @brief 0xF200E100 : Flash Sequencer Channel 0 to Channel 3 LUN Information
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t LUN_CH0 : 4;         ///<BIT [3:0] LUN Channel 0.
        uint16_t LUN_CH1 : 4;         ///<BIT [7:4] LUN Channel 1.
        uint16_t LUN_CH2 : 4;         ///<BIT [11:8] LUN Channel 2.
        uint16_t LUN_CH3 : 4;         ///<BIT [15:12] LUN Channel 3.
    } b;    
} FlashSeqChan0ToChan3Lun_t;

///  @brief 0xF200E102 : Flash Sequencer Channel 4 to Channel 7 LUN Information
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t LUN_CH4 : 4;         ///<BIT [3:0] LUN Channel 4.
        uint16_t LUN_CH5 : 4;         ///<BIT [7:4] LUN Channel 5.
        uint16_t LUN_CH6 : 4;         ///<BIT [11:8] LUN Channel 6.
        uint16_t LUN_CH7 : 4;         ///<BIT [15:12] LUN Channel 7.
    } b;    
} FlashSeqChan4ToChan7Lun_t;

///  @brief 0xF200E108 : All CE Enable Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ALL_CE_EN_CH0       : 1;         ///<BIT [0] All CE Mode Enable for Channel 0.
        uint16_t ALL_CE_EN_CH1       : 1;         ///<BIT [1] All CE Mode Enable for Channel 1.
        uint16_t ALL_CE_EN_CH2       : 1;         ///<BIT [2] All CE Mode Enable for Channel 2.
        uint16_t ALL_CE_EN_CH3       : 1;         ///<BIT [3] All CE Mode Enable for Channel 3.
#if (_SOC_REV_ >= 0x0210)
        uint16_t ALL_CE_EN_CH4       : 1;         ///<BIT [4] All CE Mode Enable for Channel 4.
        uint16_t ALL_CE_EN_CH5       : 1;         ///<BIT [5] All CE Mode Enable for Channel 5.
        uint16_t ALL_CE_EN_CH6       : 1;         ///<BIT [6] All CE Mode Enable for Channel 6.
        uint16_t ALL_CE_EN_CH7       : 1;         ///<BIT [7] All CE Mode Enable for Channel 7.
        uint16_t RF200E108_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
#else
        uint16_t RF200E108_15_4_RSVD :12;         ///<BIT [15:4] Reserved.
#endif
    } b;    
} AllCeEnCtrl_t;

///  @brief 0xF200E200 : QS Gate Control 1 for Channel 0 and Channel 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t QS_GATE_DLY_CH0   : 7;         ///<BIT [6:0] QS_GATE Delay Control for Channel 0.
        uint16_t RF200E200_7_RSVD  : 1;         ///<BIT [7] Reserved.
        uint16_t QS_GATE_DLY_CH1   : 7;         ///<BIT [14:8] QS_GATE Delay Control for Channel 1.
        uint16_t RF200E200_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} QsGateCtrl1ForChan0And_t;

///  @brief 0xF200E202 : QS Gate Control 2 for Channel 2 and Channel 3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t QS_GATE_DLY_CH2   : 7;         ///<BIT [6:0] QS_GATE Delay Control for Channel 2.
        uint16_t RF200E202_7_RSVD  : 1;         ///<BIT [7] Reserved.
        uint16_t QS_GATE_DLY_CH3   : 7;         ///<BIT [14:8] QS_GATE Delay Control for Channel 3.
        uint16_t RF200E202_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} QsGateCtrl2ForChan2And_t;

///  @brief 0xF200E204 : QS Gate Control 3 for Channel 4 and Channel 5
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t QS_GATE_DLY_CH4   : 7;         ///<BIT [6:0] QS_GATE Delay Control for Channel 4.
        uint16_t RF200E204_7_RSVD  : 1;         ///<BIT [7] Reserved.
        uint16_t QS_GATE_DLY_CH5   : 7;         ///<BIT [14:8] QS_GATE Delay Control for Channel 5.
        uint16_t RF200E204_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} QsGateCtrl3ForChan4And_t;

///  @brief 0xF200E206 : QS Gate Control 4 for Channel 6 and Channel 7
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t QS_GATE_DLY_CH6   : 7;         ///<BIT [6:0] QS_GATE Delay Control for Channel 6.
        uint16_t RF200E206_7_RSVD  : 1;         ///<BIT [7] Reserved.
        uint16_t QS_GATE_DLY_CH7   : 7;         ///<BIT [14:8] QS_GATE Delay Control for Channel 7.
        uint16_t RF200E206_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} QsGateCtrl4ForChan6And_t;

///  @brief 0xF200E210 : Drive Select for Channel 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t D_DRV_SEL_CH0     : 3;         ///<BIT [2:0] Data Sub-PHY Drive Strength Select for Channel 0.
        uint16_t RF200E210_3_RSVD  : 1;         ///<BIT [3] Reserved.
        uint16_t C_DRV_SEL_CH0     : 3;         ///<BIT [6:4] Command Sub-PHY Drive Strength Select for Channel 0.
        uint16_t RF200E210_7_RSVD  : 1;         ///<BIT [7] Reserved.
        uint16_t RE_DRV_SEL_CH0    : 3;         ///<BIT [10:8] Read Enable Bar Sub-PHY Drive Strength Select for Channel 0.
        uint16_t RF200E210_11_RSVD : 1;         ///<BIT [11] Reserved.
        uint16_t WE_DRV_SEL_CH0    : 3;         ///<BIT [14:12] Write Enable Bar Sub-PHY Drive Strength Select for Channel 0.
        uint16_t RF200E210_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} DriveSelForChan0_t;

///  @brief 0xF200E212 : Drive Select for Channel 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t D_DRV_SEL_CH1     : 3;         ///<BIT [2:0] Data Sub-PHY Drive Strength Select for Channel 1.
        uint16_t RF200E212_3_RSVD  : 1;         ///<BIT [3] Reserved.
        uint16_t C_DRV_SEL_CH1     : 3;         ///<BIT [6:4] Command Sub-PHY Drive Strength Select for Channel 1.
        uint16_t RF200E212_7_RSVD  : 1;         ///<BIT [7] Reserved.
        uint16_t RE_DRV_SEL_CH1    : 3;         ///<BIT [10:8] Read Enable Bar Sub-PHY Drive Strength Select for Channel 1.
        uint16_t RF200E212_11_RSVD : 1;         ///<BIT [11] Reserved.
        uint16_t WE_DRV_SEL_CH1    : 3;         ///<BIT [14:12] Write Enable Bar Sub-PHY Drive Strength Select for Channel 1.
        uint16_t RF200E212_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} DriveSelForChan1_t;

///  @brief 0xF200E214 : Drive Select for Channel 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t D_DRV_SEL_CH2     : 3;         ///<BIT [2:0] Data Sub-PHY Drive Strength Select for Channel 2.
        uint16_t RF200E214_3_RSVD  : 1;         ///<BIT [3] Reserved.
        uint16_t C_DRV_SEL_CH2     : 3;         ///<BIT [6:4] Command Sub-PHY Drive Strength Select for Channel 2.
        uint16_t RF200E214_7_RSVD  : 1;         ///<BIT [7] Reserved.
        uint16_t RE_DRV_SEL_CH2    : 3;         ///<BIT [10:8] Read Enable Bar Sub-PHY Drive Strength Select for Channel 2.
        uint16_t RF200E214_11_RSVD : 1;         ///<BIT [11] Reserved.
        uint16_t WE_DRV_SEL_CH2    : 3;         ///<BIT [14:12] Write Enable Bar Sub-PHY Drive Strength Select for Channel 2.
        uint16_t RF200E214_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} DriveSelForChan2_t;

///  @brief 0xF200E216 : Drive Select for Channel 3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t D_DRV_SEL_CH3     : 3;         ///<BIT [2:0] Data Sub-PHY Drive Strength Select for Channel 3.
        uint16_t RF200E216_3_RSVD  : 1;         ///<BIT [3] Reserved.
        uint16_t C_DRV_SEL_CH3     : 3;         ///<BIT [6:4] Command Sub-PHY Drive Strength Select for Channel 3.
        uint16_t RF200E216_7_RSVD  : 1;         ///<BIT [7] Reserved.
        uint16_t RE_DRV_SEL_CH3    : 3;         ///<BIT [10:8] Read Enable Bar Sub-PHY Drive Strength Select for Channel 3.
        uint16_t RF200E216_11_RSVD : 1;         ///<BIT [11] Reserved.
        uint16_t WE_DRV_SEL_CH3    : 3;         ///<BIT [14:12] Write Enable Bar Sub-PHY Drive Strength Select for Channel 3.
        uint16_t RF200E216_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} DriveSelForChan3_t;

///  @brief 0xF200E218 : Drive Select for Channel 4
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t D_DRV_SEL_CH4     : 3;         ///<BIT [2:0] Data Sub-PHY Drive Strength Select for Channel 4.
        uint16_t RF200E218_3_RSVD  : 1;         ///<BIT [3] Reserved.
        uint16_t C_DRV_SEL_CH4     : 3;         ///<BIT [6:4] Command Sub-PHY Drive Strength Select for Channel 4.
        uint16_t RF200E218_7_RSVD  : 1;         ///<BIT [7] Reserved.
        uint16_t RE_DRV_SEL_CH4    : 3;         ///<BIT [10:8] Read Enable Bar Sub-PHY Drive Strength Select for Channel 4.
        uint16_t RF200E218_11_RSVD : 1;         ///<BIT [11] Reserved.
        uint16_t WE_DRV_SEL_CH4    : 3;         ///<BIT [14:12] Write Enable Bar Sub-PHY Drive Strength Select for Channel 4.
        uint16_t RF200E218_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} DriveSelForChan4_t;

///  @brief 0xF200E21A : Drive Select for Channel 5
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t D_DRV_SEL_CH5     : 3;         ///<BIT [2:0] Data Sub-PHY Drive Strength Select for Channel 5.
        uint16_t RF200E21A_3_RSVD  : 1;         ///<BIT [3] Reserved.
        uint16_t C_DRV_SEL_CH5     : 3;         ///<BIT [6:4] Command Sub-PHY Drive Strength Select for Channel 5.
        uint16_t RF200E21A_7_RSVD  : 1;         ///<BIT [7] Reserved.
        uint16_t RE_DRV_SEL_CH5    : 3;         ///<BIT [10:8] Read Enable Bar Sub-PHY Drive Strength Select for Channel 5.
        uint16_t RF200E21A_11_RSVD : 1;         ///<BIT [11] Reserved.
        uint16_t WE_DRV_SEL_CH5    : 3;         ///<BIT [14:12] Write Enable Bar Sub-PHY Drive Strength Select for Channel 5.
        uint16_t RF200E21A_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} DriveSelForChan5_t;

///  @brief 0xF200E21C : Drive Select for Channel 6
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t D_DRV_SEL_CH6     : 3;         ///<BIT [2:0] Data Sub-PHY Drive Strength Select for Channel 6.
        uint16_t RF200E21C_3_RSVD  : 1;         ///<BIT [3] Reserved.
        uint16_t C_DRV_SEL_CH6     : 3;         ///<BIT [6:4] Command Sub-PHY Drive Strength Select for Channel 6.
        uint16_t RF200E21C_7_RSVD  : 1;         ///<BIT [7] Reserved.
        uint16_t RE_DRV_SEL_CH6    : 3;         ///<BIT [10:8] Read Enable Bar Sub-PHY Drive Strength Select for Channel 6.
        uint16_t RF200E21C_11_RSVD : 1;         ///<BIT [11] Reserved.
        uint16_t WE_DRV_SEL_CH6    : 3;         ///<BIT [14:12] Write Enable Bar Sub-PHY Drive Strength Select for Channel 6.
        uint16_t RF200E21C_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} DriveSelForChan6_t;

///  @brief 0xF200E21E : Drive Select for Channel 7
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t D_DRV_SEL_CH7     : 3;         ///<BIT [2:0] Data Sub-PHY Drive Strength Select for Channel 7.
        uint16_t RF200E21E_3_RSVD  : 1;         ///<BIT [3] Reserved.
        uint16_t C_DRV_SEL_CH7     : 3;         ///<BIT [6:4] Command Sub-PHY Drive Strength Select for Channel 7.
        uint16_t RF200E21E_7_RSVD  : 1;         ///<BIT [7] Reserved.
        uint16_t RE_DRV_SEL_CH7    : 3;         ///<BIT [10:8] Read Enable Bar Sub-PHY Drive Strength Select for Channel 7.
        uint16_t RF200E21E_11_RSVD : 1;         ///<BIT [11] Reserved.
        uint16_t WE_DRV_SEL_CH7    : 3;         ///<BIT [14:12] Write Enable Bar Sub-PHY Drive Strength Select for Channel 7.
        uint16_t RF200E21E_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} DriveSelForChan7_t;

#if (_SOC_REV_ < 0x0210)
///  @brief 0xF200E220 : Drive Select for Channel 8
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t D_DRV_SEL_CH8     : 3;         ///<BIT [2:0] Data Sub-PHY Drive Strength Select for Channel 8.
        uint16_t RF200E220_3_RSVD  : 1;         ///<BIT [3] Reserved.
        uint16_t C_DRV_SEL_CH8     : 3;         ///<BIT [6:4] Command Sub-PHY Drive Strength Select for Channel 8.
        uint16_t RF200E220_7_RSVD  : 1;         ///<BIT [7] Reserved.
        uint16_t RE_DRV_SEL_CH8    : 3;         ///<BIT [10:8] Read Enable Bar Sub-PHY Drive Strength Select for Channel 8.
        uint16_t RF200E220_11_RSVD : 1;         ///<BIT [11] Reserved.
        uint16_t WE_DRV_SEL_CH8    : 3;         ///<BIT [14:12] Write Enable Bar Sub-PHY Drive Strength Select for Channel 8.
        uint16_t RF200E220_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} DriveSelForChan8_t;

///  @brief 0xF200E222 : Drive Select for Channel 9
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t D_DRV_SEL_CH9     : 3;         ///<BIT [2:0] Data Sub-PHY Drive Strength Select for Channel 9.
        uint16_t RF200E222_3_RSVD  : 1;         ///<BIT [3] Reserved.
        uint16_t C_DRV_SEL_CH9     : 3;         ///<BIT [6:4] Command Sub-PHY Drive Strength Select for Channel 9.
        uint16_t RF200E222_7_RSVD  : 1;         ///<BIT [7] Reserved.
        uint16_t RE_DRV_SEL_CH9    : 3;         ///<BIT [10:8] Read Enable Bar Sub-PHY Drive Strength Select for Channel 9.
        uint16_t RF200E222_11_RSVD : 1;         ///<BIT [11] Reserved.
        uint16_t WE_DRV_SEL_CH9    : 3;         ///<BIT [14:12] Write Enable Bar Sub-PHY Drive Strength Select for Channel 9.
        uint16_t RF200E222_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} DriveSelForChan9_t;

///  @brief 0xF200E224 : Drive Select for Channel 10
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t D_DRV_SEL_CH10    : 3;         ///<BIT [2:0] Data Sub-PHY Drive Strength Select for Channel 10.
        uint16_t RF200E224_3_RSVD  : 1;         ///<BIT [3] Reserved.
        uint16_t C_DRV_SEL_CH10    : 3;         ///<BIT [6:4] Command Sub-PHY Drive Strength Select for Channel 10.
        uint16_t RF200E224_7_RSVD  : 1;         ///<BIT [7] Reserved.
        uint16_t RE_DRV_SEL_CH10   : 3;         ///<BIT [10:8] Read Enable Bar Sub-PHY Drive Strength Select for Channel 10.
        uint16_t RF200E224_11_RSVD : 1;         ///<BIT [11] Reserved.
        uint16_t WE_DRV_SEL_CH10   : 3;         ///<BIT [14:12] Write Enable Bar Sub-PHY Drive Strength Select for Channel 10.
        uint16_t RF200E224_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} DriveSelForChan10_t;

///  @brief 0xF200E226 : Drive Select for Channel 11
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t D_DRV_SEL_CH11    : 3;         ///<BIT [2:0] Data Sub-PHY Drive Strength Select for Channel 11.
        uint16_t RF200E226_3_RSVD  : 1;         ///<BIT [3] Reserved.
        uint16_t C_DRV_SEL_CH11    : 3;         ///<BIT [6:4] Command Sub-PHY Drive Strength Select for Channel 11.
        uint16_t RF200E226_7_RSVD  : 1;         ///<BIT [7] Reserved.
        uint16_t RE_DRV_SEL_CH11   : 3;         ///<BIT [10:8] Read Enable Bar Sub-PHY Drive Strength Select for Channel 11.
        uint16_t RF200E226_11_RSVD : 1;         ///<BIT [11] Reserved.
        uint16_t WE_DRV_SEL_CH11   : 3;         ///<BIT [14:12] Write Enable Bar Sub-PHY Drive Strength Select for Channel 11.
        uint16_t RF200E226_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} DriveSelForChan11_t;

///  @brief 0xF200E228 : Drive Select for Channel 12
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t D_DRV_SEL_CH12    : 3;         ///<BIT [2:0] Data Sub-PHY Drive Strength Select for Channel 12.
        uint16_t RF200E228_3_RSVD  : 1;         ///<BIT [3] Reserved.
        uint16_t C_DRV_SEL_CH12    : 3;         ///<BIT [6:4] Command Sub-PHY Drive Strength Select for Channel 12.
        uint16_t RF200E228_7_RSVD  : 1;         ///<BIT [7] Reserved.
        uint16_t RE_DRV_SEL_CH12   : 3;         ///<BIT [10:8] Read Enable Bar Sub-PHY Drive Strength Select for Channel 12.
        uint16_t RF200E228_11_RSVD : 1;         ///<BIT [11] Reserved.
        uint16_t WE_DRV_SEL_CH12   : 3;         ///<BIT [14:12] Write Enable Bar Sub-PHY Drive Strength Select for Channel 12.
        uint16_t RF200E228_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} DriveSelForChan12_t;

///  @brief 0xF200E22A : Drive Select for Channel 13
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t D_DRV_SEL_CH13    : 3;         ///<BIT [2:0] Data Sub-PHY Drive Strength Select for Channel 13.
        uint16_t RF200E22A_3_RSVD  : 1;         ///<BIT [3] Reserved.
        uint16_t C_DRV_SEL_CH13    : 3;         ///<BIT [6:4] Command Sub-PHY Drive Strength Select for Channel 13.
        uint16_t RF200E22A_7_RSVD  : 1;         ///<BIT [7] Reserved.
        uint16_t RE_DRV_SEL_CH13   : 3;         ///<BIT [10:8] Read Enable Bar Sub-PHY Drive Strength Select for Channel 13.
        uint16_t RF200E22A_11_RSVD : 1;         ///<BIT [11] Reserved.
        uint16_t WE_DRV_SEL_CH13   : 3;         ///<BIT [14:12] Write Enable Bar Sub-PHY Drive Strength Select for Channel 13.
        uint16_t RF200E22A_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} DriveSelForChan13_t;

///  @brief 0xF200E22C : Drive Select for Channel 14
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t D_DRV_SEL_CH14    : 3;         ///<BIT [2:0] Data Sub-PHY Drive Strength Select for Channel 14.
        uint16_t RF200E22C_3_RSVD  : 1;         ///<BIT [3] Reserved.
        uint16_t C_DRV_SEL_CH14    : 3;         ///<BIT [6:4] Command Sub-PHY Drive Strength Select for Channel 14.
        uint16_t RF200E22C_7_RSVD  : 1;         ///<BIT [7] Reserved.
        uint16_t RE_DRV_SEL_CH14   : 3;         ///<BIT [10:8] Read Enable Bar Sub-PHY Drive Strength Select for Channel 14.
        uint16_t RF200E22C_11_RSVD : 1;         ///<BIT [11] Reserved.
        uint16_t WE_DRV_SEL_CH14   : 3;         ///<BIT [14:12] Write Enable Bar Sub-PHY Drive Strength Select for Channel 14.
        uint16_t RF200E22C_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} DriveSelForChan14_t;

///  @brief 0xF200E22E : Drive Select for Channel 15
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t D_DRV_SEL_CH15    : 3;         ///<BIT [2:0] Data Sub-PHY Drive Strength Select for Channel 15.
        uint16_t RF200E22E_3_RSVD  : 1;         ///<BIT [3] Reserved.
        uint16_t C_DRV_SEL_CH15    : 3;         ///<BIT [6:4] Command Sub-PHY Drive Strength Select for Channel 15.
        uint16_t RF200E22E_7_RSVD  : 1;         ///<BIT [7] Reserved.
        uint16_t RE_DRV_SEL_CH15   : 3;         ///<BIT [10:8] Read Enable Bar Sub-PHY Drive Strength Select for Channel 15.
        uint16_t RF200E22E_11_RSVD : 1;         ///<BIT [11] Reserved.
        uint16_t WE_DRV_SEL_CH15   : 3;         ///<BIT [14:12] Write Enable Bar Sub-PHY Drive Strength Select for Channel 15.
        uint16_t RF200E22E_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} DriveSelForChan15_t;
#endif    // #if (_SOC_REV_ < 0x0210)

///  @brief 0xF200E230 : ZP ZN Control 1 for Channel 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DATA_ZPR_CH0 : 4;         ///<BIT [3:0] Data Sub-PHY ZPR Slew Rate Control Select for Channel 0.
        uint16_t DATA_ZNR_CH0 : 4;         ///<BIT [7:4] Data Sub-PHY ZNR Slew Rate Control Select for Channel 0.
        uint16_t CMD_ZPR_CH0  : 4;         ///<BIT [11:8] Command Sub-PHY ZPR Slew Rate Control Select for Channel 0.
        uint16_t CMD_ZNR_CH0  : 4;         ///<BIT [15:12] Command Sub-PHY ZNR Slew Rate Control Select for Channel 0.
    } b;    
} ZpZnCtrl1ForChan0_t;

///  @brief 0xF200E232 : ZP ZN Control 2 for Channel 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t REB_ZPR_CH0 : 4;         ///<BIT [3:0] Read Enable Bar Differential Sub-PHY ZPR Slew Rate Control Sele.
        uint16_t REB_ZNR_CH0 : 4;         ///<BIT [7:4] Read Enable Bar Differential Sub-PHY ZNR Slew Rate Control Sele.
        uint16_t WEB_ZPR_CH0 : 4;         ///<BIT [11:8] Write Enable Bar Differential Sub-PHY ZPR Slew Rate Control Sel.
        uint16_t WEB_ZNR_CH0 : 4;         ///<BIT [15:12] Write Enable Bar Differential Sub-PHY ZNR Slew Rate Control Sel.
    } b;    
} ZpZnCtrl2ForChan0_t;

///  @brief 0xF200E234 : ZP ZN Control 1 for Channel 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DATA_ZPR_CH1 : 4;         ///<BIT [3:0] Data Sub-PHY ZPR Slew Rate Control Select for Channel 1.
        uint16_t DATA_ZNR_CH1 : 4;         ///<BIT [7:4] Data Sub-PHY ZNR Slew Rate Control Select for Channel 1.
        uint16_t CMD_ZPR_CH1  : 4;         ///<BIT [11:8] Command Sub-PHY ZPR Slew Rate Control Select for Channel 1.
        uint16_t CMD_ZNR_CH1  : 4;         ///<BIT [15:12] Command Sub-PHY ZNR Slew Rate Control Select for Channel 1.
    } b;    
} ZpZnCtrl1ForChan1_t;

///  @brief 0xF200E236 : ZP ZN Control 2 for Channel 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t REB_ZPR_CH1 : 4;         ///<BIT [3:0] Read Enable Bar Differential Sub-PHY ZPR Slew Rate Control Sele.
        uint16_t REB_ZNR_CH1 : 4;         ///<BIT [7:4] Read Enable Bar Differential Sub-PHY ZNR Slew Rate Control Sele.
        uint16_t WEB_ZPR_CH1 : 4;         ///<BIT [11:8] Write Enable Bar Differential Sub-PHY ZPR Slew Rate Control Sel.
        uint16_t WEB_ZNR_CH1 : 4;         ///<BIT [15:12] Write Enable Bar Differential Sub-PHY ZNR Slew Rate Control Sel.
    } b;    
} ZpZnCtrl2ForChan1_t;

///  @brief 0xF200E238 : ZP ZN Control 1 for Channel 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DATA_ZPR_CH2 : 4;         ///<BIT [3:0] Data Sub-PHY ZPR Slew Rate Control Select for Channel 2.
        uint16_t DATA_ZNR_CH2 : 4;         ///<BIT [7:4] Data Sub-PHY ZNR Slew Rate Control Select for Channel 2.
        uint16_t CMD_ZPR_CH2  : 4;         ///<BIT [11:8] Command Sub-PHY ZPR Slew Rate Control Select for Channel 2.
        uint16_t CMD_ZNR_CH2  : 4;         ///<BIT [15:12] Command Sub-PHY ZNR Slew Rate Control Select for Channel 2.
    } b;    
} ZpZnCtrl1ForChan2_t;

///  @brief 0xF200E23A : ZP ZN Control 2 for Channel 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t REB_ZPR_CH2 : 4;         ///<BIT [3:0] Read Enable Bar Differential Sub-PHY ZPR Slew Rate Control Sele.
        uint16_t REB_ZNR_CH2 : 4;         ///<BIT [7:4] Read Enable Bar Differential Sub-PHY ZNR Slew Rate Control Sele.
        uint16_t WEB_ZPR_CH2 : 4;         ///<BIT [11:8] Write Enable Bar Differential Sub-PHY ZPR Slew Rate Control Sel.
        uint16_t WEB_ZNR_CH2 : 4;         ///<BIT [15:12] Write Enable Bar Differential Sub-PHY ZNR Slew Rate Control Sel.
    } b;    
} ZpZnCtrl2ForChan2_t;

///  @brief 0xF200E23C : ZP ZN Control 1 for Channel 3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DATA_ZPR_CH3 : 4;         ///<BIT [3:0] Data Sub-PHY ZPR Slew Rate Control Select for Channel 3.
        uint16_t DATA_ZNR_CH3 : 4;         ///<BIT [7:4] Data Sub-PHY ZNR Slew Rate Control Select for Channel 3.
        uint16_t CMD_ZPR_CH3  : 4;         ///<BIT [11:8] Command Sub-PHY ZPR Slew Rate Control Select for Channel 3.
        uint16_t CMD_ZNR_CH3  : 4;         ///<BIT [15:12] Command Sub-PHY ZNR Slew Rate Control Select for Channel 3.
    } b;    
} ZpZnCtrl1ForChan3_t;

///  @brief 0xF200E23E : ZP ZN Control 2 for Channel 3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t REB_ZPR_CH3 : 4;         ///<BIT [3:0] Read Enable Bar Differential Sub-PHY ZPR Slew Rate Control Sele.
        uint16_t REB_ZNR_CH3 : 4;         ///<BIT [7:4] Read Enable Bar Differential Sub-PHY ZNR Slew Rate Control Sele.
        uint16_t WEB_ZPR_CH3 : 4;         ///<BIT [11:8] Write Enable Bar Differential Sub-PHY ZPR Slew Rate Control Sel.
        uint16_t WEB_ZNR_CH3 : 4;         ///<BIT [15:12] Write Enable Bar Differential Sub-PHY ZNR Slew Rate Control Sel.
    } b;    
} ZpZnCtrl2ForChan3_t;

///  @brief 0xF200E240 : ZP ZN Control 1 for Channel 4
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DATA_ZPR_CH4 : 4;         ///<BIT [3:0] Data Sub-PHY ZPR Slew Rate Control Select for Channel 4.
        uint16_t DATA_ZNR_CH4 : 4;         ///<BIT [7:4] Data Sub-PHY ZNR Slew Rate Control Select for Channel 4.
        uint16_t CMD_ZPR_CH4  : 4;         ///<BIT [11:8] Command Sub-PHY ZPR Slew Rate Control Select for Channel 4.
        uint16_t CMD_ZNR_CH4  : 4;         ///<BIT [15:12] Command Sub-PHY ZNR Slew Rate Control Select for Channel 4.
    } b;    
} ZpZnCtrl1ForChan4_t;

///  @brief 0xF200E242 : ZP ZN Control 2 for Channel 4
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t REB_ZPR_CH4 : 4;         ///<BIT [3:0] Read Enable Bar Differential Sub-PHY ZPR Slew Rate Control Sele.
        uint16_t REB_ZNR_CH4 : 4;         ///<BIT [7:4] Read Enable Bar Differential Sub-PHY ZNR Slew Rate Control Sele.
        uint16_t WEB_ZPR_CH4 : 4;         ///<BIT [11:8] Write Enable Bar Differential Sub-PHY ZPR Slew Rate Control Sel.
        uint16_t WEB_ZNR_CH4 : 4;         ///<BIT [15:12] Write Enable Bar Differential Sub-PHY ZNR Slew Rate Control Sel.
    } b;    
} ZpZnCtrl2ForChan4_t;

///  @brief 0xF200E244 : ZP ZN Control 1 for Channel 5
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DATA_ZPR_CH5 : 4;         ///<BIT [3:0] Data Sub-PHY ZPR Slew Rate Control Select for Channel 5.
        uint16_t DATA_ZNR_CH5 : 4;         ///<BIT [7:4] Data Sub-PHY ZNR Slew Rate Control Select for Channel 5.
        uint16_t CMD_ZPR_CH5  : 4;         ///<BIT [11:8] Command Sub-PHY ZPR Slew Rate Control Select for Channel 5.
        uint16_t CMD_ZNR_CH5  : 4;         ///<BIT [15:12] Command Sub-PHY ZNR Slew Rate Control Select for Channel 5.
    } b;    
} ZpZnCtrl1ForChan5_t;

///  @brief 0xF200E246 : ZP ZN Control 2 for Channel 5
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t REB_ZPR_CH5 : 4;         ///<BIT [3:0] Read Enable Bar Differential Sub-PHY ZPR Slew Rate Control Sele.
        uint16_t REB_ZNR_CH5 : 4;         ///<BIT [7:4] Read Enable Bar Differential Sub-PHY ZNR Slew Rate Control Sele.
        uint16_t WEB_ZPR_CH5 : 4;         ///<BIT [11:8] Write Enable Bar Differential Sub-PHY ZPR Slew Rate Control Sel.
        uint16_t WEB_ZNR_CH5 : 4;         ///<BIT [15:12] Write Enable Bar Differential Sub-PHY ZNR Slew Rate Control Sel.
    } b;    
} ZpZnCtrl2ForChan5_t;

///  @brief 0xF200E248 : ZP ZN Control 1 for Channel 6
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DATA_ZPR_CH6 : 4;         ///<BIT [3:0] Data Sub-PHY ZPR Slew Rate Control Select for Channel 6.
        uint16_t DATA_ZNR_CH6 : 4;         ///<BIT [7:4] Data Sub-PHY ZNR Slew Rate Control Select for Channel 6.
        uint16_t CMD_ZPR_CH6  : 4;         ///<BIT [11:8] Command Sub-PHY ZPR Slew Rate Control Select for Channel 6.
        uint16_t CMD_ZNR_CH6  : 4;         ///<BIT [15:12] Command Sub-PHY ZNR Slew Rate Control Select for Channel 6.
    } b;    
} ZpZnCtrl1ForChan6_t;

///  @brief 0xF200E24A : ZP ZN Control 2 for Channel 6
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t REB_ZPR_CH6 : 4;         ///<BIT [3:0] Read Enable Bar Differential Sub-PHY ZPR Slew Rate Control Sele.
        uint16_t REB_ZNR_CH6 : 4;         ///<BIT [7:4] Read Enable Bar Differential Sub-PHY ZNR Slew Rate Control Sele.
        uint16_t WEB_ZPR_CH6 : 4;         ///<BIT [11:8] Write Enable Bar Differential Sub-PHY ZPR Slew Rate Control Sel.
        uint16_t WEB_ZNR_CH6 : 4;         ///<BIT [15:12] Write Enable Bar Differential Sub-PHY ZNR Slew Rate Control Sel.
    } b;    
} ZpZnCtrl2ForChan6_t;

///  @brief 0xF200E24C : ZP ZN Control 1 for Channel 7
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DATA_ZPR_CH7 : 4;         ///<BIT [3:0] Data Sub-PHY ZPR Slew Rate Control Select for Channel 7.
        uint16_t DATA_ZNR_CH7 : 4;         ///<BIT [7:4] Data Sub-PHY ZNR Slew Rate Control Select for Channel 7.
        uint16_t CMD_ZPR_CH7  : 4;         ///<BIT [11:8] Command Sub-PHY ZPR Slew Rate Control Select for Channel 7.
        uint16_t CMD_ZNR_CH7  : 4;         ///<BIT [15:12] Command Sub-PHY ZNR Slew Rate Control Select for Channel 7.
    } b;    
} ZpZnCtrl1ForChan7_t;

///  @brief 0xF200E24E : ZP ZN Control 2 for Channel 7
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t REB_ZPR_CH7 : 4;         ///<BIT [3:0] Read Enable Bar Differential Sub-PHY ZPR Slew Rate Control Sele.
        uint16_t REB_ZNR_CH7 : 4;         ///<BIT [7:4] Read Enable Bar Differential Sub-PHY ZNR Slew Rate Control Sele.
        uint16_t WEB_ZPR_CH7 : 4;         ///<BIT [11:8] Write Enable Bar Differential Sub-PHY ZPR Slew Rate Control Sel.
        uint16_t WEB_ZNR_CH7 : 4;         ///<BIT [15:12] Write Enable Bar Differential Sub-PHY ZNR Slew Rate Control Sel.
    } b;    
} ZpZnCtrl2ForChan7_t;

///  @brief 0xF200E270 : Data Sub PHY ZD Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DATA_ZD_CH0         : 1;         ///<BIT [0] Data Sub-PHY ZD Driver Control Select for Channel 0.
        uint16_t DATA_ZD_CH1         : 1;         ///<BIT [1] Data Sub-PHY ZD Driver Control Select for Channel 1.
        uint16_t DATA_ZD_CH2         : 1;         ///<BIT [2] Data Sub-PHY ZD Driver Control Select for Channel 2.
        uint16_t DATA_ZD_CH3         : 1;         ///<BIT [3] Data Sub-PHY ZD Driver Control Select for Channel 3.
#if (_SOC_REV_ >= 0x0210)
        uint16_t DATA_ZD_CH4         : 1;         ///<BIT [4] Data Sub-PHY ZD Driver Control Select for Channel 4.
        uint16_t DATA_ZD_CH5         : 1;         ///<BIT [5] Data Sub-PHY ZD Driver Control Select for Channel 5.
        uint16_t DATA_ZD_CH6         : 1;         ///<BIT [6] Data Sub-PHY ZD Driver Control Select for Channel 6.
        uint16_t DATA_ZD_CH7         : 1;         ///<BIT [7] Data Sub-PHY ZD Driver Control Select for Channel 7.
        uint16_t RF200E270_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
#else
        uint16_t RF200E270_15_4_RSVD :12;         ///<BIT [15:4] Reserved.
#endif
    } b;    
} DataSubPhyZdCtrl_t;

///  @brief 0xF200E272 : Command Sub PHY ZD Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t CMD_ZD_CH0          : 1;         ///<BIT [0] Command Sub-PHY ZD Driver Control Select for Channel 0.
        uint16_t CMD_ZD_CH1          : 1;         ///<BIT [1] Command Sub-PHY ZD Driver Control Select for Channel 1.
        uint16_t CMD_ZD_CH2          : 1;         ///<BIT [2] Command Sub-PHY ZD Driver Control Select for Channel 2.
        uint16_t CMD_ZD_CH3          : 1;         ///<BIT [3] Command Sub-PHY ZD Driver Control Select for Channel 3.
#if (_SOC_REV_ >= 0x0210)
        uint16_t CMD_ZD_CH4          : 1;         ///<BIT [4] Command Sub-PHY ZD Driver Control Select for Channel 4.
        uint16_t CMD_ZD_CH5          : 1;         ///<BIT [5] Command Sub-PHY ZD Driver Control Select for Channel 5.
        uint16_t CMD_ZD_CH6          : 1;         ///<BIT [6] Command Sub-PHY ZD Driver Control Select for Channel 6.
        uint16_t CMD_ZD_CH7          : 1;         ///<BIT [7] Command Sub-PHY ZD Driver Control Select for Channel 7.
        uint16_t RF200E272_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
#else
        uint16_t RF200E272_15_4_RSVD :12;         ///<BIT [15:4] Reserved.
#endif
    } b;    
} CmdSubPhyZdCtrl_t;

///  @brief 0xF200E274 : REB Sub PHY ZD Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t REB_ZD_CH0          : 1;         ///<BIT [0] REB Sub-PHY ZD Driver Control Select for Channel 0.
        uint16_t REB_ZD_CH1          : 1;         ///<BIT [1] REB Sub-PHY ZD Driver Control Select for Channel 1.
        uint16_t REB_ZD_CH2          : 1;         ///<BIT [2] REB Sub-PHY ZD Driver Control Select for Channel 2.
        uint16_t REB_ZD_CH3          : 1;         ///<BIT [3] REB Sub-PHY ZD Driver Control Select for Channel 3.
#if (_SOC_REV_ >= 0x0210)
        uint16_t REB_ZD_CH4          : 1;         ///<BIT [4] REB Sub-PHY ZD Driver Control Select for Channel 4.
        uint16_t REB_ZD_CH5          : 1;         ///<BIT [5] REB Sub-PHY ZD Driver Control Select for Channel 5.
        uint16_t REB_ZD_CH6          : 1;         ///<BIT [6] REB Sub-PHY ZD Driver Control Select for Channel 6.
        uint16_t REB_ZD_CH7          : 1;         ///<BIT [7] REB Sub-PHY ZD Driver Control Select for Channel 7.
        uint16_t RF200E274_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
#else
        uint16_t RF200E274_15_4_RSVD :12;         ///<BIT [15:4] Reserved.
#endif
    } b;    
} RebSubPhyZdCtrl_t;

///  @brief 0xF200E276 : WEB Sub PHY ZD Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t WEB_ZD_CH0          : 1;         ///<BIT [0] WEB Sub-PHY ZD Driver Control Select for Channel 0.
        uint16_t WEB_ZD_CH1          : 1;         ///<BIT [1] WEB Sub-PHY ZD Driver Control Select for Channel 1.
        uint16_t WEB_ZD_CH2          : 1;         ///<BIT [2] WEB Sub-PHY ZD Driver Control Select for Channel 2.
        uint16_t WEB_ZD_CH3          : 1;         ///<BIT [3] WEB Sub-PHY ZD Driver Control Select for Channel 3.
#if (_SOC_REV_ >= 0x0210)
        uint16_t WEB_ZD_CH4          : 1;         ///<BIT [4] WEB Sub-PHY ZD Driver Control Select for Channel 4.
        uint16_t WEB_ZD_CH5          : 1;         ///<BIT [5] WEB Sub-PHY ZD Driver Control Select for Channel 5.
        uint16_t WEB_ZD_CH6          : 1;         ///<BIT [6] WEB Sub-PHY ZD Driver Control Select for Channel 6.
        uint16_t WEB_ZD_CH7          : 1;         ///<BIT [7] WEB Sub-PHY ZD Driver Control Select for Channel 7.
        uint16_t RF200E276_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
#else
        uint16_t RF200E276_15_4_RSVD :12;         ///<BIT [15:4] Reserved.
#endif
    } b;    
} WebSubPhyZdCtrl_t;

///  @brief 0xF200E27C : Calibration Pad Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t ZP                   : 4;         ///<BIT [3:0] ZP Calibration.
        uint16_t ZN                   : 4;         ///<BIT [7:4] ZN Calibration.
        uint16_t CAL_EN               : 1;         ///<BIT [8] Enable Calibration Measurements.
        uint16_t CAL_N_PB             : 1;         ///<BIT [9] Enable Calibration Measurements for the N or P Driver.
        uint16_t RF200E27C_14_10_RSVD : 5;         ///<BIT [14:10] Reserved.
        uint16_t INC                  : 1;         ///<BIT [15] Enable Driver Strength Increase.
    } b;    
} CalPadCtrl_t;

///  @brief 0xF200E280 : DLL Control 1 for Channel 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DELAY_TEST_CH0     : 9;         ///<BIT [8:0] Delay Test Control for Channel 0.
        uint16_t AUTO_UPDATE_EN_CH0 : 1;         ///<BIT [9] Automatic Update Enable for Channel 0.
        uint16_t BYPASS_EN_CH0      : 1;         ///<BIT [10] DLL Delay Bypass for Channel 0.
        uint16_t GAIN2X_CH0         : 1;         ///<BIT [11] 2X Gain Enable for Channel 0.
        uint16_t RST_CH0            : 1;         ///<BIT [12] DLL Reset Enable for Channel 0.
        uint16_t TEST_EN_CH0        : 1;         ///<BIT [13] DLL Test Mode Enable for Channel 0.
        uint16_t UPDATE_EN_CH0      : 1;         ///<BIT [14] DLL Manual Update Enable for Channel 0.
        uint16_t RF200E280_15_RSVD  : 1;         ///<BIT [15] Reserved.
    } b;    
} DllCtrl1ForChan0_t;

///  @brief 0xF200E282 : DLL Control 2 for Channel 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PHSEL0_CH0           : 6;         ///<BIT [5:0] DLL Phase Select 0 for Channel 0.
        uint16_t RF200E282_7_6_RSVD   : 2;         ///<BIT [7:6] Reserved.
        uint16_t PHSEL1_CH0           : 6;         ///<BIT [13:8] DLL Phase Select 1 for Channel 0.
        uint16_t RF200E282_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
    } b;    
} DllCtrl2ForChan0_t;

///  @brief 0xF200E284 : DLL Control 1 for Channel 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DELAY_TEST_CH1     : 9;         ///<BIT [8:0] Delay Test Control for Channel 1.
        uint16_t AUTO_UPDATE_EN_CH1 : 1;         ///<BIT [9] Automatic Update Enable for Channel 1.
        uint16_t BYPASS_EN_CH1      : 1;         ///<BIT [10] DLL Delay Bypass for Channel 1.
        uint16_t GAIN2X_CH1         : 1;         ///<BIT [11] 2X Gain Enable for Channel 1.
        uint16_t RST_CH1            : 1;         ///<BIT [12] DLL Reset Enable for Channel 1.
        uint16_t TEST_EN_CH1        : 1;         ///<BIT [13] DLL Test Mode Enable for Channel 1.
        uint16_t UPDATE_EN_CH1      : 1;         ///<BIT [14] DLL Manual Update Enable for Channel 1.
        uint16_t RF200E284_15_RSVD  : 1;         ///<BIT [15] Reserved.
    } b;    
} DllCtrl1ForChan1_t;

///  @brief 0xF200E286 : DLL Control 2 for Channel 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PHSEL0_CH1           : 6;         ///<BIT [5:0] DLL Phase Select 0 for Channel 1.
        uint16_t RF200E286_7_6_RSVD   : 2;         ///<BIT [7:6] Reserved.
        uint16_t PHSEL1_CH1           : 6;         ///<BIT [13:8] DLL Phase Select 1 for Channel 1.
        uint16_t RF200E286_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
    } b;    
} DllCtrl2ForChan1_t;

///  @brief 0xF200E288 : DLL Control 1 for Channel 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DELAY_TEST_CH2     : 9;         ///<BIT [8:0] Delay Test Control for Channel 2.
        uint16_t AUTO_UPDATE_EN_CH2 : 1;         ///<BIT [9] Automatic Update Enable for Channel 2.
        uint16_t BYPASS_EN_CH2      : 1;         ///<BIT [10] DLL Delay Bypass for Channel 2.
        uint16_t GAIN2X_CH2         : 1;         ///<BIT [11] 2X Gain Enable for Channel 2.
        uint16_t RST_CH2            : 1;         ///<BIT [12] DLL Reset Enable for Channel 2.
        uint16_t TEST_EN_CH2        : 1;         ///<BIT [13] DLL Test Mode Enable for Channel 2.
        uint16_t UPDATE_EN_CH2      : 1;         ///<BIT [14] DLL Manual Update Enable for Channel 2.
        uint16_t RF200E288_15_RSVD  : 1;         ///<BIT [15] Reserved.
    } b;    
} DllCtrl1ForChan2_t;

///  @brief 0xF200E28A : DLL Control 2 for Channel 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PHSEL0_CH2           : 6;         ///<BIT [5:0] DLL Phase Select 0 for Channel 2.
        uint16_t RF200E28A_7_6_RSVD   : 2;         ///<BIT [7:6] Reserved.
        uint16_t PHSEL1_CH2           : 6;         ///<BIT [13:8] DLL Phase Select 1 for Channel 2.
        uint16_t RF200E28A_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
    } b;    
} DllCtrl2ForChan2_t;

///  @brief 0xF200E28C : DLL Control 1 for Channel 3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DELAY_TEST_CH3     : 9;         ///<BIT [8:0] Delay Test Control for Channel 3.
        uint16_t AUTO_UPDATE_EN_CH3 : 1;         ///<BIT [9] Automatic Update Enable for Channel 3.
        uint16_t BYPASS_EN_CH3      : 1;         ///<BIT [10] DLL Delay Bypass for Channel 3.
        uint16_t GAIN2X_CH3         : 1;         ///<BIT [11] 2X Gain Enable for Channel 3.
        uint16_t RST_CH3            : 1;         ///<BIT [12] DLL Reset Enable for Channel 3.
        uint16_t TEST_EN_CH3        : 1;         ///<BIT [13] DLL Test Mode Enable for Channel 3.
        uint16_t UPDATE_EN_CH3      : 1;         ///<BIT [14] DLL Manual Update Enable for Channel 3.
        uint16_t RF200E28C_15_RSVD  : 1;         ///<BIT [15] Reserved.
    } b;    
} DllCtrl1ForChan3_t;

///  @brief 0xF200E28E : DLL Control 2 for Channel 3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PHSEL0_CH3           : 6;         ///<BIT [5:0] DLL Phase Select 0 for Channel 3.
        uint16_t RF200E28E_7_6_RSVD   : 2;         ///<BIT [7:6] Reserved.
        uint16_t PHSEL1_CH3           : 6;         ///<BIT [13:8] DLL Phase Select 1 for Channel 3.
        uint16_t RF200E28E_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
    } b;    
} DllCtrl2ForChan3_t;

///  @brief 0xF200E290 : DLL Control 1 for Channel 4
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DELAY_TEST_CH4     : 9;         ///<BIT [8:0] Delay Test Control for Channel 4.
        uint16_t AUTO_UPDATE_EN_CH4 : 1;         ///<BIT [9] Automatic Update Enable for Channel 4.
        uint16_t BYPASS_EN_CH4      : 1;         ///<BIT [10] DLL Delay Bypass for Channel 4.
        uint16_t GAIN2X_CH4         : 1;         ///<BIT [11] 2X Gain Enable for Channel 4.
        uint16_t RST_CH4            : 1;         ///<BIT [12] DLL Reset Enable for Channel 4.
        uint16_t TEST_EN_CH4        : 1;         ///<BIT [13] DLL Test Mode Enable for Channel 4.
        uint16_t UPDATE_EN_CH4      : 1;         ///<BIT [14] DLL Manual Update Enable for Channel 4.
        uint16_t RF200E290_15_RSVD  : 1;         ///<BIT [15] Reserved.
    } b;    
} DllCtrl1ForChan4_t;

///  @brief 0xF200E292 : DLL Control 2 for Channel 4
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PHSEL0_CH4           : 6;         ///<BIT [5:0] DLL Phase Select 0 for Channel 4.
        uint16_t RF200E292_7_6_RSVD   : 2;         ///<BIT [7:6] Reserved.
        uint16_t PHSEL1_CH4           : 6;         ///<BIT [13:8] DLL Phase Select 1 for Channel 4.
        uint16_t RF200E292_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
    } b;    
} DllCtrl2ForChan4_t;

///  @brief 0xF200E294 : DLL Control 1 for Channel 5
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DELAY_TEST_CH5     : 9;         ///<BIT [8:0] Delay Test Control for Channel 5.
        uint16_t AUTO_UPDATE_EN_CH5 : 1;         ///<BIT [9] Automatic Update Enable for Channel 5.
        uint16_t BYPASS_EN_CH5      : 1;         ///<BIT [10] DLL Delay Bypass for Channel 5.
        uint16_t GAIN2X_CH5         : 1;         ///<BIT [11] 2X Gain Enable for Channel 5.
        uint16_t RST_CH5            : 1;         ///<BIT [12] DLL Reset Enable for Channel 5.
        uint16_t TEST_EN_CH5        : 1;         ///<BIT [13] DLL Test Mode Enable for Channel 5.
        uint16_t UPDATE_EN_CH5      : 1;         ///<BIT [14] DLL Manual Update Enable for Channel 5.
        uint16_t RF200E294_15_RSVD  : 1;         ///<BIT [15] Reserved.
    } b;    
} DllCtrl1ForChan5_t;

///  @brief 0xF200E296 : DLL Control 2 for Channel 5
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PHSEL0_CH5           : 6;         ///<BIT [5:0] DLL Phase Select 0 for Channel 5.
        uint16_t RF200E296_7_6_RSVD   : 2;         ///<BIT [7:6] Reserved.
        uint16_t PHSEL1_CH5           : 6;         ///<BIT [13:8] DLL Phase Select 1 for Channel 5.
        uint16_t RF200E296_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
    } b;    
} DllCtrl2ForChan5_t;

///  @brief 0xF200E298 : DLL Control 1 for Channel 6
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DELAY_TEST_CH6     : 9;         ///<BIT [8:0] Delay Test Control for Channel 6.
        uint16_t AUTO_UPDATE_EN_CH6 : 1;         ///<BIT [9] Automatic Update Enable for Channel 6.
        uint16_t BYPASS_EN_CH6      : 1;         ///<BIT [10] DLL Delay Bypass for Channel 6.
        uint16_t GAIN2X_CH6         : 1;         ///<BIT [11] 2X Gain Enable for Channel 6.
        uint16_t RST_CH6            : 1;         ///<BIT [12] DLL Reset Enable for Channel 6.
        uint16_t TEST_EN_CH6        : 1;         ///<BIT [13] DLL Test Mode Enable for Channel 6.
        uint16_t UPDATE_EN_CH6      : 1;         ///<BIT [14] DLL Manual Update Enable for Channel 6.
        uint16_t RF200E298_15_RSVD  : 1;         ///<BIT [15] Reserved.
    } b;    
} DllCtrl1ForChan6_t;

///  @brief 0xF200E29A : DLL Control 2 for Channel 6
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PHSEL0_CH6           : 6;         ///<BIT [5:0] DLL Phase Select 0 for Channel 6.
        uint16_t RF200E29A_7_6_RSVD   : 2;         ///<BIT [7:6] Reserved.
        uint16_t PHSEL1_CH6           : 6;         ///<BIT [13:8] DLL Phase Select 1 for Channel 6.
        uint16_t RF200E29A_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
    } b;    
} DllCtrl2ForChan6_t;

///  @brief 0xF200E29C : DLL Control 1 for Channel 7
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DELAY_TEST_CH7     : 9;         ///<BIT [8:0] Delay Test Control for Channel 7.
        uint16_t AUTO_UPDATE_EN_CH7 : 1;         ///<BIT [9] Automatic Update Enable for Channel 7.
        uint16_t BYPASS_EN_CH7      : 1;         ///<BIT [10] DLL Delay Bypass for Channel 7.
        uint16_t GAIN2X_CH7         : 1;         ///<BIT [11] 2X Gain Enable for Channel 7.
        uint16_t RST_CH7            : 1;         ///<BIT [12] DLL Reset Enable for Channel 7.
        uint16_t TEST_EN_CH7        : 1;         ///<BIT [13] DLL Test Mode Enable for Channel 7.
        uint16_t UPDATE_EN_CH7      : 1;         ///<BIT [14] DLL Manual Update Enable for Channel 7.
        uint16_t RF200E29C_15_RSVD  : 1;         ///<BIT [15] Reserved.
    } b;    
} DllCtrl1ForChan7_t;

///  @brief 0xF200E29E : DLL Control 2 for Channel 7
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PHSEL0_CH7           : 6;         ///<BIT [5:0] DLL Phase Select 0 for Channel 7.
        uint16_t RF200E29E_7_6_RSVD   : 2;         ///<BIT [7:6] Reserved.
        uint16_t PHSEL1_CH7           : 6;         ///<BIT [13:8] DLL Phase Select 1 for Channel 7.
        uint16_t RF200E29E_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
    } b;    
} DllCtrl2ForChan7_t;

///  @brief 0xF200E2C0 : DLL Monitor CH0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DELAY_OUT_CH0        : 8;         ///<BIT [7:0] Delay Output for Channel 0.
        uint16_t LOCK_CH0             : 1;         ///<BIT [8] Lock Status for Channel 0.
        uint16_t CLK_TEST_CH0         : 1;         ///<BIT [9] Reference Clock Output for Channel 0.
        uint16_t RF200E2C0_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
    } b;    
} DllMonitorCh0_t;

///  @brief 0xF200E2C2 : DLL Monitor CH1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DELAY_OUT_CH1        : 8;         ///<BIT [7:0] Delay Output for Channel 1.
        uint16_t LOCK_CH1             : 1;         ///<BIT [8] Lock Status for Channel 1.
        uint16_t CLK_TEST_CH1         : 1;         ///<BIT [9] Reference Clock Output for Channel 1.
        uint16_t RF200E2C2_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
    } b;    
} DllMonitorCh1_t;

///  @brief 0xF200E2C4 : DLL Monitor CH2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DELAY_OUT_CH2        : 8;         ///<BIT [7:0] Delay Output for Channel 2.
        uint16_t LOCK_CH2             : 1;         ///<BIT [8] Lock Status for Channel 2.
        uint16_t CLK_TEST_CH2         : 1;         ///<BIT [9] Reference Clock Output for Channel 2.
        uint16_t RF200E2C4_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
    } b;    
} DllMonitorCh2_t;

///  @brief 0xF200E2C6 : DLL Monitor CH3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DELAY_OUT_CH3        : 8;         ///<BIT [7:0] Delay Output for Channel 3.
        uint16_t LOCK_CH3             : 1;         ///<BIT [8] Lock Status for Channel 3.
        uint16_t CLK_TEST_CH3         : 1;         ///<BIT [9] Reference Clock Output for Channel 3.
        uint16_t RF200E2C6_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
    } b;    
} DllMonitorCh3_t;

///  @brief 0xF200E2C8 : DLL Monitor CH4
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DELAY_OUT_CH4        : 8;         ///<BIT [7:0] Delay Output for Channel 4.
        uint16_t LOCK_CH4             : 1;         ///<BIT [8] Lock Status for Channel 4.
        uint16_t CLK_TEST_CH4         : 1;         ///<BIT [9] Reference Clock Output for Channel 4.
        uint16_t RF200E2C8_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
    } b;    
} DllMonitorCh4_t;

///  @brief 0xF200E2CA : DLL Monitor CH5
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DELAY_OUT_CH5        : 8;         ///<BIT [7:0] Delay Output for Channel 5.
        uint16_t LOCK_CH5             : 1;         ///<BIT [8] Lock Status for Channel 5.
        uint16_t CLK_TEST_CH5         : 1;         ///<BIT [9] Reference Clock Output for Channel 5.
        uint16_t RF200E2CA_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
    } b;    
} DllMonitorCh5_t;

///  @brief 0xF200E2CC : DLL Monitor CH6
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DELAY_OUT_CH6        : 8;         ///<BIT [7:0] Delay Output for Channel 6.
        uint16_t LOCK_CH6             : 1;         ///<BIT [8] Lock Status for Channel 6.
        uint16_t CLK_TEST_CH6         : 1;         ///<BIT [9] Reference Clock Output for Channel 6.
        uint16_t RF200E2CC_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
    } b;    
} DllMonitorCh6_t;

///  @brief 0xF200E2CE : DLL Monitor CH7
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DELAY_OUT_CH7        : 8;         ///<BIT [7:0] Delay Output for Channel 7.
        uint16_t LOCK_CH7             : 1;         ///<BIT [8] Lock Status for Channel 7.
        uint16_t CLK_TEST_CH7         : 1;         ///<BIT [9] Reference Clock Output for Channel 7.
        uint16_t RF200E2CE_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
    } b;    
} DllMonitorCh7_t;

///  @brief 0xF200E300 : Pad Power-Down Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PWR_DOWN_CH0        : 1;         ///<BIT [0] Power-Down for the NAND Pad of Channel 0.
        uint16_t PWR_DOWN_CH1        : 1;         ///<BIT [1] Power-Down for the NAND Pad of Channel 1.
        uint16_t PWR_DOWN_CH2        : 1;         ///<BIT [2] Power-Down for the NAND Pad of Channel 2.
        uint16_t PWR_DOWN_CH3        : 1;         ///<BIT [3] Power-Down for the NAND Pad of Channel 3.
        uint16_t PWR_DOWN_CH4        : 1;         ///<BIT [4] Power-Down for the NAND Pad of Channel 4.
        uint16_t PWR_DOWN_CH5        : 1;         ///<BIT [5] Power-Down for the NAND Pad of Channel 5.
        uint16_t PWR_DOWN_CH6        : 1;         ///<BIT [6] Power-Down for the NAND Pad of Channel 6.
        uint16_t PWR_DOWN_CH7        : 1;         ///<BIT [7] Power-Down for the NAND Pad of Channel 7.
        uint16_t RF200E300_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
    } b;    
} PadPowerDnCtrl_t;

///  @brief 0xF200E302 : Pad Pull-Up and Pull-Down Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t PULL_DOWN_DQ   : 1;         ///<BIT [0] Pull Down for DQ Pad of All Channels.
        uint16_t PULL_UP_DQ     : 1;         ///<BIT [1] Pull Up for DQ Pad of All Channels.
        uint16_t PULL_DOWN_DQSC : 1;         ///<BIT [2] Pull Down for DQS_C Pad of All Channels.
        uint16_t PULL_UP_DQSC   : 1;         ///<BIT [3] Pull Up for DQS_C Pad of All Channels.
        uint16_t PULL_DOWN_DQS  : 1;         ///<BIT [4] Pull Down for DQS Pad of All Channels.
        uint16_t PULL_UP_DQS    : 1;         ///<BIT [5] Pull Up for DQS Pad of All Channels.
        uint16_t PULL_DOWN_AC   : 1;         ///<BIT [6] Pull Down for CLE/ALE/CE Pad of All Channels.
        uint16_t PULL_UP_AC     : 1;         ///<BIT [7] Pull Up for CLE/ALE/CE Pad of All Channels.
        uint16_t PULL_DOWN_REC  : 1;         ///<BIT [8] Pull Down for RE_C Pad of All Channels.
        uint16_t PULL_UP_REC    : 1;         ///<BIT [9] Pull Up for RE_C Pad of All Channels.
        uint16_t PULL_DOWN_REN  : 1;         ///<BIT [10] Pull Down for RE_N Pad of All Channels.
        uint16_t PULL_UP_REN    : 1;         ///<BIT [11] Pull Up for RE_N Pad of All Channels.
        uint16_t PULL_DOWN_WP   : 1;         ///<BIT [12] Pull Down for WP Pad of All channels.
        uint16_t PULL_UP_WP     : 1;         ///<BIT [13] Pull Up for WP Pad of All Channels.
        uint16_t PULL_DOWN_WEN  : 1;         ///<BIT [14] Pull Down for WE_N Pad of All Channels.
        uint16_t PULL_UP_WEN    : 1;         ///<BIT [15] Pull Up for WE_N Pad of All Channels.
    } b;    
} PadPullUpAndPullDnCtrl_t;

///  @brief 0xF200E304 : VREF RCV Type Selection
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200E304_1_0_RSVD : 2;         ///<BIT [1:0] Reserved.
        uint16_t RF200E304_2_RSVD   : 1;         ///<BIT [2] Reserved.
        uint16_t DQS_VREF_SEL       : 1;         ///<BIT [3] DQS Reference Voltage Select for All Channels.
        uint16_t RF200E304_5_4_RSVD : 2;         ///<BIT [5:4] Reserved.
        uint16_t RF200E304_6_RSVD   : 1;         ///<BIT [6] Reserved.
        uint16_t REB_VREF_SEL       : 1;         ///<BIT [7] REB Reference Voltage Select for All Channels.
        uint16_t RF200E304_8_RSVD   : 1;         ///<BIT [8] Reserved.
        uint16_t WEB_VREF_SEL       : 1;         ///<BIT [9] WEB Reference Voltage Select for All Channels.
        uint16_t RF200E304_10_RSVD  : 1;         ///<BIT [10] Reserved.
        uint16_t DQ_RCVTYPE         : 1;         ///<BIT [11] DQ Receiver Type Select for All Channels.
        uint16_t DQS_RCVTYPE        : 1;         ///<BIT [12] DQS Receiver Type Select for All Channels.
        uint16_t CMD_RCVTYPE        : 1;         ///<BIT [13] Command (CLE/ALE/CE) Receiver Type Select for All Channels.
        uint16_t REB_RCVTYPE        : 1;         ///<BIT [14] REB Receiver Type Select for All Channels.
        uint16_t WEB_RCVTYPE        : 1;         ///<BIT [15] WEB Receiver Type Select for All Channels.
    } b;    
} VrefRcvTypeSelectio_t;

///  @brief 0xF200E324 : Data Sub PHY ODT Setting 1
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ODT_SEL_CH0       : 3;         ///<BIT [2:0] Data Sub-PHY DQ/DQS ODT Value Select for Channel 0.
        uint32_t RF200E324_3_RSVD  : 1;         ///<BIT [3] Reserved.
        uint32_t ODT_SEL_CH1       : 3;         ///<BIT [6:4] Data Sub-PHY DQ/DQS ODT Value Select for Channel 1.
        uint32_t RF200E324_7_RSVD  : 1;         ///<BIT [7] Reserved.
        uint32_t ODT_SEL_CH2       : 3;         ///<BIT [10:8] Data Sub-PHY DQ/DQS ODT Value Select for Channel 2.
        uint32_t RF200E324_11_RSVD : 1;         ///<BIT [11] Reserved.
        uint32_t ODT_SEL_CH3       : 3;         ///<BIT [14:12] Data Sub-PHY DQ/DQS ODT Value Select for Channel 3.
        uint32_t RF200E324_15_RSVD : 1;         ///<BIT [15] Reserved.
        uint32_t ODT_SEL_CH4       : 3;         ///<BIT [18:16] Data Sub-PHY DQ/DQS ODT Value Select for Channel 4.
        uint32_t RF200E324_19_RSVD : 1;         ///<BIT [19] Reserved.
        uint32_t ODT_SEL_CH5       : 3;         ///<BIT [22:20] Data Sub-PHY DQ/DQS ODT Value Select for Channel 5.
        uint32_t RF200E324_23_RSVD : 1;         ///<BIT [23] Reserved.
        uint32_t ODT_SEL_CH6       : 3;         ///<BIT [26:24] Data Sub-PHY DQ/DQS ODT Value Select for Channel 6.
        uint32_t RF200E324_27_RSVD : 1;         ///<BIT [27] Reserved.
        uint32_t ODT_SEL_CH7       : 3;         ///<BIT [30:28] Data Sub-PHY DQ/DQS ODT Value Select for Channel 7.
        uint32_t RF200E324_31_RSVD : 1;         ///<BIT [31] Reserved.
    } b;    
} DataSubPhyOdtSetting1_t;

///  @brief 0xF200E32C : Command Sub PHY ODT Setting
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t CMD_ODT_SEL          : 3;         ///<BIT [2:0] Command Sub-PHY ODT Value for All Channels.
        uint16_t RF200E32C_3_RSVD     : 1;         ///<BIT [3] Reserved.
        uint16_t REB_ODT_SEL          : 3;         ///<BIT [6:4] REB Sub-PHY ODT Value for All Channels.
        uint16_t RF200E32C_7_RSVD     : 1;         ///<BIT [7] Reserved.
        uint16_t WEB_ODT_SEL          : 3;         ///<BIT [10:8] WEB Sub-PHY ODT Value for All Channels.
        uint16_t RF200E32C_15_11_RSVD : 5;         ///<BIT [15:11] Reserved.
    } b;    
} CmdSubPhyOdtSetting_t;

///  @brief 0xF200E32E : DQS Gate Control for Channel 0 to 3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DQS_GATE_CTRL_CH0 : 4;         ///<BIT [3:0] DQS Gate Cycle Control for Channel 0.
        uint16_t DQS_GATE_CTRL_CH1 : 4;         ///<BIT [7:4] DQS Gate Cycle Control for Channel 1.
        uint16_t DQS_GATE_CTRL_CH2 : 4;         ///<BIT [11:8] DQS Gate Cycle Control for Channel 2.
        uint16_t DQS_GATE_CTRL_CH3 : 4;         ///<BIT [15:12] DQS Gate Cycle Control for Channel 3.
    } b;    
} DqsGateCtrlForChan0To3_t;

///  @brief 0xF200E330 : DQS Gate Control for Channel 4 to 7
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DQS_GATE_CTRL_CH4 : 4;         ///<BIT [3:0] DQS Gate Cycle Control for Channel 4.
        uint16_t DQS_GATE_CTRL_CH5 : 4;         ///<BIT [7:4] DQS Gate Cycle Control for Channel 5.
        uint16_t DQS_GATE_CTRL_CH6 : 4;         ///<BIT [11:8] DQS Gate Cycle Control for Channel 6.
        uint16_t DQS_GATE_CTRL_CH7 : 4;         ///<BIT [15:12] DQS Gate Cycle Control for Channel 7.
    } b;    
} DqsGateCtrlForChan4To7_t;

///  @brief 0xF200E380 : NAND PHY Read Sequence Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RDRST_CTRL    : 4;         ///<BIT [3:0] Data PHY FIFO Reset Assertion Control.
        uint16_t RECEN_CTRL    : 4;         ///<BIT [7:4] Data PHY Receiver Enable Control.
        uint16_t RPTR_DLY_CTRL : 4;         ///<BIT [11:8] Data PHY FIFO Read Pointer Delay Control.
        uint16_t ODT_DLY_CTRL  : 4;         ///<BIT [15:12] Data PHY ODT Enable Delay Control.
    } b;    
} NandPhyRdSequenceCtrl_t;

///  @brief 0xF200E382 : NAND PHY Configuration 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF200E382_0_RSVD     : 1;         ///<BIT [0] Reserved.
        uint16_t CLK_SEL              : 1;         ///<BIT [1] CLK Speed Selection.
        uint16_t FCLK_SEL             : 1;         ///<BIT [2] FC_CLK and FCLK Speed Selection.
        uint16_t RESET_ABORT_TIME_OUT : 1;         ///<BIT [3] Reset Abort Time-Out.
        uint16_t MP_ER_SEQ_SEL        : 1;         ///<BIT [4] Multi-Plane Erase Sequence Select.
        uint16_t NO_RE_C              : 1;         ///<BIT [5] RE Complement (RE_C) Pin Disable.
        uint16_t NO_DQS_C             : 1;         ///<BIT [6] DQS Complement (DQS_C) Pin Disable.
        uint16_t DIS_PUPD_ODT1        : 1;         ///<BIT [7] DQ/DQS/DQS_C Pull-Up or Pull-Down Disable for ODT Enabled.
        uint16_t EN_DQS_GATE          : 1;         ///<BIT [8] DQS Gate Envelope Enable.
        uint16_t BUSY_INFO_SEL        : 1;         ///<BIT [9] Busy Information Logging Unit Select.
        uint16_t REN_UBAL_EN          : 1;         ///<BIT [10] Unbalanced RE_N Duty Cycle Enable.
        uint16_t WEN_UBAL_EN          : 1;         ///<BIT [11] Unbalanced WE_N Duty Cycle Enable.
        uint16_t EN_DG                : 1;         ///<BIT [12] Dependency Group Logic Enable.
        uint16_t DG_FLEX_EN           : 1;         ///<BIT [13] Dependency Group Flexible Descriptor Enable.
        uint16_t RF200E382_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
    } b;    
} NandPhyCfg1_t;

///  @brief 0xF200E384 : DDR2 Warm-Up Cycle Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t READ_LATENCY  : 4;         ///<BIT [3:0] Read RE and DQS Warm-Up Cycle Latency in NV-DDR2 or Toggle2 Mod.
        uint16_t PROG_LATENCY  : 4;         ///<BIT [7:4] Program DQS Warm-Up Cycle Latency in NV-DDR2 or Toggle2 Mode.
        uint16_t ABORT_TO_READ : 4;         ///<BIT [11:8] Abort on Time-Out Limitation For READ Operations.
        uint16_t ABORT_TO_PROG : 4;         ///<BIT [15:12] Abort on Time-Out Limitation For PROGRAM Operations.
    } b;    
} Ddr2WarmUpCycleCtrl_t;

///  @brief 0xF200E386 : Flash Interface Configuration 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DQ_MIRROR_EN_CH0    : 1;         ///<BIT [0] DQ Bit Mirroring Enable for Channel 0.
        uint16_t DQ_MIRROR_EN_CH1    : 1;         ///<BIT [1] DQ Bit Mirroring Enable for Channel 1.
        uint16_t DQ_MIRROR_EN_CH2    : 1;         ///<BIT [2] DQ Bit Mirroring Enable for Channel 2.
        uint16_t DQ_MIRROR_EN_CH3    : 1;         ///<BIT [3] DQ Bit Mirroring Enable for Channel 3.
        uint16_t DQ_MIRROR_EN_CH4    : 1;         ///<BIT [4] DQ Bit Mirroring Enable for Channel 4.
        uint16_t DQ_MIRROR_EN_CH5    : 1;         ///<BIT [5] DQ Bit Mirroring Enable for Channel 5.
        uint16_t DQ_MIRROR_EN_CH6    : 1;         ///<BIT [6] DQ Bit Mirroring Enable for Channel 6.
        uint16_t DQ_MIRROR_EN_CH7    : 1;         ///<BIT [7] DQ Bit Mirroring Enable for Channel 7.
        uint16_t RF200E386_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
    } b;    
} FlashInterfacCfg2_t;

///  @brief 0xF200E388 : Flash Interface Configuration 3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t BUSY_DIV_SEL      : 5;         ///<BIT [4:0] Busy Information Clock Divider Value Select.
        uint16_t BUSY_LOG_EN       : 1;         ///<BIT [5] Busy Information Logging Enable.
        uint16_t ABORT_TIME_OUT_EN : 1;         ///<BIT [6] Abort Time-Out Enable.
        uint16_t ABORT_TO_RDSR     : 5;         ///<BIT [11:7] Abort Time-Out Limitation for RDSR Operations.
        uint16_t ABORT_TO_ERASE    : 4;         ///<BIT [15:12] Abort Time-Out Limitation for ERASE Operations.
    } b;    
} FlashInterfacCfg3_t;

///  @brief 0xF200E390 : NAND PHY Initial Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t NP_SYNC_RSTB        : 1;         ///<BIT [0] NAND PHY Reset B.
        uint16_t NP_RST_EN           : 1;         ///<BIT [1] NAND PHY Reset Enable.
        uint16_t CLK_ALIGN_GEN       : 1;         ///<BIT [2] Clock Timing Align Generate.
        uint16_t NP_ZD_EN            : 1;         ///<BIT [3] NAND PHY Pad Output Driver Enable.
        uint16_t RF200E390_15_4_RSVD :12;         ///<BIT [15:4] Reserved.
    } b;    
} NandPhyInitCtrl_t;

///  @brief 0xF200E392 : Hidden Configuration 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RRDY_CNT            : 4;         ///<BIT [3:0] Read Buffer Ready Count.
        uint16_t RF200E392_15_4_RSVD :12;         ///<BIT [15:4] Reserved.
    } b;    
} HiddenCfg1_t;

///  @brief 0xF200E398 : Command Memory Access
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t CM_ACC_EN           : 1;         ///<BIT [0] Enable Command Memory Access From CPU.
        uint16_t CM_AUTO_WADDR       : 1;         ///<BIT [1] Automatic Command Memory Address Increment.
        uint16_t RF200E398_15_2_RSVD :14;         ///<BIT [15:2] Reserved.
    } b;    
} CmdMemAccess_t;

///  @brief 0xF200E39A : Command Memory Address
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t CM_ADDR             : 8;         ///<BIT [7:0] Command Memory Access Address.
        uint16_t RF200E39A_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
    } b;    
} CmdMemAddr_t;

///  @brief 0xF200E39E : Command Memory Data Port 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t CM_DATA2            : 4;         ///<BIT [3:0] Command Memory Read/Write Data [19:16].
        uint16_t RF200E39E_15_4_RSVD :12;         ///<BIT [15:4] Reserved.
    } b;    
} CmdMemDataPort2_t;

///  @brief 0xF200E3A0 : Flexible Mode DQ Table 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t F_DQ_0 : 8;         ///<BIT [7:0] DQ0 Value for the Flexible Mode.
        uint16_t F_DQ_1 : 8;         ///<BIT [15:8] DQ1 Value for the Flexible Mode.
    } b;    
} FlexibleModeDqTbl1_t;

///  @brief 0xF200E3A2 : Flexible Mode DQ Table 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t F_DQ_2 : 8;         ///<BIT [7:0] DQ2 Value for the Flexible Mode.
        uint16_t F_DQ_3 : 8;         ///<BIT [15:8] DQ3 Value for the Flexible Mode.
    } b;    
} FlexibleModeDqTbl2_t;

///  @brief 0xF200E3A4 : Flexible Mode DQ Table 3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t F_DQ_4 : 8;         ///<BIT [7:0] DQ4 Value for the Flexible Mode.
        uint16_t F_DQ_5 : 8;         ///<BIT [15:8] DQ5 Value for the Flexible Mode.
    } b;    
} FlexibleModeDqTbl3_t;

///  @brief 0xF200E3A6 : Flexible Mode DQ Table 4
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t F_DQ_6 : 8;         ///<BIT [7:0] DQ6 Value for the Flexible Mode.
        uint16_t F_DQ_7 : 8;         ///<BIT [15:8] DQ7 Value for the Flexible Mode.
    } b;    
} FlexibleModeDqTbl4_t;

///  @brief 0xF200E3A8 : Flexible Mode DQ Table 5
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t F_DQ_8 : 8;         ///<BIT [7:0] DQ8 Value for the Flexible Mode.
        uint16_t F_DQ_9 : 8;         ///<BIT [15:8] DQ9 Value for the Flexible Mode.
    } b;    
} FlexibleModeDqTbl5_t;

///  @brief 0xF200E3AA : Flexible Mode DQ Table 6
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t F_DQ_10 : 8;         ///<BIT [7:0] DQ10 Value for the Flexible Mode.
        uint16_t F_DQ_11 : 8;         ///<BIT [15:8] DQ11 Value for the Flexible Mode.
    } b;    
} FlexibleModeDqTbl6_t;

///  @brief 0xF200E3AC : Flexible Mode DQ Table 7
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t F_DQ_12 : 8;         ///<BIT [7:0] DQ12 Value for the Flexible Mode.
        uint16_t F_DQ_13 : 8;         ///<BIT [15:8] DQ13 Value for the Flexible Mode.
    } b;    
} FlexibleModeDqTbl7_t;

///  @brief 0xF200E3AE : Flexible Mode DQ Table 8
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t F_DQ_14 : 8;         ///<BIT [7:0] DQ14 Value for the Flexible Mode.
        uint16_t F_DQ_15 : 8;         ///<BIT [15:8] DQ15 Value for the Flexible Mode.
    } b;    
} FlexibleModeDqTbl8_t;

#if (_SOC_REV_ < 0x0210)
///  @brief 0xF200E3B0 : DCT SRAM Access Configuration
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DCT_CPU_ACC_EN       : 1;         ///<BIT [0] DCT SRAM Path CPU Access Enable.
        uint16_t DCT_AUTO_WADDR       : 1;         ///<BIT [1] DCT Address Automatic Increment Enable During CPU Write to DCT .
        uint16_t DCT_DPG_EN           : 1;         ///<BIT [2] Data Protection Parity Generation Enable During Write to DCT SR.
        uint16_t DCT_DPP_EN           : 1;         ///<BIT [3] DCT SRAM Data Protection Enable.
        uint16_t DC_TRK_EN            : 1;         ///<BIT [4] Defective Column Tracking Enable.
        uint16_t SW_CTRL_DCT_PTR      : 1;         ///<BIT [5] Software Control for DCT Address Pointer in Descriptor.
        uint16_t DCT_RTC              : 2;         ///<BIT [7:6] RTC for DCT SRAMs in All Channels.
        uint16_t DCT_WTC              : 2;         ///<BIT [9:8] WTC for DCT SRAMs in All Channels.
        uint16_t RF200E3B0_11_10_RSVD : 2;         ///<BIT [11:10] Reserved.
        uint16_t DCT_CH_SEL           : 4;         ///<BIT [15:12] Channel Select for CPU Access of the DCT SRAM.
    } b;    
} DctSramAccessCfg_t;

///  @brief 0xF200E3B2 : DCT SRAM Address Port
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DCT_ADDR             :12;         ///<BIT [11:0] Initial Address for CPU Access of the DCT SRAM.
        uint16_t RF200E3B2_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
    } b;    
} DctSramAddrPort_t;

///  @brief 0xF200E3BC : DCT SRAM LRM0 Control for Channel 0 through 7
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DCT_PDLVMC_0  : 8;         ///<BIT [7:0] DCT SRAM PDLVMC Control 0.
        uint16_t DCT_PDFVSSM_0 : 8;         ///<BIT [15:8] DCT SRAM PDFVSSM Control 0.
    } b;    
} DctSramLrm0CtrlForChan0Through_t;

///  @brief 0xF200E3C0 : DCT SRAM LRM1 Control for Channel 8 through 15
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DCT_PDLVMC_1  : 8;         ///<BIT [7:0] DCT SRAM PDLVMC Control 1.
        uint16_t DCT_PDFVSSM_1 : 8;         ///<BIT [15:8] DCT SRAM PDFVSSM Control 1.
    } b;    
} DctSramLrm1CtrlForChan8Through_t;
#endif  // #if (_SOC_REV_ < 0x0210)

///  @brief 0xF200E3C2 : HPRJ Configuration
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t HPRJ_JOB_PROC_NUM    : 3;         ///<BIT [2:0] HPRJ Maximum Job Number.
        uint16_t NO_INSRT_BEFORE_PROG : 1;         ///<BIT [3] HPRJ Insert Jobs Before PROG.
        uint16_t FC_CLE_DEFAULT       : 1;         ///<BIT [4] Enable CLE Default High.
        uint16_t RF200E3C2_11_5_RSVD  : 7;         ///<BIT [11:5] Reserved.
        uint16_t FC_RE_ASYNC_EN       : 1;         ///<BIT [12] Asymmetric Read/Write Speed Enable.
        uint16_t FC_RE_ASYNC_LMT      : 2;         ///<BIT [14:13] Frequency when Read Asymmetric Read/Write Speed Enabled.
        uint16_t RF200E3C2_15_RSVD    : 1;         ///<BIT [15] Reserved.
    } b;    
} HprjCfg_t;

///  @brief 0xF200E3C4 : Read Status Mask 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t READY_BIT3  : 3;         ///<BIT [2:0] Ready Bit 3.
        uint16_t FAIL_BIT7   : 3;         ///<BIT [5:3] Fail Bit 7.
        uint16_t FAIL_BIT8   : 3;         ///<BIT [8:6] Fail Bit 8.
        uint16_t FAIL_BIT9   : 3;         ///<BIT [11:9] Fail Bit 9.
        uint16_t FAIL_BIT10  : 3;         ///<BIT [14:12] Fail Bit 10.
        uint16_t EN_RDSR_ROW : 1;         ///<BIT [15] Enable RDSR Row.
    } b;    
} RdStatMsk0_t;

///  @brief 0xF200E3C6 : Read Status Mask 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t FAIL_BIT11           : 3;         ///<BIT [2:0] Fail Bit 11.
        uint16_t RF200E3C6_3_RSVD     : 1;         ///<BIT [3] Reserved.
        uint16_t FAIL_BIT15           : 3;         ///<BIT [6:4] Fail Bit 15.
        uint16_t RF200E3C6_7_RSVD     : 1;         ///<BIT [7] Reserved.
        uint16_t FAIL_BIT16           : 3;         ///<BIT [10:8] Fail Bit 16.
        uint16_t RF200E3C6_15_11_RSVD : 5;         ///<BIT [15:11] Reserved.
    } b;    
} RdStatMsk1_t;

///  @brief 0xF200E3C8 : NAND Time-Out Status
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t STOP_ON_TIME_OUT_ST : 4;         ///<BIT [3:0] Stop on Time-Out Status.
        uint16_t RF200E3C8_15_4_RSVD :12;         ///<BIT [15:4] Reserved.
    } b;    
} NandTimeOutStat_t;

///  @brief 0xF200E3CA : Suspend Polling and Multi-Volume Enable
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t VOL_SEL              : 1;         ///<BIT [0] Volume Select.
        uint16_t TSPD_STEP            : 2;         ///<BIT [2:1] Timing Suspend Step.
        uint16_t TSPD_WT_CNT          : 6;         ///<BIT [8:3] Timing Suspend Wait Counter.
        uint16_t VOL_NUM_BIT_USED     : 3;         ///<BIT [11:9] Volume Number Bit Used.
        uint16_t RF200E3CA_15_12_RSVD : 4;         ///<BIT [15:12] Reserved.
    } b;    
} SuspendPollingAndMultiVolumeEn_t;

///  @brief 0xF200E3CC : Volume Select Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t CMD_VOL_SEL : 8;         ///<BIT [7:0] Volume Select Command.
        uint16_t TVDLY       : 8;         ///<BIT [15:8] Volume Delay Timer.
    } b;    
} VolumeSelCtrl_t;

///  @brief 0xF200E3CE : Erase Suspend Command Port
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t CMD_ERASE_RES : 8;         ///<BIT [7:0] Command for Erase Resume.
        uint16_t CMD_ERASE_SUS : 8;         ///<BIT [15:8] Command for Erase Suspend.
    } b;    
} EraseSuspendCmdPort_t;

///  @brief 0xF200E3D0 : Program Suspend Command Port
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t CMD_PROG_RES : 8;         ///<BIT [7:0] Command for Erase Resume.
        uint16_t CMD_PROG_SUS : 8;         ///<BIT [15:8] Command for Erase Suspend.
    } b;    
} ProgSuspendCmdPort_t;

///  @brief 0xF200E3D2 : Suspend Timing Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t TPSPD : 8;         ///<BIT [7:0] Maximum Time for Program Suspend Command.
        uint16_t TESPD : 8;         ///<BIT [15:8] Maximum Time for Erase Suspend Command.
    } b;    
} SuspendTmCtrl_t;

///  @brief 0xF200E3D4 : Flash Sequencer Status Byte Control 6
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t FAIL_BIT12        : 3;         ///<BIT [2:0] Fail Bit 12.
        uint16_t RF200E3D4_3_RSVD  : 1;         ///<BIT [3] Reserved.
        uint16_t READY_BIT4        : 3;         ///<BIT [6:4] Ready Bit 4.
        uint16_t RF200E3D4_7_RSVD  : 1;         ///<BIT [7] Reserved.
        uint16_t FAIL_BIT13        : 3;         ///<BIT [10:8] Fail Bit 13.
        uint16_t RF200E3D4_11_RSVD : 1;         ///<BIT [11] Reserved.
        uint16_t FAIL_BIT14        : 3;         ///<BIT [14:12] Fail Bit 14.
        uint16_t RF200E3D4_15_RSVD : 1;         ///<BIT [15] Reserved.
    } b;    
} FlashSeqStatByteCtrl6_t;


/// @brief 0xF200BA00-0xF200BA28 : Fct_t MODULE
typedef struct
{
    NandFlashControllIntrStat_t nandFlashControllIntrStat;                                 ///< 0xF200BA00 : NAND Flash Controller Interrupt Status
    NandFlashControllIntrEnForSocCpu_t nandFlashControllIntrEnForSocCpu;                   ///< 0xF200BA02 : NAND Flash Controller Interrupt Enable for SoC CPU
    NandFlashControllTstBusSel_t nandFlashControllTstBusSel;                               ///< 0xF200BA04 : NAND Flash Controller Test Bus Select
    uint8_t rsvd_RF200BA06[2];                                                             ///< 0xF200BA06 - 0xF200BA07 : Reserved.
    NandFlashInterfacDriverAndSeqRevisionId_t nandFlashInterfacDriverAndSeqRevisionId;     ///< 0xF200BA08 : NAND Flash Interface Driver and Sequencer Revision ID
    EccAndRevisionId_t eccAndRevisionId;                                                   ///< 0xF200BA0A : ECC and Revision ID
    uint8_t rsvd_RF200BA0C[14];                                                            ///< 0xF200BA0C - 0xF200BA19 : Reserved.
    NandFlashControllEcuIntrEnForSoc_t nandFlashControllEcuIntrEnForSoc;                   ///< 0xF200BA1A : NAND Flash Controller ECU Interrupt Enable for SoC CPU
    uint8_t rsvd_RF200BA1C[6];                                                             ///< 0xF200BA1C - 0xF200BA21 : Reserved.
    uint16_t FCT_TEST_OUT;                                                                 ///< 0xF200BA22 : NAND Flash Controller Interrupt Enable for SoC CPU
} Fct_t;
COMPILE_ASSERT(offsetof(Fct_t,FCT_TEST_OUT)==0xF200BA22-0xF200BA00,"check register structure offset");


/// @brief 0xF200E000-0xF200E800 : NandFlashIf_t MODULE
typedef struct
{
    FlashInterfacCfg_t flashInterfacCfg;                                               ///< 0xF200E000 : Flash Interface Configuration
    FlashDirectIndAccessCfg_t flashDirectIndAccessCfg;                                 ///< 0xF200E002 : Flash Direct/Indirect Access Configuration
    FlashInterfacTmCtrl1_t flashInterfacTmCtrl1;                                       ///< 0xF200E004 : Flash Interface Timing Control 1
    FlashInterfacTmCtrl2_t flashInterfacTmCtrl2;                                       ///< 0xF200E006 : Flash Interface Timing Control 2
    FlashInterfacTmCtrl3AsynchroDeviceAnd_t flashInterfacTmCtrl3AsynchroDeviceAnd;     ///< 0xF200E008 : Flash Interface Timing Control 3 (Asynchronous Device and SDR Device)
    FlashInterfacTmCtrl4Toggle1Toggle2_t flashInterfacTmCtrl4Toggle1Toggle2;           ///< 0xF200E00A : Flash Interface Timing Control 4 (Toggle 1/Toggle2/NV-DDR2 Device)
    FlashDirectAccessCtrlPort1_t flashDirectAccessCtrlPort1;                           ///< 0xF200E00C : Flash Direct Access Control Port 1
    uint16_t FLASH_ADDR_DATA_DIR;                                                      ///< 0xF200E00E : Flash Direct Access Address/Data Port
    FlashInterfacDataLatchTmCtrlChan0_t flashInterfacDataLatchTmCtrlChan0;             ///< 0xF200E010 : Flash Interface Data Latch Timing Control Channel 0 and Channel 1
    FlashInterfacDataLatchTmCtrlChan2_t flashInterfacDataLatchTmCtrlChan2;             ///< 0xF200E012 : Flash Interface Data Latch Timing Control Channel 2 and Channel 3
    FlashInterfacDataLatchTmCtrlChan4_t flashInterfacDataLatchTmCtrlChan4;             ///< 0xF200E014 : Flash Interface Data Latch Timing Control Channel 4 and Channel 5
    FlashInterfacDataLatchTmCtrlChan6_t flashInterfacDataLatchTmCtrlChan6;             ///< 0xF200E016 : Flash Interface Data Latch Timing Control Channel 6 and Channel 7
    uint8_t rsvd_RF200E018[8];                                                         ///< 0xF200E018 - 0xF200E01F : Reserved.
    FlashIndAccessCfg_t flashIndAccessCfg;                                             ///< 0xF200E020 : Flash Indirect Access Configuration
    FlashIndAccessCmdPort1_t flashIndAccessCmdPort1;                                   ///< 0xF200E022 : Flash Indirect Access Command Port 1
    FlashIndAccessCmdPort2_t flashIndAccessCmdPort2;                                   ///< 0xF200E024 : Flash Indirect Access Command Port 2
    FlashIndAccessCmdPort3_t flashIndAccessCmdPort3;                                   ///< 0xF200E026 : Flash Indirect Access Command Port 3
    FlashIndAccessCmdPort4_t flashIndAccessCmdPort4;                                   ///< 0xF200E028 : Flash Indirect Access Command Port 4
    uint16_t XFER_CNT_IND;                                                             ///< 0xF200E02A : Flash Indirect Access Data Transfer Count
    uint16_t FLASH_DATA_IND_15_0;                                                      ///< 0xF200E02C : Flash Indirect Access Data Port Low
    uint16_t FLASH_DATA_IND_31_16;                                                     ///< 0xF200E02E : Flash Indirect Access Data Port High
    FlashInterfacTmCtrl5_t flashInterfacTmCtrl5;                                       ///< 0xF200E030 : Flash Interface Timing Control 5
    FlashInterfacTmCtrl6_t flashInterfacTmCtrl6;                                       ///< 0xF200E032 : Flash Interface Timing Control 6
    FlashInterfacTmCtrl7_t flashInterfacTmCtrl7;                                       ///< 0xF200E034 : Flash Interface Timing Control 7
    FlashInterfacTmCtrl8_t flashInterfacTmCtrl8;                                       ///< 0xF200E036 : Flash Interface Timing Control 8
    FlashInterfacTmCtrl9_t flashInterfacTmCtrl9;                                       ///< 0xF200E038 : Flash Interface Timing Control 9
    uint16_t INT_STR;                                                                  ///< 0xF200E03A : Flash Sequencer Interrupt Status
    uint16_t ERR_DES_ID;                                                               ///< 0xF200E03C : Flash Sequencer Error Descriptor ID
    FlashSeqCtrl2_t flashSeqCtrl2;                                                     ///< 0xF200E03E : Flash Sequencer Control 2
    FlashSeqCtrl1_t flashSeqCtrl1;                                                     ///< 0xF200E040 : Flash Sequencer Control 1
    FlashSeqCmdPort1_t flashSeqCmdPort1;                                               ///< 0xF200E042 : Flash Sequencer Command Port 1
    FlashSeqCmdPort2_t flashSeqCmdPort2;                                               ///< 0xF200E044 : Flash Sequencer Command Port 2
    FlashSeqCmdPort3_t flashSeqCmdPort3;                                               ///< 0xF200E046 : Flash Sequencer Command Port 3
    FlashSeqCmdPort4_t flashSeqCmdPort4;                                               ///< 0xF200E048 : Flash Sequencer Command Port 4
    FlashSeqCmdPort5_t flashSeqCmdPort5;                                               ///< 0xF200E04A : Flash Sequencer Command Port 5
    FlashSeqCmdPort6_t flashSeqCmdPort6;                                               ///< 0xF200E04C : Flash Sequencer Command Port 6
    uint8_t rsvd_RF200E04E[18];                                                        ///< 0xF200E04E - 0xF200E05F : Reserved.
    FlashSeqInterfacTmChan0Cfg_t flashSeqInterfacTmChan0Cfg;                           ///< 0xF200E060 : Flash Sequencer Interface Timing Channel 0 Configuration
    FlashSeqInterfacTmChan1Cfg_t flashSeqInterfacTmChan1Cfg;                           ///< 0xF200E062 : Flash Sequencer Interface Timing Channel 1 Configuration
    FlashSeqInterfacTmChan2Cfg_t flashSeqInterfacTmChan2Cfg;                           ///< 0xF200E064 : Flash Sequencer Interface Timing Channel 2 Configuration
    FlashSeqInterfacTmChan3Cfg_t flashSeqInterfacTmChan3Cfg;                           ///< 0xF200E066 : Flash Sequencer Interface Timing Channel 3 Configuration
    FlashSeqInterfacTmChan4Cfg_t flashSeqInterfacTmChan4Cfg;                           ///< 0xF200E068 : Flash Sequencer Interface Timing Channel 4 Configuration
    FlashSeqInterfacTmChan5Cfg_t flashSeqInterfacTmChan5Cfg;                           ///< 0xF200E06A : Flash Sequencer Interface Timing Channel 5 Configuration
    FlashSeqInterfacTmChan6Cfg_t flashSeqInterfacTmChan6Cfg;                           ///< 0xF200E06C : Flash Sequencer Interface Timing Channel 6 Configuration
    FlashSeqInterfacTmChan7Cfg_t flashSeqInterfacTmChan7Cfg;                           ///< 0xF200E06E : Flash Sequencer Interface Timing Channel 7 Configuration
    FlashSeqInterfacTmCtrl1_t flashSeqInterfacTmCtrl1;                                 ///< 0xF200E070 : Flash Sequencer Interface Timing Control 1
    FlashSeqInterfacTmCtrl2_t flashSeqInterfacTmCtrl2;                                 ///< 0xF200E072 : Flash Sequencer Interface Timing Control 2
    FlashSeqInterfacTmCtrl3_t flashSeqInterfacTmCtrl3;                                 ///< 0xF200E074 : Flash Sequencer Interface Timing Control 3
    FlashSeqInterfacTmCtrl4_t flashSeqInterfacTmCtrl4;                                 ///< 0xF200E076 : Flash Sequencer Interface Timing Control 4
    FlashSeqInterfacTmCtrl5_t flashSeqInterfacTmCtrl5;                                 ///< 0xF200E078 : Flash Sequencer Interface Timing Control 5
    FlashSeqInterfacTmCtrl6_t flashSeqInterfacTmCtrl6;                                 ///< 0xF200E07A : Flash Sequencer Interface Timing Control 6
    FlashSeqInterfacTmCtrl7_t flashSeqInterfacTmCtrl7;                                 ///< 0xF200E07C : Flash Sequencer Interface Timing Control 7
    FlashSeqInterfacTmCtrl8_t flashSeqInterfacTmCtrl8;                                 ///< 0xF200E07E : Flash Sequencer Interface Timing Control 8
    FlashSeqChanEngineClkCtrl1_t flashSeqChanEngineClkCtrl1;                           ///< 0xF200E080 : Flash Sequencer Channel Engine Clock Control 1
    uint8_t rsvd_RF200E082[2];                                                         ///< 0xF200E082 - 0xF200E083 : Reserved.
    FlashSeqStatByteCtrl1_t flashSeqStatByteCtrl1;                                     ///< 0xF200E084 : Flash Sequencer Status Byte Control 1
    FlashSeqStatByteCtrl2_t flashSeqStatByteCtrl2;                                     ///< 0xF200E086 : Flash Sequencer Status Byte Control 2
    FlashSeqStatByteCtrl3_t flashSeqStatByteCtrl3;                                     ///< 0xF200E088 : Flash Sequencer Status Byte Control 3
    FlashSeqStatByteCtrl4_t flashSeqStatByteCtrl4;                                     ///< 0xF200E08A : Flash Sequencer Status Byte Control 4
    uint8_t rsvd_RF200E08C[4];                                                         ///< 0xF200E08C - 0xF200E08F : Reserved.
    FlashSeqFlashSeqCmdPort7_t flashSeqFlashSeqCmdPort7;                               ///< 0xF200E090 : Flash Sequencer Flash Sequencer Command Port 7
    FlashSeqCmdPort8_t flashSeqCmdPort8;                                               ///< 0xF200E092 : Flash Sequencer Command Port 8
    FlashSeqCmdPort9_t flashSeqCmdPort9;                                               ///< 0xF200E094 : Flash Sequencer Command Port 9
    FlashSeqCmdPort10_t flashSeqCmdPort10;                                             ///< 0xF200E096 : Flash Sequencer Command Port 10
    FlashSeqCmdPort11_t flashSeqCmdPort11;                                             ///< 0xF200E098 : Flash Sequencer Command Port 11
    FlashSeqCmdPort12_t flashSeqCmdPort12;                                             ///< 0xF200E09A : Flash Sequencer Command Port 12
    uint8_t rsvd_RF200E09C[4];                                                         ///< 0xF200E09C - 0xF200E09F : Reserved.
    FlashSeqChan0BlkBoundaryInformat_t flashSeqChan0BlkBoundaryInformat;               ///< 0xF200E0A0 : Flash Sequencer Channel 0 Block Boundary Information
    FlashSeqChan1BlkBoundaryInformat_t flashSeqChan1BlkBoundaryInformat;               ///< 0xF200E0A2 : Flash Sequencer Channel 1 Block Boundary Information
    FlashSeqChan2BlkBoundaryInformat_t flashSeqChan2BlkBoundaryInformat;               ///< 0xF200E0A4 : Flash Sequencer Channel 2 Block Boundary Information
    FlashSeqChan3BlkBoundaryInformat_t flashSeqChan3BlkBoundaryInformat;               ///< 0xF200E0A6 : Flash Sequencer Channel 3 Block Boundary Information
    FlashSeqChan4BlkBoundaryInformat_t flashSeqChan4BlkBoundaryInformat;               ///< 0xF200E0A8 : Flash Sequencer Channel 4 Block Boundary Information
    FlashSeqChan5BlkBoundaryInformat_t flashSeqChan5BlkBoundaryInformat;               ///< 0xF200E0AA : Flash Sequencer Channel 5 Block Boundary Information
    FlashSeqChan6BlkBoundaryInformat_t flashSeqChan6BlkBoundaryInformat;               ///< 0xF200E0AC : Flash Sequencer Channel 6 Block Boundary Information
    FlashSeqChan7BlkBoundaryInformat_t flashSeqChan7BlkBoundaryInformat;               ///< 0xF200E0AE : Flash Sequencer Channel 7 Block Boundary Information
    uint8_t rsvd_RF200E0B0[16];                                                        ///< 0xF200E0B0 - 0xF200E0BF : Reserved.
    uint16_t CH_STR;                                                                   ///< 0xF200E0C0 : Flash Sequencer Status Port 2
    FlashSeqStatPort3_t flashSeqStatPort3;                                             ///< 0xF200E0C2 : Flash Sequencer Status Port 3
    uint8_t rsvd_RF200E0C4[12];                                                        ///< 0xF200E0C4 - 0xF200E0CF : Reserved.
    EfFifoClkCtrlPort_t efFifoClkCtrlPort;                                             ///< 0xF200E0D0 : EF FIFO Clock Control Port
    uint8_t rsvd_RF200E0D2[46];                                                        ///< 0xF200E0D2 - 0xF200E0FF : Reserved.
    FlashSeqChan0ToChan3Lun_t flashSeqChan0ToChan3Lun;                                 ///< 0xF200E100 : Flash Sequencer Channel 0 to Channel 3 LUN Information
    FlashSeqChan4ToChan7Lun_t flashSeqChan4ToChan7Lun;                                 ///< 0xF200E102 : Flash Sequencer Channel 4 to Channel 7 LUN Information
    uint8_t rsvd_RF200E104[4];                                                         ///< 0xF200E104 - 0xF200E107 : Reserved.
    AllCeEnCtrl_t allCeEnCtrl;                                                         ///< 0xF200E108 : All CE Enable Control
    uint8_t rsvd_RF200E10A[246];                                                       ///< 0xF200E10A - 0xF200E1FF : Reserved.
    QsGateCtrl1ForChan0And_t qsGateCtrl1ForChan0And;                                   ///< 0xF200E200 : QS Gate Control 1 for Channel 0 and Channel 1
    QsGateCtrl2ForChan2And_t qsGateCtrl2ForChan2And;                                   ///< 0xF200E202 : QS Gate Control 2 for Channel 2 and Channel 3
    QsGateCtrl3ForChan4And_t qsGateCtrl3ForChan4And;                                   ///< 0xF200E204 : QS Gate Control 3 for Channel 4 and Channel 5
    QsGateCtrl4ForChan6And_t qsGateCtrl4ForChan6And;                                   ///< 0xF200E206 : QS Gate Control 4 for Channel 6 and Channel 7
    uint8_t rsvd_RF200E208[8];                                                         ///< 0xF200E208 - 0xF200E20F : Reserved.
    DriveSelForChan0_t driveSelForChan0;                                               ///< 0xF200E210 : Drive Select for Channel 0
    DriveSelForChan1_t driveSelForChan1;                                               ///< 0xF200E212 : Drive Select for Channel 1
    DriveSelForChan2_t driveSelForChan2;                                               ///< 0xF200E214 : Drive Select for Channel 2
    DriveSelForChan3_t driveSelForChan3;                                               ///< 0xF200E216 : Drive Select for Channel 3
    DriveSelForChan4_t driveSelForChan4;                                               ///< 0xF200E218 : Drive Select for Channel 4
    DriveSelForChan5_t driveSelForChan5;                                               ///< 0xF200E21A : Drive Select for Channel 5
    DriveSelForChan6_t driveSelForChan6;                                               ///< 0xF200E21C : Drive Select for Channel 6
    DriveSelForChan7_t driveSelForChan7;                                               ///< 0xF200E21E : Drive Select for Channel 7
#if (_SOC_REV_ >= 0x0210)
    uint8_t rsvd_RF200E220[16];                                                        ///< 0xF200E220 - 0xF200E22F : Reserved.
#else
    DriveSelForChan8_t driveSelForChan8;                                               ///< 0xF200E220 : Drive Select for Channel 8
    DriveSelForChan9_t driveSelForChan9;                                               ///< 0xF200E222 : Drive Select for Channel 9
    DriveSelForChan10_t driveSelForChan10;                                             ///< 0xF200E224 : Drive Select for Channel 10
    DriveSelForChan11_t driveSelForChan11;                                             ///< 0xF200E226 : Drive Select for Channel 11
    DriveSelForChan12_t driveSelForChan12;                                             ///< 0xF200E228 : Drive Select for Channel 12
    DriveSelForChan13_t driveSelForChan13;                                             ///< 0xF200E22A : Drive Select for Channel 13
    DriveSelForChan14_t driveSelForChan14;                                             ///< 0xF200E22C : Drive Select for Channel 14
    DriveSelForChan15_t driveSelForChan15;                                             ///< 0xF200E22E : Drive Select for Channel 15
#endif
    ZpZnCtrl1ForChan0_t zpZnCtrl1ForChan0;                                             ///< 0xF200E230 : ZP ZN Control 1 for Channel 0
    ZpZnCtrl2ForChan0_t zpZnCtrl2ForChan0;                                             ///< 0xF200E232 : ZP ZN Control 2 for Channel 0
    ZpZnCtrl1ForChan1_t zpZnCtrl1ForChan1;                                             ///< 0xF200E234 : ZP ZN Control 1 for Channel 1
    ZpZnCtrl2ForChan1_t zpZnCtrl2ForChan1;                                             ///< 0xF200E236 : ZP ZN Control 2 for Channel 1
    ZpZnCtrl1ForChan2_t zpZnCtrl1ForChan2;                                             ///< 0xF200E238 : ZP ZN Control 1 for Channel 2
    ZpZnCtrl2ForChan2_t zpZnCtrl2ForChan2;                                             ///< 0xF200E23A : ZP ZN Control 2 for Channel 2
    ZpZnCtrl1ForChan3_t zpZnCtrl1ForChan3;                                             ///< 0xF200E23C : ZP ZN Control 1 for Channel 3
    ZpZnCtrl2ForChan3_t zpZnCtrl2ForChan3;                                             ///< 0xF200E23E : ZP ZN Control 2 for Channel 3
    ZpZnCtrl1ForChan4_t zpZnCtrl1ForChan4;                                             ///< 0xF200E240 : ZP ZN Control 1 for Channel 4
    ZpZnCtrl2ForChan4_t zpZnCtrl2ForChan4;                                             ///< 0xF200E242 : ZP ZN Control 2 for Channel 4
    ZpZnCtrl1ForChan5_t zpZnCtrl1ForChan5;                                             ///< 0xF200E244 : ZP ZN Control 1 for Channel 5
    ZpZnCtrl2ForChan5_t zpZnCtrl2ForChan5;                                             ///< 0xF200E246 : ZP ZN Control 2 for Channel 5
    ZpZnCtrl1ForChan6_t zpZnCtrl1ForChan6;                                             ///< 0xF200E248 : ZP ZN Control 1 for Channel 6
    ZpZnCtrl2ForChan6_t zpZnCtrl2ForChan6;                                             ///< 0xF200E24A : ZP ZN Control 2 for Channel 6
    ZpZnCtrl1ForChan7_t zpZnCtrl1ForChan7;                                             ///< 0xF200E24C : ZP ZN Control 1 for Channel 7
    ZpZnCtrl2ForChan7_t zpZnCtrl2ForChan7;                                             ///< 0xF200E24E : ZP ZN Control 2 for Channel 7
    uint8_t rsvd_RF200E250[32];                                                        ///< 0xF200E250 - 0xF200E26F : Reserved.
    DataSubPhyZdCtrl_t dataSubPhyZdCtrl;                                               ///< 0xF200E270 : Data Sub PHY ZD Control
    CmdSubPhyZdCtrl_t cmdSubPhyZdCtrl;                                                 ///< 0xF200E272 : Command Sub PHY ZD Control
    RebSubPhyZdCtrl_t rebSubPhyZdCtrl;                                                 ///< 0xF200E274 : REB Sub PHY ZD Control
    WebSubPhyZdCtrl_t webSubPhyZdCtrl;                                                 ///< 0xF200E276 : WEB Sub PHY ZD Control
    uint8_t rsvd_RF200E278[4];                                                         ///< 0xF200E278 - 0xF200E27B : Reserved.
    CalPadCtrl_t calPadCtrl;                                                           ///< 0xF200E27C : Calibration Pad Control
    uint8_t rsvd_RF200E27E[2];                                                         ///< 0xF200E27E - 0xF200E27F : Reserved.
    DllCtrl1ForChan0_t dllCtrl1ForChan0;                                               ///< 0xF200E280 : DLL Control 1 for Channel 0
    DllCtrl2ForChan0_t dllCtrl2ForChan0;                                               ///< 0xF200E282 : DLL Control 2 for Channel 0
    DllCtrl1ForChan1_t dllCtrl1ForChan1;                                               ///< 0xF200E284 : DLL Control 1 for Channel 1
    DllCtrl2ForChan1_t dllCtrl2ForChan1;                                               ///< 0xF200E286 : DLL Control 2 for Channel 1
    DllCtrl1ForChan2_t dllCtrl1ForChan2;                                               ///< 0xF200E288 : DLL Control 1 for Channel 2
    DllCtrl2ForChan2_t dllCtrl2ForChan2;                                               ///< 0xF200E28A : DLL Control 2 for Channel 2
    DllCtrl1ForChan3_t dllCtrl1ForChan3;                                               ///< 0xF200E28C : DLL Control 1 for Channel 3
    DllCtrl2ForChan3_t dllCtrl2ForChan3;                                               ///< 0xF200E28E : DLL Control 2 for Channel 3
    DllCtrl1ForChan4_t dllCtrl1ForChan4;                                               ///< 0xF200E290 : DLL Control 1 for Channel 4
    DllCtrl2ForChan4_t dllCtrl2ForChan4;                                               ///< 0xF200E292 : DLL Control 2 for Channel 4
    DllCtrl1ForChan5_t dllCtrl1ForChan5;                                               ///< 0xF200E294 : DLL Control 1 for Channel 5
    DllCtrl2ForChan5_t dllCtrl2ForChan5;                                               ///< 0xF200E296 : DLL Control 2 for Channel 5
    DllCtrl1ForChan6_t dllCtrl1ForChan6;                                               ///< 0xF200E298 : DLL Control 1 for Channel 6
    DllCtrl2ForChan6_t dllCtrl2ForChan6;                                               ///< 0xF200E29A : DLL Control 2 for Channel 6
    DllCtrl1ForChan7_t dllCtrl1ForChan7;                                               ///< 0xF200E29C : DLL Control 1 for Channel 7
    DllCtrl2ForChan7_t dllCtrl2ForChan7;                                               ///< 0xF200E29E : DLL Control 2 for Channel 7
    uint8_t rsvd_RF200E2A0[32];                                                        ///< 0xF200E2A0 - 0xF200E2BF : Reserved.
    DllMonitorCh0_t dllMonitorCh0;                                                     ///< 0xF200E2C0 : DLL Monitor CH0
    DllMonitorCh1_t dllMonitorCh1;                                                     ///< 0xF200E2C2 : DLL Monitor CH1
    DllMonitorCh2_t dllMonitorCh2;                                                     ///< 0xF200E2C4 : DLL Monitor CH2
    DllMonitorCh3_t dllMonitorCh3;                                                     ///< 0xF200E2C6 : DLL Monitor CH3
    DllMonitorCh4_t dllMonitorCh4;                                                     ///< 0xF200E2C8 : DLL Monitor CH4
    DllMonitorCh5_t dllMonitorCh5;                                                     ///< 0xF200E2CA : DLL Monitor CH5
    DllMonitorCh6_t dllMonitorCh6;                                                     ///< 0xF200E2CC : DLL Monitor CH6
    DllMonitorCh7_t dllMonitorCh7;                                                     ///< 0xF200E2CE : DLL Monitor CH7
    uint8_t rsvd_RF200E2D0[48];                                                        ///< 0xF200E2D0 - 0xF200E2FF : Reserved.
    PadPowerDnCtrl_t padPowerDnCtrl;                                                   ///< 0xF200E300 : Pad Power-Down Control
    PadPullUpAndPullDnCtrl_t padPullUpAndPullDnCtrl;                                   ///< 0xF200E302 : Pad Pull-Up and Pull-Down Control
    VrefRcvTypeSelectio_t vrefRcvTypeSelectio;                                         ///< 0xF200E304 : VREF RCV Type Selection
    uint8_t rsvd_RF200E306[30];                                                        ///< 0xF200E306 - 0xF200E323 : Reserved.
    DataSubPhyOdtSetting1_t dataSubPhyOdtSetting1;                                     ///< 0xF200E324 : Data Sub PHY ODT Setting 1
    uint8_t rsvd_RF200E328[4];                                                         ///< 0xF200E328 - 0xF200E32B : Reserved.
    CmdSubPhyOdtSetting_t cmdSubPhyOdtSetting;                                         ///< 0xF200E32C : Command Sub PHY ODT Setting
    DqsGateCtrlForChan0To3_t dqsGateCtrlForChan0To3;                                   ///< 0xF200E32E : DQS Gate Control for Channel 0 to 3
    DqsGateCtrlForChan4To7_t dqsGateCtrlForChan4To7;                                   ///< 0xF200E330 : DQS Gate Control for Channel 4 to 7
    uint8_t rsvd_RF200E332[78];                                                        ///< 0xF200E332 - 0xF200E37F : Reserved.
    NandPhyRdSequenceCtrl_t nandPhyRdSequenceCtrl;                                     ///< 0xF200E380 : NAND PHY Read Sequence Control
    NandPhyCfg1_t nandPhyCfg1;                                                         ///< 0xF200E382 : NAND PHY Configuration 1
    Ddr2WarmUpCycleCtrl_t ddr2WarmUpCycleCtrl;                                         ///< 0xF200E384 : DDR2 Warm-Up Cycle Control
    FlashInterfacCfg2_t flashInterfacCfg2;                                             ///< 0xF200E386 : Flash Interface Configuration 2
    FlashInterfacCfg3_t flashInterfacCfg3;                                             ///< 0xF200E388 : Flash Interface Configuration 3
    uint8_t rsvd_RF200E38A[6];                                                         ///< 0xF200E38A - 0xF200E38F : Reserved.
    NandPhyInitCtrl_t nandPhyInitCtrl;                                                 ///< 0xF200E390 : NAND PHY Initial Control
    HiddenCfg1_t hiddenCfg1;                                                           ///< 0xF200E392 : Hidden Configuration 1
    uint8_t rsvd_RF200E394[4];                                                         ///< 0xF200E394 - 0xF200E397 : Reserved.
    CmdMemAccess_t cmdMemAccess;                                                       ///< 0xF200E398 : Command Memory Access
    CmdMemAddr_t cmdMemAddr;                                                           ///< 0xF200E39A : Command Memory Address
    uint16_t CM_DATA1;                                                                 ///< 0xF200E39C : Command Memory Data Port 1
    CmdMemDataPort2_t cmdMemDataPort2;                                                 ///< 0xF200E39E : Command Memory Data Port 2
    FlexibleModeDqTbl1_t flexibleModeDqTbl1;                                           ///< 0xF200E3A0 : Flexible Mode DQ Table 1
    FlexibleModeDqTbl2_t flexibleModeDqTbl2;                                           ///< 0xF200E3A2 : Flexible Mode DQ Table 2
    FlexibleModeDqTbl3_t flexibleModeDqTbl3;                                           ///< 0xF200E3A4 : Flexible Mode DQ Table 3
    FlexibleModeDqTbl4_t flexibleModeDqTbl4;                                           ///< 0xF200E3A6 : Flexible Mode DQ Table 4
    FlexibleModeDqTbl5_t flexibleModeDqTbl5;                                           ///< 0xF200E3A8 : Flexible Mode DQ Table 5
    FlexibleModeDqTbl6_t flexibleModeDqTbl6;                                           ///< 0xF200E3AA : Flexible Mode DQ Table 6
    FlexibleModeDqTbl7_t flexibleModeDqTbl7;                                           ///< 0xF200E3AC : Flexible Mode DQ Table 7
    FlexibleModeDqTbl8_t flexibleModeDqTbl8;                                           ///< 0xF200E3AE : Flexible Mode DQ Table 8
#if (_SOC_REV_ >= 0x0210)
    uint8_t rsvd_RF200E3B0[18];                                                        ///< 0xF200E3B0 - 0xF200E3C1 : Reserved.
#else
    DctSramAccessCfg_t dctSramAccessCfg;                                               ///< 0xF200E3B0 : DCT SRAM Access Configuration
    DctSramAddrPort_t dctSramAddrPort;                                                 ///< 0xF200E3B2 : DCT SRAM Address Port
    uint16_t DCT_DATA_PORT;                                                            ///< 0xF200E3B4 : DCT SRAM Data Port
    uint8_t rsvd_RF200E3B6[2];                                                         ///< 0xF200E3B6 - 0xF200E3B7 : Reserved.
    uint16_t DCT_DPP_ERR;                                                              ///< 0xF200E3B8 : DCT SRAM DPP Status
    uint8_t rsvd_RF200E3BA[2];                                                         ///< 0xF200E3BA - 0xF200E3BB : Reserved.
    DctSramLrm0CtrlForChan0Through_t dctSramLrm0CtrlForChan0Through;                   ///< 0xF200E3BC : DCT SRAM LRM0 Control for Channel 0 through 7
    uint8_t rsvd_RF200E3BE[2];                                                         ///< 0xF200E3BE - 0xF200E3BF : Reserved.
    DctSramLrm1CtrlForChan8Through_t dctSramLrm1CtrlForChan8Through;                   ///< 0xF200E3C0 : DCT SRAM LRM1 Control for Channel 8 through 15
#endif
    HprjCfg_t hprjCfg;                                                                 ///< 0xF200E3C2 : HPRJ Configuration
    RdStatMsk0_t rdStatMsk0;                                                           ///< 0xF200E3C4 : Read Status Mask 0
    RdStatMsk1_t rdStatMsk1;                                                           ///< 0xF200E3C6 : Read Status Mask 1
    NandTimeOutStat_t nandTimeOutStat;                                                 ///< 0xF200E3C8 : NAND Time-Out Status
    SuspendPollingAndMultiVolumeEn_t suspendPollingAndMultiVolumeEn;                   ///< 0xF200E3CA : Suspend Polling and Multi-Volume Enable
    VolumeSelCtrl_t volumeSelCtrl;                                                     ///< 0xF200E3CC : Volume Select Control
    EraseSuspendCmdPort_t eraseSuspendCmdPort;                                         ///< 0xF200E3CE : Erase Suspend Command Port
    ProgSuspendCmdPort_t progSuspendCmdPort;                                           ///< 0xF200E3D0 : Program Suspend Command Port
    SuspendTmCtrl_t suspendTmCtrl;                                                     ///< 0xF200E3D2 : Suspend Timing Control
    FlashSeqStatByteCtrl6_t flashSeqStatByteCtrl6;                                     ///< 0xF200E3D4 : Flash Sequencer Status Byte Control 6
} NandFlashIf_t;
COMPILE_ASSERT(offsetof(NandFlashIf_t,flashSeqStatByteCtrl6)==0xF200E3D4-0xF200E000,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile Fct_t rFct;                          ///< 0xF200BA00-0xF200BA28 : Fct_t MODULE

//-----------------------------------------------------------------------------
//  Exported register reference (Non volatile type for optimization)
//-----------------------------------------------------------------------------
extern volatile NandFlashIf_t rNandFlashIf;          ///< 0xF200E000-0xF200E800 : NandFlashIf_t MODULE
extern Fct_t mFct;                          ///< 0xF200BA00-0xF200BA28 : Fct_t MODULE
extern NandFlashIf_t mNandFlashIf;          ///< 0xF200E000-0xF200E800 : NandFlashIf_t MODULE
