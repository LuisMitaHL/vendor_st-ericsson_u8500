/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef EXE_SMS_CLIENT_H
#define EXE_SMS_CLIENT_H 1

#include "atc_log.h"
#include "atc_selector.h"
#include "exe_request_record.h"
#include "exe_sms_utils.h"
#include "t_sms.h"
#include "t_sms_cb_session.h"

typedef struct smsclient_s smsclient_t;

void *smsclient_open_session(void);
exe_request_result_t smsclient_sms_event_subscribe(smsclient_t *smsclient_p, exe_request_record_t *record_p);
exe_request_result_t smsclient_sms_event_unsubscribe(smsclient_t *smsclient_p, exe_request_record_t *record_p);
void smsclient_close_session(void *service_p);
SMS_RequestControlBlock_t *smsclient_alloc_requestctrlblock(smsclient_t *smsclient_p, void *data_p);
void smsclient_free_requestctrlblock(SMS_RequestControlBlock_t **requestCtrlBlock_pp);
void smsclient_set_sms_subscription(smsclient_t *sms_client_p, bool sms_subscribed);
bool smsclient_get_sms_subscription(smsclient_t *sms_client_p);
void find_sms_init(exe_msg_search_info_t *const search_info_p, exe_msg_status_t sms_status, msg_storage_t storage);
bool find_sms_set(SMS_SearchInfo_t *const search_info_p, exe_cmgl_status_t status, msg_storage_t storage, exe_msg_position_t position, exe_msg_browse_option_t browse);
bool find_sms_slot_set(SMS_Slot_t *const slot_p,  msg_storage_t storage, exe_msg_position_t position);
#endif

