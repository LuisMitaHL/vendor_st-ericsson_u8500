/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   Earpiece.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _Earpiece_H_
#define _Earpiece_H_

#include "AudiocodecBase.h"
#include "AudiocodecBaseNmfMpc.h"
#include "wrapping_macros.h"

//! earpiece factory method used when registering the AvSink component to the ENS core
OMX_ERRORTYPE earpieceFactoryMethod(ENS_Component_p * ppENSComponent);

//-----------------------------------------------------------------------------
//! \class EarpieceNmfMpc
//! \brief OMX Earpiece mono renderer Nmf Mpc proxy
//!
//-----------------------------------------------------------------------------
class EarpieceNmfMpc: public AudioRendererAb8500NmfMpc {
    public:
        //! \param audiocodec reference associated OMX component
        //! \param audioApi collection of callbacks for audio chipset apis
        EarpieceNmfMpc(AudiocodecBase &audiocodec, audio_hal_channel channel) 
            : AudioRendererAb8500NmfMpc(audiocodec, channel, EAR_INPUT_PORT_IDX, HAVE_FEEDBACK_PORT) {
                myself = static_cast<EarpieceNmfMpc *>(NULL); 
            }

        virtual ~EarpieceNmfMpc(){}

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
        static EarpieceNmfMpc * myself;

};

//-----------------------------------------------------------------------------
//! \class Earpiece
//! \brief OMX earpiece source
//-----------------------------------------------------------------------------
class Earpiece: public AudioRendererAb8500 {
    public:
        //! constructor that handle errors
        OMX_ERRORTYPE construct(void);

        virtual ~Earpiece(){}

        //! retrieves dynamic configuration
        virtual OMX_ERRORTYPE getConfig(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentConfigStructure) const;
     
        //! sets dynamic configuration
        virtual OMX_ERRORTYPE setConfig(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentConfigStructure);
};

#endif // _Earpiece_H_
