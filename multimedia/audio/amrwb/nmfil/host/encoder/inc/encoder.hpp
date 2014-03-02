/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _amrwb_nmfil_encoder_hpp_
#define _amrwb_nmfil_encoder_hpp_

#include "amr-wb.h"

class amrwb_nmfil_host_encoder : public amrwb_nmfil_host_encoderTemplate
{
public:

  amrwb_nmfil_host_encoder() {
  	
  	amrwbenc_config_struct.Payload_Format  = 130;
  	amrwbenc_config_struct.dtx             = 1;
  	amrwbenc_config_struct.memory_preset   = 0; 
  	amrwbenc_config_struct.mode            = 0;
  	amrwbenc_config_struct.noHeader        = 0;
  	amrwbenc_info_struct.used_mode         = 0;
  	};

  virtual t_uint16 encodeFrame(void *interface);

  virtual void setParameter(AmrwbEncParam_t params);

  virtual void setConfig(AmrwbEncConfig_t config);

  virtual void open(void *interface);

  virtual void reset(void *interface);

  virtual t_uint16 getMaxFrameSize(void);

  virtual t_uint16 getMaxSamples(void);

  virtual t_uint16 getSampleBitSize(void);

  virtual t_uint32 close(void *interface);

private:
  AMRWB_ENCODER_CONFIG_STRUCT_T   amrwbenc_config_struct;
  AMRWB_ENCODER_INFO_STRUCT_T     amrwbenc_info_struct;
};

#endif // _amrwb_nmfil_encoder_hpp_


