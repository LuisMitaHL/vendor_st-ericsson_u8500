/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _TRACE_H_
#define _TRACE_H_

#include <inc/type.h> /* provided by NMF */
#include <stdarg.h>

#ifdef STM_TRACE_IP
#include <xti.h>
#define SIA_TRACE_CHANNEL 110
#define TRACE_TAB_SIZE 3
#endif /*STM_TRACE_IP*/

#ifdef TRACE_NMFPRINTF
int	vsprintf( char *buf, const char *fmt, va_list ap );
#define MPC_MAX_STRING_SIZE	80
extern char ATTR(printf_private_buffer)[MPC_MAX_STRING_SIZE];
#include "itc_api.h"
#endif /* TRACE_NMFPRINTF */
/*------------------------------------------------------------------------
 * Types
 *----------------------------------------------------------------------*/
enum e_trace_id {
/*0x01*/IRP_TRACE_CLK_SWITCH = 0x01,
/*0x02*/IRP_TRACE_EVENT,
/*0x03*/IRP_TRACE_EVENT_QUEUED,
/*0x04*/IRP_TRACE_PE_WRITE,
/*0x05*/IRP_TRACE_PE_READ,
/*0x06*/IRP_TRACE_READ_COMPD,
/*0x07*/IRP_TRACE_INTERCEPTED_PE,
/*0x08*/IRP_TRACE_COMPLETED_GE,
/*0x09*/IRP_TRACE_EWARP_IT,
/*0x0A*/IRP_TRACE_HOST_IT,
/*0x0B*/IRP_TRACE_STATE_SWITCH,
/*0x0C*/IRP_TRACE_SUBSTATE_SWITCH,
/*0x0D*/IRP_TRACE_LOCK,
/*0x0E*/IRP_TRACE_ERROR,
/*0x0F*/IRP_TRACE_CLK_DETECTED,
/*0x10*/IRP_TRACE_CLK_LOST,

/*0x20*/ISPCTL_TRACE_INFO = 0x20,
/*0x21*/ISPCTL_TRACE_ERROR,
/*0x22*/ISPCTL_TRACE_DEBUG,

/*0x30*/GRB_TRACE_INFO = 0x30,
/*0x31*/GRB_TRACE_ERROR,
/*0x32*/GRB_TRACE_DEBUG,

/*0x40*/GRBCTL_TRC_EXECUTE = 0x40,
/*0x41*/GRBCTL_TRC_GRABCOMPLETED,
/*0x42*/GRBCTL_TRC_FILLTHISBUFFER,
/*0x43*/GRBCTL_TRC_MAX
};

/*------------------------------------------------------------------------
 * Variables (exported)
 *----------------------------------------------------------------------*/
/* code that includes this file must define a global called G_trace_enabled */
extern t_uint8 G_trace_enabled;

/*------------------------------------------------------------------------
 * Functions (inlined)
 *----------------------------------------------------------------------*/
#ifdef TRACE_NMFPRINTF
#pragma inline
static char *sprintf_private(char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsprintf(ATTR(printf_private_buffer), format, args);
    va_end(args);
    return ATTR(printf_private_buffer);
}
#endif /* NMF_PRINTF */

#pragma inline
static void TRACE(enum e_trace_id trace_id, t_uint32 data1, t_uint32 data2)
{
#ifdef STM_TRACE_IP
	t_uint16 trace_tab[TRACE_TAB_SIZE];
	int i;

	/* traces are costly on 8820 platform (~230 cycles per trace),
	   so it's cheaper to make traces conditional */
	if (G_trace_enabled) {
		/* initialize the trace to nonsense, in case I
		   someday increase the trace size but forget to
		   initialize the rest. Inefficient, but CYA. */
		for (i=0; i<TRACE_TAB_SIZE; i++) {
			trace_tab[i] = 0xFFFFU;
		}

		trace_tab[0] = (t_uint16)trace_id;
		trace_tab[1] = data1;
		trace_tab[2] = data2;
		xti_tab_ui(SIA_TRACE_CHANNEL, trace_tab, TRACE_TAB_SIZE);
	}
#endif /* STM_TRACE_IP */
#ifdef TRACE_NMFPRINTF
    {
        t_uint32 ts = ITC_GET_TIMER_32();
        NmfPrint0(EE_DEBUG_LEVEL_ERROR, sprintf_private("TS=%ld trace_id=0x%04X data1=0x%04X data2=0x%04X\n", ts, trace_id, data1, data2));
    }
#endif /* NMF_PRINTF */
}

#endif /* _TRACE_H_ */
