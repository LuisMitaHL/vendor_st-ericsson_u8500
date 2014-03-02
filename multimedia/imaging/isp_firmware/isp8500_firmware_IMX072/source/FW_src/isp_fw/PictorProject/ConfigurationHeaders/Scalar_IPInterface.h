/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 \file 	Scalar_IPInterface.h

 \brief This file is NOT a part of the scalar module release code.
 		All inputs needed by the scalar module that can only
	 	be resolved at the project level (at integration time)
	 	are met through this file.
	 	It is the responsibility of the integrator to generate
	 	this file at integration time and meet all the input
	 	dependencies.
	 	For the current code release, there are no input dependencies
	 	to be met. However this file must be present (even if it is blank)
	 	in incldue path of the integrated project during compilation of code.
	 	
 \ingroup Scalar
*/
#ifndef SCALAR_IPINTERFACE_H_
#   define SCALAR_IPINTERFACE_H_
#   include "GenericFunctions.h"
#   include "FrameDimension_op_interface.h"

/// Interface to provide the ceiling of a float value
#   define Scalar_RoundUp(f_Value)   GenericFunctions_Ceiling(f_Value)

/// Interface to fetch the minimum line blanking required after the GPS0
#   define Scalar_GetMinimumLineBlankingBeyondGPS0() FrameDimension_GetMinimumLineBlankingBeyondGPS0()

/// Interface to fetch the minimum line blanking required after the GPS1
#   define Scalar_GetMinimumLineBlankingBeyondGPS1() FrameDimension_GetMinimumLineBlankingBeyondGPS1()

/// Interface to provide the pending pre scale factor
#   define Scalar_GetPendingPreScaleFactor()                                               \
        (                                                                                \
            FrameDimension_IsFDMModeAuto() ? FrameDimension_GetRequestedPreScaleFactor() \
                : FrameDimension_GetCurrentPreScaleFactor()                              \
        )

#endif /*SCALAR_IPINTERFACE_H_*/

