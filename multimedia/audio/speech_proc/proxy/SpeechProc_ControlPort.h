/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     SpeechProc_ControlPort.h
 * \brief    OMX part of Speech Proc proxy (port)
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#ifndef _SPEECHPROC_CONTROLPORT_H_
#define _SPEECHPROC_CONTROLPORT_H_

#include "ENS_Port.h"

class SpeechProc_ControlPort : public ENS_Port
{
 public :
  SpeechProc_ControlPort(ENS_Component &enscomp);
  void applyNewSidetoneGain(t_sint32 gain);

 protected:
  // inherited frome ENS_port
  virtual OMX_ERRORTYPE setFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE& portDef);

  virtual OMX_ERRORTYPE checkCompatibility(OMX_HANDLETYPE hTunneledComponent,
										   OMX_U32 nTunneledPort) const;
};


#endif // _SPEECHPROC_CONTROLPORT_H_
