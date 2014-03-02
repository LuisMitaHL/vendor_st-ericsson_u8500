#ifndef _FSM_INL_
#define _FSM_INL_

#include <fsm/generic/include/FSM.h>
#include <dbc.h>

inline FSM_State FSM_getState(const FSM *this) {
    PRECONDITION(!this->dispatching);
    return this->state;
}

#endif // _FSM_INL_
