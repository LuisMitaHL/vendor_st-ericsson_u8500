/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include <vc1dec/arm_nmf/decoder.nmf>
#include "settings.h"
#include "types.h"
#include "host_types.h"
#include "host_decoder.h"
#include "host_alloc.h"

#ifdef MALLOC_DEBUG
#define GetPhysicallAddr(a)		(NMF_LOG("Get Physicall at %s %d %x\n",__FILE__,__LINE__,a),VFM_GetPhysical(local_vfmmemory_ctxt, (t_uint8 *)a))
#define GetLogicalAddr(a)		(NMF_LOG("Get Logicall at %s %d %x\n",__FILE__,__LINE__,a),VFM_GetLogical(local_vfmmemory_ctxt, (t_uint8 *)a))
#else
#define GetPhysicallAddr(a)		VFM_GetPhysical(local_vfmmemory_ctxt, (t_uint8 *)a)
#define GetLogicalAddr(a)		VFM_GetLogical(local_vfmmemory_ctxt, (t_uint8 *)a)
#endif

#define Endianess(a)	((((t_uint32)(a) & 0xffff )<<16) + (((t_uint32)(a)>> 16) & 0xffff ))

// in case of pure SW, no need to convert back and forth logical/physical adresses because we are only in the ARM
// world and we don't have to use any DMA
#ifdef NO_HAMAC
#define CONVERT_TO_AHB_ADDRESS_BE(a)   (t_address) (a)
#define CONVERT_TO_MTF_AHB_ADDRESS(a)   (t_address) (a)
#define UNCONVERT_TO_MTF_AHB_ADDRESS(a)   (t_address) (a)
#define MTF_MARKER(a) a 
#define UNMARK_MTF(a) a
#else
#define CONVERT_TO_AHB_ADDRESS_BE(a)   		(t_address) Endianess(GetPhysicallAddr(a))
#define CONVERT_TO_MTF_AHB_ADDRESS(a)   	(t_address) Endianess(MTF_MARKER(GetPhysicallAddr(a)))
#define UNCONVERT_TO_MTF_AHB_ADDRESS(a)   	(t_address) GetLogicalAddr( UNMARK_MTF(Endianess((t_uint32) (a))))
#define MTF_MARKER(a)	((t_uint32) (a)	| 1 )
#define UNMARK_MTF(a)	((t_uint32) (a)	&  0xFFFFFFFE )
#endif

#define HAMACALLOC(a,n)  (a*) HWBUFF_ALLOC((sizeof(a)*n), 8-1)


void DisplayParam(t_uint16 nslices, t_hamac_param	*t);

/** \brief This structure define an output frame buffer. */
void	HamacFillFrameBuffIn(void *local_vfmmemory_ctxt,ts_t1xhv_vdc_frame_buf_in * p, t_sva_buffers * buff)
{
    
    
   	p->addr_fwd_ref_buffer= CONVERT_TO_AHB_ADDRESS_BE(buff->fwd_ref_frame);  /**<\brief Address of Forward reference buffer. used for    tranmitting refframe fro conceament ??? */
    p->addr_bwd_ref_buffer= CONVERT_TO_AHB_ADDRESS_BE(buff->bwd_ref_frame);  /**<\brief Address of backward reference buffer */
    //p->reserved_2=0;           /**<\brief Reserved 32 */
    //p->reserved_3=0;           /**<\brief Reserved 32                          */
}


/** \brief This structure define an output frame buffer. */
void	HamacFillFrameBuffOut(void *local_vfmmemory_ctxt,ts_t1xhv_vdc_frame_buf_out *p, t_sva_buffers * buff)
{
	/**<\brief Address of output frame buffer. Y  */
  	p->addr_dest_buffer             =CONVERT_TO_AHB_ADDRESS_BE(buff->curr_frame);	
    
	// To store intermediate param needed for Deblocking , one buffer per picture
	p->addr_deblocking_param_buffer =CONVERT_TO_AHB_ADDRESS_BE(buff->p_deblocking_paramv);	

	// NOT USED
	p->addr_motion_vector_buffer   	=0; 	/**<\brief Start add of motion vector buffer */
  	p->addr_jpeg_coef_buffer       	=0; 	/**<\brief Start address of JPEG Coef buffer */
  	p->addr_jpeg_line_buffer        =0;	/**<\brief Start address of JPEG line buffer */

	// Reconstructed Local Buffer
#ifdef __ndk8500_a0__
	p->addr_dest_local_rec_buffer = (t_uint32) GetPhysicallAddr((t_ahb_address)buff->local_recon_buff);
	p->addr_dest_local_rec_buffer= (p->addr_dest_local_rec_buffer + 255)& 0xFFFFFF00; // ensure alignment to 256 bytes 
	p->addr_dest_local_rec_buffer = Endianess(p->addr_dest_local_rec_buffer);
#else
    	p->addr_dest_local_rec_buffer   =0;	
#endif

	// NOT USED
    	p->addr_dest_buffer_deblock     =0;	/**<\brief Destination buffer start address for deblocking */

}


  
void	HamacFillInternals(void *local_vfmmemory_ctxt,ts_t1xhv_vdc_internal_buf *p, t_sva_buffers * buff)
{
  // VPP Dummy	
	p->addr_vpp_dummy_buffer=CONVERT_TO_AHB_ADDRESS_BE(buff->aux_frame);

  // DMA CUP Context
	p->addr_h264d_block_info=CONVERT_TO_AHB_ADDRESS_BE(buff->cup_context);    

  // NOT USED
	p->addr_h264d_mb_slice_map=0;       

	p->addr_mv_history_buffer=CONVERT_TO_AHB_ADDRESS_BE(buff->p_mv_history_buf);  
}	
	
			
	


void	HamacFillBistreamIn(void *local_vfmmemory_ctxt,ts_t1xhv_bitstream_buf_pos *p, t_sva_buffers * buff)
{
#ifdef NO_HAMAC

    p-> addr_bitstream_start = (t_ahb_address)buff->curr_bitstream->addr;      /**<\brief Bitstream Start add. inside buffer */
    p-> bitstream_offset=buff->curr_bitstream->os;          /**<\brief Bitstream offset in bits           */

    ((ts_t1xhv_bitstream_buf*)(p-> addr_bitstream_buf_struct)) -> addr_buffer_start =  (t_ahb_address)buff->curr_bitstream->addr;
    ((ts_t1xhv_bitstream_buf*)(p-> addr_bitstream_buf_struct)) -> addr_buffer_end =  (t_ahb_address)buff->curr_bitstream->end;
    ((ts_t1xhv_bitstream_buf*)(p-> addr_bitstream_buf_struct)) -> addr_window_start =  (t_ahb_address)buff->curr_bitstream->addr;
    ((ts_t1xhv_bitstream_buf*)(p-> addr_bitstream_buf_struct)) -> addr_window_end =  (t_ahb_address)buff->curr_bitstream->end;

#else
	t_uint32	start_addr, aligned_addr, end_addr;		
        //+ change for ER 336257 
	tps_t1xhv_bitstream_buf_link node1;
    	//- change for ER 336257 

  	// Start address must be aligned on 16 bytes	
	start_addr = (t_uint32) GetPhysicallAddr((t_ahb_address)buff->curr_bitstream->addr);
	aligned_addr= start_addr & 0xFFFFFFF0;
	
	p->addr_bitstream_start = Endianess(aligned_addr);
	
       //+ change for ER 336257 
	node1 = ((ts_t1xhv_bitstream_buf_link*)(p-> addr_bitstream_buf_struct));
	node1->addr_next_buf_link = (t_ahb_address)node1;
        //- change for ER 336257 
  	
	// Then we compute the offset to compensate this alignement , offset in bits
   	p->bitstream_offset = Endianess(8 * (start_addr  - aligned_addr));

	// End address must be added with a provision due to pipe implementation of the DMA
	end_addr = (t_uint32)GetPhysicallAddr((t_ahb_address)buff->curr_bitstream->end) + 512;
	
        //+ change for ER 336257 
	node1->addr_prev_buf_link = 0;
	node1->addr_buffer_start = p->addr_bitstream_start;
	node1->addr_buffer_end = Endianess(end_addr);
	//- change for ER 336257 

#endif

    //p-> reserved_1=0;                /**<\brief Reserved 32                        */

}

void HamacFillParameterIn(void *local_vfmmemory_ctxt,ts_t1xhv_vdc_vc1_param_in_common *p,ts_sequence_parameters *seq_params,t_uint16 pic_width,t_uint16 pic_height)
{
	p->profile      		= seq_params->profile;  
	p->level        		= seq_params->level;    
    	p->quantizer    		= seq_params->quantizer;                  
    	p->dquant       		= seq_params->dquant;                      
    	p->max_b_frames 		= seq_params->max_b_frames;                  
    	p->q_framerate_for_postproc 	= seq_params->q_framerate_for_postproc;       
    	p->q_bitrate_for_postproc 	= seq_params->q_bitrate_for_postproc;        
	p->loop_filter_enabled 		= seq_params->loop_filter_enabled;                
    	p->multires_coding_enabled 	= seq_params->multires_coding_enabled;            
   	p->fast_uvmc_enabled 		= seq_params->fast_uvmc_enabled;                  
    	p->extended_mv_enabled 		= seq_params->extended_mv_enabled;                
    	p->variable_size_transform_enabled 	= seq_params->variable_size_transform_enabled;    
    	p->overlap_transform_enabled 	= seq_params->overlap_transform_enabled;    
    	p->syncmarker_enabled 		= seq_params->syncmarker_enabled;             
    	p->rangered_enabled 		= seq_params->rangered_enabled;               
    	p->frame_interpolation_enabled 	= seq_params->frame_interpolation_enabled; 
    	p->max_picture_width 		= pic_width;
    	p->max_picture_height 		= pic_height;
}

/****************************************************************************/
/**
 * \brief 	This is called only once when initializing the decoder, it is 
 *            called N times, where N is the HAMAC PIPE depth. It allocates
 *            the substructures of ts_t1xhv_vdc_subtask_param  
 * 
 **/
/****************************************************************************/
void	HamacAllocParameter(void *vfm_memory_ctxt,t_hamac_param *t,t_uint16 n)
{
	// Input Frames (backward/forward references)
#ifdef VERBOSE_ALLOC
	NMF_LOG("Allocating in frame buff in struct\n");
#endif
	t->addr_in_frame_buffer		= HAMACALLOC(ts_t1xhv_vdc_frame_buf_in,1);    // MTF 
	
	// Output Frame (destination frame)
#ifdef VERBOSE_ALLOC
	NMF_LOG("Allocating frame buff out struct\n");
#endif
	t->addr_out_frame_buffer	= HAMACALLOC(ts_t1xhv_vdc_frame_buf_out,1); 	// MTF 

	// Internal Buffers (eg vpp dummy, cup context, ...)
#ifdef VERBOSE_ALLOC
	NMF_LOG("Allocating intenal buff struct \n");
#endif
	t->addr_internal_buffer		= HAMACALLOC(ts_t1xhv_vdc_internal_buf,1);  // MTF

	// bitstream in
#ifdef VERBOSE_ALLOC
	NMF_LOG("Allocating bitstream buf struct");
#endif
	t->addr_in_bitstream_buffer	= HAMACALLOC(ts_t1xhv_bitstream_buf_pos,1);// MTF 
       //+ change for ER 336257 
	t->addr_in_bitstream_buffer->addr_bitstream_buf_struct =(t_uint32)  HAMACALLOC(ts_t1xhv_bitstream_buf_link,1);//NO MTF
       //- change for ER 336257 
	// bistream out	-> unused for VC1 codec
	/*
#ifdef VERBOSE_ALLOC
	NMF_LOG("Allocating in buff pos struct\n");
#endif
	t->addr_out_bitstream_buffer= HAMACALLOC(ts_t1xhv_bitstream_buf_pos,1); // MTF
	t->addr_out_bitstream_buffer->addr_bitstream_buf_struct =(t_uint32)  HAMACALLOC(ts_t1xhv_bitstream_buf,1);//NO MTF
	*/
	
	// Input main decoder params -> unused for VC1 codec
	/*
#ifdef VERBOSE_ALLOC
	NMF_LOG("Allocating in param in struct \n");
#endif
	t->addr_in_parameters		= HAMACALLOC(ts_t1xhv_vdc_vc1_param_in,1); // MTF 
	*/
	
	// Output decoder params
#ifdef VERBOSE_ALLOC
	NMF_LOG("Allocating param out struct\n");
#endif
	t->addr_out_parameters		= HAMACALLOC(ts_t1xhv_vdc_vc1_param_out,1); // MTF

	// Input / Ouput parameters
	// -> allocated only once at init (not for each frame)
	/*
#ifdef VERBOSE_ALLOC
	NMF_LOG("Allocating param inout in struct\n");
#endif
	t->addr_in_frame_parameters	= HAMACALLOC(ts_t1xhv_vdc_vc1_param_inout,1); // MTF

#ifdef VERBOSE_ALLOC
	NMF_LOG("Allocating param inout out struct\n");
#endif
	t->addr_out_frame_parameters	= HAMACALLOC(ts_t1xhv_vdc_vc1_param_inout,1); // MTF
	*/
}


void	HamacAllocParameterIn(void *vfm_memory_ctxt,ts_t1xhv_vdc_vc1_param_in_common **p)
{
	// input decoder params
	*p = HAMACALLOC(ts_t1xhv_vdc_vc1_param_in_common,1);    // MTF 
}
	
/****************************************************************************/
/**
 * \brief 	This is called only once when destroying the decoder. It frees
 *            the substructures of ts_t1xhv_vdc_subtask_param  
 * 
 **/
/****************************************************************************/
void	HamacDeallocParameter(void *vfm_memory_ctxt,t_hamac_param *t, t_uint16 n)
{
 	FREE(t->addr_in_frame_buffer);
	FREE(t->addr_out_frame_buffer); 	 

	FREE(t->addr_internal_buffer);  

	// bistream in
  FREE((void*)(t->addr_in_bitstream_buffer->addr_bitstream_buf_struct));
	FREE(t->addr_in_bitstream_buffer); 

	// bistream out
  	//free((void*)(t->addr_out_bitstream_buffer->addr_bitstream_buf_struct));
	//free(t->addr_out_bitstream_buffer);

	// Input main decoder params
	//free(t->addr_in_parameters);  

	// Output decoder params
	FREE(t->addr_out_parameters); 
	
	// inout
	//free(t->addr_in_frame_parameters);
	//free(t->addr_out_frame_parameters);
}

void	HamacDeallocParameterIn(void *vfm_memory_ctxt,ts_t1xhv_vdc_vc1_param_in_common *p)
{	
	// Input decoder params
	FREE(p);  
}

#if VERBOSE_PARAM

void DisplayFrameBuffIn(ts_t1xhv_vdc_frame_buf_in *p)
{
	NMF_LOG("HOST:addr_fwd_ref_buffer : %x\n",p->addr_fwd_ref_buffer);
	NMF_LOG("HOST:addr_bwd_ref_buffer : %x \n",p->addr_bwd_ref_buffer);
}
void DisplayFrameBuffOut(ts_t1xhv_vdc_frame_buf_out *p)
{
    NMF_LOG("HOST:   addr_dest_buffer			 :%x\n",p->addr_dest_buffer);             
    NMF_LOG("HOST:   addr_deblocking_param_buffer :%x\n",p->addr_deblocking_param_buffer);
    NMF_LOG("HOST:   addr_motion_vector_buffer    :%x\n",p->addr_motion_vector_buffer);
    NMF_LOG("HOST:   addr_jpeg_coef_buffer        :%x\n",p->addr_jpeg_coef_buffer);
    NMF_LOG("HOST:   addr_jpeg_line_buffer        :%x\n",p->addr_jpeg_line_buffer);
}

void DisplayInternalBuff(ts_t1xhv_vdc_internal_buf  *p)
{
    NMF_LOG("HOST:  addr_vpp_dummy_buffer  :%x\n",p->addr_vpp_dummy_buffer);
    NMF_LOG("HOST:  addr_h264d_block_info  :%x\n",p->addr_h264d_block_info);
    NMF_LOG("HOST:  addr_h264d_mb_slice_ma :%x\n",p->addr_h264d_mb_slice_map);
    NMF_LOG("HOST:  addr_mv_history_buffer :%x\n",p->addr_mv_history_buffer);
}

 


void DisplayParam(t_uint16 nslices, t_hamac_param	*t)
{
	DisplayFrameBuffIn(t->addr_in_frame_buffer);
	DisplayFrameBuffOut( t->addr_out_frame_buffer);
	DisplayInternalBuff(t->addr_internal_buffer);
}
#endif


/****************************************************************************/
/**
 * \brief 	This is called for each frame, in order to fill ts_t1xhv_vdc_subtask_param
 *            and its sub-structures 
 * 
 **/
/****************************************************************************/
void HamacFillParamIn(void *local_vfmmemory_ctxt,t_dec *dec, t_uint16 nslices,t_hamac_param	*t)
{ 
#if VERBOSE_STANDARD == 1
	NMF_LOG("NMF-ARM decoder: call to HamacFillParamIn()\n");
#endif	
	HamacFillFrameBuffIn(local_vfmmemory_ctxt,t->addr_in_frame_buffer,&dec->sva_buffers);
	HamacFillFrameBuffOut(local_vfmmemory_ctxt,t->addr_out_frame_buffer,&dec->sva_buffers);
	HamacFillInternals(local_vfmmemory_ctxt,t->addr_internal_buffer,&dec->sva_buffers);
	HamacFillBistreamIn(local_vfmmemory_ctxt,t->addr_in_bitstream_buffer,&dec->sva_buffers);

	// Input / Ouput parameters
	t->addr_in_frame_parameters = (ts_t1xhv_vdc_vc1_param_inout *)(dec->sva_buffers.p_param_inout);
	t->addr_out_frame_parameters = t->addr_in_frame_parameters;

#if VERBOSE_PARAM 
    DisplayParam(nslices, t);
#endif
}

/****************************************************************************/
/**
 * \brief 	This is called for each frame, just before calling Hamac part, it converts
 *           the pointers to substructures in  ts_t1xhv_vdc_subtask_param to physical
 *           addresses usable by SVA DMA  
 * 
 **/
/****************************************************************************/
void HamacToPhysicalAddresses(void *local_vfmmemory_ctxt,t_hamac_param *t, t_uint16 n)
{
#if VERBOSE_STANDARD == 1
	NMF_LOG("NMF-ARM decoder: call to HamacToPhysicalAddresses()\n");
#endif
	// NO Endianess is needed for pointers above , because NMF marshalling take care of this conversion.
	// buff input
	t->addr_in_frame_buffer = 	(ts_t1xhv_vdc_frame_buf_in *)MTF_MARKER(GetPhysicallAddr(t->addr_in_frame_buffer));
    	// buff output
	t->addr_out_frame_buffer = (ts_t1xhv_vdc_frame_buf_out *)MTF_MARKER(GetPhysicallAddr(t->addr_out_frame_buffer));
	// internal buffer
	t->addr_internal_buffer = (ts_t1xhv_vdc_internal_buf *)MTF_MARKER(GetPhysicallAddr(t->addr_internal_buffer));
	// bistream in
  t->addr_in_bitstream_buffer->addr_bitstream_buf_struct =CONVERT_TO_MTF_AHB_ADDRESS(t->addr_in_bitstream_buffer->addr_bitstream_buf_struct);
	t->addr_in_bitstream_buffer = (ts_t1xhv_bitstream_buf_pos *)MTF_MARKER(GetPhysicallAddr(t->addr_in_bitstream_buffer));

	// bistream out	-> unused for VC1 codec
//  t->addr_out_bitstream_buffer->addr_bitstream_buf_struct =CONVERT_TO_MTF_AHB_ADDRESS(t->addr_out_bitstream_buffer->addr_bitstream_buf_struct);
	//t->addr_out_bitstream_buffer = (ts_t1xhv_bitstream_buf_pos *)MTF_MARKER(GetPhysicallAddr(t->addr_out_bitstream_buffer));

	// Input main decoder params -> unused for VC1 codec
	//t->addr_in_parameters = (ts_t1xhv_vdc_vc1_param_in *)MTF_MARKER(GetPhysicallAddr(t->addr_in_parameters));
	// Output decoder params
	t->addr_out_parameters = (ts_t1xhv_vdc_vc1_param_out *)MTF_MARKER(GetPhysicallAddr(t->addr_out_parameters));
	// INOUT Input main decoder params
	t->addr_in_frame_parameters = (ts_t1xhv_vdc_vc1_param_inout *)MTF_MARKER(GetPhysicallAddr(t->addr_in_frame_parameters));
	// INOUT Output decoder params
	t->addr_out_frame_parameters = (ts_t1xhv_vdc_vc1_param_inout *)MTF_MARKER(GetPhysicallAddr(t->addr_out_frame_parameters));
}

void HamacToPhysicalAddresses(void *local_vfmmemory_ctxt,ts_t1xhv_vdc_vc1_param_in_common **p)
{
	// input decoder params
	*p = (ts_t1xhv_vdc_vc1_param_in_common *)MTF_MARKER(GetPhysicallAddr(*p));
}

/****************************************************************************/
/**
 * \brief 	This is called for each frame, just after Hamac part is finished, it restores
 *           the pointers to substructures in  ts_t1xhv_vdc_subtask_param to logical 
 *           addresses usable by ARM
 * 
 **/
/****************************************************************************/
void HamacToLogicalAddresses(void *local_vfmmemory_ctxt,t_hamac_param *t, t_uint16 n)
{
#if VERBOSE_STANDARD == 1
	NMF_LOG("NMF-ARM decoder: call to HamacToLogicalAddresses()\n");
#endif
	// buff input
	t->addr_in_frame_buffer = (ts_t1xhv_vdc_frame_buf_in *)VFM_GetLogical(local_vfmmemory_ctxt, (t_uint8 *)UNMARK_MTF(t->addr_in_frame_buffer));
	// buff output
	t->addr_out_frame_buffer = (ts_t1xhv_vdc_frame_buf_out *)VFM_GetLogical(local_vfmmemory_ctxt, (t_uint8 *)UNMARK_MTF(t->addr_out_frame_buffer));

	// internal buffer
	t->addr_internal_buffer = (ts_t1xhv_vdc_internal_buf *)VFM_GetLogical(local_vfmmemory_ctxt, (t_uint8 *)UNMARK_MTF(t->addr_internal_buffer));

	// bistream in
	t->addr_in_bitstream_buffer = (ts_t1xhv_bitstream_buf_pos *)VFM_GetLogical(local_vfmmemory_ctxt, (t_uint8 *)UNMARK_MTF(t->addr_in_bitstream_buffer));
	t->addr_in_bitstream_buffer->addr_bitstream_buf_struct = UNCONVERT_TO_MTF_AHB_ADDRESS(t->addr_in_bitstream_buffer->addr_bitstream_buf_struct);

	// bitstream out => unused for VC1
	//t->addr_out_bitstream_buffer = (ts_t1xhv_bitstream_buf_pos *)VFM_GetLogical(local_vfmmemory_ctxt, (t_uint8 *)UNMARK_MTF(t->addr_out_bitstream_buffer));
	//t->addr_out_bitstream_buffer->addr_bitstream_buf_struct = UNCONVERT_TO_MTF_AHB_ADDRESS(t->addr_out_bitstream_buffer->addr_bitstream_buf_struct);

	// Input main decoder params => unused for VC1
	//t->addr_in_parameters = (ts_t1xhv_vdc_vc1_param_in *)VFM_GetLogical(local_vfmmemory_ctxt, (t_uint8 *)UNMARK_MTF(t->addr_in_parameters));

	// Output decoder params
	t->addr_out_parameters = (ts_t1xhv_vdc_vc1_param_out *)VFM_GetLogical(local_vfmmemory_ctxt, (t_uint8 *)UNMARK_MTF(t->addr_out_parameters));

	// INOUT Input main decoder params
	t->addr_in_frame_parameters = (ts_t1xhv_vdc_vc1_param_inout *)VFM_GetLogical(local_vfmmemory_ctxt, (t_uint8 *)UNMARK_MTF(t->addr_in_frame_parameters));
	// INOUT Output decoder params
	t->addr_out_frame_parameters = (ts_t1xhv_vdc_vc1_param_inout *)VFM_GetLogical(local_vfmmemory_ctxt, (t_uint8 *)UNMARK_MTF(t->addr_out_frame_parameters));
}
