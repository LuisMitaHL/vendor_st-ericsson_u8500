/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   Port.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _PORT_H_
#define _PORT_H_

#include "queue.h"
#include "OmxEvent.h"

typedef enum {
    InputPort,
    OutputPort
} Direction;

// local type matching interface structure generated
// by nmf tools for emptythisbuffer/fillthisbuffer itfs
typedef struct {
    void *THIS;
    void (*deliverBuffer)(Buffer_p);
} Ideliverbuffer;

struct component;

typedef struct Port {
    FSM                   fsm;
    unsigned int          isBufferSupplier    : 1;
    unsigned int          direction           : 1;
    unsigned int          eosReceived         : 1;
    unsigned int          dequeuedBufferCount : 8;
    unsigned int          isHWport            : 1;
    struct Port *         sharingPort;
    Queue_t               bufferQueue;
    const Ideliverbuffer *nmfItf;
    unsigned int          portIdx;
    bool                  notifyStateTransition;
    OMX_STATETYPE         disabledState;
    OMX_STATETYPE         flushState;
    struct Component *    componentTHIS;
    bool                  isPortTunneled;
    bool                  isBufferResetReq;
    bool                  eventAppliedOnEachPorts;
} Port;

#endif // _PORT_H_
