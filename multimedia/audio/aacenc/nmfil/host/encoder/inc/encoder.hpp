/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef _aacenc_nmfil_encoder_hpp_
#define _aacenc_nmfil_encoder_hpp_

#include "common_interface.h"
#include "heaacencode.h"


class aacenc_nmfil_host_encoder : public aacenc_nmfil_host_encoderTemplate
{
public:

  aacenc_nmfil_host_encoder() {
  	heaac_config.bEncodeMono = 0;
		heaac_config.bitRate = 128000;
		heaac_config.eAacEncBsFormat = HEAAC_BSFORMAT_ADTS_MPEG2;
		heaac_config.eAacEncProfile = HEAAC_PROFILE_AACLC ;
		heaac_config.nBits = 16 ;
		heaac_config.nChannels = 2;
		heaac_config.sampleRate = 48000; 
		heaac_info_config.numOutBytes = 0;
  	
  	};

  virtual t_uint16 encodeFrame(void *interface);

  virtual void setParameter(AacEncParam_t params);

//  virtual void setConfig(AacEncConfig_t config);

  virtual void open(void *interface);

  virtual void reset(void *interface);

  virtual t_uint16 getMaxFrameSize(void);

  virtual t_uint16 getMaxSamples(void);

  virtual t_uint16 getSampleBitSize(void);

  virtual t_uint32 close(void *interface);

private:
				HEAAC_ENCODER_CONFIG heaac_config;
				HEAAC_ENCODER_INFO_STRUCT_T heaac_info_config;

};

#endif // _aacenc_nmfil_encoder_hpp_


