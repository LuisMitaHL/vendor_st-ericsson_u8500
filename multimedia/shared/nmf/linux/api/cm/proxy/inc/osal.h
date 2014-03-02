/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/*!
 * \brief OSAL CM Proxy OSAL (Operating System Abstraction Layer) API
 *
 * This file contains the OSAL API for CM proxy that must be implemented by integrator for each
 * user or kernel application that access CM API. This CM API was provided through a CM user or kernel proxy.
 *
 * \defgroup CM_PROXY_OSAL_API CM Proxy OSAL (Operating System Abstraction Layer) API
 * \ingroup CM_PROXY_MODULE
 */
#ifndef CM_COMMON_OSAL_H_
#define CM_COMMON_OSAL_H_

#include <cm/inc/cm_type.h>

/*!
 * \brief Identifier of file open by OSAL
 * \ingroup CM_PROXY_OSAL_API
 */
typedef t_uint32* t_nmf_osal_fs_file_handle;

/*!
 * \brief Identifier of lock create by OSAL
 * \ingroup CM_PROXY_OSAL_API
 */
typedef t_uint32 t_nmf_osal_sync_handle;

/*!
 * \brief Global User Proxy lock declaration
 */
extern t_nmf_osal_sync_handle cmProxyLock;

/*!
 * \brief Memory Allocation routine (libc malloc-like).
 *
 * \param size minium size in byte to be allocated
 *
 * \ingroup CM_PROXY_OSAL_API
 */
PUBLIC void* OSAL_Alloc(t_cm_size size);

/*!
 * \brief Memory Free routine (libc free-like).
 *
 * \param pHandle previously allocated chunk handle
 *
 * \ingroup CM_PROXY_OSAL_API
 */
PUBLIC void  OSAL_Free(void *pHandle);

/*!
 * \brief Log routine (libc free-like).
 *
 * \param format sprintf string limited to integer value and 3 parameters
 * \param param1
 * \param param2
 * \param param3
 *
 * \ingroup CM_PROXY_OSAL_API
 */
PUBLIC void  OSAL_Log(const char *format, void* param1, void* param2, void* param3);

/* FILESYSTEM */

/*!
 * \brief Description of the File System part of the OS Adaptation Layer
 *
 * <B>Goal:</B> Load dynamically the component file from the component repository.
 * Here, it loads the file and read the data into a buffer
 *
 * Called (indirectly) by:
 * - CM_RepositoryInit() call once at initialization of the client
 * - CM_InstantiateComponent() call (only for the first user component instantiation and when loading the EE on a given Media Processor)
 * - CM_BindComponentxxx() call (only for the first binding component instantiation)
 *
 * \param[in] fileName: (const char *) path of the file to be checked
 * \param[out] buffer: (void **) pointer to the data read (content of the file)
 * \param[out] pSize: (t_cm_size *) pointer to the size of the data read
 * \param[in] userspace: (t_bool) boolean to specify whether the handler/pointer returned will be used in userspace or kernelspace
 *
 * \return file handle on the file opened or Null in case of error
 *
 * \ingroup CM_PROXY_OSAL_API
 */
PUBLIC t_nmf_osal_fs_file_handle OSAL_ReadFile(const char *fileName, void **buffer, t_cm_size *pSize, t_bool userspace);

/*!
 * \brief Description of the File System part of the OS Adaptation Layer
 *
 * <B>Goal:</B> Load dynamically the component file from the component repository.
 * Here, it closes the file
 *
 * Called (indirectly through cm_pushComponent()) by:
 * - CM_Init() call once at initialization of the client
 * - CM_InstantiateComponent() call (only for the first user component instantiation and when loading the EE on a given Media Processor)
 * - CM_BindComponentxxx() call (only for the first binding component instantiation)
 *
 * \param[in] handle: (t_nmf_osal_fs_file_handle) handle of the file returned when OSAL_ReadFile is performed
 *
 * \ingroup CM_PROXY_OSAL_API
 */
PUBLIC void OSAL_CloseFile(t_nmf_osal_fs_file_handle handle);

/*!
 * \brief Description of the Synchronization part of the OS Adaptation Layer
 *
 * <B>Goal:</B> Use synchronize multiple call and access to some shared variables
 *              in a multithreaded context.
 *
 * \return handle of the Mutex created
 *
 * Called by:
 *  - any CM API call
 *
 * \ingroup CM_PROXY_OSAL_API
 */
PUBLIC t_nmf_osal_sync_handle OSAL_CreateLock(void);

/*!
 * \brief Description of the Synchronization part of the OS Adaptation Layer
 *
 * <B>Goal:</B> Get the ownership of the given lock
 *
 * \param[in] handle handle of the Mutex to be locked
 *
 * Called by:
 *  - any CM API call
 *
 * \ingroup CM_PROXY_OSAL_API
 */
PUBLIC void OSAL_Lock(
        t_nmf_osal_sync_handle handle);

/*!
 * \brief Description of the Synchronization part of the OS Adaptation Layer
 *
 * <B>Goal:</B> Release the lock (caller must be the one get the ownership of the lock)
 *
 * \param[in] handle handle of the Mutex to be unlocked
 *
 * Called by:
 *  - any CM API call
 *
 * \ingroup CM_PROXY_OSAL_API
 */
PUBLIC void OSAL_Unlock(
        t_nmf_osal_sync_handle handle);

/*!
 * \brief Description of the Synchronization part of the OS Adaptation Layer
 *
 * <B>Goal:</B> Destroy a lock. It can't be used any more after this.
 *
 * \param[in] handle handle of the Mutex to be destroyed
 *
 * Called by:
 *  - any CM API call
 *
 * \ingroup CM_PROXY_OSAL_API
 */
PUBLIC void OSAL_DestroyLock(
        t_nmf_osal_sync_handle handle);

#ifdef CM_API_TIMING
/*!
 * The following are used in debugging mode only to trace the timing of each CM calls.
 * Return the current time in microsecond
 */
PUBLIC unsigned long long OSAL_GetTime(void);

/*!
 * The following table is used to register the timing in each CM call.
 * The index used for each CM API is hard-coded (not fun, I know).
 * Here are the index currenlty used:
 *
 * 0  CM_InstantiateComponent
 * 1  CM_StartComponent
 * 2  CM_StopComponent
 * 3  CM_DestroyComponent
 * 4  CM_BindComponent
 * 5  CM_BindComponentTrace
 * 6  CM_UnbindComponent
 * 7  CM_BindComponentToVoid
 * 8  CM_BindComponentAsynchronous
 * 9  CM_UnbindComponentAsynchronous
 * 10 CM_AllocMpcMemory
 * 11 CM_FreeMpcMemory
 * 12 CM_GetMpcMemorySystemAddress
 * 13 CM_GetMpcMemoryMpcAddress
 * 14 CM_GetMpcMemoryStatus
 * 15 CM_GetMpcMemoryStatusEx
 * 16 CM_CreateMemoryDomain
 * 17 CM_DestroyMemoryDomain
 * 18 CM_CreateMemoryDomainScratch
 * 19 CM_GetDomainCoreId
 * 20 CM_UnbindComponentFromUser
 * 21 CM_BindComponentFromUser
 * 22 CM_BindComponentFromUserEx
 * 23 CM_BindComponentToUser
 * 24 CM_UnbindComponentToUser
 * 25 _Z24CM_BindComponentFromUsermPKcmPN3NMF18InterfaceReferenceE
 * 26 _Z26CM_BindComponentFromUserExmPKcmhPN3NMF18InterfaceReferenceE
 * 27 _Z26CM_UnbindComponentFromUserPN3NMF18InterfaceReferenceE
 * 28 _Z22CM_BindComponentToUsermmPKcPvm
 * 29 _Z24CM_UnbindComponentToUsermmPKcPPv
 * 30 CM_GetComponentDescription
 * 31 CM_ReadComponentAttribute
 * 32 CM_GetComponentRequiredInterfaceNumber
 * 33 CM_GetComponentRequiredInterface
 * 34 CM_GetComponentRequiredInterfaceBinding
 * 35 CM_GetComponentProvidedInterfaceNumber
 * 36 CM_GetComponentProvidedInterface
 * 37 CM_GetComponentPropertyNumber
 * 38 CM_GetComponentPropertyName
 * 39 CM_GetComponentPropertyValue
 * 40 CM_GetComponentMemoryInfo
 * 41 CM_GetComponentListHeader
 * 42 CM_GetComponentListNext
 * 43 CM_WriteComponentAttribute
 *
 *
 *  The following API are not traced today:
 *  CM_BindComponentAsynchronousEx (traced under CM_BindComponentAsynchronous)
 *  CM_CloseChannel
 *  CM_CreateChannel
 *  CM_ExecuteMessage
 *  CM_FlushChannel
 *  CM_GetExecutiveEngineHandle
 *  CM_GetMessage
 *  CM_GetVersion
 *  CM_INTERNAL_PushEventWithSize
 *  CM_INTERNAL_RegisterHost2MpcInterfaces
 *  CM_INTERNAL_RegisterMpc2HostInterfaces
 *  CM_INTERNAL_UnregisterHost2MpcInterfaces
 *  CM_INTERNAL_UnregisterMpc2HostInterfaces
 *  CM_Migrate
 *  CM_PushComponent
 *  CM_RegisterService
 *  CM_SetMode
 *  CM_StringError
 *  CM_Unmigrate
 *  CM_UnregisterService
 *  CM_getMpcLoadCounter
 */
#define NB_CM_API 64
struct func_entry {
	const char *name;
	unsigned int time;
	unsigned int call_count;
};

extern struct func_entry func[NB_CM_API];
#endif

#endif /*CM_COMMON_OSAL_H_ */
