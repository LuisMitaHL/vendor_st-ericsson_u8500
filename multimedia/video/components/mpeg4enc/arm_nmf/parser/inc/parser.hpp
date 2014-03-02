/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef PARSER_HPP_
#define PARSER_HPP_

#include "Component.h"
#include "ENS_List.h"
#include "t1xhv_vec.idt"
#include "arm_nmf/share/vfm_vec_mpeg4.idt"
#include "OMX_Component.h"
#include <common/inc/mpeg4enc_arm_mpc_common.h>
#include <common/inc/mpeg4enc_arm_mpc.h>


typedef enum
{
	NOT_WRITTEN,
	WRITTEN_ONCE,
	WRITTEN_TWICE
} t_vos_written_times;

class mpeg4enc_arm_nmf_parser: public Component, public mpeg4enc_arm_nmf_parserTemplate
{
private:
	OMX_U32 	set_flag_for_eow;
	OMX_U32 	handle_send_command_eow;
	OMX_U8 		eosReached;
	t_uint16 	get_codec_parameter;

	t_uint16 	get_hw_resource;
	t_uint16 	get_hw_resource_requested;

	t_uint32 	frame_number;

	OMX_BUFFERHEADERTYPE 	*bufIn;
	OMX_BUFFERHEADERTYPE 	*bufOut;


	OMX_U32 mStabCropVectorX[STAB_ARRAY_SIZE];
	OMX_U32 mStabCropVectorY[STAB_ARRAY_SIZE];
	OMX_U32 mStabCropWindowWidth[STAB_ARRAY_SIZE];
	OMX_U32 mStabCropWindowHeight[STAB_ARRAY_SIZE];
	OMX_U16 stab_crop_array_start;
	OMX_U16 stab_crop_array_end_plus1;

	OMX_U32     fillThisBufferCount;
	OMX_U32     emptyThisBufferCount;
	OMX_U32     fillThisBufferDoneCount;
	OMX_U32     emptyThisBufferDoneCount;

	t_uint8 	set_param_done;

	t_uint32 	prev_bitstream_size;
	t_uint16 	prev_no_of_packets;
	t_uint32 	partial_end_algo_count;

	OMX_STATETYPE compState;

	t_uint8 control_algo_called;
	t_uint8 pause_state_called;

	t_uint32 	param_desc_mpc_addr;

	t_uint16 	last_read_conf_no;
	t_uint16 	new_config_received;

	t_uint16 	once;

	t_sva_timestamp_value prev_pts;

	t_vos_written_times vos_written;
	t_uint8* bufferptr;
	OMX_U32 filledLen;
	OMX_U32 filledLenLastVOL; 

	codec_param_type param;
	ts_ddep_vec_mpeg4_ddep_desc s_mpeg4e_ddep_desc;
	t_sva_ec_save 	current_header, previous_header;
	t_uint16 previous_skipped_flag;


public:
    //member functions
    mpeg4enc_arm_nmf_parser();
    virtual ~mpeg4enc_arm_nmf_parser();

    //Component virtual functions
    virtual void process() ;

	void 		processActual();
    t_uint16 	download_parameters(void);
	virtual void destroy(void) ;
	virtual void reset();

	virtual void disablePortIndication(t_uint32 portIdx);
	virtual void enablePortIndication(t_uint32 portIdx);
	virtual void flushPortIndication(t_uint32 portIdx) ;
	virtual void stateChangeIndication(OMX_STATETYPE oldState, OMX_STATETYPE newState);
	virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled);	

	virtual void endAlgo(t_t1xhv_status status, t_t1xhv_encoder_info info, t_uint32 durationInTicks);
    virtual void eventEndAlgo(t_t1xhv_status status, t_t1xhv_encoder_info info, t_uint32 durationInTicks);

	virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param);

	void setNeeds(t_uint16 channelId, ts_ddep_buffer_descriptor bbm_desc, ts_ddep_buffer_descriptor debugBuffer_desc);
	virtual void setParameter(t_t1xhv_algo_id algoId, ts_ddep_buffer_descriptor ddep_vec_mpeg4_param_desc);
	virtual void fsmInit(fsmInit_t initFSM);
	void ReleaseBuffer(t_uint32 port_idx,OMX_BUFFERHEADERTYPE * buf);
	void release_resources(void);
	virtual void setConfig(t_uint16 channelId);

	OMX_BOOL buffer_available_atoutput();
	OMX_BOOL buffer_available_atinput();

	virtual void emptyThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer);
	virtual void fillThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer);

	virtual void processEvent(void);

    virtual t_nmf_error construct(void);

    virtual void eventProcess();

	virtual void sendEventEOS(t_uint32 flags);

	virtual void setCropForStab(t_uint32 crop_left_offset, t_uint32 crop_top_offset, t_uint32 overscan_width, t_uint32 overscan_height);

	void WriteBits(t_sva_ec_writestream_header *pStream, t_uint32 data, t_uint32 nbBits);
	void flushWriteStream(t_sva_ec_writestream_header *pStream);
	t_uint32 SH_headerCreation(ts_t1xhv_vec_mpeg4_param_in *pMecIn_parameters,
						   ts_ddep_vec_mpeg4_custom_param_in *pMecIn_custom_parameters,
						   t_sva_ec_save *ap_current_header, t_sva_ec_save *ap_previous_header,
						   t_sva_timestamp_value pts);
	t_uint32 SP_headerCreation(ts_t1xhv_vec_mpeg4_param_in *pMecIn_parameters,
						   ts_ddep_vec_mpeg4_custom_param_in *pMecIn_custom_parameters,
						   t_sva_ec_save *ap_current_header, t_sva_ec_save *ap_previous_header,
						   t_sva_timestamp_value pts, t_uint32 buf_level);

	t_uint32 Write_VOS_VO_VOL(ts_t1xhv_vec_mpeg4_param_in *pMecIn_parameters,
						   ts_ddep_vec_mpeg4_custom_param_in *pMecIn_custom_parameters, t_uint16 *addr_pointer);

	t_sva_timestamp_value HclTicksFromMicroSeconds( t_sva_timestamp_value pts );

private:
	Port mPorts[TOTAL_PORT_COUNT];

	ts_ddep_vec_mpeg4_param_desc_dh *ps_ddep_vec_mpeg4_param_desc_dh;
	ts_ddep_vec_mpeg4_param_desc *ps_ddep_vec_mpeg4_param_desc_host_set;
	ts_ddep_vec_mpeg4_param_desc *ps_ddep_vec_mpeg4_param_desc_tmp_set;
	ts_ddep_vec_mpeg4_param_desc *ps_ddep_vec_mpeg4_param_desc_mpc;

   /*************************************************************************************
	SW JPEG Encode code
	*************************************************************************************/
	//void ResolveDependencies();
};

#endif /* PARSER_HPP_ */
