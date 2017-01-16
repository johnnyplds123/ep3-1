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
//! @brief Media AES registers (Ref: 88SS1093_Security_Hardware_Standard.pdf - 09/15/2014)
//!
//=============================================================================

typedef unsigned int    uint32;

typedef struct _MEDIA_AES
{
    uint32 AesEngine0Configuration                        ; // RAES00000h     
    uint32 AesEngine0Control                              ; // RAES00004h     
    uint32 AesEngine0Command                              ; // RAES00008h     
    uint32 rsvd_0x000c_0x0010[(0x0010-0x000c)/4]          ; //            
    uint32 AesEngine0Status                               ; // RAES00010h     
    uint32 rsvd_0x0014_0x0020[(0x0020-0x0014)/4]          ; //            
    uint32 AesEngine0Monitor                              ; // RAES00020h     
    uint32 rsvd_0x0024_0x0100[(0x0100-0x0024)/4]          ; //            
    uint32 AesEngine1Configuration                        ; // RAES00100h     
    uint32 AesEngine1Control                              ; // RAES00104h     
    uint32 AesEngine1Command                              ; // RAES00108h     
    uint32 rsvd_0x010c_0x0110[(0x0110-0x010c)/4]          ; //            
    uint32 AesEngine1Status                               ; // RAES00110h     
    uint32 rsvd_0x0114_0x0120[(0x0120-0x0114)/4]          ; //            
    uint32 AesEngine1Monitor                              ; // RAES00120h     
    uint32 rsvd_0x0124_0x0200[(0x0200-0x0124)/4]          ; //            
    uint32 AesSharedControl                               ; // RAES00200h     
    uint32 AesSharedCommand                               ; // RAES00204h     
    uint32 rsvd_0x0208_0x020c[(0x020c-0x0208)/4]          ; //            
    uint32 AesSharedEngineStatus                          ; // RAES0020Ch     
    uint32 AesSharedMemoryPowerDownControl                ; // RAES00210h     
    uint32 AesSharedMemoryTimeControl                     ; // RAES00214h     
    uint32 rsvd_0x0218_0x0220[(0x0220-0x0218)/4]          ; //            
    uint32 AesSharedBISTControl                           ; // RAES00220h     
    uint32 AesSharedBISTStatus                            ; // RAES00224h     
    uint32 rsvd_0x0228_0x0230[(0x0230-0x0228)/4]          ; //            
    uint32 AesSharedKey1AccessPointer                     ; // RAES00230h     
    uint32 AesSharedKey1AccessDWord                       ; // RAES00234h     
    uint32 rsvd_0x0238_0x0240[(0x0240-0x0238)/4]          ; //            
    uint32 AesSharedKey1AccessStatus                      ; // RAES00240h     
    uint32 rsvd_0x0244_0x0248[(0x0248-0x0244)/4]          ; //            
    uint32 AesSharedKey1AccessStatusParity                ; // RAES00248h     
    uint32 AesSharedKey1AccessStatusDWord0                ; // RAES0024Ch     
    uint32 AesSharedKey1AccessStatusDWord1                ; // RAES00250h     
    uint32 rsvd_0x0254_0x0260[(0x0260-0x0254)/4]          ; //            
    uint32 AesSharedKey2AccessPointer                     ; // RAES00260h     
    uint32 AesSharedKey2AccessDWord                       ; // RAES00264h     
    uint32 rsvd_0x0268_0x0270[(0x0270-0x0268)/4]          ; //            
    uint32 AesSharedKey2AccessStatus                      ; // RAES00270h     
    uint32 rsvd_0x0274_0x0278[(0x0278-0x0274)/4]          ; //            
    uint32 AesSharedKey2AccessStatusParity                ; // RAES00278h     
    uint32 AesSharedKey2AccessStatusDWord0                ; // RAES0027Ch     
    uint32 AesSharedKey2AccessStatusDWord1                ; // RAES00280h     
    uint32 rsvd_0x0284_0x0290[(0x0290-0x0284)/4]          ; //            
    uint32 AesSharedInitializationVectorAccessPointer     ; // RAES00290h     
    uint32 AesSharedInitializationVectorAccessDWord       ; // RAES00294h     
    uint32 rsvd_0x0298_0x02a0[(0x02a0-0x0298)/4]          ; //            
    uint32 AesSharedInitializationVectorAccessStatus      ; // RAES002A0h     
    uint32 rsvd_0x02a4_0x02a8[(0x02a8-0x02a4)/4]          ; //            
    uint32 AesSharedInitializationVectorAccessStatusParity; // RAES002A8h     
    uint32 AesSharedInitializationVectorAccessStatusDWord0; // RAES002ACh     
    uint32 AesSharedInitializationVectorAccessStatusDword1; // RAES002B0h     
    uint32 rsvd_0x02b4_0x02c0[(0x02c0-0x02b4)/4]          ; //            
    uint32 AesSharedLogicBlockAddressAccessPointer        ; // RAES002C0h     
    uint32 AesSharedLogicBlockAddressAccessDWord0         ; // RAES002C4h     
    uint32 AesSharedLogicBlockAddressAccessDWord1         ; // RAES002C8h     
    uint32 AesSharedLogicBlockAddressAccessBlockCount     ; // RAES002CCh     
    uint32 rsvd_0x02d0_0x02e0[(0x02e0-0x02d0)/4]          ; //            
    uint32 AesSharedMiscellaneousRangeWriteEnable         ; // RAES002E0h     
    uint32 AesSharedMiscellaneousRangReadEnable           ; // RAES002E4h     
    uint32 AesSharedMiscellaneousRangBypass               ; // RAES002E8h     
    uint32 AesSharedMiscellaneousRangValid                ; // RAES002ECh     
    uint32 AesSharedMiscellaneousKeySize0                 ; // RAES002F0h     
    uint32 AesSharedMiscellaneousKeySize1                 ; // RAES002F4h     
} MEDIA_AES;

// Check the size of this structure
extern char __ct_[(sizeof(MEDIA_AES)==0x02f8) ? 1 : -1]; ///< compile time assert






