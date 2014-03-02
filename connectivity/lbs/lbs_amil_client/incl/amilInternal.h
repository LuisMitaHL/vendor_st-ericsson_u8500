/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) ST-Ericsson SA 2009. All rights reserved.
 *  This code is ST-Ericsson proprietary and confidential.
 *  Any use of the code for whatever purpose is subject to
 *  specific written permission of ST-Ericsson.
 *
 *  Author: Janani Murthy
 *  Email : janani.murthy@stericsson.com
 *****************************************************************************/

 /** Netlnk status enum */

#ifndef AMIL_INTERNAL_H
#define AMIL_INTERNAL_H

#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include "shm_netlnk.h"
#include "agpsosa.h"

#include "amilExtern.h"


#undef  LOG_TAG
#define LOG_TAG "AMIL"

#define INF       ALOGI
#define ERR       ALOGE
#define DBG       ALOGD

#define K_AMIL_MAX_DBCLIENTS                        4
#define K_AMIL_BUFFER_SIZE                          1024
#define K_AMIL_MAX_SOCKET_CREATE_ATTEMPTS           4
#define K_AMIL_SOCKET_SELECT_TIMEOUT_SEC            3
#define K_AMIL_SLEEP_BEFORE_MODEM_QUERY_MSEC        3000
#define K_AMIL_SELECT_TIMEOUT_CLIENT_NOT_INIT_SEC   5



/*TODO : Finalise the value */
#define K_NETLINK_SOCKET_CREATE_SLEEP_MS            2000

#define MC_NETLINK_MAX(A, B) (A)>(B)?(A):(B)

typedef enum {
    K_AMIL_CLIENT_INITIALIZED,
    K_AMIL_CLIENT_DEINITIALIZED
} t_AmilClientStatus;

typedef enum {
     K_NETLINK_MODEM_READY,
     K_NETLINK_MODEM_NOT_READY
}t_AmilModemState;

typedef struct {
    bool (* init)  (int8_t);
    void (* deinit)(int8_t);
    t_AmilClientStatus ClientState;
}t_AmilClientsBlock;

typedef struct t_selectorBlock {
    struct t_selectorBlock *next;
    int32_t fd;
    t_AmilClientsCallback pp_callback;
    int8_t handle;
} t_AmilClientsSelectorBlock;




void Amil1_02Main(void);
int32_t Amil1_03RecvNetlinkData(int32_t vp_netlinkFD);
void Amil1_04InitializeAllClients();
void Amil1_05DeInitializeAllClients();
void Amil1_08AddFileDescriptor( int32_t vp_fd ,t_AmilClientsCallback pp_callback, int8_t vp_handle);
void Amil1_09RemoveFileDescriptor( int32_t vp_fd );
int32_t Amil1_10GetMaxFDOfClients(fd_set *set);
struct timeval* Amil1_11GetTimeOut( void );
bool Amil1_12CheckModemReady(int32_t vl_NetlinkFD);
void Amil1_13DeInitializeClient(int8_t vp_handle);


#endif /*AMIL_INTERNAL_H*/

