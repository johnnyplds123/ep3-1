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
//! @Basic utilities for accessing BCM & tools
//!
//=============================================================================
//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"
#include "bcmClientSection.h"
#include "BcmClientPrintf.h"
//#include "BcmClientProcessor.h"
#include "BcmClientTypedef.h"       // Contains BCM internal data type & Register Offset
#include "BcmClientUtilities.h"

#include "SignedErom.inc" // Contains signed basic   EROM image in 32-bit Hex format
#include "BcmClientFunctions.h" // Contains signed basic   EROM image in 32-bit Hex format
#include "BcmClientVars.h"

// LEO #include <stdarg.h>


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
//  Imported data declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function proto-type without header include
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Global data
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------
#if _CPU_ == CPU_ID0
#define RSA_DEBUG   FALSE
#if RSA_DEBUG
#define RSA_KEY_SIZE    1024

__align(16)  U32 RsaSign[RSA_KEY_SIZE/32];
__align(16)  U32 RsaModulus[RSA_KEY_SIZE/32];
__align(16)  U32 RsaPubExp[RSA_KEY_SIZE/32];
#endif


//-----------------------------------------------------------------------------
/**
    Name:   STATUS BCM_wait_for_boot

    Description:    Wait for BCM ready from Boot
                    Notes: BCM should be ready around 450,000 ns with fast-boot

    @param[in]      None
    @param[out]     None

    @return         status code
**/
//-----------------------------------------------------------------------------
STATUS BCM_wait_for_boot (void)
{
    STATUS status = STATUS_SUCCESS;
    volatile MAILBOX *pMBOX = (MAILBOX *) BCM_MBOX_BASE_ADDR;
    U32 waitCnt = 0x1000;

    printf_tube(": BCM Boot-Up: [C4] = %X\n", pMBOX->fifo_status);

    while(((pMBOX->fifo_status) & 0x100) == 0)
    {
        if ((pMBOX->host_intp_reg) != 0 )
        {
            if ((pMBOX->return_status)!= 0)
            {
                status = (STATUS)(pMBOX->return_status);
            }
            else
            {
                status = (STATUS)STATUS_UNEXPECTED_BOOT_ERROR;
            }
            break;
        }
        else
        {
            --waitCnt;
            if (waitCnt == 0)
            {
                break;  // don't wait inside this loop forever, in case that someone else cleared the interrupt bit before we check it.
            }
        }
    }
    // Eldora BCM will set interrupt after boot up
    waitCnt = 0x1000;
    while (--waitCnt)
    {
        if (pMBOX->host_intp_reg & 0x1)
        break;
    }

    if (waitCnt==0)
    {
        //printf_tube(": Cann't detect interrupt after the boot up !!\n");
        printf_tube(": [C4] = %X, [C8] = %X\n", pMBOX->fifo_status, pMBOX->host_intp_reg);
    }
    pMBOX -> host_intp_reg = 0xffffffff; // Clear Intp
    if (pMBOX->host_intp_reg & 0x1)
    {
        // The interrupt status bit still sets!
        ASSERT(0);  //cj, while(1);
    }

    return (status);
}
#endif  //End of #if _CPU_ == CPU_ID0

//------------------------------------------------------- Wait for complete
// Keep checking the interrupt. If the command can't be
// completed within "waitCnt", time out error is returned.

#ifndef IPC_BCM_STATUS_RETURN_DEBUG
    #define IPC_BCM_STATUS_RETURN_DEBUG
#endif


STATUS BCM_wait_for_complete (U32 waitCnt)
{
    volatile MAILBOX *pMBOX = (MAILBOX *) BCM_MBOX_BASE_ADDR;

    while (waitCnt)
    {
        if (pMBOX->host_intp_reg & 0x1)
            break;
        else
            waitCnt--;
    }

    if (waitCnt==0)
    {
        return ((STATUS)STATUS_COMMAND_TIMEOUT);
    }
    else
    {
      #if 0     // It was this
        pMBOX -> host_intp_reg = 0xffffffff; // Clear Intp
      #else     // New, suggested
        #if _CPU_==CPU_ID0
        bcmClientVars.stReads = 0;
        #endif
        pMBOX -> host_intp_reg = 0xffffffff; // Clear Intp
        while(1)
        {
        #if _CPU_==CPU_ID0
            bcmClientVars.stReads++;
        #endif
            if (pMBOX->host_intp_reg & 0x1)
            {
                continue;
            }
            else
            {
                break;
            }
        }
        #if _CPU_==CPU_ID0
        // Keep the max
        if (bcmClientVars.stReads > bcmClientVars.stReadsMax)
        {
            bcmClientVars.stReadsMax = bcmClientVars.stReads;
        }
        #endif
      #endif
        return ((STATUS)pMBOX->return_status);
    }
}


//------------------------------------------------------- Check return status
// Check BCM return status
void BCM_check_status (STATUS status)
{
    //volatile MAILBOX *pMBOX = (MAILBOX *) BCM_MBOX_BASE_ADDR;
    #if _CPU_==CPU_ID0
    bcmClientVars.ipcCmdTimeout = 0;
    #endif
    if (status != STATUS_SUCCESS)
    {
        write_tube(": !!!BCM NG = "); write_tube_dword(status);
        // Read Panic Code (More information about the fail status)

        /* write_tube(": BCM Panic Code @ Status Register 0  = "); write_tube_dword(pMBOX->status0);
        write_tube(": BCM Panic Code @ Status Register 1  = "); write_tube_dword(pMBOX->status1);
        write_tube(": BCM Panic Code @ Status Register 2  = "); write_tube_dword(pMBOX->status2);
        write_tube(": BCM Panic Code @ Status Register 3  = "); write_tube_dword(pMBOX->status3);
        write_tube(": BCM Panic Code @ Status Register 4  = "); write_tube_dword(pMBOX->status4);
        write_tube(": BCM Panic Code @ Status Register 5  = "); write_tube_dword(pMBOX->status5);
        write_tube(": BCM Panic Code @ Status Register 6  = "); write_tube_dword(pMBOX->status6);
        write_tube(": BCM Panic Code @ Status Register 7  = "); write_tube_dword(pMBOX->status7);
        write_tube(": BCM Panic Code @ Status Register 8  = "); write_tube_dword(pMBOX->status8);
        write_tube(": BCM Panic Code @ Status Register 9  = "); write_tube_dword(pMBOX->status9);
        write_tube(": BCM Panic Code @ Status Register 10 = "); write_tube_dword(pMBOX->status10);
        write_tube(": BCM Panic Code @ Status Register 11 = "); write_tube_dword(pMBOX->status11);
        write_tube(": BCM Panic Code @ Status Register 12 = "); write_tube_dword(pMBOX->status12);
        write_tube(": BCM Panic Code @ Status Register 13 = "); write_tube_dword(pMBOX->status13);
        write_tube(": BCM Panic Code @ Status Register 14 = "); write_tube_dword(pMBOX->status14);
        write_tube(": BCM Panic Code @ Status Register 15 = "); write_tube_dword(pMBOX->status15); */
        #if _CPU_==CPU_ID0
        bcmClientVars.rd_err_cnt ++;
        bcmClientVars.ipcCmdTimeout = 0;

        if (status == STATUS_COMMAND_TIMEOUT)
            bcmClientVars.ipcCmdTimeout = 1;

        bcmClientVars.stSave = status;
        #endif
        // Just simply hang here for NOW
        ASSERT(0);  //cj, while(1);

    // TEST_END;
  }
  //printf_tube(": BCM OK\n");
}

#if _CPU_ == CPU_ID0
//------------------------------------------------------- Obtain BCM Version
// Inputs are the destination addresses of core and
// platform version of the irom in AXI address space
// Enough space must be allocated.
//
// Prevent of changing the argument (can't use const cuz it is @ left hand side)
//U8 coreVersion[20] = { 0 };
//U8 platformVersion[20] = { 0 };
STATUS BCM_irom_version (U32 *buf)
{
    STATUS status;
    volatile MAILBOX *pMBOX = (MAILBOX *) BCM_MBOX_BASE_ADDR;
    U32 *coreVersion        = (U32 *)buf;           //(U32 *)AXI_MEM_BASE_ADDR;
    U32 *platformVersion    = (U32 *)(buf+0x100);   //(U32 *)(AXI_MEM_BASE_ADDR+0x100);
    U32 i;

    printf_tube(": BCM IROM ver: ");
    status = Host_IPC_Cmd_2BCM (BCM_GET_VERSION_INFO, 0x10000, 2, coreVersion, platformVersion);

    simplePrintf("%x %x %x %x %x\n", pMBOX->status0, pMBOX->status1, pMBOX->status2, pMBOX->status3, pMBOX->return_status);

    //for(i=0; i<pMBOX->status0*4; i++)
    //    simplePrintf("%c",*((U8*)coreVersion+i));
    //simplePrintf("\n");

    //for(i=0; i<pMBOX->status2*4; i++)
    //    simplePrintf("%c",*((U8*)platformVersion+i));
    //simplePrintf("\n");

    return(status);

}

void Chk_BCM_irom_version(U32 *buf)
{
    BCM_check_status(BCM_irom_version(buf));
}

#ifdef BCM_EROM_SUPPORT
//------------------------------------------------------- Obtain BCM Version
// This function must be called after basic erom
// is loaded

//cj: "coreVersion" must be located at DDR area for DMA xfer.
//ex: U8 coreVersion[64] ;
//      simplePrintf("%c%c%c%c\n", coreVersion[0],coreVersion[1],coreVersion[2],coreVersion[3]);
STATUS BCM_erom_version (U32 *buf)
{
    STATUS status;
    volatile MAILBOX *pMBOX = (MAILBOX *) BCM_MBOX_BASE_ADDR;
    U32 *coreVersion = (U32 *)buf;  //(U32 *)AXI_MEM_BASE_ADDR;

    printf_tube(": BCM EROM Ver: ");
    status = Host_IPC_Cmd_2BCM (SOC_GET_VERSION_INFO, 0x10000, 1, coreVersion);
    simplePrintf("%x %x %x\n", pMBOX->status0, pMBOX->status1, pMBOX->return_status);
    //simplePrintf("%c%c%c%c\n", coreVersion[0],coreVersion[1],coreVersion[2],coreVersion[3]);

    return(status);
}

void Chk_BCM_erom_version(U32 *buf)
{
    BCM_check_status(BCM_erom_version(buf));
}
#endif

//---------------------------------------------------------------------------------------+
//                                                                                 Tools |
//---------------------------------------------------------------------------------------+

//------------------------------------------------------- Load Pattern
// Load array data to system memory
//
// mode:    0=Copy
//          1=Copy and change byte endian format
//          2=Copy and change hex word to integer (Reverse Load)
// numWord: Total number of word for copy
// pIn:     Test vector array (stored in a header file)
// pOut:    Destination starting address (AXI memory)
//
// Notes: pOut would be modify when return the function call
void Load_Pattern( U32 mode, U32 numWords, const U32 *pIn, U32 *pOut)
{
    U32 i, byte3, byte2, byte1, byte0;

    // Copy and change hex word to integer
    //------------------------------------
    // octet word stream to integer:  used for RSA modulus, exponent, & signature
    if( mode == 2)
    {
      for(i=0; i<numWords; i++)
            pOut[numWords-i-1] = pIn[i];
    }

    // Copy and change byte endian format
    //------------------------------------
    // little to big endian: Used for RSA message
    else if (mode == 1)
    {
        for(i=0; i<numWords; i++)
        {
            byte0 = (pIn[i]>>24) & 0xff;
            byte1 = (pIn[i]>>16) & 0xff;
            byte2 = (pIn[i]>> 8) & 0xff;
            byte3 = (pIn[i]>> 0) & 0xff;
            pOut[i] = (byte3<<24) | (byte2<<16) | (byte1<<8) | byte0;
        }
    }
    // Copy without any change
    //------------------------------------
    else
    {
        for(i=0; i<numWords; i++)
        pOut[i] = pIn[i];
    }
}

//------------------------------------------------------- Compare Data
// Compare two 32-bit data
//
// endian = 0:Compare without change endian
//          1:Change d0's endian
void Comp_Pattern(U32 endian, U32 d0, U32 d1)
{
  U32 data;

  if (endian==0) data = d0;
  else           data = (d0>>24) | (d0<<24) | ((d0 & 0xff0000)>>8) | ((d0 & 0xff00)<<8);

  if (data!=d1)
    {
      //write_tube(": Data Compare failed - Mismatch\n");
      //write_tube_dword(data);
      //write_tube_dword(d1);

      printf_tube(": Data Compare failed - Mismatch d0=%x, d1=%x\n", data, d1);
      bcmClientVars.rd_err_cnt ++;
      ASSERT(0);    //while(1);
    }
  printf_tube(": Data compare succeed \n");
}


//---------------------------------------------------------------------------------------+
//                                                                 EROM Related Function |
//---------------------------------------------------------------------------------------+

//------------------------------------------------------- Image Authorize
// This function calls the authorize PI
//
// ptr_img     : Source address of the image
// ptr_mod     : Source address of moduli
// ptr_exp     : Source address of exponent
// ptr_sig     : Source address of signature
// ptr_tok     : Source address of token (two-factor). Set to 0 when not use
// imgSizeByte : Image size in Byte
// keyLenBit   : Operation field size in bit
// tokLenBit   : Token length in bit
// hashMode    : 0=sha1, 1=sha224, 2=sha256, 3=sha384, 4=sha512
// sel         : 0=BCM_FW_AUTHORIZE 1=BCM_SOC_TWO_FACTOR_FW_AUTHORIZE

STATUS BCM_image_authorize (U32 *ptr_img, U32 *ptr_mod, U32 *ptr_exp, U32 *ptr_sig, U32 *ptr_tok,
                U32 imgSizeByte, U32 keyLenBit, U32 tokLenBit, U32 hashMode, U32 sel)
{
    STATUS status;

    if (sel==0)
    {
        //====== One Factor
        printf_tube(": Image Authorize Command - One Factor\n");
                                        //cmd, numparams, key Length, moduli location, exp location, image size, image location, signature type, signature Length, signature location
        status = Host_IPC_Cmd_2BCM (BCM_FW_AUTHORIZE, 0x100000, 8, keyLenBit, (U32)ptr_mod, (U32)ptr_exp, imgSizeByte, (U32)ptr_img, hashMode, (keyLenBit/8), (U32)ptr_sig);
    }
    else
    {
        //====== Two Factor
        printf_tube(": Image Authorize Command - Two Factor\n");
                                        //cmd, numparams, key Length, moduli location, exp location, image size, image location, signature type, signature Length, signature location, token location, token size (Notes: it is not used since size is fixed to 32-bit in platform.c)
        status = Host_IPC_Cmd_2BCM (BCM_FW_AUTHORIZE, 0x100000, 10, keyLenBit, (U32)ptr_mod, (U32)ptr_exp, imgSizeByte, (U32)ptr_img, hashMode, (keyLenBit/8), (U32)ptr_sig, (U32)ptr_tok, tokLenBit);
    }

    return(status);
}


//------------------------------------------------------- Load Blank EROM
// This function will load basic EROM to BCM,
//
// bufAddr: A buffer location to holds the EROM image and
//          pub-key as well as signature.
// sel:     0=BCM_FW_AUTHORIZE 1=BCM_SOC_TWO_FACTOR_FW_AUTHORIZE
//
// Notes:
// 1. "SignedErom.inc" must be created by SHA1 1024-bit
// 2. If two-factor, token must be loaded to OTP (opt_init.dat)
// 3. The array in SignedErom.inc places data's MSW
//    at array[0]

STATUS BcmDownLoadErom (U32 *bufAddr, U32 sel)
{
    U32 i;
    U32 *ptr_erom, *ptr_mod, *ptr_exp, *ptr_sig, *ptr_tok;
    U32 *pMEM = bufAddr;

    printf_tube(": Load BCM EROM\n");

     // Allocate the System Momory
    i = 0;
    ptr_tok  = &pMEM[i]; i+=64;
    ptr_mod  = &pMEM[i]; i+=64;
    ptr_exp  = &pMEM[i]; i+=64;
    ptr_sig  = &pMEM[i]; i+=64;
    ptr_erom = &pMEM[i];

    // Load EROM Array to system memory
    Load_Pattern( 1, (kBASIC_EROM_SIZE/4), BASIC_EROM,                 ptr_erom ); // little to big endian
    Load_Pattern( 2, (1024/32),            BASIC_EROM_SIGNATURE,       ptr_sig  ); // octet to integer
    Load_Pattern( 2, (1024/32),            BASIC_EROM_MODULUS,         ptr_mod  ); // octet to integer
    Load_Pattern( 2, (1024/32),            BASIC_EROM_PUBLIC_EXPONENT, ptr_exp  ); // octet to integer
    ptr_tok[0] = 0x12345678;

    // Notes:
    //  1. Token size is not used since size is fixed to 32-bit in platform.c
    //  2. A 32-bit Token is used in two factor case (See otp_init.dat)
    //     - Deployment 1 authorization token = 0x12345678
    //     - Deployment 1 authorization token = 0x87654321
    //     - Deployment 1 authorization token = 0xabcdef01

    return (BCM_image_authorize (ptr_erom, ptr_mod, ptr_exp, ptr_sig, ptr_tok, kBASIC_EROM_SIZE, 1024, 32, 0, sel));
}


// This function verifies the EROM image with RSA PKCS V1.5 only.
void BCM_RSA_verify_erom_image(U32 *MEM)
{
  STATUS status;
  U32 i;
  //U32 *MEM = (U32 *) AXI_MEM_BASE_ADDR;

  // Pointers to test vector in user's memory
  U32 *ptr_msg, *ptr_mod, *ptr_exp, *ptr_sig;

  // Config
  //-------------------------------------------------------
  //printf_tube(": PKCS1 TEST\n");
  i=0;
  ptr_mod = &MEM[i]; i+=256/4;  // Point to 256 byte mem location
  ptr_exp = &MEM[i]; i+=256/4;  // Point to 256 byte mem location
  ptr_sig = &MEM[i]; i+=256/4;  // Point to 256 byte mem location
  ptr_msg = &MEM[i];            // Point to the image location

  // Verify: 1024 bit Key, SHA1, Signature
  //-------------------------------------------------------
    Load_Pattern( 1, (kBASIC_EROM_SIZE/4), BASIC_EROM,                 ptr_msg ); // little to big endian
    Load_Pattern( 2, (1024/32),            BASIC_EROM_SIGNATURE,       ptr_sig  ); // octet to integer
    Load_Pattern( 2, (1024/32),            BASIC_EROM_MODULUS,         ptr_mod  ); // octet to integer
    Load_Pattern( 2, (1024/32),            BASIC_EROM_PUBLIC_EXPONENT, ptr_exp  ); // octet to integer
#if RSA_DEBUG
D_PRINTF("ptr_sig:\n");
for(i=0; i<32; ) {
    D_PRINTF("%X ", ptr_sig[i]);

    ++i;
    if((i%4)==0)
        D_PRINTF("\n");
}
#endif

  status = BCM_pkcs_v15_verify (ptr_msg, ptr_mod, ptr_exp, ptr_sig, 0, kBASIC_EROM_SIZE, 1024);

  BCM_check_status(status);

  return;
}

#if RSA_DEBUG
// This function verifies the firmware (Boot+Down+FW) image with RSA PKCS V1.5 only.
// U32 *Fw: FW start address
// U32 fwSize: FW size (bytes), RSA signature is not included.
Error_t BCM_RSA_verify_fw_image(U32 *pFw, U32 fwSize)
{
    STATUS status;
    U32 i;
    U32 *pSign = pFw + fwSize/4 - RSA_KEY_SIZE/32;
    U32 *pTag = pSign-0X04;

    // Config
    //-------------------------------------------------------
    printf_tube(": FW PKCS1 CHECK %X %X %X\n", pFw, fwSize, pSign);
    D_PRINTF("%X, %X, %X, %X\n",pTag[0], pTag[1], pTag[2], pTag[3]);

    // Verify: 1024 bit Key, SHA1, Signature
    //-------------------------------------------------------

    //Load_Pattern( 1, ( pTag[2]/4), pFw,   pFw   ); // little to big endian

    Load_Pattern( 2, (RSA_KEY_SIZE/32), pSign, RsaSign ); // octet to integer
  D_PRINTF("pSign:\n");
  for(i=0; i<32; ) {
      D_PRINTF("%X ", pSign[i]);

      ++i;
      if((i%4)==0)
          D_PRINTF("\n");
  }
  D_PRINTF("RsaSign:\n");
  for(i=0; i<32; ) {
      D_PRINTF("%X ", RsaSign[i]);

      ++i;
      if((i%4)==0)
          D_PRINTF("\n");
  }

    Load_Pattern( 2, (RSA_KEY_SIZE/32),  BASIC_EROM_MODULUS,         RsaModulus  ); // octet to integer
    Load_Pattern( 2, (RSA_KEY_SIZE/32),  BASIC_EROM_PUBLIC_EXPONENT, RsaPubExp  ); // octet to integer

    status = BCM_pkcs_v15_verify (pFw, RsaModulus, RsaPubExp, RsaSign, 0, /* 0x4000 */ pTag[2], RSA_KEY_SIZE);
  //  BCM_check_status(status);
D_PRINTF("pkcs: %X\n",status);
    if (status != STATUS_SUCCESS)
        return cEcError;
    else
        return cEcNoError;
}
#endif

#ifndef _BASIC_EROM_H_

//------------------------------------------------------- Load Reqular EROM
// This function will load EROM to BCM,
//
// sel: 0=BCM_FW_AUTHORIZE 1=BCM_SOC_TWO_FACTOR_FW_AUTHORIZE
//
// Notes:
// 1. "signed_bcm_erom.h" must be created by SHA256 1024-bit
// 2. If two-factor, token must be loaded to OTP (opt_init.dat)
// 3. The array in signedErom.h places data's MSW
//    at array[0]

STATUS BCM_load_erom (U32 sel, U32 *pMEM)
{
    U32 i;
    U32 *ptr_erom, *ptr_mod, *ptr_exp, *ptr_sig, *ptr_tok;
    //U32 *pMEM = (U32 *) AXI_MEM_BASE_ADDR;

    printf_tube(": Load BCM Regular EROM\n");

    // Allocate the System Momory
    i = 0;
    ptr_tok  = &pMEM[i]; i+=64;
    ptr_mod  = &pMEM[i]; i+=64;
    ptr_exp  = &pMEM[i]; i+=64;
    ptr_sig  = &pMEM[i]; i+=64;
    ptr_erom = &pMEM[i];

    // Load EROM Array to system memory
    Load_Pattern( 1, (kEROM_SIZE/4), EROM,                 ptr_erom ); // little to big endian
    Load_Pattern( 2, (1024/32),      EROM_SIGNATURE,       ptr_sig  ); // octet to integer
    Load_Pattern( 2, (1024/32),      EROM_MODULUS,         ptr_mod  ); // octet to integer
    Load_Pattern( 2, (1024/32),      EROM_PUBLIC_EXPONENT, ptr_exp  ); // octet to integer
    ptr_tok[0] = 0x12345678;

    // Notes:
    //  1. Token size is not used since size is fixed to 32-bit in platform.c
    //  2. A 32-bit Token is used in two factor case (See otp_init.dat)
    //     - Deployment 1 authorization token = 0x12345678
    //     - Deployment 1 authorization token = 0x87654321
    //     - Deployment 1 authorization token = 0xabcdef01

    return (BCM_image_authorize (ptr_erom, ptr_mod, ptr_exp, ptr_sig, ptr_tok, kEROM_SIZE, 1024, 32, 2, sel));
}

//-------------------------------------------------------
//------------------------------------------------------- Load rsa EROM
// This function will load rsa_EROM to BCM,
//
// sel: 0=BCM_FW_AUTHORIZE 1=BCM_SOC_TWO_FACTOR_FW_AUTHORIZE
//
// Notes:
// 1. "signed_rsa_signing_erom.h" must be created by SHA1 1024-bit
// 2. If two-factor, token must be loaded to OTP (opt_init.dat)
// 3. The array in signed_rsa_signing_erom.h places data's MSW
//    at array[0]

STATUS BCM_load_rsa_erom (U32 sel, U32* pMEM)
{
    U32 i;
    U32 *ptr_erom, *ptr_mod, *ptr_exp, *ptr_sig, *ptr_tok;
    //U32 *pMEM = (U32 *) AXI_MEM_BASE_ADDR;

    printf_tube(": Load BCM rsa signing EROM\n");

    // Allocate the System Momory
    i = 0;
    ptr_tok  = &pMEM[i]; i+=64;
    ptr_mod  = &pMEM[i]; i+=64;
    ptr_exp  = &pMEM[i]; i+=64;
    ptr_sig  = &pMEM[i]; i+=64;
    ptr_erom = &pMEM[i];

    // Load EROM Array to system memory
    Load_Pattern( 1, (kRSA_SIGNING_EROM_SIZE/4), RSA_SIGNING_EROM,                 ptr_erom ); // little to big endian
    Load_Pattern( 2, (1024/32),      RSA_SIGNING_EROM_SIGNATURE,       ptr_sig  ); // octet to integer
    Load_Pattern( 2, (1024/32),      RSA_SIGNING_EROM_MODULUS,         ptr_mod  ); // octet to integer
    Load_Pattern( 2, (1024/32),      RSA_SIGNING_EROM_PUBLIC_EXPONENT, ptr_exp  ); // octet to integer
    ptr_tok[0] = 0x12345678;

    // Notes:
    //  1. Token size is not used since size is fixed to 32-bit in platform.c
    //  2. A 32-bit Token is used in two factor case (See otp_init.dat)
    //     - Deployment 1 authorization token = 0x12345678
    //     - Deployment 1 authorization token = 0x87654321
    //     - Deployment 1 authorization token = 0xabcdef01

    return (BCM_image_authorize (ptr_erom, ptr_mod, ptr_exp, ptr_sig, ptr_tok, kRSA_SIGNING_EROM_SIZE, 1024, 32, 0, sel));
}
#endif // End of #ifndef _BASIC_EROM_H_
#endif //End of #if _CPU_ == CPU_ID0


//------------------------------------------------------- APB Write
// Can only be used after APB EROM is loaded to BCM
//
// port:  0=Midea_AES, 1=OTP
// waddr: Write apb address (byte address)
// wdata: Write data
//
// Notes: APB write data is provide within the Mailbox
//        AXI interface is not used
STATUS BCM_apb_write (U32 port, U32 waddr, U32 wdata)
{
    STATUS status;

    status = Host_IPC_Cmd_2BCM(EROM_APB_WRITE, 0x10000, 3, port, waddr, wdata);
    return (status);
}

//------------------------------------------------------- APB Read
// Can only be used after APB EROM is loaded to BCM
//
// port:  0=Midea_AES, 1=OTP
// raddr: Read apb address (byte address)
// rdata: Read data destination address
//
// Notes: APB read data value will be in Mailbox status 0
//        AXI interface is not used
STATUS BCM_apb_read (U32 port, U32 raddr, U32 *rdata)
{
    STATUS status;
    volatile MAILBOX *pMBOX = (MAILBOX *) BCM_MBOX_BASE_ADDR;

    // status = BCM_image_authorize(EROM_APB_READ, 3, port, waddr, wdata);
    status = Host_IPC_Cmd_2BCM(EROM_APB_READ, 0x10000, 2, port, raddr);
    *rdata = pMBOX->status0; // Read data is in Mailbox's status0
  //simplePrintf("%s: %04X\n",__FUNCTION__, status);
    return (status);
}



U32 If_This_Cmd_Need_Delay(U32 cmd)
{
    U32 needDelay = 0;

  #if 1 // LEO After integrate into SDK, set it to no-delay to test it.
    switch(cmd)
    {
    #ifdef BCM_EROM_SUPPORT
        case EROM_APB_WRITE:
        case EROM_APB_READ:
        case EROM_SELECT_ENTROPY_PP:

        case EROM_OTP_READ_CALL_L2_IROM_PRIM:
		case EROM_OTP_READ_VDD_SETPOINT_IROM_PRIM:
        case EROM_AES_KEY_WRAP:         // BCM_aes_key_wrap
    #endif
        case BCM_AES_KEY_GEN:          // BCM_aes_key_gen
        case BCM_HMAC_KEY_GEN:
        case BCM_DRBG_GEN_RAN_BITS:
        needDelay = 2;
        break;

        // THe followitng primitives don't need delay.


    #ifdef BCM_EROM_SUPPORT
        case ELDORA_EROM_UNLOAD:
        case EROM_READ_MAES_KEY:
        case EROM_READ_MAES_XTS_KEY:
        case EROM_READ_MAES_IV:
        case EROM_BATCH_KEYRAM_ZERORIZE:
        case SOC_GET_VERSION_INFO:
    #endif
        case BCM_GET_VERSION_INFO:
        case BCM_FW_AUTHORIZE:
        case BCM_AES_LOAD_IV:           // BCM_aes_engine
        case BCM_AES_LOAD_KEY:          // BCM_aes_engine
        case BCM_AES_INIT:              // BCM_aes_engine
        case BCM_AES_PROCESS:           // BCM_aes_engine
        case BCM_AESX_LOAD_KEY:         // BCM_aesx_engine
        case BCM_AESX_LOAD_IV:          // BCM_aesx_engine
        case BCM_AESX_INIT:             // BCM_aesx_engine
        case BCM_AESX_PROCESS:          // BCM_aesx_engine
        case BCM_HASH_INIT:
        case BCM_HASH_UPDATE:
        case BCM_HASH_FINAL:
        case BCM_HASH_ZERORIZE:
        case BCM_HMAC_LOAD_KEY:
        case BCM_HMAC_INIT:
        case BCM_HMAC_UPDATE:
        case BCM_HMAC_FINAL:
        case BCM_HMAC_ZERORIZE:
        case BCM_RSASSA_PKCS1_V15_VERIFY:

        case BCM_RESET:
        case BCM_SELF_TEST:
        needDelay = 1;
        break;

        default:    // Others, no delay needed.
            // needDelay = 0;
            ASSERT(0);  //cj, while(1);   // catch it here.

            // break;
    }
  #endif    // End of #if 0 // LEO After integrate into SDK, set it to no-delay to test it.

    return(needDelay);
}



//-----------------------------------------------------------------------------
/**
    Name:   STATUS  Host_IPC_Cmd_2BCM

    Description:    Load params to IPC and issue a command to send the params
                    to BCM, then wait to the command complete.
                    This is a function with a veriable number of params.

    @param[in]      U32  cmd         -  an enum value for an IROM/EROM function
    @param[in]      U32  waits       -  number of loops to wait while checking command done status from BCM.
    @param[in]      U32* pNumParams  -  It counts params0, params1,...only, if they
                                     exist, and it does not include the cmd.
                                     The Min numParams is 0,
                                     the Max nmuParam is 12 which is limited by
                                     BCM architecture.

    @param[in]      U32 param0, U32 param1, ... - The variable-length arguments list.
    @param[out]     None

    @return         status code
**/
//-----------------------------------------------------------------------------
#define MAX_IPC_PARAMS          (16)    // total param entries of IPC.

STATUS Host_IPC_Cmd_2BCM (U32 cmd, U32 waits, U32 numParams, ...)
{
    STATUS status;
    va_list ap;
    int ii;
    MAILBOX *pMBOX = (MAILBOX *) BCM_MBOX_BASE_ADDR;
    U32 *pv = &pMBOX->arg0;
    volatile U32 tmp;   // For debug


    if (numParams > MAX_IPC_PARAMS)
    {
        status = STATUS_FUNCTION_OUT_OF_RANGE;
    }
    else
    {

        va_start(ap, numParams);
        for(ii = 0; ii < numParams; ii++)
        {
            // *(pv+ii) = va_arg(ap, int);
            tmp = va_arg(ap, int);
            // printf(tmp);
            *(pv+ii) = tmp;
        }
        // Fill up other param slots
        for(ii = numParams; ii < MAX_IPC_PARAMS; ii++)
        {
            *(pv+ii) = 0x0;     // Note: The arg14 and arg15 must be either 0 or pointers of host link-list.
        }

        // According to IROM team, it would be better to clear the interrupt status one more time
        // before issuing the command.
        pMBOX -> host_intp_reg = 0xffffffff; // Clear Intp

        // Adding this delay can make the BCM_Test(), which calls all the primitives in the code,
        // running for a long time (many loops) without failure. When this delay is removed, it
        // gets a timeout error after running awhile, maybe after a few loops, maybe after many
        // loops. We still investigate the cause.
        switch (If_This_Cmd_Need_Delay(cmd))
        {
            case 1:
                tmp = 100;
                while(--tmp);
                break;
            case 2:
                tmp = 200;
                while(--tmp);
                break;
            default:
                break;
        }

        pMBOX -> cmd  = cmd;
        status = BCM_wait_for_complete (waits);
        va_end(ap);
    }

    return (status);
}


#if _CPU_ == CPU_ID0

//-----------------------------------------------------------------------------
/**
    Name:                   memClr

    Description:            Set memory to 0.

    @param[in]  bytes       Number bytes to clear.
    @param[in]  ptr         location

    @return                 None
**/
//-----------------------------------------------------------------------------
void memClr(U32 bytes, void *ptr)
{
    U32 ii;
    U8 *lptr = (U8 *)ptr;

    for (ii = bytes; ii > 0; ii--)
    {
        *lptr++ = 0;
    }
    return;
}

//-----------------------------------------------------------------------------
/**
    Name:                   memCpy

    Description:            Copy memory.

    @param[in]  pDes        from location
    @param[in]  pSrc        to location
    @param[in]  words       Number of words to copy.

    @return                 None
**/
//-----------------------------------------------------------------------------
void memCpy(U32 *pDes, U32 *pSrc, U32 words)
{
    U32 ii;

    for (ii = words; ii > 0; ii--)
    {
        *pDes++ = *pSrc++;
    }
    return;
}


//-----------------------------------------------------------------------------
/**
    Name:                   memSet

    Description:            Set memory to a pattern value. Can be used to replace memClr().

    @param[in]  ptr         location
    @param[in]  pattern     a byte of pattern value to set
    @param[in]  bytes       Number bytes to set.

    @return                 None
**/
//-----------------------------------------------------------------------------
void memSet(void *ptr, U8 pattern, U32 bytes)
{
    U32 ii;
    U8 *lptr = (U8 *)ptr;

    for (ii = bytes; ii > 0; ii--)
    {
        *lptr++ = pattern;
    }
    return;
}

#ifdef BCM_EROM_SUPPORT
//-----------------------------------------------------------------------------
/**
    Name:                   HostCall_bcm_erom_otp_read

    Description:            Call a EROM primitive to read OTP and return the content
                            to the address specified. The address should be in AXI
                            memory space, ie, either TCM or DDR area.

    @param[in]  U32 otpOp   An OTP type enum
    @param[in]  addr        location to place the OTP content

    @return                 status
**/
//-----------------------------------------------------------------------------
STATUS HostCall_bcm_erom_otp_read(U32 otpOp, U32 addr)
{
    STATUS status;

    status = Host_IPC_Cmd_2BCM (EROM_OTP_READ_CALL_L2_IROM_PRIM, 0x100000, 2, otpOp, addr);

    return (status);
}

//-----------------------------------------------------------------------------
/**
    Name:                   HostCall_bcm_unload_EROM

    Description:            Unload current EROM from BCM.

    @param[in]              None.

    @return                 status
**/
//-----------------------------------------------------------------------------
STATUS HostCall_bcm_unload_EROM(void)
{
    STATUS status;

    status = Host_IPC_Cmd_2BCM (ELDORA_EROM_UNLOAD, 0x100000, 0);

    return (status);
}

//-----------------------------------------------------------------------------
/**
    Name:                   BCM_otp_read_vdd_setpoint

    Description:            Call a EROM primitive to read VDD setpoint

    @param[in]  pSetPoint   Pointer to VDD setpoint

    @return                 status
**/
//-----------------------------------------------------------------------------
STATUS BCM_otp_read_vdd_setpoint(U32* pSetPoint)
{
    STATUS status;
    volatile MAILBOX *pMBOX = (MAILBOX *)BCM_MBOX_BASE_ADDR;

    status = Host_IPC_Cmd_2BCM (EROM_OTP_READ_VDD_SETPOINT_IROM_PRIM, 0x100000, 0);

    *pSetPoint = pMBOX->status0;

    return status;
}
#endif
#endif //End of #if _CPU_ == CPU_ID0
