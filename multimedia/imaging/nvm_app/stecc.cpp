/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    stecc.cpp
 * \brief   STE Camera Calibration library
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#include "stecc.h"

typedef unsigned char	uint8;
typedef unsigned short	uint16;
typedef unsigned int	uint32;

#include "cam_drv_nvm_SMIApp.h"
#include <string.h>
#include <math.h>
#ifdef STECC_DEBUG
#	include <stdio.h>
#endif

#define STECC_SHADING_GRID_W				9
#define STECC_SHADING_GRID_H				7

#define STECC_ILLUMINANT_CHROMATICITY_X		0.333f
#define STECC_ILLUMINANT_CHROMATICITY_Y		0.333f

struct stecc_calibration_data
{
	CAM_DRV_NVM_T					header;

	CAM_DRV_NVM_SENSITIVITY_DATA_T	sensitivity_data;

	unsigned char					shading_tables_count;

	CAM_DRV_NVM_LS_TABLE_T			shading_header;
	CAM_DRV_NVM_LSC_DATA_T			shading_data[STECC_SHADING_GRID_W * STECC_SHADING_GRID_H];
};

void stecc_revision(unsigned int & major, unsigned int & minor, unsigned int & build)
{
	major = STECC_REVISION_MAJOR;
	minor = STECC_REVISION_MINOR;
	build = STECC_REVISION_BUILD;
}

void stecc_get_buffers_size(const stecc_bayer_image_descriptor & images_descriptor, unsigned int & temporary_data_buffer_size, unsigned int & calibration_data_buffer_size)
{
	temporary_data_buffer_size = 0;
	
	temporary_data_buffer_size += images_descriptor.width * images_descriptor.height * sizeof(unsigned short);			// 16-bpp unpacked raw Bayer image
	temporary_data_buffer_size += 4 * (images_descriptor.width / 2) * (images_descriptor.height / 2) * sizeof(float);	// float colour planes image
	temporary_data_buffer_size += 4 * STECC_SHADING_GRID_W * STECC_SHADING_GRID_H * sizeof(float);						// float colour planes grid

	calibration_data_buffer_size = sizeof(stecc_calibration_data);
}

void stecc_internal_raw12_to_raw16(unsigned char * bi, unsigned char * bo, int si, int so, int mc, int mr)
{
	for (int r = 0; r < mr * 2; r++)
	{
		for (int c = 0; c < mc; c++)
		{
			unsigned int i1 = *bi++;
			unsigned int i2 = *bi++;
			unsigned int i3 = *bi++;

			unsigned short int p1 = (unsigned short int) ((i1 << 4) | ((i3 & 0x0F) >> 0));
			unsigned short int p2 = (unsigned short int) ((i2 << 4) | ((i3 & 0xF0) >> 4));

			*((unsigned short int *) bo) = p1;

			bo += 2;

			*((unsigned short int *) bo) = p2;

			bo += 2;
		}

		bi += si - 3 * mc;
		bo += so - 4 * mc;
	}
}

bool stecc_compute(const stecc_bayer_image_descriptor & images_descriptor, void ** images_array, unsigned int images_count, void * temporary_data_buffer, void * calibration_data_buffer)
{
	bool retval = false;

	if (calibration_data_buffer != NULL)
	{
		stecc_calibration_data & calibration_data = *((stecc_calibration_data *) calibration_data_buffer);

		memset(&calibration_data, 0, sizeof(stecc_calibration_data));

		unsigned int cell_w = (images_descriptor.width  / 2) / STECC_SHADING_GRID_W;
		unsigned int cell_h = (images_descriptor.height / 2) / STECC_SHADING_GRID_H;

		unsigned int cell_size = (cell_w < cell_h) ? cell_w : cell_h;

#		ifdef STECC_DEBUG

		{
			printf("%d\t%d\n", cell_size * 2, cell_size * 2);

			printf("\n");

			for (unsigned int gx_index = 0; gx_index < STECC_SHADING_GRID_W; gx_index++)
			{
				unsigned int start_x = ((images_descriptor.width / 2) - (cell_size * STECC_SHADING_GRID_W)) * gx_index / (STECC_SHADING_GRID_W - 1) + gx_index * cell_size;

				printf("%d\t", start_x * 2);
			}

			printf("\n");

			for (unsigned int gy_index = 0; gy_index < STECC_SHADING_GRID_H; gy_index++)
			{
				unsigned int start_y = ((images_descriptor.height / 2) - (cell_size * STECC_SHADING_GRID_H)) * gy_index / (STECC_SHADING_GRID_H - 1) + gy_index * cell_size;

				printf("%d\t", start_y * 2);
			}

			printf("\n");

			printf("\n");
		}

#		endif

		if (images_count == 0)
		{
			// fake measured average value

			unsigned int fake_value = (images_descriptor.pedestal_value + images_descriptor.clipping_value) / 2;

			// sensor array size

			{
				calibration_data.header.sens_resolution.width	= images_descriptor.width;
				calibration_data.header.sens_resolution.height	= images_descriptor.height;
			}

			// channels sensitivities

			{
				{
					calibration_data.header.sensitivity_data.sens_offset = ((unsigned char *) &(calibration_data.sensitivity_data)) - ((unsigned char *) &(calibration_data.header.sensitivity_data));

					calibration_data.header.sensitivity_data.sensitivity_colour_temps = 1;
				}

				{
					calibration_data.sensitivity_data.x_value_x1000 = (unsigned short) (STECC_ILLUMINANT_CHROMATICITY_X * 1000.0f + 0.5f);
					calibration_data.sensitivity_data.y_value_x1000 = (unsigned short) (STECC_ILLUMINANT_CHROMATICITY_Y * 1000.0f + 0.5f);

					calibration_data.sensitivity_data.sens.r  = fake_value;
					calibration_data.sensitivity_data.sens.gr = fake_value;
					calibration_data.sensitivity_data.sens.gb = fake_value;
					calibration_data.sensitivity_data.sens.b  = fake_value;
				}
  			}

			// lens shading
			
			{
				{
					calibration_data.header.ls_data.ls_measured_lp_nums_offset	= ((unsigned char *) &(calibration_data.shading_tables_count)) - ((unsigned char *) &(calibration_data.header.ls_data));
					calibration_data.header.ls_data.ls_comTable_offset			= ((unsigned char *) &(calibration_data.shading_header)) - ((unsigned char *) &(calibration_data.header.ls_data));

					calibration_data.header.ls_data.ls_measured_temp_num		= 1;
					calibration_data.header.ls_data.ls_table_num				= 1;
				}

				{
					calibration_data.shading_tables_count = 1;
				}

				{
					calibration_data.shading_header.lsc_data_offset		= ((unsigned char *) &(calibration_data.shading_data)) - ((unsigned char *) &(calibration_data.shading_header));

					calibration_data.shading_header.x_value_x1000		= (unsigned short) (STECC_ILLUMINANT_CHROMATICITY_X * 1000.0f + 0.5f);
					calibration_data.shading_header.y_value_x1000		= (unsigned short) (STECC_ILLUMINANT_CHROMATICITY_Y * 1000.0f + 0.5f);
					calibration_data.shading_header.x_division			= STECC_SHADING_GRID_W;
					calibration_data.shading_header.y_division			= STECC_SHADING_GRID_H;
					calibration_data.shading_header.ch					= 4;
					calibration_data.shading_header.center_bit_depth	= 12;
					calibration_data.shading_header.other_bit_depth		= 12;
					calibration_data.shading_header.object_distance		= 0;
					calibration_data.shading_header.zoom_factor			= 0;
 				}

				{
					unsigned int index_s = 0;

					for (unsigned int index_y = 0; index_y < STECC_SHADING_GRID_H; index_y++)
					{
						for (unsigned int index_x = 0; index_x < STECC_SHADING_GRID_W; index_x++)
						{
							calibration_data.shading_data[index_s].r  = fake_value;
							calibration_data.shading_data[index_s].gr = fake_value;
							calibration_data.shading_data[index_s].gb = fake_value;
							calibration_data.shading_data[index_s].b  = fake_value;

							index_s += 1;
						}
					}
				}
			}
		}
		else
		{
			unsigned short int * unpacked_image = (unsigned short int *) temporary_data_buffer;

			float * average_plane[4];

			for (unsigned int channel_index = 0; channel_index < 4; channel_index++)
			{
				average_plane[channel_index] = (float *) ((unsigned char *) temporary_data_buffer + images_descriptor.width * images_descriptor.height * sizeof(unsigned short) + channel_index * (images_descriptor.width / 2) * (images_descriptor.height / 2) * sizeof(float));

				memset(average_plane[channel_index], 0, (images_descriptor.width / 2) * (images_descriptor.height / 2) * sizeof(float));
			}

			for (unsigned int images_index = 0; images_index < images_count; images_index++)
			{
				stecc_internal_raw12_to_raw16((unsigned char *) (images_array[images_index]), (unsigned char *) unpacked_image, images_descriptor.stride, images_descriptor.width * 2, images_descriptor.width / 2, images_descriptor.height / 2);

				for (unsigned int channel_index = 0; channel_index < 4; channel_index++)
				{
					unsigned short int * src;

					switch(channel_index)
					{
						case 0:
						{
							src = unpacked_image + (0 + 0 * images_descriptor.width);

							break;
						}

						case 1:
						{
							src = unpacked_image + (1 + 0 * images_descriptor.width);

							break;
						}

						case 2:
						{
							src = unpacked_image + (0 + 1 * images_descriptor.width);

							break;
						}

						case 3:
						{
							src = unpacked_image + (1 + 1 * images_descriptor.width);

							break;
						}
					}

					float * dst = average_plane[channel_index];

					for (unsigned int y_index = 0; y_index < images_descriptor.height / 2; y_index++)
					{
						for (unsigned int x_index = 0; x_index < images_descriptor.width / 2; x_index++)
						{
							*dst += *src;

							src += 2;
							dst += 1;
						}

						src += images_descriptor.width;
					}
				}
			}

			for (unsigned int channel_index = 0; channel_index < 4; channel_index++)
			{
				float * cur = average_plane[channel_index];

				for (unsigned int y_index = 0; y_index < images_descriptor.height / 2; y_index++)
				{
					for (unsigned int x_index = 0; x_index < images_descriptor.width / 2; x_index++)
					{
						*cur /= images_count;

						cur += 1;
					}
				}
			}

			float * average_grid[4];

			for (unsigned int channel_index = 0; channel_index < 4; channel_index++)
			{
				average_grid[channel_index] = (float *) ((unsigned char *) temporary_data_buffer + images_descriptor.width * images_descriptor.height * sizeof(unsigned short) + 4 * (images_descriptor.width / 2) * (images_descriptor.height / 2) * sizeof(float) + channel_index * STECC_SHADING_GRID_W * STECC_SHADING_GRID_H * sizeof(float));

				memset(average_grid[channel_index], 0, STECC_SHADING_GRID_W * STECC_SHADING_GRID_H * sizeof(float));

				for (unsigned int gy_index = 0; gy_index < STECC_SHADING_GRID_H; gy_index++)
				{
					unsigned int start_y = ((images_descriptor.height / 2) - (cell_size * STECC_SHADING_GRID_H)) * gy_index / (STECC_SHADING_GRID_H - 1) + gy_index * cell_size;

					for (unsigned int gx_index = 0; gx_index < STECC_SHADING_GRID_W; gx_index++)
					{
						unsigned int start_x = ((images_descriptor.width / 2) - (cell_size * STECC_SHADING_GRID_W)) * gx_index / (STECC_SHADING_GRID_W - 1) + gx_index * cell_size;

						float sum = 0.0f;

						for (unsigned int iy_index = start_y; iy_index < start_y + cell_size; iy_index++)
						{
							for (unsigned int ix_index = start_x; ix_index < start_x + cell_size; ix_index++)
							{
								sum += *(average_plane[channel_index] + ix_index + iy_index * (images_descriptor.width  / 2));
							}
						}

						*(average_grid[channel_index] + gx_index + gy_index * STECC_SHADING_GRID_W) = sum / (cell_size * cell_size);
					}
				}
			}

			float * average_grid_rr;
			float * average_grid_gr;
			float * average_grid_gb;
			float * average_grid_bb;

			switch(images_descriptor.phase)
			{
				case stecc_bayer_phase_first_r:
				{
					average_grid_rr = average_grid[0];
					average_grid_gr = average_grid[1];
					average_grid_gb = average_grid[2];
					average_grid_bb = average_grid[3];

					break;
				}

				case stecc_bayer_phase_first_gr:
				{
					average_grid_rr = average_grid[1];
					average_grid_gr = average_grid[0];
					average_grid_gb = average_grid[3];
					average_grid_bb = average_grid[2];

					break;
				}

				case stecc_bayer_phase_first_gb:
				{
					average_grid_rr = average_grid[2];
					average_grid_gr = average_grid[3];
					average_grid_gb = average_grid[0];
					average_grid_bb = average_grid[1];

					break;
				}

				case stecc_bayer_phase_first_b:
				{
					average_grid_rr = average_grid[3];
					average_grid_gr = average_grid[2];
					average_grid_gb = average_grid[1];
					average_grid_bb = average_grid[0];

					break;
				}
			}

			float average_center_rr = 0.0f;
			float average_center_gr = 0.0f;
			float average_center_gb = 0.0f;
			float average_center_bb = 0.0f;

			for (int index_y = -1; index_y <= 1; index_y++)
			{
				for (int index_x = -1; index_x <= 1; index_x++)
				{
					unsigned int grid_sample = (STECC_SHADING_GRID_W / 2 + index_x) + (STECC_SHADING_GRID_H / 2 + index_y) * STECC_SHADING_GRID_W;

					average_center_rr += average_grid_rr[grid_sample];
					average_center_gr += average_grid_gr[grid_sample];
					average_center_gb += average_grid_gb[grid_sample];
					average_center_bb += average_grid_bb[grid_sample];
				}
			}

			average_center_rr /= 9;
			average_center_gr /= 9;
			average_center_gb /= 9;
			average_center_bb /= 9;

			// sensor array size

			{
				calibration_data.header.sens_resolution.width	= images_descriptor.width;
				calibration_data.header.sens_resolution.height	= images_descriptor.height;
			}

			// channels sensitivities

			{
				{
					calibration_data.header.sensitivity_data.sens_offset = ((unsigned char *) &(calibration_data.sensitivity_data)) - ((unsigned char *) &(calibration_data.header.sensitivity_data));

					calibration_data.header.sensitivity_data.sensitivity_colour_temps = 1;
				}

				{
					calibration_data.sensitivity_data.x_value_x1000 = (unsigned short int) (STECC_ILLUMINANT_CHROMATICITY_X * 1000.0f + 0.5f);
					calibration_data.sensitivity_data.y_value_x1000 = (unsigned short int) (STECC_ILLUMINANT_CHROMATICITY_Y * 1000.0f + 0.5f);

					calibration_data.sensitivity_data.sens.r  = (unsigned short int) (average_center_rr + 0.5f);
					calibration_data.sensitivity_data.sens.gr = (unsigned short int) (average_center_gr + 0.5f);
					calibration_data.sensitivity_data.sens.gb = (unsigned short int) (average_center_gb + 0.5f);
					calibration_data.sensitivity_data.sens.b  = (unsigned short int) (average_center_bb + 0.5f);
				}
  			}

			// lens shading
			
			{
				{
					calibration_data.header.ls_data.ls_measured_lp_nums_offset	= ((unsigned char *) &(calibration_data.shading_tables_count)) - ((unsigned char *) &(calibration_data.header.ls_data));
					calibration_data.header.ls_data.ls_comTable_offset			= ((unsigned char *) &(calibration_data.shading_header)) - ((unsigned char *) &(calibration_data.header.ls_data));

					calibration_data.header.ls_data.ls_measured_temp_num		= 1;
					calibration_data.header.ls_data.ls_table_num				= 1;
				}

				{
					calibration_data.shading_tables_count = 1;
				}

				{
					calibration_data.shading_header.lsc_data_offset		= ((unsigned char *) &(calibration_data.shading_data)) - ((unsigned char *) &(calibration_data.shading_header));

					calibration_data.shading_header.x_value_x1000		= (unsigned short) (STECC_ILLUMINANT_CHROMATICITY_X * 1000.0f + 0.5f);
					calibration_data.shading_header.y_value_x1000		= (unsigned short) (STECC_ILLUMINANT_CHROMATICITY_Y * 1000.0f + 0.5f);
					calibration_data.shading_header.x_division			= STECC_SHADING_GRID_W;
					calibration_data.shading_header.y_division			= STECC_SHADING_GRID_H;
					calibration_data.shading_header.ch					= 4;
					calibration_data.shading_header.center_bit_depth	= 12;
					calibration_data.shading_header.other_bit_depth		= 12;
					calibration_data.shading_header.object_distance		= 0;
					calibration_data.shading_header.zoom_factor			= 0;
 				}

				{
					for (unsigned int index_y = 0; index_y < STECC_SHADING_GRID_H; index_y++)
					{
						for (unsigned int index_x = 0; index_x < STECC_SHADING_GRID_W; index_x++)
						{
							unsigned int index_s = index_x + index_y * STECC_SHADING_GRID_W;

							calibration_data.shading_data[index_s].r  = (unsigned short int) (average_grid_rr[index_s] + 0.5f);
							calibration_data.shading_data[index_s].gr = (unsigned short int) (average_grid_gr[index_s] + 0.5f);
							calibration_data.shading_data[index_s].gb = (unsigned short int) (average_grid_gb[index_s] + 0.5f);
							calibration_data.shading_data[index_s].b  = (unsigned short int) (average_grid_bb[index_s] + 0.5f);
						}
					}
				}
			}
		}

		// not calibrated

		{
			// autofocus (empty)

			{
				calibration_data.header.af_data.C_F_object_distances_offset		= 0;
				calibration_data.header.af_data.additional_af_data_offset		= 0;

				calibration_data.header.af_data.obj_dis_availability_bitmask	= 0;
				calibration_data.header.af_data.size_of_additional_af_data		= 0;
				calibration_data.header.af_data.fine_step_im					= 0;
				calibration_data.header.af_data.fine_step_mi					= 0;
				calibration_data.header.af_data.availability_bitmask_h			= 0;
				calibration_data.header.af_data.availability_bitmask_u			= 0;
				calibration_data.header.af_data.availability_bitmask_d			= 0;
				calibration_data.header.af_data.pos_unit_relation_to_cw			= 0;
				calibration_data.header.af_data.total_positions_h				= 0;
				calibration_data.header.af_data.total_positions_u				= 0;
				calibration_data.header.af_data.total_positions_d				= 0;
			}

			// defects maps (empty)

			{
				calibration_data.header.defect_data.couplet_type1_map_offset		= 0;
				calibration_data.header.defect_data.couplet_type2_map_offset		= 0;
				calibration_data.header.defect_data.line_map_offset					= 0;

				calibration_data.header.defect_data.num_of_couplet_type1_defects	= 0;
				calibration_data.header.defect_data.num_of_couplet_type2_defects	= 0;
				calibration_data.header.defect_data.num_of_line_defects				= 0;
			}

			// mechanical shutter and neutral density filter (empty)

			{
				calibration_data.header.nd_ms_data.shut_delay_us	= 0;
				calibration_data.header.nd_ms_data.ND_trans_x100	= 0;
			}

			// black offset (empty)

			{
				calibration_data.header.spatial_pedestal_data.spatial_pedestal_table_offset	= 0;

				calibration_data.header.spatial_pedestal_data.number_of_tables				= 0;
			}
		}

		// auxiliary data

		{
			// checksums

			{
				calibration_data.header.checksum				= 0;
				calibration_data.header.checksum_calculated		= 0;
				calibration_data.header.number_of_errors		= 0;
				calibration_data.header.checksum2				= 0;
				calibration_data.header.checksum2_calculated	= 0;
				calibration_data.header.decoder_status			= 0;
				calibration_data.header.total_data_size			= sizeof(calibration_data);
			}

			// offsets

			{
				calibration_data.header.offset_af_data_object_distances	= 0;
				calibration_data.header.offset_af_additional_af_data	= 0;
				calibration_data.header.offset_ls_no_of_lens_positions	= ((unsigned char *) &(calibration_data.shading_tables_count)) - ((unsigned char *) &(calibration_data.header));
				calibration_data.header.offset_com_tables				= ((unsigned char *) &(calibration_data.shading_header		)) - ((unsigned char *) &(calibration_data.header));
				calibration_data.header.offset_ls_lsc_data				= ((unsigned char *) &(calibration_data.shading_data		)) - ((unsigned char *) &(calibration_data.header));
				calibration_data.header.offset_sensitivity_data			= ((unsigned char *) &(calibration_data.sensitivity_data	)) - ((unsigned char *) &(calibration_data.header));
				calibration_data.header.offset_defect_couplet_type1		= 0;
				calibration_data.header.offset_defect_couplet_type2		= 0;
				calibration_data.header.offset_defect_line				= 0;
				calibration_data.header.offset_pedestal_table			= 0;
				calibration_data.header.offset_pedestal_data			= 0;
			}
		}

#		ifdef STECC_DEBUG

		{
			printf("rr = %d\n", calibration_data.sensitivity_data.sens.r);
			printf("gr = %d\n", calibration_data.sensitivity_data.sens.gr);
			printf("gb = %d\n", calibration_data.sensitivity_data.sens.gb);
			printf("bb = %d\n", calibration_data.sensitivity_data.sens.b);
					
			printf("\n");
		}

		{
			for (unsigned int index_y = 0; index_y < STECC_SHADING_GRID_H; index_y++)
			{
				for (unsigned int index_x = 0; index_x < STECC_SHADING_GRID_W; index_x++)
				{
					unsigned int index_s = index_x + index_y * STECC_SHADING_GRID_W;

					printf("%d\t", calibration_data.shading_data[index_s].r);
				}

				printf("\n");
			}

			printf("\n");
		}

		{
			for (unsigned int index_y = 0; index_y < STECC_SHADING_GRID_H; index_y++)
			{
				for (unsigned int index_x = 0; index_x < STECC_SHADING_GRID_W; index_x++)
				{
					unsigned int index_s = index_x + index_y * STECC_SHADING_GRID_W;

					printf("%d\t", calibration_data.shading_data[index_s].gr);
				}

				printf("\n");
			}

			printf("\n");
		}

		{
			for (unsigned int index_y = 0; index_y < STECC_SHADING_GRID_H; index_y++)
			{
				for (unsigned int index_x = 0; index_x < STECC_SHADING_GRID_W; index_x++)
				{
					unsigned int index_s = index_x + index_y * STECC_SHADING_GRID_W;

					printf("%d\t", calibration_data.shading_data[index_s].gb);
				}

				printf("\n");
			}

			printf("\n");
		}

		{
			for (unsigned int index_y = 0; index_y < STECC_SHADING_GRID_H; index_y++)
			{
				for (unsigned int index_x = 0; index_x < STECC_SHADING_GRID_W; index_x++)
				{
					unsigned int index_s = index_x + index_y * STECC_SHADING_GRID_W;

					printf("%d\t", calibration_data.shading_data[index_s].b);
				}

				printf("\n");
			}

			printf("\n");
		}

#		endif

		retval =true;
	}

	return retval;
}

uint16 & get_lsc_channel_reference(CAM_DRV_NVM_LSC_DATA_T & lsc, unsigned int channel)
{
	uint16 * retval = NULL;

	switch(channel)
	{
		case 0:
		{
			retval = &(lsc.r);

			break;
		}
		case 1:
		{
			retval = &(lsc.gr);

			break;
		}
		case 2:
		{
			retval = &(lsc.gb);

			break;
		}
		case 3:
		{
			retval = &(lsc.b);

			break;
		}
	}

	return *retval;
}

bool absminmax_ratio_less_than_threshold(float a, float b, float threshold)
{
	if (a < 0.0f) a = -a;
	if (b < 0.0f) b = -b;

	float min = (a < b) ? a : b;
	float max = (a > b) ? a : b;

	if (max != 0.0f)
	{
		return ((min / max) < threshold);
	}
	else
	{
		return (1.0f < threshold);
	}
}

bool stecc_check(const stecc_bayer_image_descriptor & images_descriptor, const stecc_check_parameters check_parameters, const void * reference_calibration_data_buffer, const void * current_calibration_data_buffer, stecc_check_intermediates & intermediates, unsigned int & code)
{
	code = 0;

	stecc_calibration_data & reference_calibration_data = *((stecc_calibration_data *) reference_calibration_data_buffer);
	stecc_calibration_data &   current_calibration_data = *((stecc_calibration_data *)   current_calibration_data_buffer);

	float r_r = (float) (reference_calibration_data.sensitivity_data.sens.r - images_descriptor.pedestal_value);
	float r_g = (float) ((reference_calibration_data.sensitivity_data.sens.gr + reference_calibration_data.sensitivity_data.sens.gb) / 2 - images_descriptor.pedestal_value);
	float r_b = (float) (reference_calibration_data.sensitivity_data.sens.b - images_descriptor.pedestal_value);

	float c_r = (float) (current_calibration_data.sensitivity_data.sens.r - images_descriptor.pedestal_value);
	float c_g = (float) ((current_calibration_data.sensitivity_data.sens.gr + current_calibration_data.sensitivity_data.sens.gb) / 2 - images_descriptor.pedestal_value);
	float c_b = (float) (current_calibration_data.sensitivity_data.sens.b - images_descriptor.pedestal_value);
  
	intermediates.ref_rg = r_r / r_g;
	intermediates.ref_bg = r_b / r_g;

	intermediates.cur_rg = c_r / c_g;
	intermediates.cur_bg = c_b / c_g;

	intermediates.var_rg = fabs(intermediates.cur_rg - intermediates.ref_rg) / intermediates.ref_rg;
	intermediates.var_bg = fabs(intermediates.cur_bg - intermediates.ref_bg) / intermediates.ref_bg;

    intermediates.cur_r = c_r;
    intermediates.cur_g = c_g;
    intermediates.cur_b = c_b;

    intermediates.ref_r = r_r;
    intermediates.ref_g = r_g;
    intermediates.ref_b = r_b;

        intermediates.var_2d = sqrt(intermediates.var_rg * intermediates.var_rg + intermediates.var_bg * intermediates.var_bg);

	if (check_parameters.channels_sensitivities_check_method == stecc_channels_sensitivities_check_method_2x1d)
	{
	    if (intermediates.var_rg > check_parameters.channels_sensitivities_tolerance)
	    {
		    code |= STECC_CHECK_CODE_RS_RG_VARIATION;
	    }

	    if (intermediates.var_bg > check_parameters.channels_sensitivities_tolerance)
	    {
		    code |= STECC_CHECK_CODE_RS_BG_VARIATION;
	    }
	}

	if (check_parameters.channels_sensitivities_check_method == stecc_channels_sensitivities_check_method_1x2d)
	{
		if (intermediates.var_2d > check_parameters.channels_sensitivities_tolerance)
		{
			code |= STECC_CHECK_CODE_RS_2D_VARIATION;
		}
	}

	for (unsigned int ch_index = 0; ch_index < 4; ch_index++)
	{
		float sum_c = 0.0f;
		float sum_r = 0.0f;

		float sum_cc = 0.0f;
		float sum_rr = 0.0f;
		float sum_cr = 0.0f;

		for (unsigned int c_index_y = 0; c_index_y < current_calibration_data.shading_header.y_division; c_index_y++)
		{
			for (unsigned int c_index_x = 0; c_index_x < current_calibration_data.shading_header.x_division; c_index_x++)
			{
				unsigned int c_idx = c_index_x + c_index_y * STECC_SHADING_GRID_W;

				unsigned int c_val;
				unsigned int r_val;

				c_val = get_lsc_channel_reference(current_calibration_data.shading_data[c_idx], ch_index);
				r_val = get_lsc_channel_reference(reference_calibration_data.shading_data[c_idx], ch_index);

				sum_c += c_val;
				sum_r += r_val;

				sum_cc += c_val * c_val;
				sum_rr += r_val * r_val;
				sum_cr += c_val * r_val;
			}
		}

		unsigned int cells_count = current_calibration_data.shading_header.x_division * current_calibration_data.shading_header.y_division;

		float avg_c = sum_c / cells_count;
		float avg_r = sum_r / cells_count;

		if (absminmax_ratio_less_than_threshold(avg_c, avg_r, check_parameters.lens_shading_min_average_minmaxratio))
		{
			code |= (STECC_CHECK_CODE_LS_RR_LEVEL << ch_index);
		}

		float erg_c = sum_cc - cells_count * avg_c * avg_c;
		float erg_r = sum_rr - cells_count * avg_r * avg_r;

		if (absminmax_ratio_less_than_threshold(sqrt(erg_c), sqrt(erg_r), check_parameters.lens_shading_min_sigma_minmaxratio))
		{
			code |= (STECC_CHECK_CODE_LS_RR_AMPLITUDE << ch_index);
		}

		if ((erg_c > 0.0f) && (erg_r > 0.0f))
		{
			float nxc = (sum_cr - cells_count * avg_c * avg_r) / sqrt(erg_c * erg_r);

			if (nxc < check_parameters.lens_shading_min_normcrosscorr)
			{
				code |= (STECC_CHECK_CODE_LS_RR_SHAPE << ch_index);
			}
		}
	}

	return (code == 0);
}
