/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   dmaout_sleep.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
  #include "audio_audiocodec_mpc_dmaout_src_dmaout_sleepTraces.h"
#endif

#include "dmaout.h"

static int allow_sleep_active = 0;
static t_sleepNotifyCallback sleepCallBack;

void METH(start)(void) {
    sleepCallBack.interfaceCallback = sleepNotify;
    registerForSleep(&sleepCallBack);
}    

void METH(stop)(void) {
    unregisterForSleep(&sleepCallBack);
}

void METH(enter)(void){
#ifdef DEBUG_TRACE    
    OstTraceInt0(TRACE_DEBUG, "dmaout: enter sleep");
#endif    
    wake_up_save_configuration();
}

void METH(leave)(void){
    wake_up_reconfiguration();
    allow_sleep_active = 0;
}

void DmaOut_okToSleep(void)
{
    // If audiocodec has not called allowSleep() at all, or since it
    // woke up, call allowSleep. The problem is that the DSP sleep reference counter
    // is forced to 0 whenever the DSP wakes up. When it reaches -1, the DSP may enter
    // sleep if it has nothing to do.
    if (!allow_sleep_active) {
        allowSleep();
        allow_sleep_active = 1;
    }
}

void DmaOut_nokToSleep(void)
{
    if (allow_sleep_active) {
	preventSleep();
        allow_sleep_active = 0;
    }
}






