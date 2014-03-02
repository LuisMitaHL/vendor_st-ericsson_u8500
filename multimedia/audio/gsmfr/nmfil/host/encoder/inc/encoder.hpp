/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _gsmfr_nmfil_encoder_hpp_
#define _gsmfr_nmfil_encoder_hpp_

extern "C" {
#include "gsmfr.h"
}
#include "common_interface.h"

class gsmfr_nmfil_host_encoder : public gsmfr_nmfil_host_encoderTemplate
{
public:

  gsmfr_nmfil_host_encoder() {
  	gsmfrenc_config_struct.has_dtx        =0;
  	gsmfrenc_config_struct.payload_format =0; 
  	gsmfrenc_config_struct.memory_preset  =0; 
  	gsmfrenc_config_struct.VC_TX_SP       =0; 
  };

  t_uint16 encodeFrame(void *interface);

  void setParameter(NmfGsmfrEncParams_t params);
  void setConfig(NmfGsmfrEncConfig_t config);

  void open(void *interface);

  void reset(void *interface);

  t_uint16 getMaxFrameSize(void);

  t_uint16 getMaxSamples(void);

  t_uint16 getSampleBitSize(void);
  
  t_uint32 close(void *itf);

private:
  GSMFR_ENCODER_CONFIG_STRUCT_T   gsmfrenc_config_struct;
  
};

#endif // _gsmfr_nmfil_encoder_hpp_


