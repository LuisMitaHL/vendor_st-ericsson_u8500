/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef EXE_PSCC_REQUESTS_NAP_H
#define EXE_PSCC_REQUESTS_NAP_H 1

#include "exe_request_record.h"
#include "libstecom.h"
#include "mpl_config.h"
#include "pscc_msg.h"
#include "stecom_i.h"

/* Connect NAP */
exe_pscc_result_t pscc_request_nap_send_sterc_create(exe_request_record_t *record_p, int cid, char *dns_server_p, char *wan_device_p);
exe_pscc_result_t pscc_request_nap_send_sterc_create_response(exe_request_record_t *record_p);

/* Disconnect NAP */
exe_pscc_result_t pscc_request_nap_send_sterc_destroy(exe_request_record_t *record_p, int sterc_handle);

/* Get NAP status */
exe_pscc_result_t pscc_request_get_nap_status_send_sterc_get(exe_request_record_t *record_p, int sterc_handle);
exe_pscc_result_t pscc_request_get_nap_status_send_sterc_get_response(exe_request_record_t *record_p);

#endif

