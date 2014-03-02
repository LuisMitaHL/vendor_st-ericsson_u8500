//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename supl_asn1.h
//
// 
// $Locker: $
//*************************************************************************
///
/// \ingroup supl_asn1.h
///
/// \file
/// \brief
///      ASN.1 helper routines implementation.
///
///      Access routines specifically for using the SUPL encode/decode routines
///      in a way useful for the supl_subsystem.
///
//*************************************************************************

#ifndef INCLUSION_GUARD_SUPL_ASN1_H
#define INCLUSION_GUARD_SUPL_ASN1_H


/*
 **************************************************************************
 * Includes
 **************************************************************************
*/

//internal supl
#include "supl_asn1_macros.h"
#include "supl_asn1_types.h"
#include "SUPL_ULP_ULP-PDU.h"
//#include "SUPL_osa.h"

/*
 **************************************************************************
 * Function prototypes
 **************************************************************************
*/

typedef unsigned char   boolean;
void SUPL_register_library(void);

void SUPL_encode_message(
  SUPL_ASN1_ulp_pdu_t * const supl_message,
  uint8_t * * const encoded_buffer,
  uint32_t * const encoded_length);

boolean SUPL_decode_message(
  const uint32_t encoded_length,
  const uint8_t * const encoded_buffer,
  SUPL_ASN1_ulp_pdu_t * * supl_message);

void SUPL_free_message(
  uint8_t *supl_message);

void SUPL_free_pdu(
  SUPL_ASN1_ulp_pdu_t *supl_pdu);

boolean SUPL_decode_header(
  const uint32_t encoded_length,
  const uint8_t * const encoded_buffer,
  SUPL_ASN1_ulp_hdr_t * * supl_header);

void SUPL_free_header(
  SUPL_ASN1_ulp_hdr_t *supl_header);

uint32_t SUPL_bitstring_2_uint32(
  const uint32_t no_of_significant_bits,
  const uint8_t * value);

uint8_t * SUPL_uint32_2_bitstring(
  const uint32_t value,
  const uint32_t no_of_significant_bits);

uint32_t SUPL_utctime_2_uint32(
  const uint8_t * utc_time);

boolean SUPL_uint32_2_utctime(
  const uint32_t utc_seconds,
  uint8_t * const time_stamp);

#endif /* INCLUSION_GUARD_SUPL_ASN1_H */
