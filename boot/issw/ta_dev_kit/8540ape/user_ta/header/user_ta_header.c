/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: jens.wiklander@stericsson.com
 */

#include <user_ta_header.h>
#include <user_ta_header_defines.h>
#include <tee_ta_api.h>
#include <tee_arith_internal.h>
#include <utee_syscalls.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <queue.h>

struct ta_session {
    uint32_t session_id;
    void *session_ctx;
    TAILQ_ENTRY(ta_session) link;
};

static TAILQ_HEAD(ta_sessions, ta_session) ta_sessions =
    TAILQ_HEAD_INITIALIZER(ta_sessions);

static uint32_t ta_ref_count = 0;
static bool ta_dead;

extern uint8_t *ta_heap_base;

uint32_t ta_param_types;
TEE_Param ta_params[TEE_NUM_PARAMS];

static void ta_header_save_params(uint32_t param_types,
                TEE_Param params[TEE_NUM_PARAMS])
{
    ta_param_types = param_types;
    if (params != NULL)
        memcpy(ta_params, params, sizeof(ta_params));
    else
        memset(ta_params, 0, sizeof(ta_params));
}

static struct ta_session *ta_header_get_session(uint32_t session_id)
{
    struct ta_session *itr;
    TAILQ_FOREACH(itr, &ta_sessions, link) {
        if (itr->session_id == session_id)
            return itr;
    }
    return NULL;
}

static TEE_Result ta_header_add_session(uint32_t session_id)
{
    struct ta_session *itr = ta_header_get_session(session_id);
    if (itr != NULL)
        return TEE_SUCCESS;

    ta_ref_count++;

    if (ta_ref_count == 1) {
        TEE_Result res;

        malloc_add_heap(ta_heap_base, TA_DATA_SIZE);

        TEE_MathAPI_Init();

        res = TA_CreateEntryPoint();
        if (res != TEE_SUCCESS)
            return res;
    }

    itr = malloc(sizeof(struct ta_session));
    if (itr == NULL)
        return TEE_ERROR_OUT_OF_MEMORY;
    itr->session_id = session_id;
    itr->session_ctx = 0;
    TAILQ_INSERT_TAIL(&ta_sessions, itr, link);

    return TEE_SUCCESS;
}

static void ta_header_remove_session(uint32_t session_id)
{
    struct ta_session *itr;
    TAILQ_FOREACH(itr, &ta_sessions, link) {
        if (itr->session_id == session_id) {
            TAILQ_REMOVE(&ta_sessions, itr, link);
            free(itr);

            ta_ref_count--;
            if (ta_ref_count == 0) {
                TA_DestroyEntryPoint();
                ta_dead = true;
            }

            return;
        }
    }
}

static TEE_Result __attribute__((noreturn)) ta_entry_open_session(
                    uint32_t param_types, TEE_Param params[TEE_NUM_PARAMS],
                    uint32_t session_id)
{
    TEE_Result res = TEE_ERROR_BAD_STATE;
    struct ta_session *session;

    if (ta_dead)
        goto function_exit;

    res = ta_header_add_session(session_id);
    if (res != TEE_SUCCESS)
        goto function_exit;

    session = ta_header_get_session(session_id);
    if (session == NULL)
        goto function_exit;

    ta_header_save_params(param_types, params);
    res = TA_OpenSessionEntryPoint(param_types, params, &session->session_ctx);
    if (res != TEE_SUCCESS) {
        ta_header_remove_session(session_id);
        goto function_exit;
    }

function_exit:
    ta_header_save_params(0, NULL);
    utee_return(res);
    /* Not reached */
}

static void __attribute__((noreturn)) ta_entry_close_session(
    uint32_t session_id)
{
    TEE_Result res = TEE_ERROR_BAD_STATE;
    struct ta_session *session;

    if (ta_dead)
        goto function_exit;

    session = ta_header_get_session(session_id);
    if (session == NULL)
        goto function_exit;

    TA_CloseSessionEntryPoint(session->session_ctx);

    ta_header_remove_session(session_id);

function_exit:
    utee_return(res);
    /* Not reached */
}

static void __attribute__((noreturn)) ta_entry_invoke_command(
    uint32_t cmd_id, uint32_t param_types, TEE_Param params[TEE_NUM_PARAMS],
    uint32_t session_id)
{
    TEE_Result res = TEE_ERROR_BAD_STATE;
    struct ta_session *session;

    if (ta_dead)
        goto function_exit;

    session = ta_header_get_session(session_id);
    if (session == NULL)
        goto function_exit;

    ta_header_save_params(param_types, params);

    res = TA_InvokeCommandEntryPoint(session->session_ctx, cmd_id, param_types,
                                     params);

function_exit:
    ta_header_save_params(0, NULL);
    utee_return(res);
    /* Not reached */
}

/* These externs are defined in the ld link script */
extern uint32_t linker_RO_sections_size;
extern uint32_t linker_RW_sections_size;
extern uint32_t linker_res_funcs_ZI_sections_size;
extern uint32_t linker_rel_dyn_GOT;

/* Note that cmd_id is not used in a User Mode TA */
const struct user_ta_func_head user_ta_func_head[]
        __attribute__ ((section (".ta_func_head"))) = {
    { 0, (uint32_t)ta_entry_open_session },
    { 0, (uint32_t)ta_entry_close_session },
    { 0, (uint32_t)ta_entry_invoke_command },
    { TA_FLAGS, 0 /* Spare */ },
    { TA_DATA_SIZE, TA_STACK_SIZE},
};

const struct user_ta_head ta_head __attribute__ ((section (".ta_head"))) =
{
    /* UUID, unique to each TA */
    TA_UUID,
    /* Number of functions in the TA */
    sizeof(user_ta_func_head) / sizeof(struct user_ta_func_head),
    /* Section size information */
    (uint32_t)&linker_RO_sections_size,
    (uint32_t)&linker_RW_sections_size,
    (uint32_t)&linker_res_funcs_ZI_sections_size,
    (uint32_t)&linker_rel_dyn_GOT,
    /* Hash type, filled in by sign-tool */
    0
};

/* Filled in by TEE Core when loading the TA */
uint8_t *ta_heap_base __attribute__ ((section (".ta_heap_base")));

const struct user_ta_property ta_props[] = {
    { "gpd.ta.singleInstance", USER_TA_PROP_TYPE_BOOL,
            &(const bool){ (TA_FLAGS & TA_FLAG_SINGLE_INSTANCE) != 0 } },

    { "gpd.ta.multiSession", USER_TA_PROP_TYPE_BOOL,
            &(const bool){ (TA_FLAGS & TA_FLAG_MULTI_SESSION) != 0 } },

    { "gpd.ta.instanceKeepAlive", USER_TA_PROP_TYPE_BOOL,
            &(const bool){ (TA_FLAGS & TA_FLAG_INSTANCE_KEEP_ALIVE) != 0 } },

    { "gpd.ta.dataSize", USER_TA_PROP_TYPE_U32,
            &(const uint32_t){ TA_DATA_SIZE } },

    { "gpd.ta.stackSize", USER_TA_PROP_TYPE_U32,
            &(const uint32_t){ TA_STACK_SIZE } },
/*
 * Extended propietary properties, name of properties must not begin with
 * "gpd."
 */
#ifdef TA_CURRENT_TA_EXT_PROPERTIES
    TA_CURRENT_TA_EXT_PROPERTIES
#endif
    };

const size_t ta_num_props = sizeof(ta_props) / sizeof(ta_props[0]);
