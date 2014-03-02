/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifdef NMF_BUILD
#include <h264dec/arm_nmf/decoder.nmf>
#endif

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "video_components_h264dec_arm_nmf_decoder_src_host_decoder_parameterTraces.h"
#endif

#include "types.h"
#include "host_types.h"
#include "host_decoder.h"
#include "settings.h"
#include "host_frameinfo.h"

t_uint16 getDpbSize(t_seq_par *p_sp);

void METH(set_memory_ctxt)(t_uint16 ack, void *mem_ctxt) {
    t_uint16 error = 0;
    if (mem_ctxt != 0) {
        if (vfm_memory_ctxt) {
#ifndef __ndk5500_a0__
            OstTraceInt0( TRACE_ERROR , "H264DEC: vfm_memory_ctxt was already set in METH(set_memory_ctxt)\n");
#endif
            NMF_PANIC("vfm_memory_ctxt was already set in METH(set_memory_ctxt)\n");
        }
        // this interface should be called as soon as possible by the ddep / application
        vfm_memory_ctxt = mem_ctxt;
    }
    if (vfm_memory_ctxt) {
        error  = init_decoder(&dec_static,vfm_memory_ctxt);
    }
    if (ack) {
        if(error)
            pendingCommandAck.pendingCommandAck(INSUFFICIENT_INTERNAL_MEMORY);
        else
            pendingCommandAck.pendingCommandAck(COMMAND_COMPLETED);
    }
}

void METH(set_nParallelDeblocking)(t_uint16 ack, t_uint16 nParallelDeblocking) {
    mDblkMode = nParallelDeblocking;
    dec_static.DBLK_mode = nParallelDeblocking;
    if (ack) {
        pendingCommandAck.pendingCommandAck(COMMAND_COMPLETED);
    }
}

void METH(set_SupportedExtension)(t_uint16 ack, t_uint32 nSupportedExtension) {
    mSupportedExtension = nSupportedExtension;
    if (ack) {
        pendingCommandAck.pendingCommandAck(COMMAND_COMPLETED);
    }
}

void METH(set_Framerate)(t_uint16 ack, t_uint32 nFramerate) {
    dec_static.g_omx_framerate = nFramerate;
    if (ack) {
        pendingCommandAck.pendingCommandAck(COMMAND_COMPLETED);
    }
}

void METH(set_Thumbnail)(t_uint16 ack, t_uint16 bThumbnailGeneration) {
    dec_static.bThumbnailGeneration = bThumbnailGeneration;

    if (ack) {
        pendingCommandAck.pendingCommandAck(COMMAND_COMPLETED);
    }
}

void METH(set_ImmediateRelease)(t_uint16 ack, t_uint16 bImmediateRelease) {
    dec_static.bImmediateRelease = bImmediateRelease;

    if (ack) {
        pendingCommandAck.pendingCommandAck(COMMAND_COMPLETED);
    }
}


void METH(set_RestrictLevel)(t_uint16 ack, t_uint16 level, t_uint16 isSoc1080pCapable) {
	dec_static.restrictMaxLevel = level;
	dec_static.isSoc1080pCapable = isSoc1080pCapable;
    if (ack) {
        pendingCommandAck.pendingCommandAck(COMMAND_COMPLETED);
    }
}
// +CR324558 CHANGE START FOR
void METH(set_picsize)(t_uint16 ack, t_uint16 pic_width, t_uint16 pic_height, t_uint32 dpb_size) {
    t_uint32 mbwidth=(pic_width+15)/16;
    t_uint32 mbheight=(pic_height+15)/16;
    t_uint32 internal_error = 0;
    t_uint8 reallocateMemory=0;
    OstTraceFiltInst3( TRACE_FLOW ,"\nDecoder::set_picsize pic_width=%d,pic_height =%d, dpb_size = %d",	pic_width,pic_height,dpb_size);
    OstTraceFiltInst3( TRACE_FLOW ,"\nDecoder::dec_static.buf.DPBsize=%d,dec_static.buf.curDPBSize=%d,dec_static.buf.AllocatedDPBsize=%d",dec_static.buf.DPBsize,dec_static.buf.curDPBSize,dec_static.buf.AllocatedDPBsize);
    if (mbwidth-1!=dec_static.old_sp.pic_width_in_mbs_minus1 ||
        mbheight-1!=dec_static.old_sp.pic_height_in_map_units_minus1 )
    {
        dec_static.old_sp.level_idc=42;
        dec_static.old_sp.pic_width_in_mbs_minus1=mbwidth-1;
        dec_static.old_sp.pic_height_in_map_units_minus1=mbheight-1;
        dec_static.old_sp.frame_mbs_only_flag=1;
        dec_static.old_sp.level_idc=0;
        dec_static.old_sp.vui.sar_width = 1;   /* square pixel by default */
        dec_static.old_sp.vui.sar_height = 1;

        reallocateMemory=1;
    }
    if(dpb_size!= dec_static.buf.curDPBSize ) {
        // This block syncs current dpb frame number on proxy port with size in decoder
        dec_static.buf.DPBsize = dpb_size;
        dec_static.buf.curDPBSize = dpb_size;

        reallocateMemory=1;
    }

    if(reallocateMemory && !dec_static.buf.memoryAllocationFailed) {
        if (dec_static.buf.AllocatedDPBsize != 0 )
        {
            FreeMemory(vfm_memory_ctxt, &dec_static.buf);
        }

        internal_error = AllocateMemory(vfm_memory_ctxt, mbwidth, mbheight, &dec_static.buf);
        if(internal_error ) {
            FreeMemory(vfm_memory_ctxt, &dec_static.buf);
        }
    }
    if (ack) {
        if(internal_error || dec_static.buf.memoryAllocationFailed)
            pendingCommandAck.pendingCommandAck(INSUFFICIENT_INTERNAL_MEMORY);
        else {
            if(reallocateMemory)
                pendingCommandAck.pendingCommandAck(INTERNAL_MEMORY_ALLOCATED);
            else
                pendingCommandAck.pendingCommandAck(COMMAND_COMPLETED);
        }
    }
}
// -CR324558 CHANGE END OF

void METH(set_par)(t_uint16 ack, t_uint16 par_width, t_uint16 par_height) {
    dec_static.picture_parameters.par_width = par_width;
    dec_static.picture_parameters.par_height = par_height;
    if (ack) {
        pendingCommandAck.pendingCommandAck(COMMAND_COMPLETED);
    }
}

void METH(set_cropping)(    t_uint16 ack,
                            t_uint16 frame_cropping_flag,
                            t_uint16 frame_crop_right,
                            t_uint16 frame_crop_left,
                            t_uint16 frame_crop_top,
                            t_uint16 frame_crop_bottom) {
    dec_static.picture_parameters.frame_cropping_flag = frame_cropping_flag;
    dec_static.picture_parameters.frame_crop_right = frame_crop_right;
    dec_static.picture_parameters.frame_crop_left = frame_crop_left;
    dec_static.picture_parameters.frame_crop_top = frame_crop_top;
    dec_static.picture_parameters.frame_crop_bottom = frame_crop_bottom;
    if (ack) {
        pendingCommandAck.pendingCommandAck(COMMAND_COMPLETED);
    }
}

void METH(set_colorspace)(t_uint16 ack, t_uint16 colorspace) {
    dec_static.picture_parameters.colorspace = colorspace;
    if (ack) {
        pendingCommandAck.pendingCommandAck(COMMAND_COMPLETED);
    }
}

void METH(set_error_map)(t_uint16 ack, t_uint8* error_map) {
    // Reset Error Map and set pointer
    mErrorMap = (Common_Error_Map *)error_map;
    if (ack) {
        pendingCommandAck.pendingCommandAck(COMMAND_COMPLETED);
    }
}

void METH(set_sva_bypass)(t_uint16 ack, t_uint16 sva_bypass) {
    dec_static.sva_bypass = sva_bypass; // 0=none, 1=no dsp call, 2=no codec call (still ddep)
    if (ack) {
        pendingCommandAck.pendingCommandAck(COMMAND_COMPLETED);
    }
}

void METH(picture_parameter_init)() {
    set_cropping(0, 0, 0, 0, 0, 0);
    set_par(0, 1, 1);
    set_colorspace(0, 0);
}

// test if change and store new values
t_sint16 METH(picture_parameter_change)(t_uint16 ack, t_seq_par *p_sp) {
    t_sint16 res=0;
    t_uint32 tempValue = 0;

    /* this code added to handle case of frame_mbs_only_flag = 0 */
	if (dec_static.old_sp.pic_height_in_map_units_minus1 != p_sp->pic_height_in_map_units_minus1)
	{
		if ((!p_sp->frame_mbs_only_flag) && (!p_sp->mb_adaptive_frame_field_flag))
		{
			OstTraceFiltInst1( TRACE_FLOW , "H264DEC: Setting new value of pic_height_in_map_units_minus1 OLD : %d ",p_sp->pic_height_in_map_units_minus1);
			tempValue = ((p_sp->pic_height_in_map_units_minus1 +1) * 2 ) - 1;
		}
		else
		{
			tempValue = p_sp->pic_height_in_map_units_minus1;
		}
	}
	else
	{
		tempValue = p_sp->pic_height_in_map_units_minus1;
	}

	OstTraceFiltInst1( TRACE_FLOW, "H264DEC: FINAL tempValue %d",tempValue);

    if (mSupportedExtension & (1<<0)) {     // VFM_SUPPORTEDEXTENSION_SIZE
        if (!((dec_static.old_sp.pic_width_in_mbs_minus1 == p_sp->pic_width_in_mbs_minus1) &&
                (dec_static.old_sp.pic_height_in_map_units_minus1 == tempValue))) {
            res = 1;
        }
    }

    if (mSupportedExtension & (1<<1)) {     // VFM_SUPPORTEDEXTENSION_PAR
        if (!(
                    (dec_static.picture_parameters.par_width == p_sp->vui.sar_width) &&
                    (dec_static.picture_parameters.par_height == p_sp->vui.sar_height))) {
            res = 1;
            dec_static.picture_parameters.par_width = p_sp->vui.sar_width;
            dec_static.picture_parameters.par_height = p_sp->vui.sar_height;
        }
    }

    if (mSupportedExtension & (1<<2)) {     // VFM_SUPPORTEDEXTENSION_PRIMARYCOLOR
        t_uint32 colorspace = compute_colorspace(p_sp->vui.video_full_range_flag, p_sp->vui.colour_primaries);
        if (!(
             (dec_static.picture_parameters.colorspace == colorspace))) {
                res = 1;
                dec_static.picture_parameters.colorspace = colorspace;
        }
    }

    if (mSupportedExtension & (1<<3)) {     // VFM_SUPPORTEDEXTENSION_CROP
        if (!(
                    (dec_static.picture_parameters.frame_cropping_flag == p_sp->frame_cropping_flag) &&
                    (dec_static.picture_parameters.frame_crop_right == 2*p_sp->frame_crop_right) &&
                    (dec_static.picture_parameters.frame_crop_left == 2*p_sp->frame_crop_left) &&
                    (dec_static.picture_parameters.frame_crop_top == 2*p_sp->frame_crop_top) &&
                    (dec_static.picture_parameters.frame_crop_bottom == 2*p_sp->frame_crop_bottom))) {
            res = 1;
            dec_static.picture_parameters.frame_cropping_flag = p_sp->frame_cropping_flag;
            dec_static.picture_parameters.frame_crop_right = 2*p_sp->frame_crop_right;
            dec_static.picture_parameters.frame_crop_left = 2*p_sp->frame_crop_left;
            dec_static.picture_parameters.frame_crop_top = 2*p_sp->frame_crop_top;
            dec_static.picture_parameters.frame_crop_bottom = 2*p_sp->frame_crop_bottom;
        }
    }
// +CR324558 CHANGE START FOR
    if((!dec_static.bThumbnailGeneration) && (mSupportedExtension & (1<<4))) {  //VFM_SUPPORTEDEXTENSION_DPBSIZE
        OstTraceFiltInst0( TRACE_FLOW ,"\nDecoder::picture_parameter_change DPB Size check.");
        OstTraceFiltInst2( TRACE_FLOW ,"\ngetDpbSize(p_sp)=%d, dec_static.buf.curDPBSize =%d",getDpbSize(p_sp), dec_static.buf.curDPBSize);
        if(getDpbSize(p_sp)>dec_static.buf.curDPBSize) {
            res=1;
        }
    }
// -CR324558 CHANGE END OF

    if (res) {
            // change in picture parameters
#ifndef __ndk5500_a0__
            OstTraceFiltInst0( TRACE_ERROR , "H264DEC: Picture parameters changed\n");
#endif
#if VERBOSE_STANDARD == 1
            NMF_LOG("Picture parameters changed\n");
#endif
    }
    if (ack) {
        pendingCommandAck.pendingCommandAck(COMMAND_COMPLETED);
    }

    return res;
}
