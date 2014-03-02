/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifdef NMF_BUILD
#include <h264dec/arm_nmf/decoder.nmf>
#endif
#ifndef __SYMBIAN32__
#include <los/api/los_api.h>
#endif

#include "types.h"
#include "host_types.h"
#include "macros.h"
#include "settings.h"
#include "host_mem.h"
#include "host_validation_io.h"
#include "local_alloc.h"
#include "host_display_queue.h"
#include "host_block_info.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "video_components_h264dec_arm_nmf_decoder_src_host_allocTraces.h"
#endif


//#include "host_input_control.h"
//#include "host_hamac_pipe.h"

/* Local function prototypes */
t_uint16 getDpbSize(t_seq_par *p_sp);
// +CR324558 CHANGE START FOR
t_uint16 getDpbSizeFromLevel(t_seq_par *p_sp);
// -CR324558 CHANGE END OF
/**
 * \brief Allocate memory for buffers
 * \param p_sp Pointer to active sequence parameter set structure
 * \param p_buff Pointer to buffers structure
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>30-11-2004&nbsp;</td><td>First release.</td></tr>
 * <tr><td valign=top>10-12-2004&nbsp;</td><td>Modified due to changes to block infos structure</td></tr>
 * </table>
 * </dl>
 *
 * The function allocates memory for frame buffers, blocks informations and macroblock to slice group map.
 */

/* see nmf/think/api/cm/engine/memory/inc/memory_type.h */
#define SVA_MM_ALIGN_WORD       0x3
#define SVA_MM_ALIGN_8WORDS     0x1f
#define SVA_MM_ALIGN_256BYTES   0xff


#define HWBUFF_ALLOC(_size,_align)   	VFM_Alloc(vfm_memory_ctxt, (_size), VFM_MEM_HWBUFFER_NONDSP, _align, 0, __LINE__, (t_uint8 *)__FILE__)
#define HWBUFF_CACHED_ALLOC(_size,_align)   	VFM_Alloc(vfm_memory_ctxt, (_size), VFM_MEM_CACHED_HWBUFFER_NONDSP, _align, 0, __LINE__, (t_uint8 *)__FILE__)
#define MPC_ALLOC(_size, _align)	VFM_Alloc(vfm_memory_ctxt, (_size), VFM_MEM_CM_MPC_ESRAM16_ALLOC, _align, 0, __LINE__, (t_uint8 *)__FILE__)
#define MPC_ALLOC_SDRAM(_size, _align)	VFM_Alloc(vfm_memory_ctxt, (_size), VFM_MEM_CM_MPC_SDRAM16_ALLOC, _align, 0, __LINE__, (t_uint8 *)__FILE__)

t_uint32 METH(AllocateMemory)(void *vfm_memory_ctxt,t_uint32 mbx,t_uint32 mby, t_dec_buff *p_buff)
{
    int i;
    t_uint32 internal_error = 0;

#ifdef VERBOSE_ALLOC
    NMF_LOG("Enter allocation \n");
#endif

    OstTraceFiltInst2( TRACE_FLOW , "H264DEC: arm_nmf: decoder: enter alloc, size = %dx%d MBs\n",mbx,mby);

    if (p_buff->AllocatedDPBsize )
    {
      OstTraceInt0( TRACE_ERROR , "H264DEC: arm_nmf: decoder: Memory leak with DPB size related allocation\n");
      //NMF_PANIC("Memory leak with DPB size related allocation");
      return 0;
    }
    p_buff->AllocatedDPBsize=p_buff->DPBsize;

    for (i=0; i<HAMAC_PIPE_SIZE+1; i++) {
#ifdef VERBOSE_ALLOC
        NMF_LOG("Allocating Param in \n");
#endif
        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: arm_nmf: decoder: Allocating param in\n");
        internal_error = HamacAllocParameter(vfm_memory_ctxt,&hamac_pipe->hamac_pipe_info[i].param_in, SLICE_LINKED_LIST_SIZE);
        if(internal_error) return 1;
        hamac_pipe->hamac_pipe_info[i].hamac_pipe_specific_info.slice_linked_list_size = SLICE_LINKED_LIST_SIZE;

        /* Allocate buffer for block info */
        /* This structure must be align in ESRAM16 or SDRAM16 (not 24), otherwise sizeof is not correct */
#ifdef VERBOSE_ALLOC
        NMF_LOG("Allocating B_info \n");
#endif

#ifdef NO_HAMAC
        hamac_pipe->hamac_pipe_info[i].hamac_pipe_specific_info.b_info =
            (t_block_info *)HWBUFF_ALLOC(mbx * mby * 16 * sizeof(t_block_info), SVA_MM_ALIGN_8WORDS);
#else
        hamac_pipe->hamac_pipe_info[i].hamac_pipe_specific_info.b_info = (t_macroblock_info*)0; /* not used ! */
#endif

#if ((defined __ndk8500_a0__) || (defined __ndk5500_a0__))
     /* intermediate buffer, 1 buffer per pipe slot */
     /* size of intermediate buffer for SODB is 276 bytes x MB width x MB height,
    so taking 1080p case... then for SESB, size is NB_SLICES * 8, we take 8160 slices */
		hamac_pipe->hamac_pipe_info[i].hamac_pipe_specific_info.bits_intermediate_buffer =
       (t_uint8*)HWBUFF_ALLOC((276 * 120 * 68) + (8160*8),SVA_MM_ALIGN_8WORDS);
    if(hamac_pipe->hamac_pipe_info[i].hamac_pipe_specific_info.bits_intermediate_buffer==NULL)
    {
      OstTraceInt0( TRACE_ERROR , "H264DEC: arm_nmf: decoder: Alloc failed for IB\n");
      //NMF_PANIC("Alloc failed for intermediate buffer\n");
      return 1;
    }
#endif


    } /* HAMAC PIPE SIZE */

#if ((defined __ndk8500_a0__) || (defined __ndk5500_a0__))
  /* Allocate CUP context, this replaces former "b_info". But now we have N buffers, where N is the size
      of the DPB. This is needed because of High Profile B frames, because CUP needs "colocated context"
      and the "colocated" can be any buffer in the DPB...  */
      OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: arm_nmf: decoder: Allocating CUP ctxt\n");
      internal_error = AllocCupContextArray(vfm_memory_ctxt,mbx,mby,p_buff);
      if(internal_error) return 1;
#endif

#ifdef VERBOSE_ALLOC
    NMF_LOG("Allocating Aux frames \n");
#endif
  OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: arm_nmf: decoder: Allocating aux frames\n");

#ifdef NO_HAMAC
    p_buff->aux_frame = (t_uint16 *)HWBUFF_ALLOC(mbx * mby * 192 * sizeof(t_uint16), SVA_MM_ALIGN_256BYTES);
#else
    // This array should not be allocated in ESRAM , this can lead to dead lock => hardware constraint
    p_buff->aux_frame = (t_uint16 *)HWBUFF_ALLOC(32, 15 );   /* THIS IS USED FOR VPP DUMMY BUFFER, 4 words of 64 bits */
#endif
    if (p_buff->aux_frame == NULL)
    {
        OstTraceInt0( TRACE_ERROR , "H264DEC: arm_nmf: decoder: Alloc failed for aux fr\n");
        //NMF_PANIC("\n\nCould not allocate memory for aux_frame!\n\n");
        return 1;
    }

#ifdef VERBOSE_ALLOC
    NMF_LOG("Allocating ParamV\n");
#endif
    OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: arm_nmf: decoder: Allocating paramV\n");

#if ((defined __ndk8500_a0__) || (defined __ndk5500_a0__))
    /* deblocking params, 2 words of 64 bits / MB */
    p_buff->p_deblocking_paramv = (tps_h4d_param *)HWBUFF_ALLOC(mbx * mby * 16, SVA_MM_ALIGN_8WORDS);
#else
    p_buff->p_deblocking_paramv = (tps_h4d_param *)HWBUFF_ALLOC(mbx * mby * sizeof(tps_h4d_param), SVA_MM_ALIGN_8WORDS);
#endif
    if (p_buff->p_deblocking_paramv == NULL)
    {
        OstTraceInt0( TRACE_ERROR , "H264DEC: arm_nmf: decoder: Alloc failed for deb params\n");
//		NMF_PANIC("Alloc failed deblocking paramv for size %d\n", mbx * mby * sizeof(tps_h4d_param));
        return 1;
    }

   internal_error = AllocateESRAM(vfm_memory_ctxt,mbx,mby,p_buff);

#ifdef VERBOSE_ALLOC
    NMF_LOG("End of Alloc\n");
#endif
    OstTraceFiltInst0( TRACE_FLOW , "H264DEC: arm_nmf: decoder: End of alloc\n");

   return internal_error;
}


/**
 * \brief Allocate memory for slice headers array.
 * \param sha Pointer to the array of slice header to allocate
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>01-08-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function allocates memory for slice headers array.
 */

t_uint16 AllocateSliceArray(void *vfm_memory_ctxt,t_slice_hdr **sha)
{
    *sha = (t_slice_hdr *)malloc(MAXNSLICE * sizeof(t_slice_hdr));

    if (*sha == NULL)
    {
        OstTraceInt0( TRACE_ERROR , "H264DEC: arm_nmf: decoder: Could not allocate memory for slice headers\n");
        return 1;
    }
    return 0;
}



/**
 * \brief Reallocate memory for slice headers array.
 * \param maxslices Current dimension of array
 * \param sha Pointer to the array of slice header to allocate
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>01-08-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function reallocates memory for slice headers array doubling the dimension.
 */

t_uint16 ReallocateSliceArray(void *vfm_memory_ctxt,t_uint16 *maxslices, t_slice_hdr **sha)
{
    void *p;
    t_uint16	prevslices=*maxslices;

    (*maxslices) <<= 1;
    p=malloc( (*maxslices)*sizeof(t_slice_hdr));
    if (p==0)
    {
        OstTraceInt0( TRACE_ERROR , "H264DEC: arm_nmf: decoder: unable to realloc slice array\n");
        //NMF_PANIC("unable to realloc slice array\n");
        return 1;
    }
    memcpy(p,*sha,prevslices*sizeof(t_slice_hdr));
    free(*sha);
    *sha= (t_slice_hdr*)p;
    return 0;
}



/**
 * \brief Free the allocated memory
 * \param p_buff Pointer to buffers structure
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>30-11-2004&nbsp;</td><td>First release.</td></tr>
 * <tr><td valign=top>10-12-2004&nbsp;</td><td>Modified due to changes to block infos structure</td></tr>
 * </table>
 * </dl>
 *
 * The function frees the allocated memory.
 */
void METH(FreeMemory)(void *vfm_memory_ctxt,t_dec_buff *p_buff)
{
    int i;

    OstTraceFiltInst0( TRACE_FLOW , "H264DEC: arm_nmf: decoder: FreeMemory\n");

    p_buff->AllocatedDPBsize=0;
    if (!vfm_memory_ctxt) {
        OstTraceInt0( TRACE_WARNING , "H264DEC: arm_nmf: decoder: FreeMemory, VFM ctxt is NULL\n");
        return; // could be the case if the construct fails
    }

    for (i=0; i<HAMAC_PIPE_SIZE+1; i++) {

        OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: arm_nmf: decoder: FreeMemory param in\n");

        HamacDeallocParameter(	vfm_memory_ctxt,&hamac_pipe->hamac_pipe_info[i].param_in,
                hamac_pipe->hamac_pipe_info[i].hamac_pipe_specific_info.slice_linked_list_size);

        /* Free blocks info buffer */
        if(hamac_pipe->hamac_pipe_info[i].hamac_pipe_specific_info.b_info!=0)
            free((void *)hamac_pipe->hamac_pipe_info[i].hamac_pipe_specific_info.b_info);

#if ((defined __ndk8500_a0__) || (defined __ndk5500_a0__))
	    OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: arm_nmf: decoder: FreeMemory IB\n");
    if(hamac_pipe->hamac_pipe_info[i].hamac_pipe_specific_info.bits_intermediate_buffer!=NULL)
      free((void*)hamac_pipe->hamac_pipe_info[i].hamac_pipe_specific_info.bits_intermediate_buffer);
#endif
	}

#if ((defined __ndk8500_a0__) || (defined __ndk5500_a0__))
    OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: arm_nmf: decoder: FreeMemory CUP ctxt\n");
    FreeCupContextArray(vfm_memory_ctxt,p_buff);
#endif

    OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: arm_nmf: decoder: FreeMemory aux\n");

    /* Free auxiliary frame buffer */
    free((void *)p_buff->aux_frame);

    OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: arm_nmf: decoder: FreeMemory paramV\n");

    /* Free deblocking filter parameter buffer */
    free((void *)p_buff->p_deblocking_paramv);

    OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: arm_nmf: decoder: FreeMemory ESRAM\n");
    FreeESRAM(vfm_memory_ctxt,p_buff);

    OstTraceFiltInst0( TRACE_FLOW , "H264DEC: arm_nmf: decoder: End of FreeMemory\n");
}

/**
 * \brief Free the memory allocated for slice headers array
 * \param sha Pointer to the array of slice header to allocate
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>01-08-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function frees the memory allocated for slice headers array.
 */

void FreeSliceMemory(void *vfm_memory_ctxt,t_slice_hdr *sha)
{
    if (!vfm_memory_ctxt) {
        return; // could be the case if the construct fails
    }

    free((void *)sha);
}

t_uint32 METH(AllocateESRAM)(void *vfm_memory_ctxt,t_uint32 mbx,t_uint32 mby, t_dec_buff *p_buff)
{
    int i;
#ifdef __ndk5500_a0__
    t_uint32 esramBase = 0x40000000;
#endif

    OstTraceFiltInst0( TRACE_FLOW , "H264DEC: arm_nmf: decoder: Allocate ESRAM\n");
    //NMF_LOG("H264DEC: arm_nmf: decoder: Allocate ESRAM\n");

 if(dec_static.esram_allocated == 1)
 {
  OstTraceFiltInst0( TRACE_DEBUG , "H264DEC: arm_nmf: decoder: ESRAM already allocated\n");
  return 0;
 }

 for (i=0; i<HAMAC_PIPE_SIZE+1; i++)
 {
#ifdef __ndk5500_a0__
        t_uint32 alloc_size = mbx * mby * sizeof(t_uint16);
        hamac_pipe->hamac_pipe_info[i].hamac_pipe_specific_info.mb_slice_map =
            (t_uint16*)(HWBUFF_ALLOC(alloc_size, SVA_MM_ALIGN_8WORDS));
#else
		/* Allocate array for MB to Slice Group Map */
		hamac_pipe->hamac_pipe_info[i].hamac_pipe_specific_info.mb_slice_map =
			(t_uint16 *)MPC_ALLOC_SDRAM(mbx * mby * sizeof(t_uint16), SVA_MM_ALIGN_8WORDS);
		if (hamac_pipe->hamac_pipe_info[i].hamac_pipe_specific_info.mb_slice_map == NULL)
		{
      OstTraceInt0( TRACE_ERROR , "H264DEC: arm_nmf: decoder: Alloc failed for slicemap\n");
			//NMF_PANIC("Alloc failed for slicemap , size %d\n", mbx * mby * sizeof(t_uint16));
      return 1;
		}
#endif
  } /* HAMAC PIPE */

#ifdef __ndk8500_a0__
  /* Allocate local reconstruction buffer, 5 lines of macroblocks, this is allocated only once
    and used for each frame decode (scratch area) */
  p_buff->local_recon_buff = (t_uint8 *)MPC_ALLOC(((mbx * 16) * (5 * 16) * 3)/2,SVA_MM_ALIGN_256BYTES);
  if(p_buff->local_recon_buff == NULL)
  {
     OstTraceInt0( TRACE_ERROR , "H264DEC: arm_nmf: decoder: Alloc failed for loc rec buf\n");
     //NMF_PANIC("Alloc failed for local recon buff\n");
     return 1;
  }
#endif

#ifdef __ndk5500_a0__
    p_buff->local_recon_buff = (t_uint8*)esramBase;
    p_buff->external_sw = p_buff->local_recon_buff + ((mbx * 16) * (5 * 16) * 3)/2;
    //external_sw2 is used for backward ref. (its height: 3 MB)
    p_buff->external_sw2 = p_buff->external_sw + (mbx * 16) * (5 * 16);
#endif

#ifdef NO_HAMAC
    p_buff->aux_frame2 = p_buff->aux_frame;
#else
#ifdef __ndk5500_a0__
   // p_buff->aux_frame2 = (t_uint16 *)(p_buff->local_recon_buff + ((mbx * 16) * (5 * 16) * 3)/2);
      p_buff->aux_frame2 = p_buff->aux_frame;
#else
    p_buff->aux_frame2 = (t_uint16 *)MPC_ALLOC(32, 15 );   /* THIS IS USED FOR VPP DUMMY BUFFER, 4 words of 64 bits */
#endif
#endif
    if (p_buff->aux_frame2 == NULL)
    {
        OstTraceInt0( TRACE_ERROR , "H264DEC: arm_nmf: decoder: Alloc failed for aux\n");
        //NMF_PANIC("\n\nCould not allocate memory for aux_frame2!\n\n");
        return 1;
    }

    dec_static.esram_allocated = 1;
  return 0;
}



void METH(FreeESRAM)(void *vfm_memory_ctxt, t_dec_buff *p_buff)
{
    int i;

    OstTraceFiltInst0( TRACE_FLOW , "H264DEC: arm_nmf: decoder: Free ESRAM\n");

    if(dec_static.esram_allocated == 0)
    {
        OstTraceFiltInst0( TRACE_FLOW , "H264DEC: arm_nmf: decoder: ESRAM already freed\n");
        return;
    }

    for(i=0; i<HAMAC_PIPE_SIZE+1; i++)
    {
        /* Free array for MB to Slice Group Map */
        free((void *)hamac_pipe->hamac_pipe_info[i].hamac_pipe_specific_info.mb_slice_map);
    }

#ifdef __ndk8500_a0__
    if(p_buff->local_recon_buff != NULL)
        free((void*)p_buff->local_recon_buff);

    free((void *)p_buff->aux_frame2);
#endif

    dec_static.esram_allocated = 0;

}

// +CR324558 CHANGE START FOR
t_uint16 getDpbSize(t_seq_par *p_sp){

    t_uint32 dpbSize_levelBased=0;
    t_uint32 maxDpbSize=0;
    dpbSize_levelBased = getDpbSizeFromLevel(p_sp);
    if ((p_sp->vui_pars_flag) && (p_sp->vui.bitstream_restriction_flag)){
        maxDpbSize = MAX(p_sp->vui.max_dec_frame_buffering, 1);
        //NMF_LOG("\nDecoder::getDpbSize p_sp->vui.max_dec_frame_buffering=%d",p_sp->vui.max_dec_frame_buffering);
    }
    else{
        //NMF_LOG("\nDecoder::getDpbSize dpbSize_levelBased=%d",dpbSize_levelBased);
		maxDpbSize = dpbSize_levelBased;
    }
    return MIN(maxDpbSize,16);
}


/**
 * \brief Find DPB size as function of level and picture size.
 * \param p_sp Pointer to active sequence parameter set structure
 * \return DPB size
 * \author Denis Zinato
 * \version 1.0
 *
 * <dl compact><dt><b>History:</b></dt><dd>
 * <table border=0 cellspacing=2 cellpadding=0>
 * <tr><td valign=top>18-01-2005&nbsp;</td><td>First release.</td></tr>
 * </table>
 * </dl>
 *
 * The function find DPB size as function of level and picture size.
 */

t_uint16 getDpbSizeFromLevel(t_seq_par *p_sp)
{
    t_uint32 pic_size = (p_sp->pic_width_in_mbs_minus1 + 1) *
        (p_sp->pic_height_in_map_units_minus1 + 1) *
        (2-p_sp->frame_mbs_only_flag)* 384;
    t_uint32 size = 0;

    switch (p_sp->level_idc)
    {
        case 9: //
        case 10:
            size = 152064;
            break;

        case 11:
            size = 345600;
            break;

        case 12:
            size = 912384;
            break;

        case 13:
            size = 912384;
            break;

        case 20:
            size = 912384;
            break;

        case 21:
            size = 1824768;
            break;

        case 22:
            size = 3110400;
            break;

        case 30:
            size = 3110400;
            break;

        case 31:
            size = 6912000;
            break;

        case 32:
            size = 7864320;
            break;

        case 40:
            size = 12582912;
            break;

        case 41:
            size = 12582912;
            break;
//+Change start for CR336095,ER334368,ER336290 level 5.1. support
        case 42:
            size = 13369344;		//Table A7 Annex A ITU-T Rec H.264 (03/2010)
            break;
//-Change end for CR336095,ER334368,ER336290 level 5.1. support
        case 50:
            size = 42393600;
            break;

        case 51:
            size = 70778880;
            break;

        default:
            fatal_error("\n\nUndefined level.\n\n");
            break;
    }
    size /= pic_size;
    return size;
}
// -CR324558 CHANGE END OF