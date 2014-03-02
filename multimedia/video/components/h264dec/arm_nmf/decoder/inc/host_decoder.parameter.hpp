/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

virtual void set_memory_ctxt(t_uint16 ack, void *mem_ctxt);
virtual void set_nParallelDeblocking(t_uint16 ack, t_uint16 nParallelDeblocking);
virtual void set_SupportedExtension(t_uint16 ack, t_uint32 nSupportedExtension);
virtual void set_Framerate(t_uint16 ack, t_uint32 nFramerate);
virtual void set_Thumbnail(t_uint16 ack, t_uint16 bThumbnailGeneration);
virtual void set_ImmediateRelease(t_uint16 ack, t_uint16 bImmediateRelease);
virtual void set_RestrictLevel(t_uint16 ack, t_uint16 level,t_uint16 isCapable);
// +CR324558 CHANGE START FOR
virtual void set_picsize(
						t_uint16 ack,
                        t_uint16 pic_width,
						t_uint16 pic_height,
						t_uint32 dpb_size);
// -CR324558 CHANGE END OF
virtual void set_par(	t_uint16 ack,
                        t_uint16 par_width,
						t_uint16 par_height);
virtual void set_cropping(
						t_uint16 ack,
                        t_uint16 frame_cropping_flag,
						t_uint16 frame_crop_right,
						t_uint16 frame_crop_left,
						t_uint16 frame_crop_top,
						t_uint16 frame_crop_bottom);
virtual void set_colorspace(t_uint16 ack, t_uint16 colorspace);
virtual void set_error_map(t_uint16 ack, t_uint8* error_map);
virtual void set_sva_bypass(t_uint16 ack, t_uint16 sva_bypass);

t_sint16 picture_parameter_change(t_uint16 ack, t_seq_par *p_sp);
void picture_parameter_init();
