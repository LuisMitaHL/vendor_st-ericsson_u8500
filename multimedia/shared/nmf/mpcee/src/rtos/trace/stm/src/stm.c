/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/* rtos/trace/stm/src/stm.c - EE.
 *
 * Copyright (c) 2006, 2007, 2008 STMicroelectronics.
 *
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by STMicroelectronics.
 *
 * Written by NMF team.
 */
#include "rtos/trace/stm.nmf"

#include "../inc/stm.h"
#include <inc/mmdsp_mapping.h>
#include <inc/archi-wrapper.h>
#include <inc/nmf-tracedescription.h>

int ATTR(myCoreId) = 1; // SVA=0, SIA=1
int ATTR(traceActive) = 0;

#define STM_DSP_SVA_OSMO_BASE                   0xF81000UL
#define STM_DSP_SIA_OSMO_BASE                   0xF81800UL

static __EXTMMIO t_stm_channel *pStmChannel[] = {(__EXTMMIO t_stm_channel *)STM_DSP_SVA_OSMO_BASE,
                                                 (__EXTMMIO t_stm_channel *)STM_DSP_SIA_OSMO_BASE};

static inline void write16(t_uword channel, t_uint16 value) {
    *((t_uint16 __EXTMMIO *)(&pStmChannel[ATTR(myCoreId)]->channel[channel].osmo[0])) = value;
}

static inline void write16T(t_uword channel, t_uint16 value) {
    *((t_uint16 __EXTMMIO *)(&pStmChannel[ATTR(myCoreId)]->channel[channel].osmot[0])) = value;
}

static inline void write32(t_uword channel, t_uint32 value) {
    pStmChannel[ATTR(myCoreId)]->channel[channel].osmo[0] = value;
}

static inline void write32T(t_uword channel, t_uint32 value) {
    pStmChannel[ATTR(myCoreId)]->channel[channel].osmot[0] = value;
}

#pragma noprefix
extern void *THIS;

#pragma force_dcumode
void /*METH(*/nmfTraceActivity/*)*/(t_nmfTraceActivityCommandDescription command, t_uint24 this, t_uint24 address)
{
    if(ATTR(traceActive))
    {
        /*
         * This function is called under scheduler control and thus is not called from interrupt context!!
         */
        ENTER_CRITICAL_SECTION;

        // header
        write32(MPC_EE_CHANNEL, (t_uint32)(TRACE_TYPE_ACTIVITY | 16 << 16)); // 16 == sizeof(struct t_nmfTraceActivity) in ARM world

        // command / domainId
        write32(MPC_EE_CHANNEL, (t_uint32)(((t_uint16)command) | ((t_uint16)ATTR(myCoreId) + 2) << 16));

        // methodId
        write32(MPC_EE_CHANNEL, (t_uint32)this);

        // componentContext
        write32T(MPC_EE_CHANNEL, (t_uint32)address);

        EXIT_CRITICAL_SECTION;
    }
}

#pragma force_dcumode
void /*METH(*/nmfTraceCommunication/*)*/(t_nmfTraceCommunicationCommandDescription command, t_uint24 remoteCoreId)
{
    if(ATTR(traceActive))
    {
        /*
         * This function is called under scheduler control and thus is not called from interrupt context!!
         */
        ENTER_CRITICAL_SECTION;

        // header
        write32(MPC_EE_CHANNEL, (t_uint32)(TRACE_TYPE_COMMUNICATION | 12 << 16)); // 12 == sizeof(struct t_nmfTraceCommunication) in ARM world

        // command
        write32(MPC_EE_CHANNEL, (t_uint32)command);

        // domainId / remoteDomainId
        write32T(MPC_EE_CHANNEL, (t_uint32)((t_uint16)ATTR(myCoreId) + 2) | (((t_uint16)remoteCoreId + 1) << 16));

        EXIT_CRITICAL_SECTION;
    }
}

#pragma force_dcumode
void /*METH(*/nmfTracePanic/*)*/(t_panic_reason reason, t_uint24 this, t_uint24 information1, t_uint24 information2)
{
    if(ATTR(traceActive))
    {
        /*
         * This function is called under scheduler control and thus is not called from interrupt context!!
         */
        ENTER_CRITICAL_SECTION;

        // header
        write32(MPC_EE_CHANNEL, (t_uint32)(TRACE_TYPE_PANIC | 20 << 16)); // 20 == sizeof(struct t_nmfTracePanic) in ARM world

        // reason / domainId
        write32(MPC_EE_CHANNEL, (t_uint32)(((t_uint16)reason) | ((t_uint16)ATTR(myCoreId) + 2) << 16));

        // componentContext
        write32(MPC_EE_CHANNEL, (t_uint32)this);

        // information1
        write32(MPC_EE_CHANNEL, (t_uint32)information1);

        // information2
        write32T(MPC_EE_CHANNEL, (t_uint32)information2);

        EXIT_CRITICAL_SECTION;
    }
}


#pragma force_dcumode
void /*METH(*/traceUser/*)*/(t_uint32 key)
{
    if(ATTR(traceActive))
    {
        /*
         * This function is called under interrupt or not
         */
        ENTER_CRITICAL_SECTION;

        // header
        write32(MPC_EE_CHANNEL, (t_uint32)(TRACE_TYPE_USER | 20 << 16)); // 20 == sizeof(struct t_nmfTracePanic) in ARM world

        // Key
        write32(MPC_EE_CHANNEL, key);

        // DomainId
        write32(MPC_EE_CHANNEL,  ((t_uint32)(ATTR(myCoreId) + 2) << 24));

        // componentContext
        write32(MPC_EE_CHANNEL, (t_uint32)*(t_uint24*)0x1);

        // CalllAddress
        write32T(MPC_EE_CHANNEL, 0x0); // __builtin_return_address(0)

        EXIT_CRITICAL_SECTION;
    }
}

#pragma force_dcumode
void /*METH(*/__mmdsp_profile_func_enter/*)*/(void* addr) {
    /*
     * This function is called under interrupt or not
     */
    ENTER_CRITICAL_SECTION;
    nmfTraceActivity(TRACE_ACTIVITY_CALL, (t_uint24)THIS, (t_uint24)addr);
    EXIT_CRITICAL_SECTION;
}

#pragma force_dcumode
void /*METH(*/__mmdsp_profile_func_exit/*)*/(void* addr) {
    /*
     * This function is called under interrupt or not
     */
    ENTER_CRITICAL_SECTION;
    nmfTraceActivity(TRACE_ACTIVITY_RETURN, (t_uint24)THIS, (t_uint24)addr);
    EXIT_CRITICAL_SECTION;
}

/* implement trace.api.trace.itf */
#pragma force_dcumode
void /*METH(*/traceWrite16/*)*/(t_uint32 channel, t_uint16 value) {
    write16((t_uword) channel, value);
}

#pragma force_dcumode
void /*METH(*/traceWrite16T/*)*/(t_uint32 channel, t_uint16 value) {
    write16T((t_uword) channel, value);
}

#pragma force_dcumode
void /*METH(*/traceWrite32/*)*/(t_uint32 channel, t_uint32 value) {
    write32((t_uword) channel, value);
}

#pragma force_dcumode
void /*METH(*/traceWrite32T/*)*/(t_uint32 channel, t_uint32 value) {
    write32T((t_uword) channel, value);
}

