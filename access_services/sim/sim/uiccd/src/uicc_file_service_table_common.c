/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdint.h>
#include "sim_internal.h"
#include "catd_modem_types.h"
#include "catd_modem.h"
#include "uiccd.h"
#include "uicc_file_paths.h"
#include "uicc_state_machine.h"

int uiccd_get_ef_adn_file_information(ste_sim_ctrl_block_t * ctrl_p)
{
    int                 i;
    ste_modem_t       * m;
    int                 file_id;
    const char        * file_path;
    sim_uicc_get_file_info_type_t type;


    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        return -1;
    }

    file_path = ADN_FILE_PATH;
    file_id = GSM_EF_ADN_FILE_ID;
    type = SIM_UICC_GET_FILE_INFO_TYPE_EF;

    i = ste_modem_get_file_information(m,
                                       (uintptr_t)ctrl_p,
                                       uicc_get_app_id(),
                                       file_id,
                                       file_path,
                                       type);

    if (i != 0) {
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_get_ef_adn_file_information: "
                   "ste_modem_get_file_information failed.");
        return -1;
    }
    return i;
}

int uiccd_read_service_table_service_status(ste_sim_ctrl_block_t * ctrl_p, sim_uicc_service_type_t service_type)
{
    int                 i;
    ste_modem_t       * m;
    int                 file_id, offset, length;
    const char        * file_path;

    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_I, "uicc : uiccd_read_service_table_service_status: No modem");
        return -1;
    }

    if (uicc_get_app_type() == STE_SIM_APP_TYPE_SIM) {
      file_path = SERVICE_TABLE_GSM_FILE_PATH;
      file_id = GSM_SERVICE_TABLE_FILE_ID;

      switch (service_type){
        case SIM_UICC_SERVICE_TYPE_FDN:
        {
          offset = 0;
          length = 1;
        }
        break;

        default:
        {
          catd_log_f(SIM_LOGGING_E, "uicc : uiccd_read_service_table_service_status: Service type not supported, service_type = %d", service_type);
          return -1;
        }
      } //switch
    }
    else if (uicc_get_app_type() == STE_SIM_APP_TYPE_USIM){
      switch (service_type){
        case SIM_UICC_SERVICE_TYPE_FDN:
        case SIM_UICC_SERVICE_TYPE_BDN:
        case SIM_UICC_SERVICE_TYPE_ACL:
        {
          file_path = SERVICE_TABLE_EST_USIM_PATH;
          file_id = USIM_SERVICE_TABLE_EST_FILE_ID;

          offset = 0;
          length = 1;
        }
        break;

        default:
        {
          catd_log_f(SIM_LOGGING_E, "uicc : uiccd_read_service_table_service_status: Service type not supported, service_type = %d", service_type);
          return -2;
        }
      } //switch
    }
    else {
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_read_service_table_service_status: Unexpected app_type");
        return -1;
    }

    i = ste_modem_file_read_binary(m,
                                   (uintptr_t)ctrl_p,
                                   uicc_get_app_id(),
                                   file_id,
                                   offset,
                                   length,
                                   file_path);

    if (i != 0) {
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_read_service_table_service_status: "
                   "ste_modem_file_read_binary failed.");
        return -1;
    }
    return i;
}
