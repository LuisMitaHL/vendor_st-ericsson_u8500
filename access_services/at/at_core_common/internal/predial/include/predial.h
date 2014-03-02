/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef PREDIAL_H
#define PREDIAL_H 1

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#define ALWAYS_SET_EMERGENCY_DIAL_STRING_112 "112"
#define ALWAYS_SET_EMERGENCY_DIAL_STRING_911 "911"

#define NO_SIM_SET_EMERGENCY_DIAL_STRING_08 "08"
#define NO_SIM_SET_EMERGENCY_DIAL_STRING_000 "000"
#define NO_SIM_SET_EMERGENCY_DIAL_STRING_110 "110"
#define NO_SIM_SET_EMERGENCY_DIAL_STRING_118 "118"
#define NO_SIM_SET_EMERGENCY_DIAL_STRING_119 "119"
#define NO_SIM_SET_EMERGENCY_DIAL_STRING_999 "999"

/* todo: Align with possible response codes from sim once
 * the module is updated
 * include the sim actual state?
 */
typedef enum {
    PREDIAL_OK,
    PREDIAL_ERROR,
    PREDIAL_ERROR_NO_SIM,
    PREDIAL_ERROR_INVALID_DIAL_STRING,
    PREDIAL_ERROR_NUMBER_NOT_ALLOWED,
    PREDIAL_ERROR_INVALID_NUMBER_FORMAT
} predial_response_code_t;

typedef enum {
    PREDIAL_REMOVE_OK,
    PREDIAL_REMOVE_ERROR,
    PREDIAL_REMOVE_ERROR_NO_MATCH
} predial_remove_dial_string_response_t;

typedef struct s_ {
    char *emergency_dial_string_p;
    struct s_* next_p;
} dynamic_emergency_dial_string_list_t;

static dynamic_emergency_dial_string_list_t *dynamic_emergency_dial_string_list_p = NULL;

predial_response_code_t voice_predial_checks(char *dial_string_p, bool *is_emergency_p);
bool predial_add_emergency_dial_string(char *dial_string_p);
predial_remove_dial_string_response_t predial_remove_emergency_dial_string(char *dial_string_p);

#endif
