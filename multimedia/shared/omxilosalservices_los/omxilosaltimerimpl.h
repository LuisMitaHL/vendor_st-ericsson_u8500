/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

#ifndef _OMXILOSALTIMERIMPL_H
#define _OMXILOSALTIMERIMPL_H

#include <omxilosalservices.h>
using namespace omxilosalservices;

#include <los/api/los_api.h>

class OmxILOsalTimerImpl : public OmxILOsalTimer
{
protected:

    friend OMX_U64 OmxILOsalTimer::GetSystemTime(void);

private:

};

#endif // _OMXILOSALTIMERIMPL_H
