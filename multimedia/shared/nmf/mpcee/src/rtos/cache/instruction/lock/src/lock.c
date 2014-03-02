/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#include <rtos/cache/instruction/lock.nmf>
#include <inc/mmdsp_mapping.h>

#define NB_OF_LOCK_WAY                  2

/* define globals*/
static t_sleepNotifyCallback sleepCb;
static struct {
    t_uint24 addr;
    t_bool isLock;
} lockState[NB_OF_LOCK_WAY];

/* define private */
static void lock(t_uint24 addr, t_uint24 way);

/* implement lifecycle.constructor.itf */
t_nmf_error METH(construct)(void)
{
    /* configure number of lock way */
    WRITE24_INDIRECT_HOST_REG(IHOST_ICACHE_LOCK_V_REG, NB_OF_LOCK_WAY);
    MMDSP_FLUSH_ICACHE();

    /* lock first way for nmf from adress 0 */
    lock(0, 0);

    return NMF_OK;
}

/* implement lifecycle.starter.itf */
void METH(start)()
{
    sleepCb.interfaceCallback = sleepNotify;
    registerForSleep(&sleepCb);
}

/* implement rtos.api.cache.lock.itf */
void METH(lock)(t_uint24 mmdspAddr, t_uint24 way)
{
    lock(mmdspAddr, way);
}

void METH(unlock)(t_uint24 way)
{
    /* sanity check */
    if (way >= NB_OF_LOCK_WAY)
        return ;
    lockState[way].isLock = FALSE;
}

/* implement ee.api.sleepNotify.itf */
void METH(enter)()
{
    /* nothing to do */
}

void METH(leave)()
{
    t_uint24 i;

    /* configure number of lock way */
    WRITE24_INDIRECT_HOST_REG(IHOST_ICACHE_LOCK_V_REG, NB_OF_LOCK_WAY);
    MMDSP_FLUSH_ICACHE();

    /* relock needed way */
    for(i=0;i<NB_OF_LOCK_WAY;i++) {
        if (lockState[i].isLock)
            lock(lockState[i].addr, i);
    }
}

/* private methods */
static void lock(t_uint24 addr, t_uint24 way)
{
    t_uint24 icacheMode;

    /* sanity check */
     /* addr must be aligned on 256 */
    if ((addr & 0xff))
        return ;
    /* use way that is below max value */
    if (way >= NB_OF_LOCK_WAY)
        return ;
    /* program addr and way */
    WRITE24_INDIRECT_HOST_REG(IHOST_ICACHE_FILL_STARTWAY_REG, addr + (way << 20));
    READ24_INDIRECT_HOST_REG(IHOST_ICACHE_MODE_REG, icacheMode);
    icacheMode |= 0x8;
    WRITE24_INDIRECT_HOST_REG(IHOST_ICACHE_MODE_REG, icacheMode);
    lockState[way].isLock = TRUE;
    lockState[way].addr = addr;
}
