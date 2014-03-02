/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __H264ENC_PORT_H
#define __H264ENC_PORT_H

#include "VFM_Port.h"


class H264Enc_Port: public VFM_Port 
{
    public:                        /* all the methods */
        /// Implementation of the OMX_EmptyThisBuffer/OMX_FillThisBuffer function.
        H264Enc_Port(const EnsCommonPortData& commonPortData, ENS_Component &enscomp);
        virtual ~H264Enc_Port() { };
        
        virtual OMX_ERRORTYPE checkSetFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE *portDef);
        virtual OMX_U32 getBufferSize() const;
        virtual OMX_U32 getBufferCountMin() const;
        void setDefault();
        virtual OMX_ERRORTYPE checkIndexParamVideoPortFormat(OMX_VIDEO_PARAM_PORTFORMATTYPE *pt);

        /// @brief Set parameters to the component, from the application. It is part
        /// of OpenMaxIL API
        /// @param [in] nParamIndex index of the structure we want to set. Supported indexes are: \n
        /// - list to be done
        /// - All the ones supported by VFM_Component::setParameter()
        /// @param [in] pt the structure that contains the parameters we want to set
        /// @return OMX_ErrorNone if no error, an error otherwise
        /// @todo Adopt a generic way to handle error. This is a general comment
        virtual OMX_ERRORTYPE setParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt);

        /// @brief Get parameters from the component, to the application. It is part of OpenMaxIL API
        /// @param [in] nParamIndex index of the structure we want to get. Supported indexes are: \n
        /// - list to be done
        /// - All the ones supported by VFM_Component::getParameter()
        /// @param [out] pt the structure that will contains the parameters we want to get
        /// @return OMX_ErrorNone if no error, an error otherwise
        virtual OMX_ERRORTYPE getParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const;
        
    private:
        H264Enc_Proxy *getProxy() const { return (H264Enc_Proxy *)(&(getENSComponent())); }
};

#endif // __H264ENC_PORT_H
