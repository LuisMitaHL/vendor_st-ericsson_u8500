/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/

#ifndef INCLUSION_GUARD_VIDEOCODECDECODE_H
#define INCLUSION_GUARD_VIDEOCODECDECODE_H

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
	* Name:         mp4d_DecodeMacroblocksCombinedMode
	*
	* Parameters:   LayerData_p  [IN/OUT]
	*
	*
	* Returns:      Nothing
	*
	* Description:	Decode the macroblocks in one GOB.
	*               MPEG4 & H.263 general
	*
	**************************************************************************/
	void mp4d_DecodeMacroblocksCombinedMode(LayerData_t* LayerData_p)  __attribute__ ((section ("Decode")));

	/*************************************************************************
	*
	* Name:         mp4d_DecodeMacroblocksDataPartitioning
	*
	* Parameters:   LayerData_p  [IN]
	*
	*
	* Returns:      Nothing
	*
	* Description:	mp4d_DecodeMacroblocksDataPartitioning is responsible for
	*               decoding the macroblocks in one dp segment. The bits
	*               are read from the bitstream and reconstucted is data
	*               produced macroblock per macroblock. H263 annex V, MPEG4 E.1.2
	*
	**************************************************************************/
	void mp4d_DecodeMacroblocksDataPartitioning(LayerData_t* LayerData_p);

	/*************************************************************************
	*
	* Name:         mp4d_ResetMB_DataD
	*
	* Parameters:   MB_Data_p  [IN/OUT]
	*
	*
	* Returns:      Nothing
	*
	* Description:	Resets macroblock data for a new picture.
	*               MPEG4 & H.263 general
	*
	**************************************************************************/
	void mp4d_ResetMB_DataD(MB_Data_t* MB_Data_p)  __attribute__ ((section ("Decode")));

	/*************************************************************************
	*
	* Name:         mp4d_ResetMB_MVDataD
	*
	* Parameters:   MB_Data_p  [IN/OUT]
	*
	*
	* Returns:      Nothing
	*
	* Description:	Resets macroblock data for a new picture.
	*               MPEG4 & H.263 general
	*
	**************************************************************************/
	void mp4d_ResetMB_MVDataD(MB_Data_t* MB_Data_p, LayerData_t* LayerData_p)  __attribute__ ((section ("Decode")));

	/*************************************************************************/
#ifdef __cplusplus
}
#endif

#endif //INCLUSION_GUARD_VIDEOCODECDECODE_H
