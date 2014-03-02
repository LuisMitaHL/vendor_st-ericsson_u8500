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
#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H

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

#define HTTP_GET        "GET"
#define HTTP_VERSION    "HTTP/1.1"
#define HTTP_USER_AGENT "User-Agent: HTMLGET 1.1"
#define HTTP_HOST       "Host"

#define SBEE_RESPONSE_LENGTH 4000
#define SBEE_PAYLOAD_LENGTH  3000

typedef struct
{
    char requestString[SBEE_REQUEST_STRING_LENGTH];
    char serverAddress[SBEE_SERVER_ADDRESS_LENGTH];
    uint32_t port;
}HttpRequest;

BOOL ProcessHttpRequest( char *request, char * server, int port );

#endif /* #ifndef HTTP_HANDLER_H */


