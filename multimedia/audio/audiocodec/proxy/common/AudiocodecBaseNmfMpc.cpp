/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   AudiocodecBaseNmfMpc.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "AudiocodecBaseNmfMpc.h"
#include "NmfMpcInOut.h"
#include "audio_chipset_api_index.h"
#include "alsactrl_audiocodec.h"
#include "AFM_Symbian_AudioExt.h"
#include "wrapping_macros.h"

#include "audiocodec_debug.h"

#include "ENS_Nmf.h"
#include "AFM_nmf.h"
#include "AFM_Utils.h"
#include "OMX_CoreExt.h"

#define LOG_TAG "audiocodec"


//-----------------------------------------------------------------------------
//              class AudiocodecBaseNmfMpc
//-----------------------------------------------------------------------------
const char* AudiocodecBaseNmfMpc::SuffixPerIndex[]={
    "[0]",
    "[1]",
    "[2]",
    "[3]",
    "[4]",
    "[5]"
};

ENS_String<32> AudiocodecBaseNmfMpc::addSuffixPerPortNb(const char * str)
{
    ENS_String<32> ret(str);
    DBC_ASSERT(mDmaPortIndex < 6);
    ret.concat(SuffixPerIndex[mDmaPortIndex]);
    return ret;
}

void AudiocodecBaseNmfMpc::handleStatusFromAudiohwCtrl(AUDIO_HAL_STATUS status)
{
    OMX_U32 nData1     = 0;
    OMX_U32 nData2     = 0;
    OMX_PTR pEventData = 0; 

    switch ( status ){
        case AUDIO_HAL_STATUS_OK:
            // Everything is OK
            return;
        case AUDIO_HAL_STATUS_UNSUPPORTED:
            nData1 = OMX_U32(OMX_ErrorUnsupportedSetting);
            break;
        case AUDIO_HAL_STATUS_ERROR:
            nData1 = OMX_U32(OMX_ErrorUndefined);	
            break;
        default:	
            DBC_ASSERT(0);
            break;
    }
    mENSComponent.eventHandlerCB(OMX_EventError, nData1, nData2, pEventData);
}

void AudiocodecBaseNmfMpc::statusCB(AUDIO_HAL_STATUS status)
{
    LOG_I("Enter.\n");
}

void AudiocodecBaseNmfMpc::registerStubsAndSkels(void) {
    CM_REGISTER_STUBS_SKELS(audiocodec_cpp);
}

void AudiocodecBaseNmfMpc::unregisterStubsAndSkels(void) {
    CM_UNREGISTER_STUBS_SKELS(audiocodec_cpp);
}

AudiocodecBaseNmfMpc::AudiocodecBaseNmfMpc(AudiocodecBase &audiocodec, audio_hal_channel channel, OMX_U32 nDMAPortIndex) 
: AFMNmfMpc_ProcessingComp(audiocodec), mVolCtrl(0),
    mStartTimeinUs(0), mAudioCodecBase(audiocodec),
    mDmaPortIndex(nDMAPortIndex), mIsStarted(false),
    mSamplerate(0), mBlockSize(0)
{
    LOG_I("Enter (Channel-type: %d).\n", channel);
    this->channel = channel;

    // setPriorityLevel(NMF_SCHED_URGENT); 
}

OMX_ERRORTYPE AudiocodecBaseNmfMpc::construct(void)
{
    LOG_I("Enter (Channel-type: %d).\n", this->channel);

    registerStubsAndSkels();

    if(audio_hal_open_channel != 0){
        if (audio_hal_open_channel(this->channel) != AUDIO_HAL_STATUS_OK)
		return OMX_ErrorUndefined;
    }

    RETURN_IF_OMX_ERROR( AFMNmfMpc_ProcessingComp::construct() );
    return OMX_ErrorNone;
}

OMX_ERRORTYPE AudiocodecBaseNmfMpc::destroy(void)
{
    RETURN_IF_OMX_ERROR( AFMNmfMpc_ProcessingComp::destroy() );

    if(audio_hal_set_power != 0){
        for(OMX_U32 i = 0; i < mAudioCodecBase.getNbChannel(); i++){
            statusCB(audio_hal_set_power(i, AUDIO_HAL_STATE_OFF, this->channel));
        }
    }

    if(audio_hal_close_channel != 0){
        audio_hal_close_channel(this->channel);
    }

    unregisterStubsAndSkels();

    return OMX_ErrorNone;
}

OMX_ERRORTYPE AudiocodecBaseNmfMpc::doSpecificSendCommand(
        OMX_COMMANDTYPE eCmd, 
        OMX_U32 nData, 
        OMX_BOOL &bDeferredCmd)
{
    switch (eCmd) 
    {
        case OMX_CommandStateSet: 
            {
                OMX_STATETYPE newstate = (OMX_STATETYPE)nData;
                switch (newstate) {
                    case OMX_StateExecuting:
                        if(!mIsStarted){
                            mIsStarted = true;
                            mStartTimeinUs = mDmaBase->getTimeInUs(0);	
                        }
                        break;
                    case OMX_StateIdle:
                        if(mIsStarted){
                            mIsStarted = false;
                            mStartTimeinUs = 0;	
                        }
                        break;

                    default:
                        break;
                }
            }
            break;

        default:
            break;
    }

    return AFMNmfMpc_ProcessingComp::doSpecificSendCommand(eCmd, nData, bDeferredCmd);
}

OMX_ERRORTYPE AudiocodecBaseNmfMpc::configureVolCtrl(NmfMpcVolCtrl * volctrl, OMX_U32 omxPortIdx)
{
    LOG_I("Enter.\n");

    OMX_AUDIO_CONFIG_VOLUMETYPE     Volctrl_ConfigVolume;
    OMX_AUDIO_CONFIG_MUTETYPE       Volctrl_ConfigMute;
    OMX_AUDIO_CONFIG_BALANCETYPE    Volctrl_ConfigBalance;

    // VolCtrl Mute configuration
    Volctrl_ConfigMute.nSize      = sizeof(OMX_AUDIO_CONFIG_MUTETYPE);
    getOmxIlSpecVersion(&Volctrl_ConfigMute.nVersion);
    Volctrl_ConfigMute.nPortIndex = omxPortIdx;
    Volctrl_ConfigMute.bMute      = OMX_FALSE;   
    // VolCtrl Balance Configuration
    Volctrl_ConfigBalance.nSize      = sizeof(OMX_AUDIO_CONFIG_BALANCETYPE);
    getOmxIlSpecVersion(&Volctrl_ConfigBalance.nVersion);
    Volctrl_ConfigBalance.nPortIndex = omxPortIdx;
    Volctrl_ConfigBalance.nBalance   = VOLCTRL_BALANCE_CENTER;    
    // VolCtrl Volume Configuration
    Volctrl_ConfigVolume.nSize          = sizeof(OMX_AUDIO_CONFIG_VOLUMETYPE);
    getOmxIlSpecVersion(&Volctrl_ConfigVolume.nVersion);
    Volctrl_ConfigVolume.nPortIndex     = omxPortIdx;
    Volctrl_ConfigVolume.bLinear        = OMX_FALSE;
    Volctrl_ConfigVolume.sVolume.nValue = VOLCTRL_VOLUME_MAX;
    Volctrl_ConfigVolume.sVolume.nMin   = VOLCTRL_VOLUME_MIN;
    Volctrl_ConfigVolume.sVolume.nMax   = VOLCTRL_VOLUME_MAX;

    RETURN_IF_OMX_ERROR(volctrl->configure());
    RETURN_IF_OMX_ERROR(volctrl->applyConfig(OMX_IndexConfigAudioVolume , &Volctrl_ConfigVolume));
    RETURN_IF_OMX_ERROR(volctrl->applyConfig(OMX_IndexConfigAudioMute   , &Volctrl_ConfigMute));
    RETURN_IF_OMX_ERROR(volctrl->applyConfig(OMX_IndexConfigAudioBalance, &Volctrl_ConfigBalance));  
    RETURN_IF_OMX_ERROR(volctrl->configureAlgo());
    return OMX_ErrorNone;
}

OMX_ERRORTYPE AudiocodecBaseNmfMpc::instantiateVolCtrl(NmfMpcVolCtrl * & new_instance, OMX_DIRTYPE direction, OMX_U32 portIndex)
{
    LOG_I("Enter.\n");

    DBC_ASSERT(mDmaBase);

    t_pcmprocessing_config volctrl_config;
    volctrl_config.processingMode                          = direction != OMX_DirOutput ? PCMPROCESSING_MODE_INPLACE_PUSH : PCMPROCESSING_MODE_INPLACE_PULL;
    volctrl_config.effectConfig.block_size                 = mAudioCodecBase.getDMABufferSize();
    volctrl_config.effectConfig.infmt.freq                 = getFreq();
    volctrl_config.effectConfig.infmt.nof_channels         = mAudioCodecBase.getNbChannel();
    volctrl_config.effectConfig.infmt.nof_bits_per_sample  = 24;   
    volctrl_config.effectConfig.outfmt.freq                = volctrl_config.effectConfig.infmt.freq; 
    volctrl_config.effectConfig.outfmt.nof_channels        = volctrl_config.effectConfig.infmt.nof_channels;
    volctrl_config.effectConfig.outfmt.nof_bits_per_sample = volctrl_config.effectConfig.infmt.nof_bits_per_sample;

    new_instance = new NmfMpcVolCtrl(OMX_FALSE, volctrl_config, portIndex);
    if (!new_instance) {
        NMF_LOG("Unsufficient resources while allocating volume controler\n");
        RETURN_IF_OMX_ERROR( OMX_ErrorInsufficientResources );
    }

    RETURN_IF_OMX_ERROR(  new_instance->instantiate(mENSComponent.getNMFDomainHandle(), getPriorityLevel()));

    //Bind volctrl with FSM libraries
    RETURN_IF_OMX_ERROR( ENS::bindComponent( new_instance->getNmfMpcHandle(), "genericfsm", mNmfGenericFsmLib, "genericfsm"));
    RETURN_IF_OMX_ERROR( ENS::bindComponent( new_instance->getNmfMpcHandle(), "componentfsm", mNmfComponentFsmLib, "componentfsm"));;

    // Connect VolCtrl
    ENS_String<32> outputport      = direction != OMX_DirOutput ? "outputport"     : addSuffixPerPortNb("outputport");
    ENS_String<32> fillthisbuffer  = direction != OMX_DirOutput ? "fillthisbuffer" : addSuffixPerPortNb("fillthisbuffer");
    ENS_String<32> outputsettings  = direction != OMX_DirOutput ? "outputsettings" : addSuffixPerPortNb("outputsettings");
    ENS_String<32> emptythisbuffer = direction != OMX_DirOutput ? addSuffixPerPortNb("emptythisbuffer") : "emptythisbuffer";
    ENS_String<32> inputport       = direction != OMX_DirOutput ? addSuffixPerPortNb("inputport")       : "inputport";
    ENS_String<32> pcmsettings     = direction != OMX_DirOutput ? addSuffixPerPortNb("pcmsettings")     : "pcmsettings";

    t_cm_instance_handle & src_handle = direction == OMX_DirOutput ? mDmaBase->getNmfMpcHandle() : new_instance->getNmfMpcHandle();
    t_cm_instance_handle & dst_handle = direction == OMX_DirOutput ? new_instance->getNmfMpcHandle() : mDmaBase->getNmfMpcHandle();

    RETURN_IF_OMX_ERROR( ENS::bindComponent( src_handle, outputport, dst_handle, emptythisbuffer));
    RETURN_IF_OMX_ERROR( ENS::bindComponent( dst_handle, inputport,  src_handle, fillthisbuffer));
    RETURN_IF_OMX_ERROR( ENS::bindComponentAsynchronous( src_handle, outputsettings, dst_handle, pcmsettings    , FIFO_DEPTH_ONE));

    return OMX_ErrorNone;
}

OMX_ERRORTYPE AudiocodecBaseNmfMpc::destroyVolCtrl(NmfMpcVolCtrl * & volctrl, OMX_DIRTYPE direction)
{
    LOG_I("Enter.\n");

    if(!volctrl) return OMX_ErrorNone;

    //UnBind emitter with FSM libraries
    RETURN_IF_OMX_ERROR( ENS::unbindComponent(volctrl->getNmfMpcHandle(), "genericfsm"));
    RETURN_IF_OMX_ERROR( ENS::unbindComponent(volctrl->getNmfMpcHandle(), "componentfsm"));


    ENS_String<32> outputport      = direction != OMX_DirOutput ? "outputport"     : addSuffixPerPortNb("outputport");
    ENS_String<32> outputsettings  = direction != OMX_DirOutput ? "outputsettings" : addSuffixPerPortNb("outputsettings");
    ENS_String<32> inputport       = direction != OMX_DirOutput ? addSuffixPerPortNb("inputport")       : "inputport";
    ENS_String<32> pcmsettings     = direction != OMX_DirOutput ? addSuffixPerPortNb("pcmsettings")     : "pcmsettings";

    t_cm_instance_handle & src_handle = direction == OMX_DirOutput ? mDmaBase->getNmfMpcHandle() : volctrl->getNmfMpcHandle();
    t_cm_instance_handle & dst_handle = direction == OMX_DirOutput ? volctrl->getNmfMpcHandle()  : mDmaBase->getNmfMpcHandle();

    RETURN_IF_OMX_ERROR( ENS::unbindComponent( src_handle, outputport));
    RETURN_IF_OMX_ERROR( ENS::unbindComponent( dst_handle, inputport));
    RETURN_IF_OMX_ERROR( ENS::unbindComponentAsynchronous( src_handle, outputsettings));

    RETURN_IF_OMX_ERROR( volctrl->deInstantiate());

    delete volctrl;
    volctrl = 0;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE AudiocodecBaseNmfMpc::instanciateController()
{
    LOG_I("Enter.\n");

    DBC_ASSERT(mVolCtrl);

    RETURN_IF_OMX_ERROR(ENS::instantiateNMFComponent(
                mENSComponent.getNMFDomainHandle(), 
                mAudioCodecBase.getDirection() == OMX_DirInput ? "audiocodec.mpc.renderercontroller" : "audiocodec.mpc.capturercontroller",
                "controller", &mNmfMain, getPriorityLevel()));

    t_cm_instance_handle vol_handle  = mVolCtrl->getNmfMpcHandle();
    t_cm_instance_handle ctrl_handle = mNmfMain;
    t_cm_instance_handle & dma_handle= mDmaBase->getNmfMpcHandle();
    
    RETURN_IF_OMX_ERROR( bindComponent(             ctrl_handle,"osttrace"              , mOstTrace        , "osttrace"                ));
    RETURN_IF_OMX_ERROR( bindComponent(             ctrl_handle,"genericfsm"            , mNmfGenericFsmLib, "genericfsm"              ));
    RETURN_IF_OMX_ERROR( bindComponentAsynchronous( ctrl_handle,"inputblk_init"         , vol_handle       , "fsminit"              , FIFO_DEPTH_ONE));
    RETURN_IF_OMX_ERROR( bindComponentAsynchronous( ctrl_handle,"inputblk_sendcommand"  , vol_handle       , "sendcommand"          , FIFO_DEPTH_ONE));
    RETURN_IF_OMX_ERROR( bindComponentAsynchronous( vol_handle ,"proxy"                 , ctrl_handle      , "inputblk_eventhandler", FIFO_DEPTH_ONE));
    RETURN_IF_OMX_ERROR( bindComponent(             ctrl_handle, "dma_state", dma_handle, addSuffixPerPortNb("dma_state")));
    RETURN_IF_OMX_ERROR( bindComponentAsynchronous( dma_handle , addSuffixPerPortNb("dma_port_state"), ctrl_handle, "dma_port_state" , FIFO_DEPTH_ONE));

    return OMX_ErrorNone;
}

OMX_ERRORTYPE AudiocodecBaseNmfMpc::destroyController()
{
    LOG_I("Enter.\n");

    t_cm_instance_handle ctrl_handle = mNmfMain;
    t_cm_instance_handle vol_handle  = mVolCtrl->getNmfMpcHandle();
    t_cm_instance_handle & dma_handle = mDmaBase->getNmfMpcHandle();

    RETURN_IF_OMX_ERROR( unbindComponent(             ctrl_handle, "dma_state"           ));
    RETURN_IF_OMX_ERROR( unbindComponentAsynchronous( dma_handle , addSuffixPerPortNb("dma_port_state")));

    RETURN_IF_OMX_ERROR( unbindComponent(             ctrl_handle, "osttrace"            ));
    RETURN_IF_OMX_ERROR( unbindComponent(             ctrl_handle, "genericfsm"          ));
    RETURN_IF_OMX_ERROR( unbindComponentAsynchronous( ctrl_handle, "inputblk_init"       ));
    RETURN_IF_OMX_ERROR( unbindComponentAsynchronous( ctrl_handle, "inputblk_sendcommand"));
    RETURN_IF_OMX_ERROR( unbindComponentAsynchronous( vol_handle , "proxy"               ));

    RETURN_IF_OMX_ERROR( ENS::destroyNMFComponent(mNmfMain));

    return OMX_ErrorNone;
}

OMX_ERRORTYPE AudiocodecBaseNmfMpc::instantiateMain(void)
{
    LOG_I("Enter.\n");

    OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettings = ((AFM_PcmPort *) mENSComponent.getPort(0))->getPcmSettings();
    mSamplerate = pcmSettings.nSamplingRate;

    switch (mSamplerate) {
			  case 8000:
            mBlockSize = BT_BLOCKSIZE_5MS_NB;
            mAudioCodecBase.updatePcmSettings(mSamplerate, mBlockSize);
            break;
        case 16000:
            mBlockSize = BT_BLOCKSIZE_5MS_WB;
            mAudioCodecBase.updatePcmSettings(mSamplerate, mBlockSize);
            break;
        default:
            break;
    }

    mDmaBase = getNmfMpcDmaHandle(mAudioCodecBase);
    if (!mDmaBase) {
        RETURN_IF_OMX_ERROR( OMX_ErrorInsufficientResources );
    }

    RETURN_IF_OMX_ERROR(mDmaBase->instantiate( mENSComponent.getNMFDomainHandle() , getPriorityLevel()));

    RETURN_IF_OMX_ERROR(instantiateVolCtrl(mVolCtrl, mAudioCodecBase.getDirection(), OMX_MASTER_PORT_IDX  ));

    RETURN_IF_OMX_ERROR(instanciateController());

    return OMX_ErrorNone;
}

OMX_ERRORTYPE AudiocodecBaseNmfMpc::deInstantiateMain(void)
{
    LOG_I("Enter.\n");

    RETURN_IF_OMX_ERROR(destroyController());

    RETURN_IF_OMX_ERROR(destroyVolCtrl(mVolCtrl, mAudioCodecBase.getDirection()));

    RETURN_IF_OMX_ERROR(mDmaBase->deInstantiate());

    freeDmaHandle();
    return OMX_ErrorNone;
}

OMX_ERRORTYPE AudiocodecBaseNmfMpc::startMain(void)
{
    LOG_I("Enter.\n");

    RETURN_IF_OMX_ERROR( mDmaBase->start());

    RETURN_IF_OMX_ERROR( mVolCtrl->start());

    return OMX_ErrorNone;
}

OMX_ERRORTYPE AudiocodecBaseNmfMpc::stopMain(void)
{
    LOG_I("Enter.\n");

    RETURN_IF_OMX_ERROR( mDmaBase->stop());

    RETURN_IF_OMX_ERROR( mVolCtrl->stop());

    return OMX_ErrorNone;
}

OMX_ERRORTYPE AudiocodecBaseNmfMpc::configureMain(void)
{
    LOG_I("Enter.\n");

    RETURN_IF_OMX_ERROR(mDmaBase->configure(mSamplerate));
    RETURN_IF_OMX_ERROR(configureVolCtrl(mVolCtrl, OMX_MASTER_PORT_IDX ));

    //! Apply power configuration if supported
    if( mAudioCodecBase.getPowerConfig()){ 
        RETURN_IF_OMX_ERROR( applyConfig((OMX_INDEXTYPE )OMX_IndexConfigAudioPower, mAudioCodecBase.getPowerConfig()) );
    }

    //! Apply volume and mute configuration if supported
    // we supposte that if a sink or source supports volume, it will also support mute
    // we suppose that if a sink or source supports mute or volume config it will supported for all its channels
    // we test only on the channel 0 and this avoid us promlems in case volume is not supported
    // because in that case config struct is null but configstruct(1) is not set and can have any value 
    if (mAudioCodecBase.getVolumeChannelConfig(0)){
        for(unsigned int i = 0; i < mAudioCodecBase.getNbChannel(); i ++){
            RETURN_IF_OMX_ERROR( applyConfig(OMX_IndexConfigAudioChannelVolume, mAudioCodecBase.getVolumeChannelConfig(i)) );
            RETURN_IF_OMX_ERROR( applyConfig(OMX_IndexConfigAudioChannelMute, mAudioCodecBase.getMuteChannelConfig(i)) );
        }
    }

    return OMX_ErrorNone;
}

t_cm_instance_handle AudiocodecBaseNmfMpc::getNmfHandle(OMX_U32 omxPortIdx) const {  

    LOG_I("Enter.\n");

    if(omxPortIdx == 0){
        return (t_cm_instance_handle)mVolCtrl->getNmfHandle();
    }
    else{
        DBC_ASSERT(0);
    }
    return (t_cm_instance_handle)0;
}

//TODO: is there a conversion function for this somewhere
t_sample_freq AudiocodecBaseNmfMpc::getFreq(void){
    switch ( mAudioCodecBase.getSamplingRate() ){
        case 48000:
            return FREQ_48KHZ;

        case 16000:
            return FREQ_16KHZ;

        case 8000:
            return FREQ_8KHZ;

        default:	
            DBC_ASSERT(0);
            break;
    }
    return FREQ_UNKNOWN;
}

//!As set config is always called before Apply config, no need to check arguments
OMX_ERRORTYPE AudiocodecBaseNmfMpc::applyConfig( OMX_INDEXTYPE nConfigIndex,  OMX_PTR pComponentConfigStructure )
{
    LOG_I("Enter.\n");

    OMX_U32 nbChannels = mAudioCodecBase.getNbChannel();
    
    switch (nConfigIndex) {
        case OMX_IndexConfigAudioPower:
        {
            LOG_I("OMX_IndexConfigAudioPower.\n");

            OMX_AUDIO_CONFIG_POWERTYPE * pPower = (OMX_AUDIO_CONFIG_POWERTYPE *) pComponentConfigStructure;
            AUDIO_HAL_STATE common_switch   = pPower->bPower ? AUDIO_HAL_STATE_ON : AUDIO_HAL_STATE_OFF;

            LOG_I("Power = %d, Channel = %d\n", (int)pPower->bPower, (int)pPower->nChannel);

            if(pPower->nChannel == OMX_ALL) {
                for(OMX_U32 i = 0; i < nbChannels; i++) {
		    statusCB(audio_hal_set_power(i, common_switch, this->channel));
                    // audio_hal_set_power(mAudioChipsetApi.statusCB, i, common_switch, this->channel);
                }
            } else {
                DBC_ASSERT(pPower->nChannel < nbChannels );
                statusCB(audio_hal_set_power(pPower->nChannel, common_switch, this->channel));
            }
            return OMX_ErrorNone; 
        }
        case OMX_IndexConfigAudioVolume:
        case OMX_IndexConfigAudioMute:
        case OMX_IndexConfigAudioChannelVolume:
        case OMX_IndexConfigAudioChannelMute:
            return OMX_ErrorNone;
        default:
            return AFMNmfMpc_ProcessingComp::applyConfig(nConfigIndex, pComponentConfigStructure);
    }
}

OMX_ERRORTYPE AudiocodecBaseNmfMpc::retrieveConfig(
        OMX_INDEXTYPE nConfigIndex,  
        OMX_PTR pComponentConfigStructure)
{
    LOG_I("Enter.\n");

    if (pComponentConfigStructure == 0) {
        RETURN_OMX_ERROR( OMX_ErrorBadParameter );
    }
    OMX_U32 nbChannels = mAudioCodecBase.getNbChannel();

    switch (nConfigIndex) {
        case OMX_IndexConfigAudioVolume:
            {
                // Not supported by Alsactrl

                CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_AUDIO_CONFIG_VOLUMETYPE);
                OMX_AUDIO_CONFIG_VOLUMETYPE * pVolume = (OMX_AUDIO_CONFIG_VOLUMETYPE *) pComponentConfigStructure;

                if (pVolume->nPortIndex != OMX_MASTER_PORT_IDX) {
                    RETURN_OMX_ERROR( OMX_ErrorBadPortIndex );
                }

                pVolume->bLinear        = OMX_TRUE;
                pVolume->sVolume.nValue = 0;
                pVolume->sVolume.nMin   = 0;
                pVolume->sVolume.nMax   = 0;

                return OMX_ErrorNone;
            }

        case OMX_IndexConfigAudioMute:
            {
                // Not supported by Alsactrl

                CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_AUDIO_CONFIG_MUTETYPE);
                OMX_AUDIO_CONFIG_MUTETYPE * pMute = (OMX_AUDIO_CONFIG_MUTETYPE *) pComponentConfigStructure;

                if (pMute->nPortIndex != OMX_MASTER_PORT_IDX) {
                    RETURN_OMX_ERROR( OMX_ErrorBadPortIndex );
                }

                pMute->bMute = OMX_FALSE;

                return OMX_ErrorNone;
            }

        case OMX_IndexConfigAudioChannelVolume:
            {
                // Not supported by Alsactrl

                CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_AUDIO_CONFIG_CHANNELVOLUMETYPE);
                OMX_AUDIO_CONFIG_CHANNELVOLUMETYPE *pChannelVolume = (OMX_AUDIO_CONFIG_CHANNELVOLUMETYPE *) pComponentConfigStructure;

		pChannelVolume->bLinear = OMX_TRUE;
                pChannelVolume->sVolume.nMin = 0;
                pChannelVolume->sVolume.nMax = 0;
                pChannelVolume->sVolume.nValue = 0;

                return OMX_ErrorNone;
            }

        case OMX_IndexConfigAudioChannelMute:
            {
                // Not supported by Alsactrl

                CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_AUDIO_CONFIG_CHANNELMUTETYPE);
                OMX_AUDIO_CONFIG_CHANNELMUTETYPE *pChannelMute = (OMX_AUDIO_CONFIG_CHANNELMUTETYPE *) pComponentConfigStructure;

                if (pChannelMute->nPortIndex != OMX_MASTER_PORT_IDX) {
                    RETURN_OMX_ERROR( OMX_ErrorBadPortIndex );
                }

                pChannelMute->bMute = OMX_FALSE;

                return OMX_ErrorNone;
            }

        case OMX_IndexConfigAudioPower:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_AUDIO_CONFIG_POWERTYPE);
                OMX_AUDIO_CONFIG_POWERTYPE * pPower = (OMX_AUDIO_CONFIG_POWERTYPE *) pComponentConfigStructure;

                if(pPower->nPortIndex != OMX_MASTER_PORT_IDX) {
                    RETURN_OMX_ERROR( OMX_ErrorBadPortIndex );
                }

                if(pPower->nChannel < nbChannels ) { 
                    AUDIO_HAL_STATE power;
                    power = audio_hal_get_power(pPower->nChannel, this->channel);
                    if(power == AUDIO_HAL_STATE_ON) {
                        pPower->bPower = OMX_TRUE;
                    }
                    else if(power == AUDIO_HAL_STATE_OFF) {
                        pPower->bPower = OMX_FALSE;
                    }
                    else {
                        RETURN_OMX_ERROR( OMX_ErrorUnsupportedSetting );
                    }
                }
                else{    
                    RETURN_OMX_ERROR( OMX_ErrorBadParameter );
                }
                return OMX_ErrorNone; 
            }
        default:
            return AFMNmfMpc_ProcessingComp::retrieveConfig(
                    nConfigIndex, pComponentConfigStructure);
    }
}


//-----------------------------------------------------------------------------
//              class  AudioRendererNmfMpc
//-----------------------------------------------------------------------------
OMX_ERRORTYPE AudioRendererNmfMpc::instantiateMain(void)
{
    LOG_I("Enter.\n");

    RETURN_IF_OMX_ERROR(AudiocodecBaseNmfMpc::instantiateMain());

    if (mHasFeedbackPort) {
        //----------------------------
        // Instantiate Volctrl  for feedback
        //
        RETURN_IF_OMX_ERROR (instantiateVolCtrl(mFeedbackVolCtrl, OMX_DirOutput, 1/*Omx Port*/));
      
        t_cm_instance_handle & out_handle  = mFeedbackVolCtrl->getNmfMpcHandle();
        t_cm_instance_handle & ctrl_handle = mNmfMain;
        t_cm_instance_handle & dma_handle  = mDmaBase->getNmfMpcHandle();

        RETURN_IF_OMX_ERROR( bindComponent            ( ctrl_handle, "feedback_dma_state"     , dma_handle , addSuffixPerPortNb("feedback_dma_state")));

        RETURN_IF_OMX_ERROR( bindComponentAsynchronous( ctrl_handle, "feedbackblk_init"       , out_handle , "fsminit"                 , FIFO_DEPTH_ONE));
        RETURN_IF_OMX_ERROR( bindComponentAsynchronous( ctrl_handle, "feedbackblk_sendcommand", out_handle , "sendcommand"             , FIFO_DEPTH_ONE));
        RETURN_IF_OMX_ERROR( bindComponentAsynchronous( out_handle , "proxy"                  , ctrl_handle, "feedbackblk_eventhandler", 6 ));
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE AudioRendererNmfMpc::deInstantiateMain(void)
{
    LOG_I("Enter.\n");

    if (mHasFeedbackPort) {
        t_cm_instance_handle out_handle    = (t_cm_instance_handle)mFeedbackVolCtrl->getNmfHandle();
        t_cm_instance_handle & ctrl_handle = mNmfMain;

        RETURN_IF_OMX_ERROR( unbindComponent            ( ctrl_handle, "feedback_dma_state"     ));
        RETURN_IF_OMX_ERROR( unbindComponentAsynchronous( ctrl_handle, "feedbackblk_init"       ));
        RETURN_IF_OMX_ERROR( unbindComponentAsynchronous( ctrl_handle, "feedbackblk_sendcommand"));

        RETURN_IF_OMX_ERROR( unbindComponentAsynchronous( out_handle ,"proxy"));

        RETURN_IF_OMX_ERROR (destroyVolCtrl(mFeedbackVolCtrl, OMX_DirOutput));
    }

    RETURN_IF_OMX_ERROR( AudiocodecBaseNmfMpc::deInstantiateMain() );
    return OMX_ErrorNone;
}

OMX_ERRORTYPE AudioRendererNmfMpc::startMain(void)
{
    LOG_I("Enter.\n");

    RETURN_IF_OMX_ERROR( AudiocodecBaseNmfMpc::startMain() );

    if (mHasFeedbackPort) {
        RETURN_IF_OMX_ERROR(mFeedbackVolCtrl->start());
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE AudioRendererNmfMpc::stopMain(void)
{
    LOG_I("Enter.\n");

    RETURN_IF_OMX_ERROR( AudiocodecBaseNmfMpc::stopMain());

    if (mHasFeedbackPort) {
        RETURN_IF_OMX_ERROR( mFeedbackVolCtrl->stop());
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE AudioRendererNmfMpc::configureMain(void)
{
    LOG_I("Enter.\n");

    RETURN_IF_OMX_ERROR(AudiocodecBaseNmfMpc::configureMain());

    if (mHasFeedbackPort) {
        RETURN_IF_OMX_ERROR(configureVolCtrl(mFeedbackVolCtrl, OMX_FEEDBACK_PORT_IDX ));
    }

    return OMX_ErrorNone;
}

t_cm_instance_handle AudioRendererNmfMpc::getNmfHandle(OMX_U32 omxPortIdx) const
{  
    LOG_I("Enter.\n");

    if(omxPortIdx ==1){
        if (mHasFeedbackPort) {
            return (t_cm_instance_handle)mFeedbackVolCtrl->getNmfHandle();
        }
        else{
            DBC_ASSERT(0);
        }
    }

    return AudiocodecBaseNmfMpc::getNmfHandle(omxPortIdx);
}

OMX_ERRORTYPE AudioRendererNmfMpc::retrieveConfig(
        OMX_INDEXTYPE nConfigIndex,  
        OMX_PTR pComponentConfigStructure)
{
    LOG_I("Enter.\n");

    switch (nConfigIndex) {
        case OMX_IndexConfigAudioProcessedDataAmount:
            {
                OMX_SYMBIAN_AUDIO_CONFIG_PROCESSEDDATAAMOUNTTYPE * pConfig = (OMX_SYMBIAN_AUDIO_CONFIG_PROCESSEDDATAAMOUNTTYPE *)pComponentConfigStructure;

                // We compute "buffer comsumption time" not "real time"
                // One counter per ports. 
                pConfig->nProcessedDataAmount = mDmaBase->getTimeInUs(getInOutNmfPortIdx());
                OMX_TICKS p2                  = mDmaBase->getTimeInUs(getInOutNmfPortIdx());

                if (pConfig->nProcessedDataAmount == p2) {
                    return OMX_ErrorNone;
                }
                RETURN_OMX_ERROR( OMX_ErrorNotReady );
            }

        default:
            return AudiocodecBaseNmfMpc::retrieveConfig(
                    nConfigIndex, pComponentConfigStructure);
    }
}    

//-----------------------------------------------------------------------------
//              class AudioCapturerNmfMpc
//-----------------------------------------------------------------------------
OMX_ERRORTYPE AudioCapturerNmfMpc::retrieveConfig( OMX_INDEXTYPE nConfigIndex,  OMX_PTR pComponentConfigStructure) {
    switch (nConfigIndex) {
        case OMX_IndexConfigAudioProcessedDataAmount:
            {
                OMX_SYMBIAN_AUDIO_CONFIG_PROCESSEDDATAAMOUNTTYPE * pConfig = (OMX_SYMBIAN_AUDIO_CONFIG_PROCESSEDDATAAMOUNTTYPE *)pComponentConfigStructure;

                // Note that we have one different initial value per port.
                // This initial value is equal to getTimeInUs(0) when port is started 
                // Only sampledPlayedCounter(0) is updated by dma nmf component, on every "MSP RX Terminal Count" interrupts.
                OMX_S64 InitialValue = getStartTimeInUs();

                // Note that we share same counter for all sources.
                // We compute "real time" not "buffer comsumption time"
                pConfig->nProcessedDataAmount = mDmaBase->getTimeInUs(0) - InitialValue; 
                return OMX_ErrorNone;		
            }

        default:
            return AudiocodecBaseNmfMpc::retrieveConfig(
                    nConfigIndex, pComponentConfigStructure);
    }
}

OMX_U32 AudioCapturerNmfMpc::getNmfPortIndex(OMX_U32 omxPortIdx) const {
    // the capturer nmf port corresponding to the omx output port
    //(from pcmprocessings wrapper) is the output port, so 1
    if (omxPortIdx == 0)
    {
        return 1;
    }
    else
    {
        // this is not a pcm port...
        DBC_ASSERT(0);
        return 0;
    }
};


//-----------------------------------------------------------------------------
//              class AudioRendererAb8500NmfMpc
//-----------------------------------------------------------------------------
NmfMpcBase * AudioRendererAb8500NmfMpc::getNmfMpcDmaHandle(const AudiocodecBase & audiocodec){

    OMX_U32 monoBlockSizeInSamples = audiocodec.getDMABufferSize();
    OMX_U32 nbMspChannels          = audiocodec.getNbMspEnabledChannels();

    mDmaOut = NmfMpcOutAb8500::getHandle(monoBlockSizeInSamples, nbMspChannels);

    return mDmaOut;
}

void AudioRendererAb8500NmfMpc::freeDmaHandle(){
    mDmaOut->freeHandle();
    mDmaOut = static_cast<NmfMpcOutAb8500 *>(NULL);
}

OMX_ERRORTYPE AudioRendererAb8500NmfMpc::applyConfig(
        OMX_INDEXTYPE nConfigIndex,  
        OMX_PTR pComponentConfigStructure)
{
    switch (nConfigIndex) {
        case AFM_IndexLowPowerRenderer:
            {
                AFM_CONFIG_LOWPOWERTYPE *config = 
                    (AFM_CONFIG_LOWPOWERTYPE*)pComponentConfigStructure;

                if(config->nLowPower == OMX_TRUE){
                    // NULL as parameter, don't send an acknowledgement to RM
                    mDmaOut->startLowPowerTransition(NULL);
                }
                else if(config->nLowPower == OMX_FALSE){
                    // NULL as parameter, don't send an acknowledgement to RM
                    mDmaOut->startNormalModeTransition(NULL);
                }
                return OMX_ErrorNone;
            }
        case OMX_IndexConfigAudioMute:
            {
                OMX_AUDIO_CONFIG_MUTETYPE * pMute = (OMX_AUDIO_CONFIG_MUTETYPE *) pComponentConfigStructure;
                LOG_I("Renderer: OMX_IndexConfigAudioMute.\n");
                LOG_I("Mute=%d \n", pMute->bMute);
                if (mDmaOut && (pMute->bMute == OMX_TRUE))
                    mDmaOut->mutePort(mPortIndex, this);
            }
            return OMX_ErrorNone;

        default:
            return AudioRendererNmfMpc::applyConfig(
                    nConfigIndex, pComponentConfigStructure);
    }
}
OMX_ERRORTYPE AudioRendererAb8500NmfMpc::retrieveConfig(
        OMX_INDEXTYPE nConfigIndex,
        OMX_PTR pComponentConfigStructure)
{
    switch (nConfigIndex) {
        case OMX_IndexConfigAudioMute:
            return OMX_ErrorNone;

        default:
            return AudioRendererNmfMpc::retrieveConfig(
                    nConfigIndex, pComponentConfigStructure);
    }
}

void AudioRendererAb8500NmfMpc::invoke_mute(OMX_BOOL muted)
{
    mENSComponent.eventHandlerCB((OMX_EVENTTYPE)OMX_EventIndexSettingChanged, (OMX_U32)OMX_ALL, (OMX_U32)OMX_IndexConfigAudioMute, (OMX_PTR) 0);
}

//-----------------------------------------------------------------------------
//              class AudioCapturerAb8500NmfMpc
//-----------------------------------------------------------------------------
NmfMpcBase * AudioCapturerAb8500NmfMpc::getNmfMpcDmaHandle(const AudiocodecBase & audiocodec){
    OMX_U32 monoBlockSizeInSamples = audiocodec.getDMABufferSize();
    OMX_U32 nbMspChannels          = audiocodec.getNbMspEnabledChannels();

    mDmaIn = NmfMpcInAb8500::getHandle(monoBlockSizeInSamples, nbMspChannels);
    return mDmaIn;
}

void AudioCapturerAb8500NmfMpc::freeDmaHandle(){
    mDmaIn->freeHandle();
    mDmaIn = static_cast<NmfMpcInAb8500*>(NULL);
}



