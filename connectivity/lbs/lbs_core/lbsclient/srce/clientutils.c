/*
* Copyright (C) ST-Ericsson 2009
*
* clientutils.c
* Date: 17-07-2009
* Author: Anil Kumar
* Email : anil.nambiar@stericsson.com
*/

#include <errno.h>
#include <pthread.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <math.h>
#include <time.h>
#ifndef ARM_LINUX_AGPS_FTR
#include "android_log.h"
#endif
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "agps_client.h"
#include "agps.h"
#include "clientutils.h"
#include "agpscodec.h"

#define MAX_SECONDS_TO_POLL 0xFFFF;
#define RECONNECT_MAX_RETRIES  50
#define RECONNECT_WAIT_DURATION_IN_MUSEC 200*1000


#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define  LOG_TAG  "clientutils"

/* +LMSqc22572 -Anil */
t_agps_CommandType receivedCommandType = SOCKET_AGPS_END_COMMAND_TYPE;
/* -LMSqc22572 -Anil */

/*****************************************************************************/
/* clientUtils_WaitForAckFromServer : wait for Server to reply */
/*****************************************************************************/
uint16_t clientUtils_WaitForStatusFromServer( t_clientUtils_GpsState *gpsState )
{
    uint16_t vl_status=0;
    DEBUG_LOG_PRINT_LEV2(("clientUtils_WaitForStatusFromServer\n"));

    while( gpsState->statusValue == SOCKET_AGPS_NO_STATUS_AVAILABLE )
    {
        usleep(SLEEP_TIME_MICRO_SECONDS);
    }
    vl_status= gpsState->statusValue;

    gpsState->statusValue = SOCKET_AGPS_NO_STATUS_AVAILABLE;

    DEBUG_LOG_PRINT_LEV2(("clientUtils_WaitForStatusFromServer : Done\n"));
    return vl_status;
}


/*****************************************************************************/
/* clientUtils_StopGpsState : Close GPS Client */
/*****************************************************************************/
void clientUtils_StopGpsState( t_clientUtils_GpsState *gpsState )
{
    if(gpsState->init == 1)
    {
        // close connection to the GPS daemon

        if(gpsState->fd != -1) {
            //set exitThread to 1, to indicate the GpsThread to exit and wait for thread to exit.
            DEBUG_LOG_PRINT_LEV2(("StopGpsState(): Setting ExitGpsThread to 1, to indicate the GpsThread to exit and wait for thread to exit.\n"));
            gpsState->exitThread = 1;

            DEBUG_LOG_PRINT_LEV2(("StopGpsState(): Closing connection to the socket\n"));
            shutdown( gpsState->fd , SHUT_WR );

            DEBUG_LOG_PRINT_LEV2(("StopGpsState(): Waiting for thread to exit\n"));
            pthread_join(gpsState->thread, NULL);
            /* close( gpsState->fd );  */
            gpsState->fd = -1;
        }
        DEBUG_LOG_PRINT_LEV2(("StopGpsState(): Setting 0 to gpsState.init\n"));
        gpsState->init = 0;
        gpsState->callback = NULL;
    }

}


/*****************************************************************************/
/* clientUtils_GpsStateThread : Main GPS Client Thread */
/*****************************************************************************/
static void clientUtils_GpsStateThread( void* arg )
{

    int         started   = 0;
    int         highestFD = 0;
    fd_set      rset,wset,eset;

    struct timeval tv;
    int cnt;

    t_clientUtils_GpsState *gpsState = (t_clientUtils_GpsState *)arg;

    DEBUG_LOG_PRINT_LEV2(("%s: gps thread running, value of gpsState.ExitGpsThread : %d\n",__FUNCTION__, gpsState->exitThread));


    // now loop
    for (;gpsState->exitThread == 0;) {

    RECONNECT_SUCCESS:
        FD_ZERO(&rset);
        FD_ZERO(&wset);
        FD_ZERO(&eset);

        FD_SET( gpsState->fd, &rset );
        highestFD = gpsState->fd;

        if( NULL != gpsState->callbackFdUpdate )
        {
            int vl_Index, maxFd;
            maxFd = gpsState->callbackFdUpdate( &rset , &wset , &eset );
            DEBUG_LOG_PRINT_LEV2(("%s: gpsState->callbackFdUpdate returned maxFD %d, highestFD %d\n",__FUNCTION__,maxFd,highestFD));

            if( maxFd > highestFD )
                highestFD = maxFd;
        }

        tv.tv_sec = MAX_SECONDS_TO_POLL;
        tv.tv_usec = 0;

        cnt = select( highestFD + 1, &rset, &wset, NULL /*&eset*/, &tv );

        if( cnt )
        {
            DEBUG_LOG_PRINT_LEV2(("%s: Select returned with %d events\n",__FUNCTION__,cnt));
        }
        else
        {
            DEBUG_LOG_PRINT_LEV2(("%s: Select returned with no events\n",__FUNCTION__));
        }
        while( cnt >= 1 )
        {
            if( FD_ISSET( gpsState->fd, &rset) )
            {
                char    *p_Buffer;
                int     v_DataLen;
                int     done = FALSE;

                p_Buffer = (char *) malloc( BUFFER_SIZE );
                v_DataLen = 0;

                do
                {
                    int32_t bytesRead = 0;
                    bytesRead = recv(gpsState->fd, p_Buffer + v_DataLen , BUFFER_SIZE, MSG_DONTWAIT );

                    if( bytesRead > 0 )
                    {
                        if( bytesRead < BUFFER_SIZE )
                        {
                            /* No further data to be read. All done */
                            done = TRUE;
                            v_DataLen += bytesRead;
                        }
                        else
                        {
                            /* Re allocate memory and copy it over */
                            char *p_Tmp;

                            /* +LMSqc54521 */
                            /* Update current data size */
                            v_DataLen += bytesRead;
                            /* -LMSqc54521 */

                            p_Tmp = realloc( p_Buffer , v_DataLen + BUFFER_SIZE );

                            if( p_Tmp != NULL )
                            {
                                p_Buffer = p_Tmp;
                                /* +LMSqc54521 */
                                //v_DataLen += BUFFER_SIZE;
                                /* -LMSqc54521 */
                                done = FALSE;
                            }
                            else
                            {
                                done = TRUE;
                                DEBUG_LOG_PRINT_LEV2(("%s: Realloc Failed!!!!",__FUNCTION__));
                            }
                        }
                    }
                    else if(   (bytesRead == 0)
                            || ((bytesRead < 0) && (!((errno == EAGAIN) || (errno == EWOULDBLOCK))))
                            )
                    {
                        bool reconnect = FALSE;
                        if(!((errno == EAGAIN) || (errno == EWOULDBLOCK)))
                        {
                            DEBUG_LOG_PRINT_LEV2(("%s : Error occured : %s hence attempting to reconnect\n",__FUNCTION__, strerror(errno)));
                        }

                        if(p_Buffer != NULL)
                        {
                            free( p_Buffer );
                            p_Buffer = NULL;
                        }

                        /* Attempt to reconnect agpsd->lbsd */
                        reconnect = clientUtils_Reconnect(gpsState);
                        if (FALSE == reconnect)
                        {
                            DEBUG_LOG_PRINT(("%s: Error in reconnection to LBSD\n",__FUNCTION__));
                            DEBUG_LOG_PRINT_LEV2(("Socket closed at lbsd"));
                            /* if this connection is closed, terminate all existing connections and close the thread */
                            gpsState->exitThread = 1;
                            gpsState->init = 0;
                            close( gpsState->fd );
                            return;
                        }
                        else
                        {
                            /* Go back to the beginning of this thread as we may have lost context with lbsd */
                            DEBUG_LOG_PRINT(("%s: SUCCESS: Connection to LBSD re-established.\n",__FUNCTION__));
                            goto RECONNECT_SUCCESS;
                        }
                    }
                    else
                    {
                        DEBUG_LOG_PRINT_LEV2(("Error occured, errno: %d\n", errno));

                        /* FIDO 370798 : Dont free memory when len size is exactly 1024 bytes*/
                        if (v_DataLen != BUFFER_SIZE)
                        {
                            if(p_Buffer != NULL)
                            {
                              free( p_Buffer );
                              p_Buffer = NULL;
                            }
                            v_DataLen = 0;
                        }
                        done = TRUE;

                        /* FIDO 370798 Dont free memory when len size is exactly 1024 bytes*/
                    }

                } while( !done );

                if( v_DataLen > 0)
                {
                    uint32_t bytesProcessed = 0;
                    t_agps_Command command;

                    DEBUG_LOG_PRINT_LEV2(("Data received, No. of bytes received is %d\n", v_DataLen));

                    // @todo : Does not correctly handle case where incorrect data is received!
                    while( bytesProcessed < v_DataLen )
                    {
                        int bytesDecoded;
                        t_Buffer v_ReadBuffer;

                        v_ReadBuffer.p_Buffer = p_Buffer + bytesProcessed;
                        v_ReadBuffer.v_Length = v_DataLen - bytesProcessed;

                        bytesDecoded = agpscodec_DecodeCommand(&command, v_ReadBuffer );
                        bytesProcessed += bytesDecoded;
                        DEBUG_LOG_PRINT_LEV2(("Received data : %d bytes decoded to command\n" ,bytesProcessed ));
/* +LMSqc22572 -Anil */
                        if( command.type != SOCKET_AGPS_STATUS )
                            receivedCommandType = command.type;
/* +LMSqc22572 -Anil */

                        /* Now send the command for further processing */
/* +ER:270717 */
                        if( bytesDecoded )
                         {
                            gpsState->callback( command );

                            if(command.data != NULL)
                            {
                               if(command.data->pData != NULL)
                               {
                                  free(command.data->pData);
                                  command.data->pData = NULL;
                               }

                               free(command.data);
                               command.data = NULL;
                            }
                        }
/* -ER:270717 */
                        else
                        {
/* CDA: Needs review */
#define AGPS_CODEC_HEADER                0xA53C7E5A
                            uint32_t index = bytesProcessed;
                            uint32_t endOfBuffer = v_DataLen;
                            uint32_t  HeaderCheck = 0;
                            unsigned char isHeaderFound = 0;

                            if(v_DataLen >= sizeof(HeaderCheck) )
                            {
                                for( ; index < endOfBuffer - (int)(sizeof(HeaderCheck)-1) && isHeaderFound == 0; index++ )
                            {
                                memcpy(&HeaderCheck, p_Buffer+index, sizeof(HeaderCheck));

                                if( HeaderCheck == AGPS_CODEC_HEADER )
                                {
                                    DEBUG_LOG_PRINT_LEV2(("Header found"));
                                    bytesProcessed = index;
                                    isHeaderFound = 1;
                                }
                            }

                            if( isHeaderFound == 0 )
                            {
                                break;
                               }
                            }
                            else
                            {
                                DEBUG_LOG_PRINT_LEV2(("Data size is less than header size"));
                                break;
                            }
                        }
/* -ER:270717 */
                    }

                }

/* +LMSqc54521 */
                if(p_Buffer != NULL)
                {
                    free(p_Buffer);
                    p_Buffer = NULL;
                }
/* -LMSqc54521 */

            }

            if( NULL != gpsState->callbackEventUpdate )
            {
                cnt -= gpsState->callbackEventUpdate(&rset , &wset , &eset );
            }

            --cnt;
        }
    }
}


/*****************************************************************************
* clientUtils_FdSetNonBlock :
* Sets the given fd (file descriptor) as non blocking
* Parameter: fd :- File descriptor
*****************************************************************************/
static void clientUtils_FdSetNonBlock(int  fd)
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


/*****************************************************************************
* clientUtils_InitGpsState :
*****************************************************************************/
bool clientUtils_InitGpsState(t_clientUtils_GpsState *gpsState)
{
    bool reconnect = FALSE;

    if( gpsState->init != 1 )
    {
        int conStatus;
        gpsState->fd = socket(AF_INET, SOCK_STREAM, 0);  /* init socket descriptor */
        if ( gpsState->fd < 0 )
        {
            DEBUG_LOG_PRINT_LEV2(("Error: socketOpen\n"));
            return FALSE;
        }

        /*** PLACE DATA IN sockaddr_in struct ***/
        bzero(&gpsState->s_in, sizeof(gpsState->s_in));

        gpsState->s_in.sin_family = AF_INET;
        gpsState->s_in.sin_addr.s_addr = htonl(INADDR_SERVER_HOST);
        gpsState->s_in.sin_port = htons(PORT);

        gpsState->exitThread = 0;

        conStatus = connect(gpsState->fd, (struct sockaddr *)&gpsState->s_in, sizeof(gpsState->s_in));
        DEBUG_LOG_PRINT_LEV2((" %s Connect Error : %d\n", __FUNCTION__,errno));

        if (conStatus < 0)
        {
            DEBUG_LOG_PRINT_LEV2(("Error: connecting\n"));

            DEBUG_LOG_PRINT_LEV2(("Attempting Reconnection"));

            /* Attempt to reconnect agpsd->lbsd */
            reconnect = clientUtils_Reconnect(gpsState);
            if (FALSE == reconnect)
            {
                DEBUG_LOG_PRINT(("%s: Error in reconnection to LBSD\n",__FUNCTION__));
                DEBUG_LOG_PRINT_LEV2(("Socket closed at lbsd"));
                /* if this connection is closed, terminate all existing connections and close the thread */
                gpsState->exitThread = 1;
                gpsState->init = 0;
                close( gpsState->fd );
                return FALSE;
            }
            else
            {
                DEBUG_LOG_PRINT(("%s: SUCCESS: Connection to LBSD re-established.\n",__FUNCTION__));
            }
        }

        gpsState->init = 1;

        clientUtils_FdSetNonBlock(gpsState->fd);

        if ( pthread_create( &(gpsState->thread), NULL, (void *)&clientUtils_GpsStateThread, (void *)gpsState ) != 0 )
        {
            DEBUG_LOG_PRINT_LEV2(("could not create gps thread: %s", strerror(errno)));
            return FALSE;
        }

        gpsState->statusValue = SOCKET_AGPS_NO_STATUS_AVAILABLE;

        DEBUG_LOG_PRINT_LEV2(("gps state initialized"));
    }

    return TRUE;
}


bool clientUtils_InitEEState(t_clientUtils_GpsState *gpsState)
{
    if( gpsState->init != 1 )
    {
        int conStatus;
        gpsState->fd = socket(AF_INET, SOCK_STREAM, 0);  /* init socket descriptor */
        if ( gpsState->fd < 0 )
        {
            DEBUG_LOG_PRINT_LEV2(("Error: socketOpen\n"));
            return FALSE;
        }

        gpsState->init = 1;

        /*** PLACE DATA IN sockaddr_in struct ***/
        bzero(&gpsState->s_in, sizeof(gpsState->s_in));

        gpsState->s_in.sin_family = AF_INET;
        gpsState->s_in.sin_addr.s_addr = htonl(INADDR_SERVER_HOST);
        gpsState->s_in.sin_port = htons(PORT);

        gpsState->exitThread = 0;

        conStatus = connect(gpsState->fd, (struct sockaddr *)&gpsState->s_in, sizeof(gpsState->s_in));
        DEBUG_LOG_PRINT_LEV2((" %s Connect Error : %d\n", __FUNCTION__,errno));

        if (conStatus < 0)
        {
            DEBUG_LOG_PRINT_LEV2(("Error: connecting\n"));
            return FALSE;
        }

        clientUtils_FdSetNonBlock(gpsState->fd);

        if ( pthread_create( &(gpsState->thread), NULL, (void *)&clientUtils_GpsStateThread, (void *)gpsState ) != 0 ) {
                DEBUG_LOG_PRINT_LEV2(("could not create gps thread: %s", strerror(errno)));
            return FALSE;
        }

        gpsState->statusValue = SOCKET_AGPS_NO_STATUS_AVAILABLE;

        DEBUG_LOG_PRINT_LEV2(("EE state initialized"));
    }

    return TRUE;
}


/*****************************************************************************
* Send data to server
*****************************************************************************/
void clientUtils_SendCommandToServer( t_clientUtils_GpsState *gpsState, t_agps_Command *command )
{
    t_Buffer v_WriteBuffer;

    DEBUG_LOG_PRINT_LEV2(("Entering clientUtils_SendCommandToServer\n"));

    if( agpscodec_EncodeCommand( command, &v_WriteBuffer) )
    {

        DEBUG_LOG_PRINT_LEV2(("ClientThread: Sending the data\n"));

        send(gpsState->fd, v_WriteBuffer.p_Buffer, v_WriteBuffer.v_Length, 0);
        free( v_WriteBuffer.p_Buffer );
        v_WriteBuffer.p_Buffer = NULL;

        DEBUG_LOG_PRINT_LEV2(("Data sent\n"));
    }
    else
    {
        DEBUG_LOG_PRINT_LEV2(("Failed to encode command!\n"));
    }
    DEBUG_LOG_PRINT_LEV2(("Exiting clientUtils_SendCommandToServer\n"));
}


/*****************************************************************************
* clientUtils_ProcessStatus
*****************************************************************************/
void clientUtils_ProcessStatus( t_clientUtils_GpsState *gpsState, e_agps_Status *status )
{
    DEBUG_LOG_PRINT_LEV2(("clientUtils_ProcessStatus: status : %d", *status ));
    gpsState->statusValue = *status;
}


/*****************************************************************************
* clientUtils_Reconnect
*****************************************************************************/
bool clientUtils_Reconnect(t_clientUtils_GpsState *gpsState)
{
    int conStatus = -1;
    int retryCount = 0;
    bool done = FALSE;

    DEBUG_LOG_PRINT(("%s: Enter\n",__FUNCTION__));
    shutdown( gpsState->fd , SHUT_WR );
    close( gpsState->fd );

    usleep(RECONNECT_WAIT_DURATION_IN_MUSEC);

    gpsState->fd = socket(AF_INET, SOCK_STREAM, 0);  /* init socket descriptor */
    if ( gpsState->fd < 0 )
    {
        DEBUG_LOG_PRINT(("%s Error: Socket creation: %s\n",__FUNCTION__, strerror(errno)));
        return FALSE;
    }

    /*** PLACE DATA IN sockaddr_in struct ***/
    bzero(&gpsState->s_in, sizeof(gpsState->s_in));

    gpsState->s_in.sin_family = AF_INET;
    gpsState->s_in.sin_addr.s_addr = htonl(INADDR_SERVER_HOST);
    gpsState->s_in.sin_port = htons(PORT);

    while ( (retryCount < RECONNECT_MAX_RETRIES) )
    {
        conStatus = connect(gpsState->fd, (struct sockaddr *)&gpsState->s_in, sizeof(gpsState->s_in));

        if (conStatus != 0 )
        {
            DEBUG_LOG_PRINT_LEV2((" %s ERROR: Connect Error. Return %d: %s\n", __FUNCTION__,conStatus,strerror(errno)));
            retryCount++;
            DEBUG_LOG_PRINT((" %s: Retrying: %d time\n", __FUNCTION__,retryCount));
            usleep(RECONNECT_WAIT_DURATION_IN_MUSEC);
        }
        else
        {
            DEBUG_LOG_PRINT(("%s: SUCCESS: Connect call Successful. Return %d\n",__FUNCTION__,conStatus));
            break;
        }
    }

    if (conStatus != 0)
    {
        DEBUG_LOG_PRINT(("%s: Error: Connection failed %d times. Application needs to restart\n",__FUNCTION__,retryCount));
        return FALSE;
    }


    if( gpsState->init == 1 )
    {
        clientUtils_FdSetNonBlock(gpsState->fd);

        gpsState->statusValue = SOCKET_AGPS_NO_STATUS_AVAILABLE;

        gpsState->reconnectReq = TRUE;

        gpsState->callbackReconnect();
    }

    DEBUG_LOG_PRINT_LEV2(("clientUtils_Reconnect: SUCCESS: Reconnected to LBSD"));
    return TRUE;
}

