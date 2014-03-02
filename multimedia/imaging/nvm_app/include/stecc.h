/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    stecc.h
 * \brief   STE Camera Calibration library interface
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#ifndef STECC_H_
#define STECC_H_

#define STECC_REVISION_MAJOR	1
#define STECC_REVISION_MINOR	1
#define STECC_REVISION_BUILD	120309

#define STECC_CHECK_CODE_RS_RG_VARIATION	(1L <<  0)
#define STECC_CHECK_CODE_RS_BG_VARIATION	(1L <<  1)
#define STECC_CHECK_CODE_LS_RR_LEVEL		(1L <<  2)
#define STECC_CHECK_CODE_LS_GR_LEVEL		(1L <<  3)
#define STECC_CHECK_CODE_LS_GB_LEVEL		(1L <<  4)
#define STECC_CHECK_CODE_LS_BB_LEVEL		(1L <<  5)
#define STECC_CHECK_CODE_LS_RR_AMPLITUDE	(1L <<  6)
#define STECC_CHECK_CODE_LS_GR_AMPLITUDE	(1L <<  7)
#define STECC_CHECK_CODE_LS_GB_AMPLITUDE	(1L <<  8)
#define STECC_CHECK_CODE_LS_BB_AMPLITUDE	(1L <<  9)
#define STECC_CHECK_CODE_LS_RR_SHAPE		(1L << 10)
#define STECC_CHECK_CODE_LS_GR_SHAPE		(1L << 11)
#define STECC_CHECK_CODE_LS_GB_SHAPE		(1L << 12)
#define STECC_CHECK_CODE_LS_BB_SHAPE		(1L << 13)
#define STECC_CHECK_CODE_RS_2D_VARIATION	(1L << 14)


enum stecc_bayer_phase
{
	stecc_bayer_phase_first_r,
	stecc_bayer_phase_first_gr,
	stecc_bayer_phase_first_gb,
	stecc_bayer_phase_first_b
};

enum stecc_channels_sensitivities_check_method
{
	stecc_channels_sensitivities_check_method_2x1d,
	stecc_channels_sensitivities_check_method_1x2d
};

struct stecc_bayer_image_descriptor
{
	unsigned int		width;
	unsigned int		height;
	int					stride;
	stecc_bayer_phase	phase;
	unsigned int		pedestal_value;
	unsigned int		clipping_value;
};

struct stecc_check_parameters
{
	stecc_channels_sensitivities_check_method	channels_sensitivities_check_method;
	float channels_sensitivities_tolerance;
	float lens_shading_min_average_minmaxratio;
	float lens_shading_min_sigma_minmaxratio;
	float lens_shading_min_normcrosscorr;
};

struct stecc_check_intermediates
{
	float ref_rg;
	float ref_bg;
	float cur_rg;
	float cur_bg;
	float var_rg;
	float var_bg;
        float var_2d;
    float ref_r;
	float ref_g;
    float ref_b;
	float cur_r;
	float cur_g;
    float cur_b;
};


/***** library versioning information ****************************************
*
* returns the library revision data, to be cross-checked against the
* STECC_REVISION_* macros
*
* outputs:
*
* major                          library major revision
* minor                          library minor revision
* revision                       library revision
*
*****************************************************************************/
void stecc_revision(unsigned int & major, unsigned int & minor, unsigned int & build);



/***** output and temporary buffers size query *******************************
*
* returns the size of the temporary and output buffers (in bytes)
*
* inputs:
*
* images_descriptor              raw Bayer images descriptor
*
*
* outputs:
*
* temporary_data_buffer_size     computed temporary data buffer size (bytes)
* calibration_data_buffer_size   computed calibration data buffer size (bytes)
*
*****************************************************************************/
void stecc_get_buffers_size(const stecc_bayer_image_descriptor & images_descriptor, unsigned int & temporary_data_buffer_size, unsigned int & calibration_data_buffer_size);



/***** calibration data computation ******************************************
*
* computes calibration data (dummy values when images_count == 0)
*
* inputs:
*
* images_descriptor              raw Bayer images descriptor
* images_array                   raw Bayer images pointers array
* images_count                   raw Bayer images count
*
*
* input/outputs:
*
* temporary_buffer               temporary data buffer
* calibration_data_buffer        calibration data buffer
*
*
* returns:                       success flag
*
*****************************************************************************/
bool stecc_compute(const stecc_bayer_image_descriptor & images_descriptor, void ** images_array, unsigned int images_count, void * temporary_data_buffer, void * calibration_data_buffer);


/***** quality check against reference data **********************************
*
* compare computed calibration data to reference ones in order to check the
* quality of the camera and enable sorting out of lower quality samples
*
* inputs:
*
* images_descriptor                 raw Bayer images descriptor
* check_parameters                  quality tolerances
* reference_calibration_data_buffer reference camera calibration data
* current_calibration_data_buffer   current camera calibration data
*
*
* input/outputs:
*
* code                              birfield for low quality codes
*                                   (_CHECK_CODE_* values)
*
*
* returns:                          good quality flag
*
*****************************************************************************/
bool stecc_check(const stecc_bayer_image_descriptor & images_descriptor, const stecc_check_parameters check_parameters, const void * reference_calibration_data_buffer, const void * current_calibration_data_buffer, stecc_check_intermediates & intermediates, unsigned int & code);


#endif /* STECC_H_ */
