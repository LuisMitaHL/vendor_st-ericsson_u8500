/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * File name       : simd_timer.h
 * Description     : simd timer framework interface
 *
 * Author          : Leif Simmons <leif.simmons@stericsson.com>
 *
 */

#ifndef __simd_timer_h__
#define __simd_timer_h__ (1)
#include <stdint.h>

/*
 * All functions in this interface are thread-safe
 *
 *
 * Typical use-case in CATd:
 *
 * void my_callback(uintptr_t timeout_event) {
 *    uintptr_t data0, data1, data2, data3;
 *    int rc = simd_timer_get_data_from_event(timeout_event, &data0, &data1, &data2, &data3);
 *    if (!rc) {
 *        // Got valid event, task took longer than 5 secs...
 *    }
 * }
 *
 * uint32_t timer;
 * int rc;
 * rc = simd_timer_create(&timer);
 * rc = simd_timer_start(timer, 5000, catd_sig_timeout_event,
 *                       (uintptr_t)my_callback, 42, 31337, 0xCAFEBABE);
 *
 * ... Do some lengthy task
 *
 * rc = simd_timer_stop(timer);
 */


/**
 * Max number of timers that CAN be supported.
 *
 * The actual number depends on the size argument when initialized
 * and how many the system supports.
 *
 * DO NOT MAKE THIS NUMBER LARGER! It may break the implementation.
 */
#define SIMD_TIMER_MAX_TIMERS (0x0FFF)

/**
 * Type definition for the call-back function to associate with time-outs.
 */
typedef void (simd_timer_callback)(uintptr_t timeout_event);


/**
 * Initialize the timer functionality.
 *
 * This funciton should be called only once.
 *
 * @param size Max number of simultaneous timers that will be supported.
 *
 * @return Zero on success, no-zero on failure.
 */
int simd_timer_initiate_timers(uint16_t size);

/**
 * Shuts down the timer functionality and de-allocates all associated memory
 *
 * Any outstanding timers or pending timer events are canceled, e.g. associated
 * call-back function will never be called and it is no longer possible to use any
 * functions in this API except simd_timer_initiate_timers().
 *
 * @return Zero on success, no-zero on failure.
 */
int simd_timer_shutdown_timers();

/**
 * Create a timer for future use.
 *
 * The timer will be idle and must be started using simd_timer_start().
 * To free the timer resource, use simd_timer_destroy().
 *
 * @param [out] handle A handle to be used when refering to this timer instance.
 *
 * @return Zero on success, non-zero on failure.
 */
int simd_timer_create(uint32_t* handle);

/**
 * Free a timer.
 *
 * Freeing a timer means that the timer resource is available for some other client.
 * There is only a limited number of timers supported, so timer that are not used
 * should be free'd.
 *
 * If the timer has been started, then it will be stopped. Any pending timer events
 * will be canceled, e.g. any associated call-back function will never be called and
 * it will no longer be valid to use the handle or timeout events stemming from this
 * timer in any of the API functions.
 *
 * @param handle A handle to the timer to free.
 */
int simd_timer_destroy(uint32_t handle);

/**
 * Starts a timer.
 *
 * The associated call-back function will be invoked when the timer has expired.
 *
 * If an already started timer is re-started, then the previous start will be canceled.
 *
 * @param handle        The handle to the timer to start.
 * @param timeout       The timeout in milliseconds.
 * @param callback      The call-back function to invoke when the timer expires.
 * @param userdata_0    Optional user data that be fetched later using the timeout event.
 * @param userdata_1    Optional user data that be fetched later using the timeout event.
 * @param userdata_2    Optional user data that be fetched later using the timeout event.
 * @param userdata_3    Optional user data that be fetched later using the timeout event.
 */
int simd_timer_start(uint32_t handle, uint32_t timeout, simd_timer_callback* callback,
                     uintptr_t userdata_0, uintptr_t userdata_1,
                     uintptr_t userdata_2, uintptr_t userdata_3);




int simd_timer_stop(uint32_t handle);

/**
 * Validates a timeout event.
 *
 * Validating a timeout event means to check that that the timeout event stems from a
 * timer that is still valid and has not been stopped or destroyed.
 *
 * In some race-scenarios, a timeout event may be propagated after the timer was stopped or destroyed.
 *
 * The associated call-back function will never be called with an 'invalid' timeout event, but
 * typically implementations will propagate the timeout event to the some other thread before it
 * is dealt with and such case this function is useful.
 *
 * @param  timeout_event The timeout event to validate.
 *
 * @return Zero if the timeout event is valid, non-zero otherwise.
 */
int simd_timer_validate_timeout_event(uintptr_t timeout_event);

/**
 * Gets the associated user data.
 *
 * This function returns the user data that was provided when the timer was started,
 * It uses the timeout event as a key to look find the data.
 *
 * Note: This function validates the timeout event, so there is no need to call
 *       simd_timer_validate_timeout_event() first.
 *
 * Note: It is valid to pass any, some, or all of the userdata_N pointers as NULL.
 *
 * @param       timeout_event   The timeout event to validate.
 * @param [out] userdata_0      User data associated with the timer event at timer start
 * @param [out] userdata_1      User data associated with the timer event at timer start
 * @param [out] userdata_2      User data associated with the timer event at timer start
 * @param [out] userdata_3      User data associated with the timer event at timer start
 *
 * @return Zero if the timeout event is valid (and thus, userdata is valid), non-zero otherwise.
 */
int simd_timer_get_data_from_event(uintptr_t timeout_event,
                                   uintptr_t* userdata_0, uintptr_t* userdata_1,
                                   uintptr_t* userdata_2, uintptr_t* userdata_3);

#endif
