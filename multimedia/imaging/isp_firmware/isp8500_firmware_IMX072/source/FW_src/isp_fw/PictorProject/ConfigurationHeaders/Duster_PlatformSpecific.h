/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 \file 		Duster_PlatformSpecific.h

 \brief		This file is NOT a part of the module release code.
	 		All inputs needed by the Duster module that are platform
	 		dependent are met through this file.
	 		It is the responsibility of the integrator to generate
	 		this file at integration time and meet all the platform
	 		specific dependencies.
	 		This file must be present in incldue path of the
	 		integrated project during compilation of code.

 \note		The following sections define the dependencies that must be
 			met by the system integrator. The way these dependencies
 			have been resolved here are just for example. These dependencies
 			must be appropriately resolved based on the platform being used.

 \ingroup 	Duster
*/
#ifndef DUSTER_PLATFORMSPECIFIC_H_
#define DUSTER_PLATFORMSPECIFIC_H_

#   include "Platform.h"



/// Default Value for Duster Mode


//  Default Value for Frame Sigma


// Default Value for Duster Enable/Disable Mode
#   define DEFAULT_DUSTER_ENABLE_DISABLE   (Flag_e_FALSE)

// Gaussian Weight
#   define DEFAULT_DUSTER_GAUSSIAN_WEIGHT   (0)

// sigma Weight
#   define DEFAULT_DUSTER_SIGMA_WEIGHT   (0)

// Default Value for UserFrameSigma
#   define DEFAULT_USER_FRAME_SIGMA   (0)

// Default value for Duster_ByPass_Ctrl
#   define DEFAULT_DUSTER_BYPASS_CTRL (0)

// Default Value for Duster ByPass Control


// Default Value for Duster Scythe Control


// Default Value for Duster Scythe mode


// Default Value for Duster ScytheControl Structure SCYTHE_RANK_HI



// Default Value for Duster ScytheControl Structure SCYTHE_RANK_LO
#   define DEFAULT_SCYTHECONTROL_SCYT_RANK_LO  (0)

// Default Value for Duster ScytheControl Structure SCYTHE_SMOOTH_HI
#   define DEFAULT_SCYTHECONTROL_SCYT_SMOOTH_HI    (0)


// Default Value for Duster ScytheControl Structure SCYTHE_SMOOTH_LO


// Default Value for Centrre correctino sigma factor
#   define DEFAULT_CENTERCORRECTION_SIGMA_FACTOR    (0)

// Default Value for ring correction normal threshhold
#   define DEFAULT_RING_CORRECTION_NORMAL_THR    (0)

// Default Value for Duster CenterWeight Structure CenterCorrectionWeight


// Default Value for Duster CenterWeight Structure CenterCorrectionWeight

// Default Value for Duster SigmaGaussianWeight Structure SigmaWeight


// Default Value for Duster SigmaGaussianWeight Structure GaussianWeight


// Default Value for Duster Output Structure FRAME_SIGMA

#   define DEFAULT_DUSTEROUTPUT_FRAME_SIGMA    20

// Default Value for Duster Output Structure GAUSSIAN_TH1
#   define DEFAULT_DUSTEROUTPUT_GAUSSIAN_TH1   0

// Default Value for Duster Output Structure GAUSSIAN_TH2
#   define DEFAULT_DUSTEROUTPUT_GAUSSIAN_TH2   0

// Default Value for Duster Output Structure GAUSSIAN_TH3
#   define DEFAULT_DUSTEROUTPUT_GAUSSIAN_TH3   0

// Default Value for Duster Output Structure S0
#   define DEFAULT_DUSTEROUTPUT_S0 0

// Default Value for Duster Output Structure S1
#   define DEFAULT_DUSTEROUTPUT_S1 0

// Default Value for Duster Output Structure S2
#   define DEFAULT_DUSTEROUTPUT_S2 0

// Default Value for Duster Output Structure SCYT_RANK_LO
#   define DEFAULT_DUSTEROUTPUT_SCYT_RANK_LO   0

// Default Value for Duster Output Structure SCYT_RANK_HI
#   define DEFAULT_DUSTEROUTPUT_SCYT_RANK_HI   0

// Default Value for Duster Output Structure SCYT_SMOOTH_LO
#   define DEFAULT_DUSTEROUTPUT_SCYT_SMOOTH_LO 0

// Default Value for Duster Output Structure SCYT_SMOOTH_HI
#   define DEFAULT_DUSTEROUTPUT_SCYT_SMOOTH_HI 0

// Default Value for Duster Output Structure CC_LOCAL_SIGMA_TH
#   define DEFAULT_DUSTEROUTPUT_CC_LOCAL_SIGMA_TH  0

// Default Value for Duster Output Structure RC_NORM_TH
#   define DEFAULT_DUSTEROUTPUT_RC_NORM_TH 0

// Default Value for Duster Output Structure SIGMA_GAUSSIAN
#   define DEFAULT_DUSTEROUTPUT_SIGMA_GAUSSIAN 0

// Default Value for Duster Output Structure GAUSSIAN_WEIGHT
#   define DEFAULT_DUSTEROUTPUT_GAUSSIAN_WEIGHT    0






#endif /*DUSTER_PLATFORMSPECIFIC_H_*/

