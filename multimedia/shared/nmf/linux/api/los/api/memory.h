/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#ifndef __INC_LOS_MEMORY_H
#define __INC_LOS_MEMORY_H

#include "type.h"

/*!
 * \defgroup LOS_MEMORY LOS Memory management API
 */

/*!
 * \brief Identifier of a memory handle.
 *
 * \ingroup LOS_MEMORY
 */
typedef unsigned int t_los_memory_handle;

/*!
 * \brief Physical address
 *
 * \ingroup LOS_MEMORY
 */

typedef unsigned int t_los_physical_address;

/*!
 * \brief Virtual address
 *
 * \ingroup LOS_MEMORY
 */

typedef unsigned int t_los_logical_address;



#define INVALID_LOS_MEMORY_HANDLE ((t_los_memory_handle)MASK_ALL32)


 /*!
 * \brief Memory mapping mode
 *
 * The mapping mode is a mask, and can be a combination several modes
 * (for example: <tt>LOS_MAPPING_CREATE_CACHED | LOS_MAPPING_CREATE_BUFFERED</tt>)
 *
 * \ingroup LOS_MEMORY
 */
typedef unsigned char t_los_mapping_mode;

#define LOS_MAPPING_DEFAULT                     0 //!<  Virtual Memory Mapping by default (cache WB) \ingroup LOS_MEMORY
#define LOS_MAPPING_CREATE_CACHED_WB            1 //!<  Virtual Memory Mapping: memory use cache in write back mode\ingroup LOS_MEMORY
#define LOS_MAPPING_CREATE_UNCACHED_UNBUFFERED  2 //!<  Virtual Memory Mapping: memory does not use cache and write buffer \ingroup LOS_MEMORY
#define LOS_MAPPING_CREATE_UNCACHED_BUFFERED    3 //!<  Virtual Memory Mapping: memory uses a write buffer but memory is not cache \ingroup LOS_MEMORY
#define LOS_MAPPING_CREATE_CACHED_WT            4 //!<  Virtual Memory Mapping: memory use cache in write throught mode\ingroup LOS_MEMORY



/*!
 * \brief Allocate memory.
 *
 * By default, the SDRAM memroy is cacheable and bufferable. Use LOS_MEM_DEFAULT to allocate memory in this mode.
 *
 * \param[in] sizeInBytes Size in byte of memory chunk to be allocated.
 * \param[in] alignmentInBytes alignment in byte.
 * \param[in] mode type of LOS_MappingMode_e. Mainly used to cache or uncache the memory
 *
 * \return (t_los_memory_handle *) pointer on allocated memory chunk handle
 *
 * \ingroup LOS_MEMORY
 */
IMPORT_SHARED t_los_memory_handle 		LOS_Alloc(unsigned int sizeInBytes, unsigned int alignmentInBytes, t_los_mapping_mode mode);


/*!
 * \brief Deallocate memory.
 *
 * TODO: more accurate descriptor
 *
 * \param[in] handle pointer on allocated memory chunk to be dealloated.
 *
 * \ingroup LOS_MEMORY
 */
IMPORT_SHARED void 					LOS_Free(t_los_memory_handle handle);

/*!
 * \brief Get number of allocated chunk
 *
 * TODO: more accurate descriptor
 *
 * \return Number of allocated chunk (0xFFFFFFFF if not supported)
 *
 * \ingroup LOS_MEMORY
 */
IMPORT_SHARED t_uint32                 LOS_GetAllocatedChunk(void);

/*!
 * \brief Dump on console allocated chunk
 *
 * TODO: more accurate descriptor
 *
 * \ingroup LOS_MEMORY
 */
IMPORT_SHARED void                     LOS_DumpAllocatedChunk(void);

/*!
 * \brief Get the virtual address from the nme memory handle
 *
 * TODO: more accurate descriptor
 *
 * \param[in] handle pointer on allocated memory chunk to be dealloated.
 * \return (t_cm_logical_address) virtual address of the memory handle (chunck)
 *
 * \ingroup LOS_MEMORY
 */
IMPORT_SHARED t_los_logical_address 	LOS_GetLogicalAddress(t_los_memory_handle handle);

/*!
 * \brief LOS_Remap will map given physical address to a virtual address. Memory will also be set as not
 *        cacheable and not bufferable.
 *
 * As most of the LOS_ functions, the implementation of this function is owned by the OS implementation choice.
 *
 * \param[in] address Physical address to remap.
 * \param[in] size Size of the memory to remap.
 * \param[in] mode control memory attributes when remap
 * \return (t_cm_logical_address) virtual address corresponding to the given physical address.
 *         In case of error, value 0xffffffff is return.
 *
 * \ingroup LOS_MEMORY
 * */
IMPORT_SHARED t_los_logical_address LOS_Remap(t_los_physical_address address, t_los_size size, t_los_mapping_mode mode);


#endif /* __INC_LOS_MEMORY_H */


