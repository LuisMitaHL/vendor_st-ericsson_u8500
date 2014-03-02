/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   MixerNmfHost.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _MIXER_NMFHOST_H_
#define _MIXER_NMFHOST_H_

#include "AFMNmfHost_ProcessingComp.h"
#include "Mixer.h"

#include "NmfHostVolCtrl.h"
#include "mixer/nmfil/host/wrapperWrapped.hpp"
#include "mixer/nmfil/host/wrapper/configure.hpp"
#include "volctrl/nmfil/host/effect/configure.hpp"
#include "volctrl/nmfil/host/effectWrapped.hpp"
#include "hst/pcmprocessings/wrapper/configure.hpp"


#define MIXER_CPTS_PRIO_LEVEL 2 // magic nb, don't know what to set

/// Concrete class implementing a Mixer processing component
class MixerNmfHost: public AFMNmfHost_ProcessingComp {
public:
MixerNmfHost(ENS_Component &enscomp) : AFMNmfHost_ProcessingComp(enscomp) {
#ifdef ENABLE_AVSYNC
        mNmfSyncLib = 0;
#endif

        mBlockSize = 0;
        mBitsPerSample = 0;
        for(int i=0;i<NB_INPUT;i++){    
            mNmfVolctrl[i] = NULL;
        }
        
    }

    virtual OMX_ERRORTYPE construct(void);
    virtual OMX_ERRORTYPE destroy(void);

    virtual	OMX_ERRORTYPE applyConfig(
        OMX_INDEXTYPE nConfigIndex,
        OMX_PTR pComponentConfigStructure);

    virtual OMX_ERRORTYPE retrieveConfig(
                OMX_INDEXTYPE nConfigIndex,
                OMX_PTR pComponentConfigStructure);
    virtual const char * getNmfSuffix(OMX_U32 portidx) const;

protected:
	virtual OMX_ERRORTYPE   doSpecificEventHandler_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2, OMX_BOOL &deferEventHandler);
    virtual OMX_ERRORTYPE   doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd);
	virtual OMX_ERRORTYPE   disablePortNotification(OMX_U32 nPortIndex);
    
	virtual OMX_ERRORTYPE   instantiateMain(void);
    virtual OMX_ERRORTYPE   startMain(void);
    virtual OMX_ERRORTYPE   stopMain(void);
    virtual OMX_ERRORTYPE   configureMain(void);
    virtual OMX_ERRORTYPE   deInstantiateMain(void);

private:
    //handle of the NMF component which provides the mixer interface
    volctrl_nmfil_host_effectWrapped    *mNmfVolctrl[NB_INPUT];

    //Host to HostEE NMF interfaces
    Imixer_nmfil_host_wrapper_configure	 mIconfigure;
    Ivolctrl_nmfil_host_effect_configure     mIconfigureVolCtrl;

    OMX_U32         mBlockSize;
    OMX_U32         mBitsPerSample;

    // private methods
    void            registerStubsAndSkels(void);
    void            unregisterStubsAndSkels(void);
    OMX_ERRORTYPE   instantiateLibraries(void);
    OMX_ERRORTYPE   deInstantiateLibraries(void);
    OMX_ERRORTYPE   instantiateInputPort(int);
    OMX_ERRORTYPE   deInstantiateInputPort(int);
    OMX_ERRORTYPE   initSamplesPlayed(void);
    void            fillInputPortParam(int idx, HostInputPortParam_t  &inputportparam);
    void            fillOutputPortParam(HostOutputPortParam_t  &outputportparam);


};

#endif //_MIXER_NMFMPC_H_
