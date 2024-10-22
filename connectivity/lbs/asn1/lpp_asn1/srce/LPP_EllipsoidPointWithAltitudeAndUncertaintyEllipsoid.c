/*
 * LPP_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid.c
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

#include "LPP_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid.h"

static int
latitudeSign_2_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	/* Replace with underlying type checker */
	td->check_constraints = asn_DEF_ENUMERATED.check_constraints;
	return td->check_constraints(td, sptr, ctfailcb, app_key);
}

/*
 * This type is implemented using ENUMERATED,
 * so here we adjust the DEF accordingly.
 */
static void
latitudeSign_2_inherit_TYPE_descriptor(asn_TYPE_descriptor_t *td) {
	td->free_struct    = asn_DEF_ENUMERATED.free_struct;
	td->print_struct   = asn_DEF_ENUMERATED.print_struct;
	td->ber_decoder    = asn_DEF_ENUMERATED.ber_decoder;
	td->der_encoder    = asn_DEF_ENUMERATED.der_encoder;
	td->xer_decoder    = asn_DEF_ENUMERATED.xer_decoder;
	td->xer_encoder    = asn_DEF_ENUMERATED.xer_encoder;
	td->uper_decoder   = asn_DEF_ENUMERATED.uper_decoder;
	td->uper_encoder   = asn_DEF_ENUMERATED.uper_encoder;
	if(!td->per_constraints)
		td->per_constraints = asn_DEF_ENUMERATED.per_constraints;
	td->elements       = asn_DEF_ENUMERATED.elements;
	td->elements_count = asn_DEF_ENUMERATED.elements_count;
     /* td->specifics      = asn_DEF_ENUMERATED.specifics;	// Defined explicitly */
}

static void
latitudeSign_2_free(asn_TYPE_descriptor_t *td,
		void *struct_ptr, int contents_only) {
	latitudeSign_2_inherit_TYPE_descriptor(td);
	td->free_struct(td, struct_ptr, contents_only);
}

static int
latitudeSign_2_print(asn_TYPE_descriptor_t *td, const void *struct_ptr,
		int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
	latitudeSign_2_inherit_TYPE_descriptor(td);
	return td->print_struct(td, struct_ptr, ilevel, cb, app_key);
}

static asn_dec_rval_t
latitudeSign_2_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const void *bufptr, size_t size, int tag_mode) {
	latitudeSign_2_inherit_TYPE_descriptor(td);
	return td->ber_decoder(opt_codec_ctx, td, structure, bufptr, size, tag_mode);
}

static asn_enc_rval_t
latitudeSign_2_encode_der(asn_TYPE_descriptor_t *td,
		void *structure, int tag_mode, ber_tlv_tag_t tag,
		asn_app_consume_bytes_f *cb, void *app_key) {
	latitudeSign_2_inherit_TYPE_descriptor(td);
	return td->der_encoder(td, structure, tag_mode, tag, cb, app_key);
}

static asn_dec_rval_t
latitudeSign_2_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const char *opt_mname, const void *bufptr, size_t size) {
	latitudeSign_2_inherit_TYPE_descriptor(td);
	return td->xer_decoder(opt_codec_ctx, td, structure, opt_mname, bufptr, size);
}

static asn_enc_rval_t
latitudeSign_2_encode_xer(asn_TYPE_descriptor_t *td, void *structure,
		int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	latitudeSign_2_inherit_TYPE_descriptor(td);
	return td->xer_encoder(td, structure, ilevel, flags, cb, app_key);
}

static asn_dec_rval_t
latitudeSign_2_decode_uper(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		asn_per_constraints_t *constraints, void **structure, asn_per_data_t *per_data) {
	latitudeSign_2_inherit_TYPE_descriptor(td);
	return td->uper_decoder(opt_codec_ctx, td, constraints, structure, per_data);
}

static asn_enc_rval_t
latitudeSign_2_encode_uper(asn_TYPE_descriptor_t *td,
		asn_per_constraints_t *constraints,
		void *structure, asn_per_outp_t *per_out) {
	latitudeSign_2_inherit_TYPE_descriptor(td);
	return td->uper_encoder(td, constraints, structure, per_out);
}

static int
altitudeDirection_7_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	/* Replace with underlying type checker */
	td->check_constraints = asn_DEF_ENUMERATED.check_constraints;
	return td->check_constraints(td, sptr, ctfailcb, app_key);
}

/*
 * This type is implemented using ENUMERATED,
 * so here we adjust the DEF accordingly.
 */
static void
altitudeDirection_7_inherit_TYPE_descriptor(asn_TYPE_descriptor_t *td) {
	td->free_struct    = asn_DEF_ENUMERATED.free_struct;
	td->print_struct   = asn_DEF_ENUMERATED.print_struct;
	td->ber_decoder    = asn_DEF_ENUMERATED.ber_decoder;
	td->der_encoder    = asn_DEF_ENUMERATED.der_encoder;
	td->xer_decoder    = asn_DEF_ENUMERATED.xer_decoder;
	td->xer_encoder    = asn_DEF_ENUMERATED.xer_encoder;
	td->uper_decoder   = asn_DEF_ENUMERATED.uper_decoder;
	td->uper_encoder   = asn_DEF_ENUMERATED.uper_encoder;
	if(!td->per_constraints)
		td->per_constraints = asn_DEF_ENUMERATED.per_constraints;
	td->elements       = asn_DEF_ENUMERATED.elements;
	td->elements_count = asn_DEF_ENUMERATED.elements_count;
     /* td->specifics      = asn_DEF_ENUMERATED.specifics;	// Defined explicitly */
}

static void
altitudeDirection_7_free(asn_TYPE_descriptor_t *td,
		void *struct_ptr, int contents_only) {
	altitudeDirection_7_inherit_TYPE_descriptor(td);
	td->free_struct(td, struct_ptr, contents_only);
}

static int
altitudeDirection_7_print(asn_TYPE_descriptor_t *td, const void *struct_ptr,
		int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
	altitudeDirection_7_inherit_TYPE_descriptor(td);
	return td->print_struct(td, struct_ptr, ilevel, cb, app_key);
}

static asn_dec_rval_t
altitudeDirection_7_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const void *bufptr, size_t size, int tag_mode) {
	altitudeDirection_7_inherit_TYPE_descriptor(td);
	return td->ber_decoder(opt_codec_ctx, td, structure, bufptr, size, tag_mode);
}

static asn_enc_rval_t
altitudeDirection_7_encode_der(asn_TYPE_descriptor_t *td,
		void *structure, int tag_mode, ber_tlv_tag_t tag,
		asn_app_consume_bytes_f *cb, void *app_key) {
	altitudeDirection_7_inherit_TYPE_descriptor(td);
	return td->der_encoder(td, structure, tag_mode, tag, cb, app_key);
}

static asn_dec_rval_t
altitudeDirection_7_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const char *opt_mname, const void *bufptr, size_t size) {
	altitudeDirection_7_inherit_TYPE_descriptor(td);
	return td->xer_decoder(opt_codec_ctx, td, structure, opt_mname, bufptr, size);
}

static asn_enc_rval_t
altitudeDirection_7_encode_xer(asn_TYPE_descriptor_t *td, void *structure,
		int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	altitudeDirection_7_inherit_TYPE_descriptor(td);
	return td->xer_encoder(td, structure, ilevel, flags, cb, app_key);
}

static asn_dec_rval_t
altitudeDirection_7_decode_uper(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		asn_per_constraints_t *constraints, void **structure, asn_per_data_t *per_data) {
	altitudeDirection_7_inherit_TYPE_descriptor(td);
	return td->uper_decoder(opt_codec_ctx, td, constraints, structure, per_data);
}

static asn_enc_rval_t
altitudeDirection_7_encode_uper(asn_TYPE_descriptor_t *td,
		asn_per_constraints_t *constraints,
		void *structure, asn_per_outp_t *per_out) {
	altitudeDirection_7_inherit_TYPE_descriptor(td);
	return td->uper_encoder(td, constraints, structure, per_out);
}

static int
memb_degreesLatitude_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 0 && value <= 8388607)) {
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
memb_degreesLongitude_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= -8388608 && value <= 8388607)) {
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
memb_altitude_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 0 && value <= 32767)) {
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
memb_uncertaintySemiMajor_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
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

static int
memb_uncertaintySemiMinor_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
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

static int
memb_orientationMajorAxis_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 0 && value <= 179)) {
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
memb_uncertaintyAltitude_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
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

static int
memb_confidence_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 0 && value <= 100)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static asn_INTEGER_enum_map_t asn_MAP_latitudeSign_value2enum_2[] = {
	{ 0,	5,	"north" },
	{ 1,	5,	"south" }
};
static unsigned int asn_MAP_latitudeSign_enum2value_2[] = {
	0,	/* north(0) */
	1	/* south(1) */
};
static asn_INTEGER_specifics_t asn_SPC_latitudeSign_specs_2 = {
	asn_MAP_latitudeSign_value2enum_2,	/* "tag" => N; sorted by tag */
	asn_MAP_latitudeSign_enum2value_2,	/* N => "tag"; sorted by N */
	2,	/* Number of elements in the maps */
	0,	/* Enumeration is not extensible */
	1	/* Strict enumeration */
};
static ber_tlv_tag_t asn_DEF_latitudeSign_tags_2[] = {
	(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (10 << 2))
};
static asn_per_constraints_t asn_PER_latitudeSign_constr_2 = {
	{ APC_CONSTRAINED,	 1,  1,  0,  1 }	/* (0..1) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 }
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_latitudeSign_2 = {
	"latitudeSign",
	"latitudeSign",
	latitudeSign_2_free,
	latitudeSign_2_print,
	latitudeSign_2_constraint,
	latitudeSign_2_decode_ber,
	latitudeSign_2_encode_der,
	latitudeSign_2_decode_xer,
	latitudeSign_2_encode_xer,
	latitudeSign_2_decode_uper,
	latitudeSign_2_encode_uper,
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_latitudeSign_tags_2,
	sizeof(asn_DEF_latitudeSign_tags_2)
		/sizeof(asn_DEF_latitudeSign_tags_2[0]) - 1, /* 1 */
	asn_DEF_latitudeSign_tags_2,	/* Same as above */
	sizeof(asn_DEF_latitudeSign_tags_2)
		/sizeof(asn_DEF_latitudeSign_tags_2[0]), /* 2 */
	&asn_PER_latitudeSign_constr_2,
	0, 0,	/* Defined elsewhere */
	&asn_SPC_latitudeSign_specs_2	/* Additional specs */
};

static asn_INTEGER_enum_map_t asn_MAP_altitudeDirection_value2enum_7[] = {
	{ 0,	6,	"height" },
	{ 1,	5,	"depth" }
};
static unsigned int asn_MAP_altitudeDirection_enum2value_7[] = {
	1,	/* depth(1) */
	0	/* height(0) */
};
static asn_INTEGER_specifics_t asn_SPC_altitudeDirection_specs_7 = {
	asn_MAP_altitudeDirection_value2enum_7,	/* "tag" => N; sorted by tag */
	asn_MAP_altitudeDirection_enum2value_7,	/* N => "tag"; sorted by N */
	2,	/* Number of elements in the maps */
	0,	/* Enumeration is not extensible */
	1	/* Strict enumeration */
};
static ber_tlv_tag_t asn_DEF_altitudeDirection_tags_7[] = {
	(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (10 << 2))
};
static asn_per_constraints_t asn_PER_altitudeDirection_constr_7 = {
	{ APC_CONSTRAINED,	 1,  1,  0,  1 }	/* (0..1) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 }
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_altitudeDirection_7 = {
	"altitudeDirection",
	"altitudeDirection",
	altitudeDirection_7_free,
	altitudeDirection_7_print,
	altitudeDirection_7_constraint,
	altitudeDirection_7_decode_ber,
	altitudeDirection_7_encode_der,
	altitudeDirection_7_decode_xer,
	altitudeDirection_7_encode_xer,
	altitudeDirection_7_decode_uper,
	altitudeDirection_7_encode_uper,
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_altitudeDirection_tags_7,
	sizeof(asn_DEF_altitudeDirection_tags_7)
		/sizeof(asn_DEF_altitudeDirection_tags_7[0]) - 1, /* 1 */
	asn_DEF_altitudeDirection_tags_7,	/* Same as above */
	sizeof(asn_DEF_altitudeDirection_tags_7)
		/sizeof(asn_DEF_altitudeDirection_tags_7[0]), /* 2 */
	&asn_PER_altitudeDirection_constr_7,
	0, 0,	/* Defined elsewhere */
	&asn_SPC_altitudeDirection_specs_7	/* Additional specs */
};

static asn_per_constraints_t asn_PER_memb_latitudeSign_constr_2 = {
	{ APC_CONSTRAINED,	 1,  1,  0,  1 }	/* (0..1) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 }
};
static asn_per_constraints_t asn_PER_memb_degreesLatitude_constr_5 = {
	{ APC_CONSTRAINED,	 23, -1,  0,  8388607 }	/* (0..8388607) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 }
};
static asn_per_constraints_t asn_PER_memb_degreesLongitude_constr_6 = {
	{ APC_CONSTRAINED,	 24, -1, -8388608,  8388607 }	/* (-8388608..8388607) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 }
};
static asn_per_constraints_t asn_PER_memb_altitudeDirection_constr_7 = {
	{ APC_CONSTRAINED,	 1,  1,  0,  1 }	/* (0..1) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 }
};
static asn_per_constraints_t asn_PER_memb_altitude_constr_10 = {
	{ APC_CONSTRAINED,	 15,  15,  0,  32767 }	/* (0..32767) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 }
};
static asn_per_constraints_t asn_PER_memb_uncertaintySemiMajor_constr_11 = {
	{ APC_CONSTRAINED,	 7,  7,  0,  127 }	/* (0..127) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 }
};
static asn_per_constraints_t asn_PER_memb_uncertaintySemiMinor_constr_12 = {
	{ APC_CONSTRAINED,	 7,  7,  0,  127 }	/* (0..127) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 }
};
static asn_per_constraints_t asn_PER_memb_orientationMajorAxis_constr_13 = {
	{ APC_CONSTRAINED,	 8,  8,  0,  179 }	/* (0..179) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 }
};
static asn_per_constraints_t asn_PER_memb_uncertaintyAltitude_constr_14 = {
	{ APC_CONSTRAINED,	 7,  7,  0,  127 }	/* (0..127) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 }
};
static asn_per_constraints_t asn_PER_memb_confidence_constr_15 = {
	{ APC_CONSTRAINED,	 7,  7,  0,  100 }	/* (0..100) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 }
};
static asn_TYPE_member_t asn_MBR_LPP_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct LPP_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid, latitudeSign),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_latitudeSign_2,
		0,	/* Defer constraints checking to the member type */
		&asn_PER_memb_latitudeSign_constr_2,
		0,
		"latitudeSign"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LPP_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid, degreesLatitude),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_degreesLatitude_constraint_1,
		&asn_PER_memb_degreesLatitude_constr_5,
		0,
		"degreesLatitude"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LPP_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid, degreesLongitude),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_degreesLongitude_constraint_1,
		&asn_PER_memb_degreesLongitude_constr_6,
		0,
		"degreesLongitude"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LPP_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid, altitudeDirection),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_altitudeDirection_7,
		0,	/* Defer constraints checking to the member type */
		&asn_PER_memb_altitudeDirection_constr_7,
		0,
		"altitudeDirection"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LPP_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid, altitude),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_altitude_constraint_1,
		&asn_PER_memb_altitude_constr_10,
		0,
		"altitude"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LPP_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid, uncertaintySemiMajor),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_uncertaintySemiMajor_constraint_1,
		&asn_PER_memb_uncertaintySemiMajor_constr_11,
		0,
		"uncertaintySemiMajor"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LPP_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid, uncertaintySemiMinor),
		(ASN_TAG_CLASS_CONTEXT | (6 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_uncertaintySemiMinor_constraint_1,
		&asn_PER_memb_uncertaintySemiMinor_constr_12,
		0,
		"uncertaintySemiMinor"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LPP_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid, orientationMajorAxis),
		(ASN_TAG_CLASS_CONTEXT | (7 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_orientationMajorAxis_constraint_1,
		&asn_PER_memb_orientationMajorAxis_constr_13,
		0,
		"orientationMajorAxis"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LPP_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid, uncertaintyAltitude),
		(ASN_TAG_CLASS_CONTEXT | (8 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_uncertaintyAltitude_constraint_1,
		&asn_PER_memb_uncertaintyAltitude_constr_14,
		0,
		"uncertaintyAltitude"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LPP_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid, confidence),
		(ASN_TAG_CLASS_CONTEXT | (9 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_confidence_constraint_1,
		&asn_PER_memb_confidence_constr_15,
		0,
		"confidence"
		},
};
static ber_tlv_tag_t asn_DEF_LPP_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_LPP_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* latitudeSign at 2610 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* degreesLatitude at 2611 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* degreesLongitude at 2612 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* altitudeDirection at 2613 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* altitude at 2614 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 }, /* uncertaintySemiMajor at 2615 */
    { (ASN_TAG_CLASS_CONTEXT | (6 << 2)), 6, 0, 0 }, /* uncertaintySemiMinor at 2616 */
    { (ASN_TAG_CLASS_CONTEXT | (7 << 2)), 7, 0, 0 }, /* orientationMajorAxis at 2617 */
    { (ASN_TAG_CLASS_CONTEXT | (8 << 2)), 8, 0, 0 }, /* uncertaintyAltitude at 2618 */
    { (ASN_TAG_CLASS_CONTEXT | (9 << 2)), 9, 0, 0 } /* confidence at 2619 */
};
static asn_SEQUENCE_specifics_t asn_SPC_LPP_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid_specs_1 = {
	sizeof(struct LPP_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid),
	offsetof(struct LPP_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid, _asn_ctx),
	asn_MAP_LPP_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid_tag2el_1,
	10,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_LPP_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid = {
	"LPP_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid",
	"LPP_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid",
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
	asn_DEF_LPP_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid_tags_1,
	sizeof(asn_DEF_LPP_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid_tags_1)
		/sizeof(asn_DEF_LPP_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid_tags_1[0]), /* 1 */
	asn_DEF_LPP_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid_tags_1,	/* Same as above */
	sizeof(asn_DEF_LPP_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid_tags_1)
		/sizeof(asn_DEF_LPP_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_LPP_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid_1,
	10,	/* Elements count */
	&asn_SPC_LPP_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid_specs_1	/* Additional specs */
};

