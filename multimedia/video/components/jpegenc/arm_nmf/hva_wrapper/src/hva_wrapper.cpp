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

#include "stdio.h"
#include <jpegenc/arm_nmf/hva_wrapper.nmf>
#include <hva_drv/arm_nmf/hva_notifier.nmf>
#include "hva_wrapper.hpp"
#include <los/api/los_api.h>

#include <string.h>
t_uint32 header_size;

//#define JPEG_ENC 0

/*------------------------------------------------------------------------
 * Global Variables
 *----------------------------------------------------------------------*/


/*------------------------------------------------------------------------
 * Private functions prototype
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
Methods of component interface
 *----------------------------------------------------------------------*/

/*****************************************************************************/
/*****************************************************************************/
t_nmf_error jpegenc_arm_nmf_hva_wrapper::construct()
{
	t_nmf_error ret = NMF_OK;
	return ret;
}

jpegenc_arm_nmf_hva_wrapper::jpegenc_arm_nmf_hva_wrapper()
{
	__task_descriptor  = 0x0;
	__hva_input_params = 0x0;
	__hva_out_params   = 0x0;
	__hva_quant_params = 0x0;

	mHVATaskDecs.nSize = 0;
	mHVATaskDecs.nMpcAddress = 0;
	mHVATaskDecs.nLogicalAddress = 0;
	mHVATaskDecs.nPhysicalAddress = 0;

	mHVAInParamsDecs.nSize = 0;
	mHVAInParamsDecs.nMpcAddress = 0;
	mHVAInParamsDecs.nLogicalAddress = 0;
	mHVAInParamsDecs.nPhysicalAddress = 0;

	mHVAOutParamsDecs.nSize = 0;
	mHVAOutParamsDecs.nMpcAddress = 0;
	mHVAOutParamsDecs.nLogicalAddress = 0;
	mHVAOutParamsDecs.nPhysicalAddress = 0;

	mHVAQuantParamsDecs.nSize = 0;
	mHVAQuantParamsDecs.nMpcAddress = 0;
	mHVAQuantParamsDecs.nLogicalAddress = 0;
	mHVAQuantParamsDecs.nPhysicalAddress = 0;
}


/*****************************************************************************/
/*****************************************************************************/
void jpegenc_arm_nmf_hva_wrapper::setMemoryParamHVAWrapper(ts_ddep_buffer_descriptor task_address,
 											  ts_ddep_buffer_descriptor in_params_address,
 											  ts_ddep_buffer_descriptor out_params_address,
 											  ts_ddep_buffer_descriptor quant_params_address)
{


	mHVATaskDecs            = task_address;
	mHVAInParamsDecs        = in_params_address;
	mHVAOutParamsDecs       = out_params_address;
	mHVAQuantParamsDecs     = quant_params_address;


	__task_descriptor = (ts_hva_task_descriptor *)mHVATaskDecs.nLogicalAddress;

//	__task_descriptor->addr_source_buffer
	__task_descriptor->addr_snap_picture = 0x0; //snap feature is NOT supported
//	__task_descriptor->addr_output_bitstream_start
//	__task_descriptor->addr_output_bitstream_end
//	__task_descriptor->header_size
	__task_descriptor->addr_in_parameters = (OMX_U32)mHVAInParamsDecs.nPhysicalAddress;
	__task_descriptor->addr_out_parameters = (OMX_U32)mHVAOutParamsDecs.nPhysicalAddress;
	__task_descriptor->addr_quant_tables_buffer = (OMX_U32)mHVAQuantParamsDecs.nPhysicalAddress;

	__hva_input_params = (ts_t1xhv_hva_input_parameters  *)mHVAInParamsDecs.nLogicalAddress;
	__hva_out_params   = (ts_t1xhv_hva_output_parameters *)mHVAOutParamsDecs.nLogicalAddress;
	__hva_quant_params = (ts_t1xhv_hva_quant_parameters  *)mHVAQuantParamsDecs.nLogicalAddress;


	

}




void jpegenc_arm_nmf_hva_wrapper::configureAlgo(t_uint32 addr_in_frame_buffer,
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
	tps_t1xhv_vec_frame_buf_in     ps_vec_jpeg_frame_buf_in;
	tps_t1xhv_vec_header_buf	   ps_jpeg_bitstream_buf_header;
	tps_t1xhv_bitstream_buf_pos    ps_jpeg_bitstream_buf_pos;
	tps_t1xhv_vec_jpeg_param_in    ps_vec_jpeg_param_in;
	tps_t1xhv_vec_jpeg_param_out   ps_vec_jpeg_param_out;
	tps_t1xhv_vec_jpeg_param_inout ps_vec_jpeg_param_inout;
	tps_t1xhv_vec_jpeg_param_inout ps_vec_jpeg_param_inout_out;
	tps_t1xhv_vec_frame_buf_out    ps_vec_jpeg_frame_buf_out;

	ps_vec_jpeg_frame_buf_in  = (tps_t1xhv_vec_frame_buf_in)addr_in_frame_buffer;
	ps_vec_jpeg_frame_buf_out = (tps_t1xhv_vec_frame_buf_out)addr_out_frame_buffer;
	ps_jpeg_bitstream_buf_header = (tps_t1xhv_vec_header_buf)addr_in_header_buffer;
	ps_jpeg_bitstream_buf_pos = (tps_t1xhv_bitstream_buf_pos)addr_in_bitstream_buffer ;
	ps_vec_jpeg_param_in = (tps_t1xhv_vec_jpeg_param_in)addr_in_parameters;
	ps_vec_jpeg_param_out = (tps_t1xhv_vec_jpeg_param_out)addr_out_parameters;
	ps_vec_jpeg_param_inout = (tps_t1xhv_vec_jpeg_param_inout)addr_in_frame_parameters;
	ps_vec_jpeg_param_inout_out = (tps_t1xhv_vec_jpeg_param_inout)addr_out_frame_parameters;

	// fill the structures from here - input parameters
	__hva_input_params->frame_width                 = ps_vec_jpeg_param_in->frame_width;
	__hva_input_params->frame_height                = ps_vec_jpeg_param_in->frame_height;
	__hva_input_params->window_width 				= ps_vec_jpeg_param_in->window_width;
	__hva_input_params->window_height               = ps_vec_jpeg_param_in->window_height;
	__hva_input_params->window_horizontal_offset    = ps_vec_jpeg_param_in->window_horizontal_offset;
	__hva_input_params->window_vertical_offset      = ps_vec_jpeg_param_in->window_vertical_offset;
	__hva_input_params->sampling_mode               = convert_sampling_mode(ps_vec_jpeg_param_in->sampling_mode);
	__hva_input_params->encode_mode                 = convert_sampling_mode(ps_vec_jpeg_param_in->sampling_mode);
	__hva_input_params->restart_interval            = ps_vec_jpeg_param_in->restart_interval;
	//__hva_input_params->restart_interval            = 0;
	__hva_input_params->two_chroma_tables_flag      = 0; /* always same table is used for both chroma */

	


	// fill the structures from here - output parameters
	__hva_out_params->bitstream_size   = 0;
	__hva_out_params->error_type       = 0;
	__hva_out_params->hje_start_time   = 0;
	__hva_out_params->hje_stop_time    = 0;

	// fill the structures from here - quant parameters
	for (int index=0;index<64;index++)
	{
		__hva_quant_params->quant_luma[index] = ps_vec_jpeg_param_in->quant_luma[index];
		__hva_quant_params->quant_chroma_1[index] = ps_vec_jpeg_param_in->quant_chroma[index];
		__hva_quant_params->quant_chroma_2[index] = 0;
	}

	//Logical Addresses
	OMX_U32 buff_size;//Ashish

	if(__hva_input_params->encode_mode==0)
		buff_size = (__hva_input_params->frame_width * __hva_input_params->frame_height * 3) / 2;
	else
		buff_size = __hva_input_params->frame_width * __hva_input_params->frame_height * 2;

	__task_descriptor->addr_source_buffer           = ps_vec_jpeg_frame_buf_in->addr_source_buffer;
	__task_descriptor->addr_output_bitstream_start  = ps_jpeg_bitstream_buf_pos->addr_bitstream_start;
	__task_descriptor->header_size                  = ps_jpeg_bitstream_buf_header->header_size;
	header_size										= __task_descriptor->header_size;//to add header size bitstream_size in endCodec
	__task_descriptor->addr_output_bitstream_end    = (t_uint32)((OMX_U32)__task_descriptor->addr_output_bitstream_start + __task_descriptor->header_size + 8 * buff_size);


	


	//copying it to output buffer
	//memcpy((OMX_U8 *)__task_descriptor->addr_output_bitstream_start,(OMX_U8 *)ps_jpeg_bitstream_buf_header->addr_header_buffer,__task_descriptor->header_size);

} /* end of configureAlgo() function. */



/*****************************************************************************/
/*****************************************************************************/
void jpegenc_arm_nmf_hva_wrapper::controlAlgo(t_t1xhv_command Command, t_uint16 param)
{

	iStartCodec.startTask((t_uint32)mHVATaskDecs.nPhysicalAddress, (t_uint32)this, (t_hif_cmd_type)JPEG_ENC );
} /* end of controlAlgo() function */

/*****************************************************************************/
/*****************************************************************************/
void jpegenc_arm_nmf_hva_wrapper::endCodec(t_uint32 status, t_uint32 info, t_uint32 duration)
{
	__hva_out_params->bitstream_size = __hva_out_params->bitstream_size + (8*header_size) ;//HVA sends only encoded bitstream size, adding hjeader size into it.
	oEndAlgo.endAlgo((t_t1xhv_status)status, (t_t1xhv_encoder_info)info, ((__hva_out_params->hje_stop_time) - (__hva_out_params->hje_start_time)), __hva_out_params->bitstream_size, 1);
}

/*****************************************************************************/
/*****************************************************************************/
void jpegenc_arm_nmf_hva_wrapper::updateAlgo(t_t1xhv_command command,t_uint32 param1, t_uint32 param2)
{

}


/*------------------------------------------------------------------------
 * Internal Functions
 *----------------------------------------------------------------------*/

void jpegenc_arm_nmf_hva_wrapper::setDebug(t_t1xhv_debug_mode mode, t_uint32 param1, t_uint32 param2)
{}

t_uint16 jpegenc_arm_nmf_hva_wrapper::convert_sampling_mode(OMX_U16 value)
{
	switch(value)
	{
		case 5 :
				 /* 1: YUV422 raster Interleaved sampling mode at Input */
				 /*	1: YUV422 Interleaved subsampling type at output i.e encode_mode */
				return 1;
				break;

		case 6 :
				 /* 0: YUV420 semi_planar Interleaved sampling mode at Input */
				 /*	0: YUV 420 interleaved subsampling type at output i.e encode_mode */
				return 0;
				break;
		default : return 0;
				  break;
	}
}


