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

#ifndef ARM_LINUX_AGPS_FTR
#include "android_log.h"
#endif
#include "string.h"
#include <pthread.h>
#include "agps_client.h"
#include "gpsclient.h"
#include "BearerHandler.h"
#include "sbeesocketinterface.h"
#include "sbeeclient.h"


BearerState bearerState;
pthread_mutex_t criticalSectionForBearer;

static t_gpsClient_agpsCallbacks   *callbacks = NULL;

void gpsclient_SendCommandToServer( t_agps_Command *command );


void BearerHandlerInit(t_gpsClient_agpsCallbacks *cbs)
{
    callbacks = cbs;
    bearerState.SuplRequestStatus = AGPS_BEARER_NOT_REQUESTED;
    bearerState.SbeeRequestStatus = AGPS_BEARER_NOT_REQUESTED;
    bearerState.BearerRequestStatus = AGPS_BEARER_NOT_REQUESTED;
    pthread_mutex_init(&criticalSectionForBearer, NULL);
}


static BOOL SendAgpsBearerRequest(e_gpsClient_AgpsStatData statusValue)
{
    BOOL retVal = FALSE;

    if(callbacks)
    {
        callbacks->agpsstat_cb( &statusValue );
        retVal = TRUE;
    }
    else
    {
        DEBUG_LOG_PRINT_LEV2(("agpsCallbacks->status_cb is null pointer\n"));
    }

    return retVal;
}

static BOOL OpenBearerConnection()
{
    return SendAgpsBearerRequest( GPSCLIENT_REQUEST_AGPS_DATA_CONN );
}


static BOOL CloseBearerConnection()
{
    return SendAgpsBearerRequest( GPSCLIENT_RELEASE_AGPS_DATA_CONN );
}

static void SetBearerState( BearerRequestor requestor, BearerRequestStatus status )
{
    DEBUG_LOG_PRINT_LEV2(("BearerHandler.SetBearerState(), Entry : Table status bearer :%d   supl: %d  sbee: %d ", bearerState.BearerRequestStatus, bearerState.SuplRequestStatus, bearerState.SbeeRequestStatus ));
    if( requestor == AGPS_BEARER_REQUESTOR_SUPL )
       bearerState.SuplRequestStatus = status;
    else if( requestor == AGPS_BEARER_REQUESTOR_SBEE )
       bearerState.SbeeRequestStatus = status;
    DEBUG_LOG_PRINT_LEV2(("BearerHandler.SetBearerState(), Exit  : Table status bearer :%d   supl: %d  sbee: %d ", bearerState.BearerRequestStatus, bearerState.SuplRequestStatus, bearerState.SbeeRequestStatus ));
}

static void SendBearerResponseToServer( t_agps_CommandType type )
{
    t_agps_Command command;
    t_agps_Packet packet;

    command.type = type;

    packet.pData = NULL;
    packet.length = 0;
    command.data = (void *) ( &packet );

    gpsclient_SendCommandToServer( &command );
}


static void SendBearerResponse( BearerRequestor requestor, BearerRequestStatus currentStatus,  BearerRequestStatus previousStatus )
{
    t_agps_CommandType result = SOCKET_AGPS_END_COMMAND_TYPE;

    DEBUG_LOG_PRINT_LEV2(("BearerHandler.SendBearerResponse(): requestor = %d, currentStatus = %d, previousStatus = %d ", requestor, currentStatus, previousStatus ));
    DEBUG_LOG_PRINT_LEV2(("BearerHandler.SendBearerResponse(), Entry : Table status bearer :%d   supl: %d  sbee: %d ", bearerState.BearerRequestStatus, bearerState.SuplRequestStatus, bearerState.SbeeRequestStatus ));
    switch( currentStatus )
    {
        case AGPS_BEARER_CONNECTION_ERR:
        case AGPS_BEARER_CLOSE_ERR:

        if( requestor == AGPS_BEARER_REQUESTOR_SBEE && previousStatus == AGPS_BEARER_OPEN_REQUESTED )
            sbeeClient_DataconnectionStatus( 1 );
        else if( requestor == AGPS_BEARER_REQUESTOR_SBEE && previousStatus == AGPS_BEARER_CLOSE_REQUESTED )
            sbeeClient_DataconnectionStatus( 3 );
        else if( requestor == AGPS_BEARER_REQUESTOR_SUPL && previousStatus == AGPS_BEARER_OPEN_REQUESTED )
            result = SOCKET_AGPS_START_BEARER_ERR;
        else
            result = SOCKET_AGPS_CLOSE_BEARER_ERR;

        break;

        case AGPS_BEARER_CONNECTION_CNF:

        if( requestor == AGPS_BEARER_REQUESTOR_SBEE && previousStatus == AGPS_BEARER_OPEN_REQUESTED )
                sbeeClient_DataconnectionStatus( 0 );
            else if( requestor == AGPS_BEARER_REQUESTOR_SBEE && previousStatus == AGPS_BEARER_CLOSE_REQUESTED )
                sbeeClient_DataconnectionStatus( 4 );
            else if( requestor == AGPS_BEARER_REQUESTOR_SUPL && previousStatus == AGPS_BEARER_OPEN_REQUESTED )
                result = SOCKET_AGPS_START_BEARER_CNF;
            else
                result = SOCKET_AGPS_CLOSE_BEARER_CNF;

        break;

        case AGPS_BEARER_CONNECTION_CLOSED:
        case AGPS_BEARER_CLOSE_CNF:

            if( requestor == AGPS_BEARER_REQUESTOR_SBEE )
                sbeeClient_DataconnectionStatus( 0 );
            else
                result = SOCKET_AGPS_CLOSE_BEARER_CNF;

        break;

        case AGPS_BEARER_NOT_REQUESTED:
        case AGPS_BEARER_OPEN_REQUESTED:
        case AGPS_BEARER_CLOSE_REQUESTED:
        {
            DEBUG_LOG_PRINT_LEV2(("BearerHandler.SendBearerResponse(): Unexpected state, %d", (int)currentStatus ));
        }
        break;
        
        DEFAULT:
        {
            /* dummy assignment to get rid of warnings when DEBUG_LOG_PRINT_LEV2 is not defined */
            currentStatus = currentStatus;
            DEBUG_LOG_PRINT_LEV2(("BearerHandler.SendBearerResponse(): Unknown state, %d", (int)currentStatus ));
        }
    }

    if( result != SOCKET_AGPS_END_COMMAND_TYPE )
         SendBearerResponseToServer( result );

    DEBUG_LOG_PRINT_LEV2(("BearerHandler.SendBearerResponse(), Exit  : Table status bearer :%d   supl: %d  sbee: %d ", bearerState.BearerRequestStatus, bearerState.SuplRequestStatus, bearerState.SbeeRequestStatus ));

}


void ProcessOpenBearerConnectionRequest(BearerRequestor requestor)
{
    pthread_mutex_lock( &criticalSectionForBearer );

    DEBUG_LOG_PRINT_LEV2(("BearerHandler.ProcessOpenBearerConnectionRequest(), Entry : Table status bearer :%d   supl: %d  sbee: %d ", bearerState.BearerRequestStatus, bearerState.SuplRequestStatus, bearerState.SbeeRequestStatus ));
    switch(bearerState.BearerRequestStatus)
    {
         case AGPS_BEARER_OPEN_REQUESTED:

            DEBUG_LOG_PRINT_LEV2(("BearerHandler.ProcessOpenBearerConnectionRequest(): state is AGPS_BEARER_OPEN_REQUESTED" ));

            SetBearerState( requestor, AGPS_BEARER_OPEN_REQUESTED );

        break;

        case AGPS_BEARER_CLOSE_REQUESTED:

            DEBUG_LOG_PRINT_LEV2(("BearerHandler.ProcessOpenBearerConnectionRequest(): state is AGPS_BEARER_CLOSE_REQUESTED" ));

            SetBearerState( requestor, AGPS_BEARER_CONNECTION_ERR );

            SendBearerResponse( requestor, AGPS_BEARER_CONNECTION_ERR, AGPS_BEARER_OPEN_REQUESTED );

        break;

        case AGPS_BEARER_CONNECTION_CNF:

            DEBUG_LOG_PRINT_LEV2(("BearerHandler.ProcessOpenBearerConnectionRequest(): state is AGPS_BEARER_CONNECTION_CNF" ));

            SetBearerState( requestor, AGPS_BEARER_CONNECTION_CNF );

            SendBearerResponse( requestor, AGPS_BEARER_CONNECTION_CNF, AGPS_BEARER_OPEN_REQUESTED );

        break;

        case AGPS_BEARER_NOT_REQUESTED:

            DEBUG_LOG_PRINT_LEV2(("BearerHandler.ProcessOpenBearerConnectionRequest(): state is AGPS_BEARER_NOT_REQUESTED" ));

            SetBearerState( requestor, AGPS_BEARER_OPEN_REQUESTED );

            bearerState.BearerRequestStatus = AGPS_BEARER_OPEN_REQUESTED;

            if( OpenBearerConnection() )
            {
                DEBUG_LOG_PRINT_LEV2(("BearerHandler.ProcessOpenBearerConnectionRequest(): OpenBearerConnection() called successfully" ));

                DEBUG_LOG_PRINT_LEV2(("BearerHandler.ProcessOpenBearerConnectionRequest(): requestor = %d, BearerRequestStatus = %d", requestor, bearerState.BearerRequestStatus ));
            }
            else
            {
                SendBearerResponse( requestor, AGPS_BEARER_CONNECTION_ERR, AGPS_BEARER_OPEN_REQUESTED );

                SetBearerState( requestor, AGPS_BEARER_NOT_REQUESTED );

                bearerState.BearerRequestStatus = AGPS_BEARER_NOT_REQUESTED;
            }

        break;

        case AGPS_BEARER_CONNECTION_ERR:
        case AGPS_BEARER_CONNECTION_CLOSED:
        case AGPS_BEARER_CLOSE_CNF:
        case AGPS_BEARER_CLOSE_ERR:
        {
             DEBUG_LOG_PRINT_LEV2(("BearerHandler.ProcessOpenBearerConnectionRequest(): Unexpected state, %d", bearerState.BearerRequestStatus ));
        }
        break;

        DEFAULT:
        {
            /* dummy assignment to get rid of warnings when DEBUG_LOG_PRINT_LEV2 is not defined */
            bearerState.BearerRequestStatus = bearerState.BearerRequestStatus;
            DEBUG_LOG_PRINT_LEV2(("BearerHandler.ProcessOpenBearerConnectionRequest(): Unknown state, %d", bearerState.BearerRequestStatus ));
        }
    }

    DEBUG_LOG_PRINT_LEV2(("BearerHandler.ProcessOpenBearerConnectionRequest(), Exit  : Table status bearer :%d   supl: %d  sbee: %d ", bearerState.BearerRequestStatus, bearerState.SuplRequestStatus, bearerState.SbeeRequestStatus ));

    pthread_mutex_unlock( &criticalSectionForBearer );
}


void ProcessCloseBearerConnectionRequest(BearerRequestor requestor)
{
    pthread_mutex_lock( &criticalSectionForBearer );

    DEBUG_LOG_PRINT_LEV2(("BearerHandler.ProcessCloseBearerConnectionRequest(), Entry : Table status bearer :%d   supl: %d  sbee: %d ", bearerState.BearerRequestStatus, bearerState.SuplRequestStatus, bearerState.SbeeRequestStatus ));
    switch(bearerState.BearerRequestStatus)
    {
        case AGPS_BEARER_OPEN_REQUESTED:

            DEBUG_LOG_PRINT_LEV2(("BearerHandler.ProcessCloseBearerConnectionRequest(): state is AGPS_BEARER_OPEN_REQUESTED" ));

            SetBearerState( requestor, AGPS_BEARER_CONNECTION_ERR );

            SendBearerResponse( requestor, AGPS_BEARER_CONNECTION_ERR, AGPS_BEARER_CLOSE_REQUESTED );

        break;

        case AGPS_BEARER_CLOSE_REQUESTED:

            DEBUG_LOG_PRINT_LEV2(("BearerHandler.ProcessCloseBearerConnectionRequest(): state is AGPS_BEARER_CLOSE_REQUESTED" ));

            SetBearerState( requestor, AGPS_BEARER_CLOSE_REQUESTED );

        break;

        case AGPS_BEARER_CONNECTION_CNF:

            DEBUG_LOG_PRINT_LEV2(("BearerHandler.ProcessCloseBearerConnectionRequest(): state is AGPS_BEARER_CONNECTION_CNF" ));

            SetBearerState( requestor, AGPS_BEARER_CLOSE_REQUESTED );

            if( bearerState.SuplRequestStatus != AGPS_BEARER_CONNECTION_CNF &&
                bearerState.SbeeRequestStatus != AGPS_BEARER_CONNECTION_CNF )
            {
                SetBearerState( requestor, AGPS_BEARER_CLOSE_REQUESTED );

                bearerState.BearerRequestStatus = AGPS_BEARER_CLOSE_REQUESTED;

                if( !CloseBearerConnection() )
                {
                    SendBearerResponse( requestor, AGPS_BEARER_CLOSE_ERR, AGPS_BEARER_CLOSE_REQUESTED );
                    SetBearerState( requestor, AGPS_BEARER_CLOSE_ERR );
                    bearerState.BearerRequestStatus = AGPS_BEARER_NOT_REQUESTED;
                }
            }
            else
            {
                SendBearerResponse( requestor, AGPS_BEARER_CLOSE_CNF, bearerState.BearerRequestStatus );
                SetBearerState( requestor, AGPS_BEARER_CONNECTION_CLOSED );
            }

        break;

        case AGPS_BEARER_NOT_REQUESTED:

            DEBUG_LOG_PRINT_LEV2(("BearerHandler.ProcessCloseBearerConnectionRequest(): state is AGPS_BEARER_NOT_REQUESTED" ));

            SendBearerResponse( requestor, AGPS_BEARER_CLOSE_ERR, AGPS_BEARER_CLOSE_REQUESTED );

        break;

        case AGPS_BEARER_CONNECTION_ERR:
        case AGPS_BEARER_CONNECTION_CLOSED:
        case AGPS_BEARER_CLOSE_CNF:
        case AGPS_BEARER_CLOSE_ERR:
        {
            DEBUG_LOG_PRINT_LEV2(("BearerHandler.ProcessCloseBearerConnectionRequest(): Unexpected state, %d", bearerState.BearerRequestStatus ));
        }
        break;
        DEFAULT:
        {
            /* dummy assignment to get rid of warnings when DEBUG_LOG_PRINT_LEV2 is not defined */
            bearerState.BearerRequestStatus = bearerState.BearerRequestStatus;
            DEBUG_LOG_PRINT_LEV2(("BearerHandler.ProcessCloseBearerConnectionRequest(): Unknown state, %d", bearerState.BearerRequestStatus ));
        }
    }
    DEBUG_LOG_PRINT_LEV2(("BearerHandler.ProcessCloseBearerConnectionRequest(), Exit  : Table status bearer :%d   supl: %d  sbee: %d ", bearerState.BearerRequestStatus, bearerState.SuplRequestStatus, bearerState.SbeeRequestStatus ));
    pthread_mutex_unlock( &criticalSectionForBearer );
}


void ProcessDataConnectionResponse( BearerRequestStatus bearerRequestStatus )
{
    pthread_mutex_lock( &criticalSectionForBearer );

    DEBUG_LOG_PRINT_LEV2(("BearerHandler.ProcessDataConnectionResponse(), Entry : Table status bearer :%d   supl: %d  sbee: %d ", bearerState.BearerRequestStatus, bearerState.SuplRequestStatus, bearerState.SbeeRequestStatus ));
    switch(bearerRequestStatus)
    {
        case AGPS_BEARER_CONNECTION_CNF:

            DEBUG_LOG_PRINT_LEV2(("BearerHandler.ProcessDataConnectionResponse(): state is AGPS_BEARER_CONNECTION_CNF" ));

            if( bearerState.SuplRequestStatus == AGPS_BEARER_OPEN_REQUESTED )
            {
                DEBUG_LOG_PRINT_LEV2(("BearerHandler.ProcessDataConnectionResponse(): requestor is SUPL" ));
                SetBearerState( AGPS_BEARER_REQUESTOR_SUPL, AGPS_BEARER_CONNECTION_CNF );
                SendBearerResponse( AGPS_BEARER_REQUESTOR_SUPL, AGPS_BEARER_CONNECTION_CNF, bearerState.BearerRequestStatus );
            }

            if( bearerState.SbeeRequestStatus == AGPS_BEARER_OPEN_REQUESTED )
            {
                DEBUG_LOG_PRINT_LEV2(("BearerHandler.ProcessDataConnectionResponse(): requestor is SBEE" ));
                SetBearerState( AGPS_BEARER_REQUESTOR_SBEE, AGPS_BEARER_CONNECTION_CNF );
                SendBearerResponse( AGPS_BEARER_REQUESTOR_SBEE, AGPS_BEARER_CONNECTION_CNF, bearerState.BearerRequestStatus );
            }
            bearerState.BearerRequestStatus = AGPS_BEARER_CONNECTION_CNF;

        break;

        case AGPS_BEARER_CONNECTION_ERR:

            DEBUG_LOG_PRINT_LEV2(("BearerHandler.ProcessDataConnectionResponse(): state is AGPS_BEARER_CONNECTION_ERR, SuplRequestStatus = %d, SbeeRequestStatus = %d",
                                                                bearerState.SuplRequestStatus, bearerState.SbeeRequestStatus ));

            if( bearerState.SuplRequestStatus == AGPS_BEARER_OPEN_REQUESTED || bearerState.SuplRequestStatus == AGPS_BEARER_CLOSE_REQUESTED )
            {
                DEBUG_LOG_PRINT_LEV2(("BearerHandler.ProcessDataConnectionResponse(): requestor is SUPL" ));
                SetBearerState( AGPS_BEARER_REQUESTOR_SUPL, AGPS_BEARER_CONNECTION_ERR );
                SendBearerResponse( AGPS_BEARER_REQUESTOR_SUPL, AGPS_BEARER_CONNECTION_ERR, bearerState.BearerRequestStatus );
            }

            if( bearerState.SbeeRequestStatus == AGPS_BEARER_OPEN_REQUESTED || bearerState.SbeeRequestStatus == AGPS_BEARER_CLOSE_REQUESTED )
            {
                DEBUG_LOG_PRINT_LEV2(("BearerHandler.ProcessDataConnectionResponse(): requestor is SBEE" ));
                SetBearerState( AGPS_BEARER_REQUESTOR_SBEE, AGPS_BEARER_CONNECTION_ERR );
                SendBearerResponse( AGPS_BEARER_REQUESTOR_SBEE, AGPS_BEARER_CONNECTION_ERR, bearerState.BearerRequestStatus );
            }
            bearerState.BearerRequestStatus = AGPS_BEARER_NOT_REQUESTED;

        break;

        case AGPS_BEARER_CLOSE_CNF:

            DEBUG_LOG_PRINT_LEV2(("BearerHandler.ProcessDataConnectionResponse(): state is AGPS_BEARER_CLOSE_CNF" ));

            if( bearerState.SuplRequestStatus == AGPS_BEARER_CLOSE_REQUESTED )
            {
                DEBUG_LOG_PRINT_LEV2(("BearerHandler.ProcessDataConnectionResponse(): requestor is SUPL" ));
                SetBearerState( AGPS_BEARER_REQUESTOR_SUPL, AGPS_BEARER_CONNECTION_CLOSED );
                SendBearerResponse( AGPS_BEARER_REQUESTOR_SUPL, AGPS_BEARER_CONNECTION_CLOSED, bearerState.BearerRequestStatus );
            }

            if( bearerState.SbeeRequestStatus == AGPS_BEARER_CLOSE_REQUESTED )
            {
                DEBUG_LOG_PRINT_LEV2(("BearerHandler.ProcessDataConnectionResponse(): requestor is SBEE" ));
                SetBearerState( AGPS_BEARER_REQUESTOR_SBEE, AGPS_BEARER_CONNECTION_CLOSED );
                SendBearerResponse( AGPS_BEARER_REQUESTOR_SBEE, AGPS_BEARER_CONNECTION_CLOSED, bearerState.BearerRequestStatus );
            }
            bearerState.BearerRequestStatus = AGPS_BEARER_NOT_REQUESTED;

        break;

        case AGPS_BEARER_NOT_REQUESTED:
        case AGPS_BEARER_OPEN_REQUESTED:
        case AGPS_BEARER_CLOSE_REQUESTED:
        case AGPS_BEARER_CONNECTION_CLOSED:
        case AGPS_BEARER_CLOSE_ERR:
        {
            DEBUG_LOG_PRINT_LEV2(("BearerHandler.ProcessDataConnectionResponse(): Unexpected BearerRequestStatus, %d", bearerRequestStatus ));
        }
        break;

        DEFAULT:
        {
            /* dummy assignment to get rid of warnings when DEBUG_LOG_PRINT_LEV2 is not defined */  
            bearerRequestStatus = bearerRequestStatus;
            DEBUG_LOG_PRINT_LEV2(("BearerHandler.ProcessDataConnectionResponse(): Unknown BearerRequestStatus, %d", bearerRequestStatus ));
        }
    }
    DEBUG_LOG_PRINT_LEV2(("BearerHandler.ProcessDataConnectionResponse(), Exit  : Table status bearer :%d   supl: %d  sbee: %d ", bearerState.BearerRequestStatus, bearerState.SuplRequestStatus, bearerState.SbeeRequestStatus ));
    pthread_mutex_unlock( &criticalSectionForBearer );
}


