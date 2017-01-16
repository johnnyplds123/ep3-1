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
//! @Entropy Bit Generation (Random Number Collection) funtions.
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
#include "RegClockPLL.h"
#include "BcmInterfaceClientTypes.h"
#include "BcmClient.h"
#include "BcmClientMaes.h"



//-----------------------------------------------------------------------------
//  Constants definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Private function proto-type definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported data proto-type without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function proto-type without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

#define PCIE        0
#define NVME        1
#define HOST_INTERFACE      NVME


extern void maes_keyRAM_zerorize(void);
extern void BcmMaesReadKeyIVs(U32 rangeNum, MaesARangeKeyIV_t *pki);
extern void BcmMaesReadKeyIVsNoAutoInc(U32 rangeNum);

volatile ClockPll_t *prClockPll = (ClockPll_t *)0xF0112000;                ///< 0xF0112000-0xF0113080 : ClockPll_t MODULE


#if (HOST_INTERFACE == PCIE)
///  @brief 0xF0104008 : PCIe DMA Control 4
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t AES_EN              : 1;         ///<BIT [0] AES Enable.
        uint16_t RF0104008_15_1_RSVD :15;         ///<BIT [15:1] Reserved.
    } b;
} PcieDmaCtrl4_t;

///  @brief 0xF0104024 : PCIe DMA Status 2
typedef union
{
    uint16_t all;///< All bits.
    struct
    {
        uint16_t RF0104024_2_0_RSVD   : 3;         ///<BIT [2:0] Reserved.
        uint16_t PDMA_ERROR           : 1;         ///<BIT [3] PCIe DMA Error.
        uint16_t MPECC_ERR            : 1;         ///<BIT [4] MPECC Error Status.
        uint16_t HLBA_MMATCH          : 1;         ///<BIT [5] Host LBA Mismatch Error Status.
        uint16_t BCRC_ERR             : 1;         ///<BIT [6] BCRC Error Status.
        uint16_t ECU_ERR              : 1;         ///<BIT [7] ECU Error Status passed through Cut-Through Mode.
        uint16_t RF0104024_8_RSVD     : 1;         ///<BIT [8] Reserved.
        uint16_t PRD_FETCH_ERR        : 1;         ///<BIT [9] PRD Fetch Error Status.
        uint16_t XFERCNT_MMATCH       : 1;         ///<BIT [10] Transfer Count Mismatch Error Status.
        uint16_t AES_ERROR            : 1;         ///<BIT [11] AES Error Status.
        uint16_t AXI1_RD_TO_ERR       : 1;         ///<BIT [12] PCIe AXI Read Time Out Error Status.
        uint16_t AXI0_RD_TO_ERR       : 1;         ///<BIT [13] UMC AXI Read Time Out Error Status.
        uint16_t RF0104024_15_14_RSVD : 2;         ///<BIT [15:14] Reserved.
    } b;
} PcieDmaStat2_t;

#elif (HOST_INTERFACE == NVME)

typedef union
{
    uint32_t all;///< All bits.
    struct
    {
        uint32_t LBA0_META_SIZE       :16;         ///<BIT [15:0] LBA0 Metadata Size.
        uint32_t LBA0_DATA_SIZE       : 3;         ///<BIT [18:16] LBA0 Data Size.
        uint32_t LBA0_AU_RATIO        : 3;         ///<BIT [21:19] LBA0 Allocation Unit (AU) Ratio.
        uint32_t LBA0_MPECC_EN        : 1;         ///<BIT [22] LBA0 MPECC Enable.
        uint32_t LBA0_HLBA_EN         : 1;         ///<BIT [23] Host LBA 0 Enable.
        uint32_t LBA0_HLBA_SIZE       : 2;         ///<BIT [25:24] LBA0 Host LBA Size.
        uint32_t LBA0_BCRC_EN         : 1;         ///<BIT [26] LBA0 BCRC Enable.
        uint32_t LBA0_AES_EN          : 1;         ///<BIT [27] LBA0 AES Enable.
        uint32_t RF5011040_31_28_RSVD : 4;         ///<BIT [31:28] Reserved.
    } b;
} LbaFmt_t;


///  @brief 0xF5011040 : LBA Format 0
///  @brief 0xF5011044 : LBA Format 1
///  @brief 0xF5011048 : LBA Format 2
///  @brief 0xF501104C : LBA Format 3
typedef struct LBA_Formats
{
    LbaFmt_t lbaFormat[4];
} LBA_Formats;


volatile LbaFmt_t *pLbaFmt = (LbaFmt_t *)0xF5011040;

__inline void lbaFmtEnableAES(uint32_t lbaFmtNum)
{
    pLbaFmt[lbaFmtNum].b.LBA0_AES_EN = 1;
}

__inline void lbaFmtDisableAES(uint32_t lbaFmtNum)
{
    pLbaFmt[lbaFmtNum].b.LBA0_AES_EN = 0;
}

#else
  #error    Undefined Interface Type!
#endif

//#define TEST_AUTO_INC_PTR       1

STATUS HostCall_erom_select_entropy_WithPP(void)
{
    STATUS status;
    status = Host_IPC_Cmd_2BCM (EROM_SELECT_ENTROPY_PP, 0x10000, 1, 1);
    return(status);
}

//pBuf must be located at DDR!!
STATUS HostCall_bcm_drbg_gen_random_bits(ebgBitCnt_t length, U32 *pBuf)
{
    STATUS status;
    status = Host_IPC_Cmd_2BCM (BCM_DRBG_GEN_RAN_BITS, 0x10000, 2, 128, pBuf);
    if((length!=bit128) && (status==STATUS_SUCCESS))
        status = Host_IPC_Cmd_2BCM (BCM_DRBG_GEN_RAN_BITS, 0x10000, 2, 128, pBuf+128/8/sizeof(U32));
    return(status);  //BCM_DRBG_GENERATE
}

void BcmMaesEnable(void)
{
    // Enable all lba format (need at least one to enable MAES engine)
    lbaFmtEnableAES(0);
    //lbaFmtEnableAES(1);
    //lbaFmtEnableAES(2);
    //lbaFmtEnableAES(3);
}

void BcmMaesDisable(void)
{
    // Enable all lba format (need at least one to enable MAES engine)
    lbaFmtDisableAES(0);
    //lbaFmtDisableAES(1);
    //lbaFmtDisableAES(2);
    //lbaFmtDisableAES(3);
}

void AesRangeRecordReset(void)
{
    U32 rdata;

#if 1 //cj test...
        // soft reset
    BCM_check_status( BCM_apb_read (0, AES_ENG0_CMD, &rdata));
        rdata |= bit(0);
    BCM_check_status( BCM_apb_write(0, AES_ENG0_CMD, rdata));
        rdata &= ~bit(0);
    BCM_check_status( BCM_apb_write(0, AES_ENG0_CMD, rdata));

        BCM_check_status( BCM_apb_read (0, AES_ENG1_CMD, &rdata));
        rdata |= bit(0);
        BCM_check_status( BCM_apb_write(0, AES_ENG1_CMD, rdata));
        rdata &= ~bit(0);
        BCM_check_status( BCM_apb_write(0, AES_ENG1_CMD, rdata));
#endif

    // Eng0 (Encryption) toggle and set as Auto mode: 1 means auto-record (from Marvell/Eddie)
    BCM_check_status( BCM_apb_read (0, AES_ENG0_CTRL, &rdata));
    rdata |= bit(8);        //if b8=1, it will cause LBA0 NG after Win8.1 "Disk Initialization"
    BCM_check_status( BCM_apb_write(0, AES_ENG0_CTRL, rdata));
    rdata &= ~bit(8);
    BCM_check_status( BCM_apb_write(0, AES_ENG0_CTRL, rdata));

    // Eng0 Range Record reset
    BCM_check_status( BCM_apb_read (0, AES_ENG0_CMD, &rdata));
    rdata |= bit(8);
    BCM_check_status( BCM_apb_write(0, AES_ENG0_CMD, rdata));
    rdata &= ~bit(8);
    BCM_check_status( BCM_apb_write(0, AES_ENG0_CMD, rdata));



    // Eng1 (Decryption) toggle and set as Auto mode
    BCM_check_status( BCM_apb_read (0, AES_ENG1_CTRL, &rdata));
    rdata |= bit(8);        //if b8=1, it will cause LBA0 NG after Win8.1 "Disk Initialization"
    BCM_check_status( BCM_apb_write(0, AES_ENG1_CTRL, rdata));
    rdata &= ~bit(8);
    BCM_check_status( BCM_apb_write(0, AES_ENG1_CTRL, rdata));

    // Eng1 Range Record reset
    BCM_check_status( BCM_apb_read (0, AES_ENG1_CMD, &rdata));
    rdata |= bit(8);
    BCM_check_status( BCM_apb_write(0, AES_ENG1_CMD, rdata));
    rdata &= ~bit(8);
    BCM_check_status( BCM_apb_write(0, AES_ENG1_CMD, rdata));

}

//
// Set AES LBA table, range would be enabled only if "locked" is false.
// * need to set LBA ranges whenever PcieDMA is soft-reset.
//
BOOL AesSetRangeTbl(U32 RangeNo, U32 startLBA, U32 blkcnt, U32 indTbl)
{
    U32 tmp32, rdata;
#ifdef AES_DBG
    D_PRINTF("* ASR: %2X %X %X %2X\n", RangeNo, startLBA, blkcnt, indTbl);
#endif
    //if (RangeNo >= TotalAESRangeNumber)        return FALSE;

    tmp32 = ((U32)0x01<<RangeNo);

    //Write Enable Table
    BCM_check_status( BCM_apb_read(0, SHR_MIS_RNG_WR_EN, &rdata));
    if(indTbl&RNG_WT_LOCKED)
        rdata &= ~tmp32;
    else
        rdata |= tmp32;
    BCM_check_status( BCM_apb_write(0, SHR_MIS_RNG_WR_EN, rdata));

    //Read Enable Table
    BCM_check_status( BCM_apb_read(0, SHR_MIS_RNG_RD_EN, &rdata));
    if(indTbl&RNG_RD_LOCKED)
        rdata &= ~tmp32;
    else
        rdata |= tmp32;
    BCM_check_status( BCM_apb_write(0, SHR_MIS_RNG_RD_EN, rdata));

    //Valid Table
    BCM_check_status( BCM_apb_read(0, SHR_MIS_RNG_VALID, &rdata));
    if(indTbl&RNG_DISABLE)
        rdata &= ~tmp32;
    else
        rdata |= tmp32;
    BCM_check_status( BCM_apb_write(0, SHR_MIS_RNG_VALID, rdata));

    // Bypass setup
    BCM_check_status( BCM_apb_read(0, SHR_MIS_RNG_BYP, &rdata));
    if(indTbl&RNG_BYPASS)
        rdata |= tmp32;
    else
        rdata &= ~tmp32;
    BCM_check_status( BCM_apb_write(0, SHR_MIS_RNG_BYP, rdata));


    // Non-Automatic pointer increment
    BCM_check_status( BCM_apb_read (0, SHR_CTRL, &rdata));
    rdata &= ~bit(0);
    BCM_check_status( BCM_apb_write(0, SHR_CTRL, rdata));

    // AES range setup
    BCM_check_status( BCM_apb_write(0, SHR_LBA_ACC_PTR, RangeNo<<4));
    BCM_check_status( BCM_apb_write(0, SHR_LBA_ACC_DW0, startLBA));
    BCM_check_status( BCM_apb_write(0, SHR_LBA_ACC_DW1, 0x00000000));

    BCM_check_status( BCM_apb_write(0, SHR_LBA_ACC_BCNT, blkcnt));

    return TRUE;

}


//
// Program "one XTS key + one XTS Tweak key + one  IV" for AES engine
//
void AesProgramKey(U8 RangeNo, U32 *key1ValPtr, U32 *key2ValPtr, U32 *ivPtr)
{
    U32 rdata;
    U32 i, tmp32;
#ifdef AES_DBG
    U32 buf[8];

    D_PRINTF("* APK: %2X %8X ", RangeNo, key1ValPtr[0]);
#endif
    // Non-Automatic pointer increment
    BCM_check_status( BCM_apb_read (0, SHR_CTRL, &rdata));
    rdata &= ~bit(0);
    BCM_check_status( BCM_apb_write(0, SHR_CTRL, rdata));

    // Program range 0 KEY
    tmp32 = ((U32)RangeNo<<4);
    BCM_check_status( BCM_apb_write(0, SHR_KEY1_ACC_PTR, tmp32));   //set Range Index (0~31)
    for(i = 0; i < 8; i++)
    {
        BCM_check_status( BCM_apb_write(0, SHR_KEY1_ACC_PTR, tmp32+i));
        BCM_check_status( BCM_apb_write(0, SHR_KEY1_ACC_DW, *key1ValPtr++));
    }

    // Program range 0 XTS KEY
    BCM_check_status( BCM_apb_write(0, SHR_KEY2_ACC_PTR, tmp32));
    for(i = 0; i < 8; i++)
    {
        BCM_check_status( BCM_apb_write(0, SHR_KEY2_ACC_PTR, tmp32+i));
        BCM_check_status( BCM_apb_write(0, SHR_KEY2_ACC_DW, *key2ValPtr++));
    }

    // Program range 0 IV
    BCM_check_status( BCM_apb_write(0, SHR_IV_ACC_PTR, tmp32));
    for(i = 0; i < 4; i++)
    {
        BCM_check_status( BCM_apb_write(0, SHR_IV_ACC_PTR, tmp32+i));
        BCM_check_status( BCM_apb_write(0, SHR_IV_ACC_DW, *ivPtr++));
    }

    //key size: 256b
    if(RangeNo>=16){
        RangeNo -= 16;
        tmp32 = ((U32)Aes256BitKey<<(RangeNo*2));
        //BCM_check_status( BCM_apb_write(0, SHR_MIS_KSIZE0, 0x00));
        BCM_check_status( BCM_apb_read (0, SHR_MIS_KSIZE1, &rdata));
        rdata |= tmp32;
        BCM_check_status( BCM_apb_write(0, SHR_MIS_KSIZE1, rdata));
    }
    else {
        tmp32 = ((U32)Aes256BitKey<<(RangeNo*2));
        BCM_check_status( BCM_apb_read (0, SHR_MIS_KSIZE0, &rdata));
        rdata |= tmp32;
        BCM_check_status( BCM_apb_write(0, SHR_MIS_KSIZE0, rdata));
        //BCM_check_status( BCM_apb_write(0, SHR_MIS_KSIZE1, 0x00));
    }

#if AES_DBG
    BCM_check_status( BCM_read_MAES_key (RangeNo, buf));
    D_PRINTF("%X\n", buf[0]);
    //D_PRINTF(" - %X %X %X %X\n", buf[0], buf[1], buf[2], buf[3]);

    //BCM_check_status( BCM_read_MAES_XTS_key (RangeNo, buf));
    //D_PRINTF(" key2: %X %X %X %X\n", buf[0], buf[1], buf[2], buf[3]);

    //BCM_check_status( BCM_read_MAES_IV (RangeNo, buf));
    //D_PRINTF(" iv: %X %X %X %X\n", buf[0], buf[1], buf[2], buf[3]);
#endif

    return;
}

//-----------------------------------------------------------------------------
/**
    Name:           BcmMaesInit

    Description:    Initializing media AES to enable range0.

    @param[in]      None
    @param[out]     None

    @return         None
**/
//-----------------------------------------------------------------------------
void BcmMaesInit(void)
{
    U32 rdata;

    //already set @  HalNvme_OneTimeInit() [
    //prClockPll->powerCtrl.b.FORCE_AHCI_NVME_SEL = 1;
    //prClockPll->powerCtrl.b.AHCI_NVME_SEL = 1;
    // ]
    //printf_tube(": AES Init\n");

    BcmMaesEnable();

    //cj added [
    maes_eng_self_test_failCase(0);
    maes_eng_self_test_passCase(0);

    maes_eng_self_test_failCase(1);
    maes_eng_self_test_passCase(1);
    // ]

    maes_keyRAM_zerorize( );

    #if 0 //redundant code for keyRAM zeroize.
    BCM_check_status( BCM_apb_read (0, SHR_CMD, &rdata));
    rdata &= bit(4);
    BCM_check_status( BCM_apb_write(0, SHR_CMD, rdata));
    #endif

    // port select: port=0 for NVME
    BCM_check_status( BCM_apb_read (0, AES_ENG0_CFG, &rdata));
    rdata |= bit(24);
    BCM_check_status( BCM_apb_write(0, AES_ENG0_CFG, rdata));
    BCM_check_status( BCM_apb_read (0, AES_ENG1_CFG, &rdata));
    rdata |= bit(24);
    BCM_check_status( BCM_apb_write(0, AES_ENG1_CFG, rdata));


    // Set AES_ENG0_CFG: encoder
    rdata = ((U32)AesRawIVMode<<20) + ((U32)AesIVMsk_M16b<<16) + ((U32)AesNormalMode);
    BCM_check_status( BCM_apb_write(0, AES_ENG0_CFG, rdata));

    // Set AES_ENG1_CFG: decoder
    rdata = ((U32)AesRawIVMode<<20) + ((U32)AesIVMsk_M16b<<16) + ((U32)AesNormalMode);
    BCM_check_status( BCM_apb_write(0, AES_ENG1_CFG, rdata));

}

//
// Key1, Key2, IV memory zerorize
//
void maes_keyRAM_zerorize(void)
{
    U32 rdata;
//simplePrintf("keyRam_zerorize\n");
    BCM_check_status( BCM_apb_read (0, SHR_CMD, &rdata));
    rdata |= bit(4);    // Cmd ZerorizeStart (bit4)
    BCM_check_status( BCM_apb_write(0, SHR_CMD, rdata));
    do{
         // waits = 100;
         // while(--waits);
         BCM_check_status( BCM_apb_read (0, SHR_ENG_STATUS, &rdata));
    } while((rdata & bit(4)) != bit(4));    // waiting for ZerorizeStartDone flag to set.

    return;
}

#if 0
//
// Read keys and IVs from a range for debugging purpose
// Note: If the pointer, pki, is NULL, the key and IV will not be transferred out.
// If the pointer is not NULL, the key and IV will be copied out.
//
void BcmMaesReadKeyIVs(U32 rangeNum, MaesARangeKeyIV_t *pki)
{
    U32 rdata, i;
    U32 key[8];
    U32 XTSkey[8];
    U32 IV[4];

    // Automatic pointer increment
    BCM_check_status( BCM_apb_read (0, SHR_CTRL, &rdata));
    rdata |= bit(0);
    BCM_check_status( BCM_apb_write(0, SHR_CTRL, rdata));

#if (HOST_INTERFACE == NVME)
    // Select a range to read key
     BCM_check_status( BCM_apb_write(0, SHR_KEY1_ACC_PTR, (rangeNum << 4)|0));
    // Read rangeNum's KEY
    for(i = 0; i < 8; i++)  BCM_check_status( BCM_apb_read(0, SHR_KEY1_ACC_DW, &key[i]));
    // Read rangeNum's XTS KEY
     BCM_check_status( BCM_apb_write(0, SHR_KEY2_ACC_PTR, (rangeNum << 4)|0));
    for(i = 0; i < 8; i++)  BCM_check_status( BCM_apb_read(0, SHR_KEY2_ACC_DW, &XTSkey[i]));
    // Read rangeNum's IV
    BCM_check_status( BCM_apb_write(0, SHR_IV_ACC_PTR, (rangeNum << 4)|0));
    for(i = 0; i < 4; i++)  BCM_check_status( BCM_apb_read(0, SHR_IV_ACC_DW, &IV[i]));
#else
    #error Not defined for BcmMaesReadKeyIVs.
#endif

    if (pki != NULL)
    {
        U32 ii;
        for (ii = 0; ii < 8; ii++)
        {
            pki->key[ii] = key[ii];
            pki->XTSkey[ii] = XTSkey[ii];
        }
        for (ii = 0; ii < 4; ii++)
        {
            pki->IV[ii] = IV[ii];
        }
    }
    return;
}
#endif

//
// Read keys and IVs from a range for debugging purpose
// Note: data will be put on stack, and will not be valid out of the cope of this function.
//
void BcmMaesReadKeyIVsNoAutoInc(U32 rangeNum)
{
    U32 rdata, i;
    U32 key[8];
    U32 XTSkey[8];
    U32 IV[4];


    // Non-Automatic pointer increment
    BCM_check_status( BCM_apb_read (0, SHR_CTRL, &rdata));
    rdata &= ~bit(0);
    BCM_check_status( BCM_apb_write(0, SHR_CTRL, rdata));

#if (HOST_INTERFACE == NVME)
    // Read rangeNum's KEY
    D_PRINTF("RdK%2X:\n ", rangeNum);
    for(i = 0; i < 8; i++)
    {
        BCM_check_status( BCM_apb_write(0, SHR_KEY1_ACC_PTR, (rangeNum << 4)| i));
        BCM_check_status( BCM_apb_read(0, SHR_KEY1_ACC_DW, &key[i]));
        D_PRINTF("%X",key[i]);
    }

    D_PRINTF("\n ");
    // Read rangeNum's XTS KEY
    for(i = 0; i < 8; i++)
    {
        BCM_check_status( BCM_apb_write(0, SHR_KEY2_ACC_PTR, (rangeNum << 4)|i));
        BCM_check_status( BCM_apb_read(0, SHR_KEY2_ACC_DW, &XTSkey[i]));
        D_PRINTF("%X",XTSkey[i]);
    }

    // Read rangeNum's IV
    D_PRINTF("\n ");
    for(i = 0; i < 4; i++)
    {
        BCM_check_status( BCM_apb_write(0, SHR_IV_ACC_PTR, (rangeNum << 4)|i));
        BCM_check_status( BCM_apb_read(0, SHR_IV_ACC_DW, &IV[i]));
        D_PRINTF("%X", IV[i]);
    }
    D_PRINTF("\n");
#else
    #error Not defined for BcmMaesReadKeyIVsNoAutoInc.
#endif
}


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
STATUS BCM_read_MAES_key(U32 rangeNum, U32 *pBuf)
{
    U32 ii;
    U32 *ps;
    STATUS status;
    U32 wait = 0x100000;
    volatile MAILBOX *pMBOX = (MAILBOX *)BCM_MBOX_BASE_ADDR;

    status = Host_IPC_Cmd_2BCM (EROM_READ_MAES_KEY, wait, 1, rangeNum);

    if (status == STATUS_SUCCESS)
    {
        ps = (U32 *)&pMBOX->status0;
        for(ii = 0; ii < 8; ii++)
        {
            *pBuf++ = *ps++;     // Read data from IPC Mailbox.
        }
    }
    else
    {
        // Stuff the buffer with 0xEE..EE
        for(ii = 0; ii < 8; ii++)
        {
            *pBuf++ = 0xEEEEEEEE;
        }
    }

    return (status);
}

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
STATUS BCM_read_MAES_XTS_key(U32 rangeNum, U32 *pBuf)
{
    U32 ii;
    U32 *ps;
    STATUS status;
    U32 wait = 0x100000;
    volatile MAILBOX *pMBOX = (MAILBOX *)BCM_MBOX_BASE_ADDR;

    status = Host_IPC_Cmd_2BCM (EROM_READ_MAES_XTS_KEY, wait, 1, rangeNum);

    if (status == STATUS_SUCCESS)
    {
        ps = (U32 *)&pMBOX->status0;
        for(ii = 0; ii < 8; ii++)
        {
            *pBuf++ = *ps++;     // Read data from IPC Mailbox.
        }
    }
    else
    {
        // Stuff the buffer with 0xEE..EE
        for(ii = 0; ii < 8; ii++)
        {
            *pBuf++ = 0xEEEEEEEE;
        }
    }

    return (status);
}



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
STATUS BCM_read_MAES_IV(U32 rangeNum, U32 *pBuf)
{
    U32 ii;
    U32 *ps;
    STATUS status;
    U32 wait = 0x100000;
    volatile MAILBOX *pMBOX = (MAILBOX *)BCM_MBOX_BASE_ADDR;

    status = Host_IPC_Cmd_2BCM (EROM_READ_MAES_IV, wait, 1, rangeNum);

    if (status == STATUS_SUCCESS)
    {
        ps = (U32 *)&pMBOX->status0;
        for(ii = 0; ii < 4; ii++)
        {
            *pBuf++ = *ps++;     // Read data from IPC Mailbox.
        }
    }
    else
    {
        // Stuff the buffer with 0xEE..EE
        for(ii = 0; ii < 4; ii++)
        {
            *pBuf++ = 0xEEEEEEEE;
        }
    }

    return (status);
}

#if 0
//
// Read a MAES key of a range in batch process mode (BCM executes all functions for
// a single client command.)
// Note: data will be put on a buffer, U32 buf[8], which is an auto variable of this function.
//
void maes_batch_read_key(U32 rangeNum)
{
    U32 buf[8];
    // Automatic pointer increment
    BCM_check_status( BCM_read_MAES_key (0, buf));
}


//
// Read a MAES XTS key of a range in batch process mode (BCM executes all functions for
// a single client command.)
// Note: data will be put on a buffer, U32 buf[8], which is an auto variable of this function.
//
void maes_batch_read_XTS_key(U32 rangeNum)
{
    U32 buf[8];
    // Automatic pointer increment
    BCM_check_status( BCM_read_MAES_XTS_key (0, buf));
}


//
// Read a MAES IVs of a range in batch process mode (BCM executes all functions for
// a single client command.)
// Note: data will be put on a buffer, U32 buf[4], which is an auto variable of this function.
//
void maes_batch_read_IV(U32 rangeNum)
{
    U32 buf[4];

    // Automatic pointer increment
    BCM_check_status( BCM_read_MAES_IV (0, buf));
}

//
// Zerorize BCM key-RAM with one command from host side - named it as batch mode (BCM executes all functions for
// a single client command.)
//
void maes_batch_keyRAM_zerorize(void)
{
    STATUS status;
    U32 wait = 0x100000;

    status = Host_IPC_Cmd_2BCM (EROM_BATCH_KEYRAM_ZERORIZE, wait, 0);
    BCM_check_status( status);
}
#endif

//-----------------------------------------------------------------------------
/**
    Name:           maes_eng0_self_test_passCase

    Description:    run an AES engine 0 self test, and expect it to pass.

    @param[in]      pass        - calling to test pass case when it is 1, or a fail case if it is 0.
    @param[out]     statusReg   - content of the AES_ENG0_STATUS register
                                  bit[5] = 1 to indicate a failure.

    @return         None
**/
//-----------------------------------------------------------------------------
void maes_eng_self_test(U32 engNo, U32 pass, U32 *statusReg)
{
    U32 rdata;
    volatile U32 statusdata;
    U32 cfgSaved, ctrlSaved;
    U32 wait = 0x1fffffff;

    volatile U32 tmp1;
    volatile U32 tmp2;

    U32 aesCmd, aesStatus, aesCfg, aesCtrl;
    if(engNo==0){
        aesCmd = AES_ENG0_CMD;
        aesStatus = AES_ENG0_STATUS;
        aesCfg = AES_ENG0_CFG;
        aesCtrl = AES_ENG0_CTRL;
    }
    else {
        aesCmd = AES_ENG1_CMD;
        aesStatus = AES_ENG1_STATUS;
        aesCfg = AES_ENG1_CFG;
        aesCtrl = AES_ENG1_CTRL;
    }

    // Do a soft reset - adding this to fix a hang when
    // calling a self_test continuously.
    BCM_check_status( BCM_apb_write(0, aesCmd, 0x01));

    BCM_check_status( BCM_apb_read (0, aesCmd, (U32 *)&tmp1));
    BCM_check_status( BCM_apb_read (0, aesStatus, (U32 *)&tmp2));
    // BCM_check_status( BCM_apb_write(0, AES_ENG0_CMD, 0x0));

    // Set AES_ENG0_CFG[2:0] = 0x04 (IP mode = self test)
    BCM_check_status( BCM_apb_read (0, aesCfg, &rdata));
    cfgSaved = rdata;
    rdata = (rdata&(~0x07)) | 0x04;
    BCM_check_status( BCM_apb_write(0, aesCfg, rdata));

    // Set AES_ENG0_CTRL[4] = 0 (Do not force self test error)
    BCM_check_status( BCM_apb_read (0, aesCtrl, &rdata));
    ctrlSaved = rdata;
    if (pass)
    {
        rdata &= ~bit(4);
        BCM_check_status( BCM_apb_write(0, aesCtrl, rdata));
    }
    else    // force self test error
    {
        rdata |= bit(4);
        BCM_check_status( BCM_apb_write(0, aesCtrl, rdata));
    }

    // Set AES_ENG0_CMD = 0x10 (Start self test)
    BCM_check_status( BCM_apb_write(0, aesCmd, 0x10));

    while(--wait)
    {
        BCM_check_status( BCM_apb_read (0, aesStatus, (U32 *)&statusdata));
        if (statusdata & bit(4)) // Pend on AES_ENG0_STATUS[4] - test done bit.
        {
            *statusReg = statusdata;
            break;
        }
        continue;
    }

    // Restore
    BCM_check_status( BCM_apb_write(0, aesCfg, cfgSaved));
    BCM_check_status( BCM_apb_write(0, aesCtrl, ctrlSaved));

    return;
}

//-----------------------------------------------------------------------------
/**
    Name:           maes_eng0_self_test_passCase

    Description:    run an AES engine 0 self test, and expect it to pass.

    @param[out]     statusReg  - content of the AES_ENG0_STATUS register
                                 bit[5] = 0 to indicate a pass result.

    @return         None
**/
//-----------------------------------------------------------------------------
void maes_eng_self_test_passCase(U32 engNo)
{
    U32 statusReg;
    U32 passed = 1;

    maes_eng_self_test(engNo, passed, &statusReg);
#if AES_DBG
    //D_PRINTF("%s: %1X %X\n", __FUNCTION__, engNo, statusReg);
#endif
    return;
}

//-----------------------------------------------------------------------------
/**
    Name:           maes_eng0_self_test_failCase

    Description:    run an AES engine 0 self test, and expect it to fail.

    @param[out]     statusReg  - content of the AES_ENG0_STATUS register. Its
                                 bit[5] = 1 to indicate a failure.

    @return         None
**/
//-----------------------------------------------------------------------------
void maes_eng_self_test_failCase(U32 engNo)
{
    U32 statusReg;
    U32 passed = 0;

    maes_eng_self_test(engNo, passed, &statusReg);
#if AES_DBG
    //D_PRINTF("%s: %1X %X\n", __FUNCTION__, engNo, statusReg);
#endif
    return;
}

#if 0
//-----------------------------------------------------------------------------
/**
    Name:           BcmMaesBypassARange

    Description:    Set MAES to bypass a range

    @param[in]      rangeNum  - the range to bypass

    @return         None
**/
//-----------------------------------------------------------------------------
void BcmMaesBypassARange(U32 rangeNum)
{
    U32 rangeBypassRead;
    U32 rangeBypassBit = (1 << rangeNum);

    BCM_check_status( BCM_apb_read (0, SHR_MIS_RNG_BYP, &rangeBypassRead));
    BCM_check_status( BCM_apb_write(0, SHR_MIS_RNG_BYP, (rangeBypassRead|rangeBypassBit)));
}

//-----------------------------------------------------------------------------
/**
    Name:           BcmMaesNotBypassARange

    Description:    Set MAES to Not-bypass a range

    @param[in]      rangeNum  - the range to bypass

    @return         None
**/
//-----------------------------------------------------------------------------
void BcmMaesNotBypassARange(U32 rangeNum)
{
    U32 rangeBypassRead;
    U32 rangeBypassBit = (1 << rangeNum);

    BCM_check_status( BCM_apb_read (0, SHR_MIS_RNG_BYP, &rangeBypassRead));
    BCM_check_status( BCM_apb_write(0, SHR_MIS_RNG_BYP, (rangeBypassRead&(~rangeBypassBit))));
}

//-----------------------------------------------------------------------------
/**
    Name:           BcmMaesReadARegister

    Description:    Read a MAES register.

    @param[in]      regOffset   - offset of the register location. AES_ENG0_CFG
                                  is at the offset 0
    @param[in]      pv          - pointer to the value it reads.

    @return         None
**/
//-----------------------------------------------------------------------------
void BcmMaesReadARegister(U32 regOffset, U32 *pv)
{
    BCM_check_status( BCM_apb_read (0, regOffset, pv));
}

//-----------------------------------------------------------------------------
/**
    Name:           BcmMaesWriteARegister

    Description:    Write a MAES register.

    @param[in]      regOffset   - offset of the register location. AES_ENG0_CFG
                                  is at the offset 0
    @param[in]      value       - value to write

    @return         None
**/
//-----------------------------------------------------------------------------
void BcmMaesWriteARegister(U32 regOffset, U32 value)
{
    BCM_check_status( BCM_apb_write (0, regOffset, value));
}

//-----------------------------------------------------------------------------
/**
    Name:           BcmMaesLatchKeyRam2EncEngine

    Description:    Function to latch MAES key RAM params to Encryption Engine.
                    Note: Call this function to load the keyram parameters, such as,
                    ey, IV, and ranges to MAES Encryption engine after (if any of) these parameters
                    are updated in KeyRAM.

    @param[in]      None

    @return         None
**/
//-----------------------------------------------------------------------------
void BcmMaesLatchKeyRam2EncEngine(void)
{
    U32 regRead;
    U32 eng0RngReset = (1 << 8);

    BCM_check_status( BCM_apb_read (0, AES_ENG0_CMD, &regRead));
    BCM_check_status( BCM_apb_write(0, AES_ENG0_CMD, (regRead |eng0RngReset)));
    BCM_check_status( BCM_apb_write(0, AES_ENG0_CMD, (regRead&(~eng0RngReset))));
}

//-----------------------------------------------------------------------------
/**
    Name:           BcmMaesLatchKeyRam2DecEngine

    Description:    Function to latch MAES key RAM params to Decryption Engine.
                    Note: Call this function to load the keyram parameters, such as,
                    ey, IV, and ranges to MAES Decryption engine after any of these parameters
                    is updated in KeyRAM.

    @param[in]      None

    @return         None
**/
//-----------------------------------------------------------------------------
void BcmMaesLatchKeyRam2DecEngine(void)
{
    U32 regRead;
    U32 eng0RngReset = (1 << 8);

    BCM_check_status( BCM_apb_read (0, AES_ENG1_CMD, &regRead));
    BCM_check_status( BCM_apb_write(0, AES_ENG1_CMD, (regRead |eng0RngReset)));
    BCM_check_status( BCM_apb_write(0, AES_ENG1_CMD, (regRead&(~eng0RngReset))));
}

//-----------------------------------------------------------------------------
/**
    Name:           BcmMaesSetRange0KeyIV

    Description:    Function to write a key&IV to range0 to keyRAM, and latch them to to
                    Decryption Engine and Decryption engine.

    @param[in]      None

    @return         None
**/
//-----------------------------------------------------------------------------
void BcmMaesSetRange0KeyIV(void)
{
    U32 i;
    U32 rdata;
    U32 key1Base = 0x100;
    U32 key2Base = 0x200;
    U32 IVbase = 0x300;

    // Automatic pointer increment mode
    BCM_check_status( BCM_apb_read (0, SHR_CTRL, &rdata));
    rdata |= bit(0);
    BCM_check_status( BCM_apb_write(0, SHR_CTRL, rdata));

    // Select range 0 to set key1
    BCM_check_status( BCM_apb_write(0, SHR_KEY1_ACC_PTR, 0));
    // Program range 0 KEY
    for(i = 0; i < 8; i++)
    {
        BCM_check_status( BCM_apb_write(0, SHR_KEY1_ACC_DW, (key1Base+i)));
    }

    // Select range 0 to set key2
    BCM_check_status( BCM_apb_write(0, SHR_KEY2_ACC_PTR, 0));
    // Program range 0 XTS KEY
    for(i = 0; i < 8; i++)
    {
        BCM_check_status( BCM_apb_write(0, SHR_KEY2_ACC_DW, (key2Base+i)));
    }

    // Select range 0 to set IV
    BCM_check_status( BCM_apb_write(0, SHR_IV_ACC_PTR, 0));
    // Program range 0 IV
    for(i = 0; i < 4; i++)
    {
        BCM_check_status( BCM_apb_write(0, SHR_IV_ACC_DW, (IVbase+i)));
    }

    BcmMaesLatchKeyRam2EncEngine();
    BcmMaesLatchKeyRam2DecEngine();
}
#endif
