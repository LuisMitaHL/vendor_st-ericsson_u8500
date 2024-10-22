/*
 * SUPL_ULP_BatchRepConditions.c
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
 * From ASN.1 module "SUPL-TRIGGERED-RESPONSE"
 *     found in "ULP_v2.0.0.asn"
 *     `asn1c -gen-PER -fcompound-names`
 */

#include <asn_internal.h>

#include "SUPL_ULP_BatchRepConditions.h"

static int
memb_num_interval_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
            asn_app_constraint_failed_f *ctfailcb, void *app_key) {
    long value;
    
    if(!sptr) {
        _ASN_CTFAIL(app_key, td, sptr,
            "%s: value not given (%s:%d)",
            td->name, __FILE__, __LINE__);
        return -1;
    }
    
    value = *(const long *)sptr;
    
    if((value >= 1 && value <= 1024)) {
        /* Constraint check succeeded */
        return 0;
    } else {
        _ASN_CTFAIL(app_key, td, sptr,
            "%s: constraint failed (%s:%d)",
            td->name, __FILE__, __LINE__);
        return -1;
    }
}

static int
memb_num_minutes_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
            asn_app_constraint_failed_f *ctfailcb, void *app_key) {
    long value;
    
    if(!sptr) {
        _ASN_CTFAIL(app_key, td, sptr,
            "%s: value not given (%s:%d)",
            td->name, __FILE__, __LINE__);
        return -1;
    }
    
    value = *(const long *)sptr;
    
    if((value >= 1 && value <= 2048)) {
        /* Constraint check succeeded */
        return 0;
    } else {
        _ASN_CTFAIL(app_key, td, sptr,
            "%s: constraint failed (%s:%d)",
            td->name, __FILE__, __LINE__);
        return -1;
    }
}

static asn_per_constraints_t asn_PER_memb_num_interval_constr_2 = {
    { APC_CONSTRAINED,     10,  10,  1,  1024 }    /* (1..1024) */,
    { APC_UNCONSTRAINED,    -1, -1,  0,  0 },
    0, 0    /* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_num_minutes_constr_3 = {
    { APC_CONSTRAINED,     11,  11,  1,  2048 }    /* (1..2048) */,
    { APC_UNCONSTRAINED,    -1, -1,  0,  0 },
    0, 0    /* No PER value map */
};
static asn_per_constraints_t asn_PER_type_BatchRepConditions_constr_1 = {
    { APC_CONSTRAINED | APC_EXTENSIBLE,  2,  2,  0,  2 }    /* (0..2,...) */,
    { APC_UNCONSTRAINED,    -1, -1,  0,  0 },
    0, 0    /* No PER value map */
};
static asn_TYPE_member_t asn_MBR_BatchRepConditions_1[] = {
    { ATF_NOFLAGS, 0, offsetof(struct BatchRepConditions, choice.num_interval),
        (ASN_TAG_CLASS_CONTEXT | (0 << 2)),
        -1,    /* IMPLICIT tag at current level */
        &asn_DEF_NativeInteger,
        memb_num_interval_constraint_1,
        &asn_PER_memb_num_interval_constr_2,
        0,
        "num-interval"
        },
    { ATF_NOFLAGS, 0, offsetof(struct BatchRepConditions, choice.num_minutes),
        (ASN_TAG_CLASS_CONTEXT | (1 << 2)),
        -1,    /* IMPLICIT tag at current level */
        &asn_DEF_NativeInteger,
        memb_num_minutes_constraint_1,
        &asn_PER_memb_num_minutes_constr_3,
        0,
        "num-minutes"
        },
    { ATF_NOFLAGS, 0, offsetof(struct BatchRepConditions, choice.endofsession),
        (ASN_TAG_CLASS_CONTEXT | (2 << 2)),
        -1,    /* IMPLICIT tag at current level */
        &asn_DEF_NULL,
        0,    /* Defer constraints checking to the member type */
        0,    /* No PER visible constraints */
        0,
        "endofsession"
        },
};
static asn_TYPE_tag2member_t asn_MAP_BatchRepConditions_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* num-interval at 631 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* num-minutes at 632 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 } /* endofsession at 633 */
};
static asn_CHOICE_specifics_t asn_SPC_BatchRepConditions_specs_1 = {
    sizeof(struct BatchRepConditions),
    offsetof(struct BatchRepConditions, _asn_ctx),
    offsetof(struct BatchRepConditions, present),
    sizeof(((struct BatchRepConditions *)0)->present),
    asn_MAP_BatchRepConditions_tag2el_1,
    3,    /* Count of tags in the map */
    0,
    3    /* Extensions start */
};
asn_TYPE_descriptor_t asn_DEF_BatchRepConditions = {
    "BatchRepConditions",
    "BatchRepConditions",
    CHOICE_free,
    CHOICE_print,
    CHOICE_constraint,
    CHOICE_decode_ber,
    CHOICE_encode_der,
    CHOICE_decode_xer,
    CHOICE_encode_xer,
    CHOICE_decode_uper,
    CHOICE_encode_uper,
    CHOICE_outmost_tag,
    0,    /* No effective tags (pointer) */
    0,    /* No effective tags (count) */
    0,    /* No tags (pointer) */
    0,    /* No tags (count) */
    &asn_PER_type_BatchRepConditions_constr_1,
    asn_MBR_BatchRepConditions_1,
    3,    /* Elements count */
    &asn_SPC_BatchRepConditions_specs_1    /* Additional specs */
};

