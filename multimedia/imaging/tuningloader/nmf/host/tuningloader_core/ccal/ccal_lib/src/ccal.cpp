/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    ccal.cpp
 * \brief   STE CCAL (camera calibration adaptation library) 8500V2 implementation
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#include "ccal.h"
#include "nmx.h"
#include "cam_drv_nvm_SMIApp.h"

bool ccal_convert_lsc_grids(const nmxMatrix *lsc_in,
							const uint32_t bayer_order,
					        const nmxMatrix *lsc_out);

bool ccal_convert_lsc_nvm(	const CAM_DRV_NVM_T	*nvm,
							const float         otp_pedestal,
							const int32_t       otp_pedestal_removed,
							const int32_t       otp_relative,
							const int32_t       otp_center_relative,
							const int32_t       otp_relative_orientation,
							const float         otp_relative_unity,
							const int32_t       otp_resolution,
							const int32_t       img_resolution,
							const int32_t       bayer_order,
							const nmxMatrix     *otp_red_green_ratio,
							const nmxMatrix     *otp_blue_green_ratio,
							const nmxMatrix     *lsc_sensor);

void ccal_revision(unsigned int & major,
				   unsigned int & minor,
				   unsigned int & build)
{
	major = CCAL_REVISION_MAJOR;
	minor = CCAL_REVISION_MINOR;
	build = CCAL_REVISION_BUILD;
}

bool ccal_generate_ste3a_calibration_data_lens_actuator(const void * raw_calibration_data_ptr, ste3a_calibration_data_lens_actuator & lens_actuator_calibration_data)
{
	if ( raw_calibration_data_ptr != NULL )
	{
		CAM_DRV_NVM_T & raw_calibration_data = *((CAM_DRV_NVM_T *) raw_calibration_data_ptr);
		
		bool Kanna = (raw_calibration_data.sens_resolution.width == 2608) && (raw_calibration_data.sens_resolution.height == 1960) && (raw_calibration_data.af_data.total_positions_h == 5) && (raw_calibration_data.af_data.availability_bitmask_h == 0x0139);
		
		if (Kanna)
		{
			// WARNING: this is specific to the Kanna/IMX072 camera module, whose LLCD implementation is not fully compliant with the specifications

			lens_actuator_calibration_data.mec_inf = raw_calibration_data.af_data.positions_horizontal[0];
			lens_actuator_calibration_data.nom_inf = raw_calibration_data.af_data.positions_horizontal[1];
			lens_actuator_calibration_data.nom_mac = raw_calibration_data.af_data.positions_horizontal[3];
			lens_actuator_calibration_data.mec_mac = raw_calibration_data.af_data.positions_horizontal[4];
		}
		else
		{
			// WARNING: not yet implemented for the general case

			lens_actuator_calibration_data.mec_inf = 0;
			lens_actuator_calibration_data.nom_inf = 0;
			lens_actuator_calibration_data.nom_mac = 1023;
			lens_actuator_calibration_data.mec_mac = 1023;
		}
	}
	else
	{
		lens_actuator_calibration_data.mec_inf = 0;
		lens_actuator_calibration_data.nom_inf = 0;
		lens_actuator_calibration_data.nom_mac = 1023;
		lens_actuator_calibration_data.mec_mac = 1023;
	}

	return true;
}

bool ccal_generate_ste3a_calibration_data(const void *nvm_buffer,
										  ste3a_calibration_data *ste3a_cal)
{
	uint32_t i;
	CAM_DRV_NVM_T * nvm = (CAM_DRV_NVM_T *) nvm_buffer;

	/* zero out output by default */
	memset((void*)ste3a_cal, 0x0, sizeof(*ste3a_cal));

	if ( nvm != NULL )
	{
		/* fill number of sensitivity measures */
		ste3a_cal->sensitivity.count = nvm->sensitivity_data.sensitivity_colour_temps;

		/* visit available calibration illuminants */
		CAM_DRV_NVM_SENSITIVITY_DATA_T *sens_data = (CAM_DRV_NVM_SENSITIVITY_DATA_T *)((char*)&nvm->sensitivity_data + nvm->sensitivity_data.sens_offset);	/* local offset used */
		for(i = 0; i < nvm->sensitivity_data.sensitivity_colour_temps; i++) {

			/* fill in chromaticity coordinates */

			ste3a_cal->sensitivity.data[i].cie_x = ((float)sens_data[i].x_value_x1000) / 1000.0f + 0.300f;
			ste3a_cal->sensitivity.data[i].cie_y = ((float)sens_data[i].y_value_x1000) / 1000.0f + 0.300f;

			/* fill in absolute channel sensitivity measures */
			ste3a_cal->sensitivity.data[i].rr = sens_data[i].sens.r;
			ste3a_cal->sensitivity.data[i].gr = sens_data[i].sens.gr;
			ste3a_cal->sensitivity.data[i].gb = sens_data[i].sens.gb;
			ste3a_cal->sensitivity.data[i].bb = sens_data[i].sens.b;
		}
	}

	ste3a_cal->valid = ccal_generate_ste3a_calibration_data_lens_actuator(nvm_buffer, ste3a_cal->lens_actuator);
	return ste3a_cal->valid;
	
}

uint32_t ccal_get_buffer_size_lsc_grids(const void *buffer_in,
										const uint32_t buffer_in_size)
{
	nmxMatrix grid_shade_flat_table;
	nmxMatrix grid_shade_target;
	nmxMatrix otp_ill_pos_comb;
	nmxMatrix otp_mean;
	nmxMatrix otp_black;
	nmxMatrix otp_coords_x;
	nmxMatrix otp_coords_y;
	nmxMatrix otp_pedestal;
	nmxMatrix otp_pedestal_removed;
	nmxMatrix otp_relative;
	nmxMatrix otp_center_relative;
	nmxMatrix otp_relative_orientation;
	nmxMatrix otp_relative_unity;
	nmxMatrix img_resolution;
	nmxMatrix otp_resolution;
	nmxMatrix otp_red_green_ratio;
	nmxMatrix otp_blue_green_ratio;
	nmxMatrix bayer_order;
	nmxMatrix otp_sensitivity_mean;
	nmxMatrix ill_cast_pos;
	nmxMatrix *matset[20];
	nmxSize_t d0, d1, d2, d3;
	nmxSize_t gd0, gd1, gd2, gd3;
	nmxSize_t buff_tmp_size = 0;

	matset[0]  = &grid_shade_flat_table;
	matset[1]  = &grid_shade_target;
	matset[2]  = &otp_ill_pos_comb;
	matset[3]  = &otp_mean;
	matset[4]  = &otp_black;
	matset[5]  = &otp_coords_x;
	matset[6]  = &otp_coords_y;
	matset[7]  = &otp_pedestal;
	matset[8]  = &otp_pedestal_removed;
	matset[9]  = &otp_relative;
	matset[10] = &otp_center_relative;
	matset[11] = &otp_relative_orientation;
	matset[12] = &otp_relative_unity;
	matset[13] = &otp_resolution;
	matset[14] = &img_resolution;
	matset[15] = &otp_red_green_ratio;
	matset[16] = &otp_blue_green_ratio;
	matset[17] = &bayer_order;
	matset[18] = &otp_sensitivity_mean;
	matset[19] = &ill_cast_pos;

	/* extraxtion of nmxMatrixes from buffer_in */
	nmxDecodeSet_a((uint8_t*)buffer_in, (uint32_t)buffer_in_size, matset, 20);

	/* grid size */
	nmxGetMatrixP4D_f(&grid_shade_flat_table, &gd0, &gd1, &gd2, &gd3);
	/* otp size */
	nmxGetMatrixP4D_f(&otp_mean, &d0, &d1, &d2, &d3);

	/* otp_sensor size */
	buff_tmp_size += d0 * d1 * d2 * d3 * sizeof(float);
	/* otp_mean_copy size */
	buff_tmp_size += d0 * d1 * d2 * d3 * sizeof(float);
	/* otp_coords_x_copy size */
	buff_tmp_size += d1 * sizeof(float);
	/* otp_coords_y_copy size */
	buff_tmp_size += d0 * sizeof(float);
	/* otp_centers size */
	buff_tmp_size += d2 * d3 * sizeof(float);
	/* otp_ch size */
	buff_tmp_size += d0 * d1 * sizeof(float);
	/* otp_gi size */
	buff_tmp_size += d0 * d1 * sizeof(float);
	/* otp_sm size */
	buff_tmp_size += d0 * d1 * sizeof(float);
	/* otp_mm size */
	buff_tmp_size += d0 * d1 * sizeof(float);
	/* adapted_grid_shade size */
	buff_tmp_size += gd0 * gd1 * gd2 * gd3 * sizeof(float);
	/* grid_rat size */
	buff_tmp_size += gd0 * gd1 * sizeof(float);
	/* grid_tmp size */
	buff_tmp_size += gd0 * gd1 * sizeof(float);
	/* grid_coords_x_copy size */
	buff_tmp_size += gd1 * sizeof(float);
	/* grid_coords_y_copy size */
	buff_tmp_size += gd0 * sizeof(float);
	/* boundary size */
	buff_tmp_size += CCAL_ISP_NUM_BAYER_CHANNELS * sizeof(float);
	/* resample support (resdownsup) size */
	buff_tmp_size += gd1 * d0 * sizeof(float);
	/* resample support (resupsup) size */
	buff_tmp_size += gd0 * d1 * sizeof(float);

	return(buff_tmp_size);
}


bool ccal_calibrate_lsc_grids(	const void *buffer_in,
								const uint32_t buffer_in_size,
								const void *nvm_buffer,
								const void *temp_buf,
								const nmxMatrix *lsc_out,
								const nmxMatrix *lsc_cast)
{
	CAM_DRV_NVM_T * nvm = (CAM_DRV_NVM_T *) nvm_buffer;
	nmxMatrix grid_shade_flat_table;
	nmxMatrix grid_shade_target;
	nmxMatrix grid_shade_ch;
	nmxMatrix otp_ill_pos_comb;
	nmxMatrix otp_mean;
	nmxMatrix otp_mean_copy;
	nmxMatrix otp_black;
	nmxMatrix otp_coords_x;
	nmxMatrix otp_coords_x_copy;
	nmxMatrix otp_coords_y;
	nmxMatrix otp_coords_y_copy;
	nmxMatrix otp_pedestal;
	nmxMatrix otp_pedestal_removed;
	nmxMatrix otp_relative;
	nmxMatrix otp_center_relative;
	nmxMatrix otp_relative_orientation;
	nmxMatrix otp_relative_unity;
	nmxMatrix img_resolution;
	nmxMatrix otp_resolution;
	nmxMatrix otp_red_green_ratio;
	nmxMatrix otp_blue_green_ratio;
	nmxMatrix bayer_order;
	nmxMatrix otp_sensitivity_mean;
	nmxMatrix ill_cast_pos;
	nmxMatrix *matset[20];
	nmxMatrix otp_sensor;
	nmxMatrix otp_centers;	/* 4D matrix (1,1,nChannes,nOtpIlls) */
	nmxMatrix otp_ch;
	nmxMatrix otp_gi;
	nmxMatrix otp_sm;
	nmxMatrix otp_mm;
	nmxMatrix adapted_grid_shade;
	nmxMatrix adapted_grid_shade_ch;
	nmxMatrix grid_rat;
	nmxMatrix grid_tmp;
	nmxMatrix grid_coords_x;
	nmxMatrix grid_coords_y;
	nmxMatrix tmp_crop;
	nmxMatrix scalar;
	nmxMatrix boundary;
	nmxMatrix resdownsup;
	nmxMatrix resupsup;
	nmxSize_t offsets[4];
	nmxSize_t size[4];
	nmxSize_t otp_size[4];
	nmxSize_t d0, d1, d2, d3, dp1, noil1;
	nmxSize_t gd0, gd1, gd2, gd3;
	nmxStride_t sk0, sk1;
	float *fdata;
	nmxMem mem;
	uint32_t temp_buf_size;
	float *icp_p, * icp_p0;
	uint32_t idxi, nills, idx, nch, idxoi;

	/* compute the size of temp_buf */
	temp_buf_size = ccal_get_buffer_size_lsc_grids(buffer_in, buffer_in_size);
	nmxMemInit(&mem, temp_buf_size, (void*)temp_buf);

	matset[0]  = &grid_shade_flat_table;
	matset[1]  = &grid_shade_target;
	matset[2]  = &otp_ill_pos_comb;
	matset[3]  = &otp_mean;
	matset[4]  = &otp_black;
	matset[5]  = &otp_coords_x;
	matset[6]  = &otp_coords_y;
	matset[7]  = &otp_pedestal;
	matset[8]  = &otp_pedestal_removed;
	matset[9]  = &otp_relative;
	matset[10] = &otp_center_relative;
	matset[11] = &otp_relative_orientation;
	matset[12] = &otp_relative_unity;
	matset[13] = &otp_resolution;
	matset[14] = &img_resolution;
	matset[15] = &otp_red_green_ratio;
	matset[16] = &otp_blue_green_ratio;
	matset[17] = &bayer_order;
	matset[18] = &otp_sensitivity_mean;
	matset[19] = &ill_cast_pos;

	/* extraxtion of ntmpmxMatrixes from buffer_in */
	nmxDecodeSet_a((uint8_t*)buffer_in, (uint32_t)buffer_in_size, matset, 20);

	nmxTranspose_a(&ill_cast_pos);
	nmxCopy_a(&ill_cast_pos, (nmxMatrix*)lsc_cast);

	if(nvm == NULL || otp_mean.dims[0] == 0) {
		nmxCopyMatrixD_a(&grid_shade_flat_table, &adapted_grid_shade);
		nmxAllocMatrix(&mem, &adapted_grid_shade);
		nmxCopy_a(&grid_shade_flat_table, &adapted_grid_shade);
	} else {
		/* prepare otp_sensor to store nvm lens shading float data */
		nmxCopyMatrixD_a(&otp_mean, &otp_sensor);
		nmxAllocMatrix(&mem, &otp_sensor);

		/* grid size */
		nmxGetMatrixP4D_f(&grid_shade_flat_table, &gd0, &gd1, &gd2, &gd3);
		/* otp size */
		nmxGetMatrixP4D_f(&otp_mean, &d0, &d1, &d2, &d3);

		nch = gd2;
		nills = gd3;

		/* initialization of otp_sensor from nmv input data */
		ccal_convert_lsc_nvm(nvm,
			*(float*)otp_pedestal.pdata,
			*(int32_t*)otp_pedestal_removed.pdata,
			*(int32_t*)otp_relative.pdata,
			*(int32_t*)otp_center_relative.pdata,
			*(int32_t*)otp_relative_orientation.pdata,
			*(float*)otp_relative_unity.pdata,
			*(int32_t*)otp_resolution.pdata,
			*(int32_t*)img_resolution.pdata,
			*(int32_t*)bayer_order.pdata,
			&otp_red_green_ratio,
			&otp_blue_green_ratio,
			&otp_sensor);

		/* create copy of otp_mean in order to preserve input buffer */
		nmxCopyMatrixD_a(&otp_mean, &otp_mean_copy);
		nmxAllocMatrix(&mem, &otp_mean_copy);
		nmxCopy_a(&otp_mean, &otp_mean_copy);

		/* create copy of otp_coords_x in order to preserve input buffer */
		nmxCopyMatrixD_a(&otp_coords_x, &otp_coords_x_copy);
		nmxAllocMatrix(&mem, &otp_coords_x_copy);
		nmxCopy_a(&otp_coords_x, &otp_coords_x_copy);

		/* create copy of otp_coords_y in order to preserve input buffer */
		nmxCopyMatrixD_a(&otp_coords_y, &otp_coords_y_copy);
		nmxAllocMatrix(&mem, &otp_coords_y_copy);
		nmxCopy_a(&otp_coords_y, &otp_coords_y_copy);

		/* remove black offset */
		nmxSubtract_f(&otp_sensor, &otp_black, &otp_sensor);
		/* initialization of otp_centers as part of otp_sensor */
		offsets[0] = (d0-1)>>1;
		offsets[1] = (d1-1)>>1;
		offsets[2] = 0;
		offsets[3] = 0;
		size[0] = 1;
		size[1] = 1;
		size[2] = d2;
		size[3] = d3;
		nmxCropMatrix_a(&otp_sensor, offsets, size, &tmp_crop);
		nmxCopyMatrixD_a(&tmp_crop, &otp_centers);
		nmxAllocMatrix(&mem, &otp_centers);
		nmxCopy_a(&tmp_crop, &otp_centers);
		/* normalize shading */
		nmxDivideNan0_f(&otp_sensor, &otp_centers, &otp_sensor);
		// nmxSave_a(&otp_sensor, "OtpSF.nmx", NULL);

		/* remove black offset */
		nmxSubtract_f(&otp_mean, &otp_black, &otp_mean_copy);
		/* initialization of otp_centers as part of otp_mean */
		nmxCropMatrix_a(&otp_mean_copy, offsets, size, &tmp_crop);
		nmxCopy_a(&tmp_crop, &otp_centers);
		/* normalize shading */
		nmxDivideNan0_f(&otp_mean_copy, &otp_centers, &otp_mean_copy);
		// nmxSave_a(&otp_mean_copy, "OtpMF.nmx", NULL);

		/* init matrixes for intermidiate data */
		nmxSetMatrixP2D_f(&otp_ch, (float*)0, d0, d1);
		nmxAllocMatrix(&mem, &otp_ch);
		nmxSetMatrixP2D_f(&otp_gi, (float*)0, d0, d1);
		nmxAllocMatrix(&mem, &otp_gi);
		nmxSetMatrixP2D_f(&otp_sm, (float*)0, d0, d1);
		nmxAllocMatrix(&mem, &otp_sm);
		nmxSetMatrixP2D_f(&otp_mm, (float*)0, d0, d1);
		nmxAllocMatrix(&mem, &otp_mm);
		/* output in float */
		nmxCopyMatrixD_a(&grid_shade_flat_table, &adapted_grid_shade);
		nmxAllocMatrix(&mem, &adapted_grid_shade);
		nmxCopy_a(&grid_shade_flat_table, &adapted_grid_shade);
		nmxSetMatrixP2D_f(&grid_rat, (float*)0, gd0, gd1);
		nmxAllocMatrix(&mem, &grid_rat);
		nmxSetMatrixP2D_f(&grid_tmp, (float*)0, gd0, gd1);
		nmxAllocMatrix(&mem, &grid_tmp);
		/* output grid x coords */
		nmxSetMatrixP2D_f(&grid_coords_x, (float*)0, gd1, 1);
		nmxAllocMatrix(&mem, &grid_coords_x);
		nmxEnumerate_if(0, &grid_coords_x);
		// nmxSave_a(&grid_coords_x, "GridCoordsX.nmx", NULL);
		/* output grid y coords */
		nmxSetMatrixP2D_f(&grid_coords_y, (float*)0, gd0, 1);
		nmxAllocMatrix(&mem, &grid_coords_y);
		nmxEnumerate_if(0, &grid_coords_y);
		// nmxSave_a(&grid_coords_y, "GridCoordsY.nmx", NULL);
		icp_p = nmxGetMatrixP2DS_f(&otp_ill_pos_comb, &noil1, &dp1, &sk0, &sk1);
		/* boundary 4 elements vector */
		nmxSetMatrixP2D_f(&boundary, (float*)0, 4, 1);
		nmxAllocMatrix(&mem, &boundary);
		fdata = (float*)boundary.pdata;
		fdata[0] = 0.0f;
		fdata[1] = 0.0f;
		fdata[2] = (float)(gd0-1);
		fdata[3] = (float)(gd1-1);
		/* resdownsup initialization */
		nmxSetMatrixP2D_f(&resdownsup, (float*)0, d0, gd1);
		nmxAllocMatrix(&mem, &resdownsup);
		/* resupsup initialization */
		nmxSetMatrixP2D_f(&resupsup, (float*)0, gd0, d1);
		nmxAllocMatrix(&mem, &resupsup);

		offsets[0] = 0;
		offsets[1] = 0;
		size[0] = gd0;
		size[1] = gd1;
		size[2] = 1;
		size[3] = 1;
		otp_size[0] = d0;
		otp_size[1] = d1;
		otp_size[2] = 1;
		otp_size[3] = 1;
		for(idxi=0; idxi<nills; idxi++) {
			offsets[3] = idxi;
			icp_p0 = icp_p + sk1 * idxi;
			for(idx=0; idx<nch; idx++) {
				offsets[2] = idx;
				nmxCropMatrix_a(&grid_shade_flat_table, offsets, size, &grid_shade_ch);
				/* computation of otp_gi */

				nmxResampleGridBicubic_fu(&grid_shade_ch, &grid_coords_y, &grid_coords_x, &otp_coords_y, &otp_coords_x, 2, &otp_gi, &resdownsup);
				/* combine OTP illuminants to aprossimate current characterization illuminant */
				nmxReciprocal_f(&otp_gi, &otp_sm);	/* shading from otp_gi antivignetting function */
				nmxSetScalar_f(&scalar, &(icp_p0[dp1-1]));
				nmxMultiply_f(&otp_sm, &scalar, &otp_sm);
				nmxCopy_a(&otp_sm, &otp_mm);
				for(idxoi=0; idxoi<d3; idxoi++) {
					nmxSetScalar_f(&scalar, &(icp_p0[idxoi]));
					offsets[3] = idxoi;
					/* compute correction grid for current illuminant and current channel */
					nmxCropMatrix_a(&otp_sensor, offsets, otp_size, &tmp_crop);
					nmxMultiply_f(&tmp_crop, &scalar, &otp_ch);
					nmxAdd_f(&otp_sm, &otp_ch, &otp_sm);
					/* compute calibrated grid for current illuminant and current channel */
					nmxCropMatrix_a(&otp_mean_copy, offsets, otp_size, &tmp_crop);
					nmxMultiply_f(&tmp_crop, &scalar, &otp_ch);
					nmxAdd_f(&otp_mm, &otp_ch, &otp_mm);
				}
				nmxExtendBoundaryGrid_f(&otp_mm, &otp_coords_y, &otp_coords_x, &boundary, &otp_mm, &otp_coords_y_copy, &otp_coords_x_copy);
				nmxResampleGridBicubic_fu(&otp_mm, &otp_coords_y_copy, &otp_coords_x_copy, &grid_coords_y, &grid_coords_x, 2, &grid_rat, &resupsup);
				nmxExtendBoundaryGrid_f(&otp_sm, &otp_coords_y, &otp_coords_x, &boundary, &otp_sm, &otp_coords_y_copy, &otp_coords_x_copy);
				nmxResampleGridBicubic_fu(&otp_sm, &otp_coords_y_copy, &otp_coords_x_copy, &grid_coords_y, &grid_coords_x, 2, &grid_tmp, &resupsup);
				nmxDivideNan0_f(&grid_rat, &grid_tmp, &grid_rat);
				offsets[3] = idxi;
				nmxCropMatrix_a(&adapted_grid_shade, offsets, size, &adapted_grid_shade_ch);
				nmxMultiply_f(&adapted_grid_shade_ch, &grid_rat, &adapted_grid_shade_ch);
			}
		}
	}
	// nmxSave_a(&adapted_grid_shade, "GridShadeAdaptF.nmx", NULL);

	nmxMultiply_f(&adapted_grid_shade, &grid_shade_target ,&adapted_grid_shade);

	ccal_convert_lsc_grids(&adapted_grid_shade, *(const uint32_t*)(bayer_order.pdata), lsc_out);

	return true;
}


bool ccal_convert_lsc_grids(const nmxMatrix *lsc_in,
							const uint32_t bayer_order,
					        const nmxMatrix *lsc_out)
{
	nmxMatrix scalar;
	nmxSize_t d0, d1, d2, d3;
	nmxSize_t gd0, gd1, gd2, gd3;
	nmxStride_t sk0, sk1, sk2, sk3;
	nmxStride_t osk0, osk1, osk2, osk3;
	float one, max_value, *fd;
	uint32_t *ud, cast;
	int32_t idxs[CCAL_ISP_NUM_BAYER_CHANNELS];

	/* bayer_order: 0 = GreenRed, 1 = Red, 2 = Blue, 3 = GreenBlue */
	switch(bayer_order) {
		/* 0 no flips */
		case 0:
			idxs[0] = 0;
			idxs[1] = 1;
			idxs[2] = 2;
			idxs[3] = 3;
			break;
		/* 1 flip left-right */
		case 1:
			idxs[0] = 1;
			idxs[1] = 0;
			idxs[2] = 3;
			idxs[3] = 2;
			nmxFlipMatrixD_a((nmxMatrix*)lsc_in,1);
			break;
		/* 2 flip up-down */
		case 2:
			idxs[0] = 2;
			idxs[1] = 3;
			idxs[2] = 0;
			idxs[3] = 1;
			nmxFlipMatrixD_a((nmxMatrix*)lsc_in,0);
			break;
		/* 3 flip left-right and flip up-down (180 rotation) */
		case 3:
			idxs[0] = 3;
			idxs[1] = 2;
			idxs[2] = 1;
			idxs[3] = 0;
			nmxFlipMatrixD_a((nmxMatrix*)lsc_in,0);
			nmxFlipMatrixD_a((nmxMatrix*)lsc_in,1);
			break;
		/* 0 no flips */
		default:
			idxs[0] = 0;
			idxs[1] = 1;
			idxs[2] = 2;
			idxs[3] = 3;
			break;
	}
	/* value of 1 in fixed point */
	one = (float)(1<<CCAL_ISP_GRIDIRON_FRACT_BITS);
	nmxSetScalar_f(&scalar,&one);
	/* transform float data values in fixed point values */
	nmxMultiply_f((nmxMatrix*)lsc_in, &scalar, (nmxMatrix*)lsc_in);
	nmxMax2_f((nmxMatrix*)lsc_in, &scalar, (nmxMatrix*)lsc_in);
	nmxSubtract_f((nmxMatrix*)lsc_in, &scalar, (nmxMatrix*)lsc_in);
	nmxRound_f((nmxMatrix*)lsc_in, (nmxMatrix*)lsc_in);
	/* limit to the max fixed point value */
	max_value = (float)((1<<CCAL_ISP_GRIDIRON_DATA_WIDTH) - 1);
	nmxSetScalar_f(&scalar,&max_value);
	nmxMin2_f((nmxMatrix*)lsc_in, &scalar, (nmxMatrix*)lsc_in);

	fd = nmxGetMatrixP4DS_f((nmxMatrix*)lsc_in, &gd0, &gd1, &gd2, &gd3, &sk0, &sk1, &sk2, &sk3);
	nmxSetMatrixP4D_u((nmxMatrix*)lsc_out, (uint32_t*)(lsc_out->pdata), gd1, 2, gd0, gd3);
	ud = nmxGetMatrixP4DS_u((nmxMatrix*)lsc_out, &d0, &d1, &d2, &d3, &osk0, &osk1, &osk2, &osk3);

	for(cast=0; cast<gd3; cast++) {
		uint32_t csk, ocsk, yi;

		csk  = cast*sk3;
		ocsk = cast*osk3;
		for(yi=0; yi<gd0; yi++) {
			uint32_t ysk, oysk, xi;

			ysk  = yi*sk0;
			oysk = yi*osk2;
			for(xi=0; xi<gd1; xi++) {
				int32_t val, xsk, oxsk, ii;

				xsk  = xi*sk1;
				oxsk = xi*osk0;
				/* GreenRed value */
				ii = csk + ysk + xsk + idxs[0]*sk2;
				val  = (uint32_t)fd[ii] << CCAL_ISP_GRIDIRON_DATA_WIDTH; 
				/* Red value */
				ii = csk + ysk + xsk + idxs[1]*sk2;
				val |= (uint32_t)fd[ii];
				/* GreenRed-Red packed value */
				ii = ocsk + oysk + oxsk;
				ud[ii] = val;
				/* Blue value */
				ii = csk + ysk + xsk + idxs[2]*sk2;
				val  = (uint32_t)fd[ii] << CCAL_ISP_GRIDIRON_DATA_WIDTH; 
				/* GreenBlue value */
				ii = csk + ysk + xsk + idxs[3]*sk2;
				val |= (uint32_t)fd[ii];
				/* Blue-GreenBlue packed value */
				ii = ocsk + oysk + oxsk + osk1;
				ud[ii] = val;
			}
		}
	}

	return true;
}


bool ccal_convert_lsc_nvm(	const CAM_DRV_NVM_T *nvm,
							const float         otp_pedestal,
							const int32_t       otp_pedestal_removed,
							const int32_t       otp_relative,
							const int32_t       otp_center_relative,
							const int32_t       otp_relative_orientation,
							const float         otp_relative_unity,
							const int32_t       otp_resolution,
							const int32_t       img_resolution,
							const int32_t       bayer_order,
							const nmxMatrix     *otp_red_green_ratio,
							const nmxMatrix     *otp_blue_green_ratio,
							const nmxMatrix     *lsc_sensor)
{
	nmxSize_t dy, dx, d2, d3;
	nmxStride_t sky, skx, sk2, sk3;
	uint32_t nvm_table_idx;
	uint32_t cx, cy, center_offset;
	uint32_t otp_ch_order[4];
	uint32_t char_ch_order[4];
	float *fd;
	float *rgr, *bgr;
	float default_green;
	CAM_DRV_NVM_LS_TABLE_T *lsc_table;
	CAM_DRV_NVM_LSC_DATA_T *lsc_data;
	bool err = false;

	/* Default value of green when used reference sensor */
	default_green = 200.0f * (float)(1<<(img_resolution-8));

	rgr = (float*)otp_red_green_ratio->pdata;
	bgr = (float*)otp_blue_green_ratio->pdata;

	/* channels order in nvm data */
	otp_ch_order[0] = 0;
	otp_ch_order[1] = 1;
	otp_ch_order[2] = 3;
	otp_ch_order[3] = 2;

	/* bayer_order first pixel is: 0 = GreenRed, 1 = Red, 2 = Blue, 3 = GreenBlue */
	switch(bayer_order) {
		case 0:
			char_ch_order[0] = 1;
			char_ch_order[1] = 0;
			char_ch_order[2] = 3;
			char_ch_order[3] = 2;
			break;
		case 1:
			char_ch_order[0] = 0;
			char_ch_order[1] = 1;
			char_ch_order[2] = 2;
			char_ch_order[3] = 3;
			break;
		case 2:
			char_ch_order[0] = 3;
			char_ch_order[1] = 2;
			char_ch_order[2] = 1;
			char_ch_order[3] = 0;
			break;
		case 3:
			char_ch_order[0] = 2;
			char_ch_order[1] = 3;
			char_ch_order[2] = 0;
			char_ch_order[3] = 1;
			break;
	}

	fd = nmxGetMatrixP4DS_f((nmxMatrix*)lsc_sensor, &dy, &dx, &d2, &d3, &sky, &skx, &sk2, &sk3);
	if(nvm->ls_data.ls_table_num != d3) {
		err = true;
		return err;
	}
	/* check grid size */
	for(nvm_table_idx = 0; nvm_table_idx < nvm->ls_data.ls_table_num; nvm_table_idx++) {
		lsc_table = &((CAM_DRV_NVM_LS_TABLE_T *)((char*)nvm + nvm->offset_com_tables))[nvm_table_idx];
		if(lsc_table->y_division != dy || lsc_table->x_division != dx) err = true;
	}
	if(err) return err;
	
	cx = dx >> 1;
	cy = dy >> 1;
	center_offset = (cy * dx) + cx;
	for(nvm_table_idx = 0; nvm_table_idx < nvm->ls_data.ls_table_num; nvm_table_idx++) {
		uint32_t i, j, ch;
		nmxSize_t isk;

		lsc_table = &((CAM_DRV_NVM_LS_TABLE_T *)((char*)nvm + nvm->offset_com_tables))[nvm_table_idx];
		lsc_data = (CAM_DRV_NVM_LSC_DATA_T *)((char*)lsc_table + lsc_table->lsc_data_offset);	

		isk = nvm_table_idx * sk3;
		
		for(ch = 0; ch < CCAL_ISP_NUM_BAYER_CHANNELS; ch++) {
			nmxSize_t chsk;
			float refgain, cvf, toimg;
			uint16_t center_value;

			chsk = char_ch_order[ch] * sk2;
			/* save center value in case of relative */
			center_value = ((uint16_t *)(&lsc_data[center_offset]))[otp_ch_order[ch]];
			toimg = 1 / (float)(1 << (lsc_table->center_bit_depth - img_resolution));
			if(otp_center_relative) {
				/* center relative to reference sensor */
				refgain = 1.0f; /* greens channels */
				if(ch==0) refgain = rgr[nvm_table_idx]; /* red channel */
				if(ch==3) refgain = bgr[nvm_table_idx]; /* blue channel */
				cvf = (float)center_value / (float)(1 << otp_resolution) * refgain * default_green;
			} else {
				/* center absolute values */
				cvf = (float)center_value * toimg;
				/* remove the pedestal in case it is present */
				if(otp_pedestal_removed == 0) cvf -= otp_pedestal;
			}
			for(i = 0; i < dy; i++) {
				nmxSize_t ysk;

				ysk = i * sky;
				for(j = 0; j < dx; j++) {
					uint32_t offset;
					nmxSize_t xsk;
					uint16_t value;

					xsk = j * skx;
					offset = (dx * i) + j;
					value = ((uint16_t *)(&lsc_data[offset]))[otp_ch_order[ch]];
					if(otp_relative) {
						if(i == cy && j == cx) {
							value = (uint16_t)otp_relative_unity;
						}
						fd[isk + ysk + xsk + chsk] = (float)value / otp_relative_unity * cvf + otp_pedestal;
					} else {
						if(otp_pedestal_removed) fd[isk + ysk + xsk + chsk] = (float)value * toimg + otp_pedestal;
						else fd[isk + ysk + xsk + chsk] = (float)value * toimg;
					}
				}
			}
		}
	}

	return(err);
}

