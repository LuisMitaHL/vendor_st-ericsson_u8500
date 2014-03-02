/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef HOST_DPB_H
#define HOST_DPB_H


#include "types.h"
#include "host_types.h"
#include "host_display_queue.h"

#ifndef OMX_TICKS_PER_SECOND
#ifdef __SYMBIAN32__
typedef unsigned long long OMX_U64;
typedef signed long long OMX_S64;
#elif defined(WIN32)
typedef unsigned __int64  OMX_U64;
typedef signed   __int64  OMX_S64;
#else /* WIN32 */
typedef unsigned long long OMX_U64;
typedef signed long long OMX_S64;
#endif /* WIN32 */
#define OMX_TICKS_PER_SECOND 1000000
#endif

static inline void set_TimeStamp(t_dec_pic_info *curr_info, t_uint32 *nTimeStampH, t_uint32 *nTimeStampL)
{
#if ENABLE_TIMING == 1
	// display_time defined only with ENABLE_TIMING
    if (*nTimeStampH==0 && *nTimeStampL==0 && curr_info!=0) {
        OMX_S64 tmp = (OMX_S64)(curr_info->display_time * OMX_TICKS_PER_SECOND);
        *nTimeStampH = (tmp>>32) & 0xFFFFFFFF;
        *nTimeStampL = (tmp    ) & 0xFFFFFFFF;
    }
#endif
}

void InitBuffer(t_dec_buff *p_buff);
void FlushDPB(t_seq_par *p_sp, t_dec_buff *p_buff, t_frames *keep_frame);
void NewIDR(t_seq_par *p_sp, t_slice_hdr *p_sh, t_dec_buff *p_buff,t_SEI*sei);
t_uint16 NewFrame(t_seq_par *p_sp, t_slice_hdr *p_sh, t_dec_buff *p_buff, t_uint32 nTimeStampH, t_uint32 nTimeStampL, t_uint32 nFlags,t_SEI*sei);
void MarkPicture(t_seq_par *p_sp, t_slice_hdr *p_sh, t_dec_buff *p_buff);
t_uint16 GenerateList(t_seq_par *p_sp, t_pic_par *p_pp, t_slice_hdr *p_sh, t_dec_buff *p_buff);

//void InsertDummyRefIntoDPB(t_dec_buff *p_buff, t_uint16 prevNum, t_seq_par *p_sp);

#endif
