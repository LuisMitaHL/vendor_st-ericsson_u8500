/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "atc_log.h"
#include "atc_selector.h"
#include "atc_selector.h"
#include "exe_internal.h"
#include "exe_sim_client.h"
#include "exe_start.h"
#include "sim.h"

#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
#include "exe_extended_sim_client.h"
#endif

typedef enum {
    EXE_SIM_STATE_UNKNOWN = 0,             /* Unknown state of the SIM card. */
    EXE_SIM_STATE_VERIFY_NEEDED,           /* SIM card requests verification. */
    EXE_SIM_STATE_READY,                   /* SIM card is ready */
} exe_sim_state_t;

struct sim_client_s {
    int                     fd_sim;
    ste_sim_t               *ste_sim_p;
    ste_sim_closure_t       ste_sim_closure;
};

/* Used as client tag when events are received from sim  */
#define EXE_SIM_EVENT_CLIENT_TAG 0

/********************************************************************
 * Private prototypes
 ********************************************************************
 */
static void sim_client_handle_unsolicited(int cause, void *data, void *user_data);
static bool sim_client_selector_callback(int fd, void *data_p);
static void sim_client_init_session(sim_client_t *sim_client);

/********************************************************************
 * Private methods
 ********************************************************************/

/*
 * Handle response for sim
 */
static void sim_client_handle_response(int cause, void *data_p, int16_t error_code, uintptr_t client_tag)
{
    /* Ordinary signal with valid client tag */
    exe_request_record_t *request_record_p;
    exe_request_t request;

    request_record_p = request_record_get_record_from_client_tag((int)client_tag);

    if (request_record_p != NULL) {
        request = request_record_get_request(request_record_p);

        if (request != NULL) {
            /* Are there any response data_p to forward to the handler function. */
            request_record_set_response_data(request_record_p, (void *)data_p);

            if (STE_UICC_CAUSE_NOT_READY == cause) {
                /* Indicates that SIM daemon is not ready to handle requests */
                ATC_LOG_E("sim_client_handle_response STE_UICC_CAUSE_NOT_READY received");
                request_record_set_resultcode(request_record_p, 0);
            } else {
                request_record_set_resultcode(request_record_p, 1);
            }

            /* Call the handler function with the request record */
            request(request_record_p);
        } else {
            ATC_LOG_E("sim_client_handle_response ERROR. Client tag data corrupt");
            /* Unsupported and unexpected signal; consume it. */
        }
    } else {
        ATC_LOG_E("sim_client_handle_response ERROR. Client tag data corrupt");
        /* Unsupported and unexpected signal; consume it. */
    }
}


static bool sim_client_selector_callback(int fd, void *data_p)
{
    int status = 0;
    sim_client_t *sim_client_p = (sim_client_t *)data_p;

    status = ste_sim_read(sim_client_p->ste_sim_p);

    if (status < 0) { /* Read of data from sim failed.  */
        ATC_LOG_E("********************************************************");
        ATC_LOG_E("***** at_core lost contact with sim daemon         *****");
        ATC_LOG_E("********************************************************");

        /* We lost the SIM server. Shut connection down and clean-up */
        sim_client_close_session(sim_client_p);
    }

    return true; /* Returning false will remove the fd from the selector. */
}


static exe_esimsr_state_t sim_reason_to_esimsr_state(sim_reason_t reason)
{
    switch (reason) {
    case SIM_REASON_STARTUP_DONE:
    case SIM_REASON_PIN_VERIFIED:
    case SIM_REASON_PIN2_VERIFIED:
        return EXE_SIM_STATE_ACTIVE;
        break;
    case SIM_REASON_PIN_NEEDED:
        return EXE_SIM_STATE_WAIT_FOR_PIN;
        break;
    case SIM_REASON_PUK_NEEDED:
    case SIM_REASON_REJECTED_CARD_SIM_LOCK:
        return EXE_SIM_STATE_BLOCKED;
        break;
    case SIM_REASON_PERMANENTLY_BLOCKED:
        return EXE_SIM_STATE_BLOCKED_FOREVER;
        break;
    case SIM_REASON_NO_CARD:
    case SIM_REASON_DISCONNECTED_CARD:
        return EXE_SIM_STATE_POWER_OFF;
        break;
    case SIM_REASON_UNKNOWN:
    case SIM_REASON_PIN2_NEEDED:
    case SIM_REASON_PUK2_NEEDED:
    case SIM_REASON_REJECTED_CARD_INVALID:
    case SIM_REASON_REJECTED_CARD_CONSECUTIVE_6F00:
    default:
        return EXE_SIM_STATE_NULL;
        break;
    }
}

void sim_client_callback(int cause, uintptr_t client_tag, void *data_p, void *user_data_p)
{
    sim_client_t *sim_client_p = (sim_client_t *)user_data_p;

    if (EXE_SIM_EVENT_CLIENT_TAG != client_tag) {
        /* Ordinary signal with valid client tag */
        sim_client_handle_response(cause, data_p, 0, client_tag);

        return;
    }

    switch (cause) {
    case STE_SIM_CAUSE_CONNECT: {
        int result;
        ATC_LOG_D("STE_SIM_CAUSE_CONNECT");

        /* Ask SIM to start working against MAL */
        result = ste_sim_startup(sim_client_p->ste_sim_p, EXE_SIM_EVENT_CLIENT_TAG);

        if (STE_SIM_SUCCESS != result) {
            ATC_LOG_E("ste_sim_startup, result %d!", result);
            goto error;
        }

        break;
    }

    case STE_SIM_CAUSE_DISCONNECT: {
        /* Disconnected, consume to prevent a print */
        ATC_LOG_D("STE_SIM_CAUSE_DISCONNECT");
        break;
    }

    case STE_SIM_CAUSE_STARTUP: {
        /* Startup complete, consume to prevent a print */
        ATC_LOG_D("STE_SIM_CAUSE_STARTUP");
        break;
    }

    case STE_CAT_CAUSE_REGISTER:
        ATC_LOG_D("STE_CAT_CAUSE_REGISTER");

        if (exe_start_get_status(EXE_START_SIM) == EXE_START_CONNECTED_WAITING) {
            /* Query current SIM state in case SIM state is already READY,
             * which means there will be no event signalling SIM_STATE_READY */
            int result = ste_uicc_sim_get_state(sim_client_p->ste_sim_p, EXE_SIM_EVENT_CLIENT_TAG);

            if (STE_SIM_SUCCESS != result) {
                ATC_LOG_E("ste_uicc_sim_get_state failed, result %d!", result);
            }
        }

        break;

    case STE_CAT_CAUSE_CAT_INFO:
        ATC_LOG_D("STE_CAT_CAUSE_CAT_INFO");

        if (NULL != data_p) {
            ste_cat_info_ind_t *cat_info_ind_p = (ste_cat_info_ind_t *)data_p;

            switch (cat_info_ind_p->info) {
            case STE_CAT_INFO_SUCCESS:
                ATC_LOG_D("STE_CAT_CAUSE_CAT_INFO -> STE_CAT_INFO_SUCCESS");
                break;
            case STE_CAT_INFO_NOT_SUPPORTED:
                ATC_LOG_D("STE_CAT_CAUSE_CAT_INFO -> STE_CAT_INFO_NOT_SUPPORTED");
                break;
            case STE_CAT_INFO_PROFILE_FAIL:
                ATC_LOG_D("STE_CAT_CAUSE_CAT_INFO -> STE_CAT_INFO_PROFILE_FAIL");
                break;
            case STE_CAT_INFO_NULL:
                ATC_LOG_D("STE_CAT_CAUSE_CAT_INFO -> STE_CAT_INFO_NULL");
                break;
            default:
                ATC_LOG_D("STE_CAT_CAUSE_CAT_INFO -> info = %d", cat_info_ind_p->info);
                break;
            }
        }

        break;

    case STE_UICC_CAUSE_REQ_GET_SIM_STATE:

        if (NULL != data_p) {
            ste_uicc_get_sim_state_response_t *uicc_get_sim_state_response_p = (ste_uicc_get_sim_state_response_t *)data_p;

            switch (uicc_get_sim_state_response_p->state) {
            case SIM_STATE_UNKNOWN:
                ATC_LOG_D("STE_UICC_CAUSE_REQ_GET_SIM_STATE -> SIM_STATE_UNKNOWN");
                break;
            case SIM_STATE_NOT_READY:
                ATC_LOG_D("STE_UICC_CAUSE_REQ_GET_SIM_STATE -> SIM_STATE_NOT_READY");
                break;
            default:
                ATC_LOG_D("STE_UICC_CAUSE_REQ_GET_SIM_STATE -> state = %d", uicc_get_sim_state_response_p->state);

                if (exe_start_get_status(EXE_START_SIM) == EXE_START_CONNECTED_WAITING) {
                    /* Update the SIM service status */
                    exe_start_set_status(EXE_START_SIM, EXE_START_CONNECTED_READY);
                }

                break;
            }

        } else {
            ATC_LOG_E("STE_UICC_CAUSE_REQ_GET_SIM_STATE, data_p == NULL!");
        }

        break;

    case STE_UICC_CAUSE_REGISTER: {
        /* Registered, consume to prevent a print */
        ATC_LOG_D("STE_UICC_CAUSE_REGISTER");
        break;
    }
    case STE_UICC_CAUSE_SIM_STATUS: {

        if (NULL != data_p) {
            sim_status_t *sim_status_p = (sim_status_t *)data_p;
            ATC_LOG_D("STE_UICC_CAUSE_SIM_STATUS, reason =%d", sim_status_p->reason);

            exe_esimsr_sim_state_t exe_sim_status;
            exe_sim_status.sim_state = sim_reason_to_esimsr_state(sim_status_p->reason);
            exe_event(EXE_UNSOLICITED_ESIMSR, &exe_sim_status);

            /* Check if PIN1 is enabled to detect if we need to do a EPEV */
            if (SIM_REASON_STARTUP_DONE == sim_status_p->reason) {
                int result = ste_uicc_pin_info(sim_client_p->ste_sim_p, EXE_SIM_EVENT_CLIENT_TAG, SIM_PIN_PIN1);
                if (STE_SIM_SUCCESS != result) {
                    ATC_LOG_E("ste_uicc_pin_info failed, result %d!", result);
                }
            }
        } else {
            ATC_LOG_E("STE_UICC_CAUSE_SIM_STATUS, data_p == NULL!");
        }

        break;
    }
    case STE_UICC_CAUSE_SIM_STATE_CHANGED: {
        /* We get this but it is an alternative to STE_UICC_CAUSE_SIM_STATUS */

        if (NULL != data_p) {
            ste_uicc_sim_state_changed_t *uicc_sim_state_changed_p = (ste_uicc_sim_state_changed_t *)data_p;
            ATC_LOG_D("STE_UICC_CAUSE_SIM_STATE_CHANGED, state =%d", uicc_sim_state_changed_p->state);

            switch (uicc_sim_state_changed_p->state) {
            case SIM_STATE_UNKNOWN:
            case SIM_STATE_NOT_READY:
                break;
            default:

                if (exe_start_get_status(EXE_START_SIM) == EXE_START_CONNECTED_WAITING) {
                    /* Update the SIM service status */
                    exe_start_set_status(EXE_START_SIM, EXE_START_CONNECTED_READY);
                }

                break;
            }
        } else {
            ATC_LOG_E("STE_UICC_CAUSE_SIM_STATUS, data_p == NULL!");
        }

        break;
    }
    case STE_UICC_CAUSE_REQ_PIN_INFO:
        if (NULL != data_p) {
            ste_uicc_pin_info_response_t *uicc_pin_info_response_p = (ste_uicc_pin_info_response_t *)data_p;
            ATC_LOG_D("STE_UICC_CAUSE_REQ_PIN_INFO");

            if (STE_UICC_PIN_STATUS_ENABLED == uicc_pin_info_response_p->pin_status) {
                ATC_LOG_D("exe_sim_state_changed, do a EPEV");
                exe_event(EXE_UNSOLICITED_EPEV, NULL);
            }
        } else {
            ATC_LOG_E("STE_UICC_CAUSE_REQ_PIN_INFO, data_p == NULL!");
        }
        break;
    case STE_CAT_CAUSE_PC:
    case STE_CAT_CAUSE_PC_NOTIFICATION: {

        if (NULL != data_p) {
            ste_cat_pc_ind_t *sim_cat_pc_p = (ste_cat_pc_ind_t *)data_p;
            exe_apdu_data_t received_data;
            ATC_LOG_D("STE_CAT_CAUSE_PC(_NOTIFICATION)");

            /* Ignore the sim_cat_pc_p->simd_tag on purpose as it has no meaningful
             * meaning for AT
             */

            received_data.buf_p = (char *) sim_cat_pc_p->apdu.buf;
            received_data.len = sim_cat_pc_p->apdu.len;

            if (NULL == received_data.buf_p ||
                    0 == received_data.len) {
                ATC_LOG_E("STE_CAT_CAUSE_PC, no apdu buf or len = 0");
                return;
            }

            if (STE_CAT_CAUSE_PC == cause) {
                exe_event(EXE_UNSOLICITED_SAT_PROACTIVE_COMMAND, &received_data);
            } else if (STE_CAT_CAUSE_PC_NOTIFICATION == cause) {
                exe_event(EXE_UNSOLICITED_SAT_EVENT_NOTIFY, &received_data);
            }
        } else {
            ATC_LOG_E("STE_CAT_CAUSE_PC, data_p == NULL");
        }

        break;
    }
    case STE_CAT_CAUSE_SESSION_END_IND: {
        ATC_LOG_D("STE_CAT_CAUSE_SESSION_END_IND");

        exe_event(EXE_UNSOLICITED_SAT_SESSION_COMPLETED, NULL);
        break;
    }
    case STE_CAT_CAUSE_PC_REFRESH_IND: {
        if (NULL != data_p) {
            ste_sim_pc_refresh_type_t refresh_type = ((ste_cat_pc_refresh_ind_t *)data_p)->type;
            exe_esimrf_unsol_t esimrf_data;
            esimrf_data.refresh_type = EXE_SIM_REFRESH_UNKNOWN;
            ATC_LOG_D("STE_CAT_CAUSE_PC_REFRESH_IND, refresh_type = %d", refresh_type);

            switch (refresh_type) {
            case STE_SIM_PC_REFRESH_INIT_AND_FULL_FILE_CHANGE: {
                esimrf_data.refresh_type = EXE_SIM_REFRESH_INIT_FULL_FILE_CHANGE;
                break;
            }
            case STE_SIM_PC_REFRESH_INIT: { /* Init */
                esimrf_data.refresh_type = EXE_SIM_REFRESH_INIT;
                break;
            }
            case STE_SIM_PC_REFRESH_UICC_RESET: { /* Reset */
                esimrf_data.refresh_type = EXE_SIM_REFRESH_RESET;
                break;
            }
            case STE_SIM_PC_REFRESH_3G_APP_RESET: {
                esimrf_data.refresh_type = EXE_SIM_REFRESH_3G_APP_RESET;
                break;
            }
            case STE_SIM_PC_REFRESH_3G_SESSION_RESET: {
                esimrf_data.refresh_type = EXE_SIM_REFRESH_3G_SESSION_RESET;
                break;
            }
            case STE_SIM_PC_REFRESH_INIT_AND_FILE_CHANGE: {
                esimrf_data.refresh_type = EXE_SIM_REFRESH_INIT_FILE_CHANGE;
                break;
            }
            case STE_SIM_PC_REFRESH_FILE_CHANGE: {
                esimrf_data.refresh_type = EXE_SIM_REFRESH_FILE_CHANGE;
                break;
            }
            case STE_SIM_PC_REFRESH_RESERVED: {
                /* No mapping to the ESIMRF response to this simd status */
                ATC_LOG_E("STE_CAT_CAUSE_PC_REFRESH_IND, with wrong refresh type STE_SIM_PC_REFRESH_RESERVED");
                break;
            }
            default :
                ATC_LOG_E("STE_CAT_CAUSE_PC_REFRESH_IND, with unknown refresh type");
            } /* switch */

            if (esimrf_data.refresh_type != EXE_SIM_REFRESH_UNKNOWN) {
                esimrf_data.file_id = 0;
                esimrf_data.path_p = NULL;
                exe_event(EXE_UNSOLICITED_ESIMRF, (void *)&esimrf_data);
            }

#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
            /* As this indication may cause actions in extension handling it is called here */
            sim_client_callback_extension(cause, client_tag, data_p, user_data_p);
#endif


        } else {
            ATC_LOG_E("STE_CAT_CAUSE_PC_REFRESH_IND, data_p == NULL!");
        }

        break;
    }

    case STE_CAT_CAUSE_PC_REFRESH_FILE_IND: {
        if (NULL != data_p) {
            sim_path_t sim_path = ((ste_cat_pc_refresh_file_ind_t *)data_p)->path;
            exe_esimrf_unsol_t esimrf_data;
            int i = 0;
            char *tmp_buffer_p;
            ATC_LOG_D("STE_CAT_CAUSE_PC_REFRESH_FILE_IND");

            /* TODO Will this event generate
             *  File Change Notification and SIM Initialization and Full File Change Notification
             *  If so then the sind api is not sufficient */
            esimrf_data.refresh_type = EXE_SIM_REFRESH_FILE_CHANGE;
            esimrf_data.file_id = (uint16_t)((sim_path.path[sim_path.pathlen-2] << 8) + sim_path.path[sim_path.pathlen-1]);

            if (sim_path.pathlen > 2) { /* Data contains a path to the given file ID */
                esimrf_data.path_p = alloca(SIM_MAX_PATH_LENGTH);

                if (!esimrf_data.path_p) {
                    goto error;
                }

                memset(esimrf_data.path_p, '\0', SIM_MAX_PATH_LENGTH);
                tmp_buffer_p = alloca(4);

                if (!tmp_buffer_p) {
                    goto error;
                }

                for (i = 0; i < (sim_path.pathlen - 2); ++i) {
                    *tmp_buffer_p = 0;
                    sprintf(tmp_buffer_p, "%02X", sim_path.path[i]);
                    strcat(esimrf_data.path_p, tmp_buffer_p);

                }
            } else {
                esimrf_data.path_p = NULL;
            }

            exe_event(EXE_UNSOLICITED_ESIMRF, (void *)&esimrf_data);

#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
            /* As this indication may cause actions in extension handling it is called here */
            sim_client_callback_extension(cause, client_tag, data_p, user_data_p);
#endif

        } else {
            ATC_LOG_E("STE_CAT_CAUSE_PC_REFRESH_FILE_IND, data_p == NULL!");
        }

        break;
error:
        ATC_LOG_E("STE_CAT_CAUSE_PC_REFRESH_FILE_IND error!");
        break;
    }

    default:
#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
        sim_client_callback_extension(cause, client_tag, data_p, user_data_p);
#else
        ATC_LOG_I("Received unsupported event = %d", cause);
#endif
        break;
    }
}

/********************************************************************
 * Public methods
 ********************************************************************
 */

/**
 * Starts a session towards the service
 * returns simclient on success, NULL on error
 */
void *sim_client_open_session()
{
    sim_client_t *sim_client_p = malloc(sizeof(*sim_client_p));
    int result;
    uint32_t reg_events = STE_CAT_CLIENT_REG_EVENTS_CAT_INFO |
                          STE_CAT_CLIENT_REG_EVENTS_CARD_STATUS;

    ATC_LOG_D("%s: called", __FUNCTION__);

    if (NULL == sim_client_p) {
        goto error;
    }

    /* Init sim_client  */
    sim_client_p->fd_sim = -1;
    sim_client_p->ste_sim_p = NULL;
    sim_client_p->ste_sim_closure.func = sim_client_callback;
    sim_client_p->ste_sim_closure.user_data = sim_client_p;
    sim_client_p->ste_sim_p = ste_sim_new_st(&(sim_client_p->ste_sim_closure));

    result = ste_sim_connect(sim_client_p->ste_sim_p, EXE_SIM_EVENT_CLIENT_TAG);

    if (STE_SIM_SUCCESS != result) {
        ATC_LOG_E("sim_client_open_session, result %d!", result);
        goto error;
    }

    sim_client_p->fd_sim = ste_sim_fd(sim_client_p->ste_sim_p);

    if (sim_client_p->fd_sim < 0) {
        ATC_LOG_E("sim_client_open_session failed to get a fd from sim");
        goto error;
    }

    /* To be able to receive a event telling that the CAT service is ready on the sim
     * we must register with the cat service */
    result = ste_cat_register(sim_client_p->ste_sim_p, EXE_SIM_EVENT_CLIENT_TAG, reg_events);

    if (STE_SIM_SUCCESS != result) {
        ATC_LOG_E("ste_sim_startup, ste_cat_register for cat events failed result %d!", result);
    }

    /* Set service status */
    exe_start_set_status(EXE_START_SIM, EXE_START_CONNECTED_WAITING);

    /* Register selector callbacks */
    selector_register_callback_for_fd(sim_client_p->fd_sim, sim_client_selector_callback, (void *)sim_client_p);

    ATC_LOG_I("sim_client_open_session done fd = %d", sim_client_p->fd_sim);
    return sim_client_p;

error:
    /* Failed to initialize all connections, close down. */
    sim_client_close_session(sim_client_p);
    return NULL;
}

/**
 * Do service specific clean up when closing session.
 */
void sim_client_close_session(void *service_p)
{
    sim_client_t *sim_client_p = (sim_client_t *)service_p;
    ATC_LOG_D("%s: called", __FUNCTION__);

    if (NULL != sim_client_p) {

        ste_sim_delete(sim_client_p->ste_sim_p, EXE_SIM_EVENT_CLIENT_TAG);
        sim_client_p->ste_sim_p = NULL;

        selector_deregister_callback_for_fd(sim_client_p->fd_sim);
        free(sim_client_p);
    }

    /* Set service status */
    exe_start_set_status(EXE_START_SIM, EXE_START_NOT_CONNECTED);
}

ste_sim_t *sim_client_get_handle(sim_client_t *sim_client_p)
{
    if (NULL == sim_client_p) {
        return NULL;
    } else {
        return sim_client_p->ste_sim_p;
    }
}

/* Add functionality for pseudo synchronization if needed */
