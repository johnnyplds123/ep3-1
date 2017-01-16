#pragma once
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
//! @brief Descriptor(Regular, Compact, Generic) header
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
/// @ SEQ descriptor format
#define DESC_FMT_REGULAR            0   ///< Regular descriptor format
#define DESC_FMT_COMPACT            1   ///< Compact descriptor format
#define DESC_FMT_GENERIC            2   ///< Generic descriptor format

/// @ SEQ Command type
#define DESC_CMD_READ               0   ///< READ
#define DESC_CMD_PROG               1   ///< PROGRAM
#define DESC_CMD_ERASE              2   ///< ERASE
#define DESC_CMD_READ_STATUS        3   ///< READ_STATUS
#define DESC_CMD_MP_READ            4   ///< Multi-Plane READ
#define DESC_CMD_MP_PROG            5   ///< Multi-Plane PROGRAM
#define DESC_CMD_MP_ERASE           6   ///< Multi-Plane ERASE
#define DESC_CMD_MP_READ_STATUS     7   ///< Multi-Plane READ_STATUS
#define DESC_CMD_MP                 0x4 ///< Multi-Plane operation
#define DESC_CMD_MLUN               0x8 ///< Multi-LUN operation

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

/// @brief SEQ Regular descriptor base(OR1=0)
typedef union
{
    uint32_t dw[6];
    struct
    {
        // dw 0
        uint32_t TRACK_ID        :11;    ///< [10: 0] TRACK ID
        uint32_t DG              :1;     ///< [   11] Dependency Group
        uint32_t MP_NUM          :3;     ///< [14:12] Multiplane Number
        uint32_t OR1             :1;     ///< [   15] Optional Row 1 Enable
        uint32_t CMD_ENH2_0      :3;     ///< [18:16] Command Enhancement [2:0]
        uint32_t AU_NUM          :3;     ///< [21:19] AU Number
        uint32_t DES_RDSR        :2;     ///< [23:22] Read Status Command Type[1:0]
        uint32_t AU_SEG          :4;     ///< [27:24] Buffer Segment Selection
        uint32_t DES_RDSR2       :1;     ///< [   28] Read Status Command Type[2]
        uint32_t TWOBT0          :1;     ///< [   29] Two-bit Type 0
        uint32_t RES             :1;     ///< [   30] Turn the descriptor rescheduling feature on or off
        uint32_t ZERO            :1;     ///< [   31] ECU trigger

        // dw 1
        uint32_t DEV             :4;     ///< [ 3: 0] Device
        uint32_t CH              :4;     ///< [ 7: 4] Channel
        uint32_t DES_TYPE        :4;     ///< [11: 8] DES type (0: READ, 1:PROGRAM, 2:ERASE)
        uint32_t DF              :2;     ///< [13:12] Descriptor Format (0: regular)
        uint32_t CIL             :1;     ///< [   14] Cut-In-Line
        uint32_t RSVD0_DW1       :1;     ///< [   15] Reserved
        uint32_t DV              :1;     ///< [   16] Descriptor Valid(always 1)
        uint32_t TB              :1;     ///< [   17] Two block count enable
        uint32_t BM              :1;     ///< [   18] Busy Time Monitor Enable
        uint32_t RSVD1_DW1       :1;     ///< [   19] Reserved
        uint32_t JU              :1;     ///< [   20] Buffer Segment Jump Enable
        uint32_t ST              :1;     ///< [   21] Streaming Enable
        uint32_t MQ              :1;     ///< [   22] Media Queue Post Enable
        uint32_t EM              :1;     ///< [   23] NAND error Mask
        uint32_t HC              :1;     ///< [   24] Host LBA Comparison for PROGRAM or Insertion for READ
        uint32_t LE              :1;     ///< [   25] AUX extraction (== le)
        uint32_t LC              :1;     ///< [   26] AUX comparison (== lc)
        uint32_t SA              :1;     ///< [   27] samsung addressing (0:old, 1:new)
        uint32_t DC              :1;     ///< [   28] Descriptor Control (0:polling, 1:non-poling)
        uint32_t RR              :1;     ///< [   29] Random Row Address (0:incremental for multiplane, 1: random) - Only Multiplane operation
        uint32_t LD              :1;     ///< [   30] Last descriptor
        uint32_t FD              :1;     ///< [   31] First descriptor

        // dw 2
        union
        {
            struct
            {
                uint32_t HCMD_INDEX_MQ   :8;     ///< [ 7: 0] Host Command Index
                uint32_t HBLK_CNT        :9;     ///< [16: 8] Host Bock Count
                uint32_t HBLK_CTL        :15;    ///< [31:17] Streaming Controller Tag
            };
            struct
            {
                uint32_t HCMD_INDEX_HWLL :9;     ///< [ 8: 0] Host Command Index
                uint32_t PRP_CNT         :7;     ///< [15: 9] Physical Range Page Count
                uint32_t AU_INDEX        :16;    ///< [31:16] Host Block Control
            };
        };

        // dw 3
        uint32_t HSTRM           :2;     ///< [ 1: 0] Host Streaming Select
        uint32_t HP              :2;     ///< [ 3: 2] MQ Port Select
        uint32_t CX              :1;     ///< [    4] Compression Extract Enable
        uint32_t CE              :1;     ///< [    5] Compression Enable
        uint32_t EC              :1;     ///< [    6] ECC Configuration
        uint32_t SE              :1;     ///< [    7] Suspension Enable
        uint32_t ENH5            :1;     ///< [    8] Command Enhancement[5]
        uint32_t AU_FORMAT       :5;     ///< [13: 9] Au Format
        uint32_t CMD_ENH4_3      :2;     ///< [15:14] Command Enhancement [4:3]
        uint32_t START_SSD_PTR   :16;    ///< [31:16] Buffer Linked List Pointer

        // dw 4
        uint16_t START_COL_ADDR;         ///< [15: 0] Data Transfer Start Column Address, starting column within page
        uint16_t XF_COUNT;               ///< [31:16] Data Transfer Count,  total bytes to transfer, including meta + user + ecc

        // dw 5
        uint32_t RAID_ID0        :4;     ///< [ 3: 0] RAID Buffer ID 0
        uint32_t RAID_CMD0       :4;     ///< [ 7: 4] RAID_Command 0
        uint32_t RAID_ID1        :4;     ///< [11: 8] RAID Buffer ID 1
        uint32_t RAID_CMD1       :4;     ///< [15:12] RAID_Command 1
        uint32_t RAID_ID2        :4;     ///< [19:16] RAID Buffer ID 2
        uint32_t RAID_CMD2       :4;     ///< [23:20] RAID_Command 2
        uint32_t RAID_ID3        :4;     ///< [27:24] RAID Buffer ID 3
        uint32_t RAID_CMD3       :4;     ///< [31:28] RAID_Command 3
    } field;
}  SeqRegularDescBase_t;

/// @brief SEQ Regular descriptor base (OR1=1)
typedef union
{
    uint32_t dw[8];
    struct
    {
        // dw 0
        uint32_t TRACK_ID        :11;    ///< [10: 0] TRACK ID
        uint32_t DG              :1;     ///< [   11] Dependency Group
        uint32_t MP_NUM          :3;     ///< [14:12] Multiplane Number
        uint32_t OR1             :1;     ///< [   15] Optional Row 1 Enable
        uint32_t CMD_ENH2_0      :3;     ///< [18:16] Command Enhancement [2:0]
        uint32_t AU_NUM          :3;     ///< [21:19] AU Number
        uint32_t DES_RDSR        :2;     ///< [23:22] Read Status Command Type[1:0]
        uint32_t AU_SEG          :4;     ///< [27:24] Buffer Segment Selection
        uint32_t DES_RDSR2       :1;     ///< [   28] Read Status Command Type[2]
        uint32_t TWOBT0          :1;     ///< [   29] Two-bit Type 0
        uint32_t RES             :1;     ///< [   30] Turn the descriptor rescheduling feature on or off
        uint32_t ZERO            :1;     ///< [   31] ECU trigger

        // dw 1
        uint32_t DEV             :4;     ///< [ 3: 0] Device
        uint32_t CH              :4;     ///< [ 7: 4] Channel
        uint32_t DES_TYPE        :4;     ///< [11: 8] DES type (0: READ, 1:PROGRAM, 2:ERASE)
        uint32_t DF              :2;     ///< [13:12] Descriptor Format (0: regular)
        uint32_t CIL             :1;     ///< [   14] Cut-In-Line
        uint32_t RSVD0_DW1       :1;     ///< [   15] Reserved
        uint32_t DV              :1;     ///< [   16] Descriptor Valid(always 1)
        uint32_t TB              :1;     ///< [   17] Two block count enable
        uint32_t BM              :1;     ///< [   18] Busy Time Monitor Enable
        uint32_t RSVD1_DW1       :1;     ///< [   19] Reserved
        uint32_t JU              :1;     ///< [   20] Buffer Segment Jump Enable
        uint32_t ST              :1;     ///< [   21] Streaming Enable
        uint32_t MQ              :1;     ///< [   22] Media Queue Post Enable
        uint32_t EM              :1;     ///< [   23] NAND error Mask
        uint32_t HC              :1;     ///< [   24] Host LBA Comparison for PROGRAM or Insertion for READ
        uint32_t LE              :1;     ///< [   25] AUX extraction (== le)
        uint32_t LC              :1;     ///< [   26] AUX comparison (== lc)
        uint32_t SA              :1;     ///< [   27] samsung addressing (0:old, 1:new)
        uint32_t DC              :1;     ///< [   28] Descriptor Control (0:polling, 1:non-poling)
        uint32_t RR              :1;     ///< [   29] Random Row Address (0:incremental for multiplane, 1: random) - Only Multiplane operation
        uint32_t LD              :1;     ///< [   30] Last descriptor
        uint32_t FD              :1;     ///< [   31] First descriptor

        // dw 2
        union
        {
            struct
            {
                uint32_t HCMD_INDEX_MQ   :8;     ///< [ 7: 0] Host Command Index
                uint32_t HBLK_CNT        :9;     ///< [16: 8] Host Bock Count
                uint32_t HBLK_CTL        :15;    ///< [31:17] Streaming Controller Tag
            };
            struct
            {
                uint32_t HCMD_INDEX_HWLL :9;     ///< [ 8: 0] Host Command Index
                uint32_t PRP_CNT         :7;     ///< [15: 9] Physical Range Page Count
                uint32_t AU_INDEX        :16;    ///< [31:16] Host Block Control
            };
            uint32_t SET_PARAM;                  ///< [31: 0] Set Parameter
        };

        // dw 3
        uint32_t HSTRM           :2;     ///< [ 1: 0] Host Streaming Select
        uint32_t HP              :2;     ///< [ 3: 2] MQ Port Select
        uint32_t CX              :1;     ///< [    4] Compression Extract Enable
        uint32_t CE              :1;     ///< [    5] Compression Enable
        uint32_t EC              :1;     ///< [    6] ECC Configuration
        uint32_t SE              :1;     ///< [    7] Suspension Enable
        uint32_t ENH5            :1;     ///< [    8] Command Enhancement[5]
        uint32_t AU_FORMAT       :5;     ///< [13: 9] Au Format
        uint32_t CMD_ENH4_3      :2;     ///< [15:14] Command Enhancement [4:3]
        uint32_t START_SSD_PTR   :16;    ///< [31:16] Buffer Linked List Pointer

        // dw 4
        uint32_t LBA7_0          :8;     ///< [ 7: 0]
        uint32_t RSVD0_DW4       :24;    ///< [31: 8] Reserved

        // dw 5
        uint32_t LBA27_8         :20;    ///< [19: 0]
        uint32_t AUCP_SID        :4;     ///< [23:20]
        uint32_t HWLL            :1;     ///< [   24] Hardware Linked List
        uint32_t AEND            :1;     ///< [   25]
        uint32_t AUST            :1;     ///< [   26]
        uint32_t CPEN            :1;     ///< [   27]
        uint32_t RAID_SID        :4;     ///< [31:28] RAID Buffer Sub-ID

        // dw 6
        uint16_t START_COL_ADDR;         ///< [15: 0] Data Transfer Start Column Address, starting column within page
        uint16_t XF_COUNT;               ///< [31:16] Data Transfer Count,  total bytes to transfer, including meta + user + ecc

        // dw 7
        uint32_t RAID_ID0        :4;     ///< [ 3: 0] RAID Buffer ID 0
        uint32_t RAID_CMD0       :4;     ///< [ 7: 4] RAID_Command 0
        uint32_t RAID_ID1        :4;     ///< [11: 8] RAID Buffer ID 1
        uint32_t RAID_CMD1       :4;     ///< [15:12] RAID_Command 1
        uint32_t RAID_ID2        :4;     ///< [19:16] RAID Buffer ID 2
        uint32_t RAID_CMD2       :4;     ///< [23:20] RAID_Command 2
        uint32_t RAID_ID3        :4;     ///< [27:24] RAID Buffer ID 3
        uint32_t RAID_CMD3       :4;     ///< [31:28] RAID_Command 3
    } field;
}  SeqRegularDescBaseOr_t;

/// @brief SEQ Regular descriptor (OR1=0, 1Plane, 2Plane)
typedef struct
{
    SeqRegularDescBase_t base;
    uint32_t START_ROW_ADDR[2];
} SeqRegularDesc_t;

/// @brief SEQ Regular descriptor (OR1=0, 4Plane)
typedef struct
{
    SeqRegularDescBase_t base;
    uint32_t START_ROW_ADDR[4];
} SeqRegularDesc4p_t;

/// @brief SEQ Regular descriptor (OR1=0, 8Plane)
typedef struct
{
    SeqRegularDescBase_t base;
    uint32_t START_ROW_ADDR[8];
} SeqRegularDesc8p_t;

/// @brief SEQ Regular descriptor (OR1=1, 1Plane, 2Plane)
typedef struct
{
    SeqRegularDescBaseOr_t base;
    uint32_t START_ROW_ADDR[2];
} SeqRegularDescOr_t;

/// @brief SEQ Regular descriptor (OR1=1, 4Plane)
typedef struct
{
    SeqRegularDescBaseOr_t base;
    uint32_t START_ROW_ADDR[4];
} SeqRegularDescOr4p_t;

/// @brief SEQ Regular descriptor (OR1=1, 8Plane)
typedef struct
{
    SeqRegularDescBaseOr_t base;
    uint32_t START_ROW_ADDR[8];
} SeqRegularDescOr8p_t;

/// @brief SEQ Compact descriptor Base (OR1=0 & RE=0)
typedef union
{
    uint32_t dw[4];

    struct
    {
        // dw 0
        uint32_t TRACK_ID        :11;    ///< [10: 0] TRACK ID
        uint32_t DG              :1;     ///< [   11] Dependency Group
        uint32_t MP_NUM          :3;     ///< [14:12] Multiplane Number
        uint32_t OR1             :1;     ///< [   15] Optional Row 1 Enable
        uint32_t CMD_ENH2_0      :3;     ///< [18:16] Command Enhancement [2:0]
        uint32_t AU_NUM          :3;     ///< [21:19] AU Number
        uint32_t DES_RDSR        :2;     ///< [23:22] Read Status Command Type
        uint32_t AU_SEG          :4;     ///< [27:24] Buffer Segment Selection
        uint32_t VOL             :1;     ///< [   28] NAND Flash Volume Number
        uint32_t TWOBT0          :1;     ///< [   29] Two-bit Type 0
        uint32_t RES             :1;     ///< [   30] Turn the descriptor rescheduling feature on or off
        uint32_t ZERO            :1;     ///< [   31] ECU trigger

        // dw 1
        uint32_t DEV             :4;     ///< [ 3: 0] Device
        uint32_t CH              :4;     ///< [ 7: 4] Channel
        uint32_t DES_TYPE        :4;     ///< [11: 8] DES type (0: READ, 1:PROGRAM, 2:ERASE)
        uint32_t DF              :2;     ///< [13:12] Descriptor Format (1: compact)
        uint32_t CS              :1;     ///< [   14] Start Colume Address Select
        uint32_t XS              :1;     ///< [   15] Transfer Count Select
        uint32_t DV              :1;     ///< [   16] Descriptor Valid(always 1)
        uint32_t TB              :1;     ///< [   17] Two block count enable
        uint32_t BM              :1;     ///< [   18] Busy Time Monitor Enable
        uint32_t RE              :1;     ///< [   19]
        uint32_t JU              :1;     ///< [   20] Buffer Segment Jump Enable
        uint32_t ST              :1;     ///< [   21] Streaming Enable
        uint32_t MQ              :1;     ///< [   22] Media Queue Post Enable
        uint32_t EM              :1;     ///< [   23] NAND error Mask
        uint32_t HC              :1;     ///< [   24] Host LBA Comparison for PROGRAM or Insertion for READ
        uint32_t LE              :1;     ///< [   25] AUX extraction (== le)
        uint32_t LC              :1;     ///< [   26] AUX comparison (== lc)
        uint32_t SA              :1;     ///< [   27] samsung addressing (0:old, 1:new)
        uint32_t DC              :1;     ///< [   28] Descriptor Control (0:polling, 1:non-poling)
        uint32_t RR              :1;     ///< [   29] Random Row Address (0:incremental for multiplane, 1: random) - Only Multiplane operation
        uint32_t LD              :1;     ///< [   30] Last descriptor
        uint32_t FD              :1;     ///< [   31] First descriptor

        // dw 2
        union
        {
            struct
            {
                uint32_t HCMD_INDEX_MQ   :8;     ///< [ 7: 0] Host Command Index
                uint32_t HBLK_CNT        :9;     ///< [16: 8] Host Bock Count
                uint32_t HBLK_CTL        :15;    ///< [31:17] Streaming Controller Tag
            };
            struct
            {
                uint32_t HCMD_INDEX_HWLL :9;     ///< [ 8: 0] Host Command Index
                uint32_t PRP_CNT         :7;     ///< [15: 9] Physical Range Page Count
                uint32_t AU_INDEX        :16;    ///< [31:16] Host Block Control
            };
        };

        // dw 3
        uint32_t HSTRM           :2;     ///< [ 1: 0] Host Streaming Select
        uint32_t HP              :2;     ///< [ 3: 2] MQ Port Select
        uint32_t CX              :1;     ///< [    4] Compression Extract Enable
        uint32_t CE              :1;     ///< [    5] Compression Enable
        uint32_t EC              :1;     ///< [    6] ECC Configuration
        uint32_t SE              :1;     ///< [    7] Suspension Enable
        uint32_t ENH5            :1;     ///< [    8] Command Enhancement[5]
        uint32_t AU_FORMAT       :5;     ///< [13: 9] Au Format
        uint32_t CMD_ENH4_3      :2;     ///< [15:14] Command Enhancement [4:3]
        uint32_t START_SSD_PTR   :16;    ///< [31:16] Buffer Linked List Pointer
    } field ;
}    SeqCompactDescBase_t;

/// @brief SEQ Compact descriptor (1Plane, 2Plane)
typedef struct
{
    SeqCompactDescBase_t base;    // DW0 ~ DW3
    uint32_t START_ROW_ADDR[2];          // DW4 ~ DW5
} SeqCompactDesc_t;

/// @brief SEQ Compact descriptor (4Plane)
typedef struct
{
    SeqCompactDescBase_t base;
    uint32_t START_ROW_ADDR[4];
} SeqCompactDesc4p_t;

/// @brief SEQ Compact descriptor (8Plane)
typedef struct
{
    SeqCompactDescBase_t base;
    uint32_t START_ROW_ADDR[8];
} SeqCompactDesc8p_t;

/// @brief SEQ Generic descriptor(OR1=0)
typedef union
{
    uint32_t dw[8] ;

    struct
    {
        // dw 0
        uint32_t TRACK_ID        :11;    ///< [10: 0] TRACK ID
        uint32_t DG              :1;     ///< [   11] Dependency Group
        uint32_t AU_NUM          :3;     ///< [14:12] AU Number
        uint32_t OR1             :1;     ///< [   15] Optional Row 1 Enable
        uint32_t RAID_ID         :4;     ///< [18:16] RAID ID
        uint32_t RAID_CMD        :4;     ///< [23:19] RAID_Command
        uint32_t AU_SEG          :4;     ///< [27:24] Buffer Segment Selection
        uint32_t RAID_CMD2_3_2   :2;     ///< [29:28] Second RAID command[3:2]
        uint32_t RES             :1;     ///< [   30] Turn the descriptor rescheduling feature on or off
        uint32_t ZERO            :1;     ///< [   31] ECU trigger

        // dw 1
        uint32_t DEV             :4;     ///< [ 3: 0] Device
        uint32_t CH              :4;     ///< [ 7: 4] Channel
        uint32_t DES_TYPE        :4;     ///< [11: 8] DES type (0: READ, 1:PROGRAM, 2:ERASE)
        uint32_t DF              :2;     ///< [13:12] Descriptor Format (2: generic)
        uint32_t PA              :1;     ///< [   14] Path Direction
        uint32_t DGRP            :1;     ///< [   15] Descriptor Group Flag
        uint32_t DV              :1;     ///< [   16] Descriptor Valid(always 1)
        uint32_t TB              :1;     ///< [   17] Two block count enable
        uint32_t BM              :1;     ///< [   18] Busy Time Monitor Enable
        uint32_t TWOBT0          :1;     ///< [   19] Two-bit Type 0
        uint32_t JU              :1;     ///< [   20] Buffer Segment Jump Enable
        uint32_t ST              :1;     ///< [   21] Streaming Enable
        uint32_t MQ              :1;     ///< [   22] Media Queue Post Enable
        uint32_t EE              :1;     ///< [   23] Error Count Logging Enable
        uint32_t HC              :1;     ///< [   24] Host LBA Comparison for PROGRAM
        uint32_t LE              :1;     ///< [   25] AUX extraction (== le)
        uint32_t LC              :1;     ///< [   26] AUX comparison (== lc)
        uint32_t READY           :3;     ///< [29:27] Ready Bit Location
        uint32_t LD              :1;     ///< [   30] Last descriptor
        uint32_t FD              :1;     ///< [   31] First descriptor

        // dw 2
        union
        {
            struct
            {
                uint32_t HCMD_INDEX_MQ   :8;     ///< [ 7: 0] Host Command Index
                uint32_t HBLK_CNT        :9;     ///< [16: 8] Host Bock Count
                uint32_t HBLK_CTL        :15;    ///< [31:17] Streaming Controller Tag
            };
            struct
            {
                uint32_t HCMD_INDEX_HWLL :9;     ///< [ 8: 0] Host Command Index
                uint32_t PRP_CNT         :7;     ///< [15: 9] Physical Range Page Count
                uint32_t AU_INDEX        :16;    ///< [31:16] Allocation Unit Index
            };
            uint32_t SET_PARAM;                  ///< [31: 0] Set Parameter
        };

        // dw 3
        uint32_t HSTRM           :2;     ///< [ 1: 0] Host Streaming Select
        uint32_t HP              :2;     ///< [ 3: 2] MQ Port Select
        uint32_t CX              :1;     ///< [    4] Compression Extract Enable
        uint32_t CE              :1;     ///< [    5] Compression Enable
        uint32_t EC              :1;     ///< [    6] ECC Configuration
        uint32_t SE              :1;     ///< [    7] Suspension Enable
        uint32_t CIL             :1;     ///< [    8] Cut-In-Line
        uint32_t AU_FORMAT       :5;     ///< [13: 9] Au Format
        uint32_t RAID_CMD2_1_0   :2;     ///< [15:14] Second RAID command[1:0]
        uint32_t START_SSD_PTR   :16;    ///< [31:16] Buffer Linked List Pointer

        // dw 4
        uint16_t START_COL_ADDR;         ///< [15: 0] starting column within page
        uint16_t XF_COUNT;               ///< [31:16] total bytes to transfer, including meta + user + ecc

        // dw 5
        uint32_t OPTION          :28;    ///< [27: 0] Option to control NAND Flash sequence
        uint32_t RAID_OFFSET     :3;     ///< [30:28] RAID Offset
        uint32_t OPT             :1;     ///< [   31]

        // dw 6
        uint32_t START_ROW_ADDR ;        ///< [31:0] starting row address within 1st plane

        // dw 7
        uint8_t FAIL;                    ///< [7 : 0] PASS/FAIL status
        uint8_t SCMD;                    ///< [15 :8] READ_STATUS Command Byte
        uint8_t CMD2;                    ///< [23:16] Second Command Byte
        uint8_t CMD1;                    ///< [31:24] First Command Byte
    } field ;
} SeqGenericDesc_t;

/// @brief SEQ Generic descriptor(OR1=1)
typedef union
{
    uint32_t dw[10] ;

    struct
    {
        // dw 0
        uint32_t TRACK_ID        :11;    ///< [10: 0] TRACK ID
        uint32_t DG              :1;     ///< [   11] Dependency Group
        uint32_t AU_NUM          :3;     ///< [14:12] AU Number
        uint32_t OR1             :1;     ///< [   15] Optional Row 1 Enable
        uint32_t RAID_ID         :4;     ///< [18:16] RAID ID
        uint32_t RAID_CMD        :4;     ///< [23:19] RAID_Command
        uint32_t AU_SEG          :4;     ///< [27:24] Buffer Segment Selection
        uint32_t RAID_CMD2_3_2   :2;     ///< [29:28] Second RAID command[3:2]
        uint32_t RES             :1;     ///< [   30] Turn the descriptor rescheduling feature on or off
        uint32_t ZERO            :1;     ///< [   31] ECU trigger

        // dw 1
        uint32_t DEV             :4;     ///< [ 3: 0] Device
        uint32_t CH              :4;     ///< [ 7: 4] Channel
        uint32_t DES_TYPE        :4;     ///< [11: 8] DES type (0: READ, 1:PROGRAM, 2:ERASE)
        uint32_t DF              :2;     ///< [13:12] Descriptor Format (2: generic)
        uint32_t PA              :1;     ///< [   14] Path Direction
        uint32_t DGRP            :1;     ///< [   15] Descriptor Group Flag
        uint32_t DV              :1;     ///< [   16] Descriptor Valid(always 1)
        uint32_t TB              :1;     ///< [   17] Two block count enable
        uint32_t BM              :1;     ///< [   18] Busy Time Monitor Enable
        uint32_t TWOBT0          :1;     ///< [   19] Two-bit Type 0
        uint32_t JU              :1;     ///< [   20] Buffer Segment Jump Enable
        uint32_t ST              :1;     ///< [   21] Streaming Enable
        uint32_t MQ              :1;     ///< [   22] Media Queue Post Enable
        uint32_t EE              :1;     ///< [   23] Error Count Logging Enable
        uint32_t HC              :1;     ///< [   24] Host LBA Comparison for PROGRAM
        uint32_t LE              :1;     ///< [   25] AUX extraction (== le)
        uint32_t LC              :1;     ///< [   26] AUX comparison (== lc)
        uint32_t READY           :3;     ///< [29:27] Ready Bit Location
        uint32_t LD              :1;     ///< [   30] Last descriptor
        uint32_t FD              :1;     ///< [   31] First descriptor

        // dw 2
        union
        {
            struct
            {
                uint32_t HCMD_INDEX_MQ   :8;     ///< [ 7: 0] Host Command Index
                uint32_t HBLK_CNT        :9;     ///< [16: 8] Host Bock Count
                uint32_t HBLK_CTL        :15;    ///< [31:17] Streaming Controller Tag
            };
            struct
            {
                uint32_t HCMD_INDEX_HWLL :9;     ///< [ 8: 0] Host Command Index
                uint32_t PRP_CNT         :7;     ///< [15: 9] Physical Range Page Count
                uint32_t AU_INDEX        :16;    ///< [31:16] Allocation Unit Index
            };
            uint32_t SET_PARAM;                  ///< [31: 0] Set Parameter
        };

        // dw 3
        uint32_t HSTRM           :2;     ///< [ 1: 0] Host Streaming Select
        uint32_t HP              :2;     ///< [ 3: 2] MQ Port Select
        uint32_t CX              :1;     ///< [    4] Compression Extract Enable
        uint32_t CE              :1;     ///< [    5] Compression Enable
        uint32_t EC              :1;     ///< [    6] ECC Configuration
        uint32_t SE              :1;     ///< [    7] Suspension Enable
        uint32_t CIL             :1;     ///< [    8] Cut-In-Line
        uint32_t AU_FORMAT       :5;     ///< [13: 9] Au Format
        uint32_t RAID_CMD2_1_0   :2;     ///< [15:14] Second RAID command[1:0]
        uint32_t START_SSD_PTR   :16;    ///< [31:16] Buffer Linked List Pointer

        // dw 4
        uint32_t LBA7_0          :8;     ///< [ 7: 0]
        uint32_t PREFIX_3B       :24;    ///< [31: 8]

        // dw 5
        uint32_t LBA27_8         :20;    ///< [19: 0]
        uint32_t AUCP_SID        :4;     ///< [23:20]
        uint32_t HWLL            :1;     ///< [   24]
        uint32_t AEND            :1;     ///< [   25]
        uint32_t AUST            :1;     ///< [   26]
        uint32_t CPEN            :1;     ///< [   27]
        uint32_t RAID_SID        :4;     ///< [31:28]

        // dw 6
        uint16_t START_COL_ADDR;         ///< [15: 0] starting column within page
        uint16_t XF_COUNT;               ///< [31:16] total bytes to transfer, including meta + user + ecc

        // dw 7
        uint32_t OPTION          :28;    ///< [27: 0] Option to control NAND Flash sequence
        uint32_t RAID_OFFSET     :3;     ///< [30:28] RAID Offset
        uint32_t OPT             :1;     ///< [   31]

        // dw 8
        uint32_t START_ROW_ADDR ;        ///< [31:0] starting row address within 1st plane

        // dw 9
        uint8_t FAIL;                    ///< [7 : 0] PASS/FAIL status
        uint8_t SCMD;                    ///< [15 :8] READ_STATUS Command Byte
        uint8_t CMD2;                    ///< [23:16] Second Command Byte
        uint8_t CMD1;                    ///< [31:24] First Command Byte
    } field ;
} SeqGenericDescOr_t ;

typedef union
{
    uint32_t dw[10] ;

    struct
    {
        // dw 0
        uint32_t TRACK_ID        :11;    ///< [10: 0] TRACK ID
        uint32_t DG              :1;     ///< [   11]
        uint32_t FLEX_LUN_NUM    :3;     ///< [14:12] FLEX_LUN_NUM
        uint32_t OR1             :1;     ///< [   15] Optional Row 1 Enable
        uint32_t RSVD            :12;     ///< [27:16] RSVD
        uint32_t VOL             :1;     ///< [28] VOL
        uint32_t TWOBT0          :1;     ///< [29] 2BT0
        uint32_t RES             :1;     ///< [   30] Turn the descriptor rescheduling feature on or off
        uint32_t ZERO            :1;     ///< [   31] ECU trigger

        // dw 1
        uint32_t DEV             :4;     ///< [ 3: 0] Device
        uint32_t CH              :4;     ///< [ 7: 4] Channel
        uint32_t DES_TYPE        :4;     ///< [11: 8] DES type (0: READ, 1:PROGRAM, 2:ERASE)
        uint32_t DF              :2;     ///< [13:12] Descriptor Format (2: generic)
        uint32_t PA              :1;     ///< [14] Path Direction
        uint32_t RSVD0_DW1       :1;     ///< [15] Reserved
        uint32_t DV              :1;     ///< [16] Descriptor Valid(always 1)
        uint32_t RSVD1_DW1       :2;     ///< [17:18] RSVD1_DW1
        uint32_t GF              :1;     ///< [19] GF
        uint32_t RSVD2_DW1       :10;    ///< [29:20] Reserved
        uint32_t LD              :1;     ///< [ 30] Last descriptor
        uint32_t FD              :1;     ///< [ 31] First descriptor

        // dw 2
        uint32_t DQ12            :8 ;     ///< [7:0] DQ12
        uint32_t DQ13            :8 ;     ///< [15:8] DQ13
        uint32_t DQ14            :8 ;     ///< [23:16] DQ14
        uint32_t DQ15            :8 ;     ///< [31:24] DQ15

        // dw 3
        uint32_t RSVD0_DW3       :8;     ///< [ 7: 0] Reserved
        uint32_t RSVD1_DW3       :1;     ///< [ 8 ] Reserved
        uint32_t RSVD2_DW3       :23;     ///< [31 : 9 ] Reserved

        // dw 4
        uint32_t RSVD0_DW4       :32;     ///< [ 31: 0] Reserved


        // dw 5
        uint32_t RSVD0_DW5       :32;     ///< [ 31: 0] Reserved

        // dw 6
        uint32_t DQ0            :8 ;     ///< [7:0] DQ0
        uint32_t DQ1            :8 ;     ///< [15:8] DQ1
        uint32_t DQ2            :8 ;     ///< [23:16] DQ2
        uint32_t DQ3            :8 ;     ///< [31:24] DQ3

        // dw 7
        uint32_t START_CMD_PTR   :8;    ///< [7: 0] START_CMD_PT
        uint32_t RSVD0_DW7       :15;   ///< [22:8] Reserved
        uint32_t OPT23_24        :2;     ///< [ 24:23]
        uint32_t RSVD1_DW7       :6;   ///< [30:25] Reserved
        uint32_t OPT31           :1;     ///< [ 31]

        // dw 8
        uint32_t DQ4            :8 ;     ///< [7:0] DQ4
        uint32_t DQ5            :8 ;     ///< [15:8] DQ5
        uint32_t DQ6            :8 ;     ///< [23:16] DQ6
        uint32_t DQ7            :8 ;     ///< [31:24] DQ7

        // dw 9
        uint32_t DQ8            :8 ;     ///< [7:0] DQ8
        uint32_t DQ9            :8 ;     ///< [15:8] DQ9
        uint32_t DQ10           :8 ;     ///< [23:16] DQ10
        uint32_t DQ11           :8 ;     ///< [31:24] DQ11
    } field ;
} SeqGenericFlexDescOr_t ;

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------


