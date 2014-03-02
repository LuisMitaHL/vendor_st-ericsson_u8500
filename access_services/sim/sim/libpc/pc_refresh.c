/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : pc_refresh.c
 * Description     : Handler function for proactive command refresh.
 *
 * Author          : Steve Critchlow <steve.critchlow@stericsson.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>

#include "pc_utilities.h"
#include "sim_internal.h"
#include "catd.h"
#include "catd_msg.h"
#include "apdu.h"
#include "sim_unused.h"
#include "func_trace.h"
#include "pc_handler.h"
#include "uicc_file_paths.h"
#include "state_machine.h"
#include "uicc_state_machine.h"

static ste_apdu_t                     * apdu_raw_p = NULL;
static ste_parsed_apdu_t              * apdu_parsed_p = NULL;
static ste_apdu_pc_refresh_t          * refresh_p = NULL;

// Used for checking if the EFrat file has been updated during a PC refresh
static const uint8_t RAT_Path[] = {0x3F,0x00,0x7F,0x66,0x5F,0x30,0x4F,0x36};
static const uint8_t RAT_Alt_Path[] = {0x3F,0x00,0x7F,0xFF,0x7F,0x66,0x5F,0x30,0x4F,0x36};

// Used for checking if SPN file is updated during a PC refresh
// Note this fix is specially for incorrect data for file path of SPN
static const uint8_t ef_spn_path_gsm[] = {0x3F,0x00,0x7F,0x20,0x6F,0x46};
static const uint8_t ef_spn_path_usim[] = {0x3F,0x00,0x7F,0xFF,0x6F,0x46};

/*************************************************************************
 * @brief:    clean everything after the PC is done
 * @params:
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
static void pc_refresh_data_reset()
{
    refresh_p = NULL;

    if (apdu_parsed_p) {
        ste_parsed_apdu_delete(apdu_parsed_p);
        apdu_parsed_p = NULL;
    }
    if (apdu_raw_p) {
        ste_apdu_delete(apdu_raw_p);
        apdu_raw_p = NULL;
    }
}

static void pc_refresh_clients_ind_with_files()
{
    ste_cat_pc_refresh_ind_t        refresh_ind;
    ste_apdu_file_name_t            *files_p;
    size_t                          i;

    // A refresh command with a file list
    catd_log_f(1, "catd : Refresh with file list! - send event to client(s)");

    refresh_ind.simd_tag    = 0;
    refresh_ind.type        = refresh_p->command_details.refresh_type;

    // Send the PC refresh event to registered clients.
    catd_send_pc_refresh_ind(&refresh_ind);

    // Also send the list of files that have changed.
    files_p = refresh_p->file_list.files_p;
    for ( i = 0 ; i < refresh_p->file_list.nr_of_files; i++, files_p++ )
    {
        ste_cat_pc_refresh_file_ind_t   refresh_file_ind = { 0, { 0, { 0 } } };
        size_t  j;
        refresh_file_ind.simd_tag = 0;
        refresh_file_ind.path.pathlen = files_p->path_len;

        for ( j = 0 ; j < files_p->path_len && j < SIM_MAX_PATH_LENGTH ; j++ )
        {
            refresh_file_ind.path.path[j] = files_p->file_path[j];
        }
        catd_send_pc_refresh_file_ind(&refresh_file_ind);

        /* If the EF Rat file has changed, send an RAT setting updated indication */
        if ((files_p->path_len == sizeof(RAT_Path) && !memcmp(files_p->file_path, RAT_Path, files_p->path_len)) ||
            (files_p->path_len == sizeof(RAT_Alt_Path) && !memcmp(files_p->file_path, RAT_Alt_Path, files_p->path_len))) {
            catd_send_rat_setting_updated_ind();
        }
    }
}


static void pc_refresh_clients_ind_without_files()
{
    ste_cat_pc_refresh_ind_t        refresh_ind;

    refresh_ind.simd_tag    = 0;
    refresh_ind.type        = refresh_p->command_details.refresh_type;

    // Send the PC refresh event to registered clients.
    catd_send_pc_refresh_ind(&refresh_ind);
}

/*************************************************************************
 * @brief:    check if the file path is correct or not based on the app type
 * @params:
 *
 * @return:   0: success, 1: fail
 *
 * Notes:
 *************************************************************************/
static int pc_refresh_file_path_integrity_check()
{
    ste_apdu_file_name_t  *files_p = NULL;
    int                    i;
    ste_sim_app_type_t     app_type;

    catd_log_f(SIM_LOGGING_D, "catd : pc_refresh_file_path_integrity_check: check for EF SPN.");
    //check the application type to see if it is 3G sim application
    app_type = uicc_get_app_type();
    files_p = refresh_p->file_list.files_p;
    if ((STE_SIM_APP_TYPE_USIM == app_type) && files_p)
    {
        for (i = 0; i < refresh_p->file_list.nr_of_files; i++)
        {
            //check if it is a 2G path for SPN file
            if (files_p->path_len == sizeof(ef_spn_path_gsm)
                && !memcmp(files_p->file_path, ef_spn_path_gsm, files_p->path_len))
            {
                catd_log_f(SIM_LOGGING_D, "catd : update EF SPN file path to usim file path.");
                memcpy(files_p->file_path, ef_spn_path_usim, files_p->path_len);
                break;
            }
            files_p++;
        }
    }

    return 0;
}

/*
 * Handle the "refresh" response from the modem.
 */
static int pc_refresh_response_handler(ste_msg_t* ste_msg)
{
    catd_msg_modem_refresh_status_t            * msg_p;
    int                                          status;
    ste_apdu_additional_info_t                   add_info;

    assert(ste_msg);

    msg_p = (catd_msg_modem_refresh_status_t*)ste_msg;

    //check the response for refresh result
    status = msg_p->i;

    add_info = SAT_RES_MEP_PROB_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;

    switch (refresh_p->command_details.refresh_type) {
        case STE_APDU_REFRESH_FILE_CHANGE:
        case STE_APDU_REFRESH_INIT_AND_FILE_CHANGE:
        case STE_APDU_REFRESH_3G_SESSION_RESET:
        {
            //the refresh is done, send TR to UICC
            if (status == 0) {
                pc_send_terminal_response_general(apdu_parsed_p, SAT_RES_CMD_PERFORMED_SUCCESSFULLY);
            }
            else {
                pc_send_terminal_response_with_add_info(apdu_parsed_p,
                                                        SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD,
                                                        &add_info);
            }
            //send indications to registered clients
            pc_refresh_clients_ind_with_files();
        }
        break;
        case STE_APDU_REFRESH_INIT_AND_FULL_FILE_CHANGE:
        case STE_APDU_REFRESH_INIT:
        case STE_APDU_REFRESH_3G_APP_RESET:
        {
            if (status == 0) {
                pc_send_terminal_response_general(apdu_parsed_p, SAT_RES_CMD_PERFORMED_SUCCESSFULLY);
            }
            else {
                pc_send_terminal_response_with_add_info(apdu_parsed_p,
                                                        SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD,
                                                        &add_info);
            }
            //inclusion of a File List is optional, and the terminal shall ignore it.
            //send the refresh indication only, no info about files
            pc_refresh_clients_ind_without_files();
        }
        break;
        case STE_APDU_REFRESH_UICC_RESET:
        {
            pc_refresh_clients_ind_without_files();
            //ask CATD to send the refresh request down to modem and wait for the response
            catd_cat_enable();

            //do the reset of CATD and UICCD internally, probably by send a internal reset message to CATD and UICCD
            //do not send TR to UICC
        }
        break;
        default:
        {
            catd_log_f(1, "catd : invalid refresh type");
        }
        break;
    }

    pc_refresh_data_reset();
    return 0;
}

static int pc_request_modem_refresh(ste_apdu_refresh_type_t refresh_type,
                                    ste_apdu_application_id_t * app_id_p,
                                    ste_apdu_file_list_t * file_list_p)
{
    ste_modem_t    *modem_p;
    int rv;
    ste_sim_ctrl_block_t* ctrl_p = NULL;

    modem_p = catd_get_modem();
    if (modem_p == NULL) {
        catd_log_f(0, "catd : Missing modem ready!");
        return -1;
    }

    ctrl_p = ste_sim_create_ctrl_block(CATD_CLIENT_TAG, CATD_FD, CATD_MSG_NONE, pc_refresh_response_handler, NULL);

    if (!ctrl_p) {
        catd_log_f(SIM_LOGGING_E, "catd : out of memory, could not do modem refresh!");
        return -1;
    }

    rv = ste_catd_modem_refresh_request(modem_p, (uintptr_t)ctrl_p, refresh_type, app_id_p, file_list_p);
    return rv;
}

/*************************************************************************
 * @brief:    handle the particular proactive command of refresh internally
 * @params:
 *            parsed_apdu_p: the parsed apdu structure.
 *            msg:           the original message with APDU data, and client_tag, etc. for possible future usage.
 *
 * @return:   the result of the operation, 0 = successs
 *
 * Notes:
 *************************************************************************/
static int pc_main_handle_refresh( ste_parsed_apdu_t    * parsed_apdu_p,
                                    catd_msg_apdu_t      * msg)
{
    ste_sat_apdu_error_t                   rv;
    ste_apdu_t                           * apdu = msg->apdu;
    ste_apdu_refresh_type_t                refresh_type = STE_APDU_REFRESH_RESERVED;
    ste_apdu_additional_info_t             add_info;
    cn_call_state_t                        call_state = CN_CALL_STATE_IDLE;

    add_info = SAT_RES_MEP_PROB_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;

    apdu_raw_p = ste_apdu_copy(apdu);  //save the raw apdu for possible future usage

    rv = ste_parsed_apdu_get(parsed_apdu_p, (void**)&refresh_p, STE_APDU_CMD_TYPE_REFRESH);
    if (rv != STE_SAT_APDU_ERROR_NONE) {
        catd_log_f(1, "catd : failed to fetch APDU data.");
        pc_send_terminal_response_error(rv, apdu);
        pc_refresh_data_reset();
        return -1;
    }

    if (!refresh_p) {
        catd_log_f(1, "catd : failed to fetch APDU data.");
        pc_send_terminal_response_error(STE_SAT_APDU_ERROR_CORRUPTED_DATA, apdu);
        pc_refresh_data_reset();
        return -1;
    }

    if ( refresh_p->bit_mask & PC_REFRESH_CommandDetails_present ) {
        refresh_type = refresh_p->command_details.refresh_type;
    }
    else {
        //the PC is not understandable
        catd_log_f(1, "catd : unable to understand the APDU for PC Refresh.");
        pc_send_terminal_response_error(STE_SAT_APDU_ERROR_CORRUPTED_DATA, apdu);
        pc_refresh_data_reset();
        return -1;
    }

    //check the state of UI, Ex., if call is onging, etc., to see if it is possible to proceed with Refresh
    //If the terminal receives a REFRESH command while in a state where execution of the command would be
    //unacceptable, upsetting the current user operation (e.g. notification during a call that the IMSI has changed), the
    //terminal shall inform the UICC using TERMINAL RESPONSE (terminal currently unable to process command -
    //currently busy on call) or TERMINAL RESPONSE (terminal currently unable to process command - screen is busy) as
    //appropriate.
    call_state = catd_get_current_call_state();
    if (call_state != CN_CALL_STATE_IDLE) {
        //busy on call, refresh cannot proceed.
        add_info = SAT_RES_MEP_CURRENTLY_BUSY_ON_CALL;
        pc_send_terminal_response_with_add_info(apdu_parsed_p,
                                                SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD,
                                                &add_info);
        pc_refresh_data_reset();
        return -1;
    }

    //To check the file path in the PC refresh command, if it is correct or not based on
    //the activated app type, this is to prevent some SIM cards of specific operator
    //from sending wrong data in the APDU.
    if (pc_refresh_file_path_integrity_check())
    {
        //the PC is not understandable
        catd_log_f(1, "catd : unable to understand file path for PC Refresh.");
        pc_send_terminal_response_error(STE_SAT_APDU_ERROR_CORRUPTED_DATA, apdu);
        pc_refresh_data_reset();
        return -1;
    }

    switch (refresh_type) {
        case STE_APDU_REFRESH_FILE_CHANGE:
        case STE_APDU_REFRESH_INIT_AND_FILE_CHANGE:
        case STE_APDU_REFRESH_3G_SESSION_RESET:
        {
            //the UICC shall supply a File List data object, indicating which EFs need to be refreshed.
            if ( refresh_p->bit_mask & PC_REFRESH_FileList_present ) {
                if ( refresh_p->bit_mask & PC_REFRESH_AID_present ) {
                    //send the refresh request down to modem and wait for the response
                    rv = pc_request_modem_refresh(refresh_type, &(refresh_p->app_id), &(refresh_p->file_list));
                }
                else {
                    //send the refresh request down to modem and wait for the response
                    rv = pc_request_modem_refresh(refresh_type, NULL, &(refresh_p->file_list));
                }
                if (rv != 0) {
                    pc_send_terminal_response_with_add_info(apdu_parsed_p,
                                                            SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD,
                                                            &add_info);
                    pc_refresh_data_reset();
                    return -1;
                }
            }
            else {
                catd_log_f(1, "catd : Error in APDU data.");
                pc_send_terminal_response_general(parsed_apdu_p, SAT_RES_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME);
                pc_refresh_data_reset();
                return -1;
            }
        }
        break;
        case STE_APDU_REFRESH_INIT_AND_FULL_FILE_CHANGE:
        case STE_APDU_REFRESH_INIT:
        case STE_APDU_REFRESH_3G_APP_RESET:
        {
            //inclusion of a File List is optional, and the terminal shall ignore it.
            if ( refresh_p->bit_mask & PC_REFRESH_AID_present ) {
                //send the refresh request down to modem and wait for the response
                rv = pc_request_modem_refresh(refresh_type, &(refresh_p->app_id), NULL);
            }
            else {
                //send the refresh request down to modem and wait for the response
                rv = pc_request_modem_refresh(refresh_type, NULL, NULL);
            }
            if (rv != 0) {
                pc_send_terminal_response_with_add_info(apdu_parsed_p,
                                                        SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD,
                                                        &add_info);
                pc_refresh_data_reset();
                return -1;
            }
        }
        break;
        case STE_APDU_REFRESH_UICC_RESET:
        {
            //if it is UICC reset, then NO TR is needed
            //do the reset and return without sending TR
            rv = pc_request_modem_refresh(refresh_type, NULL, NULL);

            if (rv != 0) {
                catd_log_f(1, "catd : failed to send the refresh request to modem.");
                pc_send_terminal_response_error(rv, apdu);
                pc_refresh_data_reset();
                return -1;
            }
            //CATD and UICCD should wait for the indication from modem to reset themselves
        }
        break;
        default:
        {
            catd_log_f(1, "catd : invalid refresh type");
            pc_refresh_data_reset();
            return -1;
        }
    }

    return 0;
}

/*************************************************************************
 * @brief:    handle the particular proactive command of refresh
 * @params:
 *            msg:         the original message with APDU data, and client_tag, etc.
 *
 * @return:   the result of the operation, 0 = successs
 *
 * Notes:
 *************************************************************************/
int catd_handle_pc_refresh(catd_msg_apdu_t  * msg)
{
    ste_sat_apdu_error_t                 rv;
    ste_apdu_t                         * apdu = msg->apdu;
    int                                  rc;

    pc_refresh_data_reset();
    rv = ste_apdu_parse(apdu, &apdu_parsed_p);

    if (rv != STE_SAT_APDU_ERROR_NONE)
    {
        catd_log_f(1, "catd : parse apdu failed");
        pc_send_terminal_response_error(rv, apdu);
        pc_refresh_data_reset();
        return -1;
    }
    rc = pc_main_handle_refresh(apdu_parsed_p, msg);

    return rc;
}

