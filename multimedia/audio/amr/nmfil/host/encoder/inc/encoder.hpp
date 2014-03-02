/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   encoder.hpp
* \brief  AMR Encoder NMF_ARM Component
* \author ST-Ericsson
*/
/*****************************************************************************/
#ifndef _amr_nmfil_encoder_hpp_
#define _amr_nmfil_encoder_hpp_

#include "amr.h"

class amr_nmfil_host_encoder : public amr_nmfil_host_encoderTemplate
{
public:

  amr_nmfil_host_encoder();

  virtual t_uint16 encodeFrame(void *interface);

  virtual void setParameter(AmrEncParam_t params);

  virtual void setConfig(AmrEncConfig_t config);

  virtual void open(void *interface);

  virtual void reset(void *interface);

  virtual t_uint16 getMaxFrameSize(void);

  virtual t_uint16 getMaxSamples(void);

  virtual t_uint16 getSampleBitSize(void);

  virtual t_uint32 close(void *interface);
private:
  AMR_ENCODER_CONFIG_STRUCT_T   amrenc_config_struct;
  AMR_ENCODER_INFO_STRUCT_T     amrenc_info_struct;
};

#endif // _amr_nmfil_encoder_hpp_


