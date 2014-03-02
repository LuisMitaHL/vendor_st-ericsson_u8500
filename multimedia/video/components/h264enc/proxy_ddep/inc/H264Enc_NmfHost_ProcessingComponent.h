/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __H264Enc_NmfHost_ProcessingComponent_H
#define __H264Enc_NmfHost_ProcessingComponent_H

#if ((!defined __SYMBIAN32__) && (!defined IMPORT_C))
    #define IMPORT_C
#endif
#if ((!defined __SYMBIAN32__) && (!defined EXPORT_C))
    #define EXPORT_C
#endif


#include "VFM_NmfHost_ProcessingComponent.h"
#include "VFM_Component.h"
#include "VFM_Port.h"
#include "OMX_Core.h"
#include "OMX_Video.h"
#include "SharedBuffer.h"

#ifdef NO_HAMAC
#include "h264enc/arm_nmf/proxynmf_stub.hpp"
#else
#include "h264enc/arm_nmf/proxynmf.hpp"
#include "h264enc/arm_nmf/proxynmf_hdtv.hpp"
#endif

#include <frameinfo.idt>
//#include <host_encoder.h>

class H264Enc_GetSpsPpsCB;

/// @ingroup H264Encoder
/// @brief Class that groups all the attributes (NMF components and interfaces)
/// used by the H264 Encoder, and instantiation / deinstantiation support
/// for these NMF components.
/// @note this class derives from VFM_NMF
class H264Enc_NmfHost_ProcessingComponent : public VFM_NmfHost_ProcessingComponent
{
    friend class H264Enc_GetSpsPpsCB;
    friend class H264Enc_Proxy;

    public:
        H264Enc_NmfHost_ProcessingComponent(ENS_Component &enscomp):
        VFM_NmfHost_ProcessingComponent(enscomp),
        mGetSpsPpsCB(0)
           { memset(&mCodecFrameInfo, 0, sizeof(mCodecFrameInfo));
           	 pProxyComponent = (H264Enc_Proxy *)(&mENSComponent);
           	 //+ code for CR 332873
           	 select1080FW = OMX_FALSE;
           	 //- code for CR 332873
           }

    virtual OMX_ERRORTYPE emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer);


    protected:
        virtual OMX_ERRORTYPE codecInstantiate(void);
        virtual OMX_ERRORTYPE codecStart(void);
        virtual OMX_ERRORTYPE codecStop(void);
        virtual OMX_ERRORTYPE codecConfigure(void);
        virtual OMX_ERRORTYPE codecDeInstantiate(void);
		virtual OMX_ERRORTYPE configure(void);

        virtual void registerStubsAndSkels(void);
        virtual void unregisterStubsAndSkels(void);

        virtual OMX_ERRORTYPE codecCreate(OMX_U32 domainId);
        virtual void codecDestroy(void);

        virtual void sendConfigToCodec();
        void getSpsPpsCB(t_sps_pps* pSequenceHeader);
        void codecRetrieveConfig(t_frameinfo *pFrameInfo);
        OMX_BOOL frameInfoDifferent(t_frameinfo *pFrameInfo1, t_frameinfo *pFrameInfo2);

        OMX_ERRORTYPE errorRecoveryDestroyAll(void);
        OMX_BOOL isNMFPanicSource(t_panic_source ee_type, OMX_U32 faultingComponent);

        virtual OMX_ERRORTYPE allocateBuffer(
            OMX_U32 nPortIndex,
            OMX_U32 nBufferIndex,
            OMX_U32 nSizeBytes,
            OMX_U8 **ppData,
            void **bufferAllocInfo,
            void **portPrivateInfo);

    private:
        Ih264enc_arm_nmf_api_set_config mISetConfig;
        Ih264enc_arm_nmf_api_set_param mISetParam;
        Ih264enc_arm_nmf_api_pendingCommandAck mIpendingCommandAck;
        H264Enc_GetSpsPpsCB *mGetSpsPpsCB;
        t_frameinfo mCodecFrameInfo;        // configuration known by the codec
        //+ code for CR 332873
        H264Enc_Proxy *pProxyComponent;
        OMX_BOOL select1080FW;
        //- code for CR 332873
};

#endif

