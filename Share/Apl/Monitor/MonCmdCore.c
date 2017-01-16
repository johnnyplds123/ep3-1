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
//! @brief Test CORE monitor command
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"
#include "SysInfo.h"
#include "TcgCommon.h"
#include "TcgNfIf.h"
#include "UncTable.h"
#include "Purge.h"
#include "SmBus.h"

#include "NormSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define SUPPORT_UART_SET_SN     TRUE

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static Error_t MonCmdCore_DumpInfo(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdCore_Cache(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdCore_CoreMsg(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdCore_Heap(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdCore_Ftl(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdCore_Rmp(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdCore_L2P(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdCore_Vac(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdCore_TrimBmp(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdCore_FtlFlushDummy(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdCore_Gc(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdCore_GcP2L(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdCore_GcTest(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdCore_Ids(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdCore_Format(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdCore_Seq(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdCore_SysInfo(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdCore_RemoveSysInfo(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdCore_Qbt(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdCore_SetSn(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdCore_TcgEraDefTbl(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdCore_TcgRngLock(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdCore_Aging(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdCore_KillFtl(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdCore_ClearNand(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdCore_Purge(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdCore_SmBus(Cstr_t pCmdStr, U32 argc, U32 argv[]);
//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
const MonCmdTbl_t monCoreCmdList[] =
{
//===============================================================================================
//    cmd string,   help string,                         min-argc, auto-dec, exe_time, handeler
//...............................................................................................
    { "CINFO",      "Dump Core Info",                       0,  ON,  OFF, MonCmdCore_DumpInfo       },
    { "DC",         "Dump Cache",                           0,  ON,  OFF, MonCmdCore_Cache          },
    { "DHM",        "Dump Core HostMsgQ",                   0,  ON,  OFF, MonCmdCore_CoreMsg        },
    { "DH",         "Dump Heap",                            0,  ON,  OFF, MonCmdCore_Heap           },
    { "DF",         "Dump FTL",                             0,  ON,  OFF, MonCmdCore_Ftl            },
    { "RMP",        "Dump Remap",                           0,  ON,  OFF, MonCmdCore_Rmp            },
    { "L2P",        "Dump L2P",                             0,  ON,  OFF, MonCmdCore_L2P            },
    { "VAC",        "Dump VAC",                             0,  ON,  OFF, MonCmdCore_Vac            },
    { "TRIMBMP",    "Dump Trim Bitmap",                     0,  ON,  OFF, MonCmdCore_TrimBmp        },
    { "FDUMY",      "Flush Dummy",                          0,  ON,  OFF, MonCmdCore_FtlFlushDummy  },
    { "DGC",        "Dump GC",                              0,  ON,  OFF, MonCmdCore_Gc             },
    { "DGCP2L",     "Dump GC P2L Table",                    0,  ON,  OFF, MonCmdCore_GcP2L          },
    { "GCTest",     "GC Test",                              0,  ON,  OFF, MonCmdCore_GcTest         },
    { "DIDS",       "Dump IDS",                             0,  ON,  OFF, MonCmdCore_Ids            },
    { "FORMAT",     "Format",                               0,  ON,  OFF, MonCmdCore_Format         },
    { "SEQ",        "Sequener",                             7,  ON,  OFF, MonCmdCore_Seq            },
    { "QBT",        "Save QBT",                             0,  ON,  OFF, MonCmdCore_Qbt            },
    { "SSN",        "Set SN",                               4,  ON,  OFF, MonCmdCore_SetSn          },
    { "RMSI",       "SysInfo Remove",                       0,  ON,  OFF, MonCmdCore_RemoveSysInfo  },
    { "SYIN",       "SysInfo Operate",                      0,  ON,  OFF, MonCmdCore_SysInfo        },
    { "KFTL",       "Kill FTL",                             0,  ON,  OFF, MonCmdCore_KillFtl        },
    { "CN",         "Clear NAND",                           0,  ON,  OFF, MonCmdCore_ClearNand      },
    { "PG",         "Purge",                                0,  ON,  OFF, MonCmdCore_Purge          },
    { "SMBUS",      "SMBus",                                0,  ON,  OFF, MonCmdCore_SmBus          },
#if _TCG_ != TCG_NONE
    { "TCGDE",      "0.Show Era 1.Clr Era/Def",             1,  ON,  OFF, MonCmdCore_TcgEraDefTbl   },
    { "TCGRL",      "0.Show RngLock 1.Set RngLock",         1,  ON,  OFF, MonCmdCore_TcgRngLock     },
#endif
    { "AGING",      "Aging Run Test",                       3,  ON,  OFF, MonCmdCore_Aging          },
    // The last line (DON'T CHANGE BELOW).
    { "",           "",                                     0,  OFF, OFF, NULL                      }   // EOT
};

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------
extern void TcgPreformatAndInit(U8 IsTcgInit);
extern void TcgKeepGroupDataBridge(U32 GrpNum);
extern void TcgEepOperatedBridge(U32 Metholdtype);
extern void TcgEepBackupBridge(void);
extern void TcgEepRestoreBridge(void);
extern void TcgPsidBackupBridge(void);
extern void TcgPsidRestoreBridge(void);
extern int  TcgH2cJobIssue(tMSG_TCG_SUBOP subOP, MsgTcg_t* pMyTcgMsg);
extern void DumpRangeInfo(void);
extern void DumpG4ErasedCount(void);
extern void DumpG5ErasedCount(void);
extern void DumpTcgEepInfo(void);

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
    Core Monitor command handler - Display Core Info

    CINFO

    @param[in]  pCmdStr     command line string
    @param[in]  argc        argument count if auto-decode is enabled
    @param[in]  argv        argument values if auto-decode is enabled


    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmdCore_DumpInfo(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgService_t msg;

    MSG_SET_SERVICE_INIT(msg, cSfnDumpInfo);

    Ipc_SendWaitMsg(cH2cUartQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Core Monitor command handler - Display Cache information.

    DC [level][HarshLvel][ResetCache]

    @param[in]  pCmdStr     command line string
    @param[in]  argc        argument count if auto-decode is enabled
    @param[in]  argv        argument values if auto-decode is enabled
                            argv[0] : level
                            argv[1] : HarshLvel
                            argv[2] : ResetCache

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmdCore_Cache(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgService_t msg;

    MEM_CLR(&msg, sizeof(msg));

    MSG_SET_SERVICE_INIT(msg, cSfnCacheDisplay);

    msg.param[0] = argv[0];   // level
    msg.param[1] = argv[1];   // HarshLvel
    msg.param[2] = argv[2];   // ResetCache

    Ipc_SendWaitMsg(cH2cUartQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Core Monitor command handler - Display Host Msg Q in Core information.

    DHM

    @param[in]  pCmdStr     command line string
    @param[in]  argc        argument count if auto-decode is enabled
    @param[in]  argv        argument values if auto-decode is enabled

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmdCore_CoreMsg(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgService_t msg;

    MEM_CLR(&msg, sizeof(msg));

    MSG_SET_SERVICE_INIT(msg, cSfnHMSGDisplay);

    Ipc_SendWaitMsg(cH2cUartQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Core Monitor command handler - Display Heap information.

    DH

    @param[in]  pCmdStr     command line string
    @param[in]  argc        argument count if auto-decode is enabled
    @param[in]  argv        argument values if auto-decode is enabled

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmdCore_Heap(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgService_t msg;

    MEM_CLR(&msg, sizeof(msg));

    MSG_SET_SERVICE_INIT(msg, cSfnHeapDisplay);

    Ipc_SendWaitMsg(cH2cUartQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Core Monitor command handler - Display FTL information.

    DF

    @param[in]  pCmdStr     command line string
    @param[in]  argc        argument count if auto-decode is enabled
    @param[in]  argv        argument values if auto-decode is enabled

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmdCore_Ftl(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgService_t msg;

    MEM_CLR(&msg, sizeof(msg));

    MSG_SET_SERVICE_INIT(msg, cSfnFTLDisplay);

    Ipc_SendWaitMsg(cH2cUartQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Core Monitor command handler - Display FTL information.

    RMP

    @param[in]  pCmdStr     command line string
    @param[in]  argc        argument count if auto-decode is enabled
    @param[in]  argv        argument values if auto-decode is enabled

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmdCore_Rmp(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgService_t msg;

    MEM_CLR(&msg, sizeof(msg));

    MSG_SET_SERVICE_INIT(msg, cSfnRMPDisplay);

    Ipc_SendWaitMsg(cH2cUartQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Core Monitor command handler - Display L2P information.

    L2P [laa][cnt]

    @param[in]  pCmdStr     command line string
    @param[in]  argc        argument count if auto-decode is enabled
    @param[in]  argv        argument values if auto-decode is enabled
                            argv[0] : laa
                            argv[1] : cnt

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmdCore_L2P(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgService_t msg;

    MEM_CLR(&msg, sizeof(msg));

    MSG_SET_SERVICE_INIT(msg, cSfnL2PDisplay);

    msg.param[0] = argv[0];   // laa
    msg.param[1] = argv[1];   // cnt

    Ipc_SendWaitMsg(cH2cUartQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Core Monitor command handler - Display VAC information.

    VAC

    @param[in]  pCmdStr     command line string
    @param[in]  argc        argument count if auto-decode is enabled
    @param[in]  argv        argument values if auto-decode is enabled

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmdCore_Vac(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgService_t msg;

    MEM_CLR(&msg, sizeof(msg));

    MSG_SET_SERVICE_INIT(msg, cSfnVACDisplay);

    Ipc_SendWaitMsg(cH2cUartQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Core Monitor command handler - Display Trim information.

    VAC

    @param[in]  pCmdStr     command line string
    @param[in]  argc        argument count if auto-decode is enabled
    @param[in]  argv        argument values if auto-decode is enabled

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmdCore_TrimBmp(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgService_t msg;

    MEM_CLR(&msg, sizeof(msg));

    MSG_SET_SERVICE_INIT(msg, cSfnTRIMBMPDisplay);

    Ipc_SendWaitMsg(cH2cUartQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Core Monitor command handler - FTL Flush Dummy.

    FDUMY

    @param[in]  pCmdStr     command line string
    @param[in]  argc        argument count if auto-decode is enabled
    @param[in]  argv        argument values if auto-decode is enabled

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmdCore_FtlFlushDummy(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgService_t msg;

    MEM_CLR(&msg, sizeof(msg));

    MSG_SET_SERVICE_INIT(msg, cSfnFTLflushDummy);

    Ipc_SendWaitMsg(cH2cUartQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Core Monitor command handler - Display IDS information.

    DIDS

    @param[in]  pCmdStr     command line string
    @param[in]  argc        argument count if auto-decode is enabled
    @param[in]  argv        argument values if auto-decode is enabled

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmdCore_Ids(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgService_t msg;

    MEM_CLR(&msg, sizeof(msg));

    MSG_SET_SERVICE_INIT(msg, cSfnIdsDisplay);

    Ipc_SendWaitMsg(cH2cUartQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Core Monitor command handler - Display GC information.

    DGC

    @param[in]  pCmdStr     command line string
    @param[in]  argc        argument count if auto-decode is enabled
    @param[in]  argv        argument values if auto-decode is enabled

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmdCore_Gc(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgService_t msg;

    MEM_CLR(&msg, sizeof(msg));

    MSG_SET_SERVICE_INIT(msg, cSfnGcDisplay);

    Ipc_SendWaitMsg(cH2cUartQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Core Monitor command handler - Display GC P2L information.

    DGCP2L

    @param[in]  pCmdStr     command line string
    @param[in]  argc        argument count if auto-decode is enabled
    @param[in]  argv        argument values if auto-decode is enabled

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmdCore_GcP2L(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgService_t msg;

    MEM_CLR(&msg, sizeof(msg));

    MSG_SET_SERVICE_INIT(msg, cSfnGcP2lDisplay);

    Ipc_SendWaitMsg(cH2cUartQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);

    return cEcNoError;
}
//-----------------------------------------------------------------------------
/**
    Core Monitor command handler - GC Test Function.

    GCTEST

    @param[in]  pCmdStr     command line string
    @param[in]  argc        argument count if auto-decode is enabled
    @param[in]  argv        argument values if auto-decode is enabled

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmdCore_GcTest(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgService_t msg;

    MEM_CLR(&msg, sizeof(msg));

    MSG_SET_SERVICE_INIT(msg, cSfnGcTest);

    msg.param[0] = argv[0];
    msg.param[1] = argv[1];
    msg.param[2] = argv[2];
    Ipc_SendWaitMsg(cH2cUartQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);

    return cEcNoError;
}
//-----------------------------------------------------------------------------
/**
    Core Monitor command handler - Preformat.

    FORMAT

    @param[in]  pCmdStr     command line string
    @param[in]  argc        argument count if auto-decode is enabled
    @param[in]  argv        argument values if auto-decode is enabled

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmdCore_Format(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgService_t msg;

#if _TCG_ != TCG_NONE
    TcgKeepGroupDataBridge(4);
    TcgEepOperatedBridge(0);
    TcgEepBackupBridge();
    TcgPsidBackupBridge();
#endif

    Purge_Center(PURGE_CORE_FTL | PURGE_CORE_UNCT);

#if _TCG_ != TCG_NONE
    TcgEepRestoreBridge();
    TcgEepOperatedBridge(2);
    TcgPsidRestoreBridge();
    TcgPreformatAndInit(TRUE);
#endif

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Core Monitor command handler - Sequencer Function

    SEQ

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        not used
    @param[in]  argv        not used

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t MonCmdCore_Seq(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgSeq_t msg;

    MEM_CLR(&msg, sizeof(msg));

    if (argc < 7 && argv[0] < 7)
    {
        D_PRINTF("OpCode[arg0] Multi/Single(1/0) Plane[arg1] CH[arg2] CE[arg3] LN[arg4] PBK[arg5] PG[arg6] (MultiPlane)PairBlk1[arg7] PairBlk2[arg8] PairBlk3[arg9]\n");
        return cEcError;
    }

    /*
    OpCode [0] : Erase
    OpCode [1] : Write Page
    OpCode [2] : Read Page
    OpCode [3] : ECC Scan
    OpCode [4] : Erase whole disk
    OpCode [5] : Write Block
    OpCode [6] : Read Block
    */
    msg.opcode       = argv[0]; ///opcode
    msg.plane        = argv[1]; ///Multi/Single(1/0)  /// Slc Single:2
    msg.paa.all32    = 0;
    msg.paa.b.ch     = argv[2];
    msg.paa.b.ce     = argv[3];
    msg.paa.b.lun    = argv[4];
    msg.paa.b.block  = (argv[5] / gNfInfo.planesPerLun);
    msg.paa.b.plane  = (argv[5] % gNfInfo.planesPerLun);
    msg.paa.b.page   = argv[6];

    D_PRINTF("Seq OpCode[%2x] PAA[CH%1d|CE%1d|LN%1d|PN%1d|BK0x%4x|PG0x%3x]\n", argv[0], msg.paa.b.ch, msg.paa.b.ce, msg.paa.b.lun, msg.paa.b.plane, msg.paa.b.block, msg.paa.b.page);

    if (msg.plane != 0) ///MultiPlane
    {
        if (msg.paa.b.plane == 0)
        {
            msg.pairblk[0] = (argv[7] % gNfInfo.planesPerLun == 1) ? (argv[7] / gNfInfo.planesPerLun) : (msg.paa.b.block);  /// plane 1
            if (gNfInfo.planesPerLun > 2)
            {
                msg.pairblk[1] = (argv[8] % gNfInfo.planesPerLun == 2) ? (argv[8] / gNfInfo.planesPerLun) : (msg.paa.b.block);  /// plane 2
                msg.pairblk[2] = (argv[9] % gNfInfo.planesPerLun == 3) ? (argv[9] / gNfInfo.planesPerLun) : (msg.paa.b.block);  /// plane 3
            }
        }
        else return cEcError;
    }

    MSG_SET_SERVICE_INIT(msg, csfnSeqRW);
    Ipc_SendWaitMsg(cH2cUartQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Core Monitor command handler - Test SysInfo Function.

    SITEST

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        not used
    @param[in]  argv        not used

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t MonCmdCore_SysInfo(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    U32 i=0;

    D_PRINTF("argv[0]: %x\n",argv[0]);

    if (argv[0] == 0x1)               //Copy Plist1 to Plist2
    {
        if ((smSysInfo->d.Header.d.SubHeader.d.PList2.d.dwValidTag != SI_TAG_PLIST2) &&
            (smSysInfo->d.Header.d.SubHeader.d.PList1.d.dwValidTag == SI_TAG_PLIST1))
        {
            MEM_COPY(&smSysInfo->d.PList2, &smSysInfo->d.PList1, sizeof(tSI_PAYLOAD_PLIST));
            MEM_COPY(&smSysInfo->d.PlistCnt.d.PList2Cnt.wCount[0], &smSysInfo->d.PlistCnt.d.PList1Cnt.wCount[0], sizeof(tSI_PLIST_CNT));
            smSysInfo->d.Header.d.SubHeader.d.PList2.d.dwValidTag = SI_TAG_PLIST2;
            SyIn_Synchronize(SI_AREA_MISC|SI_AREA_PLIST2, SYSINFO_WRITE, SI_SYNC_BY_UART);
        }
    }
    else if (argv[0] == 0x2)               //Copy Plist1 to Plist2
    {
        if (smSysInfo->d.Header.d.SubHeader.d.PList2.d.dwValidTag == SI_TAG_PLIST2) D_PRINTF("PLIST2 Exist \n");
        if (smSysInfo->d.Header.d.SubHeader.d.PList1.d.dwValidTag == SI_TAG_PLIST1) D_PRINTF("PLIST1 Exist \n");
    }
    else if(argv[0] == 0x14)   //Sync test
    {
        for (i = 0; i <= 0x100; i++)
        {
            D_PRINTF("Start_Write\n");
            SyIn_Synchronize(SI_AREA_MISC, SYSINFO_WRITE, SI_SYNC_BY_UART);
            D_PRINTF("End\n");
        }
    }
    else if(argv[0] == 0x80)
    {
        if (argc != 2) return (cEcNoError);
        Purge_Center(argv[1]);
    }
    else if (argv[0] == 0x87)  //NAND 2 NOR
    {
       SyIn_Synchronize(NULL, SYSINFO_WRITE, SI_SYNC_BY_NOR);
       D_PRINTF("End\n");
    }
    else if (argv[0] == 0x89)
    {
        #if CHK_FW_TYPE(FW_AGING)
        Debug_DumpMemU32((U32)&smSysInfo->d.AgingEI, SI_PAYLOAD_SIZE_AGINGEI);
        #else
        Debug_DumpMemU32((U32)&smSysInfo->d.FtlRmpData, sizeof(tSI_PAYLOAD_FTL_RMPBLK));
        #endif
        D_PRINTF("End\n");
    }
    else
    {
        D_PRINTF("Unknow Idx\n");
    }
    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Core Monitor command handler - Erase SysInfo Data.

    SITEST

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        not used
    @param[in]  argv        not used

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t MonCmdCore_RemoveSysInfo(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgSeq_t msg;

    MEM_CLR(&msg, sizeof(msg));
    MSG_SET_SERVICE_INIT(msg, cSfnSIClear);
    Ipc_SendWaitMsg(cH2cUartQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);
    D_PRINTF("Done\n");
    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Core Monitor command handler - Erase SysInfo Data.

    PURGE

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        not used
    @param[in]  argv        not used

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t MonCmdCore_KillFtl(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    if (smOverrideFTL != DWORD_MASK) smOverrideFTL = DWORD_MASK;
    else                             smOverrideFTL = FTL_OVERRIDE_TAG;

    Purge_Center(PURGE_NOR_FTLOVERRIDE);
    return(cEcNoError);
}
//-----------------------------------------------------------------------------
/**
    Core Monitor command handler - Erase NAND Data.

    PURGE

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        not used
    @param[in]  argv        not used

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t MonCmdCore_ClearNand(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    Purge_Center(PURGE_NAND_SYSTEM_WO);
    return(cEcNoError);

}
//-----------------------------------------------------------------------------
/**
    Core Monitor command handler - Erase SysInfo Data.

    PURGE

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        not used
    @param[in]  argv        not used

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t MonCmdCore_Purge(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    U64* pArea = NULL;
    U32  split[2];
    U8   idx;

    if(argc == 0)
    {
        D_PRINTF("argv[0~7] areaIdx\n");
    }

    MEM_CLR(split, sizeof(U32) * 2);
    for(idx = 0; idx < argc; idx++)
    {
        if (argv[idx] < 32)      split[0] |= BIT(argv[idx]);
        else if (argv[idx] < 64) split[1] |= BIT((argv[idx]-32));
    }
    pArea = (U64*)split;
    Purge_Center(*pArea);

    return(cEcNoError);
}
//-----------------------------------------------------------------------------
/**
    Core Monitor command handler - Set SN Function.

    SSN

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        not used
    @param[in]  argv        not used

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t MonCmdCore_SetSn(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
#if (SUPPORT_UART_SET_SN == TRUE)
    #define UART_SN_BYTE_COUNT  12
    #define UART_SN_BUFF_COUNT  20
    #define UART_SN_ARGC_COUNT  4

    U8 bBuffer[UART_SN_BUFF_COUNT];
    U8 i, size;

    // mode
    if (argv[0] >= 2)
    {
        D_PRINTF("SetSN mode 0: PCBA 1: Drive\n");
        return cEcInvalidParameter;
    }

    MEM_SET(bBuffer, 0x20, UART_SN_BUFF_COUNT);
    size = 0;

    D_PRINTF("arg: %1X %04X %04X %04X\n", argv[0], argv[1], argv[2], argv[3]);

    // Handle the "P"CBA
    if (argv[0] == 0)
    {
        bBuffer[0] = 0x50;
        size++;
    }

    for(i = 1; i < UART_SN_ARGC_COUNT; i++)
    {
        size += sprintf((char *)&bBuffer[size], "%04X", argv[i]);
        D_PRINTF("Buf: %s, size: %d\n", bBuffer, size);
    }

    bBuffer[UART_SN_BYTE_COUNT] = 0;
    D_PRINTF("SSN: %s\n", bBuffer);
    bBuffer[UART_SN_BYTE_COUNT] = 0x20;

    MEM_COPY(smSysInfo->d.MPInfo.d.SerialNumberDrive, bBuffer, UART_SN_BUFF_COUNT);
    if (argv[0] == 0)
    {
        MEM_COPY(smSysInfo->d.MPInfo.d.SerialNumberPCBA, bBuffer, UART_SN_BUFF_COUNT);
    }

    SyIn_Synchronize(SI_AREA_MPINFO, SYSINFO_WRITE, SI_SYNC_BY_UART);
#endif

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Core Monitor command handler - Backup QBT Function.

    QBT

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        not used
    @param[in]  argv        not used

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t MonCmdCore_Qbt(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    Msg_t* pMsg;

    pMsg = (Msg_t*)Ipc_GetExtendMsg(cMcShutdown);
    MSG_SET_EXTEND_INIT(pMsg, cMcShutdown, TRUE);

    IPC_SendMsgQ(cH2cReqQueue, pMsg);
    while (pMsg->hdr.b.status != cMsgCompleted);

    return cEcNoError;
}

#if _TCG_ != TCG_NONE
//-----------------------------------------------------------------------------
/**
    Core Monitor command handler - Operate TCG Defect/Erase count table

    TCGDE

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        not used
    @param[in]  argv        not used

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmdCore_TcgEraDefTbl(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    switch (argv[0])
    {
        case 0 :
            TCG_PRINTF("TcgNorEepIdx = %X\n", tcgEepProgIdx);
            DumpG4ErasedCount();
            DumpG5ErasedCount();
            DumpTcgEepInfo();
            break;
        case 1 :
            MEM_SET((void *)tcgG4EraCnt, 0, sizeof(tcgG4EraCnt));   // force clear G4 erased count
            MEM_SET((void *)tcgG5EraCnt, 0, sizeof(tcgG5EraCnt));   // force clear G5 erased count

            MEM_SET((void *)tcgG4Dft, 0, sizeof(tcgG4Dft));   // force clear G4 defect table
            MEM_SET((void *)tcgG5Dft, 0, sizeof(tcgG5Dft));   // force clear G5 defect table
        #ifdef TCG_EEP_NOR
            TcgH2cJobIssue(MSG_TCG_NOREEP_WR, NULL);
        #else
            SyIn_Synchronize(SI_AREA_MISC, SYSINFO_WRITE, SI_SYNC_BY_TCG);
        #endif
            break;
        default:
            break;
    }

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Core Monitor command handler - Get TCG Rang Info

    TCGRL

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        not used
    @param[in]  argv        not used

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmdCore_TcgRngLock(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    switch (argv[0])
    {
        case 0 :
            DumpRangeInfo();
            break;
        case 1 :
            // TODO: set range lock
            break;
        default:
            break;
    }

    return cEcNoError;
}
#endif

//-----------------------------------------------------------------------------
/**
    Core Monitor command handler - Run Aging Test

    TCGRL

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        not used
    @param[in]  argv        not used

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmdCore_Aging(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgService_t msg;

    MEM_CLR(&msg, sizeof(msg));

    msg.param[0] = argv[0]; //opcode (0: dump 1: run aging 2: calculate nand busy)
    msg.param[1] = argv[1]; //loop
    msg.param[2] = argv[2]; //mode
    msg.param[3] = argv[3]; //aging (0: ENABLE 1:DISABLE)

    if(argv[3] != 0)   // DISABLE Aging
    {
        smSysInfo->d.AgingData.d.Aging_Signature = DWORD_MASK;
        smSysInfo->d.AgingData.d.Aging_Mode      = DISABLE;
    }
    else
    {
        MSG_SET_SERVICE_INIT(msg, cSfnAging);
        Ipc_SendWaitMsg(cH2cUartQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);
    }
    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Core Monitor command handler - SmBus Test

    SEQ

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        not used
    @param[in]  argv        not used

    @return   error code
**/
//-----------------------------------------------------------------------------
Error_t MonCmdCore_SmBus(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgService_t msg;

    MEM_CLR(&msg, sizeof(msg));
    MEM_CLR(gMonBuffer, SMBUS_VPD_PAYLOAD_SIZE);

    MSG_SET_SERVICE_INIT(msg, cSfnSmBus);

    #if (CO_SUPPORT_SMBUS == TRUE)
    if (argv[0] <= 2)
    {

        msg.param[0] = argv[0]; //opcode (0: SmBus VPD Inject 1: VPD Read  2: VPD Write)
        msg.param[1] = (U32)gMonBuffer;
        msg.param[2] = argv[1]; //Offset
        msg.param[3] = argv[2]; //Len

        Ipc_SendWaitMsg(cH2cUartQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);
        if (argv[0])
        {
            D_PRINTF("[SMBS] Payload\n");
            Debug_DumpMemU8((U32) gMonBuffer, argv[2]);
        }
    }
    else
    {
        return cEcError;
    }
    #endif

    return cEcNoError;
}
/* End of File */
