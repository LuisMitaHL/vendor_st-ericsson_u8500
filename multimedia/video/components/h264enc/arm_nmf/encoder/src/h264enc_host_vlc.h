/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _HOST_VLC_H_
#define _HOST_VLC_H_

#include "h264enc_host_parset.h"
//#include "types.h"

#define ABS(A) ((A)<(0) ? (-(A)):(A))
#define assert(x)

typedef struct codelement
{
  t_uint32 data; /* UVLC bitpattern */
  t_sint8  nbit; /* length of code */
} CodElement;

t_sint8 host_writeSyntaxElement2Buf_UVLC(CodElement *se, Bitstream* this_streamBuffer);
t_sint8 host_writeSyntaxElement2Buf_Fixed(CodElement *se, Bitstream* this_streamBuffer);

t_sint8 ue_v(t_sint32 value, Bitstream *bitstream);
t_sint8 u_1(t_sint32 value, Bitstream *bitstream);
t_sint8 se_v(t_sint32 value, Bitstream *bitstream);
t_sint8 u_v(t_sint32 n, t_sint32 value, Bitstream *bitstream);

void host_ue_linfo(t_uint16 ue, CodElement *sym);
void host_se_linfo(t_sint16 se, CodElement *sym);

#endif

