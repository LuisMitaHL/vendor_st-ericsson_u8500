/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    ccal_test.cpp
 * \brief   ccal_lib test application
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#include "ste3a.h"
#include "ccal.h"
#include "nmx.h"

#ifdef _MSC_VER // Microsoft C compiler ?
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXPECTED_CCAL_REVISION_MAJOR	2
#define EXPECTED_CCAL_REVISION_MINOR	1

#define EXPECTED_NMX_REVISION_MAJOR	2
#define EXPECTED_NMX_REVISION_MINOR	1

#define LINE_SIZE 200
#define FILE_NAME_SIZE 1000

using namespace std;

int main(int argc, char *argv[]) {
	unsigned int ccal_revision_major;
	unsigned int ccal_revision_minor;
	unsigned int ccal_revision_build;
	unsigned int nmx_revision_major;
	unsigned int nmx_revision_minor;
	unsigned int nmx_revision_build;
	char *grid_shade_binary_filename;
	FILE *grid_shade_binary_file;
	uint32_t grid_shade_size;
	void *grid_shade_data;
	char *nvm_binary_filename;
	FILE *nvm_binary_file;
	uint32_t nvm_size;
	void *nvm_data;
	char *dump_filename;
	uint32_t buffer_tmp_size;
	void *buffer_tmp_data;
	uint32_t lsc_data[26624];
	nmxMatrix lsc_out;
	float cast_data[4];
	nmxMatrix cast_positions;

	/* check CCAL library version */
	printf("CCAL library test application\n\n");
	ccal_revision(ccal_revision_major, ccal_revision_minor, ccal_revision_build);
	if(!((ccal_revision_major == EXPECTED_CCAL_REVISION_MAJOR) &&
		 (ccal_revision_minor == EXPECTED_CCAL_REVISION_MINOR) &&
		 (ccal_revision_major == CCAL_REVISION_MAJOR) &&
		 (ccal_revision_minor == CCAL_REVISION_MINOR))) {
		printf("CCAL library revision mismatch\n");
		return(0);
	}

	/* check NMX library version */
	nmxRevision(&nmx_revision_major, &nmx_revision_minor, &nmx_revision_build);
	if(!((nmx_revision_major == EXPECTED_NMX_REVISION_MAJOR) &&
		 (nmx_revision_minor == EXPECTED_NMX_REVISION_MINOR) &&
		 (nmx_revision_major == NMX_REVISION_MAJOR) &&
		 (nmx_revision_minor == NMX_REVISION_MINOR))) {
		printf("NMX library revision mismatch\n");
		return(0);
	}

	if(argc != 4) {
		printf("Use: %s <grid_shade_binary_filename> <nvm_binary_filename> <output_filename>\n",argv[0]);
		return(0);
	}

	grid_shade_binary_filename = argv[1];
	nvm_binary_filename = argv[2];
	dump_filename = argv[3];

	/* open and check grid_shade_binary_filename */
	grid_shade_binary_file = fopen(grid_shade_binary_filename, "rb");
	if(grid_shade_binary_file == NULL) {
		printf("fopen failed\n");
		return false;
	}
	if(fseek(grid_shade_binary_file, 0, SEEK_END) != 0) {
		printf("fseek failed\n");
		return false;
	}
	if ((grid_shade_size = ftell(grid_shade_binary_file)) == -1) {
		printf("ftell failed\n");
		return false;
	}
	if(fseek(grid_shade_binary_file, 0, SEEK_SET) != 0) {
		printf("fseek failed\n");
		return false;
	}
	if((grid_shade_data = malloc(grid_shade_size)) == NULL) {
		printf("malloc failed\n");
		return false;
	}
	if(fread(grid_shade_data, grid_shade_size, 1, grid_shade_binary_file) != 1) {
		printf("fread failed\n");
		fclose(grid_shade_binary_file);
		free(grid_shade_data);
		return false;
	}
	fclose(grid_shade_binary_file);

	/* open and check nvm_binary_filename */
	nvm_binary_file = fopen(nvm_binary_filename, "rb");
	if(nvm_binary_file == NULL) {
		printf("fopen failed\n");
		return false;
	}
	if(fseek(nvm_binary_file, 0, SEEK_END) != 0) {
		printf("fseek failed\n");
		return false;
	}
	if ((nvm_size = ftell(nvm_binary_file)) == -1) {
		printf("ftell failed\n");
		return false;
	}
	if(fseek(nvm_binary_file, 0, SEEK_SET) != 0) {
		printf("fseek failed\n");
		return false;
	}
	if((nvm_data = malloc(nvm_size)) == NULL) {
		printf("malloc failed\n");
		return false;
	}
	if(fread(nvm_data, nvm_size, 1, nvm_binary_file) != 1) {
		printf("fread failed\n");
		fclose(nvm_binary_file);
		free(nvm_data);
		return false;
	}
	fclose(nvm_binary_file);

	ste3a_calibration_data calibration_data;

	ccal_generate_ste3a_calibration_data(nvm_data, &calibration_data);

	/* Compute the no adaptation */
	ccal_generate_ste3a_calibration_data(NULL, &calibration_data);

	/* Initialize lsc_out matrix as a vector (the output will be reshape) */
	nmxSetMatrixP2D_u(&lsc_out, lsc_data, 26624, 1);

	/* Initialize cast_position vector of four float elements */
	nmxSetMatrixP2D_f(&cast_positions, cast_data, 4, 1);

	/* Compute buffer_tmp_size and allocate memory */
	buffer_tmp_size  = ccal_get_buffer_size_lsc_grids(grid_shade_data, grid_shade_size);
	if((buffer_tmp_data = malloc(buffer_tmp_size)) == NULL) {
		printf("malloc failed\n");
		return false;
	}

	/* Compute the nvm adaptation */
	ccal_calibrate_lsc_grids(grid_shade_data, grid_shade_size, nvm_data, buffer_tmp_data, &lsc_out, &cast_positions);

	/* Print out lsc_out size and cast value */
	printf("lsc_out number of dimensions = %d\n", lsc_out.ndims);
	printf("\tsizeX   = %d\n", lsc_out.dims[0]);
	printf("\tGrR-BGb = %d\n", lsc_out.dims[1]);
	printf("\tsizeY   = %d\n", lsc_out.dims[2]);
	printf("\tNofCast = %d\n", lsc_out.dims[3]);
	printf("Cast position 0 = %f\n", cast_data[0]);
	printf("Cast position 1 = %f\n", cast_data[1]);
	printf("Cast position 2 = %f\n", cast_data[2]);
	printf("Cast position 3 = %f\n", cast_data[3]);

	nmxSave_a(&lsc_out, dump_filename, NULL);

	/* Compute the no adaptation */
	ccal_calibrate_lsc_grids(grid_shade_data, grid_shade_size, NULL, buffer_tmp_data, &lsc_out, &cast_positions);

	dump_filename[0] = 'N';
	nmxSave_a(&lsc_out, dump_filename, NULL);

	free(grid_shade_data);
	free(nvm_data);
	free(buffer_tmp_data);

	return(0);
}

