/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>

#include "cn_log.h"
#include "cn_data_types.h"
#include "cn_macros.h"
#include "message_handler.h"
#include "request_handling.h"


#include "cn_battery_internal.h"


cn_bool_t cn_battery_is_low()
{
    int fd = -1;
    char text_p[CN_BATTERY_BUFFER_SIZE];
    int res = 0;
    int capacity = 0;
    cn_bool_t is_low = FALSE;

    memset(text_p, 0, CN_BATTERY_BUFFER_SIZE);

    if ((fd = open(CN_BATTERY_CAPACITY_FILE, O_RDONLY)) < 0) {
        CN_LOG_W("Cannot open battery capacity file");
        return is_low;
    }

    /* read the number of items */
    res = read(fd, text_p, (CN_BATTERY_BUFFER_SIZE - 1));

    if (res < 1) {
        CN_LOG_W("Read error: %d", res);
        goto exit;
    }

    capacity = atoi(text_p);
    CN_LOG_D("Charger capacity value = %d", capacity);

    if (capacity < CN_BATTERY_LOW_LIMIT) {
        is_low = TRUE;
    }

exit:
    close(fd);
    return is_low;
}

void cn_battery_modified()
{
    int fd = -1;
    char text_p[CN_BATTERY_BUFFER_SIZE];
    int res = 0;
    request_record_t *record_p = NULL;
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_request_set_user_activity_status_t activity_status;

    memset(&activity_status, 0, sizeof(cn_request_set_user_activity_status_t));
    memset(text_p, 0, CN_BATTERY_BUFFER_SIZE);

    if ((fd = open(CN_BATTERY_STATUS_FILE, O_RDONLY)) < 0) {
        CN_LOG_E("Cannot open battery status file");
        return;
    }

    /* read the number of items */
    res = read(fd, text_p, (CN_BATTERY_BUFFER_SIZE - 1));

    if (res < 1) {
        CN_LOG_E("Read error: %d", res);
        goto exit;
    }

    CN_LOG_D("Charger status = %s", text_p);

    record_p = request_record_create(CN_REQUEST_SET_USER_ACTIVITY_STATUS, 0, -1);

    if (!record_p) {
        CN_LOG_E("ERROR, not possible to create request record!");
        goto exit;
    }

    record_p->request_handler_p = handle_request_set_user_activity_status;

    if (strncmp(CN_BATTERY_TEXT_CHARGING, text_p, strlen(CN_BATTERY_TEXT_CHARGING)) != 0) {
        if (cn_battery_is_low()) {
            activity_status.battery_status = CN_BATTERY_STATUS_LOW;
            CN_LOG_D("Sending battery Low");
        } else {
            activity_status.battery_status = CN_BATTERY_STATUS_NORMAL;
            CN_LOG_D("Sending battery Normal");
        }
    } else {
        activity_status.battery_status = CN_BATTERY_STATUS_IN_CHARGER;
        CN_LOG_D("Sending battery Charging");
    }

    status = handle_request_set_user_activity_status(&activity_status, record_p);
    if (REQUEST_STATUS_PENDING != status) {
        request_record_free(record_p);
    }

exit:
    close(fd);
}

