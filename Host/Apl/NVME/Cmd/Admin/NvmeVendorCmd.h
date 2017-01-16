#pragma once
// ============================================================================
//
// ==        Copyright (c) LITEON Corporation  -  All rights reserved        ==
//
//  This computer program contains confidential and proprietary information,
//  and may NOT be reproduced or transmitted, in whole or in part, in any form,
//  or by any means electronic, mechanical, photo-optical, or otherwise, and
//  may NOT be translated into another language without the express written
//  permission from LITEON Corporation.
//
// ==        Copyright (c) LITEON Corporation  -  All rights reserved        ==
//
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------
#define VSC_LOG             ENABLE
#define VSC_CMDLOG          DISABLE

#define RD_VSC              ENABLE
#define NORMAL_VSC          ENABLE
#define OEM_VSC             ENABLE

#define RD_START            0x00
#define NORMAL_START        0x10
#define OEM_START           0x20
#define VSC_END             0x30

#define SIGNATURE_CHK       0x4356544C // "LTVC" Little India)
#define SIGNATURE_CHK_RVS   0x4C545643 // "LTVC"
#define TAB_HEAD_ECTB       0x42544345
#define TAB_HEAD_VCTB       0x42544356
#define TAB_HEAD_GLTB       0x42544C47
#define TAB_HEAD_ECCB       0x42434345

#define VSC_AGING_DRAM      0x4D415244

#if   (MAX_SUPPORTED_PLANES == 4)
    #define VSC_SUPPORT_MAX_BLK         ( 1069 )
#elif (MAX_SUPPORTED_PLANES == 2)
    #define VSC_SUPPORT_MAX_BLK         ( 2138 )
#endif

#define VSC_MAX_SECTOR      512
#define VSC_MAX_BUFSIZE     cNvmeScratchBufferSize
#define VSC_HEAD_SIZE       sizeof(tVSC_Htbp)

#define VSC_ECTABLE_SIZE    ((smSysInfo->d.FtlData.d.EndIdx - smSysInfo->d.FtlData.d.StartIdx + 1) << 1)
#define VSC_VSCTABLE_SIZE   ((smSysInfo->d.FtlData.d.EndIdx - smSysInfo->d.FtlData.d.StartIdx + 1) << 2)

#define ERASE_COUNT         2
#define VALID_COUNT         4

//-----------------------------------------------------------------------------
//  Vsc Mode define:
//-----------------------------------------------------------------------------

//----------------------------------------------
// Vendor Command Function
//----------------------------------------------
//VscDeviceHWCheck
enum
{
    VSC_DIR_NODATA          = 0xFC,
    VSC_DIR_WRITE           = 0xFD,
    VSC_DIR_READ            = 0xFE
};

//VscDeviceHWCheck
enum
{
    VSC_HW_READ_FLASHID     = 0x00,
    VSC_HW_PCIE_CONFIG      = 0x10,
    VSC_HW_PCIE_DID_VID     = 0x11,
    VSC_HW_CHECK_PLP        = 0x20,
    VSC_HW_CHECK_SOC_TYPE   = 0x40,
    VSC_HW_VPD_INITIAL      = 0x50,
    VSC_HW_VPD_WRTIE        = 0x58,
    VSC_HW_VPD_READ         = 0x59,
    VSC_HW_READ_DRAM_DLL    = 0x60
};

//VscDeviceFWCheck
enum
{
    VSC_FW_READFWCONFIG     = 0x00,
    VSC_FW_READ_FW_MODEL    = 0x40,
    VSC_FW_READ_BOOT_VERSION= 0x80
};

//SysInfo Read / Write Operation
enum
{
    VSC_SI_READ             = 0x00,
    VSC_SI_WIRTE            = 0x01
};

//Purge Operation
enum
{
    VSC_PG_MP_PREFORMAT     = 0x00
};

//VscAgingOperation
enum
{
    VSC_AG_ERASE_NANDFLASH  = 0x00,
    VSC_AG_SCAN_DEFECT      = 0x01,
    VSC_AG_AGING_BATCH      = 0x02,
    VSC_AG_PLIST_OP         = 0x03
};

//FTL-Related Operation
enum
{
    VSC_FTL_DISK_LOCK        = 0x00,
    VSC_FTL_NAND_FLUSH_CNT   = 0x08,
    VSC_FTL_UNCT_INSERT      = 0x10,
    VSC_FTL_UNCT_DELETE      = 0x11,
    VSC_FTL_UNCT_RESET       = 0x12
};

//VscInternalTableOperation
enum
{
    VSC_TB_READ_ERASE_CNT  = 0x00,
    VSC_TB_READ_VAILD_CNT  = 0x01,
    VSC_TB_READ_GLIST      = 0x02,
    VSC_TB_READ_UNCTB      = 0x03,
    VSC_TB_CLEAR_GLIST     = 0x10
};

//VscDebugOperation
enum
{
    VSC_DBG_SHOW_INFO     = 0x00,
    VSC_DBG_SAVE_LOG      = 0x01,
    VSC_DBG_READ_LOG      = 0x02
};

//VscDebugOperation
enum
{
    VSC_NVME_FLUSH      = 0x00,
    VSC_NVME_COMMIT     = 0x03
};


//----------------------------------------------
// MISC Define
//----------------------------------------------
// Ipc Core Seq Function
enum
{
    VSC_COREDBG_CMD_ERASE = 0,
    VSC_COREDBG_CMD_WRITE,
    VSC_COREDBG_CMD_READ,
    VSC_COREDBG_CMD_ECCSCAN,
    VSC_COREDBG_CMD_ERASEALL,
    VSC_COREDBG_CMD_WRITE_BLK,
    VSC_COREDBG_CMD_READ_BLK,
    VSC_COREDBG_CMD_SCANDEFECT,
    VSC_COREDBG_CMD_NAND_BUSYTIME,
    VSC_COREDBG_CMD_COUNT
};

//P-List DW13 Operaiotn
enum
{
    VSC_PLIST_FORCE_COPY   = 0x01,
    VSC_PLIST_NORMAL_COPY  = 0x02
};

//-----------------------------------------------------------------------------
//  Vsc Parameter Error definitions:
//-----------------------------------------------------------------------------
typedef enum
{
    ERROR_NoError           = 0x00,
    ERROR_NoSupFun          = 0xF0,
    ERROR_SIGNATURE         = 0xF1,
    ERROR_TRANSFER_SIZE     = 0xF2,
    ERROR_FUN_DIR           = 0xF3,
    ERROR_IPC_TIMEOUT       = 0xF4
}VscError_t;

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------
#define  VSC_CMD_DIR(FC, FD, FE)             (FC & 0xff) | ((FD << 1) & 0xff) | ((FE << 2) & 0xff)
#define  VSC_XFER_LEN_DW2BYTE(LEN)           (LEN << 2)
#define  VSC_CHECK_XFER_SIZE(MD, LEN)        (MD == VSC_TB_READ_ERASE_CNT) ? ((LEN > VSC_ECTABLE_SIZE)      ? TRUE : FALSE ): \
                                             (MD == VSC_TB_READ_VAILD_CNT) ? ((LEN > VSC_VSCTABLE_SIZE)     ? TRUE : FALSE ): \
                                             (MD == VSC_TB_READ_GLIST)     ? ((LEN > SI_PAYLOAD_SIZE_GLIST) ? TRUE : FALSE) : FALSE

#define VSC_CHANGE_XFER_SSDIDX(IDX, P)       {smSSD[IDX].startAddr = (U32)P;}
#define VSC_ASSIGN_TBHD(HDR, TAG, SIZE)      {(HDR).hTag = TAG; (HDR).hLength = SIZE;}

#if VSC_LOG == ENABLE
#define  VSC_PRINTF(F,args...)               D_PRINTF("[VSC] "F, ##args)          ///< VSC message print out
#else
#define  VSC_PRINTF(F,args...)               D_PRINTF("")                         ///< VSC message print out
#endif
//-----------------------------------------------------------------------------
//  VSC Function definitions:
//-----------------------------------------------------------------------------
typedef enum
{
    Vsc_NonSuppor = 0,
    Vsc_DeviceHWCheck = 0x10,
    Vsc_DeviceFWCheck,              // 0x11
    Vsc_SysInfoOperation,           // 0x12
    Vsc_PurageOperation,            // 0x13
    Vsc_AgingOnlyOperation,         // 0x14
    Vsc_FTLRelatedOperation,        // 0x15
    Vsc_InternalTableOperation,     // 0x16
    Vsc_DebugOperation,             // 0x17
    Vsc_NvmeAlternative             // 0x18
}tVSC_FnN;

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
// Mabe change HI LO BYTE for SSDX
typedef struct
{
    U8       VscFunction;     ///< DW12 Byte 0 Function
    U8       VscMode;         ///< DW12 Byte 1 Mode
    U8       Dw12Byte2;       ///< DW12 Byte 2 Other
    U8       Dw12Byte3;       ///< DW12 Byte 3 Other
}tVSC_CMD_Mode;

typedef  struct
{
    AdminCommandCommon_t DW9_0;    ///< DW9..0
    uint32_t             NDT;      ///< DW10
    uint32_t             NDM;      ///< DW11
    tVSC_CMD_Mode        VscDW12;  ///< DW12
    uint32_t             VscDW13;  ///< DW13
    uint32_t             VscDW14;  ///< DW14
    uint32_t             Signature;///< DW15
} NvmeAdminVendorCommand_t;
COMPILE_ASSERT(sizeof(NvmeAdminVendorCommand_t)==64, "NVMe command size shall be 64");

typedef  struct
{
    NvmeAdminVendorCommand_t*   pCdb;
    MsgVsc_t*                   pVscMsg;
    U8*                         pVenBuffer;
    U32                         xferSize;
} tVSCInternalInfo_t;

// VSC FW config structure
typedef  struct
{
    U8      FatalErrorHeader[16];
    U32     AsicErrorCode;
    U32     NandErrorCode;
    U32     DramErrorCode;
    U32     SpiErrorCode;
    U32     OthersErrorCode;
}tVSC_FatalError;

typedef  struct
{
    U8      ClusterHeader[16];
    U8      ClusterType[16];
    U8      CountOfUserData[16];
    U8      EraseCountEntrySize[16];
    U8      ValidCountEntrySize[16];
}tVSC_Cluster;

typedef  struct
{
	U8      NandHeader[16];
    U8      NandFlashId[16];
    U32     ChCnt;
    U32     CeCnt;
    U32     LunCnt;
    U32     DieCnt;
    U32     PlaneWrite;
    U32     PlaneRead;
    U32     LaaSizeByte;
    U32     reserved;
    U32     PageSizeInByte;
    U32     PageCntInBlk;
}tVSC_NandInfo;

typedef  struct
{
    U8                  FwConfigHeader[16];
    U8                  AsicNickName[16];
    U8                  AsicRevision[16];
    U64                 AgingHeader;
    U32                 AgingRevision;
    U32                 IsAgingFw;
    U8                  VscBufferSizeInByte[16];
    U8                  SysinfoBufferSizeInByte[16];
    U8                  PcbaSn[16];
    U8                  DriveSn[20];
    U8                  reserved0[12];
    U8                  FwSvnRevision[16];
    U8                  reserved1[32];
    tVSC_FatalError     FatalError;
    U8                  reserved2[28];
    tVSC_Cluster        Cluster;
    U8                  reserved3[48];
    tVSC_NandInfo       NandInfo;
    U8                  reserved4[48];
}tVSC_FwConfig;

//DW12.DW13 Data Structure
typedef  union
{
    U32 all32[2];
    struct{
        U32        VscHwOffset;
        U32        VscHwOpLen;
    }hw;
    struct{
        U32        VscfwOp;
        U32        reserved;
    }fw;
    struct{
        U32        VscSiOffset;
        U32        VscSiOpLen;
    }si;
    struct{
        U8         VscAgEn;
        U16        VscAgLoop;
        U8         VscPlistCopyMode;
        U8         VscAgMode;
        U8         reserved[3];
    }ag;
    struct{
        U32        VscTbOffset;
        U32        VscTbHeadTag;
    }tb;
    struct{
        U32        VscEccLba;
        U32        VscEccLen;
    }ft;
}tVSC_SubData;

typedef Error_t (*tVSC_CMD_Fn)(HcmdQ_t*, tVSCInternalInfo_t* pVscInternalInfo);

typedef union
{
    U8 all;
    struct
    {
        U8 vDirFC:1;
        U8 vDirFD:1;
        U8 vDirFE:1;
        U8 reserved:5;
    }b;
} tVSC_Attr;

typedef struct
{
    tVSC_CMD_Fn  Fnt;
    tVSC_Attr    Attr;
    U8           cmdCode;
}tVSC_Fnt;

typedef union
{
    U32   all32[4];
    struct
    {
        U32  hEcMax;
        U32  hEcAvg;
        U32  hEcMin;
        U32  hEc_reserved;
    }VscEc_h;
    struct
    {
        U32  hVcMax;
        U32  hVcAvg;
        U32  hVcMin;
        U32  hEc_reserved;
    }VscVc_h;
    struct
    {
        U16  hGl_DefErcnt;
        U16  hGl_DefWrcnt;
        U16  hGl_DefRdcnt;
        U8   hGl_reserved1[10];
    }VscGl_h;
    struct
    {
        U16  hEcc_Cnt;
        U8   hEcc_reserved1[14];
    }VscECC_h;
}VscTabPmt_h;

typedef struct
{
    U32         hTag;
    U16         hType;
    U16         hVersion;
    U32         hLength;
    U32         hOffset;
    VscTabPmt_h hParameter;
}tVSC_Htbp;
COMPILE_ASSERT(sizeof(tVSC_Htbp)==32, "NVMe Table Head size shall be 32");

//-----------------------------------------------------------------------------
//  VSC Read FW Model String
//-----------------------------------------------------------------------------
typedef struct
{
    const U16 Info_Add;
    const U8  ModelInfo[16];
} menuItem_t;

typedef struct
{
    const menuItem_t *pMenuItems;
    const U8  nItems;
} menu_t;

static menuItem_t pMenuItems[]=
{
    {0x000,    "LITE-ON NVMe VSC"},
    {0x010,    "ELDORA"},
    {0x020,    "2.0"},
    {0x030,    "Aging"},
    {0x0C0,    "Fatal Error"},
    {0x100,    "Cluster"},
    {0x180,    "Nand Flash"}
};
static menu_t pMenu = {pMenuItems,sizeof(pMenuItems) / sizeof(pMenuItems[0])};
//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
