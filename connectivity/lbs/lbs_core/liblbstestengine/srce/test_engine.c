/*
 * Positioning Manager
 *
 * test_engine.c
 *
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <strings.h>

#include <agpsosa.h>
#include <test_engine_process.h>
#include <test_engine_int.h>
//#include <pos_api.h>

/*
 * Client linked list struct
 */
typedef struct TSTENG_client_s {
    int fd;
    t_OsaThreadHandle thread;
    TSTENG_handle_t handle;
    TSTENG_command_item_t *cmd_p;
    struct TSTENG_client_s *next;
} TSTENG_client_t;

extern TSTENG_table_t *TSTENG_registered_tables;

static TSTENG_command_item_t *TSTENG_find_in_table(const TSTENG_command_line_t * cmdline, TSTENG_command_item_t * table);

static int TSTENG_parse_command_line(const char *raw_command_p, TSTENG_command_line_t * cmd_line_p);
static int TSTENG_util_string_compare(const char *string1_p, const char *string2_p);
static TSTENG_command_item_t *TSTENG_find_submodule_table(const TSTENG_command_line_t * cmdline);

/*
 * Tables
 */
void TSTENG_register_table(char *name, TSTENG_command_item_t * table)
{
    TSTENG_table_t *new=NULL;
    TSTENG_table_t *nxt=NULL;
    TSTENG_table_t *pl_table_t=NULL;

    INF("adding %s\n", name);

    new = OSA_Malloc(sizeof(*pl_table_t));
    if (new == NULL) {
    CRI("unable to allocate memory\n");
    return;
    }

    bzero(new, sizeof(*pl_table_t));

    new->name = name;
    new->table = table;
    new->next = NULL;

    nxt = TSTENG_registered_tables;
    if (nxt == NULL) {
    TSTENG_registered_tables = new;
    } else {
    while (nxt->next != NULL)
        nxt = nxt->next;

    nxt->next = new;
    }
}

/*
 * TSTENG_find_in_table
 *
 * Find a command in the function table
 */
static TSTENG_command_item_t *TSTENG_find_in_table(const TSTENG_command_line_t * cmdline, TSTENG_command_item_t * table)
{
    TSTENG_command_item_t *nxt;
    const char *str = cmdline->command_buf + cmdline->arg_offsets[0];

    nxt = table;
    while (nxt->command_p != NULL) {
    if (TSTENG_util_string_compare(str, nxt->command_p)) {
        return nxt;
    }
    nxt++;
    }

    return NULL;
}

/*
 * TSTENG_find_submodule_table
 *
 * Search for submodule table
 */
static TSTENG_command_item_t *TSTENG_find_submodule_table(const TSTENG_command_line_t * cmdline)
{
    TSTENG_table_t *table;
    const char *str = cmdline->command_buf;

    table = TSTENG_registered_tables;
    while (table != NULL) {
    if (TSTENG_util_string_compare(str, table->name))
        return table->table;

    table = table->next;
    }

    return NULL;
}

/*
 * TSTENG_execute_command
 *
 *  Execute the test command
 */
int TSTENG_execute_command(TSTENG_handle_t handle, const char *str_p, TSTENG_command_item_t ** clientcmd_pp)
{
    int i, j;
    int result = TSTENG_RESULT_FAIL;
    int arg_offsets[TSTENG_MAX_ARGS];
    TSTENG_command_item_t *table_p=NULL;
    TSTENG_command_item_t *cmd_p=NULL;
    TSTENG_command_line_t *cmd_line_p=NULL;

    // allocate command line structure
    if ((cmd_line_p = OSA_Malloc(sizeof(*cmd_line_p))) == NULL) {
    CRI("unable to allocate memory\n");
    return result;
    }
    // Parse the string ...into command parameters
    if (TSTENG_parse_command_line(str_p, cmd_line_p) == FALSE) {
    ERR("failed to parse command line\n");
    OSA_Free(cmd_line_p);
    return result;
    }

    table_p = TSTENG_find_submodule_table(cmd_line_p);
    if (table_p == NULL) {
    ERR("failed to find command\n");
    OSA_Free(cmd_line_p);
    return result;
    }

    cmd_p = TSTENG_find_in_table(cmd_line_p, table_p);
    if (cmd_p == NULL) {
    ERR("command does not exist\n");
    OSA_Free(cmd_line_p);
    return result;
    }

    if (clientcmd_pp != NULL) {
    *clientcmd_pp = cmd_p;
    }

    i = j = 0;

    for (i = 1; i < cmd_line_p->num_args; arg_offsets[j++] = cmd_line_p->arg_offsets[i++]);

    result = cmd_p->function_p(handle, cmd_line_p->command_buf, arg_offsets, cmd_line_p->num_args - 1);

    OSA_Free(cmd_line_p);
    return result;
}


/*
 * TSTENG_parse_command_line
 *
 * Description: Creates a TSTENG_command_line_t from the raw command line
 *
 */
static int TSTENG_parse_command_line(const char *str_p, TSTENG_command_line_t * cmd_line_p)
{
    int found_space = FALSE;
    int found_backslash = FALSE;
    int index = 0;

    DBG("parse command line: %s\n", str_p);

    // Error command line structure is not allocated
    if ((str_p == NULL) || (cmd_line_p == NULL)) {
    return FALSE;
    }
    // Initialise the cmdLine_p struct
    cmd_line_p->num_args = 0;
    memset(cmd_line_p->arg_offsets, '\0', TSTENG_MAX_ARGS);

    // Skip leading SPACEs
    while (*str_p != '\0' && *str_p == TSTENG_SPACE_CHAR)
    str_p++;

    // Begin command line parsing
    // Parse until end of input OR until no more data fits in the CmdLine_t
    // Also make sure no argument is longer than TSTENG_MAXLEN_ENTRYNAME
    while (*str_p != 0 && index < (TSTENG_MAXLEN_CMDBUF - 1)) {
    found_space = (*str_p == TSTENG_SPACE_CHAR);
    found_backslash = (*str_p == TSTENG_BACKSLASH_CHAR);

    if (!found_space && !found_backslash) {
        DBG(" add to command buffer: %x at index %d\n", *str_p, index);
        cmd_line_p->command_buf[index++] = *str_p++;
        continue;        // ## CONTINUE ## while
    }

    if (found_space) {
        found_space = FALSE;    // Clear the flag

        // Skip duplicate SPACEs
        while (*str_p != 0 && *str_p == TSTENG_SPACE_CHAR)
        str_p++;

        DBG(" add Null to command buffer at index %d\n", index);
        cmd_line_p->command_buf[index++] = 0;    // Args are '0' separated

        // Found new argument or end of string
        if (*str_p != 0) {
        // New argument found
        cmd_line_p->arg_offsets[cmd_line_p->num_args] = index;
        cmd_line_p->num_args++;
        if (cmd_line_p->num_args >= TSTENG_MAX_ARGS) {
            break;    // Break the while loop
        }
        }
        continue;        // ## CONTINUE ## while
    }

    if (found_backslash) {
        // Move to next character and store that
        // unless the next character is the new-line character(s)
        if (*(str_p + 1) != 0) {
        str_p++;
        cmd_line_p->command_buf[index] = *str_p;
        } else {
        // Store the backslash character
        cmd_line_p->command_buf[index] = *str_p;
        }
        index++;
        found_backslash = FALSE;
    }
    str_p++;
    }

    cmd_line_p->command_buf[index] = 0;    // Zero terminate the last argument

    // Something went wrong, for instance perhaps the command line
    // was full of SPACE chars!!
    if (cmd_line_p->command_buf[0] == 0) {
    DBG("parsing error!\n");
    return FALSE;
    }
    // Debug the output
    {
    int i;
    DBG("args found %d\n", cmd_line_p->num_args);
    DBG("command is %s\n", cmd_line_p->command_buf);
    for (i = 0; i < cmd_line_p->num_args; i++) {
        DBG("\tPosition %d: %s\n", cmd_line_p->arg_offsets[i], (cmd_line_p->command_buf + cmd_line_p->arg_offsets[i]));
    }
    }
    return TRUE;
}

/*
 * TSTENG_util_string_compare
 *
 * Compares 2 terminated strings, returns TRUE if identical
 *              The comparison is case-insensitive [A-Z], [a-z]
 *
 */
static int TSTENG_util_string_compare(const char *string1_p, const char *string2_p)
{
    int return_value = TRUE;

    DBG("comapring string %s with string %s\n", string1_p, string2_p);

    if ((string1_p == NULL) || (string2_p == NULL)) {
    if ((string1_p == NULL) && (string2_p == NULL)) {
        // Both strings are NULL thus equal
        DBG("input strings are NULL\n");
        return_value = TRUE;
    }
    ERR("input string %s is NULL\n", (string1_p == NULL ? "string1_p" : "string2_p"));
    return_value = FALSE;
    }

    if (return_value) {
    int str_length1 = strlen(string1_p);
    int str_length2 = strlen(string2_p);

    // Strings equaly long ?
    if (str_length1 != str_length2) {
        DBG("not same length   len1 %d len2 %d\n", str_length1, str_length2);
        return_value = FALSE;
    } else {
        // Compare every character in the string
        // If the character is a letter, compare in UPPER-case
        while (str_length1--) {
        char val1 = *string1_p;
        char val2 = *string2_p;

        // Check for small character range
        // Do not convert to lower-case if non-letter
        val1 = toupper(val1);
        val2 = toupper(val2);

        if (val1 != val2) {
            // String mismatch
            return_value = FALSE;
            break;
        }
        string1_p++;
        string2_p++;
        }
    }
    }
    return return_value;
}
