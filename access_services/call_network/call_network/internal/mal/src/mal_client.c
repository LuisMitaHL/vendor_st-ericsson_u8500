/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "fdmon.h"
#include "modem_client.h"

#include "cn_log.h"
#include "cn_macros.h"
#include "cn_client.h"
#include "request_handling.h"
#include "event_handling.h"
#include "main.h"


#ifdef USE_MAL_CS
#include "mal_call.h"
#include "mal_ss.h"
#endif
#ifdef USE_MAL_GSS
#include "mal_gss.h"
#endif
#ifdef USE_MAL_MCE
#include "mal_mce.h"
#endif
#ifdef USE_MAL_MIS
#include "mal_mis.h"
#endif
#ifdef USE_MAL_NET
#include "mal_net.h"
#endif
#ifdef USE_MAL_NVD
#include "mal_nvd.h"
#endif
#ifdef USE_MAL_RF
#include "mal_rf.h"
#endif
#ifdef USE_MAL_MTE
#include "mal_mte.h"
#endif

#ifdef USE_MAL_FTD
#include "mal_ftd.h"
#endif

#include "mal_utils.h"
#include "shm_netlnk.h"

struct modem_client_s {
    int fd_mce;
    int fd_nvd;
    int fd_mis;
    int fd_net;
    int fd_gss;
    int fd_call;
    int fd_ss;
    int fd_rf;
    int fd_netlnk;
    int fd_mte;
    int fd_ftd;
};


/********************************************************************
 * Private prototypes
 ********************************************************************
 */

#ifdef USE_MAL_CS
void call_event_callback(int mal_call_event_id, void *data_p, int mal_error, void *modem_tag_p);
int mal_client_call_callback(const int fd, const void *data_p);

void ss_event_callback(int mal_ss_event_id, void *data_p, int error_code, void *modem_tag_p);
static int mal_client_ss_callback(const int fd, const void *data_p);
#endif

#ifdef USE_MAL_GSS
void gss_event_callback(int mal_gss_event_id, void *data_p, mal_gss_error_type mal_error, void *modem_tag_p);
static int mal_client_gss_callback(const int fd, const void *data_p);
#endif

#ifdef USE_MAL_MCE
void mce_event_callback(int mal_event_id, void *data_p, mal_mce_error_type error_code, void *modem_tag_p);
static int mal_client_mce_callback(const int fd, const void *data_p);
static int mal_client_netlnk_callback(const int fd, const void *data_p);
#endif

#ifdef USE_MAL_MIS
void mis_event_callback(int mal_event_id, void *data_p, mal_mis_error_type error_code, void *modem_tag_p);
static int mal_client_mis_callback(const int fd, const void *data_p);
#endif

#ifdef USE_MAL_NET
void net_event_callback(int mal_event_id, void *data_p, mal_net_error_type error_code, void *modem_tag_p);
static int mal_client_net_callback(const int fd, const void *data_p);
#endif

#ifdef USE_MAL_NVD
void nvd_event_callback(int mal_event_id, void *data_p, mal_nvd_error_type error_code, void *modem_tag_p);
static int mal_client_nvd_callback(const int fd, const void *data_p);
#endif

#ifdef USE_MAL_RF
void rf_event_callback(int mal_event_id, void *data_p, mal_rf_error_type error_code, void *modem_tag);
static int mal_client_rf_callback(const int fd, const void *data_p);
#endif

#ifdef USE_MAL_MTE
void mte_event_callback(int mal_event_id, void *data_p, mal_mte_error_type error_code, void *modem_tag);
static int mal_client_mte_callback(const int fd, const void *data_p);
#endif
#ifdef USE_MAL_FTD
void ftd_event_callback(int mal_event_id, void *data_p, mal_ftd_error_type error_code, void *modem_tag);
static int mal_client_ftd_callback(const int fd, const void *data_p);
#endif

static int mal_client_callback_remove(const int fd, const void *data_p);


/********************************************************************
 * Private methods
 ********************************************************************
 */

#ifdef USE_MAL_CS
/*
 * Callback implementation for mal call functionality
 */
void call_event_callback(int mal_call_event_id, void *data_p, int mal_error, void *modem_tag_p)
{
    request_record_t *record_p = NULL;

    if (NULL == modem_tag_p) { /* MAL event */
        switch (mal_call_event_id) {
        case MAL_CALL_RING:
            CN_LOG_D("MAL_CALL_RING");
            handle_event_ring(data_p);
            break;
        case MAL_CALL_RING_WAITING:
            CN_LOG_D("MAL_CALL_RING_WAITING");
            handle_event_ring_waiting(data_p);
            break;
        case MAL_CALL_STATE_CHANGED:
            CN_LOG_D("MAL_CALL_STATE_CHANGED");
            handle_event_call_state_changed(data_p);
            break;
        case MAL_CALL_MODEM_DTMF_STOP_RESP:
            CN_LOG_D("MAL_CALL_MODEM_DTMF_STOP_RESP");
            /* Ignore */
            break;
        case MAL_CALL_SUPP_SVC_NOTIFICATION:
            CN_LOG_D("MAL_CALL_SUPP_SVC_NOTIFICATION");
            handle_event_call_supp_svc_notification(data_p);
            break;
        case MAL_CALL_CNAP:
            CN_LOG_D("MAL_CALL_CNAP");
            handle_event_call_cnap(data_p);
            break;
        case MAL_CALL_GEN_ALERTING_TONE:
            CN_LOG_D("MAL_CALL_GEN_ALERTING_TONE");
            handle_event_generate_local_comfort_tones(data_p);
            break;
        case MAL_CALL_MODEM_EMERG_NBR_IND:
            CN_LOG_D("MAL_CALL_MODEM_EMERG_NBR_IND");
            handle_event_modem_emergency_number_indication(data_p);
            break;
        default:
            CN_LOG_W("Unknown MAL CS event! (mal_event_id:%d)", mal_call_event_id);
            break;
        }
    } else  { /* MAL response. Lookup client tag and invoke handler function. */
        request_status_t status;
        record_p = request_record_from_modem_tag(modem_tag_p);

        if (!record_p) {
            CN_LOG_E("invalid modem_tag_p %p, mal_call_event_id 0x%02X", modem_tag_p, mal_call_event_id);
            goto exit_func;
        }

        record_p->response_error_code = mal_error;
        record_p->response_data_p = data_p;
        status = record_p->request_handler_p(NULL, record_p);

        if (REQUEST_STATUS_PENDING != status) {
            request_record_free(record_p);
        }
    }

exit_func:
    return;
}


int mal_client_call_callback(const int fd, const void *data_p)
{
    modem_client_t *modem_client_p = (modem_client_t *)data_p;
    (void)fd;


    if (modem_client_p) {
        mal_call_response_handler();
    }


    return TRUE;
}


/*
 * Callback implementation for mal ss functionality
 */
void ss_event_callback(int mal_ss_event_id, void *data_p, int error_code, void *modem_tag_p)
{
    request_record_t *record_p = NULL;

    if (NULL == modem_tag_p) { /* MAL event */
        switch (mal_ss_event_id) {
        case MAL_SS_ON_USSD: {
            CN_LOG_D("MAL_SS_ON_USSD");
            handle_event_ussd(data_p);
            break;
        }
        case MAL_SS_DETAILED_FAIL_CAUSE: {
            CN_LOG_D("MAL_SS_DETAILED_FAIL_CAUSE");
            handle_event_ss_detailed_fail_cause(data_p);
            break;
        }
        case MAL_SS_STATUS_IND: {
            CN_LOG_D("MAL_SS_STATUS_IND");
            handle_event_ss_status_ind(data_p);
            break;
        }
        default: {
            CN_LOG_W("Unknown MAL SS event! (mal_event_id:%d)", mal_ss_event_id);
            break;
        }
        }
    } else  { /* MAL response. Lookup client tag and invoke handler function. */
        request_status_t status;
        record_p = request_record_from_modem_tag(modem_tag_p);

        if (!record_p) {
            CN_LOG_E("invalid modem_tag_p %p, mal_ss_event_id 0x%02X", modem_tag_p, mal_ss_event_id);
            goto exit_func;
        }

        record_p->response_error_code = error_code;
        record_p->response_data_p = data_p;
        status = record_p->request_handler_p(NULL, record_p);

        if (REQUEST_STATUS_PENDING != status) {
            request_record_free(record_p);
        }
    }

exit_func:
    return;
}


int mal_client_ss_callback(const int fd, const void *data_p)
{
    modem_client_t *modem_client_p = (modem_client_t *)data_p;
    (void)fd;


    if (modem_client_p) {
        mal_ss_response_handler();
    }

    return TRUE;
}


#endif /* USE_MAL_CS */


#ifdef USE_MAL_GSS
/*
 * Callback implementation for mal gss functionality
 */
void gss_event_callback(int mal_gss_event_id, void *data_p, mal_gss_error_type mal_error, void *modem_tag_p)
{
    request_record_t *record_p = NULL;
    (void)mal_error;

    if (NULL == modem_tag_p) { /* MAL event */
        switch (mal_gss_event_id) {
        case MAL_GSS_SET_USER_ACTIVITY_INFO_IND:
            CN_LOG_D("MAL_GSS_SET_USER_ACTIVITY_INFO_IND");
            /* Ignore */
            break;
        default: {
            CN_LOG_E("Unknown MAL GSS event! (mal_event_id:%d)", mal_gss_event_id);
            break;
        }
        }
    } else  { /* MAL response. Lookup client tag and invoke handler function. */
        request_status_t status;
        record_p = request_record_from_modem_tag(modem_tag_p);

        if (!record_p) {
            CN_LOG_E("invalid modem_tag_p %p, mal_gss_event_id 0x%02X", modem_tag_p, mal_gss_event_id);
            goto exit_func;
        }

        record_p->response_error_code = mal_error;
        record_p->response_data_p = data_p;
        status = record_p->request_handler_p(NULL, record_p);

        if (REQUEST_STATUS_PENDING != status) {
            request_record_free(record_p);
        }
    }

exit_func:
    return;
}


int mal_client_gss_callback(const int fd, const void *data_p)
{
    modem_client_t *modem_client_p = (modem_client_t *)data_p;
    (void)fd;


    if (modem_client_p) {
        mal_gss_response_handler();
    }

    return TRUE;
}
#endif /* USE_MAL_GSS */


#ifdef USE_MAL_MCE
/*
 * Callback implementation for mal mce functionality
 */
void mce_event_callback(int mal_event_id, void *data_p, mal_mce_error_type error_code, void *modem_tag_p)
{
    request_record_t *record_p = NULL;
    (void)error_code;

    if (NULL == modem_tag_p) { /* MAL event */
        switch (mal_event_id) {
        case MAL_MCE_RF_STATE_IND: {
            CN_LOG_D("MAL_MCE_RF_STATE_IND");
            handle_event_radio_status(data_p);
            break;
        }
        default: {
            CN_LOG_W("Unknown MAL MCE event! (mal_event_id:%d)", mal_event_id);
            break;
        }
        }
    } else  { /* MAL response. Lookup client tag and invoke handler function. */
        request_status_t status;
        record_p = request_record_from_modem_tag(modem_tag_p);

        if (!record_p) {
            CN_LOG_E("invalid modem_tag_p %p, mal_event_id 0x%02X", modem_tag_p, mal_event_id);
            goto exit_func;
        }

        record_p->response_error_code = error_code;
        record_p->response_data_p = data_p;
        status = record_p->request_handler_p(NULL, record_p);

        if (REQUEST_STATUS_PENDING != status) {
            request_record_free(record_p);
        }
    }

exit_func:
    return;
}


int mal_client_mce_callback(const int fd, const void *data_p)
{
    modem_client_t *modem_client_p = (modem_client_t *)data_p;
    (void)fd;


    if (modem_client_p) {
        mal_mce_response_handler();
    }

    return TRUE;
}

int mal_client_netlnk_callback(const int fd, const void *data_p)
{
    int netlnk_msg = -1;
    modem_client_t *modem_client_p = (modem_client_t *)data_p;
    (void)fd;


    if (modem_client_p) {
        netlnk_socket_recv(fd, &netlnk_msg);

        if (netlnk_msg == MODEM_RESET_IND) {
            CN_LOG_D("received modem reset indication!!");

            /* Request process termination */
            main_shutdown();

            /* Return -1 as modem reset indication is received */
            return -1;
        }
    }

    return TRUE;
}
#endif /* USE_MAL_MCE */


#ifdef USE_MAL_MIS
/*
 * Callback implementation for mal mis functionality
 */
void mis_event_callback(int mal_event_id, void *data_p, mal_mis_error_type error_code, void *modem_tag_p)
{
    request_record_t *record_p = NULL;
    (void)error_code;

    if (NULL == modem_tag_p) { /* MAL event */
        switch (mal_event_id) {
            /*
                case XXX: {
                    handle_event_XXX(data_p);
                    break;
                }
            */
        default: {
            CN_LOG_E("Unknown MAL MIS event! (mal_event_id:%d)", mal_event_id);
            break;
        }
        }
    } else  { /* MAL response. Lookup client tag and invoke handler function. */
        request_status_t status;
        record_p = request_record_from_modem_tag(modem_tag_p);

        if (!record_p) {
            CN_LOG_E("invalid modem_tag_p %p, mal_event_id 0x%02X", modem_tag_p, mal_event_id);
            goto exit_func;
        }

        record_p->response_error_code = error_code;
        record_p->response_data_p = data_p;
        status = record_p->request_handler_p(NULL, record_p);

        if (REQUEST_STATUS_PENDING != status) {
            request_record_free(record_p);
        }
    }

exit_func:
    return;
}


int mal_client_mis_callback(const int fd, const void *data_p)
{
    modem_client_t *modem_client_p = (modem_client_t *)data_p;
    (void)fd;


    if (modem_client_p) {
        mal_mis_response_handler();
    }

    return TRUE;
}
#endif /* USE_MAL_MIS */


#ifdef USE_MAL_NET
/*
 * Callback implementation for mal net functionality
 */
void net_event_callback(int mal_event_id, void *data_p, mal_net_error_type error_code, void *modem_tag_p)
{
    request_record_t *record_p = NULL;

    if (NULL == modem_tag_p) { /* MAL event */
        switch (mal_event_id) {
        case MAL_NET_MODEM_REG_STATUS_IND:
            CN_LOG_D("MAL_NET_MODEM_REG_STATUS_IND");
            handle_event_modem_registration_status(data_p);
            break;
        case MAL_NET_TIME_IND:
            CN_LOG_D("MAL_NET_TIME_IND");
            handle_event_time_info(data_p);
            break;
        case MAL_NET_NITZ_NAME_IND:
            CN_LOG_D("MAL_NET_NITZ_NAME_IND");
            handle_event_name_info(data_p);
            break;
        case MAL_NET_UNSOL_NEIGHBOUR_CELLS_INFO:
            CN_LOG_D("MAL_NET_UNSOL_NEIGHBOUR_CELLS_INFO");
            handle_event_neighbour_cells_info(data_p);
            break;
        case MAL_NET_RSSI_IND:
            CN_LOG_D("MAL_NET_RSSI_IND");
            handle_event_signal_info(data_p);
            break;
        case MAL_NET_CELL_INFO_IND:
            CN_LOG_D("MAL_NET_CELL_INFO_IND");
            handle_event_cell_info(data_p);
            break;
        case MAL_NET_MODEM_DETAILED_FAIL_CAUSE:
            CN_LOG_D("MAL_NET_MODEM_DETAILED_FAIL_CAUSE");
            handle_event_net_modem_detailed_fail_cause(data_p);
            break;
        case MAL_NET_RADIO_INFO_IND:
            CN_LOG_D("MAL_NET_RADIO_INFO_IND");
            handle_event_radio_info(data_p);
            break;
        case MAL_NET_RAT_IND:
            CN_LOG_D("MAL_NET_RAT_IND");
            handle_event_rat_name(data_p);
            break;
        default:
            CN_LOG_W("Unknown MAL NET event! (mal_event_id:%d)", mal_event_id);
            break;
        }
    } else  { /* MAL response. Lookup client tag and invoke handler function. */
        request_status_t status;
        record_p = request_record_from_modem_tag(modem_tag_p);

        if (!record_p) {
            CN_LOG_E("invalid modem_tag_p %p, mal_event_id 0x%02X", modem_tag_p, mal_event_id);
            goto exit_func;
        }

        record_p->response_error_code = error_code;
        record_p->response_data_p = data_p;
        status = record_p->request_handler_p(NULL, record_p);

        if (REQUEST_STATUS_PENDING != status) {
            request_record_free(record_p);
        }
    }

exit_func:
    return;
}


int mal_client_net_callback(const int fd, const void *data_p)
{
    modem_client_t *modem_client_p = (modem_client_t *)data_p;
    (void)fd;


    if (modem_client_p) {
        mal_net_response_handler();
    }

    return TRUE;
}
#endif /* USE_MAL_NET */

#ifdef USE_MAL_NVD
void nvd_event_callback(int mal_event_id, void *data_p, mal_nvd_error_type error_code, void *modem_tag_p)
{
    request_record_t *record_p = NULL;

    if (NULL == modem_tag_p) { /* MAL event */
        switch (mal_event_id) {
            default:
                CN_LOG_E("Unknown MAL NVD event! (mal_event_id:%d)", mal_event_id);
                break;
        }
    } else  { /* MAL response. Lookup client tag and invoke handler function. */
        request_status_t status;
        record_p = request_record_from_modem_tag(modem_tag_p);

        if (!record_p) {
            CN_LOG_E("invalid modem_tag_p %p, mal_event_id 0x%02X", modem_tag_p, mal_event_id);
            goto exit_func;
        }

        record_p->response_error_code = error_code;
        record_p->response_data_p = data_p;
        status = record_p->request_handler_p(NULL, record_p);

        if (REQUEST_STATUS_PENDING != status) {
            request_record_free(record_p);
        }
    }

exit_func:
    return;
}

static int mal_client_nvd_callback(const int fd, const void *data_p)
{
    modem_client_t *modem_client_p = (modem_client_t *)data_p;
    (void)fd;

    if (modem_client_p) {
        mal_nvd_response_handler();
    }

    return TRUE;
}
#endif /* USE_MAL_NVD */

#ifdef USE_MAL_RF
void rf_event_callback(int mal_event_id, void *data_p, mal_rf_error_type error_code, void *modem_tag)
{
    request_record_t *record_p = NULL;

    if (NULL == modem_tag) { /* MAL event */
        switch (mal_event_id) {
        default:
            CN_LOG_W("Unknown MAL RF event! (mal_event_id:%d)", mal_event_id);
            break;
        }
    } else  { /* MAL response. Lookup client tag and invoke handler function. */
        request_status_t status;
        record_p = request_record_from_modem_tag(modem_tag);

        if (!record_p) {
            CN_LOG_E("invalid modem_tag %p, mal_event_id 0x%02X", modem_tag, mal_event_id);
            goto exit_func;
        }

        record_p->response_error_code = error_code;
        record_p->response_data_p = data_p;
        status = record_p->request_handler_p(NULL, record_p);

        if (REQUEST_STATUS_PENDING != status) {
            request_record_free(record_p);
        }
    }

exit_func:
    return;
}

static int mal_client_rf_callback(const int fd, const void *data_p)
{
    modem_client_t *modem_client_p = (modem_client_t *)data_p;
    (void)fd;

    if (modem_client_p) {
        mal_rf_response_handler();
    }

    return TRUE;
}

#endif /* USE_MAL_RF */

#ifdef USE_MAL_FTD
/*
 * Callback implementation for mal ftd functionality
 */
void ftd_event_callback(int mal_event_id, void *data_p, mal_ftd_error_type error_code, void *modem_tag)
{
    request_record_t *record_p = NULL;

    if (NULL == modem_tag) { /* MAL event */
        switch (mal_event_id) {
        case MAL_FTD_MEASUREMENT_IND:
            CN_LOG_D("MAL_FTD_MEASUREMENT_IND");
            handle_event_empage_measurement(data_p);
            break;

        case MAL_FTD_ERROR_IND:
            CN_LOG_D("MAL_FTD_ERROR_IND");
            handle_event_empage_error((void *)error_code);
            break;

        case MAL_FTD_ACT_IND:
            CN_LOG_D("MAL_FTD_ACT_IND");
            handle_event_empage_activation((void *)error_code);
            break;

        case MAL_FTD_DEACT_IND:
            CN_LOG_D("MAL_FTD_DEACT_IND");
            handle_event_empage_deactivation((void *)error_code);
            break;

        default:
            CN_LOG_E("Unknown MAL FTD event! (mal_event_id:%d)", mal_event_id);
            break;
        }
    } else { /* MAL response. Lookup client tag and invoke handler function. */
        request_status_t status;
        record_p = request_record_from_modem_tag(modem_tag);

        if (!record_p) {
            CN_LOG_E("invalid modem_tag %p, mal_event_id 0x%02X", modem_tag, mal_event_id);
            goto exit_func;
        }

        record_p->response_error_code = error_code;
        record_p->response_data_p     = data_p;
        status = record_p->request_handler_p(NULL, record_p);

        if (REQUEST_STATUS_PENDING != status) {
            request_record_free(record_p);
        }
    }

exit_func:
    return;
}


int mal_client_ftd_callback(const int fd, const void *data_p)
{
    modem_client_t *modem_client_p = (modem_client_t *)data_p;

    if (modem_client_p) {
        mal_ftd_response_handler(fd);
    }

    return TRUE;
}
#endif

#ifdef USE_MAL_MTE
/*
 * Callback implementation for mal mte functionality
 */
void mte_event_callback(int mal_event_id, void *data_p, mal_mte_error_type error_code, void *modem_tag)
{
    request_record_t *record_p = NULL;
    (void)error_code;

    if (NULL == modem_tag) { /* MAL event */
        CN_LOG_E("Unknown MAL MTE event! (mal_event_id:%d)", mal_event_id);
    } else  { /* MAL response. Lookup client tag and invoke handler function. */
        request_status_t status;
        record_p = request_record_from_modem_tag(modem_tag);

        if (!record_p) {
            CN_LOG_E("invalid modem_tag %p, mal_event_id 0x%02X", modem_tag, mal_event_id);
            goto exit_func;
        }

        record_p->response_error_code = error_code;
        record_p->response_data_p = data_p;
        status = record_p->request_handler_p(NULL, record_p);

        if (REQUEST_STATUS_PENDING != status) {
            request_record_free(record_p);
        }
     }

exit_func:
    return;
}

static int mal_client_mte_callback(const int fd, const void *data_p)
{
    modem_client_t *modem_client_p = (modem_client_t *)data_p;
    (void)fd;

    if (modem_client_p) {
        mal_mte_response_handler();
    }

    return TRUE;
}
#endif /* USE_MAL_MTE */


int mal_client_callback_remove(const int fd, const void *data_p)
{
    modem_client_t *modem_client_p = (modem_client_t *)data_p;
    (void)fd;
    (void)modem_client_p;


    /* FIXME: File descriptor removed from monitoring because of closure or error */
    CN_LOG_D("mal_client_callback_remove() called by fdmon for fd = %d!", fd);

    return 0;
}



/********************************************************************
 * Public methods
 ********************************************************************
 */

/**
 * Starts a session towards the modem
 * returns malclient on success, NULL on error
 */
modem_client_t *modem_client_open_session(void)
{
    modem_client_t *modem_client_p;
    int result = 0;


    /* Init mal_client */
    modem_client_p = calloc(1, sizeof(*modem_client_p));

    if (!modem_client_p) {
        CN_LOG_E("calloc failed for modem_client_p!");
        goto error;
    }

#ifdef USE_MAL_MCE
    /* Start mal mce */
    result = mal_mce_init(&modem_client_p->fd_mce, &modem_client_p->fd_netlnk);

    if (MAL_MCE_SUCCESS != result) {
        CN_LOG_E("modem_client_open_session error init MCE: result = %d", result);
        goto error;
    }

    if (-1 == modem_client_p->fd_mce || -1 == modem_client_p->fd_netlnk) {
        CN_LOG_E("modem_client_open_session error init MCE: no fd received!");
        goto error;
    }

    result = mal_mce_register_callback(mce_event_callback);

    if (MAL_MCE_SUCCESS != result) {
        CN_LOG_E("modem_client_open_session error reg callback MCE: result = %d", result);
        goto error;
    }

    result = mal_mce_config();

    if (MAL_MCE_SUCCESS != result) {
        CN_LOG_E("modem_client_open_session error config MCE: result = %d", result);
        goto error;
    }

    fdmon_add(modem_client_p->fd_mce, (void *)modem_client_p, &mal_client_mce_callback, &mal_client_callback_remove);
    fdmon_add(modem_client_p->fd_netlnk, (void *)modem_client_p, &mal_client_netlnk_callback, &mal_client_callback_remove);
#endif /* USE_MAL_MCE */

#ifdef USE_MAL_NET
    /* Start mal net */
    result = mal_net_init(&modem_client_p->fd_net);

    if (MAL_NET_SUCCESS != result) {
        CN_LOG_E("modem_client_open_session error init NET: result = %d", result);
        /* if this fails then we must deinit call */
        goto error;
    }

    if (-1 == modem_client_p->fd_net) {
        CN_LOG_E("modem_client_open_session error init NET: no fd received!");
        goto error;
    }

    result = mal_net_register_callback(net_event_callback);

    if (MAL_NET_SUCCESS != result) {
        CN_LOG_E("modem_client_open_session error reg callback NET: result = %d", result);
        goto error;
    }

    fdmon_add(modem_client_p->fd_net, (void *)modem_client_p, &mal_client_net_callback, &mal_client_callback_remove);
#endif /* USE_MAL_NET */

#ifdef USE_MAL_CS
    /* Start mal call */
    result = mal_call_init(&modem_client_p->fd_call);

    if (MAL_SUCCESS != result) {
        CN_LOG_E("modem_client_open_session error init CALL: result = %d", result);
        goto error;
    }

    if (-1 == modem_client_p->fd_call) {
        CN_LOG_E("modem_client_open_session error init CALL: no fd received!");
        goto error;
    }

    result = mal_call_register_callback(call_event_callback);

    if (MAL_SUCCESS != result) {
        CN_LOG_E("modem_client_open_session error reg callback CALL: result = %d", result);
        goto error;
    }

    result = mal_call_config();

    if (MAL_SUCCESS != result) {
        CN_LOG_E("modem_client_open_session error config CALL: result = %d", result);
        goto error;
    }

    fdmon_add(modem_client_p->fd_call, (void *)modem_client_p, &mal_client_call_callback, &mal_client_callback_remove);
#endif

#ifdef USE_MAL_CS
    /* Start mal ss */
    result = mal_ss_init(&modem_client_p->fd_ss);

    if (MAL_SUCCESS != result) {
        CN_LOG_E("modem_client_open_session error init SS: result = %d", result);
        /* if this fails then we must deinit call */
        goto error;
    }

    if (-1 == modem_client_p->fd_ss) {
        CN_LOG_E("modem_client_open_session error init SS: no fd received!");
        goto error;
    }

    result = mal_ss_register_callback(ss_event_callback);

    if (MAL_SUCCESS != result) {
        CN_LOG_E("modem_client_open_session error reg callback SS: result = %d", result);
        goto error;
    }

    result = mal_ss_config();

    if (MAL_SUCCESS != result) {
        CN_LOG_E("modem_client_open_session error config SS: result = %d", result);
        goto error;
    }

    fdmon_add(modem_client_p->fd_ss, (void *)modem_client_p, &mal_client_ss_callback, &mal_client_callback_remove);
#endif /* USE_MAL_CS */

#ifdef USE_MAL_GSS
    /* Start mal gss */
    result = mal_gss_init(&modem_client_p->fd_gss);

    if (MAL_GSS_SUCCESS != result) {
        CN_LOG_E("modem_client_open_session error init GSS: result = %d", result);
        goto error;
    }

    if (-1 == modem_client_p->fd_gss) {
        CN_LOG_E("modem_client_open_session error init GSS: no fd received!");
        goto error;
    }

    result = mal_gss_register_callback(gss_event_callback);

    if (MAL_GSS_SUCCESS != result) {
        CN_LOG_E("modem_client_open_session error reg callback GSS: result = %d", result);
        goto error;
    }

    fdmon_add(modem_client_p->fd_gss, (void *)modem_client_p, &mal_client_gss_callback, &mal_client_callback_remove);
#endif /* USE_MAL_GSS */

#ifdef USE_MAL_MIS
    /* Start mal mis */
    result = mal_mis_init(&modem_client_p->fd_mis);

    if (MAL_MIS_SUCCESS != result) {
        CN_LOG_E("modem_client_open_session error init MIS: result = %d", result);
        goto error;
    }

    if (-1 == modem_client_p->fd_mis) {
        CN_LOG_E("modem_client_open_session error init MIS: no fd received!");
        goto error;
    }

    result = mal_mis_register_callback(mis_event_callback);

    if (MAL_MIS_SUCCESS != result) {
        CN_LOG_E("modem_client_open_session error reg callback MIS: result = %d", result);
        goto error;
    }

    fdmon_add(modem_client_p->fd_mis, (void *)modem_client_p, &mal_client_mis_callback, &mal_client_callback_remove);
#endif /* USE_MAL_MIS */

#ifdef USE_MAL_NVD
    /* Start mal nvd */
    result = mal_nvd_init(&modem_client_p->fd_nvd);

    if (MAL_NVD_SUCCESS != result) {
        CN_LOG_E("mal_nvd_init: result = %d", result);
        goto error;
    }

    if (-1 == modem_client_p->fd_nvd) {
        CN_LOG_E("init NVD: no fd received!");
        goto error;
    }

    result = mal_nvd_register_callback(nvd_event_callback);

    if (MAL_NVD_SUCCESS != result) {
        CN_LOG_E("mal_nvd_register_callback: result = %d", result);
        goto error;
    }

    fdmon_add(modem_client_p->fd_nvd, (void *)modem_client_p, &mal_client_nvd_callback, &mal_client_callback_remove);
#endif /* USE_MAL_NVD */

#ifdef USE_MAL_RF
    /* Start mal rf */
    result = mal_rf_init(&modem_client_p->fd_rf);

    if (MAL_RF_SUCCESS != result) {
        CN_LOG_E("modem_client_open_session error init RF: result = %d", result);
        goto error;
    }

    if (-1 == modem_client_p->fd_rf) {
        CN_LOG_E("modem_client_open_session error init RF: no fd received!");
        goto error;
    }

    result = mal_rf_register_callback(rf_event_callback);

    if (MAL_RF_SUCCESS != result) {
        CN_LOG_E("modem_client_open_session error reg callback RF: result = %d", result);
        goto error;
    }

    fdmon_add(modem_client_p->fd_rf, (void *)modem_client_p, &mal_client_rf_callback, &mal_client_callback_remove);
#endif /* USE_MAL_RF */

#ifdef USE_MAL_FTD
    /* Start mal ftd */
    result = mal_ftd_init(&modem_client_p->fd_ftd);

    if (MAL_FTD_SUCCESS != result) {
        CN_LOG_E("modem_client_open_session error init FTD: result = %d", result);
        goto error;
    }

    if (-1 == modem_client_p->fd_ftd) {
        CN_LOG_E("modem_client_open_session error init FTD: no fd received!");
        goto error;
    }

    result = mal_ftd_register_callback(ftd_event_callback);

    if (MAL_FTD_SUCCESS != result) {
        CN_LOG_E("modem_client_open_session error reg callback FTD: result = %d", result);
        goto error;
    }

    fdmon_add(modem_client_p->fd_ftd, (void *)modem_client_p, &mal_client_ftd_callback, &mal_client_callback_remove);
#endif /* USE_MAL_FTD */

#ifdef USE_MAL_MTE
    /* Start mal mte */
    result = mal_mte_init(&modem_client_p->fd_mte);

    if (MAL_MTE_SUCCESS != result) {
        CN_LOG_E("modem_client_open_session error init MTE: result = %d", result);
        goto error;
    }

    if (-1 == modem_client_p->fd_mte) {
        CN_LOG_E("modem_client_open_session error init MTE: no fd received!");
        goto error;
    }

    result = mal_mte_register_callback((mal_mte_event_cb_t)mte_event_callback);

    if (MAL_MTE_SUCCESS != result) {
        CN_LOG_E("modem_client_open_session error reg callback MTE: result = %d", result);
        goto error;
    }

    fdmon_add(modem_client_p->fd_mte, (void *)modem_client_p, &mal_client_mte_callback, &mal_client_callback_remove);
#endif /* USE_MAL_MTE */

    return modem_client_p;

error:
    /* Failed to initialize all connections, close down. */
    modem_client_close_session(modem_client_p);

    return NULL;
}

void modem_client_close_session(modem_client_t *modem_client_p)
{

    /* TODO: Check if deinit should come before fdmon_del, or other way around */

    if (modem_client_p) {
#ifdef USE_MAL_CS
        if (!(0 > modem_client_p->fd_call)) {
            mal_call_deinit();
            fdmon_del(modem_client_p->fd_call);
            modem_client_p->fd_call = -1;
        }
#endif

#ifdef USE_MAL_GSS
        if (!(0 > modem_client_p->fd_gss)) {
            mal_gss_deinit();
            fdmon_del(modem_client_p->fd_gss);
            modem_client_p->fd_gss = -1;
        }
#endif

#ifdef USE_MAL_NET
        if (!(0 > modem_client_p->fd_net)) {
            mal_net_deinit();
            fdmon_del(modem_client_p->fd_net);
            modem_client_p->fd_net = -1;
        }
#endif

#ifdef USE_MAL_MCE
        if ((!(0 > modem_client_p->fd_mce )) && (!(0 > modem_client_p->fd_netlnk ))) {
            mal_mce_deinit();
            fdmon_del(modem_client_p->fd_mce);
            fdmon_del(modem_client_p->fd_netlnk);
            modem_client_p->fd_mce = -1;
            modem_client_p->fd_netlnk = -1;
        }
#endif

#ifdef USE_MAL_CS
        if (!(0 > modem_client_p->fd_ss )) {
            mal_ss_deinit();
            fdmon_del(modem_client_p->fd_ss);
            modem_client_p->fd_ss = -1;
        }
#endif

#ifdef USE_MAL_MIS
        if (!(0 > modem_client_p->fd_mis )) {
            mal_mis_deinit();
            fdmon_del(modem_client_p->fd_mis);
            modem_client_p->fd_mis = -1;
        }

#endif

#ifdef USE_MAL_FTD

        if (modem_client_p->fd_ftd >= 0) {
            mal_ftd_deinit();
            fdmon_del(modem_client_p->fd_ftd);
            modem_client_p->fd_ftd = -1;
        }
#endif

#ifdef USE_MAL_NVD
        if (!(0 > modem_client_p->fd_nvd )) {
            mal_nvd_deinit();
            fdmon_del(modem_client_p->fd_nvd);
            modem_client_p->fd_nvd = -1;
        }
#endif

#ifdef USE_MAL_RF

        if (!(0 > modem_client_p->fd_rf )) {
            mal_rf_deinit();
            fdmon_del(modem_client_p->fd_rf);
            modem_client_p->fd_rf = -1;
        }

#endif

#ifdef USE_MAL_MTE
        if (!(0 > modem_client_p->fd_mte)) {
               mal_mte_deinit();
               fdmon_del(modem_client_p->fd_mte);
               modem_client_p->fd_mte = -1;
        }
#endif

        free(modem_client_p);
    }
}


