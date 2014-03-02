/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

#ifndef _OMXILOSALMUTEXIMPL_H
#define _OMXILOSALMUTEXIMPL_H

#include <omxilosalservices.h>
using namespace omxilosalservices;

#include <los/api/los_api.h>

class OmxILOsalMutexImpl : public OmxILOsalMutex
{
protected:

    friend OMX_ERRORTYPE OmxILOsalMutex::MutexCreate(OmxILOsalMutex* &aOmxILOsalMutex);
    friend void OmxILOsalMutex::MutexFree(OmxILOsalMutex* &aOmxILOsalMutex);
    OmxILOsalMutexImpl();
    virtual ~OmxILOsalMutexImpl();

    inline t_los_mutex_id getMutexHandle(){return iMutex;};

private:

    // Take the mutex
    virtual void MutexLock();

    // Release the mutex
    virtual void MutexUnlock();

    virtual OMX_ERRORTYPE MutexTryLock();

    t_los_mutex_id iMutex;
};

#endif // _OMXILOSALMUTEXIMPL_H
