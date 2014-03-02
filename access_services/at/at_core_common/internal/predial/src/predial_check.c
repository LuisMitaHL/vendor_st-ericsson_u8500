/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <predial.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

static bool check_if_always_set_emergency_dial_string(char *dial_string_p);
static bool check_if_no_sim_set_emergency_dial_string(char *dial_string_p);
static bool check_if_dynamic_emergency_dial_string(char *dial_string_p);
#if 0
static bool check_if_ecc_emergency_dial_string(char *dial_string_p);
static bool check_if_allowed_according_to_fdn_settings(char *dial_string_p);
static sim_bake_response_t check_and_aligned_the_dial_string(char *dial_string_p);
#endif
/*
 *===============================================================================
 *
 *   Function: voice_predial_checks
 *
 *   INPUT:   dial_string    - Pointer to the dial_string
 *            is_emergency   - Pointer to boolean indicating emergency call
 *
 *   OUTPUT:  dial_string    - The dial string is change by the sim module to
 *                             align with expected format
 *                             Note: Extra memory needs to be included as
 *                             characters might be added
 *            is_emergency   - True if the number means an emergency call
 *
 *   RETURNS: A status code for the execution of the function is returned:
 *     PREDIAL_OK,           - Command successfully handled
 *     PREDIAL_ERROR         - Some unknown error occurred, abort call command
 *     PREDIAL_ERROR_NO_SIM  - There is no SIM or SIM is not ready
 *     PREDIAL_ERROR_INVALID_DIAL_STRING - Invalid input parameter, usually
 *                             indicates a NULL string sent.
 *
 *   Checks if a dial string is an emergency call, is valid (not on the sim
 *   barred list or not on the allowed list depending on SIM settings) and if
 *   a valid number aligns the dial string with SIM expected format.
 *
 *   Note: The most important is the returned is_emergency so when using this
 *   function check that even prior to checking the returned error code, if
 *   emergency is true call regardless
 *
 *===============================================================================
 */
predial_response_code_t voice_predial_checks(char *dial_string_p, bool *is_emergency_p)
{
    *is_emergency_p = false;
#ifdef AT_CORE_TEST_PREDIAL /* For predial test */
    bool sim_present = true;
#endif

    if (NULL == dial_string_p) {
        return PREDIAL_ERROR_INVALID_DIAL_STRING;
    }

    /* Regardless if we have sim or not some set emergency numbers
     * always apply, so we start with checking if it is any of those
     * numbers. No interaction with the sim module is needed.
     */
    if (check_if_always_set_emergency_dial_string(dial_string_p)) {
        *is_emergency_p = true;
        return PREDIAL_OK;
    }

    /* Vendor set emergency numbers in the dynamic list are used
     * regardless if we have sim or not, so no need to check for sim yet
     */
    if (check_if_dynamic_emergency_dial_string(dial_string_p)) {
        *is_emergency_p = true;
        return PREDIAL_OK;
    }

    /* todo: something like sim_present = sim_check for sim(); or use the
     * response call from the first call to sim
     */
#ifdef AT_CORE_TEST_PREDIAL /* For predial test */

    if (sim_present) {
        if (check_if_ecc_emergency_dial_string(dial_string_p)) {
            *is_emergency_p = true;
            return PREDIAL_OK;
        }

        /* Normal call*/
        /* NVD */
        if (!check_if_allowed_according_to_nvd_settings(dial_string_p)) {
            return PREDIAL_ERROR_NUMBER_NOT_ALLOWED;
        }

        /* todo: Do we need to get data from the mal here before we can call sim?
         *
         */
        {
            /* Temporary type, check to align with sim */
            sim_bake_response_t bake_response;
            bake_response = check_and_aligned_the_dial_string(dial_string_p);

            if (SIM_ERROR == bake_response) {
                return PREDIAL_ERROR_INVALID_NUMBER_FORMAT;
            }
        }
    } else {
        /* If no sim, we need to check the list of commands the only applies in
         * this case. If not this is nor emergency call and we have no sim so stop
         * the call.
         */
        if (check_if_no_sim_set_emergency_dial_string(dial_string_p)) {
            *is_emergency_p = true;
            return PREDIAL_OK;
        }

        return PREDIAL_ERROR_NO_SIM;
    }

#endif

    return PREDIAL_OK;
}

/* predial_add_emergency_number()
 *
 * descr: Adds a number to the internal AT list that will be validate if a dial string
 * from the ATD command is an emergency number
 *
 * Note that there currently no way to call this function, so a new AT command is needed
 * for that
 *
 * The list lives as long the process using it and are not freed unless an entry is removed
 * by the upper layers using an AT command calling predial_remove_emergency_dial_string,
 * so this might look like a memory leak in some tools.
 */

bool predial_add_emergency_dial_string(char *dial_string_p)
{
    dynamic_emergency_dial_string_list_t *new_p;
    dynamic_emergency_dial_string_list_t *last_p = NULL;

    if (dial_string_p == NULL) {
        return false;
    }

    new_p = (dynamic_emergency_dial_string_list_t *)malloc(sizeof(dynamic_emergency_dial_string_list_t));
    new_p->emergency_dial_string_p = (char *)malloc(strlen(dial_string_p)+1);
    strcpy(new_p->emergency_dial_string_p, dial_string_p);
    new_p->next_p = NULL;

    if (NULL == dynamic_emergency_dial_string_list_p) {
        dynamic_emergency_dial_string_list_p = new_p;
    } else {
        last_p = dynamic_emergency_dial_string_list_p;

        while (NULL != last_p->next_p) {
            last_p = last_p->next_p;
        }

        last_p->next_p = new_p;
    }

    return true;
}

predial_remove_dial_string_response_t predial_remove_emergency_dial_string(char *dial_string_p)
{
    predial_remove_dial_string_response_t result = PREDIAL_REMOVE_OK;
    dynamic_emergency_dial_string_list_t *current_p;
    dynamic_emergency_dial_string_list_t *previous_p = NULL;

    if (dial_string_p != NULL) {
        current_p = dynamic_emergency_dial_string_list_p;

        while ((current_p != NULL) && strcmp(dial_string_p, current_p->emergency_dial_string_p)) {
            previous_p = current_p;
            current_p = current_p->next_p;
        }

        if (current_p == NULL) {
            result = PREDIAL_REMOVE_ERROR_NO_MATCH;
        }
        /* found entry to remove */
        else {
            /* If previous_p is NULL it is only one entry in the list,
             * set dynamic_emergency_dial_string_list_p to NULL */
            if (previous_p == NULL) {
                dynamic_emergency_dial_string_list_p = NULL;
            } else {
                /* current_p->next_p could be NULL but that is safe */
                previous_p->next_p = current_p->next_p;
            }

            free(current_p->emergency_dial_string_p);
            free(current_p);
        }
    } else {
        result = PREDIAL_REMOVE_ERROR;
    }

    return result;
}

static bool check_if_always_set_emergency_dial_string(char *dial_string_p)
{
    if (!strcmp(dial_string_p, ALWAYS_SET_EMERGENCY_DIAL_STRING_112) ||
            !strcmp(dial_string_p, ALWAYS_SET_EMERGENCY_DIAL_STRING_911)) {
        return true;
    }

    return false;
}

static bool check_if_no_sim_set_emergency_dial_string(char *dial_string_p)
{
    if (!strcmp(dial_string_p, NO_SIM_SET_EMERGENCY_DIAL_STRING_08) ||
            !strcmp(dial_string_p, NO_SIM_SET_EMERGENCY_DIAL_STRING_000) ||
            !strcmp(dial_string_p, NO_SIM_SET_EMERGENCY_DIAL_STRING_110) ||
            !strcmp(dial_string_p, NO_SIM_SET_EMERGENCY_DIAL_STRING_118) ||
            !strcmp(dial_string_p, NO_SIM_SET_EMERGENCY_DIAL_STRING_119) ||
            !strcmp(dial_string_p, NO_SIM_SET_EMERGENCY_DIAL_STRING_999)) {
        return true;
    }

    return false;
}

static bool check_if_dynamic_emergency_dial_string(char *dial_string_p)
{
    dynamic_emergency_dial_string_list_t *current_p;
    current_p = dynamic_emergency_dial_string_list_p;

    while (current_p != NULL) {
        if (!strcmp(current_p->emergency_dial_string_p, dial_string_p)) {
            return true;
        }

        current_p = current_p->next_p;
    }

    return false;
}

#if 0
/* The two below functions are just skeleton coded and need to be aligned with the sim
 * implementation once that is in place, it will not even build at the moment is do not
 * change the #if 0 above
 */
static bool check_if_ecc_emergency_dial_string(char *dial_string_p)
{
    int i = 0;
    int number_of_entries;
    sim_response_status_t some_status_response;
    sim_ecc_response_t *sim_response_p;
    sim_read_ecc(sim_response_p, &some_status_response, &number_of_entries);

    if (0 == some_status_response) {  /* OK */
        /* Loop over the returned values, right now it has a bad syntax on purpose,
         * just to give an idea. Change once we have the sim type */
        for (i; i < number_of_entries; i++) {
            /

            if (strcpm(dial_string_p, sim_response_p[i])) {
                return true;
            }
        }
    }

    return false;
}


static bool check_if_allowed_according_to_fdn_settings(char *dial_string_p)
{
    int i = 0;
    int number_of_entries;
    sim_response_status_t some_status_response;
    sim_fdn_response_t *sim_fdn_response_p;
    sim_read_fdn(sim_fdn_response_p, &some_status_response, &number_of_entries);

    if (0 == some_status_response) {  /* OK */
        /* Check if we only have a set of allowed numbers */
        if (sim_response_p->allowed_numbers_list == TRUE) {
            for (i; i < number_of_entries; i++) {
                /* Bad syntax, change when we have the sim type */
                if (strcpm(dial_string_p, sim_fdn_response_p[i])) {
                    return true;
                }
            }

            return false;
        }

        /* Check the dial_string is not a barred number */
        if (sim_response_p->barred_numbers_list == TRUE) {
            for (i; i < number_of_entries; i++) {
                /* Bad syntax, change when we have the sim type */
                if (strcpm(dial_string_p, sim_fdn_response_p[i])) {
                    return false;
                }
            }
        }

        return true;
    }

    /* Include the SIM response some way here? */
    return false;
}

static sim_bake_response_t check_and_aligned_the_dial_string(char *dial_string_p)
{
    /* todo: We need to wait for the sim implementation to see what data they need as
     * input. We will either hard code it or, if not possible, we need to retrieve
     * the data from below layers. So this function could possible be a lot more
     * complex
     */
    return sim_request_validate_and_modify_dial_string(dial_string_p);
}
#endif

