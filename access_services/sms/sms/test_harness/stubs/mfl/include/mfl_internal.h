/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef INCLUSION_GUARD_RSWBP_INTERNAL
#define INCLUSION_GUARD_RSWBP_INTERNAL 1

#include "t_os.h"
#include "t_basicdefinitions.h"
#include "g_serviceregister.h"

#define COMPILE_TIME_ASSERT(pred)            \
            switch(0){case 0:case pred:;}

/**
 * @def ASSERT_EXACT_SIZE used to assert size when compiling
 *
 */
#define ASSERT_EXACT_SIZE(type, size)        \
            COMPILE_TIME_ASSERT(sizeof(type) == size)

/**
 * @def ASSERT_MATCHING_SIZE used to assert matching size off two types
 *
 */
#define ASSERT_MATCHING_SIZE(type, type_2)    \
            COMPILE_TIME_ASSERT(sizeof(type) == sizeof(type_2))

typedef struct {
    PROCESS p_id;               //Only valid in the direction App-to-Acc
    uint16 signal_size;
} signal_header_t;

#define SERVICE_REGISTER_RESERVED_PID 0

/**
 * The mfl_signal_alloc() function.
 *
 * Allocate a signal to be used in the OSE request code. It will allocate a signal of given size.
 *
 * The given given primitive will be added to the signal. Space for ClientTag is added.
 *
 * @brief Allocate a signal
 *
 * @param [in] size The size of the pay load of the signal.
 * @param [in] primitive The primitive denotes the OSE signal id
 *
 * @retval A signal
 * @retval NULL No signal returned. Check errno for error codes. Does not correspond to the OSE behavior. cracha isdlöhflasjkfkljas
 *
 *
 * @see mfl_signal_free()
 *
 */
void *mfl_signal_alloc(
    const size_t size,
    const SIGSELECT primitive);

/**
 * The mfl_signal_send() function.
 *
 * Send a signal to a specific process id. The process resides in the modem.
 * Implementation of  SEND() macro used by OSE requests.
 *
 * @brief Send a signal to a process in the modem
 *
 * @param [in] signal_ptr_ptr The signal data to send
 * @param [out] process_id The process to send the signal to
 *
 *
 */
void mfl_signal_send(
    void **signal_ptr_ptr,
    PROCESS process_id);

/**
 * The mfl_signal_get_p_id() function.
 *
 * Gets the process id from the given signal.
 * Implementation of SENDER() macro used by OSE requests.
 *
 * @brief Get the process id
 *
 * @param [in] signal_ptr_ptr The signal data to send
 *
 * @retval Process id
 *
 */
PROCESS mfl_signal_get_p_id(
    void **signal_ptr_ptr);

/**
 * The mfl_resolve_process_id() function.
 *
 * Resolve a given process name to the dynamic process id.
 *
 * @brief Resolve a process name to a process id
 *
 * @param [in] name_ptr The process name to resolve
 * @param [out] process_id_ptr The resolved process id
 *
 * @retval Service register result value
 *
 */
SR_Result_t mfl_resolve_process_id(
    const char *const name_ptr,
    PROCESS * process_id_ptr);

/**
 * The mfl_internal_client_tag_set() function.
 *
 * Set the client tag in the given signal.
 * Implementation of Do_ClientTag_Set() macro used by OSE requests.
 *
 * @brief Set client tag
 *
 * @param [in] sig_struct_ptr Signal to set the client tag in
 * @param [out] client_tad_new
 *
 * @retval Client tag status value
 *
 */
ClientTagStatus_t mfl_internal_client_tag_set(
    void *const sig_struct_ptr,
    const ClientTag_t client_tag_new);

#endif
