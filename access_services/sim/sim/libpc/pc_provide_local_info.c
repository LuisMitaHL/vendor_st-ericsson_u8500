/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : pc_provide_local_info.c
 * Description     : Handler function for proactive command provide local info.
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
#include <dirent.h>
#include <time.h>

#include "pc_utilities.h"
#include "sim_internal.h"
#include "catd.h"
#include "catd_msg.h"
#include "apdu.h"
#include "sim_unused.h"
#include "func_trace.h"

#include "cops.h"
#include "catd_cn_client.h"
#include "cn_client.h"

#define POWER_SUPPLY_PATH "/sys/class/power_supply"

typedef struct {
    char* present;
    char* capacity;
} ste_battery_paths;

static ste_battery_paths battery_paths = { NULL, NULL };

//To save the NMR related info from CN, and use it in PLI for NMR
typedef struct {
    ste_apdu_t     * apdu_p;
    cn_nmr_info_t  * cn_nmr_p;
    cn_cell_info_t * cn_cell_info_p;
} catd_pc_pli_nmr_info_t;

static catd_pc_pli_nmr_info_t pli_nmr_info;

static int dec2bcd(int decimal);

/*************************************************************************
 * @brief:    handle the batery of proactive command provide_local_info
 * @params:
 *            parsed_apdu_p: Parsed apdu from the original apdu data
 *
 * @return:   ste_sat_apdu_error_t
 *
 * Notes:
 *************************************************************************/
static ste_sat_apdu_error_t pc_handle_battery(ste_parsed_apdu_t   * parsed_apdu_p);

/*************************************************************************
 * @brief:    handle the IMEI of proactive command provide_local_info
 * @params:
 *            parsed_apdu_p: Parsed apdu from the original apdu data
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
static ste_sat_apdu_error_t pc_handle_imei(ste_parsed_apdu_t   * parsed_apdu_p);

/*************************************************************************
 * @brief:    handle the date, time and timezone of proactive command provide_local_info
 * @params:
 *            parsed_apdu_p: Parsed apdu from the original apdu data
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
static ste_sat_apdu_error_t pc_handle_date_time_timezone(ste_parsed_apdu_t   * parsed_apdu_p);

/*************************************************************************
 * @brief:    handle the IMEISV of proactive command provide_local_info
 * @params:
 *            parsed_apdu_p:  Parsed apdu from the original apdu data
 *
 * @return:   ste_sat_apdu_error_t
 *
 * Notes:
 *************************************************************************/
static ste_sat_apdu_error_t pc_handle_imeisv(ste_parsed_apdu_t   * parsed_apdu_p);
/*************************************************************************
 * @brief:    handle the location of proactive command provide_local_info
 * @params:   raw_apdu_p:     Original apdu received from card
 *
 * @return:   ste_sat_apdu_error_t
 *
 * Notes:
 *************************************************************************/
static ste_sat_apdu_error_t pc_handle_location(ste_apdu_t *raw_apdu_p);

/**
 * @brief Handle the network search mode variant of provide local information
 *
 * @params raw_apdu Pointer to unparsed APDU as received from the SAT Application
 *
 * @return STE_SAT_APDU_ERROR_NONE if all is well, any other value on error.
 */
static ste_sat_apdu_error_t pc_handle_search_mode(ste_apdu_t* raw_apdu);

/**
 * @brief Handle the access technology variant of provide local information
 *
 * @params raw_apdu Pointer to unparsed APDU as received from the SAT Application
 *
 * @return STE_SAT_APDU_ERROR_NONE if all is well, any other value on error.
 */
static ste_sat_apdu_error_t pc_handle_access_technology(ste_apdu_t* raw_apdu);

/**
 * @brief Asyncronous response handler for request cell info in pc_handle_access_technology().
 */
static int pc_handle_access_technology_cn_response(ste_msg_t *ste_msg);

/**
 * @brief Asyncronous response handler for cn_request_registration_state_normal.
 */
static int pc_handle_search_mode_cn_response_handler(ste_msg_t *ste_msg);

/**
 * @brief Handle the timing advance variant of provide local information
 *
 * @params raw_apdu Pointer to unparsed APDU as received from the SAT Application
 *
 * @return STE_SAT_APDU_ERROR_NONE if all is well, any other value on error.
 */
static ste_sat_apdu_error_t pc_handle_timing_advance(ste_apdu_t* raw_apdu);

/**
 * @brief Asyncronous response handler for pc_handle_timing_advance().
 */
static int pc_handle_timing_advance_cn_response_handler(ste_msg_t *ste_msg);

/**
 * @brief Handle the Network Measurement Result variant of provide local information
 *
 * @params raw_apdu Pointer to unparsed APDU as received from the SAT Application
 *
 * @return STE_SAT_APDU_ERROR_NONE if all is well, any other value on error.
 */
static ste_sat_apdu_error_t pc_handle_nmr(ste_apdu_t* raw_apdu, cn_nmr_rat_type_t nmr_rat_type, uint8_t nmr_type);

/**
 * @brief Asyncronous response handler for pc_handle_nmr().
 */
static int pc_handle_nmr_cn_response_handler(ste_msg_t *ste_msg);

/**
 * @brief Asyncronous response handler for request cell info in pc_handle_nmr().
 */
static int pc_handle_cell_info_cn_response_handler(ste_msg_t *ste_msg);

/*************************************************************************
 * @brief:    handle the particular proactive command of provide local info internally
 * @params:
 *            parsed_apdu_p: the parsed apdu structure.
 *            msg:           the original message with APDU data, and client_tag, etc. for possible future usage.
 *
 * @return:   ste_sat_apdu_error_t
 *
 * Notes:
 *************************************************************************/
static void
pc_main_handle_provide_local_info( ste_parsed_apdu_t   * parsed_apdu_p,
                                   catd_msg_apdu_t     * msg)
{
    ste_sat_apdu_error_t                   rv;
    ste_apdu_provide_local_info_type_t     info_type = STE_SAT_PROVIDE_LOCAL_INFO_TYPE_END;
    ste_apdu_pc_provide_local_info_t     * pli_p;

    rv = ste_parsed_apdu_get(parsed_apdu_p, (void**)&pli_p, STE_APDU_CMD_TYPE_PROVIDE_LOCAL_INFO);
    if (rv != STE_SAT_APDU_ERROR_NONE)
    {
        catd_log_f(SIM_LOGGING_E, "catd : failed to fetch APDU data.");
        pc_send_terminal_response_error(rv, msg->apdu);
        return;
    }
    if (pli_p->bit_mask & PC_PROVIDE_LOCAL_INFO_Type_present)
    {
        info_type = pli_p->info_type;
    }
    else
    {
        catd_log_f(SIM_LOGGING_E, "catd : Error in APDU data.");
        pc_send_terminal_response_error(rv, msg->apdu);
        return;
    }

    switch (info_type)
    {
        case STE_SAT_PROVIDE_LOCAL_INFO_TYPE_LOCATION_INFO:
        {
            rv = pc_handle_location(msg->apdu);
        }
        break;
        case STE_SAT_PROVIDE_LOCAL_INFO_TYPE_ME_IMEI:
        {
            rv = pc_handle_imei(parsed_apdu_p);
        }
        break;
        case STE_SAT_PROVIDE_LOCAL_INFO_TYPE_ME_IMEISV:
        {
            rv = pc_handle_imeisv(parsed_apdu_p);
        }
        break;
        case STE_SAT_PROVIDE_LOCAL_INFO_TYPE_NW_MEASURE_RESULT:
        {
            rv = pc_handle_nmr(msg->apdu, pli_p->nmr_rat_type, pli_p->nmr_utran_type);
        }
        break;
        case STE_SAT_PROVIDE_LOCAL_INFO_TYPE_DATE_TIME_ZONE:
        {
            rv = pc_handle_date_time_timezone(parsed_apdu_p);
        }
        break;
        case STE_SAT_PROVIDE_LOCAL_INFO_TYPE_LANGUAGE:
        {
            catd_log_f(SIM_LOGGING_D, "catd : %s NOT SUPPORTED - language", __func__);
            rv = STE_SAT_APDU_ERROR_INTERNAL_IMPLEMENTATION;
        }
        break;
        case STE_SAT_PROVIDE_LOCAL_INFO_TYPE_TIMING_ADVANCE:
        {
            rv = pc_handle_timing_advance(msg->apdu);
        }
        break;
        case STE_SAT_PROVIDE_LOCAL_INFO_TYPE_ACCESS_TECH:
        {
            rv = pc_handle_access_technology(msg->apdu);
        }
        break;
        case STE_SAT_PROVIDE_LOCAL_INFO_TYPE_BATTERY:
        {
            rv = pc_handle_battery(parsed_apdu_p);
        }
        break;
        case STE_SAT_PROVIDE_LOCAL_INFO_TYPE_SEARCH_MODE:
        {
            rv = pc_handle_search_mode(msg->apdu);
        }
        break;
        default:
        {
            catd_log_f(SIM_LOGGING_E, "catd : %s : unsupported info type=%d", __func__, info_type);
            rv = STE_SAT_APDU_ERROR_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
        }
    }

    if (rv != STE_SAT_APDU_ERROR_NONE) {
        pc_send_terminal_response_error(rv, msg->apdu);
        return;
    }
}

/*************************************************************************
 * @brief:    handle the particular proactive command of provide local info
 * @params:
 *            msg:         the original message with APDU data, and client_tag, etc.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
void catd_handle_pc_provide_local_info(catd_msg_apdu_t  * msg)
{
    ste_sat_apdu_error_t                 rv;
    ste_apdu_t                         * apdu = msg->apdu;
    ste_parsed_apdu_t                  * parsed_apdu_p = NULL;

    rv = ste_apdu_parse(apdu, &parsed_apdu_p);

    if (rv == STE_SAT_APDU_ERROR_NONE) {
        pc_main_handle_provide_local_info(parsed_apdu_p, msg);
    } else {
        pc_send_terminal_response_error(rv, apdu);
    }
    ste_parsed_apdu_delete(parsed_apdu_p);
}

int readFromFile(const char* path, char* buf, size_t size)
{
    size_t count = 0;

    if (!path) {
        return -1;
    }

    int fd = open(path, O_RDONLY, 0);
    if (fd == -1) {
        catd_log_f(SIM_LOGGING_E, "catd : %s : Could not open %s", __func__, path);
        return -1;
    }

    count = read(fd, buf, size);
    if (count > 0) {
        count = (count < size) ? count : size - 1;
        while (count > 0 && buf[count-1] == '\n') count--;
        buf[count] = '\0';
    } else {
        buf[0] = '\0';
    }

    close(fd);
    return count;
}

int getBatteryPaths()
{
    char path[256];
    char buf[20];
    struct dirent* entry;
    int length = 0;

    DIR* dir = opendir(POWER_SUPPLY_PATH);
    if (dir == NULL) {
        catd_log_f(SIM_LOGGING_E, "catd : %s : Could not open %s", __func__, POWER_SUPPLY_PATH);
        return -1;
    }
    while ((entry = readdir(dir))) {
        const char* name = entry->d_name;

        // ignore "." and ".."
        if (name[0] == '.' && (name[1] == 0 || (name[1] == '.' && name[2] == 0))) {
            continue;
        }

        // Look for "type" file in each subdirectory
        snprintf(path, sizeof(path), "%s/%s/type", POWER_SUPPLY_PATH, name);
        length = readFromFile(path, buf, sizeof(buf));
        if (length > 0) {
            if (buf[length - 1] == '\n') {
                buf[length - 1] = 0;
            }

            if (strcmp(buf, "Battery") == 0) {
                snprintf(path, sizeof(path), "%s/%s/present", POWER_SUPPLY_PATH, name);
                if (access(path, R_OK) == 0) {
                    battery_paths.present = (char *)calloc(1, strlen(path) + 1);
                    strcpy(battery_paths.present, path);
                }
                snprintf(path, sizeof(path), "%s/%s/capacity", POWER_SUPPLY_PATH, name);
                if (access(path, R_OK) == 0) {
                    battery_paths.capacity = (char *)calloc(1, strlen(path) + 1);
                    strcpy(battery_paths.capacity, path);
                }
            }
        }
    }
    closedir(dir);

    if (!battery_paths.present)
        catd_log_f(SIM_LOGGING_E, "catd : %s : present path not found", __func__);
    if (!battery_paths.capacity)
        catd_log_f(SIM_LOGGING_E, "catd : %s : capacity path not found", __func__);

    return 0;
}

static ste_sat_apdu_error_t pc_handle_battery(ste_parsed_apdu_t   * parsed_apdu_p)
{
    ste_apdu_t                  *tr_apdu_p;
    ste_command_result_t        cmd_result;
    uint8_t                     state = STE_SAT_PROVIDE_LOCAL_INFO_BATTERY_STATE_UNKNOWN;
    ste_sat_apdu_error_t        rv;
    uint8_t                     present;
    int                         capacity;
    size_t                      len = 5;
    char                        buf[len];

    catd_log_f(SIM_LOGGING_D, "catd : %s : ENTER", __func__);

    if (!battery_paths.present || !battery_paths.capacity) {
        getBatteryPaths();
    }

    if (readFromFile(battery_paths.present, buf, len) > 0) {
        present = atoi(buf);
        if (present == 1) {
            if (readFromFile(battery_paths.capacity, buf, len) > 0) {
                capacity = atoi(buf);
                catd_log_f(SIM_LOGGING_D, "catd : %s : Battery capacity = %d%", __func__, capacity);
                //Best effort mapping to Android default battery indication levels(GUI)
                //Ref. frameworks/base/core/res/res/drawable/stat_sys_battery.xml for details
                if (capacity < 0 || capacity > 100) {
                    //Error
                    state = STE_SAT_PROVIDE_LOCAL_INFO_BATTERY_STATE_UNKNOWN;
                } else if (capacity < 15) {
                    state = STE_SAT_PROVIDE_LOCAL_INFO_BATTERY_STATE_VERY_LOW;
                } else if (capacity < 30) {
                    state = STE_SAT_PROVIDE_LOCAL_INFO_BATTERY_STATE_LOW;
                } else if (capacity < 50) {
                    state = STE_SAT_PROVIDE_LOCAL_INFO_BATTERY_STATE_AVERAGE;
                } else if (capacity < 90) {
                    state = STE_SAT_PROVIDE_LOCAL_INFO_BATTERY_STATE_GOOD;
                } else {
                    state = STE_SAT_PROVIDE_LOCAL_INFO_BATTERY_STATE_FULL;
                }
            }
        } else {
            state = STE_SAT_PROVIDE_LOCAL_INFO_BATTERY_STATE_NO_BATTERY;
        }
    }

    catd_log_f(SIM_LOGGING_D, "catd : %s : battery charge state=0x%X", __func__, state);

    //fill the data into the result structure
    cmd_result.general_result = SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
    cmd_result.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
    cmd_result.other_data_type = STE_CMD_RESULT_BATTERY;
    cmd_result.other_data.battery_state = state;

    //fill and initiate additional data fields
    cmd_result.additional_info_p = NULL;
    cmd_result.additional_info_size = 0;

    rv = ste_apdu_terminal_response(parsed_apdu_p, &cmd_result, &tr_apdu_p);
    if (rv != STE_SAT_APDU_ERROR_NONE)
    {
        catd_log_f(SIM_LOGGING_E, "catd : %s : Failed to create terminal response APDU.", __func__);
        if (tr_apdu_p != NULL) {
            ste_apdu_delete(tr_apdu_p);
        }
        return rv;
    }
    //create a terminal response msg and put it into the mq
    catd_sig_tr(CATD_FD, tr_apdu_p, CATD_CLIENT_TAG);

    ste_apdu_delete(tr_apdu_p);

    catd_log_f(SIM_LOGGING_D, "catd : %s : EXIT", __func__);
    return STE_SAT_APDU_ERROR_NONE;
}

static ste_sat_apdu_error_t pc_handle_imei(ste_parsed_apdu_t   * parsed_apdu_p)
{
    ste_apdu_t                  *tr_apdu_p;
    ste_command_result_t        cmd_result;
    ste_cmd_result_imei_t       imei;
    cops_imei_t                 cops_imei;
    cops_context_id_t           *ctx = NULL;
    cops_return_code_t          ret_code;
    ste_sat_apdu_error_t        rv;

    catd_log_f(SIM_LOGGING_D, "catd : %s : ENTER", __func__);

    ret_code = cops_context_create(&ctx,NULL,NULL);
    if (ret_code != COPS_RC_OK)
    {
        catd_log_f(SIM_LOGGING_E, "catd: couldn't create cops context");
        return STE_SAT_APDU_ERROR_UNKNOWN;
    }

    ret_code = cops_read_imei(ctx, &cops_imei);

    //free cops_context
    cops_context_destroy(&ctx);

    if (ret_code != COPS_RC_OK)
    {
        catd_log_f(SIM_LOGGING_E, "catd: didnt fetch imei correctly");
        return STE_SAT_APDU_ERROR_UNKNOWN;
    }

    //copy the IMEI number
    memcpy(&imei.Digit, &cops_imei, sizeof(cops_imei));

    //fill in IMEI data
    imei.Length = (sizeof(cops_imei)+1) / 2; //Length of value part in bytes
    imei.OddEvenIndication = 1; //Odd = 1, Even = 0
    imei.TypeOfIdentity = 2; //010 means IMEI

    //fill the data into the result structure
    cmd_result.general_result = SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
    cmd_result.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
    cmd_result.other_data_type = STE_CMD_RESULT_IMEI;
    cmd_result.other_data.imei_p = &imei;

    //fill and initiate additional data fields
    cmd_result.additional_info_p = NULL;
    cmd_result.additional_info_size = 0;

    rv = ste_apdu_terminal_response(parsed_apdu_p, &cmd_result, &tr_apdu_p);
    if (rv != STE_SAT_APDU_ERROR_NONE)
    {
        catd_log_f(SIM_LOGGING_E, "catd : failed to create terminal response APDU.");
        if (tr_apdu_p != NULL) {
            ste_apdu_delete(tr_apdu_p);
        }
        return rv;
    }
    //create a terminal response msg and put it into the mq
    catd_sig_tr(CATD_FD, tr_apdu_p, CATD_CLIENT_TAG);

    ste_apdu_delete(tr_apdu_p);
    catd_log_f(SIM_LOGGING_D, "catd : %s : EXIT", __func__);
    return STE_SAT_APDU_ERROR_NONE;
}

static ste_sat_apdu_error_t pc_handle_imeisv(ste_parsed_apdu_t   * parsed_apdu_p)
{
    ste_apdu_t                  *tr_apdu_p;
    ste_command_result_t        cmd_result;
    ste_cmd_result_imeisv_t     imeisv;
    cops_imei_t                 cops_imeisv;
    cops_context_id_t           *ctx = NULL;
    cops_return_code_t          ret_code;
    ste_sat_apdu_error_t        rv;

    catd_log_f(SIM_LOGGING_D, "catd : %s : ENTER", __func__);

    ret_code = cops_context_create(&ctx,NULL,NULL);
    if (ret_code != COPS_RC_OK)
    {
        catd_log_f(SIM_LOGGING_E, "catd: couldn't create cops context");
        return STE_SAT_APDU_ERROR_UNKNOWN;
    }

    ret_code = cops_read_imei(ctx, &cops_imeisv);

    //free cops_context
    cops_context_destroy(&ctx);

    if (ret_code != COPS_RC_OK)
    {
        catd_log_f(SIM_LOGGING_E, "catd: didnt fetch imei correctly");
        return STE_SAT_APDU_ERROR_UNKNOWN;
    }

    //copy the IMEISV number
    memcpy(&imeisv.Digit, &cops_imeisv, sizeof(cops_imeisv));

    //fill in IMEISV data
    imeisv.Length = 9; //Length of value part in bytes, always 9 for IMEISV cause of SVN added.
    imeisv.OddEvenIndication = 0; //Odd = 1, Even = 0
    imeisv.TypeOfIdentity = 3; //011 means IMEISV
    imeisv.SVN[0] = 0; //Software version hardcoded to 00
    imeisv.SVN[1] = 0;

    //fill the data into the result structure
    cmd_result.general_result = SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
    cmd_result.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
    cmd_result.other_data_type = STE_CMD_RESULT_IMEISV;
    cmd_result.other_data.imeisv_p = &imeisv;

    //fill and initiate additional data fields
    cmd_result.additional_info_p = NULL;
    cmd_result.additional_info_size = 0;

    rv = ste_apdu_terminal_response(parsed_apdu_p, &cmd_result, &tr_apdu_p);
    if (rv != STE_SAT_APDU_ERROR_NONE)
    {
        catd_log_f(SIM_LOGGING_E, "catd : failed to create terminal response APDU.");
        if (tr_apdu_p != NULL) {
            ste_apdu_delete(tr_apdu_p);
        }
        return rv;
    }
    //create a terminal response msg and put it into the mq
    catd_sig_tr(CATD_FD, tr_apdu_p, CATD_CLIENT_TAG);

    ste_apdu_delete(tr_apdu_p);
    catd_log_f(SIM_LOGGING_D, "catd : %s : EXIT", __func__);
    return STE_SAT_APDU_ERROR_NONE;
}

static ste_sat_apdu_error_t pc_handle_date_time_timezone(ste_parsed_apdu_t   * parsed_apdu_p) {
    ste_apdu_t                              *tr_apdu_p;
    ste_apdu_date_time_t                    date_time;
    ste_command_result_t                    cmd_result;
    time_t                                  now;
    struct tm                               tm_now;
    ste_sat_apdu_error_t                    rv;

    catd_log_f(SIM_LOGGING_D, "catd : %s : ENTER", __func__);

    // Init time values
    now = time(NULL);
    tm_now = *localtime(&now);

    //Fill in the date, time and timezone
    date_time.time.hours = dec2bcd(tm_now.tm_hour);
    date_time.time.minutes = dec2bcd(tm_now.tm_min);
    date_time.time.seconds = dec2bcd(tm_now.tm_sec);

    date_time.date.day = dec2bcd(tm_now.tm_mday);
    date_time.date.month = dec2bcd(tm_now.tm_mon+1); //since tm_mon is months since January
    date_time.date.year = dec2bcd(tm_now.tm_year%100); //since tm_year is years since 1900

#ifdef  HAVE_ANDROID_OS
    date_time.timezone.timezone = dec2bcd(tm_now.tm_gmtoff/3600);
#else
    date_time.timezone.timezone = dec2bcd(tm_now.__tm_gmtoff/3600);
#endif

    date_time.timezone.daylight = tm_now.tm_isdst;

    //fill the data into the result structure
    cmd_result.general_result = SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
    cmd_result.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
    cmd_result.other_data_type = STE_CMD_RESULT_DATE_TIME_TIMEZONE;
    cmd_result.other_data.date_time = &date_time;

    //fill and initiate additional data fields
    cmd_result.additional_info_p = NULL;
    cmd_result.additional_info_size = 0;

    rv = ste_apdu_terminal_response(parsed_apdu_p, &cmd_result, &tr_apdu_p);
    if (rv != STE_SAT_APDU_ERROR_NONE)
    {
        catd_log_f(SIM_LOGGING_E, "catd : failed to create terminal response APDU.");
        if (tr_apdu_p != NULL) {
            ste_apdu_delete(tr_apdu_p);
        }
        return rv;
    }
    //create a terminal response msg and put it into the mq
    catd_sig_tr(CATD_FD, tr_apdu_p, CATD_CLIENT_TAG);

    ste_apdu_delete(tr_apdu_p);
    catd_log_f(SIM_LOGGING_D, "catd : %s : EXIT", __func__);
    return STE_SAT_APDU_ERROR_NONE;
}

static int pc_handle_location_cn_response_handler(ste_msg_t *ste_msg) {
    ste_apdu_location_info_t    *local_info = NULL;
    catd_msg_cn_t               *msg_p;
    cn_message_t                *cn_msg_p;
    ste_sim_ctrl_block_t        *ctrl_p;
    ste_apdu_t                  *apdu_p;
    ste_parsed_apdu_t           *parsed_apdu_p = NULL;
    ste_apdu_t                  *tr_apdu_p = NULL;
    ste_command_result_t         cmd_result;
    ste_sat_apdu_error_t         rv;
    cn_cell_info_t              *cell_info_p;
    ste_apdu_additional_info_t   add_info;

    catd_log_f(SIM_LOGGING_D, "catd : %s : ENTER", __func__);

    assert(ste_msg);
    msg_p = (catd_msg_cn_t*) ste_msg;

    assert(msg_p->cn_msg);
    cn_msg_p = (cn_message_t*) msg_p->cn_msg;

    ctrl_p = (ste_sim_ctrl_block_t*)(msg_p->client_tag);
    assert(ctrl_p);

    apdu_p = (ste_apdu_t*)(ctrl_p->transaction_data_p);
    assert(apdu_p);

    rv = ste_apdu_parse(apdu_p, &parsed_apdu_p);
    if (rv != STE_SAT_APDU_ERROR_NONE) {
        catd_log_f(SIM_LOGGING_E, "catd : %s failed to parse APDU", __func__);
        rv = STE_SAT_APDU_ERROR_INTERNAL_IMPLEMENTATION;
        goto error;
    }

    if (CN_RESPONSE_CELL_INFO == cn_msg_p->type && cn_msg_p->error_code== CN_SUCCESS) {
        cell_info_p = (cn_cell_info_t *) cn_msg_p->payload;

        if (!cell_info_p) {
            catd_log_f(SIM_LOGGING_E, "catd : %s error registration info from CN.", __func__);
            rv = STE_SAT_APDU_ERROR_INTERNAL_IMPLEMENTATION;
            goto error;
        }

        if (cell_info_p->service_status == CN_NW_NO_COVERAGE) {

            catd_log_f(SIM_LOGGING_D, "catd : %s: Location status cannot be provided due to no service. service_status = %d.",
                    __func__, cell_info_p->service_status);

            add_info = SAT_RES_MEP_NO_SERVICE;

            //fill the data into the result structure
            cmd_result.general_result = SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD;
            cmd_result.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
            cmd_result.other_data_type = STE_CMD_RESULT_NOTHING;

            //fill and initiate additional data fields
            cmd_result.additional_info_p = (uint8_t*)&add_info;
            cmd_result.additional_info_size = 1;

        } else if (cell_info_p->service_status == CN_NW_LIMITED_SERVICE) {

            catd_log_f(SIM_LOGGING_D, "catd : %s: Location status, emergency calls only. service_status = %d.",
                                __func__, cell_info_p->service_status);

            local_info = create_local_info_from_cell_info(cell_info_p);

            if (local_info == NULL) {
                catd_log_f(SIM_LOGGING_E, "catd : %s failed to convert local info, normal service", __func__);
                rv = STE_SAT_APDU_ERROR_INTERNAL_IMPLEMENTATION;
                goto error;
            }

            //fill the data into the result structure
            cmd_result.general_result = SAT_RES_CMD_PERFORMED_SUCC_LIMITED_SERVICE;
            cmd_result.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
            cmd_result.other_data_type = STE_CMD_RESULT_LOCATION;
            cmd_result.other_data.location_info_p = local_info;

            //fill and initiate additional data fields
            cmd_result.additional_info_p = NULL;
            cmd_result.additional_info_size = 0;

        } else if (cell_info_p->service_status == CN_NW_LIMITED_SERVICE_WAITING) {
            cmd_result.general_result = SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD;
            cmd_result.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
            add_info = SAT_RES_MEP_PROB_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;

            //fill and initiate additional data fields
            cmd_result.additional_info_p = (uint8_t*)&add_info;
            cmd_result.additional_info_size = 1;
        } else {
            catd_log_f(SIM_LOGGING_D, "catd : %s: Send down location info to SIM. reg_status = %d.",
                    __func__, cell_info_p->service_status);

            local_info = create_local_info_from_cell_info(cell_info_p);

            if (local_info == NULL) {
                catd_log_f(SIM_LOGGING_E, "catd : %s failed to convert local info, normal service", __func__);
                rv = STE_SAT_APDU_ERROR_INTERNAL_IMPLEMENTATION;
                goto error;
            }

            //fill the data into the result structure
            cmd_result.general_result = SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
            cmd_result.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
            cmd_result.other_data_type = STE_CMD_RESULT_LOCATION;
            cmd_result.other_data.location_info_p = local_info;

            //fill and initiate additional data fields
            cmd_result.additional_info_p = NULL;
            cmd_result.additional_info_size = 0;
        }
    } else {
        catd_log_f(SIM_LOGGING_E, "catd : %s failed to get local info from cn", __func__);
        rv = STE_SAT_APDU_ERROR_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD;
        goto error;
    }

    rv = ste_apdu_terminal_response(parsed_apdu_p, &cmd_result, &tr_apdu_p);
    if (rv != STE_SAT_APDU_ERROR_NONE) {
        catd_log_f(SIM_LOGGING_E, "catd : %s failed to create terminal response APDU.", __func__);
        goto error;
    }
    //create a terminal response msg and put it into the mq
    catd_sig_tr(CATD_FD, tr_apdu_p, CATD_CLIENT_TAG);

    ste_parsed_apdu_delete(parsed_apdu_p);
    ste_apdu_delete(tr_apdu_p);
    ste_apdu_delete(apdu_p);

    if (local_info) {
        free(local_info);
    }

    catd_log_f(SIM_LOGGING_D, "catd : %s : EXIT", __func__);
    return 0;

error:
    pc_send_terminal_response_error(rv, apdu_p);
    ste_parsed_apdu_delete(parsed_apdu_p);
    ste_apdu_delete(tr_apdu_p);
    ste_apdu_delete(apdu_p);
    if (local_info) {
        free(local_info);
    }

    catd_log_f(SIM_LOGGING_D, "catd : %s : EXIT ERROR", __func__);
    return rv;
}

static ste_sat_apdu_error_t pc_handle_location(ste_apdu_t *raw_apdu_p)
{
    cn_context_t            *cn_context_p = NULL;
    ste_sim_ctrl_block_t    *ctrl_p = NULL;
    ste_apdu_t              *transaction_apdu_p = NULL;

    catd_log_f(SIM_LOGGING_D, "catd : %s : ENTER", __func__);

    cn_context_p = cn_get_client();
    if (cn_context_p == NULL) {
        catd_log_f(SIM_LOGGING_E, "%s : Failed to get cn client context", __func__);
        return STE_SAT_APDU_ERROR_UNKNOWN;
    }

    //Copy apdu to be able to send terminal response later from async handler
    transaction_apdu_p = ste_apdu_copy(raw_apdu_p);
    if (!transaction_apdu_p) {
        catd_log_f(SIM_LOGGING_E, "%s : Failed to copy APDU", __func__);
        return STE_SAT_APDU_ERROR_UNKNOWN;
    }

    ctrl_p = ste_sim_create_ctrl_block(CATD_CLIENT_TAG,
                                       CATD_FD,
                                       CATD_MSG_NONE,
                                       pc_handle_location_cn_response_handler,
                                       transaction_apdu_p);

    if (ctrl_p) {
        if (CN_SUCCESS != cn_request_cell_info(cn_context_p, (uintptr_t) ctrl_p)) {
            catd_log_f(SIM_LOGGING_E, "%s : Failed to request cn", __func__);
            ste_apdu_delete(transaction_apdu_p);
            return STE_SAT_APDU_ERROR_UNKNOWN;
        }
    } else {
        catd_log_f(SIM_LOGGING_E, "%s : Failed to create ctrl block", __func__);
        ste_apdu_delete(transaction_apdu_p);
        return STE_SAT_APDU_ERROR_UNKNOWN;
    }

    catd_log_f(SIM_LOGGING_D, "catd : %s : EXIT", __func__);
    return STE_SAT_APDU_ERROR_NONE;
}

static ste_sat_apdu_error_t pc_handle_access_technology(ste_apdu_t* raw_apdu)
{
    cn_context_t*         context = NULL;
    ste_sim_ctrl_block_t* ctrl_p  = NULL; // Use _p naming convention because someone likes to grep for "ctrl_p" :)
    ste_apdu_t*           apdu    = NULL;
    ste_sat_apdu_error_t  result  = STE_SAT_APDU_ERROR_NONE; // Assume all is well until proven otherwise
    int rc                        = ~CN_SUCCESS; // Initiate with a value guaranteed to be different from CN_SUCCESS

    catd_log_f(SIM_LOGGING_D, "catd : %s : ENTER", __func__);

    context = cn_get_client();
    apdu    = ste_apdu_copy(raw_apdu);
    ctrl_p  = ste_sim_create_ctrl_block(CATD_CLIENT_TAG,
                                        CATD_FD,
                                        CATD_MSG_NONE,
                                        pc_handle_access_technology_cn_response,
                                        apdu);

    if (context && apdu && ctrl_p) {
        rc = cn_request_cell_info(context, (cn_client_tag_t)ctrl_p);
    }

    /*
     * Error handling & cleanup, if needed...
     */
    if (rc != CN_SUCCESS) {
        catd_log_f(SIM_LOGGING_E, "%s : Error (0x%X, 0x%X, 0x%X, %d)", __func__, context, apdu, ctrl_p, rc);
        free(ctrl_p);
        ste_apdu_delete(apdu);
        result = STE_SAT_APDU_ERROR_UNKNOWN;
    }

    catd_log_f(SIM_LOGGING_D, "catd : %s : EXIT(%d)", __func__, result);
    return result;
}

static ste_sat_apdu_error_t pc_handle_search_mode(ste_apdu_t* raw_apdu)
{
    cn_context_t*         context = NULL;
    ste_sim_ctrl_block_t* ctrl_p  = NULL; // Use _p naming convention because someone likes to grep for "ctrl_p" :)
    ste_apdu_t*           apdu    = NULL;
    ste_sat_apdu_error_t  result  = STE_SAT_APDU_ERROR_NONE; // Assume all is well until proven otherwise
    int rc                        = ~CN_SUCCESS; // Initiate with a value guaranteed to be different from CN_SUCCESS

    catd_log_f(SIM_LOGGING_D, "catd : %s : ENTER", __func__);

    context = cn_get_client();
    apdu    = ste_apdu_copy(raw_apdu);
    ctrl_p  = ste_sim_create_ctrl_block(CATD_CLIENT_TAG,
                                        CATD_FD,
                                        CATD_MSG_NONE,
                                        pc_handle_search_mode_cn_response_handler,
                                        apdu);

    if (context && apdu && ctrl_p) {
        rc = cn_request_registration_state_normal(context, (cn_client_tag_t)ctrl_p);
    }

    /*
     * Error handling & cleanup, if needed...
     */
    if (rc != CN_SUCCESS) {
        catd_log_f(SIM_LOGGING_E, "%s : Error (0x%X, 0x%X, 0x%X, %d)", __func__, context, apdu, ctrl_p, rc);
        free(ctrl_p);
        ste_apdu_delete(apdu);
        result = STE_SAT_APDU_ERROR_UNKNOWN;
    }

    catd_log_f(SIM_LOGGING_D, "catd : %s : EXIT(%d)", __func__, result);
    return result;
}

static int pc_handle_search_mode_cn_response_handler(ste_msg_t *ste_msg)
{
    catd_msg_cn_t*          msg      = (catd_msg_cn_t*)ste_msg;
    ste_sim_ctrl_block_t*   ctrl_p   = NULL; // Use _p naming convention because someone likes to grep for "ctrl_p" :)
    ste_apdu_t*             apdu;
    ste_apdu_t*             response = NULL;
    ste_parsed_apdu_t*      parsed   = NULL;
    ste_sat_apdu_error_t    rc;
    ste_apdu_pc_provide_local_info_t* pli_p;

    catd_log_f(SIM_LOGGING_D, "catd : %s : ENTER", __func__);

    assert(msg);
    assert(msg->cn_msg);
    ctrl_p = (ste_sim_ctrl_block_t*)msg->client_tag;  assert(ctrl_p);
    apdu   = (ste_apdu_t*)ctrl_p->transaction_data_p; assert(apdu);

    // APDU was parsed once before when we set up the request, so it really should be parsable this time too...
    rc = ste_apdu_parse(apdu, &parsed);
    if (rc                      == STE_SAT_APDU_ERROR_NONE               &&
        parsed                                                           &&
        msg->cn_msg->type       == CN_RESPONSE_REGISTRATION_STATE_NORMAL &&
        msg->cn_msg->error_code == CN_SUCCESS                             ) {

        rc = ste_parsed_apdu_get(parsed, (void**)&pli_p, STE_APDU_CMD_TYPE_PROVIDE_LOCAL_INFO);
        if (rc == STE_SAT_APDU_ERROR_NONE)
        {
            ste_command_result_t    result;
            cn_registration_info_t* info     = (cn_registration_info_t*)msg->cn_msg->payload;

            result.general_result         = SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
            result.command_status         = STE_SAT_PC_CMD_STATUS_COMPLETED;

            if (pli_p->info_type == STE_SAT_PROVIDE_LOCAL_INFO_TYPE_SEARCH_MODE)
            {
                result.other_data_type        = STE_CMD_RESULT_SEARCH_MODE;
                result.other_data.search_mode = info->search_mode; // cn_network_search_mode_t complies with TS 102.223 sect 8.75
                result.additional_info_p      = NULL;
                result.additional_info_size   = 0;

                rc = ste_apdu_terminal_response(parsed, &result, &response); // response will only be set if all is ok
            } else {
                rc = STE_SAT_APDU_ERROR_UNKNOWN;
            }
        }
    } else {
        rc = STE_SAT_APDU_ERROR_UNKNOWN;
    }

    if (rc == STE_SAT_APDU_ERROR_NONE && response) { // If we managed to create a response, then all is well!
        catd_sig_tr(CATD_FD, response, CATD_CLIENT_TAG);
    } else {
        pc_send_terminal_response_error(rc, apdu);
    }

    ste_apdu_delete(response);      // Can handle null pointer...
    ste_parsed_apdu_delete(parsed); // Can handle null pointer...
    ste_apdu_delete(apdu);          // Can handle null pointer...

    catd_log_f(SIM_LOGGING_D, "catd : %s : EXIT(%d)", __func__, rc);
    return 0;
}

static int pc_handle_access_technology_cn_response(ste_msg_t *ste_msg)
{
    catd_msg_cn_t*          msg      = (catd_msg_cn_t*)ste_msg;
    ste_sim_ctrl_block_t*   ctrl_p   = NULL;
    ste_apdu_t*             apdu;
    ste_apdu_t*             response = NULL;
    ste_parsed_apdu_t*      parsed   = NULL;
    ste_sat_apdu_error_t    rc;
    ste_apdu_pc_provide_local_info_t* pli_p;

    catd_log_f(SIM_LOGGING_D, "catd : %s : ENTER", __func__);

    assert(msg);
    assert(msg->cn_msg);
    ctrl_p = (ste_sim_ctrl_block_t*)msg->client_tag;  assert(ctrl_p);
    apdu   = (ste_apdu_t*)ctrl_p->transaction_data_p; assert(apdu);

    // APDU was parsed once before when we set up the request, so it really should be parsable this time too...
    rc = ste_apdu_parse(apdu, &parsed);
    if (rc                      == STE_SAT_APDU_ERROR_NONE               &&
        parsed                                                           &&
        msg->cn_msg->type       == CN_RESPONSE_CELL_INFO                 &&
        msg->cn_msg->error_code == CN_SUCCESS                             ) {

        rc = ste_parsed_apdu_get(parsed, (void**)&pli_p, STE_APDU_CMD_TYPE_PROVIDE_LOCAL_INFO);
        if (rc == STE_SAT_APDU_ERROR_NONE)
        {
            ste_command_result_t    result;
            cn_cell_info_t* info     = (cn_cell_info_t*)msg->cn_msg->payload;

            if (info->service_status != CN_NW_LIMITED_SERVICE_WAITING) {
                result.general_result         = SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
                result.command_status         = STE_SAT_PC_CMD_STATUS_COMPLETED;

                if (pli_p->info_type == STE_SAT_PROVIDE_LOCAL_INFO_TYPE_ACCESS_TECH) {
                    result.other_data_type        = STE_CMD_RESULT_ACCESS_TECHNOLOGY;
                    result.other_data.rat         = info->rat;
                    result.additional_info_p      = NULL;
                    result.additional_info_size   = 0;

                    rc = ste_apdu_terminal_response(parsed, &result, &response); // response will only be set if all is ok
                } else {
                    rc = STE_SAT_APDU_ERROR_UNKNOWN;
                }
            } else {
                ste_apdu_additional_info_t   add_info;

                result.general_result         = SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD;
                result.command_status         = STE_SAT_PC_CMD_STATUS_COMPLETED;

                add_info = SAT_RES_MEP_PROB_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;

                //fill and initiate additional data fields
                result.additional_info_p = (uint8_t*)&add_info;
                result.additional_info_size = 1;

                rc = ste_apdu_terminal_response(parsed, &result, &response); // response will only be set if all is ok
            }
        }
    } else {
        rc = STE_SAT_APDU_ERROR_UNKNOWN;
    }

    if (rc == STE_SAT_APDU_ERROR_NONE && response) { // If we managed to create a response, then all is well!
        catd_sig_tr(CATD_FD, response, CATD_CLIENT_TAG);
    } else {
        pc_send_terminal_response_error(rc, apdu);
    }

    ste_apdu_delete(response);      // Can handle null pointer...
    ste_parsed_apdu_delete(parsed); // Can handle null pointer...
    ste_apdu_delete(apdu);          // Can handle null pointer...

    catd_log_f(SIM_LOGGING_D, "catd : %s : EXIT(%d)", __func__, rc);
    return 0;
}

static ste_sat_apdu_error_t pc_handle_timing_advance(ste_apdu_t* raw_apdu)
{
    cn_context_t*         context = NULL;
    ste_sim_ctrl_block_t* ctrl_p  = NULL; // Use _p naming convention because someone likes to grep for "ctrl_p" :)
    ste_apdu_t*           apdu    = NULL;
    ste_sat_apdu_error_t  result  = STE_SAT_APDU_ERROR_NONE; // Assume all is well until proven otherwise
    int rc                        = ~CN_SUCCESS; // Initiate with a value guaranteed to be different from CN_SUCCESS

    catd_log_f(SIM_LOGGING_D, "catd : %s : ENTER", __func__);

    context = cn_get_client();
    apdu    = ste_apdu_copy(raw_apdu);
    ctrl_p  = ste_sim_create_ctrl_block(CATD_CLIENT_TAG,
                                        CATD_FD,
                                        CATD_MSG_NONE,
                                        pc_handle_timing_advance_cn_response_handler,
                                        apdu);

    if (context && apdu && ctrl_p) {
        rc = cn_get_timing_advance_value(context, (cn_client_tag_t)ctrl_p);
    }

    /*
     * Error handling & cleanup, if needed...
     */
    if (rc != CN_SUCCESS) {
        catd_log_f(SIM_LOGGING_E, "%s : Error (0x%X, 0x%X, 0x%X, %d)", __func__, context, apdu, ctrl_p, rc);
        free(ctrl_p);
        ste_apdu_delete(apdu);
        result = STE_SAT_APDU_ERROR_UNKNOWN;
    }

    catd_log_f(SIM_LOGGING_D, "catd : %s : EXIT(%d)", __func__, result);
    return result;
}

static int pc_handle_timing_advance_cn_response_handler(ste_msg_t *ste_msg)
{
    catd_msg_cn_t*          msg      = (catd_msg_cn_t*)ste_msg;
    ste_sim_ctrl_block_t*   ctrl_p   = NULL; // Use _p naming convention because someone likes to grep for "ctrl_p" :)
    ste_apdu_t*             apdu;
    ste_apdu_t*             response = NULL;
    ste_parsed_apdu_t*      parsed   = NULL;
    ste_sat_apdu_error_t    rc;

    catd_log_f(SIM_LOGGING_D, "catd : %s : ENTER", __func__);

    assert(msg);
    assert(msg->cn_msg);
    assert(msg->cn_msg->type == CN_RESPONSE_TIMING_ADVANCE);
    ctrl_p = (ste_sim_ctrl_block_t*)msg->client_tag;  assert(ctrl_p);
    apdu   = (ste_apdu_t*)ctrl_p->transaction_data_p; assert(apdu);

    // APDU was parsed once before when we set up the request, so it really should be parsable this time too...
    rc = ste_apdu_parse(apdu, &parsed);
    if (rc                      == STE_SAT_APDU_ERROR_NONE               &&
        parsed                                                           &&
        msg->cn_msg->error_code == CN_SUCCESS                             ) {

        ste_command_result_t    result;
        cn_timing_advance_info_t* info = (cn_timing_advance_info_t*)msg->cn_msg->payload;
        assert(info); // There has to be a payload if success was reported.

        result.general_result                   = SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
        result.command_status                   = STE_SAT_PC_CMD_STATUS_COMPLETED;
        result.other_data_type                  = STE_CMD_RESULT_TIMING_ADVANCE;
        result.other_data.timing_advance.status = info->validity;
        result.other_data.timing_advance.value  = info->value;
        result.additional_info_p                = NULL;
        result.additional_info_size             = 0;

        rc = ste_apdu_terminal_response(parsed, &result, &response); // response will only be set if all is ok
    } else {
        rc = STE_SAT_APDU_ERROR_UNKNOWN;
    }

    if (response) { // If we managed to create a response, then all is well!
        catd_sig_tr(CATD_FD, response, CATD_CLIENT_TAG);
    } else {
        pc_send_terminal_response_error(rc, apdu);
    }

    ste_apdu_delete(response);      // Can handle null pointer...
    ste_parsed_apdu_delete(parsed); // Can handle null pointer...
    ste_apdu_delete(apdu);          // Can handle null pointer...

    catd_log_f(SIM_LOGGING_D, "catd : %s : EXIT(%d)", __func__, rc);
    return 0;
}

//NMR
static ste_sat_apdu_error_t pc_handle_nmr(ste_apdu_t* raw_apdu, cn_nmr_rat_type_t nmr_rat_type, uint8_t nmr_type)
{
    cn_context_t*           context_p = NULL;
    ste_sim_ctrl_block_t*   ctrl_p = NULL;  //ctrl block for CN NMR request
    ste_sim_ctrl_block_t*   ctrl_p_1 = NULL;  //ctrl block for CN cell info request
    ste_sat_apdu_error_t    result = STE_SAT_APDU_ERROR_NONE;
    cn_nmr_utran_type_t     utran_type = 0x00;
    cn_error_code_t         rc = CN_FAILURE;

    catd_log_f(SIM_LOGGING_D, "catd : %s : ENTER", __func__);

    if (!raw_apdu) {
        result = STE_SAT_APDU_ERROR_UNKNOWN;
        goto ERROR;
    }
    //init the global value and copy the raw apdu
    pli_nmr_info.cn_nmr_p = NULL;
    pli_nmr_info.cn_cell_info_p = NULL;
    pli_nmr_info.apdu_p = ste_apdu_copy(raw_apdu);

    context_p = cn_get_client();
    ctrl_p_1 = ste_sim_create_ctrl_block(CATD_CLIENT_TAG,
                                         CATD_FD,
                                         CATD_MSG_NONE,
                                         pc_handle_cell_info_cn_response_handler,
                                         NULL);

    //Request for cell info
    if (context_p && ctrl_p_1) {
        rc = cn_request_cell_info(context_p, (cn_client_tag_t)ctrl_p_1);
    }

    //Error handling
    if (rc != CN_SUCCESS) {
        catd_log_f(SIM_LOGGING_E, "%s : Error (0x%x, 0x%x, 0x%x, %d)", __func__, context_p, pli_nmr_info.apdu_p, ctrl_p_1, rc);
        free(ctrl_p_1);
        ste_apdu_delete(pli_nmr_info.apdu_p);
        result = STE_SAT_APDU_ERROR_UNKNOWN;
        goto ERROR;
    }

    //Now request for NMR
    ctrl_p = ste_sim_create_ctrl_block(CATD_CLIENT_TAG,
                                       CATD_FD,
                                       CATD_MSG_NONE,
                                       pc_handle_nmr_cn_response_handler,
                                       NULL);

    /* if UTRAN, map nmr type */
    if(nmr_rat_type == CN_NMR_RAT_TYPE_UTRAN) {
        switch(nmr_type) {
            case 0x01:
            {
               utran_type = CN_NMR_UTRAN_TYPE_INTRA_FREQ_NMR;
            }
            break;
            case 0x02:
            {
                utran_type = CN_NMR_UTRAN_TYPE_INTER_FREQ_NMR;
            }
            break;
            case 0x03:
            {
                utran_type = CN_NMR_UTRAN_TYPE_INTER_RAT_NMR;
            }
            break;
            default:
            {
                //Error no use of UTRAN nmr type, its GERAN
                utran_type = 0x00;
            }
        }
    }

    //Requst for NMR
    if (context_p && ctrl_p) {
        rc = cn_request_nmr_info(context_p, nmr_rat_type, utran_type, (cn_client_tag_t)ctrl_p);
    }

    //Error handling
    if (rc != CN_SUCCESS) {
        catd_log_f(SIM_LOGGING_E, "%s : Error (0x%x, 0x%x, 0x%x, %d)", __func__, context_p, pli_nmr_info.apdu_p, ctrl_p, rc);
        free(ctrl_p);
        ste_apdu_delete(pli_nmr_info.apdu_p);
        result = STE_SAT_APDU_ERROR_UNKNOWN;
    }

ERROR:
    catd_log_f(SIM_LOGGING_D, "catd : %s : EXIT(%d)", __func__, rc);
    return result;
}

static int pc_pli_nmr_terminal_response()
{
    ste_apdu_t*                           response_p      = NULL;
    ste_parsed_apdu_t*                    parsed_apdu_p   = NULL;
    ste_sat_apdu_error_t                  result;
    int                                   rv = 0;
    ste_apdu_pc_provide_local_nmr_info_t* local_nmr_info_p  = NULL;
    ste_apdu_additional_info_t            add_info;

    catd_log_f(SIM_LOGGING_D, "catd : %s : ENTER", __func__); //CN_REQUEST_CELL_INFO

    if (!pli_nmr_info.apdu_p || !pli_nmr_info.cn_nmr_p || !pli_nmr_info.cn_cell_info_p)
    {
        catd_log_f(SIM_LOGGING_E, "pc_pli_nmr_terminal_response: unable to proceed with PC PLI NMR.\n");
        return -1;
    }

    result = ste_apdu_parse(pli_nmr_info.apdu_p, &parsed_apdu_p);

    if (result == STE_SAT_APDU_ERROR_NONE && parsed_apdu_p)
    {
        ste_command_result_t    command_result;

        //check the service status
        if (pli_nmr_info.cn_cell_info_p->service_status == CN_NW_LIMITED_SERVICE)
        {
            command_result.general_result = SAT_RES_CMD_PERFORMED_SUCC_LIMITED_SERVICE;
        }
        else if (pli_nmr_info.cn_cell_info_p->service_status == CN_NW_LIMITED_SERVICE_WAITING)
        {
            command_result.general_result = SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD;
        }
        else
        {
            command_result.general_result = SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
        }
        command_result.command_status       = STE_SAT_PC_CMD_STATUS_COMPLETED;
        command_result.other_data_type      = STE_CMD_RESULT_NMR;

        //Allocate memory for the nmr_info struct
        local_nmr_info_p = malloc(sizeof(ste_apdu_pc_provide_local_nmr_info_t));

        if (!local_nmr_info_p)
        {
            catd_log_f(SIM_LOGGING_E, "pc_pli_nmr_terminal_response: memory allocation failed.\n");
            rv = -1;
            goto ERROR;
        }

        command_result.other_data.nmr_info = local_nmr_info_p;

        if (pli_nmr_info.cn_nmr_p->rat == CN_NMR_RAT_TYPE_GERAN)
        {
            command_result.other_data.nmr_info->nmr_rat_type = CN_NMR_RAT_TYPE_GERAN;
            memcpy(command_result.other_data.nmr_info->nmr_geran.measurement_results,
                    pli_nmr_info.cn_nmr_p->nmr_info.geran_info.measurement_results,
                    sizeof(pli_nmr_info.cn_nmr_p->nmr_info.geran_info.measurement_results));
            memcpy(command_result.other_data.nmr_info->nmr_geran.ARFCN_list,
                    pli_nmr_info.cn_nmr_p->nmr_info.geran_info.ARFCN_list,
                    (pli_nmr_info.cn_nmr_p->nmr_info.geran_info.num_of_channels) * sizeof(uint16_t));
            command_result.other_data.nmr_info->nmr_geran.num_of_channels = pli_nmr_info.cn_nmr_p->nmr_info.geran_info.num_of_channels;

        }
        else if (pli_nmr_info.cn_nmr_p->rat == CN_NMR_RAT_TYPE_UTRAN)
        {
            command_result.other_data.nmr_info->nmr_rat_type = CN_NMR_RAT_TYPE_UTRAN;
            memcpy(command_result.other_data.nmr_info->nmr_utran.measurements_report,
                    pli_nmr_info.cn_nmr_p->nmr_info.utran_info.measurements_report,
                    pli_nmr_info.cn_nmr_p->nmr_info.utran_info.mm_report_len);
            command_result.other_data.nmr_info->nmr_utran.mm_report_len = pli_nmr_info.cn_nmr_p->nmr_info.utran_info.mm_report_len;
        }

        if (pli_nmr_info.cn_cell_info_p->service_status != CN_NW_LIMITED_SERVICE_WAITING) {
            command_result.additional_info_p    = NULL;
            command_result.additional_info_size = 0;
        } else {
            add_info = SAT_RES_MEP_PROB_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
            command_result.additional_info_p = (uint8_t*)&add_info;
            command_result.additional_info_size = 1;
        }

        result = ste_apdu_terminal_response(parsed_apdu_p, &command_result, &response_p);

        if (result != STE_SAT_APDU_ERROR_NONE)
        {
            catd_log_f(SIM_LOGGING_E, "catd : %s : Failed to create terminal response APDU.", __func__);
            rv = -1;
            //not to return, we need to send an error terminal response back to modem
        }
    }
    else
    {
        catd_log_f(SIM_LOGGING_E, "pc_pli_nmr_terminal_response: failed to parse APDU PLI NMR.\n");
        result = STE_SAT_APDU_ERROR_UNKNOWN;
        rv = -1;
        //not to return, we need to send an error terminal response back to modem
    }

    if (response_p)
    {
        catd_sig_tr(CATD_FD, response_p, CATD_CLIENT_TAG);
    }
    else
    {
        pc_send_terminal_response_error(result, pli_nmr_info.apdu_p);
    }

ERROR:
    //clean up
    if (parsed_apdu_p)
    {
        ste_parsed_apdu_delete(parsed_apdu_p);  //ste_parsed_apdu_delete can handle NULL parameter
        parsed_apdu_p = NULL;
    }
    if (response_p)
    {
        ste_apdu_delete(response_p);
        response_p = NULL;
    }
    if (local_nmr_info_p)
    {
        free(local_nmr_info_p);
        local_nmr_info_p = NULL;
    }
    ste_apdu_delete(pli_nmr_info.apdu_p);
    pli_nmr_info.apdu_p = NULL;
    free(pli_nmr_info.cn_cell_info_p);
    pli_nmr_info.cn_cell_info_p = NULL;
    free(pli_nmr_info.cn_nmr_p);
    pli_nmr_info.cn_nmr_p = NULL;

    catd_log_f(SIM_LOGGING_D, "catd : %s : EXIT(%d)",__func__, rv);
    return rv;
}

static int pc_handle_cell_info_cn_response_handler(ste_msg_t *ste_msg)
{
    catd_msg_cn_t*                        msg             = (catd_msg_cn_t*)ste_msg;
    ste_sim_ctrl_block_t*                 ctrl_p          = NULL;
    int                                   rv;
    cn_cell_info_t*                       cell_info_p     = NULL;

    catd_log_f(SIM_LOGGING_D, "catd : %s : ENTER", __func__); //CN_REQUEST_CELL_INFO

    assert(msg);
    assert(msg->cn_msg);
    assert(msg->cn_msg->type == CN_RESPONSE_CELL_INFO);
    ctrl_p = (ste_sim_ctrl_block_t*)msg->client_tag;
    assert(ctrl_p);

    if (!pli_nmr_info.apdu_p)
    {
        goto ERROR;
    }

    if (msg->cn_msg->error_code == CN_SUCCESS)
    {
        cell_info_p = (cn_cell_info_t*)(msg->cn_msg->payload);
        assert(cell_info_p);

        //save the received data from CN to global value to be used for terminal response
        if (!pli_nmr_info.cn_cell_info_p)
        {
            pli_nmr_info.cn_cell_info_p = malloc(sizeof(cn_cell_info_t));

            if (!pli_nmr_info.cn_cell_info_p)
            {
                catd_log_f(SIM_LOGGING_E, "pc_handle_cell_info_cn_response_handler: memory allocation failed.\n");
                //not much to do if memory failed
                pc_send_terminal_response_error(STE_SAT_APDU_ERROR_UNKNOWN, pli_nmr_info.apdu_p);
                goto ERROR;
            }
            //this memcpy relies on the data structure of CN, the struct cannot have pointers within it
            memcpy(pli_nmr_info.cn_cell_info_p, cell_info_p, sizeof(cn_cell_info_t));
        }
    }
    else
    {
        catd_log_f(SIM_LOGGING_E, "catd : %s : error resp from CN for req CELL INFO.", __func__);
        pc_send_terminal_response_error(STE_SAT_APDU_ERROR_UNKNOWN, pli_nmr_info.apdu_p);
        goto ERROR;
    }

    //check if we have got the response for CN_REQUEST_CELL_INFO
    if (pli_nmr_info.cn_nmr_p)
    {
        //if yes, then we send down terminal response
        rv = pc_pli_nmr_terminal_response();
        if (rv)
        {
            catd_log_f(SIM_LOGGING_E, "catd : %s : Failed to send terminal response APDU.", __func__);
            return 0;
        }
    }
    else
    {
        //otherwise we just wait for NMR response
    }

    catd_log_f(SIM_LOGGING_D, "catd : %s : EXIT(%d)",__func__, 0);
    return 0;

ERROR:
    //error happened, clean everything
    ste_apdu_delete(pli_nmr_info.apdu_p);
    pli_nmr_info.apdu_p = NULL;
    free(pli_nmr_info.cn_nmr_p);
    pli_nmr_info.cn_nmr_p = NULL;

    catd_log_f(SIM_LOGGING_D, "catd : %s : EXIT(%d)",__func__, -1);
    return 0;
}

static int pc_handle_nmr_cn_response_handler(ste_msg_t *ste_msg)
{
    catd_msg_cn_t*                        msg             = (catd_msg_cn_t*)ste_msg;
    ste_sim_ctrl_block_t*                 ctrl_p          = NULL;
    int                                   rv;
    cn_nmr_info_t*                        nmr_p           = NULL;

    catd_log_f(SIM_LOGGING_D, "catd : %s : ENTER", __func__); //CN_REQUEST_NMR_INFO

    assert(msg);
    assert(msg->cn_msg);
    assert(msg->cn_msg->type == CN_RESPONSE_NMR_INFO);
    ctrl_p = (ste_sim_ctrl_block_t*)msg->client_tag;
    assert(ctrl_p);

    if (!pli_nmr_info.apdu_p)
    {
        goto ERROR;
    }

    if (msg->cn_msg->error_code == CN_SUCCESS)
    {
        nmr_p = (cn_nmr_info_t*)msg->cn_msg->payload;
        assert(nmr_p);

        //save the NMR data from CN to be used for terminal response
        if (!pli_nmr_info.cn_nmr_p)
        {
            pli_nmr_info.cn_nmr_p = malloc(sizeof(cn_nmr_info_t));

            if (!pli_nmr_info.cn_nmr_p)
            {
                catd_log_f(SIM_LOGGING_E, "pc_handle_nmr_cn_response_handler: memory allocation failed.\n");
                //not much to do if memory failed
                pc_send_terminal_response_error(STE_SAT_APDU_ERROR_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD, pli_nmr_info.apdu_p);
                goto ERROR;
            }
            //this memcpy relies on the data structure of CN, the struct cannot have pointers within it
            memcpy(pli_nmr_info.cn_nmr_p, nmr_p, sizeof(cn_nmr_info_t));
        }
    }
    else
    {
        catd_log_f(SIM_LOGGING_E, "catd : %s : error resp from CN for req NMR.", __func__);
        pc_send_terminal_response_error(STE_SAT_APDU_ERROR_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD, pli_nmr_info.apdu_p);
        goto ERROR;
    }

    //check if we have got the response for CN_REQUEST_CELL_INFO
    if (pli_nmr_info.cn_cell_info_p)
    {
        //if yes, then we send down terminal response
        rv = pc_pli_nmr_terminal_response();
        if (rv)
        {
            catd_log_f(SIM_LOGGING_E, "catd : %s : Failed to send terminal response APDU.", __func__);
            return 0;
        }
    }
    else
    {
        //otherwise we just wait for CELL INFO response
    }

    catd_log_f(SIM_LOGGING_D, "catd : %s : EXIT(%d)",__func__, 0);
    return 0;

ERROR:
    //error happened, clean everything
    ste_apdu_delete(pli_nmr_info.apdu_p);
    pli_nmr_info.apdu_p = NULL;
    free(pli_nmr_info.cn_cell_info_p);
    pli_nmr_info.cn_cell_info_p = NULL;

    catd_log_f(SIM_LOGGING_D, "catd : %s : EXIT(%d)",__func__, -1);
    return 0;
}

/* Internal method to translate from decimal to BCD code */
static int dec2bcd(int decimal)
{
    return ((decimal/10)<<4)+(decimal%10);
}
