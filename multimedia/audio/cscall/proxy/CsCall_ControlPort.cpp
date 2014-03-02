/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   CsCall_ControlPort.cpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "ENS_Component.h"
#include "ENS_Port_Fsm.h"
#include "CsCall_ControlPort.h"
#include "OMX_Symbian_ExtensionNames_Ste.h"
#include "OMX_Symbian_AudioExt_Ste.h"
#include "audio_chipset_api_index.h"

CsCall_ControlPort::CsCall_ControlPort (OMX_U32 nIndex, OMX_DIRTYPE eDir, ENS_Component &enscomp)
    : ENS_Port(EnsCommonPortData(nIndex, 
                                 eDir, 
                                 0, 
                                 //FIXME : minimal size of buffer ?
                                 0/*sizeof(OMX_TIME_MEDIATIMETYPE)*/, 
                                 OMX_PortDomainOther, 
                                 OMX_BufferSupplyUnspecified),
               enscomp)
{
    mParamPortDefinition.format.other.eFormat = OMX_OTHER_FormatTime;

    setMpc(false);
}

OMX_ERRORTYPE CsCall_ControlPort::setFormatInPortDefinition(
                const OMX_PARAM_PORTDEFINITIONTYPE& portDef)
{
    if (portDef.format.other.eFormat != OMX_OTHER_FormatTime) {
        return OMX_ErrorBadParameter;
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE CsCall_ControlPort::checkCompatibility(
                OMX_HANDLETYPE hTunneledComponent,  
                OMX_U32 nTunneledPort) const {
   return OMX_ErrorNone; 
}

void CsCall_ControlPort::drop (t_uint32 us)
{
    OMX_HANDLETYPE tunnelledComp = getTunneledComponent();
    OMX_SYMBIAN_AUDIO_CONFIG_UPLINKTIMINGTYPE uplinkTiming;
    OMX_INDEXTYPE index;
    uplinkTiming.nSize = sizeof(OMX_SYMBIAN_AUDIO_CONFIG_UPLINKTIMINGTYPE);
    getOmxIlSpecVersion(&(uplinkTiming.nVersion));
    uplinkTiming.nDeliveryTime = us;
    uplinkTiming.nModemProcessingTime = 0;
    if (getPortFsm()->getState() == OMX_StateExecuting) {
        if (isEnabled()) {
            if (OMX_GetExtensionIndex(tunnelledComp, (char*)OMX_SYMBIAN_INDEX_CONFIG_AUDIO_UPLINKTIMIMG_NAME, &index) != OMX_ErrorNone) {
                index = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigAudioUplinkTiming;
            }
            if (OMX_SetConfig(tunnelledComp, index,&uplinkTiming) != OMX_ErrorNone) {
                //NMF_LOG("Warning: setConfig for change timing failed !!!");
            }
        }
    }
}


