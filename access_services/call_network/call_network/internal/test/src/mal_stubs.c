/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "mal_call.h"
#include "mal_gss.h"
#include "mal_mce.h"
#include "mal_net.h"
#include "mal_ss.h"
#include "mal_mis.h"
#include "mal_nvd.h"
#include "mal_ftd.h"
#include "mal_rf.h"
#include "mal_utils.h"
#include "shm_netlnk.h"
#include "cn_macros.h"

/* Global variables */
int32_t g_mal_net_set_mode__net_mode = -1;
int32_t g_mal_return_value = 0;
int8_t g_mal_call_request_hangup__call_id = -1;
mal_call_dtmf_info g_mal_call_request_dtmf_send__dtmf_info = { UTF8_STRING, NULL, 0, 0, 0};
mal_gss_network_type g_mal_gss_set_preferred_network_type__type = -1;
mal_net_cs_request_type g_mal_net_control_cs__req_type = 0;
mal_net_nmr_rat_type g_mal_rat_type = MAL_NET_NMR_RAT_GERAN;
mal_net_utran_nmr_type g_mal_nmr_type = MAL_NET_SIM_INTER_RAT_NMR;
mal_net_nw_access_conf_data g_mal_net_nw_access_conf_data = { 0, 0 };
mal_ss_serv_class_info g_mal_ss_serv_class_info = 0;
mal_ss_serv_class_info g_mal_ss_query_serv_class_info = 0;
mal_net_reg_status_set_mode g_mal_net_reg_status_set_mode = 0;
uint32_t  g_mal_ss_forward_status = 0;
uint32_t  g_mal_ss_forward_reason = 0;
uint32_t  g_mal_ss_forward_time = 0;
uint32_t  g_mal_ss_query_forward_status = 0;
uint32_t  g_mal_ss_query_forward_reason = 0;
mal_ss_service_op g_mal_ss_service_op = 0;
uint32_t g_mal_string_length = 0;
uint8_t *g_mal_net_manual_register__mnc_mcc_p = NULL;
uint8_t g_mal_no_of_calls = 0;
void *g_mal_call_list_p = NULL;
void *g_mal_client_tag_p = NULL;
void *g_mal_request_data_p = NULL;
char g_ussd_string[4];
mal_net_ind_type  g_mal_net_set_ind_state_type = 0;
char g_mal_nvd_set_default_called = FALSE;
mal_net_ind_state g_mal_net_set_ind_state_state = 0;
mal_rf_tx_back_off_event g_mal_rf_send_tx_back_off_event = 0;
void clear_global_stub_variables() /* this function is called prior to each testcase */
{
    g_mal_call_list_p = NULL;
    g_mal_call_request_hangup__call_id = -1;
    g_mal_client_tag_p = NULL;
    g_mal_gss_set_preferred_network_type__type = -1;
    g_mal_net_control_cs__req_type = 0;
    free(g_mal_net_manual_register__mnc_mcc_p);
    g_mal_net_manual_register__mnc_mcc_p = NULL;
    g_mal_net_set_mode__net_mode = -1;
    g_mal_nmr_type = MAL_NET_SIM_INTER_RAT_NMR;
    g_mal_no_of_calls = 0;
    g_mal_rat_type = MAL_NET_NMR_RAT_GERAN;
    g_mal_request_data_p = NULL;
    g_mal_return_value = 0;
    g_mal_ss_serv_class_info = 0;
    g_mal_ss_query_serv_class_info = 0;
    g_mal_ss_service_op = 0;
    g_mal_string_length = 0;
    g_mal_ss_forward_status = 0;
    g_mal_ss_forward_reason = 0;
    g_mal_ss_forward_time = 0;
    g_mal_ss_query_forward_status = 0;
    g_mal_ss_query_forward_reason = 0;
    g_mal_net_reg_status_set_mode = 0;
    g_mal_nvd_set_default_called = FALSE;
    g_mal_net_set_ind_state_type = 0;
    g_mal_net_set_ind_state_state = 0;
    g_mal_rf_send_tx_back_off_event = 0;
    free(g_mal_call_request_dtmf_send__dtmf_info.dtmf_string);
    memset(&g_mal_call_request_dtmf_send__dtmf_info, 0, sizeof(g_mal_call_request_dtmf_send__dtmf_info));
    memset(g_ussd_string, 0, sizeof(g_ussd_string));
}

int32_t mal_mce_rf_on(void *client_tag)
{
    g_mal_client_tag_p = client_tag;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


int32_t mal_mce_rf_off(void *client_tag)
{
    g_mal_client_tag_p = client_tag;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


int32_t mal_mce_reset(void *client_tag)
{
    g_mal_client_tag_p = client_tag;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


int32_t mal_mce_rf_status_query(void *client_tag)
{
    g_mal_client_tag_p = client_tag;
    CN_LOG_D("mal stub enter");

    return g_mal_return_value;
}


int32_t mal_gss_set_preferred_network_type(mal_gss_network_type type, void *client_tag)
{
    CN_LOG_D("mal stub enter (type:%d)", type);
    g_mal_gss_set_preferred_network_type__type = type;
    g_mal_client_tag_p = client_tag;
    return g_mal_return_value;
}


int32_t mal_gss_get_preferred_network_type(void *client_tag)
{
    g_mal_client_tag_p = client_tag;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


int32_t mal_net_req_registration_state_normal(void *client_tag)
{
    g_mal_client_tag_p = client_tag;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


int32_t mal_net_req_registration_state_gprs(void *client_tag)
{
    g_mal_client_tag_p = client_tag;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


int32_t mal_net_req_cell_info(void *client_tag)
{
    g_mal_client_tag_p = client_tag;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


int32_t mal_net_set_mode(void *data, void *client_tag)
{
    CN_LOG_D("mal stub enter");

    if (NULL != data)  {
        g_mal_net_set_mode__net_mode = *((int32_t *)data);
    }

    g_mal_client_tag_p = client_tag;

    return g_mal_return_value;
}

int32_t mal_net_register(void *client_tag)
{
    CN_LOG_D("mal stub enter");

    g_mal_client_tag_p = client_tag;

    return g_mal_return_value;
}

int32_t mal_net_manual_register(mal_net_manual_reg_info *reg_info, void *client_tag)
{
    CN_LOG_D("mal stub enter");
    g_mal_net_manual_register__mnc_mcc_p = (uint8_t *)strdup(reg_info->mcc_mnc);
    g_mal_client_tag_p = client_tag;
    return g_mal_return_value;
}

int32_t mal_net_control_cs(mal_net_cs_request_type req, void *client_tag)
{
    CN_LOG_D("mal stub enter");
    g_mal_client_tag_p = client_tag;
    g_mal_net_control_cs__req_type = req;
    return g_mal_return_value;
}


int32_t mal_net_deregister(void *client_tag)
{
    g_mal_client_tag_p = client_tag;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


int32_t mal_net_query_mode(void *client_tag)
{
    g_mal_client_tag_p = client_tag;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


int32_t mal_net_manual_search(void *client_tag)
{
    g_mal_client_tag_p = client_tag;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


int32_t mal_net_interrupt_search(void *client_tag)
{
    g_mal_client_tag_p = client_tag;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


int32_t mal_net_get_rssi(void *client_tag)
{
    g_mal_client_tag_p = client_tag;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}

int32_t mal_call_request_getcalllist(void *data, void **response, void *numOfCalls)
{
    (void) data;
    CN_LOG_D("mal stub enter");

    if (NULL != response) {
        *response = g_mal_call_list_p;
    }

    if (NULL != numOfCalls) {
        *((int32_t *)numOfCalls) = g_mal_no_of_calls;
    }

    return g_mal_return_value;
}


int32_t mal_call_request_freecalllist(void *data)
{
    (void)data;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


int32_t mal_call_request_dialup(void *data, void *client_tag)
{
    CN_LOG_D("mal stub enter");

    if (data && g_mal_request_data_p) {
        mal_call_dailup *data_p = data;
        mal_call_dailup *request_p = (mal_call_dailup *) g_mal_request_data_p;

        if (data_p->phone_number && request_p->phone_number) {
            (void) strncpy((char *) request_p->phone_number, (char *) data_p->phone_number, g_mal_string_length);
            memmove(request_p->bc_data, data_p->bc_data, data_p->bc_length);
        }

        request_p->bc_length = data_p->bc_length;
        request_p->clir = data_p->clir;
        request_p->call_type = data_p->call_type;
    }

    g_mal_client_tag_p = client_tag;
    return g_mal_return_value;
}

int32_t mal_call_request_hangup(void *data, void *client_tag)
{
    CN_LOG_D("mal stub enter");

    if (data) {
        g_mal_call_request_hangup__call_id = *((int8_t *)data);
    }

    g_mal_client_tag_p = client_tag;

    return g_mal_return_value;
}


int32_t mal_call_request_swap_call(void *data, void *client_tag)
{
    CN_LOG_D("mal stub enter");

    if (data && g_mal_request_data_p) {
        *((int32_t *) g_mal_request_data_p) = *((int32_t *) data);
    }

    g_mal_client_tag_p = client_tag;

    return g_mal_return_value;
}


int32_t mal_call_request_hold(void *data, void *client_tag)
{
    CN_LOG_D("mal stub enter");

    if (data && g_mal_request_data_p) {
        *((int32_t *) g_mal_request_data_p) = *((int32_t *) data);
    }

    g_mal_client_tag_p = client_tag;

    return g_mal_return_value;
}


int32_t mal_call_request_resume(void *data, void *client_tag)
{
    CN_LOG_D("mal stub enter");

    if (data && g_mal_request_data_p) {
        *((int32_t *) g_mal_request_data_p) = *((int32_t *) data);
    }

    g_mal_client_tag_p = client_tag;

    return g_mal_return_value;
}


int32_t mal_call_request_conference_call(void *data, void *client_tag)
{
    CN_LOG_D("mal stub enter");

    if (data && g_mal_request_data_p) {
        *((int32_t *) g_mal_request_data_p) = *((int32_t *) data);
    }

    g_mal_client_tag_p = client_tag;

    return g_mal_return_value;
}


int32_t mal_call_request_conference_call_split(void *data, void *client_tag)
{
    CN_LOG_D("mal stub enter");

    if (data && g_mal_request_data_p) {
        *((int32_t *) g_mal_request_data_p) = *((int32_t *) data);
    }

    g_mal_client_tag_p = client_tag;

    return g_mal_return_value;
}


int32_t mal_call_request_explicit_call_transfer(void *data, void *client_tag)
{
    CN_LOG_D("mal stub enter");

    if (data && g_mal_request_data_p) {
        *((int32_t *) g_mal_request_data_p) = *((int32_t *) data);
    }

    g_mal_client_tag_p = client_tag;

    return g_mal_return_value;
}


int32_t mal_call_request_answer(void *data, void *client_tag)
{
    (void)data;
    g_mal_client_tag_p = client_tag;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


int32_t mal_call_request_get_clir(void *client_tag)
{
    g_mal_client_tag_p = client_tag;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


int32_t mal_call_request_set_clir(void *data, void *client_tag)
{
    CN_LOG_D("mal stub enter");

    if (NULL != data) {
        *((uint8_t *) g_mal_request_data_p) = *((uint8_t *) data);
    }

    g_mal_client_tag_p = client_tag;
    return g_mal_return_value;
}


int32_t mal_call_request_modem_property_get(void *property, void *client_tag)
{
    (void)property;
    g_mal_client_tag_p = client_tag;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


int32_t mal_call_request_modem_property_set(void *property, void *data, void *client_tag)
{
    CN_LOG_D("mal stub enter");
    (void)property;

    if (NULL != data) {
        *((mal_call_modem_property *) g_mal_request_data_p) = *((mal_call_modem_property *) data);
    }

    g_mal_client_tag_p = client_tag;
    return g_mal_return_value;
}

int32_t mal_call_request_dtmf_send(void *call_id, void *data, void *client_tag)
{
    CN_LOG_D("mal stub enter");
    (void)call_id;

    if (data) {
        mal_call_dtmf_info *data_p = (mal_call_dtmf_info *)data;
        mal_call_dtmf_info *dtmf_p = &g_mal_call_request_dtmf_send__dtmf_info;

        dtmf_p->string_type        = data_p->string_type;
        dtmf_p->dtmf_duration_time = data_p->dtmf_duration_time;
        dtmf_p->dtmf_pause_time    = data_p->dtmf_pause_time;

        if (data_p->dtmf_string != NULL && data_p->length > 0) {
            int data_len = data_p->length;

            if (data_p->string_type == BCD_STRING) {
                data_len = (data_len + 1) / 2;
            }

            dtmf_p->dtmf_string = (char *)calloc(sizeof(char), data_len + 1);
            memcpy(dtmf_p->dtmf_string, data_p->dtmf_string, data_len);
            CN_LOG_V("copied %d bytes", data_len);

            dtmf_p->length = data_p->length;

        } else {
            CN_LOG_E("dtmf_string=NULL, or length=0!");
            dtmf_p->length = 0;
        }

        CN_LOG_V("dtmf len = %d", dtmf_p->length);
        CN_LOG_V("dtmf type = %d", dtmf_p->string_type);

        if (dtmf_p->string_type == UTF8_STRING) {
            CN_LOG_V("dtmf str = %s", dtmf_p->dtmf_string);
        }

    } else {
        CN_LOG_E("data==NULL!");
    }

    g_mal_client_tag_p = client_tag;
    return g_mal_return_value;
}

int32_t mal_call_request_dtmf_start(void *call_id, void *data, void *client_tag)
{
    CN_LOG_D("mal stub enter");

    if (call_id && data) {
        struct mal_request_data_t {
            char character;
            int32_t call_id;
        };
        ((struct mal_request_data_t *) g_mal_request_data_p)->character = *((char *) data);
        ((struct mal_request_data_t *) g_mal_request_data_p)->call_id = *((int32_t *) call_id);
    }

    g_mal_client_tag_p = client_tag;

    return g_mal_return_value;
}


int32_t mal_call_request_dtmf_stop(void *data, void *client_tag)
{
    CN_LOG_D("mal stub enter");

    if (data) {
        *((int32_t *) g_mal_request_data_p) = *((int32_t *) data);
    }

    g_mal_client_tag_p = client_tag;

    return g_mal_return_value;
}


int32_t mal_ss_request_ussd(void *data, void *client_tag)
{
    mal_ss_ussd_data *stub_data = (mal_ss_ussd_data *) g_mal_request_data_p;

    CN_LOG_D("mal stub enter");

    if (data && g_mal_request_data_p) {
        stub_data->length = ((mal_ss_ussd_data *) data)->length;
        stub_data->dcs = ((mal_ss_ussd_data *) data)->dcs;
        (void) memcpy((void *) g_ussd_string, ((mal_ss_ussd_data *) data)->ussd_str, sizeof(g_ussd_string)); /* Non-zero terminated */
    }

    g_mal_client_tag_p = client_tag;

    return g_mal_return_value;
}


int32_t mal_ss_cancel_ussd(void *client_tag)
{
    g_mal_client_tag_p = client_tag;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


int32_t mal_ss_request_query_clip(void *client_tag)
{
    g_mal_client_tag_p = client_tag;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


int32_t mal_ss_calling_name_presentation_query(void *client_tag)
{
    g_mal_client_tag_p = client_tag;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


int32_t mal_ss_request_query_colr(void *client_tag)
{
    g_mal_client_tag_p = client_tag;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


int32_t mal_ss_set_call_waiting(void *data, void *client_tag)
{
    CN_LOG_D("mal stub enter");

    if (NULL != data) {
        ((int32_t *) g_mal_request_data_p)[0] = ((int32_t *) data)[0];
        ((int32_t *) g_mal_request_data_p)[1] = ((int32_t *) data)[1];

    }

    g_mal_client_tag_p = client_tag;
    return g_mal_return_value;
}


int32_t mal_ss_query_call_waiting(void *data, void *client_tag)
{
    CN_LOG_D("mal stub enter");

    if (NULL != data) {
        *((int32_t *) g_mal_request_data_p) = *((int32_t *) data);
    }

    g_mal_client_tag_p = client_tag;
    return g_mal_return_value;
}


int32_t mal_ss_set_call_forward(void *data, void *client_tag)
{
    (void)data;
    g_mal_client_tag_p = client_tag;
    CN_LOG_D("mal stub enter");
    g_mal_ss_forward_status = ((mal_ss_call_forward_info *) data)->status;
    g_mal_ss_forward_reason = ((mal_ss_call_forward_info *) data)->reason;
    g_mal_ss_forward_time = ((mal_ss_call_forward_info *) data)->timeSeconds;
    g_mal_ss_serv_class_info = ((mal_ss_call_forward_info *) data)->serviceClass;
    return g_mal_return_value;
}


int32_t mal_ss_query_call_forward(void *data, void *client_tag)
{
    (void)data;
    g_mal_client_tag_p = client_tag;
    CN_LOG_D("mal stub enter");
    g_mal_ss_query_forward_status = ((mal_ss_call_forward_info *) data)->status;
    g_mal_ss_query_forward_reason = ((mal_ss_call_forward_info *) data)->reason;
    g_mal_ss_query_serv_class_info = ((mal_ss_call_forward_info *) data)->serviceClass;
    return g_mal_return_value;
}


int32_t mal_ss_set_call_barring(void *str_code, void *svc_op_class, void *pword, void *client_tag)
{
    (void)str_code;
    (void)pword;
    CN_LOG_D("mal stub enter");
    g_mal_ss_serv_class_info = ((mal_ss_service_op_class *) svc_op_class)->service_class;
    g_mal_ss_service_op = ((mal_ss_service_op_class *) svc_op_class)->service_op;
    g_mal_client_tag_p = client_tag;
    return g_mal_return_value;
}


int32_t mal_ss_query_call_barring(void *str_code, void *svc_class, void *client_tag)
{
    (void)str_code;
    (void)svc_class;
    g_mal_client_tag_p = client_tag;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


int32_t mal_ss_change_barring_password(void *str_code, void *old_pword, void *new_pword, void *client_tag)
{
    (void)str_code;
    (void)old_pword;
    (void)new_pword;
    g_mal_client_tag_p = client_tag;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


void mal_ss_response_handler()
{
    CN_LOG_D("mal stub enter");
}


void mal_gss_response_handler()
{
    CN_LOG_D("mal stub enter");
}


void mal_mce_response_handler()
{
    CN_LOG_D("mal stub enter");
}


void mal_net_response_handler()
{
    CN_LOG_D("mal stub enter");
}


int32_t mal_mce_init(int32_t *fd_mce, int32_t *fd_netlnk)
{
    (void)fd_mce;
    CN_LOG_D("mal stub enter");
    *fd_mce = 123;
    *fd_netlnk = 785;
    return g_mal_return_value;
}


int32_t mal_mce_register_callback(mal_mce_event_cb_t event_cb)
{
    (void)event_cb;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


int32_t mal_mce_config()
{
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


int32_t mal_net_init(int32_t *fd_net)
{
    (void)fd_net;
    CN_LOG_D("mal stub enter");
    *fd_net = 124;
    return g_mal_return_value;
}


int32_t mal_net_register_callback(mal_net_event_cb_t event_cb)
{
    (void)event_cb;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


int32_t mal_call_init(int32_t *fd_call)
{
    (void)fd_call;
    CN_LOG_D("mal stub enter");
    *fd_call = 125;
    return g_mal_return_value;
}


int32_t mal_call_register_callback(mal_call_event_cb_t event_cb)
{
    (void)event_cb;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


int32_t mal_call_config()
{
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


int32_t mal_ss_init(int32_t *fd_ss)
{
    (void)fd_ss;
    CN_LOG_D("mal stub enter");
    *fd_ss = 126;
    return g_mal_return_value;
}


int32_t mal_ss_register_callback(mal_ss_event_cb_t event_cb)
{
    (void)event_cb;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


int32_t mal_ss_config()
{
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


int32_t mal_gss_init(int32_t *fd_gss)
{
    (void)fd_gss;
    CN_LOG_D("mal stub enter");
    *fd_gss = 127;
    return g_mal_return_value;
}


int32_t mal_gss_register_callback(mal_gss_event_cb_t event_cb)
{
    (void)event_cb;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


int32_t mal_call_deinit()
{
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


int32_t mal_gss_deinit()
{
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


int32_t mal_net_deinit()
{
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}

int32_t mal_mce_deinit()
{
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


int32_t mal_ss_deinit()
{
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}

int32_t mal_gss_set_environment_information(mal_gss_env_info_t *env_info, void *client_tag)
{
    (void)env_info;
    g_mal_client_tag_p = client_tag;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}

int32_t mal_net_set_control_unsol_neighbour_cell_info(uint8_t mode, mal_net_neighbour_rat_type rat)
{
    (void)mode;
    (void)rat;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}

int32_t mal_net_get_control_unsol_neighbour_cell_info(uint8_t *mode, mal_net_neighbour_rat_type rat)
{
    (void)mode;
    (void)rat;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}

int32_t mal_net_get_neighbour_cell_ids(mal_net_neighbour_rat_type rat, void *client_tag, mal_net_neighbour_request_type request_type)
{
    (void)rat;
    g_mal_client_tag_p = client_tag;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}


int32_t mal_net_configure_network_access(mal_net_nw_access_conf_data *conf_data, void *client_tag)
{
    CN_LOG_D("mal stub enter");

    if (NULL != conf_data) {
        g_mal_net_nw_access_conf_data.reg_conf = conf_data->reg_conf;
        g_mal_net_nw_access_conf_data.roam_conf = conf_data->roam_conf;
    } else {
        CN_LOG_D("conf_data is NULL");
    }

    g_mal_client_tag_p = client_tag;

    return g_mal_return_value;
}


int32_t mal_mis_set_pp_flag(uint16_t flag_id, uint16_t value, void *client_tag)
{
    (void)flag_id;
    (void)value;
    g_mal_client_tag_p = client_tag;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}

int32_t mal_mis_init(int32_t *fd_mis)
{
    (void)fd_mis;
    CN_LOG_D("mal stub enter");
    *fd_mis = 128;
    return g_mal_return_value;
}

int32_t mal_mis_register_callback(mal_mis_event_cb_t event_cb)
{
    (void)event_cb;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}

int32_t mal_mis_deinit()
{
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}
void mal_mis_response_handler()
{
    CN_LOG_D("mal stub enter");
}


void mal_call_response_handler()
{
    CN_LOG_D("mal stub enter");
}

int32_t mal_call_set_supp_svc_notification(void *data)
{
    (void)data;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}

int32_t mal_net_set_ind_state(mal_net_ind_type ind_type, mal_net_ind_state ind_state)
{
    g_mal_net_set_ind_state_type  = ind_type;
    g_mal_net_set_ind_state_state = ind_state;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}

int32_t mal_net_get_rab_status(mal_net_rab_status_type *rab_status)
{
    (void)rab_status;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}

int32_t mal_mis_req_baseband_version(void *client_tag)
{
    CN_LOG_D("mal stub enter");
    g_mal_client_tag_p = client_tag;
    return g_mal_return_value;
}

int32_t mal_mis_req_pp_flags(uint8_t all, uint16_t flag_id, void *client_tag)
{
    CN_LOG_D("mal stub enter");
    g_mal_client_tag_p = client_tag;
    return g_mal_return_value;
}
int32_t mal_call_request_modem_emerg_number_modify(void *data, void *client_tag)
{
    (void)data;
    CN_LOG_D("mal stub enter");
    g_mal_client_tag_p = client_tag;
    return g_mal_return_value;
}

int32_t mal_call_request_get_emerg_num_list(void *client_tag)
{
    CN_LOG_D("mal stub enter");
    g_mal_client_tag_p = client_tag;
    return g_mal_return_value;
}

int32_t mal_net_get_encsq_mode(uint8_t *encsq_mode)
{
    (void)encsq_mode;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}

int32_t mal_net_configure_rssi(mal_net_rssi_conf_data *data, void *client_tag)
{
    (void)data;
    CN_LOG_D("mal stub enter");
    g_mal_client_tag_p = client_tag;
    return g_mal_return_value;
}

int32_t mal_net_get_rssi_conf_values(mal_net_rssi_conf_data *rssi_conf_data)
{
    (void)rssi_conf_data;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}

int32_t mal_net_enable_disable_rssi(uint8_t encsq_mode, void *client_tag)
{
    (void)encsq_mode;
    CN_LOG_D("mal stub enter");
    g_mal_client_tag_p = client_tag;
    return g_mal_return_value;
}


int32_t mal_net_req_nmr_info(mal_net_nmr_rat_type rat_type,
                             mal_net_utran_nmr_type nmr_type, void *client_tag)
{
    CN_LOG_D("mal stub enter");
    g_mal_client_tag_p = client_tag;

    if (g_mal_rat_type != rat_type) {
        return MAL_NET_INVALID_RAT_TYPE;
    }

    if (g_mal_nmr_type != nmr_type) {
        return MAL_NET_INVALID_DATA;
    }

    return g_mal_return_value;
}

int32_t mal_gss_get_timing_advance_value(void *client_tag)
{
    CN_LOG_D("mal stub enter");
    g_mal_client_tag_p = client_tag;
    return g_mal_return_value;
}

int32_t mal_gss_set_hsxpa_mode(mal_gss_hsxpa_mode mode, void *client_tag)
{
    (void)mode;
    CN_LOG_D("mal stub enter");
    g_mal_client_tag_p = client_tag;
    return g_mal_return_value;
}

int32_t mal_gss_get_hsxpa_mode(void *client_tag)
{
    CN_LOG_D("mal stub enter");
    g_mal_client_tag_p = client_tag;
    return g_mal_return_value;
}

int32_t mal_ss_request_get_clir(void *client_tag)
{
    CN_LOG_D("mal stub enter");
    g_mal_client_tag_p = client_tag;
    return g_mal_return_value;
}

int32_t netlnk_socket_recv(int32_t netlnkfd, int *msg)
{
    CN_LOG_D("mal stub enter");
    (void)netlnkfd;
    (void)msg;
    return 0;
}

int32_t mal_net_configure_reg_status(mal_net_reg_status_set_mode reg_status_mode, void *client_tag)
{
    CN_LOG_D("mal stub enter");

    g_mal_net_reg_status_set_mode = reg_status_mode;
    g_mal_client_tag_p = client_tag;

    return g_mal_return_value;
}

int32_t mal_net_req_current_rat_name(void *client_tag)
{
    CN_LOG_D("mal stub enter");

    g_mal_client_tag_p = client_tag;

    return g_mal_return_value;
}

int32_t mal_mce_modem_switch_off(void *client_tag)
{
    CN_LOG_D("mal stub enter");

    g_mal_client_tag_p = client_tag;

    return g_mal_return_value;
}

void mal_rf_response_handler()
{
    CN_LOG_D("mal stub enter");
}

int32_t mal_rf_init(int32_t *fd_rf_p)
{
    CN_LOG_D("mal stub enter");

    if (fd_rf_p) {
        *fd_rf_p = 789;
    }

    return MAL_RF_SUCCESS;
}

int32_t mal_rf_register_callback(mal_rf_event_cb_t event_cb)
{
    (void)event_cb;

    CN_LOG_D("mal stub enter");

    return MAL_RF_SUCCESS;
}

int32_t mal_rf_config()
{
    CN_LOG_D("mal stub enter");

    return MAL_RF_SUCCESS;
}

int32_t mal_rf_deinit()
{
    CN_LOG_D("mal stub enter");

    return MAL_RF_SUCCESS;
}

int32_t mal_rf_send_tx_back_off_event(mal_rf_tx_back_off_event event, void *client_tag)
{
    CN_LOG_D("mal stub enter");

    (void)event;

    g_mal_rf_send_tx_back_off_event = event;
    g_mal_client_tag_p = client_tag;

    return g_mal_return_value;
}
void mal_nvd_response_handler()
{
    CN_LOG_D("mal stub enter");
}

int32_t mal_nvd_init(int32_t *fd_nvd)
{
    (void)fd_nvd;
    CN_LOG_D("mal stub enter");
    *fd_nvd = 887;
    return g_mal_return_value;
}

int32_t mal_nvd_deinit()
{
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}

int32_t mal_nvd_register_callback(mal_nvd_event_cb_t event_cb)
{
    (void)event_cb;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}

int32_t mal_nvd_set_default(void *client_tag)
{
    CN_LOG_D("mal stub enter");
    g_mal_client_tag_p = client_tag;
    g_mal_nvd_set_default_called = TRUE;
    return g_mal_return_value;
}

int32_t mal_ftd_init(int32_t *fd_ftd)
{
    (void)fd_ftd;
    CN_LOG_D("mal stub enter");
    *fd_ftd = 129;
    return g_mal_return_value;
}

int32_t mal_ftd_deinit()
{
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}

int32_t mal_ftd_register_callback(mal_ftd_event_cb_t event_cb)
{
    (void)event_cb;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}

int32_t mal_ftd_activate_request(int32_t page_id, uint8_t mode)
{
    (void) page_id;
    (void) mode;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}

int32_t mal_ftd_deactivate_request(int32_t page_id)
{
    (void) page_id;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}

int32_t mal_ftd_stop_dump_once_report(int32_t page_id)
{
    (void) page_id;
    CN_LOG_D("mal stub enter");
    return g_mal_return_value;
}

void mal_ftd_response_handler(int32_t dgram)
{
    (void) dgram;
    CN_LOG_D("mal stub enter");
}


