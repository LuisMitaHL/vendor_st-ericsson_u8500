/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "cn_ss_command_handling.h"
#include "cn_log.h"
#include "cn_macros.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* datatype definitions */

/***********************************
 *  SS string pattern definitions. *
 ***********************************
 *
 * See 3GPP TS 22.030, section 6.5.2 for more information.
 *
 * NOTE:
 * =====
 * Some SS patterns are accepted by commercial phones that are not
 * according to 3GPP (TS 22.030), e.g. "*SC*#" is used instead of
 * the specified "*SC#" pattern for activation.
 *
 * This is what's specified in 3GPP:
 * "Where a particular service request does not require any SI, "*SI" is
 * not entered, e.g. Activation becomes *SC#SEND."
 *
 * This can be seen as an extension of 3GPP. If the functionality is needed
 * the flag ENABLE_NON_3GPP_SS_PATTERNS can be enabled. The flag only enables
 * additional patterns for decoding strings given by the user. For SS encoding
 * the 3GPP pattern is used when the flag is enabled.
 *
 * Note that the order of the elements in the table affects the encoder.
 */
#define ENABLE_NON_3GPP_SS_PATTERNS 0
static cn_ss_pattern_entry_t ss_pattern_table[] = { 
    { CN_SS_PROCEDURE_TYPE_ACTIVATION,    "*SC#"     },
    { CN_SS_PROCEDURE_TYPE_DEACTIVATION,  "#SC*SI#"  },
    { CN_SS_PROCEDURE_TYPE_DEACTIVATION,  "#SC#"     },
    { CN_SS_PROCEDURE_TYPE_INTERROGATION, "*#SC*SI#" },
    { CN_SS_PROCEDURE_TYPE_INTERROGATION, "*#SC#"    },
    { CN_SS_PROCEDURE_TYPE_REGISTRATION,  "*SC*SI#"  },
    { CN_SS_PROCEDURE_TYPE_REGISTRATION,  "**SC*SI#" },
    { CN_SS_PROCEDURE_TYPE_ERASURE,       "##SC*SI#" },
    { CN_SS_PROCEDURE_TYPE_ERASURE,       "##SC#"    },
#ifdef ENABLE_NON_3GPP_SS_PATTERNS
    { CN_SS_PROCEDURE_TYPE_ACTIVATION,    "*SC*#"    },
    { CN_SS_PROCEDURE_TYPE_DEACTIVATION,  "#SC*#"    },
    { CN_SS_PROCEDURE_TYPE_INTERROGATION, "*#SC*#"   },
    { CN_SS_PROCEDURE_TYPE_ERASURE,       "##SC*#"   },
#endif
    { CN_SS_PROCEDURE_TYPE_UNKNOWN,       NULL       }
};

/* static functions */
static cn_bool_t determine_ss_pattern(char *ss_string_p, char *pattern_p, cn_uint8_t pattern_buffer_size);
static cn_ss_procedure_type_t determine_procedure_type(char *ss_string_p);
static cn_ss_type_t get_ss_type(char *ss_string_p);
static void get_supplementary_info(char *ss_string_p, char *supplementary_info_p, cn_si_type_t si_type);


cn_ss_string_compounds_t *decode_ss_string(char *ss_string_p)
{
    cn_ss_string_compounds_t *command_p = NULL;

    if (!ss_string_p) {
        CN_LOG_E("ss_string_p is NULL!");
        goto error;
    }

    command_p = calloc(1, sizeof(cn_ss_string_compounds_t));

    if (!command_p) {
        CN_LOG_E("calloc failed for command_p!");
        goto error;
    }

    command_p->ss_type        = CN_SS_TYPE_UNKNOWN;
    command_p->procedure_type = CN_SS_PROCEDURE_TYPE_UNKNOWN;

    CN_LOG_D("decoding SS string \"%s\"", ss_string_p);

    command_p->procedure_type = determine_procedure_type(ss_string_p);
    if(CN_SS_PROCEDURE_TYPE_UNKNOWN == command_p->procedure_type) {
        CN_LOG_E("unknown ss procedure type!");
        goto exit;
    }

    command_p->ss_type = get_ss_type(ss_string_p);
    if (CN_SS_TYPE_UNKNOWN == command_p->ss_type) {
        CN_LOG_E("unknown ss type! (service code)");
        goto exit;
    }

    get_supplementary_info(ss_string_p, command_p->supplementary_info_a, CN_SI_TYPE_SIA);
    get_supplementary_info(ss_string_p, command_p->supplementary_info_b, CN_SI_TYPE_SIB);
    get_supplementary_info(ss_string_p, command_p->supplementary_info_c, CN_SI_TYPE_SIC);
    /* Take care of the activation procedure case with SI (excluding SIA).
     * This is handled as a special case instead of having a complex pattern
     * table with all possible SIA, SIB, SIC variations.
     */
    if (CN_SS_PROCEDURE_TYPE_REGISTRATION == command_p->procedure_type &&
        0 == command_p->supplementary_info_a[0]) {
        command_p->procedure_type = CN_SS_PROCEDURE_TYPE_ACTIVATION;
    }

    CN_LOG_D("procedure_type = %d", command_p->procedure_type);

exit:
    return command_p;

error:
    return NULL;
}

#define STRING_BUFFER_SIZE 4
char *encode_ss_string(cn_ss_string_compounds_t *command_p)
{
    char *ss_string_p  = NULL;
    char *ss_pattern_p = NULL;
    char string[STRING_BUFFER_SIZE] = {0, 0, 0, 0};
    cn_uint32_t i = 0;


    if (!command_p) {
        CN_LOG_E("command_p is NULL!");
        goto error;
    }

    if (CN_SS_TYPE_UNKNOWN == command_p->ss_type) {
        CN_LOG_E("unknown ss_type!");
        goto error;
    }

    ss_string_p = calloc(1, CN_MAX_STRING_SIZE);
    if (!ss_string_p) {
        CN_LOG_E("calloc failed for ss_string_p!");
        goto error;
    }

    /* get ss pattern string
     * NOTE: "*SC*SI#" or "**SC*SI#" is selected depending on the order
     *       in ss_pattern_table. One of them will never be used for the
     *       encoder (they are equivalent so it is not an issue).
     */
    for(i=0; NULL != ss_pattern_table[i].pattern_p; i++) {
        if (command_p->procedure_type == ss_pattern_table[i].type) {
            ss_pattern_p = ss_pattern_table[i].pattern_p;
            break;
        }
    }
    if (!ss_pattern_p) {
        CN_LOG_E("unknown procedure type!");
        goto error;
    }

    /* Take care of the activation procedure case with SI (excluding SIA).
     * This is handled as a special case instead of having a complex pattern
     * table with all possible SIA, SIB, SIC variations.
     */
    if (CN_SS_PROCEDURE_TYPE_ACTIVATION == command_p->procedure_type &&
          0 == command_p->supplementary_info_a[0] &&
          (0 != command_p->supplementary_info_b[0] ||
           0 != command_p->supplementary_info_c[0])) {
        ss_pattern_p = "*SC*SI#";
    }

    for (i=0; i<strlen(ss_pattern_p); i++) {
       switch(ss_pattern_p[i]) {
           /* Append SS string with * or # */
           case '*':
           case '#': // "*SC*SI#" "*SC#"
               /* Skip '*' char before SI since that is handled in the default: case */
               if (i > 1 && '*' == ss_pattern_p[i] && 0 == memcmp(&ss_pattern_p[i-2], "SC", 2)) {
                   break;
               }

               string[0] = ss_pattern_p[i];
               string[1] = 0;
               (void)strncat(ss_string_p, string, CN_MAX_STRING_SIZE - strlen(ss_string_p));
               break;
           /* Append SS string with SC or SI in its final string form */
           default:
               /* SC component */
               if (0 == memcmp(&ss_pattern_p[i], "SC", 2)) {
                 if (002 == command_p->ss_type || 003 == command_p->ss_type) {
                     snprintf(string, STRING_BUFFER_SIZE, "%03d", command_p->ss_type);
                 } else {
                     snprintf(string, STRING_BUFFER_SIZE, "%d", command_p->ss_type);
                 }

                 (void)strncat(ss_string_p, string, CN_MAX_STRING_SIZE - strlen(ss_string_p));
               } 

               if (0 == memcmp(&ss_pattern_p[i], "SI", 2)) {
                 /* SIA component */
                 if (('\0' != command_p->supplementary_info_a[0]) || ('\0' != command_p->supplementary_info_b[0]) ||
                     ('\0' != command_p->supplementary_info_c[0])) {
                     (void)strncat(ss_string_p, "*", CN_MAX_STRING_SIZE - strlen(ss_string_p));
                 }

                 if ('\0' != command_p->supplementary_info_a[0]) {
                     (void)strncat(ss_string_p, command_p->supplementary_info_a, CN_MAX_STRING_SIZE - strlen(ss_string_p));
                 }

                 /* SIB component */
                 if (('\0' != command_p->supplementary_info_b[0]) || ('\0' != command_p->supplementary_info_c[0])) {
                     (void)strncat(ss_string_p, "*", CN_MAX_STRING_SIZE - strlen(ss_string_p));
                 }
                 if ('\0' != command_p->supplementary_info_b[0]) {
                     (void)strncat(ss_string_p, command_p->supplementary_info_b, CN_MAX_STRING_SIZE - strlen(ss_string_p));
                 }

                 /* SIC component */
                 if ('\0' != command_p->supplementary_info_c[0]) {
                     (void)strncat(ss_string_p, "*", CN_MAX_STRING_SIZE - strlen(ss_string_p));
                 }
                 if ('\0' != command_p->supplementary_info_c[0]) {
                     (void)strncat(ss_string_p, command_p->supplementary_info_c, CN_MAX_STRING_SIZE - strlen(ss_string_p));
                 }
               }
               break;
       } /* end of switch statement */
    } /* end of for loop */

    CN_LOG_D("encoded SS string \"%s\"", ss_string_p);

    return ss_string_p;

error:
    if (ss_string_p) {
        free(ss_string_p);
    }
    return NULL;
}


#define NR_OF_SS_IDS 2 /* SC and SI */
static cn_bool_t determine_ss_pattern(char *ss_string_p, char *pattern_p, cn_uint8_t pattern_buffer_size)
{
    char *ss_id_string[] = { "SC", "SI" };
    cn_uint32_t ss_id = 0;
    cn_uint32_t length = 0;
    cn_uint32_t i = 0;


    if (!ss_string_p) {
        CN_LOG_E("ss_string_p is NULL!");
        goto error;
    }

    if (!pattern_p) {
        CN_LOG_E("pattern_p is NULL!");
        goto error;
    }

    if (pattern_buffer_size < 10) {
        CN_LOG_E("pattern_buffer_size < 10");
        goto error;
    }

    memset(pattern_p, 0, pattern_buffer_size);

    /* Create pattern string by traversing through each character in the SS string */
    for (i = 0; i < strlen(ss_string_p); i++) {
        switch (ss_string_p[i]) {
        case '*':
        case '#':
            /* Do not add SIA, SIB and SIC separator character as this is not the level of
             * detail needed for the procedure type string matching.
             */
            if ((i > 0 && 1 == ss_id && '*' == ss_string_p[i] && '*' == ss_string_p[i-1]) ||
                    (ss_id > 1 && '*' == ss_string_p[i])) {
                continue;
            }

            strncat(pattern_p, &ss_string_p[i], 1);
            break;
        default: /* SC or SI, add SS ID string */
            length = strlen(pattern_p);

            if (length > 0) {
                /* Add SS ID string only when the first SC/SI character has been reached */
                if ('*' == pattern_p[length-1] || '#' == pattern_p[length-1]) {
                    if (ss_id < NR_OF_SS_IDS) {
                        strncat(pattern_p, ss_id_string[ss_id], 2);
                        ss_id++;
                    }
                }
            }

            break;
        } /* end of switch statement */
    } /* end of for loop */

    return TRUE;

error:
    return FALSE;
}


static cn_ss_procedure_type_t determine_procedure_type(char *ss_string_p)
{
    cn_ss_procedure_type_t procedure_type = CN_SS_PROCEDURE_TYPE_UNKNOWN;
    char pattern[50];
    cn_uint32_t i = 0;


    determine_ss_pattern(ss_string_p, pattern, 50);

    /* Match pattern against entries in table */
    while (ss_pattern_table[i].pattern_p != NULL) {

        /* Go to next table entry if the pattern is larger than the current entry */
        if (strlen(pattern) > strlen(ss_pattern_table[i].pattern_p)) {
            goto next_iteration;
        }

        if (memcmp(pattern, ss_pattern_table[i].pattern_p, strlen(pattern) + 1) == 0) {
            procedure_type = ss_pattern_table[i].type;
        }

next_iteration:
        i++;
    }

    return procedure_type;
}


static cn_ss_type_t get_ss_type(char *ss_string_p)
{
    cn_ss_type_t ss_type = CN_SS_TYPE_UNKNOWN;
    cn_uint32_t i = 0;


    /* Create pattern string by traversing through each character in the SS string */
    for(i=0; i<strlen(ss_string_p); i++) {
        switch (ss_string_p[i]) {
            case '*':
            case '#':
                break;
            default: /* SC or SI, extract SC */
                if (i > 0 && ('*' == ss_string_p[i-1] || '#' == ss_string_p[i-1])) {
                    ss_type = atoi(&ss_string_p[i]);
                    if(0 == ss_type) {
                        ss_type = CN_SS_TYPE_UNKNOWN;
                    }
                    goto exit;
                }
                break;
        } /* end of switch statement */
    } /* end of for loop */

exit:

    CN_LOG_D("ss_type: %d", ss_type);

    return ss_type;
}


static void get_supplementary_info(char *ss_string_p, char *supplementary_info_p, cn_si_type_t si_type)
{
    cn_uint32_t ss_id = 0;
    cn_uint32_t current_ss_id = 0;
    cn_uint32_t i = 0;


    if (!ss_string_p) {
        CN_LOG_E("ss_string_p is NULL!");
        goto error;
    }

    if (!supplementary_info_p) {
        CN_LOG_E("supplementary_info_a_p is NULL!");
        goto error;
    }

    switch (si_type) {
    case CN_SI_TYPE_SIA:
            current_ss_id = 1;
        break;
    case CN_SI_TYPE_SIB:
            current_ss_id = 2;
        break;
    case CN_SI_TYPE_SIC:
            current_ss_id = 3;
        break;
    default:
        CN_LOG_E("unknown si type!");
        goto error;
    }

    /* Create pattern string by traversing through each character in the SS string */
    for(i=0; i<strlen(ss_string_p); i++) {
        switch (ss_string_p[i]) {
            case '*':
            case '#':
                /* Do not add SIA, SIB and SIC separator character as this is not the level of
                 * detail needed for the procedure type string matching.
                 */
                if ( ss_id < 1 ) {
                    /* Before first SI we can skip all *,# after that we need to look more at * */
                    break;
                }
                /* fall trough */
            default: /* SC or SI, add SS ID string */
                if ( i > 0 && ('*' == ss_string_p[i-1] || '#' == ss_string_p[i-1])) {
                    if (ss_id == current_ss_id) {
                        char *end_pos = strchr(&ss_string_p[i],'*');
                        if(NULL == end_pos) {
                            /* No star search for hash */
                            end_pos = strchr(&ss_string_p[i],'#');
                        }
                        if(NULL == end_pos) {
                            /* No hash either bad string */
                            goto error;
                        }
                        strncpy(supplementary_info_p, &ss_string_p[i],end_pos-(&ss_string_p[i]));
                    }
                    ss_id++;
                }
                break;
        } /* end of switch statement */
    } /* end of for loop */

    return;

error:
    return;
}

