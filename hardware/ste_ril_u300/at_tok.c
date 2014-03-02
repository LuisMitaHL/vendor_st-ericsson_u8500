/* //device/system/reference-ril/at_tok.c
**
** Copyright 2006, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#include "at_tok.h"
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>


/**
 * Starts tokenizing an AT response string.
 * Returns -1 if this is not a valid response string, 0 on success.
 * Updates *p_cur with current position.
 */
int at_tok_start(char **p_cur)
{
    if (*p_cur == NULL)
        return -1;

    /* Skip prefix,
       consume "^[^:]:". */

    *p_cur = strchr(*p_cur, ':');

    if (*p_cur == NULL)
        return -1;

    (*p_cur)++;

    return 0;
}

static void skipWhiteSpace(char **p_cur)
{
    if (*p_cur == NULL)
        return;

    while (**p_cur != '\0' && isspace(**p_cur))
        (*p_cur)++;
}

static void skipNextComma(char **p_cur)
{
    if (*p_cur == NULL)
        return;

    while (**p_cur != '\0' && **p_cur != ',')
        (*p_cur)++;

    if (**p_cur == ',')
        (*p_cur)++;
}

/* Returns true if src_p has reached the end of a string.
 * Handles both quoted and unquoted strings. */
static bool end_of_string(char *src_p, bool quoted, bool escaped)
{
    if (quoted) {
        /* String parameter ends with quote. */
        return (*src_p == '"' && !escaped);
    } else {
        /* String parameter ends with comma or NULL termination. */
        return (*src_p == ',' || *src_p == '\0');
    }
}

/*
 * This function parses out the next parameter from the input string.
 * Parameters are separated by commas ',' and can be integers or
 * text strings. Text strings can be encapsulated by quotes '"' or not.
 *
 * Quotes within text strings can be handled if they are escaped, i.e.
 * preceded by a backslash '\', and a backslash may also be escaped.
 *
 * Returns a pointer to the parsed out parameter.
 * Input data *p_cur points to next parameter at return.
 *
 * Examples:
 *   Input data: *p_cur = '"Invalid option \"9\"",15'
 *   will result in,
 *     return = 'Invalid option "9"'
 *     *p_cur = '15'
 *   Input data: *p_cur = '"\some data\\","some more data"'
 *   will result in,
 *     return = '\some data\'
 *     *p_cur = '"some more data"'
 */
static char *nextTok(char **p_cur)
{
    char *ret = NULL;
    char *temp = NULL;
    char *p_cur_end = NULL;
    bool quoted = false;
    bool escaped = false;
    bool found_escaped_character = false;

    skipWhiteSpace(p_cur);

    if (*p_cur == NULL)
        return NULL;

    p_cur_end = *p_cur + strlen(*p_cur);

    if (**p_cur == '"') {
        /* The string is encapsulated with quotes, remove these. */
        quoted = true;
        (*p_cur)++;
    }

    temp = *p_cur;

    /* Go through the string to find any escaped quotes or backslashes: '\"' or '\\'. */
    while (temp < p_cur_end && !end_of_string(temp, quoted, escaped)) {
        if (escaped && ('"' == *temp || RIL_ESCAPE_CHARACTER == *temp)) {
            /* Remove the escape character: '\'. */
            found_escaped_character = true;
            strcpy(temp - 1, temp); /* temp is here never the first byte. */
            escaped = false;
        } else {
            /* If escape character then toggle value for escaped. */
            escaped = RIL_ESCAPE_CHARACTER == *temp ? !escaped : false;
            temp++;
        }
    }

    /* Return the parsed string, ret, and the rest of the input data, p_cur. */
    if (found_escaped_character) {
        /* Terminate the modified string and set return pointers. */
        *temp = '\0';
        ret = *p_cur;
        *p_cur = temp + 1;
        skipNextComma(p_cur);
    } else if (quoted) {
        ret = strsep(p_cur, "\"");
        skipNextComma(p_cur);
    } else {
        ret = strsep(p_cur, ",");
    }

    return ret;
}


/**
 * Parses the next integer in the AT response line and places it in *p_out.
 * Returns 0 on success and -1 on fail.
 * Updates *p_cur.
 * "base" is the same as the base param in strtol.
 */
static int at_tok_nextint_base(char **p_cur, int *p_out, int base, int uns)
{
    char *ret;

    if (*p_cur == NULL)
        return -1;

    ret = nextTok(p_cur);

    if (ret == NULL)
        return -1;
    else {
        long l;
        char *end;

        if (uns)
            l = strtoul(ret, &end, base);
        else
            l = strtol(ret, &end, base);

        *p_out = (int) l;

        if (end == ret)
            return -1;
    }

    return 0;
}

/**
 * Parses the next base 10 integer in the AT response line
 * and places it in *p_out.
 * Returns 0 on success and -1 on fail.
 * Updates *p_cur.
 */
int at_tok_nextint(char **p_cur, int *p_out)
{
    return at_tok_nextint_base(p_cur, p_out, 10, 0);
}

/**
 * Parses the next base 16 integer in the AT response line
 * and places it in *p_out.
 * Returns 0 on success and -1 on fail.
 * Updates *p_cur.
 */
int at_tok_nexthexint(char **p_cur, int *p_out)
{
    return at_tok_nextint_base(p_cur, p_out, 16, 1);
}

int at_tok_nextbool(char **p_cur, char *p_out)
{
    int ret;
    int result;

    ret = at_tok_nextint(p_cur, &result);

    if (ret < 0)
        return -1;

    /* Booleans should be 0 or 1. */
    if (!(result == 0 || result == 1))
        return -1;

    if (p_out != NULL)
        *p_out = (char) result;

    return ret;
}

int at_tok_nextstr(char **p_cur, char **p_out)
{
    if (*p_cur == NULL)
        return -1;

    *p_out = nextTok(p_cur);

    return 0;
}

/** Returns 1 on "has more tokens" and 0 if not. */
int at_tok_hasmore(char **p_cur)
{
    return !(*p_cur == NULL || **p_cur == '\0');
}

/** *p_out returns count of given character (needle) in given string (p_in). */
int at_tok_charcounter(char *p_in, char needle, int *p_out)
{
    char *p_cur = p_in;
    int num_found = 0;

    if (p_in == NULL)
        return -1;

    while (*p_cur != '\0') {
        if (*p_cur == needle) {
            num_found++;
        }

        p_cur++;
    }

    *p_out = num_found;
    return 0;
}
