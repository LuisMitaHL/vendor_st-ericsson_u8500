/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#include <sleep/null.nmf>

/* implement ee.api.sleep.itf */
void allowSleep()
{
    //just do nothink
}

void preventSleep()
{
    //just do nothink
}

/* implement ee.api.sleepRegister.itf */
void registerForSleep(t_sleepNotifyCallback *pCallback)
{
    //just do nothink
}

void unregisterForSleep(t_sleepNotifyCallback *pCallback)
{
    //just do nothink
}

/* implement sleep.api.ulp.itf */
void METH(enter)()
{
    //just do nothink
}

/* need symbols for crt0 */
t_uint24 isSleeping = 0;
t_uint24 forceWakeup = 0;
void sleep_wakeup()
{
    //just do nothink
}
