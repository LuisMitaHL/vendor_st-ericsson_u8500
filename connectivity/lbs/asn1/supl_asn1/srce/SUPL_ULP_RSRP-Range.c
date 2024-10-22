/*
 * SUPL_ULP_RSRP-Range.c
 *
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * Author: david.gowers@stericsson.com for ST-Ericsson.
 * License terms: Redistribution and modifications are permitted subject to BSD license. 
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of ST-Ericsson nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ST-ERICSSON BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * Generated by asn1c-0.9.22 (http://lionet.info/asn1c)
 * From ASN.1 module "Ver2-ULP-Components"
 *     found in "ULP_v2.0.0.asn"
 *     `asn1c -gen-PER -fcompound-names`
 */

#include <asn_internal.h>

#include "SUPL_ULP_RSRP-Range.h"

int
RSRP_Range_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
            asn_app_constraint_failed_f *ctfailcb, void *app_key) {
    long value;
    
    if(!sptr) {
        _ASN_CTFAIL(app_key, td, sptr,
            "%s: value not given (%s:%d)",
            td->name, __FILE__, __LINE__);
        return -1;
    }
    
    value = *(const long *)sptr;
    
    if((value >= 0 && value <= 97)) {
        /* Constraint check succeeded */
        return 0;
    } else {
        _ASN_CTFAIL(app_key, td, sptr,
            "%s: constraint failed (%s:%d)",
            td->name, __FILE__, __LINE__);
        return -1;
    }
}

/*
 * This type is implemented using NativeInteger,
 * so here we adjust the DEF accordingly.
 */
static void
RSRP_Range_1_inherit_TYPE_descriptor(asn_TYPE_descriptor_t *td) {
    td->free_struct    = asn_DEF_NativeInteger.free_struct;
    td->print_struct   = asn_DEF_NativeInteger.print_struct;
    td->ber_decoder    = asn_DEF_NativeInteger.ber_decoder;
    td->der_encoder    = asn_DEF_NativeInteger.der_encoder;
    td->xer_decoder    = asn_DEF_NativeInteger.xer_decoder;
    td->xer_encoder    = asn_DEF_NativeInteger.xer_encoder;
    td->uper_decoder   = asn_DEF_NativeInteger.uper_decoder;
    td->uper_encoder   = asn_DEF_NativeInteger.uper_encoder;
    if(!td->per_constraints)
        td->per_constraints = asn_DEF_NativeInteger.per_constraints;
    td->elements       = asn_DEF_NativeInteger.elements;
    td->elements_count = asn_DEF_NativeInteger.elements_count;
    td->specifics      = asn_DEF_NativeInteger.specifics;
}

void
RSRP_Range_free(asn_TYPE_descriptor_t *td,
        void *struct_ptr, int contents_only) {
    RSRP_Range_1_inherit_TYPE_descriptor(td);
    td->free_struct(td, struct_ptr, contents_only);
}

int
RSRP_Range_print(asn_TYPE_descriptor_t *td, const void *struct_ptr,
        int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
    RSRP_Range_1_inherit_TYPE_descriptor(td);
    return td->print_struct(td, struct_ptr, ilevel, cb, app_key);
}

asn_dec_rval_t
RSRP_Range_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
        void **structure, const void *bufptr, size_t size, int tag_mode) {
    RSRP_Range_1_inherit_TYPE_descriptor(td);
    return td->ber_decoder(opt_codec_ctx, td, structure, bufptr, size, tag_mode);
}

asn_enc_rval_t
RSRP_Range_encode_der(asn_TYPE_descriptor_t *td,
        void *structure, int tag_mode, ber_tlv_tag_t tag,
        asn_app_consume_bytes_f *cb, void *app_key) {
    RSRP_Range_1_inherit_TYPE_descriptor(td);
    return td->der_encoder(td, structure, tag_mode, tag, cb, app_key);
}

asn_dec_rval_t
RSRP_Range_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
        void **structure, const char *opt_mname, const void *bufptr, size_t size) {
    RSRP_Range_1_inherit_TYPE_descriptor(td);
    return td->xer_decoder(opt_codec_ctx, td, structure, opt_mname, bufptr, size);
}

asn_enc_rval_t
RSRP_Range_encode_xer(asn_TYPE_descriptor_t *td, void *structure,
        int ilevel, enum xer_encoder_flags_e flags,
        asn_app_consume_bytes_f *cb, void *app_key) {
    RSRP_Range_1_inherit_TYPE_descriptor(td);
    return td->xer_encoder(td, structure, ilevel, flags, cb, app_key);
}

asn_dec_rval_t
RSRP_Range_decode_uper(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
        asn_per_constraints_t *constraints, void **structure, asn_per_data_t *per_data) {
    RSRP_Range_1_inherit_TYPE_descriptor(td);
    return td->uper_decoder(opt_codec_ctx, td, constraints, structure, per_data);
}

asn_enc_rval_t
RSRP_Range_encode_uper(asn_TYPE_descriptor_t *td,
        asn_per_constraints_t *constraints,
        void *structure, asn_per_outp_t *per_out) {
    RSRP_Range_1_inherit_TYPE_descriptor(td);
    return td->uper_encoder(td, constraints, structure, per_out);
}

static asn_per_constraints_t asn_PER_type_RSRP_Range_constr_1 = {
    { APC_CONSTRAINED,     7,  7,  0,  97 }    /* (0..97) */,
    { APC_UNCONSTRAINED,    -1, -1,  0,  0 },
    0, 0    /* No PER value map */
};
static ber_tlv_tag_t asn_DEF_RSRP_Range_tags_1[] = {
    (ASN_TAG_CLASS_UNIVERSAL | (2 << 2))
};
asn_TYPE_descriptor_t asn_DEF_RSRP_Range = {
    "RSRP-Range",
    "RSRP-Range",
    RSRP_Range_free,
    RSRP_Range_print,
    RSRP_Range_constraint,
    RSRP_Range_decode_ber,
    RSRP_Range_encode_der,
    RSRP_Range_decode_xer,
    RSRP_Range_encode_xer,
    RSRP_Range_decode_uper,
    RSRP_Range_encode_uper,
    0,    /* Use generic outmost tag fetcher */
    asn_DEF_RSRP_Range_tags_1,
    sizeof(asn_DEF_RSRP_Range_tags_1)
        /sizeof(asn_DEF_RSRP_Range_tags_1[0]), /* 1 */
    asn_DEF_RSRP_Range_tags_1,    /* Same as above */
    sizeof(asn_DEF_RSRP_Range_tags_1)
        /sizeof(asn_DEF_RSRP_Range_tags_1[0]), /* 1 */
    &asn_PER_type_RSRP_Range_constr_1,
    0, 0,    /* No members */
    0    /* No specifics */
};

