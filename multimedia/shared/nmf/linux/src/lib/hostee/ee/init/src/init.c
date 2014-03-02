/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#include <ee/init.nmf>

/* implement ee.api.init */
t_sint32 METH(init)()
{
    t_sint32 res;

    res = initCallBack.init();
    if (res) {goto errorCb;}
    res = initScheduler.init();
    if (res) {goto errorSched;}
    if (!IS_NULL_INTERFACE(initDistribution, init)) {
        res = initDistribution.init();
        if (res) {goto errorDist;}
    }

    nmfTraceInit();

    return 0;

errorDist:
    if (!IS_NULL_INTERFACE(initDistribution, destroy))
        initDistribution.destroy();
errorSched:
    initCallBack.destroy();
errorCb:
    return res;
}

void METH(destroy)()
{
    nmfTraceDone();

    if (!IS_NULL_INTERFACE(initDistribution, destroy))
            initDistribution.destroy();
    initScheduler.destroy();
    initCallBack.destroy();
}

