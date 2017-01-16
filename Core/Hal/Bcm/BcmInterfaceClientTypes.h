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
//! @file   BcmInterfaceClientTypes.h
//! @brief  All Data types serving Interface block and Client block of BCM EROM code.
// 
// ============================================================================
//
// Notes: 
//    If BCM IROM or EROM is modified, ones needs to make sure all data structure
//    here is matching those in new BCM IROM or EROM.


#pragma once


typedef struct MaesARangeKeyIV_t
{
    uint32_t key[8];
    uint32_t XTSkey[8];
    uint32_t IV[4];
} MaesARangeKeyIV_t;


