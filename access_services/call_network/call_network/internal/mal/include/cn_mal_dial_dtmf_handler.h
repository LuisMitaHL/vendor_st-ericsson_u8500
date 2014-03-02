/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Function related to handling DTMF sending when dialling.
 */

#ifndef __cn_mal_dial_dtmf_handler_h__
#define __cn_mal_dial_dtmf_handler_h__

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

#include "cn_macros.h"
#include "event_handling.h"
#include "request_handling.h"
#include "cn_mal_assertions.h" /* do NOT remove this inclusion! */

#ifdef USE_MAL_CS

/**
 * Stores and associates a cn_request_dtmf_string_t instance with the specific call id.
 *
 * The associated cn_request_dtmf_string_t instance can later be retrieved using the
 * mddh_fetch_call_id_associated_dtmf() function.
 *
 * @param call_id The call id to associate with a DTMF string.
 * @param dtmf_p  A pointer to the DTMF instance that contains the DTMF string.
 *
 * @return Zero on success, non-zero on failure.
 */
int mddh_associate_dtmf_with_call_id(cn_sint32_t call_id, cn_request_dtmf_string_t* dtmf_p);

/**
 * Finds and returns the cn_request_dtmf_string_t instance associated with the given call id.
 * The cn_request_dtmf_string_t instance is disassociated and removed from the storage.
 *
 * Note: It is up to the caller to free the memory when appropriate.
 *
 * @param call_id The call id to associate with a DTMF string.
 *
 * @return Non-NULL on success, a pointer to the DTMF instance that contains the DTMF string.
 *         NULL on failure.
 */
cn_request_dtmf_string_t* mddh_fetch_call_id_associated_dtmf(cn_sint32_t call_id);


/**
 * Extracts & removes DTMF parts from dialing string.
 *
 * Note: Allocates memory. It is up to the caller to free the memory when appropriate.
 *
 * @param dial_p A pointer to the cn_request_dial_t instace that contains the dialing string.
 *
 * @return Extracted DTMF parts, or NULL if no DTMF present.
 */
cn_request_dtmf_string_t* mddh_util_extract_and_remove_dtmf(cn_request_dial_t* dial_p);

/**
 * State change handler for automatic DTMF sending after dial
 *
 * @param call_id The call id for which there is a call state change.
 * @param state   The new call state.
 *
 */
void mddh_dtmf_handle_call_state_change(cn_sint32_t call_id, cn_call_state_t state);

#endif /* USE_MAL_CS */

#endif /* __cn_mal_dial_dtmf_handler_h__ */
