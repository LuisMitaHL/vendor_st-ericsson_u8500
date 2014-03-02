/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : pc_timer_management.c
 * Description     : Handler function for proactive command timer management.
 *
 * Author          : Haiyuan Bu <haiyuan.bu@stericsson.com>
 *                   Magnus Gustavsson <magnus.m.gustavsson@stericsson.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include "pc_utilities.h"
#include "sim_internal.h"
#include "catd.h"
#include "catd_msg.h"
#include "apdu.h"
#include "sim_unused.h"
#include "func_trace.h"

/* Internal defines. */
#define CATD_TIMER_ONE_INFO (((uint16_t)STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_ONE) << 8)
#define CATD_TIMER_TWO_INFO (((uint16_t)STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_TWO) << 8)
#define CATD_TIMER_THREE_INFO (((uint16_t)STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_THREE) << 8)
#define CATD_TIMER_FOUR_INFO (((uint16_t)STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_FOUR) << 8)
#define CATD_TIMER_FIVE_INFO (((uint16_t)STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_FIVE) << 8)
#define CATD_TIMER_SIX_INFO (((uint16_t)STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_SIX) << 8)
#define CATD_TIMER_SEVEN_INFO (((uint16_t)STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_SEVEN) << 8)
#define CATD_TIMER_EIGHT_INFO (((uint16_t)STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_EIGHT) << 8)

#define CATD_GET_TIMER_INFO_TABLE_POS(timer_info) (((timer_info & 0xFF00) >> 8) - STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_ONE)
#define CATD_GET_TIMER_ID_TABLE_POS(timer_id) ((timer_id) - STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_ONE)

#define TIMER_VALUE_MAX_24_HOURS 86400 // Max timer expiration time.

/* Internal type declaration. */

typedef struct {
    timer_t                                 timer_id_system;   // Timer ID, given at timer_create command.
    ste_apdu_timer_identifier_code_value_t  timer_id_pc;       // Timer ID, given in proactive command.
    time_t                                  time_initially_started; // The time when the timer was started.
    uint16_t                                timer_sig_info;    // Information passed between different context.
    ste_apdu_t                              *apdu_p;           // To store data for Timer Expiration Envelope Command until the SIM accepted the command.
    boolean                                 is_ec_started;     // Track if Envelope is already being sent to card
} timer_handler_table_t;

static timer_handler_table_t timer_handler_table[] = {
    {(timer_t)-1, STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_ONE, 0, (uint16_t)CATD_TIMER_ONE_INFO, NULL, FALSE},
    {(timer_t)-1, STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_TWO, 0, (uint16_t)CATD_TIMER_TWO_INFO, NULL, FALSE},
    {(timer_t)-1, STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_THREE, 0, (uint16_t)CATD_TIMER_THREE_INFO, NULL, FALSE},
    {(timer_t)-1, STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_FOUR, 0, (uint16_t)CATD_TIMER_FOUR_INFO, NULL, FALSE},
    {(timer_t)-1, STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_FIVE, 0, (uint16_t)CATD_TIMER_FIVE_INFO, NULL, FALSE},
    {(timer_t)-1, STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_SIX, 0, (uint16_t)CATD_TIMER_SIX_INFO, NULL, FALSE},
    {(timer_t)-1, STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_SEVEN, 0, (uint16_t)CATD_TIMER_SEVEN_INFO, NULL, FALSE},
    {(timer_t)-1, STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_EIGHT, 0, (uint16_t)CATD_TIMER_EIGHT_INFO, NULL, FALSE}
};

#define NUMBER_OF_TIMERS ((uint8_t) (sizeof(timer_handler_table) / sizeof(timer_handler_table_t)))

/* Internal functions. */
static int catd_get_timer_value(const ste_apdu_timer_identifier_code_value_t timer_id,
                                uint32_t *timer_value_p);

/*************************************************************************
 * @brief:    Parse timer value from seconds to hours, minutes and seconds.
 *
 * @params:
 *            time       Time (in seconds) since the timer was started last time.
 *            hours_p    Number of hours.
 *            minutes_p  Number of minutes.
 *            seconds_p  Number of seconds.
 *
 * @return:   void.
 *
 * Notes:
 *************************************************************************/
static void catd_parse_timer_value(const uint32_t time, uint8_t *hours_p,
                                   uint8_t *minutes_p, uint8_t *seconds_p)
{
    int remaining_seconds;


    *hours_p = time/3600;   // Number of seconds in one hour is 3600.
    remaining_seconds = time%3600;
    *minutes_p = remaining_seconds/60;   // Number of seconds in one minute is 60.
    remaining_seconds = remaining_seconds%60;
    *seconds_p = remaining_seconds;

    catd_log_f(SIM_LOGGING_D, "catd : catd_parse_timer_value, time = %d seconds, hours = %d, minutes = %d, seconds = %d", time, *hours_p, *minutes_p, *seconds_p);

}

/*************************************************************************
 * @brief:    Start a timer
 *
 * @params:
 *            timer_id         Timer to start.
 *            new_timer_value  New timer expiration time.
 *
 * @return:   Result of the start timer operation.
 *            SAT_RES_CMD_PERFORMED_SUCCESSFULLY:             Successful.
 *            SAT_RES_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME       Erroneous input parameter value.
 *            SAT_RES_ACTION_CONTRADICTS_CURRENT_TIMER_STATE  Wrong command.
 *            SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD      Internal problems. Try again.
 *
 * Notes: When a timer is ordered to be started, a timer "object" must first
 *        be created. Since a timer can be ordered to be started several
 *        times with new (or the same) timer value, no new timer "object"
 *        needs to be created at each start command.
 *
 *        A timer value larger than zero means that the timer will be started
 *        and will time out after that many seconds (unless deactivated or
 *        deleted before that time). A zero timer value means that the timer
 *        should be deactivated (stopped), but that is not handled in this
 *        function.
 *
 *        A uint32_t is used to store timer ID (0xFF00) and number of times
 *        (0x00FF) that the timer has been started since last deactivate or
 *        expiration. This value is propagated between CATD reader and CATD
 *        as some kind of control block.
 *
 *************************************************************************/
static int catd_start_timer(const ste_apdu_timer_identifier_code_value_t timer_id,
                            const uint32_t new_timer_value)
{
    int table_pos;
    int result;
    struct sigevent sev;
    struct itimerspec its;
    uint8_t nbr_of_starts;


    catd_log_f(SIM_LOGGING_D, "catd : catd_start_timer, timer_id = %d, new_timer_value = %d", timer_id, new_timer_value);

    // Verify that we have a valid timer ID.
    if (timer_id == STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_UNKNOWN)
    {
        catd_log_f(SIM_LOGGING_E, "catd : invalid timer ID.");
        return SAT_RES_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
    }
    /* If zero, it means deactivate, and that is not what we do in this function.
     * For that situation, use catd_deactivate_timer. */
    if (new_timer_value == 0) {
        catd_log_f(SIM_LOGGING_E, "catd : Invalid timer value. Timer value is zero (deactivate timer).");
        return SAT_RES_ACTION_CONTRADICTS_CURRENT_TIMER_STATE;
    }

    if (new_timer_value > TIMER_VALUE_MAX_24_HOURS) {
        catd_log_f(SIM_LOGGING_E, "catd : timer value is larger than 24 hours (not valid).");
        return SAT_RES_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
    }

    table_pos = CATD_GET_TIMER_ID_TABLE_POS(timer_id);

    catd_log_f(SIM_LOGGING_V, "catd : timer_sig_info = 0x%04X", timer_handler_table[table_pos].timer_sig_info);

    // If the timer has not been created before, lets do it now.
    if (timer_handler_table[table_pos].timer_id_system == (timer_t)-1) {
        memset(&sev, 0, sizeof(sev));
        sev.sigev_notify = SIGEV_SIGNAL;  // At timer expiration, send signal in sigev_signo.
        sev.sigev_signo = SIGALRM;        // Signal to be sent at timer expiration.
        sev.sigev_value.sival_int = table_pos; // At timeout, include data to know what timer that expired.

        result = timer_create(CLOCK_REALTIME, &sev, &timer_handler_table[table_pos].timer_id_system);

        if (result < 0) {
            catd_log_f(SIM_LOGGING_E, "catd : failed to create timer = %d", timer_id);
            return SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD;
        }
        catd_log_f(SIM_LOGGING_D, "catd : timer = %d created.", timer_id);
    }

    catd_log_f(SIM_LOGGING_V, "catd : timer_id_system = %d", (timer_t)timer_handler_table[table_pos].timer_id_system);

    /* Start the timer. It will expire after new_timer_value if not disarmed before that,
     * or set with a new value. Since both intervals are zero, the timer will not be
     * restarted after expiration. */
    its.it_value.tv_sec = new_timer_value;      // Expiration time, seconds.
    its.it_value.tv_nsec = 0;                   // Expiration time, nano seconds.
    its.it_interval.tv_sec = 0;                 // Inverval time, seconds.
    its.it_interval.tv_nsec = 0;                // Interval time, nano seconds.

    result = timer_settime(timer_handler_table[table_pos].timer_id_system, 0, &its, NULL);

    if (result < 0) {
        catd_log_f(SIM_LOGGING_E, "catd: failed to start timer = %d", timer_id);
        return SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD;
    }
    // Save the time when the timer was started. Needed in envelope command to SIM at timer expiration.
    if (time(&timer_handler_table[table_pos].time_initially_started) == -1) {
        catd_log_f(SIM_LOGGING_E, "catd: failed to get current time.");
        return SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD;
    }

    catd_log_f(SIM_LOGGING_V, "time_initially_started = %d", timer_handler_table[table_pos].time_initially_started);

    /* If there is any Timer Expiration Envelope Command that are
     * expected to be resent, remove that APDU data. */
    if (timer_handler_table[table_pos].apdu_p) {
        ste_apdu_delete(timer_handler_table[table_pos].apdu_p);

        timer_handler_table[table_pos].apdu_p = NULL;
    }

    // To avoid that number of start information overflows into timer ID part/section.
    nbr_of_starts = (timer_handler_table[table_pos].timer_sig_info & 0x00FF) + 1;
    // Save timer ID and number of times the timer has been started.
    timer_handler_table[table_pos].timer_sig_info = (timer_handler_table[table_pos].timer_sig_info & 0xFF00) + nbr_of_starts;

    catd_log_f(SIM_LOGGING_V, "catd : timer_sig_info = 0x%04X", timer_handler_table[table_pos].timer_sig_info);

    return SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
}

/*************************************************************************
 * @brief:    Deactivate a timer.
 *
 * @params:
 *            timer_id       Timer to deactivate.
 *            timer_value_p  Output data (remaining timer value before expiration)
 *                           from the deactivated timer, if the operation is successful.
 *
 * @return:   Result of the read timer operation.
 *            SAT_RES_CMD_PERFORMED_SUCCESSFULLY:             Successful.
 *            SAT_RES_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME       Erroneous input parameter value.
 *            SAT_RES_ACTION_CONTRADICTS_CURRENT_TIMER_STATE  The timer is already deactivated.
 *            SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD      Internal problems. Try again.
 *
 * Notes:
 *************************************************************************/
static int catd_deactivate_timer(const ste_apdu_timer_identifier_code_value_t timer_id,
                                 uint32_t *timer_value_p)
{
    int table_pos;
    int result;
    struct itimerspec timer_settings;


    catd_log_f(SIM_LOGGING_D, "catd : catd_deactivate_timer, timer_id = %d", timer_id);

    if (timer_id == STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_UNKNOWN ||
        timer_value_p == NULL) {
        catd_log_f(SIM_LOGGING_E, "catd : invalid input parameters.");
        return SAT_RES_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
    }

    table_pos = CATD_GET_TIMER_ID_TABLE_POS(timer_id);

    // Read the timer's current expiration value to see if it's already deactivated.
    result = catd_get_timer_value(timer_id, timer_value_p);

    if (result != SAT_RES_CMD_PERFORMED_SUCCESSFULLY) {
        return result;
    }
    else {
        // Deactivate the timer by setting both expiration times to zero.
        timer_settings.it_value.tv_sec = 0;      // Expiration time, seconds.
        timer_settings.it_value.tv_nsec = 0;     // Expiration time, nano seconds.
        timer_settings.it_interval.tv_sec = 0;   // Interval time, seconds.
        timer_settings.it_interval.tv_nsec = 0;  // Interval time, nano seconds.

        result = timer_settime(timer_handler_table[table_pos].timer_id_system, 0, &timer_settings, NULL);

        if (result < 0) {
            catd_log_f(SIM_LOGGING_E, "catd : could not deactivate timer.");
            return SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD;
        }

        /* If there is any Timer Expiration Envelope Command that are
         * expected to be resent, remove that APDU data. */
        if (timer_handler_table[table_pos].apdu_p) {
            ste_apdu_delete(timer_handler_table[table_pos].apdu_p);

            timer_handler_table[table_pos].apdu_p = NULL;
            timer_handler_table[table_pos].is_ec_started = FALSE;
        }
    }
    return SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
}

/*************************************************************************
 * @brief:    Get remaining timer value (in seconds) before expiration.
 *
 * @params:
 *            timer_id       Timer to read from.
 *            timer_value_p  Output data (remaining timer value before expiration),
 *                           if the operation is successful.
 *
 * @return:   Result of the read timer operation.
 *            SAT_RES_CMD_PERFORMED_SUCCESSFULLY:             Successful.
 *            SAT_RES_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME       Erroneous input parameter value.
 *            SAT_RES_ACTION_CONTRADICTS_CURRENT_TIMER_STATE  The timer is already deactivated.
 *            SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD      Internal problems. Try again.
 *
 * Notes:
 *************************************************************************/
static int catd_get_timer_value(const ste_apdu_timer_identifier_code_value_t timer_id,
                                uint32_t *timer_value_p)
{
    int table_pos;
    int result;
    struct itimerspec timer_settings;


    catd_log_f(SIM_LOGGING_D, "catd : catd_get_timer_value, timer_id = %d ", timer_id);

    if (timer_id == STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_UNKNOWN ||
        timer_value_p == NULL) {
        catd_log_f(SIM_LOGGING_E, "catd : invalid input parameters.");
        return SAT_RES_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
    }

    table_pos = CATD_GET_TIMER_ID_TABLE_POS(timer_id);

    if (timer_handler_table[table_pos].timer_id_system == (timer_t)-1) {
        catd_log_f(SIM_LOGGING_E, "catd : cannot get timer's value since it has not yet been created.");
        return SAT_RES_ACTION_CONTRADICTS_CURRENT_TIMER_STATE;
    }

    catd_log_f(SIM_LOGGING_V, "catd : timer_id_system = %d", (timer_t)timer_handler_table[table_pos].timer_id_system);

    result = timer_gettime(timer_handler_table[table_pos].timer_id_system, &timer_settings);

    if (result < 0) {
        catd_log_f(SIM_LOGGING_E, "catd : error when reading timer value, result = %d ", result);
        return SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD;
    }

    if (timer_settings.it_value.tv_sec == 0 && timer_settings.it_value.tv_nsec == 0) {
        catd_log_f(SIM_LOGGING_E, "catd : timer is deactivated.");
        return SAT_RES_ACTION_CONTRADICTS_CURRENT_TIMER_STATE;
    }

    *timer_value_p = timer_settings.it_value.tv_sec;

    return SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
}

/*************************************************************************
 * @brief:    Try to find what timer that expired so we can send data to
 *            desired pipe.
 * @params:
 *            sig            Signal number.
 *            si_p           Signal information data.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
static void catd_sig_timeout_handler(int sig, siginfo_t *si_p, void *UNUSED(uc))
{
    int table_pos;
    int fd;
    int result;


    catd_log_f(SIM_LOGGING_D, "catd : catd_sig_timeout_handler");

    if (si_p == NULL) {
        catd_log_f(SIM_LOGGING_E, "catd : si_p is NULL!!!");
        return;
    }

    /* Read table position where the timer's expiration data is located.
     * This was stored in sev.sigev_value.sival_int (see catd_start_timer). */
    table_pos = si_p->si_value.sival_int;

    catd_log_f(SIM_LOGGING_V, "catd : timer ID = %d expired", (table_pos + 1));

    if (table_pos < 0 || table_pos > (int) NUMBER_OF_TIMERS - ((int) STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_ONE)) {
        catd_log_f(SIM_LOGGING_E, "catd : invalid timer ID.");
        return;
    }

    switch(sig) {
    case SIGALRM:
        fd = catd_get_timer_write_socket();  // Get file descriptor.

        catd_log_f(SIM_LOGGING_V, "catd : timer_sig_info = 0x%04X", timer_handler_table[table_pos].timer_sig_info);

        result = write(fd, &timer_handler_table[table_pos].timer_sig_info, sizeof(uint16_t));

        if (result < 0) {
            catd_log_f(SIM_LOGGING_E, "catd : failed writing data to socket.");
        }
        break;

    default:
        break;
    }

}

/*************************************************************************
 * @brief:    Handle the particular proactive command of timer management
 *            internally
 * @params:
 *            parsed_apdu_p: The parsed apdu structure.
 *            msg:           The original message with APDU data, and
 *                           client_tag, etc. for possible future usage.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
static void pc_main_handle_timer_management( ste_parsed_apdu_t    * parsed_apdu_p,
                                             catd_msg_apdu_t      * msg)
{
    ste_sat_apdu_error_t                   rv;
    ste_apdu_pc_timer_management_t       * timer_management_p;
    ste_apdu_t                           * apdu_p = msg->apdu;
    uint32_t                               timer_value;
    ste_apdu_general_result_t              general_result;
    ste_command_result_t                   cmd_result;
    ste_apdu_t*                            response_p = NULL;
    ste_apdu_additional_info_t             add_info = SAT_RES_ADD_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;


    catd_log_f(SIM_LOGGING_D, "catd : pc_main_handle_timer_management");

    rv = ste_parsed_apdu_get(parsed_apdu_p, (void**)&timer_management_p, STE_APDU_CMD_TYPE_TIMER_MANAGEMENT);

    if (rv != STE_SAT_APDU_ERROR_NONE) {
        catd_log_f(SIM_LOGGING_E, "catd : failed to fetch APDU data.");
        pc_send_terminal_response_error(rv, apdu_p);
        return;
    }

    if (timer_management_p->bit_mask & PC_TIMER_MANAGEMENT_TimerIdentifier_present) {
        switch((int)timer_management_p->command_details.timer_action) {
            case TIMER_START:
                timer_value = timer_management_p->timer_value.timer_value_data;
                cmd_result.other_data.timer_value[2] = 0;
                cmd_result.other_data.timer_value[3] = 0;
                cmd_result.other_data.timer_value[4] = 0;
                general_result = catd_start_timer(timer_management_p->timer_identifier.timer_code, timer_management_p->timer_value.timer_value_data);
                break;
            case TIMER_DEACTIVATE:
                general_result = catd_deactivate_timer(timer_management_p->timer_identifier.timer_code, &timer_value);
                break;
            case TIMER_GET_CURRENT_VALUE:
                general_result = catd_get_timer_value(timer_management_p->timer_identifier.timer_code, &timer_value);
                break;
            default:
                general_result = SAT_RES_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME;
                break;
        }
        // Check if the proactive command was successful.
        if (general_result == SAT_RES_CMD_PERFORMED_SUCCESSFULLY) {
            // "Extra" data needed in the response.
            cmd_result.general_result = SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
            cmd_result.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
            cmd_result.additional_info_size = 0;
            cmd_result.additional_info_p = NULL;
            cmd_result.other_data_type = STE_CMD_RESULT_TIMER_MANAGEMENT;
            if ((int)timer_management_p->command_details.timer_action == TIMER_START) {
                cmd_result.other_data.timer_value[1] = 0; // Do not add Timer value in response.
            }
            else {
                cmd_result.other_data.timer_value[1] = 1; // Add Timer value in response.
            }
            cmd_result.other_data.timer_value[0] = (uint8_t) timer_management_p->timer_identifier.timer_code;

            catd_parse_timer_value(timer_value, &cmd_result.other_data.timer_value[2],
                                   &cmd_result.other_data.timer_value[3], &cmd_result.other_data.timer_value[4]);

            cmd_result.other_data.timer_value[2] = ste_apdu_encode_bcd(cmd_result.other_data.timer_value[2]);
            cmd_result.other_data.timer_value[3] = ste_apdu_encode_bcd(cmd_result.other_data.timer_value[3]);
            cmd_result.other_data.timer_value[4] = ste_apdu_encode_bcd(cmd_result.other_data.timer_value[4]);

            rv = ste_apdu_terminal_response(parsed_apdu_p, &cmd_result, &response_p);

            if (rv != STE_SAT_APDU_ERROR_NONE) {
                catd_log_f(SIM_LOGGING_E, "catd : failed to create terminal response.");
                pc_send_terminal_response_error(rv, apdu_p);

                if (response_p != NULL) {
                    ste_apdu_delete(response_p);
                }

                return;
            }
            // Create a terminal response msg and put it into the mq.
            catd_sig_tr(CATD_FD, response_p, CATD_CLIENT_TAG);
            ste_apdu_delete(response_p);
        }
        else {
            cmd_result.general_result = general_result;
            cmd_result.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;

            if (general_result == SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD) {
                cmd_result.additional_info_size = 1;
                cmd_result.additional_info_p = (uint8_t*)(&add_info);
            }
            else {
                cmd_result.additional_info_size = 0;
                cmd_result.additional_info_p = NULL;
            }

            cmd_result.other_data_type = STE_CMD_RESULT_NOTHING;
            rv = ste_apdu_terminal_response(parsed_apdu_p, &cmd_result, &response_p);

            if (rv != STE_SAT_APDU_ERROR_NONE) {
                catd_log_f(SIM_LOGGING_E, "catd : failed to create terminal response.");
                pc_send_terminal_response_error(rv, apdu_p);

                if (response_p != NULL) {
                    ste_apdu_delete(response_p);
                }

                return;
            }
            // Create a terminal response msg and put it into the mq.
            catd_sig_tr(CATD_FD, response_p, CATD_CLIENT_TAG);
            ste_apdu_delete(response_p);
        }
    }
    else
    {
        catd_log_f(SIM_LOGGING_E, "catd : Error in APDU data.");
        pc_send_terminal_response_general(parsed_apdu_p, SAT_RES_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME);
        return;
    }

}

/*************************************************************************
 * @brief:    Handle the particular proactive command of timer management.
 * @params:
 *            msg:   The original message with APDU data, and client_tag, etc.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
void catd_handle_pc_timer_management(catd_msg_apdu_t  * msg)
{
    ste_sat_apdu_error_t                 rv;
    ste_apdu_t                         * apdu = msg->apdu;
    ste_parsed_apdu_t                  * parsed_apdu_p = NULL;

    assert(msg);

    catd_log_f(SIM_LOGGING_D, "catd : catd_handle_pc_timer_management");
    catd_log_b(SIM_LOGGING_V, "catd : APDU = ", ste_apdu_get_raw(apdu), ste_apdu_get_raw_length(apdu));
    rv = ste_apdu_parse(apdu, &parsed_apdu_p);

    if (rv == STE_SAT_APDU_ERROR_NONE) {
        pc_main_handle_timer_management(parsed_apdu_p, msg);
    } else {
        pc_send_terminal_response_error(rv, apdu);
    }
    ste_parsed_apdu_delete(parsed_apdu_p);
}

/*************************************************************************
 * @brief:    Clean up a timer's expiration related data.
 * @params:
 *            timer_info:   Timer information.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
void catd_timer_reset_expiration_data(const uint16_t timer_info)
{
    uint8_t  table_pos;


    catd_log_f(SIM_LOGGING_D, "catd : catd_timer_reset_expiration_data, timer_info = 0x%04X", timer_info);
    table_pos = CATD_GET_TIMER_INFO_TABLE_POS(timer_info);

    if (table_pos < NUMBER_OF_TIMERS) {
        if (timer_handler_table[table_pos].apdu_p) {
            catd_log_b(SIM_LOGGING_V, "catd : apdu_p = ", ste_apdu_get_raw(timer_handler_table[table_pos].apdu_p), ste_apdu_get_raw_length(timer_handler_table[table_pos].apdu_p));
            catd_log_f(SIM_LOGGING_V, "catd : catd_timer_reset_expiration_data  timer_sig_info = 0x%04X", timer_handler_table[table_pos].timer_sig_info);

            ste_apdu_delete(timer_handler_table[table_pos].apdu_p);

            timer_handler_table[table_pos].apdu_p = NULL;
            timer_handler_table[table_pos].is_ec_started = FALSE;
        }
    }
}

/*************************************************************************
 * @brief:    Clean up a timer's Envelope Ongoing flag.
 * @params:
 *            timer_info:   Timer information.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
void catd_timer_reset_ec_started_flag(const uint16_t timer_info)
{
    uint8_t  table_pos;


    catd_log_f(SIM_LOGGING_D, "catd : catd_timer_reset_ec_started_flag, timer_info = 0x%04X", timer_info);
    table_pos = CATD_GET_TIMER_INFO_TABLE_POS(timer_info);

    if (table_pos < NUMBER_OF_TIMERS) {
        timer_handler_table[table_pos].is_ec_started = FALSE;
    }
}


/*************************************************************************
 * @brief:    Create a Timer Expiration Envelope Command to be sent to the
 *            SIM.
 *
 * @params:
 *            timer_info    Information containing timer ID and number of
 *                          times the timer has been started.
 *
 * @return:   Result of operation:
 *            * A pointer to a Timer Expiration Envelope Command APDU.
 *            * NULL if no APDU should be sent to the SIM, or due to
 *              failure in allocating memory for the APDU data.
 *
 * Notes: Each time a timer is started, the number of times the timer has
 *        been started is increased by one, until it reach 0xFF, when it
 *        overflows and get the value 0x00. This value, together with the
 *        timer's ID is stored in a table.
 *
 *        In case a timer expires and we have not yet send a Timer Expiration
 *        Envelope Command, the timer can be ordered to start again with
 *        a new (or same) timeout value. At the same time the number of
 *        times the timer has been started is increased by one.
 *
 *        To avoid problems with race conditions, check that the timer ID
 *        and the number of times the timer has been started can be found
 *        in the table. If not, the timer expired due to an earlier start
 *        command, and we should not send any Timer Expiration Envelope
 *        Command as we currently use it for supervising some other activity.
 *
 *************************************************************************/
ste_apdu_t *catd_timer_create_expiration_ec(const uint16_t timer_info)
{
    uint8_t     table_pos;
    uint8_t     hours;
    uint8_t     minutes;
    uint8_t     seconds;
    uint32_t    time_value = 0;  // The time from that the timer was started until now.
    time_t      current_time;
    ste_apdu_t  *apdu_p = NULL;


    catd_log_f(SIM_LOGGING_D, "catd : catd_timer_create_expiration_ec");
    catd_log_f(SIM_LOGGING_V, "catd : timer_info = 0x%04X", timer_info);

    table_pos = CATD_GET_TIMER_INFO_TABLE_POS(timer_info);

    if (table_pos < NUMBER_OF_TIMERS) {
        if (timer_info == timer_handler_table[table_pos].timer_sig_info) {
            // Get current time (in seconds) since 1st of January, 1970, or a NULL pointer if it fails.
            if (time(&current_time) != -1) {
                // Get time in seconds from the time the timer was started and present time.

                catd_log_f(SIM_LOGGING_V, "catd : current_time = %d", current_time);
                catd_log_f(SIM_LOGGING_V, "catd : time_initially_started = %d", timer_handler_table[table_pos].time_initially_started);

                time_value = (uint32_t)(current_time - timer_handler_table[table_pos].time_initially_started);

                catd_log_f(SIM_LOGGING_V, "catd : time_value = %d", time_value);

                catd_parse_timer_value(time_value, &hours, &minutes, &seconds);

                apdu_p = ste_apdu_timer_expiration_ec(timer_handler_table[table_pos].timer_id_pc, hours, minutes, seconds);

                if (apdu_p != NULL) {
                    /* There should be no data stored already by this timer,
                     * but if there is, delete old allocated memory. */
                    if (timer_handler_table[table_pos].apdu_p) {
                        catd_log_f(SIM_LOGGING_D, "catd : deleting old APDU data.");

                        ste_apdu_delete(timer_handler_table[table_pos].apdu_p);
                    }

                    timer_handler_table[table_pos].apdu_p = apdu_p; // Store the new data.
                    catd_log_b(SIM_LOGGING_V, "catd : APDU = ", ste_apdu_get_raw(apdu_p), ste_apdu_get_raw_length(apdu_p));
                }
                else {
                    catd_log_f(SIM_LOGGING_E, "catd : could not create APDU!!!");
                }
            }
            else {
                catd_log_f(SIM_LOGGING_E, "catd : could not get current time!!!");
            }
        }
        else {
            catd_log_f(SIM_LOGGING_E, "catd : invalid parameters");
        }
    }
    else {
        catd_log_f(SIM_LOGGING_E, "catd : table_pos >= NUMBER_OF_TIMERS!!!");
    }

    return apdu_p;
}


/*************************************************************************
 * @brief:    Check if there is any Timer Expiration Envelope Command that
 *            the SIM could not handle last time we sent it. In case there
 *            are more than one APDU waiting to be sent, send the APDU
 *            that have waited longest to be sent.
 *
 * @params:
 *            timer_info_p  Information containing timer ID and number of
 *                          times the timer has been started.
 *            apdu_p        Timer Expiration Envelope Command APDU data.
 *
 * @return:   Result of check operation.
 *            0:  Successful, timer_info and apdu_p contains valid data.
 *           -1:  Could not find any data.
 *
 * Notes:
 *************************************************************************/
int catd_timer_check_for_ec_retransmission(uint16_t *timer_info_p, ste_apdu_t **apdu_p)
{
    time_t   current_time;
    int      i;
    int      table_pos = -1;
    uint32_t time_value = 0;
    uint8_t  hours;
    uint8_t  minutes;
    uint8_t  seconds;


    catd_log_f(SIM_LOGGING_D, "catd : catd_timer_check_for_ec_retransmission");
    // Get current time (in seconds) since 1st of January, 1970, or a NULL pointer if it fails.
    if (time(&current_time) == -1) {
        catd_log_f(SIM_LOGGING_D, "catd : could not get current time!!!");
        return -1;
    }

    // Find the oldest APDU data to be sent (if any).
    for (i=0; i< NUMBER_OF_TIMERS; i++) {
        if ( timer_handler_table[i].is_ec_started == TRUE ) {
            return -1;
        }
        if (timer_handler_table[i].apdu_p != NULL) {
            if (((uint32_t)(current_time - timer_handler_table[i].time_initially_started)) > time_value) {

                catd_log_f(SIM_LOGGING_V, "catd : current_time = %d", current_time);
                catd_log_f(SIM_LOGGING_V, "catd : time_initially_started = %d", timer_handler_table[i].time_initially_started);

                time_value = (uint32_t)(current_time - timer_handler_table[i].time_initially_started);

                catd_log_f(SIM_LOGGING_V, "catd : time_value = %d", time_value);
                table_pos = i;
            }
        }
    }

    // If we found an APDU to send, delete old APDU data and create a new one with new time stamp/info.
    if (table_pos != -1) {
        ste_apdu_delete(timer_handler_table[table_pos].apdu_p);

        catd_parse_timer_value(time_value, &hours, &minutes, &seconds);

        *apdu_p = ste_apdu_timer_expiration_ec(timer_handler_table[table_pos].timer_id_pc, hours, minutes, seconds);

        *timer_info_p = timer_handler_table[table_pos].timer_sig_info;

        timer_handler_table[table_pos].apdu_p = *apdu_p; // Store the new data.
        timer_handler_table[table_pos].is_ec_started = TRUE;
        catd_log_f(SIM_LOGGING_V, "catd : timer_info = 0x%04X", *timer_info_p);
        catd_log_b(SIM_LOGGING_V, "catd : APDU = ", ste_apdu_get_raw(*apdu_p), ste_apdu_get_raw_length(*apdu_p));

        return 0;
    }

    return -1;
}

/*************************************************************************
 * @brief:    Delete all timers that have been created, deallocate
 *            memory and set values to default value.
 *
 * @params:
 *            None.
 *
 * @return:
 *
 * Notes:
 *************************************************************************/
void catd_timer_clean_up_data(void)
{
    int i;


    catd_log_f(SIM_LOGGING_D, "catd : catd_timer_clean_up_data");

    for (i=0; i< NUMBER_OF_TIMERS; i++) {
        if (timer_handler_table[i].timer_id_system != (timer_t)-1) {
            timer_delete(timer_handler_table[i].timer_id_system);
        }

        if (timer_handler_table[i].apdu_p != NULL)
        {
            ste_apdu_delete(timer_handler_table[i].apdu_p);
        }

        timer_handler_table[i].apdu_p = NULL;
        timer_handler_table[i].timer_sig_info = timer_handler_table[i].timer_sig_info & 0xFF00;
        timer_handler_table[i].is_ec_started = FALSE;
    }
}

/*************************************************************************
 * @brief:    Create a signal handler to handle timer expiration.
 *
 * @params:
 *            void
 *
 * @return:   Result of the create timer handler operation.
 *            0: Successful.
 *           -1: Could not create a signal handler.
 *
 * Notes:    Create a signal handler. Each process can have many signal
 *           handlers. If more than one is used for the same signal, the
 *           last created overrides any preceeding handlers. For this
 *           process we use SIGALRM to report timer expiration.
 *
 *************************************************************************/
int catd_timer_create_signal_handler(void)
{
    struct sigaction sa;
    int result;


    catd_log_f(SIM_LOGGING_D, "catd : catd_timer_create_signal_handler");

   sa.sa_flags = SA_SIGINFO;  // The signal handler function takes three arguments.
   sa.sa_sigaction = catd_sig_timeout_handler;  // Signal handler function when a timer expire.
   sigemptyset(&sa.sa_mask);
   result = sigaction(SIGALRM, &sa, NULL);  // Listen for SIGALRM signals.

   if (result < 0) {
       catd_log_f(SIM_LOGGING_E, "catd : could not create signal handler for timer expiration.");
   }

    return result;

}
