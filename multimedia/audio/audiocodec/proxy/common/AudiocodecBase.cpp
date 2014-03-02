/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   AudiocodecBase.cpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "AudiocodecBase.h"
#include "AudiocodecBaseNmfMpc.h"
#include "audio_chipset_api_index.h"

#include "AFM_Symbian_AudioExt.h"
#include "OMX_Symbian_ExtensionNames_Ste.h"
#include "ENS_Wrapper_Services.h"
#include "wrapping_macros.h"

#include "audiocodec_top.h"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
  #include "audio_audiocodec_proxy_common_AudiocodecBaseTraces.h"
#endif //OST_TRACE_COMPILER_IN_USE

AUDIOCODEC_API_EXPORT unsigned int audiocodec_global_for_link_patch_Msp_Channel = 8;

//-----------------------------------------------------------------------------
//                  Class AudiocodecBase implementation
//-----------------------------------------------------------------------------
AudiocodecBase::AudiocodecBase()
: mBufferSize(0), mMspNbChannel(0), mDirection(OMX_DirInput), mNbChannel(0), mSamplingRate(0), mConfigVolumeChannel(0), mConfigMuteChannel(0),
    mConfigPower(0), mSupportVolumeConfig(OMX_FALSE)
{
}

AudiocodecBase::~AudiocodecBase(){
    if(mConfigVolumeChannel){
        delete mConfigVolumeChannel;
    }
    if(mConfigMuteChannel){
        delete mConfigMuteChannel;
    }
    if(mConfigPower){
        delete mConfigPower;
    }
}

OMX_AUDIO_PARAM_PCMMODETYPE AudiocodecBase::getDefaultPcmSettings(OMX_U32 nChannels, OMX_U32 nSamplingRate) const {
    OMX_AUDIO_PARAM_PCMMODETYPE defaultPcmSettings;

    defaultPcmSettings.nSize          = sizeof(OMX_AUDIO_PARAM_PCMMODETYPE);
    defaultPcmSettings.nPortIndex     = 0;  //TODO is it OMX port ? if yes please do not use 0 but add a parameter to this function
    defaultPcmSettings.eNumData       = OMX_NumericalDataSigned;
    defaultPcmSettings.eEndian        = OMX_EndianLittle;
    defaultPcmSettings.bInterleaved   = OMX_TRUE;
    defaultPcmSettings.ePCMMode       = OMX_AUDIO_PCMModeLinear;
    defaultPcmSettings.nBitPerSample  = 16; // Speech proc can't handle 24-bit on IL level, only on AFM level

    defaultPcmSettings.nChannels      = nChannels;
    defaultPcmSettings.nSamplingRate  = nSamplingRate;

    for(unsigned i=0;i<OMX_AUDIO_MAXCHANNELS;i++){
        defaultPcmSettings.eChannelMapping[i]=OMX_AUDIO_ChannelNone;
    }

    if(nChannels < 2) {
        defaultPcmSettings.eChannelMapping[0] = OMX_AUDIO_ChannelCF;
    }
    else {
        defaultPcmSettings.eChannelMapping[0] = OMX_AUDIO_ChannelLF;
        defaultPcmSettings.eChannelMapping[1] = OMX_AUDIO_ChannelRF;
    }

    return defaultPcmSettings;
}

 OMX_ERRORTYPE
AudiocodecBase::construct(
        OMX_DIRTYPE eDirection,
        OMX_U32 nBufferSizeMin,
        OMX_U32 nDmaBufferSize,
        OMX_U32 nChannels,
        OMX_U32 nSamplingRate,
        OMX_BOOL aSupportVolumeConfig,
        OMX_U32  nbOmxPort)
{
    mSamplingRate        = nSamplingRate;
    mBufferSize          = nDmaBufferSize;
    mDirection           = eDirection;
    mNbChannel           = nChannels;
    mSupportVolumeConfig = aSupportVolumeConfig;

    if(EnsWrapper_getASIC_ID() == ENS_ASIC_ID_DB8500V1){
        mMspNbChannel = audiocodec_global_for_link_patch_Msp_Channel;
    }
    else{
        mMspNbChannel = 8;
    }

    RETURN_IF_OMX_ERROR( AFM_Component::construct(nbOmxPort) );

    RETURN_IF_OMX_ERROR( createPcmPort(OMX_MASTER_PORT_IDX, eDirection, nBufferSizeMin, getDefaultPcmSettings(nChannels, nSamplingRate)) );

    if(mSupportVolumeConfig){
        mConfigVolumeChannel = new OMX_AUDIO_CONFIG_CHANNELVOLUMETYPE[nChannels];
        if(! mConfigVolumeChannel){
            RETURN_OMX_ERROR( OMX_ErrorInsufficientResources );
        }
        mConfigMuteChannel   = new OMX_AUDIO_CONFIG_CHANNELMUTETYPE[nChannels];

        //TODO: handle new failure
        // Default Value
        for(unsigned int i = 0; i < nChannels; i ++){
            mConfigVolumeChannel[i].nSize          = sizeof(OMX_AUDIO_CONFIG_CHANNELVOLUMETYPE);
            getOmxIlSpecVersion(&mConfigVolumeChannel[i].nVersion);
            mConfigVolumeChannel[i].nPortIndex     = 0;
            mConfigVolumeChannel[i].nChannel       = OMX_ALL;
            mConfigVolumeChannel[i].bLinear        = OMX_TRUE;
            mConfigVolumeChannel[i].sVolume.nValue = 50;
            mConfigVolumeChannel[i].sVolume.nMin   = 0;
            mConfigVolumeChannel[i].sVolume.nMax   = 100;
            mConfigVolumeChannel[i].bIsMIDI        = OMX_FALSE;
        }

        for(unsigned int i = 0; i < nChannels; i ++){
            mConfigMuteChannel[i].nSize    = sizeof(OMX_AUDIO_CONFIG_CHANNELMUTETYPE);
            getOmxIlSpecVersion(&mConfigMuteChannel[i].nVersion);
            mConfigMuteChannel[i].nPortIndex = OMX_MASTER_PORT_IDX;
            mConfigMuteChannel[i].nChannel = OMX_ALL;
            mConfigMuteChannel[i].bMute    = OMX_TRUE;
            mConfigMuteChannel[i].bIsMIDI  = OMX_FALSE;
        }
    }

    //TODO: handle new failure
    mConfigPower         = new OMX_AUDIO_CONFIG_POWERTYPE;

    mConfigPower->nSize    = sizeof(OMX_AUDIO_CONFIG_POWERTYPE);
    getOmxIlSpecVersion(&mConfigPower->nVersion);
    mConfigPower->nPortIndex = OMX_MASTER_PORT_IDX;
    mConfigPower->nChannel = OMX_ALL;
    mConfigPower->bPower   = OMX_TRUE;
    return OMX_ErrorNone;
}

OMX_ERRORTYPE AudiocodecBase::updatePcmSettings(OMX_U32 samplerate, OMX_U32 blocksize) 
{
    mSamplingRate = samplerate;
    mBufferSize = blocksize;
    return OMX_ErrorNone;
}

OMX_ERRORTYPE AudiocodecBase::setParameter(
        OMX_INDEXTYPE nParamIndex,
        OMX_PTR pComponentParameterStructure)
{
    switch (nParamIndex) {
        case AFM_IndexParamMspNbChannel:
            {
                if(EnsWrapper_getASIC_ID() == ENS_ASIC_ID_DB8500V1){
                    AFM_AUDIO_PARAM_MSP_NB_CHANNEL * mspParam =
                        (AFM_AUDIO_PARAM_MSP_NB_CHANNEL *)pComponentParameterStructure ;
                    mMspNbChannel = mspParam->nNbChannel;
                }
                else{
                    return OMX_ErrorUnsupportedIndex;
                }
            }
            break;
        default:
            return AFM_Component::setParameter(
                    nParamIndex, pComponentParameterStructure);
    }
    return OMX_ErrorNone;
}

OMX_ERRORTYPE AudiocodecBase::getParameter(
        OMX_INDEXTYPE nParamIndex,
        OMX_PTR pComponentParameterStructure) const
{
    if (pComponentParameterStructure == 0) {
        RETURN_OMX_ERROR( OMX_ErrorBadParameter );
    }

    switch (nParamIndex) {

        case OMX_IndexParamAudioPortFormat:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_AUDIO_PARAM_PORTFORMATTYPE);
                OMX_AUDIO_PARAM_PORTFORMATTYPE *portformat = (OMX_AUDIO_PARAM_PORTFORMATTYPE *) pComponentParameterStructure;

                //TODO : check with OMX_ALL
                if(portformat->nPortIndex >= getPortCount()){
                    RETURN_OMX_ERROR( OMX_ErrorBadPortIndex );
                }

                if (portformat->nIndex != 0) {
                    RETURN_OMX_ERROR( OMX_ErrorNoMore );
                }

                portformat->eEncoding = OMX_AUDIO_CodingPCM;
                break;
            }

        case AFM_IndexParamPcmLayout:
            {
                AFM_PARAM_PCMLAYOUTTYPE *pcmlayout =
                    (AFM_PARAM_PCMLAYOUTTYPE *) pComponentParameterStructure;

                OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettings = ((AFM_PcmPort *) getPort(0))->getPcmSettings();

                if(pcmlayout->nPortIndex >= getPortCount()){
                    RETURN_OMX_ERROR( OMX_ErrorBadPortIndex );
                }

                if (pcmSettings.nSamplingRate == 16000) {
                    pcmlayout->nBlockSize = BT_BLOCKSIZE_5MS_WB; 
                } else if (pcmSettings.nSamplingRate == 8000) {
                    pcmlayout->nBlockSize = BT_BLOCKSIZE_5MS_NB;
                } else {
                    pcmlayout->nBlockSize = mBufferSize;	
                }

                pcmlayout->nChannels      = getNbChannel();
                pcmlayout->nMaxChannels   = getNbChannel();
                pcmlayout->nBitsPerSample = 24;
                pcmlayout->nNbBuffers     = 1;

                DEBUG_LOG(" getParameter - pcmlayout->samplerate    %d\n", pcmSettings.nSamplingRate);  //TTT
                DEBUG_LOG(" getParameter - pcmlayout->nChannels    %d\n", pcmlayout->nChannels);  //TTT
                DEBUG_LOG(" getParameter - pcmlayout->nBlockSize   %d\n", pcmlayout->nBlockSize);  //TTT
                DEBUG_LOG(" getParameter - pcmlayout->nMaxChannels %d\n", pcmlayout->nMaxChannels);  //TTT
                break;
            }

        case AFM_IndexParamMspNbChannel:
            {
                if(EnsWrapper_getASIC_ID() == ENS_ASIC_ID_DB8500V1){
                    AFM_AUDIO_PARAM_MSP_NB_CHANNEL * mspParam =
                        (AFM_AUDIO_PARAM_MSP_NB_CHANNEL *)pComponentParameterStructure ;
                    mspParam->nNbChannel = mMspNbChannel;
                }
                else{
                    return OMX_ErrorUnsupportedIndex;
                }
            }
            break;

        default:
            return AFM_Component::getParameter(
                    nParamIndex, pComponentParameterStructure);
    }
    return OMX_ErrorNone;

}


OMX_AUDIO_CONFIG_CHANNELVOLUMETYPE * AudiocodecBase::getVolumeChannelConfig(OMX_U32 channelIndex) const {
    if(channelIndex < mNbChannel){
        return &mConfigVolumeChannel[channelIndex];
    }
    return static_cast<OMX_AUDIO_CONFIG_CHANNELVOLUMETYPE *>(NULL);
}

OMX_AUDIO_CONFIG_CHANNELMUTETYPE * AudiocodecBase::getMuteChannelConfig(OMX_U32 channelIndex) const {
    if(channelIndex < mNbChannel){
        return &mConfigMuteChannel[channelIndex];
    }
    return static_cast<OMX_AUDIO_CONFIG_CHANNELMUTETYPE *>(NULL);
}

OMX_AUDIO_CONFIG_POWERTYPE * AudiocodecBase::getPowerConfig() const {
    return mConfigPower;
}


OMX_ERRORTYPE AudiocodecBase::getConfig( OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentConfigStructure) const {
    if (pComponentConfigStructure == 0) {
        RETURN_OMX_ERROR( OMX_ErrorBadParameter );
    }

    switch (nParamIndex) {
        case OMX_IndexConfigAudioVolume:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_AUDIO_CONFIG_VOLUMETYPE);
                OMX_AUDIO_CONFIG_VOLUMETYPE * pVolume = (OMX_AUDIO_CONFIG_VOLUMETYPE *) pComponentConfigStructure;

                if (pVolume->nPortIndex != OMX_MASTER_PORT_IDX) {
                    RETURN_OMX_ERROR( OMX_ErrorBadPortIndex );
                }

                OMX_BS32 volume = mConfigVolumeChannel[0].sVolume;
                for(OMX_U32 i = 0; i < mNbChannel; i++){
                    OMX_BS32 tmp_volume = mConfigVolumeChannel[i].sVolume;
                    if(volume.nValue != tmp_volume.nValue){
                        RETURN_OMX_ERROR( OMX_ErrorUnsupportedSetting );
                    }
                }

                pVolume->bLinear       = mConfigVolumeChannel[0].bLinear;
                pVolume->sVolume.nValue= mConfigVolumeChannel[0].sVolume.nValue;
                pVolume->sVolume.nMin  = mConfigVolumeChannel[0].sVolume.nMin;
                pVolume->sVolume.nMax  = mConfigVolumeChannel[0].sVolume.nMax;

                return OMX_ErrorNone;
            }

        case OMX_IndexConfigAudioMute:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_AUDIO_CONFIG_MUTETYPE);
                OMX_AUDIO_CONFIG_MUTETYPE * pMute =
                    (OMX_AUDIO_CONFIG_MUTETYPE *) pComponentConfigStructure;

                if (pMute->nPortIndex != OMX_MASTER_PORT_IDX) {
                    RETURN_OMX_ERROR( OMX_ErrorBadPortIndex );
                }

                OMX_BOOL mute = mConfigMuteChannel[0].bMute;
                for(OMX_U32 i = 0; i < mNbChannel; i++){
                    OMX_BOOL tmp_mute = mConfigMuteChannel[i].bMute;
                    if(mute != tmp_mute){
                        RETURN_OMX_ERROR( OMX_ErrorUnsupportedSetting );
                    }
                }

                pMute->bMute  = mute;
                return OMX_ErrorNone;
            }
        case OMX_IndexConfigAudioChannelVolume:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_AUDIO_CONFIG_CHANNELVOLUMETYPE);
                OMX_AUDIO_CONFIG_CHANNELVOLUMETYPE * pVolume = (OMX_AUDIO_CONFIG_CHANNELVOLUMETYPE *) pComponentConfigStructure;

                if (pVolume->nPortIndex != OMX_MASTER_PORT_IDX) {
                    RETURN_OMX_ERROR( OMX_ErrorBadPortIndex );
                }

                if(pVolume->nChannel == OMX_ALL){
                    //We select channel 0 in this case
                    *pVolume = mConfigVolumeChannel[0];
                }
                else{
                    if (pVolume->nChannel >= mNbChannel) {
                        RETURN_OMX_ERROR( OMX_ErrorBadParameter );
                    }
                    *pVolume = mConfigVolumeChannel[pVolume->nChannel];
                }
                return OMX_ErrorNone;
            }
        case OMX_IndexConfigAudioChannelMute:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_AUDIO_CONFIG_CHANNELMUTETYPE);
                OMX_AUDIO_CONFIG_CHANNELMUTETYPE * pMute =
                    (OMX_AUDIO_CONFIG_CHANNELMUTETYPE *) pComponentConfigStructure;

                if (pMute->nPortIndex != OMX_MASTER_PORT_IDX) {
                    RETURN_OMX_ERROR( OMX_ErrorBadPortIndex );
                }

                if(pMute->nChannel == OMX_ALL){
                    //We select channel 0 in this case
                    *pMute = mConfigMuteChannel[0];
                }
                else{
                    if (pMute->nChannel >= mNbChannel) {
                        RETURN_OMX_ERROR( OMX_ErrorBadParameter );
                    }
                    *pMute = mConfigMuteChannel[pMute->nChannel];
                }
                return OMX_ErrorNone;
            }
        case OMX_IndexConfigAudioPower:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_AUDIO_CONFIG_POWERTYPE);
                OMX_AUDIO_CONFIG_POWERTYPE * pPower = (OMX_AUDIO_CONFIG_POWERTYPE *) pComponentConfigStructure;

                if (pPower->nPortIndex != OMX_MASTER_PORT_IDX) {
                    RETURN_OMX_ERROR( OMX_ErrorBadPortIndex );
                }

                *pPower = *mConfigPower;

                return OMX_ErrorNone;
            }
        default:
            return AFM_Component::getConfig(nParamIndex,pComponentConfigStructure);
    }
}

OMX_ERRORTYPE AudiocodecBase::getExtensionIndex(OMX_STRING extension, OMX_INDEXTYPE* index) const
{
	if (ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(extension) == OMX_SYMBIAN_INDEX_CONFIG_AUDIO_DATAAMOUNT_NAME)
	{
		*index = (OMX_INDEXTYPE)OMX_IndexConfigAudioProcessedDataAmount;
		return OMX_ErrorNone;
	}
	else
	{
		return AFM_Component::getExtensionIndex(extension,index);
	}
}
OMX_ERRORTYPE AudiocodecBase::setConfig( OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentConfigStructure) {
    if (pComponentConfigStructure == 0) {
        RETURN_OMX_ERROR( OMX_ErrorBadParameter );
    }

    switch (nParamIndex) {
        case OMX_IndexConfigAudioVolume:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_AUDIO_CONFIG_VOLUMETYPE);
                OMX_AUDIO_CONFIG_VOLUMETYPE * pVolume = (OMX_AUDIO_CONFIG_VOLUMETYPE *) pComponentConfigStructure;

                if (pVolume->nPortIndex != OMX_MASTER_PORT_IDX) {
                    RETURN_OMX_ERROR( OMX_ErrorBadPortIndex );
                }
                if(pVolume->bLinear == OMX_FALSE){
                    RETURN_OMX_ERROR( OMX_ErrorBadParameter );
                }
                if((pVolume->sVolume.nMin != 0) || (pVolume->sVolume.nMax != 100)){
                    RETURN_OMX_ERROR( OMX_ErrorBadParameter );
                }
                if (pVolume->sVolume.nValue < pVolume->sVolume.nMin ||
                        pVolume->sVolume.nValue > pVolume->sVolume.nMax) {
                    RETURN_OMX_ERROR( OMX_ErrorBadParameter );
                }
                if (!mSupportVolumeConfig) {
                    return OMX_ErrorUnsupportedIndex;
                }
                if (mConfigVolumeChannel == NULL) {
                    RETURN_OMX_ERROR( OMX_ErrorInsufficientResources );
                }

                DEBUG_LOG(" Volume=%d \n", pVolume->sVolume.nValue);

                for(unsigned int i = 0; i < mNbChannel; i ++){
                    mConfigVolumeChannel[i].nChannel       = i;
                    mConfigVolumeChannel[i].sVolume.nValue = pVolume->sVolume.nValue;
                }

                return OMX_ErrorNone;
            }

        case OMX_IndexConfigAudioMute:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_AUDIO_CONFIG_MUTETYPE);
                OMX_AUDIO_CONFIG_MUTETYPE * pMute = (OMX_AUDIO_CONFIG_MUTETYPE *) pComponentConfigStructure;

                if (pMute->nPortIndex != OMX_MASTER_PORT_IDX) {
                    RETURN_OMX_ERROR( OMX_ErrorBadPortIndex );
                }
                if((pMute->bMute != OMX_TRUE) && (pMute->bMute != OMX_FALSE)){
                    RETURN_OMX_ERROR( OMX_ErrorBadParameter );
                }
                if (!mSupportVolumeConfig) {
                    return OMX_ErrorUnsupportedIndex;
                }
                if (mConfigMuteChannel == NULL) {
                    RETURN_OMX_ERROR( OMX_ErrorInsufficientResources );
                }

                DEBUG_LOG(" Mute=%d \n", pMute->bMute);

                for(unsigned int i = 0; i < mNbChannel; i ++){
                    mConfigMuteChannel[i].nChannel = i;
                    mConfigMuteChannel[i].bMute    = pMute->bMute;
                }

                return OMX_ErrorNone;
            }

        case OMX_IndexConfigAudioChannelVolume:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_AUDIO_CONFIG_CHANNELVOLUMETYPE);
                OMX_AUDIO_CONFIG_CHANNELVOLUMETYPE * pChannelVolume = (OMX_AUDIO_CONFIG_CHANNELVOLUMETYPE *) pComponentConfigStructure;

                if (pChannelVolume->nPortIndex != OMX_MASTER_PORT_IDX) {
                    RETURN_OMX_ERROR( OMX_ErrorBadPortIndex );
                }

                if (pChannelVolume->bLinear != OMX_TRUE) {
                    RETURN_OMX_ERROR( OMX_ErrorBadParameter );
                }

                if((pChannelVolume->nChannel != OMX_ALL) && (pChannelVolume->nChannel >= mNbChannel)){
                    RETURN_OMX_ERROR( OMX_ErrorBadParameter );
                }

                if(pChannelVolume->sVolume.nMin != 0 || pChannelVolume->sVolume.nMax != 100){
                    RETURN_OMX_ERROR( OMX_ErrorBadParameter );
                }

                if (pChannelVolume->sVolume.nValue < pChannelVolume->sVolume.nMin ||
                        pChannelVolume->sVolume.nValue > pChannelVolume->sVolume.nMax) {
                    RETURN_OMX_ERROR( OMX_ErrorBadParameter );
                }

                if (!mSupportVolumeConfig) {
                    return OMX_ErrorUnsupportedIndex;
                }

                if (mConfigVolumeChannel == NULL) {
                    RETURN_OMX_ERROR( OMX_ErrorInsufficientResources );
                }

                DEBUG_LOG(" Volume=%d Channel=%d \n", pChannelVolume->sVolume.nValue, pChannelVolume->nChannel);

                if(pChannelVolume->nChannel == OMX_ALL){
                    for(unsigned int i = 0; i < mNbChannel; i ++){
                        mConfigVolumeChannel[i] = *pChannelVolume;
                    }
                }
                else{
                    mConfigVolumeChannel[pChannelVolume->nChannel] = *pChannelVolume;
                }

                return OMX_ErrorNone;
            }

        case OMX_IndexConfigAudioChannelMute:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_AUDIO_CONFIG_CHANNELMUTETYPE);
                OMX_AUDIO_CONFIG_CHANNELMUTETYPE * pChannelMute = (OMX_AUDIO_CONFIG_CHANNELMUTETYPE *) pComponentConfigStructure;

                if (pChannelMute->nPortIndex != OMX_MASTER_PORT_IDX) {
                    RETURN_OMX_ERROR( OMX_ErrorBadPortIndex );
                }
                if((pChannelMute->nChannel != OMX_ALL) && (pChannelMute->nChannel >= mNbChannel)){
                    RETURN_OMX_ERROR( OMX_ErrorBadParameter );
                }
                if((pChannelMute->bMute != OMX_TRUE) && (pChannelMute->bMute != OMX_FALSE)){
                    RETURN_OMX_ERROR( OMX_ErrorBadParameter );
                }
                if (!mSupportVolumeConfig) {
                    return OMX_ErrorUnsupportedIndex;
                }
                if (mConfigMuteChannel == NULL) {
                    RETURN_OMX_ERROR( OMX_ErrorInsufficientResources );
                }

                DEBUG_LOG(" Mute=%d Channel=%d \n", pChannelMute->bMute, pChannelMute->nChannel);

                if(pChannelMute->nChannel == OMX_ALL){
                    for(unsigned int i = 0; i < mNbChannel; i ++){
                        mConfigMuteChannel[i] = *pChannelMute;
                    }
                }
                else{
                    mConfigMuteChannel[pChannelMute->nChannel] = *pChannelMute;
                }
                return OMX_ErrorNone;
            }

        case OMX_IndexConfigAudioPower:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_AUDIO_CONFIG_POWERTYPE);
                OMX_AUDIO_CONFIG_POWERTYPE * pPower = (OMX_AUDIO_CONFIG_POWERTYPE *) pComponentConfigStructure;

                if (pPower->nPortIndex != OMX_MASTER_PORT_IDX) {
                    RETURN_OMX_ERROR( OMX_ErrorBadPortIndex );
                }

                if((pPower->nChannel != OMX_ALL) && (pPower->nChannel >= mNbChannel)){
                    RETURN_OMX_ERROR( OMX_ErrorBadParameter );
                }

                DEBUG_LOG(" Power=%d Channel=%d \n", pPower->bPower, pPower->nChannel);

                * mConfigPower = *pPower;
                return OMX_ErrorNone;
            }
        default:
            return AFM_Component::setConfig(nParamIndex,pComponentConfigStructure);
    }
}

AUDIOCODEC_API_EXPORT 
RM_STATUS_E AudiocodecBase::getResourcesEstimation(const OMX_PTR pCompHdl, RM_EMDATA_T* pEstimationData) {
    // fill in defaults
    RM_STATUS_E status = ENS_Component::getResourcesEstimation(pCompHdl, pEstimationData);

    if (status == RM_E_NONE) {
        // update UCC bitmap to request reserved audio esram
        pEstimationData->eUccBitmap |= RM_UCC_AUDIO_ESRAM;
    }
    return status;
}

OMX_U32 AudiocodecBase::getUidTopDictionnary(void)
{
    return KOstaudiocodec_top_ID;
}


//-----------------------------------------------------------------------------
//                  Class AudioRenderer implementation
//-----------------------------------------------------------------------------
OMX_ERRORTYPE AudioRenderer::construct(OMX_U32  nBufferSizeMin, OMX_U32  nDmaBufferSize, OMX_U32 nChannels, OMX_U32  nSamplingRate, OMX_BOOL aSupportVolumeConfig, OMX_BOOL aHasFeedbackPort) {

    OMX_U32 nbOmxPorts = 1;
    if(aHasFeedbackPort){
        nbOmxPorts++;
    }

    RETURN_IF_OMX_ERROR( AudiocodecBase::construct(OMX_DirInput, nBufferSizeMin, nDmaBufferSize, nChannels, nSamplingRate, aSupportVolumeConfig, nbOmxPorts) );

    if (aHasFeedbackPort) {
    	RETURN_IF_OMX_ERROR( createPcmPort(OMX_FEEDBACK_PORT_IDX, OMX_DirOutput, nBufferSizeMin, getDefaultPcmSettings(nChannels, nSamplingRate)) );
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE AudioRenderer::getParameter(
        OMX_INDEXTYPE nParamIndex,
        OMX_PTR pComponentParameterStructure) const
{
    if (pComponentParameterStructure == 0) {
        return OMX_ErrorBadParameter;
    }

    switch (nParamIndex) {
        default:
            return AudiocodecBase::getParameter(
                    nParamIndex, pComponentParameterStructure);
    }
}

OMX_ERRORTYPE AudioRenderer::setConfig( OMX_INDEXTYPE nParamIndex,  OMX_PTR pComponentConfigStructure) {
    if (pComponentConfigStructure == 0) {
        return OMX_ErrorBadParameter;
    }

    switch (nParamIndex) {
        default:
            return AudiocodecBase::setConfig(
                    nParamIndex, pComponentConfigStructure);
    }
}

OMX_ERRORTYPE AudioRenderer::getConfig( OMX_INDEXTYPE nParamIndex,  OMX_PTR pComponentConfigStructure) const {
    if (pComponentConfigStructure == 0) {
        return OMX_ErrorBadParameter;
    }

    switch (nParamIndex) {
		case OMX_IndexConfigAudioProcessedDataAmount:
		{
            CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_SYMBIAN_AUDIO_CONFIG_PROCESSEDDATAAMOUNTTYPE);

            OMX_SYMBIAN_AUDIO_CONFIG_PROCESSEDDATAAMOUNTTYPE * pConfig = (OMX_SYMBIAN_AUDIO_CONFIG_PROCESSEDDATAAMOUNTTYPE *)pComponentConfigStructure;

            pConfig->nProcessedDataAmount  = 0;
            return OMX_ErrorNone;
		}

        default:
            return AudiocodecBase::getConfig(
                    nParamIndex, pComponentConfigStructure);
    }
}

AudioRendererNmfMpc * AudioRenderer::getAudioRendererNmfMpc(void) const{
    const AFMNmfMpc_ProcessingComp & comp = static_cast<const AFMNmfMpc_ProcessingComp &>( getProcessingComponent());
    return (AudioRendererNmfMpc *)(&comp);
}

//-----------------------------------------------------------------------------
//                  Class AudioCapturer implementation
//-----------------------------------------------------------------------------
OMX_ERRORTYPE AudioCapturer::construct(OMX_U32  nBufferSizeMin, OMX_U32  nDmaBufferSize, OMX_U32 nChannels, OMX_U32  nSamplingRate, OMX_BOOL aSupportVolumeConfig) {
    RETURN_IF_OMX_ERROR( AudiocodecBase::construct(OMX_DirOutput, nBufferSizeMin, nDmaBufferSize, nChannels, nSamplingRate, aSupportVolumeConfig) );
    return OMX_ErrorNone;
}

OMX_ERRORTYPE AudioCapturer::setParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure) {
    if (pComponentParameterStructure == 0) {
        return OMX_ErrorBadParameter;
    }

    switch (nParamIndex) {
        case AFM_IndexParamCapturer:
            {
                AFM_PARAM_CAPTURERTYPE * param_capturer =
                    (AFM_PARAM_CAPTURERTYPE *) pComponentParameterStructure;

                mBufferSize = param_capturer->nBufferSize;

                return OMX_ErrorNone;
            }

        default:
            return AudiocodecBase::setParameter(
                    nParamIndex, pComponentParameterStructure);
    }
}

OMX_ERRORTYPE AudioCapturer::getParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure) const {
    if (pComponentParameterStructure == 0) {
        return OMX_ErrorBadParameter;
    }

    switch (nParamIndex) {
    
    case AFM_IndexParamCapturer:
            {
                AFM_PARAM_CAPTURERTYPE * param_capturer =
                     (AFM_PARAM_CAPTURERTYPE *) pComponentParameterStructure;

                param_capturer->nBufferSize = mBufferSize;

                return OMX_ErrorNone;
            }

        default:
            return AudiocodecBase::getParameter(
                    nParamIndex, pComponentParameterStructure);
    }
}

OMX_ERRORTYPE AudioCapturer::getConfig( OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentConfigStructure) const {
    if (pComponentConfigStructure == 0) {
        return OMX_ErrorBadParameter;
    }

    switch (nParamIndex) {
		case OMX_IndexConfigAudioProcessedDataAmount:
		{
            CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_SYMBIAN_AUDIO_CONFIG_PROCESSEDDATAAMOUNTTYPE);

			OMX_SYMBIAN_AUDIO_CONFIG_PROCESSEDDATAAMOUNTTYPE * pConfig = (OMX_SYMBIAN_AUDIO_CONFIG_PROCESSEDDATAAMOUNTTYPE *)pComponentConfigStructure;
			pConfig->nProcessedDataAmount = 0;
			return OMX_ErrorNone;
		}

        default:
            return AudiocodecBase::getConfig(
                    nParamIndex, pComponentConfigStructure);
    }
}

AudioCapturerNmfMpc * AudioCapturer::getAudioCapturerNmfMpc(void) const{
    const AFMNmfMpc_ProcessingComp & comp = static_cast<const AFMNmfMpc_ProcessingComp &>( getProcessingComponent());
    return (AudioCapturerNmfMpc *)(&comp);
}

//-----------------------------------------------------------------------------
//                  Class AudioRendererAb8500 implementation
//-----------------------------------------------------------------------------
OMX_ERRORTYPE AudioRendererAb8500::construct(OMX_U32 nChannels, OMX_BOOL aSupportVolumeConfig, OMX_BOOL aHasFeedbackPort) {

    OMX_U32 nBufferSizeMin = OMX_PORT_BUFFER_SIZE_MIN_AB8500;
    OMX_U32 nSamplingRate  = 48000;
   // TODO : no figures
    RETURN_IF_OMX_ERROR( AudioRenderer::construct(nBufferSizeMin, 48, nChannels, nSamplingRate, aSupportVolumeConfig, aHasFeedbackPort) );

    mLowPowerEnabled = OMX_FALSE;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE AudioRendererAb8500::getParameter( OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure) const
{
    if (pComponentParameterStructure == 0) {
        RETURN_OMX_ERROR( OMX_ErrorBadParameter );
    }


    switch (nParamIndex) {

        case AFM_IndexParamPcmLayout:
            {
                AFM_PARAM_PCMLAYOUTTYPE *pcmlayout =
                    (AFM_PARAM_PCMLAYOUTTYPE *) pComponentParameterStructure;

                if(pcmlayout->nPortIndex >= getPortCount()){
                    RETURN_OMX_ERROR( OMX_ErrorBadPortIndex );
                }

                pcmlayout->nChannels      = getNbChannel();
                pcmlayout->nBlockSize     = mBufferSize * 5;
                pcmlayout->nMaxChannels   = getNbChannel();
                pcmlayout->nBitsPerSample = 24;
                pcmlayout->nNbBuffers     = 1;
                return OMX_ErrorNone;
            }

        default:
            return AudioRenderer::getParameter(
                    nParamIndex, pComponentParameterStructure);
    }
}

OMX_ERRORTYPE AudioRendererAb8500::setConfig( OMX_INDEXTYPE nParamIndex,  OMX_PTR pComponentConfigStructure) {
    if (pComponentConfigStructure == 0) {
        return OMX_ErrorBadParameter;
    }

    switch (nParamIndex) {
        case AFM_IndexLowPowerRenderer:
            {
                AFM_CONFIG_LOWPOWERTYPE *config = (AFM_CONFIG_LOWPOWERTYPE*)pComponentConfigStructure;
                mLowPowerEnabled = config->nLowPower;
                OstTraceInt1(TRACE_DEBUG, "AudioRenderer::setConfig  mLowPowerEnabled=%d",mLowPowerEnabled);
                return OMX_ErrorNone;
            }
        default:
            return AudioRenderer::setConfig(
                    nParamIndex, pComponentConfigStructure);
    }
}

OMX_ERRORTYPE AudioRendererAb8500::getConfig( OMX_INDEXTYPE nParamIndex,  OMX_PTR pComponentConfigStructure) const {
    if (pComponentConfigStructure == 0) {
        return OMX_ErrorBadParameter;
    }

    switch (nParamIndex) {
        case AFM_IndexLowPowerRenderer:
        {
            AFM_CONFIG_LOWPOWERTYPE *config = (AFM_CONFIG_LOWPOWERTYPE*)pComponentConfigStructure;
            config->nLowPower = mLowPowerEnabled;

            return OMX_ErrorNone;
        }
        default:
            return AudioRenderer::getConfig(
                    nParamIndex, pComponentConfigStructure);
    }
}


//-----------------------------------------------------------------------------
//                  Class AudioCapturerAb8500 implementation
//-----------------------------------------------------------------------------
OMX_ERRORTYPE AudioCapturerAb8500::construct(OMX_U32 nChannels, OMX_BOOL aSupportVolumeConfig) {
    //TODO: no figures
    RETURN_IF_OMX_ERROR( AudioCapturer::construct(OMX_PORT_BUFFER_SIZE_MIN_AB8500, 48, nChannels, 48000, aSupportVolumeConfig) );
    return OMX_ErrorNone;
}

OMX_ERRORTYPE AudioCapturerAb8500::setParameter( OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure)  {
    if (pComponentParameterStructure == 0) {
        return OMX_ErrorBadParameter;
    }

    switch (nParamIndex) {
        default:
            return AudioCapturer::setParameter(
                    nParamIndex, pComponentParameterStructure);
    }
}

OMX_ERRORTYPE AudioCapturerAb8500::getParameter( OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure) const {
    if (pComponentParameterStructure == 0) {
        return OMX_ErrorBadParameter;
    }

    switch (nParamIndex) {
        case AFM_IndexParamPcmLayout:
            {
                AFM_PARAM_PCMLAYOUTTYPE *pcmlayout =
                    (AFM_PARAM_PCMLAYOUTTYPE *) pComponentParameterStructure;

                if(pcmlayout->nPortIndex >= getPortCount()){
                    RETURN_OMX_ERROR( OMX_ErrorBadPortIndex );
                }

                pcmlayout->nChannels      = getNbChannel();
                pcmlayout->nBlockSize     = mBufferSize * 5;
                pcmlayout->nMaxChannels   = getNbChannel();
                pcmlayout->nBitsPerSample = 24;
                pcmlayout->nNbBuffers     = 1;
                return OMX_ErrorNone;
            }
        
        default:
            return AudioCapturer::getParameter(
                    nParamIndex, pComponentParameterStructure);
    }
}

OMX_ERRORTYPE AudioCapturerAb8500::getConfig( OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentConfigStructure) const {
    if (pComponentConfigStructure == 0) {
        return OMX_ErrorBadParameter;
    }

    switch (nParamIndex) {
        default:
            return AudioCapturer::getConfig(
                    nParamIndex, pComponentConfigStructure);
    }
}

