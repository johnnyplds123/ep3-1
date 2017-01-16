//-----------------------------------------------------------------------------
// Title         : BCM Functions
// Project       : CZ1
//-----------------------------------------------------------------------------
// File          : bcm_functions.c
// Description   : Contains all the BCM function calls
//
//-----------------------------------------------------------------------------


#include "BuildPkg.h"
#include "SharePkg.h"
#include "bcmClientSection.h"
#include "BcmClientTypedef.h"
#include "BcmClientUtilities.h"        // Contains BCM basic utilities


//=============== Contains the follwoing functions ======================
// BCM AES
//----------
// BCM_aes_load_key
// BCM_aesx_load_key
// BCM_aes_load_iv
// BCM_aesx_load_iv
// BCM_aes_init
// BCM_aesx_init
// BCM_aes_process
// BCM_aesx_process
// BCM_aes_key_gen
// BCM_aes_key_wrap
// BCM_aes_engine
// BCM_aesx_engine
// BCM_aes_mre
//
// HASH
//----------
// BCM_hash_init
// BCM_hash_update
// BCM_hash_final
// BCM_hash_zerorize
// BCM_hash_engine
//
// HMAC
//----------
// BCM_hmac_load_key
// BCM_hmac_init
// BCM_hmac_update
// BCM_hmac_final
// BCM_hmac_zerorize
// BCM_hmac_key_gen
// BCM_hmac_engine
//
// PKCS
//----------
// BCM_pkcs_v15_verify
//
//----------
//
// Media AES
//----------
// BCM_maes_load_key
// BCM_maes_read_key
// BCM_maes_load_iv
// BCM_maes_read_iv
// BCM_maes_load_lba
// BCM_maes_read_lba
// BCM_maes_init_table
//
// OTP
//----------
// BCM_otp_write
// BCM_otp_read
//
// Others
//----------
//
//=======================================================================

void BCM_reset (void)
{
    STATUS status;

    //simplePrintf("%s\n", __FUNCTION__);
    status = Host_IPC_Cmd_2BCM (BCM_RESET, 0x10000, 0);
    BCM_check_status(status);
}

void BCM_self_test (U32 test_map)
{
    STATUS status;

    //simplePrintf("%s\n", __FUNCTION__);
    status = Host_IPC_Cmd_2BCM (BCM_SELF_TEST, 0x10000, 1, test_map);
    BCM_check_status(status);
}

#if 0   // BCM_aes_xxx / BCM_hash_xxx / BCM_hmac_xxx functions are marked off.
//--------------------------------------------------------------------------------+
//                                                                            AES |
//--------------------------------------------------------------------------------+
//--------------------------------------------------------- AES Zerorize
void BCM_aes_zerorize (void)
{
    STATUS status;

    simplePrintf("%s\n", __FUNCTION__);
    status = Host_IPC_Cmd_2BCM (BCM_AES_ZERORIZE, 0x100000, 0);
    BCM_check_status(status);
}

//--------------------------------------------------------- AES Load Key
// src       : Key source address
// keyLenBit : Key Length in Bit (128,192,256)
// keySel    : 0=Key, 1=XTS_Key
// id        : 0=Null, 2=AES, 7=AES2
//
// Notes:
// 1. RKEK is used if Key address is 0xffff_ffff
// 2. The id is used to select the AES engine (only used
//    when there are 2 AES in BCM) When id=0, default AES
//    engine would be used. Don't use any other value
//    other than 0, 1, and 2

void BCM_aes_load_key (U32 *src, U32 keyLenBit, U32 keySel)
{
    STATUS status;

    printf_tube(": AES Load Key Command\n");
    status = Host_IPC_Cmd_2BCM (BCM_AES_LOAD_KEY, 0x10000, 3, keyLenBit, (U32)src, keySel);
    BCM_check_status(status);
}

void BCM_aesx_load_key (U32 *src, U32 keyLenBit, U32 keySel, ENGINE_ID id)
{
    STATUS status;

    printf_tube(": AES ID=%x Load Key Command\n", id);
    status = Host_IPC_Cmd_2BCM (BCM_AESX_LOAD_KEY, 0x10000, 4, keyLenBit, (U32)src, keySel, id);
    BCM_check_status(status);
}

//--------------------------------------------------------- AES Load IV
// src : IV source address
// id  : 0=Null, 2=AES, 7=AES2
//
// Notes:
// The id is used to select the AES engine (only used when
// there are 2 AES in BCM) When id=0, default AES engine
// would be used. Don't use any other value other than
// 0, 1, and 2

void BCM_aes_load_iv (U32 *src)
{
    STATUS status;

    printf_tube(": AES Load IV Command\n");
    status = Host_IPC_Cmd_2BCM (BCM_AES_LOAD_IV, 0x10000, 1, src);
    BCM_check_status(status);
}

void BCM_aesx_load_iv (U32 *src, ENGINE_ID id)
{
    STATUS status;

    printf_tube(": AES ID=%x Load IV Command\n", id);
    status = Host_IPC_Cmd_2BCM (BCM_AESX_LOAD_IV, 0x10000, 2, (U32)src, id);
    BCM_check_status(status);
}

//--------------------------------------------------------- AES Init
// direction : 0=Encrypt, 1=Decrypt
// keyLenBit : Key Length in Bit (128,192,256)
// mode      : 0=ECB, 1=CBC, 2=CTR, 3=XTS, 4=KWP

void BCM_aes_init (U32 direction, U32 keyLenBit, U32 mode)
{
    STATUS status;

    printf_tube(": AES Init Command\n");
    status = Host_IPC_Cmd_2BCM (BCM_AES_INIT, 0x10000, 3, direction, keyLenBit, mode);
    BCM_check_status(status);
}

// direction : 0=Encrypt, 1=Decrypt
// keyLenBit : Key Length in Bit (128,192,256)
// mode      : 0=ECB, 1=CBC, 2=CTR, 3=XTS, 4=KWP
// id        : 0=Null, 2=AES, 7=AES2
// iv_mode   : 0=None, 1=Swap, 2=XOR
// iv_mask   : 0=MSB16, 1=MSB32, 2=MSB48, 3=MSB64, 4=LBS16, 5=LSB32, 6=LSB48, 7=LSB64
// lba_hi    : MSB 32-bit LBA value
// lba_lo    : LSB 32-bit LBA value
// interleave: CBC interleaving mode enable
// endian    : 0=Little, 1=Big
// ctr_mod   : CTR counter modulus
//
// Notes:
// The id is used to select the AES engine (only used when
// there are 2 AES in BCM) When id=0, default AES engine
// would be used. Don't use any other value other than
// 0, 1, and 2

void BCM_aesx_init (U32 direction,  U32 keyLenBit, U32 mode, ENGINE_ID id,
            U32 iv_mode,    U32 iv_mask,   U32 lba_hi, U32 lba_lo,
            U32 interleave, U32 endian,    U32 ctr_mod)
{
    STATUS status;

    printf_tube(": AES ID=%x Init Command\n", id);
    status = Host_IPC_Cmd_2BCM (BCM_AESX_INIT, 0x10000, 11, direction, keyLenBit, mode, id, iv_mode, iv_mask, lba_hi, lba_lo, interleave, endian, ctr_mod);
    BCM_check_status(status);
}

//--------------------------------------------------------- AES Process
// src         : Input  data source address
// dest        : Output data destination address
// dataLenByte : data length in byte
// flag        : 0=Continue on previous task, 1=New task

void BCM_aes_process (U32 *src, U32 *dest, U32 dataLenByte, U32 flag)
{
    STATUS status;

    printf_tube(": AES Process Command\n");
    status = Host_IPC_Cmd_2BCM (BCM_AES_PROCESS, 0x10000, 4, (U32)src, (U32)dest, dataLenByte, flag);
    BCM_check_status(status);
}

// src         : Input  data source address
// dest        : Output data destination address
// dataLenByte : data length in byte
// flag        : 0=Continue on previous task, 1=New task
// numSet      : Number of sector (0=1_sector, 1=2_sectors...etc)
// id          : 0=Null, 2=AES, 7=AES2

void BCM_aesx_process (U32 *src, U32 *dest, U32 dataLenByte, U32 flag, U32 numSet, ENGINE_ID id)
{
    STATUS status;

    printf_tube(": AES ID=%x Process Command\n", id);
    status = Host_IPC_Cmd_2BCM (BCM_AESX_PROCESS, 0x10000, 6, (U32)src, (U32)dest, dataLenByte, flag, numSet, id);
    BCM_check_status(status);
}

//--------------------------------------------------------- AES Key Gen
// dest      : Output key destination address
// keyLenBit : Key Length in Bit (128,192,256)

void BCM_aes_key_gen (U32 *dest, U32 keyLenBit)
{
    STATUS status;

    printf_tube(": AES key Gen Command\n");
    status = Host_IPC_Cmd_2BCM (BCM_AES_KEY_GEN, 0x100000, 2, keyLenBit, (U32)dest);
    BCM_check_status(status);
}

#ifdef BCM_EROM_SUPPORT
//--------------------------------------------------------- AES Key Wrap Old
// This is an EROM key wrap function call (old version)
//
// d_src       : Input  data source address
// d_dest      : Output data destination address
// param_src   : array[0] -> Key source address
//             : array[1] -> Key size
//             : array[2] -> IV  source address
//             : array[3] -> IV  size (must be 128)
// iv_dest     : IV destination address
// direction   : 0=Wrap, 1=Unwrap
// dataLenWord : Data length in word
//
// Notes: RKEK is used if input Key address is 0xffff_ffff

void BCM_aes_key_wrap (U32 *d_src, U32 *d_dest, U32 *param_src, U32 *iv_dest,
            U32 direction, U32 dataLenWord)
{
    STATUS status;

    printf_tube(": AES key Wrap/Unwrap Command (Old Version) \n");
    status = Host_IPC_Cmd_2BCM (EROM_AES_KEY_WRAP, 0x100000, 6, direction, (U32)d_src, dataLenWord, (U32)d_dest, (U32)iv_dest, (U32)param_src);
    BCM_check_status(status);
}
#endif
//--------------------------------------------------------- AES Engine Call
// src         : Input  data source address
// dest        : Output data destination address
// pKey1       : Key1 source address
// pKey2       : Key2 source address
// pIV         : IV source address
// mode        : 0=ECB, 1=CBC, 2=CTR, 3=XTS, 4=KWP
// dir         : 0=Encrypt, 1=Decrypt
// dataLenByte : Data length in byte
// keyLenBit   : Key length in Bit
//
//Notes: RKEK is used if Key pointer is 0xffff_ffff

void BCM_aes_engine (U32 *src, U32 *dest, U32 *pKey1, U32 *pKey2, U32 *pIV,
            U32 mode, U32 dir, U32 dataLenByte, U32 keyLenBit)
{
  printf_tube(": === AES Engine Call ===\n");
  if (mode!=0) BCM_aes_load_iv  (pIV);                 // Load IV   (not for ECB)
  if (mode==3) BCM_aes_load_key (pKey2, keyLenBit, 1); // Load Key2 (XTS only)
  BCM_aes_load_key (pKey1, keyLenBit, 0);              // Load Key1
  BCM_aes_init     (dir, keyLenBit, mode);             // AES init
  BCM_aes_process  (src, dest, dataLenByte, 1);        // AES process
}

// src         : Input  data source address
// dest        : Output data destination address
// pKey1       : Key1 source address
// pKey2       : Key2 source address
// pIV         : IV source address
// mode        : 0=ECB, 1=CBC, 2=CTR, 3=XTS, 4=KWP
// dir         : 0=Encrypt, 1=Decrypt
// dataLenByte : Data length in byte
// keyLenBit   : Key length in Bit
// iv_mode     : 0=None, 1=Swap, 2=XOR
// iv_mask     : 0=MSB16, 1=MSB32, 2=MSB48, 3=MSB64, 4=LBS16, 5=LSB32, 6=LSB48, 7=LSB64
// lba_hi      : MSB 32-bit LBA value
// lba_lo      : LSB 32-bit LBA value
// interleave  : CBC interleaving mode enable
// endian      : 0=Little, 1=Big
// numSet      : Number of sector (0=1_sector, 1=2_sectors...etc)
// ctr_mod     : CTR counter modulus
// id          : Engine ID (ENGINE_ID_AES or ENGINE_ID_AES2)

void BCM_aesx_engine (U32 *src, U32 *dest, U32 *pKey1, U32 *pKey2, U32 *pIV,
            U32 mode, U32 dir, U32 dataLenByte, U32 keyLenBit,
            U32 iv_mode,    U32 iv_mask,   U32 lba_hi, U32 lba_lo,
            U32 interleave, U32 endian,    U32 numSet, U32 ctr_mod,
            ENGINE_ID id)
{
  printf_tube(": === AESX Engine Call ===\n");
  if (mode!=0) BCM_aesx_load_iv  (pIV, id);
  if (mode==3) BCM_aesx_load_key (pKey2, keyLenBit, 1, id);
  BCM_aesx_load_key (pKey1, keyLenBit, 0, id);
  BCM_aesx_init     (dir, keyLenBit, mode, id, iv_mode, iv_mask, lba_hi, lba_lo, interleave, endian, ctr_mod);
  BCM_aesx_process  (src, dest, dataLenByte, 1, numSet, id);
}

//--------------------------------------------------------- AES MRE
// AES 1 engine is decryption
// AES 2 engine is encryption
//
// src          : Input  data source address
// dest         : Output data destination address
// dataLenByte  : Data length in byte
// numSet       : Number of sector (0=1_sector, 1=2_sectors...etc)
// new_flag     : 0:Resume, 1=NewTask
// a1_pKey1     : Key1 source address for AES 1 engine
// a1_pKey2     : Key2 source address for AES 1 engine
// a1_pIV       : IV source address for AES 1 engine
// a1_mode      : AES 1 mode - 0=ECB, 1=CBC, 2=CTR, 3=XTS, 4=KWP
// a1_keyLenBit : AES 1 Key length in Bit
// a2_pKey1     : Key1 source address for AES 2 engine
// a2_pKey2     : Key2 source address for AES 2 engine
// a2_pIV       : IV source address for AES 2 engine
// a2_mode      : AES 2 mode - 0=ECB, 1=CBC, 2=CTR, 3=XTS, 4=KWP
// a2_keyLenBit : AES 2 Key length in Bit

void BCM_aes_mre (U32 *src, U32 *dest, U32 dataLenByte, U32 numSet, U32 new_flag,
          U32 *a1_pKey1, U32 *a1_pKey2, U32 *a1_pIV, U32 a1_mode, U32 a1_keyLenBit,
          U32 *a2_pKey1, U32 *a2_pKey2, U32 *a2_pIV, U32 a2_mode, U32 a2_keyLenBit)
{
    STATUS status;
    printf_tube(": === AES MRE Call ===\n");

    // AES 1 Engine Set Up (Decryption)
    if (a1_mode!=0) BCM_aesx_load_iv  (a1_pIV, ENGINE_ID_AES);
    if (a1_mode==3) BCM_aesx_load_key (a1_pKey2, a1_keyLenBit, 1, ENGINE_ID_AES);
    BCM_aesx_load_key (a1_pKey1, a1_keyLenBit, 0, ENGINE_ID_AES);
    BCM_aesx_init     (1, a1_keyLenBit, a1_mode, ENGINE_ID_AES, 0, 0, 1, 2, 0, 0, 0);

    // AES 2 Engine Set Up (Encryption)
    if (a2_mode!=0) BCM_aesx_load_iv  (a2_pIV, ENGINE_ID_AES);
    if (a2_mode==3) BCM_aesx_load_key (a2_pKey2, a2_keyLenBit, 1, ENGINE_ID_AES);
    BCM_aesx_load_key (a2_pKey1, a2_keyLenBit, 0, ENGINE_ID_AES);
    BCM_aesx_init     (0, a2_keyLenBit, a2_mode, ENGINE_ID_AES, 0, 0, 1, 2, 0, 0, 0);

    // Re-encryption
    status = Host_IPC_Cmd_2BCM (BCM_AESX_RECRYPT_PROCESS, 0x10000, 5, (U32)src, (U32)dest, dataLenByte, new_flag, numSet);
    BCM_check_status(status);
}
#endif
//--------------------------------------------------------------------------------+
//                                                                           HASH |
//--------------------------------------------------------------------------------+

//--------------------------------------------------------- HASH Init
// mode : 0=SHA1, 1=SHA224, 2=SHA256, 3=MD5
// flag : 0=Digest stays in register, 1=Output to other engines

void BCM_hash_init (U32 mode, U32 flag)
{
    STATUS status;

    //printf_tube(".h1"); //(": HASH Init Command\n");
    status = Host_IPC_Cmd_2BCM (BCM_HASH_INIT, 0x10000, 2, mode, flag);
    BCM_check_status(status);
}


//--------------------------------------------------------- HASH Update
// src         : Input data source address
// dataLenByte : Input data length in byte
void BCM_hash_update (U32 *src, U32 dataLenByte)
{
    STATUS status;

    //printf_tube(".h2"); //ASH Update Command\n");
    status = Host_IPC_Cmd_2BCM (BCM_HASH_UPDATE, 0x10000, 2, src, dataLenByte);
    BCM_check_status(status);
}


//--------------------------------------------------------- HASH Final
// src        : Input data source address
// dest       : Output digest destination address
// segLenByte : segment length in byte
// pad        : 0=FW padding, 1=HW padding

void BCM_hash_final (U32 *src, U32 *dest, U32 segLenByte, U32 pad)
{
    STATUS status;

    //printf_tube(".h3"); //ASH Final Command\n");
    status = Host_IPC_Cmd_2BCM (BCM_HASH_FINAL, 0x100000, 4, (U32)src, (U32)dest, segLenByte, pad);
    BCM_check_status(status);
}


//--------------------------------------------------------- HASH Zerorize
void BCM_hash_zerorize ()
{
    STATUS status;

    //printf_tube(".h0\n");  //ASH Zerorize Command\n");
    status = Host_IPC_Cmd_2BCM (BCM_HASH_ZERORIZE, 0x100000, 0);
    BCM_check_status(status);
}


//--------------------------------------------------------- HAES Engine Call
// src         : Message source address
// dest        : Digest destination address
// mode        : 0=SHA1, 1=SHA224, 2=SHA256, 3=MD5
// dataLenByte : Message length in byte
// bytePerOp   : Num of byte hash process for each hash update

void BCM_hash_engine (U32 *src, U32 *dest, U32 mode, U32 dataLenByte, U32 bytePerOp)
{
  U32 dataLeftByte = dataLenByte;
  U32 addr = (U32)   src;    // Current msg address
  U32 *pMsg = (U32 *) addr;

  //printf_tube(": === Hash Engine Call === %X %X\n", (U32)pMsg, bytePerOp);
  BCM_hash_init (mode, 0); // Digest wills tay in register

  while (dataLeftByte>=bytePerOp)  // Hash update using bytePerOp
    {
      BCM_hash_update (pMsg, bytePerOp);
      pMsg = (U32 *) (addr+bytePerOp);
      dataLeftByte-=bytePerOp;
    }

  BCM_hash_final (pMsg, dest, dataLeftByte, 1); // Hash Final on residual
  BCM_hash_zerorize ();
}
#if 0
//--------------------------------------------------------------------------------+
//                                                                           HMAC |
//--------------------------------------------------------------------------------+

//--------------------------------------------------------- HMAC Load Key
// src        : Key source address
// keyLenByte : Key Length in Byte

void BCM_hmac_load_key (U32 *src, U32 keyLenByte)
{
    STATUS status;

    printf_tube(": HMAC Load key Command\n");
    status = Host_IPC_Cmd_2BCM (BCM_HMAC_LOAD_KEY, 0x10000, 2, keyLenByte, (U32)src);
    BCM_check_status(status);
}

//--------------------------------------------------------- HMAC Init
// mode : 0=SHA1, 1=SHA224, 2=SHA256, 3=MD5
// flag : 0=Digest stays in register, 1=Output to other engines

void BCM_hmac_init (U32 mode, U32 flag)
{
    STATUS status;

    printf_tube(": HMAC Init Command\n");
    status = Host_IPC_Cmd_2BCM (BCM_HMAC_INIT, 0x10000, 2, mode, flag);
    BCM_check_status(status);
}

//--------------------------------------------------------- HMAC Update
// src         : Input data source address
// dataLenByte : Input data length in byte

void BCM_hmac_update (U32 *src, U32 dataLenByte)
{
    STATUS status;

    printf_tube(": HMAC Update Command\n");
    status = Host_IPC_Cmd_2BCM (BCM_HMAC_UPDATE, 0x10000, 2, (U32)src, dataLenByte);
    BCM_check_status(status);
}

//--------------------------------------------------------- HMAC Final
// src        : Input data source address
// dest       : Output digest destination address
// segLenByte : segment length in byte
// pad        : 0=FW padding, 1=HW padding

void BCM_hmac_final (U32 *src, U32 *dest, U32 segLenByte, U32 pad)
{
    STATUS status;

    printf_tube(": HMAC Final Command\n");
    status = Host_IPC_Cmd_2BCM (BCM_HMAC_FINAL, 0x10000, 4, (U32)src, (U32)dest, segLenByte, pad);
    BCM_check_status(status);
}

//--------------------------------------------------------- HASH Zerorize
void BCM_hmac_zerorize ()
{
    STATUS status;

    printf_tube(": HMAC Zerorize Command\n");
    status = Host_IPC_Cmd_2BCM (BCM_HMAC_ZERORIZE, 0x10000, 0);
    BCM_check_status(status);
}


//--------------------------------------------------------- HMAC Key Gen
// dest       : Output key destination address
// keyLenByte : Key Length in Byte

void BCM_hmac_key_gen (U32 *dest, U32 keyLenByte)
{
    STATUS status;

    printf_tube(": HMAC Key Gen Command\n");
    status = Host_IPC_Cmd_2BCM (BCM_HMAC_KEY_GEN, 0x100000, 2, keyLenByte, (U32)dest);
    BCM_check_status(status);
}


//--------------------------------------------------------- HMAC Engine Call
// src         : Message source address
// dest        : Digest destination address
// pKey        : Key source address
// mode        : 0=SHA1, 1=SHA224, 2=SHA256, 3=MD5
// dataLenByte : Message length in byte
// bytePerOp   : Num of byte hash process for each hash update
// keyLenByte  : Key Length in Byte

void BCM_hmac_engine (U32 *src, U32 *dest, U32 *pKey,
           U32 mode, U32 dataLenByte, U32 bytePerOp, U32 keyLenByte)
{
  U32 dataLeftByte = dataLenByte;
  U32 addr  = (U32)   src;    // Current msg address
  U32 *pMsg = (U32 *) addr;

  printf_tube(": === HMAC Engine Call ===\n");
  BCM_hmac_load_key (pKey, keyLenByte);
  BCM_hmac_init     (mode, 0);

  while (dataLeftByte>=bytePerOp)  // Hash update using bytePerOp
    {
      BCM_hmac_update (pMsg, bytePerOp);
      pMsg = (U32 *) (addr+bytePerOp);
      dataLeftByte-=bytePerOp;
    }
  BCM_hmac_final (pMsg, dest, dataLeftByte, 1); // Hash Final on residual
  BCM_hmac_zerorize ();
}
#endif  //End of #if 0

//--------------------------------------------------------------------------------+
//                                                                           PKCS |
//--------------------------------------------------------------------------------+

//--------------------------------------------------------- PKCS Verify
// pMsg        : Message source address
// pMod        : Modulus source address
// pExp        : Public key exponent source address
// pSig        : Signature source address
// hashMode    : 0=SHA1, 1=SHA224, 2=SHA256, 3=MD5
// dataLenByte : Message length in byte
// keyLenBit   : Key length in bit

STATUS BCM_pkcs_v15_verify (U32 *pMsg, U32 *pMod, U32 *pExp, U32 *pSig,
            U32 hashMode, U32 dataLenByte, U32 keyLenBit)
{
    STATUS status;

    printf_tube(": PKCS Verify ...\n");
    status = Host_IPC_Cmd_2BCM (BCM_RSASSA_PKCS1_V15_VERIFY, 0x100000, 8, keyLenBit, (U32)pMod, \
                    (U32)pExp, hashMode, (U32)pMsg, dataLenByte, (U32)pSig, (keyLenBit/8));
    // Don't check it, it's a failed status.   BCM_check_status(status);
    return(status);
}

//--------------------------------------------------------------------------------+
//                                                                          ECDSA |
//--------------------------------------------------------------------------------+

#ifdef BCM_EROM_SUPPORT

//--------------------------------------------------------------------------------+
//                                                                      Media AES |
//--------------------------------------------------------------------------------+
// EROM must be loaded first for APB access

#if 0
//--------------------------------------------------------- Media AES Load Key
// src       : Key source address
// keyLenBit : Key Length in Bit (128,192,256)
// keySel    : 0=Key1, 1=Key2
// rangSel   : Range Select (32 sets key - 0~31)

void BCM_maes_load_key (U32 *src, U32 rangSel, U32 keyLenBit, U32 keySel)
{
  U32 i;
  //printf_tube(": === Media AES Load Key Function ===\n");

  // Auto Increament
  BCM_check_status( BCM_apb_write(0, SHR_CTRL, 0x1));

  // Key1
  if (keySel==0)
    {
      BCM_check_status( BCM_apb_write(0, SHR_KEY1_ACC_PTR, rangSel<<4)); // Select Range & reset pointer
      for (i=0; i<(keyLenBit/32); i++) BCM_check_status( BCM_apb_write(0, SHR_KEY1_ACC_DW, src[i]));
    }
  // Key2
  else
    {
      BCM_check_status( BCM_apb_write(0, SHR_KEY2_ACC_PTR, rangSel<<20)); // Select Range & reset pointer
      for (i=0; i<(keyLenBit/32); i++) BCM_check_status( BCM_apb_write(0, SHR_KEY2_ACC_DW, src[i]));
    }
}

//--------------------------------------------------------- Media AES Read Key
// dest      : read Key destination address
// keyLenBit : Key Length in Bit (128,192,256)
// keySel    : 0=Key1, 1=Key2
// rangSel   : Range Select (32 sets key - 0~31)

void BCM_maes_read_key (U32 *dest, U32 rangSel, U32 keyLenBit, U32 keySel)
{
  U32 i;
  //printf_tube(": === Media AES Load Key Function ===\n");

  // Auto Increament
  BCM_check_status( BCM_apb_write(0, SHR_CTRL, 0x1));

  // Key1
  if (keySel==0)
    {
      BCM_check_status( BCM_apb_write(0, SHR_KEY1_ACC_PTR, rangSel<<4)); // Select Range & reset pointer
      for (i=0; i<(keyLenBit/32); i++) BCM_check_status( BCM_apb_read(0, SHR_KEY1_ACC_DW, &dest[i]));
    }
  // Key2
  else
    {
      BCM_check_status( BCM_apb_write(0, SHR_KEY2_ACC_PTR, rangSel<<20)); // Select Range & reset pointer
      for (i=0; i<(keyLenBit/32); i++) BCM_check_status( BCM_apb_read(0, SHR_KEY2_ACC_DW, &dest[i]));
    }
}


//--------------------------------------------------------- Media AES Load IV
// src     : IV source address
// rangSel : Range Select (32 sets key)

void BCM_maes_load_iv (U32 *src, U32 rangSel)
{
  U32 i;
  //printf_tube(": === Media AES Load IV Function ===\n");

  BCM_check_status( BCM_apb_write(0, SHR_CTRL, 0x1));              // Auto Increament
  BCM_check_status( BCM_apb_write(0, SHR_IV_ACC_PTR, rangSel<<4)); // Set pointer
  for (i=0; i<4; i++) BCM_check_status( BCM_apb_write(0, SHR_IV_ACC_DW, src[i]));
}

//--------------------------------------------------------- Media AES Read IV
// dest    : Read IV destination address
// rangSel : Range Select (32 sets key)

void BCM_maes_read_iv (U32 *dest, U32 rangSel)
{
  U32 i;
  //printf_tube(": === Media AES Load IV Function ===\n");
  BCM_check_status( BCM_apb_write(0, SHR_CTRL, 0x1));              // Auto Increament
  BCM_check_status( BCM_apb_write(0, SHR_IV_ACC_PTR, rangSel<<4)); // Set pointer
  for (i=0; i<4; i++) BCM_check_status( BCM_apb_read(0, SHR_IV_ACC_DW, &dest[i]));
}

//--------------------------------------------------------- Media AES Load LBA
// src     : LBA source address (3 words)
//           src[0]= Starting LBA [31:00]
//           src[1]= Starting LBA [63:32]
//           src[2]= Number of blk (min=1)
// rangSel : Range Select (32 sets key)
//
// Notes:
// the starting LBA address and blkCnt defines the LBA range
// of the key, iv sets

void BCM_maes_load_lba (U32 *src, U32 rangSel)
{
  //printf_tube(": === Media AES Load LBA Function ===\n");
  BCM_check_status( BCM_apb_write(0, SHR_LBA_ACC_PTR,  rangSel<<4)); // Set pointer
  BCM_check_status( BCM_apb_write(0, SHR_LBA_ACC_DW0,  src[0]));     // Start LBA [31:00]
  BCM_check_status( BCM_apb_write(0, SHR_LBA_ACC_DW1,  src[1]));     // Start LBA [63:32]
  BCM_check_status( BCM_apb_write(0, SHR_LBA_ACC_BCNT, src[2]));     // Blk Cnt
}

//--------------------------------------------------------- Media AES Read LBA
// dest    : Read LBA destination address (3 words)
//           dest[0]= Starting LBA [31:00]
//           dest[1]= Starting LBA [63:32]
//           dest[2]= Number of blk (min=1)
// rangSel : Range Select (32 sets key)

void BCM_maes_read_lba (U32 *dest, U32 rangSel)
{
  //printf_tube(": === Media AES Read LBA Function ===\n");
  BCM_check_status( BCM_apb_write(0, SHR_LBA_ACC_PTR,  rangSel<<4)); // Set pointer
  BCM_check_status( BCM_apb_read (0, SHR_LBA_ACC_DW0,  &dest[0]));   // Start LBA [31:00]
  BCM_check_status( BCM_apb_read (0, SHR_LBA_ACC_DW1,  &dest[1]));   // Start LBA [63:32]
  BCM_check_status( BCM_apb_read (0, SHR_LBA_ACC_BCNT, &dest[2]));   // Blk Cnt
}

//--------------------------------------------------------- Media AES Load Init Table
// This function initializes the range table to the
// pre-defined value (currently only setup from range 0~3)
//
// Range  En  Bypass  LBAs  cnt  keySize  Key1&Key2   IV
//------------------------------------------------------------
//   0    1     0       0    8   128-bit  {8{32'h0}} {4{32'h0}}
//   1    1     0       8    8   192-bit  {8{32'h1}} {4{32'h1}}
//   2    1     0      16    8   256-bit  {8{32'h2}} {4{32'h2}}
//   3    1     1      24    8   128-bit  {8{32'h3}} {4{32'h3}}
//
// Key & IV value is the range number (repeat for each word)

void BCM_maes_init_table ()
{
  U32 lba[3];  // LBA information
  U32 data[8]; // Key and IV
  U32 idx, j;

#if ELDORA_SOC_VERSION_1_1
  // Range Enable (MSB=set31 ~ LSB=set0)
  BCM_check_status( BCM_apb_write(0, SHR_MIS_RNG_EN, 0x0000000f));
#elif ELDORA_SOC_VERSION_2_0_UP
  // Range Enable (MSB=set31 ~ LSB=set0)
  BCM_check_status( BCM_apb_write(0, SHR_MIS_RNG_WR_EN, 0x0000000f));
  BCM_check_status( BCM_apb_write(0, SHR_MIS_RNG_RD_EN, 0x0000000f));
  BCM_check_status( BCM_apb_write(0, SHR_MIS_RNG_VALID, 0x0000000f));
#else
    #error Undefined Eldora SOC Version!
#endif

  // Bypass (MSB=set31 ~ LSB=set0)
  BCM_check_status( BCM_apb_write(0, SHR_MIS_RNG_BYP, 0x00000008));

  // Key Size (MSB=Set15 ~ LSB=set0)
  BCM_check_status( BCM_apb_write(0, SHR_MIS_KSIZE0, 0x00000018));

  // Initial LBA array
  lba[0] = 0; // Starting LBA [31:00]
  lba[1] = 0; // Starting LBA [63:32]
  lba[2] = 8; // Number of blk (min=1)

  // Key & IV
  for (idx=0; idx<4; idx++)
    {
      printf_tube(": ------------------ Program Media AES Range %x\n", idx);
      for (j=0; j<8; j++) data[j] = idx;
      BCM_maes_load_lba (lba,  idx);
      BCM_maes_load_iv  (data, idx);
      BCM_maes_load_key (data, idx, 256, 0); // Key 1
      BCM_maes_load_key (data, idx, 256, 1); // Key 2
      lba[0]+=8;
    }
}

//--------------------------------------------------------------------------------+
//                                                                            OTP |
//--------------------------------------------------------------------------------+
// EROM must be loaded first for APB access
// AIX interface is not used. Read/Write date is done within the Mailbox

//--------------------------------------------------------- OTP Write
// addr: Write address (0~63)
// src : Write data source addr
//       src[0] = OTP LSB 32-bit (DW0)
//       src[1] = OTP MSB 32-bit (DW1)
//       src[2] = OTP security data (3-bit)
//                bit[2]   = Security bit
//                bit[1:0] = OTP test bit

void BCM_otp_write (U32 addr, U32 *src)
{
  U32 rdata;

  printf_tube(": === OTP Write Function ===\n");
  // Clear interrupt
  BCM_check_status( BCM_apb_write(1, OTP_INTERRUPT,  0x1));
  // Write Address
  BCM_check_status( BCM_apb_write(1, OTP_ROW_ADDR,   addr));
  // Write Data
  BCM_check_status( BCM_apb_write(1, OTP_LO,         src[0]));
  BCM_check_status( BCM_apb_write(1, OTP_HI,         src[1]));
  BCM_check_status( BCM_apb_write(1, OTP_SEC_TEST,   src[2]));
  // Start write
  BCM_check_status( BCM_apb_write(1, OTP_MATCH_CODE, 0x18d ));  // Match Code
  BCM_check_status( BCM_apb_write(1, OTP_CONTROL,    0x3   ));
  // Wait for interrupt
  rdata=0;
  while (rdata==0)
    {
      BCM_check_status( BCM_apb_read(1, OTP_INTERRUPT, &rdata));
    }
}

//--------------------------------------------------------- OTP Read
// addr: Read address (0~63)
// dest: Read data destination address
//       dest[0] = OTP LSB 32-bit (DW0)
//       dest[1] = OTP MSB 32-bit (DW1)
//       dest[2] = OTP security data (3-bit)
//                 bit[2]   = Security bit
//                 bit[1:0] = OTP test bit

void BCM_otp_read (U32 addr, U32 *dest)
{
  U32 rdata;

  printf_tube(": === OTP Read Function ===\n");
  // Clear interrupt
  BCM_check_status( BCM_apb_write(1, OTP_INTERRUPT, 0x1));
  // Read Address
  BCM_check_status( BCM_apb_write(1, OTP_ROW_ADDR,  addr));
  // Start read
  BCM_check_status( BCM_apb_write(1, OTP_CONTROL,   0x5));
  // Wait for interrupt
  rdata=0;
  while (rdata==0)
    {
      BCM_check_status( BCM_apb_read(1, OTP_INTERRUPT, &rdata));
    }
  // Obtain Read Data from Register
  BCM_check_status( BCM_apb_read(1, OTP_LO,       &dest[0]));
  BCM_check_status( BCM_apb_read(1, OTP_HI,       &dest[1]));
  BCM_check_status( BCM_apb_read(1, OTP_SEC_TEST, &dest[2]));
}
#endif //End of #if 0
#endif // end of BCM_EROM_SUPPORT
//--------------------------------------------------------------------------------+
//                                                                         Others |
//--------------------------------------------------------------------------------+
