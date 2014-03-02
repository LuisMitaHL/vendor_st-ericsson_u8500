/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/* rtos/trace/xti/src/xti.c - EE.
 *
 * Copyright (c) 2006, 2007, 2008 STMicroelectronics.
 *
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by STMicroelectronics.
 *
 * Written by NMF team.
 */
#include <rtos/trace/xti.nmf>

#include "../inc/xti.h"
#include <inc/mmdsp_mapping.h>
#include <inc/archi-wrapper.h>
#include <inc/nmf-tracedescription.h>

int ATTR(myCoreId) = 1;
int ATTR(traceActive) = 0;

#define XTI_DSP_OSMO_BASE ((0x30000>>1) + MMIO_DSP_BASE_ADDR)

static __EXTMMIO t_xti_osmo_space *pXtiSpace = (__EXTMMIO t_xti_osmo_space *)XTI_DSP_OSMO_BASE;

static inline void write32(t_uword channel, t_uint32 value) {
    pXtiSpace->osmo[channel].reg = value;
}

static inline void write32T(t_uword channel, t_uint32 value) {
    pXtiSpace->osmoT[channel].reg = value;
}

#pragma noprefix
extern void *THIS;

void /*METH(*/nmfTraceActivity/*)*/(t_nmfTraceActivityCommandDescription command, t_uint24 this, t_uint24 address)
{
}

void /*METH(*/nmfTraceCommunication/*)*/(t_nmfTraceCommunicationCommandDescription command, t_uint24 remoteCoreId)
{
}

void /*METH(*/nmfTracePanic/*)*/(t_panic_reason reason, t_uint24 this, t_uint24 information1, t_uint24 information2)
{
}

void /*METH(*/traceUser/*)*/(t_uint32 key)
{
}

void /*METH(*/__mmdsp_profile_func_enter/*)*/(void* addr)
{
}

void /*METH(*/__mmdsp_profile_func_exit/*)*/(void* addr)
{
}

/* implement trace.api.trace.itf */
/* use empty implementation since target deprecated */
void /*METH(*/traceWrite16/*)*/(t_uint32 channel, t_uint16 value) {
}
void /*METH(*/traceWrite16T/*)*/(t_uint32 channel, t_uint16 value) {
}
void /*METH(*/traceWrite32/*)*/(t_uint32 channel, t_uint32 value) {
}
void /*METH(*/traceWrite32T/*)*/(t_uint32 channel, t_uint32 value) {
}
