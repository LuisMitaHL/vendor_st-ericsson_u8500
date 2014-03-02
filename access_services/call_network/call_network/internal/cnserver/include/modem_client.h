/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __modem_client_h__
#define __modem_client_h__ (1)

typedef struct modem_client_s modem_client_t;

modem_client_t *modem_client_open_session(void);
void modem_client_close_session(modem_client_t *modem_client_p);

#endif /* __modem_client_h__ */
