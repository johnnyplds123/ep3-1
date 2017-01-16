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
//! @brief NVMe I/F spec and NVMe specific HW control header
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define CO_IOCMD_FETCH_MODE         0           // IO command fetch mode0

// HAL Configurations for "Auto CABM Clear En" and "Auto CQ" in a HAL layer
// AUTO_CABM should be disabled to support transfer done callback
#if (_MEDIA_==MEDIA_NONE)
    #define CO_AUTO_CABM_CLEAR      ENABLE      /// use HW Auto CABM clear function
#else
    #define CO_AUTO_CABM_CLEAR      DISABLE     /// don't use HW Auto CABM clear function to support transfer done call back
#endif

#define CO_AUTO_CQ                  ENABLE      /// use HW Auto Completion function

#if (CO_AUTO_CABM_CLEAR==ENABLE) && (CO_AUTO_CQ==DISABLE)
    #error "Invalid HAL configuration (AutoCabmClearEn and Manual CQ)"
#endif

#define CO_RELEASE_CMDS_PER_GROUP   ENABLE      /// release commads per group
//#define CO_READ_COMMAND_INORDER_ACTIVATION_FIFO     ENABLE // (_SOC_REV_>=0x0200)

/// @brief NVMe CQE mode enumeration
typedef enum
{
    cCqeDefault=0,          ///< Default CQE mode
    cCqeHwIo,               ///< HW I/O CQE mode
} NvmeCqeModeId_t;

///////////////////////////////
// NVMe Register configuration

// CAP
static const uint32_t cNvmeCapTo = 0x28;    // 20 sec
static const uint32_t cNvmeCapAms = 0x1;
static const uint32_t cNvmeCapCqr = 0x1;
static const uint32_t cNvmeCapMqes = 511;   // Zero-based
static const uint32_t cNvmeCapMpsmax = 0x3;
static const uint32_t cNvmeCapMpsmin = 0x0;
static const uint32_t cNvmeCapCss = 0x1;
static const uint32_t cNvmeCapNssrs = 0x1;
static const uint32_t cNvmeCapDstrd = 0x0;

// VS - Version 1.1
static const uint32_t cNvmeMajorVersionNumber = 0x0001;
static const uint32_t cNvmeMinorVersionNumber = 0x0200;

// CC
static const uint32_t cNvmeCcIosqes = 6;
static const uint32_t cNvmeCcIocqes = 4;

// Function Queue Allocation Informaiton
static const uint32_t cNvmeCregFun0Ftbi = 0x0;
static const uint32_t cNvmeCregFun0Fts = 0x11;
static const uint32_t cNvmeCregFun1Ftbi = 0x11;
static const uint32_t cNvmeCregFun1Fts = 0x0;
static const uint32_t cNvmeCregFun2Ftbi = 0x12;
static const uint32_t cNvmeCregFun2Fts = 0x0;

// Queue Arbiter Information
static const uint32_t cNvmeQarbFun0HpWeight = 0x2;
static const uint32_t cNvmeQarbFun0MpWeight = 0x2;
static const uint32_t cNvmeQarbFun0LpWeight = 0x2;
static const uint32_t cNvmeQarbFun0ArbBurst = 0x3;

// Data manager
static const uint32_t cNvmeDatmAdSsdszInBytes = 0x1000;

static const uint16_t cAdminQueueId = 0;

static const uint32_t cAuoCabmClrEn = (CO_AUTO_CABM_CLEAR);        // "CABM Auto clear Enable".

static const uint32_t cDefaultFunctionId=0;     // Default function id

// MCQ index allocation for full manual status posting
static const uint32_t cAppMcqIndexMask = 0xFFFFFFF0;  // index 0 to 3 for Application layer
static const uint32_t cHalMcqIndexMask = 0xFFFFFF0F;  // index 4 to 7 for internal HAL to emulate auto EoC

// Resereved admin index to support non-block size I/O command
static const uint16_t cReservedAdminIndexForIoCmd = HW_MAX_ADMIN_CMD_INDEX - 1;
static const uint16_t cReservedIoIndexForAdminCmd = HW_MAX_IO_CMD_INDEX - 1;

static const uint32_t cNvmeCregFuncFtbi[3] = { 0x00, 0x11, 0x12};
static const uint32_t cNvmeCregFuncFts[3]  = { 0x11, 0x00, 0x00};

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define GET_ADM_SQID_FROM_INDEX(I)          (sNvmeAdminXfer[(I)].id.b.SQID)
#define GET_ADM_FID_FROM_INDEX(I)           (sNvmeAdminXfer[(I)].id.b.FID)
#define GET_ADM_CID_FROM_INDEX(I)           (sNvmeAdminXfer[(I)].id.b.CID)
#define GET_ADM_XFER_PARAM_FROM_INDEX(I)    (sNvmeAdminXfer[(I)].xferDoneParam)
#define SET_ADM_XFER_PARAM_FROM_INDEX(I,X)  (sNvmeAdminXfer[(I)].xferDoneParam=(X))
#define GET_IO_SQID_FROM_INDEX(I)           (sNvmeIoXfer[(I)].id.b.SQID)
#define GET_IO_FID_FROM_INDEX(I)            (sNvmeIoXfer[(I)].id.b.FID)
#define GET_IO_CID_FROM_INDEX(I)            (sNvmeIoXfer[(I)].id.b.CID)
#define GET_IO_XFER_PARAM_FROM_INDEX(I)     (sNvmeIoXfer[(I)].xferDoneParam)
#define SET_IO_XFER_PARAM_FROM_INDEX(I,X)   (sNvmeIoXfer[(I)].xferDoneParam=(X))
#define GET_IO_ATTR_FROM_INDEX(I)           (sNvmeIoXfer[(I)].attr.all)
#define SET_IO_ATTR_FROM_INDEX(I,X)         (sNvmeIoXfer[(I)].attr.all=(X))
#define GET_IO_NSID_FROM_INDEX(I)           (sNvmeIoXfer[(I)].nsid)
#define SET_IO_NSID_FROM_INDEX(I,X)         (sNvmeIoXfer[(I)].nsid=(X))
#define GET_EOC_FROM_INDEX(I)               (sNvmeIoXfer[(I)].attr.b.autoEoC)
#define IS_AUTO_EOC(I)                      (GET_EOC_FROM_INDEX(I)==TRUE)

#define GET_COMMAND_INDEX_FROM_MCQID(I)     (NvmeHba.mcqCommandIndex[(I)])
#define SAVE_IO_XFER_ID(I)                  (sNvmeIoXfer[(I)].id.all=*((volatile uint32_t*)(&rNvmeHwIoCmd[(I)])+2))

#define IS_AUTO_CABM_CLEAR_ENABLED()        (cAuoCabmClrEn)
#define IS_ADMCMD_XFER_DATA(I)              (BIT((I)) & rNvmeDataMngr.dataManagerCtrl0.b.DATM_ACMD_CABM)
#define IS_IOCMD_XFER_DONE(I)               (BA_TESTB_U32_L2M(rNvmeBitmap.CABM0, (I)))
#define IS_IOCMD_XFERABLE(I)                (BA_TESTB_U32_L2M(NvmeHba.bitmapXferable, (I)))
#define SET_IOCMD_XFERABLE(I)               (BA_SET_U32_L2M(NvmeHba.bitmapXferable, (I)))
#define CLR_IOCMD_XFERABLE(I)               (BA_CLR_U32_L2M(NvmeHba.bitmapXferable, (I)))
#define SET_CMD_ACTIVE(I)                   (BA_WR1_U32_L2M(rNvmeBitmapW1s.CABM0, (I)))
#define CLR_CMD_ACTIVE(I)                   (BA_WR1_U32_L2M(rNvmeBitmapW1c.CABM0, (I)))
#define IS_CMD_ACTIVATED_FW(I)              (BA_TESTB_U32_L2M(NvmeHba.bitmapFwCabm, (I)))
#define SET_CMD_COMPLETE(I)                 (BA_WR1_U32_L2M(rNvmeBitmapW1s.CCBM, (I)))
#define CLR_CMD_COMPLETE(I)                 (BA_WR1_U32_L2M(rNvmeBitmapW1c.CCBM, (I)))

#define SET_CMD_ACTIVATED(I)                {\
                                            BA_SET_U32_L2M(NvmeHba.bitmapFwCabm, (I)); \
                                            BIT_SET(NvmeHba.bitmapFwCabmGroup, BIT((I)/32)); \
                                            }
#define CLR_CMD_ACTIVATED(I)                {\
                                            BA_CLR_U32_L2M(NvmeHba.bitmapFwCabm, (I)); \
                                            if(NvmeHba.bitmapFwCabm[(I)/32]==0) BIT_CLR(NvmeHba.bitmapFwCabmGroup, BIT((I)/32)); \
                                            }
#define CABM_DW_FW(DW)                      (NvmeHba.bitmapFwCabm[(DW)])
#define CABM_GROUP_BM_FW()                  (NvmeHba.bitmapFwCabmGroup)
#define CABM_DW(DW)                         (rNvmeBitmap.CABM0[(DW)])
#define CABM_GROUP_BM()                     (rNvmeBitmapSram.bitmapSramCmdActivatiBitmapGroupBitmap.b.CABM_0_GBM)
#define CCBM_DW_FW(DW)                      (NvmeHba.bitmapFwCcbm[(DW)])
#define CCBM_DW(DW)                         (rNvmeBitmap.CCBM[(DW)])
#define CCBM_GROUP_BM()                     (rNvmeBitmapSram.bitmapSramCmdCompletiBitmapGroupBitmap.b.CCBM_GBM)

#define DTDBM_DW(DW)                        (rNvmeBitmap.DTDBM[DW])

#define ACTIVATED_DW(DW)                    (CABM_DW(DW))           // Activated bitmaps for a DMA transfer
#define ACTIVATED_GROUP_BM()                (CABM_GROUP_BM())
#define ACTIVATED_DW_FW(DW)                 (CABM_DW_FW(DW))        // Activated FW bitmaps for a DMA transfer
#define ACTIVATED_GROUP_BM_FW()             (CABM_GROUP_BM_FW())

#if (CO_AUTO_CABM_CLEAR==ENABLE)
#define COMPLETED_GROUP_BM()                (CABM_GROUP_BM())
#define COMPLETED_DW(DW)                    (CABM_DW(DW))
#define COMPLETED_DW_FW(DW)                 (~((FW_CABM_DW)(DW)))   // AutoCQ(Completion), ManualCQ (Xfer Done)
#else // (CO_AUTO_CABM_CLEAR==DISABLE)
#define COMPLETED_GROUP_BM()                (CCBM_GROUP_BM())
#define COMPLETED_DW(DW)                    (CCBM_DW(DW))
#define COMPLETED_DW_FW(DW)                 (CCBM_DW_FW(DW))        // AutoCQ(Completion), ManualCQ (Xfer Done)
#endif  // CO_AUTO_CABM_CLEAR

#define GET_COMPLETED_COUNT(N)              ((N)=rNvmeBitmapSram.bitmapSramCmdCompletiBitmapCntr.b.CCBM_CNTR)
#define DECREASE_COMPLETED_COUNT(N)         (rNvmeBitmapSram.bitmapSramCmdCompletiBitmapCntrDecrease.all=(N)) // FW decrease CCBM counter

//#define IS_CQ_CREATED(ID)                   (rNvmeController.queueReg[ID].cq.b.QC)
#define IS_CQ_CREATED(ID)                   ((NvmeHba.cqCreatedBitmap & BIT(ID))!=0)
#define CREATE_CQ(ID)                       BIT_SET(NvmeHba.cqCreatedBitmap, BIT(ID))
#define DELETE_CQ(ID)                       BIT_CLR(NvmeHba.cqCreatedBitmap, BIT(ID))

//#define IS_SQ_CREATED(ID)                   (rNvmeController.queueReg[ID].sq.b.QC)
#define IS_SQ_CREATED(ID)                   ((NvmeHba.sqCreatedBitmap & BIT(ID))!=0)
#define CREATE_SQ(ID)                       BIT_SET(NvmeHba.sqCreatedBitmap, BIT(ID))
#define DELETE_SQ(ID)                       BIT_CLR(NvmeHba.sqCreatedBitmap, BIT(ID))

#define IO_CMD_FIFO_CNT()                   (rNvmeCmdSram.cmdSramFwIOCmdCnt.b.FW_IO_FIFO_CNTR)
#define ADMIN_CMD_FIFO_CNT()                (rNvmeCmdSram.cmdSramFwAdminCmdCnt.b.FW_AD_FIFO_CNTR)

#define CIABM_FREE_CNT()                    (rNvmeBitmapSram.bitmapSramCmdIndexAvailablBitmapCntr.b.CIABM_CNTR)
#define DATA_XFER_DONE_BIT_MAP()            (rNvmeBitmapSram.bitmapSramDataXfrDoneBitmapGroupBitmap.b.DTDBM_GBM)

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief Nvme power mode definition
typedef enum
{
    cNvmePmActive=1,        ///< Active mode - highest power consumption state
    cNvmePmDeepSleep=15     ///< Deep sleep mode - Lowest power consumption state
} NvmePowerMode_t;

typedef union
{
    uint32_t all;
    struct
    {
        uint32_t autoCabmClrEn      : 1;    ///< Command Active Bitmap Clear Enable
        uint32_t cacheEnable        : 1;    ///< Command Active Bitmap Clear Enable
    } b;
} NvmeDeviceControl_t;

typedef uint16_t HldCmdId_t; ///< command Identifier (by HLD)

/// @brief NVMe Admin Command Id
typedef struct
{
    uint32_t cid  :16;  ///<BIT [15:0] Command Identifier
    uint32_t sqid :10;  ///<BIT [25:16] Submission Queue Identifier
    uint32_t fid  : 6;  ///<BIT [31:26] Function Identifier
} AdminCommandId_t;

/// @brief NVMe IO Command Id for mode 1
typedef struct
{
    uint16_t sqid :10;  ///<BIT [9:0] Submission Queue Identifier
    uint16_t fid  : 6;  ///<BIT [15:10] Function Identifier
} NvmeEntryId_t;

/// @brief NVMe Hal Command Ids
typedef union
{
    uint32_t all;   // All bits
    struct
    {
        uint32_t SQID :10;   ///<BIT [9:0] Submission Queue Identifier
        uint32_t FID  : 6;   ///<BIT [15:10] Function Identifier
        uint32_t CID  :16;   ///<BIT [31:16] Command Identifier
    } b;
} NvmeCommandIds_t;

/// @brief NVMe host bus adapter
typedef struct
{
    bool     ready;                                         ///< ready to service
    uint32_t acmdInUseBitmap;                               ///< Admin command in use bitmap
    uint32_t mcqInUseBitmap;                                ///< MCQ in use bitmap, (0 means not used, 1 means that may be used, should look at MCQ_DONE HW register)
    uint32_t cqCreatedBitmap;                               ///< Bitmap for Completion queue created (to minimize register access)
    uint32_t sqCreatedBitmap;                               ///< Bitmap for Submission queue created (to minimize register access)

    uint32_t bitmapFwCabmGroup;                             ///< Group bitmap for CABM activation
    uint32_t bitmapFwCabm[HW_MAX_IO_CMD_INDEX/32];          ///< Bitmap for CABM activation of IO commands to check transfer done
    uint32_t bitmapXferable[HW_MAX_IO_CMD_INDEX/32];        ///< Bitmap for transfer commands
    uint32_t bitmapRepostAdm;                               ///< Bitmap to repost CQ for ADM commands
    uint32_t bitmapRepostIo[HW_MAX_IO_CMD_INDEX/32];        ///< Bitmap to repost CQ for IO commands

    uint32_t mcqFifoBackup[8][4];                           ///< MCQ status manager FIFO data backup to retry in case of error
} NvmeHba_t;
COMPILE_ASSERT(sizeof(NvmeHba_t)==252, "OTF update fail");

/// @brief NVMe Command Common DW 0
typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t OPC:8;         ///< Opcode DW0 bits[7:0]
        uint32_t FUSE:2;        ///< Fused Operation (00=normal, 01=fused 1st command, 10=fused last command, 11=reserved) DW0 bits[9:8]
        uint32_t reserved0:5;   ///< Reserved DW0 bits[14:10]
        uint32_t PSDT:1;        ///< PRP or SGL for Data Transfer(PSDT) DW0 bits[15]
        uint32_t CID:16;        ///< Command Identifier DW0 bits[31:16]
    } b;
} NvmeCommandDw0_t;

/// @brief NVMe Admin Cdb to save in HAL memory (6 Dwords)
typedef struct
{
    NvmeCommandDw0_t DW0;       ///< DW0  <- FW_ACMD.DW1 , Nvme Command Common DW0
    uint32_t NSID;              ///< DW1  <- FW_ACMD.DW2 , Namespace Identifier
    uint64_t MPTR_MSGLP;        ///< DW4-5  <- FW_ACMD.DW5-6, Should be 64-bit aligned
    uint32_t DW10;              ///< DW10 <- FW_ACMD.DW11, Command specific
    uint32_t DW11;              ///< DW11 <- FW_ACMD.DW11, Command specific
} NvmeAdminCdb_t;
COMPILE_ASSERT(sizeof(NvmeAdminCdb_t)==(6*4), "NvmeAdminCdb_t size shall be 6 dwords");

/// @brief NVMe Io Cdb to save in HAL memory (6 Dwords)
typedef struct
{
    NvmeCommandDw0_t dw0;       ///< DW0  <- FW_IOCMD.DW1, Nvme Command Common DW0
    uint32_t NSID;              ///< DW1  <- FW_IOCMD.DW2 , Namespace Identifier
    union                       ///< DW10_11 <-- FW.DW2_3, Should be 64-bit aligned
    {
        uint64_t SLBA;              ///< Start LBA (Use __packed to prevent any padding)
        struct
        {
            uint32_t dw10;          ///< CDW10
            uint32_t dw11;          ///< CDW11
        } b;
    } dw10_11;
    union
    {
        uint32_t all; ///< All bits
        struct
        {
            uint32_t NLB      :16;  ///<BIT [15:00] Number of Logical Blocks
            uint32_t reserved : 9;  ///<BIT [25:16] Reserved
            uint32_t PRINFO   : 4;  ///<BIT [29:26] Protection Information Field
            uint32_t FUA      : 1;  ///<BIT [30] Force Unit Access
            uint32_t LR       : 1;  ///<BIT [31] Limited Retry
        } b;
    } dw12;              ///< DW12 <- FW_IOCMD.DW4 & ~DSM, LR, FUA, PRINFO, NLB
    union
    {
        uint32_t all; /// All bits
        struct
        {
            uint32_t DSM      : 8;  ///<BIT [07:00] Dataset Management
            uint32_t reserved :24;  ///<BIT [31:08] Reserved
        } b;
    } dw13;          ///< DW13 <- FW_IOCMD.DW4.DSM
} NvmeIoCdb_t;
COMPILE_ASSERT(sizeof(NvmeIoCdb_t)==(6*4), "NvmeIoCdb_t size shall be 6 dwords");

/// @brief NVMe Admin Transfer parameters in HAL memory (2 Dwords)
typedef struct
{
    NvmeCommandIds_t id;
    uint32_t xferDoneParam;
} NvmeAdminXfer_t;
COMPILE_ASSERT(sizeof(NvmeAdminXfer_t)==(2*4), "NvmeAdminXfer_t size shall be 2 dwords");

/// @brief NVMe Io Transfer parameters in HAL memory (4 Dwords)
typedef struct
{
    NvmeCommandIds_t id;
    uint32_t         xferDoneParam;
    XferAttr_t       attr;
    uint32_t         nsid;
} NvmeIoXfer_t;
COMPILE_ASSERT(sizeof(NvmeIoXfer_t)==(4*4), "NvmeIoXfer_t size shall be 4 dwords");

/// @brief NVMe Abort Data Xfer info
typedef struct
{
    volatile    uint32_t active;
    volatile    uint32_t Id;
} NvmeAbortInfo_t;

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern NvmeHba_t            NvmeHba;                                ///< NVMe host bus adapter
extern NvmeAdminCdb_t       sNvmeAdminCdb[HW_MAX_ADMIN_CMD_INDEX];  ///< NVMe Hal Admin Cdb format
extern NvmeIoCdb_t          sNvmeIoCdb[HW_MAX_IO_CMD_INDEX];        ///< NVMe Hal Io Cdb format
extern NvmeAdminXfer_t      sNvmeAdminXfer[HW_MAX_ADMIN_CMD_INDEX]; ///< NVMe Hal Admin Xfer parameters
extern NvmeIoXfer_t         sNvmeIoXfer[HW_MAX_IO_CMD_INDEX];       ///< NVMe Hal Io Xfer parameters
extern NvmeAbortInfo_t      gNvmeAbortCmdInfo;
//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t     HalNvme_OneTimeInit(InitBootMode_t initMode);
void        HalNvme_InitAdminQueue(void);
void        HalNvme_SetReadyState(void);
void        HalNvme_Reset(ResetMode_t resetMode);
void        HalNvme_ResetController(void);
void        HalNvme_ResetSubsystem(void);
void        HalNvme_ResetHwLinkList(void);
void        HalNvme_ShutDown(void);
void        HalNvme_ReleaseCommandComplete(void);
void        HalNvme_ProcessCompletedIoCommands(void);
void        HalNvme_ReleaseIoCommand(uint32_t cmdIndex);
void        HalNvme_ReleaseAdminCommand(uint32_t cmdIndex);
Error_t     HalNvme_SetPowerMode(NvmePowerMode_t pmMode);
void        HalNvme_IoCmdAbort(uint16_t CmdIdx, uint32_t GroupId, uint32_t Bitmap);
void        HalNvme_CreateIoSq(uint64_t prp, uint32_t sqId, uint32_t qSize0, uint32_t contiguous, uint32_t priority, uint32_t cqId);
void        HalNvme_CreateIoCq(uint64_t prp, uint32_t cqId, uint32_t qSize0, uint32_t contiguous, uint32_t interruptEnable, uint32_t interruptVector);
void        HalNvme_DeleteIoQueue(bool Cq, uint16_t QId);
Error_t     HalNvme_SetInterruptCoalescing(uint32_t timeUsec, uint32_t threshold);
Error_t     HalNvme_SetInterruptConfiguration(uint32_t vector, uint32_t disable);
Error_t     HalNvme_SetCommandArbitration(uint32_t burst, uint32_t lpw, uint32_t mpw, uint32_t hpw);
uint32_t    HalNvme_GetLbaFormat(uint32_t idx);
void        HalNvme_SetLbaFormat(uint32_t lbaFmt, uint32_t idx);
void        HalNvme_SsdSetup(void);
void        HalNvme_ShutdownComplete(void);

//-----------------------------------------------------------------------------
//  Exported function reference - inline
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
    Find first set or Find first one in a range of specific bits from the start bit.

    It will search one from the start bit in direction from LSB to MSB.
    On platforms with an efficient count leading zeros operation such as ARM and PowerPC,
    FFS can be computed by: ffs(x) = w - clz(x & (-x)).
    w is word size. it is 32 for an uint32_t variable.

    @param[in]  value       the value of data
    @param[in]  highBit     the MSB of the bits range
    @param[in]  lowBit      the LSB of the bits range
    @param[in]  startBit    the start bit to search one, which should be between highBit and lowBit.

    @return     bitPos    0 to 31: the first bit position which is set from LSB.\n
                          0xFFFF:  no bits which is set.
**/
//-----------------------------------------------------------------------------
__inline uint16_t GetFirstSetOfRange(uint32_t value, uint32_t highBit, uint32_t startBit, uint32_t lowBit)
{
    uint16_t bitPos = 0xFFFF;  // Invalid bit position

    // For the performance reason, take over the responsibility of checking startBit to the caller.
    // if(startBit < lowBit || startBit > highBit) { startBit = lowBit; }

    bitPos = FFS_RANGE(value, highBit, startBit); // 0xFFFF is invalid.
    if((bitPos == 0xFFFF) && (startBit != 0))
    {
        bitPos = FFS_RANGE(value, startBit - 1, lowBit);
    }

    return (bitPos);
}

//-----------------------------------------------------------------------------
/**
    Find first set in a range of specific bits from the start bit in reverse directin.

    It will search one from the start bit in direction from MSB to LSB.
    On platforms with an efficient count leading zeros operation such as ARM and PowerPC,
    FFS can be computed by: ffs_rvs(x) = w - clz(x & (-x)).
    w is word size. it is 32 for an uint32_t variable.

    @param[in]  value       the value of data
    @param[in]  highBit     the MSB of the bits range
    @param[in]  lowBit      the LSB of the bits range
    @param[in]  startBit    the start bit to search one, which should be between highBit and lowBit.

    @return     bitPos    0 to 31: the first bit position which is set from LSB.\n
                          0xFFFF:  no bits which is set.
**/
//-----------------------------------------------------------------------------
static __inline uint16_t GetFirstMsbSetOfRange(uint32_t value, uint32_t highBit, uint32_t startBit, uint32_t lowBit)
{
    uint16_t bitPos = 0xFFFF;  // Invalid bit position

    // For the performance reason, take over the responsibility of checking startBit to the caller.
    // if(startBit < lowBit || startBit > highBit) { startBit = lowBit; }

    bitPos = FFS_RANGE_RVS(value, startBit, lowBit);   // 0xFFFF is invalid

    if((bitPos == 0xFFFF) && (startBit != 7))
    {
        bitPos = FFS_RANGE_RVS(value, highBit, startBit + 1);
    }

    return (bitPos);
}

//-----------------------------------------------------------------------------
/**
    Check if any admin command bitmap is available.\n
    There are 8 HW_ACMD entries in HW_CMD_SRAM.\n
    So if 8 bits of all indices are used, then it should be 0xFF.\n

    @return available flag  0 (Not avaiable), 1 (Available)
**/
//-----------------------------------------------------------------------------
static __inline int32_t HalNvme_IsAcmdIndexAvailable(void)
{
    return (NvmeHba.acmdInUseBitmap != (BIT(HW_MAX_ADMIN_CMD_INDEX)-1));
}

//-----------------------------------------------------------------------------
/**
    Check if any io command FIFO is available.\n

    @return available flag
**/
//-----------------------------------------------------------------------------
static __inline int32_t HalNvme_IsIocmdIdAvailable(void)
{
    return (1); // TODO:
}

//-----------------------------------------------------------------------------
/**
    Check if an specific Admin Command is still alive.

    @param[in]  cmdIndex    a command index
    @param[in]  fid         a function identifier from Host
    @param[in]  sqid        a submission queue identifier from Host
    @param[in]  cid         a command identifier from Host

    @return     true        if an Admin command is still alive.\n
                false       if no matched admin command.

**/
//-----------------------------------------------------------------------------
__inline bool HalNvme_IsAdminCommandAlive(uint16_t cmdIndex, uint32_t fid, uint32_t sqid, uint32_t cid)
{
    return ( (GET_ADM_FID_FROM_INDEX(cmdIndex)  == fid) &&
             (GET_ADM_SQID_FROM_INDEX(cmdIndex) == sqid) &&
             (GET_ADM_CID_FROM_INDEX(cmdIndex)  == cid) &&
             (NvmeHba.acmdInUseBitmap & BIT(cmdIndex))
           );
}

//-----------------------------------------------------------------------------
/**
    Lock the bitmap of a given command index.

    @param[in]  cmdIndex    a command index
**/
//-----------------------------------------------------------------------------
static __inline void HalNvme_LockAcmdBitmap(uint16_t cmdIndex)
{
    BIT_SET(NvmeHba.acmdInUseBitmap, BIT(cmdIndex));
}

//-----------------------------------------------------------------------------
/**
    Get a available command index id for an Io command FIFO in manual mode.\n
    There are 256 HW_IO_CMD entries in HW_CMD_SRAM.\n
    So it should be pick up any available one within 256 slots.\n

    @return command Id.\n
                0 to 255: Return an available slot.\n
                0xFFFF: No available slot.\n
**/
//-----------------------------------------------------------------------------
__inline uint16_t HalNvme_GetAvailableManualIocmdId(void)
{
    // TODO: Implement to pick up one bit out of 256 bits.
    return (0xFFFF);
}

//-----------------------------------------------------------------------------
/**
    Lock the bitmap of a given command id and save other parameters for io command mode 1.

    @param[in]  cmdIndex    a command index
    @param[in]  fid         a function identifier from Host
    @param[in]  sqid        a submission queue identifier from Host
**/
//-----------------------------------------------------------------------------
__inline void HalNvme_LockManualIocmdParameters(uint16_t cmdIndex, uint32_t fid, uint32_t sqid)
{

    // TODO: Lock the manual io command index
}

//-----------------------------------------------------------------------------
/**
    Release the bitmap of a given command id and save other parameters.
**/
//-----------------------------------------------------------------------------
__inline void HalNvme_ReleaseManualIocmdBitmap(uint16_t cmdIndex)
{
    // TODO: Release the manual io command index
}
