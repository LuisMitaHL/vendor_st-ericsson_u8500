//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename os_secure.c
//
// $Header: X:/MKS Projects/prototype/prototype/os_abstraction/rcs/os_secure.c 1.14 2009/01/19 12:32:28Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup os_abstraction
///
/// \file
/// \brief
///      ASN.1 SSL helper routines implementation.
///
///      Access routines specifically for using the SUPL encode/decode routines
///      in a way useful for the supl implementation specifically the secure layer.
///
//*************************************************************************
#include "assert.h"
#include "os_secure.h"
#include "openssl/evp.h"
#include "openssl/sha.h"
#include "string.h"

/// Inner Pad XOR value
#define IPAD         0x36
/// Outer Pad XOR value
#define OPAD         0x5C
/// Input key length in bytes
#define HASH_KEYLEN    64
/// Output hash length in bytes
#define HASH_LEN       20

//*****************************************************************************
/// \brief
///      Function to calculate VER given the fqdn and SUPL-INIT.
///
///      For Proxy mode SUPL-POS-INIT must contain a verification field (VER) which
///      is an HMAC (Hash Message Authentication Code) which is generated as below:
///         - VER = H(H-SLP XOR OPAD, H(H-SLP XOR IPAD, SUPL INIT))
///         - H = Hash function (SHA-1).
///         - H-SLP = FQDN of the SLP.
///         - SUPL-INIT = SUPL-INIT received in the PUSH.
///         - IPAD (Inner PAD) = Array of 0x36, length of H-SLP
///         - OPAD (Outer PAD) = Array of 0x5c, length or H-SLP
///
/// \note
///      This function could be easily tidied up but is only called once per
///      NI SUPL Sequence and is much clearer left like this.
/// \returns
///      Nothing.
//*****************************************************************************
BL GN_Secure_Calc_VER
(
   unsigned char  *p_VER,        ///< [out] VER data.
   char           *p_fqdn,       ///< [in] FQDN (zero terminated string).
   unsigned char  *p_suplinit,   ///< [in] Location of SUPL-INIT PDU.
   size_t         pdu_length     ///< [in] Size of SUPL-INIT PDU.
)
{
   unsigned char inData1[1000];
   unsigned char inData2[1000];
   unsigned char hashData1[1000];
   unsigned char hashData2[1000];
   size_t loopi;
   size_t fqdn_len = strlen(p_fqdn);

   size_t derived_pdu_length = ( (U1) *p_suplinit * 0x100 ) + ((U1) *(p_suplinit + 1) );

   // This check is removed to allow the pdu to be sent in with a length/size mismatch.
   // This can then be verified as a protocol error.
   //if (derived_pdu_length != pdu_length)
   //{
   //   // The contents of the start of the PDU don't contain the length of the PDU.
   //   return FALSE;
   //}

   // Create data for inner hash
   // First part of indata contains fqdn xor'd with IPAD.
   for( loopi=0 ; loopi < fqdn_len ; loopi++ )
   {
      inData1[loopi] = p_fqdn[loopi] ^ IPAD;   
   }
   // The second part (up to HASH_KEYLEN) is set to 0 xor'd with IPAD.
   for( loopi = fqdn_len ; loopi < HASH_KEYLEN ; loopi++ )
   {
      inData1[loopi] = 0 ^ IPAD;
   }
   // Add SUPL-INIT to the end.
   for( loopi = 0 ; loopi < derived_pdu_length ; loopi++ )
   {
      inData1[HASH_KEYLEN + loopi] = p_suplinit[loopi];   
   }
   // Clear out the destination.
   for( loopi = 0 ; loopi < HASH_LEN ; loopi++ )
   {
      hashData1[loopi] = 0;
   }

   // Perform inner hash
   EVP_Digest(
      inData1,
      ( HASH_KEYLEN + derived_pdu_length ),
      hashData1,
      NULL,
      EVP_sha1(),
      NULL );

   // Create data for outer hash
   // First part of indata contains fqdn xor'd with OPAD.
   for( loopi = 0 ; loopi < fqdn_len ; loopi++ )
   {
      inData2[loopi] = p_fqdn[loopi] ^ OPAD;   
   }
   // The second part (up to HASH_KEYLEN) is set to 0 xor'd with OPAD.
   for( loopi = fqdn_len ; loopi < HASH_KEYLEN ; loopi++ )
   {
      inData2[loopi] = 0 ^ OPAD;   
   }
   // Add Initial hash to the end.
   for( loopi = 0 ; loopi < HASH_LEN ; loopi++ )
   {
      inData2[HASH_KEYLEN + loopi] = hashData1[loopi];   
   }
   // Clear out the destination.
   for( loopi = 0 ; loopi < HASH_LEN ; loopi++ )
   {
      hashData2[loopi] = 0;   
   }

   // Perform outer hash
   EVP_Digest(
      inData2,
      ( HASH_KEYLEN + HASH_LEN ),
      hashData2,
      NULL,
      EVP_sha1(),
      NULL );

   // Ver should now be the first hashData[] truncated to 64 bits.
   for( loopi = 0 ; loopi < GN_GPS_VER_LEN ; loopi++ )
   {
      p_VER[loopi] = hashData2[loopi];   
   }

   printf( "Calculated VER:" );
   for( loopi = 0 ; loopi < GN_GPS_VER_LEN ; loopi++ )
   {
      printf( "%02x", p_VER[loopi] );   
   }
   printf( "\n" );
   return TRUE;

}
