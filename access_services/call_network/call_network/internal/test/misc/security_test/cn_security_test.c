/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 *  Test program for socket security.
 *
 *  Author: Johan Norberg  <johan.xx.norberg@stericsson.com>
 */

#include <stdio.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#include <cn_client.h>
#include <cn_log.h>

#define DIAL_STRING "8521039"

int main(int argc, char* argv[])
{
    int result;
    cn_context_t *context_p = NULL;
    int request_fd = -1;
    cn_dial_t dial_settings;
    cn_uint32_t size = 0;
    cn_message_t *message_p = NULL;
    cn_response_dial_t *response_dial_p = NULL;
    memset(&dial_settings, 0, sizeof(dial_settings));

    (void)argc;
    (void)argv;

    CN_LOG_D("enter");

    CN_LOG_D("setuid");
    setuid(1002);

    CN_LOG_D("cn_client_init");
    result = cn_client_init(&context_p);
    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_client_init failed!");
        goto exit;
    }

    CN_LOG_D("cn_client_get_request_fd");
    result = cn_client_get_request_fd(context_p, &request_fd);
    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_client_get_request_fd failed!");
        goto cleanup;
    }

    dial_settings.call_type = CN_CALL_TYPE_VOICE_CALL;
    dial_settings.sat_initiated = FALSE;
    memmove(dial_settings.phone_number, DIAL_STRING, sizeof(char) * (strlen(DIAL_STRING)+1));
    dial_settings.clir = 2;
    dial_settings.bc_length = 0;

    CN_LOG_D("invoking unauthorized dial request -> %s (only RIL/AT and SIM are allowed to do this)", dial_settings.phone_number);
    result = cn_request_dial(context_p, &dial_settings, 123);
    if (CN_SUCCESS != result) {
         CN_LOG_E("cn_request_dial failed!");
         goto cleanup;
    }

    /* Wait for CN_RESPONSE_DIAL. In this simple test app we don't need select(). */
    CN_LOG_D("waiting for response.");
    while (TRUE) {
        result = cn_get_message_queue_size(request_fd, &size);
        if (CN_SUCCESS != result) {
            CN_LOG_E("cn_get_message_queue_size failed!");
            goto cleanup;
        }

        if (size > 0) {
            sleep(1); /* at least one byte has been received. Ensure that all bytes are received */
            break;
        }
    }

    result = cn_message_receive(request_fd, &size, &message_p);
    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        goto cleanup;
    }

    if (CN_RESPONSE_DIAL == message_p->type) {
        CN_LOG_E("CN_RESPONSE_DIAL received!");
        goto cleanup;
    }

    response_dial_p = (cn_response_dial_t*) message_p->payload;

    if (CN_SERVICE_TYPE_AFTER_SAT_CC_VOICE_CALL == response_dial_p->service_type) {
        CN_LOG_D("CN_SERVICE_TYPE_AFTER_SAT_CC_VOICE_CALL");
        goto cleanup;
    }

    CN_LOG_D("call_id: %d", response_dial_p->data.call_id);

cleanup:
    result = cn_client_shutdown(context_p);
    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_client_shutdown failed!");
        goto exit;
    }

exit:
    CN_LOG_D("exit");
    return 0;
}
