/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _gsmhr_nmfil_encoder_hpp_
#define _gsmhr_nmfil_encoder_hpp_

#include "gsmhr.h"

class gsmhr_nmfil_host_encoder : public gsmhr_nmfil_host_encoderTemplate
{
public:

  gsmhr_nmfil_host_encoder() {
  	
  	gsmhrenc_config_struct.memory_preset  =0;
  	gsmhrenc_config_struct.DTXon          =0;
    gsmhrenc_config_struct.payload_format =0;
    gsmhrenc_config_struct.res_error      =0;
    gsmhrenc_config_struct.VC_TX_VAD      =0;
    gsmhrenc_config_struct.VC_TX_SP       =0;
    gsmhrenc_config_struct.etsi_testing   =0;
    
    };

  virtual t_uint16 encodeFrame(void *interface);

  virtual void setParameter(GsmhrEncParams_t params);
  virtual void setConfig(GsmhrEncConfig_t config);

  virtual void open(void *interface);

  virtual void reset(void *interface);

  virtual t_uint16 getMaxFrameSize(void);

  virtual t_uint16 getMaxSamples(void);

  virtual t_uint16 getSampleBitSize(void);

  t_uint32 close (void *itf);
private:
  GSMHR_ENCODER_CONFIG_STRUCT_T   gsmhrenc_config_struct;

};

#endif // _gsmhr_nmfil_encoder_hpp_


