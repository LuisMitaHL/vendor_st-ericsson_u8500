/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _PCMSPLITTER_NMFMPC_H_
#define _PCMSPLITTER_NMFMPC_H_

#include "AFMNmfMpc_ProcessingComp.h"

#include "PcmSplitter.h"

#include "audio_chipset_api_index.h"
#include "audio_codecs_chipset_api.h"

#include "host/pcm_splitter/nmfil/wrapper/configure.hpp"

/// Concrete class implementing a PcmSplitter processing component
class PcmSplitterNmfMpc: public AFMNmfMpc_ProcessingComp {
    public:

        PcmSplitterNmfMpc(ENS_Component &enscomp);

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

        virtual t_cm_instance_handle getNmfHandle(OMX_U32 portIdx) const;
        virtual const char * getNmfSuffix(OMX_U32 portIdx) const;
        virtual bool          supportsPcmProbe() const { return true; };
        virtual OMX_U32       getNmfPortIndex(OMX_U32 omxPortIdx) const { return omxPortIdx; };

    private:

        t_cm_instance_handle 	        mNmfVolctrl[NB_OUTPUT];
        t_cm_instance_handle 	        mNmfEffectsLib;

        //Host to Dsp NMF interfaces
        Ipcm_splitter_nmfil_wrapper_configure	mIconfigure;

        // port heaps
        void *                          mHeap[NB_OUTPUT_PORTS];
        int                             mHeapSize[NB_OUTPUT_PORTS];
        t_cm_memory_handle              mMemory[NB_OUTPUT_PORTS];

        static int srcHeapSizes[9][9];
        static int srcHeapSizeLowLatency;
        static const char * suffixItf[NB_OUTPUT+1];

        // private methods
        void            registerStubsAndSkels(void);
        void            unregisterStubsAndSkels(void);
        OMX_ERRORTYPE   instantiateLibraries(void);
        OMX_ERRORTYPE   deInstantiateLibraries(void);
        OMX_ERRORTYPE   instantiateOutputPort(int);
        OMX_ERRORTYPE   deInstantiateOutputPort(int);
        int             computeHeapSize(int idx);
        void            allocatePortHeap(int idx);
        void            freePortHeap(int idx);
        void            fillOutputPortParam(int idx, OutputPortParam_t  &outputportparam);
        void            fillInputPortParam(InputPortParam_t  &inputportparam);
        OMX_ERRORTYPE   retrieveVolumeRampConfig(OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE *pVolumeRamp);

};

#endif //_PCMSPLITTER_NMFMPC_H_
