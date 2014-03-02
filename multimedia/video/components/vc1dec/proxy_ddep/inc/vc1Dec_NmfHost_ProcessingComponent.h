/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef __vc1Dec_NmfHost_ProcessingComponent_H
#define __vc1Dec_NmfHost_ProcessingComponent_H

#include "VFM_NmfHost_ProcessingComponent.h"
#include "VFM_Component.h"
#include "VFM_Port.h"
#include "OMX_Core.h"
#include "OMX_Video.h"
#include "SharedBuffer.h"

#ifdef NO_HAMAC
#include "vc1dec/arm_nmf/proxynmf_sw.hpp"
#define VC1_COMPONENT vc1dec_arm_nmf_proxynmf_sw
#else
#include "vc1dec/arm_nmf/proxynmf.hpp"
#define VC1_COMPONENT vc1dec_arm_nmf_proxynmf
#endif

#include <frameinfo.idt>
#include <host_decoder.h>

class vc1Dec_GetConfigCB;

/// @ingroup vc1Decoder
/// @brief Class that groups all the attributes (NMF components and interfaces)
/// used by the vc1 Decoder, and instantiation / deinstantiation support
/// for these NMF components.
/// @note this class derives from VFM_NMF
class vc1Dec_NmfHost_ProcessingComponent : public VFM_NmfHost_ProcessingComponent
{
    friend class vc1Dec_GetConfigCB;
    
    public:
        vc1Dec_NmfHost_ProcessingComponent(ENS_Component &enscomp)
            : VFM_NmfHost_ProcessingComponent(enscomp), mGetConfigCB(0)
                { memset(&mCodecFrameInfo, 0, sizeof(mCodecFrameInfo)); }

        OMX_ERRORTYPE errorRecoveryDestroyAll(void); /* +ER 341788 CHANGE START FOR */
         
    protected:
        virtual OMX_ERRORTYPE codecInstantiate(void);
        virtual OMX_ERRORTYPE codecStart(void);
        virtual OMX_ERRORTYPE codecStop(void);
        virtual OMX_ERRORTYPE codecConfigure(void);
        virtual OMX_ERRORTYPE codecDeInstantiate(void);
        virtual void resetDecodeImmediateRelease() ;
        
        virtual void registerStubsAndSkels(void);
        virtual void unregisterStubsAndSkels(void);
        
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

    private:
        Ivc1dec_arm_nmf_api_set_config mISetConfig;
        Ivc1dec_arm_nmf_api_set_configuration mISetConfiguration;
        vc1Dec_GetConfigCB *mGetConfigCB;
        t_frameinfo mCodecFrameInfo;        // configuration known by the codec
};

#endif
