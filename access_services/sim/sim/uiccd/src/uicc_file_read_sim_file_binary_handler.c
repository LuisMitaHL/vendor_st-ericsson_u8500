/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>
#include "sim_internal.h"
#include "catd_modem_types.h"
#include "uicc_internal.h"
#include "simd.h"
#include "uiccd_msg.h"
#include "uicc_file_smsc_common.h"
#include "uicc_state_machine.h"

#ifdef SIM_MNC_LENGTH_CHECK
#include "uicc_file_paths.h"
#include "uicc_file_ad_common.h"
#endif

static int uiccd_encode_and_send_read_sim_file_binary_response(int fd,
                                                               uintptr_t client_tag,
                                                               sim_uicc_status_code_t uicc_status_code,
                                                               sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                               sim_uicc_status_word_t status_word,
                                                               const uint8_t  *data,
                                                               size_t  data_len)
{
    char                    *buf_p = NULL;
    char                    *p = NULL;
    size_t                   buf_len = 0;

    buf_len = sizeof(uicc_status_code);
    buf_len += sizeof(uicc_status_code_fail_details);
    buf_len += sizeof(status_word.sw1) + sizeof(status_word.sw2);
    buf_len += sizeof(uint8_t)*data_len;
    buf_len += sizeof(data_len);

    buf_p = malloc(buf_len);
    if (buf_p == NULL){
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_read_sim_file_binary_response, memory allocation failed");
        return -1;
    }
    p = buf_p;

    p = sim_enc(p, &uicc_status_code, sizeof(uicc_status_code));
    p = sim_enc(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details));
    p = sim_enc(p, &status_word.sw1, sizeof(status_word.sw1));
    p = sim_enc(p, &status_word.sw2, sizeof(status_word.sw2));
    p = sim_enc(p, &data_len, sizeof(data_len));
    p = sim_enc(p, data, sizeof(uint8_t)*(data_len));

    if (buf_len != (size_t)(p-buf_p)) {
        free(buf_p);
        return -1;
    }

    sim_send_generic(fd,
                     STE_UICC_RSP_READ_SIM_FILE_BINARY,
                     buf_p,
                     buf_len,
                     client_tag);

    free(buf_p);

    return 0;
}

#ifdef SIM_MNC_LENGTH_CHECK

/*
 *  Scan the plmn list in the file defined by plmn_static_operator_list_p and search for any of the combinations
 *  sim_mcc/sim_mnc_2digits or sim_mcc/sim_mnc_3digits. Return the length of mnc in the combination found.
 */
static int uiccd_scan_plmn_list_file(uint16_t sim_mcc, uint16_t sim_mnc_2digits, uint16_t sim_mnc_3digits, uint8_t *mnc_length)
{
    int                 result = -2;
    int                 errnum, lineno, mandatory, pos;
    int                 header_valid = 0;
    char                line[PLMN_LIST_MAX_LINE_LEN];
    int                 param_order[PLMN_LIST_COLUMN_MNC+1] = { PLMN_LIST_COLUMN_UNKNOWN };
    unsigned int        param_no;
    long                mcc, mnc;
    uint16_t            mnc_comp;
    int                 mcc_correct = 1;
    const char          *filepath_p;
    FILE                *file_p = NULL;

    filepath_p = plmn_static_operator_list_p;

    catd_log_f(SIM_LOGGING_I, "Start PLMN list scanning, filepath=\"%s\"", filepath_p);

    file_p = fopen(filepath_p, "r");

    if (!file_p) {
        errnum = errno;
        catd_log_f(SIM_LOGGING_E,"uiccd_scan_plmn_list_file: Error %d (%s) when opening \"%s\" for reading!",
                 errnum, strerror(errnum), filepath_p);
        result = -1;
        goto exit;
    }


    mcc = 0;
    mnc = -1;
    mnc_comp = -1;

    lineno = 0;

    while (fgets(line, sizeof(line), file_p) != NULL) {
        lineno++;
        pos = 0;

        /* Handle comment lines (ignore) */
        if (line[0] == '#') {
            continue;
        }

        /* Parse header-line */
        if (strncasecmp(str_header_p, &line[pos], strlen(str_header_p)) == 0) {
            pos += strlen(str_header_p);

            /* Extract column names from header-line */
            header_valid = 0;
            mandatory = 0;

            for (param_no = 0;
                    param_no < (sizeof(param_order) / sizeof(int)) && line[pos] != '\0';
                    param_no++) {

                /* Locate beginning of column name */
                while ((pos<PLMN_LIST_MAX_LINE_LEN) && line[pos] != '\0' && !isalpha(line[pos])) {
                    pos++;
                }

                if (line[pos] == '\0') {
                    /* End of line reached */
                    break;
                } else if (strncasecmp(str_mcc_p, &line[pos], strlen(str_mcc_p)) == 0) {
                    mandatory |= 0x1 << PLMN_LIST_COLUMN_MCC;
                    param_order[param_no] = PLMN_LIST_COLUMN_MCC;
                    pos += strlen(str_mcc_p);
                } else if (strncasecmp(str_mnc_p, &line[pos], strlen(str_mnc_p)) == 0) {
                    mandatory |= 0x1 << PLMN_LIST_COLUMN_MNC;
                    param_order[param_no] = PLMN_LIST_COLUMN_MNC;
                    pos += strlen(str_mnc_p);
                }  else {
                    param_order[param_no] = PLMN_LIST_COLUMN_UNKNOWN;
                    catd_log_f(SIM_LOGGING_E,"uiccd_scan_plmn_list_file: Unrecognized column name \"%s\" on line %d, pos %d!", &line[pos], lineno, pos);

                    while ((pos<PLMN_LIST_MAX_LINE_LEN) && line[pos] != '\0' && isalpha(line[pos])) {
                        pos++;
                    }
                }
            }

            /* Check that all mandatory columns are present */
            if ((mandatory & mandatory_mask) != mandatory_mask) {
                catd_log_f(SIM_LOGGING_E,"uiccd_scan_plmn_list_file: Header on line %d, one or more mandatory columns missing! Aborting operation.", lineno);
                result = -1;
                break;
            }

            header_valid = 1;
            continue;
        }

        /* Don't try to parse line if there is no valid header */
        if (!header_valid) {
            continue;
        }

        /* Ignore empty lines */
        while ((pos<PLMN_LIST_MAX_LINE_LEN) && line[pos] != '\0' && isspace(line[pos])) {
            pos++;
        }

        if (line[pos] == '\0') {
            continue;
        }

        /* Parse operator line */
        errnum = 0;
        mcc_correct = 1;
        mcc = 0;
        mnc = -1;
        mnc_comp = -1;

        for (param_no = 0;
                param_no < sizeof(param_order) / sizeof(int) && mcc_correct && !errnum;
                param_no++) {

            /* Locate beginning of next parameter value (digit or double-quote) */
            while (line[pos] != '\0' && !isdigit(line[pos])) {
                pos++;
            }

            switch (param_order[param_no]) {
            case PLMN_LIST_COLUMN_MCC: {
                /* Country-code */
                char *end_p;
                mcc = strtol(&line[pos], &end_p, 0);
                pos = end_p - line;

                errnum = errno;

                if (!errnum && (mcc < PLMN_MCC_MIN || mcc > PLMN_MCC_MAX)) {
                    catd_log_f(SIM_LOGGING_E, "uiccd_scan_plmn_list_file: Error reading MCC");
                    errnum = ERANGE;
                }

                if (!errnum)
                {
                   if (sim_mcc != (uint16_t)mcc)
                   {
                     /* MCC from sim differs from MCC read from list. Continue to the next line in the list. */
                     catd_log_f(SIM_LOGGING_I, "uiccd_scan_plmn_list_file: MCC not matching, (uint16_t)mcc = 0x%3x", (uint16_t)mcc);
                     mcc_correct = 0;
                     continue;
                   }
                   else if ((mnc_comp >= PLMN_MNC_MIN)&&(mnc_comp <= PLMN_MNC_MAX))
                   {
                     /* In case the column order in the file (incorrectly) is MNC first. */
                     if (mnc_comp == sim_mnc_2digits)
                     {
                       *mnc_length = 2;
                       goto mnc_length_found;
                     }
                     else if (mnc_comp == sim_mnc_3digits)
                     {
                       *mnc_length = 3;
                       goto mnc_length_found;
                     }
                   }
                }

                else if (errnum == EINVAL || errnum == ERANGE)
                {
                  catd_log_f(SIM_LOGGING_E,"uiccd_scan_plmn_list_file: Line %d, error %d (%s) when converting country-code (MCC)!",
                             lineno, errnum, strerror(errnum));
                }

                break;
            }

            case PLMN_LIST_COLUMN_MNC: {
                /* Network-code */
                char *end_p;
                int  mnc_str_len;

                mnc = strtol(&line[pos], &end_p, 0);
                mnc_str_len = strlen(&line[pos]);
                pos = end_p - line;

                errnum = errno;

                if (!errnum && (mnc < PLMN_MNC_MIN || mnc > PLMN_MNC_MAX)) {
                    catd_log_f(SIM_LOGGING_E, "uiccd_scan_plmn_list_file: Error reading MNC");
                    errnum = ERANGE;
                }

                if (!errnum)
                {
                   /* A MNC of 2 digits length is maximum 5 characters long in the line (mnc_str_len. */
                   /* The not used nibble in a two digit long MNC shall be set to 0xF.                */
                   mnc_comp = (uint16_t)mnc;

                   if (mnc_str_len <= PLMN_LIST_MNC_2_MAX_STR_LEN)
                   {
                      mnc_comp |= 0xF00;
                   }

                   if ((sim_mcc == (uint16_t)mcc) && (mnc_comp == sim_mnc_2digits))
                   {
                     *mnc_length = 2;
                     goto mnc_length_found;
                   }
                   else if ((sim_mcc == (uint16_t)mcc) && (mnc_comp == sim_mnc_3digits))
                   {
                     *mnc_length = 3;
                     goto mnc_length_found;
                   }
                }
                else if (errnum == EINVAL || errnum == ERANGE)
                {
                   catd_log_f(SIM_LOGGING_E,"uiccd_scan_plmn_list_file: Line %d, error %d (%s) when converting network-code (MNC)!",
                              lineno, errnum, strerror(errnum));
                }

                break;
            }

            default:
                break;
            }
        }
    }

mnc_length_found:
    catd_log_f(SIM_LOGGING_I,"uiccd_scan_plmn_list_file: PLMN found: MCC=0x%3x, MNC=0x%3x, MNC length=%d!", mcc, mnc, *mnc_length);
    result = 0;
    fclose(file_p);

exit:
    return result;
}




int uiccd_read_imsi_number(ste_sim_ctrl_block_t * ctrl_p)
{
    int                 i;
    ste_modem_t       * m;
    int                 file_id, offset, length;
    const char        * file_path;

    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_I, "uicc : uiccd_read_imsi_number: No modem");
        return -1;
    }

    file_id = EF_IMSI_FILE_ID;

    if (uicc_get_app_type() == STE_SIM_APP_TYPE_SIM) {
      file_path = EF_IMSI_GSM_PATH;
    }
    else if (uicc_get_app_type() == STE_SIM_APP_TYPE_USIM){
      file_path = EF_IMSI_USIM_PATH;
    }
    else {
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_read_imsi_number: Unexpected app_type");
        return -1;
    }

    /* Read full IMSI number including length */
    offset = 0;
    length = 0;

    i = ste_modem_file_read_binary(m,
                                   (uintptr_t)ctrl_p,
                                   uicc_get_app_id(),
                                   file_id,
                                   offset,
                                   length,
                                   file_path);

    if (i != 0) {
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_read_imsi_number: "
                   "ste_modem_file_read_binary failed.");
        return -1;
    }
    return i;
}


/**
 * @brief                  Handler function for ef_ad read binary responses.
 * @param ste_msg          The original message from client.
 * @return                 Status of the transaction. Transaction is DONE (failure and success): 0;
 *                                                    Transaction to be continued: -1
 *
 * This transaction handler handles the following scenario:
 * READ BINARY RESPONSE AD -> Save data -> Read EF_IMSI -> READ BINARY RESPONSE IMSI -> Extract and save MCC and MNC
 * -> Check C&N PLMN list if operator to be found using MCC + MNC of different lengths -> transaction is done
 */
static int uiccd_ad_read_binary_transaction_handler(ste_msg_t * ste_msg)
{
    ste_sim_ctrl_block_t             * ctrl_p = (ste_sim_ctrl_block_t*)(ste_msg->client_tag);
    uiccd_trans_data_ad_t            * data_p;
    int                               rv;
    sim_uicc_status_word_t            status_word = { 0, 0 };
    uint8_t                           mnc_length = 0x0F;
    uint16_t                          mnc_2dig, mnc_3dig, mcc;


    if (ctrl_p == NULL) {
        return 0;
    }
    data_p = (uiccd_trans_data_ad_t*)(ctrl_p->transaction_data_p);
    if (data_p == NULL) {
        free(ctrl_p);
        return 0;
    }

    catd_log_f(SIM_LOGGING_I, "uicc : Enter uiccd_ad_read_binary_transaction_handler");

    switch (data_p->trans_state)
    {
        case UICCD_TRANS_AD_READ_AD_DATA:
        {
            uiccd_msg_read_sim_file_binary_response_t   * rsp_p;

            catd_log_f(SIM_LOGGING_I, "uicc : Transaction state UICCD_TRANS_AD_READ_AD_DATA");

            if (ste_msg->type != UICCD_MSG_READ_SIM_FILE_BINARY_RSP) {
                catd_log_f(SIM_LOGGING_E, "uicc : wrong msg type for transaction state UICCD_TRANS_AD_READ_AD_DATA");
                return -1;
            }

            rsp_p = (uiccd_msg_read_sim_file_binary_response_t*)ste_msg;

            catd_log_f(SIM_LOGGING_I, "uicc : UICCD_TRANS_AD_READ_AD_DATA,  status = %d, details = %d, sw1 = %d, sw2 = %d",
                                                            rsp_p->uicc_status_code, rsp_p->uicc_status_code_fail_details, rsp_p->status_word.sw1, rsp_p->status_word.sw2 );

            if (rsp_p->uicc_status_code != SIM_UICC_STATUS_CODE_OK)
            {
              catd_log_f(SIM_LOGGING_I, "uicc : UICCD_TRANS_AD_READ_AD_DATA failure, status = %d, details = %d sw1 = %d, sw2 = %d",
                                         rsp_p->uicc_status_code, rsp_p->uicc_status_code_fail_details, rsp_p->status_word.sw1, rsp_p->status_word.sw2 );

              uiccd_encode_and_send_read_sim_file_binary_response(ctrl_p->fd,
                                                                  ctrl_p->client_tag,
                                                                  rsp_p->uicc_status_code,
                                                                  rsp_p->uicc_status_code_fail_details,
                                                                  rsp_p->status_word,
                                                                  rsp_p->data,
                                                                  rsp_p->len);

              //transaction must be ended
              free(data_p);
              free(ctrl_p);
              return 0;
            }

            catd_log_f(SIM_LOGGING_I, "uicc : UICCD_TRANS_AD_READ_AD_DATA  extract and save data");

            if ((data_p->ef_ad_read_offset>(EF_AD_MNC_LENGTH_DATA_POS-1))||
               ((data_p->ef_ad_read_offset + rsp_p->len)<EF_AD_MNC_LENGTH_DATA_POS))
            {
              /* The fourth byte has not been read, return data read and end the transaction */
              uiccd_encode_and_send_read_sim_file_binary_response(ctrl_p->fd,
                                                                  ctrl_p->client_tag,
                                                                  rsp_p->uicc_status_code,
                                                                  rsp_p->uicc_status_code_fail_details,
                                                                  rsp_p->status_word,
                                                                  rsp_p->data,
                                                                  rsp_p->len);

              //transaction must be ended
              free(data_p);
              free(ctrl_p);
              return 0;
            }

            /* extract MNC length, fourth and last byte in total data */
            mnc_length = mnc_length & rsp_p->data[EF_AD_MNC_LENGTH_DATA_POS-data_p->ef_ad_read_offset-1];
            data_p->mnc_length = mnc_length;

            catd_log_f(SIM_LOGGING_I, "uicc : UICCD_TRANS_AD_READ_AD_DATA  extract and save data, mnc length raw data =  %#04x, mnc length extracted = %d",
                                      rsp_p->data[EF_AD_MNC_LENGTH_DATA_POS-data_p->ef_ad_read_offset-1], mnc_length  );

            /* save ef_ad data in transaction data. */
            memset(data_p->ef_ad_data, 0, EF_AD_DATA_MAX_LENGTH + 1);
            memcpy(data_p->ef_ad_data, rsp_p->data, rsp_p->len);
            data_p->ef_ad_data_len = rsp_p->len;

            // Read IMSI
            data_p->trans_state = UICCD_TRANS_AD_READ_IMSI_DATA;

            rv = uiccd_read_imsi_number(ctrl_p);

            if (rv != 0) {
              uiccd_encode_and_send_read_sim_file_binary_response(ctrl_p->fd,
                                                                  ctrl_p->client_tag,
                                                                  SIM_UICC_STATUS_CODE_FAIL,
                                                                  SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                  status_word,
                                                                  NULL,
                                                                  0);
              catd_log_f(SIM_LOGGING_E, "uicc : ste_modem_request_sim_file_read_binary failed.");

              //transaction must be ended
              free(data_p);
              free(ctrl_p);
              return 0;
            }
        }
        break;

        case UICCD_TRANS_AD_READ_IMSI_DATA:
        {
            uiccd_msg_read_sim_file_binary_response_t   * rsp_p;

            catd_log_f(SIM_LOGGING_I, "uicc : Transaction state UICCD_TRANS_AD_READ_IMSI_DATA");

            if (ste_msg->type != UICCD_MSG_READ_SIM_FILE_BINARY_RSP) {
                catd_log_f(SIM_LOGGING_E, "uicc : wrong msg type for transaction state UICCD_TRANS_AD_READ_IMSI_DATA");
                return -1;
            }

            rsp_p = (uiccd_msg_read_sim_file_binary_response_t*)ste_msg;

            catd_log_f(SIM_LOGGING_I, "uicc : UICCD_TRANS_AD_READ_IMSI_DATA,  status = %d, details = %d, sw1 = %d, sw2 = %d",
                                                            rsp_p->uicc_status_code, rsp_p->uicc_status_code_fail_details, rsp_p->status_word.sw1, rsp_p->status_word.sw2 );

            if (rsp_p->uicc_status_code != SIM_UICC_STATUS_CODE_OK)
            {
                catd_log_f(SIM_LOGGING_I, "uicc : UICCD_TRANS_AD_READ_IMSI_DATA failure, status = %d, details = %d sw1 = %d, sw2 = %d",
                                rsp_p->uicc_status_code, rsp_p->uicc_status_code_fail_details, rsp_p->status_word.sw1, rsp_p->status_word.sw2 );

                uiccd_encode_and_send_read_sim_file_binary_response(ctrl_p->fd,
                                                                    ctrl_p->client_tag,
                                                                    SIM_UICC_STATUS_CODE_FAIL,
                                                                    SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                    status_word,
                                                                    NULL,
                                                                    0);
                catd_log_f(SIM_LOGGING_E, "uicc : ste_modem_request_sim_file_read_binary failed.");

                //transaction must be ended
                free(data_p);
                free(ctrl_p);
                return 0;
            }

            catd_log_f(SIM_LOGGING_I, "uicc : UICCD_TRANS_AD_READ_IMSI_DATA  extract and save data");

            /* extract MCC and MNC from IMSI number */

            /* Copy the nibbles one by one. Byte 0 is the length, LSN of byte 1 is parity information */

            mcc = ((uint16_t)rsp_p->data[1] << 4 ) & 0x0F00;   /* MCC first digit, byte 1 MSN */
            mcc |= ((uint16_t)rsp_p->data[2] << 4 ) & 0x00F0;  /* MCC second digit, byte 2 LSN */
            mcc |= (uint16_t)( rsp_p->data[2] >> 4 ) & 0x000F;  /* MCC third digit, byte 2 MSN */

            mnc_2dig = ((uint16_t)rsp_p->data[3] << 4 ) & 0x00F0; /* MCC first digit, byte 3 LSN */
            mnc_2dig |= (uint16_t)( rsp_p->data[3] >> 4 ) & 0x000F; /* MCC second digit, byte 3 MSN */

            mnc_2dig |= 0x0F00; /* Set the not used nibble to OxF */

            mnc_3dig = ((uint16_t)rsp_p->data[3] << 8 ) & 0x0F00;  /* MNC first digit, byte 3 LSN */
            mnc_3dig |= ((uint16_t)rsp_p->data[3]) & 0x00F0;       /* MCC second digit, byte 3 MSN */
            mnc_3dig |= (uint16_t)( rsp_p->data[4]) & 0x000F;      /* MCC third digit, byte 4 LSN */

            catd_log_f(SIM_LOGGING_D, "Extracted values MCC=0x%03X, MNC 2 dig=0x%03X, MNC 3 dig=0x%03X", mcc, mnc_2dig, mnc_3dig);

            mnc_length = data_p->mnc_length;

            /* Scan the PLMN list in file ./plmn.latam.list to find the correct MNC length */
            rv = uiccd_scan_plmn_list_file(mcc, mnc_2dig, mnc_3dig, &mnc_length);

            if (rv != 0)
            {
              catd_log_f(SIM_LOGGING_E, "uicc : UICCD_TRANS_AD_READ_IMSI_DATA failure, uiccd_scan_plmn_list_file failed" );

              uiccd_encode_and_send_read_sim_file_binary_response(ctrl_p->fd,
                                                                  ctrl_p->client_tag,
                                                                  SIM_UICC_STATUS_CODE_FAIL,
                                                                  SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                  status_word,
                                                                  NULL,
                                                                  0);

              //transaction must be ended
              free(data_p);
              free(ctrl_p);
              return 0;
            }

            if (mnc_length != data_p->mnc_length)
            {
              catd_log_f(SIM_LOGGING_D, "uicc : UICCD_TRANS_AD_READ_IMSI_DATA, update MNC length in return data with new value:" );
              catd_log_f(SIM_LOGGING_D, "uicc : EF_AD MNC length = %d, New MNC length = %d",  data_p->mnc_length, mnc_length );
              data_p->ef_ad_data[EF_AD_MNC_LENGTH_DATA_POS-1] = mnc_length;
            }

            uiccd_encode_and_send_read_sim_file_binary_response(ctrl_p->fd,
                                                                ctrl_p->client_tag,
                                                                rsp_p->uicc_status_code,
                                                                rsp_p->uicc_status_code_fail_details,
                                                                rsp_p->status_word,
                                                                data_p->ef_ad_data,
                                                                data_p->ef_ad_data_len);

             //the transaction is done
             free(data_p);
             free(ctrl_p);
             return 0;

        }
        break;

        default:
        {
            catd_log_f(SIM_LOGGING_E, "uicc : wrong transaction state for ad read binary");
            free(data_p);
            free(ctrl_p);
            return 0;
        }
    }
    return -1;
}

#endif

int uiccd_main_read_sim_file_binary(ste_msg_t * ste_msg)
{

    ste_modem_t            *m;
    uiccd_msg_read_sim_file_binary_t *msg = (uiccd_msg_read_sim_file_binary_t *) ste_msg;
    ste_sim_ctrl_block_t   * ctrl_p;
    sim_uicc_status_word_t status_word = {0,0};

 // Enter busy state...

    catd_log_f(SIM_LOGGING_I, "uicc : READ EF binary message received from: %d ",msg->fd);


    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        uiccd_encode_and_send_read_sim_file_binary_response(msg->fd,
                                                            msg->client_tag,
                                                            SIM_UICC_STATUS_CODE_FAIL,
                                                            SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                            status_word,
                                                            NULL,
                                                            0);
        return -1;
    } else {
      int i;
      const char *p = msg->data, *p_max = msg->data + msg->len;
      int file_id, offset, length;
      char *file_path = NULL;
      size_t path_len;

#ifdef SIM_MNC_LENGTH_CHECK
      uiccd_trans_data_ad_t  * data_p = NULL;
#endif


        // split msg_data into separate parameters
        p = sim_dec(p, &file_id, sizeof(file_id), p_max);
        p = sim_dec(p, &offset, sizeof(offset), p_max);
        p = sim_dec(p, &length, sizeof(length), p_max);

        p = sim_dec(p, &path_len, sizeof(path_len), p_max);

        if (path_len > 0) {
          file_path = malloc(path_len);
          if (!file_path) {
            uiccd_encode_and_send_read_sim_file_binary_response(msg->fd,
                                                                msg->client_tag,
                                                                SIM_UICC_STATUS_CODE_FAIL,
                                                                SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                status_word,
                                                                NULL,
                                                                0);
            catd_log_f(SIM_LOGGING_E, "uicc : memory allocation failed");
            return -1;
          }
          p = sim_dec(p, file_path, path_len*sizeof(*file_path), p_max);
          if (!p || file_path[path_len-1] != '\0') {
            uiccd_encode_and_send_read_sim_file_binary_response(msg->fd,
                                                                msg->client_tag,
                                                                SIM_UICC_STATUS_CODE_FAIL,
                                                                SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                status_word,
                                                                NULL,
                                                                0);
            catd_log_f(SIM_LOGGING_E, "uicc : sim_dec failed");
            free(file_path);
            return -1;
          }
        }
        else
        {
            path_len = 1;
            file_path = malloc(path_len);
            if (!file_path) {
              uiccd_encode_and_send_read_sim_file_binary_response(msg->fd,
                                                                  msg->client_tag,
                                                                  SIM_UICC_STATUS_CODE_FAIL,
                                                                  SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                  status_word,
                                                                  NULL,
                                                                  0);
              catd_log_f(SIM_LOGGING_E, "uicc : memory allocation failed");
              return -1;
            }
            file_path[0] = '\0';
        }

#ifdef SIM_MNC_LENGTH_CHECK
        /* If reading of file EF_AD is requested and MNC length (4th byte in data) is included in data to be read
         * start a check of MCC/MNC to ensure that MNC length read from EF_AD is correct.
         */
        if ((file_id == EF_AD_FILE_ID)&&
            ((offset<=EF_AD_MNC_LENGTH_DATA_POS)&&((length == 0)||((offset+length)>=EF_AD_MNC_LENGTH_DATA_POS))))
        {
          /* Start transaction to check that the value of MNC length specified in EF_AD data is correct,
           * else return error:
           * 1) Read EF_AD and save data
           * 2) Read EF_IMSI, extract MCC and MNC two digits long and MNC three digits long from the IMSI number
           * 3) Check the PLMN list if any operator is to be found using MCC+MNC values from step 2)
           * 4) Return EF_AD data including the correct MNC length calculated from step 3).
           */

          catd_log_f(SIM_LOGGING_I, "uicc : READ EF_AD data: start AD transaction ");

          data_p = malloc(sizeof(uiccd_trans_data_ad_t));
          if (!data_p) {
             catd_log_f(SIM_LOGGING_E, "uicc : memory allocation failed");

             uiccd_encode_and_send_read_sim_file_binary_response(msg->fd,
                                                                 msg->client_tag,
                                                                 SIM_UICC_STATUS_CODE_FAIL,
                                                                 SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                 status_word,
                                                                 NULL,
                                                                 0);

             free(file_path);
             return -1;
          }

          memset(data_p, 0, sizeof(uiccd_trans_data_ad_t));

          data_p->trans_state = UICCD_TRANS_AD_READ_AD_DATA;
          data_p->ef_ad_read_offset = offset;

          ctrl_p = ste_sim_create_ctrl_block(msg->client_tag, msg->fd, UICCD_MSG_READ_SIM_FILE_BINARY_TRANSACTION, uiccd_ad_read_binary_transaction_handler, data_p);
        }
        else
#endif
        {
          ctrl_p = ste_sim_create_ctrl_block(msg->client_tag, msg->fd, msg->type, NULL, NULL);
        }

        if (!ctrl_p) {
            uiccd_encode_and_send_read_sim_file_binary_response(msg->fd,
                                                                msg->client_tag,
                                                                SIM_UICC_STATUS_CODE_FAIL,
                                                                SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                status_word,
                                                                NULL,
                                                                0);
            free(file_path);
#ifdef SIM_MNC_LENGTH_CHECK
            free(data_p);
#endif
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
            uiccd_encode_and_send_read_sim_file_binary_response(msg->fd,
                                                                msg->client_tag,
                                                                SIM_UICC_STATUS_CODE_FAIL,
                                                                SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                status_word,
                                                                NULL,
                                                                0);
            catd_log_f(SIM_LOGGING_E, "uicc : "
                       "ste_modem_request_sim_file_read_binary failed.");
            free(ctrl_p);
            free(file_path);
#ifdef SIM_MNC_LENGTH_CHECK
            free(data_p);
#endif
            return -1;
        }
        free(file_path);
    }
    return 0;
}

int uiccd_main_read_sim_file_binary_response(ste_msg_t * ste_msg)
{
    uiccd_msg_read_sim_file_binary_response_t* msg;
    char                    *p = NULL;
    char                    *buf = NULL;
    size_t                  buf_len;
    unsigned                data_len;
    ste_sim_ctrl_block_t   * ctrl_p;

    msg = (uiccd_msg_read_sim_file_binary_response_t*)ste_msg;

    catd_log_f(SIM_LOGGING_I, "uicc : EF Binary read response message received");

    ctrl_p = (ste_sim_ctrl_block_t*)(msg->client_tag);

    if (!ctrl_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : Control Block is NULL");
        return -1;
    }

    data_len = msg->len;

    buf_len = sizeof(char)*data_len;
    buf_len += sizeof(data_len);
    buf_len += sizeof(msg->status_word.sw1) + sizeof(msg->status_word.sw2);
    buf_len += sizeof(msg->uicc_status_code) + sizeof(msg->uicc_status_code_fail_details);

    buf = malloc(buf_len);
    if (!buf) {
      catd_log_f(SIM_LOGGING_E, "uicc : Could not allocate memory in uiccd_main_read_sim_file_binary_response");
      return -1;
    }
    p = buf;

    p = sim_enc(p, &msg->uicc_status_code, sizeof(sim_uicc_status_code_t));
    p = sim_enc(p, &msg->uicc_status_code_fail_details, sizeof(sim_uicc_status_code_fail_details_t));
    p = sim_enc(p, &msg->status_word.sw1, sizeof(msg->status_word.sw1));
    p = sim_enc(p, &msg->status_word.sw2, sizeof(msg->status_word.sw2));
    p = sim_enc(p, &data_len, sizeof(data_len));
    p = sim_enc(p, msg->data,      sizeof(char)*(data_len));

    if (buf_len != (size_t)(p-buf)) {
        catd_log_f(SIM_LOGGING_E, "uicc: uiccd_main_read_sim_file_binary_response, assert failure");
    }

    assert( buf_len == (size_t)(p-buf) );

    sim_send_generic(ctrl_p->fd,
                     STE_UICC_RSP_READ_SIM_FILE_BINARY,
                     buf,
                     buf_len,
                     ctrl_p->client_tag);

    free( buf );
    free(ctrl_p);
    return 0;
}
