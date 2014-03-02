/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/*!
 * \brief Public Component Manager Memory API.
 *
 * This file contains the Component Manager API for manipulating memory.
 */
/*!
 * \defgroup MEMORY CM Memories API
 * \ingroup CM_USER_API
 *
 */

#ifndef COMMON_MEMORY_WRAPPER_H
#define COMMON_MEMORY_WRAPPER_H

#include <cm/engine/memory/inc/memory_type.h>

/*!
 * \brief Allocate memory in a Media Processor Core memory
 *
 * The memory is allocated for a given MPC, into a given addressable memory segment and for a given size and alignment.\n
 *
 * To obtain memory, this method can be used like this: \code
 * t_cm_memory_handle block;
 * t_cm_domain_id domainId
 * t_cm_system_address blockAddress;
 * char* blockPtr;
 *
 * CM_AllocMpcMemory(domainId, CM_MM_MPC_ESRAM16, size, CM_MM_ALIGN_WORD, &block);
 * CM_GetMpcMemorySystemAddress(block, &blockAddress);
 * blockPtr = (char*)blockAddress.logical;
 *
 * ...
 *
 * CM_FreeMpcMemory(block);
 * \endcode
 *
 * \param[in] domainId Domain identifier.
 * \param[in] memType One of the addressable (by the given MPC) memory
 * \param[in] preferredLocationArray Specify in which preferred banks we shall allocate in priority (array => ordered list)
 * \param[in] size Minimum memory block size (in memType word size).
 * \param[in] memAlignment Required alignment for the allocated memory block.
 * \param[out] pHandle Address to put the memory handle of the allocated memory block.
 *
 * \exception CM_NO_MORE_MEMORY
 * \exception CM_INVALID_PARAMETER
 * \exception CM_OK
 *
 * \return exception number.
 *
 *
 * \ingroup MEMORY
 */
PUBLIC IMPORT_SHARED t_cm_error CM_AllocMpcMemory(
        t_cm_domain_id domainId,
        t_cm_mpc_memory_type memType,
        t_cm_size size,
        t_cm_mpc_memory_alignment memAlignment,
        t_cm_memory_handle *pHandle
        );


/*!
 * \brief Free a MPC memory block.
 *
 * Free a memory block previously allocated by \ref CM_AllocMpcMemory.
 *
 * See \ref CM_AllocMpcMemory for how using this method.
 *
 * \param[in] handle Handle of a memory block
 *
 * \exception CM_UNKNOWN_MEMORY_HANDLE
 * \exception CM_OK
 *
 * \return exception number.
 *
 * \ingroup MEMORY
 */
PUBLIC IMPORT_SHARED t_cm_error CM_FreeMpcMemory( t_cm_memory_handle handle);

/*!
 * \brief Get the start address of the MPC memory block seen by the host CPU (physical and logical)
 *
 * \param[in] handle Handle of a memory block
 * \param[out] pSystemAddress Returned host system address
 *
 * \exception CM_UNKNOWN_MEMORY_HANDLE
 * \exception CM_OK
 *
 * \return exception number.
 *
 * \ingroup MEMORY
 */
PUBLIC IMPORT_SHARED t_cm_error CM_GetMpcMemorySystemAddress(t_cm_memory_handle handle, t_cm_system_address *pSystemAddress);

/*!
 * \brief Get the start address of the memory block seen by the Media Processor Core
 *
 * \param[in] handle Handle of a memory block
 * \param[out] pMpcAddress Returned mpc address
 *
 * \exception CM_UNKNOWN_MEMORY_HANDLE
 * \exception CM_OK
 *
 * \return exception number.
 *
 * \ingroup MEMORY
 */
PUBLIC IMPORT_SHARED t_cm_error CM_GetMpcMemoryMpcAddress(t_cm_memory_handle handle, t_uint32 *pMpcAddress);

/*!
 * \brief Get the memory status for given memory type of a given Media Processor Core
 *
 * \param[in] domainId domain identifier.
 * \param[in] memType One of the addressable (by the given MPC) memory
 * \param[out] pStatus memory status
 *
 * \exception CM_UNKNOWN_MEMORY_HANDLE
 * \exception CM_INVALID_DOMAIN
 * \exception CM_OK
 *
 * \return exception number.
 *
 * \ingroup MEMORY
 */
PUBLIC IMPORT_SHARED t_cm_error CM_GetMpcMemoryStatus(t_cm_domain_id domainId, t_cm_mpc_memory_type memType, t_cm_allocator_status *pStatus);

/*!
 * \brief Get the memory status for given memory type of a given Media Processor Core
 *
 * \param[in] coreId core identifier.
 * \param[in] memType One of the addressable (by the given MPC) memory
 * \param[out] pStatus memory status
 *
 * \exception CM_UNKNOWN_MEMORY_HANDLE
 * \exception CM_INVALID_DOMAIN
 * \exception CM_OK
 *
 * \return exception number.
 *
 * \ingroup MEMORY
 */
PUBLIC IMPORT_SHARED t_cm_error CM_GetMpcMemoryStatusEx(t_nmf_core_id coreId, t_cm_mpc_memory_type memType, t_cm_allocator_status *pStatus);
#endif /* COMMON_MEMORY_WRAPPER_H */

