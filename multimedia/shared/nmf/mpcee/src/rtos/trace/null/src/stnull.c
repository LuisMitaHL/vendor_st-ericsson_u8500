/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/* rtos/trace/null/src/stnull.c - EE.
 *
 * Copyright (c) 2006, 2007, 2008 STMicroelectronics.
 *
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by STMicroelectronics.
 *
 * Written by NMF team.
 */
#include "rtos/trace/null.nmf"

const int ATTR(myCoreId) = 0;

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

void /*METH(*/__mmdsp_profile_func_enter/*)*/(void* addr) {
}

void /*METH(*/__mmdsp_profile_func_exit/*)*/(void* addr) {
}

/* implement trace.api.trace.itf */
void /*METH(*/traceWrite16/*)*/(t_uint32 channel, t_uint16 value) {
}
void /*METH(*/traceWrite16T/*)*/(t_uint32 channel, t_uint16 value) {
}
void /*METH(*/traceWrite32/*)*/(t_uint32 channel, t_uint32 value) {
}
void /*METH(*/traceWrite32T/*)*/(t_uint32 channel, t_uint32 value) {
}

