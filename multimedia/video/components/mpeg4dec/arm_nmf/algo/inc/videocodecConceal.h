/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/

#ifndef INCLUSION_GUARD_VIDEOCODECCONCEAL_H
#define INCLUSION_GUARD_VIDEOCODECCONCEAL_H

#ifdef __cplusplus
extern "C" {
#endif

	/*************************************************************************
	* Includes
	**************************************************************************/
#include "videocodecDecInternal.h"

	/*************************************************************************
	* Types, constants and external variables
	**************************************************************************/

	/*************************************************************************
	* Declarations of functions
	**************************************************************************/

	/*************************************************************************
	*
	* Name:         mp4d_ConcealPicture
	*
	* Parameters:   LayerData_p  [IN]
	*
	*
	* Returns:      Nothing
	*
	* Description:  Conceals the parts of a picture that is not correctly decoded
	*               due to bit errors and packet losses
	*
	**************************************************************************/
#ifdef ERC_SUPPORT
	void mp4d_ConcealPicture(LayerData_t* LayerData_p, unsigned int start, unsigned int end);
#endif
	/*************************************************************************/
#ifdef __cplusplus
}
#endif

#endif //INCLUSION_GUARD_VIDEOCODECCONCEAL_H
