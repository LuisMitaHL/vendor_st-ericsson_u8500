/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#ifndef ANDROID
#include <linux/capability.h>
#endif

#include <eeConfiguration.nmf>

#ifdef __STN_5500
#include "eeConfiguration_5500.h"
#elif defined __STN_8500
#include "eeConfiguration_8500.h"
#elif defined __STN_9540
#include "eeConfiguration_9540.h"
#elif defined __STN_8540
#include "eeConfiguration_9540.h"
#elif defined WORKSTATION
#include "eeConfiguration_x86.h"
#else
#error "HostEE configuration file for this platform is missing"
#endif

// globals
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static unsigned int usage_count = 0;

#ifndef ANDROID
extern int capget(cap_user_header_t hdrp, cap_user_data_t datap);
#endif

static int has_capabilities(t_uint32 mask)
{
	struct __user_cap_header_struct header;
	struct __user_cap_data_struct data;

	header.version = _LINUX_CAPABILITY_VERSION;
	header.pid = getpid();

	if (capget(&header, &data)) {
		NMF_LOG("Error: capget failed : %s\n", strerror(errno));
	}

	return data.effective & mask;
}

static void setschedparam(int priority)
{
	int err = 0;

	if (priority < 100) {
		if (has_capabilities(1 << CAP_SYS_NICE)) {
			/* Real-Time prio */
			struct sched_param sched_param;

			sched_param.sched_priority =
				sched_get_priority_max(SCHED_RR) /*99*/ - priority;
			err = sched_setscheduler(0, SCHED_RR, &sched_param);
		} else {
			NMF_LOG("HostEE: requesting Real Time priority %d in process not having "
				"SYS_NICE capabilities - exiting\n", priority);
			pthread_exit(NULL);
		}
	} else {
		err = setpriority(PRIO_PROCESS, 0, priority - 120);
	}
	if (err)
		NMF_LOG("HostEE: can not set scheduling param when starting thread  (error: %s (%d))\n", strerror(errno), errno);
}

static struct th_config *get_cfg_for_prio(int priority)
{
	unsigned int i;
	for (i = 0; i < MAX_SCHEDULER_PRIORITY; i++) {
		if (thread_cfg[i].nmf_prio == priority)
			return &thread_cfg[i];
	}
	return NULL;
}

#ifndef HOST_ONLY
static void distribution_service_stater(void *priority)
{
#ifdef PR_SET_NAME
	prctl(PR_SET_NAME, (unsigned long)"NMFEE-MPC-SRV", 0, 0, 0);
#endif
	if (has_capabilities(1 << CAP_SYS_NICE)) {
		setschedparam(SERVICE_TH_RT_PRIO);
	} else {
		setschedparam(SERVICE_TH_NON_RT_PRIO);
	}
	distributionServiceRoutine.routine(0);
}

static void distribution_stater(void *config)
{
	struct th_config *cfg = config;
#ifdef PR_SET_NAME
	char name[16];
	snprintf(name, 16, "NMFEE-MPCCALL-%d", cfg->nmf_prio);
	prctl(PR_SET_NAME, (unsigned long)name, 0, 0, 0);
#endif
	setschedparam(cfg->linux_prio);
	distributionStartRoutine.routine((void *)cfg->nmf_prio);
}
#endif /* HOST_ONLY */

static void sched_stater(void *cpu_prio)
{
	int priority = (int)cpu_prio & 0xFF;
	struct th_config *cfg = get_cfg_for_prio(priority);
#ifdef PR_SET_NAME
	char name[16];
	snprintf(name, 16, cfg->th_name, priority, ((int)cpu_prio)>>8);
	prctl(PR_SET_NAME, (unsigned long)name, 0, 0, 0);
#endif
	setschedparam(cfg->linux_prio);
	schedStartRoutine.routine(cpu_prio);
}

/* implement ee.api.initlib */
static t_nmf_error ee_init(void)
{
    int error, n_cpu = sysconf(_SC_NPROCESSORS_CONF); /* could use _SC_NPROCESSORS_ONLN instead */
    unsigned int i;
#ifndef HOST_ONLY
    pthread_attr_t attr;
    pthread_t distributionServiceThread;
#endif /* HOST_ONLY */

    for (i = 0; i < MAX_SCHEDULER_PRIORITY; i++) {
	    if (thread_cfg[i].n_worker_threads == NR_CPU)
		    thread_cfg[i].n_worker_threads = n_cpu;
    }

    /* init ee kernel */
    error = init.init();
    if (error != NMF_OK)
	    return error;

#ifndef HOST_ONLY
    error = pthread_attr_init(&attr);
    if (error) {
	    NMF_LOG("Error initializing thread attribute: %s (%d)\n", strerror(error), error);
	    return NMF_INTEGRATION_ERROR0;
    }
    /* create distribution thread */
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    error = pthread_create(&distributionServiceThread, &attr, (void*)distribution_service_stater, NULL);
    if (error) {
	    NMF_LOG("Error creating thread: %s (%d)\n", strerror(error), error);
	    return NMF_INTEGRATION_ERROR0;
    }

    pthread_attr_destroy(&attr);
#endif /* HOST_ONLY */

    return NMF_OK;
}

static void ee_destroy(void)
{
	init.destroy();
}

/* implement ee.api.out.itf */
EXPORT_SHARED t_nmf_error EE_CreateChannel(t_nmf_channel_flag flags, t_nmf_channel *channel)
{
    t_nmf_error res = NMF_OK;
    
    // init if not yet done
    pthread_mutex_lock(&mutex);
    if (usage_count == 0)
	    res = ee_init();
    if (res == NMF_OK)
	    usage_count++;
    pthread_mutex_unlock(&mutex);
    if (res != NMF_OK)
	    return res;
    
    // now try to create channel
    res = out.EE_CreateChannel(flags, channel);
    if (res != NMF_OK)
    {
	    pthread_mutex_lock(&mutex);
	    if (--usage_count == 0)
		    ee_destroy();
	    pthread_mutex_unlock(&mutex);
    }
    
    return res;
}

EXPORT_SHARED t_nmf_error EE_FlushChannel(t_nmf_channel channel, t_bool *isFlushMessageGenerated)
{
	return out.EE_FlushChannel(channel, isFlushMessageGenerated);
}

EXPORT_SHARED t_nmf_error EE_CloseChannel(t_nmf_channel channel)
{
    t_nmf_error res;
    
    pthread_mutex_lock(&mutex);
    if (usage_count == 0)
	    res = NMF_INVALID_PARAMETER;
    else {
	    res = out.EE_CloseChannel(channel);
	    if ((res == NMF_OK) && (--usage_count == 0))
		    ee_destroy();
    }
    pthread_mutex_unlock(&mutex);
    
    return res;
}

EXPORT_SHARED t_nmf_error EE_RegisterService(t_nmf_channel channel, t_nmf_serviceCallback handler, void *contextHandler)
{
    return out.EE_RegisterService(channel, handler, contextHandler);
}

EXPORT_SHARED t_nmf_error EE_UnregisterService(t_nmf_channel channel, t_nmf_serviceCallback handler, void *contextHandler)
{
    return out.EE_UnregisterService(channel, handler, contextHandler);
}

EXPORT_SHARED t_nmf_error EE_RegisterNotify(t_nmf_channel channel, t_nmf_notify notify, void *contextHandler)
{
    return out.EE_RegisterNotify(channel, notify, contextHandler);
}

EXPORT_SHARED t_nmf_error EE_GetMessage(t_nmf_channel channel, void **clientContext, char **message, t_bool blockable)
{
    return out.EE_GetMessage(channel, clientContext, message, blockable);
}

EXPORT_SHARED void EE_ExecuteMessage(void *itfref, char *message)
{
    out.EE_ExecuteMessage(itfref, message);
}

EXPORT_SHARED void EE_GetVersion(t_uint32 *version)
{
	out.EE_GetVersion(version);
}

EXPORT_SHARED t_nmf_error EE_SetMode(t_ee_cmd_id aCmdID, t_sint32 aParam)
{
	return out.EE_SetMode(aCmdID, aParam);
}

/* implement ee.api.priority.itf */
t_bool METH(isDistributionThread)(t_uint32 priority)
{
#ifndef HOST_ONLY
	struct th_config *cfg = get_cfg_for_prio(priority);
	if (cfg)
		return cfg->distribution_th;
#endif /* HOST_ONLY */
	return FALSE;
}

t_sint32 METH(notifyStart)(t_uint32 priority, t_uint32 channel)
{
    t_sint32 j;
    int error;
    pthread_attr_t attr;
    pthread_t th;
    struct th_config *cfg = get_cfg_for_prio(priority);

    /* avoid warning about unused param */
    (void) channel;

    if (cfg == NULL)
	    return 0;

    error = pthread_attr_init(&attr);
    if (error) {
	    NMF_LOG("Error initializing thread attribute: %s (%d)\n", strerror(error), error);
	    return 0;
    }
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

#ifndef HOST_ONLY
    /* create distribution thread */
    if (cfg->distribution_th) {
	    error = pthread_create(&th, &attr, (void*)distribution_stater, (void*)cfg);
	    if (error) {
		    NMF_LOG("HostEE: can not create REALTIME thread (error: %s (%d)); defaulting to non-realtime\n", strerror(error), error);
		    return 0;
	    }
    }
#endif /* HOST_ONLY */

    /* create worker threads */
    for(j=0; j<cfg->n_worker_threads; j++) {
	    error = pthread_create(&th, &attr, (void*)sched_stater, (void*)((j<<8)+priority));
	    if (error) {
		    NMF_LOG("Error creating thread: %s (%d)\n", strerror(error), error);
		    return -j;
	    }
    }

    pthread_attr_destroy(&attr);
    return j;
}

void METH(notifyStop)(t_uint32 priority, t_uint32 channel)
{
}
