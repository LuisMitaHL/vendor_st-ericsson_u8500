/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef FDMON_H_
#define FDMON_H_

/**
 * @brief Callback function used by the file descriptor monitor
 * @param fd        File descriptor
 * @param context   Context for callback
 * @return          Result code from callback
 *
 */
typedef int fdmon_callback_t(const int fd, const void *context);

/**
 * The fdmon_init() function initialises file descriptor monitoring
 *
 * @brief Initialises file descriptor monitoring.
 *
 * @return Returns an int.
 * @retval 0 if successful
 * @retval -1 if not.
 */
int fdmon_init(void);

/**
 * The fdmon_shutdown() function shuts down file descriptor monitoring
 *
 * @brief Shuts down file descriptor monitoring.
 *
 * @return Returns an int.
 * @retval 0 if successful
 * @retval -1 if not.
 */
int fdmon_shutdown(void);

/**
 * The fdmon_add() function adds a file descriptor to monitor
 *
 * @brief Adds a file descriptor to monitor.
 *
 * @param fd        The file descriptor.
 * @param context   Context that will be supplied in callback
 * @param cb_event  Callback function that will be called when
 *                  an event is detected on the file descriptor.
 * @param cb_remove Callback function that will be called when the
 *                  event is removed from monitoring following an
 *                  an error or if the call to cb_event returns a
 *                  negative value.
 *
 * @return Returns an int.
 * @retval 0 if successful
 * @retval -1 if not.
 */
int fdmon_add(const int fd, const void *context, fdmon_callback_t *cb_event, fdmon_callback_t *cb_remove);

/**
 * The fdmon_remove() function removes a file descriptor from monitoring
 *
 * @brief Removes a file descriptor from monitoring.
 *
 * @param fd        The file descriptor.
 *
 * @return Returns an int.
 * @retval 0 if successful
 * @retval -1 if not.
 */
int fdmon_del(int fd);

/**
 * The fdmon_waitevent() function waits for an event on a monitored file descriptor
 *
 * @brief Wait for an event on a monitored file descriptor
 *
 * @param timeout   Timeout in milliseconds, <0-indefinite wait, =0-no wait, >0-milliseconds to wait
 *
 * @return Returns an int.
 * @retval 0 if successful
 * @retval -1 if not.
 */
int fdmon_waitevent(const long timeout);

/**
 * The fdmon_stopwait() function stops waiting for an event on a monitored file descriptor
 *
 * @brief Initialises file descriptor monitoring.
 *
 * @return Returns an int.
 * @retval 0 if successful
 * @retval -1 if not.
 */
int fdmon_stopwait(void);

#endif /* FDMON_H_ */
