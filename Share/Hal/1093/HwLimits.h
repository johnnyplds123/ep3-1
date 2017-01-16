
//-----------------------------------------------------------------------------
// Limitation of Capabilities of Hardware
//-----------------------------------------------------------------------------
// HW_MAX_      Maximum number of XXXX by HW    (one based value)
// HW_LAST_     Last number of XXXX by HW       (zero based value)

//-----------------------------------------------------------------------------
// Core
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Host
//-----------------------------------------------------------------------------
#define HW_MAX_LBA_FORMATS              4       // max. supported number of LBA formats
#define HW_MAX_IO_QUEUES                16      // max. supported number of I/O queues (1 for ADM queue) for FUNC0
#define HW_MAX_QUEUE_SIZE               4096    // max. supported number of queue entries
#define HW_MAX_INT_VECTORS              19      // max. supported number of interrupt vectors
#define HW_MAX_NAMESPACES               4       // max. supported number of namespace

#define HW_MAX_ADMIN_CMD_INDEX          8       // max. supported admin command index
#define HW_MAX_IO_CMD_INDEX             256     // max. supported IO command index

//-----------------------------------------------------------------------------
// Media
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Sequencer map
//-----------------------------------------------------------------------------
/// @brief Sequencer maximum number of NAND channels and devices
#define MAX_SUPPORTED_CHANNELS          8
#define MAX_SUPPORTED_DEVICES           8
#define MAX_SUPPORTED_PLANES            4
#define MAX_SUPPORTED_LUNS              2

#if _NAND_ == NAND_MLC
    #define MAX_SUPPORTED_PAGE          256
    #define MAX_SUPPORTED_BLOCK         4252
#elif _NAND_ == NAND_3DTLCG2
    #define MAX_SUPPORTED_PAGE          576
    #define MAX_SUPPORTED_BLOCK         3944 ///CS
#endif

/// @brief FTL Group
#define MAX_SUPPORTED_FTL_GROUP         1

/// @brief Descriptor FIFO (Mandatory)
#define DF_FIFO_SIZE                    0x5A00
#define DF_FIFO_ALIGN                   8

/// @brief AU Encoder Command FIFO (Mandatory)
#define AENC_FIFO_SIZE                  0x2000// Keep HW default value
#define AENC_FIFO_ENTRY_SIZE            32
#define AENC_FIFO_ALIGN                 4

/// @brief AU Decoder Command FIFO (Mandatory)
#define ADEC_FIFO_SIZE                  0x2000// Keep HW default value
#define ADEC_FIFO_ENTRY_SIZE            32
#define ADEC_FIFO_ALIGN                 4

/// @brief AU Decoder Command FIFO (Optional)
#define HP_ADEC_FIFO_SIZE               0
#define HP_ADEC_FIFO_ENTRY_SIZE         32
#define HP_ADEC_FIFO_ALIGN              4

/// @brief Sequencer LUT (Mandatory)
#define LUT_SIZE                        0x800// Keep HW recommended value
#define LUT_ENTRY_SIZE                  8
#define LUT_ALIGN                       4

/// @brief Sequencer LUT Encoder (Mandatory)
#define LUT_AU_ENC_SIZE                 0x600// Keep HW recommended value
#define LUT_AU_ENC_ENTRY_SIZE           12
#define LUT_AU_ENC_ALIGN                4

/// @brief Sequencer LUT Decoder (Mandatory)
#define LUT_AU_DEC_SIZE                 0x600// Keep HW recommended value
#define LUT_AU_DEC_ENTRY_SIZE           12
#define LUT_AU_DEC_ALIGN                4

/// @brief Track Memory (Mandatory)
#define TRK_CONF_NUM                    2

#if (TRK_CONF_NUM == 0)
    #define TRKM_SIZE                   0x7000  // # of TRK = 2K   # of DES per TRK = 4
#elif (TRK_CONF_NUM == 1)
    #define TRKM_SIZE                   0x4000  // # of TRK = 512  # of DES per TRK = 16
#elif (TRK_CONF_NUM == 2)
    #define TRKM_SIZE                   0x3400  // # of TRK = 128  # of DES per TRK = 64
#elif (TRK_CONF_NUM == 3)
    #define TRKM_SIZE                   0x3100  // # of TRK = 32   # of DES per TRK = 256
#elif (TRK_CONF_NUM == 4)
    #define TRKM_SIZE                   0x3040  // # of TRK = 8    # of DES per TRK = 1024
#else
    #error "Invalid TRK configuration !!!"
#endif

#define TRKM_ALIGN                      4

/// @brief AUX Insert FIFO
#define AIF_FIFO_SIZE                   0x3F00
#define AIF_FIFO_ALIGN                  12

/// @brief AUX Compare FIFO
#define ACF_FIFO_SIZE                   0
#define ACF_FIFO_ALIGN                  4

/// @brief AUX Extract FIFO
#define AEF_FIFO_SIZE                   0x900
#define AEF_FIFO_ALIGN                  12      ///< If AUX OOO this must set to 12 byte aligned, AUX in order plz set to 8/16 byte aligned

/// @brief HLBA Insert FIFO
#define HIF_FIFO_SIZE                   0
#define HIF_FIFO_ALIGN                  4

/// @brief HLBA Compare FIFO
#define HCF_FIFO_SIZE                   0
#define HCF_FIFO_ALIGN                  4

/// @brief ERROR Count FIFO
#define ECF_FIFO_SIZE                   0
#define ECF_FIFO_ALIGN                  8

/// @brief NAND Parameter FIFO
#define GETP_FIFO_SIZE                  0 //0x200// Read parameter page data size for ONFI
#define GETP_FIFO_ALIGN                 8

/// @brief MQP0 FIFO
#define MQP0_FIFO_SIZE                  0
#define MQP0_FIFO_ENTRY_SIZE            4
#define MQP0_FIFO_ALIGN                 8

/// @brief MQP1 FIFO
#define MQP1_FIFO_SIZE                  0
#define MQP1_FIFO_ENTRY_SIZE            4
#define MQP1_FIFO_ALIGN                 8

/// @brief MQP2 FIFO
#define MQP2_FIFO_SIZE                  0
#define MQP2_FIFO_ENTRY_SIZE            4
#define MQP2_FIFO_ALIGN                 8

/// @brief MQP3 FIFO
#define MQP3_FIFO_SIZE                  0
#define MQP3_FIFO_ENTRY_SIZE            4
#define MQP3_FIFO_ALIGN                 8

/// @brief NAND Busy Time Monitor FIFO
#define BTMF_FIFO_SIZE                  0x100
#define BTMF_FIFO_ALIGN                 8

/// @brief Compression Encoder FIFO
#define CEF_FIFO_SIZE                   0
#define CEF_FIFO_ALIGN                  4

/// @brief Compression Decoder FIFO
#define CDF_FIFO_SIZE                   0
#define CDF_FIFO_ALIGN                  8

/// @brief Compression Extract FIFO
#define CXF_FIFO_SIZE                   0
#define CXF_FIFO_ALIGN                  8

/// @brief Slow Retry FIFO
#define SRF_FIFO_SIZE                   0x20
#define SRF_FIFO_ALIGN                  8

/// @brief Fast Descriptor FIFO
#define FDF_FIFO_SIZE                   0
#define FDF_FIFO_ALIGN                  8

/// @brief Fast AUX Insert FIFO
#define FAIF_FIFO_SIZE                  0
#define FAIF_FIFO_ALIGN                 4

/// @brief Fast AUX Compare FIFO
#define FACF_FIFO_SIZE                  0
#define FACF_FIFO_ALIGN                 4

/// @brief Fast HLBA Compare FIFO
#define FHCF_FIFO_SIZE                  0
#define FHCF_FIFO_ALIGN                 8

/// @brief Compression Encoder FIFO
#define FCEF_FIFO_SIZE                  0
#define FCEF_FIFO_ALIGN                 4

/// @brief Compression Decoder FIFO
#define FCDF_FIFO_SIZE                  0
#define FCDF_FIFO_ALIGN                 8

/// @brief Fast Slow Retry FIFO
#define FSRF_FIFO_SIZE                  0
#define FSRF_FIFO_ALIGN                 8
