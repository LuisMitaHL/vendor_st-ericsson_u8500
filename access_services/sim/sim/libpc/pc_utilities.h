/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : pc_utilities.h
 * Description     : common functions declaration used by libpc.
 *
 * Author          : Haiyuan Bu <haiyuan.bu@stericsson.com>
 *
 */

#ifndef __pc_utilities_h__
#define __pc_utilities_h__ (1)

#include "apdu.h"
#include "cn_message_types.h"

/**
 * Macro that return the smaller value of two values.
 *
 * This version of the macro is double-evaluation safe, e.g.
 * calls similar to MIN(++a, b++) are safe and behave as expected.
 */
#ifdef MIN
#undef MIN
#endif
#define MIN(x, y) ({ \
            __typeof__ (x) _x = (x); \
            __typeof__ (y) _y = (y); \
            _x < _y ? _x : _y; })

/**
 * Macro that return the larger value of two values.
 *
 * This version of the macro is double-evaluation safe, e.g.
 * calls similar to MAX(++a, b++) are safe and behave as expected.
 */
#ifdef MAX
#undef MAX
#endif
#define MAX(x, y) ({ \
            __typeof__ (x) _x = (x); \
            __typeof__ (y) _y = (y); \
            _x > _y ? _x : _y; })


void pc_send_terminal_response_general(ste_parsed_apdu_t         * parsed_apdu_p, 
                                       ste_apdu_general_result_t   general_result);

void pc_send_terminal_response_with_add_info(ste_parsed_apdu_t          * parsed_apdu_p,
                                             ste_apdu_general_result_t    general_result,
                                             ste_apdu_additional_info_t * add_info_p);

void pc_send_terminal_response_error(ste_sat_apdu_error_t  error_info, 
                                     ste_apdu_t           *apdu_p);

ste_apdu_location_info_t *create_local_info_from_cell_info(cn_cell_info_t *cell_info_p);

/*
 * This routine encodes the 8 bit buffer to GSM 7 bit default message
 * param [In_p]              Input buffer
 * param [Out_p]             Output buffer
 * param [NoOfOctetsToPack]  No of octets to pack
 * return                    No of octets packed
 */
uint8_t convert_GSM8bit_2_GSM7bit(uint8_t *In_p, uint8_t *Out_p, uint8_t NoOfOctetsToPack);
int convert_bcd2ascii(uint8_t* in, size_t size, char* out, size_t maxlen);
#endif
