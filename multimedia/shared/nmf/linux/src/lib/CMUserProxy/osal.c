/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/** \file osal.c
 *
 * Implementation of the CM User Proxy OSAL Implementation
 * Linked with user space NMF applications
 *
 */


#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <inc/nmf-limits.h>
#include <cm/proxy/inc/osal.h>
#include "cmioctl.h"

#include "cmproxy.h"

/** \defgroup OSAL_IMPLEMENTATION Implementation of the NMF user space OSAL interface
 */

struct osal_file {
	t_uint32 fd;
	size_t size;
	void *data;
};

extern enum board_version board;

/** \ingroup OSAL_IMPLEMENTATION
 * OSAL_Alloc implementation
 */
void* OSAL_Alloc(t_cm_size size)
{
	return malloc(size); 
}

/** \ingroup OSAL_IMPLEMENTATION
 * OSAL_Free implementation
 */
void OSAL_Free(void* mem)
{
	free(mem);
}

/** \ingroup OSAL_IMPLEMENTATION
 * OSAL_Log implementation
 */
void  OSAL_Log(const char *format, void* param1, void* param2, void* param3)
{
#ifdef ANDROID
	ALOGE(format, param1, param2, param3);
#else
	printf(format, param1, param2, param3);
#endif

}

/** \ingroup OSAL_IMPLEMENTATION
 * OSAL_Read implementation
 */
t_nmf_osal_fs_file_handle OSAL_ReadFile(const char *fileName, void **buffer, t_cm_size *pSize, t_bool userspace)
{
	char fn[MAX_COMPONENT_FILE_PATH_LENGTH];
	int fd, len;
	void *data;
	struct stat stat;
        struct osal_file *p_osal_file;

	(void) userspace; /* avoid warning about unused parameter */

	if (board == U8500_V2) {
		len = __builtin_strlen(REPOSITORY_DIR "mmdsp_8500_v2/");
		strncpy(fn, REPOSITORY_DIR "mmdsp_8500_v2/", len+1);
	} else if (board == U9540_V1) {
		len = __builtin_strlen(REPOSITORY_DIR "mmdsp_9540_v1/");
		strncpy(fn, REPOSITORY_DIR "mmdsp_9540_v1/", len+1);
	} else
		return 0;

	if ((len + strlen(fileName)) > MAX_COMPONENT_FILE_PATH_LENGTH-1) {
		ALOGE("%s - filename too long: %s\n", __func__, fileName);
		return 0;
	}

	strncat(fn, fileName, MAX_COMPONENT_FILE_PATH_LENGTH - len);

	fd = open(fn, O_RDONLY);
	if (fd == -1) {
		ALOGE("%s - Can't open file %s: %s\n", __func__, fn, strerror(errno));
		return 0;
	}

	if (fstat(fd, &stat)) {
		close(fd);
		ALOGE("%s - Can't stat file %s: %s\n", __func__, fn, strerror(errno));
		return 0;
	} 

	data = mmap(NULL, stat.st_size, PROT_READ, MAP_SHARED, fd, 0);

	if (data == MAP_FAILED) {
		close(fd);
		ALOGE("%s - Can't mmap file %s: %s\n", __func__, fn, strerror(errno));
		return 0;
	}

	p_osal_file = malloc(sizeof(*p_osal_file));
	if (p_osal_file == NULL) {
		munmap(data, stat.st_size);		
		close(fd);
		ALOGE("%s - Can't allocate memory\n", __func__);
		return 0;
	}
	*buffer = data;
	*pSize  = stat.st_size;
	p_osal_file->fd = fd;
	p_osal_file->data = data;
	p_osal_file->size = stat.st_size;

	return (t_nmf_osal_fs_file_handle)p_osal_file;
}

/** \ingroup OSAL_IMPLEMENTATION
 * OSAL_CloseFile implementation
 */
void OSAL_CloseFile(t_nmf_osal_fs_file_handle handle)
{
        struct osal_file *p_osal_file = (struct osal_file *)handle;

	munmap(p_osal_file->data, p_osal_file->size);
	close(p_osal_file->fd);
	free(p_osal_file);
}

/** \ingroup OSAL_IMPLEMENTATION
 * OSAL_CreateLock implementation
 */
t_nmf_osal_sync_handle OSAL_CreateLock(void)
{
	pthread_mutex_t *mutex = malloc(sizeof(*mutex));

	if (!mutex)
		return 0;

	*mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	return (t_nmf_osal_sync_handle)mutex;
}

/** \ingroup OSAL_IMPLEMENTATION
 * OSAL_Lock implementation
 */
void OSAL_Lock(t_nmf_osal_sync_handle handle)
{
	pthread_mutex_lock((pthread_mutex_t *)handle);
}

/** \ingroup OSAL_IMPLEMENTATION
 * OSAL_Unlock implementation
 */
void OSAL_Unlock(t_nmf_osal_sync_handle handle)
{
	pthread_mutex_unlock((pthread_mutex_t *)handle);
}

/** \ingroup OSAL_IMPLEMENTATION
 * OSAL_DestroyLock implementation
 */
void OSAL_DestroyLock(t_nmf_osal_sync_handle handle)
{
	free((void *)handle);
}

#ifdef CM_API_TIMING
/** \ingroup OSAL_IMPLEMENTATION
 * OSAL_GetTime implementation
 */
unsigned long long OSAL_GetTime(void)
{
	struct timeval tv ={0, 0};
		
	gettimeofday(&tv, NULL);
	return (unsigned long long)tv.tv_sec*1000000 + (unsigned long long)tv.tv_usec;
}
#endif
