/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef HOST_SH_CONCEALMENT_H
#define HOST_SH_CONCEALMENT_H

#include "types.h"
#include "host_types.h"


#define UE  0
#define SE  1

#define GENERIC             10
#define FIRST_MB_IN_SLICE   1
#define SLICE_TYPE          2
#define PIC_PAR_SET_ID      3
#define FRAME_NUM           4
#define IDR_ID              5
#define MMCO                6
#define NUMREF              7


t_uint16 conceal_sh(t_pic_par *pps, t_seq_par *sp, t_dec_buff *buf, t_slice_hdr *old_sh, t_old_slice *old_sl, t_uint16 next_slice_flag, t_sint16 slice_num, t_slice_hdr *sh);

#endif
