/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
*
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson.
*
* \brief   Volume PCM port implementation
* \author  ST-Ericsson
*/
/*****************************************************************************/
#include "Volume_PcmPort.h"

Volume_PcmPort::Volume_PcmPort(OMX_U32 nIndex, OMX_U32 nBufferSizeMin,
                               const OMX_AUDIO_PARAM_PCMMODETYPE &defaultPcmSettings,
                               ENS_Component &enscomp): AFM_PcmPort(nIndex, OMX_DirInput, nBufferSizeMin, defaultPcmSettings, enscomp)
{
}



OMX_ERRORTYPE Volume_PcmPort::checkPcmSettings(const OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettings) const
{

    // Update the sampling rate of the output port (assuming the component does not support SRC)
    AFM_PcmPort *outputPort = (AFM_PcmPort *) getENSComponent().getPort(1);
    OMX_AUDIO_PARAM_PCMMODETYPE outputPortPcmSettings = outputPort->getPcmSettings();
 
    if(outputPortPcmSettings.nSamplingRate != pcmSettings.nSamplingRate)
    {
        outputPortPcmSettings.nSamplingRate = pcmSettings.nSamplingRate;
        outputPort->updateSettings(OMX_IndexParamAudioPcm, &outputPortPcmSettings);
    }

    return OMX_ErrorNone;
}
