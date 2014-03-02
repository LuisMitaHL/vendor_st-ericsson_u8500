/******************************************************************************
 Copyright (c) 2009-2011, ST-Ericsson SA
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:
 
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials
      provided with the distribution.
   3. Neither the name of the ST-Ericsson SA nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior written
      permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#ifdef __NMF

//#include "stdio.h"

#include <jpegenc/arm_nmf/algo.nmf>

#include "jpegenc_compress.h"

/*------------------------------------------------------------------------
 * Global Variables
 *----------------------------------------------------------------------*/

//static t_uint32 jdcStartTime;

/*------------------------------------------------------------------------
 * Private functions prototype
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
Methods of component interface
 *----------------------------------------------------------------------*/

/*****************************************************************************/
/**
 * \brief  init
 *
 * Init jpeg Decoder Algo component. Implicitly called while instanciating
 *   this component.
 *   Unmask wanted internal interrupts.
 */
/*****************************************************************************/
t_nmf_error jpegenc_arm_nmf_algo::construct()
{
	return NMF_OK;
}

/*****************************************************************************/
/**
 * \brief  controlAlgo
 *
 * VP6 Decoder : Control Algo component.
 *
 * \param   sCommand
 * \param   param (n/a)
  */
/*****************************************************************************/
void jpegenc_arm_nmf_algo::controlAlgo(t_t1xhv_command Command, t_uint16 param)
{
	t_t1xhv_status status;
	t_uint16 errorOrInfo;
	/* Launch jpeg Decoder */

	errorOrInfo = jpegenc_compress((t_uint32)(&Gs_vec_jpeg_session));
	//errorOrInfo = jpegenc_compress(addr_session_buffer);

	if(errorOrInfo != VEC_ERT_NONE)
	{
		status = STATUS_JOB_ABORTED;
	}
	else
	{
		status = STATUS_JOB_COMPLETE;
	}

	oEndAlgo.endAlgo(status, (t_t1xhv_encoder_info)errorOrInfo, 0, Gs_vec_jpeg_session.Gps_vec_jpeg_param_out->bitstream_size,0);


} /* end of controlAlgo() function */


void jpegenc_arm_nmf_algo::configureAlgo(t_uint32 addr_in_frame_buffer,
										 t_uint32 addr_out_frame_buffer,
										 t_uint32 addr_internal_buffer,
										 t_uint32 addr_in_header_buffer,
										 t_uint32 addr_in_bitstream_buffer,
										 t_uint32 addr_out_bitstream_buffer,
										 t_uint32 addr_in_parameters,
										 t_uint32 addr_out_parameters,
										 t_uint32 addr_in_frame_parameters,
										 t_uint32 addr_out_frame_parameters)
{

	tps_t1xhv_vec_jpeg_session Gps_vec_jpeg_session = &Gs_vec_jpeg_session;

#ifdef TRACE_ENABLE
    tps_t1xhv_vec_jpeg_param_in     tmp_in_param    =(tps_t1xhv_vec_jpeg_param_in)addr_in_parameters;
    tps_t1xhv_vec_jpeg_param_inout  tmp_inout_param =(tps_t1xhv_vec_jpeg_param_inout)addr_in_frame_parameters;
	t_uint32 i;

	printf("Algo: JPEG Encoder Input PARAMETERS:\n");

	printf("frame_width=%d\n",tmp_in_param->frame_width);
	printf("frame_height=%d\n",tmp_in_param->frame_height);
	printf("window_width=%d\n",tmp_in_param->window_width);
	printf("window_height=%d\n",tmp_in_param->window_height);
	printf("window_horizontal_offset=%d\n",tmp_in_param->window_horizontal_offset);
	printf("window_vertical_offset=%d\n",tmp_in_param->window_vertical_offset);
	printf("sampling_mode=%d\n",tmp_in_param->sampling_mode);
	printf("restart_interval=%d\n",tmp_in_param->restart_interval);

#if 0
	//Quant
	printf("quant_luma\n");
	for(i=0;i<64;i++)
	{
		if((i+1)%8 == 0)
		{
			printf("\n");
		}
		printf("%d ",tmp_in_param->quant_luma[i]);
	}

	printf("\nquant_chroma\n");
	for(i=0;i<64;i++)
	{
		if((i+1)%8 == 0)
		{
			printf("\n");
		}

		printf("%d ",tmp_in_param->quant_chroma[i]);
	}
#endif //if 0

	printf("last_slice=%d\n",tmp_in_param->last_slice);

    printf("Algo: JPEG Encoder InOut PARAMETERS:\n");

	printf("restart_mcu_count=%d\n",tmp_inout_param->restart_mcu_count);
	printf("dc_predictor_y=%d\n",tmp_inout_param->dc_predictor_y);
	printf("dc_predictor_cb=%d\n",tmp_inout_param->dc_predictor_cb);
	printf("dc_predictor_cr=%d\n",tmp_inout_param->dc_predictor_cr);
	printf("restart_marker_id=%d\n",tmp_inout_param->restart_marker_id);
#endif //TRACE_ENABLE

	Gps_vec_jpeg_session->Gps_vec_jpeg_frame_buf_in			= (tps_t1xhv_vec_frame_buf_in)addr_in_frame_buffer;
	Gps_vec_jpeg_session->Gps_jpeg_bitstream_buf_header		= (tps_t1xhv_vec_header_buf)addr_in_header_buffer;
	Gps_vec_jpeg_session->Gps_jpeg_bitstream_buf_pos		= (tps_t1xhv_bitstream_buf_pos)addr_in_bitstream_buffer ;
	Gps_vec_jpeg_session->Gps_vec_jpeg_param_in				= (tps_t1xhv_vec_jpeg_param_in)addr_in_parameters;
	Gps_vec_jpeg_session->Gps_vec_jpeg_param_out			= (tps_t1xhv_vec_jpeg_param_out)addr_out_parameters;	
	Gps_vec_jpeg_session->Gps_vec_jpeg_param_inout			= (tps_t1xhv_vec_jpeg_param_inout)addr_in_frame_parameters;
	Gps_vec_jpeg_session->Gps_vec_jpeg_param_inout_out		= (tps_t1xhv_vec_jpeg_param_inout)addr_out_frame_parameters;

} /* end of configureAlgo() function. */

void jpegenc_arm_nmf_algo::updateAlgo(t_t1xhv_command command,t_uint32 param1, t_uint32 param2)
{


}


/*------------------------------------------------------------------------
 * Internal Functions
 *----------------------------------------------------------------------*/

void jpegenc_arm_nmf_algo::setDebug(t_t1xhv_debug_mode mode, t_uint32 param1, t_uint32 param2)
{}

#endif

