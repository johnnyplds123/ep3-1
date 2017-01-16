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
#define SUP_NUM_OF_STREAMID             9
#define STREAMID_MASK_ID                0xF
#define STREAMID_MASK_ND                0xF

#if (CO_SUPPORT_DIRECTIVES == TRUE)
    #define SUP_DIRE_STREAMS            ENABLE
    #define SUP_DIRE_ADVBGCTRL          DISABLE
    #define SUP_EVENTNOTI               DISABLE
    #define SUP_EVENTNOTI_STREAMS       DISABLE
    #define SUP_EVENTNOTI_ADVBGCTRL     DISABLE
#else
    #define SUP_DIRE_STREAMS            DISABLE
    #define SUP_DIRE_ADVBGCTRL          DISABLE
    #define SUP_EVENTNOTI               DISABLE
    #define SUP_EVENTNOTI_STREAMS       DISABLE
    #define SUP_EVENTNOTI_ADVBGCTRL     DISABLE
#endif

#define DIRE_TYPE_IDENTIFY              0x00
#define DIRE_TYPE_STREAMS               0x01
#define DIRE_TYPE_ADVBGCTRL             0x40

// ---- Receive
#define IDENTIFY_OP_RETURNPARA          01
#define IDENTIFY_OP_EVENTNOTIF          02

#define STREAM_OP_RETURNPARA            01
#define STREAM_OP_GETSTA                02
#define STREAM_OP_ALLOCRES              03

#define ADVBGCTL_OP_RETURNPARA          01
#define ADVBGCTL_OP_RETURNSTA           02

// ---- Send
#define IDENTIFY_OP_ENABLEDIRE          01
#define IDENTIFY_OP_EVENTCLR            02

#define STREAM_OP_RELIDENT              01
#define STREAM_OP_RELRES                02

#define ADVBGCTL_OP_SETPARA             01
#define ADVBGCTL_OP_START               02
#define ADVBGCTL_OP_STOP                03

// ---- CMD Fetcher Copy DDR Structure
#define CMDF_SIZE_PER_CMD               72
#define CMDF_DW_PER_CMD                 (CMDF_SIZE_PER_CMD / 4)

#define CMDF_SHIFT_DTYPE                (0x14)
#define CMDF_SHIFT_DSPEC                (0x10)

#define CMDF_MASK_SQID                  (0x0000001F)
#define CMDF_MASK_OPC                   (0x000000FF)
#define CMDF_MASK_DTYPE                 (0x00F00000)
#define CMDF_MASK_DSPEC                 (0xFFFF0000)

#define CMDF_OFFSET_DW0                 (0 + 2)
#define CMDF_OFFSET_DW10                (10 + 2)
#define CMDF_OFFSET_DW12                (12 + 2)
#define CMDF_OFFSET_DW13                (13 + 2)

#define STREAM_SWS                      (128 * KBYTE)
#define STREAM_SGS                      (  1 * GBYTE)
//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief Directive Error Code
typedef enum {
    cErr_Dire_NoErr = 0x00,
    cErr_Dire_Type  = 0x01,
    cErr_Dire_Op    = 0x02,
} tDireErrCode;

/// @brief NVMe Admin Command structure - Directive Send
typedef  struct
{
    AdminCommandCommon_t DW9_0; ///< DW9..0

    uint32_t NUMD;              ///< Number of Dwords       (DW10)
    uint32_t DOPER:8;           ///< Directive Operation    (DW11 bits[7:0])
    uint32_t DTYPE:8;           ///< Directive Type         (DW11 bits[15:8])
    uint32_t DSPEC:16;          ///< Directive Specific     (DW11 bits[31:16])

    union
    {
        uint32_t dw12;                  ///< DW12
        struct                          ///< --- Enable Directive
        {
            uint32_t enable_ENDIR:1;    ///< Specifies the Directive Type for which to provide event notification
            uint32_t enable_resv1:7;    ///< Reserve
            uint32_t enable_DTYPE:8;    ///< This field specifies the Directive Type to enable or disable
            uint32_t enable_resv16:16;  ///< Reserve
        };
        struct                          ///< --- Event Clear
        {
            uint32_t evnClr_DTYPE:8;    ///< This field specifies the Directive Type for which to clear an event
            uint32_t evnClr_resv8:24;   ///< Reserve
        };
        struct                          ///< --- Set Parameters
        {
            uint32_t setPar_OPPARAM:8;  ///< This field indicates the operational parameters for Advanced Background Control operations.
            uint32_t setPar_resv8:8;    ///< Reserve
            uint32_t setPar_WARTH:16;   ///< This field indicates the threshold to alert the host of limited write allocation resources
        };
        struct                          ///< --- Set Parameters
        {
            uint32_t start_ABCT:8;      ///< This field specifies the maximum time that the controller shall perform host-initiated advanced background control
            uint32_t start_resv8:8;     ///< Reserve
            uint32_t start_WARTARG:16;  ///< This field indicates the target of available write allocation resources
        };
    };
    uint32_t reserved11[3];     ///< DW15..13
} NvmeAdminDirectiveSendCommand_t;
COMPILE_ASSERT(sizeof(NvmeAdminDirectiveSendCommand_t) == 64, "NVMe command size shall be 64");

/// @brief NVMe Admin Command structure - Directive Recieve
typedef  struct
{
    AdminCommandCommon_t DW9_0; ///< DW9..0

    uint32_t NUMD;              ///< Number of Dwords       (DW10)
    uint32_t DOPER:8;           ///< Directive Operation    (DW11 bits[7:0])
    uint32_t DTYPE:8;           ///< Directive Type         (DW11 bits[15:8])
    uint32_t DSPEC:16;          ///< Directive Specific     (DW11 bits[31:16])

    union
    {
        uint32_t dw12;                  ///< DW12
        struct                          ///< --- Enable Directive
        {
            uint32_t event_DTYPE:8;     ///< Specifies the Directive Type for which to provide event notification
            uint32_t event_resv:24;     ///< Reserve
        };
        struct                          ///< --- Event Clear
        {
            uint32_t alRes_NSR:16;      ///< This field specifies the number of streams resources the host is requesting be allocated
            uint32_t alRes_resv:16;     ///< Reserve
        };
    };

    uint32_t reserved_1[3];     ///< DW15..13
} NvmeAdminDirectiveRecieveCommand_t;
COMPILE_ASSERT(sizeof(NvmeAdminDirectiveRecieveCommand_t) == 64, "NVMe command size shall be 64");

/// @brief Direct Return Parameter
typedef struct
{
    // Directives Supported
    U32 supp_Identify:1;    // DW0
    U32 supp_Streams:1;
    U32 rsv_supp0:30;

    U32 rsv_supp1;          // DW1

    U32 supp_AdvBgCon:1;    // DW2
    U32 rsv_supp2:31;
    U32 rsv_supp3[5];       // DW3 ~ DW7

    // Directives Enable
    U32 enab_Identify:1;    // DW8
    U32 enab_Streams:1;
    U32 rsv_enab0:30;

    U32 rsv_enab1;          // DW9

    U32 enab_AdvBgCon:1;    // DW10
    U32 rsv_enab2:31;
    U32 rsv_enab3[5];       // DW11 ~ DW15

    // Directives Event Notification support
    U32 evno_Identify:1;    // DW16
    U32 evno_Streams:1;
    U32 rsv_evno:30;

    U32 rsv_evno1;          // DW17

    U32 evno_AdvBgCon:1;    // DW18
    U32 rsv_evno2:31;
    U32 rsv_evno3[5];       // DW19 ~ DW23

    U32 rsv_96[1000];       // DW24 ~ DW1023
} DirecIdentifyReturnPara_t;
COMPILE_ASSERT(sizeof(DirecIdentifyReturnPara_t) == 4096, "Return Parameters Data Structure size shall be 4096");

/// @brief Stream Return Parameter
typedef struct
{
    // Directives Supported
    U32 msl:16;     // Max Stream Limit                 -- DW0
    U32 nssa:16;    // NVM Subsystem Streams Available
    U32 nsso:16;    // NVM Subsystem Streams Open       -- DW1
    U32 rsv_1:16;
    U32 rsv_2[2];   //                                  -- DW2 ~ 3
    U32 sws;        // Stream Write Size                -- DW4
    U32 sgs:16;     // Stream Granularity Size          -- DW5
    U32 nsa:16;     // Namespace Streams Allocated
    U32 nso:16;     // Namespace Streams Open           -- DW6
    U32 rsv_3:16;
    U32 rsv_4;      //                                  -- DW7
} StreamReturnPara_t;
COMPILE_ASSERT(sizeof(StreamReturnPara_t) == 32, "Return Parameters Data Structure size shall be 32");

/// @brief Advanced Background Control Return Parameter
typedef struct
{
    // Directives Supported
    U32 warmin:16;      // DW0
    U32 warmaxt:16;
    U32 warth:16;       // DW1
    U32 reserve_1:16;
    U32 opmode:8;       // DW2
    U32 rsv_2:24;
    U32 rsv_3[5];       // DW3~7
} AdvBgReturnPara_t;
COMPILE_ASSERT(sizeof(AdvBgReturnPara_t) == 32, "Return Parameters Data Structure size shall be 32");

/// @brief Advanced Background Control Return Status
typedef struct
{
    // Directives Supported
    U32 opstat:8;   // DW0
    U32 rsv_1:8;
    U32 warav:16;
    U32 rsv_2[7];   // DW1~7
} AdvBgReturnSta_t;
COMPILE_ASSERT(sizeof(AdvBgReturnSta_t) == 32, "Return Status Data Structure size shall be 32");

/// @brief Advanced Background Control Event Notification
typedef struct
{
    U32 status:8;   // DW0
    U32 rsv_1:8;
    U32 warav:16;
    U32 rsv_2[7];   // DW1~7
} AdvBgEventNoti_t;
COMPILE_ASSERT(sizeof(AdvBgEventNoti_t) == 32, "ABC Event Notification size shall be 32");

typedef struct
{
    uint32_t allocCnt;
    uint32_t openCnt;
    uint16_t idTable[SUP_NUM_OF_STREAMID];
} StreamInfo_t;

typedef struct
{
    U32 en_Stream:1;
    U32 en_AdvBgCtrl:1;
    U32 en_rsv:30;
    StreamInfo_t streamInfo[NVME_NUM_OF_NAMESPACES];
} NvmeDirectiveMgr_t;

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern uint8_t              gCmdfPtr;
extern uint32_t*            pCmdfBuf;
extern NvmeDirectiveMgr_t   gDirectiveMgr;

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
extern void Directive_WriteHandling(MsgHostIO_t* pHcmdMsg, uint32_t nsid);
extern void Directive_Init(InitBootMode_t initMode);
