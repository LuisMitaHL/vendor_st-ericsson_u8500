/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "sim_internal.h"
#include "catd_modem_types.h"
#include "uicc_internal.h"
#include "simd.h"
#include "uiccd_msg.h"
#include "uicc_file_service_table_common.h"
#include "uicc_file_paths.h"
#include "uicc_state_machine.h"

static int uiccd_encode_and_send_get_service_availability_response(int fd,
                                                                   uintptr_t client_tag,
                                                                   sim_uicc_status_code_t uicc_status_code,
                                                                   sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                   sim_uicc_service_availability_t service_status )
{
    char                    *buf_p = NULL;
    char                    *p = NULL;
    size_t                   buf_len;

    buf_len = sizeof(uicc_status_code);
    buf_len += sizeof(uicc_status_code_fail_details);
    buf_len += sizeof(service_status);

    buf_p = malloc(buf_len);
    if (buf_p == NULL){
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_get_service_availability_response, memory allocation failed");
        return -1;
    }
    p = buf_p;

    p = sim_enc(p, &uicc_status_code, sizeof(uicc_status_code));
    p = sim_enc(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details));
    p = sim_enc(p, &service_status, sizeof(service_status));

    if (buf_len != (size_t)(p-buf_p)) {
        free(buf_p);
        return -1;
    }

    sim_send_generic(fd,
                     STE_UICC_RSP_GET_SERVICE_AVAILABILITY,
                     buf_p,
                     buf_len,
                     client_tag);

    free(buf_p);

    return 0;
}

static int uiccd_read_usim_service_table_service_availability(ste_sim_ctrl_block_t * ctrl_p, sim_uicc_service_type_t service_type, int filesize)
{
    int                 i;
    ste_modem_t       * m;
    int                 file_id, offset, length;
    const char        * file_path;

    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_I, "uicc : uiccd_read_usim_service_table_service_availability: No modem");
        return -1;
    }

    file_path = SERVICE_TABLE_UST_USIM_PATH;
    file_id = USIM_SERVICE_TABLE_UST_FILE_ID;
    length = 1;

    catd_log_f(SIM_LOGGING_I, "uicc : ENTER uiccd_read_usim_service_table_service_availability: Requested Service type = %d",service_type);

    switch (service_type){
      case SIM_UICC_SERVICE_TYPE_LOCAL_PHONE_BOOK:
      case SIM_UICC_SERVICE_TYPE_FDN:
      case SIM_UICC_SERVICE_TYPE_EXT_2:
      case SIM_UICC_SERVICE_TYPE_SDN:
      case SIM_UICC_SERVICE_TYPE_EXT_3:
      case SIM_UICC_SERVICE_TYPE_BDN:
      case SIM_UICC_SERVICE_TYPE_EXT_4:
      case SIM_UICC_SERVICE_TYPE_OCI_AND_OCT:
      {
          offset = 0;
      }
      break;

      case SIM_UICC_SERVICE_TYPE_ICI_AND_ICT:
      case SIM_UICC_SERVICE_TYPE_SMS:
      case SIM_UICC_SERVICE_TYPE_SMSR:
      case SIM_UICC_SERVICE_TYPE_SMSP:
      case SIM_UICC_SERVICE_TYPE_AOC:
      case SIM_UICC_SERVICE_TYPE_CCP:
      case SIM_UICC_SERVICE_TYPE_CBMI:
      case SIM_UICC_SERVICE_TYPE_CBMIR:
      {
          offset = 1;
      }
      break;

      case SIM_UICC_SERVICE_TYPE_GID1:
      case SIM_UICC_SERVICE_TYPE_GID2:
      case SIM_UICC_SERVICE_TYPE_SPN:
      case SIM_UICC_SERVICE_TYPE_PLMNWACT:
      case SIM_UICC_SERVICE_TYPE_MSISDN:
      case SIM_UICC_SERVICE_TYPE_IMG:
      case SIM_UICC_SERVICE_TYPE_SOLSA:
      case SIM_UICC_SERVICE_TYPE_ENH_PRECEDENCE_PREEMPTION:
      {
          offset = 2;
      }
      break;

      case SIM_UICC_SERVICE_TYPE_AAEM:
      case SIM_UICC_SERVICE_TYPE_GSM_ACCESS:
      case SIM_UICC_SERVICE_TYPE_SMS_PP:
      case SIM_UICC_SERVICE_TYPE_SMS_CB:
      case SIM_UICC_SERVICE_TYPE_CALL_CONTROL_BY_USIM:
      case SIM_UICC_SERVICE_TYPE_MO_SMS_CONTROL_BY_USIM:
      case SIM_UICC_SERVICE_TYPE_RUN_AT_COMMAND:
      {
          offset = 3;
      }
      break;

      case SIM_UICC_SERVICE_TYPE_EST:
      case SIM_UICC_SERVICE_TYPE_ACL:
      case SIM_UICC_SERVICE_TYPE_DCK:
      case SIM_UICC_SERVICE_TYPE_CNL:
      case SIM_UICC_SERVICE_TYPE_GSM_SECURITY_CONTEXT:
      case SIM_UICC_SERVICE_TYPE_CPBCCH:
      case SIM_UICC_SERVICE_TYPE_INV_SCAN:
      {
          offset = 4;
      }
      break;

      case SIM_UICC_SERVICE_TYPE_MEXE:
      case SIM_UICC_SERVICE_TYPE_OPLMNWACT:
      case SIM_UICC_SERVICE_TYPE_HPLMNWACT:
      case SIM_UICC_SERVICE_TYPE_EXT_5:
      case SIM_UICC_SERVICE_TYPE_PNN:
      case SIM_UICC_SERVICE_TYPE_OPL:
      case SIM_UICC_SERVICE_TYPE_MBDN:
      case SIM_UICC_SERVICE_TYPE_MWIS:
      {
          offset = 5;
      }
      break;

      case SIM_UICC_SERVICE_TYPE_CFIS:
      case SIM_UICC_SERVICE_TYPE_SPDI:
      case SIM_UICC_SERVICE_TYPE_MMS:
      case SIM_UICC_SERVICE_TYPE_EXT_8:
      case SIM_UICC_SERVICE_TYPE_CALL_CONTROL_ON_GPRS:
      case SIM_UICC_SERVICE_TYPE_MMSUCP:
      case SIM_UICC_SERVICE_TYPE_NIA:
      {
          offset = 6;
      }
      break;

      case SIM_UICC_SERVICE_TYPE_VGCS_AND_VGCSS:
      case SIM_UICC_SERVICE_TYPE_VBS_AND_VBSS:
      case SIM_UICC_SERVICE_TYPE_PSEUDO:
      case SIM_UICC_SERVICE_TYPE_UPLMNWLAN:
      case SIM_UICC_SERVICE_TYPE_OPLMNWLAN:
      case SIM_UICC_SERVICE_TYPE_USER_CONTR_WSID_LIST:
      case SIM_UICC_SERVICE_TYPE_OPERATOR_CONTR_WSID_LIST:
      case SIM_UICC_SERVICE_TYPE_VGCS_SECURITY:
      {
          offset = 7;
      }
      break;

      case SIM_UICC_SERVICE_TYPE_VBS_SECURITY:
      case SIM_UICC_SERVICE_TYPE_WRI:
      case SIM_UICC_SERVICE_TYPE_MMS_STORAGE:
      case SIM_UICC_SERVICE_TYPE_GBA:
      case SIM_UICC_SERVICE_TYPE_MBMS_SECURITY:
      case SIM_UICC_SERVICE_TYPE_DDL_USSD_APPL_MODE:
      case SIM_UICC_SERVICE_TYPE_ADD_TERM_PROF:
      {
          offset = 8;
      }
      break;

      default:
      {
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_read_usim_service_table_service_availability: Service type not supported, service_type = %d", service_type);
        return -2;
      }
    } //switch

    if (offset >= filesize) {
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_read_usim_service_table_service_availability: Offset bigger than actual filesize"
                " offset = %d, filesize = %d, requested service_type = %d",offset, filesize, service_type);
        return -2;
    }

    i = ste_modem_file_read_binary(m,
                                   (uintptr_t)ctrl_p,
                                   uicc_get_app_id(),
                                   file_id,
                                   offset,
                                   length,
                                   file_path);

    if (i != 0) {
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_read_usim_service_table_service_availability: "
                   "ste_modem_file_read_binary failed.");
        return -1;
    }
    catd_log_f(SIM_LOGGING_I, "uicc : EXIT uiccd_read_usim_service_table_service_availability ");
    return i;
}



static int uiccd_read_sim_service_table_service_availability(ste_sim_ctrl_block_t * ctrl_p, sim_uicc_service_type_t service_type, int filesize)
{
    int                 i;
    ste_modem_t       * m;
    int                 file_id, offset, length;
    const char        * file_path;

    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_I, "uicc : uiccd_read_sim_service_table_service_availability: No modem");
        return -1;
    }

    file_path = SERVICE_TABLE_GSM_FILE_PATH;
    file_id = GSM_SERVICE_TABLE_FILE_ID;
    length = 1;

    catd_log_f(SIM_LOGGING_I, "uicc : ENTER uiccd_read_sim_service_table_service_availability: Requested Service type = %d",service_type);

    switch (service_type){

      case SIM_UICC_SERVICE_TYPE_CHV1_DISABLE:
      case SIM_UICC_SERVICE_TYPE_ADN:
      case SIM_UICC_SERVICE_TYPE_FDN:
      case SIM_UICC_SERVICE_TYPE_SMS:
      {
        offset = 0;
      }
      break;

      case SIM_UICC_SERVICE_TYPE_AOC:
      case SIM_UICC_SERVICE_TYPE_CCP:
      case SIM_UICC_SERVICE_TYPE_PLMNSEL:
      {
        offset = 1;
      }
      break;

      case SIM_UICC_SERVICE_TYPE_MSISDN:
      case SIM_UICC_SERVICE_TYPE_EXT_1:
      case SIM_UICC_SERVICE_TYPE_EXT_2:
      case SIM_UICC_SERVICE_TYPE_SMSP:
      {
        offset = 2;
      }
      break;

      case SIM_UICC_SERVICE_TYPE_LND:
      case SIM_UICC_SERVICE_TYPE_CBMI:
      case SIM_UICC_SERVICE_TYPE_GID1:
      case SIM_UICC_SERVICE_TYPE_GID2:
      {
        offset = 3;
      }
      break;

      case SIM_UICC_SERVICE_TYPE_SPN:
      case SIM_UICC_SERVICE_TYPE_SDN:
      case SIM_UICC_SERVICE_TYPE_EXT_3:
      {
        offset = 4;
      }
      break;

      case SIM_UICC_SERVICE_TYPE_VGCS_AND_VGCSS:
      case SIM_UICC_SERVICE_TYPE_VBS_AND_VBSS:
      case SIM_UICC_SERVICE_TYPE_ENH_PRECEDENCE_PREEMPTION:
      case SIM_UICC_SERVICE_TYPE_AAEM:
      {
        offset = 5;
      }
      break;

      case SIM_UICC_SERVICE_TYPE_SMS_CB:
      case SIM_UICC_SERVICE_TYPE_SMS_PP:
      case SIM_UICC_SERVICE_TYPE_MENU_SELECT:
      case SIM_UICC_SERVICE_TYPE_CALL_CONTROL:
      {
        offset = 6;
      }
      break;

      case SIM_UICC_SERVICE_TYPE_PROACTIVE_SIM:
      case SIM_UICC_SERVICE_TYPE_CBMIR:
      case SIM_UICC_SERVICE_TYPE_BDN:
      case SIM_UICC_SERVICE_TYPE_EXT_4:
      {
        offset = 7;
      }
      break;

      case SIM_UICC_SERVICE_TYPE_DCK:
      case SIM_UICC_SERVICE_TYPE_CNL:
      case SIM_UICC_SERVICE_TYPE_SMSR:
      case SIM_UICC_SERVICE_TYPE_NIA:
      {
        offset = 8;
      }
      break;

      case SIM_UICC_SERVICE_TYPE_MO_SMS_CONTR_BY_SIM:
      case SIM_UICC_SERVICE_TYPE_GPRS:
      case SIM_UICC_SERVICE_TYPE_IMG:
      case SIM_UICC_SERVICE_TYPE_SOLSA:
      {
        offset = 9;
      }
      break;

      case SIM_UICC_SERVICE_TYPE_USSD_STR_DATA_SUPPORT:
      case SIM_UICC_SERVICE_TYPE_RUN_AT_COMMAND:
      case SIM_UICC_SERVICE_TYPE_PLMNWACT:
      case SIM_UICC_SERVICE_TYPE_OPLMNWACT:
      {
        offset = 10;
      }
      break;

      case SIM_UICC_SERVICE_TYPE_HPLMNWACT:
      case SIM_UICC_SERVICE_TYPE_CPBCCH:
      case SIM_UICC_SERVICE_TYPE_INV_SCAN:
      case SIM_UICC_SERVICE_TYPE_ECCP:
      {
        offset = 11;
      }
      break;

      case SIM_UICC_SERVICE_TYPE_MEXE:
      case SIM_UICC_SERVICE_TYPE_PNN:
      case SIM_UICC_SERVICE_TYPE_OPL:
      {
        offset = 12;
      }
      break;

      case SIM_UICC_SERVICE_TYPE_MBDN:
      case SIM_UICC_SERVICE_TYPE_MWIS:
      case SIM_UICC_SERVICE_TYPE_CFIS:
      case SIM_UICC_SERVICE_TYPE_SPDI:
      {
        offset = 13;
      }
      break;

      case SIM_UICC_SERVICE_TYPE_MMS:
      case SIM_UICC_SERVICE_TYPE_EXT_8:
      case SIM_UICC_SERVICE_TYPE_MMSUCP:
      {
        offset = 14;
      }
      break;

      default:
      {
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_read_sim_service_table_service_availability: Service type not supported, service_type = %d", service_type);
        return -2;
      }
    } //switch

    if (offset >= filesize) {
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_read_sim_service_table_service_availability: Offset bigger than actual filesize"
                " offset = %d, filesize = %d, requested service_type = %d",offset, filesize, service_type);
        return -2;
    }

    i = ste_modem_file_read_binary(m,
                                   (uintptr_t)ctrl_p,
                                   uicc_get_app_id(),
                                   file_id,
                                   offset,
                                   length,
                                   file_path);

    if (i != 0) {
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_read_sim_service_table_service_availability: "
                   "ste_modem_file_read_binary failed.");
        return -1;
    }
    catd_log_f(SIM_LOGGING_I, "uicc : EXIT uiccd_read_sim_service_table_service_availability ");
    return i;
}



static sim_uicc_service_availability_t uiccd_usim_check_service_availability(sim_uicc_service_type_t service_type, const uint8_t * ef_ust_data)
{
    sim_uicc_service_availability_t   service_status;


    service_status = SIM_UICC_SERVICE_NOT_AVAILABLE;

    switch (service_type){
      case SIM_UICC_SERVICE_TYPE_LOCAL_PHONE_BOOK:
      case SIM_UICC_SERVICE_TYPE_ICI_AND_ICT:
      case SIM_UICC_SERVICE_TYPE_GID1:
      case SIM_UICC_SERVICE_TYPE_AAEM:
      case SIM_UICC_SERVICE_TYPE_MEXE:
      case SIM_UICC_SERVICE_TYPE_CFIS:
      case SIM_UICC_SERVICE_TYPE_VGCS_AND_VGCSS:
      case SIM_UICC_SERVICE_TYPE_VBS_SECURITY:
        // bit 1 indicates service available
        if ((*ef_ust_data & USIM_SERVICE_TYPE_BIT1_INDICATES_AVAILABLE) == USIM_SERVICE_TYPE_BIT1_INDICATES_AVAILABLE)
           service_status = SIM_UICC_SERVICE_AVAILABLE;
      break;

      case SIM_UICC_SERVICE_TYPE_FDN:
      case SIM_UICC_SERVICE_TYPE_SMS:
      case SIM_UICC_SERVICE_TYPE_GID2:
      case SIM_UICC_SERVICE_TYPE_EST:
      case SIM_UICC_SERVICE_TYPE_OPLMNWACT:
      case SIM_UICC_SERVICE_TYPE_VBS_AND_VBSS:
      case SIM_UICC_SERVICE_TYPE_WRI:
        // bit 2 indicates service available
        if ((*ef_ust_data & USIM_SERVICE_TYPE_BIT2_INDICATES_AVAILABLE) == USIM_SERVICE_TYPE_BIT2_INDICATES_AVAILABLE)
           service_status = SIM_UICC_SERVICE_AVAILABLE;
      break;

      case SIM_UICC_SERVICE_TYPE_EXT_2:
      case SIM_UICC_SERVICE_TYPE_SMSR:
      case SIM_UICC_SERVICE_TYPE_SPN:
      case SIM_UICC_SERVICE_TYPE_GSM_ACCESS:
      case SIM_UICC_SERVICE_TYPE_ACL:
      case SIM_UICC_SERVICE_TYPE_HPLMNWACT:
      case SIM_UICC_SERVICE_TYPE_SPDI:
      case SIM_UICC_SERVICE_TYPE_PSEUDO:
      case SIM_UICC_SERVICE_TYPE_MMS_STORAGE:
        // bit 3 indicates service available
        if ((*ef_ust_data & USIM_SERVICE_TYPE_BIT3_INDICATES_AVAILABLE) == USIM_SERVICE_TYPE_BIT3_INDICATES_AVAILABLE)
           service_status = SIM_UICC_SERVICE_AVAILABLE;
      break;

      case SIM_UICC_SERVICE_TYPE_SDN:
      case SIM_UICC_SERVICE_TYPE_SMSP:
      case SIM_UICC_SERVICE_TYPE_PLMNWACT:
      case SIM_UICC_SERVICE_TYPE_SMS_PP:
      case SIM_UICC_SERVICE_TYPE_DCK:
      case SIM_UICC_SERVICE_TYPE_EXT_5:
      case SIM_UICC_SERVICE_TYPE_MMS:
      case SIM_UICC_SERVICE_TYPE_UPLMNWLAN:
      case SIM_UICC_SERVICE_TYPE_GBA:
        // bit 4 indicates service available
        if ((*ef_ust_data & USIM_SERVICE_TYPE_BIT4_INDICATES_AVAILABLE) == USIM_SERVICE_TYPE_BIT4_INDICATES_AVAILABLE)
           service_status = SIM_UICC_SERVICE_AVAILABLE;
      break;

      case SIM_UICC_SERVICE_TYPE_EXT_3:
      case SIM_UICC_SERVICE_TYPE_AOC:
      case SIM_UICC_SERVICE_TYPE_MSISDN:
      case SIM_UICC_SERVICE_TYPE_SMS_CB:
      case SIM_UICC_SERVICE_TYPE_CNL:
      case SIM_UICC_SERVICE_TYPE_PNN:
      case SIM_UICC_SERVICE_TYPE_EXT_8:
      case SIM_UICC_SERVICE_TYPE_OPLMNWLAN:
      case SIM_UICC_SERVICE_TYPE_MBMS_SECURITY:
        // bit 5 indicates service available
        if ((*ef_ust_data & USIM_SERVICE_TYPE_BIT5_INDICATES_AVAILABLE) == USIM_SERVICE_TYPE_BIT5_INDICATES_AVAILABLE)
           service_status = SIM_UICC_SERVICE_AVAILABLE;
      break;

      case SIM_UICC_SERVICE_TYPE_BDN:
      case SIM_UICC_SERVICE_TYPE_CCP:
      case SIM_UICC_SERVICE_TYPE_IMG:
      case SIM_UICC_SERVICE_TYPE_CALL_CONTROL_BY_USIM:
      case SIM_UICC_SERVICE_TYPE_GSM_SECURITY_CONTEXT:
      case SIM_UICC_SERVICE_TYPE_OPL:
      case SIM_UICC_SERVICE_TYPE_CALL_CONTROL_ON_GPRS:
      case SIM_UICC_SERVICE_TYPE_USER_CONTR_WSID_LIST:
      case SIM_UICC_SERVICE_TYPE_DDL_USSD_APPL_MODE:
        // bit 6 indicates service available
        if ((*ef_ust_data & USIM_SERVICE_TYPE_BIT6_INDICATES_AVAILABLE) == USIM_SERVICE_TYPE_BIT6_INDICATES_AVAILABLE)
           service_status = SIM_UICC_SERVICE_AVAILABLE;
      break;

      case SIM_UICC_SERVICE_TYPE_EXT_4:
      case SIM_UICC_SERVICE_TYPE_CBMI:
      case SIM_UICC_SERVICE_TYPE_SOLSA:
      case SIM_UICC_SERVICE_TYPE_MO_SMS_CONTROL_BY_USIM:
      case SIM_UICC_SERVICE_TYPE_CPBCCH:
      case SIM_UICC_SERVICE_TYPE_MBDN:
      case SIM_UICC_SERVICE_TYPE_MMSUCP:
      case SIM_UICC_SERVICE_TYPE_OPERATOR_CONTR_WSID_LIST:
        // bit 7 indicates service available
        if ((*ef_ust_data & USIM_SERVICE_TYPE_BIT7_INDICATES_AVAILABLE) == USIM_SERVICE_TYPE_BIT7_INDICATES_AVAILABLE)
           service_status = SIM_UICC_SERVICE_AVAILABLE;
      break;

      case SIM_UICC_SERVICE_TYPE_OCI_AND_OCT:
      case SIM_UICC_SERVICE_TYPE_CBMIR:
      case SIM_UICC_SERVICE_TYPE_ENH_PRECEDENCE_PREEMPTION:
      case SIM_UICC_SERVICE_TYPE_RUN_AT_COMMAND:
      case SIM_UICC_SERVICE_TYPE_INV_SCAN:
      case SIM_UICC_SERVICE_TYPE_MWIS:
      case SIM_UICC_SERVICE_TYPE_NIA:
      case SIM_UICC_SERVICE_TYPE_VGCS_SECURITY:
      case SIM_UICC_SERVICE_TYPE_ADD_TERM_PROF:
       // bit 8 indicates service available
        if ((*ef_ust_data & USIM_SERVICE_TYPE_BIT8_INDICATES_AVAILABLE) == USIM_SERVICE_TYPE_BIT8_INDICATES_AVAILABLE)
           service_status = SIM_UICC_SERVICE_AVAILABLE;
      break;

      default:
      {
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_usim_check_service_availability: Service type not supported, service_type = %d", service_type);
        service_status = SIM_UICC_SERVICE_AVAILABILITY_UNKNOWN;
      }
    } //switch


    return service_status;
}

static sim_uicc_service_availability_t uiccd_sim_check_service_availability(sim_uicc_service_type_t service_type, const uint8_t * ef_sst_data)
{
    sim_uicc_service_availability_t   service_status;


    service_status = SIM_UICC_SERVICE_NOT_AVAILABLE;

    switch (service_type){
      case SIM_UICC_SERVICE_TYPE_CHV1_DISABLE:
      case SIM_UICC_SERVICE_TYPE_AOC:
      case SIM_UICC_SERVICE_TYPE_MSISDN:
      case SIM_UICC_SERVICE_TYPE_LND:
      case SIM_UICC_SERVICE_TYPE_SPN:
      case SIM_UICC_SERVICE_TYPE_VGCS_AND_VGCSS:
      case SIM_UICC_SERVICE_TYPE_SMS_CB:
      case SIM_UICC_SERVICE_TYPE_PROACTIVE_SIM:
      case SIM_UICC_SERVICE_TYPE_DCK:
      case SIM_UICC_SERVICE_TYPE_MO_SMS_CONTR_BY_SIM:
      case SIM_UICC_SERVICE_TYPE_USSD_STR_DATA_SUPPORT:
      case SIM_UICC_SERVICE_TYPE_HPLMNWACT:
      case SIM_UICC_SERVICE_TYPE_MEXE:
      case SIM_UICC_SERVICE_TYPE_MBDN:
      case SIM_UICC_SERVICE_TYPE_MMS:
        // bit 1 indicates service allocated, bit 2 indicates service activated
        if ((*ef_sst_data & SIM_SERVICE_TYPE_BIT1_BIT2_IND_ACTIVATED_ALLOCATED) == SIM_SERVICE_TYPE_BIT1_BIT2_IND_ACTIVATED_ALLOCATED)
           service_status = SIM_UICC_SERVICE_AVAILABLE;
      break;

      case SIM_UICC_SERVICE_TYPE_ADN:
      case SIM_UICC_SERVICE_TYPE_CCP:
      case SIM_UICC_SERVICE_TYPE_EXT_1:
      case SIM_UICC_SERVICE_TYPE_CBMI:
      case SIM_UICC_SERVICE_TYPE_SDN:
      case SIM_UICC_SERVICE_TYPE_VBS_AND_VBSS:
      case SIM_UICC_SERVICE_TYPE_SMS_PP:
      case SIM_UICC_SERVICE_TYPE_CBMIR:
      case SIM_UICC_SERVICE_TYPE_CNL:
      case SIM_UICC_SERVICE_TYPE_GPRS:
      case SIM_UICC_SERVICE_TYPE_RUN_AT_COMMAND:
      case SIM_UICC_SERVICE_TYPE_CPBCCH:
      case SIM_UICC_SERVICE_TYPE_MWIS:
      case SIM_UICC_SERVICE_TYPE_EXT_8:
        // bit 3 indicates service allocated, bit 4 indicates service activated
        if ((*ef_sst_data & SIM_SERVICE_TYPE_BIT3_BIT4_IND_ACTIVATED_ALLOCATED) == SIM_SERVICE_TYPE_BIT3_BIT4_IND_ACTIVATED_ALLOCATED)
           service_status = SIM_UICC_SERVICE_AVAILABLE;
      break;

      case SIM_UICC_SERVICE_TYPE_FDN:
      case SIM_UICC_SERVICE_TYPE_PLMNSEL:
      case SIM_UICC_SERVICE_TYPE_EXT_2:
      case SIM_UICC_SERVICE_TYPE_GID1:
      case SIM_UICC_SERVICE_TYPE_EXT_3:
      case SIM_UICC_SERVICE_TYPE_ENH_PRECEDENCE_PREEMPTION:
      case SIM_UICC_SERVICE_TYPE_MENU_SELECT:
      case SIM_UICC_SERVICE_TYPE_BDN:
      case SIM_UICC_SERVICE_TYPE_SMSR:
      case SIM_UICC_SERVICE_TYPE_IMG:
      case SIM_UICC_SERVICE_TYPE_PLMNWACT:
      case SIM_UICC_SERVICE_TYPE_INV_SCAN:
      case SIM_UICC_SERVICE_TYPE_PNN:
      case SIM_UICC_SERVICE_TYPE_CFIS:
      case SIM_UICC_SERVICE_TYPE_MMSUCP:
        // bit 5 indicates service allocated, bit 6 indicates service activated
        if ((*ef_sst_data & SIM_SERVICE_TYPE_BIT5_BIT6_IND_ACTIVATED_ALLOCATED) == SIM_SERVICE_TYPE_BIT5_BIT6_IND_ACTIVATED_ALLOCATED)
           service_status = SIM_UICC_SERVICE_AVAILABLE;
      break;

      case SIM_UICC_SERVICE_TYPE_SMS:
      case SIM_UICC_SERVICE_TYPE_SMSP:
      case SIM_UICC_SERVICE_TYPE_GID2:
      case SIM_UICC_SERVICE_TYPE_AAEM:
      case SIM_UICC_SERVICE_TYPE_CALL_CONTROL:
      case SIM_UICC_SERVICE_TYPE_EXT_4:
      case SIM_UICC_SERVICE_TYPE_NIA:
      case SIM_UICC_SERVICE_TYPE_SOLSA:
      case SIM_UICC_SERVICE_TYPE_OPLMNWACT:
      case SIM_UICC_SERVICE_TYPE_ECCP:
      case SIM_UICC_SERVICE_TYPE_OPL:
      case SIM_UICC_SERVICE_TYPE_SPDI:
        // bit 7 indicates service allocated, bit 8 indicates service activated
        if ((*ef_sst_data & SIM_SERVICE_TYPE_BIT7_BIT8_IND_ACTIVATED_ALLOCATED) == SIM_SERVICE_TYPE_BIT7_BIT8_IND_ACTIVATED_ALLOCATED)
           service_status = SIM_UICC_SERVICE_AVAILABLE;
      break;

      default:
      {
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_sim_check_service_availability: Service type not supported, service_type = %d", service_type);
        service_status = SIM_UICC_SERVICE_AVAILABILITY_UNKNOWN;
      }
    } //switch

    return service_status;
}

/**
 * @brief                  Handler function for get service availability responses.
 * @param ste_msg          The original message from client.
 * @return                 Status of the transaction. Transaction is DONE (failure and success): 0;
 *                                                    Transaction to be continued: -1
 *
 * Please read the description for this transaction from the comment for function 'uiccd_main_get_service_availability'.
 * This transaction handler handles the following scenario:
 * GET READ BINARY RESPONSE  -> retrieve the data requested according to the information from client
 * -> return read and service status to client -> transaction is done
 */
static int uiccd_get_service_availability_transaction_handler(ste_msg_t * ste_msg)
{
    ste_sim_ctrl_block_t              * ctrl_p = (ste_sim_ctrl_block_t*)(ste_msg->client_tag);
    uiccd_trans_data_service_table_t  * data_p;
    int                               rv;
    sim_uicc_service_availability_t   service_status = SIM_UICC_SERVICE_AVAILABILITY_UNKNOWN;


    if (ctrl_p == NULL) {
        catd_log_f(SIM_LOGGING_E, "uicc : Exit 1 %s", __func__);
        return 0;
    }
    data_p = (uiccd_trans_data_service_table_t*)(ctrl_p->transaction_data_p);
    if (data_p == NULL) {
        catd_log_f(SIM_LOGGING_E, "uicc : Exit 2 %s", __func__);
        free(ctrl_p);
        return 0;
    }

    catd_log_f(SIM_LOGGING_I, "uicc : Enter %s", __func__);

    switch (data_p->trans_state)
    {
        case UICCD_TRANS_SERVICE_TABLE_GET_FORMAT:
        {
            catd_log_f(SIM_LOGGING_D, "uicc : Transaction state UICCD_MSG_SIM_FILE_GET_FORMAT_RSP");

            uiccd_msg_sim_file_get_format_response_t  * file_format_p = (uiccd_msg_sim_file_get_format_response_t*)ste_msg;

            if (ste_msg->type != UICCD_MSG_SIM_FILE_GET_FORMAT_RSP) {
                catd_log_f(SIM_LOGGING_E, "uicc : wrong msg type for transaction state UICCD_TRANS_SERVICE_TABLE_GET_FORMAT");
                return -1;
            }

            if (uicc_get_app_type() == STE_SIM_APP_TYPE_SIM) {
                // Spcial case
                if (data_p->service_type == SIM_UICC_SERVICE_TYPE_MO_SMS_CONTROL_BY_USIM) {
                    data_p->service_type = SIM_UICC_SERVICE_TYPE_MO_SMS_CONTR_BY_SIM;
                }

                rv = uiccd_read_sim_service_table_service_availability(ctrl_p, data_p->service_type, file_format_p->file_size);
            } else if (uicc_get_app_type() == STE_SIM_APP_TYPE_USIM) {
              // Special case
              if (data_p->service_type == SIM_UICC_SERVICE_TYPE_CALL_CONTROL) {
                  data_p->service_type = SIM_UICC_SERVICE_TYPE_CALL_CONTROL_BY_USIM;
              }

              rv = uiccd_read_usim_service_table_service_availability(ctrl_p, data_p->service_type, file_format_p->file_size);

            }
            else {
              catd_log_f(SIM_LOGGING_E, "uicc : Unexpected app_type");
              rv = -1;
            }

            if (rv != 0) {
                //since this mechanism of calling uiccd_sig_get_service_availability_response to trigg the send back the result to the client is not working,
                //the result is directly sent back here

                if (rv == -2)
                    rv = uiccd_encode_and_send_get_service_availability_response(ctrl_p->fd,
                                                                               ctrl_p->client_tag,
                                                                               SIM_UICC_STATUS_CODE_OK,
                                                                               SIM_UICC_STATUS_CODE_FAIL_DETAILS_SERVICE_NOT_SUPPORTED,
                                                                               SIM_UICC_SERVICE_NOT_AVAILABLE);
                else
                    rv = uiccd_encode_and_send_get_service_availability_response(ctrl_p->fd,
                                                                               ctrl_p->client_tag,
                                                                               SIM_UICC_STATUS_CODE_FAIL,
                                                                               SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                               SIM_UICC_SERVICE_AVAILABILITY_UNKNOWN);

                catd_log_f(SIM_LOGGING_E, "uicc : "
                           "uiccd_main_get_service_availability failed: read service table failed. data_p->service_type = %d", data_p->service_type);
                //transaction must be ended
                free(data_p);
                free(ctrl_p);
                return 0;
            }

            //the transaction is done
            data_p->trans_state = UICCD_TRANS_SERVICE_TABLE_READ_BINARY;
        }
        break;

        case UICCD_TRANS_SERVICE_TABLE_READ_BINARY:
        {
            uiccd_msg_read_sim_file_binary_response_t   * rsp_p;

            catd_log_f(SIM_LOGGING_I, "uicc : Transaction state UICCD_TRANS_SERVICE_TABLE_READ_BINARY");

            if (ste_msg->type != UICCD_MSG_READ_SIM_FILE_BINARY_RSP) {
                catd_log_f(SIM_LOGGING_E, "uicc : wrong msg type for transaction state UICCD_TRANS_SERVICE_TABLE_READ_BINARY");
                return -1;
            }

            rsp_p = (uiccd_msg_read_sim_file_binary_response_t*)ste_msg;

            if (rsp_p->uicc_status_code != SIM_UICC_STATUS_CODE_OK) {
               catd_log_f(SIM_LOGGING_E, "uicc : read of requested data failed");

               if (rsp_p->uicc_status_code_fail_details == SIM_UICC_STATUS_CODE_FAIL_DETAILS_INVALID_PARAMETERS) {
                 catd_log_f(SIM_LOGGING_E, "uicc : requested byte not possible to read, i.e. service is not supported. data_p->service_type = %d", data_p->service_type);
                 rv = uiccd_encode_and_send_get_service_availability_response(ctrl_p->fd,
                                                                              ctrl_p->client_tag,
                                                                              rsp_p->uicc_status_code,
                                                                              SIM_UICC_STATUS_CODE_FAIL_DETAILS_SERVICE_NOT_SUPPORTED,
                                                                              SIM_UICC_SERVICE_NOT_AVAILABLE);
                }
                else
                  rv = uiccd_encode_and_send_get_service_availability_response(ctrl_p->fd,
                                                                               ctrl_p->client_tag,
                                                                               rsp_p->uicc_status_code,
                                                                               rsp_p->uicc_status_code_fail_details,
                                                                               SIM_UICC_SERVICE_AVAILABILITY_UNKNOWN);




                free(data_p);
                free(ctrl_p);
                return 0;
            }

            if (rsp_p->len != 1) {
                catd_log_f(SIM_LOGGING_E, "uicc : one byte only expected from service table");
                rv = uiccd_encode_and_send_get_service_availability_response(ctrl_p->fd,
                                                                             ctrl_p->client_tag,
                                                                             SIM_UICC_STATUS_CODE_FAIL,
                                                                             SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                             SIM_UICC_SERVICE_AVAILABILITY_UNKNOWN);
                free(data_p);
                free(ctrl_p);
                return 0;
            }

            if (uicc_get_app_type() == STE_SIM_APP_TYPE_SIM) {
              service_status = uiccd_sim_check_service_availability(data_p->service_type, rsp_p->data);
             }
             else if (uicc_get_app_type() == STE_SIM_APP_TYPE_USIM) {
              service_status = uiccd_usim_check_service_availability(data_p->service_type, rsp_p->data);
            }
            else {
              catd_log_f(SIM_LOGGING_E, "uicc : uiccd_get_service_availability: Unexpected app_type");
              rv = uiccd_encode_and_send_get_service_availability_response(ctrl_p->fd,
                                                                    ctrl_p->client_tag,
                                                                    SIM_UICC_STATUS_CODE_FAIL,
                                                                    SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                    SIM_UICC_SERVICE_AVAILABILITY_UNKNOWN);
              free(data_p);
              free(ctrl_p);
              return 0;
            }

            //send the result back to client
            rv = uiccd_encode_and_send_get_service_availability_response(ctrl_p->fd,
                                                                  ctrl_p->client_tag,
                                                                  rsp_p->uicc_status_code,
                                                                  rsp_p->uicc_status_code_fail_details,
                                                                  service_status);

            //the transaction is done
            data_p->trans_state = UICCD_TRANS_SERVICE_TABLE_INIT;

            free(data_p);
            free(ctrl_p);
            return 0;
        }
        break;

        default:
        {
            catd_log_f(SIM_LOGGING_E, "uicc : wrong transaction state for get service availability");
            free(data_p);
            free(ctrl_p);
            return 0;
        }
    }
    return -1;
}

/**
 * @brief                  Main function for get availability information from the service table.
 * @param ste_msg          The original message from client.
 * @return                 Status of the operation. Success: 0; Failed: -1
 *
 * The availability status for a service is indicated by specific bit in the service table. In USIM the service table is in file EF-UST (USIM Service Table)
 * and in GSM in file EF-SST (SIM Service Table). The value of this bit is returned to the client.
 *
 * The transaction implementation includes the handler function for the responses from modem, the user data(named transaction data)
 * needed for the transaction including transaction state, etc., and the transaction id, which is exactly the same as the
 * message type from client. In this way, when a response(for example, read record resp) is received from modem, the response function
 * will check if the transaction id is the same as the resposne message type, if it is the same, then it goes through the old
 * response function. Otherwise it will go through the transaction handler function.
 *
 * For get service availability information the transaction includes a  call to read_binary. Corresponding transaction
 * state is named read_binary and handles the responses from modem. Before sending the first request
 * to the modem, a ctrl block to save all the transaction related data is created.
 * The typical state transition for this  transation is :
 * READ BINARY -> DONE
 */
int uiccd_main_get_service_availability(ste_msg_t * ste_msg)
{
    ste_modem_t                          * m;
    uiccd_msg_get_service_availability_t * msg = (uiccd_msg_get_service_availability_t *) ste_msg;
    ste_sim_ctrl_block_t                 * ctrl_p;
    int                                    i = -1;
    uiccd_trans_data_service_table_t     * data_p;

    const char *p = msg->data;
    const char *p_max = msg->data + msg->len;
    sim_uicc_service_type_t service_type;


    catd_log_f(SIM_LOGGING_I, "uicc : GET service availability message received from: %d ",msg->fd);

    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        //since this mechanism of calling uiccd_sig_get_service_availability_response to trigg the send back the result to the client is not working,
        //the result is directly sent back here
        i = uiccd_encode_and_send_get_service_availability_response(msg->fd,
                                                                    msg->client_tag,
                                                                    SIM_UICC_STATUS_CODE_FAIL,
                                                                    SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                    SIM_UICC_SERVICE_AVAILABILITY_UNKNOWN);
        return -1;
    }

    data_p = malloc(sizeof(uiccd_trans_data_service_table_t));
    if (!data_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : memory allocation failed");
        //since this mechanism of calling uiccd_sig_get_service_availability_response to trigg the send back the result to the client is not working,
        //the result is directly sent back here
        i = uiccd_encode_and_send_get_service_availability_response(msg->fd,
                                                                    msg->client_tag,
                                                                    SIM_UICC_STATUS_CODE_FAIL,
                                                                    SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                    SIM_UICC_SERVICE_AVAILABILITY_UNKNOWN);
        return -1;
    }
    memset(data_p, 0, sizeof(uiccd_trans_data_service_table_t));

    //decode the data from client
    p = sim_dec(p, &service_type, sizeof(service_type), p_max);

    if ( !p ) {
        // Something went wrong in the sim_dec above.
        catd_log_f(SIM_LOGGING_E, "uicc : create ctrl block for get service availability transaction failed decoding msg data");
        //since this mechanism of calling uiccd_sig_get_service_availability_response to trigg the send back the result to the client is not working,
        //the result is directly sent back here
        i = uiccd_encode_and_send_get_service_availability_response(msg->fd,
                                                                    msg->client_tag,
                                                                    SIM_UICC_STATUS_CODE_FAIL,
                                                                    SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                    SIM_UICC_SERVICE_AVAILABILITY_UNKNOWN);
        free(data_p);
        return -1;
    }

    //save the data from client
    data_p->service_type = service_type;
    data_p->trans_state = UICCD_TRANS_SERVICE_TABLE_GET_FORMAT;

    ctrl_p = ste_sim_create_ctrl_block(msg->client_tag, msg->fd, msg->type, uiccd_get_service_availability_transaction_handler, data_p);

    catd_log_f(SIM_LOGGING_I, "uicc : uiccd_main_get_service_availability: service_type = %d", service_type);
    if (!ctrl_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : create ctrl block for get service availability transaction failed");
        //since this mechanism of calling uiccd_sig_get_service_availability_response to trigg the send back the result to the client is not working,
        //the result is directly sent back here
        i = uiccd_encode_and_send_get_service_availability_response(msg->fd,
                                                                    msg->client_tag,
                                                                    SIM_UICC_STATUS_CODE_FAIL,
                                                                    SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                    SIM_UICC_SERVICE_AVAILABILITY_UNKNOWN);
        free(data_p);
        return -1;
    }

    if (uicc_get_app_type() == STE_SIM_APP_TYPE_SIM) {
        catd_log_f(SIM_LOGGING_D, "uicc : SIM application, read SST file format");
        i = ste_modem_sim_file_get_format(m,
                                          (uintptr_t)ctrl_p,
                                          uicc_get_app_id(),
                                          GSM_SERVICE_TABLE_FILE_ID,
                                          SERVICE_TABLE_GSM_FILE_PATH);
    } else if (uicc_get_app_type() == STE_SIM_APP_TYPE_USIM){
        catd_log_f(SIM_LOGGING_D, "uicc : USIM application, read UST file format");
        i = ste_modem_sim_file_get_format(m,
                                          (uintptr_t)ctrl_p,
                                          uicc_get_app_id(),
                                          USIM_SERVICE_TABLE_UST_FILE_ID,
                                          SERVICE_TABLE_UST_USIM_PATH);
    }


    if (i != 0) {
        i = uiccd_encode_and_send_get_service_availability_response(msg->fd,
                                                                    msg->client_tag,
                                                                    SIM_UICC_STATUS_CODE_FAIL,
                                                                    SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                    SIM_UICC_SERVICE_AVAILABILITY_UNKNOWN);

        catd_log_f(SIM_LOGGING_E, "uicc : "
                   "uiccd_main_get_service_availability failed: get ust/sst file format");
        //transaction must be ended
        free(data_p);
        free(ctrl_p);
        return -1;
    }

    return 0;
}
