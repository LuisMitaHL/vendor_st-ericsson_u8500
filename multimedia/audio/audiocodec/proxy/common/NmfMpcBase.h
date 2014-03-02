/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   NmfMpcBase.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/

#ifndef  _NMFMPCBASE_H_
#define  _NMFMPCBASE_H_

#include "Audiocodec_macros.h"
#include "ENS_Nmf.h"
#include "NmfMpcDmaMsp.h"

// NMF interfaces
#include "host/common/configure.hpp"

//! defines number of microsecond in one millisecond
const unsigned int NB_US_IN_ONE_MS = 1000;
//! defines number of port for BT Nmf Mpc component
const unsigned int BT_DMA_NB_MNF_PORTS  = 1;

//-----------------------------------------------------------------------------
//! \class NmfMpcBase
//! \brief Base abstract class used by all Ab8500/BT In/Out Nmf Mpc proxy
//!
//-----------------------------------------------------------------------------
class NmfMpcBase : public NmfComponent {

    public:

		//! \param inOutMpcName name of Nmf Mpc component
		//! \param mspDmaCtrlItfName name of interface required to control MSP and DMA
		//! \param nbNmfPort specifies number of accessories.
        //! \note for "out" components do not count feedback port in nbNmfPort 
        NmfMpcBase(const char * inOutMpcName, const char * mspDmaCtrlItfName, unsigned int nbNmfPort);

        //! Pure virtual from NmfComponent: instantiate Nmf Mpc components
        virtual OMX_ERRORTYPE instantiate(OMX_U32 domainId, OMX_U32 priority);
        //! Pure virtual from NmfComponent: deInstantiate Nmf Mpc components
        virtual OMX_ERRORTYPE deInstantiate();
        //! Pure virtual from NmfComponent: start Nmf Mpc components
        virtual OMX_ERRORTYPE start();
        //! Pure virtual from NmfComponent: stop Nmf Mpc components
        virtual OMX_ERRORTYPE stop();
        
        //! configures Nmf Mpc components
        OMX_ERRORTYPE configure(OMX_U16 samplerate);

        //! returns time in us per port
        OMX_S64 getTimeInUs(unsigned int portIdx) ;

    protected:
        //! instantiates/binds libraries used by Nmp Mpc component
        virtual OMX_ERRORTYPE instantiateLibraries(OMX_U32 domainId, OMX_U32 priority);
        //! destroys/unbinds libraries used by Nmp Mpc component
        virtual OMX_ERRORTYPE deInstantiateLibraries(void);
        //! allocates ping pong buffer used for DMA transfer
        virtual OMX_ERRORTYPE allocateDoubleBuffer(OMX_U32 domainId) = 0;
        //! frees ping pong buffer used for DMA transfer
        virtual OMX_ERRORTYPE freeDoubleBuffer();
        
        //! PingPong buffer handle
        t_cm_memory_handle    mDoubleBuffer;         
        //! Pointer on singleton used to configure MSP/DMA
        NmfMpcDmaMspBase   *  mDmaMsp;
        //! contains information passed to Nmf Mpc conponent when configure is called
        DmaConfig_t           mMmdspConfig;
     

    private:
        OMX_U32               mInstantiateCount; //!< Singleton implementation variable for instanciation
        OMX_U32               mStartCount;       //!< Singleton implementation variable for startComponent
        OMX_U32               mStartDmaCount;    //!< Singleton implementation variable for start DMA transfer


        bool                  mIsConfigured;  //!< contains information is Nmf Mpc component is already configured
        Icommon_configure     mIconfigure;    //!< Nmf Mpc configuration interface

        ENS_String<32>        mInOutMpcName;           //<! dmain(_bt) or dmaout(_bt)
        ENS_String<32>        mMspDmaCtrInterfaceName; //<! mspDmaControlAb8500 or mspDmaControlBT

        unsigned int          mNbNmfPort;  //!< stores number of Nmf ports (do not count feedback port)
       
        t_cm_memory_handle    mSPHandle;   //!< shared memory between ARM and MMDSP handle
        SamplesPlayed_t *     mSParmAddr;  //!< logical address of shared memory 
};


//-----------------------------------------------------------------------------
//! \class NmfMpcBaseAb8500
//! \brief Base abstract class used by Ab8500 In/Out Nmf Mpc proxy
//!
//-----------------------------------------------------------------------------
class NmfMpcBaseAb8500 : public NmfMpcBase {

    public:
		//! \param inOutMpcName name of Nmf Mpc component
		//! \param nbNmfPort specifies number of accessories.
        //! \param monoBlockSizeInSamples size of one channel in samples
		//! \param nbMspChannels limits channels send to MSP by DMA
        NmfMpcBaseAb8500(const char * inOutMpcName, unsigned int nbNmfPort, OMX_U32 monoBlockSizeInSamples, OMX_U32 nbMspChannels);

    protected:
        
        virtual ~NmfMpcBaseAb8500() { mDmaMspAb8500->freeHandle(); }

        //! Pointer on singleton used to configure MSP1/DMA for Ab8500
        NmfMpcDmaMspAb8500 *  mDmaMspAb8500;
};


//-----------------------------------------------------------------------------
//! \class NmfMpcBaseBT
//! \brief Base abstract class used by BT In/Out Nmf Mpc proxy
//!
//-----------------------------------------------------------------------------
class NmfMpcBaseBT : public NmfMpcBase {

    public:
		//! \param inOutMpcName name of Nmf Mpc component
        //! \param monoBlockSizeInSamples size of one channel in samples
        NmfMpcBaseBT(const char * inOutMpcName, OMX_U32 monoBlockSizeInSamples);

    protected:

        virtual ~NmfMpcBaseBT() { mDmaMspBT->freeHandle(); } 

        //! allocates ping pong buffer used for DMA transfer
        virtual OMX_ERRORTYPE   allocateDoubleBuffer(OMX_U32 domainId);
       
        OMX_U32 mMonoBlockSizeInSamples;  //!< Mono grain in samples   

    private:
        //! Pointer on singleton used to configure MSP0/DMA for BT
        NmfMpcDmaMspBT *  mDmaMspBT;
};


#endif   // _NMFMPCBASE_H_

