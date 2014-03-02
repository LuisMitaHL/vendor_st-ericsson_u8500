/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef _T_QUEUE_H_
#define _T_QUEUE_H_

/**
 * @addtogroup ldr_utilities
 * @{
 *   @addtogroup queue
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

/** type of queue callback functions. */
typedef enum {
    QUEUE_EMPTY,
    QUEUE_NONEMPTY
} QueueCallbackType_e;

/**
 * Typedef of callback function used for the queue.
 * One callback function is used when the fifo is empty,
 * and another when the fifo is nonempty.
 * These function are set by calling Do_Fifo_SetCallback.
 *
 * @param [in] Queue_p pointer to a valid queue(One that is created using
 *             Do_Fifo_Create).
 * @param [in] Param_p additional parameters to the function.
 * @return     None.
 */
typedef void (*QueueCallback_fn)(const void *const Queue_p, void *Param_p);

/** @} */
/** @} */
#endif /*T_QUEUE_H_*/
