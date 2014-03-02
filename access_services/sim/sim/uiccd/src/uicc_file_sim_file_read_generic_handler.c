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
#include "uicc_state_machine.h"

//control data for read generic
typedef struct {
    int                      file_id;
    int                      length;
    int                      record_id;
    int                      offset;
    sim_file_structure_t     file_type;
} uiccd_trans_data_read_generic_t;

/**
 * @brief                  Function for creating the result string for read generic, and send back to client.
 * @param fd               socket fd to be sent to.
 * @param client_tag       client tag from the client.
 * @param status           read generic status.
 * @param data_p           data of the read result.
 * @param length           length of the read data.
 * @param file_id          The original file id from client.
 * @return                 Status of the operation. Success: 0; Failed: -1
 */
static int uiccd_read_generic_send_response(int fd, uintptr_t client_tag,
                                            sim_uicc_status_code_t uicc_status_code,
                                            sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                            sim_uicc_status_word_t status_word,
                                            uint8_t * data_p, size_t length, int file_id)
{
    char                   * p = NULL;
    char                   * buf_p = NULL;
    size_t                   buf_len;
    unsigned                 data_len;
    unsigned                 file_id_len;

    data_len = length;
    file_id_len = sizeof(file_id);

    buf_len = sizeof(char) * (data_len + file_id_len);
    buf_len += sizeof(data_len) + sizeof(file_id_len);
    buf_len += sizeof(uicc_status_code) + sizeof(uicc_status_code_fail_details);
    buf_len += sizeof(status_word.sw1) + sizeof(status_word.sw2);

    buf_p = malloc(buf_len);
    if (!buf_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : memory allocation failed");
        sim_send_generic(fd,
                         STE_UICC_RSP_SIM_FILE_READ_GENERIC,
                         NULL,
                         0,
                         client_tag);
        return -1;
    }

    p = buf_p;

    p = sim_enc(p, &uicc_status_code, sizeof(uicc_status_code));
    p = sim_enc(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details));
    p = sim_enc(p, &status_word.sw1, sizeof(status_word.sw1));
    p = sim_enc(p, &status_word.sw2, sizeof(status_word.sw2));

    p = sim_enc(p, &data_len, sizeof(data_len));
    p = sim_enc(p, data_p, sizeof(char)*(data_len));

    p = sim_enc(p, &file_id_len, sizeof(file_id_len));    //currently, we only send back file id, but if other info is needed, encode it after file id
    p = sim_enc(p, &file_id, sizeof(char)*(file_id_len));

    if (buf_len != (size_t)(p - buf_p)) {
        catd_log_f(SIM_LOGGING_E, "uicc: uiccd_read_generic_send_response, assert failure");
    }

    assert( buf_len == (size_t)(p - buf_p) );

    sim_send_generic(fd,
                     STE_UICC_RSP_SIM_FILE_READ_GENERIC,
                     buf_p,
                     buf_len,
                     client_tag);

    free(buf_p);

    return 0;
}

/**
 * @brief                  Handler function for read generic file response.
 * @param ste_msg          The original message from client.
 * @return                 Status of the operation. Success: 0; Failed: -1
 *
 * From the response message, we get the ctrl block for the read generic request.
 * In the ctrl block, there is information like file id, which will be encoded and
 * sent back to client.
 * The file id will be used by client to identify which file this specific read is on.
 */
static int uiccd_main_sim_file_read_generic_response(ste_msg_t * ste_msg)
{
    uiccd_msg_sim_file_read_generic_response_t * msg;
    uiccd_trans_data_read_generic_t            * file_data_p;
    ste_sim_ctrl_block_t   * ctrl_p;
    int                      rv;

    msg = (uiccd_msg_sim_file_read_generic_response_t*)ste_msg;

    catd_log_f(SIM_LOGGING_I, "uicc : read generic response message received");

    ctrl_p = (ste_sim_ctrl_block_t*)(msg->client_tag);

    if (!ctrl_p) {  //this will never happen
        catd_log_f(SIM_LOGGING_E, "uicc : Control Block is NULL");
        return 0;  //return 0 because anyway this transaction is done
    }
    file_data_p = (uiccd_trans_data_read_generic_t*)(ctrl_p->transaction_data_p); //we want to send back to client the file id, which is saved in this ctrl block
    if (!file_data_p) {  //this will never happen
        catd_log_f(SIM_LOGGING_E, "uicc : file_data_p is NULL");
        return 0;  //return 0 because anyway this transaction is done
    }

    rv = uiccd_read_generic_send_response(ctrl_p->fd, ctrl_p->client_tag, msg->uicc_status_code, msg->uicc_status_code_fail_details, msg->status_word,
                                          msg->data, msg->len, file_data_p->file_id);

    if (rv != 0) {
        catd_log_f(SIM_LOGGING_E, "uicc : send back read generic result failed");
        //free resources, and return 0 because anyway this transaction is done
    }

    free(file_data_p);
    free(ctrl_p);

    return 0;
}

/**
 * @brief                  Main function for read generic file.
 * @param ste_msg          The original message from client.
 * @return                 Status of the operation. Success: 0; Failed: -1
 *
 * To read a specific file from sim card, the client needs to provide the file id,
 * and other related data such as record id, offset, etc., if needed.
 * This main function will then save the file_id and related info into the
 * transaction data, which will be used when read response is received.
 *
 * Based on the different file id, we call different interfaces from modem(read record, read binary)
 * to fetch the data. Certainly, different responses will be sent back from modem.
 * But both responses(either read record rsp or read binary rsp) are handled in the
 * transaction handler function, instead of the old read record
 * or read binary rsp handler function.
 *
 * This function has no use since the client knows what type of file it is
 * accessing and hence how it should handle the response.
 */
int uiccd_main_sim_file_read_generic(ste_msg_t * ste_msg)
{
    const struct {
        char       * usim_file_path;
        int          usim_file_id;
        char       * gsm_file_path;
        int          gsm_file_id;
    } imsi_file = { "3F007FFF", 0x6F07, "3F007F20", 0x6F07 };

    uiccd_trans_data_read_generic_t    * file_data_p;
    uiccd_msg_sim_file_read_generic_t  * msg = (uiccd_msg_sim_file_read_generic_t *) ste_msg;
    ste_modem_t            * modem_p;
    ste_sim_ctrl_block_t   * ctrl_p;
    int                      rv;
    const char             * p     = msg->data;
    const char             * p_max = msg->data + msg->len;
    int                      file_id;
    int                      modem_file_id;
    int                      param_len;
    void                   * file_param_p = NULL;
    char                   * file_path_p = NULL;
    sim_uicc_status_word_t status_word = {0,0};

    catd_log_f(SIM_LOGGING_I, "uicc : SIM FILE READ GENERIC message received from: %d ",msg->fd);

    // split msg_data into separate parameters, file id is to be used soon, so decode the data earlier
    p = sim_dec(p, &file_id, sizeof(file_id), p_max);
    p = sim_dec(p, &param_len, sizeof(param_len), p_max);

    modem_p = catd_get_modem();
    if (!modem_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        //since this mechanism of calling uiccd_sig_sim_file_read_generic_response to triger the send back to client the read result is not working, we directly send back the result here
        //uiccd_sig_sim_file_read_generic_response(msg->client_tag, -1, NULL, 0);
        rv = uiccd_read_generic_send_response(msg->fd,
                                              msg->client_tag,
                                              SIM_UICC_STATUS_CODE_FAIL,
                                              SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                              status_word,
                                              NULL,
                                              0,
                                              file_id);
        return -1;
    }

    if (param_len > 0) {
        file_param_p = malloc(param_len);
        if (!file_param_p) {
            catd_log_f(SIM_LOGGING_E, "uicc : memory allocation failed");
            //since this mechanism of calling uiccd_sig_sim_file_read_generic_response to triger the send back to client the read result is not working, we directly send back the result here
            //uiccd_sig_sim_file_read_generic_response(msg->client_tag, -1, NULL, 0);
            rv = uiccd_read_generic_send_response(msg->fd,
                                                  msg->client_tag,
                                                  SIM_UICC_STATUS_CODE_FAIL,
                                                  SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                  status_word,
                                                  NULL,
                                                  0,
                                                  file_id);
            return -1;
        }
        p = sim_dec(p, (char*)file_param_p, param_len, p_max);
        if ( !p ) {
            // Something went wrong in one of the sim_dec above.
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_main_sim_file_read_generic failed");
            //since this mechanism of calling uiccd_sig_sim_file_read_generic_response to triger the send back to client the read result is not working, we directly send back the result here
            //uiccd_sig_sim_file_read_generic_response(msg->client_tag, -1, NULL, 0);
            rv = uiccd_read_generic_send_response(msg->fd,
                                                  msg->client_tag,
                                                  SIM_UICC_STATUS_CODE_FAIL,
                                                  SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                  status_word,
                                                  NULL,
                                                  0,
                                                  file_id);
            free(file_param_p);
            return -1;
        }
    }

    //to save the file id, to be used when get the response
    file_data_p = malloc(sizeof(uiccd_trans_data_read_generic_t));
    if (!file_data_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : memory allocation failed");
        //since this mechanism of calling uiccd_sig_sim_file_read_generic_response to triger the send back to client the read result is not working, we directly send back the result here
        //uiccd_sig_sim_file_read_generic_response(msg->client_tag, -1, NULL, 0);
        rv = uiccd_read_generic_send_response(msg->fd,
                                              msg->client_tag,
                                              SIM_UICC_STATUS_CODE_FAIL,
                                              SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                              status_word,
                                              NULL,
                                              0,
                                              file_id);
        free(file_param_p);
        return -1;
    }
    memset(file_data_p, 0, sizeof(uiccd_trans_data_read_generic_t));
    file_data_p->file_id = file_id;  //save the file_id from client, it is to be used when response is sent back

    //if there is params such as record_id, offset, etc. passed by client, then save it into the file_data_p.
    //for different file_id, the members of file_data_p should be set to proper values,
    //based on the information from client and the information of SIM card, for example, card type, activated application, etc.
    switch (file_id)
    {
        case SIM_EF_IMSI:
        {
            file_path_p = (uicc_get_card_type() == STE_UICC_CARD_TYPE_ICC ? imsi_file.gsm_file_path : imsi_file.usim_file_path);
            modem_file_id = (uicc_get_card_type() == STE_UICC_CARD_TYPE_ICC ? imsi_file.gsm_file_id : imsi_file.usim_file_id);
            file_data_p->length = SIM_EF_IMSI_LEN;
            file_data_p->file_type = SIM_FILE_STRUCTURE_TRANSPARENT;
            file_data_p->offset = 0;
        }
        break;
        default:
        {
            catd_log_f(SIM_LOGGING_E, "uicc : unknown file id from client");
            uiccd_sig_sim_file_read_generic_response(msg->client_tag,
                                                     SIM_UICC_STATUS_CODE_FAIL,
                                                     SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                     status_word,
                                                     NULL,
                                                     0);
            free(file_param_p);
            free(file_data_p);
            return -1;
        }
    }

    ctrl_p = ste_sim_create_ctrl_block(msg->client_tag, msg->fd, msg->type, uiccd_main_sim_file_read_generic_response, file_data_p);
    if (!ctrl_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : failed to create ctrl block for read generic");
        //since this mechanism of calling uiccd_sig_sim_file_read_generic_response to triger the send back to client the read result is not working, we directly send back the result here
        //uiccd_sig_sim_file_read_generic_response(msg->client_tag, -1, NULL, 0);
        rv = uiccd_read_generic_send_response(msg->fd,
                                              msg->client_tag,
                                              SIM_UICC_STATUS_CODE_FAIL,
                                              SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                              status_word,
                                              NULL,
                                              0,
                                              file_id);
        free(file_param_p);
        free(file_data_p);
        return -1;
    }

    //now call modem based on the file data info.
    switch (file_data_p->file_type)
    {
        case SIM_FILE_STRUCTURE_TRANSPARENT:
        {
            rv = ste_modem_file_read_binary(modem_p, (uintptr_t)ctrl_p, uicc_get_app_id(),
                                           modem_file_id, file_data_p->offset, file_data_p->length,
                                           file_path_p);
        }
        break;

        case SIM_FILE_STRUCTURE_LINEAR_FIXED:
        {
            rv = ste_modem_file_read_record(modem_p, (uintptr_t)ctrl_p, uicc_get_app_id(),
                                           modem_file_id, file_data_p->record_id, file_data_p->length,
                                           file_path_p);
        }
        break;
        default:
        {
            catd_log_f(SIM_LOGGING_E, "uicc : unknown file type=%d",file_data_p->file_type);
            //since this mechanism of calling uiccd_sig_sim_file_read_generic_response to triger the send back to client the read result is not working, we directly send back the result here
            //uiccd_sig_sim_file_read_generic_response(msg->client_tag, -1, NULL, 0);
            rv = uiccd_read_generic_send_response(msg->fd,
                                                  msg->client_tag,
                                                  SIM_UICC_STATUS_CODE_FAIL,
                                                  SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                  status_word,
                                                  NULL,
                                                  0,
                                                  file_id);
            free(file_param_p);
            free(file_data_p);
            free(ctrl_p);
            return -1;
        }
    }

    if (rv != 0) {
        catd_log_f(SIM_LOGGING_E, "uicc : call modem to read sim file failed.");
        //since this mechanism of calling uiccd_sig_sim_file_read_generic_response to triger the send back to client the read result is not working, we directly send back the result here
        //uiccd_sig_sim_file_read_generic_response(msg->client_tag, -1, NULL, 0);
        rv = uiccd_read_generic_send_response(msg->fd,
                                              msg->client_tag,
                                              SIM_UICC_STATUS_CODE_FAIL,
                                              SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                              status_word,
                                              NULL,
                                              0,
                                              file_id);
        free(file_param_p);
        free(file_data_p);
        free(ctrl_p);
        return -1;
    }

    if (file_param_p) {
        free(file_param_p);
    }

    return 0;
}
