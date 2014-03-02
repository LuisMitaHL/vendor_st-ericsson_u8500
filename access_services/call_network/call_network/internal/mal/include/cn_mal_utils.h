/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __cn_mal_utils_h__
#define __cn_mal_utils_h__ (1)

#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>

#ifdef USE_MAL_CS
#include "mal_call.h"
#include "mal_ss.h"
#endif /* USE_MAL_CS */

#ifdef USE_MAL_GSS
#include "mal_gss.h"
#endif /* USE_MAL_GSS */

#ifdef USE_MAL_MCE
#include "mal_mce.h"
#endif /* USE_MAL_MCE */

#ifdef USE_MAL_NET
#include "mal_net.h"
#endif /* USE_MAL_NET */

#include "mal_utils.h"

#include "cn_client.h"
#include "cn_log.h"

#include "cnsocket.h"
#include "cnserver.h"

#include "cn_data_types.h"
#include "cn_log.h"
#include "event_handling.h"
#include "request_handling.h"
#include "cn_ss_command_handling.h"
#include "cn_mal_assertions.h" /* do NOT remove this inclusion! */

#ifdef USE_MAL_CS
typedef enum {
    SS_MAL_SET_BARRING,
    SS_MAL_QUERY_BARRING,
    SS_MAL_SET_FORWARDING,
    SS_MAL_QUERY_FORWARDING,
    SS_MAL_SET_WAITING,
    SS_MAL_QUERY_WAITING,
    SS_MAL_SET_SIGNALING,       /* TODO: NOT YET SUPPORTED.*/
    SS_MAL_QUERY_SIGNALING,     /* TODO: NOT YET SUPPORTED.*/
    SS_MAL_QUERY_CLIR,
    SS_MAL_QUERY_CLIP,
    SS_MAL_QUERY_COLR,
    SS_MAL_QUERY_CNAP,
    SS_MAL_TRANSFER             /* TODO: NOT YET SUPPORTED*/
} cn_ss_mal_command_t;

#endif /* USE_MAL_CS */

#ifdef USE_MAL_CS
void util_call_cause(exit_cause *mal_call_cause_p, cn_exit_cause_t *call_cause_p);
void util_call_context(mal_call_context *mal_call_context_p, cn_call_context_t *call_context_p);
void util_call_failure_response_cause(void *data_p, request_record_t *record_p, cn_message_type_t response_type, const char *calling_function);

int util_get_call_by_filter(cn_call_state_filter_t *filter_p, cn_call_context_t *call_context_p);
void util_convert_ss_error_type(int mal_error, mal_ss_error_type_value *ss_error, cn_ss_error_t *ss_error_p);
#endif

#ifdef USE_MAL_FTD
void set_empage_urc_issue(bool state);
bool get_empage_urc_issue(void);
void set_empage_write_tofile(bool state);
bool get_empage_write_tofile(void);
void set_empage_seqnr(int value);
int get_empage_seqnr(void);
void set_empage_activepage(int value);
int  get_empage_activepage(void);
bool get_empage_activation_in_progress(void);
void set_empage_activation_in_progress(bool state);
bool get_empage_deactivation_in_progress(void);
void set_empage_deactivation_in_progress(bool state);
#endif /* USE_MAL_FTD */


cn_sint32_t cn_util_convert_ss_service_class_to_cn_class(cn_sint32_t ss_service_class);
int util_copy_neighbour_cells_info_data(cn_neighbour_cells_info_t *cn_neighbour_cells_info, mal_net_neighbour_cells_info *mal_neighbour_cells_info,cn_uint32_t *size);
int cn_util_convert_cn_ss_type_to_mal_ss_command(cn_ss_string_compounds_t *ss_data_p, cn_ss_mal_command_t *command_p);
int cn_util_convert_cn_ss_type_to_mal_ss_facility(cn_ss_string_compounds_t *ss_data_p, char **facility_pp);
int cn_util_convert_cn_procedure_type_to_mal_mode(cn_ss_string_compounds_t *ss_data_p, cn_sint32_t *mode_p);
int cn_util_convert_cn_ss_type_to_mal_cf_reason(cn_ss_string_compounds_t *ss_data_p, cn_call_forwarding_reason_t *reason_p);
int convert_toa(cn_ss_ton_t toa, cn_ss_npi_t npi);
cn_bool_t util_convert_cn_trigger_level_to_mal(cn_reg_status_trigger_level_t trigger_level, mal_net_reg_status_set_mode *reg_status_mode_p);
cn_bool_t cn_util_path_present_in_fs(char *path_p);
void cn_util_create_empty_file_in_fs(char *path_p);

#endif /* __cn_mal_utils_h__ */
