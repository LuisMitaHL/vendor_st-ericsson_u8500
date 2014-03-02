/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
#ifndef _pcm_codec_hpp_
#define _pcm_codec_hpp_

/* type definition decoder instance */


typedef struct PCMCODEC_Dec_Inst_t {
	short  *in_buf;
	short  *out_buf;
} PCM_CODEC_STRUCT_T;



#define PCM_MAX_BITSTREAM_SIZE_IN_BYTES   (640)

#define PCM8K_BITSTREAM_SIZE_IN_BYTES     (320)
#define PCM16K_BITSTREAM_SIZE_IN_BYTES    (640)

#define PCM8K_BITSTREAM_LENGTH   		  (160)
#define PCM16K_BITSTREAM_LENGTH   		  (320)

#define PCM_MAX_BITSTREAM_SIZE_IN_BITS    (PCM_MAX_BITSTREAM_SIZE_IN_BYTES * 8)
#define PCM_SAMPLE_FRAME_SIZE_IN_WORDS    (320)
#define PCM_BITS_PER_SAMPLE               (16)
#define PCM_NUMBER_OF_SAMPLES_IN_FRAME    (320)

#define PCM_DECODER_MAX_PCM_CHANNELS      16




#endif // _pcm_codec_hpp_







