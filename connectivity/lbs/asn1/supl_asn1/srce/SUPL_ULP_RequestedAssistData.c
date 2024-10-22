/*
 * SUPL_ULP_RequestedAssistData.c
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
 * From ASN.1 module "SUPL-POS-INIT"
 *     found in "ULP_v2.0.0.asn"
 *     `asn1c -gen-PER -fcompound-names`
 */

#include <asn_internal.h>

#include "SUPL_ULP_RequestedAssistData.h"

static asn_TYPE_member_t asn_MBR_RequestedAssistData_1[] = {
    { ATF_NOFLAGS, 0, offsetof(struct RequestedAssistData, almanacRequested),
        (ASN_TAG_CLASS_CONTEXT | (0 << 2)),
        -1,    /* IMPLICIT tag at current level */
        &asn_DEF_BOOLEAN,
        0,    /* Defer constraints checking to the member type */
        0,    /* No PER visible constraints */
        0,
        "almanacRequested"
        },
    { ATF_NOFLAGS, 0, offsetof(struct RequestedAssistData, utcModelRequested),
        (ASN_TAG_CLASS_CONTEXT | (1 << 2)),
        -1,    /* IMPLICIT tag at current level */
        &asn_DEF_BOOLEAN,
        0,    /* Defer constraints checking to the member type */
        0,    /* No PER visible constraints */
        0,
        "utcModelRequested"
        },
    { ATF_NOFLAGS, 0, offsetof(struct RequestedAssistData, ionosphericModelRequested),
        (ASN_TAG_CLASS_CONTEXT | (2 << 2)),
        -1,    /* IMPLICIT tag at current level */
        &asn_DEF_BOOLEAN,
        0,    /* Defer constraints checking to the member type */
        0,    /* No PER visible constraints */
        0,
        "ionosphericModelRequested"
        },
    { ATF_NOFLAGS, 0, offsetof(struct RequestedAssistData, dgpsCorrectionsRequested),
        (ASN_TAG_CLASS_CONTEXT | (3 << 2)),
        -1,    /* IMPLICIT tag at current level */
        &asn_DEF_BOOLEAN,
        0,    /* Defer constraints checking to the member type */
        0,    /* No PER visible constraints */
        0,
        "dgpsCorrectionsRequested"
        },
    { ATF_NOFLAGS, 0, offsetof(struct RequestedAssistData, referenceLocationRequested),
        (ASN_TAG_CLASS_CONTEXT | (4 << 2)),
        -1,    /* IMPLICIT tag at current level */
        &asn_DEF_BOOLEAN,
        0,    /* Defer constraints checking to the member type */
        0,    /* No PER visible constraints */
        0,
        "referenceLocationRequested"
        },
    { ATF_NOFLAGS, 0, offsetof(struct RequestedAssistData, referenceTimeRequested),
        (ASN_TAG_CLASS_CONTEXT | (5 << 2)),
        -1,    /* IMPLICIT tag at current level */
        &asn_DEF_BOOLEAN,
        0,    /* Defer constraints checking to the member type */
        0,    /* No PER visible constraints */
        0,
        "referenceTimeRequested"
        },
    { ATF_NOFLAGS, 0, offsetof(struct RequestedAssistData, acquisitionAssistanceRequested),
        (ASN_TAG_CLASS_CONTEXT | (6 << 2)),
        -1,    /* IMPLICIT tag at current level */
        &asn_DEF_BOOLEAN,
        0,    /* Defer constraints checking to the member type */
        0,    /* No PER visible constraints */
        0,
        "acquisitionAssistanceRequested"
        },
    { ATF_NOFLAGS, 0, offsetof(struct RequestedAssistData, realTimeIntegrityRequested),
        (ASN_TAG_CLASS_CONTEXT | (7 << 2)),
        -1,    /* IMPLICIT tag at current level */
        &asn_DEF_BOOLEAN,
        0,    /* Defer constraints checking to the member type */
        0,    /* No PER visible constraints */
        0,
        "realTimeIntegrityRequested"
        },
    { ATF_NOFLAGS, 0, offsetof(struct RequestedAssistData, navigationModelRequested),
        (ASN_TAG_CLASS_CONTEXT | (8 << 2)),
        -1,    /* IMPLICIT tag at current level */
        &asn_DEF_BOOLEAN,
        0,    /* Defer constraints checking to the member type */
        0,    /* No PER visible constraints */
        0,
        "navigationModelRequested"
        },
    { ATF_POINTER, 2, offsetof(struct RequestedAssistData, navigationModelData),
        (ASN_TAG_CLASS_CONTEXT | (9 << 2)),
        -1,    /* IMPLICIT tag at current level */
        &asn_DEF_NavigationModel,
        0,    /* Defer constraints checking to the member type */
        0,    /* No PER visible constraints */
        0,
        "navigationModelData"
        },
    { ATF_POINTER, 1, offsetof(struct RequestedAssistData, ver2_RequestedAssistData_extension),
        (ASN_TAG_CLASS_CONTEXT | (10 << 2)),
        -1,    /* IMPLICIT tag at current level */
        &asn_DEF_Ver2_RequestedAssistData_extension,
        0,    /* Defer constraints checking to the member type */
        0,    /* No PER visible constraints */
        0,
        "ver2-RequestedAssistData-extension"
        },
};
static int asn_MAP_RequestedAssistData_oms_1[] = { 9, 10 };
static ber_tlv_tag_t asn_DEF_RequestedAssistData_tags_1[] = {
    (ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_RequestedAssistData_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* almanacRequested at 242 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* utcModelRequested at 243 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* ionosphericModelRequested at 244 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* dgpsCorrectionsRequested at 245 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* referenceLocationRequested at 246 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 }, /* referenceTimeRequested at 247 */
    { (ASN_TAG_CLASS_CONTEXT | (6 << 2)), 6, 0, 0 }, /* acquisitionAssistanceRequested at 248 */
    { (ASN_TAG_CLASS_CONTEXT | (7 << 2)), 7, 0, 0 }, /* realTimeIntegrityRequested at 249 */
    { (ASN_TAG_CLASS_CONTEXT | (8 << 2)), 8, 0, 0 }, /* navigationModelRequested at 250 */
    { (ASN_TAG_CLASS_CONTEXT | (9 << 2)), 9, 0, 0 }, /* navigationModelData at 251 */
    { (ASN_TAG_CLASS_CONTEXT | (10 << 2)), 10, 0, 0 } /* ver2-RequestedAssistData-extension at 253 */
};
static asn_SEQUENCE_specifics_t asn_SPC_RequestedAssistData_specs_1 = {
    sizeof(struct RequestedAssistData),
    offsetof(struct RequestedAssistData, _asn_ctx),
    asn_MAP_RequestedAssistData_tag2el_1,
    11,    /* Count of tags in the map */
    asn_MAP_RequestedAssistData_oms_1,    /* Optional members */
    1, 1,    /* Root/Additions */
    9,    /* Start extensions */
    12    /* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_RequestedAssistData = {
    "RequestedAssistData",
    "RequestedAssistData",
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
    asn_DEF_RequestedAssistData_tags_1,
    sizeof(asn_DEF_RequestedAssistData_tags_1)
        /sizeof(asn_DEF_RequestedAssistData_tags_1[0]), /* 1 */
    asn_DEF_RequestedAssistData_tags_1,    /* Same as above */
    sizeof(asn_DEF_RequestedAssistData_tags_1)
        /sizeof(asn_DEF_RequestedAssistData_tags_1[0]), /* 1 */
    0,    /* No PER visible constraints */
    asn_MBR_RequestedAssistData_1,
    11,    /* Elements count */
    &asn_SPC_RequestedAssistData_specs_1    /* Additional specs */
};

