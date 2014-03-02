/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef CN_TIMER_H_
#define CN_TIMER_H_


typedef struct {
    void *data_p;
    int size_of_data;
} cn_timer_callback_data_t;

typedef int (*cn_timer_callback_t)(void *);


/**
 * The cn_timer_init() function initialises Call & Network timer
 *
 * @brief Initialises Call & Network timer
 *
 * @return Returns an int.
 * @retval 0 if successful
 * @retval -1 if not
 */
int cn_timer_init(void);

/**
 * The cn_timer_start() function starts a timer and register callback function to be called when timer will expire
 *
 * @brief Starts a timer and register callback function to be called when timer will expire
 *
 * @param timeout   Timeout in ms
 * @param timer_callback   Callback function which shoud be called when timer will expire
 * @param callback_data_p   Pointer to data needed in timer_callback function
 *
 * @return Returns an int.
 * @retval 0 if successful
 * @retval -1 if not
 */
int cn_timer_start(int timeout, cn_timer_callback_t timer_callback, cn_timer_callback_data_t *callback_data_p);

/**
 * The cn_timer_stop() function stops a timer and deregister callback function
 *
 * @brief Stop a timer and deregister callback function
 *
 * @param timer_callback   Callback function which shoud be called when timer will expire
 *
 * @return Returns an int.
 * @retval 0 if successful
 * @retval -1 if not
 */
int cn_timer_stop(cn_timer_callback_t timer_callback);

#endif /* CN_TIMER_H_ */
