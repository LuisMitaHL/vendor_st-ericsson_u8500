/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   decoder.hpp
* \brief  AMR NMF_ARM Decoder
* \author ST-Ericsson
*/
/*****************************************************************************/
#ifndef _amr_nmfil_decoder_hpp_
#define _amr_nmfil_decoder_hpp_

#include "amr.h"

class amr_nmfil_host_decoder : public amr_nmfil_host_decoderTemplate
{
public:

  amr_nmfil_host_decoder();

  virtual void setParameter(AmrDecParams_t Params);
  virtual void setConfig(AmrDecConfig_t Config);

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
  AMR_DECODER_INFO_STRUCT_T    amrdec_info_struct;
  AMR_DECODER_CONFIG_STRUCT_T  amrdec_config_struct;
};

#endif // _amr_nmfil_decoder_hpp_


