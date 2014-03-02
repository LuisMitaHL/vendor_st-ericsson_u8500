/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : tuicc.c
 * Description     : test executable .
 *
 * Author          : Annwesh Mukherjee <annwesh.xa.mukherjee@stericsson.com>
 *
 */


#include "sim.h"
#include "sim_unused.h"
#include "sim_internal.h"

#include "uicc_internal.h"
#include "cat_barrier.h"
#include "func_trace.h"
#include "tuicc.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <pthread.h>
#include "sim_log.h"

#define TTEST( FUNC, CT, C, B, T )              \
do {                                            \
    int test_i;                                 \
    cat_barrier_set(B);                         \
    test_i = FUNC(C, CT);                       \
    if ( test_i ) {                             \
        ste_sim_delete(C, CT);                  \
        cat_barrier_delete(B);                  \
        return #FUNC " failed.";                \
    }                                           \
    test_i = my_barrier_timedwait(B, T);        \
    if ( test_i ) {                             \
        cat_barrier_delete(B);                  \
        ste_sim_delete(C, CT);                  \
        return #FUNC " timed out";              \
    }                                           \
} while(0)

static const char *ste_sim_cause_name[STE_UICC_CAUSE__MAX] = {
    "SIM NOOP",
    "SIM CONNECT",
    "SIM DISCONNECT",
    "SIM SHUTDOWN",
    "SIM PING",
    "SIM HANGUP",
    "SIM STARTUP",
    "STE SIM LOG LEVEL",
    "STE MODEM LOG LEVEL",
    "CAT NOOP",
    "CAT REGISTER",
    "CAT PC",
    "CAT SIM EC CALL CONTROL",
    "CAT IND CAT STATUS",
    "CAT EC",
    "CAT TR",
    "CAT SET TERMINAL PROFILE",
    "CAT GET TERMINAL PROFILE",
    "CAT GET CAT STATUS",
    "CAT PC NOTIFICATION",
    "CAT DEREGISTER",
    "CAT PC SETUP CALL IND",
    "CAT ANSWER CALL",
    "CAT EVENT DOWNLOAD",
    "CAT SIM EC SMS CONTROL",
    "CAT SETUP CALL RESULT",
    "CAT PC REFRESH IND",
    "CAT PC REFRESH FILE IND",
    "CAT SESSION END",
    "CAT ENABLE",
    "CAT INFO",
    "CAT RAT SETTING UPDATED",
    "UICC NOOP",
    "UICC REGISTER",
    "UICC REQ PIN VERIFY",
    "UICC REQ PIN NEEDED",
    "UICC REQ READ SIM FILE RECORD",
    "UICC REQ UPDATE SIM FILE RECORD",
    "UICC REQ PIN CHANGE",
    "UICC REQ READ SIM FILE BINARY",
    "UICC REQ UPDATE SIM FILE BINARY",
    "UICC REQ SIM FILE GET FORMAT",
    "UICC REQ GET FILE INFORMATION",
    "UICC REQ GET SIM STATE",
    "UICC SIM STATE CHANGED",
    "UICC REQ PIN DISABLE",
    "UICC REQ PIN ENABLE",
    "UICC REQ PIN INFO",
    "UICC REQ PIN UNBLOCK",
    "UICC NOT READY",
    "UICC REQ GET APP INFO",
    "UICC REQ READ SMSC",
    "UICC REQ UPDATE SMSC",
    "UICC REQ SIM FILE READ GENERIC",
    "UICC REQ SIM FILE READ IMSI",
    "UICC REQ UPDATE SERVICE TABLE",
    "UICC REQ GET SERVICE TABLE",
    "UICC SIM STATUS",
    "UICC REQ SIM ICON READ",
    "UICC REQ READ SUBSCRIBER NUMBER",
    "UICC REQ SIM FILE UPDATE PLMN",
    "UICC REQ SIM FILE READ PLMN",
    "UICC REQ CARD STATUS",
    "UICC REQ APP STATUS",
    "UICC REQ APPL APDU SEND",
    "UICC REQ SMSC GET ACTIVE",
    "UICC REQ SMSC SET ACTIVE",
    "UICC REQ SMSC GET RECORD MAX",
    "UICC REQ SMSC SAVE TO RECORD",
    "UICC REQ SMSC RESTORE FROM RECORD",
    "UICC REQ SIM CHANNEL SEND",
    "UICC REQ SIM CHANNEL OPEN",
    "UICC REQ SIM CHANNEL CLOSE",
    "UICC REQ GET SERVICE AVAILABILITY",
    "UICC REQ_SIM FILE READ FDN",
    "UICC REQ SIM FILE READ ECC",
    "UICC REQ SIM RESET",
    "UICC REQ SIM POWER ON",
    "UICC REQ SIM POWER OFF",
    "UICC REQ SIM READ PREFERRED RAT SETTING"
};

typedef struct {
    int                     cause;
    void                   *data;
} sync_data_t;

// This is for holding the details for which SIM file to read
typedef struct {
    int     file_id;
    char    file_path[21];
    char    *pathptr;
    union {
      int offset;
      int rec_id;
      int record_or_offset; // For icon read, this takes any value.
    } v;
    int     length;
} filerec_t;

int inter = 0; // Interactive flag.

static uint8_t simFileBuffer[256];

int uicc_sim = 0; // Set to non-zero for sim, otherwise usim is assumed

uintptr_t passive_client_tag = 0;
int passive_mode = 0;
int hangup = 0;

const char * const file_types[] =
  {"UNKNOWN", "NO_INFO", "DF", "MF", "TRANSPARENT", "LINEAR_FIXED", "CYCLIC", "BER_TLV"};

const uint32_t              reg_events = STE_CAT_CLIENT_REG_EVENTS_CAT_INFO;

#ifndef HAVE_ANDROID_OS
uicc_request_status_t ste_uicc_set_app_type_to_sim(ste_sim_t * uicc,
                                                         uintptr_t client_tag)
{
  int rv = UICC_REQUEST_STATUS_OK;
  if (!uicc) {
    return UICC_REQUEST_STATUS_FAILED_PARAMETER;
  }

  if (uicc->state != ste_sim_state_connected) {
    return UICC_REQUEST_STATUS_FAILED_STATE;
  }

  rv = sim_send_generic(uicc->fd,
                        STE_UICC_REQ_SIM_SET_APP_TYPE_TO_SIM,
                        NULL,
                        0,
                        client_tag);
  if (rv < 0) {
    rv = ste_sim_disconnect(uicc, client_tag);
  }
  return rv;
}
#endif
sync_data_t            *sync_data_create(int cause, const void *data)
{
  // In this function, data placed on the stack needs to be copied to the heap
  // or the data will be popped from the stack when the barrier is released.
    sync_data_t            *p = malloc(sizeof(sync_data_t));
    if (p) {
        p->cause = cause;
        p->data = 0;

        switch ( cause ) {
        case STE_UICC_CAUSE_NOOP:
        {
            //printf("sync_data_create: STE_UICC_CAUSE_NOOP has no data\n");
        }
        break;
        case STE_UICC_CAUSE_REGISTER:
        {
            //printf("sync_data_create: STE_UICC_CAUSE_REGISTER has no data\n");
        }
        break;
        case STE_UICC_CAUSE_REQ_PIN_VERIFY:
        {
            ste_uicc_pin_verify_response_t* d;
            d = malloc( sizeof(*d) );
            if ( d != NULL ) {
                memcpy( d, data, sizeof(*d) );
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_PIN_NEEDED:
        {
            //printf("sync_data_create: STE_UICC_CAUSE_REQ_PIN_NEEDED has no data\n");
        }
        break;
        case STE_UICC_CAUSE_REQ_PIN_UNBLOCK:
        {
            ste_uicc_pin_unblock_response_t* d;
            d = malloc( sizeof(*d) );
            if ( d != NULL ) {
                memcpy( d, data, sizeof(*d) );
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_PIN_ENABLE:
        {
            ste_uicc_pin_enable_response_t* d;
            d = malloc( sizeof(*d) );
            if ( d != NULL ) {
                memcpy( d, data, sizeof(*d) );
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_PIN_DISABLE:
        {
            ste_uicc_pin_disable_response_t* d;
            d = malloc( sizeof(*d) );
            if ( d != NULL ) {
                memcpy( d, data, sizeof(*d) );
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_READ_SIM_FILE_RECORD:
        {
            ste_uicc_sim_file_read_record_response_t* d;
            d = malloc( sizeof(*d) );
            if ( d != NULL ) {
                memcpy( d, data, sizeof(*d) );

                d->data = malloc(d->length);
                if ( d->data != NULL ) {
                    memcpy(d->data,
                        ((ste_uicc_sim_file_read_record_response_t *) data)->data,
                        d->length);
                }
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_PIN_INFO:
        {
            ste_uicc_pin_info_response_t* d;
            d = malloc( sizeof(*d) );
            if ( d != NULL ) {
                memcpy( d, data, sizeof(*d) );
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_UPDATE_SIM_FILE_RECORD:
        {
            ste_uicc_update_sim_file_record_response_t* d;
            d = malloc( sizeof(*d) );
            if ( d != NULL ) {
                memcpy( d, data, sizeof(*d) );
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_PIN_CHANGE:
        {
            ste_uicc_pin_change_response_t* d;
            d = malloc( sizeof(*d) );
            if ( d != NULL ) {
                memcpy( d, data, sizeof(*d) );
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_READ_SIM_FILE_BINARY:
        {
            ste_uicc_sim_file_read_binary_response_t* d;
            d = malloc( sizeof(*d) );
            if ( d != NULL ) {
                memcpy( d, data, sizeof(*d) );

                d->data = malloc(d->length);
                if ( d->data != NULL ) {
                    memcpy(d->data,
                        ((ste_uicc_sim_file_read_binary_response_t *)data)->data,
                        d->length);
                }
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_UPDATE_SIM_FILE_BINARY:
        {
            ste_uicc_update_sim_file_binary_response_t* d;
            d = malloc( sizeof(*d) );
            if ( d != NULL ) {
                memcpy( d, data, sizeof(*d) );
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_SIM_FILE_GET_FORMAT:
        {
            ste_uicc_sim_file_get_format_response_t* d;
            d = malloc( sizeof(*d) );
            if ( d != NULL ) {
                memcpy( d, data, sizeof(*d) );
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_GET_FILE_INFORMATION:
        {
            ste_uicc_get_file_information_response_t* d;
            d = malloc( sizeof(*d) );
            if ( d != NULL ) {
                memcpy( d, data, sizeof(*d) );
                d->fcp = malloc(d->length);
                if ( d->fcp != NULL ) {
                    memcpy(d->fcp,
                        ((ste_uicc_get_file_information_response_t*)data)->fcp,
                        d->length);
                }
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_SMSC_GET_ACTIVE:
        {
            ste_uicc_sim_smsc_get_active_response_t* d;
            uint8_t * text_p;
            unsigned  ch_nr;

            d = malloc( sizeof(*d) );
            if ( d != NULL ) {
                memcpy( d, data, sizeof(*d) );

                text_p = (uint8_t*)(((ste_uicc_sim_smsc_get_active_response_t *) data)->smsc.num_text.text_p);
                ch_nr = ((ste_uicc_sim_smsc_get_active_response_t *) data)->smsc.num_text.no_of_characters;  //ASCII format is returned
                if (ch_nr > 0) {
                    if (text_p == NULL) {
                        abort();
                    }
                    d->smsc.num_text.text_p = malloc(ch_nr + 1);
                    if ( d->smsc.num_text.text_p != NULL ) {
                        memset(d->smsc.num_text.text_p, 0, ch_nr + 1);
                        memcpy(d->smsc.num_text.text_p, text_p, ch_nr);
                    }
                }
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_SMSC_SET_ACTIVE:
        {
            ste_uicc_sim_smsc_set_active_response_t* d;
            d = malloc( sizeof(*d) );
            memcpy( d, data, sizeof(*d) );
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_SMSC_SAVE_TO_RECORD:
        {
            ste_uicc_sim_smsc_save_to_record_response_t* d;
            d = malloc( sizeof(*d) );
            memcpy( d, data, sizeof(*d) );
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_SMSC_RESTORE_FROM_RECORD:
        {
            ste_uicc_sim_smsc_restore_from_record_response_t* d;
            d = malloc( sizeof(*d) );
            memcpy( d, data, sizeof(*d) );
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_SMSC_GET_RECORD_MAX:
        {
            ste_uicc_sim_smsc_get_record_max_response_t* d;
            d = malloc( sizeof(*d) );
            if ( d != NULL ) {
                memcpy( d, data, sizeof(*d) );
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_SIM_FILE_READ_IMSI:
        {
            if (data) {
                ste_uicc_sim_file_read_imsi_response_t* d;
                d = malloc( sizeof(*d) );
                if ( d != NULL ) {
                    memcpy( d, data, sizeof(*d) );
                }
                p->data = d;
            }
            else {
                p->data = NULL;
            }
        }
        break;
        case STE_UICC_CAUSE_REQ_GET_SIM_STATE:
        {
            ste_uicc_get_sim_state_response_t* d;
            d = malloc( sizeof(*d) );
            if ( d != NULL ) {
                memcpy( d, data, sizeof(*d) );
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_SIM_STATE_CHANGED:
        {
            ste_uicc_sim_state_changed_t* d;
            d = malloc( sizeof(*d) );
            if ( d != NULL ) {
                memcpy( d, data, sizeof(*d) );
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_GET_APP_INFO:
        {
            ste_uicc_sim_app_info_response_t* d;
            d = malloc( sizeof(*d) );
            if ( d != NULL ) {
                memcpy( d, data, sizeof(*d) );
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_NOT_READY:
        {
            ste_uicc_not_ready_t    *d;
            d = malloc( sizeof(*d) );
            if ( d != NULL ) {
                memcpy( d, data, sizeof(*d) );
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_UPDATE_SERVICE_TABLE:
        {
            ste_uicc_update_service_table_response_t* d;
            d = malloc( sizeof(*d) );
            if ( d != NULL ) {
                memcpy( d, data, sizeof(*d) );
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_GET_SERVICE_TABLE:
        {
            ste_uicc_get_service_table_response_t* d;
            d = malloc( sizeof(*d) );
            if ( d != NULL ) {
                memcpy( d, data, sizeof(*d) );
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY:
        {
            ste_uicc_get_service_availability_response_t* d;
            d = malloc( sizeof(*d) );
            if ( d != NULL ) {
                memcpy( d, data, sizeof(*d) );
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_SIM_STATUS:
        {
            sim_reason_t *d;
            d = malloc(sizeof(*d)); // "box" the enum number
            if ( d != NULL ) {
                if (d) memcpy( d, data, sizeof(*d));
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_SIM_ICON_READ:
        {
            ste_uicc_sim_icon_read_response_t *d;
            d = malloc( sizeof(*d) );
            if ( d != NULL ) {
                memcpy( d, data, sizeof(*d) );

                d->data = malloc(d->length);
                if ( d->data != NULL ) {
                    memcpy(d->data,
                        ((ste_uicc_sim_icon_read_response_t *)data)->data,
                        d->length);
                }
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_READ_SUBSCRIBER_NUMBER: {
            ste_sim_subscriber_number_response_t *d = NULL;
            ste_sim_subscriber_number_record_t *dst_rec_p = NULL;
            ste_sim_subscriber_number_record_t *src_rec_p = NULL;
            uint8_t i = 0;

            d = malloc(sizeof(*d));

            if(NULL == d){
                break;
            }

            /* Copy the content of the subscriber no response. */
            memcpy(d, data, sizeof(*d));

            d->record_data_p = NULL;
            d->record_data_p = malloc(sizeof(ste_sim_subscriber_number_record_t) * d->number_of_records);

            if(NULL == d->record_data_p){
                free(d);
                break;
            }

            dst_rec_p = d->record_data_p;
            for (i = 0; i < d->number_of_records; i++) {
                dst_rec_p->alpha_p = NULL;
                dst_rec_p->number_p = NULL;
                dst_rec_p++;
            }

            dst_rec_p = d->record_data_p;
            src_rec_p = ((ste_sim_subscriber_number_response_t *)data)->record_data_p;

            for (i = 0; i < d->number_of_records; i++) {
                dst_rec_p->alpha_id_actual_len = src_rec_p->alpha_id_actual_len;
                dst_rec_p->number_actual_len = src_rec_p->number_actual_len;
                dst_rec_p->type = src_rec_p->type;
                dst_rec_p->speed = src_rec_p->speed;
                dst_rec_p->service = src_rec_p->service;
                dst_rec_p->itc = src_rec_p->itc;

                dst_rec_p->alpha_p = malloc(sizeof(uint8_t) * dst_rec_p->alpha_id_actual_len);

                if(NULL == dst_rec_p->alpha_p){
                    i = d->number_of_records;
                    break;
                }

                memcpy(dst_rec_p->alpha_p, src_rec_p->alpha_p, dst_rec_p->alpha_id_actual_len);

                dst_rec_p->number_p = malloc(sizeof(uint8_t) * dst_rec_p->number_actual_len);

                if(NULL == dst_rec_p->number_p){
                    i = d->number_of_records;
                    break;
                }

                memcpy(dst_rec_p->number_p, src_rec_p->number_p, dst_rec_p->number_actual_len);

                dst_rec_p++;
                src_rec_p++;
            }

            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_SIM_FILE_READ_PLMN:
        {
            ste_uicc_sim_file_read_plmn_response_t* d;
            d = malloc( sizeof(*d) );
            if ( d != NULL ) {
                memcpy( d, data, sizeof(*d) );

                d->ste_sim_plmn_with_AcT_p = malloc(sizeof(ste_sim_plmn_with_AcT_t) * d->nr_of_plmn);
                if (d->ste_sim_plmn_with_AcT_p != NULL) {
                    memcpy(d->ste_sim_plmn_with_AcT_p,
                        ((ste_uicc_sim_file_read_plmn_response_t *)data)->ste_sim_plmn_with_AcT_p,
                        (sizeof(ste_sim_plmn_with_AcT_t) * d->nr_of_plmn));
                }
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_SIM_FILE_UPDATE_PLMN:
        {
            ste_uicc_update_sim_file_plmn_response_t* d;
            d = malloc(sizeof(*d));
            if ( d != NULL ) {
                memcpy(d, data, sizeof(*d));
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_CARD_STATUS:
        {
            ste_uicc_sim_card_status_response_t *d;
            d = malloc(sizeof *d);
            if ( d != NULL ) {
                memcpy(d, data, sizeof *d);
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_APP_STATUS:
        {
            ste_uicc_sim_app_status_response_t *d;
            d = malloc(sizeof *d);
            if ( d != NULL ) {
                memcpy(d, data, sizeof(*d));
                if (d->status == UICC_REQUEST_STATUS_OK && d->label) {
                    d->label = malloc(d->label_len+1);
                    if ( d->label ) {
                        memcpy(d->label,
                            ((ste_uicc_sim_app_status_response_t *)data)->label,
                            d->label_len+1);
                    }
                }
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_SIM_CHANNEL_SEND:
        {
            ste_uicc_sim_channel_send_response_t *d;
            d = malloc(sizeof *d);
            if ( d != NULL ) {
                memcpy(d, data, sizeof(*d));
                if (d->uicc_status_code == UICC_REQUEST_STATUS_OK && d->apdu) {
                    d->apdu = malloc(d->apdu_len);
                    memcpy(d->apdu,
                        ((ste_uicc_sim_channel_send_response_t *)data)->apdu,
                        d->apdu_len);
                }
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_SIM_CHANNEL_OPEN:
        {
            ste_uicc_sim_channel_open_response_t *d;
            d = malloc(sizeof *d);
            if ( d != NULL ) {
                memcpy(d, data, sizeof(*d));
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_SIM_CHANNEL_CLOSE:
        {
            ste_uicc_sim_channel_close_response_t *d;
            d = malloc(sizeof *d);
            if ( d != NULL ) {
                memcpy(d, data, sizeof(*d));
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_SIM_FILE_READ_FDN:
        {
          ste_uicc_sim_fdn_response_t *d;
          d = malloc(sizeof(*d));
          if (d) {
            ste_uicc_sim_fdn_record_t *f, *df;
            size_t f_len;

            memcpy(d, data, sizeof(*d));
            f_len = d->number_of_records;
            f = malloc(sizeof(*f)*f_len);
            df = d->fdn_record_p;
            d->fdn_record_p = f;
            if (f) {
              int i;

              memcpy(f, df, sizeof(*f)*f_len);
              for (i=0;i<(int)f_len;i++) {
                size_t dc_len = (size_t)df[i].dial_string.no_of_characters;
                uint8_t *c, *dc;
                size_t da_len = (size_t)df[i].alpha_string.no_of_characters;
                uint8_t *a, *da;

                if (dc_len == 0) continue;
                dc = df[i].dial_string.text_p;
                c = malloc(dc_len);
                f[i].dial_string.text_p = c;
                if (!dc) continue;
                memcpy(c, dc, dc_len);

                if (da_len == 0) continue;
                da = df[i].alpha_string.text_p;
                a = malloc(da_len);
                f[i].alpha_string.text_p = a;
                if (!da) continue;
                memcpy(a, da, da_len);
              }
            }
          }
          p->data = d;
       }
        break;

        case STE_UICC_CAUSE_REQ_SIM_FILE_READ_ECC:
        {
          ste_uicc_sim_ecc_response_t *d;
          d = malloc(sizeof(*d));
          if (d) {
            ste_uicc_sim_ecc_number_t *e, *de;
            size_t e_len;

            memcpy(d, data, sizeof(*d));
            e_len = d->number_of_records;
            e = malloc(sizeof(*e)*e_len);
            de = d->ecc_number_p;
            d->ecc_number_p = e;
            if (e) {
              int i;

              memcpy(e, de, sizeof(*e)*e_len);
              for (i=0;i<(int)e_len;i++) {
                size_t da_len = de[i].length;
                uint8_t *a, *da;
                if (da_len == 0) continue;
                da = de[i].alpha;
                a = malloc(da_len);
                e[i].alpha = a;
                if (!da) continue;
                memcpy(a, da, da_len);
              }
            }
          }
          printf("sync data create: STE_UICC_CAUSE_REQ_SIM_FILE_READ_ECC");
          p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_SIM_RESET:
        {
            ste_uicc_sim_reset_response_t *d;
            d = malloc(sizeof *d);
            if ( d != NULL ) {
                memcpy(d, data, sizeof(*d));
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_SIM_POWER_ON:
        {
            ste_uicc_sim_power_on_response_t *d;
            d = malloc(sizeof *d);
            if ( d != NULL ) {
                memcpy(d, data, sizeof(*d));
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_SIM_POWER_OFF:
        {
            ste_uicc_sim_power_off_response_t *d;
            d = malloc(sizeof *d);
            if ( d != NULL ) {
                memcpy(d, data, sizeof(*d));
            }
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_REQ_SIM_READ_PREFERRED_RAT_SETTING:
        {
            ste_uicc_sim_read_preferred_RAT_setting_response_t *d;
            d = malloc(sizeof *d);
            if ( d != NULL ) {
                memcpy(d, data, sizeof(*d));
            }
            p->data = d;
        }
        break;
        default:
        {
            if ( cause >= STE_UICC_CAUSE__MAX ) {
                printf("sync_data_create: Unexpected cause=%d\n",cause);
            }
        }
        break;
        }
    }

    return p;
}

void sync_data_delete(sync_data_t * p)
{
    if (p && p->data) {
        free( p->data );
    }
    free(p);

}

uintptr_t get_ct_from_cause(int cause)
{
     switch(cause) {
           case STE_SIM_CAUSE_NOOP:
                return TUICC_CT_STE_SIM_CAUSE_NOOP;
           case STE_SIM_CAUSE_CONNECT:
                return TUICC_CT_STE_SIM_CAUSE_CONNECT;
           case STE_SIM_CAUSE_DISCONNECT:
                return TUICC_CT_STE_SIM_CAUSE_DISCONNECT;
           case STE_SIM_CAUSE_SHUTDOWN:
                return TUICC_CT_STE_SIM_CAUSE_SHUTDOWN;
           case STE_SIM_CAUSE_PING:
                return TUICC_CT_STE_SIM_CAUSE_PING;
           case STE_SIM_CAUSE_SIM_LOG_LEVEL:
                return TUICC_CT_STE_SIM_SET_SIM_DEBUG_LEVEL;
           case STE_SIM_CAUSE_MODEM_LOG_LEVEL:
                return TUICC_CT_STE_SIM_SET_MODEM_DEBUG_LEVEL;
           case STE_SIM_CAUSE_HANGUP:
                return TUICC_CT_STE_SIM_CAUSE_HANGUP;
           case STE_UICC_CAUSE_NOOP:
                return TUICC_CT_STE_UICC_CAUSE_NOOP;
           case STE_UICC_CAUSE_REGISTER:
                return TUICC_CT_STE_UICC_CAUSE_REGISTER;
           case STE_UICC_CAUSE_REQ_PIN_VERIFY:
                return TUICC_CT_STE_UICC_CAUSE_REQ_PIN_VERIFY;
           case STE_UICC_CAUSE_REQ_PIN_NEEDED:
                return TUICC_CT_STE_UICC_CAUSE_REQ_PIN_NEEDED;
           case STE_UICC_CAUSE_REQ_READ_SIM_FILE_RECORD:
                return TUICC_CT_STE_UICC_CAUSE_REQ_READ_SIM_FILE_RECORD;
           case STE_UICC_CAUSE_REQ_UPDATE_SIM_FILE_RECORD:
                return TUICC_CT_STE_UICC_CAUSE_REQ_UPDATE_SIM_FILE_RECORD;
           case STE_UICC_CAUSE_REQ_PIN_CHANGE:
                return TUICC_CT_STE_UICC_CAUSE_REQ_PIN_CHANGE;
           case STE_UICC_CAUSE_REQ_READ_SIM_FILE_BINARY:
                return TUICC_CT_STE_UICC_CAUSE_REQ_READ_SIM_FILE_BINARY;
           case STE_UICC_CAUSE_REQ_UPDATE_SIM_FILE_BINARY:
                return TUICC_CT_STE_UICC_CAUSE_REQ_UPDATE_SIM_FILE_BINARY;
           case STE_UICC_CAUSE_REQ_SIM_FILE_GET_FORMAT:
                return TUICC_CT_STE_UICC_CAUSE_REQ_SIM_FILE_GET_FORMAT;
           case STE_UICC_CAUSE_REQ_GET_FILE_INFORMATION:
                return TUICC_CT_STE_UICC_CAUSE_REQ_GET_FILE_INFORMATION;
           case STE_UICC_CAUSE_REQ_GET_SIM_STATE:
                return TUICC_CT_STE_UICC_CAUSE_REQ_GET_SIM_STATE;
           case STE_UICC_CAUSE_SIM_STATE_CHANGED:
                return TUICC_CT_STE_UICC_CAUSE_SIM_STATE_CHANGED;
           case STE_UICC_CAUSE_REQ_PIN_DISABLE:
                return TUICC_CT_STE_UICC_CAUSE_REQ_PIN_DISABLE;
           case STE_UICC_CAUSE_REQ_PIN_ENABLE:
                return TUICC_CT_STE_UICC_CAUSE_REQ_PIN_ENABLE;
           case STE_UICC_CAUSE_REQ_PIN_INFO:
                return TUICC_CT_STE_UICC_CAUSE_REQ_PIN_INFO;
           case STE_UICC_CAUSE_REQ_PIN_UNBLOCK:
                return TUICC_CT_STE_UICC_CAUSE_REQ_PIN_UNBLOCK;
           case STE_UICC_CAUSE_REQ_SMSC_GET_ACTIVE:
                return TUICC_CT_STE_UICC_CAUSE_REQ_SMSC_GET_ACTIVE;
           case STE_UICC_CAUSE_REQ_SMSC_SET_ACTIVE:
                return TUICC_CT_STE_UICC_CAUSE_REQ_SMSC_SET_ACTIVE;
           case STE_UICC_CAUSE_REQ_SIM_FILE_READ_IMSI:
                return TUICC_CT_STE_UICC_CAUSE_REQ_SIM_FILE_READ_IMSI;
           case STE_UICC_CAUSE_NOT_READY:
               return TUICC_CT_STE_UICC_CAUSE_NOT_READY;
           case STE_UICC_CAUSE_REQ_UPDATE_SERVICE_TABLE:
                return TUICC_CT_STE_UICC_CAUSE_REQ_UPDATE_SERVICE_TABLE;
           case STE_UICC_CAUSE_REQ_GET_SERVICE_TABLE:
                return TUICC_CT_STE_UICC_CAUSE_REQ_GET_SERVICE_TABLE;
           case STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY:
                return TUICC_CT_STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY;
           case STE_UICC_CAUSE_SIM_STATUS:
                return TUICC_CT_STE_UICC_CAUSE_SIM_STATUS;
           case STE_UICC_CAUSE_REQ_SIM_ICON_READ:
                return TUICC_CT_STE_UICC_CAUSE_REQ_SIM_ICON;
           case STE_UICC_CAUSE_REQ_READ_SUBSCRIBER_NUMBER:
                return TUICC_CT_STE_UICC_CAUSE_REQ_SUBSCRIBER_NUMBER;
           case STE_UICC_CAUSE_REQ_SIM_FILE_READ_FDN:
                return TUICC_CT_STE_UICC_CAUSE_REQ_SIM_FILE_READ_FDN;
           case STE_UICC_CAUSE_REQ_SIM_FILE_READ_ECC:
                return TUICC_CT_STE_UICC_CAUSE_REQ_SIM_FILE_READ_ECC;
           default:
                return TUICC_CT_NOT_USED;

     }
}


void the_callback(int cause, uintptr_t client_tag, void *data, void *vuser_data)
{
    cat_barrier_t          *bar = (cat_barrier_t *) vuser_data;

    passive_client_tag = client_tag;

    if (cause == STE_SIM_CAUSE_HANGUP) {
        hangup = 1;

        if (passive_mode == 1) {
            sync_data_t            *sd = sync_data_create(cause, data);
            cat_barrier_release(bar, sd);
        }
    }
    /* If the cause is SIM_STATE_CHANGED, we should not release the barrier, rather it should only be released for response type events */
    else if ((! (cause == STE_SIM_CAUSE_DISCONNECT && hangup == 1)) &&
             cause != STE_UICC_CAUSE_SIM_STATE_CHANGED &&
             cause != STE_UICC_CAUSE_SIM_STATUS) {
        sync_data_t            *sd = sync_data_create(cause, data);
        cat_barrier_release(bar, sd);
    }

    /* If the cause is SIM_STATE_CHANGED, we check for the passive mode since the passive client is interested in any async events */
    if (STE_UICC_CAUSE_SIM_STATE_CHANGED == cause ||
        cause == STE_UICC_CAUSE_SIM_STATUS) {
        if (passive_mode == 1) {
            sync_data_t            *sd = sync_data_create(cause, data);
            cat_barrier_release(bar, sd);
        }
    }
    if (cause == STE_UICC_CAUSE_SIM_STATUS)
      printf("#\tSIM_STATUS: %d\n", *(sim_reason_t *)data);
    printf("#\treceived(async): %2d %08x %08x %p %p : %s\n",
           cause,
           (unsigned int)client_tag,
           (unsigned int)get_ct_from_cause(cause),
           (void*) data,
           (void*) vuser_data, ste_sim_cause_name[cause]);
}



void the_callback_sync( int cause, uintptr_t client_tag,
                        void *data, void *vuser_data )
{
    printf("#\treceived(sync): %2d %08x %08x %p %p : %s\n",
           cause,
           (unsigned int)client_tag,
           (unsigned int)get_ct_from_cause(cause),
           (void*) data,
           (void*) vuser_data, ste_sim_cause_name[cause]);
}

// Utility function to print data with tlvs
void print_tlvs(uint8_t *data, int data_len)
{
  int i = 0, j;
  while (i < data_len) {
    printf("T=%2x L=%d", data[i], data[i+1]);
    for (j = 0; j<data[i+1]; j++)
      printf(" %#04x'%c'", data[i+2+j], isgraph(data[i+2+j]) ? data[i+2+j] : ' ');
    i = i+2+j;
    printf("\n");
  }
  if (i != data_len )
    printf("TLV: Corrupt data: i=%d != i=%d data_len", i, data_len);
  printf("Done TLV\n");
}

#define MLEN    16

static void print_status(ste_uicc_status_code_t c) {
  const char *s;
  switch (c) {
  case STE_UICC_STATUS_CODE_OK:
    s = "OK";
    break;
  case STE_UICC_STATUS_CODE_FAIL:
    s = "FAIL";
    break;
  case STE_UICC_STATUS_CODE_UNKNOWN:
    s = "UNKNOWN";
    break;
  case STE_UICC_STATUS_CODE_NOT_READY:
    s = "NOT READY";
    break;
  case STE_UICC_STATUS_CODE_SHUTTING_DOWN:
    s = "SHUTTING DOWN";
    break;
  case STE_UICC_STATUC_CODE_CARD_READY:
    s = "CARD READY";
    break;
  case STE_UICC_STATUS_CODE_CARD_NOT_READY:
    s = "CARD NOT READY";
    break;
  case STE_UICC_STATUS_CODE_CARD_DISCONNECTED:
    s = "CARD_DISCONNECTED";
    break;
  case STE_UICC_STATUS_CODE_CARD_NOT_PRESENT:
    s = "CARD_NOT_PRESENT";
    break;
  case STE_UICC_STATUS_CODE_CARD_REJECTED:
    s = "CARD_REJECTED";
    break;
  case STE_UICC_STATUS_CODE_APPL_ACTIVE:
    s = "APPL_ACTIVE";
    break;
  case STE_UICC_STATUS_CODE_APPL_ACTIVE_PIN:
    s = "APPL_ACTIVE_PIN";
    break;
  case STE_UICC_STATUS_CODE_APPL_ACTIVE_PUK:
    s = "APPL_ACTIVE_PUK";
    break;
  case STE_UICC_STATUS_CODE_APPL_NOT_ACTIVE:
    s = "APPL_NOT_ACTIVE";
    break;
  case STE_UICC_STATUS_CODE_PIN_ENABLED:
    s = "PIN_ENABLED";
    break;
  case STE_UICC_STATUS_CODE_PIN_ENABLED_NOT_VERIFIED:
    s = "PIN_ENABLED_NOT_VERIFIED";
    break;
  case STE_UICC_STATUS_CODE_PIN_ENABLED_VERIFIED:
    s = "PIN_ENABLED_VERIFIED";
    break;
  case STE_UICC_STATUS_CODE_PIN_ENABLED_BLOCKED:
    s = "PIN_ENABLED_BLOCKED";
    break;
  case STE_UICC_STATUS_CODE_PIN_ENABLED_PERM_BLOCKED:
    s = "PIN_ENABLED_PERM_BLOCKED";
    break;
  case STE_UICC_STATUS_CODE_PIN_DISABLED:
    s = "PIN_DISABLED";
    break;
  default:
    s = "unknown";
    break;
  }
  printf("%d %s", c, s);
}

static void print_data(uint8_t* buff, size_t length)
{
    char    hex_line[MLEN*3+6+1];
    char    asc_line[MLEN+1];
    size_t  i = 0, j = 0;

    for ( i = 0 ; i < length && buff != NULL ; i++ ) {
        if ( j == 0 ) sprintf(hex_line,"%04zx: ", i);
        sprintf( &hex_line[j*3+6], "%02x ", buff[i]);
        asc_line[j] = buff[i] >= ' ' && buff[i] <= '~' ? buff[i] : '.';
        asc_line[j+1] = '\0';
        if ( ++j == MLEN ) {
            printf("%s %s\n", hex_line, asc_line);
            j = 0;
        }
    }
    if ( j != 0 ) {
        printf("%s%*s %s\n", hex_line, (int)((MLEN-j)*3), "", asc_line);
    }
}

static void prompt ( const char *msg )
{
    fputs( msg, stdout );
    fflush( stdout );
}

static void editMemory ( uint8_t *mem, size_t len )
{
    printf("Enter offset then bytes or \"strings\"\n"
           "Eg. 0x20 \"hello\" 13 0x0a \"world\"\n"
           "Enter \'d\' to display current memory contents\n"
           "Enter blank line or EOF to end the edit\n" );
    do {
        char                line[BUFSIZ];
        unsigned long int   offset, byte;
        char                *p;
        if ( fgets( line, sizeof line, stdin ) == NULL ) break;
        if ( line[0] == '\n' ) break;
        if ( line[0] == 'd' ) {
            print_data( mem, len );
            continue;
        }
        offset = strtoul( line, &p, 0 );
        do {
            while ( isspace(*p) ) p++;
            if ( *p == '\0' ) break;    // reached the end of the line
            if ( *p == '\"' ) {
                p++;
                do {
                    if ( offset < len ) mem[offset++] = *p;
                    p++;
                } while ( *p != '\"' );
                p++;
            } else {
                byte = strtoul( p, &p, 0 );
                if ( offset < len ) mem[offset++] = (uint8_t)byte;
            }
        } while( offset < len );
    } while ( 1 );

    clearerr(stdin);    // in case EOF was used to end the input
}

typedef enum {
    FILE_REC_READ_FOR_BINARY,
    FILE_REC_READ_FOR_RECORD,
    FILE_REC_READ_FOR_ICON
} fileRecType_t;

static int readFileSpec ( filerec_t *rec, fileRecType_t type )
{
    int result = 0;
    char buff[100];
    prompt("Enter details for a SIM file and record\n"
           "EOF or invalid input will end the request\n");
    do {
        prompt("File ID (HEX int): ");
        if ( fgets(buff, sizeof buff, stdin) == NULL ) break;
        if ( sscanf(buff, "%x", (unsigned int *)&rec->file_id) != 1 ) break;

        if (type == FILE_REC_READ_FOR_BINARY ||
            type == FILE_REC_READ_FOR_RECORD) {
          prompt("File path (HEX string, or '0' for empty string, or return for NULL file path)\n: ");
          if ( fgets(buff, sizeof buff, stdin) == NULL ) break;

          { char *p = strchr(buff,'\n'); if ( p ) *p = '\0'; }    // remove the \n

          rec->pathptr = rec->file_path;              // not the exception case
          if ( buff[0] == '\0' ) {
            rec->pathptr = NULL;
          } else if ( buff[0] == '0' && buff[1] == '\n' ) {
            rec->file_path[0] = '\0';
          } else {
            // Should validate the string here
            strncpy(rec->file_path, buff, sizeof(rec->file_path));
            rec->file_path[sizeof(rec->file_path)-1] = '\0';
          }
        }
        if ( type == FILE_REC_READ_FOR_BINARY ) {
            prompt("Offset: ");
            if ( fgets(buff, sizeof buff, stdin) == NULL ) break;
            if ( sscanf( buff, "%i", &rec->v.offset ) != 1 ) break;
        } else if (type == FILE_REC_READ_FOR_RECORD) {
            prompt("Record id: ");
            if ( fgets(buff, sizeof buff, stdin) == NULL ) break;
            if ( sscanf( buff, "%i", &rec->v.rec_id ) != 1 ) break;
        } else if (type == FILE_REC_READ_FOR_ICON) {
            prompt("Record id or offset: ");
            if ( fgets(buff, sizeof buff, stdin) == NULL ) break;
            if ( sscanf( buff, "%i", &rec->v.record_or_offset) != 1 ) break;
        } else rec->v.offset = 0; // Whatever
        prompt("Length (0=All): ");
        if ( fgets(buff, sizeof buff, stdin) == NULL ) break;
        if ( sscanf( buff, "%i", &rec->length ) != 1 ) break;

        // if we got here, we're good to go
        result = 1;
    } while (0);

    clearerr(stdin);    // in case EOF was used to end the input
    return result;
}

int my_barrier_timedwait(cat_barrier_t * bar, unsigned ms)
{
    sync_data_t            *sd = 0;
    int                     i;
    i = cat_barrier_timedwait(bar, (void **) &sd, ms);
    if (sd)
        sync_data_delete(sd);
    return i;
}

// This is a test of connect disconnect only
const char             *test_connectdisconnect()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t       pc;
    int                     i;
    cat_barrier_t          *bar = cat_barrier_new();


    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";

    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    i = ste_sim_connect(uicc, TUICC_CT_STE_SIM_CAUSE_CONNECT);
    if (i) {
        ste_sim_delete(uicc, TUICC_CT_NOT_USED);
        cat_barrier_delete(bar);
        return "connect failed when connected.";
    }

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);

    i = ste_sim_disconnect(uicc, TUICC_CT_STE_SIM_CAUSE_DISCONNECT);
    if (!i) {
        ste_sim_delete(uicc, TUICC_CT_NOT_USED);
        cat_barrier_delete(bar);
        return "disconnect did not fail when disconnected.";
    }

    ste_sim_delete(uicc, TUICC_CT_NOT_USED);

    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}



const char             *test_ping()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t       pc;
    int                     i;
    cat_barrier_t          *bar = cat_barrier_new();


    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";

    i = ste_sim_ping(uicc, TUICC_CT_STE_SIM_CAUSE_PING);
    if (!i) {
        ste_sim_delete(uicc, TUICC_CT_NOT_USED);
        cat_barrier_delete(bar);
       return "ping did not fail when not connected.";
    }

    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    TTEST(ste_sim_ping, TUICC_CT_STE_SIM_CAUSE_PING, uicc, bar, 100);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);

    i = ste_sim_disconnect(uicc, TUICC_CT_STE_SIM_CAUSE_DISCONNECT);
    if (!i) {
        ste_sim_delete(uicc, TUICC_CT_NOT_USED);
        cat_barrier_delete(bar);
        return "disconnect did not fail when disconnected.";
    }

    ste_sim_delete(uicc, TUICC_CT_NOT_USED);

    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}




const char             *test_ping_sync()
{
    ste_sim_t              *sim;
    ste_sim_closure_t       pc;
    int i;


    pc.func = the_callback_sync;
    pc.user_data = 0;

    sim = ste_sim_new_st(&pc);
    if (!sim)
        return "ste_sim_new failed.";

    i = ste_sim_ping(sim, TUICC_CT_STE_SIM_CAUSE_PING);
    if (!i) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "ping did not fail when not connected.";
    }

    i = ste_sim_connect( sim, TUICC_CT_STE_SIM_CAUSE_CONNECT );
    if ( i ) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "ping connect failed.";
    }

    i = ste_sim_ping_sync( sim, TUICC_CT_STE_SIM_CAUSE_PING );
    if ( i ) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "ping_sync failed";
    }

    i = ste_sim_disconnect( sim, TUICC_CT_STE_SIM_CAUSE_DISCONNECT );
    if ( i ) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "ping disconnect failed";
    }

    ste_sim_delete(sim, TUICC_CT_NOT_USED);


    sleep(1);

    return 0;
}


const char             *test_set_sim_debug_level()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t       pc;
    int                     i;
    cat_barrier_t          *bar = cat_barrier_new();
    int                     level = 3;

    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";

    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    do {
        sync_data_t *sd = 0;
        if ( inter ) {
            printf("Set the log level for RIL SIMD\n");
            printf("0=Errors, 1=Info, 2=Debug, 3=Verbose\n");
            printf("Enter level > "); fflush(stdout);
            char buff[100];
            if ( fgets( buff, sizeof buff, stdin ) != NULL ) {
                if ( sscanf( buff, "%d", &level ) == 1 ) {
                    if ( !(level >= 0 && level <= 3) ) {
                        fprintf(stderr,"Out of range 0..3\n");
                        level = 0;
                    }
                } else {
                    fprintf(stderr,"Not an integer\n");
                }
            }
        }
        cat_barrier_set(bar);
        i = ste_sim_set_sim_log_level(uicc, TUICC_CT_STE_SIM_SET_SIM_DEBUG_LEVEL,
                level);
        if ( i ) {
          ste_sim_delete(uicc, TUICC_CT_NOT_USED);
          return "set_sim_debug_level failed";
        }
        i = cat_barrier_timedwait(bar, (void **) &sd, 1000);
        if ( i )
            return "set_sim_debug_level timed out";
        if( !sd || sd->cause != STE_SIM_CAUSE_SIM_LOG_LEVEL)
            return "set_sim_debug_level wrong response";
        /* There is no data in the response */
        sync_data_delete(sd);
    } while ( 0 );

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);

    ste_sim_delete(uicc, TUICC_CT_NOT_USED);

    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}

const char             *test_set_modem_debug_level()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t       pc;
    int                     i;
    cat_barrier_t          *bar = cat_barrier_new();
    int                     level = 3;

    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";

    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    do {
        sync_data_t *sd = 0;
        if ( inter ) {
            printf("Set the log level for RIL MAL-REQ and RIL MAL-RSP\n");
            printf("0=Errors, 1=Info, 2=Debug, 3=Verbose\n");
            printf("Enter level > "); fflush(stdout);
            char buff[100];
            if ( fgets( buff, sizeof buff, stdin ) != NULL ) {
                if ( sscanf( buff, "%d", &level ) == 1 ) {
                    if ( !(level >= 0 && level <= 3) ) {
                        fprintf(stderr,"Out of range 0..3\n");
                        level = 0;
                    }
                } else {
                    fprintf(stderr,"Not an integer\n");
                }
            }
        }
        cat_barrier_set(bar);
        i = ste_sim_set_modem_log_level(uicc, TUICC_CT_STE_SIM_SET_MODEM_DEBUG_LEVEL,
                level);
        if ( i ) {
          ste_sim_delete(uicc, TUICC_CT_NOT_USED);
          return "set_modem_debug_level failed";
        }
        i = cat_barrier_timedwait(bar, (void **) &sd, 1000);
        if ( i )
            return "set_modem_debug_level timed out";
        if( !sd || sd->cause != STE_SIM_CAUSE_MODEM_LOG_LEVEL)
            return "set_modem_debug_level wrong response";
        /* There is no data in the response */
        sync_data_delete(sd);
    } while ( 0 );

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);

    ste_sim_delete(uicc, TUICC_CT_NOT_USED);

    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}


const char             *test_readSIM_FILE_record_sync()
{
    ste_sim_t              *sim = NULL;
    ste_sim_closure_t       pc;
    int                     test_i;
    ste_uicc_sim_file_read_record_response_t read_result =
        { STE_UICC_STATUS_CODE_UNKNOWN, STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS, {0, 0}, NULL, 0};


    pc.func = the_callback_sync;
    pc.user_data = 0;

    sim = ste_sim_new_st(&pc);
    if (sim == NULL)
        return "ste_sim_new failed.";

    test_i = ste_sim_ping(sim, TUICC_CT_STE_SIM_CAUSE_PING);
    if (test_i == 0) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "ping did not fail when not connected.";
    }

    test_i = ste_sim_connect( sim, TUICC_CT_STE_SIM_CAUSE_CONNECT );
    if ( test_i != 0 ) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "ping connect failed.";
    }

    do {
        filerec_t   recInfo;

        // Read first SMS record
        recInfo.file_id = 0x6F3C;
        recInfo.v.rec_id = 1;
        recInfo.length = 0;
        recInfo.pathptr = uicc_sim ? "3F007F10" : "3F007FFF";

        if ( inter && readFileSpec( &recInfo, FILE_REC_READ_FOR_RECORD ) == 0 ) break;

        test_i = ste_uicc_sim_file_read_record_sync(sim,
                TUICC_CT_STE_UICC_CAUSE_REQ_READ_SIM_FILE_RECORD, recInfo.file_id,
                recInfo.v.rec_id, recInfo.length, recInfo.pathptr, &read_result);
    } while (0);

    if ( test_i != 0 ) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "readSIM_FILE_record_sync failed";
    }

    printf("#\tREAD LENGTH = %d hex:\n", (int)read_result.length);
    printf("#\tSTATUS WORD 1 = %d hex:\n", (int)read_result.status_word.sw1);
    printf("#\tSTATUS WORD 2 = %d hex:\n", (int)read_result.status_word.sw2);
    printf("#\tUICC STATUS CODE = %d hex:\n", (int)read_result.uicc_status_code);
    printf("#\tUICC STATUS CODE DETAILS = %d hex:\n", (int)read_result.uicc_status_code_fail_details);

    if (read_result.data) {
        memcpy(simFileBuffer,read_result.data,read_result.length);
        print_data(read_result.data,read_result.length);
        free(read_result.data);
    }

    test_i = ste_sim_disconnect( sim, TUICC_CT_STE_SIM_CAUSE_DISCONNECT );
    if ( test_i ) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "ping disconnect failed";
    }

    ste_sim_delete(sim, TUICC_CT_NOT_USED);

    sleep(1);

    return 0;
}

const char             *test_readSIM_FILE_binary_sync()
{
    ste_sim_t              *sim = NULL;
    ste_sim_closure_t       pc;
    int                     test_i;
    ste_uicc_sim_file_read_binary_response_t read_result =
       { STE_UICC_STATUS_CODE_UNKNOWN, STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS, {0, 0}, NULL, 0};


    pc.func = the_callback_sync;
    pc.user_data = 0;

    sim = ste_sim_new_st(&pc);
    if (sim == NULL)
        return "ste_sim_new failed.";

    test_i = ste_sim_ping(sim, TUICC_CT_STE_SIM_CAUSE_PING);
    if (test_i == 0) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "ping did not fail when not connected.";
    }

    test_i = ste_sim_connect( sim, TUICC_CT_STE_SIM_CAUSE_CONNECT );
    if ( test_i != 0 ) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "ping connect failed.";
    }

    do {
        filerec_t   recInfo;

        // Reads EF_PL file on USIM
        recInfo.file_id = 0x2F05;
        recInfo.v.offset = 0;
        recInfo.length = 0;
        recInfo.pathptr = "3F00";

        if ( inter && readFileSpec( &recInfo, FILE_REC_READ_FOR_BINARY ) == 0 ) break;

        test_i = ste_uicc_sim_file_read_binary_sync(sim,
                TUICC_CT_STE_UICC_CAUSE_REQ_READ_SIM_FILE_BINARY, recInfo.file_id,
                recInfo.v.offset, recInfo.length, recInfo.pathptr, &read_result);
    } while (0);

    if ( test_i != 0 ) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "readSIM_FILE_binary_sync failed";
    }

    printf("#\tREAD LENGTH = %d hex:\n", (int)read_result.length);
    printf("#\tSTATUS WORD 1 = %d hex:\n", (int)read_result.status_word.sw1);
    printf("#\tSTATUS WORD 2 = %d hex:\n", (int)read_result.status_word.sw2);
    printf("#\tUICC STATUS CODE = %d hex:\n", (int)read_result.uicc_status_code);
    printf("#\tUICC STATUS CODE DETAILS = %d hex:\n", (int)read_result.uicc_status_code_fail_details);
    printf("#\tREAD LENGTH = %d hex:\n", (int)read_result.length);

    if (read_result.data) {
        print_data(read_result.data,read_result.length);
        memcpy(simFileBuffer,read_result.data,read_result.length);
        free(read_result.data);
    }

    test_i = ste_sim_disconnect( sim, TUICC_CT_STE_SIM_CAUSE_DISCONNECT );
    if ( test_i != 0 ) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "ping disconnect failed";
    }

    ste_sim_delete(sim, TUICC_CT_NOT_USED);

    sleep(1);

    return 0;
}

const char             *test_updateSIM_FILE_record_sync()
{
    ste_sim_t              *sim;
    ste_sim_closure_t       pc;
    int                     test_i;
    ste_uicc_update_sim_file_record_response_t update_result = { -2, -2, {0,0}};


    pc.func = the_callback_sync;
    pc.user_data = 0;

    sim = ste_sim_new_st(&pc);
    if (!sim)
        return "ste_sim_new failed.";


    test_i = ste_sim_ping(sim, TUICC_CT_STE_SIM_CAUSE_PING);
    if (!test_i) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "ping did not fail when not connected.";
    }

    test_i = ste_sim_connect( sim, TUICC_CT_STE_SIM_CAUSE_CONNECT );
    if ( test_i ) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "ping connect failed.";
    }

    do {
        filerec_t   recInfo;

        // Update first SMS record by default
        recInfo.file_id = 0x6F3C;
        recInfo.v.rec_id = 1;
        recInfo.length = 176;
        recInfo.pathptr = uicc_sim ? "3F007F10" : "3F007FFF";

        memset(simFileBuffer, 0xFF, sizeof(simFileBuffer));
        simFileBuffer[0] = 0;

        if ( inter ) {
            int r = readFileSpec( &recInfo, FILE_REC_READ_FOR_RECORD );
            if ( r == 0 ) break;
            editMemory(simFileBuffer,recInfo.length);
        } else {
            strcpy((char *) simFileBuffer + 1, "Team MH - sync write");
        }

        test_i = ste_uicc_sim_file_update_record_sync(sim,
                        TUICC_CT_STE_UICC_CAUSE_REQ_UPDATE_SIM_FILE_RECORD,
                        recInfo.file_id, recInfo.v.rec_id, recInfo.length,
                        recInfo.pathptr, simFileBuffer,
                        &update_result);
    } while (0);

    if ( test_i ) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "updateSIM_FILE_record_sync failed";
    }

    printf("#\tSTATUS WORD 1 = %d hex:\n", (int)update_result.status_word.sw1);
    printf("#\tSTATUS WORD 2 = %d hex:\n", (int)update_result.status_word.sw2);
    printf("#\tUICC STATUS CODE = %d hex:\n", (int)update_result.uicc_status_code);
    printf("#\tUICC STATUS CODE DETAILS = %d hex:\n", (int)update_result.uicc_status_code_fail_details);

    test_i = ste_sim_disconnect( sim, TUICC_CT_STE_SIM_CAUSE_DISCONNECT );
    if ( test_i ) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "ping disconnect failed";
    }

    ste_sim_delete(sim, TUICC_CT_NOT_USED);

    sleep(1);

    return 0;
}

const char             *test_updateSIM_FILE_binary_sync()
{
    ste_sim_t              *sim;
    ste_sim_closure_t       pc;
    int                     test_i;
    ste_uicc_update_sim_file_binary_response_t update_result = { -2, -2, {0,0}};

    pc.func = the_callback_sync;
    pc.user_data = 0;

    sim = ste_sim_new_st(&pc);
    if (!sim)
        return "ste_sim_new failed.";

    test_i = ste_sim_ping(sim, TUICC_CT_STE_SIM_CAUSE_PING);
    if (!test_i) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "ping did not fail when not connected.";
    }

    test_i = ste_sim_connect( sim, TUICC_CT_STE_SIM_CAUSE_CONNECT );
    if ( test_i ) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "ping connect failed.";
    }

    do {
        filerec_t   recInfo;

        // Update EF_LP  by default
        recInfo.file_id = 0x2F05;
        recInfo.v.offset = 2;
        recInfo.length = 2;
        recInfo.pathptr = uicc_sim ? "3F00" : "3F00";

        memset(simFileBuffer, 0xFF, sizeof(simFileBuffer));
        simFileBuffer[0] = 0;

        if ( inter ) {
            int r = readFileSpec( &recInfo, FILE_REC_READ_FOR_BINARY );
            if ( r == 0 ) break;
            editMemory(simFileBuffer,recInfo.length);
        } else {
            simFileBuffer[0] = 'm';
            simFileBuffer[1] = 'h';
        }

        test_i = ste_uicc_sim_file_update_binary_sync(sim,
                TUICC_CT_STE_UICC_CAUSE_REQ_UPDATE_SIM_FILE_BINARY,
                recInfo.file_id, recInfo.v.offset, recInfo.length,
                recInfo.pathptr, simFileBuffer, &update_result);
    } while (0);

    if ( test_i ) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "updateSIM_FILE_binary_sync failed";
    }

    printf("#\tSTATUS WORD 1 = %d hex:\n", (int)update_result.status_word.sw1);
    printf("#\tSTATUS WORD 2 = %d hex:\n", (int)update_result.status_word.sw2);
    printf("#\tUICC STATUS CODE = %d hex:\n", (int)update_result.uicc_status_code);
    printf("#\tUICC STATUS CODE DETAILS = %d hex:\n", (int)update_result.uicc_status_code_fail_details);

    test_i = ste_sim_disconnect( sim, TUICC_CT_STE_SIM_CAUSE_DISCONNECT );
    if ( test_i ) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "ping disconnect failed";
    }

    ste_sim_delete(sim, TUICC_CT_NOT_USED);

    sleep(1);

    return 0;
}



const char             *test_simGETFILEFORMAT_sync()
{
    ste_sim_t              *sim;
    ste_sim_closure_t       pc;
    int                     i;
    int file_id             = 0x2F05;
    const char *file_path   = "3F00";
    ste_uicc_sim_file_get_format_response_t file_format;

    pc.func = the_callback_sync;
    pc.user_data = 0;

    sim = ste_sim_new_st(&pc);
    if (!sim)
        return "ste_sim_new failed.";

    i = ste_sim_ping(sim, TUICC_CT_STE_SIM_CAUSE_PING);
    if (!i) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "ping did not fail when not connected.";
    }

    i = ste_sim_connect( sim, TUICC_CT_STE_SIM_CAUSE_CONNECT );
    if ( i ) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "ping connect failed.";
    }

    i = ste_uicc_sim_file_get_format_sync(sim, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_FILE_GET_FORMAT, file_id, file_path, &file_format);

    if ( i ) {
      ste_sim_delete(sim, TUICC_CT_NOT_USED);
      return "simGETFILEFORMAT_sync failed";
    }
    printf("SIM_FILE_GET_FORMAT"
            "status_word 1 = %d\n"
            "status_word 2 = %d\n"
            "uicc status code = %d\n"
            "uicc status code details = %d\n"
            "file_type=%s\n"
            "file_size=%d\n"
            "record_len=%d\n"
            "num_records=%d\n",
            file_format.status_word.sw1,
            file_format.status_word.sw2,
            file_format.uicc_status_code,
            file_format.uicc_status_code_fail_details,
            file_types[file_format.file_type],
            file_format.file_size,
            file_format.record_len,
            file_format.num_records);

    i = ste_sim_disconnect( sim, TUICC_CT_STE_SIM_CAUSE_DISCONNECT );
    if ( i ) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "ping disconnect failed";
    }

    ste_sim_delete(sim, TUICC_CT_NOT_USED);

    sleep(1);

    return 0;
}


const char             *test_getFILEINFORMATION_sync()
{
    ste_sim_t              *sim;
    ste_sim_closure_t       pc;
    int                     i;
    const char *file_path = "3F007FFF"; // USIM;
    int file_id = 0x6F07;
    ste_uicc_get_file_information_response_t file_info;
    ste_uicc_sim_get_file_info_type_t type = STE_UICC_SIM_GET_FILE_INFO_TYPE_EF;

    pc.func = the_callback_sync;
    pc.user_data = 0;

    sim = ste_sim_new_st(&pc);
    if (!sim)
        return "ste_sim_new failed.";

    i = ste_sim_ping(sim, TUICC_CT_STE_SIM_CAUSE_PING);
    if (!i) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "ping did not fail when not connected.";
    }

    i = ste_sim_connect( sim, TUICC_CT_STE_SIM_CAUSE_CONNECT );
    if ( i ) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "ping connect failed.";
    }

    i = ste_uicc_sim_get_file_information_sync(sim,
            TUICC_CT_STE_UICC_CAUSE_REQ_GET_FILE_INFORMATION,
            file_id, file_path, type, &file_info);

    if ( i ) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "getFILEINFORMATION_sync failed";
    }

    printf("#\tGET_FILE_INFORMATION length=%d\n", file_info.length);
    printf("#\tSTATUS WORD 1 = %d hex:\n", (int)file_info.status_word.sw1);
    printf("#\tSTATUS WORD 2 = %d hex:\n", (int)file_info.status_word.sw2);
    printf("#\tUICC STATUS CODE = %d hex:\n", (int)file_info.uicc_status_code);
    printf("#\tUICC STATUS CODE DETAILS = %d hex:\n", (int)file_info.uicc_status_code_fail_details);

    for (i=0; i<file_info.length; i++)
  printf(" %#04x'%c'", file_info.fcp[i], isgraph(file_info.fcp[i]) ? file_info.fcp[i] : ' ');
    printf("\n");

    if (file_info.fcp)
        free(file_info.fcp);

    i = ste_sim_disconnect( sim, TUICC_CT_STE_SIM_CAUSE_DISCONNECT );
    if ( i ) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "ping disconnect failed";
    }

    ste_sim_delete(sim, TUICC_CT_NOT_USED);

    sleep(1);

    return 0;
}

const char *test_readREADIMSI_sync ( void )
{
    ste_sim_t              *sim;
    ste_sim_closure_t       pc;
    int                     i;
    ste_uicc_sim_file_read_imsi_response_t file_info;

    pc.func = the_callback_sync;
    pc.user_data = 0;

    sim = ste_sim_new_st(&pc);
    if (!sim)
        return "ste_sim_new failed.";

    i = ste_sim_connect( sim, TUICC_CT_STE_SIM_CAUSE_CONNECT );
    if ( i ) {
        return "readREADIMSI_sync connect failed.";
    }

    i = ste_uicc_sim_file_read_specific_imsi_sync( sim,
            TUICC_CT_STE_UICC_CAUSE_REQ_READ_SPECIFIC_CHAR_FILE, &file_info );

    if ( i )
        return "readREADIMSI_sync failed";

    printf("#\tGET_FILE_INFORMATION UICC STATUS CODE");
    printf("#\tREAD UICC STATUS CODE = %d (success=%d)\n", file_info.uicc_status_code, STE_UICC_STATUS_CODE_OK);
    printf("#\tREAD UICC STATUS CODE DETAILS = %d\n", file_info.uicc_status_code_fail_details);
    printf("#\tREAD STATUS WORD 1 = %d\n", file_info.status_word.sw1);
    printf("#\tREAD STATUS WORD 2 = %d\n", file_info.status_word.sw2);

    if ( file_info.uicc_status_code == STE_UICC_STATUS_CODE_OK ) {
        int i;
        printf("#\tRaw IMSI:");
        for ( i = 0 ; i < SIM_EF_IMSI_LEN ; i++ ) {
            printf( " %02x", file_info.raw_imsi[i] );
        }
        printf("\n");
        printf("#\tIMSI String:%s\n", file_info.imsi );
    }

    i = ste_sim_disconnect( sim, TUICC_CT_STE_SIM_CAUSE_DISCONNECT );
    if ( i ) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "readREADIMSI_sync disconnect failed";
    }

    ste_sim_delete(sim, TUICC_CT_NOT_USED);

    sleep(1);

    return 0;
}


const char *test_readIMSI ( void )
{
    ste_sim_t              *sim;
    ste_sim_closure_t       pc;
    cat_barrier_t           *bar = cat_barrier_new();

    pc.func = the_callback;
    pc.user_data = bar;

    sim = ste_sim_new(&pc);
    if (!sim)
        return "ste_sim_new failed.";

    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, sim, bar, 100);

    sleep(1);

    do {
        int test_i;
        sync_data_t *sd = 0;
        ste_uicc_sim_file_read_imsi_response_t* p;

        cat_barrier_set(bar);
        {
            test_i = ste_uicc_sim_file_read_imsi( sim, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_FILE_READ_IMSI );
        }

        if ( test_i )
            return "readIMSI failed";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 1000);
        if ( test_i )
            return "readIMSI timed out";

        if( !sd || sd->cause != STE_UICC_CAUSE_REQ_SIM_FILE_READ_IMSI )
            return "ste_uicc_sim_file_read_imsi wrong response";

        p = (ste_uicc_sim_file_read_imsi_response_t*)(sd->data);
        if (p) {
            printf("#\tREAD UICC STATUS CODE = %d\n", p->uicc_status_code);
            printf("#\tREAD UICC STATUS CODE DETAILS = %d\n", p->uicc_status_code_fail_details);
            printf("#\tREAD STATUS WORD 1 = %d\n", p->status_word.sw1);
            printf("#\tREAD STATUS WORD 2 = %d\n", p->status_word.sw2);
            if ( p->uicc_status_code == STE_UICC_STATUS_CODE_OK ) {
                int index;
                printf("#\tRaw IMSI:");
                for ( index = 0 ; index < SIM_EF_IMSI_LEN ; index++ ) {
                    printf( " %02x", p->raw_imsi[index] );
                }
                printf("\n");
                printf("#\tIMSI String:%s\n", p->imsi );
            }
        }
        else {
            printf("#\tREAD STATUS = -1, SIMD returned error.\n");
        }

        sync_data_delete(sd);
	if (!inter) break;
    } while(1);


    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, sim, bar, 100);
    ste_sim_delete(sim, TUICC_CT_NOT_USED);

    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}

const char             *test_SMSCRecordMax()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t      pc;
    cat_barrier_t           *bar = cat_barrier_new();

    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";

    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);

    do {
      int test_i;
      sync_data_t *sd = 0;
      ste_uicc_sim_smsc_get_record_max_response_t* p;

      cat_barrier_set(bar);
      {
        test_i = ste_uicc_sim_smsc_get_record_max(uicc,
           TUICC_CT_STE_UICC_CAUSE_REQ_SMSC_GET_RECORD_MAX);
      }

      if ( test_i )
          return  "ste_uicc_sim_smsc_get_record_max failed.";
      test_i = cat_barrier_timedwait(bar, (void **) &sd, 2000);
      if ( test_i )
          return "ste_uicc_sim_smsc_get_record_max timed out";

      if( !sd || sd->cause != STE_UICC_CAUSE_REQ_SMSC_GET_RECORD_MAX )
          return "ste_uicc_sim_smsc_get_record_max wrong response";

      p = (ste_uicc_sim_smsc_get_record_max_response_t *)(sd->data);
      printf("#\tGET SMSC RECORD MAX Response\n");
      printf("#\tUICC STATUS CODE: %d\n", p->uicc_status_code);
      printf("#\tUICC STATUS CODE DETAILS: %d\n", p->uicc_status_code_fail_details);
      printf("#\tRecord MAX = %d\n", p->max_record_id);
      printf("\n");

      sync_data_delete(sd);
      if (!inter) break;
    } while(1);

    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);

    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}

const char             *test_SMSCRestoreFromRecord()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t      pc;
    cat_barrier_t           *bar = cat_barrier_new();

    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";

    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);

    do {
      int test_i;
      sync_data_t *sd = 0;
      ste_uicc_sim_smsc_restore_from_record_response_t* p;
      int record_id = 2;

      if (inter) {
          printf("Enter Record ID\n");
          scanf("%d", &record_id);
      }

      cat_barrier_set(bar);
      {
        test_i = ste_uicc_sim_smsc_restore_from_record(uicc,
                                                       TUICC_CT_STE_UICC_CAUSE_REQ_SMSC_RESTORE_FROM_RECORD,
                                                       record_id);
      }

      if ( test_i )
          return  "ste_uicc_sim_smsc_restore_from_record failed.";
      test_i = cat_barrier_timedwait(bar, (void **) &sd, 2000);
      if ( test_i )
          return "ste_uicc_sim_smsc_restore_from_record timed out";

      if( !sd || sd->cause != STE_UICC_CAUSE_REQ_SMSC_RESTORE_FROM_RECORD )
          return "ste_uicc_sim_smsc_restore_from_record wrong response";

      p = (ste_uicc_sim_smsc_restore_from_record_response_t *)(sd->data);
      printf("#\tRESTORE FROM RECORD Response\n");
      printf("#\tUICC STATUS CODE: %d\n", p->uicc_status_code);
      printf("#\tUICC STATUS CODE DETAILS: %d\n", p->uicc_status_code_fail_details);
      printf("\n");

      sync_data_delete(sd);
      if (!inter) break;
    } while(1);

    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);

    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}

const char             *test_SMSCSaveToRecord()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t      pc;
    cat_barrier_t           *bar = cat_barrier_new();

    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";

    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);

    do {
      int test_i;
      sync_data_t *sd = 0;
      ste_uicc_sim_smsc_save_to_record_response_t* p;
      int record_id = 2;

      if (inter) {
          printf("Enter Record ID\n");
          scanf("%d", &record_id);
      }

      cat_barrier_set(bar);
      {
        test_i = ste_uicc_sim_smsc_save_to_record(uicc,
                                                  TUICC_CT_STE_UICC_CAUSE_REQ_SMSC_SAVE_TO_RECORD,
                                                  record_id);
      }

      if ( test_i )
          return  "ste_uicc_sim_smsc_save_to_record.";
      test_i = cat_barrier_timedwait(bar, (void **) &sd, 2000);
      if ( test_i )
          return "ste_uicc_sim_smsc_save_to_record timed out";

      if( !sd || sd->cause != STE_UICC_CAUSE_REQ_SMSC_SAVE_TO_RECORD )
          return "ste_uicc_sim_smsc_save_to_record wrong response";

      p = (ste_uicc_sim_smsc_save_to_record_response_t *)(sd->data);
      printf("#\tSAVE TO RECORD Response\n");
      printf("#\tUICC STATUS CODE: %d\n", p->uicc_status_code);
      printf("#\tUICC STATUS CODE DETAILS: %d\n", p->uicc_status_code_fail_details);
      printf("#\tUICC STATUS WORD 1: %d\n", p->status_word.sw1);
      printf("#\tUICC STATUS WORD 2: %d\n", p->status_word.sw2);
      printf("\n");

      sync_data_delete(sd);
      if (!inter) break;
    } while(1);

    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);

    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}

const char             *test_getSMSC()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t      pc;
    int                     i;
    cat_barrier_t           *bar = cat_barrier_new();


    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);

    do {
      int test_i;
      sync_data_t *sd = 0;
      ste_uicc_sim_smsc_get_active_response_t* p;
      char * index_p = NULL;

        cat_barrier_set(bar);
        {
	  test_i = ste_uicc_sim_smsc_get_active(uicc,
	     TUICC_CT_STE_UICC_CAUSE_REQ_SMSC_GET_ACTIVE);
        }

        if ( test_i )
            return  "ste_uicc_sim_smsc_get_active failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 2000);
        if ( test_i )
            return "ste_uicc_sim_smsc_get_active timed out";

        if( !sd || sd->cause != STE_UICC_CAUSE_REQ_SMSC_GET_ACTIVE )
            return "ste_uicc_sim_smsc_get_active wrong response";

        p = (ste_uicc_sim_smsc_get_active_response_t*)(sd->data);
        printf("#\tREAD SMSC Response\n");
        printf("#\tUICC STATUS CODE: %d\n", p->uicc_status_code);
        printf("#\tUICC STATUS CODE DETAILS: %d\n", p->uicc_status_code_fail_details);

        if (p->uicc_status_code == STE_UICC_STATUS_CODE_OK && p->smsc.num_text.no_of_characters == 0 && p->smsc.num_text.text_p == NULL) {
            printf("#\t    NO valid SMSC record on SIM card\n");
        }
        else {
            printf("#\t    Nr Of Characters = %d\n", p->smsc.num_text.no_of_characters);
            printf("#\t    TON = %d\n", p->smsc.ton);
            printf("#\t    NPI = %d\n", p->smsc.npi);
            index_p = (char*)(p->smsc.num_text.text_p);
            printf("#\t    NUMBER = ");
            for (i = 0; i < (int)(p->smsc.num_text.no_of_characters); i++)
                printf("  %02x", index_p[i]);
            printf("\n");
        }

        if (index_p)
            free(index_p);

        sync_data_delete(sd);
	if (!inter) break;
    } while(1);


    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);

    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}


const char             *test_setSMSC()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t       pc;
    cat_barrier_t          *bar = cat_barrier_new();


    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);

    do {
      int test_i;
      sync_data_t *sd = 0;
      ste_uicc_sim_smsc_set_active_response_t* p;
      ste_sim_call_number_t smsc;
      uint8_t num[] = "46461111116";

      if (inter) {
          printf("Enter SMSC (11 numbers): ");
          scanf("%11s", num);
      }

      smsc.num_text.text_coding = STE_SIM_ASCII8;
      smsc.num_text.no_of_characters = 11;
      smsc.num_text.text_p = (void*)num;
      smsc.ton = (ste_sim_type_of_number_t)STE_SIM_TON_INTERNATIONAL;
      smsc.npi = (ste_sim_numbering_plan_id_t)STE_SIM_NPI_ISDN;


        cat_barrier_set(bar);
        {
            test_i = ste_uicc_sim_smsc_set_active(uicc,
            TUICC_CT_STE_UICC_CAUSE_REQ_SMSC_SET_ACTIVE, &smsc);
        }

        if ( test_i )
            return  "ste_uicc_sim_smsc_set_active failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 2000);
        if ( test_i )
            return "ste_uicc_sim_smsc_set_active timed out";

        if( !sd || sd->cause != STE_UICC_CAUSE_REQ_SMSC_SET_ACTIVE )
            return "ste_uicc_sim_smsc_set_active wrong response";

        p = (ste_uicc_sim_smsc_set_active_response_t*)(sd->data);
        printf("#\tSTE_UICC_FILE_UPDATE_SMSC_RESP: UICC STATUS CODE = %d\n", p->uicc_status_code);
        printf("#\tSTE_UICC_FILE_UPDATE_SMSC_RESP: UICC STATUS CODE DETAILS = %d\n", p->uicc_status_code_fail_details);
        printf("#\tSTE_UICC_FILE_UPDATE_SMSC_RESP: STATUS WORD 1 = %d\n", p->status_word.sw1);
        printf("#\tSTE_UICC_FILE_UPDATE_SMSC_RESP: STATUS WORD 2 = %d\n", p->status_word.sw2);

        sync_data_delete(sd);
	if (!inter) break;
    } while(1);


    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);

    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}


const char *test_getSMSC_sync ( void )
{
    ste_sim_t              *sim;
    ste_sim_closure_t       pc;
    int                     i;
    ste_uicc_sim_smsc_get_active_response_t smsc_info;

    pc.func = the_callback_sync;
    pc.user_data = 0;

    sim = ste_sim_new_st(&pc);
    if (!sim)
        return "ste_sim_new failed.";

    i = ste_sim_connect( sim, TUICC_CT_STE_SIM_CAUSE_CONNECT );
    if ( i ) {
        return "readSMSC_sync connect failed.";
    }

    i = ste_uicc_sim_smsc_get_active_sync( sim,
            TUICC_CT_STE_UICC_CAUSE_REQ_SMSC_GET_ACTIVE, &smsc_info );

    if ( i ) {
        printf("#\t\nREAD_SMSC STATUS CODE = %d\n STATUS CODE DETAILS = %d\n",
            smsc_info.uicc_status_code,
            smsc_info.uicc_status_code_fail_details);
        return "readSMSC_sync failed";
    }

    printf("#\tREAD_SMSC STATUS = %d (success=%d)\n",
            smsc_info.uicc_status_code, STE_UICC_STATUS_CODE_OK);
    if ( smsc_info.uicc_status_code == STE_UICC_STATUS_CODE_OK ) {
        unsigned i;
        uint8_t * temp_p = smsc_info.smsc.num_text.text_p;

        printf("#\tSMSC:");
        for ( i = 0 ; i < smsc_info.smsc.num_text.no_of_characters ; i++ ) {
            printf( " %02x", *temp_p++ );
        }
        printf("\n#\tSMSC String:%s\n", (char*)smsc_info.smsc.num_text.text_p );
        free(smsc_info.smsc.num_text.text_p);
    }

    i = ste_sim_disconnect( sim, TUICC_CT_STE_SIM_CAUSE_DISCONNECT );
    if ( i ) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "readSMSC_sync disconnect failed";
    }

    ste_sim_delete(sim, TUICC_CT_NOT_USED);

    sleep(1);

    return 0;
}

const char *test_setSMSC_sync ( void )
{
    ste_sim_t              *sim;
    ste_sim_closure_t       pc;
    int                     i;
    ste_sim_call_number_t   smsc;
    ste_uicc_sim_smsc_set_active_response_t set_result;
    uint8_t           num[] = "4646111111";

    smsc.num_text.text_coding = STE_SIM_ASCII8;
    smsc.num_text.no_of_characters = 10;
    smsc.num_text.text_p = (void*)num;
    smsc.ton = (ste_sim_type_of_number_t)STE_SIM_TON_INTERNATIONAL;
    smsc.npi = (ste_sim_numbering_plan_id_t)STE_SIM_NPI_ISDN;

    pc.func = the_callback_sync;
    pc.user_data = 0;

    sim = ste_sim_new_st(&pc);
    if (!sim)
        return "ste_sim_new failed.";

    i = ste_sim_connect( sim, TUICC_CT_STE_SIM_CAUSE_CONNECT );
    if ( i ) {
        return "setSMSC_sync connect failed.";
    }

    i = ste_uicc_sim_smsc_set_active_sync( sim,
            TUICC_CT_STE_UICC_CAUSE_REQ_SMSC_SET_ACTIVE, &smsc, &set_result );

    if ( i )
        return "setSMSC_sync failed";

    printf("#\tSET SMSC UICC STATUS CODE = %d (success=%d)\n", set_result.uicc_status_code,STE_UICC_STATUS_CODE_OK);
    printf("#\tSET SMSC UICC STATUS CODE DETAILS = %d\n", set_result.uicc_status_code_fail_details);
    printf("#\tSET SMSC STATUS WORD 1 = %d\n", set_result.status_word.sw1);
    printf("#\tSET SMSC STATUS WORD 2 = %d\n", set_result.status_word.sw2);

    i = ste_sim_disconnect( sim, TUICC_CT_STE_SIM_CAUSE_DISCONNECT );
    if ( i ) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "setSMSC_sync disconnect failed";
    }

    ste_sim_delete(sim, TUICC_CT_NOT_USED);

    sleep(1);

    return 0;
}


const char             *test_updateServiceTable()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t       pc;
    cat_barrier_t          *bar = cat_barrier_new();
    sim_service_type_t      check_for_service_type[] = {
      SIM_SERVICE_TYPE_FDN,
      SIM_SERVICE_TYPE_BDN,
      SIM_SERVICE_TYPE_ACL,
      SIM_SERVICE_TYPE_UNKNOWN
    };
    int                     continue_test = sizeof(check_for_service_type)/sizeof(sim_service_type_t);
    int                     enable_service = 1;


    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);

    do {
      int test_i;
      sync_data_t *sd = 0;
      ste_uicc_update_service_table_response_t* p;


        cat_barrier_set(bar);
        {
          if (inter) {
            char pin[10];
            int service;
            sim_service_type_t service_type;

            printf("Enter PIN2: ");
            scanf("%4s", pin);
            printf("Enter service (1 = FDN, 5 = BDN, 32 = ACL ): ");
            scanf("%d", &service);
            printf("Enter status (1 = enable, 0 = disable): ");
            scanf("%d", &enable_service);

            switch( service ){
              case 1: service_type = SIM_SERVICE_TYPE_FDN; break;
              case 5: service_type = SIM_SERVICE_TYPE_BDN; break;
              case 32: service_type = SIM_SERVICE_TYPE_ACL; break;
              default: service_type = SIM_SERVICE_TYPE_UNKNOWN; break;
            }

            test_i = ste_uicc_update_service_table(uicc,
                     TUICC_CT_STE_UICC_CAUSE_REQ_UPDATE_SERVICE_TABLE, pin, service_type, (uint8_t)enable_service);
          }
          else
            test_i = ste_uicc_update_service_table(uicc,
                     TUICC_CT_STE_UICC_CAUSE_REQ_UPDATE_SERVICE_TABLE, "5678", check_for_service_type[--continue_test], (uint8_t)enable_service);
        }

        if ( test_i )
            return  "ste_uicc_update_service_table failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 2000);
        if ( test_i )
            return "ste_uicc_update_service_table timed out";

        if( !sd || sd->cause != STE_UICC_CAUSE_REQ_UPDATE_SERVICE_TABLE )
            return "ste_uicc_update_service_table wrong response";

        p = (ste_uicc_update_service_table_response_t*)(sd->data);
        printf("#\tSTE_UICC_FILE_UPDATE_SERVICE_TABLE_RESP: UICC STATUS CODE = %d\n", p->uicc_status_code);
        printf("#\tSTE_UICC_FILE_UPDATE_SERVICE_TABLE_RESP: UICC STATUS CODE DETAILS = %d\n", p->uicc_status_code_fail_details);
        printf("#\tSTE_UICC_FILE_UPDATE_SERVICE_TABLE_RESP: STATUS WORD 1 = %d\n", p->status_word.sw1);
        printf("#\tSTE_UICC_FILE_UPDATE_SERVICE_TABLE_RESP: STATUS WORD 2 = %d\n", p->status_word.sw2);

        sync_data_delete(sd);

        if (inter) {
        printf("Continue test? (0 = No, 1 = Yes): ");
        scanf("%d", &continue_test);
        } else if ( continue_test == 0 && enable_service == 1 ) {
          // Run one more round with enable service set to 0
          continue_test = sizeof(check_for_service_type)/sizeof(sim_service_type_t);
          enable_service = 0;
        }
    } while(continue_test);


    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);

    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}


const char             *test_getServiceTable()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t       pc;
    cat_barrier_t          *bar = cat_barrier_new();
    sim_service_type_t      check_for_service_type[] = {
        SIM_SERVICE_TYPE_FDN,
        SIM_SERVICE_TYPE_BDN,
        SIM_SERVICE_TYPE_ACL,
        SIM_SERVICE_TYPE_UNKNOWN
    };
    int                     continue_test = sizeof(check_for_service_type)/sizeof(sim_service_type_t);


    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);

    do {
      int test_i;
      sync_data_t *sd = 0;
      ste_uicc_get_service_table_response_t* p;

        cat_barrier_set(bar);
        {
          if (inter) {
            int service;
            sim_service_type_t service_type;

            printf("Enter service (1 = FDN, 5 = BDN, 32 = ACL): ");
            scanf("%d", &service);

            switch( service ){
              case 1: service_type = SIM_SERVICE_TYPE_FDN; break;
              case 5: service_type = SIM_SERVICE_TYPE_BDN; break;
              case 32: service_type = SIM_SERVICE_TYPE_ACL; break;
              default: service_type = SIM_SERVICE_TYPE_UNKNOWN; break;
            }

            test_i = ste_uicc_get_service_table(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_GET_SERVICE_TABLE, service_type);
          }
          else
            test_i = ste_uicc_get_service_table(uicc,
                                                TUICC_CT_STE_UICC_CAUSE_REQ_GET_SERVICE_TABLE, 
                                                check_for_service_type[--continue_test]);
        }

        if ( test_i )
            return  "ste_uicc_get_service_table failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 2000);
        if ( test_i )
            return "ste_uicc_get_service_table timed out";

        if( !sd || sd->cause != STE_UICC_CAUSE_REQ_GET_SERVICE_TABLE )
            return "ste_uicc_get_service_table wrong response";

        p = (ste_uicc_get_service_table_response_t*)(sd->data);
        printf("#\tSTE_UICC_FILE_GET_SERVICE_TABLE_RESP: UICC STATUS CODE = %d\n", p->uicc_status_code);
        printf("#\tSTE_UICC_FILE_GET_SERVICE_TABLE_RESP: UICC STATUS CODE DETAILS = %d\n", p->uicc_status_code_fail_details);
        printf("#\tSTE_UICC_FILE_GET_SERVICE_TABLE_RESP: STATUS WORD 1 = %d\n", p->status_word.sw1);
        printf("#\tSTE_UICC_FILE_GET_SERVICE_TABLE_RESP: STATUS WORD 2 = %d\n", p->status_word.sw2);
        printf("#\tSTE_UICC_FILE_GET_SERVICE_TABLE_RESP: SERVICE STATUS = %d\n", p->service_status);

        sync_data_delete(sd);

        if (inter) {
            printf("Continue test? (0 = No, 1 = Yes): ");
            scanf("%d", &continue_test);
        }

    } while(continue_test);


    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);

    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}


const char             *test_getServiceTable_sync()
{
    ste_sim_t              *sim = NULL;
    ste_sim_closure_t       pc;
    int                     test_i;
    int                     continue_test = 1;


    pc.func = the_callback_sync;
    pc.user_data = 0;

    sim = ste_sim_new_st(&pc);
    if (!sim)
        return "ste_sim_new_st failed.";

    test_i = ste_sim_ping(sim, TUICC_CT_STE_SIM_CAUSE_PING);
    if (test_i == 0) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "ping did not fail when not connected.";
    }

    test_i = ste_sim_connect( sim, TUICC_CT_STE_SIM_CAUSE_CONNECT );
    if ( test_i != 0 ) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "ping connect failed.";
    }

    do {
      ste_uicc_get_service_table_response_t  read_response =
       { STE_UICC_STATUS_CODE_UNKNOWN, STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS, {0, 0}, 0};

      if (inter) {
        int service;
        sim_service_type_t service_type;

        printf("Enter service (1 = FDN, 5 = BDN, 32 = ACL ): ");
        scanf("%d", &service);

        switch( service ){
          case 1: service_type = SIM_SERVICE_TYPE_FDN; break;
          case 5: service_type = SIM_SERVICE_TYPE_BDN; break;
          case 32: service_type = SIM_SERVICE_TYPE_ACL; break;
          default: service_type = SIM_SERVICE_TYPE_UNKNOWN; break;
        }

        test_i = ste_uicc_get_service_table_sync(sim, TUICC_CT_STE_UICC_CAUSE_REQ_GET_SERVICE_TABLE, service_type, &read_response);
      }
      else
        test_i = ste_uicc_get_service_table_sync(sim, TUICC_CT_STE_UICC_CAUSE_REQ_GET_SERVICE_TABLE, SIM_SERVICE_TYPE_FDN, &read_response);

      if ( test_i != 0 ) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "ste_uicc_get_service_table failed";
      }

      printf("#\tSTE_UICC_FILE_GET_SERVICE_TABLE_RESP: UICC STATUS CODE = %d\n", (int)read_response.uicc_status_code);
      printf("#\tSTE_UICC_FILE_GET_SERVICE_TABLE_RESP: UICC STATUS CODE DETAILS = %d\n", (int)read_response.uicc_status_code_fail_details);
      printf("#\tSTE_UICC_FILE_GET_SERVICE_TABLE_RESP: STATUS WORD 1 = %d\n", (int)read_response.status_word.sw1);
      printf("#\tSTE_UICC_FILE_GET_SERVICE_TABLE_RESP: STATUS WORD 2 = %d\n", (int)read_response.status_word.sw2);
      printf("#\tSTE_UICC_FILE_GET_SERVICE_TABLE_RESP: SERVICE STATUS = %d\n", (int)read_response.service_status);

      if ((!inter)) break;

      printf("Continue test? (0 = No, 1 = Yes): ");
      scanf("%d", &continue_test);

      if (continue_test==0) break;

    } while(1);

    sleep(1);

    test_i = ste_sim_disconnect( sim, TUICC_CT_STE_SIM_CAUSE_DISCONNECT );
    if ( test_i != 0 ) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "ping disconnect failed";
    }

    ste_sim_delete(sim, TUICC_CT_NOT_USED);

    sleep(1);

    return 0;
}


const char             *test_getServiceAvailability()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t       pc;
    cat_barrier_t          *bar = cat_barrier_new();
    sim_service_type_t      check_for_Services[] = {
      SIM_SERVICE_TYPE_LOCAL_PHONE_BOOK,
      SIM_SERVICE_TYPE_FDN,                       // Fixed Dialling Number
      SIM_SERVICE_TYPE_EXT_2,                     // Extension 2
      SIM_SERVICE_TYPE_SDN,                       // Service Dialling Number
      SIM_SERVICE_TYPE_EXT_3,                     // Extension 3
      SIM_SERVICE_TYPE_BDN,                       // Barred Dialling Number
      SIM_SERVICE_TYPE_EXT_4,                     // Extension 4
      SIM_SERVICE_TYPE_OCI_AND_OCT,               // Outgoing Call Information (OCI and OCT)
      SIM_SERVICE_TYPE_ICI_AND_ICT,               // Incoming Call Information (ICI and ICT)
      SIM_SERVICE_TYPE_SMS,                       // Short Message Storage
      SIM_SERVICE_TYPE_SMSR,                      // Short Message Service Reports
      SIM_SERVICE_TYPE_SMSP,                      // Short Message Service Parameters
      SIM_SERVICE_TYPE_AOC,                       // Advice of Charge
      SIM_SERVICE_TYPE_CCP,                       // Capability Configuration Parameters
      SIM_SERVICE_TYPE_CBMI,                      // Cell Broadcast Message Identifier
      SIM_SERVICE_TYPE_CBMIR,                     // Cell Broadcast Message Identifier Ranges
      SIM_SERVICE_TYPE_GID1,                      // Group Identifier Level 1
      SIM_SERVICE_TYPE_GID2,                      // Group Identifier Level 2
      SIM_SERVICE_TYPE_SPN,                       // Service Provider Name
      SIM_SERVICE_TYPE_PLMNWACT,                  // User controlled PLMN selector with Access Technology
      SIM_SERVICE_TYPE_MSISDN,                    // MSISDN
      SIM_SERVICE_TYPE_IMG,                       // Image
      SIM_SERVICE_TYPE_SOLSA,                     // Support of Localised Service Areas
      SIM_SERVICE_TYPE_ENH_PRECEDENCE_PREEMPTION, // Enhanced Multi-Level Precedence and Pre-emption Service
      SIM_SERVICE_TYPE_AAEM,                      // Automatic Answer for eMLPP
      SIM_SERVICE_TYPE_GSM_ACCESS,                // GSM Access
      SIM_SERVICE_TYPE_SMS_PP,                    // Data download via SMS-PP
      SIM_SERVICE_TYPE_SMS_CB,                    // Data download via SMS-CB
      SIM_SERVICE_TYPE_CALL_CONTROL_BY_USIM,      // Call Control by USIM
      SIM_SERVICE_TYPE_MO_SMS_CONTROL_BY_USIM,    // MO-SMS Control by USIM
      SIM_SERVICE_TYPE_RUN_AT_COMMAND,            // RUN AT COMMAND command
      SIM_SERVICE_TYPE_EST,                       // Enabled Services Table
      SIM_SERVICE_TYPE_ACL,                       // APN Control List
      SIM_SERVICE_TYPE_DCK,                       // Depersonalisation Control Keys
      SIM_SERVICE_TYPE_CNL,                       // Co-operative Network List
      SIM_SERVICE_TYPE_GSM_SECURITY_CONTEXT,      // GSM security context
      SIM_SERVICE_TYPE_CPBCCH,                    // CPBCCH Information
      SIM_SERVICE_TYPE_INV_SCAN,                  // Investigation Scan
      SIM_SERVICE_TYPE_MEXE,                      // MExE Service table
      SIM_SERVICE_TYPE_OPLMNWACT,                 // Operator controlled PLMN selector with Access Technology
      SIM_SERVICE_TYPE_HPLMNWACT,                 // HPLMN selector with Access Technology
      SIM_SERVICE_TYPE_EXT_5,                     // Extension 5
      SIM_SERVICE_TYPE_PNN,                       // PLMN Network Name
      SIM_SERVICE_TYPE_OPL,                       // Operator PLMN List
      SIM_SERVICE_TYPE_MBDN,                      // Mailbox Dialling Numbers
      SIM_SERVICE_TYPE_MWIS,                      // Message Waiting Indication Status
      SIM_SERVICE_TYPE_CFIS,                      // Call Forwarding Indication Status
      SIM_SERVICE_TYPE_SPDI,                      // Service Provider Display Information
      SIM_SERVICE_TYPE_MMS,                       // Multimedia Messaging Service
      SIM_SERVICE_TYPE_EXT_8,                     // Extension 8
      SIM_SERVICE_TYPE_CALL_CONTROL_ON_GPRS,      // Call control on GPRS by USIM
      SIM_SERVICE_TYPE_MMSUCP,                    // MMS User Connectivity Parameters
      SIM_SERVICE_TYPE_NIA,                       // Network's indication of alerting in the MS
      SIM_SERVICE_TYPE_VGCS_AND_VGCSS,            // VGCS Group Identifier List
      SIM_SERVICE_TYPE_VBS_AND_VBSS,              // VBS Group Identifier List
      SIM_SERVICE_TYPE_PSEUDO,                    // Pseudonym
      SIM_SERVICE_TYPE_UPLMNWLAN,                 // User Controlled PLMN selector for WLAN access
      SIM_SERVICE_TYPE_OPLMNWLAN,                 // Operator Controlled PLMN selector for WLAN access
      SIM_SERVICE_TYPE_USER_CONTR_WSID_LIST,      // User controlled WSID list
      SIM_SERVICE_TYPE_OPERATOR_CONTR_WSID_LIST,  // Operator controlled WSID list
      SIM_SERVICE_TYPE_VGCS_SECURITY,             // VGCS security
      SIM_SERVICE_TYPE_VBS_SECURITY,              // VBS security
      SIM_SERVICE_TYPE_WRI,                       // WLAN Reauthentication Identity
      SIM_SERVICE_TYPE_MMS_STORAGE,               // Multimedia Messages Storage
      SIM_SERVICE_TYPE_GBA,                       // Generic Bootstrapping Architecture
      SIM_SERVICE_TYPE_MBMS_SECURITY,             // MBMS security
      SIM_SERVICE_TYPE_DDL_USSD_APPL_MODE,        // Data download via USSD and USSD application mode
      SIM_SERVICE_TYPE_ADD_TERM_PROF,             // Additional TERMINAL PROFILE after UICC activation
      SIM_SERVICE_TYPE_CHV1_DISABLE,              // CHV1 disable function
      SIM_SERVICE_TYPE_ADN,                       // Abbreviated Dialling Numbers
      SIM_SERVICE_TYPE_PLMNSEL,                   // PLMN selector
      SIM_SERVICE_TYPE_EXT_1,                     // Extension 1
      SIM_SERVICE_TYPE_LND,                       // Last Number Dialled
      SIM_SERVICE_TYPE_MENU_SELECT,               // Menu selection
      SIM_SERVICE_TYPE_CALL_CONTROL,              // Call control
      SIM_SERVICE_TYPE_PROACTIVE_SIM,             // Proactive SIM
      SIM_SERVICE_TYPE_MO_SMS_CONTR_BY_SIM,       // Mobile Originated Short Message control by SIM
      SIM_SERVICE_TYPE_GPRS,                      // GPRS
      SIM_SERVICE_TYPE_USSD_STR_DATA_SUPPORT,     // USSD string data object supported in Call Control
      SIM_SERVICE_TYPE_ECCP,                       // Extended Capability Configuration Parameters
      SIM_SERVICE_TYPE_UNKNOWN
    };
    int                     continue_test = sizeof(check_for_Services)/ sizeof(sim_service_type_t);

    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);

    do {
      int test_i;
      sync_data_t *sd = 0;
      ste_uicc_get_service_availability_response_t* p;

        cat_barrier_set(bar);
        {
          if (inter) {
            int service;
            sim_service_type_t service_type;

            printf("Enter service number (0-79): ");
            scanf("%d", &service);

            if ((service >= 0) && (service <= 79))
              service_type = service;
            else
              service_type = SIM_SERVICE_TYPE_UNKNOWN;

            test_i = ste_uicc_get_service_availability(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY, service_type);
          }
          else
            test_i = ste_uicc_get_service_availability(uicc,
                                                       TUICC_CT_STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY,
                                                       check_for_Services[--continue_test]);
        }

        if ( test_i )
            return  "ste_uicc_get_service_availability failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 2000);
        if ( test_i )
            return "ste_uicc_get_service_availability timed out";

        if( !sd || sd->cause != STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY )
            return "ste_uicc_get_service_availability wrong response";

        p = (ste_uicc_get_service_availability_response_t*)(sd->data);
        printf("#\tSTE_UICC_FILE_GET_SERVICE_AVAILABILITY_RESP: UICC STATUS CODE = %d\n", p->uicc_status_code);
        printf("#\tSTE_UICC_FILE_GET_SERVICE_AVAILABILITY_RESP: UICC STATUS CODE DETAILS = %d\n", p->uicc_status_code_fail_details);
        printf("#\tSTE_UICC_FILE_GET_SERVICE_AVAILABILITY_RESP: SERVICE AVAILABILITY = %d\n", p->service_availability);

        sync_data_delete(sd);

        if ( inter ) {
            printf("Continue test? (0 = No, 1 = Yes): ");
            scanf("%d", &continue_test);
        }
    } while(continue_test != 0 );


    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);

    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}



const char             *test_getServiceAvailability_sync()
{
    ste_sim_t              *sim = NULL;
    ste_sim_closure_t       pc;
    int                     test_i;
    int                     continue_test = 1;


    pc.func = the_callback_sync;
    pc.user_data = 0;

    sim = ste_sim_new_st(&pc);
    if (!sim)
        return "ste_sim_new_st failed.";

    test_i = ste_sim_ping(sim, TUICC_CT_STE_SIM_CAUSE_PING);
    if (test_i == 0) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "ping did not fail when not connected.";
    }

    test_i = ste_sim_connect( sim, TUICC_CT_STE_SIM_CAUSE_CONNECT );
    if ( test_i != 0 ) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "connect failed.";
    }

    do {
      ste_uicc_get_service_availability_response_t  read_response =
       { STE_UICC_STATUS_CODE_UNKNOWN, STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS, 0};

      if (inter) {
        int service;
        sim_service_type_t service_type;

        printf("Enter service number (0-79): ");
        scanf("%d", &service);

        if ((service >= 0) && (service <= 79))
          service_type = service;
        else
          service_type = SIM_SERVICE_TYPE_UNKNOWN;

        test_i = ste_uicc_get_service_availability_sync(sim, TUICC_CT_STE_UICC_CAUSE_REQ_GET_SERVICE_TABLE, service_type, &read_response);
      }
      else
        test_i = ste_uicc_get_service_availability_sync(sim, TUICC_CT_STE_UICC_CAUSE_REQ_GET_SERVICE_TABLE, SIM_SERVICE_TYPE_FDN, &read_response);

      if ( test_i != 0 ) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "ste_uicc_get_service_availability failed";
      }

      printf("#\tSTE_UICC_FILE_GET_SERVICE_AVAILABILITY_RESP: UICC STATUS CODE = %d\n", (int)read_response.uicc_status_code);
      printf("#\tSTE_UICC_FILE_GET_SERVICE_AVAILABILITY_RESP: UICC STATUS CODE DETAILS = %d\n", (int)read_response.uicc_status_code_fail_details);
      printf("#\tSTE_UICC_FILE_GET_SERVICE_AVAILABILITY_RESP: SERVICE AVAILABILITY = %d\n", (int)read_response.service_availability);

      if ((!inter)) break;

      printf("Continue test? (0 = No, 1 = Yes): ");
      scanf("%d", &continue_test);

      if (continue_test==0) break;

    } while(1);

    sleep(1);

    test_i = ste_sim_disconnect( sim, TUICC_CT_STE_SIM_CAUSE_DISCONNECT );
    if ( test_i != 0 ) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "disconnect failed";
    }

    ste_sim_delete(sim, TUICC_CT_NOT_USED);

    sleep(1);

    return 0;
}





const char             *test_register()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t       pc;
    cat_barrier_t           *bar = cat_barrier_new();

    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";

    // Connect
    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    // Register
    TTEST(ste_uicc_register, TUICC_CT_STE_UICC_CAUSE_REGISTER, uicc, bar, 100);


    // Disconnect
    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);

    ste_sim_delete(uicc, TUICC_CT_NOT_USED);

    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}



const char             *test_connect10()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t       pc;
    cat_barrier_t           *bar = cat_barrier_new();

    unsigned int            n = 10;

    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";

    while (n--) {

        TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

        TTEST(ste_sim_ping, TUICC_CT_STE_SIM_CAUSE_PING, uicc, bar, 100);

        TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    }

    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}



const char             *test_shutdown()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t       pc;
    int                     i;
    cat_barrier_t           *bar = cat_barrier_new();


    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    TTEST(ste_sim_shutdown, TUICC_CT_STE_SIM_CAUSE_SHUTDOWN, uicc, bar, 100);

    sleep(1);

    i = ste_sim_disconnect(uicc, TUICC_CT_STE_SIM_CAUSE_DISCONNECT);
    if (i) {
        ste_sim_delete(uicc, TUICC_CT_NOT_USED);
        cat_barrier_delete(bar);
        return "test_shutdown: disconnect failed.";
    }

    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}

const char             *test_sendPIN()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t       pc;
    cat_barrier_t          *bar = cat_barrier_new();


    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);

    do {
        int test_i;
        sync_data_t            *sd = 0;
        ste_uicc_pin_verify_response_t* p;

        cat_barrier_set(bar);
        if (inter) {
            char code[10];
            int input;
            sim_pin_id_t pin_id;
            printf("PIN: ");
            scanf("%4s", code);
            printf("PIN ID (1 or 2): ");
            scanf("%d", &input);

            pin_id = (input == 2 ? SIM_PIN_ID_PIN2 : SIM_PIN_ID_PIN1);

            test_i = ste_uicc_pin_verify(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_VERIFY, pin_id, code);
        } else {
            test_i = ste_uicc_pin_verify(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_VERIFY, SIM_PIN_ID_PIN1, "1111");
        }

        if ( test_i )
            return  "ste_uicc_pin_verify failed.";
        test_i = cat_barrier_timedwait(bar, (void **) &sd, 1000);
        if ( test_i )
            return "ste_uicc_pin_verify timed out";

        if( sd && sd->cause == STE_UICC_CAUSE_REQ_PIN_VERIFY ) {
            printf("#\tSIM pin verify response received \n");
            p = (ste_uicc_pin_verify_response_t*)(sd->data);
            printf("#\tSTATUS WORD 1 = %d hex:\n", (int)p->status_word.sw1);
            printf("#\tSTATUS WORD 2 = %d hex:\n", (int)p->status_word.sw2);
            printf("#\tUICC STATUS CODE = %d hex:\n", (int)p->uicc_status_code);
            printf("#\tUICC STATUS CODE DETAILS = %d hex:\n", (int)p->uicc_status_code_fail_details);
            sync_data_delete(sd);
            break;
         }

        sync_data_delete(sd);
    } while(0);

    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);
    return 0;
}


const char             *test_sendPUK()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t       pc;
    cat_barrier_t          *bar = cat_barrier_new();


    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);

    do {
        int test_i;
        sync_data_t            *sd = 0;
        ste_uicc_pin_unblock_response_t* p;

        cat_barrier_set(bar);
        if (inter) {
          char pin_code[10], puk_code[10];
          printf("PUK: ");
          scanf("%8s", puk_code);
          printf("New PIN: ");
          scanf("%4s", pin_code);
          test_i = ste_uicc_pin_unblock(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_UNBLOCK, SIM_PIN_ID_PIN1, pin_code, puk_code);
        } else {
          test_i = ste_uicc_pin_unblock(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_UNBLOCK, SIM_PIN_ID_PIN1, "1234", "11111111");
        }
        if ( test_i )
            return  "ste_uicc_pin_unblock failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 1000);
        if ( test_i )
            return "ste_uicc_pin_unblock timed out";

        if( sd && sd->cause == STE_UICC_CAUSE_REQ_PIN_UNBLOCK ) {
            p = (ste_uicc_pin_unblock_response_t*)(sd->data);
            printf("#\tSTATUS WORD 1 = %d hex:\n", (int)p->status_word.sw1);
            printf("#\tSTATUS WORD 2 = %d hex:\n", (int)p->status_word.sw2);
            printf("#\tUICC STATUS CODE = %d hex:\n", (int)p->uicc_status_code);
            printf("#\tUICC STATUS CODE DETAILS = %d hex:\n", (int)p->uicc_status_code_fail_details);
        }

        sync_data_delete(sd);
    } while(0);


    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);

    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}


const char             *test_changePIN()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t       pc;
    cat_barrier_t          *bar = cat_barrier_new();
    int test_i;

    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    do {
        sync_data_t            *sd = 0;
        ste_uicc_pin_change_response_t* p;
        cat_barrier_set(bar);

        if (inter) {
          char old[10];
          char new[10];
          int pin_id;
          printf("OLD PIN: ");
          scanf("%4s", old);
          printf("NEW PIN: ");
          scanf("%4s", new);
          printf("Enter PIN id (1 = PIN1, 2 = PIN2)\n");
          scanf("%d", &pin_id);

          while (pin_id < 1 || pin_id > 2) {
              printf("Incorrect PIN id...\n");
              printf("Enter PIN id (1 = PIN1, 2 = PIN2)\n");
              scanf("%d", &pin_id);
          }

          test_i = ste_uicc_pin_change(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_CHANGE, old, new, (pin_id == 1 ? SIM_PIN_ID_PIN1 : SIM_PIN_ID_PIN2));
        }
        else
          test_i = ste_uicc_pin_change(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_CHANGE, "1111", "2345", SIM_PIN_ID_PIN1);
        if ( test_i )
            return  "ste_uicc_pin_change failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 1000);
        if ( test_i )
            return "ste_uicc_pin_change timed out";

        if( sd && sd->cause == STE_UICC_CAUSE_REQ_PIN_CHANGE ) {
            printf("#\tSIM change pin response received \n");
            p = (ste_uicc_pin_change_response_t*)(sd->data);
            printf("#\tSTATUS WORD 1 = %d hex:\n", (int)p->status_word.sw1);
            printf("#\tSTATUS WORD 2 = %d hex:\n", (int)p->status_word.sw2);
            printf("#\tUICC STATUS CODE = %d hex:\n", (int)p->uicc_status_code);
            printf("#\tUICC STATUS CODE DETAILS = %d hex:\n", (int)p->uicc_status_code_fail_details);

            sync_data_delete(sd);
            break;
         }

        sync_data_delete(sd);
    } while(1);


    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);

    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}



const char             *test_disablePIN()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t       pc;
    cat_barrier_t          *bar = cat_barrier_new();


    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);

    do {
        int test_i;
        sync_data_t            *sd = 0;
        ste_uicc_pin_disable_response_t* p;

        cat_barrier_set(bar);
        if (inter) {
          char code[10];
          printf("PIN: ");
          scanf("%4s", code);
          test_i = ste_uicc_pin_disable(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_DISABLE, code);
        } else
          test_i = ste_uicc_pin_disable(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_DISABLE, "1111");
          if ( test_i )
              return  "ste_uicc_pin_diasble failed.";

          test_i = cat_barrier_timedwait(bar, (void **) &sd, 1000);
          if ( test_i )
              return "ste_uicc_pin_disable timed out";

          if( sd && sd->cause == STE_UICC_CAUSE_REQ_PIN_DISABLE ) {

              p = (ste_uicc_pin_disable_response_t*)(sd->data);
              printf("#\tPIN disable response\n");
              printf("#\tSTATUS WORD 1 = %d hex:\n", (int)p->status_word.sw1);
              printf("#\tSTATUS WORD 2 = %d hex:\n", (int)p->status_word.sw2);
              printf("#\tUICC STATUS CODE = %d hex:\n", (int)p->uicc_status_code);
              printf("#\tUICC STATUS CODE DETAILS = %d hex:\n", (int)p->uicc_status_code_fail_details);
          }
          sync_data_delete(sd);
    } while(0);

    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);

    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}


const char             *test_enablePIN()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t       pc;
    cat_barrier_t          *bar = cat_barrier_new();


    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);

    do {
        int test_i;
        sync_data_t            *sd = 0;
        ste_uicc_pin_enable_response_t* p;

        cat_barrier_set(bar);
	if (inter) {
	  char code[10];
	  printf("PIN: ");
	  scanf("%4s", code);
	  test_i = ste_uicc_pin_enable(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_ENABLE, code);
	} else
	  test_i = ste_uicc_pin_enable(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_ENABLE, "1111");
        if ( test_i )
            return  "ste_uicc_pin_diasble failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 1000);
        if ( test_i )
            return "ste_uicc_pin_enable timed out";

        if( sd && sd->cause == STE_UICC_CAUSE_REQ_PIN_ENABLE ) {
            p = (ste_uicc_pin_enable_response_t*)(sd->data);
            printf("#\tPIN enable response\n");
            printf("#\tSTATUS WORD 1 = %d hex:\n", (int)p->status_word.sw1);
            printf("#\tSTATUS WORD 2 = %d hex:\n", (int)p->status_word.sw2);
            printf("#\tUICC STATUS CODE = %d hex:\n", (int)p->uicc_status_code);
            printf("#\tUICC STATUS CODE DETAILS = %d hex:\n", (int)p->uicc_status_code_fail_details);
        }
        sync_data_delete(sd);
    } while(0);


    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}


const char             *test_requestPIN_info()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t       pc;
    cat_barrier_t          *bar = cat_barrier_new();
    sim_pin_puk_id_t        pin_id;


    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);

    for ( pin_id = SIM_PIN_PIN1 ; pin_id <= SIM_PIN_PUK2 ; pin_id++ ) {
        int test_i;
        sync_data_t            *sd = 0;
        ste_uicc_pin_info_response_t* p;

        cat_barrier_set(bar);
        test_i = ste_uicc_pin_info(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_INFO,pin_id);
        if ( test_i )
            return  "ste_uicc_pin_info failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 1000);
        if ( test_i )
            return "ste_uicc_pin_info timed out";

        if( sd && sd->cause == STE_UICC_CAUSE_REQ_PIN_INFO ) {
            p = (ste_uicc_pin_info_response_t*)(sd->data);
            printf("#\tInfo for PIN_ID=%d\n",pin_id);
            printf("#\tUICC STATUS CODE = %d hex:\n", (int)p->uicc_status_code);
            printf("#\tUICC STATUS CODE DETAILS = %d hex:\n", (int)p->uicc_status_code_fail_details);
            printf("#\tResponse PIN status = %d\n", p->pin_status);
            printf("#\tResponse attempts remaining= %d\n", p->attempts_remaining);
        }

        sync_data_delete(sd);
    }


    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);

    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}


const char *test_readSIM_FILE_record()
{
    ste_sim_t *uicc;
    ste_sim_closure_t pc;
    cat_barrier_t *bar = cat_barrier_new();

    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";

    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);

    do {
        filerec_t   recInfo;
        int         test_i;
        sync_data_t *sd = 0;
        ste_uicc_sim_file_read_record_response_t* p;

        cat_barrier_set(bar);

        // Read first SMS record
        recInfo.file_id = 0x6F3C;
        recInfo.v.rec_id = 1;
        recInfo.length = 0;
        recInfo.pathptr = uicc_sim ? "3F007F10" : "3F007FFF";

        if ( inter && readFileSpec( &recInfo, FILE_REC_READ_FOR_RECORD ) == 0 ) break;

        test_i = ste_uicc_sim_file_read_record(uicc,
                TUICC_CT_STE_UICC_CAUSE_REQ_READ_SIM_FILE_RECORD, recInfo.file_id,
                recInfo.v.rec_id, recInfo.length, recInfo.pathptr);

        if (test_i)
            return "ste_uicc_sim_file_read_record failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 1000);
        if (test_i)
            return "ste_uicc_sim_file_read_record timed out";

        if (!sd || sd->cause != STE_UICC_CAUSE_REQ_READ_SIM_FILE_RECORD)
            return "ste_uicc_sim_file_read_record wrong response";

        p = (ste_uicc_sim_file_read_record_response_t*)(sd->data);
        printf("#\tREAD LENGTH = %d hex:\n", (int)p->length);
        printf("#\tSTATUS WORD 1 = %d hex:\n", (int)p->status_word.sw1);
        printf("#\tSTATUS WORD 2 = %d hex:\n", (int)p->status_word.sw2);
        printf("#\tUICC STATUS CODE = %d hex:\n", (int)p->uicc_status_code);
        printf("#\tUICC STATUS CODE DETAILS = %d hex:\n", (int)p->uicc_status_code_fail_details);
        printf("#\tREAD LENGTH = %d hex:\n", (int) p->length);

        if (p->data) {
            print_data(p->data,p->length);
            memcpy(simFileBuffer,p->data,p->length);
            free(p->data);
        }

        sync_data_delete(sd);
        if (!inter)
            break;
    } while (1);

    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);

    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}


const char             *test_readSIM_FILE_binary()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t      pc;
    cat_barrier_t           *bar = cat_barrier_new();


    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);

    do {
        filerec_t   recInfo;
        int         test_i;
        sync_data_t *sd = 0;
        ste_uicc_sim_file_read_binary_response_t* p;

        //  Correct values for test with MSL and MAL
        cat_barrier_set(bar);

        // Reads EF_PL file on USIM
        recInfo.file_id = 0x2F05;
        recInfo.v.offset = 0;
        recInfo.length = 0;
        recInfo.pathptr = "3F00";

        if ( inter && readFileSpec( &recInfo, FILE_REC_READ_FOR_BINARY ) == 0 ) break;

        test_i = ste_uicc_sim_file_read_binary(uicc,
                TUICC_CT_STE_UICC_CAUSE_REQ_READ_SIM_FILE_BINARY, recInfo.file_id,
                recInfo.v.offset, recInfo.length, recInfo.pathptr);

        if ( test_i )
            return  "ste_uicc_sim_file_read_binary failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 1000);
        if ( test_i )
            return "ste_uicc_sim_file_read_binary timed out";

        if( !sd || sd->cause != STE_UICC_CAUSE_REQ_READ_SIM_FILE_BINARY )
            return "ste_uicc_sim_file_read_binary wrong response";

        p = (ste_uicc_sim_file_read_binary_response_t*)(sd->data);
        printf("#\tREAD LENGTH = %d hex:\n", (int)p->length);
        printf("#\tSTATUS WORD 1 = %d hex:\n", (int)p->status_word.sw1);
        printf("#\tSTATUS WORD 2 = %d hex:\n", (int)p->status_word.sw2);
        printf("#\tUICC STATUS CODE = %d hex:\n", (int)p->uicc_status_code);
        printf("#\tUICC STATUS CODE DETAILS = %d hex:\n", (int)p->uicc_status_code_fail_details);

        if (p->data) {
            print_data(p->data,p->length);
            memcpy(simFileBuffer,p->data,p->length);
            free(p->data);
        }

        sync_data_delete(sd);
        if (!inter) break;
    } while(1);


    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}



const char             *test_simGETFILEFORMAT()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t      pc;
    cat_barrier_t           *bar = cat_barrier_new();


    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);

    do {
      int test_i, i;
        sync_data_t            *sd = 0;
        ste_uicc_sim_file_get_format_response_t* p;

        cat_barrier_set(bar);
	if (inter) {
	  int file_id;
	  char file_path[21], *file_path2;;
	  printf("FileID (HEX): ");
	  i = scanf("%x", (unsigned *)&file_id);
	  if (i == 0 || i == EOF || file_id == 0) break;
	  printf("File path (HEX): ");
	  i = scanf("%s", file_path);
	  if (i == 0 || i == EOF) break;
	  if (strcmp(file_path, "0") == 0)
	    file_path2 = NULL;
	  else if (strcmp(file_path, "1") == 0)
	    file_path2 = "";
	  else
	    file_path2 = file_path;
	  test_i = ste_uicc_sim_file_get_format
	    (uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_FILE_GET_FORMAT, file_id, file_path2);
	} else {
	  int file_id = 0x2F05;
	  const char *file_path = "3F00";
	  test_i = ste_uicc_sim_file_get_format
	    (uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_FILE_GET_FORMAT, file_id, file_path);
	}
        if ( test_i )
            return  "ste_uicc_sim_file_get_format failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 1000);
        if ( test_i )
            return "ste_uicc_sim_file_get_format timed out";

        if( !sd || sd->cause != STE_UICC_CAUSE_REQ_SIM_FILE_GET_FORMAT )
            return "ste_uicc_sim_file_get_format_response wrong response";
	p = (ste_uicc_sim_file_get_format_response_t*)(sd->data);
        printf("SIM_FILE_GET_FORMAT "
            "status_word 1 = %d\n"
            "status_word 2 = %d\n"
            "uicc status code = %d\n"
            "uicc status code details = %d\n"
            "file_type=%s\n"
            "file_size=%d\n"
            "record_len=%d\n"
            "num_records=%d\n",
            p->status_word.sw1,
            p->status_word.sw2,
            p->uicc_status_code,
            p->uicc_status_code_fail_details,
            file_types[p->file_type],
            p->file_size,
            p->record_len,
            p->num_records);

        sync_data_delete(sd);
	if (!inter) break;
    } while(1);
    sleep(1);
    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);
    sleep(1);
    return 0;
}


const char             *test_simGetFormatModemError()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t      pc;
    cat_barrier_t          *bar = cat_barrier_new();


    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);

    do {
        int test_i;
        sync_data_t            *sd = 0;
        ste_uicc_sim_file_get_format_response_t* p;

        cat_barrier_set(bar);
        {
	  int file_id = 0xDEAD;
	  test_i = ste_uicc_sim_file_get_format
	    (uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_FILE_GET_FORMAT, file_id, NULL);
	}
        if ( test_i ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            cat_barrier_delete(bar);
            return  "ste_uicc_sim_file_get_format failed.";
        }

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 1000);
        if ( test_i ) {
            TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            cat_barrier_delete(bar);
            return "ste_uicc_sim_file_get_format timed out";
        }

        if( !sd || sd->cause != STE_UICC_CAUSE_REQ_SIM_FILE_GET_FORMAT ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            cat_barrier_delete(bar);
            sync_data_delete(sd);
            return "ste_uicc_sim_file_get_format_response wrong response";
        }
	p = (ste_uicc_sim_file_get_format_response_t*)(sd->data);
        if (p) {
            printf("SIM_FILE_GET_FORMAT UICC STATUS CODE = %d\n"
                   "UICC STATUS CODE DETAILS %d\n"
                   "STATUS WORD 1%d\n"
                   "STATUS WORD 2 %d\n"
                   "file_type=%s\n"
                   "file_size=%d\n"
                   "record_len=%d\n"
                   "num_records=%d\n",
                   p->uicc_status_code,
                   p->uicc_status_code_fail_details,
                   p->status_word.sw1,
                   p->status_word.sw2,
                   file_types[p->file_type],
                   p->file_size,
                   p->record_len,
                   p->num_records);
        }
        else {
            printf("SIM_FILE_GET_FORMAT STATUS = -1");
        }
        sync_data_delete(sd);
	if (!inter) break;
    } while(1);
    sleep(1);
    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);
    sleep(1);
    return 0;
}

const char             *test_getFILEINFORMATION()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t       pc;
    cat_barrier_t          *bar = cat_barrier_new();


    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);

    do {
      int test_i, i;
      sync_data_t *sd = 0;
      ste_uicc_get_file_information_response_t* p;
      int file_id = 0x6F07; // EF_IMSI
      char file_path[21] = "3F007FFF"; // USIM;
      ste_uicc_sim_get_file_info_type_t type = STE_UICC_SIM_GET_FILE_INFO_TYPE_EF;

      cat_barrier_set(bar);
      if (inter) {
        printf("FileID (HEX): ");
        i = scanf("%x", (unsigned *) &file_id);
        if (i == 0 || i == EOF)
            break;
        printf("File path (HEX): "); // Make sure it is a hex string
        i = scanf("%s", file_path);
        if (i == 0 || i == EOF)
            break;
        printf("Information type 0=EF, 1=DF: ");
        i = scanf("%i", (int*) &type);
        if (i == 0 || i == EOF)
            break;
      }

      test_i = ste_uicc_sim_get_file_information(uicc,
                    TUICC_CT_STE_UICC_CAUSE_REQ_GET_FILE_INFORMATION, file_id,
                    file_path, type);

      if ( test_i )
        return  "ste_uicc_get_file_information failed.";

      test_i = cat_barrier_timedwait(bar, (void **) &sd, 1000);
      if ( test_i )
        return "ste_uicc_get_file_information timed out";

      if( sd && sd->cause == STE_UICC_CAUSE_REQ_GET_FILE_INFORMATION ) {
          p = (ste_uicc_get_file_information_response_t*)(sd->data);
          printf("#\tGET_FILE_INFORMATION length=%d\n", p->length);
          printf("#\tSTATUS WORD 1 = %d hex:\n", (int)p->status_word.sw1);
          printf("#\tSTATUS WORD 2 = %d hex:\n", (int)p->status_word.sw2);
          printf("#\tUICC STATUS CODE = %d hex:\n", (int)p->uicc_status_code);
          printf("#\tUICC STATUS CODE DETAILS = %d hex:\n", (int)p->uicc_status_code_fail_details);

          if (p->fcp) {
            print_data(p->fcp,p->length);
            free(p->fcp);
          }
       }

      sync_data_delete(sd);
      if (!inter) break;
    } while(1);
    sleep(1);
    // Disconnect
    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 1000);

    ste_sim_delete(uicc, TUICC_CT_NOT_USED);

    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}

const char             *test_getSIMSTATE_sync( void )
{
    ste_sim_t              *sim;
    ste_sim_closure_t       pc;
    int                     i;
    ste_uicc_get_sim_state_response_t sim_state;

    pc.func = the_callback_sync;
    pc.user_data = 0;

    sim = ste_sim_new_st(&pc);
    if (!sim)
        return "ste_sim_new failed.";

    i = ste_sim_connect( sim, TUICC_CT_STE_SIM_CAUSE_CONNECT );
    if ( i ) {
        return "getSIMSTATE_sync: connect failed.";
    }

    i = ste_uicc_sim_get_state_sync(sim, TUICC_CT_STE_UICC_CAUSE_REQ_GET_SIM_STATE, &sim_state);

    if ( i )
        return  "getSIMSTATE_sync: ste_uicc_sim_get_state_sync failed";

    printf("#\tGET_SIM_STATE RETURN");
    printf("#\tGET_SIM_STATE_ERROR_CAUSE = %d (success=0)\n", sim_state.error_cause);
    printf("#\tGET_SIM_STATE_SYNC, SIM state = %d\n", sim_state.state);

    i = ste_sim_disconnect( sim, TUICC_CT_STE_SIM_CAUSE_DISCONNECT );
    if ( i ) {
        ste_sim_delete(sim, TUICC_CT_NOT_USED);
        return "getSIMSTATE_sync disconnect failed";
    }

    ste_sim_delete(sim, TUICC_CT_NOT_USED);

    sleep(1);

    return 0;
}

const char             *test_getSIMSTATE()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t      pc;
    int                     i;
    cat_barrier_t           *bar = cat_barrier_new();


    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);

    do {
        int test_i;
        sync_data_t            *sd = 0;
        ste_uicc_get_sim_state_response_t* p;

        cat_barrier_set(bar);
        test_i = ste_uicc_sim_get_state(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_GET_SIM_STATE);
        if ( test_i )
            return  "ste_uicc_sim_get_state.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 1000);
        if ( test_i )
            return "ste_uicc_sim_get_state timed out";

        if( sd && sd->cause == STE_UICC_CAUSE_REQ_GET_SIM_STATE ) {
            printf("#\tSIM file get sim state information response received \n");
            p = (ste_uicc_get_sim_state_response_t*)(sd->data);
            printf("#\tGET_SIM_STATE STATUS = %d\n", p->error_cause);
            printf("#\tGET_SIM_STATE value = %d\n", p->state);

            sync_data_delete(sd);
            break;
         }

        sync_data_delete(sd);
    } while(1);


    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);


    i = ste_sim_disconnect(uicc, TUICC_CT_STE_SIM_CAUSE_DISCONNECT);
    if (!i)
        return "disconnect did not fail when disconnected.";

    ste_sim_delete(uicc, TUICC_CT_NOT_USED);

    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}

const char             *test_updateSIM_FILE_record()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t      pc;
    cat_barrier_t           *bar = cat_barrier_new();


    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_uicc_new failed.";


    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);

    do {
        filerec_t   recInfo;
        int         test_i;
        sync_data_t *sd = 0;
        ste_uicc_update_sim_file_record_response_t *p;

        cat_barrier_set(bar);
        // Update first SMS record by default
        recInfo.file_id = 0x6F3C;
        recInfo.v.rec_id = 1;
        recInfo.length = 176;
        recInfo.pathptr = uicc_sim ? "3F007F10" : "3F007FFF";

        memset(simFileBuffer, 0xFF, sizeof(simFileBuffer));
        simFileBuffer[0] = 0;

        if ( inter ) {
            int r = readFileSpec( &recInfo, FILE_REC_READ_FOR_RECORD );
            if ( r == 0 ) break;
            editMemory(simFileBuffer,recInfo.length);
        } else {
            strcpy((char *) simFileBuffer + 1, "Team MH - async write");
        }

        test_i = ste_uicc_sim_file_update_record(uicc,
                        TUICC_CT_STE_UICC_CAUSE_REQ_UPDATE_SIM_FILE_RECORD,
                        recInfo.file_id, recInfo.v.rec_id, recInfo.length,
                        recInfo.pathptr, simFileBuffer);

        if ( test_i )
            return  "ste_uicc_update_sim_file_record failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 1000);
        if ( test_i )
            return "ste_uicc_update_sim_file_record timed out";

        if( sd && sd->cause == STE_UICC_CAUSE_REQ_UPDATE_SIM_FILE_RECORD ) {
            printf("#\tSIM file update record response received \n");
            p = (ste_uicc_update_sim_file_record_response_t*)(sd->data);
            printf("#\tSTATUS WORD 1 = %d hex:\n", (int)p->status_word.sw1);
            printf("#\tSTATUS WORD 2 = %d hex:\n", (int)p->status_word.sw2);
            printf("#\tUICC STATUS CODE = %d hex:\n", (int)p->uicc_status_code);
            printf("#\tUICC STATUS CODE DETAILS = %d hex:\n", (int)p->uicc_status_code_fail_details);
            sync_data_delete(sd);
            break;
         }

         sync_data_delete(sd);
    } while(1);


    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);

    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}


const char             *test_updateSIM_FILE_binary()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t      pc;
    cat_barrier_t           *bar = cat_barrier_new();


    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_uicc_new failed.";


    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);

    do {
        filerec_t   recInfo;
        int         test_i;
        sync_data_t *sd = 0;
        ste_uicc_update_sim_file_binary_response_t *prsp_update_binary;

        cat_barrier_set(bar);

        // Update EF_LP  by default
        recInfo.file_id = 0x2F05;
        recInfo.v.offset = 2;
        recInfo.length = 2;
        recInfo.pathptr = uicc_sim ? "3F00" : "3F00";

        memset(simFileBuffer, 0xFF, sizeof(simFileBuffer));
        simFileBuffer[0] = 0;

        if ( inter ) {
            int r = readFileSpec( &recInfo, FILE_REC_READ_FOR_BINARY );
            if ( r == 0 ) break;
            editMemory(simFileBuffer,recInfo.length);
        } else {
            simFileBuffer[0] = 'm';
            simFileBuffer[1] = 'h';
        }

        test_i = ste_uicc_sim_file_update_binary(uicc,
                TUICC_CT_STE_UICC_CAUSE_REQ_UPDATE_SIM_FILE_BINARY,
                recInfo.file_id, recInfo.v.offset, recInfo.length,
                recInfo.pathptr, simFileBuffer);

        if ( test_i )
            return  "ste_uicc_update_sim_file_binary failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 2000);
        if ( test_i )
            return "ste_uicc_update_sim_file_binary timed out";

        if( sd && sd->cause == STE_UICC_CAUSE_REQ_UPDATE_SIM_FILE_BINARY ) {
            printf("#\tSIM file update binary response received \n");
            prsp_update_binary = (ste_uicc_update_sim_file_binary_response_t*)(sd->data);
            printf("#\tSTATUS WORD 1 = %d hex:\n", (int)prsp_update_binary->status_word.sw1);
            printf("#\tSTATUS WORD 2 = %d hex:\n", (int)prsp_update_binary->status_word.sw2);
            printf("#\tUICC STATUS CODE = %d hex:\n", (int)prsp_update_binary->uicc_status_code);
            printf("#\tUICC STATUS CODE DETAILS = %d hex:\n", (int)prsp_update_binary->uicc_status_code_fail_details);
            sync_data_delete(sd);
            break;
         }

         sync_data_delete(sd);
    } while(1);


    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}

const char *test_get_app_info() {
    ste_sim_t *uicc;
    ste_sim_closure_t pc;
    cat_barrier_t *bar = cat_barrier_new();

    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_uicc_new failed.";

    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);

    do {
        int test_i;
        sync_data_t            *sd = 0;
        ste_uicc_sim_app_info_response_t *p;

        cat_barrier_set(bar);
        test_i = ste_uicc_get_app_info(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_APP_INFO );
        if (test_i) return  "ste_uicc_get_app_info failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 2000);
        if (test_i)
            return "ste_uicc_get_app_info timed out";
        if(sd && sd->cause == STE_UICC_CAUSE_REQ_GET_APP_INFO) {
         printf("#\tGet app info response received \n");
         p  = (ste_uicc_sim_app_info_response_t*)(sd->data);
         printf("#\tSTATUS = %d\n", p->status);
         switch (p->app_type) {
         case SIM_APP_UNKNOWN:
           printf("#\tType is unknown\n");
           break;
         case SIM_APP_GSM:
           printf("#\tType is ICC/GSM\n");
           break;
         case SIM_APP_USIM:
           printf("#\tType is UICC/USIM\n");
           break;
         case SIM_APP_ISIM:
           printf("#\tType is UICC/ISIM\n");
           break;
         default:
           printf("#\tType is corrupt!\n");
           break;
         }
         break;
       }
       sync_data_delete(sd);
    } while (1);

    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}

const char *test_card_status() {
  ste_sim_t *uicc;
  ste_sim_closure_t pc;
  cat_barrier_t *bar = cat_barrier_new();

  pc.func = the_callback;
  pc.user_data = bar;

  uicc = ste_sim_new(&pc);
  if (!uicc)
    return "ste_uicc_new failed.";

  TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

  sleep(1);

  do {
    int test_i;
    sync_data_t            *sd = 0;
    ste_uicc_sim_card_status_response_t *p;

    cat_barrier_set(bar);
    test_i = ste_uicc_card_status(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_CARD_STATUS);
    if (test_i) return  "ste_uicc_card_status failed.";

    test_i = cat_barrier_timedwait(bar, (void **) &sd, 2000);
    if (test_i)
      return "ste_uicc_card_status timed out";
    if(sd && sd->cause == STE_UICC_CAUSE_REQ_CARD_STATUS) {
      printf("#\tGet card status response received \n");
      p  = (ste_uicc_sim_card_status_response_t*)(sd->data);
      printf("#\tSTATUS = "); print_status(p->status);
      printf("#\tnum_apps = %d\n", p->num_apps);
      printf("#\tcard_type = %d\n", p->card_type);
      printf("#\tcard_state = %d\n", p->card_state);
      printf("#\tupin_state = %d\n", p->upin_state);
    }
    sync_data_delete(sd);
  } while (0);

  sleep(1);

  TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
  ste_sim_delete(uicc, TUICC_CT_NOT_USED);
  cat_barrier_delete(bar);

  sleep(1);

  return 0;
}

const char *test_app_status() {
  ste_sim_t *uicc;
  ste_sim_closure_t pc;
  cat_barrier_t *bar = cat_barrier_new();

  pc.func = the_callback;
  pc.user_data = bar;

  uicc = ste_sim_new(&pc);
  if (!uicc)
    return "ste_uicc_new failed.";

  TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

  sleep(1);

  do {
    int test_i;
    sync_data_t            *sd = 0;
    ste_uicc_sim_app_status_response_t *p;
    int app_index = 0;

    cat_barrier_set(bar);
    if (inter) {
      printf("App index: ");
      scanf("%d", &app_index);
    }
    test_i = ste_uicc_app_status(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_APP_STATUS, app_index );
    if (test_i) return "ste_uicc_app_status failed.";

    test_i = cat_barrier_timedwait(bar, (void **) &sd, 2000);
    if (test_i)
      return "ste_uicc_app_status timed out";
    if(sd && sd->cause == STE_UICC_CAUSE_REQ_APP_STATUS) {
      printf("#\tGet app status response received \n");
      p  = (ste_uicc_sim_app_status_response_t *)(sd->data);
      printf("#\tSTATUS = "); print_status(p->status);
      if (p->status == STE_UICC_STATUS_CODE_OK) {
        printf("\n#\tapp_type = %d\n", p->app_type);
        printf("#\tapp_state = %d\n", p->app_state);
        printf("#\taid_len = %zd\n", p->aid_len);
        print_data(p->aid, p->aid_len);
        printf("#\tlabel = %s\n", p->label);
        printf("#\tmark = %d\n", p->mark);
        printf("#\tpin_mode = %d\n", p->pin_mode);
        printf("#\tpin_state = %d\n", p->pin_state);
        printf("#\tpin2_state = %d", p->pin2_state);
        free(p->label);
      }
      printf("\n");
    }
    sync_data_delete(sd);
  } while (0);

  sleep(1);

  TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
  ste_sim_delete(uicc, TUICC_CT_NOT_USED);
  cat_barrier_delete(bar);

  sleep(1);

  return 0;
}

static uint16_t current_open_channel;
struct apdu_info {
    const char *command_name;
    uint8_t    *command;
    size_t      command_size;
};
static uint8_t usim_select_df[] = {
        0x00, 0xA4, 0x08, 0x0C,         // Select
        0x04, 0x3F, 0x00, 0x7F, 0xFF    // USIM Telecom DF
};
static uint8_t usim_status[] = {
        0x00, 0xF2, 0x00, 0x00,         // Status
        0x00
};
static uint8_t  usim_select_lp[] = {
        0x00, 0xA4, 0x08, 0x0C,         // Select
        0x06, 0x3F, 0x00, 0x7F, 0xFF, 0x6F, 0x07    // EF(LP)
};
static uint8_t  usim_read_binary_lp[] = {
        0x00, 0xB0, 0x00, 0x00,         // Read binary
        0x00
};
static uint8_t  usim_update_binary_lp[] = {
        0x00, 0xD6, 0x00, 0x02,         // Read binary
        0x02,
        0x53, 0x76                      // "Sv"
};
static uint8_t  usim_select_ecc[] = {
        0x00, 0xA4, 0x08, 0x0C,         // Select
        0x06, 0x3F, 0x00, 0x7F, 0xFF, 0x6F, 0xB7    // EF(ECC)
};
static uint8_t  usim_read_record_ecc[] = {
        0x00, 0xB2, 0x01, 0x04,         // Read record
        0x00
};
static uint8_t  usim_select_sms[] = {
        0x00, 0xA4, 0x08, 0x0C,         // Select
        0x06, 0x3F, 0x00, 0x7F, 0xFF, 0x6F, 0x3C    // EF(SMS)
};
static uint8_t  usim_read_record_sms[] = {
        0x00, 0xB2, 0x01, 0x04,
        0x00
};
static uint8_t  usim_update_record_sms[] = {
        0x00, 0xDC, 0x01, 0x04,         // Update record
        0xB0,
        0x00, 0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x61, 0x20, 0x74, 0x65, 0x73, 0x74, 0x20, // .This is a test
        0x6d, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // message
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
static uint8_t usim_search_record_sms[] = {
        0x00, 0xA2, 0x00, 0x06,         // Search record    (P2=0x06, enhanced search, ETSI TS 102 221
        0x06,
            0x04,                       // Start at record number in P1, offset into record is in B2
            0x01,                       // Offset into record is 1 byte
            0x54, 0x68, 0x69, 0x73,     // "This" is the search string
        0x00
};


static uint8_t usim_select_autheticate[] = {
        0x00, 0xA4, 0x08, 0x0C,         // Select
        0x04, 0x3F, 0x00, 0x7F, 0xFF    // USIM Telecom DF
};
static uint8_t usim_authenticate[] = {
        0x00, 0x88, 0x00, 0x81,         // Authenticate
        0x22,
        0x10, 0x3C, 0x71, 0x2F, 0xC9, 0xDF, 0x36, 0xD1,
        0xED, 0x72, 0xDE, 0x9D, 0x02, 0x01, 0xED, 0xE3,
        0xF1, 0x10, 0xFA, 0x9C, 0x7B, 0xDE, 0x86, 0x29,
        0x00, 0x00, 0x3C, 0x67, 0x15, 0x93, 0x87, 0xB0, 0xB7, 0x9A,
        0x00
};
static uint8_t usim_select_challenge[] = {
        0x00, 0xA4, 0x08, 0x0C,         // Select
        0x04, 0x3F, 0x00, 0x7F, 0xFF    // USIM Telecom DF
};
static uint8_t usim_get_challenge[] = {
        0x00, 0x84, 0x00, 0x00,         // Get Challenge data
        0x20
};
static uint8_t usim_select_get_response[] = {
        0x00, 0xc0, 0x00, 0x00, 0x00    /* Get response data, last byte is size */
};

#define ASIZE(x)            sizeof(x)/sizeof(x[0])
#define CMD(x)              { #x, x, ASIZE(x) }
struct apdu_info usim_cmds[] = {
        CMD(usim_select_df),        CMD(usim_status),
        CMD(usim_select_lp),        CMD(usim_read_binary_lp),
        CMD(usim_select_lp),        CMD(usim_update_binary_lp),
        CMD(usim_select_lp),        CMD(usim_read_binary_lp),
        CMD(usim_select_ecc),       CMD(usim_read_record_ecc),
        CMD(usim_select_sms),       CMD(usim_read_record_sms),
        CMD(usim_select_sms),       CMD(usim_update_record_sms),
        CMD(usim_select_sms),       CMD(usim_read_record_sms),
        CMD(usim_select_sms),       CMD(usim_search_record_sms),
        CMD(usim_select_autheticate), CMD(usim_authenticate),
        CMD(usim_select_challenge), CMD(usim_get_challenge),
};

#define NUM_SELECT_CMDS 11

struct apdu_info select_cmds[] = {
        CMD(usim_select_df),
        CMD(usim_select_lp),
        CMD(usim_select_lp),
        CMD(usim_select_lp),
        CMD(usim_select_ecc),
        CMD(usim_select_sms),
        CMD(usim_select_sms),
        CMD(usim_select_sms),
        CMD(usim_select_sms),
        CMD(usim_select_autheticate),
        CMD(usim_select_challenge)
};

static size_t next_usim_buffer( size_t *index, uint8_t *buff, size_t size ) {
    size_t  result = 0;
    if ( *index >= ASIZE(usim_cmds) ) {
        result = 0;
    } else {
        size_t  i;
        printf("# %s\n", usim_cmds[*index].command_name );
        printf("# AT+CSIM=%zu,\"",usim_cmds[*index].command_size*2);
        for ( i = 0 ; i < usim_cmds[*index].command_size ; i++ ) {
            printf("%02hhX",usim_cmds[*index].command[i]);
        }
        printf("\"\n");
        if ( usim_cmds[*index].command_size <= size ) {
            memcpy( buff, usim_cmds[*index].command, usim_cmds[*index].command_size );
            result = usim_cmds[*index].command_size;
        } else {
            printf("# Buffer of %zu bytes too small for data of %zu bytes\n",
                    size, usim_cmds[*index].command_size );
            result = 0;
        }
        (*index)++;
    }
    return result;
}

static void print_cmd_result(sync_data_t *sd, int cause) {
    if(sd && sd->cause == cause) {
        ste_uicc_sim_channel_send_response_t *p =
                (ste_uicc_sim_channel_send_response_t *)(sd->data);

        printf("#\tSIM channel send response received \n");
        printf("#\tSTATUS = ");
        print_status(p->uicc_status_code);

        printf("\n");
        printf("#\tSTATUS Details=%d\n",p->uicc_status_code_fail_details);
        printf("#\tsw1=0x%02x, sw2=0x%02x\n", p->status_word.sw1, p->status_word.sw2);

        if (p->uicc_status_code == STE_UICC_STATUS_CODE_OK) {
            print_data(p->apdu, p->apdu_len);
            free(p->apdu);
        }
        printf("\n");
    }
    sync_data_delete(sd);
}

const char *test_sim_select()
{
    ste_sim_t *uicc;
    cat_barrier_t *bar = cat_barrier_new();
    ste_sim_closure_t pc;
    int test_i, i;

    if ( inter ) {
        printf("Interactive mode not supported for this test\n");
    }
    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);

    if ( !uicc ) {
        return "ste_uicc_new failed.";
    }

    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    for (i = 0; i < NUM_SELECT_CMDS; i++) {
        uint8_t cmd[255];
        size_t cmd_size = 0;
        sync_data_t *sd = 0;
        size_t rsp_size = 0;

        cmd_size = select_cmds[i].command_size;
        memmove(cmd, select_cmds[i].command, cmd_size);

        cat_barrier_set(bar);

        test_i = ste_uicc_sim_channel_send(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_CHANNEL_SEND,
                                           0, cmd_size, cmd );
        if ( test_i ) {
            return "Send channel failed";
        }

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 2000);
        if ( sd && sd->data ) {
            ste_uicc_sim_channel_send_response_t *p =
                    (ste_uicc_sim_channel_send_response_t *)(sd->data);
            rsp_size = p->status_word.sw2;
        }
        print_cmd_result(sd, STE_UICC_CAUSE_REQ_SIM_CHANNEL_SEND);
        sd = 0;

        sleep(1);

        if ( rsp_size > 0 && (cmd[3] == 0x0C || cmd[3] == 0x00) ) {
            /* Get response */
            cmd_size = 5;
            memmove(cmd, usim_select_get_response, cmd_size);
            cmd[4] = rsp_size; /* Set size */
            cat_barrier_set(bar);

            test_i = ste_uicc_sim_channel_send(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_CHANNEL_SEND,
                                               0, cmd_size, cmd );
            if ( test_i ) {
                return "Send channel failed";
            }

            test_i = cat_barrier_timedwait(bar, (void **) &sd, 2000);

            print_cmd_result(sd, STE_UICC_CAUSE_REQ_SIM_CHANNEL_SEND);
            sd = 0;
        }

        cmd_size = select_cmds[i].command_size;
        memmove(cmd, select_cmds[i].command, cmd_size);
        cmd[3] = 0x04; /* FCP template reply */
        cat_barrier_set(bar);

        test_i = ste_uicc_sim_channel_send(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_CHANNEL_SEND,
                                           0, cmd_size, cmd );
        if ( test_i ) {
            return "Send channel failed";
        }

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 2000);

        print_cmd_result(sd, STE_UICC_CAUSE_REQ_SIM_CHANNEL_SEND);

        sleep(1);

    }
    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}

const char *test_sim_log_level()
{
    FILE *fp = NULL;
    fp = fopen(UTIL_LOG_FILE, "w");

    printf("test_sim_log_level");
    if (!fp) {
        printf("Failed to open file");
        return "sim log level failed";
    }

    fclose(fp);

    return 0;

}

const char * test_sim_utils1()
{
    ste_sim_text_t  * source_text_p;
    uint8_t          dest[100];;
    uint16_t          max_len;
    uint16_t          actual_len_p;
    char * text = "012345*#CDEF";
    char * text2 = "012345*#CDE";

    printf("test_sim_utils1");

    if (sim_convert_UCS2_to_BCD(NULL,0,NULL,NULL) != -1)
        return "test_sim_utils1 failed";

    if (utility_UCS2_to_BCD(NULL,NULL,0,NULL) != -1)
        return "test_sim_utils1 failed";

    source_text_p = (ste_sim_text_t * ) malloc (sizeof (ste_sim_text_t));

    if (!source_text_p)
        return "test_sim_utils1 failed";

    source_text_p->text_p = NULL;

    if (sim_convert_UCS2_to_BCD(source_text_p,10,&actual_len_p,dest) != -1) {
        free(source_text_p);
        return "test_sim_utils1 failed";
    }

    source_text_p->no_of_characters = strlen(text);
    source_text_p->text_p = text;
    source_text_p->text_coding = STE_SIM_UCS2;

    if (sim_convert_UCS2_to_BCD(source_text_p,12,&actual_len_p,dest) != 0) {
        free(source_text_p);
        return "test_sim_utils1 failed";
    }

    source_text_p->no_of_characters = 0;
    if (sim_convert_UCS2_to_BCD(source_text_p,12,&actual_len_p,dest) != 0) {
        if (actual_len_p != 0) {
            free(source_text_p);
            return "test_sim_utils1 failed";
        }
    }

    source_text_p->no_of_characters = strlen(text2);
    source_text_p->text_p = text2;
    source_text_p->text_coding = STE_SIM_UCS2;

    if (sim_convert_UCS2_to_BCD(source_text_p,12,&actual_len_p,dest) != 0) {
        free(source_text_p);
        return "test_sim_utils1 failed";
    }

    source_text_p->no_of_characters = 50;
    if (utility_UCS2_to_BCD(source_text_p,dest,12,&actual_len_p) != -1) {
        free(source_text_p);
        return "test_sim_utils1 failed";
    }

    source_text_p->text_coding = STE_SIM_CODING_UNKNOWN;
    if (utility_UCS2_to_BCD(source_text_p,dest,12,&actual_len_p) != -1) {
        free(source_text_p);
        return "test_sim_utils1 failed";
    }

    free(source_text_p);
    return 0;

}

const char * test_sim_utils2()
{
    ste_sim_text_t  * source_text_p;
    uint8_t          dest[100];
    uint16_t         dest2[100];
    uint16_t          max_len;
    uint16_t          actual_len_p;
    char * text = "012345*#CDEF";
    char * text2 = "012345*#CDE";

    printf("test_sim_utils2");

    if (sim_convert_ASCII8_to_BCD(NULL,0,NULL,NULL) != -1)
        return "test_sim_utils2 failed";

    if (utility_ASCII8_to_UCS2(NULL,NULL,0,NULL) != -1)
        return "test_sim_utils2 failed";

    source_text_p = (ste_sim_text_t * ) malloc (sizeof (ste_sim_text_t));

    if (!source_text_p)
        return "test_sim_utils2 failed";

    source_text_p->text_p = NULL;

    if (sim_convert_ASCII8_to_BCD(source_text_p,10,&actual_len_p,dest) != -1) {
        free(source_text_p);
        return "test_sim_utils2 failed";
    }

    source_text_p->no_of_characters = strlen(text);
    source_text_p->text_p = text;
    source_text_p->text_coding = STE_SIM_ASCII8;

    if (sim_convert_ASCII8_to_BCD(source_text_p,12,&actual_len_p,dest) != 0) {
        free(source_text_p);
        return "test_sim_utils2 failed";
    }

    source_text_p->no_of_characters = 0;
    if (sim_convert_ASCII8_to_BCD(source_text_p,12,&actual_len_p,dest) != 0) {
        if (actual_len_p != 0) {
            free(source_text_p);
            return "test_sim_utils2 failed";
        }
    }

    source_text_p->no_of_characters = strlen(text2);
    source_text_p->text_p = text2;
    source_text_p->text_coding = STE_SIM_ASCII8;

    if (sim_convert_ASCII8_to_BCD(source_text_p,12,&actual_len_p,dest) != 0) {
        free(source_text_p);
        return "test_sim_utils2 failed";
    }

    source_text_p->text_coding = STE_SIM_CODING_UNKNOWN;
    if (utility_ASCII8_to_UCS2(source_text_p,dest2,12,&actual_len_p) != -1) {
        free(source_text_p);
        return "test_sim_utils2 failed";
    }

    free(source_text_p);
    return 0;

}

const char *test_sim_select_relative()
{
    ste_sim_t *uicc;
    cat_barrier_t *bar = cat_barrier_new();
    ste_sim_closure_t pc;
    int test_i, i;
    int num_iter = 0;
    uint8_t usim_select_mf_implicit[] = {
              0x00, 0xA4, 0x00, 0x0C,0x00     // Select
    };
    uint8_t usim_select_df_child_of_current_df[] = {
              0x00, 0xA4, 0x01, 0x0C,         // Select
              0x02, 0x7F, 0xFF                // USIM DF Telecom
    };
    uint8_t usim_select_ef_child_of_current_df[] = {
              0x00, 0xA4, 0x09, 0x0C,         // Select
              0x02, 0x6F, 0x07                // USIM MF
    };
    uint8_t usim_select_df_parent_of_current_df[] = {
              0x00, 0xA4, 0x03, 0x0C,         // Select
              0x02, 0x6F, 0x07                // USIM MF
    };
    uint8_t sim_select_mf[] = {
              0x00, 0xA4, 0x00, 0x00,0x02, 0x3f, 0x00     // Select MF
    };
    uint8_t sim_select_df[] = {
              0x00, 0xA4, 0x00, 0x00,0x02, 0x7f, 0x00     // Select DF from MF
    };
    uint8_t sim_select_ef[] = {
              0x00, 0xA4, 0x00, 0x00,0x02, 0x6f, 0x07     // Select child EF
    };
    struct apdu_info usim_cmds_relative[] = {
        CMD(usim_select_mf_implicit),
        CMD(usim_select_df_child_of_current_df),
        CMD(usim_select_ef_child_of_current_df),
        CMD(usim_select_df_parent_of_current_df)
    };

    struct apdu_info sim_cmds_relative[] = {
        CMD(sim_select_mf),
        CMD(sim_select_df),
        CMD(sim_select_ef)
    };

    if ( inter ) {
        printf("Interactive mode not supported for this test\n");
    }
    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);

    if ( !uicc ) {
        return "ste_uicc_new failed.";
    }

    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    if ( uicc_sim ) {
        num_iter = 3;
    } else {
        num_iter = 4;
    }

    for (i = 0; i < num_iter; i++) {
         uint8_t cmd[255];
        size_t cmd_size = 0;
        sync_data_t *sd = 0;

        if ( uicc_sim ) {
            cmd_size = sim_cmds_relative[i].command_size;
            memcpy(cmd, sim_cmds_relative[i].command, cmd_size);
        } else {
            cmd_size = usim_cmds_relative[i].command_size;
            memcpy(cmd, usim_cmds_relative[i].command, cmd_size);
        }

        cat_barrier_set(bar);

        test_i = ste_uicc_sim_channel_send(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_CHANNEL_SEND,
                                           0, cmd_size, cmd );
        if ( test_i ) {
            return "Send channel failed";
        }

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 2000);
        print_cmd_result(sd, STE_UICC_CAUSE_REQ_SIM_CHANNEL_SEND);
        sd = 0;

        sleep(1);

    }
    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}


const char *test_sim_channel_send_fail_01()
{
    uint8_t cmd_edit[256] = { 0 };
    char    input[100];
    uint8_t cmd1[] = {
        0x00,   // Class (to be changed to include logical channel)
        0xA4,   // INStruction = SELECT
        0x00,   // P1 = Select DF, EF or MF by file id
        0x04,   // P2 = Return FCP template, for the first/only occurrence
        0x02,   // Lc =
        0x6F,   // 6F07=File
        0x07,
        0x00,   // Le = Max length of result is 256
    };
    uint8_t cmd2[] = {
        0x00,   // Class (to be changed to include logical channel)
        0xB0,   // INStruction = Read Binary
        0x00,   // P1 = Offset to the first byte to read (ie, zero)
        0x00,   // P2 = Ditto
        //0x00,   // Lc = 0 (no input params)
        0x09,   // Le = 9 bytes
    };
    struct {
        uint8_t     *apdu;
        size_t       len;
    } apdus[] = {
        // NOTE: This test sequence will not currently work when the
        // target application on the USIM is the telecom application.
        // Select a non-telecom application first, preferably an "echo" application.
        { cmd1, sizeof(cmd1) },
        //{ cmd2, sizeof(cmd2) },
    };
    ste_sim_t *uicc;
    ste_sim_closure_t pc;
    cat_barrier_t *bar = cat_barrier_new();
    size_t n;
    size_t current = 0;

    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_uicc_new failed.";

    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);
    printf("#\tSIM channel send: current channel=%hu\n",current_open_channel);
    if ( inter || current_open_channel == 0 ) {
        printf("Enter current channel ID -> ");
        fflush(stdout);
        if ( fgets(input, sizeof input, stdin) == NULL ) return 0;
        if ( sscanf(input, "%hu", &current_open_channel) != 1 ) return 0;
    }

    sleep(1);
    for ( n = 0 ; n < sizeof(apdus)/sizeof(apdus[0]) || inter ; n++ ) {
        int test_i;
        sync_data_t            *sd = 0;
        ste_uicc_sim_channel_send_response_t *p;

        cat_barrier_set(bar);

        if (inter) {
            size_t  edit_size = next_usim_buffer(&current,cmd_edit,sizeof(cmd_edit));
            if ( edit_size == 0 ) {
                printf("Enter bytes for APDU to send\n");
                editMemory(cmd_edit,sizeof(cmd_edit));
                printf("Enter buffer length (0 to exit) -> ");
                fflush(stdout);
                fgets(input, sizeof input, stdin);
                if ( sscanf(input, "%zu", &edit_size) == EOF || edit_size == 0 ) {
                    break;
                }
            } else {
                print_data( cmd_edit, edit_size );
            }
            cmd_edit[0] |= (current_open_channel & 0x0F);
            test_i = ste_uicc_sim_channel_send(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_CHANNEL_SEND,
                            current_open_channel, edit_size, cmd_edit );
        } else {
            apdus[n].apdu[0] |= (current_open_channel & 0x0F);
            test_i = ste_uicc_sim_channel_send(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_CHANNEL_SEND,
                            current_open_channel, apdus[n].len, apdus[n].apdu );
        }
        if (test_i) return "sim_channel_send failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 2000);
        if (test_i)
            return "sim_channel_send timed out";
        if(sd && sd->cause == STE_UICC_CAUSE_REQ_SIM_CHANNEL_SEND) {
          printf("#\tSIM channel send response received \n");
          p  = (ste_uicc_sim_channel_send_response_t *)(sd->data);
          printf("#\tSTATUS = ");
          print_status(p->uicc_status_code);
          printf("\n");
          printf("#\tSTATUS Details=%d\n",p->uicc_status_code_fail_details);
          printf("#\tsw1=0x%02x, sw2=0x%02x\n", p->status_word.sw1, p->status_word.sw2);
          if (p->uicc_status_code == STE_UICC_STATUS_CODE_OK) {
            print_data(p->apdu, p->apdu_len);
            free(p->apdu);
          }
          printf("\n");
        }
       sync_data_delete(sd);
    }

    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}


const char *test_sim_channel_send_fail_02()
{
    uint8_t cmd_edit[256] = { 0 };
    char    input[100];
    uint8_t cmd1[] = {
        0x00,   // Class (to be changed to include logical channel)
        0xA4,   // INStruction = SELECT
        0x00,   // P1 = Select DF, EF or MF by file id
        0x04,   // P2 = Return FCP template, for the first/only occurrence
        0x02,   // Lc =
        0x6F,   // 6F07=File
        0x07,
        0x00,   // Le = Max length of result is 256
    };
    uint8_t cmd2[] = {
        0x00,   // Class (to be changed to include logical channel)
        0xB0,   // INStruction = Read Binary
        0x00,   // P1 = Offset to the first byte to read (ie, zero)
        0x00,   // P2 = Ditto
        //0x00,   // Lc = 0 (no input params)
        0x09,   // Le = 9 bytes
    };
    struct {
        uint8_t     *apdu;
        size_t       len;
    } apdus[] = {
        // NOTE: This test sequence will not currently work when the
        // target application on the USIM is the telecom application.
        // Select a non-telecom application first, preferably an "echo" application.
        { cmd1, sizeof(cmd1) },
        //{ cmd2, sizeof(cmd2) },
    };
    ste_sim_t *uicc;
    ste_sim_closure_t pc;
    cat_barrier_t *bar = cat_barrier_new();
    size_t n;
    size_t current = 0;

    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_uicc_new failed.";

    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);
    printf("#\tSIM channel send: current channel=%hu\n",current_open_channel);
    if ( inter || current_open_channel == 0 ) {
        printf("Enter current channel ID -> ");
        fflush(stdout);
        if ( fgets(input, sizeof input, stdin) == NULL ) return 0;
        if ( sscanf(input, "%hu", &current_open_channel) != 1 ) return 0;
    }

    sleep(1);
    for ( n = 0 ; n < sizeof(apdus)/sizeof(apdus[0]) || inter ; n++ ) {
        int test_i;
        sync_data_t            *sd = 0;
        ste_uicc_sim_channel_send_response_t *p;

        cat_barrier_set(bar);

        if (inter) {
            size_t  edit_size = next_usim_buffer(&current,cmd_edit,sizeof(cmd_edit));
            if ( edit_size == 0 ) {
                printf("Enter bytes for APDU to send\n");
                editMemory(cmd_edit,sizeof(cmd_edit));
                printf("Enter buffer length (0 to exit) -> ");
                fflush(stdout);
                fgets(input, sizeof input, stdin);
                if ( sscanf(input, "%zu", &edit_size) == EOF || edit_size == 0 ) {
                    break;
                }
            } else {
                print_data( cmd_edit, edit_size );
            }
            cmd_edit[0] |= (current_open_channel & 0x0F);
            test_i = ste_uicc_sim_channel_send(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_CHANNEL_SEND,
                            current_open_channel, edit_size, cmd_edit );
        } else {
            apdus[n].apdu[0] |= (current_open_channel & 0x0F);
            test_i = ste_uicc_sim_channel_send(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_CHANNEL_SEND,
                            current_open_channel, apdus[n].len, apdus[n].apdu );
        }
        if (test_i) return "sim_channel_send failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 2000);
        if (test_i)
            return "sim_channel_send timed out";
        if(sd && sd->cause == STE_UICC_CAUSE_REQ_SIM_CHANNEL_SEND) {
          printf("#\tSIM channel send response received \n");
          p  = (ste_uicc_sim_channel_send_response_t *)(sd->data);
          printf("#\tSTATUS = ");
          print_status(p->uicc_status_code);
          printf("\n");
          printf("#\tSTATUS Details=%d\n",p->uicc_status_code_fail_details);
          printf("#\tsw1=0x%02x, sw2=0x%02x\n", p->status_word.sw1, p->status_word.sw2);
          if (p->uicc_status_code == STE_UICC_STATUS_CODE_OK) {
            print_data(p->apdu, p->apdu_len);
            free(p->apdu);
          }
          printf("\n");
        }
       sync_data_delete(sd);
    }

    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}


//wrong SELECT apdu.
const char *test_sim_channel_send_fail_03()
{
    uint8_t cmd_edit[256] = { 0 };
    char    input[100];
    uint8_t cmd1[] = {
        0x00,   // Class (to be changed to include logical channel)
        0xA4,   // INStruction = SELECT
        0x00,   // P1 = Select DF, EF or MF by file id
        0x04,   // P2 = Return FCP template, for the first/only occurrence
        0x04,   // Lc =
        0x6F,   // 6F07=File
        0x07,
        0x00,   // Le = Max length of result is 256
    };
    uint8_t cmd2[] = {
        0x00,   // Class (to be changed to include logical channel)
        0xB0,   // INStruction = Read Binary
        0x00,   // P1 = Offset to the first byte to read (ie, zero)
        0x00,   // P2 = Ditto
        //0x00,   // Lc = 0 (no input params)
        0x09,   // Le = 9 bytes
    };
    struct {
        uint8_t     *apdu;
        size_t       len;
    } apdus[] = {
        // NOTE: This test sequence will not currently work when the
        // target application on the USIM is the telecom application.
        // Select a non-telecom application first, preferably an "echo" application.
        { cmd1, sizeof(cmd1) },
        { cmd2, sizeof(cmd2) },
    };
    ste_sim_t *uicc;
    ste_sim_closure_t pc;
    cat_barrier_t *bar = cat_barrier_new();
    size_t n;
    size_t current = 0;

    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_uicc_new failed.";

    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);
    printf("#\tSIM channel send: current channel=%hu\n",current_open_channel);
    if ( inter || current_open_channel == 0 ) {
        printf("Enter current channel ID -> ");
        fflush(stdout);
        if ( fgets(input, sizeof input, stdin) == NULL ) return 0;
        if ( sscanf(input, "%hu", &current_open_channel) != 1 ) return 0;
    }

    sleep(1);
    for ( n = 0 ; n < sizeof(apdus)/sizeof(apdus[0]) || inter ; n++ ) {
        int test_i;
        sync_data_t            *sd = 0;
        ste_uicc_sim_channel_send_response_t *p;

        cat_barrier_set(bar);

        if (inter) {
            size_t  edit_size = next_usim_buffer(&current,cmd_edit,sizeof(cmd_edit));
            if ( edit_size == 0 ) {
                printf("Enter bytes for APDU to send\n");
                editMemory(cmd_edit,sizeof(cmd_edit));
                printf("Enter buffer length (0 to exit) -> ");
                fflush(stdout);
                fgets(input, sizeof input, stdin);
                if ( sscanf(input, "%zu", &edit_size) == EOF || edit_size == 0 ) {
                    break;
                }
            } else {
                print_data( cmd_edit, edit_size );
            }
            cmd_edit[0] |= (current_open_channel & 0x0F);
            test_i = ste_uicc_sim_channel_send(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_CHANNEL_SEND,
                            current_open_channel, edit_size, cmd_edit );
        } else {
            apdus[n].apdu[0] |= (current_open_channel & 0x0F);
            test_i = ste_uicc_sim_channel_send(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_CHANNEL_SEND,
                            current_open_channel, apdus[n].len, apdus[n].apdu );
        }
        if (test_i) return "sim_channel_send failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 2000);
        if (test_i)
            return "sim_channel_send timed out";
        if(sd && sd->cause == STE_UICC_CAUSE_REQ_SIM_CHANNEL_SEND) {
          printf("#\tSIM channel send response received \n");
          p  = (ste_uicc_sim_channel_send_response_t *)(sd->data);
          printf("#\tSTATUS = ");
          print_status(p->uicc_status_code);
          printf("\n");
          printf("#\tSTATUS Details=%d\n",p->uicc_status_code_fail_details);
          printf("#\tsw1=0x%02x, sw2=0x%02x\n", p->status_word.sw1, p->status_word.sw2);
          if (p->uicc_status_code == STE_UICC_STATUS_CODE_OK) {
            print_data(p->apdu, p->apdu_len);
            free(p->apdu);
          }
          printf("\n");
        }
       sync_data_delete(sd);
    }

    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}

//GET RESPONSE apdu
const char *test_sim_channel_send_fail_04()
{
    uint8_t cmd_edit[256] = { 0 };
    char    input[100];
    uint8_t cmd1[] = {
        0x00,   // Class (to be changed to include logical channel)
        0xC0,   // INStruction = SELECT
        0x00,   // P1 = Select DF, EF or MF by file id
        0x04,   // P2 = Return FCP template, for the first/only occurrence
        0x02,   // Lc =
        0x6F,   // 6F07=File
        0x07,
        0x00,   // Le = Max length of result is 256
    };
    uint8_t cmd2[] = {
        0x00,   // Class (to be changed to include logical channel)
        0xB0,   // INStruction = Read Binary
        0x00,   // P1 = Offset to the first byte to read (ie, zero)
        0x00,   // P2 = Ditto
        //0x00,   // Lc = 0 (no input params)
        0x09,   // Le = 9 bytes
    };
    struct {
        uint8_t     *apdu;
        size_t       len;
    } apdus[] = {
        // NOTE: This test sequence will not currently work when the
        // target application on the USIM is the telecom application.
        // Select a non-telecom application first, preferably an "echo" application.
        { cmd1, sizeof(cmd1) },
        { cmd2, sizeof(cmd2) },
    };
    ste_sim_t *uicc;
    ste_sim_closure_t pc;
    cat_barrier_t *bar = cat_barrier_new();
    size_t n;
    size_t current = 0;

    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_uicc_new failed.";

    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);
    for ( n = 0 ; n < sizeof(apdus)/sizeof(apdus[0]) || inter ; n++ ) {
        int test_i;
        sync_data_t            *sd = 0;
        ste_uicc_sim_channel_send_response_t *p;

        cat_barrier_set(bar);

        test_i = ste_uicc_sim_channel_send(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_CHANNEL_SEND,
                            0, apdus[n].len, apdus[n].apdu );
        if (test_i) return "sim_channel_send failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 2000);
        if (test_i)
            return "sim_channel_send timed out";
        if(sd && sd->cause == STE_UICC_CAUSE_REQ_SIM_CHANNEL_SEND) {
          printf("#\tSIM channel send response received \n");
          p  = (ste_uicc_sim_channel_send_response_t *)(sd->data);
          printf("#\tSTATUS = ");
          print_status(p->uicc_status_code);
          printf("\n");
          printf("#\tSTATUS Details=%d\n",p->uicc_status_code_fail_details);
          printf("#\tsw1=0x%02x, sw2=0x%02x\n", p->status_word.sw1, p->status_word.sw2);
          if (p->uicc_status_code == STE_UICC_STATUS_CODE_OK) {
            print_data(p->apdu, p->apdu_len);
            free(p->apdu);
          }
          printf("\n");
        }
       sync_data_delete(sd);
    }

    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}

const char *test_sim_channel_send()
{
    uint8_t cmd_edit[256] = { 0 };
    char    input[100];
    uint8_t cmd1[] = {
        0x00,   // Class (to be changed to include logical channel)
        0xA4,   // INStruction = SELECT
        0x00,   // P1 = Select DF, EF or MF by file id
        0x04,   // P2 = Return FCP template, for the first/only occurrence
        0x02,   // Lc =
        0x6F,   // 6F07=File
        0x07,
        0x00,   // Le = Max length of result is 256
    };
    uint8_t cmd2[] = {
        0x00,   // Class (to be changed to include logical channel)
        0xB0,   // INStruction = Read Binary
        0x00,   // P1 = Offset to the first byte to read (ie, zero)
        0x00,   // P2 = Ditto
        //0x00,   // Lc = 0 (no input params)
        0x09,   // Le = 9 bytes
    };
    struct {
        uint8_t     *apdu;
        size_t       len;
    } apdus[] = {
        // NOTE: This test sequence will not currently work when the
        // target application on the USIM is the telecom application.
        // Select a non-telecom application first, preferably an "echo" application.
        { cmd1, sizeof(cmd1) },
        { cmd2, sizeof(cmd2) },
    };
    ste_sim_t *uicc;
    ste_sim_closure_t pc;
    cat_barrier_t *bar = cat_barrier_new();
    size_t n;
    size_t current = 0;

    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_uicc_new failed.";

    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);
    printf("#\tSIM channel send: current channel=%hu\n",current_open_channel);
    if ( inter || current_open_channel == 0 ) {
        printf("Enter current channel ID -> ");
        fflush(stdout);
        if ( fgets(input, sizeof input, stdin) == NULL ) return 0;
        if ( sscanf(input, "%hu", &current_open_channel) != 1 ) return 0;
    }

    sleep(1);
    for ( n = 0 ; n < sizeof(apdus)/sizeof(apdus[0]) || inter ; n++ ) {
        int test_i;
        sync_data_t            *sd = 0;
        ste_uicc_sim_channel_send_response_t *p;

        cat_barrier_set(bar);

        if (inter) {
            size_t  edit_size = next_usim_buffer(&current,cmd_edit,sizeof(cmd_edit));
            if ( edit_size == 0 ) {
                printf("Enter bytes for APDU to send\n");
                editMemory(cmd_edit,sizeof(cmd_edit));
                printf("Enter buffer length (0 to exit) -> ");
                fflush(stdout);
                fgets(input, sizeof input, stdin);
                if ( sscanf(input, "%zu", &edit_size) == EOF || edit_size == 0 ) {
                    break;
                }
            } else {
                print_data( cmd_edit, edit_size );
            }
            cmd_edit[0] |= (current_open_channel & 0x0F);
            test_i = ste_uicc_sim_channel_send(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_CHANNEL_SEND,
                            current_open_channel, edit_size, cmd_edit );
        } else {
            apdus[n].apdu[0] |= (current_open_channel & 0x0F);
            test_i = ste_uicc_sim_channel_send(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_CHANNEL_SEND,
                            current_open_channel, apdus[n].len, apdus[n].apdu );
        }
        if (test_i) return "sim_channel_send failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 2000);
        if (test_i)
            return "sim_channel_send timed out";
        if(sd && sd->cause == STE_UICC_CAUSE_REQ_SIM_CHANNEL_SEND) {
          printf("#\tSIM channel send response received \n");
          p  = (ste_uicc_sim_channel_send_response_t *)(sd->data);
          printf("#\tSTATUS = ");
          print_status(p->uicc_status_code);
          printf("\n");
          printf("#\tSTATUS Details=%d\n",p->uicc_status_code_fail_details);
          printf("#\tsw1=0x%02x, sw2=0x%02x\n", p->status_word.sw1, p->status_word.sw2);
          if (p->uicc_status_code == STE_UICC_STATUS_CODE_OK) {
            print_data(p->apdu, p->apdu_len);
            free(p->apdu);
          }
          printf("\n");
        }
       sync_data_delete(sd);
    }

    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}

//GET RESPONSE apdu
const char *test_sim_channel_send_fail_05()
{
    uint8_t cmd_edit[256] = { 0 };
    char    input[100];
    uint8_t cmd1[] = {
        0x00,   // Class (to be changed to include logical channel)
        0xC0,   // INStruction = SELECT
        0x00,   // P1 = Select DF, EF or MF by file id
        0x04,   // P2 = Return FCP template, for the first/only occurrence
        0x02,   // Lc =
        0x6F,   // 6F07=File
        0x07,
        0x00,   // Le = Max length of result is 256
    };
    uint8_t cmd2[] = {
        0x00,   // Class (to be changed to include logical channel)
        0xB0,   // INStruction = Read Binary
        0x00,   // P1 = Offset to the first byte to read (ie, zero)
        0x00,   // P2 = Ditto
        //0x00,   // Lc = 0 (no input params)
        0x09,   // Le = 9 bytes
    };
    struct {
        uint8_t     *apdu;
        size_t       len;
    } apdus[] = {
        // NOTE: This test sequence will not currently work when the
        // target application on the USIM is the telecom application.
        // Select a non-telecom application first, preferably an "echo" application.
        { cmd1, sizeof(cmd1) },
        { cmd2, sizeof(cmd2) },
    };
    ste_sim_t *uicc;
    ste_sim_closure_t pc;
    cat_barrier_t *bar = cat_barrier_new();
    size_t n;
    size_t current = 0;

    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_uicc_new failed.";

    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);
    for ( n = 0 ; n < sizeof(apdus)/sizeof(apdus[0]) || inter ; n++ ) {
        int test_i;
        sync_data_t            *sd = 0;
        ste_uicc_sim_channel_send_response_t *p;

        cat_barrier_set(bar);

        test_i = ste_uicc_sim_channel_send(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_CHANNEL_SEND,
                            0, apdus[n].len, apdus[n].apdu );
        if (test_i) return "sim_channel_send failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 2000);
        if (test_i)
            return "sim_channel_send timed out";
        if(sd && sd->cause == STE_UICC_CAUSE_REQ_SIM_CHANNEL_SEND) {
          printf("#\tSIM channel send response received \n");
          p  = (ste_uicc_sim_channel_send_response_t *)(sd->data);
          printf("#\tSTATUS = ");
          print_status(p->uicc_status_code);
          printf("\n");
          printf("#\tSTATUS Details=%d\n",p->uicc_status_code_fail_details);
          printf("#\tsw1=0x%02x, sw2=0x%02x\n", p->status_word.sw1, p->status_word.sw2);
          if (p->uicc_status_code == STE_UICC_STATUS_CODE_OK) {
            print_data(p->apdu, p->apdu_len);
            free(p->apdu);
          }
          printf("\n");
        }
       sync_data_delete(sd);
    }

    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}

const char *test_sim_channel_open()
{
    char input[100];
    uint8_t java_aid[] = {
        0xA0, 0x00, 0x00, 0x00, 0x18, 0x50, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x52, 0x41, 0x44, 0x50,
    };
    ste_sim_t *uicc;
    ste_sim_closure_t pc;
    cat_barrier_t *bar = cat_barrier_new();

    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_uicc_new failed.";

    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);
    do {
        int test_i;
        sync_data_t            *sd = 0;
        ste_uicc_sim_channel_open_response_t *p;
        size_t                  aid_len = 0;

        cat_barrier_set(bar);

        if (inter) {
            size_t  edit_size;
            printf("Enter bytes for the required application ID\n");
            editMemory(java_aid,sizeof(java_aid));
            printf("Enter AID length (0 to exit) -> ");
            fflush(stdout);
            fgets(input, sizeof input, stdin);
            if ( sscanf(input, "%zu", &edit_size) == EOF || edit_size == 0 ) {
                break;
            }
            aid_len = edit_size;
        } else {
            aid_len = sizeof(java_aid);
        }

        if ( uicc_sim ) {
            test_i = ste_uicc_sim_channel_open(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_CHANNEL_OPEN,
                            0, 0 );
        } else {
            test_i = ste_uicc_sim_channel_open(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_CHANNEL_OPEN,
                            aid_len, java_aid );
        }
        if (test_i) return "sim_channel_open failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 2000);
        if (test_i)
            return "sim_channel_open timed out";
        if(sd && sd->cause == STE_UICC_CAUSE_REQ_SIM_CHANNEL_OPEN) {
          printf("#\tSIM channel open response received \n");
          p  = (ste_uicc_sim_channel_open_response_t *)(sd->data);
          printf("#\tSTATUS = ");
          print_status(p->uicc_status_code);
          printf("\n");
          printf("#\tSTATUS Details=%d\n",p->uicc_status_code_fail_details);
          printf("#\tsw1=0x%02x, sw2=0x%02x\n", p->status_word.sw1, p->status_word.sw2);
          if (p->uicc_status_code == STE_UICC_STATUS_CODE_OK) {
              printf("#\tSession ID=%d\n", p->session_id);
              current_open_channel = p->session_id;
          }
        }
       sync_data_delete(sd);
    } while (0);

    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}

const char *test_sim_channel_open_fail_01()
{
    char input[100];
    uint8_t java_aid[] = {
        0xA0, 0x00, 0x00, 0x00, 0x18, 0x50, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x52, 0x41, 0x44, 0x50,
    };
    ste_sim_t *uicc;
    ste_sim_closure_t pc;
    cat_barrier_t *bar = cat_barrier_new();

    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_uicc_new failed.";

    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);
    do {
        int test_i;
        sync_data_t            *sd = 0;
        ste_uicc_sim_channel_open_response_t *p;
        size_t                  aid_len = 0;

        cat_barrier_set(bar);

        if (inter) {
            size_t  edit_size;
            printf("Enter bytes for the required application ID\n");
            editMemory(java_aid,sizeof(java_aid));
            printf("Enter AID length (0 to exit) -> ");
            fflush(stdout);
            fgets(input, sizeof input, stdin);
            if ( sscanf(input, "%zu", &edit_size) == EOF || edit_size == 0 ) {
                break;
            }
            aid_len = edit_size;
        } else {
            aid_len = sizeof(java_aid);
        }

        if ( uicc_sim ) {
            test_i = ste_uicc_sim_channel_open(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_CHANNEL_OPEN,
                            0, 0 );
        } else {
            test_i = ste_uicc_sim_channel_open(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_CHANNEL_OPEN,
                            aid_len, java_aid );
        }
        if (test_i) return "sim_channel_open failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 2000);
        if (test_i)
            return "sim_channel_open timed out";
        if(sd && sd->cause == STE_UICC_CAUSE_REQ_SIM_CHANNEL_OPEN) {
          printf("#\tSIM channel open response received \n");
          p  = (ste_uicc_sim_channel_open_response_t *)(sd->data);
          printf("#\tSTATUS = ");
          print_status(p->uicc_status_code);
          printf("\n");
          printf("#\tSTATUS Details=%d\n",p->uicc_status_code_fail_details);
          printf("#\tsw1=0x%02x, sw2=0x%02x\n", p->status_word.sw1, p->status_word.sw2);
          if (p->uicc_status_code == STE_UICC_STATUS_CODE_OK) {
              printf("#\tSession ID=%d\n", p->session_id);
              current_open_channel = p->session_id;
          }
        }
       sync_data_delete(sd);
    } while (0);

    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}


const char *test_sim_channel_open_fail_02()
{
    char input[100];
    uint8_t java_aid[] = {
        0xA0, 0x00, 0x00, 0x00, 0x18, 0x50, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x52, 0x41, 0x44, 0x50,
    };
    ste_sim_t *uicc;
    ste_sim_closure_t pc;
    cat_barrier_t *bar = cat_barrier_new();

    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_uicc_new failed.";

    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);
    do {
        int test_i;
        sync_data_t            *sd = 0;
        ste_uicc_sim_channel_open_response_t *p;
        size_t                  aid_len = 0;

        cat_barrier_set(bar);

        if (inter) {
            size_t  edit_size;
            printf("Enter bytes for the required application ID\n");
            editMemory(java_aid,sizeof(java_aid));
            printf("Enter AID length (0 to exit) -> ");
            fflush(stdout);
            fgets(input, sizeof input, stdin);
            if ( sscanf(input, "%zu", &edit_size) == EOF || edit_size == 0 ) {
                break;
            }
            aid_len = edit_size;
        } else {
            aid_len = sizeof(java_aid);
        }

        if ( uicc_sim ) {
            test_i = ste_uicc_sim_channel_open(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_CHANNEL_OPEN,
                            0, 0 );
        } else {
            test_i = ste_uicc_sim_channel_open(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_CHANNEL_OPEN,
                            aid_len, java_aid );
        }
        if (test_i) return "sim_channel_open failed.";

    } while (0);

    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}

const char *test_sim_channel_close_fail_01()
{
    char    input[100];
    ste_sim_t *uicc;
    ste_sim_closure_t pc;
    cat_barrier_t *bar = cat_barrier_new();

    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_uicc_new failed.";

    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);
    printf("#\tSIM channel send: current channel=%hu\n",current_open_channel);
    if ( inter || current_open_channel == 0 ) {
        printf("Enter current channel ID -> ");
        fflush(stdout);
        if ( fgets(input, sizeof input, stdin) == NULL ) return 0;
        if ( sscanf(input, "%hu", &current_open_channel) != 1 ) return 0;
    }

    sleep(1);
    do {
        int test_i;
        sync_data_t            *sd = 0;
        ste_uicc_sim_channel_close_response_t *p;

        cat_barrier_set(bar);

        test_i = ste_uicc_sim_channel_close(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_CHANNEL_CLOSE,
                        current_open_channel );
        if (test_i) return "sim_channel_close failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 2000);
        if (test_i)
            return "sim_channel_close timed out";
        if(sd && sd->cause == STE_UICC_CAUSE_REQ_SIM_CHANNEL_CLOSE) {
          printf("#\tSIM channel close response received \n");
          p  = (ste_uicc_sim_channel_close_response_t *)(sd->data);
          printf("#\tSTATUS = ");
          print_status(p->uicc_status_code);
          printf("\n");
          printf("#\tSTATUS Details=%d\n",p->uicc_status_code_fail_details);
          printf("#\tsw1=0x%02x, sw2=0x%02x\n", p->status_word.sw1, p->status_word.sw2);
        }
       sync_data_delete(sd);
    } while (0);

    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}


const char *test_sim_channel_close_fail_02()
{
    char    input[100];
    ste_sim_t *uicc;
    ste_sim_closure_t pc;
    cat_barrier_t *bar = cat_barrier_new();

    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_uicc_new failed.";

    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);
    printf("#\tSIM channel send: current channel=%hu\n",current_open_channel);
    if ( inter || current_open_channel == 0 ) {
        printf("Enter current channel ID -> ");
        fflush(stdout);
        if ( fgets(input, sizeof input, stdin) == NULL ) return 0;
        if ( sscanf(input, "%hu", &current_open_channel) != 1 ) return 0;
    }

    sleep(1);
    do {
        int test_i;
        sync_data_t            *sd = 0;
        ste_uicc_sim_channel_close_response_t *p;

        cat_barrier_set(bar);

        test_i = ste_uicc_sim_channel_close(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_CHANNEL_CLOSE,
                        current_open_channel );
        if (test_i) return "sim_channel_close failed.";

    } while (0);

    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}

const char *test_sim_channel_close()
{
    char    input[100];
    ste_sim_t *uicc;
    ste_sim_closure_t pc;
    cat_barrier_t *bar = cat_barrier_new();

    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_uicc_new failed.";

    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);
    printf("#\tSIM channel send: current channel=%hu\n",current_open_channel);
    if ( inter || current_open_channel == 0 ) {
        printf("Enter current channel ID -> ");
        fflush(stdout);
        if ( fgets(input, sizeof input, stdin) == NULL ) return 0;
        if ( sscanf(input, "%hu", &current_open_channel) != 1 ) return 0;
    }

    sleep(1);
    do {
        int test_i;
        sync_data_t            *sd = 0;
        ste_uicc_sim_channel_close_response_t *p;

        cat_barrier_set(bar);

        test_i = ste_uicc_sim_channel_close(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_CHANNEL_CLOSE,
                        current_open_channel );
        if (test_i) return "sim_channel_close failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 2000);
        if (test_i)
            return "sim_channel_close timed out";
        if(sd && sd->cause == STE_UICC_CAUSE_REQ_SIM_CHANNEL_CLOSE) {
          printf("#\tSIM channel close response received \n");
          p  = (ste_uicc_sim_channel_close_response_t *)(sd->data);
          printf("#\tSTATUS = ");
          print_status(p->uicc_status_code);
          printf("\n");
          printf("#\tSTATUS Details=%d\n",p->uicc_status_code_fail_details);
          printf("#\tsw1=0x%02x, sw2=0x%02x\n", p->status_word.sw1, p->status_word.sw2);
        }
       sync_data_delete(sd);
    } while (0);

    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}

const char             *test_startup()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t       pc;
    cat_barrier_t           *bar = cat_barrier_new();


    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    ste_cat_register( uicc, TUICC_CT_STE_UICC_CAUSE_REGISTER, reg_events );
#ifndef HAVE_ANDROID_OS
    if ( uicc_sim &&
         ste_uicc_set_app_type_to_sim( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SET_APP_TYPE_SIM ) != 0 ) {
        return "ste_uicc_set_app_type_to_sim";
    }
#endif
    sleep(1);

    do {
        int test_i;
        sync_data_t            *sd = 0;

        cat_barrier_set(bar);
        test_i = ste_sim_startup(uicc, TUICC_CT_STE_SIM_CAUSE_STARTUP);
        if ( test_i ) {
            TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            cat_barrier_delete(bar);
            return  "ste_sim_startup";
        }

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 5000);
        if ( test_i ) {
            TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            cat_barrier_delete(bar);
            return "ste_sim_startup timed out";
        }

        if( sd && sd->cause == STE_SIM_CAUSE_STARTUP ) {
            printf("#\tSIM startup response received \n");
            sync_data_delete(sd);
            break;
        }

        sync_data_delete(sd);
    } while(1);

    TTEST(ste_cat_deregister, TUICC_CT_STE_UICC_CAUSE_DEREGISTER, uicc, bar, 100);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}

const char             *test_passive()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t       pc;
    int                      i;
    cat_barrier_t            *bar = cat_barrier_new();
    passive_mode = 1;

    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    for (;;) {
        sync_data_t            *sd;

        cat_barrier_set(bar);
        i = cat_barrier_timedwait(bar, (void **) &sd, 10000);
        if (i) {
            static int count = 5;
            if ( count && count-- ) {   // suppress after so many iterations
                printf("#\tmain Timed out\n");
            }
            continue;
        }
        if (!sd) {
            printf("#\tmain No sync data.\n");
            continue;
        }

        printf("#\tmain: %2d %p : %s\n",
               sd->cause,
               (void*) sd->data, ste_sim_cause_name[sd->cause]);

        if (sd->cause == STE_SIM_CAUSE_DISCONNECT || sd->cause == STE_SIM_CAUSE_HANGUP) {
            sync_data_delete(sd);
            break;
        }


        sync_data_delete(sd);
    }

// Passive connections are disconnected when STE_SIM_CAUSE_DISCONNECT is sent
//    i = ste_sim_disconnect(uicc, TUICC_CT_STE_SIM_CAUSE_DISCONNECT);
//    if ( !i ) {
//        printf("disconnect failed\n");
//    }

    ste_sim_delete(uicc, TUICC_CT_NOT_USED);

    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}

static char *handle_plmn_read_result(ste_uicc_sim_file_read_plmn_response_t *read_response_p) {
    int                     GSM_AcT = 1;
    int                     GSM_Compact_AcT = 0;
    int                     UTRAN_AcT =1;

    if (read_response_p) {
        printf("#\tREAD UICC STATUS CODE = %d\n", read_response_p->uicc_status_code);
        printf("#\tREAD UICC STATUS CODE DETAILS = %d\n", read_response_p->uicc_status_code_fail_details);
        printf("#\tSTATUS WORD 1 = %d\n", read_response_p->status_word.sw1);
        printf("#\tSTATUS WORD 2 = %d\n", read_response_p->status_word.sw2);
        if (read_response_p->uicc_status_code == STE_UICC_STATUS_CODE_OK) {
            int index;
            printf("#\nPLMN:\n");
            printf("nr_of_plmn: %d\n", read_response_p->nr_of_plmn);
            printf("index\tMCC\tMNC\tAcT (GSM GSMcomp UTRAN)\n");
            printf("----------------------------\n");
            for (index = 0; index < read_response_p->nr_of_plmn; index++) {
                if ((read_response_p->ste_sim_plmn_with_AcT_p[index].AcT & SIM_ACT_GSM_BITMASK)
                        != SIM_ACT_UNSPECIFIED_BITMASK) {
                    GSM_AcT = 1;
                } else {
                    GSM_AcT = 0;
                }
                if ((read_response_p->ste_sim_plmn_with_AcT_p[index].AcT & SIM_ACT_GSM_COMPACT_BITMASK)
                        != SIM_ACT_UNSPECIFIED_BITMASK) {
                    GSM_Compact_AcT = 1;
                } else {
                    GSM_Compact_AcT = 0;
                }
                if ((read_response_p->ste_sim_plmn_with_AcT_p[index].AcT & SIM_ACT_UTRAN_BITMASK)
                        != SIM_ACT_UNSPECIFIED_BITMASK) {
                    UTRAN_AcT = 1;
                } else {
                    UTRAN_AcT = 0;
                }
                printf("[%d]\t%x%x%x\t%x%x%x \t%x%x%x\n", index,
                       read_response_p->ste_sim_plmn_with_AcT_p[index].PLMN.MCC1,
                       read_response_p->ste_sim_plmn_with_AcT_p[index].PLMN.MCC2,
                       read_response_p->ste_sim_plmn_with_AcT_p[index].PLMN.MCC3,
                       read_response_p->ste_sim_plmn_with_AcT_p[index].PLMN.MNC1,
                       read_response_p->ste_sim_plmn_with_AcT_p[index].PLMN.MNC2,
                       read_response_p->ste_sim_plmn_with_AcT_p[index].PLMN.MNC3, GSM_AcT, GSM_Compact_AcT,
                        UTRAN_AcT);
            }
            printf("\n");
        } else {
            return "readPLMN failed";
        }
        free(read_response_p->ste_sim_plmn_with_AcT_p);
    } else {
        return "#\tREAD STATUS = -1, SIMD returned error.";
    }
    return 0;
}

const char *test_readPLMN ( void )
{
    ste_sim_t              *sim;
    ste_sim_closure_t       pc;
    cat_barrier_t          *bar = cat_barrier_new();
    char                   *rtn_str;

    pc.func = the_callback;
    pc.user_data = bar;
    sim = ste_sim_new(&pc);
    if (!sim)
        return "ste_sim_new failed.";

    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, sim, bar, 100);

    sleep(1);

    do {
        int test_i;
        sync_data_t *sd = 0;
        ste_uicc_sim_file_read_plmn_response_t* p;

        cat_barrier_set(bar);
        {
            test_i = ste_uicc_sim_file_read_plmn( sim, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_FILE_READ_PLMN, SIM_FILE_ID_PLMN_WACT);
        }

        if ( test_i )
            return "readPLMN failed";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 1000);
        if ( test_i )
            return "readPLMN timed out";

        if( !sd || sd->cause != STE_UICC_CAUSE_REQ_SIM_FILE_READ_PLMN )
            return "ste_uicc_sim_file_read_plmn wrong response";


        p = (ste_uicc_sim_file_read_plmn_response_t*) (sd->data);
        if((rtn_str = handle_plmn_read_result(p))){
            return rtn_str;
        }

        sync_data_delete(sd);
    if (!inter) break;
    } while(1);


    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, sim, bar, 100);
    ste_sim_delete(sim, TUICC_CT_NOT_USED);

    cat_barrier_delete(bar);

    sleep(1);
    return 0;
}

const char *test_updatePLMN ( void )
{
    ste_sim_t         *sim;
    ste_sim_closure_t  pc;
    cat_barrier_t     *bar = cat_barrier_new();
    char               foo[5];

    pc.func = the_callback;
    pc.user_data = bar;

    sim = ste_sim_new(&pc);
    if (!sim)
        return "ste_sim_new failed.";

    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, sim, bar, 100);

    sleep(1);

    do {
        int test_i, i;
        int index = -1;
        sync_data_t *sd = 0;
        ste_sim_plmn_with_AcT_t* plmn_p = NULL;
        ste_uicc_sim_plmn_AccessTechnology_t plmnAcT = 0xffff;
        ste_uicc_update_sim_file_plmn_response_t *p = NULL;
        ste_uicc_sim_plmn_file_id_t file_id = SIM_FILE_ID_PLMN_WACT;

        plmn_p = malloc(sizeof(ste_sim_plmn_with_AcT_t));
        // memset to make valgrind happy
        memset(plmn_p, 0, sizeof(ste_sim_plmn_with_AcT_t));
        plmn_p->PLMN.MCC1 = 0x1;
        plmn_p->PLMN.MCC2 = 0x2;
        plmn_p->PLMN.MCC3 = 0x3;
        plmn_p->PLMN.MNC1 = 0x1;
        plmn_p->PLMN.MNC2 = 0x1;
        plmn_p->PLMN.MNC3 = 0xf;
        plmn_p->AcT = plmnAcT;

        if (inter) {
            foo[0] = 0;
            printf("file_id?\n(1): PLMNwact\n(2): PLMNsel\n");
            scanf("%s", foo);
            if (foo[0] == '2') {
                file_id = SIM_FILE_ID_PLMN_SEL;
            }
            foo[0] = 0;
            printf("(U)pdate or (R)emove element? (U/R)");
            scanf("%s", foo);
            if (foo[0] == 'R'){
                free(plmn_p);
                plmn_p = NULL;
            }

            printf("index to update/remove?: ");
            i = scanf("%i", (int *) &index);
        }

        cat_barrier_set(bar);
        {
            test_i = ste_uicc_sim_file_update_plmn(sim,
                                                   TUICC_CT_STE_UICC_CAUSE_REQ_SIM_FILE_READ_PLMN,
                                                   index,
                                                   plmn_p,
                                                   file_id);
        }
        free(plmn_p);
        if ( test_i )
            return "updatePLMN failed";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 1000);
        if ( test_i )
            return "updatePLMN timed out";

        if( !sd || sd->cause != STE_UICC_CAUSE_REQ_SIM_FILE_UPDATE_PLMN )
            return "ste_uicc_sim_file_update_plmn wrong response";

        p = (ste_uicc_update_sim_file_plmn_response_t*)(sd->data);
        if (p) {
            printf("#\tUPDATE UICC STATUS CODE = %d\n", p->uicc_status_code);
            printf("#\tUPDATE UICC STATUS CODE DETAILS = %d\n", p->uicc_status_code_fail_details);
            printf("#\tSTATUS WORD 1 = %d\n", p->status_word.sw1);
            printf("#\tSTATUS WORD 2 = %d\n", p->status_word.sw2);
            if (p->uicc_status_code == STE_UICC_STATUS_CODE_OK) {

            } else {
                return "updatePLMN failed";
            }
        } else {
            printf("#\tUPDATE STATUS = -1, SIMD returned error.\n");
        }
        sync_data_delete(sd);
    if (!inter) break;
    } while(1);


    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, sim, bar, 100);
    ste_sim_delete(sim, TUICC_CT_NOT_USED);

    cat_barrier_delete(bar);

    sleep(1);
    return 0;
}

#define FITEST( C, FILEID, FILEPATH, B, T )     \
do {                                            \
    int test_i;                                 \
    ste_uicc_sim_get_file_info_type_t type = STE_UICC_SIM_GET_FILE_INFO_TYPE_EF;\
    cat_barrier_set(B);                         \
    test_i = ste_uicc_sim_get_file_information(C, TUICC_CT_STE_UICC_CAUSE_REQ_GET_FILE_INFORMATION, \
            FILEID, FILEPATH, type);            \
    if ( test_i ) {                             \
        ste_sim_delete(C, TUICC_CT_STE_UICC_CAUSE_REQ_GET_FILE_INFORMATION);    \
        cat_barrier_delete(B);                  \
        return "ste_uicc_sim_get_file_information failed.";                     \
    }                                           \
    test_i = my_barrier_timedwait(B, T);        \
    if ( test_i ) {                             \
        ste_sim_delete(C, TUICC_CT_STE_UICC_CAUSE_REQ_GET_FILE_INFORMATION);    \
        cat_barrier_delete(B);                  \
        return "ste_uicc_sim_get_file_information timed out";                   \
    }                                           \
} while(0)

const char *test_readPLMN_sel()
{

    ste_sim_t              *sim;
    ste_sim_closure_t       pc;
    cat_barrier_t          *bar = cat_barrier_new();
    char                   *rtn_str;

    pc.func = the_callback;
    pc.user_data = bar;

    sim = ste_sim_new(&pc);
    if (!sim)
        return "ste_sim_new failed.";

    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, sim, bar, 100);

    sleep(1);

    do {
        int test_i;
        sync_data_t *sd = 0;
        ste_uicc_sim_file_read_plmn_response_t* p;
        cat_barrier_set(bar);
        {
            test_i = ste_uicc_sim_file_read_plmn( sim, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_FILE_READ_PLMN_SEL, SIM_FILE_ID_PLMN_SEL);
        }

        if ( test_i )
            return "readPLMNsel failed";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 1000);
        if ( test_i )
            return "readPLMNsel timed out";

        if( !sd || sd->cause != STE_UICC_CAUSE_REQ_SIM_FILE_READ_PLMN )
            return "ste_uicc_sim_file_read_plmn_sel wrong response";

        p = (ste_uicc_sim_file_read_plmn_response_t*) (sd->data);
        if ((rtn_str = handle_plmn_read_result(p))) {
            return rtn_str;
        }
        sync_data_delete(sd);
    if (!inter) break;
    } while(1);


    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, sim, bar, 100);
    ste_sim_delete(sim, TUICC_CT_NOT_USED);

    cat_barrier_delete(bar);

    sleep(1);
    return 0;

}

// Test accessing a number of files that Android seems to be doing.
const char *test_many_files() {
    ste_sim_t              *uicc;
    ste_sim_closure_t       pc;
    int                     i,j;
    cat_barrier_t          *bar = cat_barrier_new();
    const uint16_t afil[] = {0x2FE2, 0x6F11, 0x6F13, 0x6F14, 0x6F16, 0x6F17, 0x6F18, 0x6F38, 0x6F40, 0x6F46, 0x6FAD, 0x6FC5, 0x6FC9, 0x6FCA, 0x6FCB, 0x6FCD, 0};
    const char *apath_usim[] = {"3F00", "3F007FFF", "3F007FFF", "3F007FFF", "3F007FFF", "3F007FFF", "3F007FFF", "3F007FFF", "3F007FFF", "3F007FFF", "3F007FFF", "3F007FFF", "3F007FFF", "3F007FFF", "3F007FFF", "3F007FFF", NULL};
    char foo[5];

    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";

    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);
    // Your test here...

    for (j=0;afil[j];j++) {
      printf("Testing %x\n", afil[j]);
      FITEST(uicc, afil[j], apath_usim[j], bar, 200);
      if (inter) {
	foo[0] = 0;
	printf("Next? (Y/N");
	scanf("%s", foo);
	if (foo[0] == 'N') break;
      }
    }

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    i = ste_sim_disconnect(uicc, TUICC_CT_STE_SIM_CAUSE_DISCONNECT);
    if (!i) {
        ste_sim_delete(uicc, TUICC_CT_NOT_USED);
        cat_barrier_delete(bar);
        return "disconnect did not fail when disconnected.";
    }

    ste_sim_delete(uicc, TUICC_CT_NOT_USED);

    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}


// This tests
const char             *test_preStart()
{
    int                     retries = 10;
    ste_sim_t              *uicc;
    ste_sim_closure_t       pc;
    int                     i;
    cat_barrier_t          *bar = cat_barrier_new();

    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";

    printf( "Please run simd within the next %d seconds\n", retries);
    for ( i = 0 ; i < retries ; i++ )
    {
        int test_i;
        cat_barrier_set(bar);
        test_i = ste_sim_connect(uicc, TUICC_CT_STE_SIM_CAUSE_CONNECT);
        if ( test_i == STE_SIM_SUCCESS ) {
            printf("simd running\n");
            test_i = my_barrier_timedwait(bar,100);
            if ( test_i ) {
                printf("Timeout waiting for connect response\n");
                goto error;
            } else {
                // successful connection, continue with the test
                break;
            }
        }
        else if ( test_i == STE_SIM_ERROR_SOCKET ) {
            printf("simd not running\n");
            sleep(1);
        } else {
            printf("ste_sim_connect returned %d\n", test_i );
            goto error;
        }
    }
    if ( i == retries ) {
        printf("Failed to contact simd within the required interval\n");
        goto error;
    }

    // The simd takes about 5 seconds to start, so lets loop the same again.
    for ( i = 0 ; i < retries ; i++ )
    {
        int test_i;

        // This should always pass
        cat_barrier_set(bar);
        test_i = ste_uicc_sim_get_state(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_GET_SIM_STATE);
        if ( test_i != STE_SIM_SUCCESS ) {
            printf("#\tste_uicc_sim_get_state returned %d\n", test_i);
        } else {
            sync_data_t            *sd = 0;

            test_i = cat_barrier_timedwait(bar, (void **) &sd, 500);
            if ( test_i ) {
                printf("#\tcat_barrier_timedwait timed out, returned %d\n", test_i);
            } else {
                if( sd && sd->cause == STE_UICC_CAUSE_REQ_GET_SIM_STATE ) {
                    ste_uicc_get_sim_state_response_t *p = (ste_uicc_get_sim_state_response_t*)(sd->data);
                    printf("#\tGET_SIM_STATE STATUS = %d, value=%d\n", p->error_cause, p->state);
                    sync_data_delete(sd);
                }
                else if ( sd && sd->cause == STE_UICC_CAUSE_NOT_READY ) {
                    ste_uicc_not_ready_t *p = (ste_uicc_not_ready_t*)(sd->data);
                    printf("#\tSTE_UICC_CAUSE_NOT_READY, original msg=%x\n", p->type );
                    sync_data_delete(sd);
                } else {
                    printf("#\tUnknown from barrier release of ste_uicc_sim_get_state\n");
                }
            }
        }

        // This should fail until simd reaches a useful state
        cat_barrier_set(bar);
        test_i = ste_uicc_get_app_info(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_APP_INFO );
        if ( test_i != STE_SIM_SUCCESS ) {
            printf("#\tste_uicc_get_app_info returned %d\n", test_i);
        } else {
            sync_data_t            *sd = 0;

            test_i = cat_barrier_timedwait(bar, (void **) &sd, 500);
            if ( test_i ) {
                printf("#\tcat_barrier_timedwait timed out, returned %d\n", test_i);
            } else {
                if(sd && sd->cause == STE_UICC_CAUSE_REQ_GET_APP_INFO) {
                    ste_uicc_sim_app_info_response_t *p  = (ste_uicc_sim_app_info_response_t*)(sd->data);
                    printf("#\tGet app info: status=%d, app_type=%d\n", p->status,p->app_type);
                    sync_data_delete(sd);
                }
                else if ( sd && sd->cause == STE_UICC_CAUSE_NOT_READY ) {
                    ste_uicc_not_ready_t *p = (ste_uicc_not_ready_t*)(sd->data);
                    printf("#\tSTE_UICC_CAUSE_NOT_READY, original msg=%x\n", p->type );
                    sync_data_delete(sd);
                } else {
                    printf("#\tUnknown from barrier release of ste_uicc_get_app_info\n");
                }
            }
        }

        sleep( 1 );
    }
#if 0
    ste_uicc_sim_get_state is something that should always succeed
    ste_uicc_get_app_info is something that should fail initially, then pass when simd is normal
#endif
#if 0
#define TTEST( FUNC, CT, C, B, T )              \
do {                                            \
    int test_i;                                 \
    cat_barrier_set(B);                         \
    test_i = FUNC(C, CT);                       \
    if ( test_i ) {                             \
        ste_sim_delete(C, CT);                  \
        cat_barrier_delete(B);                  \
        return #FUNC " failed.";                \
    }                                           \
    test_i = my_barrier_timedwait(B, T);        \
    if ( test_i ) {                             \
        cat_barrier_delete(B);                  \
        ste_sim_delete(C, CT);                  \
        return #FUNC " timed out";              \
    }                                           \
} while(0)
#endif
    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
//
//    i = ste_sim_disconnect(uicc, TUICC_CT_STE_SIM_CAUSE_DISCONNECT);
//    if (!i) {
//        ste_sim_delete(uicc, TUICC_CT_NOT_USED);
//        cat_barrier_delete(bar);
//        return "disconnect did not fail when disconnected.";
//    }
//
//    ste_sim_delete(uicc, TUICC_CT_NOT_USED);

error:
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}


const char             *test_icon_read()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t      pc;
    cat_barrier_t           *bar = cat_barrier_new();


    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);

    do {
        filerec_t   recInfo;
        int         test_i;
        sync_data_t *sd = 0;
        ste_uicc_sim_icon_read_response_t* p;

        //  Correct values for test with MSL and MAL
        cat_barrier_set(bar);

        // Reads EF_ICON file on USIM
        recInfo.file_id = 0x4f20;
        recInfo.v.record_or_offset = 0;
        recInfo.length = 0;
        recInfo.pathptr = NULL; // Not used here, though

        if (inter && readFileSpec( &recInfo, FILE_REC_READ_FOR_ICON) == 0 ) break;

        test_i = ste_uicc_sim_icon_read(uicc,
                TUICC_CT_STE_UICC_CAUSE_REQ_SIM_ICON, recInfo.file_id,
                recInfo.v.record_or_offset, recInfo.length);

        if ( test_i )
            return  "ste_uicc_sim_icon_read failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 1000);
        if ( test_i )
            return "ste_uicc_sim_icon_read timed out";

        if( !sd || sd->cause != STE_UICC_CAUSE_REQ_SIM_ICON_READ)
            return "ste_uicc_sim_icon_read wrong response";

        p = (ste_uicc_sim_icon_read_response_t *)(sd->data);
        printf("#\tREAD LENGTH = %d hex:\n", (int)p->length);
        printf("#\tSTATUS WORD 1 = %d hex:\n", (int)p->status_word.sw1);
        printf("#\tSTATUS WORD 2 = %d hex:\n", (int)p->status_word.sw2);
        printf("#\tUICC STATUS CODE = %d hex:\n", (int)p->uicc_status_code);
        printf("#\tUICC STATUS CODE DETAILS = %d hex:\n", (int)p->uicc_status_code_fail_details);

        if (p->data) {
            print_data(p->data,p->length);
            memcpy(simFileBuffer,p->data,p->length);
            free(p->data);
        }

        sync_data_delete(sd);
        if (!inter) break;
    } while(1);


    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}

const char             *test_icon_read_binary()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t      pc;
    cat_barrier_t           *bar = cat_barrier_new();


    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);

    do {
        filerec_t   recInfo;
        int         test_i;
        sync_data_t *sd = 0;
        ste_uicc_sim_icon_read_response_t* p;

        //  Correct values for test with MSL and MAL
        cat_barrier_set(bar);

        // Reads EF_ICON file on USIM as binary
        recInfo.file_id = 0x4f21;
        recInfo.v.record_or_offset = 0;
        recInfo.length = 0;
        recInfo.pathptr = NULL; // Not used here, though

        if (inter && readFileSpec( &recInfo, FILE_REC_READ_FOR_ICON) == 0 ) break;

        test_i = ste_uicc_sim_icon_read(uicc,
                TUICC_CT_STE_UICC_CAUSE_REQ_SIM_ICON, recInfo.file_id,
                recInfo.v.record_or_offset, recInfo.length);

        if ( test_i )
            return  "ste_uicc_sim_icon_read failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 1000);
        if ( test_i )
            return "ste_uicc_sim_icon_read timed out";

        if( !sd || sd->cause != STE_UICC_CAUSE_REQ_SIM_ICON_READ)
            return "ste_uicc_sim_icon_read wrong response";

        p = (ste_uicc_sim_icon_read_response_t *)(sd->data);
        printf("#\tREAD LENGTH = %d hex:\n", (int)p->length);
        printf("#\tSTATUS WORD 1 = %d hex:\n", (int)p->status_word.sw1);
        printf("#\tSTATUS WORD 2 = %d hex:\n", (int)p->status_word.sw2);
        printf("#\tUICC STATUS CODE = %d hex:\n", (int)p->uicc_status_code);
        printf("#\tUICC STATUS CODE DETAILS = %d hex:\n", (int)p->uicc_status_code_fail_details);

        if (p->data) {
            print_data(p->data,p->length);
            memcpy(simFileBuffer,p->data,p->length);
            free(p->data);
        }

        sync_data_delete(sd);
        if (!inter) break;
    } while(1);


    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}

const char             *test_getSubscriberNumber()
{
    ste_sim_t         *uicc;
    ste_sim_closure_t pc;
    cat_barrier_t     *bar = cat_barrier_new();
    ste_sim_subscriber_number_record_t *tmp_p = NULL;
    int i = 0;

    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);

    if (!uicc) {
        return "ste_sim_new failed.";
    }


    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);

    int test_i;
    sync_data_t *sd = 0;
    ste_sim_subscriber_number_response_t *p = NULL;

    cat_barrier_set(bar);
    {
        test_i = ste_uicc_get_subscriber_number(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SUBSCRIBER_NUMBER);
    }

    if (test_i) {
        return  "ste_uicc_get_subscriber_number failed.";
    }

    test_i = cat_barrier_timedwait(bar, (void **) &sd, 2000);

    if (test_i) {
        return "ste_uicc_get_subscriber_number timed out";
    }

    if (!sd || sd->cause != STE_UICC_CAUSE_REQ_READ_SUBSCRIBER_NUMBER) {
        return "ste_uicc_get_subscriber_number wrong response";
    }

    p = (ste_sim_subscriber_number_response_t *)(sd->data);

    printf("#\tREAD Sbscriber Number Response\n");
    printf("#\tUICC STATUS CODE: %d\n", p->uicc_status_code);
    printf("#\tUICC STATUS CODE DETAILS: %d\n", p->uicc_status_code_fail_details);
    printf("#\tUICC STATUS WORD (1,2) DETAILS: (%d,%d)\n", p->status_word.sw1, p->status_word.sw2);
    printf("#\tNumber of Records=%d\n", p->number_of_records);
    printf("\n");

    if (p->uicc_status_code == STE_UICC_STATUS_CODE_OK && p->number_of_records == 0) {
        printf("#\t    NO valid MSISDN record on SIM card\n");
    } else {

        tmp_p = p->record_data_p;

        for (i = 0; i < p->number_of_records; i++) {
            printf("#\tMSISDN Record Nr: %d\n", (i + 1));
            printf("#\t    Alpha = %s\n", tmp_p->alpha_p);
            printf("#\t    Number = %s\n", tmp_p->number_p);

            /* Add type to output */
            if (tmp_p->type <= 7) {
                printf("#\t    Type = 0x%X\n", tmp_p->type);
            } else {
                printf("#\t    Type = \n");
            }

            /* Add speed to output */
            if (tmp_p->speed <= 134) {
                printf("#\t    Speed = 0x%X\n", tmp_p->speed);
            } else {
                printf("#\t    Speed = \n");
            }

            /* Add service to output */
            if (tmp_p->service <= 5) {
                printf("#\t    Service = 0x%X\n", tmp_p->service);
            } else {
                printf("#\t    Service = \n");
            }

            /* Add itc to output */
            if (tmp_p->itc <= 1) {
                printf("#\t    ITC = 0x%X\n", tmp_p->itc);
            } else {
                printf("#\t    ITC = \n");
            }

            printf("\n");

            tmp_p++;
        }
    }

    tmp_p = p->record_data_p;

    for (i = 0; i < p->number_of_records; i++) {
        free(tmp_p->alpha_p);
        free(tmp_p->number_p);
        tmp_p++;
    }

    free(p->record_data_p);

    sync_data_delete(sd);

    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);

    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}

const char             *test_read_fdn()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t      pc;
    cat_barrier_t           *bar = cat_barrier_new();


    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);

    do {
      int         test_i, i;
        sync_data_t *sd = 0;
        ste_uicc_sim_fdn_response_t *p;
        ste_uicc_sim_fdn_record_t *f;

        //  Correct values for test with MSL and MAL
        cat_barrier_set(bar);

        test_i = ste_uicc_sim_file_read_fdn(uicc,
                TUICC_CT_STE_UICC_CAUSE_REQ_SIM_FILE_READ_FDN);

        if ( test_i )
            return  "ste_uicc_sim_file_read_fdn failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 1000);
        if ( test_i )
            return "ste_uicc_sim_file_read_fdn timed out";

        if( !sd || sd->cause != STE_UICC_CAUSE_REQ_SIM_FILE_READ_FDN)
            return "ste_uicc_sim_file_read_fdn wrong response";

        p = (ste_uicc_sim_fdn_response_t *)(sd->data);
        printf("#\tUICC STATUS CODE = %d hex:\n", (int)p->uicc_status_code);
        printf("#\tUICC STATUS CODE DETAILS = %d hex:\n", (int)p->uicc_status_code_fail_details);
        printf("#\tSTATUS WORD 1 = %d hex:\n", (int)p->status_word.sw1);
        printf("#\tSTATUS WORD 2 = %d hex:\n", (int)p->status_word.sw2);
        printf("#\tREAD LENGTH = %d hex:\n", (int)p->number_of_records);
        f = p->fdn_record_p;
        for (i=0;i<(int)p->number_of_records;i++) {
          printf("\n#\tDial 'string'(size): '%.*s'(%d)\n", (int)f[i].dial_string.no_of_characters, (char *)f[i].dial_string.text_p, (int)f[i].dial_string.no_of_characters);
          printf("#\tAlpha 'string'(size): '%.*s'(%d)\n", (int)f[i].alpha_string.no_of_characters, (char *)f[i].alpha_string.text_p, (int)f[i].alpha_string.no_of_characters);
          printf("#\tTON: %x NPI: %x\n", f[i].ton, f[i].npi);
        }
        for (i=0;i<(int)p->number_of_records;i++) {
          free(f[i].dial_string.text_p);
          free(f[i].alpha_string.text_p);
        }
        free(f);
        sync_data_delete(sd);
    } while(0);


    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}


const char             *test_read_ecc()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t      pc;
    cat_barrier_t           *bar = cat_barrier_new();


    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);

    do {
      int         test_i, i;
        sync_data_t *sd = 0;

        ste_uicc_sim_ecc_response_t *p;
        ste_uicc_sim_ecc_number_t *e;

        //  Correct values for test with MSL and MAL
        cat_barrier_set(bar);

        test_i = ste_uicc_sim_file_read_ecc(uicc,
                TUICC_CT_STE_UICC_CAUSE_REQ_SIM_FILE_READ_ECC);

        if ( test_i )
            return  "ste_uicc_sim_file_read_ecc failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 1000);
        if ( test_i )
            return "ste_uicc_sim_file_read_ecc timed out";

        if( !sd || sd->cause != STE_UICC_CAUSE_REQ_SIM_FILE_READ_ECC)
            return "ste_uicc_sim_file_read_ecc wrong response";

        p = (ste_uicc_sim_ecc_response_t *)(sd->data);
        printf("#\tUICC STATUS CODE = %d hex:\n", (int)p->uicc_status_code);
        printf("#\tUICC STATUS CODE DETAILS = %d hex:\n", (int)p->uicc_status_code_fail_details);
        printf("#\tSTATUS WORD 1 = %d hex:\n", (int)p->status_word.sw1);
        printf("#\tSTATUS WORD 2 = %d hex:\n", (int)p->status_word.sw2);
        printf("#\tREAD LENGTH = %d hex:\n", (int)p->number_of_records);

        e = p->ecc_number_p;
        for (i=0;i<(int)p->number_of_records;i++) {
          printf("#\tNumber: %s Category: %x Alpha: ", e[i].number, (unsigned int)e[i].category);
          print_data(e[i].alpha, e[i].length);
        }
        for (i=0;i<(int)p->number_of_records;i++)
          free(e[i].alpha);
        free(e);
        sync_data_delete(sd);
    } while(0);


    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}



const char             *test_read_generic()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t      pc;
    cat_barrier_t           *bar = cat_barrier_new();


    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    sleep(1);

    do {
      int         test_i, i;
        sync_data_t *sd = 0;

        ste_uicc_sim_ecc_response_t *p;
        ste_uicc_sim_ecc_number_t *e;

        //  Correct values for test with MSL and MAL
        cat_barrier_set(bar);

        test_i = ste_uicc_sim_file_read_generic(uicc,
                TUICC_CT_STE_UICC_CAUSE_REQ_SIM_FILE_READ_GENERIC,0x7FFF,NULL,0);

        if ( test_i )
            return  "ste_uicc_sim_file_read_generic failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 1000);
        if ( test_i )
            return "ste_uicc_sim_file_read_ecc timed out";

        if( !sd || sd->cause != STE_UICC_CAUSE_REQ_SIM_FILE_READ_ECC)
            return "ste_uicc_sim_file_read_ecc wrong response";

        /* TODO: Print response data */
        sync_data_delete(sd);

    } while(0);


    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}

const char             *test_read_fdn_sync()
{
    ste_sim_t         *uicc;
    ste_sim_closure_t  pc;
    int i;
    ste_uicc_sim_fdn_response_t p;
    ste_uicc_sim_fdn_record_t *f;

    pc.func = the_callback_sync;
    pc.user_data = 0;

    uicc = ste_sim_new_st(&pc);
    if (!uicc)
        return "ste_sim_new failed.";
    i = ste_sim_connect(uicc, TUICC_CT_STE_SIM_CAUSE_CONNECT);
    if (i)
        return "ste_uicc_sim_file_read_fdn_sync: connect failed.";
    i = ste_uicc_sim_file_read_fdn_sync(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_FILE_READ_FDN, &p);
    if (i)
      return "ste_uicc_sim_file_read_fdn_sync: connect failed.";
    printf("#\tUICC STATUS CODE = %d hex:\n", (int)p.uicc_status_code);
    printf("#\tUICC STATUS CODE DETAILS = %d hex:\n", (int)p.uicc_status_code_fail_details);
    printf("#\tSTATUS WORD 1 = %d hex:\n", (int)p.status_word.sw1);
    printf("#\tSTATUS WORD 2 = %d hex:\n", (int)p.status_word.sw2);
    printf("#\tREAD LENGTH = %d hex:\n", (int)p.number_of_records);
    f = p.fdn_record_p;
    for (i=0;i<(int)p.number_of_records;i++) {
      printf("\n#\tDial 'string'(size): '%.*s'(%d)\n", (int)f[i].dial_string.no_of_characters, (char *)f[i].dial_string.text_p, (int)f[i].dial_string.no_of_characters);
      printf("#\tAlpha 'string'(size): '%.*s'(%d)\n", (int)f[i].alpha_string.no_of_characters, (char *)f[i].alpha_string.text_p, (int)f[i].alpha_string.no_of_characters);
      printf("#\tTON: %x NPI: %x\n", f[i].ton, f[i].npi);
    }
    for (i=0;i<(int)p.number_of_records;i++) {
      free(f[i].dial_string.text_p);
      free(f[i].alpha_string.text_p);
    }
    free(f);
    sleep(1);
    i = ste_sim_disconnect(uicc, TUICC_CT_STE_SIM_CAUSE_DISCONNECT);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    if (i)
      return "ste_uicc_sim_file_read_fdn_sync: disconnect failed";
    sleep(1);
    return 0;
}

const char             *test_read_ecc_sync()
{
    ste_sim_t *uicc;
    ste_sim_closure_t pc;
    int i;
    ste_uicc_sim_ecc_response_t p;
    ste_uicc_sim_ecc_number_t *e;

    pc.func = the_callback_sync;
    pc.user_data = 0;

    uicc = ste_sim_new_st(&pc);
    if (!uicc)
        return "ste_sim_new failed.";
    i = ste_sim_connect(uicc, TUICC_CT_STE_SIM_CAUSE_CONNECT);
    if (i)
        return "ste_uicc_sim_file_read_ecc_sync: connect failed.";
    i = ste_uicc_sim_file_read_ecc_sync(uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_FILE_READ_ECC, &p);

    if (i)
      return "ste_uicc_sim_file_read_ecc_sync: connect failed.";

    printf("#\tUICC STATUS CODE = %d hex:\n", (int)p.uicc_status_code);
    printf("#\tUICC STATUS CODE DETAILS = %d hex:\n", (int)p.uicc_status_code_fail_details);
    printf("#\tSTATUS WORD 1 = %d hex:\n", (int)p.status_word.sw1);
    printf("#\tSTATUS WORD 2 = %d hex:\n", (int)p.status_word.sw2);
    printf("#\tREAD LENGTH = %d hex:\n", (int)p.number_of_records);
    e = p.ecc_number_p;
    for (i=0;i<(int)p.number_of_records;i++) {
      printf("#\tNumber: %s Category: %x Alpha: ", e[i].number, (unsigned int)e[i].category);
      print_data(e[i].alpha, e[i].length);
    }
    for (i=0;i<(int)p.number_of_records;i++)
      free(e[i].alpha);
    free(e);
    sleep(1);
    i = ste_sim_disconnect(uicc, TUICC_CT_STE_SIM_CAUSE_DISCONNECT);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    if (i)
      return "ste_uicc_sim_file_read_ecc_sync: disconnect failed";
    sleep(1);
    return 0;
}

const char             *test_reset()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t      pc;
    cat_barrier_t           *bar = cat_barrier_new();

    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    do {
        int         test_i;
        sync_data_t *sd = 0;

        ste_uicc_sim_reset_response_t *p;

        //  Correct values for test with MSL and MAL
        cat_barrier_set(bar);

        test_i = ste_uicc_sim_reset(uicc,
                                    TUICC_CT_STE_UICC_CAUSE_REQ_RESET);

        if ( test_i )
            return  "test_reset failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 1000);
        if ( test_i )
            return "test_reset timed out";

        if( !sd || sd->cause != STE_UICC_CAUSE_REQ_SIM_RESET)
            return "ste_uicc_sim_reset wrong response";

        p = (ste_uicc_sim_reset_response_t *)(sd->data);
        printf("#\tUICC STATUS CODE = %d hex:\n", (int)p->uicc_status_code);
        printf("#\tUICC STATUS CODE DETAILS = %d hex:\n", (int)p->uicc_status_code_fail_details);
        printf("#\tSTATUS WORD 1 = %d hex:\n", (int)p->status_word.sw1);
        printf("#\tSTATUS WORD 2 = %d hex:\n", (int)p->status_word.sw2);

        sync_data_delete(sd);
    } while(0);


    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}


const char             *test_sim_power_on()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t       pc;
    cat_barrier_t          *bar = cat_barrier_new();

    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    do {
        int         test_i;
        sync_data_t *sd = 0;

        ste_uicc_sim_power_on_response_t *p;

        //  Correct values for test with MSL and MAL
        cat_barrier_set(bar);

        test_i = ste_uicc_sim_power_on(uicc,
                                       TUICC_CT_STE_UICC_CAUSE_REQ_SIM_POWER_ON);

        if ( test_i )
            return  "test_sim_power_on failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 1000);
        if ( test_i )
            return "test_sim_power_on timed out";

        if( !sd || sd->cause != STE_UICC_CAUSE_REQ_SIM_POWER_ON)
            return "ste_uicc_sim_power_on wrong response";

        p = (ste_uicc_sim_power_on_response_t *)(sd->data);
        printf("#\tUICC STATUS CODE = %d hex:\n", (int)p->uicc_status_code);
        printf("#\tUICC STATUS CODE DETAILS = %d hex:\n", (int)p->uicc_status_code_fail_details);
        printf("#\tSTATUS WORD 1 = %d hex:\n", (int)p->status_word.sw1);
        printf("#\tSTATUS WORD 2 = %d hex:\n", (int)p->status_word.sw2);

        sync_data_delete(sd);
    } while(0);


    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}


const char             *test_sim_power_off()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t       pc;
    cat_barrier_t          *bar = cat_barrier_new();

    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";

    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    do {
        int         test_i;
        sync_data_t *sd = 0;

        ste_uicc_sim_power_off_response_t *p;

        //  Correct values for test with MSL and MAL
        cat_barrier_set(bar);

        test_i = ste_uicc_sim_power_off(uicc,
                                        TUICC_CT_STE_UICC_CAUSE_REQ_SIM_POWER_OFF);

        if ( test_i )
            return  "test_sim_power_off failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 1000);
        if ( test_i )
            return "test_sim_power_off timed out";

        if( !sd || sd->cause != STE_UICC_CAUSE_REQ_SIM_POWER_OFF)
            return "ste_uicc_sim_power_off wrong response";

        p = (ste_uicc_sim_power_off_response_t *)(sd->data);
        printf("#\tUICC STATUS CODE = %d hex:\n", (int)p->uicc_status_code);
        printf("#\tUICC STATUS CODE DETAILS = %d hex:\n", (int)p->uicc_status_code_fail_details);
        printf("#\tSTATUS WORD 1 = %d hex:\n", (int)p->status_word.sw1);
        printf("#\tSTATUS WORD 2 = %d hex:\n", (int)p->status_word.sw2);

        sync_data_delete(sd);
    } while(0);

    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}

const char             *test_read_preferred_RAT_setting()
{
    ste_sim_t              *uicc;
    ste_sim_closure_t       pc;
    cat_barrier_t          *bar = cat_barrier_new();

    pc.func = the_callback;
    pc.user_data = bar;

    uicc = ste_sim_new(&pc);
    if (!uicc)
        return "ste_sim_new failed.";

    TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

    do {
        int         test_i;
        sync_data_t *sd = 0;

        ste_uicc_sim_read_preferred_RAT_setting_response_t *p;

        //  Correct values for test with MSL and MAL
        cat_barrier_set(bar);

        test_i = ste_uicc_sim_read_preferred_RAT_setting(uicc,
                                                         TUICC_CT_STE_UICC_CAUSE_REQ_SIM_READ_PREFERRED_RAT_SETTING);

        if ( test_i )
            return  "ste_uicc_sim_read_preferred_RAT_setting failed.";

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 1000);
        if ( test_i )
            return "ste_uicc_sim_read_preferred_RAT_setting timed out";

        if( !sd || sd->cause != STE_UICC_CAUSE_REQ_SIM_READ_PREFERRED_RAT_SETTING)
            return "ste_uicc_sim_read_preferred_RAT_setting wrong response";

        p = (ste_uicc_sim_read_preferred_RAT_setting_response_t *)(sd->data);
        printf("#\tUICC STATUS CODE = %d hex:\n", (int)p->uicc_status_code);
        printf("#\tUICC STATUS CODE DETAILS = %d hex:\n", (int)p->uicc_status_code_fail_details);
        printf("# RAT setting = %d\n", (int)p->RAT);
        sync_data_delete(sd);
    } while(0);

    sleep(1);

    TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}

const char             *test_invalid_input_scenario( ste_sim_t *connected_uicc, cat_barrier_t* bar )
{
  ste_sim_closure_t       pc = { the_callback, bar };
  ste_sim_t              *uicc = ste_sim_new(&pc);
  if (!uicc)
      return "ste_sim_new failed.";

  if ( /* UICC NULL */
       ( ste_uicc_register( NULL, TUICC_CT_STE_UICC_CAUSE_REGISTER )
       != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not connected */
       ( ste_uicc_register( uicc, TUICC_CT_STE_UICC_CAUSE_REGISTER )
       != UICC_REQUEST_STATUS_FAILED_STATE )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_register failed";
  }

  if  ( /* UICC NULL */
        ( ste_uicc_pin_verify( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_VERIFY,
          SIM_PIN_ID_PIN1, "1111") != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
        /* UICC Not connected */
        ( ste_uicc_pin_verify( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_VERIFY,
          SIM_PIN_ID_PIN1, "1111") != UICC_REQUEST_STATUS_FAILED_STATE ) ||
        /* PIN 1 NULL */
        ( ste_uicc_pin_verify( connected_uicc, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_VERIFY,
          SIM_PIN_ID_PIN1, NULL) != UICC_REQUEST_STATUS_FAILED_PARAMETER )
      ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_pin_verify failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_pin_disable( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_DISABLE,
         "1111") != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* Null input buffer */
       ( ste_uicc_pin_disable( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_DISABLE,
         NULL) != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not Connected */
       ( ste_uicc_pin_disable( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_DISABLE,
         "1111") != UICC_REQUEST_STATUS_FAILED_STATE )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_pin_disable failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_pin_enable( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_ENABLE,
         "1111") != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not connected */
       ( ste_uicc_pin_enable( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_ENABLE,
         "1111") != UICC_REQUEST_STATUS_FAILED_STATE ) ||
       /* Null input Buffer */
       ( ste_uicc_pin_enable( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_ENABLE,
         NULL) != UICC_REQUEST_STATUS_FAILED_PARAMETER )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_pin_enable failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_pin_info( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_INFO, SIM_PIN_PIN1)
         != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* Invalid PIN ID */
       ( ste_uicc_pin_info( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_INFO, 0xFF)
         != UICC_REQUEST_STATUS_FAILED_PARAMETER )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_pin_info failed";
  }

  if ( /* UICC NULL */
       (ste_uicc_pin_change( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_CHANGE, "1111",
        "2345", SIM_PIN_ID_PIN1) != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* Null OLD PIN */
       (ste_uicc_pin_change( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_CHANGE, NULL,
        "2345", SIM_PIN_ID_PIN1) != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* Null NEW PIN */
       (ste_uicc_pin_change( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_CHANGE, "1111",
        NULL, SIM_PIN_ID_PIN1) != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* Invalid New PIN */
       (ste_uicc_pin_change( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_CHANGE, "1111",
        "abcd", SIM_PIN_ID_PIN1) != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not Connected */
       (ste_uicc_pin_change( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_CHANGE, "1111",
        "2345", SIM_PIN_ID_PIN1) != UICC_REQUEST_STATUS_FAILED_STATE ) ||
       /* Invalid PIN ID */
       (ste_uicc_pin_change( connected_uicc, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_CHANGE, "1111",
        "2345", 0xFF) != UICC_REQUEST_STATUS_FAILED_PARAMETER )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_pin_change failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_pin_unblock( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_UNBLOCK,
         SIM_PIN_ID_PIN1, "1234", "11111111") != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* Null PIN */
       ( ste_uicc_pin_unblock( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_UNBLOCK,
         SIM_PIN_ID_PIN1, NULL, "11111111") != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* Invalid PIN */
       ( ste_uicc_pin_unblock( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_UNBLOCK,
         SIM_PIN_ID_PIN1, "abcd", "11111111") != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* NULL PUK */
       ( ste_uicc_pin_unblock( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_UNBLOCK,
         SIM_PIN_ID_PIN1, "1234", NULL) != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not connected */
       ( ste_uicc_pin_unblock( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_PIN_UNBLOCK,
         SIM_PIN_ID_PIN1, "1234", "11111111") != UICC_REQUEST_STATUS_FAILED_STATE )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_pin_unblock failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_sim_file_get_format( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_FILE_GET_FORMAT,
         0xDEAD, NULL) != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not Connected */
       ( ste_uicc_sim_file_get_format( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_FILE_GET_FORMAT,
         0xDEAD, NULL) != UICC_REQUEST_STATUS_FAILED_STATE )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_sim_file_get_format failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_sim_file_read_record( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_READ_SIM_FILE_RECORD,
         0x6F3C, 1, 0, uicc_sim ? "3f007f10" : "3f007fff") != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not Connected */
       ( ste_uicc_sim_file_read_record( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_READ_SIM_FILE_RECORD,
         0x6F3C, 1, 0, uicc_sim ? "3f007f10" : "3f007fff") != UICC_REQUEST_STATUS_FAILED_STATE )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_sim_file_read_record failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_sim_file_read_binary( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_READ_SIM_FILE_BINARY,
         0x2f05, 0, 0, "3f00") != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not Connected */
      ( ste_uicc_sim_file_read_binary( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_READ_SIM_FILE_BINARY,
         0x2f05, 0, 0, "3f00") != UICC_REQUEST_STATUS_FAILED_STATE )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_sim_file_read_binary failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_sim_file_update_binary( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_UPDATE_SIM_FILE_BINARY,
       0x2f05, 2, 2, "3f00", NULL) != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not Connected */
       ( ste_uicc_sim_file_update_binary( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_UPDATE_SIM_FILE_BINARY,
       0x2f05, 2, 2, "3f00", NULL) != UICC_REQUEST_STATUS_FAILED_STATE ) ||
       /* Invalid Data */
       ( ste_uicc_sim_file_update_binary( connected_uicc, TUICC_CT_STE_UICC_CAUSE_REQ_UPDATE_SIM_FILE_BINARY,
       0x2f05, 2, 2, "3f00", NULL) != UICC_REQUEST_STATUS_FAILED_PARAMETER )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_sim_file_update_binary failed";
  }

  if ( /* UICC NULL */
      ( ste_uicc_sim_file_update_record( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_UPDATE_SIM_FILE_RECORD,
       0x6f3c, 1, 176, uicc_sim ? "3F007F10" : "3F007FFF", NULL ) != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not Connected */
       ( ste_uicc_sim_file_update_record( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_UPDATE_SIM_FILE_RECORD,
       0x6f3c, 1, 176, uicc_sim ? "3F007F10" : "3F007FFF", NULL ) != UICC_REQUEST_STATUS_FAILED_STATE ) ||
       /* Invalid Data */
       ( ste_uicc_sim_file_update_record( connected_uicc, TUICC_CT_STE_UICC_CAUSE_REQ_UPDATE_SIM_FILE_RECORD,
       0x6f3c, 1, 176, uicc_sim ? "3F007F10" : "3F007FFF", NULL ) != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* Invalid Len */
       ( ste_uicc_sim_file_update_record( connected_uicc, TUICC_CT_STE_UICC_CAUSE_REQ_UPDATE_SIM_FILE_RECORD,
       0x6f3c, 1, 300, uicc_sim ? "3F007F10" : "3F007FFF", NULL ) != UICC_REQUEST_STATUS_FAILED_PARAMETER )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_sim_file_update_record failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_sim_get_file_information( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_GET_FILE_INFORMATION,
       0x3f00, "", STE_UICC_SIM_GET_FILE_INFO_TYPE_DF ) != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not Connected */
       ( ste_uicc_sim_get_file_information( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_GET_FILE_INFORMATION,
       0x3f00, "", STE_UICC_SIM_GET_FILE_INFO_TYPE_DF ) != UICC_REQUEST_STATUS_FAILED_STATE )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_sim_get_file_information failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_sim_get_state( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_GET_SIM_STATE)
       != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not Connected */
       ( ste_uicc_sim_get_state( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_GET_SIM_STATE)
       != UICC_REQUEST_STATUS_FAILED_STATE )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_sim_get_state failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_get_app_info( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_APP_INFO )
       != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not Connected */
       ( ste_uicc_get_app_info( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_APP_INFO )
       != UICC_REQUEST_STATUS_FAILED_STATE )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_get_app_info failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_sim_smsc_get_active( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_SMSC_GET_ACTIVE )
       != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not Connected */
       ( ste_uicc_sim_smsc_get_active( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SMSC_GET_ACTIVE )
       != UICC_REQUEST_STATUS_FAILED_STATE )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_sim_smsc_get_active failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_sim_smsc_set_active( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_SMSC_SET_ACTIVE,
       NULL ) != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not Connected */
       ( ste_uicc_sim_smsc_set_active( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SMSC_SET_ACTIVE,
       NULL ) != UICC_REQUEST_STATUS_FAILED_STATE ) ||
       /* NULL SMSC */
       ( ste_uicc_sim_smsc_set_active( connected_uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SMSC_SET_ACTIVE,
       NULL ) != UICC_REQUEST_STATUS_FAILED_PARAMETER )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_sim_smsc_set_active failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_sim_smsc_get_record_max( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_SMSC_GET_RECORD_MAX )
       != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not Connected */
       ( ste_uicc_sim_smsc_get_record_max( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SMSC_GET_RECORD_MAX )
       != UICC_REQUEST_STATUS_FAILED_STATE )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_sim_smsc_get_record_max failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_sim_smsc_save_to_record( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_SMSC_SAVE_TO_RECORD,
       1) != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not Connected */
       ( ste_uicc_sim_smsc_save_to_record( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SMSC_SAVE_TO_RECORD,
       1) != UICC_REQUEST_STATUS_FAILED_STATE )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_sim_smsc_save_to_record failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_sim_smsc_restore_from_record( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_SMSC_RESTORE_FROM_RECORD,
       1) != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not Connected */
       ( ste_uicc_sim_smsc_restore_from_record( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SMSC_RESTORE_FROM_RECORD,
       1) != UICC_REQUEST_STATUS_FAILED_STATE )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_sim_smsc_restore_from_record failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_sim_file_read_generic( NULL, TUICC_CT_STE_SIM_CAUSE_NOOP,
       0, NULL, 0) != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not Connected */
       ( ste_uicc_sim_file_read_generic( uicc, TUICC_CT_STE_SIM_CAUSE_NOOP,
       0, NULL, 0) != UICC_REQUEST_STATUS_FAILED_STATE )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_sim_file_read_generic failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_sim_file_read_imsi( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_FILE_READ_IMSI )
       != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not Connected */
       ( ste_uicc_sim_file_read_imsi( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_FILE_READ_IMSI )
       != UICC_REQUEST_STATUS_FAILED_STATE )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_sim_file_read_imsi failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_sim_file_read_plmn( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_FILE_READ_PLMN_SEL,
       SIM_FILE_ID_PLMN_SEL) != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not Connected */
       ( ste_uicc_sim_file_read_plmn( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_FILE_READ_PLMN_SEL,
       SIM_FILE_ID_PLMN_SEL) != UICC_REQUEST_STATUS_FAILED_STATE )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_sim_file_read_plmn failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_sim_file_update_plmn( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_FILE_READ_PLMN,
       0, NULL, 0) != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not Connected */
       ( ste_uicc_sim_file_update_plmn( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_FILE_READ_PLMN,
       0, NULL, 0) != UICC_REQUEST_STATUS_FAILED_STATE ) ||
       /* Invalid PILM */
       ( ste_uicc_sim_file_update_plmn( connected_uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_FILE_READ_PLMN,
       -1, NULL, 0) != UICC_REQUEST_STATUS_FAILED_PARAMETER )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_sim_file_update_plmn failed";
  }

  if ( /* UICC NULL, PIN NULL */
       ( ste_uicc_update_service_table( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_UPDATE_SERVICE_TABLE,
       NULL, SIM_SERVICE_TYPE_FDN, 1) != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* Invalid PIN len */
       ( ste_uicc_update_service_table( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_UPDATE_SERVICE_TABLE,
       "56785678567856781", SIM_SERVICE_TYPE_FDN, 1) != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UIC Not Connected */
       ( ste_uicc_update_service_table( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_UPDATE_SERVICE_TABLE,
       "5678", SIM_SERVICE_TYPE_FDN, 1) != UICC_REQUEST_STATUS_FAILED_STATE )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_update_service_table failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_get_service_table( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_GET_SERVICE_TABLE,
       SIM_SERVICE_TYPE_FDN ) != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not Connected */
       ( ste_uicc_get_service_table( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_GET_SERVICE_TABLE,
       SIM_SERVICE_TYPE_FDN ) != UICC_REQUEST_STATUS_FAILED_STATE )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_get_service_table failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_get_service_availability( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY,
       SIM_SERVICE_TYPE_FDN) != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not COnnected */
       ( ste_uicc_get_service_availability( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY,
       SIM_SERVICE_TYPE_FDN) != UICC_REQUEST_STATUS_FAILED_STATE )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_get_service_availability failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_sim_icon_read( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_ICON,
       0x4f20, 0, 0) != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not Connected */
       ( ste_uicc_sim_icon_read( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_ICON,
       0x4f20, 0, 0) != UICC_REQUEST_STATUS_FAILED_STATE )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_sim_icon_read failed";
  }

  if ( /* UICC NULL */
       (ste_uicc_get_subscriber_number( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_SUBSCRIBER_NUMBER )
       != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not Connected */
       (ste_uicc_get_subscriber_number( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SUBSCRIBER_NUMBER )
       != UICC_REQUEST_STATUS_FAILED_STATE )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_get_subscriber_number failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_card_status( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_CARD_STATUS )
       != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not Connected */
       ( ste_uicc_card_status( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_CARD_STATUS )
       != UICC_REQUEST_STATUS_FAILED_STATE )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_card_status failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_app_status( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_APP_STATUS, 0 )
       != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not Connected */
       ( ste_uicc_app_status( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_APP_STATUS, 0 )
       != UICC_REQUEST_STATUS_FAILED_STATE )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_app_statu failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_sim_channel_send( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_CHANNEL_SEND,
       current_open_channel, 0, NULL) != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not Connected */
       ( ste_uicc_sim_channel_send( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_CHANNEL_SEND,
       current_open_channel, 0, NULL) != UICC_REQUEST_STATUS_FAILED_STATE )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_sim_channel_send failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_sim_channel_open( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_CHANNEL_OPEN,
       0, 0 ) != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not Connected */
       ( ste_uicc_sim_channel_open( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_CHANNEL_OPEN,
       0, 0 ) != UICC_REQUEST_STATUS_FAILED_STATE ) ||
       /* AID Invalid */
       ( ste_uicc_sim_channel_open( connected_uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_CHANNEL_OPEN,
       0, 0 ) != UICC_REQUEST_STATUS_FAILED_PARAMETER )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return " ste_uicc_sim_channel_open failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_sim_channel_close( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_CHANNEL_CLOSE,
       0 ) != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not Connected */
       ( ste_uicc_sim_channel_close( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_CHANNEL_CLOSE,
       0 ) != UICC_REQUEST_STATUS_FAILED_STATE )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_sim_channel_close failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_sim_file_read_fdn( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_FILE_READ_FDN )
       != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not Connected */
       ( ste_uicc_sim_file_read_fdn( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_FILE_READ_FDN )
       != UICC_REQUEST_STATUS_FAILED_STATE )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_sim_file_read_fdn failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_sim_file_read_ecc( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_FILE_READ_ECC )
       != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not Connected */
       ( ste_uicc_sim_file_read_ecc( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_FILE_READ_ECC )
       != UICC_REQUEST_STATUS_FAILED_STATE )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_sim_file_read_ecc failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_sim_reset( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_RESET )
       != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not Connected */
       ( ste_uicc_sim_reset( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_RESET )
       != UICC_REQUEST_STATUS_FAILED_STATE )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_sim_reset failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_sim_power_on( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_POWER_ON )
       != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not COnnected */
       ( ste_uicc_sim_power_on( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_POWER_ON )
       != UICC_REQUEST_STATUS_FAILED_STATE )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_sim_power_on failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_sim_power_off( NULL, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_POWER_OFF )
       != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not COnnected */
       ( ste_uicc_sim_power_off( uicc, TUICC_CT_STE_UICC_CAUSE_REQ_SIM_POWER_OFF )
       != UICC_REQUEST_STATUS_FAILED_STATE )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_sim_power_off failed";
  }

  if ( /* UICC NULL */
       ( ste_uicc_sim_read_preferred_RAT_setting( NULL,
       TUICC_CT_STE_UICC_CAUSE_REQ_SIM_READ_PREFERRED_RAT_SETTING ) != UICC_REQUEST_STATUS_FAILED_PARAMETER ) ||
       /* UICC Not Connected */
       ( ste_uicc_sim_read_preferred_RAT_setting( uicc,
       TUICC_CT_STE_UICC_CAUSE_REQ_SIM_READ_PREFERRED_RAT_SETTING ) != UICC_REQUEST_STATUS_FAILED_STATE )
     ) {
            ste_sim_delete(uicc, TUICC_CT_NOT_USED);
            return "ste_uicc_sim_read_preferred_RAT_setting failed";
  }

  ste_sim_delete(uicc, TUICC_CT_NOT_USED);
  return NULL;
}
const char             *test_for_coverage_negative_scenarios()
{
  ste_sim_t              *uicc;
  ste_sim_closure_t       pc;
  cat_barrier_t          *bar = cat_barrier_new();
  const char             *ret_val = NULL;
  pc.func = the_callback;
  pc.user_data = bar;
  uicc = ste_sim_new(&pc);
  if (!uicc)
      return "ste_sim_new failed.";
  TTEST(ste_sim_connect, TUICC_CT_STE_SIM_CAUSE_CONNECT, uicc, bar, 100);

  ret_val = test_invalid_input_scenario(uicc, bar);
  if ( ret_val != NULL )
  {
      return ret_val;
  }
  TTEST(ste_sim_disconnect, TUICC_CT_STE_SIM_CAUSE_DISCONNECT, uicc, bar, 100);
  ste_sim_delete(uicc, TUICC_CT_NOT_USED);
  cat_barrier_delete(bar);
  sleep(1);
  return 0;

}

typedef const char     *test_func_t();

static const char *help ();

/**
 * Table of mapping test names to test functions.
 * Just expand the list with more items in no particular order.
 */
struct test_entry {
    char                   *test_name;
    test_func_t            *test_func;
};

// The following define is just to fool indent not to produce odd indentation!
#define TENTRY(N,F) {N,F}
static const struct test_entry tests[] = {
    TENTRY("condis", test_connectdisconnect),
    TENTRY("select", test_sim_select),
    TENTRY("selectRelative", test_sim_select_relative),
    TENTRY("ping", test_ping),
    TENTRY("ping_sync", test_ping_sync),
#if 0 // We do not need the register test case for an active client
    TENTRY("register", test_register),
#endif
    TENTRY("connect10", test_connect10),
    TENTRY("shutdown", test_shutdown),
    TENTRY("sendPIN", test_sendPIN),
    TENTRY("changePIN", test_changePIN),
    TENTRY("disablePIN", test_disablePIN),
    TENTRY("enablePIN", test_enablePIN),
    TENTRY("PINinfo", test_requestPIN_info),
    TENTRY("sendPUK", test_sendPUK),
    TENTRY("readSIM_FILE_record", test_readSIM_FILE_record),
    TENTRY("readSIM_FILE_binary", test_readSIM_FILE_binary),
    TENTRY("updateSIM_FILE_record", test_updateSIM_FILE_record),
    TENTRY("updateSIM_FILE_binary", test_updateSIM_FILE_binary),
    TENTRY("simFILEGETFORMAT", test_simGETFILEFORMAT),
    TENTRY("getFILEINFORMATION", test_getFILEINFORMATION),
    TENTRY("getSIMSTATE", test_getSIMSTATE),
    TENTRY("getSIMSTATE_sync", test_getSIMSTATE_sync),
    TENTRY("passive", test_passive),
    TENTRY("rb", test_readSIM_FILE_binary),
    TENTRY("rr", test_readSIM_FILE_record),
    TENTRY("ub", test_updateSIM_FILE_binary),
    TENTRY("ur", test_updateSIM_FILE_record),
    TENTRY("readSIM_FILE_record_sync", test_readSIM_FILE_record_sync),
    TENTRY("readSIM_FILE_binary_sync", test_readSIM_FILE_binary_sync),
    TENTRY("updateSIM_FILE_record_sync", test_updateSIM_FILE_record_sync),
    TENTRY("updateSIM_FILE_binary_sync", test_updateSIM_FILE_binary_sync),
    TENTRY("simFILEGETFORMAT_sync", test_simGETFILEFORMAT_sync),
    TENTRY("getFILEINFORMATION_sync", test_getFILEINFORMATION_sync),
    TENTRY("readREADIMSI_sync", test_readREADIMSI_sync),
    TENTRY("readIMSI", test_readIMSI),
    TENTRY("getSMSC", test_getSMSC),
    TENTRY("setSMSC", test_setSMSC),
    TENTRY("setSMSC_sync", test_setSMSC_sync),
    TENTRY("getSMSC_sync", test_getSMSC_sync),
    TENTRY("SMSCSaveToRecord", test_SMSCSaveToRecord),
    TENTRY("SMSCRestoreFromRecord", test_SMSCRestoreFromRecord),
    TENTRY("SMSCRecordMax", test_SMSCRecordMax),
    TENTRY("updateServiceTable", test_updateServiceTable),
    TENTRY("getServiceTable", test_getServiceTable),
    TENTRY("getServiceTable_sync", test_getServiceTable_sync),
    TENTRY("getServiceAvailability", test_getServiceAvailability),
    TENTRY("getServiceAvailability_sync", test_getServiceAvailability_sync),
    TENTRY("fi", test_getFILEINFORMATION),
    TENTRY("ff", test_simGETFILEFORMAT),
    TENTRY("ai", test_get_app_info),
    TENTRY("preStart", test_preStart),
    TENTRY("startup", test_startup),
    TENTRY("many_files", test_many_files),
    TENTRY("simGetFormatModemError", test_simGetFormatModemError),
    TENTRY("ir", test_icon_read),
    TENTRY("irb", test_icon_read_binary),
    TENTRY("getSubscriberNumber", test_getSubscriberNumber),
    TENTRY("readPLMN", test_readPLMN),
    TENTRY("updatePLMN", test_updatePLMN),
    TENTRY("readPLMNsel", test_readPLMN_sel),
    TENTRY("as", test_app_status),
    TENTRY("cs", test_card_status),
    TENTRY("simChannelSendFail01",test_sim_channel_send_fail_01),
    TENTRY("simChannelSendFail02",test_sim_channel_send_fail_02),
    TENTRY("simChannelSendFail03",test_sim_channel_send_fail_03),
    TENTRY("simChannelSendFail04",test_sim_channel_send_fail_04),
    TENTRY("simChannelSend",test_sim_channel_send),
    TENTRY("simChannelSendFail05",test_sim_channel_send_fail_05),
    TENTRY("simChannelOpenFail01",test_sim_channel_open_fail_01),
    TENTRY("simChannelOpenFail02",test_sim_channel_open_fail_02),
    TENTRY("simChannelOpen",test_sim_channel_open),
    TENTRY("simChannelCloseFail01",test_sim_channel_close_fail_01),
    TENTRY("simChannelCloseFail02",test_sim_channel_close_fail_02),
    TENTRY("simChannelClose",test_sim_channel_close),
    TENTRY("help", help),
    TENTRY("setSimDebugLevel",test_set_sim_debug_level),
    TENTRY("setModemDebugLevel",test_set_modem_debug_level),
    TENTRY("simdbg",test_set_sim_debug_level),
    TENTRY("simmodemdbg",test_set_modem_debug_level),
    TENTRY("fdn",test_read_fdn),
    TENTRY("fdn_sync",test_read_fdn_sync),
    TENTRY("ecc", test_read_ecc),
    TENTRY("ecc_sync", test_read_ecc_sync),
    TENTRY("read_generic", test_read_generic),
    TENTRY("reset", test_reset),
    TENTRY("poweron", test_sim_power_on),
    TENTRY("poweroff", test_sim_power_off),
    TENTRY("read_RAT_setting", test_read_preferred_RAT_setting),
    TENTRY("coverage_negative_scenarios", test_for_coverage_negative_scenarios),
    TENTRY("simloglevel",test_sim_log_level),
    TENTRY("simutils1",test_sim_utils1),
    TENTRY("simutils2",test_sim_utils2),
    TENTRY(0, 0)                /* Terminator */
};

static const char *help ()
{
    int i;
    printf("The following commands are available\n");
    for ( i = 0 ; tests[i].test_name != 0 ; i++ ) {
        printf("%s\n", tests[i].test_name );
    }
    return 0;
}

test_func_t            *find_test_func(const char *name)
{
    int                     i;
    for (i = 0; tests[i].test_name; i++) {
        if (strcmp(name, tests[i].test_name) == 0) {
            return tests[i].test_func;
        }
    }
    return 0;
}

/*
 * Put all test names here in the order you would like them to run when you
 * call ./tuicc with no test names
 */
static const char      *all_tests[] = {
    "condis",
    "select",
    "selectRelative",
    "ping",
    "connect10",
#if 0
    "register",
#endif
    "preStart",
    "sendPIN",
    "changePIN",
    "disablePIN",
    "enablePIN",
    "PINinfo",
    "changePIN",
    "sendPUK",
    "readSIM_FILE_record",
    "readSIM_FILE_binary",
    "updateSIM_FILE_record",
    "updateSIM_FILE_binary",
    "simFILEGETFORMAT",
    "getFILEINFORMATION",
    "getSIMSTATE",
    "getSIMSTATE_sync",
    "getSMSC",
    "setSMSC",
    "setSMSC_sync",
    "getSMSC_sync",
    "SMSCSaveToRecord",
    "SMSCRestoreFromRecord",
    "SMSCRecordMax",
    "readIMSI",
    "ping_sync",
    "readSIM_FILE_record_sync",
    "readSIM_FILE_binary_sync",
    "updateSIM_FILE_record_sync",
    "updateSIM_FILE_binary_sync",
    "simFILEGETFORMAT_sync",
    "getFILEINFORMATION_sync",
    "readREADIMSI_sync",
    "simGetFormatModemError",
    "updateServiceTable",
    "getServiceTable",
    "getServiceTable_sync",
    "getServiceAvailability",
    "getServiceAvailability_sync",
    "readPLMN",
    "updatePLMN",
    "readPLMNsel",
    "cs",
    "as",
    "getSubscriberNumber",
    "simChannelOpen",
    "simChannelSendFail01",
    "simChannelSendFail02",
    "simChannelSendFail03",
    "simChannelSendFail04",
    "simChannelSend",
    "simChannelSendFail05",
    "simChannelClose",
    "setSimDebugLevel",
    "setModemDebugLevel",
    "fdn",
    "fdn_sync",
    "ecc",
    "ecc_sync",
    "read_generic",
    "read_RAT_setting",
    "many_files",
    "ir",
    "irb",
    "ai",
    "reset",
    /* Make sure to run poweron after power off */
    /* also keeping these to the end to be on the safe side. */
    /* TODO: Need further debugging to check why other cases fail,
     * when running after poweron */
    "poweroff",
    "poweron",
    "coverage_negative_scenarios",
    "simloglevel",
    "simutils1",
    "simutils2",
};

void run_tests(int argc, const char *argv[])
{
    int numFailed = 0;
    int numPassed = 0;

    printf("tuicc: entering run_tests\n");

    while (argc) {
        test_func_t            *f = find_test_func(*argv);
        const char             *s;

        if (!f) {
            printf("No such test function: %s\n", *argv);
            exit(EXIT_FAILURE);
        }

        printf("# \n");
        printf("# Starting test %s\n", *argv);
        s = f();
        if (s) {
            numFailed++;
            printf("FAILED : %s : %s\n", *argv, s);
        } else {
            numPassed++;
            printf("OK     : %s\n", *argv);
        }

        --argc;
        ++argv;
    }

    // Don't change the printed text!
    printf("\n########################## SUMMARY ##########################\n");
    printf("TEST CASES EXECUTED: %d\n", numFailed + numPassed);
    printf("PASSED: %d\n", numPassed);
    printf("FAILED: %d\n", numFailed);
}

/* Hammer test not relevant for host testing */
#ifdef CFG_ENABLE_SIM_STRESS_TEST

#define HAMMER_THREADS 5

#ifdef UNUSED
#undef UNUSED
#define UNUSED(T,X) do{T unused_var=(X);(X)=unused_var;}while(0)
#endif

void mrhammer_callback(int cause, uintptr_t client_tag, void *data, void *vuser_data)
{
    UNUSED(int, cause);
    UNUSED(uintptr_t, client_tag);
    UNUSED(void *, data);
    UNUSED(void *, vuser_data);
    printf("%s: Got callback!\n", __func__);
}

int nr_of_threads = 0;

static void * hammer_thread(void *data)
{
    ste_sim_t              *uicc = (ste_sim_t *)data;
    int status;
    int i;

    for (i = 3; i > 0; i--) {
        printf("Starting in %d seconds\n", i);
        sleep(1);
    }

    while (1) {

        time_t seconds;
        long nap_time;

        filerec_t   recInfo;

        // Read first SMS record
        recInfo.file_id = 0x6F3C;
        recInfo.v.rec_id = 1;
        recInfo.length = 0;
        recInfo.pathptr = uicc_sim ? "3F007F10" : "3F007FFF";

        printf("Calling ste_uicc_sim_read_preferred_RAT_setting!\n");

        status = ste_uicc_sim_read_preferred_RAT_setting(uicc,
                                                         0);

        if (status) {
            printf("ste_uicc_sim_read_preferred_RAT_setting failed!\n");
        }

        printf("Calling ste_uicc_sim_file_read_record!\n");
        status = ste_uicc_sim_file_read_record(uicc,
                                               0,
                                               recInfo.file_id,
                                               recInfo.v.rec_id,
                                               recInfo.length,
                                               recInfo.pathptr);

        if (status) {
            printf("ste_uicc_sim_file_read_record failed!\n");
        }

        time(&seconds);
        srand((unsigned int) seconds);

        nap_time = rand() % 500000 + 500000;

        printf("Sleeping for %ld nanoseconds", nap_time);
        usleep( nap_time ); //Sleep for one second
    }

    ste_sim_disconnect(uicc, TUICC_CT_STE_SIM_CAUSE_DISCONNECT);
    ste_sim_delete(uicc, TUICC_CT_NOT_USED);

    return NULL;
}

int get_max_fd(ste_sim_t *uicc[], int len) {
    int max_fd = ste_sim_fd(uicc[0]);
    int i;

    for (i = 1; i < len; i++) {
        max_fd = max_fd > ste_sim_fd(uicc[i]) ? max_fd : ste_sim_fd(uicc[i]);
    }

    return max_fd;
}

static void hammer_time()
{
    pthread_t tid[HAMMER_THREADS];
    int i;
    fd_set                  rset;
    int                     max_fd;
    ste_sim_closure_t       pc;
    ste_sim_t              *uicc[HAMMER_THREADS];
    int status;

    pc.func = mrhammer_callback;
    pc.user_data = 0;

    for (i = 0; i < HAMMER_THREADS; i++) {
        uicc[i] = ste_sim_new_st(&pc);

        if (!uicc[i]) {
            printf("Could not create uicc object for stress test!");
            return;
        }

        status = ste_sim_connect(uicc[i], TUICC_CT_STE_SIM_CAUSE_CONNECT);

        if (status) {
            printf("Could not connect uicc object for stress test!");
            return;
        }
    }

    for (i = 0; i < HAMMER_THREADS; i++) {
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

        printf("Creating thread nr : %d", i);
        status = pthread_create(&tid[i], &attr, hammer_thread, uicc[i]);

        if (status < 0) {
            perror("failed to create thread");
        }
    }

    for (;;) {
        FD_ZERO(&rset);
        max_fd = get_max_fd(uicc, HAMMER_THREADS);

        for (i = 0; i < HAMMER_THREADS; i++) {
            FD_SET(ste_sim_fd(uicc[i]), &rset);
        }

        i = select(max_fd + 1, &rset, 0, 0, NULL);

        if (i < 0) {     /* Error */
            int                     e = errno;
            printf("read : Reader select failed!\n");
            if (e == EBADF) {
                printf("read : Reader select failed EBADF\n");
                break;
            } else if (e == EINTR) {
                printf("read : Reader select failed EINTR\n");
                /* ignore */
            } else if (e == EINVAL) {
                printf("read : Reader select failed EINVAL\n");
                break;
            } else if (e == ENOMEM) {
                printf("read : Reader select failed ENOMEM\n");
                break;
            } else {
                printf("read : Reader select failed UNKNOWN\n");
                break;
            }
        } else {
            for (i = 0; i < HAMMER_THREADS; i++) {
                if (FD_ISSET(ste_sim_fd(uicc[i]), &rset)) {
                    ste_sim_read(uicc[i]);
                }
            }
        }
    }

    for (i = 0; i < HAMMER_THREADS; i++) {
        pthread_join(tid[i], NULL);
    }


    return;
}

#endif // CFG_ENABLE_SIM_STRESS_TEST

// If we put -i as first arg, then it runs in interactive mode, i.e.
// it prompts user for pin and stuff. It is up to each test function to
// be programmed for handling -i.

// Use as follows:
// start simd on target.
// If modem asks for PIN, start tuicc -i respondPIN. Enter PIN code. It verifies. tuicc exits.
// Having verified pin, you are allowed to run other tests like changePIN
// shutdown etc.
//

int main(int argc, const char *argv[])
{
    setProcName(argv[0]);
    setbuf(stdout, 0);
    int mrhammer = 0;

    --argc;
    ++argv;

    if (!argc) {                /* Do all */
        int                     n = sizeof(all_tests) / sizeof(char *);
        run_tests(n, all_tests);
    } else {
        // Consume any options
        while (argc && **argv == '-') {
            if (strcmp(*argv, "-i") == 0) inter = 1;
            else if (strcmp(*argv, "-sim") == 0) uicc_sim = 1;
            else if (strcmp(*argv, "-usim") == 0) uicc_sim = 0;
#ifdef CFG_ENABLE_SIM_STRESS_TEST
            else if (strcmp(*argv, "-mrhammer") == 0) mrhammer = 1;
#endif // CFG_ENABLE_SIM_STRESS_TEST
            argv++;
            argc--;
        }

        if (!mrhammer) {
            // Remaining args should be test case names.
            if (inter) printf("tuicc is interactive\n");
            if (uicc_sim) printf("tuicc assumes uicc with SIM instead of USIM");
            run_tests(argc, argv);
        } else {
#ifdef CFG_ENABLE_SIM_STRESS_TEST
            hammer_time();
#endif
        }
    }
    return EXIT_SUCCESS;
}
