/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef _R_MEMMORY_UTILS_H_
#define _R_MEMMORY_UTILS_H_

/**
 * @addtogroup ldr_utilities
 * @{
 *   @addtogroup memory_utils
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdlib.h>
#include "error_codes.h"
#include "t_basicdefinitions.h"
#ifdef CFG_ENABLE_LOADER_TYPE
#include "cpu_support.h"

/*******************************************************************************
 * Macro that release allocated memory space in heap.
 ******************************************************************************/
#define BUFFER_FREE(Buffer) \
if (NULL != (Buffer)) \
{ \
  CPU_Irq_State_t IRQ_status; \
  \
  IRQ_status = CPU_IrqDisable();\
  free(Buffer); \
  (Buffer) = NULL; \
  \
  if (IRQ_status == CPU_IRQ_ENABLE)\
  {\
    CPU_IrqEnable();\
  }\
}
#else //CFG_ENABLE_LOADER_TYPE
#define BUFFER_FREE(Buffer) \
if (NULL != (Buffer)) \
{ \
  free(Buffer); \
  (Buffer) = NULL; \
}
#endif // CFG_ENABLE_LOADER_TYPE

/**
 * Macro that check alignment. If variable is not aligned it set the status
 * variable ReturnValue to appropriate error code and send the function to error
 * label.
 *
 * @param [in] x           Variable for alignment check.
 *
 */
#define IS_ALIGNED(x)  if(((x) & 3)) \
                       { \
                         ReturnValue = E_UNALIGNED_DATA; \
                         goto ErrorExit; \
                       } \
 
/** @} */
/** @} */
#endif /*_R_MEMMORY_UTILS_H_*/
