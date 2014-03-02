/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __H264Dec_NmfHost_ProcessingComponent_H
#define __H264Dec_NmfHost_ProcessingComponent_H

#include "VFM_NmfHost_ProcessingComponent.h"
#include "VFM_Component.h"
#include "VFM_Port.h"
#include "OMX_Core.h"
#include "OMX_Video.h"
#include "SharedBuffer.h"

#ifdef NO_HAMAC
#include "h264dec/arm_nmf/proxynmf_sw.hpp"
#define H264_COMPONENT h264dec_arm_nmf_proxynmf_sw
#else
#include "h264dec/arm_nmf/proxynmf.hpp"
#include "h264dec/mpc/composite/h264decmpc.hpp"
#define H264_COMPONENT h264dec_arm_nmf_proxynmf
#endif

#include <frameinfo.idt>
#include <host_decoder.h>

class H264Dec_GetConfigCB;

/// @ingroup H264Decoder
/// @brief Class that groups all the attributes (NMF components and interfaces)
/// used by the H264 Decoder, and instantiation / deinstantiation support
/// for these NMF components.
/// @note this class derives from VFM_NMF
class H264Dec_NmfHost_ProcessingComponent : public VFM_NmfHost_ProcessingComponent
{
    friend class H264Dec_GetConfigCB;
    
    public:
        H264Dec_NmfHost_ProcessingComponent(ENS_Component &enscomp) 
            : VFM_NmfHost_ProcessingComponent(enscomp), mGetConfigCB(0), mErrorRecoveryHappened(OMX_FALSE)
                { memset(&mCodecFrameInfo, 0, sizeof(mCodecFrameInfo)); }

        OMX_ERRORTYPE errorRecoveryDestroyAll(void);
        OMX_ERRORTYPE errorRecoveryKillMpc(t_cm_domain_id & aMpcCodecDomainId);
        OMX_ERRORTYPE errorRecoveryRestartMpc(t_cm_domain_id aMpcCodecDomainId);
// +CR324558 CHANGE START FOR
		OMX_U32 getInternalDPBSize();
		void setInternalDPBSize(OMX_U32 dpbSize);
// -CR324558 CHANGE END OF
    protected:
        virtual OMX_ERRORTYPE codecInstantiate(void);
        virtual OMX_ERRORTYPE codecStart(void);
        virtual OMX_ERRORTYPE codecStop(void);
        virtual OMX_ERRORTYPE codecConfigure(void);
        virtual OMX_ERRORTYPE codecDeInstantiate(void);
        
        virtual void registerStubsAndSkels(void);
        virtual void unregisterStubsAndSkels(void);
        
		/* +Change for CR 399075 */
        virtual void resetDecodeImmediateRelease();
        /* -Change for CR 399075 */

        virtual OMX_ERRORTYPE codecCreate(OMX_U32 domainId);
        virtual void codecDestroy(void);
        
        virtual void sendConfigToCodec();
        void getConfigCB(t_frameinfo *pFrameInfo);
        void codecRetrieveConfig(t_frameinfo *pFrameInfo);
        OMX_BOOL frameInfoDifferent(t_frameinfo *pFrameInfo1, t_frameinfo *pFrameInfo2);

        virtual OMX_ERRORTYPE allocateBuffer(
            OMX_U32 nPortIndex,
            OMX_U32 nBufferIndex, 
            OMX_U32 nSizeBytes, 
            OMX_U8 **ppData, 
            void **bufferAllocInfo,
            void **portPrivateInfo);
        
        virtual OMX_ERRORTYPE emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer);
        virtual OMX_ERRORTYPE fillThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer);
        void h264dec_assert(int condition, int line, OMX_BOOL isFatal);
// +CR324558 CHANGE START FOR
        virtual OMX_ERRORTYPE doSpecificEventHandler_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2, OMX_BOOL &deferEventHandler);
// -CR324558 CHANGE END OF

    private:
        Ih264dec_arm_nmf_api_set_config mISetConfig;
        Ih264dec_arm_nmf_api_set_configuration mISetConfiguration;
        Ih264dec_arm_nmf_api_error_recovery mIErrorRecovery;
#ifdef NO_HAMAC
#else
#ifndef __ndk5500_a0__
        Ih264dec_mpc_api_trace_init mITraceInit;
#endif        
#endif
        H264Dec_GetConfigCB *mGetConfigCB;
        t_frameinfo mCodecFrameInfo;        // configuration known by the codec
        NMF::Composite * mMpcCodec;
        OMX_BOOL mErrorRecoveryHappened;
};

#endif
