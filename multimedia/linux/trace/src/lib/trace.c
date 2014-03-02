/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#if defined (__ARM_LINUX)

#include <trace/stm.h>
#include "stm-trace.h"

static struct stm_channel *stm_channels = NULL;
static int max_channels;
static int fd = -1, use_count=0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_key_t channel_key;

////////////////////////////////////////////////////////////////////////////
int stm_trace_open(void)
{
	int error=0;
	pthread_mutex_lock(&mutex);
	if (fd == -1) {
		fd = open("/dev/" STM_DEV_NAME, O_RDWR);
		if (fd == -1) {
			error = errno;
#ifdef TRACE_DEBUG
			perror ("Open device \"trace\" failed:");
#endif
		} else {
			ioctl(fd, STM_GET_NB_MAX_CHANNELS, &max_channels);
			stm_channels = (struct stm_channel *)
				mmap (0, max_channels*sizeof(*stm_channels),
					PROT_WRITE, MAP_SHARED, fd, 0);
			if (stm_channels == MAP_FAILED) {
				error = errno;
#ifdef TRACE_DEBUG
				perror ("Mapping \"trace\" device failed:");
#endif
				close (fd);
				fd = -1;
				stm_channels = NULL;
			} else
				use_count = 1;
		}
	} else
		use_count++;
	pthread_mutex_unlock(&mutex);
	return error;
}

void stm_trace_close(void)
{
	pthread_mutex_lock(&mutex);
	if (0 == --use_count) {
		munmap((void *)stm_channels, max_channels*sizeof(*stm_channels));
		stm_channels = NULL;
		close (fd);
		fd = -1;
	}
	pthread_mutex_unlock(&mutex);
}

#define DEFLLTFUN(type, size) \
	void  stm_trace_##size(unsigned char channel, unsigned type data) \
	{ *((volatile unsigned type *) &(stm_channels[channel].no_stamp##size)) = data;} \
	void stm_tracet_##size(unsigned char channel, unsigned type data) \
	{ *((volatile unsigned type *) &(stm_channels[channel].stamp##size)) = data;}

DEFLLTFUN(char, 8)
DEFLLTFUN(short, 16)
DEFLLTFUN(int, 32)
DEFLLTFUN(long long, 64)

static void free_key(void *channel) {
	if (channel) {
		ioctl(fd, STM_RELEASE_CHANNEL, ((int)channel)-1);
		stm_trace_close();
	}
}

static void create_key(void) {
	pthread_key_create(&channel_key, free_key);
}

static int get_key_value(void) {
	static pthread_once_t keycreated = PTHREAD_ONCE_INIT;
	pthread_once(&keycreated, create_key);
	return (int)pthread_getspecific(channel_key);
}

/* Register the caller thread as trace user, get and associate a STM channel */
static int stm_trace_register_thread(int *channel) {
	int error;

	if (stm_trace_open())
		return -1;

	/* The channel returned by the driver is numbered between 0 to 255 */
	if (ioctl(fd, STM_GET_FREE_CHANNEL, channel)) {
		stm_trace_close();
#ifdef TRACE_DEBUG
		perror("Error registering thread in stm_trace system:");
#endif
		return errno;
	}

	error = pthread_setspecific(channel_key, (void*)((*channel)+1));
	if (error) {
		ioctl(fd, STM_RELEASE_CHANNEL, *channel);
		stm_trace_close();
#ifdef TRACE_DEBUG
		fprintf(stderr, "Error registering threads as trace user: %s\n", strerror(error));
#endif
		return error;
	}

	return 0;
}

/* Retreive the channel ID if already registered
   if not already registered then register this thread */
int stm_trace_get_thread_channel(int *channel) {

	int channel_id = get_key_value();
	
	if (channel_id)
		channel_id--;
	else if (stm_trace_register_thread(&channel_id))
		return -1;

	*channel = channel_id;
	return 0;
}

/* Unregister the caller thread as trace user */
void stm_trace_free_thread_channel(void) {
	int channel = get_key_value();

	if (channel) {
		ioctl(fd, STM_RELEASE_CHANNEL, channel-1);
		pthread_setspecific(channel_key, 0);
	}
	stm_trace_close();
}

/* Output the buffer into the XTI channel */
int stm_trace_buffer(size_t size, const char *buffer)
{
	int channel;

	if (stm_trace_get_thread_channel(&channel) != 0)
  		return -1;
	
	while (size > 0) {
		if (size > 8) {
			stm_trace_64(channel, (long long)htonl(*(int*)buffer)<<32 | htonl(*(int *)(buffer+4)));
			size   -= 8;
			buffer += 8;
		} else  {
			if (size == 8) {
				stm_tracet_64(channel, (long long)htonl(*(int*)buffer)<<32 | htonl(*(int *)(buffer+4)));
				return 0;
			}

			if (size > 4) {
				stm_trace_32(channel, htonl(*(int*)buffer));
				size   -= 4;
				buffer += 4;
			}

			if (size == 4) {
				stm_tracet_32(channel, htonl(*(int*)buffer));
				return 0;
			}

			if (size > 2) {
				stm_trace_16(channel, htons(*(short*)buffer));
				size   -= 2;
				buffer += 2;
			}
			if (size == 2) {
				stm_tracet_16(channel, htons(*(short*)buffer));
				return 0;
			}
			if (size /* == 1 */) {
				stm_tracet_8(channel, *buffer);
				return 0;
			}
		}
	}
	return 0;
}

#endif	/* #if defined (__ARM_LINUX) */

