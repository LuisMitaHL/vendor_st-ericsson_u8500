/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Implementation internal interfaces for pre-dial checks.
 */

#ifndef __cn_pdc_internal_h__
#define __cn_pdc_internal_h__ (1)

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "cn_log.h"
#include "cn_data_types.h"
#include "message_handler.h"
#include "cn_ss_command_handling.h"
#include "cn_pdc.h"
#include "fdmon.h"
#include "sim.h"

typedef enum {
    CN_SERVICE_TYPE_UNKNOWN,
    CN_SERVICE_TYPE_VOICE_CALL,
    CN_SERVICE_TYPE_SS,
    CN_SERVICE_TYPE_USSD,
} cn_service_type_t;

typedef struct {
    cn_service_type_t service_type;
    cn_bool_t         sat_initiated;
    union {
        cn_dial_t       voice_call;
        cn_ss_command_t ss;
        cn_ussd_info_t  ussd;
    } service;
} cn_pdc_input_t;

typedef struct {
    cn_service_type_t service_type;
    cn_bool_t         emergency_call;
    union {
        cn_dial_t       voice_call;
        cn_ss_command_t ss;
        cn_ussd_info_t  ussd;
    } service;
} cn_pdc_output_t;

typedef enum {
    CN_PDC_STATE_UNKNOWN,
    CN_PDC_STATE_ECC,
    CN_PDC_STATE_FDN,
    CN_PDC_STATE_SAT_CC,
    CN_PDC_STATE_ECC_AFTER_SAT_CC,
} cn_pdc_state_t;

typedef enum {
    CN_PDC_FDN_STATE_INITIAL = 0,
    CN_PDC_FDN_STATE_WAIT_FOR_UST_RESPONSE,
    CN_PDC_FDN_STATE_WAIT_FOR_EST_RESPONSE,
    CN_PDC_FDN_STATE_WAIT_FOR_FDN_LIST_RESPONSE,
    CN_PDC_FDN_STATE_ERROR,
} cn_pdc_fdn_state_t;

typedef enum {
    CN_PDC_ECC_STATE_INITIAL = 0,
    CN_PDC_ECC_STATE_WAIT_FOR_SIM_LIST,
    CN_PDC_ECC_STATE_RECEIVED_SIM_LIST,
    CN_PDC_ECC_STATE_POST_SAT_CC,
    CN_PDC_ECC_STATE_ERROR
} cn_pdc_ecc_state_t;

typedef  enum {
    CN_PDC_SAT_STATE_INITIAL = 0,
    CN_PDC_SAT_STATE_QUERY_RESPONSE,
    CN_PDC_SAT_STATE_RESPONSE,
    CN_PDC_SAT_STATE_UNKOWN
} cn_pdc_sat_state_t;


struct _cn_pdc_t {
    cn_pdc_state_t     state;
    cn_pdc_sat_state_t sat_state;
    cn_pdc_input_t    *pdc_input_p;
    cn_pdc_output_t   *pdc_output_p;
    request_record_t  *record_p;
    cn_pdc_ecc_state_t pdc_ecc_state;
    cn_pdc_fdn_state_t pdc_fdn_state;
};

typedef enum {
    CN_ECC_RESULT_UNKNOWN,
    CN_ECC_RESULT_ERROR,
    CN_ECC_RESULT_PENDING,
    CN_ECC_RESULT_CHECK_NOT_RELEVANT,
    CN_ECC_RESULT_EMERGENCY_CALL,
    CN_ECC_RESULT_NO_EMERGENCY_CALL
} cn_ecc_result_t;

typedef enum {
    CN_FDN_RESULT_UNKNOWN,
    CN_FDN_RESULT_ERROR,
    CN_FDN_RESULT_PENDING,
    CN_FDN_RESULT_CHECK_NOT_RELEVANT,
    CN_FDN_RESULT_REJECT,
    CN_FDN_RESULT_ACCEPT
} cn_fdn_result_t;

typedef enum {
    CN_SAT_CC_RESULT_UNKNOWN,
    CN_SAT_CC_RESULT_ERROR,
    CN_SAT_CC_RESULT_PENDING,
    CN_SAT_CC_RESULT_DONE
} cn_sat_cc_result_t;

typedef struct {
    cn_sat_cc_result_t result;                  /* result/status of the PDC operation*/
    ste_cat_call_control_result_t  cc_result;   /* result from the SAT CC call */
    ste_cat_cc_dialled_string_type_t cc_type;   /* Type of operation (Call setup, SS, USSD etc. */
    unsigned    no_of_characters;
    char text_p[CN_MAX_STRING_BUFF];            /* This buffer is large enough for all cases. */
    ste_sim_text_coding_t text_coding;
    ste_sim_type_of_number_t ton;
    ste_sim_numbering_plan_id_t npi;
    uint8_t dcs;                                /* data coding scheme, only relevant for STE_CAT_CC_USSD */
} cn_sat_cc_result_struct_t;

/* Message handling */
cn_pdc_input_t* cn_pdc_translate_message_to_pdc_input(cn_message_type_t message_type, void *data_p);
cn_message_type_t get_response_message_type_from_request(cn_message_type_t type);

/* ECC, FDN, SAT CC functions*/
cn_ecc_result_t cn_pdc_ecc_check(cn_pdc_t* pdc_p);
cn_fdn_result_t cn_pdc_fdn_check(cn_pdc_t* pdc_p);
void cn_pdc_sat_cc(cn_pdc_t* pdc_p, cn_sat_cc_result_struct_t* result_p);

/* Utility functions */
void initialize_pdc_data_in_request_record(void *data_p, request_record_t *record_p);
void copy_input_data_to_output_data(cn_pdc_t *pdc_data_p);
void translate_cat_cc_result_to_new_input_data_for_ecc_check(cn_sat_cc_result_struct_t* response_p, cn_pdc_t* pdc_data_p);
char* convert_bcd_ascii(char * bcd_p,int len);
cn_message_type_t get_response_message_type_from_request(cn_message_type_t type);
cn_ss_procedure_type_t convert_call_forward_status_to_procedure_type(cn_uint32_t status);
cn_sint32_t cn_util_convert_cn_service_class_to_ss_class(cn_sint32_t cn_service_class);
cn_ss_ton_t cn_util_convert_ss_toa_to_cn_ss_toa(int type);
cn_ss_type_t convert_reason_to_cn_cf_ss_type(cn_uint32_t reason);
cn_ss_type_t convert_facility_to_cn_call_barring_ss_type(char *facility_p);

#endif /* __cn_pdc_internal_h__ */

