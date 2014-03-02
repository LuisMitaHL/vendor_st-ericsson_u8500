/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#ifndef COPS_API_INTERNAL_H
#define COPS_API_INTERNAL_H

#include <cops_state.h>
#ifndef COPS_ENABLE_FEATURE_STUBBED_SECURE_WORLD
#include <tee_client_api.h>
#endif

struct cops_context_id {
    struct cops_state *state;
};

typedef uint32_t cops_api_msg_t;

#endif       /* COPS_API_INTERNAL_H */
