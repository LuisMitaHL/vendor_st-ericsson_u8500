/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#ifndef __INC_LOS_SEM_H
#define __INC_LOS_SEM_H

/*!
 * \defgroup LOS_SEM LOS Semaphore API
 */


/*!
 * \brief Identifier of the semaphore.
 *
 * \ingroup LOS_SEM
 */
typedef unsigned int			t_los_sem_id;


/*!
 * \brief Creates a semaphore, to synchronize objects
 *
 * \param[in] value (int) value of how many times, the semaphore can be taken
 * \return Handle (id) of the created sempahore (t_los_sem_id), (t_los_sem_id)0 if the call has failed
 *
 * \ingroup LOS_SEM
 * */
IMPORT_SHARED t_los_sem_id			LOS_SemaphoreCreate( int value );

/*!
 * \brief Destroys (and releases all memory allocated for it) the given semaphore
 *
 * \param[in] hsem id of the sempahore (t_los_sem_id);
 *
 * \return 0
 *
 * \ingroup LOS_SEM
 * */
IMPORT_SHARED int					LOS_SemaphoreDestroy( t_los_sem_id hsem );

/*!
 * \brief Wait on the given semaphore. If a thread try to take the semaphore more than possible, the thread waits.
 *
 *
 * \param[in] hsem id of the sempahore (t_los_sem_id);
 *
 * \ingroup LOS_SEM
 * */
IMPORT_SHARED void                 LOS_SemaphoreWait( t_los_sem_id hsem );

/*!
 * \brief Wait on the given semaphore. If a thread try to take the semaphore more than possible, the thread waits unless
 *  timeout expired.
 *
 * \note Use LOS_SemaphoreWait to wait without timeout.
 *
 * \param[in] hsem id of the sempahore (t_los_sem_id);
 * \param[in] timeOutInMs timeout in milli seconds.
 *
 * \return 0 on success, -1 on timeout.
 *
 * \ingroup LOS_SEM
 * */
IMPORT_SHARED int                  LOS_SemaphoreWaitTimeOut( t_los_sem_id hsem, t_uint32 timeOutInMs);

/*!
 * \brief LOS_SemaphoreTryWait is a non-blocking variant of LOS_SemaphoreWait
 *
 * If the semaphore pointed to by hsem has non-zero count, the count is atomically decreased
 * and LOS_SemaphoreTryWait immediately returns 0.
 * If the semaphore count is zero, LOS_SemaphoreTryWait immediately returns -1
 *
 * \param[in] hsem pointer to the semaphore object
 *
 * \return 0 on success, -1 on failure
 *
 * \ingroup LOS_SEM
 * */
IMPORT_SHARED int 					LOS_SemaphoreTryWait	(t_los_sem_id hsem);

/*!
 * \brief Notify and give back the the given semaphore. the next thread waiting for the semaphore automatically gets it.
 *
 *
 * \param[in] hsem id of the sempahore (t_los_sem_id);
 *
 * \ingroup LOS_SEM
 * */
IMPORT_SHARED void					LOS_SemaphoreNotify( t_los_sem_id hsem );

 /*!
 * \brief LOS_SemaphoreGetCount stores in the location pointed to by sval the current count of the semaphore hsem.
 *
 * \param[in] hsem sem pointer to the semaphore object t_los_sem_id
 * \param[out] sval pointer to the counter value
 *
 * \return 0
 *
 * \ingroup LOS_SEM
 * */
IMPORT_SHARED int					LOS_SemaphoreGetCount( t_los_sem_id hsem, int * sval);

#endif
