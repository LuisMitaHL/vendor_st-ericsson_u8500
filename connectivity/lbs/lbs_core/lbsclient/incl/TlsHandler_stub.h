#ifndef TLS_HANDLER_STUB_H
#define TLS_HANDLER_STUB_H


#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <errno.h>
#include <pthread.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <math.h>
#include <time.h>
#ifndef ARM_LINUX_AGPS_FTR
#include "android_log.h"
#include <cutils/sockets.h>
#include <cutils/properties.h>
#endif

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"

#include "netdb.h"



#define TRUE    1
#define FALSE   0


#define  INADDR_SERVER_HOST    0x7f000001



#define TLS_MAX_CONNECTIONS    5
#define TLS_READ_BYTE_CHUNK    256


static int  dnsResolveServer;
static int  dnsResolveClient;
static int  dnsResolvePort;
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

typedef enum
{
    K_TLS_STATE_UNUSED,
    K_TLS_STATE_WAIT_SERVER_CONNECTION,
    K_TLS_STATE_HANDSHAKE_IN_PROGRESS,
    K_TLS_STATE_HANDSHAKE_DONE,
    K_TLS_STATE_SHUTDOWN_IN_PROGRESS,
    K_TLS_STATE_DNS_RESOLUTION_IN_PROGRESS,
} e_TlsState;

typedef enum
{
    K_TLS_DNS_SUCCESS,
    K_TLS_DNS_ERROR
} e_TlsDnsReport;

    typedef struct
    {
        int                socket;
        e_TlsState        state;
        unsigned int    handle;
        BIO                *p_BIO;
        SSL                *p_SSL;
        t_TlsServer        server;
        char            *p_Data;
        int                v_DataLength;
        int             isTls;
    } t_TlsConnection;


typedef struct
{
    e_TlsDnsReport      v_Report;
    int                 v_Handle;
    struct sockaddr_in  v_Addr;
} t_TlsDnr;

typedef struct
{
    t_TlsServer v_Server;
    int            v_Handle;
} t_ResolveDnrReq;


typedef struct
{
    int v_Enum;
    char* p_EnumAsString;
}t_tlsEnumAsString;




static SSL_CTX *gp_SSL_CTX ;


#define ADD_ENUM_AS_STRING( enum )  { enum , #enum }

#define SOCKADDR_IN struct sockaddr_in
#define HOSTENT     struct hostent

#define DEFAULT_CERT_PATH   "/etc/cacert.txt"

#if 1
#define DEFAULT_CIPHER_LIST     "DES-CBC3-SHA:AES128-SHA:NULL-SHA"
#else
#define DEFAULT_CIPHER_LIST     "AES128-SHA:NULL-SHA"
#endif

static char *vg_NetworkInterface = NULL;

static t_TlsConnection g_TlsConnection[TLS_MAX_CONNECTIONS];


int TlsHandleReadEvent( const fd_set *p_Set );
int TlsHandleWriteEvent( const fd_set *p_Set );
int TlsHandleErrorEvent( const fd_set *p_Set );
int TlsPopulateWriteSelect( fd_set *p_Set );
int TlsPopulateReadSelect( fd_set *p_Set );
int TlsPopulateErrorSelect( fd_set *p_Set );

void TlsSetNetworkInterface( char *ifname );

e_TlsError TlsDisconnect( unsigned int v_Handle );
e_TlsError TlsWrite( unsigned int v_Handle , char* p_Data , int v_Length );
e_TlsError TlsConnect( unsigned int handle , t_TlsServer *p_Server );
int TlsInit( char* cacertPath );
e_TlsError TlsConnectUnsecure( unsigned int handle , t_TlsServer *p_Server );


void TlsDeinit();

// Implemented by calling party
void TEST_SUPL_send_event(void);

void TlsReadInd(unsigned int handle , char *p_Data , int v_Length );
void TlsConnectCnf( unsigned int handle );
void TlsConnectErr( unsigned int handle , int error );
void TlsDisconnectCnf( unsigned int handle );
//void TlsDisconnectInd( unsigned int handle );




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

