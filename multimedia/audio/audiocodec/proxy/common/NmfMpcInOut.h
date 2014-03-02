/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   NmfMpcInOut.h
 * \brief  Input / Output for Ab8500 and BT Nmf Mpc proxies
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef  _NMFMPCDMAOUT_H_
#define  _NMFMPCDMAOUT_H_
#include "BindContext.h"
#include "NmfMpcBase.h"

#include "host/dmaout/low_power.hpp"
#include "host/dmaout/low_signal.hpp"
#include "AudiocodecBaseNmfMpc.h"

//! defines number of port for Ab8500 capturer Nmf Mpc component
const unsigned int AB_8500_DMAIN_NB_MNF_PORTS  = 3;
//! defines number of port for Ab8500 renderer Nmf Mpc component
const unsigned int AB_8500_DMAOUT_NB_MNF_PORTS = 6;

//! defines number of stereo slots in Ring Buffer for Ab8500 renderer Nmf Mpc component
const unsigned int AB_8500_RING_BUFFER_SLOTS   = 64;

//-----------------------------------------------------------------------------
//! \class NmfMpcOutAb8500
//! \brief Proxy of Nmf Mpc component "dmaout"
//! 
//! This singleton is used for Ab8500 renderer
//! 
//-----------------------------------------------------------------------------
class NmfMpcOutAb8500 : public NmfMpcBaseAb8500 ,public dmaout_low_signalDescriptor{

    public:
        //! singleton: creates unique instance on first call
        //! \param monoBlockSizeInSamples size of one channel in samples
		//! \param nbMspChannels limits channels send to MSP by DMA
        static NmfMpcOutAb8500 * getHandle(OMX_U32 monoBlockSizeInSamples, OMX_U32 nbMspChannels);
        //! singleton: destroy unique instance on last call
        void                     freeHandle(void);

        //! instantiates/binds libraries used by Nmp Mpc component
        virtual OMX_ERRORTYPE instantiateLibraries(OMX_U32 domainId, OMX_U32 priority);
        //! destroys/unbinds libraries used by Nmp Mpc component
        virtual OMX_ERRORTYPE deInstantiateLibraries(void);

        //! starts burst mode / Low power 
        void startLowPowerTransition(OMX_HANDLETYPE hdlToBeAcknowledged);

        void startLowPowerTransition(OMX_U32 LPADomain, OMX_HANDLETYPE hdlToBeAcknowledged);

        //! stops burst mode / Low power 
        void startNormalModeTransition(OMX_HANDLETYPE hdlToBeAcknowledged);

        void mutePort(t_uint16 port_idx, AudioRendererAb8500NmfMpc *client);

        //! acknowledges to RME that burst mode has been entered
        void  acknowledgeRmForLowPowerMode(void);
        
        virtual void fifoEmptied(void);

        virtual void portMuted(t_uint16 port_idx);

        virtual void lowPowerModeSetup(t_uint16 port_idx);

        virtual void digitalMute(t_uint16 mute);


    protected:
        //! allocates ping pong buffer used for DMA transfer
        //! \note No allocation is performed as we reuse buffer for "msp_dma_ctrl_ab8500"
        virtual OMX_ERRORTYPE allocateDoubleBuffer(OMX_U32 domainId);
        //! frees ping pong buffer used for DMA transfer
        //! \note No free is performed as we reuse buffer for "msp_dma_ctrl_ab8500"
        virtual OMX_ERRORTYPE freeDoubleBuffer(void);

    private:
        BindContext *         mBindContext; //!< Mpc to Host binding context

        Idmaout_low_power     mLowPower;        //!< Host to Mpc enable/disble low power interface
        t_cm_memory_handle    mRingBufferSDRAM; //!< Ring buffer handle in SDRAM, fallback if no ESRAM
        t_cm_memory_handle    mRingBufferESRAM; //!< Ring buffer handle, for ESRAM

        OMX_HANDLETYPE        mHdlToBeAcknowledged; //!< Handle to acknowledge after burst/normal mode transition

        //! allocates ring buffer used for DMA transfer in burst/transition mode
        OMX_ERRORTYPE         allocateRingBuffer(OMX_U32 domainId);
        //! frees ring buffer used for DMA transfer in burst/transition mode
        OMX_ERRORTYPE         freeRingBuffer(void);

        static NmfMpcOutAb8500 * mDmaOut;   //!< singleton: pointer on unique instance
        static int               mRefCount; //!< singleton: reference counter

        OMX_U32                  mDomainId;

        AudioRendererAb8500NmfMpc* clients[8];

        NmfMpcOutAb8500(OMX_U32 monoBlockSizeInSamples, OMX_U32 nbMspChannels);
        ~NmfMpcOutAb8500();
        
        //! NMF callbacks from Mpc to Host: switch to burst mode
        //! \param framecount indicates at which frame Ab8500 must switch to burt mode
        virtual void lowPowerMode(t_uint16 framecount);
        //! NMF callbacks from Mpc to Host: switch to normal mode
        //! \note This also acknowleges to RME that the transition to normal mode is done
        virtual void normalMode(void);
};

//-----------------------------------------------------------------------------
//! \class NmfMpcInAb8500
//! \brief Proxy of Nmf Mpc component "dmain"
//! 
//! This singleton is used for Ab8500 capturer
//! 
//-----------------------------------------------------------------------------
class NmfMpcInAb8500 : public NmfMpcBaseAb8500 {

    public:
        //! singleton: creates unique instance on first call
        //! \param monoBlockSizeInSamples size of one channel in samples
		//! \param nbMspChannels limits channels send to MSP by DMA
        static NmfMpcInAb8500 *  getHandle(OMX_U32 monoBlockSizeInSamples, OMX_U32 nbMspChannels);
        //! singleton: destroy unique instance on last call
        void                     freeHandle(void);

    protected:
        //! allocates ping pong buffer used for DMA transfer
        virtual OMX_ERRORTYPE allocateDoubleBuffer(OMX_U32 domainId);

    private:

        static NmfMpcInAb8500 *  mDmaIn;    //!< singleton: pointer on unique instance
        static int               mRefCount; //!< singleton: reference counter
        
        //! \param monoBlockSizeInSamples size of one channel in samples
		//! \param nbMspChannels limits channels send to MSP by DMA
        NmfMpcInAb8500(OMX_U32 monoBlockSizeInSamples, OMX_U32 nbMspChannels);
};

//-----------------------------------------------------------------------------
//! \class NmfMpcOutBT
//! \brief Proxy of Nmf Mpc component "dmaout_bt"
//! 
//! This singleton is used for BT renderer
//! 
//-----------------------------------------------------------------------------
class NmfMpcOutBT : public NmfMpcBaseBT {

    public:
        //! singleton: creates unique instance on first call
        //! \param monoBlockSizeInSamples size of one channel in samples
        static NmfMpcOutBT * getHandle(OMX_U32 monoBlockSizeInSamples);
        //! singleton: destroy unique instance on last call
        void freeHandle(void);
      
        //! instantiates/binds libraries used by Nmp Mpc component
        virtual OMX_ERRORTYPE instantiateLibraries(OMX_U32 domainId, OMX_U32 priority);
        //! destroys/unbinds libraries used by Nmp Mpc component
        virtual OMX_ERRORTYPE deInstantiateLibraries(void); 

    private:
        static NmfMpcOutBT * mDmaBTOut; //!< singleton: pointer on unique instance
        static int           mRefCount; //!< singleton: reference counter
       
        //! \param monoBlockSizeInSamples size of one channel in samples
        NmfMpcOutBT(OMX_U32 monoBlockSizeInSamples);

};

//-----------------------------------------------------------------------------
//! \class NmfMpcInBT
//! \brief Proxy of Nmf Mpc component "dmain_bt"
//! 
//! This singleton is used for BT capturer
//! 
//-----------------------------------------------------------------------------
class NmfMpcInBT : public NmfMpcBaseBT {

    public:
        //! singleton: creates unique instance on first call
        //! \param monoBlockSizeInSamples size of one channel in samples
         static NmfMpcInBT * getHandle(OMX_U32 monoBlockSizeInSamples);
        //! singleton: destroy unique instance on last call
         void                freeHandle(void);

    private:
        static NmfMpcInBT * mDmaBTIn;  //!< singleton: pointer on unique instance
        static int          mRefCount; //!< singleton: reference counter
        
        //! \param monoBlockSizeInSamples size of one channel in samples
        NmfMpcInBT(OMX_U32 monoBlockSizeInSamples);

};

#endif // _NMFMPCDMAOUT_H_
