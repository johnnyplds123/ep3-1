//-----------------------------------------------------------------------------
// Title         : BCM Functions
// File          : bcm_functions.h
// Description   : Contains all the BCM function calls
//
//-----------------------------------------------------------------------------

#ifndef _BCM_FUNCTIONS_H
#define _BCM_FUNCTIONS_H


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

extern void BCM_reset(void);
extern void BCM_self_test(U32);


//--------------------------------------------------------------------------------+
//                                                                            AES |
//--------------------------------------------------------------------------------+
extern void BCM_aes_zerorize (void);

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

extern void BCM_aes_load_key (U32 *src, U32 keyLenBit, U32 keySel);
extern void BCM_aesx_load_key (U32 *src, U32 keyLenBit, U32 keySel, ENGINE_ID id);


//--------------------------------------------------------- AES Load IV
// src : IV source address
// id  : 0=Null, 2=AES, 7=AES2
//
// Notes:
// The id is used to select the AES engine (only used when
// there are 2 AES in BCM) When id=0, default AES engine
// would be used. Don't use any other value other than
// 0, 1, and 2

extern void BCM_aes_load_iv (U32 *src);
extern void BCM_aesx_load_iv (U32 *src, ENGINE_ID id);


//--------------------------------------------------------- AES Init
// direction : 0=Encrypt, 1=Decrypt
// keyLenBit : Key Length in Bit (128,192,256)
// mode      : 0=ECB, 1=CBC, 2=CTR, 3=XTS, 4=KWP

extern void BCM_aes_init (U32 direction, U32 keyLenBit, U32 mode);


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

extern void BCM_aesx_init (U32 direction,  U32 keyLenBit, U32 mode, ENGINE_ID id,
            U32 iv_mode,    U32 iv_mask,   U32 lba_hi, U32 lba_lo,
            U32 interleave, U32 endian,    U32 ctr_mod);



//--------------------------------------------------------- AES Process
// src         : Input  data source address
// dest        : Output data destination address
// dataLenByte : data length in byte
// flag        : 0=Continue on previous task, 1=New task

extern void BCM_aes_process (U32 *src, U32 *dest, U32 dataLenByte, U32 flag);


// src         : Input  data source address
// dest        : Output data destination address
// dataLenByte : data length in byte
// flag        : 0=Continue on previous task, 1=New task
// numSet      : Number of sector (0=1_sector, 1=2_sectors...etc)
// id          : 0=Null, 2=AES, 7=AES2

extern void BCM_aesx_process (U32 *src, U32 *dest, U32 dataLenByte, U32 flag, U32 numSet, ENGINE_ID id);


//--------------------------------------------------------- AES Key Gen
// dest      : Output key destination address
// keyLenBit : Key Length in Bit (128,192,256)

extern void BCM_aes_key_gen (U32 *dest, U32 keyLenBit);



//--------------------------------------------------------- AES Key Wrap
// This is an EROM key wrap function call
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

extern void BCM_aes_key_wrap (U32 *d_src, U32 *d_dest, U32 *param_src, U32 *iv_dest,
            U32 direction, U32 dataLenWord);


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

extern void BCM_aes_engine (U32 *src, U32 *dest, U32 *pKey1, U32 *pKey2, U32 *pIV,
            U32 mode, U32 dir, U32 dataLenByte, U32 keyLenBit);


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

extern void BCM_aesx_engine (U32 *src, U32 *dest, U32 *pKey1, U32 *pKey2, U32 *pIV,
            U32 mode, U32 dir, U32 dataLenByte, U32 keyLenBit,
            U32 iv_mode,    U32 iv_mask,   U32 lba_hi, U32 lba_lo,
            U32 interleave, U32 endian,    U32 numSet, U32 ctr_mod,
            ENGINE_ID id);


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

extern void BCM_aes_mre (U32 *src, U32 *dest, U32 dataLenByte, U32 numSet, U32 new_flag,
            U32 *a1_pKey1, U32 *a1_pKey2, U32 *a1_pIV, U32 a1_mode, U32 a1_keyLenBit,
            U32 *a2_pKey1, U32 *a2_pKey2, U32 *a2_pIV, U32 a2_mode, U32 a2_keyLenBit);

//--------------------------------------------------------------------------------+
//                                                                           HASH |
//--------------------------------------------------------------------------------+

//--------------------------------------------------------- HASH Init
// mode : 0=SHA1, 1=SHA224, 2=SHA256, 3=MD5
// flag : 0=Digest stays in register, 1=Output to other engines

extern void BCM_hash_init (U32 mode, U32 flag);

//--------------------------------------------------------- HASH Update
// src         : Input data source address
// dataLenByte : Input data length in byte
extern void BCM_hash_update (U32 *src, U32 dataLenByte);

//--------------------------------------------------------- HASH Final
// src        : Input data source address
// dest       : Output digest destination address
// segLenByte : segment length in byte
// pad        : 0=FW padding, 1=HW padding

extern void BCM_hash_final (U32 *src, U32 *dest, U32 segLenByte, U32 pad);

//--------------------------------------------------------- HASH Zerorize
extern void BCM_hash_zerorize(void);


//--------------------------------------------------------- HAES Engine Call
// src         : Message source address
// dest        : Digest destination address
// mode        : 0=SHA1, 1=SHA224, 2=SHA256, 3=MD5
// dataLenByte : Message length in byte
// bytePerOp   : Num of byte hash process for each hash update

extern void BCM_hash_engine (U32 *src, U32 *dest, U32 mode, U32 dataLenByte, U32 bytePerOp);

//--------------------------------------------------------------------------------+
//                                                                           HMAC |
//--------------------------------------------------------------------------------+

//--------------------------------------------------------- HMAC Load Key
// src        : Key source address
// keyLenByte : Key Length in Byte

extern void BCM_hmac_load_key (U32 *src, U32 keyLenByte);

//--------------------------------------------------------- HMAC Init
// mode : 0=SHA1, 1=SHA224, 2=SHA256, 3=MD5
// flag : 0=Digest stays in register, 1=Output to other engines

extern void BCM_hmac_init (U32 mode, U32 flag);

//--------------------------------------------------------- HMAC Update
// src         : Input data source address
// dataLenByte : Input data length in byte

extern void BCM_hmac_update (U32 *src, U32 dataLenByte);

//--------------------------------------------------------- HMAC Final
// src        : Input data source address
// dest       : Output digest destination address
// segLenByte : segment length in byte
// pad        : 0=FW padding, 1=HW padding

extern void BCM_hmac_final (U32 *src, U32 *dest, U32 segLenByte, U32 pad);

//--------------------------------------------------------- HASH Zerorize
extern void BCM_hmac_zerorize(void);

//--------------------------------------------------------- HMAC Key Gen
// dest       : Output key destination address
// keyLenByte : Key Length in Byte

extern void BCM_hmac_key_gen (U32 *dest, U32 keyLenByte);

//--------------------------------------------------------- HMAC Engine Call
// src         : Message source address
// dest        : Digest destination address
// pKey        : Key source address
// mode        : 0=SHA1, 1=SHA224, 2=SHA256, 3=MD5
// dataLenByte : Message length in byte
// bytePerOp   : Num of byte hash process for each hash update
// keyLenByte  : Key Length in Byte

extern void BCM_hmac_engine (U32 *src, U32 *dest, U32 *pKey,
            U32 mode, U32 dataLenByte, U32 bytePerOp, U32 keyLenByte);

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

extern STATUS BCM_pkcs_v15_verify (U32 *pMsg, U32 *pMod, U32 *pExp, U32 *pSig,
              U32 hashMode, U32 dataLenByte, U32 keyLenBit);

//--------------------------------------------------------------------------------+
//                                                                          ECDSA |
//--------------------------------------------------------------------------------+


//--------------------------------------------------------------------------------+
//                                                                      Media AES |
//--------------------------------------------------------------------------------+
// EROM must be loaded first for APB access

//--------------------------------------------------------- Media AES Load Key
// src       : Key source address
// keyLenBit : Key Length in Bit (128,192,256)
// keySel    : 0=Key1, 1=Key2
// rangSel   : Range Select (32 sets key - 0~31)

extern void BCM_maes_load_key (U32 *src, U32 rangSel, U32 keyLenBit, U32 keySel);

//--------------------------------------------------------- Media AES Read Key
// dest      : read Key destination address
// keyLenBit : Key Length in Bit (128,192,256)
// keySel    : 0=Key1, 1=Key2
// rangSel   : Range Select (32 sets key - 0~31)

extern void BCM_maes_read_key (U32 *dest, U32 rangSel, U32 keyLenBit, U32 keySel);

//--------------------------------------------------------- Media AES Load IV
// src     : IV source address
// rangSel : Range Select (32 sets key)

extern void BCM_maes_load_iv (U32 *src, U32 rangSel);

//--------------------------------------------------------- Media AES Read IV
// dest    : Read IV destination address
// rangSel : Range Select (32 sets key)

extern void BCM_maes_read_iv (U32 *dest, U32 rangSel);

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

extern void BCM_maes_load_lba (U32 *src, U32 rangSel);

//--------------------------------------------------------- Media AES Read LBA
// dest    : Read LBA destination address (3 words)
//           dest[0]= Starting LBA [31:00]
//           dest[1]= Starting LBA [63:32]
//           dest[2]= Number of blk (min=1)
// rangSel : Range Select (32 sets key)

extern void BCM_maes_read_lba (U32 *dest, U32 rangSel);

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

extern void BCM_maes_init_table(void);

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

extern void BCM_otp_write (U32 addr, U32 *src);

//--------------------------------------------------------- OTP Read
// addr: Read address (0~63)
// dest: Read data destination address
//       dest[0] = OTP LSB 32-bit (DW0)
//       dest[1] = OTP MSB 32-bit (DW1)
//       dest[2] = OTP security data (3-bit)
//                 bit[2]   = Security bit
//                 bit[1:0] = OTP test bit

extern void BCM_otp_read (U32 addr, U32 *dest);

//--------------------------------------------------------------------------------+
//                                                                         Others |
//--------------------------------------------------------------------------------+


#endif
