/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "atc_log.h"
#include "atc_string.h"


/*
 * ATC string conversion
 *
 * These utilities converts strings between different character sets. Currently
 * supported character sets are;
 *      UTF-8
 *      UCS2
 *      HEX
 *      GSM7
 *      GSM8
 *
 * NOTE 1: Any conversion of GSM7 to or from any other character set than GSM8,
 *         will result in an intermediate translation to GSM8.
 *
 * NOTE 2: Any conversion of UTF-8 to or from any other character set than UCS2,
 *         will result in an intermediate translation to UCS2.
 *
 * NOTE 3: The length fields indicate the string length in bytes, not characters.
 *         Also note that a GSM escape character is counted as one character.
 *
 * NOTE 4: For GSM7, add or conversely remove carriage-return (CR) padding from
 *         the end of a GSM7 string in cases where only one bit is used in
 *         the last byte.
 */

typedef struct {
    atc_charset_t charset;
    char *name_p;
} atc_charset_entry_t;

/* NOTE: the order here must be the same as for atc_charset_t */
static atc_charset_entry_t charset_db[] = {
    {ATC_CHARSET_UTF8           , "UTF-8"},
    {ATC_CHARSET_HEX            , "HEX"},
    {ATC_CHARSET_GSM7           , "GSM7"},
    {ATC_CHARSET_GSM8           , "GSM8"},
    {ATC_CHARSET_IRA            , "IRA"},
    {ATC_CHARSET_8859_1         , "8859/1"},
    {ATC_CHARSET_UCS2           , "UCS2"},
    {ATC_CHARSET_UNDEF_BINARY   , "BINARY"},
    {ATC_CHARSET_MAX            , ""},
};

#define ATC_GSM_CR_CHARACTER            (uint8_t)0x0D   /* Carriage return */
#define ATC_GSM_ESCAPE_CHARACTER        (uint8_t)0x1B   /* Escape */
#define ATC_ESCAPE_CHARACTER            (uint8_t)0x5C   /* Backslash '\' */
#define ATC_MAX_NUMBER_ESCAPED_QUOTES   10

static const uint16_t util_gsm8_to_ucs2[128] = {
    '@',   0xA3,  '$',   0xA5,  0xE8,  0xE9,  0xF9,  0xEC,  0xF2,  0xC7,  '\n',  0xD8,  0xF8,  '\r',  0xC5,  0xE5,
    0x394, '_',   0x3A6, 0x393, 0x39B, 0x3A9, 0x3A0, 0x3A8, 0x3A3, 0x398, 0x39E, 0,     0xC6,  0xE6,  0xDF,  0xC9,
    ' ',   '!',   '"',   '#',   0xA4,  '%',   '&',   '\'',  '(',   ')',   '*',   '+',   ',',   '-',   '.',   '/',
    '0',   '1',   '2',   '3',   '4',   '5',   '6',   '7',   '8',   '9',   ':',   ';',   '<',   '=',   '>',   '?',
    0xA1,  'A',   'B',   'C',   'D',   'E',   'F',   'G',   'H',   'I',   'J',   'K',   'L',   'M',   'N',   'O',
    'P',   'Q',   'R',   'S',   'T',   'U',   'V',   'W',   'X',   'Y',   'Z',   0xC4,  0xD6,  0x147, 0xDC,  0xA7,
    0xBF,  'a',   'b',   'c',   'd',   'e',   'f',   'g',   'h',   'i',   'j',   'k',   'l',   'm',   'n',   'o',
    'p',   'q',   'r',   's',   't',   'u',   'v',   'w',   'x',   'y',   'z',   0xE4,  0xF6,  0xF1,  0xFC,  0xE0,
};

static const uint16_t util_gsm8_extended_to_ucs2[128] = {
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     '\f',  0,     0,     0,     0,     0,
    0,     0,     0,     0,     '^',   0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,     '{',   '}',   0,     0,     0,     0,     0,     '\\',
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     '[',   '~',   ']',   0,
    '|',   0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0x20AC, 0,    0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
};

static const char hex_char[] = "0123456789ABCDEF";


/********************************************************************
 * Private prototypes
 ********************************************************************
 */
static char *atc_get_charset_name(atc_charset_t charset);
static size_t atc_get_strnlen_chars(atc_charset_t charset, void *dest_p, size_t length);
static bool atc_start_of_string(uint8_t *src_p, int pos);
static bool atc_end_of_string(uint8_t *src_p, int rem);
/* GSM7 <-> GSM8 */
static uint8_t atc_get_gsm8char_from_gsm7string(uint8_t *string_p, size_t char_nbr);
static void atc_put_gsm8char_in_gsm7string(uint8_t **string_pp, uint8_t c, size_t char_nbr);
static size_t atc_convert_gsm7_to_gsm8(uint8_t *src_p, size_t byte_length, uint8_t *dest_p);
static size_t atc_convert_gsm8_to_gsm7(uint8_t *src_p, size_t byte_length, uint8_t *dest_p);
/* GSM8 <-> UCS2 */
static int atc_get_gsm8_from_ucs2(uint16_t ucs2, uint8_t *gsm8_p);
static size_t atc_convert_gsm8_to_ucs2(uint8_t *src_p, size_t byte_length, uint16_t *dest_p);
static size_t atc_convert_ucs2_to_gsm8(uint16_t *src_p, size_t byte_length, uint8_t *dest_p);
/* UCS2 <-> UTF-8 */
static size_t atc_convert_ucs2_to_utf8(uint16_t *src_p, size_t byte_length, uint8_t *dest_p);
static size_t atc_convert_utf8_to_ucs2(uint8_t *src_p, size_t byte_length, uint16_t *dest_p);
/* BYTE / WORD <-> HEX */
static int atc_get_nibble_from_hex(uint8_t c);
static int atc_get_byte_from_hex(uint8_t hi_c, uint8_t lo_c);
static size_t atc_write_character_to_hex(uint32_t src, size_t no_of_bytes, uint8_t *dest_p);
static size_t atc_convert_hex_to_byte(uint8_t *src_p, size_t byte_length, uint8_t *dest_p);
static size_t atc_convert_byte_to_hex(uint8_t *src_p, size_t byte_length, uint8_t *dest_p);
static size_t atc_convert_word_to_hex(uint16_t *src_p, size_t byte_length, uint8_t *dest_p);
static size_t atc_convert_hex_to_word(uint8_t *src_p, size_t byte_length, uint16_t *dest_p);

static size_t atc_copy_in_to_out(uint8_t *src_p, size_t byte_length, uint8_t *dest_p, size_t termination_chars);

/********************************************************************
 * Private methods
 ********************************************************************
 */

/*
 * Returns name of character set.
 */
char *atc_get_charset_name(atc_charset_t charset)
{
    if (0 > (int)charset || ATC_CHARSET_MAX <= charset) {
        return NULL;
    }

    return charset_db[charset].name_p;
}


/*
 * Count number of characters (not bytes) present in string.
 * Most useful for encodings with variable byte lengths, such as UTF-8.
 */
size_t atc_get_strnlen_chars(atc_charset_t charset, void *str_p, size_t length)
{
    size_t char_length = 0;

    switch (charset) {
    case ATC_CHARSET_UTF8: {
        uint8_t *utf8_p = (uint8_t *)str_p;
        size_t pos;
        uint8_t c;

        for (char_length = pos = 0; pos < length;) {
            c = utf8_p[pos++];

            if (0x80 < c) {
                if (0xC0 == (c & 0xE0)) {
                    /* 2-byte long UTF-8 character */
                    if (0x80 < utf8_p[pos++] && pos < length) {
                        char_length++;
                        continue;
                    }

                    /* Invalid or incomplete character */
                    break;
                } else if (0xE0 == (c & 0xF0)) {
                    /* 3-byte long UTF-8 character */
                    if (0x80 < utf8_p[pos++] && pos < length) {
                        if (0x80 < utf8_p[pos++] && pos < length) {
                            char_length++;
                            continue;
                        }
                    }

                    /* Invalid or incomplete character */
                    break;
                } else {
                    /* 4-byte long UTF-8 character */
                    if (0x80 < utf8_p[pos++] && pos < length) {
                        if (0x80 < utf8_p[pos++] && pos < length) {
                            if (0x80 < utf8_p[pos++] && pos < length) {
                                char_length++;
                                continue;
                            }
                        }
                    }

                    /* Invalid or incomplete character */
                    break;
                }
            } else {
                /* 1-byte long UTF-8 character */
                char_length++;
            }
        }

        break;
    }

    case ATC_CHARSET_HEX: {
        uint8_t *hex_p = (uint8_t *)str_p;
        uint8_t c;

        for (char_length = 0; char_length < length; char_length++) {
            c = hex_p[char_length];

            if ((c < '0' || c > '9') &&
                    (c < 'A' || c > 'F') &&
                    (c < 'a' || c > 'f')) {
                /* Not a valid HEX character */
                break;
            }
        }

        break;
    }

    case ATC_CHARSET_GSM7:
        char_length = length * 8 / 7;
        break;

    case ATC_CHARSET_GSM8:
        char_length = length;
        break;

    case ATC_CHARSET_IRA:
        char_length = strnlen((char *)str_p, length);
        break;

    case ATC_CHARSET_UCS2: {
        uint16_t *ucs2_p = (uint16_t *)str_p;

        for (char_length = 0; char_length < (length / sizeof(uint16_t)); char_length++) {
            if (ucs2_p[char_length] == 0x0) {
                break;
            }
        }

        break;
    }

    case ATC_CHARSET_UNDEF_BINARY:
        char_length = length;
        break;

    default:
        ATC_LOG_W("Unhandled character set (%d) %s!", charset, atc_get_charset_name(charset));
        char_length = 0;
        break;
    }

    return char_length;
}


/*
 * Returns the specified character in a GSM7 encoded string. First
 * character in string is number 0.
 */
static uint8_t atc_get_gsm8char_from_gsm7string(uint8_t *string_p, size_t char_nbr)
{
    int index = 0;
    int shift = 0;
    int rest = 0;
    uint8_t c;

    /* Calculate the 8-bit string index for the 7-bit character. */
    if (char_nbr > 0) {
        index = char_nbr - 1 - (char_nbr - 1) / 8;
    }

    /* Calculate the bit shift. */
    rest = char_nbr % 8;

    if (rest < 2) {
        shift = rest * 7;
    } else {
        shift = rest * 7 - (rest - 1) * 8;
    }

    /* Parse out the 7-bit character from the one or two 8-bit words needed. */
    c = (*(string_p + index) >> shift) & 0x7f;

    if (shift > 1) {
        c = ((*(string_p + index + 1) << (8 - shift)) | c) & 0x7f;
    }

    return c;
}


static int atc_get_gsm8_from_ucs2(uint16_t ucs2, uint8_t *gsm8_p)
{
    int i;

    for (i = 0; i < 128; i++) {
        if (ucs2 == util_gsm8_to_ucs2[i]) {
            *gsm8_p = (uint8_t)i;
            return 0;
        }

        if (ucs2 == util_gsm8_extended_to_ucs2[i]) {
            *gsm8_p = (uint8_t)i;
            return 1;
        }
    }

    /* Not found */
    return -1;
}


static void atc_put_gsm8char_in_gsm7string(uint8_t **string_pp, uint8_t c, size_t char_nbr)
{
    int shift = 0;
    int rest = 0;
    uint8_t *write_p = *string_pp;

    /* Calculate the bit shift. */
    rest = char_nbr % 8;

    if (rest < 2) {
        shift = rest * 7;
    } else {
        shift = rest * 7 - (rest - 1) * 8;
    }

    if (0 == shift) {
        *write_p = c & 0x7f;
    } else {
        *write_p = (uint8_t) * write_p | (c << shift);
        write_p++;
        *write_p = (uint8_t)(c >> (8 - shift));
    }

    *string_pp = write_p;
}


/* Convert a single character value to a string of hex characters, one pair for each byte */
static size_t atc_write_character_to_hex(uint32_t src, size_t no_of_bytes, uint8_t *dest_p)
{
    size_t src_byte_no, dest_char_no;
    uint8_t src_byte_value;

    for (src_byte_no = no_of_bytes, dest_char_no = 0; 0 < src_byte_no; src_byte_no--) {
        src_byte_value = (uint8_t)(src >> ((src_byte_no - 1) * 8));
        dest_p[dest_char_no++] = hex_char[src_byte_value >> 4 & 0xF];
        dest_p[dest_char_no++] = hex_char[src_byte_value & 0xF];
    }

    return dest_char_no;
}

/*
 * Converts a HEX encoded value into a binary value.
 *
 * hex_c        : Hex value
 *
 * Returns the 4-bit binary value converted from the input argument,
 * or -1 if the argument did not contain a recognizable hex value.
 */
static int atc_get_nibble_from_hex(uint8_t hex_c)
{
    if (hex_c >= '0' && hex_c <= '9') {
        return hex_c - '0';
    } else if (hex_c >= 'A' && hex_c <= 'F') {
        return 10 + hex_c - 'A';
    } else if (hex_c >= 'a' && hex_c <= 'f') {
        return 10 + hex_c - 'a';
    }

    return -1;
}

/*
 * Converts two HEX encoded values into a binary value.
 *
 * hi_hex_c     : High-order hex value
 * lo_hex_c     : Low-order hex value
 *
 * Returns the 8-bit binary value converted from the input arguments,
 * or -1 if the arguments did not contain recognizable hex values.
 */
static int atc_get_byte_from_hex(uint8_t hi_hex_c, uint8_t lo_hex_c)
{
    int hi_c, lo_c;

    hi_c = atc_get_nibble_from_hex(hi_hex_c);

    if (0 <= hi_c) {
        lo_c = atc_get_nibble_from_hex(lo_hex_c);

        if (0 <= lo_c) {
            return hi_c << 4 | lo_c;
        }
    }

    return -1;
}


static bool atc_start_of_string(uint8_t *src_p, int pos)
{
    if (0 < pos) {
        if (',' == *(src_p - 1) ||
                ' ' == *(src_p - 1) ||
                ('(' == *(src_p - 1) && (2 > pos || ',' == *(src_p - 2)))) {
            return true;
        }
    }

    return false;
}


static bool atc_end_of_string(uint8_t *src_p, int rem)
{
    if (0 < rem) {
        if (',' == *(src_p + 1) ||
                '\r' == *(src_p + 1) ||
                (')' == *(src_p + 1) && (2 > rem || ',' == *(src_p + 2) || '\r' == *(src_p + 2)))) {
            return true;
        }
    }

    return false;
}

/*
 * Converts a GSM7 encoded string into a GSM8 string.
 *
 * src_p        : Pointer to GSM7 encoded string
 * byte_length  : Number of bytes in source string to convert
 * dest_p       : Pointer to destination character string encoded in GSM8
 *
 * Note: Reduce length by 1 character if the end of the string is padded
 *       with a carriage-return (CR) character.
 *
 * Returns the number of 8-bit GSM8 characters written to the string.
 */
static size_t atc_convert_gsm7_to_gsm8(uint8_t *src_p, size_t byte_length, uint8_t *dest_p)
{
    size_t nbr_of_septets = byte_length * 8 / 7;
    uint8_t *write_p = dest_p;
    size_t characters = 0;
    uint8_t c;

    for (characters = 0; characters < nbr_of_septets; characters++) {
        /* Get next GSM8 character from GSM7 string */
        c = atc_get_gsm8char_from_gsm7string(src_p, characters);

        /* Write character to GSM8 string */
        *write_p++ = c;
    }

    /* Do not count a trailing CR if byte-length is a multiple of 7 */
    if (0 == (byte_length % 7) && ATC_GSM_CR_CHARACTER == *(write_p - 1)) {
        characters--;
    }

    return characters;
}

/*
 * Converts a GSM8 encoded string into a GSM7 string.
 *
 * src_p        : Pointer to GSM8 encoded string
 * byte_length  : Number of bytes in source string to convert
 * dest_p       : Pointer to destination character string encoded in GSM7
 *
 * Note: Add carriage-return (CR) pad character and increase length by 1
 *       character if there is room for a character in the last byte.
 *
 * Returns the number of 7-bit GSM7 characters written to the string.
 */
static size_t atc_convert_gsm8_to_gsm7(uint8_t *src_p, size_t byte_length, uint8_t *dest_p)
{
    uint8_t *read_p = src_p;
    uint8_t *src_end_p = src_p + byte_length;
    uint8_t *write_p = dest_p;
    size_t characters = 0;
    uint8_t c;

    while (read_p < src_end_p) {
        /* Get next GSM8 character from string */
        c = *read_p++;

        /* Pack into GSM7 string */
        atc_put_gsm8char_in_gsm7string(&write_p, c, characters);

        characters++;
    }

    /* Pad string with a CR if byte-length is 8n - 1 where n=1,2,3.. */
    /* The following calculation checks if the remainder of dividing
     * byte_length by 8 is equal to 7. In that case, byte_length shall
     * satisfy the above mentioned condition */
    if ((byte_length & 0x07) == 0x07 ) {
        atc_put_gsm8char_in_gsm7string(&write_p, ATC_GSM_CR_CHARACTER, characters);
        characters++;
    }

    return characters;
}

/*
 * Converts a GSM8 encoded string into a UCS2 string.
 *
 * src_p        : Pointer to GSM8 encoded string
 * byte_length  : Number of bytes in source string to convert
 * dest_p       : Pointer to destination character string encoded in UCS2
 *
 * Returns the number of 16-bit UCS2 characters written to the string.
 */
static size_t atc_convert_gsm8_to_ucs2(uint8_t *src_p, size_t byte_length, uint16_t *dest_p)
{
    uint8_t *read_p = src_p;
    uint8_t *src_end_p = src_p + byte_length;
    uint8_t *write_p = (uint8_t *)dest_p;
    size_t characters = 0;
    uint8_t gsm8_c;
    uint16_t ucs2_c;

    while (read_p < src_end_p) {
        /* Get next GSM8 character from string */
        gsm8_c = *read_p++;

        /* Get the corresponding UCS2 value */
        if (ATC_GSM_ESCAPE_CHARACTER == gsm8_c) {
            gsm8_c = *read_p++;
            ucs2_c = util_gsm8_extended_to_ucs2[gsm8_c];
        } else {
            ucs2_c = util_gsm8_to_ucs2[gsm8_c];
        }

        /* Write out the 16-bit UCS2 character, high-order byte first */
        *write_p++ = (uint8_t)(ucs2_c >> 8);
        *write_p++ = (uint8_t)ucs2_c;

        characters++;
    }

    /* Add string termination character */
    *write_p++ = '\0';
    *write_p = '\0';

    return characters;
}

/*
 * Converts a UCS2 encoded string into a GSM8 string.
 *
 * src_p        : Pointer to UCS2 encoded string
 * byte_length  : Number of bytes in source string to convert
 * dest_p       : Pointer to destination character string encoded in GSM8
 *
 * Returns the number of 8-bit GSM8 characters written to the string. Note that
 * any escape characters are included in the count.
 */
static size_t atc_convert_ucs2_to_gsm8(uint16_t *src_p, size_t byte_length, uint8_t *dest_p)
{
    uint8_t *read_p = (uint8_t *)src_p;
    uint8_t *src_end_p = read_p + byte_length;
    uint8_t *write_p = dest_p;
    size_t characters = 0;
    uint16_t ucs2_c;
    uint8_t gsm8_c;
    int result;

    /* Two bytes (UCS2) needed for conversion of one character, adjust the size. */
    src_end_p -= byte_length % sizeof(uint16_t);

    while (read_p < src_end_p) {
        /* Read out the 16 bit UCS2 character */
        ucs2_c = (*read_p << 8) | *(read_p + 1);

        /* Convert UCS2 character to GSM8 character */
        result = atc_get_gsm8_from_ucs2(ucs2_c, &gsm8_c);

        if (0 > result) {
            ATC_LOG_W("Unable to convert UCS2 character 0x%X to GSM alphabet!", ucs2_c);
        } else {
            if (0 < result) {
                *write_p++ = ATC_GSM_ESCAPE_CHARACTER;
                characters++;
            }

            *write_p++ = gsm8_c;
            characters++;
        }

        read_p += sizeof(uint16_t);
    }

    return characters;
}

/*
 * Converts a UCS2 encoded string into a UTF-8 string.
 *
 * src_p        : Pointer to UCS2 encoded string
 * byte_length  : Number of bytes in source string to convert
 * dest_p       : Pointer to destination character string encoded in UTF-8
 *
 * Returns the number of 8- to 32-bit UTF-8 characters written to the string.
 *
 * NOTE: UCS2 limitations only ever allow a 3-byte long (24-bit) UTF-8
 * character to be produced. Never a 4-byte (32-bit) character.
 */
static size_t atc_convert_ucs2_to_utf8(uint16_t *src_p, size_t byte_length, uint8_t *dest_p)
{
    uint8_t *read_p = (uint8_t *)src_p;
    uint8_t *src_end_p = read_p + byte_length;
    uint8_t *write_p = dest_p;
    size_t characters = 0;
    uint16_t c;

    /* Two bytes (UCS2) needed for conversion of one character, adjust the size. */
    src_end_p -= byte_length % 2;

    while (read_p < src_end_p) {
        /* Read out the 16-bit UCS2 character, high-order byte first */
        c = *read_p++;
        c = (c << 8) | *read_p++;

        /* Convert UCS2 character to UTF-8 character */
        if (c < 0x80) {
            *write_p++ = (uint8_t)c;
        } else if (c < 0x800) {
            *write_p++ = (uint8_t)((c >> 6) | 0xC0);
            *write_p++ = (uint8_t)((c & 0x3f) | 0x80);
        } else {
            *write_p++ = (uint8_t)((c >> 12) | 0xE0);
            *write_p++ = (uint8_t)(((c >> 6) & 0x3F) | 0x80);
            *write_p++ = (uint8_t)((c & 0x3F) | 0x80);
        }

        characters++;
    }

    /* Add string termination character */
    *write_p = '\0';

    return characters;
}

/*
 * Converts a UTF-8 encoded string into a UCS2 string.
 *
 * src_p        : Pointer to UCS2 encoded string
 * byte_length  : Number of bytes in source string to convert
 * dest_p       : Pointer to destination character string encoded in UTF-8
 *
 * Returns the number of 16-bit UCS2 characters written to the string.
 */
static size_t atc_convert_utf8_to_ucs2(uint8_t *src_p, size_t byte_length, uint16_t *dest_p)
{
    uint8_t *read_p = src_p;
    uint8_t *src_end_p = src_p + byte_length;
    uint8_t *write_p = (uint8_t *)dest_p;
    size_t characters = 0;
    uint16_t c;

    while (read_p < src_end_p) {
        /* Convert a UTF-8 character to UCS2. */
        c = *read_p++;

        if (0x80 < c) {
            if (0xC0 == (c & 0xE0)) {
                /* 2-byte long UTF-8 character */
                if (read_p < src_end_p) {
                    c = c & 0x1F;
                    c = (c << 6) | (*read_p++ & 0x3F);
                } else {
                    /* Input string ended prematurely */
                    read_p++;
                    continue;
                }
            } else if (0xE0 == (c & 0xF0)) {
                /* 3-byte long UTF-8 character */
                if (read_p + 1 < src_end_p) {
                    c = c & 0x0F;
                    c = (c << 6) | (*read_p++ & 0x3F);
                    c = (c << 6) | (*read_p++ & 0x3F);
                } else {
                    /* Input string ended prematurely */
                    read_p += 2;
                    continue;
                }
            } else {
                /* 4-byte long UTF-8 character
                 * This long UTF-8 characters have no corresponding
                 * representation in the UCS2 character set. */
                *read_p += 3;
                continue;
            }
        }

        /* Write out the 16 bit UCS2 character, high-order byte first */
        *write_p++ = (uint8_t)(c >> 8);
        *write_p++ = (uint8_t)c;

        characters++;
    }

    /* Add string termination character */
    *write_p++ = '\0';
    *write_p = '\0';

    return characters;
}

/*
 * Converts a HEX encoded string into a byte (binary) string.
 *
 * src_p        : Pointer to HEX encoded string
 * byte_length  : Number of bytes in source string to convert
 * dest_p       : Pointer to destination character string encoded in binary
 *
 * Returns the number of 8-bit binary characters written to the destination string.
 */
static size_t atc_convert_hex_to_byte(uint8_t *src_p, size_t byte_length, uint8_t *dest_p)
{
    uint8_t *read_p = src_p;
    uint8_t *src_end_p = src_p + byte_length;
    uint8_t *write_p = dest_p;
    size_t characters = 0;
    int c;

    /* Two HEX characters needed for conversion of one character, adjust the size. */
    src_end_p -= byte_length % 2;

    while (read_p < src_end_p) {
        /* Convert from a pair of HEX characters into one GSM8 character */
        c = atc_get_byte_from_hex(*read_p, *(read_p + 1));

        /* Skip characters that do not convert */
        if (0 > c) {
            ATC_LOG_W("Conversion from hex characters '%c%c' failed!", *read_p, *(read_p + 1));
        } else {
            /* Write GSM8 character to string */
            *write_p++ = (uint8_t)c;

            characters++;
        }

        read_p += sizeof(uint16_t);
    }

    return characters;
}

/*
 * Converts a binary encoded string into a HEX string.
 *
 * src_p        : Pointer to binary encoded string
 * byte_length  : Number of bytes in source string to convert
 * dest_p       : Pointer to destination character string encoded in HEX
 *
 * Returns the number of 8-bit HEX characters written to the destination string.
 */
static size_t atc_convert_byte_to_hex(uint8_t *src_p, size_t byte_length, uint8_t *dest_p)
{
    uint8_t *read_p = src_p;
    uint8_t *src_end_p = src_p + byte_length;
    uint8_t *write_p = dest_p;
    size_t characters = 0;
    uint8_t c;

    while (read_p < src_end_p) {
        /* Get next binary character */
        c = *read_p++;

        /* Convert a single character value to a string of HEX characters, one pair for each byte */
        write_p += atc_write_character_to_hex(c, sizeof(uint8_t), write_p);

        characters += 2;
    }

    /* Add string termination character */
    *write_p = '\0';

    return characters;
}

/*
 * Converts a word (2-byte long characters such as UCS2) encoded string into a HEX string.
 *
 * src_p        : Pointer to 2-byte word character encoded string
 * byte_length  : Number of bytes in source string to convert
 * dest_p       : Pointer to destination character string encoded in HEX
 *
 * Returns the number of 8-bit HEX characters written to the destination string.
 */
static size_t atc_convert_word_to_hex(uint16_t *src_p, size_t byte_length, uint8_t *dest_p)
{
    /* Two bytes for each word, adjust the size. */
    byte_length -= byte_length % sizeof(uint16_t);

    return atc_convert_byte_to_hex((uint8_t *)src_p, byte_length, dest_p);
}

/*
 * Converts a HEX encoded string into a word (2-byte long characters such as UCS2) encoded string.
 *
 * src_p        : Pointer to HEX character encoded string
 * byte_length  : Number of bytes in source string to convert
 * dest_p       : Pointer to destination 2-byte word character string
 *
 * Returns the number of 16-bit word characters written to the destination string.
 */
static size_t atc_convert_hex_to_word(uint8_t *src_p, size_t byte_length, uint16_t *dest_p)
{
    /* Four HEX characters needed for one word, adjust the size. */
    byte_length -= byte_length % (sizeof(uint16_t) * 2);

    return atc_convert_hex_to_byte(src_p, byte_length, (uint8_t *)dest_p) / sizeof(uint16_t);
}

/*
 * Copies input string to output string without conversion.
 *
 * src_p            : Pointer to source string
 * byte_length      : Number of bytes in source string to copy
 * dest_p           : Pointer to destination string
 * termination_chars: Number of string terminations characters
 *                    written to the destination string
 *
 * Returns the number of bytes copied to the destination string.
 */
static size_t atc_copy_in_to_out(uint8_t *src_p, size_t byte_length, uint8_t *dest_p, size_t termination_chars)
{
    uint8_t *read_p = src_p;
    uint8_t *src_end_p = src_p + byte_length;
    uint8_t *write_p = dest_p;
    size_t characters = 0;

    while (read_p < src_end_p) {
        *write_p++ = *read_p++;

        characters++;
    }

    /* Add string termination character(s) */
    while (0 < termination_chars--) {
        *write_p++ = '\0';
    }

    return characters;
}


/********************************************************************
 * Public methods
 ********************************************************************
 */

/*
 * Converts a source string with one character encoding to an output string with the
 * same or different character encoding.
 *
 * src_charset          : Source character encoding
 * src_p                : Pointer to HEX character encoded string
 * src_byte_length      : Number of bytes in source string to convert
 * dest_charset         : Destination character encoding
 * dest_p               : Pointer to destination character string
 * dest_byte_length_p   : Optional pointer to
 *
 * Returns the number of characters (which may or may NOT be the same as the number of bytes!)
 * written to the destination string.
 */
size_t atc_string_convert(atc_charset_t src_charset, uint8_t *src_p, size_t src_byte_length,
                          atc_charset_t dest_charset, uint8_t *dest_p, size_t *dest_byte_length_p)
{
    uint8_t *gsm8_p = NULL;
    uint16_t *ucs2_p = NULL;
    size_t num_bytes = src_byte_length;
    size_t num_chars;

    ATC_LOG_D("src_charset=%d, src_p=%p, src_byte_length=%d, dest_charset=%d, dest_p=%p, dest_byte_length_p=%p",
              src_charset, src_p, src_byte_length, dest_charset, dest_p, dest_byte_length_p);

    /* Parameter validation */
    if (NULL == src_p) {
        ATC_LOG_E("Invalid parameter 'src_p'");
        num_chars = num_bytes = 0;
        goto error;
    }

    if (1 > src_byte_length) {
        ATC_LOG_E("Invalid parameter 'src_byte_length', value = %d", src_byte_length);
        num_chars = num_bytes = 0;
        goto error;
    }

    if (NULL == dest_p) {
        ATC_LOG_E("Invalid parameter 'dest_p'");
        num_chars = num_bytes = 0;
        goto error;
    }

    switch (dest_charset) {
    case ATC_CHARSET_UTF8:

        switch (src_charset) {
        case ATC_CHARSET_UTF8:
            /* UTF-8 -> UTF-8 */
            num_bytes = atc_copy_in_to_out(src_p, num_bytes, dest_p, sizeof(uint8_t));
            num_chars = atc_get_strnlen_chars(dest_charset, dest_p, num_bytes);
            break;

        case ATC_CHARSET_HEX:
            /* HEX -> UCS2 -> UTF-8 */
            num_chars = num_bytes / 4;  /* Four HEX characters needed for one 2-byte UCS2 character */
            ucs2_p = (uint16_t *)calloc(sizeof(uint16_t), num_chars + 1);

            if (NULL == ucs2_p) {
                ATC_LOG_E("%s->%s: Memory allocation of %d bytes failed!",
                          atc_get_charset_name(src_charset), atc_get_charset_name(dest_charset),
                          (num_chars + 1) * sizeof(uint16_t));
                break;
            }

            num_chars = atc_convert_hex_to_word(src_p, num_bytes, ucs2_p);
            num_bytes = num_chars * sizeof(uint16_t);

            if (1 > num_bytes) {
                ATC_LOG_E("%s->%s: HEX to UCS2 conversion error!",
                          atc_get_charset_name(src_charset), atc_get_charset_name(dest_charset));
                break;
            }

            num_chars = atc_convert_ucs2_to_utf8(ucs2_p, num_bytes, dest_p);
            num_bytes = strlen((const char *)dest_p);
            break;

        case ATC_CHARSET_GSM7:
            /* GSM7 -> GSM8 -> UCS2 -> UTF-8 */
            num_chars = num_bytes * 8 / 7; /* One 7-bit GSM7 character need one 8-bit (1-byte) GSM8 character */
            gsm8_p = (uint8_t *)calloc(sizeof(uint8_t), num_chars + 1);

            if (NULL == gsm8_p) {
                ATC_LOG_E("%s->%s: Memory allocation of %d bytes failed!",
                          atc_get_charset_name(src_charset), atc_get_charset_name(dest_charset),
                          (num_chars + 1) * sizeof(uint8_t));
                break;
            }

            num_chars = num_bytes = atc_convert_gsm7_to_gsm8(src_p, num_bytes, gsm8_p);

            if (1 > num_bytes) {
                ATC_LOG_E("%s->%s: GSM7 to GSM8 conversion error!",
                          atc_get_charset_name(src_charset), atc_get_charset_name(dest_charset));
                break;
            }

            ucs2_p = (uint16_t *)calloc(sizeof(uint16_t), num_bytes + 1);

            if (NULL == ucs2_p) {
                ATC_LOG_E("%s->%s: Memory allocation of %d bytes failed!",
                          atc_get_charset_name(src_charset), atc_get_charset_name(dest_charset),
                          (num_bytes + 1) * sizeof(uint16_t));
                break;
            }

            num_chars = atc_convert_gsm8_to_ucs2(gsm8_p, num_bytes, ucs2_p);
            num_bytes = num_chars * sizeof(uint16_t);

            if (1 > num_bytes) {
                ATC_LOG_E("%s->%s: GSM7 to UCS2 conversion error!",
                          atc_get_charset_name(src_charset), atc_get_charset_name(dest_charset));
                break;
            }

            num_chars = atc_convert_ucs2_to_utf8(ucs2_p, num_bytes, dest_p);
            num_bytes = strlen((const char *)dest_p);
            break;

        case ATC_CHARSET_GSM8:
            /* GSM8 -> UCS2 -> UTF-8 */
            num_chars = num_bytes;
            ucs2_p = (uint16_t *)calloc(sizeof(uint16_t), num_chars + 1);

            if (NULL == ucs2_p) {
                ATC_LOG_E("%s->%s: Memory allocation of %d bytes failed!",
                          atc_get_charset_name(src_charset), atc_get_charset_name(dest_charset),
                          (num_chars + 1) * sizeof(uint16_t));
                break;
            }

            num_chars = atc_convert_gsm8_to_ucs2(src_p, num_bytes, ucs2_p);
            num_bytes = num_chars * sizeof(uint16_t);

            if (1 > num_bytes) {
                ATC_LOG_E("%s->%s: GSM8 to UCS2 conversion error!",
                          atc_get_charset_name(src_charset), atc_get_charset_name(dest_charset));
                break;
            }

            num_chars = atc_convert_ucs2_to_utf8(ucs2_p, num_bytes, dest_p);
            num_bytes = strlen((const char *)dest_p);
            break;

        case ATC_CHARSET_UCS2:
            /* UCS2 -> UTF-8 */
            num_chars = atc_convert_ucs2_to_utf8((uint16_t *)src_p, num_bytes, dest_p);
            num_bytes = strlen((const char *)dest_p);
            break;

        default:
            ATC_LOG_E("%s->%s: Unsupported string conversion!",
                      atc_get_charset_name(src_charset), atc_get_charset_name(dest_charset));
            num_chars = num_bytes = 0;
            break;
        }

        break;

    case ATC_CHARSET_HEX:

        switch (src_charset) {
        case ATC_CHARSET_UTF8:
            /* UTF-8 -> UCS2 -> HEX */
            num_chars = atc_get_strnlen_chars(src_charset, src_p, num_bytes);
            ucs2_p = (uint16_t *)calloc(sizeof(uint16_t), num_chars + 1);

            if (NULL == ucs2_p) {
                ATC_LOG_E("%s->%s: Memory allocation of %d bytes failed!",
                          atc_get_charset_name(src_charset), atc_get_charset_name(dest_charset),
                          (num_chars + 1) * sizeof(uint16_t));
                break;
            }

            num_chars = atc_convert_utf8_to_ucs2(src_p, num_bytes, ucs2_p);
            num_bytes = num_chars * sizeof(uint16_t);

            if (1 > num_bytes) {
                ATC_LOG_E("%s->%s: UTF-8 to UCS2 conversion error!",
                          atc_get_charset_name(src_charset), atc_get_charset_name(dest_charset));
                break;
            }

            num_chars = num_bytes = atc_convert_word_to_hex(ucs2_p, num_bytes, dest_p);
            break;

        case ATC_CHARSET_HEX:
            /* HEX -> HEX */
            num_chars = num_bytes = atc_copy_in_to_out(src_p, num_bytes, dest_p, sizeof(uint8_t));
            break;

        case ATC_CHARSET_GSM7:
            /* GSM7 -> GSM8 -> HEX */
            num_chars = num_bytes * 8 / 7;
            gsm8_p = (uint8_t *)calloc(sizeof(uint8_t), num_chars + 1);

            if (NULL == gsm8_p) {
                ATC_LOG_E("%s->%s: Memory allocation of %d bytes failed!",
                          atc_get_charset_name(src_charset), atc_get_charset_name(dest_charset),
                          (num_chars + 1) * sizeof(uint8_t));
                break;
            }

            num_chars = num_bytes = atc_convert_gsm7_to_gsm8(src_p, num_bytes, gsm8_p);

            if (1 > num_bytes) {
                ATC_LOG_E("%s->%s: GSM7 to GSM8 conversion error!",
                          atc_get_charset_name(src_charset), atc_get_charset_name(dest_charset));
                break;
            }

            num_chars = num_bytes = atc_convert_byte_to_hex(gsm8_p, num_bytes, dest_p);
            break;

        case ATC_CHARSET_GSM8:
        case ATC_CHARSET_UNDEF_BINARY:
            /* GSM8 or BINARY -> HEX */
            num_chars = num_bytes = atc_convert_byte_to_hex(src_p, num_bytes, dest_p);
            break;

        case ATC_CHARSET_UCS2:
            /* UCS2 -> HEX */
            num_chars = num_bytes = atc_convert_word_to_hex((uint16_t *)src_p, num_bytes, dest_p);
            break;

        default:
            ATC_LOG_E("%s->%s: Unsupported string conversion!",
                      atc_get_charset_name(src_charset), atc_get_charset_name(dest_charset));
            num_chars = num_bytes = 0;
            break;
        }

        break;

    case ATC_CHARSET_GSM7:

        switch (src_charset) {
        case ATC_CHARSET_UTF8:
            /* UTF-8 -> UCS2 -> GSM8 -> GSM7 */
            num_chars = atc_get_strnlen_chars(src_charset, src_p, num_bytes);
            ucs2_p = (uint16_t *)calloc(sizeof(uint16_t), num_chars + 1);

            if (NULL == ucs2_p) {
                ATC_LOG_E("%s->%s: Memory allocation of %d bytes failed!",
                          atc_get_charset_name(src_charset), atc_get_charset_name(dest_charset),
                          (num_chars + 1) * sizeof(uint16_t));
                break;
            }

            num_chars = atc_convert_utf8_to_ucs2(src_p, num_bytes, ucs2_p);
            num_bytes = num_chars * sizeof(uint16_t);

            if (1 > num_bytes) {
                ATC_LOG_E("%s->%s: UTF-8 to UCS2 conversion error!",
                          atc_get_charset_name(src_charset), atc_get_charset_name(dest_charset));
                break;
            }

            /* NOTE: take into account that every GSM8 character may need to be escaped
             * which effectively doubles the size of the memory required for conversion.
             */
            gsm8_p = (uint8_t *)calloc(sizeof(uint8_t), (num_chars * 2) + 1);

            if (NULL == gsm8_p) {
                ATC_LOG_E("%s->%s: Memory allocation of %d bytes failed!",
                          atc_get_charset_name(src_charset), atc_get_charset_name(dest_charset),
                          (num_chars + 1) * sizeof(uint8_t));
                break;
            }

            num_chars = num_bytes = atc_convert_ucs2_to_gsm8(ucs2_p, num_bytes, gsm8_p);

            if (1 > num_bytes) {
                ATC_LOG_E("%s->%s: UCS2 to GSM8 conversion error!",
                          atc_get_charset_name(src_charset), atc_get_charset_name(dest_charset));
                break;
            }

            num_chars = atc_convert_gsm8_to_gsm7(gsm8_p, num_bytes, dest_p);
            num_bytes = ((num_chars * 7) + 6) / 8;
            break;

        case ATC_CHARSET_HEX:
            /* HEX -> GSM8 -> GSM7 */
            num_chars = num_bytes / 2; /* Two HEX characters needed for one 1-byte GSM8 character */
            gsm8_p = (uint8_t *)calloc(sizeof(uint8_t), num_chars + 1);

            if (NULL == gsm8_p) {
                ATC_LOG_E("%s->%s: Memory allocation of %d bytes failed!",
                          atc_get_charset_name(src_charset), atc_get_charset_name(dest_charset),
                          (num_chars + 1) * sizeof(uint8_t));
                break;
            }

            num_chars = num_bytes = atc_convert_hex_to_byte(src_p, num_bytes, gsm8_p);

            if (1 > num_bytes) {
                ATC_LOG_E("%s->%s: HEX to GSM8 conversion error!",
                          atc_get_charset_name(src_charset), atc_get_charset_name(dest_charset));
                break;
            }

            num_chars = atc_convert_gsm8_to_gsm7(gsm8_p, num_bytes, dest_p);
            num_bytes = ((num_chars * 7) + 6) / 8;
            break;

        case ATC_CHARSET_GSM7:
            /* GSM7 -> GSM7 */
            num_bytes = atc_copy_in_to_out(src_p, num_bytes, dest_p, 0);
            num_chars = (num_bytes * 8) / 7;
            break;

        case ATC_CHARSET_GSM8:
            /* GSM8 -> GSM7 */
            num_chars = atc_convert_gsm8_to_gsm7(src_p, num_bytes, dest_p);
            num_bytes = ((num_chars * 7) + 6) / 8;
            break;

        default:
            ATC_LOG_E("%s->%s: Unsupported string conversion!",
                      atc_get_charset_name(src_charset), atc_get_charset_name(dest_charset));
            num_chars = num_bytes = 0;
            break;
        }

        break;

    case ATC_CHARSET_GSM8:

        switch (src_charset) {
        case ATC_CHARSET_UTF8:
            /* UTF-8 -> UCS2 -> GSM8 */
            num_chars = atc_get_strnlen_chars(src_charset, src_p, num_bytes);
            ucs2_p = (uint16_t *)calloc(sizeof(uint16_t), num_chars + 1);

            if (NULL == ucs2_p) {
                ATC_LOG_E("%s->%s: Memory allocation of %d bytes failed!",
                          atc_get_charset_name(src_charset), atc_get_charset_name(dest_charset),
                          (num_chars + 1) * sizeof(uint16_t));
                break;
            }

            num_chars = atc_convert_utf8_to_ucs2(src_p, num_bytes, ucs2_p);
            num_bytes = num_chars * sizeof(uint16_t);

            if (1 > num_bytes) {
                ATC_LOG_E("%s->%s: UTF-8 to UCS2 conversion error!",
                          atc_get_charset_name(src_charset), atc_get_charset_name(dest_charset));
            }

            num_chars = num_bytes = atc_convert_ucs2_to_gsm8(ucs2_p, num_bytes, dest_p);
            break;

        case ATC_CHARSET_HEX:
            /* HEX -> GSM8 */
            num_chars = num_bytes = atc_convert_hex_to_byte(src_p, num_bytes, dest_p);
            break;

        case ATC_CHARSET_GSM7:
            /* GSM7 -> GSM8 */
            num_chars = num_bytes = atc_convert_gsm7_to_gsm8(src_p, num_bytes, dest_p);
            break;

        case ATC_CHARSET_GSM8:
            /* GSM8 -> GSM8 */
            num_chars = num_bytes = atc_copy_in_to_out(src_p, num_bytes, dest_p, 0);
            break;

        default:
            ATC_LOG_E("%s->%s: Unsupported string conversion!",
                      atc_get_charset_name(src_charset), atc_get_charset_name(dest_charset));
            num_chars = num_bytes = 0;
            break;
        }

        break;

    case ATC_CHARSET_UCS2:

        switch (src_charset) {
        case ATC_CHARSET_UTF8:
            /* UTF-8 -> UCS2 */
            num_chars = atc_convert_utf8_to_ucs2(src_p, num_bytes, (uint16_t *)dest_p);
            num_bytes = num_chars * sizeof(uint16_t);
            break;

        case ATC_CHARSET_HEX:
            /* HEX -> UCS2 */
            num_chars = atc_convert_hex_to_word(src_p, num_bytes, (uint16_t *)dest_p);
            num_bytes = num_chars * sizeof(uint16_t);
            break;

        case ATC_CHARSET_GSM7:
            /* GSM7 -> GSM8 -> UCS2 */
            num_chars = num_bytes * 8 / 7;
            gsm8_p = (uint8_t *)calloc(sizeof(uint8_t), num_chars + 1);

            if (NULL == gsm8_p) {
                ATC_LOG_E("%s->%s: Memory allocation of %d bytes failed!",
                          atc_get_charset_name(src_charset), atc_get_charset_name(dest_charset),
                          (num_chars + 1) * sizeof(uint8_t));
                break;
            }

            num_chars = num_bytes = atc_convert_gsm7_to_gsm8(src_p, num_bytes, gsm8_p);

            if (1 > num_bytes) {
                ATC_LOG_E("%s->%s: GSM7 to GSM8 conversion error!",
                          atc_get_charset_name(src_charset), atc_get_charset_name(dest_charset));
            }

            num_chars = atc_convert_gsm8_to_ucs2(gsm8_p, num_bytes, (uint16_t *)dest_p);
            num_bytes = num_chars * sizeof(uint16_t);
            break;

        case ATC_CHARSET_GSM8:
            /* GSM8 -> UCS2 */
            num_chars = atc_convert_gsm8_to_ucs2(src_p, num_bytes, (uint16_t *)dest_p);
            num_bytes = num_chars * sizeof(uint16_t);
            break;

        case ATC_CHARSET_UCS2:
            /* UCS2 -> UCS2 */
            num_bytes = atc_copy_in_to_out(src_p, num_bytes, dest_p, sizeof(uint16_t));
            num_chars = num_bytes / sizeof(uint16_t);
            break;

        default:
            ATC_LOG_E("%s->%s: Unsupported string conversion!",
                      atc_get_charset_name(src_charset), atc_get_charset_name(dest_charset));
            num_chars = num_bytes = 0;
            break;
        }

        break;

    case ATC_CHARSET_UNDEF_BINARY:

        switch (src_charset) {
        case ATC_CHARSET_HEX:
            /* HEX -> BINARY */
            num_chars = num_bytes = atc_convert_hex_to_byte(src_p, num_bytes, dest_p);
            break;

        case ATC_CHARSET_UNDEF_BINARY:
            /* BINARY -> BINARY */
            num_chars = num_bytes = atc_copy_in_to_out(src_p, num_bytes, dest_p, sizeof(uint8_t));
            break;

        default:
            ATC_LOG_E("%s->%s: Unsupported string conversion!",
                      atc_get_charset_name(src_charset), atc_get_charset_name(dest_charset));
            num_chars = num_bytes = 0;
            break;
        }

        break;

    default:
        ATC_LOG_E("Unsupported string conversion, %s -> %s !",
                  atc_get_charset_name(src_charset), atc_get_charset_name(dest_charset));
        num_chars = num_bytes = 0;
        break;
    }

    if (NULL != dest_byte_length_p) {
        *dest_byte_length_p = num_bytes;
    }

error:
    free(gsm8_p);
    free(ucs2_p);

    return num_chars;
}


/*
 * Goes through the input data and identifies text strings. If the strings includes
 * quotes, '"', then these will be escaped, i.e. replaced with '\"'.
 */
void atc_escape_quotes(uint8_t **src_pp, uint16_t src_length)
{
    uint8_t escape = ATC_ESCAPE_CHARACTER;
    uint8_t quote = '\"';
    uint8_t *new_src_p;
    uint8_t *read_p;
    bool inside_quote;
    int number_of_quotes;
    int new_pos;
    int pos;

    /*
     * 2-pass loop:
     *   1) Count number of unescaped double-quotes
     *   2) Insert escape character in front of unescaped double-quotes
     */
    new_src_p = NULL;

loop:

    for (read_p = *src_pp, pos = new_pos = number_of_quotes = 0, inside_quote = false;
            *read_p != '\0' && pos < src_length;
            read_p++, pos++) {

        if (quote == *read_p) {
            if (!inside_quote) {
                inside_quote = atc_start_of_string(read_p, pos);
            } else {
                if (atc_end_of_string(read_p, src_length - (pos + 1))) {
                    inside_quote = false;
                } else {
                    /* Test for preceding escape-character, except in left-most position */
                    if (0 == pos || escape != *(read_p - 1)) {

                        /* Need preceding escape character */
                        number_of_quotes++;

                        if (NULL != new_src_p) {
                            /* Insert escape character */
                            new_src_p[new_pos++] = escape;
                        }

                    }
                }
            }
        }

        if (NULL != new_src_p) {
            new_src_p[new_pos++] = *read_p;
        }
    }

    /* If unescaped double-quotes found, allocate new string, and run loop again. */
    if (NULL == new_src_p) {

        if (0 < number_of_quotes) {

            /* Allocate new string */
            new_src_p = calloc(1, src_length + number_of_quotes + 1);

            if (NULL == new_src_p) {
                ATC_LOG_E("Memory allocation error!");
                goto exit;
            }

            /* Jump to perform 2:nd pass */
            goto loop;
        }

    } else {

        /* Replace input string with new string */
        free(*src_pp);
        *src_pp = new_src_p;
    }

exit:
    return;
}
