/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   CsCall_PcmPort.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _CSCALL_PCMPORT_H_
#define _CSCALL_PCMPORT_H_

#include "AFM_PcmPort.h"

class CsCall_PcmPort : public AFM_PcmPort {
  
public: 
  CsCall_PcmPort(OMX_U32 nIndex, OMX_DIRTYPE eDir, OMX_U32 nBufferSizeMin,
				 const OMX_AUDIO_PARAM_PCMMODETYPE &defaultPcmSettings,
				 ENS_Component &enscomp);
  
  virtual OMX_ERRORTYPE setParameter(OMX_INDEXTYPE nParamIndex,
									 OMX_PTR pComponentParameterStructure);
};

#endif // _CSCALL_PCMPORT_H_
