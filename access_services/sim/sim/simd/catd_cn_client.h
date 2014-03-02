/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef catd_cn_client_h
#define catd_cn_client_h 1

#include "cn_client.h"

/**
 * Opens a CN client session.
 */
void cn_client_open_session();

/**
 * Do service specific clean up when closing session.
 */
void cn_client_close_session(void);

/**
 * Returns CN client if initialized, otherwise start it. NULL on error.
 */
cn_context_t *cn_get_client(void);

/**
 * Returns CN client if initialized, NULL if not
 */
cn_context_t *cn_get_started_client(void);

/**
 * Function used by the uiccd thread to indicate
 * that the CN event shall be sent to the uiccd message queue
 */
void cn_uiccd_subscribe(uint8_t subscribe);

#endif
