/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \brief   OSI Implementation of Vibra Sink
 * \author  ST-Ericsson
 */
/*****************************************************************************/

#include "alsactrl_audiocodec.h"
#include "Vibra.h"
#include "audiocodec_debug.h"

OMX_ERRORTYPE vibra1FactoryMethod(ENS_Component_p * ppENSComponent)
{
    LOG_I("Enter.\n");

    OMX_ERRORTYPE error = OMX_ErrorNone;

    Vibra *vibra = new Vibra;
    if (vibra == 0) {
        return OMX_ErrorInsufficientResources;
    }
    error = vibra->construct();
    if (error != OMX_ErrorNone) {
        delete vibra;
        *ppENSComponent = 0;

        return OMX_ErrorInsufficientResources;
    }
    else {
        *ppENSComponent = vibra;
    }

    Vibra1NmfMpc *vibra_ProcessingComp = new Vibra1NmfMpc(*vibra, AUDIO_HAL_CHANNEL_VIBRAL);
    if (vibra_ProcessingComp == 0) {
        delete vibra_ProcessingComp;
        return OMX_ErrorInsufficientResources;
    }

    vibra->setProcessingComponent(vibra_ProcessingComp);    
    return error;
}

OMX_ERRORTYPE vibra2FactoryMethod(ENS_Component_p * ppENSComponent) {
    OMX_ERRORTYPE error = OMX_ErrorNone;

    Vibra *vibra = new Vibra;
    if (vibra == 0) {
        return OMX_ErrorInsufficientResources;
    }
    error = vibra->construct();
    if (error != OMX_ErrorNone) {
        delete vibra;
        *ppENSComponent = 0;

        return OMX_ErrorInsufficientResources;
    }
    else {
        *ppENSComponent = vibra;
    }

    Vibra2NmfMpc *vibra_ProcessingComp = new Vibra2NmfMpc(*vibra, AUDIO_HAL_CHANNEL_VIBRAR);
    if (vibra_ProcessingComp == 0) {
        delete vibra_ProcessingComp;
        return OMX_ErrorInsufficientResources;
    }

    vibra->setProcessingComponent(vibra_ProcessingComp);    
    return error;
}

OMX_ERRORTYPE VibraBaseNmfMpc::applyConfig( OMX_INDEXTYPE nConfigIndex,  OMX_PTR pComponentConfigStructure) {
    switch (nConfigIndex) {
        default:
            return AudioRendererAb8500NmfMpc::applyConfig(
                nConfigIndex, pComponentConfigStructure);
    }
}

OMX_ERRORTYPE VibraBaseNmfMpc::retrieveConfig( OMX_INDEXTYPE nConfigIndex,  OMX_PTR pComponentConfigStructure) {
    if (pComponentConfigStructure == 0) {
        return OMX_ErrorBadParameter;
    }

    switch (nConfigIndex) {
        default:
         	return AudioRendererAb8500NmfMpc::retrieveConfig(
                       nConfigIndex, pComponentConfigStructure);
    }
}

OMX_ERRORTYPE VibraBaseNmfMpc::configureMain(void) {
    return AudioRendererAb8500NmfMpc::configureMain();
}

Vibra1NmfMpc * Vibra1NmfMpc::myself = 0;

OMX_ERRORTYPE Vibra1NmfMpc::construct()
{
    myself = this; 
    RETURN_IF_OMX_ERROR( AudioRendererAb8500NmfMpc::construct() );

    return OMX_ErrorNone;	
}

OMX_ERRORTYPE Vibra1NmfMpc::destroy() {
	RETURN_IF_OMX_ERROR( AudioRendererAb8500NmfMpc::destroy() );
    //TODO: check lifetime of CB handler
	myself = static_cast<Vibra1NmfMpc *>(NULL); 
    return OMX_ErrorNone;
}

void Vibra1NmfMpc::statusCB(AUDIO_HAL_STATUS status) {
    if(myself) {
        myself->handleStatusFromAudiohwCtrl(status);
    }
    else {
        //TODO How should we handle this case ????
    }
}


Vibra2NmfMpc * Vibra2NmfMpc::myself = 0;

OMX_ERRORTYPE Vibra2NmfMpc::construct()
{
    myself = this; 
    RETURN_IF_OMX_ERROR( AudioRendererAb8500NmfMpc::construct() );

    return OMX_ErrorNone;	
}

OMX_ERRORTYPE Vibra2NmfMpc::destroy() {
	RETURN_IF_OMX_ERROR( AudioRendererAb8500NmfMpc::destroy() );
    //TODO: check lifetime of CB handler
	myself = static_cast<Vibra2NmfMpc *>(NULL); 
    return OMX_ErrorNone;
}

void Vibra2NmfMpc::statusCB(AUDIO_HAL_STATUS status) {
    if(myself) {
        myself->handleStatusFromAudiohwCtrl(status);
    }
    else {
        //TODO How should we handle this case ????
    }
}

OMX_ERRORTYPE Vibra::construct(void) {
    RETURN_IF_OMX_ERROR( AudioRendererAb8500::construct(NB_CHANNEL_MONO, DO_NOT_SUPPORT_VOLUME_CONFIG, DO_NOT_HAVE_FEEDBACK_PORT) );

	//Setting Haptics Driver Properties

	iHapticsDriverProperties.nPortIndex=0;
	iHapticsDriverProperties.nNativeSamplingRate = 48000;

	//Voltage and Impedance values are needed in order to adopt the driving signal into vendor's HW
	//characteristics
	iHapticsDriverProperties.nHapticsDriverNominalVoltage = 1300; //In mV
	iHapticsDriverProperties.nHapticsDriverImpedance      = 15000;   //In mOhm

    return OMX_ErrorNone;
}

OMX_ERRORTYPE Vibra::getParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure) const {

    if (pComponentParameterStructure == 0) {
        return OMX_ErrorBadParameter;
    }

    switch (nParamIndex)
    {
        case OMX_IndexParamAudioHapticsDriverProperties :      //Vibra driver properties
        {
            OMX_AUDIO_PARAM_HAPTICSDRIVERPROPERTIESTYPE * properties = (OMX_AUDIO_PARAM_HAPTICSDRIVERPROPERTIESTYPE *) pComponentParameterStructure;

            if (properties->nPortIndex != 0) {
                return OMX_ErrorBadPortIndex;
            }

            *properties=iHapticsDriverProperties;

            break;
        }

        default: 
            return AudioRendererAb8500::getParameter(nParamIndex,pComponentParameterStructure);

    }

	return OMX_ErrorNone;
}


