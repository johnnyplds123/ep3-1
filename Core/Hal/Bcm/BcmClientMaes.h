///////////////////////////////////////////////////////////////////////////// 
//                                    NOTICE                               //
//                                                                         //
//            COPYRIGHT MARVELL INTERNATIONAL LTD. AND ITS AFFILIATES      //
//                             ALL RIGHTS RESERVED                         //
//                                                                         //
//    The source code for this computer program is  CONFIDENTIAL  and a    //
//    TRADE SECRET of MARVELL  INTERNATIONAL  LTD. AND  ITS  AFFILIATES    //
//    ('MARVELL'). The receipt or possession of  this  program does not    //
//    convey any rights to  reproduce or  disclose  its contents, or to    //
//    manufacture,  use, or  sell  anything  that it  may  describe, in    //
//    whole or in part, without the specific written consent of MARVELL.   //
//    Any  reproduction  or  distribution  of this  program without the    //
//    express written consent of MARVELL is a violation of the copyright   //
//    laws and may subject you to criminal prosecution.                    //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef INCLUDE_MAES_H
#define INCLUDE_MAES_H


typedef enum ebgBitCnt
{
    bit128 = 0,
    bit256 = 1,
} ebgBitCnt_t;

//AES Config1 [R1A10E008]
typedef enum
{
    AesNormalMode = 0,
    AesBypassMode = 1,
    //AESMediaReEncMode = 2,
    //AESMediaReEncLoopback = 3,
    AesSelfCheckMode = 4
} mtAesIPMode;

typedef enum
{
    Aes128BitKey = 0,
    Aes256BitKey = 1,
    Aes192BitKey = 2
} mtAesKeySize;

typedef enum
{
    AesRawIVMode = 0,
    AesSwapIVMode = 1,
    AesXorIVMode = 2
} mtAesIVMode;

typedef enum
{
    AesIVMsk_M16b = 0,
    AesIVMsk_M32b = 1,
    AesIVMsk_M48b = 2,
    AesIVMsk_M64b = 3,
    AesIVMsk_L16b = 4,
    AesIVMsk_L32b = 5,
    AesIVMsk_L48b = 6,
    AesIVMsk_L64b = 7
} mtAesIVMsk;

STATUS HostCall_erom_select_entropy_WithPP(void);
STATUS HostCall_bcm_drbg_gen_random_bits(ebgBitCnt_t length, U32 *pBuf);
//void BcmMaesEnable(void);
//void BcmMaesDisable(void);
BOOL AesSetRangeTbl(U32 RangeNo, U32 startLBA, U32 blkcnt, U32 indTbl);
void AesProgramKey(U8 RangeNo, U32 *key1ValPtr, U32 *key2ValPtr, U32 *ivPtr);
void AesRangeRecordReset(void);

void BcmMaesReadKeyIVsNoAutoInc(U32 rangeNum);


//-----------------------------------------------------------------------------
/**
    Name:           maes_eng0_self_test_passCase
    
    Description:    run an AES engine 0 self test, and expect it to pass.
    
    @param[out]     statusReg  - content of the AES_ENG0_STATUS register
                                 bit[5] = 0 to indicate a pass result.

    @return         None
**/
//-----------------------------------------------------------------------------
void maes_eng_self_test_passCase(U32);

//-----------------------------------------------------------------------------
/**
    Name:           maes_eng0_self_test_failCase
    
    Description:    run an AES engine 0 self test, and expect it to fail.
    
    @param[out]     statusReg  - content of the AES_ENG0_STATUS register. Its
                                 bit[5] = 1 to indicate a failure.

    @return         None
**/
//-----------------------------------------------------------------------------
void maes_eng_self_test_failCase(U32);



//-----------------------------------------------------------------------------
/**
    Name:   STATUS  BCM_read_MAES_key
    
    Description:    Read a MAES key of a range in batch process mode, i.e. BCM executes 
                    all functions for a single client command.
                    
                    Data will be put on a buffer, U32 pBuf[8], which is supplied by caller.
                    
    @param[in]      U32 rangeNum.
    @param[in]      U32 *pBuf, a pointer to an array of 8 words or larger, which 
                               will hold the key read from MAES.
                               
    @return         status code
**/
//-----------------------------------------------------------------------------
STATUS BCM_read_MAES_key(U32 rangeNum, U32 *pBuf);


//-----------------------------------------------------------------------------
/**
    Name:   STATUS  BCM_read_MAES_XTS_key
    
    Description:    Read a MAES XTS key of a range in batch process mode, i.e. BCM executes 
                    all functions for a single client command.
                    
                    Data will be put on a buffer, U32 pBuf[8], which is supplied by caller.
                    
    @param[in]      U32 rangeNum.
    @param[in]      U32 *pBuf, a pointer to an array of 8 words or larger, which 
                               will hold the XTS key read from MAES.
                               
    @return         status code
**/
//-----------------------------------------------------------------------------
STATUS BCM_read_MAES_XTS_key(U32 rangeNum, U32 *pBuf);


//-----------------------------------------------------------------------------
/**
    Name:   STATUS  BCM_read_MAES_IV
    
    Description:    Read a MAES IV of a range in batch process mode, i.e. BCM executes 
                    all functions for a single client command.
                    
                    Data will be put on a buffer, U32 pBuf[4], which is supplied by caller.
                    
    @param[in]      U32 rangeNum.
    @param[in]      U32 *pBuf, a pointer to an array of 4 words or larger, which 
                               will hold the IV read from MAES.
                               
    @return         status code
**/
//-----------------------------------------------------------------------------
STATUS BCM_read_MAES_IV(U32 rangeNum, U32 *pBuf);


//-----------------------------------------------------------------------------
/**
    Name:   STATUS  maes_batch_read_key
    
    Description:    Read a MAES key of a range in batch process mode, i.e. BCM executes 
                    all functions for a single client command.
                    
                    Data will be put on a buffer, U32 pBuf[8], which is an auto variable of 
                    this function.
                    
    @param[in]      U32 rangeNum.
                               
    @return         None
**/
//-----------------------------------------------------------------------------
//
void maes_batch_read_key(U32 rangeNum);


//
//-----------------------------------------------------------------------------
/**
    Name:   STATUS  maes_batch_read_XTS_key
    
    Description:    Read a MAES XTS key of a range in batch process mode, i.e. BCM executes 
                    all functions for a single client command.
                    
                    Data will be put on a buffer, U32 pBuf[8], which is an auto variable of 
                    this function.
                    
    @param[in]      U32 rangeNum.
                               
    @return         None.
**/
//-----------------------------------------------------------------------------
void maes_batch_read_XTS_key(U32 rangeNum);

//-----------------------------------------------------------------------------
/**
    Name:   STATUS  maes_batch_read_IV
    
    Description:    Read a MAES IV of a range in batch process mode, i.e. BCM executes 
                    all functions for a single client command.
                    
                    Data will be put on a buffer, U32 pBuf[4], which is an auto variable of 
                    this function.
                    
    @param[in]      U32 rangeNum.
                               
    @return         None
**/
//-----------------------------------------------------------------------------
void maes_batch_read_IV(U32 rangeNum);




void maes_batch_keyRAM_zerorize(void);


#endif
