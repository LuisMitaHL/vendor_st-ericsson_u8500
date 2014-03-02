/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _gsmfr_nmfil_decoder_hpp_
#define _gsmfr_nmfil_decoder_hpp_

extern "C" {
#include "gsmfr.h"
}

class gsmfr_nmfil_host_decoder : public gsmfr_nmfil_host_decoderTemplate
{
public:

  gsmfr_nmfil_host_decoder() {
  	gsmfrdec_config_struct.has_dtx        =0;
  	gsmfrdec_config_struct.payload_format =0; 
  	gsmfrdec_config_struct.memory_preset  =0; 
  	gsmfrdec_config_struct.VC_RX_SP       =0;
  	gsmfrdec_config_struct.VC_RX_SID      =0;
  	gsmfrdec_config_struct.VC_RX_TAF      =0;
  	gsmfrdec_config_struct.VC_RX_BFI      =0;
  	};

  t_uint16 checkSynchro(t_uint24 msp, t_uint24 lsp, t_sint32 *framesize_p);
  
  t_uint16 decodeFrame(void *interface);

  void open(void *interface);

  void reset(void *interface);
  
  void setParameter(NmfGsmfrDecParams_t Params);
    
  void setConfig(NmfGsmfrDecConfig_t config);
  
  void setOutputBuf(void *interface, void *buf);

  t_sint32 getMaxFrameSize(void);
  
  void getOutputGrain(t_uint16 *blocksize, t_uint16 *maxchans);

  t_uint16 getHeaderType(void);

  t_uint16 getSampleBitSize(void);

  t_sint32 getBlockSize(void);

  t_uint32 close (void *itf);
private:
   GSMFR_DECODER_CONFIG_STRUCT_T  gsmfrdec_config_struct;
};

#endif // _gsmfr_nmfil_decoder_hpp_
