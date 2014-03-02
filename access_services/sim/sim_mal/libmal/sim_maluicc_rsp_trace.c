/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : sim_maluicc_rsp_trace.c
 * Description     : Utility trace
 *
 * Author          : Steve Critchlow <steve.critchlow@stericsson.com>
 *
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "simd.h" // For event stream, logging functions etc.
#include "sim_mal_defs.h"
#include "mal_uicc.h"
#include "mal_uicc_apdu.h"
#include "sim_trace_util.h"
#include "sim_log.h"



#define LOG_NDEBUG 0
#ifndef LOCAL_COMPILE_TEST
#define LOG_TAG "RIL MAL-RSP"   // MAL-Response
#include <cutils/log.h>
#else
#define LOGI(x)
#define LOGD(x)
#define LOGV(x)
#endif

static void log_f(sim_logging_t level, const char *fmt, ...)
    __attribute__ ((format (printf, 2, 3)));

static void log_f(sim_logging_t level, const char *fmt, ...)
{
    char    buf[1024];
    va_list arg;
    size_t  len;

    if ( level >  sim_get_log_level()) return;

    va_start(arg, fmt);
    vsnprintf(buf, sizeof(buf), fmt, arg);
    len = strlen(buf);
    if ( buf[len-1] == '\n' ) buf[len-1] = '\0';    // clip the newline
    switch ( level ) {
    case SIM_LOGGING_E:
        catd_log_s(SIM_LOGGING_E, buf, 0);
        break;
    case SIM_LOGGING_I:
        catd_log_s(SIM_LOGGING_I, buf, 0);
        break;
    case SIM_LOGGING_D:
        catd_log_s(SIM_LOGGING_D, buf, 0);
        break;
    case SIM_LOGGING_V:
        catd_log_s(SIM_LOGGING_V, buf, 0);
        break;
    }
    va_end(arg);
}

/**
 * @brief This function dumps all the callback messages in varying levels of verbosity
 *
 * Adapted from the modem lib uicc_test.c file.
 */
void sim_maluicc_indication_response_trace(int message_id, void* data, int UNUSED(mal_error), void* UNUSED(client_tag))
{
    uint8_t i, num_sb;
    uint8_t pin_indication = 0;
    uint8_t pin_sw1 = 0;
    uint8_t pin_sw2 = 0;
    mal_uicc_resp_sim_status_t* uicc_resp_sim_status = NULL;
    mal_uicc_appln_resp_t* uicc_appln_resp = NULL;
    mal_uicc_card_resp_t* uicc_card_resp = NULL;
    uicc_appl_cmd_resp_t* uicc_appl_cmd = NULL;
    uicc_pin_ind_t* uicc_pin_ind = NULL;
    uicc_pin_resp_t* uicc_pin_resp = NULL;
    uicc_cat_resp_t* uicc_cat_resp = NULL;
    uicc_ind_t* uicc_ind = NULL;
    uicc_card_ind_t* uicc_card_ind = NULL;
    uicc_application_ind_t* uicc_application_ind = NULL;
    uicc_cat_ind_t *uicc_cat_ind = NULL;
    uicc_apdu_resp_t *uicc_apdu_resp = NULL;

    if (!data) {
        log_f(0, "MAL indication response trace: NULL data received, message_id = 0x%x\n", message_id);
        return;
    }

    switch(message_id)
    {
    case MAL_UICC_IND:
        log_f(0,"sim_mal_cbdh MAL_UICC_IND\n");
        uicc_ind = (uicc_ind_t*)(data);
        log_f(1,"uicc_ind->service_type = %s\n", sim_trace_lookup_service_type(uicc_ind->service_type));
//        if(uicc_ind->service_type == MAL_UICC_START_UP_COMPLETE)
//            log_f(1,"uicc ind status MAL_UICC_START_UP_COMPLETE\n");
//        else if(uicc_ind->service_type == MAL_UICC_SHUTTING_DOWN)
//            log_f(1,"uicc ind status MAL_UICC_SHUTTING_DOWN\n");
    break;

    case MAL_UICC_RESP:
        log_f(0,"sim_mal_cbdh MAL_UICC_RESP\n");
        uicc_resp_sim_status = (mal_uicc_resp_sim_status_t*)(data);
        log_f(1,"uicc_resp_sim_status->service_type = %s\n", sim_trace_lookup_service_type(uicc_resp_sim_status->service_type));
        log_f(1,"uicc_resp_sim_status->status = %x, details = %x\n", uicc_resp_sim_status->status, uicc_resp_sim_status->details);
        log_f(1,"uicc_resp_sim_status->server_status = %x\n", uicc_resp_sim_status->server_status);

//        if(uicc_resp_sim_status->status == MAL_UICC_STATUS_FAIL)
//            break;

//        if(uicc_resp_sim_status->server_status == MAL_UICC_STATUS_NOT_READY)
//            log_f(1,"UICC Server Not Ready : %x\n", uicc_resp_sim_status->server_status);
//        if(uicc_resp_sim_status->server_status == MAL_UICC_STATUS_START_UP_COMPLETED)
//            log_f(1,"UICC Server Ready : %x\n", uicc_resp_sim_status->server_status);
    break;

    case MAL_UICC_CARD_IND:
        log_f(0,"sim_mal_cbdh MAL_UICC_CARD_IND\n");
        uicc_card_ind = (uicc_card_ind_t*)(data);
        log_f(1,"uicc_card_ind->service_type = %s\n", sim_trace_lookup_service_type(uicc_card_ind->service_type));
        log_f(1,"uicc_card_ind->card_type = %s\n", sim_trace_lookup_card_type(uicc_card_ind->card_type));
        log_f(1,"uicc_card_ind->n_sb = %x\n", uicc_card_ind->n_sb);

//        if(uicc_card_ind->service_type == MAL_UICC_CARD_DISCONNECTED)
//            log_f(2,"Card status MAL_UICC_CARD_DISCONNECTED\n");
//        else if(uicc_card_ind->service_type == MAL_UICC_CARD_REMOVED)
//            log_f(2,"Card status MAL_UICC_CARD_REMOVED\n");
//        else if(uicc_card_ind->service_type == MAL_UICC_CARD_NOT_PRESENT)
//            log_f(2,"Card status MAL_UICC_CARD_NOT_PRESENT\n");
//        else if(uicc_card_ind->service_type == MAL_UICC_CARD_READY)
//            log_f(2,"Card status MAL_UICC_CARD_READY\n");
//        else if(uicc_card_ind->service_type == MAL_UICC_CARD_REJECTED)
//            log_f(2,"Card status MAL_UICC_CARD_REJECTED\n");

        if(uicc_card_ind->n_sb)
        {
            if(uicc_card_ind->service_type == MAL_UICC_CARD_READY)
            {
/*                 log_f(2,"uicc_card_ind->uicc_sb_card_info.voltage_class = %x\n", */
/*                         uicc_card_ind->uicc_sb_card_info.voltage_class); */
/*                 log_f(2,"uicc_card_ind->uicc_sb_card_info.clf_support = %x\n", */
/*                         uicc_card_ind->uicc_sb_card_info.clf_support); */
            }
            if(uicc_card_ind->service_type == MAL_UICC_CARD_REJECTED)
            {
/*                 log_f(2,"uicc_card_ind->uicc_sb_card_reject_cause.reject_cause = %x\n", */
/*                         uicc_card_ind->uicc_sb_card_reject_cause.reject_cause); */
            }
        }
    break;

    case MAL_UICC_CARD_RESP:
        log_f(0,"sim_mal_cbdh MAL_UICC_CARD_RESP\n");
        uicc_card_resp = (mal_uicc_card_resp_t*)(data);
        log_f(1,"uicc_card_resp->service_type = %s\n", sim_trace_lookup_service_type(uicc_card_resp->service_type));
        log_f(1,"uicc_card_resp->status = %x, details = %x\n", uicc_card_resp->status, uicc_card_resp->details);
        log_f(1,"uicc_card_resp->card_type = %s\n", sim_trace_lookup_card_type(uicc_card_resp->card_type));
        log_f(1,"uicc_card_resp->n_sb = %x\n", uicc_card_resp->n_sb);

        if(uicc_card_resp->status == MAL_UICC_STATUS_FAIL)
            break;

        if(uicc_card_resp->n_sb)
        {
            if(uicc_card_resp->service_type == MAL_UICC_CARD_STATUS_GET)
            {
/*                 uint8_t card_status; */
/*                 card_status = uicc_card_resp->uicc_sb_card_status_get.card_status; */

/*                 if(card_status == MAL_UICC_STATUS_CARD_READY) */
/*                     log_f(2,"MAL_UICC_STATUS_CARD_READY UICC CARD STATUS: %x\n", card_status); */
/*                 else if(card_status == MAL_UICC_STATUS_CARD_NOT_READY) */
/*                     log_f(2,"MAL_UICC_STATUS_CARD_NOT_READY UICC CARD STATUS:%x\n", card_status); */
/*                 else if(card_status == MAL_UICC_STATUS_CARD_DISCONNECTED) */
/*                     log_f(2,"MAL_UICC_STATUS_CARD_DISCONNECTED UICC CARD STATUS: %x\n", card_status); */
/*                 else if(card_status == MAL_UICC_STATUS_CARD_NOT_PRESENT) */
/*                     log_f(2,"MAL_UICC_STATUS_CARD_NOT_PRESENT UICC CARD STATUS:%x\n", card_status); */
            }
            else if(uicc_card_resp->service_type == MAL_UICC_CARD_INFO_GET)
            {
/*                 log_f(2,"uicc_card_resp->uicc_sb_card_info_get.voltage_class = %d\n", */
/*                         uicc_card_resp->uicc_sb_card_info_get.voltage_class); */
/*                 log_f(2,"uicc_card_resp->uicc_sb_card_info_get.clf_support = %d\n", */
/*                         uicc_card_resp->uicc_sb_card_info_get.clf_support); */
            }
        }
    break;

    case MAL_UICC_APPLICATION_IND:
        log_f(0,"sim_mal_cbdh MAL_UICC_APPLICATION_IND\n");
        uicc_application_ind = (uicc_application_ind_t*)(data);
        log_f(1,"uicc_application_ind->service_type = %s\n", sim_trace_lookup_service_type(uicc_application_ind->service_type));
        log_f(1,"uicc_application_ind->appl_id = %x\n", uicc_application_ind->appl_id);
        log_f(1,"uicc_application_ind->n_sb = %x\n", uicc_application_ind->n_sb);

//        if(uicc_application_ind->service_type == MAL_UICC_APPL_TERMINATED)
//            log_f(2,"Application status MAL_UICC_APPL_TERMINATED\n");
//        else if(uicc_application_ind->service_type == MAL_UICC_APPL_RECOVERED)
//            log_f(2,"Application status MAL_UICC_APPL_RECOVERED\n");
//        else if(uicc_application_ind->service_type == MAL_UICC_APPL_ACTIVATED)
//            log_f(2,"Application status MAL_UICC_APPL_ACTIVATED\n");

        if(uicc_application_ind->n_sb)
        {
/*             log_f(2,"uicc_application_ind->sb_application.application_type = %x\n", */
/*                     uicc_application_ind->sb_application.application_type); */
/*             log_f(2,"uicc_application_ind->sb_application.appl_id = %x\n", */
/*                     uicc_application_ind->sb_application.appl_id); */
        }
    break;

    case MAL_UICC_APPLICATION_RESP:
        log_f(0,"sim_mal_cbdh MAL_UICC_APPLICATION_RESP\n");
        uicc_appln_resp = (mal_uicc_appln_resp_t*)(data);
        log_f(1,"uicc_appln_resp->service_type = %s\n", sim_trace_lookup_service_type(uicc_appln_resp->service_type));
        log_f(1,"uicc_appln_resp->status = %x, details = %x\n", uicc_appln_resp->status, uicc_appln_resp->details);
        log_f(1,"uicc_appln_resp->card_type = %s\n", sim_trace_lookup_card_type(uicc_appln_resp->card_type));
        log_f(1,"uicc_appln_resp->n_sb = %x\n", uicc_appln_resp->n_sb);

        if(uicc_appln_resp->status == MAL_UICC_STATUS_FAIL)
            break;

        num_sb = uicc_appln_resp->n_sb;

        if(num_sb)
        {
            switch  (uicc_appln_resp->service_type)
            {
                case MAL_UICC_APPL_LIST:
                {
                    for(i = 0; i < num_sb; i++)
                    {
                        if(uicc_appln_resp->sub_block.uicc_appln_list.uicc_sb_appl_data_object[i] != NULL)
                        {
                            uicc_sb_appl_data_object_t  *p = uicc_appln_resp->sub_block.uicc_appln_list.uicc_sb_appl_data_object[i];
                            log_f(2,"uicc_appln_resp->list[%d]->application_type = %x\n", i, p->application_type);
                            log_f(2,"uicc_appln_resp->list[%d]->appl_id = %x\n", i, p->appl_id);
                            log_f(2,"uicc_appln_resp->list[%d]->appl_status = %x\n", i, p->appl_status);
                            log_f(2,"uicc_appln_resp->list[%d]->appl_do_len = %d\n", i, p->appl_do_len );
                            log_f(2,"printing data ...\n");
                            sim_trace_print_data(p->appl_do, p->appl_do_len,log_f);
                        }
                    }
                    break;
                }

                case MAL_UICC_APPL_HOST_ACTIVATE:
                {
                    if(uicc_appln_resp->card_type == MAL_UICC_CARD_TYPE_ICC)
                        log_f(2,"card type is ICC  : %x\n", uicc_appln_resp->card_type);
                    else if (uicc_appln_resp->card_type == MAL_UICC_CARD_TYPE_UICC)
                        log_f(2,"card type is UICC  : %x\n", uicc_appln_resp->card_type);
                    else if (uicc_appln_resp->card_type == MAL_UICC_CARD_TYPE_UNKNOWN)
                        log_f(2,"card type is UNKNOWN  : %x\n", uicc_appln_resp->card_type);

                    for(i = 0; i < MAX_CHV_LIST_SIZE; i++)
                    {
                        if((uicc_appln_resp->sub_block.uicc_appln_host_activate.uicc_sb_chv[i]) != NULL)
                        {
                            log_f(2,"uicc_appln_resp->sub_block.uicc_appln_host_activate.uicc_sb_chv[%d]->chv_qualifier = %x\n",
                                    i, uicc_appln_resp->sub_block.uicc_appln_host_activate.uicc_sb_chv[i]->chv_qualifier);
                            log_f(2,"uicc_appln_resp->sub_block.uicc_appln_host_activate.uicc_sb_chv[%d]->pin_id = %x\n",
                                    i, uicc_appln_resp->sub_block.uicc_appln_host_activate.uicc_sb_chv[i]->pin_id);
                        }
                    }

                    log_f(2,"uicc_appln_resp->sub_block.uicc_appln_host_activate.uicc_sb_client.client_id  : %d\n",
                            uicc_appln_resp->sub_block.uicc_appln_host_activate.uicc_sb_client.client_id);

                    break;
                }

                case MAL_UICC_APPL_STATUS_GET:
                {
                    log_f(2,"uicc_appln_resp->sub_block.uicc_appl_status_get.uicc_sb_appl_status.appl_status = %x\n",
                            uicc_appln_resp->sub_block.uicc_appl_status_get.uicc_sb_appl_status.appl_status);
                    if(uicc_appln_resp->sub_block.uicc_appl_status_get.uicc_sb_appl_status.appl_status == MAL_UICC_STATUS_APPL_ACTIVE)
                        log_f(2,"APPLICATION IS ACTIVE\n");
                    else if(uicc_appln_resp->sub_block.uicc_appl_status_get.uicc_sb_appl_status.appl_status == MAL_UICC_STATUS_APPL_NOT_ACTIVE)
                        log_f(2,"APPLICATION IS INACTIVE\n");
                    break;
                }

                default:
                {
                    log_f(2,"No decode for service type=%x\n",uicc_appln_resp->service_type);
                    break;
                }
            }
        }
    break;

    case MAL_UICC_APPL_CMD_IND:
        //uicc_appl_cmd = (uicc_appl_cmd_resp_t*)(data);
        log_f(0,"sim_mal_cbdh MAL_UICC_APPL_CMD_IND\n");
        log_f(0,"This has no associated structure\n");
    break;

    case MAL_UICC_APPL_CMD_RESP:
        log_f(0,"sim_mal_cbdh MAL_UICC_APPL_CMD_RESP\n");
        uicc_appl_cmd = (uicc_appl_cmd_resp_t*)(data);
        log_f(1,"uicc_appl_cmd->service_type = %s\n", sim_trace_lookup_service_type(uicc_appl_cmd->service_type));
        log_f(1,"uicc_appl_cmd->status = %x, details = %x\n", uicc_appl_cmd->status, uicc_appl_cmd->details);
        log_f(1,"uicc_appl_cmd->card_type = %s\n", sim_trace_lookup_card_type(uicc_appl_cmd->card_type));
        log_f(1,"uicc_appl_cmd->n_sb = %x\n", uicc_appl_cmd->n_sb);

        if(uicc_appl_cmd->status == MAL_UICC_STATUS_FAIL)
            break;

        if(uicc_appl_cmd->n_sb)
        {
            if((uicc_appl_cmd->service_type == MAL_UICC_APPL_READ_TRANSPARENT)||
               (uicc_appl_cmd->service_type == MAL_UICC_APPL_READ_LINEAR_FIXED))
            {
/*                 uint8_t* fileid_data; */
/*                 int data_length; */
/*                 data_length = uicc_appl_cmd->sub_block.data.data_length; */
/*                 log_f(2,"data length = %d\n",data_length ); */
/*                 log_f(2,"printing data ...\n"); */
/*                 fileid_data = uicc_appl_cmd->sub_block.data.data; */
/*                 sim_trace_print_data(fileid_data, data_length,log_f); */
            }
            else if(uicc_appl_cmd->service_type == MAL_UICC_APPL_FILE_INFO)
            {
/*                 uint8_t* fileid_data; */
/*                 int data_length; */
/*                 data_length = uicc_appl_cmd->sub_block.uicc_sb_fci.fci_length; */
/*                 log_f(2,"data length = %d\n",data_length ); */
/*                 log_f(2,"printing data ...\n"); */
/*                 fileid_data = uicc_appl_cmd->sub_block.uicc_sb_fci.fci; */
/*                 sim_trace_print_data(fileid_data, data_length,log_f); */
            }
            else if ( (uicc_appl_cmd->service_type == MAL_UICC_APPL_UPDATE_TRANSPARENT) ||
                      (uicc_appl_cmd->service_type == MAL_UICC_APPL_UPDATE_LINEAR_FIXED) )
            {
                log_f(2,"MAL_UICC_APPL_UPDATE decode not implemented yet!\n");
            }
            else if (uicc_appl_cmd->service_type == MAL_UICC_APPL_APDU_SEND) {
                uicc_sb_apdu_t  *p = &uicc_appl_cmd->sub_block.appl_apdu_send_resp.uicc_sb_apdu;
                log_f(2,"cmd_force = %d, apdu_len=%d\n",
                    p->cmd_force,
                    p->apdu_length );
                sim_trace_print_data(p->apdu, p->apdu_length,log_f);
            }
        }
    break;

    case MAL_UICC_PIN_IND:
        log_f(0,"sim_mal_cbdh MAL_UICC_PIN_IND\n");
        uicc_pin_ind = (uicc_pin_ind_t*)(data);
        log_f(1,"uicc_pin_ind->service_type = %s\n", sim_trace_lookup_service_type(uicc_pin_ind->service_type));
        log_f(1,"uicc_pin_ind->pin_id = %x\n", uicc_pin_ind->pin_id);
        log_f(1,"uicc_pin_ind->appl_id = %x\n", uicc_pin_ind->appl_id);

        pin_indication  = uicc_pin_ind->service_type;

        if(pin_indication == MAL_UICC_PIN_VERIFY_NEEDED)
            log_f(2,"UICC_PIN_VERIFY_NEEDED ........\n");

        if(pin_indication == MAL_UICC_PIN_VERIFIED)
            log_f(2,"UICC_PIN_VERIFIED ........\n");

        if(pin_indication == MAL_UICC_PIN_UNBLOCK_NEEDED)
            log_f(2,"UICC_PIN_UNBLOCK_NEEDED ........\n");
    break;

    case MAL_UICC_PIN_RESP:
        log_f(0,"sim_mal_cbdh MAL_UICC_PIN_RESP\n");
        uicc_pin_resp = (uicc_pin_resp_t*)(data);
        log_f(1,"uicc_pin_resp->service_type = %s\n", sim_trace_lookup_service_type(uicc_pin_resp->service_type));
        log_f(1,"uicc_pin_resp->status = %x, details = %x\n", uicc_pin_resp->status, uicc_pin_resp->details);
        log_f(1,"uicc_pin_resp->card_type = %s\n", sim_trace_lookup_card_type(uicc_pin_resp->card_type));
        log_f(1,"uicc_pin_resp->n_sb = %x\n", uicc_pin_resp->n_sb);

        if(uicc_pin_resp->status == MAL_UICC_STATUS_FAIL)
            break;

        if(uicc_pin_resp->n_sb)
        {
            switch  (uicc_pin_resp->service_type)
            {
                case MAL_UICC_PIN_ENABLE:
                {
                    pin_sw1 = uicc_pin_resp->sub_block_t.uicc_pin_enable_resp.pin_uicc_sb_status_word.sw1;
                    pin_sw2 = uicc_pin_resp->sub_block_t.uicc_pin_enable_resp.pin_uicc_sb_status_word.sw2;
                    log_f(2,"PIN service type = MAL_UICC_PIN_ENABLE, sw1=%x, sw2=%x\n",pin_sw1,pin_sw2);
                    break;
                }
                case MAL_UICC_PIN_DISABLE:
                {
                    pin_sw1 = uicc_pin_resp->sub_block_t.uicc_pin_disable_resp.pin_uicc_sb_status_word.sw1;
                    pin_sw2 = uicc_pin_resp->sub_block_t.uicc_pin_disable_resp.pin_uicc_sb_status_word.sw2;
                    log_f(2,"PIN service type = MAL_UICC_PIN_DISABLE, sw1=%x, sw2=%x\n",pin_sw1,pin_sw2);
                    break;
                }
                case MAL_UICC_PIN_VERIFY:
                {
                    pin_sw1 = uicc_pin_resp->sub_block_t.uicc_pin_verify_resp.pin_uicc_sb_status_word.sw1;
                    pin_sw2 = uicc_pin_resp->sub_block_t.uicc_pin_verify_resp.pin_uicc_sb_status_word.sw2;
                    log_f(2,"PIN service type = MAL_UICC_PIN_VERIFY, sw1=%x, sw2=%x\n",pin_sw1,pin_sw2);
                    break;
                }
                case MAL_UICC_PIN_CHANGE:
                {
                    pin_sw1 = uicc_pin_resp->sub_block_t.uicc_pin_change_resp.pin_uicc_sb_status_word.sw1;
                    pin_sw2 = uicc_pin_resp->sub_block_t.uicc_pin_change_resp.pin_uicc_sb_status_word.sw2;
                    log_f(2,"PIN service type = MAL_UICC_PIN_CHANGE, sw1=%x, sw2=%x\n",pin_sw1,pin_sw2);
                    break;
                }
                case MAL_UICC_PIN_INFO:
                {
                    log_f(2,"uicc_pin_info_resp.pin_uicc_sb_pin_info.pin_status = %x\n",
                            uicc_pin_resp->sub_block_t.uicc_pin_info_resp.pin_uicc_sb_pin_info.pin_status);
                    log_f(2,"uicc_pin_info_resp.pin_uicc_sb_pin_info.pin_att = %x\n",
                            uicc_pin_resp->sub_block_t.uicc_pin_info_resp.pin_uicc_sb_pin_info.pin_att);
                    log_f(2,"uicc_pin_info_resp.pin_uicc_sb_pin_info.puk_att = %x\n",
                            uicc_pin_resp->sub_block_t.uicc_pin_info_resp.pin_uicc_sb_pin_info.puk_att);
                    break;
                }
                case MAL_UICC_PIN_UNBLOCK:
                {
                    pin_sw1 = uicc_pin_resp->sub_block_t.uicc_pin_unblock_resp.pin_uicc_sb_status_word.sw1;
                    pin_sw2 = uicc_pin_resp->sub_block_t.uicc_pin_unblock_resp.pin_uicc_sb_status_word.sw2;
                    log_f(2,"PIN service type = MAL_UICC_PIN_UNBLOCK, sw1=%x, sw2=%x\n",pin_sw1,pin_sw2);
                    break;
                }
                default:
                {
                    log_f(2,"PIN service type = UNKNOWN\n");
                    break;
                }
            }
        }
    break;

    case MAL_UICC_CAT_IND:
        log_f(0,"sim_mal_cbdh MAL_UICC_CAT_IND\n");
        uicc_cat_ind = (uicc_cat_ind_t*)(data);
        log_f(1,"uicc_cat_ind->service_type = %s\n", sim_trace_lookup_service_type(uicc_cat_ind->service_type));
        log_f(1,"uicc_cat_ind->card_type = %s\n", sim_trace_lookup_card_type(uicc_cat_ind->card_type));
        log_f(1,"uicc_cat_ind->n_sb = %x\n", uicc_cat_ind->n_sb);
        if(uicc_cat_ind->n_sb) {
            uicc_sb_apdu_t  *sb = &uicc_cat_ind->sub_block.uicc_cat_fetched_cmd.uicc_sb_apdu;
            log_f(2,"cmd_force=%x, data length = %d\n",sb->cmd_force, sb->apdu_length );
            log_f(2,"printing data ...\n");
            sim_trace_print_data(sb->apdu, sb->apdu_length,log_f);
        }
    break;

    case MAL_UICC_CAT_RESP:
        log_f(0,"sim_mal_cbdh MAL_UICC_CAT_RESP\n");
        uicc_cat_resp = (uicc_cat_resp_t*)(data);
        log_f(1,"uicc_cat_resp->service_type = %s\n", sim_trace_lookup_service_type(uicc_cat_resp->service_type));
        log_f(1,"uicc_cat_resp->status = %x, details = %x\n", uicc_cat_resp->status, uicc_cat_resp->details);
        log_f(1,"uicc_cat_resp->card_type = %s\n", sim_trace_lookup_card_type(uicc_cat_resp->card_type));
        log_f(1,"uicc_cat_resp->n_sb = %x\n", uicc_cat_resp->n_sb);
        switch ( uicc_cat_resp->service_type ) {
        case MAL_UICC_CAT_TERMINAL_RESPONSE: {
            uicc_cat_terminal_response_resp_t   *sb = &uicc_cat_resp->sub_block.uicc_cat_terminal_response_resp;
            log_f(2,"APDU Force Flag=%x\n",sb->uicc_sb_apdu.cmd_force);
            sim_trace_print_data(sb->uicc_sb_apdu.apdu, sb->uicc_sb_apdu.apdu_length,log_f);
        }
        break;
        case MAL_UICC_CAT_ENVELOPE: {
            uicc_cat_envelope_resp_t    *sb = &uicc_cat_resp->sub_block.uicc_cat_envelope_resp;
            log_f(2,"APDU Force Flag=%x\n",sb->uicc_sb_apdu.cmd_force);
            sim_trace_print_data(sb->uicc_sb_apdu.apdu, sb->uicc_sb_apdu.apdu_length,log_f);
        }
        break;
        case MAL_UICC_CAT_POLL: {
            uicc_cat_poll_resp_t    *sb = &uicc_cat_resp->sub_block.uicc_cat_poll_resp;
            log_f(2,"APDU Force Flag=%x\n",sb->uicc_sb_apdu.cmd_force);
            sim_trace_print_data(sb->uicc_sb_apdu.apdu, sb->uicc_sb_apdu.apdu_length,log_f);
        }
        break;
        case MAL_UICC_CAT_REFRESH: {
            uicc_cat_refresh_resp_t *sb = &uicc_cat_resp->sub_block.uicc_cat_refresh_resp;
            log_f(2,"sb.nb_refresh_result_instances = %d\n", sb->nb_refresh_result_instances);
            log_f(2,"sb.refresh_type = %s\n", sim_trace_lookup_refresh_type(sb->uicc_sb_refresh.type));
            log_f(2,"Additional Data (first block only)\n");
            if (sb->nb_refresh_result_instances > 0 && sb->uicc_sb_refresh_result[0]) {
                log_f(2,"sb.refresh_result[0].refresh_status = %x\n", sb->uicc_sb_refresh_result[0]->refresh_status);
                sim_trace_print_data(sb->uicc_sb_refresh_result[0]->additional_info,
                                     sb->uicc_sb_refresh_result[0]->additional_info_length,log_f);
            }
        }
        break;
        }
    break;

    case MAL_UICC_APDU_RESP:
        log_f(0,"sim_mal_cbdh MAL_UICC_APDU_RESP\n");
        uicc_apdu_resp = (uicc_apdu_resp_t*)(data);
        log_f(1,"uicc_apdu_resp->service_type = %s\n", sim_trace_lookup_service_type(uicc_apdu_resp->service_type));
        log_f(1,"uicc_apdu_resp->status = %x, details = %x\n", uicc_apdu_resp->status, uicc_apdu_resp->details);
        log_f(1,"uicc_apdu_resp->n_sb = %x\n", uicc_apdu_resp->n_sb);
        if ( uicc_apdu_resp->n_sb > 0 ) {
            switch ( uicc_apdu_resp->service_type ) {
                case MAL_UICC_APDU_SEND:
                {
                    uicc_apdu_send_resp_t     *p = &uicc_apdu_resp->sub_block.uicc_apdu_send_resp;
                    log_f(2,"APDU: Force=%d, Len=%d", p->uicc_sb_apdu.cmd_force, p->uicc_sb_apdu.apdu_length);
                    sim_trace_print_data(p->uicc_sb_apdu.apdu,p->uicc_sb_apdu.apdu_length,log_f);
                }
                break;
                case MAL_UICC_ATR_GET:
                {
                    uicc_apdu_atr_get_resp_t  *p = &uicc_apdu_resp->sub_block.uicc_apdu_atr_get_resp;
                    log_f(2,"ATR: Force=%d, Len=%d", p->uicc_sb_apdu.cmd_force, p->uicc_sb_apdu.apdu_length);
                    sim_trace_print_data(p->uicc_sb_apdu.apdu,p->uicc_sb_apdu.apdu_length,log_f);
                }
                break;
                default:
                {
                    log_f(2,"APDU_RESP service type = UNKNOWN\n");
                    break;
                }
            }
        }
    break;
    default:
        log_f(0,"sim_mal_cbdh unknown message id=0x%x\n",message_id);
    break;
    }
}
