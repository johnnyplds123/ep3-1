#pragma once
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
//-----------------------------------------------------------------------------
// Title         : BCM Utilities
//-----------------------------------------------------------------------------
// File          : BcmClientUtilities.h
//-----------------------------------------------------------------------------
// Description :
//   - Include all basic utilities for accessing BCM & tools
//-----------------------------------------------------------------------------
// Copyright (c) 2014 by Marvell Semiconductor This model is the confidential and
// proprietary property of Marvell Semiconductor and the possession or use of this
// file requires a written license from Marvell Semiconductor.
//------------------------------------------------------------------------------
// Modification history :
//
//-----------------------------------------------------------------------------

#include "BcmClientPrintf.h"
#include "BcmClientTypedef.h"       // Contains BCM internal data type & Register Offset
#include "BcmClientVars.h"


//---------------------------------------------------------------------------------------+
//                                                                         Configuration |
//---------------------------------------------------------------------------------------+
// Base address
#define BCM_MBOX_BASE_ADDR    0x38000000  // BCM Mailbox
//#define AXI_MEM_BASE_ADDR     0x48000000    //cj, 0x8800000   // AXI Data Memory
#define DDR_MEM_BASE_ADDR     0x40000000  // DDR Memory (SDRAM_INIT is needed in the test)



//---------------------------------------------------------------------------------------+
//                                                                   BCM Mailbox Control |
//---------------------------------------------------------------------------------------+

//------------------------------------------------------- Wait for boot-up
// Wait for BCM ready from Boot
//
// Notes: BCM should be ready around 450,000 ns with fast-boot

STATUS BCM_wait_for_boot (void);


//------------------------------------------------------- Wait for complete
// Keep checking the interrupt. If the command can't be
// completed within "waitCnt", time out error is return

STATUS BCM_wait_for_complete (U32 waitCnt);


//------------------------------------------------------- Check return status
// Check BCM return status
void BCM_check_status (STATUS status);


//------------------------------------------------------- Obtain BCM Version
// Inputs are the destination addresses of core and
// platform version of the irom in AXI address space
// Enough space must be allocated.
//
// Prevent of changing the argument (can't use const cuz it is @ left hand side)
STATUS BCM_irom_version (U32*);


//------------------------------------------------------- Obtain BCM Version
// This function must be called after basic erom
// is loaded
STATUS BCM_erom_version (U32*);


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
void Load_Pattern( U32 mode, U32 numWords, const U32 *pIn, U32 *pOut);

//------------------------------------------------------- Compare Data
// Compare two 32-bit data
//
// endian = 0:Compare without change endian
//          1:Change d0's endian
void Comp_Pattern(U32 endian, U32 d0, U32 d1);

#ifdef BCM_EROM_SUPPORT
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
                U32 imgSizeByte, U32 keyLenBit, U32 tokLenBit, U32 hashMode, U32 sel);


//------------------------------------------------------- Load Basic EROM
// This function will load basic EROM to BCM,
//
// sel: 0=BCM_FW_AUTHORIZE 1=BCM_SOC_TWO_FACTOR_FW_AUTHORIZE
//
// Notes:
// 1. "signedErom.h" must be created by SHA1 1024-bit
// 2. If two-factor, token must be loaded to OTP (opt_init.dat)
// 3. The array in signedErom.h places data's MSW
//    at array[0]

STATUS BcmDownLoadErom (U32 *bufAddr, U32 sel);


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
STATUS BCM_load_erom (U32 sel, U32 *pMEM);


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
STATUS BCM_load_rsa_erom (U32 sel, U32 *pMEM);


//------------------------------------------------------- Unload EROM
// This function will unload EROM,
STATUS HostCall_bcm_unload_EROM(void);


//------------------------------------------------------- APB Write
// Can only be used after APB EROM is loaded to BCM
//
// port:  0=Midea_AES, 1=OTP
// waddr: Write apb address (byte address)
// wdata: Write data
//
// Notes: APB write data is provide within the Mailbox
//        AXI interface is not used
STATUS BCM_apb_write (U32 port, U32 waddr, U32 wdata);


//------------------------------------------------------- APB Read
// Can only be used after APB EROM is loaded to BCM
//
// port:  0=Midea_AES, 1=OTP
// raddr: Read apb address (byte address)
// rdata: Read data destination address
//
// Notes: APB read data value will be in Mailbox status 0
//        AXI interface is not used
STATUS BCM_apb_read (U32 port, U32 raddr, U32 *rdata);

//------------------------------------------------------- APB Read VDD setpoint
// Can only be used after APB EROM is loaded to BCM
//
//
// Notes: APB read data value will be in Mailbox status 0
//        AXI interface is not used
STATUS BCM_otp_read_vdd_setpoint(U32* pSetPoint);
#endif


//-----------------------------------------------------------------------------
/**
    Name:   STATUS  Host_IPC_Cmd_2BCM

    Description:    Load params to IPC and issue a command to send the params
                    to BCM, then wait to the command complete.
                    This is a function with a veriable number of params.

    @param[in]      U32  cmd         -  an enum value for an IROM/EROM function
    @param[in]      U32  waits       -  number of loops to wait while checking command done status from BCM.
    @param[in]      U32  numParams   -  Number of parameters in the variable-length
                                        argument list.
                                     The Min numParams is 0,
                                     the Max nmuParam is 12 which is limited by
                                     BCM architecture.

    @param[in]      U32 param0, U32 param1, ... - The variable-length arguments list.
    @param[out]     None

    @return         status code
**/
//-----------------------------------------------------------------------------
STATUS Host_IPC_Cmd_2BCM (U32 cmd, U32 waits, U32 numParams, ...);



STATUS HostCall_bcm_erom_otp_read(U32 otpOp, U32 addr);

// Need info
void memClr(U32 bytes, void *ptr);
void memSet(void *ptr, U8 pattern, U32 bytes);
void memCpy(U32 *pDes, U32 *pSrc, U32 words);

