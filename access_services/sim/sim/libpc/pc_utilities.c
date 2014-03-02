/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : pc_utilities.c
 * Description     : common functions used by libpc.
 *
 * Author          : Haiyuan Bu <haiyuan.bu@stericsson.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>

#include "sim_internal.h"
#include "catd.h"
#include "apdu.h"
#include "sim_unused.h"
#include "func_trace.h"
#include "pc_utilities.h"


/*************************************************************************
 * @brief:    create and send the general terminal response
 * @params:
 *            parsed_apdu_p:     the parsed apdu structure.
 *            general_result:    general result of the operation.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
void pc_send_terminal_response_general(ste_parsed_apdu_t         * parsed_apdu_p,
                                       ste_apdu_general_result_t   general_result)
{
    ste_sat_apdu_error_t    rv;
    ste_command_result_t    result;
    ste_apdu_t           *  tr_apdu_p = NULL;

    result.general_result = general_result;
    result.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
    result.additional_info_size = 0;
    result.additional_info_p = NULL;
    result.other_data_type = STE_CMD_RESULT_NOTHING;

    rv = ste_apdu_terminal_response(parsed_apdu_p, &result, &tr_apdu_p);
    if (rv != STE_SAT_APDU_ERROR_NONE)
    {
        catd_log_f(SIM_LOGGING_E, "catd : failed to create terminal response.");
        ste_apdu_delete(tr_apdu_p);
        return;
    }
    catd_sig_tr(CATD_FD, tr_apdu_p, CATD_CLIENT_TAG);
    ste_apdu_delete(tr_apdu_p);
}


/*************************************************************************
 * @brief:    create and send the general terminal response with add info
 * @params:
 *            parsed_apdu_p:     the parsed apdu structure.
 *            general_result:    general result of the operation.
 *            add_info_p:        additional info to be added in TR
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
void pc_send_terminal_response_with_add_info(ste_parsed_apdu_t          * parsed_apdu_p,
                                             ste_apdu_general_result_t    general_result,
                                             ste_apdu_additional_info_t * add_info_p)
{
    ste_sat_apdu_error_t    rv;
    ste_command_result_t    result;
    ste_apdu_t           *  tr_apdu_p = NULL;

    result.general_result = general_result;
    result.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
    if (add_info_p) {
        result.additional_info_size = 1;
        result.additional_info_p = (uint8_t*)add_info_p;
    }
    else {
        result.additional_info_size = 0;
        result.additional_info_p = NULL;
    }

    result.other_data_type = STE_CMD_RESULT_NOTHING;

    rv = ste_apdu_terminal_response(parsed_apdu_p, &result, &tr_apdu_p);
    if (rv != STE_SAT_APDU_ERROR_NONE) {
        catd_log_f(SIM_LOGGING_E, "catd : failed to create terminal response.");
        ste_apdu_delete(tr_apdu_p);
        return;
    }
    catd_sig_tr(CATD_FD, tr_apdu_p, CATD_CLIENT_TAG);
    ste_apdu_delete(tr_apdu_p);
}

/*************************************************************************
 * @brief:    create and send the terminal response with error info
 * @params:
 *            error_info:     returned error from apdu.
 *            apdu_p:         original apdu data from card.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
void pc_send_terminal_response_error(ste_sat_apdu_error_t  error_info,
                                     ste_apdu_t           *apdu_p)
{
    ste_apdu_t           *  tr_apdu_p;

    tr_apdu_p = ste_apdu_tr_general_error(error_info, apdu_p);
    if (!tr_apdu_p)
    {
        catd_log_f(SIM_LOGGING_E, "catd : not able to create terminal response.");
        return;
    }

    catd_sig_tr(CATD_FD, tr_apdu_p, CATD_CLIENT_TAG);
    ste_apdu_delete(tr_apdu_p);
}

ste_apdu_location_info_t *create_local_info_from_cell_info(cn_cell_info_t *cell_info_p) {

    ste_apdu_location_info_t *location_info_p = malloc(sizeof(ste_apdu_location_info_t));
    if (location_info_p == NULL) {
        return NULL;
    }

    //Convert PLMN
    create_bcd_from_mcc_mnc((uint8_t*)cell_info_p->mcc_mnc, (uint8_t*) &location_info_p->net_info.PLMN);

    //Convert LAC
    uint8_t *lac = (uint8_t*) &location_info_p->net_info.LAC;
    *lac++ = (uint8_t) (cell_info_p->current_ac >> 8) & 0xFF;
    *lac = (uint8_t) cell_info_p->current_ac & 0xFF;

    //Convert CID
    location_info_p->cell_info.cid = cell_info_p->current_cell_id;

    switch (cell_info_p->rat) {
    case CN_CELL_RAT_WCDMA:
        location_info_p->cell_info.UseExtended = TRUE;
        break;
    default:
        location_info_p->cell_info.UseExtended = FALSE;
        break;
    }

    return location_info_p;
}

uint8_t convert_GSM8bit_2_GSM7bit(uint8_t *In_p, uint8_t *Out_p, uint8_t NoOfOctetsToPack)
{
    uint8_t NoOfOctetsOfPackedData = 0;

    if (NoOfOctetsToPack > 0) {
        uint8_t *Stop_p = NULL;
        uint8_t shift = 0;

        *Out_p = 0;
        Stop_p = In_p + NoOfOctetsToPack;

        do {
            *Out_p = (*In_p++ >> shift);

            if (In_p == Stop_p) {
                break;
            }

            *Out_p = *Out_p | (*In_p << (7 - shift));
            Out_p++;

            shift++;
            shift = (shift % 7);

            if (shift == 0) {
                In_p++;
            }
        } while (In_p < Stop_p);

        NoOfOctetsOfPackedData = (NoOfOctetsToPack - NoOfOctetsToPack / 8);
    }

    return NoOfOctetsOfPackedData;
}

int convert_bcd2ascii(uint8_t* in, size_t size, char* out, size_t maxlen)
{
    char translate[] = "0123456789*#pDEF"; // Digits D & E are not valid, F is end marker.
    int  len         = -1;

    assert(in);
    assert(out);
    assert(maxlen > 0);

    if (out && maxlen >= (2*size + 1)) {
        uint8_t i;
        for (len = 0, i = 0; i < size; i++) {
            char byte = in[i];

            // First digit in low nibble
            uint8_t bcd = byte & 0x0F;
            if (bcd == 0x0F) {
                break;
            } else if (bcd < 0x0D) {
                (out)[len++] = translate[bcd];
            }

            // Second digit in high nibble
            bcd = (byte >> 4) & 0x0F;
            if (bcd == 0x0F) {
                break;
            } else if (bcd < 0x0D) {
                (out)[len++] = translate[bcd];
            }
        }
        (out)[len] = 0; // Null terminate, just in case...
    }
    return len;
}
