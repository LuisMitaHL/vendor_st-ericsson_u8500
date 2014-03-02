/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   CsCall_ControlPort.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _CSCALL_CONTROLPORT_H_
#define _CSCALL_CONTROLPORT_H_

#include "ENS_Port.h"
#if defined(__SYMBIAN32__) || defined(LINUX)
#include "cscall/nmf/host/composite/cscall.hpp"
#endif
#include "cscall/nmf/host/composite/cscallLoopULDLAlg.hpp"

class CsCall_ControlPort: public ENS_Port, public cscall_nmf_host_uplink_dropDataDescriptor {
    public:
        CsCall_ControlPort (OMX_U32 nIndex,
                            OMX_DIRTYPE eDir,
                            ENS_Component &enscomp);
        
        //~CsCall_ControlPort () {};

    protected:

        virtual OMX_ERRORTYPE setFormatInPortDefinition(
                const OMX_PARAM_PORTDEFINITIONTYPE& portDef);

        virtual OMX_ERRORTYPE checkCompatibility(
                OMX_HANDLETYPE hTunneledComponent,  
                OMX_U32 nTunneledPort) const;

        virtual void drop(t_uint32 us);

};

#endif

