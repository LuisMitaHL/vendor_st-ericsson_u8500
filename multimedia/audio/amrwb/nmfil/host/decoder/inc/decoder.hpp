/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _amrwb_nmfil_decoder_hpp_
#define _amrwb_nmfil_decoder_hpp_

#include "amr-wb.h"

class amrwb_nmfil_host_decoder : public amrwb_nmfil_host_decoderTemplate
{
public:

  amrwb_nmfil_host_decoder() {
  	
  	amrwbdec_config_struct.Payload_Format  = 130;
  	amrwbdec_config_struct.concealment_on  = 1;
  	amrwbdec_config_struct.memory_preset   = 0; 
  	amrwbdec_config_struct.noHeader        = 0;
  	amrwbdec_info_struct.used_mode         = 0;
  	};

  virtual void setParameter(AmrwbDecParams_t Params);
  virtual void setConfig(AmrwbDecConfig_t Config);

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
  AMRWB_DECODER_INFO_STRUCT_T    amrwbdec_info_struct;
  AMRWB_DECODER_CONFIG_STRUCT_T  amrwbdec_config_struct;
};

#endif // _amrwb_nmfil_decoder_hpp_


