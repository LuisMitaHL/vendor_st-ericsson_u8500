/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _R_LOADER_SEC_LIB_PORT_FUNCS_H
#define _R_LOADER_SEC_LIB_PORT_FUNCS_H
/**
 * @addtogroup ldr_security_library
 * @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_loader_sec_lib.h"
#include "e_loader_sec_lib.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/**
 * This routine frees the allocated memory region. Loader Security Library
 * external.
 *
 * @param[in] Buf_pp  Pointer to the allocated memory region to free.
 *
 * @return see \c LoaderSecLib_Output_t for detailed description of the error
 *                codes.
 */
LoaderSecLib_Output_t LoaderSecLib_Free(uint8 **const Buf_pp);

/**
 * This routine allocates a memory region. Loader Security Library external.
 *
 * @param[in] BufLength   The length of the buffer to allocate.
 *
 * @return the allocated buffer, the function will return NULL if the allocation
 *                               failed.
 */
void *LoaderSecLib_Allocate(const uint32 BufLength);

/** @} */
#endif /*_R_LOADER_SEC_LIB_PORT_FUNCS_H*/
