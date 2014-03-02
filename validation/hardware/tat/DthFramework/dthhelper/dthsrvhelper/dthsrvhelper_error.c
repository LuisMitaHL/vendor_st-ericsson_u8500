/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file
* \brief   9P interfaces for DTH server helper
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include <string.h>
#include <stdio.h>
#include "dthsrvhelper_error.h"


#define ERRNO_INFO_ENTRY(code) { code, #code, "" } /* info is resolved later with strerror() */
#define TATERR_INFO_ENTRY(code, info) { TAT_ERROR_##code, #code, info }

const struct st_error_info TAT_ERROR_STRINGS[] =
{
    TATERR_INFO_ENTRY(OFF,              "success"),

    /* linux errors (order does not matter)
     * Add only inputs that are expected to be formerly identified.
     * These codes should be the one producing specific behavior on client side.
     */
    ERRNO_INFO_ENTRY(EBADF),   /* may be useful later */
    ERRNO_INFO_ENTRY(EACCES),  /* DTH file handles this exception */
    ERRNO_INFO_ENTRY(EBADRQC), /* very frequent in many services */

    /* TAT generic errors (remove TAT_ERROR prefix for code names)
     * IMPORTANT: elements in the table are to be in ascending error code order
     * or the algorithm used by get_error_info() will fail!
     */
    TATERR_INFO_ENTRY(ON,               "failure"),
    TATERR_INFO_ENTRY(VERSION,          "fail to get version"),
    TATERR_INFO_ENTRY(REVISION,         "fail to get revision"),
    TATERR_INFO_ENTRY(CHECKSUM,         "checksum error"),
    TATERR_INFO_ENTRY(PROCESS_NULL,     "null process"),
    TATERR_INFO_ENTRY(NUM_PROCESS,      "wrong pid"),
    TATERR_INFO_ENTRY(BAD_OPERATION,    "invalid operation"),
    TATERR_INFO_ENTRY(CASE,             "error case"),
    TATERR_INFO_ENTRY(RESOURCE,         "resource not available"),
    TATERR_INFO_ENTRY(PARAMETERS,       "parameters are invalid"),
    TATERR_INFO_ENTRY(OUT_OF_MEM,       "not enough memory"),
    TATERR_INFO_ENTRY(BAD_COMMAND,      "command unrecognized"),
    TATERR_INFO_ENTRY(BAD_REQ,          "invalid request"),
    TATERR_INFO_ENTRY(DEADLOCK,         "deadlock detected"),
    TATERR_INFO_ENTRY(INIT_LENGHT,      "init length failure"),
    TATERR_INFO_ENTRY(INIT,             "init failure"),
    TATERR_INFO_ENTRY(NOT_AVAILABLE,    "operation not permitted"),
    TATERR_INFO_ENTRY(BAD_DEV_ID,       "invalid device id"),
    TATERR_INFO_ENTRY(BAD_MANUFACT_ID,  "invalid manufacturer id"),
    TATERR_INFO_ENTRY(IT,               "interrupt"),
    TATERR_INFO_ENTRY(CHK_DIFFERED,     "conditionnal checked failed"),
    TATERR_INFO_ENTRY(MEM_ALLOC,        "memory allocation"),
    TATERR_INFO_ENTRY(MEM_FREE,         "memory free"),
    TATERR_INFO_ENTRY(ENTRY_POINT,      "missing entry point"),
    TATERR_INFO_ENTRY(FILE_OPEN,        "fail to open file"),
    TATERR_INFO_ENTRY(FILE_WRITE,       "cannot write to file"),
    TATERR_INFO_ENTRY(FILE_READ,        "cannot read file"),
    TATERR_INFO_ENTRY(NULL_POINTER,     "null pointer"),
    TATERR_INFO_ENTRY(NOT_FOUND,        "not found"),
    TATERR_INFO_ENTRY(ALIGN,            "not aligned"),
    TATERR_INFO_ENTRY(TYPE,             "type unknown"),
    TATERR_INFO_ENTRY(TOO_MANY,         "too many items"),
    TATERR_INFO_ENTRY(SEND,             "fail to send data"),
    TATERR_INFO_ENTRY(RECEIVE,          "fail to receive data"),

    /* TAT feature errors (remove TAT_ERROR prefix for code names) */
    TATERR_INFO_ENTRY(NOT_MATCHING_MSG, "not the response expected"),
    TATERR_INFO_ENTRY(ISI_LINK,         "modem not ready"),
};

void get_error_info(struct st_error_info* pp_err)
{
    /*fprintf(stdout, "find error info %d\n", pp_err->code);*/

    static const int vl_nb_items = sizeof(TAT_ERROR_STRINGS) /
        sizeof(struct st_error_info);

    if ( pp_err->code < TAT_ERROR_MIN ) {
        /* very few elements in table and error codes are suspected to change:
         * best is to use a sequential search */
        int i;
        for ( i = 0; (i < vl_nb_items) && (TAT_ERROR_STRINGS[i].code < TAT_ERROR_MIN); i++ ) {
            if ( TAT_ERROR_STRINGS[i].code == pp_err->code ) {
                /*fprintf(stdout, "found error at position %d\n", i);*/
                memcpy(pp_err, TAT_ERROR_STRINGS + i, sizeof(struct st_error_info));
                break;
            }
        }
    } else {
        /* use dichotomic search algorithm to reduce time consumption
         * Works only if the element in the table are ordered by ascendant error
         * codes, which is a requirement */
        int a = 0, b = vl_nb_items-1;

        while (a <= b) {
            int m = (a + b) / 2;
            /*fprintf(stdout, "check error at position %d (%d, %s, %s)\n", m,
                TAT_ERROR_STRINGS[m].code, TAT_ERROR_STRINGS[m].name, TAT_ERROR_STRINGS[m].info);*/

            if ( TAT_ERROR_STRINGS[m].code == pp_err->code ) {
                /*fprintf(stdout, "found error at position %d\n", m);*/
                memcpy(pp_err, TAT_ERROR_STRINGS + m, sizeof(struct st_error_info));
                break;
            } else if ( TAT_ERROR_STRINGS[m].code > pp_err->code ) {
                b = m-1;
            } else {
                a = m+1;
            }
        }
    }
}

