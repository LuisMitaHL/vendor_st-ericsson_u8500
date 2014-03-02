/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : state_machine.c
 * Description     : State machine implementation file
 *
 * Author          : Steve Critchlow <steve.critchlow@stericsson.com>
 *
 */
#include <string.h>
#include <assert.h>
#include "state_machine.h"

const ste_sim_state_t* ste_sim_state_get_by_name( const ste_sim_state_t *table,
                                                  const char* name )
{
    while ( table->ss_name ) {
        if ( strcmp( name, table->ss_name ) == 0 )
            return table;
        ++table;
    }
    assert(0);
    return STE_STATE_NOT_HANDLED;
}

int ste_sim_state_validate_table(const ste_sim_state_t *table)
{
    int result = 1;
    while ( table->ss_name && result == 1 ) {
        result = table->ss_action_func != NULL &&
                 table->ss_getmsg_func != NULL;
        assert(result != 0);
        ++table;
    }
    return result;
}
