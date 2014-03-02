/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifdef NMF_BUILD
#include <h264dec/arm_nmf/decoder.nmf>
#endif

extern "C" {
//FIXME: this can be removed once NMF correctly exports this function
#if ((!defined __SYMBIAN32__) && (!defined IMPORT_C))
  #define IMPORT_C
#endif
#if ((!defined __SYMBIAN32__) && (!defined EXPORT_C))
  #define EXPORT_C
#endif

extern IMPORT_C void NMF_PANIC(const char* fmt, ...);

}

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "video_components_h264dec_arm_nmf_decoder_src_host_decoderTraces.h"
#endif

#include "settings.h"
#include "types.h"
#include "hamac_types.h"
#include "lifecyclecheck.h"
#include "host_decoder.h"
#include "macros.h"
#include "interface.h"
#include "common_bitstream.h"
#include "host_bitstream.h"
#include "host_sei.h"
#include "host_block_info.h"
#include "hamac_bitstream.h"
#include "hamac_deblocking.h"
#include "hamac_concealment.h"
#include "host_sh_concealment.h"
#include "host_mem.h"
#include "host_DPB.h"
#include "host_fmo.h"
#include "host_decoder.h"
#include "host_hamac_pipe.h"
#include "user_interface.h"
#include "local_alloc.h"
#define INLINEWORKARROUND	1
#include "host_input_control.h"
#include <omx_define.h>
#include "ost_event.h"
#include "host_frameinfo.h"

#ifdef __ndk5500_a0__
#include "hva_cmds.h"
#endif

#define where(a,b)	a#b

#define Endianess(a)	((((t_uint32)(a) & 0xffff )<<16) + (((t_uint32)(a)>> 16) & 0xffff ))

#if VERBOSE_IB == 1
static void PrintIntBuf(t_dec * dec);
#endif
/**
 * \brief Decoder main function.
 */

#if VERBOSE_STANDARD == 1
static	t_uint16	frame_num=0;
#endif
#if  CHECK_SYNCHRONE_HW
  static volatile t_uint32 hw_is_busy=0;
#endif


#if CHECK_LIST
void check_list(char *s,t_bit_buffer *p)
{
    t_bit_buffer *p_b0=p;
    while(p)
    {
        p=(t_bit_buffer*)p->next;
        if (p==p_b0)
        {
            OstTraceFiltInst2( TRACE_FLOW ,"H264DEC: arm_nmf: decoder: ERROR Reloop %s  %p\n",s,p);
            while(1);
        }
    }
}
#endif

#ifdef DEBUG_NAL
t_dec	*mydec;
t_uint32	nalct=0;
void checksum(t_uint32 ct,t_bit_buffer *pb)
{
    t_uint8	*p;
    t_uint32	i,n,sum=0;
    p=pb->addr;
    nalct++;
    do
    {
        sum+=*p++;
    }
    while(p!=pb->end);
    NMF_LOG("F %d N %d checksum %8x\n",mydec->nframes,nalct,sum);


}
#endif

#if VERBOSE_DISPLAY_NAL
t_uint32	nalctt=0;
void display_nal(t_bit_buffer *pb)
{
    t_uint8	*p;
    t_uint32	ct,i,n;

    ct=pb->end-pb->addr;
    p=pb->addr;
    NMF_LOG("nal %p  [%d] size=%d =>",pb->addr,++nalctt,ct);
    n=MIN(ct,4);
    for(i=0;i<n;i++) NMF_LOG("%2x ",*p++);
    NMF_LOG("...");
    p=pb->addr+ct-1;
    n=MIN(ct,4);
    p-=n-1;
    for(i=0;i<(n+2);i++) NMF_LOG("%2x ",*p++);
    NMF_LOG("\n");
}

#endif




//#define NAL_MODE
METH(h264dec_arm_nmf_decoder)() : mDblkMode(1), mSupportedExtension(0), mErrorMap(0)
{
    dec_static.maxslices = 0;
    dec_static.old_sp.level_idc=42;
    vfm_memory_ctxt = 0;
    dec_static.gone_to_sleep = 1; // by default sleep is allowed
    dec_static.esram_allocated = 0;
    dec_static.is_mvc_stream = 0;
    dec_static.buf.frames = (t_frames*)NULL;

    // The following is needed to avoid issues with uninitialized trace objects
    dummy_TraceInfo.parentHandle = dummy_TraceInfo.traceEnable = dummy_TraceInfo.dspAddr = 0;
    setTraceInfo(&dummy_TraceInfo,0);
}

void METH(traceInit)(TraceInfo_t trace_info, t_uint16 id)
{
  iTraceInitOut.traceInit(trace_info,id);
}

t_uint16 METH(init_decoder)(t_dec *dec, void *vfm_memory_ctxt)
{
  OstTraceFiltInst0( TRACE_FLOW , "H264DEC: arm_nmf: decoder: init_decoder\n");
#ifdef DEBUG_NAL
  mydec=dec;
#endif
    // _vfmmemory_ctxt = VFM_CreateMemoryList();
    dec->bImmediateRelease = 0;
    dec->num_seq_par =0;
    dec->num_pic_par = 0;
  dec->num_subset_seq_par = 0;
  dec->pending_sps_update = 0;

    dec->next_idr=1;
    dec->buf.HED_used = 3;

    dec->sh_status=0;
    dec->stop=0;
    dec->failure=0;
    dec->buf.curr_3d_format = 0;
  dec->is_mvc_stream = 0;

  dec->g_last_timestamp = 0;
  dec->g_start_time = 0;
  dec->g_first_nal_after_flush = 1;
  dec->g_wrong_timestamps = 0;
  dec->g_no_timestamps_from_demuxer = 1;


    dec->first_slice = 1;
    dec->sps_active = 0;
    dec->old_sp.pic_width_in_mbs_minus1=0;
    dec->old_sp.pic_height_in_map_units_minus1=0;
    dec->old_sp.frame_mbs_only_flag=1;
    dec->slice_num = -1;
    dec->nframes=0;
    dec->prevSh = (t_slice_hdr *)NULL;
    dec->use_rs = 0;
    /* Allocate slice headers array */
    if (dec->maxslices==0) {
        dec_static.maxslices = MAXNSLICE;
        AllocateSliceArray(vfm_memory_ctxt,&dec->sh);
    }

    InitOldSlice(&dec->old_sl);
    dec->buf.initialized = 0;
    dec->host.buff = &dec->buf;

    input_control_init(&dec->input_control);
//Change Start for ER357298
    //input_control->dpb_frame_available--;
                                             /* in case of seek we need one output buffer morein order to start */
//Change End for ER357298
    hamac_pipe_init(&dec->hamac_pipe);
    aux_hamac_pipe_fifo_init();
    dec->p_active_sp=0;
    dec->buf.DPBsize = 0;
// +CR324558 CHANGE START FOR
    dec->buf.curDPBSize=0;
// -CR324558 CHANGE END OF
    dec->buf.AllocatedDPBsize = 0;
    dec->buf.g_seek_mode = 0;
    dec->DBLK_mode = mDblkMode;
    dec->sva_bypass = 0;
    dec->buf.g_decoded_frm_ctr = 0;
    dec->aux_counter = 0;
    InitBuffer(&dec->buf);
    if(dec->buf.frames == NULL){
         t_uint16 error = display_queue_init_frames_array(&dec->buf, MAXNUMFRM+HAMAC_PIPE_SIZE+1+DISPLAY_QUEUE_SIZE+DEF_FTB_SIZE, vfm_memory_ctxt);
         if(error) {
             dec->buf.memoryAllocationFailed=1;
             return 1;
         }
    }
    dec->buf.memoryAllocationFailed=0;
    display_queue_reset_frames_array(&dec->buf);
    picture_parameter_init();
    return 0;
}

void METH(close_decoder)(t_dec *dec,void *vfm_memory_ctxt)
{
  OstTraceFiltInst0( TRACE_FLOW , "H264DEC: arm_nmf: decoder: close_decoder\n");
  if(hamac_pipe_is_empty()){
	 FreeMemory(vfm_memory_ctxt,&dec->buf);
	}
    if (vfm_memory_ctxt) {
        display_queue_free_frames_array(&dec->buf,vfm_memory_ctxt);
    }
    FreeSliceMemory(vfm_memory_ctxt,dec->sh);
}


t_nmf_error METH(construct)(void)
{
    MeasureEventInit();
    return NMF_OK;
}


void METH(start)(void)
{
}

void METH(stop)(void)
{
}

void METH(destroy)(void)
{
  OstTraceFiltInst0( TRACE_FLOW , "H264DEC: arm_nmf: decoder: destroy\n");
    // FIXME: codec->stop() is not automatically called by NMF
    MeasureEventDisplayStatus();
    close_decoder(&dec_static,vfm_memory_ctxt);
}

void  METH(decoder_error)(t_dec *dec, t_uint32 nFlags)
{
  OstTraceFiltInst0( TRACE_FLOW , "H264DEC: arm_nmf: decoder: decoder_error\n");
	//NMF_PANIC("ENTER DECODER ERROR\n");
	dec->failure = 1;
	if (dec->sps_active)
	{
//FIXME not needed JMG		hamac_pipe_reset_frame();
		FlushDPB(&dec->buf, -1);
	}
	BumpPendingFrame(dec,nFlags);
}

static void timing_update(t_SEI *sei, t_uint32 size)
{
#if ENABLE_TIMING == 1
    sei->timings.bits += (size << 3);
#endif
}
#if 0
void output_checksum(t_seq_par *p_sp, t_uint8 *p)
{
    t_uint32	sum=0;
    static t_uint32	picture_ct=0;
    t_uint32 size = ((p_sp->pic_width_in_mbs_minus1+1)*(p_sp->pic_height_in_map_units_minus1+1)) << 8;
    t_uint32	osize;

    size+=size/2;
    osize=size;
    picture_ct++;
    while (size--) sum+=*p++;
    NMF_LOG("Output checksum[%d] %08X on %d byte\n",picture_ct,sum,osize);
}
#endif
#if VERBOSE_SLICE_EVENT>0
t_uint32                      abs_slice_count=0;
#endif

void ComputeConcealParam(t_slice_hdr *p_slice, t_host_info *p_host,t_dec *dec)
{
    t_uint16 i;
    t_sint32 poc = MIN_SINT_32;
    t_sint16 index;

    /* Addresses of reference frame buffer components */
    p_host->buff->ref_frame = 0;
    p_host->buff->intra_conc = 1;


    if (p_slice->nut != 5)
    {

        // 5 means P slice
        if ((p_slice->slice_type == 0) || (p_slice->slice_type == 5))
        {
            /* If P slice use list0 first entry */
            p_host->buff->ref_frame = (t_address)p_host->buff->list0[0][0];

            /* Tell Hamac to check for Scene Change P  */
            p_host->buff->intra_conc = 2;
        }
        else
        {
            /* If I slice search for highest POC in DPB */
            index = -1;
            for (i = 0; i < p_host->buff->DPBsize+1; i++)
            {
                if (((p_host->buff->pics_buf[i].marked_short) || (p_host->buff->pics_buf[i].marked_long)) && (p_host->buff->pics_buf[i].poc > poc) && (p_host->buff->pics_buf[i].frame_pos != -1))
                {
                    poc = p_host->buff->pics_buf[i].poc;
                    index = i;
                }
            }

            if (index > -1)
            {
                p_host->buff->ref_frame =  (t_address)p_host->buff->pics_buf[index].ptr[0];

                /* Tell hamac to check for scene change I */
                p_host->buff->intra_conc = 3;
            }
        }
    }

//	NMF_LOG("intra_conc=%d\n",p_host->buff->intra_conc);
//	if (p_host->buff->ref_frame) output_checksum( dec->p_active_sp,(t_uint8*) p_host->buff->ref_frame);
}


void METH(input_decoder_command)(t_command_type command_type) {
    OstTraceFiltInst1( TRACE_FLOW , "H264DEC: arm_nmf: decoder: input_decoder_command %d\n",command_type);
    if (get_decoder_state() == DECODER_STATE_FLUSH) {
        OstTraceInt0( TRACE_ERROR , "H264DEC: arm_nmf: decoder: Command received while in wrong state\n");
        NMF_PANIC("Command received while in wrong state\n");
    }
    if ((command_type == COMMAND_FLUSH)||(command_type == COMMAND_PAUSE)) {
        output_decoder_command(command_type);
    }

}

// On a flush command , we post a void frame (fake frame to go through the pipe)
// Then we flush the DPB , so all available frames are added to the list of frame to be bumped
// The void frame will be recognized as a FLUSH frame in the output stage
// this will force the return of all the output buffers available in the free pool of buffer
// input buffer are mechanically return as soon as a frame is decoded


void METH(output_decoder_command)(t_command_type command_type) {
    t_uint32 tmp_offset;

  OstTraceFiltInst1( TRACE_FLOW , "H264DEC: arm_nmf: decoder: output_decoder_command %d\n",command_type);

	if (get_decoder_state() == DECODER_STATE_FLUSH) {
    OstTraceInt0( TRACE_ERROR , "Command received while in wrong state\n");
		NMF_PANIC("Command received while in wrong state\n");
	}
	if (command_type == COMMAND_FLUSH) {

	if(get_decoder_state()==DECODER_STATE_NONE)
	{
    OstTraceFiltInst0( TRACE_FLOW , "H264DEC: arm_nmf: decoder: FLUSH\n");

		//NMF_LOG("Received flush command\n");
		set_decoder_state(DECODER_STATE_FLUSH);

		// Here, when a flush command comes, potentially there is no more place in the hamac_pipe.
		// But... we need the hamac pipe just to store the frames to be bumped by FlushDPB.
    // So, what we do is to go to next position (which potentially is the read ptr if pipe is full)
    //  then we retrieve the number of frames already stacked in the bump list (tmp_offset)
    //  then we flush DPB (this will add frames to be bumped on top)
    //  then we copy the bump list (starting from the offset of the already stacked frames) into aux pipe
    //  ... and then we rewind hamac pipe
    // Note that aux_hamac_pipe_fifo_copy_bumped_frames() will restore the original nbr of frames to be bumped
		hamac_pipe_next_write();
		tmp_offset = hamac_pipe_bumped_frames_number();
    FlushDPB(&dec_static.buf, -1 /*none */); // Flush the DPB
		aux_hamac_pipe_fifo_copy_bumped_frames(tmp_offset);   // frames will be bumped through void frame callback
		aux_hamac_pipe_fifo_push(VOID_FRAME_OUTPUT_FLUSH,dec_static.aux_counter);
		hamac_pipe_rewind_write();

            if (dec_static.sva_bypass == 1) {
#ifdef __ndk5500_a0__
                endCodec(HVA_ERROR_NONE, (t_uint32)VOID_FRAME, 0);
#else
                endCodec(STATUS_JOB_COMPLETE, (t_uint32)VDC_VOID, 0);
#endif
            }
            else {
#ifdef __ndk5500_a0__
                iStartCodec.voidFrame((t_uint32)this,  (t_uint32)SYNC_HEC_HVC);
#else
                iStartCodec.voidFrameHed();
#endif
            }
        } // DECODER STATE NONE, normal flush
        else
        {
            OstTraceFiltInst0( TRACE_FLOW , "H264DEC: arm_nmf: decoder: FLUSH after param change\n");

            //SPECIAL FLUSH AFTER PARAM CHANGE
            aux_hamac_pipe_fifo_copy_bumped_frames(0);   // frames will be bumped through void frame callback
            aux_hamac_pipe_fifo_push(VOID_FRAME_OUTPUT_FLUSH,dec_static.aux_counter);
            //hamac_pipe_next_write();
#ifdef __ndk5500_a0__
            iStartCodec.voidFrame((t_uint32)this,  (t_uint32)SYNC_HEC_HVC);
#else
            iStartCodec.voidFrameHed();
#endif
        }

        }  /* FLUSH */
        else if(command_type == COMMAND_PAUSE)
        {
            OstTraceFiltInst0( TRACE_FLOW , "H264DEC: arm_nmf: decoder: PAUSE\n");

            /*
             * This is just a helper for the ddep. When this command is sent, no more NALs are given to
             * the codec, the ddep will wait for acknowledge of this command telling him that decoder
             * has no more buffers under processing
             * So concretely we just post a void frame and send ack to ddep when it comes back.
             * This "pause" info is also used for power management purpose by the decoder
             */

            aux_hamac_pipe_fifo_reset_frame();
            aux_hamac_pipe_fifo_push(VOID_FRAME_PAUSE_INFO,dec_static.aux_counter);
#ifdef __ndk5500_a0__
            iStartCodec.voidFrame((t_uint32)this,  (t_uint32)SYNC_HEC_HVC);
#else
            iStartCodec.voidFrameHed();
#endif
        }
}


void METH(nal)(t_bit_buffer *p_b,void *sei_val, t_uint32 nTimeStampH, t_uint32 nTimeStampL, t_uint32 nFlags)
{
    t_SEI		*sei=(t_SEI*)sei_val;
    t_uint32 		nalFlags=0;

    if (get_decoder_state() != DECODER_STATE_NONE) {
        OstTraceFiltInst0( TRACE_WARNING , "H264DEC: arm_nmf: decoder: Nal received while in wrong state\n");
        input_control_acknowledge(p_b, XOFF);
        return;
    }

    // MeasureEventStart0(TRACE_EVENT_BUFFER_START,"decoder buffer processing");

#if VERBOSE_INPUT_CONTROL >= 1
    static	int nal_count=0;
    if (p_b) NMF_LOG("Get input buffer %p  %d\n",p_b,++nal_count);
    //check_list("nal",dec_static.input_control.bit_buffer_list);
#endif

  OstTraceFiltInst1( TRACE_FLOW , "H264DEC: arm_nmf: decoder: nal() %x\n",(t_uint32)p_b);

    // if p_b is null ,then decoder has reiceived last buffer
    if (p_b)
    {
        t_nal_boundaries *nal_boundaries;

#ifdef DEBUG_NAL
        checksum(dec_static.nframes,p_b);
#endif
#if	VERBOSE_DISPLAY_NAL
        display_nal(p_b);
#endif


        p_b->next=0;
        // Mark p_b as inuse by SW
        //p_b->inuse=NONVCL_UNDER_PROCESSING;
        //NMF_LOG("Mark %p USE BY SW\n",p_b);

        // Clear EndOfStream flags , should be use only for last nal (null pointer)  compressed buffer
        nFlags=nFlags & (~OSI_OMX_BUFFERFLAG_EOS);

        // Loops on all NAL in current buffer
        nal_boundaries=(t_nal_boundaries*) p_b->nal_boundaries;
        for(t_uint32 i=0;i<nal_boundaries->nal_count;i++)
        {
            p_b->addr=	nal_boundaries->nal_start[i];;
            p_b->end=	nal_boundaries->nal_start[i+1];
            p_b->os=8;

            if (i==(nal_boundaries->nal_count-1))
            {
                nalFlags=nFlags | OSI_OMX_BUFFERFLAG_EOF;
                //nalFlags=OSI_OMX_BUFFERFLAG_EOF ;
            }
            else
            {
                nalFlags=nFlags;
            }

            //Send data NAL per NAL
            nal_process(&dec_static,p_b,sei,nTimeStampH,nTimeStampL,nalFlags);

        }
        // Mark nal_boundaries as unused
        nal_boundaries->nal_count=0;

        if(dec_static.stop==1)
        { // if an error happened (stream not supported ... etc, ack current buff and XOFF)
// +ER373259
#ifndef NAL_MODE
          if ((dec_static.failure==3) && ((p_b->inuse& VCL_UNDER_PROCESSING) == 1)) {
              p_b->inuse=0;
          }
#endif
// -ER373259
          input_control_acknowledge(p_b,XOFF);
          if(dec_static.failure==2)
            nal_ctl.fatal_decoding_error(1); // ESRAM allocation failed
          else
            nal_ctl.fatal_decoding_error(0); // bitstream not supported/fatal parsing error
          return;
        }

        // Send XON if "decoder input fifo queue" is not full and if there are available output image
        // mark p_b as no more used by SW , but may be by HW
        if (get_decoder_state() == DECODER_STATE_NONE) {
            input_control_acknowledge(p_b,XON);
        }
        else
        {
            input_control_acknowledge(p_b,XOFF); // don't send XON if we are in PICCHANGE or FLUSH state
            dec_static.buf.keep_pb = p_b; // the current input buffer must be given back to ddep after the flush
        }
    }
    else
    {
#if VERBOSE_INPUT_CONTROL >= 1
        NMF_LOG("Get Last NAL (null pointer)\n");
#endif
    OstTraceFiltInst0( TRACE_FLOW , "H264DEC: arm_nmf: decoder: Get Last NAL (null pointer)\n");

		// The emitter send a Null NAL to signal the EOS , this is used to inform the decoder that he got all the NAL
		// of the current frame , so it can decode it and mark the last frame to display as EOS. It means it has to
		// search in DPB for the last one to display.
		// In case last NAL is a non vcl and DPB is emptied (already flushed) then the decoder can not marked a picture
		// as EOS , it has to create a dummy picture marked as corrupted and EOS.
		nal_process(&dec_static,0,sei,0,0,nFlags |OSI_OMX_BUFFERFLAG_EOS);
		//input_control_acknowledge(0);
	}
	//MeasureEventStop(TRACE_EVENT_BUFFER_STOP,"decoder buffer processing");
}


void	METH(nal_process)(t_dec *dec,t_bit_buffer *p_b,t_SEI *sei,t_uint32 nTimeStampH, t_uint32 nTimeStampL, t_uint32 nFlags)
{
    t_uint16 status;
    t_uint16 nri;
    t_uint32 size=0 ;
    //MeasureEventStart0(TRACE_EVENT_2_START,"start nal");

#if VERBOSE_SLICE_EVENT>0
    abs_slice_count++;
#endif

    // Increment hw pipe pointer to point on a free location of
    // the hw fifo (use to post the jobs to HW)
    hamac_pipe_reset_frame();

    // if decoder has been stop , then DPB has to be flush
    // and frames must be sent back to proxy
    if (dec->stop) {
        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: arm_nmf: decoder: stop is set\n");
        //NMF_PANIC("STOP is set\n");
        BumpPendingFrame(dec, nFlags);
        //MeasureEventStop(TRACE_EVENT_2_STOP,"start nal");
        return;
    }

    // Proxy sent NILL pointer to signal Last frame or flush
    if (p_b==0)
    {
        dec->stop=1;
        // if there is at list one pending slice to decode , force the decoder to run
        if (dec->slice_num >= 0)
        {
            t_uint32	slices;
            // analyze the pending slices
            t_slices_status	status=ProcessAllSlices(sei,nTimeStampH, nTimeStampL, nFlags,&slices);

            if (status==SLICES_OK)
            {
                // Post frame perform decoding to HW ,
                // this call will also flushDPB
                //MeasureEventStop(TRACE_EVENT_2_STOP,"start nal");
                FireFrameDecode(slices,p_b,sei,size);
                return;
            }
        }
        else
        {
            // no slices to decode
            dec->first_slice = (dec->nut != 1) & (dec->nut != 5);
            if (dec->sps_active)	FlushDPB(&dec->buf, -1);
            timing_update(sei, size);
            BumpPendingFrame(dec, nFlags);
        }
        dec->stop=0;
        //MeasureEventStop(TRACE_EVENT_2_STOP,"start nal");
        return;
    }

    size = (t_uint32)p_b->end - (t_uint32)p_b->start;


    status = GetNALUType(p_b,&nri,&dec->nut,dec->is_mvc_stream);

    if((status==2)&&(((nFlags&OSI_OMX_BUFFERFLAG_EOF)==0))) /* case of unsupported NAL (can be the case of SVC): skip this NAL */
    {
        InputControlAdd2List(dec,p_b,0); // (ER402496)this will avoid bitstream buffer to be released prematurely
        hamac_pipe_rewind_write(); /* do this because there is hamac_pipe_reset_frame at the beg of the fct */
        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: arm_nmf: decoder: skipping unsupported NAL\n");
        return;
    }

    if (status==1)
    {
        decoder_error(dec, nFlags);
        timing_update(sei, size);
        //MeasureEventStop(TRACE_EVENT_2_STOP,"start nal");
        return;
    }
    t_uint32	slice_use_by_hw=0;
    // current NAL is a NON IDR or CUrent NAL is an IDR
    if((dec->nut == 1) || (dec->nut == 5))
    {

        /* memorize the timestamp of the first NAL after a flush and use it as T0 for timestamps check and generation */
        if(dec_static.g_first_nal_after_flush)
        {
          dec_static.g_first_nal_after_flush = 0;
          dec_static.g_start_time = (((OMX_S64)nTimeStampH) << 32) | (((OMX_S64)nTimeStampL) & 0xFFFFFFFF);
          OstTraceFiltInst1( TRACE_FLOW , "H264DEC: Catch start time = %d\n",nTimeStampL);
#if VERBOSE_STANDARD == 1
          NMF_LOG("Catch start time = %d\n",nTimeStampL);
#endif
        }

        // process current VCL nal
        slice_use_by_hw=1;
        t_vclstatus	status=ProcessVideoNAL(dec,p_b,nri,&slice_use_by_hw);
        //NMF_LOG("Use by HW=%d\n",slice_use_by_hw);
        switch(status)
        {
            case VCL_STOP:
                //NMF_LOG("VCL STOP\n");
                timing_update(sei, size);
                BumpPendingFrame(dec, nFlags);
                InputControlAdd2List(dec,p_b,0);	//ER402496
// +ER373259
#ifndef NAL_MODE
                dec->failure = 3;
#endif
// -ER373259
                //MeasureEventStop(TRACE_EVENT_2_STOP,"start nal");
                return;

            case VCL_ERROR:
                //NMF_LOG("VCL ERROR\n");
                decoder_error(dec, nFlags);
                timing_update(sei, size);
                InputControlAdd2List(dec,p_b,0);	//ER402496
                //MeasureEventStop(TRACE_EVENT_2_STOP,"start nal");
                return;


            default:
                break;

        }
    }
    //MeasureEventStop(TRACE_EVENT_2_STOP,"start nal");
    // Does decoder reach a Frame boundarie? If yes , all information concerning the current
    // frame are collapsed and decoder is fired. The frame detection boundary is different in
    // frame mode or in NAL mode. In frame mode it will be assumed that flags EOF marks the LAST nal
    // of the current frame. In NAM mode , the decoder need to received one further nal than current
    // frame
    t_uint16 frame_boundary;
#ifdef  NAL_MODE
    // NAL mode
    // frame boundary is detected if NAL is not a VCL one or if Start of picture has been detected
    frame_boundary= (dec->sh_status==SOP) || ((dec->nut != 1) & (dec->nut != 5));
#else
    // FRAME MODE
    frame_boundary= (nFlags & OSI_OMX_BUFFERFLAG_EOF) || (nFlags & OSI_OMX_BUFFERFLAG_EOS) ;
    // FIXME not tested need fix in Source splitter
    dec->sh_status=0;
#endif
    // Decode the current frame if :
    //  1) decoder is synchronized
    //  2) There are slices in the current frame
    //  3) Decoder reaches a frame boundary or stop flag has been set
    if (( frame_boundary || (dec->stop==1)) && (dec->slice_num >= 0))
    {
        //NMF_LOG("frame boundary");
        t_uint32 slices;
        t_slices_status status=ProcessAllSlices(sei,nTimeStampH, nTimeStampL, nFlags,&slices);
        // perform decoding only if received slices are correct
        switch(status)
        {
            case SLICES_OK:
#if VERBOSE_SLICE_EVENT>0
                NMF_LOG("Fire at slice %d\n",abs_slice_count);
#endif

                FireFrameDecode(slices,p_b,sei,size);
                return;

            case NO_SLICES	:
                //NMF_LOG("no slices");
                break;

            default:
                OstTraceInt0( TRACE_ERROR , "H264DEC: arm_nmf: decoder: line675 unsupported case\n");
                NMF_PANIC("unsupported case\n");
                break;
        }
    }
    // process non VCL data
    //MeasureEventStart0(TRACE_EVENT_1_START,"ParseNonVcl");
    ParseNonVCL(p_b,dec,sei);
    dec->first_slice = (dec->nut != 1) & (dec->nut != 5);
    if (dec->stop)
    {
        if (dec->sps_active) FlushDPB(&dec->buf, -1);
    }
    timing_update(sei, size);
    BumpPendingFrame(dec, nFlags);
#ifdef NAL_MODE
    // a buffer can contains a single NAL , the VCL-nal are added to the current list
    // in Frame mode , its done when firing the decoding
    if (slice_use_by_hw) InputControlAdd2List(dec,p_b,0);	//ER402496
#endif
    //MeasureEventStop(TRACE_EVENT_1_STOP,"Parse Non VCL");
}


/**
 *
 */
t_vclstatus METH(ProcessVideoNAL)(t_dec *dec,t_bit_buffer *p_b,t_uint16 nri,t_uint32 *slice_use_by_hw)
{
    if ((dec->num_seq_par == 0) || (dec->num_pic_par == 0))
    {
    OstTraceFiltInst0( TRACE_WARNING , "H264DEC: arm_nmf: decoder: No SPS or PPS available\n");
#if VERBOSE_STANDARD == 1
        NMF_LOG("\nImpossible to read slice data. No sequence parameter or picture parameter sets available.\n");
#endif
        // Change +ER399509
        dec->stop = 1;
        // Change -ER399509
        return VCL_ERROR;
    }

    dec->next_idr = 0;

    if (dec->nut == 1)
    {
        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: arm_nmf: decoder: ----------CODED SLICE OF A NON-IDR PICTURE---------\n");
#if VERBOSE_BITSTREAM==1
        NMF_LOG("\n--------- CODED SLICE OF A NON-IDR PICTURE ---------\n");
#endif
    }
    else
    {
        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: arm_nmf: decoder: ----------CODED SLICE OF AN IDR PICTURE------------\n");
#if VERBOSE_BITSTREAM==1
        NMF_LOG("\n--------- CODED SLICE OF AN IDR PICTURE ------------\n");
#endif
    }
    dec->slice_num++;

    if (dec->slice_num == dec->maxslices)
    {
        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: arm_nmf: decoder: reallocate slice array\n");
        ReallocateSliceArray(vfm_memory_ctxt,&dec->maxslices, &dec->sh);
    }

    /* Initialize new slice */
    dec->sh[dec->slice_num].error = 0;
    dec->sh[dec->slice_num].slice_num = dec->slice_num;
    dec->sh[dec->slice_num].nut = dec->nut;
    dec->sh[dec->slice_num].nri = nri;
    dec->sh[dec->slice_num].p_bsh_start = *p_b;
    dec->sh[dec->slice_num].next = (t_uint32) p_b->end;

#if ((defined __ndk8500_a0__) || (defined __ndk5500_a0__))
    p_b->reserved = 0;  // aeb_counter to count anti-emulation bytes in slice hdr
#endif

	t_uint16 status = GetSliceHeader(p_b,&dec->buf,(void*)dec->stab,dec->ptab,dec->num_pic_par,dec->num_seq_par,
                                   dec->num_subset_seq_par,dec->first_slice,&dec->old_sl,&dec->sh[dec->slice_num]);

#if ((defined __ndk8500_a0__) || (defined __ndk5500_a0__))
    dec->sh[dec->slice_num].aeb_counter = p_b->reserved; // aeb counter
#endif

    if (status == STOP)
    {
        dec->stop = 1;
        return VCL_STOP;
    }
    else if ((status == ERR) || (status == DROP))
    {
        dec->sh[dec->slice_num].error = 1;
        dec->error_count++;
    }
    else if ((status == RS) && (dec->use_rs == 0))
    {
        /* Decode redundant slices only if whole primary piture is lost */
        if (dec->slice_num == 0 && isNewPictureRS(&dec->sh[dec->slice_num], &dec->old_sl))
        {
#if VERB_ERR_CONC == 1
            NMF_LOG("\nMissing primary picture. Decoding redundant copy.\n");
#endif
            OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: arm_nmf: decoder: Missing primary picture. Decoding redundant copy.\n");
            dec->use_rs = (dec->slice_num == 0) ? 1 : 2;
            dec->sh[dec->slice_num].p_bsh_end = *p_b;
            dec->error_count = 0;
        }
    }
    else
    {
        dec->sh[dec->slice_num].p_bsh_end = *p_b;
        dec->error_count = 0;
    }

    if ((status != DROP) && ((status != RS) || (dec->use_rs > 0)))
    {
        if (!dec->first_slice)
        {
            dec->sh_status = isNewPicture(&dec->sh[dec->slice_num], &dec->old_sl, dec->use_rs);
        }

        if ((dec->use_rs == 1) && (status == RS) && (dec->sh_status == SOP))
        {
            dec->use_rs = 0;
            dec->slice_num--;
            *slice_use_by_hw=0;
            dec->sh_status = 0;
        }
        else
            saveOldPar(&dec->sh[dec->slice_num], &dec->old_sl);
    }
    else
    {
        dec->slice_num--;
        *slice_use_by_hw=0;
        dec->sh_status = 0;
        if (status == RS)
        {
            OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: arm_nmf: decoder: Dropped redundant slice\n");
#if VERBOSE_BITSTREAM > 0
            NMF_LOG("\nDropped redundant slice\n");
#endif
        }
        if (status != RS)
        {
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: arm_nmf: decoder: Dropped slice %d\n",dec->slice_num+1);
#if VERB_ERR_CONC == 1
            NMF_LOG("\nDropped slice %i\n", dec->slice_num+1);
#endif
        }
    }

    return VCL_OK;
}


void METH(SelectActiveSPS)(void)
{
    t_dec *dec=&dec_static;
    t_sint32	index_sps;

	// Search for active SPS
	dec->p_active_sp = dec->stab;
	index_sps = 0;
	while ((dec->p_active_sp->seq_par_set_id != dec->p_active_pp->seq_par_set_id) && (index_sps++< dec->num_seq_par))
	{
		dec->p_active_sp++;
	}

        dec->host.sp = dec->p_active_sp;

        /* special case of two SPSs with the same ID, new one has been saved in a temp location */
        if((dec->pending_sps_update) && (dec->tmp_sps.seq_par_set_id == dec->p_active_sp->seq_par_set_id))
        {
            memcpy((void*)dec->p_active_sp,(void*)&(dec->tmp_sps),sizeof(t_seq_par));
            dec->pending_sps_update = 0;
        }

        /* FIXME: this part of code seems to be executed only once at the beginning,
           not at each SPS activation */
        if(dec->sps_active==0)
        {
            if (dec->sps_active==0) dec->sps_active=1;
// +CR324558 CHANGE START FOR
            dec->buf.DPBsize = getDpbSize(dec->p_active_sp);
// -CR324558 CHANGE END OF
           /* PicSizeInMbs is used only for scene change detection for ERC */
            dec->buf.PicSizeInMbs = (dec->p_active_sp->pic_width_in_mbs_minus1+1) *
                (dec->p_active_sp->pic_height_in_map_units_minus1+1) *
                (2-dec->p_active_sp->frame_mbs_only_flag);

            InitBuffer(&dec->buf);
        }
}

void  METH(NoSliceAvailable)(void)
{
    t_dec *dec=&dec_static;
    // If Start of Picture one slice more has been processed
    if (dec->sh_status == SOP)
    {
        // first slice of next image is  available
        // so this slice is kept and move in position 0
        dec->old_sh = dec->sh[dec->slice_num-1];
        dec->sh[0] = dec->sh[dec->slice_num];
        dec->sh[0].slice_num = 0;
        dec->slice_num = 0;
        dec->use_rs = (dec->use_rs == 2) ? 1 : 0;
    }
    else
    {
        // first slice of next image is not available
        // so slice_num set to -1 to said no slice available

        dec->old_sh = dec->sh[dec->slice_num];
        dec->slice_num = -1;
        dec->use_rs = 0;
    }

    dec->prevSh = &dec->old_sh;
}

// Current frame is removed because redundant in DPB , so all slices belonging to
// this frame are removed from the slice-array
// If slices belonging to next frame are availables they are keept
void	 METH(DiscardSliceArray)(t_sint32 slices)
{
    t_sint32	i,j;
    // slice processed matches the number of slice in the frame
    t_dec *dec=&dec_static;
    if (slices == (dec->slice_num+1))
    {
        dec->old_sh = dec->sh[dec->slice_num];
        dec->slice_num = -1;
        dec->use_rs = 0;	// no redundant slice
    }
    else
    {
        // the processed slices are move out from array (0-slice-num)
        dec->old_sh = dec->sh[slices-1];
        for (j = slices, i = 0; j < dec->slice_num+1; j++)
        {
            dec->sh[i] = dec->sh[j];
            dec->sh[i].slice_num = i;
            i++;
        }
        dec->slice_num = dec->slice_num - slices;
        dec->sh_status = (dec->slice_num > 0);
        dec->use_rs = (dec->use_rs == 2) ? 1 : 0;
    }

    dec->prevSh = &dec->old_sh;
}




t_slices_status	METH(ProcessAllSlices)(t_SEI *sei,t_uint32 nTimeStampH,t_uint32 nTimeStampL,t_uint32 nFlags,t_uint32 *p_slices)
{
    t_dec *dec=&dec_static;
    t_uint16 slices;
    t_uint16 first_image_in_seek_mode = 0;

    dec_static.is_first_sei_chunk=1;

    // Ready to decode a frame but its the first picture and and not an idr
    //NMF_LOG(" enter frame decode at NAL %d\n",nalct-1);
    //MeasureEventStart0(TRACE_EVENT_PROCESSALLSLICES_START,"process all slices");
    if (!dec->sps_active && (dec->sh[0].nut != 5))
    {
        first_image_in_seek_mode = 1; /* this is the first picture in the seq and it's not an IDR => enter seek mode */
        dec->buf.g_seek_mode = 1;
    }

    /* Slice header error concealment */
    slices = conceal_sh(dec->ptab, dec->stab, &dec->buf, dec->prevSh, &dec->old_sl, dec->sh_status==SOP, dec->slice_num, dec->sh);

#if CONC_FRAME_NUM == 0
    if (slices == 0)
    {
        // If SOP one slice more has been processed
        NoSliceAvailable();
        return NO_SLICES;
    }
#endif
    /* Activate picture parameter set. It must be the same for all the slices of a picture. */
    dec->p_active_pp = dec->ptab;
    while (dec->p_active_pp->pic_par_set_id != dec->sh[0].pic_par_set_id)
        dec->p_active_pp++;

    dec->host.pp = dec->p_active_pp;

    // Its an IDR frame
    if (dec->sh[0].nut == 5)
    {
        dec->buf.g_seek_mode = 0; /* An IDR is found, quit seek mode */

        /* Activate sequence parameter set. It must be the same for all the video sequence. */
        if (dec->sps_active)
        {
            dec->old_sp = *dec->p_active_sp;
        }

        // Select the active Sequence Parameter Set
        SelectActiveSPS();

        // in case a parameter change has occurred, we put the decoder in a special state
        // in which we stop requesting input NALs, and we send a void frame in order to
        // send then to dddep an invalid picture saying "something has changed"
        if(picture_parameter_change(0, dec->p_active_sp))
        {
            set_decoder_state(DECODER_STATE_PICCHANGE);
            aux_hamac_pipe_fifo_reset_frame();
            aux_hamac_pipe_fifo_push(VOID_FRAME_PIC_PARAM_CHANGE,dec_static.aux_counter);
#ifdef __ndk5500_a0__
            iStartCodec.voidFrame((t_uint32)this,  (t_uint32)SYNC_HEC_HVC);
#else
            iStartCodec.voidFrameHed();
#endif
        }

        dec->buf.PreviousFrameNum = -1;

        /* DPB specific IDR management */
        NewIDR(dec->p_active_sp, &dec->sh[0], &dec->buf, nTimeStampH, nTimeStampL, nFlags,sei);

        // reset error map
        if (mErrorMap && mErrorMap->tryLockMutex() == OMX_ErrorNone) { // Mutex locked
            mErrorMap->reset_with_size_but_notevent(
                    (((dec->p_active_sp->pic_width_in_mbs_minus1+1)*
                      (dec->p_active_sp->pic_height_in_map_units_minus1+1)) / 8)+1);
            mErrorMap->releaseMutex();
        }

        OstTraceFiltInst2( TRACE_FLOW , "H264DEC: arm_nmf: decoder: Decoding IDR frame %d POC %d\n",dec->sh[0].frame_num, (t_uint32)(dec->buf.curr_info->poc&0xFFFFFFFFul));
#if VERBOSE_STANDARD == 1
        NMF_LOG("\nDecoding IDR frame %i\tPOC %li\n", dec->sh[0].frame_num, dec->buf.curr_info->poc);
#endif
    }
    else
    {
        /* this is the only thing to be done to support decoding from an arbitrary frame.
           We insert a grey frame in the first position of the DPB and we assing it
           the value (frame_num-1) and mark it as short ref. This will automatically
           be taken into account when generating lists 0/1, because they'll appear to
           be incomplete (1 element only, the dummy ref frame that has been marked short
           ref) so the only element will be replicated to all the list. Then the decoded
           pictures will progressively fill the DPB (and the lists) and dummy frame will
           automatically be ejected from DPB thanks to sliding window mechanism. In this
           ref code implementation, I have used the buffer buf.frames[0] to fill it with
           grey YUV data; the dummy buffer is NOT to be displayed ! (this is guaranteed
           by the fact that the buffer is marked as DECODEONLY).
           Note that the dummy ref frame must be part of the DPB (ie we cannot reference
           a special descriptor outside DPB) because the firmware will need a valid index
           into DPB for CUP contexts etc. */
        if (first_image_in_seek_mode==1)
        {
            OstTraceFiltInst0( TRACE_FLOW , "H264DEC: arm_nmf: decoder: Entering Seek mode \n");
#if VERBOSE_STANDARD == 1
            NMF_LOG("Entering Seek mode ....\n");
#endif
            /* activate the SPS as if we were decoding an IDR */
            if (!dec->sps_active)
                dec->buf.PreviousFrameNum = dec->sh[0].frame_num;
            else
                dec->old_sp = *dec->p_active_sp;

            SelectActiveSPS();
            if(picture_parameter_change(0, dec->p_active_sp))
            {
                set_decoder_state(DECODER_STATE_PICCHANGE);
                aux_hamac_pipe_fifo_reset_frame();
                aux_hamac_pipe_fifo_push(VOID_FRAME_PIC_PARAM_CHANGE,dec_static.aux_counter);
#ifdef __ndk5500_a0__
                iStartCodec.voidFrame((t_uint32)this,  (t_uint32)SYNC_HEC_HVC);
#else
                iStartCodec.voidFrameHed();
#endif
            }

            InsertDummyRefIntoDPB(&dec->buf,dec->sh[0].frame_num-1,dec->p_active_sp);
            first_image_in_seek_mode = 0;
        }

        /* DPB management for a new non IDR picture */
        t_uint32 status;
        status = NewFrame(dec->p_active_sp, &dec->sh[0], &dec->buf, nTimeStampH, nTimeStampL, nFlags,sei);

        if ((status == 1) || (status == 2))
        {
            OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: arm_nmf: decoder: DPB NewFrame: frame number %i already exists\n", dec->sh[0].frame_num);
#if VERB_ERR_CONC == 1
            NMF_LOG("\nDPB NewFrame: frame number %i already exists or unable to conc missing frs. Dropping frame.\n", dec->sh[0].frame_num);
#endif
            DiscardSliceArray(slices);
            //MeasureEventStop(TRACE_EVENT_PROCESSALLSLICES_STOP,"process all slices");
            return NO_SLICES;
        }

        OstTraceFiltInst2( TRACE_FLOW , "H264DEC: arm_nmf: decoder: Decoding frame slice_num %d POC %d\n", dec->slice_num, (t_uint32)(dec->buf.curr_info->poc&0xFFFFFFFFul));
#if VERBOSE_STANDARD == 1
        NMF_LOG("\nDecoding frame slice_num %i\tPOC %li\n", dec->slice_num, dec->buf.curr_info->poc);
#endif
    }
    *p_slices=slices;
    //MeasureEventStop(TRACE_EVENT_PROCESSALLSLICES_STOP,"process all slices");
    return SLICES_OK ;

}
void display_hw_list(char *s,t_bit_buffer *pin);

void METH(FireFrameDecode)(t_sint32 slices,t_bit_buffer *p_b,t_SEI *sei,t_uint32 size)
{
    t_dec		*dec=&dec_static;
    t_sint16 slice_num = dec->slice_num;	//ER402496
    //MeasureEventStart0(TRACE_EVENT_3_START,"fire");
    // Get the pointer on a Param_in structure , from the fifo use in front of HW decoder
    t_hamac_param *param_in;

  /* can happen if this is first pic after a PAUSE */
  if((dec->esram_allocated == 0) && (dec->p_active_sp!=0))
  {
  	t_uint32	mbx	=	(dec->p_active_sp->pic_width_in_mbs_minus1+1);
	  t_uint32	mby	=	(dec->p_active_sp->pic_height_in_map_units_minus1+1) * (2-dec->p_active_sp->frame_mbs_only_flag);

        t_uint16 error = AllocateESRAM(vfm_memory_ctxt,mbx,mby,&dec->buf);
        if(error) {
          dec->stop=1;
          dec->failure=2;
          return;
        }
    }

    param_in = hamac_pipe_get_write_param_in();

    dec->buf.b_info = hamac_pipe_get_write_b_info();
    dec->buf.mb_slice_map = hamac_pipe_get_write_mb_slice_map();
    dec->buf.bits_intermediate_buffer = hamac_pipe_get_write_bits_intermediate_buffer();

    if((get_decoder_state()!=DECODER_STATE_PICCHANGE)&&(dec->p_active_sp!=0))
    {
        resetBlockInfo(dec->p_active_sp, &dec->buf);
        ComputeParamIn(dec,slices);
    }

        // Performe decoding of all slices
        if (hamac_pipe_get_write_slice_linked_list_size() < slices) {
            t_uint32	allocated_slices=slices+10;

      //      NMF_LOG("Realloc Slice list and parameter because there are in same chunk (was too small) %d\n",slices);

            HamacDeallocParameter(vfm_memory_ctxt,param_in, allocated_slices);
            t_uint16 error = HamacAllocParameter(vfm_memory_ctxt,param_in, allocated_slices);
            if(error) {
              dec->stop=1;
              dec->failure=2;
              return;
            }
            hamac_pipe_set_write_slice_linked_list_size(allocated_slices);
        }
        if(get_decoder_state()!=DECODER_STATE_PICCHANGE)
        HamacFillParamIn(vfm_memory_ctxt,dec,slices,param_in);
#ifdef __ndk5500_a0__
        HamacFillTaskDescriptor(vfm_memory_ctxt, dec, param_in);
#endif
        // At the end of decoding this buffer will be free if no more use by NON-VCL processing

        hamac_pipe_set_slices(slices);

        // Decoding original place

        dec->buf.PreviousFrameNum = dec->sh[0].frame_num;

        MarkPicture(dec->p_active_sp, &dec->sh[0], &dec->buf);

#if ENABLE_TIMING == 1
       // if (DecodeTime(sei))
       // {
         //   dec->buf.curr_info->display_time = sei->timings.t_o_dpb;
       // }
#endif
        dec->nframes++;
        //if((dec->nframes % 100) ==0)NMF_LOG("FRAME=%d\n",dec->nframes);
        //if(dec->nframes <30) NMF_LOG("a FRAME=%d\n",dec->nframes);

#if VERBOSE_STANDARD == 1
        if(dec->nframes <10) NMF_LOG("a FRAME=%d\n",dec->nframes);
        if((dec->nframes % 100) ==0)NMF_LOG("FRAME=%d\n",dec->nframes);
#endif

#ifdef	_CACHE_OPT_
        // Force cache clean to be sure that bitstream has been written in SDRAM
        t_uint8 *end_addr,*start_addr;

        if(p_b!=NULL)
        {
            start_addr= (t_uint8*) p_b->start;
            end_addr=(t_uint8*) dec->sh[slices-1].next + 4;

            VFM_CacheClean(vfm_memory_ctxt, start_addr,end_addr-start_addr+1);
        }
#endif

        //MeasureEventStart0(TRACE_EVENT_CACHECLEAN_START,"end fired");
        // No more need to keep slice array
        DiscardSliceArray(slices);


        if(p_b!=NULL)
        {
            ParseNonVCL(p_b,dec,sei);
        }
        dec->first_slice = (dec->nut != 1) & (dec->nut != 5);


        // End of Decoding
        if (dec->stop)
        {
            if (dec->sps_active)
            {
                FlushDPB(&dec->buf, -1);
            }
        }
        timing_update(sei, size);

        // managing buffer list
//+ER402496
#ifndef NAL_MODE
        t_uint8 force = 0;
        if(slice_num !=-1)
            force=1;
        else
            force=0;
        // in Frame mode , current buffer must be added now to the list
        InputControlAdd2List(dec,p_b,force);
#endif
//-ER402496

#if VERBOSE_INPUT_CONTROL >= 1
        display_hw_list("HWstart",dec->input_control.bit_buffer_list);
#endif
        hamac_pipe_set_bit_buffer_addr(dec->input_control.bit_buffer_list);

        InputControlInitList(dec,p_b) ;

        if(get_decoder_state()==DECODER_STATE_PICCHANGE)
        {
            hamac_pipe_rewind_write();
            return;
        }

        // Post decoding request
        static int ct=0;
        OstTraceFiltInst1( TRACE_DEBUG , "H264DEC: arm_nmf: decoder: Fire frame on hamac %d\n",ct);
        ct++;
#if VERBOSE_STANDARD == 1
        NMF_LOG("\t\tFire frame on hamac %d\n",ct);
#endif
#if  CHECK_SYNCHRONE_HW
        if (hw_is_busy) NMF_PANIC("HW is busy here ! \n");
        hw_is_busy=1;
#endif

        if (dec->sva_bypass == 1) {
            OstTraceFiltInst0( TRACE_FLOW , "H264DEC: arm_nmf: decoder: Bypassing DSP\n");
#if VERBOSE_STANDARD == 1
            NMF_LOG("Bypassing dsp\n");
#endif
            hamac_pipe_set_void_frame_info(VOID_FRAME_ALGO_BYPASS);
            //MeasureEventStart0(TRACE_EVENT_HW_START,"hardware start");
#ifdef __ndk5500_a0__
            endCodec(HVA_ERROR_NONE, (t_uint32)ERT_NONE, 0);
#else
            endCodec(STATUS_JOB_COMPLETE, (t_uint32)VDC_ERT_NONE, 0);
#endif
        }
        else if (dec->sva_bypass == 2) {
            OstTraceFiltInst0( TRACE_FLOW , "H264DEC: arm_nmf: decoder: Bypassing DSP but with ddep\n");
#if VERBOSE_STANDARD == 1
            NMF_LOG("Bypassing codec on dsp (still goint through ddep)\n");
#endif
            //hamac_pipe_set_void_frame_info(VOID_FRAME_ALGO_BYPASS);
            aux_hamac_pipe_fifo_reset_frame();
            aux_hamac_pipe_fifo_push(VOID_FRAME_ALGO_BYPASS,dec_static.aux_counter);
            //MeasureEventStart0(TRACE_EVENT_HW_START,"hardware start");
#ifdef __ndk5500_a0__
            iStartCodec.voidFrame((t_uint32)this,  (t_uint32)SYNC_HEC_HVC);
#else
            iStartCodec.voidFrameHed();
#endif
        }
        else {
		//OstTraceEventStart0(TRACE_EVENT_HAMACPHYSICALL_START,"hamac in");
		PrepareHedParameters(vfm_memory_ctxt,param_in, &(param_in->hed_params));
/* + Changes for CR 350542 */
		//for output frame
		OMX_U32 tempAddr;
		OMX_U8 *tempPtr;

		tempAddr = Endianess(param_in->addr_in_parameters->dpb_addresses[param_in->addr_in_parameters->curr_pic_idx]);
		tempPtr = VFM_GetLogical(vfm_memory_ctxt, (t_uint8 *)tempAddr);
		OMX_U32 picture_height = (dec->p_active_sp->pic_height_in_map_units_minus1+1) * 16;
		OMX_U32 picture_width = (dec->p_active_sp->pic_width_in_mbs_minus1+1)*16;
		VFM_CacheInvalidate(vfm_memory_ctxt, tempPtr,(picture_height*picture_width*1.5));
/* - Changes for CR 350542 */
		param_in->hed_params.HED_used = dec->buf.HED_used;
		HamacToPhysicalAddresses(vfm_memory_ctxt,param_in, slices);
		// Force Flush of all Param in
#ifdef _CACHE_OPT_
            VFM_CacheClean(vfm_memory_ctxt, param_in->heap_addr ,param_in->heap_size);
#endif


            //OstTraceEventStop(TRACE_EVENT_HAMACPHYSICALL_STOP,"hamac in");
            //MeasureEventStart0(TRACE_EVENT_HW_START,"hardware start");

            // if this is the first frame after sleep, wakeup
            if(dec->gone_to_sleep)
            {
                dec->gone_to_sleep = 0;
                iStartCodec.preventSleep();
            }
#ifdef __ndk5500_a0__

            /*physical @*/
            tps_t1xhv_vdc_h264_param_in task_desc = param_in->addr_in_parameters;
            t_uint32 hvaProfile = (t_uint32)H264_BASELINE_DEC;
            if (dec->buf.HED_used == 3) {
            hvaProfile = H264_HIGHPROFILE_DEC;
            }
            /* TODO useless ? -> endCodec callback @ instead this pointer */
            iStartCodec.startTask( (t_uint32)task_desc, (t_uint32)this, hvaProfile );
#else
		iStartCodec.startCodecHed(
			(t_uint32)param_in->addr_in_frame_buffer ,
			(t_uint32)param_in->addr_out_frame_buffer,
			(t_uint32)param_in->addr_internal_buffer,
			(t_uint32)param_in->addr_in_bitstream_buffer,
			(t_uint32)param_in->addr_out_bitstream_buffer,
			(t_uint32)param_in->addr_in_parameters,
			(t_uint32)param_in->addr_out_parameters,
			(t_uint32)param_in->addr_in_frame_parameters,
			(t_uint32)param_in->addr_out_frame_parameters,
      param_in->hed_params.HED_used,
      param_in->hed_params.bitstream_start,
      param_in->hed_params.bitstream_end,
      param_in->hed_params.ib_start,
      param_in->hed_params.hed_cfg,
      param_in->hed_params.hed_picwidth,
      param_in->hed_params.hed_codelength);
#endif
        }
        //MeasureEventStop(TRACE_EVENT_CACHECLEAN_START,"end fired");
        //MeasureEventStop(TRACE_EVENT_3_STOP,"fire");
}


void METH(InputControlInitList)(t_dec *dec, t_bit_buffer *p_b)
{

    // reinit buffer list with current buffer if VCL buffer , normaly comes from a new frame
    dec->input_control.bit_buffer_list=0;

#ifdef NAL_MODE
    // one more NAL (or buffer) has been sent to find frame boundary
    if ((dec->nut==1) || (dec->nut==5))
    {
        if (p_b)
        {
            p_b->inuse|=VCL_UNDER_PROCESSING;
            p_b->next=0;
            dec->input_control.bit_buffer_list=p_b;
        }
    }
#endif
}



/*****************************************
 * End of MPC decoding callback
 *****************************************/
void METH(endCodec)(t_uint32 status, t_uint32 info, t_uint32  duration)
{
#ifdef __ndk5500_a0__
    if ( HVA_ERROR_NONE != (t_hva_status_error)status ) {
            } else {
        switch ((t_hva_h264dec_status)info) {
            case TASK_OK:
                // NMF_LOG("endCodec HVA info -> TASK_OK \n");
                break;
            case PICTURE_LOSS:
                // NMF_LOG("endCodec HVA info -> PICTURE_LOSS status ->%d \n", status);
                break;
            case HEC_IB_OVERFLOW_ERROR:
                //NMF_LOG("endCodec HVA info -> HEC_IB_OVERFLOW_ERROR status ->%d \n", status);
                break;

            case HEC_BITSTREAM_UNDERFLOW_ERROR:
                //NMF_LOG("endCodec HVA info -> HEC_BITSTREAM_UNDERFLOW_ERROR status ->%d \n", status);
                break;
            case HEC_READ_PLUG_ERROR:
                //NMF_LOG("endCodec HVA info -> HEC_READ_PLUG_ERROR status ->%d \n", status);
                break;
            case HEC_WRITE_PLUG_ERROR:
                //NMF_LOG("endCodec HVA info -> HEC_WRITE_PLUG_ERROR status ->%d \n", status);
                break;
            case VOID_FRAME:
                //NMF_LOG("endCodec HVA info -> VOID_FRAME status ->%d \n", status);
                break;
            case ERT_NONE:
                //NMF_LOG("endCodec HVA info -> ERT_NONE status ->%d \n", status);
                break;
        }
    }
#else

#if 0  // now the error recovery is handled through NMF Panic from DSP up to OMX proxy
  /* error recovery mechanism ! */
  if(status == STATUS_JOB_ABORTED)
  {
    if(duration == 0xbeef)
        {
            if(info==VDC_ERT)
            {
                OstTraceFiltInst0( TRACE_WARNING , "H264DEC: arm_nmf: decoder: Decoder MPC component timeout\n");
                NMF_LOG("ERROR ! Decoder MPC component timeout\n");
                iStartCodec.startCodecHed(0,0,0,0,0,0,0,0,0,0xbeef,0,0,0,0,0,0); // ask for trace
                return;
            }
            else
            {
                OstTraceFiltInst0( TRACE_WARNING , "H264DEC: arm_nmf: decoder: CABAC MPC component timeout\n");
                NMF_LOG("ERROR ! CABAC MPC component timeout\n");
                iStartCodec.startCodecHed(0,0,0,0,0,0,0,0,0,0xbeef,0,0,0,0,0,0); // ask for trace
                return;
            }
        }
        else
        {
            if(duration!=0xbeed)
            {
                NMF_LOG("TRACE %x\n",duration);
                iStartCodec.startCodecHed(0,0,0,0,0,0,0,0,0,0xbeef,0,0,0,0,0,0); // ask for trace
            }
            else
            {
                t_uint16 i;
                NMF_LOG("--------ENDOFTRACE----------\n");
                for(i=0;i<128;i++)
                {
                    NMF_LOG(".\n");
                }
            }
            return;
        }
    }
#endif
#endif


#if  CHECK_SYNCHRONE_HW
    hw_is_busy=0;
#endif
    //NMF_LOG("Duration is %d\n",duration);
    //time expressed in us , so convert it from 90Khz timer
    MeasureDelta(TRACE_DELTA_SVA,"hardware decoder",(duration*100)/9);
    //MeasureEventStop(TRACE_EVENT_HW_STOP,"dec stop");

#ifdef __ndk5500_a0__
    if((status==HVA_ERROR_NONE) && (VOID_FRAME==(t_hva_h264dec_status)info) )
#else
        if((status==STATUS_JOB_COMPLETE) && (VDC_VOID==(t_t1xhv_decoder_info)info) )
#endif
        {
        VoidEndOfFrame();
        } else {
        DecodeEndOfFrame(); }
}



/**********************************************************************
 * This function handles end of decoding callback for non-void frames
 **********************************************************************/
void	METH(DecodeEndOfFrame)(void)
{
    t_hamac_param *hamac_param;
    t_dec	* dec=&dec_static;
    t_uint32	eos;

    OstTraceFiltInst0( TRACE_FLOW , "H264DEC: arm_nmf: decoder: End of decoding frame\n");

#if VERBOSE_STANDARD == 1
    NMF_LOG("\nEnd of Decoding frame \n");
#endif

    hamac_pipe_next_read_pos();
    hamac_param = hamac_pipe_get_read_param_in();

/* + Changes for CR 350542 */
    HamacToLogicalAddresses(vfm_memory_ctxt, hamac_param, hamac_pipe_get_slices());
		//for output frame
		OMX_U32 tempAddr;
		OMX_U8 *tempPtr;

		tempAddr = Endianess(hamac_param->addr_in_parameters->dpb_addresses[hamac_param->addr_in_parameters->curr_pic_idx]);
		tempPtr = VFM_GetLogical(vfm_memory_ctxt, (t_uint8 *)tempAddr);
		OMX_U32 picture_height = (dec->p_active_sp->pic_height_in_map_units_minus1+1) * 16;
		OMX_U32 picture_width = (dec->p_active_sp->pic_width_in_mbs_minus1+1)*16;
		VFM_CacheInvalidate(vfm_memory_ctxt, tempPtr,(picture_height*picture_width*1.5));
/* - Changes for CR 350542 */
    if(dec_static.sva_bypass==0)
    {
		/* + Changes for CR 350542 */
        //HamacToLogicalAddresses(vfm_memory_ctxt, hamac_param, hamac_pipe_get_slices());
        /* - Changes for CR 350542 */
        FillErrorMap(hamac_param); // uses p_active_sp !
#if VERBOSE_IB == 1
        PrintIntBuf(dec);
#endif
    }

    /* + Changes for CR 399075 */
    if(dec->bImmediateRelease)
    {
        BumpFrame(&dec->buf);
        dec->bImmediateRelease = 0;
    }
    /* - Changes for CR 399075 */

    // After call back from algo part, bumping frames
    eos=hamac_pipe_bump_frames(dec);

#if VERBOSE_XON > 0
    NMF_LOG("test if require XON NAL DecodeEndOfFrame\n");
#endif

    // if not the last frames
    if (eos==0)
    {
#if VERBOSE_XON > 0
        NMF_LOG("No EOS so call XON/XOFF mechanism\n");
#endif
        t_bit_buffer *p_b= hamac_pipe_get_bit_buffer_addr();
        hamac_pipe_next_read(); // free position in hamac_pipe
        input_control_acknowledge_pending(p_b);
    }
    else
    {
        hamac_pipe_next_read(); // free position in hamac_pipe
    }
}/* End of DecodeEndOfFrame*/


/***************************************************************************
 * Handler for end of void frames decoding callback
 ***************************************************************************/
void	METH(VoidEndOfFrame)(void)
{
    t_dec	* dec=&dec_static;

    OstTraceFiltInst0( TRACE_FLOW , "H264DEC: arm_nmf: decoder: End of void frame\n");

#if VERBOSE_STANDARD == 1
    NMF_LOG("\nEnd of VOID frame \n");
#endif

    t_void_frame_info void_frame_info = (aux_hamac_pipe_fifo_get_current_el())->void_frame_info;//hamac_pipe_get_void_frame_info();
    aux_hamac_pipe_bump_frames(dec);   // first bump regular frames

    switch(void_frame_info)
    {
        case VOID_FRAME_ALGO_BYPASS:
            break;
        case VOID_FRAME_OUTPUT_FLUSH:
            if (get_decoder_state()==DECODER_STATE_PICCHANGE)
            {
                input_control_acknowledge_pending(dec->buf.keep_pb);
                DPB_Revert(&dec->buf);
                hamac_pipe_void_frame(dec, void_frame_info);   // this will flush remaining bufs and send ack to ddep
            }
            else
            {
                dec_static.g_first_nal_after_flush = 1;
                dec->buf.g_decoded_frm_ctr = 0; // for timestamps generation
                dec_static.g_wrong_timestamps = 0;
                hamac_pipe_void_frame(dec, void_frame_info);   // this will flush remaining bufs and send ack to ddep
#if ((defined __ndk8500_a0__) || (defined __ndk5500_a0__))
                InitCupContextFifo(&dec->buf);
#endif
            }
            break;
        case VOID_FRAME_PIC_PARAM_CHANGE:
            if (get_decoder_state()==DECODER_STATE_PICCHANGE)
            {
                t_frameinfo frameinfo;
                set_frameinfo(&frameinfo, dec->p_active_sp, &dec->sh[0], 0, 0, 0);
                display_queue_output_picture((t_frames*)0, INVALID_PIC, &frameinfo);
                OstTraceFiltInst0( TRACE_FLOW , "H264DEC: arm_nmf: decoder: Sending invalid pic to ddep to signal a change\n");
#if VERBOSE_STANDARD == 1
                NMF_LOG("Sending invalid pic to ddep to signal a change\n");
#endif
            }
            else
            {
                OstTraceInt0( TRACE_ERROR , "H264DEC: arm_nmf: decoder: PIC param change void frame received but not in PICCHANGE state\n");
                NMF_PANIC("PIC param change void frame received but not in PICCHANGE state\n");
            }
            break;
        case VOID_FRAME_PAUSE_INFO:
            hamac_pipe_void_frame(dec, void_frame_info);   // send ack to ddep
            if(dec->gone_to_sleep == 0)
            {
                dec->gone_to_sleep = 1;
                iStartCodec.allowSleep();
                if(dec->esram_allocated == 1)
                    FreeESRAM(vfm_memory_ctxt,&dec->buf);
            }
            break;
        default:
            hamac_pipe_void_frame(dec, void_frame_info);   // common handler
            break;
    }

    aux_hamac_pipe_fifo_pop();
}/* End of VoidEndOfFrame*/

// +ER402496
//this NAL is considered as the first one of next frame to process
//SO the list of NAL buffer is reinitialized  ,this list is
//used by proxy to free all the buffer
void METH(InputControlAdd2List)(t_dec *dec, t_bit_buffer *p_b,t_uint8 force_add)
{
    // if NAL is a VCL it will be freed by the end of HW decoder
    // so the current NAL is added to the bit_buffer_list under construction
    if  (force_add || ((dec->nut==1) || (dec->nut==5) || (dec->nut==14) || (dec->nut==20)|| (dec->nut==12)))
    {
        if (p_b)
        {
            p_b->inuse|=VCL_UNDER_PROCESSING;
            //NMF_LOG("Mark %p USE BY HW\n",p_b);
            // add this buffer to the list if not already in
            if (p_b!=dec->input_control.bit_buffer_list)
            {
                p_b->next=dec->input_control.bit_buffer_list;
                dec->input_control.bit_buffer_list=p_b;
                // check_list("vcl nal",dec->input_control.bit_buffer_list);
            }
        }
    }
}
// -ER402496


// Post frame to display to HW pipe
// if there is no frame to display , create an empty one
//
void METH(BumpPendingFrame)(t_dec *dec, t_uint32 nFlags)
{
    // Flush or EOS : end of stream , we forced the flush
    // It is the Last NAL buffer
    if ((hamac_pipe_bumped_frames_number()>0) || (nFlags & OSI_OMX_BUFFERFLAG_EOS))
    {
        // No input buffer to free at the end of this stage
        // Buffer will be free by normal loop on NAL
        hamac_pipe_set_bit_buffer_addr(0);

        if (nFlags & OSI_OMX_BUFFERFLAG_EOS) { // EOS
            OstTraceFiltInst0( TRACE_FLOW , "H264DEC: arm_nmf: decoder: BumpPendingFrame : EOS frame\n");
#if VERBOSE_STANDARD == 1
            NMF_LOG("BumpPendingFrame : EOS frame\n");
#endif
            if (hamac_pipe_mark_eos_frame()) {
                //hamac_pipe_set_void_frame_info(VOID_FRAME_EOS_WITH_FLUSH);
                aux_hamac_pipe_fifo_copy_bumped_frames(0);   // frames will be bumped through void frame callback
                aux_hamac_pipe_fifo_push(VOID_FRAME_EOS_WITH_FLUSH,dec_static.aux_counter);
            } else { // no frames to bump
                // MC: this case must never happen (not handled in ddep)
                //hamac_pipe_set_void_frame_info(VOID_FRAME_EOS_WO_FLUSH);
                aux_hamac_pipe_fifo_reset_frame();
                aux_hamac_pipe_fifo_push(VOID_FRAME_EOS_WO_FLUSH,dec_static.aux_counter);
            }
        }
        else /* case of hamac_pipe_bumped_frames_number()>0 but not EOS */
        {
            OstTraceFiltInst0( TRACE_WARNING , "H264DEC: arm_nmf: decoder: this case is not handled by DDEP\n");
            aux_hamac_pipe_fifo_copy_bumped_frames(0);   // frames will be bumped through void frame callback
            aux_hamac_pipe_fifo_push(VOID_FRAME_EOS_WITH_FLUSH,dec_static.aux_counter);
        }

        if (dec_static.sva_bypass == 1) {
#ifdef __ndk5500_a0__
            endCodec(HVA_ERROR_NONE, (t_uint32)VOID_FRAME, 0);
#else
            endCodec(STATUS_JOB_COMPLETE, VDC_VOID, 0);
#endif
        }
        else {
#ifdef __ndk5500_a0__
            iStartCodec.voidFrame((t_uint32)this,  (t_uint32)SYNC_HEC_HVC);
#else
            iStartCodec.voidFrameHed();
#endif
        }
    }

    hamac_pipe_rewind_write();  // rewind hamac pipe in all cases (void frame now goes in aux pipe)

}




t_uint32	METH(get_frame_size_from_active_sps)(void)
{
    t_dec 	*dec=	&dec_static;
    if (dec->sps_active)
    {
        t_uint32 size32= (dec->p_active_sp->pic_width_in_mbs_minus1+1)
            * (dec->p_active_sp->pic_height_in_map_units_minus1+1)
            * (2- dec->p_active_sp->frame_mbs_only_flag);
        size32<<=8; // from Macroblock to pixel
        size32+=size32>>1; // including all plan YUV
        return size32;
    }
    else
    {
        // no active sps so we assume size is null to avoid to
        // trig an error
        return 0;
    }
}


void	METH(output_buffer)(void *ptr, t_uint32 size16)
{
    t_dec *dec=&dec_static;


    t_uint32	current_buffer_size=get_frame_size_from_active_sps();

    if ((size16< current_buffer_size) && (size16!=0) && (get_decoder_state() != DECODER_STATE_PICCHANGE))
    {
        OstTraceInt2( TRACE_ERROR , "H264DEC: arm_nmf: decoder: Proxy provided too small DPB buffer :%d expected %d\n",size16,current_buffer_size);
        NMF_PANIC("Proxy provided too small DPB buffer :%d expected %d\n",size16,current_buffer_size);
    }


    if (get_decoder_state() == DECODER_STATE_FLUSH) {
        OstTraceInt0( TRACE_WARNING , "H264DEC: arm_nmf: decoder: Output buffer received while in flush state\n");
        NMF_PANIC("Output buffer received while in flush state\n");
    }

    // For the thumbnail generation use case, actually only one physical output buffer is needed.
    // So here we cheat by increasing by 2 the nbr of available bfrs.
    // 1 for the empty EOS buffer, 1 for the dummy grey frame for seek
    if((dec->bThumbnailGeneration)&&(ptr))
    {
#if VERBOSE_STANDARD == 1
      NMF_LOG("\nThumbnail Generation Use case\n");
#endif
      input_control->dpb_frame_available+=2;
    }

    if (ptr) display_queue_new_output_buffer(dec, ptr);

    OstTraceFiltInst1( TRACE_FLOW , "H264DEC: arm_nmf: decoder: New output buffer %d\n",(t_uint32)ptr);
#if VERBOSE_STANDARD == 1
    NMF_LOG("\nNew output buffer %p\n",ptr);
#endif
}


/* Parse non-VCL NAL */
void	METH(ParseNonVCL)(t_bit_buffer *p_b,t_dec *dec,t_SEI *sei)
{
    t_uint16	status;


    switch(dec->nut)
    {
        case 1:
        case 5:
        case 20: /* for MVC */
            /* Already parsed */
            dec->sh_status = 0;
            break;

        case 2:
        case 3:
        case 4:
            OstTraceInt0( TRACE_WARNING , "H264DEC: arm_nmf: decoder: Data partitioning not supported in baseline profile. Exiting\n");
#if VERBOSE_STANDARD == 1
            NMF_LOG("\nData partitioning not supported in baseline profile. Exiting.\n");
#endif
            dec->stop = 1;
            break;

        case 6:
            /* FIXME status is not used ?? */
            status = GetSEI(p_b, dec, sei);
            break;

        case 7:
            status = GetSequenceParSet(p_b,&dec->num_seq_par,dec->stab,&(dec->tmp_sps),&(dec->pending_sps_update));
            if (status != 0)
            {
                OstTraceInt0( TRACE_WARNING , "H264DEC: arm_nmf: decoder: Error in SPS. Exiting\n");
#if VERBOSE_STANDARD == 1
                NMF_LOG("\nError in sequence parameter set. Exiting.\n");
#endif
                dec->stop = 1;
//+Change start for CR336095,ER334368,ER336290 level 5.1. support
                if(status==2) {
					OstTraceInt0( TRACE_WARNING , "H264DEC: arm_nmf: decoder: DPB requirements not met.Exiting.\n");
                    dec->failure=2;
                }
//-Change end for CR336095,ER334368,ER336290 level 5.1. support
            }
            break;

        case 8:
            if ((dec->num_seq_par == 0) && (dec->num_subset_seq_par == 0))
            {
                OstTraceInt0( TRACE_WARNING , "H264DEC: arm_nmf: decoder: No SPS available and no subset SPS available.\n");
#if VERBOSE_STANDARD == 1
                NMF_LOG("\nNo SPS available and no subset SPS available.\n");
#endif
                dec->stop = 1;
                break;
            }

            status = GetPictureParSet(p_b,&dec->num_pic_par,dec->ptab,&dec->num_seq_par,dec->stab);

            if (status != 0)
            {
                OstTraceInt0( TRACE_WARNING , "H264DEC: arm_nmf: decoder: Error in PPS. Exiting\n");
#if VERBOSE_STANDARD == 1
                NMF_LOG("\nError in picture parameter set. Exiting.\n");
#endif
                dec->stop = 1;
            }
            break;

        case 9:
            /* Access unit delimiter. */
            break;

        case 10:
            /* End of sequence. Next access unit should be IDR. */
            dec->next_idr = 1;
            break;

        case 11:
            /* End of stream. Last NAL unit. */
            //dec->stop = 1;   // MC: commented out in order to avoid EOS with pb=0 and no frames to bump
            break;

        case 12:
            /* Filler data. */
            break;

        case 15:   /* for MVC */
            status = GetSubsetSequenceParSet(p_b,&dec->num_subset_seq_par,dec->subset_sp,&dec->is_mvc_stream);
            if (status != 0)
            {
                OstTraceInt0( TRACE_WARNING , "H264DEC: arm_nmf: decoder: Error in Subset SPS. Exiting\n");
#if VERBOSE_STANDARD == 1
                NMF_LOG("\nError in subset sequence parameter set. Exiting.\n");
#endif
                dec->stop = 1;
//+Change start for CR336095,ER334368,ER336290 level 5.1. support
                if(status==2) {
					OstTraceInt0( TRACE_WARNING , "H264DEC: arm_nmf: decoder: Subset SPS DPB requirements not met.Exiting.\n");
                    dec->failure = 2;
                }
//-Change end for CR336095,ER334368,ER336290 level 5.1. support
            }
            break;

        default:
            /* NAL Unit type 0 or 13-23 (reserved) or 24-31 (unspecified). */
            break;
    }

}

// #define VERBOSE_ERRORMAP 1
void METH(FillErrorMap)(t_hamac_param *hamac_param) {
#ifdef NO_HAMAC
    // FIXME: not implemented in the SW decoder
    return;
#else
    int i, j;
    t_ushort_value first_mb, mb_num, mb_count;
    unsigned short data;
    unsigned char *addr;
    t_dec *dec=&dec_static;
    t_uint32 mb_width  =	(dec->p_active_sp->pic_width_in_mbs_minus1+1);
//Changes Start for ER435666
    t_uint32 mb_height =	0;
	t_uint32 tempValue = 0;
	if ((!dec->p_active_sp->frame_mbs_only_flag) && (!dec->p_active_sp->mb_adaptive_frame_field_flag))
	{
			OstTraceFiltInst1( TRACE_FLOW , "H264DEC: Setting new value of pic_height_in_map_units_minus1 OLD : %d ",dec->p_active_sp->pic_height_in_map_units_minus1);
			tempValue = ((dec->p_active_sp->pic_height_in_map_units_minus1 +1) * 2 ) - 1;
	}
	else
	{
			tempValue = dec->p_active_sp->pic_height_in_map_units_minus1;
	}

	mb_height = tempValue + 1;
//Changes End for ER435666

#ifdef VERBOSE_ERRORMAP
    unsigned char DebugErrorMap[_SIZE_ERROR_MAP_];
    memset(DebugErrorMap, 0, _SIZE_ERROR_MAP_);
    //for (i=0; i<_SIZE_ERROR_MAP_; i++) DebugErrorMap[i]=0;
#else
    if (mErrorMap && mErrorMap->tryLockMutex() == OMX_ErrorNone) { // Mutex locked
#endif
        //NMF_LOG("NEW MAP ---------------------------\n");

        // 8 sets of erroneous MB
        for (i=0; i<8; i++) {
            first_mb = hamac_param->addr_out_parameters->slice_loss_first_mb[i]+1;
            mb_num = hamac_param->addr_out_parameters->slice_loss_mb_num[i];
            //NMF_LOG("MAP %d %d\n",first_mb,mb_num);
            if (mb_num<=0) {
                continue;
            }
            if (!((first_mb<=mb_width*mb_height) && ((t_uint32)first_mb+mb_num-1<=mb_width*mb_height))) {
                OstTraceInt0( TRACE_WARNING , "H264DEC: arm_nmf: decoder: Wrong case in Error Map!\n");
                NMF_PANIC("Wrong cases in %s line %d:  %d %d %d %d\n", __FILE__, __LINE__, first_mb, mb_num, mb_width, mb_height);
            }

#ifdef VERBOSE_ERRORMAP
            addr = DebugErrorMap;
#else
            addr = mErrorMap->mErrorMap;
#endif
            addr += first_mb/8;

            // computing first byte
            data=*addr;
            mb_count = MIN(8-(first_mb%8)+1, mb_num); // nb of mb to fill first byte
            if (mb_count < 8) { // if 8, we fill it in step 2
                mb_num -= mb_count;
                for (j=0; j<mb_count; j++) {
                    data |= 1<<(j+(first_mb%8-1));
                }
                *addr++ = data;
            }

            // filling 0xFF bytes
            for (j=0; j<mb_num/8; j++) {
                *addr++ = 0xFF;
            }

            // filling in last byte
            data=*addr;
            for (j=0; j<mb_num%8; j++) {
                data |= 1<<j;
            }
            *addr++ = data;

            // Compute the FastUpdate structure (1st mb - nb of mbs)
            FastUpdate_Map *pFast = &mErrorMap->mFastUpdate;
            if (pFast->mNumMBs != 0) {
                t_uint32 current_first_mb = pFast->mFirstGOB*mb_width + pFast->mFirstMB;
                t_uint32 current_last_mb = current_first_mb + pFast->mNumMBs;
                t_uint32 last_mb = first_mb + mb_num - 1;
                if (current_first_mb<first_mb) {
                    first_mb = current_first_mb;
                }
                if (current_last_mb>last_mb) {
                    last_mb = current_last_mb;
                }
                mb_num = last_mb - first_mb + 1;
            }
            pFast->mFirstGOB = first_mb / mb_width;
            pFast->mFirstMB = first_mb % mb_width;
            pFast->mNumMBs = mb_num;
        }
#ifndef VERBOSE_ERRORMAP
        mErrorMap->releaseMutex();
    }
#else
    int error=0;
    int size = ((dec_static.p_active_sp->pic_height_in_map_units_minus1+1) *
            (dec_static.p_active_sp->pic_width_in_mbs_minus1+1) + 7) /8; // ceiling
    // count number of errors
    addr = DebugErrorMap;
    for (i=0; i<size; i++) {
        if (*addr != 0) {
            for (j=0; j<8; j++) {
                if (*addr & (1<<j)) error++;
            }
        }
        addr++;
    }
    // display them
    if (error) {
        OstTraceFiltInst2( TRACE_FLOW , "H264DEC: arm_nmf: decoder: Error map (size %d bytes, %d errors)\n",size, error);
        addr = DebugErrorMap;
        for (i=0; i<size; i++) {
            OstTraceFiltInst1( TRACE_FLOW , "H264DEC: arm_nmf: decoder: Errors  %x ", *addr++);
            if ((dec_static.p_active_sp->pic_width_in_mbs_minus1+1+7)/8 == 0) OstTraceFiltInst0( TRACE_FLOW ,"\n");
        }
        OstTraceFiltInst0( TRACE_FLOW ,"\n\n");
    }
#endif
#endif  // NO_HAMAC
}

void   METH(ComputeParamIn)(t_dec *dec, t_uint16 slices)
{
    t_sint16 	i;
    t_uint16	max_first_mb=0;
    t_uint16	ASO_flag=0;
    t_uint16	FMO_flag=0;
    t_uint16  RS_flag=0;

    /* Set FMO flag */
    if (dec->p_active_pp->num_slice_groups_minus1 != 0) FMO_flag = 1;

    if (dec->p_active_pp->redundant_pic_cnt_present_flag != 0) RS_flag = 1;

    for (i = 0; i < slices; i++)
    {
        if (!dec->sh[i].error)
        {
            /* Generate list0 if decoding a P/B slice */
            if ((dec->sh[i].slice_type == 0) || (dec->sh[i].slice_type == 5)
                    ||(dec->sh[i].slice_type == 1) || (dec->sh[i].slice_type == 6))
            {
                if (GenerateList(dec->p_active_sp, dec->p_active_pp, &dec->sh[i], &dec->buf))
                {
#if VERB_ERR_CONC == 1
                    NMF_LOG("\nSlice %i not completely decoded\n", i);
#endif
                    dec->sh[i].error = 1;
                }
            }

            if (dec->sh[i].first_mb_in_slice < max_first_mb)
            {
                if(dec->p_active_sp->profile_idc == 66) /* ASO is only allowed in baseline */
                    ASO_flag = 1;
                else
                    dec->sh[i].error = 1;          /* if we are in main/high, discard the slice */
            }
            else
            {
                max_first_mb = dec->sh[i].first_mb_in_slice;
            }

        }
        else
        {
#if VERB_ERR_CONC == 1
            NMF_LOG("\nSlice %i discarded\n", i);
#endif
        }
    }

    /* Compute MbSliceMap only once per picture, based on infos from last non-erroneous slice.
       Why choosing the last ? Because in previous version of the code, it was done for all slices
       and each time previous result was smashed, so to keep compatible... */
    if(slices!=0)
    {
        for (i = slices-1; i >= 0; i--)
        {
            if (!dec->sh[i].error)
            {
                MbSliceMap(dec->p_active_sp, dec->p_active_pp, &dec->sh[i], &dec->buf);
                break;
            }
        }
    }

    // Optimized mode , working by MB slice
    dec->buf.DBLK_mode=dec->DBLK_mode;

#ifdef HACK_DISABLE_HED
    dec->buf.HED_used = 0; // HACK TO DISABLE HED on 8500 v1 !!!!
#endif

    // Not possible if FMO or ASO work on full image
    if (FMO_flag || ASO_flag) {dec->buf.DBLK_mode=1;dec->buf.HED_used = 0;}

    if (RS_flag) dec->buf.HED_used = 0; /* HED preprocessor does not support redundant slices */

    ComputeConcealParam(&dec->sh[0],&dec->host,dec);

}

#if VERBOSE_IB == 1
static void PrintIntBuf(t_dec * dec)
{
    t_uint32 word1, word2;
    // print SESB
    t_uint16 i=0;
    do
    {
        word1 = (*(t_uint32 *)(dec->host.buff->bits_intermediate_buffer+i*8));
        word2 = (*(t_uint32 *)(dec->host.buff->bits_intermediate_buffer+i*8+4));
        SWAP32(word1);
        SWAP32(word2);
        NMF_LOG("err_st = %d, last = %d, first = %d, addr = %d\n",(word1>>31),
                ((word1>>30)&0x1),
                ((word1>>16)&0x3FFF),
                ((word1)&0xFFFF));
      NMF_LOG("slice_offset %d\n\n",(word2&0xFFFFF));
        i++;
    }while(((word1>>30)&0x1)!=1);
}
#endif


/*
 this function is called through proxy+ddep, when a panic/hang has happened on DSP side and
 DSP network has been destroyed and rebuilt. Now the decoder has to resend frames to DSP to
 restart the decoding
 */
void METH(error_recovery)()
{
 t_sint16 pipe_read_backup, aux_pipe_read_backup;
 t_hamac_param *param_in;

#if VERBOSE_STANDARD == 1
 NMF_LOG("H264DEC: received error recovery request\n");
#endif

 /* first, save current read position of hamac pipe, we restore it when we exit this function */
 pipe_read_backup = hamac_pipe_get_read();

 /* push again to DSP what was pushed before the crash */
 while(!hamac_pipe_is_empty())
 {
  hamac_pipe_next_read_pos();
  param_in = hamac_pipe_get_read_param_in();
#ifndef __ndk5500_a0__
  iStartCodec.startCodecHed(
			(t_uint32)param_in->addr_in_frame_buffer ,
			(t_uint32)param_in->addr_out_frame_buffer,
			(t_uint32)param_in->addr_internal_buffer,
			(t_uint32)param_in->addr_in_bitstream_buffer,
			(t_uint32)param_in->addr_out_bitstream_buffer,
			(t_uint32)param_in->addr_in_parameters,
			(t_uint32)param_in->addr_out_parameters,
			(t_uint32)param_in->addr_in_frame_parameters,
			(t_uint32)param_in->addr_out_frame_parameters,
      param_in->hed_params.HED_used,
      param_in->hed_params.bitstream_start,
      param_in->hed_params.bitstream_end,
      param_in->hed_params.ib_start,
      param_in->hed_params.hed_cfg,
      param_in->hed_params.hed_picwidth,
      param_in->hed_params.hed_codelength);
#endif
  hamac_pipe_next_read();
 }

 /* restore hamac pipe read position */
 hamac_pipe_set_read(pipe_read_backup);

 /* do the same with void frames ! */
 aux_pipe_read_backup = aux_hamac_pipe_get_read();
 while(!aux_hamac_pipe_is_empty())
 {
#ifndef __ndk5500_a0__
   iStartCodec.voidFrameHed();
#endif
   aux_hamac_pipe_fifo_pop();
 }
 aux_hamac_pipe_set_read(aux_pipe_read_backup);

}
//+Change start for CR336095,ER334368,ER336290 level 5.1. support
t_uint8 METH(CheckLevelVsDPBSize)(t_seq_par *p_sp){
    OstTraceInt3( TRACE_FLOW ,"\nCheckLevelVsDPBSize: getDpbSize(p_sp)=%d,dec_static.buf.AllocatedDPBsize=%d,(mSupportedExtension & (1<<4))=%d",getDpbSize(p_sp),dec_static.buf.AllocatedDPBsize,(mSupportedExtension & (1<<4)));
    if(dec_static.bThumbnailGeneration || (mSupportedExtension & (1<<4)) || (getDpbSize(p_sp)<=dec_static.buf.AllocatedDPBsize)){
        OstTraceInt0( TRACE_FLOW ,"\nCheckLevelVsDPBSize O.K.");
        return 0;
    }
    else{
        OstTraceInt0( TRACE_FLOW ,"\nCheckLevelVsDPBSize N.O.K.");
        return 1;
    }
}

t_uint8 METH(CheckLevelVsResolution)(t_seq_par *p_sp){
    t_uint32 pic_height,maxHeight,maxWidth,tempValue;
    t_uint32 pic_width = (p_sp->pic_width_in_mbs_minus1 + 1) * 16;

    OstTraceInt1(TRACE_FLOW , "H264DEC: CheckLevelVsResolution, Original value of pic_height_in_map_units_minus1 : %d",p_sp->pic_height_in_map_units_minus1);
	if ((!p_sp->frame_mbs_only_flag) && (!p_sp->mb_adaptive_frame_field_flag))
	{
		OstTraceInt0( TRACE_FLOW , "H264DEC: CheckLevelVsResolution Setting new value");
		tempValue = ((p_sp->pic_height_in_map_units_minus1 +1) * 2 ) - 1;
	}
	else
	{
		tempValue = p_sp->pic_height_in_map_units_minus1;
	}
	pic_height = (tempValue + 1) * 16 ;
	OstTraceInt1(TRACE_FLOW , "H264DEC: CheckLevelVsResolution, Value of pic_height : %d",pic_height);

    if(dec_static.isSoc1080pCapable){
        OstTraceInt0( TRACE_FLOW ,"\nCheckLevelVsResolution unrestricted capability");
        maxHeight = 1088;
        maxWidth = 1920;
    }
    else{
        OstTraceInt0( TRACE_FLOW ,"\nCheckLevelVsResolution restricted capability");
        maxHeight = 720;
        maxWidth = 1280;
    }

    // + Change for ER346056
    if((pic_width*pic_height)<=(maxHeight*maxWidth) && (pic_width*pic_height)>=(16*16) ) {
		return 0;
    }
    else{
		OstTraceInt0( TRACE_ERROR ,"\nCheckLevelVsResolution RETURNING ERROR");
		return 1;
    }
    // - Change for ER346056
}

//-Change End for CR336095,ER334368,ER336290 level 5.1. support
//Change for ER357298 Start
t_uint32 METH(pipe_is_ready)()
{
    t_dec *dec=&dec_static;
     //NMF_LOG("\nH264Dec pipe_is_ready,input_control->dpb_frame_available=%d ,dec->sps_active=%d",input_control->dpb_frame_available,dec->sps_active);
    if(!dec->sps_active) {
        if (( ! hamac_pipe_is_full()) && (input_control->dpb_frame_available >= 2) )
        {
            return 1;
        }
    }
    else{
        if (( ! hamac_pipe_is_full()) && (input_control->dpb_frame_available >= 1) )
        {
            return 1;
        }
    }
    return 0;
}
//Change for ER357298 End
