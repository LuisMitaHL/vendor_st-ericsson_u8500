/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 * There are 3 implementations of timers here, all use the POSIX timer
 * - Default implementation uses POSIX timer with notification through SIGTIMER
 *   signal (SIGEV_SIGNAL): the handler (callback) is executed in a sighandler,
 *   which imposes some restriction in what is executed and can't be used in our
 *   case.
 * - Second implementation uses POSIX timer with notification through spawned
 *   threads (SIGEV_THREAD, under flag __USE_POSIX_SIGEV_THREAD). The implementation
 *   differs in glibc and bionic libc (android):
 *   . in glibc, an common intermediate, non RT, thread in used to catch a
 *     specific timer signal; the callback is then executed in a spawned thread,
 *     with the specified atributes
 *   . In bionic libc, a per-timer-thread is spawned, for each timer, with the
 *     specified attributes. The callback is executeded in this thread.
 *   => this option is used on Android, but can't be used with glibc due
 *      to the usage of this non-RT thread.
 * - Third implementation is a custom implementation of POSIX timer with
 *   notification through threads (under flag __USE_THREAD):
 *   a common threads, with high RT prio, is spawned and catches signals from the
 *   timers. It then executes the specific timer-callback of the expired timers.
 *   => this option is used on non-Android linux (glibc) but can't be used on Android
 *   due to lack of support of sigwaitinfo() function.
 */

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <linux/capability.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/prctl.h>
#include <timer.nmf>

#ifdef __USE_POSIX_SIGEV_THREAD
/* Implementation 1: use POSIX timer with notification through spawned threads
 * (SIGEV_THREAD POSIX Timer)
 */

#ifndef WORKSTATION
// Use this global attr and init it only once, to avoid to init it at
// each construct call.
static pthread_attr_t th_attr; 
static int init_attr_status = 0 ; // global shared status

static int has_capabilities(__u32 mask)
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

// Called once to init th_attr
// Status is reported through the shared init_attr_status
static void init_thread_attr(void) {
  struct sched_param sched_param;

  init_attr_status = pthread_attr_init(&th_attr);
  if (init_attr_status) {
    NMF_LOG("Error initializing attribute in Timer: %s (%d)\n", strerror(init_attr_status), init_attr_status);
    return;
  }
#ifndef ANDROID
  init_attr_status = pthread_attr_setinheritsched(&th_attr, PTHREAD_EXPLICIT_SCHED);
  if (init_attr_status) {
    NMF_LOG("Error setting sched-inherit policy in Timer: %s (%d)\n", strerror(init_attr_status), init_attr_status);
    return;
  }
#endif

  if (has_capabilities(1 << CAP_SYS_NICE)) {
    /* In case process has no permissions to create RT threads priority will be set
       once the thread is created to the highest non RT priority */
    init_attr_status = pthread_attr_setschedpolicy(&th_attr, SCHED_RR);
    if (init_attr_status) {
      NMF_LOG("Error setting sched policy in Timer: %s (%d)\n", strerror(init_attr_status), init_attr_status);
      return;
    }

    sched_param.sched_priority = sched_get_priority_max(SCHED_RR);
    init_attr_status = pthread_attr_setschedparam(&th_attr, &sched_param);
    if (init_attr_status) {
      NMF_LOG("Error setting sched param in Timer: %s (%d)\n", strerror(init_attr_status), init_attr_status);
      return;
    }
  }

  pthread_attr_setdetachstate(&th_attr, PTHREAD_CREATE_DETACHED);
}
#endif

static void timer_handler(sigval_t value) {
  timer *t;
  t = static_cast<timer*>(value.sival_ptr);
  if (t->isPrioritySet) {
    t->alarm.signal();
  } else {
#ifdef PR_SET_NAME
    prctl(PR_SET_NAME, (unsigned long)"timer", 0, 0, 0);
#endif
    if (has_capabilities(1 << CAP_SYS_NICE) == 0) {
      if (setpriority(PRIO_PROCESS, 0, -20)) {
        NMF_LOG("Error: setpriority -20 failed : %s\n", strerror(errno));
      }
    }
    t->isPrioritySet = true;
  }
}

t_nmf_error METH(construct)(void) {
  struct sigevent sigevt;
  struct itimerspec timer;

#ifndef WORKSTATION
  static pthread_once_t initattr_once = PTHREAD_ONCE_INIT;

  pthread_once(&initattr_once, init_thread_attr);
  if (init_attr_status)
    return NMF_INTEGRATION_ERROR0;

  sigevt.sigev_notify_attributes = &th_attr; /* Usefull if SIGEV_THREAD */
#else
  sigevt.sigev_notify_attributes = NULL; /* Usefull if SIGEV_THREAD */
#endif
  sigevt.sigev_notify = SIGEV_THREAD;
  sigevt.sigev_value.sival_ptr = this;
  sigevt.sigev_notify_function = &timer_handler; /* Usefull if SIGEV_THREAD */

  if (timer_create(CLOCK_MONOTONIC, &sigevt, &timerid)) {
    perror("timer_create() in Timer failed");
    return NMF_INTEGRATION_ERROR0;
  }

  // arms timer to expires right away to set the priority
  timer.it_value.tv_sec = 0;
  timer.it_value.tv_nsec = 1;
  timer.it_interval.tv_sec = 0;
  timer.it_interval.tv_nsec = 0;
  if (timer_settime(timerid, 0, &timer, NULL)) {
    perror("timer_settime() in Timer failed");
    return NMF_INTEGRATION_ERROR0;
  }

  return NMF_OK;
}

#else // __USE_POSIX_SIGEV_THREAD
#define SIGTIMER (SIGRTMIN+4)

#ifdef __USE_THREAD
/* Implementation 2: custom implementation of POSIX timer with notification in a
 * specific thread
 */

#include <semaphore.h>
#include <unistd.h>

static int init_status=0; // global shared status
static pid_t th_timer_tid;

#ifdef ANDROID
extern int __rt_sigtimedwait(const sigset_t *uthese, siginfo_t *uinfo, const struct timespec *uts, size_t sigsetsize);
#else
#include <sys/syscall.h>
static pid_t gettid(void) { return syscall(__NR_gettid); }
#endif

/* Helper function to support starting threads for SIGEV_THREAD.  */
static void *timer_thread_main (void *arg)
{
  /* Wait for the SIGTIMER signal */
  sem_t *sem = (sem_t *)arg;
#ifdef ANDROID
  unsigned long sigset[2];
  memset(&sigset, 0, sizeof(sigset));
#else
  sigset_t sigset;
  sigemptyset(&sigset);
#endif
  sigaddset((sigset_t*)&sigset, SIGTIMER);

  th_timer_tid = gettid();
  sem_post(sem);

  /* Endless loop for waiting for signals. The loop is only ended when
     the thread is canceled.  */
  while (1) {
    siginfo_t si;
    int result;

#ifdef ANDROID
    result = __rt_sigtimedwait((sigset_t*)&sigset, &si, NULL, sizeof(sigset));
#else
    result = sigwaitinfo(&sigset, &si);
#endif
    if (result > 0)
      {
	if (si.si_code == SI_TIMER) {
	  timer *t;
	  t = static_cast<timer*>(si.si_value.sival_ptr);
	  t->alarm.signal();
	}
      }
  }
  return NULL;
}

static void start_timer_thread(void) {
  pthread_attr_t attr;
  pthread_t th;
  sigset_t ss, oss;
#ifndef WORKSTATION
  struct sched_param sched_param;
#endif
  sem_t sem;

  init_status = sem_init(&sem, 0, 0);
  if (init_status) {
    perror("Error initializing semaphore in Timer");
    return;
  }

  init_status = pthread_attr_init(&attr);
  if (init_status) {
    NMF_LOG("Error initializing attribute in Timer: %s (%d)\n", strerror(init_status), init_status);
    return;
  }
#ifndef WORKSTATION
#ifndef ANDROID
  init_status = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
  if (init_status) {
    NMF_LOG("Error setting sched-inherit policy in Timer: %s (%d)\n", strerror(init_status), init_status);
    return;
  }
#endif
  init_status = pthread_attr_setschedpolicy(&attr, SCHED_RR);
  if (init_status) {
    NMF_LOG("Error setting sched policy in Timer: %s (%d)\n", strerror(init_status), init_status);
    return;
  }

  sched_param.sched_priority = sched_get_priority_max(SCHED_RR);
  init_status = pthread_attr_setschedparam(&attr, &sched_param);
  if (init_status) {
    NMF_LOG("Error setting sched param in Timer: %s (%d)\n", strerror(init_status), init_status);
    return;
  }
#endif

  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  sigfillset (&ss);
  sigprocmask(SIG_SETMASK, &ss, &oss);

  /* Create the helper thread for this timer.  */
  init_status = pthread_create (&th, &attr, timer_thread_main, &sem);
  if (init_status)
    NMF_LOG("Error creating Timer thread: %s (%d)\n", strerror(init_status), init_status);
  else
    sem_wait(&sem);

  sem_close(&sem);

  /* Restore the signal mask.  */
  sigprocmask(SIG_SETMASK, &oss, NULL);

  /* No need for the attribute anymore.  */
  (void) pthread_attr_destroy (&attr);
}

t_nmf_error METH(construct)(void) {
  struct sigevent sigevt;
  static pthread_once_t start_th_once = PTHREAD_ONCE_INIT;

  pthread_once(&start_th_once, start_timer_thread);
  if (init_status)
    return NMF_INTEGRATION_ERROR0;

  sigevt.sigev_notify = SIGEV_SIGNAL | SIGEV_THREAD_ID;
  sigevt.sigev_signo  = SIGTIMER;
  sigevt._sigev_un._tid = th_timer_tid;
  sigevt.sigev_value.sival_ptr = this;

  if (timer_create(CLOCK_MONOTONIC, &sigevt, &timerid)) {
    perror("timer_create() in Timer failed");
    return NMF_INTEGRATION_ERROR0;
  }

  return NMF_OK;
}

#else //__USE_THREAD
/* Implementation 3: use POSIX timer with notification through signals
 * (SIGEV_SIGNAL POSIX Timer)
 */

static int init_status=0; // global shared status

static void timer_handler(int signo, siginfo_t *info, void *context) {
  timer *t;

  // Ignore signal that are not triggered by a timer
  // Note: sanity check, may be useless
  if (info->si_code != SI_TIMER)
    return;

  t = static_cast<timer*>(info->si_value.sival_ptr);
  t->alarm.signal();
}

static void install_timer_hdl(void) {
  struct sigaction sigact;

  sigact.sa_sigaction = &timer_handler;
  sigemptyset(&sigact.sa_mask);
  sigact.sa_flags = SA_RESTART|SA_SIGINFO;

  if ((init_status=sigaction(SIGTIMER, &sigact, NULL)))
    perror("sigaction() call in Timer failed");
}

t_nmf_error METH(construct)(void) {
  struct sigevent sigevt;
  static pthread_once_t setsigtimer_once = PTHREAD_ONCE_INIT;

  pthread_once(&setsigtimer_once, install_timer_hdl);
  if (init_status)
    return NMF_INTEGRATION_ERROR0;

  sigevt.sigev_notify = SIGEV_SIGNAL; /* SIGEV_SIGNAL or SIGEV_THREAD */
  sigevt.sigev_signo  = SIGTIMER;
  //Specify either sival_ptr or sival_int within sigev_value
  sigevt.sigev_value.sival_ptr = this;

  if (timer_create(CLOCK_MONOTONIC, &sigevt, &timerid)) {
    perror("timer_create() in Timer failed");
    return NMF_INTEGRATION_ERROR0;
  }

  return NMF_OK;
}
#endif //__USE_THREAD 
#endif //__USE_POSIX_SIGEV_THREAD

/* Common part in all implementation */

void METH(destroy)(void) {
  if (timerid) {
    timer_delete(timerid);
    timerid = 0;
  }
}

/* implement timer.api.timer.itf */

/* Time are expressed in us */
t_uint32 METH(startTimer)(t_uint32 fisrtAlarm, t_uint32 period) {
  struct itimerspec timer;
  if (fisrtAlarm) {
    timer.it_value.tv_sec = fisrtAlarm/1000000;
    timer.it_value.tv_nsec = (fisrtAlarm%1000000)*1000;
  } else if (period) {
    timer.it_value.tv_sec =  period/1000000;
    timer.it_value.tv_nsec = (period%1000000)*1000;
  } else {
    /* If it_value is zero, the timer is disarmed; that's not what we want */
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_nsec = 1;
  }
  timer.it_interval.tv_sec = period/1000000;
  timer.it_interval.tv_nsec = (period%1000000)*1000;

  if (timer_settime(timerid, 0, &timer, NULL)) {
    perror("timer_settime() in Timer failed");
    return (t_uint32)NMF_INTEGRATION_ERROR0;
  }

  return 0;
}

/* Time are expressed in us */
t_uint32 METH(startHighPrecisionTimer)(t_uint32 fisrtAlarm, t_uint32 period) {
  return startTimer(fisrtAlarm, period);
}

void METH(stopTimer)(void) {
  struct itimerspec timer;
  timer.it_value.tv_sec = 0;
  timer.it_value.tv_nsec = 0;
  timer.it_interval.tv_sec = 0;
  timer.it_interval.tv_nsec = 0;

  timer_settime(timerid, 0, &timer, NULL);

  return ;
}

METH(timer)(void) {
  timerid = 0;
  isPrioritySet = false;
}

METH(~timer)(void) {
  destroy();
}
