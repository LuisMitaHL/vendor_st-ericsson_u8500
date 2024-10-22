/*
 * SUPL_ULP_UTRAN-GANSSReferenceTime.c
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

#include "SUPL_ULP_UTRAN-GANSSReferenceTime.h"

static int
memb_ganssTOD_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
            asn_app_constraint_failed_f *ctfailcb, void *app_key) {
    long value;
    
    if(!sptr) {
        _ASN_CTFAIL(app_key, td, sptr,
            "%s: value not given (%s:%d)",
            td->name, __FILE__, __LINE__);
        return -1;
    }
    
    value = *(const long *)sptr;
    
    if((value >= 0 && value <= 86399)) {
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
memb_utran_GANSSTimingOfCell_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
            asn_app_constraint_failed_f *ctfailcb, void *app_key) {
    long value;
    
    if(!sptr) {
        _ASN_CTFAIL(app_key, td, sptr,
            "%s: value not given (%s:%d)",
            td->name, __FILE__, __LINE__);
        return -1;
    }
    
    value = *(const long *)sptr;
    
    if((value >= 0 && value <= 3999999)) {
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
memb_sfn_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
            asn_app_constraint_failed_f *ctfailcb, void *app_key) {
    long value;
    
    if(!sptr) {
        _ASN_CTFAIL(app_key, td, sptr,
            "%s: value not given (%s:%d)",
            td->name, __FILE__, __LINE__);
        return -1;
    }
    
    value = *(const long *)sptr;
    
    if((value >= 0 && value <= 4095)) {
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
memb_ganss_TODUncertainty_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
            asn_app_constraint_failed_f *ctfailcb, void *app_key) {
    long value;
    
    if(!sptr) {
        _ASN_CTFAIL(app_key, td, sptr,
            "%s: value not given (%s:%d)",
            td->name, __FILE__, __LINE__);
        return -1;
    }
    
    value = *(const long *)sptr;
    
    if((value >= 0 && value <= 127)) {
        /* Constraint check succeeded */
        return 0;
    } else {
        _ASN_CTFAIL(app_key, td, sptr,
            "%s: constraint failed (%s:%d)",
            td->name, __FILE__, __LINE__);
        return -1;
    }
}

static asn_per_constraints_t asn_PER_type_modeSpecificInfo_constr_4 = {
    { APC_CONSTRAINED,     1,  1,  0,  1 }    /* (0..1) */,
    { APC_UNCONSTRAINED,    -1, -1,  0,  0 },
    0, 0    /* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_ganssTOD_constr_2 = {
    { APC_CONSTRAINED,     17, -1,  0,  86399 }    /* (0..86399) */,
    { APC_UNCONSTRAINED,    -1, -1,  0,  0 },
    0, 0    /* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_utran_GANSSTimingOfCell_constr_3 = {
    { APC_CONSTRAINED,     22, -1,  0,  3999999 }    /* (0..3999999) */,
    { APC_UNCONSTRAINED,    -1, -1,  0,  0 },
    0, 0    /* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_sfn_constr_9 = {
    { APC_CONSTRAINED,     12,  12,  0,  4095 }    /* (0..4095) */,
    { APC_UNCONSTRAINED,    -1, -1,  0,  0 },
    0, 0    /* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_ganss_TODUncertainty_constr_10 = {
    { APC_CONSTRAINED,     7,  7,  0,  127 }    /* (0..127) */,
    { APC_UNCONSTRAINED,    -1, -1,  0,  0 },
    0, 0    /* No PER value map */
};
static asn_TYPE_member_t asn_MBR_fdd_5[] = {
    { ATF_NOFLAGS, 0, offsetof(struct UTRAN_GANSSReferenceTime__modeSpecificInfo__fdd, referenceIdentity),
        (ASN_TAG_CLASS_CONTEXT | (0 << 2)),
        -1,    /* IMPLICIT tag at current level */
        &asn_DEF_PrimaryCPICH_Info,
        0,    /* Defer constraints checking to the member type */
        0,    /* No PER visible constraints */
        0,
        "referenceIdentity"
        },
};
static ber_tlv_tag_t asn_DEF_fdd_tags_5[] = {
    (ASN_TAG_CLASS_CONTEXT | (0 << 2)),
    (ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_fdd_tag2el_5[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 } /* referenceIdentity at 1622 */
};
static asn_SEQUENCE_specifics_t asn_SPC_fdd_specs_5 = {
    sizeof(struct UTRAN_GANSSReferenceTime__modeSpecificInfo__fdd),
    offsetof(struct UTRAN_GANSSReferenceTime__modeSpecificInfo__fdd, _asn_ctx),
    asn_MAP_fdd_tag2el_5,
    1,    /* Count of tags in the map */
    0, 0, 0,    /* Optional elements (not needed) */
    -1,    /* Start extensions */
    -1    /* Stop extensions */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_fdd_5 = {
    "fdd",
    "fdd",
    SEQUENCE_free,
    SEQUENCE_print,
    SEQUENCE_constraint,
    SEQUENCE_decode_ber,
    SEQUENCE_encode_der,
    SEQUENCE_decode_xer,
    SEQUENCE_encode_xer,
    SEQUENCE_decode_uper,
    SEQUENCE_encode_uper,
    0,    /* Use generic outmost tag fetcher */
    asn_DEF_fdd_tags_5,
    sizeof(asn_DEF_fdd_tags_5)
        /sizeof(asn_DEF_fdd_tags_5[0]) - 1, /* 1 */
    asn_DEF_fdd_tags_5,    /* Same as above */
    sizeof(asn_DEF_fdd_tags_5)
        /sizeof(asn_DEF_fdd_tags_5[0]), /* 2 */
    0,    /* No PER visible constraints */
    asn_MBR_fdd_5,
    1,    /* Elements count */
    &asn_SPC_fdd_specs_5    /* Additional specs */
};

static asn_TYPE_member_t asn_MBR_tdd_7[] = {
    { ATF_NOFLAGS, 0, offsetof(struct UTRAN_GANSSReferenceTime__modeSpecificInfo__tdd, referenceIdentity),
        (ASN_TAG_CLASS_CONTEXT | (0 << 2)),
        -1,    /* IMPLICIT tag at current level */
        &asn_DEF_CellParametersID,
        0,    /* Defer constraints checking to the member type */
        0,    /* No PER visible constraints */
        0,
        "referenceIdentity"
        },
};
static ber_tlv_tag_t asn_DEF_tdd_tags_7[] = {
    (ASN_TAG_CLASS_CONTEXT | (1 << 2)),
    (ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_tdd_tag2el_7[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 } /* referenceIdentity at 1624 */
};
static asn_SEQUENCE_specifics_t asn_SPC_tdd_specs_7 = {
    sizeof(struct UTRAN_GANSSReferenceTime__modeSpecificInfo__tdd),
    offsetof(struct UTRAN_GANSSReferenceTime__modeSpecificInfo__tdd, _asn_ctx),
    asn_MAP_tdd_tag2el_7,
    1,    /* Count of tags in the map */
    0, 0, 0,    /* Optional elements (not needed) */
    -1,    /* Start extensions */
    -1    /* Stop extensions */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_tdd_7 = {
    "tdd",
    "tdd",
    SEQUENCE_free,
    SEQUENCE_print,
    SEQUENCE_constraint,
    SEQUENCE_decode_ber,
    SEQUENCE_encode_der,
    SEQUENCE_decode_xer,
    SEQUENCE_encode_xer,
    SEQUENCE_decode_uper,
    SEQUENCE_encode_uper,
    0,    /* Use generic outmost tag fetcher */
    asn_DEF_tdd_tags_7,
    sizeof(asn_DEF_tdd_tags_7)
        /sizeof(asn_DEF_tdd_tags_7[0]) - 1, /* 1 */
    asn_DEF_tdd_tags_7,    /* Same as above */
    sizeof(asn_DEF_tdd_tags_7)
        /sizeof(asn_DEF_tdd_tags_7[0]), /* 2 */
    0,    /* No PER visible constraints */
    asn_MBR_tdd_7,
    1,    /* Elements count */
    &asn_SPC_tdd_specs_7    /* Additional specs */
};

static asn_TYPE_member_t asn_MBR_modeSpecificInfo_4[] = {
    { ATF_NOFLAGS, 0, offsetof(struct UTRAN_GANSSReferenceTime__modeSpecificInfo, choice.fdd),
        (ASN_TAG_CLASS_CONTEXT | (0 << 2)),
        0,
        &asn_DEF_fdd_5,
        0,    /* Defer constraints checking to the member type */
        0,    /* No PER visible constraints */
        0,
        "fdd"
        },
    { ATF_NOFLAGS, 0, offsetof(struct UTRAN_GANSSReferenceTime__modeSpecificInfo, choice.tdd),
        (ASN_TAG_CLASS_CONTEXT | (1 << 2)),
        0,
        &asn_DEF_tdd_7,
        0,    /* Defer constraints checking to the member type */
        0,    /* No PER visible constraints */
        0,
        "tdd"
        },
};
static asn_TYPE_tag2member_t asn_MAP_modeSpecificInfo_tag2el_4[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* fdd at 1622 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 } /* tdd at 1624 */
};
static asn_CHOICE_specifics_t asn_SPC_modeSpecificInfo_specs_4 = {
    sizeof(struct UTRAN_GANSSReferenceTime__modeSpecificInfo),
    offsetof(struct UTRAN_GANSSReferenceTime__modeSpecificInfo, _asn_ctx),
    offsetof(struct UTRAN_GANSSReferenceTime__modeSpecificInfo, present),
    sizeof(((struct UTRAN_GANSSReferenceTime__modeSpecificInfo *)0)->present),
    asn_MAP_modeSpecificInfo_tag2el_4,
    2,    /* Count of tags in the map */
    0,
    -1    /* Extensions start */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_modeSpecificInfo_4 = {
    "modeSpecificInfo",
    "modeSpecificInfo",
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
    &asn_PER_type_modeSpecificInfo_constr_4,
    asn_MBR_modeSpecificInfo_4,
    2,    /* Elements count */
    &asn_SPC_modeSpecificInfo_specs_4    /* Additional specs */
};

static asn_TYPE_member_t asn_MBR_UTRAN_GANSSReferenceTime_1[] = {
    { ATF_NOFLAGS, 0, offsetof(struct UTRAN_GANSSReferenceTime, ganssTOD),
        (ASN_TAG_CLASS_CONTEXT | (0 << 2)),
        -1,    /* IMPLICIT tag at current level */
        &asn_DEF_NativeInteger,
        memb_ganssTOD_constraint_1,
        &asn_PER_memb_ganssTOD_constr_2,
        0,
        "ganssTOD"
        },
    { ATF_POINTER, 2, offsetof(struct UTRAN_GANSSReferenceTime, utran_GANSSTimingOfCell),
        (ASN_TAG_CLASS_CONTEXT | (1 << 2)),
        -1,    /* IMPLICIT tag at current level */
        &asn_DEF_NativeInteger,
        memb_utran_GANSSTimingOfCell_constraint_1,
        &asn_PER_memb_utran_GANSSTimingOfCell_constr_3,
        0,
        "utran-GANSSTimingOfCell"
        },
    { ATF_POINTER, 1, offsetof(struct UTRAN_GANSSReferenceTime, modeSpecificInfo),
        (ASN_TAG_CLASS_CONTEXT | (2 << 2)),
        +1,    /* EXPLICIT tag at current level */
        &asn_DEF_modeSpecificInfo_4,
        0,    /* Defer constraints checking to the member type */
        0,    /* No PER visible constraints */
        0,
        "modeSpecificInfo"
        },
    { ATF_NOFLAGS, 0, offsetof(struct UTRAN_GANSSReferenceTime, sfn),
        (ASN_TAG_CLASS_CONTEXT | (3 << 2)),
        -1,    /* IMPLICIT tag at current level */
        &asn_DEF_NativeInteger,
        memb_sfn_constraint_1,
        &asn_PER_memb_sfn_constr_9,
        0,
        "sfn"
        },
    { ATF_POINTER, 1, offsetof(struct UTRAN_GANSSReferenceTime, ganss_TODUncertainty),
        (ASN_TAG_CLASS_CONTEXT | (4 << 2)),
        -1,    /* IMPLICIT tag at current level */
        &asn_DEF_NativeInteger,
        memb_ganss_TODUncertainty_constraint_1,
        &asn_PER_memb_ganss_TODUncertainty_constr_10,
        0,
        "ganss-TODUncertainty"
        },
};
static int asn_MAP_UTRAN_GANSSReferenceTime_oms_1[] = { 1, 2, 4 };
static ber_tlv_tag_t asn_DEF_UTRAN_GANSSReferenceTime_tags_1[] = {
    (ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_UTRAN_GANSSReferenceTime_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* ganssTOD at 1618 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* utran-GANSSTimingOfCell at 1619 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* modeSpecificInfo at 1622 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* sfn at 1625 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 } /* ganss-TODUncertainty at 1626 */
};
static asn_SEQUENCE_specifics_t asn_SPC_UTRAN_GANSSReferenceTime_specs_1 = {
    sizeof(struct UTRAN_GANSSReferenceTime),
    offsetof(struct UTRAN_GANSSReferenceTime, _asn_ctx),
    asn_MAP_UTRAN_GANSSReferenceTime_tag2el_1,
    5,    /* Count of tags in the map */
    asn_MAP_UTRAN_GANSSReferenceTime_oms_1,    /* Optional members */
    3, 0,    /* Root/Additions */
    4,    /* Start extensions */
    6    /* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_UTRAN_GANSSReferenceTime = {
    "UTRAN-GANSSReferenceTime",
    "UTRAN-GANSSReferenceTime",
    SEQUENCE_free,
    SEQUENCE_print,
    SEQUENCE_constraint,
    SEQUENCE_decode_ber,
    SEQUENCE_encode_der,
    SEQUENCE_decode_xer,
    SEQUENCE_encode_xer,
    SEQUENCE_decode_uper,
    SEQUENCE_encode_uper,
    0,    /* Use generic outmost tag fetcher */
    asn_DEF_UTRAN_GANSSReferenceTime_tags_1,
    sizeof(asn_DEF_UTRAN_GANSSReferenceTime_tags_1)
        /sizeof(asn_DEF_UTRAN_GANSSReferenceTime_tags_1[0]), /* 1 */
    asn_DEF_UTRAN_GANSSReferenceTime_tags_1,    /* Same as above */
    sizeof(asn_DEF_UTRAN_GANSSReferenceTime_tags_1)
        /sizeof(asn_DEF_UTRAN_GANSSReferenceTime_tags_1[0]), /* 1 */
    0,    /* No PER visible constraints */
    asn_MBR_UTRAN_GANSSReferenceTime_1,
    5,    /* Elements count */
    &asn_SPC_UTRAN_GANSSReferenceTime_specs_1    /* Additional specs */
};

