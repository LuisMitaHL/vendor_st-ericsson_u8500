/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : pc_send_sms.c
 * Description     : Handler function for proactive command send short message.
 *
 * Author          : Haiyuan Bu <haiyuan.bu@stericsson.com>
 *                   Leif Simmons <leif.simmons@stericsson.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <alloca.h>

#include "pc_utilities.h"
#include "sim_internal.h"
#include "catd.h"
#include "catd_msg.h"
#include "apdu.h"
#include "sim_unused.h"
#include "func_trace.h"
#include "uicc_internal.h"
#include "uiccd.h"

#include "r_sms.h"

#ifndef MIN
#define MIN(x,y) ((x<y) ? x : y)
#endif

#ifndef MAX
#define MAX(x,y) ((x>y) ? x : y)
#endif


#ifndef TPDU_PRESENT_MASK
#define TPDU_PRESENT_MASK (PC_SEND_SMS_3GPP_SMS_TPDU_present | PC_SEND_SMS_CDMA_SMS_TPDU_present)
#endif

#define SMS_BIT_MASK_SMS_SUBMIT     0x01
#define SMS_BIT_MASK_VP_FORMAT      0x18

/*
 * This the struct to hold the stuff we need to stash away while performing asynchronous requests...
 */
typedef struct {
    ste_apdu_t*                       apdu_raw;     // Points to the original "rawer" APDU
    ste_parsed_apdu_t*                apdu_parsed;  // Points to the parsed APDU
    boolean                           icon_handled; // False if there is a icon and the client doesn't have a GUI
    ste_apdu_pc_send_short_message_t* ssm;          // Points to the ssm struct inside the parsed APDU
} pc_send_short_message_stash_t;

/*
 * Deletes the stash
 */
static void delete_stash(pc_send_short_message_stash_t* stash)
{
    catd_log_f(SIM_LOGGING_D, "catd/libpc : %s", __PRETTY_FUNCTION__);

    if (stash) {
        stash->ssm = NULL;

        if (stash->apdu_raw) {
            ste_apdu_delete(stash->apdu_raw);
        }
        if (stash->apdu_parsed) {
            ste_parsed_apdu_delete(stash->apdu_parsed);
        }

        /*
         * No need to deallocate stash->ssm. It points to the SSM in the parsed APDU,
         * which is de-allocated when the parsed apdu is de-allocated.
         *
         * However, lets reset the memory to have a good chance of early fail if
         * someone tries to access the memory after it has been free'd.
         */
        memset(stash, 0, sizeof(*stash));
        free(stash);
    }
}

/*
 * Send short message response handler.
 *
 * Should return zero to indicate that transaction is done, or non-zero if more work remains .
 *
 */
static int send_short_message_response(ste_msg_t* ste_msg)
{
    catd_msg_sms_cb_signal_t*      msg     = (catd_msg_sms_cb_signal_t*)ste_msg;
    ste_sim_ctrl_block_t*          ctrl_p; // Use _p naming convention to allow searching for ctrl_p.
    pc_send_short_message_stash_t* stash;
    SMS_RequestStatus_t            rs      = SMS_REQUEST_OK;
    SMS_Error_t                    ec      = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    uint8_t                        ref;

    catd_log_f(SIM_LOGGING_D, "catd/libpc : %s", __PRETTY_FUNCTION__);

    assert(msg);             // There must be a message
    assert(msg->signal);     // There must be a signal in the signal message

    /*
     * The client tag is actually a pointer to the control block we created when we set up the request.
     *
     * TODO: We should use a table to translate between transaction id and a data pointer instead of
     *       using the client tag as a pointer.
     */
    ctrl_p = (ste_sim_ctrl_block_t*)(msg->client_tag);
    assert(ctrl_p); // There must be a control block in the message

    /*
     *  The transaction data pointer is actually the pointer to the stash that we created
     *  when we set up the request.
     */
    stash = (pc_send_short_message_stash_t*)(ctrl_p->transaction_data_p);
    assert(stash);          // There must be a stash!
    assert(stash->apdu_parsed);    // There must be an APDU in the stash

    rs = Response_SMS_ShortMessageSend(msg->signal, &ref, &ec);
    if (rs == SMS_REQUEST_OK) {
        ste_apdu_general_result_t result  = (stash->icon_handled) ? SAT_RES_CMD_PERFORMED_SUCCESSFULLY : SAT_RES_CMD_PERFORMED_SUCC_WITHOUT_REQ_ICON;
        pc_send_terminal_response_general(stash->apdu_parsed, result);
    } else {
        ste_apdu_additional_info_t  add_info = SAT_RES_CC_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
        catd_log_f(SIM_LOGGING_E, "catd/libpc : Response_SMS_ShortMessageSend failed. rs = %d ec = %X", rs, ec);
        if(rs == SMS_REQUEST_FAILED_APPLICATION && ec == SMS_ERROR_INTERN_MO_SMS_CONTROL_BY_USIM_REJECTED) {
            add_info = SAT_RES_CC_ACTION_NOT_ALLOWED;
            pc_send_terminal_response_with_add_info(stash->apdu_parsed, SAT_RES_INTERACTION_WITH_SIM_CALL_CONTROL_PERM, &add_info);
        } else {
            pc_send_terminal_response_with_add_info(stash->apdu_parsed, SAT_RES_NETWORK_CURRENTLY_UNABLE, &add_info);
        }
    }

    delete_stash(stash);
    return 0;
}

/*
 *  Async setup send short message, should return the following:
 *      -1 if error
 *       0 ok, caller may send PC notification to client
 *       1 ok, caller must not PC send notification to client (will be taken care of later)
 */
static int setup_async_send_short_message(pc_send_short_message_stash_t* stash, ste_apdu_address_t* smsc)
{
    ste_sim_ctrl_block_t*   ctrl_p  = NULL; // Use _p naming convention to allow searching for ctrl_p.
    SMS_SMSC_Address_TPDU_t sms;
    uint16_t sms_offset_ud = 0;

    catd_log_f(SIM_LOGGING_D, "catd/libpc : %s", __PRETTY_FUNCTION__);

    assert(stash);
    assert(stash->ssm);

    if (smsc->dial_text_p && smsc->str_length > 0) {
        // Note: The dialing number is coded according to TS 31.102 table 4.4: Extended BCD coding
        //       The length in this context is the size of the coded data in the buffer (string).
        sms.ServiceCenterAddress.TypeOfAddress = smsc->ton_npi;
        sms.ServiceCenterAddress.Length = MIN(smsc->str_length+1, (size_t)SMS_MAX_PACKED_ADDRESS_LENGTH+1);
        memcpy(sms.ServiceCenterAddress.AddressValue, smsc->dial_text_p, smsc->str_length);

        if (stash->ssm->command_details.packing_required) {
            uint8_t* tp_p;
            tp_p = stash->ssm->tpdu.data_p;

            catd_log_f(SIM_LOGGING_D, "catd/libpc : %s - packing required indicated.", __PRETTY_FUNCTION__);

            if (*tp_p & SMS_BIT_MASK_SMS_SUBMIT) {
                uint8_t tp_da_length;
                uint8_t tp_vp_length = 0; // Not present
                uint8_t tp_udl;
                uint8_t *temp_packed_ud_p = NULL;
                uint8_t *tpdu_ud_p = NULL;
                uint8_t tudu_ud_len = 0;

                catd_log_b(SIM_LOGGING_D, "catd : unpacked SMTPDU = ", stash->ssm->tpdu.data_p, stash->ssm->tpdu.data_len);

                /** Determine length of TP-Validity-Period. */
                if ((*tp_p & SMS_BIT_MASK_VP_FORMAT) == 0x10) {
                    tp_vp_length = 1;
                } else if ((*tp_p & SMS_BIT_MASK_VP_FORMAT) == 0x08 ||
                        (*tp_p & SMS_BIT_MASK_VP_FORMAT) == 0x18) {
                    tp_vp_length = 7 ;
                }

                /** Move past TP-Destination-Address.  */
                tp_p++; // TP-Message-Reference
                tp_p++; // TP-Destination-Address (length in semi-octets)
                tp_da_length = *tp_p;
                tp_p++; // TP-Destination-Address (type)
                tp_p += ((tp_da_length / 2) + (tp_da_length % 2)); // Move past address fields
                tp_p++; // TP-Protocol-Identifier

                /** Change TP-Data-Coding-Scheme. */
                tp_p++; // TP-Data-Coding-Scheme
                if ((*tp_p & 0xC0) == 0x00) { // General Data Coding indication
                    *tp_p = (*tp_p & 0xF3); //Unset character set to GSM 7 bit default alphabet
                } else if ((*tp_p & 0xF0) == 0xF0) { // Data coding/message class
                    *tp_p = (*tp_p & 0xFB); //Unset character set to GSM 7 bit default alphabet
                } else {
                    catd_log_f(SIM_LOGGING_E, "catd/libpc : %s - Unsupported coding scheme for packing required (0x%X).",
                            __PRETTY_FUNCTION__, *tp_p);
                    goto Error;
                }

                /** Pack TP-User-Data. */
                tp_p += tp_vp_length + 1; // Move past TP-Validity-Period to TP-User-Data-Length
                tp_udl = *tp_p; // TP-User-Data-Length indicates nr of octets in TP-User-Data when 8bit
                sms_offset_ud = tp_p - stash->ssm->tpdu.data_p + 1;
                tpdu_ud_p = tp_p + 1;
                tudu_ud_len = tp_udl;

                catd_log_f(SIM_LOGGING_I, "catd/libpc : %s: TPDU user data len: %d.", __PRETTY_FUNCTION__, tudu_ud_len);

                temp_packed_ud_p = (uint8_t *)calloc(sizeof(uint8_t), tudu_ud_len);
                if (temp_packed_ud_p) {
                    uint8_t packed_octets = convert_GSM8bit_2_GSM7bit((uint8_t *)tp_p + 1,
                            (uint8_t *)temp_packed_ud_p, tudu_ud_len);

                    /** When changing TP-Data-Coding-Scheme to GSM 7 bit default alphabet
                     * TP-User-Data-Length indicates septets instead of octets(as for 8bit).
                     * In both cases the TP-User-Data-Length indicates the number of characters
                     * within the coded TP-User-Data therefore no update of the TP-User-Data-Length
                     * is required. */

                    sms.TPDU.Length = sms_offset_ud + MIN(packed_octets, (size_t)SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH);
                    memcpy(sms.TPDU.Data, stash->ssm->tpdu.data_p, sms_offset_ud ); // Copy the TPDU till including User Data Len
                    memcpy(sms.TPDU.Data + sms_offset_ud, temp_packed_ud_p, packed_octets );

                    free((void *)temp_packed_ud_p);
                } else {
                    catd_log_f(SIM_LOGGING_E, "catd/libpc : %s - Failed to calloc.", __PRETTY_FUNCTION__);
                    goto Error;
                }
            } else {
                catd_log_f(SIM_LOGGING_E, "catd/libpc : %s - Invalid SMS type, packing required.", __PRETTY_FUNCTION__);
                goto Error;
            }
        } else {

            // TPDU data pointer and length sanity checks have already been made...
            sms.TPDU.Length = MIN(stash->ssm->tpdu.data_len, (size_t)SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH);
            memcpy(sms.TPDU.Data, stash->ssm->tpdu.data_p, sms.TPDU.Length);
        }

        ctrl_p = ste_sim_create_ctrl_block(CATD_CLIENT_TAG, // ignored for SMS/CB handling
                                           CATD_FD,         // ignored for SMS/CB handling
                                           CATD_MSG_NONE,   // ignored for SMS/CB handling
                                           send_short_message_response,
                                           stash);
        if (ctrl_p) {
            SMS_RequestStatus_t       rs  = SMS_REQUEST_OK;
            SMS_Error_t               ec  = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
            uint8_t                   ref = 0;
            SMS_RequestControlBlock_t rcb = { SMS_NO_WAIT_MODE, (uint32_t)ctrl_p, catd_get_sms_cb_request_socket() };

            catd_log_f(SIM_LOGGING_D, "catd : SMSCA TON = 0x%02X", sms.ServiceCenterAddress.TypeOfAddress);
            catd_log_b(SIM_LOGGING_D, "catd : SMSCA = ", sms.ServiceCenterAddress.AddressValue, sms.ServiceCenterAddress.Length);
            catd_log_b(SIM_LOGGING_D, "catd : packed SMTPDU = ", sms.TPDU.Data, sms.TPDU.Length);

            rs = Request_SMS_SatShortMessageSend(&rcb, &sms, NULL, FALSE, &ref, &ec);
            if (rs == SMS_REQUEST_PENDING) {
                catd_pc_send_pc_notification_ind(stash->apdu_raw); // Notify client, if any...
                return 1;
            }
        }
    }

    // Error handling & clean up.
    Error:
        catd_log_f(SIM_LOGGING_E, "catd/libpc : %s - error detected.", __PRETTY_FUNCTION__);
        pc_send_terminal_response_error(STE_SAT_APDU_ERROR_UNKNOWN, stash->apdu_raw);
        delete_stash(stash); // delete stash, de-allocate raw & parsed APDUs
        if (ctrl_p) {
            free(ctrl_p);
        }
        return -1;
}

/*
 * Read SMSC response handler.
 *
 * Should return zero to indicate that transaction is done, or non-zero if more work remains .
 *
 */
static int read_smsc_response(ste_msg_t* ste_msg)
{
    catd_msg_uiccd_response_t*     msg    = (catd_msg_uiccd_response_t*)ste_msg;
    ste_sim_ctrl_block_t*          ctrl_p = NULL; // Use _p naming convention to allow searching for ctrl_p.
    pc_send_short_message_stash_t* stash  = NULL;

    catd_log_f(SIM_LOGGING_D, "catd/libpc : %s", __PRETTY_FUNCTION__);
    assert(msg);

    ctrl_p = (ste_sim_ctrl_block_t*)(msg->client_tag);
    assert(ctrl_p); // There must be a control block in the message

    stash = (pc_send_short_message_stash_t*)(ctrl_p->transaction_data_p);
    assert(stash);

    if (msg->command == STE_UICC_RSP_READ_SMSC) {
        if ((msg->response != NULL) && (msg->response_len >  0)) {
            ste_sim_call_number_t                   scn;
            ste_sim_short_message_address_t         ssma;
            int                                     rc = 0;
            const char                             *p = msg->response;
            const char                             *p_max = msg->response + msg->response_len;
            sim_uicc_status_code_t                  uicc_status_code;
            sim_uicc_status_code_fail_details_t     uicc_status_code_fail_details;
            sim_uicc_status_word_t                  status_word;
            int                                     status_len = sizeof(uicc_status_code) +
                                                                 sizeof(uicc_status_code_fail_details) +
                                                                 sizeof(status_word.sw1) +
                                                                 sizeof(status_word.sw2);

            // split msg_data into separate parameters
            p = sim_dec(p, &uicc_status_code, sizeof(uicc_status_code), p_max);
            p = sim_dec(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details), p_max);
            p = sim_dec(p, &status_word.sw1, sizeof(status_word.sw1), p_max);
            p = sim_dec(p, &status_word.sw2, sizeof(status_word.sw2), p_max);

            memset(&scn, 0, sizeof(scn));

            if (msg->response_len > status_len) {
                sim_decode_smsc_string(&scn, p, msg->response_len - status_len); // Allocates memory: scn.num_text.text_p
            }

            rc = sim_undo_create_ril_smsc_str(&ssma, &scn); // Can handle scn.num_text.text_p == NULL
            free(scn.num_text.text_p); // Must de-allocate scn.num_text.text_p

            if (!rc) {
                ste_apdu_address_t smsc;
                smsc.ton_npi = ssma.addr_type;
                smsc.str_length = ssma.length;
                smsc.dial_text_p = ssma.addr_value;

                /*
                 * Negation of return value from setup_async_send_short_message needed to
                 * make it semantically match the value we should return.
                 */
                return !setup_async_send_short_message(stash, &smsc);
            }

            if (uicc_status_code != SIM_UICC_STATUS_CODE_OK) {
                catd_log_f(SIM_LOGGING_E, "catd/libpc : READ SMSC failed with "
                                          "status code: %d"
                                          "status code details: %d"
                                          "status word 1: %d"
                                          "status word 2: %d",
                                          uicc_status_code,
                                          uicc_status_code_fail_details,
                                          status_word.sw1,
                                          status_word.sw2);
            }
        }
    }

    // Error handling & clean up.
    catd_log_f(SIM_LOGGING_E, "catd/libpc : %s - error detected. ", __PRETTY_FUNCTION__);
    pc_send_terminal_response_error(STE_SAT_APDU_ERROR_UNKNOWN, stash->apdu_raw);
    delete_stash(stash); // deallocated raw & parsed APDUs
    return 1;
}

/*
 *  Async setup read SMSC, should return the following:
 *      -1 if error
 *       0 ok, caller may send PC notification to client
 *       1 ok, caller must not PC send notification to client (will be taken care of later)
 */
static int setup_async_read_smsc(pc_send_short_message_stash_t* stash)
{
    ste_sim_ctrl_block_t* ctrl_p = NULL; // Use _p naming convention to allow searching for ctrl_p.
    int socket;

    catd_log_f(SIM_LOGGING_D, "catd/libpc : %s", __PRETTY_FUNCTION__);

    assert(stash);

    socket = catd_get_uiccd_request_socket();
    if (socket >= 0) {
        ctrl_p = ste_sim_create_ctrl_block(CATD_CLIENT_TAG, // ignored
                                           CATD_FD,         // ignored
                                           CATD_MSG_NONE,   // ignored
                                           read_smsc_response,
                                           stash);
        if (ctrl_p) {
            const char* dummy = __PRETTY_FUNCTION__;
            uiccd_sig_read_smsc(socket, (uintptr_t)ctrl_p, dummy, strlen(dummy));
            return 1;
        }
    }

    // Error handling & clean up.
    catd_log_f(SIM_LOGGING_E, "catd/libpc : %s - error detected. ", __PRETTY_FUNCTION__);
    pc_send_terminal_response_error(STE_SAT_APDU_ERROR_UNKNOWN, stash->apdu_raw);
    delete_stash(stash); // delete stash, de-allocate raw & parsed APDUs

    // No need to free ctrl_p, if we get here, then it is NULL...
    // Make sure this holds true in future changes, or free ctrl_p here!

    return -1;
}

/*
 * Connect to SMM/CB response handler.
 *
 * Should return zero to indicate that transaction is done, or non-zero if more work remains .
 *
 */
static int connect_to_sms_cb_response(ste_msg_t* ste_msg)
{
    catd_msg_sms_cb_perform_connect_t* msg     = (catd_msg_sms_cb_perform_connect_t*)ste_msg;
    ste_sim_ctrl_block_t*              ctrl_p; // Use _p naming convention to allow searching for ctrl_p.
    pc_send_short_message_stash_t*     stash;
    int                                socket  = catd_get_sms_cb_request_socket();

    catd_log_f(SIM_LOGGING_D, "catd/libpc : %s", __PRETTY_FUNCTION__);

    assert(msg);

    ctrl_p = (ste_sim_ctrl_block_t*)(msg->client_tag);
    assert(ctrl_p);

    stash = (pc_send_short_message_stash_t*)(ctrl_p->transaction_data_p);
    assert(stash);
    assert(stash->ssm);

   if (socket >= 0) // Connected to SMS/CB...
   {
       if (stash->ssm->bit_mask & PC_SEND_SMS_Address_present) {
           // Can send SMS, have SMSC
           return !setup_async_send_short_message(stash, &(stash->ssm->address));
       } else {
           // Connected to SMS/CB, but have to read SMSC from uiccd before sending SMS
           return !setup_async_read_smsc(stash);
       }
   }

   // Error handling & clean up.
   catd_log_f(SIM_LOGGING_E, "catd/libpc : %s - error detected. ", __PRETTY_FUNCTION__);
   pc_send_terminal_response_general(stash->apdu_parsed, SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD);
   delete_stash(stash); // de-allocate raw & parsed APDUs
   return 0;
}

/*
 *  Async setup connect to SMS/CB, should return the following:
 *      -1 if error
 *       0 ok, caller may send PC notification to client
 *       1 ok, caller must not PC send notification to client (will be taken care of later)
 */
static int setup_async_connect_to_sms_cb(pc_send_short_message_stash_t* stash)
{
    ste_sim_ctrl_block_t* ctrl_p = NULL; // Use _p naming convention to allow searching for ctrl_p.
    int socket = catd_get_sms_cb_request_socket();

    catd_log_f(SIM_LOGGING_D, "catd/libpc : %s", __PRETTY_FUNCTION__);

    assert(stash);

    if (socket < 0) {
        ctrl_p = ste_sim_create_ctrl_block(CATD_CLIENT_TAG, // ignored
                                           CATD_FD,         // ignored
                                           CATD_MSG_NONE,   // ignored
                                           connect_to_sms_cb_response,
                                           stash);
        if (ctrl_p) {
            catd_sig_sms_cb_perform_connect((uintptr_t)ctrl_p);
            return 1;
        }
    }

    // Error handling & clean up.
    catd_log_f(SIM_LOGGING_E, "catd/libpc : %s - error detected. ", __PRETTY_FUNCTION__);
    pc_send_terminal_response_error(STE_SAT_APDU_ERROR_INTERNAL_IMPLEMENTATION, stash->apdu_raw);
    delete_stash(stash); // de-allocate raw & parsed APDUs
    return -1;
}

/*
 * The "real" implementation of catd_handle_pc_send_short_message()
 */
static int pc_main_handle_send_short_message(catd_msg_apdu_t* msg, boolean gui_capable_client)
{
    pc_send_short_message_stash_t* stash = NULL;
    ste_apdu_t*                    apdu  = NULL;
    ste_sat_apdu_error_t           rc    = STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;

    catd_log_f(SIM_LOGGING_D, "catd/libpc : %s", __PRETTY_FUNCTION__);

    assert(msg);
    assert(msg->apdu);

    apdu = msg->apdu;
    msg->apdu = NULL; // Prevent framework from deleting the raw APDU when we exit

    stash = malloc(sizeof(*stash));
    if (stash) {
        stash->icon_handled = TRUE;
        stash->ssm          = NULL;
        stash->apdu_raw     = apdu;
        stash->apdu_parsed  = NULL;
        rc = ste_apdu_parse(stash->apdu_raw, &stash->apdu_parsed); // Allocates memory
    }

    if (rc == STE_SAT_APDU_ERROR_NONE) {
        rc = ste_parsed_apdu_get(stash->apdu_parsed, (void**)&stash->ssm, STE_APDU_CMD_TYPE_SEND_SHORT_MESSAGE); // Doesn't allocate memory
        if (rc == STE_SAT_APDU_ERROR_NONE)
        {
            uint16_t bit_mask = stash->ssm->bit_mask;
            if (bit_mask && PC_SEND_SMS_IconIdentifier_present)
            {
                stash->icon_handled = gui_capable_client;
            }

            // Check that all mandatory things are there
            if ((bit_mask & TPDU_PRESENT_MASK)  &&
                (stash->ssm->tpdu.data_len > 0) &&
                (stash->ssm->tpdu.data_p)        ) {

                int socket = catd_get_sms_cb_request_socket();
                if (socket < 0) {
                    // Need to connect to SMS/CB before sending SMS
                    return setup_async_connect_to_sms_cb(stash);
                } else if (bit_mask & PC_SEND_SMS_Address_present) {
                    // Can send SMS, have SMSC
                    return setup_async_send_short_message(stash, &(stash->ssm->address));
                } else {
                    // Connected to SMS/CB, but have to read SMSC from uiccd before sending SMS
                    return setup_async_read_smsc(stash);
                }
            }
            rc = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
        }
    }

    // Error handling & clean up.
    catd_log_f(SIM_LOGGING_E, "catd/libpc : %s - error detected.", __PRETTY_FUNCTION__);
    pc_send_terminal_response_error(rc, apdu);
    delete_stash(stash); // de-allocate raw & parsed APDUs
    return -1;
}

/*
 * See pc_handler.h
 */
int catd_handle_pc_send_short_message(catd_msg_apdu_t* msg, boolean gui_capable_client)
{
    catd_log_f(SIM_LOGGING_D, "catd/libpc : %s", __PRETTY_FUNCTION__);

    assert(msg);
    return pc_main_handle_send_short_message(msg, gui_capable_client);
}


