/*
 * LPP_GNSS-AlmanacElement.c
 *
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * Author: sunilsatish.rao@stericsson.com for ST-Ericsson.
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
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "LPP_PDU-Definitions"
 * 	found in "LPP_RRLP_Rel_10.asn"
 * 	`asn1c -gen-PER`
 */

#include <asn_internal.h>

#include "LPP_GNSS-AlmanacElement.h"

static asn_TYPE_member_t asn_MBR_LPP_GNSS_AlmanacElement_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct LPP_GNSS_AlmanacElement, choice.keplerianAlmanacSet),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_LPP_AlmanacKeplerianSet,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"keplerianAlmanacSet"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LPP_GNSS_AlmanacElement, choice.keplerianNAV_Almanac),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_LPP_AlmanacNAV_KeplerianSet,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"keplerianNAV-Almanac"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LPP_GNSS_AlmanacElement, choice.keplerianReducedAlmanac),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_LPP_AlmanacReducedKeplerianSet,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"keplerianReducedAlmanac"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LPP_GNSS_AlmanacElement, choice.keplerianMidiAlmanac),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_LPP_AlmanacMidiAlmanacSet,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"keplerianMidiAlmanac"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LPP_GNSS_AlmanacElement, choice.keplerianGLONASS),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_LPP_AlmanacGLONASS_AlmanacSet,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"keplerianGLONASS"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LPP_GNSS_AlmanacElement, choice.ecef_SBAS_Almanac),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_LPP_AlmanacECEF_SBAS_AlmanacSet,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"ecef-SBAS-Almanac"
		},
};
static asn_TYPE_tag2member_t asn_MAP_LPP_GNSS_AlmanacElement_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* keplerianAlmanacSet at 3478 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* keplerianNAV-Almanac at 3479 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* keplerianReducedAlmanac at 3480 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* keplerianMidiAlmanac at 3481 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* keplerianGLONASS at 3482 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 } /* ecef-SBAS-Almanac at 3483 */
};
static asn_CHOICE_specifics_t asn_SPC_LPP_GNSS_AlmanacElement_specs_1 = {
	sizeof(struct LPP_GNSS_AlmanacElement),
	offsetof(struct LPP_GNSS_AlmanacElement, _asn_ctx),
	offsetof(struct LPP_GNSS_AlmanacElement, present),
	sizeof(((struct LPP_GNSS_AlmanacElement *)0)->present),
	asn_MAP_LPP_GNSS_AlmanacElement_tag2el_1,
	6,	/* Count of tags in the map */
	0,
	6	/* Extensions start */
};
static asn_per_constraints_t asn_PER_LPP_GNSS_AlmanacElement_constr_1 = {
	{ APC_CONSTRAINED | APC_EXTENSIBLE,  3,  3,  0,  5 }	/* (0..5,...) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 }
};
asn_TYPE_descriptor_t asn_DEF_LPP_GNSS_AlmanacElement = {
	"LPP_GNSS-AlmanacElement",
	"LPP_GNSS-AlmanacElement",
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
	0,	/* No effective tags (pointer) */
	0,	/* No effective tags (count) */
	0,	/* No tags (pointer) */
	0,	/* No tags (count) */
	&asn_PER_LPP_GNSS_AlmanacElement_constr_1,
	asn_MBR_LPP_GNSS_AlmanacElement_1,
	6,	/* Elements count */
	&asn_SPC_LPP_GNSS_AlmanacElement_specs_1	/* Additional specs */
};

