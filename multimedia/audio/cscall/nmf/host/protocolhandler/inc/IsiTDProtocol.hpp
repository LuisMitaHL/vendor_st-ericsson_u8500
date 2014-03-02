/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   IsiProtocol.hpp
 * \brief  implementation of Protocol class for ISI protocol.
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef __IsiTDProtocolhpp
#define __IsiTDProtocolhpp

#include "cscall/nmf/host/protocolhandler/inc/IsiProtocol.hpp"

class IsiTDProtocol : public IsiProtocol
{
public:
  IsiTDProtocol(void);
  virtual ~IsiTDProtocol() {};
  
  virtual bool process_speech_data_downlink(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE *decoder);

  virtual bool process_speech_data_uplink(OMX_BUFFERHEADERTYPE *encoder_buffer, OMX_BUFFERHEADERTYPE *msg,
										  Cscall_Codec_Config_t *codec_config, Cscall_Codec_Status_t *codec_status,
										  bool *config_updated);

protected:
  virtual void     getCodecAndNetwork(IsiMessage &isi_msg, ISI_CODEC_NAME_T &codec, ISI_NETWORK_NAME_T &network);
  virtual t_uint32 getTimingValue(IsiMessage &isi_msg, t_uint16 &response);
  
private:
  bool             mIsFirstTimingReq;
  ISI_CODEC_NAME_T mRealCodecInUse;
};

#endif // __IsiProtocolhpp
