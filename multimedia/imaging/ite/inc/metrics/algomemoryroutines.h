/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Defines
 */


#ifndef __INCLUDE_ALGO_MEMORY_ROUTINES_H__
#define __INCLUDE_ALGO_MEMORY_ROUTINES_H__

#ifndef WIN32
#include <los/api/los_api.h>
#endif
/*
 * Includes 
 */
#include "algotypedefs.h"


#ifdef __cplusplus
extern "C"
{
#endif   

/**
 * Function to start profiling memory usage.
 *
 * @param aAddress        None.
 *
 * @return                [TAny] None.
 */
TAny start_memory_profiler(void);

/**
 * Function to profile memory usage in the allocation.
 *
 * @param aAddress        [TUint32] Address of the pointer.
 * @param aSize           [TUint32] Number of bytes allocated.
 *
 * @return                [TAny] None.
 */
TAny insert_mem_array(TUint32 aAddress, TUint32 aSize);

/**
 * Function to profile memory usage in the deallocation.
 *
 * @param aAddress        [TUint32] Address of the pointer.
 *
 * @return                [TAny] None.
 */
TAny delete_mem_array(TUint32 aAddress);

/**
 * Function to show the memory profiling result.
 *
 * @param aAddress        None.
 *
 * @return                [TAny] None.
 */
TAny show_memory_result(void);

/**
 * Wrapper for malloc() function
 *
 * @param aValue        [TUint32] Number of bytes to be allocated.
 *
 * @return              [TAny*] TAny pointer to the memory.
 */
TAny* AlgoMalloc( TUint32 aValue );

/**
 * Wrapper for free() function
 *
 * @param aPointer      [TAny*] Pointer to the memory to be deallocated.
 *
 * @return              [TAny] None
 */
TAny AlgoFree( TAny* aPointer );

/**
 * Wrapper for memcpy() function
 *
 * @param aDestination      [TAny*] Pointer to the destination array where the content is to be copied, type-casted to a pointer of type TAny*. 
 * @param aSource           [TAny*] Pointer to the source of data to be copied, type-casted to a pointer of type TAny*. 
 * @param aSize             [TUint32] Number of bytes to copy. 
 *
 * @return                  [TAny*] destination is returned.
 */
TAny* AlgoMemcpy ( TAny* aDestination, const TAny* aSource, TUint32 aSize );

/**
 * Wrapper for LOS_GetLogicalAddress() function
 * for Nomadik target, this function is defined in imaging validation environment
 * @param aValue        [TUint32] Memory handle.
 *
 * @return              [TUint32] physical address of memory handle.
 */
#ifdef WIN32
TUint32 LOS_GetLogicalAddress( TUint32 aValue );
 
#endif

#ifdef __cplusplus
}
#endif 


#endif	//__INCLUDE_ALGO_MEMORY_ROUTINES_H__

