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

/**
*  This file implements interfacing functions on hamac side. Those functions
*  either reconstruct Hamac_info from jpeg_param_(in)out or construct
*  jpeg_param_out , jpeg_param_inout from Hamac_info
**/
/****************************************************************************/

#ifndef __NMF
/*------------------------------------------------------------------------
* Includes
*----------------------------------------------------------------------*/
#include "stdio.h"
#include "string.h"
#include <assert.h>
#include "types.h"

#include "host_interface_jpegenc.h"

//tps_t1xhv_vec_jpeg_session    Gps_vec_jpeg_session;

/*------------------------------------------------------------------------
* Functions (locals)
*----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
* Functions (exported)
*----------------------------------------------------------------------*/

/****************************************************************************/
/**
* \brief 	 reads jpeg_param_in/jpep_param_inout from host and updates Hamac_info
* \author 	jean-marc volle
* 
**/
/****************************************************************************/

void jpegenc_configure_algo( t_uint32 addr_session_buffer,
							 t_uint32 addr_in_frame_buffer,
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
	tps_t1xhv_vec_jpeg_session    Gps_vec_jpeg_session;	

    /* First we erase current Hamac_info to be sure that nothing has been kept */
	Gps_vec_jpeg_session = (tps_t1xhv_vec_jpeg_session)addr_session_buffer;

	Gps_vec_jpeg_session->Gps_vec_jpeg_frame_buf_in			= (tps_t1xhv_vec_frame_buf_in)addr_in_frame_buffer;
	Gps_vec_jpeg_session->Gps_jpeg_bitstream_buf_header		= (tps_t1xhv_vec_header_buf)addr_in_header_buffer;
	Gps_vec_jpeg_session->Gps_jpeg_bitstream_buf_pos		= (tps_t1xhv_bitstream_buf_pos)addr_in_bitstream_buffer ;
	Gps_vec_jpeg_session->Gps_vec_jpeg_param_in				= (tps_t1xhv_vec_jpeg_param_in)addr_in_parameters;
	Gps_vec_jpeg_session->Gps_vec_jpeg_param_out			= (tps_t1xhv_vec_jpeg_param_out)addr_out_parameters;	
	Gps_vec_jpeg_session->Gps_vec_jpeg_param_inout			= (tps_t1xhv_vec_jpeg_param_inout)addr_in_frame_parameters;
	Gps_vec_jpeg_session->Gps_vec_jpeg_param_inout_out		= (tps_t1xhv_vec_jpeg_param_inout)addr_out_frame_parameters;
}
#endif

