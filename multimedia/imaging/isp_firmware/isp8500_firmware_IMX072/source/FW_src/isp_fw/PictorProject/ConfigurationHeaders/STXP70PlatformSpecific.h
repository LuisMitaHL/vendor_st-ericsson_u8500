/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 \file 		STXP70PlatformSpecific.h

 \brief		This file is NOT a part of the module release code.
	 		All inputs needed by the STXP70 module that are paltform
	 		dependent (like the total number of interrupts instantiated
	 		in the core) are met through this file.
	 		It is the responsibility of the integrator to generate
	 		this file at integration time and meet all the platform
	 		specific dependencies.
	 		This file must be present in incldue path of the
	 		integrated project during compilation of code.

 \note		The following sections define the dependencies that must be
 			met by the system integrator. The way these dependencies
 			have been resolved here are just for example. These dependencies
 			must be appropriately resolved based on the platform being used.

 \ingroup STXP70
*/
#ifndef STXP70PLATFORMSPECIFIC_H_
#   define STXP70PLATFORMSPECIFIC_H_

/// Specifies the number of interrupts instantiated in the core.
/// Normally there are either 15 or 31 interrupts instantiated
/// in the core. This does not include the NMI.
#   define STXP70_NUMBER_OF_INTERRUPTS 0x1F
#endif /*STXP70PLATFORMSPECIFIC_H_*/

