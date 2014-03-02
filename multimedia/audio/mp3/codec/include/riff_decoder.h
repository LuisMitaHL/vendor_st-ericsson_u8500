
 

/* $Id: riff_decoder.h,v 1.5 2004/04/26 15:41:58 marcelr Exp $ */

#ifndef _riff_decoder_h_
#define _riff_decoder_h_

#include "pcmfile.h"
#include "audiolibs_common.h"
#include "common_interface.h"

#ifdef __cplusplus
#define MY_C "C"
#else
#define MY_C
#endif

/* pcm decoder */
extern MY_C CODEC_INIT_T riff_init(CODEC_INTERFACE_T *config);

extern MY_C int  riff_syncword(int hi, int lo, int *length);

extern MY_C void riff_decode_frame(CODEC_INTERFACE_T *config);

//extern MY_C DecoderDescription const DESC_MEM riff_description;

#endif /* Do not edit below this line */

