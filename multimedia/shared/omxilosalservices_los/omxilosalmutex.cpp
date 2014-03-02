/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

#include "omxilosalmuteximpl.h"

OMX_ERRORTYPE OmxILOsalMutex::MutexCreate(OmxILOsalMutex* &aOmxILOsalMutex) {
    aOmxILOsalMutex = new OmxILOsalMutexImpl();

    if (((OmxILOsalMutexImpl*)aOmxILOsalMutex)->getMutexHandle() == 0) {
        return OMX_ErrorUndefined;
    }
    return OMX_ErrorNone;
}

void OmxILOsalMutex::MutexFree(OmxILOsalMutex* &aOmxILOsalMutex) {
    delete aOmxILOsalMutex ;
    aOmxILOsalMutex = 0;
}

OmxILOsalMutexImpl::OmxILOsalMutexImpl() {
    iMutex = LOS_MutexCreate();
}

OmxILOsalMutexImpl::~OmxILOsalMutexImpl() {
    LOS_MutexDestroy(iMutex);
}

void OmxILOsalMutexImpl::MutexLock() {
    LOS_MutexLock(iMutex);
}

void OmxILOsalMutexImpl::MutexUnlock() {
    LOS_MutexUnlock(iMutex);
}

OMX_ERRORTYPE OmxILOsalMutexImpl::MutexTryLock() {
    if (LOS_MutexTryLock(iMutex) == 0) {
        return OMX_ErrorNone;
    } else {
        return OMX_ErrorUndefined;
    }
}
