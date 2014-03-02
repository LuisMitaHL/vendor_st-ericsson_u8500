/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef HOST_FRAMEINFO_H
#define HOST_FRAMEINFO_H

#include "types.h"
#include "host_types.h"
#include "OMX_Video.h"

t_uint16 getDpbSize(t_seq_par *p_sp); // in host_alloc.c

static inline t_uint16 get_profile(t_seq_par *p_sp) {
	switch (p_sp->profile_idc) {
		case 66 : return OMX_VIDEO_AVCProfileBaseline; // h264 std A.2.1
		case 77 : return OMX_VIDEO_AVCProfileMain; // h264 std A.2.2
		case 88 : return OMX_VIDEO_AVCProfileExtended; // h264 std A.2.3
		case 100 : return OMX_VIDEO_AVCProfileHigh;  // h264 std A.2.4
		case 110 : return OMX_VIDEO_AVCProfileHigh10;  // h264 std A.2.5
		case 122 : return OMX_VIDEO_AVCProfileHigh422;  // h264 std A.2.6
		case 144 : return OMX_VIDEO_AVCProfileHigh444;  // h264 std A.2.7
	}
	NMF_PANIC("\nERROR: unknown profile_idc.\n");
	return 0;
}

static inline t_uint16 get_level(t_seq_par *p_sp) {
	// h264 std table A1 :
	// If level_idc is equal to 11 and constraint_set3_flag is equal to 1, the indicated level is level 1b.
	// Otherwise (level_idc is not equal to 11 or constraint_set3_flag is not equal to 1), level_idc shall be set equal to a
	// value of ten times the level number specified in Table A-1 and constraint_set3_flag shall be set equal to 0.
	switch (p_sp->level_idc) {
	  case 9:  return OMX_VIDEO_AVCLevel1b;
	  case 10: return OMX_VIDEO_AVCLevel1; 
    case 11: 
		if (p_sp->constraint_flags && 0x8 /* bit 3 */)
			return OMX_VIDEO_AVCLevel1b;
		else
			return OMX_VIDEO_AVCLevel11; 
    case 12: return OMX_VIDEO_AVCLevel12; 
    case 13: return OMX_VIDEO_AVCLevel13; 
    case 20: return OMX_VIDEO_AVCLevel2; 
    case 21: return OMX_VIDEO_AVCLevel21; 
    case 22: return OMX_VIDEO_AVCLevel22; 
    case 30: return OMX_VIDEO_AVCLevel3; 
    case 31: return OMX_VIDEO_AVCLevel31; 
    case 32: return OMX_VIDEO_AVCLevel32; 
    case 40: return OMX_VIDEO_AVCLevel4; 
    case 41: return OMX_VIDEO_AVCLevel41; 
    case 42: return OMX_VIDEO_AVCLevel42; 
    case 50: return OMX_VIDEO_AVCLevel5; 
    case 51: return OMX_VIDEO_AVCLevel51; 
    }
	NMF_PANIC("\nERROR: unknown level_idc.\n");
	return 0;
}


static inline t_uint16 get_LoopFilterMode(t_slice_hdr *p_sh) {
	// h264 std 8.7
	//when disable_deblocking_filter_idc is equal to 2, macroblocks in different slices are considered not available during specified steps of the operation of the deblocking filter process.
	switch (p_sh->disable_deblocking_filter_idc) {
		case 0 : return OMX_VIDEO_AVCLoopFilterEnable;
		case 1 : return OMX_VIDEO_AVCLoopFilterDisable;
		case 2 : return OMX_VIDEO_AVCLoopFilterDisableSliceBoundary;
    }
	NMF_PANIC("\nERROR: unknown disable_deblocking_filter_idc.\n");
	return 0;
}

static inline t_uint32 compute_colorspace(t_uint16 video_full_range_flag, t_uint16 colour_primaries)
{
    /* this is the definition in the video_chipset_api
        typedef enum OMX_COLORPRIMARYTYPE  {
            OMX_ColorPrimaryUnknown=0,          // Default value
            OMX_ColorPrimaryBT601FullRange,     // YUV range from 0 to 255 with BT.601 defined YUV to RGB color conversion matrix
            OMX_ColorPrimaryBT601LimitedRange,  // YUV range from 16 to 235 or 240 for chrominance with BT.601 defined YUV to RGB color conversion matrix
            OMX_ColorPrimaryBT709FullRange,     // YUV range from 0 to 255 with BT.709 defined YUV to RGB color conversion matrix
            OMX_ColorPrimaryBT709LimitedRange   // YUV range from 16 to 235 or 240 for chrominance with BT.709 defined YUV to RGB color conversion matrix
        } OMX_COLORPRIMARYTYPE;
     */
    // Cf. the Video SAS, section "8.2 Color ranges"
    if (video_full_range_flag==0) {
        if (colour_primaries==1) {
            return 4;   // OMX_ColorPrimaryBT709LimitedRange;
        } else {
            return 2;   // OMX_ColorPrimaryBT601LimitedRange;
        }
    } else {
        if (colour_primaries==1) {
            return 3;   // OMX_ColorPrimaryBT709FullRange;
        } else {
            return 1;   // OMX_ColorPrimaryBT601FullRange;
        }
    }
}

static inline void set_frameinfo(t_frameinfo *info, t_seq_par *p_sp, t_slice_hdr *p_sh, 
                                 t_uint32 nTimeStampH, t_uint32 nTimeStampL,
                                 t_uint16 current_3d_format) {
    if (p_sp) {
    	info->common_frameinfo.eProfile					= get_profile(p_sp);
    	info->common_frameinfo.eLevel					= get_level(p_sp);
    	//info->specific_frameinfo.bDirect8x8Inference 	= p_sh->direct_8x8_inf_flag; // h264 std 7.4.2.1
    	//info->specific_frameinfo.nCabacInitIdc		= p_sh->cabac_init_idc;
    	info->specific_frameinfo.eLoopFilterMode		= get_LoopFilterMode(p_sh);
    	info->common_frameinfo.pic_width				= (p_sp->pic_width_in_mbs_minus1+1)*16;
        info->common_frameinfo.pic_height				= (p_sp->pic_height_in_map_units_minus1+1)*16*(2-p_sp->frame_mbs_only_flag);
    	info->common_frameinfo.dpb_size					= getDpbSize(p_sp);
    	info->common_frameinfo.frame_cropping_flag		= p_sp->frame_cropping_flag;
    	info->common_frameinfo.frame_crop_right			= 2*p_sp->frame_crop_right;
    	info->common_frameinfo.frame_crop_left			= 2*p_sp->frame_crop_left;
    	info->common_frameinfo.frame_crop_top			= 2*p_sp->frame_crop_top;
    	info->common_frameinfo.frame_crop_bottom		= 2*p_sp->frame_crop_bottom;
    	
        info->common_frameinfo.nTimeStampH 				= nTimeStampH;
        info->common_frameinfo.nTimeStampL 				= nTimeStampL;
        info->common_frameinfo.nPixelAspectRatioWidth   = p_sp->vui.sar_width;
        info->common_frameinfo.nPixelAspectRatioHeight  = p_sp->vui.sar_height;
        info->common_frameinfo.nColorPrimary            = compute_colorspace(p_sp->vui.video_full_range_flag, p_sp->vui.colour_primaries);
		info->specific_frameinfo.pErrorMap				= 0; /* could be set to pErrorMap, but wrong type */
        info->specific_frameinfo.n3dFormat              = current_3d_format;
    }
}

#endif // HOST_FRAMEINFO_H
