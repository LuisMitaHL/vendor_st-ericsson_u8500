/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#ifndef __USE_XOPEN2K
#define __USE_XOPEN2K
#endif
#include <semaphore.h>
#include <stdarg.h>
#ifndef __USE_ISOC99
#define __USE_ISOC99
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#ifndef ANDROID
#include <linux/capability.h>
#endif

#include <los/api/los_api.h>
//#define LOG_NDEBUG 0
#define LOG_TAG "LOS"
#include "log-utils.h"

#define timesub(a, b, res)					\
	do {							\
		(res)->tv_sec  = (a)->tv_sec  - (b)->tv_sec;	\
		(res)->tv_nsec = (a)->tv_nsec - (b)->tv_nsec;	\
		if ((res)->tv_nsec < 0) {			\
			(res)->tv_nsec += 1000000000;		\
			(res)->tv_sec  -= 1;			\
		}						\
	} while (0)

EXPORT_SHARED void LOS_Log(const char *format, ...)
{
	va_list arglist;
	va_start(arglist, format);
#ifdef ANDROID
	LOG_PRI_VA(ANDROID_LOG_INFO, "", format, arglist);
#else
	vprintf(format, arglist);
#endif
	va_end(arglist);
}

EXPORT_SHARED void LOS_Log_Err(const char *format, ...)
{
	va_list arglist;
	va_start(arglist, format);
#ifdef ANDROID
	LOG_PRI_VA(ANDROID_LOG_ERROR, "", format, arglist);
#else
	vfprintf(stderr, format, arglist);
#endif
	va_end(arglist);
}

EXPORT_SHARED void LOS_Scanf(const char *format, ...)
{
	va_list arglist;
	va_start(arglist, format);
	vscanf(format, arglist);
	va_end(arglist);
}

EXPORT_SHARED t_los_process_id LOS_GetCurrentId(void)
{
	return (t_los_process_id)syscall(__NR_gettid);
}

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
		ALOGE("capget failed : %s\n", strerror(errno));
	}

	return data.effective & mask;
}

struct thread_starter_args {
	void* arg;
	const char* name;
	int priority;
	void (*start_routine) (void *);
};

static void * thread_starter(void* starter_args)
{
	struct thread_starter_args args = *((struct thread_starter_args *) starter_args);
	free(starter_args);

	if (args.priority != 0) {
		if (setpriority(PRIO_PROCESS, 0, args.priority)) {
			ALOGE("setpriority to %d failed - not starting thread !!!", args.priority);
			return NULL;
		}
	}
#ifdef PR_SET_NAME
	prctl(PR_SET_NAME, (unsigned long)args.name, 0, 0, 0);
#endif
	args.start_routine(args.arg);
	return NULL;
}

EXPORT_SHARED t_los_process_id LOS_ThreadCreate(void (*start_routine) (void *),
						void* arg,
						int stacksize,
						t_los_priority priority,
						const char* name)
{
	int err;
	pthread_t thread;
	pthread_attr_t attr;
	struct thread_starter_args* starter_args;

	starter_args = malloc(sizeof(struct thread_starter_args));
	if (starter_args == NULL) {
		ALOGE("%s:%d - malloc failed\n", __func__, __LINE__);
		return 0;
	}

	starter_args->priority = 0;

	if ((err = pthread_attr_init(&attr))) {
		ALOGE("%s - pthread_attr_init failed: error=%d\n", __func__, err);
		return 0;
	}
	if (((unsigned)stacksize >= PTHREAD_STACK_MIN)
	    && ((err = pthread_attr_setstacksize(&attr, stacksize)))) {
		ALOGE("%s - pthread_attr_setstacksize failed: error=%d\n", __func__, err);
		pthread_attr_destroy(&attr);
		return 0;
	}

#ifndef WORKSTATION
	if(priority == LOS_USER_URGENT_PRIORITY) {
		if (has_capabilities(1 << CAP_SYS_NICE)) {
			struct sched_param sched_param;
#ifndef ANDROID
			err = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
			if (err) {
				ALOGE("%s - pthread_attr_setinheritschedfailed: error=%d\n",
				     __func__, err);
				return 0;
			}
#endif
			err = pthread_attr_setschedpolicy(&attr, SCHED_RR);
			if (err) {
				ALOGE("%s - Error setting sched policy for thread: error=%d\n",
				     __func__, err);
				return 0;
			}

			sched_param.sched_priority = sched_get_priority_min(SCHED_RR);
			err = pthread_attr_setschedparam(&attr, &sched_param);
			if (err) {
				ALOGE("%s - Error setting sched policy for thread: error=%d\n",
				     __func__, err);
				return 0;
			}
		} else {
			starter_args->priority = -20;
		}
	}
#endif
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	starter_args->arg = arg;
	starter_args->start_routine = start_routine;
	starter_args->name = name;

	err = pthread_create(&thread, &attr, thread_starter, (void*) starter_args);
	pthread_attr_destroy(&attr);

	if (err) {
		ALOGE("%s - pthread_create failed: error=%d\n", __func__, err);
		return 0;
	}
	return (t_los_process_id)thread;
}

EXPORT_SHARED void LOS_ThreadExit(void)
{
	int s=0;
	pthread_exit(&s);
}

EXPORT_SHARED void LOS_Exit(void)
{
	exit(0);
}

EXPORT_SHARED void LOS_Yield(void)
{
	sched_yield() ;
}

EXPORT_SHARED void LOS_Sleep(t_uint32 time)
{
	/* time is in ms */
	unsigned long long endtime;
	time=time*1000; /* convert in us */
	endtime = LOS_getSystemTime()+time;
	while (usleep(time) && (errno==EINTR)) {
		unsigned long long cur=LOS_getSystemTime();
		if (cur > endtime)
			break;
		time = (endtime-cur);
	}
}

EXPORT_SHARED t_los_memory_handle LOS_Alloc(unsigned int sizeInBytes, unsigned int alignmentInBytes, t_los_mapping_mode mode)
{
	(void) alignmentInBytes; /* avoid warning about unused parameter */
	(void) mode; /* avoid warning about unused parameter */
	return (t_los_memory_handle)malloc(sizeInBytes);
}


EXPORT_SHARED void LOS_Free(t_los_memory_handle handle)
{
	free((void *)handle);
}

EXPORT_SHARED t_los_logical_address LOS_GetLogicalAddress(t_los_memory_handle handle)
{
    return (t_los_logical_address) handle;
}

EXPORT_SHARED t_los_logical_address LOS_Remap(t_los_physical_address address, t_los_size size,
				t_los_mapping_mode mode)
{
	static int fd=-1;
	unsigned int offset;
	void *addr;
	(void) mode; /* avoid warning about unused parameter */

	if (fd < 0) {
		fd = open("/dev/mem", O_RDWR);
		if (fd < 0) {
			ALOGE("%s(%d) - open failed: %s(%d)\n", __func__, __LINE__, strerror(errno), errno);
			return 0xffffffff;
		}
	}

	/* The address must be page-aligned */
	/* => align the given address to a page boundary and fix the size accordingly */
	offset = address % getpagesize(); // this computes the offset of the given address within a page-boundary
	address -= offset; // set the address to the closest page boundary
	size += offset; //fix the size
	addr = mmap(0, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, address);
	if (addr == MAP_FAILED) {
		ALOGE("%s(%d) - mmap failed: %s(%d)\n", __func__, __LINE__, strerror(errno), errno);
		return 0xffffffff;
	}

	return (t_los_logical_address) addr;
}

EXPORT_SHARED t_uint32 LOS_GetAllocatedChunk(void)
{
	//ALOGE("%s: not supported\n", __func__);
	return 0;
}

EXPORT_SHARED void LOS_DumpAllocatedChunk(void)
{
	//ALOGE("%s: not supported\n", __func__);
}

struct los_mutex_t {
	/* Pthread lock */
	pthread_mutex_t mutex;
	/* Thread ID holding lock */
	volatile t_los_process_id tid;
};

EXPORT_SHARED t_los_mutex_id LOS_MutexCreate(void)
{
	struct los_mutex_t *los_mutex = malloc(sizeof(struct los_mutex_t));

	if (!los_mutex)
		return 0;

	los_mutex->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	los_mutex->tid = 0;

	ALOGV("LOS_MutexCreate: 0x%08x\n", (unsigned int)los_mutex);

	return (t_los_mutex_id)los_mutex;
}

EXPORT_SHARED int LOS_MutexDestroy(t_los_mutex_id mutex)
{
	struct los_mutex_t *los_mutex = (struct los_mutex_t *)mutex;

	ALOGV("LOS_MutexDestroy: 0x%08x\n", mutex);

	if (los_mutex->tid != 0) {
		ALOGE("LOS_MutexDestroy: called while lock held by tid=%d\n", los_mutex->tid);
	}
	free(los_mutex);

	return 0;
}

#ifndef ANDROID

static int pthread_mutex_lock_timeout_np(pthread_mutex_t *mutex, unsigned msecs)
{
    int error=0;
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_nsec += (msecs%1000) * 1000000;
    ts.tv_sec += msecs/1000 + ts.tv_nsec/1000000000;
    ts.tv_nsec %= 1000000000;

    /* To check against false timeout we'll use CLOCK_MONOTONIC */
    struct timespec abs_timeout;
    clock_gettime(CLOCK_MONOTONIC, &abs_timeout);
    abs_timeout.tv_nsec += (msecs%1000) * 1000000;
    abs_timeout.tv_sec += msecs/1000 + abs_timeout.tv_nsec/1000000000;
    abs_timeout.tv_nsec %= 1000000000;
retry:
    error = pthread_mutex_timedlock(mutex, &ts);
    if (error) {
        if (error == ETIMEDOUT) {
            struct timespec mts,rts;
            clock_gettime(CLOCK_MONOTONIC, &mts);
            timesub(&abs_timeout, &mts, &rts);
            if (rts.tv_sec < 0 || rts.tv_nsec < 0) {
                /* Real timeout */
                return error;
            } else {
                /* Spurious timeout due to time change
                recompute absolute timeout upon CLOCK_REALTIME */
                   clock_gettime(CLOCK_REALTIME, &ts);
                   ts.tv_nsec += rts.tv_nsec;
                   ts.tv_sec  += rts.tv_sec + ts.tv_nsec/1000000000;
                   ts.tv_nsec %= 1000000000;
                   goto retry;
                    }
            }
            return error;
        }
        return 0;
}

#endif

EXPORT_SHARED void LOS_MutexLock(t_los_mutex_id mutex)
{
	struct los_mutex_t *los_mutex = (struct los_mutex_t *)mutex;
	t_los_process_id tid = LOS_GetCurrentId();

	ALOGV("LOS_MutexLock: 0x%08x\n", mutex);

	if (los_mutex->tid == tid) {
		ALOGW("LOS_MutexLock: 0x%08x called while already held tid=%d\n", mutex, tid);
	}

	// Use a timed lock to detect soft lockup - wait no more than 3s
	while (pthread_mutex_lock_timeout_np(&los_mutex->mutex, 3000)) {
		ALOGE("LOS_MutexLock: tid=%d - failed to aquire lock within 3s - held by tid=%d\n",
		     tid, los_mutex->tid);
	}
	ALOGV("LOS_MutexLock: 0x%08x acquired\n", mutex);
	los_mutex->tid = tid;
}

EXPORT_SHARED int LOS_MutexTryLock(t_los_mutex_id mutex)
{
	struct los_mutex_t *los_mutex = (struct los_mutex_t *)mutex;
	t_los_process_id tid = LOS_GetCurrentId();

	ALOGV("LOS_MutexTryLock: 0x%08x\n", mutex);

	if (los_mutex->tid == tid) {
		ALOGW("LOS_MutexLock: 0x%08x called while already held tid=%d\n", mutex, tid);
	}

	if (pthread_mutex_trylock(&los_mutex->mutex)) {
		ALOGV("LOS_MutexTryLock: 0x%08x already held\n", mutex);
		return LOS_BUSY;
	}

	ALOGV("LOS_MutexTryLock: 0x%08x acquired\n", mutex);
	los_mutex->tid = tid;

	return 0;
}

EXPORT_SHARED void LOS_MutexUnlock(t_los_mutex_id mutex)
{
	struct los_mutex_t *los_mutex = (struct los_mutex_t *)mutex;
	t_los_process_id tid = LOS_GetCurrentId();

        ALOGV("LOS_MutexUnlock: 0x%08x\n", mutex);

	if (los_mutex->tid != tid) {
		ALOGV("LOS_MutexUnlock: 0x%08x held by tid=%d unlocked by non helding tid=%d\n",
		     mutex, los_mutex->tid, tid);
	}
	los_mutex->tid = 0;
	pthread_mutex_unlock(&los_mutex->mutex);
	ALOGV("LOS_MutexUnlock: 0x%08x released\n", mutex);
}

/* 
 * \param[in] filename file name (string) of the file to open
 * \param[in] mode C string containing a file access modes
 *
 * \return (t_los_file *) pointer on the file instance
 *
 * \see <tt>man fopen</tt> for more information...
 *    
 * \ingroup LOS_FILE
 * */ 
EXPORT_SHARED t_los_file* LOS_fopen(const char *filename, const char *mode)
{
	return (t_los_file*)fopen(filename, mode);
}

/*!
 * \brief LOS_fread reads an opened "binary" file on the GFS
 * 
 * 
 * \param[in] ptr pointer to the file data which must be already allocated  
 * \param[in] size 
 * \param[in] count 
 * \param[in] stream pointer on the file instance
 *
 * \return (size_t) size of data really copied to the buffer
 *    
 * \see <tt>man fread</tt> for more information...
 *
 * \ingroup LOS_FILE
 * */ 
EXPORT_SHARED t_los_size LOS_fread(void *ptr, t_los_size size, t_los_size count, t_los_file *stream)
{
	return fread(ptr, size, count, (FILE *)stream);
}

/*!
 * \brief LOS_fseek seeks an opened "binary" file on the GFS
 * 
 * \param[in] stream pointer on the file instance 
 * \param[in] offset 
 * \param[in] origin 
 *
 * \return (int) If successful, the function returns a zero value. Otherwise, it returns nonzero value.
 *    
 * \see <tt>man fseek</tt> for more information...
 *
 * \ingroup LOS_FILE
 * */ 
EXPORT_SHARED int LOS_fseek(t_los_file *stream, long int offset, t_los_origin origin)
{
	return fseek((FILE *)stream, offset, origin);
}

/*!
 * \brief LOS_ftell returns the position of an opened "binary" file on the GFS
 * 
 * \param[in] stream pointer on the file instance 
 *
 * \return (int) the value returned corresponds to the number of bytes from the beginning of the file.
 *    
 * \see <tt>man ftell</tt> for more information...
 *
 * \ingroup LOS_FILE
 *
 */ 
EXPORT_SHARED long int LOS_ftell(t_los_file * stream)
{
	return ftell((FILE *)stream);
}


/*!
 * \brief LOS_fwrite writes an array of count elements.
 * 
 * LOS_fwrite writes an array of count elements, each one with a size of size bytes, 
 * from the block of memory pointed by ptr to the current position in the stream.
 * The postion indicator of the stream is advanced by the total amount of bytes written.
 * The total amount of bytes written is (size * count).
 * 
 * \param[in] ptr Pointer to the array of elements to be written.
 * \param[in] size Size in bytes of each element to be written.
 * \param[in] count Number of elements, each one with a size of size bytes.
 * \param[in] stream Pointer to a FILE object that specifies an output stream.
 *
 * \return The total number of elements successfully written is returned as a t_los_size object,
 *  which is an integral data type. If this number differs from the count parameter, it indicates an error.
 *    
 * \see <tt>man fwrite</tt> for more information...
 *
 * \ingroup LOS_FILE
 *
 */
EXPORT_SHARED t_los_size LOS_fwrite(const void *ptr, t_los_size size, t_los_size count, t_los_file *stream)
{
	return fwrite(ptr, size, count, (FILE *)stream);
}

/*!
 * \brief LOS_fclose closes already opened "binary" file on the GFS and free all memory allocated for it
 * 
 * \param[in] stream pointer on the file instance 
 *
 * \return (int) If the stream is successfully closed, a zero value is returned. On failure, EOF is returned.
 *    
 * \see <tt>man fclose</tt> for more information...
 *
 * \ingroup LOS_FILE
 *
 */
EXPORT_SHARED int LOS_fclose(t_los_file *stream)
{
	return fclose((FILE *)stream);
}

/*!
 * \brief Creates a semaphore, to synchronize objects
 * 
 * \param[in] value (int) value of how many times, the semaphore can be taken    
 * \return Handle (id) of the created sempahore (t_los_sem_id);
 *   
 * \ingroup LOS_SEM
 *
 */ 
EXPORT_SHARED t_los_sem_id LOS_SemaphoreCreate(int value)
{
	sem_t *sem = malloc(sizeof(*sem));

	if (sem == NULL) {
		ALOGE("%s: malloc returned NULL\n", __func__);
		return 0;
	}

	if (sem_init(sem, 0, value)) {
		ALOGE("%s: sem_init failed: errno=%d (%s)\n", __func__, errno, strerror(errno));
		free(sem);
		return 0;
	}
	return (t_los_sem_id)sem;
}

/*!
 * \brief Destroys (and releases all memory allocated for it) the given semaphore
 * 
 * \param[in] hsem id of the sempahore (t_los_sem_id);
 *  
 * \return 0
 *   
 * \ingroup LOS_SEM
 *
 */
EXPORT_SHARED int LOS_SemaphoreDestroy(t_los_sem_id hsem)
{
	if (sem_destroy((sem_t *)hsem))
#ifdef ANDROID
		/* For now, Android sem_destroy() wrongly returns EBUSY if sem->count==0
		   => Ignore this error in that case to avoid a memory leak by not
		   freeing this semaphore */
		if (errno != EBUSY)
#endif
		return -1;
	free((void*)hsem);
	return 0;
}

/*!
 * \brief Wait on the given semaphore. If a thread try to take the semaphore more than possible, the thread waits.
 * 
 *  
 * \param[in] hsem id of the sempahore (t_los_sem_id);
 *   
 * \ingroup LOS_SEM
 *
 */ 
EXPORT_SHARED void LOS_SemaphoreWait(t_los_sem_id hsem)
{
retry:
   if (sem_wait((sem_t *)hsem)) {
		if (errno == EINTR)
			goto retry;
		else
			ALOGE("%s: sem_wait failed: errno=%d (%s)\n", __func__, errno, strerror(errno));
	}
}


/*!
 * \brief Wait on the given semaphore. If a thread try to take the semaphore more than possible, the thread waits unless
 *  timeout expired. 
 * 
 * \note Use LOS_SemaphoreWait to wait without timeout.
 *  
 * \param[in] hsem id of the sempahore (t_los_sem_id);
 * \param[in] timeOutInMs timeout in milli seconds.
 * 
 * \return 0 on success, -1 on timeout.
 * 
 * \ingroup LOS_SEM
 *
 */ 
EXPORT_SHARED int LOS_SemaphoreWaitTimeOut(t_los_sem_id hsem, t_uint32 timeOutInMs)
{
	/* bionic uses CLOCK_REALTIME to compute back a relative time sent to kernel */
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_nsec += (timeOutInMs%1000) * 1000000;
	ts.tv_sec += timeOutInMs/1000 + ts.tv_nsec/1000000000;
	ts.tv_nsec %= 1000000000;

	/* To check against false timeout we'll use CLOCK_MONOTONIC */
	struct timespec abs_timeout;
	clock_gettime(CLOCK_MONOTONIC, &abs_timeout);
	abs_timeout.tv_nsec += (timeOutInMs%1000) * 1000000;
	abs_timeout.tv_sec += timeOutInMs/1000 + abs_timeout.tv_nsec/1000000000;
	abs_timeout.tv_nsec %= 1000000000;
retry:
	if (sem_timedwait((sem_t *)hsem, &ts)) {
		if (errno == EINTR)
			goto retry;
		if (errno == ETIMEDOUT) {
			struct timespec mts,rts;
			clock_gettime(CLOCK_MONOTONIC, &mts);
			timesub(&abs_timeout, &mts, &rts);
			if (rts.tv_sec < 0 || rts.tv_nsec < 0) {
				/* Real timeout */
				return -1;
			} else {
				/* Spurious timeout due to time change
				   recompute absolute timeout upon CLOCK_REALTIME */
				clock_gettime(CLOCK_REALTIME, &ts);
				ts.tv_nsec += rts.tv_nsec;
				ts.tv_sec  += rts.tv_sec + ts.tv_nsec/1000000000;
				ts.tv_nsec %= 1000000000;
				goto retry;
			}
		}
		return -1;
	}
	return 0;
}

/*!
 * \brief LOS_SemaphoreTryWait is a non-blocking variant of LOS_SemaphoreWait
 * 
 * \param[in] hsem pointer to the semaphore object
 *  
 * \return 0 on success, -1 on failure
 * 
 * \ingroup LOS_SEM
 *
 */
EXPORT_SHARED int LOS_SemaphoreTryWait(t_los_sem_id hsem)
{
retry:
	if (sem_trywait((sem_t *)hsem)) {
		if (errno == EINTR)
			goto retry;
		return -1;
	}
	return 0;
}


/*!
 * \brief Notify and give back the given semaphore. the next thread waiting for the semaphore automatically gets it.
 *  
 * \param[in] hsem id of the sempahore (t_los_sem_id);
 *   
 * \ingroup LOS_SEM
 *
 */ 
EXPORT_SHARED void LOS_SemaphoreNotify(t_los_sem_id hsem)
{
	sem_post((sem_t *)hsem);
}

/*!
 * \brief LOS_SemaphoreGetCount stores in the location pointed to by sval the current count of the semaphore hsem. 
 * 
 * \param[in] hsem sem pointer to the semaphore object t_los_sem_id
 * \param[out] sval pointer to the counter value
 *  
 * \return 0
 *  
 * \ingroup LOS_SEM
 *
 */ 
EXPORT_SHARED int LOS_SemaphoreGetCount(t_los_sem_id hsem, int *sval)
{
	return sem_getvalue((sem_t *)hsem, sval);

}

/*!
 * \brief Get the system time and returns it in microsecond
 * 
 * The implementation of this function is owned by the OS implementation choice. 
 *  
 * 
 * \return time (unsigned long long) in microsecond since the beginning of the OS boot
 *   
 * \ingroup LOS_TIME
 *
 */ 
EXPORT_SHARED unsigned long long LOS_getSystemTime(void)
{
	struct timeval tv ={0, 0};
		
	gettimeofday(&tv, NULL);
	return (unsigned long long)tv.tv_sec*1000000 + (unsigned long long)tv.tv_usec;
}
