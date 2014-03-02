/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   BindContext.cpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "BindContext.h"

int           BindContext::mRefCount    = 0;
BindContext * BindContext::mBindContext = 0;

BindContext * BindContext::getHandle(void) {
    if (mRefCount++) {
        return mBindContext;
    }
    mBindContext = new BindContext();
    
    return mBindContext;
}

void BindContext::freeHandle() {
    if (--mRefCount) {
        return;
    }
    
    delete mBindContext;
    mBindContext = static_cast<BindContext*>(NULL);
}

BindContext::BindContext() : mEnsContext(NULL) {
    OMX_ERRORTYPE error = ENS::localCreateFullContext(mEnsContext);
    if(error != OMX_ErrorNone){
        delete mBindContext;
        mBindContext = static_cast<BindContext*>(NULL);
    }
}
   
BindContext::~BindContext() {
    if(mEnsContext){
        ENS::localDestroyFullContext(mEnsContext);
    }
}

