/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef HOST_BITSTREAM_H
#define HOST_BITSTREAM_H

#include "types.h"
#include "host_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OK      0
#define ERR     1
#define DROP    2
#define RS      3
#define STOP    4

#define SOP     1

#define NAL_ERROR	1
#define NAL_LAST	2
#define NAL_OK		0

t_uint16 FindNextNALU(t_bit_buffer *p_b, t_uint16 *size, t_uint32 *p_next);
t_uint16 GetNALUType(t_bit_buffer *p_b, t_uint16 *p_nal_ref_idc, t_uint16 *p_nal_unit_type);
t_uint16 GetSequenceParSet(t_bit_buffer *p_b, t_sint16 *p_nsp, t_seq_par *p_s);
t_uint16 GetPictureParSet(t_bit_buffer *p_b, t_sint16 *p_npp, t_pic_par *p_p);
t_uint16 GetSliceHeader(t_bit_buffer *p_b, t_dec_buff *buf, t_seq_par *p_q, t_pic_par *p_p, t_uint16 num_pic_par, t_uint16 num_seq_par, t_uint16 first_slice, t_old_slice *p_old_sl, t_slice_hdr *p_s);
t_uint16 isNewPicture(t_slice_hdr *p_sh, t_old_slice *p_old_sl, t_uint16 rs_flag);
t_uint16 isNewPictureRS(t_slice_hdr *p_sh, t_old_slice *p_old_sl);
void InitOldSlice(t_old_slice *old_sl);
void saveOldPar(t_slice_hdr *p_sh, t_old_slice *p_old_sl);

 
#ifdef __cplusplus
}
#endif

#endif
