/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : tcat.c
 * Description     : test executable .
 *
 * Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
 *
 */


#include "sim.h"

#include "cat_barrier.h"
#include "apdu.h"
#include "func_trace.h"
#include "tcat.h"
#include "sim_internal.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

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
    "CAT SESSION END IND",
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
    "UICC REQ SIM FILE READ FDN",
    "UICC REQ SIM FILE READ ECC",
    "UICC REQ SIM RESET",
    "UICC REQ SIM POWER ON",
    "UICC REQ SIM POWER OFF",
    "UICC REQ SIM READ PREFERRED RAT SETTING",

};

typedef struct {
    int                     cause;
    void                   *data;
} sync_data_t;

const uint32_t              reg_events = STE_CAT_CLIENT_REG_EVENTS_CAT_STATUS
                                       | STE_CAT_CLIENT_REG_EVENTS_CARD_STATUS
                                       | STE_CAT_CLIENT_REG_EVENTS_GUI_CAPABLE_CLIENT
                                       | STE_CAT_CLIENT_REG_EVENTS_PC
                                       | STE_CAT_CLIENT_REG_EVENTS_PC_NOTIFICATION
                                       | STE_CAT_CLIENT_REG_EVENTS_PC_REFRESH
                                       | STE_CAT_CLIENT_REG_EVENTS_PC_SETUP_CALL_IND
                                       | STE_CAT_CLIENT_REG_EVENTS_SESSION_END_IND
                                       | STE_CAT_CLIENT_REG_EVENTS_CAT_INFO
                                       | STE_CAT_CLIENT_REG_EVENTS_RAT_SETTING_UPDATED;

int inter = 0; // Interactive flag.
int uicc_sim = 0; // Set to non-zero for sim, otherwise usim is assumed

sync_data_t            *sync_data_create(int cause, void *data)
{
    sync_data_t            *p = malloc(sizeof(sync_data_t));
    int                    *status_p = NULL;

    if (!p)
    {
        printf("tcat: sync_data_create memory allocation(sync_data_t) failed. \n");
        return NULL;
    }

    p->cause = cause;
    p->data = data;

    switch ( cause )
    {
        case STE_SIM_CAUSE_NOOP:
        {
        }
        break;

        case STE_SIM_CAUSE_PING:
        {
        }
        break;

        case STE_SIM_CAUSE_CONNECT:
        {
        }
        break;

        case STE_SIM_CAUSE_DISCONNECT:
        {
        }
        break;

        case STE_SIM_CAUSE_HANGUP:
        {
            printf("tcat: sync_data_create, got HANGUP signal. \n");
        }
        break;

        case STE_SIM_CAUSE_SHUTDOWN:
        {
        }
        break;

        case STE_CAT_CAUSE_NOOP:
        {
        }
        break;

        case STE_CAT_CAUSE_DEREGISTER:
        {
            if (data)
            {
                status_p = malloc( sizeof(int) );
                if (status_p)
                {
                    *status_p = *((int*)data);
                    p->data = status_p;
                }
                else
                {
                    free(p);
                    p = NULL;
                    return NULL;
                }
            }
        }
        break;

        case STE_UICC_CAUSE_SIM_STATE_CHANGED:
        {
            ste_uicc_sim_state_changed_t* d;
            d = malloc( sizeof(*d) );
            if (!d)
            {
                free(p);
                p = NULL;
                return NULL;
            }
            memcpy( d, data, sizeof(*d) );
            p->data = d;
        }
        break;
        case STE_UICC_CAUSE_SIM_STATUS:
        {
            sim_reason_t *d;
            d = malloc(sizeof(*d)); // "box" the enum number
            if ( d != NULL ) {
                memcpy( d, data, sizeof(*d));
            }
            p->data = d;
        }
        break;

        case STE_CAT_CAUSE_SESSION_END_IND:
        {
            printf("tcat: sync_data_create, got STE_CAT_CAUSE_SESSION_END_IND. \n");
        }
        break;

        case STE_CAT_CAUSE_REGISTER:
        {
            if (data)
            {
                status_p = malloc( sizeof(int) );
                if (status_p)
                {
                    *status_p = *((int*)data);
                    p->data = status_p;
                }
                else
                {
                    free(p);
                    p = NULL;
                    return NULL;
                }
            }
        }
        break;

        case STE_CAT_CAUSE_ENABLE:
        {
            ste_cat_enable_response_t  *rsp_p;

            if (data)
            {
                rsp_p = malloc( sizeof(*rsp_p) );
                if (rsp_p)
                {
                    memcpy( rsp_p, data, sizeof(*rsp_p) );
                    p->data = rsp_p;
                }
                else
                {
                    free(p);
                    p = NULL;
                    return NULL;
                }
            }
        }
        break;

        case STE_SIM_CAUSE_STARTUP:
        {
            if (data)
            {
                status_p = malloc( sizeof(int) );
                if (status_p)
                {
                    *status_p = *((int*)data);
                    p->data = status_p;
                }
                else
                {
                    free(p);
                    p = NULL;
                    return NULL;
                }
            }
        }
        break;

        case STE_CAT_CAUSE_PC:
        case STE_CAT_CAUSE_PC_NOTIFICATION:
        {
            ste_cat_pc_ind_t       *pc_ind_p;

            if (data)
            {
                pc_ind_p = malloc( sizeof(ste_cat_pc_ind_t) );
                if (!pc_ind_p)
                {
                    free(p);
                    p = NULL;
                    return NULL;
                }
                memcpy( pc_ind_p, data, sizeof(ste_cat_pc_ind_t) );

                pc_ind_p->apdu.buf = malloc(pc_ind_p->apdu.len + 1);
                if (!pc_ind_p->apdu.buf)
                {
                    printf("tcat: sync_data_create memory allocation(pc_ind_p->apdu.buf) failed\n");
                    free(pc_ind_p);
                    pc_ind_p = NULL;
                    free(p);
                    p = NULL;
                    return NULL;
                }
                memset(pc_ind_p->apdu.buf, 0, pc_ind_p->apdu.len + 1);
                memcpy(pc_ind_p->apdu.buf, ((ste_cat_pc_ind_t *)data)->apdu.buf, pc_ind_p->apdu.len);

                p->data = pc_ind_p;
            }
        }
        break;

        case STE_CAT_CAUSE_TR:
        {
            ste_cat_tr_response_t  *tr_rsp_p;

            if (data)
            {
                tr_rsp_p = malloc( sizeof(ste_cat_tr_response_t) );
                if (tr_rsp_p)
                {
                    memcpy( tr_rsp_p, data, sizeof(ste_cat_tr_response_t) );
                    p->data = tr_rsp_p;
                }
                else
                {
                    free(p);
                    p = NULL;
                    return NULL;
                }
            }
        }
        break;

        case STE_CAT_CAUSE_EC:
        {
            ste_cat_ec_response_t  *ec_rsp_p;

            if (data)
            {
                ec_rsp_p = malloc( sizeof(ste_cat_ec_response_t) );
                if (ec_rsp_p)
                {
                    memcpy( ec_rsp_p, data, sizeof(ste_cat_ec_response_t) );
                    ec_rsp_p->apdu.buf = malloc(ec_rsp_p->apdu.len + 1);
                    if (!ec_rsp_p->apdu.buf)
                    {
                        printf("tcat: sync_data_create memory allocation(ec_rsp_p->apdu.buf) failed\n");
                        free(ec_rsp_p);
                        ec_rsp_p = NULL;
                        free(p);
                        p = NULL;
                        return NULL;
                    }
                    memset(ec_rsp_p->apdu.buf, 0, ec_rsp_p->apdu.len + 1);
                    memcpy(ec_rsp_p->apdu.buf, ((ste_cat_ec_response_t *)data)->apdu.buf, ec_rsp_p->apdu.len);

                    p->data = ec_rsp_p;
                }
                else
                {
                    free(p);
                    p = NULL;
                    return NULL;
                }
            }
        }
        break;

        case STE_CAT_CAUSE_SET_TERMINAL_PROFILE:
        {
            ste_cat_set_terminal_profile_response_t  *rsp_p;

            if (data)
            {
                rsp_p = malloc( sizeof(*rsp_p) );
                if (rsp_p)
                {
                    memcpy( rsp_p, data, sizeof(*rsp_p) );
                    p->data = rsp_p;
                }
                else
                {
                    free(p);
                    p = NULL;
                    return NULL;
                }
            }
        }
        break;

        case STE_CAT_CAUSE_GET_TERMINAL_PROFILE:
        {
            ste_cat_get_terminal_profile_response_t  *rsp_p;

            if (data)
            {
                rsp_p = malloc( sizeof(*rsp_p) );
                if (rsp_p)
                {
                    memcpy( rsp_p, data, sizeof(*rsp_p) );
                    rsp_p->profile_p = malloc(rsp_p->profile_len + 1);
                    if (!rsp_p->profile_p)
                    {
                        printf("tcat: sync_data_create memory allocation(rsp_p->profile_p) failed\n");
                        free(rsp_p);
                        rsp_p = NULL;
                        free(p);
                        p = NULL;
                        return NULL;
                    }
                    memset(rsp_p->profile_p, 0, rsp_p->profile_len + 1);
                    memcpy(rsp_p->profile_p, ((ste_cat_get_terminal_profile_response_t *)data)->profile_p, rsp_p->profile_len);

                    p->data = rsp_p;
                }
                else
                {
                    free(p);
                    p = NULL;
                    return NULL;
                }
            }
        }
        break;

        case STE_CAT_CAUSE_GET_CAT_STATUS:
        case STE_CAT_CAUSE_IND_CAT_STATUS:
        {
            ste_cat_cat_status_t  *rsp_p;

            if (data)
            {
                rsp_p = malloc( sizeof(*rsp_p) );
                if (rsp_p)
                {
                    memcpy( rsp_p, data, sizeof(*rsp_p) );
                    p->data = rsp_p;
                }
                else
                {
                    free(p);
                    p = NULL;
                    return NULL;
                }
            }
        }
        break;

        case STE_CAT_CAUSE_SIM_EC_CALL_CONTROL:
        {
            ste_cat_call_control_response_t  *cc_rsp_p;

            if (data)
            {
                cc_rsp_p = malloc( sizeof(ste_cat_call_control_response_t) );
                if (cc_rsp_p)
                {
                    ste_cat_call_control_response_t  * temp_p = (ste_cat_call_control_response_t*)data;

                    memcpy( cc_rsp_p, temp_p, sizeof(ste_cat_call_control_response_t) );
                    p->data = cc_rsp_p;

                    //copy the other parameters inside of the ste_cat_call_control_response_t structure
                    //copy ste_cat_call_control_t    cc_info;
                    sim_copy_call_control_data(&(cc_rsp_p->cc_info), &(temp_p->cc_info));
                    //copy ste_sim_string_t          user_indication;
                    if (temp_p->user_indication.no_of_bytes > 0 && temp_p->user_indication.str_p)
                    {
                        cc_rsp_p->user_indication.str_p = malloc(temp_p->user_indication.no_of_bytes + 1);
                        if (!cc_rsp_p->user_indication.str_p)
                        {
                            cc_rsp_p->user_indication.no_of_bytes = 0;
                            free(cc_rsp_p);
                            cc_rsp_p = NULL;
                            free(p);
                            p = NULL;
                            return NULL;
                        }
                        memset(cc_rsp_p->user_indication.str_p, 0, temp_p->user_indication.no_of_bytes + 1);
                        memcpy(cc_rsp_p->user_indication.str_p, temp_p->user_indication.str_p, temp_p->user_indication.no_of_bytes);
                    }
                }
                else
                {
                    free(p);
                    p = NULL;
                    return NULL;
                }
            }
        }
        break;

        case STE_CAT_CAUSE_SIM_EC_SMS_CONTROL:
        {
            ste_cat_sms_control_response_t  *sc_rsp_p;

            if (data)
            {
                sc_rsp_p = malloc( sizeof(ste_cat_sms_control_response_t) );
                if (sc_rsp_p)
                {
                    ste_cat_sms_control_response_t  * temp_p = (ste_cat_sms_control_response_t*)data;

                    memcpy( sc_rsp_p, temp_p, sizeof(ste_cat_sms_control_response_t) );
                    p->data = sc_rsp_p;

                    //copy the other parameters inside of the ste_cat_sms_control_response_t structure
                    //copy ste_cat_sms_control_t    sc_info;
                    sim_copy_sms_control_data(&(sc_rsp_p->sc_info), &(temp_p->sc_info));
                    //copy ste_sim_string_t          user_indication;
                    if (temp_p->user_indication.no_of_bytes > 0 && temp_p->user_indication.str_p)
                    {
                        sc_rsp_p->user_indication.str_p = malloc(temp_p->user_indication.no_of_bytes + 1);
                        if (!sc_rsp_p->user_indication.str_p)
                        {
                            sc_rsp_p->user_indication.no_of_bytes = 0;
                            free(sc_rsp_p);
                            sc_rsp_p = NULL;
                            free(p);
                            p = NULL;
                            return NULL;
                        }
                        memset(sc_rsp_p->user_indication.str_p, 0, temp_p->user_indication.no_of_bytes + 1);
                        memcpy(sc_rsp_p->user_indication.str_p, temp_p->user_indication.str_p, temp_p->user_indication.no_of_bytes);
                    }
                }
                else
                {
                    free(p);
                    p = NULL;
                    return NULL;
                }
            }
        }
        break;
        case STE_CAT_CAUSE_PC_SETUP_CALL_IND:
        {
            ste_cat_pc_setup_call_ind_t  *ind_p;

            if (data)
            {
                ind_p = malloc( sizeof(*ind_p) );
                if (ind_p)
                {
                    memcpy( ind_p, data, sizeof(*ind_p) );
                    p->data = ind_p;
                }
                else
                {
                    free(p);
                    p = NULL;
                    return NULL;
                }
            }
        }
        break;
        case STE_CAT_CAUSE_ANSWER_CALL:
        {
            ste_cat_answer_call_response_t  *rsp_p;

            if (data)
            {
                rsp_p = malloc( sizeof(*rsp_p) );
                if (rsp_p)
                {
                    memcpy( rsp_p, data, sizeof(*rsp_p) );
                    p->data = rsp_p;
                }
                else
                {
                    free(p);
                    p = NULL;
                    return NULL;
                }
            }
        }
        break;
        case STE_CAT_CAUSE_EVENT_DOWNLOAD:
        {
            ste_cat_event_download_response_t  *rsp_p;

            if (data)
            {
                rsp_p = malloc( sizeof(*rsp_p) );
                if (rsp_p)
                {
                    memcpy( rsp_p, data, sizeof(*rsp_p) );
                    p->data = rsp_p;
                }
                else
                {
                    free(p);
                    p = NULL;
                    return NULL;
                }
            }
        }
        break;
        case STE_CAT_CAUSE_PC_REFRESH_IND:  //!! TODO: Needs a test case
        {
            ste_cat_pc_refresh_ind_t *ind_p;
            ste_cat_pc_refresh_ind_t *data_p = data;

            if (data_p)
            {
                ind_p = malloc( sizeof(*ind_p) );
                if (ind_p)
                {
                    *ind_p = *data_p;
                    p->data = ind_p;
                }
                else
                {
                    free(p);
                    p = NULL;
                    return NULL;
                }
            }
        }
        break;
        case STE_CAT_CAUSE_PC_REFRESH_FILE_IND: //!! TODO: Needs a test case
        {
            ste_cat_pc_refresh_file_ind_t *ind_p;
            ste_cat_pc_refresh_file_ind_t *data_p = data;

            if (data_p)
            {
                ind_p = malloc( sizeof(*ind_p) );
                if (ind_p)
                {
                    *ind_p = *data_p;
                    p->data = ind_p;
                }
                else
                {
                    free(p);
                    p = NULL;
                    return NULL;
                }
            }
        }
        break;

        default:
        {
            printf("sync_data_create: Unexpected cause = %d\n", cause);
        }
    }

    return p;
}

void sync_data_delete(sync_data_t * p)
{
    if (p && p->data)
    {
        free( p->data );
    }
    free(p);
}

uintptr_t get_ct_from_cause(int cause)
{
     switch(cause) {
           case STE_SIM_CAUSE_NOOP:
                return TCAT_CT_STE_SIM_CAUSE_NOOP;
           case STE_SIM_CAUSE_CONNECT:
                return TCAT_CT_STE_SIM_CAUSE_CONNECT;
           case STE_SIM_CAUSE_DISCONNECT:
                return TCAT_CT_STE_SIM_CAUSE_DISCONNECT;
           case STE_SIM_CAUSE_SHUTDOWN:
                return TCAT_CT_STE_SIM_CAUSE_SHUTDOWN;
           case STE_SIM_CAUSE_PING:
                return TCAT_CT_STE_SIM_CAUSE_PING;
           case STE_SIM_CAUSE_HANGUP:
                return TCAT_CT_STE_SIM_CAUSE_HANGUP;
           case STE_CAT_CAUSE_NOOP:
                return TCAT_CT_STE_CAT_CAUSE_NOOP;
           case STE_CAT_CAUSE_REGISTER:
                return TCAT_CT_STE_CAT_CAUSE_REGISTER;
           case STE_CAT_CAUSE_PC:
                return TCAT_CT_STE_CAT_CAUSE_PC;
           case STE_CAT_CAUSE_EC:
                return TCAT_CT_STE_CAT_CAUSE_EC;
           case STE_CAT_CAUSE_GET_CAT_STATUS:
                return TCAT_CT_STE_CAT_CAUSE_GET_CAT_STATUS;
           case STE_CAT_CAUSE_GET_TERMINAL_PROFILE:
                return TCAT_CT_STE_CAT_CAUSE_GET_TERMINAL_PROFILE;
           case STE_CAT_CAUSE_SET_TERMINAL_PROFILE:
                return TCAT_CT_STE_CAT_CAUSE_SET_TERMINAL_PROFILE;
           case STE_CAT_CAUSE_SIM_EC_CALL_CONTROL:
                return TCAT_CT_STE_CAT_CAUSE_SIM_EC_CALL_CONTROL;
           case STE_CAT_CAUSE_SIM_EC_SMS_CONTROL:
                return TCAT_CT_STE_CAT_CAUSE_SIM_EC_SMS_CONTROL;
           case STE_CAT_CAUSE_ANSWER_CALL:
                return TCAT_CT_STE_CAT_CAUSE_ANSWER_CALL;
           case STE_CAT_CAUSE_EVENT_DOWNLOAD:
                return TCAT_CT_STE_CAT_CAUSE_EVENT_DOWNLOAD;
           case STE_CAT_CAUSE_ENABLE:
                return TCAT_CT_STE_CAT_CAUSE_CAT_ENABLE;

           default:
                return TCAT_CT_UNKNOWN;

     }
}

void the_callback_with_sync_data(int cause, uintptr_t client_tag, void *data, void *vuser_data)
{
    cat_barrier_t          *bar = (cat_barrier_t *) vuser_data;
    sync_data_t            *sd = NULL;

    if (cause != STE_UICC_CAUSE_SIM_STATE_CHANGED && cause != STE_UICC_CAUSE_SIM_STATUS)
    {
        sd = sync_data_create(cause, data);
        cat_barrier_release(bar, sd);
    }

    printf("#\treceived: %2d %08x %08x %p %p : %s\n",
           cause,
           (unsigned int)client_tag,
           (cause == STE_CAT_CAUSE_PC ? (unsigned int)client_tag : (unsigned int)get_ct_from_cause(cause)), /* PC CT should only be passed back in the repond */
           (void*) data,
           (void*) vuser_data, ste_sim_cause_name[cause]);
}

void the_callback_without_sync_data(int cause, uintptr_t client_tag, void *data, void *vuser_data)
{
    cat_barrier_t          *bar = (cat_barrier_t *) vuser_data;
    static int              simd_hangup = 0;

    if (cause != STE_SIM_CAUSE_HANGUP && !(cause == STE_SIM_CAUSE_DISCONNECT && simd_hangup == 1))
    {
        cat_barrier_release(bar, NULL);
    }
    else
    {
        simd_hangup = 1;
    }

    printf("#\treceived: %2d %08x %08x %p %p : %s\n",
           cause,
           (unsigned int)client_tag,
           (cause == STE_CAT_CAUSE_PC ? (unsigned int)client_tag : (unsigned int)get_ct_from_cause(cause)), /* PC CT should only be passed back in the repond */
           (void*) data,
           (void*) vuser_data, ste_sim_cause_name[cause]);
}


int my_barrier_timedwait(cat_barrier_t * bar, unsigned ms)
{
    sync_data_t            *sd = 0;
    int                     i;

    i = cat_barrier_timedwait(bar, (void **) &sd, ms);
    if (sd)
    {
        sync_data_delete(sd);
    }
    return i;
}


// Timed test
//      int(*)(ste_cat_t*)      FUNC    Function to test
//      ste_cat_t*              C       cat object
//      uintptr_t               CT      Client tag
//      uint32_t                R       Registered events for CAT
//      cat_barrier_t*          B       barrier
//      unsigned                T       Time out in ms

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
        ste_sim_delete(C, CT);                  \
        cat_barrier_delete(B);                  \
        return #FUNC " timed out";              \
    }                                           \
} while(0)

#define TTEST_A( FUNC, CT, C, A, B, T )         \
do {                                            \
    int test_i;                                 \
    cat_barrier_set(B);                         \
    test_i = FUNC(C, CT);                       \
    if ( test_i ) {                             \
        ste_sim_delete(C, CT);                  \
        cat_barrier_delete(B);                  \
        ste_apdu_delete(A);                     \
        return #FUNC " failed.";                \
    }                                           \
    test_i = my_barrier_timedwait(B, T);        \
    if ( test_i ) {                             \
        ste_sim_delete(C, CT);                  \
        cat_barrier_delete(B);                  \
        ste_apdu_delete(A);                     \
        return #FUNC " timed out";              \
    }                                           \
} while(0)

#define TTEST_APDU( FUNC, CT, C, A, B, T )      \
do {                                            \
    int test_i;                                 \
    const uint8_t* buf_i;                       \
    size_t len_i;                               \
    buf_i = ste_apdu_get_raw(A);                \
    len_i = ste_apdu_get_raw_length(A);         \
    cat_barrier_set(B);                         \
    test_i = FUNC(C, CT, (const char*)buf_i,len_i);  \
    if ( test_i ) {                             \
        ste_sim_delete(C, CT);                  \
        cat_barrier_delete(B);                  \
        ste_apdu_delete(A);                     \
        return #FUNC " failed.";                \
    }                                           \
    test_i = my_barrier_timedwait(B, T);        \
    if ( test_i ) {                             \
        ste_sim_delete(C, CT);                  \
        cat_barrier_delete(B);                  \
        ste_apdu_delete(A);                     \
        return #FUNC " timed out";              \
    }                                           \
} while(0)

#define TTEST_R( FUNC, CT, C, R, B, T )         \
do {                                            \
    int test_i;                                 \
    cat_barrier_set(B);                         \
    test_i = FUNC(C, CT, R);                    \
    if ( test_i ) {                             \
        ste_sim_delete(C, CT);                  \
        cat_barrier_delete(B);                  \
        return #FUNC " failed.";                \
    }                                           \
    test_i = my_barrier_timedwait(B, T);        \
    if ( test_i ) {                             \
        ste_sim_delete(C, CT);                  \
        cat_barrier_delete(B);                  \
        return #FUNC " timed out";              \
    }                                           \
} while(0)


// This is a test of connect disconnect only
const char             *test_connectdisconnect(void)
{
    ste_sim_t              *cat;
    ste_sim_closure_t       pc;
    int                     i;
    cat_barrier_t          *bar = cat_barrier_new();


    pc.func = the_callback_without_sync_data;
    pc.user_data = bar;

    cat = ste_sim_new(&pc);
    if (!cat)
        return "ste_sim_new failed.";

    TTEST(ste_sim_connect, TCAT_CT_STE_SIM_CAUSE_CONNECT, cat, bar, 100);

    TTEST(ste_sim_disconnect, TCAT_CT_STE_SIM_CAUSE_DISCONNECT, (ste_sim_t*)cat, bar, 100);

    i = ste_sim_disconnect((ste_sim_t*)cat, TCAT_CT_STE_SIM_CAUSE_DISCONNECT);
    if (!i) {
        ste_sim_delete(cat, TCAT_CT_NOT_USED);
        cat_barrier_delete(bar);
       return "disconnect did not fail when disconnected.";
    }

    ste_sim_delete(cat, TCAT_CT_NOT_USED);

    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}



const char             *test_ping(void)
{
    ste_sim_t              *cat;
    ste_sim_closure_t       pc;
    int                     i;
    cat_barrier_t          *bar = cat_barrier_new();


    pc.func = the_callback_without_sync_data;
    pc.user_data = bar;

    cat = ste_sim_new(&pc);
    if (!cat)
        return "ste_sim_new failed.";

    i = ste_sim_ping(cat, TCAT_CT_STE_SIM_CAUSE_PING);
    if (!i){
        ste_sim_delete(cat, TCAT_CT_NOT_USED);
        cat_barrier_delete(bar);
        return "ping did not fail when not connected.";
    }


    TTEST(ste_sim_connect, TCAT_CT_STE_SIM_CAUSE_CONNECT, cat, bar, 100);

    TTEST(ste_sim_ping, TCAT_CT_STE_SIM_CAUSE_PING, cat, bar, 100);

    TTEST(ste_sim_disconnect, TCAT_CT_STE_SIM_CAUSE_DISCONNECT, cat, bar, 100);

    i = ste_sim_disconnect(cat, TCAT_CT_STE_SIM_CAUSE_DISCONNECT);
    if (!i) {
        ste_sim_delete(cat, TCAT_CT_NOT_USED);
        cat_barrier_delete(bar);
        return "disconnect did not fail when disconnected.";
    }

    ste_sim_delete(cat, TCAT_CT_NOT_USED);

    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}



const char             *test_register()
{
    ste_sim_t              *cat;
    ste_sim_closure_t       pc;
    cat_barrier_t          *bar = cat_barrier_new();


    pc.func = the_callback_without_sync_data;
    pc.user_data = bar;

    cat = ste_sim_new(&pc);
    if (!cat)
        return "ste_sim_new failed.";

    // Connect
    TTEST(ste_sim_connect, TCAT_CT_STE_SIM_CAUSE_CONNECT, cat, bar, 100);

    // Register
    TTEST_R(ste_cat_register, TCAT_CT_STE_CAT_CAUSE_REGISTER, cat, reg_events, bar, 100);

    // CAT enable
    TTEST_R(ste_cat_enable, TCAT_CT_STE_CAT_CAUSE_CAT_ENABLE, cat, STE_CAT_ENABLE_TYPE_ENABLE, bar, 100);

    TTEST(ste_cat_deregister, TCAT_CT_STE_SIM_CAUSE_DEREGISTER, cat, bar, 100);

    // CAT disable
    TTEST_R(ste_cat_enable, TCAT_CT_STE_CAT_CAUSE_CAT_ENABLE, cat, STE_CAT_ENABLE_TYPE_DISABLE, bar, 100);

    // Disconnect
    TTEST(ste_sim_disconnect, TCAT_CT_STE_SIM_CAUSE_DISCONNECT, cat, bar, 100);

    ste_sim_delete(cat, TCAT_CT_NOT_USED);

    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}



const char             *test_connect10()
{
    ste_sim_t              *cat;
    ste_sim_closure_t       pc;
    cat_barrier_t          *bar = cat_barrier_new();

    unsigned int            n = 10;

    pc.func = the_callback_without_sync_data;
    pc.user_data = bar;

    cat = ste_sim_new(&pc);
    if (!cat)
        return "ste_sim_new failed.";

    while (n--) {
        TTEST(ste_sim_connect, TCAT_CT_STE_SIM_CAUSE_CONNECT, cat, bar, 100);

        // Register
        TTEST_R(ste_cat_register, TCAT_CT_STE_CAT_CAUSE_REGISTER, cat, reg_events, bar, 100);

        TTEST(ste_cat_deregister, TCAT_CT_STE_SIM_CAUSE_DEREGISTER, cat, bar, 100);

        TTEST(ste_sim_disconnect, TCAT_CT_STE_SIM_CAUSE_DISCONNECT, cat, bar, 100);
    }

    ste_sim_delete(cat, TCAT_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}


const char             *test_registerdisconnectXtimes()
{
typedef struct {
   ste_sim_t              *cat;
   ste_sim_closure_t       pc;
}testdataperclient_t;
   testdataperclient_t ListOfClients[10];
   cat_barrier_t       *bar[10];

   int            n;

   n = 9;

   // Connect and register all 10 clients

   while (n >= 0) {
        bar[n] = cat_barrier_new();
        ListOfClients[n].pc.func = the_callback_without_sync_data;
        ListOfClients[n].pc.user_data = bar[n];
        ListOfClients[n].cat = ste_sim_new(&(ListOfClients[n].pc));

        TTEST(ste_sim_connect, TCAT_CT_STE_SIM_CAUSE_CONNECT,
	      ListOfClients[n].cat, bar[n], 100);

        TTEST_R(ste_cat_register, TCAT_CT_STE_CAT_CAUSE_REGISTER,
	        ListOfClients[n].cat, reg_events, bar[n], 100);

        if (n == 0)
	  break;
        n--;
   }

   // Disconnect every other client.

   n = 9;

   while (n >= 0) {
        printf("registerdisconnectXtimes: n=%d disconnect \n", n);

        TTEST(ste_cat_deregister, TCAT_CT_STE_SIM_CAUSE_DEREGISTER,
             ListOfClients[n].cat, bar[n], 100);

        TTEST(ste_sim_disconnect, TCAT_CT_STE_SIM_CAUSE_DISCONNECT,
	     ListOfClients[n].cat, bar[n], 100);
        n = n - 2;
    }

   // Once again connect and register the clients that were disconnected.

   n = 9;

   while (n >= 0) {
        printf("registerdisconnectXtimes connect register: n=%d \n", n);

        TTEST(ste_sim_connect, TCAT_CT_STE_SIM_CAUSE_CONNECT,
	      ListOfClients[n].cat, bar[n], 100);

        TTEST_R(ste_cat_register, TCAT_CT_STE_CAT_CAUSE_REGISTER,
	        ListOfClients[n].cat, reg_events, bar[n], 100);

        n = n - 2;
   }

   // Disconnect all clients and clean up.

   n = 9;

   while (n >= 0) {
        printf("registerdisconnectXtimes: n=%d disconnect \n", n);

        TTEST(ste_cat_deregister, TCAT_CT_STE_SIM_CAUSE_DEREGISTER,
             ListOfClients[n].cat, bar[n], 100);

        TTEST(ste_sim_disconnect, TCAT_CT_STE_SIM_CAUSE_DISCONNECT,
	     ListOfClients[n].cat, bar[n], 100);

       ste_sim_delete(ListOfClients[n].cat, TCAT_CT_NOT_USED);
       cat_barrier_delete(bar[n]);

        n--;
    }

    sleep(1);

    return 0;
}


const char             *test_startup()
{
    ste_sim_t              *sim;
    ste_sim_closure_t       pc;
    cat_barrier_t          *bar = cat_barrier_new();


    pc.func = the_callback_with_sync_data;
    pc.user_data = bar;

    sim = ste_sim_new(&pc);
    if (!sim)
    {
        return "ste_sim_new failed.";
    }

    TTEST(ste_sim_connect, TCAT_CT_STE_SIM_CAUSE_CONNECT, sim, bar, 100);

    sleep(1);

    do
    {
        int test_i;
        sync_data_t            *sd = 0;

        cat_barrier_set(bar);
        test_i = ste_sim_startup(sim, TCAT_CT_STE_SIM_CAUSE_STARTUP);
        if ( test_i )
        {
            return  "ste_sim_startup";
        }

        test_i = cat_barrier_timedwait(bar, (void **) &sd, 5000);
        if ( test_i )
        {
            return "ste_sim_startup timed out";
        }

        if( sd && sd->cause == STE_SIM_CAUSE_STARTUP )
        {
            printf("#\tSIM startup response received \n");
            sync_data_delete(sd);
            break;
        }

        sync_data_delete(sd);
    } while(1);

    sleep(1);

    TTEST(ste_sim_disconnect, TCAT_CT_STE_SIM_CAUSE_DISCONNECT, sim, bar, 100);
    ste_sim_delete(sim, TCAT_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}


const char             *test_shutdown()
{
    ste_sim_t              *cat;
    ste_sim_closure_t       pc;
    int                     i;
    cat_barrier_t          *bar = cat_barrier_new();


    pc.func = the_callback_without_sync_data;
    pc.user_data = bar;

    cat = ste_sim_new(&pc);
    if (!cat)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TCAT_CT_STE_SIM_CAUSE_CONNECT, cat, bar, 100);

    TTEST(ste_sim_shutdown, TCAT_CT_STE_SIM_CAUSE_SHUTDOWN, cat, bar, 100);

    sleep(1);

    i = ste_sim_disconnect(cat, TCAT_CT_STE_SIM_CAUSE_DISCONNECT); // Disconnect after shutdown should not use the barrier
    if (i) {
        ste_sim_delete(cat, TCAT_CT_NOT_USED);
        cat_barrier_delete(bar);
        return "test_shutdown: ste_sim_disconnect Failed";
    }

    ste_sim_delete(cat, TCAT_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    return 0;
}

static int main_ec_call_control(ste_sim_t *cat, cat_barrier_t *bar, ste_cat_call_control_t * cc_p)
{
    int   i;
    int   ret_val = -1;
    int   is_done = 0;

    if (!cat || !bar || !cc_p)
    {
        return -1;
    }

    i = ste_cat_call_control(cat, TCAT_CT_STE_CAT_CAUSE_SIM_EC_CALL_CONTROL, cc_p);

    if (i)
    {
        printf("#\tCALL CONTROL test: ste_cat_call_control failed.\n");
        return -1;
    }

    for (;is_done == 0;)
    {
        sync_data_t            *sd;

        cat_barrier_set(bar);
        i = cat_barrier_timedwait(bar, (void **) &sd, 10000);
        if (i)
        {
            printf("#\tCALL CONTROL test: Timed out\n");
            continue;
        }
        if (!sd)
        {
            printf("#\tCALL CONTROL test: No sync data.\n");
            continue;
        }

        printf("#\tCALL CONTROL test:  %2d %p : %s\n", sd->cause, (void*) sd->data, ste_sim_cause_name[sd->cause]);

        if (sd->cause == STE_SIM_CAUSE_DISCONNECT || sd->cause == STE_SIM_CAUSE_HANGUP)
        {
            sync_data_delete(sd);
            break;
        }

        if (sd->cause == STE_CAT_CAUSE_SIM_EC_CALL_CONTROL)
        {
            ste_cat_call_control_response_t  *cc_rsp_p;

            cc_rsp_p = sd->data;
            if (!cc_rsp_p)
            {
                printf("#\tCALL CONTROL test: No data for this EC CALL CONTROL response.\n");
                sync_data_delete(sd);
                break;
            }

            printf("#\tCALL CONTROL test: EC CALL CONTROL response. CALL CONTROL Result = 0x%x\n", cc_rsp_p->cc_result);
            //if needed, also print out the cc_info, and user_indication information
            //and also free them, of course
            sim_free_call_control_data(&(cc_rsp_p->cc_info));
            if (cc_rsp_p->user_indication.no_of_bytes > 0)
            {
                free(cc_rsp_p->user_indication.str_p);
            }

            is_done = 1;
            ret_val = 0;
        }
        sync_data_delete(sd);
    }  //end for (;is_done == 0;)

    return ret_val;
}

const char             *test_ecCallControl()
{
    ste_sim_t              *cat;
    ste_sim_closure_t       pc;
    cat_barrier_t          *bar = cat_barrier_new();
    int                     i;

    ste_cat_call_control_t         cc;
    ste_cat_cc_call_setup_t        cc_call;
    ste_cat_cc_dialled_address_t   dial_address;
    ste_cat_cc_sub_address_t       sub_address;
    ste_cat_cc_ccp_t               ccp_1;
    ste_cat_cc_ccp_t               ccp_2;
    ste_sim_text_t                 dial_string;
    uint8_t call_no[] = "46222";
    uint8_t ccp1[] = "1111";
    uint8_t ccp2[] = "2222";
    uint8_t sub_address_str[] = "3333";

    pc.func = the_callback_with_sync_data;
    pc.user_data = bar;

    cat = ste_sim_new(&pc);
    if (!cat)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TCAT_CT_STE_SIM_CAUSE_CONNECT, cat, bar, 100);

    sleep(1);

    TTEST_R(ste_cat_register, TCAT_CT_STE_CAT_CAUSE_REGISTER, cat, reg_events, bar, 100);
    // CAT enable
    TTEST_R(ste_cat_enable, TCAT_CT_STE_CAT_CAUSE_CAT_ENABLE, cat, STE_CAT_ENABLE_TYPE_ENABLE, bar, 100);

    dial_string.text_coding = STE_SIM_BCD;
    dial_string.no_of_characters = 10;
    dial_string.text_p = call_no;

    dial_address.dialled_string_p = &dial_string;
    dial_address.ton = STE_SIM_TON_INTERNATIONAL;
    dial_address.npi = STE_SIM_NPI_ISDN;

    ccp_1.len = 4;
    ccp_1.ccp_p = ccp1;

    ccp_2.len = 4;
    ccp_2.ccp_p = ccp2;

    sub_address.len = 4;
    sub_address.sub_address_p = sub_address_str;

    cc_call.address = dial_address;
    cc_call.sub_address = sub_address;
    cc_call.ccp_1 = ccp_1;
    cc_call.ccp_2 = ccp_2;
    cc_call.bc_repeat_ind = 0x01;

    cc.cc_type = STE_CAT_CC_CALL_SETUP;
    cc.cc_data.call_setup_p = &cc_call;

    i = main_ec_call_control(cat, bar, &cc);

    TTEST(ste_cat_deregister, TCAT_CT_STE_SIM_CAUSE_DEREGISTER, cat, bar, 100);

    TTEST(ste_sim_disconnect, TCAT_CT_STE_SIM_CAUSE_DISCONNECT, cat, bar, 100);

    ste_sim_delete(cat, TCAT_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    if (i)
    {
        printf("#\ttest_ecCallControl failed.\n");
        return "test_ecCallControl failed.";
    }
    else
    {
        printf("#\ttest_ecCallControl succeeded.\n");
        return 0;
    }
}

static int main_ec_call_control_pdp(ste_sim_t *cat, cat_barrier_t *bar, ste_cat_call_control_t * cc_p)
{
    int   i;
    int   ret_val = -1;
    int   is_done = 0;

    if (!cat || !bar || !cc_p)
    {
        return -1;
    }

    i = ste_cat_call_control(cat, TCAT_CT_STE_CAT_CAUSE_SIM_EC_CALL_CONTROL, cc_p);

    if (i)
    {
        printf("#\tCALL CONTROL PDP test: ste_cat_call_control failed.\n");
        return -1;
    }

    for (;is_done == 0;)
    {
        sync_data_t            *sd;

        cat_barrier_set(bar);
        i = cat_barrier_timedwait(bar, (void **) &sd, 10000);
        if (i)
        {
            printf("#\tCALL CONTROL PDP test: Timed out\n");
            continue;
        }
        if (!sd)
        {
            printf("#\tCALL CONTROL PDP test: No sync data.\n");
            continue;
        }

        printf("#\tCALL CONTROL PDP test:  %2d %p : %s\n", sd->cause, (void*) sd->data, ste_sim_cause_name[sd->cause]);

        if (sd->cause == STE_SIM_CAUSE_DISCONNECT || sd->cause == STE_SIM_CAUSE_HANGUP)
        {
            sync_data_delete(sd);
            break;
        }

        if (sd->cause == STE_CAT_CAUSE_SIM_EC_CALL_CONTROL)
        {
            ste_cat_call_control_response_t  *cc_rsp_p;

            cc_rsp_p = sd->data;
            if (!cc_rsp_p)
            {
                printf("#\tCALL CONTROL PDP test: No data for this EC CALL CONTROL response.\n");
                sync_data_delete(sd);
                break;
            }

            printf("#\tCALL CONTROL PDP test: EC CALL CONTROL response. CALL CONTROL Result = 0x%x\n", cc_rsp_p->cc_result);
            //if needed, also print out the cc_info, and user_indication information
            //and also free them, of course
            sim_free_call_control_data(&(cc_rsp_p->cc_info));
            if (cc_rsp_p->user_indication.no_of_bytes > 0)
            {
                free(cc_rsp_p->user_indication.str_p);
            }

            is_done = 1;
            ret_val = 0;
        }
        sync_data_delete(sd);
    }  //end for (;is_done == 0;)

    return ret_val;
}

const char             *test_ecCallControl_pdp()
{
    ste_sim_t              *cat;
    ste_sim_closure_t       pc;
    cat_barrier_t          *bar = cat_barrier_new();
    int                     i;

    ste_cat_call_control_t         cc;
    ste_cat_cc_pdp_t               cc_pdp;
    uint8_t pdp_str[] = "ipv6;telia;xxx@ste.com;8888";

    pc.func = the_callback_with_sync_data;
    pc.user_data = bar;

    cat = ste_sim_new(&pc);
    if (!cat)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TCAT_CT_STE_SIM_CAUSE_CONNECT, cat, bar, 100);

    sleep(1);

    TTEST_R(ste_cat_register, TCAT_CT_STE_CAT_CAUSE_REGISTER, cat, reg_events, bar, 100);
    // CAT enable
    TTEST_R(ste_cat_enable, TCAT_CT_STE_CAT_CAUSE_CAT_ENABLE, cat, STE_CAT_ENABLE_TYPE_ENABLE, bar, 100);

    cc_pdp.pdp_context.text_coding = STE_SIM_CODING_UNKNOWN;
    cc_pdp.pdp_context.no_of_bytes = 27;
    cc_pdp.pdp_context.str_p = pdp_str;

    cc.cc_type = STE_CAT_CC_PDP;
    cc.cc_data.pdp_p = &cc_pdp;

    i = main_ec_call_control_pdp(cat, bar, &cc);

    TTEST(ste_cat_deregister, TCAT_CT_STE_SIM_CAUSE_DEREGISTER, cat, bar, 100);

    TTEST(ste_sim_disconnect, TCAT_CT_STE_SIM_CAUSE_DISCONNECT, cat, bar, 100);

    ste_sim_delete(cat, TCAT_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    if (i)
    {
        printf("#\ttest_ecCallControl_pdp failed.\n");
        return "test_ecCallControl_pdp failed.";
    }
    else
    {
        printf("#\ttest_ecCallControl_pdp succeeded.\n");
        return 0;
    }
}

static int main_ec_ss_control(ste_sim_t *cat, cat_barrier_t *bar, ste_cat_call_control_t * cc_p)
{
    int   i;
    int   ret_val = -1;
    int   is_done = 0;

    if (!cat || !bar || !cc_p)
    {
        return -1;
    }

    i = ste_cat_call_control(cat, STE_CAT_CAUSE_SIM_EC_CALL_CONTROL, cc_p);

    if (i)
    {
        printf("#\tCALL CONTROL SS test: ste_cat_call_control failed.\n");
        return -1;
    }

    for (;is_done == 0;)
    {
        sync_data_t            *sd;

        cat_barrier_set(bar);
        i = cat_barrier_timedwait(bar, (void **) &sd, 10000);
        if (i)
        {
            printf("#\tCALL CONTROL SS test: Timed out\n");
            continue;
        }
        if (!sd)
        {
            printf("#\tCALL CONTROL SS test: No sync data.\n");
            continue;
        }

        printf("#\tCALL CONTROL SS test:  %2d %p : %s\n", sd->cause, (void*) sd->data, ste_sim_cause_name[sd->cause]);

        if (sd->cause == STE_SIM_CAUSE_DISCONNECT || sd->cause == STE_SIM_CAUSE_HANGUP)
        {
            sync_data_delete(sd);
            break;
        }

        if (sd->cause == STE_CAT_CAUSE_SIM_EC_CALL_CONTROL)
        {
            ste_cat_call_control_response_t  *cc_rsp_p;

            cc_rsp_p = sd->data;
            if (!cc_rsp_p)
            {
                printf("#\tCALL CONTROL SS test: No data for this SS CONTROL response.\n");
                sync_data_delete(sd);
                break;
            }

            printf("#\tCALL CONTROL SS test: SS CONTROL response. CALL CONTROL Result = 0x%x\n", cc_rsp_p->cc_result);
            //if needed, also print out the cc_info, and user_indication information
            //and also free them, of course
            sim_free_call_control_data(&(cc_rsp_p->cc_info));
            if (cc_rsp_p->user_indication.no_of_bytes > 0)
            {
                free(cc_rsp_p->user_indication.str_p);
            }

            is_done = 1;
            ret_val = 0;
        }
        sync_data_delete(sd);
    }  //end for (;is_done == 0;)

    return ret_val;
}

const char             *test_ecSSControl()
{
    ste_sim_t              *cat;
    ste_sim_closure_t       pc;
    cat_barrier_t          *bar = cat_barrier_new();
    int                     i;

    ste_cat_call_control_t         cc;
    ste_cat_cc_ss_t        ss_p;
    ste_sim_text_t                 dial_string;
    uint8_t call_no[] = "46222";

    pc.func = the_callback_with_sync_data;
    pc.user_data = bar;

    cat = ste_sim_new(&pc);
    if (!cat)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TCAT_CT_STE_SIM_CAUSE_CONNECT, cat, bar, 100);

    sleep(1);

    TTEST_R(ste_cat_register, TCAT_CT_STE_CAT_CAUSE_REGISTER, cat, reg_events, bar, 100);
    // CAT enable
    TTEST_R(ste_cat_enable, TCAT_CT_STE_CAT_CAUSE_CAT_ENABLE, cat, STE_CAT_ENABLE_TYPE_ENABLE, bar, 100);

    cc.cc_type = STE_CAT_CC_SS;
    cc.cc_data.ss_p = &ss_p;

    dial_string.text_coding = STE_SIM_BCD;
    dial_string.no_of_characters = 10;
    dial_string.text_p = call_no;

    ss_p.address.dialled_string_p = &dial_string;
    ss_p.address.ton = STE_SIM_TON_INTERNATIONAL;
    ss_p.address.npi = STE_SIM_NPI_ISDN;

    i = main_ec_ss_control(cat, bar, &cc);

    TTEST(ste_cat_deregister, TCAT_CT_STE_SIM_CAUSE_DEREGISTER, cat, bar, 100);

    TTEST(ste_sim_disconnect, TCAT_CT_STE_SIM_CAUSE_DISCONNECT, cat, bar, 100);

    ste_sim_delete(cat, TCAT_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    if (i)
    {
        printf("#\ttest_ecSSControl failed.\n");
        return "test_ecSSControl failed.";
    }
    else
    {
        printf("#\ttest_ecSSControl succeeded.\n");
        return 0;
    }
}

static int main_ec_ussd_control(ste_sim_t *cat, cat_barrier_t *bar, ste_cat_call_control_t * cc_p)
{
    int   i;
    int   ret_val = -1;
    int   is_done = 0;

    if (!cat || !bar || !cc_p)
    {
        return -1;
    }

    i = ste_cat_call_control(cat, STE_CAT_CAUSE_SIM_EC_CALL_CONTROL, cc_p);

    if (i)
    {
        printf("#\tCALL CONTROL USSD test: ste_cat_call_control failed.\n");
        return -1;
    }

    for (;is_done == 0;)
    {
        sync_data_t            *sd;

        cat_barrier_set(bar);
        i = cat_barrier_timedwait(bar, (void **) &sd, 10000);
        if (i)
        {
            printf("#\tCALL CONTROL USSD test: Timed out\n");
            continue;
        }
        if (!sd)
        {
            printf("#\tCALL CONTROL test: No sync data.\n");
            continue;
        }

        printf("#\tCALL CONTROL USSD test:  %2d %p : %s\n", sd->cause, (void*) sd->data, ste_sim_cause_name[sd->cause]);

        if (sd->cause == STE_SIM_CAUSE_DISCONNECT || sd->cause == STE_SIM_CAUSE_HANGUP)
        {
            sync_data_delete(sd);
            break;
        }

        if (sd->cause == STE_CAT_CAUSE_SIM_EC_CALL_CONTROL)
        {
            ste_cat_call_control_response_t  *cc_rsp_p;

            cc_rsp_p = sd->data;
            if (!cc_rsp_p)
            {
                printf("#\tCALL CONTROL USSD test: No data for this SS CONTROL response.\n");
                sync_data_delete(sd);
                break;
            }

            printf("#\tCALL CONTROL USSD test: USS CONTROL response. CALL CONTROL Result = 0x%x\n", cc_rsp_p->cc_result);
            //if needed, also print out the cc_info, and user_indication information
            //and also free them, of course
            sim_free_call_control_data(&(cc_rsp_p->cc_info));
            if (cc_rsp_p->user_indication.no_of_bytes > 0)
            {
                free(cc_rsp_p->user_indication.str_p);
            }

            is_done = 1;
            ret_val = 0;
        }
        sync_data_delete(sd);
    }  //end for (;is_done == 0;)

    return ret_val;
}

const char             *test_ecUSSDControl()
{
    ste_sim_t              *cat;
    ste_sim_closure_t       pc;
    cat_barrier_t          *bar = cat_barrier_new();
    int                     i;

    ste_cat_call_control_t         cc;
    ste_cat_cc_ussd_t        uss_p;
    ste_sim_string_t                 dial_string;
    uint8_t call_no[] = "*121*1#";

    pc.func = the_callback_with_sync_data;
    pc.user_data = bar;

    cat = ste_sim_new(&pc);
    if (!cat)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TCAT_CT_STE_SIM_CAUSE_CONNECT, cat, bar, 100);

    sleep(1);

    TTEST_R(ste_cat_register, TCAT_CT_STE_CAT_CAUSE_REGISTER, cat, reg_events, bar, 100);
    // CAT enable
    TTEST_R(ste_cat_enable, TCAT_CT_STE_CAT_CAUSE_CAT_ENABLE, cat, STE_CAT_ENABLE_TYPE_ENABLE, bar, 100);

    cc.cc_type = STE_CAT_CC_USSD;
    cc.cc_data.ussd_p = &uss_p;

    dial_string.text_coding = STE_SIM_ASCII8;
    dial_string.no_of_bytes = sizeof(call_no);
    dial_string.str_p = call_no;

    uss_p.ussd_data.dialled_string_p = &dial_string;
    uss_p.ussd_data.dcs = STE_SIM_GSM_Default;

    i = main_ec_ussd_control(cat, bar, &cc);

    TTEST(ste_cat_deregister, TCAT_CT_STE_SIM_CAUSE_DEREGISTER, cat, bar, 100);

    TTEST(ste_sim_disconnect, TCAT_CT_STE_SIM_CAUSE_DISCONNECT, cat, bar, 100);

    ste_sim_delete(cat, TCAT_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    if (i)
    {
        printf("#\ttest_ecUssdControl failed.\n");
        return "test_ecUssdControl failed.";
    }
    else
    {
        printf("#\ttest_ecUssdControl succeeded.\n");
        return 0;
    }
}

static int main_ec_sms_control(ste_sim_t *cat, cat_barrier_t *bar, ste_cat_sms_control_t * sc_p)
{
    int   i;
    int   ret_val = -1;
    int   is_done = 0;

    if (!cat || !bar || !sc_p)
    {
        return -1;
    }

    i = ste_cat_sms_control(cat, TCAT_CT_STE_CAT_CAUSE_SIM_EC_SMS_CONTROL, sc_p);

    if (i)
    {
        printf("#\tSMS CONTROL test: ste_cat_sms_control failed.\n");
        return -1;
    }

    for (;is_done == 0;)
    {
        sync_data_t            *sd;

        cat_barrier_set(bar);
        i = cat_barrier_timedwait(bar, (void **) &sd, 10000);
        if (i)
        {
            printf("#\tSMS CONTROL test: Timed out\n");
            continue;
        }
        if (!sd)
        {
            printf("#\tSMS CONTROL test: No sync data.\n");
            continue;
        }

        printf("#\tSMS CONTROL test:  %2d %p : %s\n", sd->cause, (void*) sd->data, ste_sim_cause_name[sd->cause]);

        if (sd->cause == STE_SIM_CAUSE_DISCONNECT || sd->cause == STE_SIM_CAUSE_HANGUP)
        {
            sync_data_delete(sd);
            break;
        }

        if (sd->cause == STE_CAT_CAUSE_SIM_EC_SMS_CONTROL)
        {
            ste_cat_sms_control_response_t  *sc_rsp_p;

            sc_rsp_p = sd->data;
            if (!sc_rsp_p)
            {
                printf("#\tSMS CONTROL test: No data for this EC SMS CONTROL response.\n");
                sync_data_delete(sd);
                break;
            }

            printf("#\tSMS CONTROL test: EC SMS CONTROL response. SMS CONTROL Result = 0x%x\n", sc_rsp_p->sc_result);
            //if needed, also print out the sc_info, and user_indication information
            //and also free them, of course
            sim_free_sms_control_data(&(sc_rsp_p->sc_info));
            if (sc_rsp_p->user_indication.no_of_bytes > 0)
            {
                free(sc_rsp_p->user_indication.str_p);
            }

            is_done = 1;
            ret_val = 0;
        }
        sync_data_delete(sd);
    }  //end for (;is_done == 0;)

    return ret_val;
}

const char             *test_ecSmsControl()
{
    ste_sim_t              *cat;
    ste_sim_closure_t       pc;
    cat_barrier_t          *bar = cat_barrier_new();
    int                     i;

    ste_cat_sms_control_t          sc;
    ste_cat_cc_dialled_address_t   smsc_address;
    ste_cat_cc_dialled_address_t   dest_address;
    ste_sim_text_t                 dial_string_1;
    ste_sim_text_t                 dial_string_2;
    uint8_t call_no_1[] = "46111";
    uint8_t call_no_2[] = "46222222";

    pc.func = the_callback_with_sync_data;
    pc.user_data = bar;

    cat = ste_sim_new(&pc);
    if (!cat)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TCAT_CT_STE_SIM_CAUSE_CONNECT, cat, bar, 100);

    sleep(1);

    TTEST_R(ste_cat_register, TCAT_CT_STE_CAT_CAUSE_REGISTER, cat, reg_events, bar, 100);
    // CAT enable
    TTEST_R(ste_cat_enable, TCAT_CT_STE_CAT_CAUSE_CAT_ENABLE, cat, STE_CAT_ENABLE_TYPE_ENABLE, bar, 100);

    dial_string_1.text_coding = STE_SIM_BCD;
    dial_string_1.no_of_characters = 10;
    dial_string_1.text_p = call_no_1;

    dial_string_2.text_coding = STE_SIM_BCD;
    dial_string_2.no_of_characters = 16;
    dial_string_2.text_p = call_no_2;

    smsc_address.dialled_string_p = &dial_string_1;
    smsc_address.ton = STE_SIM_TON_INTERNATIONAL;
    smsc_address.npi = STE_SIM_NPI_ISDN;

    dest_address.dialled_string_p = &dial_string_2;
    dest_address.ton = STE_SIM_TON_INTERNATIONAL;
    dest_address.npi = STE_SIM_NPI_ISDN;

    sc.smsc = smsc_address;
    sc.dest = dest_address;

    i = main_ec_sms_control(cat, bar, &sc);

    TTEST(ste_cat_deregister, TCAT_CT_STE_SIM_CAUSE_DEREGISTER, cat, bar, 100);

    TTEST(ste_sim_disconnect, TCAT_CT_STE_SIM_CAUSE_DISCONNECT, cat, bar, 100);

    ste_sim_delete(cat, TCAT_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    if (i)
    {
        printf("#\ttest_ecSmsControl failed.\n");
        return "test_ecSmsControl failed.";
    }
    else
    {
        printf("#\ttest_ecSmsControl succeeded.\n");
        return 0;
    }
}

static void create_terminal_response(ste_parsed_apdu_t * apdu_in,
                                     ste_apdu_t ** apdu_out)
{
    ste_sat_apdu_error_t    err;
    ste_command_result_t    result;
    unsigned char           AdditionalInfo[20] = "Additional Info OK.";

    result.general_result = SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
    result.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
    result.additional_info_size = 20;
    result.additional_info_p = AdditionalInfo;
    result.other_data_type = STE_CMD_RESULT_NOTHING;
    result.other_data.imei_p = NULL;

    err = ste_apdu_terminal_response(apdu_in, &result, apdu_out);
    if (err != STE_SAT_APDU_ERROR_NONE)
    {
        printf("#\tPC test: create_terminal_response failed.\n");
        return;
    }
}

static int main_proactive_command(ste_sim_t *cat, cat_barrier_t *bar)
{
    int   i;
    int   ret_val = -1;
    int   is_done = 0;
    uintptr_t client_tag = 0;

    if (!cat || !bar)
    {
        return -1;
    }

    for (;is_done == 0;)
    {
        sync_data_t            *sd;

        cat_barrier_set(bar);
        i = cat_barrier_timedwait(bar, (void **) &sd, 10000);
        if (i)
        {
            printf("#\tPC test: Timed out\n");
            continue;
        }
        if (!sd)
        {
            printf("#\tPC test: No sync data.\n");
            continue;
        }

        printf("#\tPC test:  %2d %p : %s\n", sd->cause, (void*) sd->data, ste_sim_cause_name[sd->cause]);

        if (sd->cause == STE_SIM_CAUSE_DISCONNECT || sd->cause == STE_SIM_CAUSE_HANGUP)
        {
            sync_data_delete(sd);
            break;
        }

        if (sd->cause == STE_CAT_CAUSE_PC)
        {
            ste_cat_pc_ind_t       *pc_ind_p;
            ste_cat_apdu_data_t    *apdu_data;
            ste_apdu_t             *apdu;

            pc_ind_p = sd->data;
            if (!pc_ind_p)
            {
                printf("#\tPC test: No data for this PC.\n");
                sync_data_delete(sd);
                continue;
            }
            apdu_data = &(pc_ind_p->apdu);
            client_tag = pc_ind_p->simd_tag;
            apdu = ste_apdu_new((uint8_t *) (apdu_data->buf), apdu_data->len);
            free(apdu_data->buf);

            if (apdu)
            {
                ste_parsed_apdu_t      *parsed_apdu = NULL;
                ste_sat_apdu_error_t    err;
                int                     kind;
                int                     type;

                err = ste_apdu_parse(apdu, &parsed_apdu);
                if (err != STE_SAT_APDU_ERROR_NONE)
                {
                    printf("#\tPC test: Parse APDU from CATD failed.\n");
                    //free resources
                    ste_apdu_delete(apdu);
                    sync_data_delete(sd);
                    ste_parsed_apdu_delete(parsed_apdu);
                    continue;
                }
                type = ste_parsed_apdu_get_type(parsed_apdu);
                kind = ste_parsed_apdu_get_kind(parsed_apdu);
                printf("#\tPC test: PC kind = 0x%x type = 0x%x\n", kind, type);

                switch (type)
                {
                    case STE_APDU_CMD_TYPE_DISPLAY_TEXT:
                    case STE_APDU_CMD_TYPE_SET_UP_MENU:
                    case STE_APDU_CMD_TYPE_GET_INKEY:
                    case STE_APDU_CMD_TYPE_GET_INPUT:
                    case STE_APDU_CMD_TYPE_PROVIDE_LOCAL_INFO:
                    case STE_APDU_CMD_TYPE_SET_UP_EVENT_LIST:
                    case STE_APDU_CMD_TYPE_SELECT_ITEM:
                    {
                        ste_apdu_t             *resp;
                        const char             *buf;
                        size_t                  len;

                        create_terminal_response(parsed_apdu, &resp);
                        buf = (const char *) ste_apdu_get_raw(resp);
                        len = ste_apdu_get_raw_length(resp);

                        i = ste_cat_terminal_response(cat, client_tag, buf, len);
                        if (i)
                        {
                            printf("#\tPC test: create TR failed.\n");
                            ret_val = -1;
                        }
                        ste_apdu_delete(resp);
                    }
                    break;
                    case STE_APDU_CMD_TYPE_OPEN_CHANNEL:
                    {
                        char buf[] = {
                            0x81, 0x03, 0x01, 0x40, 0x01, // Command Details
                            0x02, 0x02, 0x82, 0x81, // Device Identities
                            0x83, 0x01, 0x00, // Command Result
                            0x38, 0x02, 0x81, 0x00, // Channel Status
                            0x35, 0x07, 0x02, 0x00, 0x00, 0x03, 0x00, 0x00, 0x02, // Bearer Description
                            0x39, 0x02, 0x05, 0x78 // Buffer Size
                        };
                        size_t len = sizeof(buf)/sizeof(char);
                        i = ste_cat_terminal_response(cat, client_tag, buf, len);
                        if (i)
                        {
                            printf("#\tPC test: create TR failed.\n");
                            ret_val = -1;
                        }
                    }
                    break;
                    case STE_APDU_CMD_TYPE_SEND_DATA:
                    {
                        char buf[] = {
                            0x81, 0x03, 0x01, 0x43, 0x01, // Command Details
                            0x02, 0x02, 0x82, 0x81, // Device Identities
                            0x83, 0x01, 0x00, // Command Result
                            0x37, 0x01, 0xFF // Channel Data Length
                        };
                        size_t len = sizeof(buf)/sizeof(char);
                        i = ste_cat_terminal_response(cat, client_tag, buf, len);
                        if (i)
                        {
                            printf("#\tPC test: create TR failed.\n");
                            ret_val = -1;
                        }
                    }
                    break;
                    case STE_APDU_CMD_TYPE_RECEIVE_DATA:
                    {
                        char buf[] = {
                            0x81, 0x03, 0x01, 0x42, 0x00, // Command Details
                            0x02, 0x02, 0x82, 0x81, // Device Identities
                            0x83, 0x01, 0x00, // Command Result
                            0xB6, 0x2F, 0xC0, 0x00, 0x00, 0x2F, 0x04, 0x0D, 0x00, // Channel Data
                            0x01, 0x00, 0x00, 0x01, 0xF4, 0xFA, 0x59, 0x00, 0x01,
                            0xF8, 0x42, 0x05, 0x78, 0x10, 0x78, 0x18, 0x4F, 0x72,
                            0x61, 0x6E, 0x67, 0x65, 0x20, 0x4C, 0x61, 0x62, 0x73,
                            0x20, 0x43, 0x41, 0x54, 0x54, 0x50, 0x20, 0x73, 0x65,
                            0x72, 0x76, 0x65, 0x72,
                            0xB7, 0x01, 0x00 // Channel Data Length
                        };
                        size_t len = sizeof(buf)/sizeof(char);
                        i = ste_cat_terminal_response(cat, client_tag, buf, len);
                        if (i)
                        {
                            printf("#\tPC test: create TR failed.\n");
                            ret_val = -1;
                        }
                    }
                    break;
                    case STE_APDU_CMD_TYPE_CLOSE_CHANNEL:
                    {
                        char buf[] = {
                            0x81, 0x03, 0x01, 0x41, 0x00, // Command Details
                            0x02, 0x02, 0x82, 0x81, // Device Identities
                            0x83, 0x01, 0x00 // Command Result
                        };
                        size_t len = sizeof(buf)/sizeof(char);
                        i = ste_cat_terminal_response(cat, client_tag, buf, len);
                        if (i)
                        {
                            printf("#\tPC test: create TR failed.\n");
                            ret_val = -1;
                        }
                    }
                    break;
                    default:
                    {
                        printf("#\tPC test: PC kind = 0x%x type = 0x%x NOT Handled\n", kind, type);
                        break;
                    }
                }  //end switch (type)
                ste_parsed_apdu_delete(parsed_apdu);
                ste_apdu_delete(apdu);
            }  //end if (apdu)
        }  //end if (sd->cause == STE_CAT_CAUSE_PC)
        else if (sd->cause == STE_CAT_CAUSE_TR)
        {
            ste_cat_tr_response_t  *tr_rsp_p;

            tr_rsp_p = sd->data;
            if (!tr_rsp_p)
            {
                printf("#\tPC test: No data for this TR response.\n");
                sync_data_delete(sd);
                break;
            }
            printf("#\tPC test: TR response. Status = 0x%x, sw1=0x%x 0x%x\n", tr_rsp_p->status,
                                                                              tr_rsp_p->status_word.sw1, tr_rsp_p->status_word.sw2);
            is_done = 1;
            ret_val = 0;
        }
        else if (sd->cause == STE_CAT_CAUSE_PC_SETUP_CALL_IND ) {
          ste_cat_pc_setup_call_ind_t  *call_p;
          printf("#\tPC test: Call Setup Indication\n");
          call_p = sd->data;
          if (!call_p)
          {
              printf("#\tPC SETUP CALL test: No data for this indication.\n");
              sync_data_delete(sd);
              break;
          }
          printf("#\tPC SETUP CALL test: PC SETUP CALL IND. duration = 0x%x\n", call_p->duration);
          printf("#\tPC SETUP CALL test: PC SETUP CALL IND. redial = 0x%x\n", call_p->redial);
          printf("#\tPC SETUP CALL test: PC SETUP CALL IND. option = 0x%x\n", call_p->option);
          i = ste_cat_answer_call(cat, TCAT_CT_STE_CAT_CAUSE_ANSWER_CALL, 1);
        }
        else if (sd->cause == STE_CAT_CAUSE_ANSWER_CALL)
        {
            ste_cat_answer_call_response_t  *rsp_p;
            rsp_p = sd->data;
            if (!rsp_p)
            {
                printf("#\tPC ANSWER CALL test: No data for this response.\n");
                sync_data_delete(sd);
                break;
            }
            printf("#\tPC ANSWER CALL test: PC SETUP CALL IND. status = 0x%x\n", rsp_p->status);
            /*
                    is_done = 1;
                    ret_val = 0;
                    */
        }
        else if (sd->cause == STE_CAT_CAUSE_SESSION_END_IND)
        {
            is_done = 1;
            ret_val = 0;
        }
        sync_data_delete(sd);
    }  //end for (;is_done == 0;)

    return ret_val;
}

const char *test_proactive_command()
{
    ste_sim_t              *cat;
    ste_sim_closure_t       pc;
    int                     i;
    cat_barrier_t          *bar = cat_barrier_new();

    pc.func = the_callback_with_sync_data;
    pc.user_data = bar;

    cat = ste_sim_new(&pc);
    if (!cat)
    {
        return "ste_sim_new failed.";
    }

    TTEST(ste_sim_connect, TCAT_CT_STE_SIM_CAUSE_CONNECT, cat, bar, 100);

    TTEST_R(ste_cat_register, TCAT_CT_STE_CAT_CAUSE_REGISTER, cat, reg_events, bar, 100);
    // CAT enable
    TTEST_R(ste_cat_enable, TCAT_CT_STE_CAT_CAUSE_CAT_ENABLE, cat, STE_CAT_ENABLE_TYPE_ENABLE, bar, 100);

    i = main_proactive_command(cat, bar);

    TTEST(ste_cat_deregister, TCAT_CT_STE_SIM_CAUSE_DEREGISTER, cat, bar, 100);

    TTEST(ste_sim_disconnect, TCAT_CT_STE_SIM_CAUSE_DISCONNECT, cat, bar, 100);

    ste_sim_delete(cat, TCAT_CT_NOT_USED);

    cat_barrier_delete(bar);

    sleep(1);
    if (i)
    {
        printf("#\ttest_proactive_command failed.\n");
        return "test_proactive_command failed.";
    }
    else
    {
        printf("#\ttest_proactive_command succeeded.\n");
        return 0;
    }
}


static int main_envelope_command(ste_sim_t *cat, cat_barrier_t *bar, ste_cat_apdu_data_t *apdu_p)
{
    int   i;
    int   ret_val = -1;
    int   is_done = 0;

    if (!cat || !bar || !apdu_p)
    {
        return -1;
    }
    i = ste_cat_envelope_command(cat, TCAT_CT_STE_CAT_CAUSE_EC, apdu_p->buf, apdu_p->len);
    if (i)
    {
        printf("#\tEC test: ste_cat_envelope_command failed.\n");
        return -1;
    }

    for (;is_done == 0;)
    {
        sync_data_t            *sd;

        cat_barrier_set(bar);
        i = cat_barrier_timedwait(bar, (void **) &sd, 10000);
        if (i)
        {
            printf("#\tEC test: Timed out\n");
            continue;
        }
        if (!sd)
        {
            printf("#\tEC test: No sync data.\n");
            continue;
        }

        printf("#\tEC test:  %2d %p : %s\n", sd->cause, (void*) sd->data, ste_sim_cause_name[sd->cause]);

        if (sd->cause == STE_SIM_CAUSE_DISCONNECT || sd->cause == STE_SIM_CAUSE_HANGUP)
        {
            sync_data_delete(sd);
            break;
        }

        if (sd->cause == STE_CAT_CAUSE_EC)
        {
            ste_cat_ec_response_t  *ec_rsp_p;

            ec_rsp_p = sd->data;
            if (!ec_rsp_p)
            {
                printf("#\tEC test: No data for this EC response.\n");
                sync_data_delete(sd);
                break;
            }

            printf("#\tEC test: EC response. Status = 0x%x\n", ec_rsp_p->ec_status);
            if (ec_rsp_p->apdu.len > 0 && ec_rsp_p->apdu.buf)
            {
                printf("#\tEC test: EC response. APDU length = %d\n", ec_rsp_p->apdu.len);
                free(ec_rsp_p->apdu.buf);
            }
            is_done = 1;
            ret_val = 0;
        }
        sync_data_delete(sd);
    }  //end for (;is_done == 0;)

    return ret_val;
}

const char *test_envelope_command()
{
    ste_sim_t              *cat;
    ste_sim_closure_t       pc;
    int                     i;
    cat_barrier_t          *bar = cat_barrier_new();
    ste_cat_apdu_data_t     apdu;
    uint8_t                 APDU_MenuSelection[] =
    {
        0xD3, 0x07, 0x82, 0x02, 0x01, 0x81, 0x90, 0x01, 0x12
    };

    pc.func = the_callback_with_sync_data;
    pc.user_data = bar;

    cat = ste_sim_new(&pc);
    if (!cat)
    {
        return "ste_sim_new failed.";
    }

    TTEST(ste_sim_connect, TCAT_CT_STE_SIM_CAUSE_CONNECT, cat, bar, 100);

    TTEST_R(ste_cat_register, TCAT_CT_STE_CAT_CAUSE_REGISTER, cat, reg_events, bar, 100);
    // CAT enable
    TTEST_R(ste_cat_enable, TCAT_CT_STE_CAT_CAUSE_CAT_ENABLE, cat, STE_CAT_ENABLE_TYPE_ENABLE, bar, 100);

    apdu.buf = (char*)APDU_MenuSelection;
    apdu.len = sizeof(APDU_MenuSelection);
    i = main_envelope_command(cat, bar, &apdu);

    TTEST(ste_cat_deregister, TCAT_CT_STE_SIM_CAUSE_DEREGISTER, cat, bar, 100);

    TTEST(ste_sim_disconnect, TCAT_CT_STE_SIM_CAUSE_DISCONNECT, cat, bar, 100);

    ste_sim_delete(cat, TCAT_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);
    if (i)
    {
        printf("#\ttest_envelope_command failed.\n");
        return "test_envelope_command failed.";
    }
    else
    {
        printf("#\ttest_envelope_command succeeded.\n");
        return 0;
    }
}

const char *test_envelope_command_bip()
{
    ste_sim_t              *cat;
    ste_sim_closure_t       pc;
    int                     i;
    cat_barrier_t          *bar = cat_barrier_new();
    ste_cat_apdu_data_t     apdu;
    uint8_t                 APDU_DataAvail[] = {
        0xD6, 0x0E, 0x19, 0x01, 0x09, 0x82, 0x02, 0x82, 0x81, 0xB8, 0x02, 0x81, 0x00, 0xB7, 0x01, 0x2F
    };

    pc.func = the_callback_with_sync_data;
    pc.user_data = bar;

    cat = ste_sim_new(&pc);
    if (!cat)
    {
        return "ste_sim_new failed.";
    }

    TTEST(ste_sim_connect, TCAT_CT_STE_SIM_CAUSE_CONNECT, cat, bar, 100);

    TTEST_R(ste_cat_register, TCAT_CT_STE_CAT_CAUSE_REGISTER, cat, reg_events, bar, 100);
    // CAT enable
    TTEST_R(ste_cat_enable, TCAT_CT_STE_CAT_CAUSE_CAT_ENABLE, cat, STE_CAT_ENABLE_TYPE_ENABLE, bar, 100);

    apdu.buf = (char*)APDU_DataAvail;
    apdu.len = sizeof(APDU_DataAvail);
    i = main_envelope_command(cat, bar, &apdu);

    TTEST(ste_cat_deregister, TCAT_CT_STE_SIM_CAUSE_DEREGISTER, cat, bar, 100);

    TTEST(ste_sim_disconnect, TCAT_CT_STE_SIM_CAUSE_DISCONNECT, cat, bar, 100);

    ste_sim_delete(cat, TCAT_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);
    if (i)
    {
        printf("#\ttest_envelope_command_bip failed.\n");
        return "test_envelope_command_bip failed.";
    }
    else
    {
        printf("#\ttest_envelope_command_bip succeeded.\n");
        return 0;
    }
}

static int main_get_cat_status(ste_sim_t *cat, cat_barrier_t *bar)
{
    int   i;
    int   ret_val = -1;
    int   is_done = 0;
    int   retries = 10;

    if (!cat || !bar)
    {
        return -1;
    }
    i = ste_cat_get_cat_status(cat, TCAT_CT_STE_CAT_CAUSE_GET_CAT_STATUS);
    if (i)
    {
        printf("#\tGET CAT STATUS test: ste_cat_get_cat_status failed.\n");
        return -1;
    }

    for (;is_done == 0 && retries > 0;)
    {
        sync_data_t            *sd;

        cat_barrier_set(bar);
        i = cat_barrier_timedwait(bar, (void **) &sd, 10000);
        if (i)
        {
            printf("#\tGET CAT STATUS test: Timed out\n");
            retries--;
            continue;
        }
        if (!sd)
        {
            printf("#\tGET CAT STATUS test: No sync data.\n");
            retries--;
            continue;
        }

        printf("#\tGET CAT STATUS test:  %2d %p : %s\n", sd->cause, (void*) sd->data, ste_sim_cause_name[sd->cause]);

        if (sd->cause == STE_SIM_CAUSE_DISCONNECT || sd->cause == STE_SIM_CAUSE_HANGUP)
        {
            sync_data_delete(sd);
            break;
        }

        if (sd->cause == STE_CAT_CAUSE_GET_CAT_STATUS ||
            sd->cause == STE_CAT_CAUSE_IND_CAT_STATUS)
        {
            ste_cat_cat_status_t  *rsp_p;

            rsp_p = sd->data;
            if (!rsp_p)
            {
                printf("#\tGET CAT STATUS test: No data for this response.\n");
                sync_data_delete(sd);
                break;
            }

            printf("#\tGET CAT STATUS test: GET CAT STATUS response. Status = 0x%x\n", rsp_p->status);
            is_done = 1;
            ret_val = 0;
        }
        sync_data_delete(sd);
    }  //end for (;is_done == 0;)

    return ret_val;
}

const char *test_get_cat_status()
{
    ste_sim_t              *cat;
    ste_sim_closure_t       pc;
    int                     i;
    cat_barrier_t          *bar = cat_barrier_new();

    pc.func = the_callback_with_sync_data;
    pc.user_data = bar;

    cat = ste_sim_new(&pc);
    if (!cat)
    {
        return "ste_sim_new failed.";
    }

    TTEST(ste_sim_connect, TCAT_CT_STE_SIM_CAUSE_CONNECT, cat, bar, 100);

    TTEST_R(ste_cat_register, TCAT_CT_STE_CAT_CAUSE_REGISTER, cat, reg_events, bar, 100);
    // CAT enable
    TTEST_R(ste_cat_enable, TCAT_CT_STE_CAT_CAUSE_CAT_ENABLE, cat, STE_CAT_ENABLE_TYPE_ENABLE, bar, 100);

    i = main_get_cat_status(cat, bar);

    {
        printf("Waiting for 10 seconds to see if any PC events");
        sleep(10);
    }
    TTEST(ste_cat_deregister, TCAT_CT_STE_SIM_CAUSE_DEREGISTER, cat, bar, 100);

    TTEST(ste_sim_disconnect, TCAT_CT_STE_SIM_CAUSE_DISCONNECT, cat, bar, 100);

    ste_sim_delete(cat, TCAT_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);
    if (i)
    {
        printf("#\ttest_get_cat_status failed.\n");
        return "test_get_cat_status failed.";
    }
    else
    {
        printf("#\ttest_get_cat_status succeeded.\n");
        return 0;
    }
}

static int main_get_terminal_profile(ste_sim_t *cat, cat_barrier_t *bar, char* profile_p, unsigned int p_len, int* profile_len)
{
    int   i;
    int   ret_val = -1;
    int   is_done = 0;

    if (!cat || !bar)
    {
        return -1;
    }
    i = ste_cat_get_terminal_profile(cat, TCAT_CT_STE_CAT_CAUSE_GET_TERMINAL_PROFILE);
    if (i)
    {
        printf("#\tGET PROFILE test: ste_cat_get_terminal_profile failed.\n");
        return -1;
    }

    for (;is_done == 0;)
    {
        sync_data_t            *sd;

        cat_barrier_set(bar);
        i = cat_barrier_timedwait(bar, (void **) &sd, 10000);
        if (i)
        {
            printf("#\tGET PROFILE test: Timed out\n");
            continue;
        }
        if (!sd)
        {
            printf("#\tGET PROFILE test: No sync data.\n");
            continue;
        }

        printf("#\tGET PROFILE test:  %2d %p : %s\n", sd->cause, (void*) sd->data, ste_sim_cause_name[sd->cause]);

        if (sd->cause == STE_SIM_CAUSE_DISCONNECT || sd->cause == STE_SIM_CAUSE_HANGUP)
        {
            sync_data_delete(sd);
            break;
        }

        if (sd->cause == STE_CAT_CAUSE_GET_TERMINAL_PROFILE)
        {
            ste_cat_get_terminal_profile_response_t  *rsp_p;

            rsp_p = sd->data;
            if (!rsp_p)
            {
                printf("#\tGET PROFILE test: No data for this EC response.\n");
                sync_data_delete(sd);
                break;
            }

            printf("#\tGET PROFILE test: GET PROFILE response. Status = 0x%x\n", rsp_p->status);
            if (rsp_p->profile_len > 0 && rsp_p->profile_len <= p_len && rsp_p->profile_p)
            {
                printf("#\tGET PROFILE test: EC response. Profile length = %d\n", rsp_p->profile_len);

                memcpy(profile_p, rsp_p->profile_p, rsp_p->profile_len);
                *profile_len = rsp_p->profile_len;
            }
            if (rsp_p->profile_p)
            {
                free(rsp_p->profile_p);
            }
            is_done = 1;
            ret_val = 0;
        }
        sync_data_delete(sd);
    }  //end for (;is_done == 0;)

    return ret_val;
}

const char *test_get_terminal_profile()
{
    ste_sim_t              *cat;
    ste_sim_closure_t       pc;
    int                     i;
    cat_barrier_t          *bar = cat_barrier_new();
    char                    get_profile[40];
    int                     get_profile_len;

    pc.func = the_callback_with_sync_data;
    pc.user_data = bar;

    cat = ste_sim_new(&pc);
    if (!cat)
    {
        return "ste_sim_new failed.";
    }

    TTEST(ste_sim_connect, TCAT_CT_STE_SIM_CAUSE_CONNECT, cat, bar, 100);

    TTEST_R(ste_cat_register, TCAT_CT_STE_CAT_CAUSE_REGISTER, cat, reg_events, bar, 100);
    // CAT enable
    TTEST_R(ste_cat_enable, TCAT_CT_STE_CAT_CAUSE_CAT_ENABLE, cat, STE_CAT_ENABLE_TYPE_ENABLE, bar, 100);

    i = main_get_terminal_profile(cat, bar, get_profile, sizeof(get_profile), &get_profile_len);

    TTEST(ste_cat_deregister, TCAT_CT_STE_SIM_CAUSE_DEREGISTER, cat, bar, 100);

    TTEST(ste_sim_disconnect, TCAT_CT_STE_SIM_CAUSE_DISCONNECT, cat, bar, 100);

    ste_sim_delete(cat, TCAT_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);
    if (i)
    {
        printf("#\ttest_get_cat_status failed.\n");
        return "test_get_cat_status failed.";
    }
    else
    {
        printf("#\ttest_get_cat_status succeeded.\n");
        return 0;
    }
}


static int main_set_terminal_profile(ste_sim_t *cat, cat_barrier_t *bar, char* profile_p, unsigned profile_len)
{
    int   i;
    int   ret_val = -1;
    int   is_done = 0;

    if (!cat || !bar || !profile_p)
    {
        return -1;
    }
    i = ste_cat_set_terminal_profile(cat, TCAT_CT_STE_CAT_CAUSE_SET_TERMINAL_PROFILE, profile_p, profile_len);
    if (i)
    {
        printf("#\tSET TERMINAL PROFILE test: ste_cat_set_terminal_profile failed.\n");
        return -1;
    }

    for (;is_done == 0;)
    {
        sync_data_t            *sd;

        cat_barrier_set(bar);
        i = cat_barrier_timedwait(bar, (void **) &sd, 10000);
        if (i)
        {
            printf("#\tSET TERMINAL PROFILE test: Timed out\n");
            continue;
        }
        if (!sd)
        {
            printf("#\tSET TERMINAL PROFILE test: No sync data.\n");
            continue;
        }

        printf("#\tSET TERMINAL PROFILE test:  %2d %p : %s\n", sd->cause, (void*) sd->data, ste_sim_cause_name[sd->cause]);

        if (sd->cause == STE_SIM_CAUSE_DISCONNECT || sd->cause == STE_SIM_CAUSE_HANGUP)
        {
            sync_data_delete(sd);
            break;
        }

        if (sd->cause == STE_CAT_CAUSE_SET_TERMINAL_PROFILE)
        {
            ste_cat_set_terminal_profile_response_t  *rsp_p;

            rsp_p = sd->data;
            if (!rsp_p)
            {
                printf("#\tSET TERMINAL PROFILE test: No data for this response.\n");
                sync_data_delete(sd);
                break;
            }

            printf("#\tSET TERMINAL PROFILE test: SET TERMINAL PROFILE response. Status = 0x%x\n", rsp_p->status);
            is_done = 1;
            ret_val = 0;
        }
        sync_data_delete(sd);
    }  //end for (;is_done == 0;)

    return ret_val;
}

const char *test_set_terminal_profile()
{
    ste_sim_t              *cat;
    ste_sim_closure_t       pc;
    int                     i;
    cat_barrier_t          *bar = cat_barrier_new();
    char                    profile[] =
    {
        0xCA, 0xFE, 0xBA, 0xBE
    };

    pc.func = the_callback_with_sync_data;
    pc.user_data = bar;

    cat = ste_sim_new(&pc);
    if (!cat)
    {
        return "ste_sim_new failed.";
    }

    TTEST(ste_sim_connect, TCAT_CT_STE_SIM_CAUSE_CONNECT, cat, bar, 100);

    TTEST_R(ste_cat_register, TCAT_CT_STE_CAT_CAUSE_REGISTER, cat, reg_events, bar, 100);
    // CAT enable
    TTEST_R(ste_cat_enable, TCAT_CT_STE_CAT_CAUSE_CAT_ENABLE, cat, STE_CAT_ENABLE_TYPE_ENABLE, bar, 100);

    i = main_set_terminal_profile(cat, bar, profile, sizeof(profile));

    TTEST(ste_cat_deregister, TCAT_CT_STE_SIM_CAUSE_DEREGISTER, cat, bar, 100);

    TTEST(ste_sim_disconnect, TCAT_CT_STE_SIM_CAUSE_DISCONNECT, cat, bar, 100);

    ste_sim_delete(cat, TCAT_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);
    if (i)
    {
        printf("#\ttest_set_terminal_profile failed.\n");
        return "test_set_terminal_profile failed.";
    }
    else
    {
        printf("#\ttest_set_terminal_profile succeeded.\n");
        return 0;
    }
}


static int main_set_and_get_terminal_profile(ste_sim_t *cat, cat_barrier_t *bar, char* set_profile_p, unsigned set_profile_len)
{
    int   ret_val = -1;

    char get_profile[40];
    int get_profile_len;

    ret_val = main_set_terminal_profile(cat, bar, set_profile_p, set_profile_len);
    if (ret_val)
    {
        return -1;
    }

    ret_val = main_get_terminal_profile(cat, bar, get_profile, sizeof(get_profile), &get_profile_len);
    if (ret_val)
    {
        return -1;
    }
    else
    {
        //assert that get equals what have been set.
        ret_val = memcmp(set_profile_p, get_profile, set_profile_len);
    }
    return ret_val;

}

const char *test_set_and_get_terminal_profile()
{
    ste_sim_t              *cat;
    ste_sim_closure_t       pc;
    int                     i;
    cat_barrier_t          *bar = cat_barrier_new();
    char                    profile[] =
    {
        0xCA, 0xFE, 0xBA, 0xBE, 0x01, 0x02, 0x03, 0x04, 0x05
    };

    pc.func = the_callback_with_sync_data;
    pc.user_data = bar;

    cat = ste_sim_new(&pc);
    if (!cat)
    {
        return "ste_sim_new failed.";
    }

    TTEST(ste_sim_connect, TCAT_CT_STE_SIM_CAUSE_CONNECT, cat, bar, 100);

    TTEST_R(ste_cat_register, TCAT_CT_STE_CAT_CAUSE_REGISTER, cat, reg_events, bar, 100);
    // CAT enable
    TTEST_R(ste_cat_enable, TCAT_CT_STE_CAT_CAUSE_CAT_ENABLE, cat, STE_CAT_ENABLE_TYPE_ENABLE, bar, 100);

    i = main_set_and_get_terminal_profile(cat, bar, profile, sizeof(profile));

    TTEST(ste_cat_deregister, TCAT_CT_STE_SIM_CAUSE_DEREGISTER, cat, bar, 100);

    TTEST(ste_sim_disconnect, TCAT_CT_STE_SIM_CAUSE_DISCONNECT, cat, bar, 100);

    ste_sim_delete(cat, TCAT_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);
    if (i)
    {
        printf("#\ttest_set_and_get_terminal_profile failed.\n");
        return "test_set_and_get_terminal_profile failed.";
    }
    else
    {
        printf("#\ttest_set_and_get_terminal_profile succeeded.\n");
        return 0;
    }
}


static int main_pc_notification(ste_sim_t *cat, cat_barrier_t *bar)
{
    int   i;
    int   ret_val = -1;
    int   is_done = 0;
    uintptr_t client_tag = 0;

    if (!cat || !bar)
    {
        return -1;
    }

    for (;is_done == 0;)
    {
        sync_data_t            *sd;

        cat_barrier_set(bar);
        i = cat_barrier_timedwait(bar, (void **) &sd, 10000);
        if (i)
        {
            printf("#\tPC test: Timed out\n");
            continue;
        }
        if (!sd)
        {
            printf("#\tPC test: No sync data.\n");
            continue;
        }

        printf("#\tPC test:  %2d %p : %s\n", sd->cause, (void*) sd->data, ste_sim_cause_name[sd->cause]);

        if (sd->cause == STE_SIM_CAUSE_DISCONNECT || sd->cause == STE_SIM_CAUSE_HANGUP)
        {
            sync_data_delete(sd);
            break;
        }

        if (sd->cause == STE_CAT_CAUSE_PC_NOTIFICATION)
        {
            ste_cat_pc_ind_t       *pc_ind_p;
            ste_cat_apdu_data_t    *apdu_data;
            ste_apdu_t             *apdu;

            pc_ind_p = sd->data;
            if (!pc_ind_p)
            {
                printf("#\tPC test: No data for this PC notification.\n");
                sync_data_delete(sd);
                continue;
            }
            apdu_data = &(pc_ind_p->apdu);
            client_tag = pc_ind_p->simd_tag;
            apdu = ste_apdu_new((uint8_t *) (apdu_data->buf), apdu_data->len);
            free(apdu_data->buf);

            if (apdu)
            {
                ste_parsed_apdu_t      *parsed_apdu = NULL;
                ste_sat_apdu_error_t    err;
                int                     kind;
                int                     type;

                err = ste_apdu_parse(apdu, &parsed_apdu);
                if (err != STE_SAT_APDU_ERROR_NONE)
                {
                    printf("#\tPC test: Parse APDU from CATD failed.\n");
                    //free resources
                    ste_apdu_delete(apdu);
                    sync_data_delete(sd);
                    ste_parsed_apdu_delete(parsed_apdu);
                    continue;
                }
                type = ste_parsed_apdu_get_type(parsed_apdu);
                kind = ste_parsed_apdu_get_kind(parsed_apdu);
                printf("#\tPC test: PC kind = 0x%x type = 0x%x\n", kind, type);

                ste_parsed_apdu_delete(parsed_apdu);
                ste_apdu_delete(apdu);
            }  //end if (apdu)
            is_done = 1;
            ret_val = 0;
        }  //end if (sd->cause == STE_CAT_CAUSE_PC_NOTIFICATION)
        sync_data_delete(sd);
    }  //end for (;is_done == 0;)

    return ret_val;
}

const char *test_pc_notification()
{
    ste_sim_t              *cat;
    ste_sim_closure_t       pc;
    int                     i;
    cat_barrier_t          *bar = cat_barrier_new();

    pc.func = the_callback_with_sync_data;
    pc.user_data = bar;

    cat = ste_sim_new(&pc);
    if (!cat)
    {
        return "ste_sim_new failed.";
    }

    TTEST(ste_sim_connect, TCAT_CT_STE_SIM_CAUSE_CONNECT, cat, bar, 100);

    TTEST_R(ste_cat_register, TCAT_CT_STE_CAT_CAUSE_REGISTER, cat, reg_events, bar, 100);
    // CAT enable
    TTEST_R(ste_cat_enable, TCAT_CT_STE_CAT_CAUSE_CAT_ENABLE, cat, STE_CAT_ENABLE_TYPE_ENABLE, bar, 100);

    i = main_pc_notification(cat, bar);

    TTEST(ste_cat_deregister, TCAT_CT_STE_SIM_CAUSE_DEREGISTER, cat, bar, 100);

    TTEST(ste_sim_disconnect, TCAT_CT_STE_SIM_CAUSE_DISCONNECT, cat, bar, 100);

    ste_sim_delete(cat, TCAT_CT_NOT_USED);

    cat_barrier_delete(bar);

    sleep(1);
    if (i)
    {
        printf("#\ttest_pc_notification failed.\n");
        return "test_pc_notification failed.";
    }
    else
    {
        printf("#\ttest_pc_notification succeeded.\n");
        return 0;
    }
}




static int main_pc_setup_call(ste_sim_t *cat, cat_barrier_t *bar)
{
    int   i;
    int   ret_val = -1;
    int   is_done = 0;

    if (!cat || !bar)
    {
        return -1;
    }

    for (;is_done == 0;)
    {
        sync_data_t            *sd;

        cat_barrier_set(bar);
        i = cat_barrier_timedwait(bar, (void **) &sd, 10000);
        if (i)
        {
            printf("#\tPC SETUP CALL test: Timed out\n");
            continue;
        }
        if (!sd)
        {
            printf("#\tPC SETUP CALL test: No sync data.\n");
            continue;
        }

        printf("#\tPC SETUP CALL test:  %2d %p : %s\n", sd->cause, (void*) sd->data, ste_sim_cause_name[sd->cause]);

        if (sd->cause == STE_SIM_CAUSE_DISCONNECT || sd->cause == STE_SIM_CAUSE_HANGUP)
        {
            sync_data_delete(sd);
            break;
        }

        if (sd->cause == STE_CAT_CAUSE_PC_SETUP_CALL_IND)
        {
            ste_cat_pc_setup_call_ind_t  *call_p;

            call_p = sd->data;
            if (!call_p)
            {
                printf("#\tPC SETUP CALL test: No data for this indication.\n");
                sync_data_delete(sd);
                break;
            }

            printf("#\tPC SETUP CALL test: PC SETUP CALL IND. duration = 0x%x\n", call_p->duration);
            printf("#\tPC SETUP CALL test: PC SETUP CALL IND. redial = 0x%x\n", call_p->redial);
            printf("#\tPC SETUP CALL test: PC SETUP CALL IND. option = 0x%x\n", call_p->option);

            i = ste_cat_answer_call(cat, TCAT_CT_STE_CAT_CAUSE_ANSWER_CALL, 1);
        }
        if (sd->cause == STE_CAT_CAUSE_ANSWER_CALL)
        {
            ste_cat_answer_call_response_t  *rsp_p;

            rsp_p = sd->data;
            if (!rsp_p)
            {
                printf("#\tPC SETUP CALL test: No data for this response.\n");
                sync_data_delete(sd);
                break;
            }

            printf("#\tPC SETUP CALL test: PC SETUP CALL IND. status = 0x%x\n", rsp_p->status);

            is_done = 1;
            ret_val = 0;
        }
        sync_data_delete(sd);
    }  //end for (;is_done == 0;)

    return ret_val;
}

const char *test_pc_setup_call()
{
    ste_sim_t              *cat;
    ste_sim_closure_t       pc;
    int                     i;
    cat_barrier_t          *bar = cat_barrier_new();

    pc.func = the_callback_with_sync_data;
    pc.user_data = bar;

    cat = ste_sim_new(&pc);
    if (!cat)
    {
        return "ste_sim_new failed.";
    }

    TTEST(ste_sim_connect, TCAT_CT_STE_SIM_CAUSE_CONNECT, cat, bar, 100);

    TTEST_R(ste_cat_register, TCAT_CT_STE_CAT_CAUSE_REGISTER, cat, reg_events, bar, 100);
    // CAT enable
    TTEST_R(ste_cat_enable, TCAT_CT_STE_CAT_CAUSE_CAT_ENABLE, cat, STE_CAT_ENABLE_TYPE_ENABLE, bar, 100);

    i = main_pc_setup_call(cat, bar);

    TTEST(ste_cat_deregister, TCAT_CT_STE_SIM_CAUSE_DEREGISTER, cat, bar, 100);

    TTEST(ste_sim_disconnect, TCAT_CT_STE_SIM_CAUSE_DISCONNECT, cat, bar, 100);

    ste_sim_delete(cat, TCAT_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);
    if (i)
    {
        printf("#\ttest_pc_setup_call failed.\n");
        return "test_pc_setup_call failed.";
    }
    else
    {
        printf("#\ttest_pc_setup_call succeeded.\n");
        return 0;
    }
}


static int main_ec_event_download(ste_sim_t *cat,
                                  cat_barrier_t *bar,
                                  ste_cat_event_download_type_t event_type,
                                  void * event_data_p)
{
    int   i;
    int   ret_val = 0;
#if 0 // Unused code
    int   is_done = 0;
#endif

    if (!cat || !bar)
    {
        return -1;
    }

    i = ste_cat_event_download(cat, TCAT_CT_STE_CAT_CAUSE_EVENT_DOWNLOAD, event_type, event_data_p);

    if (i)
    {
        printf("#\tEVENT DOWNLOAD test: ste_cat_event_download failed.\n");
        return -1;
    }

#if 0 // Unused code
    for (;is_done == 0;)
    {
        sync_data_t            *sd;

        cat_barrier_set(bar);
        i = cat_barrier_timedwait(bar, (void **) &sd, 10000);
        if (i)
        {
            printf("#\tEVENT DOWNLOAD test: Timed out\n");
            continue;
        }
        if (!sd)
        {
            printf("#\tEVENT DOWNLOAD test: No sync data.\n");
            continue;
        }

        printf("#\tEVENT DOWNLOAD test:  %2d %p : %s\n", sd->cause, (void*) sd->data, ste_sim_cause_name[sd->cause]);

        if (sd->cause == STE_SIM_CAUSE_DISCONNECT || sd->cause == STE_SIM_CAUSE_HANGUP)
        {
            sync_data_delete(sd);
            break;
        }

        if (sd->cause == STE_CAT_CAUSE_EVENT_DOWNLOAD)
        {
            ste_cat_event_download_response_t * rsp_p;

            rsp_p = sd->data;
            if (!rsp_p)
            {
                printf("#\tEVENT DOWNLOAD test: No data for this EC EVENT DOWNLOAD response.\n");
                sync_data_delete(sd);
                break;
            }

            printf("#\tEVENT DOWNLOAD test: EC EVENT DOWNLOAD response. EVENT DOWNLOAD Result = 0x%x\n", rsp_p->status);

            is_done = 1;
            ret_val = 0;
        }
        sync_data_delete(sd);
    }  //end for (;is_done == 0;)
#endif
    return ret_val;
}

const char             *test_ecEventDownload()
{
    ste_sim_t              *cat;
    ste_sim_closure_t       pc;
    cat_barrier_t          *bar = cat_barrier_new();
    int                     i;

    ste_cat_event_mt_call_t        event_data;
    ste_cat_event_download_type_t  event_type;
    uint8_t call_addr[] = "46111";
    uint8_t sub_addr[] = "2222";

    pc.func = the_callback_with_sync_data;
    pc.user_data = bar;

    cat = ste_sim_new(&pc);
    if (!cat)
        return "ste_sim_new failed.";


    TTEST(ste_sim_connect, TCAT_CT_STE_SIM_CAUSE_CONNECT, cat, bar, 100);

    sleep(1);

    TTEST_R(ste_cat_register, TCAT_CT_STE_CAT_CAUSE_REGISTER, cat, reg_events, bar, 100);
    // CAT enable
    TTEST_R(ste_cat_enable, TCAT_CT_STE_CAT_CAUSE_CAT_ENABLE, cat, STE_CAT_ENABLE_TYPE_ENABLE, bar, 100);

    event_type = STE_CAT_EVENT_DOWNLOAD_TYPE_MT_CALL;
    event_data.transaction_id = 0x01;
    event_data.address.ton = STE_SIM_TON_INTERNATIONAL;
    event_data.address.npi = STE_SIM_NPI_ISDN;
    event_data.address.text_coding = STE_SIM_BCD;
    event_data.address.no_of_characters = 10;
    event_data.address.text_p = call_addr;
    event_data.sub_address.len = 0x04;
    event_data.sub_address.sub_address_p = sub_addr;

    i = main_ec_event_download(cat, bar, event_type, &event_data);

    TTEST(ste_cat_deregister, TCAT_CT_STE_SIM_CAUSE_DEREGISTER, cat, bar, 100);

    TTEST(ste_sim_disconnect, TCAT_CT_STE_SIM_CAUSE_DISCONNECT, cat, bar, 100);

    ste_sim_delete(cat, TCAT_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);

    if (i)
    {
        printf("#\ttest_ecEventDownload failed.\n");
        return "test_ecEventDownload failed.";
    }
    else
    {
        printf("#\ttest_ecEventDownload succeeded.\n");
        return 0;
    }
}

static int main_send_raw_apdu(ste_sim_t *cat, cat_barrier_t *bar, char* apdu, int apdu_len )
{
    int   i;

    if (!cat || !bar)
    {
        return -1;
    }

    i = ste_cat_raw_apdu( cat, TCAT_CT_STE_CAT_CAUSE_RAW_APDU, apdu, apdu_len );
    if (i)
    {
        printf("#\tSEND RAW APDU test: ste_cat_raw_apdu failed.\n");
        return -1;
    }

    return 0;
}

/* This feature is not supported by modem currently */
const char             *test_send_raw_apdu()
{
    ste_sim_t              *cat;
    ste_sim_closure_t       pc;
    int                     i;
    cat_barrier_t          *bar = cat_barrier_new();
    char apdu[] = { 0xa0, 0xa4, 0x00, 0x00, 0x02, 0x3f, 0x00
    };

    pc.func = the_callback_with_sync_data;
    pc.user_data = bar;

    cat = ste_sim_new(&pc);
    if (!cat)
    {
        return "ste_sim_new failed.";
    }

    TTEST(ste_sim_connect, TCAT_CT_STE_SIM_CAUSE_CONNECT, cat, bar, 100);

    TTEST_R(ste_cat_register, TCAT_CT_STE_CAT_CAUSE_REGISTER, cat, reg_events, bar, 100);
    // CAT enable
    TTEST_R(ste_cat_enable, TCAT_CT_STE_CAT_CAUSE_CAT_ENABLE, cat, STE_CAT_ENABLE_TYPE_ENABLE, bar, 100);

    i = main_send_raw_apdu(cat, bar, apdu, sizeof(apdu));

    TTEST(ste_cat_deregister, TCAT_CT_STE_SIM_CAUSE_DEREGISTER, cat, bar, 100);

    TTEST(ste_sim_disconnect, TCAT_CT_STE_SIM_CAUSE_DISCONNECT, cat, bar, 100);

    ste_sim_delete(cat, TCAT_CT_NOT_USED);
    cat_barrier_delete(bar);

    sleep(1);
    if (i)
    {
        printf("#\ttest_pc_setup_call failed.\n");
        return "test_pc_setup_call failed.";
    }
    else
    {
        printf("#\ttest_pc_setup_call succeeded.\n");
        return 0;
    }
}

typedef const char     *test_func_t();

static const char *help ();

#define MAX_NAME_SIZE   (64)

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
    TENTRY("ping", test_ping),
    TENTRY("register", test_register),
    TENTRY("connect10", test_connect10),
    TENTRY("repeatregister", test_registerdisconnectXtimes),
    TENTRY("startup", test_startup),
    TENTRY("pc", test_proactive_command),
    TENTRY("ec", test_envelope_command),
    TENTRY("ec_bip", test_envelope_command_bip),
    TENTRY("get_cat_status", test_get_cat_status),
    TENTRY("get_profile", test_get_terminal_profile),
    TENTRY("set_profile", test_set_terminal_profile),
    TENTRY("set_and_get_profile", test_set_and_get_terminal_profile),
    TENTRY("callcontrol", test_ecCallControl),
    TENTRY("callcontrol_pdp", test_ecCallControl_pdp),
    TENTRY("smscontrol", test_ecSmsControl),
    TENTRY("pc_notification", test_pc_notification),
    TENTRY("pc_setupcall", test_pc_setup_call),
    TENTRY("eventdownload", test_ecEventDownload),
    TENTRY("send_raw_apdu", test_send_raw_apdu),
    TENTRY("callcontrol_ss", test_ecSSControl),
    TENTRY("callcontrol_uss",test_ecUSSDControl),
    TENTRY("shutdown", test_shutdown),
    TENTRY("help", help),
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
 * call ./tcat with no test names
 */
static const char      *all_tests[] = {
    "condis",
    "ping",
    "connect10",
    "register",
    "repeatregister",
    "ec",
    "get_cat_status",
    "get_profile",
    "set_profile",
    "set_and_get_profile",
    "callcontrol",
    "callcontrol_pdp",
    "smscontrol",
    "callcontrol_ss",
    "callcontrol_uss",
    "eventdownload",
    "send_raw_apdu",
};


void run_tests(int argc, const char *argv[])
{
    int numFailed = 0;
    int numPassed = 0;

    printf("tcat: entering run_tests\n");

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
            printf("FAILED : %s : %s\n", *argv, s);
            numFailed++;
        } else {
            printf("OK     : %s\n", *argv);
            numPassed++;
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


int main(int argc, const char *argv[])
{
    setProcName(argv[0]);
    setbuf(stdout, 0);

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
            argv++;
            argc--;
        }
        // Remaining args should be test case names.
        if (inter) printf("tcat is interactive\n");
        if (uicc_sim) printf("tcat assumes uicc with SIM instead of USIM");
        run_tests(argc, argv);
    }
    return EXIT_SUCCESS;
}
