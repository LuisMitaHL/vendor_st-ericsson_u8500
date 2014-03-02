/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   AudiocodecBase.h
 * \brief  Bases classes that ease OMX components implementation
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _AudiocodecBase_H_
#define _AudiocodecBase_H_

#include "AFM_Component.h"
#include "Audiocodec_macros.h"

#include "audio_chipset_api_index.h"
#include "audio_hw_control_chipset_api.h"

class AudiocodecBaseNmfMpc;
class AudioRendererNmfMpc;
class AudioCapturerNmfMpc;

/*typedef struct OMX_AUDIO_CONFIG_POWERTYPE  { 
     OMX_U32 nSize;                                    
     OMX_VERSIONTYPE nVersion;                   
     OMX_U32 nPortIndex;            
     OMX_U32 nChannel;
     OMX_BOOL bPower;
} OMX_AUDIO_CONFIG_POWERTYPE;*/

//-----------------------------------------------------------------------------
//! \class AudiocodecBase
//! \brief Base abstract class used by all OMX renderer and capturer (Ab8500 and BT)
//-----------------------------------------------------------------------------
class AudiocodecBase: public AFM_Component {
    public:
        AudiocodecBase();
        ~AudiocodecBase();

        //! \param eDirection indicates if OMX port 0 is an input or an output
        //! \param nBufferSizeMin number of samples of OMX port buffer
        //! \param nDmaBufferSize number of samples send by DMA for one channel
        //! \param nChannels number of channel (mono/stereo...)
        //! \param nSamplingRate Sampling rate in Hz
        //! \param aSupportVolumeConfig indicates if SHAI function related to volume/mute are provided
        //! \param nbOmxPort number of OMX port
        OMX_ERRORTYPE construct(
                OMX_DIRTYPE eDirection,
        		OMX_U32  nBufferSizeMin,
        		OMX_U32  nDmaBufferSize,
        		OMX_U32  nChannels,
                OMX_U32  nSamplingRate,
                OMX_BOOL aSupportVolumeConfig,
                OMX_U32  nbOmxPort = 1
        		);

        //! retrieves static parameter
        virtual OMX_ERRORTYPE getParameter(
                OMX_INDEXTYPE nParamIndex,  
                OMX_PTR pComponentParameterStructure) const;

        //! sets static parameters
        virtual OMX_ERRORTYPE setParameter(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentParameterStructure);

        //! retrieves dynamic configuration
        virtual OMX_ERRORTYPE getConfig(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentConfigStructure) const;
        
        virtual OMX_ERRORTYPE getExtensionIndex(OMX_STRING extension, OMX_INDEXTYPE* index) const;
        //! sets dynamic configuration
        virtual OMX_ERRORTYPE setConfig(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentConfigStructure);

        //! fills in resources estimation request from RME
        AUDIOCODEC_API_IMPORT virtual RM_STATUS_E getResourcesEstimation(
               const OMX_PTR pCompHdl,
               RM_EMDATA_T* pEstimationData);

        //! returns number of samples send by DMA for one channel
        inline int getDMABufferSize(void) const { return mBufferSize; }

        //! returns number of enabled Msp channels
        inline OMX_U32      getNbMspEnabledChannels(void) const { return mMspNbChannel; }
        //! returns number of channels (mono / stereo)
        inline OMX_U32      getNbChannel(void)            const { return mNbChannel   ; }
        //! returns Sampling rate in Hz
        inline OMX_U32      getSamplingRate(void)         const { return mSamplingRate; }
        //! returns direction of OMX port 0 (input or output)
        inline OMX_DIRTYPE  getDirection(void)            const { return mDirection   ; }

        //! updates the pcm settings. Only to be done when switching between 8 and 16 kHz in BT
        OMX_ERRORTYPE                      updatePcmSettings(OMX_U32 samplerate, OMX_U32 blocksize);
        //! returns Volumes configuration
        OMX_AUDIO_CONFIG_CHANNELVOLUMETYPE * getVolumeChannelConfig(OMX_U32 channelIndex) const;
        //! returns Mute configuration
        OMX_AUDIO_CONFIG_CHANNELMUTETYPE   * getMuteChannelConfig(OMX_U32 channelIndex) const;
        //! returns Power configuration
        OMX_AUDIO_CONFIG_POWERTYPE         * getPowerConfig(void) const;

        virtual OMX_U32 getUidTopDictionnary(void);
        
    protected:
        //! stores number of samples of OMX port buffer
        OMX_U32                 mBufferSize;
        //! stores number of samples send by DMA for one channel
        OMX_U32                 mMspNbChannel;
        
        //! returns a default Pcm Setting
        //! \param nChannels number of channel (mono/stereo...)
        //! \param nSamplingRate Sampling rate in Hz
        OMX_AUDIO_PARAM_PCMMODETYPE getDefaultPcmSettings(OMX_U32 nChannels, OMX_U32 nSamplingRate) const ;

    private:
        //! stores direction of OMX port 0 (input or output)
        OMX_DIRTYPE             mDirection;
        //! stores number of channels (mono / stereo)
        OMX_U32                 mNbChannel;
        //! stores Sampling rate in Hz
        OMX_U32                 mSamplingRate;

        //! stores Volumes configuration
        OMX_AUDIO_CONFIG_CHANNELVOLUMETYPE * mConfigVolumeChannel;
        //! stores Mute configuration
        OMX_AUDIO_CONFIG_CHANNELMUTETYPE   * mConfigMuteChannel;
        //! stores Power configuration
        OMX_AUDIO_CONFIG_POWERTYPE         * mConfigPower;

        //! stores Volume configuration supported
        OMX_BOOL mSupportVolumeConfig;
};

//-----------------------------------------------------------------------------
//! \class AudioRenderer
//! \brief Base abstract class used by all OMX renderer (Ab8500 and BT)
//-----------------------------------------------------------------------------
class AudioRenderer: public AudiocodecBase {
    public:
        //! \param nBufferSizeMin number of samples of OMX port buffer
        //! \param nDmaBufferSize number of samples send by DMA for one channel
        //! \param nChannels number of channel (mono/stereo...)
        //! \param nSamplingRate Sampling rate in Hz
        //! \param aSupportVolumeConfig indicates if SHAI function related to volume/mute are provided
        //! \param hasFeedbackPort indicates if sink has a reference Omx port
        OMX_ERRORTYPE construct(
                OMX_U32  nBufferSizeMin,
                OMX_U32  nDmaBufferSize,
                OMX_U32  nChannels,
                OMX_U32  nSamplingRate,
                OMX_BOOL aSupportVolumeConfig = OMX_FALSE,
                OMX_BOOL hasFeedbackPort      = OMX_FALSE
                );

        //! sets dynamic configuration
        virtual	OMX_ERRORTYPE setConfig(
                    OMX_INDEXTYPE nParamIndex,
                    OMX_PTR pComponentConfigStructure);
        
        //! retrieves dynamic configuration
        virtual OMX_ERRORTYPE getConfig(
                    OMX_INDEXTYPE nParamIndex,
                    OMX_PTR pComponentConfigStructure) const;
                    
        virtual OMX_ERRORTYPE getParameter(
	         OMX_INDEXTYPE nParamIndex,
	         OMX_PTR pComponentParameterStructure) const;

        //! returns associates Nmp Mpc proxy
        AudioRendererNmfMpc * getAudioRendererNmfMpc(void) const;
};

//-----------------------------------------------------------------------------
//! \class AudioCapturer
//! \brief Base abstract class used by all OMX capturer (Ab8500 and BT)
//-----------------------------------------------------------------------------
class AudioCapturer: public AudiocodecBase {
    public:
        //! \param nBufferSizeMin number of samples of OMX port buffer
        //! \param nDmaBufferSize number of samples send by DMA for one channel
        //! \param nChannels number of channel (mono/stereo...)
        //! \param nSamplingRate Sampling rate in Hz
        //! \param aSupportVolumeConfig indicates if SHAI function related to volume/mute are provided
        OMX_ERRORTYPE construct(
                OMX_U32  nBufferSizeMin,
                OMX_U32  nDmaBufferSize,
                OMX_U32  nChannels,
                OMX_U32  nSamplingRate,
                OMX_BOOL aSupportVolumeConfig
                );

        //! retrieves static parameter
        virtual OMX_ERRORTYPE getParameter(
	         OMX_INDEXTYPE nParamIndex,
	         OMX_PTR pComponentParameterStructure) const;

        //! sets static parameters
        virtual OMX_ERRORTYPE setParameter(
            OMX_INDEXTYPE nParamIndex,
            OMX_PTR pComponentParameterStructure);

        //! retrieves dynamic configuration
		OMX_ERRORTYPE getConfig(
             OMX_INDEXTYPE nParamIndex,
             OMX_PTR pComponentConfigStructure) const;

        //! returns associates Nmp Mpc proxy
        AudioCapturerNmfMpc * getAudioCapturerNmfMpc(void) const;
};

//-----------------------------------------------------------------------------
//! \class AudioRendererAb8500
//! \brief Base abstract class used by all OMX Ab8500 renderer
//-----------------------------------------------------------------------------
class AudioRendererAb8500: public AudioRenderer {
    public:
        //! \param nChannels number of channel (mono/stereo...)
        //! \param aSupportVolumeConfig indicates if SHAI function related to volume/mute are provided
        //! \param hasFeedbackPort indicates if sink has a reference Omx port
        OMX_ERRORTYPE construct(
                OMX_U32 nChannels,
                OMX_BOOL aSupportVolumeConfig = OMX_FALSE,
                OMX_BOOL hasFeedbackPort      = OMX_FALSE
                );

        //! retrieves dynamic configuration
        virtual OMX_ERRORTYPE getConfig(
                    OMX_INDEXTYPE nParamIndex,
                    OMX_PTR pComponentConfigStructure) const;

        //! sets dynamic configuration
        virtual	OMX_ERRORTYPE setConfig(
                    OMX_INDEXTYPE nParamIndex,
                    OMX_PTR pComponentConfigStructure);

        virtual OMX_ERRORTYPE getParameter(
	         OMX_INDEXTYPE nParamIndex,
	         OMX_PTR pComponentParameterStructure) const;

    protected:
        //TODO: move this in avsink only
        //! stores information if Low power is enabled or not
        OMX_BOOL    mLowPowerEnabled;
};

//-----------------------------------------------------------------------------
//! \class AudioCapturerAb8500
//! \brief Base abstract class used by all OMX Ab8500 capturer
//-----------------------------------------------------------------------------
class AudioCapturerAb8500: public AudioCapturer {
    public:
        //! \param nChannels number of channel (mono/stereo...)
        //! \param aSupportVolumeConfig indicates if SHAI function related to volume/mute are provided
        OMX_ERRORTYPE construct(
                OMX_U32 nChannels,
                OMX_BOOL aSupportVolumeConfig
                );

        //! retrieves static parameter
        virtual OMX_ERRORTYPE setParameter(
            OMX_INDEXTYPE nParamIndex,
            OMX_PTR pComponentParameterStructure);

        //! sets static parameters
        virtual OMX_ERRORTYPE getParameter(
	         OMX_INDEXTYPE nParamIndex,
	         OMX_PTR pComponentParameterStructure) const;

        //! retrieves dynamic configuration
		OMX_ERRORTYPE getConfig(
             OMX_INDEXTYPE nParamIndex,
             OMX_PTR pComponentConfigStructure) const; 
};

#endif // _AudiocodecBase_H_
