/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

//
// Temporarily use the Android tone generator until the
// same functionality is available on STELP level. The tone generator
// runs in a separate thread. It can not run in the ADM thread,
// since the tone generator calls will end up calling ADM again.
//

#include "ste_adm_msg.h"
#include "ste_adm_srv.h"
#include "ste_adm_dbg.h"
#include "ste_adm_db.h"
#include <string.h>

/*
* See ste_adm_client_get_toplevel_map for documentation.
*/
void util_api_get_toplevel_map(msg_get_toplevel_map_t* msg_p, srv_session_t** client_pp)
{
    (void) client_pp;
    const char *actual_dev = NULL;

    ALOG_INFO("util_api_get_toplevel_map: toplevel = %s\n", msg_p->toplevel);

    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    srv_become_worker_thread();
    ste_adm_res_t res = adm_db_toplevel_mapping_get(msg_p->toplevel, &actual_dev);
    strncpy(msg_p->actual, actual_dev, STE_ADM_MAX_DEVICE_NAME_LENGTH+1);

    srv_become_normal_thread();

    if (res != STE_ADM_RES_OK)
    {
        ALOG_ERR("util_api_get_toplevel_map: Failed with error %d\n", res);
        msg_p->base.result = res;
        srv_send_reply((msg_base_t*)msg_p);
    }

    srv_send_reply((msg_base_t*)msg_p);
}

/*
* See ste_adm_client_set_external_delay for documentation.
*/
void util_api_set_external_delay(msg_set_external_delay_t* msg_p, srv_session_t** client_pp)
{
    (void) client_pp;

    ALOG_INFO("util_api_set_external_delay: device = %s, delay = %d\n", msg_p->device, msg_p->external_delay);

    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    srv_become_worker_thread();

    ste_adm_res_t res = adm_db_external_delay_set(msg_p->device, msg_p->external_delay);

    srv_become_normal_thread();

    if (res != STE_ADM_RES_OK)
    {
        ALOG_ERR("util_api_set_external_delay: Failed with error %d\n", res);
        msg_p->base.result = res;
        srv_send_reply((msg_base_t*)msg_p);
    }

    srv_send_reply((msg_base_t*)msg_p);
}

/*
* See ste_adm_client_max_out_latency for documentation.
*/
void util_api_get_max_out_latency(msg_get_max_out_latency_t* msg_p, srv_session_t** client_pp)
{
    (void) client_pp;

    ALOG_INFO("util_api_get_max_out_latency: device = %s\n", msg_p->device);

    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    srv_become_worker_thread();

    ste_adm_res_t res = adm_db_external_delay_get(msg_p->device, &msg_p->latency);
    ALOG_INFO("util_api_get_max_out_latency: delay = %d\n", msg_p->latency);
    srv_become_normal_thread();

    if (res != STE_ADM_RES_OK)
    {
        ALOG_ERR("util_api_get_max_out_latency: Failed with error %d\n", res);
        msg_p->base.result = res;
        srv_send_reply((msg_base_t*)msg_p);
    }

    srv_send_reply((msg_base_t*)msg_p);
}

/*
* See ste_adm_client_get_toplevel_device for documentation.
*/
void util_api_get_toplevel_device(msg_get_toplevel_device_t* msg_p, srv_session_t** client_pp)
{
    (void) client_pp;
    const char *toplevel_dev = NULL;

    ALOG_INFO("util_api_get_toplevel_device: actual = %s\n", msg_p->actual);

    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    srv_become_worker_thread();
    ste_adm_res_t res = adm_db_toplevel_device_get(msg_p->actual, &toplevel_dev);
    srv_become_normal_thread();

    if (res == STE_ADM_RES_OK) {
        strncpy(msg_p->toplevel, toplevel_dev, STE_ADM_MAX_DEVICE_NAME_LENGTH+1);
    } else {
        ALOG_ERR("util_api_get_toplevel_device: Failed with error %d\n", res);
        msg_p->base.result = res;
        srv_send_reply((msg_base_t*)msg_p);
    }

    srv_send_reply((msg_base_t*)msg_p);
}


/*
* See ste_adm_client_set_external_delay for documentation.
*/
void util_api_execute_sql(msg_execute_sql_t* msg_p, srv_session_t** client_pp)
{
    (void) client_pp;

    ALOG_INFO("util_api_execute_sql:\n");

    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    srv_become_worker_thread();

    ste_adm_res_t res = adm_db_execute_sql(msg_p->sql);

    srv_become_normal_thread();

    if (res != STE_ADM_RES_OK)
    {
        ALOG_ERR("util_api_execute_sql: Failed with error %d\n", res);
        msg_p->base.result = res;
        srv_send_reply((msg_base_t*)msg_p);
    }

    srv_send_reply((msg_base_t*)msg_p);
}











