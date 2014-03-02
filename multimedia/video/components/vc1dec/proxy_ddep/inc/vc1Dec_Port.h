/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef __vc1DEC_PORT_H
#define __vc1DEC_PORT_H

#include "VFM_Port.h"
#include "host_types.h"


class vc1Dec_Port: public VFM_Port
{
    friend class vc1_Proxy;

    public:                        /* all the methods */
        /// Implementation of the OMX_EmptyThisBuffer/OMX_FillThisBuffer function.
        vc1Dec_Port(const EnsCommonPortData& commonPortData, ENS_Component &enscomp);
        virtual ~vc1Dec_Port() { };

        virtual OMX_ERRORTYPE checkSetFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE *portDef);
        virtual OMX_U32 getBufferSize() const;
        virtual OMX_U32 getBufferCountMin() const;
        void setDefault();
        virtual OMX_ERRORTYPE checkIndexParamVideoPortFormat(OMX_VIDEO_PARAM_PORTFORMATTYPE *pt);
        vc1Dec_Proxy *getProxy() const {return (vc1Dec_Proxy *)(&(getENSComponent())); } /* +ER 349396 CHANGE START FOR */
        #ifdef PACKET_VIDEO_SUPPORT
	        OMX_U32 mSuggestedBufferSize;
		#endif
};

#endif
