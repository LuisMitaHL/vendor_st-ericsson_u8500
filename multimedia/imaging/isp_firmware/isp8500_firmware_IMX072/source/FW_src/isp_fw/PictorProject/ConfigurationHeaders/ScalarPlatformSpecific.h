/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 \file 		ScalarPlatformSpecific.h

 \brief		This file is NOT a part of the module release code.
	 		All inputs needed by the scalar module that are paltform
	 		dependent (like the maximum number of scalar coefficients)
	 		are met through this file.
	 		It is the responsibility of the integrator to generate
	 		this file at integration time and meet all the platform
	 		specific dependencies.
	 		This file must be present in incldue path of the
	 		integrated project during compilation of code.

 \note		The following sections define the dependencies that must be
 			met by the system integrator. The way these dependencies
 			have been resolved here are just for example. These dependencies
 			must be appropriately resolved based on the platform being used.

 \ingroup Scalar
*/
#ifndef SCALARPLATFORMSPECIFIC_
#   define SCALARPLATFORMSPECIFIC_

#   include "Platform.h"

/// Maximum number of scalar coefficients. This should be taken from the
/// maximum number of coefficients registers in the device register map.
#   define MAX_COF_COUNT   32

/// Maximum value of physical buffer level. This value has been taken from
/// the GPScale C model.
#   define MAX_B_SIZE  20

#	define EXT_DS_PRE_TOP_BORDER	2	/* external-to-image downscaling pre-scaler top-border cost */
#	define EXT_DS_PRE_BOT_BORDER	2	/* external-to-image downscaling pre-scaler bottom-border cost */
#	define EXT_DS_PRE_LFT_BORDER	2	/* external-to-image downscaling pre-scaler left-border cost */
#	define EXT_DS_PRE_RGT_BORDER	2	/* external-to-image downscaling pre-scaler right-border cost */

#	define EXT_DS_PST_TOP_BORDER	0	/* external-to-image downscaling post-scaler top-border cost */
#	define EXT_DS_PST_BOT_BORDER	0	/* external-to-image downscaling post-scaler bottom-border cost */
#	define EXT_DS_PST_LFT_BORDER	0	/* external-to-image downscaling post-scaler left-border cost */
#	define EXT_DS_PST_RGT_BORDER	0	/* external-to-image downscaling post-scaler right-border cost */

#	define EXT_US_PRE_TOP_BORDER	2	/* external-to-image upscaling pre-scaler top-border cost */
#	define EXT_US_PRE_BOT_BORDER	2	/* external-to-image upscaling pre-scaler bottom-border cost */
#	define EXT_US_PRE_LFT_BORDER	2	/* external-to-image upscaling pre-scaler left-border cost */
#	define EXT_US_PRE_RGT_BORDER	2	/* external-to-image upscaling pre-scaler right-border cost */

#	define EXT_US_PST_TOP_BORDER	0	/* external-to-image upscaling post-scaler top-border cost */
#	define EXT_US_PST_BOT_BORDER	0	/* external-to-image upscaling post-scaler bottom-border cost */
#	define EXT_US_PST_LFT_BORDER	0	/* external-to-image upscaling post-scaler left-border cost */
#	define EXT_US_PST_RGT_BORDER	0	/* external-to-image upscaling post-scaler right-border cost */

#	define INT_DS_PRE_TOP_BORDER	2	/* internal-to-image downscaling pre-scaler top-border cost */
#	define INT_DS_PRE_BOT_BORDER	2	/* internal-to-image downscaling pre-scaler bottom-border cost */
#	define INT_DS_PRE_LFT_BORDER	2	/* internal-to-image downscaling pre-scaler left-border cost */
#	define INT_DS_PRE_RGT_BORDER	2	/* internal-to-image downscaling pre-scaler right-border cost */
#	define INT_DS_PST_TOP_BORDER	2	/* internal-to-image downscaling post-scaler top-border cost */
#	define INT_DS_PST_BOT_BORDER	2	/* internal-to-image downscaling post-scaler bottom-border cost */
#	define INT_DS_PST_LFT_BORDER	2	/* internal-to-image downscaling post-scaler left-border cost */
#	define INT_DS_PST_RGT_BORDER	2	/* internal-to-image downscaling post-scaler right-border cost */

#	define INT_US_PRE_TOP_BORDER	2	/* internal-to-image upscaling pre-scaler top-border cost */
#	define INT_US_PRE_BOT_BORDER	2	/* internal-to-image upscaling pre-scaler bottom-border cost */
#	define INT_US_PRE_LFT_BORDER	2	/* internal-to-image upscaling pre-scaler left-border cost */
#	define INT_US_PRE_RGT_BORDER	2	/* internal-to-image upscaling pre-scaler right-border cost */
#	define INT_US_PST_TOP_BORDER	2	/* internal-to-image upscaling post-scaler top-border cost */
#	define INT_US_PST_BOT_BORDER	2	/* internal-to-image upscaling post-scaler bottom-border cost */
#	define INT_US_PST_LFT_BORDER	2	/* internal-to-image upscaling post-scaler left-border cost */
#	define INT_US_PST_RGT_BORDER	2	/* internal-to-image upscaling post-scaler right-border cost */


#endif /* SCALARPLATFORMSPECIFIC_ */

