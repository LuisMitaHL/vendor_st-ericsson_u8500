/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef ATC_DEBUG_H_
#define ATC_DEBUG_H_

void print_hex(unsigned char *pointer, unsigned int len);
void at_debug_print_conn(anchor_connection_t *conn_p);
bool at_debug_mkfifo(void);
int at_debug_init(void);

#endif /* ATC_DEBUG_H_ */
