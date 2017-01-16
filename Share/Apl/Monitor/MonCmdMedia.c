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
//! @brief Test monitor command
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"
#include "MemTest.h"
#include "SysInfo.h"

#include "NormSection.h"   // CODE AND DATA MEMORY MAPPING
//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define DIRECTCMD_TO_SVC_FN(x)  ((x) + cSfnNfimReadId)    // Using Start Offset

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
static Error_t MonCmdMedia_Reset(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdMedia_DumpInfo(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdMedia_ReadId(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdMedia_ReadParamPage(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdMedia_GetFeatures(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdMedia_SetFeatures(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdMedia_PkgConfig(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdMedia_IndirectRdWr(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdMedia_DirectRdWr(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdMedia_ScanDefect(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdMedia_Fio(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdMedia_GetVoltageOffset(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdMedia_SetVoltageOffset(Cstr_t pCmdStr, U32 argc, U32 argv[]);
static Error_t MonCmdMedia_ReadRetry(Cstr_t pCmdStr, U32 argc, U32 argv[]);
//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
const MonCmdTbl_t monMediaCmdList[] =
{
//===============================================================================================
//    cmd string, help string,                      min-argc, auto-dec, exe_time, handeler
//...............................................................................................
    { "RST",      "Reset",                          1,       ON,      OFF,     MonCmdMedia_Reset            },

    { "MINFO",    "Dump Media Info",                0,       ON,      OFF,     MonCmdMedia_DumpInfo         },
    { "ID",       "Read ID",                        0,       ON,      OFF,     MonCmdMedia_ReadId           },
    { "RDPP",     "Read Para.Page",                 0,       ON,      OFF,     MonCmdMedia_ReadParamPage    },
    { "GETF",     "Get Features",                   1,       ON,      OFF,     MonCmdMedia_GetFeatures      },
    { "SETF",     "Set Features",                   2,       ON,      OFF,     MonCmdMedia_SetFeatures      },
    { "GETV",     "Get Volt.Offset",                1,       ON,      OFF,     MonCmdMedia_GetVoltageOffset },
    { "SETV",     "Set Volt.Offset",                2,       ON,      OFF,     MonCmdMedia_SetVoltageOffset },
    { "PKGCFG",   "Pack.Config",                    2,       ON,      OFF,     MonCmdMedia_PkgConfig        },
    { "IDRW",     "Indirect Op.",                   1,       ON,       ON,     MonCmdMedia_IndirectRdWr     },
    { "DTRW",     "Direct Op.",                     1,       ON,       ON,     MonCmdMedia_DirectRdWr       },
    { "SCF",      "Scan Defect",                    0,       ON,      OFF,     MonCmdMedia_ScanDefect       },
    { "DJ",       "Dump Fio Job",                   0,       ON,      OFF,     MonCmdMedia_Fio              },
    { "DR",       "Dump Read Retry",                0,       ON,      OFF,     MonCmdMedia_ReadRetry        },
    // The last line (DON'T CHANGE BELOW).
    { "",         "",                               0,      OFF,      OFF,     NULL                         } // EOT
};

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    Media Monitor command handler - Indirect mode Reset

    RST [CMD][channel=0] [lane=0]

    @param[in]  pCmdStr     command line string
    @param[in]  argc        argument count if auto-decode is enabled
    @param[in]  argv        argument values if auto-decode is enabled
                            argv[0] : (FFh=RESET, FCh=SYNC_RESET)
                            argv[1] : channel
                            argv[2] : lane

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmdMedia_Reset(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgPhyRwe_t msg;

    MEM_CLR(&msg, sizeof(msg));

    MSG_SET_SERVICE_INIT(msg, cSfnNfimReset);

    Ipc_SendWaitMsg(cH2mReqQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);

    return cEcNoError;
}
//-----------------------------------------------------------------------------
/**
    Media Monitor command handler - Display media information.

    MINFO

    @param[in]  pCmdStr     command string buffer pointer
    @param[in]  argc        not used
    @param[in]  argv        not used

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmdMedia_DumpInfo(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgService_t msg;

    MSG_SET_SERVICE_INIT(msg, cSfnDumpInfo);

    msg.param[0] = argv[0];   // level

    Ipc_SendWaitMsg(cH2mReqQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);

    return(cEcNoError);
}

//-----------------------------------------------------------------------------
/**
    Media Monitor command handler - Indirect mode read ID

    RDPP [ADDR=0] [count=6] [buffer_address=gMonBuffer]

    @param[in]  pCmdStr     command line string
    @param[in]  argc        argument count if auto-decode is enabled
    @param[in]  argv        argument values if auto-decode is enabled
                            argv[0] : page address
                            argv[1] : byte count to read
                            argv[2] : buffer address

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmdMedia_ReadId(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgPhyRwe_t msg;

    MEM_CLR(&msg, sizeof(msg));
    MEM_SET(gMonBuffer, BYTE_MASK, MONBUFFER_SIZE);

    MSG_SET_SERVICE_INIT(msg, cSfnNfimReadId);

    msg.byteCount = 16 * smSysInfo->d.MiscData.d.NandInfo.d.NandFlashInfo.numChannels * smSysInfo->d.MiscData.d.NandInfo.d.NandFlashInfo.devicesPerChannel;
    msg.pBuffer   = gMonBuffer;
    msg.param[0]  = argv[0];

    Ipc_SendWaitMsg(cH2mReqQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);

    Debug_DumpMemU8((U32)msg.pBuffer, msg.byteCount);

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Media Monitor command handler - Indirect mode read and dump parameter page

    RDPP [ADDR=0] [count=256] [buffer_address=gMonBuffer]

    @param[in]  pCmdStr     command line string
    @param[in]  argc        argument count if auto-decode is enabled
    @param[in]  argv        argument values if auto-decode is enabled
                            argv[0] : page address (0 for ONFI, 40h for JEDEC)
                            argv[1] : byte count to read
                            argv[2] : buffer address

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmdMedia_ReadParamPage(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgPhyRwe_t msg;

    MEM_CLR(&msg, sizeof(msg));
    MEM_SET(gMonBuffer, BYTE_MASK, MONBUFFER_SIZE);

    if (argv[2] != 0x40)
    {
        D_PRINTF("argv0: ch\n");
        D_PRINTF("argv1: ce\n");
        D_PRINTF("argv2: JEDEC 0x40\n");
        return cEcNoError;
    }
    MSG_SET_SERVICE_INIT(msg, cSfnNfimReadPp);

    msg.paa.b.ch  = argv[0];
    msg.paa.b.ce  = argv[1];
    msg.param[0]  = argv[2];  //row address 0x40
    msg.byteCount = 512;
    msg.pBuffer   = gMonBuffer;

    Ipc_SendWaitMsg(cH2mReqQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);

    Debug_DumpMemU8((U32)msg.pBuffer, msg.byteCount);

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Media Monitor command handler - Indirect mode Get feature

    GETP [ADDR=01] [channel=0] [lane=0]

    @param[in]  pCmdStr     command line string
    @param[in]  argc        argument count if auto-decode is enabled
    @param[in]  argv        argument values if auto-decode is enabled
                            argv[0] : Feature address (01=Timing mode, ...)
                            argv[1] : channel
                            argv[2] : lane

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmdMedia_GetFeatures(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgPhyRwe_t msg;

    MEM_CLR(&msg, sizeof(msg));
    MEM_SET(gMonBuffer, BYTE_MASK, MONBUFFER_SIZE);

    if ((argv[0] != 0x02) && (argv[0] != 0x10) && (argv[0] != 0x30))
    {
        D_PRINTF("argv0: 02h (Toggle2.0) 10h(Driven Strength) 30h(External Vpp)\n");
        D_PRINTF("argv1: parameter\n");
        return cEcNoError;
    }

    MSG_SET_SERVICE_INIT(msg, cSfnNfimGetf);

    msg.param[0]  = argv[0];   ///row address
    msg.byteCount = 8 * smSysInfo->d.MiscData.d.NandInfo.d.NandFlashInfo.numChannels * smSysInfo->d.MiscData.d.NandInfo.d.NandFlashInfo.devicesPerChannel;
    msg.pBuffer   = gMonBuffer;

    Ipc_SendWaitMsg(cH2mReqQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);

    Debug_DumpMemU8((U32)msg.pBuffer, msg.byteCount);

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Media Monitor command handler - Indirect mode Set feature

    SETF value ADDR [channel=0] [lane=0]

    @param[in]  pCmdStr     command line string
    @param[in]  argc        argument count if auto-decode is enabled
    @param[in]  argv        argument values if auto-decode is enabled
                            argv[0] : Feature address (01=Timing mode, ...)
                            argv[1] : set value
                            argv[2] : channel
                            argv[3] : lane

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmdMedia_SetFeatures(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgPhyRwe_t msg;

    MEM_CLR(&msg, sizeof(msg));
    MEM_SET(gMonBuffer, BYTE_MASK, MONBUFFER_SIZE);

    if ((argv[0] != 0x02) && (argv[0] != 0x10) && (argv[0] != 0x30))
    {
        D_PRINTF("argv0: 02h (Toggle2.0) 10h(Driven Strength) 30h(External Vpp)\n");
        D_PRINTF("argv1: parameter\n");
        return cEcNoError;
    }

    MSG_SET_SERVICE_INIT(msg, cSfnNfimSetf);

    msg.param[0] = argv[0];   ///row address
    msg.param[1] = argv[1];  /// set feature parameter

    msg.pBuffer   = gMonBuffer;
    Ipc_SendWaitMsg(cH2mReqQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Media Monitor command handler - Indirect mode Get feature

    GETP [ADDR=01] [channel=0] [lane=0]

    @param[in]  pCmdStr     command line string
    @param[in]  argc        argument count if auto-decode is enabled
    @param[in]  argv        argument values if auto-decode is enabled
                            argv[0] : Feature address (01=Timing mode, ...)
                            argv[1] : channel
                            argv[2] : lane

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmdMedia_GetVoltageOffset(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgPhyRwe_t msg;

    MEM_CLR(&msg, sizeof(msg));
    MEM_SET(gMonBuffer, BYTE_MASK, MONBUFFER_SIZE);

    MSG_SET_SERVICE_INIT(msg, csfnNfimGetVoltageOffset);

    msg.param[0]  = argv[0];
    msg.byteCount = 8 * smSysInfo->d.MiscData.d.NandInfo.d.NandFlashInfo.numChannels * smSysInfo->d.MiscData.d.NandInfo.d.NandFlashInfo.devicesPerChannel;
    msg.pBuffer   = gMonBuffer;

    Ipc_SendWaitMsg(cH2mReqQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);

    Debug_DumpMemU8((U32)msg.pBuffer, msg.byteCount);

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Media Monitor command handler - Indirect mode Set feature

    SETF value ADDR [channel=0] [lane=0]

    @param[in]  pCmdStr     command line string
    @param[in]  argc        argument count if auto-decode is enabled
    @param[in]  argv        argument values if auto-decode is enabled
                            argv[0] : Feature address (01=Timing mode, ...)
                            argv[1] : set value
                            argv[2] : channel
                            argv[3] : lane

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmdMedia_SetVoltageOffset(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgPhyRwe_t msg;

    MEM_CLR(&msg, sizeof(msg));
    MEM_SET(gMonBuffer, BYTE_MASK, MONBUFFER_SIZE);

    MSG_SET_SERVICE_INIT(msg, csfnNfimSetVoltageOffset);

    msg.param[0]  = argv[0];
    msg.param[1]  = argv[1];   /// set voltage parameter

    msg.pBuffer  = gMonBuffer;

    Ipc_SendWaitMsg(cH2mReqQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);

    return cEcNoError;
}
//-----------------------------------------------------------------------------
/**
    Media Monitor command handler - Package configure

    PKGCFG [IF_DATA] [XFER_MODE]

    @param[in]  pCmdStr     command line string
    @param[in]  argc        argument count if auto-decode is enabled
    @param[in]  argv        argument values if auto-decode is enabled
                            argv[0] : IF_DATA (0,1:ASYNC, 2:NVDDR, 3:TOGGLE1, 4:NVDDR2, 5:TOGGLE2)
                            argv[1] : XFER_MODE

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmdMedia_PkgConfig(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgPhyRwe_t msg;

    MEM_CLR(&msg, sizeof(msg));
    MEM_SET(gMonBuffer, BYTE_MASK, MONBUFFER_SIZE);

    D_PRINTF("argv0: reqIfData\n");
    D_PRINTF("argv1: reqXferMode\n");

    MSG_SET_SERVICE_INIT(msg, cSfnNfimPkgcfg);

    msg.param[0] = argv[0];   //reqIfData
    msg.param[1] = argv[1];   //reqXferMode

    Ipc_SendWaitMsg(cH2mReqQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);

    M_PRINTF("PKG_CFG IF_DATA:%1x XFER_MODE:%1x\n", msg.param[0], msg.param[1]);

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Media Monitor command handler - Indirect mode NAND access

    IDRW  function [channel=0] [lane=0] [row=0] [buffer_address=gMonBuffer]

    @param[in]  pCmdStr     command line string
    @param[in]  argc        argument count if auto-decode is enabled
    @param[in]  argv        argument values if auto-decode is enabled
                            argv[0] : function(0=read, 1=write, 2=erase)
                            argv[1] : channel
                            argv[2] : lane
                            argv[3] : row
                            argv[4] : buffer_address

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmdMedia_IndirectRdWr(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgPhyRwe_t msg;

    MEM_SET(gMonBuffer, BYTE_MASK, MONBUFFER_SIZE);

    if (((argc > 7) || (argc < 7)) && (argv[0] < 4))
    {
        D_PRINTF("OpCode[arg0] SLC[arg1 MLC/TLC:0 SLC:0xA2] CH[arg2] CE[arg3] LN[arg4] BK[arg5] PG[arg6]\n");
        D_PRINTF("OpCode[2]: Indirect Read  (Physical Block Unit)\n");
        D_PRINTF("OpCode[3]: Indirect Write (Physical (MLC:Page / TLC:WL)\n");
        D_PRINTF("OpCode[4]: Indirect Erase (Physical Page  Unit)\n");
    }

    MEM_CLR(&msg, sizeof(msg));

    MSG_SET_SERVICE_INIT(msg, DIRECTCMD_TO_SVC_FN(argv[0]));

    msg.param[0]    = argc>1 ? argv[1] : 0;
    msg.paa.b.ch    = argc>2 ? argv[2] : 0;
    msg.paa.b.ce    = argc>3 ? argv[3] : 0;
    msg.paa.b.lun   = argc>4 ? argv[4] : 0;
    msg.paa.b.block = argc>5 ?(argv[5] / gNfInfo.planesPerLun) : 0;
    msg.paa.b.plane = argc>5 ?(argv[5] % gNfInfo.planesPerLun) : 0;
    msg.paa.b.page  = argc>6 ? argv[6] : 0;

    msg.pBuffer  = gMonBuffer;

    Ipc_SendWaitMsg(cH2mReqQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Media Monitor command handler - Backend Test

    BERW  function

    @param[in]  pCmdStr     command line string
    @param[in]  argc        argument count if auto-decode is enabled
    @param[in]  argv        argument values if auto-decode is enabled
                            argv[0] : function
                            argv[1] : parameter1
                            argv[2] : parameter2

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmdMedia_DirectRdWr(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgPhyRwe_t msg;

    MEM_SET(gMonBuffer, BYTE_MASK, MONBUFFER_SIZE);

    if (((argc > 7) || (argc < 7)) && (argv[0] < 4))
    {
        D_PRINTF("OpCode[arg0] SLC[arg1 MLC/TLC:0 SLC:0xA2] CH[arg2] CE[arg3] LN[arg4] BK[arg5] PG[arg6]\n");
        D_PRINTF("OpCode[2]: Direct Read  (Physical Block Unit)\n");
        D_PRINTF("OpCode[3]: Direct Write (Physical (MLC:Page / TLC:WL)\n");
        D_PRINTF("OpCode[4]: Direct Erase (Physical Page  Unit)\n");
    }

    MEM_CLR(&msg, sizeof(msg));

    MSG_SET_SERVICE_INIT(msg, DIRECTCMD_TO_SVC_FN(argv[0]));

    msg.param[0]    = argc>1 ? argv[1] : 0;
    msg.paa.b.ch    = argc>2 ? argv[2] : 0;
    msg.paa.b.ce    = argc>3 ? argv[3] : 0;
    msg.paa.b.lun   = argc>4 ? argv[4] : 0;
    msg.paa.b.block = argc>5 ?(argv[5] / gNfInfo.planesPerLun) : 0;
    msg.paa.b.plane = argc>5 ?(argv[5] % gNfInfo.planesPerLun) : 0;
    msg.paa.b.page  = argc>6 ? argv[6] : 0;

    msg.pBuffer  = gMonBuffer;

    Ipc_SendWaitMsg(cH2mReqQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);

    if (DIRECTCMD_TO_SVC_FN(argv[0]) == cSfnNfimScanDefect)
    {
        smSysInfo->d.Header.d.SubHeader.d.PList1.d.dwValidTag     = SI_TAG_PLIST1;
        smSysInfo->d.Header.d.SubHeader.d.PlistCnt.d.dwValidTag   = SI_TAG_PLSTCNT;

        SyIn_Synchronize(SI_AREA_PLSTCNT | SI_AREA_PLIST1, SYSINFO_WRITE, SI_SYNC_BY_SCANDEFECT);
        SyIn_Synchronize(SI_AREA_PLSTCNT | SI_AREA_PLIST1, SYSINFO_WRITE, SI_SYNC_BY_NOR);
    }

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Media Monitor command handler - Backend Test

    BERW  function

    @param[in]  pCmdStr     command line string
    @param[in]  argc        argument count if auto-decode is enabled
    @param[in]  argv        argument values if auto-decode is enabled

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmdMedia_ScanDefect(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgPhyRwe_t msg;

    MEM_CLR(&msg, sizeof(msg));
    MEM_SET(gMonBuffer, BYTE_MASK, MONBUFFER_SIZE);

    MSG_SET_SERVICE_INIT(msg, cSfnNfimScanDefect);

    Ipc_SendWaitMsg(cH2mReqQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);

    smSysInfo->d.Header.d.SubHeader.d.PList1.d.dwValidTag     = SI_TAG_PLIST1;
    smSysInfo->d.Header.d.SubHeader.d.PlistCnt.d.dwValidTag   = SI_TAG_PLSTCNT;

    SyIn_Synchronize(SI_AREA_PLSTCNT | SI_AREA_PLIST1, SYSINFO_WRITE, SI_SYNC_BY_SCANDEFECT);
    SyIn_Synchronize(SI_AREA_PLSTCNT | SI_AREA_PLIST1, SYSINFO_WRITE, SI_SYNC_BY_NOR);

    return cEcNoError;
}
//-----------------------------------------------------------------------------
/**
    Media Monitor command handler - Display Fio Job information.

    DJ

    @param[in]  pCmdStr     command line string
    @param[in]  argc        argument count if auto-decode is enabled
    @param[in]  argv        argument values if auto-decode is enabled

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmdMedia_Fio(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgService_t msg;

    MEM_CLR(&msg, sizeof(msg));

    MSG_SET_SERVICE_INIT(msg, cSfnDumpFioInfo);

    msg.param[0] = argv[0];   // level

    Ipc_SendWaitMsg(cH2mReqQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Media Monitor command handler - Display Read Retry information.

    DR

    @param[in]  pCmdStr     command line string
    @param[in]  argc        argument count if auto-decode is enabled
    @param[in]  argv        argument values if auto-decode is enabled

    @return   error code
**/
//-----------------------------------------------------------------------------
static Error_t MonCmdMedia_ReadRetry(Cstr_t pCmdStr, U32 argc, U32 argv[])
{
    MsgService_t msg;

    MEM_CLR(&msg, sizeof(msg));

    MSG_SET_SERVICE_INIT(msg, cSfnDumpRRInfo);

    msg.param[0] = argv[0];   // level
    msg.param[1] = argv[1];   // param

    Ipc_SendWaitMsg(cH2mReqQueue, (Msg_t *)&msg, (Msg_t *)&msg, IPC_NO_TIMEOUT);

    return cEcNoError;
}
