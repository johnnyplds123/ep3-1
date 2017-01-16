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
//! @brief Host Base class definition
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Conditional compile option
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  constant definitions:
//-----------------------------------------------------------------------------
/// @brief host command state enumeration
typedef enum
{
    cHcsFree=0,            ///< free list
    cHcsXferReady,         ///< transfer ready
    cHcsCmdWait,           ///< wait for trim xfer done
    cHcsAsyncWait,         ///< asynchronous notification wait
    cHcsAsyncReport,
    cHcsFlushWait,
    cHcsCmdAbort,
    cHcsCount
} HcmdStateId_t;

/// @brief host interface state
typedef enum
{
    cHifPowerOn=0,         ///< Power on initialization in progress
    cHifReset,             ///< Reset in progress
    cHifReady,             ///< Interface ready
} HostIfState_t;

/// @brief Host CMD initial mode
typedef enum
{
    cHcIniPowerOn = 0,      ///< Power on initialization in progress
    cHcIniReset,            ///< Reset in progress
} HcInitialMode_t;

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define GetHcmdIndex(pHcmdQ)                (pHcmdQ->index) //ARRAY_INDEX_FROM_PTR(gHcmdQ, pHcmdQ))
#define GetHcmdAddressInfoPtr(pHcmdQ)       (&pHcmdQ->addrInfo)
#define GetHcmdBufDescPtr(pHcmdQ)           (&pHcmdQ->bufDesc)
#define GetHcmdHandler(pHcmdQ)              (pHcmdQ->fptr)
#define SetHcmdHandler(pHcmdQ, handler)     (pHcmdQ->fptr=(HcmdHandler_t)handler)
#define GetHcmdId(pHcmdQ)                   (pHcmdQ->cmdId)
#define GetHcmdMsgPtr(pHcmdQ)               (&smHcmdMsg[GetHcmdIndex(pHcmdQ)])
#define EnableHcmdXferDoneCallBack(pHcmdQ)  (pHcmdQ->bufDesc.callBack=ENABLE)
#define SetHcmdAdjVsbc(pHcmdQ, vsbc)        (pHcmdQ->adjVsbc = vsbc)
#define GetHcmdAdjVsbc(pHcmdQ)              (pHcmdQ->adjVsbc)

#define GetHcmdQuePtr(pHcmdMsg)             (&gHcmdQ[ARRAY_INDEX_FROM_PTR(smHcmdMsg, pHcmdMsg)])
#define AllocHcmdQ()                        (Host_PopFromHead(cHcsFree))
#define ReleaseHcmdQ(pHcmdQ)                (Host_PushToTail(cHcsFree, pHcmdQ))

#define Host_PushToHead(state, pHcmdQ)      Host_PushTo(state, pHcmdQ, true)
#define Host_PushToTail(state, pHcmdQ)      Host_PushTo(state, pHcmdQ, false)
#define Host_PopFromHead(state)             Host_PopFrom(state, true)
#define Host_PopFromTail(state)             Host_PopFrom(state, false)
#define Host_IsEmptyList(state)             (gHostIfMgr.stateList[state].count==0)

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief host command state list
typedef struct
{
    volatile uint32_t count;            ///< elements count in list
    LinkHeadTail_t headTail;            ///< head and tail pointer
} HcmdStateList_t;

typedef void (*HcmdHandler_t)(void*);   ///< Host command handler function pointer

/// @brief host command base attribute
typedef union
{
    uint16_t all;
    struct
    {
        uint16_t perfCmd:1;             ///< performance command
        uint16_t autoEoC:1;             ///< auto EoC (End of Command by HW or Interrupt)
        uint16_t wholeSdb:1;            ///< update whole SDB
        XferDir_t xferDir:1;            ///< data transfer direction (0=D2H, 1=H2D)
        uint16_t xferMode:2;            ///< Interface dependant transfer mode
        uint16_t xferNotify:1;          ///< xfer done event set notification
        uint16_t overlapped:1;          ///< Overlapped command processing
        uint16_t alocCache:1;           ///< allocate data cache
        uint16_t reservedForBase:3;     ///< reserved for base class attributes
        uint16_t reservedForChild:4;    ///< reserved for child class attributes
    } b;
} HcmdAttr_t;

/// @brief command base attributes (see HcmdAttr_t)
#define ATTR_PERF_CMD       0x0001      ///< Performance command
#define ATTR_AUTO_EOC       0x0002      ///< auto EoC (End of Command by HW or Interrupt)
#define ATTR_WHOLE_SDB      0x0004      ///< Update whole SDB
#define ATTR_XFER_H2D       0x0008      ///< Data xfer direction (Host to device)
//---
#define ATTR_GLOBAL_NS      0x0010      ///< Allow global namespace
#define ATTR_XFER_NOTIFY    0x0040      ///< Data transfer done event set notification
#define ATTR_OVERLAPPED     0x0080      ///< Can be overlapped command processing
//---
#define ATTR_ALOC_CACHE     0x0100      ///< Allocate data cache
//---
#define ATTR_LBA64          0x1000      ///< 64 bit LBA
#define ATTR_MDTS           0x2000      ///< need to perform MDTS check

/// @brief NVMe command decoding attributes (see HcmdAttr_t.reservedForBase)
#define ATTR_UNC            0x0200      ///< Ecc Table & WUNC

/// @brief host command queue structure
typedef struct
{
    DlinkEntry_t dlink;                 ///< doubly linked list for state                       8
    uint16_t index;                     ///< index to other data structure                      2
    uint16_t cmdId;                     ///< command identifier from HLD                        2
    volatile    HcmdAttr_t attr;        ///< command attribute                                  2
    int16_t adjVsbc;                    ///< adjust Valid Streaming Block Count (512B unit)     2
    AddressInfo_t addrInfo;             ///< address information                                16
    HcmdQBufDesc_t bufDesc;             ///< buffer descriptor                                  8
    HcmdHandler_t fptr;                 ///< handler function pointer                           4
    uint32_t asycType;                  ///< Asyc Event report type                             4
    uint32_t asycInfo;                  ///< Asyc Event report Information                      4
    uint32_t status;                    ///< command status                                     4
    Error_t error;                      ///< error code                                         4
    uint32_t rsvd;                      ///< alignment 8 bytes
} HcmdQ_t;
COMPILE_ASSERT(sizeof(HcmdQ_t)==64, "OTF update fail");

/// @brief host command statistics
typedef struct
{
    uint32_t cmdEventCnt;               ///< total commands event/isr count
    uint32_t cmdFetchCnt;               ///< total commands fetch count
    uint32_t cmdDoneCnt;                ///< total commands done count
    volatile uint32_t mediaReqJobCnt;   ///< total media request job count
    volatile uint32_t mediaReqDoneCnt;  ///< total media request done count
    uint32_t ioXferReqCnt;              ///< total IO Xfer request count
    uint32_t ioXferDoneCnt;             ///< total IO Xfer done count
    uint32_t currAbortCnt;              ///< current command abort cmd
    uint32_t hardResetCnt;              ///< total hard reset count
    uint32_t softResetCnt;              ///< total soft reset count
    volatile uint32_t adminOverlapCnt;  ///< total admin overlapped count
    uint32_t rsvd;                      ///< reserved
} HcmdStatistics_t;

/// @brief host interface manager
typedef struct
{
    HcmdStatistics_t    info;  ///< host command statistic
    HcmdStateList_t     stateList[cHcsCount]; ///< command state list array
} HostIfMgr_t;
COMPILE_ASSERT(sizeof(HostIfMgr_t)==132, "OTF update fail");

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern HostIfMgr_t gHostIfMgr;      ///< host interface manager
extern HcmdQ_t     gHcmdQ[];        ///< host command queue
extern HcmdQ_t     gSysInfoCmdQ;    ///< SysInfo request

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t  Host_OneTimeInit(InitBootMode_t initMode);
void     Host_Reset(HcInitialMode_t hcIniMode);
HcmdQ_t* Host_PopFrom(HcmdStateId_t state, bool head);
void     Host_PushTo(HcmdStateId_t state, HcmdQ_t* pHcmdQ, bool head);

//-----------------------------------------------------------------------------
/**
    Host fetch command (Base class function)

    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
static __inline void Host_FetchCmd(HcmdQ_t* pHcmdQ)
{
    gHostIfMgr.info.cmdFetchCnt++;
}

//-----------------------------------------------------------------------------
/**
    Host complete command (Base class function)

    @param[in]  pHcmdQ   Host command queue pointer
**/
//-----------------------------------------------------------------------------
static __inline void Host_CommandComplete(HcmdQ_t* pHcmdQ)
{
    gHostIfMgr.info.cmdDoneCnt++;
}

//-----------------------------------------------------------------------------
/**
    Host issue Media Request
**/
//-----------------------------------------------------------------------------
static __inline void Host_MediaReq(void)
{
    gHostIfMgr.info.mediaReqJobCnt++;
}

//-----------------------------------------------------------------------------
/**
    Host Media Request Done
**/
//-----------------------------------------------------------------------------
static __inline void Host_MediaDone(void)
{
    gHostIfMgr.info.mediaReqDoneCnt++;
}

//-----------------------------------------------------------------------------
/**
    Host Media Request All Done check
**/
//-----------------------------------------------------------------------------
static __inline uint32_t Host_MediaAllDone(void)
{
    return gHostIfMgr.info.mediaReqDoneCnt == gHostIfMgr.info.mediaReqJobCnt;
}

//-----------------------------------------------------------------------------
/**
    Host Media Request All Reset
**/
//-----------------------------------------------------------------------------
static __inline void Host_MediaJobReset(void)
{
    gHostIfMgr.info.mediaReqJobCnt = 0;
    gHostIfMgr.info.mediaReqDoneCnt = 0;
}

//-----------------------------------------------------------------------------
/**
    Host issue IO Xfer Request
**/
//-----------------------------------------------------------------------------
static __inline void Host_IoXferReq(void)
{
    gHostIfMgr.info.ioXferReqCnt++;
}

//-----------------------------------------------------------------------------
/**
    Host IO Xfer Request Done
**/
//-----------------------------------------------------------------------------
static __inline void Host_IoXferDone(void)
{
    gHostIfMgr.info.ioXferDoneCnt++;
}

//-----------------------------------------------------------------------------
/**
    Host IO Xfer Reset
**/
//-----------------------------------------------------------------------------
static __inline void Host_IoXferReset(void)
{
    gHostIfMgr.info.ioXferReqCnt = 0;
    gHostIfMgr.info.ioXferDoneCnt = 0;
}

//-----------------------------------------------------------------------------
/**
    Host Media Request All Done check
**/
//-----------------------------------------------------------------------------
static __inline uint32_t Host_XferAllDone(void)
{
    return gHostIfMgr.info.ioXferDoneCnt == gHostIfMgr.info.ioXferReqCnt;
}

//-----------------------------------------------------------------------------
/**
    Host admin command request (Need overlapped case)

**/
//-----------------------------------------------------------------------------
static __inline void Host_AdminCmdOverlapReq(void)
{
    gHostIfMgr.info.adminOverlapCnt++;
}

//-----------------------------------------------------------------------------
/**
    Host admin command done (Need overlapped case)

**/
//-----------------------------------------------------------------------------
static __inline void Host_AdminCmdOverlapDone(void)
{
    gHostIfMgr.info.adminOverlapCnt--;
}

//-----------------------------------------------------------------------------
/**
    Host admin command reset (Need overlapped case)

**/
//-----------------------------------------------------------------------------
static __inline void Host_AdminCmdReset(void)
{
    gHostIfMgr.info.adminOverlapCnt = 0;
}
