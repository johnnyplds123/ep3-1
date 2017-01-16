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
//! @brief Memory section directive for code and data
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include
//-----------------------------------------------------------------------------
#pragma thumb
#pragma arm section code   = "code_norm"
#pragma arm section rwdata = "data_norm"
#pragma arm section zidata = "data_norm"
#pragma arm section rodata = "data_norm_ro"

