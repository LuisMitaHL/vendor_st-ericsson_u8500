/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) ST-Ericsson SA 2009. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 *  specific written permission of ST-Ericsson.
 *
 *  Author: Anshuman Pandey
 *  Email : anshuman.pandey@stericsson.com
 *****************************************************************************/

#ifndef TLS_HANDLER_H
#define TLS_HANDLER_H

typedef enum
{
    K_TLS_NO_ERROR,
    K_TLS_WARN_WAIT_SERVER_CONNECTION,
    K_TLS_ERROR_NO_SOCKET,
    K_TLS_ERROR_NO_CONNECT,
    K_TLS_ERROR_BIO_INIT,
    K_TLS_ERROR_SSL_INIT,
    K_TLS_ERROR_SSL_HANDSHAKE,
    K_TLS_ERROR_DNS_RESOLVE_FAILURE
} e_TlsError;

typedef enum
{
    K_TLS_IPV4_ADDRESS,
    K_TLS_IPV6_ADDRESS,
    K_TLS_FQDN_ADDRESS
} e_TlsServerAddrType;

typedef struct
{
    char                a_Addr[256];
    e_TlsServerAddrType    v_Type;
    unsigned short      v_Port;
} t_TlsServer;

int TlsHandleReadEvent( const fd_set *p_Set );
int TlsHandleWriteEvent( const fd_set *p_Set );
int TlsHandleErrorEvent( const fd_set *p_Set );
int TlsPopulateWriteSelect( fd_set *p_Set );
int TlsPopulateReadSelect( fd_set *p_Set );
int TlsPopulateErrorSelect( fd_set *p_Set );

e_TlsError TlsConnect( unsigned int handle , t_TlsServer *p_Server );
e_TlsError TlsConnectUnsecure( unsigned int handle , t_TlsServer *p_Server );
e_TlsError TlsDisconnect( unsigned int handle );
e_TlsError TlsWrite( unsigned int v_Handle , char* p_Data , int v_Length );
int TlsInit( char* cacertPath );
void TlsDeinit();

// Implemented by calling party
void TlsReadInd(unsigned int handle , char *p_Data , int v_Length );
void TlsConnectCnf( unsigned int handle );
void TlsConnectErr( unsigned int handle , int error );
void TlsDisconnectCnf( unsigned int handle );
void TlsDisconnectInd( unsigned int handle );

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

#endif