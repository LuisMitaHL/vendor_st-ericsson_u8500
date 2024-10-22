/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


/* Include automaticaly built nmf file */
#include <resource_manager/sleep.nmf>
#include <inc/archi-wrapper.h>

#include <trace/mpc_trace.h>
#include <trace/mpc_trace.c>


/*------------------------------------------------------------------------
 * derived data types & defines 
 *----------------------------------------------------------------------*/
/** by NB_MAX_PENDING_RESOURCE queue length
*   right now max 8 codecs can be handled.
*   increase the number to increase codec count support
*/
#define NB_MAX_REQUESTS 8

/**
 * structure for book keeping.
 * which codec has requested prevent sleep. and which once
 * allowed back.
 */
typedef struct {
	Iresource_manager_api_inform_resource_status *cb;
	int      sleep_count;
} ts_vrm_sleep_desc;

/* state enum */
typedef enum {
    TE_ALLOW_SLEEP_STATE = -1,
    TE_PREVENT_SLEEP_STATE = 1
} te_sleep_state;

/*------------------------------------------------------------------------
 * Global Variables
 *----------------------------------------------------------------------*/

/* NMF strucure facilitates to register callback */
static t_sleepNotifyCallback sleepCb;

/** main book keeping array. .cb is NULL & .sleep_count is -1
 *  by deault. it means Sleep is allowed by default. */
static EXTMEM ts_vrm_sleep_desc g_vrm_sleep_desc[NB_MAX_REQUESTS];

/** this variable is incremented/decremented based on all to
 *  allowInternalSleep/preventInternalSleep. default value is
 *  -1.
*/
static EXTMEM int g_internal_sleep_status;

/* just for printing sleep enter and exit counts */
static EXTMEM t_uint24 sleep_count_enter, sleep_count_leave;

/** this varible maintains replica of effective NMF sleep status
 *  at NMF's end. note that when leaving sleep allowSleep() is
 *  called again to do so. (since currenlty NMF resets its own
 *  sleep count before existing sleep). we explicitly need to
 *  call allowSleep to retain the status equivalent to
 *  g_sleep_state_actual.
*/
static EXTMEM te_sleep_state g_sleep_state_actual;

/** this varible is used to optimize hwInit() call from
 *  allocator component */
static EXTMEM int slept;

/*------------------------------------------------------------------------
 * Private functions

 *----------------------------------------------------------------------*/
/*****************************************************************************/
/**
 * \brief  vrm_sleep_status 
 * Internal function (directly/indirectly accessed by
 * vrm_init(),
 * vrm_sleep_allow_prevent(),
 * vrm_internal_sleep_allow_prevent()
 * 
 * returns current sleep requirement. prevent or sleep. if any
 * of the .sleep_count is >= 0 or g_internal_sleep_status is >=0
 * TE_PREVENT_SLEEP_STATE is returned.
 */
/*****************************************************************************/
static te_sleep_state vrm_sleep_status()
{
	int i;
	if (g_internal_sleep_status >= 0) 
		return TE_PREVENT_SLEEP_STATE;

	for (i = 0; i<NB_MAX_REQUESTS; i++)
	{
		printf("\n sleep.c:vrm_sleep_status() g_vrm_sleep_desc[%d].sleep_count=%d", i, g_vrm_sleep_desc[i].sleep_count);
		if (g_vrm_sleep_desc[i].sleep_count >= 0)
		{
			return TE_PREVENT_SLEEP_STATE;
		}
	}

	return TE_ALLOW_SLEEP_STATE;
}

/*****************************************************************************/
/**
 * \brief  vrm_sleep 
 * Internal function (directly/indirectly accessed by
 * vrm_init(),
 * vrm_sleep_allow_prevent(),
 * vrm_internal_sleep_allow_prevent()
 * 
 * apply the required sleep status by calling
 * allowSleep()/preventSleep() of NMF
 */
/*****************************************************************************/
static void vrm_sleep()
{
	printf("\n vrm_sleep(): vrm_sleep_status()=%d g_sleep_state_actual=%d", vrm_sleep_status(), g_sleep_state_actual);
	if (vrm_sleep_status()==TE_ALLOW_SLEEP_STATE)
	{
		if (g_sleep_state_actual!=TE_ALLOW_SLEEP_STATE)
		{
			g_sleep_state_actual = TE_ALLOW_SLEEP_STATE;
			printf("\n vrm_sleep(): allowSleep()");
#ifndef __DISABLE_POWER_MANAGEMENT_SUPPORT
			allowSleep(); //as per Step 2 of NMF ULP support, 
#endif /* __DISABLE_POWER_MANAGEMENT_SUPPORT */
		}
	}
	else 
	{
		if (g_sleep_state_actual!=TE_PREVENT_SLEEP_STATE)
		{
			g_sleep_state_actual = TE_PREVENT_SLEEP_STATE;
			printf("\n vrm_sleep(): preventSleep()");
#ifndef __DISABLE_POWER_MANAGEMENT_SUPPORT
			preventSleep(); //as per Step 2 of NMF ULP support, 
#endif /* __DISABLE_POWER_MANAGEMENT_SUPPORT */
		}
	}
}

/*****************************************************************************/
/**
 * \brief  vrm_init 
 * Internal function (vfm lib interface)
 * 
 * initialization and activation of sleep mode.
 * [sleep mode is by default turned on]
 */
/*****************************************************************************/
static void vrm_init()
{
	int i;
	ENTER_CRITICAL_SECTION
	for (i = 0; i<NB_MAX_REQUESTS; i++)
        g_vrm_sleep_desc[i].sleep_count = -1;

	g_internal_sleep_status = -1;

	g_sleep_state_actual = vrm_sleep_status();
#ifndef __DISABLE_POWER_MANAGEMENT_SUPPORT
	if (g_sleep_state_actual == TE_ALLOW_SLEEP_STATE)
		allowSleep();
	else preventSleep();
#endif /* __DISABLE_POWER_MANAGEMENT_SUPPORT */
	EXIT_CRITICAL_SECTION
}


/*****************************************************************************/
/**
 * \brief vrm_internal_sleep_allow_prevent
 * Internal function (vfm lib interface)
 * 
 * to receive request of sleep/prevent and apply the result by
 * activating/de-activating sleep mode.
 */
/*****************************************************************************/
static void vrm_internal_sleep_allow_prevent(te_sleep_state status)
{
	ENTER_CRITICAL_SECTION
	g_internal_sleep_status += (int)status;
	vrm_sleep();
	EXIT_CRITICAL_SECTION
}

/*****************************************************************************/
/**
 * \brief vrm_sleep_allow_prevent
 * Internal function (vfm lib interface)
 * 
 * update sleep count table (main book) depending on codec's
 * requests. and apply effective result by
 * activating/de-activating sleep mode.
 */
/*****************************************************************************/
static void vrm_sleep_allow_prevent(te_sleep_state status, Iresource_manager_api_inform_resource_status *cb)
{
	int found=0;
	int i;
	ENTER_CRITICAL_SECTION
	printf("\n sleep.c:vrm_sleep_allow_prevent() status=%d cb=%d" , status, cb);
	for (i = 0; i<NB_MAX_REQUESTS; i++)
		if (g_vrm_sleep_desc[i].cb == cb && g_vrm_sleep_desc[i].sleep_count != -1)
		{
			found = 1;
			printf(" found");
			g_vrm_sleep_desc[i].sleep_count+=(int)status;
			break;
		}

	if (!found)
		for (i = 0; i<NB_MAX_REQUESTS; i++)
			if (g_vrm_sleep_desc[i].sleep_count == -1)
			{
                g_vrm_sleep_desc[i].cb = cb;
				g_vrm_sleep_desc[i].sleep_count+=(int)status;
				found = 1;
				printf("\n sleep.c:vrm_sleep_allow_prevent() slot filled=%d", i);
				break;
			}

	vrm_sleep();

	EXIT_CRITICAL_SECTION

	// More error handling and limits ?
	if (!found) // no slot found for cb
		Panic(INTERNAL_PANIC, 0x57EE6/*SLEEP*/);
}


/*------------------------------------------------------------------------
 * Methodes of component interface
 *----------------------------------------------------------------------*/

/*****************************************************************************/
/**
 * \brief  construct
 *
 * Init resource_manage / sleep part. Implicitly called while instanciating 
 *    this component. 
 *    Initialize all internal variables & registers sleep callbacks
 *    By default it enables "sleep" mode provided by NMF.
 */
/*****************************************************************************/
#pragma force_dcumode
t_nmf_error METH(construct)(void)
{
	vrm_init();
#ifndef __DISABLE_POWER_MANAGEMENT_SUPPORT
	sleepCb.interfaceCallback = sleepNotify;
	registerForSleep(&sleepCb);
#endif /* __DISABLE_POWER_MANAGEMENT_SUPPORT */
	return NMF_OK;
} /* end of construct() function */

/*****************************************************************************/
/**
 * \brief  destroy
 *
 * DeInit resource_manage / sleep part. Implicitly called while instanciating 
 *    this component. 
 *    De-registers sleep callbacks and prevents sleep mode. 
 */
/*****************************************************************************/
#pragma force_dcumode
void METH(destroy)(void)
{
#ifndef __DISABLE_POWER_MANAGEMENT_SUPPORT
	preventSleep();
	unregisterForSleep(&sleepCb);
#endif /* __DISABLE_POWER_MANAGEMENT_SUPPORT */
} /* end of destroy() function */ 


/*****************************************************************************/
/**
 * \brief  enter
 * 
 * implicitly called by NMF-EE before entering sleep 
 *
 * just performs book keeping eg maintaining sleep_count_enter 
 */
/*****************************************************************************/
#pragma force_dcumode
/* implement ee.api.sleepNotify.itf/enter */
void METH(enter)()
{
	ENTER_CRITICAL_SECTION
	sleep_count_enter++;
	EXIT_CRITICAL_SECTION
} /* end of enter() function */

/*****************************************************************************/
/**
 * \brief  leave
 * 
 * implicitly called by NMF-EE after leaving sleep state 
 *
 *  performs book keeping eg maintaining sleep_count_leave
 *  and to optimized re-init of hw. also used to keep the sleep count of NMF inline
 * 	with internal "g_sleep_state_actual".
 */
/*****************************************************************************/
#pragma force_dcumode
/* implement ee.api.sleepNotify.itf/leave */
void METH(leave)()
{
	ENTER_CRITICAL_SECTION
	sleep_count_leave++;
	slept = 1;
	EXIT_CRITICAL_SECTION
#ifndef __DISABLE_POWER_MANAGEMENT_SUPPORT
	allowSleep();
#endif /* __DISABLE_POWER_MANAGEMENT_SUPPORT */
} /* end of leave() function */


/*****************************************************************************/
/**
 * \brief  allowSleep
 * called by codec (ddep) when leaving EXE state
 * used to let allow RM entering sleep. usually called after preventSleep when 
 * OMX component leaving EXE state.
 */
/*****************************************************************************/
#pragma force_dcumode
void METH(allowSleep)(Iresource_manager_api_inform_resource_status *cb)
{
	printf("\n sleep.c:allowSleep(codec)%d/%d", sleep_count_enter, sleep_count_enter);
	vrm_sleep_allow_prevent(TE_ALLOW_SLEEP_STATE, cb);
} /* end of allowSleep() function */

/*****************************************************************************/
/**
 * \brief  preventSleep
 * called by codec (ddep) when entering EXE state 
 * used to reduce latency if any of the codec is in EXE state. codec may call this API. 
 * usually done when entering EXE state. if not called, mmdsp's sleep mode remains on
 * and for every last freeResource call dsp may fall into sleep state causing wakeup 
 * latency on next frame.
 */
/*****************************************************************************/
#pragma force_dcumode

void METH(preventSleep)(Iresource_manager_api_inform_resource_status *cb)
{
	printf("\n sleep.c:preventSleep(codec)%d/%d", sleep_count_enter, sleep_count_enter);
	vrm_sleep_allow_prevent(TE_PREVENT_SLEEP_STATE, cb);
} /* end of preventSleep() function */


/*****************************************************************************/
/**
 * \brief allowInternalSleep
 * Interface for internal use of RM. 
 * This interface is called by allocator sub-component internally by RM. 
 * Used to allow sleep if any resource is not allocated / requested
 */
/*****************************************************************************/
#pragma force_dcumode
void METH(allowInternalSleep)()
{
	printf("\n sleep.c:allowInternalSleep");
	vrm_internal_sleep_allow_prevent(TE_ALLOW_SLEEP_STATE);
} /* end of allowInternalSleep() function */

/*****************************************************************************/
/**
 * \brief  preventInternalSleep
 * Interface for internal use of RM.
 * This interface is called by allocator sub-component internally by RM. 
 * Used to prevent sleep if any resource is allocated (it is assumed that if 
 * ddep has allocated it, it is in use).
 */
/*****************************************************************************/
#pragma force_dcumode
void METH(preventInternalSleep)()
{
	printf("\n sleep.c:preventInternalSleep");
	vrm_internal_sleep_allow_prevent(TE_PREVENT_SLEEP_STATE);
} /* end of preventInternalSleep() function */


/*****************************************************************************/
/**
 * \brief  statusInternalSleep
 * Interface for internal use of RM.
 * This interface is called by allocator sub-component internally by RM. 
 * Used to optimize hw-re-init. in case sleep was not entered it does not 
 * reset hwInit flag FALSE.
 */
/*****************************************************************************/
#pragma force_dcumode
void METH(statusInternalSleep)()
{
	int tmp;
	printf("\n statusInternalSleep %d/%d", sleep_count_enter, sleep_count_enter);

	ENTER_CRITICAL_SECTION
	tmp = slept;
	slept = 0;
	EXIT_CRITICAL_SECTION

	// to optimize hwInit(), we don't need re-init if not gone to sleep
	if (tmp) iSleepStatus.goneToSleep();
} /* end of statusInternalSleep() function */

/*****************************************************************************/
/**
 * \brief  setDebug
 * Interface to enable capturing of RM printf for debugging 
 * (temporarily being used to know sleep functioning and status).
 * This interface can be called by ddep OR from host 
 * see "trace" component's README file for details
 */
/*****************************************************************************/
void METH(setDebug)(t_uint32 mode, t_uint32 param1, t_uint32 param2)
{

#ifdef __DEBUG_TRACE_ENABLE

    mpc_trace_init(param1, param2);
#endif //#ifdef __DEBUG_TRACE_ENABLE

	printf("\ndone init sleep");
} /* End of setDebug() fucntion. */

