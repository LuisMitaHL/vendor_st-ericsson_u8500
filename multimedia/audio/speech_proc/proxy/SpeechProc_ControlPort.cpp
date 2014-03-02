/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     SpeechProc_ControlPort.cpp
 * \brief    OMX part of Speech Proc proxy (port)
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#include "SpeechProc_ControlPort.h"
#include "speech_proc_omx_interface.h"
#include "AFM_Symbian_AudioExt.h"

SpeechProc_ControlPort::SpeechProc_ControlPort(ENS_Component &enscomp)
  :ENS_Port(EnsCommonPortData(OUTPUT_CONTROL_PORT,
                              OMX_DirOutput,
                              0,
                              0,
                              OMX_PortDomainOther,
                              OMX_BufferSupplyUnspecified),
            enscomp)
{
  mParamPortDefinition.format.other.eFormat = OMX_OTHER_FormatBinary;
  setMpc(false);
}


OMX_ERRORTYPE SpeechProc_ControlPort::setFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE& portDef)
{
  if (portDef.format.other.eFormat != OMX_OTHER_FormatBinary) {
    return OMX_ErrorBadParameter;
  }
  return OMX_ErrorNone;
}


OMX_ERRORTYPE SpeechProc_ControlPort::checkCompatibility(OMX_HANDLETYPE hTunneledComponent,
                                                         OMX_U32 nTunneledPort) const
{
  OMX_ERRORTYPE err;
  OMX_PARAM_PORTDEFINITIONTYPE tunneledPortDef;

  tunneledPortDef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
  tunneledPortDef.nPortIndex = nTunneledPort;
  getOmxIlSpecVersion(&tunneledPortDef.nVersion);

  err = OMX_GetParameter(hTunneledComponent, OMX_IndexParamPortDefinition, &tunneledPortDef);
  if (err != OMX_ErrorNone) return err;

  if (tunneledPortDef.eDir != OMX_DirInput) {
    return OMX_ErrorPortsNotCompatible;
  }

  if (tunneledPortDef.eDomain != OMX_PortDomainOther) {
    return OMX_ErrorPortsNotCompatible;
  }

  if (tunneledPortDef.format.other.eFormat != OMX_OTHER_FormatBinary) {
    return OMX_ErrorPortsNotCompatible;
  }

  return OMX_ErrorNone;
}


void SpeechProc_ControlPort::applyNewSidetoneGain(t_sint32 gain)
{
  if (isEnabled())
  {
	OMX_HANDLETYPE tunnelledComp = getTunneledComponent();

	if (tunnelledComp != 0)
	{
	  OMX_INDEXTYPE index;
	  if (OMX_GetExtensionIndex(tunnelledComp, (char*)OMX_SYMBIAN_INDEX_CONFIG_AUDIO_SIDETONEATTENUATION_NAME, &index) != OMX_ErrorNone)
	  {
		OMX_AUDIO_CONFIG_SIDETONEATTENUATIONINFOTYPE sidetoneconfig;
		sidetoneconfig.nSize = sizeof(OMX_AUDIO_CONFIG_SIDETONEATTENUATIONINFOTYPE);
		getOmxIlSpecVersion(&(sidetoneconfig.nVersion));

		sidetoneconfig.nPortIndex     = 0;
		sidetoneconfig.bLinear        = OMX_FALSE;
		sidetoneconfig.sVolume.nValue = gain;
		sidetoneconfig.sVolume.nMin   = -6000; // -60dB
		sidetoneconfig.sVolume.nMax   =  3700; //  37dB

		if (OMX_SetConfig(tunnelledComp, index, &sidetoneconfig) != OMX_ErrorNone) {
		  //NMF_LOG("Warning: setConfig for change sidetone attenuation failed !!!");
		}
      }
	}
  }
}
