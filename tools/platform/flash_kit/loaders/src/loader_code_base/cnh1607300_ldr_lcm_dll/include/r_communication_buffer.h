/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _INCLUSION_GUARD_R_COMMUNICATION_BUFFER_H_
#define _INCLUSION_GUARD_R_COMMUNICATION_BUFFER_H_
/**
 * \addtogroup ldr_communication_buffer Loader Communication Buffers
 *
 * Loader Communication Buffer Module contains functions that are
 *  needed for static buffer management. With functions from this
 *  module you can initialize/deinitialize, allocate/deallocate,
 *  and receive information for available buffers.
 *
 * @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"
#include "t_communication_buffer.h"
#include "error_codes.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/**
 *  Initialization of static buffers for communication.
 *
 * @param [in]  Object_p    Initialized buffer context.
 *
 * @retval  E_ALLOCATE_FAILED Failed to allocate memory.
 * @retval  E_SUCCESS The function completed successfully.
 */
ErrorCode_e Do_Buffer_BuffersInit(void *Object_p);

/**
 *  Allocates a buffer.
 *
 *  @param [in]  Object_p    Initialized buffer context.
 *  @param [in]  BufferSize  Size of buffer that will be reserved.
 *
 *  @return  Returns a pointer to the allocated buffer.
 *  @retval  NULL if allocation fail.
 */
void *Do_Buffer_BufferAllocate(void *Object_p, int BufferSize);

/**
 *  Deallocates the buffer.
 *
 *  @param [in] Object_p   Initialized buffer context.
 *  @param [in] Buffer_p   A pointer to data buffer.
 *  @param [in] BufferSize Size of the allocated buffer.
 *
 *  @retval  E_SUCCESS                  The function completed successfully.
 *  @retval  E_INVALID_INPUT_PARAMETERS The expected value into the function was incorrect.
 *  @retval  E_INVALID_TYPE_OF_BUFFER   Invalid type of buffer.
 */
ErrorCode_e Do_Buffer_BufferRelease(void *Object_p, void *Buffer_p, int BufferSize);

/**
 *  Search for available communication buffers.
 *
 *  @param [in] Object_p   Initialized buffer context.
 *  @param [in] BufferSize Size of the requested free buffers.
 *
 *  @return Number of specified free buffers.
 */
uint32 Do_Buffer_BuffersAvailable(void *Object_p, int BufferSize);

/**
 *  Deinitializes all the communication buffers.
 *
 *  @param [in] Object_p   Initialized buffer context.
 *
 */
void Do_Buffer_BuffersDeinit(void *Object_p);

/** @} */
#endif /* _INCLUSION_GUARD_R_COMMUNICATION_BUFFER_H_ */
