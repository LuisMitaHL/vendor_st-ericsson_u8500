/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/

#ifndef INCLUSION_GUARD_VIDEOCODECINSTREAMBUFFER_H
#define INCLUSION_GUARD_VIDEOCODECINSTREAMBUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

	/*************************************************************************
	* Includes
	*************************************************************************/
#include "videocodecDecInternal.h"

	/*************************************************************************
	* Types, constants and external variables
	*************************************************************************/

	/*************************************************************************
	* Declaration of functions
	**************************************************************************/

	/*************************************************************************
	*
	* Name:         mp4d_InstreamBufferInit
	*
	* Parameters:   Standard    [IN]
	*               Instream_p  [OUT]
	*
	*
	* Returns:      Nothing
	*
	* Description:	Initiate Instream buffer
	*
	**************************************************************************/

	void mp4d_InstreamBufferInit(/*Standard_t Standard, */
		InstreamBuffer_t* Instream_p);

	/*************************************************************************
	*
	* Name:         mp4d_GetBits
	*
	* Parameters:   Instream_p   [IN/OUT]
	*               Length       [IN]
	*
	*
	* Returns:      uint32       [RET]
	*
	* Description:	Get n bits.
	*
	**************************************************************************/

	uint32 mp4d_GetBits(InstreamBuffer_t* Instream_p,
		int Length);

	/*************************************************************************
	*
	* Name:         ShowBits
	*
	* Parameters:   Instream_p  [IN]
	*               length      [IN]
	*
	* Returns:      n bits
	*
	* Description:  Show n bits, don't move pointer.
	*
	**************************************************************************/
	uint32 mp4d_ShowBits(InstreamBuffer_t* Instream_p, int Length);

	/*************************************************************************
	*
	* Name:         mp4d_FillBuffer32Ex
	*
	* Parameters:   Instream_p  [IN/OUT]
	*
	*
	* Returns:      Nothing
	*
	* Description:	Fill buffer32
	*
	**************************************************************************/

	void mp4d_FillBuffer32Ex(InstreamBuffer_t* Instream_p) __attribute__ ((section ("GetCoeff")));

	/*************************************************************************
	*
	* Name:         mp4d_GetSymbolindex
	*
	* Parameters:   Instream_p  [IN/OUT]
	*               Table_p     [IN]
	*
	*
	* Returns:      int         [RET]
	*
	* Description:	Get an index value from a VLC bitstream
	*
	**************************************************************************/

	int mp4d_GetSymbolindex(InstreamBuffer_t* Instream_p,
		const int* Table_p) __attribute__ ((section ("GetCoeff")));

	/*************************************************************************
	*
	* Name:         mp4d_StartCodeOrEndFollows
	*
	* Parameters:   LayerData_p  [IN/OUT]
	*               NrZeroBits   [OUT]
	*
	*
	* Returns:      boolean     [RET]
	*
	* Description:	Indicate if start code or end follows. The procedure removed
	*               the alignment bits is a startcode follows
	*
	**************************************************************************/

	boolean mp4d_StartCodeOrEndFollows(LayerData_t* LayerData_p, uint8 *NrZeroBits) __attribute__ ((section ("Decode")));

	/*************************************************************************
	*
	* Name:         FindFirstStartCode
	*
	* Parameters:   LayerData_p    [IN]
	*               Instream_p     [IN/OUT]
	*
	*
	* Returns:      ParseEvent_t   [RET]
	*
	* Description:	Searches after a startcode in the bit stream
	*
	**************************************************************************/

	ParseEvent_t mp4d_FindStartCode(LayerData_t* LayerData_p,
		InstreamBuffer_t* Instream_p);

	/*************************************************************************/

	void re_initialize(uint8 *rdptr, sint32 incnt, uint32 buffer, InstreamBuffer_t* Instream_p);

	sint32 fault_condition2(sint32 mb_in_video_packet,
		t_uint16 MBA,
		sint32 VP_MBA_init,
		uint32 start_VP_byte,
		uint32 start_VP_bit,
		uint8* tmp2_rdptr,
		sint32 tmp2_incnt,
		InstreamBuffer_t* Instream_p,
		LayerData_t* LayerData_p);

#ifdef __cplusplus
}
#endif

#endif //INCLUSION_GUARD_VIDEOCODECINSTREAMBUFFER_H
