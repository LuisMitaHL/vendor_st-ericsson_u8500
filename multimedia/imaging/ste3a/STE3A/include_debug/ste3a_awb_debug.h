/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    ste3a_awb_debug.h
 * \brief   STE3A AWB debug types
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#ifndef STE3A_AWB_DEBUG_H_
#define STE3A_AWB_DEBUG_H_

#include "ste3a_types.h"

// algorithm internal constants and types (should be moved to a private header file)
#define STE3A_AWB_TUNING_FORMAT			0x54425741
#define STE3A_AWB_TUNING_VERSION		11

#define STE3A_AWB_GAUSSIAN_DEF			(sizeof(ste3a_gaussian) / sizeof(ste3a_gaussian_param))
#define STE3A_AWB_N_OF_MIXTURE			5
// number of live pdf (low-light, high-light and sky)
#define STE3A_AWB_N_OF_RUNTIME_PDF		(STE3A_AWB_N_OF_RUNTIME_CONTEXT + 1)
#define STE3A_AWB_N_OF_ILL_CTX			16
#define STE3A_AWB_N_OF_SUBJECT_CTX		8
#define STE3A_AWB_PDF_SIZE				128
#define STE3A_AWB_LOCUS_OVERSAMPLING	(STE3A_AWB_PDF_SIZE - 1)
#define STE3A_AWB_LOG_TABLE_SIZE		256


typedef float		ste3a_probability;						// probability density					[pure number ]
typedef float		ste3a_relevance;						// relevance							[pure number ]
typedef float		ste3a_locus_coord;						// the coord on the locus from 0 to N	[pure number ]
typedef uint8_t		ste3a_locus_coord_idx;					// the coord on the locus from 0 to N	[pure number ]
typedef float		ste3a_gaussian_param;					// type for gaussian lights_pdf_gauss_mix	[pure number ]

struct ste3a_illuminants_chroma
{
	ste3a_rb_chromaticity			illuminants[STE3A_AWB_N_OF_ILLUMINANTS];
};

struct ste3a_locus_chroma
{
	ste3a_rb_chromaticity			locus[STE3A_AWB_PDF_SIZE];
};

struct ste3a_class_chroma
{
	ste3a_rb_chromaticity			discrete[STE3A_AWB_N_OF_ILL_CLASSES];
};

struct ste3a_class_chroma_factor
{
	ste3a_probability				factor[STE3A_AWB_N_OF_ILL_CLASSES];
};

struct ste3a_illuminant_relevance
{
	ste3a_relevance					illuminants[STE3A_AWB_N_OF_ILL_CLASSES];
};

struct ste3a_surface_relevance
{
	ste3a_relevance					surfaces[STE3A_AWB_N_OF_SURFACES];
};

struct ste3a_locus_coord_range
{
	ste3a_locus_coord				low;
	ste3a_locus_coord				hi;
};

struct ste3a_kelvin_range
{
	ste3a_kelvin					low;
	ste3a_kelvin					hi;
};

struct ste3a_gaussian
{
	ste3a_gaussian_param			mean;
	ste3a_gaussian_param			sigma;
	ste3a_gaussian_param			weight;
};

struct ste3a_gaussian_mixture
{
	ste3a_gaussian					gaussian[STE3A_AWB_N_OF_MIXTURE];
};

struct ste3a_pdf
{
	ste3a_probability				pdf[STE3A_AWB_PDF_SIZE];
};

struct ste3a_discrete_pdf
{
	ste3a_probability				pdf[STE3A_AWB_N_OF_DISCRETE_ILL];
};

struct ste3a_rb_gains
{
	ste3a_gain						r;
	ste3a_gain						b;
};

struct ste3a_awb_intermediates
{
	// illuminants
	// the rounded locus coords for the four characterization illuminants (uint8_t)
	ste3a_locus_coord				ills_locus_coord[STE3A_AWB_N_OF_ILLUMINANTS];
	ste3a_locus_coord_idx			ills_locus_idx[STE3A_AWB_N_OF_ILLUMINANTS];
	// to be calculate when ills_kelvin changes
	ste3a_locus_coord				locus_coords[STE3A_AWB_PDF_SIZE];
	ste3a_kelvin					locus_kelvins[STE3A_AWB_PDF_SIZE];
	ste3a_float						locus_weights[STE3A_AWB_PDF_SIZE];
	// locus oversampled tuning.ills_log_gain_pair
	ste3a_rb_gains					locus_log_gain_pairs[STE3A_AWB_PDF_SIZE];
	// surfaces
	ste3a_float						sqr_sigma_table[STE3A_AWB_N_OF_SURFACES];
	ste3a_float						sqr_sigma_table_inv[STE3A_AWB_N_OF_SURFACES];
	// context
	// to be calculate when mixtures changes
	ste3a_pdf						lights_pdf[STE3A_AWB_N_OF_ILL_CTX];
	ste3a_pdf						runtime_light_pdf_pair[STE3A_AWB_N_OF_RUNTIME_CONTEXT];
	ste3a_pdf						ctx_pdf;
	// to be calculate when mixtures changes
	ste3a_kelvin					lights_pdf_mean_kelvin[STE3A_AWB_N_OF_ILL_CTX]; // TODO OBSOLETE
	ste3a_kelvin					runtime_light_pdf_mean_kelvin_pair[STE3A_AWB_N_OF_RUNTIME_PDF]; // TODO OBSOLETE
	// to be calculate when mixtures changes
	ste3a_locus_coord				lights_pdf_mean_coord[STE3A_AWB_N_OF_ILL_CTX]; // TODO OBSOLETE
	ste3a_locus_coord				runtime_light_pdf_mean_coord_pair[STE3A_AWB_N_OF_RUNTIME_PDF]; // TODO OBSOLETE
	ste3a_illuminant_relevance		runtime_light_pair_ill_classes_likelihood[STE3A_AWB_N_OF_RUNTIME_CONTEXT];
	ste3a_surface_relevance			runtime_subject_pair_surfs_likelihood[STE3A_AWB_N_OF_RUNTIME_CONTEXT];
	ste3a_kelvin_range				runtime_light_pair_kelvin_range[STE3A_AWB_N_OF_RUNTIME_CONTEXT];
	ste3a_locus_coord_range			runtime_light_pair_locus_coord_range[STE3A_AWB_N_OF_RUNTIME_CONTEXT];
	ste3a_illuminant_relevance		ill_classes_likelihood;
	ste3a_surface_relevance			surfs_likelihood;
	ste3a_locus_coord_range			locus_coord_range;
	// support
	ste3a_float						log_table[STE3A_AWB_LOG_TABLE_SIZE];
	ste3a_gain						awb_gains[3];
	ste3a_gain						awb_gains_status[3];
	ste3a_pdf						estimated_pdf;
	ste3a_pdf						estimated_pdf_status;
	ste3a_pdf						surfs_locus_histogram_tmp[STE3A_AWB_N_OF_SURFACES];
	ste3a_pdf						surfs_locus_histogram[STE3A_AWB_N_OF_SURFACES];
	ste3a_probability				surfs_locus_histogram_sum[STE3A_AWB_N_OF_SURFACES];
	ste3a_discrete_pdf				surfs_discrete_histogram_tmp[STE3A_AWB_N_OF_SURFACES];
	ste3a_discrete_pdf				surfs_discrete_histogram[STE3A_AWB_N_OF_SURFACES];
	ste3a_locus_chroma				surfs_locus_chromaticity[STE3A_AWB_N_OF_SURFACES-1];
	ste3a_class_chroma				surfs_ill_classes_chromaticity_diff[STE3A_AWB_N_OF_SURFACES-1];
	ste3a_class_chroma				surfs_ill_classes_chromaticity_sqr_diff[STE3A_AWB_N_OF_SURFACES-1];
	ste3a_class_chroma_factor		surfs_ill_classes_chromaticity_factor[STE3A_AWB_N_OF_SURFACES-1];
	ste3a_class_chroma				surfs_ill_classes_chromaticity_target[STE3A_AWB_N_OF_SURFACES-1];
	ste3a_float						surfs_comb_weight[STE3A_AWB_N_OF_SURFACES];
	ste3a_float						surfs_used_area[STE3A_AWB_N_OF_SURFACES];
	ste3a_float						surfs_total_score[STE3A_AWB_N_OF_SURFACES];
	ste3a_pdf						mean_locus_histogram;
	ste3a_discrete_pdf				mean_discrete_histogram;
	ste3a_float						ill_classes_score[STE3A_AWB_N_OF_ILL_CLASSES];
	ste3a_rb_gains					correction_log_gain_pair;
	ste3a_gain						tilt_log_gain_pair[3];
	ste3a_rb_gains					ill_classes_nominal_log_gain_pair[STE3A_AWB_N_OF_ILL_CLASSES];
	ste3a_float						ill_classes_hypothesis[STE3A_AWB_N_OF_ILL_CLASSES]; // TODO vector of 3 elements
	ste3a_float						locus_coord_result;
	ste3a_light_level				normalize_light_level;
	ste3a_float						zero_one[STE3A_AWB_N_OF_RUNTIME_CONTEXT];
	ste3a_color_matrix				ill_classes_color_matrix[STE3A_AWB_N_OF_ILL_CLASSES];
	ste3a_color_matrix				color_matrix;
	ste3a_color_matrix				color_matrix_status;
	ste3a_kelvin					ill_kelvin_status;
	ste3a_float						locus10[10][STE3A_AWB_N_OF_SURFACES-1];
	ste3a_count						update_time;
	ste3a_count						evaluate_time;
	ste3a_bool						stable_done;
	ste3a_bool						flash_fired;
	ste3a_bool						awb_reliable;
	ste3a_count						unreliable_switch_time;
};

#endif /* STE3A_AWB_DEBUG_H_ */
