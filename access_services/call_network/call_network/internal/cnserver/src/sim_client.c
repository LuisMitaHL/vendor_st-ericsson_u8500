/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include "sim.h"
#include "sim_client.h"
#include "cn_pdc_internal.h"
#include "plmn_main.h"
#include "plmn_sim.h"

#include "cn_rat_control.h"

typedef struct {
    int fd;
    ste_sim_t        *ste_sim_p;
    ste_sim_closure_t ste_sim_closure;
} sim_client_t;

static sim_client_t *sim_client_p = NULL;
static cn_bool_t rat_initiated = FALSE; /* Track if RAT has been initiated from SIM. */

#define SIM_EVENT_CLIENT_TAG 0

cn_bool_t sim_client_init()
{
    int retries_left = 30;
    cn_bool_t result = FALSE;

    /* Initialize connection to the SIM process.
     *
     * Retry connection setup if it fails (to remove race condition
     * between CNS and SIM during system startup).
     */
    while (retries_left > 0) {
        if (!sim_client_open_session()) {
            retries_left--;
            CN_LOG_E("sim_client_open_session failed, retries_left=%d", retries_left);
            usleep(500000); /* 0.5 second sleep */
        } else {
            CN_LOG_D("successfully connected to SIM");
            result = TRUE;
            break;
        }
    }

    return result;
}


cn_bool_t sim_client_shutdown()
{
    /* Tear down the connection to the SIM process */
    sim_client_close_session();

    return TRUE;
}


cn_void_t sim_client_callback(int cause, uintptr_t client_tag, void *data_p, void *user_data_p)
{
    request_record_t *record_p = NULL;
    request_status_t status = 0;

    (void)user_data_p;

    switch (cause) {
    case STE_SIM_CAUSE_CONNECT: {
        int result;
        CN_LOG_D("STE_SIM_CAUSE_CONNECT");

        /* Ask SIM to start working against MAL */
        result = ste_sim_startup(sim_client_p->ste_sim_p, SIM_EVENT_CLIENT_TAG);

        if (STE_SIM_SUCCESS != result) {
            CN_LOG_E("ste_sim_startup, result %d!", result);
        }

        /* Query current SIM state */
        result = ste_uicc_sim_get_state(sim_client_p->ste_sim_p, SIM_EVENT_CLIENT_TAG);

        if (UICC_REQUEST_STATUS_OK != result) {
            CN_LOG_E("ste_uicc_sim_get_state failed, result %d!", result);
        }

        /* Register with the CAT server to get event if RAT setting changes, or file on SIM changes. */
        result = ste_cat_register(sim_client_p->ste_sim_p, SIM_EVENT_CLIENT_TAG,
                                  STE_CAT_CLIENT_REG_EVENTS_RAT_SETTING_UPDATED | STE_CAT_CLIENT_REG_EVENTS_PC_REFRESH);

        if (STE_SIM_SUCCESS != result) {
            CN_LOG_E("ste_cat_register failed, result %d!", result);
        }

        break;
    }
    case STE_SIM_CAUSE_DISCONNECT:
        CN_LOG_D("STE_SIM_CAUSE_DISCONNECT");
        break;

    case STE_UICC_CAUSE_REQ_GET_SIM_STATE:

        if (NULL != data_p) {
            ste_uicc_get_sim_state_response_t *uicc_get_sim_state_response_p = (ste_uicc_get_sim_state_response_t *)data_p;

            switch (uicc_get_sim_state_response_p->state) {
            case SIM_STATE_NOT_READY:
                CN_LOG_D("STE_UICC_CAUSE_REQ_GET_SIM_STATE -> SIM_STATE_NOT_READY");
                break;
            case SIM_STATE_PIN_NEEDED:
                CN_LOG_D("STE_UICC_CAUSE_REQ_GET_SIM_STATE -> SIM_STATE_PIN_NEEDED");
                break;
            case SIM_STATE_READY:
                CN_LOG_D("STE_UICC_CAUSE_REQ_GET_SIM_STATE -> SIM_STATE_READY");
                plmn_sim_init();
                break;
            default:
                CN_LOG_D("STE_UICC_CAUSE_REQ_GET_SIM_STATE -> state = %d", uicc_get_sim_state_response_p->state);
                break;
            }
        } else {
            CN_LOG_E("STE_UICC_CAUSE_REQ_GET_SIM_STATE, data_p == NULL!");
        }

        break;

    case STE_UICC_CAUSE_SIM_STATE_CHANGED:

        if (NULL != data_p) {
            ste_uicc_sim_state_changed_t *uicc_sim_state_changed_p = (ste_uicc_sim_state_changed_t *)data_p;

            switch (uicc_sim_state_changed_p->state) {
            case SIM_STATE_NOT_READY:
                CN_LOG_D("STE_UICC_CAUSE_SIM_STATE_CHANGED -> SIM_STATE_NOT_READY");
                break;
            case SIM_STATE_PIN_NEEDED:
                CN_LOG_D("STE_UICC_CAUSE_SIM_STATE_CHANGED -> SIM_STATE_PIN_NEEDED");

                if (!rat_initiated) {
                    if (0 == update_rat_from_sim()) {
                        rat_initiated = TRUE;
                    }
                }

                break;
            case SIM_STATE_PUK_NEEDED:
                CN_LOG_D("STE_UICC_CAUSE_SIM_STATE_CHANGED -> SIM_STATE_PUK_NEEDED");

                if (!rat_initiated) {
                    if (0 == update_rat_from_sim()) {
                        rat_initiated = TRUE;
                    }
                }

                break;
            case SIM_STATE_READY:
                CN_LOG_D("STE_UICC_CAUSE_SIM_STATE_CHANGED -> SIM_STATE_READY");

                if (!rat_initiated) {
                    if (0 == update_rat_from_sim()) {
                        rat_initiated = TRUE;
                    }
                }

                plmn_sim_init();
                break;
            case SIM_STATE_SAP:
            case SIM_STATE_SIM_ABSENT:
            case SIM_STATE_PERMANENTLY_BLOCKED:
            case SIM_STATE_DISCONNECTED_CARD:
            case SIM_STATE_REJECTED_CARD_INVALID:
            case SIM_STATE_REJECTED_CARD_SIM_LOCK:
            case SIM_STATE_REJECTED_CARD_CONSECUTIVE_6F00:
                rat_initiated = FALSE;
                break;
            default:
                CN_LOG_D("STE_UICC_CAUSE_SIM_STATE_CHANGED -> state = %d", uicc_sim_state_changed_p->state);
                break;
            }
        } else {
            CN_LOG_E("STE_UICC_CAUSE_SIM_STATE_CHANGED, data_p == NULL!");
        }

        break;

    case STE_UICC_CAUSE_SIM_STATUS:

        if (NULL != data_p) {
            CN_LOG_D("STE_UICC_CAUSE_SIM_STATUS -> reason =%d", ((sim_status_t *)data_p)->reason);
        } else {
            CN_LOG_E("STE_UICC_CAUSE_SIM_STATUS, data_p == NULL!");
        }

        break;

    case STE_CAT_CAUSE_RAT_SETTING_UPDATED:
        CN_LOG_D("STE_CAT_CAUSE_RAT_SETTING_UPDATED");
        update_rat_from_sim();
        break;

    case STE_CAT_CAUSE_PC_REFRESH_IND:          /**< Refresh Proactive command */
        CN_LOG_D("STE_CAT_CAUSE_PC_REFRESH_IND");
        plmn_sim_init();
        break;

    case STE_CAT_CAUSE_PC_REFRESH_FILE_IND: {   /**< File change by PC Refresh command */
        ste_cat_pc_refresh_file_ind_t *refresh_file_ind_p = (ste_cat_pc_refresh_file_ind_t *)data_p;
        CN_LOG_D("STE_CAT_CAUSE_PC_REFRESH_FILE_IND");

        /* Check if the changed file matches one that concerns the PLMN operator list */
        if (plmn_sim_path_match(&refresh_file_ind_p->path)) {
            plmn_sim_init();
        }

        break;
    }
    default:

        if (NULL != (void *)client_tag) {
            record_p = request_record_from_modem_tag((void *)client_tag);

            if (!record_p) {
                CN_LOG_E("record_p is NULL!");
                goto exit;
            }

            record_p->response_data_p = data_p;
            record_p->response_error_code = cause;

            status = record_p->request_handler_p(NULL, record_p);

            if (REQUEST_STATUS_PENDING != status) {
                request_record_free(record_p);
            }

        } else {
            CN_LOG_I("unhandled SIM cause:%d", cause);
        }

        break;
    }


exit:
    return;
}

/* DESCRIPTION OF CALLBACKS:
 * sim_client_select_callback() is called by the select() handler. The SIM library call ste_sim_read()
 * will read pending data in the socket buffer and invoke a message parser. When valid data is available,
 * this will result in a callback to sim_client_callback() that deals with the messages themselves.
 */
int sim_client_select_callback(const int fd, const void *data_p)
{
    sim_client_t *sim_client_p = NULL;
    int status = -1;

    PARAMETER_NOT_USED(fd);

    sim_client_p = (sim_client_t *) data_p;

    if (!sim_client_p) {
        CN_LOG_E("sim_client_p is NULL!");
        goto exit;
    }

    status = ste_sim_read(sim_client_p->ste_sim_p);

    if (status < 0) { /* Read of data from sim failed.  */
        CN_LOG_E("********************************************************");
        CN_LOG_E("*****     socket connection to sim daemon lost     *****");
        CN_LOG_E("********************************************************");
        (void)sim_client_close_session();
    }

exit:
    return TRUE;
}

cn_bool_t sim_client_open_session()
{
    int result = 0;

    sim_client_p = calloc(1, sizeof(sim_client_t));

    if (!sim_client_p) {
        CN_LOG_E("calloc failed for sim_client_p!");
        goto error;
    }

    sim_client_p->fd = -1;
    sim_client_p->ste_sim_closure.func = sim_client_callback;
    sim_client_p->ste_sim_closure.user_data = sim_client_p;

    sim_client_p->ste_sim_p = ste_sim_new_st(&(sim_client_p->ste_sim_closure));

    if (!sim_client_p->ste_sim_p) {
        CN_LOG_E("ste_sim_new_st failed!");
        goto error;
    }

    result = ste_sim_connect(sim_client_p->ste_sim_p, SIM_EVENT_CLIENT_TAG);

    if (STE_SIM_SUCCESS != result) {
        CN_LOG_E("ste_sim_connect failed, result %d!", result);
        goto error;
    }

    sim_client_p->fd = ste_sim_fd(sim_client_p->ste_sim_p);

    if (sim_client_p->fd < 0) {
        CN_LOG_E("ste_sim_fd failed, fd %d!", sim_client_p->fd);
        goto error;
    }

    result = fdmon_add(sim_client_p->fd, (void *)sim_client_p, sim_client_select_callback, sim_client_select_callback);

    if (result < 0) {
        CN_LOG_E("fdmon_add failed, result %d!", result);
        goto error;
    }

    return TRUE;

error:
    sim_client_close_session();
    return FALSE;
}

cn_void_t sim_client_close_session()
{
    int status = 0;

    if (sim_client_p) {
        status = ste_sim_disconnect(sim_client_p->ste_sim_p, SIM_EVENT_CLIENT_TAG);

        if (status < 0) {
            CN_LOG_E("ste_sim_disconnect failed!");
        }

        status = fdmon_del(sim_client_p->fd);

        if (status < 0) {
            CN_LOG_E("fdmon_del failed!");
        }

        free(sim_client_p);
        sim_client_p = NULL;
    }
}

ste_sim_t *sim_client_get_handle()
{
    if (!sim_client_p) {
        CN_LOG_E("sim_client_p is NULL!");
        return NULL;
    } else {
        return sim_client_p->ste_sim_p;
    }
}
