/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "BinarySplitter_PcmPort.h"
#include "AFM_Types.h"
#include "AFM_Index.h"
#include "AFM_PcmPort.h"

BinarySplitter_PcmPort::BinarySplitter_PcmPort(
    OMX_U32 nIndex,
    OMX_DIRTYPE eDir,
    OMX_U32 nBufferSizeMin,
    const OMX_AUDIO_PARAM_PCMMODETYPE &defaultPcmSettings,
    ENS_Component &enscomp)
        : AFM_PcmPort(
            nIndex, eDir, nBufferSizeMin, defaultPcmSettings, enscomp),
        mBinarySplitterSettings(defaultPcmSettings)
{
#ifndef STE_PLATFORM_U5500
    mIsSynchronized=OMX_FALSE;
#endif

    mBinarySplitterSettings                = defaultPcmSettings;
    mBinarySplitterSettings.nSize          = sizeof(OMX_AUDIO_PARAM_PCMMODETYPE);
    getOmxIlSpecVersion(&mBinarySplitterSettings.nVersion);
    mBinarySplitterSettings.nPortIndex     = getPortIndex();
    mBinarySplitterSettings.eChannelMapping[0] = OMX_AUDIO_ChannelLF;
    mBinarySplitterSettings.eChannelMapping[1] = OMX_AUDIO_ChannelRF;
}

OMX_ERRORTYPE BinarySplitter_PcmPort::setParameter(
    OMX_INDEXTYPE nParamIndex,
    OMX_PTR pComponentParameterStructure)
{
    OMX_ERRORTYPE error;
    OMX_U8 i=0, mFlag=0;
    OMX_U8 portcount = (OMX_U8) getENSComponent().getPortCount();
    switch (nParamIndex) {
    case OMX_IndexParamAudioPcm:
    {
        OMX_U8 cActiveRole[OMX_MAX_STRINGNAME_SIZE], cRole[OMX_MAX_STRINGNAME_SIZE];
        error = getENSComponent().getActiveRole(cActiveRole);
        if (error != OMX_ErrorNone) return error;

        ENS_String<25> role = "audio_splitter.binary";
        role.put((OMX_STRING)cRole);

        while (cActiveRole[i] != '\0' && i < OMX_MAX_STRINGNAME_SIZE) {
            if (cRole[i] != cActiveRole[i])
            {
                mFlag = 1;
                break;
            }
            i++;
        }

        OMX_AUDIO_PARAM_PCMMODETYPE *pcmSettings =
            (OMX_AUDIO_PARAM_PCMMODETYPE *)pComponentParameterStructure;
        BinarySplitter_PcmPort *pcmPort = (BinarySplitter_PcmPort *) getENSComponent().getPort(pcmSettings->nPortIndex);

#if 0 //to be used after changes in test setup
        // for channelswitch, o/p port settings can be different from i/p port
        if (mFlag == 1)
        {
            error = pcmPort->updateBinarySplitterSettings(
                        nParamIndex, pComponentParameterStructure);
            if (error != OMX_ErrorNone) return error;
        }
        else
        {
            if (pcmSettings->nPortIndex == 0) {
                //update i/p port settings
                error = pcmPort->updateBinarySplitterSettings(
                            nParamIndex, pComponentParameterStructure);
                if (error != OMX_ErrorNone) return error;
                //propagate input port settings to output ports
                for (i = 1; i < portcount; i++)
                {
                    BinarySplitter_PcmPort *outputPort = (BinarySplitter_PcmPort *) getENSComponent().getPort(i);
                    error = outputPort->updateBinarySplitterSettings(
                                nParamIndex, pComponentParameterStructure);
                    if (error != OMX_ErrorNone) return error;
                }
            }
            else {
                OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettingsIn =
                    ((AFM_PcmPort *) getENSComponent().getPort(0))->getPcmSettings();
                //check if o/p port settings match that of i/p port, else return error
                if ((pcmSettings->nSamplingRate != pcmSettingsIn.nSamplingRate) ||
                        (pcmSettings->nChannels != pcmSettingsIn.nChannels) ||
                        (pcmSettings->nBitPerSample != pcmSettingsIn.nBitPerSample)){
                    return OMX_ErrorBadParameter;
                }
            }
        }
#else
    error = pcmPort->updateBinarySplitterSettings(
                nParamIndex, pComponentParameterStructure);
    if (error != OMX_ErrorNone) return error;

	  if (mFlag == 0)
	  {
	      if (pcmSettings->nPortIndex == 0)
	      {
            //propagate input port settings to output ports
            for (i = 1; i < portcount; i++)
            {
                BinarySplitter_PcmPort *outputPort = (BinarySplitter_PcmPort *) getENSComponent().getPort(i);
                error = outputPort->updateBinarySplitterSettings(
                            nParamIndex, pComponentParameterStructure);
                if (error != OMX_ErrorNone) return error;
            }
        }
    }
#endif

        return OMX_ErrorNone;
    }

    case OMX_IndexParamAudioSynchronized:
    {
        CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_SYMBIAN_AUDIO_PARAM_SYNCHRONIZEDTYPE);
        OMX_SYMBIAN_AUDIO_PARAM_SYNCHRONIZEDTYPE *pParams =
            (OMX_SYMBIAN_AUDIO_PARAM_SYNCHRONIZEDTYPE *) pComponentParameterStructure;

        mIsSynchronized = pParams->bIsSynchronized;

        return OMX_ErrorNone;
    }

    default:
        return AFM_PcmPort::setParameter(
                   nParamIndex, pComponentParameterStructure);
    }
}

OMX_ERRORTYPE BinarySplitter_PcmPort::getParameter(
    OMX_INDEXTYPE nParamIndex,
    OMX_PTR pComponentParameterStructure) const
{
    switch (nParamIndex) {
    case OMX_IndexParamAudioPcm:
    {
        OMX_AUDIO_PARAM_PCMMODETYPE *pcmformat =
            (OMX_AUDIO_PARAM_PCMMODETYPE *) pComponentParameterStructure;
        OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettings =
            ((BinarySplitter_PcmPort *) getENSComponent().getPort(pcmformat->nPortIndex))->getPcmSettings();

        *pcmformat = pcmSettings;

        return OMX_ErrorNone;
    }

    case OMX_IndexParamAudioSynchronized:
    {
        CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_SYMBIAN_AUDIO_PARAM_SYNCHRONIZEDTYPE);
        OMX_SYMBIAN_AUDIO_PARAM_SYNCHRONIZEDTYPE *pParams =
            (OMX_SYMBIAN_AUDIO_PARAM_SYNCHRONIZEDTYPE *) pComponentParameterStructure;

        pParams->bIsSynchronized = mIsSynchronized;

        return OMX_ErrorNone;
    }

    default:
        return AFM_PcmPort::getParameter(
                   nParamIndex, pComponentParameterStructure);
    }
}

OMX_ERRORTYPE BinarySplitter_PcmPort::updateBinarySplitterSettings(
    OMX_INDEXTYPE nParamIndex,
    OMX_PTR pComponentParameterStructure)
{
    OMX_AUDIO_PARAM_PCMMODETYPE *pcmSettings =
        (OMX_AUDIO_PARAM_PCMMODETYPE *)pComponentParameterStructure;

    mBinarySplitterSettings = *pcmSettings;

    mBinarySplitterSettings.nPortIndex= getPortIndex();

    return AFM_PcmPort::setParameter(
               nParamIndex, pComponentParameterStructure);

}


