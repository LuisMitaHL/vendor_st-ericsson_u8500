/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "testcases.h"
#include "module_test_main.h"
#include "cn_client.h"
#include "cn_log.h"
#include "cn_ss_command_handling.h"
#include "cn_pdc_internal.h"

tc_result_t pdc_msg_translation_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_request_call_forward_info_t cn_request_call_forward_info;
    cn_pdc_input_t *pdc_input_p = NULL;

    cn_request_call_forward_info.call_forward_info.status = 1; /* activation */
    cn_request_call_forward_info.call_forward_info.reason = CN_CALL_FORWARDING_REASON_UNCONDITIONAL;
    cn_request_call_forward_info.call_forward_info.service_class = CN_SERVICE_CLASS_GSM_VOICE_FACSIMILE_SMS;
    cn_request_call_forward_info.call_forward_info.toa = CN_SS_TON_UNKNOWN;
    cn_request_call_forward_info.call_forward_info.number[0] = 0; /* NULL */
    cn_request_call_forward_info.call_forward_info.time_seconds = 0;

    pdc_input_p = cn_pdc_translate_message_to_pdc_input(CN_REQUEST_SET_CALL_FORWARD, &cn_request_call_forward_info);

    TC_ASSERT(NULL != pdc_input_p);
    TC_ASSERT(CN_SERVICE_TYPE_SS == pdc_input_p->service_type);
    TC_ASSERT(0 == memcmp(pdc_input_p->service.ss.mmi_string, "*21**10#", strlen("*21**10#") + 1));

exit:
    free(pdc_input_p);
    return tc_result;
}

tc_result_t pdc_msg_translation_positive_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_request_call_forward_info_t cn_request_call_forward_info;
    cn_pdc_input_t *pdc_input_p = NULL;

    cn_request_call_forward_info.call_forward_info.status = 1; /* activation */
    cn_request_call_forward_info.call_forward_info.reason = CN_CALL_FORWARDING_REASON_NO_REPLY;
    cn_request_call_forward_info.call_forward_info.service_class = CN_SERVICE_CLASS_GSM_VOICE_FACSIMILE_SMS;
    cn_request_call_forward_info.call_forward_info.toa = CN_SS_TON_UNKNOWN;
    cn_request_call_forward_info.call_forward_info.number[0] = 0; /* NULL */
    cn_request_call_forward_info.call_forward_info.time_seconds = 0;

    pdc_input_p = cn_pdc_translate_message_to_pdc_input(CN_REQUEST_SET_CALL_FORWARD, &cn_request_call_forward_info);

    TC_ASSERT(NULL != pdc_input_p);
    TC_ASSERT(CN_SERVICE_TYPE_SS == pdc_input_p->service_type);
    TC_ASSERT(0 == memcmp(pdc_input_p->service.ss.mmi_string, "*61**10#", strlen("*61**10#") + 1));

exit:
    free(pdc_input_p);
    return tc_result;
}

/* Check that the service class filter for *21# works (no zero in SIB) */
tc_result_t pdc_msg_translation_positive_3()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_request_call_forward_info_t cn_request_call_forward_info;
    cn_pdc_input_t *pdc_input_p = NULL;

    cn_request_call_forward_info.call_forward_info.status = 1; /* activation */
    cn_request_call_forward_info.call_forward_info.reason = CN_CALL_FORWARDING_REASON_UNCONDITIONAL;
    cn_request_call_forward_info.call_forward_info.service_class = 0;
    cn_request_call_forward_info.call_forward_info.toa = CN_SS_TON_UNKNOWN;
    cn_request_call_forward_info.call_forward_info.number[0] = 0; /* NULL */
    cn_request_call_forward_info.call_forward_info.time_seconds = 0;

    pdc_input_p = cn_pdc_translate_message_to_pdc_input(CN_REQUEST_SET_CALL_FORWARD, &cn_request_call_forward_info);

    TC_ASSERT(NULL != pdc_input_p);
    TC_ASSERT(CN_SERVICE_TYPE_SS == pdc_input_p->service_type);
    TC_ASSERT(0 == memcmp(pdc_input_p->service.ss.mmi_string, "*21#", strlen("*21#") + 1));

exit:
    free(pdc_input_p);
    return tc_result;
}

/* Check that the retry timeout setting is set */
tc_result_t pdc_msg_translation_positive_4()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_request_call_forward_info_t cn_request_call_forward_info;
    cn_pdc_input_t *pdc_input_p = NULL;

    cn_request_call_forward_info.call_forward_info.status = 1; /* activation */
    cn_request_call_forward_info.call_forward_info.reason = CN_CALL_FORWARDING_REASON_NO_REPLY;
    cn_request_call_forward_info.call_forward_info.service_class = CN_SERVICE_CLASS_GSM_VOICE_FACSIMILE_SMS;
    cn_request_call_forward_info.call_forward_info.toa = CN_SS_TON_UNKNOWN;
    cn_request_call_forward_info.call_forward_info.number[0] = 0; /* NULL */
    cn_request_call_forward_info.call_forward_info.time_seconds = 15;

    pdc_input_p = cn_pdc_translate_message_to_pdc_input(CN_REQUEST_SET_CALL_FORWARD, &cn_request_call_forward_info);

    TC_ASSERT(NULL != pdc_input_p);
    TC_ASSERT(CN_SERVICE_TYPE_SS == pdc_input_p->service_type);
    TC_ASSERT(0 == memcmp(pdc_input_p->service.ss.mmi_string, "*61**10*15#", strlen("*61**10*15#") + 1));

exit:
    free(pdc_input_p);
    return tc_result;
}

/* case CN_REQUEST_QUERY_CALL_BARRING */
tc_result_t pdc_msg_translation_positive_5()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_request_query_call_barring_t cn_request_query_call_barring;
    cn_pdc_input_t *pdc_input_p = NULL;

    sprintf(cn_request_query_call_barring.facility, "%s", "OX");
    cn_request_query_call_barring.service_class = CN_SERVICE_CLASS_GSM_VOICE_FACSIMILE_SMS;

    pdc_input_p = cn_pdc_translate_message_to_pdc_input(CN_REQUEST_QUERY_CALL_BARRING, &cn_request_query_call_barring);

    TC_ASSERT(NULL != pdc_input_p);
    TC_ASSERT(CN_SERVICE_TYPE_SS == pdc_input_p->service_type);
    TC_ASSERT(0 == memcmp(pdc_input_p->service.ss.mmi_string, "*#332**10#", strlen("*#332**10#") + 1));

exit:
    free(pdc_input_p);
    return tc_result;
}

tc_result_t pdc_msg_translation_positive_6()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_request_call_forward_info_t cn_request_call_forward_info;
    cn_pdc_input_t *pdc_input_p = NULL;

    cn_request_call_forward_info.call_forward_info.status = 1; /* activation */
    cn_request_call_forward_info.call_forward_info.reason = CN_CALL_FORWARDING_REASON_ALL_CALL_FORWARDING;
    cn_request_call_forward_info.call_forward_info.service_class = 80;
    cn_request_call_forward_info.call_forward_info.toa = CN_SS_TON_UNKNOWN;
    cn_request_call_forward_info.call_forward_info.number[0] = 0; /* NULL */
    cn_request_call_forward_info.call_forward_info.time_seconds = 0;

    pdc_input_p = cn_pdc_translate_message_to_pdc_input(CN_REQUEST_SET_CALL_FORWARD, &cn_request_call_forward_info);

    TC_ASSERT(NULL != pdc_input_p);
    TC_ASSERT(CN_SERVICE_TYPE_SS == pdc_input_p->service_type);
    TC_ASSERT(0 == memcmp(pdc_input_p->service.ss.mmi_string, "*002**22#", strlen("*002**22#") + 1));

exit:
    free(pdc_input_p);
    return tc_result;
}

tc_result_t pdc_msg_translation_positive_7()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_request_call_forward_info_t cn_request_call_forward_info;
    cn_pdc_input_t *pdc_input_p = NULL;

    cn_request_call_forward_info.call_forward_info.status = 1; /* activation */
    cn_request_call_forward_info.call_forward_info.reason = CN_CALL_FORWARDING_REASON_UNCONDITIONAL;
    cn_request_call_forward_info.call_forward_info.service_class = CN_SERVICE_CLASS_GSM_VOICE_FACSIMILE_SMS;
    cn_request_call_forward_info.call_forward_info.toa = CN_SS_TON_UNKNOWN;
    cn_request_call_forward_info.call_forward_info.number[0] = 0; /* NULL */
    cn_request_call_forward_info.call_forward_info.time_seconds = 0;

    pdc_input_p = cn_pdc_translate_message_to_pdc_input(CN_REQUEST_SET_CALL_FORWARD, &cn_request_call_forward_info);

    TC_ASSERT(NULL != pdc_input_p);
    TC_ASSERT(CN_SERVICE_TYPE_USSD == pdc_input_p->service_type);
    TC_ASSERT(0 == memcmp(pdc_input_p->service.ss.mmi_string, "*21**10#", strlen("*21**10#") + 1));

exit:
    free(pdc_input_p);
    return tc_result;

}
