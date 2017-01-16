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
#include "BuildPkg.h"
#include "SharePkg.h"
#include "SysInfo.h"

//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------
void NamingRule_HandleModelName(void);
void NamingRule_HandleFWName(void);
U32  NamingRule_GetCapacityIndex(U32 UserSizeInGB);
U32  NamingRule_SetCapacityString(U32 index);

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
COMPILE_ASSERT(_OP_ < OP_LIST_COUNT, "Unsupported OP");

const CapacityMap_t gCapacityMap[OP_LIST_COUNT][FW_CAPACITY_COUNT] =
{
    {128,   FW_CAPACITY_128,    0,
     256,   FW_CAPACITY_256,    0,
     512,   FW_CAPACITY_512,    0,
     1024,  FW_CAPACITY_1024,   0,
     2048,  FW_CAPACITY_2048,   0,
     4096,  FW_CAPACITY_4096,   0},
    {120,   FW_CAPACITY_120,    0,
     240,   FW_CAPACITY_240,    0,
     480,   FW_CAPACITY_480,    0,
     960,   FW_CAPACITY_960,    0,
     1920,  FW_CAPACITY_1920,   0,
     2048,  FW_CAPACITY_3840,   0}, // 2048 for 2GB DRAM L2P limitatiom
    #if (OP_LIST_COUNT > 2)
    {100,   FW_CAPACITY_100,    0,
     200,   FW_CAPACITY_200,    0,
     400,   FW_CAPACITY_400,    0,
     800,   FW_CAPACITY_800,    0,
     1600,  FW_CAPACITY_1600,   0,
     2048,  FW_CAPACITY_3200,   0}  // 2048 for 2GB DRAM L2P limitatiom
    #endif
};

U8  LiteOnModelName[NVME_ID_BYTEMAX_MODELNAME   + NVME_ID_BYTE_SPARE];
U8  LiteOnFWRevision[NVME_ID_BYTEMAX_FWREVISION + NVME_ID_BYTE_SPARE];
U32 gCapacityIndex;

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    NamingRule one time initialization function

    @param[in]  initMode
**/
//-----------------------------------------------------------------------------
Error_t AplNamingRule_OneTimeInit(InitBootMode_t initMode)
{
    U8  *DriveSN;

    DriveSN = MEM_AllocBuffer(NVME_ID_BYTEMAX_SERIALNUMBER + NVME_ID_BYTE_SPARE, 4);

    MEM_SET(&LiteOnModelName[0],  0x20 ,NVME_ID_BYTEMAX_MODELNAME);
    MEM_SET(&LiteOnFWRevision[0], 0x20 ,NVME_ID_BYTEMAX_FWREVISION);
    MEM_SET(&DriveSN[0],          0x20 ,NVME_ID_BYTEMAX_SERIALNUMBER);

    #if (_MEDIA_ != MEDIA_DRAM)
    gCapacityIndex = NamingRule_GetCapacityIndex(gNfInfo.userCapacity);
    #endif

    NamingRule_HandleModelName(); // take care model name
    NamingRule_HandleFWName();    // take care FW name

    MEM_COPY(DriveSN, smSysInfo->d.MPInfo.d.SerialNumberDrive, NVME_ID_BYTEMAX_SERIALNUMBER);

    //String Termination
    MEM_SET(&LiteOnModelName[NVME_ID_BYTEMAX_MODELNAME],   0x00, NVME_ID_BYTE_SPARE);
    MEM_SET(&LiteOnFWRevision[NVME_ID_BYTEMAX_FWREVISION], 0x00, NVME_ID_BYTE_SPARE);
    MEM_SET(&DriveSN[NVME_ID_BYTEMAX_SERIALNUMBER],        0x00, NVME_ID_BYTE_SPARE);

    /*
    D_PRINTF("[Driv] Model|%s\n",      LiteOnModelName);
    D_PRINTF("[Driv] FW   |%s\b-%s\n", LiteOnFWRevision, _FW_REV_SUB_);
    D_PRINTF("[Driv] SN   |%s\n",      DriveSN);
    */
    D_PRINTF("[Driv] MD|%s\n"
             "[Driv] FW|%s\b-%s\n"
             "[Driv] SN|%s\n", LiteOnModelName, LiteOnFWRevision, _FW_REV_SUB_, DriveSN);

    MEM_FreeBuffer(DriveSN);

    return cEcNoError;
}

//-----------------------------------------------------------------------------
/**
    Assign cacpacity character of model name
**/
//-----------------------------------------------------------------------------
void NamingRule_HandleModelName(void)
{
    U32 index;

    MEM_COPY(&LiteOnModelName[0], _MODEL_NAME_, strlen(_MODEL_NAME_));

    index = strlen(_MODEL_NAME_) - 1;
    index = NamingRule_SetCapacityString(index);

    #if (_CUSTOMER_ == CUSTOMER_DELL)
    index = strlen(_MODEL_NAME_) - 1;

    index = NamingRule_SetCapacityString(index);
    LiteOnModelName[index++] = 'G';
    LiteOnModelName[index++] = 'B';
    #endif
}

//-----------------------------------------------------------------------------
/**
    Assign cacpacity character of firmware name
**/
//-----------------------------------------------------------------------------
void NamingRule_HandleFWName(void)
{
    MEM_COPY(&LiteOnFWRevision[0], _FW_REV_, strlen(_FW_REV_));

    // Byte 2
    #if (_MEDIA_ == MEDIA_DRAM)
        LiteOnFWRevision[2] = FW_CAPACITY_RAMDISK;
    #else
        LiteOnFWRevision[2] = gCapacityMap[_OP_][gCapacityIndex].CapacityChar;
    #endif
}

//-----------------------------------------------------------------------------
/**
    Assign cacpacity character of model name

    @param[in]  capacity in GB
    @retun      capacity index
**/
//-----------------------------------------------------------------------------
U32 NamingRule_GetCapacityIndex(U32 UserSizeInGB)
{
    U32  i;

    for (i = 0; i < FW_CAPACITY_COUNT; i++)
    {
        if (UserSizeInGB == gCapacityMap[_OP_][i].CapacityInGb)
        {
            return i;
        }
    }

    ASSERT(0);

    return 0;
}

//-----------------------------------------------------------------------------
/**
    Convert capacity to ascii number

    @param[in]  current capacity index
    @retun      capacity index after modified
**/
//-----------------------------------------------------------------------------
U32 NamingRule_SetCapacityString(U32 index)
{
    #if (_MEDIA_ == MEDIA_DRAM)
    SET_MODEL_STRING(index, MODEL_STRING_RAMDISK)
    #else

    U32 size = gCapacityMap[_OP_][gCapacityIndex].CapacityInGb;
    U32 numOfDigit = 0;
    U8  byteArray[6];

    while (size)
    {
        byteArray[numOfDigit++] = (size % 10) + '0';
        size /= 10;
    }

    do
    {
        LiteOnModelName[index++] = byteArray[--numOfDigit];
    } while(numOfDigit);

    #endif

    return index;
}

/* End of File */

