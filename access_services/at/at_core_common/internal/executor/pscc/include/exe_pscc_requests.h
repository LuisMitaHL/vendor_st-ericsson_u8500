/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef EXE_PSCC_REQUESTS_H
#define EXE_PSCC_REQUESTS_H 1

#include "exe_pscc.h"
#include "exe_request_record.h"
/*
 *
 * Request functions
 *
 */
exe_request_result_t request_set_PDP_context_state(exe_request_record_t *record_p);           /* AT+CGACT    */
exe_request_result_t request_get_activation_stats(exe_request_record_t *record_p);            /* AT+CGACT?   */
exe_request_result_t request_set_PDP_context(exe_request_record_t *record_p);                 /* AT+CGDCONT  */
exe_request_result_t request_get_PDP_context(exe_request_record_t *record_p);                 /* AT+CGDCONT? */
exe_request_result_t request_activate_PDP_context(exe_request_record_t *record_p);            /* AT*EPPSD    */
exe_request_result_t request_set_PDP_auth(exe_request_record_t *record_p);                    /* AT*EIAAUW   */
exe_request_result_t request_nap_connect(exe_request_record_t *record_p);                     /* AT*ENAP     */
exe_request_result_t request_nap_disconnect(exe_request_record_t *record_p);                  /* AT*ENAP     */
exe_request_result_t request_nap_read(exe_request_record_t *record_p);                        /* AT*ENAP     */
exe_request_result_t request_set_PDP_minimum_qos(exe_request_record_t *record);               /* AT+CGEQMIN  */
exe_request_result_t request_get_PDP_minimum_qos(exe_request_record_t *record);               /* AT+CGEQMIN? */
exe_request_result_t request_set_PDP_required_qos(exe_request_record_t *record);              /* AT+CGEQREQ  */
exe_request_result_t request_get_PDP_required_qos(exe_request_record_t *record);              /* AT+CGEQREQ? */
exe_request_result_t request_set_PDP_minimum_qos_2G(exe_request_record_t *record_p);          /* AT+CGQMIN   */
exe_request_result_t request_get_PDP_minimum_qos_2G(exe_request_record_t *record_p);          /* AT+CGQMIN?  */
exe_request_result_t request_set_PDP_required_qos_2G(exe_request_record_t *record_p);         /* AT+CGQREQ   */
exe_request_result_t request_get_PDP_required_qos_2G(exe_request_record_t *record_p);         /* AT+CGQREQ?  */
exe_request_result_t request_set_PDP_negotiated_qos(exe_request_record_t *record);            /* AT+CGEQNEG  */
exe_request_result_t request_get_PDP_address(exe_request_record_t *record);                   /* AT+CGPADDR  */
exe_request_result_t request_set_GPRS_state(exe_request_record_t *record_p);                  /* AT+CGATT    */
exe_request_result_t reguest_get_GPRS_state(exe_request_record_t *record_p);                  /* AT+CGATT    */
exe_request_result_t request_set_PDP_modify(exe_request_record_t *record_p);                  /* AT+CGCMOD   */
exe_request_result_t request_set_PDP_send_data(exe_request_record_t *record_p);               /* AT*EUPLINK  */
exe_request_result_t request_psdun(exe_request_record_t *record_p);                           /* ATD*99#     */

/* Internal request handling of disconnect event */
exe_request_result_t internal_request_disconnected_failure_cause(exe_request_record_t *record_p);

exe_request_result_t pscc_request_send_cgev_event(int cid, char *pdp_addr_p, int pdp_type, exe_cgev_response_type_t response_type);
bool psccclient_increment_and_get_current_data_list_index(exe_request_record_t *record_p, int *current_index_p);

/*
 * Customer Extensions
 */
exe_request_result_t pscc_request_send_pcd_event(int cid, int pcd_status);

#endif
