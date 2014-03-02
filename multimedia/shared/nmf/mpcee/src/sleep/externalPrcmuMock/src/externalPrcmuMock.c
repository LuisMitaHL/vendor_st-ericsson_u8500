/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#include <sleep/externalPrcmuMock.nmf>

/* implement sleep.api.prcmu.itf */
/* When from EE we call asynchronous itf we need to do the this switch by hand */
#pragma noprefix
extern void *THIS;
void METH(sendMsg)(t_prcmu_msg_type msg)
{
    if (!IS_NULL_INTERFACE(prcmu, sendMsg)) {
        void *oldThis;
        
        oldThis = THIS;
        THIS = prcmu.THIS;
        prcmu.sendMsg(msg);
        THIS = oldThis;
    } else {
        Panic(INTERNAL_PANIC, 0);
    }
}

