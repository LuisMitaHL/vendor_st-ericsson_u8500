/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \brief   OSI Implementation of Vibra Sink
 * \author  ST-Ericsson
 */
/*****************************************************************************/

#ifndef _VIBRA_H_
#define _VIBRA_H_


#include <audio_chipset_api_index.h>
//#include <haptics_chipset_api.h>

#include "AudiocodecBase.h"
#include "AudiocodecBaseNmfMpc.h"
#include "wrapping_macros.h"

typedef struct OMX_AUDIO_PARAM_HAPTICSDRIVERPROPERTIESTYPE
{
	OMX_U32 nSize;
	OMX_VERSIONTYPE nVersion;
	OMX_U32 nPortIndex;
	OMX_U32 nNativeSamplingRate;
	OMX_U32 nHapticsDriverNominalVoltage;
	OMX_U32 nHapticsDriverImpedance;
} OMX_AUDIO_PARAM_HAPTICSDRIVERPROPERTIESTYPE;

//! Vibra1 factory method used when registering component to the ENS core
OMX_ERRORTYPE vibra1FactoryMethod(ENS_Component_p * ppENSComponent);
//! Vibra2 factory method used when registering component to the ENS core
OMX_ERRORTYPE vibra2FactoryMethod(ENS_Component_p * ppENSComponent);

//-----------------------------------------------------------------------------
//! \class VibraBaseNmfMpc
//! \brief Base class for OMX Vibra(s) renderer Nmf Mpc proxy
//!
//-----------------------------------------------------------------------------
class VibraBaseNmfMpc: public AudioRendererAb8500NmfMpc {
    public:
        //! \param audiocodec reference associated OMX component
        //! \param audioApi collection of callbacks for audio chipset apis
        //! \param portIndex of Nmf Mpc components used for this source/sink
        VibraBaseNmfMpc(AudiocodecBase & audiocodec, audio_hal_channel channel, OMX_U32 portIndex)
            : AudioRendererAb8500NmfMpc(audiocodec, channel, portIndex) { }
 
        //! sets power, volume, mute... configuration
        virtual OMX_ERRORTYPE applyConfig(
                OMX_INDEXTYPE nConfigIndex,
                OMX_PTR pComponentConfigStructure);
        //! configures power, volume, mute... configuration
        virtual OMX_ERRORTYPE retrieveConfig(
                OMX_INDEXTYPE nConfigIndex,  
                OMX_PTR pComponentConfigStructure);        

    protected:
        //TODO//! configure default power, volume, mute... 
        virtual OMX_ERRORTYPE configureMain(void);
};

//-----------------------------------------------------------------------------
//! \class Vibra1NmfMpc
//! \brief OMX Vibra1 renderer Nmf Mpc proxy
//!
//-----------------------------------------------------------------------------
class Vibra1NmfMpc: public VibraBaseNmfMpc {
    public:
        //! \param audiocodec reference associated OMX component
        //! \param audioApi collection of callbacks for audio chipset apis
        Vibra1NmfMpc(AudiocodecBase & audiocodec, audio_hal_channel channel)
            : VibraBaseNmfMpc(audiocodec, channel, VIBRAL_INPUT_PORT_IDX ) {
                myself = static_cast<Vibra1NmfMpc *>(NULL); 
            }

        //! call open_channel
        virtual OMX_ERRORTYPE construct(void);
        //! call close channel
        virtual OMX_ERRORTYPE destroy(void);

        //! handles errors from audio_chipset_api
        void statusCB(AUDIO_HAL_STATUS status);
	
    private:

        //TODO if callback can be called only once a time please implement singleton design pattern completly 
        static Vibra1NmfMpc * myself;

};

//-----------------------------------------------------------------------------
//! \class Vibra2NmfMpc
//! \brief OMX Vibra2 renderer Nmf Mpc proxy
//!
//-----------------------------------------------------------------------------
class Vibra2NmfMpc: public VibraBaseNmfMpc {
    public:
        //! \param audiocodec reference associated OMX component
        //! \param audioApi collection of callbacks for audio chipset apis
        Vibra2NmfMpc(AudiocodecBase & audiocodec, audio_hal_channel channel)
            : VibraBaseNmfMpc(audiocodec, channel, VIBRAR_INPUT_PORT_IDX ) {
                myself = static_cast<Vibra2NmfMpc *>(NULL); 
            }

        //! call open_channel
        virtual OMX_ERRORTYPE construct(void);
        //! call close channel
        virtual OMX_ERRORTYPE destroy(void);
       
        //! handles errors from audio_chipset_api
        void statusCB(AUDIO_HAL_STATUS status);
	
    private:

        //TODO if callback can be called only once a time please implement singleton design pattern completly 
        static Vibra2NmfMpc * myself;
};


//-----------------------------------------------------------------------------
//! \class Vibra
//! \brief OMX vibra(s) sink
//-----------------------------------------------------------------------------
class Vibra: public AudioRendererAb8500 {
    public:
        //! constructor that handle errors
        OMX_ERRORTYPE construct(void);

        virtual ~Vibra(){}

        //! retrieves static parameters
        virtual OMX_ERRORTYPE getParameter(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentParameterStructure) const;

    private:
        //! stores Haptics propeties parameters
        OMX_AUDIO_PARAM_HAPTICSDRIVERPROPERTIESTYPE iHapticsDriverProperties;
};

#endif //_Vibra_H_
