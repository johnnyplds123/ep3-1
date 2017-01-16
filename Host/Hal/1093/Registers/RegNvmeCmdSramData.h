#pragma once
// ============================================================================
//
//     Copyright (c) Marvell Corporation 2000-2014  -  All rights reserved
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
//! @brief NVMe Command SRAM Data
//!
//! Doc Version: INTERNAL
//! Doc Date: 6/3/2014
//=============================================================================
/// @cond HW_REG

#include <stddef.h>

/// @brief NVMe FW IO CMD (MODE 0)
#pragma anon_unions // unnamed union
typedef struct
{
    ///<    DW0
    union                       
    {
        uint32_t DW0;       ///< all flags
        struct
        {
            uint32_t OPC        :8;     ///<BIT [7:0] Opcode
            uint32_t FUSE       :2;     ///<BIT [9:8] Fused Operation (00=normal, 01=fused 1st command, 10=fused last command, 11=reserved)
            uint32_t PRIORITY   :2;     ///<BIT [11:10] The priority service class
            uint32_t reserved12 :4;     ///<BIT [15:12] Reserved
            uint32_t CID        :9;     ///<BIT [24:16] Command Identifier
            uint32_t reserved25 :7;     ///<BIT [31:25] Reserved
        };
    };

    ///<    DW1
    uint32_t NSID;                      ///<    Namespace Identifier

    ///<    DW2 & DW3
    uint32_t SLBA_LO_CDW10;             ///<    SLBA Low DW or CDW10
    uint32_t SLBA_HI_CDW11;             ///<    SLBA High DW or CDW11

    ///<    DW4
    union
    {
        uint32_t all;
        struct
        {
            uint32_t NLB        :16;    ///<BIT [15:0] Number of Logical Block
            uint32_t DATM       : 8;    ///<BIT [23:16] Dataset Management
            uint32_t reserv24   : 2;    ///<BIT [25:24] Reserved
            uint32_t PRINFO     : 4;    ///<BIT [29:26] Protection Information Field
            uint32_t FUA        : 1;    ///<BIT [30] Force Unit Access
            uint32_t LR         : 1;    ///<BIT [31] Limited Retry
        } b;
    } DW4;
} NvmeFwIoCmdMode0_t;
COMPILE_ASSERT(sizeof(NvmeFwIoCmdMode0_t)==(5*4), "NvmeFwIoCmd_t size shall be 0x10");

/// @brief NVMe FW IO CMD (MODE 1)
typedef struct
{
    ///< DW0
    uint32_t SQID       :10;    ///<BIT [9:0] Submission Queue Identifier
    uint32_t FID        : 6;    ///<BIT [15:10] Function Identifier
    uint32_t reserv16   :16;    ///<BIT [31:16] Reserved

    ///< DW1~16
    uint32_t IODW[16];          ///<    NVMe I/O Command Space (IODW 0~15)
} NvmeFwIoCmdMode1_t;
COMPILE_ASSERT(sizeof(NvmeFwIoCmdMode1_t)==(17*4), "NvmeFwIoCmd_t size shall be 0x44");

/// @brief NVMe Command SRAM FW IO CMD
typedef union
{
    NvmeFwIoCmdMode0_t m0;
    NvmeFwIoCmdMode1_t m1;
} NvmeFwIoCmd_t;

/// @brief NVMe Command SRAM ADM CMD DW0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        ///< DW0
        uint32_t SQID       :10;    ///<BIT [9:0] Submission Queue Identifier
        uint32_t FID        : 6;    ///<BIT [15:10] Function Identifier
        uint32_t reserv16   :16;    ///<BIT [31:16] Reserved
    } b;
} NvmeFwAdmCmdDw0_t;
COMPILE_ASSERT(sizeof(NvmeFwAdmCmdDw0_t)==(4), "NvmeFwAdmCmdDw0_t size shall be 0x4");

/// @brief NVMe Command SRAM ADM CMD
typedef struct
{
    ///< DW0
    NvmeFwAdmCmdDw0_t DW0;      ///< NVMe FW Admin Command DW0

    ///< DW1~16
    uint32_t ADW[16];           ///< NVMe Admin Command Space (ADW 0~15)
} NvmeFwAdmCmd_t;
COMPILE_ASSERT(sizeof(NvmeFwAdmCmd_t)==(17*4), "NvmeFwAdmCmd_t size shall be 0x44");

/// @brief NVMe Command Ring Buffer
typedef struct
{
    ///< DW0
    uint32_t DW0;

    ///< DW1
    uint32_t SQID       :10;    ///<BIT [9:0] Submission Queue Identifier
    uint32_t FID        : 6;    ///<BIT [15:10] Function Identifier
    uint32_t reserv16   :16;    ///<BIT [31:16] Reserved

    ///<    DW2~17
    uint32_t CDW[16];           ///<    NVMe Command Space (CDW 0~15)
} NvmeCommandRingBuffer_t;
COMPILE_ASSERT(sizeof(NvmeCommandRingBuffer_t)==(18*4), "NvmeCommandRingBuffer_t size shall be 0x48");

/// @brief NVMe HW IO CMD DW0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        // DW0
        uint32_t AB         : 1;    ///<BIT [0] Abort, this command is aborted by the firmware.
        uint32_t RWMODE     : 1;    ///<BIT [1] Read/Write operation, 0 = Read, 1 = Write.
        uint32_t BWDONE     : 1;    ///<BIT [2] Backend Write Done
        uint32_t CD         : 1;    ///<BIT [3] CQ Posting Disable (0 = Enable HW auto CQ posting)
        uint32_t IT         : 1;    ///<BIT [4] Interrupt Trigger, An interrupt shall be triggered after the command is completed.
        uint32_t reverv5_10 : 6;    ///<BIT [10:5] Reserved
        uint32_t JU         : 1;    ///<BIT [11] Write SSD design (1=an old SSD, 0=a new SSD design)
        uint32_t RM         : 1;    ///<BIT [12] Read Transfer Method 
                                    ///<         0 = Use Hardware Link List, 1 = Use SSD
        uint32_t WM         : 1;    ///<BIT [13] Write Transfer Method
                                    ///<         0 = Use Hardware Link List, 1 = Use SSD
        uint32_t WC         : 1;    ///<BIT [14] Write Cache Enable
        uint32_t WS         : 1;    ///<BIT [15] Write Streaming Control Enable
        uint32_t LBAF       : 4;    ///<BIT [19:16] LBA Format, The LBA size and metadata size that the namespace has been formatted with.
        uint32_t MS         : 1;    ///<BIT [20] Metadata Settings, The transfer method of the metadata
                                    ///<         0 = Transferred as part of a seperate buffer
                                    ///<         1 = Transferred as part of an extended data LBA
        uint32_t PI         : 3;    ///<BIT [23:21] Protection Information (0 = No protection, 1 = protection type 1, ...)
        uint32_t PIL        : 1;    ///<BIT [24:] Protection Information Location
                                    ///<          0 = Transferred as the last eight bytes of metadata
                                    ///<          1 = Transferred as the first eight bytes of metadata
        uint32_t reverv25   : 7;    ///<BIT [31:25] Reserved
    } b;
} NvmeHwIoCmdDw0_t;

/// @brief NVMe HW IO CMD DW1
#pragma anon_unions // unnamed union
typedef union
{
    uint32_t all;                   ///< all flags
    struct
    {
        uint32_t POINTER    :16;    ///<BIT [15:00] Pointer
                                    ///<    If Hardware Link List disabled
                                    ///<      - The first SSD entry of the link list
                                    ///<      - The head pointer of the ring buffer
                                    ///<    Else if Hardware Link List enabled
                                    ///<      - Bit[15:08] = Head Pointer
                                    ///<      - Bit[07:00] = Tail Pointer
        uint32_t OFFSET     : 8;    ///<BIT [23:16] The Sector Offset. The data transfer starts at the location described
                                    ///             by the first SSD entry of the link list plus this sector offset.
                                    ///             This field is only valid when RM/WM and SSD Jump(JU) are set..
        uint32_t SEGMENT    : 8;    ///<BIT [31:24] Segment Number(0~255) that identifies the streaming channel
    };
} NvmeHwIoCmdDw1_t;

/// @brief NVMe HW IO CMD
typedef struct
{
    // DW0
    NvmeHwIoCmdDw0_t    dw0;

    // DW1
    NvmeHwIoCmdDw1_t    dw1;

    // DW2
    uint32_t SQID       :10;    ///<BIT [9:0] Submission Queue Identifier
    uint32_t FID        : 6;    ///<BIT [15:10] Function Identifier
    uint32_t CID        :16;    ///<BIT [31:16] Command Identifier

    // DW3 & DW4
    uint32_t PRP1_LO;           ///< PRP Entry 1 Low DW
    uint32_t PRP1_HI;           ///< PRP Entry 1 High DW

    // DW5 & DW6
    uint32_t PRP2_LO;           ///< PRP Entry 2 Low DW
    uint32_t PRP2_HI;           ///< PRP Entry 2 High DW

    // DW7 & DW8
    uint32_t MPTR_LO;           ///< Metadata Pointer Low DW
    uint32_t MPTR_HI;           ///< Metadata Pointer High DW

    // DW9 & DW10
    uint32_t SLBA_LO_CDW10;     ///<    SLBA Low DW or CDW10
    uint32_t SLBA_HI_CDW11;     ///<    SLBA High DW or CDW11

    // DW11
    uint32_t NRB        :17;    ///<BIT [16:0] Number of Remaining Block (NLB is a 0's based value but NRB is not)
    uint32_t ERROR      : 1;    ///<BIT [17] Error Bit
    uint32_t LCEI       : 4;    ///<BIT [21:18] Large Command Extension Index
    uint32_t LLBACNT    : 4;    ///<BIT [25:22] Last AU LBA Count
    uint32_t PRINFO     : 4;    ///<BIT [29:26] Protection Information Field
    uint32_t NB         : 1;    ///<BIT [30] New Block (0 = Not newly Create, 1 = Newly Create)
    uint32_t PNB        : 1;    ///<BIT [31] PRP New Block

    // DW12
    uint32_t EXPECTED_ILBRT;    ///< Expected Initial Logical Block Reference Tag

    // DW13
    uint32_t ELBATM     :16;    ///<BIT [15:00] Expected Logical Block Application Tag Mask
    uint32_t ELBAT      :16;    ///<BIT [31:16] Expected Logical Block Application Tag
    
    // DW14
    uint32_t BC1        : 6;    ///<BIT [05:00] Block Count 1, The number of required block
    uint32_t reserv6    : 6;    ///<BIT [11:06] Reserved
    uint32_t LS1        : 1;    ///<BIT [12] Last Set 1, Whether the PRP table is the last set.
    uint32_t PF1        : 1;    ///<BIT [13] PRP Fetched 1, Whether the PRP is fetched.
    uint32_t PP1        : 1;    ///<BIT [14] Partial PRP 1, The first PRP is a partial PRP in the current PRP set.
    uint32_t SGL        : 1;    ///<BIT [15] Scatter Gather List, Whether SGL is used.
    uint32_t BC2        : 6;    ///<BIT [21:16] Block Count 2, The number of required block
    uint32_t reserv22   : 6;    ///<BIT [27:22] Reserved
    uint32_t LS2        : 1;    ///<BIT [28] Last Set 2, Whether the PRP table is the last set.
    uint32_t PF2        : 1;    ///<BIT [29] PRP Fetched 2, Whether the PRP is fetched.
    uint32_t PP2        : 1;    ///<BIT [30] Partial PRP 2, The first PRP is a partial PRP in the current PRP set.
    uint32_t PBS        : 1;    ///<BIT [31] PBS, Which set should be used first.
                                ///<          0 = Set 1 is ahead of set 2.
                                ///<          1 = Set 2 is ahead of set 1.

    // DW15
    uint32_t LAUI       :16;    ///<BIT [15:00] Last AU Index, the absolute index of the last AU
    uint32_t NSID       : 4;    ///<BIT [19:16] Namespace Identifier
    uint32_t reserv20   :12;    ///<BIT [31:20] Reserved
} NvmeHwIoCmd_t;
COMPILE_ASSERT(sizeof(NvmeHwIoCmd_t)==(16*4), "NvmeHwIoCmd_t size shall be 0x40");

/// @brief NVMe HW ADM CMD
typedef struct
{
    // DW0
    uint32_t AB         : 1;    ///<BIT [0] Abort, this command is aborted by the firmware.
    uint32_t RWMODE     : 1;    ///<BIT [1] Read/Write operation, 0 = Read, 1 = Write.
    uint32_t reverv2_10 : 9;    ///<BIT [10:2] Reserved
    uint32_t JU         : 1;    ///<BIT [11] Write SSD design (1=an old SSD, 0=a new SSD design)
    uint32_t RM         : 1;    ///<BIT [12] Read Transfer Method 
                                ///<         0 = Reserved, 1 = Use SSD
    uint32_t WM         : 1;    ///<BIT [13] Write Transfer Method
                                ///<         0 = Use Use Ring Buffer, 1 = Use SSD
    uint32_t reverv14   :18;    ///<BIT [31:14] Reserved

    // DW1
    uint32_t POINTER    :16;    ///<BIT [15:00] Buffer Head Pointer
                                ///<    - The first SSD entry of the link list
                                ///<    - The head pointer of the ring buffer
    uint32_t BSIZE      : 6;    ///<BIT [21:16] Buffer Size, the size of the ring buffer structure
    uint32_t reserv22   :10;    ///<BIT [31:22] Reserved

    // DW2
    uint32_t reserved_dw2;

    // DW3 & DW4
    uint32_t PRP1_LO;          ///< PRP Entry 1 Low DW
    uint32_t PRP1_HI;          ///< PRP Entry 1 High DW

    // DW5 & DW6
    uint32_t PRP2_LO;          ///< PRP Entry 2 Low DW
    uint32_t PRP2_HI;          ///< PRP Entry 2 High DW
    
    // DW7
    uint32_t NDW;               ///< Number of Dwords that the HW shall transfer.

} NvmeHwAdmCmd_t;
COMPILE_ASSERT(sizeof(NvmeHwAdmCmd_t)==(8*4), "NvmeHwAdmCmd_t size shall be 0x40");

//-----------------------------------------------------------------------------
//  Exported register reference (Volatile type)
//-----------------------------------------------------------------------------
extern volatile NvmeFwIoCmd_t rNvmeFwIoCmd[];   ///< 0xF5400000 : NvmeFwIoCmd_t MODULE
extern volatile NvmeFwAdmCmd_t rNvmeFwAdmCmd[]; ///< 0xF5400800 : NvmeFwAdmCmd_t MODULE
extern volatile NvmeHwIoCmd_t rNvmeHwIoCmd[];   ///< 0xF5401000 : NvmeHwIoCmd_t MODULE
extern volatile NvmeHwAdmCmd_t rNvmeHwAdmCmd[]; ///< 0xF5409000 : NvmeHwIoCmd_t MODULE
