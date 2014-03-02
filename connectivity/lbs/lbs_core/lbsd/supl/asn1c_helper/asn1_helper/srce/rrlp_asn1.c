/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

//
// Filename rrlp_asn1.c
//
//
// $Locker: $
//*************************************************************************
///
/// \ingroup rrlp_asn1.c
///
/// \file
/// \brief
///      ASN.1 helper routines implementation.
///
///      Access routines specifically for using the RRLP encode/decode routines
///      in a way useful for the supl_subsystem.
///
//*************************************************************************
//**************************************************************************
//* Includes
// **************************************************************************

//internal rrlp
#include "rrlp_asn1.h"

//external
#include <asn_application.h>
extern asn_TYPE_descriptor_t asn_DEF_RRLP_PDU;

/*
 **************************************************************************
 * Local defines and type declarations
 **************************************************************************
*/

#define RRLP_ASN1_PDU asn_DEF_RRLP_PDU
#define LPP_ASN1_PDU  asn_DEF_LPP_Message
/*
 **************************************************************************
 * Function: RRLP_RegisterLibrary
 *
 * Parameters:
 *   [In] none
 *   [Out] none
 *
 * Return value: boolean
 *
 * Description: Register with the RRLP ASN1 library
 **************************************************************************
*/
typedef unsigned char boolean;
boolean  RRLP_RegisterLibrary(void)
{
    boolean registered = TRUE;
//    RRLP_INF_("RRLP ASN1C library active!\n");
    return (registered);
}

/*
 **************************************************************************
 * Function: RRLP_EncodeMessage
 *
 * Parameters:
 *   [In] RRLP_ASN1_ulp_pdu_t*
 *   [Out] uint8_t**,
 *         uint32_t*
 *
 * Return value: void
 *
 * Description: Encode the SUPL message into ASN.1
 **************************************************************************
*/
void RRLP_EncodeMessage(RRLP_ASN1_pdu_t * const rrlp_message, uint8_t * *const encoded_buffer, uint32_t * const encoded_length)
{
    asn_TYPE_descriptor_t *td = &RRLP_ASN1_PDU;
    ssize_t length = 0;
    uint8_t *encoded_message = NULL;

    length = uper_encode_to_new_buffer(td,
//  asn_per_constraints_t *constraints,
                       NULL, rrlp_message, (void **) &encoded_message);

    if (length > 0) {                 //(length > 1) need to chk raghu
    *encoded_buffer = encoded_message;
    *encoded_length = (uint32_t) length;
    } else {
//    RRLP_ERR_("RRLP ASN1 Encoding Error -> %s!\n", td->name);

    *encoded_buffer = NULL;
    *encoded_length = 0;
    }
}

/*
 **************************************************************************
 * Function: RRLP_DecodeMessage
 *
 * Parameters:
 *   [In] uint32_t,
 *        uint8_t*
 *   [Out] RRLP_ASN1_pdu_t**
 *
 * Return value: boolean
 *
 * Description: Decode the SUPL message from ASN.1
 **************************************************************************
*/
boolean RRLP_DecodeMessage(const uint32_t encoded_length, const uint8_t * const encoded_buffer, RRLP_ASN1_pdu_t * *rrlp_message)
{
    RRLP_ASN1_pdu_t *decoded_message = NULL;

    boolean decoded = FALSE;

    asn_codec_ctx_t *opt_codec_ctx = 0;
    char *encoded_message;
    size_t encoded_message_len;
    asn_dec_rval_t rval;
    int ecbits = 0;        // Extra consumed bits in case of PER

    // ASN.1 input messsage
    encoded_message = (char *) encoded_buffer;
    encoded_message_len = encoded_length;

    // Pretend immediate EOF
    rval.code = RC_WMORE;
    rval.consumed = 0;

    rval = uper_decode(opt_codec_ctx, &RRLP_ASN1_PDU, (void **) &decoded_message, encoded_message, encoded_message_len, 0, 0);

    ecbits = rval.consumed % 8;    // Extra bits
    rval.consumed /= 8;        // Convert to value in bytes!

    if (rval.code == RC_OK) {
    *rrlp_message = (RRLP_ASN1_pdu_t *) decoded_message;

    decoded = TRUE;
    } else {
//    RRLP_ERR_("RRLP ASN1 Decoding Error -> %d!\n", rval.code);
    }

    return (decoded);
}

/*
 **************************************************************************
 * Function: RRLP_FreeMessage
 *
 * Parameters:
 *   [In] uint8_t*
 *   [Out] none
 *
 * Return value: void
 *
 * Description: Free the RRLP message (ASN.1 encoded allocated memory)
 **************************************************************************
*/
void RRLP_FreeMessage(uint8_t * rrlp_message)
{
    if (rrlp_message != NULL) {
#ifdef RRLP_ALLOC_DEBUG
    RRLP_INF_("RRLP ASN1 free message = %p !\n", rrlp_message);
#endif                // RRLP_ALLOC_DEBUG

    //free allocated buffers
//    RRLP_HEAP_FREE(rrlp_message);
    }
}

/*
 **************************************************************************
 * Function: RRLP_FreePDU
 *
 * Parameters:
 *   [In] RRLP_ASN1_pdu_t*
 *   [Out] none
 *
 * Return value: void
 *
 * Description: Free the RRLP PDU (ASN.1 decoded allocated memory)
 **************************************************************************
*/
void RRLP_FreePDU(RRLP_ASN1_pdu_t * rrlp_pdu)
{
    if (rrlp_pdu != NULL) {
#ifdef RRLP_ALLOC_DEBUG
    RRLP_INF_("RRLP ASN1 free PDU = %p !\n", rrlp_pdu);
#endif                // RRLP_ALLOC_DEBUG

    //free allocated PDU
    ASN_STRUCT_FREE(RRLP_ASN1_PDU, rrlp_pdu);
    }
}



