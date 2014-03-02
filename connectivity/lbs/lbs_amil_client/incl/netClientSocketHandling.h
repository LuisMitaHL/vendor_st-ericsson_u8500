/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) ST-Ericsson SA 2009. All rights reserved.
 *  This code is ST-Ericsson proprietary and confidential.
 *  Any use of the code for whatever purpose is subject to
 *  specific written permission of ST-Ericsson.
 *
 *  Author: Balaji Natakala
 *  Email : balaji.natakala@stericsson.com
 *****************************************************************************/

#ifndef NET_CLIENT_SOCKET_HANDLING_H
#define NET_CLIENT_SOCKET_HANDLING_H



// Net Server Header files
#include "cn_message_types.h"
#include "cn_data_types.h"

#include "netClient.h"
#include "gns.h"
#include "gnsSUPL_Typedefs.h"

s_gnsSUPL_MobileInfo vl_MobileInfo;
cn_bool_t vl_GetNeighbourCellsReporting;


/**
 * @brief Structure used to store information needed to handle a timer. The timer is used when
 *        concatenated CN messages are received.
 *
 * @param v_TimerSet         TRUE if a timer has been set, but it has not yet been cleared.
 *
 * @param v_TimerLengthSecs Length, in seconds, that the timer was specified for.
 *
 * @param timer_stored_now  Stores time at which the timer was initialised. Used to track
 *                          elapsed time while timer is active.
 *
 * @param v_SelectTimeout    Stores the timeout value which is passed to select(). Note that
 *                          select() may update this value so its value should not be relied
 *                          upon after select() has been called.
 */
typedef struct {
    bool v_TimerSet;
    long v_TimerLengthSecs;
    struct timeval v_TimerCreatedNow;
    struct timeval v_SelectTimeout;
} t_netClient_TimeoutInfo;

#define K_NETCLIENT_UNDEFINED_SOCKET           (-1)
#define K_NETCLIENT_UICC_INIT_THREAD_ID_VALUE  (0)

bool netClient1_1Init(int8_t vp_handle);
static void *netClient1_2SocketHandlerThread(void *threadid);
static void netClient1_3CreateCallNetSession(int8_t vp_handle);
static void netClient1_4HandleSocketRcv(void);
bool netClient1_5HandleCnSocketRead(void* pp_FileDes, int32_t vp_SelectReturn, int8_t vp_handle);
static void netClient1_6HandleRespEvntRegistration(cn_registration_info_t * registration_info_p);
static void netClient1_7GetMccandMnc(char *mcc_mnc, uint16_t *val_Mcc, uint16_t *val_Mnc);
static int netClient1_8GetHighestSocketValue (void) ;
static struct timeval *netClient1_9GetTimeoutForSelect(void);
static t_netClient_TimeoutInfo *netClient1_10GetTimeoutInfo(void);
void netClient1_11SelectTimeoutSet(const long vp_Seconds);
void netClient1_12Deinit(int8_t vp_handle);
static void netClient1_13ShutdownCallNetSession(int8_t vp_handle);
void netClient1_14HandleECI(cn_neighbour_cells_info_t *neighboring_cell_p);
void netClient1_15HandleReportingStatus(cn_bool_t bool_type_p);


#endif /*NET_CLIENT_SOCKET_HANDLING_H*/

