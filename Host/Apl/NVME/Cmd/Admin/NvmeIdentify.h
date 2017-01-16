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
//! @brief
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
//#define MODEL_NAME          "MARVELL - " SOC_NICK_NAME " "
#define MODEL_NAME_128      "LITEON " "CX2-8B128" " "
#define MODEL_NAME_256      "LITEON " "CX2-8B256" " "
#define MODEL_NAME_512      "LITEON " "CX2-8B512" " "
#define MODEL_NAME_1024     "LITEON " "CX2-GB1024" " "

#define FW_REVISION_RD      "03"

#define FW_REVISION_128     "487DQ" FW_REVISION_RD " "
#define FW_REVISION_256     "488DQ" FW_REVISION_RD " "
#define FW_REVISION_512     "489DQ" FW_REVISION_RD " "
#define FW_REVISION_1024    "48ADQ" FW_REVISION_RD " "
//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief Identify  Power State Descriptor Data Structure
typedef struct
{
    uint32_t MP:16;            ///< Maximum Power (bits[15:00])
    uint32_t reserved16:8;     ///< Reserved (bits[23:16])
    uint32_t MPS:1;            ///< Max Power Scale (bits[24])
    uint32_t NOPS:1;           ///< Non-Operational State (bits[25])
    uint32_t reserved26:6;     ///< Reserved (bits[31:26])

    uint32_t ENLAT;            ///< Entry Latency (bits[63:32])

    uint32_t EXLAT;            ///< Exit Latency (bits[95:64])

    uint32_t RRT:5;            ///< Relative Read Throughput (bits[100:96])
    uint32_t reserved101:3;    ///< Reserved (bits[103:101])
    uint32_t RRL:5;            ///< Relative Read Latency (bits[108:104])
    uint32_t reserved109:3;    ///< Reserved (bits[111:109])
    uint32_t RWT:5;            ///< Relative Write Throughput (bits[116:112])
    uint32_t reserved117:3;    ///< Reserved (bits[119:117])
    uint32_t RWL:5;            ///< Relative Write Latency (bits[124:120])
    uint32_t reserved125:3;    ///< Reserved (bits[127:125])

    uint8_t reserved128[16];  ///< Reserved (bits[255:128])

} PowerStateDescriptor_t;
COMPILE_ASSERT(sizeof(PowerStateDescriptor_t) == 32, "Power State Descriptor size shall be 32");

/// @brief Identify LBA Format Data Structure, NVM Command Set Specific
typedef struct
{
    uint32_t  MS:16;           ///< Meta data Size (bits[15:00])
    uint32_t  LBADS:8;         ///< LBA Data Size (bits[23:16])
    uint32_t  RP:2;            ///< Relative Performance (bits[25:24])
    uint32_t  reserved26:6;    ///< Reserved (bits[31:26])

} LbaFormatData_t;
COMPILE_ASSERT(sizeof(LbaFormatData_t) == 4, "Lba Format Data size shall be 4");

/// @brief NVMe Identify structure
typedef struct
{
                                        /// Controller Capabilities and Features
    uint16_t VID;                       ///< PCI Vendor ID (bytes[01:00])
    uint16_t SSVID;                     ///< PCI Subsystem Vendor ID ((bytes[03:02])
    uint8_t  SN[20];                    ///< Serial Number (bytes[23:04])
    uint8_t  MN[40];                    ///< Model Number (bytes[63:24])
    uint8_t  FR[8];                     ///< Firmware Revision (bytes[71:64])
    uint32_t RAB:8;                     ///< Recommended Arbitration Burst (bytes[72])
    uint32_t IEEE:24;                   ///< IEEE OUI Identifier (bytes[75:73])

                                                   /// Controller Multi-Path I/O and Namespace Sharing Capabilities (bytes[76])
    uint8_t  CMIC_TWO_OR_MORE_PCIE_PORTS:1;        ///< NVM subsystem may contain two or more physical PCI Express ports (bits[00])
    uint8_t  CMIC_TWO_OR_MORE_PCIE_CONTROLLERS:1;  ///< NVM subsystem may contain two or more controllers (bits[01])
    uint8_t  CMIC_SR_IOV_ASSOCIATED:1;             ///< controller is associated with an SR-IOV Virtual Function (bits[02])
    uint8_t  reserved76:5;                         ///< Reserved (bits[07:02])

    uint8_t  MDTS;                      ///< Maximum Data Transfer Size (bytes[77])
    uint16_t CNTLID;                    ///< Controller ID (bytes[79:78])

    uint32_t VER;                       ///< Version  (bytes[83:80])
    uint32_t RTD3R;                     ///< RTD3R    (bytes[87:84])
    uint32_t RTD3E;                     ///< RTD3E    (bytes[91:88])
    uint8_t  reserved92[164];           ///< Reserved (bytes[255:92])


                                        /// Admin Command Set Attributes & Optional Controller Capabilities
                                        ///< Optional Admin Command Support (bytes[257:256])
    uint16_t  OACS_CTL_SECURITY_SEND_RECEIVE_SUPPORT:1;     ///< controller supports the Security Send and Security Receive commands (bits[00])
    uint16_t  OACS_CTL_FORMAT_NMV_SUPPORT:1;                ///< controller supports the Format NVM command (bits[01])
    uint16_t  OACS_CTL_FIRMWARE_DOWNLOAD_SUPPORT:1;         ///< controller supports the Firmware Activate and Firmware Download commands (bits[02])
    uint16_t  OACS_CTL_NAMESPACE:1;                         ///< controller supports namespace management commands (bits[03])
    uint16_t  OACS_CTL_DEVICE_SELF_TEST:1;                  ///< controller supports the Device Self-test command (bits[04])
    uint16_t  OACS_CTL_DIRECTIVES:1;                        ///< controller supports the Directive command (bits[05])
    uint16_t  reserved256:10;                               ///< Reserved (bits[15:06])

    uint8_t  ACL;                       ///< Abort Command Limit (bytes[258])
    uint8_t  AERL;                      ///< Asynchronous Event Request Limit (bytes[259])

                                                    ///< Firmware Updates (bytes[260])
    uint8_t  FRMW_FIRST_FIRMWARE_SLOT_RD_ONLY:1;    ///< indicates that the first firmware slot (slot 1) is read only (bits[00])
    uint8_t  FRMW_NUM_OF_DEVICE_SUPPORTED:3;        ///< indicate the number of firmware slots that the device supports (bits[03:01])
    uint8_t  FRMW_WITHOUT_RESET_SUPPORTED:1;        ///<indicates that the controller supports firmware activation without a reset(bits[04])
    uint8_t  reserved260:3;               ///< Reserved (bits[07:05])

                                        ///< Log Page Attribute (bytes[261])
    uint8_t  LPA_SUP_SMART:1;           ///< controller supports the SMART / Health information log page on a per namespace basis (bits[00])
    uint8_t  LPA_SUP_COMMAND_EFFECTS:1; ///< controller supports the Command Effects log page  (bits[01])

    uint8_t  reserved261:6;             ///< Log Page Attribute (bits[07:02])

    uint8_t  ELPE;                      ///< Error Log Page Entries (bytes[262])
    uint8_t  NPSS;                      ///< Number of Power States Support (bytes[263])

                                        ///< Admin Vendor Specific Command Configuration (bytes[264])
    uint8_t  AVSCC:1;                   ///< indicates that all Admin Vendor Specific Commands use the format defined in Figure 13 (bits[00])
    uint8_t  reserved264:7;             ///< Reserved (bits[07:01])

                                        ///< Autonomous Power State Transition Attributes (bytes[265])
    uint8_t  APSTA:1;                   ///< controller supports autonomous power state transitions (bits[00])
    uint8_t  reserved265:7;             ///< Reserved (bits[07:01])

    uint16_t  WCTEMP;                   ///< This field indicates the minimum Composite Temperature field value (bytes[267:266])
    uint16_t  CCTEMP;                   ///< This field indicates the minimum Composite Temperature field value (bytes[269:268])
    uint16_t  MTFA;                     ///< This field is specified in 100 millisecond (bytes[271:270])
    uint8_t   reserved270[8];           ///< Reserved                 (bytes[279:272])
    uint64_t  TNVMCAP[2];               ///< Total NVM Capacity       (bytes[295:280])
    uint64_t  UNVMCAP[2];               ///< Unallocated NVM Capacity (bytes[311:296])
    uint8_t   reserved312[4];           ///< Reserved                 (bytes[315:312])

    uint16_t  EDSTT;                    ///< Extended Device Self-test Time (bytes[317:316])
    uint8_t   DSTO;                     ///< Device Self-test Options (bytes[318])
    uint8_t   reserved319[193];         ///< Reserved                 (bytes[511:319])

                                        /// NVM Command Set Attributes
                                        ///< Submission Queue Entry Size (bytes[512])
    uint8_t  SQES_REQUIRED:4;           ///< required Submission Queue Entry size (2^n) (bits[03:00])
    uint8_t  SQES_MAXIMUM:4;            ///< maximum Submission Queue entry size (2^n) (bits[07:04])

                                        ///< Completion Queue Entry Size (bytes[513])
    uint8_t  CQES_REQUIRED:4;           ///< required Completion Queue Entry size (2^n) (bits[03:00])
    uint8_t  CQES_MAXIMUM:4;            ///< maximum Completion Queue entry size (2^n) (bits[07:04])

    uint8_t  reserved514[2];            ///< Reserved (bytes[515:514])
    uint32_t NN;                        ///< Number of Namespaces (bytes[519:516])

                                        ///< Optional NVM Command Support (bytes[521:520])
    uint16_t ONCS_COMPARE_SUPPORT:1;                  ///< supports the Compare command (bits[00])
    uint16_t ONCS_WRITE_UNCORRECTABLE_SUPPORT:1;      ///< supports the Write Uncorrectable command (bits[01])
    uint16_t ONCS_DATASET_MANAGEMENT_SUPPORT:1;       ///< supports the Dataset Management command (bits[02])
    uint16_t ONCS_WRITE_ZEROES_SUPPORT:1;             ///< supports the Write Zeroes command (bits[03])
    uint16_t ONCS_FEATURES_SELECT_SAVE_SUPPORT:1;     ///< supports the Save field in the Set Features command and Select field in Get Features command (bits[04])
    uint16_t ONCS_RESERVATIONS_SUPPORT:1;             ///< supports reservations (bytes[5])
    uint16_t reserved520:10;            ///< Reserved (bits[15:06])

                                        ///< Fused Operation Support (bytes[523:522])
    uint16_t FUSES:1;                   ///< controller supports the Compare and Write fused operation (bits[00])
    uint16_t reserved522:15;            ///< Reserved (bits[15:01])


                                                  ///< Format NVM Attributes (bytes[524])
    uint8_t  FNA_FORMAT_TO_ALL_NAMESPACES:1;      ///< format operation applies to all namespaces  (bits[00])
    uint8_t  FNA_ERASE_TO_ALL_NAMESPACES:1;       ///< cryptographic erase and user data erase functionality apply to all namespaces  (bits[01])
    uint8_t  FNA_CRYPTOGRAPHIC_ERASE_SUPPORT:1;   ///< cryptographic erase is supported as part of the secure erase functionality (bits[02])
    uint8_t  reserved524:5;                       ///< Reserved  (bits[07:03])

                                        /// Volatile Write Cache (bytes[525])
    uint8_t  VWC:1;                     ///< indicates that a volatile write cache is present (bits[00])
    uint8_t  reserved525:7;             ///< Reserved (bits[07:01])

    uint16_t AWUN;                      ///< Atomic Write Unit Normal (bytes[527:526])
    uint16_t AWUPF;                     ///< Atomic Write Unit Power Fail (bytes[529:528])

                                        ///< NVM Vendor Specific Command Configuration (bytes[530])
    uint8_t  NVSCC:1;                   ///< indicates that all NVM Vendor Specific Commands use the format defined in Figure 13 (bits[00])
    uint8_t  reserved530:7;             ///< Reserved (bits[07:01])

    uint8_t  reserved531;               ///< Reserved (bytes[531])
    uint16_t ACWU;                      ///< Atomic Compare & Write Unit (bytes[533:532])
    uint16_t reserved534;               ///< Reserved (bytes[535:534])

                                        ///< SGL Support (bytes[539:536])
    uint32_t CTR_SGL_SUPPORT:1;         ///< controller supports SGLs for the NVM Command Set bit[00])
    uint32_t reserved536:15;            ///< Reserved bit[15:01])
    uint32_t SGL_BIT_BUCKET_SUPPORT:1;  ///< SGL Bit Bucket descriptor support bit[16])
    uint32_t reserved538:15;            ///< Reserved bit[31:17])

    uint8_t  reserved540[164];          ///< Reserved (bytes[703:540])

                                        /// I/O Command Set Attributes
    uint8_t  reserved704[1344];         ///< Reserved (bytes[2047:704])
    PowerStateDescriptor_t PSD[32];     ///< Power State 0-31 Descriptor (bytes[3071:2048])

                                        /// Vendor Specific
    uint8_t VS[207];                    ///< Vendor Specific (bytes[3278:3072])

                                        /// Security features (bytes[3279])
    uint8_t security_ATA_SEC:1;         ///< Security supported
    uint8_t security_SED:1;             ///< Device Encrypts All User Data on the device
    uint8_t security_TCG:1;             ///< Trusted Computing feature set is supported
    uint8_t reserved3279:5;             ///< Reserved bit[7:3])

    uint8_t VS3280[816];                ///< Vendor Specific (bytes[4095:3280])
} NvmeIdentifyController_t;
COMPILE_ASSERT(sizeof(NvmeIdentifyController_t) == 4096, "Identify Controller Data structure size shall be 4096");

/*
    78          Controller Multi-Path I/O and Namespace Sharing Capabilities (CMIC) = 0
    257:256     Optional Admin Command Support (OACS) = 0
                Firmware Activate and Firmware Download commands - not supported
                Format NVM command - not supported
                Security Send and Security Receive commands. - not supported
    258         Abort Command Limit (ACL) = 3
    259         Asynchronous Event Request Limit (AERL) = 3
    261         Log Page Attributes (LPA) = 0
                SMART / Health information log page - not supported
*/

/// @brief Identify Namespace Data Structure, NVM Command Set Specific
typedef struct
{
    uint64_t NSZE;              ///< Namespace Size (bytes[7:0])
    uint64_t NCAP;              ///< Namespace Capacity (bytes[15:8])
    uint64_t NUSE;              ///< Namespace Utilization (bytes[23:16])

                                ///< Namespace Features (bytes[24])
    uint8_t  NSFEAT:1;          ///< Namespace Features (bits[0])
    uint8_t  reserved24:7;      ///< Reserved (bits[07:01])

    uint8_t  NLBAF;             ///< Number of LBA Formats (bytes[25])

                                              /// Formatted LBA Size (bytes[26])
    uint8_t  FLBAS_SUPPORTED_COMBINATION:4;   ///< indicates one of the 16 supported combinations indicated in this data structure (bits[03:00])
    uint8_t  FLBAS_METADATA_LBA_END:1;        ///< indicates that the metadata is transferred at the end of the data LBA (bits[04])
    uint8_t  reserved26:3;                    ///< Reserved (bits[07:05])

                                ///< Metadata Capabilities (bytes[27])
    uint8_t  MC:2;              ///< the namespace supports metadata transferred as part of separate buffer specified in Metadata Pointer (bits[00])
    uint8_t  reserved27:6;      ///< Reserved (bits[07:01])

                                            /// End-to-end Data Protection Capabilities (bytes[28])
    uint8_t  DPC_INFO_TYPE1:1;              ///<  the namespace supports Protection Information Type 1 (bits[00])
    uint8_t  DPC_INFO_TYPE2:1;              ///<  the namespace supports Protection Information Type 2 (bits[01])
    uint8_t  DPC_INFO_TYPE3:1;              ///<  the namespace supports Protection Information Type 3 (bits[02])
    uint8_t  DPC_INFO_FIRST_EIGHT_BYTES:1;  ///<  the namespace supports protection information transferred as first eight-byte metadata (bits[03])
    uint8_t  DPC_INFO_LAST_EIGHT_BYTES:1;   ///<  the namespace supports protection information transferred as last eight-byte metadata (bits[04])
    uint8_t  reserved28:3;                  ///<  Reserved (bits[07:05])

                                ///< End-to-end Data Protection Type Settings (bytes[29])
    uint8_t  PI:3;              ///<  indicate Protection Information is enabled and the type of Protection Information enabled (bits[02:00])
    uint8_t  PIL:1;             ///<  indicates the protection information location, if enabled, is transferred as first eight bytes of metadata (bits[03])
    uint8_t  reserved29:4;      ///<  Reserved (bits[07:04])


                                ///< Namespace Multi-path I/O and Namespace Sharing Capabilities (bytes[30])
    uint8_t  NMIC:1;            ///< Namespace Multi-path I/O and Namespace Sharing Capabilities (bits[00])
    uint8_t  reserved30:7;      ///< Reserved (bits[07:01])

                                                  ///< Reservation Capabilities (bytes[31])
    uint8_t  RESCAP_PERSIST_THROUGH_PWR_LOSS:1;   ///<  the namespace supports the Persist Through Power Loss capability (bits[00])
    uint8_t  RESCAP_WR_EXCLUSIVE:1;               ///<  the namespace supports the Write Exclusive reservation type (bits[01])
    uint8_t  RESCAP_EXCLUSIVE_ACCESS:1;           ///<  the namespace supports the Exclusive Access reservation type (bits[02])
    uint8_t  RESCAP_WR_REGISTRANTS_ONLY:1;        ///<  the namespace supports the Write Exclusive Registrants Only reservation type (bits[03])
    uint8_t  RESCAP_ACCESS_REGISTRANTS_ONLY:1;    ///<  the namespace supports the Exclusive Access Registrants Only reservation type (bits[04])
    uint8_t  RESCAP_WR_ALL_REGISTRANTS:1;         ///<  the namespace supports the Write Exclusive All Registrants reservation type (bits[05])
    uint8_t  RESCAP_ACCESS_ALL_REGISTRANTS:1;     ///<  the namespace supports the Exclusive Access All Registrants reservation type (bits[06])
    uint8_t  reserved31:1;            ///<  (bits[07])

    uint8_t  reserved32[16];    ///< Reserved (bytes[47:32])
    uint64_t NVMCAP[2];         ///< NVM Capacity (bytes[63:48])
    uint8_t  reserved64[56];    ///< Reserved (bytes[119:64])

    uint64_t EUI64;             ///< IEEE Extended Unique Identifier (bytes[127:120])
    LbaFormatData_t LBAF[16];   ///< LBA Format  0 Support (bytes[191:128])
    uint8_t  reserved192[192];  ///< Reservation Capabilities (bytes[192:383])
    uint8_t  VS[3712];         ///< Vendor Specific (bytes[4095:384])

} NvmeIdentifyNamespace_t;
COMPILE_ASSERT(sizeof(NvmeIdentifyNamespace_t) == 4096, "Nvme Identify Namespace size shall be 4096");

/// @brief Identify Active Namespace IDs Structure
typedef struct
{
    uint32_t ACTIVE_NSID[1024]; ////< Active Namespace ID
} NvmeIdentifyActiveNamespaceIds_t;
COMPILE_ASSERT(sizeof(NvmeIdentifyActiveNamespaceIds_t) == 4096, "Nvme Identify Active Namespace Ids size shall be 4096");

//Namespace Management
typedef struct
{
    uint32_t CONTAIN_NSID[1024];
} NvmeIdentifyContainNamespaceIds_t;
COMPILE_ASSERT(sizeof(NvmeIdentifyContainNamespaceIds_t) == 4096, "Nvme Identify Namespace size shall be 4096");

typedef struct
{
    uint16_t NumId;
    uint16_t CNTLID[2047];
} NvmeIdentifyControlList_t;
COMPILE_ASSERT(sizeof(NvmeIdentifyControlList_t) == 4096, "Nvme Identify Namespace size shall be 4096");

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------


