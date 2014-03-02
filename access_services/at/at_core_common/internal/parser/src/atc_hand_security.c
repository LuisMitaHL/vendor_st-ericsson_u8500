/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/* ATC related header files */
#include <atc_config.h>
#include <atc_exe_glue.h>
#include <atc_log.h>
#include <atc_handlers.h>
#include <atc_parser.h>
#include <atc_parser_util.h>
#include <atc_string.h>
#include <exe.h>

/* Standard C header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <c_type.h>

typedef struct {                /* Structure for facility lock : */

    char                   *facility_p;      /* ... lock code defined in GSM 07.07.           */
    bool                    has_passwd;     /* ... the facility has a changeable password.   */
    bool                    clck_support;   /*   Facility supported by command CLCK        */
    bool                    cpwd_support;   /*   Facility supported by command CPWD        */
    unsigned char           min_length;     /* ... min length of the facility lock password. */
    unsigned char           max_length;     /* ... max length of the facility lock password. */
    atc_facility_locks_t       code;           /* ... enumeration of the facility lock.        */
    exe_request_id_t        clck_exe_id;         /* id for calling executer */
    exe_request_id_t        cpwd_exe_id;         /* id for calling executer */
} facility_locks_table_record_s;

/* Define a table for ME and network facility locks.                         */
static const facility_locks_table_record_s facility_locks_table[] = {
    {"CS", false, false, false, 4, 8,  ATC_FAC_CNTRL,     EXE_CLCK_CS, EXE_CPWD_NONE},
    {"PS", true,  false, false, 4, 8,  ATC_FAC_PH_SIM,    EXE_CLCK_PS, EXE_CPWD_NONE},
    {"PF", false, false, false, 4, 8,  ATC_FAC_PH_FSIM,   EXE_CLCK_PF, EXE_CPWD_NONE},
    {"SC", true,  true,  true , 4, 8,  ATC_FAC_SIM_PIN,   EXE_CLCK_SC, EXE_CPWD_SC},
    {"P2", true,  false, true , 4, 8,  ATC_FAC_SIM_PIN2,  EXE_CLCK_P2, EXE_CPWD_P2},
    {"AO", true,  true,  true,  4, 8,  ATC_FAC_BAOC,      EXE_CLCK_BARRING, EXE_CPWD_BARRING},
    {"OI", true,  true,  true,  4, 8,  ATC_FAC_BOIC,      EXE_CLCK_BARRING, EXE_CPWD_BARRING},
    {"OX", true,  true,  true,  4, 8,  ATC_FAC_BOIC_exHC, EXE_CLCK_BARRING, EXE_CPWD_BARRING},
    {"AI", true,  true,  true,  4, 8,  ATC_FAC_BAIC,      EXE_CLCK_BARRING, EXE_CPWD_BARRING},
    {"IR", true,  true,  true,  4, 8,  ATC_FAC_BAIC_Roam, EXE_CLCK_BARRING, EXE_CPWD_BARRING},
    {"NT", false, false, false, 4, 8,  ATC_FAC_BInotTA,   EXE_CLCK_NT, EXE_CPWD_NONE},
    {"NM", false, false, false, 4, 8,  ATC_FAC_BInotME,   EXE_CLCK_NM, EXE_CPWD_NONE},
    {"NS", false, false, false, 4, 8,  ATC_FAC_BInotSIM,  EXE_CLCK_NS, EXE_CPWD_NONE},
    {"NA", false, false, false, 4, 8,  ATC_FAC_BInotAny,  EXE_CLCK_NA, EXE_CPWD_NONE},
    {"AB", true,  true,  true,  4, 8,  ATC_FAC_BAll,      EXE_CLCK_BARRING, EXE_CPWD_BARRING},
    {"AG", true,  true,  true,  4, 8,  ATC_FAC_BAllOut,   EXE_CLCK_BARRING, EXE_CPWD_BARRING},
    {"AC", true,  true,  true,  4, 8,  ATC_FAC_BAllIn,    EXE_CLCK_BARRING, EXE_CPWD_BARRING},
    {"PN", false, true,  false, 4, 16, ATC_FAC_NetPer,    EXE_CLCK_PN, EXE_CPWD_NONE},
    {"PU", false, true,  false, 4, 16, ATC_FAC_NetUbPer,  EXE_CLCK_PU, EXE_CPWD_NONE},
    {"PP", false, true,  false, 4, 16, ATC_FAC_SerPer,    EXE_CLCK_PP, EXE_CPWD_NONE},
    {"PC", false, true,  false, 4, 16, ATC_FAC_CorPer,    EXE_CLCK_PC, EXE_CPWD_NONE},
    {"LL", false, false, false, 4, 4,  ATC_FAC_LL,        EXE_CLCK_LL, EXE_CPWD_NONE},    /* ALS (LL is PIN2 protected, PIN2 is 4 chars long). */
    {"FD", false, true,  false, 4, 8,  ATC_FAC_FDN,       EXE_CLCK_FD, EXE_CPWD_NONE},
    {"EL", false, true,  false, 4, 16,  ATC_FAC_ESL,      EXE_CLCK_EL, EXE_CPWD_NONE}
};

static const unsigned char FACILLITY_LOCKS_TABLE_LENGTH =
    (sizeof(facility_locks_table)
     / sizeof(facility_locks_table_record_s));


static exe_cops_auth_data_t *authenticate_data_p = NULL;
static exe_cops_simlock_data_t *simlock_data_p = NULL;

/*
 *=====================================================================
 *
 *  Function: facility_get_lock_code
 *
 *  Input:  facility -
 *
 *  Output: facility -
 *
 *  The function returns the facility lock code associated with
 *  string type facility 'Facility'.
 *
 *  Example: 'facility' = "OI" -> the function returns code FAC_BOIC.
 *
 * ==================================================================
 */
static atc_facility_locks_t facility_get_lock_code(char *facility_p)
{
    unsigned char i;

    if (*facility_p != END_OF_STRING) {
        for (i = 0; i < FACILLITY_LOCKS_TABLE_LENGTH; i++) {
            if ((facility_locks_table[i].facility_p[0] ==
                    to_upper(facility_p[0]))
                    && (facility_locks_table[i].facility_p[1]
                        == to_upper(facility_p[1]))) {
                return facility_locks_table[i].code;
            }
        }
    }

    return ATC_FAC_ERROR;
}

/*
 *=====================================================================
 *
 *  Function: facility_locks_table_record_s
 *
 *  Input:  code  - Facility lock code, e.g. FAC_BAOC, FAC_BAIC or
 *                  FAC_BAIC_Roam.
 *
 *  Output: -
 *
 *  The function returns a pointer to the facility-locks table entry
 *  for code 'code'.
 *
 *  Example: For 'code' == FAC_BOIC, the function returns a pointer to
 *      the entry { "OI", TRUE,  4, 8, FAC_BOIC }.
 *
 * ==================================================================
 */
static const facility_locks_table_record_s *get_facility_lock_entry(atc_facility_locks_t code)
{
    unsigned char i;

    if (code < FACILLITY_LOCKS_TABLE_LENGTH) {
        if (facility_locks_table[code].code == code) {
            return &facility_locks_table[code];
        } else {
            for (i = 0; i < FACILLITY_LOCKS_TABLE_LENGTH; i++) {
                if (facility_locks_table[i].code == code) {
                    return &facility_locks_table[i];
                }
            }
        }
    }

    return NULL;
}
/*
 *=====================================================================
 *
 *  Function: facility_get_clck_facility_list
 *
 *  Input:  buffer_p  - Pointer to empty string type buffer for
 *                      storage of the result (facility list).
 *          all       - Boolean type flag used to indicate that all
 *                      facilities are wanted.
 *
 *  Output: buffer_p  - Pointer to buffer containing a string type list
 *                      of supported lock facilities.
 *
 *  The function returns a string type list of supported lock
 *  facilities.
 *
 * ==================================================================
 */
static void facility_get_clck_facility_list(char *buffer_p, bool all)
{
    unsigned char i;

    strcat(buffer_p, " (\"");

    for (i = 0; i < FACILLITY_LOCKS_TABLE_LENGTH; i++) {
        if ((all || facility_locks_table[i].has_passwd)
                && (facility_locks_table[i].clck_support)) {
            strcat(buffer_p, facility_locks_table[i].facility_p);
            strcat(buffer_p, "\",\"");
        }
    }

    buffer_p[strlen(buffer_p) - 2] = END_OF_STRING;
    strcat(buffer_p, ")");
}

/*
 *=====================================================================
 *
 *  Function: facility_get_cpwd_facility_list
 *
 *  Input:  buffer_p  - Pointer to empty string type buffer for
 *                    storage of the result (facility list).
 *          all       - Boolean type flag used to indicate that all
 *                    facilities are wanted.
 *
 *  Output: buffer_p  - Pointer to buffer containing a string type list
 *                    of supported lock facilities.
 *
 *  The function returns a string type list of supported lock
 *  facilities.
 *
 * ==================================================================
 */
static void facility_get_cpwd_facility_list(AT_CommandLine_t buffer_p, bool all)
{
    unsigned char i;

    strcat((char *)buffer_p, "(\"");

    for (i = 0; i < FACILLITY_LOCKS_TABLE_LENGTH; i++) {
        if ((all || facility_locks_table[i].has_passwd)
                && (facility_locks_table[i].cpwd_support)) {
            strcat((char *)buffer_p, facility_locks_table[i].facility_p);
            AT_AddValue(buffer_p, "\",0),(\"", facility_locks_table[i].max_length);
        }
    }

    buffer_p[strlen((char *)buffer_p) - 3] = END_OF_STRING;
}

exe_request_id_t get_clck_exe_id(atc_facility_locks_t code)
{
    unsigned char i;

    for (i = 0; i < FACILLITY_LOCKS_TABLE_LENGTH; i++) {
        if (facility_locks_table[i].code == code) {
            return facility_locks_table[i].clck_exe_id;
        }
    }

    return EXE_LAST;
}

exe_request_id_t get_cpwd_exe_id(atc_facility_locks_t code)
{
    unsigned char i;

    for (i = 0; i < FACILLITY_LOCKS_TABLE_LENGTH; i++) {
        if (facility_locks_table[i].code == code) {
            return facility_locks_table[i].cpwd_exe_id;
        }
    }

    return EXE_LAST;
}

char *get_facility_string(atc_facility_locks_t code)
{
    unsigned char i;

    for (i = 0; i < FACILLITY_LOCKS_TABLE_LENGTH; i++) {
        if (facility_locks_table[i].code == code) {
            return facility_locks_table[i].facility_p;
        }
    }

    return NULL;
}

/*
 *=====================================================================
 *
 *  Function: facility_validate_password_length
 *
 *  Input:  facility   - The facility for which the password length
 *                        shall be verified.
 *          length      - The password length to be verified.
 *
 *  Output: -
 *
 *  The function returns 'true' if the submitted password length 'length'
 *  is less than or equal to the maximum password length for
 *  the given facility 'facility'; 'false' otherwise.
 *
 * ==================================================================
 */
bool facility_validate_password_length(atc_facility_locks_t facility,
                                       unsigned char length)
{
    const facility_locks_table_record_s *record_p =
        get_facility_lock_entry(facility);

    if (record_p != NULL) {
        if ((length >= record_p->min_length) && (length <= record_p->max_length)) {
            return true;
        }
    }

    return false;
}


static void handle_response(AT_CommandLine_t info_text,
                            exe_clck_t *clck_res_p,
                            AT_ParserState_s *parser_p)
{
    int classes[] = {EXE_CLASSX_VOICE, EXE_CLASSX_DATA, EXE_CLASSX_FAX,
                     EXE_CLASSX_SMS, EXE_CLASSX_DATA_CIRCUIT_SYNC,
                     EXE_CLASSX_DATA_CIRCUIT_ASYNC, EXE_CLASSX_PACKET_ACCESS,
                     EXE_CLASSX_PAD_ACCESS
                    };
    int nr_of_classes = sizeof(classes) / sizeof(int);
    int i = 0;
    int status = 0;
    int last_response = 0;
    int req_class = (parser_p->clck_class == EXE_CLASSX_DEFAULT) ?
            EXE_CLASSX_MASK_SMS : parser_p->clck_class;
    int enabled_class = clck_res_p->class;

    /* If class 2 is included we need to enable class 16-128 */
    req_class = (req_class & 0x2) ? (req_class | 0xF0) : req_class;

    /* Determine the index of the class that will be the last to be reported */
    for (i = 0; i < nr_of_classes; i++) {
        if ((EXE_CLASSX_DATA == classes[i]) ||
            ((classes[i] == EXE_CLASSX_SMS) && !(req_class & EXE_CLASSX_SMS))) {
            continue;
        }

        status = ((enabled_class & classes[i]) && (req_class & classes[i]))
                ? 1 : 0; /* Enabled and requested only */

        if (EXE_CLCK_STATUS_ACTIVE != clck_res_p->lock_status ||
                (!status && req_class == classes[i])) {
            if (req_class & classes[i]) {
                last_response = i;
            }
        } else if (status) {
            last_response = i;
        }
    }

    for (i = 0; i < nr_of_classes; i++) {
        /*
         * do not describe the data class since its
         * descendants are listed in detail (16,32,64,128).
         * Also skip SMS if not requested.
         */
        if ((EXE_CLASSX_DATA == classes[i]) ||
           ((classes[i] == EXE_CLASSX_SMS) && !(req_class & EXE_CLASSX_SMS))) {
            continue;
        }

        status = ((enabled_class & classes[i]) && (req_class & classes[i]))
                ? 1 : 0; /* Enabled and requested only */

        if (EXE_CLCK_STATUS_ACTIVE != clck_res_p->lock_status ||
                (!status && req_class == classes[i])) {
            if (req_class & classes[i]) {
                sprintf((char *) info_text, ": %d,%d", 0, classes[i]);
                Parser_SendMultipleResponses(parser_p, AT_NO_OVERRIDE, info_text,
                        (i == last_response));
            }
        } else if (status) {
            sprintf((char *) info_text, ": %d,%d", status, classes[i]);
            Parser_SendMultipleResponses(parser_p, AT_NO_OVERRIDE, info_text,
                    (i == last_response));
        }
    } /* end of for loop */
}

static void free_static_cops_auth_data()
{
    if (authenticate_data_p) {
        free(authenticate_data_p->rsa_challenge_response_p);
        free(authenticate_data_p->nl_key_p);
        free(authenticate_data_p->nsl_key_p);
        free(authenticate_data_p->spl_key_p);
        free(authenticate_data_p->cl_key_p);
        free(authenticate_data_p->esl_key_p);
        free(authenticate_data_p);
        authenticate_data_p = NULL;
    }
}

static void free_simlock_data(exe_cops_simlock_data_t *exe_simlock_data_p)
{
    if (exe_simlock_data_p) {
        free(exe_simlock_data_p->simlock_data);
        free(exe_simlock_data_p);
    }
}

static void free_static_cops_simlock_data()
{
    if (simlock_data_p) {
        free_simlock_data(simlock_data_p);
        simlock_data_p = NULL;
    }
}

static bool is_clck_cops_simlock_restart_resp(exe_clck_mode_t clck_mode, atc_facility_locks_t facility_code)
{
    if (EXE_CLCK_MODE_UNLOCK == clck_mode) {
        if (ATC_FAC_NetPer == facility_code ||
                ATC_FAC_NetUbPer == facility_code ||
                ATC_FAC_SerPer == facility_code ||
                ATC_FAC_CorPer == facility_code ||
                ATC_FAC_ESL == facility_code) {
            return true;
        }
    }

    return false;
}

/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CLCK_Handle
 *
 *   INPUT:   parser_p  - Pointer to the current parser state.
 *            info_text - Pointer to a string buffer to put information text.
 *
 *   OUTPUT:  parser_p->
 *              Pos    - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There where a general error.
 *            AT_CME_ERROR - There where a GSM specific error, the error code is
 *                           returned in ASCII format in info_text.
 *            AT_PENDING   - The command is not ready but returns while
 *                           waiting for an asynchronous response.
 *
 *   The CLCK command is used is used to lock, unlock or interrogate a ME or a
 *   network facility <fac>. Password is normally needed to do such actions.
 *
 *   SET  - Locks, unlocks or updates a ME or network facility lock:
 *          +CLCK=<fac>,<mode>[,<passwd>[,<class>]]
 *            <fac> is the abbreviation for the requested facility lock.
 *            <mode> is the mode for the set command.
 *            <passwd> the password as defined by +CPWD or default password.
 *            <class> bit masc for the bearer capabilities class.
 *            <fac> and <passwd> are string type values, the other parameters are
 *                  integer type values.
 *          SET::QUIRY STATUS:
 *          +CLCK: <status1>,[<class1>
 *          [+CLCK: <status2>,[<class2>
 *          [...]]
 *            <status> is the current status (PAS_PLUS_CLCK_STATUS_ACTIVE or
 *                     PAS_PLUS_CLCK_STATUS_NOT_ACTIVE).
 *
 *   TEST - Writes the supported facility locks and the maximum length of each
 *          password to info_text:
 *          +CLCK: <fac1>,[<fac2>,[...]]
 *          <facx> is the abbreviation for the requested facility lock.
 *
 *

 *===============================================================================
 */
AT_Command_e AT_PLUS_CLCK_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    StrParam_t facility; /* Reference to the requested facility lock. */
    IntParam_t mode; /* Requested command mode. */
    StrParam_t passwd; /* Reference to the facility lock password. */
    IntParam_t class = EXE_CLASSX_DEFAULT; /* Requested bearer capability class. */
    bool error_flag = false; /* Flags an error. */
    AT_Command_e result = AT_OK;
    atc_facility_locks_t facility_code; /* Requested facility lock code. */
    exe_request_result_t exe_res = EXE_FAILURE;
    exe_t *exe = NULL;

    switch (parser_p->Mode) {
    case AT_MODE_SET:

        if (!parser_p->IsPending) {
            facility = Parser_GetStrParam(parser_p, &error_flag, NULL);

            if (error_flag) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                result = AT_CME_ERROR;
                goto error;
            }

            facility_code = facility_get_lock_code((char *) facility);

            if (facility_code == ATC_FAC_ERROR) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_SUPPORTED);
                result = AT_CME_ERROR;
                goto error;
            }

            mode = Parser_GetIntParam(parser_p, &error_flag, NULL);

            if (error_flag) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                result = AT_CME_ERROR;
                goto error;
            }

            if (EXE_CLCK_MODE_QUERY_STATUS < mode) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                result = AT_CME_ERROR;
                goto error;
            }

            /* Facility "CS" supports only <mode> parameter
             * Check if the user by mistake sends more parameters for CNTRL!*/
            if ((ATC_FAC_CNTRL == facility_code) && (!parser_p->NoMoreParams)) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_SUPPORTED);
                return AT_CME_ERROR;
            }

            passwd = Parser_GetStrParam(parser_p, &error_flag, (StrParam_t) "");

            if (error_flag) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                result = AT_CME_ERROR;
                goto error;
            }

            class = Parser_GetIntParam(parser_p, &error_flag, &class);

            if (error_flag) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                result = AT_CME_ERROR;
                goto error;
            }

            if (EXE_CLASSX_VOICE > class) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                result = AT_CME_ERROR;
                goto error;
            }

            exe_clck_t exe_data;
            exe_data.mode = mode;
            exe_data.passwd_p = passwd;
            exe_data.class = class;
            /* Clear response data */
            exe_data.lock_status = 0;
            exe_data.status = 0;
            exe_data.facility_p = NULL;

            exe_res = EXE_FAILURE;
            exe = atc_get_exe_handle();

            switch (facility_code) {
            case ATC_FAC_SIM_PIN:
            case ATC_FAC_FDN: {
                exe_res = exe_request(exe,
                                      get_clck_exe_id(facility_code),
                                      (void *) &exe_data,
                                      (void *) parser_p,
                                      &parser_p->request_handle);

                result = parser_handle_exe_result(exe_res, parser_p);
                break;
            }
            case ATC_FAC_BAOC:                    /*   Bar All Outgoing Calls. */
            case ATC_FAC_BOIC:                    /*   Bar Outgoing International Calls. */
            case ATC_FAC_BOIC_exHC:               /*   Bar Outgoing International Calls except to home country. */
            case ATC_FAC_BAIC:                    /*   Bar All Incoming Calls. */
            case ATC_FAC_BAIC_Roam:               /*   BIC-Roam, Bar Incoming Calls when Roaming outside the home country. */
            case ATC_FAC_BAll:                    /*   All Barring services. */
            case ATC_FAC_BAllOut:                 /*   All outGoing barring services. */
            case ATC_FAC_BAllIn: {                /*   All inComing barring services. */

                exe_data.facility_p = get_facility_string(facility_code);
                exe_res = exe_request(exe, get_clck_exe_id(facility_code), (void *) &exe_data,
                                      (void *)parser_p, &parser_p->request_handle);

                result = parser_handle_exe_result(exe_res, parser_p);

                break;
            }
            case ATC_FAC_NetPer:
            case ATC_FAC_NetUbPer:
            case ATC_FAC_SerPer:
            case ATC_FAC_CorPer:
            case ATC_FAC_ESL: {
                /* cops client, synchronous call*/
                exe_data.facility_p = get_facility_string(facility_code);
                exe_res = exe_request(exe, get_clck_exe_id(facility_code), (void *) &exe_data,
                                      (void *) parser_p, &parser_p->request_handle);

                result = parser_handle_exe_result(exe_res, parser_p);

                if (AT_OK != result) {
                    goto error;
                }

                if (EXE_CLCK_MODE_QUERY_STATUS == exe_data.mode) {
                    sprintf((char *) info_text, ": %d", exe_data.lock_status);
                    Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
                } else if (EXE_CLCK_MODE_UNLOCK == exe_data.mode) {
                    /* If a successful simlock unlock, then restart sim */
                    exe_res = exe_request(exe, EXE_SIM_RESTART, NULL, (void *)parser_p,
                                          &parser_p->request_handle);

                    ATC_LOG_D("EXE_SIM_RESTART request result: %d", exe_res);

                    result = parser_handle_exe_result(exe_res, parser_p);

                    if (result != AT_PENDING) {
                        goto error;
                    }
                }

                break;
            }

            default:
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_SUPPORTED);
                result = AT_CME_ERROR;
                goto error;
            }

            /* The CLCK command operates towards different sub parts and will
             * respond differently dependent on class and facility mode. It must also
             * distinguish between a query and lock/unlock.
             * To support this we must save the facility code, class and mode in the parser.
             *  */
            parser_p->facility_code = facility_code;
            parser_p->clck_mode = mode;
            parser_p->clck_class = class;

        } else {
            /* Response from SIM */
            bool have_extended_error_information = false;
            exe_clck_t *clck_data_p = NULL;

            if (parser_p->abort) {
                exe_request_result_t er = exe_request_abort(atc_get_exe_handle(), parser_p->request_handle);
                if (EXE_SUCCESS != er && EXE_PENDING != er) {
                    ATC_LOG_E("Abort of request failed!");
                }
                result = AT_OK;
                break;
            }

            result = parser_handle_exe_result(message->exe_result_code, parser_p);

            if (is_clck_cops_simlock_restart_resp(parser_p->clck_mode, parser_p->facility_code)) {
                if (AT_OK == result) {
                    ATC_LOG_D("EXE_SIM_RESTART response returned OK");
                } else {
                    if (NULL != message->data_p) {
                        Parser_SetCMEE_ErrorType(parser_p, *((exe_cmee_error_t *)message->data_p));
                        ATC_LOG_D("EXE_SIM_RESTART response cmee: %d", parser_p->CMEE_ErrorType);
                        goto error;
                    }
                }
            } else {
                clck_data_p = (exe_clck_t *)(message->data_p);

                if (AT_OK != result && NULL != message->data_p) {
                    have_extended_error_information = true;
                }

                if (AT_OK == result) {
                    if (EXE_CLCK_MODE_QUERY_STATUS == parser_p->clck_mode) {
                        switch (parser_p->facility_code) {
                        case ATC_FAC_FDN:
                        case ATC_FAC_SIM_PIN: {
                            if (!have_extended_error_information) {
                                /* FDN and SIM_PIN do not consider class when giving the status */

                                if (NULL == clck_data_p) {
                                    ATC_LOG_E("AT_PLUS_CLCK_Handle: clck_data_p is NULL");
                                    result = AT_CME_ERROR;
                                    Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                                    goto error;
                                }

                                if (CMEE_OK != clck_data_p->status) {
                                    result = AT_CME_ERROR;
                                    Parser_SetCMEE_ErrorType(parser_p, clck_data_p->status);
                                } else {
                                    sprintf((char *) info_text,
                                            ": %d", clck_data_p->lock_status);
                                    Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
                                }
                            }

                            break;
                        }
                        case ATC_FAC_BAOC:                    /*   Bar All Outgoing Calls. */
                        case ATC_FAC_BOIC:                    /*   Bar Outgoing International Calls. */
                        case ATC_FAC_BOIC_exHC:               /*   Bar Outgoing International Calls except to home country. */
                        case ATC_FAC_BAIC:                    /*   Bar All Incoming Calls. */
                        case ATC_FAC_BAIC_Roam:               /*   BIC-Roam, Bar Incoming Calls when Roaming outside the home country. */
                        case ATC_FAC_BAll:                    /*   All Barring services. */
                        case ATC_FAC_BAllOut:                 /*   All outGoing barring services. */
                        case ATC_FAC_BAllIn:                  /*   All inComing barring services. */
                            /* Fall through intended. Cases in place for readability */
                        default: {  /* Used for class driven response */
                            handle_response(info_text, (exe_clck_t *)(message->data_p), parser_p);
                            break;
                        }
                        }
                    } else {
                        switch (parser_p->facility_code) {
                        case ATC_FAC_FDN:
                        case ATC_FAC_SIM_PIN: {
                            if (clck_data_p == NULL) {
                                ATC_LOG_E("AT_PLUS_CLCK_Handle: clck_data_p is NULL");
                                Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                                result = AT_CME_ERROR;
                                goto error;
                            }

                            if (AT_OK == result && CMEE_OK != clck_data_p->status) {
                                if (parser_p->facility_code == ATC_FAC_SIM_PIN) {
                                    /* To adapt to PTCRB test demands we will give the error code
                                     * CMEE_INCORRECT_PASSWORD. Only when PIN1 related */
                                    if (clck_data_p->status == CMEE_SIM_PIN_REQUIRED) {
                                        clck_data_p->status = CMEE_INCORRECT_PASSWORD;
                                    } else if ( clck_data_p->status == CMEE_SIM_BLOCKED ) {
                                        clck_data_p->status = CMEE_SIM_PUK_REQUIRED;
                                    }
                                }

                                if (parser_p->facility_code == ATC_FAC_FDN) {
                                    /* Fixed dialing is secured by PIN2 and PUK2.
                                     * SIM only gives a general error code that indicates
                                     * that PIN or PUK is needed. We have to test here if it is PIN2 and PUK2
                                     * for Fixed dialing
                                     */
                                    if (clck_data_p->status == CMEE_SIM_PIN_REQUIRED) {
                                        clck_data_p->status = CMEE_SIM_PIN2_REQUIRED;
                                    } else if (clck_data_p->status == CMEE_SIM_PUK_REQUIRED ||
                                               clck_data_p->status == CMEE_SIM_BLOCKED) {
                                        clck_data_p->status = CMEE_SIM_PUK2_REQUIRED;
                                    }
                                }

                                Parser_SetCMEE_ErrorType(parser_p, clck_data_p->status);
                                result = AT_CME_ERROR;
                                goto error;
                            }
                        }
                        default: {
                            break;
                        }
                        }
                    }
                } else if (have_extended_error_information) {
                    Parser_SetCMEE_ErrorType(parser_p, clck_data_p->status);
                    result = AT_CME_ERROR;
                }
            }
        }

        break;

    case AT_MODE_TEST:
        sprintf((char *) info_text, ":");
        facility_get_clck_facility_list((char *) info_text, true);
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;
    default:
        Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_SUPPORTED);
        result = AT_CME_ERROR;
        break;
    }

error:
    return result;
}

/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CPWD_Handle
 *
 *   INPUT:   parser_p - Pointer to the current parser state.
 *            info_text - Pointer to a string buffer to put information text.
 *
 *   OUTPUT:  parser->
 *              Pos    - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There where a general error.
 *            AT_CME_ERROR - There where a GSM specific error, the error code is
 *                           returned in ASCII format in InfoText.
 *            AT_PENDING   - The command is not ready but returns while
 *                           waiting for an asynchronous response.
 *
 *   The CPWD command is used to set a new password for the facility lock function
 *   defined by command Facility Lock +CLCK.
 *
 *   SET  - Updates the password for the given facility:
 *          +CPWD=<fac>,<oldpwd>,<newpwd>
 *            <fac> is the abbreviation for the requested facility lock.
 *            <oldpwd> is the current password.
 *            <newpwd> is the new password.
 *            All parameters are string type values.
 *
 *   TEST - Writes the supported facility locks and the maximum length of each
 *          password to InfoText:
 *          +CPWD=(<fac1>,<pwdlength1>),[(<fac2>,<pwdlength2>),[...]]
 *            <pwdlength> is the maximum length for the facility lock password.
 *            <pwdlength> is an integer type.
 *
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CPWD_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    exe_cmee_error_t current_cmee = CMEE_OK;
    AT_Command_e result = AT_OK;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        if (!parser_p->IsPending) {
            StrParam_t facility;       /* Reference to the <fac> parameter.    */
            StrParam_t old_pwd;         /* Reference to the <oldpwd> parameter. */
            StrParam_t new_pwd;         /* Reference to the <newpwd> parameter. */
            bool error_flag = false;    /* Flags a parameter parsing error.     */

            atc_facility_locks_t facility_code; /* Requested facility lock code.  */

            facility = Parser_GetStrParam(parser_p, &error_flag, NULL);
            old_pwd = Parser_GetStrParam(parser_p, &error_flag, NULL);
            new_pwd = Parser_GetStrParam(parser_p, &error_flag, NULL);

            /* Last Parameter received. Check if the user by mistake sends more parameters! */
            if (!parser_p->NoMoreParams) {
                current_cmee = CMEE_INCORRECT_PARAMETERS;
                result = AT_CME_ERROR;
                goto error;
            }

            /* error_flag is set true when:
             no default value was given and the parameter was left out
             or if there was an error in the parameter syntax. */
            if (error_flag) {
                current_cmee = CMEE_INCORRECT_PARAMETERS;
                result = AT_CME_ERROR;
                goto error;
            }

            facility_code = facility_get_lock_code((char *) facility);

            if (ATC_FAC_ERROR == facility_code) {
                current_cmee = CMEE_OPERATION_NOT_SUPPORTED;
                break;
            }

            /* Validate the password parameters. */
            if (!facility_validate_password_length(facility_code, (unsigned char) strlen((char const *) old_pwd)) ||
                    !facility_validate_password_length(facility_code, (unsigned char) strlen((char const *) new_pwd))) {
                current_cmee = CMEE_INCORRECT_PASSWORD;
                break;
            }

            exe_t *exe_p = atc_get_exe_handle();
            exe_request_result_t exe_res = EXE_FAILURE;
            exe_change_pin_t exe_data;
            exe_data.new_passwd_p = new_pwd;
            exe_data.old_passwd_p = old_pwd;
            exe_data.facility_p   = NULL;

            /* Clear data used in the response */
            exe_data.error_code = CMEE_OK;

            switch (facility_code) {
            case ATC_FAC_SIM_PIN: {
                exe_data.pin_id = EXE_SIM_PIN_ID_PIN1;
                break;
            }
            case ATC_FAC_SIM_PIN2: {
                exe_data.pin_id = EXE_SIM_PIN_ID_PIN2;
                break;
            }
            /* Call Barring password change is governed by ETSI TS 122.030 defining ZZ=330 (All Barring) as only
             * service for which change password procedure is valid. This contradicts the referring statement of
             * ETSI TS 127.007 where the <fac>s of +CPWD is to be found in +CLCK command list of <fac>s.
             * As AT commands are defined (as an API) from what is stated in 127.007 facility will be hard coded
             * to "AB" for all <fac>s relating to Call Barring.*/
            case ATC_FAC_BAOC:
            case ATC_FAC_BOIC:
            case ATC_FAC_BOIC_exHC:
            case ATC_FAC_BAIC:
            case ATC_FAC_BAIC_Roam:
            case ATC_FAC_BAllOut:
            case ATC_FAC_BAllIn: /* Fall through intentional */
            case ATC_FAC_BAll: {
                exe_data.facility_p = "AB";
                break;
            }

            default: {
                current_cmee = CMEE_OPERATION_NOT_SUPPORTED;
                goto error;
                break;
            }
            }

            exe_res = exe_request(exe_p, get_cpwd_exe_id(facility_code), (void *) &exe_data,
                                  (void *) parser_p, &parser_p->request_handle);
            result = parser_handle_exe_result(exe_res, parser_p);

            /* The CPWD command operates towards different sub parts and will
             * respond differently dependent on facility mode.
             *  */
            parser_p->facility_code = facility_code;

            break;
        } else { /* parser_p->IsPending */
            result = parser_handle_exe_result(message->exe_result_code, parser_p);

            if (AT_OK == result || NULL != message->data_p) {
                exe_change_pin_t *exe_cpwd_result_p = (exe_change_pin_t *)(message->data_p);
                current_cmee = exe_cpwd_result_p->error_code;

                if (parser_p->facility_code == ATC_FAC_SIM_PIN2) {
                    /* Changing PIN2
                     * SIM only gives a general error code that indicates
                     * that PIN or PUK is needed. We have to test here if it is PIN2 and PUK2
                     * for PIN2
                     */
                    if (current_cmee == CMEE_SIM_PIN_REQUIRED) {
                        current_cmee = CMEE_SIM_PIN2_REQUIRED;
                    } else if (current_cmee == CMEE_SIM_PUK_REQUIRED || current_cmee == CMEE_SIM_BLOCKED) {
                        current_cmee = CMEE_SIM_PUK2_REQUIRED;
                    }
                } else if ( parser_p->facility_code == ATC_FAC_SIM_PIN) {
                    if ( current_cmee == CMEE_SIM_BLOCKED ) {
                        current_cmee = CMEE_SIM_PUK_REQUIRED;
                    }
                }
            }

            break;
        }
    }

    case AT_MODE_TEST: {
        sprintf((char *) info_text, ": ");
        facility_get_cpwd_facility_list(info_text, false);
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        break;
    }

    default:
        current_cmee = CMEE_UNKNOWN;
        break;

    }

error:

    if (current_cmee != CMEE_OK) {
        Parser_SetCMEE_ErrorType(parser_p, current_cmee);
        return AT_CME_ERROR;
    }

    return result;

} /* End of AT_PLUS_CPWD_Handle */


/*
 *===============================================================================
 *
 *   Function: AT_STAR_ECPSCHBLKR_Handle
 *
 *   INPUT:   parser_p - Pointer to the current parser state.
 *            info_text - Pointer to a string buffer to put information text.
 *
 *   OUTPUT:  parser->
 *              Pos    - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There where a general error.
 *            AT_CME_ERROR - There where a GSM specific error, the error code is
 *                           returned in ASCII format in InfoText.
 *            AT_PENDING   - The command is not ready but returns while
 *                           waiting for an asynchronous response.
 *
 *   The ECPSCHBLKR command is used to retrieve RSA challenge data.
 *   The data will be represented in ASCII-HEX format.
 *
 *   DO   - Get Challenge Data.
 *
 *   TEST - Test command to show if the command is supported.
 *
 *
 *===============================================================================
 */
AT_Command_e AT_STAR_ECPSCHBLKR_Handle(AT_ParserState_s *parser_p,
                                       AT_CommandLine_t info_text,
                                       AT_ResponseMessage_t message)
{
    exe_cmee_error_t current_cmee = CMEE_OK;
    AT_Command_e result = AT_OK;
    exe_request_result_t exe_res = EXE_FAILURE;
    exe_t *exe = NULL;
    unsigned char *hex_str_p = NULL;
    size_t converted_size = 0;

    switch (parser_p->Mode) {
    case AT_MODE_DO: {
        exe_cops_challenge_t challenge;

        exe = atc_get_exe_handle();
        /* cops client, synchronous call */
        exe_res = exe_request(exe, EXE_GET_CHALLENGE_DATA, (void *) &challenge, (void *) parser_p,
                              &parser_p->request_handle);
        result = parser_handle_exe_result(exe_res, parser_p);

        if (AT_OK != result) {
            goto error;
        }

        /* Allocate the double size for ASCII format plus 1 for null termination */
        hex_str_p = (unsigned char *)calloc(1, challenge.challenge_len * 2 + 1);

        atc_string_convert(ATC_CHARSET_UNDEF_BINARY, challenge.rsa_challenge_data, challenge.challenge_len,
                           ATC_CHARSET_HEX, (uint8_t *)hex_str_p, &converted_size);

        sprintf((char *) info_text, ": %s", hex_str_p);
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        break;
    }

    case AT_MODE_TEST:
        result = AT_OK;
        break;
    default:
        current_cmee = CMEE_UNKNOWN;
        break;

    }

    free(hex_str_p);
    return result;

error:
    return result;
}

/*
 *===============================================================================
 *
 *   Function: AT_STAR_ECPSAUTHU_Handle
 *
 *   INPUT:   parser_p - Pointer to the current parser state.
 *            info_text - Pointer to a string buffer to put information text.
 *
 *   OUTPUT:  parser->
 *              Pos    - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There where a general error.
 *            AT_CME_ERROR - There where a GSM specific error, the error code is
 *                           returned in ASCII format in InfoText.
 *            AT_PENDING   - The command is not ready but returns while
 *                           waiting for an asynchronous response.
 *
 *   The ECPSAUTHU command is used to put the ME in an authenticated state for
 *   operations requiring authentication.
 *
 *   SET   - Authenticate/De-authenticate.
 *
 *   TEST - Test command to show if the command is supported.
 *
 *
 *===============================================================================
 */
AT_Command_e AT_STAR_ECPSAUTHU_Handle(AT_ParserState_s *parser_p,
                                      AT_CommandLine_t info_text,
                                      AT_ResponseMessage_t message)
{
    bool error_flag = false; /* Flags an error. */
    AT_Command_e result = AT_OK;
    uint8_t operation;
    uint8_t auth_type;
    exe_t *exe = NULL;
    exe_request_result_t exe_res = EXE_FAILURE;
    exe_cmee_error_t current_cmee = CMEE_OK;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {

        operation = Parser_GetIntParam(parser_p, &error_flag, NULL);

        if (error_flag) {
            Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
            result = AT_CME_ERROR;
            goto error;
        }

        switch (operation) {
        case COPS_OPERATION_TYPE_INIT: {
            auth_type = Parser_GetIntParam(parser_p, &error_flag, NULL);

            if (error_flag) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                result = AT_CME_ERROR;
                goto error;
            }

            if (auth_type > EXE_COPS_AUTH_TYPE_SIMLOCK) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                result = AT_CME_ERROR;
                goto error;
            }

            free_static_cops_auth_data();
            authenticate_data_p = calloc(1, sizeof(exe_cops_auth_data_t));
            authenticate_data_p->auth_type = auth_type;
            break;
        }
        case COPS_OPERATION_TYPE_COMPLETE: {

            if (NULL == authenticate_data_p) {
                ATC_LOG_E("%s: authenticate_data_p is NULL", __func__);
                current_cmee = CMEE_OP_NOT_APPLICABLE;
                goto error;
            }

            /* cops client, synchronous call */
            exe = atc_get_exe_handle();
            exe_res = exe_request(exe, EXE_COPS_AUTHENTICATE, (void *) authenticate_data_p, (void *) parser_p,
                                  &parser_p->request_handle);
            result = parser_handle_exe_result(exe_res, parser_p);

            if (AT_OK != result) {
                goto error;
            }

            free_static_cops_auth_data();
            break;
        }
        case COPS_OPERATION_TYPE_ABORT: {
            /* cops client, synchronous call */
            exe = atc_get_exe_handle();
            exe_res = exe_request(exe, EXE_COPS_DEAUTHENTICATE, NULL,
                                  (void *) parser_p, &parser_p->request_handle);
            result = parser_handle_exe_result(exe_res, parser_p);

            if (AT_OK != result) {
                goto error;
            }

            free_static_cops_auth_data();
            break;
        }
        default:
            goto error;
            break;
        }

        break;
    }
    case AT_MODE_TEST: {
        break;
    }
    default:
        current_cmee = CMEE_UNKNOWN;
        break;
    }

    return result;

error:

    if (current_cmee != CMEE_OK) {
        Parser_SetCMEE_ErrorType(parser_p, current_cmee);
        return AT_CME_ERROR;
    }

    return result;
}

/*
 *===============================================================================
 *
 *   Function: AT_STAR_ECPSAUTHDU_Handle
 *
 *   INPUT:   parser_p - Pointer to the current parser state.
 *            info_text - Pointer to a string buffer to put information text.
 *
 *   OUTPUT:  parser->
 *              Pos    - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There where a general error.
 *            AT_CME_ERROR - There where a GSM specific error, the error code is
 *                           returned in ASCII format in InfoText.
 *            AT_PENDING   - The command is not ready but returns while
 *                           waiting for an asynchronous response.
 *
 *   The ECPSAUTHDU command is used to enter authentication data for authentication.
 *   The authentication data supplied is either all the SIMLock
 *   control keys or a signed challenge that COPS has issued
 *   and the caller retrieved prior to calling this function.
 *
 *   SET   - Enter authentication data
 *
 *   TEST - Test command to show if the command is supported.
 *
 *
 *===============================================================================
 */
AT_Command_e AT_STAR_ECPSAUTHDU_Handle(AT_ParserState_s *parser_p,
                                       AT_CommandLine_t info_text,
                                       AT_ResponseMessage_t message)
{
    bool error_flag = false; /* Flags an error. */
    AT_Command_e result = AT_OK;
    exe_cmee_error_t current_cmee = CMEE_OK;
    StrParam_t rsa_chunk = NULL;
    uint8_t index;
    uint32_t binary_size;
    StrParam_t key = NULL;
    char **tmp_key_p = NULL;
    size_t converted_size = 0;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {

        if (!authenticate_data_p) {
            ATC_LOG_E("%s: authenticate_data_p is NULL", __func__);
            current_cmee = CMEE_OP_NOT_APPLICABLE;
            goto error;
        }

        char **SIMLock_keys[] =  { &authenticate_data_p->nl_key_p,
                                   &authenticate_data_p->nsl_key_p,
                                   &authenticate_data_p->spl_key_p,
                                   &authenticate_data_p->cl_key_p,
                                   &authenticate_data_p->esl_key_p
                                 };

        switch (authenticate_data_p->auth_type) {
        case EXE_COPS_AUTH_TYPE_RSA: {
            rsa_chunk = Parser_GetStrParam(parser_p, &error_flag, NULL);

            if (error_flag) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                result = AT_CME_ERROR;
                goto error;
            }

            /* size that needs to be allocated for binary data */
            binary_size = strlen(rsa_chunk);
            /* When converting form ascii hex -> bin we will fit 2 chars into one byte,
             * thus only using half the original size.
             * Add 1 to binary_size before dividing it by 2 to avoid truncation error if
             * binary_size is an odd number. */
            binary_size = (binary_size + 1) / 2;

            if (NULL == authenticate_data_p->rsa_challenge_response_p) {
                /* First call to ECPSAUTHDU */
                authenticate_data_p->rsa_challenge_response_p = malloc(binary_size);
            } else {
                /* All following calls to ECPSAUTHDU */
                authenticate_data_p->rsa_challenge_response_p = realloc(
                            authenticate_data_p->rsa_challenge_response_p, binary_size
                            + authenticate_data_p->rsa_size);

            }

            if (NULL == authenticate_data_p->rsa_challenge_response_p) {
                goto error;
            }

            atc_string_convert(ATC_CHARSET_HEX, (uint8_t *)rsa_chunk, strlen((char *)rsa_chunk),
                               ATC_CHARSET_UNDEF_BINARY,
                               (uint8_t *)(authenticate_data_p->rsa_challenge_response_p + authenticate_data_p->rsa_size),
                               &converted_size);

            authenticate_data_p->rsa_size += binary_size;
            break;
        }
        case EXE_COPS_AUTH_TYPE_SIMLOCK: {
            /* Consume first parameter since its not valid in the SIMLock key case */
            Parser_GetStrParam(parser_p, &error_flag, NULL);
            error_flag = false;
            /* Parse the following SIMLock keys */

            for (index = 0; index < (sizeof(SIMLock_keys) / sizeof(char **)); index++) {
                tmp_key_p = SIMLock_keys[index];
                key = Parser_GetStrParam(parser_p, &error_flag, NULL);

                if (error_flag) {
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                    result = AT_CME_ERROR;
                    goto error;
                }

                *tmp_key_p = strdup(key);

                if (NULL == *tmp_key_p) {
                    goto error;
                }
            }

            break;
        }
        default:
            result = AT_ERROR;
            goto error;
        }

        break;
    }
    case AT_MODE_TEST: {
        break;
    }
    default:
        current_cmee = CMEE_UNKNOWN;
        break;
    }

error:

    if (current_cmee != CMEE_OK) {
        Parser_SetCMEE_ErrorType(parser_p, current_cmee);
        return AT_CME_ERROR;
    }

    return result;
}

/*
 *===============================================================================
 *
 *   Function: AT_STAR_ECPSIMEIW_Handle
 *
 *   INPUT:   parser_p - Pointer to the current parser state.
 *            info_text - Pointer to a string buffer to put information text.
 *
 *   OUTPUT:  parser->
 *              Pos    - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There where a general error.
 *            AT_CME_ERROR - There where a GSM specific error, the error code is
 *                           returned in ASCII format in InfoText.
 *            AT_PENDING   - The command is not ready but returns while
 *                           waiting for an asynchronous response.
 *
 *   The ECPSAUTHDU command is used to enter a new IMEI number.
 *   Note that for the function to complete successfully, authentication must be granted
 *   prior to calling this function.
 *   Authentication can be granted by using the commands ECPSAUTHU and ECPSAUTHDU.
 *
 *   SET   - Enter IMEI data
 *
 *   TEST - Test command to show if the command is supported.
 *
 *
 *===============================================================================
 */
AT_Command_e AT_STAR_ECPSIMEIW_Handle(AT_ParserState_s *parser_p,
                                      AT_CommandLine_t info_text,
                                      AT_ResponseMessage_t message)
{
    bool error_flag = false; /* Flags an error. */
    AT_Command_e result = AT_OK;
    exe_cmee_error_t current_cmee = CMEE_OK;
    StrParam_t imei = NULL;
    exe_cops_imei_t exe_imei;
    exe_t *exe = NULL;
    exe_request_result_t exe_res = EXE_FAILURE;
    uint8_t index;
    char c;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        imei = Parser_GetStrParam(parser_p, &error_flag, NULL);

        if (error_flag) {
            current_cmee = CMEE_INCORRECT_PARAMETERS;
            goto error;
        }

        for (index = 0; index < strnlen(imei, EXE_COPS_IMEI_MAX_LEN); index++) {
            c = imei[index];

            /*Convert ASCII digit to integer value */
            if ('0' <= c && c <= '9') {
                exe_imei.imei[index] = c - '0';
            } else {
                current_cmee = CMEE_INCORRECT_PARAMETERS;
                goto error;
            }
        }

        /* cops client, synchronous call */
        exe = atc_get_exe_handle();
        exe_res = exe_request(exe, EXE_COPS_SET_IMEI, (void *) &exe_imei,
                              (void *) parser_p, &parser_p->request_handle);
        result = parser_handle_exe_result(exe_res, parser_p);

        if (AT_OK != result) {
            goto error;
        }
    }
    case AT_MODE_TEST: {
        break;
    }
    default:
        current_cmee = CMEE_UNKNOWN;
        break;
    }

error:

    if (current_cmee != CMEE_OK) {
        Parser_SetCMEE_ErrorType(parser_p, current_cmee);
        return AT_CME_ERROR;
    }

    return result;
}

/*
 *===============================================================================
 *
 *   Function: AT_STAR_ECPSLCW_Handle
 *
 *   INPUT:   parser_p - Pointer to the current parser state.
 *            info_text - Pointer to a string buffer to put information text.
 *
 *   OUTPUT:  parser->
 *              Pos    - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There where a general error.
 *            AT_CME_ERROR - There where a GSM specific error, the error code is
 *                           returned in ASCII format in InfoText.
 *            AT_PENDING   - The command is not ready but returns while
 *                           waiting for an asynchronous response.
 *
 *   The ECPSLCW command is used to enter a new SIMLock keys.
 *   Note that for the function to complete successfully, authentication must be granted
 *   prior to calling this function.
 *   Authentication can be granted by using the commands ECPSAUTHU and ECPSAUTHDU.
 *
 *   SET   - Enter SIMLock keys
 *
 *   TEST - Test command to show if the command is supported.
 *
 *
 *===============================================================================
 */
AT_Command_e AT_STAR_ECPSLCW_Handle(AT_ParserState_s *parser_p,
                                    AT_CommandLine_t info_text,
                                    AT_ResponseMessage_t message)
{
    bool error_flag = false; /* Flags an error. */
    AT_Command_e result = AT_OK;
    exe_cmee_error_t current_cmee = CMEE_OK;
    exe_t *exe = NULL;
    exe_request_result_t exe_res = EXE_FAILURE;
    exe_cops_simlock_keys_t exe_simlock_keys;
    StrParam_t key = NULL;
    uint8_t index;
    char **SIMLock_keys[] =  { &exe_simlock_keys.nl_key_p,
                               &exe_simlock_keys.nsl_key_p,
                               &exe_simlock_keys.spl_key_p,
                               &exe_simlock_keys.cl_key_p,
                               &exe_simlock_keys.esl_key_p
                             };

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        for (index = 0; index < (sizeof(SIMLock_keys) / sizeof(char **)); index++) {
            key = Parser_GetStrParam(parser_p, &error_flag, NULL);

            if (error_flag) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                result = AT_CME_ERROR;
                goto error;
            }

            *SIMLock_keys[index] = key;
        }

        /* cops client, synchronous call */
        exe = atc_get_exe_handle();
        exe_res = exe_request(exe, EXE_COPS_SET_SIMLOCK, (void *) &exe_simlock_keys, (void *) parser_p,
                              &parser_p->request_handle);
        result = parser_handle_exe_result(exe_res, parser_p);

        if (AT_OK != result) {
            goto error;
        }
    }
    case AT_MODE_TEST: {
        break;
    }
    default:
        current_cmee = CMEE_UNKNOWN;
        break;
    }

error:

    if (current_cmee != CMEE_OK) {
        Parser_SetCMEE_ErrorType(parser_p, current_cmee);
        return AT_CME_ERROR;
    }

    return result;
}

/*
 *===============================================================================
 *
 *   Function: AT_STAR_ECPSSIMLOCK_Handle
 *
 *   INPUT:   parser_p - Pointer to the current parser state.
 *            info_text - Pointer to a string buffer to put information text.
 *
 *   OUTPUT:  parser->
 *              Pos    - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There where a general error.
 *            AT_CME_ERROR - There where a GSM specific error, the error code is
 *                           returned in ASCII format in InfoText.
 *            AT_PENDING   - The command is not ready but returns while
 *                           waiting for an asynchronous response.
 *
 *   The ECPSSIMLOCK command is used to trigger new SIMLock data.
 *   Note that for the function to complete successfully, authentication must be granted
 *   prior to calling this function.
 *   Authentication can be granted by using the commands ECPSAUTHU and ECPSAUTHDU.
 *
 *   SET   - Trigger SIMLock data.
 *
 *   TEST - Test command to show if the command is supported.
 *
 *
 *===============================================================================
 */
AT_Command_e AT_STAR_ECPSSIMLOCK_Handle(AT_ParserState_s *parser_p,
                                        AT_CommandLine_t info_text,
                                        AT_ResponseMessage_t message)
{
    bool error_flag = false; /* Flags an error. */
    AT_Command_e result = AT_OK;
    uint8_t operation;
    exe_t *exe = NULL;
    exe_request_result_t exe_res = EXE_FAILURE;
    exe_cmee_error_t current_cmee = CMEE_OK;
    exe_cops_simlock_data_t *read_data_p = NULL;
    char *hex_str_p = NULL;
    char *hex_tmp_p = NULL;
    char *format_str_p = NULL;
    size_t total_hex_str_len;
    uint16_t max_rsp_buf_size;
    uint16_t remaining_hex_len;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {

        operation = Parser_GetIntParam(parser_p, &error_flag, NULL);

        if (error_flag) {
            Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
            result = AT_CME_ERROR;
            goto error;
        }

        switch (operation) {
        case COPS_OPERATION_TYPE_INIT: {
            free_static_cops_simlock_data();
            simlock_data_p = calloc(1, sizeof(exe_cops_simlock_data_t));
            break;
        }
        case COPS_OPERATION_TYPE_COMPLETE: {

            if (NULL == simlock_data_p) {
                ATC_LOG_E("%s: simlock_data_p is NULL", __func__);
                current_cmee = CMEE_OP_NOT_APPLICABLE;
                goto error;
            }

            /* cops client, synchronous call */
            exe = atc_get_exe_handle();
            exe_res = exe_request(exe, EXE_COPS_WRITE_SL_DATA, (void *) simlock_data_p, (void *) parser_p,
                                  &parser_p->request_handle);
            free_static_cops_simlock_data();
            result = parser_handle_exe_result(exe_res, parser_p);

            if (AT_OK != result) {
                goto error;
            }

            break;
        }
        case COPS_OPERATION_TYPE_ABORT: {
            free_static_cops_simlock_data();
            break;
        }
        default:
            goto error;
            break;
        }

        break;
    }
    case AT_MODE_READ: {
        read_data_p = calloc(1, sizeof(exe_cops_simlock_data_t));

        if (!read_data_p) {
            goto error;
        }

        /* cops client, synchronous call */
        exe = atc_get_exe_handle();
        exe_res = exe_request(exe, EXE_COPS_READ_SL_DATA, (void *) read_data_p,
                              (void *) parser_p, &parser_p->request_handle);
        result = parser_handle_exe_result(exe_res, parser_p);

        if (AT_OK != result) {
            goto error;
        }

        /* Allocate the double size for non-hex format plus 1 for null termination */
        total_hex_str_len = read_data_p->simlock_data_size * 2 + 1;
        hex_str_p = (char *) calloc(1, total_hex_str_len);

        if (!hex_str_p) {
            goto error;
        }

        atc_string_convert(ATC_CHARSET_UNDEF_BINARY, (uint8_t *)read_data_p->simlock_data, read_data_p->simlock_data_size,
                           ATC_CHARSET_HEX, (uint8_t *)hex_str_p, &total_hex_str_len);

        /* Since the maximum response buffer is limited, the resulting hex string will
           have to be split up in several prints if it is larger than the maximum buffer */
        hex_tmp_p = hex_str_p;
        /* Max buffer is total buffer - length of "*ECPSSIMLOCK: " + null termination */
        max_rsp_buf_size = RESULT_TEXT_SIZE - (14 + 1);
        remaining_hex_len = total_hex_str_len;
        /* Construct a format string to specify maximum number of characters to be printed */
        asprintf(&format_str_p, ": %%.%ds", max_rsp_buf_size);

        while (remaining_hex_len > max_rsp_buf_size) {
            sprintf((char *) info_text, format_str_p, hex_tmp_p);
            hex_tmp_p += max_rsp_buf_size;
            Parser_SendMultipleResponses(parser_p, AT_NO_OVERRIDE, info_text, false);
            remaining_hex_len -= max_rsp_buf_size;
        }

        sprintf((char *) info_text, ": %s", hex_tmp_p);
        Parser_SendMultipleResponses(parser_p, AT_NO_OVERRIDE, info_text, true);

        free(format_str_p);
        free(hex_str_p);
        hex_str_p = NULL;
        free_simlock_data(read_data_p);
        read_data_p = NULL;
        break;
    }
    case AT_MODE_TEST: {
        break;
    }
    default:
        current_cmee = CMEE_UNKNOWN;
        break;
    }

    return result;

error:
    free_simlock_data(read_data_p);
    read_data_p = NULL;

    if (current_cmee != CMEE_OK) {
        Parser_SetCMEE_ErrorType(parser_p, current_cmee);
        return AT_CME_ERROR;
    }

    return result;
}


/*
 *===============================================================================
 *
 *   Function: AT_STAR_ECPSSIMLOCKD_Handle
 *
 *   INPUT:   parser_p - Pointer to the current parser state.
 *            info_text - Pointer to a string buffer to put information text.
 *        atc_string_convert(ATC_CHARSET_UNDEF_BINARY, (uint8_t *)read_data_p->simlock_data, read_data_p->simlock_data_size,
                           ATC_CHARSET_HEX, (uint8_t *)hex_str_p, &total_hex_str_len);

 *
 *   OUTPUT:  parser->
 *              Pos    - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There where a general error.
 *            AT_CME_ERROR - There where a GSM specific error, the error code is
 *                           returned in ASCII format in InfoText.
 *            AT_PENDING   - The command is not ready but returns while
 *                           waiting for an asynchronous response.
 *
 *   The ECPSSIMLOCKD command is used to enter SIMLock data.
 *   Note that for the function to complete successfully, authentication must be granted
 *   prior to calling this function.
 *   Authentication can be granted by using the commands ECPSAUTHU and ECPSsimlock_data_chunk = Parser_GetStrParam(parser_p, &error_flag, NULL);AUTHDU.
 *
 *   SET   - Enter SIMLock data
 *
 *   TEST - Test command to show if the command is supported.
 *
 *
 *===============================================================================
 */
AT_Command_e AT_STAR_ECPSSIMLOCKD_Handle(AT_ParserState_s *parser_p,
        AT_CommandLine_t info_text,
        AT_ResponseMessage_t message)
{
    bool error_flag = false; /* Flags an error. */
    AT_Command_e result = AT_OK;
    exe_cmee_error_t current_cmee = CMEE_OK;
    StrParam_t simlock_data_chunk = NULL;
    uint8_t index;
    size_t binary_size;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {

        if (!simlock_data_p) {
            ATC_LOG_E("%s: simlock_data_p is NULL", __func__);
            current_cmee = CMEE_OP_NOT_APPLICABLE;
            goto error;
        }

        simlock_data_chunk = Parser_GetStrParam(parser_p, &error_flag, NULL);

        if (error_flag) {
            Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
            result = AT_CME_ERROR;
            goto error;
        }

        /* size that needs to be allocated for binary data */
        binary_size = strlen(simlock_data_chunk);

        /* When converting from hex -> bin we will fit 2 chars into one byte,
         * thus only using half the original size.
         * Add 1 to binary_size before dividing it by 2 to avoid truncation error if
         * binary_size is an odd number.
         */
        binary_size = (binary_size + 1) / 2;

        if (NULL == simlock_data_p->simlock_data) {
            /* First call to ECPSAUTHDU */
            simlock_data_p->simlock_data = malloc(binary_size);
        } else {
            /* All following calls to ECPSAUTHDU */
            simlock_data_p->simlock_data = realloc(simlock_data_p->simlock_data, binary_size
                                                   + simlock_data_p->simlock_data_size);

        }

        if (NULL == simlock_data_p->simlock_data) {
            goto error;
        }

        atc_string_convert(ATC_CHARSET_HEX, (uint8_t *)simlock_data_chunk, strlen((char *)simlock_data_chunk),
                           ATC_CHARSET_UNDEF_BINARY,
                           (uint8_t *)(simlock_data_p->simlock_data + simlock_data_p->simlock_data_size),
                           &binary_size);

        simlock_data_p->simlock_data_size += binary_size;
        break;
    }
    case AT_MODE_TEST: {
        break;
    }
    default:
        current_cmee = CMEE_UNKNOWN;
        break;
    }

error:

    if (current_cmee != CMEE_OK) {
        Parser_SetCMEE_ErrorType(parser_p, current_cmee);
        return AT_CME_ERROR;
    }

    return result;
}
/*
*===============================================================================
*
*   Function: AT_STAR_ECPSAUTHPE_Handle
*
*   Desc :   *ECPSAUTHPE: authenticate permanent engineering mode
*
*   INPUT:   parser_p  - Pointer to the current parser state.
*            info_text - Pointer to a string buffer to put information text.
*
*   OUTPUT:  parser_p->
*              Pos     - The current position in the command line.
*
*   RETURNS: A status code for the execution of the command is returned:
*            AT_OK        - The command was successfully handled.
*            AT_ERROR     - There where a general error.
*            AT_CME_ERROR - There where a GSM specific error, the error code is
*                           returned in ASCII format in info_text.
**
*   DO   - Authenticate permanent engineering mode and set device in authenticated state
*
*   TEST - Test command returns support for the command.
*
*
*===============================================================================
*/
AT_Command_e AT_STAR_ECPSAUTHPE_Handle(AT_ParserState_s *parser_p,
                                       AT_CommandLine_t info_text,
                                       AT_ResponseMessage_t message)
{
    AT_Command_e          result     = AT_OK;
    exe_t                 *exe       = NULL;
    exe_request_result_t  exe_res    = EXE_FAILURE;

    switch (parser_p->Mode) {
        case AT_MODE_DO:
            /* cops client, synchronous call */
            exe = atc_get_exe_handle();

            exe_res = exe_request(exe,
                                  EXE_COPS_AUTH_ENG_MODE,
                                  (void *) NULL,
                                  (void *) parser_p,
                                  &parser_p->request_handle);


            result = parser_handle_exe_result(exe_res, parser_p);

            if (AT_OK != result) {
                ATC_LOG_E("exe_request error!");
                result = AT_ERROR;
            }
            break;

        case AT_MODE_TEST:
            break;

        default:
            ATC_LOG_E("Invalid mode!");
            result = AT_ERROR;
            break;
    }

    return result;
}

/*
*===============================================================================
*
*   Function: AT_STAR_ECPSDEAUTHPE_Handle
*
*   Desc :   *ECPSDEAUTHPE: deauthenticate permanent engineering mode
*
*   INPUT:   parser_p  - Pointer to the current parser state.
*            info_text - Pointer to a string buffer to put information text.
*
*   OUTPUT:  parser_p->
*              Pos     - The current position in the command line.
*
*   RETURNS: A status code for the execution of the command is returned:
*            AT_OK        - The command was successfully handled.
*            AT_ERROR     - There where a general error.
*            AT_CME_ERROR - There where a GSM specific error, the error code is
*                           returned in ASCII format in info_text.
**
*   DO   - Deauthenticate permanent engineering mode
*
*   TEST - Test command returns support for the command.
*
*
*===============================================================================
*/
AT_Command_e AT_STAR_ECPSDEAUTHPE_Handle(AT_ParserState_s *parser_p,
                                         AT_CommandLine_t info_text,
                                         AT_ResponseMessage_t message)
{
    AT_Command_e          result     = AT_OK;
    exe_t                 *exe       = NULL;
    exe_request_result_t  exe_res    = EXE_FAILURE;

    switch (parser_p->Mode) {
        case AT_MODE_DO:
            /* cops client, synchronous call */
            exe = atc_get_exe_handle();

            exe_res = exe_request(exe,
                                  EXE_COPS_DEAUTH_ENG_MODE,
                                  (void *) NULL,
                                  (void *) parser_p,
                                  &parser_p->request_handle);


            result = parser_handle_exe_result(exe_res, parser_p);

            if (AT_OK != result) {
                ATC_LOG_E("exe_request error!");
                result = AT_ERROR;
            }
        break;

        case AT_MODE_TEST:
            break;

        default:
            ATC_LOG_E("Invalid mode!");
            result = AT_ERROR;
            break;
    }

    return result;
}

/*
*===============================================================================
*
*   Function: AT_STAR_ECPSLOCKBOOTP_Handle
*
*   Desc :   *ECPSLOCKBOOTP: lock bootpartition permanently
*
*   INPUT:   parser_p  - Pointer to the current parser state.
*            info_text - Pointer to a string buffer to put information text.
*
*   OUTPUT:  parser_p->
*              Pos     - The current position in the command line.
*
*   RETURNS: A status code for the execution of the command is returned:
*            AT_OK        - The command was successfully handled.
*            AT_ERROR     - There where a general error.
*            AT_CME_ERROR - There where a GSM specific error, the error code is
*                           returned in ASCII format in info_text.
**
*   DO   - Lock bootpartition permanently
*
*   TEST - Test command returns support for the command.
*
*
*===============================================================================
*/
AT_Command_e AT_STAR_ECPSLOCKBOOTP_Handle(AT_ParserState_s *parser_p,
                                         AT_CommandLine_t info_text,
                                         AT_ResponseMessage_t message)
{
    AT_Command_e          result     = AT_OK;
    exe_t                 *exe       = NULL;
    exe_request_result_t  exe_res    = EXE_FAILURE;

    switch (parser_p->Mode) {
        case AT_MODE_DO:
            /* cops client, synchronous call */
            exe = atc_get_exe_handle();

            exe_res = exe_request(exe,
                                  EXE_COPS_LOCK_BOOTPARTITION,
                                  (void *) NULL,
                                  (void *) parser_p,
                                  &parser_p->request_handle);


            result = parser_handle_exe_result(exe_res, parser_p);

            if (AT_OK != result) {
                ATC_LOG_E("exe_request error!");
                result = AT_ERROR;
            }
        break;

        case AT_MODE_TEST:
            break;

        default:
            ATC_LOG_E("Invalid mode!");
            result = AT_ERROR;
            break;
    }

    return result;
}

/*
*===============================================================================
*
*   Function: AT_STAR_ECPSINITARB_Handle
*
*   Desc :   *ECPSINITARB: initializes the antirollback table
*
*   INPUT:   parser_p  - Pointer to the current parser state.
*            info_text - Pointer to a string buffer to put information text.
*
*   OUTPUT:  parser_p->
*              Pos     - The current position in the command line.
*
*   RETURNS: A status code for the execution of the command is returned:
*            AT_OK        - The command was successfully handled.
*            AT_ERROR     - There where a general error.
*            AT_CME_ERROR - There where a GSM specific error, the error code is
*                           returned in ASCII format in info_text.
**
*   SET   - Initializes the antirollback table with given key
*
*   TEST - Test command returns support for the command.
*
*
*===============================================================================
*/
AT_Command_e AT_STAR_ECPSINITARB_Handle(AT_ParserState_s *parser_p,
                                        AT_CommandLine_t info_text,
                                        AT_ResponseMessage_t message)
{
    AT_Command_e          result     = AT_OK;
    exe_t                 *exe       = NULL;
    exe_request_result_t  exe_res    = EXE_FAILURE;
    bool                  error_flag = false;
    uint16_t              product_id = 0;

    switch (parser_p->Mode) {
        case AT_MODE_SET:
            /* cops client, synchronous call */
            exe = atc_get_exe_handle();

            product_id = (uint16_t)Parser_GetLongIntParam(parser_p,
                                                          &error_flag,
                                                          NULL,
                                                          65535);
            /* We'll get an error due to the limit if we have an overflow */
            if (error_flag) {
                ATC_LOG_E("Invalid parameter");
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                result = AT_CME_ERROR;
                break;
            }
            exe_res = exe_request(exe,
                                  EXE_COPS_INIT_ANTIROLLBACK,
                                  (void *) &product_id,
                                  (void *) parser_p,
                                  &parser_p->request_handle);

            result = parser_handle_exe_result(exe_res, parser_p);

            if (AT_OK != result) {
                ATC_LOG_E("exe_request error!");
                result = AT_ERROR;
            }
        break;

        case AT_MODE_TEST:
            break;

        default:
            ATC_LOG_E("Invalid mode");
            result = AT_ERROR;
            break;
    }

    return result;
}

/*
 *===============================================================================
 *
 *   Function: AT_STAR_ECPSDTW_Handle
 *
 *   INPUT:   parser_p - Pointer to the current parser state.
 *            info_text - Pointer to a string buffer to put information text.
 *
 *   OUTPUT:  parser->
 *              Pos    - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There where a general error.
 *            AT_CME_ERROR - There where a GSM specific error, the error code is
 *                           returned in ASCII format in InfoText.
 *            AT_PENDING   - The command is not ready but returns while
 *                           waiting for an asynchronous response.
 *
 *   The ECPSDTW command is used to enter D&T key data.
 *
 *   SET   - Takes an input-version number as first parameter,
 *           which decides the format of the following parameters.
 *           Version id     Parameters
 *           1              Flags,
 *                          key0[, key1...key7] where each key is 64 chars [0..f]
 *
 *           Example: AT*ECPSDTW=1, 255,"1234567890abcdeabcde..","feab123456.."
 *
 *   TEST - Test command to show if the command is supported.
 *
 *
 *===============================================================================
 */
AT_Command_e AT_STAR_ECPSDTW_Handle(AT_ParserState_s *parser_p,
        AT_CommandLine_t info_text,
        AT_ResponseMessage_t message)
{
    /* Number of allowed keys to be entered */
    #define MAX_NBR_HASHKEYS 6
    /* Every uint8_t holds 8 bit, so 32 uint8_t can hold a 256 bit key.
     * But in the input strings, every character represents 4-bit, so
     * it takes 2 characters to represent one uint8_t of data.
     */
    #define HASHKEY_LENGTH_TYPE_0 (256/8)

    AT_Command_e result = AT_OK;
    exe_cmee_error_t current_cmee = CMEE_OK;
    exe_request_result_t exe_res = EXE_FAILURE;
    exe_t *exe = NULL;
    bool error_flag = false;
    exe_cops_d_and_t_data_t dnt_data;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        uint32_t version_type = 0;
        version_type = (uint32_t)Parser_GetLongIntParam(parser_p,
                                                        &error_flag,
                                                        NULL,
                                                        0xffffffff);

        if (error_flag) {
            current_cmee = CMEE_INCORRECT_PARAMETERS;
            ATC_LOG_E("Illegal first parameter (version_id)!");
            goto error;
        }
        /* Switch structure for future expansion of input-types */
        switch ((exe_cops_d_and_t_input_types_t)version_type) {
        case COPS_D_AND_T_INPUT_TYPE_1: {
            uint32_t flags = 0;
            int nbr_hashes = 0;

            uint8_t hashkeys[MAX_NBR_HASHKEYS][HASHKEY_LENGTH_TYPE_0];
            /* The flags is 32-bit, thus with a maximum of 0xffffffff */
            flags = (uint32_t)Parser_GetLongIntParam(parser_p,
                                                     &error_flag,
                                                     NULL,
                                                     0xffffffff);

            if (error_flag) {
                current_cmee = CMEE_INCORRECT_PARAMETERS;
                ATC_LOG_E("Error reading flags!");
                goto error;
            }

            for (nbr_hashes = 0; nbr_hashes < MAX_NBR_HASHKEYS; nbr_hashes++) {
                StrParam_t key;
                key = Parser_GetStrParam(parser_p, &error_flag, NULL);
                if (error_flag || (key == NULL))
                    break;
                else if (strlen(key) != HASHKEY_LENGTH_TYPE_0 * 2) {
                    ATC_LOG_E("Key[%d] is of wrong length", nbr_hashes);
                    current_cmee = CMEE_INCORRECT_PARAMETERS;
                    goto error;
                }
                else {
                    size_t final_length = 0;
                    /* We use two characters to represent one byte */
                    atc_string_convert(ATC_CHARSET_HEX,
                                       (uint8_t *)key,
                                       HASHKEY_LENGTH_TYPE_0 * 2,
                                       ATC_CHARSET_UNDEF_BINARY,
                                       &hashkeys[nbr_hashes][0],
                                       &final_length);

                    if (final_length != HASHKEY_LENGTH_TYPE_0) {
                        ATC_LOG_E("Key[%d] converted to wrong length",
                                  nbr_hashes);
                        current_cmee = CMEE_INCORRECT_PARAMETERS;
                        goto error;
                    }
                }
            }
            /* Check so we have no trailing parameters */
            if (!parser_p->NoMoreParams) {
                ATC_LOG_E("Too many parameters.");
                current_cmee = CMEE_INCORRECT_PARAMETERS;
                goto error;
            }
            dnt_data.version = version_type;
            dnt_data.flags = flags;
            dnt_data.hashlist_len = HASHKEY_LENGTH_TYPE_0 *
                                    sizeof(uint8_t) *
                                    nbr_hashes;
            if (dnt_data.hashlist_len != 0)
                dnt_data.hashlist = &hashkeys[0][0];
            else
                dnt_data.hashlist = NULL;
            break;
        }
        default:
            ATC_LOG_E("Invalid input type!");
            current_cmee = CMEE_UNKNOWN;
            goto error;
            break;
        }

        /* cops client, synchronous call */
        exe = atc_get_exe_handle();
        /* Send pointer to struct holding all info */
        exe_res = exe_request(exe,
                              EXE_COPS_WRITE_D_AND_T,
                              (void *) &dnt_data,
                              (void *) parser_p,
                              &parser_p->request_handle);
        result = parser_handle_exe_result(exe_res, parser_p);

        if (AT_OK != result) {
            ATC_LOG_E("exe_request error!");
            result = AT_ERROR;
        }
        break;
    }
    case AT_MODE_TEST: {
        break;
    }
    default:
        ATC_LOG_E("Invalid mode!");
        current_cmee = CMEE_UNKNOWN;
        break;
    }

error:
    if (current_cmee != CMEE_OK) {
        Parser_SetCMEE_ErrorType(parser_p, current_cmee);
        return AT_CME_ERROR;
    }
    return result;
}
