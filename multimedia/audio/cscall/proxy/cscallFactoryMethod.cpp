/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     cscallFactoryMethod.cpp
 * \brief    factory method for CSCall proxy
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#include "CsCallComponent.h"
#include "CsCallNmfHost_ProcessingComp.h"
#include "cscall.h"
#include "cscall_config.h"

OMX_ERRORTYPE cscallFactoryMethod(ENS_Component_p * ppENSComponent) {
    OMX_ERRORTYPE error  = OMX_ErrorNone;

    CsCallComponent *cscall = new CsCallComponent;
    if (cscall == 0) {
        return OMX_ErrorInsufficientResources;
    }

    CsCallNmfHost_ProcessingComp *cscallProcessingCpt =
        new CsCallNmfHost_ProcessingComp ((ENS_Component&)*cscall);
    if (cscallProcessingCpt == 0) {
        return OMX_ErrorInsufficientResources;
    }

    error = cscall->construct();
#ifdef OST_TRACE_ALL_ON
	TraceBuilder * trace = cscall->getTraceBuilder();
	trace->setTraceZone(0xf,0xffff);
#endif

    if(error != OMX_ErrorNone){
        delete cscall;
        delete cscallProcessingCpt;
    }
    else{

        cscall->setProcessingComponent(cscallProcessingCpt);
        *ppENSComponent = cscall;
    }

    return OMX_ErrorNone;
}

