/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : state_machine.h
 * Description     : State machine definitions
 *
 * Author          : Steve Critchlow <steve.critchlow@stericsson.com>
 *
 */


#ifndef __state_machine_h__
#define __state_machine_h__ (1)

#include "msgq.h"

/**
 * @brief Defines a state.
 */
typedef struct ste_sim_state_tag ste_sim_state_t;

/**
 * @brief State function to act on a message
 * @param s     The state itself
 * @param msg   Message to act on.  0 when the state is entered.
 * @param ud    User data pointer, for any additional context information.
 * @returns     Next state (may be the same) or
 *              STE_STATE_NOT_HANDLED if the message was not handled.
 */
typedef const ste_sim_state_t *(ste_sim_state_func_t) (const ste_sim_state_t * s,
                                                       ste_msg_t* msg,
                                                       void *ud);

/**
 * @brief       State function to retrieve a message.
 * @param s     The state itself
 * @param ud    User data pointer, for any additional context information.
 * @returns     Message.
 */
typedef ste_msg_t   *(ste_sim_state_get_msg_func_t) (const ste_sim_state_t* s,
                                                     void *ud);

/**
 * @brief State definition.
 */
struct ste_sim_state_tag {
    const char                   *ss_name;         /**< Name of the state */
    ste_sim_state_func_t         *ss_action_func;  /**< State Action function */
    ste_sim_state_get_msg_func_t *ss_getmsg_func;  /**< Get message function */
};

/**
 * @brief Lookup a state name
 * @param table     The state table
 * @param name      The name of the state to locate
 * @returns         A pointer to the corresponding state table entry
 */
const ste_sim_state_t *ste_sim_state_get_by_name(const ste_sim_state_t *table, const char *name);

/**
 * @brief Validates a state table
 * @param table     The state table
 * @returns         TRUE if the table seems to be valid.
 */
int ste_sim_state_validate_table(const ste_sim_state_t *table);

/**
 * @brief The state function did not handle the message
 */
#define STE_STATE_NOT_HANDLED   0

#endif
