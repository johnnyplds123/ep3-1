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
//  constant definitions:
//-----------------------------------------------------------------------------
#define MSG_QUEUE_NUM            9                                          ///< number of message queue

#define H2M_REQ_QUEUE_DEPTH      4                                          ///< Host to Media request queue depth
#define M2H_COM_QUEUE_DEPTH      4                                          ///< Media to Host completion queue depth
#define H2C_REQ_QUEUE_DEPTH      (MAX_HCMD_Q_COUNT + MAX_OTHER_MSG_Q_COUNT) ///< Host to Core request queue depth (Command + Other Job SMART or ...)
#define C2H_COM_QUEUE_DEPTH      (MAX_HCMD_Q_COUNT + MAX_OTHER_MSG_Q_COUNT) ///< Core to Host completion queue depth (Command + Other Job SMART or ...)
#define C2M_REQ_QUEUE_DEPTH      MAX_HCMD_Q_COUNT                           ///< Core to Media request queue depth
#define M2C_COM_QUEUE_DEPTH      MAX_HCMD_Q_COUNT                           ///< Media to Core completion queue depth
#define H2C_CACHE_QUEUE_DEPTH    MAX_HCMD_Q_COUNT                           ///< Host to Core cache release request queue depth
#define H2C_UART_QUEUE_DEPTH     4                                          ///< Host to Core cache release request queue depth
#define C2M_OTHER_QUEUE_DEPTH    4                                          ///< Host to Core cache release request queue depth

#define MSG_MAX_PRIORITY_BITS    4
#define MSG_MAX_PRIORITY_NUM     ((1 << MSG_MAX_PRIORITY_BITS) - 1)

#define IPC_NO_TIMEOUT           (0xFFFFFFFF)    ///< No time out

#define SYNC_CPU2_W_CPU1_FLAG   0x32575531  // 2WU1
#define SYNC_CPU0_W_CPU2_FLAG   0x30575532  // 0WU2
#define SYNC_CPU1_W_CPU2_FLAG   0x31575532  // 2WU1

#define SYNC_TIMEOUT_US        (120 * 1000 * 1000)

typedef enum
{
    HMSG_EXT_IDX_IDLE_FLUSH = MAX_HCMD_Q_COUNT,
    HMSG_EXT_IDX_SMART,
    HMSG_EXT_IDX_SYSINFO,
    HMSG_EXT_IDX_TCG,
    HMSG_EXT_IDX_VSC,
    HMSG_EXT_IDX_PM,
    HMSG_EXT_IDX_SHUTDOWN,
    HMSG_EXT_IDX_SET_FEATURE,
    HMSG_EXT_IDX_PURGE,
    HMSG_EXT_IDX_HOSTRESET,
    HMSG_EXT_IDX_MAX
} MsgExtendIdx_t;
COMPILE_ASSERT(HMSG_EXT_IDX_MAX < (MAX_HCMD_Q_COUNT + MAX_OTHER_MSG_Q_COUNT), "HMSG_EXT_IDX_MAX > (MAX_HCMD_Q_COUNT + MAX_OTHER_MSG_Q_COUNT)");

/// @brief IPC command
typedef enum
{
    cIpcCmdGoCpu1=3,         ///< Start CPU1 (used by SoC ROM)
    cIpcCmdReadyToRun=4,     ///< Ready to run (all code is loaded and core HW is initialized)
    cIpcCmdMsgIn=0x10,       ///< New message In
    cIpcCmdMsgCompleted,     ///< message completed
} IpcCommand_t;

/// @brief IPC Message delivery status
typedef enum
{
    cMsgReleased=0,     ///< Message buffer released (Free to use)
    cMsgAllocated,      ///< Message buffer allocated
    cMsgPosted,         ///< Message posted by sender
    cMsgReceived,       ///< Message received by receiver
    cMsgExecuted,       ///< Message excuted by receiver --> track done, or IDS active
    cMsgXfered,         ///< Message Host data Xfered
    cMsgCmdAbort,       ///< Message Host data Xfered abort
    cMsgCompleted,      ///< Message completed, sender got completion status
} MsgStatus_t;

/// @brief IPC Message Queue Id
typedef enum
{
    cH2mReqQueue = 0,  ///< Host to Media request queue
    cM2hComQueue,      ///< Media to Host completion queue
    cH2cReqQueue,      ///< Host to Core request queue
    cC2hComQueue,      ///< Core to Host completion queue
    cC2mReqQueue,      ///< Core to Media request queue
    cM2cComQueue,      ///< Media to Core completion queue
    cH2cCacheRelQueue, ///< Core handle interface cache release
    cH2cUartQueue,     ///< Host to Core Uart request queue
    cC2mOtherQueue,    ///< Core to Media Uart request queue
    cMsgPoolLast
} MsgQueueId_t;

/// @brief service function number
typedef enum
{
    cSfnSysBegin,         //  System service message codes
    cSfnDumpInfo,         ///< dump media info
    cSfnWakeup,           ///< wakeup CPU1/2 from PS3/4
    cSfnSysEnd,

    cSfnNfimMsgBegin,     //   NAND Flash interface message codes
    cSfnNfimReadId,       ///< Read Flash Id (direct mode)
    cSfnNfimReadPp,       ///< Read Parameter page (direct mode)
    cSfnNfimRead,         ///< Physical read (direct mode)
    cSfnNfimWrite,        ///< Physical write (direct mode)
    cSfnNfimErase,        ///< Physical erase (direct mode)
    cSfnNfimScanDefect,   ///< ScanDefect   (direct mode)
    cSfnNfimGetf,         ///< Get features (direct mode)
    cSfnNfimSetf,         ///< Set features (direct mode)
	csfnNfimSetVoltageOffset,  ///< Set Voltage Offset (direct mode)
    csfnNfimGetVoltageOffset,///< Get Voltage Offset (direct mode)
    cSfnNfimReset,        ///< Reset (direct mode)
    cSfnNfimPkgcfg,       ///< Change xfer mode
    cSfnHoldCpu1,         ///< Hold Cpu 1
    cSfnNfimMsgEnd,

    cSfnVscMsgBegin,      //   Media Side FW service message codes
    cSfnVscReadIdAll,     ///< Read Flash Id All(direct mode)
    cSfnVscPreformat,     ///< Preformat
    cSfnVscRdNandFlushCnt,///< Read WCS flush count
    cSfnVscReadECTable,   ///< Read EC Table
    cSfnVscReadVCTable,   ///< Read Vac Table
    cSfnVscFwCrc,         ///< Firmware Binary CRC
    cSfnVscSeq,           ///< Seq command
    cSfnVscHwCheckPLP,    ///< Hw Check PLP
    cSfnVscFwConfig,      ///< FW Read Config
    cSfnVscSmBus,         ///< FW Read Config
    cSfnVscMsgEnd,

    cSfnMediaDbgMsgBegin, //   Media debug message codes
    cSfnDumpFioInfo,      ///< Dump fio information
    cSfnDumpRRInfo,       ///< Dump read retry information
    cSfnMediaDbgMsgEnd,

    cSfnCoreDbgMsgBegin,  //   Core debug message codes
    cSfnCacheDisplay,
    cSfnHMSGDisplay,
    cSfnHeapDisplay,
    cSfnFTLDisplay,
    cSfnRMPDisplay,
    cSfnL2PDisplay,
    cSfnVACDisplay,
    cSfnTRIMBMPDisplay,
    cSfnFTLflushDummy,
    cSfnGcDisplay,
    cSfnGcP2lDisplay,
	cSfnGcTest,
    cSfnIdsDisplay,
    csfnPreformat,
    csfnSeqRW,
    cSfnAging,
    cSfnHwCheck,
    cSfnSmBus,
    cSfnSIClear,
    cSfnHoldCpu2,
    cSfnCoreDbgMsgEnd,

    cSfnLast
} SvcFn_t;

/// @brief Host Performance command service group operation code
typedef enum
{
    cMcRead=0,            ///< Read LBA/LMA
    cMcWrite,             ///< Write LBA/LMA
    cMcTrim,              ///< Trim LBA/LMA
    cMcFlush,             ///< Flush
    cMcFuaFlush,          ///< FUA Flush
    cMcIdleFlush,         ///< Idle Flush
    cMcShutdown,          ///< Shutdown notification
    cMcSmartOp,           ///< SMART Operation
    cMcVendor,            ///< Vendor
    cMcSysInfo,           ///< SysInfo
    cMcTcg,               ///< Tcg
    cMcFormat,            ///< Format
    cMcWriteZero,         ///< Write Zero
    cMcCompareCmd,        ///< Compare
    cMcResetCache,        ///< Reset Cache
    cMcHostPowerDown,     ///< Power Down
    cMcHostSetFeature,    ///< Set Feature
    cMcPurge,             ///< Purge
    cMcHostReset,         ///< NVMe Reset
    cMcHostPerfLast,      ///< Last host performance command service group op code indicator
} MsgHostCode_t;

/// @brief Smart sub operation code
typedef enum
{
    cSmrAttri=0,          ///< Collect smart attributes
    cSmrSelfTestIni,      ///< Smart self-test initial
    cSmrSelfTestOp,       ///< Smart self-test operation

} MsgSmrSubCode_t;

/// @brief Smart sub operation code
typedef enum
{
    cSetFeaManagementTemperature = 0,
} MsgSetFeaSubCode_t;

/// @PRP Status
enum
{
    cMcPrpError=0,        ///< for error handle
    cMcPrpReady,          ///< PRP fetch done
    cMcPrpWait            ///<
};

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define MSG_GET_PTR_FROM_HANDLE(HANDLE, MSG_STR)    ((MSG_STR *)(HANDLE))

#define MSG_SET_SERVICE_INIT(MSG, SFN)              {(MSG).hdr.all = 0; (MSG).hdr.b.opCode=SFN; (MSG).hdr.b.nc=1; (MSG).error = cEcNoError;}
#define MSG_SET_HOST_INIT(MSG, OP)                  {(MSG)->hdr.all = 0; (MSG)->hdr.b.opCode=OP; (MSG)->hdr.b.status = cMsgPosted; (MSG)->error = cEcNoError;}
#define MSG_SET_EXTEND_INIT(MSG, OP, NC)            {(MSG)->hdr.all = 0; (MSG)->hdr.b.opCode=OP; (MSG)->hdr.b.nc=NC; (MSG)->hdr.b.status = cMsgPosted; (MSG)->error = cEcNoError;}

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef void* MsgHandle_t;                  ///< message handle

typedef union
{
    uint32_t all;
    struct
    {
        uint32_t opCode:8;                      ///< message operation code
        uint32_t subOpCode:8;                   ///< message sub operation code
        volatile uint32_t status:8;             ///< message transaction status
        uint32_t prty:MSG_MAX_PRIORITY_BITS;    ///< priority for high priority message(ex. SysInfo)
        uint32_t lbaf:2;                        ///< lba format
        uint32_t nc:1;                          ///< no completion
        uint32_t forceAbort:1;                  ///< force abort xfer
    } b;
} MsgHdr_t;
COMPILE_ASSERT(sizeof(MsgHdr_t)==4, "size must be 4 bytes");

/// @brief Generic message structure (32 Bytes)
typedef struct _Msg_t
{
    MsgHdr_t hdr;        ///< message common header
    Error_t  error;      ///< error code
    struct _Msg_t* next; ///< pointer of next msg

    uint32_t param[5];   ///< message code specific parameters 0..4
} Msg_t;

/// @brief SysInfo message
typedef struct _MsgSI_t
{
    MsgHdr_t hdr;        ///< message common header
    Error_t  error;      ///< error code
    struct _Msg_t* next; ///< pointer of next msg

    uint32_t area;
    uint32_t op;
    uint32_t syncby;
    tPAA     quickIdx;
    uint32_t reserved;
} MsgSI_t;
COMPILE_ASSERT(sizeof(MsgSI_t)<=sizeof(Msg_t), "shall be less than Msg_t size");

/// @brief Purge message
typedef struct _MsgPG_t
{
    MsgHdr_t hdr;        ///< message common header
    Error_t  error;      ///< error code
    struct _Msg_t* next; ///< pointer of next msg

    uint32_t reserved1;  ///< align to 8 Byte
    uint64_t pgBmp;
    uint32_t reserved2[2];
} MsgPG_t;
COMPILE_ASSERT(sizeof(MsgPG_t)<=sizeof(Msg_t), "shall be less than Msg_t size");
/// @brief Service message group structure (cMgService.*)
typedef Msg_t MsgService_t;

/// @brief Media Physical Address access (MG_TEST.PHY_READ)
typedef struct
{
    MsgHdr_t hdr;        ///< message common header
    Error_t  error;      ///< error code
    Msg_t*   next;       ///< pointer of next msg
    void*    pBuffer;    ///< buffer pointer
	tPAA     paa;        ///< paa
    uint32_t param[2];   ///< optional parameter
	uint16_t param1;     ///< optional parameter
    uint16_t byteCount;  ///< transfer byte count
} MsgPhyRwe_t;
COMPILE_ASSERT(sizeof(MsgPhyRwe_t)<=sizeof(Msg_t), "shall be less than Msg_t size");

/// @brief Media Physical Address access (MG_TEST.PHY_READ)
typedef struct
{
    MsgHdr_t hdr;        ///< message common header
    Error_t  error;      ///< error code
    Msg_t*   next;       ///< pointer of next msg
    uint16_t opcode;
    uint16_t plane;      ///< single or multi plane
	tPAA     paa;        ///< paa  20B
    uint16_t pairblk[3];   ///< multi plane block
    uint16_t param1;     ///< parameter1
    uint32_t param2;     ///< parameter2
} MsgSeq_t;
COMPILE_ASSERT(sizeof(MsgSeq_t)<=sizeof(Msg_t), "shall be less than Msg_t size");

/// @brief Core VSC message
typedef struct
{
    MsgHdr_t hdr;        ///< message common header
    Error_t  error;      ///< error code
    Msg_t*   next;       ///< pointer of next msg
    void*    pBuffer;    ///< buffer pointer
	tPAA     paa;        ///< paa
    uint32_t param[2];   ///< optional parameter
	uint16_t param1;     ///< optional parameter
    uint16_t byteCount;  ///< transfer byte count
} MsgVsc_t;
COMPILE_ASSERT(sizeof(MsgVsc_t)<=sizeof(Msg_t), "shall be less than Msg_t size");

/// @brief Smart message group structure
typedef struct
{
    MsgHdr_t hdr;        ///< message common header
    Error_t  error;      ///< error code
    Msg_t*   next;       ///< pointer of next msg

    uint32_t subOp;      ///< Smart subOperation code
    uint32_t type;       ///< Selftest type
    uint32_t param[3];   ///< service specific parameter
} MsgSmart_t;
COMPILE_ASSERT(sizeof(MsgSmart_t)<=sizeof(Msg_t), "shall be less than Msg_t size");

/// @brief Buffer descriptor structure
#pragma anon_unions // unnamed union
typedef struct
{
    uint16_t ssdIndex;              ///< SSD index
    uint16_t ssdCnt;                ///< SSD count
    uint16_t laaStartBmp;           ///< start laa bmp
    uint16_t laaEndBmp;             ///< end laa bmp
    uint16_t css;                   ///< cache search status
    union
    {
        uint16_t allFlags;          ///< all flags
        struct
        {
            uint16_t cssTemp:7;     ///< Baukup css for Cache abort condition
            uint16_t flowCtrl:4;    ///< XferFlowCtrl_t flow control (non-streaming, IDS, streamingId1..7)
            uint16_t callBack:1;    ///< buffer fill completion call back
            uint16_t xferSetup:2;   ///< data xfer (see XferSetup_t)
            uint16_t prpRdy:2;      ///< PRP Ready
        };
    };
} BufDesc_t;
COMPILE_ASSERT(sizeof(BufDesc_t)==12, "shall be 12 bytes size");

/// @brief Host media read or write message structure (MsgHostIO_t.*)
typedef struct
{
    MsgHdr_t    hdr;        ///< message common header
    Error_t     error;      ///< error code
    Msg_t*      next;       ///< pointer of next msg

    uint32_t    laa;
    XferAttr_t  attrInfo;
    BufDesc_t   bufDesc;
} MsgHostIO_t;
COMPILE_ASSERT(sizeof(MsgHostIO_t)<=sizeof(Msg_t), "shall be less than Msg_t size");

/// @brief Host write object message structure (cMgHostPerf.*)
typedef struct
{
    MsgHdr_t    hdr;        ///< message common header
    BufDesc_t   bufDesc;
} MsgCacheRelObj_t;
COMPILE_ASSERT(sizeof(MsgCacheRelObj_t)<=sizeof(Msg_t), "shall be less than Msg_t size");

// Standard message structure
typedef struct
{
    MsgHdr_t    hdr;        ///< message common header
    Error_t     error;      ///< error code
    Msg_t*      next;       ///< pointer of next msg

    uint8_t*    pBuffer;    ///< trim buffer address
    uint32_t    entryCnt;   ///< number of trim entry count
    uint32_t    param[3];   ///< reserved
} MsgTrim_t;
COMPILE_ASSERT(sizeof(MsgTrim_t)<=sizeof(Msg_t), "shall be less than Msg_t size");

/// @brief Format message structure
typedef struct
{
    MsgHdr_t    hdr;        ///< message common header
    Error_t     error;      ///< error code
    Msg_t*      next;       ///< pointer of next msg

    uint32_t    laaStart;
    uint32_t    laaEnd;
    uint16_t    SES;        ///< Secure Erase Settings
    uint16_t    LBAF;       ///< LBA format

    uint32_t    param[2];   ///< reserved
} MsgFormat_t;
COMPILE_ASSERT(sizeof(MsgFormat_t)<=sizeof(Msg_t), "shall be less than Msg_t size");

/// @brief Write Zero message structure
typedef struct
{
    MsgHdr_t    hdr;        ///< message common header
    Error_t     error;      ///< error code
    Msg_t*      next;       ///< pointer of next msg

    uint32_t    nb;         ///< host lba address
    uint64_t    lba;        ///< number of blocks
    uint16_t    mbu_shift;  ///< lba to mbu shift (LOG2(MBU / LBA))
    uint16_t    mbu_mask;   ///< lba per mbu mask (MBU / LBA - 1)
    uint32_t    param;      ///< reserved
} MsgWrtZero_t;
COMPILE_ASSERT(sizeof(MsgWrtZero_t)<=sizeof(Msg_t), "shall be less than Msg_t size");

/// @brief Host power down message structure
typedef struct
{
    MsgHdr_t            hdr;        ///< message common header
    Error_t             error;      ///< error code
    Msg_t*              next;       ///< pointer of next msg
    DevicePowerMode_t   powerMode;
    uint32_t            param[4];   ///< test function specific parameter 0..3
} MsgPM_t;
COMPILE_ASSERT(sizeof(MsgPM_t)<=sizeof(Msg_t), "shall be less than Msg_t size");

/// @brief Host media access message structure (MG_TCG_PERF.*)
typedef struct
{
    MsgHdr_t      hdr;        ///< message common header
    Error_t       error;      ///< error code
    Msg_t*        next;       ///< pointer of next msg

    uint16_t      laas;       ///< laaStart
    uint16_t      laae;       ///< laaEnd
    uint16_t      ssdIndex;   ///< SSD index
    PVOID         pBuffer;    ///< buffer pointer
    uint32_t      param[2];   ///< test function specific parameter 1..6
} MsgTcg_t;
COMPILE_ASSERT(sizeof(MsgTcg_t)<=sizeof(Msg_t), "shall be less than Msg_t size");

typedef struct
{
    MsgHdr_t    hdr;        ///< message common header
    Error_t     error;      ///< error code
    Msg_t*      next;       ///< pointer of next msg

    PVOID       pG1Tbl;    ///< G1 Table pointer
    PVOID       pG2Tbl;    ///< G2 Table pointer
    PVOID       pG3Tbl;    ///< G3 Table pointer
    PVOID       pG4Tbl;    ///< G4 Table pointer
    PVOID       pG5Tbl;    ///< G5 Table pointer
} MsgTcgGroup_t;
COMPILE_ASSERT(sizeof(MsgTcgGroup_t)<=sizeof(Msg_t), "shall be less than Msg_t size");

typedef struct
{
    MsgHdr_t      hdr;        ///< message common header
    Error_t       error;      ///< error code
    Msg_t*        next;       ///< pointer of next msg

    PVOID      pTmpBuf;         ///< Temp Buffer pointer
    PVOID      pTmpMBRL2PTbl;      ///< MBR Temp L2P Table pointer
    PVOID      pTmpDSL2PTbl;       ///< DS Temp L2P Table pointer
    PVOID      pLockingRangeTbl;   ///< Locking Range Table pointer
    uint32_t   param[1];   ///< test function specific parameter
} MsgTcgBuffer_t;
COMPILE_ASSERT(sizeof(MsgTcgBuffer_t)<=sizeof(Msg_t), "shall be less than Msg_t size");

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern volatile void*               smpCompMsg;             ///< completion message pointer
extern          Msg_t               smHcmdMsg[];            ///< host command message array
extern          CirBuf_t            smMsgQueue[];
extern          MsgCacheRelObj_t    smCacheRelMsg[];
extern volatile uint32_t            smCacheRelMsgIdx;

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
void        Ipc_OneTimeInit(InitBootMode_t initMode);
Msg_t*      Ipc_GetExtendMsg(uint32_t opCode);

uint32_t    Ipc_GetCount(MsgQueueId_t msgQueueId);
bool        Ipc_IsEmpty(MsgQueueId_t msgQueueId);
bool        Ipc_IsFull(MsgQueueId_t msgQueueId);

MsgHandle_t Ipc_GetMsg(MsgQueueId_t msgQueueId, uint32_t timeOutUsec);
MsgHandle_t Ipc_PeekMsg(MsgQueueId_t msgQueueId, bool remove);
Error_t     Ipc_PutMsg(MsgQueueId_t msgQueueId, MsgHandle_t hMsg, uint32_t timeOutUsec);

MsgHandle_t Ipc_SendMsg(MsgQueueId_t msgQueueId, Msg_t* pReqMsg, uint32_t timeOutUsec);
Error_t     Ipc_WaitCompletion(MsgHandle_t hMsg, Msg_t* pCompMsg, uint32_t timeOutUsec);
Error_t     Ipc_CompleteMsg(MsgHandle_t hMsg, uint32_t timeOutUsec);
Error_t     Ipc_SendWaitMsg(MsgQueueId_t msgQueueId, Msg_t* pReqMsg, Msg_t* pCompMsg, uint32_t timeOutUsec);

void*       Ipc_AllocSharedMem(MsgQueueId_t msgQueueId, uint32_t size);
void        Ipc_ReleaseSharedMem(void* pBuffer);

//-----------------------------------------------------------------------------
//  Inline Function
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
    Peek message count of specific queue number

    @param[in]  Q  queue number
**/
//-----------------------------------------------------------------------------
static __inline U32 IPC_MsgQFastPeek(U32 Q)
{
    return smMsgQueue[Q].writeCount != smMsgQueue[Q].readCount;
}

//-----------------------------------------------------------------------------
/**
    Send message to specific queue

    @param[in]  Q  queue number
    @param[in]  M  pointer of message
**/
//-----------------------------------------------------------------------------
static __inline void IPC_SendMsgQ(uint32_t Q, Msg_t* M)
{
#if _MEDIA_== MEDIA_DRAM
    if (Q == cH2cReqQueue)
    {
        Q = cC2hComQueue;
    }
#endif
    MEM_U32(smMsgQueue[Q].pWrite) = (uint32_t)M;

    smMsgQueue[Q].pWrite += smMsgQueue[Q].entrySize;

    if (smMsgQueue[Q].pWrite >= smMsgQueue[Q].pEnd)
    {
        smMsgQueue[Q].pWrite = smMsgQueue[Q].pBegin;
    }

    smMsgQueue[Q].writeCount++;
}

//-----------------------------------------------------------------------------
/**
    Get message from specific queue

    @param[in]  Q  queue number

    @return     pointer of message
**/
//-----------------------------------------------------------------------------
static __inline Msg_t* IPC_GetMsgQ(uint32_t Q)
{
    Msg_t* M;

    M = (Msg_t*)MEM_U32(smMsgQueue[Q].pRead);

    smMsgQueue[Q].pRead += smMsgQueue[Q].entrySize;

    if (smMsgQueue[Q].pRead >= smMsgQueue[Q].pEnd)
    {
        smMsgQueue[Q].pRead = smMsgQueue[Q].pBegin;
    }

    smMsgQueue[Q].readCount++;

    return M;
}

//-----------------------------------------------------------------------------
/**
    Get release free release cache message

    @return     pointer of message
**/
//-----------------------------------------------------------------------------
static __inline MsgCacheRelObj_t* IPC_GetCacheMsgPtr(void)
{
    MsgCacheRelObj_t* pCacheMsg;
    U32               timeout = 0;

    pCacheMsg = &smCacheRelMsg[smCacheRelMsgIdx];

    while (pCacheMsg->hdr.b.status != cMsgCompleted)
    {
        timeout++;
        ASSERT(timeout < 0xF0000000);
    }

    if (++smCacheRelMsgIdx == HW_MAX_IO_CMD_INDEX) smCacheRelMsgIdx = 0;

    return pCacheMsg;
}
