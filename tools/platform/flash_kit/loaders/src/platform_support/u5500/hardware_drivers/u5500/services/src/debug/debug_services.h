/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides debug definitons
* \author  ST-Ericsson
*/
/*****************************************************************************/


#ifndef _INITDEBUG_H_
#define _INITDEBUG_H_

/* Timer used to call DBG_getPrints regularly on timer interupt */
#define TIMER1_BASE_ADDR    debugTimerAddressForInterrupt

/* Timer used to timeStamp debug messages in debug HCL */
#define TIMESTAMP_TIMER_BASE_ADDR   debugTimeStampTimerBaseAddress

#define T1LOAD                      0x000
#define T1VAL                       0x004
#define T1CTRL                      0x008
#define T1ICLR                      0x00C
#define T1RIS                       0x010
#define T1MIS                       0x014
#define T1BGLOAD                    0x018

#define TIMER1_DELAY                0x060

/* Function Prototypes */
int logMsg
    (
        unsigned long   debug_id,
        const char      *function_name,
        char            *arg_string,
        char            *arg1,
        unsigned long   arg2,
        unsigned long   arg3,
        unsigned long   arg4,
        unsigned long   arg5,
        unsigned long   arg6,
        unsigned long   arg7,
        long            exit_param
    );
#endif /* END OF File */

