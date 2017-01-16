//-----------------------------------------------------------------------------
// SoC architecture
//-----------------------------------------------------------------------------
/// @brief CPU Architectures
#define SOC_NAME                        1093
#define SOC_NICK_NAME                   "Eldora"
#define SOC_NUM_OF_CPU                  3

/// @brief CPU types
#define CPU0_TYPE                       "cortex-r5"
#define CPU1_TYPE                       "cortex-r5"
#define CPU2_TYPE                       "cortex-r5"

//-----------------------------------------------------------------------------
// Memory map
//-----------------------------------------------------------------------------
/// @brief CPU0's TCM address
#define CPU0_ITCM_ADDR                  0x00000000
#define CPU0_ITCM_SIZE                  (96*1024)
#define CPU0_DTCM_ADDR                  0x00800000
#define CPU0_DTCM_SIZE                  (64*1024)
#define CPU0_DTCM_WAKEUP_SIZE           (0)

/// @brief CPU1's TCM address
#define CPU1_ITCM_ADDR                  0x00000000
#define CPU1_ITCM_SIZE                  (96*1024)
#define CPU1_DTCM_ADDR                  0x00800000
#define CPU1_DTCM_SIZE                  (64*1024)
#define CPU1_DTCM_WAKEUP_SIZE           (0x200)

/// @brief CPU2's TCM address
#define CPU2_ITCM_ADDR                  0x00000000
#define CPU2_ITCM_SIZE                  (96*1024)
#define CPU2_DTCM_ADDR                  0x00800000
#define CPU2_DTCM_SIZE                  (64*1024)
#define CPU2_DTCM_WAKEUP_SIZE           (0x200)

/// @brief HW TCM address
#define HW_SHARED_TCM_ADDR              0x00900000
#define HW_SHARED_TCM_SIZE              (64*1024)

/// @brief Shared TCM address
#define SHARED_TCM_OTF_FLG_ADDR         (HW_SHARED_TCM_ADDR)
#define SHARED_TCM_OTF_FLG_SIZE         (0x04)
#define SHARED_TCM_OTF_VVG_ADDR         (HW_SHARED_TCM_ADDR + SHARED_TCM_OTF_FLG_SIZE)
#define SHARED_TCM_OTF_VVG_SIZE         (0x04)
#define SHARED_TCM_OTF_INFO_SIZE        (0x20)
#define SHARED_TCM_ADDR                 (HW_SHARED_TCM_ADDR + SHARED_TCM_OTF_INFO_SIZE)
#define SHARED_TCM_SIZE                 (HW_SHARED_TCM_SIZE - SHARED_TCM_OTF_INFO_SIZE)

/// @brief HW IDS address
#define HW_IDS_ADDR                     0x1E800000
#define HW_IDS_SIZE                     (128*1024)
#define MAX_IDS_AU_COUNT                (128)

/// @brief HW SPM address
#define HW_SPM_ADDR                     0x1F000000
#define HW_SPM_SIZE                     (128*1024)

/// @brief shared scratch pad memory address
#define SHARED_SPM_REBOOT_ADDR          (HW_SPM_ADDR)
#define SHARED_SPM_REBOOT_SIZE          (6*1024)
#define SHARED_SPM_ADDR                 (HW_SPM_ADDR+SHARED_SPM_REBOOT_SIZE)
#define SHARED_SPM_SIZE                 (HW_SPM_SIZE-SHARED_SPM_REBOOT_SIZE)

/// @brief BOOT's TCM address
#define BOOT_ITCM_ADDR                  0x0F001000
#define BOOT_ITCM_SIZE                  (64*1024)
#define BOOT_DTCM_ADDR                  (BOOT_ITCM_ADDR + BOOT_ITCM_SIZE)
#define BOOT_DTCM_SIZE                  (HW_SPM_SIZE - BOOT_ITCM_SIZE - SHARED_SPM_REBOOT_SIZE)

/// @brief HW SPM address
#define HW_SPI_OPROM_ADDR               0x210E0000   //Norman for OPROM support

/// @brief DRAM address
#define DIRECT_DRAM_ADDR                0x40000000
#define CACHED_DRAM_ADDR                0x40000000   // Firmware has to define a cached region because ROM doesn't support it

/// @brief Shared TCM AXI mapped address
#define AXI_ATCM_CPU0_ADDR              0x08000000
#define AXI_B0TCM_CPU0_ADDR             0x08800000
#define AXI_B1TCM_CPU0_ADDR             0x08900000
#define AXI_ATCM_CPU1_ADDR              0x0A000000
#define AXI_B0TCM_CPU1_ADDR             0x0A800000
#define AXI_B1TCM_CPU1_ADDR             0x0A900000
#define AXI_ATCM_CPU2_ADDR              0x0C000000
#define AXI_B0TCM_CPU2_ADDR             0x0C800000
#define AXI_B1TCM_CPU2_ADDR             0x0C900000

/// @brief Shared TCM AXI mapped offset
#define AXI_B0TCM_CPU0_OFFSET           0x08000000
#define AXI_B0TCM_CPU1_OFFSET           0x0A000000
#define AXI_B0TCM_CPU2_OFFSET           0x0C000000

/// @brief global monitor register region for spin lock
#define SPIN_LOCK_ADDR                  0xC8000000
#define SPIN_LOCK_SIZE                  (32*0x80)     // total register address size, 32 spin lock ids

//-----------------------------------------------------------------------------
// Sequencer map
//-----------------------------------------------------------------------------
#define MAX_DESC_PER_TRK                64  ///< MAX. # of descriptors per TRK (fixed by TRK_CONF[14:12])
#define MAX_TRK_NUM                     128 ///< MAX  # of Track

//-----------------------------------------------------------------------------
// Firmware Image Define
//-----------------------------------------------------------------------------
#define DFW_HEADER_START_ADDR           0x4000
#define DFW_HEADER_ID                   0x5A
#define DFW_FW_ID                       0x99
#define DFW_FW_IMAGE_TAG                0xED
#define DFW_FW_SELECT_TAG               0xBA

#define DFW_FW_HEADRER_TAG              0x44485746      //FWHD
#define DFW_FW_MAFW_TAG                 0x00005746      //FW00
#define DFW_FW_BKFW_TAG                 0x4B425746      //FWBK
#define DFW_FW_BOOT_TAG                 0x544F4F42      //BOOT

#define DFW_FW_MAX_SIZE                 0xE0000
#define DFW_BOOT_MAX_SIZE               0x10000
#define DFW_MAIN_MAX_SIZE               0x68000
#define DFW_FW_HEAD_SIZE                0x1000                          // 4   KB
#define DFW_FW_DESC_SIZE                sizeof(DfwDescript_t)           // 32  B
#define DFW_FW_SELECT_SIZE              sizeof(DfwSelectTag_t)          // 32  B
#define DFW_FW_MODULE_SIZE              sizeof(DfwModuleInfo_t)         // 32  B
#define DFW_FW_TAG_SIZE                 sizeof(DfwImageTag_t)           // 4   KB
#define DFW_FW_CSS_SIZE                 0x400                           // 1   KB
#define DFW_OPROM_SIZE                  0x14000                         // 80  KB

//----------------------------------------------------
// FW SPI :Boot-Main-OPROM  Offset: (0x0~0xF3FFF)
// FW SPI :FW Header        Offset: (0xF4000~0xF4FFF)
// FW SPI :FW Header Backup Offset: (0xF5000~0xF5FFF)
//----------------------------------------------------
#define DFW_BOOT_OFFSET                 0x00
#define DFW_CURRENT_HEAD_OFFSET         (DFW_FW_MAX_SIZE + DFW_OPROM_SIZE)
#define DFW_BACKUP_HEAD_OFFSET          (DFW_CURRENT_HEAD_OFFSET + DFW_FW_HEAD_SIZE)
//----------------------------------------------------------------------------
// ROM layout : single linear region VS multiple scattered regions
//----------------------------------------------------------------------------
#define CPU0_RESERVED_DRAM              0x00080000          // 512KB reserved for download FW
#define FW_IMAGE_SIZE                   CPU0_RESERVED_DRAM  // 512KB now

#define CPU0_DRAM_LOAD_BASE             (DIRECT_DRAM_ADDR+CPU0_RESERVED_DRAM)
#define CPU0_DRAM_EXE_BASE              (CACHED_DRAM_ADDR+CPU0_RESERVED_DRAM)
#define CPU0_DRAM_ALLOC_SIZE            0x000A0000  // 640KB

#define CPU1_DRAM_LOAD_BASE             (CPU0_DRAM_LOAD_BASE+CPU0_DRAM_ALLOC_SIZE)
#define CPU1_DRAM_EXE_BASE              (CPU0_DRAM_EXE_BASE+CPU0_DRAM_ALLOC_SIZE)
#define CPU1_DRAM_ALLOC_SIZE            0x00020000          // 128KB

#define CPU2_DRAM_LOAD_BASE             (CPU1_DRAM_LOAD_BASE+CPU1_DRAM_ALLOC_SIZE)
#define CPU2_DRAM_EXE_BASE              (CPU1_DRAM_EXE_BASE+CPU1_DRAM_ALLOC_SIZE)
#define CPU2_DRAM_ALLOC_SIZE            0x00020000          // 128KB

#define DRAM_USED_LAST_BASE             (CPU2_DRAM_LOAD_BASE+CPU2_DRAM_ALLOC_SIZE)

//-----------------------------------------------------------------------------
// ARM LIB/Semi-hosting
//-----------------------------------------------------------------------------
#define ARM_LIB_STACKHEAP_SIZE          256     // Semi-hosting print-out messages were corrupted when 256 bytes.
