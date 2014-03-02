/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    stecc_test_app.cpp
 * \brief   STE Camera Calibration library test application
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#include "stecc.h"

#include <stdlib.h>
#include <stdio.h>


#define EXPECTED_STECC_REVISION_MAJOR	1
#define EXPECTED_STECC_REVISION_MINOR	1


int main(int argc, char *argv[])
{
	printf("STECC library test application\n\n");

	unsigned int stecc_revision_major;
	unsigned int stecc_revision_minor;
	unsigned int stecc_revision_build;

	stecc_revision(stecc_revision_major, stecc_revision_minor, stecc_revision_build);

	if (	(stecc_revision_major == EXPECTED_STECC_REVISION_MAJOR) &&
			(stecc_revision_minor == EXPECTED_STECC_REVISION_MINOR) &&
			(STECC_REVISION_MAJOR == EXPECTED_STECC_REVISION_MAJOR) &&
			(STECC_REVISION_MINOR == EXPECTED_STECC_REVISION_MINOR)		)
	{
		stecc_bayer_image_descriptor images_descriptor;

		images_descriptor.width				= atoi(argv[1]);
		images_descriptor.height			= atoi(argv[2]);
		images_descriptor.phase				= stecc_bayer_phase_first_gb;
		images_descriptor.stride			= atoi(argv[3]);
		images_descriptor.pedestal_value	= 256;
		images_descriptor.clipping_value	= 4095;

		unsigned int image_size = images_descriptor.stride * images_descriptor.height;

		unsigned int images_count = argc - 7;

		unsigned int temporary_data_buffer_size;
		unsigned int calibration_data_buffer_size;

		stecc_get_buffers_size(images_descriptor, temporary_data_buffer_size, calibration_data_buffer_size);

		unsigned int buffers_size = temporary_data_buffer_size + 2 * calibration_data_buffer_size + images_count * (image_size + sizeof(void *));

		void * buffers;

		if ((buffers = malloc(buffers_size)) != NULL)
		{
			void * temporary_data_buffer		= (unsigned char *) buffers + 0;
			void * calibration_data_buffer_c	= (unsigned char *) buffers + temporary_data_buffer_size;
			void * calibration_data_buffer_r	= (unsigned char *) buffers + temporary_data_buffer_size + calibration_data_buffer_size;

			void ** images_array = (void **) ((unsigned char *) buffers + temporary_data_buffer_size + 2 * calibration_data_buffer_size);

			unsigned char * images_buffer = (unsigned char *) buffers + temporary_data_buffer_size + 2 * calibration_data_buffer_size + images_count * sizeof(void *);

			bool images_load_failed = false;

			for (unsigned int images_index = 0; images_index < images_count; images_index++)
			{
				images_array[images_index] = (void *) (images_buffer + image_size * images_index);

				FILE * fi;

				if ((fi = fopen(argv[5 + images_index], "rb")) != NULL)
				{
					fseek(fi, atoi(argv[4]), SEEK_SET);

					if (fread(images_array[images_index], image_size, 1, fi) != 1)
					{
						images_load_failed = true;

						printf("cannot read input file (%s)\n", argv[5 + images_index]);
					}

					fclose(fi);
				}
				else
				{
					images_load_failed = true;

					printf("cannot open input file (%s)\n", argv[5 + images_index]);
				}
			}

			if (images_load_failed == false)
			{
				if (stecc_compute(images_descriptor, images_array, images_count, temporary_data_buffer, calibration_data_buffer_c))
				{
					FILE * fo;

					if ((fo = fopen(argv[argc - 2], "wb")) != NULL)
					{
						if (fwrite(calibration_data_buffer_c, calibration_data_buffer_size, 1, fo) == 1)
						{
						}
						else
						{
							printf("cannot write calibration file\n");
						}

						fclose(fo);
					}
					else
					{
						printf("cannot open calibration file\n");
					}

					FILE * fi;

					if ((fi = fopen(argv[argc - 1], "rb")) != NULL)
					{
						if (fread(calibration_data_buffer_r, calibration_data_buffer_size, 1, fi) == 1)
						{
							stecc_check_parameters    check_parameters;
							stecc_check_intermediates check_intermediates;

							check_parameters.channels_sensitivities_check_method	= stecc_channels_sensitivities_check_method_1x2d;
							check_parameters.channels_sensitivities_tolerance		= 0.15f;
							check_parameters.lens_shading_min_average_minmaxratio	= 0.70f;
							check_parameters.lens_shading_min_sigma_minmaxratio		= 0.60f;
							check_parameters.lens_shading_min_normcrosscorr			= 0.90f;

							unsigned int code;

							if (stecc_check(images_descriptor, check_parameters, calibration_data_buffer_r, calibration_data_buffer_c, check_intermediates, code))
							{
								printf("good camera:\n");
							}
							else
							{
								printf("low quality camera:\n");
							}

							printf("\treference (golden sample) r/g = %f\n", check_intermediates.ref_rg);
							printf("\treference (golden sample) b/g = %f\n", check_intermediates.ref_bg);
							printf("\tcurrent sample r/g = %f\n", check_intermediates.cur_rg);
							printf("\tcurrent sample b/g = %f\n", check_intermediates.cur_bg);
							printf("\trelative variation r/g = %f\n", check_intermediates.var_rg);
							printf("\trelative variation b/g = %f\n", check_intermediates.var_bg);
							printf("\tused relative variation threshold method = %s\n", (check_parameters.channels_sensitivities_check_method == stecc_channels_sensitivities_check_method_1x2d) ? "1x2d" : "2x1d");
							printf("\tused relative variation threshold = %f\n", check_parameters.channels_sensitivities_tolerance);
						}
						else
						{
							printf("cannot read reference calibration file\n");
						}

						fclose(fo);
					}
					else
					{
						printf("cannot open reference calibration file\n");
					}
				}
				else
				{
					printf("cannot compute calibration data\n");
				}
			}
		}
		else
		{
			printf("cannot allocate memory buffers\n");
		}
	}
	else
	{
		printf("STECC library revision mismatch\n");
	}

	return 0;
}

