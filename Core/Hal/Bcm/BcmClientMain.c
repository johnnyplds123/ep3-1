// ============================================================================
//
//     Copyright (c) Marvell Corporation 2000-2014  -  All rights reserved
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
//! \file
//! @Test BCM functions
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------

#include "BuildPkg.h"
#include "SharePkg.h"
#include "bcmClientSection.h"
#include "BcmClientPrintf.h"
#include "BcmClientTypedef.h"
#include "BcmClientUtilities.h"
//#include "BcmClientEbg.h"
//#include "BcmClientOtp.h"
#include "BcmClient.h"
#include "BcmClientMaes.h"
//#include "bcmClientTestDrivers.h"
//#include "BcmClientVersion.h"
//#include "BcmInterfaceClientTypes.h"
#include "BcmClientFunctions.h"
#include "BcmClientVars.h"
#if _TCG_ != TCG_NONE
#include "TcgCommon.h"
#include "TcgTbl.h"
#endif


//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------

#define BCM_MAES_INIT_TEST      DISABLE //ENABLE
#define BCM_MAES_SELF_TEST      DISABLE //ENABLE
#define BCM_MAES_BATCH_CMD_TEST DISABLE //ENABLE

#define BCM_EBG_TEST            DISABLE //ENABLE
#define BCM_OTP_TEST            DISABLE //ENABLE
#define BCM_RELOAD_TEST         DISABLE //ENABLE

#define BCM_AES_TEST_DRIVER     DISABLE //ENABLE
#define BCM_HASH_TEST_DRIVER    DISABLE //ENABLE
#define BCM_HMAC_TEST_DRIVER    DISABLE //ENABLE
#define BCM_PKCS15_TEST_DRIVER  DISABLE //ENABLE
#define BCM_APB_TEST_DRIVER     DISABLE //ENABLE
#define BCM_MRE_AES_TEST        DISABLE //ENABLE

#define BCM_EROM_RLS_PACKAGE    TRUE

#define BCM_BUF_DW_SZ           (256/32)


//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function proto-type definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported data proto-type without header include
//-----------------------------------------------------------------------------
#if _CPU_==CPU_ID2 && CO_SUPPORT_BCM
extern tG3                  *pG3;
extern sEnabledLockingTable *pLockingRangeTable;
#endif

//-----------------------------------------------------------------------------
//  Imported function proto-type without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported Parameter proto-tpye
//-----------------------------------------------------------------------------
uint8_t gBcmInitStat;
//-----------------------------------------------------------------------------
//  Data
//-----------------------------------------------------------------------------
#if _CPU_==CPU_ID0
BcmClientVars bcmClientVars;
U32 bcmTmpBuf1[BCM_BUF_DW_SZ];
U32 bcmTmpBuf2[SHA256_DIGEST_SIZE];
#endif

#if CO_SUPPORT_BCM

#if _TCG_ == TCG_NONE

U32  aesKey0[] = {
        0x00000000,
        0x11111111,
        0x22222222,
        0x33333333,
        0x44444444,
        0x55555555,
        0x66666666,
        0x77777777
     };

U32  aesKey1[] = {
        0x88888888,
        0x99999999,
        0xaaaaaaaa,
        0xbbbbbbbb,
        0xcccccccc,
        0xdddddddd,
        0xeeeeeeee,
        0xffffffff
     };
#endif

U32 keyXts[] =   {
        0x33333333,
        0x44444444,
        0x55555555,
        0x66666666,
        0x77777777,
        0x88888888,
        0x99999999,
        0xaaaaaaaa,
    };

U32 iv128[] =    {
        0x1516557e,
        0xd2a628ae,
        0x1588abf7,
        0x4f3c09cf,
        // --
        0x56786634,
        0xd2a628ae,
        0x1588abf7,
        0x4f3c19cf
    };

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

extern void BCM_check_status (STATUS status);
extern void Chk_BCM_irom_version(U32*);
extern void Chk_BCM_erom_version(U32*);
extern void BcmMaesInit(void);
extern void maes_keyRAM_zerorize(void);
//extern void BcmMaesReadKeyIVs(u32 rangeNum, MaesARangeKeyIV_t *pki);
extern void BCM_RSA_verify_erom_image(U32*);
extern Error_t BcmMreDmacTest(void);
extern void InitBcmVars(void);


//#if(BCM_EROM_RLS_PACKAGE == TRUE)
//    u32 pbuf[2048/32];
//#else
//    u32 *pbuf;
//#endif

#if 0 //cj
//-----------------------------------------------------------------------------
/**
    Name:           BCM_LoadERom

    Intialize BCM, download EROM image to BCM and start to run it.

    @param[in]  pCmdStr     command line string
    @param[in]  argc        argument count if auto-decode is enabled
    @param[in]  argv        argument values if auto-decode is enabled

    @return   error code
**/
//-----------------------------------------------------------------------------
void BCM_LoadERom (u32 sel)
{

    if (bcmClientVars.eromLoaded == 0)
    {
        bcmClientVars.eromLoadStarted = 1;

        //printVersionInfo();
        // InitBcmVars();
        #if(BCM_EROM_RLS_PACKAGE != TRUE)
        pbuf = MEM_AllocBuffer(32*KBYTE, 512); //MEM_AllocDtcmDram(1*KBYTE, 512);
        memCpy(pbuf, (u32 *)AXI_MEM_BASE_ADDR, 32*KBYTE/4);
        #endif
        printf_tube(": System Init Start\n");
        BCM_wait_for_boot();        // Prep for BCM EROM download

        //------------------------------------------------------------------------+
        //                                                          Load BCM EROM |
        //------------------------------------------------------------------------+
        BCM_check_status(BcmDownLoadErom((u32 *)AXI_MEM_BASE_ADDR, 0));
        // Restore it.
        #if(BCM_EROM_RLS_PACKAGE != TRUE)
        memCpy((u32 *)AXI_MEM_BASE_ADDR, pbuf, 32*KBYTE/4);
        #endif

        bcmClientVars.eromLoadStarted = 0;
        bcmClientVars.eromLoaded = 1;
    }
}


// Wrappers:
void BCM_LoadERomNoTokenVerify (void)
{
    BCM_LoadERom(0);
}

void BCM_LoadERomWithTokenVerify (void)
{
    BCM_LoadERom(2);
}
#endif

//-----------------------------------------------------------------------------
/**
    Name:           BCM_Test
    Description:

    @param[in]      None
    @param[out]     None

    @return         None
**/
//-----------------------------------------------------------------------------
#if 0
void BCM_Test (void)
{
    U32 i;
    STATUS status;
    //U32 *pbuf = (u32 *)AXI_MEM_BASE_ADDR;

    //printVersionInfo();

    // This should be in BCM start.s
    // InitBcmVars();

    //------------------------------------------------------------------------+
    //                                                  Initialize System Mem |
    //------------------------------------------------------------------------+
    simplePrintf("BCM Test Start\n");

    //------------------------------------------------------------------------+
    //                                                            BCM Boot-Up |
    //------------------------------------------------------------------------+
    // LEO - No need, someone cleared int already:
    BCM_check_status(BCM_wait_for_boot());

    //------------------------------------------------------------------------+
    //                                                          Load BCM EROM |
    //------------------------------------------------------------------------+
    Chk_BCM_irom_version( );

    BCM_reset();
    BCM_self_test(0);   //all
    BCM_reset();


    //EROM must be loaded whenever power up
    // This function verifies the EROM iamge with RSA PKCS V1.5 only.
    BCM_RSA_verify_erom_image( );

    if (bcmClientVars.eromLoaded == 0)
    {
        bcmClientVars.eromLoadStarted = 1;
        BCM_check_status(BcmDownLoadErom(pbuf, 0));
        bcmClientVars.eromLoadStarted = 0;
        bcmClientVars.eromLoaded = 1;
    }

    // Chk_BCM_irom_version( );
    Chk_BCM_erom_version( );


    //------------------------------------------------------------------------+
    //                                                         Start BCM Test |
    //------------------------------------------------------------------------+

    #if (BCM_RELOAD_TEST == ENABLE)

    if (bcmClientVars.eromLoaded != 0)
    {
        // first, unload the last EROM from BCM.
        HostCall_bcm_unload_EROM( );
        bcmClientVars.eromLoaded = 0;
    }

    // reload the EROM
    if (bcmClientVars.eromLoaded == 0)
    {
        bcmClientVars.eromLoadStarted = 1;
        BCM_check_status(BcmDownLoadErom(pbuf, 0));
        bcmClientVars.eromLoadStarted = 0;
        bcmClientVars.eromLoaded = 1;
    }

    // It should be OK to show ERROM alive.
    // And we will see an update at 0x0880_0000
    Chk_BCM_erom_version( );

    #endif

    #if (BCM_MAES_INIT_TEST == ENABLE)
        BcmMaesInit( );
    #endif

    #if (BCM_MRE_AES_TEST == ENABLE)
    BcmMreDmacTest( );
    #endif

   bcmClientVars.testLoops = 1000;

  while(--bcmClientVars.testLoops)
  {

    #if (BCM_MAES_BATCH_CMD_TEST == ENABLE)
        maes_batch_read_key(0);
        maes_batch_read_XTS_key(0);
        maes_batch_read_IV(0);
        maes_batch_keyRAM_zerorize();
    // #else
        BcmMaesReadKeyIVs(0, NULL);
        maes_keyRAM_zerorize( );
        BcmMaesReadKeyIVs(0, NULL);
    #endif

    #if (BCM_MAES_SELF_TEST == ENABLE)
        maes_eng0_self_test_failCase();
        maes_eng0_self_test_passCase();
    #endif

    #if (BCM_EBG_TEST == ENABLE)
        HostCall_bcm_check_entropy_pp_select( );
    #endif // End of #if (BCM_EBG_TEST == ENABLE)


    //------------------------------------------------------------------------+
    //                                                         Start ROM Test |
    //------------------------------------------------------------------------+
    #if (BCM_AES_TEST_DRIVER == ENABLE)
        BCM_aes_test_driver    ();
    #endif

    #if (BCM_HASH_TEST_DRIVER == ENABLE)
        BCM_hash_test_driver   ();
    #endif

    #if (BCM_HMAC_TEST_DRIVER == ENABLE)
        BCM_hmac_test_driver   ();
    #endif

    #if (BCM_PKCS15_TEST_DRIVER == ENABLE)
        BCM_pkcs15_test_driver ();
    #endif

  #if 1 // Note: Both of the tests below access OTP componet, but only one
        // can be enable to test. If both of them are enabled, the test
        // will fail with a timeout error that is caused by no interrupt status
        // from BCM for allowed time limit - Need to find why.
    #if (BCM_OTP_TEST == ENABLE)
        HostCall_bcm_erom_otp_read_test();
    #endif
  #else
    #if (BCM_APB_TEST_DRIVER == ENABLE)
        BCM_apb_test_driver ();
    #endif
  #endif

    simplePrintf(".");

  } // End of while(--bcmClientVars.testLoops);

    simplePrintf("BCM Test Done\n");
    TEST_END;
}

#endif

#if _CPU_ == CPU_ID0
void InitBcmVars(void)
{
    bcmClientVars.eromLoaded = 0;
    bcmClientVars.eromLoadStarted = 0;
    bcmClientVars.rd_err_cnt = 1;
    bcmClientVars.ipcCmdTimeout = 0;
    bcmClientVars.testLoops = 0;
    bcmClientVars.originStatus = (STATUS)0;
    bcmClientVars.firstStatus = (STATUS)0;
    bcmClientVars.delayedStatus = (STATUS)0;
    bcmClientVars.stSave = 0;
    bcmClientVars.stReads = 0;
    bcmClientVars.stReadsMax = 0;
}

//-----------------------------------------------------------------------------
/**
    One time Initialization of BCM HW.\n

    @return error code
**/
//-----------------------------------------------------------------------------
Error_t BCM_OneTimeInit(InitBootMode_t initMode)
{
    //D_PRINTF("[BCM ] BCMOneTimeInit\n");
    U32 i;
    STATUS status;
    U32 *pbuf;  // = (u32 *)AXI_MEM_BASE_ADDR;

    //printVersionInfo();

    // This should be in BCM start.s
    InitBcmVars();

    //------------------------------------------------------------------------+
    //                                                  Initialize System Mem |
    //------------------------------------------------------------------------+
    //simplePrintf("SEC Init:\n");

    //------------------------------------------------------------------------+
    //                                                            BCM Boot-Up |
    //------------------------------------------------------------------------+
    // LEO - No need, someone cleared int already:
    BCM_check_status(BCM_wait_for_boot());

    //------------------------------------------------------------------------+
    //                                                          Load BCM EROM |
    //------------------------------------------------------------------------+
    pbuf = (U32*)MEM_AllocBuffer(1024 * 16, 4);
    Chk_BCM_irom_version(pbuf);

    BCM_reset();
    BCM_self_test(0);   //all
    BCM_reset();

#if 1
    //EROM must be loaded whenever power up
    // This function verifies the EROM iamge with RSA PKCS V1.5 only.
    BCM_RSA_verify_erom_image(pbuf);

    if (bcmClientVars.eromLoaded == 0)
    {
        bcmClientVars.eromLoadStarted = 1;
        BCM_check_status(BcmDownLoadErom(pbuf, 0));
        bcmClientVars.eromLoadStarted = 0;
        bcmClientVars.eromLoaded = 1;
    }

    // Chk_BCM_irom_version( );
    Chk_BCM_erom_version(pbuf);
#endif
    MEM_FreeBuffer(pbuf);

    BcmMaesInit( );
#if _TCG_ == TCG_NONE
    HAL_SEC_InitAesKeyRng();
#endif
#if _TCG_ != TCG_NONE
    //-------------------------------------------------------
    printf_tube(": Hash Test\n");
    for(i=0; i<G1.b.mAdmCPin_Tbl.val[CPIN_MSID_IDX].pwd[0]; i++)
        *((U8*)bcmTmpBuf1+i) = G1.b.mAdmCPin_Tbl.val[CPIN_MSID_IDX].pwd[1+i];

    BCM_hash_engine(bcmTmpBuf1, bcmTmpBuf2, 2, G1.b.mAdmCPin_Tbl.val[CPIN_MSID_IDX].pwd[0], 64);
    //for(i=0; i<8; i++)  D_PRINTF("%8X ", bcmTmpBuf2[i]);  D_PRINTF("\n");

    if(bcmTmpBuf2[0]!=0x3511A5D6) {
        D_PRINTF("Hash NG!! %8\n", bcmTmpBuf2[0]);
        ASSERT(0);
    }
#endif
    gBcmInitStat = TRUE;
    return cEcNoError;
}

//
// NAME:    HAL_SEC_InitAesKeyRng(void)
//
void HAL_SEC_InitAesKeyRng(void)
{
#if _TCG_ != TCG_NONE
    U32 ii, rng;

    maes_keyRAM_zerorize();

    for(ii=0;ii<=LOCKING_RANGE_CNT;ii++) {
        rng=mLockingRangeTable[ii].rangeNo;
        AesProgramKey(ii, G3.b.mLckKAES_256_Tbl.val[rng].key, keyXts, iv128);

        if(rng==0) // global range
            break;
    }
#else
    AesProgramKey(0, aesKey1, keyXts, iv128);
#endif
    HAL_SEC_SetAesRange();
    //AesRangeRecordReset();
}

//
// NAME:     HAL_SEC_SetAesKey
//
void HAL_SEC_SetAesKey(U8 RangeNo, U32 *key1ValPtr)
{
    if (RangeNo >= TotalAESRangeNumber)
        return;

    AesProgramKey(RangeNo, key1ValPtr, keyXts, iv128);
    AesRangeRecordReset();
    return;
}
void HAL_SEC_GetAesKey(U8 RangeNo){
    BcmMaesReadKeyIVsNoAutoInc (RangeNo);
}
void HAL_SEC_SetAesRange(void)
{
#if _TCG_ != TCG_NONE
    U32 ii, mode;

    //mPcieDma.pcieDmaCtrl4.b.AES_EN = 1;
    for(ii=0;ii<=LOCKING_RANGE_CNT;ii++){
        mode = mLockingRangeTable[ii].writeLocked + (mLockingRangeTable[ii].readLocked<<1);

        if(mLockingRangeTable[ii].rangeNo)
            AesSetRangeTbl(ii, mLockingRangeTable[ii].rangeStart, mLockingRangeTable[ii].blkcnt, mode);
        else { // global range
            AesSetRangeTbl(ii, 0x00, 0xFFFFFFFF, mode);
            break;
        }
    }
#else
    //mPcieDma.pcieDmaCtrl4.b.AES_EN = 1;

    AesSetRangeTbl(0, 0x00, 0xFFFFFFFF, 0x00);
#endif

    AesRangeRecordReset();

}

/**********************************************************************
* NAME:     HAL_SEC_CollectEntropySamples
*
* Collect entropy samples (or random bits) from the digital entropy source.
* Each sample has 32 (16 x 2) bits. The number samples to collect is specified
* by numSamples as an input of the function. pData is another input which
* is a pointer to an available memory space that is large enough to
* hold the collected samples.
***********************************************************************/
void HAL_SEC_CollectEntropySamples(U32 *pData, U32 numSamples)
{
    U32 i;

    if(numSamples>BCM_BUF_DW_SZ)
        numSamples = BCM_BUF_DW_SZ;

    BCM_check_status(HostCall_erom_select_entropy_WithPP());    // RNG setting

    BCM_check_status(HostCall_bcm_drbg_gen_random_bits(bit256, bcmTmpBuf1));
    D_PRINTF("* RNG: %08X...\n", bcmTmpBuf1[0]);
    for(i=0; i<numSamples; i++)
        pData[i] = bcmTmpBuf1[i];
}

//-----------------------------------------------------------------------------
/**
    Security FlushAll
**/
//-----------------------------------------------------------------------------
void Nvme_Security_FlushAll(void)
{
#if 0
    Msg_t* pMsg;

    pMsg = IPC_GetFreeShareMsg();
    MSG_SET_HOST_NON_PERF_HEADER(*pMsg, MC_FLUSH_ALL);
    IPC_MsgQFastWrite(H2M_REQ_FAST_QUEUE, pMsg);
    IPC_WaitMsgExecuted(pMsg);
#endif
}

/**********************************************************************
* NAME:    HAL_Sha256(...)
*
***********************************************************************/
void HAL_Sha256(U8 *src, U8 len, U8 *dest)
{
    U32 i;
    U8* pt1 = (U8*)bcmTmpBuf1 ;
    U8* pt2 = (U8*)bcmTmpBuf2 ;

    if(len>(BCM_BUF_DW_SZ*4))
        ASSERT(0);

    memClr(BCM_BUF_DW_SZ*4, pt1);
    for(i=0; i<len; i++)
        *(pt1+i) = src[i];

    BCM_hash_engine(bcmTmpBuf1, bcmTmpBuf2, 2, len, 64);
    for(i=0; i<SHA256_DIGEST_SIZE; i++)
        dest[i] = *(pt2+i);

    memClr(BCM_BUF_DW_SZ*4, pt1);
}

void BcmRd(U32 port, U32 cmd, U32* pdata)
{
    BCM_check_status( BCM_apb_read(port, cmd, pdata));
}

void BcmWt(U32 port, U32 cmd, U32 data)
{
    BCM_check_status( BCM_apb_write(port, cmd, data));
}

#endif // _CPU_ == CPU_ID0

//MRE-Related Functions Start Here (for CPU_ID0 and CPU_ID2)
void MreAesDecryptionEnable(bool enable)
{
    U32 rdata;

    // RC
    BCM_check_status( BCM_apb_read (2, RC_AES_CFG, &rdata));

    if (enable == true)
    {
        rdata |= bit(0);    //RC_IP_MODE = 1h
        BCM_check_status( BCM_apb_write(2, RC_AES_CFG, rdata));
    }
    else  // if (enable ==  false)
    {
        rdata &= ~bit(0);
        BCM_check_status( BCM_apb_write(2, RC_AES_CFG, rdata));
    }
}

/**********************************************************************
* NAME:    HAL_MRE_Wr_AesPathOn(void)
*
***********************************************************************/
void MreAesEncryptionEnable(bool enable)
{
    U32 rdata;

    // WC
    BCM_check_status( BCM_apb_read (2, WC_AES_CFG, &rdata));
    if (enable == true)
    {
        rdata |= bit(0);   // WC_IP_MODE = 1
        BCM_check_status( BCM_apb_write(2, WC_AES_CFG, rdata));
    }
    else  // if (enable == false)
    {
        rdata &= ~bit(0);
        BCM_check_status( BCM_apb_write(2, WC_AES_CFG, rdata));
    }
}

/**********************************************************************
* NAME:    HAL_MRE_Wr_AesInit(void)
*
***********************************************************************/
void HAL_MRE_AesInit(U32 range)
{
    U32 i;
    U32 wdata;
#if 0
    // RC
    //BCM_check_status( BCM_apb_write(2, RC_AES_CTRL, 1));      // AES Reset
    //BCM_check_status( BCM_apb_write(2, RC_AES_CTRL, 0));
    // WC
    //BCM_check_status( BCM_apb_write(2, WC_AES_CTRL, 1));      // AES Reset
    //BCM_check_status( BCM_apb_write(2, WC_AES_CTRL, 0));

    wdata = ((U32)AesRawIVMode<<12) + ((U32)AesIVMsk_M16b<<8) + ((U32)Aes256BitKey<<4);
    BCM_check_status( BCM_apb_write (2, RC_AES_CFG, wdata));
    BCM_check_status( BCM_apb_write (2, WC_AES_CFG, wdata));

    // XTS KEY      D_PRINTF("\nXTS: ");
    for(i = 0; i < 8; i++) {
        BCM_check_status( BCM_apb_write(2, RC_AES_KEY2_WORD0 + (4 * i), keyXts[i] ));
    }

    for(i = 0; i < 8; i++) {
        BCM_check_status( BCM_apb_write(2, WC_AES_KEY2_WORD0 + (4 * i), keyXts[i] ));
    }

    // AES KEY/
D_PRINTF("A_Key:");
#if _CPU_==CPU_ID0
        for(i = 0; i < 8; i++)  {
            D_PRINTF("%X ", G3.b.mLckKAES_256_Tbl.val[ range ].key[i]);
            BCM_check_status( BCM_apb_write(2, RC_AES_KEY1_WORD0 + (4 * i), G3.b.mLckKAES_256_Tbl.val[ range ].key[i] ));
        }
D_PRINTF("\n");
        for(i = 0; i < 8; i++)  {
            D_PRINTF("%X ", G3.b.mLckKAES_256_Tbl.val[ range ].key[i]);
            BCM_check_status( BCM_apb_write(2, WC_AES_KEY1_WORD0 + (4 * i), G3.b.mLckKAES_256_Tbl.val[ range ].key[i] ));
        }
#elif _CPU_==CPU_ID2
        for(i = 0; i < 8; i++)  {
            BCM_check_status( BCM_apb_write(2, RC_AES_KEY1_WORD0 + (4 * i), pG3->b.mLckKAES_256_Tbl.val[ range ].key[i] ));
        }
        for(i = 0; i < 8; i++)  {
            BCM_check_status( BCM_apb_write(2, WC_AES_KEY1_WORD0 + (4 * i), pG3->b.mLckKAES_256_Tbl.val[ range ].key[i] ));
        }
#endif

    // IV
    for(i = 0; i < 4; i++)  {
        BCM_check_status( BCM_apb_write(2, RC_AES_IV_WORD0 + (4 * i), iv128[i] ));
    }
    for(i = 0; i < 4; i++)  {
        BCM_check_status( BCM_apb_write(2, WC_AES_IV_WORD0 + (4 * i), iv128[i] ));
    }

    // AES stream size:
    BCM_check_status( BCM_apb_write(2, RC_AES_STREAMSIZE, (512 >> 2) ));   //RC: streamSize = 0
    BCM_check_status( BCM_apb_write(2, WC_AES_STREAMSIZE, (512 >> 2) ));   //WC: streamSize = 512

    // AES skip size
    BCM_check_status( BCM_apb_write(2, RC_AES_SKIPSIZE, 0 ));   // RC
    BCM_check_status( BCM_apb_write(2, WC_AES_SKIPSIZE, 0 ));   // WC

    MreAesDecryptionEnable(true);      // read channel AES data path ON
    MreAesEncryptionEnable(false);
#else

    // WC
    //BCM_check_status( BCM_apb_write(2, WC_AES_CTRL, 1));      // AES Reset
    //BCM_check_status( BCM_apb_write(2, WC_AES_CTRL, 0));

    wdata = ((U32)AesRawIVMode<<12) + ((U32)AesIVMsk_M16b<<8) + ((U32)Aes256BitKey<<4);
    BCM_check_status( BCM_apb_write (2, WC_AES_CFG, wdata));

    for(i = 0; i < 8; i++) {
        BCM_check_status( BCM_apb_write(2, WC_AES_KEY2_WORD0 + (4 * i), keyXts[i] ));
    }
#if _TCG_ != TCG_NONE
#if _CPU_==CPU_ID0
    for(i = 0; i < 8; i++)  {
        BCM_check_status( BCM_apb_write(2, WC_AES_KEY1_WORD0 + (4 * i), G3.b.mLckKAES_256_Tbl.val[ range ].key[i] ));
    }
#elif _CPU_==CPU_ID2
    for(i = 0; i < 8; i++)  {
        BCM_check_status( BCM_apb_write(2, WC_AES_KEY1_WORD0 + (4 * i), pG3->b.mLckKAES_256_Tbl.val[ range ].key[i] ));
    }
#endif
#endif
    for(i = 0; i < 4; i++)  {
        BCM_check_status( BCM_apb_write(2, WC_AES_IV_WORD0 + (4 * i), iv128[i] ));
    }

    // AES stream size:
    BCM_check_status( BCM_apb_write(2, RC_AES_STREAMSIZE, 0 ));             //RC: streamSize = 0
    BCM_check_status( BCM_apb_write(2, WC_AES_STREAMSIZE, (512 >> 2) ));   //WC: streamSize = 512

    // AES skip size
    BCM_check_status( BCM_apb_write(2, RC_AES_SKIPSIZE, 0 ));   // RC
    BCM_check_status( BCM_apb_write(2, WC_AES_SKIPSIZE, 0 ));   // WC

    MreAesDecryptionEnable(false);      // read channel AES data path off
    MreAesEncryptionEnable(true);
#endif
}

/**********************************************************************
* NAME:     HAL_MRE_Wr_AesIsBusy
*
* Is AES write change busy ?
***********************************************************************/
U32 HAL_MRE_Wr_AesBusy(void)
{
    U32 rdata;

    BCM_check_status( BCM_apb_read (2, WC_AES_STATUS, &rdata));

    return rdata & 0x01;
}

/**********************************************************************
* NAME:    HAL_MRE_Wr_AesReset(void)
*
***********************************************************************/
void HAL_MRE_Wr_AesReset(void)
{
    BCM_check_status( BCM_apb_write(2, WC_AES_CTRL, 1));      // AES Reset
    BCM_check_status( BCM_apb_write(2, WC_AES_CTRL, 0));
}

#endif  //#if CO_SUPPORT_BCM
