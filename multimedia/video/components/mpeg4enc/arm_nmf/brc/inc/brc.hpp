/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef BRC_HPP_
#define BRC_HPP_

#include "Component.h"
#include "ENS_List.h"
#include "t1xhv_vec.idt"
#include "arm_nmf/share/vfm_vec_mpeg4.idt"
#include "OMX_Component.h"
#include <common/inc/mpeg4enc_arm_mpc_common.h>
#include <common/inc/mpeg4enc_arm_mpc_brc.h>

class mpeg4enc_arm_nmf_brc: public TraceObject,public mpeg4enc_arm_nmf_brcTemplate
{
private:
	//> variables here
	t_sva_brc_eot_fifo eotFifo;
	
	t_sva_brc_qpConstant_state *pStateQpConstant;
	t_sva_brc_cbr_state *pStateCBR;
	t_sva_brc_vbr_state *pStateVBR;
	
	t_sva_brc_qpConstant_state state_qp_constant;
	t_sva_brc_cbr_state state_cbr;
	t_sva_brc_vbr_state state_vbr;
	
	t_uint32 once_global;
	
	
	t_sva_ec_save skipFifo[3];
	
	t_bool isCurrentItSkip;
	t_bool isCurrentStrategicSkip;

public:
    //member functions
    mpeg4enc_arm_nmf_brc();
    virtual ~mpeg4enc_arm_nmf_brc();

	t_uint32 maxVOPsize(t_uint32 mbnum);
	void NoBRC_InitSeq(ts_t1xhv_vec_mpeg4_param_in* mp4_par_in, ts_ddep_vec_mpeg4_custom_param_in SHAREDMEM* mp4_custom_par_in, t_sva_timestamp_value pts, t_sva_brc_out* pbrc_par_out, t_bool *pIsPreviousSkip);
	void CBR_InitSeq(ts_t1xhv_vec_mpeg4_param_in* mp4_par_in, ts_ddep_vec_mpeg4_custom_param_in* mp4_custom_par_in, ts_t1xhv_vec_mpeg4_param_inout* mp4_par_inout, t_sva_timestamp_value pts, t_sva_brc_out* pbrc_par_out, t_bool *pIsPreviousSkip ,t_uint32 *buffer_depletion);
	void VBR_InitSeq(ts_t1xhv_vec_mpeg4_param_in* mp4_par_in, ts_ddep_vec_mpeg4_custom_param_in SHAREDMEM* mp4_custom_par_in, ts_t1xhv_vec_mpeg4_param_inout* mp4_par_inout, t_sva_timestamp_value pts, t_sva_brc_out* pbrc_par_out, t_bool *pIsPreviousSkip, t_uint32 *buffer_depletion);
	void NoBRC_InitPict(ts_t1xhv_vec_mpeg4_param_in* mp4_par_in, ts_ddep_vec_mpeg4_custom_param_in SHAREDMEM* mp4_custom_par_in, t_sva_timestamp_value pts, t_sva_brc_out* pbrc_par_out, t_bool *pIsPreviousSkip);
	void CBR_InitPict(ts_t1xhv_vec_mpeg4_param_in* mp4_par_in, ts_ddep_vec_mpeg4_custom_param_in* mp4_custom_par_in, t_sva_timestamp_value pts, t_sva_brc_out* pbrc_par_out, t_bool *pIsPreviousSkip, t_uint32 *buffer_depletion);
	void VBR_InitPict(ts_t1xhv_vec_mpeg4_param_in* mp4_par_in, ts_ddep_vec_mpeg4_custom_param_in SHAREDMEM* mp4_custom_par_in, t_sva_timestamp_value pts, t_sva_brc_out* pbrc_par_out, t_bool *pIsPreviousSkip, t_uint32 *buffer_depletion);
	void BRC_InitSeq(ts_t1xhv_vec_mpeg4_param_in* mp4_par_in, ts_ddep_vec_mpeg4_custom_param_in* mp4_custom_par_in, ts_t1xhv_vec_mpeg4_param_inout* mp4_par_inout, t_sva_timestamp_value pts, t_sva_brc_out* pbrc_par_out, t_bool *pIsPreviousSkip, t_uint32 *buffer_depletion);
	void BRC_InitPict(ts_t1xhv_vec_mpeg4_param_in* mp4_par_in, ts_ddep_vec_mpeg4_custom_param_in SHAREDMEM* mp4_custom_par_in, t_sva_timestamp_value pts, t_sva_brc_out* pbrc_par_out, t_bool *pIsPreviousSkip, t_uint32 *buffer_depletion);
	virtual void SP_GetNextFrameParamIn(ts_t1xhv_vec_mpeg4_param_in* mp4_par_in, ts_ddep_vec_mpeg4_custom_param_in* mp4_custom_par_in, ts_t1xhv_vec_mpeg4_param_inout* mp4_par_inout, t_sva_timestamp_value pts, t_uint32 frame_number, t_uint32 *buf_level, t_uint32 last_bitstream_size);
	virtual void SH_GetNextFrameParamIn(ts_t1xhv_vec_mpeg4_param_in* mp4_par_in, ts_ddep_vec_mpeg4_custom_param_in* mp4_custom_par_in, ts_t1xhv_vec_mpeg4_param_inout* mp4_par_inout, t_sva_timestamp_value pts, t_uint32 frame_number);
	
	void sva_EC_BRC_FinishPicture(t_sva_brc_in *pBrcIn);
	virtual void sva_EC_MP4_SetFrameParamOut(const ts_t1xhv_vec_mpeg4_param_out* pMpeg4ParamOut, const ts_t1xhv_vec_mpeg4_param_inout* pMeg4ParamInOut);
	virtual void set_trace(void * addr,t_uint16 id);
	virtual t_nmf_error construct(void);

private:

};

#endif /* BRC_HPP_ */
