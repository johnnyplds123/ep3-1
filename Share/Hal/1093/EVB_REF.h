#pragma once
//-----------------------------------------------------------------------------
// Target configuration (EVAL board and referene setting)
//-----------------------------------------------------------------------------
#define HW_TYPE                         HW_TYPE_SOC
#define HW_CLOCK_FREQ_RATIO             1               // actual clock frequency ratio is 1:1
#define FW_CRC_CHECK                    TRUE

//-----------------------------------------------------------------------------
// Clock tree and frequency definitions
//-----------------------------------------------------------------------------
/// @brief  Clock source definition
#define SRC_OSC                         0
#define SRC_TBG_A                       1
#define SRC_TBG_B                       2
#define SRC_TBG_C                       3
#define SRC_TBG_D                       4
#define SRC_CPU                         8
#define SRC_RAB                         9
#define SRC_AXI                         10
#define SRC_UART                        11

/// @brief  OSC and TBG clock frequency definition
#define OSC_FREQ_MHZ                    40
#if (_DRAM_ == DRAM_DDR4)
    #define TBG_A_FREQ_MHZ              1600
#else
    #define TBG_A_FREQ_MHZ              1600
#endif
#define TBG_B_FREQ_MHZ                  500
#define TBG_C_FREQ_MHZ                  200     // FW set frequency based on NAND I/F speed
#define TBG_D_FREQ_MHZ                  1000    // FW set frequency based on NAND I/F speed

/// @brief  MC5 clock definition (DDR = 2 x MC5)
#define MC5_CLOCK_FREQ_MHZ              (TBG_A_FREQ_MHZ / 4)

/// @brief  CPU clock definition
#define CPU_CLOCK_FREQ_MHZ              (TBG_B_FREQ_MHZ)

/// @brief Nand flash clock definition
#define NFLASH_CLOCK_FREQ_MHZ           (TBG_C_FREQ_MHZ / 2)

/// @brief ECU clock definition
#define ECU_CLOCK_FREQ_MHZ              (TBG_D_FREQ_MHZ / 2)

/// @brief  AXI  clock definition
#define AXI_CLOCK_SRC                   SRC_CPU
#define AXI_CLOCK_FREQ_MHZ              (CPU_CLOCK_FREQ_MHZ)

/// @brief TIMER clock definition
#define TIMER_CLOCK_SRC                 SRC_CPU
#define TIMER_CLOCK_FREQ_MHZ            (CPU_CLOCK_FREQ_MHZ / 2)
#define TIMER_TICK_PER_USEC             2

/// @brief UART clock definition
#define UART_CLOCK_SRC                  SRC_CPU
#define UART_CLOCK_FREQ_MHZ             (CPU_CLOCK_FREQ_MHZ / 2)

#if _FW_TYPE_ == FW_DNLD
    #define UART_BAUD_RATE              115200
#else
    #define UART_BAUD_RATE              921600
#endif
/// @brief serial flash/SPI clock definition
#define SFLASH_CLOCK_SRC                SRC_CPU
#define SFLASH_CLOCK_FREQ_MHZ           (CPU_CLOCK_FREQ_MHZ / 2)
#define SFLASH_BAUD_RATE                15

//-----------------------------------------------------------------------------
// HOST - General definitions
//-----------------------------------------------------------------------------
#define MAX_HCMD_Q_COUNT                (HW_MAX_ADMIN_CMD_INDEX + HW_MAX_IO_CMD_INDEX) ///< maximum number of host command queue entry count (Admin+Io Command)
#define MAX_OTHER_MSG_Q_COUNT           16
#define MAX_CACHE_REL_Q_COUNT           HW_MAX_IO_CMD_INDEX

#if _DRAM_ == DRAM_NONE
    #define RAM_DRIVE_BUFFER_SIZE       (16 * KBYTE)
    #define RAM_DRIVE_HBU_SIZE          MBYTE2LBU(1024)
#else
    #define RAM_DRIVE_BUFFER_SIZE       (_DRAM_SIZE_ / 2 * MBYTE)
    #define RAM_DRIVE_HBU_SIZE          MBYTE2LBU(_DRAM_SIZE_ / 2)
#endif

#define REBOOT_BACKUP_DATA_SIZE         (4 * KBYTE)
//-----------------------------------------------------------------------------
// HOST - Logical block definitions
//-----------------------------------------------------------------------------
#define HLBU_DATA_SIZE                  512     ///< host block unit byte size (data only)
#define HLBU_BCRC_SIZE                  0       ///< size of CRC for host data (0 for disabling)
#define HLBU_HLBA_SIZE                  0       ///< HLBA size (0 for disabling)
#define HLBU_MPECC_SIZE                 0       ///< size of MPECC for host data (0 for disabling)
#define HLBU_TOTAL_SIZE                 (HLBU_DATA_SIZE+HLBU_BCRC_SIZE+HLBU_HLBA_SIZE+HLBU_MPECC_SIZE)
#define HLBU_ALIGN_4KB                 ((4096 / HLBU_DATA_SIZE) - 1)
//-----------------------------------------------------------------------------
// HOT - PCIe related definitions
//-----------------------------------------------------------------------------
/// PCI SubSys, Vendor and Device Ids
// Marvell setting
#define MARVELL_PCIE_VENDOR_ID          0x1B4B
#define MARVELL_PCIE_DEVICE_ID_ELDORA   0x1093

// LITEON setting
#define LITEON_PCIE_VENDOR_ID           0x14A4

// introducing new naming rule
// mask & shift
#define LITEON_PCIE_NAMING_N3_MASK      0xF000 // Nibble 3: ASIC ID; ref. document
#define LITEON_PCIE_NAMING_N2_MASK      0x0F00 // Nibble 2: SSD Gen. code; ref. document
#define LITEON_PCIE_NAMING_B0_MASK      0x00FF // Byte   0: customer info; ref. document
#define LITEON_PCIE_NAMING_N3_SHIFT     12
#define LITEON_PCIE_NAMING_N2_SHIFT     8
// rule
#define LITEON_PCIE_NAMING_N3           0x2     // 2 for Eldora
#define LITEON_PCIE_NAMING_N2_GEN2      0x2     // 2 for EP2/CX2
#define LITEON_PCIE_NAMING_N2_GEN3      0x3     // 3 for EP3
#define LITEON_PCIE_NAMING_B0_EP        0xA0    // 0xA0 for enterprise
#define LITEON_PCIE_NAMING_B0_CL        0x00    // 0x00 for OEM/ODM

#if   (_GENERATION_ == GENERATION_EP2)
    #define LITEON_PCIE_NAMING_DID      (LITEON_PCIE_NAMING_B0_EP                                   |\
                                        (LITEON_PCIE_NAMING_N2_GEN2 << LITEON_PCIE_NAMING_N2_SHIFT) |\
                                        (LITEON_PCIE_NAMING_N3      << LITEON_PCIE_NAMING_N3_SHIFT))
#elif (_GENERATION_ == GENERATION_EP3)
    #define LITEON_PCIE_NAMING_DID      (LITEON_PCIE_NAMING_B0_EP                                   |\
                                        (LITEON_PCIE_NAMING_N2_GEN3 << LITEON_PCIE_NAMING_N2_SHIFT) |\
                                        (LITEON_PCIE_NAMING_N3      << LITEON_PCIE_NAMING_N3_SHIFT))
#elif (_GENERATION_ == GENERATION_CX2)
    #define LITEON_PCIE_NAMING_DID      (LITEON_PCIE_NAMING_B0_CL                                   |\
                                        (LITEON_PCIE_NAMING_N2_GEN2 << LITEON_PCIE_NAMING_N2_SHIFT) |\
                                        (LITEON_PCIE_NAMING_N3      << LITEON_PCIE_NAMING_N3_SHIFT))
#endif

#define LITEON_NAMING_RULE              TRUE

#if (LITEON_NAMING_RULE == TRUE)
    #define PCI_SUBSYS_ID               MARVELL_PCIE_DEVICE_ID_ELDORA
    #define PCI_SUBSYS_VENDOR_ID        MARVELL_PCIE_VENDOR_ID
    #define PCI_VENDOR_ID               LITEON_PCIE_VENDOR_ID
    #define PCI_DEVICE_ID               LITEON_PCIE_NAMING_DID
#else
    #define PCI_SUBSYS_ID               0x0100
    #define PCI_SUBSYS_VENDOR_ID        0x1B4B
    #define PCI_VENDOR_ID               0x1B4B
    #define PCI_DEVICE_ID               0x1093
#endif

//-----------------------------------------------------------------------------
// HOST - NVMe parameters and capabilities  limit definitions
//-----------------------------------------------------------------------------
#define NVME_PCIE_ID                    PCI_DEVICE_ID   ///< PCI NVMe controller Id
#define NVME_IEEE_OUI_ID_MARVELL        0x005043        ///< Marvell
#define NVME_IEEE_OUI_ID_LITEON         0x002303        ///< LITE-ON IT Corporation

#define NVME_VER                        0x10200         ///< VER
#define NVME_RTD3R                      0x1E8480        ///< 2 s
#define NVME_RTD3E                      0x1E8480        ///< 2 s

#define NVME_NUM_OF_LBAF                2               ///< Number of LBA Formats by FW
#define NVME_NUM_OF_NAMESPACES          1               ///< Number of Namespaces by FW
#define NVME_ABORT_COMMAND_LIMIT        4               ///< Abort Command Limit (IDFY 258)
#define NVME_ASYNC_EVENT_LIMIT          4               ///< Asynchronous Event Request Limit (IDFY 259)  // ubuntu:6
#define NVME_NUM_OF_FW_SLOTS            2               ///< number of firmware slots (IDFY 260[3:1])
#define NVME_ERROR_LOG_ENTIRIES         64              ///< Error Log Page Entries (IDFY 262)

#if (_GENERATION_ == GENERATION_EP2 || _GENERATION_ == GENERATION_EP3)
    #define NVME_NUM_OF_POWER_STATES    1               ///< Number of Power States Support (IDFY 263)
#else
    #define NVME_NUM_OF_POWER_STATES    5               ///< Number of Power States Support (IDFY 263)
#endif

#define NVME_MAX_DATA_TRANSFER_SIZE     (128 * KBYTE)

#define NVME_WARNING_TEMPERATURE        0x166       ///< 85 C
#define NVME_CRITICAL_TEMPERATURE       0x170       ///< 95 C
#define NVME_NVSCC_USE_OPTION           TRUE
#define NVME_MTFA_LIMIT                 0x32        ///<

#define SET_READY_AFTER_FTL             TRUE
#define CEL2KELVIN_CONVERTION           273
#define CO_SUPPORT_DATA_PROTECTION      FALSE

#define NVME_REGISTER_DUMP              FALSE

//-----------------------------------------------------------------------------
// NVME - Device Self-Test
//-----------------------------------------------------------------------------
#if (_GENERATION_ == GENERATION_CX2)
    #if (_MEDIA_ != MEDIA_DRAM)
        #define CO_SUPPORT_DEVICE_SELF_TEST     TRUE
    #else
        #define CO_SUPPORT_DEVICE_SELF_TEST     FALSE
    #endif
#else
    #define CO_SUPPORT_DEVICE_SELF_TEST         FALSE
#endif

//-----------------------------------------------------------------------------
// Host/Media Logical block definitions
//-----------------------------------------------------------------------------
#if (_NAND_ == NAND_MLC)
    #define TOTAL_AU_SIZE               4436        ///< Total AU SIZE
#elif (_NAND_ == NAND_3DTLCG2)
    #define TOTAL_AU_SIZE               4580        ///< Total AU SIZE
#endif

#define AU_SIZE                         4096        ///< ECU allocation unit size
#define MBU_SIZE                        AU_SIZE     ///< media block unit byte size (data only)
#define META_DATA_MODE                  1           ///< meta data mode(0), aux data mode(1)
#define META_DATA_SIZE                  12          ///< meta/aux data size
#define META_MPECC_SIZE                 0           ///< meta MPECC/CRC size for only meta data mode (0 means disabled)

#define HBU_PER_MBU                     (MBU_SIZE / HLBU_DATA_SIZE)     ///< number of HBUs per MBU
#define HBU_PER_MBU_MASK                (HBU_PER_MBU - 1)               ///< msak of HBU_PER_MBU number

#if HBU_PER_MBU == 8
    #define HBU_PER_MBU_SFT             3                               ///< Log2 of HBU_PER_MBU
#elif HBU_PER_MBU == 16
    #define HBU_PER_MBU_SFT             4                               ///< Log2 of HBU_PER_MBU
#else
    #error "unsupported size"
#endif

#if (META_DATA_MODE == 0)
    #define META_ALL_SIZE               (META_DATA_SIZE + META_MPECC_SIZE)
#else
    #define META_ALL_SIZE               0
#endif

//-----------------------------------------------------------------------------
// BUFFER/MEDIA - Segment and SSD definitions
//-----------------------------------------------------------------------------
#define NUM_OF_FS_SEG                   16
#define NUM_OF_HS_SEG                   8
#define NUM_OF_SSD                      4000        ///< number of SSD for both Host & flash media

//-----------------------------------------------------------------------------
//  Security
//-----------------------------------------------------------------------------
#if (_TCG_ != TCG_NONE)
    #define CO_SUPPORT_BCM                      TRUE
    #define CO_SUPPORT_READSOC 				    FALSE
    #define CO_SUPPORT_PSID                     TRUE

    #if (_TCG_ == TCG_PYRITE)
        #define CO_SUPPORT_AES                  FALSE
        #define CO_SUPPORT_BLOCK_SID_AUTH       TRUE
    #else
        #define CO_SUPPORT_AES                  TRUE
        #define CO_SUPPORT_BLOCK_SID_AUTH       FALSE
    #endif
#else
	#define CO_SUPPORT_AES                      FALSE
	#if (_CODE_TYPE_ == CODE_MAIN) && (CHK_FW_TYPE(FW_AGING))
		#define CO_SUPPORT_BCM                  TRUE
    #else
        #define CO_SUPPORT_BCM                  FALSE
	#endif
	#define CO_SUPPORT_READSOC 					CO_SUPPORT_BCM
#endif

//-----------------------------------------------------------------------------
//  Power Management
//-----------------------------------------------------------------------------
#if (_GENERATION_ == GENERATION_CX2)
    #define CO_SUPPORT_DEVICE_POWER_MANAGEMENT  TRUE
    #define HOST_IF_POWER_MANAGEMENT            FEATURE_SUPPORTED
#else
    #define CO_SUPPORT_DEVICE_POWER_MANAGEMENT  FALSE
    #define HOST_IF_POWER_MANAGEMENT            FEATURE_NOT_SUPPORTED
#endif

//-----------------------------------------------------------------------------
//  Thermal Throttle
//-----------------------------------------------------------------------------
#define THERMAL_THROTTLE                    ENABLE
#define THERMAL_THROTTLE_ADAPTIVE_MODE      DISABLE

#if ((_GENERATION_ == GENERATION_EP2) || (_GENERATION_ == GENERATION_EP3))
    #define THERAML_TEMPERATURE_FROM_SOC    TRUE
#else
    #define THERAML_TEMPERATURE_FROM_SOC    FALSE
#endif

#if ((_DRAM_ & DRAM_TYPE_MASK) == DRAM_LPDDR)
    #define TT_DEGREE_MT_0              70
    #define TT_DEGREE_MT_1              73
    #define TT_DEGREE_MT_2              78
    #define TT_DEGREE_OFFSET            3
    #define TT_DEGREE_CRIT              85
    #define TT_DEGREE_WARN              80
    #define TT_ADAPTIVE_LEVEL           6
#elif ((_DRAM_ & DRAM_TYPE_MASK) == DRAM_DDR)
    #define TT_DEGREE_MT_0              85
    #define TT_DEGREE_MT_1              115
    #define TT_DEGREE_MT_2              120
    #define TT_DEGREE_OFFSET            5
    #define TT_DEGREE_CRIT              140
    #define TT_DEGREE_WARN              130
    #define TT_ADAPTIVE_LEVEL           6
#endif

//-----------------------------------------------------------------------------
//  Constants definitions: Common Settings
//-----------------------------------------------------------------------------
#if (_MEDIA_ < MEDIA_NAND)
    #define CO_SUPPORT_EARLY_MOVE_BLK           FALSE
    #define CO_SUPPORT_PLP                      FALSE
    #define CO_SUPPORT_IDLEFLUSH                FALSE
#else
    #if (_GENERATION_ == GENERATION_EP2)
        #define CO_SUPPORT_EARLY_MOVE_BLK       FALSE
        #define CO_SUPPORT_PLP                  TRUE
        #define CO_SUPPORT_IDLEFLUSH            TRUE
    #elif (_GENERATION_ == GENERATION_EP3)
        #define CO_SUPPORT_EARLY_MOVE_BLK       FALSE
        #define CO_SUPPORT_PLP                  TRUE
        #define CO_SUPPORT_IDLEFLUSH            TRUE
    #else
        #define CO_SUPPORT_EARLY_MOVE_BLK       TRUE
        #define CO_SUPPORT_PLP                  FALSE
        #define CO_SUPPORT_IDLEFLUSH            TRUE
    #endif
#endif

//-----------------------------------------------------------------------------
//  Experimental Features
//-----------------------------------------------------------------------------
#define TOGGLE_533                          FALSE
#define FIO_JOB_HEAP_MEMORY                 FALSE
#define DESC_SUPPORT_MULTI_TRK_PER_JOB      TRUE
#if (_FW_TYPE_ != FW_AGING)
    #define CO_SUPPORT_NAND_BUSYTIME        FALSE
#else
    #define CO_SUPPORT_NAND_BUSYTIME        TRUE
#endif

#if (_NAND_ == NAND_3DTLCG2 && _FW_TYPE_ != FW_AGING)
    #define CO_SUPPORT_PATROL_READ          TRUE
#else
    #define CO_SUPPORT_PATROL_READ          FALSE
#endif

#if (FIO_JOB_HEAP_MEMORY == FALSE)
    #define ASM_DMB()            __asm { DMB }
#else
    #define ASM_DMB()
#endif
