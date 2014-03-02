/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
#ifndef _pcm_dec_hpp_
#define _pcm_dec_hpp_

#include "Component.h"
#include "common_interface.h"
#include "cscall_codec.h"

// class cscall_nmf_host_pcm_codec_pcm_dec : public cscall_nmf_host_pcm_codec_pcm_decTemplate

class cscall_nmf_host_pcm_codec_pcm_dec : public cscall_nmf_host_pcm_codec_pcm_decTemplate
{
public:

  cscall_nmf_host_pcm_codec_pcm_dec();

   // Instantiated functions
  virtual void setConfig(PcmCodecConfig_t config);

  virtual t_uint16 checkSynchro(t_uint24 msp, t_uint24 lsp, t_sint32 *framesize_p);

  virtual t_uint16 decodeFrame(void *interface);

  virtual void open(void *interface);

  virtual void reset(void *interface);

  virtual void setOutputBuf(void *interface, void *buf);

  virtual t_sint32 getMaxFrameSize(void);

  virtual void getOutputGrain(t_uint16 *blocksize, t_uint16 *maxchans);

  virtual t_uint16 getHeaderType(void);

  virtual t_uint16 getSampleBitSize(void);

  virtual t_sint32 getBlockSize(void);

  virtual t_uint32 close(void *interface);


private:
	void copyAndSwapEndianess(unsigned char * buffer_out,const unsigned char * buffer_in,t_uint16 size);
    bool big_endian_flag;
	PcmCodecSamplingRate_e  pcmdec_sampling_rate;
};

#endif // _pcm_dec_hpp_







