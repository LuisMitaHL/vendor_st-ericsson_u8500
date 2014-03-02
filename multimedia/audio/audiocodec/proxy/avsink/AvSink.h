/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   AvSink.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _AvSink_H_
#define _AvSink_H_

#include "AudiocodecBase.h"
#include "AudiocodecBaseNmfMpc.h"
#include "wrapping_macros.h"

//! avsink factory method used when registering component to the ENS core
OMX_ERRORTYPE AvSinkFactoryMethod(ENS_Component_p * ppENSComponent);

//-----------------------------------------------------------------------------
//! \class AvSinkNmfMpc
//! \brief OMX Av stereo renderer Nmf Mpc proxy
//!
//-----------------------------------------------------------------------------
class AvSinkNmfMpc: public AudioRendererAb8500NmfMpc {
    public:
        //! \param audiocodec reference associated OMX component
        //! \param audioApi collection of callbacks for audio chipset apis
        AvSinkNmfMpc(AudiocodecBase & audiocodec, audio_hal_channel channel) 
            : AudioRendererAb8500NmfMpc(audiocodec, channel, AVSINK_INPUT_PORT_IDX, HAVE_FEEDBACK_PORT) {
                myself = static_cast<AvSinkNmfMpc *>(NULL); 
            }

        virtual ~AvSinkNmfMpc(){}

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
        static AvSinkNmfMpc * myself;

};

//-----------------------------------------------------------------------------
//! \class AVSink_RDB
//! \brief sets default domain
//-----------------------------------------------------------------------------
class AVSink_RDB : public ENS_ResourcesDB {
    public:
        //! \param nbOfDomains numbers of domain (one per port)
        AVSink_RDB(OMX_U32 nbOfDomains);
};

//-----------------------------------------------------------------------------
//! \class AVSink
//! \brief OMX avsink
//-----------------------------------------------------------------------------
class AVSink: public AudioRendererAb8500 {
    public:
        //! constructor that handle errors
        OMX_ERRORTYPE construct(void);
        
        virtual ~AVSink() {}

        //TODO remove useless pCompHdl parameter or give explanation about potential usage
        //! implement ENS interface to allows low power
        //virtual RM_STATUS_E getCapabilities(OMX_IN const OMX_PTR pCompHdl, OMX_INOUT OMX_U32* pCapBitmap){
        virtual RM_STATUS_E getCapabilities(OMX_INOUT OMX_U32 &pCapBitmap){
            //*pCapBitmap = RM_CAP_LOWPOWERCTRLSIG;
            pCapBitmap = RM_CAP_LOWPOWERCTRLSIG;
            return RM_E_NONE;    
        }

        //! instanciates a AVSink_RDB object 
        virtual OMX_ERRORTYPE createResourcesDB(void);

        //! retrieves dynamic configuration
        virtual OMX_ERRORTYPE getConfig(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentConfigStructure) const;

        //! sets dynamic configuration
        virtual OMX_ERRORTYPE setConfig(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentConfigStructure);

        //! Api called by RME to enable lowPower
        virtual OMX_ERRORTYPE StartLowPower(OMX_U32 LPADomainID);

        //! Api called by RME to enable lowPower
        virtual OMX_ERRORTYPE StartLowPower(void);
      
        //! Api called by RME to disable lowPower
        virtual OMX_ERRORTYPE StopLowPower();
};

#endif // _AvSink_H_
