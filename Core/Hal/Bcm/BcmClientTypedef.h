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
//! @file   BcmClientTypedef.h
//! @brief  Include all Data types that are used in BCM IROM & EROM
//
// ============================================================================
//
// Notes:
//    If BCM IROM or EROM is modified, ones needs to make sure all data structure
//    here is matching those in new BCM IROM or EROM.


#ifndef _BCM_TYPEDEF
#define _BCM_TYPEDEF

#define DISABLE                 (0)
#define ENABLE                  (1)

#define BCM_EROM_SUPPORT

#define TotalAESRangeNumber     32
#define AES_DBG                 ENABLE


//#include "BcmClientProcessor.h"

#define SOC_VERSION_1_0             (10)
#define SOC_VERSION_1_1             (11)
#define SOC_VERSION_2_0             (20)
#define SOC_VERSION_2_1             (21)
#define ELDORA_SOC_VERSION          SOC_VERSION_2_1
#define ELDORA_SOC_VERSION_1_0      (ELDORA_SOC_VERSION == SOC_VERSION_1_0)
#define ELDORA_SOC_VERSION_1_1      (ELDORA_SOC_VERSION == SOC_VERSION_1_1)
#define ELDORA_SOC_VERSION_2_0      (ELDORA_SOC_VERSION == SOC_VERSION_2_0)
#define ELDORA_SOC_VERSION_2_1      (ELDORA_SOC_VERSION == SOC_VERSION_2_1)
#define ELDORA_SOC_VERSION_2_0_UP   ((ELDORA_SOC_VERSION == SOC_VERSION_2_0) || (ELDORA_SOC_VERSION == SOC_VERSION_2_1))


//------------------------------------------------------------------------+
//                                                      Mailbox Structure |
//------------------------------------------------------------------------+
// It must match with BCM's _MAILBOX in "mailbox.h"

typedef struct _MAILBOX {
  uint32_t arg0;                  // 0x0 Write Only
  uint32_t arg1;                  // 0x4 Write Only
  uint32_t arg2;                  // 0x8 Write Only
  uint32_t arg3;                  // 0xc Write Only
  uint32_t arg4;                  // 0x10 Write Only
  uint32_t arg5;                  // 0x14 Write Only
  uint32_t arg6;                  // 0x18 Write Only
  uint32_t arg7;                  // 0x1c Write Only
  uint32_t arg8;                  // 0x20 Write Only
  uint32_t arg9;                  // 0x24 Write Only
  uint32_t arg10;                 // 0x28 Write Only
  uint32_t arg11;                 // 0x2c Write Only
  uint32_t arg12;                 // 0x30 Write Only
  uint32_t arg13;                 // 0x34 Write Only
  uint32_t arg14;                 // 0x38 Write Only: linked-list dma host-read  pointer
  uint32_t arg15;                 // 0x3c Write Only: linked-list dma host-write pointer
  uint32_t cmd;                   // 0x40 Write Only
  uint8_t  reserved[60];

  uint32_t return_status;         // 0x80 Read Only
  uint32_t status0;               // 0x84 Read Only
  uint32_t status1;               // 0x88 Read Only
  uint32_t status2;               // 0x8c Read Only
  uint32_t status3;               // 0x90 Read Only
  uint32_t status4;               // 0x94 Read Only
  uint32_t status5;               // 0x98 Read Only
  uint32_t status6;               // 0x9c Read Only
  uint32_t status7;               // 0xa0 Read Only
  uint32_t status8;               // 0xa4 Read Only
  uint32_t status9;               // 0xa8 Read Only
  uint32_t status10;              // 0xac Read Only
  uint32_t status11;              // 0xb0 Read Only
  uint32_t status12;              // 0xb4 Read Only
  uint32_t status13;              // 0xb8 Read Only
  uint32_t status14;              // 0xbc Read Only
  uint32_t status15;              // 0xc0 Read Only
  uint32_t fifo_status;           // 0xc4 Read Only

  uint32_t host_intp_reg;         // 0xc8 Write-to-Clear
  uint32_t host_intp_mask;        // 0xcc Write-to-Clear
  uint32_t host_exct_addr;        // 0xd0 Write-to-Clear
  uint32_t sp_trust_reg;          // 0xd4 Read Only
  uint32_t wtm_id;                // 0xd8 Read Only
  uint32_t wtm_revision;          // 0xdc Read Only
} MAILBOX;

//------------------------------------------------------------------------+
//                                                         IROM Primitive |
//------------------------------------------------------------------------+
// Each Primitive is assiociated with a number
// The number is used to search for correct function call in IROM
//
// It must match with the PI defined in BCM's "layer2.h"

typedef enum _IROM_PRIMITIIVE {
  BCM_CANT_USE = 0,

  BCM_SET_FLAGS,
  BCM_GET_VERSION_INFO,
  BCM_RESET,
  BCM_SELF_TEST,              // 4
  BCM_SET_BATCH_COUNT,
  BCM_CONFIGURE_DMA,
  BCM_GET_SYSTEM_STATE,

  BCM_GET_CONTEXT_INFO,       // 8
  BCM_LOAD_ENGINE_CONTEXT,
  BCM_STORE_ENGINE_CONTEXT,
  BCM_PURGE_CONTEXT_CACHE,

  BCM_AES_INIT,               // 12
  BCM_AES_ZERORIZE,
  BCM_AES_PROCESS,
  BCM_AES_LOAD_IV,
  BCM_AES_LOAD_KEY,
  BCM_AES_KEY_GEN,

  BCM_HASH_INIT,              // 18
  BCM_HASH_ZERORIZE,
  BCM_HASH_UPDATE,
  BCM_HASH_FINAL,

  BCM_HMAC_INIT,              // 22
  BCM_HMAC_ZERORIZE,
  BCM_HMAC_UPDATE,
  BCM_HMAC_FINAL,             // 25
  BCM_HMAC_LOAD_KEY,
  BCM_HMAC_KEY_GEN,

  BCM_DRBG_GEN_RAN_BITS,      // 28
  BCM_DRBG_FIPS_TEST,
  BCM_DRBG_RESEED,

  BCM_OTP_WRITE,              // 31
  BCM_OTP_READ,

  BCM_RSASSA_PKCS1_V15_VERIFY_INIT,
  BCM_RSASSA_PKCS1_V15_VERIFY_UPDATE,
  BCM_RSASSA_PKCS1_V15_VERIFY_FINAL,
  BCM_RSASSA_PKCS1_V15_VERIFY,

  BCM_MC_INIT,                // 37
  BCM_MC_LOAD,
  BCM_MC_READ,
  BCM_MC_SAVE,

  BCM_FW_AUTHORIZE,           // 41

  BCM_EC_ZEROIZE,
  BCM_EC_INIT,
  BCM_EC_OP,
  BCM_EC_LOAD_PARMS,
  BCM_EC_GET_RESULTS,         // 46
  BCM_EC_LOAD_PARMS_LONG,
  BCM_EC_GET_RESULTS_LONG,
  BCM_EC_POINT_MULTIPLY,
  BCM_EC_POINT_ADD,           // 50
  BCM_EC_POINT_SUBTRACT,
  BCM_EC_ADDITIVE_INVERSION,
  BCM_EC_POINT_DOUBLE,

  BCM_ZMODP_ZEROIZE,          // 54
  BCM_ZMODP_PRECOMP,
  BCM_ZMODP_PRECOMP_PARAM,
  BCM_ZMODP_EXP,
  BCM_ZMODP_MULT_INVERSE,     // 58
  BCM_ZMODP_MODULAR_MULTIPLY,
  BCM_ZMODP_MODULAR_EXPONENTIATE,
  BCM_SLEEP,
  BCM_ZMODP_MODULAR_ADD,
  BCM_ZMODP_MODULAR_SUBTRACT, // 63

  BCM_DRBG_INSTANTIATE,       // 64
  BCM_DRBG_GENERATE,
  BCM_RESUME,

  BCM_AESX_LOAD_KEY,          // 67
  BCM_AESX_LOAD_IV,
  BCM_AESX_INIT,
  BCM_AESX_RECRYPT_PROCESS,
  BCM_AESX_PROCESS,

  BCM_NVR_WRITE,              // 72
  BCM_NVR_READ,
  BCM_NVR_ERASE,

  BCM_PRIMITIVE_LAST_ONE      // 75
} IROM_PRIMITIIVE;


//------------------------------------------------------------------------+
//                                                         EROM PRIMITIVE |
//------------------------------------------------------------------------+
// Each Primitive is assiociated with a number
// The number is used to search for correct function call in EROM
//
// Notes:
// If the number is already assigned to a function, depending on
// the search order in EROM or IROM (sched.c) That function can be replaced

// It must match with the PI defined in BCM's "layer3.h"
// Change Project name , such as "MONET", to "SOC"
typedef enum _IROM_PRIMITIVE_SUPERSET {
  BCM_SOC_AUTHENTICATED_COMMAND = BCM_PRIMITIVE_LAST_ONE, // 75
  BCM_SOC_TWO_FACTOR_FW_AUTHORIZE,
  BCM_SOC_RSASSA_PKCS1_V15_TWO_FACTOR_VERIFY,
  BCM_SOC_MANAGE_DMA_PARITY,
  BCM_SOC_PRIMITIVE_LAST_ONE // 79
} IROM_PRIMITIVE_SUPERSET;

#ifdef BCM_EROM_SUPPORT

// insignia-specific layer3 primitives
// note that it starts where the table, above, ends
typedef enum _EROM_PRIMITIVE
{
    SOC_GET_VERSION_INFO     = BCM_SOC_PRIMITIVE_LAST_ONE,
    EROM_APB_WRITE,                     // enum value for BCM: STATUS erom_apb_write ( U32 apb_port, U32 byte_address, U32 value );
    EROM_APB_READ,                      // enum value for BCM: STATUS erom_apb_read ( U32 apb_port, U32 byte_address );
    EROM_APB_DMA_WRITE,                 // enum value for BCM: STATUS erom_apb_dma_write ( U32 apb_port, U32 src_address, U32 dst_address, U32 num_words, U32 inc_mode );
    EROM_APB_DMA_READ,                  // enum value for BCM: STATUS erom_apb_dma_read ( U32 apb_port, U32 src_address, U32 dst_address, U32 num_words, U32 inc_mode );

    EROM_SELECT_ENTROPY_PP,             // enum value for BCM: STATUS erom_select_entropy_pp(postProcessor_t pp);
    EROM_OTP_READ_CALL_L2_IROM_PRIM,    // enum value for BCM: STATUS erom_otp_read_call_l2_irom_prim(OTP otp, U32 *pDst );
    EROM_OTP_WRITE_CALL_L2_IROM_PRIM,   // enum value for BCM: STATUS erom_otp_write_call_l2_irom_prim(OTP otp, U32 *pDst );
    EROM_OTP_READ_VDD_SETPOINT_IROM_PRIM, // enum value for BCM: STATUS erom_otp_read_vdd_setpoint_irom_prim(void);
    EROM_READ_MAES_KEY,                 // enum value for BCM: STATUS erom_read_maes_key(U32 rangeNum);
    EROM_READ_MAES_XTS_KEY,             // enum value for BCM: STATUS erom_read_maes_xts_key(U32 rangeNum);
    EROM_READ_MAES_IV,                  // enum value for BCM: STATUS erom_read_maes_iv(U32 rangeNum);
    EROM_BATCH_KEYRAM_ZERORIZE,         // enum value for BCM: STATUS erom_keyRAM_zerorize(void);

    EROM_AES_KEY_WRAP,
    ELDORA_EROM_UNLOAD,

    EROM_PRIMITIVE_LAST_ONE
} EROM_PRIMITIVE;

#endif

//------------------------------------------------------------------------+
//                                                                 STATUS |
//------------------------------------------------------------------------+
// It must match with the status defined in status.h

typedef enum _STATUS {
  STATUS_SUCCESS = 0,
  STATUS_FAILURE = 255,
  STATUS_PENDING,                   // 0x100
  STATUS_NO_RESOURCES,
  STATUS_BAD_DEVICE,
  STATUS_SHORT_XFR,
  STATUS_OFFLINE,                   // 260
  STATUS_NO_REQUESTS_PENDING,
  STATUS_UNSUPPORTED_FUNCTION,
  STATUS_UNSUPPORTED_PARAMETER,
  STATUS_IN_USE,
  STATUS_CANCELED,
  STATUS_NULL_BUFFER,
  STATUS_ILLEGAL_BLOCK_SIZE,
  STATUS_BUFFER_NOT_ALIGNED,
  STATUS_PARAMETER_OUT_OF_RANGE,
  STATUS_NULL_POINTER,              // 270
  STATUS_CRC_ERROR,
  STATUS_ECC_ERROR,                 // 0x110
  STATUS_PARITY_ERROR,
  STATUS_FRAMING_ERROR,
  STATUS_OVERRUN_ERROR,
  STATUS_OFFSET_ERROR,
  STATUS_IN_PROGRESS,
  STATUS_RETRY_COUNT_EXCEEDED,
  STATUS_BAD_ENGINE_OP,
  STATUS_BAD_CACHE_ID,              // 280
  STATUS_LIFECYCLE_SPENT,
  STATUS_PERMISSION_DENIED,
  STATUS_DEVICE_CORRUPTED,
  STATUS_MODE_ERROR,
  STATUS_BAD_THREAD_ID,
  STATUS_BAD_TRANSFER_SIZE,
  STATUS_CONVERGENCE_ERROR,
  STATUS_FATAL_INTERNAL_ERROR,      // 0x120
  STATUS_ECB_MODE_WITH_PARTIAL_CODEWORD,
  STATUS_NO_KEY,                    // 0x122 / 290
  STATUS_INVALID_BINDING,
  STATUS_INVALID_SIGNATURE,
  STATUS_SIGNATURE_REPRESENTATIVE_OUT_OF_RANGE,
  STATUS_HASH_NOT_SUPPORTED,
  STATUS_INTENDED_ENCODED_MESSAGE_LENGTH_TOO_SHORT,
  STATUS_INTEGER_TOO_LARGE,
  STATUS_ENGINE_CONTEXT_MISMATCH,
  STATUS_ILLEGAL_KEY,
  STATUS_DMA_TIMEOUT,
  STATUS_DMA_BUS_ERROR,             // 0x12C / 300
  STATUS_DMA_PARITY_ERROR,
  STATUS_DMA_LINKED_LIST_ACCESS_ERROR,
  STATUS_DMA_PAUSE_COMPLETION_TIMEOUT,
  STATUS_DMA_IDIOPATHIC_ERROR,      // 0x130
  STATUS_HASH_TIMEOUT,
  STATUS_AES_TIMEOUT,
  STATUS_ZMODP_TIMEOUT,
  STATUS_EC_TIMEOUT,
  STATUS_DES_TIMEOUT,
  STATUS_RC4_TIMEOUT,               // 310
  STATUS_MD5_TIMEOUT,
  STATUS_MCT_TIMEOUT,
  STATUS_EBG_TIMEOUT,
  STATUS_OTP_TIMEOUT,
  STATUS_HASH_MESSAGE_OVERFLOW,
  STATUS_UNSUPPORTED_DIGEST_TYPE,
  STATUS_BUS_ERROR,
  STATUS_RSA_MODULUS_TOO_SHORT,
  STATUS_MESSAGE_REPRESENTATIVE_OUT_OF_RANGE,
  STATUS_DIGEST_MISMATCH,           // 0x140, 320
  STATUS_INSUFFICIENT_PRIVILEGE,
  STATUS_ZERO_DIVISOR,
  STATUS_RDSA_PUBLIC_EXPONENT_OUT_OF_RANGE,
  STATUS_RDSA_INVALID_KEY_LENGTH,
  STATUS_PRIVATE_KEY_TOO_SMALL,
  STATUS_GENERATE_PRIME_FAILURE,
  STATUS_XPXQ_DISTANCE_INVALID,
  STATUS_PQ_DISTANCE_INVALID,
  STATUS_RDSA_PUBLIC_EXPONENT_EVEN_ERROR,
  STATUS_ECDSA_INVALID_CURVE_TYPE,  // 330

  STATUS_ECDSA_HASH_NOT_SUPPORTED,
  STATUS_ECDSA_HASH_ERROR,

  STATUS_ECDSA_VALID_SIGNATURE,
  STATUS_ECDSA_INVALID_SIGNATURE,
  STATUS_ECDSA_INVALID_POINT_FORMAT,
  STATUS_ECDSA_INVALID_COMPRESSED_POINT,    //0x150
  STATUS_ECDSA_INVALID_HYBRID_POINT,
  STATUS_ECDSA_INVALID_UNCOMPRESSED_POINT,
  STATUS_ECDSA_OCTET2POINT_ERROR,
  STATUS_ECDSA_VALID_PUBLIC_KEY,            //0x154, 340
  STATUS_ECDSA_PUBLIC_KEY_NOT_ON_CURVE,
  STATUS_ECDSA_PUBLIC_KEY_OUT_OF_RANGE,

  STATUS_UNEXPECTED_IRQ,
  STATUS_UNEXPECTED_FIQ,
  STATUS_UNEXPECTED_UNDEFINED_INST,
  STATUS_UNEXPECTED_DATA_ABORT,
  STATUS_UNEXPECTED_PREFETCH_ABORT,
  STATUS_UNEXPECTED_SWI,
  STATUS_UNEXPECTED_UNUSED,
  STATUS_ZERO_INPUT_TO_INVERTER,  // 350
  STATUS_ZERO_SCALAR_OUTPUT,
  STATUS_ZERO_KEY_RECEIVED_FOR_SCALAR_OPERATION,    //0x160
  STATUS_WRONG_IROM_VERSION,
  STATUS_BAD_ENGINE_ID,
  STATUS_FUNCTION_OUT_OF_RANGE,             //0x163
  STATUS_READ_VERIFICATION_FAILED,
  STATUS_SELF_TEST_ERROR,
  STATUS_INCORRECT_SYSTEM_STATE,
  STATUS_INVALID_TOKEN,
  STATUS_DRBG_RESEED_REQUIRED,   // 360
  STATUS_UNSUPPORTED_ALGORITHM,
  STATUS_SLEEP_CONTEXT_PREVIOUSLY_RESTORED,
  STATUS_SLEEP_CONTEXT_INVALID,
  STATUS_DMA_FIFO_PARITY_ERROR,
  STATUS_EROM_ALREADY_LOADED,    // 365
  STATUS_BIU_MAILBOX_OVERRUN,
  STATUS_USER_DEFINED = 4096,
  STATUS_LAST_ONE
} STATUS;

// Test Bench's Status
typedef enum _EXT_STATUS {
  STATUS_COMMAND_TIMEOUT = STATUS_USER_DEFINED,
  STATUS_UNEXPECTED_BOOT_ERROR,
  STATUS_TEST_FAILURE,
  EXT_STATUS_LAST_ONE
} EXT_STATUS;

// Panic Code
//  - When mailbox 0x80 (return_status) is STATUS_FATAL_INTERNAL_ERROR,
//    the mailbox 0x84 (status 0) contains the following code

typedef enum _PANIC_CODES {
  PANIC_CANT_USE                   = 0x00,
  PANIC_UNEXPECTED_IRQ             = 0x0A,     // 10
  PANIC_UNEXPECTED_FIQ             = 0x0B,     // 11
  PANIC_UNDEFINED_INSTRUCTION      = 0x0C,     // 12
  PANIC_DATA_ABORT_TRAP            = 0x0D,     // 13
  PANIC_PREFETCH_ABORT_TRAP        = 0x0E,     // 14
  PANIC_UNEXPECTED_SWI             = 0x0F,     // 15
  PANIC_UNEXPECTED_NMI,
  PANIC_UNEXPECTED_HARD_FAULT,
  PANIC_UNEXPECTED_MMU_EXCEPTION,
  PANIC_UNEXPECTED_USAGE_FAULT,
  PANIC_UNEXPECTED_BUS_FAULT,                  // 20
  PANIC_SPIKE = 32,                            // place a spike in the ground here
  PANIC_STATE_CORRUPTED,                       // 33
  PANIC_RED_ZONE_OVERFLOW,
  PANIC_YELLOW_ZONE_OVERFLOW,
  PANIC_MEMORY_ALLOCATION,
  PANIC_WATCHDOG_TIMEOUT,
  PANIC_CORRUPTED_DISPATCH_TABLE,              // primtive table has unsupported argument count
  PANIC_SYSTEM_EXIT,                           // somebody did a _sys_exit()!
  PANIC_UNUSED_HANDLER_INVOKED,                // somehow, unused_handler() was called!
  PANIC_BAD_ENGINE_ID,                         // 41
  PANIC_BAD_EROM_TRANSFER_ADDRESS,
  PANIC_UNEXPECTED_EXCEPTION,
  PANIC_UNSUPPORTED_INTERNAL_FUNCTION,
  PANIC_UNSUPPORTED_FUNCTION,
  PANIC_DRBG_SELFTEST_FAILURE,
  PANIC_HASH_SELFTEST_FAILURE,
  PANIC_AES_SELFTEST_FAILURE,
  PANIC_ZMODP_SELFTEST_FAILURE,
  PANIC_DMA_SELFTEST_FAILURE,                  // 50
  PANIC_DES_SELFTEST_FAILURE,
  PANIC_RC4_SELFTEST_FAILURE,
  PANIC_EC_SELFTEST_FAILURE,
  PANIC_MTC_SELFTEST_FAILURE,
  PANIC_ENTROPY_SOURCE_SELFTEST_FAILURE,       // 55
  PANIC_INIT_FAILED,
  PANIC_LIFECYCLE_SPENT,
  PANIC_LUCASTEST_VALUE_NOT_FOUND,
  PANIC_SYSTEM_TIMER_DEAD,
  PANIC_OTP_CORRUPTED,                         // 60
  PANIC_PLATFORM_SELFTEST_FAILURE,
  PANIC_UNCORRECTABLE_ECC_ERROR,
  PANIC_ENGINE_SELFTEST_FAILURE,
  PANIC_UNCORRECTABLE_FAILURE_DURING_POWERDOWN_SEQUENCE,
  PANIC_UNCORRECTABLE_FAILURE_DURING_STATE_TRANSITION_SEQUENCE,
  PANIC_CPU_SELF_TEST_FAILURE,
  PANIC_ROM_CHECK_FAILURE,                     // 67
  PANIC_RAM_CHECK_FAILURE,
  PANIC_RSA_SELFTEST_FAILURE,
  PANIC_SIDE_CHANNEL_ATTACK,                   // 70
  PANIC_INTERNAL_ADDRESS_RANGE,
  PANIC_DRBG_FAILURE,
  PANIC_AES2_SELFTEST_FAILURE,
  PANIC_SPURIOUS_INTERRUPT,
  PANIC_AXI_SLAVE_BUS_ERROR,                   // 75
  PANIC_AXI_ADDRESS_ACCESS_ERROR,
  PANIC_AXI_INPUT_SLAVE_FIFO_PARITY_ERROR,
  PANIC_RESERVED_OR_UNASSIGNED_IRQ_ACTIVE,
  PANIC_NULL_POINTER,                          // 79
  PANIC_LAST_ONE = 127,
} PANIC_CODE;

//------------------------------------------------------------------------+
//                                                              Algorithm |
//------------------------------------------------------------------------+
// It must mathch with BCM's algos.h

typedef enum _ALGORITHM_TYPE {
  ALG_SHA1,
  ALG_SHA224,
  ALG_SHA256,
  ALG_SHA384,
  ALG_SHA512,
  ALG_MD5,
  ALG_AES_ECB128,
  ALG_AES_ECB256,
  ALG_AES_CBC128,
  ALG_AES_CBC192,
  ALG_AES_CBC256,
  ALG_AES_CTR128,
  ALG_AES_CTR192,
  ALG_AES_CTR256,
  ALG_AES_XTS128,
  ALG_AES_XTS256,
  ALG_DES_ECB,
  ALG_DES_CBC,
  ALG_TDES_ECB,
  ALG_TDES_CBC,
  ALG_RC4,
  ALG_EC_SCALAR_POINT_MULT,
  ALG_EC_POINT_ADD,
  ALG_EC_POINT_SUBTRACT,
  ALG_EC_POINT_DOUBLE,
  ALG_EC_POINT_ADDITIVE_INVERSION,
  ALG_EC_ZERORIZE,
  ALG_ECDSA_CURVE_P224,
  ALG_ECDSA_CURVE_P256,
  ALG_ECDSA_CURVE_P192,
  ALG_ECDSA_CURVE_P384,
  ALG_ECDSA_CURVE_P521,
  ALG_AES_CBC_NIST_CS2128,
  ALG_AES_CBC_NIST_CS2192,
  ALG_AES_CBC_NIST_CS2256,
  ALG_AES_KEYWRAP128,
  ALG_AES_KEYWRAP192,
  ALG_AES_KEYWRAP256,
  ALG_AES_BCC128,
  ALG_AES_BCC192,
  ALG_AES_BCC256
} ALGORITHM_TYPE;

//------------------------------------------------------------------------+
//                                                              Engine ID |
//------------------------------------------------------------------------+
// It must mathch with BCM's engines.h

typedef enum _ENGINE_ID {
  ENGINE_ID_NULL = 0,                       //  must be first and eval to zero
  ENGINE_ID_HASH,
  ENGINE_ID_AES,   // 2
  ENGINE_ID_ZMODP,
  ENGINE_ID_EC,    // 4
  ENGINE_ID_DES,
  ENGINE_ID_RC4,   // 6
  ENGINE_ID_AES2,
  ENGINE_ID_LAST_ONE
} ENGINE_ID;

//------------------------------------------------------------------------+
//                                                    APB Register Offset |
//------------------------------------------------------------------------+

//---------------------------------------------- Media AES
#define AES_ENG0_CFG            (0x0)
#define AES_ENG0_CTRL           (0x4)
#define AES_ENG0_CMD            (0x8)
#define AES_ENG0_STATUS         (0x10)
#define AES_ENG0_MONT           (0x20)

#define AES_ENG1_CFG            (0x100)
#define AES_ENG1_CTRL           (0x104)
#define AES_ENG1_CMD            (0x108)
#define AES_ENG1_STATUS         (0x110)
#define AES_ENG1_MONT           (0x120)

#define SHR_CTRL                (0x200)
#define SHR_CMD                 (0x204)
#define SHR_ENG_STATUS          (0x20c)
#define SHR_MEM_PWD_CTRL        (0x210)
#define SHR_MEM_TIME_CTRL       (0x214)
#define SHR_BIST_CTRL           (0x220)
#define SHR_BIST_STATUS         (0x224)
#define SHR_KEY1_ACC_PTR        (0x230)
#define SHR_KEY1_ACC_DW         (0x234)
#define SHR_KEY1_ACC_STATUS     (0x240)
#define SHR_KEY1_ACC_STATUS_PAR (0x248)
#define SHR_KEY1_ACC_STATUS_DW0 (0x24c)
#define SHR_KEY1_ACC_STATUS_DW1 (0x250)
#define SHR_KEY2_ACC_PTR        (0x260)
#define SHR_KEY2_ACC_DW         (0x264)
#define SHR_KEY2_ACC_STATUS     (0x270)
#define SHR_KEY2_ACC_STATUS_PAR (0x278)
#define SHR_KEY2_ACC_STATUS_DW0 (0x27c)
#define SHR_KEY2_ACC_STATUS_DW1 (0x280)
#define SHR_IV_ACC_PTR          (0x290)
#define SHR_IV_ACC_DW           (0x294)
#define SHR_IV_ACC_STATUS       (0x2a0)
#define SHR_IV_ACC_STATUS_PAR   (0x2a8)
#define SHR_IV_ACC_STATUS_DW0   (0x2ac)
#define SHR_IV_ACC_STATUS_DW1   (0x2b0)
#define SHR_LBA_ACC_PTR         (0x2c0)
#define SHR_LBA_ACC_DW0         (0x2c4)
#define SHR_LBA_ACC_DW1         (0x2c8)
#define SHR_LBA_ACC_BCNT        (0x2cc)
#if ELDORA_SOC_VERSION_1_1
    #define SHR_MIS_RNG_EN      (0x2e0)
    #define SHR_MIS_RNG_BYP     (0x2e8)
#elif ELDORA_SOC_VERSION_2_0_UP
    #define SHR_MIS_RNG_WR_EN   (0x2e0)
    #define SHR_MIS_RNG_RD_EN   (0x2e4)
    #define SHR_MIS_RNG_BYP     (0x2e8)
    #define SHR_MIS_RNG_VALID   (0x2ec)
#else
    #error Undefined Eldora SOC Version!
#endif
    #define SHR_MIS_KSIZE0      (0x2f0)
    #define SHR_MIS_KSIZE1      (0x2f4)

//---------------------------------------------- MRE AES

#define RC_AES_CFG              (0x0000)
#define RC_AES_CTRL             (0x0004)
#define RC_AES_STATUS           (0x000c)
#define RC_AES_STREAMSIZE       (0x0010)
#define RC_AES_SKIPSIZE         (0x0014)

#define RC_AES_KEY2_WORD0       (0x0020)
#define RC_AES_KEY2_WORD1       (0x0024)
#define RC_AES_KEY2_WORD2       (0x0028)
#define RC_AES_KEY2_WORD3       (0x002C)
#define RC_AES_KEY2_WORD4       (0x0030)
#define RC_AES_KEY2_WORD5       (0x0034)
#define RC_AES_KEY2_WORD6       (0x0038)
#define RC_AES_KEY2_WORD7       (0x003C)

#define RC_AES_KEY1_WORD0       (0x0040)
#define RC_AES_KEY1_WORD1       (0x0044)
#define RC_AES_KEY1_WORD2       (0x0048)
#define RC_AES_KEY1_WORD3       (0x004C)
#define RC_AES_KEY1_WORD4       (0x0050)
#define RC_AES_KEY1_WORD5       (0x0054)
#define RC_AES_KEY1_WORD6       (0x0058)
#define RC_AES_KEY1_WORD7       (0x005C)

#define RC_AES_IV_WORD0         (0x0060)
#define RC_AES_IV_WORD1         (0x0064)
#define RC_AES_IV_WORD2         (0x0068)
#define RC_AES_IV_WORD3         (0x006C)

#define WC_AES_CFG              (0x00A0)
#define WC_AES_CTRL             (0x00A4)
#define WC_AES_STATUS           (0x00AC)
#define WC_AES_STREAMSIZE       (0x00B0)
#define WC_AES_SKIPSIZE         (0x00B4)

#define WC_AES_KEY2_WORD0       (0x00C0)
#define WC_AES_KEY2_WORD1       (0x00C4)
#define WC_AES_KEY2_WORD2       (0x00C8)
#define WC_AES_KEY2_WORD3       (0x00CC)
#define WC_AES_KEY2_WORD4       (0x00D0)
#define WC_AES_KEY2_WORD5       (0x00D4)
#define WC_AES_KEY2_WORD6       (0x00D8)
#define WC_AES_KEY2_WORD7       (0x00DC)

#define WC_AES_KEY1_WORD0       (0x00E0)
#define WC_AES_KEY1_WORD1       (0x00E4)
#define WC_AES_KEY1_WORD2       (0x00E8)
#define WC_AES_KEY1_WORD3       (0x00EC)
#define WC_AES_KEY1_WORD4       (0x00F0)
#define WC_AES_KEY1_WORD5       (0x00F4)
#define WC_AES_KEY1_WORD6       (0x00F8)
#define WC_AES_KEY1_WORD7       (0x00FC)

#define WC_AES_IV_WORD0         (0x0100)
#define WC_AES_IV_WORD1         (0x0104)
#define WC_AES_IV_WORD2         (0x0108)
#define WC_AES_IV_WORD3         (0x010C)

//---------------------------------------------- OTP
#define OTP_CONTROL             (0x0)
#define OTP_INTERRUPT           (0x4)
#define OTP_INTERRUPT_MASK      (0x8)
#define OTP_ROW_ADDR            (0x10)
#define OTP_LO                  (0x14)
#define OTP_HI                  (0x18)
#define OTP_SEC_TEST            (0x1c)
#define OTP_MATCH_CODE          (0x20)
#define OTP_TIME_CTRL           (0x24)
#define OTP_USER_REG            (0x28)
#define OTP_TEST_CTRL           (0x2c)
#define OTP_VOLT_CTRL           (0x30)
#define OTP_PROG_MUL            (0x34)



#endif

