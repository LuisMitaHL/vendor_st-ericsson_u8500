/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/*!
 * \brief Public Component Manager Memory User OS API.
 *
 * This file contains the Component Manager OS API for manipulating memory.
 */

#ifndef CM_MEMORY_OS_H_
#define CM_MEMORY_OS_H_

#include <cm/engine/memory/inc/memory_type.h>

PUBLIC t_cm_error CM_OS_Free(t_cm_memory_handle handle);


/*!
 * \brief Get the memory status of the standard memory allocator (CM_Alloc/CM_Free)
 *
 * \param[out] pStatus
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_GetAllocStatus(t_cm_allocator_status *pStatus);


/*!
 * \brief Allocate memory in a Media Processor Core memory
 *
 * \param[in] domainId
 * \param[in] memType
 * \param[in] size
 * \param[in] memAlignment
 * \param[out] pHandle
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_AllocMpcMemory(
        t_cm_domain_id domainId,
        t_cm_mpc_memory_type memType,
        t_cm_size size,
        t_cm_mpc_memory_alignment memAlignment,
        t_cm_memory_handle *pHandle
        );


/*!
 * \brief Free a MPC memory block.
 *
 * \param[in] handle
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_FreeMpcMemory(t_cm_memory_handle handle);

/*!
 * \brief Get the start address of the MPC memory block seen by the host CPU (physical and logical)
 *
 * The logical system address returned by this method is valid only in kernel space and the physical
 * address is accessible only from kernel space too.
 *
 * \see OSMem "OS Memory management" for seeing an integration example.
 *
 * \param[in] handle
 * \param[out] pSystemAddress
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_GetMpcMemorySystemAddress(
        t_cm_memory_handle handle,
        t_cm_system_address *pSystemAddress);

/*!
 * \brief Get the start address of the memory block seen by the Media Processor Core
 *
 * \param[in] handle
 * \param[out] pMpcAddress
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_GetMpcMemoryMpcAddress(
        t_cm_memory_handle handle,
        t_uint32 *pMpcAddress);

/*!
 * \brief Get the memory status for given memory type of a given Media Processor Core
 *
 * \param[in] domainId
 * \param[in] memType
 * \param[out] pStatus
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_GetMpcMemoryStatus(
        t_nmf_core_id coreId,
        t_cm_mpc_memory_type memType,
        t_cm_allocator_status *pStatus);

#endif /* CM_MEMORY_OS_H_ */

