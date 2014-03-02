/* ST-Ericsson U300 RIL
**
** Copyright (C) ST-Ericsson AB 2008-2010
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
**
** Based on reference-ril by The Android Open Source Project.
**
** Modified for ST-Ericsson U300 modems.
** Author: Christian Bejram <christian.bejram@stericsson.com>
*/

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/route.h>
#include "u300-ril.h"

#include <arpa/inet.h>

#include <cutils/properties.h>

#include "misc.h"

#define LOG_TAG "RILV"
#include <utils/Log.h>

#define INITIAL_STRING_SET_SIZE 10

/** Returns 1 if line starts with prefix, 0 if it does not. */
int strStartsWith(const char *line, const char *prefix)
{
    for (; *line != '\0' && *prefix != '\0'; line++, prefix++)
        if (*line != *prefix) {
            return 0;
        }

    return *prefix == '\0';
}

/* Returns number of character c found in line, searching at most length characters */
int strCountChar(const char c, const char *line, size_t length)
{
    int count = 0;

    if (c != '\0') {
        for (; line != NULL && *line != '\0' && length > 0; line++, length--) {
            if (*line == c) {
                count++;
            }
        }
    }

    return count;
}

/* Find first tag in unquoted part of document */
char *findFirstElementTag(const char *document,
                          const char *elementTag,
                          const char quoteChar)
{
    char *start = NULL;

    for (start = (char *)document; start != NULL;) {
        start = strstr(start, elementTag);

        if (start != NULL) {
            /* Check for tag inside quoted (opaque) portion of document */
            if (strCountChar(quoteChar, document, start - document) % 2 == 0) {
                /* Not inside quoted part, tag found, exit */
                break;
            }

            start = strchr(start, quoteChar);
        }
    }

    return start;
}

/**
 * Very simple function that extract and returns what's between ElementBeginTag
 * and ElementEndTag. Handles nested tags, and tags within quoted portions of
 * document.
 *
 * Optional argument 'remainder' returns a pointer to the remainder
 * of the document to be "scanned". This can be used if subsequent
 * scanning/searching is desired.
 *
 * Optional argument 'quoteChar' that when set to a non-zero value like '"'
 * will prevent searching for tags inside 'quoteChar' enclosed parts of the
 * document.
 *
 * This function is used to extract the parameters from the XML result
 * returned by U3xx during a PDP Context setup, and used to parse the
 * tuples of operators returned from AT+COPS.
 *
 *  const char* document        - Document to be scanned
 *  const char* elementBeginTag - Begin tag to scan for, return whats
 *                                between begin/end tags
 *  const char* elementEndTag   - End tag to scan for, return whats
 *                                between begin/end tags
 *  const char quoteChar        - Quotation character that surrounds opaque
 *                                parts of the document where tags are not
 *                                searched for. Set to '\0' if not relevant.
 *  char** remainder            - Returns the a pointer to the remainder
 *                                of 'document'. This parameter is optional
 *
 *  return char* containing whats between begin/end tags, allocated on the
 *               heap, need to free this.
 *               return NULL if nothing is found.
 */
char *getFirstElementValue(const char *document,
                           const char *elementBeginTag,
                           const char *elementEndTag,
                           const char quoteChar,
                           char **remainder)
{
    char *value = NULL;
    char *begin = NULL;
    char *end = NULL;
    char *nested_begin = NULL;

    if (document != NULL && elementBeginTag != NULL && elementEndTag != NULL
            && strcmp(elementBeginTag, elementEndTag) != 0) {
        /* Locate beginning tag in document */
        begin = findFirstElementTag(document, elementBeginTag, quoteChar);

        /* Locate end tag in document */
        for (end = nested_begin = begin; end != NULL; end++) {
            end = findFirstElementTag(end, elementEndTag, quoteChar);

            if (end != NULL) {
                /* Check for nested begin tag */
                nested_begin = findFirstElementTag(nested_begin + 1, elementBeginTag, quoteChar);

                if (nested_begin != NULL && nested_begin < end) {
                    /* Found nested begin tag, loop */
                    continue;
                }

                /* Success! String delimited by begin and end tag found */
                /* Make a copy of the delimited string w/o delimiters */
                int n = strlen(elementBeginTag);
                int m = end - (begin + n);
                value = (char *) malloc((m + 1) * sizeof(char));

                if (value != NULL) {
                    strncpy(value, (begin + n), m);
                    value[m] = (char) 0;
                }

                /* Optional, return a pointer to the remainder of the document,
                   to be used when document contains many tags with same name. */
                if (remainder != NULL) {
                    *remainder = end + strlen(elementEndTag);
                }
            }

            break;
        }
    }

    return value;
}

char char2nib(char c)
{
    if (c >= 0x30 && c <= 0x39) {
        return c - 0x30;
    }

    if (c >= 0x41 && c <= 0x46) {
        return c - 0x41 + 0xA;
    }

    if (c >= 0x61 && c <= 0x66) {
        return c - 0x61 + 0xA;
    }

    return 0;
}

int stringToBinary(/*in*/ const char *string,
                          /*in*/ size_t len,
                          /*out*/ unsigned char *binary)
{
    int pos;
    const char *it;
    const char *end = &string[len];

    if (end < string) {
        return -EINVAL;
    }

    if (len & 1) {
        return -EINVAL;
    }

    for (pos = 0, it = string; it != end; ++pos, it += 2) {
        binary[pos] = char2nib(it[0]) << 4 | char2nib(it[1]);
    }

    return 0;
}

int binaryToString(/*in*/ const unsigned char *binary,
                          /*in*/ size_t len,
                          /*out*/ char *string)
{
    int pos;
    const unsigned char *it;
    const unsigned char *end = &binary[len];
    static const char nibbles[] = {
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
    };

    if (end < binary) {
        return -EINVAL;
    }

    for (pos = 0, it = binary; it != end; ++it, pos += 2) {
        string[pos + 0] = nibbles[*it >> 4];
        string[pos + 1] = nibbles[*it & 0x0f];
    }

    string[pos] = 0;
    return 0;
}

int parseTlv(/*in*/ const char *stream,
                    /*in*/ const char *end,
                    /*out*/ struct tlv *tlv)
{
#define TLV_STREAM_GET(stream, end, p)  \
    do {                                \
        if (stream + 1 >= end)          \
            goto underflow;             \
        p = ((unsigned)char2nib(stream[0]) << 4)  \
          | ((unsigned)char2nib(stream[1]) << 0); \
        stream += 2;                    \
    } while (0)

    size_t size;

    TLV_STREAM_GET(stream, end, tlv->tag);
    TLV_STREAM_GET(stream, end, size);

    if (stream + size * 2 > end) {
        goto underflow;
    }

    tlv->data = &stream[0];
    tlv->end  = &stream[size * 2];
    return 0;

underflow:
    return -EINVAL;
#undef TLV_STREAM_GET
}

/**
 * property_set_verified()
 *
 * Sets the property using Android set_property then reads the value until it
 * has been verified that the value is set. (set_property is asynchronous as
 * defined by Android.)
 *
 * Note: even if function fails on timeout it is still possible that the value
 * is set at a later time. If the value is to be cleared this has to be done
 * manually.
 *
 * Returns:
 *   0 on success
 *  -1 on property_set fail
 *  -2 on timeout checking the value
 */
int property_set_verified(const char *key, const char *value)
{
    int ret = -2;
    int numChecks = 0;
    int valuelen = 0;
    static const int maxchecks =
        PROPERTY_SET_MAX_MS_WAIT / PROPERTY_SET_CHECK_INTERVAL_MS;
    static const int msWait = 1000 * PROPERTY_SET_CHECK_INTERVAL_MS;

    /* Set property */
    if (key == NULL || value == NULL) {
        ret = -1;
        goto exit;
    }

    ret = property_set(key, value);

    if (ret < 0) {
        ret = -1;
        goto exit;
    }

    valuelen = strlen(value);

    /* Loop and verify the property is set in database */
    for (numChecks = 0; numChecks < maxchecks; numChecks++) {
        char checkvalue[PROPERTY_VALUE_MAX + 1];
        int getRet = property_get(key, checkvalue, NULL);

        if (getRet == valuelen) {
            if (strcmp(value, checkvalue) == 0) {
                ret = 0;
                break;
            }
        }

        ALOGI("%s() sleeping %d ms to check property have been finally set",
             __func__, PROPERTY_SET_CHECK_INTERVAL_MS);
        usleep(msWait);
    }

exit:
    return ret;
}

void stringSetInit(StringSet *set)
{
    ALOGD("CBS CONFIG: Initing String Set");
    set->strings = malloc(INITIAL_STRING_SET_SIZE * sizeof(char *));
    set->dcs_strings = malloc(INITIAL_STRING_SET_SIZE * sizeof(char *));
    set->used = 0;
    set->dcs_used = 0;
    set->allocated = INITIAL_STRING_SET_SIZE;
    set->dcs_allocated = INITIAL_STRING_SET_SIZE;
}

/**
 * Returns true if and only if the string is included in the set.
 *
 * The set parameter must be a non-null pointer to a set. An uninitialized set
 * is treated as an empty set.
 */
bool stringSetContains(const StringSet *set, const char *string)
{
    int i;
    assert(set);

    if (set->strings == NULL) {
        return false;
    }

    for (i = 0; i < set->used; i++) {
        ALOGD("STRING %d: %s", i, set->strings[i]);

        if (strcmp(set->strings[i], string) == 0) {
            return true;
        }
    }

    return false;
}

/**
 * Add the string to the set, if it wasn't in the set already.
 *
 * This function will implicitly initialize the set if necessary. The caller
 * then becomes responsible for ensuring that stringSetFree is called.
 *
 * Empty strings are not allowed in the set, and false will be returned if this
 * function is called with an empty string.
 *
 * The function returns true if the string was added or already was present
 * in the set, and false if it was not added and was not present in the set
 * from before.
 */
bool stringSetAdd(StringSet *set, const char *mid_string, const char *dcs_string)
{
    if (set->strings == NULL) {
        stringSetInit(set);
    }

    if ((strlen(mid_string) == 0) || strlen(dcs_string) == 0) {
        return false;
    }

    if (stringSetContains(set, mid_string)) {
        return true;
    }

    if (set->allocated == set->used) {
        int new_allocated = set->allocated * 2;
        char **new_strings = realloc(set->strings,
                                     new_allocated * sizeof(char *));
        assert(new_strings);
        set->strings = new_strings;
        set->allocated = new_allocated;
    }

    set->strings[set->used++] = strdup(mid_string);

    if (set->dcs_allocated == set->dcs_used) {
        int new_dcs_allocated = set->dcs_allocated * 2;
        char **new_dcs_strings = realloc(set->dcs_strings,
                                         new_dcs_allocated * sizeof(char *));
        assert(new_dcs_strings);
        set->dcs_strings = new_dcs_strings;
        set->dcs_allocated = new_dcs_allocated;
    }

    set->dcs_strings[set->dcs_used++] = strdup(dcs_string);
    ALOGD("SET ADDED & USED %d", set->used);
    return true;
}

/** Free the string set and all the copies of strings it contains.
 *
 * If the set is null or not initialized, then nothing is done.
 */
void stringSetFree(StringSet *set)
{
    int i;

    if (set == NULL) {
        return;
    }

    if (set->strings == NULL) {
        return;
    }

    for (i = 0; i < set->used; i++) {
        free(set->strings[i]);
    }

    for (i = 0; i < set->dcs_used; i++) {
        free(set->dcs_strings[i]);
    }

    free(set->strings);
    free(set->dcs_strings);
    set->strings = NULL;
    set->dcs_strings = NULL;
    set->allocated = 0;
    set->used = 0;
    set->dcs_allocated = 0;
    set->dcs_used = 0;
}

/**
 * Return a concatenation of all the MID strings in the set, separated by commas.
 *
 * The set parameter must be a non-null pointer to a set. If the set is
 * uninititialized or empty, then an empty string is returned.
 *
 * The caller is responsible to call free() on the return value.
 */
char *stringSetToCommaSeparatedString(const StringSet *set)
{
    int len = 0;
    int i;
    char *out;
    char *t;

    assert(set);

    for (i = 0; i < set->used && set->strings != NULL; i++) {
        len += strlen(set->strings[i]) + 1;
    }

    if (len == 0) {
        return strdup("");
    }

    t = out = malloc(len);

    for (i = 0; i < set->used; i++) {
        ALOGD("STRING %d: %s", i, set->strings[i]);

        if (i != 0) {
            t += sprintf(t, ",");
        }

        t += sprintf(t, "%s", set->strings[i]);
    }

    return out;
}

/**
 * Return a concatenation of all the DCS strings in the set, separated by commas.
 *
 * The set parameter must be a non-null pointer to a set. If the set is
 * uninititialized or empty, then an empty string is returned.
 *
 * The caller is responsible to call free() on the return value.
 */
char *dcsStringSetToCommaSeparatedString(const StringSet *set)
{
    int len = 0;
    int i;
    char *out;
    char *t;

    assert(set);

    for (i = 0; i < set->dcs_used && set->dcs_strings != NULL; i++) {
        len += strlen(set->dcs_strings[i]) + 1;
    }

    if (len == 0) {
        return strdup("");
    }

    t = out = malloc(len);

    for (i = 0; i < set->dcs_used; i++) {
        ALOGD("STRING %d: %s", i, set->dcs_strings[i]);

        if (i != 0) {
            t += sprintf(t, ",");
        }

        t += sprintf(t, "%s", set->dcs_strings[i]);
    }

    return out;
}



/**
 * Remove a string from the set.
 *
 * The return value is true if the string was found and removed, and false if
 * the string was not found.
 *
 * An uninitialized set is treated as an empty set. That is, false will be
 * returned. The stringSetRemove function will not initialize the set.
 *
 * The memory used internally for storing the string is freed, and the memory
 * used for storing the internal pointer array is shrunk if necessary.
 */
bool stringSetRemove(StringSet *set, const char *string)
{
    int i;
    bool found = false;

    assert(set);

    if (set->strings == NULL) {
        return false;
    }

    for (i = 0; i < set->used && !found; i++) {
        ALOGD("STRING %d: %s", i, set->strings[i]);

        if (strcmp(set->strings[i], string) == 0) {
            /*
             * The set is unordered, so we can delete an item in the middle by
             * simply moving the last element there.
             */
            free(set->strings[i]);
            set->strings[i] = set->strings[set->used - 1];
            set->used--;
            found = true;

            if (set->used <= (set->allocated / 2)
                    && set->used >= (INITIAL_STRING_SET_SIZE * 2)) {
                size_t new_allocated = set->allocated / 2;
                char **new_strings = realloc(set->strings,
                                             new_allocated * sizeof(char *));

                if (new_strings) {
                    set->strings = new_strings;
                    set->allocated = new_allocated;
                }
            }

            free(set->dcs_strings[i]);
            set->dcs_strings[i] = set->dcs_strings[set->dcs_used - 1];
            set->dcs_used--;

            if (set->dcs_used <= (set->dcs_allocated / 2)
                    && set->dcs_used >= (INITIAL_STRING_SET_SIZE * 2)) {
                size_t new_dcs_allocated = set->dcs_allocated / 2;
                char **new_dcs_strings = realloc(set->dcs_strings,
                                                 new_dcs_allocated * sizeof(char *));

                if (new_dcs_strings) {
                    set->dcs_strings = new_dcs_strings;
                    set->dcs_allocated = new_dcs_allocated;
                }
            }
        }
    }

    return found;
}
