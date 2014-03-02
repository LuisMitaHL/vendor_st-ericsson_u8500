/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    steaeci.h
 * \brief   STE Auto White Balance internal functions declarations
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#ifndef STEAWBI_H_
#define STEAWBI_H_

#include "nmx.h"
#include "nmxAawb.h"


// TODO: move compensation kelvin range to tuning domain
struct ste3a_awb_camera_data
{
	// format
	uint32_t						format;
	uint32_t						version;
	// part-to-part variation handling
	ste3a_percentage				part_to_part_tol_percent;										// classification tolerance to residual part-to-part chromaticity variation (percent)
	ste3a_percentage				max_adaptive_correction_percent;								// maximum adaptive chromaticity correction (percent)
	// raw data significance handling
	ste3a_pixel_level				channel_low_thr_value;
	ste3a_percentage				clip_high_thr_percent;
	ste3a_float						nvm_sensitivity_pedestal;
	ste3a_float						nvm_reference[STE3A_AWB_N_OF_SENS];
	ste3a_float						nvm_delta[STE3A_AWB_N_OF_SENS];
	ste3a_float						corner_significance;
	// illuminants
	ste3a_kelvin					ills_kelvin[STE3A_AWB_N_OF_ILLUMINANTS];
	ste3a_rb_gains					ills_log_gain_pair[STE3A_AWB_N_OF_ILLUMINANTS];
	ste3a_color_matrix				ills_color_matrix_ref[STE3A_AWB_N_OF_ILLUMINANTS];
	ste3a_color_matrix				ills_color_matrix[STE3A_AWB_N_OF_ILLUMINANTS];
	ste3a_color_matrix				color_matrix_nvm_adaptation[STE3A_AWB_N_OF_SENS][STE3A_AWB_N_OF_ILLUMINANTS];
	// surfaces
	uint8_t							surfs_is_refelectance[STE3A_AWB_N_OF_SURFACES];
	ste3a_percentage				surfs_min_area_percent[STE3A_AWB_N_OF_SURFACES];
	ste3a_percentage				surfs_max_area_percent[STE3A_AWB_N_OF_SURFACES];
	ste3a_percentage				surfs_tol_percent[STE3A_AWB_N_OF_SURFACES];
	ste3a_float						surfs_adaptation[STE3A_AWB_N_OF_SURFACES-1];
	// surfaces + illuminants
	// no defalut surface (because -1)
	ste3a_illuminants_chroma		chromaticity_table_ref[STE3A_AWB_N_OF_SURFACES-1];
	ste3a_illuminants_chroma		chromaticity_table[STE3A_AWB_N_OF_SURFACES-1];
	ste3a_illuminants_chroma		chromaticity_nvm_adaptation[STE3A_AWB_N_OF_SENS][STE3A_AWB_N_OF_SURFACES-1];
	// context
	ste3a_gaussian_mixture			lights_pdf_gauss_mix[STE3A_AWB_N_OF_ILL_CTX];
	ste3a_illuminant_relevance		lights_ill_classes_likelihood[STE3A_AWB_N_OF_ILL_CTX];
	ste3a_surface_relevance			subjects_surfs_likelihood[STE3A_AWB_N_OF_SUBJECT_CTX];
	ste3a_kelvin_range				lights_kelvin_range[STE3A_AWB_N_OF_ILL_CTX];
};

struct steawb_internal
{
	// input data
	nmxMatrix	grid;							// input data
	// nvm reference
	nmxMatrix	nvm_reference;				// from camera data
	nmxMatrix	nvm_delta;				// from camera data
	// illuminants
	nmxMatrix	ills_kelvin;				// from camera data
	nmxMatrix	ills_log_gain_pair;			// from camera data
	nmxMatrix	ills_locus_coord;		// form intermediate
	nmxMatrix	ills_locus_idx;			// form intermediate
	nmxMatrix	ills_color_matrix_ref;		// from camera data
	nmxMatrix	ills_color_matrix;		// from camera data
	nmxMatrix	color_matrix_nvm_adaptation;		// from camera data
	// oversampled
	nmxMatrix	locus_coords;					// form intermediate
	nmxMatrix	locus_kelvins;					// form intermediate
	nmxMatrix	locus_weights;					// form intermediate
	nmxMatrix	locus_log_gain_pairs;					// form intermediate
	// surface
	nmxMatrix	surfs_min_area_percent;				// from tuning (DA FARE)
	nmxMatrix	surfs_max_area_percent;				// from tuning (DA FARE)
	nmxMatrix	surfs_is_refelectance;		// from tuning (DA FARE)
	nmxMatrix	surfs_adaptation;
	// surface + illuminants
	nmxMatrix	chromaticity_table_ref;				// from camera data
	nmxMatrix	chromaticity_table;				// from camera data
	nmxMatrix	chromaticity_nvm_adaptation;				// from camera data
	nmxMatrix	nvm_chroma_adaptation;	// from camera data
	nmxMatrix	surfs_tol_percent;		// from camera data
	nmxMatrix	sqr_sigma_table;				// from intermediate
	nmxMatrix	sqr_sigma_table_inv;			// from intermediate
	nmxMatrix	locus10;			// from intermediate
	// context
	nmxMatrix	runtime_light_idx_pair;			// form tuning
	nmxMatrix	runtime_subject_idx_pair;		// form tuning
	nmxMatrix	lights_pdf_gauss_mix;						// form tuning
	nmxMatrix	lights_pdf;							// form intermediate
	nmxMatrix	runtime_light_pdf_pair;					// form intermediate
	nmxMatrix	ctx_pdf;						// form intermediate runtime (interpolation)
	nmxMatrix	sky_pdf;						// last pdf in lights_pdf
	nmxMatrix	lights_pdf_mean_kelvin;				// form intermediate
	nmxMatrix	runtime_light_pdf_mean_kelvin_pair;		// form intermediate
	nmxMatrix	lights_pdf_mean_coord;				// form intermediate
	nmxMatrix	runtime_light_pdf_mean_coord_pair;		// form intermediate
	nmxMatrix	lights_ill_classes_likelihood;				// from tuning
	nmxMatrix	runtime_light_pair_ill_classes_likelihood;		// from intermediate
	nmxMatrix	ill_classes_likelihood;				// from intermediate runtime (interpolation)
	nmxMatrix	subjects_surfs_likelihood;			// from tuning
	nmxMatrix	runtime_subject_pair_surfs_likelihood;	// from intermediate
	nmxMatrix	surfs_likelihood;				// from intermediate runtime (interpolation)
	nmxMatrix	lights_kelvin_range;				// from tuning
	nmxMatrix	runtime_light_pair_kelvin_range;		// from intermediate
	nmxMatrix	runtime_light_pair_locus_coord_range;		// from intermediate
	nmxMatrix	locus_coord_range;			// form intermediate
	// sensor
	nmxMatrix	sensor_log_tilts;				// from tuning
	// support
	nmxMatrix	log_table;						// from intermediate
	nmxMatrix	awb_gains;						// from intermediate
	nmxMatrix	estimated_pdf;					// form intermediate runtime (temporary)
	nmxMatrix	estimated_pdf_status;			// form intermediate runtime (status)
	nmxMatrix	surfs_locus_histogram_tmp;		// form intermediate runtime (temporary)
	nmxMatrix	surfs_locus_histogram;			// form intermediate runtime (status)
	nmxMatrix	surfs_locus_histogram_sum;		// form intermediate runtime (status)
	nmxMatrix	surfs_discrete_histogram_tmp;	// form intermediate runtime (temporary)
	nmxMatrix	surfs_discrete_histogram;		// form intermediate runtime (status)
	nmxMatrix	surfs_locus_chromaticity;				// form intermediate runtime (status)
	nmxMatrix	surfs_ill_classes_chromaticity_diff;			// form intermediate runtime (status)
	nmxMatrix	surfs_ill_classes_chromaticity_sqr_diff;		// form intermediate runtime (status)
	nmxMatrix	surfs_ill_classes_chromaticity_factor;		// form intermediate runtime (status)
	nmxMatrix	surfs_ill_classes_chromaticity_target;		// form intermediate runtime (status)
	nmxMatrix	surfs_comb_weight;				// form intermediate runtime (status)
	nmxMatrix	surfs_used_area;				// form intermediate runtime (status)
	nmxMatrix	surfs_total_score;				// form intermediate runtime (status)
	nmxMatrix	mean_locus_histogram;			// form intermediate runtime (status)
	nmxMatrix	mean_discrete_histogram;		// form intermediate runtime (status)
	nmxMatrix	ill_classes_score;		// form intermediate runtime (status)
	nmxMatrix	correction_log_gain_pair;			// form intermediate runtime (status)
	nmxMatrix	tilt_log_gain_pair;					// form intermediate runtime (status)
	nmxMatrix	ill_classes_nominal_log_gain_pair;				// form intermediate runtime (status)
	nmxMatrix	ill_classes_hypothesis;	// form intermediate runtime (status)
	nmxMatrix	locus_coord_result;				// form intermediate runtime (status)
	nmxMatrix	scenes_light_level_thr_pair;				// from tuning
	nmxMatrix	normalize_light_level;		// form intermediate runtime (status)
	nmxMatrix	zero_one;						// form intermediate
	nmxMatrix	ill_classes_color_matrix;// form intermediate
	nmxMatrix	color_matrix;					// form intermediate
};

void	steawbi_init_internal_structure(
		const	ste3a_awb_tuning		&tuning,
		const	ste3a_awb_camera_data	&camera_data,
		const	ste3a_awb_intermediates	&intermediate,
				steawb_internal			&internal );

void	steawbi_init_all(
		const	ste3a_awb_tuning			&tuning,
		const	ste3a_awb_camera_data		&camera_data,
				steawb_internal				&internal );

void	steawbi_init_runtime_pdfs(
		const	ste3a_awb_tuning			&tuning,
		const	ste3a_awb_camera_data		&camera_data,
				ste3a_awb_intermediates		&intermediates );

void	steawbi_init_locus_coord_ranges(
				steawb_internal		&internal);

void	steawbi_init_pdfs(
				nmxMatrix	*mixtures,
				nmxMatrix	*locus_kelvins,
				nmxMatrix	*locus_weights,
				nmxMatrix	*lights_pdf,
				nmxMatrix	*lights_pdf_mean_coord,
				nmxMatrix	*lights_pdf_mean_kelvin );

void	steawbi_init_rb_gains(
				nmxMatrix	*ills_log_gain_pair,
				nmxMatrix	*locus_log_gain_pairs );

void	steawbi_ctx_interpolation(
				ste3a_light_level	light_level,
				steawb_internal		&internal );

ste3a_bool	steawbi_load_camera_data(
		const 	void * buffer,
				ste3a_awb_camera_data & camera_data);

void	steawbi_default_tuning(
				ste3a_awb_tuning	&tuning );

void	steawbi_initialize_camera_data(
				ste3a_awb_camera_data	&camera_data );

void steawbi_compute_stats_buffer(
		const	ste3a_grid_stats_descriptor		& grid_stats_descriptor,
		const	ste3a_gridstats_val *			  grid_frst,
				ste3a_gridstats_val *			& grid_base,
				ste3a_count						& grid_offs,
				ste3a_count						& grid_size);

void steawbi_update_calibration_data(
		const	ste3a_calibration_data	*calibration,
				ste3a_float				nvm_sensitivity_pedestal,
				nmxMatrix				*nvm_reference,
				nmxMatrix				*nvm_delta );

void steawbi_compute_calibration_adaptation(
				nmxMatrix	*nvm_delta,
				nmxMatrix	*ills_color_matrix_ref,
				nmxMatrix	*color_matrix_nvm_adaptation,
				nmxMatrix	*chromaticity_table_ref,
				nmxMatrix	*chromaticity_nvm_adaptation,
				nmxMatrix	*ills_color_matrix,
				nmxMatrix	*chromaticity_table,
				nmxMatrix	*ills_log_gain_pair,
				nmxMatrix	*locus_log_gain_pairs );

float steawbi_compute_covered_area_percent(
				nmxMatrix	*surfs_used_area);

#endif /* STEAWBI_H_ */
