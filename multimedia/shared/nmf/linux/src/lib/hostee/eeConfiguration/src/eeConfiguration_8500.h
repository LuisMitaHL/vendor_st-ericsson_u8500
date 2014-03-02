/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef EECONFIGURATION_H
#define EECONFIGURATION_H

/*
 * This file describes the HostEE configuration in terms of
 * - thread priority mapping between NMF Priority and Linux prio.
 * - number of sheduling threads per priority
 * - whether a distribution thread must be started for that priority
 *   (required if you plan to bind some DSP components to ARM
 *   components having this prio)
 *
 * This config. files is for 8500 family
 */

#define NR_CPU -1 /* used to set number of thread to match number of CPUs */

/* Name of the different threads started by NMF EE (must be less than 16 chars) */
#define NMFEE_NAME  "MMEE-%d-%d"
#define CSCALL_NAME "CSCALL-%d-%d"
#define IO_NAME     "MMIO-%d-%d"

/*
 * The prio is the internal kernel priority, which is the inverse
 * of the priority that is usually displayed by user-space tools.
 * - Zero represents the highest (Real-Time) priority (99).
 * - 99 is the lowest Real-Time priority
 * - Prio 100 starts the "nice" priorities with 100 being equal to nice -20
 * - and 139 being nice 19.
 * Threads with Real-Time prio are started with SCHED_RR policy
 */

struct th_config {
	int nmf_prio;            /* NMF priority for this entry */
	int linux_prio;          /* Linux priority of threads (see above) */
	int n_worker_threads;    /* Number of worker threads to start
				    if set to NR_CPU, it starts as many threads as CPU */
	char *th_name;           /* Worker thread name: must contain two %d as format */
	t_bool distribution_th;  /* Whether a distribution thread must be started
				    Must be set to true only if a binding from a dsp to
				    a HostEE component in that priority is used */
} thread_cfg[] = {
	{       /* NMF prio 0 : background */
		.nmf_prio       = 0,
		.linux_prio     = 102,
		.n_worker_threads = NR_CPU,
		.th_name  = NMFEE_NAME,
		.distribution_th = TRUE,
	},
	{       /* NMF prio 1 : normal */
		.nmf_prio       = 1,
		.linux_prio     = 101,
		.n_worker_threads = NR_CPU,
		.th_name  = NMFEE_NAME,
		.distribution_th = TRUE,
	},
	{       /* NMF prio 2 : urgent */
		.nmf_prio       = 2,
		.linux_prio     = 100,
		.n_worker_threads = NR_CPU,
		.th_name  = NMFEE_NAME,
		.distribution_th = TRUE,
	},
	{       /* NMF prio 3 : not used */
		.nmf_prio       = 3,
		.linux_prio     = 50,
		.n_worker_threads = 1,
		.th_name  = CSCALL_NAME,
		.distribution_th = TRUE,
	},
	{        /* NMF prio 4 : CScall */
		.nmf_prio       = 4,
		.linux_prio     = 49,
		.n_worker_threads = 1,
		.th_name  = CSCALL_NAME,
		.distribution_th = TRUE,
	},
	{        /* NMF prio 5 : CScall */
		.nmf_prio       = 5,
		.linux_prio     = 48,
		.n_worker_threads = 1,
		.th_name  = CSCALL_NAME,
		.distribution_th = TRUE,
	},
	{       /* NMF prio 6 : IO */
		/* DO NOT CHANGE */
		.nmf_prio       = 6,
		.linux_prio     = 120,
		.n_worker_threads = 4,
		.th_name  = IO_NAME,
		.distribution_th = FALSE,
	},
};

#define SERVICE_TH_RT_PRIO 40 /* (RT-) priority of the service thread */
#define SERVICE_TH_NON_RT_PRIO 100 /* (non RT-) priority of the service thread */

/*
 * The below define defines the number of NMF priority used.
 * It can and must be less or equal than MAX_SCHEDULER_PRIORITY_NUMBER
 */
#define MAX_SCHEDULER_PRIORITY (sizeof(thread_cfg)/sizeof(thread_cfg[0]))

#endif /* EECONFIGURATION_H */
