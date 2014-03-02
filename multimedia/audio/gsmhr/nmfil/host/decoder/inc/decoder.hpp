/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _gsmhr_nmfil_decoder_hpp_
#define _gsmhr_nmfil_decoder_hpp_

#include "gsmhr.h"

class gsmhr_nmfil_host_decoder : public gsmhr_nmfil_host_decoderTemplate
{
public:

  gsmhr_nmfil_host_decoder() {
  	
  	gsmhrdec_config_struct.memory_preset  =0;
  	gsmhrdec_config_struct.payload_format =0;
    gsmhrdec_config_struct.VC_RX_SP       =0;
    gsmhrdec_config_struct.VC_RX_SID      =0;
    gsmhrdec_config_struct.VC_RX_TAF      =0;
    gsmhrdec_config_struct.VC_RX_UFI      =0;
    gsmhrdec_config_struct.VC_RX_BFI      =0;
    gsmhrdec_config_struct.etsi_testing   =0;
};

  virtual void setParameter(GsmhrDecParams_t Params);
  virtual void setConfig(GsmhrDecConfig_t config);

  virtual t_uint16 checkSynchro(t_uint24 msp, t_uint24 lsp, t_sint32 *framesize_p);
  virtual t_uint16 decodeFrame(void *interface);

  virtual void open(void *interface);

  virtual void reset(void *interface);
  
  virtual void setOutputBuf(void *interface, void *buf);

  t_sint32 getMaxFrameSize(void);
  
  virtual void getOutputGrain(t_uint16 *blocksize, t_uint16 *maxchans);

  virtual t_uint16 getHeaderType(void);

  virtual t_uint16 getSampleBitSize(void);

  t_sint32 getBlockSize(void);

  t_uint32 close (void *itf);
private:
  GSMHR_DECODER_CONFIG_STRUCT_T  gsmhrdec_config_struct;
};

#endif // _gsmhr_nmfil_decoder_hpp_


