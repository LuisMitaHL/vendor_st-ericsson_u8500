/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef EXE_SMS_REQUESTS_CBS_H
#define EXE_SMS_REQUESTS_CBS_H 1


#include "exe_request_record.h"
#include "exe_request_record_p.h"

exe_request_result_t exe_cbs_subscribe(smsclient_t *smsclient_p, exe_request_record_t *record_p, exe_cscb_data_t *cscb_data_p);
exe_request_result_t exe_cbs_unsubscribe(smsclient_t *smsclient_p, exe_request_record_t *record_p, exe_cscb_data_t *cscb_data_p);
exe_request_result_t exe_cbs_get_subscriptions(smsclient_t *smsclient_p,
        exe_request_record_t *record_p, uint8_t number_of_subscriptions);
exe_request_result_t exe_cbs_activate_subscription(smsclient_t *smsclient_p, exe_request_record_t *record_p);
exe_request_result_t exe_cbs_deactivate_subcription(smsclient_t *smsclient_p, exe_request_record_t *record_p);
exe_request_result_t exe_cbs_get_number_of_subcriptions(smsclient_t *smsclient_p, exe_request_record_t *record_p);


#endif /* EXE_SMS_REQUESTS_CBS_H */
