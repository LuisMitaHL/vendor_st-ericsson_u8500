/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   BtTx.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _BtTx_H_
#define _BtTx_H_

#include "AudiocodecBase.h"
#include "AudiocodecBaseNmfMpc.h"
#include "wrapping_macros.h"

class NmfMpcOutBT;

//! bluetooth mono renderer factory method used when registering the AvSink component to the ENS core
OMX_ERRORTYPE BtTxFactoryMethod(ENS_Component_p * ppENSComponent);

//-----------------------------------------------------------------------------
//! \class BtTxNmfMpc
//! \brief OMX Bluetooth mono renderer Nmf Mpc proxy
//!
//-----------------------------------------------------------------------------
class BtTxNmfMpc: public AudioRendererNmfMpc {
    public:
        //FIXME: informatinon of HAS_FEEDBACK is duplicated
        //! \param audiocodec reference associated OMX component
        //! \param audioApi collection of callbacks for audio chipset apis
        BtTxNmfMpc(AudiocodecBase & audiocodec, audio_hal_channel channel) 
            : AudioRendererNmfMpc(audiocodec, channel, BT_NMF_PORT_IDX, HAVE_FEEDBACK_PORT) {
                myself = static_cast<BtTxNmfMpc *>(NULL); 
                mDmaBTOut = (NmfMpcOutBT *)0;
            }

        virtual ~BtTxNmfMpc(){}

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

        //! creates unique instance of "dmaout_bt"
        virtual NmfMpcBase * getNmfMpcDmaHandle(const AudiocodecBase & audiocodec);
        //! frees unique instance of "dmaout_bt"
        virtual void         freeDmaHandle(void);

        //! returns str concataned with a suffix "[nmfportnb]" if needed
        //! \note As BT have only one port str is directly returned
        // TODO remove virtual if BT aligned
        virtual ENS_String<32> addSuffixPerPortNb(const char * str);

        //! handles errors from audio_chipset_api
        void statusCB(AUDIO_HAL_STATUS status);

    private:
        //TODO if callback can be called only once a time please implement singleton design pattern completly 
        static BtTxNmfMpc * myself;

        //! Pointer on Dma Nmf Mpc singleton 
        NmfMpcOutBT *              mDmaBTOut;

};


//-----------------------------------------------------------------------------
//! \class BtTx
//! \brief OMX bluetooth mono sink
//-----------------------------------------------------------------------------
class BtTx: public AudioRenderer {
    public:
        //! constructor that handle errors
        OMX_ERRORTYPE construct(void);
        
        virtual ~BtTx(){}

        //! retrieves dynamic configuration
        virtual OMX_ERRORTYPE getConfig(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentConfigStructure) const;
        
        //! sets dynamic configuration
        virtual OMX_ERRORTYPE setConfig(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentConfigStructure);
};

#endif // _BtTx_H_
