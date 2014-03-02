/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef ATC_ANCHOR_H_
#define ATC_ANCHOR_H_

#include <stdbool.h>
#include <unistd.h>

#define MAX_NBR_CONNECTIONS (8)

#define DEBUG_CONNECTION (MAX_NBR_CONNECTIONS)

#define SERIAL_DEVICE_MAX_NUMBER (4)
#define SERIAL_DEVICE_MAX_LENGTH (15)

typedef enum {
    ANC_POINT_NONE, ANC_POINT_AT, ANC_POINT_DBMX, ANC_POINT_DUN
} anchor_point_t;

typedef enum {
    ANC_TYPE_NONE, ANC_TYPE_DEBUG, ANC_TYPE_SERIAL
} anchor_type_t;

typedef struct {
    anchor_point_t connected_to;
    anchor_type_t type;
    int fdr;
    int fdw;
    pid_t pid;
    char device_name[SERIAL_DEVICE_MAX_LENGTH + 1];
    long baud;
} anchor_connection_t;

void anc_init(void);

bool anc_handover(anchor_connection_t *conn, anchor_point_t to);

anchor_connection_t *anc_get_connections(void);

anchor_connection_t *anc_get_conn_by_fd(int fd);

int anc_connect(char *name, long baud);
bool anc_disconnect(anchor_connection_t *conn);

/* Some functions for test until we have
 * a working USB connection */
int test_connect(char *name);
int test_disconnect(char *name);

#endif /* ATC_ANCHOR_H_ */
