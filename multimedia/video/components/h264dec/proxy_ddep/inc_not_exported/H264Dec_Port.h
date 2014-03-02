/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __H264DEC_PORT_H
#define __H264DEC_PORT_H

#include "VFM_Port.h"
#include "host_types.h"


class H264Dec_Port: public VFM_Port
{
    friend class H264_Proxy;
    
    public:                        /* all the methods */
        /// Implementation of the OMX_EmptyThisBuffer/OMX_FillThisBuffer function.
        H264Dec_Port(const EnsCommonPortData& commonPortData, ENS_Component &enscomp);
        virtual ~H264Dec_Port() { };
        
        virtual OMX_ERRORTYPE checkSetFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE *portDef);
        virtual OMX_ERRORTYPE setParameter(OMX_INDEXTYPE nParamIndex,OMX_PTR pComponentParameterStructure);
        virtual OMX_U32 getBufferSize() const;
        virtual OMX_U32 getBufferCountMin() const;
        void setDefault();
        virtual OMX_ERRORTYPE checkIndexParamVideoPortFormat(OMX_VIDEO_PARAM_PORTFORMATTYPE *pt);

        H264Dec_Proxy *getProxy() const { return (H264Dec_Proxy *)(&(getENSComponent())); }
        void h264dec_assert(int condition, int line, OMX_BOOL isFatal);
        static void h264dec_assert_static(int condition, int line, OMX_BOOL isFatal);
// +CR324558 CHANGE START FOR
	private:
		OMX_U8 mStaticDpbSize;
// -CR324558 CHANGE END OF
#ifdef PACKET_VIDEO_SUPPORT
                OMX_U32 mSuggestedBufferSize;
#endif
	OMX_BOOL isUpdatedOnce;
};

#endif
