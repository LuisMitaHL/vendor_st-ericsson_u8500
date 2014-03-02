/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    ccal.h
 * \brief   STE CCAL (camera calibration adaptation library) 8500V2 library interface
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#ifndef CCAL_H_
#define CCAL_H_

// library revision
#define CCAL_REVISION_MAJOR	2
#define CCAL_REVISION_MINOR	1
#define CCAL_REVISION_BUILD	120202

// gridiron specific constants
// gridiron number of cast positions
#define CCAL_ISP_MAX_GRIDIRON_CASTS 4
// gridiron coefficients hardware bit depth
#define CCAL_ISP_GRIDIRON_DATA_WIDTH 10
// gridiron coefficients hardware fractional bits
#define CCAL_ISP_GRIDIRON_FRACT_BITS 8
#define CCAL_ISP_NUM_BAYER_CHANNELS 4
#define CCAL_ISP_INPUT_IMAGE_BITS 12

// translate to standard type names
#define uint8 uint8_t
#define uint16 uint16_t
#define uint32 uint32_t

#include "stdint.h"
#include "nmx.h"
#include "ste3a.h"

/***** library versioning information ****************************************
*
* returns the library revision data, to be cross-checked against the
* CCAL_REVISION_* macros
*
* outputs:
*   major			library major revision
*   minor			library minor revision
*   revision		library revision
*
*****************************************************************************/
void ccal_revision(unsigned int & major,
				   unsigned int & minor,
				   unsigned int & build);

/***** generate STE3A calibration data ***************************************
*
* generate an STE3A calibration data structure starting from NVM data
*
* inputs:
*   nvm 			NVM data buffer
*
* outputs:
*   ste3a_cal		STE3A calibration data buffer
*
* returns:			flag indicating success
*
*****************************************************************************/
bool ccal_generate_ste3a_calibration_data(const void *nvm_buffer,
										  ste3a_calibration_data *ste3a_cal);

/***** adapt lens shading correction grids ***********************************
*
* return temporary buffer size to compute adapt lens shading correction grids
* to NVM calibration data
*
* inputs:
*   buffer_in		stream from lsc binary file
*   buffer_in_size	size in byte of buffer_in
*
* returns:			needed buffer size
*
*****************************************************************************/
uint32_t ccal_get_buffer_size_lsc_grids(const void *buffer_in,
										const uint32_t buffer_in_size);

/***** adapt lens shading correction grids ***********************************
*
* adapt lens shading correction grids to NVM calibration data
*
* inputs:
*   buffer_in		stream from lsc binary file
*   buffer_size		size in byte of buffer_in
*   nvm 			NVM data buffer
*   temp_buf		temporary buffer
*
* outputs:
*   lsc_out			calibrated grids (in device independent format)
*   lsc_cast		cast values of calibrated grids
*
* returns:			flag indicating success
*
*****************************************************************************/
bool ccal_calibrate_lsc_grids(	const void *buffer_in,
								const uint32_t buffer_in_size,
								const void *nvm,
								const void *temp_buf,
								const nmxMatrix *lsc_out,
								const nmxMatrix *lsc_cast);

#endif
