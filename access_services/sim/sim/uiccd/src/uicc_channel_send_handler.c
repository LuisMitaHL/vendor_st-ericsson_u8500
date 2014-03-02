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

#define MAX_PATH_LENGTH 16

#define APDU_CLA_BYTE   0
#define APDU_INS_BYTE   1
#define APDU_P1_BYTE    2
#define APDU_P2_BYTE    3
#define APDU_LC_BYTE    4

typedef struct select_cache_s {
    uint8_t path[MAX_PATH_LENGTH];
    uint8_t path_length;
    int file_id;
    boolean wasLatestSelect;
    uint8_t bytes_remaining;
} select_cache_t;

static select_cache_t *select_cache_p = NULL;

typedef enum {
    UICCD_TRANS_CHANNEL_SEND_GET_FORMAT,
    UICCD_TRANS_CHANNEL_SEND_GET_FILE_INFORMATION
} uiccd_trans_channel_send_state_t;

typedef enum {
    UICCD_SEND_MESSAGE_NORMAL,
    UICCD_SEND_MESSAGE_STATUS_WORD,
    UICCD_SEND_MESSAGE_GET_RESPONSE
} uiccd_send_message_state_t;

typedef struct {
    uiccd_trans_channel_send_state_t trans_state;
    int file_id;
    uint8_t path[MAX_PATH_LENGTH + 2];
    size_t path_length;
    uiccd_send_message_state_t message_state;
    boolean wasRelative;
    uint8_t req_size;
    uint8_t max_return_size;
    uint8_t CLA;
} uiccd_trans_data_channel_send_t;

typedef enum {
    UICCD_SELECT_P1_COMMAND_DF_EF_MF_FILEID =      0x00,
    UICCD_SELECT_P1_COMMAND_CHILD_CURRENT_DF =     0x01,
    UICCD_SELECT_P1_COMMAND_PARENT_CURRENT_DF =    0x03,
    UICCD_SELECT_P1_COMMAND_PATH_FROM_MF =         0x08,
    UICCD_SELECT_P1_COMMAND_PATH_CURRENT_DF =      0x09
} uiccd_select_p1_command_t;

typedef enum {
    UICCD_SELECT_P2_COMMAND_NO_DATA_RETURN_SIM =  0x00,
    UICCD_SELECT_P2_COMMAND_RETURN_FCP_TEMPLATE = 0x04,
    UICCD_SELECT_P2_COMMAND_NO_DATA_RETURN_USIM = 0x0C
} uiccd_select_p2_command_t;

void uiccd_channel_send_clear_cache();
static int cache_path(uint8_t *path, uint8_t path_length, boolean use_fileid);
static int append_cached_path(uint8_t *new_path, uint8_t path_length);
static void clear_latest_flag();

static int uiccd_encode_and_send_sim_channel_send_response( int fd,
                                                            uintptr_t client_tag,
                                                            sim_uicc_status_code_t uicc_status_code,
                                                            sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                            sim_uicc_status_word_t status_word,
                                                            const uint8_t  *data,
                                                            size_t data_len)
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
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_sim_channel_send_response, memory allocation failed");
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
                     STE_UICC_RSP_SIM_CHANNEL_SEND,
                     buf_p,
                     buf_len,
                     client_tag);

    free(buf_p);

    return 0;
}

int uiccd_main_sim_channel_send_response(ste_msg_t * ste_msg)
{
    uiccd_msg_sim_channel_send_response_t* msg;
    ste_sim_ctrl_block_t   * ctrl_p;

    msg = (uiccd_msg_sim_channel_send_response_t*)ste_msg;
    ctrl_p = (ste_sim_ctrl_block_t*)(msg->client_tag);

    if (!ctrl_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : Control Block is NULL");
        return -1;
    }

    uiccd_encode_and_send_sim_channel_send_response(ctrl_p->fd,
                                                    ctrl_p->client_tag,
                                                    msg->uicc_status_code,
                                                    msg->uicc_status_code_fail_details,
                                                    msg->status_word,
                                                    msg->data,
                                                    msg->len);

    free(ctrl_p);
    return 0;
}

/**
 * @brief                  Get file information
 * @param ctrl_p           Sim control block
 * @param file_id          File ID, can be 0
 * @param path             Path, can be NULL.
 * @param path_length      Length of supplied path.
 * @param type             Type of file.
 * @return                 Status of the operation. Operation is Ok: 0; Operation failed: -1
 *
 * Converts input path to char before sending it on.
 *
 */
static int uiccd_get_file_information(ste_sim_ctrl_block_t * ctrl_p,
                                      int file_id,
                                      uint8_t *path,
                                      size_t path_length,
                                      sim_uicc_get_file_info_type_t type)
{
    int status;
    uint8_t i;
    ste_modem_t *m;
    char char_path[200];

    m = catd_get_modem();

    if (!m) {
        catd_log_f(SIM_LOGGING_E, "%s : No modem", __func__);
        return -1;
    }

    memset(char_path, 0x0, sizeof(char) * 200);

    for (i = 0; i < path_length; i++) {
        sprintf(&char_path[i * 2], "%.2X", path[i]);
    }

    status = ste_modem_get_file_information(m, (uintptr_t)ctrl_p,
                                            uicc_get_app_id(),
                                            file_id,
                                            char_path,
                                            type);

    if (status != 0) {
        catd_log_f(SIM_LOGGING_E, "%s : "
                   "ste_modem_get_file_information failed.", __func__);
        return -1;
    }

    return status;
}

/**
 * @brief                  Handler function for channel send
 * @param ste_msg          The original message from client.
 * @return                 Status of the transaction. Transaction is DONE: 0; Transaction to be continued: -1
 *
 * All transaction states check the status code. If the status code indicate a failure, the transaction is ended
 * and the error is sent to the client.
 *
 */
static int uiccd_channel_send_transaction_handler(ste_msg_t * ste_msg)
{
    ste_sim_ctrl_block_t             * ctrl_p = (ste_sim_ctrl_block_t*)(ste_msg->client_tag);
    uiccd_trans_data_channel_send_t  * data_p;
    int                                result = 0; // Default is to the end the transaction

    if (ctrl_p == NULL) {
        return 0;
    }

    data_p = (uiccd_trans_data_channel_send_t*)(ctrl_p->transaction_data_p);

    if (data_p == NULL) {
        free(ctrl_p);
        return 0;
    }

    switch (data_p->trans_state)
    {
        case UICCD_TRANS_CHANNEL_SEND_GET_FILE_INFORMATION:
        {
            uiccd_msg_get_file_information_response_t* rsp_p;
            uint8_t *rsp_data_p = NULL;
            size_t rsp_len;

            if (ste_msg->type != UICCD_MSG_GET_FILE_INFORMATION_RSP) {
                catd_log_f(SIM_LOGGING_E, "uicc : wrong msg type for transaction state UICCD_TRANS_CHANNEL_SEND_GET_FILE_INFORMATION");
                return -1; // Continue transaction
            }

            rsp_p = (uiccd_msg_get_file_information_response_t*)ste_msg;
            rsp_len = rsp_p->len;
            rsp_data_p = rsp_p->data;

            switch( data_p->message_state ) {
                case UICCD_SEND_MESSAGE_STATUS_WORD:
                {
                    rsp_data_p = (uint8_t *)&rsp_p->status_word.sw1;
                    rsp_len = 2;
                    if ( rsp_p->uicc_status_code == SIM_UICC_STATUS_CODE_OK ) {
                        if (uicc_get_card_type() == STE_UICC_CARD_TYPE_ICC &&
                                data_p->CLA != 0x00) {
                            rsp_p->status_word.sw1 = 0x9F;
                            rsp_p->status_word.sw2 = rsp_p->len + 2; /* For the status words */
                        } else {
                            rsp_p->status_word.sw1 = 0x90;
                            rsp_p->status_word.sw2 = 0x00;
                        }
                        catd_log_f(SIM_LOGGING_D, "%s : Sending status word: %x %x", __func__, rsp_p->status_word.sw1, rsp_p->status_word.sw2);
                    } else {
                        clear_latest_flag();
                        if ( data_p->wasRelative ) {
                            cache_path(select_cache_p->path, select_cache_p->path_length, FALSE);
                        }
                    }
                    break;
                }
                case UICCD_SEND_MESSAGE_GET_RESPONSE:
                {
                    if ( select_cache_p->bytes_remaining == 0 &&
                            data_p->req_size != (rsp_p->len + 2) ) {
                        rsp_p->status_word.sw1 = 0x67;
                        rsp_p->status_word.sw2 = 0x00;
                        rsp_data_p = (uint8_t *)&rsp_p->status_word.sw1;
                        rsp_len = 2;
                        catd_log_f(SIM_LOGGING_D, "%s : Get file information size mismatch: %d, %d", __func__, data_p->req_size, rsp_p->len + 2);
                    } else {
                        if ( select_cache_p->bytes_remaining > 0) {
                            uint8_t left = select_cache_p->bytes_remaining;
                            memmove(rsp_data_p, &rsp_data_p[rsp_len - left], left);
                            rsp_len = left;
                            select_cache_p->bytes_remaining = 0;
                        }
                        rsp_p->data = realloc(rsp_p->data, rsp_len + 2);
                        rsp_data_p = rsp_p->data;
                        rsp_data_p[rsp_len] = rsp_p->status_word.sw1;
                        rsp_data_p[rsp_len + 1] = rsp_p->status_word.sw2;
                        rsp_len += 2;
                    }
                    break;
                }
                case UICCD_SEND_MESSAGE_NORMAL:
                {
                    if ( rsp_p->uicc_status_code != SIM_UICC_STATUS_CODE_OK ) {
                        rsp_data_p = (uint8_t *)&rsp_p->status_word.sw1;
                        rsp_len = 2;
                        clear_latest_flag();
                        if ( data_p->wasRelative ) {
                            cache_path(select_cache_p->path, select_cache_p->path_length, FALSE);
                        }
                    } else { /* Add status words to end of data */
                        if ( (data_p->max_return_size > 0 &&
                                (rsp_len > data_p->max_return_size)) ||
                                rsp_len > 256) {
                            data_p->max_return_size =
                                    (data_p->max_return_size == 0) ?
                                            256 : data_p->max_return_size;

                            rsp_p->status_word.sw1 = 0x61;
                            rsp_p->status_word.sw2 = rsp_len - data_p->max_return_size;
                            select_cache_p->bytes_remaining = rsp_p->status_word.sw2;
                            select_cache_p->wasLatestSelect = TRUE;
                            rsp_len = data_p->max_return_size;
                        }
                        rsp_p->data = realloc(rsp_p->data, rsp_len + 2);
                        rsp_data_p = rsp_p->data;
                        rsp_data_p[rsp_len] = rsp_p->status_word.sw1;
                        rsp_data_p[rsp_len + 1] = rsp_p->status_word.sw2;
                        rsp_len += 2;
                    }
                    break;
                }
                default:
                {
                    catd_log_f(SIM_LOGGING_E, "%s : Unknown message state", __func__);
                    free(data_p);
                    free(ctrl_p);
                    return 0;
                }
            }

            catd_log_f(SIM_LOGGING_D, "%s : status: %X, sw1: %.2X, sw2: %.2X", __func__,
                    rsp_p->uicc_status_code, rsp_p->status_word.sw1, rsp_p->status_word.sw2);
            if ( uiccd_encode_and_send_sim_channel_send_response(ctrl_p->fd,
                                                                ctrl_p->client_tag,
                                                                rsp_p->uicc_status_code,
                                                                rsp_p->uicc_status_code_fail_details,
                                                                rsp_p->status_word,
                                                                rsp_data_p,
                                                                rsp_len) != 0 ) {
                catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_sim_channel_send_response failed");
            }
            free(data_p);
            free(ctrl_p);
        }
        break;
        default:
        {
            catd_log_f(SIM_LOGGING_E, "uicc : wrong transaction state for read smsc=%d",data_p->trans_state);
            free(data_p);
            free(ctrl_p);
        }
        break;
    }
    return result;
}

void uiccd_channel_send_clear_cache()
{
    if ( !select_cache_p ) {
        return;
    }

    free(select_cache_p);
    select_cache_p = NULL;
}

static int cache_path(uint8_t *path, uint8_t path_length, boolean use_fileid)
{
    uint8_t rm_len = 0;

    if ( !path || path_length == 0 ) {
        return -1;
    }

    if (select_cache_p) {
        uiccd_channel_send_clear_cache();
    }

    select_cache_p = malloc(sizeof(select_cache_t));

    if (!select_cache_p) {
        return -1;
    }

    memset(select_cache_p, 0, sizeof(select_cache_t));
    select_cache_p->path_length = path_length;

    if ( path_length == 2 && (path[0] == 0x3F && path[1] == 0x00) ) {
        select_cache_p->file_id = 0;
    } else if (use_fileid) {    /* Store the last FID as file id. */
        rm_len = 2;
        select_cache_p->file_id = (path[path_length - 2] << 8) | path[path_length - 1];
    }

    /* Last FID is stored in file id */
    memmove(select_cache_p->path, path, select_cache_p->path_length - rm_len);
    select_cache_p->path_length -= rm_len;
    select_cache_p->wasLatestSelect = TRUE;

    return 0;
}

static int append_cached_path(uint8_t *new_path, uint8_t path_length)
{
    select_cache_t path;
    boolean use_fileid = FALSE;
    if ( !select_cache_p ) {
        return -1;
    }
    memset(&path, 0, sizeof(select_cache_t));
    path.path_length = select_cache_p->path_length;
    memmove(path.path, select_cache_p->path, select_cache_p->path_length);
    path.file_id = select_cache_p->file_id;

    if ( (path.path_length + path_length + ((path.file_id != 0) ? 2 : 0)) >=
            MAX_PATH_LENGTH ) {
        return -1;
    }

    if ( path.file_id != 0 ) {
        path.path[path.path_length] = path.file_id >> 8;
        path.path[path.path_length + 1] = path.file_id & 0xFF;
        path.path_length += 2;
        use_fileid = TRUE;
    }

    memmove(&path.path[path.path_length], new_path, path_length);
    path.path_length += path_length;

    cache_path(path.path, path.path_length, use_fileid);

    return 0;
}

static int parent_cached_path(uint8_t *new_path, uint8_t path_length)
{
    select_cache_t path;

    if ( !select_cache_p ) {
        return -1;
    }

    memset(&path, 0, sizeof(select_cache_t));
    path.path_length = select_cache_p->path_length;
    memmove(path.path, select_cache_p->path, path.path_length);

    path.path[path.path_length] = select_cache_p->file_id >> 8;
    path.path[path.path_length + 1] = select_cache_p->file_id & 0xFF;
    path.path_length += 2;

    if ( path_length > 0 ) {
        int len = (path.path_length > 0) ? path.path_length : 2;
        path.path[len - 2] = new_path[0];
        path.path[len - 1] = new_path[1];
    } else if (path.path_length > 2) {
        path.path[path.path_length - 1] = 0x0;
        path.path[path.path_length - 2] = 0x0;
        path.path_length -= 2;
    }

    cache_path(path.path, path.path_length, FALSE);

    return 0;
}

static uint8_t add_mf_to_path(uint8_t *path, int length)
{
    uint8_t temp[MAX_PATH_LENGTH];

    if (!path || length == 0 ||
            (length + 2) >= MAX_PATH_LENGTH) {
        return 0;
    }

    if ( ((path[0] << 8) | path[1]) == 0x3F00 ) {
        return 0;
    }

    memset(temp, 0x0, sizeof(uint8_t) * MAX_PATH_LENGTH);

    temp[0] = 0x3F;
    temp[1] = 0x00;

    memmove(temp + 2, path, length);

    length += 2;
    memmove(path, temp, length);

    return 2;
}

static void clear_latest_flag()
{
    if ( !select_cache_p ) {
        return;
    }

    select_cache_p->wasLatestSelect = FALSE;
}

/**
 * @brief                  Check if this operation is a select and handle it.
 * @param ste_msg          Message
 * @param apdu             APDU
 * @return                 Status of the operation. Operation is Ok: 0; Operation failed: -1
 *
 * Check for SELECT and handles the different P1 and P2 commands, also
 * caches the path for further operations.
 *
 */
static int check_select(const uiccd_msg_sim_channel_send_t * ste_msg,
                        uint8_t *apdu, uint32_t apdu_len)
{
    sim_uicc_status_word_t  status_word = { 0x90, 0x00 };
    sim_uicc_status_code_t uicc_status_code = SIM_UICC_STATUS_CODE_OK;
    sim_uicc_status_code_fail_details_t uicc_status_code_fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
    ste_sim_ctrl_block_t   * ctrl_p = NULL;
    uiccd_trans_data_channel_send_t  * data_p = NULL;
    uint8_t max_retsize = 0;
    int failure = 0;
    int send_response = 1;

    if ( apdu[APDU_INS_BYTE] != 0xA4 ) {
        return 0; /* This is not a select, the request should not be cached */
    }

    // This is a SELECT command. The path should be cached until the next send operation.
    // In the following send requests, the cached path will be used.
    if (apdu[APDU_LC_BYTE] >= MAX_PATH_LENGTH ) {
        // Bad file path length (too long)
        catd_log_f(SIM_LOGGING_E, "%s: Invalid path length (%x)", __func__, apdu[APDU_LC_BYTE]);
        status_word.sw1 = 0x6A;
        status_word.sw2 = 0x82;  // File not found
        failure = 1;
    }

    // APDU contains a maximum return size
    if ( (apdu_len - 5) != apdu[APDU_LC_BYTE]) {
        max_retsize = apdu[apdu_len - 1];
    }
    // Check the P2 parameter to find out if a data response is expected at this time.
    // If no data included and P1 is 0 no data should be returned.
    if (apdu[APDU_P2_BYTE] != UICCD_SELECT_P2_COMMAND_NO_DATA_RETURN_USIM ||
            (apdu[APDU_LC_BYTE] != 0 &&
             apdu[APDU_P1_BYTE] != UICCD_SELECT_P1_COMMAND_DF_EF_MF_FILEID) ) {

        data_p = malloc(sizeof(uiccd_trans_data_channel_send_t));

        if ( !data_p ) {
            failure = 1;
        }

        if ( !failure ) {
            memset(data_p, 0, sizeof(uiccd_trans_data_channel_send_t));
            data_p->trans_state = UICCD_TRANS_CHANNEL_SEND_GET_FILE_INFORMATION;
            data_p->message_state = UICCD_SEND_MESSAGE_NORMAL;
            data_p->wasRelative = FALSE;
            data_p->max_return_size = max_retsize;
            data_p->CLA = apdu[APDU_CLA_BYTE];

            ctrl_p = ste_sim_create_ctrl_block(ste_msg->client_tag, ste_msg->fd, ste_msg->type, uiccd_channel_send_transaction_handler, data_p);

            if (!ctrl_p) {
                failure = 1;
            }
        }

        if ( !failure ) {
            sim_file_structure_t type = 0;
            /* Separate the file id from the path */
            data_p->file_id = (apdu[APDU_LC_BYTE+1] << 8) | apdu[APDU_LC_BYTE+2];
            data_p->path_length = apdu[APDU_LC_BYTE];
            memmove(data_p->path, &apdu[APDU_LC_BYTE + 1], data_p->path_length);

            switch (apdu[APDU_P1_BYTE]) {
                case UICCD_SELECT_P1_COMMAND_DF_EF_MF_FILEID:
                {
                    /* If SIM application, send from previous cached path
                     * if the file ID is other than MF */
                    if ( uicc_get_app_type() == STE_SIM_APP_TYPE_SIM &&
                        ((data_p->path[0] << 8) |
                             data_p->path[1]) != 0x3F00 ) {
                       if ( append_cached_path(data_p->path, data_p->path_length)
                            != 0 ) {
                        catd_log_f(SIM_LOGGING_E, "%s : Unable to append path",
                                  __func__);
                        failure = 1;
                        break;
                      }
                      data_p->wasRelative = TRUE;
                      break;
                    }

                    // Select by file ID
                    uiccd_channel_send_clear_cache();
                    data_p->path_length += add_mf_to_path(data_p->path, data_p->path_length);
                    cache_path(data_p->path, data_p->path_length, TRUE);
                    break;
                }
                case UICCD_SELECT_P1_COMMAND_CHILD_CURRENT_DF:
                {
                    // Select child DF of the current DF
                    if ( append_cached_path(data_p->path, data_p->path_length)
                            != 0 ) {
                        catd_log_f(SIM_LOGGING_E, "%s : Unable to append path",
                                __func__);
                        failure = 1;
                    }
                    data_p->wasRelative = TRUE;
                    break;
                }
                case UICCD_SELECT_P1_COMMAND_PARENT_CURRENT_DF:
                {
                    // Select parent DF of current DF
                    if ( parent_cached_path(data_p->path, data_p->path_length)
                            != 0 ) {
                        catd_log_f(SIM_LOGGING_E, "%s : Unable to traverse path",
                                __func__);
                        failure = 1;
                    }
                    break;
                }
                case UICCD_SELECT_P1_COMMAND_PATH_FROM_MF:
                {
                    // Select by path from MF
                    uiccd_channel_send_clear_cache();
                    data_p->path_length += add_mf_to_path(data_p->path, data_p->path_length);
                    if ( cache_path(data_p->path, data_p->path_length, TRUE) != 0 ) {
                        catd_log_f(SIM_LOGGING_E, "%s : Unable to cache path",
                                __func__);
                        failure = 1;
                    }
                    break;
                }
                case UICCD_SELECT_P1_COMMAND_PATH_CURRENT_DF:
                {
                    // Select by path from current DF
                    if ( append_cached_path(data_p->path, data_p->path_length)
                            != 0 ) {
                        catd_log_f(SIM_LOGGING_E, "%s : Unable to append path",
                                __func__);
                        failure = 1;
                    }
                    data_p->wasRelative = TRUE;
                    break;
                }
                default:
                    /* Incorrect P1-P2 */
                    status_word.sw1 = 0x6A;
                    status_word.sw2 = 0x86;
                    catd_log_f(SIM_LOGGING_D, "%s : Unhandled P1 command: %d",
                            __func__, apdu[APDU_P1_BYTE]);
                    failure = 1;
                    break;
            }
            if ( !select_cache_p ) {
                failure = 1;
            }
            if ( !failure && uiccd_get_file_information(ctrl_p,
                    select_cache_p->file_id,
                    select_cache_p->path,
                    select_cache_p->path_length, type) ) {
                catd_log_f(SIM_LOGGING_E, "%s : get file information failed!",
                                                __func__);
                failure = 1;
            }
            if ( !failure && (apdu[APDU_P2_BYTE] == 0x00 ||
                 apdu[APDU_P2_BYTE] == 0x0c) ) {
                data_p->message_state = UICCD_SEND_MESSAGE_STATUS_WORD;
                send_response = 0;
            } else if (!failure && apdu[APDU_P2_BYTE] == 0x04) {
                clear_latest_flag();
                send_response = 0;
            } else {
                failure = 1;
                send_response = 1;
            }
        }
    }

    if (failure) {
        uicc_status_code = SIM_UICC_STATUS_CODE_FAIL;
        uicc_status_code_fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR;
        free(data_p);
        free(ctrl_p);
    }

    if (failure || send_response) {

        // Send response
        // Note: If the path is incorrect for some other reason, then the
        // status word response to following APDU will reflect this.
        uiccd_encode_and_send_sim_channel_send_response(ste_msg->fd,
                                                        ste_msg->client_tag,
                                                        uicc_status_code,
                                                        uicc_status_code_fail_details,
                                                        status_word,
                                                        &status_word.sw1, 2);
    }
    return 1;
}

/**
 * @brief                  Check if this operation is a get response and handle it.
 * @param ste_msg          Message
 * @param apdu             APDU
 * @return                 Status of the operation. Operation is Ok: 0; Operation failed: -1
 *
 * Check for get response and use the cached path to supply a response.
 *
 */
static int check_get_response(const uiccd_msg_sim_channel_send_t * ste_msg,
                        uint8_t *apdu)
{
    sim_uicc_status_word_t  status_word = { 0x90, 0x00 };
    sim_uicc_status_code_t uicc_status_code = SIM_UICC_STATUS_CODE_OK;
    sim_uicc_status_code_fail_details_t uicc_status_code_fail_details =
            SIM_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
    ste_sim_ctrl_block_t   * ctrl_p = NULL;
    uiccd_trans_data_channel_send_t  * data_p = NULL;
    sim_file_structure_t type = 0;
    int failure = 0;

    if ( !select_cache_p || (apdu[APDU_INS_BYTE] != 0xC0 ||
         select_cache_p->wasLatestSelect == FALSE) ) {
        catd_log_f(SIM_LOGGING_D, "%s : Not get response", __func__);
        return 0; /* This is not a get response or nothing cached*/
    }

    data_p = malloc(sizeof(uiccd_trans_data_channel_send_t));

    if ( !data_p ) {
        failure = 1;
    }

    if ( !failure ) {
        memset(data_p, 0, sizeof(uiccd_trans_data_channel_send_t));
        data_p->trans_state = UICCD_TRANS_CHANNEL_SEND_GET_FILE_INFORMATION;
        data_p->req_size = apdu[APDU_LC_BYTE];
        data_p->message_state = UICCD_SEND_MESSAGE_GET_RESPONSE;
        data_p->wasRelative = FALSE;

        memmove(data_p->path, select_cache_p->path, select_cache_p->path_length);
        data_p->path_length = select_cache_p->path_length;
        data_p->file_id = select_cache_p->file_id;

        ctrl_p = ste_sim_create_ctrl_block(ste_msg->client_tag, ste_msg->fd, ste_msg->type, uiccd_channel_send_transaction_handler, data_p);

        if (!ctrl_p) {
            failure = 1;
        }
    }

    if ( !failure && uiccd_get_file_information(ctrl_p, data_p->file_id,
            (data_p->path_length) > 0 ? data_p->path : NULL,
                    data_p->path_length, type) ) {
        catd_log_f(SIM_LOGGING_E, "%s : Failed to get file info", __func__);
        uicc_status_code = SIM_UICC_STATUS_CODE_FAIL;
        uicc_status_code_fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR;
        failure = 1;
        free(data_p);
        free(ctrl_p);
        data_p = NULL;
    }
    if ( failure ) {
        uiccd_encode_and_send_sim_channel_send_response(ste_msg->fd,
                                                        ste_msg->client_tag,
                                                        uicc_status_code,
                                                        uicc_status_code_fail_details,
                                                        status_word,
                                                        &status_word.sw1, 2);
    }

    clear_latest_flag();
    return 1;
}

int uiccd_main_sim_channel_send(const ste_msg_t * ste_msg)
{
    ste_modem_t            *m;
    uiccd_msg_sim_channel_send_t *msg = (uiccd_msg_sim_channel_send_t *) ste_msg;
    ste_sim_ctrl_block_t   * ctrl_p;
    sim_uicc_status_word_t status_word = {0,0};
    uint8_t *data = NULL;
    uint8_t len = 0;

    catd_log_f(SIM_LOGGING_I, "uicc : SIM Channel Send message received from: %d ",msg->fd);

    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        uiccd_encode_and_send_sim_channel_send_response(msg->fd,
                                                        msg->client_tag,
                                                        SIM_UICC_STATUS_CODE_FAIL,
                                                        SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                        status_word,
                                                        NULL, 0);
        return -1;
    } else {
        int                     i;
        const char             *p = msg->data;
        const char             *p_max = msg->data + msg->len;
        uint16_t                session_id;
        uint32_t                apdu_len;
        uint8_t                *apdu;

        p = sim_dec(p, &session_id, sizeof(session_id), p_max);
        p = sim_dec(p, &apdu_len, sizeof(apdu_len), p_max);

        apdu = malloc(apdu_len);

        if ( apdu == NULL ) {
            uiccd_encode_and_send_sim_channel_send_response(msg->fd,
                                                            msg->client_tag,
                                                            SIM_UICC_STATUS_CODE_FAIL,
                                                            SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                            status_word,
                                                            NULL, 0);
            return -1;
        }

        p = sim_dec(p, apdu, apdu_len, p_max);

        if ( !p ) {
            // Something went wrong in one of the sim_dec above.
            uiccd_encode_and_send_sim_channel_send_response(msg->fd,
                                                            msg->client_tag,
                                                            SIM_UICC_STATUS_CODE_FAIL,
                                                            SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                            status_word,
                                                            NULL, 0);
            catd_log_f(SIM_LOGGING_E, "%s : uiccd_sig_sim_channel_send failed", __func__);
            free(apdu);
            return -1;
        }

        /* Check size of path in APDU */
        if ( apdu[APDU_INS_BYTE] == 0xA4 && ((apdu_len - 5) != apdu[APDU_LC_BYTE] &&
                (apdu_len - 6) != apdu[APDU_LC_BYTE]) ) {
            status_word.sw1 = 0x67;
            status_word.sw2 = 0x00;
            uiccd_encode_and_send_sim_channel_send_response(msg->fd,
                                                            msg->client_tag,
                                                            SIM_UICC_STATUS_CODE_FAIL,
                                                            SIM_UICC_STATUS_CODE_FAIL_DETAILS_INVALID_PARAMETERS,
                                                            status_word,
                                                            NULL, 0);
            catd_log_f(SIM_LOGGING_E, "%s : APDU length mismatch", __func__);
            free(apdu);
            return -1;
        }

        /* Check if the APDU is a select command, in that case it will be cached */
        if (session_id == 0 && check_select(msg, apdu, apdu_len)) {
            free(apdu);
            return 0;
        } else if (session_id == 0 && check_get_response(msg, apdu)) {
            free(apdu);
            return 0;
        }

        ctrl_p = ste_sim_create_ctrl_block(msg->client_tag, msg->fd, msg->type, uiccd_channel_send_transaction_handler, NULL);

        if (!ctrl_p) {
            uiccd_encode_and_send_sim_channel_send_response(msg->fd,
                                                            msg->client_tag,
                                                            SIM_UICC_STATUS_CODE_FAIL,
                                                            SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                            status_word,
                                                            NULL, 0);
            free(apdu);
            return -1;
        }

        if ( select_cache_p ) {
            if (select_cache_p->path_length > 0) {
                len = 0;
                data = calloc(1, select_cache_p->path_length + (sizeof(uint8_t) * 2));
                memmove(data, select_cache_p->path, select_cache_p->path_length);
                if ( select_cache_p->file_id != 0 ) {
                    data[select_cache_p->path_length] =
                            select_cache_p->file_id >> 8;
                    data[select_cache_p->path_length + 1] =
                            select_cache_p->file_id & 0xff;
                    len += sizeof(uint8_t) * 2;
                }
                len += select_cache_p->path_length;
            }
        }

        i = ste_modem_channel_send(m,
                               (uintptr_t)ctrl_p,
                               session_id,
                               apdu_len,
                               apdu,
                               data,
                               len);

        free(apdu);
        free(data);

        if (i != 0) {
            uiccd_encode_and_send_sim_channel_send_response(msg->fd,
                                                            msg->client_tag,
                                                            SIM_UICC_STATUS_CODE_FAIL,
                                                            SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                            status_word,
                                                            NULL, 0);
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_main_sim_channel_send failed");
            free(ctrl_p);
            return -1;
        }
    }

    return 0;
}
