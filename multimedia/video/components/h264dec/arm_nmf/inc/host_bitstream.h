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

typedef enum {
  SCALING_LIST_4x4 = 0,
  SCALING_LIST_8x8
} te_ScalingListType;

t_uint16 FindNextNALU(t_bit_buffer *p_b, t_uint16 *size, t_uint32 *p_next);
t_uint16 GetNALUType(t_bit_buffer *p_b, t_uint16 *p_nal_ref_idc, t_uint16 *p_nal_unit_type, t_uint16 is_mvc_stream);
t_uint16 GetSequenceParSet(t_bit_buffer *p_b, t_sint16 *p_nsp, t_seq_par *p_s, t_seq_par *tmp_sps, t_uint16 * pending_sps_update);
t_uint16 GetSubsetSequenceParSet(t_bit_buffer *p_b, t_sint16 *p_nsubsetsp, t_subset_seq_par *p_subset_sp, t_uint16 * is_mvc_stream);
t_uint16 GetPictureParSet(t_bit_buffer *p_b, t_sint16 *p_npp, t_pic_par *p_p,t_sint16 *p_nsp, t_seq_par *p_s);
t_uint16 GetSliceHeader(t_bit_buffer *p_b, t_dec_buff *buf, void *p_q, t_pic_par *p_p, t_uint16 num_pic_par, 
                        t_uint16 num_seq_par, t_uint16 num_subset_seq_par,
                        t_uint16 first_slice, t_old_slice *p_old_sl, t_slice_hdr *p_s);
t_uint16 isNewPicture(t_slice_hdr *p_sh, t_old_slice *p_old_sl, t_uint16 rs_flag);
t_uint16 isNewPictureRS(t_slice_hdr *p_sh, t_old_slice *p_old_sl);
void InitOldSlice(t_old_slice *old_sl);
void saveOldPar(t_slice_hdr *p_sh, t_old_slice *p_old_sl);

#if VERBOSE_BITSTREAM > 0
#define OstTraceFiltInst0(a,b)       NMF_LOG(b)
#define OstTraceFiltInst1(a,b,c)     NMF_LOG(b,c)
#define OstTraceFiltInst2(a,b,c,d)   NMF_LOG(b,c,d)
#define OstTraceFiltInst3(a,b,c,d,e) NMF_LOG(b,c,d,e)
#endif
 
#ifdef __cplusplus
}
#endif

#endif
