/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _pcm_parser_h_
#define _pcm_parser_h_

extern void
pcmfile_local_init(struct AudioInfo *info);

extern void
pcmfile_local_decode_frame(CODEC_INTERFACE_T * config);

extern CODEC_INIT_T
pcmfile_init(CODEC_INTERFACE_T * config);

extern void
pcmfile_decode_frame(CODEC_INTERFACE_T * config);

extern int
pcmfile_syncword(int hi, int lo, int *length);

#endif /* Do not edit below this line */
