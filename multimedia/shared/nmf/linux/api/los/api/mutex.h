/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#ifndef __INC_LOS_MUTEX_H
#define __INC_LOS_MUTEX_H

/*!
 * \defgroup LOS_MUTEX LOS Mutexes API
 */

/*!
 * \brief Identifier of the mutex.
 *
 * \ingroup LOS_MUTEX
 */
typedef unsigned int			t_los_mutex_id;
#define LOS_BUSY                -1


/*!
 * \brief Creates a mutex, returns the handle of the mutexe if there is enough memory to create it.
 *
 * The implementation of this function is owned by the OS implementation choice.
 *
 * \return Handle (id) of the created mutex (t_los_mutex_id), (t_los_mutex_id)0 if the call has failed
 * for any reason.
 *
 * \ingroup LOS_MUTEX
 * */
IMPORT_SHARED t_los_mutex_id		LOS_MutexCreate( void );

/*!
 * \brief Destroys the mutex identified by the mutex id passed in parameter. It frees all the memory allocated for it.
 *
 * The implementation of this function is owned by the OS implementation choice.
 *
 * \param[in] mutex (t_los_mutex_id) Id of the mutex to be destroyed;
 *
 * \return Always return 0.
 *
 * \ingroup LOS_MUTEX
 * */
IMPORT_SHARED int					LOS_MutexDestroy( t_los_mutex_id mutex );

/*!
 * \brief Locks the mutex. Once the mutex is locked, noone can take it until it has been unlocked.
 *
 * The implementation of this function is owned by the OS implementation choice.
 *
 * \param[in] mutex (t_los_mutex_id) Id of the mutex to be locked;
 *
 * \ingroup LOS_MUTEX
 * */
IMPORT_SHARED void					LOS_MutexLock( t_los_mutex_id mutex );

/*!
 * \brief LOS_MutexTryLock Non blocking version of LOS_MutexLock.
 *
 * LOS_MutexTryLock behaves identically to LOS_MutexLock, except that
 * it does not block the calling thread if the mutex is already locked by another thread
 * Instead, LOS_MutexTryLock returns immediately with the error code LOS_BUSY.
 *
 * \param[in] mutex id of the mutex identifier to be locked
 *
 * \return LOS_BUSY if mutex is already locked. Otherwise it returns 0.
 *
 * \ingroup LOS_MUTEX
 * */
IMPORT_SHARED int					LOS_MutexTryLock( t_los_mutex_id mutex );

/*!
 * \brief Unlocks the mutex. Once the mutex is unlocked, anyone can take it.
 *
 * If the mutex is already free (unlocked), the function behave as if there was no call
 * The implementation of this function is owned by the OS implementation choice.
 *
 * \param[in] mutex (t_los_mutex_id) Id of the mutexe to be unlocked;
 *
 * \ingroup LOS_MUTEX
 * */
IMPORT_SHARED void					LOS_MutexUnlock( t_los_mutex_id mutex );

#endif
