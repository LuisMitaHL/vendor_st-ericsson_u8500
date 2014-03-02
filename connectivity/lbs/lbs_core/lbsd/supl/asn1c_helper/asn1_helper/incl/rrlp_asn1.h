//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename rrlp_asn1.h
//
//
// $Locker: $
//*************************************************************************
///
/// \ingroup rrlp_asn1.h
///
/// \file
/// \brief
///      ASN.1 helper routines implementation.
///
///      Access routines specifically for using the RRLP encode/decode routines
///      in a way useful for the supl_subsystem.
///
//*************************************************************************

#ifndef INCLUSION_GUARD_RRLP_ASN1_H
#define INCLUSION_GUARD_RRLP_ASN1_H


/*
 **************************************************************************
 * Includes
 **************************************************************************
*/

#include "rrlp_asn1_macros.h"
#include "rrlp_asn1_types.h"

/*
 **************************************************************************
 * Function prototypes
 **************************************************************************
*/

unsigned char RRLP_RegisterLibrary(void);

void RRLP_EncodeMessage(RRLP_ASN1_pdu_t * const rrlp_message, uint8_t * *const encoded_buffer, uint32_t * const encoded_length);

unsigned char RRLP_DecodeMessage(const uint32_t encoded_length, const uint8_t * const encoded_buffer, RRLP_ASN1_pdu_t * *rrlp_message);

void RRLP_FreeMessage(uint8_t * rrlp_message);

void RRLP_FreePDU(RRLP_ASN1_pdu_t * rrlp_pdu);





#endif                /* INCLUSION_GUARD_RRLP_ASN1_H */
