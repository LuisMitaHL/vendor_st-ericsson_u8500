/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <alloca.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef USE_ICONV
#include <iconv.h>
#elif USE_UCNV
#include <unicode/ucnv.h>
#endif

#include "cn_log.h"
#include "cn_macros.h"
#include "str_convert.h"

/* Macros */
#define MAX_CONVERTERS      10
#ifdef USE_ICONV
#define CONVERTER_HANDLE iconv_t
#elif USE_UCNV
#define CONVERTER_HANDLE UConverter *
#endif

/* Constants */
static const struct {
    const char *string;
} charset_names[] = {
    {"GSM_DEFAULT"},/* CHARSET_GSM_DEFAULT_7_BIT */
    {"UCS-2"},      /* CHARSET_UCS_2 (subset of UTF-16BE) */
    {"US-ASCII"},   /* CHARSET_US_ASCII */
    {"GSM0338"},    /* CHARSET_GSM_DEFAULT_8_BIT */
    {"UTF-8"},      /* CHARSET_UTF_8 */
    {"UTF-16LE"},   /* CHARSET_UTF_16LE */
    {"UTF-16BE"},   /* CHARSET_UTF_16BE (superset of UCS-2) */
    {"UTF-32LE"},   /* CHARSET_UTF_32LE */
    {"UTF-32BE"},   /* CHARSET_UTF_32BE */
};

/* Local data */
static struct {
#ifdef USE_ICONV
    string_encoding_t from_charset;
    string_encoding_t to_charset;
#elif USE_UCNV
    string_encoding_t charset;
#endif
    CONVERTER_HANDLE handle;
} charset_converters[MAX_CONVERTERS];


/* Local function prototypes */
static int str_convert_get_converter(string_encoding_t from_charset, CONVERTER_HANDLE *from_conv_p,
                                     string_encoding_t to_charset, CONVERTER_HANDLE *to_conv_p);
static int str_convert_alloc_converter(string_encoding_t from_charset, CONVERTER_HANDLE *from_conv_p,
                                       string_encoding_t to_charset, CONVERTER_HANDLE *to_conv_p);
static int str_convert_septet_to_octet(size_t from_length, uint8_t *from_string_p, size_t spare_bits,
                                       size_t to_length, uint8_t *to_string_p);
static int str_convert_octet_to_septet(size_t from_length, uint8_t *from_string_p,
                                       size_t to_length, uint8_t *to_string_p);
#ifdef USE_UCNV
static bool str_convert_is_utf_16(string_encoding_t charset);
#endif


/* Initialize converter array */
bool str_convert_init()
{
    int i;


    for (i = 0; i < MAX_CONVERTERS; i++) {
        charset_converters[i].handle = NULL;
    }

    return true;
}


/* ICU4C always converts from or to UTF-16. Therefore, converting to/from any
 * other source/target requires a two step conversion process.
 *
 * ICONV can convert directly from one character set to another in a one-step
 * operation.
 *
 * Initially this module will only support to or from UTF-8.
 */

int str_convert_string(string_encoding_t from_charset, size_t from_length, uint8_t *from_string_p, size_t spare_bits,
                       string_encoding_t to_charset, size_t *to_length_p, uint8_t *to_string_p)
{
    CONVERTER_HANDLE from_conv;
    CONVERTER_HANDLE to_conv;
    bool from_7_bit = false;
    bool to_7_bit = false;
    size_t length;
    uint8_t *string_p;
    int converted = 0;
    int result;


    CN_LOG_D("from_charset=%d, from_length=%d, to_charset=%d, *to_length_p=%d",
             from_charset, from_length, to_charset, *to_length_p);

    if (from_charset == to_charset) {
        /* No conversion, adjust/copy length and strings, and return */
        if (*to_length_p > from_length) {
            *to_length_p = from_length;
        }

        memcpy(to_string_p, from_string_p, *to_length_p);
        to_string_p[*to_length_p] = '\0';
        return from_length;
    }

    /* GSM Default alphabet (7-bit packed) is not supported by the converters.
     * These strings must first be massaged from an array of septets to octets... */
    if (CHARSET_GSM_DEFAULT_7_BIT == from_charset) {
        from_charset = CHARSET_GSM_DEFAULT_8_BIT;
        from_7_bit = true;
    }

    /* or from an array of octets to septets. */
    if (CHARSET_GSM_DEFAULT_7_BIT == to_charset) {
        to_charset = CHARSET_GSM_DEFAULT_8_BIT;
        to_7_bit = true;
    }

    /* (Al)Locate converter(s) */
    result = str_convert_get_converter(from_charset, &from_conv, to_charset, &to_conv);

    if (result < 0) {
        CN_LOG_E("Unable to convert from character set %d to %d!", from_charset, to_charset);
        return result;
    }

    /* Run zeroth conversion */
    if (from_7_bit) {
        length = from_length + from_length / 7;
        string_p = (uint8_t *)alloca(length + 1);
        length = str_convert_septet_to_octet(from_length, from_string_p, spare_bits, length, string_p);
    } else {
        length = from_length;
        string_p = from_string_p;
    }

    /* Run first conversion */
#ifdef USE_ICONV
    int errnum;
    char *inbuf_p = string_p;
    size_t inbytesleft = (size_t)length;
    char *outbuf_p;
    char *outstr_p;
    size_t outbytesleft, outstr_length;
    size_t res;

    /* Allocate memory to hold result string and call ICONV to perform the conversion.
     * Assume worst case, 1 input character becomes 4 output characters. */
    outstr_length = outbytesleft = (length + 1) * sizeof(uint32_t);
    outstr_p = outbuf_p = (char *)alloca(outbytesleft);

    res = iconv(from_conv,
                &inbuf_p, &inbytesleft,
                &outbuf_p, &outbytesleft);

    /* Check for errors */
    if (res == (size_t) - 1) {
        errnum = errno;
        CN_LOG_E("iconv error %d (%s) !", errnum, strerror(errnum));
        return -1;
    }

    /* Find out how many bytes iconv actualy used for the converted string */
    outstr_length -= outbytesleft;

#elif USE_UCNV
    UChar *ustr_p;
    int32_t ustr_length;
    UErrorCode errcode = U_ZERO_ERROR;
    int32_t res;

    if (str_convert_is_utf_16(from_charset)) {
        /* From string is already UTF-16 encoded. Skip this conversion step */
        ustr_length = length;
        ustr_p = (UChar *)string_p;
    } else {
        /* Allocate memory to hold result string and call ICU to perform the conversion */
        ustr_length = length + 1;
        ustr_p = (UChar *)alloca((size_t)(ustr_length * sizeof(UChar)));

        res = ucnv_toUChars(from_conv,
                            ustr_p, ustr_length,
                            (const char *)string_p, length,
                            &errcode);

        if (res < 0) {
            CN_LOG_E("ucnv_toUChars res %d!", res);
            return -1;
        }

        /* Check for errors, ignore warnings */
        if (U_ZERO_ERROR != errcode && errcode > U_ERROR_WARNING_LIMIT) {
            CN_LOG_E("ucnv_toUChars error code %d!", errcode);
            return -1;
        }

#ifdef CN_DEBUG_ENABLED
        {
            char datastr[100] = { '\0' };
            char str[10];
            int i;

            for (i = 0; i < 30 && i < res; i++) {
                sprintf(str, "%02X ", *(ustr_p + i));
                strcat(datastr, str);
            }

            CN_LOG_D("ucnv_toUChars(conv=%p, charset=%d, ustr=\"%s\") = %d",
                     (void *)from_conv,
                     from_charset,
                     datastr,
                     res);
        }
#endif /* CN_DEBUG_ENABLED */

        ustr_length = res;
    }

#endif /* USE_UCNV */

    /* Run second conversion, unless it is the same converter as we ran before */
    if (to_conv == from_conv) {
        /* One step conversion, copy result to output parameters and return */
#ifdef USE_ICONV
        /* ICONV uses a one step conversion */

        if (*to_length_p > outstr_length) {
            *to_length_p = outstr_length;
        }

        memcpy(to_string_p, outstr_p, *to_length_p);
        to_string_p[*to_length_p] = '\0';
        converted = outstr_length;

#elif USE_UCNV

        if (*to_length_p > (size_t)ustr_length) {
            *to_length_p = (size_t)ustr_length;
        }

        memcpy(to_string_p, ustr_p, *to_length_p);
        to_string_p[*to_length_p] = '\0';
        converted = ustr_length;
#endif
    } else {
#ifdef USE_UCNV
        res = ucnv_fromUChars(to_conv,
                              (char *)to_string_p, (int32_t) * to_length_p,
                              ustr_p, ustr_length,
                              &errcode);

        /* Check for errors, ignore warnings */
        if (U_ZERO_ERROR != errcode && errcode > U_ERROR_WARNING_LIMIT) {
            CN_LOG_E("ucnv_fromUChars error code %d!", errcode);
            return -1;
        }

        if (*to_length_p > (size_t)res) {
            *to_length_p = (size_t)res;
        }

#ifdef CN_DEBUG_ENABLED
        {
            char datastr[100] = { '\0' };
            char str[10];
            int i;

            for (i = 0; i < 30 && i < res; i++) {
                sprintf(str, "%02X ", *(to_string_p + i));
                strcat(datastr, str);
            }

            CN_LOG_D("ucnv_fromUChars(conv=%p, charset=%d, str=\"%s\") = %d",
                     (void *)to_conv,
                     to_charset,
                     datastr,
                     res);
        }
#endif /* CN_DEBUG_ENABLED */

        converted = res;
#endif
    }

    /* Run final conversion, convert in-place */
    if (to_7_bit) {
        converted = str_convert_octet_to_septet(converted, to_string_p, converted, to_string_p);
    }

    return converted;
}


/* Release all allocated converters */
void str_convert_shutdown()
{
    int i;


    for (i = 0; i < MAX_CONVERTERS; i++) {
        if (NULL != charset_converters[i].handle) {
#ifdef USE_ICONV
            iconv_close(charset_converters[i].handle);
#elif USE_UCNV
            ucnv_close(charset_converters[i].handle);
#endif
            charset_converters[i].handle = NULL;
        }
    }
}


int str_convert_get_converter(string_encoding_t from_charset, CONVERTER_HANDLE *from_conv_p,
                              string_encoding_t to_charset, CONVERTER_HANDLE *to_conv_p)
{
    CONVERTER_HANDLE from_conv = NULL;
    CONVERTER_HANDLE to_conv = NULL;
    int i;


    CN_LOG_D("from_charset=%d, to_charset=%d", from_charset, to_charset);

    /* Parameter validation */
    if (from_charset >= CHARSET_MAX) {
        CN_LOG_E("Invalid from_charset %d!", from_charset);
        return -1;
    }

    if (to_charset >= CHARSET_MAX) {
        CN_LOG_E("Invalid to_charset %d!", to_charset);
        return -1;
    }

    /* Search for existing converter(s) */
    for (i = 0; i < MAX_CONVERTERS; i++) {
        if (NULL != charset_converters[i].handle) {
#ifdef USE_ICONV

            if (from_charset == charset_converters[i].from_charset &&
                    to_charset == charset_converters[i].to_charset) {
                from_conv = to_conv = charset_converters[i].handle;
                break;
            }

#elif USE_UCNV

            if (from_charset == charset_converters[i].charset) {
                from_conv = charset_converters[i].handle;

                if (str_convert_is_utf_16(to_charset)) {
                    /* Conversion to UTF-16 requires one step only */
                    to_conv = from_conv;
                    break;
                }
            } else if (to_charset == charset_converters[i].charset) {
                to_conv = charset_converters[i].handle;

                if (str_convert_is_utf_16(from_charset)) {
                    /* Conversion from UTF-16 requires one step only */
                    from_conv = to_conv;
                    break;
                }
            }

#endif
        }
    }

    /* Allocate converters */
    if (NULL == from_conv) {
        str_convert_alloc_converter(from_charset, &from_conv,
                                    to_charset, &to_conv);
    }

    if (NULL == to_conv) {
        str_convert_alloc_converter(from_charset, &from_conv,
                                    to_charset, &to_conv);
    }

    /* Exit if done */
#ifndef USE_UCNV

    if (from_conv && to_conv) {
        goto all_done;
    }

#else

    /* Conversion to or from UTF-16BE (or subset thereof) requires one converter only */
    if ((from_conv || str_convert_is_utf_16(from_charset)) &&
            (to_conv || str_convert_is_utf_16(to_charset))) {
        goto all_done;
    }

#endif

    CN_LOG_E("Failed getting a converter from=%d-\"%s\", to=%d-\"%s\"!",
             from_charset, charset_names[from_charset].string,
             to_charset, charset_names[to_charset].string);

    return -1;

all_done:
    *from_conv_p = from_conv;
    *to_conv_p = to_conv;

    return 1;
}


int str_convert_alloc_converter(string_encoding_t from_charset, CONVERTER_HANDLE *from_conv_p,
                                string_encoding_t to_charset, CONVERTER_HANDLE *to_conv_p)
{
    CONVERTER_HANDLE from_conv = *from_conv_p;
    CONVERTER_HANDLE to_conv = *to_conv_p;
    int i;


    CN_LOG_D("from_charset=%d, to_charset=%d", from_charset, to_charset);

    /* Allocate new converter(s) */
    for (i = 0; i < MAX_CONVERTERS; i++) {
        if (NULL == charset_converters[i].handle) {
            CONVERTER_HANDLE handle;
#ifdef USE_ICONV
            int errnum;

            /* Open the converter */
            handle = iconv_open(charset_names[to_charset].string,
                                charset_names[from_charset].string);

            /* Check for errors */
            if (handle == (CONVERTER_HANDLE) - 1) {
                errnum = errno;
                CN_LOG_E("iconv_open(%d-\"%s\", %d-\"%s\") error %d (%s)!",
                         to_charset, charset_names[to_charset].string,
                         from_charset, charset_names[from_charset].string,
                         errnum, strerror(errnum));
                goto error;
            }

            /* Update converter table */
            charset_converters[i].from_charset = from_charset;
            charset_converters[i].to_charset = to_charset;
            charset_converters[i].handle = handle;

            to_conv = from_conv = handle;

            /* Done */
            goto done;

#elif USE_UCNV
            string_encoding_t charset;
            UErrorCode errcode = U_ZERO_ERROR;

            if (NULL == from_conv && !str_convert_is_utf_16(from_charset)) {
                charset = from_charset;
            } else if (NULL == to_conv && !str_convert_is_utf_16(to_charset)) {
                charset = to_charset;
            } else {
                /* Already done! */
                goto done;
            }

            /* Open the converter */
            handle = ucnv_open(charset_names[charset].string, &errcode);

            /* Check for errors or warnings */
            if (U_ZERO_ERROR != errcode) {
                CN_LOG_E("ucnv_open(%d-\"%s\") error/warning code %d!",
                         charset, charset_names[charset].string, errcode);
                goto error;
            }

            /* Update converter table and retry the search */
            charset_converters[i].charset = charset;
            charset_converters[i].handle = handle;

            if (charset == from_charset) {
                from_conv = handle;
            } else {
                to_conv = handle;
            }

            /* Done */
            goto done;
#endif /* USE_UCNV */
        }
    }

error:
    return -1;

done:
    *from_conv_p = from_conv;
    *to_conv_p = to_conv;

    return 1;
}


int str_convert_septet_to_octet(size_t from_length, uint8_t *from_string_p, size_t spare_bits,
                                size_t to_length, uint8_t *to_string_p)
{
    uint8_t carry;
    size_t pos;
    size_t septet;
    int shift;


    for (carry = pos = septet = shift = 0;
            pos < from_length && septet < to_length;
            pos++, shift = (shift + 1) % 7) {

        to_string_p[septet++] = (from_string_p[pos] << shift | carry) & 0x7F;
        carry = from_string_p[pos] >> (7 - shift);

        if (6 == shift) {
            to_string_p[septet++] = carry & 0x7F;
            carry >>= 7;
        }
    }

    if ((from_length % 7 == 0 && (from_string_p[from_length-1] & 0x7F) == 0x0D) || spare_bits == 7) {
        /* Ignore CR at the End. Truncate the string. */
        septet--;
    }

    if (septet <= to_length) {
        /* For safety, null terminate the string */
        to_string_p[septet] = '\0';
    }

    return septet;
}


int str_convert_octet_to_septet(size_t from_length, uint8_t *from_string_p,
                                size_t to_length, uint8_t *to_string_p)
{
    uint16_t carry;
    size_t pos;
    size_t octet;
    int shift;

    memset(to_string_p, 0x0D, to_length);

    for (carry = pos = octet = 0, shift = 7;
            pos < from_length && octet < to_length;
            pos++, shift += 7) {

        carry |= (uint16_t)(from_string_p[pos] & 0x7F) << (shift - 7);

        if (7 < shift) {
            to_string_p[octet++] = (uint8_t)carry;
            carry >>= 8;
            shift -= 8;
        }
    }

    if (0 < shift) {
        to_string_p[octet++] = (uint8_t)carry << (shift - 7);
    }

    if (octet <= to_length) {
        to_string_p[octet] = '\0';
    }

    return octet;
}


#ifdef USE_UCNV
bool str_convert_is_utf_16(string_encoding_t charset)
{
    bool result = false;

    if (CHARSET_UTF_16 == charset ||
            (CHARSET_UCS_2 == charset && CHARSET_UTF_16 == CHARSET_UTF_16BE)) {
        result = true;
    }

    return result;
}
#endif /* USE_UCNV */
