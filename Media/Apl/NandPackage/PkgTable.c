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
//! @brief NAND Flash Package Table information
//!
//=============================================================================

//-----------------------------------------------------------------------------
//  Include files:
//-----------------------------------------------------------------------------
#include "BuildPkg.h"
#include "SharePkg.h"
#include "HalNfif.h"

#include "FastSection.h"   // CODE AND DATA MEMORY MAPPING

//-----------------------------------------------------------------------------
//  Macros definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data type definitions: typedef, struct or class
//-----------------------------------------------------------------------------
/// @brief NAND Flash Package table
typedef struct
{
    uint8_t idInfo[6];           ///< NAND flash Identifier
    uint8_t idLength;            ///< effective Id length
    uint8_t ifType;              ///< Interface type
    Cstr_t description;       ///< NAND description
} PkgInfoTbl_t;

//-----------------------------------------------------------------------------
//  Private function prototype definitions:
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Data declaration: Private or Public
//-----------------------------------------------------------------------------
// NAND Flash Package List
extern const PkgInfoTbl_t gPkgList[] =
{
    { {0x98, 0x3A, 0xA5, 0x82, 0x7A, 0xD6 }, 6, NF_JEDEC,   "Toshiba TH58TEG8D2HBA8C"},
    { {0x98, 0x3A, 0x95, 0x93, 0x7A, 0xD0 }, 6, NF_JEDEC,   "Toshiba TH58TEG9DDKBA8H"},
    { {0x98, 0x3A, 0x95, 0x93, 0x7A, 0xD7 }, 6, NF_JEDEC,   "Toshiba TH58TEG9DDJBA89"},
    { {0x98, 0xD7, 0xA0, 0x32, 0x76, 0xD6 }, 6, NF_JEDEC,   "Toshiba TH58TEG6H2HBA4C"},
    { {0x98, 0xDE, 0x94, 0x93, 0x76, 0xD7 }, 6, NF_JEDEC,   "Toshiba TH58TEG7DDJBA4C"},
    { {0x98, 0xDE, 0xA4, 0x82, 0x76, 0xD7 }, 6, NF_JEDEC,   "Toshiba TH58TEG8D2JBA8C"},
    { {0x98, 0x3C, 0xA9, 0x92, 0x7A, 0x50 }, 6, NF_JEDEC,   "Toshiba TH58TEG9TDKTA20"},
    { {0x98, 0xD7, 0xD4, 0x32, 0x76, 0xD5 }, 6, NF_JEDEC,   "Toshiba TH58TVG7E2FBA89"},
    { {0x98, 0x3C, 0x98, 0xB3, 0x76, 0xF1 }, 6, NF_JEDEC,   "Toshiba TH58TFG9T22BA4C"},
    { {0x98, 0x00, 0x00, 0x00, 0x00, 0x00 }, 1, NF_JEDEC,   "Toshiba" },
    { {0xAD, 0xE7, 0x94, 0xDA, 0x74, 0xC3 }, 6, NF_ONFI,    "Hynix H27QDG8VEBIR" },
    { {0xAD, 0x00, 0x00, 0x00, 0x00, 0x00 }, 1, NF_ONFI,    "Hynix" },
    { {0x2C, 0x68, 0x04, 0x46, 0x89, 0x00 }, 6, NF_ONFI,    "Micron MT29F32G08CBCBBH1" },
    { {0x2C, 0x88, 0x04, 0x4B, 0xA9, 0x00 }, 6, NF_ONFI,    "Micron MT29F256G08CMCABH2" },
    { {0x2C, 0xA4, 0xE5, 0x54, 0xA9, 0x00 }, 6, NF_ONFI,    "Micron MT29F512G08CKCCBH7" },
    { {0x2C, 0x84, 0x64, 0x54, 0xA9, 0x00 }, 6, NF_ONFI,    "Micron MT29F512G08CMCCB" },
    { {0x2C, 0x00, 0x00, 0x00, 0x00, 0x00 }, 1, NF_ONFI,    "Micron" },
    { {0xEC, 0xD7, 0x14, 0x76, 0x54, 0xC2 }, 6, NF_JEDEC,   "Samsung K9HDGD8U5M" },
    { {0xEC, 0x00, 0x00, 0x00, 0x00, 0x00 }, 1, NF_JEDEC,   "Samsung" },
    { {0x45, 0x3E, 0xB6, 0x92, 0x7E, 0x50 }, 6, NF_JEDEC,   "SanDisk" },
    { {0x45, 0x3A, 0xA5, 0x93, 0x7E, 0x51 }, 6, NF_JEDEC,   "SanDisk SDYNSHDRM128GEC"},
    { {0x4D, 0x01, 0x00, 0x00, 0x00, 0x00 }, 2, NF_ONFI,    "Marvell NFM ONFI Simple" },
    { {0x4D, 0x11, 0x00, 0x00, 0x00, 0x00 }, 2, NF_ONFI,    "Marvell NFM ONFI Complex" },
    { {0x4D, 0x31, 0x00, 0x00, 0x00, 0x00 }, 2, NF_JEDEC,   "Marvell NFM TOGGLE Simple" },
    { {0x4D, 0x41, 0x00, 0x00, 0x00, 0x00 }, 2, NF_JEDEC,   "Marvell NFM TOGGLE Complex" },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 1, NF_UNKNOWN, "Not found" },
    { {0xFF, 0x00, 0x00, 0x00, 0x00, 0x00 }, 1, NF_UNKNOWN, "Not found" },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0, NF_UNKNOWN, "Unknown" }
};

//-----------------------------------------------------------------------------
//  Imported data prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Imported function prototype without header include
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Codes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    Get NAND Flash Package description.

    @param[in]  idInfo  NAND ID information

    @return NAND Package description string pointer
**/
//-----------------------------------------------------------------------------
Cstr_t Pkg_GetDescriptionString(const uint8_t idInfo[])
{
    const PkgInfoTbl_t* pTable = gPkgList;

    while (TRUE)
    {
        if (MEM_CMP(pTable->idInfo, idInfo, pTable->idLength)==0)
        {
            return pTable->description;
        }

        pTable++;
    }
}

