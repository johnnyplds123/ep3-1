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
//! @brief Hardware Re-Read Registers
//! 
//! Doc Version: STANDARD
//! Doc Date: 2/19/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  Hardware Re-Read Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xF200FC00 : Re-Read Control Register 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RRD_SOFT_RST        : 1;         ///<BIT [0] RRD Soft Reset.
        uint16_t MAX_RRD_NUM         : 3;         ///<BIT [3:1] Maximum Re-Read Number.
        uint16_t RRD_DF_QLF_HW_FULL  : 1;         ///<BIT [4] RRD Descriptor FIFO QLF Hardware Full.
        uint16_t RRD_USE_HPRJ        : 1;         ///<BIT [5] RRD Use High Priority Job.
        uint16_t RF200FC00_15_6_RSVD :10;         ///<BIT [15:6] Reserved.
    } b;    
} ReRdCtrlRegister1_t;

///  @brief 0xF200FC02 : HWRRD Type1/2 2-Bit Decoding AU Number Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RRD_MAX_T12_2B_NUM     : 2;         ///<BIT [1:0] HWRRD Maximum Type1/2 2-Bit Decoding AU Number.
        uint16_t RF200FC02_7_2_RSVD     : 6;         ///<BIT [7:2] Reserved.
        uint16_t RRD_MAX_T12_2B_NUM_RDY : 1;         ///<BIT [8] HWRRD Maximum Type1/2 2-Bit Decoding AU Number Update Ready.
        uint16_t RRD_MAX_T12_2B_NUM_ERR : 1;         ///<BIT [9] HWRRD Maximum Type1/2 2-Bit Decoding AU Number Update Error.
        uint16_t RF200FC02_15_10_RSVD   : 6;         ///<BIT [15:10] Reserved.
    } b;    
} HwrrdType122BitDecodingAuNum_t;

///  @brief 0xF200FC06 : Re-Read Status FIFO Counter
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RRD_SF_CNT          : 7;         ///<BIT [6:0] RRD Status FIFO Count.
        uint16_t RF200FC06_15_7_RSVD : 9;         ///<BIT [15:7] Reserved.
    } b;    
} ReRdStatFifoCntr_t;

///  @brief 0xF200FC18 : Re-Read SRAM Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RRD_WTC             : 2;         ///<BIT [1:0] RRD Write Timing Control (WTC) Setting in SRAM.
        uint16_t RRD_RTC             : 2;         ///<BIT [3:2] RRD Read Timing Control (RTC) Setting in SRAM.
        uint16_t RRD_PDWN            : 1;         ///<BIT [4] RRD Power-Down Setting in SRAM.
        uint16_t RRD_PDLVMC          : 1;         ///<BIT [5] RRD Power-Down LVMC Setting in SRAM.
        uint16_t RRD_PDFVSSM         : 1;         ///<BIT [6] RRD Power-Down FVSSM Setting in SRAM.
        uint16_t RF200FC18_15_7_RSVD : 9;         ///<BIT [15:7] Reserved.
    } b;    
} ReRdSramCtrl_t;

///  @brief 0xF200FC1A : Re-Read SRAM Status
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RRD_SRAM_DPP_ERR    : 1;         ///<BIT [0] RRD DPP Error Status in SRAM.
        uint16_t RF200FC1A_15_1_RSVD :15;         ///<BIT [15:1] Reserved.
    } b;    
} ReRdSramStat_t;

///  @brief 0xF200FCB0 : Re-Read Descriptor Template 1_0 Number
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RRD_DES_TEMPL0_NUM : 8;         ///<BIT [7:0] HWRRD Descriptor Template 0 Number.
        uint16_t RRD_DES_TEMPL1_NUM : 8;         ///<BIT [15:8] HWRRD Descriptor Template 1 Number.
    } b;    
} ReRdDescriptTemplate10Num_t;

///  @brief 0xF200FCEE : Re-Read Descriptor Template 63_62 Number
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RRD_DES_TEMPL62_NUM : 8;         ///<BIT [7:0] HWRRD Descriptor Template 62 Number.
        uint16_t RRD_DES_TEMPL63_NUM : 8;         ///<BIT [15:8] HWRRD Descriptor Template 63 Number.
    } b;    
} ReRdDescriptTemplate6362Num_t;

///  @brief 0xF200FCF0 : HW Re-Read Descriptor Template 2-Bit Decoding Control 0
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DES_TEMPL0_2B  : 1;         ///<BIT [0] Descriptor Template 0 2-Bit.
        uint16_t DES_TEMPL1_2B  : 1;         ///<BIT [1] Descriptor Template 1 2-Bit.
        uint16_t DES_TEMPL2_2B  : 1;         ///<BIT [2] Descriptor Template 2 2-Bit.
        uint16_t DES_TEMPL3_2B  : 1;         ///<BIT [3] Descriptor Template 3 2-Bit.
        uint16_t DES_TEMPL4_2B  : 1;         ///<BIT [4] Descriptor Template 4 2-Bit.
        uint16_t DES_TEMPL5_2B  : 1;         ///<BIT [5] Descriptor Template 5 2-Bit.
        uint16_t DES_TEMPL6_2B  : 1;         ///<BIT [6] Descriptor Template 6 2-Bit.
        uint16_t DES_TEMPL7_2B  : 1;         ///<BIT [7] Descriptor Template 7 2-Bit.
        uint16_t DES_TEMPL8_2B  : 1;         ///<BIT [8] Descriptor Template 8 2-Bit.
        uint16_t DES_TEMPL9_2B  : 1;         ///<BIT [9] Descriptor Template 9 2-Bit.
        uint16_t DES_TEMPL10_2B : 1;         ///<BIT [10] Descriptor Template 10 2-Bit.
        uint16_t DES_TEMPL11_2B : 1;         ///<BIT [11] Descriptor Template 11 2-Bit.
        uint16_t DES_TEMPL12_2B : 1;         ///<BIT [12] Descriptor Template 12 2-Bit.
        uint16_t DES_TEMPL13_2B : 1;         ///<BIT [13] Descriptor Template 13 2-Bit.
        uint16_t DES_TEMPL14_2B : 1;         ///<BIT [14] Descriptor Template 14 2-Bit.
        uint16_t DES_TEMPL15_2B : 1;         ///<BIT [15] Descriptor Template 15 2-Bit.
    } b;    
} HwReRdDescriptTemplate2BitDecoding00_t;

///  @brief 0xF200FCF2 : HW Re-Read Descriptor Template 2-Bit Decoding Control 1
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DES_TEMPL16_2B : 1;         ///<BIT [0] Descriptor Template 16 2-Bit.
        uint16_t DES_TEMPL17_2B : 1;         ///<BIT [1] Descriptor Template 17 2-Bit.
        uint16_t DES_TEMPL18_2B : 1;         ///<BIT [2] Descriptor Template 18 2-Bit.
        uint16_t DES_TEMPL19_2B : 1;         ///<BIT [3] Descriptor Template 19 2-Bit.
        uint16_t DES_TEMPL20_2B : 1;         ///<BIT [4] Descriptor Template 20 2-Bit.
        uint16_t DES_TEMPL21_2B : 1;         ///<BIT [5] Descriptor Template 21 2-Bit.
        uint16_t DES_TEMPL22_2B : 1;         ///<BIT [6] Descriptor Template 22 2-Bit.
        uint16_t DES_TEMPL23_2B : 1;         ///<BIT [7] Descriptor Template 23 2-Bit.
        uint16_t DES_TEMPL24_2B : 1;         ///<BIT [8] Descriptor Template 24 2-Bit.
        uint16_t DES_TEMPL25_2B : 1;         ///<BIT [9] Descriptor Template 25 2-Bit.
        uint16_t DES_TEMPL26_2B : 1;         ///<BIT [10] Descriptor Template 26 2-Bit.
        uint16_t DES_TEMPL27_2B : 1;         ///<BIT [11] Descriptor Template 27 2-Bit.
        uint16_t DES_TEMPL28_2B : 1;         ///<BIT [12] Descriptor Template 28 2-Bit.
        uint16_t DES_TEMPL29_2B : 1;         ///<BIT [13] Descriptor Template 29 2-Bit.
        uint16_t DES_TEMPL30_2B : 1;         ///<BIT [14] Descriptor Template 30 2-Bit.
        uint16_t DES_TEMPL31_2B : 1;         ///<BIT [15] Descriptor Template 31 2-Bit.
    } b;    
} HwReRdDescriptTemplate2BitDecoding11_t;

///  @brief 0xF200FCF4 : HW Re-Read Descriptor Template 2-Bit Decoding Control 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DES_TEMPL32_2B : 1;         ///<BIT [0] Descriptor Template 32 2-Bit.
        uint16_t DES_TEMPL33_2B : 1;         ///<BIT [1] Descriptor Template 33 2-Bit.
        uint16_t DES_TEMPL34_2B : 1;         ///<BIT [2] Descriptor Template 34 2-Bit.
        uint16_t DES_TEMPL35_2B : 1;         ///<BIT [3] Descriptor Template 35 2-Bit.
        uint16_t DES_TEMPL36_2B : 1;         ///<BIT [4] Descriptor Template 36 2-Bit.
        uint16_t DES_TEMPL37_2B : 1;         ///<BIT [5] Descriptor Template 37 2-Bit.
        uint16_t DES_TEMPL38_2B : 1;         ///<BIT [6] Descriptor Template 38 2-Bit.
        uint16_t DES_TEMPL39_2B : 1;         ///<BIT [7] Descriptor Template 39 2-Bit.
        uint16_t DES_TEMPL40_2B : 1;         ///<BIT [8] Descriptor Template 40 2-Bit.
        uint16_t DES_TEMPL41_2B : 1;         ///<BIT [9] Descriptor Template 41 2-Bit.
        uint16_t DES_TEMPL42_2B : 1;         ///<BIT [10] Descriptor Template 42 2-Bit.
        uint16_t DES_TEMPL43_2B : 1;         ///<BIT [11] Descriptor Template 43 2-Bit.
        uint16_t DES_TEMPL44_2B : 1;         ///<BIT [12] Descriptor Template 44 2-Bit.
        uint16_t DES_TEMPL45_2B : 1;         ///<BIT [13] Descriptor Template 45 2-Bit.
        uint16_t DES_TEMPL46_2B : 1;         ///<BIT [14] Descriptor Template 46 2-Bit.
        uint16_t DES_TEMPL47_2B : 1;         ///<BIT [15] Descriptor Template 47 2-Bit.
    } b;    
} HwReRdDescriptTemplate2BitDecoding22_t;

///  @brief 0xF200FCF6 : HW Re-Read Descriptor Template 2-Bit Decoding Control 3
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t DES_TEMPL48_2B : 1;         ///<BIT [0] Descriptor Template 48 2-Bit.
        uint16_t DES_TEMPL49_2B : 1;         ///<BIT [1] Descriptor Template 49 2-Bit.
        uint16_t DES_TEMPL50_2B : 1;         ///<BIT [2] Descriptor Template 50 2-Bit.
        uint16_t DES_TEMPL51_2B : 1;         ///<BIT [3] Descriptor Template 51 2-Bit.
        uint16_t DES_TEMPL52_2B : 1;         ///<BIT [4] Descriptor Template 52 2-Bit.
        uint16_t DES_TEMPL53_2B : 1;         ///<BIT [5] Descriptor Template 53 2-Bit.
        uint16_t DES_TEMPL54_2B : 1;         ///<BIT [6] Descriptor Template 54 2-Bit.
        uint16_t DES_TEMPL55_2B : 1;         ///<BIT [7] Descriptor Template 55 2-Bit.
        uint16_t DES_TEMPL56_2B : 1;         ///<BIT [8] Descriptor Template 56 2-Bit.
        uint16_t DES_TEMPL57_2B : 1;         ///<BIT [9] Descriptor Template 57 2-Bit.
        uint16_t DES_TEMPL58_2B : 1;         ///<BIT [10] Descriptor Template 58 2-Bit.
        uint16_t DES_TEMPL59_2B : 1;         ///<BIT [11] Descriptor Template 59 2-Bit.
        uint16_t DES_TEMPL60_2B : 1;         ///<BIT [12] Descriptor Template 60 2-Bit.
        uint16_t DES_TEMPL61_2B : 1;         ///<BIT [13] Descriptor Template 61 2-Bit.
        uint16_t DES_TEMPL62_2B : 1;         ///<BIT [14] Descriptor Template 62 2-Bit.
        uint16_t DES_TEMPL63_2B : 1;         ///<BIT [15] Descriptor Template 63 2-Bit.
    } b;    
} HwReRdDescriptTemplate2BitDecoding33_t;

///  @brief 0xF200FD00 : Re-Read CPU Access SRAM Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RRD_CPU_ACC_SRAM_EN  : 1;         ///<BIT [0] RRD CPU Access SRAM Enable.
        uint16_t RRD_CPU_ACC_SRAM_WR  : 1;         ///<BIT [1] RRD CPU Access SRAM Write.
        uint16_t RRD_CPU_ACC_SRAM_RD  : 1;         ///<BIT [2] RRD CPU Access SRAM Read.
        uint16_t RF200FD00_7_3_RSVD   : 5;         ///<BIT [7:3] Reserved.
        uint16_t RRD_BYTE_INDX        : 3;         ///<BIT [10:8] RRD Byte Index.
        uint16_t RF200FD00_15_11_RSVD : 5;         ///<BIT [15:11] Reserved.
    } b;    
} ReRdCpuAccessSramCtrl_t;

///  @brief 0xF200FD02 : Re-Read CPU Access SRAM Address
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RRD_CPU_ACC_SRAM_ADDR : 7;         ///<BIT [6:0] RRD CPU Access SRAM Address.
        uint16_t RF200FD02_15_7_RSVD   : 9;         ///<BIT [15:7] Reserved.
    } b;    
} ReRdCpuAccessSramAddr_t;

///  @brief 0xF200FD08 : Re-Read MBIST Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RRD_MBIST_STATUS_CLK      : 1;         ///<BIT [0] RRD MBIST Status Clock.
        uint16_t RRD_MBIST_MEM_EXT_NWRT    : 1;         ///<BIT [1] RRD MBIST Memory External NWRT.
        uint16_t RRD_MBIST_MEM_EXT_NCE     : 1;         ///<BIT [2] RRD MBIST Memory External NCE.
        uint16_t RRD_MBIST_MEM_EXT_CTRL    : 1;         ///<BIT [3] RRD MBIST Memory External Control.
        uint16_t RRD_MBIST_MEM_EXT_CLK_DIS : 1;         ///<BIT [4] RRD MBIST Memory External Clock Disable.
        uint16_t RRD_MBIST_BISTMODE        : 1;         ///<BIT [5] RRD MBIST BIST Mode.
        uint16_t RRD_MBIST_ROWCOLSW        : 1;         ///<BIT [6] RRD MBIST Row Column Switch.
        uint16_t RRD_MBIST_RUNSTOP         : 1;         ///<BIT [7] RRD MBIST Run Stop.
        uint16_t RRD_MBIST_STS_IN          : 1;         ///<BIT [8] RRD MBIST Status In.
        uint16_t RRD_MBIST_STS_EN          : 1;         ///<BIT [9] RRD MBIST Status Enable.
        uint16_t RRD_MBIST_RST_B           : 1;         ///<BIT [10] RRD MBIST Reset B.
        uint16_t RF200FD08_15_11_RSVD      : 5;         ///<BIT [15:11] Reserved.
    } b;    
} ReRdMbistCtrl_t;

///  @brief 0xF200FD0A : Re-Read MBIST Status
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RRD_MBIST_FINISH    : 1;         ///<BIT [0] RRD MBIST Finish.
        uint16_t RRD_MBIST_ERR       : 1;         ///<BIT [1] RRD MBIST Error.
        uint16_t RRD_MBIST_STS_OUT   : 1;         ///<BIT [2] RRD MBIST Status Out.
        uint16_t RRD_MBIST_FAIL      : 1;         ///<BIT [3] RRD MBIST Fail.
        uint16_t RF200FD0A_15_4_RSVD :12;         ///<BIT [15:4] Reserved.
    } b;    
} ReRdMbistStat_t;

///  @brief 0xF200FD10 : Re-Read Debug Control
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RRD_MON_SEL         : 8;         ///<BIT [7:0] RRD Monitor Select.
        uint16_t RF200FD10_15_8_RSVD : 8;         ///<BIT [15:8] Reserved.
    } b;    
} ReRdDbgCtrl_t;


/// @brief 0xF200FC00-0xF200FD20 : HwReread_t MODULE
typedef struct
{
    ReRdCtrlRegister1_t reRdCtrlRegister1;                                            ///< 0xF200FC00 : Re-Read Control Register 1
    HwrrdType122BitDecodingAuNum_t hwrrdType122BitDecodingAuNum;                      ///< 0xF200FC02 : HWRRD Type1/2 2-Bit Decoding AU Number Control
    uint8_t rsvd_RF200FC04[2];                                                        ///< 0xF200FC04 - 0xF200FC05 : Reserved.
    ReRdStatFifoCntr_t reRdStatFifoCntr;                                              ///< 0xF200FC06 : Re-Read Status FIFO Counter
    uint16_t RRD_SF_ADDR_ST_15_0;                                                     ///< 0xF200FC08 : Re-Read Status FIFO Start Address Low
    uint16_t RRD_SF_ADDR_ST_31_16;                                                    ///< 0xF200FC0A : Re-Read Status FIFO Start Address High
    uint16_t RRD_SF_ADDR_END_15_0;                                                    ///< 0xF200FC0C : Re-Read Status FIFO End Address Low
    uint16_t RRD_SF_ADDR_END_31_16;                                                   ///< 0xF200FC0E : Re-Read Status FIFO End Address High
    uint16_t SEQ_ADEC_LUT_ST_ADDR_15_0;                                               ///< 0xF200FC10 : Sequencer AU Decoder LUT Start Address Low
    uint16_t SEQ_ADEC_LUT_ST_ADDR_31_16;                                              ///< 0xF200FC12 : Sequencer AU Decoder LUT Start Address High
    uint16_t ROW_ADDR_INC_15_0;                                                       ///< 0xF200FC14 : Multi-Plane Row Address Increment Low
    uint16_t ROW_ADDR_INC_31_16;                                                      ///< 0xF200FC16 : Multi-Plane Row Address Increment High
    ReRdSramCtrl_t reRdSramCtrl;                                                      ///< 0xF200FC18 : Re-Read SRAM Control
    ReRdSramStat_t reRdSramStat;                                                      ///< 0xF200FC1A : Re-Read SRAM Status
    uint32_t RRD_DF_ADDR_ST;                                                          ///< 0xF200FC1C : Re-Read Descriptor FIFO Start Address
    uint32_t RRD_DF_ADDR_END;                                                         ///< 0xF200FC20 : Re-Read Descriptor FIFO End Address
    uint16_t RRD_DF_SIZE;                                                             ///< 0xF200FC24 : Re-Read Descriptor FIFO Size
    uint16_t RRD_DF_DS_CNT;                                                           ///< 0xF200FC26 : Re-Read Descriptor Count
    uint16_t RRD_DF_W_ADDR;                                                           ///< 0xF200FC28 : Re-Read Descriptor FIFO Write Address Offset
    uint16_t RRD_DF_R_ADDR;                                                           ///< 0xF200FC2A : Re-Read Descriptor FIFO Read Address Offset
    uint8_t rsvd_RF200FC2C[4];                                                        ///< 0xF200FC2C - 0xF200FC2F : Reserved.
    uint16_t RRD_DES_TEMPL0_PTR;                                                      ///< 0xF200FC30 : Re-Read Descriptor Template 0 Pointer
    uint8_t rsvd_RF200FC32[124];                                                      ///< 0xF200FC32 - 0xF200FCAD : Reserved.
    uint16_t RRD_DES_TEMPL63_PTR;                                                     ///< 0xF200FCAE : Re-Read Descriptor Template 63 Pointer
    ReRdDescriptTemplate10Num_t reRdDescriptTemplate10Num;                            ///< 0xF200FCB0 : Re-Read Descriptor Template 1_0 Number
    uint8_t rsvd_RF200FCB2[60];                                                       ///< 0xF200FCB2 - 0xF200FCED : Reserved.
    ReRdDescriptTemplate6362Num_t reRdDescriptTemplate6362Num;                        ///< 0xF200FCEE : Re-Read Descriptor Template 63_62 Number
    HwReRdDescriptTemplate2BitDecoding00_t hwReRdDescriptTemplate2BitDecoding0_0;     ///< 0xF200FCF0 : HW Re-Read Descriptor Template 2-Bit Decoding Control 0
    HwReRdDescriptTemplate2BitDecoding11_t hwReRdDescriptTemplate2BitDecoding1_1;     ///< 0xF200FCF2 : HW Re-Read Descriptor Template 2-Bit Decoding Control 1
    HwReRdDescriptTemplate2BitDecoding22_t hwReRdDescriptTemplate2BitDecoding2_2;     ///< 0xF200FCF4 : HW Re-Read Descriptor Template 2-Bit Decoding Control 2
    HwReRdDescriptTemplate2BitDecoding33_t hwReRdDescriptTemplate2BitDecoding3_3;     ///< 0xF200FCF6 : HW Re-Read Descriptor Template 2-Bit Decoding Control 3
    uint8_t rsvd_RF200FCF8[8];                                                        ///< 0xF200FCF8 - 0xF200FCFF : Reserved.
    ReRdCpuAccessSramCtrl_t reRdCpuAccessSramCtrl;                                    ///< 0xF200FD00 : Re-Read CPU Access SRAM Control
    ReRdCpuAccessSramAddr_t reRdCpuAccessSramAddr;                                    ///< 0xF200FD02 : Re-Read CPU Access SRAM Address
    uint16_t RRD_CPU_ACC_SRAM_DIN;                                                    ///< 0xF200FD04 : Re-Read CPU Access SRAM Input Data
    uint16_t RRD_CPU_ACC_SRAM_DOUT;                                                   ///< 0xF200FD06 : Re-Read CPU Access SRAM Output Data
    ReRdMbistCtrl_t reRdMbistCtrl;                                                    ///< 0xF200FD08 : Re-Read MBIST Control
    ReRdMbistStat_t reRdMbistStat;                                                    ///< 0xF200FD0A : Re-Read MBIST Status
    uint8_t rsvd_RF200FD0C[4];                                                        ///< 0xF200FD0C - 0xF200FD0F : Reserved.
    ReRdDbgCtrl_t reRdDbgCtrl;                                                        ///< 0xF200FD10 : Re-Read Debug Control
    uint16_t RRD_MON_BUS;                                                             ///< 0xF200FD12 : Re-Read Monitor Bus
} HwReread_t;
COMPILE_ASSERT(offsetof(HwReread_t,RRD_MON_BUS)==0xF200FD12-0xF200FC00,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile HwReread_t rHwReread;                ///< 0xF200FC00-0xF200FD20 : HwReread_t MODULE

//-----------------------------------------------------------------------------
//  Exported register reference (Non volatile type for optimization)
//-----------------------------------------------------------------------------
extern HwReread_t mHwReread;                ///< 0xF200FC00-0xF200FD20 : HwReread_t MODULE
