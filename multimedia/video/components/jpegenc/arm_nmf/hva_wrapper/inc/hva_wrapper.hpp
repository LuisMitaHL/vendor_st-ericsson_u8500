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

#ifndef _HVA_WRAPPER_HPP_
#define _HVA_WRAPPER_HPP_

#include "Component.h"
#include "ENS_List.h"
#include "t1xhv_vec.idt"
#include "arm_nmf/share/vfm_vec_jpeg.idt"
#include "OMX_Component.h"
#include "arm_nmf/share/t1xhv_vec_jpeg_5500.idt"


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
/* t_decoder_state  controlalgo(ts_t1xhv_vdc_subtask_param*   param);*/

class jpegenc_arm_nmf_hva_wrapper: public jpegenc_arm_nmf_hva_wrapperTemplate
{
 // 'constructor' provided interface
  public:
    virtual t_nmf_error construct(void);

  // 'iAlgo' provided interface
  public:
    virtual void configureAlgo(t_uint32 addr_in_frame_buffer, t_uint32 addr_out_frame_buffer,
							   t_uint32 addr_internal_buffer, t_uint32 addr_in_header_buffer,
							   t_uint32 addr_in_bitstream_buffer, t_uint32 addr_out_bitstream_buffer,
							   t_uint32 addr_in_parameters, t_uint32 addr_out_parameters,
							    t_uint32 addr_in_frame_parameters, t_uint32 addr_out_frame_parameters);
    virtual void controlAlgo(t_t1xhv_command command, t_uint16 param);
    virtual void updateAlgo(t_t1xhv_command command,t_uint32 param1, t_uint32 param2);
    virtual void setDebug(t_t1xhv_debug_mode mode, t_uint32 param1, t_uint32 param2) ;
	virtual void setMemoryParamHVAWrapper(ts_ddep_buffer_descriptor task_address,
 											  ts_ddep_buffer_descriptor in_params_address,
 											  ts_ddep_buffer_descriptor out_params_address,
 											  ts_ddep_buffer_descriptor quant_params_address);

    virtual void endCodec(t_uint32 status, t_uint32 info, t_uint32 duration);

  // 'oEndAlgo' required interface
  public:
    jpegenc_arm_nmf_hva_wrapper();

	private:
	OMX_U16 convert_sampling_mode(OMX_U16 value);

    ts_hva_task_descriptor * __task_descriptor;
    ts_t1xhv_hva_input_parameters  * __hva_input_params;
    ts_t1xhv_hva_quant_parameters  * __hva_quant_params;
    ts_t1xhv_hva_output_parameters * __hva_out_params;


    /// @brief Buffer description for HVA based task buffer descriptor
	ts_ddep_buffer_descriptor						mHVATaskDecs;
    /// @brief Buffer description for HVA based input params
	ts_ddep_buffer_descriptor						mHVAInParamsDecs;
    /// @brief Buffer description for HVA based input params
	ts_ddep_buffer_descriptor						mHVAOutParamsDecs;
    /// @brief Buffer description for HVA based input params
	ts_ddep_buffer_descriptor						mHVAQuantParamsDecs;
};
#endif /* _HVA_WRAPPER_HPP_ */

