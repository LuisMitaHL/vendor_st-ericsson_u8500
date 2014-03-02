/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdlib.h>
#include <string.h>

#include "exe_sim_client.h"
#include "whitebox.h"
#include "sim.h"
#include "wb_test.h"

#define CAUSE_DUMMY 0 /* For responses only client tag is important */

#define IMSI_TEST "240991234567891"
#define IMSI_TEST_RAW "ABABABAB4"

#define STKE_RESPONSE_STRING "0000"
#define STKE_RESPONSE_STRING_LENGTH 4

#define STKI_STRING "1111"
#define STKN_STRING "2222"

#define EXE_SIM_NORMAL_COMPLETION 0

extern int state;

/* Mirror the internal sim client states */
typedef enum {
    EXE_WB_TEST_SIM_STATE_UNKNOWN = 0,             /* Unknown state of the SIM card. */
    EXE_WB_TEST_SIM_STATE_VERIFY_NEEDED,           /* SIM card requests verification. */
    EXE_WB_TEST_SIM_STATE_READY,                   /* SIM card is ready */
} exe_wb_test_sim_state_t;


/* Mirror the internal sim client */
struct wb_test_sim_client_s {
    int fd_sim;
    ste_sim_t *ste_sim_p;
    ste_sim_closure_t ste_sim_closure;
    exe_wb_test_sim_state_t exe_sim_state;
};

typedef struct wb_test_sim_client_s wb_test_sim_client_t;

ste_sim_call_number_t *sms_service_center_p;

int ste_sim_startup(ste_sim_t *cat, uintptr_t client_tag)
{
    return 0;
}

int ste_sim_read(ste_sim_t *sim_p)
{
    static int cpol_alternate = 1; /* Used to make the cpol test switch behavior the second time the function is entered. */

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_esimsr_0: {
        ste_uicc_get_sim_state_response_t response;
        response.error_cause = EXE_SIM_NORMAL_COMPLETION;
        response.state = SIM_STATE_SIM_ABSENT;

        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response,
                            NULL);
        break;
    }
    case wb_testcase_esimsr_1: {
        ste_uicc_get_sim_state_response_t response;
        response.error_cause = EXE_SIM_NORMAL_COMPLETION;
        response.state = SIM_STATE_READY;

        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response,
                            NULL);
        break;
    }
    case wb_testcase_esimsr_2: {
        ste_uicc_sim_state_changed_t event;
        event.state = SIM_STATE_SIM_ABSENT;
        wb_test_sim_client_t sim_client;
        sim_client.exe_sim_state = EXE_WB_TEST_SIM_STATE_UNKNOWN;

        sim_client_callback(STE_UICC_CAUSE_SIM_STATE_CHANGED,
                            0 /*EXECUTOR.client_tag_p*/, &event,
                            &sim_client);
        break;
    }
    case wb_testcase_esimsr_3: {
        ste_uicc_sim_state_changed_t event;
        event.state = SIM_STATE_PERMANENTLY_BLOCKED;
        wb_test_sim_client_t sim_client;
        sim_client.exe_sim_state = EXE_WB_TEST_SIM_STATE_UNKNOWN;

        sim_client_callback(STE_UICC_CAUSE_SIM_STATE_CHANGED,
                            0 /*EXECUTOR.client_tag_p*/, &event,
                            &sim_client);
        break;
    }
    case wb_testcase_epee_2: {
        ste_uicc_sim_state_changed_t event;
        event.state = SIM_STATE_PIN_NEEDED;
        wb_test_sim_client_t sim_client;
        sim_client.exe_sim_state = EXE_WB_TEST_SIM_STATE_UNKNOWN;

        sim_client_callback(STE_UICC_CAUSE_SIM_STATE_CHANGED,
                            0 /*EXECUTOR.client_tag_p*/, &event,
                            &sim_client);

        event.state = SIM_STATE_READY;
        sim_client.exe_sim_state = EXE_WB_TEST_SIM_STATE_VERIFY_NEEDED;

        sim_client_callback(STE_UICC_CAUSE_SIM_STATE_CHANGED,
                            0 /*EXECUTOR.client_tag_p*/, &event,
                            &sim_client);
        break;
    }
    case wb_testcase_crsm_read_get_resp:
    case wb_testcase_crsm_read_get_resp_2:
    case wb_testcase_crsm_read_get_resp_3:
    case wb_testcase_crsm_read_get_resp_4:
    case wb_testcase_crsm_read_get_resp_5:
    case wb_testcase_crsm_read_get_resp_6: {
        ste_uicc_get_file_information_response_t response;
        ste_uicc_status_word_t status_word = {0, 0};
        response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response.status_word = status_word;
        /* This is the format that sw1 and sw2 is sent on according to 3GPP TS 31.101 */
        response.fcp = (uint8_t *)"AA 03";
        response.length = 5;
        sim_client_callback(CAUSE_DUMMY,
                            EXECUTOR.client_tag_p,
                            &response,
                            NULL);
        break;
    }
    case wb_testcase_crsm_read_file_bin:
    case wb_testcase_crsm_read_file_bin_2:
    case wb_testcase_crsm_read_file_bin_3: {
        ste_uicc_sim_file_read_binary_response_t response;
        ste_uicc_status_word_t status_word = {0, 0};
        response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response.status_word = status_word;
        /* This is the format that sw1 and sw2 is sent on according to 3GPP TS 31.101 */
        response.data = (uint8_t *) "AA 03";
        response.length = 5;
        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response,
                            NULL);
        break;
    }
    case wb_testcase_crsm_read_file_rec:
    case wb_testcase_crsm_read_file_rec_2:
    case wb_testcase_crsm_read_file_rec_3: {
        ste_uicc_sim_file_read_record_response_t response;
        ste_uicc_status_word_t status_word = {0, 0};
        response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response.status_word = status_word;
        /* This is the format that sw1 and sw2 is sent on according to 3GPP TS 31.101 */
        response.data = (uint8_t *)"AA 03";
        response.length = 5;
        sim_client_callback(CAUSE_DUMMY,
                            EXECUTOR.client_tag_p,
                            &response,
                            NULL);
        break;
    }
    case wb_testcase_crsm_update_file_rec:
    case wb_testcase_crsm_update_file_rec_1:
    case wb_testcase_crsm_update_file_rec_1_2:
    case wb_testcase_crsm_update_file_rec_2:
    case wb_testcase_crsm_update_file_rec_3:
    case wb_testcase_crsm_update_file_rec_4: {
        ste_uicc_update_sim_file_record_response_t response;
        ste_uicc_status_word_t status_word = {0, 0};
        response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response.status_word = status_word;

        sim_client_callback(CAUSE_DUMMY,
                            EXECUTOR.client_tag_p,
                            &response,
                            NULL);
        break;
    }
    case wb_testcase_crsm_update_file_bin:
    case wb_testcase_crsm_update_file_bin_1:
    case wb_testcase_crsm_update_file_bin_1_2:
    case wb_testcase_crsm_update_file_bin_2:
    case wb_testcase_crsm_update_file_bin_3:
    case wb_testcase_crsm_update_file_bin_4: {
        ste_uicc_update_sim_file_binary_response_t response;
        ste_uicc_status_word_t status_word = {0, 0};
        response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response.status_word = status_word;

        sim_client_callback(CAUSE_DUMMY,
                            EXECUTOR.client_tag_p,
                            &response,
                            NULL);
        break;
    }
    case wb_testcase_crsm_read_status:
    case wb_testcase_crsm_read_status_2:
    case wb_testcase_crsm_read_status_3: {
        ste_uicc_get_file_information_response_t response;
        ste_uicc_status_word_t status_word = {0, 0};
        uint8_t temp_fcp[2] = {10 , 10};

        response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response.status_word.sw1 = 0;
        response.status_word.sw2 = 0;
        response.fcp = (uint8_t *)&temp_fcp;
        response.length = 2;

        sim_client_callback(CAUSE_DUMMY,
                            EXECUTOR.client_tag_p,
                            &response,
                            NULL);
        break;
    }
    case wb_testcase_cuad_do: {

        ste_uicc_sim_card_status_response_t response;

        response.card_state = STE_UICC_CARD_STATE_READY;
        response.upin_state = STE_UICC_PIN_STATE_UNKNOWN;
        response.card_type = SIM_CARD_TYPE_UICC;
        response.status = EXE_SIM_NORMAL_COMPLETION;
        response.num_apps = 1;

        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);

        ste_uicc_sim_file_read_record_response_t response_2;
        ste_uicc_status_word_t status_word = {0x90, 0x00};
        response_2.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response_2.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response_2.status_word = status_word;

        response_2.length = 10;
        response_2.data = NULL;
        response_2.data = (uint8_t *)malloc(sizeof(uint8_t) * response_2.length);;
        *(response_2.data + 0) = 0x31;
        *(response_2.data + 1) = 0x32;
        *(response_2.data + 2) = 0x33;
        *(response_2.data + 3) = 0x34;
        *(response_2.data + 4) = 0x32;
        *(response_2.data + 5) = 0x34;
        *(response_2.data + 6) = 0x33;
        *(response_2.data + 7) = 0x34;
        *(response_2.data + 8) = 0x32;
        *(response_2.data + 9) = 0x34;

        sim_client_callback(CAUSE_DUMMY,
                            EXECUTOR.client_tag_p,
                            &response_2,
                            NULL);

        free(response_2.data);

        break;
    }
    case wb_testcase_cuad_do_2: {
        ste_uicc_sim_card_status_response_t response;

        response.card_state = STE_UICC_CARD_STATE_READY;
        response.upin_state = STE_UICC_PIN_STATE_UNKNOWN;
        response.card_type = SIM_CARD_TYPE_ICC;
        response.status = EXE_SIM_NORMAL_COMPLETION;
        response.num_apps = 0;

        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);

        break;
    }
    case wb_testcase_cuad_do_3: {

        ste_uicc_sim_card_status_response_t response;

        response.card_state = STE_UICC_CARD_STATE_READY;
        response.upin_state = STE_UICC_PIN_STATE_UNKNOWN;
        response.card_type = SIM_CARD_TYPE_UICC;
        response.status = EXE_SIM_NORMAL_COMPLETION;
        response.num_apps = 3;

        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);

        ste_uicc_sim_file_read_record_response_t response_2;
        ste_uicc_status_word_t status_word = {0x90, 0x00};
        response_2.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response_2.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response_2.status_word = status_word;

        response_2.length = 10;
        response_2.data = NULL;
        response_2.data = (uint8_t *)malloc(sizeof(uint8_t) * response_2.length);;
        *(response_2.data + 0) = 0x31;
        *(response_2.data + 1) = 0x32;
        *(response_2.data + 2) = 0x33;
        *(response_2.data + 3) = 0x34;
        *(response_2.data + 4) = 0x32;
        *(response_2.data + 5) = 0x34;
        *(response_2.data + 6) = 0x33;
        *(response_2.data + 7) = 0x34;
        *(response_2.data + 8) = 0x32;
        *(response_2.data + 9) = 0x34;

        sim_client_callback(CAUSE_DUMMY,
                            EXECUTOR.client_tag_p,
                            &response_2,
                            NULL);

        free(response_2.data);

        ste_uicc_sim_file_read_record_response_t response_3;
        response_3.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response_3.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response_3.status_word = status_word;

        response_3.length = 10;
        response_3.data = NULL;
        response_3.data = (uint8_t *)malloc(sizeof(uint8_t) * response_3.length);;
        *(response_3.data + 0) = 0x31;
        *(response_3.data + 1) = 0x32;
        *(response_3.data + 2) = 0x33;
        *(response_3.data + 3) = 0x34;
        *(response_3.data + 4) = 0x32;
        *(response_3.data + 5) = 0x34;
        *(response_3.data + 6) = 0x33;
        *(response_3.data + 7) = 0x34;
        *(response_3.data + 8) = 0x32;
        *(response_3.data + 9) = 0x34;

        sim_client_callback(CAUSE_DUMMY,
                            EXECUTOR.client_tag_p,
                            &response_3,
                            NULL);

        free(response_3.data);

        ste_uicc_sim_file_read_record_response_t response_4;
        response_4.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response_4.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response_4.status_word = status_word;

        response_4.length = 10;
        response_4.data = NULL;
        response_4.data = (uint8_t *)malloc(sizeof(uint8_t) * response_4.length);;
        *(response_4.data + 0) = 0x31;
        *(response_4.data + 1) = 0x32;
        *(response_4.data + 2) = 0x33;
        *(response_4.data + 3) = 0x34;
        *(response_4.data + 4) = 0x32;
        *(response_4.data + 5) = 0x34;
        *(response_4.data + 6) = 0x33;
        *(response_4.data + 7) = 0x34;
        *(response_4.data + 8) = 0x32;
        *(response_4.data + 9) = 0x34;

        sim_client_callback(CAUSE_DUMMY,
                            EXECUTOR.client_tag_p,
                            &response_4,
                            NULL);

        free(response_4.data);

        break;
    }
    case wb_testcase_cpin_read: {
        ste_uicc_get_sim_state_response_t response;
        response.error_cause = EXE_SIM_NORMAL_COMPLETION;
        response.state = SIM_STATE_READY;

        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response,
                            NULL);
        break;
    }
    case wb_testcase_cpin_set:
    case wb_testcase_cpin_set_wrong_pin: {
        ste_uicc_get_sim_state_response_t get_response;
        ste_uicc_pin_change_response_t verify_response;

        if (SIM_WAITING_FOR_GET_STATE == EXECUTOR.sim_state) {
            get_response.error_cause = EXE_SIM_NORMAL_COMPLETION;
            get_response.state = SIM_STATE_PIN_NEEDED;

            sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p,
                                &get_response, NULL);
        } else {
            if (SIM_WAITING_FOR_VERIFY_OK == EXECUTOR.sim_state) {
                ste_uicc_status_word_t status_word = {0, 0};
                verify_response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
                verify_response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
                verify_response.status_word = status_word;
            } else if (SIM_WAITING_FOR_VERIFY_FAIL == EXECUTOR.sim_state) {
                ste_uicc_status_word_t status_word = {0x63, 0xC2};
                verify_response.uicc_status_code = STE_UICC_STATUS_CODE_FAIL;
                verify_response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
                verify_response.status_word = status_word;
            } else {
                /* Should never happen */
                return -1;
            }

            sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p,
                                &verify_response, NULL);
        }

        break;
    }
    case wb_testcase_ecexpin_query_state: {
        ste_uicc_get_sim_state_response_t response;
        response.error_cause = EXE_SIM_NORMAL_COMPLETION;
        response.state = SIM_STATE_READY;

        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response,
                            NULL);
        break;
    }
    case wb_testcase_ecexpin_set_puk: {
        ste_uicc_pin_change_response_t verify_response;

        if (SIM_WAITING_FOR_VERIFY_OK == EXECUTOR.sim_state) {
            ste_uicc_status_word_t status_word = {0, 0};
            verify_response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
            verify_response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
            verify_response.status_word = status_word;
        } else if (SIM_WAITING_FOR_VERIFY_FAIL == EXECUTOR.sim_state) {
            return -1;
        } else {
            /* Should never happen */
            return -1;
        }

        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p,
                            &verify_response, NULL);

        break;
    }
    case wb_testcase_cpwd_2: {
        ste_uicc_pin_change_response_t response;
        ste_uicc_status_word_t status_word = {0, 0};
        response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response.status_word = status_word;
        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response,
                            NULL);
        break;
    }
    case wb_testcase_csca_set_smsc_1:
    case wb_testcase_csca_set_smsc_2:
    case wb_testcase_csca_set_smsc_3:
    case wb_testcase_csca_set_smsc_4:
    case wb_testcase_csca_set_smsc_5: {
        ste_uicc_sim_smsc_set_active_response_t response;
        ste_uicc_status_word_t status_word = {0, 0};
        response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response.status_word = status_word;

        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
        break;
    }

    case wb_testcase_csca_get_smsc_1:
    case wb_testcase_csca_get_smsc_2:
    case wb_testcase_csca_get_smsc_3:
    case wb_testcase_csca_get_smsc_4:
    case wb_testcase_csca_get_smsc_5: {
        ste_uicc_sim_smsc_get_active_response_t response;

        response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response.smsc.num_text.text_p = sms_service_center_p->num_text.text_p;
        response.smsc.num_text.no_of_characters = sms_service_center_p->num_text.no_of_characters;
        response.smsc.num_text.text_coding = sms_service_center_p->num_text.text_coding;
        response.smsc.npi = sms_service_center_p->npi;
        response.smsc.ton = sms_service_center_p->ton;

        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
        free(sms_service_center_p->num_text.text_p);
        free(sms_service_center_p);
        break;
    }

    case wb_testcase_cimi_do: {
        ste_uicc_sim_file_read_imsi_response_t response;
        ste_uicc_status_word_t status_word = {0, 0};

        response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response.status_word = status_word;
        memcpy(response.imsi, IMSI_TEST, SIM_EF_IMSI_STRING_LEN);
        memcpy(response.raw_imsi, IMSI_TEST_RAW, SIM_EF_IMSI_LEN);

        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
        break;
    }

    case wb_testcase_epinr_set_1:
    case wb_testcase_epinr_set_2: {
        ste_uicc_pin_info_response_t response;

        response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;

        response.pin_status = STE_UICC_PIN_STATUS_ENABLED;
        response.attempts_remaining = 3;

        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
        break;
    }

    case wb_testcase_stkc_set_0:
    case wb_testcase_stkc_set_1: {
        int response = 0;

        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);

        ste_cat_enable_response_t response_2;

        response_2.status = 0;

        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response_2, NULL);
    }
    break;

    case wb_testcase_stkc_set_exe_fail: {
        int response = -1;

        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
    }
    break;

    case wb_testcase_stkc_read_zero:
    case wb_testcase_stkc_read_one: {
        ste_cat_get_terminal_profile_response_t response;
        char terminal_profile[4] = { 0xA, 0xB, 0xB, 0xA };

        response.status = 0;
        response.profile_p = terminal_profile;
        response.profile_len = 4;

        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);

        ste_cat_enable_response_t response_2;

        response_2.status = 0;

        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response_2, NULL);
    }
    break;

    case wb_testcase_stke_send: {
        ste_cat_ec_response_t response;
        ste_cat_apdu_data_t apdu;

        apdu.buf = malloc(STKE_RESPONSE_STRING_LENGTH + 1);
        memcpy(apdu.buf, STKE_RESPONSE_STRING, STKE_RESPONSE_STRING_LENGTH);
        apdu.buf[STKE_RESPONSE_STRING_LENGTH] = '\0';
        apdu.len = STKE_RESPONSE_STRING_LENGTH;
        response.ec_status = STE_CAT_RESPONSE_OK;
        response.apdu = apdu;

        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
        free(apdu.buf);
        break;
    }

    case wb_testcase_stki: {
        ste_cat_pc_ind_t response;
        response.simd_tag = 0;
        response.apdu.buf = NULL;
        response.apdu.buf = malloc(STKE_RESPONSE_STRING_LENGTH + 1);
        memset(response.apdu.buf, '\0', STKE_RESPONSE_STRING_LENGTH);
        strcat(response.apdu.buf, STKI_STRING);
        response.apdu.len = STKE_RESPONSE_STRING_LENGTH;

        sim_client_callback(STE_CAT_CAUSE_PC, EXECUTOR.client_tag_p, &response, NULL);

        free(response.apdu.buf);
        break;
    }

    case wb_testcase_stkn: {

        ste_cat_pc_ind_t response;
        response.simd_tag = 0;
        response.apdu.buf = NULL;
        response.apdu.buf = malloc(STKE_RESPONSE_STRING_LENGTH + 1);
        memset(response.apdu.buf, '\0', STKE_RESPONSE_STRING_LENGTH);
        strcat(response.apdu.buf, STKN_STRING);
        response.apdu.len = STKE_RESPONSE_STRING_LENGTH;

        sim_client_callback(STE_CAT_CAUSE_PC_NOTIFICATION, EXECUTOR.client_tag_p, &response, NULL);

        free(response.apdu.buf);
        break;
    }

    case wb_testcase_stkend: {

        sim_client_callback(STE_CAT_CAUSE_SESSION_END_IND, EXECUTOR.client_tag_p, NULL, NULL);
        break;
    }

    case wb_testcase_stkr_set_executer_error: {
        ste_cat_tr_response_t response;
        ste_uicc_status_word_t status_word = {0, 0};
        response.status = 1;
        response.status_word = status_word;

        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
        break;
    }

    case wb_testcase_stkr_set_ok: {
        ste_cat_tr_response_t response;
        ste_uicc_status_word_t status_word = {144, 0};

        response.status = 0;
        response.status_word = status_word;
        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
        break;
    }

    case wb_testcase_clck_set_enable: {
        ste_uicc_pin_enable_response_t response;
        ste_uicc_get_service_availability_response_t serv_rsp;
        ste_uicc_status_word_t status_word = {0, 0};

        switch(state) {
        case 0:
            serv_rsp.uicc_status_code = STE_UICC_STATUS_CODE_OK;
            serv_rsp.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
            serv_rsp.service_availability = STE_UICC_SERVICE_AVAILABLE;
            sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &serv_rsp, NULL);
            break;
        case 1:
        case 2:
            response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
            response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
            response.status_word = status_word;
            sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
            break;
        default:
            response.uicc_status_code = STE_UICC_STATUS_CODE_FAIL;
            response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR;
            response.status_word = status_word;
            sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
            break;
        }

        break;
    }
    case wb_testcase_clck_set_enable_neg: {
        ste_uicc_pin_enable_response_t response;
        ste_uicc_get_service_availability_response_t serv_rsp;
        ste_uicc_status_word_t status_word = {0, 0};

        switch(state) {
        case 0:
            serv_rsp.uicc_status_code = STE_UICC_STATUS_CODE_OK;
            serv_rsp.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
            serv_rsp.service_availability = STE_UICC_SERVICE_NOT_AVAILABLE;
            sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &serv_rsp, NULL);
            break;
        default:
            response.uicc_status_code = STE_UICC_STATUS_CODE_FAIL;
            response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR;
            response.status_word = status_word;
            sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
            break;
        }
        break;
    }
    case wb_testcase_clck_query_neg: {
        ste_uicc_pin_enable_response_t response;
        ste_uicc_get_service_availability_response_t serv_rsp;
        ste_uicc_status_word_t status_word = {0, 0};

        switch(state) {
        case 0:
            serv_rsp.uicc_status_code = STE_UICC_STATUS_CODE_OK;
            serv_rsp.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
            serv_rsp.service_availability = STE_UICC_SERVICE_NOT_AVAILABLE;
            sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &serv_rsp, NULL);
            break;
        default:
            response.uicc_status_code = STE_UICC_STATUS_CODE_FAIL;
            response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR;
            response.status_word = status_word;
            sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
            break;
        }
        break;
    }
    case wb_testcase_clck_set_disable: {
        ste_uicc_pin_disable_response_t response;
        ste_uicc_status_word_t status_word = {0, 0};
        ste_uicc_get_service_availability_response_t serv_rsp;

        switch(state) {
        case 0:
            serv_rsp.uicc_status_code = STE_UICC_STATUS_CODE_OK;
            serv_rsp.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
            serv_rsp.service_availability = STE_UICC_SERVICE_AVAILABLE;
            sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &serv_rsp, NULL);
            break;
        case 1:
        case 2:
            response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
            response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
            response.status_word = status_word;
            sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
            break;
        default:
            response.uicc_status_code = STE_UICC_STATUS_CODE_FAIL;
            response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR;
            response.status_word = status_word;
            sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
            break;
        }
        break;
    }

    case wb_testcase_clck_set_dis_pin: {
        ste_uicc_pin_disable_response_t response;
        ste_uicc_status_word_t status_word = {99, 193};
        ste_uicc_get_service_availability_response_t serv_rsp;

        switch(state) {
        case 0:
            serv_rsp.uicc_status_code = STE_UICC_STATUS_CODE_OK;
            serv_rsp.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
            serv_rsp.service_availability = STE_UICC_SERVICE_AVAILABLE;
            sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &serv_rsp, NULL);
            break;
        case 1:
        case 2:
            response.uicc_status_code = STE_UICC_STATUS_CODE_FAIL;
            response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
            response.status_word = status_word;
            sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
            break;
        default:
            response.uicc_status_code = STE_UICC_STATUS_CODE_FAIL;
            response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR;
            sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
            break;
        }
        break;
    }

    case wb_testcase_clck_set_dis_puk: {
        ste_uicc_pin_disable_response_t response;
        ste_uicc_status_word_t status_word = {99, 192};
        ste_uicc_get_service_availability_response_t serv_rsp;

        switch(state) {
        case 0:
            serv_rsp.uicc_status_code = STE_UICC_STATUS_CODE_OK;
            serv_rsp.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
            serv_rsp.service_availability = STE_UICC_SERVICE_AVAILABLE;
            sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &serv_rsp, NULL);
            break;
        case 1:
        case 2:
            response.uicc_status_code = STE_UICC_STATUS_CODE_FAIL;
            response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
            response.status_word = status_word;
            sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
            break;
        default:
            response.uicc_status_code = STE_UICC_STATUS_CODE_FAIL;
            response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR;
            sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
            break;
        }
        break;
    }

    case wb_testcase_clck_set_query: {
        if (PARSER_p->facility_code == ATC_FAC_NetPer) {
            ste_uicc_sim_power_off_response_t response;

            response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
            response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
            sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
        } else {
            ste_uicc_pin_info_response_t response;

            response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
            response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
            response.pin_status = 0;
            response.attempts_remaining = 3;
            sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
        }
        break;
    }
    case wb_testcase_cnum_do: {
        ste_sim_subscriber_number_response_t response;
        ste_sim_subscriber_number_record_t *rec_data_p = NULL;
        uint8_t i = 0;

        response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response.status_word.sw1 = 0;
        response.status_word.sw2 = 0;
        response.alpha_id_len = 1;
        response.number_of_records = 2;
        response.record_length = 33;
        response.record_data_p = NULL;
        response.record_data_p = (ste_sim_subscriber_number_record_t *)calloc(sizeof(ste_sim_subscriber_number_record_t) * response.number_of_records, 1);

        rec_data_p = response.record_data_p;

        rec_data_p->alpha_id_actual_len = 0;
        rec_data_p->alpha_coding = STE_SIM_UCS2;
        rec_data_p->alpha_p = NULL;
        rec_data_p->alpha_p = (uint8_t *)calloc(sizeof(uint8_t) * (rec_data_p->alpha_id_actual_len + 1), 1);
        *(rec_data_p->alpha_p) = '\0';

        rec_data_p->number_actual_len = 4;
        rec_data_p->number_p = NULL;
        rec_data_p->number_p = (uint8_t *)calloc(sizeof(uint8_t) * rec_data_p->number_actual_len, 1);
        *(rec_data_p->number_p + 0) = 0x31;
        *(rec_data_p->number_p + 1) = 0x31;
        *(rec_data_p->number_p + 2) = 0x32;
        *(rec_data_p->number_p + 3) = 0x0;

        rec_data_p->type = 0x0;
        rec_data_p->speed = 0xFF;
        rec_data_p->service = 0xFF;
        rec_data_p->itc = 0xFF;

        rec_data_p++;

        rec_data_p->alpha_id_actual_len = 0;
        rec_data_p->alpha_p = NULL;
        rec_data_p->alpha_coding = STE_SIM_GSM_Default;
        rec_data_p->alpha_p = (uint8_t *)calloc(sizeof(uint8_t) * (rec_data_p->alpha_id_actual_len + 1), 1);
        *(rec_data_p->alpha_p + 0) = '\0';

        rec_data_p->number_actual_len = 4;
        rec_data_p->number_p = NULL;
        rec_data_p->number_p = (uint8_t *)calloc(sizeof(uint8_t) * rec_data_p->number_actual_len, 1);
        *(rec_data_p->number_p + 0) = 0x39;
        *(rec_data_p->number_p + 1) = 0x31;
        *(rec_data_p->number_p + 2) = 0x31;
        *(rec_data_p->number_p + 3) = 0x0;

        rec_data_p->type = 0x0;
        rec_data_p->speed = 0xFF;
        rec_data_p->service = 0xFF;
        rec_data_p->itc = 0xFF;


        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);

        rec_data_p = response.record_data_p;

        for (i = 0; i < 2; i++) {
            if (NULL != rec_data_p->alpha_p) {
                free(rec_data_p->alpha_p);
            }

            if (NULL != rec_data_p->number_p) {
                free(rec_data_p->number_p);
            }

            rec_data_p++;
        }

        if (NULL != response.record_data_p) {
            free(response.record_data_p);
        }

        break;
    }
    case wb_testcase_plmn_write:
    case wb_testcase_plmn_write_illegal:
    case wb_testcase_plmn_remove_element: {
        ste_uicc_update_sim_file_plmn_response_t response;
        response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response.status_word.sw1 = 0x0;
        response.status_word.sw2 = 0x0;
        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
        break;
    }

    case wb_testcase_plmn_write_rejected: {
        ste_uicc_update_sim_file_plmn_response_t response;
        response.uicc_status_code = STE_UICC_STATUS_CODE_FAIL;
        response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response.status_word.sw1 = 0x98;
        response.status_word.sw2 = 0x04;
        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
        break;
    }

    case wb_testcase_plmn_write_retry: {
        if (cpol_alternate % 2) {
            /* First entry, send FILE_NOT_FOUND */
            ste_uicc_sim_file_read_plmn_response_t response;
            ATC_LOG_E("wb_testcase_plmn_write_retry, sending file not found");
            response.uicc_status_code = STE_UICC_STATUS_CODE_FAIL;
            response.uicc_status_code_fail_details
            = STE_UICC_STATUS_CODE_FAIL_DETAILS_FILE_NOT_FOUND;
            response.status_word.sw1 = 0x0;
            response.status_word.sw2 = 0x0;
            response.nr_of_plmn = 0;
            sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
        } else {
            ATC_LOG_E("wb_testcase_plmn_write_retry, sending OK");
            /*Second entry, send OK */
            ste_uicc_update_sim_file_plmn_response_t response;
            response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
            response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
            response.status_word.sw1 = 0x0;
            response.status_word.sw2 = 0x0;
            sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
        }

        cpol_alternate++;
        break;
    }
    case wb_testcase_plmn_read_two: {
        ste_uicc_sim_file_read_plmn_response_t response;
        ste_sim_plmn_with_AcT_t PLMNarr[4];
        response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response.status_word.sw1 = 0x0;
        response.status_word.sw2 = 0x0;
        PLMNarr[0].AcT = 0x8080;
        PLMNarr[0].PLMN.MCC1 = 2;
        PLMNarr[0].PLMN.MCC2 = 4;
        PLMNarr[0].PLMN.MCC3 = 0;
        PLMNarr[0].PLMN.MNC1 = 1;
        PLMNarr[0].PLMN.MNC2 = 2;
        PLMNarr[0].PLMN.MNC3 = 0xF;

        /* Add one invalid item to verify that executor removes it */
        PLMNarr[1].AcT = 0x0;
        PLMNarr[1].PLMN.MCC1 = 7;
        PLMNarr[1].PLMN.MCC2 = 8;
        PLMNarr[1].PLMN.MCC3 = 3;
        PLMNarr[1].PLMN.MNC1 = 5;
        PLMNarr[1].PLMN.MNC2 = 4;
        PLMNarr[1].PLMN.MNC3 = 0xE;

        PLMNarr[2].AcT = 0x0;
        PLMNarr[2].PLMN.MCC1 = 7;
        PLMNarr[2].PLMN.MCC2 = 8;
        PLMNarr[2].PLMN.MCC3 = 9;
        PLMNarr[2].PLMN.MNC1 = 5;
        PLMNarr[2].PLMN.MNC2 = 4;
        PLMNarr[2].PLMN.MNC3 = 3;

        /* Add one invalid item to verify that executor removes it */
        PLMNarr[3].AcT = 0x0;
        PLMNarr[3].PLMN.MCC1 = 0xF;
        PLMNarr[3].PLMN.MCC2 = 0xF;
        PLMNarr[3].PLMN.MCC3 = 0xF;
        PLMNarr[3].PLMN.MNC1 = 0xF;
        PLMNarr[3].PLMN.MNC2 = 0xF;
        PLMNarr[3].PLMN.MNC3 = 0xF;

        response.nr_of_plmn = 4;
        response.ste_sim_plmn_with_AcT_p = PLMNarr;
        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
        break;
    }

    case wb_testcase_plmn_read_none: {
        ste_uicc_sim_file_read_plmn_response_t response;
        response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response.status_word.sw1 = 0x0;
        response.status_word.sw2 = 0x0;
        response.nr_of_plmn = 0;
        response.ste_sim_plmn_with_AcT_p = NULL;
        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
        break;
    }
    case wb_testcase_plmn_read_retry: {
        if (cpol_alternate % 2) {
            /* First entry, send FILE_NOT_FOUND */
            ste_uicc_sim_file_read_plmn_response_t response;
            response.uicc_status_code = STE_UICC_STATUS_CODE_FAIL;
            response.uicc_status_code_fail_details
            = STE_UICC_STATUS_CODE_FAIL_DETAILS_FILE_NOT_FOUND;
            response.status_word.sw1 = 0x0;
            response.status_word.sw2 = 0x0;
            response.nr_of_plmn = 0;
            sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
        } else {
            /*Second entry, send valid data */
            ste_uicc_sim_file_read_plmn_response_t response;
            ste_sim_plmn_with_AcT_t PLMNarr[1];
            response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
            response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
            response.status_word.sw1 = 0x0;
            response.status_word.sw2 = 0x0;
            PLMNarr[0].AcT = 0x8080;
            PLMNarr[0].PLMN.MCC1 = 2;
            PLMNarr[0].PLMN.MCC2 = 4;
            PLMNarr[0].PLMN.MCC3 = 0;
            PLMNarr[0].PLMN.MNC1 = 1;
            PLMNarr[0].PLMN.MNC2 = 2;
            PLMNarr[0].PLMN.MNC3 = 0xF;
            response.nr_of_plmn = 1;
            response.ste_sim_plmn_with_AcT_p = PLMNarr;
            sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
        }

        cpol_alternate++;
        break;
    }
    case wb_testcase_plmn_read_retry_failed: {
        /* send FILE_NOT_FOUND */
        ste_uicc_sim_file_read_plmn_response_t response;
        response.uicc_status_code = STE_UICC_STATUS_CODE_FAIL;
        response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_FILE_NOT_FOUND;
        response.status_word.sw1 = 0x0;
        response.status_word.sw2 = 0x0;
        response.nr_of_plmn = 0;
        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
        break;
    }

    case wb_testcase_plmn_read_failed:
    case wb_testcase_plmn_test_failed: {
        ste_uicc_sim_file_read_plmn_response_t response;
        response.uicc_status_code = STE_UICC_STATUS_CODE_FAIL;
        response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response.status_word.sw1 = 0x0;
        response.status_word.sw2 = 0x0;
        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
        break;
    }

    case wb_testcase_plmn_test: {
        ste_uicc_sim_file_read_plmn_response_t response;
        ste_sim_plmn_with_AcT_t PLMNarr[10];
        response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response.status_word.sw1 = 0x0;
        response.status_word.sw2 = 0x0;
        response.nr_of_plmn = 10;
        response.ste_sim_plmn_with_AcT_p = PLMNarr;
        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
        break;
    }

    case wb_testcase_csas_set_2:
    case wb_testcase_cres_set_2:
    case wb_testcase_csas_test:
    case wb_testcase_cres_test: {

        ste_uicc_sim_smsc_get_record_max_response_t response;

        response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response.status_word.sw1 = 0x0;
        response.status_word.sw2 = 0x0;

        response.max_record_id = 10;
        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
        break;
    }

    case wb_testcase_csas_set_3:
    case wb_testcase_cres_set_3: {

        ste_uicc_sim_smsc_get_record_max_response_t response;

        response.uicc_status_code = STE_UICC_STATUS_CODE_CARD_DISCONNECTED;
        response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response.status_word.sw1 = 0x0;
        response.status_word.sw2 = 0x0;

        response.max_record_id = 0;
        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
        break;
    }

    case wb_testcase_csas_set_4:
    case wb_testcase_cres_set_4: {

        ste_uicc_sim_smsc_get_record_max_response_t response;

        response.uicc_status_code = STE_UICC_STATUS_CODE_CARD_DISCONNECTED;
        response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response.status_word.sw1 = 0x0;
        response.status_word.sw2 = 0x0;

        response.max_record_id = 255;
        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
        break;
    }

    case wb_testcase_csas_test_1:
    case wb_testcase_cres_test_1: {

        ste_uicc_sim_smsc_get_record_max_response_t response;

        response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response.status_word.sw1 = 0x0;
        response.status_word.sw2 = 0x0;

        response.max_record_id = 3;
        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
        break;
    }

    case wb_testcase_csas_test_2:
    case wb_testcase_cres_test_2: {

        ste_uicc_sim_smsc_get_record_max_response_t response;

        response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response.status_word.sw1 = 0x0;
        response.status_word.sw2 = 0x0;

        response.max_record_id = 2;
        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
        break;
    }

    case wb_testcase_csas_test_3:
    case wb_testcase_cres_test_3: {

        ste_uicc_sim_smsc_get_record_max_response_t response;

        response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response.status_word.sw1 = 0x0;
        response.status_word.sw2 = 0x0;

        response.max_record_id = 255;
        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
        break;
    }

    case wb_testcase_csas_set_1:
    case wb_testcase_csas_do: {

        ste_uicc_sim_smsc_get_record_max_response_t response;

        response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response.status_word.sw1 = 0x0;
        response.status_word.sw2 = 0x0;

        response.max_record_id = 10;
        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);

        ste_uicc_sim_smsc_save_to_record_response_t response_2;

        response_2.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response_2.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response_2.status_word.sw1 = 0x0;
        response_2.status_word.sw2 = 0x0;

        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response_2, NULL);

        break;
    }

    case wb_testcase_cres_set_1:
    case wb_testcase_cres_do: {

        ste_uicc_sim_smsc_get_record_max_response_t response;

        response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response.status_word.sw1 = 0x0;
        response.status_word.sw2 = 0x0;

        response.max_record_id = 10;
        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);

        ste_uicc_sim_smsc_restore_from_record_response_t response_2;

        response_2.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response_2.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response_2.status_word.sw1 = 0x0;
        response_2.status_word.sw2 = 0x0;

        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response_2, NULL);

        break;
    }

    case wb_testcase_cchc_set2:
    case wb_testcase_cchc_set3: {
        ste_uicc_sim_channel_close_response_t response;

        ste_uicc_status_word_t status_word = {0, 0};
        response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response.status_word = status_word;

        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
        break;
    }

    case wb_testcase_ccho_set2: {
        ste_uicc_sim_channel_open_response_t response;

        ste_uicc_status_word_t status_word = {0, 0};
        response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response.status_word = status_word;
        response.session_id = 1;

        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);

        break;
    }

    case wb_testcase_cgla_set_3: {
        ste_uicc_status_word_t status_word = {0, 0};

        ste_uicc_sim_channel_send_response_t response;
        uint8_t temp_apdu[4];
        temp_apdu[0] = 180;
        temp_apdu[1] = 181;
        temp_apdu[2] = 182;
        temp_apdu[3] = 183;

        response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response.status_word = status_word;
        response.apdu_len = 4;
        response.apdu = (uint8_t *)&temp_apdu;

        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
        break;
    }

    case wb_testcase_csim_set_1: {
        ste_uicc_status_word_t status_word = { 0, 0 };

        ste_uicc_sim_channel_open_response_t response;

        response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response.uicc_status_code_fail_details
        = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response.status_word = status_word;
        response.session_id = 1;

        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
        break;
    }

    case wb_testcase_csim_set_2:
    case wb_testcase_csim_set_2_1: {
        ste_uicc_status_word_t status_word = {0, 0};

        ste_uicc_sim_channel_send_response_t response;
        uint8_t temp_apdu[4];
        temp_apdu[0] = 180;
        temp_apdu[1] = 181;
        temp_apdu[2] = 182;
        temp_apdu[3] = 183;

        response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
        response.uicc_status_code_fail_details
        = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        response.status_word = status_word;
        response.apdu_len = 4;
        response.apdu = (uint8_t *) &temp_apdu;

        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
        break;
    }

    case wb_testcase_eshlvocr_test: {
        ste_cat_answer_call_response_t response;
        response.status = 0;
        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);
        break;
    }

    case wb_testcase_efsim_read: {

        ste_uicc_sim_card_status_response_t response;

        response.card_state = SIM_CARD_TYPE_UICC;
        response.upin_state = STE_UICC_PIN_STATE_UNKNOWN;

        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response, NULL);

        ste_uicc_sim_app_status_response_t response_2;

        response_2.status = STE_UICC_STATUS_CODE_OK;
        response_2.app_type = SIM_APP_MARK_GSM_UMTS;
        response_2.app_state = STE_UICC_APP_STATE_ACTIVE;
        response_2.aid_len = 3;
        response_2.aid[0] = 0x32;
        response_2.aid[1] = 0x33;
        response_2.aid[2] = 0x34;
        response_2.label_len = 0;
        response_2.label = NULL;
        response_2.mark = SIM_APP_MARK_GSM_UMTS;
        response_2.pin_mode = SIM_APP_PIN_MODE_NORMAL;
        response_2.pin_state = STE_UICC_PIN_STATE_ENABLED;
        response_2.pin2_state = STE_UICC_PIN_STATE_ENABLED;

        sim_client_callback(CAUSE_DUMMY, EXECUTOR.client_tag_p, &response_2, NULL);

        break;
    }
    case wb_testcase_esimrf_3: {
        ste_cat_pc_refresh_ind_t response_ind;

        response_ind.simd_tag = (uintptr_t)NULL;
        response_ind.type = STE_SIM_PC_REFRESH_INIT_AND_FULL_FILE_CHANGE;

        sim_client_callback(STE_CAT_CAUSE_PC_REFRESH_IND, EXECUTOR.client_tag_p, &response_ind, NULL);

        break;
    }

    case wb_testcase_esimrf_4: {

        ste_cat_pc_refresh_ind_t response_ind;

        response_ind.simd_tag = (uintptr_t)NULL;
        response_ind.type = STE_SIM_PC_REFRESH_FILE_CHANGE;

        sim_client_callback(STE_CAT_CAUSE_PC_REFRESH_IND, EXECUTOR.client_tag_p, &response_ind, NULL);

        break;
    }

    case wb_testcase_esimrf_5: {

        ste_cat_pc_refresh_ind_t response_ind;

        response_ind.simd_tag = (uintptr_t)NULL;
        response_ind.type = STE_SIM_PC_REFRESH_INIT_AND_FILE_CHANGE;

        sim_client_callback(STE_CAT_CAUSE_PC_REFRESH_IND, EXECUTOR.client_tag_p, &response_ind, NULL);

        break;
    }

    case wb_testcase_esimrf_6: {

        ste_cat_pc_refresh_ind_t response_ind;

        response_ind.simd_tag = (uintptr_t)NULL;
        response_ind.type = STE_SIM_PC_REFRESH_INIT;

        sim_client_callback(STE_CAT_CAUSE_PC_REFRESH_IND, EXECUTOR.client_tag_p, &response_ind, NULL);

        break;
    }

    case wb_testcase_esimrf_7: {

        ste_cat_pc_refresh_ind_t response_ind;

        response_ind.simd_tag = (uintptr_t)NULL;
        response_ind.type = STE_SIM_PC_REFRESH_UICC_RESET;

        sim_client_callback(STE_CAT_CAUSE_PC_REFRESH_IND, EXECUTOR.client_tag_p, &response_ind, NULL);

        break;
    }

    case wb_testcase_esimrf_8: {

        ste_cat_pc_refresh_ind_t response_ind;

        response_ind.simd_tag = (uintptr_t)NULL;
        response_ind.type = STE_SIM_PC_REFRESH_3G_APP_RESET;

        sim_client_callback(STE_CAT_CAUSE_PC_REFRESH_IND, EXECUTOR.client_tag_p, &response_ind, NULL);

        break;
    }

    case wb_testcase_esimrf_9: {

        ste_cat_pc_refresh_ind_t response_ind;

        response_ind.simd_tag = (uintptr_t)NULL;
        response_ind.type = STE_SIM_PC_REFRESH_3G_SESSION_RESET;

        sim_client_callback(STE_CAT_CAUSE_PC_REFRESH_IND, EXECUTOR.client_tag_p, &response_ind, NULL);

        break;
    }

    case wb_testcase_esimrf_10: {


        ste_cat_pc_refresh_ind_t response_ind;

        response_ind.simd_tag = (uintptr_t)NULL;
        response_ind.type = STE_SIM_PC_REFRESH_RESERVED;

        sim_client_callback(STE_CAT_CAUSE_PC_REFRESH_IND, EXECUTOR.client_tag_p, &response_ind, NULL);

        break;
    }

    case wb_testcase_esimrf_11: {

        ste_cat_pc_refresh_file_ind_t response_ind;

        response_ind.simd_tag = (uintptr_t)NULL;
        response_ind.path.pathlen = 2;
        response_ind.path.path[0] = 0x2f;
        response_ind.path.path[1] = 0xe2;

        sim_client_callback(STE_CAT_CAUSE_PC_REFRESH_FILE_IND, EXECUTOR.client_tag_p, &response_ind, NULL);

        break;
    }

    case wb_testcase_esimrf_12: {

        ste_cat_pc_refresh_file_ind_t response_ind;

        response_ind.simd_tag = (uintptr_t)NULL;
        response_ind.path.pathlen = 5;
        response_ind.path.path[0] = 0xfe;
        response_ind.path.path[1] = 0xdc;
        response_ind.path.path[2] = 0xba;
        response_ind.path.path[3] = 0x2f;
        response_ind.path.path[4] = 0xe2;
        sim_client_callback(STE_CAT_CAUSE_PC_REFRESH_FILE_IND, EXECUTOR.client_tag_p, &response_ind, NULL);

        break;
    }

    case wb_testcase_esimrf_13: {

        ste_cat_pc_refresh_file_ind_t response_ind;

        response_ind.simd_tag = (uintptr_t)NULL;
        response_ind.path.pathlen = 7;
        response_ind.path.path[0] = 0xfe;
        response_ind.path.path[1] = 0xdc;
        response_ind.path.path[2] = 0xba;
        response_ind.path.path[3] = 0x12;
        response_ind.path.path[4] = 0x34;
        response_ind.path.path[5] = 0x2f;
        response_ind.path.path[6] = 0xe2;
        sim_client_callback(STE_CAT_CAUSE_PC_REFRESH_FILE_IND, EXECUTOR.client_tag_p, &response_ind, NULL);

        break;
    }
#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
#include <inc_wb_ste_sim_read.h>
#endif

    default: {
        ATC_LOG_E("ste_sim_read(): unknown test case!");
        break;
    }
    }

    return 0;
}

ste_sim_t *ste_sim_new_st(const ste_sim_closure_t *closure)
{
    return (ste_sim_t *) 1;
}

int ste_sim_fd(const ste_sim_t *sim)
{
    return FD_SIM_REQUEST;
}

int ste_sim_connect(ste_sim_t *sim_p,
                    uintptr_t client_tag)
{
    EXECUTOR.simclient_p = (void *)client_tag;
    return 0;
}

int ste_sim_disconnect(ste_sim_t *sim,
                       uintptr_t client_tag)
{
    return 0;
}

void ste_sim_delete(ste_sim_t *sim,
                    uintptr_t client_tag)
{
    return;
}

int ste_sim_ping(ste_sim_t *sim,
                 uintptr_t client_tag)
{
    return 0;
}

int ste_uicc_pin_info(ste_sim_t *uicc,
                      uintptr_t client_tag,
                      sim_pin_puk_id_t pin)
{
    EXECUTOR.client_tag_p = (void *)client_tag;
    return 0;
}

uicc_request_status_t ste_uicc_sim_file_read_binary(ste_sim_t *uicc,
        uintptr_t client_tag,
        int file_id,
        int offset,
        int length,
        const char *file_path)
{
    EXECUTOR.client_tag_p = (void *)client_tag;
    return 0;
}

uicc_request_status_t ste_uicc_sim_file_read_record(ste_sim_t *uicc,
        uintptr_t client_tag,
        int file_id,
        int offset,
        int length,
        const char *file_path)
{
    EXECUTOR.client_tag_p = (void *)client_tag;
    return 0;
}

uicc_request_status_t ste_uicc_sim_get_file_information(ste_sim_t *uicc,
        uintptr_t client_tag,
        int file_id,
        const char *file_path,
        ste_uicc_sim_get_file_info_type_t type)
{
    EXECUTOR.client_tag_p = (void *)client_tag;
    return 0;
}

uicc_request_status_t ste_uicc_sim_file_update_binary(ste_sim_t *uicc,
        uintptr_t client_tag,
        int file_id,
        int offset,
        int length,
        const char *file_path,
        const uint8_t *data)
{
    EXECUTOR.client_tag_p = (void *)client_tag;
    return 0;
}

uicc_request_status_t ste_uicc_sim_file_update_record(ste_sim_t *uicc,
        uintptr_t client_tag,
        int file_id,
        int record_id,
        int length,
        const char *file_path,
        const uint8_t *data)
{
    EXECUTOR.client_tag_p = (void *)client_tag;
    return 0;
}

uicc_request_status_t ste_uicc_sim_get_state(ste_sim_t *uicc_p,
        uintptr_t client_tag)
{
    if (NULL == (void *)client_tag) {
        return -1;
    }

    EXECUTOR.client_tag_p = (void *)client_tag;
    EXECUTOR.sim_state = SIM_WAITING_FOR_GET_STATE;

    return 0;
}

uicc_request_status_t ste_uicc_sim_file_update_plmn(ste_sim_t *uicc, uintptr_t  client_tag, int index, ste_sim_plmn_with_AcT_t *plmn, ste_uicc_sim_plmn_file_id_t file_id)
{
    if (NULL == (void *)client_tag) {
        ATC_LOG_E("ste_uicc_sim_file_update_plmn(): client_tag is NULL");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    EXECUTOR.client_tag_p = (void *) client_tag;

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_plmn_remove_element:
        ATC_LOG_E("test case remove");

        if (plmn != NULL) {
            ATC_LOG_E("plmn is not NULL");
            return UICC_REQUEST_STATUS_FAILED;
        }

        break;
    default:
        break;
    }

    return UICC_REQUEST_STATUS_OK;
}

uicc_request_status_t ste_uicc_sim_file_read_plmn(ste_sim_t *uicc,
        uintptr_t client_tag,
        ste_uicc_sim_plmn_file_id_t file_id)
{
    static int cpol_alternate = 1;

    if (NULL == (void *) client_tag) {
        ATC_LOG_E("ste_uicc_sim_file_read_plmn_sel(): client_tag is NULL");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    EXECUTOR.client_tag_p = (void *) client_tag;

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_plmn_read_retry_failed: {
        /*First entry, return OK */
        if (cpol_alternate % 2) {
            cpol_alternate++;
            return UICC_REQUEST_STATUS_OK;
        } else {
            /*Second entry, return FAIL */
            cpol_alternate++;
            return UICC_REQUEST_STATUS_FAILED;
        }

        break;
    }
    default:
        return UICC_REQUEST_STATUS_OK;
    }
}

int ste_uicc_pin_verify(ste_sim_t *uicc,
                        uintptr_t client_tag,
                        const sim_pin_id_t pin_id,
                        const char *buf)
{
    if (NULL == (void *)client_tag) {
        return -1;
    }

    if (!strcmp(buf, "1234")) {
        EXECUTOR.sim_state = SIM_WAITING_FOR_VERIFY_OK;
    } else {
        EXECUTOR.sim_state = SIM_WAITING_FOR_VERIFY_FAIL;
    }

    EXECUTOR.client_tag_p = (void *)client_tag;

    return 0;
}

int ste_uicc_pin_unblock(ste_sim_t *uicc,
                         uintptr_t client_tag,
                         const sim_pin_id_t pin_id,
                         const char *pin,
                         const char *puk)
{
    if (NULL == (void *)client_tag) {
        return -1;
    }

    if (!strcmp(puk, "12345678")) {
        EXECUTOR.sim_state = SIM_WAITING_FOR_VERIFY_OK;
    } else {
        EXECUTOR.sim_state = SIM_WAITING_FOR_VERIFY_FAIL;
    }

    EXECUTOR.client_tag_p = (void *)client_tag;

    return 0;

}

int ste_uicc_pin_disable(ste_sim_t *uicc,
                         uintptr_t client_tag,
                         const char *buf)
{
    if (NULL == (void *)client_tag) {
        return -1;
    }

    EXECUTOR.client_tag_p = (void *)client_tag;

    return 0;
}

int ste_uicc_pin_enable(ste_sim_t *uicc,
                        uintptr_t client_tag,
                        const char *buf)
{
    if (NULL == (void *)client_tag) {
        return -1;
    }

    EXECUTOR.client_tag_p = (void *)client_tag;

    return 0;
}


int ste_uicc_pin_change(ste_sim_t *uicc,
                        uintptr_t client_tag,
                        const char *old_pin,
                        const char *new_pin,
                        const sim_pin_id_t pin_id)
{
    if (NULL == (void *)client_tag) {
        return -1;
    }

    EXECUTOR.client_tag_p = (void *)client_tag;

    return 0;
}

uicc_request_status_t ste_uicc_sim_smsc_set_active(ste_sim_t *uicc,
        uintptr_t client_tag,
        ste_sim_call_number_t *smsc_p)
{
    int len = 0;

    if (NULL == (void *) client_tag) {
        ATC_LOG_E("ste_uicc_sim_file_update_smsc(): client_tag is NULL");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (NULL == smsc_p) {
        ATC_LOG_E("ste_uicc_sim_file_update_smsc(): smsc_p is NULL");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    EXECUTOR.client_tag_p = (void *) client_tag;

    sms_service_center_p = malloc(sizeof(ste_sim_call_number_t));
    memcpy(sms_service_center_p, smsc_p, sizeof(ste_sim_call_number_t));

    len = (strlen(smsc_p->num_text.text_p) + 1) * sizeof(char);
    sms_service_center_p->num_text.text_p = malloc(len);

    memcpy(sms_service_center_p->num_text.text_p, smsc_p->num_text.text_p, len);

    return UICC_REQUEST_STATUS_OK;
}

uicc_request_status_t ste_uicc_sim_smsc_get_active(ste_sim_t *uicc,
        uintptr_t client_tag)
{
    if (NULL == (void *)client_tag) {
        ATC_LOG_E("ste_uicc_sim_file_read_smsc(): client_tag is NULL");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    EXECUTOR.client_tag_p = (void *)client_tag;
    return UICC_REQUEST_STATUS_OK;
}

uicc_request_status_t ste_uicc_sim_file_read_imsi(ste_sim_t *uicc,
        uintptr_t client_tag)
{
    if (NULL == (void *)client_tag) {
        ATC_LOG_E("ste_uicc_sim_file_read_imsi(): client_tag is NULL");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    EXECUTOR.client_tag_p = (void *)client_tag;
    return UICC_REQUEST_STATUS_OK;
}

int ste_cat_register(ste_sim_t *cat, uintptr_t client_tag, uint32_t reg_events)
{
    if (NULL == (void *)client_tag) {
        return -1;
    }

    EXECUTOR.client_tag_p = (void *)client_tag;

    return 0;
}

int ste_cat_deregister(ste_sim_t *cat, uintptr_t client_tag)
{
    if (NULL == (void *)client_tag) {
        return -1;
    }

    EXECUTOR.client_tag_p = (void *)client_tag;
    return 0;
}

#define STKE_SEND_STRING "0000"

int ste_cat_envelope_command(ste_sim_t *cat,
                             uintptr_t client_tag,
                             const char *buf,
                             unsigned len)
{
    EXECUTOR.client_tag_p = (void *)client_tag;

    if (!strncmp(buf, STKE_SEND_STRING, len)) {
        return 0;
    }

    return -1;
}

int ste_cat_get_terminal_profile(ste_sim_t *cat, uintptr_t client_tag)
{
    if (NULL == (void *)client_tag) {
        return -1;
    }

    EXECUTOR.client_tag_p = (void *)client_tag;

    return 0;
}

int ste_cat_terminal_response(ste_sim_t *uicc,
                              uintptr_t client_tag,
                              const char *buf,
                              unsigned len)
{
    if (NULL == (void *) client_tag) {
        ATC_LOG_E("ste_cat_terminal_response(): client_tag is NULL");
        return -1;
    }

    EXECUTOR.client_tag_p = (void *) client_tag;

    return 0;
}

uicc_request_status_t ste_uicc_get_app_info(ste_sim_t *uicc, uintptr_t client_tag)
{
    if (NULL == (void *)client_tag) {
        ATC_LOG_E("ste_uicc_sim_file_read_imsi(): client_tag is NULL");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    EXECUTOR.client_tag_p = (void *)client_tag;
    return UICC_REQUEST_STATUS_OK;
}

uicc_request_status_t ste_uicc_get_service_availability(ste_sim_t *uicc,
        uintptr_t client_tag,
        sim_service_type_t service_type)
{
    if (NULL == (void *)client_tag) {
        ATC_LOG_E("client_tag is NULL");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    EXECUTOR.client_tag_p = (void *)client_tag;
    return UICC_REQUEST_STATUS_OK;
}

uicc_request_status_t ste_uicc_update_service_table(ste_sim_t *uicc,
        uintptr_t client_tag,
        const char *pin,
        sim_service_type_t service_type,
        uint8_t enable_service)
{
    if (NULL == (void *)client_tag) {
        ATC_LOG_E("ste_uicc_update_service_table(): client_tag is NULL");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    EXECUTOR.client_tag_p = (void *)client_tag;
    return UICC_REQUEST_STATUS_OK;
}


uicc_request_status_t ste_uicc_get_service_table(ste_sim_t *uicc,
        uintptr_t client_tag,
        sim_service_type_t service_type)
{
    if (NULL == (void *)client_tag) {
        ATC_LOG_E("ste_uicc_get_service_table(): client_tag is NULL");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    EXECUTOR.client_tag_p = (void *)client_tag;
    return UICC_REQUEST_STATUS_OK;
}

uicc_request_status_t ste_uicc_sim_icon_read(ste_sim_t *uicc,
        uintptr_t client_tag,
        int file_id,
        int record_or_offset,
        int length)
{
    if (NULL == (void *)client_tag) {
        ATC_LOG_E("ste_uicc_sim_icon_read(): client_tag is NULL");
        return -1;
    }

    EXECUTOR.client_tag_p = (void *)client_tag;
    return 0;
}

uicc_request_status_t ste_uicc_get_subscriber_number(ste_sim_t *uicc_p,
        uintptr_t client_tag)
{
    if (NULL == (void *)client_tag) {
        ATC_LOG_E("ste_uicc_get_subscriber_number(): client_tag is NULL");
        return -1;
    }

    EXECUTOR.client_tag_p = (void *)client_tag;
    return 0;
}

uicc_request_status_t ste_uicc_sim_channel_open(ste_sim_t       *uicc,
        uintptr_t       client_tag,
        uint32_t        aid_len,
        uint8_t         *aid)
{
    if (NULL == (void *)client_tag) {
        ATC_LOG_E("ste_uicc_sim_channel_open(): client_tag is NULL");
        return -1;
    }

    EXECUTOR.client_tag_p = (void *)client_tag;
    return 0;
}

uicc_request_status_t ste_uicc_sim_channel_opened(ste_sim_t     *uicc,
        uintptr_t     client_tag,
        uint16_t      session_id)
{
    if (NULL == (void *)client_tag) {
        ATC_LOG_E("ste_uicc_sim_channel_opened(): client_tag is NULL");
        return -1;
    }

    EXECUTOR.client_tag_p = (void *)client_tag;
    return 0;
}

uicc_request_status_t ste_uicc_sim_channel_closing(ste_sim_t    *uicc,
        uintptr_t    client_tag,
        uint16_t     session_id)
{
    if (NULL == (void *)client_tag) {
        ATC_LOG_E("ste_uicc_sim_channel_closing(): client_tag is NULL");
        return -1;
    }

    EXECUTOR.client_tag_p = (void *)client_tag;
    return 0;
}

uicc_request_status_t ste_uicc_sim_channel_close(ste_sim_t      *uicc,
        uintptr_t      client_tag,
        uint16_t       session_id)
{
    if (NULL == (void *)client_tag) {
        ATC_LOG_E("ste_uicc_sim_channel_close(): client_tag is NULL");
        return -1;
    }

    EXECUTOR.client_tag_p = (void *)client_tag;
    return 0;
}

uicc_request_status_t ste_uicc_sim_channel_send(ste_sim_t   *uicc,
        uintptr_t   client_tag,
        uint16_t    session_id,
        uint32_t    apdu_len,
        uint8_t     *apdu)
{
    if (NULL == (void *)client_tag) {
        ATC_LOG_E("ste_uicc_sim_channel_send(): client_tag is NULL");
        return -1;
    }

    EXECUTOR.client_tag_p = (void *)client_tag;
    return 0;
}



uicc_request_status_t ste_uicc_sim_smsc_get_record_max(ste_sim_t *uicc,
        uintptr_t client_tag)
{
    if (NULL == (void *)client_tag) {
        ATC_LOG_E("ste_uicc_sim_smsc_get_record_max(): client_tag is NULL");
        return -1;
    }

    EXECUTOR.client_tag_p = (void *)client_tag;
    return 0;
}

uicc_request_status_t ste_uicc_sim_smsc_restore_from_record(ste_sim_t *uicc,
        uintptr_t client_tag,
        int record_id)
{
    if (NULL == (void *)client_tag) {
        ATC_LOG_E("ste_uicc_sim_smsc_restore_from_record(): client_tag is NULL");
        return -1;
    }

    EXECUTOR.client_tag_p = (void *)client_tag;
    return 0;
}

uicc_request_status_t ste_uicc_sim_smsc_save_to_record(ste_sim_t *uicc,
        uintptr_t client_tag,
        int record_id)
{
    if (NULL == (void *)client_tag) {
        ATC_LOG_E("ste_uicc_sim_smsc_save_to_record(): client_tag is NULL");
        return -1;
    }

    EXECUTOR.client_tag_p = (void *)client_tag;
    return 0;
}

uicc_request_status_t ste_uicc_card_status(ste_sim_t *uicc,
        uintptr_t client_tag)
{
    if (NULL == (void *)client_tag) {
        ATC_LOG_E("ste_uicc_card_status(): client_tag is NULL");
        return -1;
    }

    EXECUTOR.client_tag_p = (void *)client_tag;
    return 0;
}

int ste_cat_answer_call(ste_sim_t    *cat,
                        uintptr_t     client_tag,
                        int           answer)
{
    EXECUTOR.client_tag_p = (void *)client_tag;

    if (1 == answer) {
        return 0;
    }

    return -1;
}

uicc_request_status_t ste_uicc_app_status(ste_sim_t *uicc,
        uintptr_t client_tag,
        int app_index)
{
    if (NULL == (void *)client_tag) {
        ATC_LOG_E("ste_uicc_app_status(): client_tag is NULL");
        return -1;
    }

    EXECUTOR.client_tag_p = (void *)client_tag;
    return 0;
}

int ste_cat_enable(ste_sim_t              *cat,
                   uintptr_t               client_tag,
                   ste_cat_enable_type_t   enable_type)
{
    if (NULL == (void *)client_tag) {
        ATC_LOG_E("ste_cat_enable(): client_tag is NULL");
        return -1;
    }

    EXECUTOR.client_tag_p = (void *)client_tag;
    return 0;
}

uicc_request_status_t ste_uicc_sim_power_off(ste_sim_t * uicc,
                                             uintptr_t client_tag)
{
    if (NULL == (void *)client_tag) {
        ATC_LOG_E("ste_uicc_sim_power_off(): client_tag is NULL");
        return -1;
    }

    EXECUTOR.client_tag_p = (void *)client_tag;
    return 0;
}

uicc_request_status_t ste_uicc_sim_power_on(ste_sim_t * uicc,
                                            uintptr_t client_tag)
{
    if (NULL == (void *)client_tag) {
        ATC_LOG_E("ste_uicc_sim_power_on(): client_tag is NULL");
        return -1;
    }

    EXECUTOR.client_tag_p = (void *)client_tag;
    return 0;
}

