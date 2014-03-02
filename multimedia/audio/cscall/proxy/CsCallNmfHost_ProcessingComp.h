/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   CsCallNmfHost_ProcessingComp.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _CSCALLNMFHOST_PROCESSINGCOMP_H_
#define _CSCALLNMFHOST_PROCESSINGCOMP_H_

#include "AFMNmfHost_ProcessingComp.h"
#include "cscall_config.h"

// NMF Composite
#if defined(__SYMBIAN32__) || defined(LINUX)
#include "cscall/nmf/host/composite/cscall.hpp"
#endif
#include "cscall/nmf/host/composite/cscallLoopULDLAlg.hpp"
#include <timer/timerWrapped.hpp>


class CsCallNmfHost_ProcessingComp: public AFMNmfHost_ProcessingComp, public cscall_nmf_host_protocolhandler_network_updateDescriptor {

public:
        CsCallNmfHost_ProcessingComp(ENS_Component &enscomp);
        ~CsCallNmfHost_ProcessingComp();

        virtual OMX_ERRORTYPE construct();
        virtual OMX_ERRORTYPE destroy();
        void InitiateSpeechCodecReq();
        void InitiateSpeechTimingReq();
        void InitiateVoiceCallStatus();
        void InitiateMRePReq(AFM_AUDIO_CONFIG_CSCALL_MREP_REQ *pComponentConfigStructure);
        void resetTimingStruct() const;
        void InitiateModemLoopBackMode();

protected:

        virtual OMX_ERRORTYPE instantiateMain(void);
        virtual OMX_ERRORTYPE startMain(void);
        virtual OMX_ERRORTYPE configureMain(void);
        virtual OMX_ERRORTYPE stopMain(void);
        virtual OMX_ERRORTYPE deInstantiateMain(void);

        // inherited from the cscall_nmf_host_protocolhandler_network_updateDescriptor interface
        virtual void newCodecReq(CsCallCodecReq_t config);

        virtual OMX_ERRORTYPE applyConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure);

        virtual OMX_ERRORTYPE doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd);

        void registerStubsAndSkels(void);
        timerWrapped    *pUlTimer;
        timerWrapped    *pDlTimer;
        timerWrapped    *pTimerMReP;

        void unregisterStubsAndSkels(void);

private:
        OMX_ERRORTYPE instantiateCodec(void);

        Icscall_nmf_host_loopbackmodem_configure        mIconfigureLoopBack;
        Icscall_nmf_host_protocolhandler_configure      *mIconfigurePH;
        Icscall_nmf_host_uplink_configure               mIconfigureUplink;
        Icscall_nmf_host_downlink_configure             mIconfigureDownlink;

        CsCallCodec_t    mSpeechCodec;
        bool             mIsRunning; 
        CsCallComponent *cscallComp;

        OMX_BUFFERHEADERTYPE_p   omxULBuffers[NB_UL_MODEM_BUFFER];
        OMX_BUFFERHEADERTYPE_p   omxDLBuffers[NB_DL_MODEM_BUFFER];

        // allocate modem buffer
        OMX_BUFFERHEADERTYPE   uplinkBuffers[NB_UL_MODEM_BUFFER];
        OMX_BUFFERHEADERTYPE   downlinkBuffers[NB_DL_MODEM_BUFFER];
};

#endif // _CSCALLNMFHOST_PROCESSINGCOMP_H_
