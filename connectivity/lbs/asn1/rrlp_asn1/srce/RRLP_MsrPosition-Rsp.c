/*
 * RRLP_MsrPosition-Rsp.c
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
 * From ASN.1 module "RRLP_Components"
 * 	found in "LPP_RRLP_Rel_10.asn"
 * 	`asn1c -gen-PER`
 */

#include <asn_internal.h>

#include "RRLP_MsrPosition-Rsp.h"

static asn_TYPE_member_t asn_MBR_RRLP_MsrPosition_Rsp_1[] = {
	{ ATF_POINTER, 10, offsetof(struct RRLP_MsrPosition_Rsp, multipleSets),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_RRLP_MultipleSets,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"multipleSets"
		},
	{ ATF_POINTER, 9, offsetof(struct RRLP_MsrPosition_Rsp, referenceIdentity),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_RRLP_ReferenceIdentity,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"referenceIdentity"
		},
	{ ATF_POINTER, 8, offsetof(struct RRLP_MsrPosition_Rsp, otd_MeasureInfo),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_RRLP_OTD_MeasureInfo,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"otd-MeasureInfo"
		},
	{ ATF_POINTER, 7, offsetof(struct RRLP_MsrPosition_Rsp, locationInfo),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_RRLP_LocationInfo,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"locationInfo"
		},
	{ ATF_POINTER, 6, offsetof(struct RRLP_MsrPosition_Rsp, gps_MeasureInfo),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_RRLP_GPS_MeasureInfo,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"gps-MeasureInfo"
		},
	{ ATF_POINTER, 5, offsetof(struct RRLP_MsrPosition_Rsp, locationError),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_RRLP_LocationError_RRLP,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"locationError"
		},
	{ ATF_POINTER, 4, offsetof(struct RRLP_MsrPosition_Rsp, extensionContainer),
		(ASN_TAG_CLASS_CONTEXT | (6 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_RRLP_ExtensionContainer,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"extensionContainer"
		},
	{ ATF_POINTER, 3, offsetof(struct RRLP_MsrPosition_Rsp, rel_98_MsrPosition_Rsp_Extension),
		(ASN_TAG_CLASS_CONTEXT | (7 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_RRLP_Rel_98_MsrPosition_Rsp_Extension,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"rel-98-MsrPosition-Rsp-Extension"
		},
	{ ATF_POINTER, 2, offsetof(struct RRLP_MsrPosition_Rsp, rel_5_MsrPosition_Rsp_Extension),
		(ASN_TAG_CLASS_CONTEXT | (8 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_RRLP_Rel_5_MsrPosition_Rsp_Extension,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"rel-5-MsrPosition-Rsp-Extension"
		},
	{ ATF_POINTER, 1, offsetof(struct RRLP_MsrPosition_Rsp, rel_7_MsrPosition_Rsp_Extension),
		(ASN_TAG_CLASS_CONTEXT | (9 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_RRLP_Rel_7_MsrPosition_Rsp_Extension,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"rel-7-MsrPosition-Rsp-Extension"
		},
};
static int asn_MAP_RRLP_MsrPosition_Rsp_oms_1[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
static ber_tlv_tag_t asn_DEF_RRLP_MsrPosition_Rsp_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_RRLP_MsrPosition_Rsp_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* multipleSets at 84 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* referenceIdentity at 85 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* otd-MeasureInfo at 86 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* locationInfo at 87 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* gps-MeasureInfo at 88 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 }, /* locationError at 89 */
    { (ASN_TAG_CLASS_CONTEXT | (6 << 2)), 6, 0, 0 }, /* extensionContainer at 90 */
    { (ASN_TAG_CLASS_CONTEXT | (7 << 2)), 7, 0, 0 }, /* rel-98-MsrPosition-Rsp-Extension at 93 */
    { (ASN_TAG_CLASS_CONTEXT | (8 << 2)), 8, 0, 0 }, /* rel-5-MsrPosition-Rsp-Extension at 94 */
    { (ASN_TAG_CLASS_CONTEXT | (9 << 2)), 9, 0, 0 } /* rel-7-MsrPosition-Rsp-Extension at 98 */
};
static asn_SEQUENCE_specifics_t asn_SPC_RRLP_MsrPosition_Rsp_specs_1 = {
	sizeof(struct RRLP_MsrPosition_Rsp),
	offsetof(struct RRLP_MsrPosition_Rsp, _asn_ctx),
	asn_MAP_RRLP_MsrPosition_Rsp_tag2el_1,
	10,	/* Count of tags in the map */
	asn_MAP_RRLP_MsrPosition_Rsp_oms_1,	/* Optional members */
	7, 3,	/* Root/Additions */
	6,	/* Start extensions */
	11	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_RRLP_MsrPosition_Rsp = {
	"RRLP_MsrPosition-Rsp",
	"RRLP_MsrPosition-Rsp",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	SEQUENCE_decode_uper,
	SEQUENCE_encode_uper,
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_RRLP_MsrPosition_Rsp_tags_1,
	sizeof(asn_DEF_RRLP_MsrPosition_Rsp_tags_1)
		/sizeof(asn_DEF_RRLP_MsrPosition_Rsp_tags_1[0]), /* 1 */
	asn_DEF_RRLP_MsrPosition_Rsp_tags_1,	/* Same as above */
	sizeof(asn_DEF_RRLP_MsrPosition_Rsp_tags_1)
		/sizeof(asn_DEF_RRLP_MsrPosition_Rsp_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_RRLP_MsrPosition_Rsp_1,
	10,	/* Elements count */
	&asn_SPC_RRLP_MsrPosition_Rsp_specs_1	/* Additional specs */
};

