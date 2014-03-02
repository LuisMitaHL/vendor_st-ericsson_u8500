//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
//
// Filename supl_asn1.c
//
//
// $Locker: $
//*************************************************************************
///
/// \ingroup supl_asn1.c
///
/// \file
/// \brief
///      ASN.1 helper routines implementation.
///
///      Access routines specifically for using the SUPL encode/decode routines
///      in a way useful for the supl_subsystem.
///
//*************************************************************************
/*
 **************************************************************************
 * Includes
 **************************************************************************
*/

//external
#include <asn_application.h>

extern asn_TYPE_descriptor_t asn_DEF_ULP_PDU;

//internal supl
#include "supl_asn1.h"
#include "supl_utility.h"
//#include "SUPL_osa.h"

/*
 **************************************************************************
 * Local defines and type declarations
 **************************************************************************
*/

#define SUPL_ASN1_ULP_PDU asn_DEF_ULP_PDU
#define SUPL_ASN1_HDR_ULP_PDU asn_DEF_ULP_PDU

//macros to extract digit
#define EXTRACT_TENS(t) (t / 10)
#define EXTRACT_UNITS(u) (u % 10)

/*
 **************************************************************************
 * Local definitions
 **************************************************************************
*/

/*
 **************************************************************************
 * Global Functions
 **************************************************************************
*/

/*
 **************************************************************************
 * Function: SUPL_register_library
 *
 * Parameters:
 *   [In] none
 *   [Out] none
 *
 * Return value: void
 *
 * Description: Register with the SUPL ASN1 library
 **************************************************************************
*/
void SUPL_register_library(void)
{
//  SUPL_DBG_("SUPL ASN1C library active !\n");
}

/*
 **************************************************************************
 * Function: SUPL_encode_message
 *
 * Parameters:
 *   [In] SUPL_ASN1_ulp_pdu_t*
 *   [Out] uint8_t**,
 *         uint32_t*
 *
 * Return value: void
 *
 * Description: Encode the SUPL message into ASN.1
 **************************************************************************
*/
void SUPL_encode_message(
  SUPL_ASN1_ulp_pdu_t * const supl_message,
  uint8_t * * const encoded_buffer,
  uint32_t * const encoded_length)
{
  asn_TYPE_descriptor_t *td = &SUPL_ASN1_ULP_PDU;
  ssize_t length = 0;
  uint8_t *encoded_message = NULL;

  length = uper_encode_to_new_buffer(
    td,
//  asn_per_constraints_t *constraints,
    NULL,
      supl_message,
      (void *)&encoded_message);

    if (length > 1)
  {
    //first 2 octets of a SUPL PER encoded ASN1 message are the length of the entire message
    *encoded_message = SUPL_INT_HBYTE((uint16_t)length);
    *(encoded_message + 1) = SUPL_INT_LBYTE((uint16_t)length);

    *encoded_buffer = encoded_message;
    *encoded_length = (uint32_t)length;

  }
  else
  {
//    SUPL_ERR_("SUPL ASN1 Encoding Error -> %s !\n", td->name);

      //asn_fprint(NULL, &SUPL_ASN1_HDR_ULP_PDU, (void *)supl_message);

    *encoded_buffer = NULL;
    *encoded_length = 0;
    }
}

/*
 **************************************************************************
 * Function: SUPL_decode_message
 *
 * Parameters:
 *   [In] uint32_t,
 *        uint8_t*
 *   [Out] SUPL_ASN1_ulp_pdu_t**
 *
 * Return value: boolean
 *
 * Description: Decode the SUPL message from ASN.1
 **************************************************************************
*/
boolean SUPL_decode_message(
  const uint32_t encoded_length,
  const uint8_t * const encoded_buffer,
  SUPL_ASN1_ulp_pdu_t * * supl_message)
{
  SUPL_ASN1_ulp_pdu_t *decoded_message = NULL;

  boolean decoded = FALSE;

    asn_codec_ctx_t *opt_codec_ctx = 0;
  char *encoded_message;
  size_t encoded_message_len;
    asn_dec_rval_t rval;

  // ASN.1 input messsage
    encoded_message = (char*)encoded_buffer;
    encoded_message_len = encoded_length;

    rval = uper_decode(opt_codec_ctx, &SUPL_ASN1_ULP_PDU, (void **)&decoded_message, encoded_message, encoded_message_len, 0,    0);

    *supl_message = (SUPL_ASN1_ulp_pdu_t *)decoded_message;

    if (rval.code == RC_OK)
    {
      decoded = TRUE;
    }
    else
    {
    //    SUPL_ERR_("SUPL ASN1 Decoding Error -> %d !\n", rval.code);
    }

  return (decoded);
}

/*
 **************************************************************************
 * Function: SUPL_free_message
 *
 * Parameters:
 *   [In] uint8_t*
 *   [Out] none
 *
 * Return value: void
 *
 * Description: Free the SUPL message (ASN.1 encoded allocated memory)
 **************************************************************************
*/

void SUPL_free_message(
  uint8_t *supl_message)
{
  if (supl_message != NULL)
  {
#ifdef SUPL_ALLOC_DEBUG
    SUPL_DBG_("SUPL ASN1 free message = %p !\n", supl_message);
#endif /* #ifdef SUPL_ALLOC_DEBUG */

    //free allocated buffers
    free(supl_message);
    supl_message = NULL;
    //SUPL_HEAP_FREE(supl_message);
  }
}


/*
 **************************************************************************
 * Function: SUPL_free_pdu
 *
 * Parameters:
 *   [In] SUPL_ASN1_ulp_pdu_t*
 *   [Out] none
 *
 * Return value: void
 *
 * Description: Free the SUPL PDU (ASN.1 decoded allocated memory)
 **************************************************************************
*/
void SUPL_free_pdu(
  SUPL_ASN1_ulp_pdu_t *supl_pdu)
{
  if (supl_pdu != NULL)
  {
#ifdef SUPL_ALLOC_DEBUG
    SUPL_DBG_("SUPL ASN1 free PDU = %p !\n", supl_pdu);
#endif /* #ifdef SUPL_ALLOC_DEBUG */

    //free allocated PDU
    ASN_STRUCT_FREE(SUPL_ASN1_ULP_PDU, supl_pdu);
  }
}

/*
 **************************************************************************
 * Function: SUPL_decode_header
 *
 * Parameters:
 *   [In] uint32_t,
 *        uint8_t*
 *   [Out] SUPL_ASN1_ulp_hdr_t**
 *
 * Return value: boolean
 *
 * Description: Decode the SUPL header from ASN.1
 **************************************************************************
*/
boolean SUPL_decode_header(
  const uint32_t encoded_length,
  const uint8_t * const encoded_buffer,
  SUPL_ASN1_ulp_hdr_t * * supl_header)
{
  SUPL_ASN1_ulp_hdr_t *decoded_message = NULL;

  boolean decoded = FALSE;

    asn_codec_ctx_t *opt_codec_ctx = 0;
  char *encoded_message;
  size_t encoded_message_len;
    asn_dec_rval_t rval;

  // ASN.1 input messsage
    encoded_message = (char*)encoded_buffer;
    encoded_message_len = encoded_length;

    rval = uper_decode(opt_codec_ctx, &SUPL_ASN1_HDR_ULP_PDU, (void **)&decoded_message, encoded_message, encoded_message_len, 0,    0);

  if (rval.code == RC_OK)
  {
    *supl_header = (SUPL_ASN1_ulp_hdr_t *)decoded_message;

    decoded = TRUE;
  }
  else
  {
//    SUPL_ERR_("SUPL ASN1 Decoding Error -> %d !\n", rval.code);
  }

  return (decoded);
}

/*
 **************************************************************************
 * Function: SUPL_free_header
 *
 * Parameters:
 *   [In] SUPL_ASN1_ulp_pdu_t*
 *   [Out] none
 *
 * Return value: void
 *
 * Description: Free the SUPL PDU Header (ASN.1 decoded allocated memory)
 **************************************************************************
*/
void SUPL_free_header(
  SUPL_ASN1_ulp_hdr_t *supl_header)
{
  if (supl_header != NULL)
  {
#ifdef SUPL_ALLOC_DEBUG
    SUPL_DBG_("SUPL ASN1 free PDU = %p !\n", supl_header);
#endif /* #ifdef SUPL_ALLOC_DEBUG */

    //free allocated PDU
    ASN_STRUCT_FREE(SUPL_ASN1_HDR_ULP_PDU, supl_header);
  }
}
