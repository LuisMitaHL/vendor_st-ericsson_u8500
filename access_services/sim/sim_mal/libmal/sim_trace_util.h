/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : sim_trace_util.h
 * Description     : Utility trace
 *
 * Author          : Steve Critchlow <steve.critchlow@stericsson.com>
 *
 */

#ifndef SIM_TRACE_UTIL_H_
#define SIM_TRACE_UTIL_H_

void sim_trace_print_data(uint8_t* buff, size_t length, void (*log_f)(sim_logging_t, const char *, ...));

const char *sim_trace_lookup_service_type ( uint8_t service_type );

const char *sim_trace_lookup_card_type ( uint8_t card_type );

const char *sim_trace_lookup_request_opcode ( uint32_t uicc_MsgID );

const char *sim_trace_lookup_refresh_type ( uint8_t refresh_type );

extern sim_logging_t sim_trace_debug_level;

#endif /* SIM_TRACE_UTIL_H_ */
