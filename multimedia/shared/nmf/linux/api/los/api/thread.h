/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#ifndef __INC_LOS_THREAD_H
#define __INC_LOS_THREAD_H
/*!
 * \defgroup LOS_THREAD LOS Threading API
 */

/*!
 * \brief Thread ID type
 *
 * \ingroup LOS_THREAD
 */
typedef unsigned int t_los_process_id;

/*!
 * \brief Scheduling priority type
 *
 * \ingroup LOS_THREAD
 */
typedef enum {
    ME_IDLE_PRIORITY                = 0,    //!<  Idle priority, the lowest possible of the thread
    LOS_USER_BACKGROUND_PRIORITY    = 1,    //!<  Minimum priority of the thread
    LOS_USER_NORMAL_PRIORITY        = 2,    //!<  Medium priority of the thread
    LOS_USER_URGENT_PRIORITY        = 3     //!<  Maximum priority of the thread
} t_los_priority;


/*!
 * \brief Get the calling thread ID
 *
 * The LOS_GetCurrentId() function shall return the thread ID of the calling thread.
 *
 * \return Handle (id) of the current thread (t_cte_process_id);
 *
 * \ingroup LOS_THREAD
 * */
IMPORT_SHARED t_los_process_id 	LOS_GetCurrentId(void);

/*!
 * \brief Thread creation
 *
 * The LOS_ThreadCreate() function shall create a new thread, with stack size
 * and priority specified as parameter.
 *
 * The thread is created executing start_routine with arg as its sole argument.
 * If the start_routine returns, the effect shall be as if there  was  an  implicit
 * call to LOS_ThreadExit().
 *
 * \param[in] start_routine function the created thread will jumped into
 * \param[in] arg (void*) parameters to pass to the routine function
 * \param[in] stacksize (int) size of the stack of the new thread
 * \param[in] priority (t_los_priority) priority value to be choosen
 * \param[in] name (const char*) name given to the created thread
 *
 * \return Handle (id) of the current thread (t_los_process_id). Null value if not created.
 *
 * \ingroup LOS_THREAD
 * */
IMPORT_SHARED t_los_process_id		LOS_ThreadCreate( void (*start_routine) (void *),
											  void* arg,
											  int stacksize,
											  t_los_priority priority,
											  const char* name );
/*!
 * \brief Thread termination
 *
 * The LOS_ThreadExit() function shall terminate the calling thread.
 * Thread termination  does not release any application visible resources, including,
 * but not limited to, mutexes and file descriptors, nor does it perform any cleanup actions.
 *
 * The LOS_ThreadExit() function does not return.
 *
 * It takes no arguments.
 *
 * \ingroup LOS_THREAD
 * */
IMPORT_SHARED void					LOS_ThreadExit(void);

/*!
 * \brief Yield the processor
 *
 * The LOS_Yield() function shall force the running thread to relinquish the processor
 * until it again becomes the head of its thread list.
 *
 * It takes no arguments.
 *
 * \ingroup LOS_THREAD
 * */
IMPORT_SHARED void                 LOS_Yield(void);

/*!
 * \brief Cause normal program termination
 *
 * The LOS_Exit() function causes normal program termination.
 *
 * The LOS_Exit() function does not return.
 *
 * \ingroup LOS_THREAD
 * */
IMPORT_SHARED void                 LOS_Exit(void);

/*!
 * \brief Thread will sleep for sleepInMs ms before being reschedule.
 *
 * The LOS_Sleep() function will remove current thread from scheduler for a peiod
 * of sleepInMs ms.
 *
 * \param[in] sleepInMs sleep duration in ms unit.
 *
 * \ingroup LOS_THREAD
 * */
IMPORT_SHARED void                 LOS_Sleep(t_uint32 sleepInMs);

#endif
