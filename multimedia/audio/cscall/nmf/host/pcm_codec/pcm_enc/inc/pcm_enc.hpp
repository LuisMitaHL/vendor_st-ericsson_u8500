/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
#ifndef _pcm_enc_hpp_
#define _pcm_enc_hpp_

#include "Component.h"
#include "common_interface.h"
#include "cscall_codec.h"

// TBD class cscall_nmf_host_pcm_codec_pcm_enc : public TraceObject, cscall_nmf_host_pcm_codec_pcm_encTemplate

class cscall_nmf_host_pcm_codec_pcm_enc : public cscall_nmf_host_pcm_codec_pcm_encTemplate
{
public:

	  cscall_nmf_host_pcm_codec_pcm_enc();

	  // Other function
	  virtual t_uint16 encodeFrame(void *interface);

	  virtual void setConfig(PcmCodecConfig_t config);

	  virtual void open(void *interface);

	  virtual void reset(void *interface);

	  virtual t_uint16 getMaxFrameSize(void);

	  virtual t_uint16 getMaxSamples(void);

	  virtual t_uint16 getSampleBitSize(void);

	  virtual t_uint32 close(void *interface);


private:
	void copyAndSwapEndianess(unsigned char * buffer_out,const unsigned char * buffer_in,t_uint16 size);
	bool big_endian_flag;
	PcmCodecSamplingRate_e  pcmdec_sampling_rate;
};

#endif // _pcm_enc_hpp_







