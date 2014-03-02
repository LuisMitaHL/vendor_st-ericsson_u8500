/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   BtRx.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _BtRx_H_
#define _BtRx_H_

#include "AudiocodecBase.h"
#include "AudiocodecBaseNmfMpc.h"
#include "wrapping_macros.h"
class NmfMpcInBT;


//! BtRx factory method used when registering the AvSink component to the ENS core
OMX_ERRORTYPE BtRxFactoryMethod(ENS_Component_p * ppENSComponent);


//-----------------------------------------------------------------------------
//! \class BtRxNmfMpc
//! \brief OMX Bluetooth mono capturer Nmf Mpc proxy
//!
//-----------------------------------------------------------------------------
class BtRxNmfMpc: public AudioCapturerNmfMpc {
    public:
        //! \param audiocodec reference associated OMX component
        //! \param audioApi collection of callbacks for audio chipset apis
        BtRxNmfMpc(AudiocodecBase & audiocodec, audio_hal_channel channel) 
            : AudioCapturerNmfMpc(audiocodec, channel) {
                myself = static_cast<BtRxNmfMpc *>(NULL); 
                mDmaBTIn = (NmfMpcInBT *) 0;
            }

        virtual ~BtRxNmfMpc(){}

        //! call open_channel
        virtual OMX_ERRORTYPE construct(void);
        //! call close channel
        virtual OMX_ERRORTYPE destroy(void);	

        //! bypass volume related stuff, as it is not supported today (10/10/2010)
        virtual OMX_ERRORTYPE applyConfig(
                OMX_INDEXTYPE nConfigIndex,
                OMX_PTR pComponentConfigStructure);
        //! bypass volume related stuff, as it is not supported today (10/10/2010)
        virtual OMX_ERRORTYPE retrieveConfig(
                OMX_INDEXTYPE nConfigIndex,  
                OMX_PTR pComponentConfigStructure);        

        //! creates unique instance of "dmain_bt"
        virtual NmfMpcBase * getNmfMpcDmaHandle(const AudiocodecBase & audiocodec);
        //! frees unique instance of "dmain_bt"
        virtual void         freeDmaHandle(void);

        //! returns str concataned with a suffix "[nmfportnb]" if needed
        //! \note As BT have only one port str is directly returned
        // TODO remove virtual if BT aligned
        virtual ENS_String<32> addSuffixPerPortNb(const char * str);

        //! handles errors from audio_chipset_api
        void statusCB(AUDIO_HAL_STATUS status);

    private:
        //TODO if callback can be called only once a time please implement singleton design pattern completly 
        static BtRxNmfMpc * myself;

        //! Pointer on Dma Nmf Mpc singleton 
        NmfMpcInBT *              mDmaBTIn;
};


//-----------------------------------------------------------------------------
//! \class BtRx
//! \brief OMX bluetooth mono capturer
//!
//-----------------------------------------------------------------------------
class BtRx: public AudioCapturer {
    public:
        //! constructor that handle errors
        OMX_ERRORTYPE construct(void);

        virtual ~BtRx(){}

        //! retrieves dynamic configuration
        virtual OMX_ERRORTYPE getConfig(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentConfigStructure) const;
        
        //! sets dynamic configuration
        virtual OMX_ERRORTYPE setConfig(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentConfigStructure);
};

#endif // _BtRx_H_
