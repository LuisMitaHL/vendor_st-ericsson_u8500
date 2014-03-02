/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   MultiMic.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _MultiMic_H_
#define _MultiMic_H_

#include "AudiocodecBase.h"
#include "AudiocodecBaseNmfMpc.h"
#include "wrapping_macros.h"

//! multi mic factory method used when registering component to the ENS core
OMX_ERRORTYPE multiMicFactoryMethod(ENS_Component_p * ppENSComponent);

//-----------------------------------------------------------------------------
//! \class MultiMicNmfMpc
//! \brief OMX Multi Mic apturer Nmf Mpc proxy
//!
//-----------------------------------------------------------------------------
class MultiMicNmfMpc: public AudioCapturerAb8500NmfMpc {
    public:
        //! \param audiocodec reference associated OMX component
        //! \param audioApi collection of callbacks for audio chipset apis
        MultiMicNmfMpc(AudiocodecBase & audiocodec, audio_hal_channel channel) 
            : AudioCapturerAb8500NmfMpc(audiocodec, channel, MULTIMIC_PORTIDX) {}
        virtual ~MultiMicNmfMpc(){}

        //! call open_channel
        virtual OMX_ERRORTYPE construct(void);
        //! call close channel
        virtual OMX_ERRORTYPE destroy(void);

        //! sets power, volume, mute... configuration
        virtual OMX_ERRORTYPE applyConfig(
                OMX_INDEXTYPE nConfigIndex,
                OMX_PTR pComponentConfigStructure);
        //! configures power, volume, mute... configuration
        virtual OMX_ERRORTYPE retrieveConfig(
                OMX_INDEXTYPE nConfigIndex,  
                OMX_PTR pComponentConfigStructure);

    protected:
        //! configure default power, volume, mute... 
        virtual OMX_ERRORTYPE configureMain(void);

    private:
        //TODO if callback can be called only once a time please implement singleton design pattern completly 
        static MultiMicNmfMpc * myself;

	 //! handles errors from audio_chipset_api
	void statusCB(AUDIO_HAL_STATUS status);
};

//-----------------------------------------------------------------------------
//! \class MultiMic
//! \brief OMX Multimic source
//-----------------------------------------------------------------------------
class MultiMic: public AudioCapturerAb8500 {
    public:
        //! constructor that handle errors
        OMX_ERRORTYPE construct(void);
   
        virtual ~MultiMic(){}

        //! retrieves dynamic configuration
        virtual OMX_ERRORTYPE getConfig(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentConfigStructure) const;

        //! sets dynamic configuration
        virtual OMX_ERRORTYPE setConfig(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentConfigStructure);

        OMX_AUDIO_CONFIG_HWLOOPCONTROLTYPE *getLoopControlConfig(){return &mConfigHwLoop;};

    private:
        OMX_AUDIO_CONFIG_HWLOOPCONTROLTYPE   mConfigHwLoop;
};

#endif // _MultiMic_H_
