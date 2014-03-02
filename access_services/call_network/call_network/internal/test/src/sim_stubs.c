/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <string.h>

#include "cn_log.h"
#include "sim.h"


/* Global variables */
int8_t g_ste_uicc_sim_file_read_ecc__int = 0;
int8_t g_ste_uicc_sim_file_read_fdn__int = 0;
int8_t g_ste_cat_call_control__int = 0;
sim_service_type_t g_ste_uicc_get_service_availability__sim_service_type = 0;
sim_service_type_t g_ste_uicc_get_service_table__sim_service_type_t = 0;
uicc_request_status_t g_ste_uicc_sim_file_read_ecc__uicc_request_status_t = UICC_REQUEST_STATUS_OK;
int8_t g_ste_uicc_get_app_info__int = 0;

int g_ste_uicc_sim_file_read_record__file_id = 0;
int g_ste_uicc_sim_file_read_record__record_id = 0;
int g_ste_uicc_sim_file_read_record__length = 0;
char g_ste_uicc_sim_file_read_record__file_path[256] = { 0 };
char *g_ste_uicc_sim_file_read_record__file_path_p = &g_ste_uicc_sim_file_read_record__file_path[0];

uintptr_t g_sim_client_tag_p = (uintptr_t)NULL;


void clear_global_sim_stub_variables()  /* this function is called prior to each testcase */
{
    g_ste_uicc_sim_file_read_ecc__int = 0;
    g_ste_uicc_sim_file_read_fdn__int = 0;
    g_ste_cat_call_control__int = 0;
    g_ste_uicc_get_service_availability__sim_service_type = 0;
    g_ste_uicc_get_service_table__sim_service_type_t = 0;
    g_sim_client_tag_p = (uintptr_t)NULL;
    g_ste_uicc_sim_file_read_ecc__uicc_request_status_t = UICC_REQUEST_STATUS_OK;

    g_ste_uicc_get_app_info__int = 0;

    g_ste_uicc_sim_file_read_record__file_id = 0;
    g_ste_uicc_sim_file_read_record__record_id = 0;
    g_ste_uicc_sim_file_read_record__length = 0;
    g_ste_uicc_sim_file_read_record__file_path[0] = '\0';
}

int ste_sim_read(ste_sim_t *sim)
{
    (void)sim;

    return 0;
}

ste_sim_t *ste_sim_new_st(const ste_sim_closure_t *closure)
{
    (void)closure;
    return (ste_sim_t *) 1;
}

int ste_sim_startup(ste_sim_t *sim_p, uintptr_t client_tag)
{
    (void)sim_p;
    (void)client_tag;
    return 0;
}

int ste_sim_connect(ste_sim_t *sim_p, uintptr_t client_tag)
{
    (void)sim_p;
    (void)client_tag;
    return 0;
}


int ste_sim_fd(const ste_sim_t *sim)
{
    (void)sim;
    return 111;
}

int  ste_cat_call_control(ste_sim_t               *cat,
                          uintptr_t                client_tag,
                          ste_cat_call_control_t *cc_p)
{
    (void) cat;
    (void) client_tag;
    (void) cc_p;

    g_ste_cat_call_control__int = 1;

    return 0;
}


int ste_sim_disconnect(ste_sim_t *sim, uintptr_t client_tag)
{
    (void)sim;
    (void)client_tag;
    return 0;
}

uicc_request_status_t ste_uicc_sim_file_read_ecc(ste_sim_t *uicc, uintptr_t client_tag)
{
    (void)uicc;
    g_sim_client_tag_p = client_tag;
    g_ste_uicc_sim_file_read_ecc__int = 1;
    /* TODO: Make case thingy for testing negative testcases */
    return g_ste_uicc_sim_file_read_ecc__uicc_request_status_t;
}

uicc_request_status_t ste_uicc_get_service_availability(ste_sim_t *uicc,
        uintptr_t client_tag,
        sim_service_type_t service_type)
{
    (void)uicc;
    (void)service_type;
    g_sim_client_tag_p = client_tag;
    g_ste_uicc_get_service_availability__sim_service_type = service_type;
    return UICC_REQUEST_STATUS_OK;
}

uicc_request_status_t ste_uicc_get_service_table(ste_sim_t *uicc,
        uintptr_t client_tag,
        sim_service_type_t service_type)
{
    (void) uicc;
    (void) service_type;
    g_sim_client_tag_p = client_tag;
    g_ste_uicc_get_service_table__sim_service_type_t = service_type;
    return UICC_REQUEST_STATUS_OK;
}

uicc_request_status_t ste_uicc_sim_file_read_fdn(ste_sim_t *uicc,
        uintptr_t client_tag)
{
    (void) uicc;

    g_sim_client_tag_p = client_tag;
    g_ste_uicc_sim_file_read_fdn__int = 1;
    return UICC_REQUEST_STATUS_OK;
}

uicc_request_status_t ste_uicc_get_app_info(ste_sim_t *uicc,
        uintptr_t client_tag)
{
    (void) uicc;

    g_sim_client_tag_p = client_tag;
    g_ste_uicc_get_app_info__int = 1;
    return UICC_REQUEST_STATUS_OK;
}

uicc_request_status_t ste_uicc_sim_file_read_record(ste_sim_t *uicc,
        uintptr_t client_tag,
        int file_id,
        int record_id,
        int length,
        const char *file_path)
{
    (void) uicc;

    g_sim_client_tag_p = client_tag;
    g_ste_uicc_sim_file_read_record__file_id = file_id;
    g_ste_uicc_sim_file_read_record__record_id = record_id;
    g_ste_uicc_sim_file_read_record__length = length;

    if (file_path) {
        strncpy(g_ste_uicc_sim_file_read_record__file_path, file_path, sizeof(g_ste_uicc_sim_file_read_record__file_path) - 1);
        g_ste_uicc_sim_file_read_record__file_path[sizeof(g_ste_uicc_sim_file_read_record__file_path)-1] = '\0';
    } else {
        g_ste_uicc_sim_file_read_record__file_path[0] = '\0';
    }

    return UICC_REQUEST_STATUS_OK;
}


int                     ste_cat_register(ste_sim_t *cat,
        uintptr_t client_tag,
        uint32_t reg_events)
{
    (void) cat;
    (void) reg_events;

    g_sim_client_tag_p = client_tag;

    return STE_SIM_SUCCESS;
}


uicc_request_status_t         ste_uicc_sim_get_state(ste_sim_t *uicc,
        uintptr_t client_tag)
{
    (void)uicc;
    g_sim_client_tag_p = client_tag;

    return UICC_REQUEST_STATUS_OK;
}

uicc_request_status_t ste_uicc_sim_read_preferred_RAT_setting(ste_sim_t *uicc,
        uintptr_t client_tag)
{
    int rv = UICC_REQUEST_STATUS_OK;

    (void)uicc;
    g_sim_client_tag_p = client_tag;

    return rv;
}
