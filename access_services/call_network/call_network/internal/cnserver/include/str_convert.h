/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __str_convert_h__
#define __str_convert_h__ (1)

#include <stdbool.h>
#include <string.h>

typedef enum {
    CHARSET_UNKNOWN             = -1,
    CHARSET_GSM_DEFAULT_7_BIT   = 0,    /* Aligned with 3GPP 24.008 Network Name IEI coding scheme */
    CHARSET_UCS_2               = 1,    /* Aligned with 3GPP 24.008 Network Name IEI coding scheme */
    CHARSET_US_ASCII,
    CHARSET_GSM_DEFAULT_8_BIT,
    CHARSET_UTF_8,
    CHARSET_UTF_16LE,                   /* UTF-16 Little Endian */
    CHARSET_UTF_16BE,                   /* UTF-16 Big Endian */
    CHARSET_UTF_32LE,                   /* UTF-32 Little Endian */
    CHARSET_UTF_32BE,                   /* UTF-32 Big Endian */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    CHARSET_UTF_16 = CHARSET_UTF_16LE,
    CHARSET_UTF_32 = CHARSET_UTF_32LE,
#else
    CHARSET_UTF_16 = CHARSET_UTF_16BE,
    CHARSET_UTF_32 = CHARSET_UTF_32BE,
#endif
    CHARSET_MAX,
} string_encoding_t;

bool str_convert_init(void);
int str_convert_string(string_encoding_t from_charset, size_t from_length, uint8_t *from_string_p, size_t spare_bits,
                       string_encoding_t to_charset, size_t *to_length_p, uint8_t *to_string_p);
void str_convert_shutdown(void);

#endif /* __str_convert_h__ */
