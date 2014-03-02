/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>

#include "atc_log.h"
#include "libstecom.h"
#include "mpl_msg.h"
#include "pscc_msg.h"
#include "sterc_msg.h"
#include "stecom_i.h"
#include "wb_test.h"
#include "whitebox.h"

#define REQUEST_SOCKET_PSCC "/dev/socket/pscc_ctrl_server"
#define EVENT_SOCKET_PSCC "/dev/socket/pscc_event_server"
#define REQUEST_SOCKET_STERC "/dev/socket/sterc_ctrl_server"
#define EVENT_SOCKET_STERC "/dev/socket/sterc_event_server"
mpl_list_t *stub_param_list_p = NULL;

char *wb_pscc_msg_id_string(int id);
char *wb_sterc_msg_id_string(int id);

void pscc_release_param_list(mpl_list_t **pscc_param_list_pp)
{
    while (NULL != *pscc_param_list_pp) {
        mpl_param_element_t *param_p = (mpl_param_element_t *) * pscc_param_list_pp;
        *pscc_param_list_pp = (mpl_list_t *)param_p->list_entry.next_p;
        free(param_p->value_p);
        free(param_p);
    }
}

void *mpl_msg_alloc_internal(mpl_msg_type_t type, int param_set_id)
{
    mpl_msg_t *msg_p = NULL;
    mpl_msg_set_descr_t *message_set_descr_p = NULL;
    mpl_param_descr_set_t *param_descr_set_p = NULL;

    msg_p = calloc(1, sizeof(mpl_msg_t));

    if (NULL == msg_p) {
        ATC_LOG_E("mpl_msg_alloc_internal: Failed allocating msg_p");
        return (NULL);
    }

    msg_p->common.param_list_p = NULL;
    msg_p->common.type = type;

    message_set_descr_p = calloc(1, sizeof(mpl_msg_set_descr_t));

    if (NULL == message_set_descr_p) {
        ATC_LOG_E("mpl_msg_alloc_internal: Failed allocating message_set_descr_p");
        return (NULL);
    }

    param_descr_set_p = calloc(1, sizeof(mpl_param_descr_set_t));

    if (NULL == param_descr_set_p) {
        ATC_LOG_E("mpl_msg_alloc_internal: Failed allocating param_descr_set_p");
        return (NULL);
    }

    msg_p->common.message_set_descr_p = message_set_descr_p;
    msg_p->common.message_set_descr_p->param_descr_set_p = param_descr_set_p;
    msg_p->common.message_set_descr_p->param_descr_set_p->param_set_id = param_set_id;

    return (msg_p);
}

size_t mpl_list_len(const mpl_list_t *list_p)
{
    size_t len = 0;
    mpl_param_element_t *elem_p = (mpl_param_element_t *)list_p;

    while (NULL != elem_p) {
        len++;
        elem_p = (mpl_param_element_t *)elem_p->list_entry.next_p;
    }

    return len;
}

mpl_param_element_t*
mpl_param_list_find(mpl_param_element_id_t param_id, mpl_list_t *param_list_p)
{
    mpl_param_element_t *elem_p = (mpl_param_element_t *)param_list_p;

    while (NULL != elem_p) {
        if (elem_p->id == param_id) {
            return elem_p;
        }

        elem_p = (mpl_param_element_t *)elem_p->list_entry.next_p;
    }

    return NULL;
}

/**
 * mpl_add_param_to_list - add parameter to param list
 *
 */
int mpl_add_param_to_list(mpl_list_t **param_list_pp,
                          mpl_param_element_id_t param_id,
                          const void *value_p)
{
    int *int_p = NULL;
    char *string_p = NULL;
    mpl_param_element_t *last_elem_p = NULL;

    if (NULL == param_list_pp) {
        goto error;
    }

    mpl_param_element_t *param_elem_p = calloc(1, sizeof(mpl_param_element_t));

    if (NULL == param_elem_p) {
        return -1;
    }

    param_elem_p->id = param_id;
    param_elem_p->list_entry.next_p = NULL;

    switch (param_id) {
        /* strings */
    case pscc_paramid_apn:
    case pscc_paramid_auth_uid:
    case pscc_paramid_auth_pwd:
    case pscc_paramid_dns_address:
    case pscc_paramid_ipv6_dns_address:
    case pscc_paramid_secondary_dns_address:
    case pscc_paramid_ipv6_secondary_dns_address:
    case pscc_paramid_gw_address:
    case pscc_paramid_netdev_name:
    case pscc_paramid_own_ip_address:
    case pscc_paramid_own_ipv6_address:
    case pscc_paramid_min_qos_sdu_error_ratio:
    case pscc_paramid_min_qos_residual_ber:
    case pscc_paramid_req_qos_sdu_error_ratio:
    case pscc_paramid_req_qos_residual_ber:
    case pscc_paramid_neg_qos_sdu_error_ratio:
    case pscc_paramid_neg_qos_residual_ber:
    case pscc_paramid_static_ip_address:
    case sterc_paramid_lan_device:
    case sterc_paramid_wan_device:
    case sterc_paramid_wan_dns:
    case sterc_paramid_tty:
        if (NULL != value_p) {
            string_p = malloc(1 + strlen((char *) value_p));
            if (NULL == string_p) {
                goto error;
            }

            strcpy(string_p, (char *) value_p);
        }
        param_elem_p->value_p = string_p;
        break;
        /* integers */
    case pscc_paramid_auth_method:
    case pscc_paramid_cause:
    case pscc_paramid_connection_status:
    case pscc_paramid_connid:
    case pscc_paramid_ct:
    case pscc_paramid_hcmp:
    case pscc_paramid_dcmp:
    case pscc_paramid_pdp_type:
    case pscc_paramid_qos_type:
    case pscc_paramid_min_qos_traffic_class:
    case pscc_paramid_min_qos_max_bitrate_uplink:
    case pscc_paramid_min_qos_max_bitrate_downlink:
    case pscc_paramid_min_qos_guaranteed_bitrate_uplink:
    case pscc_paramid_min_qos_guaranteed_bitrate_downlink:
    case pscc_paramid_min_qos_extended_max_bitrate_uplink:
    case pscc_paramid_min_qos_extended_max_bitrate_downlink:
    case pscc_paramid_min_qos_extended_guaranteed_bitrate_uplink:
    case pscc_paramid_min_qos_extended_guaranteed_bitrate_downlink:
    case pscc_paramid_min_qos_delivery_order:
    case pscc_paramid_min_qos_max_sdu_size:
    case pscc_paramid_min_qos_delivery_erroneous_sdu:
    case pscc_paramid_min_qos_traffic_handling_priority:
    case pscc_paramid_min_qos_transfer_delay:
    case pscc_paramid_min_qos_source_statistics_descriptor:
    case pscc_paramid_min_qos_signalling_indication:
    case pscc_paramid_req_qos_traffic_class:
    case pscc_paramid_req_qos_max_bitrate_uplink:
    case pscc_paramid_req_qos_max_bitrate_downlink:
    case pscc_paramid_req_qos_guaranteed_bitrate_uplink:
    case pscc_paramid_req_qos_guaranteed_bitrate_downlink:
    case pscc_paramid_req_qos_extended_max_bitrate_uplink:
    case pscc_paramid_req_qos_extended_max_bitrate_downlink:
    case pscc_paramid_req_qos_extended_guaranteed_bitrate_uplink:
    case pscc_paramid_req_qos_extended_guaranteed_bitrate_downlink:
    case pscc_paramid_req_qos_delivery_order:
    case pscc_paramid_req_qos_max_sdu_size:
    case pscc_paramid_req_qos_delivery_erroneous_sdu:
    case pscc_paramid_req_qos_traffic_handling_priority:
    case pscc_paramid_req_qos_transfer_delay:
    case pscc_paramid_req_qos_source_statistics_descriptor:
    case pscc_paramid_req_qos_signalling_indication:
    case pscc_paramid_neg_qos_traffic_class:
    case pscc_paramid_neg_qos_max_bitrate_uplink:
    case pscc_paramid_neg_qos_max_bitrate_downlink:
    case pscc_paramid_neg_qos_guaranteed_bitrate_uplink:
    case pscc_paramid_neg_qos_guaranteed_bitrate_downlink:
    case pscc_paramid_neg_qos_extended_max_bitrate_uplink:
    case pscc_paramid_neg_qos_extended_max_bitrate_downlink:
    case pscc_paramid_neg_qos_extended_guaranteed_bitrate_uplink:
    case pscc_paramid_neg_qos_extended_guaranteed_bitrate_downlink:
    case pscc_paramid_neg_qos_delivery_order:
    case pscc_paramid_neg_qos_max_sdu_size:
    case pscc_paramid_neg_qos_delivery_erroneous_sdu:
    case pscc_paramid_neg_qos_traffic_handling_priority:
    case pscc_paramid_neg_qos_transfer_delay:
    case pscc_paramid_neg_qos_source_statistics_descriptor:
    case pscc_paramid_neg_qos_signalling_indication:
    case pscc_paramid_min_qos_precedence_class:
    case pscc_paramid_min_qos_delay_class:
    case pscc_paramid_min_qos_reliability_class:
    case pscc_paramid_min_qos_peak_throughput_class:
    case pscc_paramid_min_qos_mean_throughput_class:
    case pscc_paramid_req_qos_precedence_class:
    case pscc_paramid_req_qos_delay_class:
    case pscc_paramid_req_qos_reliability_class:
    case pscc_paramid_req_qos_peak_throughput_class:
    case pscc_paramid_req_qos_mean_throughput_class:
    case pscc_paramid_attach_status:
    case pscc_paramid_uplink_data_size:
    case pscc_paramid_attach_mode:
    case pscc_paramid_tx_data_count_hi:
    case pscc_paramid_tx_data_count_lo:
    case pscc_paramid_rx_data_count_hi:
    case pscc_paramid_rx_data_count_lo:
    case sterc_paramid_message:
    case sterc_paramid_ct:
    case sterc_paramid_cid:
    case sterc_paramid_handle:
    case sterc_paramid_mode:
        if (NULL != value_p) {
            int_p = malloc(sizeof(int));

            if (NULL == int_p) {
                goto error;
            }

            *int_p = *((int *) value_p);
        }
        param_elem_p->value_p = int_p;
        break;
#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
#include <inc_wb_extended_stub_pscc_params.h>
#endif

    default:
        assert(0);
        break;
    }

    if ((pscc_paramid_ct == param_id) || (sterc_paramid_ct == param_id)) {
        EXECUTOR.client_tag_p = (void *) ((uintptr_t)* int_p);
    }

    if (NULL == *param_list_pp) {
        *param_list_pp = (mpl_list_t *)param_elem_p;
    } else { /* insert after existing elements */
        last_elem_p = (mpl_param_element_t *) * param_list_pp;

        while (NULL != last_elem_p->list_entry.next_p) {
            last_elem_p = (mpl_param_element_t *)last_elem_p->list_entry.next_p;
        }

        last_elem_p->list_entry.next_p = (mpl_list_t *)param_elem_p;
    }

    return 0;
error:
    return -1;
}

int mpl_msg_pack(const mpl_msg_t *msg_p, char **buf, size_t *buflen_p)
{
    int param_set_id;

    if (NULL == msg_p ||
            NULL == msg_p->common.message_set_descr_p ||
            NULL == msg_p->common.message_set_descr_p->param_descr_set_p) {

        ATC_LOG_E("mpl_msg_pack, msg_p not fully allocated");
        return -1;
    }

    param_set_id = msg_p->common.message_set_descr_p->param_descr_set_p->param_set_id;

    switch (param_set_id) {
    case PSCC_PARAM_SET_ID:
        ATC_LOG_I(">>> mpl_msg_pack: %s\n", wb_pscc_msg_id_string(msg_p->common.id));

        if (-1 != EXECUTOR.pscc_request_id) {
            ATC_LOG_E("mpl_msg_pack, tried to pack %s, before we got response to %s", wb_pscc_msg_id_string(msg_p->common.id), wb_pscc_msg_id_string(EXECUTOR.pscc_request_id));
            return -1;
        } else {
            EXECUTOR.pscc_request_id = msg_p->common.id;
            EXECUTOR.sterc = false;
            return 0;
        }

    case STERC_PARAM_SET_ID:
        ATC_LOG_I(">>> mpl_msg_pack: %s\n", wb_sterc_msg_id_string(msg_p->common.id));

        if (-1 != EXECUTOR.sterc_request_id) {
            ATC_LOG_E("mpl_msg_pack, tried to pack %s, before we got response to %s", wb_sterc_msg_id_string(msg_p->common.id), wb_sterc_msg_id_string(EXECUTOR.sterc_request_id));
            return -1;
        } else {
            EXECUTOR.sterc_request_id = msg_p->common.id;
            EXECUTOR.sterc = true;
            return 0;
        }

    default:
        ATC_LOG_E("mpl_msg_pack, incorrect param_set_id");
        return -1;
    }
}

int mpl_msg_unpack(char *buf, size_t buflen, mpl_msg_t *msg_p)
{
    if (NULL == msg_p) {
        return -1;
    }

    if (!EXECUTOR.sterc) {
        msg_p->common.param_list_p = EXECUTOR.pscc_param_list_p;
        msg_p->common.type = EXECUTOR.pscc_msg_type;

        switch (EXECUTOR.pscc_msg_type) {
        case mpl_msg_type_resp:
            msg_p->resp.result = EXECUTOR.pscc_result;
            break;
        case mpl_msg_type_event:
            msg_p->event.id = EXECUTOR.pscc_event_id;
            EXECUTOR.pscc_event_id = -1;
            break;
        default:
            break;
        }
    } else {
        msg_p->common.param_list_p = EXECUTOR.sterc_param_list_p;
        msg_p->common.type = EXECUTOR.sterc_msg_type;

        switch (EXECUTOR.sterc_msg_type) {
        case mpl_msg_type_resp:
            msg_p->resp.result = sterc_result_ok;
            break;
        case mpl_msg_type_event:
            msg_p->event.id = EXECUTOR.sterc_event_id;
            EXECUTOR.sterc_event_id = -1;
            break;
        default:
            break;
        }
    }

    return 0;
}

void mpl_msg_free(mpl_msg_t *msg_p)
{
    if (NULL == msg_p) {
        return;
    }

    switch (msg_p->common.type) {
    case mpl_msg_type_req:
        pscc_release_param_list(&msg_p->req.param_list_p);
        break;
    case mpl_msg_type_resp:
        pscc_release_param_list(&msg_p->resp.param_list_p);
        break;
    case mpl_msg_type_event:
        pscc_release_param_list(&msg_p->event.param_list_p);
        break;
    default:
        ATC_LOG_E("mpl_msg_free invalid type");
        break;
    }

    free(msg_p->common.message_set_descr_p->param_descr_set_p);
    free(msg_p->common.message_set_descr_p);
    free(msg_p);
}

int pscc_init(void *user_p, mpl_log_fp log_fp)
{
    return 0;
}

int sterc_init(void *user_p, mpl_log_fp log_fp)
{
    return 0;
}

int stec_send(struct stec_ctrl *ctrl_p, const char *buf, size_t buf_len)
{
    return 0;
}

int stec_recv(struct stec_ctrl *ctrl_p, char *buf, size_t buf_len,
              int sec_timeout)
{
    return 1;
}

struct stec_ctrl *stec_open(const struct sockaddr *addr_p,
                            socklen_t addr_len) {
    struct stec_ctrl *ctrl_p = NULL;

    if (NULL == addr_p || NULL == ((struct sockaddr_un *)addr_p)->sun_path) {
        goto error;
    }

    ctrl_p = calloc(1, sizeof(struct stec_ctrl));

    if (!strcmp(((struct sockaddr_un *)addr_p)->sun_path, REQUEST_SOCKET_PSCC)) {
        ctrl_p->sock = FD_PSCC_REQUEST;
    } else if (!strcmp(((struct sockaddr_un *)addr_p)->sun_path, EVENT_SOCKET_PSCC)) {
        ctrl_p->sock = FD_PSCC_EVENT;
    } else if (!strcmp(((struct sockaddr_un *)addr_p)->sun_path, REQUEST_SOCKET_STERC)) {
        ctrl_p->sock = FD_STERC_REQUEST;
    } else if (!strcmp(((struct sockaddr_un *)addr_p)->sun_path, EVENT_SOCKET_STERC)) {
        ctrl_p->sock = FD_STERC_EVENT;
    } else {
        free(ctrl_p);
        goto error;
    }


    return ctrl_p;
error:
    return NULL;
}

void stec_close(struct stec_ctrl *ctrl_p)
{
    free(ctrl_p);
}

int stec_subscribe(struct stec_ctrl *ctrl_p)
{
    int res = 0;

    if (!ctrl_p) {
        return -1;
    }

    return 0; /* OK */
}

int stec_unsubscribe(struct stec_ctrl *ctrl_p)
{
    int res = 0;

    if (!ctrl_p) {
        return -1;
    }

    return 0; /* OK */
}

