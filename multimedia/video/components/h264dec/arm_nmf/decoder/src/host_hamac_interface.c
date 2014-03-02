/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifdef NMF_BUILD
#include <h264dec/arm_nmf/decoder.nmf>
#endif

#include "settings.h"
#include "types.h"
#include "host_types.h"
#include "hamac_types.h"
#include "host_decoder.h"
#include "local_alloc.h"
#include "ost_event.h"

// force Logical @ = Physicall @ in think environement
//#define NO_MMU
#define OPTIMIZED_PARAM_POOL
#ifdef MALLOC_DEBUG
#define GetPhysicallAddr(a)		(printf("Get Physicall at %s %d L %p %p\n",__FILE__,__LINE__,a,VFM_GetPhysical(local_vfmmemory_ctxt, (t_uint8 *)a)),VFM_GetPhysical(local_vfmmemory_ctxt, (t_uint8 *)a))
#define GetLogicalAddr(a)		(printf("Get Logicall at %s %d %p L %p\n",__FILE__,__LINE__,a,VFM_GetLogical(local_vfmmemory_ctxt, (t_uint8 *)a)),VFM_GetLogical(local_vfmmemory_ctxt, (t_uint8 *)a))
#else

#ifdef NO_MMU

int __think_memory__flat_mapping = 1;
#define GetPhysicallAddr(a)		 (t_uint8 *)a
#define GetLogicalAddr(a)		(t_uint8 *)a

#else

#define GetPhysicallAddr(a)		VFM_GetPhysical(local_vfmmemory_ctxt, (t_uint8 *)a)
#define GetLogicalAddr(a)		VFM_GetLogical(local_vfmmemory_ctxt, (t_uint8 *)a)

#endif

#endif

#ifndef __ndk5500_a0__
#define Endianess(a)	((((t_uint32)(a) & 0xffff )<<16) + (((t_uint32)(a)>> 16) & 0xffff ))
#else
#define Endianess(a)	(a)
#endif

#ifdef NO_HAMAC
#define CONVERT_TO_AHB_ADDRESS_BE(a)   (t_address) (a)
#else
#define CONVERT_TO_AHB_ADDRESS_BE(a)   (t_address) Endianess(GetPhysicallAddr(a))
#endif




void DisplayParam(t_uint16 nslices, t_hamac_param	*t);

/** \brief This structure define an output frame buffer. */
void	HamacFillFrameBuffIn(void *local_vfmmemory_ctxt,ts_t1xhv_vdc_frame_buf_in * p,t_host_info *p_host)
{
// +ER372487
    if(p_host->buff->ref_frame==0)   
      p->addr_fwd_ref_buffer=CONVERT_TO_AHB_ADDRESS_BE(p_host->buff->curr_frame[0]);
	else
        p->addr_fwd_ref_buffer= CONVERT_TO_AHB_ADDRESS_BE(p_host->buff->ref_frame);  /**<\brief Address of Forward reference buffer. used for    tranmitting refframe fro conceament ??? */
// -ER372487
    p->addr_bwd_ref_buffer=0; /**<\brief Address of backward reference buffer */
    p->addr_source_buffer=0;                /**<\brief Source buffer start address for deblocking */
    p->addr_deblocking_param_buffer_in=0;   /**<\brief Deblocking parameters will used  as a input of deblocking */
}


/** \brief This structure define an output frame buffer. */
void	HamacFillFrameBuffOut(void *local_vfmmemory_ctxt,ts_t1xhv_vdc_frame_buf_out *p,t_host_info *p_host)
{
    /**<\brief Address of output frame buffer. Y  */
    p->addr_dest_buffer             =CONVERT_TO_AHB_ADDRESS_BE(p_host->buff->curr_frame[0]);	
    // To store intermediate param needed for Deblocking , one buffer per picture
    p->addr_deblocking_param_buffer =CONVERT_TO_AHB_ADDRESS_BE(p_host->buff->p_deblocking_paramv);	

    // NOT USED
    p->addr_motion_vector_buffer   	=0; /**<\brief Start add of motion vector buffer */
    p->addr_jpeg_coef_buffer       	=0; /**<\brief Start address of JPEG Coef buffer */
    p->addr_jpeg_line_buffer        =0;	/**<\brief Start address of JPEG line buffer */
#ifdef __ndk8500_a0__ 
    p->addr_dest_local_rec_buffer   =CONVERT_TO_AHB_ADDRESS_BE(p_host->buff->local_recon_buff);
#else
#ifdef __ndk5500_a0__
    p->addr_dest_local_rec_buffer   = (t_ahb_address)p_host->buff->local_recon_buff;
#else
    p->addr_dest_local_rec_buffer   =0;	
#endif
#endif
    p->addr_dest_buffer_deblock     =0;	/**<\brief Destination buffer start address for deblocking */
    /*p->reserved_3                   =0;*/	/**<\brief Reserved 32                       */
}

/**
 *
 */
void	HamacFillInternals(void *local_vfmmemory_ctxt,ts_t1xhv_vdc_internal_buf *p,t_host_info *p_host)
{
    // VPP DUMMY BUFFER
  if(p_host->buff->HED_used==0)
  {
	 p->addr_vpp_dummy_buffer=CONVERT_TO_AHB_ADDRESS_BE(p_host->buff->aux_frame);      //DDR because no RLCB
  }
  else
  {
#ifdef __ndk5500_a0__
      p->addr_vpp_dummy_buffer = (t_ahb_address)p_host->buff->aux_frame2;    // ESRAM because RLCB
#else
      p->addr_vpp_dummy_buffer=CONVERT_TO_AHB_ADDRESS_BE(p_host->buff->aux_frame2);    // ESRAM because RLCB
#endif
  }

#if !((defined __ndk8500_a0__) || (defined __ndk5500_a0__))
  // Block info , not used in Base line ?
  p->addr_h264d_block_info=CONVERT_TO_AHB_ADDRESS_BE(p_host->buff->b_info);    
#else
  p->addr_h264d_block_info = 0; /* unused */
#endif

    // Slice map
    p->addr_h264d_mb_slice_map=CONVERT_TO_AHB_ADDRESS_BE(p_host->buff->mb_slice_map);       

    // FIXME Not used by the HArdware , but tricks to pass a parameter on the slice hearder
    // Code must be rewritten to avoid use of this structure in the Deblocking !!!
    p->addr_mv_history_buffer=(t_address) p_host->sh;  
}	

/**
 *
 */
void	HamacFillBistreamIn(void *local_vfmmemory_ctxt,ts_t1xhv_bitstream_buf_pos *p, t_slice_hdr *sh, t_slice_hdr * sh_last) 
{
#if ((defined __ndk8500_a0__) || (defined __ndk5500_a0__))
    t_uint32 start_addr, end_addr;
    t_uint8 * tmp_sh_last_next = (t_uint8*)sh_last->next;

    // to handle correctly last buffer (see patch in ddep)
    if((*(tmp_sh_last_next-1))!=0)
    {
        tmp_sh_last_next++;
    }

    // detect END of Sequence NALU and remove it (otherwise HED does ERC on it...)
    /* + Change start for ER 419506.. the third && condition */
    if((*(tmp_sh_last_next-4)==0)
            && (*(tmp_sh_last_next-3)==0) 
            && (*(tmp_sh_last_next-2)==1)
	    && ((*(tmp_sh_last_next-1)==0xa) || (*(tmp_sh_last_next-1)==0xc) || (*(tmp_sh_last_next-1)==0x9)))
    {
        tmp_sh_last_next-=3;
    }
    /* - Change end for ER 419506 */

    start_addr = (t_uint32) GetPhysicallAddr((sh->p_bsh_start.addr - 3));
    end_addr = (t_uint32) GetPhysicallAddr((tmp_sh_last_next-2)); 
        // make the end address point to the last valid byte of last slice

    // *(tmp_sh_last_next-1) = 0; // already done by ddep
    // *(tmp_sh_last_next)   = 0; // already done by ddep
    // *(tmp_sh_last_next+1) = 0;
    // *(tmp_sh_last_next+2) = 0x1;

#if 0
    printf("END [%8x %8x %8x %8x] %8x %8x %8x %8x\n", *(tmp_sh_last_next-5),
            *(tmp_sh_last_next-4),
            *(tmp_sh_last_next-3),
            *(tmp_sh_last_next-2),
            *(tmp_sh_last_next-1),
            *(tmp_sh_last_next),
            *(tmp_sh_last_next+1),
            *(tmp_sh_last_next+2));
#endif

    ((ts_t1xhv_bitstream_buf*)p->addr_bitstream_buf_struct)->addr_buffer_start =
        ((ts_t1xhv_bitstream_buf*)p->addr_bitstream_buf_struct)->addr_window_start = 
        Endianess(start_addr);
    ((ts_t1xhv_bitstream_buf*)p->addr_bitstream_buf_struct)->addr_buffer_end =
        ((ts_t1xhv_bitstream_buf*)p->addr_bitstream_buf_struct)->addr_window_end = 
        Endianess(end_addr);  
#endif
    /*p-> addr_bitstream_buf_struct=0; */ /**<\brief Choose buffer structure            */
    p-> addr_bitstream_start=0;      /**<\brief Bitstream Start add. inside buffer */
    p-> bitstream_offset=0;          /**<\brief Bitstream offset in bits           */
    p-> reserved_1=0;                /**<\brief Reserved 32                        */
}/*End of HamacFillBistreamIn*/

/**
 *Set descriptor for the CABAC intermediate buffer 
 */
void HamacFillBistreamOut(void *local_vfmmemory_ctxt,ts_t1xhv_bitstream_buf_pos *p,t_host_info *p_host)
{
#if ((defined __ndk8500_a0__) || (defined __ndk5500_a0__))
    t_uint32 addr;

    addr = (t_uint32) GetPhysicallAddr(p_host->buff->bits_intermediate_buffer);

    /*addr_bitstream_start should be 32-byte aligned*/
    p->addr_bitstream_start = Endianess(addr + 8160*8);
    p-> bitstream_offset=0; 

    ((ts_t1xhv_bitstream_buf*)p->addr_bitstream_buf_struct)->addr_buffer_start =
        ((ts_t1xhv_bitstream_buf*)p->addr_bitstream_buf_struct)->addr_window_start 
        = Endianess(addr);
    ((ts_t1xhv_bitstream_buf*)p->addr_bitstream_buf_struct)->addr_buffer_end =
        ((ts_t1xhv_bitstream_buf*)p->addr_bitstream_buf_struct)->addr_window_end  
        = Endianess(addr +  (276 * 120 * 68) + (8160*8));
#endif
}


void HamacFillBitstreamParameter(void *local_vfmmemory_ctxt,ts_t1xhv_vdc_h264_slice *p, 
 ts_t1xhv_bitstream_buf *b,t_slice_hdr *sh,t_uint16 last)
{
    t_uint32 	bitstream_size;
    bitstream_size=(sh->next-(t_address)sh->p_bsh_end.addr);
#ifdef NO_HAMAC
    p->bitstream_size_in_bytes=  bitstream_size;

    p->addr_bitstream_start= (t_address)(sh->p_bsh_end.addr);

    p->bitstream_offset=sh->p_bsh_end.os;

    b->addr_buffer_start= b->addr_window_start= p->addr_bitstream_start;
    b->addr_buffer_end   = b->addr_window_end   = p->addr_bitstream_start+bitstream_size;
#else
    t_uint32	aligned_addr,start_addr;
    t_uint32 	byte_offset,bit_offset;

    p->bitstream_size_in_bytes= Endianess( bitstream_size);

    // Start address must be aligned on 16 bytes	
    start_addr = (t_uint32) GetPhysicallAddr(sh->p_bsh_end.addr);
    aligned_addr= start_addr & 0xFFFFFFF0;
    p->addr_bitstream_start=Endianess(aligned_addr);


    // Then we compute the offset to compensate this alignement , offset in bits
    // Bitstream offset (in bits) is the offset between 16byte-aligned address and slice data first bit 
    byte_offset = start_addr  - aligned_addr;
    bit_offset        = 8 * byte_offset + (8 - sh->p_bsh_end.os);
    p->bitstream_offset=Endianess(bit_offset);
#ifndef __ndk5500_a0__
    b->addr_buffer_start= b->addr_window_start = Endianess(start_addr);

    // MC: I have put a larger margin on v1 because at least in the TLM the BDU input FIFO is huge
    b->addr_buffer_end = b->addr_window_end = Endianess(start_addr + bitstream_size+512*4); 
#endif
#endif  /*NO_HAMAC*/
} /*End of HamacFillBitstreamParameter*/





void HamacFillSliceParameterIn(void *local_vfmmemory_ctxt,ts_t1xhv_vdc_h264_slice *p,
                                t_host_info *p_host, t_dec_buff *p_buff,t_uint16 last)
{
    t_uint16 i;
    t_sint32 l;
    ts_t1xhv_vdc_h264_weight_list * loc_weight_list;
    t_uint32 start_bit_offset, end_bit_offset;
    ts_t1xhv_bitstream_buf	*b;

    p->discarded_slice=p_host->sh->error;   /**<\brief flag set by host for discarded slice */

    t_uint32 cabac_alignment_one_bit_nb = 0;
    t_uint8 mask_last_slice_header_byte = 0x00;
    t_uint8 *p_last_slice_header_byte;

    /* Process cabac_alignment_one_bit erroneous bits fixing */
    /* = set cabac_alignment_one_bit number to 0     */
    cabac_alignment_one_bit_nb = 0;
    /* + check whether current entropy mode is CABAC */
    if   (1 == p_host->pp->entropy_mode_flag)
    {
        /*   Compute cabac_alignment_one_bit number
         *   = set it to remaining bits in slice header last byte */
        cabac_alignment_one_bit_nb =p_host->sh->p_bsh_end.os;
        /*   then if not null, patch last slice header byte to set these bits to 1
         *       when they are not */
        mask_last_slice_header_byte = (0xFF >> (8-cabac_alignment_one_bit_nb));
        p_last_slice_header_byte = (t_uint8 *)(p_host->sh->p_bsh_end.addr);
        if (mask_last_slice_header_byte != (mask_last_slice_header_byte & *p_last_slice_header_byte))
        {
            *p_last_slice_header_byte |= mask_last_slice_header_byte;
        }
    }

    /* Set offsets (in [0;8]) according to unread remaining bits (in [0;8])*/
    start_bit_offset = 8 - p_host->sh->p_bsh_start.os;
    end_bit_offset   = 8 - p_host->sh->p_bsh_end.os;

    /* used for data flow with HED: this is the size of the slice header WITHOUT counting
       the anti-emulation bytes, it is needed by the FW to skip the parsing of the slice
       header in the intermediate buffer */
    p->slice_header_bit_offset
        = (p_host->sh->p_bsh_end.addr - p_host->sh->p_bsh_start.addr) *8 
        + (end_bit_offset - start_bit_offset)
        - (p_host->sh->aeb_counter*8);


    /* set slice info parameters according to corresponding slice header */
    p->first_mb_in_slice     = p_host->sh->first_mb_in_slice;
    p->slice_qp              = p_host->sh->slice_qp;
    p->slice_type            = p_host->sh->slice_type;
    p->slice_num             = p_host->sh->slice_num;
    p->s_info_disable_filter = p_host->sh->disable_deblocking_filter_idc;
    p->s_info_alpha_c0_offset_div2 = p_host->sh->slice_alpha_c0_offset_div2;
    p->s_info_beta_offset_div2     = p_host->sh->slice_beta_offset_div2;

    if((p_host->sh->slice_type==1)||(p_host->sh->slice_type==6)) 
    {
        p->direct_spatial_mv_pred_flag = p_host->sh->direct_spatial_mv_pred_flag;
    }
    else
        p->direct_spatial_mv_pred_flag = 0;

    if((p_host->sh->slice_type==0)||(p_host->sh->slice_type==5)||
            (p_host->sh->slice_type==1)||(p_host->sh->slice_type==6))
    {
        if(p_host->sh->num_ref_idx_active_override_flag)
        {
            p->num_ref_idx_l0_active_minus1 
                = p_host->sh->num_ref_idx_l0_active_minus1;
        }
        else
        {
            p->num_ref_idx_l0_active_minus1 
                = p_host->pp->num_ref_idx_l0_active_minus1;
        }
    }
    else
        p->num_ref_idx_l0_active_minus1 =0;

    if((p_host->sh->slice_type==1)||(p_host->sh->slice_type==6))
    {
        if(p_host->sh->num_ref_idx_active_override_flag)
        {
            p->num_ref_idx_l1_active_minus1 
                = p_host->sh->num_ref_idx_l1_active_minus1;
        }
        else
        {
            p->num_ref_idx_l1_active_minus1 
                = p_host->pp->num_ref_idx_l1_active_minus1;
        }
    }
    else
        p->num_ref_idx_l1_active_minus1 = 0;

    /* P slices, do only L0 */
    if ((p_host->sh->slice_type==0)||(p_host->sh->slice_type==5))   
    {
        for(l=0;l<16;l++)     
        {
            p->listX_RefIdx2RefPic[l]
                = p_host->sh->pic[l];
        }
    }

    /* B slices, build L0 and L1 */ 
    if((p_host->sh->slice_type==1)||(p_host->sh->slice_type==6))  
    {
        for(l=0;l<16;l++)   
        {
            p->listX_RefIdx2RefPic[l]
                =(p_host->sh->pic[l])|(p_host->sh->pic_l1[l]<<8);
        }   
    }

    p->long_termlist = 0;
    if((p_host->sh->slice_type==1)||(p_host->sh->slice_type==6)||
            (p_host->sh->slice_type==0)||(p_host->sh->slice_type==5))
    {
        for(l=0;l<p_buff->DPBsize+1;l++)   
        {
            p->long_termlist 
              |= (p_buff->pics_buf[l].marked_long<<l);
        } 
    } 

    p->long_termlist = Endianess(p->long_termlist);
#ifndef __ndk5500_a0__
    p->addr_weighted_pred_buffer = 0;
#endif

    /* for WP explicit only */
    if ((((p_host->sh->slice_type==0)||(p_host->sh->slice_type==5)) && p_host->pp->weighted_pred_flag)
            || (((p_host->sh->slice_type==1)||(p_host->sh->slice_type==6))&&p_host->pp->weighted_bipred_idc==1))   
    {
#ifndef __ndk5500_a0__
        p->addr_weighted_pred_buffer = p->reserved32_0; /* address allocated only once in SliceAlloc */
#endif

        loc_weight_list = (ts_t1xhv_vdc_h264_weight_list *)p->addr_weighted_pred_buffer;

        loc_weight_list->luma_log2_weight_denom = p_host->sh->luma_log2_weight_denom;
        loc_weight_list->chroma_log2_weight_denom = p_host->sh->chroma_log2_weight_denom;

        for(i=0;i<16;i++)
        {
            loc_weight_list->weight_l0[i][0] = p_host->sh->luma_weight_l0[i];
            loc_weight_list->offset_l0[i][0] = p_host->sh->luma_offset_l0[i];
            loc_weight_list->weight_l1[i][0] = p_host->sh->luma_weight_l1[i];
            loc_weight_list->offset_l1[i][0] = p_host->sh->luma_offset_l1[i];

            loc_weight_list->weight_l0[i][1] = p_host->sh->chroma_weight_l0[i][0];
            loc_weight_list->offset_l0[i][1] = p_host->sh->chroma_offset_l0[i][0];
            loc_weight_list->weight_l1[i][1] = p_host->sh->chroma_weight_l1[i][0];
            loc_weight_list->offset_l1[i][1] = p_host->sh->chroma_offset_l1[i][0];

            loc_weight_list->weight_l0[i][2] = p_host->sh->chroma_weight_l0[i][1];
            loc_weight_list->offset_l0[i][2] = p_host->sh->chroma_offset_l0[i][1];
            loc_weight_list->weight_l1[i][2] = p_host->sh->chroma_weight_l1[i][1];
            loc_weight_list->offset_l1[i][2] = p_host->sh->chroma_offset_l1[i][1];
        }        
    } 
#ifdef __ndk5500_a0__
    HamacFillBitstreamParameter(local_vfmmemory_ctxt, p, 
            (ts_t1xhv_bitstream_buf *)0, p_host->sh, last);
#else
    b = (ts_t1xhv_bitstream_buf	*)p->addr_bitstream_buf_struct;
    HamacFillBitstreamParameter(local_vfmmemory_ctxt,p,b,p_host->sh,last);
#endif
}/*End of HamacFillSliceParameterIn*/


/**
 *
 */
void HamacFillParameterIn(void *local_vfmmemory_ctxt,ts_t1xhv_vdc_h264_param_in *p,t_host_info *p_host)
{
    t_sint16 i,ii;
    ts_t1xhv_vdc_h264_scaling_list * loc_sc_list;
#ifdef __ndk5500_a0__
    p->ASOFMO_flag = p_host->buff->DBLK_mode;
#else
    p->DBLK_flag=p_host->buff->DBLK_mode;  //p->DBLK_flag = 0;  
#endif
    p->intra_conc=p_host->buff->intra_conc;                  /**<\brief flag for concealment updated in SVA                  */    

    p->pic_width_in_mbs  = p_host->sp->pic_width_in_mbs_minus1+1; 
#ifdef __ndk5500_a0__
    p->pic_height_in_map_units = p_host->sp->pic_height_in_map_units_minus1+1;
#else
    p->pic_height_in_mbs = (2-p_host->sp->frame_mbs_only_flag) * (p_host->sp->pic_height_in_map_units_minus1+1); 
#endif
    p->direct_8x8_inference_flag           = p_host->sp->direct_8x8_inf_flag;  
    p->DPBSize                             = p_host->buff->DPBsize;     
    p->pic_order_cnt_type                  = p_host->sp->pic_order_cnt_type;
    p->delta_pic_order_always_zero_flag    = p_host->sp->delta_pic_order_always_zero_flag;
    p->log2_max_frame_num                  = p_host->sp->log2_max_frame_num_minus4+4;

    if(p_host->sp->pic_order_cnt_type==0)
    {
        p->log2_max_pic_order_cnt_lsb = p_host->sp->log2_max_pic_order_cnt_lsb_minus4+4;
    }
    else
    {
        p->log2_max_pic_order_cnt_lsb = 0;    
    }
    p->frame_mbs_only_flag                 = p_host->sp->frame_mbs_only_flag;
    p->mb_adaptive_frame_field_flag                 = 0;
    p->monochrome                 = 0;

    for(i=0;i<p_host->buff->DPBsize+1;i++)
    {     
        if((p_host->buff->pics_buf[i].poc 
                    == p_host->buff->curr_info->poc)
                &&(p_host->buff->pics_buf[i].frame_num 
                    == p_host->buff->curr_info->frame_num))
        { 
            p->curr_pic_idx = i; 
            break;
        }
    }   

    for(i=0;i<p_host->buff->DPBsize+1;i++)
    {
        p->poc_list[i]      = Endianess(p_host->buff->pics_buf[i].poc);
        if(p_host->buff->pics_buf[i].ptr!=0)    
        {
            p->dpb_addresses[i] = CONVERT_TO_AHB_ADDRESS_BE((t_ahb_address)(p_host->buff->pics_buf[i].ptr[0]));
        }  
        else  
        {
            p->dpb_addresses[i] = 0;
        }

#ifndef NO_HAMAC
#if VERBOSE_CUP_FIFO == 1
        NMF_LOG("DPB[%d] => CUP idx %d\n",i,p_host->buff->pics_buf[i].cup_ctx_idx);
#endif
        if(p_host->buff->pics_buf[i].cup_ctx_idx!=-1)  
        {
            p->addr_cup_ctx[i]  = 
                CONVERT_TO_AHB_ADDRESS_BE((t_ahb_address)(p_host->buff->cup_ctx_desc.cup_context[p_host->buff->pics_buf[i].cup_ctx_idx]));
        } 
        else
            p->addr_cup_ctx[i]  = 0;
#endif    
    }
    p->chroma_qp_index_offset           = p_host->pp->chroma_qp_index_offset;
    p->constr_intra_pred_flag           = p_host->pp->constr_intra_pred_flag;
    p->weighted_pred_flag               = p_host->pp->weighted_pred_flag;
    p->weighted_bipred_idc              = p_host->pp->weighted_bipred_idc;
    p->entropy_coding_mode_flag         = p_host->pp->entropy_mode_flag;
    p->transform_8x8_mode_flag          = p_host->pp->transform_8x8_mode_flag;
    p->num_ref_idx_l0_active_minus1     = p_host->pp->num_ref_idx_l0_active_minus1;
    p->num_ref_idx_l1_active_minus1     = p_host->pp->num_ref_idx_l1_active_minus1;
    p->pic_init_qp                      = p_host->pp->pic_init_qp_minus26+26;
    p->deblocking_filter_control_present_flag = p_host->pp->debl_flt_control_present_flag;
    p->pic_order_present_flag           = p_host->pp->pic_order_present_flag;
    p->second_chroma_qp_index_offset    = p_host->pp->second_chroma_qp_index_offset;

    /* by default set it to NULL */
    p->addr_scaling_matrix = 0;

    if(p_host->sp->seq_scaling_matrix_present_flag)
    {      
        p->addr_scaling_matrix = p->reserved32_0;
        loc_sc_list = (ts_t1xhv_vdc_h264_scaling_list*)p->addr_scaling_matrix;
        for(i=0;i<8;i++)
        {
            if(i<6)
            {
                for(ii=0;ii<16;ii++)
                    loc_sc_list->scaling_matrix_4x4[i][ii] = p_host->sp->seq_scaling_list.ScalingList4x4[i][ii]; 
            }
            else
            {
                for(ii=0;ii<64;ii++)
                    loc_sc_list->scaling_matrix_8x8[i-6][ii] = p_host->sp->seq_scaling_list.ScalingList8x8[i-6][ii];           
            }
        }
    }

    if(p_host->pp->pic_scaling_matrix_present_flag)   
    {      
        p->addr_scaling_matrix = p->reserved32_0;
        loc_sc_list = (ts_t1xhv_vdc_h264_scaling_list*)p->addr_scaling_matrix;
        for(i=0;i<8;i++) 
        {
            if(i<6)    
            {
                for(ii=0;ii<16;ii++)
                    loc_sc_list->scaling_matrix_4x4[i][ii] = p_host->pp->pic_scaling_list.ScalingList4x4[i][ii]; 
            } 
            else  
            {
                for(ii=0;ii<64;ii++)
                    loc_sc_list->scaling_matrix_8x8[i-6][ii] = p_host->pp->pic_scaling_list.ScalingList8x8[i-6][ii];           
            }
        }
    }

#ifdef __ndk5500_a0__
    //TODO  p->HED_used =
#else     
    p->HED_used = p_host->buff->HED_used; 
#endif
}/*End of HamacFillParameterIn*/ 


/**
 * Fill in address fields of HVA task descriptor (should be physical addresses)
 */
void METH(HamacFillTaskDescriptor)(void *local_vfmmemory_ctxt, t_dec *dec, t_hamac_param *t)
{
#ifdef __ndk5500_a0__
    t_uint32 *p;
    int i;
    //    if (dec->buf.HED_used == 3) {
    t->addr_in_parameters->addr_input_bitstream_start = ((ts_t1xhv_bitstream_buf*)t->addr_in_bitstream_buffer->addr_bitstream_buf_struct)->addr_buffer_start;
    t->addr_in_parameters->addr_input_bitstream_end = ((ts_t1xhv_bitstream_buf*)t->addr_in_bitstream_buffer->addr_bitstream_buf_struct)->addr_buffer_end;
    t->addr_in_parameters->addr_ib_sesb_start =  ((ts_t1xhv_bitstream_buf*)t->addr_out_bitstream_buffer->addr_bitstream_buf_struct)->addr_buffer_start;
    t->addr_in_parameters->addr_ib_end = ((ts_t1xhv_bitstream_buf*)t->addr_out_bitstream_buffer->addr_bitstream_buf_struct)->addr_buffer_end;
    //  }
    t->addr_in_parameters->addr_erc_ref_buffer = t->addr_in_frame_buffer->addr_fwd_ref_buffer; 
    t->addr_in_parameters->addr_local_rec_buffer = t->addr_out_frame_buffer->addr_dest_local_rec_buffer;
    t->addr_in_parameters->addr_mb_slice_map = t->addr_internal_buffer->addr_h264d_mb_slice_map; 

    //get physical
    if (t->addr_out_parameters!=NULL) {
        //NMF_LOG("%s: before get physical t->addr_out_parameters:0x%x\n", __func__, t->addr_out_parameters);
        t->addr_in_parameters->addr_param_out = (t_ahb_address) (VFM_GetPhysical(local_vfmmemory_ctxt, (t_uint8 *)(t->addr_out_parameters)));
        //NMF_LOG("%s: after get physical t->addr_in_parameters->addr_param_out:0x%x\n", __func__, t->addr_in_parameters->addr_param_out);
    }
    else  //NMF_LOG("%s: before get physical t->addr_out_parameters: NULL\n", __func__);

    //t->addr_in_parameters->addr_param_out = (t_ahb_address)(t->addr_out_parameters);
    //TODO addr_external_sw+addr_external_sw2 -> mpc_alloc
    t->addr_in_parameters->addr_external_sw = (t_ahb_address)dec->buf.external_sw;
    t->addr_in_parameters->addr_external_sw2 = (t_ahb_address)dec->buf.external_sw2;

    /*TODO remove this #ifdef VERBOSE_PARAM
    NMF_LOG("%s: addr_input_bitstream_start: %p\n", __func__, 
            t->addr_in_parameters->addr_input_bitstream_start);
    NMF_LOG("%s: addr_input_bitstream_end:   %p\n", __func__, 
            t->addr_in_parameters->addr_input_bitstream_end);
    NMF_LOG("%s: addr_ib_sesb_start:         %p\n", __func__, 
            t->addr_in_parameters->addr_ib_sesb_start);
    NMF_LOG("%s: addr_ib_end:                %p\n", __func__, 
            t->addr_in_parameters->addr_ib_end);
    NMF_LOG("%s: addr_erc_ref_buffer:        %p\n", __func__,
            t->addr_in_parameters->addr_erc_ref_buffer);
    NMF_LOG("%s: addr_local_rec_buffer:      %p\n", __func__, 
            t->addr_in_parameters->addr_local_rec_buffer);
    if (t->addr_in_parameters->addr_external_sw)
        NMF_LOG("%s: addr_external_sw:      %p\n", __func__,  
                t->addr_in_parameters->addr_external_sw);
    if (t->addr_in_parameters->addr_external_sw2)
        NMF_LOG("%s: addr_external_sw2:      %p\n", __func__, 
                t->addr_in_parameters->addr_external_sw2);
    NMF_LOG("%s: addr_mb_slice_map:          %p\n", __func__,
            t->addr_in_parameters->addr_mb_slice_map);
    NMF_LOG("%s: addr_param_out:             %p\n", __func__, 
            t->addr_in_parameters->addr_param_out);
    */
     
#endif
}/*End of HamacFillTaskDescriptor*/

#include "VFM_Memory.h"

#undef CONVERT_TO_AHB_ADDRESS_BE
// here we redefine this macro because @ conversion from logical to physicall are optimized here
// There is no need to call the VFM each time


#ifdef NO_HAMAC
#define CONVERT_TO_AHB_ADDRESS_BE(a)   (t_address) (a)
#define CONVERT_TO_MTF_AHB_ADDRESS(a)   (t_address) (a)
#define CONVERT_TO_MTF_DSP_ADDRESS(a)   (t_address) (a)
#define UNCONVERT_TO_MTF_AHB_ADDRESS(a)   (t_address) (a)
#define UNCONVERT_TO_MTF_DSP_ADDRESS(a)   (t_address) (a)
#define MTF_MARKER(a) a 
#define UNMARK_MTF(a) a
#else

#ifdef  OPTIMIZED_PARAM_POOL
#undef GetPhysicallAddr
#undef GetLogicalAddr
#define GetPhysicallAddr(a)			((t_uint8 *)(a)+t->offset_addr)
#define GetLogicalAddr(a)			((t_uint8 *)(a)-t->offset_addr)
#endif

#define CONVERT_TO_AHB_ADDRESS_BE(a)   		(t_address) Endianess(GetPhysicallAddr(a))
#define CONVERT_TO_MTF_AHB_ADDRESS(a)   	(t_address) Endianess(MTF_MARKER(GetPhysicallAddr(a)))
#define CONVERT_TO_MTF_DSP_ADDRESS(a)   	(t_address) (MTF_MARKER(GetPhysicallAddr(a)))
#define UNCONVERT_TO_MTF_AHB_ADDRESS(a)   	(t_address) GetLogicalAddr( UNMARK_MTF(Endianess((t_uint32) (a))))
#define UNCONVERT_TO_MTF_DSP_ADDRESS(a)   	(t_address) GetLogicalAddr( UNMARK_MTF((t_uint32) (a)))
#define MTF_MARKER(a)				((t_uint32) (a)	| 1 )
#define UNMARK_MTF(a)				((t_uint32) (a)	&  0xFFFFFFFE )
#endif

// adress are aligned on 8 bytes
#define ALIGN(a,n)	(((a+n-1)/n)*n)
#define HAMACALLOCSIZE(a,n)  ALIGN(sizeof(a)*n,8)
#define OLD_HAMACALLOC(a,n)  (a*)VFM_Alloc(vfm_memory_ctxt, (sizeof(a)*n), VFM_MEM_HWBUFFER_NONDSP, 8-1, 0, __LINE__, (t_uint8 *)__FILE__)

#ifndef  OPTIMIZED_PARAM_POOL

#ifdef _CACHE_OPT_
#define HAMACALLOC(a,n)  (a*)VFM_Alloc(vfm_memory_ctxt, (sizeof(a)*n), VFM_MEM_CACHED_HWBUFFER_NONDSP, 8-1, 0, __LINE__, (t_uint8 *)__FILE__)
#else		
#define HAMACALLOC(a,n)  (a*)VFM_Alloc(vfm_memory_ctxt, (sizeof(a)*n), VFM_MEM_HWBUFFER_NONDSP, 8-1, 0, __LINE__, (t_uint8 *)__FILE__)
#endif

#else
#define HAMACALLOC(a,n) 	   (a*)hamac_alloc(t,sizeof(a)*n)

t_uint16 AllocParamPool(void *local_vfmmemory_ctxt,t_hamac_param *t,t_uint32 nsize)
{
#ifdef _CACHE_OPT_
    t_uint8	*p=VFM_Alloc(local_vfmmemory_ctxt,nsize, VFM_MEM_CACHED_HWBUFFER_NONDSP, 8-1, 0, __LINE__, (t_uint8 *)__FILE__);
#else
    t_uint8	*p=VFM_Alloc(local_vfmmemory_ctxt,nsize, VFM_MEM_HWBUFFER_NONDSP, 8-1, 0, __LINE__, (t_uint8 *)__FILE__);
#endif
    if(p==0) return 1;
    t->offset_addr=VFM_GetPhysical(local_vfmmemory_ctxt, p)-p;
    t->heap_size=nsize;
    t->heap_addr=p;
    t->heap_current_addr=p;
    return 0;
}


void *hamac_alloc(t_hamac_param *t,t_uint32 nsize)
{
    nsize=ALIGN(nsize,8);

    void	*p=t->heap_current_addr;
    t->heap_current_addr+=nsize;

    if (t->heap_current_addr > (t->heap_addr+t->heap_size)) NMF_PANIC("Allocated Param In too small\n");
    return p;
}
#endif


/**
 *
 */
t_uint16 HamacAllocSlice(void *vfm_memory_ctxt,t_hamac_param *t, t_uint16 n) {
    t_uint16	i;
    ts_t1xhv_vdc_h264_slice *slice;
#ifdef VERBOSE_ALLOC
    NMF_LOG("Allocating Slices\n");
#endif
    t->addr_in_parameters->addr_first_slice=(t_uint32) HAMACALLOC(ts_t1xhv_vdc_h264_slice,n);//NO MTF
    if(t->addr_in_parameters->addr_first_slice == 0) return 1;
    slice = (ts_t1xhv_vdc_h264_slice*) t->addr_in_parameters->addr_first_slice;

    for(i=0;i<n-1;i++)  
    {
#ifdef VERBOSE_ALLOC
        NMF_LOG("Allocating Slice # %d\n",i);
#endif
#ifndef __ndk5500_a0__
        slice->addr_bitstream_buf_struct =(t_uint32)  HAMACALLOC(ts_t1xhv_bitstream_buf,1);//NO MTF
        if(slice->addr_bitstream_buf_struct == 0) return 1;
#endif
#ifdef __ndk8500_a0__	
        /* addr_weighted_pred_buffer, keep its address in reserved field, then use it or not depending on slice */ 	
        slice->reserved32_0 =(t_uint32)  HAMACALLOC(ts_t1xhv_vdc_h264_weight_list,1);//NO MTF 
        if(slice->reserved32_0 == 0) return 1;
#endif		
#ifdef __ndk5500_a0__
        slice->addr_weighted_pred_buffer = (t_uint32)  HAMACALLOC(ts_t1xhv_vdc_h264_weight_list,1);//NO MTF 
        if(slice->addr_weighted_pred_buffer == 0) return 1;
#endif
        //slice->addr_next_h264_slice=(t_uint32) slice + sizeof(ts_t1xhv_vdc_h264_slice);//NO MTF, done in FillParamIn
        slice++;
    }
    //LASt slice
#ifndef __ndk5500_a0__
    slice->addr_bitstream_buf_struct =(t_uint32)  HAMACALLOC(ts_t1xhv_bitstream_buf,1);//NO MTF
    if(slice->addr_bitstream_buf_struct == 0) return 1;
#endif

#ifdef __ndk8500_a0__	
    /* addr_weighted_pred_buffer, keep its address in reserved field, then use it or not depending on slice */ 	
    slice->reserved32_0 =(t_uint32)  HAMACALLOC(ts_t1xhv_vdc_h264_weight_list,1);//NO MTF 
    if(slice->reserved32_0 == 0) return 1;
#endif
#ifdef __ndk5500_a0__
    slice->addr_weighted_pred_buffer = (t_uint32)  HAMACALLOC(ts_t1xhv_vdc_h264_weight_list,1);//NO MTF 
    if(slice->addr_weighted_pred_buffer == 0) return 1;
#endif
    return 0;
}

void HamacDeallocSlice(void *vfm_memory_ctxt,t_hamac_param *t,t_uint16 n) {
    t_uint16	i;
    ts_t1xhv_vdc_h264_slice *slice;

    slice = (ts_t1xhv_vdc_h264_slice*) t->addr_in_parameters->addr_first_slice;
    for(i=0;i<n;i++)   
    {
#ifndef __ndk5500_a0__
        free((void*)slice->addr_bitstream_buf_struct);
#endif
#ifdef __ndk8500_a0__	
        /* addr_weighted_pred_buffer */ 	
        free((void*)slice->reserved32_0); 
#endif	
#ifdef __ndk5500_a0__
        free((void*)slice->addr_weighted_pred_buffer);
#endif
        slice++;
    }
    if((ts_t1xhv_vdc_h264_slice*) t->addr_in_parameters->addr_first_slice) free((ts_t1xhv_vdc_h264_slice*) t->addr_in_parameters->addr_first_slice);
}

#ifdef  OPTIMIZED_PARAM_POOL
t_uint32 HamacComputeParamSize(t_uint32	n)
{
    t_uint32	i,	nsize=0;
    nsize+= HAMACALLOCSIZE(ts_t1xhv_vdc_frame_buf_in,1);    // MTF 
    nsize+= HAMACALLOCSIZE(ts_t1xhv_vdc_frame_buf_out,1); 	// MTF 
    nsize+= HAMACALLOCSIZE(ts_t1xhv_vdc_internal_buf,1);  	// MTF
    nsize+= HAMACALLOCSIZE(ts_t1xhv_bitstream_buf_pos,1);// MTF 
    nsize+= HAMACALLOCSIZE(ts_t1xhv_bitstream_buf,1);//NO MTF
    nsize+= HAMACALLOCSIZE(ts_t1xhv_bitstream_buf_pos,1); // MTF
    nsize+= HAMACALLOCSIZE(ts_t1xhv_bitstream_buf,1);//NO MTF
    nsize+= HAMACALLOCSIZE(ts_t1xhv_vdc_h264_param_in,1); // MTF 
    //nsize+= HAMACALLOCSIZE(ts_t1xhv_vdc_h264_param_out,1); // MC: param out removed from cacheable param pool 


    nsize+=HAMACALLOCSIZE(ts_t1xhv_vdc_h264_slice,n);//NO MTF

    for(i=0;i<n-1;i++)
    {
#ifndef __ndk5500_a0__
        nsize+=HAMACALLOCSIZE(ts_t1xhv_bitstream_buf,1);//NO MTF
#endif
#if ((defined __ndk8500_a0__) || (defined __ndk5500_a0__))
        nsize+=HAMACALLOCSIZE(ts_t1xhv_vdc_h264_weight_list,1);//NO MTF 
#endif		
    }

#ifndef __ndk5500_a0__
    nsize+=	 HAMACALLOCSIZE(ts_t1xhv_bitstream_buf,1);//NO MTF
#endif
#if ((defined __ndk8500_a0__) || (defined __ndk5500_a0__))
    nsize+=	 HAMACALLOCSIZE(ts_t1xhv_vdc_h264_weight_list,1);//NO MTF 
#endif

#if ((defined __ndk8500_a0__) || (defined __ndk5500_a0__))
    nsize+= HAMACALLOCSIZE(ts_t1xhv_vdc_h264_scaling_list,1); // MTF
#endif
    return nsize;
}


#endif



t_uint16	METH(HamacAllocParameter)(void *vfm_memory_ctxt,t_hamac_param *t,t_uint16 n)
{
#ifdef	OPTIMIZED_PARAM_POOL 
    t_uint32	ParamSize=HamacComputeParamSize(n);
    t_uint16 error = AllocParamPool(vfm_memory_ctxt,t,ParamSize);
    if(error && ParamSize) return 1;
#endif


    // Reference buffer , list 0 ?
#ifdef VERBOSE_ALLOC
    NMF_LOG("Allocating in frame buff in struct\n");
#endif
    t->addr_in_frame_buffer  = HAMACALLOC(ts_t1xhv_vdc_frame_buf_in,1);    // MTF 
    if(t->addr_in_frame_buffer==0) return 1;
#ifdef VERBOSE_ALLOC
    NMF_LOG("Allocating frame buff out struct\n");
#endif
    t->addr_out_frame_buffer = HAMACALLOC(ts_t1xhv_vdc_frame_buf_out,1); 	// MTF 
    if(t->addr_out_frame_buffer==0) return 1;
#ifdef VERBOSE_ALLOC
    NMF_LOG("Allocating intenal buff struct \n");
#endif
    t->addr_internal_buffer = HAMACALLOC(ts_t1xhv_vdc_internal_buf,1);  // MTF
    if(t->addr_internal_buffer==0) return 1;

    // bistream in
#ifdef VERBOSE_ALLOC
    NMF_LOG("Allocating bitstream buf struct");
#endif
    t->addr_in_bitstream_buffer	= HAMACALLOC(ts_t1xhv_bitstream_buf_pos,1);// MTF 
    if(t->addr_in_bitstream_buffer==0) return 1;
    t->addr_in_bitstream_buffer->addr_bitstream_buf_struct 
        =(t_uint32)  HAMACALLOC(ts_t1xhv_bitstream_buf,1);//NO MTF
    if(t->addr_in_bitstream_buffer->addr_bitstream_buf_struct==0) return 1;
    // bistream out
#ifdef VERBOSE_ALLOC
    NMF_LOG("Allocating in buff pos struct\n");
#endif
    t->addr_out_bitstream_buffer= HAMACALLOC(ts_t1xhv_bitstream_buf_pos,1); // MTF
    if(t->addr_out_bitstream_buffer==0) return 1;
    t->addr_out_bitstream_buffer->addr_bitstream_buf_struct 
        =(t_uint32)  HAMACALLOC(ts_t1xhv_bitstream_buf,1);//NO MTF
    if(t->addr_out_bitstream_buffer->addr_bitstream_buf_struct==0) return 1;
    // Input main decoder params
#ifdef VERBOSE_ALLOC
    NMF_LOG("Allocating in param in struct \n");
#endif
    t->addr_in_parameters		= HAMACALLOC(ts_t1xhv_vdc_h264_param_in,1); // MTF 
    if(t->addr_in_parameters==0) return 1;

    // Output decoder params
#ifdef VERBOSE_ALLOC
    NMF_LOG("Allocating param out struct\n");
#endif
    t->addr_out_parameters = OLD_HAMACALLOC(ts_t1xhv_vdc_h264_param_out,1); // MTF
    if(t->addr_out_parameters==0) return 1;

    error = HamacAllocSlice(vfm_memory_ctxt,t, n);
    if(error) return 1;

#ifdef __ndk8500_a0__ 
    /* keep it into this reserved field and use it only if needed */
    t->addr_in_parameters->reserved32_0 = (t_ulong_value) HAMACALLOC(ts_t1xhv_vdc_h264_scaling_list,1); // MTF
    if(t->addr_in_parameters->reserved32_0==0) return 1;
#endif

#ifdef __ndk5500_a0__
//TODO check scaling_matrix ?  t->addr_in_parameters->addr_scaling_matrix
    t->addr_in_parameters->reserved32_0 = (t_ulong_value) HAMACALLOC(ts_t1xhv_vdc_h264_scaling_list,1);
    if(t->addr_in_parameters->reserved32_0==0) return 1;
 #endif

    t->addr_in_frame_parameters  = 0;// Not used
    t->addr_out_frame_parameters = 0;// Not used
    return 0;
}

void	METH(HamacDeallocParameter)(void *vfm_memory_ctxt,t_hamac_param *t, t_uint16 n)
{
#ifdef OPTIMIZED_PARAM_POOL
    free(t->heap_addr);
    t->heap_addr=0;
    free(t->addr_out_parameters); 
#else
    HamacDeallocSlice(vfm_memory_ctxt,t, n);

    free(t->addr_in_frame_buffer);
    free(t->addr_out_frame_buffer); 	 

    free(t->addr_internal_buffer);  

    // bistream in
    free((void*)(t->addr_in_bitstream_buffer->addr_bitstream_buf_struct));
    free(t->addr_in_bitstream_buffer); 

    // bistream out
    free((void*)(t->addr_out_bitstream_buffer->addr_bitstream_buf_struct));
    free(t->addr_out_bitstream_buffer);

    // Input main decoder params
#if ((defined __ndk8500_a0__) || (defined __ndk5500_a0__))
    free(t->addr_in_parameters->reserved32_0); /* scaling lists */
#endif
    free(t->addr_in_parameters);  

    // Output decoder params
    free(t->addr_out_parameters); 
#endif
}

//#define VERBOSE_PARAM 1
#if VERBOSE_PARAM

void DisplayFrameBuffIn(ts_t1xhv_vdc_frame_buf_in *p)
{
    printf("HOST:addr_fwd_ref_buffer : %x\n",p->addr_fwd_ref_buffer);
    printf("HOST:addr_bwd_ref_buffer : %x \n",p->addr_bwd_ref_buffer);
}
void DisplayFrameBuffOut(ts_t1xhv_vdc_frame_buf_out *p)
{
    printf("HOST:   addr_dest_buffer			 :%p\n",p->addr_dest_buffer);             
    printf("HOST:   addr_deblocking_param_buffer :%p\n",p->addr_deblocking_param_buffer);
    printf("HOST:   addr_motion_vector_buffer    :%p\n",p->addr_motion_vector_buffer);
    printf("HOST:   addr_jpeg_coef_buffer        :%p\n",p->addr_jpeg_coef_buffer);
    printf("HOST:   addr_jpeg_line_buffer        :%p\n",p->addr_jpeg_line_buffer);
}

void DisplayInternalBuff(ts_t1xhv_vdc_internal_buf  *p)
{
    printf("HOST:  addr_h264d_H4D_buffer  :%p\n",p->addr_vpp_dummy_buffer);             
    printf("HOST:  addr_h264d_block_info  :%p\n",p->addr_h264d_block_info);
    printf("HOST:  addr_h264d_mb_slice_ma :%p\n",p->addr_h264d_mb_slice_map);
    printf("HOST:  addr_mv_history_buffer :%p\n",p->addr_mv_history_buffer);
}


void DisplayParamIn(ts_t1xhv_vdc_h264_param_in *p)
{
  t_uint16 i;
  
  OstTraceFiltInst1( TRACE_FLOW ,"H264DEC: arm_nmf: decoder: HOST: addr_first_slice :%p\n",p->addr_first_slice); 
    
    for(i=0;i<17;i++)
    {
        printf("HOST: poc_list[%d]=%ld\n",i,p->poc_list[i]);
        printf("HOST: dpb_addresses[%d]=%p\n",i,p->dpb_addresses[i]);
    }
    printf("HOST: curr_pic_idx %d\n",p->curr_pic_idx);
#ifdef __ndk5500_a0__ 
/*
  NMF_LOG("HOST:   addr_input_bitstream_start: %p\n", p->addr_input_bitstream_start);
  NMF_LOG("HOST:   addr_input_bitstream_end:   %p\n", p->addr_input_bitstream_end);
  NMF_LOG("HOST:   addr_ib_sesb_start:         %p\n", p->addr_ib_sesb_start);
  NMF_LOG("HOST:   addr_ib_end:                %p\n", p->addr_ib_end);
  NMF_LOG("HOST:   addr_erc_ref_buffer:        %p\n", p->addr_erc_ref_buffer);
  NMF_LOG("HOST:   addr_local_rec_buffer:      %p\n", p->addr_local_rec_buffer);
  if (p->addr_external_sw)
      NMF_LOG("HOST:   addr_external_sw:      %p\n", p->addr_external_sw);
  if (p->addr_external_sw2)
      NMF_LOG("HOST:   addr_external_sw2:      %p\n", p->addr_external_sw2);
  NMF_LOG("HOST:   addr_mb_slice_map:          %p\n", p->addr_mb_slice_map);
  NMF_LOG("HOST:   addr_param_out:             %p\n", p->addr_param_out);
*/
#endif
}

void DisplaySliceParamIn(ts_t1xhv_vdc_h264_slice *p)
{
    t_uint16 	i=0;
  t_uint16 j;
     ts_t1xhv_vdc_h264_weight_list * loc_weight_list = (ts_t1xhv_vdc_h264_weight_list *)p->addr_weighted_pred_buffer;
 
     printf("HOST SL: discarded_slice %d\n",p->discarded_slice);
     printf("HOST SL: first_mb_in_slice %d\n",p->first_mb_in_slice);
     printf("HOST SL: slice_type %d\n",p->slice_type);
     printf("HOST SL: slice_num %d\n",p->slice_num);
     printf("HOST SL: direct_spatial_mv_pred_flag %d\n",p->direct_spatial_mv_pred_flag);
     printf("HOST SL: num_ref_idx_l0_active_minus1 %d\n",p->num_ref_idx_l0_active_minus1);
     printf("HOST SL: num_ref_idx_l1_active_minus1 %d\n",p->num_ref_idx_l1_active_minus1);
     printf("HOST SL: slice_qp %d\n",p->slice_qp);
     printf("HOST SL: s_info_disable_filter %d\n",p->s_info_disable_filter);
     printf("HOST SL: s_info_alpha_c0_offset_div2 %d\n",p->s_info_alpha_c0_offset_div2);
     printf("HOST SL: s_info_beta_offset_div2 %d\n",p->s_info_beta_offset_div2);
     printf("HOST SL: slice_header_bit_offset %d\n",p->slice_header_bit_offset);
     printf("HOST SL: long_termlist %x\n",p->long_termlist);
#ifndef __ndk5500_a0__
     printf("HOST SL: addr_bitstream_buf_struct %x\n",p->addr_bitstream_buf_struct);
#endif
     printf("HOST SL: addr_bitstream_start %x\n",p->addr_bitstream_start);
     printf("HOST SL: bitstream_offset %x\n",p->bitstream_offset);
     printf("HOST SL: bitstream_size_in_bytes %x\n",p->bitstream_size_in_bytes);
 for (j=0; j<16; j++ ){
      printf("HOST SL: listX_RefIdx2RefPic[%d] %x\n",j,p->listX_RefIdx2RefPic[j]);
 }
 
        printf("IF DEBUG (SLH)--- Weighted Prediction ---\n");
        for (j=0; j<3; j++ ){  
		  printf("IF DEBUG ");
            for (i=0;(i<8)&&(i<p->num_ref_idx_l0_active_minus1+1);i++){
                printf(" w[%3d] o[%3d] ",loc_weight_list->weight_l0[i][j], loc_weight_list->offset_l0[i][j]);
            } 
            printf("\n");
	  printf("IF DEBUG ");
            for (i=8;(i<16)&&(i<p->num_ref_idx_l0_active_minus1+1);i++){
                printf(" w[%3d] o[%3d] ",loc_weight_list->weight_l0[i][j], loc_weight_list->offset_l0[i][j]);
            } 
            printf("\n");
	  printf("IF DEBUG ");
            for (i=0;(i<8)&&(i<p->num_ref_idx_l1_active_minus1+1);i++){
                printf(" w[%3d] o[%3d] ",loc_weight_list->weight_l1[i][j], loc_weight_list->offset_l1[i][j]);
            }
	
            printf("\n");
	  printf("IF DEBUG ");
            for (i=8;(i<16)&&(i<p->num_ref_idx_l1_active_minus1+1);i++){
                printf(" w[%3d] o[%3d] ",loc_weight_list->weight_l1[i][j], loc_weight_list->offset_l1[i][j]);
            }}
            printf("\n");
}

void DisplayParam(t_uint16 nslices, t_hamac_param	*t)
{
    t_uint16 	i=0;
    ts_t1xhv_vdc_h264_slice*	slice;

    static int momo_ctr = 0;
    if(momo_ctr++<165)
        return;

    DisplayFrameBuffIn(t->addr_in_frame_buffer);
    DisplayFrameBuffOut( t->addr_out_frame_buffer);
    DisplayInternalBuff(t->addr_internal_buffer);
    DisplayParamIn(t->addr_in_parameters );

    slice=(ts_t1xhv_vdc_h264_slice*)t->addr_in_parameters->addr_first_slice;
    for(i=0;i<nslices;i++)
    {
        DisplaySliceParamIn(slice);
        slice++;
    }
}
#endif


void METH(HamacFillParamIn)(void *local_vfmmemory_ctxt,t_dec *dec, t_uint16 nslices,t_hamac_param	*t)
{
    t_uint16 	i=0;
    ts_t1xhv_vdc_h264_slice*	slice;

    //MeasureEventStart0(TRACE_EVENT_FILLPARAMIN_START,"fill param");

    dec->host.sh = &dec->sh[0];
    HamacFillFrameBuffIn(local_vfmmemory_ctxt,t->addr_in_frame_buffer,&dec->host);
    HamacFillFrameBuffOut(local_vfmmemory_ctxt,t->addr_out_frame_buffer,&dec->host);
    HamacFillInternals(local_vfmmemory_ctxt,t->addr_internal_buffer,&dec->host);
    HamacFillBistreamIn(local_vfmmemory_ctxt,t->addr_in_bitstream_buffer,&dec->sh[0], &dec->sh[nslices-1]);
    HamacFillBistreamOut(local_vfmmemory_ctxt,t->addr_out_bitstream_buffer,&dec->host);
    HamacFillParameterIn(local_vfmmemory_ctxt,t->addr_in_parameters,&dec->host);
#ifdef __ndk5500_a0__
    t->addr_in_parameters->nb_slice_in_frame = nslices;
    //NMF_LOG("%s: setting param_in nb_slice_in_frame:%d (nslices:%d)\n", __func__, t->addr_in_parameters->nb_slice_in_frame, nslices);
#endif
    // cast because field are defined as 32 bit int .. in struct
    slice=(ts_t1xhv_vdc_h264_slice*)t->addr_in_parameters->addr_first_slice;
    for(i=0;i<nslices-1;i++)  
    {
        dec->host.sh = &dec->sh[i];
        HamacFillSliceParameterIn(local_vfmmemory_ctxt,slice,&dec->host,&dec->buf,0);
        slice->addr_next_h264_slice=(t_uint32) slice + sizeof(ts_t1xhv_vdc_h264_slice);//NO MTF
        slice++;
    }
    dec->host.sh = &dec->sh[i];
    HamacFillSliceParameterIn(local_vfmmemory_ctxt,slice,&dec->host,&dec->buf,1);
    slice->addr_next_h264_slice=(t_uint32) slice + sizeof(ts_t1xhv_vdc_h264_slice);//NO MTF
    slice->addr_next_h264_slice=0;

#if VERBOSE_PARAM 
    DisplayParam(nslices, t);
#endif
    //MeasureEventStop(TRACE_EVENT_FILLPARAMIN_STOP,"fill param");
}




/**
 *
 */
void METH(HamacToPhysicalAddresses)(void *local_vfmmemory_ctxt,t_hamac_param *t, t_uint16 n)
{
    t_uint16	i;
    ts_t1xhv_vdc_h264_slice *slice;
#ifdef __ndk5500_a0__
        //NMF_LOG("%s: t->addr_in_parameters->addr_first_slice logical:%p ", __func__, 
                //t->addr_in_parameters->addr_first_slice);
#endif
    slice = (ts_t1xhv_vdc_h264_slice*) t->addr_in_parameters->addr_first_slice;
    t->addr_in_parameters->addr_first_slice=CONVERT_TO_MTF_AHB_ADDRESS(slice);
#ifdef __ndk5500_a0__
   //NMF_LOG("-> phys:%p\n", t->addr_in_parameters->addr_first_slice);
#endif

    for(i=0;i<n-1;i++)
    {
#if ((defined __ndk8500_a0__) || (defined __ndk5500_a0__))
        if(slice->addr_weighted_pred_buffer!=0)
        { // addr_weighted_pred_buffer is allocated in DDR so we set LSB to 0 (LSB used to indicate DDR vs ESRAM)
            slice->addr_weighted_pred_buffer =CONVERT_TO_AHB_ADDRESS_BE(slice->addr_weighted_pred_buffer);      
        }
#endif	

#ifdef __ndk5500_a0__
        slice->addr_bitstream_start = (t_ahb_address)GetPhysicallAddr(slice->addr_bitstream_start);
#else
        slice->addr_bitstream_buf_struct =CONVERT_TO_MTF_AHB_ADDRESS(slice->addr_bitstream_buf_struct);
#endif
        slice->addr_next_h264_slice =CONVERT_TO_MTF_AHB_ADDRESS( slice->addr_next_h264_slice);
        slice++;
    }

#if ((defined __ndk8500_a0__) || (defined __ndk5500_a0__))
    if(slice->addr_weighted_pred_buffer!=0)
    {// addr_weighted_pred_buffer is allocated in DDR so we set LSB to 0 (LSB used to indicate DDR vs ESRAM)
        slice->addr_weighted_pred_buffer =CONVERT_TO_AHB_ADDRESS_BE(slice->addr_weighted_pred_buffer);      
    }
#endif
#ifdef __ndk5500_a0__
    slice->addr_bitstream_start = (t_ahb_address)GetPhysicallAddr(slice->addr_bitstream_start);
#else
    slice->addr_bitstream_buf_struct =CONVERT_TO_MTF_AHB_ADDRESS(slice->addr_bitstream_buf_struct);
#endif
    slice->addr_next_h264_slice  = 0;


    // NO Endianess is needed for pointers above , because NMF marshalling take care of this conversion.
    // buff input
    t->addr_in_frame_buffer = 	(ts_t1xhv_vdc_frame_buf_in *)CONVERT_TO_MTF_DSP_ADDRESS(t->addr_in_frame_buffer);
    // buff output
    t->addr_out_frame_buffer = (ts_t1xhv_vdc_frame_buf_out *)CONVERT_TO_MTF_DSP_ADDRESS(t->addr_out_frame_buffer);
    //internal buffer
    t->addr_internal_buffer = (ts_t1xhv_vdc_internal_buf *)CONVERT_TO_MTF_DSP_ADDRESS(t->addr_internal_buffer);
    // bistream in
#if ((defined __ndk8500_a0__) || (defined __ndk5500_a0__))
    t->addr_in_bitstream_buffer->addr_bitstream_buf_struct = 
        (t_ahb_address)CONVERT_TO_MTF_AHB_ADDRESS(t->addr_in_bitstream_buffer->addr_bitstream_buf_struct);
#endif
    t->addr_in_bitstream_buffer = (ts_t1xhv_bitstream_buf_pos *)CONVERT_TO_MTF_DSP_ADDRESS(t->addr_in_bitstream_buffer);
    // bistream out
#if ((defined __ndk8500_a0__) || (defined __ndk5500_a0__))
    t->addr_out_bitstream_buffer->addr_bitstream_buf_struct = 
        (t_ahb_address)CONVERT_TO_MTF_AHB_ADDRESS(t->addr_out_bitstream_buffer->addr_bitstream_buf_struct);
#endif
    t->addr_out_bitstream_buffer = (ts_t1xhv_bitstream_buf_pos *)CONVERT_TO_MTF_DSP_ADDRESS(t->addr_out_bitstream_buffer);

#if ((defined __ndk8500_a0__)  || (defined __ndk5500_a0__))
    if(t->addr_in_parameters->addr_scaling_matrix!=0)
    { // scaling matrix is allocated in DDR so we set LSB to 0 (LSB used to indicate DDR vs ESRAM)
        t->addr_in_parameters->addr_scaling_matrix = CONVERT_TO_AHB_ADDRESS_BE(t->addr_in_parameters->addr_scaling_matrix);
    }
#endif

#ifdef __ndk5500_a0__
    // Input main decoder params	
    t->addr_in_parameters = (ts_t1xhv_vdc_h264_param_in *)CONVERT_TO_AHB_ADDRESS_BE(t->addr_in_parameters);
#else
    t->addr_in_parameters = (ts_t1xhv_vdc_h264_param_in *)CONVERT_TO_MTF_DSP_ADDRESS(t->addr_in_parameters);
#endif

    // Output decoder params
    // This is not a cacheable buffer !
    t->addr_out_parameters = 
    (ts_t1xhv_vdc_h264_param_out *)
    (MTF_MARKER(VFM_GetPhysical(local_vfmmemory_ctxt, (t_uint8 *)(t->addr_out_parameters))));

#ifdef __ndk5500_a0__ 
    /*NMF_LOG("addr_in_frame_buffer = %x\n",t->addr_in_frame_buffer);
    NMF_LOG("addr_out_frame_buffer = %x\n",t->addr_out_frame_buffer);
    NMF_LOG("addr_internal_buffer = %x\n",t->addr_internal_buffer);
    NMF_LOG("addr_in_bitstream_buffer = %x\n",t->addr_in_bitstream_buffer);
    NMF_LOG("addr_out_bitstream_buffer = %x\n",t->addr_out_bitstream_buffer);
    NMF_LOG("addr_in_params = %x\n",t->addr_in_parameters);
    NMF_LOG("addr_out_params = %x\n",t->addr_out_parameters); */
#endif
} /*End of HamacToPhysicalAddresses*/


/**
 *
 */
void METH(HamacToLogicalAddresses)(void *local_vfmmemory_ctxt,t_hamac_param *t, t_uint16 n)
{
    t_uint16	i;
    ts_t1xhv_vdc_h264_slice *slice;


    // Reference buffer , list 0 ?
    t->addr_in_frame_buffer = (ts_t1xhv_vdc_frame_buf_in *)UNCONVERT_TO_MTF_DSP_ADDRESS(t->addr_in_frame_buffer);
    t->addr_out_frame_buffer = (ts_t1xhv_vdc_frame_buf_out *)UNCONVERT_TO_MTF_DSP_ADDRESS(t->addr_out_frame_buffer);
    t->addr_internal_buffer = (ts_t1xhv_vdc_internal_buf *)UNCONVERT_TO_MTF_DSP_ADDRESS(t->addr_internal_buffer);

    // bistream in
    t->addr_in_bitstream_buffer = (ts_t1xhv_bitstream_buf_pos *)UNCONVERT_TO_MTF_DSP_ADDRESS(t->addr_in_bitstream_buffer);
#if ((defined __ndk8500_a0__) || (defined __ndk5500_a0__))
    t->addr_in_bitstream_buffer->addr_bitstream_buf_struct 
        = (t_ahb_address)UNCONVERT_TO_MTF_AHB_ADDRESS(t->addr_in_bitstream_buffer->addr_bitstream_buf_struct);
#endif
    // bistream out
    t->addr_out_bitstream_buffer = (ts_t1xhv_bitstream_buf_pos *)UNCONVERT_TO_MTF_DSP_ADDRESS(t->addr_out_bitstream_buffer);
#if ((defined __ndk8500_a0__) || (defined __ndk5500_a0__))
    t->addr_out_bitstream_buffer->addr_bitstream_buf_struct 
        = (t_ahb_address)UNCONVERT_TO_MTF_AHB_ADDRESS(t->addr_out_bitstream_buffer->addr_bitstream_buf_struct);
#endif

#ifdef __ndk5500_a0__
    // Input main decoder params	
    t->addr_in_parameters = (ts_t1xhv_vdc_h264_param_in *)UNCONVERT_TO_MTF_AHB_ADDRESS(t->addr_in_parameters);
#else
    t->addr_in_parameters = (ts_t1xhv_vdc_h264_param_in *)UNCONVERT_TO_MTF_DSP_ADDRESS(t->addr_in_parameters);
#endif

    // Output decoder params, we don't use cache for those, because they are written by DSP and read by ARM !
    t->addr_out_parameters =  
            (ts_t1xhv_vdc_h264_param_out *)VFM_GetLogical(local_vfmmemory_ctxt, 
            (t_uint8 *)( UNMARK_MTF((t_uint32)(t->addr_out_parameters))));

#ifdef __ndk5500_a0__
        //NMF_LOG("%s: t->addr_in_parameters->addr_first_slice physical:%p ", __func__, 
                //t->addr_in_parameters->addr_first_slice);
#endif
    t->addr_in_parameters->addr_first_slice=UNCONVERT_TO_MTF_AHB_ADDRESS(t->addr_in_parameters->addr_first_slice);
    slice = (ts_t1xhv_vdc_h264_slice*) t->addr_in_parameters->addr_first_slice;

#ifdef __ndk5500_a0__
        //NMF_LOG("-> physical:%p\n", __func__, t->addr_in_parameters->addr_first_slice);
#endif

    for(i=0;i<n-1;i++)
    {
#ifdef __ndk5500_a0__
        //NMF_LOG("%s: slice %d: %p slice->addr_bitstream_start phys:%p ", __func__, i, slice, slice->addr_bitstream_start);
       // slice->addr_bitstream_start = UNCONVERT_TO_MTF_AHB_ADDRESS(slice->addr_bitstream_start);
       slice->addr_bitstream_start = (t_address) GetLogicalAddr(slice->addr_bitstream_start);
        //NMF_LOG("-> logical:%p\n", slice->addr_bitstream_start);
#else
        slice->addr_bitstream_buf_struct = UNCONVERT_TO_MTF_AHB_ADDRESS(slice->addr_bitstream_buf_struct);
#endif
        slice->addr_next_h264_slice 	=  UNCONVERT_TO_MTF_AHB_ADDRESS (slice->addr_next_h264_slice);
        slice=(ts_t1xhv_vdc_h264_slice*) slice->addr_next_h264_slice;
    }
    //Last slice  
#ifdef __ndk5500_a0__
    //NMF_LOG("%s: slice %d: %p slice->addr_bitstream_start phys:%p ", __func__, i, slice, slice->addr_bitstream_start);
//    slice->addr_bitstream_start = UNCONVERT_TO_MTF_AHB_ADDRESS(slice->addr_bitstream_start);
       slice->addr_bitstream_start = (t_address) GetLogicalAddr(slice->addr_bitstream_start);
    //NMF_LOG("-> logical:%p\n", slice->addr_bitstream_start);
#else
    slice->addr_bitstream_buf_struct = UNCONVERT_TO_MTF_AHB_ADDRESS(slice->addr_bitstream_buf_struct);
#endif
    slice->addr_next_h264_slice = 0;

//	t->addr_in_frame_parameters = VFM_GetLogical(_vfmmemory_ctxt, (t_uint8 *)t->addr_in_frame_parameters);
//	t->addr_out_frame_parameters = VFM_GetLogical(_vfmmemory_ctxt, (t_uint8 *)t->addr_out_frame_parameters);
}/*End of HamacToLogicalAddresses*/


void METH(PrepareHedParameters)(void *local_vfmmemory_ctxt,t_hamac_param *t, t_hed_param *hed)
{
#ifdef __ndk5500_a0__
    t_uint32 pic_height_in_mbs =  (2-t->addr_in_parameters->frame_mbs_only_flag) * 
        (t->addr_in_parameters->pic_height_in_map_units);
#else
    t_uint32 pic_height_in_mbs = t->addr_in_parameters->pic_height_in_mbs;
#endif
    hed->hed_cfg =  ((t_uint32)t->addr_in_parameters->direct_8x8_inference_flag<<30)|
        ((t_uint32)t->addr_in_parameters->transform_8x8_mode_flag<<27)|
        ((t_uint32)t->addr_in_parameters->pic_init_qp<<21)|
        ((t_uint32)t->addr_in_parameters->num_ref_idx_l1_active_minus1<<16)|
        ((t_uint32)t->addr_in_parameters->num_ref_idx_l0_active_minus1<<11)|
        ((t_uint32)t->addr_in_parameters->deblocking_filter_control_present_flag<<10)|
        ((t_uint32)(t->addr_in_parameters->weighted_bipred_idc&0x1)<< 9)|
        ((t_uint32)t->addr_in_parameters->weighted_pred_flag<< 8)|
        ((t_uint32)t->addr_in_parameters->delta_pic_order_always_zero_flag<< 6)|
        ((t_uint32)t->addr_in_parameters->pic_order_present_flag<< 5)|
        ((t_uint32)t->addr_in_parameters->pic_order_cnt_type<< 3)|
        ((t_uint32)t->addr_in_parameters->frame_mbs_only_flag<< 2)|
        ((t_uint32)t->addr_in_parameters->entropy_coding_mode_flag<< 1);

    hed->hed_picwidth = (((t->addr_in_parameters->pic_width_in_mbs 
                        * pic_height_in_mbs)-1) << 16);
    hed->hed_picwidth |= t->addr_in_parameters->pic_width_in_mbs;

    hed->hed_codelength = ((t->addr_in_parameters->log2_max_frame_num|
                (t->addr_in_parameters->log2_max_pic_order_cnt_lsb<<5)));

    hed->bitstream_start = ((ts_t1xhv_bitstream_buf*)t->addr_in_bitstream_buffer->addr_bitstream_buf_struct)->addr_buffer_start;
    hed->bitstream_end   = ((ts_t1xhv_bitstream_buf*)t->addr_in_bitstream_buffer->addr_bitstream_buf_struct)->addr_buffer_end;
    hed->ib_start = ((ts_t1xhv_bitstream_buf*)t->addr_out_bitstream_buffer->addr_bitstream_buf_struct)->addr_buffer_start;

    hed->bitstream_start = Endianess(hed->bitstream_start);
    hed->bitstream_end = Endianess(hed->bitstream_end);
    hed->ib_start = Endianess(hed->ib_start);
}
