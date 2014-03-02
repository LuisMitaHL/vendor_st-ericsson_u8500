#include<TlsHandler_stub.h>
//#include "test_inf.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define  LOG_TAG  "TlsHandler_stub"


t_TlsConnection *vg_Tlsconnection = NULL;
unsigned int    vg_Server_present_Flag = FALSE;
unsigned int    vg_disconnect_Flag = FALSE;
unsigned int  vg_TestHandle = 0;
unsigned int vg_TestConnectCnf = TRUE;
unsigned int vg_TestConnectCnf_Flag = FALSE;
char *vg_Write_Buffer = NULL;
//int t_flage=FALSE;

//int t_test = FALSE;
 //TEST_SUPL_SharedContext vg_testSuplCtx;


t_tlsEnumAsString a_TlsStateEnumAsString[] =
{
    ADD_ENUM_AS_STRING(K_TLS_STATE_UNUSED),
    ADD_ENUM_AS_STRING(K_TLS_STATE_WAIT_SERVER_CONNECTION),
    ADD_ENUM_AS_STRING(K_TLS_STATE_HANDSHAKE_IN_PROGRESS),
    ADD_ENUM_AS_STRING(K_TLS_STATE_HANDSHAKE_DONE),
    ADD_ENUM_AS_STRING(K_TLS_STATE_SHUTDOWN_IN_PROGRESS),
    ADD_ENUM_AS_STRING(K_TLS_STATE_DNS_RESOLUTION_IN_PROGRESS),
    {-1,NULL}
};
#define TLS_STATE_STRING( X )    GetEnumAsString( X , a_TlsStateEnumAsString )

void Tls_openssl_info_callback(SSL *s, int where, int ret);

int TlsInit( char* cacertPath )
{
    cacertPath = cacertPath;
    DEBUG_LOG_PRINT_LEV2(("TlsInit : Entry"));


    DEBUG_LOG_PRINT_LEV2(("TlsInit : Exit"));
    return TRUE;
}

void TEST_SUPL_send_event()
{
    DEBUG_LOG_PRINT_LEV2(("Tls : Event Generated"));

    vg_Server_present_Flag = TRUE;
}


e_TlsError TlsConnect( unsigned int handle , t_TlsServer *p_Server )
{

    DEBUG_LOG_PRINT_LEV2(("TlsConnect : Entry"));
    t_TlsConnection *p_TlsConnection = malloc(sizeof(t_TlsConnection));
    t_TlsServer *pl_TlsServer=NULL;
    p_TlsConnection->handle = handle;

    vg_Tlsconnection = malloc(sizeof(t_TlsConnection));
    memcpy( &vg_Tlsconnection->server , p_Server , sizeof(*pl_TlsServer) );

        vg_TestHandle = handle ;
    TEST_SUPL_send_event();

    DEBUG_LOG_PRINT_LEV2(("TlsConnect : Exit"));
    return K_TLS_NO_ERROR ;
    
}



e_TlsError TlsDisconnect( unsigned int v_Handle )
{
    v_Handle = v_Handle;
    DEBUG_LOG_PRINT_LEV2(("TlsDisconnect"));
    vg_disconnect_Flag = TRUE;
    TEST_SUPL_send_event();
    
    return K_TLS_NO_ERROR;
}



e_TlsError TlsWrite( unsigned int v_Handle , char* p_Data , int v_Length )
{
    v_Handle = v_Handle;
    DEBUG_LOG_PRINT_LEV2(("TlsWrite : v_Length = %d Entry",v_Length));

    vg_Write_Buffer = malloc(v_Length);
    
    DEBUG_LOG_PRINT_LEV2(("TlsWrite "));
    memcpy(vg_Write_Buffer,p_Data,v_Length);
    //t_flage=TRUE;
    TEST_SUPL_send_event();
    
    DEBUG_LOG_PRINT_LEV2(("TlsWrite : Exit"));
    return K_TLS_NO_ERROR;
}



void TlsDeinit()
{
    
}


char* GetEnumAsString( int v_Enum , t_tlsEnumAsString *p_Table )
{
    int vl_Index = 0;

    if( NULL == p_Table )
    {
        return "Invalid Table";
    }

    do
    {
        if( -1 == p_Table[vl_Index].v_Enum )
        {
            /* This is the highest possible value in the table. So no other enums are present */
            /* Return error */
            return p_Table[vl_Index].p_EnumAsString;
        }
        else if( v_Enum == p_Table[vl_Index].v_Enum )
        {
            return p_Table[vl_Index].p_EnumAsString;
        }
        else
        {
            vl_Index++;
        }
    } while(1);

    return "Enum Value Not in Table";
}

void TlsTransitionState( t_TlsConnection *p_TlsConnection , e_TlsState state )
{
    DEBUG_LOG_PRINT_LEV2(("Tls State Transition Handle : %d , %s -> %s\n",
        p_TlsConnection->handle , TLS_STATE_STRING(p_TlsConnection->state) , TLS_STATE_STRING(state) ));

    p_TlsConnection->state = state;
}



t_TlsConnection* TlsGetConnFromHandle( unsigned int v_Handle )
{
    int vl_Index;

    for( vl_Index = 0; vl_Index < TLS_MAX_CONNECTIONS ; vl_Index++ )
    {
        if( g_TlsConnection[vl_Index].handle == v_Handle )
        {
            DEBUG_LOG_PRINT_LEV2(("TlsGetConnFromHandle : Found Handle %d at index %d",v_Handle , vl_Index ));
            return g_TlsConnection+vl_Index;
        }
    }

    return NULL;
}

t_TlsConnection *TlsGetFreeConnection( )
{
    int vl_Index;

    for( vl_Index = 0; vl_Index < TLS_MAX_CONNECTIONS ; vl_Index++ )
    {
        if( g_TlsConnection[vl_Index].state == K_TLS_STATE_UNUSED )
        {
            DEBUG_LOG_PRINT_LEV2(("TlsGetFreeConnection : Found at index %d\n", vl_Index ));
            return g_TlsConnection+vl_Index;
        }
    }

    return NULL;
}

void TlsRemoveConnection( t_TlsConnection *p_Conn )
{
    if( p_Conn->p_SSL )    SSL_free( p_Conn->p_SSL ); // This also releases p_BIO
    if( p_Conn->p_Data ) 
    {
        free(p_Conn->p_Data);
        p_Conn->p_Data = NULL;
    }
    if( p_Conn->socket)  close( p_Conn->socket );
    TlsTransitionState( p_Conn , K_TLS_STATE_UNUSED );
/*+LMSqc38060_2*/
    p_Conn->p_SSL = NULL;
    p_Conn->p_Data = NULL;
    p_Conn->socket = 0;
/*-LMSqc38060_2*/

}
static int TlsValidateNetworkInterface(int sock)
{
    struct ifconf iflist;
    struct ifreq *ifr;
    int           numIf,i,done;

    if( (vg_NetworkInterface == NULL) || (strlen(vg_NetworkInterface) == 0) )
    {
        DEBUG_LOG_PRINT(("TlsValidateNetworkInterface : No Network i/f set"));
        return FALSE;
    }

    if (ioctl(sock, SIOCGIFCONF, &iflist) < 0) {
         DEBUG_LOG_PRINT(("ERROR1 : ioctl(%d,SIOCGIFCONF) Failed" , sock));
         return FALSE;
    }

    if ((ifr = malloc (iflist.ifc_len)) == NULL) {
         DEBUG_LOG_PRINT(("ERROR : Malloc Failed" ));
         return FALSE;
    }
    /* Retrieve the interfaces */

    iflist.ifc_ifcu.ifcu_req = ifr;

    if(ioctl(sock, SIOCGIFCONF, &iflist) < 0)
    {
        DEBUG_LOG_PRINT(("ERROR : ioctl(%d,SIOCGIFCONF) Failed" , sock));
        free (ifr);
        ifr = NULL;
        return FALSE;
    }

    ifr     = iflist.ifc_req;
    numIf   = iflist.ifc_len / sizeof(struct ifreq);

    /* Iterate through the list of interfaces. */
    for(i = 0,done=FALSE; i < numIf && !done; i++)
    {
        struct ifreq *item = &ifr[i];

       /* Show the device name and IP address */
        DEBUG_LOG_PRINT(("%s: IP %s\n", \
               item->ifr_name,\
               inet_ntoa(((struct sockaddr_in *)&item->ifr_addr)->sin_addr)));
        if( strcmp(vg_NetworkInterface,item->ifr_name) == 0 )
        {
            done = TRUE;
        }
        else if (strcmp("pdp0",item->ifr_name) == 0)
        {
            DEBUG_LOG_PRINT(("TlsValidateNetworkInterface:found default PDP0"));
            //todo
            // set the local flags to manage pdp0 by default.
            // But now , just consider number of ifs > 0 , to have pdp0 from GpsLocationProvider.
        }
    }

    if (done == FALSE &&  numIf > 0)
    {
        strcpy(vg_NetworkInterface, "pdp0");
        DEBUG_LOG_PRINT(("TlsValidateNetworkInterface:Set default :%s,numberIFs:%d ",vg_NetworkInterface,numIf ));
        done = TRUE;
    }
    else
    {
        DEBUG_LOG_PRINT(("TlsValidateNetworkInterface:Fault. Number of IFs-Zerof" ));
        //return FALSE;
    }
    free (ifr);
    ifr = NULL;

    return done;
}


static int TlsBindSocketToInterface( int sock )
{
    int retFnVal= FALSE;

    if( TlsValidateNetworkInterface(sock) )
    {
        int retVal =-1;

        DEBUG_LOG_PRINT(("TlsBindSocketToInterface : Binding socket to %s" , vg_NetworkInterface ));

        retVal = setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, vg_NetworkInterface, strlen(vg_NetworkInterface)+1 );


        if( retVal < 0 )
        {
            DEBUG_LOG_PRINT(("ERROR : setsockopt(%d,SO_BINDTODEVICE) Failed <%d> <%s>" ,sock, errno , strerror(errno)));
        }
        else
        {
            DEBUG_LOG_PRINT(("TlsBindSocketToInterface : Success"));
            retFnVal= TRUE;
        }
    }
    else
    {
        DEBUG_LOG_PRINT(("TlsBindSocketToInterface : No Valid Network i/f found"));
    }
    return retFnVal;
}


e_TlsError TlsDoHandshake( t_TlsConnection *p_TlsConnection)
{
    int vl_Return;
    unsigned long er;

    DEBUG_LOG_PRINT_LEV2(("TlsDoHandshake : handle %d : Entry\n" , p_TlsConnection->handle));

     vl_Return = SSL_connect(p_TlsConnection->p_SSL);

     DEBUG_LOG_PRINT_LEV2(("TlsDoHandshake : vl_Return %d \n" ,vl_Return));
     if( vl_Return <= 0 )
     {
         int vl_SslError = SSL_get_error(p_TlsConnection->p_SSL,vl_Return);
         DEBUG_LOG_PRINT_LEV2(("SSL_get_error : %d\n" , vl_SslError ));
         switch(vl_SslError)
         {
         case SSL_ERROR_WANT_READ:
            vl_Return = K_TLS_NO_ERROR;
            break;
         case SSL_ERROR_WANT_WRITE:
            vl_Return = TlsDoHandshake( p_TlsConnection );
            break;
         default:
            er = ERR_get_error();
            DEBUG_LOG_PRINT_LEV2(("TlsDoHandshake : %s\n" , ERR_error_string(er,NULL) ));
            DEBUG_LOG_PRINT_LEV2(("SSL_get_error : %d\n" , vl_SslError ));
            TlsConnectErr(p_TlsConnection->handle , 0 );
            vl_Return = K_TLS_ERROR_SSL_HANDSHAKE;
            TlsRemoveConnection(p_TlsConnection);
         }
     }
     else
     {
         int ret = SSL_get_verify_result(p_TlsConnection->p_SSL);
         ret = ret;
         DEBUG_LOG_PRINT_LEV2(("TlsDoHandshake : SSL_get_verify_result : %d" , ret));
         TlsTransitionState(p_TlsConnection , K_TLS_STATE_HANDSHAKE_DONE );
         TlsConnectCnf(p_TlsConnection->handle );
         vl_Return = K_TLS_NO_ERROR;
     }

     DEBUG_LOG_PRINT_LEV2(("TlsDoHandshake : Exit\n"));
     return vl_Return;
}


e_TlsError TlsInitHandshake(t_TlsConnection *p_TlsConnection)
{
    DEBUG_LOG_PRINT_LEV2(("TlsInitHandshake : handle %d : Entry\n" , p_TlsConnection->handle ));

    p_TlsConnection->p_BIO =
 //        BIO_new_socket(
         BIO_new_fd(
         p_TlsConnection->socket,
            BIO_NOCLOSE);

      if(p_TlsConnection->p_BIO == NULL)
      {
         DEBUG_LOG_PRINT_LEV2(("ERROR failed to create SSL BIO on socket\n" ));
         if(p_TlsConnection->socket)   close(p_TlsConnection->socket);
         return K_TLS_ERROR_BIO_INIT;
      }

    // Prepare for SSL layer
     p_TlsConnection->p_SSL = SSL_new(gp_SSL_CTX);
     if(p_TlsConnection->p_SSL == NULL)
     {
        DEBUG_LOG_PRINT_LEV2(("ERROR failed to create SSL structure for SSL connection\n" ));
        BIO_free(p_TlsConnection->p_BIO);
        p_TlsConnection->p_BIO = NULL;
        return K_TLS_ERROR_SSL_INIT;
     }

     SSL_set_bio(
        p_TlsConnection->p_SSL,
        p_TlsConnection->p_BIO,
        p_TlsConnection->p_BIO);
     SSL_set_mode(p_TlsConnection->p_SSL, SSL_MODE_AUTO_RETRY);

     TlsTransitionState(p_TlsConnection , K_TLS_STATE_HANDSHAKE_IN_PROGRESS);

     DEBUG_LOG_PRINT_LEV2(("TlsInitHandshake : Exit\n"));

     return TlsDoHandshake( p_TlsConnection );
}

static void TlsHandlePossibleConnect( t_TlsConnection *p_Conn )
{
    struct sockaddr_in remote;
    socklen_t v_length;
    int v_RetVal;

    v_length = sizeof(remote);

    v_RetVal = getpeername(p_Conn->socket, (struct sockaddr *)&remote, &v_length);
    DEBUG_LOG_PRINT_LEV2(("getpeername : returned %d\n" , v_RetVal ));
    if( v_RetVal == 0 )
    {
        if( p_Conn->isTls )
        {
        TlsInitHandshake(p_Conn);
        }
        else
        {
            TlsTransitionState(p_Conn , K_TLS_STATE_HANDSHAKE_DONE );
            TlsConnectCnf(p_Conn->handle );
        }
    }
    else
    {
        v_RetVal = errno;
        DEBUG_LOG_PRINT_LEV2(("getpeername : errno %s, errValue=%d\n" , strerror(v_RetVal), v_RetVal ));
        TlsConnectErr(p_Conn->handle , 0 );
        TlsRemoveConnection(p_Conn);
    }
}

static void TlsSetNonBlock(int  fd)
{
    int  ret, flags;

    do {
        flags = fcntl(fd, F_GETFL);
    } while (flags < 0 && errno == EINTR);

    if (flags < 0) {
        DEBUG_LOG_PRINT_LEV2(( "%s: could not get flags for fd %d: %s",
               __FUNCTION__, fd, strerror(errno) ));
    }

    do {
        ret = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    } while (ret < 0 && errno == EINTR);

    if (ret < 0) {
        DEBUG_LOG_PRINT_LEV2(( "%s: could not set fd %d to non-blocking: %s",
               __FUNCTION__, fd, strerror(errno) ));
    }
}

void * TlsDnrResolverThread( void *pParam )
{
    struct hostent  *p_ResolvedFQDN;
    t_ResolveDnrReq *p_ResolveDnr;
    t_TlsDnr        v_TlsDnr;
    SOCKADDR_IN     vl_SockAddr;
    int             vl_Connect;
    int             sock;

    p_ResolveDnr = (t_ResolveDnrReq*)pParam;


    v_TlsDnr.v_Handle = p_ResolveDnr->v_Handle;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if ( sock == -1 )
    {
        DEBUG_LOG_PRINT_LEV2(("Socket Error! %d" , errno ));
        return NULL;
    }

    memset(&vl_SockAddr, 0, sizeof(vl_SockAddr));
    vl_SockAddr.sin_family = AF_INET;
    vl_SockAddr.sin_port = htons(dnsResolvePort);
    vl_SockAddr.sin_addr.s_addr = htonl( INADDR_SERVER_HOST );

    vl_Connect = connect( sock,
                      (struct sockaddr *)&vl_SockAddr,
                      sizeof(vl_SockAddr));

    if( vl_Connect == -1 )
    {
        DEBUG_LOG_PRINT_LEV2(("Connection Error! %d" , errno ));
        return NULL;
    }

    DEBUG_LOG_PRINT_LEV2(("Calling gethostbyname for server %s\n" ,p_ResolveDnr->v_Server.a_Addr ));

    p_ResolvedFQDN = gethostbyname( p_ResolveDnr->v_Server.a_Addr );

    DEBUG_LOG_PRINT_LEV2(("Called gethostbyname with return %p\n" , p_ResolvedFQDN ));

    if( NULL == p_ResolvedFQDN  )
    {
        /* Send error */
        v_TlsDnr.v_Report = K_TLS_DNS_ERROR;
    }
    else
    {
        /* We only retrieve the first address in the list and use it.
            The address is null terminated.
            future : If necessary, we may possibly copy the whole list and then
            cycle through it to find  working address */

        v_TlsDnr.v_Addr.sin_family = p_ResolvedFQDN->h_addrtype;
        v_TlsDnr.v_Addr.sin_port = htons(p_ResolveDnr->v_Server.v_Port);
        v_TlsDnr.v_Addr.sin_addr.s_addr = *((unsigned long *)(p_ResolvedFQDN->h_addr_list[0]) );

        v_TlsDnr.v_Report = K_TLS_DNS_SUCCESS;
    }

    DEBUG_LOG_PRINT_LEV2(("Sending DNR result %d\n" , v_TlsDnr.v_Report ));

    send( sock , &v_TlsDnr , sizeof(v_TlsDnr) , 0);

//  shutdown( sock , SD_SEND );

    usleep(100*1000);
    if(sock>=0)

    close(sock);

    free( p_ResolveDnr );
    p_ResolveDnr = NULL;

    return NULL;
}

int TlsSetListenForDnsSocket()
{
   struct sockaddr_in Server_Address; /* Local address */
    /* + RRR : Dnr Resolution Problem */
    int attempts = 0;
    int success = FALSE;
    /* - RRR : Dnr Resolution Problem */

   dnsResolvePort = 9999;

    /* + RRR : Dnr Resolution Problem */
    do
    {
        int status;

   memset(&Server_Address, 0, sizeof(Server_Address)); /* Zero out structure */
   Server_Address.sin_family = AF_INET;                /* Internet address family */
   Server_Address.sin_addr.s_addr = htonl(INADDR_SERVER_HOST); /* Any incoming interface */
   Server_Address.sin_port = htons(dnsResolvePort);        /* Local port */

   dnsResolveServer =  socket( AF_INET ,SOCK_STREAM, 0);
        if (-1 == dnsResolveServer )
        {
            // Failed
            attempts++;
            continue;
        }


        status=1;

        if (setsockopt(dnsResolveServer,SOL_SOCKET,SO_REUSEADDR,&status,sizeof(status)) == -1)
        {
            DEBUG_LOG_PRINT_LEV2(("TlsSetListenForDnsSocket : setsockopt failed due to %s\n", strerror(errno) ));
        }

        DEBUG_LOG_PRINT_LEV2(("TlsSetListenForDnsSocket : dnsResolveServer = %d\n" , dnsResolveServer ));

   if (bind(dnsResolveServer,
               (struct sockaddr *) &Server_Address,
               sizeof(Server_Address)) < 0)
    {
            DEBUG_LOG_PRINT_LEV2(("TlsSetListenForDnsSocket : Couldn't bind to socket at port address %d due to %s!\n" ,
                dnsResolvePort , strerror(errno)));
            dnsResolvePort ++;

            if(dnsResolveServer >= 0)   close( dnsResolveServer );
            dnsResolveServer = 0;
            attempts++;
    }
    else
    {
            success = TRUE;
        }
    }while( (!success ) && (attempts <=5 ) );
    /* - RRR : Dnr Resolution Problem */

    /* + RRR : Dnr Resolution Problem */
    if( success )
    {
    /* - RRR : Dnr Resolution Problem */
         /* Mark the socket so it will listen for incoming connections */
         if (listen(dnsResolveServer, 1) < 0)
         {
             DEBUG_LOG_PRINT_LEV2(("socket() failed" ));
             return FALSE;
         }
         else
         {
            //TlsSetNonBlock(dnsResolveServer);
            DEBUG_LOG_PRINT_LEV2(("Listening for connection now"));
            return TRUE;
         }
    }
    /* + RRR : Dnr Resolution Problem */
    else return FALSE;
    /* + RRR : Dnr Resolution Problem */
}


e_TlsError TlsDoDnsResolution(t_TlsConnection *p_Conn)
{
    t_ResolveDnrReq    *p_ResolveDnr;
    pthread_t           thread;

/* + RRR : Dnr Resolution Problem */
    if( TlsSetListenForDnsSocket() == FALSE )
    {
        DEBUG_LOG_PRINT_LEV2(("TlsDoDnsResolution : Setup Failed!"));
        return K_TLS_ERROR_DNS_RESOLVE_FAILURE;
    }
/* - RRR : Dnr Resolution Problem */

    p_ResolveDnr = malloc( sizeof( *p_ResolveDnr ) );
    p_ResolveDnr->v_Handle = p_Conn->handle;
    p_ResolveDnr->v_Server = p_Conn->server;

    if( 0 != pthread_create( &thread , NULL ,TlsDnrResolverThread ,p_ResolveDnr ) )
    {
        if(dnsResolveServer >= 0)   close( dnsResolveServer );
        return K_TLS_ERROR_DNS_RESOLVE_FAILURE;
    }

    /* Now we wait for the thread to create the connection */
    dnsResolveClient = accept( dnsResolveServer , NULL , NULL );

    DEBUG_LOG_PRINT_LEV2(("TlsDoDnsResolution : Thread has connected at socket %d!" , dnsResolveClient ));

    if( dnsResolveClient >= 0 )
    {
        DEBUG_LOG_PRINT_LEV2(("TlsDoDnsResolution : Set Dns socket to non blocking!"));
        TlsSetNonBlock(dnsResolveClient);
        return K_TLS_NO_ERROR;
    }
    else return K_TLS_ERROR_DNS_RESOLVE_FAILURE;
}

e_TlsError TlsDoConnect( t_TlsDnr v_TlsDnr )
{
    t_TlsConnection *p_TlsConnection;
    int            vl_Connect;
    int            vl_GetLastError = 0;
    struct sockaddr *pl_soc=NULL;

    DEBUG_LOG_PRINT_LEV2(("TlsDoConnect : handle %d\n" , v_TlsDnr.v_Handle ));

    p_TlsConnection = TlsGetConnFromHandle(v_TlsDnr.v_Handle);

    p_TlsConnection->socket = socket(v_TlsDnr.v_Addr.sin_family, SOCK_STREAM, 0);
    if ( p_TlsConnection->socket < 0 )
    {
        DEBUG_LOG_PRINT_LEV2(("TlsHandleConnectReq : socket : %s" , strerror(vl_GetLastError) ));
        return K_TLS_ERROR_NO_SOCKET;
    }

    // make sure we don't block on receives
    DEBUG_LOG_PRINT_LEV2(("TlsHandleConnectReq : Calling TlsSetNonBlock\n"));
    TlsSetNonBlock( p_TlsConnection->socket );
    DEBUG_LOG_PRINT_LEV2(("TlsHandleConnectReq : Called TlsSetNonBlock\n"));
    if(FALSE == TlsBindSocketToInterface(p_TlsConnection->socket))
    {
        DEBUG_LOG_PRINT_LEV2(("TlsHandleConnectReq : Bind failed trying the regular connection\n"));
        //TBD whether this issue is because gprs0 is not available or the build was done wrongly return K_TLS_ERROR_NO_CONNECT;
    }

    vl_Connect = connect( p_TlsConnection->socket, (struct sockaddr *)&v_TlsDnr.v_Addr ,
                      sizeof(*pl_soc));

    vl_GetLastError = errno;

    DEBUG_LOG_PRINT_LEV2(("TlsHandleConnectReq : connect : %d" , vl_Connect ));

    if( vl_Connect > 0 )
    {
        TlsInitHandshake( p_TlsConnection );
        DEBUG_LOG_PRINT_LEV2(("TlsHandleConnectReq : connect succeeded\n"));
        return K_TLS_NO_ERROR;
    }
    else if( vl_Connect == -1 )
    {
        DEBUG_LOG_PRINT_LEV2(("TlsHandleConnectReq : connect : %s" , strerror(vl_GetLastError) ));
        if( (EWOULDBLOCK == vl_GetLastError ) || ( EINPROGRESS == vl_GetLastError ))
        {
            TlsTransitionState(p_TlsConnection ,  K_TLS_STATE_WAIT_SERVER_CONNECTION);
            return K_TLS_WARN_WAIT_SERVER_CONNECTION;
        }
        else
        {
            DEBUG_LOG_PRINT_LEV2(("TlsHandleConnectReq : connect error : %d\n",vl_GetLastError));
            return K_TLS_ERROR_NO_CONNECT;
        }
    }
    else
    {
        DEBUG_LOG_PRINT_LEV2(("TlsHandleConnectReq : connect error vl_Connect : %d\n",vl_Connect));
        return K_TLS_ERROR_NO_CONNECT;
    }
}

void TlsHandleDnsResponse()
{
    int v_Length;
    t_TlsDnr v_Dnr;

    DEBUG_LOG_PRINT_LEV2(("TlsHandleDnsResponse : Entry"));

    v_Length = recv( dnsResolveClient , &v_Dnr , sizeof(v_Dnr) , 0 );

    if( v_Length == sizeof(v_Dnr ) )
    {
        if( v_Dnr.v_Report == K_TLS_DNS_SUCCESS )
        {
            TlsDoConnect(v_Dnr);
        }
        else
        {
            t_TlsConnection *p_Conn =  TlsGetConnFromHandle(v_Dnr.v_Handle);
            DEBUG_LOG_PRINT_LEV2(("TlsConnectErr : v_Dnr.v_Report is error."));
            TlsConnectErr(v_Dnr.v_Handle,0);
            TlsRemoveConnection(p_Conn);
        }
    }
    else
    {
        DEBUG_LOG_PRINT_LEV2(("Incorrect Data Length from DNS Thread : %d bytes" , v_Length));
    }

    if(dnsResolveClient>=0)   close( dnsResolveClient );
    if(dnsResolveServer>=0)   close( dnsResolveServer );

    dnsResolveClient = -1;
    dnsResolveServer = -1;
}

static e_TlsError TlsConnectEx( unsigned int handle , t_TlsServer *p_Server , int isSecure )
{
    t_TlsConnection *p_TlsConnection;
    int lv_Ret = K_TLS_NO_ERROR;

    p_TlsConnection = TlsGetFreeConnection();

    p_TlsConnection->handle = handle;

    p_TlsConnection->isTls = isSecure;
    t_TlsServer *pl_TlsServer=NULL;

    memcpy( &p_TlsConnection->server , p_Server , sizeof(*pl_TlsServer) );

    DEBUG_LOG_PRINT_LEV2(("TlsConnectEx : handle %d , server : %s , port = %u , TLS=%s\n" , p_TlsConnection->handle , p_TlsConnection->server.a_Addr , p_TlsConnection->server.v_Port , p_TlsConnection->isTls ? "YES" : "NO" ));

    switch(p_TlsConnection->server.v_Type)
    {
        case K_TLS_IPV4_ADDRESS:
        {
            t_TlsDnr v_Dnr;

            memset(&v_Dnr.v_Addr, 0, sizeof(v_Dnr.v_Addr));

            v_Dnr.v_Handle = p_TlsConnection->handle;
            v_Dnr.v_Addr.sin_family = AF_INET;
            v_Dnr.v_Addr.sin_port = htons(p_TlsConnection->server.v_Port);
            v_Dnr.v_Addr.sin_addr.s_addr = inet_addr(p_TlsConnection->server.a_Addr);

            lv_Ret =TlsDoConnect(v_Dnr);
        }
        break;

        case K_TLS_FQDN_ADDRESS:
        {
            lv_Ret =TlsDoDnsResolution(p_TlsConnection);
        }
        break;
        default:
            /* Not handled : K_TLS_IPV6_ADDRESS*/
            break;

    }

    DEBUG_LOG_PRINT_LEV2(("TlsConnect : lv_Ret = %d!", lv_Ret));
    return lv_Ret;
}


e_TlsError TlsConnectUnsecure( unsigned int handle , t_TlsServer *p_Server )
{
    return TlsConnectEx( handle , p_Server , FALSE );
}

void TlsDoRead( t_TlsConnection *p_Conn )
{
    int vl_ReadBytes;
    char *pl_DataBuffer;
    int  vl_Length = 0;


    pl_DataBuffer = malloc( TLS_READ_BYTE_CHUNK );

    do
    {
        if( p_Conn->isTls )
        {
        vl_ReadBytes = SSL_read(p_Conn->p_SSL,pl_DataBuffer + vl_Length,TLS_READ_BYTE_CHUNK);
        }
        else
        {
            vl_ReadBytes = read(p_Conn->socket,pl_DataBuffer + vl_Length,TLS_READ_BYTE_CHUNK);
        }

        if( vl_ReadBytes == 0 )
        {
            break;
        }
        else if ( vl_ReadBytes == TLS_READ_BYTE_CHUNK )
        {
            vl_Length += TLS_READ_BYTE_CHUNK;
            pl_DataBuffer = realloc( pl_DataBuffer , vl_Length + TLS_READ_BYTE_CHUNK );
        }
        else
        {
            vl_Length += vl_ReadBytes;
        }
    } while( vl_ReadBytes == TLS_READ_BYTE_CHUNK );


    if( vl_Length > 0 )
    {
        TlsReadInd( p_Conn->handle , pl_DataBuffer , vl_Length );
    }

    free( pl_DataBuffer );
    pl_DataBuffer = NULL;

    if( vl_ReadBytes == 0 )
    {
        if( p_Conn->state == K_TLS_STATE_SHUTDOWN_IN_PROGRESS )
        {
            TlsDisconnectCnf(p_Conn->handle);
            TlsRemoveConnection( p_Conn );
        }
    }
}

int TlsHandleReadEvent( const fd_set *p_Set )
{
    int vl_Index;
    t_TlsConnection *p_Conn;
    int vl_Count=0;

    if( dnsResolveClient >= 0 )
    {
        // Check if there is data from DNS thread
        if( FD_ISSET(dnsResolveClient , p_Set) )
        {
            DEBUG_LOG_PRINT_LEV2(("TlsHandleReadEvent : Response to DNR query \n"));
            TlsHandleDnsResponse();
        }
    }

    for( vl_Index = 0; vl_Index < TLS_MAX_CONNECTIONS ; vl_Index++ )
    {
        p_Conn = g_TlsConnection + vl_Index;

        if( FD_ISSET(p_Conn->socket , p_Set) )
        {
            vl_Count++;
            DEBUG_LOG_PRINT_LEV2(("TlsHandleReadEvent : %d\n" , p_Conn->handle ));
            switch( p_Conn->state )
            {
            case K_TLS_STATE_HANDSHAKE_IN_PROGRESS:
                TlsDoHandshake( p_Conn );
                break;
            case K_TLS_STATE_HANDSHAKE_DONE:
            case K_TLS_STATE_SHUTDOWN_IN_PROGRESS:
                //Do a read here
                TlsDoRead(p_Conn);
                break;
            default:
                /* Not handled : K_TLS_STATE_UNUSED , K_TLS_STATE_WAIT_SERVER_CONNECTION & K_TLS_STATE_DNS_RESOLUTION_IN_PROGRESS */
                break;
            }
        }
    }

    return vl_Count;
}

int TlsHandleWriteEvent( const fd_set *p_Set )
{
    int vl_Index;
    t_TlsConnection *p_Conn;
    int vl_Count=0;

    for( vl_Index = 0; vl_Index < TLS_MAX_CONNECTIONS ; vl_Index++ )
    {
        p_Conn = g_TlsConnection + vl_Index;
        if( FD_ISSET(p_Conn->socket , p_Set) )
        {
            vl_Count++;
            DEBUG_LOG_PRINT_LEV2(("TlsHandleWriteEvent : %d\n" , p_Conn->handle ));
            switch( p_Conn->state )
            {
            case K_TLS_STATE_WAIT_SERVER_CONNECTION:
                TlsHandlePossibleConnect( p_Conn );
                //TlsInitHandshake( p_Conn );
                break;
            default:
                DEBUG_LOG_PRINT_LEV2(("WARNING: Write event cannot occurs in this state = %d\n" , p_Conn->state));
            }
        }
    }

    return vl_Count;
}

int TlsHandleErrorEvent( const fd_set *p_Set )
{
    int vl_Index;
    t_TlsConnection *p_Conn;
    int vl_Count=0;

    for( vl_Index = 0; vl_Index < TLS_MAX_CONNECTIONS ; vl_Index++ )
    {
        p_Conn = g_TlsConnection + vl_Index;
        if( FD_ISSET(p_Conn->socket , p_Set) )
        {
            vl_Count++;
            DEBUG_LOG_PRINT_LEV2(("TlsHandleErrorEvent : %d\n" , p_Conn->handle ));
        }
    }

    return vl_Count;
}

int TlsPopulateWriteSelect( fd_set *p_Set )
{
    int vl_Index;
    t_TlsConnection *p_Conn;
    int highestFd = 0;


//    DEBUG_LOG_PRINT_LEV2(("TlsPopulateWriteSelect : Entry"));

    for( vl_Index = 0; vl_Index < TLS_MAX_CONNECTIONS ; vl_Index++ )
    {
        p_Conn = g_TlsConnection + vl_Index;
        if( p_Conn->state == K_TLS_WARN_WAIT_SERVER_CONNECTION )
        {
            FD_SET( p_Conn->socket , p_Set );
             if( highestFd < p_Conn->socket )
                 highestFd = p_Conn->socket;
        }
    }

    return highestFd;
}

int TlsPopulateReadSelect( fd_set *p_Set )
{
    int vl_Index;
    t_TlsConnection *p_Conn;
    int highestFd = 0;

    DEBUG_LOG_PRINT_LEV2(("TlsPopulateReadSelect : Entry"));

    for( vl_Index = 0; vl_Index < TLS_MAX_CONNECTIONS ; vl_Index++ )
    {
        p_Conn = g_TlsConnection + vl_Index;
        if( p_Conn->state != K_TLS_STATE_UNUSED )
        {
            FD_SET( p_Conn->socket , p_Set );
            if( highestFd < p_Conn->socket )
                 highestFd = p_Conn->socket;
        }
    }

    if( dnsResolveClient >= 0 )
    {
        DEBUG_LOG_PRINT_LEV2(("TlsPopulateReadSelect : Waiting for DNS query return"));
        FD_SET( dnsResolveClient , p_Set );

        if( highestFd < dnsResolveClient )
        {
            highestFd = dnsResolveClient;
        }
    }

    return highestFd;
}

int TlsPopulateErrorSelect( fd_set *p_Set )
{
//    DEBUG_LOG_PRINT_LEV2(("TlsPopulateErrorSelect : Entry"));
    return TlsPopulateReadSelect( p_Set );
}

e_TlsError TlsDoShutdown( t_TlsConnection *p_TlsConnection )
{
   if( p_TlsConnection->isTls )
    {
    int vl_Return;

    vl_Return = SSL_shutdown(p_TlsConnection->p_SSL);

    if( vl_Return < 0 )
    {
        int vl_SslError = SSL_get_error(p_TlsConnection->p_SSL,vl_Return);
        switch(vl_SslError)
        {
            case SSL_ERROR_WANT_READ:
                TlsTransitionState( p_TlsConnection , K_TLS_STATE_SHUTDOWN_IN_PROGRESS );
                return K_TLS_NO_ERROR;
            case SSL_ERROR_WANT_WRITE:
                return TlsDoShutdown( p_TlsConnection );
            default:
                return K_TLS_ERROR_SSL_HANDSHAKE;
        }
    }
    else
    {
        TlsTransitionState( p_TlsConnection , K_TLS_STATE_SHUTDOWN_IN_PROGRESS );
        return K_TLS_NO_ERROR;
    }
    }
    else
    {
        shutdown(p_TlsConnection->socket, SHUT_WR);
        TlsTransitionState( p_TlsConnection , K_TLS_STATE_SHUTDOWN_IN_PROGRESS);
        return K_TLS_NO_ERROR;
    }
}

void TlsSetNetworkInterface( char *ifname )
{
    int Max_size;

    Max_size = strlen(ifname)+1;
    vg_NetworkInterface = (char *) malloc (Max_size);

    if( NULL != ifname )
    {
        memcpy(vg_NetworkInterface,ifname,strlen(ifname)+1);
        DEBUG_LOG_PRINT(("TlsSetNetworkInterface : Set %s",vg_NetworkInterface));
    }
    else
    {
        memset(vg_NetworkInterface,0,Max_size );
        DEBUG_LOG_PRINT(("TlsSetNetworkInterface : Clear Network Interface"));
    }
}


void Tls_openssl_info_callback(SSL *s, int where, int ret)
{
    const char *str;
    int w;

    w=where& ~SSL_ST_MASK;

    if (w & SSL_ST_CONNECT) str="Handshake";
    else if (w & SSL_ST_ACCEPT) str="SSL_accept";
    else str="undefined";

    if (where & SSL_CB_LOOP)
    {
        DEBUG_LOG_PRINT_LEV2(("TlsDebug : %s:%s\n",str,SSL_state_string_long(s)));
    }

    if (where & SSL_CB_ALERT)
    {
        str=(where & SSL_CB_READ)?"read":"write";
        DEBUG_LOG_PRINT_LEV2(("TlsDebug : SSLv3 alert %s:%s:%s\n",\
                str,\
                SSL_alert_type_string_long(ret),\
                SSL_alert_desc_string_long(ret)));
    }

    if (where & SSL_CB_EXIT)
    {
        if (ret == 0)
        {
            DEBUG_LOG_PRINT_LEV2(("TlsDebug : %s:failed in %s\n",\
                    str,SSL_state_string_long(s)));
        }
        else if (ret < 0)
        {
            int ret1 = SSL_get_error(s,ret);
            switch(ret1)
            {
                case SSL_ERROR_WANT_READ:
                    DEBUG_LOG_PRINT_LEV2(("TlsDebug : %s:incomplete : %s\n",\
                        str,SSL_state_string_long(s)));
                break;

                case SSL_ERROR_WANT_WRITE:
                    DEBUG_LOG_PRINT_LEV2(("TlsDebug : %s:incomplete : %s\n",\
                        str,SSL_state_string_long(s)));
                break;
                default:
                    DEBUG_LOG_PRINT_LEV2(("TlsDebug : %s:error in %s\n",\
                        str,SSL_state_string_long(s)));
                    if (w & SSL_ST_CONNECT)
                    {
                        DEBUG_LOG_PRINT_LEV2(("TlsDebug : %s:Done",str));
                    }
            }
        }
    }

    if(  where & SSL_CB_HANDSHAKE_START)
    {
        DEBUG_LOG_PRINT_LEV2(("TlsDebug : Handshake:Start"));
    }

    if(  where & SSL_CB_HANDSHAKE_DONE )
    {
        DEBUG_LOG_PRINT_LEV2(("TlsDebug : Handshake:Done"));
    }
}




