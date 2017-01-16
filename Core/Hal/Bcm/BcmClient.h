#pragma once

//-----------------------------------------------------------------------------
// Title         : BCM Client
// File          : bcm_client.h
// Description   : Contains all the BCM function calls for main FW
//
//-----------------------------------------------------------------------------

//#ifndef _BCM_CLIENT_H
//#define _BCM_CLIENT_H

#define SHA256_DIGEST_SIZE ( 256 / 8)

#define   TotalAESRangeNumber    32

//AES Indication Table
#define RNG_WT_LOCKED   0x01        //b0: Reverse of Range Write Enable.
#define RNG_RD_LOCKED   0x02        //b1: Reverse of Range Read Enable
#define RNG_DISABLE     0x04        //b2:
#define RNG_BYPASS      0x08

extern uint8_t gBcmInitStat;

void BCM_Test(void);
Error_t BCM_OneTimeInit(InitBootMode_t initMode);
void BcmMaesEnable(void);
void BcmMaesDisable(void);
void HAL_SEC_InitAesKeyRng(void);
void HAL_SEC_SetAesKey(U8 RangeNo, U32 *key1ValPtr);
void HAL_SEC_SetAesRange(void);
void HAL_SEC_CollectEntropySamples(U32 *pData, U32 numSamples);
void Nvme_Security_FlushAll(void);

void HAL_MRE_AesInit(U32 range);
U32 HAL_MRE_Wr_AesBusy(void);
void HAL_MRE_Wr_AesReset(void);
void HAL_Sha256(U8 *src, U8 len, U8 *dest);


void BcmRd(U32 port, U32 cmd, U32* pdata);
void BcmWt(U32 port, U32 cmd, U32 data);

Error_t BCM_RSA_verify_fw_image(U32 *pFw, U32 fwSize);



//#endif
