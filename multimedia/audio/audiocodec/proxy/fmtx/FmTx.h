/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   FmTx.h
 * \brief  FM sink  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _FmTx_H_
#define _FmTx_H_

#include "AudiocodecBase.h"
#include "AudiocodecBaseNmfMpc.h"
#include "wrapping_macros.h"

//! FM sink factory method used when registering component to the ENS core
OMX_ERRORTYPE FmTxFactoryMethod(ENS_Component_p * ppENSComponent);

//-----------------------------------------------------------------------------
//! \class FmTxNmfMpc
//! \brief OMX FM stereo renderer Nmf Mpc proxy
//!
//-----------------------------------------------------------------------------
class FmTxNmfMpc: public AudioRendererAb8500NmfMpc {
    public:
        //! \param audiocodec reference associated OMX component
        //! \param audioApi collection of callbacks for audio chipset apis
        FmTxNmfMpc(AudiocodecBase & audiocodec, audio_hal_channel channel) 
            : AudioRendererAb8500NmfMpc(audiocodec, channel, FMTX_INPUT_PORT_IDX, DO_NOT_HAVE_FEEDBACK_PORT) {
                myself = static_cast<FmTxNmfMpc *>(NULL); 
            }

        virtual ~FmTxNmfMpc(){}

        //! call open_channel
        virtual OMX_ERRORTYPE construct(void);
        //! call close channel
        virtual OMX_ERRORTYPE destroy(void);
        //OMX_ERRORTYPE destroy(void);

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
        static FmTxNmfMpc * myself;

};


//-----------------------------------------------------------------------------
//! \class FmTx
//! \brief OMX FM sink
//-----------------------------------------------------------------------------
class FmTx: public AudioRendererAb8500 {
    public:
        //! constructor that handle errors
        OMX_ERRORTYPE construct(void);
       
        virtual ~FmTx(){}

        //! retrieves dynamic configuration
        virtual OMX_ERRORTYPE getConfig(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentConfigStructure) const;
      
        //! sets dynamic configuration
        virtual OMX_ERRORTYPE setConfig(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentConfigStructure);
};

#endif // _FmTx_H_
