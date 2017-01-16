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
//! @brief NVMe Packet Manager Registers
//! 
//! Doc Version: STANDARD
//! Doc Date: 03/18/2015
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

//-----------------------------------------------------------------------------
//  NVMe Packet Manager Registers structure definitions
//-----------------------------------------------------------------------------
///  @brief 0xF5013000 : PCIe DAB1 Write Configuration
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DAB1_ID              : 4;         ///<BIT [3:0] DAB1 ID-Reserved.
        uint32_t DAB1_LEN             : 4;         ///<BIT [7:4] DAB1 Length Before Write.
        uint32_t DAB1_LOCK            : 2;         ///<BIT [9:8] DAB1 Write Lock.
        uint32_t DAB1_BUF_WRACS       : 1;         ///<BIT [10] DAB1 Bufferable Write Access.
        uint32_t RF5013000_15_11_RSVD : 5;         ///<BIT [15:11] Reserved.
        uint32_t DAB1_PROT            : 3;         ///<BIT [18:16] DAB1 Protection Type.
        uint32_t DAB1_MAXRSP          : 6;         ///<BIT [24:19] DAB1 Maximum Outstanding Write Response.
        uint32_t RF5013000_31_25_RSVD : 7;         ///<BIT [31:25] Reserved.
    } b;    
} PcieDab1WrCfg_t;

///  @brief 0xF5013004 : PCIe DAB1 Read Configuration
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t DAB1_CR_ID           : 4;         ///<BIT [3:0] DAB1 ID.
        uint32_t DAB1_CR_LEN          : 4;         ///<BIT [7:4] DAB1 Length Read.
        uint32_t DAB1_CR_LOCK         : 2;         ///<BIT [9:8] DAB1 Read Lock.
        uint32_t RF5013004_15_10_RSVD : 6;         ///<BIT [15:10] Reserved.
        uint32_t DAB1_CR_PROT         : 3;         ///<BIT [18:16] DAB1 Read Protection Type.
        uint32_t DAB1_MAXREQ          : 6;         ///<BIT [24:19] DAB1 Maximum Outstanding Read Request.
        uint32_t RF5013004_31_25_RSVD : 7;         ///<BIT [31:25] Reserved.
    } b;    
} PcieDab1RdCfg_t;

///  @brief 0xF5013008 : PCIe AWUSER Field Configuration
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t AWUSER_TYPE          : 4;         ///<BIT [3:0] PCIe AWUSER PCIe Type.
        uint32_t AWUSER_IDO           : 1;         ///<BIT [4] PCIe AWUSER PCIe ID Based Ordering.
        uint32_t AWUSER_EP            : 1;         ///<BIT [5] PCIe AWUSER PCIe Endpoint Poison.
        uint32_t AWUSER_ATTR          : 2;         ///<BIT [7:6] PCIe AWUSER PCIe Attribute.
        uint32_t AWUSER_TC            : 3;         ///<BIT [10:8] PCIe AWUSER PCIe Traffic Class.
        uint32_t RF5013008_31_11_RSVD :21;         ///<BIT [31:11] Reserved.
    } b;    
} PcieAwuserFieldCfg_t;

///  @brief 0xF501300C : PCIe ARUSER Field Configuration
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ARUSER_TYPE          : 4;         ///<BIT [3:0] PCIe ARUSER PCIe Type.
        uint32_t ARUSER_IDO           : 1;         ///<BIT [4] PCIe ARUSER PCIe ID Based Ordering.
        uint32_t ARUSER_EP            : 1;         ///<BIT [5] PCIe ARUSER PCIe Endpoint Poison.
        uint32_t ARUSER_ATTR          : 2;         ///<BIT [7:6] PCIe ARUSER PCIe Attribute.
        uint32_t ARUSER_TC            : 3;         ///<BIT [10:8] PCIe ARUSER PCIe Traffic Class.
        uint32_t RF501300C_31_11_RSVD :21;         ///<BIT [31:11] Reserved.
    } b;    
} PcieAruserFieldCfg_t;

///  @brief 0xF5013010 : Packet Manager Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RD_REQ_ARB_MD       : 2;         ///<BIT [1:0] Packet Manager Read Request Arbitration Mode.
        uint32_t LMT_EN              : 1;         ///<BIT [2] Read Request Limit Enable.
        uint32_t DAB_PAR_EN          : 1;         ///<BIT [3] PCIe DAB Data Parity Check Enable.
        uint32_t RF5013010_31_4_RSVD :28;         ///<BIT [31:4] Reserved.
    } b;    
} PacketManagerCtrl_t;

///  @brief 0xF5013014 : Packet Manager Read Requester Port 0 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t REQ_LMT_CNT_P0       : 5;         ///<BIT [4:0] Outstanding Request Limit Count Value for Port 0.
        uint32_t RF5013014_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t PRI_VAL_P0           : 3;         ///<BIT [10:8] Priority Value for Port 0.
        uint32_t RF5013014_31_11_RSVD :21;         ///<BIT [31:11] Reserved.
    } b;    
} PacketManagerRdRequestePort0Ctrl_t;

///  @brief 0xF5013018 : Packet Manager Read Requester Port 1 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t REQ_LMT_CNT_P1       : 5;         ///<BIT [4:0] Outstanding Request Limit Count Value for Port 1.
        uint32_t RF5013018_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t PRI_VAL_P1           : 3;         ///<BIT [10:8] Priority Value for Port 1.
        uint32_t RF5013018_31_11_RSVD :21;         ///<BIT [31:11] Reserved.
    } b;    
} PacketManagerRdRequestePort1Ctrl_t;

///  @brief 0xF5013024 : Packet Manager Read Requester Port 4 Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t REQ_LMT_CNT_P4       : 5;         ///<BIT [4:0] Outstanding Request Limit Count Value for Port 4.
        uint32_t RF5013024_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t PRI_VAL_P4           : 3;         ///<BIT [10:8] Priority Value for Port 4.
        uint32_t RF5013024_31_11_RSVD :21;         ///<BIT [31:11] Reserved.
    } b;    
} PacketManagerRdRequestePort4Ctrl_t;

///  @brief 0xF501302C : Packet Manager Current Read Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PKTM_REQ_PTR         : 5;         ///<BIT [4:0] Current Packet Manager Read Request Pointer.
        uint32_t RF501302C_7_5_RSVD   : 3;         ///<BIT [7:5] Reserved.
        uint32_t PKTM_DATA_PTR        : 5;         ///<BIT [12:8] Current Packet Manager Read Data Pointer.
        uint32_t RF501302C_15_13_RSVD : 3;         ///<BIT [15:13] Reserved.
        uint32_t PKTM_REQ_SM          : 4;         ///<BIT [19:16] Packet Manager Read Request State Machine.
        uint32_t RF501302C_23_20_RSVD : 4;         ///<BIT [23:20] Reserved.
        uint32_t PKTM_DATA_SM         : 4;         ///<BIT [27:24] Packet Manager Read Data State Machine.
        uint32_t RF501302C_31_28_RSVD : 4;         ///<BIT [31:28] Reserved.
    } b;    
} PacketManagerCurRdStat_t;

///  @brief 0xF5013030 : Packet Manager AXI Monitor Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t SEL                  :16;         ///<BIT [15:0] Packet Manager PCIe AXI Monitor Signal Select.
        uint32_t RF5013030_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} PacketManagerAxiMonitorCtrl_t;

///  @brief 0xF5013034 : Packet Manager Monitor Signal Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t VAL                  :16;         ///<BIT [15:0] Packet Manager PCIe AXI Monitor Signal Value.
        uint32_t RF5013034_31_16_RSVD :16;         ///<BIT [31:16] Reserved.
    } b;    
} PacketManagerMonitorSgnStat_t;

///  @brief 0xF5013040 : Packet Manager AXI Abort Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t ABT                 : 1;         ///<BIT [0] Packet Manager PCIe AXI Abort.
        uint32_t RF5013040_31_1_RSVD :31;         ///<BIT [31:1] Reserved.
    } b;    
} PacketManagerAxiAbortCtrl_t;

///  @brief 0xF5013044 : Packet Manager AXI Abort Status
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t RD_DONE             : 1;         ///<BIT [0] PCIe AXI Read Abort Done.
        uint32_t WR_DONE             : 1;         ///<BIT [1] PCIe AXI Write Abort Done.
        uint32_t RF5013044_31_2_RSVD :30;         ///<BIT [31:2] Reserved.
    } b;
} PacketManagerAxiAbortStatus_t;

///  @brief 0xF5013080 : Packet Manager Error FIFO Count
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t PKTM_ERR_CNTR       : 4;         ///<BIT [3:0] Packet Manager Error FIFO Status Counter.
        uint32_t RF5013080_31_4_RSVD :28;         ///<BIT [31:4] Reserved.
    } b;    
} PacketManagerErrFifoCnt_t;

///  @brief 0xF5013090 : Packet Manager Error Control
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t FORCE_PKTM_RD_PERR  : 2;         ///<BIT [1:0] Force Packet Manager SRAM Read Parity Error.
        uint32_t RF5013090_3_2_RSVD  : 2;         ///<BIT [3:2] Reserved.
        uint32_t IGNORE_RD_PERR      : 1;         ///<BIT [4] Ignore Packet Manager Read 1-Bit Parity Error.
        uint32_t RF5013090_31_5_RSVD :27;         ///<BIT [31:5] Reserved.
    } b;    
} PacketManagerErrCtrl_t;

/// @brief 0xF5013000-0xF5013180 : NvmePktMngr_t MODULE
typedef struct
{
    PcieDab1WrCfg_t pcieDab1WrCfg;                                           ///< 0xF5013000 : PCIe DAB1 Write Configuration
    PcieDab1RdCfg_t pcieDab1RdCfg;                                           ///< 0xF5013004 : PCIe DAB1 Read Configuration
    PcieAwuserFieldCfg_t pcieAwuserFieldCfg;                                 ///< 0xF5013008 : PCIe AWUSER Field Configuration
    PcieAruserFieldCfg_t pcieAruserFieldCfg;                                 ///< 0xF501300C : PCIe ARUSER Field Configuration
    PacketManagerCtrl_t packetManagerCtrl;                                   ///< 0xF5013010 : Packet Manager Control
    PacketManagerRdRequestePort0Ctrl_t packetManagerRdRequestePort0Ctrl;     ///< 0xF5013014 : Packet Manager Read Requester Port 0 Control
    PacketManagerRdRequestePort1Ctrl_t packetManagerRdRequestePort1Ctrl;     ///< 0xF5013018 : Packet Manager Read Requester Port 1 Control
    uint8_t rsvd_RF501301C[8];                                               ///< 0xF501301C - 0xF5013023 : Reserved.
    PacketManagerRdRequestePort4Ctrl_t packetManagerRdRequestePort4Ctrl;     ///< 0xF5013024 : Packet Manager Read Requester Port 4 Control
    uint32_t PKTM_RD_REQ_PEND;                                               ///< 0xF5013028 : Packet Manager Pending Read Request Vector
    PacketManagerCurRdStat_t packetManagerCurRdStat;                         ///< 0xF501302C : Packet Manager Current Read Status
    PacketManagerAxiMonitorCtrl_t packetManagerAxiMonitorCtrl;               ///< 0xF5013030 : Packet Manager AXI Monitor Control
    PacketManagerMonitorSgnStat_t packetManagerMonitorSgnStat;               ///< 0xF5013034 : Packet Manager Monitor Signal Status
    uint8_t rsvd_RF5013038[8];                                               ///< 0xF5013038 - 0xF501303F : Reserved.
    PacketManagerAxiAbortCtrl_t packetManagerAxiAbortCtrl;                   ///< 0xF5013040 : Packet Manager AXI Abort Control
    PacketManagerAxiAbortStatus_t packetManagerAxiAbortStatus;               ///< 0xF5013044 : Packet Manager AXI Abort Status
    uint8_t rsvd_RF5013048[56];                                              ///< 0xF5013048 - 0xF501307F : Reserved.
    PacketManagerErrFifoCnt_t packetManagerErrFifoCnt;                       ///< 0xF5013080 : Packet Manager Error FIFO Count

    uint32_t PKTM_ERR_FIFO_DW[2];                                            ///< 0xF5013084 - 0xF5013088 : Packet Manager Error FIFO Dword 0/1
    //    uint32_t PKTM_ERR_FIFO_DW0;                                        ///< 0xF5013084 : Packet Manager Error FIFO Dword 0
    //    uint32_t PKTM_ERR_FIFO_DW1;                                        ///< 0xF5013088 : Packet Manager Error FIFO Dword 1
    uint8_t rsvd_RF501308C[4];                                               ///< 0xF501308C - 0xF501308F : Reserved.
    PacketManagerErrCtrl_t packetManagerErrCtrl;                             ///< 0xF5013090 : Packet Manager Error Control
    uint8_t rsvd_RF5013094[108];                                             ///< 0xF5013094 - 0xF50130FF : Reserved.
    uint32_t PKTM_AXI_DBG0;                                                  ///< 0xF5013100 : PCIe AXI Debug 0
    uint32_t PKTM_AXI_DBG1;                                                  ///< 0xF5013104 : PCIe AXI Debug 1
    uint32_t PKTM_AXI_DBG2;                                                  ///< 0xF5013108 : PCIe AXI Debug 2
    uint32_t PKTM_AXI_DBG3;                                                  ///< 0xF501310C : PCIe AXI Debug 3
    uint32_t PKTM_AXI_DBG4;                                                  ///< 0xF5013110 : PCIe AXI Debug 4
    uint32_t PKTM_AXI_DBG5;                                                  ///< 0xF5013114 : PCIe AXI Debug 5
    uint32_t PKTM_AXI_DBG6;                                                  ///< 0xF5013118 : PCIe AXI Debug 6
    uint32_t PKTM_AXI_DBG7;                                                  ///< 0xF501311C : PCIe AXI Debug 7
    uint32_t PKTM_AXI_DBG8;                                                  ///< 0xF5013120 : PCIe AXI Debug 8
    uint32_t PKTM_AXI_DBG9;                                                  ///< 0xF5013124 : PCIe AXI Debug 9
    uint32_t PKTM_AXI_DBG10;                                                 ///< 0xF5013128 : PCIe AXI Debug 10
    uint32_t PKTM_AXI_DBG11;                                                 ///< 0xF501312C : PCIe AXI Debug 11
    uint32_t PKTM_AXI_DBG12;                                                 ///< 0xF5013130 : PCIe AXI Debug 12
    uint32_t PKTM_AXI_DBG13;                                                 ///< 0xF5013134 : PCIe AXI Debug 13
    uint32_t PKTM_AXI_DBG14;                                                 ///< 0xF5013138 : PCIe AXI Debug 14
    uint32_t PKTM_AXI_DBG15;                                                 ///< 0xF501313C : PCIe AXI Debug 15
    uint32_t PKTM_AXI_DBG16;                                                 ///< 0xF5013140 : PCIe AXI Debug 16
    uint32_t PKTM_AXI_DBG17;                                                 ///< 0xF5013144 : PCIe AXI Debug 17
    uint32_t PKTM_AXI_DBG18;                                                 ///< 0xF5013148 : PCIe AXI Debug 18
    uint32_t PKTM_AXI_DBG19;                                                 ///< 0xF501314C : PCIe AXI Debug 19
    uint32_t PKTM_AXI_DBG20;                                                 ///< 0xF5013150 : PCIe AXI Debug 20
    uint32_t PKTM_AXI_DBG21;                                                 ///< 0xF5013154 : PCIe AXI Debug 21
    uint32_t PKTM_AXI_DBG22;                                                 ///< 0xF5013158 : PCIe AXI Debug 22
    uint32_t PKTM_AXI_DBG23;                                                 ///< 0xF501315C : PCIe AXI Debug 23
    uint32_t PKTM_AXI_DBG24;                                                 ///< 0xF5013160 : PCIe AXI Debug 24
    uint32_t PKTM_AXI_DBG25;                                                 ///< 0xF5013164 : PCIe AXI Debug 25
    uint32_t PKTM_AXI_DBG26;                                                 ///< 0xF5013168 : PCIe AXI Debug 26
    uint32_t PKTM_AXI_DBG27;                                                 ///< 0xF501316C : PCIe AXI Debug 27
    uint32_t PKTM_AXI_DBG28;                                                 ///< 0xF5013170 : PCIe AXI Debug 28
    uint32_t PKTM_AXI_DBG29;                                                 ///< 0xF5013174 : PCIe AXI Debug 29
    uint32_t PKTM_AXI_DBG30;                                                 ///< 0xF5013178 : PCIe AXI Debug 30
    uint32_t PKTM_AXI_DBG31;                                                 ///< 0xF501317C : PCIe AXI Debug 31
} NvmePktMngr_t;
COMPILE_ASSERT(offsetof(NvmePktMngr_t,PKTM_AXI_DBG31)==0xF501317C-0xF5013000,"check register structure offset");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile NvmePktMngr_t rNvmePktMngr;          ///< 0xF5013000-0xF5013180 : NvmePktMngr_t MODULE
