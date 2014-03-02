/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

//define _GNU_SOURCE to pull sem_timedwait declaration on RedHat
#define _GNU_SOURCE
#include <os.nmf>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#ifndef WORKSTATION
#include <trace/stm.h>
#endif

/* implement ee.api.semaphore.itf */
hSem METH(create)(t_uint32 value)
{
    sem_t *pRes;

    pRes = (sem_t *) malloc(sizeof(sem_t));
    if (pRes == 0) {return (hSem) 0;}
    if (sem_init(pRes, 0, value) == -1)
    {
        free((void *)pRes);
        return (hSem) 0;
    }

    return (hSem)pRes;
}

void METH(P)(hSem sem)
{
    int err;

    do
    {
        err = sem_wait((sem_t *)sem);
    } while(err == -1 && errno == EINTR);
    NMF_ASSERT(err != -1);
}

t_sint32 METH(tryP)(hSem sem)
{
    int err;

    do
    {
        err = sem_trywait((sem_t *)sem);
    } while(err == -1 && errno == EINTR);

    return err;
}

t_sint32 METH(PTimeout)(hSem sem, t_uint32 timeOutInMs)
{
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec += timeOutInMs/1000;
	ts.tv_nsec += (timeOutInMs%1000) * 1e6;
	while (ts.tv_nsec >= 1e9) {
		ts.tv_nsec -= 1e9;
		ts.tv_sec++;
	}
retry:
	if (sem_timedwait((sem_t *)sem, &ts)) {
		if (errno == EINTR)
			goto retry;
		return -1;
	}
	return 0;
}

t_sint32 METH(V)(hSem sem)
{
    return sem_post((sem_t *)sem);
}

t_sint32 METH(destroy)(hSem sem)
{
    t_sint32 res = sem_close((sem_t *)sem);
    
    free((void *)sem);
    
    return res;
}

/* implement ee.api.mutex.itf */
EXPORT_SHARED hMutex eeMutexCreate()
{
    pthread_mutex_t *pRes;

    pRes = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    if (pRes == 0) {return (hMutex) 0;}
    if (pthread_mutex_init(pRes, NULL) != 0)
    {
        free((void *)pRes);
        return (hMutex) 0;
    }

    return (hMutex)pRes;
}

EXPORT_SHARED t_sint32 eeMutexDestroy(hMutex mutex)
{
    t_sint32 res = pthread_mutex_destroy((pthread_mutex_t *) mutex);
    
    if (res == 0) {free((void *) mutex);}
    
    return res;
}

EXPORT_SHARED void eeMutexLock(hMutex mutex)
{
    pthread_mutex_lock((pthread_mutex_t *) mutex);
}

EXPORT_SHARED t_sint32 eeMutexLockTry(hMutex mutex)
{
    return pthread_mutex_trylock((pthread_mutex_t *) mutex);
}

EXPORT_SHARED void eeMutexUnlock(hMutex mutex)
{
    pthread_mutex_unlock((pthread_mutex_t *) mutex);
}

/* implement ee.api.allocator.itf */
void* METH(alloc)(t_uint32 size)
{
    return malloc(size);
}

void METH(free)(void* addr)
{
    free(addr);
}

/* implement ee.api.osal.stm.itf */
#ifndef WORKSTATION
static struct stm_channel *stm_trace = NULL;
static size_t max_channels;

void METH(getChannelRegister)(t_uint32 channel, t_uint64 **STMTimestampRegister, t_uint64 **STMRegister)
{
	static t_uint64 dummy;
	if (stm_trace == NULL || channel > max_channels) {
		*STMTimestampRegister = &dummy;
		*STMRegister          = &dummy;
	} else {
		*STMTimestampRegister = &(stm_trace[channel].stamp64);
		*STMRegister          = &(stm_trace[channel].no_stamp64);
	}
}

/* implement starter interface */
void METH(start)()
{
	int fd;

	fd = open("/dev/" STM_DEV_NAME, O_RDWR);
	if (fd != -1) {
		ioctl(fd, STM_GET_NB_MAX_CHANNELS, &max_channels);

		stm_trace = (struct stm_channel *)mmap(0, max_channels*sizeof(*stm_trace), PROT_WRITE, MAP_SHARED, fd, 0);
		if (stm_trace == MAP_FAILED) {
			perror ("ARM-EE: STM trace - mmap failed");
			close (fd);
			stm_trace = NULL;
		}
	}
	if (stm_trace == NULL)
		fprintf(stderr, "ARM-EE: STM trace not supported\n");
}
#else
void METH(getChannelRegister)(t_uint32 channel, t_uint64 **STMTimestampRegister, t_uint64 **STMRegister)
{
	static t_uint64 dummy;
	*STMTimestampRegister = &dummy;
	*STMRegister          = &dummy;
}

/* implement starter interface */
void METH(start)()
{
}
#endif
