/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/

#ifndef _ALGO_HPP_
#define _ALGO_HPP_
#include "videocodecDecInternal.h"

/*------------------------------------------------------------------------
* Includes
*----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
* Defines
*----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
* Types
*----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
* Functions (exported)
*----------------------------------------------------------------------*/

class mpeg4dec_arm_nmf_algo: public mpeg4dec_arm_nmf_algoTemplate
{
	// 'constructor' provided interface
public:
	virtual t_nmf_error construct(void);

	// 'iAlgo' provided interface

	virtual void configureAlgo(t_uint32 addr_in_frame_buffer, t_uint32 addr_out_frame_buffer, t_uint32 addr_internal_buffer, t_uint32 addr_in_bitstream_buffer, t_uint32 addr_out_bitstream_buffer, t_uint32 addr_in_parameters, t_uint32 addr_out_parameters, t_uint32 addr_in_frame_parameters, t_uint32 addr_out_frame_parameters);
	virtual void controlAlgo(t_t1xhv_command command, t_uint16 channelNumber, t_uint16 param);
	virtual void setDebug(t_t1xhv_debug_mode mode, t_uint32 param1, t_uint32 param2) ;
	virtual void updateAlgo(t_t1xhv_command command);

	LayerData_t LayerData;
	LayerData_t* LayerData_p;
	InstreamBuffer_t InstreamBuffer;
	AIC_Data_t AIC_Data;
	MB_Data_t MB_Data[45*80]; //MBs in 720p
	RCoeff_t RCoeff[80+1];    //MB width for 720p

#ifdef ERC_SUPPORT
	//uint32	forward_mb_array[3600];
	//uint32	backward_mb_array[3600];
#endif
	//	  HVIDEODEC decoder;
	mpeg4dec_arm_nmf_algo()/*: Primitive("mpeg4dec.arm_nmf.algo")*/ 
	{
		LayerData_p = &LayerData;
		LayerData_p->Instream_p = &InstreamBuffer;
		LayerData_p->MB_DataArray_p = (MB_Data_t*)&MB_Data[0];
		LayerData_p->AIC_Data_p = &AIC_Data;
		LayerData_p->AIC_Data_p->Ap = (RCoeff_t*)&RCoeff[0];
#ifdef ERC_SUPPORT
		//	printf("forward_mb_array=%x, backward_mb_array=%x\n", &forward_mb_array[0], &backward_mb_array[0]);
		//	LayerData_p->Instream_p->forward_mb = forward_mb_array;
		//	LayerData_p->Instream_p->backward_mb = backward_mb_array;
#endif
	}
};
#endif /* _ALGO_H_ */

