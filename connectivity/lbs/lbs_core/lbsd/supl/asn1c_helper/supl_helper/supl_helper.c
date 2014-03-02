//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
//
// Filename supl_helper.c
//
// $Header: X:/MKS Projects/prototype/prototype/asn1_telelogic/supl_helper/rcs/supl_helper.c 1.82 2009/01/16 14:48:42Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup supl_helper
///
/// \file
/// \brief
///      ASN.1 helper routines implementation.
///
///      Access routines specifically for using the SUPL encode/decode routines
///      in a way useful for the supl_subsystem.
///
//*************************************************************************

#include "gps_ptypes.h"
#include "os_assert.h"
#include "supl_log.h"
#include "supl_helper.h"
//#include "SUPL_asn1coder.h"
//#include "SUPL.h"
//#include "ULP_PDU_asn1.h"
#include "supl_config.h"
#include "pdu_defs.h"
#include "GAD_Conversions.h"
//#include "coder_fiostub.h"
//#include "possupl_asn1.h"
#include "SUPL_ULP_NavigationModel.h"
#include "SUPL_ULP_SessionID.h"
#include "SUPL_ULP_ULP-PDU.h"
#include "SUPL_ULP_SETId.h"
#include "SUPL_ULP_SLPAddress.h"
#include "SUPL_ULP_Status.h"
#include "supl_asn1_macros.h"
#include "SUPL_ULP_StatusCode.h"
#include "supl_asn1_types.h"
#include "supl_asn1.h"
#include "supl_hmac.h"
#include "time.h"

extern s_GN_SUPL_V2_Multiple_Loc_ID *SUPL_Mlid_FirstElem;

void supl_get_SetCapabilities( s_SUPL_Instance *p_SUPL_Instance,   s_GN_SetCapabilities *p_SetCapabilities ); 

#define MSISDN_OCTET_LENGTH      8 ///< From ASN.1 definition of SETId: msisdn     OCTET STRING(SIZE (8))
#define IMSI_OCTET_LENGTH        8 ///< From ASN.1 definition of SETId: imsi       OCTET STRING(SIZE (8))
#define MDN_OCTET_LENGTH         8 ///< From ASN.1 definition of SETId: mdn        OCTET STRING(SIZE (8))
#define MIN_BITSTRING_LENGTH    34 ///< From ASN.1 definition of SETId: min        BIT STRING(SIZE (34)), -- coded according to TIA-553
#define privacyOverride 4

#define MaxLocIdSize 64

#define SDL_True  1
#define SDL_False 0

//Status
#define current Status_current
#define stale Status_stale
#define unknown Status_unknown
//Posmethods
#define agpsSETassisted1 PosMethod_agpsSETassisted
#define agpsSETbased1 PosMethod_agpsSETbased
#define agpsSETassistedpref1 PosMethod_agpsSETassistedpref
#define agpsSETbasedpref1 PosMethod_agpsSETbasedpref
#define autonomousGPS1 PosMethod_autonomousGPS
#define eCID1 PosMethod_eCID
#define aFLT1 PosMethod_aFLT
#define eOTD1 PosMethod_eOTD
#define oTDOA1 PosMethod_oTDOA
#define noPosition1 PosMethod_noPosition

//Status Code

#define unspecified StatusCode_unspecified
#define systemFailure StatusCode_systemFailure
#define unexpectedMessage StatusCode_unexpectedMessage
#define protocolError StatusCode_protocolError
#define dataMissing StatusCode_dataMissing
#define unexpectedDataValue StatusCode_unexpectedDataValue
#define posMethodFailure StatusCode_posMethodFailure
#define posMethodMismatch StatusCode_posMethodMismatch
#define posProtocolMismatch StatusCode_posProtocolMismatch
#define targetSETnotReachable StatusCode_targetSETnotReachable
#define versionNotSupported StatusCode_versionNotSupported
#define resourceShortage StatusCode_resourceShortage
#define invalidSessionId StatusCode_invalidSessionId
#define nonProxyModeNotSupported StatusCode_nonProxyModeNotSupported
#define proxyModeNotSupported StatusCode_proxyModeNotSupported
#define positioningNotPermitted StatusCode_positioningNotPermitted
#define authNetFailure StatusCode_authNetFailure
#define authSuplinitFailure StatusCode_authSuplinitFailure
#define consentDeniedByUser StatusCode_consentDeniedByUser
#define consentGrantedByUser StatusCode_consentGrantedByUser

#define ver2_sessionStopped StatusCode_ver2_sessionStopped

#define PDU_WIDTH (32)

//*****************************************************************************
/// \brief
///      Function for logging a raw PDU Buffer Store.
//*****************************************************************************

void supl_PDU_Buffer_Store_Log
(
   CH                   *Log_Prefix,         ///< Prefix of log message.
   s_PDU_Buffer_Store   *p_PDU_Buffer_Store  ///< Address of the PDU_Buffer_Store pointer to free.
)
{
   if ( p_PDU_Buffer_Store != NULL )
   {
      if ( p_PDU_Buffer_Store->PDU_Encoded.p_PDU_Data != NULL &&
           p_PDU_Buffer_Store->PDU_Encoded.Length     != 0       )
      {
         CH* PDU_String ;
         CH* String_Ptr ;
         U2  i ;
         U2 j = 1, Count =0;
         U2 tot = p_PDU_Buffer_Store->PDU_Encoded.Length / PDU_WIDTH;

         if (p_PDU_Buffer_Store->PDU_Encoded.Length % PDU_WIDTH)
         {
              tot++;
         }

         /// Allocate 3 characters to display each of the bytes and a space plus one string terminator.
         PDU_String = GN_Calloc( 1, PDU_WIDTH * 3 + 1 ) ;

         String_Ptr = PDU_String ;

         GN_SUPL_Log("%s:PDU_Buffer_Store Length: %5u, Data:", Log_Prefix, p_PDU_Buffer_Store->PDU_Encoded.Length);
         

         /// Loop around for each character and make a printable string of the buffer contents.
         for ( i = 0 ; i < p_PDU_Buffer_Store->PDU_Encoded.Length ; i++ )
         {
            Count += sprintf( String_Ptr + Count, " %02x", p_PDU_Buffer_Store->PDU_Encoded.p_PDU_Data[i] );

            if ((i % PDU_WIDTH) == PDU_WIDTH - 1)
            {
               GN_SUPL_Log("%s:Data %u/%u:%s", Log_Prefix, j, tot, String_Ptr);
               j++;
               Count = 0;
            }
         }
         if(Count)
         {
             GN_SUPL_Log("Left part of encoded Buffer:%s",  String_Ptr);
         }
         


         GN_Free( PDU_String );
      }
      else
      {
         GN_SUPL_Log( "%s:PDU_Buffer_Store Empty", Log_Prefix );
      }
   }
   else
   {
      GN_SUPL_Log( "%s:PDU_Buffer_Store Invalid", Log_Prefix );
   }
}

BL supl_Is_Version2( s_SUPL_Instance  *p_SUPL_Instance  ) 
{
   U1 Config_Major;
   BL IsSupported = FALSE;
   s_SUPL_InstanceData  *p_SUPL = p_SUPL_Instance->p_SUPL; // SUPL specific data.

   supl_config_get_SUPL_version( &Config_Major , NULL, NULL );

   // Check to see if compiler setting allows for SUPL 2.0
   if( Config_Major == 2 )
   {
      // In NI cases, we check for Transaction SUPL version which was populated while handling SUPL INIT message.
      if(p_SUPL->NetworkInitiated) 
      {
         if( p_SUPL->TransactionSuplVersion == 2 )
         {
            IsSupported = TRUE;
         }
      }
      // IN SI cases, SET sends the supported version as configured.
      else
      {
         IsSupported = TRUE;
      }
   }

   return IsSupported;
}


//*****************************************************************************
/// \brief
///      Encodes a SUPL ULP structure prepared for the ASN.1 compiler into a
///      #s_PDU_Buffer_Store
///
/// \returns
///      #TRUE if successful.
///      #FALSE if unsuccessful.
//*****************************************************************************
BL supl_PDU_Encode
(
   void                    *handle ,   ///< [in] handle of SUPL instance requesting encode
   void                 *p_PDU_Src, ///< [in] Pointer to generated SUPL structure
                                    ///   ULP_PDU_t. Passed in as (void *) to
                                    ///   hide implementation.
   s_PDU_Buffer_Store   *p_PDU_Buf  ///< [out] Store containing encoded PDU
)
{
   uint8_t* encodedASN1 =NULL;        /*  Data stream to hold the encoded message */
   uint32_t encodedStringLength = 0; /*  To hold the length of the encoded message */


   ULP_PDU_t  *p_CastPDU = p_PDU_Src;

   GN_SUPL_Log_SuplMessageEncoded( handle , p_CastPDU );

   if ( p_CastPDU->message.present == UlpMessage_PR_msSUPLEND )
   {

          if ( p_CastPDU->message.choice.msSUPLEND.statusCode )
        {
            GN_SUPL_Log_SuplEndStatus( handle , supl_PDU_Get_SUPLEND_StatusCode(p_CastPDU ) );
        }
   }

   // asn1_init_encode_buffer() is used to initialize the static encode buffer
   // required for encoding any message   into ASN.1, has to be called before encoding.
   //asn1_init_encode_buffer();

    SUPL_encode_message(p_CastPDU,&encodedASN1,&encodedStringLength);

   if ( encodedStringLength > 0 )
   {
      p_PDU_Buf->PDU_Encoded.p_PDU_Data = (U1*) encodedASN1;
      p_PDU_Buf->PDU_Encoded.Length     = (U2)  encodedStringLength;

      // To save reencoding the PDU, use the fact that PER encoded PDUs have the
      // length as the first two octets and insert the length directly.
      p_PDU_Buf->PDU_Encoded.p_PDU_Data[0] = (U1)( ( encodedStringLength >> 8 ) & 0xff );
      p_PDU_Buf->PDU_Encoded.p_PDU_Data[1] = (U1)( ( encodedStringLength      ) & 0xff );

      supl_PDU_Buffer_Store_Log( "supl_PDU_Encode", p_PDU_Buf );

      return TRUE;
   }
   else
   {
      GN_SUPL_Log( "GN_SUPL_asn1_encode: Failed ");
      return FALSE;
   }
}


//*****************************************************************************
/// \brief
///      Decodes a SUPL ULP structure prepared for the ASN.1 compiler from a
///      #s_PDU_Buffer_Store
///
/// \returns
///      #TRUE if decode was successful.
///      #FALSE if decode was unsuccessful.
//*****************************************************************************
BL supl_PDU_Decode
(
   void                    *handle ,          ///< [in] handle of SUPL instance requesting decode
   s_PDU_Buffer_Store   *p_PDU_Buf,       ///< [in] Store containing encoded PDU
   void                 **p_p_PDU_Dest,   ///< [out] Pointer to generated SUPL ULP
                                          ///   structure pointer ULP_PDU_t.
                                          ///   Passed in as (void **) to hide
                                          ///   implementation.
   e_GN_StatusCode      *p_GN_StatusCode  ///< [out] Status of the decode
)
{
   uint8_t *encodedASN1;          /*  Data stream to hold the encoded message */
   uint32_t encodedStringLength; /*  To hold the length of the encoded message */
   uint32_t retValue;            /*  To hold the return value indicating success / failure of encode / decode operation  */

   ULP_PDU_t **p_p_CastPDU = (ULP_PDU_t**) p_p_PDU_Dest;
   ULP_PDU_t *p_CastPDU = NULL;

   //*p_p_CastPDU = GN_Calloc( 1, sizeof( ULP_PDU_t ) );

   encodedASN1 = (uint8_t *) p_PDU_Buf->PDU_Encoded.p_PDU_Data;
   encodedStringLength = (uint32_t) p_PDU_Buf->PDU_Encoded.Length;

   supl_PDU_Buffer_Store_Log( "supl_PDU_Decode", p_PDU_Buf );

   retValue = SUPL_decode_message(encodedStringLength,encodedASN1,&p_CastPDU);

   ASN1_SET(p_p_CastPDU) = p_CastPDU;

   if ( retValue )
   {
      #ifdef PDU_LOGGING
         //#if PDU_LOGGING == 'VERB'
           // ASN1Print_ULP_PDU( stdout, *p_p_CastPDU );
         //#endif
      #endif

      GN_SUPL_Log_SuplMessageDecoded( handle , *p_p_CastPDU );

      if ( (*p_p_CastPDU)->message.present == UlpMessage_PR_msSUPLEND )
      {
         if ( (*p_p_CastPDU)->message.choice.msSUPLEND.statusCode)
         {
             GN_SUPL_Log_SuplEndStatus( handle , supl_PDU_Get_SUPLEND_StatusCode(*p_p_CastPDU) );
         }
      }
      // Check length
      if ( p_PDU_Buf->PDU_Encoded.Length == (*p_p_CastPDU)->length )
      {
         *p_GN_StatusCode = GN_StatusCode_NONE;
         return TRUE;
      }
      else
      {
         *p_GN_StatusCode = GN_StatusCode_protocolError;
         return FALSE;
      }
   }
   else
   {
      return FALSE;
   }
}




//*****************************************************************************
/// \brief
///      Allocates storage for ULP_PDU_t in a way that hides the implementation.
///
/// \returns
///      Pointer to storage for ULP_PDU_t.
//*****************************************************************************
void *supl_PDU_Alloc( void )
{
   ULP_PDU_t *p_TempPDU;

   p_TempPDU = GN_Calloc( 1, sizeof( ULP_PDU_t ) ); /* not malloc! */
   if( !p_TempPDU ) {
      GN_SUPL_Log( "GN_Calloc() failed" );
      return NULL;
   }
   return ( (void*) p_TempPDU );
}


//*****************************************************************************
/// \brief
///      Frees storage for ULP_PDU_t in a way that hides the implementation.
///
/// \returns
///      nothing.
//*****************************************************************************
void supl_PDU_Free
(
   void *p_PDU ///< [in] Pointer to storage for ULP_PDU_t.
)
{
   ULP_PDU_t *p_TempPDU = (ULP_PDU_t *)p_PDU;

   if ( p_TempPDU != NULL )
   {
      SUPL_free_pdu( p_TempPDU ); //need to be check-raghu
   }

}


//*****************************************************************************
/// \brief
///      Add a bitstring in an array of U1 to a SDL_BitString type.
///
/// \details
///      Bit strings in ASN.1 are listed with the most significant bits first with
///      no padding. By convention it is easier to have the least significant bit
///      in the lowest store and ignore the upper bits.
///      this function takes a pointer to an array of U1 with the first element
///      holding the most significant bit of the bitstring with the unused bits
///      in the upper part of the U1.
///      bit 0 of the bitstring is in the highest index in the least significant
///      bit position.
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SDL_BitString_U1_ptr
(
   SDL_Bit_String *p_SDL_Bit_String,   ///< [out] Pointer to Telelogic ASN.1 bit string type.
   U1 *p_Value,                        ///< [in] Bits held in an array of U1, least significant bit in b0 of index 0.
   U2 NoOfBits                         ///< [in] Number of bits to encode.
)
{
   U1 SizeOfValue = ( ( NoOfBits - 1 ) / 8 ) + 1; // Calculate the size of the data from the number of bits it contains.
   U1 UnusedBits = ( SizeOfValue * 8 ) - NoOfBits; // Calculate how many of the bit in the most significant byte to skip.

   p_SDL_Bit_String->size = SizeOfValue;

   p_SDL_Bit_String->bits_unused =  UnusedBits;

   p_SDL_Bit_String->buf = GN_Calloc(SizeOfValue, sizeof(uint8_t));

   memcpy( p_SDL_Bit_String->buf , p_Value , SizeOfValue );
}


//*****************************************************************************
/// \brief
///      Adds length to the unencoded SUPL PDU ULP_PDU_t.
///
/// \returns
///      nothing.
//*****************************************************************************
void supl_PDU_Add_Length
(
   void *p_ThisPDU,  ///< [out] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   U2 Length         ///< [in] Length of PDU after encoding.
)
{
    ULP_PDU_t *p_CastPDU = p_ThisPDU;

    // Need to calculate the length of encoded pdu later reinsert and reencode
    p_CastPDU->length = Length;
}


//*****************************************************************************
/// \brief
///      Adds Version to the unencoded SUPL PDU ULP_PDU_t.
///
///      The version is obtained from the os configuration.
///
/// \returns
///      nothing.
//*****************************************************************************
void supl_PDU_Add_Version
(
   void *p_ThisPDU ,  ///< [out] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
    s_SUPL_Instance *p_SUPL_Instance    // for version check             
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;
   U1 Major, Minor, Service_Indicator;

   supl_config_get_SUPL_version( &Major, &Minor, &Service_Indicator );

   // -- protocol version expressed as x.y.z (e.g., 5.1.0)
   // Version ::= SEQUENCE {
   //    maj      INTEGER(0..255),
   //    min      INTEGER(0..255),
   //    servind  INTEGER(0..255)}

   if( Major == 1)
   {
       p_PDU_Cast->version.maj = Major;
   }
   else
   {
      if( supl_Is_Version2(p_SUPL_Instance) )
      {
         p_PDU_Cast->version.maj = 2;
      }
      else
      {
         p_PDU_Cast->version.maj = 1;
      }
   }

   p_PDU_Cast->version.min                = Minor;
   p_PDU_Cast->version.servind            = Service_Indicator;
}


//*****************************************************************************
/// \brief
///      Gets Version from the decoded SUPL PDU ULP_PDU_t.
///
///      The version is obtained from the os configuration.
///
/// \returns
///      nothing.
//*****************************************************************************
void supl_PDU_Get_Version
(
   void  *p_ThisPDU,          ///< [out] Pointer to unencoded structure
                              ///   ULP_PDU_t. Passed in as (void *) to hide
                              ///   implementation.
   U1    *p_Major,            ///< [out] Major version number of SUPL Spec.
   U1    *p_Minor,            ///< [out] Minor version number.
   U1    *p_Service_Indicator ///< [out] Service indicator.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   // -- protocol version expressed as x.y.z (e.g., 5.1.0)
   // Version ::= SEQUENCE {
   //    maj      INTEGER(0..255),
   //    min      INTEGER(0..255),
   //    servind  INTEGER(0..255)}

   *p_Major             = (U1)p_PDU_Cast->version.maj;
   *p_Minor             = (U1)p_PDU_Cast->version.min;
   *p_Service_Indicator = (U1)p_PDU_Cast->version.servind;
}


static void supl_PDU_Add_SetSessionIdLocal(
   SetSessionID_t **p_p_SetSessionID ,
   U2       sessionID,  ///< [in] Session ID.
   s_SetId  *p_SetId )
{
   SetSessionID_t *p_SetSessionID;

   // SetSessionID ::= SEQUENCE {sessionId  INTEGER(0..65535),
      //                           setId      SETId}

      // SETId ::= CHOICE {
      //    msisdn     OCTET STRING(SIZE (8)),
      //    mdn        OCTET STRING(SIZE (8)),
      //    min        BIT STRING(SIZE (34)), -- coded according to TIA-553
      //    imsi       OCTET STRING(SIZE (8)),
      //    nai        IA5String(SIZE (1..1000)),
      //    iPAddress  IPAddress,
      //    ...}

      // -- msisdn, mdn and imsi are a BCD (Binary Coded Decimal) string
      // -- represent digits from 0 through 9,
      // -- two digits per octet, each digit encoded 0000 to 1001 (0 to 9)
      // -- bits 8765 of octet n encoding digit 2n
      // -- bits 4321 of octet n encoding digit 2(n-1) +1
      // -- not used digits in the string shall be filled with 1111

      //p_PDU_Cast->sessionID.setSessionIDPresent = SDL_True;             /* Should be present for SUPL_START */

      //initialise set session id
     //ASN1_PARAM_3_INIT(p_PDU_Cast->sessionID.setSessionID, sessionId);
     //ASN1_PARAM_ALLOC(p_PDU_Cast->sessionID.setSessionID, SetSessionID_t);

      ASN1_SET(p_p_SetSessionID) = (SetSessionID_t*)GN_Calloc( 1, sizeof(SetSessionID_t) );
      p_SetSessionID = ASN1_GET(p_p_SetSessionID);
      p_SetSessionID->sessionId = sessionID;

      switch ( p_SetId->type )
      {
      case GN_SETId_PR_msisdn:
         p_SetSessionID->setId.present = SETId_PR_msisdn;
         p_SetSessionID->setId.choice.msisdn.size = MSISDN_OCTET_LENGTH;
         p_SetSessionID->setId.choice.msisdn.buf = GN_Calloc( 1, MSISDN_OCTET_LENGTH );
         memcpy(  p_SetSessionID->setId.choice.msisdn.buf,
                  p_SetId->u.msisdn,
                  p_SetSessionID->setId.choice.msisdn.size);
         break;
      case GN_SETId_PR_mdn:
         p_SetSessionID->setId.present           = SETId_PR_mdn;
         p_SetSessionID->setId.choice.mdn.size       = MDN_OCTET_LENGTH;
         p_SetSessionID->setId.choice.mdn.buf         = GN_Calloc( 1, MDN_OCTET_LENGTH );
         memcpy(  p_SetSessionID->setId.choice.mdn.buf,
                  p_SetId->u.mdn,
                  p_SetSessionID->setId.choice.mdn.size );
         break;
      case GN_SETId_PR_min:
         p_SetSessionID->setId.present           = SETId_PR_min;
         supl_PDU_Add_SDL_BitString_U1_ptr(
            &p_SetSessionID->setId.choice.min,
            p_SetId->u.min,
            MIN_BITSTRING_LENGTH );

         break;
      case GN_SETId_PR_imsi:
         p_SetSessionID->setId.present             = SETId_PR_imsi;
         /// ULP-Components ASN.1  imsi     OCTET STRING(SIZE (8)),
         p_SetSessionID->setId.choice.imsi.size     = IMSI_OCTET_LENGTH;
         p_SetSessionID->setId.choice.imsi.buf      = GN_Calloc( 1, IMSI_OCTET_LENGTH );   /* not malloc! */
         memcpy(  p_SetSessionID->setId.choice.imsi.buf,
                  p_SetId->u.imsi,
                  p_SetSessionID->setId.choice.imsi.size );
         break;
      case GN_SETId_PR_nai:
         p_SetSessionID->setId.present            = SETId_PR_nai;
         p_SetSessionID->setId.choice.nai.buf              = GN_Calloc( 1, strlen( (CH*) p_SetId->u.nai ) + 2 );
         p_SetSessionID->setId.choice.nai.buf[0] = 'V';
         strcpy( (CH*)&p_SetSessionID->setId.choice.nai.buf[1], (CH*) p_SetId->u.nai );
         break;
      case GN_SETId_PR_iPAddressV4:
         //   ipv4Address  OCTET STRING(SIZE (4)),
         p_SetSessionID->setId.present                                 = SETId_PR_iPAddress;
         p_SetSessionID->setId.choice.iPAddress.present                    = IPAddress_PR_ipv4Address;
         p_SetSessionID->setId.choice.iPAddress.choice.ipv4Address.size       = 4;
         p_SetSessionID->setId.choice.iPAddress.choice.ipv4Address.buf         = GN_Calloc( 1, 4 );//GN_Calloc( 1, 4 );
        memcpy( p_SetSessionID->setId.choice.iPAddress.choice.ipv4Address.buf,
                 p_SetId->u.iPAddress.ipv4Address,
                 p_SetSessionID->setId.choice.iPAddress.choice.ipv4Address.size );
         break;
      case GN_SETId_PR_iPAddressV6:
         //   ipv6Address  OCTET STRING(SIZE (16))}
         p_SetSessionID->setId.present                                    = SETId_PR_iPAddress;
         p_SetSessionID->setId.choice.iPAddress.present                   = IPAddress_PR_ipv6Address;
         p_SetSessionID->setId.choice.iPAddress.choice.ipv6Address.size   = 16;
         p_SetSessionID->setId.choice.iPAddress.choice.ipv6Address.buf    = GN_Calloc( 1, 16 );//GN_Calloc( 1, 16 );
         memcpy(p_SetSessionID->setId.choice.iPAddress.choice.ipv6Address.buf,
                 p_SetId->u.iPAddress.ipv6Address,
                 p_SetSessionID->setId.choice.iPAddress.choice.ipv6Address.size );
         break;
      case GN_SETId_PR_NOTHING: /* No components present */
      default:
         GN_SUPL_Log( "unsupported set id type" );
         break;
      }

}


//*****************************************************************************
/// \brief
///      Adds SET Session ID to the unencoded SUPL PDU ULP_PDU_t.
///
/// \returns
///      nothing.
//*****************************************************************************
void supl_PDU_Add_setSessionID
(
   void     *p_ThisPDU, ///< [out] Pointer to unencoded structure
                        ///   ULP_PDU_t. Passed in as (void *) to hide
                        ///   implementation.
   U2       sessionID,  ///< [in] Session ID.
   s_SetId  *p_SetId    ///< [in] Set ID.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   supl_PDU_Add_SetSessionIdLocal( &p_PDU_Cast->sessionID.setSessionID , sessionID , p_SetId );
}



//*****************************************************************************
/// \brief
///      Get SET Session ID from the unencoded SUPL PDU ULP_PDU_t.
///
/// \returns
///      nothing.
//*****************************************************************************
BL supl_PDU_Get_setSessionID
(
   void     *p_ThisPDU,    ///< [in] Pointer to unencoded structure
                           ///   ULP_PDU_t. Passed in as (void *) to hide
                           ///   implementation.
   U2       *p_sessionID,  ///< [out] Session ID.
   s_SetId  *p_SetId       ///< [out] Set ID.
)
{
  ULP_PDU_t *p_PDU_Cast = p_ThisPDU;
  BL Result = TRUE;

  if ( p_PDU_Cast->sessionID.setSessionID != NULL)
   {
      *p_sessionID = (U2) p_PDU_Cast->sessionID.setSessionID->sessionId;

      switch ( p_PDU_Cast->sessionID.setSessionID->setId.present )
      {
      case SETId_PR_msisdn:
         p_SetId->type = GN_SETId_PR_msisdn;
         memcpy(  p_SetId->u.msisdn,
                  p_PDU_Cast->sessionID.setSessionID->setId.choice.msisdn.buf,
                  MSISDN_OCTET_LENGTH );
         break;

      case SETId_PR_mdn:
      case SETId_PR_min:
      case SETId_PR_imsi:
         p_SetId->type = GN_SETId_PR_imsi;
         memcpy(  p_SetId->u.imsi,
                  p_PDU_Cast->sessionID.setSessionID->setId.choice.imsi.buf,
                  IMSI_OCTET_LENGTH );
         break;
       /* The below 2 cases are to be handled because in case of erroneous SET Session ID, SUPL END needs to be containing the same ID */
      case SETId_PR_nai:
         p_SetId->type = GN_SETId_PR_nai;
         memcpy(  p_SetId->u.nai,
                  p_PDU_Cast->sessionID.setSessionID->setId.choice.nai.buf,
                  p_PDU_Cast->sessionID.setSessionID->setId.choice.nai.size);
         break;
      case SETId_PR_iPAddress:

         switch ( p_PDU_Cast->sessionID.setSessionID->setId.choice.iPAddress.present)
         {
            case IPAddress_PR_ipv4Address:
             p_SetId->type = GN_SETId_PR_iPAddressV4;
             memcpy(  p_SetId->u.iPAddress.ipv4Address,
                    p_PDU_Cast->sessionID.setSessionID->setId.choice.iPAddress.choice.ipv4Address.buf,
                      4 );
             break;
            case IPAddress_PR_ipv6Address:
             p_SetId->type = GN_SETId_PR_iPAddressV6;
             memcpy(  p_SetId->u.iPAddress.ipv6Address,
                     p_PDU_Cast->sessionID.setSessionID->setId.choice.iPAddress.choice.ipv6Address.buf,
                      16 );
             break;
            default:
             GN_SUPL_Log( "unsupported set id type : IP Address type" );
             Result = FALSE;
             break;
         }
         break;
      default:
         GN_SUPL_Log( "unsupported set id type" );
         Result = FALSE;
         break;
      }
   }
   else
   {
       Result = FALSE;
   }

   return Result;
}


//*****************************************************************************
/// \brief
///      Validates SET Session ID in the unencoded SUPL PDU ULP_PDU_t.
///
/// \returns
///      TRUE if SET Session matches.
//*****************************************************************************
BL supl_PDU_Validate_setSessionID
(
   void *p_ThisPDU,  ///< [in] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   U2 sessionID,     ///< [in] Session ID.
   s_SetId *p_SetId  ///< [in] Set ID.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   if ( p_PDU_Cast->sessionID.setSessionID == NULL)
      return FALSE;

   // setSessionID defined in ULP-Components.asn1
   if ( p_PDU_Cast->sessionID.setSessionID->sessionId != sessionID )
      return FALSE;

   switch ( p_SetId->type )
   {
   case GN_SETId_PR_NOTHING:             /* No components present */
      GN_SUPL_Log( "Invalid set id type!" );
      return FALSE;
      break;
   case GN_SETId_PR_msisdn:
       if ( p_PDU_Cast->sessionID.setSessionID->setId.present != SETId_PR_msisdn )
         return FALSE;
       if ( memcmp( p_PDU_Cast->sessionID.setSessionID->setId.choice.msisdn.buf,
             p_SetId->u.msisdn, MSISDN_OCTET_LENGTH ) != 0 )
         return FALSE;
      break;
   case GN_SETId_PR_mdn:
      GN_SUPL_Log( "Unsupported set id type: mdn" );
      return FALSE;
      break;
   case GN_SETId_PR_min:
      GN_SUPL_Log( "Unsupported set id type: min" );
      return FALSE;
      break;
   case GN_SETId_PR_imsi:
      if ( p_PDU_Cast->sessionID.setSessionID->setId.present != SETId_PR_imsi )
         return FALSE;
      if ( memcmp( p_PDU_Cast->sessionID.setSessionID->setId.choice.imsi.buf,
             p_SetId->u.imsi, IMSI_OCTET_LENGTH ) != 0 )
         return FALSE;
      break;
   case GN_SETId_PR_nai:
      GN_SUPL_Log( "Unsupported set id type: nai" );
      return FALSE;
      break;
   case GN_SETId_PR_iPAddressV4:
      GN_SUPL_Log( "Unsupported set id type: iPAddressV4" );
      return FALSE;
      break;
   case GN_SETId_PR_iPAddressV6:
      GN_SUPL_Log( "Unsupported set id type: iPAddressV6" );
      return FALSE;
      break;
   default:
      GN_SUPL_Log( "Invalid set id type!" );
      return FALSE;
      break;
   }
   return TRUE;
}


//*****************************************************************************
/// \brief
///      Adds SLP Session ID to the unencoded SUPL PDU ULP_PDU_t.
///
/// \returns
///      nothing.
//*****************************************************************************
void supl_PDU_Add_slpSessionIDLocal
(
   SlpSessionID_t **p_p_SlpSessionID,
   U1                *p_sessionID,  ///< [in] Session ID.
   s_GN_SLPAddress   *p_SlpId       ///< [in] Slp ID.
)
{
   SlpSessionID_t *p_SlpSessionID;
   if ( p_sessionID != NULL )
   {
       /* Alocate the memory only if slp session id is present */

   ASN1_SET(p_p_SlpSessionID) = (SlpSessionID_t*)GN_Calloc( 1, sizeof(SlpSessionID_t) );
   p_SlpSessionID = ASN1_GET(p_p_SlpSessionID);


      // ULP-Components ASN.1 SlpSessionID ::= SEQUENCE {
      //                         sessionID  OCTET STRING(SIZE (4)),
      p_SlpSessionID->sessionID.buf        = GN_Calloc( 1, 4 );
      memcpy( p_SlpSessionID->sessionID.buf,
              p_sessionID,
              4 );
      p_SlpSessionID->sessionID.size = 4;
     // p_PDU_Cast->sessionID.slpSessionID.sessionID.IsAssigned  = XASS; //need to be check

      switch ( p_SlpId->type )
      {
      case GN_SLPAddress_PR_NOTHING:
         break;
      case GN_SLPAddress_PR_iPV4Address:
          p_SlpSessionID->slpId.present                                 = SLPAddress_PR_iPAddress;
          p_SlpSessionID->slpId.choice.iPAddress.present                = IPAddress_PR_ipv4Address;
          p_SlpSessionID->slpId.choice.iPAddress.choice.ipv4Address.size       = 4;
         p_SlpSessionID->slpId.choice.iPAddress.choice.ipv4Address.buf         = GN_Calloc( 1, 4 );
         memcpy(  p_SlpSessionID->slpId.choice.iPAddress.choice.ipv4Address.buf,
                  p_SlpId->u.iPAddress.ipv4Address,
                  4 );
        // p_SLPAddress->U.iPAddress2.U.ipv4Address.IsAssigned   = XASS;
         break;
      case GN_SLPAddress_PR_iPV6Address:
         p_SlpSessionID->slpId.present                                 = SLPAddress_PR_iPAddress;
         p_SlpSessionID->slpId.choice.iPAddress.present                    = IPAddress_PR_ipv6Address;
         p_SlpSessionID->slpId.choice.iPAddress.choice.ipv6Address.size       = 16;
         p_SlpSessionID->slpId.choice.iPAddress.choice.ipv6Address.buf         = GN_Calloc( 1, 16 );
         memcpy(  p_SlpSessionID->slpId.choice.iPAddress.choice.ipv6Address.buf,
                  p_SlpId->u.iPAddress.ipv6Address,
                  16 );
        // p_SLPAddress->U.iPAddress2.U.ipv6Address.IsAssigned   = XASS;
         break;
      case GN_SLPAddress_PR_fQDN:
         {
            U2 FQDN_Size = (U2) sizeof( p_SlpId->u.FQDN );

            p_SlpSessionID->slpId.present = SLPAddress_PR_fQDN;

            p_SlpSessionID->slpId.choice.fQDN.buf = GN_Calloc( 1, FQDN_Size );
            p_SlpSessionID->slpId.choice.fQDN.size = sprintf( (char *)p_SlpSessionID->slpId.choice.fQDN.buf, "%s", (L1*)p_SlpId->u.FQDN );

         }
         break;
      default:
         break;
      }
   }
}


//*****************************************************************************
/// \brief
///      Adds SLP Session ID to the unencoded SUPL PDU ULP_PDU_t.
///
/// \returns
///      nothing.
//*****************************************************************************
void supl_PDU_Add_slpSessionID
(
   void              *p_ThisPDU,    ///< [in/out] Pointer to unencoded structure
                                    ///   ULP_PDU_t. Passed in as (void *) to hide
                                    ///   implementation.
   U1                *p_sessionID,  ///< [in] Session ID.
   s_GN_SLPAddress   *p_SlpId       ///< [in] Slp ID.
)
{

    ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

    supl_PDU_Add_slpSessionIDLocal(&p_PDU_Cast->sessionID.slpSessionID,
                                  p_sessionID,p_SlpId);
}


//*****************************************************************************
/// \brief
///      Gets SLP Session ID from the unencoded SUPL PDU ULP_PDU_t.
///
/// \returns
///      TRUE if SLP Session available.
//*****************************************************************************
BL supl_PDU_Get_slpSessionID
(
   void              *p_ThisPDU,       ///< [in/out] Pointer to unencoded structure
                                       ///   ULP_PDU_t. Passed in as (void *) to hide
                                       ///   implementation.
   U1                **p_p_sessionID,  ///< [out] Session Id.
   s_GN_SLPAddress   *p_SlpId          ///< [out] SLP ID.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   if ( p_PDU_Cast->sessionID.slpSessionID == NULL ) return FALSE;
   switch ( p_PDU_Cast->sessionID.slpSessionID->slpId.present)
   {
   case SLPAddress_PR_iPAddress:
      /// \todo Gaurd against size issues.
       switch ( p_PDU_Cast->sessionID.slpSessionID->slpId.choice.iPAddress.present)
      {
      case IPAddress_PR_ipv4Address:
         p_SlpId->type = GN_SLPAddress_PR_iPV4Address;
         memcpy(  p_SlpId->u.iPAddress.ipv4Address,
             p_PDU_Cast->sessionID.slpSessionID->slpId.choice.iPAddress.choice.ipv4Address.buf,
                  4 );
         break;
      case IPAddress_PR_ipv6Address:
         p_SlpId->type = GN_SLPAddress_PR_iPV6Address;
         memcpy(  p_SlpId->u.iPAddress.ipv6Address,
             p_PDU_Cast->sessionID.slpSessionID->slpId.choice.iPAddress.choice.ipv6Address.buf,
                  16 );
         break;
      default:
         return FALSE;
         break;
      }
      if ( *p_p_sessionID == NULL )
      {
         *p_p_sessionID = GN_Calloc( 1, p_PDU_Cast->sessionID.slpSessionID->sessionID.size );
      }
      memcpy(  *p_p_sessionID,
          p_PDU_Cast->sessionID.slpSessionID->sessionID.buf,
               p_PDU_Cast->sessionID.slpSessionID->sessionID.size );
      break;
   case SLPAddress_PR_fQDN:
      p_SlpId->type = GN_SLPAddress_PR_fQDN;
      strcpy(  p_SlpId->u.FQDN,
               (CH*)p_PDU_Cast->sessionID.slpSessionID->slpId.choice.fQDN.buf );
      if ( *p_p_sessionID == NULL )
      {
         *p_p_sessionID = GN_Calloc( 1, p_PDU_Cast->sessionID.slpSessionID->sessionID.size );
      }
      memcpy( (char *) *p_p_sessionID,
                (char *) p_PDU_Cast->sessionID.slpSessionID->sessionID.buf,
               p_PDU_Cast->sessionID.slpSessionID->sessionID.size );
      break;
   default:
      p_SlpId->type = GN_SLPAddress_PR_NOTHING;
      break;
   }
   return TRUE;
}


//*****************************************************************************
/// \brief
///      Validates SLP Session ID in the unencoded SUPL PDU ULP_PDU_t.
///
/// \returns
///      TRUE if SLP Session matches.
//*****************************************************************************
BL supl_PDU_Validate_slpSessionID
(
   void              *p_ThisPDU,    ///< [in] Pointer to unencoded structure
                                    ///   ULP_PDU_t. Passed in as (void *) to hide
                                    ///   implementation.
   U1                *p_sessionID,  ///< [in] Session ID.
   s_GN_SLPAddress   *p_SlpId       ///< [in] Slp ID.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

  if ( p_PDU_Cast->sessionID.slpSessionID == NULL )
      return FALSE;

   if ( memcmp( p_PDU_Cast->sessionID.slpSessionID->sessionID.buf,p_sessionID, 4 ) != 0 )
      return FALSE;

   switch ( p_SlpId->type )
   {
   case GN_SLPAddress_PR_NOTHING:
      break;
   case GN_SLPAddress_PR_iPV4Address:
       if ( p_PDU_Cast->sessionID.slpSessionID->slpId.present != SLPAddress_PR_iPAddress )
      {
         return FALSE;
      }
       else if ( p_PDU_Cast->sessionID.slpSessionID->slpId.choice.iPAddress.present == IPAddress_PR_ipv4Address )
      {
          if ( memcmp( p_PDU_Cast->sessionID.slpSessionID->slpId.choice.iPAddress.choice.ipv4Address.buf,
                      p_SlpId->u.iPAddress.ipv4Address,
                      4 ) != 0 )
         {
            return FALSE;
         }
      }
      else
      {
         return FALSE;
      }
      break;
   case GN_SLPAddress_PR_iPV6Address:
      if ( p_PDU_Cast->sessionID.slpSessionID->slpId.present != SLPAddress_PR_iPAddress )
      {
         return FALSE;
      }
      else if ( p_PDU_Cast->sessionID.slpSessionID->slpId.choice.iPAddress.present == IPAddress_PR_ipv6Address )
      {
          if ( memcmp( p_PDU_Cast->sessionID.slpSessionID->slpId.choice.iPAddress.choice.ipv6Address.buf,
                      p_SlpId->u.iPAddress.ipv6Address,
                      16 ) != 0 )
         {
            return FALSE;
         }
      }
      else
      {
         return FALSE;
      }
      break;
   case GN_SLPAddress_PR_fQDN:
      {
         if ( p_PDU_Cast->sessionID.slpSessionID->slpId.present != SLPAddress_PR_fQDN )
         {
            return FALSE;
         }
           if ( strcmp( (CH*)p_PDU_Cast->sessionID.slpSessionID->slpId.choice.fQDN.buf, (CH*)p_SlpId->u.FQDN ) != 0 )
         {
            return FALSE;
         }
      }
      break;
   default:
         return FALSE;
      break;
   }
   return TRUE;
}


//*****************************************************************************
/// \brief
///      Add a bitstring in a U4 to a SDL_BitString type.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SDL_BitString_U4
(
   SDL_Bit_String *p_Bit_String,   ///< [out] Pointer to Telelogic ASN.1 bit string type.
   U4             Value,               ///< [in] Bits held in a U4, least significant bit in b0.
   U2             NoOfBits             ///< [in] Number of bits to encode.
)
{
   int i;
   U4 tempVal = Value;
   U1 SizeOfValue = ( ( NoOfBits - 1 ) / 8 ) + 1; // Calculate the size of the data from the number of bits it contains.
   U1 UnusedBits = ( SizeOfValue * 8 ) - NoOfBits; // Calculate how many of the bit in the most significant byte to skip.
 
   p_Bit_String->buf = (uint8_t*)GN_Calloc(1, SizeOfValue);
   // Start with the least significant byte highest in the allocated buffer.
   for ( i = SizeOfValue-1 ; i >= 0 ; i-- )
   {
      *( p_Bit_String->buf + i ) = tempVal & 0xFF;
      tempVal = tempVal >> 8;
   }
   p_Bit_String->size = SizeOfValue;
   p_Bit_String->bits_unused = UnusedBits;

}


//*****************************************************************************
/// \brief
///      Gets a bitstring SDL_BitString type into a U4.
///
/// \returns
///      TRUE if the bit string will fit into a U4.
//*****************************************************************************
BL supl_PDU_Get_SDL_BitString_U4
(
   SDL_Bit_String *p_Bit_String,   ///< [in] Pointer to Telelogic ASN.1 bit string type.
   U4             *p_Value             ///< [out] U4 in which to store the bits, least significant bit in b0.
)
{
   int i;
   U4 tempVal = 0;

   U1 p_tempVal[4]= {0,0,0,0};
   
   U1 NoOfBits = p_Bit_String->size * 8;
   U1 SizeOfValue = ( ( NoOfBits - 1 ) / 8 ) + 1; // Calculate the size of the data from the number of bits it contains.

   if ( NoOfBits > 16 )
   {
      // Bitstring won't fit into a U4.
      return FALSE;
   }

   // Start with the most significant bit lowest in the allocated buffer.
   for ( i = 0 ; i < SizeOfValue ; i++ )
   {
       if ( ( *(p_Bit_String->buf + i) & 0xFF ) != 0 )
      {
        *(p_tempVal +(SizeOfValue - 1)- i) = *(p_Bit_String->buf + i);
      }
   }

   tempVal =   ((p_tempVal[3] << 24) & 0xFF000000) | ((p_tempVal[2] << 16) & 0x00FF0000)
                 | ((p_tempVal[1] << 8 ) & 0x0000FF00) | ((p_tempVal[0]) & 0x000000FF);
   
   *p_Value = tempVal ;
    
   return TRUE;
}



//*****************************************************************************
/// \brief
///      Converts IP Address from os config to SLP Address format.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_GN_SLPAddress_From_GN_IpAddress
(
   s_GN_SLPAddress   *p_GN_SLPAddress, ///< [out] SLP Address.
   s_SUPL_IpAddress  *p_SUPL_IpAddress ///< [in] SLP Address from os config.
)
{
   int  Add0, Add1, Add2, Add3, Add4, Add5, Add6, Add7, Add8, Add9, Add10, Add11, Add12, Add13, Add14, Add15;

   switch ( p_SUPL_IpAddress->TcpIp_AddressType )
   {
   case IPV4_Address: // IP address is of type IPV4.
      p_GN_SLPAddress->type = GN_SLPAddress_PR_iPV4Address;

      sscanf( p_SUPL_IpAddress->Address, "%u.%u.%u.%u", &Add0, &Add1, &Add2, &Add3 );

      p_GN_SLPAddress->u.iPAddress.ipv4Address[0] = Add0;
      p_GN_SLPAddress->u.iPAddress.ipv4Address[1] = Add1;
      p_GN_SLPAddress->u.iPAddress.ipv4Address[2] = Add2;
      p_GN_SLPAddress->u.iPAddress.ipv4Address[3] = Add3;

      break;
   case IPV6_Address: // IP address is of type IPV6.
      p_GN_SLPAddress->type = GN_SLPAddress_PR_iPV6Address;
      sscanf( p_SUPL_IpAddress->Address, "%u:%u:%u:%u:%u:%u:%u:%u:%u:%u:%u:%u:%u:%u:%u:%u",
            &Add0, &Add1, &Add2, &Add3, &Add4, &Add5, &Add6, &Add7, &Add8, &Add9, &Add10, &Add11, &Add12, &Add13, &Add14, &Add15 );
      p_GN_SLPAddress->u.iPAddress.ipv6Address[0] = Add0;
      p_GN_SLPAddress->u.iPAddress.ipv6Address[1] = Add1;
      p_GN_SLPAddress->u.iPAddress.ipv6Address[2] = Add2;
      p_GN_SLPAddress->u.iPAddress.ipv6Address[3] = Add3;
      p_GN_SLPAddress->u.iPAddress.ipv6Address[4] = Add4;
      p_GN_SLPAddress->u.iPAddress.ipv6Address[5] = Add5;
      p_GN_SLPAddress->u.iPAddress.ipv6Address[6] = Add6;
      p_GN_SLPAddress->u.iPAddress.ipv6Address[7] = Add7;
      p_GN_SLPAddress->u.iPAddress.ipv6Address[8] = Add8;
      p_GN_SLPAddress->u.iPAddress.ipv6Address[9] = Add9;
      p_GN_SLPAddress->u.iPAddress.ipv6Address[10] = Add10;
      p_GN_SLPAddress->u.iPAddress.ipv6Address[11] = Add11;
      p_GN_SLPAddress->u.iPAddress.ipv6Address[12] = Add12;
      p_GN_SLPAddress->u.iPAddress.ipv6Address[13] = Add13;
      p_GN_SLPAddress->u.iPAddress.ipv6Address[14] = Add14;
      p_GN_SLPAddress->u.iPAddress.ipv6Address[15] = Add15;
      break;
   case FQDN_Address:
      strcpy( p_GN_SLPAddress->u.FQDN , p_SUPL_IpAddress->Address );
      break;
   default :
      p_GN_SLPAddress->type = GN_SLPAddress_PR_NOTHING;
   }
}


//*****************************************************************************
/// \brief
///      Creates an SLP Session ID based on the Instance ID.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_Make_slpSessionID
(
   U4                SessionId,        ///< [in] Current instance data used to
                                       ///   create the Session ID.
   U1                **p_p_sessionID,  ///< [out] Session Id.
   s_GN_SLPAddress   *p_SlpId          ///< [out] SLP ID.
)
{
   // SlpSessionID ::= SEQUENCE {
   //    sessionID   OCTET STRING(SIZE (4)),
   //    slpId       SLPAddress}

   s_SUPL_IpAddress *p_SUPL_IpAddress;

   *p_p_sessionID = GN_Calloc( 1, 4 + 1 ); // Size of sessionID from ASN.1 definition + 1 for null.
   memcpy( *p_p_sessionID, &SessionId, 4 );

   (*p_p_sessionID)[3] = (U1) ( SessionId       & 0xff );
   (*p_p_sessionID)[2] = (U1) ( SessionId >>  8 & 0xff );
   (*p_p_sessionID)[1] = (U1) ( SessionId >> 16 & 0xff );
   (*p_p_sessionID)[0] = (U1) ( SessionId >> 24 & 0xff );

   p_SUPL_IpAddress = supl_config_get_SUPL_SLP_address();

   supl_GN_SLPAddress_From_GN_IpAddress( p_SlpId, p_SUPL_IpAddress );
}


//*****************************************************************************
/// \brief
///      Function to identify the contents of an unencoded SUPL PDU.
///
/// \returns
///      The type of message as an enum of #e_GN_UlpMessageType
//*****************************************************************************
e_GN_UlpMessageType supl_PDU_Get_MessageType
(
   void *p_ThisPDU   ///< [in] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   switch ( p_PDU_Cast->message.present )
   {
   case UlpMessage_PR_msSUPLINIT:               return GN_UlpMessage_PR_msSUPLINIT;              break;
   case UlpMessage_PR_msSUPLSTART:              return GN_UlpMessage_PR_msSUPLSTART;             break;
   case UlpMessage_PR_msSUPLRESPONSE:           return GN_UlpMessage_PR_msSUPLRESPONSE;          break;
   case UlpMessage_PR_msSUPLPOSINIT:            return GN_UlpMessage_PR_msSUPLPOSINIT;           break;
   case UlpMessage_PR_msSUPLPOS:                return GN_UlpMessage_PR_msSUPLPOS;               break;
   case UlpMessage_PR_msSUPLEND:                return GN_UlpMessage_PR_msSUPLEND;               break;
   case UlpMessage_PR_msSUPLAUTHREQ:            return GN_UlpMessage_PR_msSUPLAUTHREQ;           break;
   case UlpMessage_PR_msSUPLAUTHRESP:           return GN_UlpMessage_PR_msSUPLAUTHRESP;          break;
   case UlpMessage_PR_msSUPLTRIGGEREDRESPONSE:  return GN_UlpMessage_PR_msSUPLTRIGGEREDRESPONSE; break;
   case UlpMessage_PR_msSUPLTRIGGEREDSTART:     return GN_UlpMessage_PR_msSUPLTRIGGEREDSTART;    break; 
   case UlpMessage_PR_msSUPLREPORT:             return GN_UlpMessage_PR_msSUPLREPORT;            break; 
   case UlpMessage_PR_msSUPLSETINIT:            return GN_UlpMessage_PR_msSUPLSETINIT;           break;
   case UlpMessage_PR_msSUPLTRIGGEREDSTOP:      return GN_UlpMessage_PR_msSUPLTRIGGEREDSTOP;     break;   
   case UlpMessage_PR_msSUPLNOTIFY:             return GN_UlpMessage_PR_msSUPLNOTIFY;            break;
   case UlpMessage_PR_msSUPLNOTIFYRESPONSE:     return GN_UlpMessage_PR_msSUPLNOTIFYRESPONSE;    break;
   default:
      OS_ASSERT( 0 );
      break;
   }
   return GN_UlpMessage_PR_NOTHING;
}


//*****************************************************************************
/// \brief
///      Function to set the unencoded SUPL PDU type to SUPL-START.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLSTART
(
   void *p_ThisPDU   ///< [in/out] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   // SUPL-START defined in SUPL-START.asn1
   p_PDU_Cast->message.present = UlpMessage_PR_msSUPLSTART;
}

//*****************************************************************************
/// \brief
///      Function to set the unencoded SUPL PDU type to SUPL-SETINIT.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLSETINIT
(
   void *p_ThisPDU   ///< [in/out] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   // SUPL-START defined in SUPL-START.asn1
   p_PDU_Cast->message.present = UlpMessage_PR_msSUPLSETINIT;
}
void supl_PDU_Add_ReportingCapabilities(
   ReportingCap_t       *reportingCapabilities,
   s_GN_SetCapabilities *p_SetCapabilities
                                        )
{
      reportingCapabilities->minInt
         = p_SetCapabilities->V2_SetCapabilities.ReportingCapabilities.MinimumInterval;

      if( p_SetCapabilities->V2_SetCapabilities.ReportingCapabilities.MaximumInterval != (U2)-1 )
      {
         reportingCapabilities->maxInt = GN_Calloc( 1 , sizeof(long) );
         *reportingCapabilities->maxInt
            = p_SetCapabilities->V2_SetCapabilities.ReportingCapabilities.MaximumInterval;
      }

      /* RepMode ::= SEQUENCE {
         realtime BOOLEAN,
         quasirealtime BOOLEAN,
         batch BOOLEAN,
         ...}*/
      reportingCapabilities->repMode.realtime
         = p_SetCapabilities->V2_SetCapabilities.ReportingCapabilities.ReportingMode.RealTime;
      reportingCapabilities->repMode.batch
         = p_SetCapabilities->V2_SetCapabilities.ReportingCapabilities.ReportingMode.Batch;
      reportingCapabilities->repMode.quasirealtime
         = p_SetCapabilities->V2_SetCapabilities.ReportingCapabilities.ReportingMode.QuasiRealTime;

      /* BatchRepCap ::= SEQUENCE {
         report-position BOOLEAN, -- set to “true” if reporting of position is
         supported
         report-measurements BOOLEAN, -- set to “true” if reporting of measurements is
         supported
         max-num-positions INTEGER (1..1024) OPTIONAL,
         max-num-measurements INTEGER (1..1024) OPTIONAL,
         ...} */


  if(p_SetCapabilities->V2_SetCapabilities.ReportingCapabilities.ReportingMode.Batch == TRUE)
  {
      reportingCapabilities->batchRepCap = GN_Calloc( 1 , sizeof(BatchRepCap_t) );

      if( p_SetCapabilities->V2_SetCapabilities.ReportingCapabilities.BatchRepCap.MaxNumMeasuremetns != (U2)-1 )
      {
         reportingCapabilities->batchRepCap->max_num_measurements = GN_Calloc( 1 , sizeof(long) );
         *reportingCapabilities->batchRepCap->max_num_measurements
            = p_SetCapabilities->V2_SetCapabilities.ReportingCapabilities.BatchRepCap.MaxNumMeasuremetns;
      }

      reportingCapabilities->batchRepCap->report_measurements
         = p_SetCapabilities->V2_SetCapabilities.ReportingCapabilities.BatchRepCap.ReportMeasurements;

      if( p_SetCapabilities->V2_SetCapabilities.ReportingCapabilities.BatchRepCap.MaxNumPosition != (U2)-1 )
      {
         reportingCapabilities->batchRepCap->max_num_positions = GN_Calloc( 1 , sizeof(long) );
         *reportingCapabilities->batchRepCap->max_num_positions
            = p_SetCapabilities->V2_SetCapabilities.ReportingCapabilities.BatchRepCap.MaxNumPosition;
      }

      reportingCapabilities->batchRepCap->report_position
         = p_SetCapabilities->V2_SetCapabilities.ReportingCapabilities.BatchRepCap.ReportPosition;
  }
  else
  {
     reportingCapabilities->batchRepCap = NULL;
  }

}


//*****************************************************************************
/// \brief
///      Function to convert a Set Location ID into an native asn1c format.
///
/// \returns
///      Nothing.
//*****************************************************************************
BL supl_PDU_Add_Multiple_LocationID
(
   MultipleLocationIds_t     *p_multipleLocationIds  ,  ///< [out] Native asn1c LocationId format.
   
   s_SUPL_Instance *p_SUPL_Instance       ///  Added for Supported Info check
)
{
    U1 index = 0;
    uint32_t   p_Current_OSTimeMs ,p_RelativeTime; ///< Local Operating System Time [msec]
    s_GN_SUPL_V2_Multiple_Loc_ID *temp = SUPL_Mlid_FirstElem ;
    BL Info_Supported ;
    BL Mlid_Supported = FALSE ;

    struct timeval currentime = {0};
    (void) gettimeofday( &currentime , NULL );

    p_Current_OSTimeMs = currentime.tv_sec*1000;

    s_GN_SUPL_V2_SupportedNetworkInfo *p_SupportedNetworkInfo = p_SUPL_Instance->p_SUPL->V2_Data.p_SupportedNetworkInfo ;

    GN_SUPL_Log( "supl_PDU_Add_Multiple_LocationID:" );
    
    SUPL_Check_Loc_Id_Expiry();

    for( index = 0; ( index < MaxLocIdSize ) && ( temp != NULL ) ; index++ )
    {
        LocationIdData_t* p_LocationIdData = (LocationIdData_t*)GN_Calloc( 1 , sizeof(LocationIdData_t) );

        LocationId_t    *p_LocationId = &(p_LocationIdData->locationId) ;

        s_LocationId *p_SetLocationID = &(temp->v_LocationId) ;

        Info_Supported = TRUE ;

        BL eCID_Supported = supl_config_get_SUPL_eCID();

        p_RelativeTime =  p_Current_OSTimeMs - temp->v_OSTimeMs ;

        if(temp->next == NULL)
        {
            p_LocationIdData->relativetimestamp = NULL ;
        }
        else
        {
            p_LocationIdData->relativetimestamp = (RelativeTime_t*)GN_Calloc( 1 , sizeof(RelativeTime_t) );

            *p_LocationIdData->relativetimestamp = p_RelativeTime/10 ;// Interms of .01s
        }
        
        p_LocationIdData->servingFlag = ( p_LocationIdData->relativetimestamp == NULL ) ? SDL_True : SDL_False;

        if(p_SetLocationID)
        {
            switch(p_SetLocationID->Status)
            {
                case CIS_stale:
                    ASN1_ENUM_SET(p_LocationId->status,stale);//Cell Information is Stale
                    break;
                case CIS_current:
                    ASN1_ENUM_SET(p_LocationId->status,current);//Cell Information is Current
                    break;
                case CIS_unknown:
                    ASN1_ENUM_SET(p_LocationId->status,unknown);//Cell Information is Unknown
                    break;
                case CIS_NONE:
                default:
                    break;
            }
            
            if((p_SetLocationID->Type  == CIT_AccessPoint_WLAN)||
               (p_SetLocationID->Type  == CIT_AccessPoint_WIMAX))
            {
                uint16_t v_MS_Addr;
                uint32_t v_LS_Addr;
                
                switch(p_SetLocationID->Type)
                {
                    case CIT_AccessPoint_WLAN:
                        if( p_SupportedNetworkInfo != NULL && p_SupportedNetworkInfo->wLAN == FALSE )
                        {
                            GN_SUPL_Log( " Wlan Not Supported " );
                            Info_Supported = FALSE ;
                        }
                        else
                        {
                            /*WLAN Access Point Wireless network is present*/
                            /*Populate the WLAN AP data on to the PDU*/
                            v_MS_Addr = p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_MS_Addr;
                            v_LS_Addr = p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_LS_Addr;
                            p_LocationId->cellInfo.present = CellInfo_PR_ver2_CellInfo_extension;
                            p_LocationId->cellInfo.choice.ver2_CellInfo_extension.present = Ver2_CellInfo_extension_PR_wlanAP;
                            p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apMACAddress.size = sizeof(v_LS_Addr )+sizeof( v_MS_Addr);
                            p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apMACAddress.buf = (char*)GN_Calloc( 1, (p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apMACAddress.size )+1 );
                            memcpy(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apMACAddress.buf,
                                  &( p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_LS_Addr),
                                   sizeof(p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_LS_Addr));

                            memcpy((p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apMACAddress.buf+ sizeof(v_LS_Addr)),
                                   &(p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_MS_Addr),
                                   sizeof(p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_MS_Addr));

                            p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apMACAddress.buf[sizeof(v_MS_Addr+v_LS_Addr)+1] = '\0';

                            p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apTransmitPower = (long *)GN_Calloc( 1, sizeof( long ) );
                            *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apTransmitPower ) = (long)p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_TransmitPower;

                            p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apAntennaGain = (long *)GN_Calloc( 1, sizeof( long ) );
                            *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apAntennaGain ) = (long)p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_AntennaGain;


                            p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apSignaltoNoise = (long *)GN_Calloc( 1, sizeof( long ) );
                            *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apSignaltoNoise ) = (long)p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SNR;

                            p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apDeviceType = (ENUMERATED_t *)GN_Calloc( 1, sizeof( ENUMERATED_t ) );

                            p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apDeviceType->size = sizeof(p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_DeviceType);
                            p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apDeviceType->buf  = (char *)GN_Calloc( 1, p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apDeviceType->size );
                            memcpy(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apDeviceType->buf,
                                   &(p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_DeviceType),
                                   p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apDeviceType->size);

                            p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apSignalStrength = (long *)GN_Calloc( 1, sizeof( long ) );
                            *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apSignalStrength ) = (long)p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SignalStrength;

                            p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apChannelFrequency = (long *)GN_Calloc( 1, sizeof( long ) );
                            *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apChannelFrequency ) = (long)p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_Channel;

                            p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apRoundTripDelay = (RTD_t *)GN_Calloc( 1, sizeof( RTD_t ) );
                            p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apRoundTripDelay->rTDValue = p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_RTDValue;
                            p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apRoundTripDelay->rTDAccuracy = (long *)GN_Calloc( 1, sizeof( long ) );
                            *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apRoundTripDelay->rTDAccuracy ) = p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_Accuracy;
                            p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apRoundTripDelay->rTDUnits.size = sizeof(p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_RTDUnits);
                            p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apRoundTripDelay->rTDUnits.buf  = (uint8_t *)GN_Calloc(1, p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apRoundTripDelay->rTDUnits.size);
                            memcpy(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apRoundTripDelay->rTDUnits.buf,
                                   &(p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_RTDUnits),
                                   p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apRoundTripDelay->rTDUnits.size);


                            p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.setTransmitPower = (long *)GN_Calloc( 1, sizeof( long ) );
                            *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.setTransmitPower ) = (long)p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETTransmitPower;


                            p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.setAntennaGain = (long *)GN_Calloc( 1, sizeof( long ) );
                            *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.setAntennaGain ) = (long)p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETAntennaGain;

                            p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.setSignaltoNoise = (long *)GN_Calloc( 1, sizeof( long ) );
                            *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.setSignaltoNoise ) = (long)p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETSNR;


                            p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.setSignalStrength = (long *)GN_Calloc( 1, sizeof( long ) );
                            *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.setSignalStrength ) = (long)p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETSignalStrength;

                            GN_SUPL_Log( "supl_PDU_Add_SetLocationID: WlanAccessPointInfo.v_MS_Addr %d",
                                          v_MS_Addr);
                            GN_SUPL_Log( "supl_PDU_Add_SetLocationID: WlanAccessPointInfo.v_LS_Addr %d",
                                          v_LS_Addr);

                            GN_SUPL_Log( "supl_PDU_Add_SetLocationID: WlanAccessPointInfo.v_Accuracy %d",
                                          *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apRoundTripDelay->rTDAccuracy ));
                            GN_SUPL_Log( "supl_PDU_Add_SetLocationID: WlanAccessPointInfo.v_AntennaGain %d",
                                          *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apAntennaGain ));
                            GN_SUPL_Log( "supl_PDU_Add_SetLocationID: WlanAccessPointInfo.v_Channel %d",
                                          *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apChannelFrequency ));
                            GN_SUPL_Log( "supl_PDU_Add_SetLocationID: WlanAccessPointInfo.v_DeviceType %s",
                                          p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apDeviceType->buf);

                            GN_SUPL_Log( "supl_PDU_Add_SetLocationID: WlanAccessPointInfo.v_RTDUnits %s",
                                          p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apRoundTripDelay->rTDUnits.buf);
                            GN_SUPL_Log( "supl_PDU_Add_SetLocationID: WlanAccessPointInfo.v_RTDValue %d",
                                          p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apRoundTripDelay->rTDValue);

                            GN_SUPL_Log( "supl_PDU_Add_SetLocationID: WlanAccessPointInfo.v_SETAntennaGain %d",
                                          *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.setAntennaGain ));
                            GN_SUPL_Log( "supl_PDU_Add_SetLocationID: WlanAccessPointInfo.v_SETSignalStrength %d",
                                          *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.setSignalStrength ));
                            GN_SUPL_Log( "supl_PDU_Add_SetLocationID: WlanAccessPointInfo.v_SETSNR %d",
                                          *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.setSignaltoNoise ));
                            GN_SUPL_Log( "supl_PDU_Add_SetLocationID: WlanAccessPointInfo.v_SETTransmitPower %d",
                                          *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.setTransmitPower ));
                            GN_SUPL_Log( "supl_PDU_Add_SetLocationID: WlanAccessPointInfo.v_SignalStrength %d",
                                          *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apSignalStrength ));
                            GN_SUPL_Log( "supl_PDU_Add_SetLocationID: WlanAccessPointInfo.v_SNR %d",
                                         *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apSignaltoNoise ));
                            GN_SUPL_Log( "supl_PDU_Add_SetLocationID: WlanAccessPointInfo.v_TransmitPower %d",
                                          *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apTransmitPower ));
                        }

                        /*Doubt apReportedLocation is not present in the SUPL instance so as to be populated in the PDU where from can we get this?*/
                        break;
                    case CIT_AccessPoint_WIMAX:
                        if( p_SupportedNetworkInfo != NULL && p_SupportedNetworkInfo->wIMAX == FALSE )
                        {
                            GN_SUPL_Log( " WIMAX Not Supported " );
                            Info_Supported = FALSE ;
                        }
                        break;
                    case CIT_AccessPoint_UNKNOWN:
                        break;
                    default:
                        break;
                }

            }
            else if((p_SetLocationID->Type == CIT_gsmCell)||(p_SetLocationID->Type == CIT_wcdmaCell)||(p_SetLocationID->Type == CIT_cdmaCell))
            {
               switch ( p_SetLocationID->Type )
               {
               case CIT_gsmCell:       // Cell information is from a GSM network.
                  if( (p_SupportedNetworkInfo != NULL) && (p_SupportedNetworkInfo->gSM == FALSE ))
                  {
                      GN_SUPL_Log( " gSM Not Supported " );
                      Info_Supported = FALSE ;
                  }
                  else
                  {
                      p_LocationId->cellInfo.present = CellInfo_PR_gsmCell;
                      p_LocationId->cellInfo.choice.gsmCell.refMCC = p_SetLocationID->of_type.gsmCellInfo.refMCC;
                      p_LocationId->cellInfo.choice.gsmCell.refMNC = p_SetLocationID->of_type.gsmCellInfo.refMNC;
                      p_LocationId->cellInfo.choice.gsmCell.refLAC = p_SetLocationID->of_type.gsmCellInfo.refLAC;
                      p_LocationId->cellInfo.choice.gsmCell.refCI  = p_SetLocationID->of_type.gsmCellInfo.refCI;
                      if ( p_SetLocationID->of_type.gsmCellInfo.tA != -1 )
                      {
                         //p_LocationId->cellInfo.U.gsmCell.tAPresent = SDL_True;
                          p_LocationId->cellInfo.choice.gsmCell.tA = (long*)GN_Calloc( 1, sizeof( long ) );
                          *p_LocationId->cellInfo.choice.gsmCell.tA = (long)p_SetLocationID->of_type.gsmCellInfo.tA;
                      }
                      if ( eCID_Supported && p_SetLocationID->of_type.gsmCellInfo.NMRCount != 0 )
                      {
                          NMR_t **p_p_Nmr;
                          NMRelement_t *nmr_one_element;
                          int NMRCount;
                          NMR_t *p_Nmr =NULL;

                         // We have more than 0 entries so we need to set this as present.
                         //p_LocationId->cellInfo.U.gsmCell.nMRPresent = SDL_True;
                         // Use p_p_thisNMR as a pointer to storage for the current NMR to be set after the record is completed.
                         p_p_Nmr = &p_LocationId->cellInfo.choice.gsmCell.nMR; //= (NMR_t*)GN_Calloc( 1, sizeof( NMR_t ) );

                        if (ASN1_GET(p_p_Nmr) == NULL)
                        {
                          SUPL_HEAP_ALLOC(ASN1_SET(p_p_Nmr), NMR_t);

                          if (ASN1_GET(p_p_Nmr) == NULL)
                          {
                               GN_SUPL_Log( "supl_PDU_Add_SetLocationID:Heap allocation error nmr_t");
                          }
                        }
                            p_Nmr = ASN1_GET(p_p_Nmr);
                            ASN1_SEQ_SIZE(p_Nmr, 0);
                              p_Nmr->ASN1_SEQ_PTR = GN_Calloc( 1, sizeof( char* ) );
                         // Allocate memory for the record.
                        // p_NMRelement = GN_Calloc( 1, sizeof( NMRelement_t ) * p_SetLocationID->of_type.gsmCellInfo.NMRCount);
                        // nmr_one_element = p_NMRelement;
                         //p_Nmr = &p_LocationId->cellInfo.choice.gsmCell.nMR;
                         for ( NMRCount = 0;
                               NMRCount < p_SetLocationID->of_type.gsmCellInfo.NMRCount &&
                               NMRCount <= 15; // NMR can have upto 15 entries. Just ignore the rest.
                               NMRCount++ )
                         {

                            // Populate the record.
                            p_Nmr->list.array[NMRCount] = GN_Calloc( 1, sizeof( NMRelement_t ));
                             nmr_one_element = p_Nmr->list.array[NMRCount];
                            nmr_one_element->aRFCN = p_SetLocationID->of_type.gsmCellInfo.p_NMRList[NMRCount].aRFCN;
                            nmr_one_element->bSIC  = p_SetLocationID->of_type.gsmCellInfo.p_NMRList[NMRCount].bSIC;
                            nmr_one_element->rxLev = p_SetLocationID->of_type.gsmCellInfo.p_NMRList[NMRCount].rxLev;

                         }

                         // Set the length field to the count or 15, whichever is smaller.
                          //assign nmr
                        p_Nmr->ASN1_SEQ_COUNT = NMRCount;
                       // ASN1_SET(p_Nmr->ASN1_SEQ_PTR) = p_NMRelement;
                        //p_Nmr->list.array = p_NMRelement;
                        ASN1_SEQ_SIZE(p_Nmr, sizeof(NMRelement_t) * NMRCount);
                        //ASN1_SET(p_p_Nmr) = p_Nmr;

                      }
                      else
                      {
                          p_LocationId->cellInfo.choice.gsmCell.nMR =NULL;
                      }
                  }
                  break;
               case CIT_wcdmaCell:     // Cell information is from a WCDMA network.
                  if( (p_SupportedNetworkInfo != NULL) && ( p_SupportedNetworkInfo->wCDMA == FALSE ))
                  {
                     GN_SUPL_Log( " wCDMA Not Supported " );
                     Info_Supported = FALSE ;
                  }
                  else
                  {
                      p_LocationId->cellInfo.present = CellInfo_PR_wcdmaCell;
                      // U2 refMCC;                    // INTEGER(0..999), -- Mobile Country Code
                      p_LocationId->cellInfo.choice.wcdmaCell.refMCC = p_SetLocationID->of_type.wcdmaCellInfo.refMCC;
                      // U2 refMNC;                    // INTEGER(0..999), -- Mobile Network Code
                      p_LocationId->cellInfo.choice.wcdmaCell.refMNC = p_SetLocationID->of_type.wcdmaCellInfo.refMNC;
                      // U4 refUC;                     // INTEGER(0..268435455), -- Cell identity
                      p_LocationId->cellInfo.choice.wcdmaCell.refUC  = p_SetLocationID->of_type.wcdmaCellInfo.refUC;
                      // I2 primaryScramblingCode;     // INTEGER(0..511) OPTIONAL (-1 = not present).,
                      if ( p_SetLocationID->of_type.wcdmaCellInfo.primaryScramblingCode != -1 )
                      {
                        // p_LocationId->cellInfo.U.wcdmaCell.primaryScramblingCodePresent = SDL_True;
                          p_LocationId->cellInfo.choice.wcdmaCell.primaryScramblingCode = (long*)GN_Calloc( 1, sizeof( long ) );
                          *p_LocationId->cellInfo.choice.wcdmaCell.primaryScramblingCode = p_SetLocationID->of_type.wcdmaCellInfo.primaryScramblingCode;
                      }
                      else
                      {
                         p_LocationId->cellInfo.choice.wcdmaCell.primaryScramblingCode = NULL;
                      }
                      // s_FrequencyInfo*
                      //    p_FrequencyInfo;           // FrequencyInfo OPTIONAL
                      if ( p_SetLocationID->of_type.wcdmaCellInfo.p_FrequencyInfo != NULL )
                      {
                         //p_LocationId->cellInfo.U.wcdmaCell.frequencyInfo1Present = SDL_True;
                          //allocate memory for asn1 frequency ino
                          p_LocationId->cellInfo.choice.wcdmaCell.frequencyInfo = (FrequencyInfo_t*)GN_Calloc( 1, sizeof( FrequencyInfo_t) );
                         /// Fill in Frequency Info.
                         switch ( p_SetLocationID->of_type.wcdmaCellInfo.p_FrequencyInfo->modeSpecificInfoType )
                         {
                         case fdd:               // Frequency Division Duplexed mode
                             p_LocationId->cellInfo.choice.wcdmaCell.frequencyInfo->modeSpecificInfo.present = FrequencyInfo__modeSpecificInfo_PR_fdd;
                             p_LocationId->cellInfo.choice.wcdmaCell.frequencyInfo->modeSpecificInfo.choice.fdd.uarfcn_DL = p_SetLocationID->of_type.wcdmaCellInfo.p_FrequencyInfo->of_type.fdd.uarfcn_DL;
                            if ( p_SetLocationID->of_type.wcdmaCellInfo.p_FrequencyInfo->of_type.fdd.uarfcn_UL != -1 )
                            {
                                p_LocationId->cellInfo.choice.wcdmaCell.frequencyInfo->modeSpecificInfo.choice.fdd.uarfcn_UL = (UARFCN_t*)GN_Calloc( 1, sizeof( UARFCN_t ) );
                                *p_LocationId->cellInfo.choice.wcdmaCell.frequencyInfo->modeSpecificInfo.choice.fdd.uarfcn_UL = p_SetLocationID->of_type.wcdmaCellInfo.p_FrequencyInfo->of_type.fdd.uarfcn_UL;
                            }
                            else
                            {
                               p_LocationId->cellInfo.choice.wcdmaCell.frequencyInfo->modeSpecificInfo.choice.fdd.uarfcn_UL = NULL;
                                //p_LocationId->cellInfo.U.wcdmaCell.frequencyInfo1.modeSpecificInfo.U.fdd1.uarfcn_ULPresent = SDL_False ;
                            }
                            break;
                         case tdd:               // Time Division Duplexed mode
                            p_LocationId->cellInfo.choice.wcdmaCell.frequencyInfo->modeSpecificInfo.present = FrequencyInfo__modeSpecificInfo_PR_tdd;
                            p_LocationId->cellInfo.choice.wcdmaCell.frequencyInfo->modeSpecificInfo.choice.tdd.uarfcn_Nt = p_SetLocationID->of_type.wcdmaCellInfo.p_FrequencyInfo->of_type.tdd.uarfcn_Nt;
                            break;
                         default:
                            OS_ASSERT(0);
                            break;
                         }
                      }
                      else
                      {
                          p_LocationId->cellInfo.choice.wcdmaCell.frequencyInfo = NULL;
                      }
                      // U1 MeasuredResultCount;       // maxFreq INTEGER ::= 8
                      if ( eCID_Supported && p_SetLocationID->of_type.wcdmaCellInfo.MeasuredResultCount != 0 )
                      {
                         MeasuredResultsList_t *p_MeasuredResultsList;
                         U1 MeasuredResultCount;
                         U1 MeasuredResultTotal = p_SetLocationID->of_type.wcdmaCellInfo.MeasuredResultCount;

                         p_LocationId->cellInfo.choice.wcdmaCell.measuredResultsList = p_MeasuredResultsList = (MeasuredResultsList_t*)GN_Calloc(1,sizeof(MeasuredResultsList_t));
                         // s_MeasuredResult*
                         //    p_MeasuredResultsList;     // OPTIONAL (Present if MeasuredResultCount > 0). Pointer to array of s_MeasuredResult.
                         for ( MeasuredResultCount = 0;
                               MeasuredResultCount < MeasuredResultTotal &&
                               MeasuredResultCount < 8; // MeasuredResultList can have upto 8 entries. Just ignore the rest.
                               MeasuredResultCount++ )
                         {
                            // Allocate memory for the record.
                            MeasuredResults_t *p_MeasuredResults = GN_Calloc( 1, sizeof( MeasuredResults_t ) );
                            s_MeasuredResult  *p_src_MeasuredResult = &p_SetLocationID->of_type.wcdmaCellInfo.p_MeasuredResultsList[MeasuredResultCount];;
                            // Populate the record.
                            // I1 utra_CarrierRSSI;          // INTEGER(0..127) OPTIONAL, (-1 = Not present).
                            if ( p_src_MeasuredResult->utra_CarrierRSSI != -1 )
                            {
                               p_MeasuredResults->utra_CarrierRSSI = GN_Calloc(1,sizeof(UTRA_CarrierRSSI_t));;
                               *p_MeasuredResults->utra_CarrierRSSI        = p_src_MeasuredResult->utra_CarrierRSSI;
                            }
                            else
                            {
                               p_MeasuredResults->utra_CarrierRSSI = NULL;
                            }

                            // BL FrequencyInfoPresent;      // TRUE if FrequencyInfo is present.
                            if ( p_src_MeasuredResult->FrequencyInfoPresent )
                            {
                                p_MeasuredResults->frequencyInfo = (FrequencyInfo_t*)GN_Calloc( 1, sizeof( FrequencyInfo_t) );
                               // s_FrequencyInfo
                               //    FrequencyInfo;             // OPTIONAL (Present if FrequencyInfo == TRUE).
                               switch ( p_src_MeasuredResult->FrequencyInfo.modeSpecificInfoType )
                               {
                               case fdd:                 // Frequency Division Duplexed mode
                                   p_MeasuredResults->frequencyInfo->modeSpecificInfo.present = FrequencyInfo__modeSpecificInfo_PR_fdd;
                                  p_MeasuredResults->frequencyInfo->modeSpecificInfo.choice.fdd.uarfcn_DL = p_src_MeasuredResult->FrequencyInfo.of_type.fdd.uarfcn_DL;
                                  if ( p_src_MeasuredResult->FrequencyInfo.of_type.fdd.uarfcn_UL != -1 )
                                  {
                                     p_MeasuredResults->frequencyInfo->modeSpecificInfo.choice.fdd.uarfcn_UL = (UARFCN_t*)GN_Calloc( 1, sizeof( UARFCN_t ) );
                                     *p_MeasuredResults->frequencyInfo->modeSpecificInfo.choice.fdd.uarfcn_UL = p_src_MeasuredResult->FrequencyInfo.of_type.fdd.uarfcn_UL;
                                  }
                                  else
                                  {
                                     p_MeasuredResults->frequencyInfo->modeSpecificInfo.choice.fdd.uarfcn_UL = NULL ;
                                  }
                                  break;
                               case tdd:                 // Time Division Duplexed mode
                                   p_MeasuredResults->frequencyInfo->modeSpecificInfo.present = FrequencyInfo__modeSpecificInfo_PR_tdd;
                                   p_MeasuredResults->frequencyInfo->modeSpecificInfo.choice.tdd.uarfcn_Nt = p_src_MeasuredResult->FrequencyInfo.of_type.tdd.uarfcn_Nt;
                                  break;
                               }
                            }
                            else
                            {
                               p_MeasuredResults->frequencyInfo = NULL;
                            }

                            // U1 CellMeasuredResultCount;   // Count of MeasuredResults up to (maxCellMeas INTEGER ::= 32)
                            if ( p_src_MeasuredResult->CellMeasuredResultCount > 0 )
                            {
                               U1 CellMeasuredResultsCount;
                               U1 CellMeasuredResultsTotal;
                               p_MeasuredResults->cellMeasuredResultsList = GN_Calloc(1,sizeof(CellMeasuredResultsList_t));

                               CellMeasuredResultsTotal = p_src_MeasuredResult->CellMeasuredResultCount;
                               // s_CellMeasuredResult*
                               //    p_CellMeasuredResultsList; // OPTIONAL (Present if CellMeasuredResultCount > 0) Pointer to an array of s_CellMeasuredResult.
                               for ( CellMeasuredResultsCount = 0;
                                     CellMeasuredResultsCount < CellMeasuredResultsTotal &&
                                     CellMeasuredResultsCount < 32; // CellMeasuredResultList can have upto 32 entries. Just ignore the rest.
                                     CellMeasuredResultsCount++ )
                               {
                                  // Allocate memory for the record.
                                  CellMeasuredResults_t *p_CellMeasuredResults = GN_Calloc(1, sizeof(CellMeasuredResults_t));

                                  s_CellMeasuredResult  *p_src_CellMeasuredResult = &p_src_MeasuredResult->p_CellMeasuredResultsList[CellMeasuredResultsCount];

                                  // I4 cellIdentity;              // INTEGER(0..268435455) OPTIONAL (-1 = Not present).
                                  if ( p_src_CellMeasuredResult->cellIdentity != -1 )
                                  {
                                      p_CellMeasuredResults->cellIdentity = GN_Calloc(1, sizeof(long));;
                                      *p_CellMeasuredResults->cellIdentity        = p_src_CellMeasuredResult->cellIdentity;
                                  }
                                  else
                                  {
                                     p_CellMeasuredResults->cellIdentity = NULL;
                                  }

                                  // e_modeSpecificInfo modeSpecificInfoType;  ///< Choice between FDD or TDD information.
                                  // union {
                                  //    s_MeasuredResultFDD  fdd;   ///< Frequency Division Duplexed mode Measured Result
                                  //    s_MeasuredResultTDD  tdd;   ///< Time Division Duplexed mode Measured Result
                                  // } of_type; ///< Union of either #s_MeasuredResultFDD or #s_MeasuredResultTDD result.
                                  switch ( p_src_CellMeasuredResult->modeSpecificInfoType )
                                  {
                                  case fdd:                 // Frequency Division Duplexed mode
                                      p_CellMeasuredResults->modeSpecificInfo.present = CellMeasuredResults__modeSpecificInfo_PR_fdd;

                                     // U2 PrimaryScramblingCode;  // INTEGER (0..511).
                                      p_CellMeasuredResults->modeSpecificInfo.choice.fdd.primaryCPICH_Info.primaryScramblingCode = p_src_CellMeasuredResult->of_type.fdd.PrimaryScramblingCode;

                                     // I1 cpich_Ec_N0;   // INTEGER  (0..63) OPTIONAL (-1 = Not Present). Values above 49 are spare.
                                     if ( p_src_CellMeasuredResult->of_type.fdd.cpich_Ec_N0 != -1 )
                                     {
                                         p_CellMeasuredResults->modeSpecificInfo.choice.fdd.cpich_Ec_N0 = GN_Calloc(1,sizeof(CPICH_Ec_N0_t));
                                        *p_CellMeasuredResults->modeSpecificInfo.choice.fdd.cpich_Ec_N0  = p_src_CellMeasuredResult->of_type.fdd.cpich_Ec_N0;
                                     }
                                     else
                                     {
                                        p_CellMeasuredResults->modeSpecificInfo.choice.fdd.cpich_Ec_N0 = NULL;
                                     }
                                     // I1 cpich_RSCP; // INTEGER (0..127) OPTIONAL (-1 = Not Present). Values above 91 are spare.
                                     if ( p_src_CellMeasuredResult->of_type.fdd.cpich_RSCP != -1 )
                                     {
                                         p_CellMeasuredResults->modeSpecificInfo.choice.fdd.cpich_RSCP  = GN_Calloc(1,sizeof(CPICH_RSCP_t));
                                        *p_CellMeasuredResults->modeSpecificInfo.choice.fdd.cpich_RSCP        = p_src_CellMeasuredResult->of_type.fdd.cpich_RSCP;
                                     }
                                     else
                                     {
                                        p_CellMeasuredResults->modeSpecificInfo.choice.fdd.cpich_RSCP = NULL;
                                     }
                                     // U1 pathloss;   // INTEGER(46..173) OPTIONAL ( 0 = Not Present). Values above 158 are spare.
                                     if ( p_src_CellMeasuredResult->of_type.fdd.pathloss != 0 )
                                     {
                                         p_CellMeasuredResults->modeSpecificInfo.choice.fdd.pathloss   = GN_Calloc(1,sizeof(Pathloss_t));
                                        *p_CellMeasuredResults->modeSpecificInfo.choice.fdd.pathloss          = p_src_CellMeasuredResult->of_type.fdd.pathloss;
                                     }
                                     else
                                     {
                                        p_CellMeasuredResults->modeSpecificInfo.choice.fdd.pathloss   = NULL;
                                     }
                                     break;
                                  case tdd:                 // Time Division Duplexed mode
                                      p_CellMeasuredResults->modeSpecificInfo.present = CellMeasuredResults__modeSpecificInfo_PR_tdd;
                                     // U1 cellParametersID;       // INTEGER (0..127).
                                      p_CellMeasuredResults->modeSpecificInfo.choice.tdd.cellParametersID      = p_src_CellMeasuredResult->of_type.tdd.cellParametersID;
                                     // I1 proposedTGSN;           // INTEGER  (0..14) OPTIONAL (-1 = Not Present).
                                     if ( p_src_CellMeasuredResult->of_type.tdd.proposedTGSN != -1 )
                                     {
                                         p_CellMeasuredResults->modeSpecificInfo.choice.tdd.proposedTGSN = GN_Calloc(1,sizeof(TGSN_t));
                                        *p_CellMeasuredResults->modeSpecificInfo.choice.tdd.proposedTGSN  = p_src_CellMeasuredResult->of_type.tdd.proposedTGSN;
                                     }
                                     else
                                     {
                                        p_CellMeasuredResults->modeSpecificInfo.choice.tdd.proposedTGSN = NULL;
                                     }
                                     // I1 primaryCCPCH_RSCP;      // INTEGER (0..127) OPTIONAL (-1 = Not Present).
                                     if ( p_src_CellMeasuredResult->of_type.tdd.primaryCCPCH_RSCP != -1 )
                                     {
                                         p_CellMeasuredResults->modeSpecificInfo.choice.tdd.primaryCCPCH_RSCP = GN_Calloc(1,sizeof(PrimaryCCPCH_RSCP_t));
                                        *p_CellMeasuredResults->modeSpecificInfo.choice.tdd.primaryCCPCH_RSCP        = p_src_CellMeasuredResult->of_type.tdd.primaryCCPCH_RSCP;
                                     }
                                     else
                                     {
                                        p_CellMeasuredResults->modeSpecificInfo.choice.tdd.primaryCCPCH_RSCP = NULL;
                                     }
                                     // U1 pathloss;               // INTEGER(46..173) OPTIONAL ( 0 = Not Present). Values above 158 are spare.
                                     if ( p_src_CellMeasuredResult->of_type.tdd.pathloss != 0 )
                                     {
                                         p_CellMeasuredResults->modeSpecificInfo.choice.tdd.pathloss   = GN_Calloc(1,sizeof(Pathloss_t));
                                        *p_CellMeasuredResults->modeSpecificInfo.choice.tdd.pathloss   = p_src_CellMeasuredResult->of_type.tdd.pathloss;
                                     }
                                     else
                                     {
                                        p_CellMeasuredResults->modeSpecificInfo.choice.tdd.pathloss = SDL_False;
                                     }
                                     // U1 timeslotISCP_Count;     // Number of entries in p_TimeslotISCP_List.
                                     if ( p_src_CellMeasuredResult->of_type.tdd.timeslotISCP_Count != 0 )
                                     {
                                        U1 TimeSlotISCPCount;

                                        TimeslotISCP_List_t *p_TimeslotISCP_List;
                                        p_CellMeasuredResults->modeSpecificInfo.choice.tdd.timeslotISCP_List = p_TimeslotISCP_List = GN_Calloc(1,sizeof(TimeslotISCP_List_t));
                                        for ( TimeSlotISCPCount = 0 ; TimeSlotISCPCount < p_src_CellMeasuredResult->of_type.tdd.timeslotISCP_Count ; TimeSlotISCPCount++ )
                                        {
                                           TimeslotISCP_t *p_TimeslotISCP = GN_Calloc( 1, sizeof( TimeslotISCP_t ) );
                                           *p_TimeslotISCP = p_src_CellMeasuredResult->of_type.tdd.p_TimeslotISCP_List[TimeSlotISCPCount] ;
                                           asn_sequence_add( &p_TimeslotISCP_List->list , p_TimeslotISCP );
                                        }
                                     }
                                     else
                                     {
                                        p_CellMeasuredResults->modeSpecificInfo.choice.tdd.timeslotISCP_List = NULL;
                                     }
                                     break;
                                  }
                                  asn_sequence_add( &p_MeasuredResults->cellMeasuredResultsList->list , p_CellMeasuredResults );
                               }
                            }
                            else
                            {
                               p_MeasuredResults->cellMeasuredResultsList = NULL;
                            }
                            // Save the current record in the linked list.
                            asn_sequence_add( &p_MeasuredResultsList->list , p_MeasuredResults );
                         }
                      }
                      else
                      {
                         p_LocationId->cellInfo.choice.wcdmaCell.measuredResultsList = NULL;
                      }
                  }
                  break;
               case CIT_cdmaCell:      // Cell information is from a CDMA network.
                  if( p_SupportedNetworkInfo != NULL && p_SupportedNetworkInfo->cDMA == FALSE )
                  {
                      GN_SUPL_Log( " CDMA Not Supported " );
                      Info_Supported = FALSE ;
                  }
                  else
                  {
                      p_LocationId->cellInfo.present = CellInfo_PR_cdmaCell;
                      p_LocationId->cellInfo.choice.cdmaCell.refNID         = p_SetLocationID->of_type.cdmaCellInfo.refNID;
                      p_LocationId->cellInfo.choice.cdmaCell.refSID         = p_SetLocationID->of_type.cdmaCellInfo.refSID;
                      p_LocationId->cellInfo.choice.cdmaCell.refBASEID      = p_SetLocationID->of_type.cdmaCellInfo.refBASEID;
                      p_LocationId->cellInfo.choice.cdmaCell.refBASELAT     = p_SetLocationID->of_type.cdmaCellInfo.refBASELAT;
                      p_LocationId->cellInfo.choice.cdmaCell.reBASELONG     = p_SetLocationID->of_type.cdmaCellInfo.reBASELONG;
                      p_LocationId->cellInfo.choice.cdmaCell.refREFPN       = p_SetLocationID->of_type.cdmaCellInfo.refREFPN;
                      p_LocationId->cellInfo.choice.cdmaCell.refSeconds     = p_SetLocationID->of_type.cdmaCellInfo.refSeconds;
                      p_LocationId->cellInfo.choice.cdmaCell.refWeekNumber  = p_SetLocationID->of_type.cdmaCellInfo.refWeekNumber;
                  }
                  break;
               }
            }
        }
        if( Info_Supported )
        {
            asn_sequence_add(&p_multipleLocationIds->list , p_LocationIdData );
            Mlid_Supported = TRUE ;
        }
        temp = temp->prev ;
    }

    return Mlid_Supported;
}


//*****************************************************************************
/// \brief
///      Function to add Multiple Location ID into an unencoded SUPL-TRIGGERED-START PDU.
///
/// \returns
///      Nothing.
//*****************************************************************************
static void supl_PDU_Add_SUPLTRIGGEREDSTART_Multiple_LocationID
(
   void *p_ThisPDU, ///< [in/out] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   s_SUPL_Instance *p_SUPL_Instance ///< [in] Current instance data used to
                                    ///   populate parts of the PDU.
                     
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;
   BL Network_Supported;

   p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTART.multipleLocationIds = (MultipleLocationIds_t*)GN_Calloc( 1, sizeof(MultipleLocationIds_t)*MaxLocIdSize);

   Network_Supported = supl_PDU_Add_Multiple_LocationID(p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTART.multipleLocationIds , p_SUPL_Instance  );

   if(Network_Supported == FALSE)
   {
        GN_SUPL_Log( " No Network Supported " );
        GN_Free(p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTART.multipleLocationIds);
        p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTART.multipleLocationIds = NULL;
   }
}

//*****************************************************************************
/// \brief
///      Function to add Multiple Location ID into an unencoded SUPL-POSINIT-START PDU.
///
/// \returns
///      Nothing.
//*****************************************************************************
static void supl_PDU_Add_SUPLPOSINIT_Multiple_LocationID
(
   void *p_ThisPDU, ///< [in/out] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   s_SUPL_Instance *p_SUPL_Instance ///< [in] Current instance data used to
                                    ///   populate parts of the PDU.
                     
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;
   BL Network_Supported;

   if ( p_PDU_Cast->message.choice.msSUPLPOSINIT.ver2_SUPL_POS_INIT_extension == NULL )
   p_PDU_Cast->message.choice.msSUPLPOSINIT.ver2_SUPL_POS_INIT_extension  = (Ver2_SUPL_POS_INIT_extension_t*)GN_Calloc( 1, sizeof(Ver2_SUPL_POS_INIT_extension_t));

   p_PDU_Cast->message.choice.msSUPLPOSINIT.ver2_SUPL_POS_INIT_extension->multipleLocationIds = (MultipleLocationIds_t*)GN_Calloc( 1, sizeof(MultipleLocationIds_t)*MaxLocIdSize);

   Network_Supported = supl_PDU_Add_Multiple_LocationID(p_PDU_Cast->message.choice.msSUPLPOSINIT.ver2_SUPL_POS_INIT_extension->multipleLocationIds , p_SUPL_Instance  );

   if(Network_Supported == FALSE)
   {
        GN_SUPL_Log( " No Network Supported " );
        GN_Free(p_PDU_Cast->message.choice.msSUPLPOSINIT.ver2_SUPL_POS_INIT_extension->multipleLocationIds);
        p_PDU_Cast->message.choice.msSUPLPOSINIT.ver2_SUPL_POS_INIT_extension->multipleLocationIds = NULL;
   }
}

//*****************************************************************************
/// \brief
///      Function to add Multiple Location ID into an unencoded SUPL-POSINIT-START PDU.
///
/// \returns
///      Nothing.
//*****************************************************************************
static void supl_PDU_Add_SUPLSTART_Multiple_LocationID
(
   void *p_ThisPDU, ///< [in/out] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   s_SUPL_Instance *p_SUPL_Instance ///< [in] Current instance data used to
                                    ///   populate parts of the PDU.
                     
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;
   BL Network_Supported;
   
   if ( p_PDU_Cast->message.choice.msSUPLSTART.ver2_SUPL_START_extension == NULL )
   p_PDU_Cast->message.choice.msSUPLSTART.ver2_SUPL_START_extension  = (Ver2_SUPL_START_extension_t*)GN_Calloc( 1, sizeof(Ver2_SUPL_START_extension_t));

   p_PDU_Cast->message.choice.msSUPLSTART.ver2_SUPL_START_extension->multipleLocationIds = (MultipleLocationIds_t*)GN_Calloc( 1, sizeof(MultipleLocationIds_t)*MaxLocIdSize);

   Network_Supported = supl_PDU_Add_Multiple_LocationID(p_PDU_Cast->message.choice.msSUPLSTART.ver2_SUPL_START_extension->multipleLocationIds , p_SUPL_Instance  );

   if(Network_Supported == FALSE)
   {
        GN_SUPL_Log( " No Network Supported " );
        GN_Free(p_PDU_Cast->message.choice.msSUPLSTART.ver2_SUPL_START_extension->multipleLocationIds);
        p_PDU_Cast->message.choice.msSUPLSTART.ver2_SUPL_START_extension->multipleLocationIds = NULL;
   }
}


//*****************************************************************************
/// \brief
///      Function to add the trigger periodic Info into asn1c format.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_TriggerParamsPeriodic(
   PeriodicParams_t       *periodicParam,
   s_GN_SUPL_V2_PeriodicTrigger *p_SetPeriodicTriggers
                                        )
{
    periodicParam->intervalBetweenFixes = p_SetPeriodicTriggers->IntervalBetweenFixes;
    periodicParam->numberOfFixes        = p_SetPeriodicTriggers->NumberOfFixes;

    if( p_SetPeriodicTriggers->StartTime != 0XFFFFFFFF)
    {
        periodicParam->startTime  = (long*)GN_Calloc( 1, sizeof( long ) );
        *periodicParam->startTime = (long)p_SetPeriodicTriggers->StartTime;
    }

    return;
}



//*****************************************************************************
/// \brief
///      Function to add the coordinate information of a point into asn1c format.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_Coordinate(
   Coordinate_t       *p_coordinateParam,
   s_GN_SUPL_V2_Coordinate *p_SetCoordinate
                                        )
{
    p_coordinateParam->latitude     = (long)p_SetCoordinate->latitude;
    p_coordinateParam->longitude    = (long)p_SetCoordinate->longitude;
    ASN1_ENUM_SET(p_coordinateParam->latitudeSign, p_SetCoordinate->latitudeSign);
}


//*****************************************************************************
/// \brief
///      Function to add the trigger area event Info into asn1c format.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_TriggerParamsAreaEvent(
   AreaEventParams_t       *areaParam,
   s_GN_SUPL_V2_AreaEventTrigger *p_SetAreaTriggers
                                        )
{
    U1 index = 0;

    ASN1_ENUM_SET(areaParam->areaEventType, p_SetAreaTriggers->areaEventType);

    /*From OMA specification OMA-TS-ULP-V2_0, 10.20.2.2 , "SET-Initiated triggered services this parameter is not useful"*/
    areaParam->locationEstimate = 0;

    areaParam->repeatedReportingParams = (RepeatedReportingParams_t*)GN_Calloc( 1 , sizeof(RepeatedReportingParams_t) );
    areaParam->repeatedReportingParams->maximumNumberOfReports = p_SetAreaTriggers->repeatedReportingParams.maximumNumberOfReports;
    areaParam->repeatedReportingParams->minimumIntervalTime    = p_SetAreaTriggers->repeatedReportingParams.minimumIntervalTime;

    if( p_SetAreaTriggers->startTime != 0XFFFFFFFF)
    {
        areaParam->startTime  = (long*)GN_Calloc( 1, sizeof( long ) );
        *areaParam->startTime = (long)p_SetAreaTriggers->startTime;
    }

    if( p_SetAreaTriggers->stopTime != 0XFFFFFFFF)
    {
        areaParam->stopTime   = (long*)GN_Calloc( 1, sizeof( long ) );
        *areaParam->stopTime  = (long)p_SetAreaTriggers->stopTime;
    }

    areaParam->geographicTargetAreaList = (GeographicTargetAreaList_t*)GN_Calloc( 1 , sizeof(GeographicTargetAreaList_t)*p_SetAreaTriggers->geographicTargetAreaCount );

    for( index = 0; index < p_SetAreaTriggers->geographicTargetAreaCount ; index++)
    {
        GeographicTargetArea_t* p_geographicTargetArea = (GeographicTargetArea_t*)GN_Calloc( 1 , sizeof(GeographicTargetArea_t) );

        switch( p_SetAreaTriggers->geographicTargetAreaParams[index].shapeType )
        {
            case GN_GeographicalTargetArea_Circlar :
                p_geographicTargetArea->present = GeographicTargetArea_PR_circularArea;
                supl_PDU_Add_Coordinate(&p_geographicTargetArea->choice.circularArea.coordinate,
                                        &p_SetAreaTriggers->geographicTargetAreaParams[index].choice.circularArea.center);
                p_geographicTargetArea->choice.circularArea.radius = (long)p_SetAreaTriggers->geographicTargetAreaParams[index].choice.circularArea.radius;

                if( p_SetAreaTriggers->geographicTargetAreaParams[index].choice.circularArea.radiusMin )
                {
                    p_geographicTargetArea->choice.circularArea.radius_min  = (long*)GN_Calloc( 1, sizeof( long ) );
                    *p_geographicTargetArea->choice.circularArea.radius_min = (long)p_SetAreaTriggers->geographicTargetAreaParams[index].choice.circularArea.radiusMin;
                }

                if( p_SetAreaTriggers->geographicTargetAreaParams[index].choice.circularArea.radiusMax )
                {
                    p_geographicTargetArea->choice.circularArea.radius_max  = (long*)GN_Calloc( 1, sizeof( long ) );
                    *p_geographicTargetArea->choice.circularArea.radius_max = (long)p_SetAreaTriggers->geographicTargetAreaParams[index].choice.circularArea.radiusMax;
                }
                break;

            case GN_GeographicalTargetArea_Ellipse :
                p_geographicTargetArea->present = GeographicTargetArea_PR_ellipticalArea;

                supl_PDU_Add_Coordinate(&p_geographicTargetArea->choice.ellipticalArea.coordinate,
                                        &p_SetAreaTriggers->geographicTargetAreaParams[index].choice.ellipticalArea.center);
                p_geographicTargetArea->choice.ellipticalArea.semiMajor = (long)p_SetAreaTriggers->geographicTargetAreaParams[index].choice.ellipticalArea.semiMajor;
                p_geographicTargetArea->choice.ellipticalArea.semiMinor = (long)p_SetAreaTriggers->geographicTargetAreaParams[index].choice.ellipticalArea.semiMinor;

                if( p_SetAreaTriggers->geographicTargetAreaParams[index].choice.ellipticalArea.semiMajorMax )
                {
                    p_geographicTargetArea->choice.ellipticalArea.semiMajor_max  = (long*)GN_Calloc( 1, sizeof( long ) );
                    *p_geographicTargetArea->choice.ellipticalArea.semiMajor_max = (long)p_SetAreaTriggers->geographicTargetAreaParams[index].choice.ellipticalArea.semiMajorMax;
                }

                if( p_SetAreaTriggers->geographicTargetAreaParams[index].choice.ellipticalArea.semiMajorMin )
                {
                    p_geographicTargetArea->choice.ellipticalArea.semiMajor_min  = (long*)GN_Calloc( 1, sizeof( long ) );
                    *p_geographicTargetArea->choice.ellipticalArea.semiMajor_min = (long)p_SetAreaTriggers->geographicTargetAreaParams[index].choice.ellipticalArea.semiMajorMin;
                }

                if( p_SetAreaTriggers->geographicTargetAreaParams[index].choice.ellipticalArea.semiMinorMax )
                {
                    p_geographicTargetArea->choice.ellipticalArea.semiMinor_max  = (long*)GN_Calloc( 1, sizeof( long ) );
                    *p_geographicTargetArea->choice.ellipticalArea.semiMinor_max = (long)p_SetAreaTriggers->geographicTargetAreaParams[index].choice.ellipticalArea.semiMinorMax;
                }

                if( p_SetAreaTriggers->geographicTargetAreaParams[index].choice.ellipticalArea.semiMinorMin )
                {
                    p_geographicTargetArea->choice.ellipticalArea.semiMinor_min  = (long*)GN_Calloc( 1, sizeof( long ) );
                    *p_geographicTargetArea->choice.ellipticalArea.semiMinor_min = (long)p_SetAreaTriggers->geographicTargetAreaParams[index].choice.ellipticalArea.semiMinorMin;
                }
                break;

            case GN_GeographicalTargetArea_Polygon :
            {
                U1 indexCoordinates = 0;
                p_geographicTargetArea->present = GeographicTargetArea_PR_polygonArea;

                for(;indexCoordinates < p_SetAreaTriggers->geographicTargetAreaParams[index].choice.polygonArea.numOfPoints; indexCoordinates++)
                {
                    Coordinate_t* p_coordinate = (Coordinate_t*)GN_Calloc( 1 , sizeof(Coordinate_t) );;

                    supl_PDU_Add_Coordinate( p_coordinate,
                                             &p_SetAreaTriggers->geographicTargetAreaParams[index].choice.polygonArea.coordinates[indexCoordinates]);


                    ASN_SEQUENCE_ADD( &p_geographicTargetArea->choice.polygonArea.polygonDescription.list, p_coordinate);
                }

                if( p_SetAreaTriggers->geographicTargetAreaParams[index].choice.polygonArea.polygonHysteresis )
                {
                    p_geographicTargetArea->choice.polygonArea.polygonHysteresis  = (long*)GN_Calloc( 1, sizeof( long ) );
                    *p_geographicTargetArea->choice.polygonArea.polygonHysteresis = (long)p_SetAreaTriggers->geographicTargetAreaParams[index].choice.polygonArea.polygonHysteresis;
                }
            }
             break;

            default:
                OS_ASSERT( 0 );
                break;
        }

        ASN_SEQUENCE_ADD(&areaParam->geographicTargetAreaList->list, p_geographicTargetArea);
    }

    /* TODO : Add Area ID list */
    //areaParam->areaIdLists = NULL;

    return;
}



//*****************************************************************************
/// \brief
///      Function to convert Set capabilities into the asn1c format.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SetCapabilities
(
   SETCapabilities_t      *p_asn1_SETCapabilities,///< [out] Set capabilities in
                                                  ///   asn1c format.
   s_GN_SetCapabilities   *p_SetCapabilities   ,  ///< [in] Set capabilities in
                                                  ///   GN format.
   s_SUPL_Instance *p_SUPL_Instance               ///  Added for Version check                                   
                                                  
)
{
   PosTechnology_t        *p_ASN1_posTech     = &p_asn1_SETCapabilities->posTechnology;
   s_GN_PosTechnology   *p_GN_posTech       = &p_SetCapabilities->PosTechnology;

   PosProtocol_t          *p_ASN1_PosProtocol = &p_asn1_SETCapabilities->posProtocol;
   s_GN_PosProtocol     *p_GN_PosProtocol   = &p_SetCapabilities->PosProtocol;

   GN_SUPL_Log( "supl_PDU_Add_SetCapabilities: Positioning technology agpsSETAssisted %s", p_GN_posTech->agpsSETassisted ? "    supported" : "not supported" );
   p_ASN1_posTech->agpsSETassisted  = p_GN_posTech->agpsSETassisted  ? SDL_True : SDL_False;
   GN_SUPL_Log( "supl_PDU_Add_SetCapabilities: Positioning technology    agpsSETBased %s", p_GN_posTech->agpsSETBased    ? "    supported" : "not supported" );
   p_ASN1_posTech->agpsSETBased     = p_GN_posTech->agpsSETBased     ? SDL_True : SDL_False;
   GN_SUPL_Log( "supl_PDU_Add_SetCapabilities: Positioning technology   autonomousGPS %s", p_GN_posTech->autonomousGPS   ? "    supported" : "not supported" );
   p_ASN1_posTech->autonomousGPS    = p_GN_posTech->autonomousGPS    ? SDL_True : SDL_False;
   GN_SUPL_Log( "supl_PDU_Add_SetCapabilities: Positioning technology            aFLT %s", p_GN_posTech->aFLT            ? "    supported" : "not supported" );
   p_ASN1_posTech->aFLT             = p_GN_posTech->aFLT             ? SDL_True : SDL_False;
   GN_SUPL_Log( "supl_PDU_Add_SetCapabilities: Positioning technology            eCID %s", p_GN_posTech->eCID            ? "    supported" : "not supported" );
   p_ASN1_posTech->eCID             = p_GN_posTech->eCID             ? SDL_True : SDL_False;
   GN_SUPL_Log( "supl_PDU_Add_SetCapabilities: Positioning technology            eOTD %s", p_GN_posTech->eOTD            ? "    supported" : "not supported" );
   p_ASN1_posTech->eOTD             = p_GN_posTech->eOTD             ? SDL_True : SDL_False;
   GN_SUPL_Log( "supl_PDU_Add_SetCapabilities: Positioning technology           oTDOA %s", p_GN_posTech->oTDOA           ? "    supported" : "not supported" );
   p_ASN1_posTech->oTDOA            = p_GN_posTech->oTDOA            ? SDL_True : SDL_False;


   GN_SUPL_Log( "supl_PDU_Add_SetCapabilities: Positioning protocol            tia801 %s", p_GN_PosProtocol->tia801      ? "    supported" : "not supported" );
   p_ASN1_PosProtocol->tia801       = p_GN_PosProtocol->tia801       ? SDL_True : SDL_False;
   GN_SUPL_Log( "supl_PDU_Add_SetCapabilities: Positioning protocol              rrlp %s", p_GN_PosProtocol->rrlp        ? "    supported" : "not supported" );
   p_ASN1_PosProtocol->rrlp         = p_GN_PosProtocol->rrlp         ? SDL_True : SDL_False;
   GN_SUPL_Log( "supl_PDU_Add_SetCapabilities: Positioning protocol               rrc %s", p_GN_PosProtocol->rrc         ? "    supported" : "not supported" );
   p_ASN1_PosProtocol->rrc          = p_GN_PosProtocol->rrc          ? SDL_True : SDL_False;

   switch (p_SetCapabilities->PrefMethod) {
   case GN_agpsSETassistedPreferred:
      GN_SUPL_Log( "supl_PDU_Add_SetCapabilities: Preferred method : agpsSETassistedPreferred" );
      ASN1_ENUM_SET(p_asn1_SETCapabilities->prefMethod,PrefMethod_agpsSETassistedPreferred);

      break;
   case GN_agpsSETBasedPreferred:
      GN_SUPL_Log( "supl_PDU_Add_SetCapabilities: Preferred method : agpsSETBasedPreferred" );
      ASN1_ENUM_SET(p_asn1_SETCapabilities->prefMethod,PrefMethod_agpsSETBasedPreferred);
      break;
   case GN_noPreference:
      GN_SUPL_Log( "supl_PDU_Add_SetCapabilities: Preferred method : noPreference" );
      ASN1_ENUM_SET(p_asn1_SETCapabilities->prefMethod,PrefMethod_noPreference);
      break;
   default:
      OS_ASSERT( 0 );
      break;
   }

   if( supl_Is_Version2( p_SUPL_Instance ) ) 
   {
      Ver2_SETCapabilities_extension_t *p_Ver2_SetCapabilities;

      GN_SUPL_Log( "supl_PDU_Add_SetCapabilities: ver2_Extension");
      p_asn1_SETCapabilities->ver2_SETCapabilities_extension = GN_Calloc( 1 , sizeof( *p_asn1_SETCapabilities->ver2_SETCapabilities_extension ) );

      p_Ver2_SetCapabilities = p_asn1_SETCapabilities->ver2_SETCapabilities_extension;

      /* Ver2-SETCapabilities-extension ::= SEQUENCE {
         serviceCapabilities ServiceCapabilities OPTIONAL,
         ...}
         */
      p_Ver2_SetCapabilities->serviceCapabilities = GN_Calloc( 1 , sizeof( ServiceCapabilities_t ) );

      /* ServiceCapabilities ::= SEQUENCE {
         servicesSupported ServicesSupported,
         reportingCapabilities ReportingCap OPTIONAL,
         eventTriggerCapabilities EventTriggerCapabilities OPTIONAL,
         sessionCapabilities SessionCapabilities,
         ...}*/

      /* ServicesSupported ::= SEQUENCE {
         periodicTrigger BOOLEAN,
         areaEventTrigger BOOLEAN,
         ...}*/
      GN_SUPL_Log( "supl_PDU_Add_SetCapabilities: PeriodicTrigger :        %s" ,
         p_SetCapabilities->V2_SetCapabilities.ServicesSupported.PeriodicTrigger ? "    supported" : "not supported");
      p_Ver2_SetCapabilities->serviceCapabilities->servicesSupported.periodicTrigger
                  = p_SetCapabilities->V2_SetCapabilities.ServicesSupported.PeriodicTrigger;
      GN_SUPL_Log( "supl_PDU_Add_SetCapabilities: areaEventTrigger:        %s" ,
         p_SetCapabilities->V2_SetCapabilities.ServicesSupported.EventTrigger ? "    supported" : "not supported");
      p_Ver2_SetCapabilities->serviceCapabilities->servicesSupported.areaEventTrigger
                  = p_SetCapabilities->V2_SetCapabilities.ServicesSupported.EventTrigger;

      /* SessionCapabilities ::= SEQUENCE {
         maxNumberTotalSessions INTEGER (1..128),
         maxNumberPeriodicSessions INTEGER (1..32),
         maxNumberTriggeredSessions INTEGER (1..32),
         ...}*/

      p_Ver2_SetCapabilities->serviceCapabilities->sessionCapabilities.maxNumberPeriodicSessions
                  = p_SetCapabilities->V2_SetCapabilities.SessionCapabilities.MaxPeriodicSessions;
      p_Ver2_SetCapabilities->serviceCapabilities->sessionCapabilities.maxNumberTotalSessions
                  = p_SetCapabilities->V2_SetCapabilities.SessionCapabilities.MaxTotalSessions;
      p_Ver2_SetCapabilities->serviceCapabilities->sessionCapabilities.maxNumberTriggeredSessions
                  = p_SetCapabilities->V2_SetCapabilities.SessionCapabilities.MaxTriggeredSession;

      /* @Todo : Pick this up from the correct config instead of hard coding as shown */
      /* ReportingCap ::= SEQUENCE {
         minInt INTEGER (1..3600), -- units in seconds
         maxInt INTEGER (1..1440) OPTIONAL, -- units in minutes
         repMode RepMode,
         batchRepCap BatchRepCap OPTIONAL, -- only used for batch and quasi real time reporting
         ...}*/
      p_Ver2_SetCapabilities->serviceCapabilities->reportingCapabilities = GN_Calloc(1,sizeof(ReportingCap_t));

      supl_PDU_Add_ReportingCapabilities( p_Ver2_SetCapabilities->serviceCapabilities->reportingCapabilities , p_SetCapabilities );

      /* EventTriggerCapabilities ::= SEQUENCE {
         geoAreaShapesSupported GeoAreaShapesSupported,
         maxNumGeoAreaSupported INTEGER (0..maxNumGeoArea) OPTIONAL,
         maxAreaIdListSupported INTEGER (0..maxAreaIdList) OPTIONAL,
         maxAreaIdSupportedPerList INTEGER (0..maxAreaId) OPTIONAL,
         ...}*/

      if( p_SetCapabilities->V2_SetCapabilities.ServicesSupported.EventTrigger )
      {
         p_Ver2_SetCapabilities->serviceCapabilities->eventTriggerCapabilities
            = GN_Calloc( 1 , sizeof( EventTriggerCapabilities_t ) );

         /* GeoAreaShapesSupported ::= SEQUENCE {
            ellipticalArea BOOLEAN,
            polygonArea BOOLEAN,
            ...} */
         p_Ver2_SetCapabilities->serviceCapabilities->eventTriggerCapabilities->geoAreaShapesSupported.ellipticalArea
            = p_SetCapabilities->V2_SetCapabilities.EventTriggerCapabilities.GeographicAreasSupported.Ellipse;
         p_Ver2_SetCapabilities->serviceCapabilities->eventTriggerCapabilities->geoAreaShapesSupported.polygonArea
            = p_SetCapabilities->V2_SetCapabilities.EventTriggerCapabilities.GeographicAreasSupported.Polygon;

         if( p_SetCapabilities->V2_SetCapabilities.EventTriggerCapabilities.MaxAreaIdList != (U1)-1 )
         {
            p_Ver2_SetCapabilities->serviceCapabilities->eventTriggerCapabilities->maxAreaIdListSupported = GN_Calloc( 1 , sizeof(long) );
            *p_Ver2_SetCapabilities->serviceCapabilities->eventTriggerCapabilities->maxAreaIdListSupported
               = p_SetCapabilities->V2_SetCapabilities.EventTriggerCapabilities.MaxAreaIdList;
         }

         if( p_SetCapabilities->V2_SetCapabilities.EventTriggerCapabilities.MaxAreaIdPerAreaIdList != (U1)-1 )
         {
            p_Ver2_SetCapabilities->serviceCapabilities->eventTriggerCapabilities->maxAreaIdSupportedPerList = GN_Calloc( 1 , sizeof(long) );
            *p_Ver2_SetCapabilities->serviceCapabilities->eventTriggerCapabilities->maxAreaIdSupportedPerList
               = p_SetCapabilities->V2_SetCapabilities.EventTriggerCapabilities.MaxAreaIdPerAreaIdList;
         }

         if( p_SetCapabilities->V2_SetCapabilities.EventTriggerCapabilities.MaxNumGeoArea != (U1)-1 )
         {
            p_Ver2_SetCapabilities->serviceCapabilities->eventTriggerCapabilities->maxNumGeoAreaSupported = GN_Calloc( 1 , sizeof(long) );
            *p_Ver2_SetCapabilities->serviceCapabilities->eventTriggerCapabilities->maxNumGeoAreaSupported
               = p_SetCapabilities->V2_SetCapabilities.EventTriggerCapabilities.MaxNumGeoArea;
         }
      }
   }

}


//*****************************************************************************
/// \brief
///      Function to add Set capabilities into an unencoded SUPL-START PDU.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLSTART_SetCapabilities
(
   void                 *p_ThisPDU,          ///< [in/out] Pointer to unencoded structure
                                             ///   ULP_PDU_t. Passed in as (void *) to hide
                                             ///   implementation.
   s_GN_SetCapabilities *p_SetCapabilities, ///< [in] Set capabilities in
                                             ///   GN format.
   s_SUPL_Instance *p_SUPL_Instance          ///  Added for Version check                             
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   supl_PDU_Add_SetCapabilities(
      &p_PDU_Cast->message.choice.msSUPLSTART.sETCapabilities,
      p_SetCapabilities , p_SUPL_Instance);
}



//*****************************************************************************
/// \brief
///      Function to retrieve a Set Location ID from a native asn1c format.
///
/// \returns
///      Nothing.
//*****************************************************************************
static void supl_PDU_Get_SetLocationID
(
   s_LocationId   *p_SetLocationID, ///< [out] Set Location ID.
   LocationId_t     *p_LocationId     ///< [in] Native asn1c LocationId format.
)
{
   switch ( ASN1_GET(ASN1_ENUM_1_GET(p_LocationId->status)))
   {
   case stale:         // Cell information is stale.
      p_SetLocationID->Status = CIS_stale;
      break;
   case current:       // Cell information is current.
      p_SetLocationID->Status = CIS_current;
      break;
   case unknown:       // Cell information is unknown.
      p_SetLocationID->Status = CIS_unknown;
      break;
   default :
       p_SetLocationID->Status = CIS_NONE;
   }

   switch ( p_LocationId->cellInfo.present )
   {
   case CellInfo_PR_gsmCell:       // Cell information is from a GSM network.
      p_SetLocationID->Type = CIT_gsmCell+1;

      p_SetLocationID->of_type.gsmCellInfo.refMCC = (U2)p_LocationId->cellInfo.choice.gsmCell.refMCC;          //->cellInfo.U.gsmCell.refMCC;
      p_SetLocationID->of_type.gsmCellInfo.refMNC = (U2)p_LocationId->cellInfo.choice.gsmCell.refMNC;
      p_SetLocationID->of_type.gsmCellInfo.refLAC = (U2)p_LocationId->cellInfo.choice.gsmCell.refLAC;
      p_SetLocationID->of_type.gsmCellInfo.refCI  = (U2)p_LocationId->cellInfo.choice.gsmCell.refCI;

      if ( p_LocationId->cellInfo.choice.gsmCell.tA != NULL)
      {
         p_SetLocationID->of_type.gsmCellInfo.tA = (I2)*p_LocationId->cellInfo.choice.gsmCell.tA;
      }
      else
      {
          p_SetLocationID->of_type.gsmCellInfo.tA = -1;
      }

      if ( p_LocationId->cellInfo.choice.gsmCell.nMR != NULL)
      {
         NMRelement_t *p_NMRElement;
           s_NMRElement *p_sNMRElement;
           int NMRCount = p_LocationId->cellInfo.choice.gsmCell.nMR->list.count;
           p_SetLocationID->of_type.gsmCellInfo.NMRCount = NMRCount;
           p_SetLocationID->of_type.gsmCellInfo.p_NMRList = p_sNMRElement = GN_Calloc( NMRCount, sizeof( s_NMRElement ) );

         for ( ;NMRCount > 0; NMRCount-- )
         {
            // Populate the record.
             p_NMRElement = p_LocationId->cellInfo.choice.gsmCell.nMR->list.array[NMRCount];
             p_sNMRElement->aRFCN = (U2)p_NMRElement->aRFCN;
             p_sNMRElement->bSIC  = (U1)p_NMRElement->bSIC;
             p_sNMRElement->rxLev = (U1)p_NMRElement->rxLev;

            // Set up pointer for next record in list.
            p_sNMRElement += 1;
         }

      }
      break;
   case CellInfo_PR_wcdmaCell:     // Cell information is from a WCDMA network.
      p_SetLocationID->Type = CIT_wcdmaCell+1;

      p_SetLocationID->of_type.wcdmaCellInfo.refMCC = (U2)p_LocationId->cellInfo.choice.wcdmaCell.refMCC;
      p_SetLocationID->of_type.wcdmaCellInfo.refMNC = (U2)p_LocationId->cellInfo.choice.wcdmaCell.refMNC;
      p_SetLocationID->of_type.wcdmaCellInfo.refUC  = (U4)p_LocationId->cellInfo.choice.wcdmaCell.refUC;
      if ( p_LocationId->cellInfo.choice.wcdmaCell.primaryScramblingCode != NULL)
      {
         p_SetLocationID->of_type.wcdmaCellInfo.primaryScramblingCode = (I2) *p_LocationId->cellInfo.choice.wcdmaCell.primaryScramblingCode;
      }
      else
      {
         p_SetLocationID->of_type.wcdmaCellInfo.primaryScramblingCode = -1;
      }
      if ( p_LocationId->cellInfo.choice.wcdmaCell.frequencyInfo != NULL )
      {
         p_SetLocationID->of_type.wcdmaCellInfo.p_FrequencyInfo = GN_Calloc( 1, sizeof( s_FrequencyInfo ) );
         switch ( p_LocationId->cellInfo.choice.wcdmaCell.frequencyInfo->modeSpecificInfo.present )
         {
         case FrequencyInfo__modeSpecificInfo_PR_fdd:
            p_SetLocationID->of_type.wcdmaCellInfo.p_FrequencyInfo->modeSpecificInfoType = fdd;
            p_SetLocationID->of_type.wcdmaCellInfo.p_FrequencyInfo->of_type.fdd.uarfcn_DL = (U2)p_LocationId->cellInfo.choice.wcdmaCell.frequencyInfo->modeSpecificInfo.choice.fdd.uarfcn_DL;
            if (p_LocationId->cellInfo.choice.wcdmaCell.frequencyInfo->modeSpecificInfo.choice.fdd.uarfcn_UL != NULL)
            {
            p_SetLocationID->of_type.wcdmaCellInfo.p_FrequencyInfo->of_type.fdd.uarfcn_UL =(I2) *p_LocationId->cellInfo.choice.wcdmaCell.frequencyInfo->modeSpecificInfo.choice.fdd.uarfcn_UL;
            }
            else
            {
                p_SetLocationID->of_type.wcdmaCellInfo.p_FrequencyInfo->of_type.fdd.uarfcn_UL=-1;
            }
            break;
         case FrequencyInfo__modeSpecificInfo_PR_tdd:
            p_SetLocationID->of_type.wcdmaCellInfo.p_FrequencyInfo->modeSpecificInfoType = tdd;
            p_SetLocationID->of_type.wcdmaCellInfo.p_FrequencyInfo->of_type.tdd.uarfcn_Nt = (U2)p_LocationId->cellInfo.choice.wcdmaCell.frequencyInfo->modeSpecificInfo.choice.tdd.uarfcn_Nt;
            break;
         default:
            break;
         }
      }
      else
      {
         p_SetLocationID->of_type.wcdmaCellInfo.p_FrequencyInfo = NULL;
      }
#if 0  /// \todo Fill in Measured results list (Only used by the server code).

       if ( p_LocationId->cellInfo.choice.wcdmaCell.measuredResultsList != NULL)
      {
         MeasuredResults_t *p_MeasuredResult;
         s_MeasuredResult *p_sMeasuredResult;
         p_sMeasuredResult = &p_SetLocationID->of_type.wcdmaCellInfo.p_MeasuredResultsList;
         int MeasuredResultCount = p_LocationId->cellInfo.choice.wcdmaCell.measuredResultsList->list.count;
         p_SetLocationID->of_type.wcdmaCellInfo.MeasuredResultCount = MeasuredResultCount;
         p_SetLocationID->of_type.wcdmaCellInfo.p_MeasuredResultsList = p_sMeasuredResult = GN_Calloc(MeasuredResultCount, sizeof( MeasuredResults ) );
        // p_MeasuredResult = *p_LocationId->cellInfo.choice.wcdmaCell.measuredResultsList->list.array;
         for(; MeasuredResultCount > 0 ; --MeasuredResultCount)
         {
             p_MeasuredResult = p_LocationId->cellInfo.choice.wcdmaCell.measuredResultsList->list.array[MeasuredResultCount];
             p_sMeasuredResult->utra_CarrierRSSI = p_MeasuredResult->utra_CarrierRSSI;
             if(p_MeasuredResult->frequencyInfo != NULL)
             {
                 p_sMeasuredResult->FrequencyInfoPresent = TRUE;
                 //p_sMeasuredResult->FrequencyInfo = GN_Calloc( 1, sizeof( s_FrequencyInfo ) );
                 switch (p_MeasuredResult->frequencyInfo->modeSpecificInfo.present)
                     {
                         case FrequencyInfo__modeSpecificInfo_PR_fdd:
                             p_sMeasuredResult->FrequencyInfo.modeSpecificInfoType = fdd;
                            p_sMeasuredResult->FrequencyInfo.of_type.fdd.uarfcn_DL = p_MeasuredResult->frequencyInfo->modeSpecificInfo.choice.fdd.uarfcn_DL;
                        if (p_MeasuredResult->frequencyInfo->modeSpecificInfo.choice.fdd.uarfcn_UL != NULL)
                            {
                            p_sMeasuredResult->FrequencyInfo.of_type.fdd.uarfcn_UL = *p_MeasuredResult->frequencyInfo->modeSpecificInfo.choice.fdd.uarfcn_UL;
                            }
                            else
                            {
                                p_sMeasuredResult->FrequencyInfo.of_type.fdd.uarfcn_UL=-1;
                            }
                            break;
                         case FrequencyInfo__modeSpecificInfo_PR_tdd:
                            p_sMeasuredResult->FrequencyInfo.modeSpecificInfoType = tdd;
                        p_sMeasuredResult->FrequencyInfo.of_type.tdd.uarfcn_Nt = p_MeasuredResult->frequencyInfo->modeSpecificInfo.choice.tdd.uarfcn_Nt;
                            break;
                     }
             }
            else
             {
               p_sMeasuredResult->FrequencyInfoPresent = FALSE;
             }
             if(p_MeasuredResult->cellMeasuredResultsList != NULL)
             {
                  CellMeasuredResults_t  *p_CellMeasuredResults;
                  int CellMeasuredResultsCount = p_MeasuredResult->cellMeasuredResultsList->list.count;
                  p_sMeasuredResult->CellMeasuredResultCount = CellMeasuredResultsCount;
                  p_sMeasuredResult->p_CellMeasuredResultsList = GN_Calloc( CellMeasuredResultsCount, sizeof( CellMeasuredResults ) );
                  for(; CellMeasuredResultsCount > 0 ; --CellMeasuredResultsCount)
                  {

                      p_CellMeasuredResults = p_MeasuredResult->cellMeasuredResultsList->list.array[CellMeasuredResultsCount];
                      if(p_CellMeasuredResults->cellIdentity != NULL)
                  {
                   p_sMeasuredResult->p_CellMeasuredResultsList->cellIdentity = *p_CellMeasuredResults->cellIdentity;
                  }
                  else
                  {
                       p_sMeasuredResult->p_CellMeasuredResultsList->cellIdentity = -1;
                  }
                  switch (p_CellMeasuredResults->modeSpecificInfo.present)
                     {
                         case CellMeasuredResults__modeSpecificInfo_PR_fdd:
                             p_sMeasuredResult->p_CellMeasuredResultsList->modeSpecificInfoType = fdd;
                             p_sMeasuredResult->p_CellMeasuredResultsList->of_type.fdd.PrimaryScramblingCode = p_CellMeasuredResults->modeSpecificInfo.choice.fdd.primaryCPICH_Info.primaryScramblingCode;
                             if (p_CellMeasuredResults->modeSpecificInfo.choice.fdd.cpich_Ec_N0 != NULL)
                            {
                                p_sMeasuredResult->p_CellMeasuredResultsList->of_type.fdd.cpich_Ec_N0 = *p_CellMeasuredResults->modeSpecificInfo.choice.fdd.cpich_Ec_N0;
                            }
                            else
                            {
                                p_sMeasuredResult->p_CellMeasuredResultsList->of_type.fdd.cpich_Ec_N0 = NULL;
                            }
                             if (p_CellMeasuredResults->modeSpecificInfo.choice.fdd.cpich_RSCP != NULL)
                            {
                                p_sMeasuredResult->p_CellMeasuredResultsList->of_type.fdd.cpich_RSCP = *p_CellMeasuredResults->modeSpecificInfo.choice.fdd.cpich_RSCP ;
                            }
                            else
                            {
                                p_sMeasuredResult->p_CellMeasuredResultsList->of_type.fdd.cpich_RSCP = NULL;
                            }
                             if (p_CellMeasuredResults->modeSpecificInfo.choice.fdd.pathloss != NULL)
                            {
                                p_sMeasuredResult->p_CellMeasuredResultsList->of_type.fdd.pathloss = *p_CellMeasuredResults->modeSpecificInfo.choice.fdd.pathloss;
                            }
                            else
                            {
                                p_sMeasuredResult->p_CellMeasuredResultsList->of_type.fdd.pathloss = NULL;
                            }
                            break;
                         case CellMeasuredResults__modeSpecificInfo_PR_tdd:
                             p_sMeasuredResult->p_CellMeasuredResultsList->modeSpecificInfoType = tdd;
                                 p_sMeasuredResult->p_CellMeasuredResultsList->of_type.tdd.cellParametersID = p_CellMeasuredResults->modeSpecificInfo.choice.tdd.cellParametersID;
                                 if (p_CellMeasuredResults->modeSpecificInfo.choice.tdd.pathloss != NULL)
                            {
                                p_sMeasuredResult->p_CellMeasuredResultsList->of_type.tdd.pathloss = *p_CellMeasuredResults->modeSpecificInfo.choice.tdd.pathloss;
                            }
                                 if (p_CellMeasuredResults->modeSpecificInfo.choice.tdd.primaryCCPCH_RSCP != NULL)
                            {
                                p_sMeasuredResult->p_CellMeasuredResultsList->of_type.tdd.primaryCCPCH_RSCP = *p_CellMeasuredResults->modeSpecificInfo.choice.tdd.primaryCCPCH_RSCP;
                            }
                                 if (p_CellMeasuredResults->modeSpecificInfo.choice.tdd.proposedTGSN != NULL)
                            {
                                p_sMeasuredResult->p_CellMeasuredResultsList->of_type.tdd.proposedTGSN = *p_CellMeasuredResults->modeSpecificInfo.choice.tdd.proposedTGSN;
                            }
                                 if( p_CellMeasuredResults->modeSpecificInfo.choice.tdd.timeslotISCP_List != NULL)
                             {
                               //TODO - Raghu Not Sure About This one
                                   TimeslotISCP_t *p_TimeslotISCP;
                               int TimeslotISCPCount = p_CellMeasuredResults->modeSpecificInfo.choice.tdd.timeslotISCP_List->list.count;
                                   p_sMeasuredResult->p_CellMeasuredResultsList->of_type.tdd.p_TimeslotISCP_List = /*p_TimeslotISCP_List =*/ GN_Calloc( TimeslotISCPCount, sizeof( TimeslotISCP_t ) );
                               p_sMeasuredResult->p_CellMeasuredResultsList->of_type.tdd.timeslotISCP_Count =  TimeslotISCPCount;
                                   for(; TimeslotISCPCount > 0 ; --TimeslotISCPCount)
                                   {
                                     p_TimeslotISCP = p_CellMeasuredResults->modeSpecificInfo.choice.tdd.timeslotISCP_List->list.array[TimeslotISCPCount];
                               p_sMeasuredResult->p_CellMeasuredResultsList->of_type.tdd.p_TimeslotISCP_List = p_TimeslotISCP;
                             }
                                 }
                           break;
                   }

            }
            }
         else
          {
            p_sMeasuredResult->p_CellMeasuredResultsList = NULL;
          }
        }
       }
      else
       {
                  p_sMeasuredResult=NULL;
      }
#endif // /// \todo Fill in Measured results list (Only used by the server code).
      break;
    case CellInfo_PR_cdmaCell:      // Cell information is from a CDMA network.
      p_SetLocationID->Type = CIT_cdmaCell+1;
      p_SetLocationID->of_type.cdmaCellInfo.reBASELONG      = p_LocationId->cellInfo.choice.cdmaCell.reBASELONG;
      p_SetLocationID->of_type.cdmaCellInfo.refBASEID       = (U2)p_LocationId->cellInfo.choice.cdmaCell.refBASEID;
      p_SetLocationID->of_type.cdmaCellInfo.refNID          = (U2)p_LocationId->cellInfo.choice.cdmaCell.refNID;
      p_SetLocationID->of_type.cdmaCellInfo.refREFPN        = (U2)p_LocationId->cellInfo.choice.cdmaCell.refREFPN;
      p_SetLocationID->of_type.cdmaCellInfo.refSeconds      = (U2)p_LocationId->cellInfo.choice.cdmaCell.refSeconds;
      p_SetLocationID->of_type.cdmaCellInfo.refSID          = (U2)p_LocationId->cellInfo.choice.cdmaCell.refSID;
      p_SetLocationID->of_type.cdmaCellInfo.refWeekNumber   = (U2)p_LocationId->cellInfo.choice.cdmaCell.refWeekNumber;
      break;
    default:
       break;
   }
}


//*****************************************************************************
/// \brief
///      Function to convert a Set Location ID into an native asn1c format.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SetLocationID
(
   LocationId_t     *p_LocationId,    ///< [out] Native asn1c LocationId format.
   s_LocationId   *p_SetLocationID  ///< [in] Set Location ID.
)
{
    BL eCID_Supported = supl_config_get_SUPL_eCID();
    if(p_SetLocationID)
    {
        if((p_SetLocationID->Type  == CIT_AccessPoint_WLAN)||
           (p_SetLocationID->Type  == CIT_AccessPoint_WIMAX))
        {

            uint16_t v_MS_Addr;
            uint32_t v_LS_Addr;
            switch(p_SetLocationID->Status)
            {
                case CIS_stale:
                    ASN1_ENUM_SET(p_LocationId->status,stale);//Access Point Information is Stale
                    break;
                case CIS_current:
                    ASN1_ENUM_SET(p_LocationId->status,current);//Access Point Information is Current
                    break;
                case CIS_unknown:
                    ASN1_ENUM_SET(p_LocationId->status,unknown);//Access Point Information is Unknown
                    break;
                /* Remove warnings */
                case CIS_NONE:
                default:
                    break;
            }
            switch(p_SetLocationID->Type)
            {
                case CIT_AccessPoint_WLAN:
                    /*WLAN Access Point Wireless network is present*/
                    /*Populate the WLAN AP data on to the PDU*/
                    v_MS_Addr = p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_MS_Addr;
                    v_LS_Addr = p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_LS_Addr;
                    p_LocationId->cellInfo.present = CellInfo_PR_ver2_CellInfo_extension;
                    p_LocationId->cellInfo.choice.ver2_CellInfo_extension.present = Ver2_CellInfo_extension_PR_wlanAP;
                    p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apMACAddress.size = sizeof(v_LS_Addr )+sizeof( v_MS_Addr);
                    p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apMACAddress.buf = (uint8_t*)GN_Calloc( 1, (p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apMACAddress.size )+1 );

                    memcpy(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apMACAddress.buf,
                           &( p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_LS_Addr),
                           sizeof(p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_LS_Addr));

                    memcpy((p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apMACAddress.buf+ sizeof(v_LS_Addr)),
                           &(p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_MS_Addr),
                           sizeof(p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_MS_Addr));

                    p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apMACAddress.buf[sizeof(v_MS_Addr+v_LS_Addr)+1] = '\0';

                    p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apTransmitPower = (long *)GN_Calloc( 1, sizeof( long ) );
                    *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apTransmitPower ) = (long)p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_TransmitPower;

                    p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apAntennaGain = (long *)GN_Calloc( 1, sizeof( long ) );
                    *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apAntennaGain ) = (long)p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_AntennaGain;


                    p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apSignaltoNoise = (long *)GN_Calloc( 1, sizeof( long ) );
                    *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apSignaltoNoise ) = (long)p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SNR;

                    p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apDeviceType = (ENUMERATED_t *)GN_Calloc( 1, sizeof( ENUMERATED_t ) );

                    p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apDeviceType->size = sizeof(p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_DeviceType);
                    p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apDeviceType->buf  = (uint8_t *)GN_Calloc( 1, p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apDeviceType->size );
                    memcpy(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apDeviceType->buf,
                           &(p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_DeviceType),
                           p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apDeviceType->size);

                    p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apSignalStrength = (long *)GN_Calloc( 1, sizeof( long ) );
                    *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apSignalStrength ) = (long)p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SignalStrength;

                    p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apChannelFrequency = (long *)GN_Calloc( 1, sizeof( long ) );
                    *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apChannelFrequency ) = (long)p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_Channel;

                    p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apRoundTripDelay = (RTD_t *)GN_Calloc( 1, sizeof( RTD_t ) );
                    p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apRoundTripDelay->rTDValue = p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_RTDValue;
                    p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apRoundTripDelay->rTDAccuracy = (long *)GN_Calloc( 1, sizeof( long ) );
                    *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apRoundTripDelay->rTDAccuracy ) = p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_Accuracy;
                    p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apRoundTripDelay->rTDUnits.size = sizeof(p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_RTDUnits);
                    p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apRoundTripDelay->rTDUnits.buf  = (uint8_t *)GN_Calloc(1, p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apRoundTripDelay->rTDUnits.size);
                    memcpy(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apRoundTripDelay->rTDUnits.buf,
                           &(p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_RTDUnits),
                           p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apRoundTripDelay->rTDUnits.size);


                    p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.setTransmitPower = (long *)GN_Calloc( 1, sizeof( long ) );
                    *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.setTransmitPower ) = (long)p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETTransmitPower;


                    p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.setAntennaGain = (long *)GN_Calloc( 1, sizeof( long ) );
                    *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.setAntennaGain ) = (long)p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETAntennaGain;

                    p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.setSignaltoNoise = (long *)GN_Calloc( 1, sizeof( long ) );
                    *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.setSignaltoNoise ) = (long)p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETSNR;


                    p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.setSignalStrength = (long *)GN_Calloc( 1, sizeof( long ) );
                    *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.setSignalStrength ) = (long)p_SetLocationID->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETSignalStrength;

                    GN_SUPL_Log( "supl_PDU_Add_SetLocationID: WlanAccessPointInfo.v_MS_Addr %d", 
                                  v_MS_Addr);
                    GN_SUPL_Log( "supl_PDU_Add_SetLocationID: WlanAccessPointInfo.v_LS_Addr %d",
                                  v_LS_Addr);

                    GN_SUPL_Log( "supl_PDU_Add_SetLocationID: WlanAccessPointInfo.v_Accuracy %d",
                                  *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apRoundTripDelay->rTDAccuracy ));
                    GN_SUPL_Log( "supl_PDU_Add_SetLocationID: WlanAccessPointInfo.v_AntennaGain %d",
                                  *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apAntennaGain ));
                    GN_SUPL_Log( "supl_PDU_Add_SetLocationID: WlanAccessPointInfo.v_Channel %d",
                                  *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apChannelFrequency ));
                    GN_SUPL_Log( "supl_PDU_Add_SetLocationID: WlanAccessPointInfo.v_DeviceType %s",
                                  p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apDeviceType->buf);

                    GN_SUPL_Log( "supl_PDU_Add_SetLocationID: WlanAccessPointInfo.v_RTDUnits %s",
                                  p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apRoundTripDelay->rTDUnits.buf);
                    GN_SUPL_Log( "supl_PDU_Add_SetLocationID: WlanAccessPointInfo.v_RTDValue %d",
                                  p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apRoundTripDelay->rTDValue);

                    GN_SUPL_Log( "supl_PDU_Add_SetLocationID: WlanAccessPointInfo.v_SETAntennaGain %d",
                                  *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.setAntennaGain ));
                    GN_SUPL_Log( "supl_PDU_Add_SetLocationID: WlanAccessPointInfo.v_SETSignalStrength %d",
                                  *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.setSignalStrength ));
                    GN_SUPL_Log( "supl_PDU_Add_SetLocationID: WlanAccessPointInfo.v_SETSNR %d",
                                  *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.setSignaltoNoise ));
                    GN_SUPL_Log( "supl_PDU_Add_SetLocationID: WlanAccessPointInfo.v_SETTransmitPower %d",
                                  *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.setTransmitPower ));
                    GN_SUPL_Log( "supl_PDU_Add_SetLocationID: WlanAccessPointInfo.v_SignalStrength %d",
                                  *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apSignalStrength ));
                    GN_SUPL_Log( "supl_PDU_Add_SetLocationID: WlanAccessPointInfo.v_SNR %d",
                                 *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apSignaltoNoise ));
                    GN_SUPL_Log( "supl_PDU_Add_SetLocationID: WlanAccessPointInfo.v_TransmitPower %d",
                                  *(p_LocationId->cellInfo.choice.ver2_CellInfo_extension.choice.wlanAP.apTransmitPower ));


                    /*Doubt apReportedLocation is not present in the SUPL instance so as to be populated in the PDU where from can we get this?*/
                    break;
                case CIT_AccessPoint_WIMAX:
                    break;
                case CIT_AccessPoint_UNKNOWN:
                    break;
                default:
                    break;
            }

        }
        else if((p_SetLocationID->Type == CIT_gsmCell)||(p_SetLocationID->Type == CIT_wcdmaCell)||(p_SetLocationID->Type == CIT_cdmaCell))
        {

           switch ( p_SetLocationID->Status )
           {
           case CIS_stale:         // Cell information is stale.
                ASN1_ENUM_SET(p_LocationId->status,stale);
                break;
           case CIS_current:       // Cell information is current.
                ASN1_ENUM_SET(p_LocationId->status,current);
                break;
           case CIS_unknown:       // Cell information is unknown.
                ASN1_ENUM_SET(p_LocationId->status,unknown);
                break;
           /* Remove warnings */
           case CIS_NONE:
           default :
              break;
           }
           switch ( p_SetLocationID->Type )
           {
           case CIT_gsmCell:       // Cell information is from a GSM network.
               p_LocationId->cellInfo.present = CellInfo_PR_gsmCell;

               p_LocationId->cellInfo.choice.gsmCell.refMCC = p_SetLocationID->of_type.gsmCellInfo.refMCC;
              p_LocationId->cellInfo.choice.gsmCell.refMNC = p_SetLocationID->of_type.gsmCellInfo.refMNC;
              p_LocationId->cellInfo.choice.gsmCell.refLAC = p_SetLocationID->of_type.gsmCellInfo.refLAC;
              p_LocationId->cellInfo.choice.gsmCell.refCI  = p_SetLocationID->of_type.gsmCellInfo.refCI;
              if ( p_SetLocationID->of_type.gsmCellInfo.tA != -1 )
              {
                 //p_LocationId->cellInfo.U.gsmCell.tAPresent = SDL_True;
                  p_LocationId->cellInfo.choice.gsmCell.tA = (long*)GN_Calloc( 1, sizeof( long ) );
                  *p_LocationId->cellInfo.choice.gsmCell.tA = (long)p_SetLocationID->of_type.gsmCellInfo.tA;
              }
              if ( eCID_Supported && p_SetLocationID->of_type.gsmCellInfo.NMRCount != 0 )
              {
                  NMR_t **p_p_Nmr;
                  NMRelement_t *nmr_one_element;
                  int NMRCount;
                  NMR_t *p_Nmr =NULL;

                 // We have more than 0 entries so we need to set this as present.
                 //p_LocationId->cellInfo.U.gsmCell.nMRPresent = SDL_True;
                 // Use p_p_thisNMR as a pointer to storage for the current NMR to be set after the record is completed.
                 p_p_Nmr = &p_LocationId->cellInfo.choice.gsmCell.nMR; //= (NMR_t*)GN_Calloc( 1, sizeof( NMR_t ) );

                if (ASN1_GET(p_p_Nmr) == NULL)
                {
                  SUPL_HEAP_ALLOC(ASN1_SET(p_p_Nmr), NMR_t);

                  if (ASN1_GET(p_p_Nmr) == NULL)
                  {
                       GN_SUPL_Log( "supl_PDU_Add_SetLocationID:Heap allocation error nmr_t");
                  }
                }
                    p_Nmr = ASN1_GET(p_p_Nmr);
                    ASN1_SEQ_SIZE(p_Nmr, 0);
                      p_Nmr->ASN1_SEQ_PTR = GN_Calloc( 1, sizeof( char* ) );
                 // Allocate memory for the record.
                // p_NMRelement = GN_Calloc( 1, sizeof( NMRelement_t ) * p_SetLocationID->of_type.gsmCellInfo.NMRCount);
                // nmr_one_element = p_NMRelement;
                 //p_Nmr = &p_LocationId->cellInfo.choice.gsmCell.nMR;
                 for ( NMRCount = 0;
                       NMRCount < p_SetLocationID->of_type.gsmCellInfo.NMRCount &&
                       NMRCount <= 15; // NMR can have upto 15 entries. Just ignore the rest.
                       NMRCount++ )
                 {

                    // Populate the record.
                    p_Nmr->list.array[NMRCount] = GN_Calloc( 1, sizeof( NMRelement_t ));
                     nmr_one_element = p_Nmr->list.array[NMRCount];
                    nmr_one_element->aRFCN = p_SetLocationID->of_type.gsmCellInfo.p_NMRList[NMRCount].aRFCN;
                    nmr_one_element->bSIC  = p_SetLocationID->of_type.gsmCellInfo.p_NMRList[NMRCount].bSIC;
                    nmr_one_element->rxLev = p_SetLocationID->of_type.gsmCellInfo.p_NMRList[NMRCount].rxLev;

                 }

                 // Set the length field to the count or 15, whichever is smaller.
                  //assign nmr
                p_Nmr->ASN1_SEQ_COUNT = NMRCount;
               // ASN1_SET(p_Nmr->ASN1_SEQ_PTR) = p_NMRelement;
                //p_Nmr->list.array = p_NMRelement;
                ASN1_SEQ_SIZE(p_Nmr, sizeof(NMRelement_t) * NMRCount);
                //ASN1_SET(p_p_Nmr) = p_Nmr;

              }
              else
              {
                  p_LocationId->cellInfo.choice.gsmCell.nMR =NULL;
              }
              break;
           case CIT_wcdmaCell:     // Cell information is from a WCDMA network.
              p_LocationId->cellInfo.present = CellInfo_PR_wcdmaCell;
              // U2 refMCC;                    // INTEGER(0..999), -- Mobile Country Code
              p_LocationId->cellInfo.choice.wcdmaCell.refMCC = p_SetLocationID->of_type.wcdmaCellInfo.refMCC;
              // U2 refMNC;                    // INTEGER(0..999), -- Mobile Network Code
              p_LocationId->cellInfo.choice.wcdmaCell.refMNC = p_SetLocationID->of_type.wcdmaCellInfo.refMNC;
              // U4 refUC;                     // INTEGER(0..268435455), -- Cell identity
              p_LocationId->cellInfo.choice.wcdmaCell.refUC  = p_SetLocationID->of_type.wcdmaCellInfo.refUC;
              // I2 primaryScramblingCode;     // INTEGER(0..511) OPTIONAL (-1 = not present).,
              if ( p_SetLocationID->of_type.wcdmaCellInfo.primaryScramblingCode != -1 )
              {
                // p_LocationId->cellInfo.U.wcdmaCell.primaryScramblingCodePresent = SDL_True;
                  p_LocationId->cellInfo.choice.wcdmaCell.primaryScramblingCode = (long*)GN_Calloc( 1, sizeof( long ) );
                  *p_LocationId->cellInfo.choice.wcdmaCell.primaryScramblingCode = p_SetLocationID->of_type.wcdmaCellInfo.primaryScramblingCode;
              }
              else
              {
                 p_LocationId->cellInfo.choice.wcdmaCell.primaryScramblingCode = NULL;
              }
              // s_FrequencyInfo*
              //    p_FrequencyInfo;           // FrequencyInfo OPTIONAL
              if ( p_SetLocationID->of_type.wcdmaCellInfo.p_FrequencyInfo != NULL )
              {
                 //p_LocationId->cellInfo.U.wcdmaCell.frequencyInfo1Present = SDL_True;
                  //allocate memory for asn1 frequency ino
                  p_LocationId->cellInfo.choice.wcdmaCell.frequencyInfo = (FrequencyInfo_t*)GN_Calloc( 1, sizeof( FrequencyInfo_t) );
                 /// Fill in Frequency Info.
                 switch ( p_SetLocationID->of_type.wcdmaCellInfo.p_FrequencyInfo->modeSpecificInfoType )
                 {
                 case fdd:               // Frequency Division Duplexed mode
                     p_LocationId->cellInfo.choice.wcdmaCell.frequencyInfo->modeSpecificInfo.present = FrequencyInfo__modeSpecificInfo_PR_fdd;
                     p_LocationId->cellInfo.choice.wcdmaCell.frequencyInfo->modeSpecificInfo.choice.fdd.uarfcn_DL = p_SetLocationID->of_type.wcdmaCellInfo.p_FrequencyInfo->of_type.fdd.uarfcn_DL;
                    if ( p_SetLocationID->of_type.wcdmaCellInfo.p_FrequencyInfo->of_type.fdd.uarfcn_UL != -1 )
                    {
                        p_LocationId->cellInfo.choice.wcdmaCell.frequencyInfo->modeSpecificInfo.choice.fdd.uarfcn_UL = (UARFCN_t*)GN_Calloc( 1, sizeof( UARFCN_t ) );
                        *p_LocationId->cellInfo.choice.wcdmaCell.frequencyInfo->modeSpecificInfo.choice.fdd.uarfcn_UL = p_SetLocationID->of_type.wcdmaCellInfo.p_FrequencyInfo->of_type.fdd.uarfcn_UL;
                    }
                    else
                    {
                       p_LocationId->cellInfo.choice.wcdmaCell.frequencyInfo->modeSpecificInfo.choice.fdd.uarfcn_UL = NULL;
                        //p_LocationId->cellInfo.U.wcdmaCell.frequencyInfo1.modeSpecificInfo.U.fdd1.uarfcn_ULPresent = SDL_False ;
                    }
                    break;
                 case tdd:               // Time Division Duplexed mode
                    p_LocationId->cellInfo.choice.wcdmaCell.frequencyInfo->modeSpecificInfo.present = FrequencyInfo__modeSpecificInfo_PR_tdd;
                    p_LocationId->cellInfo.choice.wcdmaCell.frequencyInfo->modeSpecificInfo.choice.tdd.uarfcn_Nt = p_SetLocationID->of_type.wcdmaCellInfo.p_FrequencyInfo->of_type.tdd.uarfcn_Nt;
                    break;
                 default:
                    OS_ASSERT(0);
                    break;
                 }
              }
              else
              {
                  p_LocationId->cellInfo.choice.wcdmaCell.frequencyInfo = NULL;
              }
              // U1 MeasuredResultCount;       // maxFreq INTEGER ::= 8
              if ( eCID_Supported && p_SetLocationID->of_type.wcdmaCellInfo.MeasuredResultCount != 0 )
              {
                 MeasuredResultsList_t *p_MeasuredResultsList;
                 U1 MeasuredResultCount;
                 U1 MeasuredResultTotal = p_SetLocationID->of_type.wcdmaCellInfo.MeasuredResultCount;

                 p_LocationId->cellInfo.choice.wcdmaCell.measuredResultsList = p_MeasuredResultsList = (MeasuredResultsList_t*)GN_Calloc(1,sizeof(MeasuredResultsList_t));
                 // s_MeasuredResult*
                 //    p_MeasuredResultsList;     // OPTIONAL (Present if MeasuredResultCount > 0). Pointer to array of s_MeasuredResult.
                 for ( MeasuredResultCount = 0;
                       MeasuredResultCount < MeasuredResultTotal &&
                       MeasuredResultCount < 8; // MeasuredResultList can have upto 8 entries. Just ignore the rest.
                       MeasuredResultCount++ )
                 {
                    // Allocate memory for the record.
                    MeasuredResults_t *p_MeasuredResults = GN_Calloc( 1, sizeof( MeasuredResults_t ) );
                    s_MeasuredResult  *p_src_MeasuredResult = &p_SetLocationID->of_type.wcdmaCellInfo.p_MeasuredResultsList[MeasuredResultCount];;
                    // Populate the record.
                    // I1 utra_CarrierRSSI;          // INTEGER(0..127) OPTIONAL, (-1 = Not present).
                    if ( p_src_MeasuredResult->utra_CarrierRSSI != -1 )
                    {
                       p_MeasuredResults->utra_CarrierRSSI = GN_Calloc(1,sizeof(UTRA_CarrierRSSI_t));;
                       *p_MeasuredResults->utra_CarrierRSSI        = p_src_MeasuredResult->utra_CarrierRSSI;
                    }
                    else
                    {
                       p_MeasuredResults->utra_CarrierRSSI = NULL;
                    }

                    // BL FrequencyInfoPresent;      // TRUE if FrequencyInfo is present.
                    if ( p_src_MeasuredResult->FrequencyInfoPresent )
                    {
                        p_MeasuredResults->frequencyInfo = (FrequencyInfo_t*)GN_Calloc( 1, sizeof( FrequencyInfo_t) );
                       // s_FrequencyInfo
                       //    FrequencyInfo;             // OPTIONAL (Present if FrequencyInfo == TRUE).
                       switch ( p_src_MeasuredResult->FrequencyInfo.modeSpecificInfoType )
                       {
                       case fdd:                 // Frequency Division Duplexed mode
                           p_MeasuredResults->frequencyInfo->modeSpecificInfo.present = FrequencyInfo__modeSpecificInfo_PR_fdd;
                          p_MeasuredResults->frequencyInfo->modeSpecificInfo.choice.fdd.uarfcn_DL = p_src_MeasuredResult->FrequencyInfo.of_type.fdd.uarfcn_DL;
                          if ( p_src_MeasuredResult->FrequencyInfo.of_type.fdd.uarfcn_UL != -1 )
                          {
                             p_MeasuredResults->frequencyInfo->modeSpecificInfo.choice.fdd.uarfcn_UL = (UARFCN_t*)GN_Calloc( 1, sizeof( UARFCN_t ) );
                             *p_MeasuredResults->frequencyInfo->modeSpecificInfo.choice.fdd.uarfcn_UL = p_src_MeasuredResult->FrequencyInfo.of_type.fdd.uarfcn_UL;
                          }
                          else
                          {
                             p_MeasuredResults->frequencyInfo->modeSpecificInfo.choice.fdd.uarfcn_UL = NULL ;
                          }
                          break;
                       case tdd:                 // Time Division Duplexed mode
                           p_MeasuredResults->frequencyInfo->modeSpecificInfo.present = FrequencyInfo__modeSpecificInfo_PR_tdd;
                           p_MeasuredResults->frequencyInfo->modeSpecificInfo.choice.tdd.uarfcn_Nt = p_src_MeasuredResult->FrequencyInfo.of_type.tdd.uarfcn_Nt;
                          break;
                       }
                    }
                    else
                    {
                       p_MeasuredResults->frequencyInfo = NULL;
                    }

                    // U1 CellMeasuredResultCount;   // Count of MeasuredResults up to (maxCellMeas INTEGER ::= 32)
                    if ( p_src_MeasuredResult->CellMeasuredResultCount > 0 )
                    {
                       U1 CellMeasuredResultsCount;
                       U1 CellMeasuredResultsTotal;
                       p_MeasuredResults->cellMeasuredResultsList = GN_Calloc(1,sizeof(CellMeasuredResultsList_t));

                       CellMeasuredResultsTotal = p_src_MeasuredResult->CellMeasuredResultCount;
                       // s_CellMeasuredResult*
                       //    p_CellMeasuredResultsList; // OPTIONAL (Present if CellMeasuredResultCount > 0) Pointer to an array of s_CellMeasuredResult.
                       for ( CellMeasuredResultsCount = 0;
                             CellMeasuredResultsCount < CellMeasuredResultsTotal &&
                             CellMeasuredResultsCount < 32; // CellMeasuredResultList can have upto 32 entries. Just ignore the rest.
                             CellMeasuredResultsCount++ )
                       {
                          // Allocate memory for the record.
                          CellMeasuredResults_t *p_CellMeasuredResults = GN_Calloc(1, sizeof(CellMeasuredResults_t));

                          s_CellMeasuredResult  *p_src_CellMeasuredResult = &p_src_MeasuredResult->p_CellMeasuredResultsList[CellMeasuredResultsCount];

                          // I4 cellIdentity;              // INTEGER(0..268435455) OPTIONAL (-1 = Not present).
                          if ( p_src_CellMeasuredResult->cellIdentity != -1 )
                          {
                              p_CellMeasuredResults->cellIdentity = GN_Calloc(1, sizeof(long));;
                              *p_CellMeasuredResults->cellIdentity        = p_src_CellMeasuredResult->cellIdentity;
                          }
                          else
                          {
                             p_CellMeasuredResults->cellIdentity = NULL;
                          }

                          // e_modeSpecificInfo modeSpecificInfoType;  ///< Choice between FDD or TDD information.
                          // union {
                          //    s_MeasuredResultFDD  fdd;   ///< Frequency Division Duplexed mode Measured Result
                          //    s_MeasuredResultTDD  tdd;   ///< Time Division Duplexed mode Measured Result
                          // } of_type; ///< Union of either #s_MeasuredResultFDD or #s_MeasuredResultTDD result.
                          switch ( p_src_CellMeasuredResult->modeSpecificInfoType )
                          {
                          case fdd:                 // Frequency Division Duplexed mode
                              p_CellMeasuredResults->modeSpecificInfo.present = CellMeasuredResults__modeSpecificInfo_PR_fdd;

                             // U2 PrimaryScramblingCode;  // INTEGER (0..511).
                              p_CellMeasuredResults->modeSpecificInfo.choice.fdd.primaryCPICH_Info.primaryScramblingCode = p_src_CellMeasuredResult->of_type.fdd.PrimaryScramblingCode;

                             // I1 cpich_Ec_N0;   // INTEGER  (0..63) OPTIONAL (-1 = Not Present). Values above 49 are spare.
                             if ( p_src_CellMeasuredResult->of_type.fdd.cpich_Ec_N0 != -1 )
                             {
                                 p_CellMeasuredResults->modeSpecificInfo.choice.fdd.cpich_Ec_N0 = GN_Calloc(1,sizeof(CPICH_Ec_N0_t));
                                *p_CellMeasuredResults->modeSpecificInfo.choice.fdd.cpich_Ec_N0  = p_src_CellMeasuredResult->of_type.fdd.cpich_Ec_N0;
                             }
                             else
                             {
                                p_CellMeasuredResults->modeSpecificInfo.choice.fdd.cpich_Ec_N0 = NULL;
                             }
                             // I1 cpich_RSCP; // INTEGER (0..127) OPTIONAL (-1 = Not Present). Values above 91 are spare.
                             if ( p_src_CellMeasuredResult->of_type.fdd.cpich_RSCP != -1 )
                             {
                                 p_CellMeasuredResults->modeSpecificInfo.choice.fdd.cpich_RSCP  = GN_Calloc(1,sizeof(CPICH_RSCP_t));
                                *p_CellMeasuredResults->modeSpecificInfo.choice.fdd.cpich_RSCP        = p_src_CellMeasuredResult->of_type.fdd.cpich_RSCP;
                             }
                             else
                             {
                                p_CellMeasuredResults->modeSpecificInfo.choice.fdd.cpich_RSCP = NULL;
                             }
                             // U1 pathloss;   // INTEGER(46..173) OPTIONAL ( 0 = Not Present). Values above 158 are spare.
                             if ( p_src_CellMeasuredResult->of_type.fdd.pathloss != 0 )
                             {
                                 p_CellMeasuredResults->modeSpecificInfo.choice.fdd.pathloss   = GN_Calloc(1,sizeof(Pathloss_t));
                                *p_CellMeasuredResults->modeSpecificInfo.choice.fdd.pathloss          = p_src_CellMeasuredResult->of_type.fdd.pathloss;
                             }
                             else
                             {
                                p_CellMeasuredResults->modeSpecificInfo.choice.fdd.pathloss   = NULL;
                             }
                             break;
                          case tdd:                 // Time Division Duplexed mode
                              p_CellMeasuredResults->modeSpecificInfo.present = CellMeasuredResults__modeSpecificInfo_PR_tdd;
                             // U1 cellParametersID;       // INTEGER (0..127).
                              p_CellMeasuredResults->modeSpecificInfo.choice.tdd.cellParametersID      = p_src_CellMeasuredResult->of_type.tdd.cellParametersID;
                             // I1 proposedTGSN;           // INTEGER  (0..14) OPTIONAL (-1 = Not Present).
                             if ( p_src_CellMeasuredResult->of_type.tdd.proposedTGSN != -1 )
                             {
                                 p_CellMeasuredResults->modeSpecificInfo.choice.tdd.proposedTGSN = GN_Calloc(1,sizeof(TGSN_t));
                                *p_CellMeasuredResults->modeSpecificInfo.choice.tdd.proposedTGSN  = p_src_CellMeasuredResult->of_type.tdd.proposedTGSN;
                             }
                             else
                             {
                                p_CellMeasuredResults->modeSpecificInfo.choice.tdd.proposedTGSN = NULL;
                             }
                             // I1 primaryCCPCH_RSCP;      // INTEGER (0..127) OPTIONAL (-1 = Not Present).
                             if ( p_src_CellMeasuredResult->of_type.tdd.primaryCCPCH_RSCP != -1 )
                             {
                                 p_CellMeasuredResults->modeSpecificInfo.choice.tdd.primaryCCPCH_RSCP = GN_Calloc(1,sizeof(PrimaryCCPCH_RSCP_t));
                                *p_CellMeasuredResults->modeSpecificInfo.choice.tdd.primaryCCPCH_RSCP        = p_src_CellMeasuredResult->of_type.tdd.primaryCCPCH_RSCP;
                             }
                             else
                             {
                                p_CellMeasuredResults->modeSpecificInfo.choice.tdd.primaryCCPCH_RSCP = NULL;
                             }
                             // U1 pathloss;               // INTEGER(46..173) OPTIONAL ( 0 = Not Present). Values above 158 are spare.
                             if ( p_src_CellMeasuredResult->of_type.tdd.pathloss != 0 )
                             {
                                 p_CellMeasuredResults->modeSpecificInfo.choice.tdd.pathloss   = GN_Calloc(1,sizeof(Pathloss_t));
                                *p_CellMeasuredResults->modeSpecificInfo.choice.tdd.pathloss   = p_src_CellMeasuredResult->of_type.tdd.pathloss;
                             }
                             else
                             {
                                p_CellMeasuredResults->modeSpecificInfo.choice.tdd.pathloss = SDL_False;
                             }
                             // U1 timeslotISCP_Count;     // Number of entries in p_TimeslotISCP_List.
                             if ( p_src_CellMeasuredResult->of_type.tdd.timeslotISCP_Count != 0 )
                             {
                                U1 TimeSlotISCPCount;

                                TimeslotISCP_List_t *p_TimeslotISCP_List;
                                p_CellMeasuredResults->modeSpecificInfo.choice.tdd.timeslotISCP_List = p_TimeslotISCP_List = GN_Calloc(1,sizeof(TimeslotISCP_List_t));
                                for ( TimeSlotISCPCount = 0 ; TimeSlotISCPCount < p_src_CellMeasuredResult->of_type.tdd.timeslotISCP_Count ; TimeSlotISCPCount++ )
                                {
                                   TimeslotISCP_t *p_TimeslotISCP = GN_Calloc( 1, sizeof( TimeslotISCP_t ) );
                                   *p_TimeslotISCP = p_src_CellMeasuredResult->of_type.tdd.p_TimeslotISCP_List[TimeSlotISCPCount] ;
                                   asn_sequence_add( &p_TimeslotISCP_List->list , p_TimeslotISCP );
                                }
                             }
                             else
                             {
                                p_CellMeasuredResults->modeSpecificInfo.choice.tdd.timeslotISCP_List = NULL;
                             }
                             break;
                          }
                          asn_sequence_add( &p_MeasuredResults->cellMeasuredResultsList->list , p_CellMeasuredResults );
                       }
                    }
                    else
                    {
                       p_MeasuredResults->cellMeasuredResultsList = NULL;
                    }
                    // Save the current record in the linked list.
                    asn_sequence_add( &p_MeasuredResultsList->list , p_MeasuredResults );
                 }
              }
              else
              {
                 p_LocationId->cellInfo.choice.wcdmaCell.measuredResultsList = NULL;
              }
              break;
           case CIT_cdmaCell:      // Cell information is from a CDMA network.
              p_LocationId->cellInfo.present = CellInfo_PR_cdmaCell;
              p_LocationId->cellInfo.choice.cdmaCell.refNID         = p_SetLocationID->of_type.cdmaCellInfo.refNID;
              p_LocationId->cellInfo.choice.cdmaCell.refSID         = p_SetLocationID->of_type.cdmaCellInfo.refSID;
              p_LocationId->cellInfo.choice.cdmaCell.refBASEID      = p_SetLocationID->of_type.cdmaCellInfo.refBASEID;
              p_LocationId->cellInfo.choice.cdmaCell.refBASELAT     = p_SetLocationID->of_type.cdmaCellInfo.refBASELAT;
              p_LocationId->cellInfo.choice.cdmaCell.reBASELONG     = p_SetLocationID->of_type.cdmaCellInfo.reBASELONG;
              p_LocationId->cellInfo.choice.cdmaCell.refREFPN       = p_SetLocationID->of_type.cdmaCellInfo.refREFPN;
              p_LocationId->cellInfo.choice.cdmaCell.refSeconds     = p_SetLocationID->of_type.cdmaCellInfo.refSeconds;
              p_LocationId->cellInfo.choice.cdmaCell.refWeekNumber  = p_SetLocationID->of_type.cdmaCellInfo.refWeekNumber;
              break;
           case CIT_AccessPoint_WIMAX:
              break;
           case CIT_AccessPoint_UNKNOWN:
              break;
           case CIT_AccessPoint_WLAN:
              break;
           default:
              break;
           }
        }
    }
}


void supl_PDU_Add_ThirdParty_Info(ThirdParty_t *p_Third_Party_Id, s_GN_SUPL_V2_ThirdPartyInfo *p_ThirdParty_info)
{

   //Need to populate the third party info in to this PDU
   if(p_Third_Party_Id == NULL)
   {
     // Got to print error message here
      return;
   }
   ThirdParty_t **p_p_ThirdParty_Id;
   ThirdPartyID_t *thirdparty_one_element;
   int NMRCount;
   ThirdParty_t *p_ThirdPartyinfo = NULL;

   GN_SUPL_Log( "supl_PDU_Add_ThirsPartyInfo: ver2_PosTechnology_extension");

   GN_SUPL_Log("supl_PDU_Add_ThirsPartyInfo: Third Party ID %d", p_ThirdParty_info->thirdPartyId);
   GN_SUPL_Log("supl_PDU_Add_ThirsPartyInfo: Third Party Name %s", p_ThirdParty_info->thirdPartyIdName);

   if(p_ThirdParty_info != NULL)
   {
            //p_Third_Party_Id->present = ThirdPartyID_PR_logicalName;
        if(p_ThirdParty_info->thirdPartyIdName != NULL)
        {
            p_p_ThirdParty_Id = &p_Third_Party_Id;

            if (ASN1_GET(p_p_ThirdParty_Id) == NULL)
            {
                SUPL_HEAP_ALLOC(ASN1_SET(p_p_ThirdParty_Id), ThirdParty_t);

                if (ASN1_GET(p_p_ThirdParty_Id) == NULL)
                {
                    GN_SUPL_Log( "supl_PDU_Add_SetLocationID:Heap allocation error ThirdParty_t");
                }
            }
            p_ThirdPartyinfo = ASN1_GET(p_p_ThirdParty_Id);
            ASN1_SEQ_SIZE(p_ThirdPartyinfo, 0);
            p_ThirdPartyinfo->ASN1_SEQ_PTR = GN_Calloc( 1, sizeof( char* ) );
            for ( NMRCount = 0; NMRCount <= 1;  NMRCount++ )
            {
             // Populate the record.
               p_ThirdPartyinfo->list.array[NMRCount] = GN_Calloc( 1, sizeof( ThirdPartyID_t));
               thirdparty_one_element = p_ThirdPartyinfo->list.array[NMRCount];
               switch((p_ThirdParty_info->thirdPartyId))
               {
                   case thirdparty_id_logicalName:
                       thirdparty_one_element->present = ThirdPartyID_PR_logicalName;//ThirdPartyID_PR_logicalName
                       thirdparty_one_element->choice.logicalName.buf = (unsigned char *)GN_Calloc(1, (strlen(p_ThirdParty_info->thirdPartyIdName)+1));

                       memcpy(thirdparty_one_element->choice.logicalName.buf,p_ThirdParty_info->thirdPartyIdName,strlen(p_ThirdParty_info->thirdPartyIdName));
                       thirdparty_one_element->choice.logicalName.size = strlen(p_ThirdParty_info->thirdPartyIdName);
                       thirdparty_one_element->choice.logicalName.buf[thirdparty_one_element->choice.logicalName.size + 1] = '\0';

                       GN_SUPL_Log("supl_PDU_Add_ThirsPartyInfo: Third Party ID Logical ID %d", thirdparty_one_element->present);
                       GN_SUPL_Log("supl_PDU_Add_ThirsPartyInfo: Third Party Logical Name %s", thirdparty_one_element->choice.logicalName.buf);

                   break;

                    case thirdparty_id_msisdn:
                        thirdparty_one_element->present = ThirdPartyID_PR_msisdn;
                        thirdparty_one_element->choice.msisdn.buf = (unsigned char *)GN_Calloc(1, (strlen(p_ThirdParty_info->thirdPartyIdName)+1));

                        memcpy(thirdparty_one_element->choice.msisdn.buf,p_ThirdParty_info->thirdPartyIdName,strlen(p_ThirdParty_info->thirdPartyIdName));
                        thirdparty_one_element->choice.msisdn.size = strlen(p_ThirdParty_info->thirdPartyIdName);
                        thirdparty_one_element->choice.msisdn.buf[thirdparty_one_element->choice.msisdn.size + 1] = '\0';

                        GN_SUPL_Log("supl_PDU_Add_ThirsPartyInfo: Third Party ID msisdn ID %d", thirdparty_one_element->present);
                        GN_SUPL_Log("supl_PDU_Add_ThirsPartyInfo: Third Party msisdn %s", thirdparty_one_element->choice.msisdn.buf);

                    break;


                    case thirdparty_id_emailaddr:
                        thirdparty_one_element->present = ThirdPartyID_PR_emailaddr;
                        thirdparty_one_element->choice.emailaddr.buf = (unsigned char *)GN_Calloc(1, (strlen(p_ThirdParty_info->thirdPartyIdName)+1));

                        memcpy(thirdparty_one_element->choice.emailaddr.buf,p_ThirdParty_info->thirdPartyIdName,strlen(p_ThirdParty_info->thirdPartyIdName));
                        thirdparty_one_element->choice.emailaddr.size = strlen(p_ThirdParty_info->thirdPartyIdName);
                        thirdparty_one_element->choice.emailaddr.buf[thirdparty_one_element->choice.emailaddr.size + 1] = '\0';

                        GN_SUPL_Log("supl_PDU_Add_ThirsPartyInfo: Third Party ID emailaddr ID %d", thirdparty_one_element->present);
                        GN_SUPL_Log("supl_PDU_Add_ThirsPartyInfo: Third Party emailaddr %s", thirdparty_one_element->choice.emailaddr.buf);

                    break;


                    case thirdparty_id_sip_uri:
                        thirdparty_one_element->present = ThirdPartyID_PR_sip_uri;
                        thirdparty_one_element->choice.sip_uri.buf = (unsigned char *)GN_Calloc(1, (strlen(p_ThirdParty_info->thirdPartyIdName)+1));

                        memcpy(thirdparty_one_element->choice.sip_uri.buf,p_ThirdParty_info->thirdPartyIdName,strlen(p_ThirdParty_info->thirdPartyIdName));
                        thirdparty_one_element->choice.sip_uri.size = strlen(p_ThirdParty_info->thirdPartyIdName);
                        thirdparty_one_element->choice.sip_uri.buf[thirdparty_one_element->choice.sip_uri.size + 1] = '\0';

                        GN_SUPL_Log("supl_PDU_Add_ThirsPartyInfo: Third Party ID sip_uri ID %d", thirdparty_one_element->present);
                        GN_SUPL_Log("supl_PDU_Add_ThirsPartyInfo: Third Party sip_uri %s", thirdparty_one_element->choice.sip_uri.buf);

                    break;


                    case thirdparty_id_ims_public_identity:
                        thirdparty_one_element->present = ThirdPartyID_PR_ims_public_identity;
                        thirdparty_one_element->choice.ims_public_identity.buf = (unsigned char *)GN_Calloc(1, (strlen(p_ThirdParty_info->thirdPartyIdName)+1));

                        memcpy(thirdparty_one_element->choice.ims_public_identity.buf,p_ThirdParty_info->thirdPartyIdName,strlen(p_ThirdParty_info->thirdPartyIdName));
                        thirdparty_one_element->choice.ims_public_identity.size = strlen(p_ThirdParty_info->thirdPartyIdName);
                        thirdparty_one_element->choice.ims_public_identity.buf[thirdparty_one_element->choice.ims_public_identity.size + 1] = '\0';

                        GN_SUPL_Log("supl_PDU_Add_ThirsPartyInfo: Third Party ID ims_public_identity ID %d", thirdparty_one_element->present);
                        GN_SUPL_Log("supl_PDU_Add_ThirsPartyInfo: Third Party ims_public_identity %s", thirdparty_one_element->choice.ims_public_identity.buf);

                    break;


                    case thirdparty_id_min:
                        thirdparty_one_element->present = ThirdPartyID_PR_min;
                        thirdparty_one_element->choice.min.buf = (unsigned char *)GN_Calloc(1, (strlen(p_ThirdParty_info->thirdPartyIdName)+1));

                        memcpy(thirdparty_one_element->choice.min.buf,p_ThirdParty_info->thirdPartyIdName,strlen(p_ThirdParty_info->thirdPartyIdName));
                        thirdparty_one_element->choice.min.size = strlen(p_ThirdParty_info->thirdPartyIdName);
                        thirdparty_one_element->choice.min.buf[thirdparty_one_element->choice.min.size + 1] = '\0';

                        GN_SUPL_Log("supl_PDU_Add_ThirsPartyInfo: Third Party ID min ID %d", thirdparty_one_element->present);
                        GN_SUPL_Log("supl_PDU_Add_ThirsPartyInfo: Third Party min %s", thirdparty_one_element->choice.min.buf);

                    break;


                    case thirdparty_id_mdn:
                        thirdparty_one_element->present = ThirdPartyID_PR_mdn;
                        thirdparty_one_element->choice.mdn.buf = (unsigned char *)GN_Calloc(1, (strlen(p_ThirdParty_info->thirdPartyIdName)+1));

                        memcpy(thirdparty_one_element->choice.mdn.buf,p_ThirdParty_info->thirdPartyIdName,strlen(p_ThirdParty_info->thirdPartyIdName));
                        thirdparty_one_element->choice.mdn.size = strlen(p_ThirdParty_info->thirdPartyIdName);
                        thirdparty_one_element->choice.mdn.buf[thirdparty_one_element->choice.mdn.size + 1] = '\0';

                        GN_SUPL_Log("supl_PDU_Add_ThirsPartyInfo: Third Party ID mdn ID %d", thirdparty_one_element->present);
                        GN_SUPL_Log("supl_PDU_Add_ThirsPartyInfo: Third Party mdn %s", thirdparty_one_element->choice.mdn.buf);

                    break;


                    case thirdparty_id_uri:
                        thirdparty_one_element->present = ThirdPartyID_PR_uri;
                        thirdparty_one_element->choice.uri.buf = (unsigned char *)GN_Calloc(1, (strlen(p_ThirdParty_info->thirdPartyIdName)+1));

                        memcpy(thirdparty_one_element->choice.uri.buf,p_ThirdParty_info->thirdPartyIdName,strlen(p_ThirdParty_info->thirdPartyIdName));
                        thirdparty_one_element->choice.uri.size = strlen(p_ThirdParty_info->thirdPartyIdName);
                        thirdparty_one_element->choice.uri.buf[thirdparty_one_element->choice.mdn.size + 1] = '\0';

                        GN_SUPL_Log("supl_PDU_Add_ThirsPartyInfo: Third Party ID uri ID %d", thirdparty_one_element->present);
                        GN_SUPL_Log("supl_PDU_Add_ThirsPartyInfo: Third Party uri %s", thirdparty_one_element->choice.uri.buf);

                    break;

                    default:

                        GN_SUPL_Log("supl_PDU_Add_ThirsPartyInfo: Invalid id %d", p_ThirdParty_info->thirdPartyId);
                    break;

             }

              p_ThirdPartyinfo->ASN1_SEQ_COUNT = NMRCount;
              ASN1_SEQ_SIZE(p_ThirdPartyinfo, sizeof(ThirdPartyID_t) * NMRCount);

          }

        }

   }
   else
   {
      GN_SUPL_Log("No ThirdParty Data in the SUPL instance");
   }

}

void supl_PDU_Add_TargetID_Info( SETId_t     *p_targetSETID, s_GN_SUPL_V2_ThirdPartyInfo *p_ThirdParty_info)
{

   GN_SUPL_Log( "supl_PDU_Add_TargetID_Info: Inside");


   if(p_ThirdParty_info != NULL)
   {

        GN_SUPL_Log("supl_PDU_Add_TargetID_Info: Target ID %d", p_ThirdParty_info->thirdPartyId);
        GN_SUPL_Log("supl_PDU_Add_TargetID_Info: Target ID Name %s", p_ThirdParty_info->thirdPartyIdName);
        if(p_ThirdParty_info->thirdPartyId == thirdparty_id_msisdn)
        {
            
              p_targetSETID->present = SETId_PR_msisdn;
              p_targetSETID->choice.msisdn.size = MSISDN_OCTET_LENGTH;
            //  p_targetSETID->choice.msisdn.size = strlen(p_ThirdParty_info->thirdPartyIdName);
              p_targetSETID->choice.msisdn.buf = (uint8_t*)GN_Calloc( 1, MSISDN_OCTET_LENGTH );
              memcpy(  p_targetSETID->choice.msisdn.buf,
                       p_ThirdParty_info->thirdPartyIdName,
                       p_targetSETID->choice.msisdn.size);
            
        }
        else if(p_ThirdParty_info->thirdPartyId == thirdparty_id_mdn)
        {
            
            p_targetSETID->present          = SETId_PR_mdn;
            p_targetSETID->choice.mdn.size      = MDN_OCTET_LENGTH;
            p_targetSETID->choice.mdn.buf        = (uint8_t*)GN_Calloc( 1, MDN_OCTET_LENGTH );
            memcpy(  p_targetSETID->choice.mdn.buf,
                     p_ThirdParty_info->thirdPartyIdName,
                     p_targetSETID->choice.mdn.size );
        }
        else if(p_ThirdParty_info->thirdPartyId == thirdparty_id_ims_public_identity)
        {
            
            p_targetSETID->present        = SETId_PR_imsi;
            /// ULP-Components ASN.1  imsi     OCTET STRING(SIZE (8)),
            p_targetSETID->choice.imsi.size  = IMSI_OCTET_LENGTH;
            p_targetSETID->choice.imsi.buf   = (uint8_t*)GN_Calloc( 1, IMSI_OCTET_LENGTH );   /* not malloc! */
            memcpy(  p_targetSETID->choice.imsi.buf,
                     p_ThirdParty_info->thirdPartyIdName,
                     p_targetSETID->choice.imsi.size );
        }
        else if(p_ThirdParty_info->thirdPartyId == thirdparty_id_min)
        {
            
            p_targetSETID->present          = SETId_PR_min;
            supl_PDU_Add_SDL_BitString_U1_ptr(
               &p_targetSETID->choice.min,
               (U1*)p_ThirdParty_info->thirdPartyIdName,
               MIN_BITSTRING_LENGTH );
        }
        else if(p_ThirdParty_info->thirdPartyId == thirdparty_id_emailaddr)
        {
        /*Since we use the same structure for both third party transfer and retrieval the below maping is done as there are uncommon fields between them*/
            
            p_targetSETID->present       = SETId_PR_nai;
            p_targetSETID->choice.nai.buf             = GN_Calloc( 1, strlen( (CH*) p_ThirdParty_info->thirdPartyIdName ) + 2 );
            p_targetSETID->choice.nai.buf[0] = 'V';
            strcpy( (CH*)&p_targetSETID->choice.nai.buf[1], (CH*) p_ThirdParty_info->thirdPartyIdName );
        }
        else if(p_ThirdParty_info->thirdPartyId == thirdparty_id_sip_uri)
        {
        /*Since we use the same structure for both third party transfer and retrieval the below maping is done as there are uncommon fields between them*/
            
             p_targetSETID->present                            = SETId_PR_iPAddress;
             p_targetSETID->choice.iPAddress.present                   = IPAddress_PR_ipv4Address;
             p_targetSETID->choice.iPAddress.choice.ipv4Address.size      = 4;
             p_targetSETID->choice.iPAddress.choice.ipv4Address.buf    = (uint8_t*)GN_Calloc( 1,4 );//GN_Calloc( 1, 4 );
            memcpy( p_targetSETID->choice.iPAddress.choice.ipv4Address.buf,
                     p_ThirdParty_info->thirdPartyIdName,
                     p_targetSETID->choice.iPAddress.choice.ipv4Address.size );
        }
        else if(p_ThirdParty_info->thirdPartyId == thirdparty_id_uri)
        {
            /*Since we use the same structure for both third party transfer and retrieval the below maping is done as there are uncommon fields between them*/
                
                p_targetSETID->present                               = SETId_PR_iPAddress;
                p_targetSETID->choice.iPAddress.present              = IPAddress_PR_ipv6Address;
                p_targetSETID->choice.iPAddress.choice.ipv6Address.size = 16;
                p_targetSETID->choice.iPAddress.choice.ipv6Address.buf  = (uint8_t*)GN_Calloc( 1, 16 );//GN_Calloc( 1, 16 );
                memcpy(p_targetSETID->choice.iPAddress.choice.ipv6Address.buf,
                        p_ThirdParty_info->thirdPartyIdName,
                        p_targetSETID->choice.iPAddress.choice.ipv6Address.size );
        }
        else 
        {
             GN_SUPL_Log( "unsupported set id type" );
        }

   }
   else
   {
      GN_SUPL_Log("No ThirdParty Data in the SUPL instance");
   }

}

void supl_PDU_Add_ApplicationID(ApplicationID_t **p_p_ApplicationID, s_GN_SUPL_V2_ApplicationInfo *p_ApplicationID_Info)
{
    ApplicationID_t *p_ApplicationId;

    GN_SUPL_Log( "supl_PDU_Add_ApplicationID: ver2_PosTechnology_extension");

    GN_SUPL_Log("supl_PDU_Add_ApplicationID: Application Provider %s", p_ApplicationID_Info->applicationProvider);
    GN_SUPL_Log("supl_PDU_Add_ApplicationID: Application Name %s", p_ApplicationID_Info->appName);
    GN_SUPL_Log("supl_PDU_Add_ApplicationID: Application Version %s", p_ApplicationID_Info->appVersion);
    if((p_ApplicationID_Info != NULL)&&(p_ApplicationID_Info->applicationIDInfoPresence))
    {

         ASN1_SET(p_p_ApplicationID) = (ApplicationID_t*)GN_Calloc( 1, sizeof(ApplicationID_t) );
         p_ApplicationId = ASN1_GET(p_p_ApplicationID);

         if(p_ApplicationID_Info->applicationProvider != NULL)
         {

             p_ApplicationId->appProvider.buf  = GN_Calloc( 1, strlen( (CH*) p_ApplicationID_Info->applicationProvider ) + 1 );

             p_ApplicationId->appProvider.size = strlen( (CH*) p_ApplicationID_Info->applicationProvider );
             strcpy( (CH*)&p_ApplicationId->appProvider.buf[0], (CH*) p_ApplicationID_Info->applicationProvider );

             GN_SUPL_Log("supl_PDU_Add_ApplicationID: Application Provider string size %d", p_ApplicationId->appProvider.size);
             GN_SUPL_Log("supl_PDU_Add_ApplicationID: Application Provider Name %s",  p_ApplicationId->appProvider.buf);
         }

        if(p_ApplicationID_Info->appName != NULL)
        {

            p_ApplicationId->appName.buf  = GN_Calloc( 1, strlen( (CH*) p_ApplicationID_Info->appName ) + 1 );

            p_ApplicationId->appName.size = strlen( (CH*) p_ApplicationID_Info->appName );
            strcpy( (CH*)&p_ApplicationId->appName.buf[0], (CH*) p_ApplicationID_Info->appName );

            GN_SUPL_Log("supl_PDU_Add_ApplicationID: Application Name string size %d", p_ApplicationId->appName.size);
            GN_SUPL_Log("supl_PDU_Add_ApplicationID: Application Name %s",  p_ApplicationId->appName.buf);
        }

        if(p_ApplicationID_Info->appVersion != NULL)
        {

            p_ApplicationId->appVersion = GN_Calloc( 1, sizeof( IA5String_t) );
            p_ApplicationId->appVersion->buf  = GN_Calloc( 1, strlen( (CH*) p_ApplicationID_Info->appVersion ) + 2 );

            p_ApplicationId->appVersion->size = strlen( (CH*) p_ApplicationID_Info->appVersion );
            strcpy( (CH*)&p_ApplicationId->appVersion->buf[0], (CH*) p_ApplicationID_Info->appVersion );

            GN_SUPL_Log("supl_PDU_Add_ApplicationID: Application Version string size %d", p_ApplicationId->appVersion->size);
            GN_SUPL_Log("supl_PDU_Add_ApplicationID: Application Version String %s",  p_ApplicationId->appVersion->buf);
        }



    }

}


//*****************************************************************************
/// \brief
///      Function to add Set Location ID into an unencoded SUPL-START PDU.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLSTART_SetLocationID
(
   void           *p_ThisPDU,       ///< [in/out] Pointer to unencoded structure
                                    ///   ULP_PDU_t. Passed in as (void *) to hide
                                    ///   implementation.
   s_LocationId   *p_SetLocationID  ///< [in] Set Location ID.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   supl_PDU_Add_SetLocationID(
       &p_PDU_Cast->message.choice.msSUPLSTART.locationId,
      p_SetLocationID );
}


//*****************************************************************************
/// \brief
///      Function to add Set Location ID into an unencoded SUPL-START PDU.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLSTART_ThirdPartyInfo
(
   void           *p_ThisPDU,       ///< [in/out] Pointer to unencoded structure
                                    ///   ULP_PDU_t. Passed in as (void *) to hide
                                    ///   implementation.
   s_GN_SUPL_V2_ThirdPartyInfo   *p_third_party_info  ///< [in] Set Location ID.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   GN_SUPL_Log( "supl_PDU_Add_SUPLSTART_ThirdPartyInfo: ID  %d" ,p_third_party_info->thirdPartyId);
   GN_SUPL_Log( "supl_PDU_Add_SUPLSTART_ThirdPartyInfo: Name    %s" ,p_third_party_info->thirdPartyIdName);
   GN_SUPL_Log( "supl_PDU_Add_SUPLSTART_ThirdPartyInfo: PDUCast Addr    %d" ,p_PDU_Cast);

   if(p_third_party_info != NULL)
   {
       if(p_PDU_Cast->message.choice.msSUPLSTART.ver2_SUPL_START_extension == NULL)
       p_PDU_Cast->message.choice.msSUPLSTART.ver2_SUPL_START_extension = (Ver2_SUPL_START_extension_t*)GN_Calloc( 1, sizeof(Ver2_SUPL_START_extension_t) );

       p_PDU_Cast->message.choice.msSUPLSTART.ver2_SUPL_START_extension->thirdParty = (ThirdParty_t*)GN_Calloc( 1, sizeof(ThirdParty_t) );

       supl_PDU_Add_ThirdParty_Info(p_PDU_Cast->message.choice.msSUPLSTART.ver2_SUPL_START_extension->thirdParty,
                                    p_third_party_info );
   }


}

//*****************************************************************************
/// \brief
///      Function to add Set Location ID into an unencoded SUPL-START PDU.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLSETINIT_ThirdPartyInfo
(
   void           *p_ThisPDU,       ///< [in/out] Pointer to unencoded structure
                                    ///   ULP_PDU_t. Passed in as (void *) to hide
                                    ///   implementation.
   s_GN_SUPL_V2_ThirdPartyInfo   *p_third_party_info  ///< [in] Set Location ID.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   GN_SUPL_Log( "supl_PDU_Add_SUPLSETINIT_ThirdPartyInfo: ID  %d" ,p_third_party_info->thirdPartyId);
   GN_SUPL_Log( "supl_PDU_Add_SUPLSETINIT_ThirdPartyInfo: Name    %s" ,p_third_party_info->thirdPartyIdName);
   GN_SUPL_Log( "supl_PDU_Add_SUPLSETINIT_ThirdPartyInfo: PDUCast Addr    %d" ,p_PDU_Cast);

   if(p_third_party_info != NULL)
   {
       supl_PDU_Add_TargetID_Info(&(p_PDU_Cast->message.choice.msSUPLSETINIT.targetSETID),
                                    p_third_party_info );
   }


}

//*****************************************************************************
/// \brief
///      Function to add Application ID Info  into an unencoded SUPL_SET_INIT PDU.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLSETINIT_AppIdInfo
(
   void           *p_ThisPDU,       ///< [in/out] Pointer to unencoded structure
                                    ///   ULP_PDU_t. Passed in as (void *) to hide
                                    ///   implementation.
   s_GN_SUPL_V2_ApplicationInfo   *p_AppId_Info  ///< [in] Set Location ID.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   GN_SUPL_Log( "supl_PDU_Add_SUPLSETINIT_AppIdInfo: PDUCast Addr    %d" ,p_PDU_Cast);

   if(p_AppId_Info != NULL)
   {
       supl_PDU_Add_ApplicationID(&p_PDU_Cast->message.choice.msSUPLSETINIT.applicationID,
                                    p_AppId_Info );
   }


}
void supl_PDU_Add_SUPLSTART_ApplicationID(   void *p_ThisPDU, s_GN_SUPL_V2_ApplicationInfo   *p_application_id)
{
    ULP_PDU_t *p_PDU_Cast = p_ThisPDU;
    if(p_application_id != NULL)
    {

        GN_SUPL_Log( "supl_PDU_Add_SUPLSTART_ApplicationID: Application Provider  %s" ,p_application_id->applicationProvider);
        GN_SUPL_Log( "supl_PDU_Add_SUPLSTART_ApplicationID: Application Name     %s" ,p_application_id->appName);

        GN_SUPL_Log( "supl_PDU_Add_SUPLSTART_ApplicationID: Application Version  %s" ,p_application_id->appVersion);

        if(p_PDU_Cast->message.choice.msSUPLSTART.ver2_SUPL_START_extension == NULL)
        p_PDU_Cast->message.choice.msSUPLSTART.ver2_SUPL_START_extension = (Ver2_SUPL_START_extension_t*)GN_Calloc( 1, sizeof(Ver2_SUPL_START_extension_t) );

        p_PDU_Cast->message.choice.msSUPLSTART.ver2_SUPL_START_extension->applicationID = (ApplicationID_t*)GN_Calloc( 1, sizeof(ApplicationID_t) );


        supl_PDU_Add_ApplicationID(&p_PDU_Cast->message.choice.msSUPLSTART.ver2_SUPL_START_extension->applicationID,
                                     p_application_id );

    }




}


//*****************************************************************************
/// \brief
///      Function to add Third Party ID Info into an unencoded SUPL-TRIGGERED-START PDU.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLTRIGGEREDSTART_ThirdPartyInfo
(
   void           *p_ThisPDU,       ///< [in/out] Pointer to unencoded structure
                                    ///   ULP_PDU_t. Passed in as (void *) to hide
                                    ///   implementation.
   s_GN_SUPL_V2_ThirdPartyInfo   *p_third_party_info  ///< [in] Third party details.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;
   if(p_third_party_info != NULL)
    {
        p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTART.thirdParty = (ThirdParty_t*)GN_Calloc( 1, sizeof(ThirdParty_t) );
        supl_PDU_Add_ThirdParty_Info(p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTART.thirdParty,
                                p_third_party_info );
    }
}


//*****************************************************************************
/// \brief
///      Function to add Application ID Info into an unencoded SUPL-TRIGGERED-START PDU.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLTRIGGEREDSTART_ApplicationID
(
   void           *p_ThisPDU,       ///< [in/out] Pointer to unencoded structure
                                    ///   ULP_PDU_t. Passed in as (void *) to hide
                                    ///   implementation.
   s_GN_SUPL_V2_ApplicationInfo   *p_application_id  ///< [in] Set Application IDD.
)
{
    ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

    if(p_application_id != NULL)
   {

       GN_SUPL_Log( "supl_PDU_Add_SUPLSTART_ApplicationID: Application Provider  %s" ,p_application_id->applicationProvider);
       GN_SUPL_Log( "supl_PDU_Add_SUPLSTART_ApplicationID: Application Name     %s" ,p_application_id->appName);

       GN_SUPL_Log( "supl_PDU_Add_SUPLSTART_ApplicationID: Application Version  %s" ,p_application_id->appVersion);
    }

   p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTART.applicationID = (ApplicationID_t*)GN_Calloc( 1, sizeof(ApplicationID_t) );

   //p_PDU_Cast->message.choice.msSUPLSTART.ver2_SUPL_START_extension->applicationID = (ThirdParty_t*)GN_Calloc( 1, sizeof(ApplicationID_t) );


   supl_PDU_Add_ApplicationID(&p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTART.applicationID,
                                p_application_id );

}


//*****************************************************************************
/// \brief
///      Function to add Get Location ID from a decoded SUPL-START PDU.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Get_SUPLSTART_SetLocationID
(
   void           *p_ThisPDU,       ///< [in/out] Pointer to unencoded structure
                                    ///   ULP_PDU_t. Passed in as (void *) to hide
                                    ///   implementation.
   s_LocationId   *p_SetLocationID  ///< [out] Set Location ID.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   supl_PDU_Get_SetLocationID(
      p_SetLocationID,
      &p_PDU_Cast->message.choice.msSUPLSTART.locationId);
}


//*****************************************************************************
/// \brief
///      Function to get SLPMode from a decoded SUPL-INIT PDU.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Get_SUPLINIT_SLPMode
(
   void           *p_ThisPDU,    ///< [in/out] Pointer to unencoded structure
                                 ///   ULP_PDU_t. Passed in as (void *) to hide
                                 ///   implementation.
   e_GN_SLPMode   *p_GN_SLPMode  ///< [out] SLP Mode.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   /// <pre>
   /// SLPMode ::= ENUMERATED {proxy(0), nonProxy(1)}
   /// </pre>

   switch ( ASN1_GET(ASN1_ENUM_1_GET(p_PDU_Cast->message.choice.msSUPLINIT.sLPMode)) )
   {
   case SLPMode_proxy:          *p_GN_SLPMode = GN_proxy;     break;
   case SLPMode_nonProxy:       *p_GN_SLPMode = GN_nonProxy;  break;
   }
}


//*****************************************************************************
/// \brief
///      Function to add Get Position Method from a decoded SUPL-POS-INIT PDU.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Get_SUPLINIT_PosMethod
(
   void           *p_ThisPDU,       ///< [in/out] Pointer to unencoded structure
                                    ///   ULP_PDU_t. Passed in as (void *) to hide
                                    ///   implementation.
   e_GN_PosMethod *p_GN_PosMethod   ///< [out] Pos Method.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;
   PosMethod_t *p_posmethod;
   p_posmethod = &p_PDU_Cast->message.choice.msSUPLINIT.posMethod;
   switch ( *(ASN1_ENUM_2_GET(p_posmethod)))
   {
   case agpsSETassisted1:     *p_GN_PosMethod = GN_PosMethod_agpsSETassisted;    break;
   case agpsSETbased1:        *p_GN_PosMethod = GN_PosMethod_agpsSETbased;       break;
   case agpsSETassistedpref1: *p_GN_PosMethod = GN_PosMethod_agpsSETassistedpref;break;
   case agpsSETbasedpref1:    *p_GN_PosMethod = GN_PosMethod_agpsSETbasedpref;   break;
   case autonomousGPS1:       *p_GN_PosMethod = GN_PosMethod_autonomousGPS;      break;
   case aFLT1:                *p_GN_PosMethod = GN_PosMethod_aFLT;               break;
   case eCID1:                *p_GN_PosMethod = GN_PosMethod_eCID;               break;
   case eOTD1:                *p_GN_PosMethod = GN_PosMethod_eOTD;               break;
   case oTDOA1:               *p_GN_PosMethod = GN_PosMethod_oTDOA;              break;
   case noPosition1:
   default:
       *p_GN_PosMethod = GN_PosMethod_noPosition;
   break;
   }
}


//*****************************************************************************
/// \brief
///      Function to add Get Notification from a decoded SUPL-POS-INIT PDU.
///
/// \returns
///      TRUE if a Notification was present in the PDU.
//*****************************************************************************
BL supl_PDU_Get_SUPLINIT_Notification
(
   void                          *p_ThisPDU,          ///< [in/out] Pointer to unencoded structure
                                                      ///   ULP_PDU_t. Passed in as (void *) to hide
                                                      ///   implementation.
   s_GN_SUPL_Notification_Parms  *p_SUPL_Notification ///< [out] Notification
)
{
   ULP_PDU_t        *p_PDU_Cast       = p_ThisPDU;
   Notification_t   *p_Notification   = p_PDU_Cast->message.choice.msSUPLINIT.notification;

   if ( p_Notification != NULL )
   {
       switch ( ASN1_GET(ASN1_ENUM_1_GET(p_Notification->notificationType )))
      {
      case NotificationType_noNotificationNoVerification:
         p_SUPL_Notification->GN_SUPL_Notify_Type = NOTIFY_NONE; break;
      case NotificationType_notificationOnly:
         p_SUPL_Notification->GN_SUPL_Notify_Type = NOTIFY_ONLY; break;
      case NotificationType_notificationAndVerficationAllowedNA:
         p_SUPL_Notification->GN_SUPL_Notify_Type = NOTIFY_ALLOWED_ON_TIMEOUT; break;
      case NotificationType_notificationAndVerficationDeniedNA:
         p_SUPL_Notification->GN_SUPL_Notify_Type = NOTIFY_DENIED_ON_TIMEOUT; break;
      case NotificationType_privacyOverride:
         p_SUPL_Notification->GN_SUPL_Notify_Type = NOTIFY_PRIVACY_OVERRIDE;
         // There is no notification with privacy override so return now to save
         // superfluous checking.
         return TRUE;
         break;
      default:
         p_SUPL_Notification->GN_SUPL_Notify_Type = NOTIFY_NONE; break;
      }

       if ( p_Notification->encodingType != NULL)
      {
         switch ( ASN1_GET(ASN1_ENUM_2_GET(p_Notification->encodingType)) )
         {
         case EncodingType_ucs2:        p_SUPL_Notification->GN_SUPL_Encoding_Type = ENCODING_ucs2;       break;
         case EncodingType_gsmDefault:  p_SUPL_Notification->GN_SUPL_Encoding_Type = ENCODING_gsmDefault; break;
         case EncodingType_utf8:        p_SUPL_Notification->GN_SUPL_Encoding_Type = ENCODING_utf8;       break;
         default:          p_SUPL_Notification->GN_SUPL_Encoding_Type = ENCODING_NONE;       break;
         }
      }

      if ( p_Notification->clientName != NULL )
      {
         p_SUPL_Notification->ClientNameLen  = p_Notification->clientName->size;
         p_SUPL_Notification->ClientName     = GN_Calloc( 1, p_SUPL_Notification->ClientNameLen );
         memcpy(  p_SUPL_Notification->ClientName,
                  p_Notification->clientName->buf,
                  p_SUPL_Notification->ClientNameLen );

         if ( p_Notification->clientNameType != NULL )
         {
            switch ( ASN1_GET(ASN1_ENUM_2_GET(p_Notification->clientNameType)) )
            {
            case FormatIndicator_logicalName:       p_SUPL_Notification->ClientNameType = NOTIFY_logicalName;         break;
            case FormatIndicator_e_mailAddress:     p_SUPL_Notification->ClientNameType = NOTIFY_e_mailAddress;       break;
            case FormatIndicator_msisdn:            p_SUPL_Notification->ClientNameType = NOTIFY_msisdn;              break;
            case FormatIndicator_url:               p_SUPL_Notification->ClientNameType = NOTIFY_url;                 break;
            case FormatIndicator_sipUrl:            p_SUPL_Notification->ClientNameType = NOTIFY_sipUrl;              break;
            case FormatIndicator_min:               p_SUPL_Notification->ClientNameType = NOTIFY_min;                 break;
            case FormatIndicator_mdn:               p_SUPL_Notification->ClientNameType = NOTIFY_mdn;                 break;
            case FormatIndicator_iMSPublicidentity: p_SUPL_Notification->ClientNameType = NOTIFY_imsPublicIdentity;   break;
            default:
               p_SUPL_Notification->ClientNameType = NOTIFY_noFormat;
            break;
            }
         }
         else
         {
            p_SUPL_Notification->ClientNameType = NOTIFY_noFormat;
         }
      }

      if ( p_Notification->requestorId != NULL )
      {
         p_SUPL_Notification->RequestorIdLen = p_Notification->requestorId->size;
         p_SUPL_Notification->RequestorId    = GN_Calloc( 1, p_SUPL_Notification->RequestorIdLen );
         memcpy(  p_SUPL_Notification->RequestorId,
                  p_Notification->requestorId->buf,
                  p_SUPL_Notification->RequestorIdLen );

         if ( p_Notification->requestorIdType != NULL )
         {
            switch ( ASN1_GET(ASN1_ENUM_2_GET(p_Notification->requestorIdType)) )
            {
            case FormatIndicator_logicalName:       p_SUPL_Notification->RequestorIdType = NOTIFY_logicalName;        break;
            case FormatIndicator_e_mailAddress:     p_SUPL_Notification->RequestorIdType = NOTIFY_e_mailAddress;      break;
            case FormatIndicator_msisdn:            p_SUPL_Notification->RequestorIdType = NOTIFY_msisdn;             break;
            case FormatIndicator_url:               p_SUPL_Notification->RequestorIdType = NOTIFY_url;                break;
            case FormatIndicator_sipUrl:            p_SUPL_Notification->RequestorIdType = NOTIFY_sipUrl;             break;
            case FormatIndicator_min:               p_SUPL_Notification->RequestorIdType = NOTIFY_min;                break;
            case FormatIndicator_mdn:               p_SUPL_Notification->RequestorIdType = NOTIFY_mdn;                break;
            case FormatIndicator_iMSPublicidentity: p_SUPL_Notification->RequestorIdType = NOTIFY_imsPublicIdentity;  break;
            default:
               p_SUPL_Notification->RequestorIdType = NOTIFY_noFormat;
            break;
            }
         }
         else
         {
            p_SUPL_Notification->RequestorIdType = NOTIFY_noFormat;
         }
      }
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}


//*****************************************************************************
/// \brief
///      Function to get Minimum Major version from a decoded SUPL-POS-INIT PDU.
///
/// \returns
///      TRUE if a minimum major version was present in the PDU.
//*****************************************************************************
BL supl_PDU_Get_SUPLINIT_MinMajorVer
(
   void           *p_ThisPDU,    ///< [in/out] Pointer to unencoded structure
                                 ///   ULP_PDU_t. Passed in as (void *) to hide
                                 ///   implementation.
   U1             *p_MinMajVer   ///< [out] Minimum Major Version.
)
{
   ULP_PDU_t                   *p_PDU_Cast       = p_ThisPDU;
   Ver2_SUPL_INIT_extension_t  *p_SuplInitExt    = p_PDU_Cast->message.choice.msSUPLINIT.ver2_SUPL_INIT_extension;

   if (    p_SuplInitExt != NULL
        && p_SuplInitExt->minimumMajorVersion != NULL)
   {
      *p_MinMajVer = *p_SuplInitExt->minimumMajorVersion;
      GN_SUPL_Log( "supl_PDU_Get_SUPLINIT_MinMajorVer: Minimum Major Version = %d",  *p_MinMajVer );
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}


//*****************************************************************************
/// \brief
///      Function to get supported Network Information from a decoded SUPL-POS-INIT PDU.
///
/// \returns
///      void
//*****************************************************************************
void supl_PDU_Get_SUPLINIT_Supported_Network_Info
(
    void           *p_ThisPDU,    ///< [in/out] Pointer to unencoded structure
                                 ///   ULP_PDU_t. Passed in as (void *) to hide
                                 ///   implementation.
    s_GN_SUPL_V2_SupportedNetworkInfo **p_p_SupportedNetworkInfo //Pointer to the supported Network Information Structure
)
{
    ULP_PDU_t                   *p_PDU_Cast      = p_ThisPDU;
    Ver2_SUPL_INIT_extension_t  *p_SuplInitExt   = p_PDU_Cast->message.choice.msSUPLINIT.ver2_SUPL_INIT_extension;

    s_GN_SUPL_V2_SupportedNetworkInfo *p_SupportedNetworkInfo;

    if(p_SuplInitExt != NULL
        && p_SuplInitExt->supportedNetworkInformation !=  NULL)
    {
        p_SupportedNetworkInfo                          = GN_Calloc( 1, sizeof(s_GN_SUPL_V2_SupportedNetworkInfo) );

        p_SupportedNetworkInfo->wLAN                    = p_SuplInitExt->supportedNetworkInformation->wLAN;
        p_SupportedNetworkInfo->cDMA                    = p_SuplInitExt->supportedNetworkInformation->cDMA;
        p_SupportedNetworkInfo->hRDP                    = p_SuplInitExt->supportedNetworkInformation->hRDP;
        p_SupportedNetworkInfo->uMB                     = p_SuplInitExt->supportedNetworkInformation->uMB;
        p_SupportedNetworkInfo->lTE                     = p_SuplInitExt->supportedNetworkInformation->lTE;
        p_SupportedNetworkInfo->wIMAX                   = p_SuplInitExt->supportedNetworkInformation->wIMAX;
        p_SupportedNetworkInfo->historic                = p_SuplInitExt->supportedNetworkInformation->historic;
        p_SupportedNetworkInfo->nonServing              = p_SuplInitExt->supportedNetworkInformation->nonServing;
        p_SupportedNetworkInfo->uTRANGPSReferenceTime   = p_SuplInitExt->supportedNetworkInformation->uTRANGPSReferenceTime;
        p_SupportedNetworkInfo->uTRANGANSSReferenceTime = p_SuplInitExt->supportedNetworkInformation->uTRANGANSSReferenceTime;
        p_SupportedNetworkInfo->gSM                     = p_SuplInitExt->supportedNetworkInformation->gSM;
        p_SupportedNetworkInfo->wCDMA                   = p_SuplInitExt->supportedNetworkInformation->wCDMA;

        if(p_SuplInitExt->supportedNetworkInformation->supportedWLANInfo !=  NULL)
        {
            p_SupportedNetworkInfo->supportedWLANInfo               = (s_GN_SUPL_V2_SupportedWLANInfo* )GN_Calloc( 1, sizeof(s_GN_SUPL_V2_SupportedWLANInfo) );

            p_SupportedNetworkInfo->supportedWLANInfo->apAG         = p_SuplInitExt->supportedNetworkInformation->supportedWLANInfo->apAG;
            p_SupportedNetworkInfo->supportedWLANInfo->apChanFreq   = p_SuplInitExt->supportedNetworkInformation->supportedWLANInfo->apChanFreq;
            p_SupportedNetworkInfo->supportedWLANInfo->apDevType    = p_SuplInitExt->supportedNetworkInformation->supportedWLANInfo->apDevType;
            p_SupportedNetworkInfo->supportedWLANInfo->apRepLoc     = p_SuplInitExt->supportedNetworkInformation->supportedWLANInfo->apRepLoc;
            p_SupportedNetworkInfo->supportedWLANInfo->apRSSI       = p_SuplInitExt->supportedNetworkInformation->supportedWLANInfo->apRSSI;
            p_SupportedNetworkInfo->supportedWLANInfo->apRTD        = p_SuplInitExt->supportedNetworkInformation->supportedWLANInfo->apRTD;
            p_SupportedNetworkInfo->supportedWLANInfo->apSN         = p_SuplInitExt->supportedNetworkInformation->supportedWLANInfo->apSN;
            p_SupportedNetworkInfo->supportedWLANInfo->apTP         = p_SuplInitExt->supportedNetworkInformation->supportedWLANInfo->apTP;
            p_SupportedNetworkInfo->supportedWLANInfo->setAG        = p_SuplInitExt->supportedNetworkInformation->supportedWLANInfo->setAG;
            p_SupportedNetworkInfo->supportedWLANInfo->setRSSI      = p_SuplInitExt->supportedNetworkInformation->supportedWLANInfo->setRSSI;
            p_SupportedNetworkInfo->supportedWLANInfo->setSN        = p_SuplInitExt->supportedNetworkInformation->supportedWLANInfo->setSN;
            p_SupportedNetworkInfo->supportedWLANInfo->setTP        = p_SuplInitExt->supportedNetworkInformation->supportedWLANInfo->setTP;
        }
        
        if(p_SuplInitExt->supportedNetworkInformation->supportedWCDMAInfo !=  NULL)
        {
            p_SupportedNetworkInfo->supportedWCDMAInfo      = (s_GN_SUPL_V2_SupportedWCDMAInfo* )GN_Calloc( 1, sizeof(s_GN_SUPL_V2_SupportedWCDMAInfo) );

            p_SupportedNetworkInfo->supportedWCDMAInfo->mRL = p_SuplInitExt->supportedNetworkInformation->supportedWCDMAInfo->mRL;
        }

        *p_p_SupportedNetworkInfo = p_SupportedNetworkInfo;

    }
}

//*****************************************************************************
/// \brief
///      Function to get supported Network Information from a decoded SUPL-RESPONSE PDU.
///
/// \returns
///      void
//*****************************************************************************
void supl_PDU_Get_SUPLRESPONSE_Supported_Network_Info
(
    void           *p_ThisPDU,    ///< [in/out] Pointer to unencoded structure
                                 ///   ULP_PDU_t. Passed in as (void *) to hide
                                 ///   implementation.
    s_GN_SUPL_V2_SupportedNetworkInfo **p_p_SupportedNetworkInfo //Pointer to the supported Network Information Structure
)
{
    ULP_PDU_t                       *p_PDU_Cast      = p_ThisPDU;
    Ver2_SUPL_RESPONSE_extension_t  *p_SuplRespExt   = p_PDU_Cast->message.choice.msSUPLRESPONSE.ver2_SUPL_RESPONSE_extension;

    s_GN_SUPL_V2_SupportedNetworkInfo *p_SupportedNetworkInfo;

    if(p_SuplRespExt != NULL
        && p_SuplRespExt->supportedNetworkInformation !=  NULL)
    {
        p_SupportedNetworkInfo                          = (s_GN_SUPL_V2_SupportedNetworkInfo* )GN_Calloc( 1, sizeof(s_GN_SUPL_V2_SupportedNetworkInfo) );

        p_SupportedNetworkInfo->wLAN                    = p_SuplRespExt->supportedNetworkInformation->wLAN;
        p_SupportedNetworkInfo->cDMA                    = p_SuplRespExt->supportedNetworkInformation->cDMA;
        p_SupportedNetworkInfo->hRDP                    = p_SuplRespExt->supportedNetworkInformation->hRDP;
        p_SupportedNetworkInfo->uMB                     = p_SuplRespExt->supportedNetworkInformation->uMB;
        p_SupportedNetworkInfo->lTE                     = p_SuplRespExt->supportedNetworkInformation->lTE;
        p_SupportedNetworkInfo->wIMAX                   = p_SuplRespExt->supportedNetworkInformation->wIMAX;
        p_SupportedNetworkInfo->historic                = p_SuplRespExt->supportedNetworkInformation->historic;
        p_SupportedNetworkInfo->nonServing              = p_SuplRespExt->supportedNetworkInformation->nonServing;
        p_SupportedNetworkInfo->uTRANGPSReferenceTime   = p_SuplRespExt->supportedNetworkInformation->uTRANGPSReferenceTime;
        p_SupportedNetworkInfo->uTRANGANSSReferenceTime = p_SuplRespExt->supportedNetworkInformation->uTRANGANSSReferenceTime;
        p_SupportedNetworkInfo->gSM                     = p_SuplRespExt->supportedNetworkInformation->gSM;
        p_SupportedNetworkInfo->wCDMA                   = p_SuplRespExt->supportedNetworkInformation->wCDMA;

        if(p_SuplRespExt->supportedNetworkInformation->supportedWLANInfo !=  NULL)
        {
        
            p_SupportedNetworkInfo->supportedWLANInfo             = (s_GN_SUPL_V2_SupportedWLANInfo* )GN_Calloc( 1, sizeof(s_GN_SUPL_V2_SupportedWLANInfo) );

            p_SupportedNetworkInfo->supportedWLANInfo->apAG       = p_SuplRespExt->supportedNetworkInformation->supportedWLANInfo->apAG;
            p_SupportedNetworkInfo->supportedWLANInfo->apChanFreq = p_SuplRespExt->supportedNetworkInformation->supportedWLANInfo->apChanFreq;
            p_SupportedNetworkInfo->supportedWLANInfo->apDevType  = p_SuplRespExt->supportedNetworkInformation->supportedWLANInfo->apDevType;
            p_SupportedNetworkInfo->supportedWLANInfo->apRepLoc   = p_SuplRespExt->supportedNetworkInformation->supportedWLANInfo->apRepLoc;
            p_SupportedNetworkInfo->supportedWLANInfo->apRSSI     = p_SuplRespExt->supportedNetworkInformation->supportedWLANInfo->apRSSI;
            p_SupportedNetworkInfo->supportedWLANInfo->apRTD      = p_SuplRespExt->supportedNetworkInformation->supportedWLANInfo->apRTD;
            p_SupportedNetworkInfo->supportedWLANInfo->apSN       = p_SuplRespExt->supportedNetworkInformation->supportedWLANInfo->apSN;
            p_SupportedNetworkInfo->supportedWLANInfo->apTP       = p_SuplRespExt->supportedNetworkInformation->supportedWLANInfo->apTP;
            p_SupportedNetworkInfo->supportedWLANInfo->setAG      = p_SuplRespExt->supportedNetworkInformation->supportedWLANInfo->setAG;
            p_SupportedNetworkInfo->supportedWLANInfo->setRSSI    = p_SuplRespExt->supportedNetworkInformation->supportedWLANInfo->setRSSI;
            p_SupportedNetworkInfo->supportedWLANInfo->setSN      = p_SuplRespExt->supportedNetworkInformation->supportedWLANInfo->setSN;
            p_SupportedNetworkInfo->supportedWLANInfo->setTP      = p_SuplRespExt->supportedNetworkInformation->supportedWLANInfo->setTP;
        }
        
        if(p_SuplRespExt->supportedNetworkInformation->supportedWCDMAInfo !=  NULL)
        {
            p_SupportedNetworkInfo->supportedWCDMAInfo      = (s_GN_SUPL_V2_SupportedWCDMAInfo* )GN_Calloc( 1, sizeof(s_GN_SUPL_V2_SupportedWCDMAInfo) );

            p_SupportedNetworkInfo->supportedWCDMAInfo->mRL = p_SuplRespExt->supportedNetworkInformation->supportedWCDMAInfo->mRL;
        }
        *p_p_SupportedNetworkInfo = p_SupportedNetworkInfo;

    }
}

//**************************************************************************************
/// \brief
///      Function to get supported Network Information from a decoded SUPL-TRIGGERED RESPONSE PDU.
///
/// \returns
///      void
//**************************************************************************************
void supl_PDU_Get_SUPLTRIGGEREDRESPONSE_Supported_Network_Info
(
    void           *p_ThisPDU,    ///< [in/out] Pointer to unencoded structure
                                 ///   ULP_PDU_t. Passed in as (void *) to hide
                                 ///   implementation.
    s_GN_SUPL_V2_SupportedNetworkInfo **p_p_SupportedNetworkInfo //Pointer to the supported Network Information Structure
)
{
    ULP_PDU_t                       *p_PDU_Cast      = p_ThisPDU;
    Ver2_SUPLTRIGGEREDRESPONSE_t *p_SuplTriggeredRsp = &p_PDU_Cast->message.choice.msSUPLTRIGGEREDRESPONSE;

    s_GN_SUPL_V2_SupportedNetworkInfo *p_SupportedNetworkInfo;

    if(p_SuplTriggeredRsp != NULL
        && p_SuplTriggeredRsp->supportedNetworkInformation !=  NULL)
    {
        p_SupportedNetworkInfo                          = (s_GN_SUPL_V2_SupportedNetworkInfo* )GN_Calloc( 1, sizeof(s_GN_SUPL_V2_SupportedNetworkInfo) );

        p_SupportedNetworkInfo->wLAN                    = p_SuplTriggeredRsp->supportedNetworkInformation->wLAN;
        p_SupportedNetworkInfo->cDMA                    = p_SuplTriggeredRsp->supportedNetworkInformation->cDMA;
        p_SupportedNetworkInfo->hRDP                    = p_SuplTriggeredRsp->supportedNetworkInformation->hRDP;
        p_SupportedNetworkInfo->uMB                     = p_SuplTriggeredRsp->supportedNetworkInformation->uMB;
        p_SupportedNetworkInfo->lTE                     = p_SuplTriggeredRsp->supportedNetworkInformation->lTE;
        p_SupportedNetworkInfo->wIMAX                   = p_SuplTriggeredRsp->supportedNetworkInformation->wIMAX;
        p_SupportedNetworkInfo->historic                = p_SuplTriggeredRsp->supportedNetworkInformation->historic;
        p_SupportedNetworkInfo->nonServing              = p_SuplTriggeredRsp->supportedNetworkInformation->nonServing;
        p_SupportedNetworkInfo->uTRANGPSReferenceTime   = p_SuplTriggeredRsp->supportedNetworkInformation->uTRANGPSReferenceTime;
        p_SupportedNetworkInfo->uTRANGANSSReferenceTime = p_SuplTriggeredRsp->supportedNetworkInformation->uTRANGANSSReferenceTime;
        p_SupportedNetworkInfo->gSM                     = p_SuplTriggeredRsp->supportedNetworkInformation->gSM;
        p_SupportedNetworkInfo->wCDMA                   = p_SuplTriggeredRsp->supportedNetworkInformation->wCDMA;

        if(p_SuplTriggeredRsp->supportedNetworkInformation->supportedWLANInfo !=  NULL)
        {
            p_SupportedNetworkInfo->supportedWLANInfo             = (s_GN_SUPL_V2_SupportedWLANInfo* )GN_Calloc( 1, sizeof(s_GN_SUPL_V2_SupportedWLANInfo) );

            p_SupportedNetworkInfo->supportedWLANInfo->apAG       = p_SuplTriggeredRsp->supportedNetworkInformation->supportedWLANInfo->apAG;
            p_SupportedNetworkInfo->supportedWLANInfo->apChanFreq = p_SuplTriggeredRsp->supportedNetworkInformation->supportedWLANInfo->apChanFreq;
            p_SupportedNetworkInfo->supportedWLANInfo->apDevType  = p_SuplTriggeredRsp->supportedNetworkInformation->supportedWLANInfo->apDevType;
            p_SupportedNetworkInfo->supportedWLANInfo->apRepLoc   = p_SuplTriggeredRsp->supportedNetworkInformation->supportedWLANInfo->apRepLoc;
            p_SupportedNetworkInfo->supportedWLANInfo->apRSSI     = p_SuplTriggeredRsp->supportedNetworkInformation->supportedWLANInfo->apRSSI;
            p_SupportedNetworkInfo->supportedWLANInfo->apRTD      = p_SuplTriggeredRsp->supportedNetworkInformation->supportedWLANInfo->apRTD;
            p_SupportedNetworkInfo->supportedWLANInfo->apSN       = p_SuplTriggeredRsp->supportedNetworkInformation->supportedWLANInfo->apSN;
            p_SupportedNetworkInfo->supportedWLANInfo->apTP       = p_SuplTriggeredRsp->supportedNetworkInformation->supportedWLANInfo->apTP;
            p_SupportedNetworkInfo->supportedWLANInfo->setAG      = p_SuplTriggeredRsp->supportedNetworkInformation->supportedWLANInfo->setAG;
            p_SupportedNetworkInfo->supportedWLANInfo->setRSSI    = p_SuplTriggeredRsp->supportedNetworkInformation->supportedWLANInfo->setRSSI;
            p_SupportedNetworkInfo->supportedWLANInfo->setSN      = p_SuplTriggeredRsp->supportedNetworkInformation->supportedWLANInfo->setSN;
            p_SupportedNetworkInfo->supportedWLANInfo->setTP      = p_SuplTriggeredRsp->supportedNetworkInformation->supportedWLANInfo->setTP;
        }
        
        if(p_SuplTriggeredRsp->supportedNetworkInformation->supportedWCDMAInfo !=  NULL)
        {
            p_SupportedNetworkInfo->supportedWCDMAInfo      = (s_GN_SUPL_V2_SupportedWCDMAInfo* )GN_Calloc( 1, sizeof(s_GN_SUPL_V2_SupportedWCDMAInfo) );

            p_SupportedNetworkInfo->supportedWCDMAInfo->mRL = p_SuplTriggeredRsp->supportedNetworkInformation->supportedWCDMAInfo->mRL;
        }
        *p_p_SupportedNetworkInfo = p_SupportedNetworkInfo;

    }
}

//*****************************************************************************
/// \brief
///      Function to add Get Notification from a decoded SUPL-POS-INIT PDU.
///
/// \returns
///      TRUE if a Notification was present in the PDU.
//*****************************************************************************
BL supl_PDU_Get_SUPLNOTIFY_Notification
(
   void                          *p_ThisPDU,          ///< [in/out] Pointer to unencoded structure
                                                      ///   ULP_PDU_t. Passed in as (void *) to hide
                                                      ///   implementation.
   s_GN_SUPL_Notification_Parms  *p_SUPL_Notification ///< [out] Notification
)
{
   ULP_PDU_t        *p_PDU_Cast       = p_ThisPDU;
   Notification_t   *p_Notification   = ( Notification_t *)&p_PDU_Cast->message.choice.msSUPLNOTIFY.notification;

   if ( p_Notification != NULL )
   {
      switch ( ASN1_GET(ASN1_ENUM_1_GET(p_Notification->notificationType )))
      {
      case NotificationType_noNotificationNoVerification:
         p_SUPL_Notification->GN_SUPL_Notify_Type = NOTIFY_NONE; break;
      case NotificationType_notificationOnly:
         p_SUPL_Notification->GN_SUPL_Notify_Type = NOTIFY_ONLY; break;
      case NotificationType_notificationAndVerficationAllowedNA:
         p_SUPL_Notification->GN_SUPL_Notify_Type = NOTIFY_ALLOWED_ON_TIMEOUT; break;
      case NotificationType_notificationAndVerficationDeniedNA:
         p_SUPL_Notification->GN_SUPL_Notify_Type = NOTIFY_DENIED_ON_TIMEOUT; break;
      case NotificationType_privacyOverride:
         p_SUPL_Notification->GN_SUPL_Notify_Type = NOTIFY_PRIVACY_OVERRIDE;
         // There is no notification with privacy override so return now to save
         // superfluous checking.
         return TRUE;
         break;
      default:
         p_SUPL_Notification->GN_SUPL_Notify_Type = NOTIFY_NONE;
         return FALSE;
         break;
      }

       if ( p_Notification->encodingType != NULL)
      {
         switch ( ASN1_GET(ASN1_ENUM_2_GET(p_Notification->encodingType)) )
         {
         case EncodingType_ucs2:        p_SUPL_Notification->GN_SUPL_Encoding_Type = ENCODING_ucs2;       break;
         case EncodingType_gsmDefault:  p_SUPL_Notification->GN_SUPL_Encoding_Type = ENCODING_gsmDefault; break;
         case EncodingType_utf8:        p_SUPL_Notification->GN_SUPL_Encoding_Type = ENCODING_utf8;       break;
         default:          p_SUPL_Notification->GN_SUPL_Encoding_Type = ENCODING_NONE;       break;
         }
      }

       if ( p_Notification->clientName != NULL )
      {
          p_SUPL_Notification->ClientNameLen = p_Notification->clientName->size;
         p_SUPL_Notification->ClientName = GN_Calloc( 1, p_SUPL_Notification->ClientNameLen );
         memcpy(  p_SUPL_Notification->ClientName,
                  p_Notification->clientName->buf,
                  p_SUPL_Notification->ClientNameLen );

         if ( p_Notification->clientNameType != NULL )
         {
            switch ( ASN1_GET(ASN1_ENUM_2_GET(p_Notification->clientNameType)) )
            {
            case FormatIndicator_logicalName:       p_SUPL_Notification->ClientNameType = NOTIFY_logicalName;         break;
            case FormatIndicator_e_mailAddress:     p_SUPL_Notification->ClientNameType = NOTIFY_e_mailAddress;       break;
            case FormatIndicator_msisdn:           p_SUPL_Notification->ClientNameType = NOTIFY_msisdn;              break;
            case FormatIndicator_url:               p_SUPL_Notification->ClientNameType = NOTIFY_url;                 break;
            case FormatIndicator_sipUrl:            p_SUPL_Notification->ClientNameType = NOTIFY_sipUrl;              break;
            case FormatIndicator_min:              p_SUPL_Notification->ClientNameType = NOTIFY_min;                 break;
            case FormatIndicator_mdn:              p_SUPL_Notification->ClientNameType = NOTIFY_mdn;                 break;
            case FormatIndicator_iMSPublicidentity: p_SUPL_Notification->ClientNameType = NOTIFY_imsPublicIdentity;   break;
            default:
               p_SUPL_Notification->ClientNameType = NOTIFY_noFormat;
            break;
            }
         }
         else
         {
            p_SUPL_Notification->ClientNameType = NOTIFY_noFormat;
         }
      }

       if ( p_Notification->requestorId != NULL )
      {
          p_SUPL_Notification->RequestorIdLen = p_Notification->requestorId->size;
         p_SUPL_Notification->RequestorId = GN_Calloc( 1, p_SUPL_Notification->RequestorIdLen );
         memcpy(  p_SUPL_Notification->RequestorId,
                  p_Notification->requestorId->buf,
                  p_SUPL_Notification->RequestorIdLen );

         if ( p_Notification->requestorIdType != NULL )
         {
            switch ( ASN1_GET(ASN1_ENUM_2_GET(p_Notification->requestorIdType)) )
            {
            case FormatIndicator_logicalName:       p_SUPL_Notification->RequestorIdType = NOTIFY_logicalName;        break;
            case FormatIndicator_e_mailAddress:     p_SUPL_Notification->RequestorIdType = NOTIFY_e_mailAddress;      break;
            case FormatIndicator_msisdn:           p_SUPL_Notification->RequestorIdType = NOTIFY_msisdn;             break;
            case FormatIndicator_url:               p_SUPL_Notification->RequestorIdType = NOTIFY_url;                break;
            case FormatIndicator_sipUrl:            p_SUPL_Notification->RequestorIdType = NOTIFY_sipUrl;             break;
            case FormatIndicator_min:              p_SUPL_Notification->RequestorIdType = NOTIFY_min;                break;
            case FormatIndicator_mdn:              p_SUPL_Notification->RequestorIdType = NOTIFY_mdn;                break;
            case FormatIndicator_iMSPublicidentity: p_SUPL_Notification->RequestorIdType = NOTIFY_imsPublicIdentity;  break;
            default:
               p_SUPL_Notification->RequestorIdType = NOTIFY_noFormat;
            break;
            }
         }
         else
         {
            p_SUPL_Notification->RequestorIdType = NOTIFY_noFormat;
         }
      }
      return TRUE;
  }
  else
  {
      return FALSE;
  }


}



//*****************************************************************************
/// \brief
///      Function to add position method into an unencoded SUPL-POS-INIT PDU..
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLPOSINIT_PosMethod
(
   void           *p_ThisPDU,    ///< [in/out] Pointer to unencoded structure
                                 ///   ULP_PDU_t. Passed in as (void *) to hide
                                 ///   implementation.
   e_GN_PosMethod GN_PosMethod   ///< [in] Pos Method.
)
{
   ULP_PDU_t     *p_PDU_Cast       = p_ThisPDU;
   e_PosMethod   temp_PosMethod = noPosition1;

   switch ( GN_PosMethod )
   {
   case GN_PosMethod_agpsSETassisted:    // SET-Assisted mode Only
      temp_PosMethod = agpsSETassisted1;
      break;
   case GN_PosMethod_agpsSETbased:       // SET-Based    mode Only
      temp_PosMethod = agpsSETbased1;
      break;
   case GN_PosMethod_agpsSETassistedpref:// SET-Assisted & SET-Based, but SET-Assisted preferred
      temp_PosMethod = agpsSETassistedpref1;
      break;
   case GN_PosMethod_agpsSETbasedpref:   // SET-Assisted & SET-Based, but SET-Based    preferred
      temp_PosMethod = agpsSETbasedpref1;
      break;
   case GN_PosMethod_autonomousGPS:      // Autonomous GPS
      temp_PosMethod = autonomousGPS1;
      break;
   case GN_PosMethod_aFLT:               // AFLT
      temp_PosMethod = aFLT1;
      break;
   case GN_PosMethod_eCID:               // Enhanced Cell ID
      temp_PosMethod = eCID1;
      break;
   case GN_PosMethod_eOTD:               // EOTD
      temp_PosMethod = eOTD1;
      break;
   case GN_PosMethod_oTDOA:              // O-TDOA
      temp_PosMethod = oTDOA1;
      break;
   case GN_PosMethod_noPosition:         // No position is used for notification and verification of a network based positioning sequence.
      temp_PosMethod = noPosition1;
      break;
   }

   ASN1_ENUM_SET(p_PDU_Cast->message.choice.msSUPLINIT.posMethod,temp_PosMethod);
}


//*****************************************************************************
/// \brief
///      Function to get required Assistance data from a decoded SUPL-POS-INIT PDU.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Get_SUPLPOSINIT_RequestedAssistData
(
   void                       *p_ThisPDU,                ///< [in/out] Pointer to unencoded structure
                                                         ///   ULP_PDU_t. Passed in as (void *) to hide
                                                         ///   implementation.
   s_GN_RequestedAssistData   *p_GN_RequestedAssistData  ///< [Out] Assist required by GPS.
)
{
   ULP_PDU_t              *p_PDU_Cast             = p_ThisPDU;
   RequestedAssistData_t  *p_RequestedAssistData  = p_PDU_Cast->message.choice.msSUPLPOSINIT.requestedAssistData;

  // SatelliteInfo_t   *p_SatelliteInfo;
   SatelliteInfoElement_t *p_SatelliteInfoElement;
   U1                   SatIndex;

   if ( p_PDU_Cast->message.choice.msSUPLPOSINIT.requestedAssistData)  //== SDL_True ) need to be check
   {
      p_GN_RequestedAssistData->almanacRequested               = p_RequestedAssistData->almanacRequested               == SDL_True;
      p_GN_RequestedAssistData->utcModelRequested              = p_RequestedAssistData->utcModelRequested              == SDL_True;
      p_GN_RequestedAssistData->ionosphericModelRequested      = p_RequestedAssistData->ionosphericModelRequested      == SDL_True;
      p_GN_RequestedAssistData->referenceLocationRequested     = p_RequestedAssistData->referenceLocationRequested     == SDL_True;
      p_GN_RequestedAssistData->referenceTimeRequested         = p_RequestedAssistData->referenceTimeRequested         == SDL_True;
      p_GN_RequestedAssistData->navigationModelRequested       = p_RequestedAssistData->navigationModelRequested       == SDL_True;
      p_GN_RequestedAssistData->realTimeIntegrityRequested     = p_RequestedAssistData->realTimeIntegrityRequested     == SDL_True;
      p_GN_RequestedAssistData->acquisitionAssistanceRequested = p_RequestedAssistData->acquisitionAssistanceRequested == SDL_True;
      p_GN_RequestedAssistData->dgpsCorrectionsRequested       = p_RequestedAssistData->dgpsCorrectionsRequested       == SDL_True;
   }
   if ( p_RequestedAssistData->navigationModelData != NULL)
   {
      p_GN_RequestedAssistData->navigationModelDataPresent = TRUE;

      p_GN_RequestedAssistData->gpsWeek   = (U2)p_RequestedAssistData->navigationModelData->gpsWeek;
      p_GN_RequestedAssistData->gpsToe    = (U1)p_RequestedAssistData->navigationModelData->gpsToe;
      p_GN_RequestedAssistData->nSAT      = (U1)p_RequestedAssistData->navigationModelData->nSAT;
      p_GN_RequestedAssistData->toeLimit  = (U1)p_RequestedAssistData->navigationModelData->toeLimit;

      if ( p_RequestedAssistData->navigationModelData->satInfo != NULL)
      {
         p_GN_RequestedAssistData->satInfoPresent = TRUE;

         p_SatelliteInfoElement = p_RequestedAssistData->navigationModelData->satInfo->list.array[0];

         for ( SatIndex = 0 ; SatIndex < p_GN_RequestedAssistData->nSAT ; SatIndex++ )
         {
            p_SatelliteInfoElement = p_RequestedAssistData->navigationModelData->satInfo->list.array[SatIndex]; //.satInfo.First;
            p_GN_RequestedAssistData->IODE[SatIndex]  = (U1)p_SatelliteInfoElement->iODE;
            p_GN_RequestedAssistData->SatID[SatIndex] = (U1)p_SatelliteInfoElement->satId;

         }
      }

   }

   return;
}




//*****************************************************************************
/// \brief
///      Function to add required Assistance data into an unencoded SUPL-POS-INIT PDU.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLPOSINIT_RequestedAssistData
(
   void *p_ThisPDU,  ///< [in/out] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   s_GN_AGPS_Assist_Req *p_GN_AGPS_Assist_Req,  ///< [in] Assist required by GPS.
   s_GN_SetCapabilities *p_SetCapabilities      ///< [out] Set Capabilities.
)
{

   RequestedAssistData_t *p_RequestedAssistData ;
   ULP_PDU_t             *p_PDU_Cast              = p_ThisPDU;
   RequestedAssistData_t **p_p_RequestedAssistData = &p_PDU_Cast->message.choice.msSUPLPOSINIT.requestedAssistData;
   U1                   SatCount;
   BL                   requestAssist           = TRUE;
   BL                   requestMsBasedAssist    = TRUE;
   BL                   requestMsAssistedAssist = TRUE;

   /* To remove compiler warnings */
   p_SetCapabilities = p_SetCapabilities;

   ASN1_SET(p_p_RequestedAssistData) = (RequestedAssistData_t*)GN_Calloc( 1, sizeof(RequestedAssistData_t) );
   p_RequestedAssistData = ASN1_GET(p_p_RequestedAssistData);

   //TODO: Add ASN.1 definitions

   #if FALSE // enable the following to suppress assistance based on set capabilities.
      if ( p_SetCapabilities->PosTechnology.agpsSETassisted == FALSE )
      {
         requestMsAssistedAssist = FALSE;
      }
      if ( p_SetCapabilities->PosTechnology.agpsSETBased == FALSE )
      {
         requestMsBasedAssist    = FALSE;
      }
      requestAssist = requestMsBasedAssist || requestMsAssistedAssist || p_SetCapabilities->PosTechnology.autonomousGPS ;
   #endif

   if ( requestAssist )
   {
      // Set this here to remove "possibly used before being set" warnings.
      // Always ask for dgps corrections and only ask for ref time and bad sv list when the library wants it.
      //p_RequestedAssistData->dgpsCorrectionsRequested       = SDL_True;
      if ( p_GN_AGPS_Assist_Req->Ref_Time_Req )    p_RequestedAssistData->referenceTimeRequested      = SDL_True;
      if ( p_GN_AGPS_Assist_Req->Bad_SV_List_Req ) p_RequestedAssistData->realTimeIntegrityRequested  = SDL_True;

      if ( requestMsAssistedAssist )
      {
         // msAssisted specific assistance has not been suppressed based on set capabilities.
         p_RequestedAssistData->acquisitionAssistanceRequested = SDL_True;
      }

      if ( requestMsBasedAssist )
      {
         // msBased specific assistance has not been suppressed based on set capabilities.
#ifndef DO_NOT_USE_ALMANAC
         if ( p_GN_AGPS_Assist_Req->Alm_Req )      p_RequestedAssistData->almanacRequested            = SDL_True;
#endif /* #ifdef DO_NOT_USE_ALMANAC */
         if ( p_GN_AGPS_Assist_Req->UTC_Req )      p_RequestedAssistData->utcModelRequested           = SDL_True;
         if ( p_GN_AGPS_Assist_Req->Ion_Req )      p_RequestedAssistData->ionosphericModelRequested   = SDL_True;
         if ( p_GN_AGPS_Assist_Req->Ref_Pos_Req )  p_RequestedAssistData->referenceLocationRequested  = SDL_True;
         if ( p_GN_AGPS_Assist_Req->Eph_Req )      p_RequestedAssistData->navigationModelRequested    = SDL_True;

         if ( p_RequestedAssistData->navigationModelRequested )
         {
               NavigationModel_t *p_NavigationModel;

            p_PDU_Cast->message.choice.msSUPLPOSINIT.requestedAssistData->navigationModelData
               = (NavigationModel_t*)GN_Calloc( 1, sizeof(NavigationModel_t) );

            p_NavigationModel = p_PDU_Cast->message.choice.msSUPLPOSINIT.requestedAssistData->navigationModelData;

            p_NavigationModel->gpsWeek  = p_GN_AGPS_Assist_Req->gpsWeek % 1024;
            p_NavigationModel->gpsToe   = p_GN_AGPS_Assist_Req->Toe;
            p_NavigationModel->nSAT     = p_GN_AGPS_Assist_Req->Num_Sat;
            p_NavigationModel->toeLimit = p_GN_AGPS_Assist_Req->Toe_Limit;

            if(p_GN_AGPS_Assist_Req->Num_Sat > 0)
               {
                  p_NavigationModel->satInfo = (SatelliteInfo_t*)GN_Calloc( 1, sizeof(SatelliteInfo_t) );

               for ( SatCount = 0 ; SatCount < p_GN_AGPS_Assist_Req->Num_Sat ; SatCount++ )
               {
                  SatelliteInfoElement_t *p_Element = GN_Calloc( 1, sizeof( SatelliteInfoElement_t ));

                  p_Element->iODE  = p_GN_AGPS_Assist_Req->IODE[SatCount];
                  p_Element->satId = p_GN_AGPS_Assist_Req->SatID[SatCount];

                  asn_sequence_add( &p_NavigationModel->satInfo->list , p_Element );
               }
            }
           }
      }
   }

   return;
}


//*****************************************************************************
/// \brief
///      Function to add Set Location ID into an unencoded SUPL-POS-INIT PDU.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLPOSINIT_SetLocationID
(
   void *p_ThisPDU,  ///< [in/out] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   s_LocationId *p_SetLocationID    ///< [in] Set Location ID.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   supl_PDU_Add_SetLocationID(
      &p_PDU_Cast->message.choice.msSUPLPOSINIT.locationId,
      p_SetLocationID );
}


//*****************************************************************************
/// \brief
///      Function to add Get Location ID from a decoded SUPL-POS-INIT PDU.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Get_SUPLPOSINIT_SetLocationID
(
   void *p_ThisPDU,  ///< [in/out] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   s_LocationId *p_SetLocationID    ///< [out] Set Location ID.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   supl_PDU_Get_SetLocationID(
      p_SetLocationID,
      &p_PDU_Cast->message.choice.msSUPLPOSINIT.locationId);
}


//*****************************************************************************
/// \brief
///      Function to convert a QoP from the native asn1c format.
///
/// \returns
///      Nothing.
//*****************************************************************************
BL supl_PDU_Get_QoP(
   QoP_t      *p_QoP,        ///< [in] Native asn1c QoP format.
   s_GN_QoP **p_p_GN_QoP   ///< [out] GN QoP format.
)
{
   s_GN_QoP *p_GN_QoP = *p_p_GN_QoP;

   p_GN_QoP->horacc    = (U1)p_QoP->horacc;
   p_GN_QoP->p_horacc  = &p_GN_QoP->horacc;

  if ( p_QoP->veracc != NULL)
   {
      p_GN_QoP->veracc      = (uint8_t)ASN1_GET(p_QoP->veracc);
      p_GN_QoP->p_veracc    = &p_GN_QoP->veracc;
   }
   if ( p_QoP->delay != NULL )
   {
      p_GN_QoP->delay       = (uint8_t)ASN1_GET(p_QoP->delay);
      p_GN_QoP->p_delay     = &p_GN_QoP->delay;
   }
   if ( p_QoP->maxLocAge != NULL)
   {
      p_GN_QoP->maxLocAge   = (uint16_t)ASN1_GET(p_QoP->maxLocAge);
      p_GN_QoP->p_maxLocAge = &p_GN_QoP->maxLocAge;
   }
   return TRUE;
}



//*****************************************************************************
/// \brief
///      Function to convert a QoP from the native asn1c format.
///
/// \returns
///      Nothing.
//*****************************************************************************
BL supl_PDU_Get_SUPLINIT_QoP(
   void     *p_ThisPDU,    ///< [in] Pointer to decoded structure
                           ///   ULP_PDU_t. Passed in as (void *) to hide
                           ///   implementation.
   s_GN_QoP **p_p_GN_QoP   ///< [out] QoP extracted from the PDU in GN format.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   if ( p_PDU_Cast->message.choice.msSUPLINIT.qoP == NULL )
   {
      return FALSE;
   }
   if ( *p_p_GN_QoP == NULL )
   {
      *p_p_GN_QoP = GN_Calloc( 1, sizeof( s_GN_QoP ) );
   }
   else
   {
      return FALSE;
   }

   return supl_PDU_Get_QoP( p_PDU_Cast->message.choice.msSUPLINIT.qoP, p_p_GN_QoP );
}


//*****************************************************************************
/// \brief
///      Function to convert a QoP into the native asn1c format.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_QoP
(
   QoP_t **p_QoP,       ///< [out] Native asn1c QoP format.
   U1 *p_horacc,     ///< [in] Horizontal accuracy. NULL = Not available.
   U1 *p_veracc,     ///< [in] Vertical accuracy. NULL = Not available.
   U2 *p_maxLocAge,  ///< [in] Maximum age of location. NULL = Not available.
   U1 *p_delay       ///< [in] Maximum permissable delay. NULL = Not available.
)
{
  QoP_t *p_QopTemp;
  if (ASN1_GET(p_QoP) == NULL)
   {
     SUPL_HEAP_ALLOC(ASN1_SET(p_QoP), QoP_t);

    if (ASN1_GET(p_QoP) == NULL)
    {
      GN_SUPL_Log( "Heap allocation error (qop_t)" );
    }
  }

  p_QopTemp = ASN1_GET(p_QoP);
  p_QopTemp->maxLocAge = NULL;
  p_QopTemp->veracc =NULL;
  p_QopTemp->delay = NULL;
  if ( p_horacc != NULL )
   {
       p_QopTemp->horacc = *p_horacc;
      if ( p_veracc != NULL )
      {
         //p_QoP->veraccPresent    = SDL_True;
         ASN1_PARAM_ALLOC(p_QopTemp->veracc, long);
         ASN1_SET(p_QopTemp->veracc) = *p_veracc;
      }
      if ( p_maxLocAge != NULL )
      {
         //p_QoP->maxLocAgePresent = SDL_True;
         ASN1_PARAM_ALLOC(p_QopTemp->maxLocAge, long);
         ASN1_SET(p_QopTemp->maxLocAge ) =  *p_maxLocAge;
      }
      if ( p_delay != NULL )
      {
        // p_QoP->delayPresent     = SDL_True;
         ASN1_PARAM_ALLOC(p_QopTemp->delay, long);
         ASN1_SET(p_QopTemp->delay)  = *p_delay;
      }
   }
    ASN1_SET(p_QoP) = p_QopTemp;
   return;
}


//*****************************************************************************
/// \brief
///      Function to add QoP into an unencoded SUPL-START PDU.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLSTART_QoP
(
   void *p_ThisPDU,  ///< [in/out] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   U1 *p_horacc,     ///< [in] Horizontal accuracy. NULL = Not available.
   U1 *p_veracc,     ///< [in] Vertical accuracy. NULL = Not available.
   U2 *p_maxLocAge,  ///< [in] Maximum age of location. NULL = Not available.
   U1 *p_delay       ///< [in] Maximum permissable delay. NULL = Not available.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   if ( p_horacc != NULL )
   {
      supl_PDU_Add_QoP(
          &p_PDU_Cast->message.choice.msSUPLSTART.qoP,
         p_horacc,
         p_veracc,
         p_maxLocAge,
         p_delay );
    }
}
//*****************************************************************************
/// \brief
///      Function to add QoP into an unencoded SUPL-SETINIT PDU.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLSETINIT_QoP
(
   void *p_ThisPDU,  ///< [in/out] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   U1 *p_horacc,     ///< [in] Horizontal accuracy. NULL = Not available.
   U1 *p_veracc,     ///< [in] Vertical accuracy. NULL = Not available.
   U2 *p_maxLocAge,  ///< [in] Maximum age of location. NULL = Not available.
   U1 *p_delay       ///< [in] Maximum permissable delay. NULL = Not available.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   if ( p_horacc != NULL )
   {
      supl_PDU_Add_QoP(
          &p_PDU_Cast->message.choice.msSUPLSETINIT.qoP,
         p_horacc,
         p_veracc,
         p_maxLocAge,
         p_delay );
    }
}


//*****************************************************************************
/// \brief
///      High level function to populate an unencoded SUPL-START PDU.
///
/// \returns
///      Pointer to unencoded structure ULP_PDU_t. Returned as (void *) to hide
///      implementation.
//*****************************************************************************
void *supl_PDU_Make_SUPLSTART
(
   s_SUPL_Instance *p_SUPL_Instance ///< [in] Current instance data used to
                                    ///   populate parts of the PDU.
)
{
   void *p_PDU_Src = 0;
   s_GN_SetCapabilities SetCapabilities;
   s_SUPL_InstanceData  *p_SUPL = p_SUPL_Instance->p_SUPL; // SUPL specific data.

   p_PDU_Src = supl_PDU_Alloc();
   GN_SUPL_Log( "SUPL-START Alloc: %08p", p_PDU_Src );

   supl_PDU_Add_Version( p_PDU_Src , p_SUPL_Instance );

   supl_PDU_Add_setSessionID( p_PDU_Src, p_SUPL->SetSessionId, &p_SUPL->SetId );

   // Add the SUPL-START message.
   supl_PDU_Add_SUPLSTART( p_PDU_Src );
   
   supl_get_SetCapabilities( p_SUPL_Instance , &SetCapabilities );  

   supl_PDU_Add_SUPLSTART_SetCapabilities( p_PDU_Src, &SetCapabilities, p_SUPL_Instance );

   supl_PDU_Add_SUPLSTART_SetLocationID(   p_PDU_Src, &p_SUPL->LocationId );

   if(p_SUPL->V2_Data.p_ThirdPartyData != NULL)
   {
      supl_PDU_Add_SUPLSTART_ThirdPartyInfo(p_PDU_Src, p_SUPL->V2_Data.p_ThirdPartyData);
   }

   if(p_SUPL->V2_Data.p_ApplicationID != NULL)
   {
      supl_PDU_Add_SUPLSTART_ApplicationID(p_PDU_Src, p_SUPL->V2_Data.p_ApplicationID);
   }
   if(( supl_Is_Version2( p_SUPL_Instance )) && ( SUPL_Mlid_FirstElem != NULL) )
   {
      supl_PDU_Add_SUPLSTART_Multiple_LocationID( p_PDU_Src , p_SUPL_Instance );
   }

   if ( p_SUPL->p_GN_QoP != NULL )
   {
      U1 *p_horacc =    p_SUPL->p_GN_QoP->p_horacc;
      U1 *p_veracc =    p_SUPL->p_GN_QoP->p_veracc;
      U2 *p_maxLocAge = p_SUPL->p_GN_QoP->p_maxLocAge;
      U1 *p_delay =     p_SUPL->p_GN_QoP->p_delay;

      supl_PDU_Add_SUPLSTART_QoP( p_PDU_Src, p_horacc, p_veracc, p_maxLocAge, p_delay );
      GN_SUPL_Log_QoP( p_SUPL_Instance->Handle , p_SUPL->p_GN_QoP );
   }
   return p_PDU_Src;
}

//*****************************************************************************
/// \brief
///      High level function to populate an unencoded SUPL-SETINIT PDU.
///
/// \returns
///      Pointer to unencoded structure ULP_PDU_t. Returned as (void *) to hide
///      implementation.
//*****************************************************************************
void *supl_PDU_Make_SUPLSETINIT
(
   s_SUPL_Instance *p_SUPL_Instance ///< [in] Current instance data used to
                                    ///   populate parts of the PDU.
)
{
   /*SUPL_SETINIT message should be having*/
   /*1.  Session ID*/
   /*2.  Target ID Info - Mandatory*/
   /*3.  Optional QoP field */
   /*4.   Optional Application ID field*/
   void *p_PDU_Src = 0;
   s_SUPL_InstanceData  *p_SUPL = p_SUPL_Instance->p_SUPL; // SUPL specific data.

   p_PDU_Src = supl_PDU_Alloc();
   GN_SUPL_Log( "SUPL-START Alloc: %08p", p_PDU_Src );

   supl_PDU_Add_Version( p_PDU_Src, p_SUPL_Instance);

   supl_PDU_Add_setSessionID( p_PDU_Src, p_SUPL->SetSessionId, &p_SUPL->SetId );
   //supl_PDU_Add_slpSessionID( p_PDU_Src, p_SUPL->p_SlpSessionId, &p_SUPL->SlpId );

   // Add the SUPL-SETINIT message.
   supl_PDU_Add_SUPLSETINIT( p_PDU_Src );
   /*The Target SET Info should be populated on to  the SUPL SETINIT PDU*/
   if(p_SUPL->V2_Data.p_ThirdPartyData != NULL)
   {
      /*Target ID Info is Mandatory so populate the Info first to the PDU*/
      supl_PDU_Add_SUPLSETINIT_ThirdPartyInfo(p_PDU_Src, p_SUPL->V2_Data.p_ThirdPartyData);
   }
   /*Will need to implement Application id info on to SUPL_SETINIT pdu later as it is optional*/
   if(p_SUPL->V2_Data.p_ApplicationID != NULL)
   {
      supl_PDU_Add_SUPLSETINIT_AppIdInfo(p_PDU_Src, p_SUPL->V2_Data.p_ApplicationID);
   }
   /*QoP field is optional in the SUPL SETINIT message. We have the QoP sent from CGPS so we will populate the PDU*/
   if ( p_SUPL->p_GN_QoP != NULL )
   {
      U1 *p_horacc =    p_SUPL->p_GN_QoP->p_horacc;
      U1 *p_veracc =    p_SUPL->p_GN_QoP->p_veracc;
      U2 *p_maxLocAge = p_SUPL->p_GN_QoP->p_maxLocAge;
      U1 *p_delay =     p_SUPL->p_GN_QoP->p_delay;

      supl_PDU_Add_SUPLSETINIT_QoP( p_PDU_Src, p_horacc, p_veracc, p_maxLocAge, p_delay );
      GN_SUPL_Log_QoP( p_SUPL_Instance->Handle , p_SUPL->p_GN_QoP );
   }

   return p_PDU_Src;
}


//*****************************************************************************
/// \brief
///      Function to set the unencoded SUPL PDU type to SUPL-RESPONSE.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLRESPONSE
(
   void *p_ThisPDU   ///< [in/out] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   // SUPL-RESPONSE defined in SUPL-RESPONSE.asn1
   p_PDU_Cast->message.present = UlpMessage_PR_msSUPLRESPONSE;
}


//*****************************************************************************
/// \brief
///      Function to add PosMethod to the unencoded SUPL PDU type SUPL-RESPONSE.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLRESPONSE_PosMethod
(
   void *p_ThisPDU,  ///< [in] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   e_GN_PosMethod GN_PosMethod
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;
   PosMethod_t *p_posMethod;

   p_posMethod = &p_PDU_Cast->message.choice.msSUPLRESPONSE.posMethod;

   switch ( GN_PosMethod )
   {
   case GN_PosMethod_agpsSETassisted:     *(p_posMethod->buf) = agpsSETassisted1;       break;
   case GN_PosMethod_agpsSETbased:        *(p_posMethod->buf) = agpsSETbased1;          break;
   case GN_PosMethod_agpsSETassistedpref: *(p_posMethod->buf) = agpsSETassistedpref1;   break;
   case GN_PosMethod_agpsSETbasedpref:    *(p_posMethod->buf) = agpsSETbasedpref1;      break;
   case GN_PosMethod_autonomousGPS:       *(p_posMethod->buf) = autonomousGPS1;         break;
   case GN_PosMethod_aFLT:                *(p_posMethod->buf) = aFLT1;                  break;
   case GN_PosMethod_eCID:                *(p_posMethod->buf) = eCID1;                  break;
   case GN_PosMethod_eOTD:                *(p_posMethod->buf) = eOTD1;                  break;
   case GN_PosMethod_oTDOA:               *(p_posMethod->buf) = oTDOA1;                 break;
   case GN_PosMethod_noPosition:          *(p_posMethod->buf) = noPosition1;            break;
   default:
      OS_ASSERT( 0 );
      break;
   }
}



//*****************************************************************************
/// \brief
///      High level function to populate an unencoded SUPL-RESPONSE PDU.
///
/// \returns
///      Pointer to unencoded structure ULP_PDU_t. Returned as (void *) to hide
///      implementation.
//*****************************************************************************
void *supl_PDU_Make_SUPLRESPONSE
(
   s_SUPL_Instance *p_SUPL_Instance ///< [in] Current instance data used to
                                    ///   populate parts of the PDU.
)
{
   void *p_PDU_Src = 0;
   s_SUPL_InstanceData  *p_SUPL = p_SUPL_Instance->p_SUPL; // SUPL specific data.

   p_PDU_Src = supl_PDU_Alloc();
   GN_SUPL_Log( "SUPL-RESPONSE Alloc: %08p", p_PDU_Src );

   supl_PDU_Add_Version( p_PDU_Src , p_SUPL_Instance);

   supl_PDU_Add_setSessionID( p_PDU_Src, p_SUPL->SetSessionId,   &p_SUPL->SetId );
   supl_PDU_Add_slpSessionID( p_PDU_Src, p_SUPL->p_SlpSessionId, &p_SUPL->SlpId );

   // Add the SUPL-RESPONSE message.
   ///\todo Remove hard coding of PosMethod.
   supl_PDU_Add_SUPLRESPONSE( p_PDU_Src );
   supl_PDU_Add_SUPLRESPONSE_PosMethod( p_PDU_Src, GN_PosMethod_agpsSETbased );

   //supl_PDU_Add_SUPLRESPONSE_SLPAddress(p_PDU_Src, supl_config_get_SUPL_SLP_address());

   return p_PDU_Src;
}



//*****************************************************************************
/// \brief
///      Function to generate and add VER to the unencoded SUPL PDU type
///      SUPL-POS-INIT.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLPOSINIT_VER
(
   void  *p_ThisPDU, ///< [in] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   U1*   p_VER       ///< VER generated from SUPL-INIT.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   if ( p_VER != NULL )
   {
      p_PDU_Cast->message.choice.msSUPLPOSINIT.ver = GN_Calloc(1, sizeof(Ver_t));

      supl_PDU_Add_SDL_BitString_U1_ptr( p_PDU_Cast->message.choice.msSUPLPOSINIT.ver , p_VER , GN_GPS_VER_LEN * 8 );
    }
}



//*****************************************************************************
/// \brief
///      Function to add Set capabilities into an unencoded SUPL-POS-INIT PDU.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLPOSINIT_SetCapabilities
(
   void *p_ThisPDU,  ///< [in/out] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   s_GN_SetCapabilities *p_SetCapabilities, ///< [in] Set capabilities in
                                            ///   GN format.
   s_SUPL_Instance *p_SUPL_Instance        /// Added for version check                                  
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   supl_PDU_Add_SetCapabilities(
       &p_PDU_Cast->message.choice.msSUPLPOSINIT.sETCapabilities,
       p_SetCapabilities , p_SUPL_Instance );
}


//*****************************************************************************
/// \brief
///      Function to set the unencoded SUPL PDU type to SUPL-POS-INIT.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLPOSINIT
(
   void *p_ThisPDU   ///< [in/out] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   // SUPL-POS-INIT defined in SUPL-POS-INIT.asn1
   p_PDU_Cast->message.present = UlpMessage_PR_msSUPLPOSINIT;
}


//*****************************************************************************
/// \brief
///      High level function to populate an unencoded SUPL-POS-INIT PDU.
///
/// \returns
///      Pointer to unencoded structure ULP_PDU_t. Returned as (void *) to hide
///      implementation.
//*****************************************************************************
void *supl_PDU_Make_SUPLPOSINIT
(
   s_SUPL_Instance *p_SUPL_Instance ///< [in] Current instance data used to
                                    ///   populate parts of the PDU.
)
{
   void *p_PDU_Src = 0;
   s_GN_SetCapabilities SetCapabilities;
   s_GN_AGPS_Assist_Req GN_AGPS_Assist_Req;
   BL Assist_Required = FALSE;
   s_SUPL_InstanceData  *p_SUPL = p_SUPL_Instance->p_SUPL; // SUPL specific data.

   p_PDU_Src = supl_PDU_Alloc();
   GN_SUPL_Log( "SUPL-POSINIT Alloc: %08p", p_PDU_Src );

   supl_PDU_Add_Version( p_PDU_Src ,p_SUPL_Instance );

   supl_PDU_Add_setSessionID( p_PDU_Src, p_SUPL->SetSessionId,   &p_SUPL->SetId );
   supl_PDU_Add_slpSessionID( p_PDU_Src, p_SUPL->p_SlpSessionId, &p_SUPL->SlpId );

   // Add the SUPL-POS-INIT message.
   supl_PDU_Add_SUPLPOSINIT( p_PDU_Src );

   // Note: This needed to be removed for testing with Andrew.
   if ( p_SUPL->p_VER != NULL )
   {
      supl_PDU_Add_SUPLPOSINIT_VER( p_PDU_Src, p_SUPL->p_VER );
   }

   supl_get_SetCapabilities( p_SUPL_Instance , &SetCapabilities );

   if(( supl_Is_Version2( p_SUPL_Instance )) && ( SUPL_Mlid_FirstElem != NULL) )
   {
      supl_PDU_Add_SUPLPOSINIT_Multiple_LocationID( p_PDU_Src , p_SUPL_Instance );
   }

   supl_PDU_Add_SUPLPOSINIT_SetCapabilities( p_PDU_Src, &SetCapabilities , p_SUPL_Instance);

   // Note: This needed to be removed for testing with Andrew.
   Assist_Required = GN_AGPS_Get_Assist_Req( &GN_AGPS_Assist_Req );

/* +Change to always request some assistance data */
#ifdef SUPL_ATLEAST_REQ_ASSIST_DATA_POS_TIME
   if ( !Assist_Required )
   {
         /* Fill in some default requests */
         memset( &GN_AGPS_Assist_Req , 0 , sizeof(GN_AGPS_Assist_Req) );

         /* Reference location and time are small
         and will not required lot of data */
         GN_AGPS_Assist_Req.Ref_Pos_Req = 1;
       GN_AGPS_Assist_Req.Ref_Time_Req = 1;
   }
#else /* SUPL_ATLEAST_REQ_ASSIST_DATA_POS_TIME */
   if( Assist_Required )
   {
#endif /* SUPL_ATLEAST_REQ_ASSIST_DATA_POS_TIME */
   supl_PDU_Add_SUPLPOSINIT_RequestedAssistData( p_PDU_Src, &GN_AGPS_Assist_Req, &SetCapabilities);
#ifndef SUPL_ATLEAST_REQ_ASSIST_DATA_POS_TIME
   }
#endif /* SUPL_ATLEAST_REQ_ASSIST_DATA_POS_TIME */
/* -Change to always request some assistance data */


   supl_PDU_Add_SUPLPOSINIT_SetLocationID( p_PDU_Src, &p_SUPL->LocationId );

   return p_PDU_Src;
}


//*****************************************************************************
/// \brief
///      Function to add Status Code to the unencoded SUPL PDU type SUPL-END.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLEND_StatusCode
(
   void *p_ThisPDU,  ///< [in/out] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   e_GN_StatusCode GN_StatusCode ///< [in] GN format status code.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   p_PDU_Cast->message.choice.msSUPLEND.statusCode = GN_Calloc(1,sizeof(StatusCode_t));


   switch( GN_StatusCode )
   {
   case  GN_StatusCode_unspecified:                ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLEND.statusCode,unspecified); /* *p_statusCode = unspecified;*/ break;
   case  GN_StatusCode_systemFailure:              ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLEND.statusCode,systemFailure);/*(p_statusCode->buf)=systemFailure;*/            break;
   case  GN_StatusCode_unexpectedMessage:          ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLEND.statusCode,unexpectedMessage);        break;
   case  GN_StatusCode_protocolError:              ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLEND.statusCode,protocolError);            break;
   case  GN_StatusCode_dataMissing:                ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLEND.statusCode,dataMissing);              break;
   case  GN_StatusCode_unexpectedDataValue:        ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLEND.statusCode,unexpectedDataValue);      break;
   case  GN_StatusCode_posMethodFailure:           ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLEND.statusCode,posMethodFailure);         break;
   case  GN_StatusCode_posMethodMismatch:          ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLEND.statusCode,posMethodMismatch);        break;
   case  GN_StatusCode_posProtocolMismatch:        ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLEND.statusCode,posProtocolMismatch);      break;
   case  GN_StatusCode_targetSETnotReachable:      ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLEND.statusCode,targetSETnotReachable);    break;
   case  GN_StatusCode_versionNotSupported:        ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLEND.statusCode,versionNotSupported);      break;
   case  GN_StatusCode_resourceShortage:           ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLEND.statusCode, resourceShortage);         break;
   case  GN_StatusCode_invalidSessionId:           ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLEND.statusCode,invalidSessionId);         break;
   case  GN_StatusCode_nonProxyModeNotSupported:   ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLEND.statusCode,nonProxyModeNotSupported); break;
   case  GN_StatusCode_proxyModeNotSupported:      ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLEND.statusCode,proxyModeNotSupported);    break;
   case  GN_StatusCode_positioningNotPermitted:    ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLEND.statusCode, positioningNotPermitted);  break;
   case  GN_StatusCode_authNetFailure:             ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLEND.statusCode, authNetFailure);           break;
   case  GN_StatusCode_authSuplinitFailure:        ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLEND.statusCode, authSuplinitFailure);      break;
   case  GN_StatusCode_consentDeniedByUser:        ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLEND.statusCode,consentDeniedByUser);       break;
   case  GN_StatusCode_consentGrantedByUser:       ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLEND.statusCode,consentGrantedByUser);     break;
   case  GN_StatusCode_ver2_sessionStopped:       ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLEND.statusCode,ver2_sessionStopped);     break;
   default:
      OS_ASSERT( 0 );
      break;
   }
}



//*****************************************************************************
/// \brief
///      Function to generate and add VER to the unencoded SUPL PDU type
///      SUPL-END.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLEND_VER
(
   void  *p_ThisPDU, ///< [in] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   s_SUPL_Instance *p_SUPL_Instance     ///< VER generated from SUPL-INIT.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   /*If SUPL PDU was not succesfully decoded,it is assumed that session is a SUPL 1.0 transaction */

   if ( p_SUPL_Instance->p_SUPL->p_VER != NULL )
   {
      p_PDU_Cast->message.choice.msSUPLEND.ver = GN_Calloc(1, sizeof(Ver_t));

      supl_PDU_Add_SDL_BitString_U1_ptr( p_PDU_Cast->message.choice.msSUPLEND.ver, p_SUPL_Instance->p_SUPL->p_VER, GN_GPS_VER_LEN * 8 );
   }
}



//*****************************************************************************
/// \brief
///      Function to add Position Estimate to the unencoded SUPL PDU type SUPL-END.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_Position
(
   Position_t *p_Position,  ///< [in/out] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   s_GN_Position  *p_GN_Position // [in] Position
)
{
   //Position ::= SEQUENCE {
   //  timestamp         UTCTime, -- shall include seconds and shall use UTC time.
   //  positionEstimate  PositionEstimate,
   //  velocity          Velocity OPTIONAL,
   //  ...}

   {  // Encode Timestamp Visible strings in SDL start with a "V"     VYYYYMMDDHHMMSS Z+EOS
        //initialise timestamp
      // ASN1_UTC_SET(p_Position->timestamp, NULL);
    p_Position->timestamp.buf = GN_Calloc( 1,  4+2+2+2+2+2+1 + 1 +1);
    p_Position->timestamp.size = 4+2+2+2+2+2+1+1+1;

    sprintf( (CH*)p_Position->timestamp.buf, "%s", p_GN_Position->timestamp );
   }
   // Encode Latitude.
   p_Position->positionEstimate.latitude     = p_GN_Position->latitude;
   ASN1_ENUM_SET(p_Position->positionEstimate.latitudeSign, p_GN_Position->latitudeSign);


   // Encode Longitude.
   p_Position->positionEstimate.longitude    = p_GN_Position->longitude;

   if ( p_GN_Position->uncertaintyPresent )
   {
      // Encode Uncertainty.
      p_Position->positionEstimate.uncertainty = GN_Calloc(1,sizeof(p_Position->positionEstimate.uncertainty)); //need to check
      p_Position->positionEstimate.uncertainty->uncertaintySemiMajor = p_GN_Position->uncertaintySemiMajor;
      p_Position->positionEstimate.uncertainty->uncertaintySemiMinor = p_GN_Position->uncertaintySemiMinor;
      /* + LMSqc37015 */
      p_Position->positionEstimate.uncertainty->orientationMajorAxis = p_GN_Position->orientationMajorAxis;
      /* - LMSqc37015 */
   }
   else
   {
      p_Position->positionEstimate.uncertainty = NULL;
   }

   if ( p_GN_Position->confidencePresent )
   {
      // Encode Confidence.
       p_Position->positionEstimate.confidence = (long*)GN_Calloc(1,sizeof(long));
      *p_Position->positionEstimate.confidence        = p_GN_Position->confidence;
   }
   else
   {
      p_Position->positionEstimate.confidence = NULL;
   }

   if ( p_GN_Position->altitudeInfoPresent )
   {
   // Encode Altitude.
      p_Position->positionEstimate.altitudeInfo = (AltitudeInfo_t*)GN_Calloc(1,sizeof(AltitudeInfo_t));
      p_Position->positionEstimate.altitudeInfo->altitude          = p_GN_Position->altitude;
      ASN1_ENUM_SET(p_Position->positionEstimate.altitudeInfo->altitudeDirection, p_GN_Position->altitudeDirection);
      /* + LMSqc37015 */
      p_Position->positionEstimate.altitudeInfo->altUncertainty    = p_GN_Position->altUncertainty;
      /* - LMSqc37015 */
   }
   else
   {
      p_Position->positionEstimate.altitudeInfo = NULL;
   }

   /* + LMSqc37015 */
   if( p_GN_Position->velocityPresent == TRUE )
   {
   /* - LMSqc37015 */
       p_Position->velocity = (Velocity_t*)GN_Calloc(1, sizeof(Velocity_t));
      switch ( p_GN_Position->VelocityIndicator )
      {
      case GN_horvel:
         // Encode Velocity.
          p_Position->velocity->present = Velocity_PR_horvel;

         // ULP-Components ASN.1 - bearing         BIT STRING(SIZE (9)),
         supl_PDU_Add_SDL_BitString_U4(
             &p_Position->velocity->choice.horvel.bearing,
            (U4) p_GN_Position->bearing,
            9 );

         // ULP-Components ASN.1 - horspeed        BIT STRING(SIZE (16)),
         supl_PDU_Add_SDL_BitString_U4(
            &p_Position->velocity->choice.horvel.horspeed,
            (U4) p_GN_Position->horspeed,
            16 );

         break;
      case GN_horandvervel:
         // Encode Velocity.
        // p_Position->velocity1Present  = SDL_True;
          p_Position->velocity->present = Velocity_PR_horandvervel;
         // ULP-Components ASN.1 - verdirect       BIT STRING(SIZE (1)),
         supl_PDU_Add_SDL_BitString_U4(
            &p_Position->velocity->choice.horandvervel.verdirect,
            (U4) p_GN_Position->verdirect,
            1 );

         // ULP-Components ASN.1 - bearing         BIT STRING(SIZE (9)),
         supl_PDU_Add_SDL_BitString_U4(
            &p_Position->velocity->choice.horandvervel.bearing,
            (U4) p_GN_Position->bearing,
            9 );

         // ULP-Components ASN.1 - horspeed        BIT STRING(SIZE (16)),
         supl_PDU_Add_SDL_BitString_U4(
            &p_Position->velocity->choice.horandvervel.horspeed,
            (U4) p_GN_Position->horspeed,
            16 );

         // ULP-Components ASN.1 - verspeed        BIT STRING(SIZE (8)),
         supl_PDU_Add_SDL_BitString_U4(
            &p_Position->velocity->choice.horandvervel.verspeed,
            (U4) p_GN_Position->verspeed,
            8 );

         break;
      case GN_horveluncert:
         // Encode Velocity.
         //p_Position->velocity1Present  = SDL_True;
         p_Position->velocity->present = Velocity_PR_horveluncert;

         // ULP-Components ASN.1 - bearing         BIT STRING(SIZE (9)),
         supl_PDU_Add_SDL_BitString_U4(
            &p_Position->velocity->choice.horveluncert.bearing,
            (U4) p_GN_Position->bearing,
            9 );

         // ULP-Components ASN.1 - horspeed        BIT STRING(SIZE (16)),
         supl_PDU_Add_SDL_BitString_U4(
            &p_Position->velocity->choice.horveluncert.horspeed,
            (U4) p_GN_Position->horspeed,
            16 );

         // ULP-Components ASN.1 - horspeed        BIT STRING(SIZE (16)),
         supl_PDU_Add_SDL_BitString_U4(
            &p_Position->velocity->choice.horveluncert.uncertspeed,
            (U4) p_GN_Position->horuncertspeed,
            16 );

         break;
      case GN_horandveruncert:
         // Encode Velocity.
         //p_Position->velocity1Present  = SDL_True;
         p_Position->velocity->present = Velocity_PR_horandveruncert;

         // ULP-Components ASN.1 - verdirect       BIT STRING(SIZE (1)),
         supl_PDU_Add_SDL_BitString_U4(
             &p_Position->velocity->choice.horandveruncert.verdirect,
            (U4) p_GN_Position->verdirect,
            1 );

         // ULP-Components ASN.1 - bearing         BIT STRING(SIZE (9)),
         supl_PDU_Add_SDL_BitString_U4(
            &p_Position->velocity->choice.horandveruncert.bearing,
            (U4) p_GN_Position->bearing,
            9 );

         // ULP-Components ASN.1 - horspeed        BIT STRING(SIZE (16)),
         supl_PDU_Add_SDL_BitString_U4(
            &p_Position->velocity->choice.horandveruncert.horspeed,
            (U4) p_GN_Position->horspeed,
            16 );

         // ULP-Components ASN.1 - verspeed        BIT STRING(SIZE (8)),
         supl_PDU_Add_SDL_BitString_U4(
            &p_Position->velocity->choice.horandveruncert.verspeed,
            (U4) p_GN_Position->verspeed,
            8 );

         // ULP-Components ASN.1 - horuncertspeed  BIT STRING(SIZE (8)),
         supl_PDU_Add_SDL_BitString_U4(
            &p_Position->velocity->choice.horandveruncert.horuncertspeed,
            (U4) p_GN_Position->horuncertspeed,
            8 );

         // ULP-Components ASN.1 -veruncertspeed  BIT STRING(SIZE (8)),
         supl_PDU_Add_SDL_BitString_U4(
            &p_Position->velocity->choice.horandveruncert.veruncertspeed,
            (U4) p_GN_Position->veruncertspeed,
            8 );

         break;
      case GN_novelocity:
      default:
        // p_Position->velocity1Present = SDL_False;
         break;
      }
   /* + LMSqc37015 */
   }

   else
   {
      //p_Position->velocity1Present = SDL_False;  //need to check-raghu
   }
   /* - LMSqc37015 */

}


//*****************************************************************************
/// \brief
///      Function to add Position Estimate to the unencoded SUPL PDU type SUPL-POS-INIT.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLPOSINIT_Position
(
   void *p_ThisPDU,  ///< [in] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   s_GN_Position  *p_GN_Position ///<[in] Position Estimate.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   p_PDU_Cast->message.choice.msSUPLPOSINIT.position = GN_Calloc( 1 , sizeof(Position_t ) );

   supl_PDU_Add_Position( p_PDU_Cast->message.choice.msSUPLPOSINIT.position, p_GN_Position );
}



//*****************************************************************************
/// \brief
///      Function to add Position Estimate to the unencoded SUPL PDU type SUPL-END.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLEND_Position
(
   void *p_ThisPDU,  ///< [in] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   s_GN_Position  *p_GN_Position ///<[in] Position Estimate.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   p_PDU_Cast->message.choice.msSUPLEND.position = GN_Calloc( 1 , sizeof(Position_t ) );

   supl_PDU_Add_Position( p_PDU_Cast->message.choice.msSUPLEND.position , p_GN_Position );
}


//*****************************************************************************
/// \brief
///      Function to get a Position Estimate from the decoded SUPL PDU type SUPL-END.
///
/// \returns
///      Nothing.
//*****************************************************************************
BL supl_PDU_Get_SUPLEND_GAD_Position
(
   void *p_ThisPDU,  ///< [in] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   s_GN_Position      *p_GN_Position,
   s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data   ///<[out] Position Estimate.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;
   Position_t *p_Position;
   U4 Temp;

   if ( p_PDU_Cast->message.choice.msSUPLEND.position != NULL)
   {

      p_Position = p_PDU_Cast->message.choice.msSUPLEND.position;
      memset( p_GN_AGPS_GAD_Data, 0, sizeof( s_GN_AGPS_GAD_Data ) );
      memset( p_GN_Position,      0, sizeof( s_GN_Position      ) );

      p_GN_Position->OS_Time_ms = GN_GPS_Get_OS_Time_ms();

      // Set values to not known where sensible.
      p_GN_AGPS_GAD_Data->altitudeDirection     =  -1; //=Not present]

      p_GN_AGPS_GAD_Data->uncertaintySemiMajor  = 255; //=Unknown]
      p_GN_AGPS_GAD_Data->uncertaintySemiMinor  = 255; //=Unknown]
      p_GN_AGPS_GAD_Data->orientationMajorAxis  = 255; //=Unknown]
      p_GN_AGPS_GAD_Data->uncertaintyAltitude   = 255; //=Unknown]
      p_GN_AGPS_GAD_Data->confidence            = 255; //=Unknown]

      p_GN_AGPS_GAD_Data->verdirect             =  -1; //=Not present]
      p_GN_AGPS_GAD_Data->bearing               =  -1; //=Not present]
      p_GN_AGPS_GAD_Data->horspeed              =  -1; //=Not present]
      p_GN_AGPS_GAD_Data->verspeed              =  -1; //=Not present]
      p_GN_AGPS_GAD_Data->horuncertspeed        = 255; //=Unknown]
      p_GN_AGPS_GAD_Data->veruncertspeed        = 255; //=Unknown]

      p_GN_Position->Valid_2D_Fix = TRUE;

      strcpy( p_GN_Position->timestamp, (CH*)&p_Position->timestamp.buf[1] ); // Exclude prepended 'V' from SDL_String.

      p_GN_AGPS_GAD_Data->latitude     = p_Position->positionEstimate.latitude;
      p_GN_AGPS_GAD_Data->latitudeSign = *(p_Position->positionEstimate.latitudeSign.buf);
     // ASN1_ENUM_SET(p_GN_AGPS_GAD_Data->latitudeSign,p_Position->positionEstimate.latitudeSign);
      p_GN_AGPS_GAD_Data->longitude    = p_Position->positionEstimate.longitude;

      p_GN_Position->latitude          = p_Position->positionEstimate.latitude;
      //ASN1_ENUM_SET(p_GN_Position->latitudeSign,p_Position->positionEstimate.latitudeSign);
      p_GN_Position->latitudeSign      = *(p_Position->positionEstimate.latitudeSign.buf);
      p_GN_Position->longitude         = p_Position->positionEstimate.longitude;

      if (p_Position->positionEstimate.uncertainty != NULL )
      {
         p_GN_AGPS_GAD_Data->uncertaintySemiMajor = (U1)p_Position->positionEstimate.uncertainty->uncertaintySemiMajor;
         p_GN_AGPS_GAD_Data->uncertaintySemiMinor = (U1)p_Position->positionEstimate.uncertainty->uncertaintySemiMinor;
         p_GN_AGPS_GAD_Data->orientationMajorAxis = (U1)p_Position->positionEstimate.uncertainty->orientationMajorAxis;

         p_GN_Position->uncertaintyPresent = TRUE;
         p_GN_Position->uncertaintySemiMajor = (U1)p_Position->positionEstimate.uncertainty->uncertaintySemiMajor;;
         p_GN_Position->uncertaintySemiMinor = (U1)p_Position->positionEstimate.uncertainty->uncertaintySemiMinor;
         p_GN_Position->orientationMajorAxis = (U1)p_Position->positionEstimate.uncertainty->orientationMajorAxis;
      }

      if ( p_Position->positionEstimate.altitudeInfo != NULL )
      {
         p_GN_AGPS_GAD_Data->altitude              = (U2)p_Position->positionEstimate.altitudeInfo->altitude;
         p_GN_AGPS_GAD_Data->altitudeDirection     = *(p_Position->positionEstimate.altitudeInfo->altitudeDirection.buf);
         p_GN_AGPS_GAD_Data->uncertaintyAltitude   = (U1)p_Position->positionEstimate.altitudeInfo->altUncertainty;

         p_GN_Position->altitudeInfoPresent  = TRUE;

         p_GN_Position->Valid_3D_Fix         = TRUE;
         p_GN_Position->altitude             = (U2)p_Position->positionEstimate.altitudeInfo->altitude;
         p_GN_Position->altitudeDirection    = *(p_Position->positionEstimate.altitudeInfo->altitudeDirection.buf);
         p_GN_Position->altUncertainty       = (U1)p_Position->positionEstimate.altitudeInfo->altUncertainty;
      }

      if ( p_Position->positionEstimate.confidence != NULL )
      {
         p_GN_AGPS_GAD_Data->confidence   = (U1)*p_Position->positionEstimate.confidence;

         p_GN_Position->confidencePresent = TRUE;
         p_GN_Position->confidence        = (U1)*p_Position->positionEstimate.confidence;
      }
      if ( p_Position->velocity != NULL)
      {
         p_GN_Position->velocityPresent = TRUE;

         switch ( p_Position->velocity->present)
         {
         case Velocity_PR_horandveruncert:
            p_GN_Position->VelocityIndicator = GN_horandveruncert;

            if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horandveruncert.bearing, &Temp ) )
            {
               p_GN_AGPS_GAD_Data->bearing = Temp;
               p_GN_Position->bearing = Temp;
            }

            if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horandveruncert.horspeed, &Temp ) )
            {
               p_GN_AGPS_GAD_Data->horspeed = Temp;
               p_GN_Position->horspeed = Temp;
            }

            if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horandveruncert.horuncertspeed, &Temp ) )
            {
               p_GN_AGPS_GAD_Data->horuncertspeed = Temp;
               p_GN_Position->horuncertspeed = Temp;
            }

            if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horandveruncert.verdirect, &Temp ) )
            {
               p_GN_AGPS_GAD_Data->verdirect = Temp;
               p_GN_Position->verdirect = Temp;
            }

            if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horandveruncert.verspeed, &Temp ) )
            {
               p_GN_AGPS_GAD_Data->verspeed = Temp;
               p_GN_Position->verspeed = Temp;
            }

            if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horandveruncert.veruncertspeed, &Temp ) )
            {
               p_GN_AGPS_GAD_Data->veruncertspeed = Temp;
               p_GN_Position->veruncertspeed = Temp;
            }

            break;
         case Velocity_PR_horandvervel:
            p_GN_Position->VelocityIndicator = GN_horandvervel;

            if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horandvervel.horspeed, &Temp ) )
            {
               p_GN_AGPS_GAD_Data->horspeed = Temp;
               p_GN_Position->horspeed = Temp;
            }

            if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horandvervel.bearing, &Temp ) )
            {
               p_GN_AGPS_GAD_Data->bearing = Temp;
               p_GN_Position->bearing = Temp;
            }

            if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horandvervel.verdirect, &Temp ) )
            {
               p_GN_AGPS_GAD_Data->verdirect = Temp;
               p_GN_Position->verdirect = Temp;
            }

            if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horandvervel.verspeed, &Temp ) )
            {
               p_GN_AGPS_GAD_Data->verspeed = Temp;
               p_GN_Position->verspeed = Temp;
            }

            break;
         case Velocity_PR_horvel:
            p_GN_Position->VelocityIndicator = GN_horvel;

            if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horvel.horspeed, &Temp ) )
            {
               p_GN_AGPS_GAD_Data->horspeed = Temp;
               p_GN_Position->horspeed = Temp;
            }

            if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horvel.bearing, &Temp ) )
            {
               p_GN_AGPS_GAD_Data->bearing = Temp;
               p_GN_Position->bearing = Temp;
            }

            break;
         case Velocity_PR_horveluncert:
            p_GN_Position->VelocityIndicator = GN_horveluncert;

            if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horveluncert.horspeed, &Temp ) )
            {
               p_GN_AGPS_GAD_Data->horspeed = Temp;
               p_GN_Position->horspeed = Temp;
            }

            if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horveluncert.bearing, &Temp ) )
            {
               p_GN_AGPS_GAD_Data->bearing = Temp;
               p_GN_Position->bearing = Temp;
            }

            if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horveluncert.uncertspeed, &Temp ) )
            {
               p_GN_AGPS_GAD_Data->horuncertspeed = Temp;
               p_GN_Position->horuncertspeed = Temp;
            }

            break;
         default:
             break;
         }
      }

      return TRUE;
   }
   else
   {
      return FALSE;
   }
}


//*****************************************************************************
/// \brief
///      Function to set the unencoded SUPL PDU type to SUPL-END.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLEND
(
   void *p_ThisPDU   ///< [in/out] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   // SUPL-END defined in SUPL-END.asn1
   p_PDU_Cast->message.present = UlpMessage_PR_msSUPLEND;
}

//*****************************************************************************
/// \brief
///      Function to set the unencoded SUPL PDU type to SUPL-NOTIFYRESPONSE.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLNOTIFYRESPONSE
(
   void *p_ThisPDU   ///< [in/out] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   // SUPL-NOTIFYRESPONSE defined in SUPL-NOTIFYRESPONSE.asn1
   p_PDU_Cast->message.present = UlpMessage_PR_msSUPLNOTIFYRESPONSE;
}

//*****************************************************************************
/// \brief
///      High level function to populate an unencoded SUPL-END PDU.
///
/// \returns
///      Pointer to unencoded structure ULP_PDU_t. Returned as (void *) to hide
///      implementation.
//*****************************************************************************
void *supl_PDU_Make_SUPLEND
(
   s_SUPL_Instance *p_SUPL_Instance ///< [in] Current instance data used to
                                    ///   populate parts of the PDU.
)
{
   void *p_PDU_Src = 0;

   s_SUPL_InstanceData  *p_SUPL = p_SUPL_Instance->p_SUPL; // SUPL specific data.

   p_PDU_Src = supl_PDU_Alloc();
   GN_SUPL_Log( "SUPL-END Alloc: %08p", p_PDU_Src );

   supl_PDU_Add_Version( p_PDU_Src , p_SUPL_Instance );

   supl_PDU_Add_setSessionID( p_PDU_Src, p_SUPL->SetSessionId,   &p_SUPL->SetId );
   supl_PDU_Add_slpSessionID( p_PDU_Src, p_SUPL->p_SlpSessionId, &p_SUPL->SlpId );

   // Add the SUPL-END message.
   supl_PDU_Add_SUPLEND( p_PDU_Src );

   return p_PDU_Src;
}

//*****************************************************************************
/// \brief
///      High level function to populate an unencoded SUPL-NOTIFY-RESPONSE PDU.
///
/// \returns
///      Pointer to unencoded structure ULP_PDU_t. Returned as (void *) to hide
///      implementation.
//*****************************************************************************
void *supl_PDU_Make_SUPLNOTIFYRESPONSE
(
   s_SUPL_Instance *p_SUPL_Instance, ///< [in] Current instance data used to
                                    ///   populate parts of the PDU.
   e_GN_StatusCode GN_StatusCode
)
{
   void *p_PDU_Src = 0;
   ULP_PDU_t *p_PDU_Cast = NULL;

   s_SUPL_InstanceData  *p_SUPL = p_SUPL_Instance->p_SUPL; // SUPL specific data.
   p_PDU_Src = supl_PDU_Alloc();

   p_PDU_Cast = p_PDU_Src;

   GN_SUPL_Log( "SUPL-NOTIFY-RESPONSE Alloc: %08p", p_PDU_Src );

   supl_PDU_Add_Version( p_PDU_Src , p_SUPL_Instance );

   supl_PDU_Add_setSessionID( p_PDU_Src, p_SUPL->SetSessionId,   &p_SUPL->SetId );
   supl_PDU_Add_slpSessionID( p_PDU_Src, p_SUPL->p_SlpSessionId, &p_SUPL->SlpId );


   supl_PDU_Add_SUPLNOTIFYRESPONSE( p_PDU_Src );

   p_PDU_Cast->message.choice.msSUPLNOTIFYRESPONSE.notificationResponse = GN_Calloc(1,sizeof(NotificationResponse_t));

   switch( GN_StatusCode )
   {
   case  GN_StatusCode_consentDeniedByUser:
       ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLNOTIFYRESPONSE.notificationResponse,NotificationResponse_notAllowed);
       
    break;
   case  GN_StatusCode_consentGrantedByUser:
       ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLNOTIFYRESPONSE.notificationResponse,NotificationResponse_allowed);
       
    break;
   default:
      OS_ASSERT( 0 );
      break;
   }


   return p_PDU_Src;
}


//*****************************************************************************
/// \brief
///      Get RRLP Payload from the unencoded SUPL PDU ULP_PDU_t of type SUPL-POS.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Get_SUPLPOS_RRLP_Payload
(
   void *p_ThisPDU,  ///< [in] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   s_PDU_Encoded *p_POS_Payload  ///< [out] RRLP Payload for processing by the
                                 ///   RRLP subsystem.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;
   p_POS_Payload->Length =
       p_PDU_Cast->message.choice.msSUPLPOS.posPayLoad.choice.rrlpPayload.size;
   p_POS_Payload->p_PDU_Data = GN_Calloc( 1, p_POS_Payload->Length );
   memcpy( p_POS_Payload->p_PDU_Data,
       p_PDU_Cast->message.choice.msSUPLPOS.posPayLoad.choice.rrlpPayload.buf,
           p_POS_Payload->Length );
}


//*****************************************************************************
/// \brief
///      Identifies the from the unencoded SUPL PDU ULP_PDU_t of type SUPL-POS.
///
/// \returns
///      #TRUE if a supported POS message was delivered.
///      #FALSE if an unsupported POS messages was delivered.
//*****************************************************************************
BL supl_Process_SUPL_POS
(
   void              *p_ThisPDU,       ///< [in] Pointer to unencoded structure
                                       ///   ULP_PDU_t. Passed in as (void *) to hide
                                       ///   implementation.
   s_PDU_Encoded     *p_POS_Payload,   ///< [in] RRLP Payload for processing by the
                                       ///   POS subsystem.
   e_GN_StatusCode   *p_GN_StatusCode  ///< [out] GN format status code.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   if ( p_PDU_Cast->message.present != UlpMessage_PR_msSUPLPOS )
   {
      *p_GN_StatusCode = GN_StatusCode_systemFailure;
      return FALSE;
   }

   switch( p_PDU_Cast->message.choice.msSUPLPOS.posPayLoad.present)
   {
   case PosPayLoad_PR_tia801payload:
      *p_GN_StatusCode = GN_StatusCode_protocolError;
      return FALSE;
      break;
   case PosPayLoad_PR_rrcPayload:
      *p_GN_StatusCode = GN_StatusCode_protocolError;
      return FALSE;
      break;
   case PosPayLoad_PR_rrlpPayload:
      // Send payload to RRLP subsystem.
      supl_PDU_Get_SUPLPOS_RRLP_Payload( p_ThisPDU, p_POS_Payload );
      break;
   default:
      *p_GN_StatusCode = GN_StatusCode_systemFailure;
      return FALSE;
      break;
   }
   return TRUE;
}


//*****************************************************************************
/// \brief
///      Add RRLP Payload to the unencoded SUPL PDU ULP_PDU_t of type SUPL-POS.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLPOS_RRLP_Payload
(
   void *p_ThisPDU,  ///< [in] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   s_PDU_Encoded *p_POS_Payload  ///< [out] RRLP Payload for processing by the
                                 ///   RRLP subsystem.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   p_PDU_Cast->message.choice.msSUPLPOS.posPayLoad.present = PosPayLoad_PR_rrlpPayload;
   p_PDU_Cast->message.choice.msSUPLPOS.posPayLoad.choice.rrlpPayload.size =
      p_POS_Payload->Length;
   p_PDU_Cast->message.choice.msSUPLPOS.posPayLoad.choice.rrlpPayload.buf =
      (uint8_t*)GN_Calloc( 1, p_POS_Payload->Length );
   memcpy ( p_PDU_Cast->message.choice.msSUPLPOS.posPayLoad.choice.rrlpPayload.buf,
            p_POS_Payload->p_PDU_Data,
            p_POS_Payload->Length );
}


//*****************************************************************************
/// \brief
///      Add velocity portion of Position Solution to the unencoded
///      SUPL PDU ULP_PDU_t of type SUPL-POS.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLPOS_AGPS_GAD_Data
(
   void *p_ThisPDU,  ///< [in] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data ///< [in] Pointer to position solution (NULL is not present).
)
{
   Velocity_t *P_Velocity;
    ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   if ( p_GN_AGPS_GAD_Data != NULL )
   {
      p_PDU_Cast->message.choice.msSUPLPOS.velocity = P_Velocity = (Velocity_t*)GN_Calloc( 1, sizeof(Velocity_t) );

      // Next check for the Horandveruncert.
      if( p_GN_AGPS_GAD_Data->verdirect       !=  -1 &&
          p_GN_AGPS_GAD_Data->bearing         !=  -1 &&
          p_GN_AGPS_GAD_Data->horspeed        !=  -1 &&
          p_GN_AGPS_GAD_Data->verspeed        !=  -1 &&
          p_GN_AGPS_GAD_Data->horuncertspeed  != 255 &&
          p_GN_AGPS_GAD_Data->veruncertspeed  != 255 )
      {


          P_Velocity->present  = Velocity_PR_horandveruncert;

         supl_PDU_Add_SDL_BitString_U4(
             &P_Velocity->choice.horandveruncert.verdirect,
            (U4) p_GN_AGPS_GAD_Data->verdirect,
            1 );

         supl_PDU_Add_SDL_BitString_U4(
             &P_Velocity->choice.horandveruncert.bearing,
            (U4) p_GN_AGPS_GAD_Data->bearing,
            9 );

         supl_PDU_Add_SDL_BitString_U4(
             &P_Velocity->choice.horandveruncert.horspeed,
            (U4) p_GN_AGPS_GAD_Data->horspeed,
            16 );

         supl_PDU_Add_SDL_BitString_U4(
            &P_Velocity->choice.horandveruncert.verspeed,
            (U4) p_GN_AGPS_GAD_Data->verspeed,
            8 );

         supl_PDU_Add_SDL_BitString_U4(
            &P_Velocity->choice.horandveruncert.horuncertspeed,
            (U4) p_GN_AGPS_GAD_Data->horuncertspeed,
            8 );

         supl_PDU_Add_SDL_BitString_U4(
             &P_Velocity->choice.horandveruncert.veruncertspeed,
            (U4) p_GN_AGPS_GAD_Data->veruncertspeed,
            8 );
      }
      // Now check for Horandvervel.
      else if ( p_GN_AGPS_GAD_Data->verdirect       != -1 &&
                p_GN_AGPS_GAD_Data->bearing         != -1 &&
                p_GN_AGPS_GAD_Data->horspeed        != -1 &&
                p_GN_AGPS_GAD_Data->verspeed        != -1 )
      {
         //p_PDU_Cast->message.U.msSUPLPOS.velocity2Present   = SDL_True;
         P_Velocity->present  = Velocity_PR_horandvervel;

         supl_PDU_Add_SDL_BitString_U4(
             &P_Velocity->choice.horandvervel.verdirect,
            (U4) p_GN_AGPS_GAD_Data->verdirect,
            1 );

         supl_PDU_Add_SDL_BitString_U4(
            &P_Velocity->choice.horandvervel.bearing,
            (U4) p_GN_AGPS_GAD_Data->bearing,
            9 );

         supl_PDU_Add_SDL_BitString_U4(
            &P_Velocity->choice.horandvervel.horspeed,
            (U4) p_GN_AGPS_GAD_Data->horspeed,
            16);

         supl_PDU_Add_SDL_BitString_U4(
             &P_Velocity->choice.horandvervel.verspeed,
            (U4) p_GN_AGPS_GAD_Data->verspeed,
            8 );
      }
      // Now check for Horveluncert.
      else if ( p_GN_AGPS_GAD_Data->bearing         !=  -1 &&
                p_GN_AGPS_GAD_Data->horspeed        !=  -1 &&
                p_GN_AGPS_GAD_Data->horuncertspeed  != 255 )
      {
        // p_PDU_Cast->message.U.msSUPLPOS.velocity2Present   = SDL_True;
         P_Velocity->present  = Velocity_PR_horveluncert;

         supl_PDU_Add_SDL_BitString_U4(
             &P_Velocity->choice.horveluncert.bearing,
            (U4) p_GN_AGPS_GAD_Data->bearing,
            9 );

         supl_PDU_Add_SDL_BitString_U4(
            &P_Velocity->choice.horveluncert.horspeed,
            (U4) p_GN_AGPS_GAD_Data->horspeed,
            16 );

         supl_PDU_Add_SDL_BitString_U4(
           &P_Velocity->choice.horveluncert.uncertspeed,
            (U4) p_GN_AGPS_GAD_Data->horuncertspeed,
            8 );
      }
      // Now check for Horvel.
      else if ( p_GN_AGPS_GAD_Data->bearing         != -1 &&
                p_GN_AGPS_GAD_Data->horspeed        != -1 )
      {
        // p_PDU_Cast->message.U.msSUPLPOS.velocity2Present   = SDL_True;
         P_Velocity->present  = Velocity_PR_horvel;

         supl_PDU_Add_SDL_BitString_U4(
             &P_Velocity->choice.horvel.bearing,
            (U4) p_GN_AGPS_GAD_Data->bearing,
            9 );

         supl_PDU_Add_SDL_BitString_U4(
            &P_Velocity->choice.horvel.horspeed,
            (U4) p_GN_AGPS_GAD_Data->horspeed,
            16 );
      }
   }
}


//*****************************************************************************
/// \brief
///      Function to set the unencoded SUPL PDU type to SUPL-POS.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLPOS
(
   void *p_ThisPDU   ///< [in/out] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   // SUPL-POS defined in SUPL-POS.asn1
   p_PDU_Cast->message.present = UlpMessage_PR_msSUPLPOS;
}


//*****************************************************************************
/// \brief
///      High level function to populate an unencoded SUPL-POS PDU.
///
/// \returns
///      Pointer to unencoded structure ULP_PDU_t. Returned as (void *) to hide
///      implementation.
//*****************************************************************************
void *supl_PDU_Make_SUPLPOS
(
   s_SUPL_Instance      *p_SUPL_Instance,    ///< [in] Current instance data used to
                                             ///   populate parts of the PDU.
   /// \todo Replace #s_PDU_Buffer_Store with #s_PDU_Encoded.
   s_PDU_Buffer_Store   *p_PDU_RRLP,         ///< [in] RRLP Payload for processing by the
                                             ///   RRLP subsystem.
   s_GN_AGPS_GAD_Data   *p_GN_AGPS_GAD_Data  ///< [in] Pointer to position solution (NULL is not present).
)
{
   void *p_PDU_Src = 0;
   s_SUPL_InstanceData *p_SUPL = p_SUPL_Instance->p_SUPL; // SUPL specific data.

   p_PDU_Src = supl_PDU_Alloc();
   GN_SUPL_Log( "SUPL-POS Alloc: %08p", p_PDU_Src );

   supl_PDU_Add_Version( p_PDU_Src , p_SUPL_Instance);

   supl_PDU_Add_setSessionID( p_PDU_Src, p_SUPL->SetSessionId,   &p_SUPL->SetId );
   supl_PDU_Add_slpSessionID( p_PDU_Src, p_SUPL->p_SlpSessionId, &p_SUPL->SlpId );

   // Add the SUPL-POS message.
   supl_PDU_Add_SUPLPOS( p_PDU_Src );
   supl_PDU_Add_SUPLPOS_RRLP_Payload( p_PDU_Src, &p_PDU_RRLP->PDU_Encoded );

   if ( p_GN_AGPS_GAD_Data != NULL )
   {
      // Add velocity information.
      // supl_PDU_Add_SUPLPOS_AGPS_GAD_Data( p_PDU_Src, p_GN_AGPS_GAD_Data );
   }
   return p_PDU_Src;
}


//*****************************************************************************
/// \brief
///      Function to get the set capabilities to be used in SUPL PDU.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_get_SetCapabilities
(
   s_SUPL_Instance *p_SUPL_Instance,         ///< [in]  Positioning Technologies supported  (can be accessed from instance)
   s_GN_SetCapabilities *p_SetCapabilities   ///< [out] Set Capabilities.
)
{
   
   memset( p_SetCapabilities, 0, sizeof( s_GN_SetCapabilities ) );

   p_SetCapabilities->PosProtocol.rrlp                = 1;
   p_SetCapabilities->PosTechnology.agpsSETBased      = supl_config_get_SUPL_agpsSETBased();
   p_SetCapabilities->PosTechnology.agpsSETassisted   = supl_config_get_SUPL_agpsSETassisted();
   p_SetCapabilities->PosTechnology.autonomousGPS     = supl_config_get_SUPL_autonomousGPS();
   p_SetCapabilities->PosTechnology.eCID              = supl_config_get_SUPL_eCID();

   if ( p_SetCapabilities->PosTechnology.agpsSETBased == TRUE )
   {
      p_SetCapabilities->PrefMethod                   = GN_agpsSETBasedPreferred;
   }
   else if ( p_SetCapabilities->PosTechnology.agpsSETassisted == TRUE )
   {
      p_SetCapabilities->PrefMethod                   = GN_agpsSETassistedPreferred;
   }
   else
   {
      p_SetCapabilities->PrefMethod                   = GN_noPreference;
   }

   if( supl_Is_Version2( p_SUPL_Instance ) ) 
   {
      /* Populate event trigger capability */
      p_SetCapabilities->V2_SetCapabilities.EventTriggerCapabilities.GeographicAreasSupported.Ellipse = supl_config_get_Geographic_Area_Ellipse();
      p_SetCapabilities->V2_SetCapabilities.EventTriggerCapabilities.GeographicAreasSupported.Polygon = supl_config_get_Geographic_Area_Polygon();
      p_SetCapabilities->V2_SetCapabilities.EventTriggerCapabilities.MaxAreaIdList = supl_config_get_Max_Area_ID_Lists();
      p_SetCapabilities->V2_SetCapabilities.EventTriggerCapabilities.MaxAreaIdPerAreaIdList = supl_config_get_Max_Area_ID_Per_List();
      p_SetCapabilities->V2_SetCapabilities.EventTriggerCapabilities.MaxNumGeoArea = supl_config_get_Max_Geographic_Areas();

      /* Batch Reporting capabilities */
      p_SetCapabilities->V2_SetCapabilities.ReportingCapabilities.BatchRepCap.MaxNumMeasuremetns
         = supl_config_get_Max_Measurement_In_Batch();
      p_SetCapabilities->V2_SetCapabilities.ReportingCapabilities.BatchRepCap.MaxNumPosition
         = supl_config_get_Max_Positions_In_Batch();
      p_SetCapabilities->V2_SetCapabilities.ReportingCapabilities.BatchRepCap.ReportMeasurements
         = supl_config_get_Batch_Report_Measurements();
      p_SetCapabilities->V2_SetCapabilities.ReportingCapabilities.BatchRepCap.ReportPosition
         = supl_config_get_Batch_Report_Position();

      p_SetCapabilities->V2_SetCapabilities.ReportingCapabilities.MaximumInterval
         = supl_config_get_Max_Report_Interval();
      p_SetCapabilities->V2_SetCapabilities.ReportingCapabilities.MinimumInterval
         = supl_config_get_Min_Report_Interval();

      p_SetCapabilities->V2_SetCapabilities.ReportingCapabilities.ReportingMode.Batch
         = supl_config_get_Reporting_Mode_Batch();
      p_SetCapabilities->V2_SetCapabilities.ReportingCapabilities.ReportingMode.QuasiRealTime
         = supl_config_get_Reporting_Mode_Quasi_Real_Time();
      p_SetCapabilities->V2_SetCapabilities.ReportingCapabilities.ReportingMode.RealTime
         = supl_config_get_Reporting_Mode_Real_Time();

      p_SetCapabilities->V2_SetCapabilities.ServicesSupported.EventTrigger
         = supl_config_get_AreaEvent_Trigger();
      p_SetCapabilities->V2_SetCapabilities.ServicesSupported.PeriodicTrigger
         = supl_config_get_Periodic_Trigger();

      p_SetCapabilities->V2_SetCapabilities.SessionCapabilities.MaxPeriodicSessions
         = supl_config_get_Max_Num_Session();
      p_SetCapabilities->V2_SetCapabilities.SessionCapabilities.MaxTriggeredSession
         = supl_config_get_Max_Num_Session();
      p_SetCapabilities->V2_SetCapabilities.SessionCapabilities.MaxTotalSessions
         = supl_config_get_Max_Num_Session();
   }

   return;
}


//*****************************************************************************
/// \brief
///      Function to convert from GPS oriented position solutions to a SUPL oriented one.
///
///      Both the GAD data and NAV data are required because the timestamp
///      required by the SUPL message is not in the AGPS GAD structure.
///
/// \returns
///      Nothing.
//*****************************************************************************
void GN_SUPL_Position_From_Solution
(
   s_GN_Position      *p_GN_Position,        ///< [out] Position Estimate in GN format.
   s_GN_GPS_Nav_Data  *p_GN_GPS_Nav_Data,    ///< [in] GN GPS Navigation solution data.
   s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data    ///< [in] GN A-GPS GAD (Geographical Area Description)
)
{

   p_GN_Position->OS_Time_ms           = p_GN_GPS_Nav_Data->OS_Time_ms;
   p_GN_Position->Valid_2D_Fix         = p_GN_GPS_Nav_Data->Valid_SingleFix;    //Required for CG2900 header files
   p_GN_Position->Valid_3D_Fix         = p_GN_GPS_Nav_Data->Valid_SingleFix;    //Required for CG2900 header files

   sprintf( p_GN_Position->timestamp,
            "%04d%02d%02d%02d%02d+%02d%02d",
            p_GN_GPS_Nav_Data->Year ,
            p_GN_GPS_Nav_Data->Month,
            p_GN_GPS_Nav_Data->Day,
            p_GN_GPS_Nav_Data->Hours,
            p_GN_GPS_Nav_Data->Minutes,
            p_GN_GPS_Nav_Data->Seconds,
            p_GN_GPS_Nav_Data->Milliseconds);
  
   // Encode Latitude.
   p_GN_Position->latitude             = p_GN_AGPS_GAD_Data->latitude;
   p_GN_Position->latitudeSign         = p_GN_AGPS_GAD_Data->latitudeSign;

   // Encode Longitude.
   p_GN_Position->longitude            = p_GN_AGPS_GAD_Data->longitude;

   // Encode Uncertainty.
   p_GN_Position->uncertaintyPresent   = TRUE;
   p_GN_Position->uncertaintySemiMajor = p_GN_AGPS_GAD_Data->uncertaintySemiMajor;
   p_GN_Position->uncertaintySemiMinor = p_GN_AGPS_GAD_Data->uncertaintySemiMinor;

   // Encode Confidence.
   p_GN_Position->confidencePresent    = TRUE;
   p_GN_Position->confidence           = p_GN_AGPS_GAD_Data->confidence;

   // Encode Altitude.
   p_GN_Position->altitudeInfoPresent  = TRUE;
   p_GN_Position->altitude             = p_GN_AGPS_GAD_Data->altitude;
   p_GN_Position->altitudeDirection    = p_GN_AGPS_GAD_Data->altitudeDirection;

   // Encode Velocity.
   p_GN_Position->velocityPresent      = TRUE;
   p_GN_Position->VelocityIndicator    = GN_horandvervel;

   // ULP-Components ASN.1 - verdirect       BIT STRING(SIZE (1)),
   p_GN_Position->verdirect            = p_GN_AGPS_GAD_Data->verdirect;

   // ULP-Components ASN.1 - bearing         BIT STRING(SIZE (9)),
   p_GN_Position->bearing              = p_GN_AGPS_GAD_Data->bearing;

   // ULP-Components ASN.1 - horspeed        BIT STRING(SIZE (16)),
   p_GN_Position->horspeed             = p_GN_AGPS_GAD_Data->horspeed;

   // ULP-Components ASN.1 - verspeed        BIT STRING(SIZE (8)),
   p_GN_Position->verspeed             = p_GN_AGPS_GAD_Data->verspeed;

   return;
}


//*****************************************************************************
/// \brief
///      Function to convert from GPS oriented position solutions to a SUPL oriented one.
///
/// \returns
///      Nothing.
//*****************************************************************************
void GN_SUPL_Pos_Data_From_Nav_Data
(
   s_GN_SUPL_Pos_Data *p_GN_SUPL_Pos_Data,   ///< [out] Position Estimate in s_GN_SUPL_Pos_Data format.
   s_GN_GPS_Nav_Data  *p_GN_GPS_Nav_Data     ///< [in] GN GPS Navigation solution data.
)
{
   // Time
   // U2 Year;                   UTC Year A.D.                     [eg 2007].
   p_GN_SUPL_Pos_Data->Year         = p_GN_GPS_Nav_Data->Year;
   // U2 Month;                  UTC Month into the year           [range 1..12].
   p_GN_SUPL_Pos_Data->Month        = p_GN_GPS_Nav_Data->Month;
   // U2 Day;                    UTC Days into the month           [range 1..31].
   p_GN_SUPL_Pos_Data->Day          = p_GN_GPS_Nav_Data->Day;
   // U2 Hours;                  UTC Hours into the day            [range 0..23].
   p_GN_SUPL_Pos_Data->Hours        = p_GN_GPS_Nav_Data->Hours;
   // U2 Minutes;                UTC Minutes into the hour         [range 0..59].
   p_GN_SUPL_Pos_Data->Minutes      = p_GN_GPS_Nav_Data->Minutes;
   // U2 Seconds;                UTC Seconds into the hour         [range 0..59].
   p_GN_SUPL_Pos_Data->Seconds      = p_GN_GPS_Nav_Data->Seconds;
   // U2 Milliseconds;           UTC Milliseconds into the second  [range 0..999].
   p_GN_SUPL_Pos_Data->Milliseconds = p_GN_GPS_Nav_Data->Milliseconds;

   // // Position
   // R8 Latitude;               WGS84 Latitude  [degrees, positive North].
   p_GN_SUPL_Pos_Data->Latitude  = p_GN_GPS_Nav_Data->Latitude;
   // R8 Longitude;              WGS84 Longitude [degrees, positive East].
   p_GN_SUPL_Pos_Data->Longitude = p_GN_GPS_Nav_Data->Longitude;

   // BL AltitudeInfo_Valid;     Altitude fields valid: Altitude and V_AccEst.
   p_GN_SUPL_Pos_Data->AltitudeInfo_Valid    = TRUE;
   // R4 Altitude;               Altitude above the WGS84 Ellipsoid [m].
   p_GN_SUPL_Pos_Data->Altitude  = p_GN_GPS_Nav_Data->Altitude_Ell;
   // R4 V_AccEst;               Vertical Accuracy estimate [m].
   p_GN_SUPL_Pos_Data->V_AccEst     = p_GN_GPS_Nav_Data->V_AccEst;

   // // Velocity

   // BL HorizontalVel_Valid;    Horizontal Velocity fields valid: HorizontalVel and Bearing.
   p_GN_SUPL_Pos_Data->HorizontalVel_Valid   = TRUE;
   // R4 HorizontalVel;          2-dimensional Horizontal Velocity [m/s].
   p_GN_SUPL_Pos_Data->HorizontalVel   = p_GN_GPS_Nav_Data->SpeedOverGround;
   // R4 Bearing;                2-dimensional Bearing [degrees].
   p_GN_SUPL_Pos_Data->Bearing         = p_GN_GPS_Nav_Data->CourseOverGround;

   // BL VerticalVel_Valid;      Vertical Velocity field valid: VerticalVel.
   p_GN_SUPL_Pos_Data->VerticalVel_Valid     = TRUE;
   // R4 VerticalVel;            Vertical Velocity [m/s, positive Up].
   p_GN_SUPL_Pos_Data->VerticalVel     = p_GN_GPS_Nav_Data->VerticalVelocity;

   // // Accuracy Estimates

   // BL H_Acc_Valid;            Horizontal Uncertainty fields valid: H_AccMaj, H_AccMin and H_AccMajBrg.
   p_GN_SUPL_Pos_Data->H_Acc_Valid           = TRUE;
   // R4 H_AccMaj;               Horizontal Error ellipse semi-major axis [m].
   p_GN_SUPL_Pos_Data->H_AccMaj     = p_GN_GPS_Nav_Data->H_AccMaj;
   // R4 H_AccMin;               Horizontal Error ellipse semi-minor axis [m].
   p_GN_SUPL_Pos_Data->H_AccMin     = p_GN_GPS_Nav_Data->H_AccMin;
   // R4 H_AccMajBrg;            Bearing of the Horizontal Error ellipse semi-major axis [degrees].
   p_GN_SUPL_Pos_Data->H_AccMajBrg  = p_GN_GPS_Nav_Data->H_AccMajBrg;

   // BL HVel_AccEst_Valid;      Horizontal Velocity uncertainty field valid: HVel_AccEst.
   p_GN_SUPL_Pos_Data->HVel_AccEst_Valid     = TRUE;
   // R4 HVel_AccEst;            2-dimensional Horizontal Velocity accuracy estimate [m/s].
   p_GN_SUPL_Pos_Data->HVel_AccEst  = p_GN_GPS_Nav_Data->HVel_AccEst;

   // BL VVel_AccEst_Valid;      Vertical Velocity uncertainty field valid: VVel_AccEst.
   p_GN_SUPL_Pos_Data->VVel_AccEst           = TRUE;
   // R4 VVel_AccEst;            2-dimensional Vertical Velocity accuracy estimate [m/s].
   p_GN_SUPL_Pos_Data->VVel_AccEst  = p_GN_GPS_Nav_Data->VVel_AccEst;

   // BL Confidence_Valid;       Confidence field valid: Confidence.
   p_GN_SUPL_Pos_Data->Confidence_Valid      = TRUE;
   // U1 Confidence;             Confidence of the solution in a percentage where RMS 1-sigma equals (67%) range [1..100].
   p_GN_SUPL_Pos_Data->Confidence  = 67 ;

   return;
}


//*****************************************************************************
/// \brief
///      Function to convert from GPS oriented position solutions to a SUPL oriented one.
///
/// \returns
///      Nothing.
//*****************************************************************************
void GN_SUPL_Pos_Data_From_GAD_Data
(
   s_GN_SUPL_Pos_Data *p_GN_SUPL_Pos_Data,   ///< [out] Position Estimate in s_GN_SUPL_Pos_Data format.
   s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data    ///< [in] GN GPS GAD position data.
)
{
   R4 Temp;

   // Time
   // // U2 Year;                   UTC Year A.D.                     [eg 2007].
   // p_GN_SUPL_Pos_Data->Year         = p_GN_GPS_Nav_Data->Year;
   // // U2 Month;                  UTC Month into the year           [range 1..12].
   // p_GN_SUPL_Pos_Data->Month        = p_GN_GPS_Nav_Data->Month;
   // // U2 Day;                    UTC Days into the month           [range 1..31].
   // p_GN_SUPL_Pos_Data->Day          = p_GN_GPS_Nav_Data->Day;
   // // U2 Hours;                  UTC Hours into the day            [range 0..23].
   // p_GN_SUPL_Pos_Data->Hours        = p_GN_GPS_Nav_Data->Hours;
   // // U2 Minutes;                UTC Minutes into the hour         [range 0..59].
   // p_GN_SUPL_Pos_Data->Minutes      = p_GN_GPS_Nav_Data->Minutes;
   // // U2 Seconds;                UTC Seconds into the hour         [range 0..59].
   // p_GN_SUPL_Pos_Data->Seconds      = p_GN_GPS_Nav_Data->Seconds;
   // // U2 Milliseconds;           UTC Milliseconds into the second  [range 0..999].
   // p_GN_SUPL_Pos_Data->Milliseconds = p_GN_GPS_Nav_Data->Milliseconds;

   // Position
   // R8 Latitude;               WGS84 Latitude  [degrees, positive North].
   Temp = (R4) ( p_GN_AGPS_GAD_Data->latitude & GAD_LATITUDE_RANGE_MASK ) /
                        GAD_LATITUDE_SCALING_FACTOR;
   Temp *= (R4) GAD_LATITUDE_DEGREES_IN_RANGE *
                        ( p_GN_AGPS_GAD_Data->latitudeSign == 0 ? 1 : -1 );
   p_GN_SUPL_Pos_Data->Latitude  = Temp;

   // R8 Longitude;              WGS84 Longitude [degrees, positive East].
   Temp = (R4) ( p_GN_AGPS_GAD_Data->longitude / (R4) GAD_LONGITUDE_SCALING_FACTOR );
   Temp *= (R4) GAD_LONGITUDE_DEGREES_IN_RANGE;
   p_GN_SUPL_Pos_Data->Longitude = Temp;

   // R4 Altitude;               Altitude above the WGS84 Ellipsoid [m].
   Temp = (R4) ( p_GN_AGPS_GAD_Data->altitude & GAD_ALTITUDE_RANGE_MASK ) *
                           ( p_GN_AGPS_GAD_Data->altitudeDirection == 0 ? 1 : -1 ) ;
   p_GN_SUPL_Pos_Data->Altitude  = Temp;

   // Velocity
   if ( p_GN_AGPS_GAD_Data->horspeed < 255 && p_GN_AGPS_GAD_Data->bearing > -1 )
   {
      // BL HorizontalVel_Valid;    Horizontal Velocity fields valid: HorizontalVel and Bearing.
      p_GN_SUPL_Pos_Data->HorizontalVel_Valid   = TRUE;
      // R4 HorizontalVel;          2-dimensional Horizontal Velocity [m/s].
      p_GN_SUPL_Pos_Data->HorizontalVel   = p_GN_AGPS_GAD_Data->horspeed;
      // R4 Bearing;                2-dimensional Bearing [degrees].
      p_GN_SUPL_Pos_Data->Bearing         = p_GN_AGPS_GAD_Data->bearing;
   }
   else
   {
      // BL HorizontalVel_Valid;    Horizontal Velocity fields valid: HorizontalVel and Bearing.
      p_GN_SUPL_Pos_Data->HorizontalVel_Valid   = FALSE;
   }

   if ( p_GN_AGPS_GAD_Data->verspeed > -1 && p_GN_AGPS_GAD_Data->verdirect > -1 )
   {
      // BL VerticalVel_Valid;      Vertical Velocity field valid: VerticalVel.
      p_GN_SUPL_Pos_Data->VerticalVel_Valid     = TRUE;
      // R4 VerticalVel;            Vertical Velocity [m/s, positive Up].
      p_GN_SUPL_Pos_Data->VerticalVel     = p_GN_AGPS_GAD_Data->verspeed * ( p_GN_AGPS_GAD_Data->verdirect == 0 ) ? 1.0f : -1.0f ;
   }
   else
   {
      // BL VerticalVel_Valid;      Vertical Velocity field valid: VerticalVel.
      p_GN_SUPL_Pos_Data->VerticalVel_Valid     = FALSE;
   }
   // Accuracy Estimates
   if ( p_GN_AGPS_GAD_Data->uncertaintySemiMajor <= GAD_UNCERTAINTY_MASK &&
        p_GN_AGPS_GAD_Data->uncertaintySemiMinor <= GAD_UNCERTAINTY_MASK &&
        p_GN_AGPS_GAD_Data->bearing <= 0 &&
        p_GN_AGPS_GAD_Data->bearing >= 359 )
   {
      // BL H_Acc_Valid;            Horizontal Uncertainty fields valid: H_AccMaj, H_AccMin and H_AccMajBrg.
      p_GN_SUPL_Pos_Data->H_Acc_Valid           = TRUE;
      // R4 H_AccMaj;               Horizontal Error ellipse semi-major axis [m].
      p_GN_SUPL_Pos_Data->H_AccMaj     = (R4) GAD_BINOMIAL_UNCERTAINTY_FROM_K(
                                                GAD_UNCERTAINTY_C,
                                                GAD_UNCERTAINTY_x,
                                                ( p_GN_AGPS_GAD_Data->uncertaintySemiMajor &
                                                  GAD_UNCERTAINTY_MASK ) );
      // R4 H_AccMin;               Horizontal Error ellipse semi-minor axis [m].
      p_GN_SUPL_Pos_Data->H_AccMin     = (R4) GAD_BINOMIAL_UNCERTAINTY_FROM_K(
                                                GAD_UNCERTAINTY_C,
                                                GAD_UNCERTAINTY_x,
                                                ( p_GN_AGPS_GAD_Data->uncertaintySemiMinor &
                                                  GAD_UNCERTAINTY_MASK ) );
      // R4 H_AccMajBrg;            Bearing of the Horizontal Error ellipse semi-major axis [degrees].
      p_GN_SUPL_Pos_Data->H_AccMajBrg  = (R4) p_GN_AGPS_GAD_Data->orientationMajorAxis * 2;
   }
   else
   {
      // BL H_Acc_Valid;            Horizontal Uncertainty fields valid: H_AccMaj, H_AccMin and H_AccMajBrg.
      p_GN_SUPL_Pos_Data->H_Acc_Valid           = FALSE;
   }

   if ( p_GN_AGPS_GAD_Data->uncertaintyAltitude <= GAD_UNCERTAINTY_ALTITUDE_MASK )
   {
      // BL AltitudeInfo_Valid;     Altitude fields valid: Altitude and V_AccEst.
      p_GN_SUPL_Pos_Data->AltitudeInfo_Valid    = TRUE;
      // R4 V_AccEst;               Vertical Accuracy estimate [m].
      p_GN_SUPL_Pos_Data->V_AccEst     = (R4) GAD_BINOMIAL_UNCERTAINTY_FROM_K(
                                                GAD_UNCERTAINTY_ALTITUDE_C,
                                                GAD_UNCERTAINTY_ALTITUDE_x,
                                                ( p_GN_AGPS_GAD_Data->uncertaintyAltitude &
                                                  GAD_UNCERTAINTY_ALTITUDE_MASK ) );
   }
   else
   {
      // BL AltitudeInfo_Valid;     Altitude fields valid: Altitude and V_AccEst.
      p_GN_SUPL_Pos_Data->AltitudeInfo_Valid    = FALSE;
   }

   // BL HVel_AccEst_Valid;      Horizontal Velocity uncertainty field valid: HVel_AccEst.
   p_GN_SUPL_Pos_Data->HVel_AccEst_Valid        = FALSE;
   // R4 HVel_AccEst;            2-dimensional Horizontal Velocity accuracy estimate [m/s].
   ///\todo Add HVel_AccEst when present in nav data structure.

   // BL VVel_AccEst_Valid;      Vertical Velocity uncertainty field valid: VVel_AccEst.
   p_GN_SUPL_Pos_Data->VVel_AccEst              = FALSE;
   // R4 VVel_AccEst;            2-dimensional Vertical Velocity accuracy estimate [m/s].
   ///\todo Add VVel_AccEst when present in nav data structure.

   // BL Confidence_Valid;       Confidence field valid: Confidence.
   p_GN_SUPL_Pos_Data->Confidence_Valid         = TRUE;
   // U1 Confidence;             Confidence of the solution in a percentage where RMS 1-sigma equals (67%) range [1..100].
   p_GN_SUPL_Pos_Data->Confidence  = 67 ;

   return;
}


//*****************************************************************************
/// \brief
///      Function to convert from GPS oriented position solutions to a SUPL oriented one.
///
/// \returns
///      Nothing.
//*****************************************************************************
void GN_SUPL_Pos_Data_From_Position
(
   s_GN_SUPL_Pos_Data *p_GN_SUPL_Pos_Data,   ///< [out] Position Estimate in s_GN_SUPL_Pos_Data format.
   s_GN_Position *p_GN_Position              ///< [in] GN GPS position data.
)
{
   R4 Temp;
   U4 u4_Year;                   // UTC Year A.D.                     [eg 2007].
   U4 u4_Month;                  // UTC Month into the year           [range 1..12].
   U4 u4_Day;                    // UTC Days into the month           [range 1..31].
   U4 u4_Hours;                  // UTC Hours into the day            [range 0..23].
   U4 u4_Minutes;                // UTC Minutes into the hour         [range 0..59].
   U4 u4_Seconds;                // UTC Seconds into the hour         [range 0..59].
   U4 u4_Milliseconds;           // UTC Milliseconds into the second  [range 0..999].

   // Time

   sscanf( p_GN_Position->timestamp, "%2u%2u%2u%2u%2u%2u.%3u",
                                     &u4_Year,  &u4_Month,   &u4_Day,
                                     &u4_Hours, &u4_Minutes, &u4_Seconds,
                                     &u4_Milliseconds );
   // U2 Year;                      // UTC Year A.D.                     [eg 2007].
   p_GN_SUPL_Pos_Data->Year         = (U2)u4_Year+ 2000;   
   // U2 Month;                     // UTC Month into the year           [range 1..12].
   p_GN_SUPL_Pos_Data->Month        = (U2)u4_Month;
   // U2 Day;                       // UTC Days into the month           [range 1..31].
   p_GN_SUPL_Pos_Data->Day          = (U2)u4_Day;
   // U2 Hours;                     // UTC Hours into the day            [range 0..23].
   p_GN_SUPL_Pos_Data->Hours        = (U2)u4_Hours;
   // U2 Minutes;                   // UTC Minutes into the hour         [range 0..59].
   p_GN_SUPL_Pos_Data->Minutes      = (U2)u4_Minutes;
   // U2 Seconds;                   // UTC Seconds into the hour         [range 0..59].
   p_GN_SUPL_Pos_Data->Seconds      = (U2)u4_Seconds;
   // U2 Milliseconds;              // UTC Milliseconds into the second  [range 0..999].
   p_GN_SUPL_Pos_Data->Milliseconds = (U2)u4_Milliseconds;

  
   // Position
   // R8 Latitude;               WGS84 Latitude  [degrees, positive North].
   Temp = (R4) ( p_GN_Position->latitude & GAD_LATITUDE_RANGE_MASK ) /
                        GAD_LATITUDE_SCALING_FACTOR;
   Temp *= (R4) GAD_LATITUDE_DEGREES_IN_RANGE *
                        ( p_GN_Position->latitudeSign == 0 ? 1 : -1 );
   p_GN_SUPL_Pos_Data->Latitude  = Temp;

   // R8 Longitude;              WGS84 Longitude [degrees, positive East].
   Temp = (R4) ( p_GN_Position->longitude / (R4) GAD_LONGITUDE_SCALING_FACTOR );
   Temp *= (R4) GAD_LONGITUDE_DEGREES_IN_RANGE;
   p_GN_SUPL_Pos_Data->Longitude = Temp;

   // R4 Altitude;               Altitude above the WGS84 Ellipsoid [m].
   Temp = (R4) ( p_GN_Position->altitude & GAD_ALTITUDE_RANGE_MASK ) *
                           ( p_GN_Position->altitudeDirection == 0 ? 1 : -1 ) ;
   p_GN_SUPL_Pos_Data->Altitude  = Temp;

   // Accuracy Estimates
   if ( p_GN_Position->uncertaintySemiMajor <= GAD_UNCERTAINTY_MASK &&
        p_GN_Position->uncertaintySemiMinor <= GAD_UNCERTAINTY_MASK &&
        p_GN_Position->bearing < 360 )
   {
      // BL H_Acc_Valid;            Horizontal Uncertainty fields valid: H_AccMaj, H_AccMin and H_AccMajBrg.
      p_GN_SUPL_Pos_Data->H_Acc_Valid           = TRUE;
      // R4 H_AccMaj;               Horizontal Error ellipse semi-major axis [m].
      p_GN_SUPL_Pos_Data->H_AccMaj     = (R4) GAD_BINOMIAL_UNCERTAINTY_FROM_K(
                                                GAD_UNCERTAINTY_C,
                                                GAD_UNCERTAINTY_x,
                                                ( p_GN_Position->uncertaintySemiMajor &
                                                  GAD_UNCERTAINTY_MASK ) );
      // R4 H_AccMin;               Horizontal Error ellipse semi-minor axis [m].
      p_GN_SUPL_Pos_Data->H_AccMin     = (R4) GAD_BINOMIAL_UNCERTAINTY_FROM_K(
                                                GAD_UNCERTAINTY_C,
                                                GAD_UNCERTAINTY_x,
                                                ( p_GN_Position->uncertaintySemiMinor &
                                                  GAD_UNCERTAINTY_MASK ) );
      // R4 H_AccMajBrg;            Bearing of the Horizontal Error ellipse semi-major axis [degrees].
      p_GN_SUPL_Pos_Data->H_AccMajBrg  = (R4) p_GN_Position->orientationMajorAxis * 2;
   }
   else
   {
      // BL H_Acc_Valid;            Horizontal Uncertainty fields valid: H_AccMaj, H_AccMin and H_AccMajBrg.
      p_GN_SUPL_Pos_Data->H_Acc_Valid           = FALSE;
   }

   if ( p_GN_Position->altUncertainty <= GAD_UNCERTAINTY_ALTITUDE_MASK )
   {
      // BL AltitudeInfo_Valid;     Altitude fields valid: Altitude and V_AccEst.
      p_GN_SUPL_Pos_Data->AltitudeInfo_Valid    = TRUE;
      // R4 V_AccEst;               Vertical Accuracy estimate [m].
      p_GN_SUPL_Pos_Data->V_AccEst     = (R4) GAD_BINOMIAL_UNCERTAINTY_FROM_K(
                                                GAD_UNCERTAINTY_ALTITUDE_C,
                                                GAD_UNCERTAINTY_ALTITUDE_x,
                                                ( p_GN_Position->altUncertainty &
                                                  GAD_UNCERTAINTY_ALTITUDE_MASK ) );
   }
   else
   {
      // BL AltitudeInfo_Valid;     Altitude fields valid: Altitude and V_AccEst.
      p_GN_SUPL_Pos_Data->AltitudeInfo_Valid    = FALSE;
   }

   if( p_GN_Position->velocityPresent )
   {
       // TODO : Check if the convertion from GAD to our internal values for velocity is proper
       switch( p_GN_Position->VelocityIndicator )
       {
          case GN_horvel:
          {
             // BL HorizontalVel_Valid;    Horizontal Velocity fields valid: HorizontalVel and Bearing.
             p_GN_SUPL_Pos_Data->HorizontalVel_Valid   = TRUE;
             // R4 HorizontalVel;          2-dimensional Horizontal Velocity [m/s].
             p_GN_SUPL_Pos_Data->HorizontalVel   = (R4) p_GN_Position->horspeed;
             // R4 Bearing;                2-dimensional Bearing [degrees].
             p_GN_SUPL_Pos_Data->Bearing         = (R4) p_GN_Position->bearing;

             // No Vertical Velocity and uncertainties
             p_GN_SUPL_Pos_Data->VerticalVel_Valid     = FALSE;
             // BL HVel_AccEst_Valid;      Horizontal Velocity uncertainty field valid: HVel_AccEst.
             p_GN_SUPL_Pos_Data->HVel_AccEst_Valid     = FALSE;
             // BL VVel_AccEst_Valid;      Vertical Velocity uncertainty field valid: VVel_AccEst.
             p_GN_SUPL_Pos_Data->VVel_AccEst_Valid     = FALSE;
             }
          break;

          case GN_horandvervel:
          {
             // BL HorizontalVel_Valid;    Horizontal Velocity fields valid: HorizontalVel and Bearing.
             p_GN_SUPL_Pos_Data->HorizontalVel_Valid   = TRUE;
             // R4 HorizontalVel;          2-dimensional Horizontal Velocity [m/s].
             p_GN_SUPL_Pos_Data->HorizontalVel   = (R4) p_GN_Position->horspeed;
             // R4 Bearing;                2-dimensional Bearing [degrees].
             p_GN_SUPL_Pos_Data->Bearing         = (R4) p_GN_Position->bearing;

             // BL VerticalVel_Valid;      Vertical Velocity field valid: VerticalVel.
             p_GN_SUPL_Pos_Data->VerticalVel_Valid     = TRUE;
             // R4 VerticalVel;            Vertical Velocity [m/s, positive Up].
             p_GN_SUPL_Pos_Data->VerticalVel     = p_GN_Position->verspeed * ( p_GN_Position->verdirect == 0 ) ? 1.0f : -1.0f ;

             // No Horizontal and Vertical Velocity uncertainties available
             // BL HVel_AccEst_Valid;      Horizontal Velocity uncertainty field valid: HVel_AccEst.
             p_GN_SUPL_Pos_Data->HVel_AccEst_Valid     = FALSE;
             // BL VVel_AccEst_Valid;      Vertical Velocity uncertainty field valid: VVel_AccEst.
             p_GN_SUPL_Pos_Data->VVel_AccEst_Valid     = FALSE;
          }
          break;

          case GN_horveluncert:
          {
             // BL HorizontalVel_Valid;    Horizontal Velocity fields valid: HorizontalVel and Bearing.
             p_GN_SUPL_Pos_Data->HorizontalVel_Valid   = TRUE;
             // R4 HorizontalVel;          2-dimensional Horizontal Velocity [m/s].
             p_GN_SUPL_Pos_Data->HorizontalVel   = (R4) p_GN_Position->horspeed;
             // R4 Bearing;                2-dimensional Bearing [degrees].
             p_GN_SUPL_Pos_Data->Bearing         = (R4) p_GN_Position->bearing;
             // BL HVel_AccEst_Valid;      Horizontal Velocity uncertainty field valid: HVel_AccEst.
             p_GN_SUPL_Pos_Data->HVel_AccEst_Valid     = TRUE;
             p_GN_SUPL_Pos_Data->HVel_AccEst     = (R4) p_GN_Position->horuncertspeed;

             // No Vertical Velocity and uncertainty
             p_GN_SUPL_Pos_Data->VerticalVel_Valid     = FALSE;
             // BL VVel_AccEst_Valid;      Vertical Velocity uncertainty field valid: VVel_AccEst.
             p_GN_SUPL_Pos_Data->VVel_AccEst_Valid     = FALSE;
          }
          break;

          case GN_horandveruncert:
          {
             // BL HorizontalVel_Valid;    Horizontal Velocity fields valid: HorizontalVel and Bearing.
             p_GN_SUPL_Pos_Data->HorizontalVel_Valid   = TRUE;
             // R4 HorizontalVel;          2-dimensional Horizontal Velocity [m/s].
             p_GN_SUPL_Pos_Data->HorizontalVel   = (R4) p_GN_Position->horspeed;
             // R4 Bearing;                2-dimensional Bearing [degrees].
             p_GN_SUPL_Pos_Data->Bearing         = (R4) p_GN_Position->bearing;

             // BL VerticalVel_Valid;      Vertical Velocity field valid: VerticalVel.
             p_GN_SUPL_Pos_Data->VerticalVel_Valid     = TRUE;
             // R4 VerticalVel;            Vertical Velocity [m/s, positive Up].
             p_GN_SUPL_Pos_Data->VerticalVel     = p_GN_Position->verspeed * ( p_GN_Position->verdirect == 0 ) ? 1.0f : -1.0f ;

             // BL HVel_AccEst_Valid;      Horizontal Velocity uncertainty field valid: HVel_AccEst.
             p_GN_SUPL_Pos_Data->HVel_AccEst_Valid     = TRUE;
             p_GN_SUPL_Pos_Data->HVel_AccEst     = (R4) p_GN_Position->horuncertspeed;

             p_GN_SUPL_Pos_Data->VVel_AccEst_Valid     = TRUE;
             p_GN_SUPL_Pos_Data->VVel_AccEst     = (R4) p_GN_Position->veruncertspeed;
          }
          break;

          default:
             p_GN_SUPL_Pos_Data->HorizontalVel_Valid   = FALSE;
             p_GN_SUPL_Pos_Data->VerticalVel_Valid     = FALSE;
             // BL HVel_AccEst_Valid;      Horizontal Velocity uncertainty field valid: HVel_AccEst.
             p_GN_SUPL_Pos_Data->HVel_AccEst_Valid     = FALSE;
             // BL VVel_AccEst_Valid;      Vertical Velocity uncertainty field valid: VVel_AccEst.
             p_GN_SUPL_Pos_Data->VVel_AccEst_Valid     = FALSE;
       }
   }
   else
   {
      p_GN_SUPL_Pos_Data->HorizontalVel_Valid   = FALSE;
         p_GN_SUPL_Pos_Data->VerticalVel_Valid     = FALSE;
      // BL HVel_AccEst_Valid;      Horizontal Velocity uncertainty field valid: HVel_AccEst.
      p_GN_SUPL_Pos_Data->HVel_AccEst_Valid     = FALSE;
      // BL VVel_AccEst_Valid;      Vertical Velocity uncertainty field valid: VVel_AccEst.
         p_GN_SUPL_Pos_Data->VVel_AccEst_Valid     = FALSE;
   }

   if( p_GN_Position->confidencePresent )
   {
       // BL Confidence_Valid;       Confidence field valid: Confidence.
       p_GN_SUPL_Pos_Data->Confidence_Valid      = TRUE;
       // U1 Confidence;             Confidence of the solution in a percentage where RMS 1-sigma equals (67%) range [1..100].
       p_GN_SUPL_Pos_Data->Confidence  = p_GN_Position->confidence;
   }
   else
   {
       p_GN_SUPL_Pos_Data->Confidence_Valid      = FALSE;
   }
   return;
}


//*****************************************************************************
/// \brief
///      Function to take a copy of the wcdma cell information in a location ID.
///
/// \returns
///      #TRUE if successful.
///      #FALSE if unsuccessful.
//*****************************************************************************
BL supl_Copy_wcdmaCellInfo
(
   s_wcdmaCellInfo *p_WCDMA_dst, ///< [out] Cell information from a WCDMA network.
   s_wcdmaCellInfo *p_WCDMA_src  ///< [in] Cell information from a WCDMA network.
)
{
   BL Status = TRUE;
   p_WCDMA_dst->refMCC = p_WCDMA_src->refMCC;
   p_WCDMA_dst->refMNC = p_WCDMA_src->refMNC;
   p_WCDMA_dst->refUC  = p_WCDMA_src->refUC;
   p_WCDMA_dst->primaryScramblingCode = p_WCDMA_src->primaryScramblingCode;
   if ( p_WCDMA_src->p_FrequencyInfo != NULL )
   {
      p_WCDMA_dst->p_FrequencyInfo = GN_Calloc( 1, sizeof( s_FrequencyInfo ) );
      p_WCDMA_dst->p_FrequencyInfo->modeSpecificInfoType = p_WCDMA_src->p_FrequencyInfo->modeSpecificInfoType;
      switch ( p_WCDMA_src->p_FrequencyInfo->modeSpecificInfoType )
      {
      case fdd:   // Frequency Division Duplexed mode
         p_WCDMA_dst->p_FrequencyInfo->of_type.fdd.uarfcn_UL = p_WCDMA_src->p_FrequencyInfo->of_type.fdd.uarfcn_UL;
         p_WCDMA_dst->p_FrequencyInfo->of_type.fdd.uarfcn_DL = p_WCDMA_src->p_FrequencyInfo->of_type.fdd.uarfcn_DL;
         break;
      case tdd:   // Time Division Duplexed mode
         p_WCDMA_dst->p_FrequencyInfo->of_type.tdd.uarfcn_Nt = p_WCDMA_src->p_FrequencyInfo->of_type.tdd.uarfcn_Nt;
         break;
      default:
         GN_SUPL_Log( "supl_Copy_wcdmaCellInfo: Frequency Info mode Specific Info Type not set to fdd or tdd." );
         Status = FALSE;
         break;
      }
   }
   else
   {
      p_WCDMA_dst->p_FrequencyInfo = NULL;
   }

   if ( p_WCDMA_src->MeasuredResultCount > 0 && p_WCDMA_src->p_MeasuredResultsList != NULL )
   {
      U1 ResultCount;

      p_WCDMA_dst->p_MeasuredResultsList = GN_Calloc( p_WCDMA_src->MeasuredResultCount, sizeof( s_MeasuredResult ) );
      for ( ResultCount = 0 ; ResultCount < p_WCDMA_src->MeasuredResultCount ; ResultCount++ )
      {
         s_MeasuredResult* p_MeasuredResult_dst = &p_WCDMA_dst->p_MeasuredResultsList[ResultCount];
         s_MeasuredResult* p_MeasuredResult_src = &p_WCDMA_src->p_MeasuredResultsList[ResultCount];

         p_WCDMA_dst->MeasuredResultCount = p_WCDMA_src->MeasuredResultCount;

         p_MeasuredResult_dst->utra_CarrierRSSI        = p_MeasuredResult_src->utra_CarrierRSSI;
         p_MeasuredResult_dst->CellMeasuredResultCount = p_MeasuredResult_src->CellMeasuredResultCount;
         if ( p_MeasuredResult_src->CellMeasuredResultCount != 0 )
         {
            // CellMeasuredResults ::= SEQUENCE {
            //    cellIdentity      INTEGER(0..268435455) OPTIONAL,
            //    modeSpecificInfo
            //       CHOICE {fdd
            //                SEQUENCE {primaryCPICH-Info   PrimaryCPICH-Info,
            //                         cpich-Ec-N0          CPICH-Ec-N0 OPTIONAL,
            //                         cpich-RSCP           CPICH-RSCP OPTIONAL,
            //                         pathloss             Pathloss OPTIONAL},
            //               tdd
            //                SEQUENCE {cellParametersID    CellParametersID,
            //                         proposedTGSN         TGSN OPTIONAL,
            //                         primaryCCPCH-RSCP    PrimaryCCPCH-RSCP OPTIONAL,
            //                         pathloss             Pathloss OPTIONAL,
            //                         timeslotISCP-List    TimeslotISCP-List OPTIONAL}}}

            U1 count;
            p_MeasuredResult_dst->p_CellMeasuredResultsList = GN_Calloc( p_MeasuredResult_src->CellMeasuredResultCount, sizeof( s_CellMeasuredResult ) );
            for ( count = 0 ; count < p_MeasuredResult_dst->CellMeasuredResultCount ; count++ )
            {
               s_CellMeasuredResult* p_CellMeasuredResult_dst = &p_MeasuredResult_dst->p_CellMeasuredResultsList[count] ;
               s_CellMeasuredResult* p_CellMeasuredResult_src = &p_MeasuredResult_src->p_CellMeasuredResultsList[count] ;

               p_CellMeasuredResult_dst->cellIdentity         = p_CellMeasuredResult_src->cellIdentity ;
               p_CellMeasuredResult_dst->modeSpecificInfoType = p_CellMeasuredResult_src->modeSpecificInfoType ;
               switch ( p_CellMeasuredResult_src->modeSpecificInfoType )
               {
               case fdd:                 // Frequency Division Duplexed mode
                  p_CellMeasuredResult_dst->of_type.fdd.PrimaryScramblingCode = p_CellMeasuredResult_src->of_type.fdd.PrimaryScramblingCode ;
                  p_CellMeasuredResult_dst->of_type.fdd.cpich_Ec_N0           = p_CellMeasuredResult_src->of_type.fdd.cpich_Ec_N0 ;
                  p_CellMeasuredResult_dst->of_type.fdd.cpich_RSCP            = p_CellMeasuredResult_src->of_type.fdd.cpich_RSCP ;
                  p_CellMeasuredResult_dst->of_type.fdd.pathloss              = p_CellMeasuredResult_src->of_type.fdd.pathloss ;
                  break;
               case tdd:                 // Time Division Duplexed mode
                  p_CellMeasuredResult_dst->of_type.tdd.cellParametersID      = p_CellMeasuredResult_src->of_type.tdd.cellParametersID ;
                  p_CellMeasuredResult_dst->of_type.tdd.proposedTGSN          = p_CellMeasuredResult_src->of_type.tdd.proposedTGSN ;
                  p_CellMeasuredResult_dst->of_type.tdd.primaryCCPCH_RSCP     = p_CellMeasuredResult_src->of_type.tdd.primaryCCPCH_RSCP ;
                  p_CellMeasuredResult_dst->of_type.tdd.pathloss              = p_CellMeasuredResult_src->of_type.tdd.pathloss ;
                  p_CellMeasuredResult_dst->of_type.tdd.timeslotISCP_Count    = p_CellMeasuredResult_src->of_type.tdd.timeslotISCP_Count ;
                  if ( p_CellMeasuredResult_src->of_type.tdd.timeslotISCP_Count > 0)
                  {
                     U1 TimeslotISCPCount ;

                     p_CellMeasuredResult_dst->of_type.tdd.p_TimeslotISCP_List = GN_Calloc( p_CellMeasuredResult_src->of_type.tdd.timeslotISCP_Count, sizeof ( U1 ) );
                     for ( TimeslotISCPCount = 0 ; TimeslotISCPCount < p_CellMeasuredResult_src->of_type.tdd.timeslotISCP_Count ; TimeslotISCPCount++ )
                     {
                        p_CellMeasuredResult_dst->of_type.tdd.p_TimeslotISCP_List[TimeslotISCPCount] = p_CellMeasuredResult_src->of_type.tdd.p_TimeslotISCP_List[TimeslotISCPCount] ;
                     }
                  }
                  else
                  {
                     p_CellMeasuredResult_dst->of_type.tdd.p_TimeslotISCP_List = NULL;
                  }
                  break;
               default:
                  GN_SUPL_Log( "supl_Copy_wcdmaCellInfo: Cell Measured Result (%d) Specific Info Type not set to fdd or tdd.", count + 1);
                  Status = FALSE;
                  break;
               }
            }
         }
         p_MeasuredResult_dst->FrequencyInfoPresent = p_MeasuredResult_src->FrequencyInfoPresent;
         if ( p_MeasuredResult_src->FrequencyInfoPresent )
         {
            p_MeasuredResult_dst->FrequencyInfo.modeSpecificInfoType = p_MeasuredResult_src->FrequencyInfo.modeSpecificInfoType;
            switch ( p_WCDMA_src->p_MeasuredResultsList->FrequencyInfo.modeSpecificInfoType )
            {
            case fdd:   // Frequency Division Duplexed mode
               p_MeasuredResult_dst->FrequencyInfo.of_type.fdd.uarfcn_UL = p_MeasuredResult_src->FrequencyInfo.of_type.fdd.uarfcn_UL;
               p_MeasuredResult_dst->FrequencyInfo.of_type.fdd.uarfcn_DL = p_MeasuredResult_src->FrequencyInfo.of_type.fdd.uarfcn_DL;
               break;
            case tdd:   // Time Division Duplexed mode
               p_MeasuredResult_dst->FrequencyInfo.of_type.tdd.uarfcn_Nt = p_MeasuredResult_src->FrequencyInfo.of_type.tdd.uarfcn_Nt;
               break;
            default:
               GN_SUPL_Log( "supl_Copy_wcdmaCellInfo: Measured Results list mode Specific Info Type not set to fdd or tdd." );
               Status = FALSE;
               break;
            }
         }
      }
   }
   else
   {
      p_WCDMA_dst->p_MeasuredResultsList = NULL;
      p_WCDMA_dst->MeasuredResultCount = 0;
   }

   return Status;
}


//*****************************************************************************
/// \brief
///      Function to free up all the allocated data in a location ID.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_Free_LocationId_Data
(
   s_LocationId *p_LocationId ///< [in] Location Id to free.
)
{
   switch ( p_LocationId->Type )
   {
   case CIT_gsmCell:   // Cell information is from a GSM network.
      if ( p_LocationId->of_type.gsmCellInfo.p_NMRList != NULL )
      {
         GN_Free( p_LocationId->of_type.gsmCellInfo.p_NMRList );
         p_LocationId->of_type.gsmCellInfo.p_NMRList = NULL;
      }
      break;
   case CIT_wcdmaCell: // Cell information is from a WCDMA network.
      if ( p_LocationId->of_type.wcdmaCellInfo.p_FrequencyInfo != NULL )
      {
         GN_Free( p_LocationId->of_type.wcdmaCellInfo.p_FrequencyInfo );
         p_LocationId->of_type.wcdmaCellInfo.p_FrequencyInfo = NULL;
      }
      if ( p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList != NULL )
      {
         if ( p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList->p_CellMeasuredResultsList != NULL )
         {
            switch ( p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList->p_CellMeasuredResultsList->modeSpecificInfoType )
            {
            case fdd:                 // Frequency Division Duplexed mode
               break;
            case tdd:                 // Time Division Duplexed mode
               if ( p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList->p_CellMeasuredResultsList->of_type.tdd.p_TimeslotISCP_List != NULL )
               {
                  GN_Free( p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList->p_CellMeasuredResultsList->of_type.tdd.p_TimeslotISCP_List );
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList->p_CellMeasuredResultsList->of_type.tdd.p_TimeslotISCP_List = NULL;
               }
               break;
            }
            GN_Free( p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList->p_CellMeasuredResultsList );
            p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList->p_CellMeasuredResultsList = NULL;
         }
         GN_Free( p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList );
         p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList = NULL;
      }
      break;
   case CIT_cdmaCell:  // Cell information is from a CDMA network.
      break;
   case CIT_AccessPoint_WIMAX:
     break;
   case CIT_AccessPoint_UNKNOWN:
     break;
   case CIT_AccessPoint_WLAN:
     break;
   default:
     break;
   }

}


//*****************************************************************************
/// \brief
///      Function to free up all the allocated data in a ThirdParty Data that was filled during the SUPL instance.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_Free_ThirdParty_Data( s_GN_SUPL_V2_ThirdPartyInfo *p_ThirdParty_Data )
{
   if(p_ThirdParty_Data != NULL)
   {
      if(p_ThirdParty_Data->thirdPartyIdName != NULL)
      {
         GN_Free( p_ThirdParty_Data->thirdPartyIdName );
         p_ThirdParty_Data->thirdPartyIdName = NULL;
      }

      GN_Free( p_ThirdParty_Data );
      p_ThirdParty_Data = NULL;
   }
}
//*****************************************************************************
/// \brief
///      Function to free up all the allocated data in a p_ApplicationID  Data that was filled during the SUPL instance.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_Free_ApplicationID_Data(s_GN_SUPL_V2_ApplicationInfo *p_ApplicationID)
{
   if( p_ApplicationID != NULL )
   {
      if( p_ApplicationID->applicationProvider != NULL )
      {
         GN_Free( p_ApplicationID->applicationProvider );
         p_ApplicationID->applicationProvider = NULL;
      }
      if( p_ApplicationID->appName != NULL )
      {
         GN_Free( p_ApplicationID->appName );
         p_ApplicationID->appName = NULL;
      }
      if( p_ApplicationID->appVersion != NULL )
      {
         GN_Free( p_ApplicationID->appVersion );
         p_ApplicationID->appVersion = NULL;
      }
      if( p_ApplicationID != NULL )
      {
         GN_Free( p_ApplicationID );
         p_ApplicationID = NULL;
      }
   }
}

///      Function to log the Location ID gsm cell info contents.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_log_LocationId_gsmCell(
   CH             *Prefix,       ///< [in] Prefix to add to the beginning of the log string.
   s_gsmCellInfo  *p_gsmCellInfo ///< [in] Location ID gsmCellInfo required for the SUPL exchange.
)
{
   // INTEGER(0..999), -- Mobile Country Code
   if ( /* p_gsmCellInfo->refMCC < 0 && */ p_gsmCellInfo->refMCC > 999 )
   {
      GN_SUPL_Log( "%s: Cell Information == gsmCell: MCC %d out of range 0..999",
                   Prefix,
                   p_gsmCellInfo->refMCC );
   }
   // INTEGER(0..999), -- Mobile Network Code
   if ( /* p_gsmCellInfo->refMNC < 0 && */ p_gsmCellInfo->refMNC > 999 )
   {
      GN_SUPL_Log( "%s: Cell Information == gsmCell: MNC %d out of range 0..999",
                   Prefix,
                   p_gsmCellInfo->refMNC );
   }
   // INTEGER(0..65535), -- Location area code
   // INTEGER(0..255) OPTIONAL, --Timing Advance. (-1 = Not present).
   if ( p_gsmCellInfo->tA < -1 && p_gsmCellInfo->tA > 250 )
   {
      GN_SUPL_Log( "%s: Cell Information == gsmCell: tA %d out of range 0..250",
                   Prefix,
                   p_gsmCellInfo->tA );
   }
   GN_SUPL_Log( "%s: Cell Information == gsmCell: refMCC = %u, refMNC = %u, refLAC = %u, refCI = %u, tA = %d, NMRCount = %u",
                Prefix,
                p_gsmCellInfo->refMCC,
                p_gsmCellInfo->refMNC,
                p_gsmCellInfo->refLAC,
                p_gsmCellInfo->refCI,
                p_gsmCellInfo->tA,
                p_gsmCellInfo->NMRCount );
   if ( p_gsmCellInfo->NMRCount != 0 )
   {
      s_NMRElement *p_NMRSource;
      U1 i;

      p_NMRSource = p_gsmCellInfo->p_NMRList;
      for ( i = 0 ; i < p_gsmCellInfo->NMRCount ; i++ )
      {
         // INTEGER(0..1023),
         if ( /* p_NMRSource->aRFCN < 0 && */ p_NMRSource->aRFCN > 1023 )
         {
            GN_SUPL_Log( "%s: Cell Information == gsmCell: NMR %u of %u : aRFCN %d out of range 0..1023",
                         Prefix,
                         i + 1,
                         p_gsmCellInfo->NMRCount,
                         p_NMRSource->aRFCN );
         }
         // INTEGER(0..63),
         if ( /* p_NMRSource->bSIC < 0 && */ p_NMRSource->bSIC > 63 )
         {
            GN_SUPL_Log( "%s: Cell Information == gsmCell: NMR %u of %u : bSIC %d out of range 0..63",
                         Prefix,
                         i + 1,
                         p_gsmCellInfo->NMRCount,
                         p_NMRSource->bSIC );
         }
         // INTEGER(0..63),
         if ( /* p_NMRSource->rxLev < 0 && */ p_NMRSource->rxLev > 63 )
         {
            GN_SUPL_Log( "%s: Cell Information == gsmCell: NMR %u of %u : rxLev %d out of range 0..63",
                         Prefix,
                         i + 1,
                         p_gsmCellInfo->NMRCount,
                         p_NMRSource->rxLev );
         }
         GN_SUPL_Log( "%s: NMR %u of %u : aRFCN = %u, bSIC = %u, rxLev = %u",
                      Prefix,
                      i + 1,
                      p_gsmCellInfo->NMRCount,
                      p_NMRSource->aRFCN,
                      p_NMRSource->bSIC,
                      p_NMRSource->rxLev );
         p_NMRSource++;
      }
   }
}


//*****************************************************************************
/// \brief
///      Function to log the Location ID cdma cell info contents.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_log_LocationId_cdmaCell(
   CH             *Prefix,       ///< [in] Prefix to add to the beginning of the log string.
   s_cdmaCellInfo *p_cdmaCellInfo///< [in] Location ID cdmaCellInfo required for the SUPL exchange.
)
{
   // INTEGER(0..65535), -- Network Id
   //if ( p_cdmaCellInfo->refNID < 0 && p_cdmaCellInfo->refNID > 65535 )
   //{
   //   GN_SUPL_Log( "%s: Cell Information == cdmaCell: refNID %u out of range 0..65535",
   //                Prefix,
   //                p_cdmaCellInfo->refNID );
   //}
   // INTEGER(0..32767), -- System Id
   if ( /* p_cdmaCellInfo->refSID < 0 &&  */ p_cdmaCellInfo->refSID > 32767 )
   {
      GN_SUPL_Log( "%s: Cell Information == cdmaCell: refSID %u out of range 0..32767",
                   Prefix,
                   p_cdmaCellInfo->refSID );
   }
   // INTEGER(0..4194303), -- Base Station Latitude
   if ( /* p_cdmaCellInfo->refBASELAT < 0 && */ p_cdmaCellInfo->refBASELAT > 4194303 )
   {
      GN_SUPL_Log( "%s: Cell Information == cdmaCell: refBASELAT %u out of range 0..4194303",
                   Prefix,
                   p_cdmaCellInfo->refBASELAT );
   }
   // INTEGER(0..8388607), -- Base Station Longitude
   if ( /* p_cdmaCellInfo->reBASELONG < 0 && */ p_cdmaCellInfo->reBASELONG > 8388607 )
   {
      GN_SUPL_Log( "%s: Cell Information == cdmaCell: reBASELONG %u out of range 0..8388607",
                   Prefix,
                   p_cdmaCellInfo->reBASELONG );
   }
   // INTEGER(0..511), -- Base Station PN Code
   if ( /* p_cdmaCellInfo->refREFPN < 0 && */ p_cdmaCellInfo->refREFPN > 511 )
   {
/* Coverity Error :self_assign
      p_cdmaCellInfo->refREFPN      = p_cdmaCellInfo->refREFPN;        ///< INTEGER(0..511), -- Base Station PN Code */
      GN_SUPL_Log( "%s: Cell Information == cdmaCell: refREFPN %u out of range 0..511",
                   Prefix,
                   p_cdmaCellInfo->refREFPN );
   }
   // INTEGER(0..65535), -- GPS Week Number
   //if ( p_cdmaCellInfo->refWeekNumber < 0 && p_cdmaCellInfo->refWeekNumber > 65535 )
   //{
   //   GN_SUPL_Log( "%s: Cell Information == cdmaCell: refWeekNumber %u out of range 0..65535",
   //                Prefix,
   //                p_cdmaCellInfo->refWeekNumber );
   //}
   // INTEGER(0..4194303),-- GPS Seconds
   if ( /* p_cdmaCellInfo->refSeconds < 0 && */ p_cdmaCellInfo->refSeconds > 4194303 )
   {
      GN_SUPL_Log( "%s: Cell Information == cdmaCell: refSeconds %u out of range 0..4194303",
                   Prefix,
                   p_cdmaCellInfo->refSeconds );
   }
   GN_SUPL_Log( "%s: Cell Information == cdmaCell: refNID = %d, refSID = %d, refBASEID = %d, refBASELAT = %d, reBASELONG = %d, refREFPN = %d, refWeekNumber = %d, refSeconds = %d",
                Prefix,
                p_cdmaCellInfo->refNID,
                p_cdmaCellInfo->refSID,
                p_cdmaCellInfo->refBASEID,
                p_cdmaCellInfo->refBASELAT,
                p_cdmaCellInfo->reBASELONG,
                p_cdmaCellInfo->refREFPN,
                p_cdmaCellInfo->refWeekNumber,
                p_cdmaCellInfo->refSeconds );
}


//*****************************************************************************
/// \brief
///      Function to log the Location ID wcdma cell info contents.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_log_LocationId_wcdmaCell(
   CH                *Prefix,          ///< [in] Prefix to add to the beginning of the log string.
   s_wcdmaCellInfo   *p_wcdmaCellInfo  ///< [in] Location ID wcdmaCellInfo required for the SUPL exchange.
)
{
   // U2 refMCC;        INTEGER(0..999), -- Mobile Country Code
   if ( /* p_wcdmaCellInfo->refMCC < 0 && */ p_wcdmaCellInfo->refMCC > 999 )
   {
      GN_SUPL_Log( "%s: Cell Information == wcdmaCell: refMCC %u out of range 0..999",
                   Prefix,
                   p_wcdmaCellInfo->refMCC );
   }
   // U2 refMNC;         INTEGER(0..999), -- Mobile Network Code
   if ( /* p_wcdmaCellInfo->refMNC < 0 && */ p_wcdmaCellInfo->refMNC > 999 )
   {
      GN_SUPL_Log( "%s: Cell Information == wcdmaCell: refMNC %u out of range 0..999",
                   Prefix,
                   p_wcdmaCellInfo->refMNC );
   }
   // U4 refUC;          INTEGER(0..268435455), -- Cell identity
   if ( /* p_wcdmaCellInfo->refUC < 0 && */ p_wcdmaCellInfo->refUC > 268435455 )
   {
      GN_SUPL_Log( "%s: Cell Information == wcdmaCell: refUC %u out of range 0..268435455",
                   Prefix,
                   p_wcdmaCellInfo->refUC );
   }
   // I2 primaryScramblingCode;  INTEGER(0..511) OPTIONAL (-1 = not present).,
   if ( p_wcdmaCellInfo->primaryScramblingCode < -1 && p_wcdmaCellInfo->primaryScramblingCode > 511 )
   {
      GN_SUPL_Log( "%s: Cell Information == wcdmaCell: primaryScramblingCode %u out of range 0..511",
                   Prefix,
                   p_wcdmaCellInfo->primaryScramblingCode );
   }

   GN_SUPL_Log( "%s: Cell Information == wcdmaCell: refMCC = %u, refMNC = %u, refUC = %u, primaryScramblingCode = %d",
                Prefix,
                p_wcdmaCellInfo->refMCC,
                p_wcdmaCellInfo->refMNC,
                p_wcdmaCellInfo->refUC,
                p_wcdmaCellInfo->primaryScramblingCode );
   if ( p_wcdmaCellInfo->p_FrequencyInfo != NULL )
   {
      // s_FrequencyInfo*
      //    p_FrequencyInfo;           FrequencyInfo OPTIONAL
      switch ( p_wcdmaCellInfo->p_FrequencyInfo->modeSpecificInfoType )
      {
      case fdd:                 // Frequency Division Duplexed mode
         if ( p_wcdmaCellInfo->p_FrequencyInfo->of_type.fdd.uarfcn_UL < -1 &&
              p_wcdmaCellInfo->p_FrequencyInfo->of_type.fdd.uarfcn_UL > 16383 )
         {
            GN_SUPL_Log( "%s: Cell Information == wcdmaCell: FrequencyInfo fdd: uarfcn_UL = %d out of range 0..16383",
                         Prefix,
                         p_wcdmaCellInfo->p_FrequencyInfo->of_type.fdd.uarfcn_UL );    // INTEGER(0..16383), OPTIONAL (-1 = Not present).
         }
         if ( /* p_wcdmaCellInfo->p_FrequencyInfo->of_type.fdd.uarfcn_DL < 0 && */
              p_wcdmaCellInfo->p_FrequencyInfo->of_type.fdd.uarfcn_DL > 16383 )
         {
            GN_SUPL_Log( "%s: Cell Information == wcdmaCell: FrequencyInfo fdd: uarfcn_DL = %u out of range 0..16383",
                         Prefix,
                         p_wcdmaCellInfo->p_FrequencyInfo->of_type.fdd.uarfcn_DL );    // INTEGER(0..16383).
         }
         GN_SUPL_Log( "%s: Cell Information == wcdmaCell: FrequencyInfo fdd: uarfcn_UL = %d, uarfcn_DL = %u",
                      Prefix,
                      p_wcdmaCellInfo->p_FrequencyInfo->of_type.fdd.uarfcn_UL,         // INTEGER(0..16383), OPTIONAL (-1 = Not present).
                      p_wcdmaCellInfo->p_FrequencyInfo->of_type.fdd.uarfcn_DL );       // INTEGER(0..16383)
         break;
      case tdd:                 //  Time Division Duplexed mode
         if ( /* p_wcdmaCellInfo->p_FrequencyInfo->of_type.tdd.uarfcn_Nt < 0 && */
              p_wcdmaCellInfo->p_FrequencyInfo->of_type.tdd.uarfcn_Nt > 16383 )
         {
            GN_SUPL_Log( "%s: Cell Information == wcdmaCell: FrequencyInfo tdd: uarfcn_Nt = %u out of range 0..16383",
                         Prefix,
                         p_wcdmaCellInfo->p_FrequencyInfo->of_type.tdd.uarfcn_Nt );    // INTEGER(0..16383).
         }
         GN_SUPL_Log( "%s: Cell Information == wcdmaCell: FrequencyInfo tdd: uarfcn_Nt = %u",
                      Prefix,
                      p_wcdmaCellInfo->p_FrequencyInfo->of_type.tdd.uarfcn_Nt );       ///< INTEGER(0..16383)
         break;
      }
   }
   if ( p_wcdmaCellInfo->MeasuredResultCount != 0 )
   {
      U1 i;
      // U1 MeasuredResultCount;       maxFreq INTEGER ::= 8
      // s_MeasuredResult*
      //    p_MeasuredResultsList;     OPTIONAL (Present if MeasuredResultCount > 0). Pointer to array of s_MeasuredResult.
         GN_SUPL_Log( "%s: Cell Information == wcdmaCell: MeasuredResult %u in total",
                      Prefix,
                      p_wcdmaCellInfo->MeasuredResultCount );
      for ( i = 0 ; i < p_wcdmaCellInfo->MeasuredResultCount ; i++ )
      {
         GN_SUPL_Log( "%s: Cell Information == wcdmaCell: Measurement %u of %u Utra_CarrierRSSI %d",
                      Prefix,
                      i + 1,
                      p_wcdmaCellInfo->MeasuredResultCount,
                      p_wcdmaCellInfo->p_MeasuredResultsList[i].utra_CarrierRSSI );

         if ( p_wcdmaCellInfo->p_MeasuredResultsList[i].FrequencyInfoPresent )
         {
            GN_SUPL_Log( "%s: Cell Information == wcdmaCell: Measurement %u of %u Frequency info present:",
                         Prefix,
                         i + 1,
                         p_wcdmaCellInfo->MeasuredResultCount );
            switch ( p_wcdmaCellInfo->p_MeasuredResultsList[i].FrequencyInfo.modeSpecificInfoType )
            {
            case fdd:                 // Frequency Division Duplexed mode
               GN_SUPL_Log( "%s: Cell Information == wcdmaCell: Measurement %u of %u type fdd uarfcn_DL = %u",
                            Prefix,
                            i + 1,
                            p_wcdmaCellInfo->MeasuredResultCount,
                            p_wcdmaCellInfo->p_MeasuredResultsList[i].FrequencyInfo.of_type.fdd.uarfcn_DL );
               GN_SUPL_Log( "%s: Cell Information == wcdmaCell: Measurement %u of %u type fdd uarfcn_DL = %d",
                            Prefix,
                            i + 1,
                            p_wcdmaCellInfo->MeasuredResultCount,
                            p_wcdmaCellInfo->p_MeasuredResultsList[i].FrequencyInfo.of_type.fdd.uarfcn_UL );
               break;
            case tdd:                  // Time Division Duplexed mode
               GN_SUPL_Log( "%s: Cell Information == wcdmaCell: Measurement %u of %u type tdd uarfcn_Nt = %d",
                            Prefix,
                            i + 1,
                            p_wcdmaCellInfo->MeasuredResultCount,
                            p_wcdmaCellInfo->p_MeasuredResultsList[i].FrequencyInfo.of_type.tdd.uarfcn_Nt );
               break;
            }
         }
         if ( p_wcdmaCellInfo->p_MeasuredResultsList[i].CellMeasuredResultCount != 0 )
         {
            U1 j;
            GN_SUPL_Log( "%s: Cell Information == wcdmaCell: MeasuredResult %u : cell measured result %d in total",
                         Prefix,
                         i + 1,
                         p_wcdmaCellInfo->p_MeasuredResultsList[i].CellMeasuredResultCount );
            for ( j = 0 ; j < p_wcdmaCellInfo->p_MeasuredResultsList[i].CellMeasuredResultCount ; j++ )
            {
               s_CellMeasuredResult *p_ThisCellMeasuredResult = &p_wcdmaCellInfo->p_MeasuredResultsList[i].p_CellMeasuredResultsList[j];
               GN_SUPL_Log( "%s: Cell Information == wcdmaCell: MeasuredResult %u : cell measured result %d of %d: cellIdentity %i",
                            Prefix,
                            i + 1,
                            j + 1,
                            p_wcdmaCellInfo->p_MeasuredResultsList[i].CellMeasuredResultCount,
                            p_ThisCellMeasuredResult->cellIdentity );
               switch ( p_ThisCellMeasuredResult->modeSpecificInfoType )
               {
               case fdd:                 // Frequency Division Duplexed mode
                  GN_SUPL_Log( "%s: Cell Information == wcdmaCell: MeasuredResult %u : cell measured result %d of %d: cellIdentity %d of type fdd: PrimaryScramblingCode: %u, cpich_Ec_N0: %d, cpich_RSCP: %d, pathloss %u",
                               Prefix,
                               i + 1,
                               j + 1,
                               p_wcdmaCellInfo->p_MeasuredResultsList[i].CellMeasuredResultCount,
                               p_ThisCellMeasuredResult->cellIdentity,
                               p_ThisCellMeasuredResult->of_type.fdd.PrimaryScramblingCode,
                               p_ThisCellMeasuredResult->of_type.fdd.cpich_Ec_N0,
                               p_ThisCellMeasuredResult->of_type.fdd.cpich_RSCP,
                               p_ThisCellMeasuredResult->of_type.fdd.pathloss );
                  break;
               case tdd:                 // Time Division Duplexed mode
                  GN_SUPL_Log( "%s: Cell Information == wcdmaCell: MeasuredResult %u : cell measured result %d of %d: cellIdentity %d of type tdd: cellParametersID: %u, proposedTGSN: %d, primaryCCPCH_RSCP: %d, pathloss: %u, timeslotISCP_Count: %d",
                               Prefix,
                               i + 1,
                               j + 1,
                               p_wcdmaCellInfo->p_MeasuredResultsList[i].CellMeasuredResultCount,
                               p_ThisCellMeasuredResult->cellIdentity,
                               p_ThisCellMeasuredResult->of_type.tdd.cellParametersID,
                               p_ThisCellMeasuredResult->of_type.tdd.proposedTGSN,
                               p_ThisCellMeasuredResult->of_type.tdd.primaryCCPCH_RSCP,
                               p_ThisCellMeasuredResult->of_type.tdd.pathloss,
                               p_ThisCellMeasuredResult->of_type.tdd.timeslotISCP_Count );
                  {
                     U1 TimeslotISCP_Count;
                     for (TimeslotISCP_Count = 0 ; TimeslotISCP_Count < p_ThisCellMeasuredResult->of_type.tdd.timeslotISCP_Count ; TimeslotISCP_Count++ )
                     {
                        GN_SUPL_Log( "%s: Cell Information == wcdmaCell: MeasuredResult %u : cell measured result %d of %d: cellIdentity %d, TimeslotISCP %d of %d: timeslotISCP: %d",
                                     Prefix,
                                     i + 1,
                                     j + 1,
                                     p_wcdmaCellInfo->p_MeasuredResultsList[i].CellMeasuredResultCount,
                                     p_ThisCellMeasuredResult->cellIdentity,
                                     TimeslotISCP_Count + 1,
                                     p_ThisCellMeasuredResult->of_type.tdd.timeslotISCP_Count,
                                     p_ThisCellMeasuredResult->of_type.tdd.p_TimeslotISCP_List[TimeslotISCP_Count] );
                     }
                  }
                  break;
               }
            }
         }
      }
   }
}


//*****************************************************************************
/// \brief
///      Function to log the Location ID contents.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_log_LocationId(
   CH             *Prefix,       ///< [in] Prefix to add to the beginning of the log string.
   s_LocationId   *p_LocationId  ///< [in] Location ID required for the SUPL exchange.
)
{

   CH *tempStr ;
   switch (p_LocationId->Status)
   {
   case CIS_stale:   tempStr = "Stale"   ; break ;
   case CIS_current: tempStr = "Current" ; break ;
   case CIS_unknown: tempStr = "Unknown" ; break ;

   case CIS_NONE:         // Cell information is not yet available.
   default:
      GN_SUPL_Log( "%s: Cell Information is not available.", Prefix);
      return ;
      break ;
   }

   GN_SUPL_Log( "%s: Cell Information Status: %s.", Prefix, tempStr );

   switch ( p_LocationId->Type )
   {
   case CIT_gsmCell:          // Cell information is from a GSM network.
      supl_log_LocationId_gsmCell( Prefix, &p_LocationId->of_type.gsmCellInfo );
      break;
   case CIT_cdmaCell:         // Cell information is from a CDMA network.
      supl_log_LocationId_cdmaCell( Prefix, &p_LocationId->of_type.cdmaCellInfo );
      break;
   case CIT_wcdmaCell:        // Cell information is from a WCDMA network.
      supl_log_LocationId_wcdmaCell( Prefix, &p_LocationId->of_type.wcdmaCellInfo );
      break;
   case CIT_AccessPoint_WIMAX:
     break;
   case CIT_AccessPoint_UNKNOWN:
     break;
   case CIT_AccessPoint_WLAN:
     break;
   default:
     break;
   }
}
//*****************************************************************************
/// \brief
///      Function to return the preferred positioning method assuming PDU is SUPL START
///
/// \returns
///      The type of message as an enum of #e_GN_PrefMethod
//*****************************************************************************
e_GN_PrefMethod supl_PDU_Get_SUPLSTART_PreferredMethod
(
   void *p_ThisPDU   ///< [in] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
)
{
    ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

    switch( ASN1_GET(ASN1_ENUM_1_GET(p_PDU_Cast->message.choice.msSUPLSTART.sETCapabilities.prefMethod) ) )
    {
       case     PrefMethod_agpsSETassistedPreferred    : return GN_agpsSETassistedPreferred;
        case     PrefMethod_agpsSETBasedPreferred: return GN_agpsSETBasedPreferred;
        case     PrefMethod_noPreference    : return GN_noPreference;
    }

    return GN_noPreference;
}

//*****************************************************************************
/// \brief
///      Function to return the status code from message
///
/// \returns
///      The type of message as an enum of #e_GN_StatusCode
//*****************************************************************************
e_GN_StatusCode supl_PDU_Get_SUPLEND_StatusCode
(
   void *p_ThisPDU   ///< [in] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;
   e_GN_StatusCode status = GN_StatusCode_NONE;

   if( p_PDU_Cast->message.present != UlpMessage_PR_msSUPLEND )
      return status;

   if( !p_PDU_Cast->message.choice.msSUPLEND.statusCode )
      return status;

   switch( ASN1_GET(ASN1_ENUM_2_GET(p_PDU_Cast->message.choice.msSUPLEND.statusCode) ) )
   {
      case unspecified  : status = GN_StatusCode_unspecified; break;
      case systemFailure : status = GN_StatusCode_systemFailure; break;
      case unexpectedMessage : status = GN_StatusCode_unexpectedMessage; break;
      case protocolError : status = GN_StatusCode_protocolError; break;
      case dataMissing : status = GN_StatusCode_dataMissing; break;
      case unexpectedDataValue : status = GN_StatusCode_unexpectedDataValue; break;
      case posMethodFailure : status = GN_StatusCode_posMethodFailure; break;
      case posMethodMismatch : status = GN_StatusCode_posMethodMismatch; break;
      case posProtocolMismatch : status = GN_StatusCode_posProtocolMismatch; break;
      case targetSETnotReachable : status = GN_StatusCode_targetSETnotReachable; break;
      case versionNotSupported : status = GN_StatusCode_versionNotSupported; break;
      case resourceShortage : status = GN_StatusCode_resourceShortage; break;
      case invalidSessionId : status = GN_StatusCode_invalidSessionId; break;
      case nonProxyModeNotSupported : status = GN_StatusCode_nonProxyModeNotSupported; break;
      case proxyModeNotSupported : status = GN_StatusCode_proxyModeNotSupported; break;
      case positioningNotPermitted : status = GN_StatusCode_positioningNotPermitted; break;
      case authNetFailure : status = GN_StatusCode_authNetFailure; break;
      case authSuplinitFailure : status = GN_StatusCode_authSuplinitFailure; break;
      case consentDeniedByUser : status = GN_StatusCode_consentDeniedByUser; break;
      case consentGrantedByUser : status = GN_StatusCode_consentGrantedByUser; break;
      case ver2_sessionStopped : status = GN_StatusCode_ver2_sessionStopped; break;
      default: break;
   }

   return status;
}

//*****************************************************************************
/// \brief
///      Function to return the triggered Mode in SUPL INIT message.
///
/// \returns
///      TRUE if triggered mode is present in SUPL INIT. Triggered mode is stored in *p_TriggeredMode
///      FALSE if triggered mode is not present
//*****************************************************************************
BL supl_PDU_Get_SUPLINIT_TriggeredMode
(
   void *p_ThisPDU,  ///< [in] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   e_GN_SUPL_V2_TriggeredMode *p_TriggeredMode    ///< [in] Pointer to receive triggered mode in case available
)
{
    ULP_PDU_t *p_CastPdu = p_ThisPDU;
    SUPLINIT_t *p_SuplInit = &p_CastPdu->message.choice.msSUPLINIT;

    if( p_SuplInit->ver2_SUPL_INIT_extension )
    {
        if( p_SuplInit->ver2_SUPL_INIT_extension->triggerType )
        {
            switch( ASN1_GET(ASN1_ENUM_2_GET(p_SuplInit->ver2_SUPL_INIT_extension->triggerType)) )
            {
                case TriggerType_areaEvent : *p_TriggeredMode = GN_TriggeredModeAreaEvent; return TRUE;
                case TriggerType_periodic  : *p_TriggeredMode = GN_TriggeredModePeriodic ; return TRUE;
            }
        }
    }
    else
        *p_TriggeredMode = GN_TriggeredModeNone;

    return FALSE;
}

//*****************************************************************************
/// \brief
///      Function to add Position Estimate to the unencoded SUPL PDU type SUPL-TRIGGERED-START.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLTRIGGEREDSTART_Position
(
   void *p_ThisPDU,  ///< [in] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   s_GN_Position  *p_GN_Position ///<[in] Position Estimate.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTART.position = GN_Calloc( 1 , sizeof(Position_t ) );

   supl_PDU_Add_Position( p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTART.position , p_GN_Position );
}


//*****************************************************************************
/// \brief
///      Function to set the unencoded SUPL PDU type to SUPL-TRIGGERED-START.
///
/// \returns
///      Nothing.
//*****************************************************************************
static void supl_PDU_Add_SUPLTRIGGEREDSTART
(
   void *p_ThisPDU   ///< [in/out] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
)
{

   GN_SUPL_Log( "Inside supl_PDU_Add_SUPLTRIGGEREDSTART" );


   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   //  SUPL-TRIGGERED-START defined in  SUPL-TRIGGERED-START.asn1
   p_PDU_Cast->message.present = UlpMessage_PR_msSUPLTRIGGEREDSTART;
}

//*****************************************************************************
/// \brief
///      Function to generate and add VER to the unencoded SUPL PDU type
///      SUPL-TRIGGERED-START.
///
/// \returns
///      Nothing.
//*****************************************************************************
static void supl_PDU_Add_SUPLTRIGGEREDSTART_VER
(
   void  *p_ThisPDU, ///< [in] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   U1*   p_VER       ///< VER generated from SUPL-INIT.
)
{

   GN_SUPL_Log( "Inside supl_PDU_Add_SUPLTRIGGEREDSTART_VER" );

   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   if ( p_VER != NULL )
   {
      p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTART.ver = GN_Calloc(1, sizeof(Ver_t));

      supl_PDU_Add_SDL_BitString_U1_ptr( p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTART.ver , p_VER , GN_GPS_VER_LEN * 8 );
    }
}

//*****************************************************************************
/// \brief
///      Function to add Set capabilities into an unencoded SUPL-TRIGGERED-START PDU.
///
/// \returns
///      Nothing.
//*****************************************************************************
static void supl_PDU_Add_SUPLTRIGGEREDSTART_SetCapabilities
(
   void *p_ThisPDU,  ///< [in/out] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   s_GN_SetCapabilities *p_SetCapabilities  ,   ///< [in] Set capabilities in
                                                ///   GN format.
   s_SUPL_Instance *p_SUPL_Instance             /// Added for version check                               
)
{
   GN_SUPL_Log( "Inside supl_PDU_Add_SUPLTRIGGEREDSTART_SetCapabilities" );

   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   supl_PDU_Add_SetCapabilities(
       &p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTART.sETCapabilities,
       p_SetCapabilities , p_SUPL_Instance );
}


//*****************************************************************************
/// \brief
///      Function to add Set Location ID into an unencoded SUPL-TRIGGERED-START PDU.
///
/// \returns
///      Nothing.
//*****************************************************************************
static void supl_PDU_Add_SUPLTRIGGEREDSTART_SetLocationID
(
   void *p_ThisPDU,  ///< [in/out] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   s_LocationId *p_SetLocationID    ///< [in] Set Location ID.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   supl_PDU_Add_SetLocationID(
      &p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTART.locationId,
      p_SetLocationID );
}


//*****************************************************************************
/// \brief
///      Function to add Trigger Type into an unencoded SUPL-TRIGGERED-START PDU.
///
/// \returns
///      Nothing.
//*****************************************************************************
static void supl_PDU_Add_SUPLTRIGGEREDSTART_TriggerInfo
(
   void *p_ThisPDU,  ///< [in/out] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   s_SUPL_Instance *p_SUPL_Instance, ///< [in] Current instance data used to
                                    ///   populate parts of the PDU.
   s_GN_SetCapabilities *p_SetCapabilities
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;
   Ver2_SUPLTRIGGEREDSTART_t *p_SuplTriggeredStart = &p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTART;
   TriggerType_t  triggerType;


   p_SuplTriggeredStart->triggerType = GN_Calloc( 1 , sizeof( TriggerType_t ) );

   switch( p_SUPL_Instance->p_SUPL->V2_Data.Triggers.triggeredMode )
   {
      case GN_TriggeredModePeriodic :
         ASN1_ENUM_SET( triggerType , TriggerType_periodic );

         memcpy( p_SuplTriggeredStart->triggerType, &triggerType, sizeof( TriggerType_t ));

         p_SuplTriggeredStart->reportingCap = GN_Calloc( 1 , sizeof( ReportingCap_t ) );
         supl_PDU_Add_ReportingCapabilities( p_SuplTriggeredStart->reportingCap , p_SetCapabilities );

         if(p_SUPL_Instance->p_SUPL->NetworkInitiated == FALSE)
         {
            p_SuplTriggeredStart->triggerParams = GN_Calloc( 1 , sizeof( TriggerParams_t ) );
            p_SuplTriggeredStart->triggerParams->present = TriggerParams_PR_periodicParams;
            supl_PDU_Add_TriggerParamsPeriodic( &p_SuplTriggeredStart->triggerParams->choice.periodicParams,
                                                &p_SUPL_Instance->p_SUPL->V2_Data.Triggers.periodicTrigger);
            GN_SUPL_Log( "DBG TRIGGEREDSTART:   Done");
         }
         break;
      case GN_TriggeredModeAreaEvent:
         ASN1_ENUM_SET( triggerType , TriggerType_areaEvent );

         memcpy( p_SuplTriggeredStart->triggerType, &triggerType, sizeof( TriggerType_t ));

         if( p_SUPL_Instance->p_SUPL->NetworkInitiated == FALSE )
         {
            p_SuplTriggeredStart->triggerParams = GN_Calloc( 1 , sizeof( TriggerParams_t ) );
            p_SuplTriggeredStart->triggerParams->present = TriggerParams_PR_areaEventParams;
            supl_PDU_Add_TriggerParamsAreaEvent( &p_SuplTriggeredStart->triggerParams->choice.areaEventParams,
                                                 &p_SUPL_Instance->p_SUPL->V2_Data.Triggers.areaEventTrigger);
            GN_SUPL_Log( "DBG TRIGGEREDSTART: supl_PDU_Add_TriggerParamsAreaEvent Done");
         }
         /*@TODO - supl_PDU_Add_TriggerParamsAreaEvent to be filled up*/

         break;
      default:
          GN_SUPL_Log( "ERR: Trigger Mode not set");
         /*@TODO: some error must be notified*/
          break;
   }
}

//*****************************************************************************
/// \brief
///      High level function to populate an unencoded SUPL-TRIGGERED-START PDU.
///
/// \returns
///      Pointer to unencoded structure ULP_PDU_t. Returned as (void *) to hide
///      implementation.
//*****************************************************************************
void *supl_PDU_Make_SUPLTRIGGEREDSTART
(
   s_SUPL_Instance *p_SUPL_Instance ///< [in] Current instance data used to
                                    ///   populate parts of the PDU.
)
{
   void *p_PDU_Src = NULL;
   s_GN_SetCapabilities SetCapabilities;
   s_SUPL_InstanceData  *p_SUPL = p_SUPL_Instance->p_SUPL; // SUPL specific data.

   p_PDU_Src = supl_PDU_Alloc();

   supl_PDU_Add_Version( p_PDU_Src , p_SUPL_Instance);

   supl_PDU_Add_setSessionID( p_PDU_Src, p_SUPL->SetSessionId,   &p_SUPL->SetId );

   if(p_SUPL->NetworkInitiated)
   {
      supl_PDU_Add_slpSessionID( p_PDU_Src, p_SUPL->p_SlpSessionId, &p_SUPL->SlpId );
   }

   supl_PDU_Add_SUPLTRIGGEREDSTART( p_PDU_Src );
   supl_get_SetCapabilities( p_SUPL_Instance , &SetCapabilities );   

   supl_PDU_Add_SUPLTRIGGEREDSTART_SetCapabilities( p_PDU_Src, &SetCapabilities , p_SUPL_Instance );

   supl_PDU_Add_SUPLTRIGGEREDSTART_SetLocationID( p_PDU_Src, &p_SUPL->LocationId );

   if( SUPL_Mlid_FirstElem != NULL)
   {
      supl_PDU_Add_SUPLTRIGGEREDSTART_Multiple_LocationID( p_PDU_Src , p_SUPL_Instance );
   }
   // Note: This needed to be removed for testing with Andrew.
   if ((p_SUPL->NetworkInitiated)&&( p_SUPL->p_VER != NULL ))
   {
     supl_PDU_Add_SUPLTRIGGEREDSTART_VER( p_PDU_Src, p_SUPL->p_VER );
     // The below is done as a SUPL POS INIT session can be triggered following the SUPL TRIGGERED RESPONSE and
     // we should not send a VER field in that message.
     GN_Free(p_SUPL->p_VER); // @TODO Need to check the sideffects
     p_SUPL->p_VER = NULL;   // @TODO Need to check the sideffects
   }


   /* Following items need to be specified based on whether SI or NI Triggering */
   /* 1. Trigger Type
      2. Reporting Capability
      3. Trigger Params ONLY in case of SI. NOT SET in case of NI  @TODO
      4. Application ID - In case of SI                            @TODO
      */

   supl_PDU_Add_SUPLTRIGGEREDSTART_TriggerInfo( p_PDU_Src , p_SUPL_Instance , &SetCapabilities );

   if( p_SUPL->MO_Request )
   {
        if(p_SUPL->V2_Data.p_ThirdPartyData != NULL)
        {
            supl_PDU_Add_SUPLTRIGGEREDSTART_ThirdPartyInfo(p_PDU_Src, p_SUPL->V2_Data.p_ThirdPartyData);
        }

        if(p_SUPL->V2_Data.p_ApplicationID != NULL)
        {
            supl_PDU_Add_SUPLTRIGGEREDSTART_ApplicationID(p_PDU_Src, p_SUPL->V2_Data.p_ApplicationID);
        }
   }

   return p_PDU_Src;
}

//*****************************************************************************
/// \brief
///      Function to set the unencoded SUPL PDU type to SUPL-REPORT.
///
/// \returns
///      Nothing.
//*****************************************************************************
static void supl_PDU_Add_SUPLREPORT
(
   void *p_ThisPDU   ///< [in/out] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   //  SUPL-REPORT defined in  SUPL-REPORT.asn1
   p_PDU_Cast->message.present = UlpMessage_PR_msSUPLREPORT;
}

//*****************************************************************************
/// \brief
///      Function to set the unencoded Session List to SUPL-REPORT.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLREPORT_SessionList
(
   void *p_ThisPDU,   ///< [in/out] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   s_SUPL_Instance *p_SUPL_Instance      ///< [in] Current instance data used to
)
{
   /* SessionList ::= SEQUENCE SIZE (1..maxnumSessions) OF SessionInformation
   SessionInformation ::= SEQUENCE {
   sessionID SessionID,
   ...}
   */

   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;
   Ver2_SUPLREPORT_t *p_SuplReport = &p_PDU_Cast->message.choice.msSUPLREPORT;
   s_SUPL_InstanceData *p_SUPL = p_SUPL_Instance->p_SUPL;

   SessionInformation_t *p_Session = GN_Calloc( 1,sizeof(SessionInformation_t));

   supl_PDU_Add_setSessionID( &p_Session->sessionID.setSessionID, p_SUPL->SetSessionId,   &p_SUPL->SetId );
   supl_PDU_Add_slpSessionID( &p_Session->sessionID.slpSessionID, p_SUPL->p_SlpSessionId, &p_SUPL->SlpId );

   /* now populate the sessionID an assign it */
   p_SuplReport->sessionList = GN_Calloc( 1 , sizeof(SessionList_t) );

   asn_sequence_add( &p_SuplReport->sessionList->list , p_Session );
}

//*****************************************************************************
/// \brief
///      Function to set the unencoded Session List to SUPL-REPORT.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLREPORT_Position
(
   void *p_ThisPDU,   ///< [in/out] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   s_GN_Position     *p_GN_Position ///< @TODO : Allow for batch reporting. Many positions
)
{
   /*ReportData ::= SEQUENCE {
   positionData PositionData OPTIONAL,
   multipleLocationIds MultipleLocationIds OPTIONAL,
   resultCode ResultCode OPTIONAL,
   timestamp TimeStamp OPTIONAL,
   ...} */

   /*
   PositionData ::= SEQUENCE {
   position Position,
   posMethod PosMethod OPTIONAL,
   gnssPosTechnology GNSSPosTechnology OPTIONAL,
   ganssSignalsInfo GANSSsignalsInfo OPTIONAL,
   ...}
   */

   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;
   Ver2_SUPLREPORT_t *p_SuplReport = &p_PDU_Cast->message.choice.msSUPLREPORT;

   ReportData_t *p_ReportData;

   /* Assign a report data list */
   p_SuplReport->reportDataList = GN_Calloc( 1 , sizeof(ReportDataList_t) );

   /* Assign one report data item */
   p_ReportData = GN_Calloc( 1 , sizeof(ReportData_t) );
   asn_sequence_add( &p_SuplReport->reportDataList->list, p_ReportData );

   /* Assign one position data */
   p_ReportData->positionData = GN_Calloc( 1, sizeof(PositionData_t) );

   /* Assign internal position structure */
   supl_PDU_Add_Position(&p_ReportData->positionData->position , p_GN_Position );

}


//*****************************************************************************
/// \brief
///      High level function to populate an unencoded SUPL-REPORT PDU.
///
/// \returns
///      Pointer to unencoded structure ULP_PDU_t. Returned as (void *) to hide
///      implementation.
//*****************************************************************************
void* supl_PDU_Make_SUPLREPORT
(
   s_SUPL_Instance *p_SUPL_Instance ///< [in] Current instance data used to
                                    ///   populate parts of the PDU.

)
{
   void *p_PDU_Src = NULL;
   s_SUPL_InstanceData    *p_SUPL = p_SUPL_Instance->p_SUPL;
   /*
   Ver2-SUPLREPORT ::= SEQUENCE {
   sessionList SessionList OPTIONAL,
   sETCapabilities SETCapabilities OPTIONAL,
   reportDataList ReportDataList OPTIONAL,
   ver Ver OPTIONAL,
   moreComponents NULL OPTIONAL,
   ...}
   */

   p_PDU_Src = supl_PDU_Alloc();
   GN_SUPL_Log( "SUPL-REPORT Alloc: %08p", p_PDU_Src );

   supl_PDU_Add_Version( p_PDU_Src , p_SUPL_Instance );

   supl_PDU_Add_setSessionID( p_PDU_Src, p_SUPL->SetSessionId,   &p_SUPL->SetId );
   supl_PDU_Add_slpSessionID( p_PDU_Src, p_SUPL->p_SlpSessionId, &p_SUPL->SlpId );

   supl_PDU_Add_SUPLREPORT(p_PDU_Src);

//   supl_PDU_Add_SUPLREPORT_SessionList( p_PDU_Src , p_SUPL_Instance);

   return p_PDU_Src;
}


//*****************************************************************************
/// \brief
///      Function to get a Periodic Parameters from the decoded SUPL PDU type SUPL-TRIGGERED-RESPONSE.
///
/// \returns
///      TRUE if a Periodic Params was present in the PDU.
//*****************************************************************************
BL supl_PDU_Get_SUPLTRIGGEREDRESPONSE_PeriodicParams
(
   void *p_ThisPDU,   ///< [in/out] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   s_GN_SUPL_V2_PeriodicTrigger  *p_TriggerParams
)
{
   BL retVal = FALSE;
   ULP_PDU_t *p_CastPdu = p_ThisPDU;
   Ver2_SUPLTRIGGEREDRESPONSE_t *p_SuplTriggeredRsp = &p_CastPdu->message.choice.msSUPLTRIGGEREDRESPONSE;

   if( p_SuplTriggeredRsp->triggerParams )
   {
      if( p_SuplTriggeredRsp->triggerParams->present == TriggerParams_PR_periodicParams )
      {
         PeriodicParams_t *p_Params = &p_SuplTriggeredRsp->triggerParams->choice.periodicParams;

         p_TriggerParams->IntervalBetweenFixes = p_Params->intervalBetweenFixes;
         p_TriggerParams->NumberOfFixes        = p_Params->numberOfFixes;

         if( p_Params->startTime != NULL )
         {
            p_TriggerParams->StartTime = *p_Params->startTime;
         }
         else
         {
            p_TriggerParams->StartTime = 0XFFFFFFFF;
         }

         retVal = TRUE;
      }
   }

   return retVal;
}

//*****************************************************************************
/// \brief
///      Function to check presence of Area Parameters from the decoded SUPL PDU type SUPL-TRIGGERED-RESPONSE.
///
/// \returns
///      TRUE if a Area Params was present in the PDU.
//*****************************************************************************
BL supl_PDU_Check_SUPLTRIGGEREDRESPONSE_AreaParams( void *p_ThisPDU )
{
   BL retVal = FALSE;
   ULP_PDU_t *p_CastPdu = p_ThisPDU;
   Ver2_SUPLTRIGGEREDRESPONSE_t *p_SuplTriggeredRsp = &p_CastPdu->message.choice.msSUPLTRIGGEREDRESPONSE;

   if( p_SuplTriggeredRsp->triggerParams == NULL )
   {
      retVal = TRUE;
   }
   return retVal;
}

//*****************************************************************************
/// \brief
///      Function to get a Area Parameters from the decoded SUPL PDU type SUPL-TRIGGERED-RESPONSE.
///
/// \returns
///      TRUE if a Area Params was present in the PDU.
//*****************************************************************************
BL supl_PDU_Get_SUPLTRIGGEREDRESPONSE_AreaParams
(
   void *p_ThisPDU,   ///< [in/out] Pointer to unencoded structure
                      ///   ULP_PDU_t. Passed in as (void *) to hide
                      ///   implementation.
   s_GN_SUPL_V2_AreaEventTrigger  *p_AreaEventParams,
   s_SUPL_Instance *p_SUPL_Instance
)
{
   BL retVal = FALSE;
   ULP_PDU_t *p_CastPdu = p_ThisPDU;
   Ver2_SUPLTRIGGEREDRESPONSE_t *p_SuplTriggeredRsp = &p_CastPdu->message.choice.msSUPLTRIGGEREDRESPONSE;

   if( p_SuplTriggeredRsp->triggerParams != NULL )
   {
      if( p_SuplTriggeredRsp->triggerParams->present == TriggerParams_PR_areaEventParams )
      {
         AreaEventParams_t *p_Params = &p_SuplTriggeredRsp->triggerParams->choice.areaEventParams;

         p_AreaEventParams->areaEventType              =  ASN1_GET(ASN1_ENUM_1_GET(p_Params->areaEventType));

         GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams: AreaEventType %i", p_AreaEventParams->areaEventType);
         
         p_AreaEventParams->locationEstimateRequested  = p_Params->locationEstimate;
         GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams: locationEstimate %i", p_AreaEventParams->locationEstimateRequested);


         if(p_SUPL_Instance->p_SUPL->NetworkInitiated)
         {
            if( p_Params->repeatedReportingParams != NULL )
            {
               p_AreaEventParams->repeatedReportingParams.maximumNumberOfReports = p_Params->repeatedReportingParams->maximumNumberOfReports;
               p_AreaEventParams->repeatedReportingParams.minimumIntervalTime    = p_Params->repeatedReportingParams->minimumIntervalTime;
            }
            else
            {
               p_AreaEventParams->repeatedReportingParams.maximumNumberOfReports = 0;
               p_AreaEventParams->repeatedReportingParams.minimumIntervalTime    = 0;
            }

            GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams: maximumNumberOfReports %i", p_AreaEventParams->repeatedReportingParams.maximumNumberOfReports);
            GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams: minimumIntervalTime %i", p_AreaEventParams->repeatedReportingParams.minimumIntervalTime);

            if( p_Params->startTime != NULL )
            {
               p_AreaEventParams->startTime = *p_Params->startTime;
            }
            else
            {
               p_AreaEventParams->startTime = 0XFFFFFFFF;
            }

            GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams: startTime %i", p_AreaEventParams->startTime);

            if( p_Params->stopTime != NULL )
            {
               p_AreaEventParams->stopTime = *p_Params->stopTime;
            }
            else
            {
               p_AreaEventParams->stopTime = 0XFFFFFFFF;
            }

            GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams: stopTime %i", p_AreaEventParams->stopTime); 

         }

         if( p_Params->geographicTargetAreaList != NULL )
         {
            U1 index = 0;

            retVal = TRUE;

            p_AreaEventParams->geographicTargetAreaCount = p_Params->geographicTargetAreaList->list.count;

            GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams: TargetAreaCount %i", p_AreaEventParams->geographicTargetAreaCount);

            p_AreaEventParams->geographicTargetAreaParams
                     = GN_Calloc( 1, (sizeof(*p_AreaEventParams->geographicTargetAreaParams)*(p_AreaEventParams->geographicTargetAreaCount)));

            for( index = 0 ; (index < p_AreaEventParams->geographicTargetAreaCount) && retVal ; index++ )
            {
               GeographicTargetArea_t* targetArea = p_Params->geographicTargetAreaList->list.array[index];

               GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams: TargetArea %i", targetArea->present);

               switch( targetArea->present )
               {
                  case GeographicTargetArea_PR_circularArea:
                     p_AreaEventParams->geographicTargetAreaParams[index].shapeType = GN_GeographicalTargetArea_Circlar;

                     p_AreaEventParams->geographicTargetAreaParams[index].choice.circularArea.center.latitude     = targetArea->choice.circularArea.coordinate.latitude;
                     p_AreaEventParams->geographicTargetAreaParams[index].choice.circularArea.center.longitude    = targetArea->choice.circularArea.coordinate.longitude;
                     p_AreaEventParams->geographicTargetAreaParams[index].choice.circularArea.center.latitudeSign = ASN1_GET(ASN1_ENUM_1_GET(targetArea->choice.circularArea.coordinate.latitudeSign));

                     p_AreaEventParams->geographicTargetAreaParams[index].choice.circularArea.radius    = targetArea->choice.circularArea.radius;

                     if( targetArea->choice.circularArea.radius_min != NULL )
                     {
                        p_AreaEventParams->geographicTargetAreaParams[index].choice.circularArea.radiusMin = *targetArea->choice.circularArea.radius_min;
                     }
                     else
                     {
                        p_AreaEventParams->geographicTargetAreaParams[index].choice.circularArea.radiusMin = 0;
                     }

                     if( targetArea->choice.circularArea.radius_max != NULL )
                     {
                        p_AreaEventParams->geographicTargetAreaParams[index].choice.circularArea.radiusMax = *targetArea->choice.circularArea.radius_max;
                     }
                     else
                     {
                        p_AreaEventParams->geographicTargetAreaParams[index].choice.circularArea.radiusMax = 0;
                     }
                  break;

                  case GeographicTargetArea_PR_ellipticalArea:
                     p_AreaEventParams->geographicTargetAreaParams[index].shapeType = GN_GeographicalTargetArea_Ellipse;

                     p_AreaEventParams->geographicTargetAreaParams[index].choice.ellipticalArea.center.latitude     = targetArea->choice.ellipticalArea.coordinate.latitude;
                     p_AreaEventParams->geographicTargetAreaParams[index].choice.ellipticalArea.center.longitude    = targetArea->choice.ellipticalArea.coordinate.longitude;
                     p_AreaEventParams->geographicTargetAreaParams[index].choice.ellipticalArea.center.latitudeSign = ASN1_GET(ASN1_ENUM_1_GET(targetArea->choice.ellipticalArea.coordinate.latitudeSign));

                     p_AreaEventParams->geographicTargetAreaParams[index].choice.ellipticalArea.semiMajor      = targetArea->choice.ellipticalArea.semiMajor;
                     p_AreaEventParams->geographicTargetAreaParams[index].choice.ellipticalArea.semiMinor      = targetArea->choice.ellipticalArea.semiMinor;
                     p_AreaEventParams->geographicTargetAreaParams[index].choice.ellipticalArea.semiMinorAngle = targetArea->choice.ellipticalArea.angle;

                     if( targetArea->choice.ellipticalArea.semiMajor_min != NULL )
                     {
                        p_AreaEventParams->geographicTargetAreaParams[index].choice.ellipticalArea.semiMajorMin = *targetArea->choice.ellipticalArea.semiMajor_min;
                     }
                     else
                     {
                        p_AreaEventParams->geographicTargetAreaParams[index].choice.ellipticalArea.semiMajorMin = 0;
                     }

                     if( targetArea->choice.ellipticalArea.semiMajor_max != NULL )
                     {
                        p_AreaEventParams->geographicTargetAreaParams[index].choice.ellipticalArea.semiMajorMax = *targetArea->choice.ellipticalArea.semiMajor_max;
                     }
                     else
                     {
                        p_AreaEventParams->geographicTargetAreaParams[index].choice.ellipticalArea.semiMajorMax = 0;
                     }

                     if( targetArea->choice.ellipticalArea.semiMinor_max != NULL )
                     {
                        p_AreaEventParams->geographicTargetAreaParams[index].choice.ellipticalArea.semiMinorMax = *targetArea->choice.ellipticalArea.semiMinor_max;
                     }
                     else
                     {
                        p_AreaEventParams->geographicTargetAreaParams[index].choice.ellipticalArea.semiMinorMax = 0;
                     }

                     if( targetArea->choice.ellipticalArea.semiMinor_min != NULL )
                     {
                        p_AreaEventParams->geographicTargetAreaParams[index].choice.ellipticalArea.semiMinorMin = *targetArea->choice.ellipticalArea.semiMinor_min;
                     }
                     else
                     {
                        p_AreaEventParams->geographicTargetAreaParams[index].choice.ellipticalArea.semiMinorMin = 0;
                     }
                  break;

                  case GeographicTargetArea_PR_polygonArea:
                  {
                     U1 pointIndex = 0;

                     p_AreaEventParams->geographicTargetAreaParams[index].shapeType = GN_GeographicalTargetArea_Polygon;


                     if( targetArea->choice.polygonArea.polygonHysteresis != NULL)
                     {
                        p_AreaEventParams->geographicTargetAreaParams[index].choice.polygonArea.polygonHysteresis = *targetArea->choice.polygonArea.polygonHysteresis;
                     }
                     else
                     {
                        p_AreaEventParams->geographicTargetAreaParams[index].choice.polygonArea.polygonHysteresis = 0;
                     }

                     p_AreaEventParams->geographicTargetAreaParams[index].choice.polygonArea.numOfPoints = targetArea->choice.polygonArea.polygonDescription.list.count;
                     p_AreaEventParams->geographicTargetAreaParams[index].choice.polygonArea.coordinates =
                              GN_Calloc(1, (sizeof(*p_AreaEventParams->geographicTargetAreaParams[index].choice.polygonArea.coordinates)
                                            *(p_AreaEventParams->geographicTargetAreaParams[index].choice.polygonArea.numOfPoints)));

                     for( pointIndex = 0; pointIndex < p_AreaEventParams->geographicTargetAreaParams[index].choice.polygonArea.numOfPoints ; pointIndex++ )
                     {
                        Coordinate_t* coordinates = targetArea->choice.polygonArea.polygonDescription.list.array[pointIndex];

                        p_AreaEventParams->geographicTargetAreaParams[index].choice.polygonArea.coordinates[pointIndex].latitude     = coordinates->latitude;
                        p_AreaEventParams->geographicTargetAreaParams[index].choice.polygonArea.coordinates[pointIndex].latitudeSign = ASN1_GET(ASN1_ENUM_1_GET(coordinates->latitudeSign));
                        p_AreaEventParams->geographicTargetAreaParams[index].choice.polygonArea.coordinates[pointIndex].longitude    = coordinates->longitude;
                     }
                  }
                  break;

                  default:
                     GN_SUPL_Log( "ERR: Area Event Type not set");
                     GN_Free(p_AreaEventParams->geographicTargetAreaParams);
                     retVal = FALSE;
                  break;
               }
            }
         }

         if( p_Params->areaIdLists != NULL )
         {
            U1 Index = 0;
            U1 NoOfAreaId = 0;      
            U1 ValidNoOfAreaId ;

            retVal = TRUE;
            
            p_AreaEventParams->AreaIdListsCount = p_Params->areaIdLists->list.count;
            
            GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams: Area Id List count %i", p_AreaEventParams->AreaIdListsCount);
             
            p_AreaEventParams->AreaIdListsParams = GN_Calloc( 1, (sizeof(*p_AreaEventParams->AreaIdListsParams)*(p_AreaEventParams->AreaIdListsCount)));
                
            for( Index = 0; (Index < p_AreaEventParams->AreaIdListsCount) && retVal ; Index++ )
            {
                ValidNoOfAreaId = 0; // For every list number of area Ids need to start with zero.

                AreaIdList_t* AreaIdlist = p_Params->areaIdLists->list.array[Index];

                p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms = GN_Calloc( 1, (sizeof(*p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms)*(AreaIdlist->areaIdSet.list.count)));

                for( NoOfAreaId = 0; (NoOfAreaId < AreaIdlist->areaIdSet.list.count) && retVal ; NoOfAreaId++ )
                {
                  AreaId_t* AreaIds = AreaIdlist->areaIdSet.list.array[NoOfAreaId];

                  GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams: Type of Area Id present %i", AreaIds->present);

                  switch( AreaIds->present )
                  {
                     case AreaId_PR_gSMAreaId:

                        p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].AreaIdType  = GN_AreaId_gSM ;

                        if( AreaIds->choice.gSMAreaId.refCI != NULL )
                        {
                            p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.gsmAreaIdInfo.refCI  = *AreaIds->choice.gSMAreaId.refCI;

                            if((AreaIds->choice.gSMAreaId.refMCC == NULL)||(AreaIds->choice.gSMAreaId.refMNC == NULL)||(AreaIds->choice.gSMAreaId.refLAC == NULL))
                            {
                               if(p_SUPL_Instance->p_SUPL->LocationId.Type == CIT_gsmCell)
                               {
                                   if(AreaIds->choice.gSMAreaId.refMCC == NULL)
                                     p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.gsmAreaIdInfo.refMCC 
                                                                         = p_SUPL_Instance->p_SUPL->LocationId.of_type.gsmCellInfo.refMCC;
                                   else
                                     p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.gsmAreaIdInfo.refMCC 
                                                                         = *AreaIds->choice.gSMAreaId.refMCC;

                                   if(AreaIds->choice.gSMAreaId.refMNC == NULL)
                                     p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.gsmAreaIdInfo.refMNC 
                                                                         = p_SUPL_Instance->p_SUPL->LocationId.of_type.gsmCellInfo.refMNC;
                                   else
                                     p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.gsmAreaIdInfo.refMNC 
                                                                         = *AreaIds->choice.gSMAreaId.refMNC;
                                   
                                   if(AreaIds->choice.gSMAreaId.refLAC == NULL)
                                     p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.gsmAreaIdInfo.refLAC 
                                                                         = p_SUPL_Instance->p_SUPL->LocationId.of_type.gsmCellInfo.refLAC;
                                   else
                                     p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.gsmAreaIdInfo.refLAC 
                                                                         = *AreaIds->choice.gSMAreaId.refLAC;

                                   GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams:gSMAreaId refMCC %i", p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.gsmAreaIdInfo.refMCC);
                                   GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams:gSMAreaId refMNC %i", p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.gsmAreaIdInfo.refMNC);
                                   GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams:gSMAreaId refLAC %i", p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.gsmAreaIdInfo.refLAC);
                                   GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams:gSMAreaId refCI  %i", p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.gsmAreaIdInfo.refCI);

                                   ValidNoOfAreaId++ ;
                                   
                               }
                               else
                               {
                                   GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams:CellInfoType not Present");
                               }

                            }
                            else
                            {
                               p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.gsmAreaIdInfo.refMCC = *AreaIds->choice.gSMAreaId.refMCC ;
                               p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.gsmAreaIdInfo.refMNC = *AreaIds->choice.gSMAreaId.refMNC;
                               p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.gsmAreaIdInfo.refLAC = *AreaIds->choice.gSMAreaId.refLAC;

                               GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams:gSMAreaId refMCC %i", p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.gsmAreaIdInfo.refMCC);
                               GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams:gSMAreaId refMNC %i", p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.gsmAreaIdInfo.refMNC);
                               GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams:gSMAreaId refLAC %i", p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.gsmAreaIdInfo.refLAC);
                               GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams:gSMAreaId refCI  %i", p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.gsmAreaIdInfo.refCI);

                               ValidNoOfAreaId++ ;
                            }


                        }
                        else
                        {
                            // Cell Identity assumed to be a mandatory parameter.
                            GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams:Cell Identity not Present");
                        }
                     break;

                     case  AreaId_PR_wCDMAAreaId:

                        p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].AreaIdType  = GN_AreaId_wCDMA ;

                        if( AreaIds->choice.wCDMAAreaId.refUC != NULL )
                        {
                            p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.wcdmaAreaIdInfo.refUC  = *AreaIds->choice.wCDMAAreaId.refUC;

                            if((AreaIds->choice.wCDMAAreaId.refMCC == NULL) || (AreaIds->choice.wCDMAAreaId.refMNC == NULL) || (AreaIds->choice.wCDMAAreaId.refLAC == NULL))
                            {
                               if(p_SUPL_Instance->p_SUPL->LocationId.Type == CIT_wcdmaCell)
                               {
                                   if(AreaIds->choice.wCDMAAreaId.refMCC == NULL)
                                     p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.wcdmaAreaIdInfo.refMCC
                                                                         = p_SUPL_Instance->p_SUPL->LocationId.of_type.wcdmaCellInfo.refMCC;
                                   else
                                     p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.wcdmaAreaIdInfo.refMCC
                                                                         = *AreaIds->choice.wCDMAAreaId.refMCC;

                                   if(AreaIds->choice.wCDMAAreaId.refMNC == NULL)
                                     p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.wcdmaAreaIdInfo.refMNC
                                                                         = p_SUPL_Instance->p_SUPL->LocationId.of_type.wcdmaCellInfo.refMNC;
                                   else
                                     p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.wcdmaAreaIdInfo.refMNC
                                                                         = *AreaIds->choice.wCDMAAreaId.refMNC;
                                   
                                   GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams:wCDMAAreaId refMCC %i", p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.wcdmaAreaIdInfo.refMCC);
                                   GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams:wCDMAAreaId refMNC %i", p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.wcdmaAreaIdInfo.refMNC);
                                   GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams:wCDMAAreaId refUC  %i", p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.wcdmaAreaIdInfo.refUC);
                                                        
                                   ValidNoOfAreaId++ ;
                               }
                               else
                               {
                                   GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams:CellInfoType not Present");
                               }

                            }
                            else
                            {
                               p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.wcdmaAreaIdInfo.refMCC = *AreaIds->choice.wCDMAAreaId.refMCC ;
                               p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.wcdmaAreaIdInfo.refMNC = *AreaIds->choice.wCDMAAreaId.refMNC;
                               p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.wcdmaAreaIdInfo.refLAC = *AreaIds->choice.wCDMAAreaId.refLAC;
                               
                               GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams:wCDMAAreaId refMCC %i", p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.wcdmaAreaIdInfo.refMCC);
                               GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams:wCDMAAreaId refMNC %i", p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.wcdmaAreaIdInfo.refMNC);
                               GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams:wCDMAAreaId refLAC %i", p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.wcdmaAreaIdInfo.refLAC);
                               GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams:wCDMAAreaId refUC  %i", p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.wcdmaAreaIdInfo.refUC);

                               ValidNoOfAreaId++ ;
                            }
                        }
                        else
                        {
                            GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams:Cell Identity not Present");
                            // Cell Identity assumed to be a mandatory parameter.
                        }                    
                     break;
             
                     case AreaId_PR_cDMAAreaId:
                        
                        p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].AreaIdType  = GN_AreaId_cDMA ;
                        
                        if( AreaIds->choice.cDMAAreaId.refBASEID != NULL )
                        {
                            p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.cdmaAreaIdInfo.refBASEID = *AreaIds->choice.cDMAAreaId.refBASEID;

                            if((AreaIds->choice.cDMAAreaId.refSID == NULL) || (AreaIds->choice.cDMAAreaId.refNID == NULL ))
                            {
                               if(p_SUPL_Instance->p_SUPL->LocationId.Type == CIT_cdmaCell)
                               {
                                   if(AreaIds->choice.cDMAAreaId.refSID == NULL)
                                     p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.cdmaAreaIdInfo.refSID 
                                                                         = p_SUPL_Instance->p_SUPL->LocationId.of_type.cdmaCellInfo.refSID;
                                   else
                                     p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.cdmaAreaIdInfo.refSID 
                                                                         = *AreaIds->choice.cDMAAreaId.refSID;
                                   
                                   if(AreaIds->choice.cDMAAreaId.refNID == NULL)
                                     p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.cdmaAreaIdInfo.refNID 
                                                                         = p_SUPL_Instance->p_SUPL->LocationId.of_type.cdmaCellInfo.refNID;
                                   else
                                     p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.cdmaAreaIdInfo.refNID 
                                                                         = *AreaIds->choice.cDMAAreaId.refNID;
                                   
                                   GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams:cdmaAreaIdInfo refSID    %i", p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.cdmaAreaIdInfo.refSID);
                                   GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams:cdmaAreaIdInfo refNID    %i", p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.cdmaAreaIdInfo.refNID);
                                   GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams:cdmaAreaIdInfo refBASEID %i", p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.cdmaAreaIdInfo.refBASEID);
                                   
                                   ValidNoOfAreaId++ ;
                               }
                               else
                               {
                                   GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams:CellInfoType not Present");
                               }

                            }
                            else
                            {
                               p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.cdmaAreaIdInfo.refSID     = *AreaIds->choice.cDMAAreaId.refSID ;
                               p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.cdmaAreaIdInfo.refNID     = *AreaIds->choice.cDMAAreaId.refNID;

                               GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams:cdmaAreaIdInfo refSID    %i", p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.cdmaAreaIdInfo.refSID);
                               GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams:cdmaAreaIdInfo refNID    %i", p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.cdmaAreaIdInfo.refNID);
                               GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams:cdmaAreaIdInfo refBASEID %i", p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms[ValidNoOfAreaId].choice.cdmaAreaIdInfo.refBASEID);

                               ValidNoOfAreaId++ ;
                            }                   
                            
                        }
                        else
                        {
                            GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams:Base Station Id not Present");
                            // Base Station Id assumed to be a mandatory parameter.
                        }   
                   
                     break;
             
                     default:
                        GN_SUPL_Log( "ERR: Area Id Type not set");                      
                        GN_Free(p_AreaEventParams->AreaIdListsParams[Index].s_AreaIdSetParms);
                        GN_Free(p_AreaEventParams->AreaIdListsParams);
                        retVal = FALSE;
                     break;
                  }
                }
                if(ValidNoOfAreaId && retVal)
                {
                  p_AreaEventParams->AreaIdListsParams[Index].NumAreaId = ValidNoOfAreaId ;
                  GN_SUPL_Log( "DBG SUPLTRIGGEREDRESPONSE_AreaParams: NumAreaId count %i", p_AreaEventParams->AreaIdListsParams[Index].NumAreaId);
                }
                else
                {
                  retVal = FALSE;
                }
           }

        }
      }
    }
   return retVal;
}


//*****************************************************************************
/// \brief
///      Function to check the emergency call is requested by the supl init or not
///
/// \returns
///      Void
//*****************************************************************************
void supl_PDU_Get_SUPLINIT_EmergencyRequest
(
   void *p_ThisPDU,  ///< [in] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   BL *p_EmeregencyReq    ///< [OUT] TRUE or False if emeregncy call is requested or not
)
{
    ULP_PDU_t *p_CastPdu = p_ThisPDU;
    SUPLINIT_t *p_SuplInit = &p_CastPdu->message.choice.msSUPLINIT;

    if( p_SuplInit->notification != NULL )
    {
        if( p_SuplInit->notification->ver2_Notification_extension != NULL )
        {
           if(p_SuplInit->notification->ver2_Notification_extension->emergencyCallLocation != NULL)
               *p_EmeregencyReq = TRUE;
           else
               *p_EmeregencyReq = FALSE;
        }
    }

}

//*****************************************************************************
/// \brief
///      Function to check the Eslp address present or not if there store into supl instance data
///
/// \returns
///      Void
//*****************************************************************************
BL supl_PDU_Get_SUPLINIT_ESlp_Address
(
   void *p_ThisPDU,  ///< [in] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   s_GN_SUPL_V2_InstanceData *p_SUPL_V2_InstanceData    ///< [OUT] E-SLP Address
)
{
    BL Result = FALSE;
    ULP_PDU_t *p_CastPdu = p_ThisPDU;
    SUPLINIT_t *p_SuplInit = &p_CastPdu->message.choice.msSUPLINIT;
    p_SUPL_V2_InstanceData->Eslp_Address_Present = FALSE;

    if( p_SuplInit->ver2_SUPL_INIT_extension != NULL )
    {
        if( p_SuplInit->ver2_SUPL_INIT_extension->e_SLPAddress != NULL )
        {
           switch(p_SuplInit->ver2_SUPL_INIT_extension->e_SLPAddress->present)
               {

            case SLPAddress_PR_iPAddress:
                GN_SUPL_Log( "SUPL-ESLP ADDRESS of wrong Type");
                break;
            case SLPAddress_PR_fQDN:
                strcpy(p_SUPL_V2_InstanceData->Eslp_FQDN,
                (char *)p_SuplInit->ver2_SUPL_INIT_extension->e_SLPAddress->choice.fQDN.buf );

                p_SUPL_V2_InstanceData->Eslp_Address_Present = TRUE;
                Result= TRUE;
                break;
            default:
                GN_SUPL_Log( "SUPL-ESLP ADDRESS is not a valid Type");
                break;

               }
        }
    }
    return Result;

}


void supl_get_Position(
    Position_t *p_Position,
    s_GN_Position      *p_GN_Position,
    s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data   ///<[out] Position Estimate.
)
{
    U4 Temp;

    memset( p_GN_AGPS_GAD_Data, 0, sizeof( s_GN_AGPS_GAD_Data ) );
    memset( p_GN_Position,      0, sizeof( s_GN_Position       ) );

    p_GN_Position->OS_Time_ms = GN_GPS_Get_OS_Time_ms();

    // Set values to not known where sensible.
    p_GN_AGPS_GAD_Data->altitudeDirection       =  -1; //=Not present]

    p_GN_AGPS_GAD_Data->uncertaintySemiMajor  = 255; //=Unknown]
    p_GN_AGPS_GAD_Data->uncertaintySemiMinor  = 255; //=Unknown]
    p_GN_AGPS_GAD_Data->orientationMajorAxis  = 255; //=Unknown]
    p_GN_AGPS_GAD_Data->uncertaintyAltitude   = 255; //=Unknown]
    p_GN_AGPS_GAD_Data->confidence            = 255; //=Unknown]

    p_GN_AGPS_GAD_Data->verdirect               =  -1; //=Not present]
    p_GN_AGPS_GAD_Data->bearing               =  -1; //=Not present]
    p_GN_AGPS_GAD_Data->horspeed               =  -1; //=Not present]
    p_GN_AGPS_GAD_Data->verspeed               =  -1; //=Not present]
    p_GN_AGPS_GAD_Data->horuncertspeed        = 255; //=Unknown]
    p_GN_AGPS_GAD_Data->veruncertspeed        = 255; //=Unknown]

    p_GN_Position->Valid_2D_Fix = TRUE;

    strcpy( p_GN_Position->timestamp,(char *) &p_Position->timestamp.buf[1] ); // Exclude prepended 'V' from SDL_String.

    p_GN_AGPS_GAD_Data->latitude      = p_Position->positionEstimate.latitude;
    p_GN_AGPS_GAD_Data->latitudeSign = *(p_Position->positionEstimate.latitudeSign.buf);
    // ASN1_ENUM_SET(p_GN_AGPS_GAD_Data->latitudeSign,p_Position->positionEstimate.latitudeSign);
    p_GN_AGPS_GAD_Data->longitude      = p_Position->positionEstimate.longitude;

    p_GN_Position->latitude          = p_Position->positionEstimate.latitude;
    //ASN1_ENUM_SET(p_GN_Position->latitudeSign,p_Position->positionEstimate.latitudeSign);
    p_GN_Position->latitudeSign      = *(p_Position->positionEstimate.latitudeSign.buf);
    p_GN_Position->longitude          = p_Position->positionEstimate.longitude;

    if (p_Position->positionEstimate.uncertainty != NULL )
    {
        p_GN_AGPS_GAD_Data->uncertaintySemiMajor = (U1)p_Position->positionEstimate.uncertainty->uncertaintySemiMajor;
        p_GN_AGPS_GAD_Data->uncertaintySemiMinor = (U1)p_Position->positionEstimate.uncertainty->uncertaintySemiMinor;
        p_GN_AGPS_GAD_Data->orientationMajorAxis = (U1)p_Position->positionEstimate.uncertainty->orientationMajorAxis;

        p_GN_Position->uncertaintyPresent = TRUE;
        p_GN_Position->uncertaintySemiMajor = (U1)p_Position->positionEstimate.uncertainty->uncertaintySemiMajor;;
        p_GN_Position->uncertaintySemiMinor = (U1)p_Position->positionEstimate.uncertainty->uncertaintySemiMinor;
        p_GN_Position->orientationMajorAxis = (U1)p_Position->positionEstimate.uncertainty->orientationMajorAxis;
    }

    if ( p_Position->positionEstimate.altitudeInfo != NULL )
    {
        p_GN_AGPS_GAD_Data->altitude              = (U2)p_Position->positionEstimate.altitudeInfo->altitude;
        p_GN_AGPS_GAD_Data->altitudeDirection      = *(p_Position->positionEstimate.altitudeInfo->altitudeDirection.buf);
        p_GN_AGPS_GAD_Data->uncertaintyAltitude   = (U1)p_Position->positionEstimate.altitudeInfo->altUncertainty;

        p_GN_Position->altitudeInfoPresent    = TRUE;

        p_GN_Position->Valid_3D_Fix         = TRUE;
        p_GN_Position->altitude             = (U2)p_Position->positionEstimate.altitudeInfo->altitude;
        p_GN_Position->altitudeDirection    = *(p_Position->positionEstimate.altitudeInfo->altitudeDirection.buf);
        p_GN_Position->altUncertainty        = (U1)p_Position->positionEstimate.altitudeInfo->altUncertainty;
    }

    if ( p_Position->positionEstimate.confidence != NULL )
    {
        p_GN_AGPS_GAD_Data->confidence     = (U1)*p_Position->positionEstimate.confidence;

        p_GN_Position->confidencePresent = TRUE;
        p_GN_Position->confidence         = (U1)*p_Position->positionEstimate.confidence;
    }

    if ( p_Position->velocity != NULL)
    {
        p_GN_Position->velocityPresent = TRUE;

        switch ( p_Position->velocity->present)
        {
            case Velocity_PR_horandveruncert:
               p_GN_Position->VelocityIndicator = GN_horandveruncert;

               if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horandveruncert.bearing, &Temp ) )
               {
                  p_GN_AGPS_GAD_Data->bearing = Temp;
                  p_GN_Position->bearing = Temp;
               }

               if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horandveruncert.horspeed, &Temp ) )
               {
                  p_GN_AGPS_GAD_Data->horspeed = Temp;
                  p_GN_Position->horspeed = Temp;
               }

               if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horandveruncert.horuncertspeed, &Temp ) )
               {
                  p_GN_AGPS_GAD_Data->horuncertspeed = Temp;
                  p_GN_Position->horuncertspeed = Temp;
               }

               if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horandveruncert.verdirect, &Temp ) )
               {
                  p_GN_AGPS_GAD_Data->verdirect = Temp;
                  p_GN_Position->verdirect = Temp;
               }

               if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horandveruncert.verspeed, &Temp ) )
               {
                  p_GN_AGPS_GAD_Data->verspeed = Temp;
                  p_GN_Position->verspeed = Temp;
               }

               if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horandveruncert.veruncertspeed, &Temp ) )
               {
                  p_GN_AGPS_GAD_Data->veruncertspeed = Temp;
                  p_GN_Position->veruncertspeed = Temp;
               }

               break;
            case Velocity_PR_horandvervel:
               p_GN_Position->VelocityIndicator = GN_horandvervel;

               if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horandvervel.horspeed, &Temp ) )
               {
                  p_GN_AGPS_GAD_Data->horspeed = Temp;
                  p_GN_Position->horspeed = Temp;
               }

               if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horandvervel.bearing, &Temp ) )
               {
                  p_GN_AGPS_GAD_Data->bearing = Temp;
                  p_GN_Position->bearing = Temp;
               }

               if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horandvervel.verdirect, &Temp ) )
               {
                  p_GN_AGPS_GAD_Data->verdirect = Temp;
                  p_GN_Position->verdirect = Temp;
               }

               if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horandvervel.verspeed, &Temp ) )
               {
                  p_GN_AGPS_GAD_Data->verspeed = Temp;
                  p_GN_Position->verspeed = Temp;
               }

               break;
            case Velocity_PR_horvel:
               p_GN_Position->VelocityIndicator = GN_horvel;

               if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horvel.horspeed, &Temp ) )
               {
                  p_GN_AGPS_GAD_Data->horspeed = Temp;
                  p_GN_Position->horspeed = Temp;
               }

               if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horvel.bearing, &Temp ) )
               {
                  p_GN_AGPS_GAD_Data->bearing = Temp;
                  p_GN_Position->bearing = Temp;
               }

               break;
            case Velocity_PR_horveluncert:
               p_GN_Position->VelocityIndicator = GN_horveluncert;

               if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horveluncert.horspeed, &Temp ) )
               {
                  p_GN_AGPS_GAD_Data->horspeed = Temp;
                  p_GN_Position->horspeed = Temp;
               }

               if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horveluncert.bearing, &Temp ) )
               {
                  p_GN_AGPS_GAD_Data->bearing = Temp;
                  p_GN_Position->bearing = Temp;
               }

               if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horveluncert.uncertspeed, &Temp ) )
               {
                  p_GN_AGPS_GAD_Data->horuncertspeed = Temp;
                  p_GN_Position->horuncertspeed = Temp;
               }

               break;
            default:
                // Velocity_PR_NOTHING - not handled
               break;
        }
    }

}


//*****************************************************************************
/// \brief
///      Function to get a Position Estimate from the decoded SUPL PDU type SUPL-REPORT.
///
/// \returns
///      Nothing.
//*****************************************************************************
BL supl_PDU_Get_SUPLREPORT_GAD_Position
(
   void *p_ThisPDU,  ///< [in] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   s_GN_Position      *p_GN_Position,
   s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data   ///<[out] Position Estimate.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;
   Position_t *p_Position = NULL;
   BL Status = FALSE;


   if ( p_PDU_Cast->message.choice.msSUPLREPORT.reportDataList != NULL )
   {
       if( p_PDU_Cast->message.choice.msSUPLREPORT.reportDataList->list.count )
       {
           U4 Temp;

           /* Handling only 1 report data count. TODO */
           p_Position = &(p_PDU_Cast->message.choice.msSUPLREPORT.reportDataList->list.array[0]->positionData->position);

           memset( p_GN_AGPS_GAD_Data, 0, sizeof( s_GN_AGPS_GAD_Data ) );
           memset( p_GN_Position,      0, sizeof( s_GN_Position      ) );

           p_GN_Position->OS_Time_ms = GN_GPS_Get_OS_Time_ms();

           // Set values to not known where sensible.
           p_GN_AGPS_GAD_Data->altitudeDirection     =  -1; //=Not present]

           p_GN_AGPS_GAD_Data->uncertaintySemiMajor  = 255; //=Unknown]
           p_GN_AGPS_GAD_Data->uncertaintySemiMinor  = 255; //=Unknown]
           p_GN_AGPS_GAD_Data->orientationMajorAxis  = 255; //=Unknown]
           p_GN_AGPS_GAD_Data->uncertaintyAltitude   = 255; //=Unknown]
           p_GN_AGPS_GAD_Data->confidence            = 255; //=Unknown]

           p_GN_AGPS_GAD_Data->verdirect             =  -1; //=Not present]
           p_GN_AGPS_GAD_Data->bearing               =  -1; //=Not present]
           p_GN_AGPS_GAD_Data->horspeed              =  -1; //=Not present]
           p_GN_AGPS_GAD_Data->verspeed              =  -1; //=Not present]
           p_GN_AGPS_GAD_Data->horuncertspeed        = 255; //=Unknown]
           p_GN_AGPS_GAD_Data->veruncertspeed        = 255; //=Unknown]

           p_GN_Position->Valid_2D_Fix = TRUE;

           strcpy( p_GN_Position->timestamp, (CH*)&p_Position->timestamp.buf[0] ); // Exclude prepended 'V' from SDL_String.
           

           p_GN_AGPS_GAD_Data->latitude     = p_Position->positionEstimate.latitude;
           p_GN_AGPS_GAD_Data->latitudeSign = *(p_Position->positionEstimate.latitudeSign.buf);
          // ASN1_ENUM_SET(p_GN_AGPS_GAD_Data->latitudeSign,p_Position->positionEstimate.latitudeSign);
           p_GN_AGPS_GAD_Data->longitude    = p_Position->positionEstimate.longitude;

           p_GN_Position->latitude          = p_Position->positionEstimate.latitude;
           //ASN1_ENUM_SET(p_GN_Position->latitudeSign,p_Position->positionEstimate.latitudeSign);
           p_GN_Position->latitudeSign      = *(p_Position->positionEstimate.latitudeSign.buf);
           p_GN_Position->longitude         = p_Position->positionEstimate.longitude;

           if (p_Position->positionEstimate.uncertainty != NULL )
           {
           
              p_GN_AGPS_GAD_Data->uncertaintySemiMajor = (U1)p_Position->positionEstimate.uncertainty->uncertaintySemiMajor;
              p_GN_AGPS_GAD_Data->uncertaintySemiMinor = (U1)p_Position->positionEstimate.uncertainty->uncertaintySemiMinor;
              p_GN_AGPS_GAD_Data->orientationMajorAxis = (U1)p_Position->positionEstimate.uncertainty->orientationMajorAxis;

              p_GN_Position->uncertaintyPresent = TRUE;
              p_GN_Position->uncertaintySemiMajor = (U1)p_Position->positionEstimate.uncertainty->uncertaintySemiMajor;;
              p_GN_Position->uncertaintySemiMinor = (U1)p_Position->positionEstimate.uncertainty->uncertaintySemiMinor;
              p_GN_Position->orientationMajorAxis = (U1)p_Position->positionEstimate.uncertainty->orientationMajorAxis;
           }

           if ( p_Position->positionEstimate.altitudeInfo != NULL )
           {
              p_GN_AGPS_GAD_Data->altitude              = (U2)p_Position->positionEstimate.altitudeInfo->altitude;
              p_GN_AGPS_GAD_Data->altitudeDirection     = *(p_Position->positionEstimate.altitudeInfo->altitudeDirection.buf);
              p_GN_AGPS_GAD_Data->uncertaintyAltitude   = (U1)p_Position->positionEstimate.altitudeInfo->altUncertainty;

              p_GN_Position->altitudeInfoPresent  = TRUE;

              p_GN_Position->Valid_3D_Fix         = TRUE;
              p_GN_Position->altitude             = (U2)p_Position->positionEstimate.altitudeInfo->altitude;
              p_GN_Position->altitudeDirection    = *(p_Position->positionEstimate.altitudeInfo->altitudeDirection.buf);
              p_GN_Position->altUncertainty       = (U1)p_Position->positionEstimate.altitudeInfo->altUncertainty;
           }

           if ( p_Position->positionEstimate.confidence != NULL )
           {
              p_GN_AGPS_GAD_Data->confidence   = (U1)*p_Position->positionEstimate.confidence;

              p_GN_Position->confidencePresent = TRUE;
              p_GN_Position->confidence        = (U1)*p_Position->positionEstimate.confidence;
           }


           if ( p_Position->velocity != NULL)
           {
              p_GN_Position->velocityPresent = TRUE;
              
              switch ( p_Position->velocity->present)
              {
              case Velocity_PR_horandveruncert:
                 p_GN_Position->VelocityIndicator = GN_horandveruncert;

                 if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horandveruncert.bearing, &Temp ) )
                 {
                    p_GN_AGPS_GAD_Data->bearing = Temp;
                    p_GN_Position->bearing = Temp;
                 }

                 if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horandveruncert.horspeed, &Temp ) )
                 {
                    p_GN_AGPS_GAD_Data->horspeed = Temp;
                    p_GN_Position->horspeed = Temp;
                 }

                 if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horandveruncert.horuncertspeed, &Temp ) )
                 {
                    p_GN_AGPS_GAD_Data->horuncertspeed = Temp;
                    p_GN_Position->horuncertspeed = Temp;
                 }

                 if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horandveruncert.verdirect, &Temp ) )
                 {
                    p_GN_AGPS_GAD_Data->verdirect = Temp;
                    p_GN_Position->verdirect = Temp;
                 }

                 if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horandveruncert.verspeed, &Temp ) )
                 {
                    p_GN_AGPS_GAD_Data->verspeed = Temp;
                    p_GN_Position->verspeed = Temp;
                 }

                 if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horandveruncert.veruncertspeed, &Temp ) )
                 {
                    p_GN_AGPS_GAD_Data->veruncertspeed = Temp;
                    p_GN_Position->veruncertspeed = Temp;
                 }

                 break;
              case Velocity_PR_horandvervel:
                 p_GN_Position->VelocityIndicator = GN_horandvervel;

                 if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horandvervel.horspeed, &Temp ) )
                 {
                    p_GN_AGPS_GAD_Data->horspeed = Temp;
                    p_GN_Position->horspeed = Temp;
                 }

                 if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horandvervel.bearing, &Temp ) )
                 {
                    p_GN_AGPS_GAD_Data->bearing = Temp;
                    p_GN_Position->bearing = Temp;
                 }

                 if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horandvervel.verdirect, &Temp ) )
                 {
                    p_GN_AGPS_GAD_Data->verdirect = Temp;
                    p_GN_Position->verdirect = Temp;
                 }

                 if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horandvervel.verspeed, &Temp ) )
                 {
                    p_GN_AGPS_GAD_Data->verspeed = Temp;
                    p_GN_Position->verspeed = Temp;
                 }

                 break;
              case Velocity_PR_horvel:
                 p_GN_Position->VelocityIndicator = GN_horvel;

                 if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horvel.horspeed, &Temp ) )
                 {
                    p_GN_AGPS_GAD_Data->horspeed = Temp;
                    p_GN_Position->horspeed = Temp;
                 }

                 if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horvel.bearing, &Temp ) )
                 {
                    p_GN_AGPS_GAD_Data->bearing = Temp;
                    p_GN_Position->bearing = Temp;
                 }

                 break;
              case Velocity_PR_horveluncert:
                 p_GN_Position->VelocityIndicator = GN_horveluncert;

                 if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horveluncert.horspeed, &Temp ) )
                 {
                    p_GN_AGPS_GAD_Data->horspeed = Temp;
                    p_GN_Position->horspeed = Temp;
                 }

                 if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horveluncert.bearing, &Temp ) )
                 {
                    p_GN_AGPS_GAD_Data->bearing = Temp;
                    p_GN_Position->bearing = Temp;
                 }

                 if ( supl_PDU_Get_SDL_BitString_U4( &p_Position->velocity->choice.horveluncert.uncertspeed, &Temp ) )
                 {
                    p_GN_AGPS_GAD_Data->horuncertspeed = Temp;
                    p_GN_Position->horuncertspeed = Temp;
                 }

                 break;
              default:
                  break;
              }
  
           }

           Status = TRUE;
       }
   }

   return Status;
}


//*****************************************************************************
/// \brief
///      Function to add Status Code to the unencoded SUPL PDU type SUPL-END.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLTRIGGEREDSTOP_StatusCode
(
   void *p_ThisPDU,  ///< [in/out] Pointer to unencoded structure
                     ///   ULP_PDU_t. Passed in as (void *) to hide
                     ///   implementation.
   e_GN_StatusCode GN_StatusCode ///< [in] GN format status code.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTOP.statusCode = GN_Calloc(1,sizeof(StatusCode_t));


   switch( GN_StatusCode )
   {
   case  GN_StatusCode_unspecified:                ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTOP.statusCode,unspecified); /* *p_statusCode = unspecified;*/ break;
   case  GN_StatusCode_systemFailure:              ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTOP.statusCode,systemFailure);/*(p_statusCode->buf)=systemFailure;*/            break;
   case  GN_StatusCode_unexpectedMessage:          ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTOP.statusCode,unexpectedMessage);        break;
   case  GN_StatusCode_protocolError:              ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTOP.statusCode,protocolError);            break;
   case  GN_StatusCode_dataMissing:                ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTOP.statusCode,dataMissing);              break;
   case  GN_StatusCode_unexpectedDataValue:        ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTOP.statusCode,unexpectedDataValue);      break;
   case  GN_StatusCode_posMethodFailure:           ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTOP.statusCode,posMethodFailure);         break;
   case  GN_StatusCode_posMethodMismatch:          ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTOP.statusCode,posMethodMismatch);        break;
   case  GN_StatusCode_posProtocolMismatch:        ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTOP.statusCode,posProtocolMismatch);      break;
   case  GN_StatusCode_targetSETnotReachable:      ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTOP.statusCode,targetSETnotReachable);    break;
   case  GN_StatusCode_versionNotSupported:        ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTOP.statusCode,versionNotSupported);      break;
   case  GN_StatusCode_resourceShortage:           ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTOP.statusCode, resourceShortage);         break;
   case  GN_StatusCode_invalidSessionId:           ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTOP.statusCode,invalidSessionId);         break;
   case  GN_StatusCode_nonProxyModeNotSupported:   ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTOP.statusCode,nonProxyModeNotSupported); break;
   case  GN_StatusCode_proxyModeNotSupported:      ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTOP.statusCode,proxyModeNotSupported);    break;
   case  GN_StatusCode_positioningNotPermitted:    ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTOP.statusCode, positioningNotPermitted);  break;
   case  GN_StatusCode_authNetFailure:             ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTOP.statusCode, authNetFailure);           break;
   case  GN_StatusCode_authSuplinitFailure:        ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTOP.statusCode, authSuplinitFailure);      break;
   case  GN_StatusCode_consentDeniedByUser:        ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTOP.statusCode,consentDeniedByUser);       break;
   case  GN_StatusCode_consentGrantedByUser:       ASN1_ENUM_SET_CHK(p_PDU_Cast->message.choice.msSUPLTRIGGEREDSTOP.statusCode,consentGrantedByUser);     break;
   default:
      OS_ASSERT( 0 );
      break;
   }
}


//*****************************************************************************
/// \brief
///      Function to set the unencoded SUPL PDU type to SUPL-TRIGGERED-STOP.
///
/// \returns
///      Nothing.
//*****************************************************************************
void supl_PDU_Add_SUPLTRIGGEREDSTOP
(
   void *p_ThisPDU   ///< [in/out] Pointer to unencoded structure
                     ///   ULP_PDU. Passed in as (void *) to hide
                     ///   implementation.
)
{
   ULP_PDU_t *p_PDU_Cast = p_ThisPDU;

   //  SUPL-REPORT defined in  SUPL-REPORT.asn1
   p_PDU_Cast->message.present = UlpMessage_PR_msSUPLTRIGGEREDSTOP;
}


//*****************************************************************************
/// \brief
///      High level function to populate an unencoded SUPL-START PDU.
///
/// \returns
///      Pointer to unencoded structure ULP_PDU. Returned as (void *) to hide
///      implementation.
//*****************************************************************************
void *supl_PDU_Make_SUPLTRIGGEREDSTOP
(
   s_SUPL_Instance *p_SUPL_Instance, ///< [in] Current instance data used to
                                    ///   populate parts of the PDU.
   e_GN_StatusCode GN_StatusCode ///< [in] GN format status code.
)
{
    void *p_PDU_Src = NULL;
    s_SUPL_InstanceData    *p_SUPL = p_SUPL_Instance->p_SUPL; // SUPL specific data.

    p_PDU_Src = supl_PDU_Alloc();
    GN_SUPL_Log( "SUPL-TRIGGEREDSTOP Alloc: %08p", p_PDU_Src );

    supl_PDU_Add_Version( p_PDU_Src , p_SUPL_Instance );

    supl_PDU_Add_setSessionID( p_PDU_Src, p_SUPL->SetSessionId,     &p_SUPL->SetId );
    supl_PDU_Add_slpSessionID( p_PDU_Src, p_SUPL->p_SlpSessionId, &p_SUPL->SlpId );
    // Add the SUPL-START message.
    supl_PDU_Add_SUPLTRIGGEREDSTOP( p_PDU_Src );

    supl_PDU_Add_SUPLTRIGGEREDSTOP_StatusCode( p_PDU_Src,GN_StatusCode);


    return p_PDU_Src;
}




