/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : cn_client.c
 * Description     : Functionality for connecting and communicating with
 *                   the Call and Network (CN) service.
 *
 * Author          : Johan Norberg   <johan.xx.norberg@stericsson.com>
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "cn_client.h"
#include "cn_log.h"
#include "wb_test.h"
#include "whitebox.h"

#define NETWORK_LIST_LEN 4
#define NET_MCC_MNC_STRING_LEN 8 /* CN_MCC_MNC_STRING_LENGTH */

static int cn_client_connect(const char *path);
static void init_dummy_call_context(cn_call_context_t *context_p);

static const uint8_t no_name[] = { 0, 'N', 0, 'o', 0, ' ', 0, 'N', 0, 'a', 0, 'm', 0, 'e', 0, 0};

static cn_client_tag_t ct = 0;
int state = 0;

cn_error_code_t cn_socket_send(int fd, void *data_p, int size)
{
    return CN_SUCCESS;
}

cn_message_t *create_message(cn_message_type_t type, cn_error_code_t error_code, cn_client_tag_t client_tag, cn_uint32_t payload_size, void *payload_p)
{
    cn_message_t *message_p = NULL;
    int msg_size = 0;
    int result = -1;

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

cn_error_code_t cn_message_receive(int fd, cn_uint32_t *size_p, cn_message_t **msg_pp)
{
    cn_error_code_t result = CN_SUCCESS;

    if (!msg_pp) {
        printf("%s: msg_pp is NULL!\n", __FUNCTION__);
        goto error;
    }

    if (!size_p) {
        printf("%s: size_p is NULL!\n", __FUNCTION__);
        goto error;
    }

    switch (CURRENT_TEST_CASE) {
        /* D */
    case wb_testcase_d:
        *msg_pp = create_message(CN_RESPONSE_USSD, CN_SUCCESS, ct, 0, NULL);
        break;

    case wb_testcase_d_clir_invoke:
    case wb_testcase_d_clir_suppress:
        *msg_pp = create_message(CN_RESPONSE_USSD, CN_SUCCESS, ct, 0, NULL);
        break;

    case wb_testcase_z_set:
    case wb_testcase_z_do_0:
    case wb_testcase_sequence: {
        cn_call_forward_info_list_t *list_p;
        int size = sizeof(cn_call_forward_info_list_t);
        list_p = calloc(1, size);
        list_p->num = 0;
        *msg_pp = create_message(CN_RESPONSE_QUERY_CALL_FORWARD, CN_SUCCESS, ct, size, list_p);
        free(list_p);
    }
    break;

    case wb_testcase_z_do_1: {
        cn_call_list_t *call_list_p = NULL;
        cn_sint32_t nr_of_calls = 1;
        cn_sint32_t size = sizeof(cn_call_list_t);
        cn_sint32_t i;

        size += sizeof(cn_call_context_t) * (nr_of_calls - 1);
        call_list_p = calloc(1, size);

        switch (state) {
        case 0:
            call_list_p->nr_of_calls = nr_of_calls;

            for (i = 0; i < nr_of_calls; i++) {
                call_list_p->call_context[i].call_state = CN_CALL_STATE_ACTIVE;
                call_list_p->call_context[i].call_id           = i + 1;
                call_list_p->call_context[i].address_type      = 145;
                call_list_p->call_context[i].is_MT             = 1;
                call_list_p->call_context[i].mode              = CN_CALL_MODE_SPEECH;
                call_list_p->call_context[i].call_cause.cause_type_sender = CN_CALL_CAUSE_TYPE_SENDER_NONE;
                (void)strncpy(call_list_p->call_context[i].number, "1234", CN_MAX_STRING_SIZE);
                (void)memcpy(call_list_p->call_context[i].name, no_name, sizeof(no_name));
            }

            break;
        case 1:
        case 2:
            call_list_p->nr_of_calls = 0;
            break;
        }

        *msg_pp = create_message(CN_RESPONSE_CURRENT_CALL_LIST, CN_SUCCESS, ct, size, call_list_p);
        free(call_list_p);
    }
    break;
    case wb_testcase_z_do_2: {
        cn_call_list_t *call_list_p = NULL;
        cn_sint32_t nr_of_calls = 5;
        cn_sint32_t size = sizeof(cn_call_list_t);
        cn_sint32_t i;

        size += sizeof(cn_call_context_t) * (nr_of_calls - 1);
        call_list_p = calloc(1, size);

        switch (state) {
        case 0:
            call_list_p->nr_of_calls = 2;
            break;
        case 1:
            call_list_p->nr_of_calls = 0;
            break;
        case 2:
            call_list_p->nr_of_calls = 1;
            break;
        case 3:
            call_list_p->nr_of_calls = 0;
            break;
        case 4:
            call_list_p->nr_of_calls = 0;
            break;
        }

        for (i = 0; i < 5; i++) {
            call_list_p->call_context[i].call_state = CN_CALL_STATE_ACTIVE;
            call_list_p->call_context[i].call_id           = i + 1;
            call_list_p->call_context[i].address_type      = 145;
            call_list_p->call_context[i].is_MT             = 1;
            call_list_p->call_context[i].mode              = CN_CALL_MODE_SPEECH;
            call_list_p->call_context[i].call_cause.cause_type_sender = CN_CALL_CAUSE_TYPE_SENDER_NONE;
            (void)strncpy(call_list_p->call_context[i].number, "1234", CN_MAX_STRING_SIZE);
            (void)memcpy(call_list_p->call_context[i].name, no_name, sizeof(no_name));
        }

        *msg_pp = create_message(CN_RESPONSE_QUERY_CALL_FORWARD, CN_SUCCESS, ct, size, call_list_p);
        free(call_list_p);
    }
    break;
    /* +CCFC */
    case wb_testcase_ccfc_set_query: {
        cn_call_forward_info_list_t *list_p;
        int num, size;

        switch (state) {
        case 0:
            num = 3;
            size = sizeof(cn_call_forward_info_list_t) +
                   sizeof(cn_call_forward_info_t) * (num - 1);
            list_p = calloc(1, size);
            list_p->num = num;

            list_p->call_fwd_info[0].status       = EXE_CCFC_MODE_ENABLE;
            list_p->call_fwd_info[0].reason       = EXE_CCFC_REASON_MOBILE_BUSY;
            list_p->call_fwd_info[0].service_class = EXE_CLASSX_VOICE;
            list_p->call_fwd_info[0].toa          = 145;
            strcpy(list_p->call_fwd_info[0].number, "+01555123123");
            list_p->call_fwd_info[0].time_seconds  = EXE_CCFC_TIME_DEFAULT + 10;

            list_p->call_fwd_info[1].status       = EXE_CCFC_STATUS_ENABLED;
            list_p->call_fwd_info[1].reason       = EXE_CCFC_REASON_NO_REPLY;
            list_p->call_fwd_info[1].service_class = EXE_CLASSX_DATA;
            list_p->call_fwd_info[1].toa          = 129;
            strcpy(list_p->call_fwd_info[1].number, "555234234");
            list_p->call_fwd_info[1].time_seconds  = EXE_CCFC_TIME_DEFAULT - 5;

            list_p->call_fwd_info[2].status       = EXE_CCFC_STATUS_DISABLED;
            list_p->call_fwd_info[2].reason       = EXE_CCFC_REASON_UNCONDITIONAL;
            list_p->call_fwd_info[2].service_class = EXE_CLASSX_FAX;
            list_p->call_fwd_info[2].toa          = 161;
            list_p->call_fwd_info[2].number[0]    = '\0';
            list_p->call_fwd_info[2].time_seconds  = EXE_CCFC_TIME_DEFAULT;
            break;
        case 1:
            num = 0;
            size = sizeof(cn_call_forward_info_list_t);
            list_p = calloc(1, size);
            list_p->num = num;
            break;
        default:
            break;
        }

        *msg_pp = create_message(CN_RESPONSE_QUERY_CALL_FORWARD, CN_SUCCESS, ct, size, list_p);
        free(list_p);
        break;
    }
    case wb_testcase_ccfc_set_register:

        switch (state) {
        case 0:
            *msg_pp = create_message(CN_RESPONSE_SET_CALL_FORWARD, CN_SUCCESS, ct, 0, NULL);
            break;
        case 1:
            *msg_pp = create_message(CN_RESPONSE_SET_CALL_FORWARD, CN_REQUEST_NOT_SUPPORTED, ct, 0, NULL);
            break;
        case 2:
            *msg_pp = create_message(CN_RESPONSE_SET_CALL_FORWARD, CN_FAILURE, ct, 0, NULL);
            break;
        default:
            break;
        }

        break;
    case wb_testcase_ccfc_set_1:
        *msg_pp = create_message(CN_RESPONSE_SET_CALL_FORWARD, CN_SUCCESS, ct, 0, NULL);
        break;

        /* +CCWA */
    case wb_testcase_ccwa_unsol_1:
    case wb_testcase_ccwa_unsol_2: {
        cn_call_context_t call_context;
        memset(&call_context, 0, sizeof(call_context));

        if (CURRENT_TEST_CASE == wb_testcase_ccwa_unsol_1) {
            strcpy(call_context.number, "+4612345678");
        } else if (CURRENT_TEST_CASE == wb_testcase_ccwa_unsol_2) {
            strcpy(call_context.number, "4612345678");
        }

        call_context.address_type = 145;
        *msg_pp = create_message(CN_EVENT_RING_WAITING, CN_SUCCESS, 0, sizeof(call_context), &call_context);
        break;
    }
    case wb_testcase_ccwa_set1:
    case wb_testcase_ccwa_set2:
    case wb_testcase_ccwa_set3:
    case wb_testcase_ccwa_set4:
        *msg_pp = create_message(CN_RESPONSE_GET_CALL_WAITING, CN_SUCCESS, ct, 0, NULL);
        break;
    case wb_testcase_ccwa_set_query: {
        cn_call_waiting_t call_waiting;
        call_waiting.setting = CN_CALL_WAITING_SERVICE_ENABLED;
        call_waiting.service_class = 5; /* classinfo (status) */
        *msg_pp = create_message(CN_RESPONSE_GET_CALL_WAITING, CN_SUCCESS, ct, sizeof(call_waiting), &call_waiting);
        break;
    }
    case wb_testcase_ccwa_query_fail1:
        /* Tests FDN blocked response */
        *msg_pp = create_message(CN_RESPONSE_GET_CALL_WAITING, CN_REQUEST_FDN_BLOCKED, ct, 0, NULL);
        break;
    case wb_testcase_ccwa_query_fail2:
        /* Tests failure response */
        *msg_pp = create_message(CN_RESPONSE_GET_CALL_WAITING, CN_FAILURE, ct, 0, NULL);
        break;
    case wb_testcase_ccwa_query_fail3:
        /* Tests success response, but without an response data */
        *msg_pp = create_message(CN_RESPONSE_GET_CALL_WAITING, CN_SUCCESS, ct, 0, NULL);
        break;
    case wb_testcase_cuus1_unsol_1: {
        cn_call_context_t call_context;
        char user_user[2] = {0xAB, 0xBA};
        memset(&call_context, 0, sizeof(call_context));
        call_context.call_state        = CN_CALL_STATE_ACTIVE;
        call_context.prev_call_state   = CN_CALL_STATE_IDLE;
        call_context.call_id           = 1;
        call_context.address_type      = 145;
        call_context.is_MT             = 1;
        call_context.mode              = CN_CALL_MODE_SPEECH;
        strcpy(call_context.number, PHONE_NUMBER_INT);
        call_context.number_presentation = CN_PRESENTATION_RESTRICTED;
        memcpy(call_context.name, no_name, sizeof(no_name));
        call_context.name_presentation  = CN_PRESENTATION_RESTRICTED;
        call_context.call_cause.cause_type_sender = CN_CALL_CAUSE_TYPE_SENDER_NONE;
        call_context.call_cause.cause = (cn_call_cause_cause_t)0;
        call_context.user_to_user_len = 2;
        call_context.user_to_user_type = 1;
        memmove(call_context.user_to_user, user_user, 2);
        *msg_pp = create_message(CN_EVENT_RING, CN_SUCCESS, ct, sizeof(cn_call_context_t), &call_context);
        break;
    }

    /* *ECAV */
    case wb_testcase_ecav: {
        cn_call_context_t call_context;
        memset(&call_context, 0, sizeof(call_context));
        call_context.call_state        = CN_CALL_STATE_ACTIVE;
        call_context.prev_call_state   = CN_CALL_STATE_IDLE;
        call_context.call_id           = 1;
        call_context.address_type      = 145;
        call_context.is_MT             = 0;
        call_context.mode              = CN_CALL_MODE_SPEECH;
        strcpy(call_context.number, PHONE_NUMBER_INT);
        call_context.number_presentation = CN_PRESENTATION_RESTRICTED;
        memcpy(call_context.name, no_name, sizeof(no_name));
        call_context.name_presentation  = CN_PRESENTATION_RESTRICTED;
        call_context.call_cause.cause_type_sender = CN_CALL_CAUSE_TYPE_SENDER_NONE;
        call_context.call_cause.cause = (cn_call_cause_cause_t)0;

        *msg_pp = create_message(CN_EVENT_CALL_STATE_CHANGED, CN_SUCCESS, ct, sizeof(cn_call_context_t), &call_context);
        break;
    }

    /* +CEER */
    case wb_testcase_ceer_d_busy:
    case wb_testcase_ceer_d_fcn_bad: {
        cn_call_context_t context_local;
        init_dummy_call_context(&context_local);

        if (wb_testcase_ceer_d_busy == CURRENT_TEST_CASE) {
            context_local.call_cause.cause_type_sender = CN_CALL_CAUSE_TYPE_SENDER_NETWORK; /* 3 */
            context_local.call_cause.cause = CN_CALL_NETWORK_CAUSE_USER_BUSY; /* 17 */
        } else if (wb_testcase_ceer_d_fcn_bad == CURRENT_TEST_CASE) {
            context_local.call_cause.cause_type_sender = CN_CALL_CAUSE_TYPE_SENDER_SERVER; /* 2 */
            context_local.call_cause.cause = CN_CALL_SERVER_CAUSE_FDN_NOT_OK; /* 20 */
        } else {
            assert(0);
        }

        switch (state) {
        case 0:
            *msg_pp = create_message(CN_RESPONSE_DIAL, CN_FAILURE, ct, sizeof(context_local.call_cause), &context_local.call_cause);
            break;
        case 1:
            *msg_pp = create_message(CN_EVENT_CALL_STATE_CHANGED, CN_SUCCESS, ct, sizeof(context_local), &context_local);
            break;
        default:
            printf("line %d: wb_testcase_ceer_d_busy, bad state!\n", __LINE__);
            break;
        }

        break;
    }

    case wb_testcase_ceer_disc_normal:
    case wb_testcase_ceer_disc_channel_lost: {
        cn_call_context_t context_local;
        init_dummy_call_context(&context_local);

        if (wb_testcase_ceer_disc_normal == CURRENT_TEST_CASE) {
            context_local.call_cause.cause_type_sender = CN_CALL_CAUSE_TYPE_SENDER_NETWORK; /* 3 */
            context_local.call_cause.cause = CN_CALL_NETWORK_CAUSE_NORMAL; /* 16 */
        } else if (wb_testcase_ceer_disc_channel_lost == CURRENT_TEST_CASE) {
            context_local.call_cause.cause_type_sender = CN_CALL_CAUSE_TYPE_SENDER_SERVER; /* 2 */
            context_local.call_cause.cause = CN_CALL_SERVER_CAUSE_CHANNEL_LOSS; /* 20 */
        } else {
            assert(0);
        }

        switch (state) {
        case 0: {
            *msg_pp = create_message(CN_EVENT_CALL_STATE_CHANGED, CN_SUCCESS, ct, sizeof(context_local), &context_local);
            break;
        }
        default:
            printf("line %d: wb_testcase_ceer_disc_normal, bad state!\n", __LINE__);
            break;
        }

        break;
    }


    case wb_testcase_ceer_a_barred_cug: {
        cn_call_context_t context_local;
        init_dummy_call_context(&context_local);

        if (wb_testcase_ceer_a_barred_cug == CURRENT_TEST_CASE) {
            context_local.call_cause.cause_type_sender = CN_CALL_CAUSE_TYPE_SENDER_NETWORK; /* 3 */
            context_local.call_cause.cause = CN_CALL_NETWORK_CAUSE_COMING_BARRED_CUG; /* 55 */
        } else {
            assert(0);
        }

        switch (state) {
        case 0: {
            cn_call_list_t *call_list_p;
            int nr_of_calls = 0;
            int payload_size = sizeof(cn_call_list_t);

            if (wb_testcase_ceer_a_barred_cug == CURRENT_TEST_CASE) {
                nr_of_calls = 1;
            } else {
                assert(0);
            }

            if (nr_of_calls > 0) {
                payload_size += sizeof(cn_call_context_t) * (nr_of_calls - 1);
            }

            call_list_p = calloc(1, payload_size);

            if (call_list_p) {
                call_list_p->nr_of_calls = 1;
                init_dummy_call_context(call_list_p->call_context);
                call_list_p->call_context[0].call_state = CN_CALL_STATE_INCOMING;
                strncpy(call_list_p->call_context[0].number, PHONE_NUMBER_INT, sizeof(PHONE_NUMBER_INT));
            } else {
                payload_size = 0;
            }

            *msg_pp = create_message(CN_RESPONSE_CURRENT_CALL_LIST, CN_SUCCESS, ct, payload_size, call_list_p);

            if (call_list_p) {
                free(call_list_p);
            }

            break;
        }
        case 1: {
            *msg_pp = create_message(CN_RESPONSE_ANSWER_CALL, CN_FAILURE, ct, sizeof(context_local.call_cause), &context_local.call_cause);
            break;
        }
        default:
            printf("line %d: wb_testcase_ceer_a_barred_cug, bad state!\n", __LINE__);
            break;
        }

        break;
    }

    case wb_testcase_ceer_unsol_gsm:
    case wb_testcase_ceer_unsol_ss_fail: {
        cn_net_detailed_fail_cause_t error_cause;

        if (wb_testcase_ceer_unsol_gsm == CURRENT_TEST_CASE) {
            error_cause.class = CN_NET_DETAILED_FAIL_CAUSE_CLASS_NET; /* 0 */
            error_cause.cause = CN_NET_GSM_CAUSE_MS_ID_CANNOT_BE_DERIVED; /* 9 */
        } else if (wb_testcase_ceer_unsol_ss_fail == CURRENT_TEST_CASE) {
            error_cause.class = CN_NET_DETAILED_FAIL_CAUSE_CLASS_SS; /* 2 this class should not go to log */
            error_cause.cause = CN_NET_SS_CAUSE_ILLEGAL_SS_OPERATION; /* 16 */
        } else {
            assert(0);
        }

        switch (state) {
        case 0: {
            *msg_pp = create_message(CN_EVENT_NET_DETAILED_FAIL_CAUSE, CN_SUCCESS, ct, sizeof(error_cause), &error_cause);
            break;
        }
        default:
            printf("line %d: wb_testcase_ceer_unsol_gsm, bad state!\n", __LINE__);
            break;
        }

        break;
    }

    /* +CFUN */
    case wb_testcase_cfun_1: /* CFUN=99 */

        switch (state) {
        case 0: {
            cn_modem_status_t modem_status = CN_MODEM_STATUS_OK;
            *msg_pp = create_message(CN_RESPONSE_RF_ON, CN_SUCCESS, ct, sizeof(modem_status), &modem_status);
            break;
        }
        case 1: {
            *msg_pp = create_message(CN_RESPONSE_SET_PREFERRED_NETWORK_TYPE, CN_SUCCESS, ct, 0, NULL);
            break;
        }
        default:
            printf("%s: wb_testcase_cfun_1, bad state=%d!\n", __FUNCTION__, state);
            break;
        }

        break;
    case wb_testcase_cfun_2: /* CFUN=100 */
        *msg_pp = create_message(CN_RESPONSE_MODEM_POWER_OFF, CN_SUCCESS, ct, 0, NULL);
        break;
    case wb_testcase_cfun_3: /* CFUN=5 */
        *msg_pp = create_message(CN_RESPONSE_SET_PREFERRED_NETWORK_TYPE, CN_SUCCESS, ct, 0, NULL);
        break;
    case wb_testcase_cfun_4: /* CFUN=6 */
        *msg_pp = create_message(CN_RESPONSE_SET_PREFERRED_NETWORK_TYPE, CN_SUCCESS, ct, 0, NULL);
        break;
    case wb_testcase_cfun_rf_off: { /* CFUN? */
        cn_rf_status_t rf_status = CN_RF_OFF;
        *msg_pp = create_message(CN_RESPONSE_RF_STATUS, CN_SUCCESS, ct, sizeof(rf_status), &rf_status);
        break;
    }
    case wb_testcase_cfun_rf_transition: { /* CFUN? */
        cn_rf_status_t rf_status = CN_RF_TRANSITION;
        *msg_pp = create_message(CN_RESPONSE_RF_STATUS, CN_SUCCESS, ct, sizeof(rf_status), &rf_status);
        break;
    }
    case wb_testcase_cfun_rf_preferred_wcdma: /* CFUN? */
    case wb_testcase_cfun_rf_gsm_only:
    case wb_testcase_cfun_rf_wcdma_only:
    case wb_testcase_cfun_rf_gsm_wcdma_auto:
    case wb_testcase_cfun_rf_cdma_evdo_auto:
    case wb_testcase_cfun_rf_cdma_only:
    case wb_testcase_cfun_rf_evdo_only:
    case wb_testcase_cfun_rf_all_auto:

        switch (state) {
        case 0: {
            cn_rf_status_t rf_status = CN_RF_ON;
            *msg_pp = create_message(CN_RESPONSE_RF_STATUS, CN_SUCCESS, ct, sizeof(rf_status), &rf_status);
            break;
        }
        case 1: {
            cn_network_type_t network_type = -1;

            switch (CURRENT_TEST_CASE) {
            case wb_testcase_cfun_rf_preferred_wcdma: /* CFUN? */
                network_type = CN_NETWORK_TYPE_GSM_WCDMA_PREFERRED_WCDMA;
                break;
            case wb_testcase_cfun_rf_gsm_only:
                network_type = CN_NETWORK_TYPE_GSM_ONLY;
                break;
            case wb_testcase_cfun_rf_wcdma_only:
                network_type = CN_NETWORK_TYPE_WCDMA_ONLY;
                break;
            case wb_testcase_cfun_rf_gsm_wcdma_auto:
                network_type = CN_NETWORK_TYPE_GSM_WCDMA_AUTO_MODE;
                break;
            case wb_testcase_cfun_rf_cdma_evdo_auto:
                network_type = CN_NETWORK_TYPE_CDMA_EVDO_AUTO_MODE;
                break;
            case wb_testcase_cfun_rf_cdma_only:
                network_type = CN_NETWORK_TYPE_CDMA_ONLY;
                break;
            case wb_testcase_cfun_rf_evdo_only:
                network_type = CN_NETWORK_TYPE_EVDO_ONLY;
                break;
            case wb_testcase_cfun_rf_all_auto:
                network_type = CN_NETWORK_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO_MODE;
                break;
            }

            *msg_pp = create_message(CN_RESPONSE_GET_PREFERRED_NETWORK_TYPE, CN_SUCCESS, ct, sizeof(network_type), &network_type);
            break;
        }
        default:
            printf("%s: wb_testcase_cfun_*, bad state=%d!\n", __FUNCTION__, state);
            break;
        }

        break;
    case wb_testcase_cfun_read_failure: {
        cn_rf_status_t rf_status = CN_RF_ON;
        *msg_pp = create_message(CN_RESPONSE_RF_STATUS, CN_SUCCESS, ct, sizeof(rf_status), &rf_status);
        break;
    }

    /* ATD responses BUSY / NO CARRIER / NO DIALTONE / NO ANSWER */
    case wb_testcase_d_busy:
    case wb_testcase_d_no_answer:
    case wb_testcase_d_no_response:
    case wb_testcase_d_no_dialtone: {
        cn_call_context_t call_context;
        memset(&call_context, 0, sizeof(call_context));
        call_context.call_state        = CN_CALL_STATE_IDLE;
        call_context.prev_call_state   = CN_CALL_STATE_ALERTING;
        call_context.call_id           = 1;
        call_context.address_type      = 145;
        call_context.is_MT             = 0;
        call_context.mode              = CN_CALL_MODE_SPEECH;
        strcpy(call_context.number, PHONE_NUMBER_INT);
        call_context.number_presentation = CN_PRESENTATION_RESTRICTED;
        memcpy(call_context.name, no_name, sizeof(no_name));
        call_context.name_presentation = CN_PRESENTATION_RESTRICTED;

        switch (CURRENT_TEST_CASE) {
        case wb_testcase_d_busy:
            call_context.call_cause.cause_type_sender = CN_CALL_CAUSE_TYPE_SENDER_NETWORK;
            call_context.call_cause.cause = CN_CALL_NETWORK_CAUSE_USER_BUSY;
            break;
        case wb_testcase_d_no_answer:
            call_context.call_cause.cause_type_sender = CN_CALL_CAUSE_TYPE_SENDER_NETWORK;
            call_context.call_cause.cause = CN_CALL_NETWORK_CAUSE_ALERT_NO_ANSWER;
            break;
        case wb_testcase_d_no_response:
            call_context.call_cause.cause_type_sender = CN_CALL_CAUSE_TYPE_SENDER_SERVER;
            call_context.call_cause.cause = CN_CALL_SERVER_CAUSE_NO_SERVICE;
            break;
        case wb_testcase_d_no_dialtone:
            call_context.call_cause.cause_type_sender = CN_CALL_CAUSE_TYPE_SENDER_SERVER;
            call_context.call_cause.cause = CN_CALL_SERVER_CAUSE_CS_INACTIVE;
            break;
        default:
            printf("%s: wb_testcase_d_*, bad state=%d!\n", __FUNCTION__, state);
            break;
        }

        *msg_pp = create_message(CN_EVENT_CALL_STATE_CHANGED, CN_SUCCESS, ct, sizeof(cn_call_context_t), &call_context);

        break;
    }
    case wb_testcase_d_failure_1:
        *msg_pp = create_message(CN_RESPONSE_DIAL, CN_REQUEST_CC_SERVICE_MODIFIED_USSD, ct, 0, NULL);
        break;
        /* +CREG */
    case wb_testcase_creg_reg_presentation_1:
    case wb_testcase_creg_reg_presentation_2:
    case wb_testcase_creg_reg_loc_presentation_1:
    case wb_testcase_creg_reg_loc_presentation_2:
    case wb_testcase_creg_reg_loc_presentation_3:
    case wb_testcase_creg_unsolicited_1:
    case wb_testcase_creg_unsolicited_2:
    case wb_testcase_creg_unsolicited_3:
    case wb_testcase_creg_unsolicited_4:
    case wb_testcase_creg_unsolicited_5:
        /* *EREG */
    case wb_testcase_ereg_reg_presentation_1:
    case wb_testcase_ereg_reg_presentation_2:
    case wb_testcase_ereg_reg_ext_presentation_1:
    case wb_testcase_ereg_reg_ext_presentation_2:
    case wb_testcase_ereg_reg_ext_presentation_3:
    case wb_testcase_ereg_unsolicited_1:
    case wb_testcase_ereg_unsolicited_2:
    case wb_testcase_ereg_unsolicited_3:
    case wb_testcase_ereg_unsolicited_4:
    case wb_testcase_ereg_unsolicited_5:
        /* +CGREG */
    case wb_testcase_cgreg_reg_presentation_1:
    case wb_testcase_cgreg_reg_presentation_2:
    case wb_testcase_cgreg_reg_loc_presentation_1:
    case wb_testcase_cgreg_reg_loc_presentation_2:
    case wb_testcase_cgreg_reg_loc_presentation_3:
    case wb_testcase_cgreg_unsolicited_1:
    case wb_testcase_cgreg_unsolicited_2:
    case wb_testcase_cgreg_unsolicited_3:
    case wb_testcase_cgreg_unsolicited_4:
    case wb_testcase_cgreg_unsolicited_5: {
        if (0 == state) { /* testcase state SET */
            printf("creating message CN_RESPONSE_REG_STATUS_EVENT_CONFIG, CN_SUCCESS\n");
            *msg_pp = create_message(CN_RESPONSE_REG_STATUS_EVENT_CONFIG, CN_SUCCESS, ct, 0, NULL);
            break;
        } else { /* testcase state READ */
            cn_registration_info_t reg_info;
            reg_info.reg_status = CN_REGISTERED_ROAMING;
            reg_info.gprs_attached = 1; /* used by +cgreg only */
            reg_info.cs_attached = 1;
            reg_info.rat = CN_RAT_TYPE_UMTS;
            reg_info.lac = 0x003D;
            reg_info.cid = 0x0085AB54;
            bool unsolicited = false;

            switch (CURRENT_TEST_CASE) {
            case wb_testcase_creg_reg_presentation_1:
                reg_info.reg_status = CN_REGISTERED_TO_HOME_NW;
                break;
            case wb_testcase_creg_reg_presentation_2:
                reg_info.reg_status = CN_NOT_REG_NOT_SEARCHING;
                break;
            case wb_testcase_creg_reg_loc_presentation_1:
                reg_info.reg_status = CN_REGISTERED_TO_HOME_NW;
                break;
            case wb_testcase_creg_reg_loc_presentation_2:
                reg_info.reg_status = CN_REGISTERED_ROAMING;
                break;
            case wb_testcase_creg_reg_loc_presentation_3:
                reg_info.reg_status = CN_NOT_REG_NOT_SEARCHING;
                break;
            case wb_testcase_creg_unsolicited_1:
                reg_info.reg_status = CN_REGISTERED_TO_HOME_NW;
                unsolicited = true;
                break;
            case wb_testcase_creg_unsolicited_2:
                reg_info.gprs_attached = 0;
                reg_info.reg_status = CN_NOT_REG_NOT_SEARCHING;
                unsolicited = true;
                break;
            case wb_testcase_creg_unsolicited_3:
                reg_info.reg_status = CN_REGISTERED_TO_HOME_NW;
                unsolicited = true;
                break;
            case wb_testcase_creg_unsolicited_4:
                reg_info.reg_status = CN_REGISTERED_ROAMING;
                unsolicited = true;
                break;
            case wb_testcase_creg_unsolicited_5:
                reg_info.gprs_attached = 0;
                reg_info.reg_status = CN_REGISTRATION_UNKNOWN;
                unsolicited = true;
                break;
                /* *EREG */
            case wb_testcase_ereg_reg_presentation_1:
                reg_info.reg_status = CN_REGISTERED_TO_HOME_NW;
                break;
            case wb_testcase_ereg_reg_presentation_2:
                reg_info.reg_status = CN_NOT_REG_NOT_SEARCHING;
                break;
            case wb_testcase_ereg_reg_ext_presentation_1:
                reg_info.reg_status = CN_REGISTERED_TO_HOME_NW;
                break;
            case wb_testcase_ereg_reg_ext_presentation_2:
                reg_info.reg_status = CN_REGISTERED_ROAMING;
                break;
            case wb_testcase_ereg_reg_ext_presentation_3:
                reg_info.reg_status = CN_NOT_REG_NOT_SEARCHING;
                break;
            case wb_testcase_ereg_unsolicited_1:
                reg_info.reg_status = CN_REGISTERED_TO_HOME_NW;
                unsolicited = true;
                break;
            case wb_testcase_ereg_unsolicited_2:
                reg_info.gprs_attached = 0;
                reg_info.reg_status = CN_NOT_REG_NOT_SEARCHING;
                unsolicited = true;
                break;
            case wb_testcase_ereg_unsolicited_3:
                reg_info.reg_status = CN_REGISTERED_TO_HOME_NW;
                unsolicited = true;
                break;
            case wb_testcase_ereg_unsolicited_4:
                reg_info.reg_status = CN_REGISTERED_ROAMING;
                unsolicited = true;
                break;
            case wb_testcase_ereg_unsolicited_5:
                reg_info.gprs_attached = 0;
                reg_info.reg_status = CN_REGISTRATION_UNKNOWN;
                unsolicited = true;
                break;
                /* +CGREG */
            case wb_testcase_cgreg_reg_presentation_1:
                reg_info.reg_status = CN_REGISTERED_TO_HOME_NW;
                break;
            case wb_testcase_cgreg_reg_presentation_2:
                reg_info.gprs_attached = 0;
                reg_info.reg_status = CN_NOT_REG_NOT_SEARCHING;
                break;
            case wb_testcase_cgreg_reg_loc_presentation_1:
                reg_info.reg_status = CN_REGISTERED_TO_HOME_NW;
                break;
            case wb_testcase_cgreg_reg_loc_presentation_2:
                reg_info.reg_status = CN_REGISTERED_ROAMING;
                break;
            case wb_testcase_cgreg_reg_loc_presentation_3:
                reg_info.gprs_attached = 0;
                reg_info.reg_status = CN_REGISTRATION_DENIED;
                break;
            case wb_testcase_cgreg_unsolicited_1:
                reg_info.reg_status = CN_REGISTERED_TO_HOME_NW;
                unsolicited = true;
                break;
            case wb_testcase_cgreg_unsolicited_2:
                reg_info.gprs_attached = 0;
                reg_info.reg_status = CN_NOT_REG_NOT_SEARCHING;
                unsolicited = true;
                break;
            case wb_testcase_cgreg_unsolicited_3:
                reg_info.gprs_attached = 1;
                reg_info.reg_status = CN_REGISTERED_TO_HOME_NW;
                reg_info.rat = CN_RAT_TYPE_EDGE;
                unsolicited = true;
                break;
            case wb_testcase_cgreg_unsolicited_4:
                reg_info.gprs_attached = 1;
                reg_info.reg_status = CN_REGISTERED_ROAMING;
                reg_info.rat = CN_RAT_TYPE_EDGE;
                unsolicited = true;
                break;
            case wb_testcase_cgreg_unsolicited_5:
                reg_info.gprs_attached = 0;
                reg_info.reg_status = CN_NOT_REG_NOT_SEARCHING;
                unsolicited = true;
                break;
            default:
                printf("%s: wb_testcase_c(g)reg_*, unknown testcase=%d!\n", __FUNCTION__, CURRENT_TEST_CASE);
                break;
            }

            if (unsolicited) {
                printf("creating message CN_EVENT_MODEM_REGISTRATION_STATUS, CN_SUCCESS\n");
                *msg_pp = create_message(CN_EVENT_MODEM_REGISTRATION_STATUS, CN_SUCCESS, ct, sizeof(reg_info), &reg_info);
            } else {
                printf("creating message CN_RESPONSE_REGISTRATION_STATE_NORMAL, CN_SUCCESS\n");
                *msg_pp = create_message(CN_RESPONSE_REGISTRATION_STATE_NORMAL, CN_SUCCESS, ct, sizeof(reg_info), &reg_info);
            }
        } /* end of else-statement */

        break;
    } /* end of case statement */

    /* A */
    case wb_testcase_a_do:
        /* H */
    case wb_testcase_h:
        /* +CEER */
    case wb_testcase_ceer_h_no_call:
    case wb_testcase_ceer_h_ok_unspec:
    case wb_testcase_ceer_h_ok:
        /* +CHLD */
    case wb_testcase_chld_0:
    case wb_testcase_chld_1:
    case wb_testcase_chld_2:
    case wb_testcase_chld_3:
    case wb_testcase_chld_4:
    case wb_testcase_chld_13:
    case wb_testcase_chld_22:
    case wb_testcase_chld_5:
    case wb_testcase_chld_29: {

        cn_call_context_t context_local;
        init_dummy_call_context(&context_local);

        context_local.call_id = 1;

        if (wb_testcase_ceer_h_no_call == CURRENT_TEST_CASE) {
            context_local.call_cause.cause_type_sender = CN_CALL_CAUSE_TYPE_SENDER_SERVER; /* 2 */
            context_local.call_cause.cause = CN_CALL_SERVER_CAUSE_NO_CALL;
        } else if (wb_testcase_ceer_h_ok == CURRENT_TEST_CASE) {
            context_local.call_cause.cause_type_sender = CN_CALL_CAUSE_TYPE_SENDER_NETWORK; /* 3 */
            context_local.call_cause.cause = CN_CALL_NETWORK_CAUSE_NORMAL; /* 16 */
        } else if (wb_testcase_ceer_h_ok_unspec == CURRENT_TEST_CASE) {
            context_local.call_cause.cause_type_sender = CN_CALL_CAUSE_TYPE_SENDER_NETWORK; /* 3 */
            context_local.call_cause.cause = CN_CALL_NETWORK_CAUSE_NORMAL_UNSPECIFIED; /* 31 */
        }

        switch (state) {
        case -1:
            *msg_pp = create_message(CN_RESPONSE_HANGUP, CN_SUCCESS, ct, 0, NULL);
            break;

        case 0: {
            cn_call_list_t *call_list_p = NULL;
            cn_sint32_t nr_of_calls;
            cn_sint32_t size = sizeof(cn_call_list_t);

            if (wb_testcase_chld_2 == CURRENT_TEST_CASE ||
                    wb_testcase_chld_3 == CURRENT_TEST_CASE ||
                    wb_testcase_chld_4 == CURRENT_TEST_CASE) {
                nr_of_calls = 2;
            } else if (wb_testcase_chld_5 == CURRENT_TEST_CASE ||
                       wb_testcase_chld_29 == CURRENT_TEST_CASE) {
                nr_of_calls = 0;
            } else {
                nr_of_calls = 1;
            }

            if (nr_of_calls > 0) {
                size += sizeof(cn_call_context_t) * (nr_of_calls - 1);
            }

            call_list_p = calloc(1, size);

            if (!call_list_p) {
                *msg_pp = create_message(CN_RESPONSE_CURRENT_CALL_LIST, CN_FAILURE, ct, 0, NULL);
            } else {
                cn_sint32_t i;
                call_list_p->nr_of_calls = nr_of_calls;

                for (i = 0; i < nr_of_calls; i++) {
                    if (wb_testcase_a_do == CURRENT_TEST_CASE) {
                        call_list_p->call_context[i].call_state = CN_CALL_STATE_INCOMING;
                    } else if (wb_testcase_h == CURRENT_TEST_CASE) {
                        call_list_p->call_context[i].call_state = CN_CALL_STATE_ACTIVE;
                    } else if (wb_testcase_ceer_h_no_call == CURRENT_TEST_CASE) {
                        call_list_p->call_context[i].call_state = CN_CALL_STATE_ACTIVE;
                    } else if (wb_testcase_ceer_h_ok_unspec == CURRENT_TEST_CASE) {
                        call_list_p->call_context[i].call_state = CN_CALL_STATE_ACTIVE;
                    } else if (wb_testcase_ceer_h_ok == CURRENT_TEST_CASE) {
                        call_list_p->call_context[i].call_state = CN_CALL_STATE_ACTIVE;
                    } else if (wb_testcase_chld_0 == CURRENT_TEST_CASE) {
                        call_list_p->call_context[i].call_state = CN_CALL_STATE_HOLDING;
                    } else if (wb_testcase_chld_1 == CURRENT_TEST_CASE) {
                        call_list_p->call_context[i].call_state = CN_CALL_STATE_WAITING;
                    } else if (wb_testcase_chld_2 == CURRENT_TEST_CASE ||
                               wb_testcase_chld_3 == CURRENT_TEST_CASE ||
                               wb_testcase_chld_4 == CURRENT_TEST_CASE) {
                        if (0 == i) {
                            call_list_p->call_context[i].call_state = CN_CALL_STATE_ACTIVE;
                        } else {
                            call_list_p->call_context[i].call_state = CN_CALL_STATE_HOLDING;
                        }
                    } else {
                        call_list_p->call_context[i].call_state = CN_CALL_STATE_IDLE;
                    }

                    call_list_p->call_context[i].call_id           = i + 1;
                    call_list_p->call_context[i].address_type      = 145;
                    call_list_p->call_context[i].is_MT             = 1;
                    call_list_p->call_context[i].mode              = CN_CALL_MODE_SPEECH;
                    call_list_p->call_context[i].call_cause.cause_type_sender = CN_CALL_CAUSE_TYPE_SENDER_NONE;
                    (void)strncpy(call_list_p->call_context[i].number, "1234", CN_MAX_STRING_SIZE);
                    (void)memcpy(call_list_p->call_context[i].name, no_name, sizeof(no_name));
                }

                *msg_pp = create_message(CN_RESPONSE_CURRENT_CALL_LIST, CN_SUCCESS, ct, size, call_list_p);
                free(call_list_p);
            }

            break;

            case 1:

                if (wb_testcase_a_do == CURRENT_TEST_CASE) {
                    *msg_pp = create_message(CN_RESPONSE_ANSWER_CALL, CN_SUCCESS, ct, 0, NULL);
                } else if (wb_testcase_h == CURRENT_TEST_CASE) {
                    *msg_pp = create_message(CN_RESPONSE_HANGUP, CN_SUCCESS, ct, 0, NULL);
                } else if (wb_testcase_ceer_h_no_call == CURRENT_TEST_CASE) {
                    *msg_pp = create_message(CN_RESPONSE_HANGUP, CN_FAILURE, ct, sizeof(context_local.call_cause), &context_local.call_cause);
                } else if (wb_testcase_ceer_h_ok_unspec == CURRENT_TEST_CASE) {
                    *msg_pp = create_message(CN_RESPONSE_HANGUP, CN_SUCCESS, ct, 0, NULL);
                } else if (wb_testcase_ceer_h_ok == CURRENT_TEST_CASE) {
                    *msg_pp = create_message(CN_RESPONSE_HANGUP, CN_SUCCESS, ct, 0, NULL);
                } else if (wb_testcase_chld_2 == CURRENT_TEST_CASE) {
                    *msg_pp = create_message(CN_RESPONSE_SWAP_CALLS, CN_SUCCESS, ct, 0, NULL);
                } else if (wb_testcase_chld_3 == CURRENT_TEST_CASE) {
                    *msg_pp = create_message(CN_RESPONSE_CONFERENCE_CALL, CN_SUCCESS, ct, 0, NULL);
                } else if (wb_testcase_chld_4 == CURRENT_TEST_CASE) {
                    *msg_pp = create_message(CN_RESPONSE_EXPLICIT_CALL_TRANSFER, CN_SUCCESS, ct, 0, NULL);
                } else {
                    *msg_pp = create_message(CN_RESPONSE_HANGUP, CN_SUCCESS, ct, 0, NULL);
                }

                break;
            case 2:

                if ((wb_testcase_ceer_h_ok_unspec == CURRENT_TEST_CASE) ||
                        (wb_testcase_ceer_h_ok == CURRENT_TEST_CASE)) {
                    *msg_pp = create_message(CN_EVENT_CALL_STATE_CHANGED, CN_SUCCESS, ct, sizeof(context_local), &context_local);
                    break;
                }

                /* otherwise, state is invalid so fall through to default: */
            }
        default:
            printf("%s: wb_testcase %d, bad state=%d!\n", __FUNCTION__, CURRENT_TEST_CASE, state);
            break;
        }

        break;
    }

    /* +CLCC */
    case wb_testcase_clcc_0:
    case wb_testcase_clcc_1:
    case wb_testcase_clcc_2: {
        cn_call_list_t *call_list_p;
        int nr_of_calls;
        int size = sizeof(cn_call_list_t);

        if (wb_testcase_clcc_0 == CURRENT_TEST_CASE) {
            nr_of_calls = 0;
        } else if (wb_testcase_clcc_1 == CURRENT_TEST_CASE) {
            nr_of_calls = 1;
        } else {
            nr_of_calls = 2;
        }

        if (nr_of_calls > 0) {
            size += sizeof(cn_call_context_t) * (nr_of_calls - 1);
        }

        call_list_p = calloc(1, size);

        if (call_list_p) {
            int i;
            call_list_p->nr_of_calls = nr_of_calls;

            for (i = 0; i < nr_of_calls; i++) {
                call_list_p->call_context[i].call_state        = (i == 1) ? CN_CALL_STATE_INCOMING : CN_CALL_STATE_ACTIVE;
                call_list_p->call_context[i].call_id           = i + 1;
                call_list_p->call_context[i].address_type      = (i == 1) ? 129 : 145;
                call_list_p->call_context[i].is_MT             = (i == 1) ? 1 : 0;
                call_list_p->call_context[i].mode              = (i == 0) ? CN_CALL_MODE_EMERGENCY : CN_CALL_MODE_SPEECH;
                call_list_p->call_context[i].name_presentation = (i == 1) ? 1 : 0;

                if (i == 1) {
                    (void)strncpy(call_list_p->call_context[i].number, "046143099", CN_MAX_STRING_SIZE);
                    call_list_p->call_context[i].cause_no_cli  = CN_CAUSE_NO_CLI_REJECT_BY_USER;
                } else {
                    (void)strncpy(call_list_p->call_context[i].number, "+4646143000", CN_MAX_STRING_SIZE);
                    call_list_p->call_context[i].cause_no_cli  = (cn_cause_no_cli_t)0;
                }
            }

            *msg_pp = create_message(CN_RESPONSE_CURRENT_CALL_LIST, CN_SUCCESS, ct, size, call_list_p);
            free(call_list_p);
        } else {
            printf("%s: calloc failed for call_list_p", __FUNCTION__);
        }

        break;
    }

    /* +CLCK */
    case wb_testcase_clck_call_barring_lock:
    case wb_testcase_clck_call_barring_unlock:
        *msg_pp = create_message(CN_RESPONSE_SET_CALL_BARRING, CN_SUCCESS, ct, 0, NULL);
        break;
    case wb_testcase_clck_call_barring_query: {
        cn_call_barring_t call_barring;
        call_barring.service_class = 7;
        call_barring.setting = CN_CALL_BARRING_ACTIVATION;
        *msg_pp = create_message(CN_RESPONSE_QUERY_CALL_BARRING, CN_SUCCESS, ct, sizeof(call_barring), &call_barring);
        break;
    }

    /* +CLIP */
    case wb_testcase_clip_read: {
        cn_clip_status_t clip_status = 0;
        *msg_pp = create_message(CN_RESPONSE_CLIP_STATUS, CN_SUCCESS, ct, sizeof(clip_status), &clip_status);
        break;
    }

    /* +CLIR */
    case wb_testcase_clir_read: {
        cn_clir_t clir_status = { CN_CLIR_SETTING_INVOCATION, CN_CLIR_STATUS_UNKNOWN };
        *msg_pp = create_message(CN_RESPONSE_CLIR_STATUS, CN_SUCCESS, ct, sizeof(clir_status), &clir_status);
        break;
    }
    case wb_testcase_clir_set:
        *msg_pp = create_message(CN_RESPONSE_SET_CLIR, CN_SUCCESS, ct, 0, NULL);
        break;

        /* +CNAP */
    case wb_testcase_cnap_read_0:
    case wb_testcase_cnap_read_1: {
        cn_cnap_status_t cnap_status = 0;
        *msg_pp = create_message(CN_RESPONSE_CNAP_STATUS, CN_SUCCESS, ct, sizeof(cnap_status), &cnap_status);
        break;
    }

    /* +COLR */
    case wb_testcase_colr_do: {
        cn_colr_status_t colr_status = 1;
        *msg_pp = create_message(CN_RESPONSE_COLR_STATUS, CN_SUCCESS, ct, sizeof(colr_status), &colr_status);
        break;
    }
    /* +COPS */
    case wb_testcase_cops_network_manual_registration_with_automatic_fallback:
        *msg_pp = create_message(CN_RESPONSE_MANUAL_NETWORK_REGISTRATION_WITH_AUTOMATIC_FALLBACK, CN_SUCCESS, ct, 0, NULL);
        break;


    case wb_testcase_cops_network_search: {
        char *long_alphanum[] = {"LongOperatorN.00", "LongOperatorN.01", "LongOperatorN.02", "LongOperatorN.03"};
        char *short_alphanum[] = {"ShortN.0", "ShortN.1", "ShortN.2", "ShortN.3"};
        char *mcc_mnc[] = {"Net.0", "Net.1", "Net.2", "Net.3"};
        uint8_t stat[] = {0, 1, 2, 3};
        uint8_t network_type[] = {1, 0, 3, 2}; /* Defines the order in the AT+COPS=? response. */
        unsigned int i;

        switch(state) {
        case 0: {
            cn_registration_info_t data;
            memset(&data, 0, sizeof(data));
            strcpy(data.long_operator_name, "LongOperatorNLongOperatorNLongOperatorNLongOperatorN123");
            data.reg_status = CN_REGISTERED_TO_HOME_NW;
            data.rat = CN_RAT_TYPE_GPRS;
            data.search_mode = CN_NETWORK_SEARCH_MODE_MANUAL;
            *msg_pp = create_message(CN_RESPONSE_REGISTRATION_STATE_NORMAL, CN_SUCCESS, ct, sizeof(data), &data);
            break;
        }
        case 1: {
            cn_manual_network_search_data_t *data_p =
                    alloca(sizeof(*data_p) * NETWORK_LIST_LEN);

            memset(data_p, 0, sizeof(*data_p) * NETWORK_LIST_LEN);

            /* Setup list */
            data_p->num_of_networks = NETWORK_LIST_LEN;

            for (i = 0; i < NETWORK_LIST_LEN; i++) {
                data_p->cn_network_info[i].network_type = network_type[i % NETWORK_LIST_LEN];
                data_p->cn_network_info[i].network_status = stat[i % NETWORK_LIST_LEN];
                strcpy(data_p->cn_network_info[i].long_op_name, long_alphanum[i % NETWORK_LIST_LEN]);
                strcpy(data_p->cn_network_info[i].short_op_name, short_alphanum[i % NETWORK_LIST_LEN]);
                strncpy(data_p->cn_network_info[i].mcc_mnc, mcc_mnc[i % NETWORK_LIST_LEN], NET_MCC_MNC_STRING_LEN);

                if (i % 2 == 0 && data_p->cn_network_info[i].network_status != 2) {
                    data_p->cn_network_info[i].umts_available = 0;
                    data_p->cn_network_info[i].band_info = 1;
                } else {
                    data_p->cn_network_info[i].umts_available = 1;
                    /* If current make it a dual rat cell */
                    if (data_p->cn_network_info[i].network_status == 2) {
                        data_p->cn_network_info[i].band_info = 1;
                    } else {
                        data_p->cn_network_info[i].band_info = 2;
                    }
                }
            }
            *msg_pp = create_message(CN_RESPONSE_MANUAL_NETWORK_SEARCH, CN_SUCCESS,
                    ct, sizeof(cn_manual_network_search_data_t) * NETWORK_LIST_LEN,
                    data_p);
            break;
        }
        default:
            break;
        }
        break;
    }

    case wb_testcase_cnap_unsolicited: {
        uint8_t name_str[] = { 0, 'N', 0, 'o', 0, ' ', 0, 'N', 0, 'a', 0, 'm', 0, 'e' };
        cn_call_context_t call_context;
        memset(&call_context, 0, sizeof(call_context));
        call_context.call_state        = CN_CALL_STATE_INCOMING;
        call_context.call_id           = 1;
        call_context.address_type      = 145;
        call_context.is_MT             = 1;
        call_context.mode              = CN_CALL_MODE_SPEECH;
        strcpy(call_context.number, PHONE_NUMBER_INT);
        call_context.number_presentation = CN_PRESENTATION_ALLOWED;
        memcpy(call_context.name, name_str, sizeof(name_str));
        call_context.name_char_length   = sizeof(name_str) / sizeof(uint16_t);
        call_context.name_presentation = CN_PRESENTATION_ALLOWED;
        *msg_pp = create_message(CN_EVENT_CALL_CNAP, CN_SUCCESS, 0, sizeof(call_context), &call_context);
        break;
    }
    case wb_testcase_ecme_unsolicited: {
        cn_net_detailed_fail_cause_t fail_cause;
        memset(&fail_cause, 0, sizeof(fail_cause));
        fail_cause.cause = 0x02; /* Corresponds to cme error 102 */
        fail_cause.class = CN_NET_DETAILED_FAIL_CAUSE_CLASS_NET;
        *msg_pp = create_message(CN_EVENT_NET_DETAILED_FAIL_CAUSE, CN_SUCCESS, 0, sizeof(fail_cause), &fail_cause);
        break;
    }
    case wb_testcase_elat_unsolicited: {
        cn_comfort_tone_generation_t tone_generation = 1;
        *msg_pp = create_message(CN_EVENT_GENERATE_LOCAL_COMFORT_TONES, CN_SUCCESS, 0, sizeof(tone_generation), &tone_generation);
        break;
    }

    case wb_testcase_elin_read_1: /* Fallthrough */
    case wb_testcase_elin_read_2: {
        cn_modem_property_t property;

        memset(&property, 0, sizeof(cn_modem_property_t));

        property.type = CN_MODEM_PROPERTY_TYPE_ALS;
        property.value.als.supported_lines = CN_SUPPORTED_ALS_LINE1_AND_LINE2;

        if (wb_testcase_elin_read_2 == CURRENT_TEST_CASE) {
            property.value.als.current_line = CN_ALS_LINE_TWO;
        } else {
            property.value.als.current_line = CN_ALS_LINE_NORMAL;
        }

        *msg_pp = create_message(CN_RESPONSE_GET_MODEM_PROPERTY, CN_SUCCESS, ct, sizeof(cn_modem_property_t), &property);
        break;
    }

    case wb_testcase_elin_set_1: /* Fallthrough */
    case wb_testcase_elin_set_2: {
        *msg_pp = create_message(CN_RESPONSE_SET_MODEM_PROPERTY, CN_SUCCESS, ct, 0, NULL);
        break;
    }

    case wb_testcase_elin_set_2_no_support: {
        *msg_pp = create_message(CN_RESPONSE_SET_MODEM_PROPERTY, CN_FAILURE, ct, 0, NULL);
        break;
    }

    case wb_testcase_elin_test_support: /* Fallthrough */
    case wb_testcase_elin_test_no_support: {
        cn_modem_property_t property;

        property.type = CN_MODEM_PROPERTY_TYPE_ALS;
        property.value.als.current_line = CN_ALS_LINE_NORMAL;

        if (wb_testcase_elin_test_no_support == CURRENT_TEST_CASE) {
            property.value.als.supported_lines = CN_SUPPORTED_ALS_LINE1;
        } else {
            property.value.als.supported_lines = CN_SUPPORTED_ALS_LINE1_AND_LINE2;
        }

        *msg_pp = create_message(CN_RESPONSE_SET_EVENT_REPORTING, CN_SUCCESS, ct, sizeof(cn_modem_property_t), &property);
        break;
    }

    /* +CPWD */
    case wb_testcase_cpwd_call_barring:
        *msg_pp = create_message(CN_RESPONSE_CHANGE_BARRING_PASSWORD, CN_SUCCESS, ct, 0, NULL);
        break;

        /* +CSSN */
    case wb_testcase_cssi_1:
    case wb_testcase_cssi_2:

        switch (state) {
        case 0:
            *msg_pp = create_message(CN_RESPONSE_SET_CSSN, CN_SUCCESS, ct, 0, NULL);
            break;
        case 1: {
            cn_supp_svc_notification_t event;
            event.notification_type = CN_NOTIFICATION_TYPE_MO_INTERMEDIATE;

            if (CURRENT_TEST_CASE == wb_testcase_cssi_1) {
                event.code = 2;
            } else {
                event.code = 4;
                event.index = 3;
            }

            event.address_type = 145;
            strcpy(event.number, "+4612345678");
            *msg_pp = create_message(CN_EVENT_CALL_SUPP_SVC_NOTIFICATION, CN_SUCCESS, 0, sizeof(event), &event);
            break;
        }
        default:
            printf("%s: wb_testcase_cssi_*, bad state=%d!\n", __FUNCTION__, state);
            break;
        }

        break;

    case wb_testcase_cssn:

        switch (state) {
        case 1:
        case 5:
            *msg_pp = create_message(CN_RESPONSE_SET_CSSN, CN_SUCCESS, ct, 0, NULL);
            break;
        default:
            printf("%s: wb_testcase_cssn_*, bad state=%d!\n", __FUNCTION__, state);
            break;
        }

        break;

        /* +CMER */
    case wb_testcase_cmer_set_7:
    case wb_testcase_cmer_set_8:
    case wb_testcase_cmer_set_9:
    case wb_testcase_cmer_set_10:
    case wb_testcase_cmer_set_11:
    case wb_testcase_cmer_set_12:
    case wb_testcase_cmer_set_13:
    case wb_testcase_cmer_set_14:
    case wb_testcase_cmer_set_neg_7:
    case wb_testcase_cmer_set_neg_8:
    case wb_testcase_cmer_set_neg_9:
    case wb_testcase_cmer_set_neg_10:
    case wb_testcase_cmer_set_neg_11:
    case wb_testcase_cmer_read:
    case wb_testcase_cmer_unsol_ciev_10: {
        *msg_pp = create_message(CN_RESPONSE_SET_SIGNAL_INFO_REPORTING, CN_SUCCESS, ct, 0, NULL);
        break;
    }

    /* +CIEV */
    case wb_testcase_cmer_unsol_ciev_1:
    case wb_testcase_cmer_unsol_ciev_2:
    case wb_testcase_cmer_unsol_ciev_3:
    case wb_testcase_cmer_unsol_ciev_4:
    case wb_testcase_cmer_unsol_ciev_5:
    case wb_testcase_cmer_unsol_ciev_6:
    case wb_testcase_cmer_unsol_ciev_disabled: {
        cn_signal_info_t signal_info_context;
        int i;

        memset(&signal_info_context, 0, sizeof(signal_info_context));
        signal_info_context.ber = 1;

        for (i = 0; i < CN_RSSI_CONF_MAX_SEGMENTS; i++) {
            signal_info_context.border[i] = 0;
        }

        signal_info_context.border[0] = 120;
        signal_info_context.border[1] = 102;
        signal_info_context.border[2] = 93;
        signal_info_context.border[3] = 87;
        signal_info_context.border[4] = 78;
        signal_info_context.border[5] = 40;
        signal_info_context.num_of_segments = 0;
        signal_info_context.no_of_signal_bars = 99;

        /*
         * 0 bars   -113 (or lower) to -109 dBm
         * 1 bar    -108 to -105 dBm
         * 2 bars   -104 to  -99 dBm
         * 3 bars    -98 to  -91 dBm
         * 4 bars    -90 to  -81 dBm
         * 5 bars    -80 to  -51 dBm (or higher)
         */
        switch (CURRENT_TEST_CASE) {
        case wb_testcase_cmer_unsol_ciev_disabled:
        case wb_testcase_cmer_unsol_ciev_1:
            signal_info_context.rssi_dbm = 130;
            break;
        case wb_testcase_cmer_unsol_ciev_2:
            signal_info_context.rssi_dbm = 105;
            break;
        case wb_testcase_cmer_unsol_ciev_3:
            signal_info_context.rssi_dbm = 99;
            break;
        case wb_testcase_cmer_unsol_ciev_4:
            signal_info_context.rssi_dbm = 91;
            break;
        case wb_testcase_cmer_unsol_ciev_5:
            signal_info_context.rssi_dbm = 81;
            break;
        case wb_testcase_cmer_unsol_ciev_6:
            signal_info_context.rssi_dbm = 60;
            break;
        default:
            signal_info_context.rssi_dbm = 0;
            break;
        }

        *msg_pp = create_message(CN_EVENT_SIGNAL_INFO, CN_SUCCESS, ct, sizeof(signal_info_context), &signal_info_context);

        break;
    }

    case wb_testcase_cssu_1:
    case wb_testcase_cssu_2:
    case wb_testcase_cssu_3:

        switch (state) {
        case 0:
            *msg_pp = create_message(CN_RESPONSE_SET_CSSN, CN_SUCCESS, ct, 0, NULL);
            break;
        case 1: {
            cn_supp_svc_notification_t event;
            event.notification_type = CN_NOTIFICATION_TYPE_MT_UNSOLICITED;

            if (CURRENT_TEST_CASE == wb_testcase_cssu_1) {
                event.code = 2;
                event.address_type = 145;
                strcpy(event.number, "+4612345678");
            } else if (CURRENT_TEST_CASE == wb_testcase_cssu_2) {
                event.code = 1;
                event.index = 3;
                event.address_type = 145;
                strcpy(event.number, "4612345678");
            } else if (CURRENT_TEST_CASE == wb_testcase_cssu_3) {
                event.code = 3;
                event.address_type = 90;
                strcpy(event.number, "12345678");
            } else {
                event.code = 10;
            }

            *msg_pp = create_message(CN_EVENT_CALL_SUPP_SVC_NOTIFICATION, CN_SUCCESS, 0, sizeof(event), &event);
            break;
        }
        }

        break;

        /* +CSQ */
    case wb_testcase_csq_do: {
        cn_rssi_t rssi = 133;
        *msg_pp = create_message(CN_RESPONSE_RSSI_VALUE, CN_SUCCESS, ct, sizeof(rssi), &rssi);
        break;
    }

    /* +CUSD */
    case wb_testcase_cusd_send_ussd:
    case wb_testcase_cusd_cancel_ussd:
        *msg_pp = create_message(CN_RESPONSE_USSD, CN_SUCCESS, ct, 0, NULL);
        break;
    case wb_testcase_cusd_unsolicited: {
        cn_ussd_info_t event = {
                .type = CN_USSD_TYPE_USSD_NOTIFY,
                .dcs = 15,
                .length = 6,
                .ussd_string = {"\040\151\332\175\246\203"}
        };
        *msg_pp = create_message(CN_EVENT_USSD, CN_SUCCESS, 0, sizeof(event), &event);
        break;
    }
    case wb_testcase_cusd_unsolicited_gsm8: {
        cn_ussd_info_t event = {
                .type = CN_USSD_TYPE_USSD_NOTIFY,
                .dcs = 68,
                .length = 19,
                .ussd_string = {" Ringt f|r: 5,62 kr"},
        };
        *msg_pp = create_message(CN_EVENT_USSD, CN_SUCCESS, 0, sizeof(event), &event);
        break;
    }
    case wb_testcase_cusd_unsolicited_ucs2: {
        cn_ussd_info_t event = {
                .type = CN_USSD_TYPE_USSD_NOTIFY,
                .dcs = 72,
                .length = 14,
                .ussd_string = {0, ' ', 0, 'R', 0, 'i', 0, '"', 0, 'n', 0, '"', 0, 'g', 0, 0},
        };
        *msg_pp = create_message(CN_EVENT_USSD, CN_SUCCESS, 0, sizeof(event), &event);
        break;
    }
    case wb_testcase_cusd_unsolicited_hex: {
        cn_ussd_info_t event = {
                .type = CN_USSD_TYPE_USSD_NOTIFY,
                .dcs = 1,
                .length = 5,
                .ussd_string = {"\040\151\332\175\246"},
        };
        *msg_pp = create_message(CN_EVENT_USSD, CN_SUCCESS, 0, sizeof(event), &event);
        break;
    }
    case wb_testcase_cusd_hex:
        *msg_pp = create_message(CN_RESPONSE_USSD, CN_SUCCESS, ct, 0, NULL);
        break;
    case wb_testcase_cusd_hex_fail:
        *msg_pp = create_message(CN_RESPONSE_USSD, CN_FAILURE, ct, 0, NULL);
        break;

        /* +VTS */
    case wb_testcase_dtmf:
    case wb_testcase_dtmf_neg:
    case wb_testcase_dtmf_test:
        *msg_pp = create_message(CN_RESPONSE_DTMF_SEND, CN_SUCCESS, ct, 0, NULL);
        break;

        /* *ETTY */
    case wb_testcase_etty_set_0:
    case wb_testcase_etty_set_1:
    case wb_testcase_etty_set_2:
    case wb_testcase_etty_set_3:
        *msg_pp = create_message(CN_RESPONSE_SET_MODEM_PROPERTY, CN_SUCCESS, ct, 0, NULL);
        break;

        /* *ETZV */
    case wb_testcase_etzv_1:
    case wb_testcase_etzv_2:
    case wb_testcase_etzv_3: {
        cn_time_info_t time;
        memset(&time, 0, sizeof(time));
        time.year = 10;
        time.month = 5;
        time.day = 20;
        time.hour = 10;
        time.minute = 45;
        time.second = 0;
        time.time_zone_sign = 0;
        time.time_zone_value = 2;
        time.day_light_saving_time = 1;
        *msg_pp = create_message(CN_EVENT_TIME_INFO, CN_SUCCESS, 0, sizeof(time), &time);
        break;
    }
    case wb_testcase_etzv_4: { /* ETZR=1 ; event with timezone data only (positive case) */
        cn_time_info_t time;
        memset(&time, 0, sizeof(time));
        time.year = CN_TIME_INFO_NOT_AVAILABLE;
        time.month = CN_TIME_INFO_NOT_AVAILABLE;
        time.day = CN_TIME_INFO_NOT_AVAILABLE;
        time.hour = CN_TIME_INFO_NOT_AVAILABLE;
        time.minute = CN_TIME_INFO_NOT_AVAILABLE;
        time.second = CN_TIME_INFO_NOT_AVAILABLE;
        time.time_zone_sign = 0;
        time.time_zone_value = 2;
        time.day_light_saving_time = CN_TIME_INFO_NOT_AVAILABLE;
        *msg_pp = create_message(CN_EVENT_TIME_INFO, CN_SUCCESS, 0, sizeof(time), &time);
        break;
    }
    case wb_testcase_etzv_5: { /* ETZR=2 ; event omitting dst field (positive case) */
        cn_time_info_t time;
        memset(&time, 0, sizeof(time));
        time.year = 10;
        time.month = 5;
        time.day = 20;
        time.hour = 10;
        time.minute = 45;
        time.second = 0;
        time.time_zone_sign = 0;
        time.time_zone_value = 2;
        time.day_light_saving_time = CN_TIME_INFO_NOT_AVAILABLE;
        *msg_pp = create_message(CN_EVENT_TIME_INFO, CN_SUCCESS, 0, sizeof(time), &time);
        break;
    }
    case wb_testcase_etzv_6: { /* ETZR=2 ; event omitting time (and dst) fields (negative case) */
        cn_time_info_t time;
        memset(&time, 0, sizeof(time));
        time.year = CN_TIME_INFO_NOT_AVAILABLE;
        time.month = CN_TIME_INFO_NOT_AVAILABLE;
        time.day = CN_TIME_INFO_NOT_AVAILABLE;
        time.hour = CN_TIME_INFO_NOT_AVAILABLE;
        time.minute = CN_TIME_INFO_NOT_AVAILABLE;
        time.second = CN_TIME_INFO_NOT_AVAILABLE;
        time.time_zone_sign = 0;
        time.time_zone_value = 2;
        time.day_light_saving_time = CN_TIME_INFO_NOT_AVAILABLE;
        *msg_pp = create_message(CN_EVENT_TIME_INFO, CN_SUCCESS, 0, sizeof(time), &time);
        break;
    }
    case wb_testcase_etzv_7: {
        cn_time_info_t time;
        memset(&time, 0, sizeof(time));
        time.year = CN_TIME_INFO_NOT_AVAILABLE;
        time.month = CN_TIME_INFO_NOT_AVAILABLE;
        time.day = CN_TIME_INFO_NOT_AVAILABLE;
        time.hour = CN_TIME_INFO_NOT_AVAILABLE;
        time.minute = CN_TIME_INFO_NOT_AVAILABLE;
        time.second = CN_TIME_INFO_NOT_AVAILABLE;
        time.time_zone_sign = 0;
        time.time_zone_value = 2;
        time.day_light_saving_time = CN_TIME_INFO_NOT_AVAILABLE;
        *msg_pp = create_message(CN_EVENT_TIME_INFO, CN_SUCCESS, 0, sizeof(time), &time);
        break;
    }

    /* *ERFSTATE */
    case wb_testcase_erfstate_unsolicited_on:
    case wb_testcase_erfstate_unsolicited_off: {
        cn_rf_status_t rf_status;

        if (wb_testcase_erfstate_unsolicited_off == CURRENT_TEST_CASE) {
            rf_status = CN_RF_OFF;
        } else {
            rf_status = CN_RF_ON;
        }

        *msg_pp = create_message(CN_EVENT_RADIO_STATUS, CN_SUCCESS, 0, sizeof(rf_status), &rf_status);
        break;
    }
    case wb_testcase_cops_get_settings: {
        cn_registration_info_t data;
        memset(&data, 0, sizeof(data));
        strcpy(data.long_operator_name, "LongOperatorNLongOperatorNLongOperatorNLongOperatorN123");
        data.reg_status = CN_REGISTERED_TO_HOME_NW;
        data.rat = CN_RAT_TYPE_GPRS;
        data.search_mode = CN_NETWORK_SEARCH_MODE_MANUAL;
        *msg_pp = create_message(CN_RESPONSE_REGISTRATION_STATE_NORMAL, CN_SUCCESS, ct, sizeof(data), &data);
        break;
    }
    case wb_testcase_ehstact_1:
    case wb_testcase_ehstact_2: {
        *msg_pp = create_message(CN_RESPONSE_SET_USER_ACTIVITY_STATUS, CN_SUCCESS, ct, 0 , NULL);
        break;
    }
    case wb_testcase_egsci_1: {
        *msg_pp = create_message(CN_RESPONSE_GET_NEIGHBOUR_CELLS_EXTD_INFO, CN_SUCCESS, ct, 0, NULL);
        break;
    }
    case wb_testcase_egsci_2: {
        cn_neighbour_cells_info_t *neighbour_cells_info_p = NULL;
        cn_uint32_t size = 0;

        neighbour_cells_info_p = calloc(1, sizeof(cn_neighbour_cells_info_t));

        size = sizeof(neighbour_cells_info_p->rat_type)+sizeof(neighbour_cells_info_p->cells_info.gsm_cells_info);

        neighbour_cells_info_p->rat_type = CN_NEIGHBOUR_RAT_TYPE_2G;
        /* General serving cell info */
        neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_rxlev = -80;
        neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_mcc = 404;
        neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_mnc = 17;
        neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_lac = 0xFEDC;
        neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_cid = 0x4321;
        neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_arfcn = 106;
        neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_bsic= 31;
        neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_timing_advance= 120;

        *msg_pp = create_message(CN_RESPONSE_GET_NEIGHBOUR_CELLS_EXTD_INFO, CN_SUCCESS, ct, size, neighbour_cells_info_p);
        free(neighbour_cells_info_p);
        break;
    }
    case wb_testcase_egnci_1:
    case wb_testcase_egnci_2: {
        cn_neighbour_cells_info_t *neighbour_cells_info_p = NULL;
        cn_uint32_t size = 0;

        neighbour_cells_info_p = calloc(1, sizeof(cn_neighbour_cells_info_t));

        size = sizeof(neighbour_cells_info_p->rat_type)+sizeof(neighbour_cells_info_p->cells_info.gsm_cells_info);

        /* General serving cell info */
        neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_rxlev = -80;
        neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_mcc = 404;
        neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_mnc = 17;
        neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_lac = 0xFEDC;
        neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_cid = 0x4321;
        neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_arfcn = 106;
        neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_bsic= 31;
        neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_timing_advance= 120;

        /* Special neighbour cell info */
        switch (CURRENT_TEST_CASE) {
        case wb_testcase_egnci_1:
            neighbour_cells_info_p->rat_type = CN_NEIGHBOUR_RAT_TYPE_2G;
            neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.num_of_neighbours = 0;
            break;
        case wb_testcase_egnci_2:
            neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.num_of_neighbours = 3;
            neighbour_cells_info_p->rat_type = CN_NEIGHBOUR_RAT_TYPE_2G;

            neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[0].mcc = 433;
            neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[0].mnc = 42;
            neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[0].lac = 0x4321;
            neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[0].cid = 0xABCD;
            neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[0].rxlev = -100;

            neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[1].mcc = 404;
            neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[1].mnc = 45;
            neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[1].lac = 0x0123;
            neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[1].cid = 0x0987;
            neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[1].rxlev = -75;

            neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[2].mcc = 532;
            neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[2].mnc = 125;
            neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[2].lac = 0x5665;
            neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[2].cid = 0x00A5;
            neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[2].rxlev = -47;
            break;
        }

        *msg_pp = create_message(CN_RESPONSE_GET_NEIGHBOUR_CELLS_COMPLETE_INFO, CN_SUCCESS, ct, size, neighbour_cells_info_p);
        free(neighbour_cells_info_p);
        break;
    }
    case wb_testcase_ewsci_1: {
        *msg_pp = create_message(CN_RESPONSE_GET_NEIGHBOUR_CELLS_EXTD_INFO, CN_SUCCESS, ct, 0, NULL);
        break;
    }
    case wb_testcase_ewsci_2:
    case wb_testcase_ewnci_1:
    case wb_testcase_ewnci_2: {
        cn_neighbour_cells_info_t *neighbour_cells_info_p = NULL;
        cn_uint32_t size = 0;

        size = sizeof(neighbour_cells_info_p->rat_type)+sizeof(neighbour_cells_info_p->cells_info.umts_cells_info);

        neighbour_cells_info_p = calloc(1, sizeof(cn_neighbour_cells_info_t));

        neighbour_cells_info_p->rat_type = CN_NEIGHBOUR_RAT_TYPE_3G;
        /* General serving cell info */
        neighbour_cells_info_p->cells_info.umts_cells_info.umts_serving_cell_info.current_cpich_ecno = 33;
        neighbour_cells_info_p->cells_info.umts_cells_info.umts_serving_cell_info.current_cpich_pathloss = 66;
        neighbour_cells_info_p->cells_info.umts_cells_info.umts_serving_cell_info.current_cpich_rscp = 14;
        neighbour_cells_info_p->cells_info.umts_cells_info.umts_serving_cell_info.current_dl_uarfcn = 10780;
        neighbour_cells_info_p->cells_info.umts_cells_info.umts_serving_cell_info.current_psc = 411;
        neighbour_cells_info_p->cells_info.umts_cells_info.umts_serving_cell_info.current_mcc = 404;
        neighbour_cells_info_p->cells_info.umts_cells_info.umts_serving_cell_info.current_mnc = 71;
        neighbour_cells_info_p->cells_info.umts_cells_info.umts_serving_cell_info.current_ucid = 0x1387654;
        neighbour_cells_info_p->cells_info.umts_cells_info.umts_serving_cell_info.current_utra_carrier_rssi = 100;

        /* Special neighbour cell info */
        switch (CURRENT_TEST_CASE) {
        case wb_testcase_ewnci_1:
            neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.num_of_dl_uarfcn = 0;
            neighbour_cells_info_p->rat_type = CN_NEIGHBOUR_RAT_TYPE_3G;
            break;
        case wb_testcase_ewnci_2:
            neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.num_of_dl_uarfcn = 2;
            neighbour_cells_info_p->rat_type = CN_NEIGHBOUR_RAT_TYPE_3G;

            neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].dl_uarfcn = 10780;
            neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].utra_carrier_rssi = 100;
            neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].num_of_cells = 2;
            neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[0].cpich_ecno = 40;
            neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[0].cpich_pathloss = 62;
            neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[0].cpich_rscp = 3;
            neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[0].psc = 414;
            neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[0].ucid = 0x1387614;
            neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[1].cpich_ecno = 45;
            neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[1].cpich_pathloss = 64;
            neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[1].cpich_rscp = 21;
            neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[1].psc = 424;
            neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[1].ucid = 0x1387254;


            neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[1].dl_uarfcn = 10773;
            neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[1].utra_carrier_rssi = 92;
            neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[1].num_of_cells = 1;
            neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[1].umts_cell_detailed_info[0].cpich_ecno = 32;
            neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[1].umts_cell_detailed_info[0].cpich_pathloss = 66;
            neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[1].umts_cell_detailed_info[0].cpich_rscp = 4;
            neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[1].umts_cell_detailed_info[0].psc = 434;
            neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[1].umts_cell_detailed_info[0].ucid = 0x1387633;
            break;
        }

        *msg_pp = create_message(CN_RESPONSE_GET_NEIGHBOUR_CELLS_EXTD_INFO, CN_SUCCESS, ct, size, neighbour_cells_info_p);
        free(neighbour_cells_info_p);
        break;
    }

    case wb_testcase_ecrat_set_1: /* *ECRAT=0  positive test: unsubscribe */
    case wb_testcase_ecrat_set_2: /* *ECRAT=1  positive test: subscribe */
        *msg_pp = create_message(CN_RESPONSE_SET_EVENT_REPORTING, CN_SUCCESS, ct, 0, NULL);
        break;
    case wb_testcase_ecrat_set_9: /* *ECRAT=0    negative test: CNS failure */
        *msg_pp = create_message(CN_RESPONSE_SET_EVENT_REPORTING, CN_FAILURE, ct, 0, NULL);
        break;
    case wb_testcase_ecrat_read_1: { /* *ECRAT? positive test: UNKNOWN */
        cn_rat_name_t rat_name = CN_RAT_NAME_UNKNOWN;
        *msg_pp = create_message(CN_RESPONSE_RAT_NAME, CN_SUCCESS, ct, sizeof(rat_name), &rat_name);
        break;
    }
    case wb_testcase_ecrat_read_2: { /* *ECRAT? positive test: GSM     */
        cn_rat_name_t rat_name = CN_RAT_NAME_GSM;
        *msg_pp = create_message(CN_RESPONSE_RAT_NAME, CN_SUCCESS, ct, sizeof(rat_name), &rat_name);
        break;
    }
    case wb_testcase_ecrat_read_3: { /* *ECRAT? positive test: UMTS    */
        cn_rat_name_t rat_name = CN_RAT_NAME_UMTS;
        *msg_pp = create_message(CN_RESPONSE_RAT_NAME, CN_SUCCESS, ct, sizeof(rat_name), &rat_name);
        break;
    }
    case wb_testcase_ecrat_read_4: { /* *ECRAT? positive test: EPS     */
        cn_rat_name_t rat_name = CN_RAT_NAME_EPS;
        *msg_pp = create_message(CN_RESPONSE_RAT_NAME, CN_SUCCESS, ct, sizeof(rat_name), &rat_name);
        break;
    }
    case wb_testcase_ecrat_read_5: /* *ECRAT? negative test: CNS failure */
        *msg_pp = create_message(CN_RESPONSE_RAT_NAME, CN_FAILURE, ct, 0, NULL);
        break;
    case wb_testcase_ecrat_unsolicited_1: { /* positive test: EPS */
        cn_rat_name_t rat_name = CN_RAT_NAME_EPS;
        *msg_pp = create_message(CN_EVENT_RAT_NAME, CN_SUCCESS, NULL, sizeof(rat_name), &rat_name);
        break;
    }
    case wb_testcase_ecrat_unsolicited_2: { /* positive test: UMTS */
        cn_rat_name_t rat_name = CN_RAT_NAME_UMTS;
        *msg_pp = create_message(CN_EVENT_RAT_NAME, CN_SUCCESS, NULL, sizeof(rat_name), &rat_name);
        break;
    }
    case wb_testcase_ecrat_unsolicited_3: { /* positive test: GSM */
        cn_rat_name_t rat_name = CN_RAT_NAME_GSM;
        *msg_pp = create_message(CN_EVENT_RAT_NAME, CN_SUCCESS, NULL, sizeof(rat_name), &rat_name);
        break;
    }
    case wb_testcase_ecrat_unsolicited_4: { /* positive test: UNKNOWN */
        cn_rat_name_t rat_name = CN_RAT_NAME_UNKNOWN;
        *msg_pp = create_message(CN_EVENT_RAT_NAME, CN_SUCCESS, NULL, sizeof(rat_name), &rat_name);
        break;
    }

    case wb_testcase_ennir_set_1: /* *ENNIR=0  positive test: unsubscribe */
    case wb_testcase_ennir_set_2: /* *ENNIR=1  positive test: subscribe */
        *msg_pp = create_message(CN_RESPONSE_SET_EVENT_REPORTING, CN_SUCCESS, ct, 0, NULL);
        break;
    case wb_testcase_ennir_set_3: /* *ENNIR=2  negative test */
        *msg_pp = create_message(CN_RESPONSE_SET_EVENT_REPORTING, CN_FAILURE, ct, 0, NULL);
        break;
   case wb_testcase_ennir_unsolicited_1: { /*ENNIR unsolicited  test */
        cn_network_name_info_t info = {
                .mcc_mnc = "12345",
                .name[0] = {{'\0', 'A', '\0', 'i', '\0', 'r', '\0', 't', '\0', 'e', '\0', 'l'}, 12, 0, CN_NETWORK_NAME_DCS_UCS2, 0},
                .name[1] = {{'\0', 'A', '\0', 't'}, 4, 0, CN_NETWORK_NAME_DCS_UCS2, 0},
            };
        *msg_pp = create_message(CN_EVENT_NETWORK_INFO, CN_SUCCESS, ct, sizeof(info), &info);
        break;
    }
    case wb_testcase_epwrred_set_1: { /* positive test, first value    */
        *msg_pp = create_message(CN_RESPONSE_SEND_TX_BACK_OFF_EVENT, CN_SUCCESS, ct, 0, NULL);
        break;
    }
    case wb_testcase_epwrred_set_2: { /* positive test, third value    */
        *msg_pp = create_message(CN_RESPONSE_SEND_TX_BACK_OFF_EVENT, CN_SUCCESS, ct, 0, NULL);
        break;
    }
    case wb_testcase_epwrred_set_3: { /* positive test, middle value   */
        *msg_pp = create_message(CN_RESPONSE_SEND_TX_BACK_OFF_EVENT, CN_SUCCESS, ct, 0, NULL);
        break;
    }
    case wb_testcase_epwrred_set_4: { /* positive test, last value -1  */
        *msg_pp = create_message(CN_RESPONSE_SEND_TX_BACK_OFF_EVENT, CN_SUCCESS, ct, 0, NULL);
        break;
    }
    case wb_testcase_epwrred_set_5: { /* positive test, last value     */
        *msg_pp = create_message(CN_RESPONSE_SEND_TX_BACK_OFF_EVENT, CN_SUCCESS, ct, 0, NULL);
        break;
    }
    case wb_testcase_epwrred_set_6: { /* negative test, negative value */
        *msg_pp = create_message(CN_RESPONSE_SEND_TX_BACK_OFF_EVENT, CN_SUCCESS, ct, 0, NULL);
        break;
    }
    case wb_testcase_eempage_set_ok01:
    case wb_testcase_eempage_set_ok01r:
    case wb_testcase_eempage_set_ok02:
    case wb_testcase_eempage_set_ok03:
    case wb_testcase_eempage_set_ok03r:
    case wb_testcase_eempage_set_ok04:
    case wb_testcase_eempage_set_ok05:
    case wb_testcase_eempage_set_ok06:
    case wb_testcase_eempage_set_ok07:
    case wb_testcase_eempage_set_ok08:
    case wb_testcase_eempage_set_ok09:
    {
        *msg_pp = create_message(CN_RESPONSE_SET_EMPAGE, CN_SUCCESS, ct, 0, NULL);
        break;
    }
    case wb_testcase_eempage_unsolicited1:
    case wb_testcase_eempage_unsolicited2:
    {
        cn_empage_info_t empage;

        empage.page  = 123;
        empage.seqnr = 456;
        *msg_pp = create_message(CN_EVENT_EMPAGE_INFO, CN_SUCCESS, ct, sizeof(cn_empage_info_t), &empage);
         break;
    }

    /* *ESVN */
    case wb_testcase_esvn_read: {
        cn_pp_flag_t pp_flag;
        cn_pp_flag_list_t pp_flag_list;
        memset(&pp_flag, 0, sizeof(pp_flag));
        memset(&pp_flag_list, 0, sizeof(pp_flag_list));
        pp_flag.pp_feature   = 5602;
        pp_flag.pp_value     = 4;
        pp_flag_list.count   = 1;
        pp_flag_list.info[0]    = pp_flag;

        *msg_pp = create_message(CN_RESPONSE_GET_PP_FLAGS, CN_SUCCESS, ct, sizeof(cn_pp_flag_list_t), &pp_flag_list);
        break;
    }

    case wb_testcase_esvn_error1: {
        cn_pp_flag_t pp_flag;
        cn_pp_flag_list_t pp_flag_list;
        memset(&pp_flag, 0, sizeof(pp_flag));
        memset(&pp_flag_list, 0, sizeof(pp_flag_list));
        pp_flag.pp_feature   = 5202;
        pp_flag.pp_value     = 2;
        pp_flag_list.count   = 1;
        pp_flag_list.info[0]    = pp_flag;

        *msg_pp = create_message(CN_RESPONSE_GET_PP_FLAGS, CN_SUCCESS, ct, sizeof(cn_pp_flag_list_t), &pp_flag_list);
        break;
    }

    case wb_testcase_esvn_error2: {
        cn_pp_flag_t pp_flag;
        cn_pp_flag_list_t pp_flag_list;
        memset(&pp_flag, 0, sizeof(pp_flag));
        memset(&pp_flag_list, 0, sizeof(pp_flag_list));
        pp_flag.pp_feature   = 5602;
        pp_flag.pp_value     = 103;
        pp_flag_list.count   = 1;
        pp_flag_list.info[0] = pp_flag;

        *msg_pp = create_message(CN_RESPONSE_GET_PP_FLAGS, CN_SUCCESS, ct, sizeof(cn_pp_flag_list_t), &pp_flag_list);
        break;
    }


#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
#include <inc_wb_extended_stub_test_cases.h>
#endif
    default:
        printf("%s: unknown testcase %d!\n", __FUNCTION__, CURRENT_TEST_CASE);
        goto error;
    }

    *size_p = 0;
    return result;

error:
    return CN_FAILURE;
}


/*************************************************************************
 *       FUNCTIONS FOR HANDLING THE CONNECTION TO THE C&N SERVICE        *
 *************************************************************************/

cn_error_code_t cn_client_init(cn_context_t **context_pp)
{
    if (context_pp) {
        *context_pp = (cn_context_t *)123;
    }

    return CN_SUCCESS;
}


cn_error_code_t cn_client_get_request_fd(cn_context_t *context_p, int *fd_p)
{
    if (fd_p) {
        *fd_p = FD_CN_REQUEST;
    }

    return CN_SUCCESS;
}


cn_error_code_t cn_client_get_event_fd(cn_context_t *context_p, int *fd_p)
{
    if (fd_p) {
        *fd_p = FD_CN_EVENT;
    }

    return CN_SUCCESS;
}


cn_error_code_t cn_client_shutdown(cn_context_t *context_p)
{
    return CN_SUCCESS;
}


/*************************************************************************
 *                           REQUEST FUNCTIONS                           *
 *************************************************************************/

cn_error_code_t cn_request_rf_on(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    ct = client_tag;

    if (wb_testcase_cfun_failure == CURRENT_TEST_CASE) {
        return CN_FAILURE;
    }

    return CN_SUCCESS;
}


cn_error_code_t cn_request_rf_off(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_rf_status(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    ct = client_tag;

    if (wb_testcase_cfun_read_failure == CURRENT_TEST_CASE && state == 0) {
        return CN_FAILURE;
    }

    return CN_SUCCESS;
}


cn_error_code_t cn_request_set_preferred_network_type(cn_context_t *context_p, cn_network_type_t type, cn_client_tag_t client_tag)
{
    ct = client_tag;

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_cfun_3:
        WB_CHECK(type == CN_NETWORK_TYPE_GSM_ONLY);
        break;
    case wb_testcase_cfun_4:
        WB_CHECK(type == CN_NETWORK_TYPE_WCDMA_ONLY);
        break;
    }

    return CN_SUCCESS;
}


cn_error_code_t cn_request_get_preferred_network_type(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    ct = client_tag;

    if (wb_testcase_cfun_read_failure == CURRENT_TEST_CASE && state == 1) {
        return CN_FAILURE;
    }

    return CN_SUCCESS;
}


cn_error_code_t cn_request_registration_state_normal(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_registration_state_gprs(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_automatic_network_registration(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_manual_network_registration(cn_context_t *context_p, cn_network_registration_req_data_t *req_data_p, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_network_deregister(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_net_query_mode(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_manual_network_search(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_interrupt_network_search(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_current_call_list(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_dial(cn_context_t *context_p, cn_dial_t *dial_p, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_hangup(cn_context_t *context_p, cn_call_state_filter_t filter, cn_uint8_t call_id, cn_client_tag_t client_tag)
{
    switch (CURRENT_TEST_CASE) {
    case wb_testcase_ceer_h_no_call:
    case wb_testcase_ceer_h_ok:
    case wb_testcase_ceer_h_ok_unspec:
        state = 1;  /* response message will be CN_SUCCESS or CN_FAILURE with cause attached */
        break;
    default:
        break;  /* needed to make the compiler happy */
    }

    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_swap_calls(cn_context_t *context_p, cn_uint8_t active_call_id, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_hold_call(cn_context_t *context_p, cn_uint8_t active_call_id, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_resume_call(cn_context_t *context_p, cn_uint8_t hold_call_id, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_conference_call(cn_context_t *context_p, cn_uint8_t active_call_id, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_conference_call_split(cn_context_t *context_p, cn_uint8_t active_call_id, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_explicit_call_transfer(cn_context_t *context_p, cn_uint8_t call_id, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_answer_call(cn_context_t *context_p, cn_uint8_t call_id, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_ussd(cn_context_t *context_p, cn_ussd_info_t *cn_ussd_info_p, cn_client_tag_t client_tag)
{
    ct = client_tag;

    if (wb_testcase_cusd_hex == CURRENT_TEST_CASE) {
        size_t length_limit = sizeof(EXECUTOR.wb_test_ussd_str) - 1;
        size_t ussd_request_length = (size_t)cn_ussd_info_p->length;

        uint8_t dcs = cn_ussd_info_p->dcs;

        if (ussd_request_length > length_limit) {
            printf("%s: ERROR mal_ss_request_ussd() string is too long\n", __FUNCTION__);
        } else {
            memmove(EXECUTOR.wb_test_ussd_str, cn_ussd_info_p->ussd_string, ussd_request_length); /* Assume it is zero filled. */
        }
    }

    return CN_SUCCESS;
}


cn_error_code_t cn_request_ussd_abort(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_rssi_value(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    ct = client_tag;

    if (wb_testcase_csq_do_fail_mal == CURRENT_TEST_CASE) {
        return CN_FAILURE;
    }

    return CN_SUCCESS;
}


cn_error_code_t cn_request_clip_status(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_cnap_status(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_colr_status(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_clir_status(cn_context_t *context_p, cn_client_tag_t client_tag)
{

    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_set_clir(cn_context_t *context_p, cn_clir_setting_t clir_setting, cn_client_tag_t client_tag)
{

    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_query_call_forward(cn_context_t *context_p, cn_call_forward_info_t *call_forward_info_p, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_set_call_forward(cn_context_t *context_p, cn_call_forward_info_t *call_forward_info_p, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_set_modem_property(cn_context_t *context_p, cn_modem_property_t modem_property, cn_client_tag_t client_tag)
{

    ct = client_tag;

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_elin_set_1:
    case wb_testcase_elin_set_2:
    case wb_testcase_elin_set_2_no_support:
        WB_CHECK(CN_MODEM_PROPERTY_TYPE_ALS == modem_property.type);
        break;
    default:
        break;
    }/* end of switch-statement */

    return CN_SUCCESS;
}

cn_error_code_t cn_request_dtmf_send(cn_context_t *context_p,
                                     char *dtmf_string_p,
                                     cn_uint16_t length,
                                     cn_dtmf_string_type_t string_type,
                                     cn_uint16_t dtmf_duration_time,
                                     cn_uint16_t dtmf_pause_time,
                                     cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_dtmf_start(cn_context_t *context_p, char character, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_dtmf_stop(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_set_call_waiting(cn_context_t *context_p, cn_call_waiting_t *call_waiting_p, cn_client_tag_t client_tag)
{

    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_get_call_waiting(cn_context_t *context_p, cn_uint32_t service_class, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_change_barring_password(cn_context_t *context_p, char *facility_p, char *old_passwd_p, char *new_passwd_p, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_query_call_barring(cn_context_t *context_p, char *facility_p, cn_uint32_t service_class, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_set_call_barring(cn_context_t *context_p, char *facility_p, cn_call_barring_t *call_barring_p, char *passwd_p, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_set_cssn(cn_context_t *context_p, cn_cssn_setting_t cssn_setting, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_set_user_activity_status(cn_context_t *context_p, cn_user_status_t user_status, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_set_product_profile_flag(cn_context_t *context_p, cn_uint16_t flag_id, cn_uint16_t flag_value, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_set_neighbour_cells_reporting(cn_context_t *context_p, cn_bool_t enable_reporting, cn_neighbour_rat_type_t rat_type, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_get_neighbour_cells_reporting(cn_context_t *context_p, cn_neighbour_rat_type_t rat_type, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}


cn_error_code_t cn_request_get_neighbour_cells_complete_info(cn_context_t *context_p, cn_neighbour_rat_type_t rat_type, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}

cn_error_code_t cn_request_get_neighbour_cells_extd_info(cn_context_t *context_p, cn_neighbour_rat_type_t rat_type, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}

cn_error_code_t cn_request_get_modem_property(cn_context_t *context_p, cn_modem_property_type_t type, cn_client_tag_t client_tag)
{
    ct = client_tag;

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_ceer_h_no_call:
    case wb_testcase_ceer_h_ok:
    case wb_testcase_ceer_h_ok_unspec:
        state = 1;  /* response message will be CN_SUCCESS or CN_FAILURE with cause attached */
        break;
    case wb_testcase_elin_set_1:
    case wb_testcase_elin_set_2:
    case wb_testcase_elin_set_2_no_support:
        WB_CHECK(CN_MODEM_PROPERTY_TYPE_ALS == type);
    default:
        break;  /* needed to make the compiler happy */
    }

    return CN_SUCCESS;
}

cn_error_code_t cn_request_set_signal_info_config(cn_context_t *context_p, cn_signal_info_config_t *config_p, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}

cn_error_code_t cn_request_get_signal_info_config(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}

cn_error_code_t cn_request_set_signal_info_reporting(cn_context_t *context_p, cn_rssi_mode_t mode, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}

cn_error_code_t cn_request_set_event_reporting(cn_context_t *context_p, cn_event_reporting_type_t type, cn_bool_t enable_reporting, cn_client_tag_t client_tag)
{
    switch (CURRENT_TEST_CASE) {
    case wb_testcase_ecrat_set_1: /* *ECRAT=0  positive test: unsubscribe */
        WB_CHECK(CN_EVENT_REPORTING_TYPE_RAT_NAME == type);
        WB_CHECK(0 == enable_reporting);
        break;
    case wb_testcase_ecrat_set_2: /* *ECRAT=1  positive test: subscribe   */
        WB_CHECK(CN_EVENT_REPORTING_TYPE_RAT_NAME == type);
        WB_CHECK(1 == enable_reporting);
        break;
    default:
        break;
    }

    ct = client_tag;
    return CN_SUCCESS;
}

cn_error_code_t cn_request_rab_status(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}

cn_error_code_t cn_request_modify_emergency_number_list(cn_context_t *context_p, cn_emergency_number_operation_t operation, cn_emergency_number_config_t *config_p, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}

cn_error_code_t cn_request_get_emergency_number_list(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}

cn_error_code_t cn_request_baseband_version(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}

cn_error_code_t cn_request_manual_network_registration_with_automatic_fallback(cn_context_t *context_p, cn_network_registration_req_data_t *req_data_p, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}

cn_error_code_t cn_request_reg_status_event_config(cn_context_t *context_p, cn_reg_status_trigger_level_t trigger_level, cn_client_tag_t client_tag)
{
    printf("%s: trigger_level: %d\n", __func__, trigger_level);

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_creg_reg_presentation_1:
    case wb_testcase_creg_reg_presentation_2:
        WB_CHECK(1 == trigger_level);
        break;
    case wb_testcase_creg_reg_loc_presentation_1:
    case wb_testcase_creg_reg_loc_presentation_2:
    case wb_testcase_creg_reg_loc_presentation_3:
        WB_CHECK(2 == trigger_level);
        break;
    default:
        break;
    } /* end of switch-statement */

    ct = client_tag;
    return CN_SUCCESS;
}

static int cn_client_connect(const char *path)
{
    return 234;
}


cn_error_code_t cn_request_set_hsxpa_mode(cn_context_t *context_p, cn_hsxpa_mode_t hsxpa_mode, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}

cn_error_code_t cn_request_get_hsxpa_mode(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}

cn_error_code_t cn_request_rat_name(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}

cn_error_code_t cn_request_modem_power_off(cn_context_t *context_p, cn_client_tag_t client_tag)
{
    ct = client_tag;
    return CN_SUCCESS;
}

cn_error_code_t cn_request_send_tx_back_off_event(cn_context_t *context_p, cn_tx_back_off_event_t event, cn_client_tag_t client_tag)
{
    switch (CURRENT_TEST_CASE) {
    case wb_testcase_epwrred_set_1: /* positive case, first value    */
        WB_CHECK(CN_TX_BACK_OFF_EVENT_NO_EVENT == event);
        break;
    case wb_testcase_epwrred_set_2: /* positive case, third value    */
        WB_CHECK(CN_TX_BACK_OFF_EVENT_FOLD_SLIDE_MECHANISM_CLOSED == event);
        break;
    case wb_testcase_epwrred_set_3: /* positive case, middle value   */
        WB_CHECK(CN_TX_BACK_OFF_EVENT_FOLD_SLIDE_MECHANISM_OPEN == event);
        break;
    case wb_testcase_epwrred_set_4: /* positive case, last value -1  */
        WB_CHECK(CN_TX_BACK_OFF_EVENT_ANTENNA_INACTIVE == event);
        break;
    case wb_testcase_epwrred_set_5: /* positive case, last value     */
        WB_CHECK(CN_TX_BACK_OFF_EVENT_LOW_BATTERY_STATE_INACTIVE == event);
        break;
    case wb_testcase_epwrred_set_6: /* negative case, negative value */
        WB_CHECK(CN_TX_BACK_OFF_EVENT_LOW_BATTERY_STATE_ACTIVE == event);
        break;
    }

    ct = client_tag;
    return CN_SUCCESS;
}

cn_error_code_t cn_request_reset_modem(cn_context_t *context_p, cn_client_tag_t client_tag)
{
      ct = client_tag;
      return CN_SUCCESS;
}

cn_error_code_t cn_request_set_empage(cn_context_t *context_p, cn_empage_t *empage_p , cn_client_tag_t client_tag)
{
      ct = client_tag;
      return CN_SUCCESS;
}

cn_error_code_t cn_request_get_pp_flags(cn_context_t *context_p, cn_bool_t all_flags, cn_uint16_t flag_id,  cn_client_tag_t client_tag)
{
      ct = client_tag;
      return CN_SUCCESS;
}

cn_error_code_t cn_request_sleep_test_mode(cn_context_t *context_p, cn_sleep_mode_setting_t sleep_mode, cn_client_tag_t client_tag)
{
      ct = client_tag;
      return CN_SUCCESS;
}

cn_error_code_t cn_request_reset_modem_with_dump(cn_context_t *context_p, cn_cpu_type_t cpu_type, cn_client_tag_t client_tag)
{
      ct = client_tag;
      return CN_SUCCESS;
}

static void init_dummy_call_context(cn_call_context_t *context_p)
{
    assert(NULL != context_p);
    context_p->call_state = CN_CALL_STATE_IDLE;
    context_p->prev_call_state = CN_CALL_STATE_IDLE;
    context_p->call_id = 1;
    context_p->address_type = 129; /* 129 = address type unknown */
    context_p->is_multiparty = false;
    context_p->is_MT = false;
    context_p->als = false;
    context_p->mode = CN_CALL_MODE_SPEECH;
    context_p->is_voice_privacy = false;
    context_p->cause_no_cli = CN_CAUSE_NO_CLI_UNAVAILABLE;
    context_p->call_cause.cause_type_sender = CN_CALL_CAUSE_TYPE_SENDER_NONE;
    context_p->call_cause.cause = (cn_call_cause_cause_t)0;
    context_p->number_presentation = CN_PRESENTATION_ALLOWED;
    strncpy(context_p->number, "", CN_MAX_STRING_SIZE);
    context_p->name_presentation = CN_PRESENTATION_ALLOWED;
    context_p->name[0] = 0;
    context_p->user_to_user_len = 0;
}
