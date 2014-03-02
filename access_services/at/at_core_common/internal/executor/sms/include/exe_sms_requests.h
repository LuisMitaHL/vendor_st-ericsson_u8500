/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef EXE_SMS_REQUESTS_H
#define EXE_SMS_REQUESTS_H 1


#include "exe_request_record.h"
#include "exe_request_record_p.h"
#include "exe_sms.h"

exe_request_result_t request_sms_delete(exe_request_record_t *record_p);
exe_request_result_t request_sms_delete_all(exe_request_record_t *record_p);
exe_request_result_t request_message_event_subscribe(exe_request_record_t *record_p);
exe_request_result_t request_sms_memory_capacity(exe_request_record_t *record_p);
exe_request_result_t request_sms_memory_capacity_get(exe_request_record_t *record_p);
exe_request_result_t request_sms_relay_link_control_get(exe_request_record_t *record_p);
exe_request_result_t request_sms_relay_link_control_set(exe_request_record_t *record_p);
exe_request_result_t request_sms_send(exe_request_record_t *record_p);
exe_request_result_t request_sms_send_ack(exe_request_record_t *record_p);
exe_request_result_t request_sms_storage_info(exe_request_record_t *record_p);
exe_request_result_t request_select_message_service_set(exe_request_record_t *record_p);
exe_request_result_t request_select_message_service_get(exe_request_record_t *record_p);
exe_request_result_t request_sms_write(exe_request_record_t *record_p);
exe_request_result_t request_cbs_set_subscription(exe_request_record_t *record_p);
exe_request_result_t request_cbs_get_subscription(exe_request_record_t *record_p);
exe_request_result_t request_sms_read(exe_request_record_t *record_p);
exe_request_result_t request_cgsms_set(exe_request_record_t *record_p);
exe_request_result_t request_cgsms_read(exe_request_record_t *record_p);
exe_request_result_t request_sms_list(exe_request_record_t *record_p);
exe_request_result_t request_send_sms_from_preferred_storage(exe_request_record_t *record_p);

#endif /* EXE_SMS_REQUESTS_H */
