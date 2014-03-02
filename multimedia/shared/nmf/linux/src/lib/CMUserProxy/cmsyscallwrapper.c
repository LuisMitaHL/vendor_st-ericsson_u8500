/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/** \file cmsyscallwrapper.c
 *
 * Implementation of the Component Manager Syscall wrapper.
 * Linked with user space NMF applications
 *
 */
#ifndef ANDROID
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#endif
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <cm/os/api/cm_os.h>
#include <cm/inc/cm.h>
#include <cm/proxy/common/communication/inc/communication_internal.h>

#include "cmproxy.h"
#include "cmioctl.h"

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static unsigned int usage_count = 0;
static char *cmld_devname[] = CMLD_DEV_NAME;
enum board_version board;

static void cm_init_once(void);

__attribute__((constructor)) static void cm_LibProxyInit(void)
{
	CM_INTERNAL_ProxyInit();
}

/** \defgroup USERPROXYLIB_INTERFACE Component Manager Proxy Library Interface
 * Includes public functions used by applications to operate NMF.
 */

/** \defgroup OS_API Implementation of the NMF Syscall API
 */

/** \defgroup OS_PRIVAPI New, private, Syscall entry points
 */

/* private functions */
static inline t_cm_error CM_OS_GetMpcMemoryDesc(t_cm_memory_handle handle, t_uint32* size, t_uint32* physAddr, t_uint32* logicalAddr, t_uint32* dspAddr);

// Declare the gettid syscall.
//#include <sys/syscall.h>
//static pid_t gettid(void) { return syscall(__NR_gettid); }

// Control device kept opened till no more callback channel are in use
static int control_fd;

int cm_control(int cmd, void* data, const char *caller)
{
	if (ioctl(control_fd, cmd, data)) {
		if (caller)
			ALOGE("%s - ioctl error: errno=%d (%s)\n", caller, errno,
			     strerror(errno));
		else
			ALOGE("%s(cmd=%d) error: errno=%d (%s)\n", __func__, cmd,
			     errno, strerror(errno));
		return CM_INTEGRATION_ERROR;
	}

	return 0;
}

/** \ingroup USERPROXYLIB_INTERFACE
 * Initializes Component Manager User Proxy.
 * Opens the cm device.
 *
 * \return NMF error code
 */
static t_nmf_error cm_init(void)
{
	char cmdevice[sizeof("/dev/")+11] = "/dev/";
	int err;
	t_uint32 nmfLibVer, nmfDriverVer;
	static pthread_once_t once = PTHREAD_ONCE_INIT;

	strcat(cmdevice, cmld_devname[0]);
	control_fd = open(cmdevice, O_RDWR|O_CLOEXEC);
	if (control_fd < 0) {
		ALOGE("%s: failed to open %s: %s\n",
		     __func__, cmdevice, strerror(errno));
		return CM_INTEGRATION_ERROR;
	}
	
	CM_GetVersion(&nmfLibVer);
	err = cm_control(CM_GETVERSION, &nmfDriverVer, __func__);
	if (err) {
		close(control_fd);
		return err;
	}

	if ((VERSION_MAJOR(nmfLibVer) != VERSION_MAJOR(nmfDriverVer))
	    && (VERSION_MINOR(nmfLibVer) != VERSION_MINOR(nmfDriverVer))) {
		ALOGE("%s: Error: Incompatible version between NMF User Proxy and NMF Driver\n", __func__);
		ALOGE("\tNMF User Proxy has version %lu.%lu.x\n",
		     VERSION_MAJOR(nmfLibVer), VERSION_MINOR(nmfLibVer));
		ALOGE("\tNMF Driver has version %lu.%lu.x\n",
		     VERSION_MAJOR(nmfDriverVer), VERSION_MINOR(nmfDriverVer));
		close(control_fd);
		return CM_INVALID_PARAMETER;
	}

	err = cm_control(CM_PRIV_GETBOARDVERSION, &board, __func__);
	if (err) {
		close(control_fd);
		return err;
	}

	err = CM_INTERNAL_Init();
	if (err != CM_OK)
		close(control_fd);
	pthread_once(&once, cm_init_once);
	return err;
}

static void cm_destroy(void)
{
	close(control_fd);
	CM_INTERNAL_Destroy();
}

/** \ingroup USERPROXYLIB_INTERFACE
 * - Initializes Component Manager User Proxy (at first call).
 * - Create a channel
 *
 * \return CM error code
 */
PUBLIC EXPORT_SHARED t_nmf_error CM_CreateChannel(t_nmf_channel_flag flags, t_nmf_channel *channel)
{
	t_os_channel *pOSchannel;
	t_nmf_error status;

	pthread_mutex_lock(&mutex);
	if (usage_count == 0) {
		status = cm_init();
		if (status != CM_OK) {
			pthread_mutex_unlock(&mutex);
			return status;
		}
	}

	status = CM_INTERNAL_CreateChannel(flags, channel, &pOSchannel);

	if (status == CM_OK && pOSchannel != NULL) {
		char cmdevice[sizeof("/dev/")+11] = "/dev/";
		int fd;
		strcat(cmdevice, cmld_devname[1]);
		fd = open(cmdevice, O_RDONLY|O_CLOEXEC);
		if (fd < 0) {
			CM_INTERNAL_CloseChannel(*channel, NULL);
			ALOGE("%s: failed to open %s: %s\n",
			     __func__, cmdevice, strerror(errno));
			status = CM_INTEGRATION_ERROR;
		} else
			*pOSchannel = fd;
	}

	if (status == CM_OK)
		usage_count++;
	else if (usage_count == 0)
		cm_destroy();

	pthread_mutex_unlock(&mutex);
	return status;
}

PUBLIC EXPORT_SHARED t_nmf_error CM_FlushChannel(t_nmf_channel channel, t_bool *isFlushMessageGenerated)
{
	t_os_channel OSchannel;
	t_nmf_error status;

	if (isFlushMessageGenerated == NULL)
		return CM_INVALID_PARAMETER;

	*isFlushMessageGenerated = FALSE;

	status = CM_INTERNAL_FlushChannel(channel, &OSchannel);
	if (status == NMF_OK) {
		if (OSchannel) {
			*isFlushMessageGenerated = TRUE;
			if (ioctl((int)OSchannel, CM_FLUSHCHANNEL)) {
				ALOGE("%s error: errno=%d (%s)\n", __func__, errno,
				     strerror(errno));
				return CM_INTEGRATION_ERROR;
			}
		}
	}
	return status;
}

PUBLIC EXPORT_SHARED t_nmf_error CM_CloseChannel(t_nmf_channel channel)
{
	t_os_channel OSchannel;
	t_nmf_error status;
	
	pthread_mutex_lock(&mutex);

	status = CM_INTERNAL_CloseChannel(channel, &OSchannel);
	if (status == CM_OK) {
		if (OSchannel)
			close((int)OSchannel);
		if (--usage_count == 0)
			cm_destroy();
	}

	pthread_mutex_unlock(&mutex);
	return status;
}

/** \ingroup USERPROXYLIB_INTERFACE
 * Handle next incoming event
 *
 * \return POSIX error code
 */
t_cm_error CM_OS_GetMessage(
        t_nmf_channel               channel,
        t_os_message *              message,
        t_uint32                    messagesize,
        t_bool                      block)
{
	int fd = (int)channel;
	int len, flag;

	flag = block ? 0 : O_NONBLOCK;
	if (fcntl(fd, F_SETFL, flag)) 
		ALOGE("%s: fnctl() failed: %s\n", __func__, strerror(errno));

	len = read(fd, message, messagesize);
	if (len < 0) {
		if (errno == EMSGSIZE)
			return CM_BUFFER_OVERFLOW;
		if (errno == EAGAIN)
			return CM_NO_MESSAGE;
		return CM_INVALID_PARAMETER;;
	}

	if (len == 0)
		return CM_FLUSH_MESSAGE;

	if ((message->type == MSG_SERVICE)
	    && (message->data.srv.type == NMF_SERVICE_PANIC)
	    && cm_has_debugfs()) {
		if (message->data.srv.data.panic.panicSource & DEBUGFS_DUMP_FLAG) {
			message->data.srv.data.panic.panicSource
				&= ~DEBUGFS_DUMP_FLAG;
			cm_debugfs_dump(&message->data.srv.data);
			ioctl(control_fd, CM_PRIV_DEBUGFS_DUMP_DONE);
		} else {
			ioctl(control_fd, CM_PRIV_DEBUGFS_WAIT_DUMP);
		}
	}
	return 0;
}

/*** CM User Proxy NMF Syscall API ***/

/** \ingroup OS_API
 * CM_OS_InstantiateComponent user space implementation
 */
t_cm_error CM_OS_InstantiateComponent(const char* templateName, t_cm_domain_id domainId, t_nmf_ee_priority priority,
				      const char* localName, const char *dataFile, t_uint32 dataFileSize, t_cm_instance_handle *component)
{
	int err;
	CM_InstantiateComponent_t data;

	data.in.templateName = templateName;
	data.in.domainId = domainId;
	data.in.priority = priority;
	data.in.localName = localName;
	data.in.dataFile = dataFile;
	data.in.dataFileSize = dataFileSize;
	err = cm_control(CM_INSTANTIATECOMPONENT, &data, __func__);
	if (err)
		return err;
	
	if (data.out.error == CM_OK)
		*component = data.out.component;

	return data.out.error;
}


/** \ingroup OS_API
 * CM_OS_PushEventWithSize user space implementation
 */
t_cm_error CM_OS_PushEventWithSize(t_cm_bf_host2mpc_handle host2mpcId, t_event_params_handle h, t_uint32 size, t_uint32 methodIndex)
{
	int err;
	CM_PushEventWithSize_t data;

	data.in.host2mpcId = host2mpcId;
	data.in.h = h;
	data.in.size = size;
	data.in.methodIndex = methodIndex;

	err = cm_control(CM_PUSHEVENTWITHSIZE, &data, __func__);
	if (err != 0 || data.out.error) {
		ALOGE("%s: ioctl error (errno=%d, CM error=%s)\n", __func__, errno,
		     CM_StringError(data.out.error));
		return CM_INTEGRATION_ERROR;
	}

	return data.out.error;
}


/** \ingroup OS_API
 *	CM_OS_BindComponentFromCMCore user space implementation
 */
t_cm_error CM_OS_BindComponentFromCMCore(const t_cm_instance_handle server, const char* providedItfServerName, t_uint32 fifosize,
	t_cm_mpc_memory_type eventMemType,
	t_cm_bf_host2mpc_handle *host2mpcId,
	const char *dataFileSkeleton,
	t_uint32 dataFileSkeletonSize)
{
	int err;
	CM_BindComponentFromCMCore_t data;

	data.in.server = server;
	data.in.providedItfServerName = providedItfServerName;
	data.in.fifosize = fifosize;
	data.in.eventMemType = eventMemType;
	data.in.dataFileSkeleton = dataFileSkeleton;
	data.in.dataFileSkeletonSize = dataFileSkeletonSize;
	err = cm_control(CM_BINDCOMPONENTFROMCMCORE, &data, __func__);
	if (err)
		return err;
	
	if (data.out.error == CM_OK)
		*host2mpcId = data.out.host2mpcId;

	return data.out.error;
}

/** \ingroup OS_API
 *	CM_OS_UnbindComponentFromCMCore user space implementation
 */
t_cm_error CM_OS_UnbindComponentFromCMCore(t_cm_bf_host2mpc_handle host2mpcId)
{
	int err;
	CM_UnbindComponentFromCMCore_t data;
	
	data.in.host2mpcId = host2mpcId;
	
	err = cm_control(CM_UNBINDCOMPONENTFROMCMCORE, &data, __func__);
	if (err)
		return err;
	
	return data.out.error;
}

/** \ingroup OS_API
 * CM_OS_BindComponentToCMCore user space implementation
 * Calls the "wrapped" version in kernel space Cfr. MPC2HOST.html
 */
t_cm_error CM_OS_BindComponentToCMCore(const t_os_channel channel, const t_cm_instance_handle client, const char* requiredItfClientName,
				       t_uint32 fifosize, t_nmf_mpc2host_handle upLayerThis, const char *dataFileStub,
				       t_uint32 dataFileStubSize, t_cm_bf_mpc2host_handle* mpc2hostId)
{
	CM_BindComponentToCMCore_t data;
	
	data.in.client = client;
	data.in.requiredItfClientName = requiredItfClientName;
	data.in.fifosize = fifosize;
	data.in.upLayerThis = upLayerThis;
	data.in.dataFileStub = dataFileStub;
	data.in.dataFileStubSize = dataFileStubSize;
	
	if (ioctl((int)channel, CM_BINDCOMPONENTTOCMCORE, &data)) {
		ALOGE("%s error: errno=%d (%s)\n", __func__, errno, strerror(errno));
		return CM_INTEGRATION_ERROR;
	}

	if (data.out.error == CM_OK)
		*mpc2hostId = data.out.mpc2hostId;
	
	return data.out.error;
}

/** \ingroup OS_API
 * CM_OS_UnbindComponentToCMCore user space implementation
 * Calls the "wrapped" version in kernel. Cfr. MPC2HOST.html
 */
t_cm_error CM_OS_UnbindComponentToCMCore(const t_os_channel channel,
					 const t_cm_instance_handle client,
					 const char *requiredItfClientName,
					 t_nmf_mpc2host_handle *upLayerThis)
{
	int err;
	CM_UnbindComponentToCMCore_t data;
	(void) channel; //avoid a warning

	data.in.client = client;
	data.in.requiredItfClientName = requiredItfClientName;
	err = cm_control(CM_UNBINDCOMPONENTTOCMCORE, &data, __func__);
	if (err)
		return err;

	if (data.out.error == CM_OK || data.out.error == CM_MPC_NOT_RESPONDING)
		*upLayerThis = data.out.upLayerThis;
	
	return data.out.error;
}

/** \ingroup OS_API
 */
t_cm_error CM_OS_BindComponentAsynchronous(
        const t_cm_instance_handle client,
        const char* requiredItfClientName,
        const t_cm_instance_handle server,
        const char* providedItfServerName,
        t_uint32 fifosize,
        t_cm_mpc_memory_type eventMemType,
        const char *dataFileSkeletonOrEvent,
        t_uint32 dataFileSkeletonOrEventSize,
        const char *dataFileStub,
        t_uint32 dataFileStubSize)
{
	int err;
	CM_BindComponentAsynchronous_t data;
	
	data.in.client = client;
	data.in.requiredItfClientName = requiredItfClientName;
	data.in.server = server;
	data.in.providedItfServerName = providedItfServerName;
	data.in.fifosize = fifosize;
	data.in.eventMemType = eventMemType;
	data.in.dataFileSkeletonOrEvent = dataFileSkeletonOrEvent;
	data.in.dataFileSkeletonOrEventSize = dataFileSkeletonOrEventSize;
	data.in.dataFileStub = dataFileStub;
	data.in.dataFileStubSize = dataFileStubSize;

	err = cm_control(CM_BINDCOMPONENTASYNCHRONOUS, &data, __func__);
	if (err)
		return err;

	return data.out.error;
}

/** \ingroup OS_API
 */
t_cm_error CM_OS_UnbindComponentAsynchronous(const t_cm_instance_handle client, const char* requiredItfClientName) 
{
	int err;
	CM_UnbindComponentAsynchronous_t data;
	
	data.in.client = client;
	data.in.requiredItfClientName = requiredItfClientName;
		
	err = cm_control(CM_UNBINDCOMPONENTASYNCHRONOUS, &data, __func__);
	if (err)
		return err;
	
	return data.out.error;
}

/** \ingroup OS_API
 */
t_cm_error CM_OS_BindComponent(const t_cm_instance_handle client, const char* requiredItfClientName,
			       const t_cm_instance_handle server, const char* providedItfServerName,
			       t_bool traced, const char *dataFileTrace, t_uint32 dataFileTraceSize)
{
	int err;
	CM_BindComponent_t data;
 
	data.in.client = client;
	data.in.requiredItfClientName = requiredItfClientName;
	data.in.server = server;
	data.in.providedItfServerName = providedItfServerName;
	data.in.traced = traced;
	data.in.dataFileTrace = dataFileTrace;
	data.in.dataFileTraceSize = dataFileTraceSize;
 
	err = cm_control(CM_BINDCOMPONENT, &data, __func__);
	if (err)
		return err;
	
	return data.out.error;
}
 
/** \ingroup OS_API
 */
t_cm_error CM_OS_UnbindComponent(const t_cm_instance_handle client, const char* requiredItfClientName)
{
	int err;
	CM_UnbindComponent_t data;
	
	data.in.client = client;
	data.in.requiredItfClientName = requiredItfClientName;
		
	err = cm_control(CM_UNBINDCOMPONENT, &data, __func__);
	if (err)
		return err;
	
	return data.out.error;
}

/** \ingroup OS_API
 */
t_cm_error CM_OS_BindComponentToVoid(const t_cm_instance_handle client, const char* requiredItfClientName)
{
	int err;
	CM_BindComponentToVoid_t data;
	
	data.in.client = client;
	data.in.requiredItfClientName = requiredItfClientName;
		
	err = cm_control(CM_BINDCOMPONENTTOVOID, &data, __func__);
	if (err)
		return err;
	
	return data.out.error;
}

/** \ingroup OS_API
 */
t_cm_error CM_OS_StartComponent(t_cm_instance_handle component)
{
	int err;
	CM_StartComponent_t data;

	data.in.client = component;
	err = cm_control(CM_STARTCOMPONENT, &data, __func__);
	if (err)
		return err;
	
	return data.out.error;
}


/** \ingroup OS_API
 */
t_cm_error CM_OS_StopComponent(t_cm_instance_handle component)
{
	int err;
	CM_StopComponent_t data;

	data.in.client = component;
	err = cm_control(CM_STOPCOMPONENT, &data, __func__);
	if (err)
		return err;
	
	return data.out.error;
}

/** \ingroup OS_API
 */
t_cm_error CM_OS_getMpcLoadCounter(t_nmf_core_id coreId, t_cm_mpc_load_counter *pMpcLoadCounter)
{
	int err;
	CM_GetMpcLoadCounter_t data;

	data.in.coreId = coreId;
	err = cm_control(CM_GETMPCLOADCOUNTER, &data, __func__);
	if (err)
		return err;

	if (data.out.error == CM_OK)
		*pMpcLoadCounter = data.out.pMpcLoadCounter;
	
	return data.out.error;
}



/** \ingroup OS_API
 *	CM_OS_DestroyComponent user space implementation
 */
t_cm_error CM_OS_DestroyComponent(t_cm_instance_handle component)
{
	int err;
	CM_DestroyComponent_t data;

	data.in.component = component;
	err = cm_control(CM_DESTROYCOMPONENT, &data, __func__);
	if (err)
		return err;

	return data.out.error;
}

/* */
/*!
 * \brief Create a memory domain.
 *
 * Creates a domain in the CM, valid for component instantiation
 * or memory allocation.
 *
 * \param[in]  domain Domain description
 * \param[out] handle Domain handle
 *
 * \ingroup CM_OS_API
 */
t_cm_error CM_OS_CreateMemoryDomain(const t_cm_domain_memory  *domain,
				    t_nmf_client_id            client,
				    t_cm_domain_id            *handle)
{
	int err;
	CM_CreateMemoryDomain_t data;

	data.in.domain = domain;
	data.in.client = client;
	err = cm_control(CM_CREATEMEMORYDOMAIN, &data, __func__);
	if (err)
		return err;

	if (data.out.error == CM_OK && handle)
		*handle = data.out.handle;

	return data.out.error;
}


/*!
 * \brief Create a scratch domain.
 *
 * \param[in]  parentId Identifier of the parent domain.
 * \param[in]  domain Description of domain memories.
 * \param[out] handle Identifier of the created domain
 *
 * \ingroup CM_OS_API
 */
t_cm_error CM_OS_CreateMemoryDomainScratch(const t_cm_domain_id parentId,
					   const t_cm_domain_memory *domain,
					   t_cm_domain_id *handle)
{
	int err;
	CM_CreateMemoryDomainScratch_t data;

	data.in.parentId = parentId;
	data.in.domain = domain;
	err = cm_control(CM_CREATEMEMORYDOMAINSCRATCH, &data, __func__);
	if (err)
		return err;

	if (data.out.error == CM_OK && handle)
		*handle = data.out.handle;

	return data.out.error;
}

/*!
 * \brief Destroy a memory domain.
 *
 * \param[in]  domain Domain identifier to destroy.
 *
 * \ingroup CM_OS_API
 */
t_cm_error CM_OS_DestroyMemoryDomain(const t_cm_domain_id domainId)
{
	int err;
	CM_DestroyMemoryDomain_t data;

	data.in.domainId = domainId;
	err = cm_control(CM_DESTROYMEMORYDOMAIN, &data, __func__);
	if (err)
		return err;

	return data.out.error;
}

/*!
 * \brief Retrieve the coreId of the given domain
 *
 * \param[in]  domainId Domain identifier.
 * \param[out] coreId   Core identifier.
 *
 * \ingroup CM_OS_API
 */
t_cm_error CM_OS_GetDomainCoreId(const t_cm_domain_id domainId, t_nmf_core_id *coreId)
{
	int err;
	CM_GetDomainCoreId_t data;

	data.in.domainId = domainId;
	err = cm_control(CM_GETDOMAINCOREID, &data, __func__);
	if (err)
		return err;

	if (data.out.error == CM_OK && coreId)
		*coreId = data.out.coreId;

	return data.out.error;
}

/** \ingroup OS_API
 * CM_OS_AllocMpcMemory user space implementation
 * \note: the preferredLocationArray should be revisited, as we loose here its size and cannot push it down.
 *        Only first element is passed here.
 */
t_cm_error CM_OS_AllocMpcMemory(t_cm_domain_id domainId, t_cm_mpc_memory_type memType, t_cm_size size,
				t_cm_memory_alignment memAlignment, t_cm_memory_handle* pHandle)
{
	int err;
	CM_AllocMpcMemory_t data;

	data.in.domainId = domainId;
	data.in.memType = memType;
	data.in.size = size;
	data.in.memAlignment = memAlignment;
	err = cm_control(CM_ALLOCMPCMEMORY, &data, __func__);
	if (err)
		return err;

	if (data.out.error == CM_OK)
		*pHandle = data.out.pHandle;
	
	return data.out.error;
}

 
/** \ingroup OS_API
 * CM_OS_GetMpcMemorySystemAddress implementation.
 * Returns logical and physical address of a previosly allocated buffer.
 *
 * -Obtains the physical address and the size of the memory area
 * -Remaps in user space the memory area
 */
t_cm_error CM_OS_GetMpcMemorySystemAddress(t_cm_memory_handle handle, t_cm_system_address *pSystemAddress)
{
	int err;
	t_uint32 size=0, physAddr=0, userLogicalAddr=0, mpcAddress, newPhysAddr, newsize, offset;
	int pagesize;

	/* Get the memory area descriptor */
	err = CM_OS_GetMpcMemoryDesc(handle, &size, &physAddr, &userLogicalAddr, &mpcAddress);
	if (err != CM_OK)
		return err;
	
	pagesize = getpagesize();
	/* If userLogicalAddr is not NULL we have already remapped the memory area before */
	if (userLogicalAddr) {
		/* See below: the kernel works which pages but we hide this to the user.
		   Thus, every userLogicalAdd returned by the kernel (page-aligned)
		   must be fixed with an offset to match the physical address */
		userLogicalAddr += (physAddr % pagesize);
		pSystemAddress->logical = userLogicalAddr;
		pSystemAddress->physical = physAddr;
		return CM_OK;
	}
	
	/* The Linux Kernel works on pages of fixed size, which is the unit of all allocation.
	   Thus mmap() only works with page-aligned addresses, and failed otherwise.
	   This constraint is too hard, due to the low amount of TCM memory.
	   In order to not propagate this constraint to NMF, we tranparently mmap() more space
	   than requested, by rounding down the SystemAddress, to have it aligned to a physical page.
	   We mmap() this aligned address, and increase size accordingly. The address returned
	   below is the one matching the requested physical address.
	   This may be unsecure as it explicitely exposes some non requested memory to the user, which
	   may write to memory initialy allocated to another process.
	   This must be reconsider and fix later, by adding some *copy* API, for example.
	*/
	offset = physAddr % pagesize;
	newPhysAddr = physAddr - offset;
	newsize = size + offset;

	/* Ask the driver to remap the buffer in user space */
	/* As we can't pass the real physical address via mmap (non-page-aligned address)
	   we first *explicitely* reserve *this* piece of memory for us. 
	   (because several small pieces of memory might have be allocated
	   on the same physical page) */
	if (ioctl(control_fd, CM_PRIVRESERVEMEMORY, physAddr) != 0) {
		ALOGE("%s: failed to reserve memory area %08X (%s)\n",
		     __func__, (int)physAddr, strerror(errno));
		return CM_INTEGRATION_ERROR;
	}
	userLogicalAddr = (t_cm_logical_address)mmap(NULL, newsize, PROT_READ|PROT_WRITE, MAP_SHARED,
						     control_fd, newPhysAddr);
	if (userLogicalAddr == (t_cm_logical_address)MAP_FAILED) {
		ALOGE("%s: mmap failed for memory area %08X (real=%p), size %d (%s)\n",
		     __func__, (int)newPhysAddr, (void*)physAddr,
		     (int)newsize, strerror(errno));
		err = CM_NO_MORE_MEMORY;
	}
	
	pSystemAddress->logical = userLogicalAddr + offset;
	pSystemAddress->physical = physAddr;
	
	return err;
}

/** \ingroup OS_API
 */
t_cm_error CM_OS_GetMpcMemoryMpcAddress(t_cm_memory_handle handle, t_uint32 *pMpcAddress)
{
	int err;
	t_uint32 size, physAddr, userLogicalAddr, mpcAddress=0;

	/* Get the memory area descriptor */
	err = CM_OS_GetMpcMemoryDesc(handle, &size, &physAddr, &userLogicalAddr, &mpcAddress);
	if (err != CM_OK)
		return err;
	
	*pMpcAddress = mpcAddress;
	
	return err;
}

/** \ingroup OS_API
 * CM_OS_FreeMpcMemory implementation
 * -Obtain size and user logical address of the memory area
 * -Unmap the user space mapping for the memory area
 * -Release the memory area
 */
t_cm_error CM_OS_FreeMpcMemory(t_cm_memory_handle handle)
{
	int err;
	t_uint32 size=0, physAddr=0, logicalAddr=0, dspAddr;
	CM_FreeMpcMemory_t data;

	/* Get the memory area descriptor */
	err = CM_OS_GetMpcMemoryDesc(handle, &size, &physAddr, &logicalAddr, &dspAddr);
	if (err != CM_OK)
		return err;

	/* Unmap ther region */
	/* See commemts in CM_OS_GetMpcMemorySystemAddress
	 * The size returned by CM_OS_GetMpcMemoryDesc is the size of allocated
	 * MPC memory, but we map more in userspace, as we must map a page-aligned region.
	 * We MUST unmap the same amount of memory, thus real mapped size is recomputed here
	 */
	if (logicalAddr)
		err = munmap((void*)logicalAddr, size + physAddr%getpagesize());

	if (err != 0)
		ALOGE("%s: failed unmapping memory region: %s\n",
		     __func__, strerror(errno));

	/* Finally, release the memory region in the driver */
	data.in.handle = handle;
	err = cm_control(CM_FREEMPCMEMORY, &data, __func__);
	if (err)
		return err;

	return data.out.error;
}


/** \ingroup OS_API
 */
t_cm_error CM_OS_GetMpcMemoryStatus(t_nmf_core_id coreId,
				    t_cm_mpc_memory_type memType,
				    t_cm_allocator_status *pStatus)
{
	int err;
	CM_GetMpcMemoryStatus_t data;

	data.in.coreId = coreId;
	data.in.memType = memType ;
	err = cm_control(CM_GETMPCMEMORYSTATUS, &data, __func__);
	if (err)
		return err;

	*pStatus = data.out.pStatus;

	return data.out.error;
}

/** \ingroup OS_API
 */
t_cm_error CM_OS_GetComponentListHeader(t_cm_instance_handle *headerComponent)
{
	int err;
	CM_GetComponentListHeader_t data;

	err = cm_control(CM_GETCOMPONENTLISTHEADER, &data, __func__);
	if (err)
		return err;

	if (data.out.error == CM_OK)
		*headerComponent = data.out.headerComponent;

	return data.out.error;
}

/** \ingroup OS_API
 */
t_cm_error CM_OS_GetComponentListNext(const t_cm_instance_handle prevComponent, t_cm_instance_handle *nextComponent)
{
	int err;
	CM_GetComponentListNext_t data;

	data.in.prevComponent = prevComponent;
	err = cm_control(CM_GETCOMPONENTLISTNEXT, &data, __func__);
	if (err)
		return err;

	if (data.out.error == CM_OK)
		*nextComponent = data.out.nextComponent;

	return data.out.error;
}

/** \ingroup OS_API
 */
t_cm_error CM_OS_GetComponentDescription(const t_cm_instance_handle  component, char *templateName,
					      t_uint32 templateNameLength, t_nmf_core_id *coreId, char *localName, t_uint32 localNameLength,
	                                      t_nmf_ee_priority *priority)
{
	int err;
	CM_GetComponentDescription_t data;

	data.in.component = component;
	data.in.templateNameLength = templateNameLength;
	data.in.localNameLength = localNameLength;
	data.in.templateName = templateName;
	data.in.localName = localName;
	err = cm_control(CM_GETCOMPONENTDESCRIPTION, &data, __func__);
	if (err)
		return err;
 
	if (data.out.error == CM_OK) {
		*coreId = data.out.coreId;
		if (priority)
			*priority = data.out.priority;
	}
	return data.out.error;
}

/** \ingroup OS_API
 */
t_cm_error CM_OS_GetComponentRequiredInterfaceNumber(const t_cm_instance_handle component,
	t_uint8 *numberRequiredInterfaces)
{
	int err;
	CM_GetComponentRequiredInterfaceNumber_t data;

	data.in.component = component;
	err = cm_control(CM_GETCOMPONENTREQUIREDINTERFACENUMBER, &data, __func__);
	if (err)
		return err;
 
	if (data.out.error == CM_OK)
		*numberRequiredInterfaces = data.out.numberRequiredInterfaces;

	return data.out.error;
}

/** \ingroup OS_API
 */
t_cm_error CM_OS_GetComponentRequiredInterface(const t_cm_instance_handle component,
						    const t_uint8 index, char *itfName, t_uint32 itfNameLength,
						    char *itfType, t_uint32 itfTypeLength,
						    t_cm_require_state *requireState, t_sint16 *collectionSize)
{
	int err;
	CM_GetComponentRequiredInterface_t data;

	data.in.component = component;
	data.in.index = index;
	data.in.itfNameLength = itfNameLength;
	data.in.itfTypeLength = itfTypeLength;
	data.in.itfName = itfName;
	data.in.itfType = itfType;
	err = cm_control(CM_GETCOMPONENTREQUIREDINTERFACE, &data, __func__);
	if (err)
		return err;
 
	if (data.out.error == CM_OK) {
		if(requireState != NULL)
			*requireState = data.out.requireState;
		*collectionSize = data.out.collectionSize;
	}

	return data.out.error;
}

/** \ingroup OS_API
 */
t_cm_error CM_OS_GetComponentRequiredInterfaceBinding(const t_cm_instance_handle component, const char *itfName,
	t_cm_instance_handle *server, char *serverItfName, t_uint32 serverItfNameLength)
{
	int err;
	CM_GetComponentRequiredInterfaceBinding_t data;

	data.in.component = component;
	data.in.itfName = itfName;
	data.in.serverItfNameLength = serverItfNameLength;
	data.in.serverItfName = serverItfName;
	err = cm_control(CM_GETCOMPONENTREQUIREDINTERFACEBINDING, &data, __func__);
	if (err)
		return err;

	if (data.out.error == CM_OK)
		*server = data.out.server;

	return data.out.error;
}

/** \ingroup OS_API
 */
t_cm_error CM_OS_GetComponentProvidedInterfaceNumber(const t_cm_instance_handle component, t_uint8 *numberProvidedInterfaces)
{
	int err;
	CM_GetComponentProvidedInterfaceNumber_t data;

	data.in.component = component;
	err = cm_control(CM_GETCOMPONENTPROVIDEDINTERFACENUMBER, &data, __func__);
	if (err)
		return err;

	if (data.out.error == CM_OK)
		*numberProvidedInterfaces = data.out.numberProvidedInterfaces;

	return data.out.error;
}

/** \ingroup OS_API
 */
t_cm_error CM_OS_GetComponentProvidedInterface(const t_cm_instance_handle component, const t_uint8 index,
	char *itfName, t_uint32 itfNameLength, char *itfType,t_uint32 itfTypeLength, t_sint16 *collectionSize)
{
	int err;
	CM_GetComponentProvidedInterface_t data;

	data.in.component = component;
	data.in.index = index;
	data.in.itfNameLength = itfNameLength;
	data.in.itfTypeLength = itfTypeLength;
	data.in.itfName = itfName;
	data.in.itfType = itfType;
	err = cm_control(CM_GETCOMPONENTPROVIDEDINTERFACE, &data, __func__);
	if (err)
		return err;
 
	if (data.out.error == CM_OK)
		*collectionSize = data.out.collectionSize;

	return data.out.error;
}

/** \ingroup OS_API
 */
t_cm_error CM_OS_GetComponentPropertyNumber(const t_cm_instance_handle component,t_uint8 *numberProperties)
{
	int err;
	CM_GetComponentPropertyNumber_t data;

	data.in.component = component;
	err = cm_control(CM_GETCOMPONENTPROPERTYNUMBER, &data, __func__);
	if (err)
		return err;

	if (data.out.error == CM_OK)
		*numberProperties = data.out.numberProperties;

	return data.out.error;
}

/** \ingroup OS_API
 */
t_cm_error CM_OS_GetComponentPropertyName(const t_cm_instance_handle component, const t_uint8 index, char *propertyName,
	t_uint32 propertyNameLength)
{
	int err;
	CM_GetComponentPropertyName_t data;

	data.in.component = component;
	data.in.index = index;
	data.in.propertyNameLength = propertyNameLength;
	data.in.propertyName = propertyName;
	err = cm_control(CM_GETCOMPONENTPROPERTYNAME, &data, __func__);
	if (err)
		return err;

	return data.out.error;
}

/** \ingroup OS_API
 */
t_cm_error CM_OS_GetComponentPropertyValue(const t_cm_instance_handle component, const char *propertyName,
	char *propertyValue, t_uint32 propertyValueLength)
{
	int err;
	CM_GetComponentPropertyValue_t data;

	data.in.component = component;
	data.in.propertyName = propertyName;
	data.in.propertyValue = propertyValue;
	data.in.propertyValueLength =propertyValueLength ;
	err = cm_control(CM_GETCOMPONENTPROPERTYVALUE, &data, __func__);
	if (err)
		return err;

	return data.out.error;
}

/** \ingroup OS_API
 */
t_cm_error CM_OS_ReadComponentAttribute(const t_cm_instance_handle component, const char* attrName, t_uint32 *value)
{
	int err;
	CM_ReadComponentAttribute_t data;

	data.in.component = component;
	data.in.attrName = attrName;
	err = cm_control(CM_READCOMPONENTATTRIBUTE, &data, __func__);
	if (err)
		return err;

	if (data.out.error == CM_OK && value)
		*value = data.out.value;

	return data.out.error;
}

t_cm_error CM_OS_WriteComponentAttribute(const t_cm_instance_handle component, const char* attrName, t_uint32 value)
{
    int err;
    CM_WriteComponentAttribute_t data;

    data.in.component = component;
    data.in.attrName = attrName;
    data.in.value=value;

    err = cm_control(CM_WRITECOMPONENTATTRIBUTE, &data, __func__);
    if (err)
        return err;

    return data.out.error;
}


t_cm_error CM_OS_GetExecutiveEngineHandle(t_cm_domain_id domainId,
					  t_cm_instance_handle *executiveEngineHandle)
 {
 	int err;
	CM_GetExecutiveEngineHandle_t data;

	data.in.domainId = domainId;
	err = cm_control(CM_GETEXECUTIVEENGINEHANDLE, &data, __func__);
	if (err)
		return err;

	if (data.out.error == CM_OK && executiveEngineHandle)
		*executiveEngineHandle = data.out.executiveEngineHandle;

	return data.out.error;
}


/** \ingroup OS_PRIVAPI
 * New syscall. Fetches memory area descriptor
 * 
 * \handle CM handle of the memory area.
 * \size Size of the memory area
 * \physAddr Physical address of the memory area
 * \logicalAddr Logical address of the memory area (may be NULL if not remapped)
 * \dspAddr Physical address as seen by MPC
 */
static inline t_cm_error CM_OS_GetMpcMemoryDesc(t_cm_memory_handle handle, t_uint32* size, t_uint32* physAddr, t_uint32* logicalAddr, t_uint32* dspAddr)
{
	int err;
	CM_PrivGetMPCMemoryDesc_t data;

	data.in.handle = handle;
	err = cm_control(CM_PRIVGETMPCMEMORYDESC, &data, __func__);
	if (err)
		return err;

	if (data.out.error == CM_OK) {
		*size = data.out.size;
		*physAddr = data.out.physAddr;
		*logicalAddr = data.out.userLogicalAddr;
		*dspAddr = data.out.mpcPhysAddr;
	}
	
	return data.out.error;
}

t_cm_error CM_OS_SetMode(t_cm_cmd_id aCmdID, t_sint32 aParam)
{
	int err;
	CM_SetMode_t data;

	data.in.aCmdID = aCmdID;
	data.in.aParam = aParam;
	err = cm_control(CM_SETMODE, &data, __func__);
	if (err)
		return err;

	return data.out.error;
}

t_cm_error CM_OS_GetRequiredComponentFiles(
        t_action_to_do action,
	const t_cm_instance_handle client,
	const char *requiredItfClientName,
	const t_cm_instance_handle server,
	const char *providedItfServerName,
	char fileList[][MAX_INTERFACE_TYPE_NAME_LENGTH],
	t_uint32 listSize,
	char *type,
	t_uint32 *methodNumber) {
	int err;
	CM_GetRequiredComponentFiles_t data;

	data.in.action = action;
	data.in.client = client;
	data.in.requiredItfClientName = requiredItfClientName;
	data.in.server = server;
	data.in.providedItfServerName = providedItfServerName;
	data.in.fileList = (char **)fileList;
	data.in.listSize = listSize;
	data.in.type = type;
	err = cm_control(CM_GETREQUIREDCOMPONENTFILES, &data, __func__);
	if (err)
		return err;
	if (data.out.error == CM_OK) {
		if(methodNumber)
			*methodNumber = data.out.methodNumber;
	}

	return data.out.error;
}

t_cm_error CM_OS_PushComponent(const char *name, const void *d, t_cm_size size) {
	int err;
	CM_PushComponent_t data;

	data.in.name = name;
	data.in.data = d;
	data.in.size = size;
	err = cm_control(CM_PUSHCOMPONENT, &data, __func__);
	if (err)
		return err;

	return data.out.error;
}

t_cm_error CM_OS_ReleaseComponent(const char *name) {
	int err;
	CM_ReleaseComponent_t data;

	data.in.name = name;
	err = cm_control(CM_RELEASECOMPONENT, &data, __func__);
	if (err)
		return err;

	return data.out.error;
}

t_cm_error CM_OS_Migrate(const t_cm_domain_id srcShared, const t_cm_domain_id src, const t_cm_domain_id dst)
{
	int err;
	CM_Migrate_t data;

	data.in.srcShared = srcShared;
	data.in.src = src;
	data.in.dst = dst;

	err = cm_control(CM_MIGRATE, &data, __func__);
	if (err)
		return err;

	return data.out.error;
}

t_cm_error CM_OS_Unmigrate(void)
{
	int err;
	CM_Unmigrate_t data;

	err = cm_control(CM_UNMIGRATE, &data, __func__);
	if (err)
		return err;

	return data.out.error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_SetupRelinkAreaMem2Per(
	t_cm_memory_handle mem_handle,
	unsigned int peripheral_addr,
	unsigned int segments,
	unsigned int segmentsize,
	unsigned int LOS)
{
	int err;
	CM_SetupRelinkArea_t data;

	data.in.mem_handle      = mem_handle;
	data.in.peripheral_addr = peripheral_addr;
	data.in.segments        = segments;
	data.in.segmentsize     = segmentsize;
	data.in.LOS             = LOS;
	data.in.type            = CMDMA_MEM_2_PER;

	err = cm_control(CM_SETUPRELINKAREA, &data, __func__);
	if (err)
		return err;

	return data.out.error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_SetupRelinkAreaPer2Mem(
	t_cm_memory_handle mem_handle,
	unsigned int peripheral_addr,
	unsigned int segments,
	unsigned int segmentsize,
	unsigned int LOS)
{
	int err;
	CM_SetupRelinkArea_t data;

	data.in.mem_handle      = mem_handle;
	data.in.peripheral_addr = peripheral_addr;
	data.in.segments        = segments;
	data.in.segmentsize     = segmentsize;
	data.in.LOS             = LOS;
	data.in.type            = CMDMA_PER_2_MEM;

	err = cm_control(CM_SETUPRELINKAREA, &data, __func__);
	if (err)
		return err;

	return data.out.error;
}

/*!
 * Pre-load all NMF components according to configuration files found
 * in repository:
 */
static void load_components(void)
{
	DIR *dir;
	struct dirent *dirent;
	char cwd[PATH_MAX];

	if (ioctl(control_fd, CM_PRIV_ISCOMPONENTCACHEEMPTY) != 0)
		return;

	ALOGI("Pre-load NMF components\n");

	getcwd(cwd, sizeof(cwd));
	if  (board == U8500_V2) {
		if (chdir(REPOSITORY_DIR "mmdsp_8500_v2/")) {
			ALOGE("NMF: chdir(" REPOSITORY_DIR "mmdsp_8500_v2) "
			     "failed: %s\n",  strerror(errno));
			return;
		}
	} else if (board == U9540_V1) {
		if (chdir(REPOSITORY_DIR "mmdsp_9540_v1/")) {
			ALOGE("NMF: chdir(" REPOSITORY_DIR "mmdsp_9540_v1) "
			     "failed: %s\n",  strerror(errno));
			return;
		}
	} else {
		ALOGE("Unknown board version: no component pre_loaded\n");
		return;
	}

	dir = opendir(".");
	if (!dir) {
		ALOGE("Failed to open repository directory: %s. "
		     "No component pre_loaded\n", strerror(errno));
		chdir(cwd);
		return;
	}

	while ((dirent = readdir(dir))) {
		int len = strlen(dirent->d_name);

		if (!strncmp(dirent->d_name, "preload_", 8)
		    && !strcmp(&dirent->d_name[len-4], ".txt")) {
			FILE *file = fopen (dirent->d_name, "r" );
			if (file != NULL) {
				t_nmf_error err;
				char line[MAX_TEMPLATE_NAME_LENGTH*2];

				while (fgets(line, sizeof line, file) != NULL ) {
					char *ptr = line, *comp_name;

					/* skip heading white space */
					while ((*ptr == ' ') || (*ptr == '\t'))
						ptr++;

					if (*ptr == '#')
						continue;
					if (*ptr == '\n')
						continue;
					comp_name = ptr;

					while ((! isspace(*ptr)) && (*ptr != '\0'))
						ptr++;
					*ptr = '\0';
					err = CM_PushComponent(comp_name);
					if (err)
						ALOGE("Pre-load of %s failed with "
						     "error %d\n", comp_name, err);
					else
						ALOGI("Component %s preloaded\n",
						     comp_name);
						
				}
				fclose(file);
			} else {
				ALOGE("Can not open file %s: %s\n",
				     dirent->d_name, strerror(errno));
			}
		}
	}
	closedir(dir);
	chdir(cwd);
}

static void cm_init_once(void)
{
	cm_debugfs_init();
	if (cm_has_debugfs())
		ioctl(control_fd, CM_PRIV_DEBUGFS_READY);
	load_components();
}
