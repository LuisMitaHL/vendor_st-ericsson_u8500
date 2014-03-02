/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/* rtos/perfmeter/src/mpcload.c - EE.
 *
 * Copyright (c) 2006, 2007, 2008 STMicroelectronics.
 *
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by STMicroelectronics.
 *
 * Written by NMF team.
 */
/*
 *
 */
#include "rtos/perfmeter.nmf"
#include <rtos/perfmeter/inc/mpcload.h>
#include <inc/archi-wrapper.h>
#include <inc/mmdsp_mapping.h>

/*
 * Define declaration
 */
#define MPC_LOAD_IDLE_STATE                 0
#define MPC_LOAD_LOAD_STATE                 1

#define PRCMU_TIMER_4_DOWNCOUNT             ((0x57454>>1) + MMIO_DSP_BASE_ADDR)
#if (defined(__STN_8500) && (__STN_8500 >= 30)) || defined(__STN_9540)
    #define PRCMU_SH_VAR_4                  ((0xB8DE4>>1) + MMIO_DSP_BASE_ADDR)
#elif defined(__STN_8500) && (__STN_8500 <= 20)
    #define PRCMU_SH_VAR_4                  ((0x5FDE4>>1) + MMIO_DSP_BASE_ADDR)
#else
    #error "Unsupported platform"
#endif

/*
 * Attribute declaration
 */
//TODO : add correct pragma or attribute so that these variables are in sdram memory
volatile t_uint24 ATTR(perfmeterDataAddr) = 1;

/*
 * Private function declaration
 */
static void switchState(t_uint24);

/* define globals*/

/*
 * PUBLIC init interrupt and start timer
 */
t_nmf_error METH(construct)(void)
{
    /* just to init counters with correct value */
    switchState(MPC_LOAD_IDLE_STATE);
    switchState(MPC_LOAD_LOAD_STATE);

    return NMF_OK;
}

/* set to idle state and update counters */
void mpcload_SetIdleState()
{
    switchState(MPC_LOAD_IDLE_STATE);
}

/* set to load state and update counters */
void mpcload_SetLoadState()
{
    switchState(MPC_LOAD_LOAD_STATE);
}

/* get current timer value */
t_uint56 mpcload_GetCurrentTimerValue()
{
    t_uint32 msbBefore;
    t_uint32 lsb;
    t_uint32 msbAfter;
    t_uint56 res;

    /* read prcmu timers */
    msbBefore = ~(*((__EXTMMIO volatile t_uint32 *)PRCMU_SH_VAR_4));
    lsb = ~(*((__EXTMMIO volatile t_uint32 *)PRCMU_TIMER_4_DOWNCOUNT));
    msbAfter = ~(*((__EXTMMIO volatile t_uint32 *)PRCMU_SH_VAR_4));

    lsb = (lsb & 0xffffffff); // Not a 32 bit value but a 48 bits
    /* handle rollover test case */
    // NOTE : there is still a window in prcmu side between counter rollover and prcmu interrupt handling
    // to update msb register => this can lead to erroneous value return here
    if (msbBefore == msbAfter || lsb >= 0x80000000UL)
        res = ((msbBefore & 0xffffffUL) << 32) + lsb;
    else
        res = ((msbAfter & 0xffffffUL) << 32) + lsb;

    return res;
}

/* PRIVATE function */
/* switch current state */
static void switchState(t_uint24 newState)
{
#if defined(EXECUTIVE_ENGINE)
    volatile static t_uint24 state = MPC_LOAD_LOAD_STATE;
    volatile static t_uint24 counter = 0;
    volatile static t_uint56 totalLoadCounter = 0;
    volatile static t_uint56 lastEvent = 0;
    t_uint56 timerValue = mpcload_GetCurrentTimerValue();
    t_uint24 *pData = (t_uint24 *) ATTR(perfmeterDataAddr);

    /* update locals */
    if (state == MPC_LOAD_LOAD_STATE)
        totalLoadCounter += timerValue - lastEvent;
    state = newState;
    lastEvent = timerValue;

    /* export to attributes */
    counter = ((counter + 1) & 0xff);
    pData[0] = (counter << 16) + ((totalLoadCounter >> 32) & 0xffff);
    pData[1] = (counter << 16) + ((totalLoadCounter >> 16) & 0xffff);
    pData[2] = (counter << 16) + ((totalLoadCounter >> 0) & 0xffff);
    pData[3] = (counter << 16) + ((lastEvent >> 32) & 0xffff);
    pData[4] = (counter << 16) + ((lastEvent >> 16) & 0xffff);
    pData[5] = (counter << 16) + ((lastEvent >> 0) & 0xffff);
    pData[6] = (counter << 16) + state;
#endif
}
