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
//! @brief <b> BUILD component definition </b>
//!
//=============================================================================

//-----------------------------------------------------------------------------
// Component definitions
//-----------------------------------------------------------------------------
/// @brief Code Type (_CODE_TYPE_)
#define CODE_BOOT                       0
#define CODE_MAIN                       1

#define CODE_MASK                       0xF
#define CHK_CODE_TYPE(X)                ((_CODE_TYPE_ & CODE_MASK) == (X))

/// @brief Code Type (_FW_TYPE_)
#define FW_NORMAL                       0x01
#define FW_AGING                        0x02
#define FW_DNLD                         0x04
#define FW_HWTEST                       0x08

#define FW_MASK                         0xFF
#define CHK_FW_TYPE(X)                  ((_FW_TYPE_ & FW_MASK) == (X))

/// @brief HW types (HW_TYPE)
#define HW_TYPE_SOC                     0       ///< SOC
#define HW_TYPE_FPGA                    1       ///< FPGA
#define HW_TYPE_EMUL                    2       ///< Emulator (Palladium, Veloce or any emulator)
#define HW_TYPE_SIMUL                   3       ///< Simulator

/// @brief Share code types (_SHARE_)
#define SHARE_BOOT                      0
#define SHARE_IRQ                       1
#define SHARE_RTOS                      2

/// @brief Host Interface types (_HOST_)
#define HOST_NONE                       0
#define HOST_SATA                       1
#define HOST_AHCI                       2
#define HOST_NVME                       3
#define HOST_SAS                        4

/// @brief Media Types (_MEDIA_)
#define MEDIA_NONE                      0
#define MEDIA_DRAM                      1
#define MEDIA_NAND                      2

/// @brief CPU id definitions (_CPU_)
#define CPU_ID0                         0
#define CPU_ID1                         1
#define CPU_ID2                         2
#define CPU_ID3                         3

/// @brief DRAM Types (_DRAM_)
#define DRAM_NONE                       0x00

#define DRAM_DDR                        0x10
#define DRAM_DDR1                       0x11
#define DRAM_DDR2                       0x12
#define DRAM_DDR3                       0x13
#define DRAM_DDR4                       0x14

#define DRAM_LPDDR                      0x20
#define DRAM_LPDDR1                     0x21
#define DRAM_LPDDR2                     0x22
#define DRAM_LPDDR3                     0x23
#define DRAM_LPDDR4                     0x24

#define DRAM_TYPE_MASK                  0xF0

/// @brief TCG (_TCG_)
#define TCG_NONE                        0
#define TCG_OPAL                        1
#define TCG_EDRV                        2
#define TCG_PYRITE                      3

/// @brief OPROM (_OPROM_)
#define OPROM_NONE                      0
#define OPROM_EP2                       1
#define OPROM_EP3                       2


/// @brief VENDOR (_VENDOR_)
#define VENDOR_NONE                     0
#define VENDOR_TSB                      1
#define VENDOR_SNDK                     2
#define VENDOR_SS                       3
#define VENDOR_HYNX                     4

/// @brief NAND TYPE (_NAND_)
#define NAND_NONE                       0
#define NAND_MLC                        1
#define NAND_2DTLC                      2
#define NAND_3DTLCG2                    3
#define NAND_3DTLCG3                    4

/// @brief GENERATION (_GENERATION_)
#define GENERATION_NONE                 0
#define GENERATION_CX2                  1
#define GENERATION_EP2                  2
#define GENERATION_EP3                  3
#define GENERATION_EP5                  4

/// @brief CUSTOMER (_CUSTOMER_)
#define CUSTOMER_NONE                   0
#define CUSTOMER_RD                     1
#define CUSTOMER_DQ                     2
#define CUSTOMER_AG                     3
#define CUSTOMER_AP                     4
#define CUSTOMER_MICROSOFT              5
#define CUSTOMER_DELL                   6
#define CUSTOMER_LENOVO                 7
#define CUSTOMER_HP                     8
#define CUSTOMER_ACER                   9

/// @brief Over-Provision (_OP_)
#define OP_7                            0
#define OP_14                           1
#define OP_LIST_COUNT                   2
