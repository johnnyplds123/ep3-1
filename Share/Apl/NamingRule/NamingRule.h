#pragma once
// ============================================================================
//
// ==        Copyright (c) LITEON Corporation  -  All rights reserved        ==
/*
    This computer program contains confidential and proprietary information,
    and may NOT be reproduced or transmitted, in whole or in part, in any form,
    or by any means electronic, mechanical, photo-optical, or otherwise, and
    may NOT be translated into another language without the express written
    permission from LITEON Corporation.
*/
// ==        Copyright (c) LITEON Corporation  -  All rights reserved        ==
//
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Constants definitions: NVMe Setting
//-----------------------------------------------------------------------------
// NVMe Identify
#define NVME_ID_BYTEMAX_SERIALNUMBER    20
#define NVME_ID_BYTEMAX_MODELNAME       40
#define NVME_ID_BYTEMAX_FWREVISION      8
#define NVME_ID_BYTE_SPARE              4

//-----------------------------------------------------------------------------
//  Constants definitions: ModelName Naming Rule
//-----------------------------------------------------------------------------
#define MODEL_STRING_RAMDISK        "RamDisk"

//-----------------------------------------------------------------------------
//  Constants definitions: HW Naming Rule
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Constants definitions: FW Naming Rule
//-----------------------------------------------------------------------------
#define FW_CAPACITY_NA              'x'
#define FW_CAPACITY_RAMDISK         'r'

#define FW_CAPACITY_128             '7'
#define FW_CAPACITY_256             '8'
#define FW_CAPACITY_512             '9'
#define FW_CAPACITY_1024            'A'
#define FW_CAPACITY_2048            'B'
#define FW_CAPACITY_4096            'C'
#define FW_CAPACITY_COUNT_OP7       6

#define FW_CAPACITY_120             'J'
#define FW_CAPACITY_240             'M'
#define FW_CAPACITY_480             'N'
#define FW_CAPACITY_960             'W'
#define FW_CAPACITY_1920            'Y'
#define FW_CAPACITY_3840            'Z'
#define FW_CAPACITY_COUNT_OP14      6

#define FW_CAPACITY_100             FW_CAPACITY_NA
#define FW_CAPACITY_200             FW_CAPACITY_NA
#define FW_CAPACITY_400             FW_CAPACITY_NA
#define FW_CAPACITY_800             FW_CAPACITY_NA
#define FW_CAPACITY_1600            'X'
#define FW_CAPACITY_3200            FW_CAPACITY_NA
#define FW_CAPACITY_COUNT_OP20      6

//COMPILE_ASSERT(FW_CAPACITY_COUNT_OP7 == FW_CAPACITY_COUNT_OP14, "FW_CAPACITY_COUNT ERROR");
#define FW_CAPACITY_COUNT           FW_CAPACITY_COUNT_OP7

//-----------------------------------------------------------------------------
//  Macro
//-----------------------------------------------------------------------------
#define SET_MODEL_STRING(index, string)     {MEM_COPY(&LiteOnModelName[index], string, sizeof(string) - 1); \
                                             index += sizeof(string) - 1;}

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
typedef struct CapacityMap_t
{
    U16 CapacityInGb;
    U8  CapacityChar;
    U8  Reserved;
} CapacityMap_t;

//-----------------------------------------------------------------------------
//  Exported variable reference
//-----------------------------------------------------------------------------
extern  U8                  LiteOnModelName[NVME_ID_BYTEMAX_MODELNAME   + NVME_ID_BYTE_SPARE];
extern  U8                  LiteOnFWRevision[NVME_ID_BYTEMAX_FWREVISION + NVME_ID_BYTE_SPARE];
extern  const CapacityMap_t gCapacityMap[OP_LIST_COUNT][FW_CAPACITY_COUNT];
extern  U32                 gCapacityIndex;

//-----------------------------------------------------------------------------
//  Exported function reference
//-----------------------------------------------------------------------------
Error_t AplNamingRule_OneTimeInit(InitBootMode_t initMode);

/* End of File */

