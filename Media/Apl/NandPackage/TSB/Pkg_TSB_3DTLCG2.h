#pragma once
//-----------------------------------------------------------------------------
//  NAND flash package parameter definitions - Hynix H27QDG8VEBIR
//-----------------------------------------------------------------------------

/// @brief NAND system level definition
#define NF_AUTO_DETECTION               TRUE

#define NJEDEC                          0x4544454A
#define NF_NCRC                         0x4352434E

/// @brief  NAND package names
#define NF_MANUFACTURER_NAME            "Auto detection"
#define NF_MODEL_NAME                   "universal"
#define NF_MODEL_ID                     {0,0,0,0,0,0}
#define NF_MODEL_ID_COMPARE_LEN         0

/// @brief NAND chip parameter definitions
#define NF_NUM_CHANNELS                 (gNfInfo.numChannels)
#define NF_DEVICES_PER_CHANNEL          (gNfInfo.devicesPerChannel)
#define NF_BITS_PER_CELL                (gNfInfo.bitsPerCell)
#define NF_LUNS_PER_DEVICE              (gNfInfo.lunsPerDevice)
#define NF_BLOCKS_PER_LUN               (gNfInfo.blocksPerLun)
#define NF_PLANE_ADDRESS_BITS           (gNfInfo.planeAddressBits)
#define NF_PAGES_PER_BLOCK              (gNfInfo.pagesPerBlock)
#define NF_DATA_BYTES_PER_PAGE          (gNfInfo.dataBytesPerPage)
#define NF_SPARE_BYTES_PER_PAGE         (gNfInfo.spareBytesPerPage)
#define NF_T_PROG                       (gNfInfo.tProg)
#define NF_T_BERS                       (gNfInfo.tBers)
#define NF_T_R                          (gNfInfo.tR)
#define NF_T_R_MP                       (gNfInfo.tRmp)
#define NF_T_CCS                        (gNfInfo.tCcs)

/// @brief NAND chip parameter definitions (derivative value)
#define NF_BITS_PAGES_PER_BLOCK         (gNfInfo.bitsPagesPerBlock)
#define NF_BITS_BLOCKS_PER_LUN          (gNfInfo.bitsBlocksPerLun)
#define NF_LUN_LSB_POSITION             (gNfInfo.lunLsbPosition)
#define NF_PLANE_LSB_POSITION           (gNfInfo.planeLsbPosition)
#define NF_PLANES_PER_LUN               (gNfInfo.planesPerLun)
#define NF_BLOCKS_PER_DEVICE            (gNfInfo.blocksPerDevice)
#define NF_BLOCKS_PER_CHANNEL           (gNfInfo.blocksPerChannel)
#define NF_PAGES_PER_DEVICE             (gNfInfo.pagesPerDevice)
#define NF_PAGES_PER_CHANNEL            (gNfInfo.pagesPerChannel)
#define NF_PAGES_PER_LUN                (gNfInfo.pagesPerLun)
#define NF_NUM_PAGES                    (gNfInfo.totalPages)
#define NF_SNPN                         (gNfInfo.snpn)

#define NF_USER_BLOCKS_PER_LUN           3642

#define NF_VREF_EN                       1
#define NF_DQS_C_EN                      1
#define NF_RE_C_EN                       1
#define NF_READ_DQS_LATENCY              0
#define NF_WRITE_DQS_LATENCY             0

#define NF_READ_RETRY_COUNT              0
