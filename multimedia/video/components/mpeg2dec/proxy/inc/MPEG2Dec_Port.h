/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __MPEG2DEC_PORT_H
#define __MPEG2DEC_PORT_H

#include "VFM_Port.h"

class MPEG2Dec_Port: public VFM_Port
{
    friend class MPEG2_Proxy;

    public:                        /* all the methods */
        /// Implementation of the OMX_EmptyThisBuffer/OMX_FillThisBuffer function.
        MPEG2Dec_Port(const EnsCommonPortData& commonPortData, ENS_Component &enscomp);
        virtual ~MPEG2Dec_Port() { };

        virtual OMX_ERRORTYPE checkSetFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE *portDef);
        virtual OMX_U32 getBufferSize() const;
        virtual OMX_U32 getBufferCountMin() const;
        void setDefault();
        virtual OMX_ERRORTYPE checkIndexParamVideoPortFormat(OMX_VIDEO_PARAM_PORTFORMATTYPE *pt);
		OMX_ERRORTYPE setIndexParamVideoPortFormat(OMX_VIDEO_PARAM_PORTFORMATTYPE *portDef);
		OMX_ERRORTYPE getIndexParamVideoPortFormat(OMX_VIDEO_PARAM_PORTFORMATTYPE *portDef);
		void mpeg2dec_port_assert(OMX_ERRORTYPE omxError, OMX_U32 line, OMX_BOOL isFatal);
		virtual OMX_ERRORTYPE setFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE& portDef);
		virtual void getSlavedSizeFromGivenMasterSize(OMX_U32 nWidthMaster, OMX_U32 nHeightMaster, OMX_U32 *nWidthSlave, OMX_U32 *nHeightSlave) const;

        /// @brief Get parameters from the component, to the application. It is part of OpenMaxIL API
        /// @param [in] nParamIndex index of the structure we want to get. Supported indexes are: \n
        /// - list to be done
        /// - All the ones supported by VFM_Component::getParameter()
        /// @param [out] pt the structure that will contains the parameters we want to get
        /// @return OMX_ErrorNone if no error, an error otherwise
        virtual OMX_ERRORTYPE getParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const;

	private:
	#ifdef PACKET_VIDEO_SUPPORT
		    OMX_U32 mSuggestedBufferSize;
	#endif

        //MPEG2Dec_Proxy *getProxy() { return (MPEG2Dec_Proxy *)(&(getENSComponent())); }
};

#endif
