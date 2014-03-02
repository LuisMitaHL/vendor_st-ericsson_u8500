/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 \file 	Duster_IPInterface.h

 \brief This file is NOT a part of the Duster module release code.
 		All inputs needed by the Duster module that can only
	 	be resolved at the project level (at integration time)
	 	are met through this file.
	 	It is the responsibility of the integrator to generate
	 	this file at integration time and meet all the input
	 	dependencies.
	 	For the current code release, there are no input dependencies
	 	to be met. However this file must be present (even if it is blank)
	 	in incldue path of the integrated project during compilation of code.

 \ingroup Duster
*/
#ifndef _DUSTER_IP_INTERFACE_H_
#define _DUSTER_IP_INTERFACE_H_

#   include "Duster_PlatformSpecific.h"
#   include "Stream.h"
#   include "Duster_op_interface.h"
#   include "Damper.h"


/// Interface to Get Duster SIGMA_SAMPLES

/// Interface to Set Duster GAUSSIAN_TH1
#   define Duster_Set_GAUSSIAN_TH1(Value)  Set_ISP_DUSTER_ISP_DUSTER_GAUSSIAN_TH1_DUSTER_GAUSSIAN_TH1(Value)

/// Interface to Set Duster GAUSSIAN_TH2
#   define Duster_Set_GAUSSIAN_TH2(Value)  Set_ISP_DUSTER_ISP_DUSTER_GAUSSIAN_TH2_DUSTER_GAUSSIAN_TH2(Value)

/// Interface to Set Duster GAUSSIAN_TH3
#   define Duster_Set_GAUSSIAN_TH3(Value)  Set_ISP_DUSTER_ISP_DUSTER_GAUSSIAN_TH3_duster_gaussian_th3(Value)

/// Interface to Set Duster SIGMA_GAUSSIAN
#   define Duster_Set_SIGMA_GAUSSIAN(Value)    Set_ISP_DUSTER_ISP_DUSTER_SIGMA_GAUSSIAN_duster_sigma_gaussian(Value)

/// Interface to Set Duster RC_NORM_TH
#   define Duster_Set_RC_NORM_TH(Value)    Set_ISP_DUSTER_ISP_DUSTER_RC_NORM_TH_duster_rc_norm_th(Value)

/// Interface to Set Duster CC_LOCAL_SIGMA_TH
#   define Duster_Set_CC_LOCAL_SIGMA_TH(Value) Set_ISP_DUSTER_ISP_DUSTER_CC_LOCAL_SIGMA_TH_duster_cc_local_sigma_th(Value)

/// Interface to Set SCYT_SMOOTH_LO
#   define Duster_Set_SCYT_SMOOTH_LO(Value)    Set_ISP_DUSTER_ISP_DUSTER_SCYT_SMOOTH_LO_duster_scyt_smooth_lo(Value)

/// Interface to Set SCYT_SMOOTH_HI
#   define Duster_Set_SCYT_SMOOTH_HI(Value)    Set_ISP_DUSTER_ISP_DUSTER_SCYT_SMOOTH_HI_duster_scyt_smooth_hi(Value)

/// Interface to Set SCYT_RANK_LO
#   define Duster_Set_SCYT_RANK_LO(Value)  Set_ISP_DUSTER_ISP_DUSTER_SCYT_RANK_LO_duster_scyt_rank_lo(Value)

/// Interface to Set SCYT_RANK_HI
#   define Duster_Set_SCYT_RANK_HI(Value)  Set_ISP_DUSTER_ISP_DUSTER_SCYT_RANK_HI_duster_scyt_rank_hi(Value)

/// Interface to Compare Scythe_Rank_Hi Value in Status register match with Scythe_Rank_Hi Reg Value


/// Interface to Set DusterByPass
#   define Duster_Set_ISP_DUSTER_BYPASS_CTRL(Value)			 Set_ISP_DUSTER_ISP_DUSTER_BYPASS_CTRL_word(Value)


/**********************************************************************************/

/*  Exported prepocessing macro                                                   */

/**********************************************************************************/
#   define Duster_EnableHardware()     Set_ISP_DUSTER_ISP_DUSTER_ENABLE_duster_enable__ENABLE()
#   define Duster_DisableHardware()    Set_ISP_DUSTER_ISP_DUSTER_ENABLE_duster_enable__DISABLE()

/**********************************************************************************/

/*  Exported VARIABLES                                                            */

/**********************************************************************************/

/**********************************************************************************/

/*  Exported FUNCTIONS                                                            */

/**********************************************************************************/
#endif // _DUSTER_IP_INTERFACE_H_

