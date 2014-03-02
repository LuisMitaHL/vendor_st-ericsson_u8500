/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   AvSource.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _AvSource_H_
#define _AvSource_H_

#include "AudiocodecBase.h"
#include "AudiocodecBaseNmfMpc.h"
#include "wrapping_macros.h"

//! avsource factory method used when registering the AvSink component to the ENS core
OMX_ERRORTYPE AvSourceFactoryMethod(ENS_Component_p * ppENSComponent);

//-----------------------------------------------------------------------------
//! \class AvSourceNmfMpc
//! \brief OMX Av mono capturer Nmf Mpc proxy
//!
//-----------------------------------------------------------------------------
class AvSourceNmfMpc: public AudioCapturerAb8500NmfMpc {
    public:
        //! \param audiocodec reference associated OMX component
        //! \param audioApi collection of callbacks for audio chipset apis
        AvSourceNmfMpc(AudiocodecBase & audiocodec, audio_hal_channel channel) 
            : AudioCapturerAb8500NmfMpc(audiocodec, channel, AVSOURCE_PORTIDX){
                myself = static_cast<AvSourceNmfMpc *>(NULL); 
            }

        virtual ~AvSourceNmfMpc() {}

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

        //! handles errors from audio_chipset_api
        void statusCB(AUDIO_HAL_STATUS status);

    protected:
        //! configure default power, volume, mute... 
        virtual OMX_ERRORTYPE configureMain(void);

    private:
        //TODO if callback can be called only once a time please implement singleton design pattern completly 
        static AvSourceNmfMpc * myself;
};

//-----------------------------------------------------------------------------
//! \class AvSource
//! \brief OMX avsource
//-----------------------------------------------------------------------------
class AvSource: public AudioCapturerAb8500 {
    public:
        //! constructor that handle errors
        OMX_ERRORTYPE construct(void);

        virtual ~AvSource(){}

        //! retrieves dynamic configuration
        virtual OMX_ERRORTYPE getConfig(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentConfigStructure) const;

        //! sets dynamic configuration
        virtual OMX_ERRORTYPE setConfig(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentConfigStructure);
};

#endif // _AvSource_H_
