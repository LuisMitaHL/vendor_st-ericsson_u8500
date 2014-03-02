/*
* Copyright (C) ST-Ericsson 2009
*
* gpsclient.h
* Date: 17-07-2009
* Author: Anil Kumar
* Email : anil.nambiar@stericsson.com
*/


#ifndef _CLIENT_UTILS_H
#define _CLIENT_UTILS_H

#define MAX_S16    32767
#define SLEEP_TIME_MICRO_SECONDS  100000


#ifndef DEBUG_LOG_PRINT 
#ifndef ARM_LINUX_AGPS_FTR

#define DEBUG_LOG_PRINT( X )                ALOGD X
#else
#define DEBUG_LOG_PRINT( X )                printf X
#endif

#ifdef DEBUG_LOG_LEV2
#define DEBUG_LOG_PRINT_LEV2( X )   DEBUG_LOG_PRINT( X )
#else  /* DEBUG_LOG_LEV2 */ 
#define DEBUG_LOG_PRINT_LEV2( X )
#endif /* DEBUG_LOG_LEV2 */

#endif

/* this is the state of our connection to the gpsd daemon */
typedef void (* t_clientUtils_GpsStateEventHandler ) ( t_agps_Command );

typedef void (* t_clientUtils_GpsReconnectHandler ) ( void );

/* + AGPSL-2.5.BEHOLD3 */
typedef int (* t_clientUtils_SocketListenUpdate )( fd_set *p_ReadFds , fd_set *p_WriteFds ,fd_set *p_ErrorFds );
typedef int (* t_clientUtils_SocketEvent )( fd_set *p_ReadFds , fd_set *p_WriteFds ,fd_set *p_ErrorFds );
/* - AGPSL-2.5.BEHOLD3 */
typedef struct {
    int                                     init;
    int                                     fd;
    t_clientUtils_GpsStateEventHandler      callback;
    pthread_t                               thread;  
    int                                     exitThread;
    struct sockaddr_in                      s_in;
    e_agps_Status                           statusValue; 
/* + AGPSL-2.5.BEHOLD3 */
    t_clientUtils_SocketListenUpdate        callbackFdUpdate;
    t_clientUtils_SocketEvent               callbackEventUpdate;
/* - AGPSL-2.5.BEHOLD3 */
/*  Keep track to see if reconenction to lbsd has occured */
    bool                                    reconnectReq;
/*  Keep track to see if Periodic Fix has been Requested to Server */
    bool                                    fixRequested;
    t_clientUtils_GpsReconnectHandler       callbackReconnect;
} t_clientUtils_GpsState;

uint16_t clientUtils_WaitForStatusFromServer( t_clientUtils_GpsState *gpsState );
void clientUtils_StopGpsState( t_clientUtils_GpsState *gpsState );
bool clientUtils_InitGpsState( t_clientUtils_GpsState *gpsState );
bool clientUtils_InitEEState( t_clientUtils_GpsState *gpsState );
void clientUtils_SendCommandToServer( t_clientUtils_GpsState *gpsState, t_agps_Command *command );
void clientUtils_ProcessStatus( t_clientUtils_GpsState *gpsState, e_agps_Status *status );
bool clientUtils_Reconnect(t_clientUtils_GpsState *gpsState);




#endif /* #ifndef _CLIENT_UTILS_H */
