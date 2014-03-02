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
#ifndef PARSER_HPP_
#define PARSER_HPP_

//#include <jpegenc/arm_nmf/parser.nmf>

#include "Component.h"
#include "ENS_List.h"
#include "t1xhv_vec.idt"
#include "arm_nmf/share/vfm_vec_jpeg.idt"
#include "OMX_Component.h"
#include <common/inc/jpegenc_arm_mpc.h>


/*
typedef enum
{
	DDEP_JPEGE_SLICE_TYPE_LAST = 0,
	DDEP_JPEGE_SLICE_TYPE_LAST_AND_FIRST,
	DDEP_JPEGE_SLICE_TYPE_FIRST,
	DDEP_JPEGE_SLICE_TYPE_SUBSEQUENT,
	DDEP_JPEGE_SLICE_TYPE_FIRST_SKIPPED
} t_ddep_jpege_slice_type;


typedef struct
{
          t_uint16          ready;
          t_t1xhv_algo_id   algoId;
          t_uint32          addr_in_frame_buffer;
          t_uint32          addr_out_frame_buffer;
          t_uint32          addr_internal_buffer;
          t_uint32          addr_header_buffer;
          t_uint32          addr_in_bitstream_buffer;
          t_uint32          addr_out_bitstream_buffer;
          t_uint32          addr_in_parameters;
          t_uint32          addr_out_parameters;
          t_uint32          addr_in_frame_parameters;
          t_uint32          addr_out_frame_parameters;

}
codec_param_type;
*/

class jpegenc_arm_nmf_parser: public Component, public jpegenc_arm_nmf_parserTemplate
{
private:
	codec_param_type param;
	t_uint16 last_read_conf_no;
	t_uint16 set_param_done;
	t_uint32 nNumber_a;
	t_uint32 isHVAbased; // need to check
	OMX_BOOL isDataEncoded;
	t_uint16 get_codec_parameter;
	//t_uint16 download_parameters();
	t_ddep_jpege_slice_type ddep_jpege_slice_type;
	OMX_U32 total_bitstream_size;
	OMX_U32 current_top,frame_height,pending_height;
	OMX_U32 window_vertical_offset_save,window_height_save,frame_height_save,current_slice_height;
	OMX_BUFFERHEADERTYPE *bufIn;
	OMX_BUFFERHEADERTYPE *bufOut_slice;

	OMX_U32 SET_FLAG_FOR_EOW;
	OMX_U32 HANDLE_SEND_COMMAND;
	ts_t1xhv_bitstream_buf_link ps_arm_nmf_jpeg_bitstream_buf_link;
	t_uint16  get_hw_resource;
	t_uint16  get_vpp_resource;
	t_uint16  get_hw_resource_requested;
	t_uint16  defer_pause;
	t_uint32  header_size;
	t_uint32  addr_header_buffer;
	t_uint32  add_tmp_buff;
	OMX_STATETYPE compState;
	ts_ddep_sec_jpeg_ddep_desc s_jpege_ddep_desc;

	OMX_BOOL isEncodingUnderProgress;



public:
    //member functions
    jpegenc_arm_nmf_parser();
    virtual ~jpegenc_arm_nmf_parser();


    //Component virtual functions
    virtual void process() ;

    //Constructor interface from component.type
#ifdef NEWFSMINIT
	virtual void fsmInit(fsmInit_t initFSM);
#else
	virtual void fsmInit(t_uint16 portsDisabled,t_uint16 portsTunneled);
#endif

	virtual void reset();
	virtual void disablePortIndication(t_uint32 portIdx);
	virtual void enablePortIndication(t_uint32 portIdx);
	virtual void flushPortIndication(t_uint32 portIdx) ;
	virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled);
    virtual void destroy(void) ;
    virtual void eventEndAlgo(t_t1xhv_status status, t_t1xhv_encoder_info info, t_uint32 durationInTicks, t_uint32 bitstream_size);
    virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param);
	virtual void stateChangeIndication(OMX_STATETYPE oldState, OMX_STATETYPE newState);

    virtual void setParameter(t_t1xhv_algo_id algoId, ts_ddep_buffer_descriptor ddep_sec_jpeg_param_desc);
	void setNeeds(t_uint16 channelId, ts_ddep_buffer_descriptor bbm_desc, ts_ddep_buffer_descriptor debugBuffer_desc,t_uint32 temp_buf_address);
    void ReleaseBuffer(t_uint32 port_idx,OMX_BUFFERHEADERTYPE * buf);
    virtual void setConfig(t_uint16 channelId);
    t_uint16 download_parameters(void);

    void release_resources(void);
    void processActual();
	OMX_BOOL buffer_available_atoutput();
	OMX_BOOL buffer_available_atinput();
	void linkin_loop(void);
	void update_link(void);
	void create_link(ts_ddep_buffer_descriptor bbm_desc);
	void program_link(ts_ddep_buffer_descriptor bbm_desc);
	void restore_values(void);
	virtual void emptyThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer);
	virtual void fillThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer);
	virtual void processEvent(void);



    virtual t_nmf_error construct(void);
	//virtual void endAlgo(t_t1xhv_status status, t_t1xhv_encoder_info info, t_uint32 durationInTicks);
	virtual void endAlgo(t_t1xhv_status status, t_t1xhv_encoder_info info, t_uint32 durationInTicks, t_uint32 bitstream_size, t_uint32 isswjpeg);

	virtual void set_nNumber(OMX_U32 nNumber);
    virtual void eventProcess();
private:
	Port mPorts[2];

	ts_ddep_sec_jpeg_param_desc_dh *ps_ddep_sec_jpeg_param_desc_dh;
	ts_ddep_sec_jpeg_param_desc_ *ps_ddep_sec_jpeg_param_desc_host_set;
	ts_ddep_sec_jpeg_param_desc_ *ps_ddep_sec_jpeg_param_desc_tmp_set;
	ts_ddep_sec_jpeg_param_desc_ *ps_ddep_sec_jpeg_param_desc_mpc_set;

   /*************************************************************************************
	SW JPEG Encode code
	*************************************************************************************/
	//void ResolveDependencies();
};

#endif /* PARSER_HPP_ */
