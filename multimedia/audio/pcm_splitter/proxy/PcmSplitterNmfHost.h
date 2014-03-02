/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _PCMSPLITTER_NMFHOST_H_
#define _PCMSPLITTER_NMFHOST_H_

#include "AFMNmfHost_ProcessingComp.h"

#include "PcmSplitter.h"

#include "audio_chipset_api_index.h"
#include "audio_codecs_chipset_api.h"

#include "pcm_splitter/nmfil/host/wrapperWrapped.hpp"
#include "pcm_splitter/nmfil/host/wrapper/configure.hpp"
#include "volctrl/nmfil/host/effect/configure.hpp"
#include "volctrl/nmfil/host/effectWrapped.hpp"
#include "hst/pcmprocessings/wrapper/configure.hpp"


/// Concrete class implementing a PcmSplitter processing component
class PcmSplitterNmfHost: public AFMNmfHost_ProcessingComp {
    public:

        PcmSplitterNmfHost(ENS_Component &enscomp);

        virtual OMX_ERRORTYPE construct(void);
        virtual OMX_ERRORTYPE destroy(void);

        virtual	OMX_ERRORTYPE applyConfig(
                OMX_INDEXTYPE nConfigIndex,
                OMX_PTR pComponentConfigStructure);

    protected:
        virtual OMX_ERRORTYPE   doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd);
        virtual OMX_ERRORTYPE   doSpecificEventHandler_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2, OMX_BOOL & deferEventHandler);
        virtual OMX_ERRORTYPE   disablePortNotification(OMX_U32 nPortIndex);

        virtual OMX_ERRORTYPE   instantiateMain(void);
        virtual OMX_ERRORTYPE   startMain(void);
        virtual OMX_ERRORTYPE   stopMain(void);
        virtual OMX_ERRORTYPE   configureMain(void);
        virtual OMX_ERRORTYPE   deInstantiateMain(void);

        virtual const char * getNmfSuffix(OMX_U32 portIdx) const;

    private:

        //handle of the NMF component which provides the mixer interface
        volctrl_nmfil_host_effectWrapped    *mNmfVolctrl[NB_OUTPUT];

        //Host to HostEE NMF interfaces
        Ipcm_splitter_nmfil_host_wrapper_configure	 mIconfigure;
        Ivolctrl_nmfil_host_effect_configure     mIconfigureVolCtrl;

        // private methods
        OMX_ERRORTYPE   instantiateLibraries(void);
        OMX_ERRORTYPE   deInstantiateLibraries(void);
        OMX_ERRORTYPE   instantiateOutputPort(int);
        OMX_ERRORTYPE   deInstantiateOutputPort(int);
        void            fillOutputPortParam(int idx, HostOutputPortParam_t  &outputportparam);
        void            fillInputPortParam(HostInputPortParam_t  &inputportparam);
        OMX_ERRORTYPE   retrieveVolumeRampConfig(OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE *pVolumeRamp);

};

#endif //_PCMSPLITTER_NMFHOST_H_
