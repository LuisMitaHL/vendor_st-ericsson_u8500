/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __fileReader_cpp
#define __fileReader_cpp
#include "h264dec/arm_nmf/inc/settings.h"
#include "h264dec/arm_nmf/inc/types.h"
#include "h264dec/arm_nmf/inc/host_types.h"
#include "h264dec/arm_nmf/inc/hamac_types.h"
#include "h264dec/arm_nmf/inc/host_decoder.h"
#include "h264dec/arm_nmf/inc/host_sei.h"

#include "host_input_control.h"
#include "host_hamac_pipe.h"
#include "error_map.h"

#include "TraceObject.h"

enum StatusVCL  {VCL_STOP,VCL_ERROR,VCL_OK };
typedef enum StatusVCL t_vclstatus;

enum StatusSlices  {PICTURE_SIZE_CHANGE,SLICES_OK,NO_SLICES};

typedef enum StatusSlices t_slices_status;

class h264dec_arm_nmf_decoder: public h264dec_arm_nmf_decoderTemplate, public TraceObject
{
    /* globals */
    
    /* methods private */
    
public:
virtual	t_nmf_error construct(void);
virtual	void 		destroy(void);
virtual void 		stop(void);
virtual void 		start(void);
    
/* methods itf */
public:
    h264dec_arm_nmf_decoder();
    
// Interfaces
virtual void 	nal(t_bit_buffer *p_b,void *sei_val, t_uint32 nTimeStampH, t_uint32 nTimeStampL, t_uint32 nFlags);
virtual void 	input_decoder_command(t_command_type command_type);
virtual void 	output_decoder_command(t_command_type command_type);
//virtual void    endCodec(t_t1xhv_status a, t_t1xhv_decoder_info b, t_uint32  duration);
virtual void    endCodec(t_uint32 a, t_uint32 b, t_uint32  duration);
virtual void   	output_buffer(void *ptr,t_uint32 size16);
virtual void  initTraceInfo(TraceInfo_t * ptr, unsigned int val) {setTraceInfo(ptr,val);}
virtual void traceInit(TraceInfo_t trace_info, t_uint16 id);


// Internal function due to NMF C++ framework
void    CheckLevelVsFrameSize(t_seq_par *p_s);
//+Change start for CR336095,ER334368,ER336290 level 5.1. support
t_uint8  CheckLevelVsDPBSize(t_seq_par *p_sp);
t_uint8 CheckLevelVsResolution(t_seq_par *p_sp);
t_uint16 CheckLevel(t_uint32 level,t_uint16 profile);
//-Change end for CR336095,ER334368,ER336290 level 5.1. support
t_uint16 GetNALUType(t_bit_buffer *p_b, t_uint16 *p_nal_ref_idc, t_uint16 *p_nal_unit_type, t_uint16 is_mvc_stream);
t_uint16 GetSequenceParSet(t_bit_buffer *p_b, t_sint16 *p_nsp, t_seq_par *p_s, t_seq_par *tmp_sps, t_uint16 * pending_sps_update);
t_uint16 GetSubsetSequenceParSet(t_bit_buffer *p_b, t_sint16 *p_nsubsetsp, t_subset_seq_par *p_subset_sp, t_uint16 * is_mvc_stream);
void	ParseNonVCL(t_bit_buffer *p_b,t_dec *dec,t_SEI *sei);
t_uint16 GetSliceHeader(t_bit_buffer *p_b, t_dec_buff *buf, void *p_q, t_pic_par *p_p, t_uint16 num_pic_par, 
                        t_uint16 num_seq_par, t_uint16 num_subset_seq_par,
                        t_uint16 first_slice, t_old_slice *p_old_sl, t_slice_hdr *p_s);
t_uint16 GetPictureParSet(t_bit_buffer *p_b, t_sint16 *p_npp, t_pic_par *p_p, t_sint16 *p_nsp, t_seq_par *p_s);
t_uint16 GetVUI(t_bit_buffer *p_b, t_vui_data *p_vui);
t_uint16 GetMVCVUI(t_bit_buffer *p_b, t_mvc_vui_data *p_mvc_vui);
t_uint16 GetSEI(t_bit_buffer *p_b, t_dec * dec, t_SEI *p_sei);
t_uint16 ByteAlign(t_bit_buffer *p_b);
t_uint16 GetStereoVideoInfo(t_bit_buffer *p_b,t_stereo_video_info_SEI *p_stereo_video_info_SEI);
t_uint16 GetFramePackingArrangement(t_bit_buffer *p_b,t_frame_packing_arrangement_SEI *p_frame_packing_arrangement_SEI);
t_uint16 GetPanScanRectInfo(t_bit_buffer *p_b, t_panscan_SEI *panscan_sei);
t_uint16 GetRecoveryPoint(t_bit_buffer *p_b, t_rp_SEI *rp_sei);
t_uint16 GetBufferingPeriodSEI(t_bit_buffer *p_b, t_dec *dec, t_SEI *p_SEI);
t_uint16 GetPictureTimingSEI(t_bit_buffer *p_b, t_SEI *p_SEI);

t_uint16	DecodeEndOfFrame(t_dec *dec);
void		NalEndProcessing(t_bit_buffer *p_b, t_dec *dec, t_SEI  *sei, t_uint32 size);

t_uint32   AllocateESRAM(void *vfm_memory_ctxt,t_uint32 mbx,t_uint32 mby, t_dec_buff *p_buff);
void  FreeESRAM(void *vfm_memory_ctxt, t_dec_buff *p_buff);

void 		FrameDecode(t_dec *dec,t_bit_buffer *p_b,t_SEI *sei,t_uint32 nTimeStampH,t_uint32 nTimeStampL,t_uint32 nFlags,t_uint32 size);

void	nal_process(t_dec *dec,t_bit_buffer *p_b,t_SEI *sei, t_uint32 nTimeStampH, t_uint32 nTimeStampL, t_uint32 nFlags);
t_uint32  CreateNalList(t_bit_buffer *p_b);

t_uint32    get_frame_size_from_active_sps(void);
t_sint16	BumpFrame(t_dec_buff*);
t_uint16 	RemoveUnusedFrame(t_dec_buff *p_buff);
t_sint16	InsertFrame(t_uint16 frame_num, t_uint32 MaxFrameNum, t_seq_par *p_sp, t_dec_buff *p_buff);
void 		MarkPicture(t_seq_par *p_sp, t_slice_hdr *p_sh, t_dec_buff *p_buff);
t_uint16	GenerateList(t_seq_par *p_sp, t_pic_par *p_pp, t_slice_hdr *p_sh, t_dec_buff *p_buff);
t_uint16	ConcealFrame(t_uint16 prevNum, t_uint16 pos, t_seq_par *sp, t_dec_buff *p_buff);
void	 	display_queue_put(t_dec_buff *p_buff, t_dpb_info *dpb_info, t_frames *frame) ;
t_uint16	NewFrame(t_seq_par *p_sp, t_slice_hdr *p_sh, t_dec_buff *p_buff, t_uint32 nTimeStampH, t_uint32 nTimeStampL, t_uint32 nFlags,t_SEI*sei);

t_uint32 	hamac_pipe_bump_frames(t_dec *dec); 
t_uint32  aux_hamac_pipe_bump_frames(t_dec *dec);
void 		hamac_pipe_send_frame_eos(t_dec *dec);
void 		hamac_pipe_output_flush(t_dec *dec);
void 		hamac_pipe_void_frame(t_dec *dec, t_void_frame_info void_frame_info);
void    DPB_Revert(t_dec_buff *p_buff);
void 		FlushDPB(t_dec_buff *p_buff, t_sint16 keep_frame);
void		NewIDR(t_seq_par *p_sp, t_slice_hdr *p_sh, t_dec_buff *p_buff, t_uint32 nTimeStampH, t_uint32 nTimeStampL, t_uint32 nFlags,t_SEI*sei);
void  		decoder_error(t_dec *dec, t_uint32 nFlags);
void  		ComputeParamIn(t_dec *dec, t_uint16 slices);
void 		display_queue_output_picture(t_frames *frame, t_picture_output picture_type, t_frameinfo *p_frameinfo);
void 		display_queue_new_output_buffer(t_dec *dec, void *ptr);
void    display_queue_reset_frames_array(t_dec_buff *p_buff);
t_uint16    display_queue_init_frames_array(t_dec_buff *p_buff, t_uint16 max_nb_frames, void *vfm_memory_ctxt);
void    display_queue_free_frames_array(t_dec_buff *p_buff,void *vfm_memory_ctxt);

void	DecodeEndOfFrame(void);
void	VoidEndOfFrame(void);

void debug_frames_info(char *s);
void input_control_acknowledge_pending(t_bit_buffer *p_b);
void input_control_acknowledge(t_bit_buffer *p_b,t_handshake mode);
void input_control_check_buffer_list(t_bit_buffer *p_b);

void 		NalEndDecoding(t_dec *,t_bit_buffer *, t_uint32 nFlags);

void display_queue_remove_unreferenced_frame(t_dec_buff *p_buff, t_uint16 dpb_loc);
void remove_unmarked_displayed_frames(t_dec_buff *p_buff, t_sint16 pos_dpb);
t_sint16 display_queue_get_frame_from_free_pool(t_dec_buff *p_buff, t_uint32 size16);
t_uint32 pipe_is_ready();
t_uint32 AllocateMemory(void *vfm_memory_ctxt,t_uint32 mbx,t_uint32 mby, t_dec_buff *p_buff);
void FreeMemory(void *vfm_memory_ctxt,t_dec_buff *p_buff);
t_uint16 init_decoder(t_dec *dec, void *vfm_memory_ctxt);
void close_decoder(t_dec *dec,void *vfm_memory_ctxt);
void InsertDummyRefIntoDPB(t_dec_buff *p_buff, t_uint16 prevNum, t_seq_par *p_sp);
void SelectActiveSPS(void);
void NoSliceAvailable(void);
void	DiscardSliceArray(t_sint32 slices);
void	DiscardSliceArrayOnError(t_sint32 slices);
t_slices_status	ProcessAllSlices(t_SEI *sei,t_uint32 nTimeStampH,t_uint32 nTimeStampL,t_uint32 nFlags,t_uint32 *slices);
void FireFrameDecode(t_sint32 slices,t_bit_buffer *p_b,t_SEI *sei,t_uint32 size);
t_vclstatus	ProcessVideoNAL(t_dec *dec,t_bit_buffer *p_b,t_uint16 nri,t_uint32 *use_by_hw);
void BumpPendingFrame(t_dec *dec, t_uint32 nFlags) ;
void InputControlAdd2List(t_dec *dec, t_bit_buffer *p_b,t_uint8 force_add);    //ER402496
void InputControlInitList(t_dec *dec, t_bit_buffer *p_b) ;
void FillErrorMap(t_hamac_param *hamac_param);
t_sint16 is_marked_frame(t_dec_buff *p_buff, t_dpb_info *dpb_info); 
t_sint16 is_dpbused_frame(t_dec_buff *p_buff, t_frames *frame);
void InitBuffer(t_dec_buff *p_buff);
t_uint16	HamacAllocParameter(void *vfm_memory_ctxt,t_hamac_param *t,t_uint16 n);
void	HamacDeallocParameter(void *vfm_memory_ctxt,t_hamac_param *t, t_uint16 n);
void HamacFillParamIn(void *local_vfmmemory_ctxt,t_dec *dec, t_uint16 nslices,t_hamac_param	*t);
void HamacToLogicalAddresses(void *local_vfmmemory_ctxt,t_hamac_param *t, t_uint16 n);
void HamacToPhysicalAddresses(void *local_vfmmemory_ctxt,t_hamac_param *t, t_uint16 n);
void HamacFillTaskDescriptor(void *local_vfmmemory_ctxt, t_dec *dec, t_hamac_param *t);
void PrepareHedParameters(void *local_vfmmemory_ctxt,t_hamac_param *t, t_hed_param *hed);
void error_recovery(void);

#include "host_input_control.hpp"
#include "host_hamac_pipe.hpp"
#include "host_decoder.parameter.hpp"

	public:
	t_dec 		dec_static;
    void		*vfm_memory_ctxt;
    t_uint16    mDblkMode;          // Deblocking mode
    t_uint32    mSupportedExtension;
    Common_Error_Map *mErrorMap;    // error map
    TraceInfo_t  dummy_TraceInfo;
};



#endif

