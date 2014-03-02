/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     SpeechProc_PcmPort.h
 * \brief    OMX part of Speech Proc proxy (port)
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#ifndef _SPEECHPROC_PCMPORT_H_
#define _SPEECHPROC_PCMPORT_H_

#include "AFM_PcmPort.h"

class SpeechProc_PcmPort : public AFM_PcmPort
{
public :
  SpeechProc_PcmPort(OMX_U32 nIndex, OMX_DIRTYPE eDir, OMX_U32 nBufferSizeMin,
					 const OMX_AUDIO_PARAM_PCMMODETYPE &defaultPcmSettings,
					 ENS_Component &enscomp);

  virtual OMX_ERRORTYPE setParameter(OMX_INDEXTYPE nParamIndex,
									 OMX_PTR pComponentParameterStructure);

  virtual OMX_ERRORTYPE checkPcmSettings(const OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettings) const;

private:
  OMX_ERRORTYPE updateSampleFreq(OMX_U32 samplerate, OMX_U32 nb_channel,
								 OMX_U32 bit_per_sample, OMX_U32 previous_buffer_size);
};


#endif // _SPEECHPROC_PCMPORT_H_
