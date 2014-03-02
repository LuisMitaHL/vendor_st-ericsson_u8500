/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _ISPCTL_EVENTCLBK_H_
#define _ISPCTL_EVENTCLBK_H_

#include "host/ispctl/api/cmd.hpp"
#include "client.h"

/** Maximum number of callbacks that can be registered */
#define ISPCTL_EVENTCLBK_MAXCOUNT 5

/** Return errors type */
typedef enum {
        ISPCTL_EVENTCLBK_ERR_NONE = 0,
        ISPCTL_EVENTCLBK_ERR_NOTFOUND,
        ISPCTL_EVENTCLBK_ERR_BUSY,
        ISPCTL_EVENTCLBK_ERR_OVERFLOW
} t_ispctlEventClbkErr;

/** Callback context handle passed back to the caller */
typedef void * t_ispctlEventClbkCtxtHnd;

/** Callback function pointer; to be registered from SIA clients */
typedef void (* t_ispctlEventClbk)(enum e_ispctlInfo infoID, t_ispctlEventClbkCtxtHnd ctxtHnd);

/** Internal storage for SIA clients hooks */
typedef struct {
        enum e_ispctlInfo infoID;
        t_sia_client_id clientID;
        t_ispctlEventClbk pClbk;
        t_ispctlEventClbkCtxtHnd ctxtHnd;
} t_ispctlEventHook;

#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CIspctlEventClbks);
#endif
class CIspctlEventClbks {
    public:
        CIspctlEventClbks();
        t_ispctlEventClbkErr setEventClbk(t_sia_client_id clientID, enum e_ispctlInfo infoID, t_ispctlEventClbk pClbk, t_ispctlEventClbkCtxtHnd ctxtHnd);
        t_ispctlEventClbkErr clearEventClbk(t_sia_client_id clientID, enum e_ispctlInfo infoID);
        t_ispctlEventClbkErr getEventClbk(t_sia_client_id clientID, enum e_ispctlInfo infoID, t_ispctlEventClbk *pClbk, t_ispctlEventClbkCtxtHnd *pCtxtHnd);
    private:
        t_ispctlEventHook mIspCtlEventHooks[ISPCTL_EVENTCLBK_MAXCOUNT];
};


#endif /* _ISPCTL_EVENTCLBK_H_ */
