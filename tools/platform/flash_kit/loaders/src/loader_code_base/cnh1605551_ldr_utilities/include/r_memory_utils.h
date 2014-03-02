/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _R_MEMMORY_UTILS_H_
#define _R_MEMMORY_UTILS_H_

/**
 * @addtogroup ldr_utilities
 * @{
 *   @addtogroup memmory_utils
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdlib.h>
#include "error_codes.h"
#include "t_basicdefinitions.h"

/*******************************************************************************
 * Macro that release allocated memory space in heap.
 ******************************************************************************/
#define BUFFER_FREE(Buffer) \
if (NULL != (Buffer)) \
{ \
  free(Buffer); \
  (Buffer) = NULL; \
}

/**
 * Macro that check alignment. If variable is not aligned it set the status
 * variable ReturnValue to appropriate error code and send the function to error
 * label.
 *
 * @param [in] x           Variable for alignment check.
 *
 * @sigbased No - Macro
 */
#define IS_ALIGNED(x)  if(((x) & 3)) \
                       { \
                         ReturnValue = E_UNALIGNED_DATA; \
                         goto ErrorExit; \
                       } \
 
/** @} */
/** @} */
#endif /*_R_MEMMORY_UTILS_H_*/
