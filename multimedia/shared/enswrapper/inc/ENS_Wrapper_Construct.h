/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _ENS_WRAPPER_CONSTRUCT_H_
#define _ENS_WRAPPER_CONSTRUCT_H_

#include "ENS_Queue.h"

extern "C" {
#include <los/api/los_api.h>
}

class ENS_WrapperConstruct {
private:
    // Semaphore to notify of a new message
    t_los_sem_id mSem;
    t_los_sem_id mExitSem;
    // The message queue
    ENS_Queue mQueue;

public:

    ENS_WrapperConstruct();
    ~ENS_WrapperConstruct();

    static void Thread(ENS_WrapperConstruct *);
    OMX_ERRORTYPE ENSProcessingPartAsync(OMX_HANDLETYPE hComponent);

};

#endif  /* #ifndef _ENS_WRAPPER_CONSTRUCT_H_ */
