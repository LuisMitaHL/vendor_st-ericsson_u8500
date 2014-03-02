/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    steafci.h
 * \brief   STE Auto Focus Control internal functions declarations
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#ifndef STEAFCI_H_
#define STEAFCI_H_

void steafci_initialize_tuning(									ste3a_afc_tuning					& tuning);

ste3a_bool steafci_full_search_best(					const	ste3a_afc_tuning					& tuning,
														const	ste3a_afc_interpolation				  interpolation,
														const	ste3a_count							  samples_count,
														const	steafc_focusstats_sample *			  samples_array,
														const	ste3a_focus_stats_descriptor		& stats_descriptor,
																ste3a_afc_intermediates				& intermediates,
																ste3a_normalized_lens_displacement	& best_displacement,
																ste3a_index							& best_sample_index);

ste3a_bool steafci_copy_stats(							const	ste3a_focus_stats_descriptor		& src_descriptor,
														const	ste3a_focusstats_high_f_val	*		  src_high_f_values,
														const	ste3a_focusstats_zero_f_val	*		  src_zero_f_values,
														const	ste3a_count	*						  src_dim_x_values,
														const	ste3a_count	*						  src_dim_y_values,
														const	ste3a_focus_stats_descriptor		& dst_descriptor,
																ste3a_normalized_sharpness	*		  dst_sharpness_values,
																ste3a_normalized_brightness	*		  dst_brightness_values);

void steafci_compute_stats_buffer(						const	ste3a_focus_stats_descriptor		& stats_descriptor,
														const	ste3a_focusstats_high_f_val *		  highf_frst,
														const	ste3a_focusstats_zero_f_val *		  zerof_frst,
														const	ste3a_count					*		  dimx_frst,
														const	ste3a_count					*		  dimy_frst,
																ste3a_focusstats_high_f_val *		& highf_base,
																ste3a_focusstats_zero_f_val *		& zerof_base,
																ste3a_count					*		& dimx_base,
																ste3a_count					*		& dimy_base,
																ste3a_count							& stats_offs,
																ste3a_count							& stats_size);

ste3a_normalized_sharpness steafci_apply_sharpness_noise_rule(
														const	ste3a_float							  input,
														const	ste3a_count							  pcount,
														const	ste3a_float							* pinput,
														const	ste3a_normalized_sharpness			* poutput);

ste3a_bool steafci_evaluate_change(						const	ste3a_float							  brgt_avgrt_threshold,
														const	ste3a_float							  shrp_avgrt_threshold,
														const	ste3a_float							  brgt_ergrt_threshold,
														const	ste3a_float							  shrp_ergrt_threshold,
														const	ste3a_normalized_correlation		  brgt_xcorr_threshold,
														const	ste3a_normalized_correlation		  shrp_xcorr_threshold,
														const	steafc_focusstats_sample			& samples_a,
														const	steafc_focusstats_sample			& samples_b,
														const	ste3a_focus_stats_descriptor		& stats_descriptor,
																ste3a_float							& change_eval_avg_sr,
																ste3a_float							& change_eval_avg_br,
																ste3a_float							& change_eval_erg_sr,
																ste3a_float							& change_eval_erg_br,
																ste3a_float							& change_eval_nxc_sf,
																ste3a_float							& change_eval_nxc_bf,
																ste3a_bool							& changed_avg_sr,
																ste3a_bool							& changed_avg_br,
																ste3a_bool							& changed_erg_sr,
																ste3a_bool							& changed_erg_br,
																ste3a_bool							& changed_nxc_sf,
																ste3a_bool							& changed_nxc_bf,
																ste3a_bool							& changed);

ste3a_bool steafci_evaluate_early_stop(					const	ste3a_afc_tuning					& tuning,
														const	ste3a_count							  samples_count,
														const	steafc_focusstats_sample *			  samples_array,
														const	ste3a_focus_stats_descriptor		& stats_descriptor,
																ste3a_afc_intermediates				& intermediates);

ste3a_gain steafci_apply_rule(							const	ste3a_float							  input,
														const	ste3a_count							  plimit,
														const	ste3a_count							  pcount,
														const	ste3a_float							* pinput,
														const	ste3a_gain							* pgain);

#endif /* STEAFCI_H_ */
