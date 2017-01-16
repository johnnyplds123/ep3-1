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
//! @brief NVMe Command codes
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
// PKG

// REG

// HAL
#include "HldNvme.h"
// APL
#include "NvmeFeatures.h"
#include "HostBase.h"
#include "Cache.h"

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
static const uint32_t cNvmeScratchBufferSize    = (8 * KBYTE);
static const uint16_t cNvmeAdminCmdTableSize    = 24;
static const uint16_t cNvmeIoCmdTableSize       = 32;
static const uint32_t cNvmeGlobalNamespaceId    = 0xFFFFFFFF; // namespace id for all namespaces
static const uint32_t cNvmeDataSetBufSize       = (8 * KBYTE);

#if _MEDIA_==MEDIA_DRAM
    /// @brief NVMe command and feature set support (IDFY 256 BIT(3:0))
    #define CO_SUPPORT_SECURITY                 FALSE
    #define CO_SUPPORT_FORMAT                   FALSE
    #if CHK_FW_TYPE(FW_DNLD)
        #define CO_SUPPORT_FW_DOWNLOAD          TRUE
    #else
        #define CO_SUPPORT_FW_DOWNLOAD          FALSE
    #endif
    #define CO_SUPPORT_NAMESPACE_MANAGEMENT     FALSE
    #define CO_SUPPORT_DIRECTIVES               FALSE
    /// @brief NVMe command and feature set support (IDFY 264 BIT(0))
    #define CO_SUPPORT_VENDORCMD                FALSE
    /// @brief NVMe command and feature set support (IDFY 520)
    #define CO_SUPPORT_COMPARE                  FALSE
    #define CO_SUPPORT_WRITE_UNCORRECTABLE      FALSE
    #define CO_SUPPORT_DATASET_MANAGEMENT       FALSE
    #define CO_SUPPORT_WRITE_ZEROES             FALSE
    #define CO_SUPPORT_FEATURES_SELECT_SAVE     FALSE
    #define CO_SUPPORT_RESERVATIONS             FALSE
    #define CO_SUPPORT_LOGPAGE_PER_NAMESPACE    FALSE
    #define CO_SUPPORT_LOGPAGE_COMMAND_EFFECTS  FALSE
#else
    /// @brief NVMe command and feature set support (IDFY 256 BIT(3:0))
    #define CO_SUPPORT_SECURITY                 FALSE
    #define CO_SUPPORT_FORMAT                   TRUE
    #define CO_SUPPORT_FW_DOWNLOAD              TRUE
    #define CO_SUPPORT_NAMESPACE_MANAGEMENT     TRUE
    #define CO_SUPPORT_DIRECTIVES               FALSE

    /// @brief NVMe command and feature set support (IDFY 264 BIT(0))
    #if (NVME_REGISTER_DUMP == TRUE)
        #define CO_SUPPORT_VENDORCMD            FALSE
    #else
        #define CO_SUPPORT_VENDORCMD            TRUE
    #endif

    /// @brief NVMe command and feature set support (IDFY 520)
    #if CHK_FW_TYPE(FW_AGING)
        #define CO_SUPPORT_WRITE_UNCORRECTABLE  FALSE
        #define CO_SUPPORT_DATASET_MANAGEMENT   FALSE
        #define CO_SUPPORT_WRITE_ZEROES         FALSE
        #define CO_SUPPORT_COMPARE              FALSE
    #else
        #define CO_SUPPORT_WRITE_UNCORRECTABLE  TRUE
        #define CO_SUPPORT_DATASET_MANAGEMENT   TRUE
        #define CO_SUPPORT_WRITE_ZEROES         FALSE
        #define CO_SUPPORT_COMPARE              TRUE
    #endif

    #define CO_SUPPORT_FEATURES_SELECT_SAVE     TRUE
    #define CO_SUPPORT_RESERVATIONS             FALSE
    #define CO_SUPPORT_LOGPAGE_PER_NAMESPACE    TRUE
    #define CO_SUPPORT_LOGPAGE_COMMAND_EFFECTS  TRUE
#endif

#if (_GENERATION_ == GENERATION_CX2)
    #define CO_SUPPORT_APST                     TRUE
#else
    #define CO_SUPPORT_APST                     FALSE
#endif

/// @brief NVMe command and feature set support (IDFY 525)
#define CO_SUPPORT_VOLATILE_WRITE_CACHE         TRUE
#define CO_FUA_WRITE                            FALSE
#define CO_CHECK_PRP_FEATURE_SUPPORT            TRUE

// NVMe FW Image Download Feature
#define BOOT_FW_NOW                             BIT(0)
#define BOOT_FW_AFT_RST                         BIT(1)
#define BOOT_FW_AFT_SUBRST                      BIT(2)
#define BOOT_FW_AFT_RBT                         BIT(3)

/// @brief NVMe command and feature set support (IDFY 3279)
#if ((CO_SUPPORT_SECURITY == TRUE) && (_GENERATION_ == GENERATION_CX2))
    #define CO_SUPPORT_ATA_SECURITY             TRUE
#else
    #define CO_SUPPORT_ATA_SECURITY             FALSE
#endif

#define PS_0_ENTER_US                           0
#define PS_0_EXIT_US                            0
#define PS_1_ENTER_US                           5
#define PS_1_EXIT_US                            5
#define PS_2_ENTER_US                           5
#define PS_2_EXIT_US                            5
#define PS_3_ENTER_US                           5000
#define PS_3_EXIT_US                            5000  //14000
#define PS_4_ENTER_US                           50000
#define PS_4_EXIT_US                            100000

enum
{
    MISC_EVENT_NONE                             = 0,
    MISC_EVENT_FLUSH                            = BIT(0),
    MISC_EVENT_ASYNC                            = BIT(1),
    MISC_EVENT_ABORT                            = BIT(2),
    MISC_EVENT_CMDWAIT                          = BIT(3)
};

#define OTF_MAX_SECTION_COUNT                   15
enum
{
    OTF_SECTION_HEADER = 0,
    OTF_SECTION_HOST,
    OTF_SECTION_NVME,
    OTF_SECTION_HBA,
    OTF_SECTION_NVME_CTRL,
    OTF_SECTION_ASYNC_CMD,
    OTF_SECTION_FW_CMD,
    OTF_SECTION_COUNT
};
COMPILE_ASSERT(OTF_MAX_SECTION_COUNT > OTF_SECTION_COUNT, "Section Overflow");

#define OTF_SECTION_HEADER_SIZE                 sizeof(OTFUpdateInfo_t)
#define OTF_SECTION_HOST_SIZE                   sizeof(HostIfMgr_t)
#define OTF_SECTION_NVME_SIZE                   512 //sizeof(NvmeIfMgr_t)
#define OTF_SECTION_HBA_SIZE                    sizeof(NvmeHba_t)
#define OTF_SECTION_NVME_CTRL_SIZE              sizeof(NvmeControllerSram_t)
#define OTF_SECTION_ASYNC_CMD_SIZE              sizeof(AsyEventBackup_t)
#define OTF_SECTION_UPFW_CMD_SIZE               sizeof(UpdateFWCmdBackup_t)

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define NvmeCmd_GetScratchBufferPointer()       (gNvmeIfMgr.pScratchBuffer)
#define NvmeCmd_GetTrimBufferPointer(cmdid)     (gNvmeIfMgrTrimBuffer + (cmdid * 2048))

// Asy Cmd Macros Use
#define NvmeCmd_ASY_RETURN_INFO_TYPE(IF, TP)    MAKE_U16(IF, TP)
#define NvmeCmd_ASY_GET_INFO(IF)                GET_B15_08(IF)
#define NvmeCmd_ASY_GET_TYPE(TP)                GET_B07_00(TP)
//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief NVMe I/F state definition
typedef enum
{
    cNvmeIfPowerUp          =0x00000000,        ///< Initial Power up inprogress
    cNvmeIfSubsystemReset   =0x00000001,        ///< Sub-system reset inprogress
    cNvmeIfControllerReset  =0x00000002,        ///< Controller Reset inprogress
    cNvmeIfShutdown         =0x00000003,        ///< Shutdown inprogress
    cNvmeHasError           =0x00000004,        ///< Error handle inprogress
    cNvmeIfReady            =0x00000005,        ///< Ready state
    cNvmeIfLast             =0x7fffffff,
} NvmeIfState_t;


/// @brief NVMe I/O Command structure - Read
typedef struct
{
    uint32_t OPC                :8;     ///< Opcode DW0[7:0]
    uint32_t FUSE               :2;     ///< Fused Operation (00=normal, 01=fused 1st command, 10=fused last command, 11=reserved) DW0[9:8]
    uint32_t reserved0          :5;     ///< Reserved DW0[14:10]
    uint32_t PSDT               :1;     ///< PRP or SGL for Data Transfer(PSDT) DW0[15]
    uint32_t CID                :16;    ///< Command Identifier DW0[31:16]

    uint32_t NSID;                      ///< Namespace Identifier (DW1)
    uint32_t reserved2[2];              ///< Reserved DW3..2
    uint64_t MPTR_MSGLP;                ///< Metadata Pointer (DW5..4)
    uint64_t PRP1_SGL1;                 ///< PRP Entry 1 (DW7..6)
    uint64_t PRP2_SGL1;                 ///< PRP Entry 2 (DW9..8)

    uint64_t SLBA;                      ///< Starting LBA (DW11..10)

    uint32_t NLB                :16;    ///< Number of Logical Blocks (DW12 bits[15:00])
    uint32_t reserved12         :10;    ///< Reserved (DW12 bits[25:16])
    uint32_t PRINFO             :4;     ///< Protection Information Field (DW12 bits[29:26])
    uint32_t FUA                :1;     ///< Force Unit Access (DW12 bits[30])
    uint32_t LR                 :1;     ///< Limited Retry (DW12 bits[31])

                                        /// Dataset Management (DSM): attributes for the dataset the LBA(s) being read are associated with
    uint32_t accessFrequency    :4;     ///< DSM Access Frequency  (DW13 bits[03:00])
    uint32_t accessLatency      :2;     ///< DSM Access Latency    (DW13 bits[05:04])
    uint32_t sequentialRequest  :1;     ///< indicates this command is part of sequential write (DW13 bits[06])
    uint32_t incompressible     :1;     ///< indicates data is not compressible (DW13 bits[07])
    uint32_t reserved13         :24;    ///< Reserved (DW13 bits[31:08])

    uint32_t EILBRT;                    ///< (Expected) Initial Logical Block Reference Tag (DW14)

    uint32_t ELBAT              :16;    ///< (Expected) Logical Block Application Tag (DW15 bits[15:0])
    uint32_t ELBATM             :16;    ///< (Expected) Logical Block Application Tag Mask (DW15 bits[31:16])
} NvmeIoRwCommand_t;
COMPILE_ASSERT(sizeof(NvmeIoRwCommand_t)==64, "NVMe read/write command size shall be 64");

/// @brief Security information collection
typedef struct
{
    uint8_t  ErasePrepare;  // 1 Byte
    uint8_t  PasswordCnt;   // 1 Byte
    uint8_t  SecurityState; // 1 Byte
    uint8_t  RetryExpired;  // 1 Byte
} SecurityInfo_t;

typedef struct
{
    uint32_t    rsv1    :3;
    uint32_t    PS      :5;
    uint32_t    IdleMs  :24;
    uint32_t    rsv2;
} NvmeApstEntry_t;

/// @brief NVMe I/F protocol and command service manager
typedef struct
{
    /*___ _____ _____   ____             _                  ____ _____  _    ____ _____
    / _ \_   _|  ___| | __ )  __ _  ___| | ___   _ _ __   / ___|_   _|/ \  |  _ \_   _|
    | | | || | | |_    |  _ \ / _` |/ __| |/ / | | | '_ \  \___ \ | | / _ \ | |_) || |
    | |_| || | |  _|   | |_) | (_| | (__|   <| |_| | |_) |  ___) || |/ ___ \|  _ < | |
    \___/ |_| |_|     |____/ \__,_|\___|_|\_\\__,_| .__/  |____/ |_/_/   \_\_| \_\|_|*/

    // 8 * 32 Bytes                     = 256
    NvmeApstEntry_t                     AutoPST[32];                    ///< Autonomous Power State Transition

    // 4 + 4 + 2 + 2                    = 12
    Error_t                             lastError;                      ///< last error code
    Error_t                             queuedError;                    ///< pending error code, deferred until receive asynch notification command
    uint16_t                            lastErrorByteOffset;            ///< indicate the byte offset of last errored command parameter
    uint16_t                            lastErrorBitOffset;             ///< indicate the bit offet of last errored command paramerer

    // 4 + 4 + 4 + 4 + 2 * 7            = 30
    uint32_t                            asyncEventMask;                 ///< Asynchronous event notification mask
    Dw11ErrorRecovery_t                 errRecovery;
    SecurityInfo_t                      SecurityMgr;                    ///< Security information collection
    NvmeIfState_t                       state;                          ///< NVMe I/F state
    uint16_t                            writeCacheEnable;               ///<
    uint16_t                            readCacheEnable;                ///<
    uint16_t                            currLightTMPTH;                 ///< Light Throttling temperature threshold
    uint16_t                            currHeavyTMPTH;                 ///< Heavy Throttling temperature threshold
    uint16_t                            PM_OperaState;
    uint16_t                            LastNvmeCmd;
    uint16_t                            AsyncEventEnable;               ///< Asynchronous event Enable

    uint8_t                             rsvd1[214];

   /*___ _____ _____   ____             _                  _____ _   _ ____
    / _ \_   _|  ___| | __ )  __ _  ___| | ___   _ _ __   | ____| \ | |  _ \
    | | | || | | |_    |  _ \ / _` |/ __| |/ / | | | '_ \  |  _| |  \| | | | |
    | |_| || | |  _|   | |_) | (_| | (__|   <| |_| | |_) | | |___| |\  | |_| |
    \___/ |_| |_|     |____/ \__,_|\___|_|\_\\__,_| .__/  |_____|_| \_|____/ */

    // 8 * 5                            = 40
    uint64_t                            hostReadCommands;               ///< number of host read commands
    uint64_t                            hostWriteCommands;              ///< number of host write commands
    uint64_t                            dataUnitsRead;                  ///< number of 512 byte data units the host has read
    uint64_t                            dataUnitsWritten;               ///< number of 512 byte data units the host has written
    uint64_t                            UnalignCnt;                     ///< number of unaligen 4KB count

    // 2 * 2 * 2                        = 8
    uint16_t                            currTMPTH[2][2];                ///< current Temperature Threshold [0:Composite,1:Sensor 1][0:Over,1:Under]

    // 4 * 3                            = 12
    uint8_t*                            pScratchBuffer;                 ///< Scratch buffer
    uint16_t                            numOfNamespacesAttached;        ///< number of namespaces attached
    uint16_t                            numOfNamespacesCreated;         ///< number of namespaces created

    // 32 * (4 + 1)                      = 160
    NvmeIfMgrNamespace_t                ns[HW_MAX_NAMESPACES + 1];      ///< namespace table (add additional )

    uint8_t                             rsvd2[292];
} NvmeIfMgr_t;
COMPILE_ASSERT(sizeof(NvmeIfMgr_t)==1024, "Fix to 1024 bytes");

/// @brief NVMe command decoding table
typedef struct
{
    uint32_t            cmdCode:8;      ///< command opcode
    uint32_t            attr:24;        ///< attribute
    HcmdHandler_t       fptr;           ///< command handler function pointer
} NvmeCmdTbl_t;

typedef struct
{
    uint32_t            srcAddr;        ///< address of source content
    uint32_t            backupAddr;     ///< address of backup destination
    uint32_t            size;           ///< backpu section size
} OTFUpdateEntry_t;

typedef struct
{
    uint32_t            count;                          ///< section count
    OTFUpdateEntry_t    entry[OTF_MAX_SECTION_COUNT];
    uint32_t            rsvd[4];
} OTFUpdateInfo_t;
COMPILE_ASSERT(sizeof(OTFUpdateInfo_t) == 200, "OTF update fail");

typedef struct
{
    HcmdQ_t  asyHcmdQ[NVME_ASYNC_EVENT_LIMIT];
    uint32_t asyBackupCnt;
    uint32_t rsv;
} AsyEventBackup_t;
COMPILE_ASSERT(sizeof(AsyEventBackup_t)==264, "Asy Event shall be 264 bytes size");

typedef struct
{
    HcmdQ_t     updateFwHcmdQ;
    NvmeSdb_t   updateFwSdb;
    uint32_t    updateFwTag;
    uint32_t    rsv;
} UpdateFWCmdBackup_t;
COMPILE_ASSERT(sizeof(UpdateFWCmdBackup_t)==88, "Fw Update shall be 88 bytes size");
//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern          NvmeIfMgr_t            gNvmeIfMgr;                    ///< NVMe I/F protocol and command service manager
extern volatile uint32_t               gBootNewFwFlag;
extern volatile uint32_t               gMiscEvnet;
extern          uint32_t               gDetectFwSize;
extern          uint8_t*               gNvmeIfMgrTrimBuffer;
extern          uint8_t                gIfBuffer[4 * KBYTE];
extern          AsyEventBackup_t       gAsyEventBackup;
extern          UpdateFWCmdBackup_t    gUpFwCmdBackup;
extern const    NvmeIfMgrLbaFormat_t   cLBAFTable[HW_MAX_LBA_FORMATS];

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t NvmeCmd_OneTimeInit(InitBootMode_t initMode);
void    NvmeCmd_Reset(ResetMode_t resetMode);
void    NvmeCmd_Complete(HcmdQ_t* pHcmdQ);
void    NvmeCmd_Invalid(HcmdQ_t* pHcmdQ);
void    NvmeCmd_ReturnCacheMsg(HcmdQ_t* pHcmdQ);
bool    NvmeCmd_ProcessOneCommand(void);
void    NvmeCmd_BootFromNewFW(void);
void    NvmeCmd_OTFUpdateInit(void);
void    NvmeCmd_OTFUpdateBackup(void);
void    NvmeCmd_OTFUpdateRestore(void);
void    NvmeCmd_OTFUpdateDump(uint32_t option);

//-----------------------------------------------------------------------------
/**
    NVMe return IO command cache

    @param[in]  pHcmdQ     Host command queue pointer
**/
//-----------------------------------------------------------------------------
__inline void NvmeCmd_ReturnCacheMsg(HcmdQ_t* pHcmdQ)
{
    MsgHostIO_t*        pHcmdMsg;
    MsgCacheRelObj_t*   pCacheMsg;

    pHcmdMsg = (MsgHostIO_t *)GetHcmdMsgPtr(pHcmdQ);

    if (pHcmdMsg->bufDesc.ssdCnt)
    {
        if (pHcmdQ->attr.b.alocCache)
        {
            #if _MEDIA_!=MEDIA_DRAM
            pCacheMsg = (MsgCacheRelObj_t *)IPC_GetCacheMsgPtr();
            pCacheMsg->hdr.b.status = cMsgPosted;
            pCacheMsg->bufDesc      = pHcmdMsg->bufDesc;

            if (pHcmdMsg->bufDesc.cssTemp == CACHE_ABORT)
            {
                pCacheMsg->bufDesc.cssTemp = pHcmdMsg->bufDesc.css;
                pCacheMsg->bufDesc.css     = CACHE_ABORT;
                pCacheMsg->hdr.b.opCode    = pHcmdMsg->hdr.b.opCode;
            }

            IPC_SendMsgQ(cH2cCacheRelQueue, (Msg_t*) pCacheMsg);
            #endif
        }

        pHcmdMsg->bufDesc.ssdCnt = 0;

        if (pHcmdQ->attr.b.autoEoC == TRUE)
        {
            pHcmdMsg->hdr.b.status = cMsgCompleted;
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Get a Host Command Queue pointer using 1 to 1 mapping between command indices and queues

    @param[in]  hldCmdId     Host Hld command identifier

    @return host command queue pointer
**/
//-----------------------------------------------------------------------------
__inline HcmdQ_t* GetHcmdQ(uint16_t cmdId)
{
    HcmdQ_t* pHcmdQ = &gHcmdQ[GET_COMMAND_INDEX(cmdId) + (IS_ADMIN_COMMAND_ID(cmdId) ? HW_MAX_IO_CMD_INDEX : 0)];

    pHcmdQ->cmdId = cmdId;

    return pHcmdQ;
}
