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

#ifndef BEARER_HANDLER_H
#define BEARER_HANDLER_H


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

typedef enum
{
    AGPS_BEARER_REQUESTOR_SUPL,
    AGPS_BEARER_REQUESTOR_SBEE
}BearerRequestor;

typedef enum
{
    AGPS_BEARER_NOT_REQUESTED,
    AGPS_BEARER_OPEN_REQUESTED,
    AGPS_BEARER_CLOSE_REQUESTED,
    AGPS_BEARER_CONNECTION_ERR,
    AGPS_BEARER_CONNECTION_CNF,
    AGPS_BEARER_CONNECTION_CLOSED,
    AGPS_BEARER_CLOSE_CNF,
    AGPS_BEARER_CLOSE_ERR
}BearerRequestStatus;


typedef struct
{
    BearerRequestStatus SuplRequestStatus;
    BearerRequestStatus SbeeRequestStatus;
    BearerRequestStatus BearerRequestStatus;
}BearerState;


void ProcessOpenBearerConnectionRequest(BearerRequestor requestor);

void ProcessCloseBearerConnectionRequest(BearerRequestor requestor);

void ProcessDataConnectionResponse( BearerRequestStatus bearerRequestStatus );

void BearerHandlerInit(t_gpsClient_agpsCallbacks *cbs);


#endif /* BEARER_HANDLER_H */

/*Begin Test Code */

/*End Test code */



