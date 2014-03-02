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

#ifndef _ALGO_HPP_
#define _ALGO_HPP_

#include "string.h"     /* for memset function */

/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
/* #include "djpeg_interface.h"*/

/*------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Types
 *----------------------------------------------------------------------*/

/** \brief djpeg_hamac_main() return value   */
#if 0
typedef enum
{
    DECODE_OK=0, /**< \brief the decoding was performed well   */
    DECODE_SUSPENDED, /**< \brief the decoding was suspended because of bitstream data shortage   */
    INVALID_BITSTREAM /**< \brief the bitstream is corrupted   */

} t_decoder_state;
#endif
/*------------------------------------------------------------------------
 * Functions (exported)
 *----------------------------------------------------------------------*/
/* t_decoder_state  controlalgo(ts_t1xhv_vdc_subtask_param*   param);*/

class jpegenc_arm_nmf_algo: public jpegenc_arm_nmf_algoTemplate
{
  
	ts_t1xhv_vec_jpeg_session Gs_vec_jpeg_session;
 // 'constructor' provided interface
  public:
    virtual t_nmf_error construct(void);

  // 'iAlgo' provided interface
  public:
    virtual void configureAlgo(t_uint32 addr_in_frame_buffer, 
							   t_uint32 addr_out_frame_buffer, 
							   t_uint32 addr_internal_buffer, 
							   t_uint32 addr_in_header_buffer, 
							   t_uint32 addr_in_bitstream_buffer, 
							   t_uint32 addr_out_bitstream_buffer,
							   t_uint32 addr_in_parameters, 
							   t_uint32 addr_out_parameters,
							   t_uint32 addr_in_frame_parameters, 
							   t_uint32 addr_out_frame_parameters);

    virtual void controlAlgo(t_t1xhv_command command, t_uint16 param);

    virtual void updateAlgo(t_t1xhv_command command,
								t_uint32 param1, 
								t_uint32 param2);

    virtual void setDebug(t_t1xhv_debug_mode mode, 
								t_uint32 param1, 
								t_uint32 param2) ;

  // 'oEndAlgo' required interface
  public:
    //Ijpegdec_arm_nmf_api_end_algo oEndAlgo;

  public:
    jpegenc_arm_nmf_algo()/*: Primitive("jpegenc.arm_nmf.algo")*/ {
		memset(&Gs_vec_jpeg_session,0x00,sizeof(Gs_vec_jpeg_session));
    }
};
#endif /* _ALGO_H_ */

