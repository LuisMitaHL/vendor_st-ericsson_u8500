/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdint.h>
#include "sim_internal.h"
#include "catd_modem_types.h"
#include "uicc_internal.h"
#include "simd.h"
#include "uiccd_msg.h"
#include "uicc_file_plmn_common.h"
#include "uicc_file_paths.h"
#include "uicc_state_machine.h"

int uiccd_get_plmn_format(ste_sim_ctrl_block_t * ctrl_p, const int file_id)
{
    int          i;
    ste_modem_t *m;
    const char  *file_path;

    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        return -1;
    }

    if (uicc_get_card_type() == STE_UICC_CARD_TYPE_ICC) {
        file_path = PLMN_GSM_PATH;
    } else {
        file_path = PLMN_USIM_PATH;
    }

    i = ste_modem_sim_file_get_format(m, (uintptr_t) ctrl_p, uicc_get_app_id(), file_id,
            file_path);

    if (i != 0) {
        catd_log_f(SIM_LOGGING_E, "uicc : "
            "ste_modem_sim_file_get_format failed.");
        return -1;
    }
    return i;
}

int uiccd_send_read_binary_plmn_request(ste_sim_ctrl_block_t *ctrl_p, size_t file_size, const int file_id)
{
    const char *file_path_p;
    int rv;
    ste_modem_t *m = catd_get_modem();

    if (uicc_get_card_type() == STE_UICC_CARD_TYPE_ICC) {
        file_path_p = PLMN_GSM_PATH;
    } else {
        file_path_p = PLMN_USIM_PATH;
    }

    rv = ste_modem_file_read_binary(m,
                                    (uintptr_t) ctrl_p,
                                    uicc_get_app_id(),
                                    file_id,
                                    0,
                                    file_size,
                                    file_path_p);
    return rv;
}

boolean uiccd_valid_plmn(const ste_uicc_sim_plmn_t * const i_p)
{
    return (
        (i_p->MCC1 <= 9) &&
        (i_p->MCC2 <= 9) &&
        (i_p->MCC3 <= 9) &&
        (i_p->MNC3 <= 9 || i_p->MNC3 == 0xf) &&
        (i_p->MNC1 <= 9) &&
        (i_p->MNC2 <= 9));
}

int uiccd_get_first_free_plmn_index(uiccd_msg_read_sim_file_binary_response_t *read_response_p,
                                    int *index,
                                    ste_uicc_sim_plmn_file_id_t file_id) {
    uint8_t nbr_of_elements;
    uint8_t *data_p;
    ste_sim_plmn_with_AcT_t *plmn_with_AcT_p;
    int i, file_id_offset;

    file_id_offset = (file_id == SIM_FILE_ID_PLMN_WACT) ? SIM_PLMN_WITH_ACT_SIZE : SIM_PLMN_SIZE;
    if (NULL == read_response_p) {
        return -1;
    }
    if (0 != (read_response_p->len % file_id_offset)) {
        return -1;
    }
    plmn_with_AcT_p = calloc(1, sizeof(ste_sim_plmn_with_AcT_t));
    if (NULL == plmn_with_AcT_p) {
        return -1;
    }

    data_p = read_response_p->data;
    nbr_of_elements = read_response_p->len / file_id_offset;
    for (i = 0; i < nbr_of_elements; i++) {

        ste_sim_decode_plmn_with_AcT(plmn_with_AcT_p, (unsigned char*) data_p);

        if (!uiccd_valid_plmn(&plmn_with_AcT_p->PLMN)) {
            // found the next free index
            *index = i;
            free(plmn_with_AcT_p);
            return 1;

        }
        data_p += file_id_offset;
    }
    catd_log_f(SIM_LOGGING_E, "uicc : No free index found");
    free(plmn_with_AcT_p);
    return -1;
}

/*************************************************************************
 * Function:      uiccd_set_access_technology
 *
 * Description:   Access technology as 2 bytes encoded for writing to SIM.
 *
 * Input Params:  access_tech   PLMN access technology
 *
 * Output Params: access_tech_bytes_p Pointer to access technology bytes encoded
 *                                  for writing to SIM
 *
 * Return:        None.
 *
 *************************************************************************/
void uiccd_set_access_technology(const ste_uicc_sim_plmn_AccessTechnology_t access_tech,
                                  uint8_t * const access_tech_bytes_p)
{ /* SetAccessTechnology */
    access_tech_bytes_p[0] = (uint8_t) access_tech;
    access_tech_bytes_p[1] = (uint8_t)(access_tech >> 8);
}
