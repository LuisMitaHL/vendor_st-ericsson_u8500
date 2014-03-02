/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _heaac_h_
#define _heaac_h_

#include "audiolibs_common.h"
#include "common_interface.h"



/*************************** NMFIL PART **************************************/

#define AAC_MAX_BITSTREAM_SIZE_IN_BIT     		12288
#define AAC_SAMPLE_FRAME_SIZE_IN_WORD     		1024
#define EAACPLUS_SAMPLE_FRAME_SIZE_IN_WORD    2048
#define AAC_BITS_PER_SAMPLE               		16       // both 16 and 32 has to be provided as per SHAI 
#define EAACPLUS_MAX_CHANNELS               		2       

/*************************** ENCODER PART **************************************/
typedef enum
{
  HEAAC_PROFILE_AACLC,
  HEAAC_PROFILE_HE,
  HEAAC_PROFILE_HE_PS,
  HEAAC_PROFILE_HE_MPS,
  NUM_HEAAC_PROFILE
}
HEAAC_PROFILE;

typedef enum
{
	HEAAC_BSFORMAT_RAW,
	HEAAC_BSFORMAT_ADTS_MPEG2,
	HEAAC_BSFORMAT_ADTS_MPEG4,
	HEAAC_BSFORMAT_ADIF
}
HEAAC_BS_FORMAT;

typedef  struct {
  int   		    		bitRate;               	/* encoder bit rate in bits/s */
  int  				    	sampleRate;							/* sample rate in Hz */
  int 				    	nChannels;							/* number of input channels */
  int				      	nBits;									/* number of bits per sample */
  HEAAC_PROFILE 		eAacEncProfile;					/* profile of the encoder */
  HEAAC_BS_FORMAT		eAacEncBsFormat;				/* output bit stream format*/
  int								bEncodeMono;		    		/* to force encoding mode to mono/stereo */
}HEAAC_ENCODER_CONFIG;

typedef  struct { 	
  int                   numOutBytes;     
}HEAAC_ENCODER_INFO_STRUCT_T;



#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

AUDIO_API_IMPORT extern void heaac_reset(CODEC_INTERFACE_T *itf);
AUDIO_API_IMPORT extern CODEC_INIT_T heaac_open(CODEC_INTERFACE_T *itf);
AUDIO_API_IMPORT extern RETURN_STATUS_LEVEL_T heaac_encode_frame(CODEC_INTERFACE_T *itf);
AUDIO_API_IMPORT extern void heaac_close(CODEC_INTERFACE_T  *itf);					   
#ifdef __cplusplus
}
#endif // __cplusplus

extern void heaac_initDefaultConfig(CODEC_INTERFACE_T *itf);


#endif //_algo_h_
