/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef ATC_STRING_H_
#define ATC_STRING_H_

#include <stdint.h>
#include "atc_parser.h"

size_t atc_string_convert(atc_charset_t src_charset, uint8_t *src_p, size_t src_byte_length,
                          atc_charset_t dest_charset, uint8_t *dest_p, size_t *dest_byte_length_p);

void atc_escape_quotes(uint8_t **src_pp, uint16_t src_length);


#endif /* ATC_STRING_H_ */
