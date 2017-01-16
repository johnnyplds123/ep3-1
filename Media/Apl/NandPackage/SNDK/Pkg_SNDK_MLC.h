#pragma once
//-----------------------------------------------------------------------------
//  NAND flash package parameter definitions - Hynix H27QDG8VEBIR
//-----------------------------------------------------------------------------

/// @brief NAND system level definition
#define NF_AUTO_DETECTION               FALSE

#define NF_INTERFACE_TYPE               NF_JEDEC
#define NF_INTERFACE_MAJOR_VERSION      2
#define NF_INTERFACE_MINOR_VERSION      0

/// @brief  NAND package names
#define NF_MANUFACTURER_NAME            "SanDisk"
#define NF_MODEL_NAME                   "SDYNSHDRM128GEC"
#define NF_MODEL_ID                     {0x45,0x3A,0xA5,0x93,0x7E,0x51}
#define NF_MODEL_ID_COMPARE_LEN         6

/// @brief NAND chip parameter definitions
#define NF_BITS_PER_CELL                2
#define NF_LUNS_PER_DEVICE              2
#define NF_BLOCKS_PER_LUN               4252
#define NF_USER_BLOCKS_PER_LUN          4096
#define NF_PLANE_ADDRESS_BITS           2
#define NF_PAGES_PER_BLOCK              256
#define NF_DATA_BYTES_PER_PAGE          16384
#define NF_SPARE_BYTES_PER_PAGE         1376
#define NF_T_PROG                       1300
#define NF_T_BERS                       5000
#define NF_T_R                          50
#define NF_T_R_MP                       100
#define NF_T_CCS                        200

/// @brief NAND chip parameter definitions (derivative value)
#define NF_BITS_PAGES_PER_BLOCK         8
#define NF_BITS_BLOCKS_PER_LUN          12
#define NF_LUN_LSB_POSITION             20
#define NF_PLANE_LSB_POSITION           8
#define NF_PLANES_PER_LUN               (1<<NF_PLANE_ADDRESS_BITS)
#define NF_BLOCKS_PER_DEVICE            (NF_BLOCKS_PER_LUN*NF_LUNS_PER_DEVICE)
#define NF_BLOCKS_PER_CHANNEL           (NF_BLOCKS_PER_DEVICE*NF_DEVICES_PER_CHANNEL)
#define NF_PAGES_PER_LUN                (NF_PAGES_PER_BLOCK*NF_BLOCKS_PER_LUN)
#define NF_PAGES_PER_DEVICE             (NF_PAGES_PER_BLOCK*NF_BLOCKS_PER_DEVICE)
#define NF_PAGES_PER_CHANNEL            (NF_PAGES_PER_DEVICE*NF_DEVICES_PER_CHANNEL)

#define NF_VREF_EN                       1
#define NF_DQS_C_EN                      1
#define NF_RE_C_EN                       1
#define NF_READ_DQS_LATENCY              0
#define NF_WRITE_DQS_LATENCY             0

#define NF_READ_RETRY_COUNT              0

