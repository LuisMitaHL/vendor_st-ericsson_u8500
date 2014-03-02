/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   FmRx.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _FmRx_H_
#define _FmRx_H_

#include "AudiocodecBase.h"
#include "AudiocodecBaseNmfMpc.h"
#include "wrapping_macros.h"

//! Fm source factory method used when registering component to the ENS core
OMX_ERRORTYPE FmRxFactoryMethod(ENS_Component_p * ppENSComponent);

//-----------------------------------------------------------------------------
//! \class FmRxNmfMpc
//! \brief OMX FM stereo capturer Nmf Mpc proxy
//!
//-----------------------------------------------------------------------------
class FmRxNmfMpc: public AudioCapturerAb8500NmfMpc {
    public:
        //! \param audiocodec reference associated OMX component
        //! \param audioApi collection of callbacks for audio chipset apis
        FmRxNmfMpc(AudiocodecBase & audiocodec, audio_hal_channel channel) 
            : AudioCapturerAb8500NmfMpc(audiocodec, channel, FMRX_PORTIDX) {
                myself = static_cast<FmRxNmfMpc *>(NULL); 
            }
        virtual ~FmRxNmfMpc(){}

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
        static FmRxNmfMpc * myself;
};

//-----------------------------------------------------------------------------
//! \class FmRx
//! \brief OMX FM source
//-----------------------------------------------------------------------------
class FmRx: public AudioCapturerAb8500 {
    public:
        //! constructor that handle errors
        OMX_ERRORTYPE construct(void);
        virtual ~FmRx(){}

        //! retrieves dynamic configuration
        virtual OMX_ERRORTYPE getConfig(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentConfigStructure) const;

        //! sets dynamic configuration
        virtual OMX_ERRORTYPE setConfig(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentConfigStructure);

};

#endif // _FmRx_H_
