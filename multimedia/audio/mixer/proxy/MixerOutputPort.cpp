/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   MixerOutputPort.cpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "MixerOutputPort.h"



OMX_ERRORTYPE MixerOutputPort::setParameter(
        OMX_INDEXTYPE nParamIndex,
        OMX_PTR pComponentParameterStructure)
{

    switch (nParamIndex) {
        case OMX_IndexParamAudioPcm:
        {
            if(mIsHost == OMX_FALSE) {
                //Don't need to check PcmParams in case of MPC implementation because
                //it's not necessary to disable input port to change output port settings.
                return AFM_PcmPort::setParameter(nParamIndex, pComponentParameterStructure);
            }

            CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_AUDIO_PARAM_PCMMODETYPE);
            OMX_AUDIO_PARAM_PCMMODETYPE *pcmsettings =(OMX_AUDIO_PARAM_PCMMODETYPE *) pComponentParameterStructure;


            OMX_AUDIO_PARAM_PCMMODETYPE &actualPcmSettings = getPcmSettings();

            // If the sample frequency are not modified
            // we don't need to reject the setParameter command
            if( (actualPcmSettings.nSamplingRate == pcmsettings->nSamplingRate) && (actualPcmSettings.nBitPerSample == pcmsettings->nBitPerSample)){
                return AFM_PcmPort::setParameter(nParamIndex, pComponentParameterStructure);
            }else
            {


                OMX_STATETYPE state;
                OMX_ERRORTYPE error;
                ENS_Component&    myENSComponent=getENSComponent();

                error = myENSComponent.GetState(&myENSComponent,&state);
                if (error != OMX_ErrorNone) return error;

                if(state!= OMX_StateLoaded)
                {
                    OMX_BOOL hasInputEnable = OMX_FALSE;

                    for(OMX_U32 idx=0; idx<myENSComponent.getPortCount()-1; idx++) {
                        if(((ENS_Port *) myENSComponent.getPort(idx+1))->isEnabled()){
                            hasInputEnable = OMX_TRUE;
                        }
                    }

                    if(hasInputEnable == OMX_TRUE){
                        return OMX_ErrorInvalidState;
                    }
                    else{
                        return AFM_PcmPort::setParameter(nParamIndex, pComponentParameterStructure);
                    }
                }
            }

            return AFM_PcmPort::setParameter(nParamIndex, pComponentParameterStructure);
        }

        default:
            return AFM_PcmPort::setParameter(nParamIndex, pComponentParameterStructure);
    }
}
