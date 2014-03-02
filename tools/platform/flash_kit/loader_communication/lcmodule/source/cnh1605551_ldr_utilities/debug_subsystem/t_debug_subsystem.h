/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef _T_DEBUG_SUBSYTEM_H_
#define _T_DEBUG_SUBSYTEM_H_

/**
 *  @addtogroup ldr_utilities
 *  @{
 *    @addtogroup debug_subsytem
 *    @{
 */

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
/** Number of messages that can be inserted in debug queue */
#define MAX_NUMBER_OF_MESSAGES (1000)

/** Type of debug output channel */
typedef enum {
    OUTPUT_CHANNEL_DEBUG_BUFFER = 0,
    OUTPUT_CHANNEL_UART0,
} Debug_Output_Channel_t;

/** @} */
/** @} */
#endif /*_T_DEBUG_SUBSYTEM_H_*/
