/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) ST-Ericsson SA 2009. All rights reserved.
 *  This code is ST-Ericsson proprietary and confidential.
 *  Any use of the code for whatever purpose is subject to
 *  specific written permission of ST-Ericsson.
 *
 *  Author: Anshuman Pandey
 *  Email : anshuman.pandey@stericsson.com
 *****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netdb.h>
#include <unistd.h>

#ifndef ARM_LINUX_AGPS_FTR
#include "android_log.h"
#endif
#include "agps_client.h"
#include "gpsclient.h"
#include "sbeesocketinterface.h"
#include "sbeeclient.h"
#include "HttpHandler.h"


pthread_t    SbeeDownloadThread;


/*
*  Generates formatted(GET) HTTP request string. The generated string can be send directly to http server.
*/
static char * GenerateHttpGetRequestString( char * requestString, char *server, int port )
{
    char   request[1024];
    char   *p_Request;




    DEBUG_LOG_PRINT_LEV2(("GenerateHttpGetRequestString(): Parameter requestString = %s, server = %s " , requestString, server ));
    snprintf(request , sizeof(request) , "GET http://%s:%d%s %s\r\nHost: %s:%d\r\n%s\r\n\r\n",
                        server,
                        port,
                        requestString,
                        HTTP_VERSION,
                        server,
                        port,
                        HTTP_USER_AGENT);

    DEBUG_LOG_PRINT_LEV2(("HTTP : %s" , request ));

    p_Request = malloc(strlen(request));
    strcpy(p_Request,request);

    return p_Request;
}


/*
*  Creates a connection to the sepecified address and port.
*  Returns the socket id for the connection. Returns -1 if any error.
*/
static int CreateConnection(char * server, int portNumber, struct addrinfo  **serverInfo)
{
    int               socketId;            // Socket connection id
    struct addrinfo   hints;               // Hints to help finding the Server address
    struct addrinfo  *temp;                // local ptr used to walk the link list
    char              port[6];             // Server port
    int               err;                 // Capture error information

    DEBUG_LOG_PRINT_LEV2(( "CreateConnection:  Server %s,  Port %d",
      server, portNumber ));

    memset( &hints,   0, sizeof(hints)   );

    // Get the HTTP Server Address information
    hints.ai_family    = AF_INET;             // for IPV4 adresses. Use AF_INET6 for IPV6 or AF_UNSPEC for both
    hints.ai_socktype  = SOCK_STREAM;         // TCP stream socket
    sprintf( port, "%d", portNumber );

    err = getaddrinfo( server, port, &hints, serverInfo );
    if (  err != 0 )
    {
        DEBUG_LOG_PRINT_LEV2(( "CreateConnection: Error finding server, getaddrinfo: %s", gai_strerror(err) ));
        return -1;
    }

    // Loop through all the results and connect to the first we can.
    socketId = -1;
    for( temp = *serverInfo ; temp != NULL ; temp = temp->ai_next )
    {
        struct sockaddr_in *tmp_addrin;

        socketId = socket( temp->ai_family, temp->ai_socktype, temp->ai_protocol );

        if ( socketId == -1 )
        {
            DEBUG_LOG_PRINT_LEV2(( "CreateConnection: Error creating a socket." ));
            continue; // We try the next socket
        }

        // we have a successful socket, try to connect
        tmp_addrin = (struct sockaddr_in *)temp->ai_addr;

        DEBUG_LOG_PRINT_LEV2(( "CreateConnection: IP address to connect is %s", inet_ntoa(tmp_addrin->sin_addr)));
        if ( connect( socketId, temp->ai_addr, temp->ai_addrlen ) == -1 )
        {
            // connect has not succeeded, print out the error cause and try with the next address
            DEBUG_LOG_PRINT_LEV2(( "CreateConnection: connect failed due to: %s, trying next address", strerror(errno)));
            close( socketId );
            socketId = -1;
        }
        else
        {
            DEBUG_LOG_PRINT_LEV2(( "CreateConnection: Success!"));
            // We have successfully connected, so we can now break out of this loop
            break;
        }
    }

    if( temp == NULL)
    {
        DEBUG_LOG_PRINT_LEV2(( "CreateConnection: Failed to connect"));
    }


    return socketId;
}



/*DEBUG_LOG_PRINT_LEV2
*  Sends HTTP request over the specified socket. This function formats the request string to HTTP GET format using the GenerateHttpGetRequestString() function
*
*  Returns TRUE if the data is sent properly otherwise a FALSE.
*/
static BOOL SendHttpRequest( char * request, char *server, int port, int socketId )
{
    char * requestString = NULL;
    BOOL retVal = TRUE;
    int bytesSent;

    requestString = GenerateHttpGetRequestString( request, server, port );

    bytesSent = send( socketId, requestString, strlen(requestString), 0 );
    DEBUG_LOG_PRINT_LEV2(( "SendHttpRequest:   %d bytes, length = %d", bytesSent, strlen(requestString)));
    if ( (size_t)bytesSent != strlen(requestString) )
    {
       DEBUG_LOG_PRINT_LEV2(( "SendHttpRequest: Error sending request., error = %s", strerror(errno) ));

       retVal = FALSE;
    }

    free( requestString );
    requestString = NULL;

    return retVal;
}



/*
*  Watis for the response from the server. This function has to be called immediately after sending the request to the server.
*
*  Returns TRUE if the data is downloaded properly otherwise a FALSE.
*/
static BOOL WaitForResponse( char *response, int socketId, int *bytesReceived )
{

    int count = 0;
    *bytesReceived = -1;
       do
       {
          usleep( 200*1000 );        // Sleep 200ms
          *bytesReceived = recv( socketId, response, SBEE_RESPONSE_LENGTH, MSG_WAITALL );
          DEBUG_LOG_PRINT_LEV2(( "OS_Download_Bytes_From_HTTP_Server:  recv(),  %d bytes", *bytesReceived ));
       }
       while( *bytesReceived == -1  &&  count++ < (3000/200) ); /* stops after 3000/200 (15 * 200 ms)  iterations if it does not get data */

       if( *bytesReceived == -1 )
           return FALSE;
       else
           return TRUE;
}


/*
*  Extracts the payload from the response
*
*  Returns number of bytes in the payload. If any thing is wrong it will return a -1.
*/
static int ProcessHttpResponse( char *response, char *outBuffer, int bytesReceived )
{
    int outputSize = 0;
    int i;

    char *head;
    char *foot;

    // Find the start of the HTTP header (includes "<RXN...>" tag).
    head = strstr( response, "<RXN" );

    // If the start could not be found, return with an error.
    if ( head == NULL )
    {
       DEBUG_LOG_PRINT_LEV2(( "ProcessHttpResponse: <RXN header not found." ));
       return( -1 );
    }

    // Find a pointer to the char at the START of the "<\RXN>" tag.
    // Note that string fcns such as strstr can't be used as they may hit
    // a string within seed data and be thrown off.
    foot = NULL;
    for ( i = 0 ; i < (bytesReceived - 5) ; i++ )
    {
       if( response[i]   == '<'  &&  response[i+1] == '/'  &&
           response[i+2] == 'R'  &&  response[i+3] == 'X'  &&
           response[i+4] == 'N'  &&  response[i+5] == '>'      )
       {
          foot = &response[i+6];         // The NULL terminator !
       }
    }

    // If the "<\RXN>" could not be found, return with an error.
    if ( foot == NULL )
    {
       return( -1 );
    }

    // Get the seed length.
    outputSize = (uint16_t)( foot - head );

   // Copy the seed data.
    memcpy( outBuffer, head, outputSize );

       return( outputSize );
}


/*
*  Handles download process (request and response)
*
*  Returns TRUE if successfull, otherwise returns FALSE.
*/
void HttpDownloadThread( void *arg )
{
    HttpRequest request;
    struct addrinfo *addressInfo;
    int socketId;
    char response[SBEE_RESPONSE_LENGTH];
    char payload[SBEE_PAYLOAD_LENGTH];
    unsigned char *payloadData;
    int bytesReceived;
    int payloadLength;
    BOOL retVal = FALSE;

    memcpy( &request, arg, sizeof(request) );
    free(arg);
    arg = NULL;

    socketId = CreateConnection(request.serverAddress, request.port, &addressInfo );

    if( socketId >= 0 )
    {
        DEBUG_LOG_PRINT_LEV2(( "HttpDownloadThread(): Request string is : %s", request.requestString ));
        DEBUG_LOG_PRINT_LEV2(( "HttpHandler.ProcessHttpRequest: server : %s, port : %d", request.serverAddress, request.port ));

        if( SendHttpRequest( request.requestString, request.serverAddress, request.port, socketId ) == TRUE )
        {
            if( WaitForResponse( response, socketId, &bytesReceived ) == TRUE )
            {
                payloadLength = ProcessHttpResponse(response, payload, bytesReceived );

                if( payloadLength > 0 )
                {
                    retVal = TRUE;
                }
            }
        }
    }

    if( retVal == TRUE )
    {
        DEBUG_LOG_PRINT_LEV2(( "HttpDownloadThread(): download successfull" ));
        payloadData = (unsigned char *) malloc( payloadLength );
        memcpy( payloadData, payload, payloadLength );

        sbeeClient_SendData( payloadData, (unsigned short) payloadLength );
    }
    else
    {
        DEBUG_LOG_PRINT_LEV2(( "HttpDownloadThread(): Failed to download" ));
        sbeeClient_DataconnectionStatus(2);
    }

    if( socketId >= 0)
        close( socketId );
    if( addressInfo != NULL )
        freeaddrinfo( addressInfo );
}


/*
*  Interface which should be used from gpsclient to process HTTP request. Initiates a thread for downloading and returns
*
*  Returns TRUE if successfully launched the thread, other wise a FALSE
*/
BOOL ProcessHttpRequest( char *request, char * server, int port )
{
    HttpRequest *requestData;

    requestData = (HttpRequest *) malloc(sizeof(HttpRequest));

    strcpy(requestData->requestString, request);
    strcpy(requestData->serverAddress, server);
    requestData->port = port;

    DEBUG_LOG_PRINT_LEV2(( "HttpHandler.ProcessHttpRequest: Request string : %s", requestData->requestString));
    DEBUG_LOG_PRINT_LEV2(( "HttpHandler.ProcessHttpRequest: server : %s, port : %d", server, port ));

    if ( pthread_create( &SbeeDownloadThread, NULL, (void *)&HttpDownloadThread, (void *)(requestData) ) != 0 ) {
#ifndef ARM_LINUX_AGPS_FTR
        ALOGE("could not create PGPS download thread: %s", strerror(errno));
#else
        DEBUG_LOG_PRINT_LEV2(( "ERROR could not create PGPS download thread: %s", strerror(errno) ));
#endif
        return FALSE;
    }

    return TRUE;
}
