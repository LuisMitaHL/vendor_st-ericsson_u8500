/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved

*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   SRC pcm processor proxy
* \author  ST-Ericsson
*/
/*****************************************************************************/
#include "SRCPcmProcessing.h"
#include "NmfHostSampleRateConv_PcmProcessing.h"

OMX_ERRORTYPE srcPcmProcFactoryMethod(ENS_Component_p * ppENSComponent) {
    OMX_ERRORTYPE error  = OMX_ErrorNone;
#ifdef NEWAB
//	OMX_U32 Mode = 6;
	OMX_U32 Mode = 0;
#endif
    SRCPcmProcessing *pcmProc = new SRCPcmProcessing();
    if (pcmProc == 0) {
        return OMX_ErrorInsufficientResources;
    }

    error = pcmProc->construct();

    if (error != OMX_ErrorNone) {
        delete pcmProc;
        *ppENSComponent = 0;
        return error;
    } else {
        *ppENSComponent = pcmProc;
    }

    SRCNmfHost_PcmProcessing *pcmProcProcessingComp = new SRCNmfHost_PcmProcessing(*pcmProc);

    if (pcmProcProcessingComp == 0) {
        delete pcmProc;
        return OMX_ErrorInsufficientResources;
    }
#ifdef NEWAB
	pcmProcProcessingComp->setParam(Mode);
#endif
    pcmProc->setProcessingComponent(pcmProcProcessingComp);

    return error;
}

OMX_ERRORTYPE  SRCPcmProcessing::construct()
{
    OMX_ERRORTYPE error;
    OMX_AUDIO_PARAM_PCMMODETYPE defaultPcmSettingsIn;
    OMX_AUDIO_PARAM_PCMMODETYPE defaultPcmSettingsOut;

    defaultPcmSettingsIn.nPortIndex     = 0;
    defaultPcmSettingsIn.eNumData       = OMX_NumericalDataSigned;
    defaultPcmSettingsIn.eEndian        = OMX_EndianLittle;
    defaultPcmSettingsIn.bInterleaved   = OMX_TRUE;
    defaultPcmSettingsIn.ePCMMode       = OMX_AUDIO_PCMModeLinear;
    defaultPcmSettingsIn.nBitPerSample  = 16;
    defaultPcmSettingsIn.nChannels      = 2;
    defaultPcmSettingsIn.nSamplingRate  = 8000;
    defaultPcmSettingsIn.eChannelMapping[0] = OMX_AUDIO_ChannelLF;
    defaultPcmSettingsIn.eChannelMapping[1] = OMX_AUDIO_ChannelRF;

    defaultPcmSettingsOut.nPortIndex     = 1;
    defaultPcmSettingsOut.eNumData       = OMX_NumericalDataSigned;
    defaultPcmSettingsOut.eEndian        = OMX_EndianLittle;
    defaultPcmSettingsOut.bInterleaved   = OMX_TRUE;
    defaultPcmSettingsOut.ePCMMode       = OMX_AUDIO_PCMModeLinear;
    defaultPcmSettingsOut.nBitPerSample  = 16;
    defaultPcmSettingsOut.nChannels      = 2;
    defaultPcmSettingsOut.nSamplingRate  = 8000;
    defaultPcmSettingsOut.eChannelMapping[0] = OMX_AUDIO_ChannelLF;
    defaultPcmSettingsOut.eChannelMapping[1] = OMX_AUDIO_ChannelRF;

    error = AFM_PcmProcessing::construct(defaultPcmSettingsIn, defaultPcmSettingsOut);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}


