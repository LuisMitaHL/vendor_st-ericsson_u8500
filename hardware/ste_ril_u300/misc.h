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
#ifndef _U300_RIL_MISC_H
#define _U300_RIL_MISC_H 1

#include <linux/ipv6.h>
#include <linux/ipv6_route.h>
#include <stdbool.h>

#define PROPERTY_SET_MAX_MS_WAIT            750
#define PROPERTY_SET_CHECK_INTERVAL_MS      50

struct tlv {
    unsigned    tag;
    const char *data;
    const char *end;
};

typedef struct
{
    char** strings;
    char** dcs_strings;
    int allocated;
    int dcs_allocated;
    int used;
    int dcs_used;
} StringSet;

int property_set_verified(const char *key, const char *value);

/** Returns 1 if line starts with prefix, 0 if it does not. */
int strStartsWith(const char *line, const char *prefix);

/* Returns number of character c found in line, searching at most length characters */
int strCountChar(const char c, const char *line, size_t length);

/* Find first tag in unquoted part of document */
char *findFirstElementTag(const char *document,
                          const char *elementTag,
                          const char quoteChar);

char *getFirstElementValue(const char *document,
                           const char *elementBeginTag,
                           const char *elementEndTag,
                           const char quoteChar,
                           char **remainingDocument);

char char2nib(char c);

int stringToBinary(const char *string,
                   size_t len,
                   unsigned char *binary);

int binaryToString(const unsigned char *binary,
                   size_t len,
                   char *string);


int parseTlv(const char *stream,
             const char *end,
             struct tlv *tlv);

void stringSetInit(StringSet* set);
bool stringSetContains(const StringSet* set, const char* string);
bool stringSetAdd(StringSet* set, const char* mid_string, const char* dcs_string);
void stringSetFree(StringSet* set);
char* stringSetToCommaSeparatedString(const StringSet* set);
char* dcsStringSetToCommaSeparatedString(const StringSet* set);
bool stringSetRemove(StringSet* set, const char* string);


#define TLV_DATA(tlv, pos) (((unsigned)char2nib(tlv.data[(pos) * 2 + 0]) << 4) | \
                            ((unsigned)char2nib(tlv.data[(pos) * 2 + 1]) << 0))

#define NUM_ELEMS(x) (sizeof(x) / sizeof(x[0]))

#endif
