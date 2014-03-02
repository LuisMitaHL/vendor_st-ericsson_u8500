/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    host_decoder.cpp
 * \brief   Main file of VC1 decoder ARM NMF component.
 * \author  ST-Ericsson
 */

#ifdef NMF_BUILD
#include <vc1dec/arm_nmf/decoder.nmf>
#endif
#include "host_alloc.h"
#include "vc1d_co_bitstream.h"
#include "vc1d_ht_picture_layer_parser.h"
#include "vc1d_ht_sequence_header_parser.h"
#include "vc1d_ht_conformance.h"
#include "host_DPB.h"
#include "lifecyclecheck.h"
#include "host_hamac_interface.h"
#include "nmf_lib.h"
#include "ost_event.h"
#include "settings.h"
#include "VFM_Memory.h"

#define Endianess(a)	((((t_uint32)(a) & 0xffff )<<16) + (((t_uint32)(a)>> 16) & 0xffff ))

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "video_components_vc1dec_arm_nmf_proxynmf_decoder_src_host_decoderTraces.h"
#endif
#define SVA_MM_ALIGN_WORD       0x3
#define SVA_MM_ALIGN_8WORDS     0x1f
#define SVA_MM_ALIGN_256BYTES   0xff

/*****************************************************************************/
/**
 * \brief 	Constructor of the decoder NMF component
 * \return  OK
 */
/*****************************************************************************/
t_nmf_error METH(construct)()
{
   OstTraceInt0(TRACE_API,  "VC1Dec: arm_nmf: decoder: construct()\n");
#if VERBOSE_STANDARD == 1
  NMF_LOG("NMF-ARM decoder: call to construct()\n");
    	// Note ostTrcFiltInst0 can not be used at that time: ENS has not alloc memory yet  (done in fsmInit)
#endif
	//MeasureEventInit();
   dec_static.sva_bypass = 0;  // this is only used for ARM load evaluations
   p_decoder_seq_param = NULL;
   dec_static.sva_buffers.p_mv_history_buf=(t_uint8*)NULL;
   dec_static.sva_buffers.aux_frame =(t_uint8*)NULL;
   dec_static.sva_buffers.cup_context =(t_uint8*)NULL;
   dec_static.sva_buffers.p_deblocking_paramv=(t_uint8*)NULL;
   dec_static.sva_buffers.local_recon_buff=(t_uint8*)NULL;
   dec_static.sva_buffers.p_param_inout=(t_uint32)NULL;
   dec_static.buf.frames = (t_frames*)NULL;
   dec_static.bImmediateRelease = 0;
   seq_param_received = FALSE;
   return NMF_OK;
}


/*****************************************************************************/
/**
 * \brief 	Stop of the decoder NMF component
 * \author 	Maurizio Colombo
 * \return  none
 */
/*****************************************************************************/
void METH(stop)(void)
{
     	OstTraceFiltInst0(TRACE_API,  "VC1Dec: arm_nmf: decoder: stop()\n");
#if VERBOSE_STANDARD == 1
    NMF_LOG("NMF-ARM decoder: call to stop()\n");
#endif
}

/*****************************************************************************/
/**
 * \brief 	Destroy of the decoder NMF component
 * \return  none
 */
/*****************************************************************************/
void METH(destroy)(void)
{
     	OstTraceFiltInst0(TRACE_API,  "VC1Dec: arm_nmf: decoder: destroy()\n");
#if VERBOSE_STANDARD == 1
    NMF_LOG("NMF-ARM decoder: call to destroy()\n");
#endif
  	//MeasureEventDisplayStatus();
   // +ER 341788 CHANGE START FOR
    t_hamac_param *param_in;
    t_void_frame_info param_info;
     t_sint16 pipe_read_backup;

  	if(!hamac_pipe_is_empty())
  	{

        OstTraceFiltInst0(TRACE_FLOW,  "VC1Dec: arm_nmf: decoder: HAMAC PIPE NOT EMPTY\n");
	//	NMF_LOG("PIPE NOT EMPTY \n");
        // input_decoder_command(COMMAND_FLUSH);
	}
	else
	{
         OstTraceFiltInst0(TRACE_FLOW,  "VC1Dec: arm_nmf: decoder: HAMAC PIPE EMPTY\n");
	//	NMF_LOG("PIPE EMPTY \n");
	}
    if( !vfm_memory_ctxt )
        {

        OstTraceInt0(TRACE_WARNING,  "VC1Dec: arm_nmf: decoder: WARNING destroy() with vfm_memory_ctxt==NULL");
       // NMF_LOG("WARNING destroy() with vfm_memory_ctxt==NULL");
        return; // could be the case if the construct fails
        }

	while(!hamac_pipe_is_empty())
	 {
		  hamac_pipe_next_read_pos();
		  param_in =  hamac_pipe_get_read_param_in();


        param_info = hamac_pipe_get_void_frame_info() ;
          for (t_uint16 i=0; i<HAMAC_PIPE_SIZE + 1; i++)
          {
              OstTraceFiltInst1(TRACE_FLOW,  "param_info  0x %x  \n", param_info);
              OstTraceFiltInst1(TRACE_FLOW,  "addr_in_frame_buffer  0x %x  ", OMX_U32(param_in->addr_in_frame_buffer));
           // NMF_LOG("\n param_info  0x %x  \n", param_info);
                //  NMF_LOG("\n addr_in_frame_buffer  0x %x  \n", param_in->addr_in_frame_buffer);
                 // NMF_LOG("\n &dec_static.hamac_pipe.hamac_pipe_info[i].param_in  0x %x  \n",dec_static.hamac_pipe.hamac_pipe_info[i].param_in);
			if ((param_in == (&dec_static.hamac_pipe.hamac_pipe_info[i].param_in)) &&
                  (!dec_static.hamac_pipe.hamac_pipe_info[i].void_frame_info))
				{
					HamacToLogicalAddresses(vfm_memory_ctxt,&dec_static.hamac_pipe.hamac_pipe_info[i].param_in, 0);
				}
		    }

		hamac_pipe_next_read();
	 }

      	if(!hamac_pipe_is_empty())
  	{
		//NMF_LOG("\nAFTER PIPE NOT EMPTY \n");
	}
	else
	{

		//NMF_LOG("\nAFTER PIPE EMPTY \n");
    	}
 // +ER 341788 CHANGE START FOR
    for (t_uint16 i=0; i<HAMAC_PIPE_SIZE+1; i++)
    {
        OstTraceFiltInst0(TRACE_FLOW,  "VC1Dec: arm_nmf: decoder: Calling HamacDeallocParameter()\n");
	     HamacDeallocParameter(vfm_memory_ctxt,&dec_static.hamac_pipe.hamac_pipe_info[i].param_in,0);
    }

    // Input parameters
    if(p_decoder_seq_param)
        {

        OstTraceFiltInst0(TRACE_FLOW,  "VC1Dec: arm_nmf: decoder: About to Call HamacDeallocParameterIn()\n");
        HamacDeallocParameterIn(vfm_memory_ctxt,p_decoder_seq_param);
        p_decoder_seq_param = NULL;
        }

    // Motion Vector History
    if(dec_static.sva_buffers.p_mv_history_buf!=(t_uint8*)NULL)
    {

        OstTraceFiltInst0(TRACE_FLOW,  "VC1Dec: arm_nmf: decoder: About to Free Motion vector Buff\n");
    	FREE(dec_static.sva_buffers.p_mv_history_buf);
    	dec_static.sva_buffers.p_mv_history_buf = (t_uint8*)NULL;
    }

    // VPP Dummy
    if(dec_static.sva_buffers.aux_frame!=(t_uint8*)NULL)
    {

        OstTraceFiltInst0(TRACE_FLOW,  "VC1Dec: arm_nmf: decoder: About to Free AUX Frame\n");
    	FREE(dec_static.sva_buffers.aux_frame);
    	dec_static.sva_buffers.aux_frame = (t_uint8*)NULL;
    }

    // DMA Cup Context
    if(dec_static.sva_buffers.cup_context!=(t_uint8*)NULL)
    {

        OstTraceFiltInst0(TRACE_FLOW,  "VC1Dec: arm_nmf: decoder: About to Free DMA Cup Context\n");
    	FREE(dec_static.sva_buffers.cup_context);
    	dec_static.sva_buffers.cup_context = (t_uint8*)NULL;
    }

    // Deblocking parameters
    if(dec_static.sva_buffers.p_deblocking_paramv!=(t_uint8*)NULL)
    {

        OstTraceFiltInst0(TRACE_FLOW,  "VC1Dec: arm_nmf: decoder: About to Free Deblocking parameters\n");
    	FREE(dec_static.sva_buffers.p_deblocking_paramv);
    	dec_static.sva_buffers.p_deblocking_paramv = (t_uint8*)NULL;
    }

    // Reconstructed Local Buffer
    if(dec_static.sva_buffers.local_recon_buff!=(t_uint8*)NULL)
    {

        OstTraceFiltInst0(TRACE_FLOW,  "VC1Dec: arm_nmf: decoder: About to Free Reconstructed Local Buffer\n");
    	FREE(dec_static.sva_buffers.local_recon_buff);
    	dec_static.sva_buffers.local_recon_buff = (t_uint8*)NULL;
    }

    // Init Input / Output parameters
    if(dec_static.sva_buffers.p_param_inout!=(t_uint32)NULL)
    {

        OstTraceFiltInst0(TRACE_FLOW,  "VC1Dec: arm_nmf: decoder: About to Free Init Input / Output parameters\n");
    	FREE(dec_static.sva_buffers.p_param_inout);
    	dec_static.sva_buffers.p_param_inout = (t_uint32)NULL;
    }

    if(dec_static.buf.frames != (t_frames*)NULL)
    {
        OstTraceFiltInst0(TRACE_FLOW,  "VC1Dec: arm_nmf: decoder: About to call display_queue_free_frames_array() \n");
        display_queue_free_frames_array(&dec_static.buf,vfm_memory_ctxt);
    }
}


/*****************************************************************************/
/**
 * \brief 	initialization input parameters
 * \author 	Olivier Barault
 * \return  none
 */
/*****************************************************************************/
void METH(init_codec)(void)
{
    	ts_t1xhv_vdc_vc1_param_in_common *param_in;

     	OstTraceFiltInst0(TRACE_API,  "VC1Dec: arm_nmf: decoder: init_codec()\n");
#if VERBOSE_STANDARD == 1
    	NMF_LOG("NMF-ARM decoder: call to init_codec()\n");
#endif

    	HamacAllocParameterIn(vfm_memory_ctxt,&p_decoder_seq_param);
    	HamacFillParameterIn(vfm_memory_ctxt,p_decoder_seq_param,&seq_params,picture_width,picture_height);

	param_in = p_decoder_seq_param;
	HamacToPhysicalAddresses(vfm_memory_ctxt,&param_in);

      	iStartCodec.initCodec((t_uint32)param_in);
}


/*****************************************************************************/
/**
 * \brief 	Interface to store vfm memory context pointer from app
 * \author 	Maurizio Colombo
 * \return  none
 */
/*****************************************************************************/
void METH(set_memory_ctxt)(void *mem_ctxt)
{
    t_uint16 i=0;

     	OstTraceFiltInst0(TRACE_API,  "VC1Dec: arm_nmf: decoder: set_memory_ctxt()\n");
#if VERBOSE_STANDARD == 1
    NMF_LOG("NMF-ARM decoder: call to set_memory_ctxt()\n");
#endif

    if (mem_ctxt != 0)
    {
        // this interface should be called as soon as possible by the ddep / application
        vfm_memory_ctxt = mem_ctxt;

        input_control_init(&dec_static.input_control);
        input_control->dpb_frame_available--; /* in case of seek we need one output buffer more
                                                 in order to start */
        hamac_pipe_init(&dec_static.hamac_pipe);
        hamac_pipe_next_read_pos();

        for(i=0;i<HAMAC_PIPE_SIZE+1; i++)
        {
            OstTraceFiltInst0(TRACE_FLOW,  "VC1Dec: arm_nmf: decoder:Abt to call HamacAllocParameter\n");
            HamacAllocParameter(vfm_memory_ctxt,&dec_static.hamac_pipe.hamac_pipe_info[i].param_in, 0);
        }

        // Motion Vector History
        t_uint32 max_mb_frame = (t_uint32)EXTEND_TO_16(picture_width)
                            * (t_uint32)EXTEND_TO_16(picture_height)/(16L*16L);
        t_uint32 mv_history_size = max_mb_frame * sizeof(t_uint32);
     	OstTraceFiltInst1(TRACE_API,  "VC1Dec: arm_nmf: decoder: allocate memory of size %d\n",mv_history_size);
        dec_static.sva_buffers.p_mv_history_buf = (t_uint8*)MPC_ALLOC_SDRAM(mv_history_size, SVA_MM_ALIGN_8WORDS);
        if( ! dec_static.sva_buffers.p_mv_history_buf )
        {
            NMF_PANIC("VC1 decoder set_memory_context: out of memory for dec_static.sva_buffers.p_mv_history_buf");
        }

        // MC : initialize MV history to 0 !
        memset((t_uint8*)dec_static.sva_buffers.p_mv_history_buf,0,mv_history_size);

    	// DMA Cup Context
    	// update for V1 : use of only 1 line (instead of full frame)
#ifdef __ndk8500_a0__
    	dec_static.sva_buffers.cup_context = (t_uint8*)HWBUFF_ALLOC(104*80*sizeof(t_uint8), SVA_MM_ALIGN_8WORDS);
      memset((t_uint8*)dec_static.sva_buffers.cup_context,0,104*80);
#else
    	dec_static.sva_buffers.cup_context = (t_uint8*)HWBUFF_ALLOC(104*3600*sizeof(t_uint8), SVA_MM_ALIGN_8WORDS);
      memset((t_uint8*)dec_static.sva_buffers.cup_context,0,104*3600);
#endif

        	display_queue_init_frames_array(&dec_static.buf,
                                      MAXNUMFRM+HAMAC_PIPE_SIZE+1+DISPLAY_QUEUE_SIZE+DEF_FTB_SIZE,
                                      vfm_memory_ctxt);

	// Deblocking parameters
    	dec_static.sva_buffers.p_deblocking_paramv = (t_uint8*)HWBUFF_ALLOC(8*3600*sizeof(t_uint8), SVA_MM_ALIGN_8WORDS);

	// Reconstructed Local Buffer
	// 5 lines of MB (16*16 pixels) allocated in ESRAM
	t_uint32 rlb_width = (t_uint32)EXTEND_TO_16(picture_width);
	t_uint32 rlb_height = 5*16;
        t_uint32 rlb_size = ((rlb_width * rlb_height * 3)/2)+256; // leave a margin of 256 for later alignment

        OstTraceFiltInst1(TRACE_API,  "VC1Dec: arm_nmf: decoder: allocate memory for local_recon_buff of size %d\n",rlb_size);
        dec_static.sva_buffers.local_recon_buff = (t_uint8*)MPC_ALLOC(rlb_size, SVA_MM_ALIGN_256BYTES);

    	// VPP Dummy
       	dec_static.sva_buffers.aux_frame = (t_uint8*)MPC_ALLOC(4*sizeof(t_uint32), SVA_MM_ALIGN_256BYTES);


        OstTraceFiltInst1(TRACE_API,  "VC1Dec: arm_nmf: decoder: allocate memory for local_recon_buff of size %d\n",(sizeof(ts_t1xhv_vdc_vc1_param_inout)));
       	// Init Input / Output parameters
    	dec_static.sva_buffers.p_param_inout = (t_uint32)HWBUFF_ALLOC(sizeof(ts_t1xhv_vdc_vc1_param_inout), SVA_MM_ALIGN_8WORDS);
    	((ts_t1xhv_vdc_vc1_param_inout*)(dec_static.sva_buffers.p_param_inout))->intensity_compensate_enabled = FALSE;
        	((ts_t1xhv_vdc_vc1_param_inout*)(dec_static.sva_buffers.p_param_inout))->luma_scale = 0;
        	((ts_t1xhv_vdc_vc1_param_inout*)(dec_static.sva_buffers.p_param_inout))->luma_shift = 0;
        	((ts_t1xhv_vdc_vc1_param_inout*)(dec_static.sva_buffers.p_param_inout))->rnd_ctrl = 0;
        	((ts_t1xhv_vdc_vc1_param_inout*)(dec_static.sva_buffers.p_param_inout))->last_ref_rangered_enabled = FALSE;
        	((ts_t1xhv_vdc_vc1_param_inout*)(dec_static.sva_buffers.p_param_inout))->previous_last_ref_rangered_enabled = FALSE;
        	// not initialized (probably done when decoding first I-Frame)
        	//dec_static.sva_buffers.p_param_inout->last_ref_interpolation_hint_enabled
        	//dec_static.sva_buffers.p_param_inout->last_ref_buffer_fullness
        	//dec_static.sva_buffers.p_param_inout->reference_resolution

      	InitBuffer(&dec_static.buf);
	ref_frame_initialized = FALSE;
	low_power_state = FALSE;
    }
}


/*****************************************************************************/
/**
 * \brief 	Interface to pass sequence-level parameters to the decoder
 *           NMF component. Those parameters are parsed by the demuxer
 *           from the RCV file header extension and given to decoder
 *           in an OMX structure
 * \author 	Maurizio Colombo
 * \return  none
 */
/*****************************************************************************/
void METH(set_picsize)
		(t_uint16 pic_width,
		 t_uint16 pic_height)
{
     	OstTraceInt0(TRACE_API,  "VC1Dec: arm_nmf: decoder: set_picsize()\n");
#if VERBOSE_STANDARD == 1
    NMF_LOG("NMF-ARM decoder: call to set_picsize()\n");
#endif

      picture_width = pic_width;
      picture_height = pic_height;
}

/*****************************************************************************/
/**
 * \brief 	Interface to pass a coded frame to the decoder
 *           NMF component. The provided buffer pointer points to the first
 *           frame data byte, not to the RCV frame header which is part of
 *           file format and thus parsed by demuxer
 * \author 	Maurizio Colombo
 * \param   p_bitstream      descriptor of bitstream buffer
 * \param   sei              metadata information for the timestamp
 * \param   nFlags          flag to handle end of sequence
 * \return  none
 */
/*****************************************************************************/
void METH(frame)
		(t_bit_buffer	*p_bitstream,	/**< Input buffer */
		void 		*sei, 		/**< TimeStamp */
	  t_uint32 nTimeStampH, t_uint32 nTimeStampL, /**< TimeStamp */
		t_uint32 	nFlags)	/**< Openmax flags */
{
	  t_vc1_picture_type 	picture_type;
		t_hamac_param *param_in;
    ts_bitstream_buffer  decoder_bitstream;


	if( p_bitstream )
	{
     	OstTraceFiltInst1(TRACE_API,  "VC1Dec: arm_nmf: decoder: frame(): size = %d\\n", p_bitstream->end - p_bitstream->addr);
#if VERBOSE_STANDARD == 1
		NMF_LOG("NMF-ARM decoder: call to frame(), size = %d\n",p_bitstream->end - p_bitstream->addr);
#endif

	}


	if(p_bitstream == NULL)
	{
		if( nFlags & OSI_OMX_BUFFERFLAG_EOS )
		{
			// Case of end of stream.
			// all frame data have already been received - last buffer is empty and only used to send EOS flag

            OstTraceFiltInst0(TRACE_FLOW,  "Handle end of stream (empty buffer)\n");
#if VERBOSE_STANDARD == 1
      			NMF_LOG("Handle end of stream (empty buffer)\n");
#endif
      			// we post a void frame (fake frame to go through the pipe)
			// The void frame will be recognized in the output stage
			// this will indicate that all received frames have been decoded (we can bump last frame)

    			hamac_pipe_reset_frame();

    			//commented for ER 370888
			//hamac_pipe_set_void_frame_info(VOID_FRAME_EOS_WITH_FLUSH);

			// BumpFrame will choose oldest ref, so let's mark most recent ref as oldest ref
      			if(dec_static.buf.oldest_ref == 0)
         			dec_static.buf.oldest_ref = 1;
      			else
         			dec_static.buf.oldest_ref = 0;
      			// Here call it with Pic type != B in order to make it bump the oldest ref
      			BumpFrame(PICTURE_TYPE_I,&dec_static.buf);

				//+change for ER 370888
      			if (hamac_pipe_mark_eos_frame())   // this will set OMX EOS flag in last buffer, needed by proxy
      			{
					OstTraceFiltInst0(TRACE_FLOW,  "VC1Dec: arm_nmf: decoder: TRUE for hamac_pipe_mark_eos_frame()");
					hamac_pipe_set_void_frame_info(VOID_FRAME_EOS_WITH_FLUSH);
				}
				else
				{
					OstTraceFiltInst0(TRACE_FLOW,  "VC1Dec: arm_nmf: decoder: FALSE for hamac_pipe_mark_eos_frame()");
					hamac_pipe_set_void_frame_info(VOID_FRAME_EOS_WO_FLUSH);
				}
				//- change for ER 370888

			if (dec_static.sva_bypass == 1) {
				endCodec(STATUS_JOB_COMPLETE, VDC_VOID, 0);
			}
			else {
				iStartCodec.voidFrame();
			}
		}

		return;
	}

	/* wrap bitstream structure into VC1 bitstream structure */
    	decoder_bitstream.data = p_bitstream->addr;
    	decoder_bitstream.size_in_bits = 8 * (p_bitstream->end - p_bitstream->addr);
    	decoder_bitstream.pt_current_byte = p_bitstream->addr;
    	decoder_bitstream.index_current_bit = 0;

	if(seq_param_received == FALSE)
	{
		// first buffer : contains sequence parameters (no frame data)
		OMX_ERRORTYPE error = parse_sequence_header(&decoder_bitstream,&seq_params); // For ER 344032
        /* + For ER 344032 */
		if (error == OMX_ErrorFormatNotDetected)
        {
            OstTraceInt0(TRACE_ERROR,  "VC1Dec: arm_nmf: decoder: frame():Error OMX_ErrorFormatNotDetected ");
            proxy.eventHandler(OMX_EventError,error,0);
        }
		/* For ER 344032 */
        seq_param_received = TRUE;
		// We will now estimate the level from the number of MBs per frame
    		seq_params.level = find_level_from_image_size(seq_params.profile,
                                                        	picture_width,
                                                        	picture_height);

		// init input parameters
		init_codec();

		// release bitstream and request a new input buffer
		p_bitstream->next = 0;  // don't use linked lists
    		p_bitstream->inuse = 0;  // tell to input control that this buffer is not in use
    		input_control_acknowledge(p_bitstream, XON);
	}
	else
	{
		// following buffers : contain frames data

    //MeasureEventStart0(TRACE_EVENT_BUFFER_START,"decoder buffer processing");

	  // Init DPB with a dummy reference (grey pixels)
	  // -> used to decode P or B-frames after seek
	  // -> used for error concealment
	  if( !ref_frame_initialized )
	    {
	      InsertDummyRefIntoDpb(&dec_static.buf,
				    picture_width,
				    picture_height);
	      ref_frame_initialized = TRUE;
	    }

	  	// exit pause state when receive a new frame
	  	if( low_power_state == TRUE )
	  	{
	  		low_power_state = FALSE;
	  		iStartCodec.preventSleep();
	  	}

    		// We read the frame type
   	 	btpar_parse_picture_type(&decoder_bitstream,&seq_params,&picture_type);

    		// Get the position in the DPB corresponding to current frame
    		t_sint16 index = InsertFrame(picture_type,&dec_static.buf, 0, &dec_static.sva_buffers);
    		dec_static.buf.curr_info = &dec_static.buf.pics_buf[index];
    		dec_static.sva_buffers.curr_frame = dec_static.buf.curr_info->ptr[0];
    		dec_static.sva_buffers.curr_bitstream = p_bitstream;

    		// store timestamp associated to current picture into the frames table
    		// this info is not used by the decoder itself, it is just carried from input to output buffers
    		dec_static.buf.frames[dec_static.buf.curr_info->frame_pos].frameinfo.common_frameinfo.nTimeStampH = nTimeStampH;
    		dec_static.buf.frames[dec_static.buf.curr_info->frame_pos].frameinfo.common_frameinfo.nTimeStampL = nTimeStampL;

    		// store Openmax flags (end of stream, ...)
    		dec_static.buf.frames[dec_static.buf.curr_info->frame_pos].nFlags = nFlags;

		// Get the pointer on a Param_in structure , from the fifo use in front of HW decoder
    		hamac_pipe_reset_frame();
		param_in = hamac_pipe_get_write_param_in();
		HamacFillParamIn(vfm_memory_ctxt,&dec_static,0,param_in);
    		hamac_pipe_set_frame_pos(dec_static.buf.curr_info->frame_pos);

    		hamac_pipe_set_bit_buffer_addr(p_bitstream); // store bitstream addr to be able to release it after decoding
    		p_bitstream->next = 0;  // don't use linked lists
    		p_bitstream->inuse = VCL_UNDER_PROCESSING;  // tell to input control that this buffer is in use
    		input_control_acknowledge(p_bitstream, XON);      // this will make p_bitstream as pending for release
                                                 	// and will request a new input buffer if the pipe is not full

    		// Determine which picture will be output after current frame's decoding
    		BumpFrame(picture_type,&dec_static.buf);

    		// Case of end of stream. In all cases, need to output most recent ref
    		if( nFlags & OSI_OMX_BUFFERFLAG_EOS )
    		{
        OstTraceFiltInst1(TRACE_FLOW, "VC1Dec: arm_nmf: decoder: frame(): Handle end of stream - Flow line %d\n", __LINE__);
#if VERBOSE_STANDARD == 1
      			NMF_LOG("Handle end of stream\n");
#endif
      			// BumpFrame will choose oldest ref, so let's mark most recent ref as oldest ref
      			if(dec_static.buf.oldest_ref == 0)
         			dec_static.buf.oldest_ref = 1;
      			else
         			dec_static.buf.oldest_ref = 0;
      			// Here call it with Pic type != B in order to make it bump the oldest ref
      			BumpFrame(PICTURE_TYPE_I,&dec_static.buf);

      			hamac_pipe_mark_eos_frame();   // this will set OMX EOS flag in last buffer, needed by proxy
    		}

	if (dec_static.sva_bypass == 1)
  {
     // bypass DSP processing, only used for ARM load evaluation
     hamac_pipe_set_void_frame_info(VOID_FRAME_ALGO_BYPASS);
	   endCodec(STATUS_JOB_COMPLETE, VDC_VOID, 0);
  }
  else
  {

#ifdef _CACHE_OPT_
    	// Force cache clean to be sure that bitstream has been written in SDRAM
	    t_uint8 *fl_end_addr,*fl_start_addr;
	    fl_start_addr= (t_uint8*) p_bitstream->addr;
	    fl_end_addr=(t_uint8*) p_bitstream->end;
	    VFM_CacheClean(vfm_memory_ctxt, fl_start_addr,fl_end_addr-fl_start_addr+1);
	    OMX_U32 tempAddr;
	    OMX_U8 *tempPtr;

	    tempAddr = Endianess(param_in->addr_out_frame_buffer->addr_dest_buffer);
	    tempPtr = VFM_GetLogical(vfm_memory_ctxt, (t_uint8 *)tempAddr);
		VFM_CacheInvalidate(vfm_memory_ctxt, tempPtr,(picture_height*picture_width*1.5));

#endif

   		HamacToPhysicalAddresses(vfm_memory_ctxt,param_in, 0);

#if VERBOSE_STANDARD == 1
   		NMF_LOG("NMF-ARM decoder: call to startCodec\n");
#endif
   		iStartCodec.startCodec((t_uint32)param_in->addr_in_frame_buffer ,
				                  (t_uint32)param_in->addr_out_frame_buffer,
				                  (t_uint32)param_in->addr_internal_buffer,
				                  (t_uint32)param_in->addr_in_bitstream_buffer,
				                  (t_uint32)param_in->addr_out_bitstream_buffer,
				                  (t_uint32)param_in->addr_in_parameters,
				                  (t_uint32)param_in->addr_out_parameters,
                  				(t_uint32)param_in->addr_in_frame_parameters,
                  				(t_uint32)param_in->addr_out_frame_parameters);
   } // no SVA bypass

    //MeasureEventStop(TRACE_EVENT_BUFFER_STOP,"decoder buffer processing");
	}
}

/*****************************************************************************/
/**
 * \brief 	Interface to provide a YUV buffer to fill to the decoder
 *           NMF component.
 * \author 	Maurizio Colombo
 * \param   frame      descriptor of YUV buffer
 * \return  none
 */
/*****************************************************************************/
void METH(output_buffer)
		(void	*frame, 	/**< buffer to be used as decoder output */
		 t_uint32 size)
{
     	OstTraceFiltInst1(TRACE_API,  "VC1Dec: arm_nmf: decoder: output_buffer(): frame = %x\n", (unsigned int)frame);

#if VERBOSE_STANDARD == 1
    NMF_LOG("NMF-ARM decoder: call to output_buffer(), frame = %x\n",frame);
#endif
    t_dec *dec=&dec_static;
    if (frame!=NULL) display_queue_new_output_buffer(dec, frame);
}

/*****************************************************************************/
/**
 * \brief 	Callback from Hamac side providing current decoded picture
 * \author 	Maurizio Colombo
 * \param   frame      descriptor of YUV buffer
 * \param   param_out  output parameters filled by Hamac part
 * \return  none
 */
/*****************************************************************************/
void METH(endCodec)
		(t_t1xhv_status status, t_t1xhv_decoder_info info, t_uint32 durationInTicks)
{
	t_hamac_param *param_in;
    t_bit_buffer * bits_buffer_emptied;
    t_dec *dec=&dec_static;
   	OstTraceFiltInst0(TRACE_API,  "VC1Dec: arm_nmf: decoder: endCodec()\n");
#if VERBOSE_STANDARD == 1
    NMF_LOG("NMF-ARM decoder: call to endCodec()\n");
#endif

	if((status==STATUS_JOB_COMPLETE)&&(info==VDC_VOID))
	{
        OstTraceFiltInst0(TRACE_FLOW,  "VC1Dec: arm_nmf: decoder: endCodec() Void Frame \n");
		// Void Frame
		hamac_pipe_next_read_pos();

   		t_void_frame_info void_frame_info = hamac_pipe_get_void_frame_info();

		hamac_pipe_bump_frames(&dec_static);
		hamac_pipe_void_frame(&dec_static, void_frame_info);

		hamac_pipe_next_read(); /* free position in hamac_pipe */

		if( void_frame_info == VOID_FRAME_PAUSE_INFO )
		{
			low_power_state = TRUE;
			iStartCodec.allowSleep();
		}

		if( void_frame_info == VOID_FRAME_OUTPUT_FLUSH )
		{
			// Reset sva buffers
			t_uint32 max_mb_frame = (t_uint32)EXTEND_TO_16(picture_width)
		  	* (t_uint32)EXTEND_TO_16(picture_height)/(16L*16L);
			t_uint32 mv_history_size = max_mb_frame * sizeof(t_uint32);
			memset((t_uint8*)dec_static.sva_buffers.p_mv_history_buf,0,mv_history_size);
#ifdef __ndk8500_a0__
			memset((t_uint8*)dec_static.sva_buffers.cup_context,0,104*80);
#else
			memset((t_uint8*)dec_static.sva_buffers.cup_context,0,104*3600);
#endif
			((ts_t1xhv_vdc_vc1_param_inout*)(dec_static.sva_buffers.p_param_inout))->intensity_compensate_enabled = FALSE;
        		((ts_t1xhv_vdc_vc1_param_inout*)(dec_static.sva_buffers.p_param_inout))->luma_scale = 0;
        		((ts_t1xhv_vdc_vc1_param_inout*)(dec_static.sva_buffers.p_param_inout))->luma_shift = 0;
        		((ts_t1xhv_vdc_vc1_param_inout*)(dec_static.sva_buffers.p_param_inout))->rnd_ctrl = 0;
        		((ts_t1xhv_vdc_vc1_param_inout*)(dec_static.sva_buffers.p_param_inout))->last_ref_rangered_enabled = FALSE;
        		((ts_t1xhv_vdc_vc1_param_inout*)(dec_static.sva_buffers.p_param_inout))->previous_last_ref_rangered_enabled = FALSE;

			InitBuffer(&dec_static.buf);
			ref_frame_initialized = FALSE;
		}
	}
	else
	{

		// Picture Frame


    /* give back to appli pic to be bumped from pipe */
   	hamac_pipe_next_read_pos();
   	param_in = hamac_pipe_get_read_param_in();

	HamacToLogicalAddresses(vfm_memory_ctxt,param_in, 0);

    OMX_U32 tempAddr;
    OMX_U8 *tempPtr;
    tempAddr = Endianess(param_in->addr_out_frame_buffer->addr_dest_buffer);
    tempPtr = VFM_GetLogical(vfm_memory_ctxt, (t_uint8 *)tempAddr);
	VFM_CacheInvalidate(vfm_memory_ctxt, tempPtr,(picture_height*picture_width*1.5));

    /* store param out */
    t_uint16 frame_pos = hamac_pipe_get_frame_pos();
    t_frameinfo * picinfo = &(dec_static.buf.frames[frame_pos].frameinfo);
    ts_t1xhv_vdc_vc1_param_out * p_out = (ts_t1xhv_vdc_vc1_param_out *) (param_in->addr_out_parameters);

    picinfo->specific_frameinfo.error_type                       = p_out->error_type;
    picinfo->specific_frameinfo.frame_interpolation_hint_enabled = p_out->frame_interpolation_hint_enabled;
    picinfo->specific_frameinfo.range_reduction_frame_enabled    = p_out->range_reduction_frame_enabled;
    picinfo->specific_frameinfo.b_fraction_numerator             = p_out->b_fraction_numerator;
    picinfo->specific_frameinfo.b_fraction_denominator           = p_out->b_fraction_denominator;
    picinfo->specific_frameinfo.buffer_fullness                  = p_out->buffer_fullness;
    picinfo->specific_frameinfo.picture_res                      = p_out->picture_res;
    picinfo->specific_frameinfo.max_picture_width                = p_out->max_picture_width;
    picinfo->specific_frameinfo.max_picture_height               = p_out->max_picture_height;
    picinfo->specific_frameinfo.picture_width                    = p_out->picture_width;
    picinfo->specific_frameinfo.picture_height                   = p_out->picture_height;
    picinfo->specific_frameinfo.picture_type                     = p_out->picture_type;

	if(( picinfo->specific_frameinfo.picture_type==0)||(picinfo->specific_frameinfo.picture_type==0x01b))
	{
    //dec->bImmediateRelease=1;
#if VERBOSE_STANDARD == 1
    NMF_LOG("\nFOUND I Frame value %d\n",picinfo->specific_frameinfo.picture_type);
#endif
    	OstTraceFiltInst1(TRACE_FLOW,  "\nFOUND I Frame value %d\n",picinfo->specific_frameinfo.picture_type);
    }
 	if(dec->bImmediateRelease)
	{
	    OstTraceFiltInst0(TRACE_FLOW,  "\n\t bImmediateRelease is set to TRUE, bumping frame immediately \n");
	    BumpFrame(PICTURE_TYPE_I,&dec->buf);
		dec->bImmediateRelease = 0;
	}
   	// After call back from algo part, bumping frames
	hamac_pipe_bump_frames(&dec_static);

    bits_buffer_emptied = hamac_pipe_get_bit_buffer_addr();
    input_control_acknowledge_pending(bits_buffer_emptied); // empty buffer done

    hamac_pipe_next_read(); /* free position in hamac_pipe */

	} // if((status==STATUS_JOB_COMPLETE)&&(info==VDC_VOID))

  //MeasureEventStop(TRACE_EVENT_2_STOP,"process end frame");
}


/*****************************************************************************/
/**
 * \brief 	Manage Input command
 * \author 	Olivier Barault
 * \param   command_type      type of command
 *				COMMAND_FLUSH	flush buffers in order to stop decode
  *				COMMAND_PAUSE	wait all the frames to be decoded before pause
* \return  none
 */
/*****************************************************************************/
void METH(input_decoder_command)(t_command_type command_type) {
	if ((command_type == COMMAND_FLUSH) || (command_type == COMMAND_PAUSE)) {
		output_decoder_command(command_type);
	}
}


/*****************************************************************************/
/**
 * \brief 	Manage Output command
 * \author 	Olivier Barault
 * \param   command_type      type of command
 *				COMMAND_FLUSH	flush buffers in order to stop decode
 *				COMMAND_PAUSE	wait all the frames to be decoded before pause
 * \return  none
 */
/*****************************************************************************/
void METH(output_decoder_command)(t_command_type command_type) {
     	OstTraceFiltInst0(TRACE_API,  "VC1Dec: arm_nmf: decoder: output_decoder_command()\n");
#if VERBOSE_STANDARD == 1
    NMF_LOG("NMF-ARM decoder: call to output_decoder_command()\n");
#endif
	if (command_type == COMMAND_FLUSH)
	{
     	OstTraceFiltInst0(TRACE_FLOW,  "VC1Dec: arm_nmf: decoder:  output_decoder_command(): COMMAND_FLUSH\n");
#if VERBOSE_STANDARD == 1
		NMF_LOG("NMF-ARM decoder: COMMAND_FLUSH\n");
#endif
		// On a flush command , we post a void frame (fake frame to go through the pipe)
		// Then we flush the DPB , so all available frames are added to the list of frame to be bumped
		// The void frame will be recognized as a FLUSH frame in the output stage
		// this will force the return of all the output buffers available in the free pool of buffer
		// input buffer are mechanically return as soon as a frame is decoded

    		hamac_pipe_reset_frame();
          	hamac_pipe_set_void_frame_info(VOID_FRAME_OUTPUT_FLUSH);
    		FlushDPB(&dec_static.buf); // Flush the DPB

		if (dec_static.sva_bypass == 1) {
			endCodec(STATUS_JOB_COMPLETE, VDC_VOID, 0);
		}
		else {
			iStartCodec.voidFrame();
		}
	}

	if (command_type == COMMAND_PAUSE)
	{
     	OstTraceFiltInst0(TRACE_FLOW,  "VC1Dec: arm_nmf: decoder:  output_decoder_command(): COMMAND_PAUSE\n");
#if VERBOSE_STANDARD == 1
		NMF_LOG("NMF-ARM decoder: COMMAND_PAUSE\n");
#endif
		// On a pause command , we post a void frame (fake frame to go through the pipe)
		// The void frame will be recognized as a PAUSE frame in the output stage
		// this will indicate that all received frames have been decoded (then the client can enter in pause state)

    		hamac_pipe_reset_frame();
		hamac_pipe_set_void_frame_info(VOID_FRAME_PAUSE_INFO);

		if (dec_static.sva_bypass == 1) {
			endCodec(STATUS_JOB_COMPLETE, VDC_VOID, 0);
		}
		else {
			iStartCodec.voidFrame();
		}
	}
}

void METH(initTraceInfo)(TraceInfo_t * ptr, unsigned int val)
{
#if VERBOSE_STANDARD == 1
	NMF_LOG("NMF-ARM decoder: initTraceInfo\n");
#endif
    setTraceInfo(ptr,val);
	mpc_trace_init.traceInit(*ptr,(t_uint16)val);
}

void METH(set_ImmediateRelease)(t_uint16 bImmediateRelease) {
    dec_static.bImmediateRelease = bImmediateRelease;


}
