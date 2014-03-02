/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#include <cops_tapp_common.h>
#include <cops_common.h>
#include <cops_storage.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifdef COPS_USE_CONSECUTIVE_MEMORY
#include <r_os.h>
#endif

cops_return_code_t cops_tapp_setup(cops_tapp_io_t **tapp_io)
{
    cops_return_code_t ret_code;
    cops_tapp_io_t *io;
    uint8_t  *p;
    size_t    len;

    len = sizeof(*io) + COPS_TAPP_IO_ARGS_SIZE +
          COPS_TAPP_IO_DATA_SIZE + COPS_TAPP_IO_RVS_SIZE +
          COPS_TAPP_IO_PERM_AUTH_STATE_DATA_SIZE;

#ifdef COPS_USE_CONSECUTIVE_MEMORY
    p = CONSECUTIVE_HEAP_UNSAFE_UNTYPED_ALLOC(len);
#else
    p = calloc(1, len);
#endif

    if (p == NULL) {
        /*lint -e557 Suppress unrecognized format %zu which is used in Linux */
        COPS_LOG(LOG_ERROR, "calloc(1, %zu): %s\n",
                 sizeof(*io), strerror(errno));
        /*lint +e557 */
        return COPS_RC_MEMORY_ALLOCATION_ERROR;
    }

#ifdef COPS_USE_CONSECUTIVE_MEMORY
    /* calloc(...) will set zeroes in the allocated buffer but
     * CONSECUTIVE_HEAP_UNSAFE_UNTYPED_ALLOC(...) will not, hence
     * this memset. */
    memset(p, 0, len);
#endif

    /* alignment of p is guaranteed when allocated by calloc */
    io = (void *)p;
    p += sizeof(*io);

    io->args.data = p;
    io->args.max_length = COPS_TAPP_IO_ARGS_SIZE;
    p += COPS_TAPP_IO_ARGS_SIZE;

    io->data.data = p;
    io->data.max_length = COPS_TAPP_IO_DATA_SIZE;
    p += COPS_TAPP_IO_DATA_SIZE;

    io->rvs.data = p;
    io->rvs.max_length = COPS_TAPP_IO_RVS_SIZE;
    p += COPS_TAPP_IO_RVS_SIZE;

    io->perm_auth_state_data.data = p;
    io->perm_auth_state_data.max_length =
        COPS_TAPP_IO_PERM_AUTH_STATE_DATA_SIZE;

    COPS_CHK_RC(cops_data_init(&io->args));
    COPS_CHK_RC(cops_data_init(&io->data));
    COPS_CHK_RC(cops_data_init(&io->rvs));
    COPS_CHK_RC(cops_data_init(&io->perm_auth_state_data));

    *tapp_io = io;

function_exit:
    return ret_code;
}

void cops_tapp_destroy(cops_tapp_io_t **tapp_io)
{
    if (*tapp_io == NULL) {
        return;
    }

#ifdef COPS_USE_CONSECUTIVE_MEMORY
    CONSECUTIVE_HEAP_UNSAFE_FREE(tapp_io);
#else
    free(*tapp_io);
#endif

    *tapp_io = NULL;
}

cops_return_code_t
cops_tapp_init_io(struct cops_state *state, cops_tapp_io_t *tapp_io)
{
    cops_return_code_t ret_code = COPS_RC_OK;

    if (tapp_io->data.max_length < state->data.length) {
        /*lint -e557 Suppress unrecognized format %zu which is used in Linux */
        COPS_SET_RC(COPS_RC_BUFFER_TOO_SMALL,
                    "COPS Data too large for secure world %zu, max %zu",
                    state->data.length, tapp_io->data.max_length);
        /*lint +e557 */
    } else if (tapp_io->perm_auth_state_data.max_length <
               state->perm_auth_state_data.length) {
        /*lint -e557 Suppress unrecognized format %zu which is used in Linux */
        COPS_SET_RC(COPS_RC_BUFFER_TOO_SMALL,
                    "Auth Data too large for secure world %zu, max %zu",
                    state->perm_auth_state_data.length,
                    tapp_io->perm_auth_state_data.max_length);
        /* lint +e557 */
    }

    memcpy(tapp_io->data.data, state->data.data, state->data.length);
    tapp_io->data.length = state->data.length;

    memcpy(tapp_io->perm_auth_state_data.data, state->perm_auth_state_data.data,
           state->perm_auth_state_data.length);
    tapp_io->perm_auth_state_data.length = state->perm_auth_state_data.length;

    tapp_io->args.length = 0;
    memset(tapp_io->args.data, 0, tapp_io->args.max_length);
    (void)cops_data_init(&tapp_io->args);

    tapp_io->rvs.length = 0;
    memset(tapp_io->rvs.data, 0, tapp_io->rvs.max_length);
    (void)cops_data_init(&tapp_io->rvs);

function_exit:
    return ret_code;
}

cops_return_code_t
cops_tapp_save_cops_data(struct cops_state *state, cops_tapp_io_t *tapp_io,
                         enum cops_data_type data_type)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_data_t *data;
    cops_data_t *io_data;

    if (COPS_DATA == data_type) {
        data = &state->data;
        io_data = &tapp_io->data;
    } else if (PERM_AUTH_STATE_DATA == data_type) {
        data = &state->perm_auth_state_data;
        io_data = &tapp_io->perm_auth_state_data;
    } else {
        COPS_SET_RC(COPS_RC_ARGUMENT_ERROR, "Unsupported data type\n");
    }

    if (io_data->length != data->length ||
        memcmp(io_data->data, data->data, data->length) != 0) {

        if (io_data->max_length != data->max_length) {
            COPS_SET_RC(COPS_RC_SERVICE_ERROR,
                        "Service has changed max_length of data, "
                        "discarding returned data(type:%u)\n", data_type);
        }

        memcpy(data->data, io_data->data, io_data->length);
        data->length = io_data->length;
        COPS_CHK_RC(cops_storage_write(io_data, data_type));
    }

function_exit:
    return ret_code;
}
