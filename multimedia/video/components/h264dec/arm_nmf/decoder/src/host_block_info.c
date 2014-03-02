/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "video_components_h264dec_arm_nmf_decoder_src_host_block_infoTraces.h"
#endif

#include "types.h"
#include "host_types.h"
#include "macros.h"
#include "settings.h"
#include "host_block_info.h"
#include "local_alloc.h"

#define SVA_MM_ALIGN_WORD       0x3
#define SVA_MM_ALIGN_8WORDS     0x1f
#define SVA_MM_ALIGN_256BYTES   0xff


/**
 * \brief Reset to default all block informations
 * \param p_sp Pointer to the active sequence parameter set
 * \param p_buff Pointer to the buffers structure
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>23-06-2004&nbsp;</td><td>First release.</td></tr>
 * <tr><td valign=top>10-12-2004&nbsp;</td><td>Modified due to changes to block infos structure</td></tr>
 * </table>
 * </dl>
 *
 * The function sets to default all block informations
 */

void resetBlockInfo(t_seq_par *p_sp, t_dec_buff *p_buff)
{
	t_sint32 i;
	t_sint32 size = (p_sp->pic_width_in_mbs_minus1+1) * (p_sp->pic_height_in_map_units_minus1+1)*(2-p_sp->frame_mbs_only_flag);
#ifdef NO_HAMAC
	t_block_info *p_blk = p_buff->b_info;
	for (i=0; i < size*16; i++)
	{
		p_blk->nslice = -1;
		p_blk->non_zero[0] = 0;
		p_blk->non_zero[1] = 0;
		p_blk->non_zero[2] = 0;
		p_blk->type = -1;
		p_blk->concealed = 0;

		p_blk++;
	}
#else
  t_macroblock_info * curr_ctx;
  t_uint32 curr_pic_idx=0;

  for(i=0;i<p_buff->DPBsize+1;i++)
    {
      if((p_buff->pics_buf[i].poc
      == p_buff->curr_info->poc)
        &&(p_buff->pics_buf[i].frame_num
        == p_buff->curr_info->frame_num))
        {
          curr_pic_idx = i;
          break;
        }
    }
  curr_ctx = (t_macroblock_info *) p_buff->cup_ctx_desc.cup_context[p_buff->pics_buf[curr_pic_idx].cup_ctx_idx];
#if VERBOSE_CUP_FIFO == 1
  NMF_LOG("Resetting CUP ctx %d\n",p_buff->pics_buf[curr_pic_idx].cup_ctx_idx);
#endif

	//memset( (void *)curr_ctx, 0x0,size*sizeof(t_macroblock_info));

  /* Initialize nslice field for all array elements */
  for (i = 0; i < size; i++)
  {
      curr_ctx[i].nslice = -1;
#ifndef __ndk5500_a0__
      curr_ctx[i].user = 0; /* MC: this needs to be initialized because used by FW to
                                    know if a macroblock has already been concealed */
#endif
  }
#endif
}



/**
 * \brief Get decode status of a macroblock
 * \param mbaddr Address of macroblock
 * \param p_b_info Pointer to the block info buffer
 * \return 1 if not decoded, 0 otherwise
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>08-04-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function get the decode status of a macroblock.
 */

t_uint16 NotDecoded(t_uint16 mbaddr, t_block_info *p_b_info)
{

#ifdef NO_HAMAC
	if ((p_b_info + mbaddr*16)->nslice == -1)
    {
        return 1;
    }
    else
    {
        return 0;
    }

#else
	fatal_error("Fatal error : b_info structure can not be used by HOST code , concealment is done at HAMAC level\n");
	return 0;
#endif
}



#if ((defined __ndk8500_a0__) || (defined __ndk5500_a0__))
/**************************************************************************************
 * This function does the allocation of the table containing CUP context buffers
 * for the whole DPB (+ the size of the Hamac pipe)
 * On 8500v1, we need CUP context for the whole DPB because of B frames and colocated,
 * and we need to pipe it because the reset of block info is done by the ARM, so
 * we need to avoid resetting a context while it is still used by the DSP...
 **************************************************************************************/
t_uint16 AllocCupContextArray(void *vfm_memory_ctxt,t_uint32 mbx,t_uint32 mby, t_dec_buff *p_buff)
{
    t_uint16 i;

    cup_context_fifo_reset(&p_buff->cup_ctx_desc);

    for(i=0;i<p_buff->DPBsize+1+HAMAC_PIPE_SIZE+1;i++)
    {
        p_buff->cup_ctx_desc.cup_context[i] = (t_uint8 *)mallocCM(mbx*mby*CUP_CONTEXT_SIZE_IN_BYTES,SVA_MM_ALIGN_8WORDS);
        if(p_buff->cup_ctx_desc.cup_context[i] == NULL)
        {
            OstTraceInt0( TRACE_ERROR , "H264DEC:  Alloc failed for cupcontext\n");
            //NMF_PANIC("Alloc failed for cupcontext\n");
            return 1;
        }
        cup_context_fifo_push(&p_buff->cup_ctx_desc,i);
    }
    for(;i<16+1+HAMAC_PIPE_SIZE+1;i++)
        p_buff->cup_ctx_desc.cup_context[i] = (t_uint8*) NULL;
    return 0;
}

/**************************************************************************************
 * This function frees the table containing CUP context buffers
 **************************************************************************************/
void FreeCupContextArray(void *vfm_memory_ctxt,t_dec_buff *p_buff)
{
  t_uint16 i;

  cup_context_fifo_reset(&p_buff->cup_ctx_desc);

  for(i=0;i<16+1+HAMAC_PIPE_SIZE+1;i++)
  {
      if(p_buff->cup_ctx_desc.cup_context[i] != NULL)
      {
        free((void*)p_buff->cup_ctx_desc.cup_context[i]);
        p_buff->cup_ctx_desc.cup_context[i] = (t_uint8 *) NULL;
      }
  }
}

/*************************************************************************************
 * This function puts back all cup context buffers into CUP context FIFO
 *************************************************************************************/
void InitCupContextFifo(t_dec_buff *p_buff)
{
  t_uint16 i;

  cup_context_fifo_reset(&p_buff->cup_ctx_desc);

  for(i=0;i<16+1+HAMAC_PIPE_SIZE+1;i++)
  {
      if(p_buff->cup_ctx_desc.cup_context[i] != NULL)
      {
         cup_context_fifo_push(&p_buff->cup_ctx_desc,i);
      }
  }

  /* this is probably redundant */
  for (i=0; i < MAXNUMFRM; i++)
    p_buff->pics_buf[i].cup_ctx_idx = -1;
}

#endif




