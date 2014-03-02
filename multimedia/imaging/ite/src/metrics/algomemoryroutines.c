/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \file     algomemoryroutines.c
* \brief    Contains Algorithm Library Memory Routines
* \author   ST Ericsson
*/

/*
 * Defines
 */

/*
 * Includes 
 */
#ifndef WIN32
#include <los/api/los_api.h>
#include <string.h>
#else
#include <stdlib.h>
#include <memory.h>
#endif

#include "algodebug.h"
#include "algomemoryroutines.h"

#define mem_array_count  50
TUint32 mem_array [mem_array_count][2];
TUint32 mem_usage_count;

/**
 * Function to start profiling memory usage.
 *
 * @param aAddress        None.
 *
 * @return                [TAny] None.
 */
TAny start_memory_profiler(void)
	{
	TUint16 i=0;

	for(i=0; i<mem_array_count; i++)
		{
		mem_array [i][0] = 0;
		mem_array [i][1] = 0;
		}

	mem_usage_count = 0;
	}

/**
 * Function to profile memory usage in the allocation.
 *
 * @param aAddress        [TUint32] Address of the pointer.
 * @param aSize           [TUint32] Number of bytes allocated.
 *
 * @return                [TAny] None.
 */
TAny insert_mem_array(TUint32 aAddress, TUint32 aSize)
	{
	TUint16 i=0;

	for(i=0; i<mem_array_count; i++)
		{
		if(mem_array[i][0] == 0)
			{
			mem_array [i][0] = aAddress;
			mem_array [i][1] = aSize;

			mem_usage_count = mem_usage_count + aSize;

			break;
			}
		}
	}

/**
 * Function to profile memory usage in the deallocation.
 *
 * @param aAddress        [TUint32] Address of the pointer.
 *
 * @return                [TAny] None.
 */
TAny delete_mem_array(TUint32 aAddress)
	{
	TUint16 i=0;

	for(i=0; i<mem_array_count; i++)
		{
		if(mem_array[i][0] == aAddress)
			{
			mem_array[i][0] = 0;
			
			mem_usage_count = mem_usage_count - mem_array[i][1];

			mem_array[i][1] = 0;

			break;
			}
		}
	}

/**
 * Function to show the memory profiling result.
 *
 * @param aAddress        None.
 *
 * @return                [TAny] None.
 */
TAny show_memory_result(void)
	{
	TUint16 i=0;

	for(i=0; i<mem_array_count; i++)
		{
		if(mem_array[i][0] != 0)
			{
			ALGO_Log_2("Address - %d   Size - %d bytes\n", mem_array[i][0], mem_array[i][1]);
			}
		}

	ALGO_Log_1("Total Memory Leaked %d bytes\n", mem_usage_count);
	}

/**
 * Wrapper for malloc() function
 *
 * @param aValue        [TUint32] Number of bytes to be allocated.
 *
 * @return              [TAny*] TAny pointer to the memory.
 */
TAny* AlgoMalloc( TUint32 aValue )
    {
#ifndef WIN32
		TAny* temp = (TAny*)LOS_Alloc(aValue, 0, LOS_MAPPING_CREATE_UNCACHED_UNBUFFERED);
#else
		TAny* temp = (TAny*)(malloc( aValue ));
#endif
	
	//memset((void*)temp, 0xCD, aValue);
	insert_mem_array((TUint32)temp, aValue);

    return temp;
    }

/**
 * Wrapper for free() function
 *
 * @param aPointer      [TAny*] Pointer to the memory to be deallocated.
 *
 * @return              [TAny] None
 */
TAny AlgoFree( TAny* aPointer )
    {
	delete_mem_array((TUint32)aPointer);

#ifndef WIN32
		LOS_Free((t_los_memory_handle)aPointer);
#else
		free(aPointer);
#endif
    }
	
/**
 * Wrapper for LOS_GetLogicalAddress() function
 * for Nomadik target, this function is defined in imaging validation environment
 * @param aValue        [TUint32] Memory handle.
 *
 * @return              [TUint32] physical address of memory handle.
 */
#ifdef WIN32
 TUint32 LOS_GetLogicalAddress( TUint32 aValue )
    {

    return aValue;
    }
#endif


/**
 * Wrapper for memcpy() function
 *
 * @param aDestination      [TAny*] Pointer to the destination array where the content is to be copied, type-casted to a pointer of type TAny*. 
 * @param aSource           [TAny*] Pointer to the source of data to be copied, type-casted to a pointer of type TAny*. 
 * @param aSize             [TUint32] Number of bytes to copy. 
 *
 * @return                  [TAny*] destination is returned.
 */
TAny* AlgoMemcpy( TAny* aDestination, const TAny* aSource, TUint32 aSize )
    {
#ifndef WIN32
		TAny* temp = memcpy(aDestination, aSource, aSize);
#else
		TAny* temp = memcpy(aDestination, aSource, aSize);
#endif
        return temp;
    }
