/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   NmfMpcDmaMsp.h
 * \brief  MspDma (Ab8500 and BT) Nmf Mpc proxy classes
 * \author ST-Ericsson
 */
/*****************************************************************************/

#ifndef  _NMFMPCDMAMSPCTRL_H_
#define  _NMFMPCDMAMSPCTRL_H_

#include "BindContext.h"
#include "NmfComponent.h"

#include "host/msp_dma_ctrl_ab8500/configure.hpp"
#include "host/msp_dma_ctrl_ab8500/bitclock.hpp"

//! Total number of MSP channels from/to BT
const unsigned int BT_TOTAL_NB_CHANNELS      = 1;

//-----------------------------------------------------------------------------
//! \class NmfMpcDmaMspCtrl
//! \brief Proxy of Nmf Mpc component "msp_dma_controller"
//!
//! This singleton is used both by Ab8500 and BT drivers. Associated Mpc
//! component provides usefull function for MSP and DMA configuration.
//!
//-----------------------------------------------------------------------------
class NmfMpcDmaMspCtrl : public NmfComponent {

    public:
        NmfMpcDmaMspCtrl(): mInstantiateCount(0), mStartCount(0){
            mNmfHostReg = (t_cm_instance_handle)0;
            mOstTrace   = (t_cm_instance_handle)0;
        }

        static NmfMpcDmaMspCtrl * getHandle(void);   //!< singleton: creates unique instance on first call
        void                      freeHandle(void);  //!< singleton: destroy unique instance on last call

        //! Pure virtual from NmfComponent: instantiate Nmf Mpc components
        virtual OMX_ERRORTYPE instantiate(OMX_U32 domainId, OMX_U32 priority);
        //! Pure virtual from NmfComponent: deInstantiate Nmf Mpc components
        virtual OMX_ERRORTYPE deInstantiate();
        //! Pure virtual from NmfComponent: start Nmf Mpc components
        virtual OMX_ERRORTYPE start();
        //! Pure virtual from NmfComponent: stop Nmf Mpc components
        virtual OMX_ERRORTYPE stop();
        
        //! returns Host Register Nmf Mpc handle
        t_cm_instance_handle getNmfHostReg(void)  { return mNmfHostReg; }    
        //! returns OST traces singleton Nmf Mpc handle
        t_cm_instance_handle getNmfOstTrace(void) { return mOstTrace; }

    private:
        t_cm_instance_handle        mNmfHostReg;       //!< Host register Nmf Mpc handle. 
        t_cm_instance_handle        mOstTrace;         //!< OST trace Nmf Mpc handle
        
        OMX_U32                     mInstantiateCount; //!< singleton: instanciate count
        OMX_U32                     mStartCount;       //!< singleton: start count

        static NmfMpcDmaMspCtrl *   mDmaMspCtrl;       //!< singleton: pointer on unique instance
        static int                  mRefCount;         //!< singleton: reference counter
        
        virtual ~NmfMpcDmaMspCtrl() {}
};

//-----------------------------------------------------------------------------
//! \class NmfMpcDmaMspBase
//! \brief Base abstract class used by DmaMspCtrlAb8500 and DmaMspCtrlBT
//!
//-----------------------------------------------------------------------------
class NmfMpcDmaMspBase : public NmfComponent {

    public:

		//! \param nbMspChannels limits channels send to MSP by DMA
        NmfMpcDmaMspBase(OMX_U32 nbMspChannels) :
            mNbMspChannelEnabled(nbMspChannels),
            mInstantiateCount(0),
            mStartCount(0),
            mConfigured(false){ }

        //! Pure virtual from NmfComponent: instantiate Nmf Mpc components
        virtual OMX_ERRORTYPE instantiate(OMX_U32 domainId, OMX_U32 priority);
        //! Pure virtual from NmfComponent: deInstantiate Nmf Mpc components
        virtual OMX_ERRORTYPE deInstantiate();
        //! Pure virtual from NmfComponent: start Nmf Mpc components
        virtual OMX_ERRORTYPE start();
        //! Pure virtual from NmfComponent: stop Nmf Mpc components
        virtual OMX_ERRORTYPE stop();
        
        //! Configures Nmf Mpc components
        virtual OMX_ERRORTYPE configure(void) { mConfigured = true; return OMX_ErrorNone; }
        //! Derived class must implement NmfMain instantiation
        virtual OMX_ERRORTYPE instantiateMain(OMX_U32 domainId, OMX_U32 priority) = 0;
        //! Derived class must implement NmfMain destruction
        virtual OMX_ERRORTYPE deInstantiateMain(void) = 0;

        //! returns Number of enabled MSP channels
        inline OMX_U32 getNbMspEnabledChannels(void) { return mNbMspChannelEnabled; }

        //! returns Host Register Nmf Mpc handle
        inline t_cm_instance_handle getNmfHostReg(void)          { return mDmaMspCtrl->getNmfHostReg(); }
        //! returns OST traces singleton Nmf Mpc handle
        inline t_cm_instance_handle getNmfOstTrace(void)         { return mDmaMspCtrl->getNmfOstTrace(); }
        //! returns DmaCtrl singleton Nmf Mpc handle
        inline t_cm_instance_handle getNmfMspDmaController(void) { return (t_cm_instance_handle)mDmaMspCtrl->getNmfHandle(); }

    protected:
        OMX_U32                     mNbMspChannelEnabled;     //!< Number of Enabled Msp Channels  
        
        OMX_U32                     mInstantiateCount; //!< Singleton implementation variable for instanciation
        OMX_U32                     mStartCount;       //!< Singleton implementation variable for startComponent
        bool                        mConfigured;       //!< Singleton implementation variable for configureComponent

        //! Pointer on singleton shared between ab8500 and BT (Msp0, Msp1 and Dma usefull fonction)
        NmfMpcDmaMspCtrl *          mDmaMspCtrl;

};

//-----------------------------------------------------------------------------
//! \class NmfMpcDmaMspAb8500
//! \brief Proxy of Nmf Mpc component "msp_dma_ctrl_ab8500"
//! 
//! This singleton is used both by DmaInAb8500 and DmaOutAb8500 drivers. 
//! Associated Mpc component configures MSP1 and associated DMA logical channels.
//! 
//! \note As we need to maintain Frame counter Tx is always started (before Rx)
//! \note Buffer used to send zeros is twice big as needed. Because it is reused
//! as ping-pong buffer when "real" Tx tranfer is started.
//!
//-----------------------------------------------------------------------------
class NmfMpcDmaMspAb8500 : public NmfMpcDmaMspBase, public msp_dma_ctrl_ab8500_bitclockDescriptor {

    public:
        //! singleton: creates unique instance on first call
        //! \param monoBlockSizeInSamples size of one channel in samples
		//! \param nbMspChannels limits channels send to MSP by DMA
        static NmfMpcDmaMspAb8500 * getHandle(OMX_U32 monoBlockSizeInSamples, OMX_U32 nbMspChannels);
        void                        freeHandle(void);  //!< singleton: destroy unique instance on last call

        //! Configures Nmf Mpc components
        virtual OMX_ERRORTYPE configure(void);
        //! Instantiates NmfMain
        virtual OMX_ERRORTYPE instantiateMain(OMX_U32 domainId, OMX_U32 priority);
        //! Destroys NmfMain
        virtual OMX_ERRORTYPE deInstantiateMain();
        
        //! returns PingPong buffer address used for Tx
        OMX_U32 getTxDoubleBufferDspAddr(void);
        //! returns mono grain in samples
        inline OMX_U32 getMonoBlockSizeInSamples(void){ return mMonoBlockSizeInSamples; }    

    private:
        BindContext *      mBindContext;             //!< Mpc to Host binding context
        t_cm_memory_handle mDoubleBufferTx;          //!< PingPong buffer handle used for Tx
        OMX_U32            mMonoBlockSizeInSamples;  //!< Mono grain in samples   

        Imsp_dma_ctrl_ab8500_configure mIconfigure;  //!< Nmf Mpc configuration interface
        

        static NmfMpcDmaMspAb8500 * mDma;            //!< singleton: pointer on unique instance
        static int                  mRefCount;       //!< singleton: reference counter

        OMX_U32  getBufferSizeInSample(void);        //! returns buffer size used to transfer zeros at startup
        NmfMpcDmaMspAb8500(OMX_U32 monoBlockSizeInSamples, OMX_U32 nbMspChannels);

        virtual ~NmfMpcDmaMspAb8500();

        //! Nmf Mpc interface implementation from Mpc to ARM. This enables or disabled Ab8500 bitclock
        virtual void enable(t_bool on_off);
};


//-----------------------------------------------------------------------------
//! \class NmfMpcDmaMspBT
//! \brief Proxy of Nmf Mpc component "msp_dma_ctrl_bt"
//! 
//! This singleton is used both by DmaInBT and DmaOutBT drivers. 
//! Associated Mpc component configures MSP0 and associated DMA logical channels.
//! 
//-----------------------------------------------------------------------------
class NmfMpcDmaMspBT :  public NmfMpcDmaMspBase {

    public:
        static NmfMpcDmaMspBT * getHandle();      //!< singleton: creates unique instance on first call
        void                    freeHandle(void); //!< singleton: destroy unique instance on last call

        //! Instantiates NmfMain
        virtual OMX_ERRORTYPE instantiateMain(OMX_U32 domainId, OMX_U32 priority);
        //! Destroys NmfMain
        virtual OMX_ERRORTYPE deInstantiateMain();
        
    private:
        
        static NmfMpcDmaMspBT *     mDmaBT;       //!< singleton: pointer on unique instance
        static int               mRefCount;       //!< singleton: reference counter
       
        NmfMpcDmaMspBT();

        virtual ~NmfMpcDmaMspBT() {}
};


#endif   // _NMFMPCDMAMSPCTRL_H_

