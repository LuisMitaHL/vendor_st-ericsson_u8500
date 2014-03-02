/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*----------------------------------------------------------------------------------*/
/* Name: cn_client_stub.c                                                                 */
/* SIM stub implementation file for SIM Library                                     */
/* version:         0.1                                                             */
/*----------------------------------------------------------------------------------*/

#include <stdio.h>

#include "r_smslinuxporting.h"

#include "cn_client.h"

//####################################################################################
//Mock for CN
//####################################################################################
/* Briefly how it works:
 *
 * When the CTAD_CN client initializes (cn_client_init called) two pipes are setup, one for
 * request/responses and another for events. The client (CATD) then request for the response
 * and event fds to listen on (the pipes[OUT])and adds them to the reader thread.
 *
 * When a cn_request_xxx function is called the cn_message_type_t (request) is written to the
 * request/response pipe. The request written to the pipe[IN] are directly available at the
 * opposite end pipe[OUT], when this happens the client calls cn_message_receive to get the actual
 * cn_message that we also create here.
 *
 */
#define OUT              0
#define IN               1

#define CN_CONTEXT 456

int fd_cn_response_pipe[2] = { -1, -1 };
int fd_cn_event_pipe[2] = { -1, -1 };

typedef struct {
    cn_message_type_t type;
    cn_client_tag_t   tag;
} cn_message_data_t;

//**************** Private helper functions ************

cn_message_t *create_cn_message(cn_message_type_t type, cn_error_code_t error_code, cn_client_tag_t client_tag, cn_uint32_t payload_size, void *payload_p)
{
    cn_message_t *message_p = NULL;
    int msg_size = 0;

    msg_size = sizeof(*message_p) + payload_size;
    message_p = calloc(1, msg_size);

    if (!message_p) {
        printf("calloc failed for message_p\n");
        goto error;
    }

    message_p->type         = (cn_uint32_t)type;
    message_p->client_tag   = client_tag;
    message_p->error_code   = error_code;
    message_p->payload_size = payload_size;

    if (payload_size && payload_p) {
        memcpy(message_p->payload, payload_p, payload_size);
    }

    return message_p;

error:
    return NULL;
}

cn_error_code_t send_cn_request(cn_message_type_t type, cn_context_t *context_p, cn_client_tag_t client_tag)
{
    int n;
    cn_message_data_t data = {type, client_tag};

    printf("cn_client_stub : cn: ENTER %s 0x%X\n", __func__, type);

    if (!context_p) {
        printf("cn_client_stub : cn: ERROR %s context null \n", __func__);
        return CN_FAILURE;
    }

    n = write(fd_cn_response_pipe[IN], &data, sizeof(data));

    if (n != sizeof(data)) {
        printf("cn_client_stub : cn: ERROR %s could not write to fd=%d n=%d \n", __func__, fd_cn_response_pipe[IN], n);
        return CN_FAILURE;
    }

    printf("cn_client_stub : cn: EXIT %s \n", __func__);
    return CN_SUCCESS;
}

cn_error_code_t send_cn_event(cn_message_type_t type, cn_context_t *context_p, cn_client_tag_t client_tag)
{
    int n;
    printf("cn_client_stub : cn: ENTER %s 0x%X\n", __func__, type);
    cn_message_data_t data = {type, client_tag};

    if (!context_p) {
        printf("cn_client_stub : cn: ERROR %s context null \n", __func__);
        return CN_FAILURE;
    }
    n = write(fd_cn_event_pipe[IN], &data, sizeof(data));

    if (n != sizeof(data)) {
        printf("cn_client_stub : cn: ERROR %s could not write to fd=%d n=%d \n", __func__, fd_cn_response_pipe[IN], n);
        return CN_FAILURE;
    }

    printf("cn_client_stub : cn: EXIT %s \n", __func__);
    return CN_SUCCESS;
}

cn_error_code_t generate_cn_event(cn_message_type_t type, cn_context_t *context_p)
{
    int n;
    cn_message_data_t data = {type, 0};

    printf("cn_client_stub : cn: ENTER %s 0x%X\n", __func__, type);

    if (!context_p) {
        printf("cn_client_stub : cn: ERROR %s context null \n", __func__);
        return CN_FAILURE;
    }
    n = write(fd_cn_event_pipe[IN], &data, sizeof(data));

    if (n != sizeof(data)) {
        printf("cn_client_stub : cn: ERROR %s could not write to fd=%d n=%d \n", __func__, fd_cn_event_pipe[IN], n);
        return CN_FAILURE;
    }

    printf("cn_client_stub : cn: EXIT %s \n", __func__);
    return CN_SUCCESS;
}

//**************** Stub functions from cn_client.h ************

cn_error_code_t cn_client_init(cn_context_t **context_pp) {
    int rc1 = -1, rc2 = -1;

    if (!context_pp) {
        return CN_FAILURE;
    }

    if ((fd_cn_response_pipe[IN] == -1 && fd_cn_response_pipe[OUT] == -1)
            && (fd_cn_event_pipe[IN] == -1 && fd_cn_event_pipe[OUT] == -1)) {

        rc1 = pipe(fd_cn_response_pipe);
        rc2 = pipe(fd_cn_event_pipe);

        if (!rc1 && !rc2) {
            printf("cn_client_stub : cn: %s - response pipe in: %d out %d\n", __func__,
                    fd_cn_response_pipe[IN], fd_cn_response_pipe[OUT]);

            printf("cn_client_stub : cn: %s - event pipe in: %d out %d\n", __func__,
                    fd_cn_event_pipe[IN], fd_cn_event_pipe[OUT]);

            *context_pp = (cn_context_t*) CN_CONTEXT;
            return CN_SUCCESS;
        }
    }

    if (!rc1) {
        close(fd_cn_response_pipe[OUT]);
        close(fd_cn_response_pipe[IN]);
    }

    if (!rc2) {
        close(fd_cn_event_pipe[OUT]);
        close(fd_cn_event_pipe[IN]);
    }

    fd_cn_response_pipe[IN] = -1;
    fd_cn_response_pipe[OUT] = -1;
    fd_cn_event_pipe[IN] = -1;
    fd_cn_event_pipe[OUT] = -1;

    *context_pp = (cn_context_t*) CN_CONTEXT;
    return CN_FAILURE;
}

cn_error_code_t cn_client_get_request_fd(cn_context_t *context_p, int *fd_p) {
    if (fd_p && fd_cn_response_pipe[OUT] != -1) {
        *fd_p = fd_cn_response_pipe[OUT];
        return CN_SUCCESS;
    } else {
        printf("sim_sub : cn: %s failed\n", __func__);
        return CN_FAILURE;
    }
}

cn_error_code_t cn_client_get_event_fd(cn_context_t *context_p, int *fd_p) {
    if (fd_p && fd_cn_event_pipe[OUT] != -1) {
        *fd_p = fd_cn_event_pipe[OUT];
        return CN_SUCCESS;
    } else {
        printf("sim_sub : cn: %s failed\n", __func__);
        return CN_FAILURE;
    }
}

cn_error_code_t cn_client_shutdown(cn_context_t *context_p) {
    if (!context_p) {
        printf("cn_client_stub : cn: context_p is null.");
        return CN_FAILURE;
    }

    printf("cn_client_stub : cn: %s - response pipe in: %d out %d\n", __func__,
            fd_cn_response_pipe[IN], fd_cn_response_pipe[OUT]);

    printf("cn_client_stub : cn: %s - event pipe in: %d out %d\n", __func__,
            fd_cn_event_pipe[IN], fd_cn_event_pipe[OUT]);

    if (fd_cn_response_pipe[OUT] != -1) {
        close(fd_cn_response_pipe[OUT]);
        fd_cn_response_pipe[OUT] = -1;
    }

    if (fd_cn_event_pipe[OUT] != -1) {
        close(fd_cn_event_pipe[OUT]);
        fd_cn_event_pipe[OUT] = -1;
    }

    if (fd_cn_response_pipe[IN] != -1) {
        close(fd_cn_response_pipe[IN]);
        fd_cn_response_pipe[IN] = -1;
    }

    if (fd_cn_event_pipe[IN] != -1) {
        close(fd_cn_event_pipe[IN]);
        fd_cn_event_pipe[IN] = -1;
    }

    return CN_SUCCESS;
}

cn_error_code_t cn_message_receive(int fd, cn_uint32_t *size_p, cn_message_t **msg_pp) {
    int n;
    cn_message_data_t data   = {0, 0};
    uint32_t          count  = 0;
    uint8_t*          buffer = (uint8_t*)&data;
    cn_error_code_t   result  = CN_SUCCESS;

    if (!msg_pp) {
        goto error;
    }

    if (!size_p) {
        goto error;
    }

    if (fd == fd_cn_response_pipe[OUT] || fd == fd_cn_event_pipe[OUT]) {

        while (count < sizeof(data))
        {
            n = read(fd, (buffer + count), (sizeof(data) - count));
            if (n > 0) {
                count += n;
            } else {
                break;
            }
        }

        if (count != sizeof(data)) {
            goto error;
        }

        printf("cn_client_stub : cn: %s - received msg 0x%X client_tag 0x%X\n", __func__, data.type, data.tag);

        switch (data.type) {
            case CN_REQUEST_CELL_INFO:
            {
                cn_cell_info_t cell_info;

                cell_info.rat = CN_NMR_RAT_TYPE_UTRAN;
                cell_info.service_status = CN_NW_SERVICE;
                cell_info.current_ac = 0x0040;
                cell_info.current_cell_id = 0x04550069;

                (void)strncpy(cell_info.mcc_mnc, (const char *)"24001",CN_MAX_STRING_SIZE);

                *msg_pp = create_cn_message(CN_RESPONSE_CELL_INFO, CN_SUCCESS, data.tag, sizeof(cell_info), &cell_info);
            }
            break;

            default: {
                result = CN_FAILURE;
                break;
            }
        }
    } else {
        goto error;
    }

    *size_p = 0;
    return result;

error:
    printf("sim_sub : cn: %s error\n", __func__);
    return CN_FAILURE;
}

cn_error_code_t cn_request_cell_info(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    return send_cn_request(CN_REQUEST_CELL_INFO, context_p, client_tag);
}


//################################ END CN MOCK
