/*
* Copyright (C) ST-Ericsson 2009
*
* agpsserver.c
* Date: 17-07-2009
* Author: Anil Kumar
* Email : anil.nambiar@stericsson.com
*/

#include "agps_server.h"
#include <unistd.h>
#include <sys/un.h>
#include <asm/socket.h>
#ifndef ARM_LINUX_AGPS_FTR
#include <android_log.h>
#endif
#include "agpsosa.h"
#include "agps.h"
#include "agpsServerInterface.h"
#include "agpscodec.h"
/* +Rahul 16-02-2010 */
#include "cgps.h"
/* -Rahul 16-02-2010 */
#include "lbscfg.h"
#ifdef AMIL_FTR
#include "amil.h"
#endif

#ifdef AGPS_GENERIC_SOLUTION_FTR
#include "gns.h"
#ifdef AGPS_UP_FTR
#include "gnsSUPL_Api.h"
#endif
#ifdef GNS_CELL_INFO_FTR
#include "gnsCellInfo_Api.h"
#endif
#ifdef AGPS_FTR
#include "gnsCP_Api.h"
#endif
#endif

#ifdef AMIL_LCS_FTR
#include "amilExtern.h"
#endif
#ifdef AGPS_SBEE_FTR
#include "sbee.h"
#include "sbeesocketinterface.h"
#endif

#include "gnsEE.h"
#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "AgpsServer"

//*****************************************************************************
/// \struct t_agps_Client
/// \brief
///      The structure stores the GPS Client Information
/// \details

typedef struct t_agps_Client   {
    struct t_agps_Client    *next;
    int                     channelId;
    int                     clientId;
    int                     toBeClosed;
    int                     fd;
    int                     sendBufferSize;
    void                    *bufferToSend;
    uint32_t                     capabilityMask;
} t_agps_Client ;

/**Event variable for Socket Listener (main socket)*/
struct epoll_event  EpollEvent;

/**Sockets for clients. For each, assign one event.*/
struct epoll_event  Events[MAX_GPS_CLIENT];

/**Socket address
struct sockaddr_in srv;*/
struct sockaddr_in srv;

/**File Descriptor for socket Listener (main socket)*/
int             ListenerFd;

/**File Descriptor for Epoll Functions*/
int             EpollFd;

/**Header for the client list (linked list for storing the information of each t_agps_Client )*/
t_agps_Client           *clients = NULL;

/**Flag variable for stoping the socket server.
For stoping socket server assign 1 to this variable.*/
int      ExitSocketServer = 0;

/**For mapping callback functions and fd's for gps client*/
t_agps_CallbackMap gpsClientCallbackMap[MAX_GPS_CLIENT];

/* + PROD_TEST */
int     ProductionTestFd;
/* + PROD_TEST */
int     DebugLogFd;
/**
* \brief This Callback function is registered by upper modules
*/
typedef void (* t_agps_DataCntNotificationCallback ) (
    t_agps_NotificationInd notification
);
#ifdef AGPS_SBEE_FTR
t_sbee_Callbacks sbee_cb;
#endif /* #ifdef AGPS_SBEE_FTR */


/**
* \brief This Callback function is registered by upper modules
*/
typedef void (* t_agps_BearerRequestCallback) ();


/**
* \brief This Callback function is registered by upper modules
*/
typedef void (* t_agps_TcpIpConnectionReqCallback)(t_agps_TcpIpConnectReq *tcpIpRequest);

/**
* \brief This Callback function is registered by upper modules
*/
typedef void (* t_agps_NotificationCallback)(t_agps_NotificationInd *notification);

#ifdef AGPS_EE_FTR

/**
* \brief This Callback function is registered by upper modules
*/
typedef void (* t_agps_EEClientCallback)(t_agps_ExtendedEphInd *notification);

/**
* \brief This Callback function is registered by upper modules
*/
typedef void (* t_agps_EEClientRefLocationCallback)();


/**
* \brief This Callback function is registered by upper modules
*/

typedef void (* t_agps_EEClientGetRefTimeCallback)();


typedef void (* t_agps_EEClientDeleteSeedCallback)();


typedef void (* t_agps_EEClientBCECallback)(t_gnsEE_NavDataBCE *pp_BCE);

#endif

/**
* \brief This Callback function is registered by upper modules
* the navigation data from AGPS.
* \warning The memory for the notification parameter is allocated by the AGPS module and freed by the application.
*/
typedef void (* t_agps_NavigationCallback) (
  t_agps_NavData p_NavData /**< [in] Navigation data of type \ref t_agps_CStructNavData or \ref t_agps_NmeaNavData */
);
typedef void (* t_agps_StatusCallback) (
  e_agps_Status agpsStatus /**< [in] Status message refer \ref e_agps_Status */
);


typedef struct
{
    int v_Enum;
    char* p_EnumAsString;
}t_agps_enum_as_string;

#define ADD_ENUM_AS_STRING( enum )  { enum , #enum }

t_agps_enum_as_string  a_SocketCommandEnumAsString[] =
{
    ADD_ENUM_AS_STRING(SOCKET_AGPS_STATUS), /* Server -> Client */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SERVICE_START), /* Client -> Server */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_APPLICATIONID_START), /* Client -> Server */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SERVICE_STOP), /* Client -> Server */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SWITCH_OFF_GPS), /* Client -> Server */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_PERIODIC_FIX), /* Client -> Server */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SINGLE_SHOT_FIX), /* Client -> Server */

    /* +Rahul 16-02-2010 */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_NAVDATA),/* Server -> Client */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_DELETE_AIDING_DATA),/* Client -> Server */

    ADD_ENUM_AS_STRING(SOCKET_AGPS_NMEA_DATA), /* Server -> Client */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_FENCE_GEOGRAPHICAL_AREA), /* Client -> Server */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_LOCATION_FORWARD), /* Client -> Server */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_LOCATION_RETRIEVAL), /* Client -> Server */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_NOTIFICATION), /* Client -> Server */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_NOTIFICATION_USER_RESPONSE), /* Server -> Client */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SET_USER_CONFIG), /* Client -> Server */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_USER_CONFIG_REQ), /* Client -> Server  */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_USER_CONFIG_RSP), /* Server -> Client */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_START_BEARER_REQ), /* Server -> Client */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_START_BEARER_CNF), /* Client -> Server */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_START_BEARER_ERR), /* Client -> Server */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_CLOSE_BEARER_REQ), /* Server -> Client */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_CLOSE_BEARER_IND), /* Client -> Server */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_CLOSE_BEARER_CNF), /* Client -> Server */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_CLOSE_BEARER_ERR), /* Client -> Server */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SUPL_TCPIP_CONNECT_REQ), /* Server -> Client */ /** \ref t_agps_TcpIpConnectReq */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SUPL_TCPIP_CONNECT_CNF), /* Client -> Server */ /** \ref t_agps_TcpIpConnectCnf */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SUPL_TCPIP_CONNECT_ERR), /* Client -> Server */ /** \ref t_agps_TcpIpConnectErr */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SUPL_TCPIP_DISCONNECT_REQ), /* Server -> Client */ /** \ref t_agps_TcpIpDisconnectReq */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SUPL_TCPIP_DISCONNECT_IND),/* Client -> Server */ /** \ref t_agps_TcpIpDisconnectInd */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SUPL_TCPIP_DISCONNECT_ERR),/* Client -> Server */ /** \ref t_agps_TcpIpDisconnectErr */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SUPL_TCPIP_DISCONNECT_CNF),/* Client -> Server */ /** \ref t_agps_TcpIpDisconnectCnf */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SUPL_MOBILE_INFO_IND),/* Client -> Server */ /** \ref t_agps_MobileInfoInd */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_WIRELESS_NETWORK_INFO_IND),/* Client -> Server */ /** \ref t_agps_WLANInfoInd */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SUPL_SMS_PUSH_IND), /* Client -> Server *//** \ref t_agps_SmsPushInd */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SUPL_SEND_DATA_REQ),/* Server -> Client */ /** \ref t_agps_SuplData */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SUPL_SEND_DATA_CNF),/* Client -> Server */ /** \ref t_agps_SuplSendDataCnf */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SUPL_RECEIVE_DATA_IND),/* Client -> Server */ /** \ref t_agps_SuplData */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SUPL_RECEIVE_DATA_CNF), /* Server -> Client *//** \ref t_agps_SuplReceiveDataCnf */
   /*Anil 16-03-2010*/
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SBEE_INITIALIZE),       /* Client -> Server */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SBEE_SET_DEVICE_CHARGING_STATUS), /* Client -> Server */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SBEE_SET_DEVICE_BEARER_STATUS), /* Client -> Server */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SBEE_SET_NETWORK_STATUS), /* Client -> Server */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SBEE_SET_SERVER_CONFIGURATION), /* Client -> Server */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SBEE_SET_USER_CONFIGURATION),  /* Client -> Server */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SBEE_START_EXTENDED_EPHEMERIS),  /* Client -> Server */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SBEE_ABORT_EXTENDED_EPHEMERIS), /* Client -> Server */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SBEE_SET_DATA_CONNECTION_STATUS), /* Client -> Server */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SBEE_SET_USER_RESPONSE), /* Client -> Server */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SBEE_RECV_DATA),  /* Client -> Server */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SBEE_NOTIFY_CONNECTION), /* Server -> Client */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SBEE_REQUEST_DATA_CONNECTION), /* Server -> Client */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SBEE_REQUEST_DATA_DISCONNECT), /* Server -> Client */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SBEE_DATA), /* Server -> Client */
   /*-Anil 16-03-2010*/
    /* +Rahul 16-02-2010 */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_GET_VERSION), /* Client -> Server */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_VERSION), /* Server < -> Client */
    /* +Rahul 16-02-2010 */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_REGISTER_CLIENT),/* Client -> Server */
    /* + PROD_TEST */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_PRODUCTION_TEST_START),
    /* - PROD_TEST */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_MOBILE_INFO_REQ),

    ADD_ENUM_AS_STRING(SOCKET_AGPS_SET_LOGGING_CONFIG),
    ADD_ENUM_AS_STRING(SOCKET_AGPS_NMEA_DATA),
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SET_PLATFORM_CONFIG), /* Client -> Server */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_DEBUG_LOG_START),/* Client -> Server */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_DEBUG_LOG_RESP),/* Client -> Server */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_NI_NOTIFICATION),
    ADD_ENUM_AS_STRING(SOCKET_AGPS_PRODUCTION_TEST_START_REQ), /* Client -> Server #67 */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_PRODUCTION_TEST_STOP_IND), /* Server < -> Client #68 */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_REGISTER_EE_CLIENT_CAPABILITIES),/* Client -> Server */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_EE_GET_EPHEMERIS_IND), /* Server < -> Client */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_EE_GET_EPHEMERIS_RSP),  /* Client -> Server */
    ADD_ENUM_AS_STRING(SOCKET_AGPS_EE_GET_REFLOCATION_IND),
    ADD_ENUM_AS_STRING(SOCKET_AGPS_EE_GET_REFLOCATION_RSP),
    ADD_ENUM_AS_STRING(SOCKET_AGPS_EE_WRITE_BROADCASTEPHEMERIS_REQ),
    ADD_ENUM_AS_STRING(SOCKET_AGPS_EE_WRITE_BROADCASTEPHEMERIS_CNF),
    ADD_ENUM_AS_STRING(SOCKET_AGPS_EE_MSL_SET_GPSTIME_IND),
    ADD_ENUM_AS_STRING(SOCKET_AGPS_EE_MSL_SET_GPSTIME_REQ),
    ADD_ENUM_AS_STRING(SOCKET_AGPS_EE_MSL_SET_GPSTIME_CNF),
    ADD_ENUM_AS_STRING(SOCKET_AGPS_SET_REFERENCE_TIME_IND),
    ADD_ENUM_AS_STRING(SOCKET_AGPS_GET_REFERENCE_TIME_REQ),
    ADD_ENUM_AS_STRING(SOCKET_AGPS_DELETE_SEED_REQ),
    ADD_ENUM_AS_STRING(SOCKET_AGPS_EE_SET_BCEDATA_IND),
    {-1,NULL}
};

#define GET_SOCKET_ENUM_AS_STRING( enum )   GetEnumAsString( (int)enum , a_SocketCommandEnumAsString )

char* GetEnumAsString( int v_Enum , t_agps_enum_as_string *p_Table )
{
    uint16_t vl_Index = 0;

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



/*-----------------------------------------------*/
/*  CLIENT TABLE                                 */
/*-----------------------------------------------*/

//Searches for a client with a specific fd is present in client list
//Parameter: fd :- File descriptor
//Returns  : pointer to a client if found, else null
t_agps_Client  *FindClient( int fd )
{
    t_agps_Client  *temp;

    if( clients == NULL )
       return NULL;
    if( clients->fd == fd )
       return clients;
    temp = clients;
    while( temp->next != NULL )
    {
        temp = temp->next;
        if ( temp->fd == fd )
            return temp;
    }
    return NULL;
}

//Searches for a client with supported mask is present in client list
//Parameter: mask :- e_agps_RegisterClientMask
//Returns  : pointer to a client if found, else null
t_agps_Client  *FindClientWithMask( e_agps_RegisterClientMask mask)
{
    t_agps_Client  *temp;

    if( clients == NULL )
       return NULL;
    DEBUG_LOG_PRINT_LEV2(("Client fd %d , capability mask %x\n",clients->fd ,clients->capabilityMask));
    if( clients->capabilityMask & mask )
       return clients;
    temp = clients;
    while( temp->next != NULL )
    {
        temp = temp->next;
        DEBUG_LOG_PRINT_LEV2(("Client fd %d , capability mask %x\n",temp->fd ,temp->capabilityMask));
        if ( temp->capabilityMask & mask )
            return temp;
    }
    return NULL;
}

//Adds a client with file descriptor fd
//parameter: fd :- File descriptor
void AddClient( int fd )
{
    t_agps_Client  *newNode, *temp;
    newNode = (t_agps_Client  *)malloc(sizeof(*temp ));
    newNode->fd = fd;
    newNode->bufferToSend = malloc(BUFFER_SIZE);
    newNode->sendBufferSize = 0;
    newNode->next = NULL;
    if(clients == NULL)
       clients = newNode;
    else
    {
        temp = clients;
        while(temp->next != NULL)
            temp = temp->next;
        temp->next = newNode;
    }
}


//Deletes the client with fd from the client list
//Parameter: fd :- File descriptor
void DeleteClient( int fd )
{
    t_agps_Client  *temp, *toDelete;
    if(clients == NULL)
        return;
    else if(clients->next == NULL)
    {
        if(clients->fd == fd)
        {
            free( clients );
            clients = NULL;
        }
    }
    else
    {
        if(clients->fd == fd)
        {
            toDelete = clients;
            clients = toDelete->next;

            free( toDelete );
            toDelete = NULL;
        }
        else
        {
            temp = clients;
            while(temp->next != NULL)
            {
                if(temp->next->fd == fd)
                {
                    toDelete = temp->next;
                    temp->next = toDelete->next;

                    free( toDelete );
                    toDelete = NULL;
                }
                /*FIDO 373508: Traversing the node while deleting mutiple clients  */
                else
                    temp = temp->next;
                /*FIDO 373508: Traversing the node while deleting mutiple clients  */
            }
        }
    }
}

static int SendCommand( t_agps_CommandType type , void* p_Data , int length , int fd )
{
    t_agps_Command commandToSend;
    t_agps_Packet packet;

    t_Buffer   v_WriteBuffer;
    commandToSend.type = type;

    if( ( length != 0 ) && ( p_Data != NULL ) )
    {
        packet.pData = p_Data;
        packet.length = length;
    }
    else
    {
        packet.pData = NULL;
        packet.length = 0;
    }
    commandToSend.data = (void *)(&packet);

    if( agpscodec_EncodeCommand(&commandToSend, &v_WriteBuffer ) )
    {
        DEBUG_LOG_PRINT_LEV2(("Command : %s , Payload Len : %d , buffer size %d , fd  %d\n", GetEnumAsString(type,a_SocketCommandEnumAsString)  , length , v_WriteBuffer.v_Length , fd));
        send(fd, v_WriteBuffer.p_Buffer, v_WriteBuffer.v_Length, 0);

        if(v_WriteBuffer.p_Buffer != NULL)
        {
            free( v_WriteBuffer.p_Buffer );
            v_WriteBuffer.p_Buffer = NULL;
        }
        return TRUE;
    }
    else
    {
        DEBUG_LOG_PRINT_LEV2(("Encode Command Failed!"));
        return FALSE;
    }
}

static int SendCommandToClientCapability(
    t_agps_CommandType type ,
    void* p_Data ,
    int length ,
    t_agps_RegisterClientMask mask )
{
    int fd;
    int retVal;
    t_agps_Client *p_Client;

    p_Client = FindClientWithMask( mask );

    if( NULL !=  p_Client )
    {
        fd = p_Client->fd;

        if( fd >= 0 )
        {
            retVal = SendCommand( type , p_Data , length , fd );
        }
        else
        {
            DEBUG_LOG_PRINT_LEV2(("ERROR : No Client found with capability %x\n" , mask ));
            retVal = FALSE;
        }
    }
    else
    {
        DEBUG_LOG_PRINT_LEV2(("ERROR : No Client found with capability %x\n" , mask ));
        retVal = FALSE;
    }

    return retVal;
}


/*-------------------------------------------------------*/
/*               CALLBACK FUNCTIONS                                                */
/*-------------------------------------------------------*/

//All the navdata callback functions will call this common function to send the data back to the socket
void CallbackFromCgps( t_cgps_NavData *pCgpsNavData, int fd )
{

    DEBUG_LOG_PRINT_LEV2(("CallbackFromCgps sent. %d, Length%d\n",pCgpsNavData->v_Type, pCgpsNavData->v_Length));

    if(pCgpsNavData->p_NavData == NULL || pCgpsNavData->v_Length == 0)
    {
        DEBUG_LOG_PRINT_LEV2(("CallbackFromCgps: No valid data\n"));
        return;
    }

    if( pCgpsNavData->v_Type == K_AGPS_NMEA )
    {
        SendCommand(SOCKET_AGPS_NMEA_DATA, pCgpsNavData->p_NavData, pCgpsNavData->v_Length, fd);

    }
    else if( pCgpsNavData->v_Type == K_AGPS_C_STRUCT )
    {
        SendCommand(SOCKET_AGPS_NAVDATA,pCgpsNavData->p_NavData,pCgpsNavData->v_Length,fd);
    }
}

//Callback functions

void CallbackFromCgps0( t_cgps_NavData pNavData )
{
    CallbackFromCgps( &pNavData, gpsClientCallbackMap[0].fd );
}

void CallbackFromCgps1( t_cgps_NavData pNavData )
{
    CallbackFromCgps( &pNavData, gpsClientCallbackMap[1].fd );
}

void CallbackFromCgps2( t_cgps_NavData pNavData )
{
    CallbackFromCgps( &pNavData, gpsClientCallbackMap[2].fd );
}

void CallbackFromCgps3( t_cgps_NavData pNavData )
{
    CallbackFromCgps( &pNavData, gpsClientCallbackMap[3].fd );
}

void CallbackFromCgps4( t_cgps_NavData pNavData )
{
    CallbackFromCgps( &pNavData, gpsClientCallbackMap[4].fd );
}

void CallbackFromCgps5( t_cgps_NavData pNavData )
{
    CallbackFromCgps( &pNavData, gpsClientCallbackMap[5].fd );
}

void CallbackFromCgps6( t_cgps_NavData pNavData )
{
    CallbackFromCgps( &pNavData, gpsClientCallbackMap[6].fd );
}

void CallbackFromCgps7( t_cgps_NavData pNavData )
{
    CallbackFromCgps( &pNavData, gpsClientCallbackMap[7].fd );
}

void CallbackFromCgps8( t_cgps_NavData pNavData )
{
    CallbackFromCgps( &pNavData, gpsClientCallbackMap[8].fd );
}

void CallbackFromCgps9( t_cgps_NavData pNavData )
{
    CallbackFromCgps( &pNavData, gpsClientCallbackMap[9].fd );
}

void CallbackFromCgps10( t_cgps_NavData pNavData )
{
    CallbackFromCgps( &pNavData, gpsClientCallbackMap[10].fd );
}

void CallbackDebugLog( t_cgps_NavData pCgpsNavData )
{
    if(pCgpsNavData.p_NavData == NULL || pCgpsNavData.v_Length == 0)
    {
        DEBUG_LOG_PRINT_LEV2(("CallbackDebugLog: No valid data\n"));
        return;
    }

    if(pCgpsNavData.v_Type != K_CGPS_NMEA)
    {
        DEBUG_LOG_PRINT_LEV2(("CallbackDebugLog: Unexpected Data type %d\n",pCgpsNavData.v_Type));
        return;
    }

    SendCommand(SOCKET_AGPS_DEBUG_LOG_RESP, pCgpsNavData.p_NavData, pCgpsNavData.v_Length, DebugLogFd);

}

//Callback functions array
t_cgps_NavigationCallback callbackFunctions[MAX_GPS_CLIENT] =
                                                       {
                                                        CallbackFromCgps0,
                                                        CallbackFromCgps1,
                                                        CallbackFromCgps2,
                                                        CallbackFromCgps3,
                                                        CallbackFromCgps4,
                                                        CallbackFromCgps5,
                                                        CallbackFromCgps6,
                                                        CallbackFromCgps7,
                                                        CallbackFromCgps8,
                                                        CallbackFromCgps9,
                                                        CallbackFromCgps10
                                                       };


void AgpsNotificationCallback(t_agps_NotificationInd *notification)
{
    t_agps_NotificationInd *pl_agps_NotificationInd=NULL;

    SendCommandToClientCapability(SOCKET_AGPS_NOTIFICATION, (void *)notification, sizeof(*pl_agps_NotificationInd), K_AGPS_REGISTER_NOTIFICATION_CLIENT);
}


/*-------------------------------------------------------*/
/*  MAP BETWEEN CALLBACK FUNCTION AND FILE DESCRIPTORS   */
/*-------------------------------------------------------*/

int FindFreeCallbackFunction()
{
    int index, result = -1;

    for( index = 0; index < MAX_GPS_CLIENT ; ++index )
    {
        if( gpsClientCallbackMap[index].status == 0 )
        {
            result = index;
            break;
        }
    }
    return result;
}

int MapCallbackFunction(int fd)
{
    int index = FindFreeCallbackFunction();
    if( index >= 0 )
    {
        gpsClientCallbackMap[index].status = 1;
        gpsClientCallbackMap[index].fd = fd;
    }
    return index;
}

void UnMapCallbackFunction(int fd)
{
    int index;
    for( index = 0; index < MAX_GPS_CLIENT; ++index )
    {
        if( gpsClientCallbackMap[index].fd == fd )
            gpsClientCallbackMap[index].status = 0;
    }
}

int FindMappedFd( int fd )
{
    int index, result = -1;
    for( index = 0; index < MAX_GPS_CLIENT; ++index)
    {
        if( gpsClientCallbackMap[index].status != 0 && gpsClientCallbackMap[index].fd == fd )
        {
            result = index;
            break;
        }
    }
    return result;
}
e_agps_Status SendFenceGeographicalAreaReq( uint32_t                     vp_Handle,
                                                 t_cgps_NavigationCallback    vp_Callback,
                                                 uint8_t                      vp_OutputType,
                                                 uint16_t                     vp_NmeaMask,
                                                 t_agps_PositioningParams*    pp_PositioningParams,
                                                 t_agps_GeographicTargetArea* pp_TargetArea,
                                                 uint16_t                     vp_SpeedThreshold,
                                                 e_agps_AreaEventType         vp_AreaEventType
                                               )
{
    uint8_t vl_NumOfElem;
    t_cgps_PositioningParams     vl_PositioningParams;
    t_cgps_GeographicTargetArea  vl_TargetArea;

    memcpy(&vl_PositioningParams, pp_PositioningParams, sizeof(t_cgps_PositioningParams)  );

    vl_TargetArea.v_ShapeType           = pp_TargetArea->v_ShapeType;
    vl_TargetArea.v_NumberOfPoints      = pp_TargetArea->v_NumberOfPoints;
    vl_TargetArea.v_EllipSemiMinorAngle = pp_TargetArea->v_EllipSemiMinorAngle;
    vl_TargetArea.v_CircularRadius      = pp_TargetArea->v_CircularRadius;
    vl_TargetArea.v_CircularRadiusMin   = pp_TargetArea->v_CircularRadiusMin;
    vl_TargetArea.v_CircularRadiusMax   = pp_TargetArea->v_CircularRadiusMax;
    vl_TargetArea.v_EllipSemiMajor      = pp_TargetArea->v_EllipSemiMajor;
    vl_TargetArea.v_EllipSemiMajorMin   = pp_TargetArea->v_EllipSemiMajorMin;
    vl_TargetArea.v_EllipSemiMajorMax   = pp_TargetArea->v_EllipSemiMajorMax;
    vl_TargetArea.v_EllipSemiMinor      = pp_TargetArea->v_EllipSemiMinor;
    vl_TargetArea.v_EllipSemiMinorMin   = pp_TargetArea->v_EllipSemiMinorMin;
    vl_TargetArea.v_EllipSemiMinorMax   = pp_TargetArea->v_EllipSemiMinorMax;

    vl_TargetArea.v_PolygonHysteresis   = pp_TargetArea->v_PolygonHysteresis;

    vl_NumOfElem = ((vl_TargetArea.v_ShapeType==K_CGPS_POLYGON_AREA)?pp_TargetArea->v_NumberOfPoints:1);

    memcpy(&vl_TargetArea.v_LatSign,   pp_TargetArea->v_LatSign,   sizeof(uint8_t) *vl_NumOfElem);
    memcpy(&vl_TargetArea.v_Latitude,  pp_TargetArea->v_Latitude,  sizeof(uint32_t)*vl_NumOfElem);
    memcpy(&vl_TargetArea.v_Longitude, pp_TargetArea->v_Longitude, sizeof(int32_t) *vl_NumOfElem);

    return MC_CGPS_REGISTER_FENCE_GEO_TARGET_AREA(vp_Handle,
                                                  (t_cgps_NavigationCallback)vp_Callback,
                                                  vp_OutputType,
                                                  vp_NmeaMask,
                                                  vl_PositioningParams,
                                                  &vl_TargetArea,
                                                  vp_SpeedThreshold,
                                                  vp_AreaEventType);
}


#ifdef AGPS_EE_FTR

void EEClientCallback(t_gnsEE_ExtendedEphInd  *extendedeph)

{
    t_agps_ExtendedEphInd vl_agps_ExtendedEphInd;

    DEBUG_LOG_PRINT(("EEClientCallback Entry" ));

    /*NOTE:-
        AS of now MSL  is registring with the  the Mask : 1664  i.e 11010000000
        it means:-
        K_AGPS_REGISTER_REFPOS_CLIENT   Set
        K_AGPS_REGISTER_EXTENDED_EPHEMERIS_CLIENT unset
        K_AGPS_REGISTER_SAEE_CLIENT Set
        K_AGPS_REGISTER_BCE_CLIENT  Set

        This is to be corrected from MSL side.
        Till this correction is received, we will keep the capability as K_AGPS_REGISTER_BCE_CLIENT in SendCommandToClientCapability at EEClientCallback
        To be changed to K_AGPS_REGISTER_EXTENDED_EPHEMERIS_CLIENT later on
        */
    if( NULL != extendedeph)
    {
        vl_agps_ExtendedEphInd.v_ConstelType  = extendedeph->v_ConstelType;
        vl_agps_ExtendedEphInd.v_EERefGPSTime = extendedeph->v_EERefGPSTime;
        vl_agps_ExtendedEphInd.v_PrnBitMask   = extendedeph->v_PrnBitMask;
        SendCommandToClientCapability(SOCKET_AGPS_EE_GET_EPHEMERIS_IND, (void*)&vl_agps_ExtendedEphInd, sizeof(t_agps_ExtendedEphInd),K_AGPS_REGISTER_BCE_CLIENT);
    }
    DEBUG_LOG_PRINT(("EEClientCallback Exit" ));
}


void EEClientGetRefLocCallback()
{
    DEBUG_LOG_PRINT(("EEClientGetRefLocCallback Entry" ));

    SendCommandToClientCapability(SOCKET_AGPS_EE_GET_REFLOCATION_IND,NULL,0,K_AGPS_REGISTER_REFPOS_CLIENT);

}



void EEClientGetRefTimeCallback()
{
    DEBUG_LOG_PRINT(("EEClientGetRefTimeCallback Entry" ));

    SendCommandToClientCapability(SOCKET_AGPS_GET_REFERENCE_TIME_REQ,NULL,0,K_AGPS_REGISTER_REFPOS_CLIENT);

}


void EEClientDeleteSeedCallback()
{
    DEBUG_LOG_PRINT(("EEClientDeleteSeedCallback Entry" ));

    SendCommandToClientCapability(SOCKET_AGPS_DELETE_SEED_REQ,NULL,0,K_AGPS_REGISTER_REFPOS_CLIENT);

}


void EEClientBceDataCallback(t_gnsEE_NavDataBCE *p_BCE)
{
    DEBUG_LOG_PRINT(("EEClientBceDataCallback Entry" ));
    SendCommandToClientCapability(SOCKET_AGPS_EE_SET_BCEDATA_IND, (void *)p_BCE, sizeof(t_agps_NavDataBCE),K_AGPS_REGISTER_BCE_CLIENT);

}


e_agps_Status EEGetRefLocationRsp(t_agps_RefPosition * RefPosition)
{
    t_gnsEE_RefPosition v_RefPosition;
    memset( &v_RefPosition , 0 , sizeof( v_RefPosition ) );
    memcpy( &v_RefPosition , RefPosition , sizeof (t_agps_RefPosition));
    DEBUG_LOG_PRINT_LEV2(("EEGetRefLocationRsp"));

    GNS_EEGetRefLocationRsp(&v_RefPosition);
    return SOCKET_AGPS_NO_ERROR;

}


e_agps_Status EEGetEphemerisRsp( t_agps_NavDataList *p_NavDataList )
{
    s_gnsEE_NavDataList v_NavDataList;

    memset( &v_NavDataList , 0 , sizeof( v_NavDataList ) );
    memcpy( &v_NavDataList , p_NavDataList , sizeof (s_gnsEE_NavDataList));

    DEBUG_LOG_PRINT(("EEGetEphemerisRsp v_numEntriesGPS = %d,v_NumEntriesGlonass =%d ",v_NavDataList.v_NumEntriesGps,v_NavDataList.v_NumEntriesGlonass));
    GNS_EEGetEphemerisRsp(&v_NavDataList);
    return SOCKET_AGPS_NO_ERROR;
}


e_agps_Status RegisterEEClientCallback(t_GnsEEClientCallback v_Callback)
{

    DEBUG_LOG_PRINT(("RegisterEEClientCallback Entry"));
    if( v_Callback != NULL )
    {
        GNS_ExtendedEphemerisRegister(v_Callback);
        return SOCKET_AGPS_NO_ERROR;
    }
    else
        return SOCKET_AGPS_INVALID_PARAMETER;


}


e_agps_Status RegisterEEClientRefLocationCallback(t_agps_EEClientRefLocationCallback v_Callback)
{
    if( v_Callback != NULL )
    {
        GNS_ReferencePositionRegister(v_Callback);
        return SOCKET_AGPS_NO_ERROR;
    }
    else
    {
        return SOCKET_AGPS_INVALID_PARAMETER;
    }


}


e_agps_Status RegisterEEClientRefTimeCallback(t_agps_EEClientGetRefTimeCallback v_Callback)
{
    if( v_Callback != NULL )
    {
        GNS_ReferenceTimeRegister(v_Callback);
        return SOCKET_AGPS_NO_ERROR;
    }
    else
    {
        return SOCKET_AGPS_INVALID_PARAMETER;
    }


}

e_agps_Status RegisterEEClientDeleteSeedCallback(t_agps_EEClientDeleteSeedCallback v_Callback)
{
    if( v_Callback != NULL )
    {
        GNS_DeleteSeedRegister(v_Callback);
        return SOCKET_AGPS_NO_ERROR;
    }
    else
    {
        return SOCKET_AGPS_INVALID_PARAMETER;
    }


}

e_agps_Status RegisterEEClientBCEDataCallback(t_agps_EEClientBCECallback v_Callback)
{
    if( v_Callback != NULL )
    {
        GNS_BCEDataRegister(v_Callback);
        return SOCKET_AGPS_NO_ERROR;
    }
    else
    {
        return SOCKET_AGPS_INVALID_PARAMETER;
    }


}

#endif



#ifdef AGPS_GENERIC_SOLUTION_FTR

t_agps_NotificationCallback notification_cb = NULL;

void GetFormattedNotificationParams(char * dest, char * src, int type, int len)
{
    memcpy(dest + 1, src, len);
    if( type == 2 || type == 3 || type == 5)
        dest[0] = strlen((const char*)dest + 1);
    else
        dest[0] = len;
}

/**
* \brief Gets Gps Configuration
*/
e_agps_Status GetGpsConfiguration()
{
    return SOCKET_AGPS_NO_ERROR;
}

/**
*
*/
#ifdef AGPS_UP_FTR



e_agps_Status StartBearerCnf()
{
    GNS_SuplEstablishBearerCnf();
    return SOCKET_AGPS_NO_ERROR;
}


/**
*
*/
e_agps_Status StartBearerErr()
{
    GNS_SuplEstablishBearerErr();
    return SOCKET_AGPS_NO_ERROR;
}


/**
*
*/
e_agps_Status CloseBearerInd()
{
    GNS_SuplCloseBearerInd();
    return SOCKET_AGPS_NO_ERROR;
}


/**
*
*/
e_agps_Status CloseBearerCnf()
{
    GNS_SuplCloseBearerCnf();
    return SOCKET_AGPS_NO_ERROR;
}


/**
*
*/
e_agps_Status CloseBearerErr()
{
    GNS_SuplCloseBearerErr();
    return SOCKET_AGPS_NO_ERROR;
}


/**
*
*/
e_agps_Status SuplTcpIpConnectCnf( t_agps_TcpIpConnectCnf *p_ConnectCnf )
{
    GNS_SuplTcpIpConnectCnf((t_GnsConnectionHandle) (p_ConnectCnf->v_ConnectionHandle) );
    return SOCKET_AGPS_NO_ERROR;
}

/**
*
*/
e_agps_Status SuplTcpIpConnectErr( t_agps_TcpIpConnectErr *p_ConnectErr )
{
    GNS_SuplTcpIpConnectErr((t_GnsConnectionHandle) (p_ConnectErr->v_ConnectionHandle),(int)p_ConnectErr->v_ErrorType );
    return SOCKET_AGPS_NO_ERROR;
}

/**
*
*/
e_agps_Status SuplTcpIpDisconnectCnf( t_agps_TcpIpDisconnectCnf *p_DisconnectCnf )
{
    GNS_SuplTcpIpDisconnectCnf((t_GnsConnectionHandle) (p_DisconnectCnf->v_ConnectionHandle) );
    return SOCKET_AGPS_NO_ERROR;
}


/**
*
*/
e_agps_Status SuplTcpIpDisconnectInd( t_agps_TcpIpDisconnectInd *p_DisconnectInd )
{
    GNS_SuplTcpIpDisconnectInd((t_GnsConnectionHandle) (p_DisconnectInd->v_ConnectionHandle) );
    return SOCKET_AGPS_NO_ERROR;
}

/**
*
*/
e_agps_Status SuplTcpIpDisconnectErr( t_agps_TcpIpDisconnectErr *p_DisconnectErr )
{
//    GNS_SuplTcpIpDisconnectErr((t_GnsConnectionHandle) (p_DisconnectErr->v_ConnectionHandle) );
    p_DisconnectErr = p_DisconnectErr;
    return SOCKET_AGPS_NO_ERROR;
}

/**
*
*/
e_agps_Status SuplMobileInfoInd( t_agps_MobileInfoInd *p_mobileInfoInd )
{
    s_gnsSUPL_MobileInfo v_MobileInfo;
    memset( &v_MobileInfo , 0 , sizeof( v_MobileInfo ) );
    /* @todo : Convert the mobile info to gns form */
    memcpy( v_MobileInfo.a_IMSI , p_mobileInfoInd->a_IMSI , strlen( (const char*)p_mobileInfoInd->a_IMSI ) + 1 );
    memcpy( v_MobileInfo.a_MSISDN, p_mobileInfoInd->a_MSISDN, strlen( (const char*)p_mobileInfoInd->a_MSISDN) + 1 );

    DEBUG_LOG_PRINT_LEV2(("IMSI : %s , MSISDN : %s, cellType : %x",
        v_MobileInfo.a_IMSI,v_MobileInfo.a_MSISDN , p_mobileInfoInd->v_CellType));

    switch( p_mobileInfoInd->v_CellType )
    {
        case 0: // GSM
            v_MobileInfo.v_CellInfoType = E_gnsSUPL_GSM_CELL;
            v_MobileInfo.v_CellInfo.v_GsmCellInfo.v_MCC = p_mobileInfoInd->v_MCC;
            v_MobileInfo.v_CellInfo.v_GsmCellInfo.v_MNC = p_mobileInfoInd->v_MNC;
            v_MobileInfo.v_CellInfo.v_GsmCellInfo.v_CellIdentity = p_mobileInfoInd->v_CellIdentity;
            /* ++ LMSqc36741 */
            /* v_MobileInfo.v_CellInfo.v_GsmCellInfo.v_LAC = K_gnsUNKNOWN_U32; */
            v_MobileInfo.v_CellInfo.v_GsmCellInfo.v_LAC = p_mobileInfoInd->v_Lac;
            /* -- LMSqc36741 */
            v_MobileInfo.v_CellInfo.v_GsmCellInfo.v_TimingAdvance = K_gnsUNKNOWN_U16;
            v_MobileInfo.v_CellInfo.v_GsmCellInfo.v_NumNeighbouringCells = 0;
            break;
        case 1: // UMTS
            v_MobileInfo.v_CellInfoType = E_gnsSUPL_WCDMA_FDD_CELL;
            v_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.v_MCC = p_mobileInfoInd->v_MCC;
            v_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.v_MNC = p_mobileInfoInd->v_MNC;
            /* +RRR : Cell ID was not being used. Now correct */
            /* v_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.v_CellIdentity = 1; //p_mobileInfoInd->v_CellIdentity; */
            v_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.v_CellIdentity = p_mobileInfoInd->v_CellIdentity;
            /* -RRR : Cell ID was not being used. Now correct */
            v_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.v_NumNeighbouringCells = 0;
            v_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.v_Uarfcn_DL = K_gnsUNKNOWN_U16;
            v_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.v_Uarfcn_UL = K_gnsUNKNOWN_U16;
            v_MobileInfo.v_CellInfo.v_WcdmaFDDCellInfo.v_PrimaryScramblingCode = K_gnsUNKNOWN_U16;
            break;

        case 2: // UMTS TDD  Raghav
            v_MobileInfo.v_CellInfoType = E_gnsSUPL_WCDMA_TDD_CELL;
            v_MobileInfo.v_CellInfo.v_WcdmaTDDCellInfo.v_MCC = p_mobileInfoInd->v_MCC;
            v_MobileInfo.v_CellInfo.v_WcdmaTDDCellInfo.v_MNC = p_mobileInfoInd->v_MNC;
            v_MobileInfo.v_CellInfo.v_WcdmaTDDCellInfo.v_CellParameterID = K_gnsUNKNOWN_U16;
            v_MobileInfo.v_CellInfo.v_WcdmaTDDCellInfo.v_CellIdentity = p_mobileInfoInd->v_CellIdentity;
            v_MobileInfo.v_CellInfo.v_WcdmaTDDCellInfo.v_NumNeighbouringCells = 0;
            v_MobileInfo.v_CellInfo.v_WcdmaTDDCellInfo.v_Uarfcn_Nt = K_gnsUNKNOWN_U16;

            break;
    /* +RRR */
    case (uint8_t)(-1) : // No Cell Info. Possibly IMSI/MSISDN
       if( !(p_mobileInfoInd->a_IMSI[0] | p_mobileInfoInd->a_MSISDN[0] ) )
           return SOCKET_AGPS_INVALID_PARAMETER;
       break;
    /* -RRR */
        default:

            return SOCKET_AGPS_INVALID_PARAMETER;
    }

    GNS_SuplMobileInfoInd(&v_MobileInfo);
    return SOCKET_AGPS_NO_ERROR;
}

/**
*
*/
e_agps_Status WLANInfoInd( t_agps_WLANInfoInd *p_wlanInfoInd )
{
    s_gnsWIRELESS_NetworkInfo v_WirelessNetworkInfo;
    memset( &v_WirelessNetworkInfo , 0 , sizeof( v_WirelessNetworkInfo ) );


    if(p_wlanInfoInd != NULL)
    {
        v_WirelessNetworkInfo.v_AccessPointType                                         = E_gnsWIRELESS_WLAN;
        v_WirelessNetworkInfo.v_AccessPointStatus                                       = E_gnsWIRELESS_ACCESSPOINT_CURRENT;

        v_WirelessNetworkInfo.v_AccessPointInfo.v_WlanApInfo.v_WLAN_AP_MS_Addr          = p_wlanInfoInd->v_WLAN_AP_MS_Addr;
        v_WirelessNetworkInfo.v_AccessPointInfo.v_WlanApInfo.v_WLAN_AP_LS_Addr          = p_wlanInfoInd->v_WLAN_AP_LS_Addr;

        v_WirelessNetworkInfo.v_AccessPointInfo.v_WlanApInfo.v_WLAN_AP_TransmitPower    = p_wlanInfoInd->v_WLAN_AP_TransmitPower;
        v_WirelessNetworkInfo.v_AccessPointInfo.v_WlanApInfo.v_WLAN_AP_AntennaGain      = p_wlanInfoInd->v_WLAN_AP_AntennaGain;

        v_WirelessNetworkInfo.v_AccessPointInfo.v_WlanApInfo.v_WLAN_AP_SNR              = p_wlanInfoInd->v_WLAN_AP_SNR;
        v_WirelessNetworkInfo.v_AccessPointInfo.v_WlanApInfo.v_WLAN_AP_DeviceType       = p_wlanInfoInd->v_WLAN_AP_DeviceType;

        v_WirelessNetworkInfo.v_AccessPointInfo.v_WlanApInfo.v_WLAN_AP_SignalStrength   = p_wlanInfoInd->v_WLAN_AP_SignalStrength;
        v_WirelessNetworkInfo.v_AccessPointInfo.v_WlanApInfo.v_WLAN_AP_Channel          = p_wlanInfoInd->v_WLAN_AP_Channel;

        v_WirelessNetworkInfo.v_AccessPointInfo.v_WlanApInfo.v_WLAN_AP_RTDValue         = p_wlanInfoInd->v_WLAN_AP_RTDValue;
        v_WirelessNetworkInfo.v_AccessPointInfo.v_WlanApInfo.v_WLAN_AP_RTDUnits         = p_wlanInfoInd->v_WLAN_AP_RTDUnits;

        v_WirelessNetworkInfo.v_AccessPointInfo.v_WlanApInfo.v_WLAN_AP_Accuracy         = p_wlanInfoInd->v_WLAN_AP_Accuracy;
        v_WirelessNetworkInfo.v_AccessPointInfo.v_WlanApInfo.v_WLAN_AP_SETTransmitPower = p_wlanInfoInd->v_WLAN_AP_SETTransmitPower;

        v_WirelessNetworkInfo.v_AccessPointInfo.v_WlanApInfo.v_WLAN_AP_SETAntennaGain   = p_wlanInfoInd->v_WLAN_AP_SETAntennaGain;
        v_WirelessNetworkInfo.v_AccessPointInfo.v_WlanApInfo.v_WLAN_AP_SETSNR           = p_wlanInfoInd->v_WLAN_AP_SETSNR;

        v_WirelessNetworkInfo.v_AccessPointInfo.v_WlanApInfo.v_WLAN_AP_SETSignalStrength= p_wlanInfoInd->v_WLAN_AP_SETSignalStrength;
        DEBUG_LOG_PRINT_LEV2(("WLANInfoInd : v_WirelessNetworkInfo.v_AccessPointInfo.v_WlanApInfo.v_WLAN_AP_LS_Addr : %d",
        v_WirelessNetworkInfo.v_AccessPointInfo.v_WlanApInfo.v_WLAN_AP_LS_Addr));

        GNS_WirelessNetworkInfoInd(&v_WirelessNetworkInfo);
        return SOCKET_AGPS_NO_ERROR;
    }
    else
    {
        DEBUG_LOG_PRINT_LEV2(("No WLAN Info Present\n" ));
        /*Need to check if the return value is valid or not */
        return SOCKET_AGPS_INVALID_PARAMETER;
    }

}




/**
*
*/
e_agps_Status SuplSmsPushInd ( t_agps_SmsPushInd *p_SmsPushInd )
{
    GNS_SuplSmsPushInd(  p_SmsPushInd->v_SmsPushInd.p_Pointer,
                         p_SmsPushInd->v_SmsPushInd.v_Length,
                         p_SmsPushInd->v_Hash.p_Pointer,
                         p_SmsPushInd->v_Hash.v_Length);
    return SOCKET_AGPS_NO_ERROR;
}


/**
*
*/
e_agps_Status SuplSendDataCnf( t_agps_SuplSendDataCnf *p_SuplSendDataCnf )
{
    GNS_SuplSendDataCnf( (t_GnsConnectionHandle)( p_SuplSendDataCnf->v_ConnectionHandle) );
    return SOCKET_AGPS_NO_ERROR;
}

/**
*
*/
e_agps_Status SuplReceiveDataInd( t_agps_SuplData *p_SuplData )
{
    GNS_SuplReceiveDataInd(p_SuplData->v_ConnectionHandle,p_SuplData->v_Data.p_Pointer , p_SuplData->v_Data.v_Length );
    return SOCKET_AGPS_NO_ERROR;
}

void __GnsSuplCallback( e_gnsSUPL_MsgType v_Type , uint32_t v_GnsMsgLen , u_gnsSUPL_MsgData *v_GnsMsgData )
{

    v_GnsMsgLen = v_GnsMsgLen;

    if (NULL == v_GnsMsgData)
        return;

    switch( v_Type )
    {
        case E_gnsSUPL_CONNECT_REQ:
        {
            t_agps_TcpIpConnectReq v_ConnectReq;

            v_ConnectReq.v_ConnectionHandle = v_GnsMsgData->v_GnsSuplConnectReq.v_Handle;
            if(v_GnsMsgData->v_GnsSuplConnectReq.v_SocketType == E_gnsSUPL_SOCKETTYPE_TCP)
               v_ConnectReq.v_IsTls = FALSE;
            else  //All other casee TLS will be enabled.
            v_ConnectReq.v_IsTls = TRUE;
            memcpy( v_ConnectReq.slpAddress.address ,
                    v_GnsMsgData->v_GnsSuplConnectReq.p_ServerAddr ,
                    strlen( (const char*)v_GnsMsgData->v_GnsSuplConnectReq.p_ServerAddr ) + 1 );

            v_ConnectReq.slpAddress.portNumber = v_GnsMsgData->v_GnsSuplConnectReq.v_ServerPortNum;
            v_ConnectReq.slpAddress.type = v_GnsMsgData->v_GnsSuplConnectReq.v_ServerAddrType;


            if(v_GnsMsgData->v_GnsSuplConnectReq.p_ServerAddr != NULL)
            {
                free( v_GnsMsgData->v_GnsSuplConnectReq.p_ServerAddr );
                v_GnsMsgData->v_GnsSuplConnectReq.p_ServerAddr = NULL;

            }

            SendCommandToClientCapability(
                SOCKET_AGPS_SUPL_TCPIP_CONNECT_REQ ,
                &v_ConnectReq ,
                sizeof( v_ConnectReq ) ,
                K_AGPS_REGISTER_DATA_CONN_CLIENT );

        }
        break;

        case E_gnsSUPL_DISCONNECT_REQ:
        {
            t_agps_TcpIpDisconnectReq v_DisconnectReq;
            v_DisconnectReq.v_ConnectionHandle = v_GnsMsgData->v_GnsSuplDisconnectReq.v_Handle;
            SendCommandToClientCapability(
                SOCKET_AGPS_SUPL_TCPIP_DISCONNECT_REQ ,
                &v_DisconnectReq ,
                sizeof( v_DisconnectReq ) ,
                K_AGPS_REGISTER_DATA_CONN_CLIENT );
        }
        break;

        case E_gnsSUPL_SEND_DATA_REQ:
        {
            t_agps_SuplData v_SuplData;

            v_SuplData.v_ConnectionHandle = v_GnsMsgData->v_GnsSuplSendDataReq.v_Handle;
            v_SuplData.v_Data.p_Pointer = v_GnsMsgData->v_GnsSuplSendDataReq.p_Data;
            v_SuplData.v_Data.v_Length = v_GnsMsgData->v_GnsSuplSendDataReq.v_DataLen;

            SendCommandToClientCapability(
                SOCKET_AGPS_SUPL_SEND_DATA_REQ ,
                &v_SuplData ,
                sizeof( v_SuplData ) ,
                K_AGPS_REGISTER_DATA_CONN_CLIENT );


            if(v_SuplData.v_Data.p_Pointer!= NULL)
            {
                free( v_SuplData.v_Data.p_Pointer );
                v_SuplData.v_Data.p_Pointer = NULL;
            }
        }
        break;
#ifndef GNS_CELL_INFO_FTR

        case E_gnsSUPL_MOBILE_INFO_REQ:
        {
            /* Api below is not defined.
               User can select to be notified on change in specific mobile info
               The possible types are : */
            /* MCC field. */
            /* MNC field. */
            /* LAC field. */
            /* CellIdentity field. */
            /* RNCId field. */
            /* Network Measurements */
            /* MSISDN field. */
            /* IMSI field. */

        /* +RRR */
        SendCommandToClientCapability(
                SOCKET_AGPS_MOBILE_INFO_REQ,
                NULL ,
                0 ,
                K_AGPS_REGISTER_MOBILE_INFO_CLIENT);
        /* -RRR */
        }
        break;
#endif

        case E_gnsSUPL_RECEIVE_DATA_CNF:
        {

        }
        break;

        case E_gnsSUPL_ESTABLISH_BEARER_REQ:
        {
            SendCommandToClientCapability(
                SOCKET_AGPS_START_BEARER_REQ ,
                NULL ,
                0 ,
                K_AGPS_REGISTER_BEARER_CLIENT );
        }
        break;

        case E_gnsSUPL_CLOSE_BEARER_REQ:
        {
            SendCommandToClientCapability(
                SOCKET_AGPS_CLOSE_BEARER_REQ ,
                NULL ,
                0 ,
                K_AGPS_REGISTER_BEARER_CLIENT );
        }
        break;

        default :
            break;
    }

}


void __GnsWirelessInfoCallback( e_gnsWIRELESS_MsgType v_Type , uint32_t v_GnsMsgLen , u_gnsWIRELESS_MsgData *v_GnsMsgData )
{

    v_GnsMsgLen= v_GnsMsgLen;

    if (NULL == v_GnsMsgData)
        return;

    switch( v_Type )
    {
        case E_gnsWIRELESS_INFO_REQ:
        {
            SendCommandToClientCapability(
                SOCKET_AGPS_WIRELESS_NETWORK_INFO_REQ,
                NULL ,
                0 ,
                K_AGPS_REGISTER_WIRELESS_NETWORK_CLIENT);
        }
        break;

        default :
            break;
    }

}



/**
*
*/
e_agps_Status RegisterBearerRequestCallback(t_agps_BearerRequestCallback callback)
{
    if( callback != NULL )
        return SOCKET_AGPS_NO_ERROR;
    else
        return SOCKET_AGPS_CALLBACK_ALREADY_REGISTERED;
}


/**
*
*/

e_agps_Status RegisterTcpIpConnectionReqCallback(t_agps_TcpIpConnectionReqCallback callback)
{
    if( callback != NULL )
        return SOCKET_AGPS_NO_ERROR;
    else
        return SOCKET_AGPS_CALLBACK_ALREADY_REGISTERED;
}

/**
*
*/
#endif /* AGPS_UP_FTR */

#if defined( AGPS_UP_FTR ) || defined( AGPS_FTR )
e_agps_Status SendUserResponse(uint32_t handle, uint8_t userResponse)
{
    MC_CGPS_NOTIFICATION_USER_ANSWER(userResponse, (void *)handle);
    return SOCKET_AGPS_NO_ERROR;
}

void CgpsNotificationCallback(t_cgps_Notification *agpsNotification)
{
    t_agps_NotificationInd notification;

    DEBUG_LOG_PRINT_LEV2(("CgpsNotificationCallback : v_ClientNameEncodingType = %d v_RequestorIdEncodingType=%d",agpsNotification->v_ClientNameEncodingType,agpsNotification->v_RequestorIdEncodingType ));

    memset(  &notification , 0 , sizeof(notification) );

    if(notification_cb != NULL)
    {
        notification.v_Handle = (uint32_t)agpsNotification->v_Handle;

        GetFormattedNotificationParams((char*)notification.a_ClientName, (char*)agpsNotification->a_ClientName,
            (int)agpsNotification->v_ClientNameEncodingType, (int)agpsNotification->v_ClientNameLen);
        GetFormattedNotificationParams((char*)notification.a_CodeWord, (char*)agpsNotification->a_CodeWord,
            (int)agpsNotification->v_CodeWordAlphabet, (int)agpsNotification->v_CodeWordLen);
        memcpy(notification.a_PhoneNumber, agpsNotification->a_PhoneNumber, K_CGPS_MAX_PHONE_NUM_LENGTH);
        GetFormattedNotificationParams((char*)notification.a_RequestorId, (char*)agpsNotification->a_RequestorId,
            (int)agpsNotification->v_RequestorIdEncodingType, (int)agpsNotification->v_RequestorIdLen);
        notification.v_ClientNameEncodingType = agpsNotification->v_ClientNameEncodingType;
        notification.v_CodeWordEncodingType = agpsNotification->v_CodeWordAlphabet;
        notification.v_LcsServiceTypeId = agpsNotification->v_LcsServiceTypeId;
        notification.v_RequestorIdEncodingType = agpsNotification->v_RequestorIdEncodingType;
        notification.v_TonNpi = agpsNotification->v_TonNpi;
        notification.v_Type = agpsNotification->v_NotificationType;
        notification.v_TonNpiConfig = agpsNotification->v_TonNpiConfig;

        notification_cb(&notification);
    }
}







e_agps_Status RegisterNotificationCallback(t_agps_NotificationCallback callback)
{
    if( callback != NULL )
    {
        notification_cb = callback;
        MC_CGPS_NOTIFICATION_CB_REGISTER( &CgpsNotificationCallback );
        return SOCKET_AGPS_NO_ERROR;
    }
    else
        return SOCKET_AGPS_INVALID_PARAMETER;
}

e_agps_Status SetGpsLocationForward(uint32_t vp_handle, t_agps_LocationForwardCommand *gpsLocFwd)
{

    t_cgps_QoP qop;
    t_cgps_ClientInfo clientInfo;
    e_cgps_LcsServiceType serviceType;


    if( gpsLocFwd == NULL )
    {
        return SOCKET_AGPS_INVALID_PARAMETER;
    }
    else
    {
        strncpy((char*)clientInfo.a_ClientExtId, (const char*)gpsLocFwd->clientInfo.a_ClientExtId, K_CGPS_MAX_REQ_LENGTH);
        strncpy((char*)clientInfo.a_MlcNumber,   (const char*)gpsLocFwd->clientInfo.a_MlcNumber,   K_CGPS_MAX_CLIENT_LENGTH);
        clientInfo.v_ClientExtIdLen         = gpsLocFwd->clientInfo.v_ClientExtIdLen;
        clientInfo.v_ClientIdConfig         = gpsLocFwd->clientInfo.v_ClientIdConfig;
        clientInfo.v_ClientIdTonNpi         = gpsLocFwd->clientInfo.v_ClientIdTonNpi;
        clientInfo.v_MlcNumberLen           = gpsLocFwd->clientInfo.v_MlcNumberLen;
        clientInfo.v_MlcNumConfig           = gpsLocFwd->clientInfo.v_MlcNumConfig;
        clientInfo.v_MlcNumTonNpi           = gpsLocFwd->clientInfo.v_MlcNumTonNpi;
        clientInfo.t_thirdparty_info.v_ThirdPartyId = gpsLocFwd->clientInfo.ThirdPartyInfo.v_ThirdPartyId;
        strncpy((char*)clientInfo.t_thirdparty_info.a_ThirdPartyName,                     \
                (const char*)gpsLocFwd->clientInfo.ThirdPartyInfo.a_ThirdPartyName,       \
                K_CGPS_MAX_THIRDPARTY_NAME_LEN);

        qop.vp_AgeLimit      = gpsLocFwd->qop.vp_AgeLimit;
        qop.vp_HorizAccuracy = gpsLocFwd->qop.vp_HorizAccuracy;
        qop.vp_Timeout       = gpsLocFwd->qop.vp_Timeout;
        qop.vp_VertAccuracy  = gpsLocFwd->qop.vp_VertAccuracy;
    }

    serviceType = (e_cgps_LcsServiceType) gpsLocFwd->serviceType;

    DEBUG_LOG_PRINT_LEV2(("SetGpsLocationForward server1: Thirdparty id = %d\n", gpsLocFwd->clientInfo.ThirdPartyInfo.v_ThirdPartyId));
    DEBUG_LOG_PRINT_LEV2(("SetGpsLocationForward server1: Thirdparty name = %s\n", gpsLocFwd->clientInfo.ThirdPartyInfo.a_ThirdPartyName));

    return MC_CGPS_REGISTER_LOCATION_FORWARD(vp_handle,&clientInfo, serviceType, &qop);

}
#endif /* AGPS_UP_FTR || AGPS_FTR*/
#ifdef AGPS_UP_FTR

e_agps_Status SetGpsLocationRetrieve(uint32_t vp_handle,t_cgps_NavigationCallback    vp_Callback, t_agps_LocationRetrieveCommand *gpsLocRet)
{
    t_cgps_QoP qop;
    t_cgps_ClientInfo clientInfo;

    memcpy(&qop,&gpsLocRet->qop,sizeof(gpsLocRet->qop)  );
    memcpy(&clientInfo,&gpsLocRet->clientInfo,sizeof(gpsLocRet->clientInfo));

    if( gpsLocRet == NULL )
    {
        return SOCKET_AGPS_INVALID_PARAMETER;
    }
    else
    {
        strncpy((char*)clientInfo.a_ClientExtId, (const char*)gpsLocRet->clientInfo.a_ClientExtId, K_CGPS_MAX_REQ_LENGTH);
        strncpy((char*)clientInfo.a_MlcNumber,   (const char*)gpsLocRet->clientInfo.a_MlcNumber,K_CGPS_MAX_CLIENT_LENGTH);
        clientInfo.v_ClientExtIdLen         = gpsLocRet->clientInfo.v_ClientExtIdLen;
        clientInfo.v_ClientIdConfig         = gpsLocRet->clientInfo.v_ClientIdConfig;
        clientInfo.v_ClientIdTonNpi         = gpsLocRet->clientInfo.v_ClientIdTonNpi;
        clientInfo.v_MlcNumberLen           = gpsLocRet->clientInfo.v_MlcNumberLen;
        clientInfo.v_MlcNumConfig           = gpsLocRet->clientInfo.v_MlcNumConfig;
        clientInfo.v_MlcNumTonNpi           = gpsLocRet->clientInfo.v_MlcNumTonNpi;
        clientInfo.t_thirdparty_info.v_ThirdPartyId = gpsLocRet->clientInfo.ThirdPartyInfo.v_ThirdPartyId;
        strncpy((char*)clientInfo.t_thirdparty_info.a_ThirdPartyName,               \
                (const char*)gpsLocRet->clientInfo.ThirdPartyInfo.a_ThirdPartyName, \
                K_CGPS_MAX_THIRDPARTY_NAME_LEN);

        qop.vp_AgeLimit      = gpsLocRet->qop.vp_AgeLimit;
        qop.vp_HorizAccuracy = gpsLocRet->qop.vp_HorizAccuracy;
        qop.vp_Timeout       = gpsLocRet->qop.vp_Timeout;
        qop.vp_VertAccuracy  = gpsLocRet->qop.vp_VertAccuracy;
    }

    DEBUG_LOG_PRINT_LEV2(("SetGpsLocationRetrieve server1: Thirdparty id = %d\n", gpsLocRet->clientInfo.ThirdPartyInfo.v_ThirdPartyId));
    DEBUG_LOG_PRINT_LEV2(("SetGpsLocationRetrieve server1: Thirdparty name = %s\n", gpsLocRet->clientInfo.ThirdPartyInfo.a_ThirdPartyName));

    return MC_CGPS_REGISTER_LOCATION_RETRIEVE(vp_handle,vp_Callback,&clientInfo,&qop);

}
#endif /* AGPS_UP_FTR */
#ifdef AGPS_SBEE_FTR


//Callbacks
void SbeeNotifyConnection(t_sbee_UserNotificationParams *notif)
{
    t_sbee_si_UserNotificationParams notification;
    notification.v_Handle = (uint32_t)notif->v_Handle;
    notification.v_NotificationType = notif->v_NotificationType;
    notification.v_TimeoutSec = notif->v_TimeoutSec;
    DEBUG_LOG_PRINT_LEV2(("Sending SOCKET_AGPS_SBEE_NOTIFY_CONNECTION to client\n" ));
    DEBUG_LOG_PRINT_LEV2(("POPOYEE_SERVER Entering gpsClient_ServiceStart() MASK = %d\n", K_AGPS_REGISTER_SBEE_CLIENT));
    SendCommandToClientCapability(SOCKET_AGPS_SBEE_NOTIFY_CONNECTION, (void *)(&notification), sizeof(t_sbee_si_UserNotificationParams), K_AGPS_REGISTER_SBEE_CLIENT);
}

void SbeeConnectionRequest(t_sbee_DataConnectionRequestParams *connectionReq)
{
    t_sbee_si_DataConnectionRequestParams connectionRequest;
    connectionRequest.v_handle = (uint32_t)connectionReq->v_handle;
    connectionRequest.a_ServerAddress.p_Pointer = (uint8_t*)connectionReq->v_ServerAddress;
    DEBUG_LOG_PRINT_LEV2(("Server Address is %s\n",  connectionReq->v_ServerAddress));
    connectionRequest.a_ServerAddress.v_Length = connectionReq->v_ServerAddressLength;
    connectionRequest.v_PortNumber = connectionReq->v_PortNumber;
    DEBUG_LOG_PRINT_LEV2(("Sending SOCKET_AGPS_SBEE_REQUEST_DATA_CONNECTION to client\n" ));

    DEBUG_LOG_PRINT_LEV2(("POPOYEE_SERVER Entering gpsClient_ServiceStart() MASK = %d\n", K_AGPS_REGISTER_SBEE_CLIENT));
    SendCommandToClientCapability(SOCKET_AGPS_SBEE_REQUEST_DATA_CONNECTION, (void *)(&connectionRequest), sizeof(t_sbee_si_DataConnectionRequestParams), K_AGPS_REGISTER_SBEE_CLIENT);
}

void SbeeCloseConnection(t_sbee_DataConnectionCancelParams *closeConnectionReq)
{
    t_sbee_si_DataConnectionCancelParams closeConnection;
    closeConnection.v_handle = (uint32_t)closeConnectionReq->v_handle;
    DEBUG_LOG_PRINT_LEV2(("Sending SOCKET_AGPS_SBEE_REQUEST_DATA_DISCONNECT to client\n" ));

    DEBUG_LOG_PRINT_LEV2(("POPOYEE_SERVER Entering gpsClient_ServiceStart() MASK = %d\n", K_AGPS_REGISTER_SBEE_CLIENT));
    SendCommandToClientCapability(SOCKET_AGPS_SBEE_REQUEST_DATA_DISCONNECT, (void *)(&closeConnection), sizeof(t_sbee_si_DataConnectionCancelParams), K_AGPS_REGISTER_SBEE_CLIENT);
}

void SbeeSendData(t_sbee_SendDataParams *data)
{
    t_sbee_si_SbeeDataParams sbeeData;
    sbeeData.v_handle = (uint32_t)data->v_handle;
    sbeeData.a_SbeeData.p_Pointer = data->v_RequestString;
    sbeeData.a_SbeeData.v_Length = data->v_RequestStringLength;
    DEBUG_LOG_PRINT_LEV2(("Sending SOCKET_AGPS_SBEE_DATA to client\n" ));

DEBUG_LOG_PRINT_LEV2(("POPOYEE_SERVER Entering gpsClient_ServiceStart() MASK = %d\n", K_AGPS_REGISTER_SBEE_CLIENT));
    SendCommandToClientCapability(SOCKET_AGPS_SBEE_DATA, (void *)(&sbeeData), sizeof(t_sbee_si_SbeeDataParams), K_AGPS_REGISTER_SBEE_CLIENT);
}

static uint64_t GetTimeInSec(int year, int month, int day, int hours, int minutes, int seconds)
{
    struct tm t = {0};
    char isPM[20];
    int64_t utc;

    t.tm_year = year - 1900;
    t.tm_mon = month - 1;
    t.tm_mday = day;

    t.tm_hour = hours;
    t.tm_min = minutes;
    t.tm_sec = seconds;

    utc = mktime(&t);

    return utc;
}

void SbeeRequestCurrentGpsTime(uint32_t *gpsTime)
{
    uint64_t t1, t2;
    time_t   current;
    struct tm utc;

    DEBUG_LOG_PRINT_LEV2(("SbeeRequestCurrentGpsTime: entry\n" ));

    time(&current);

    gmtime_r(&current, &utc);

    DEBUG_LOG_PRINT_LEV2(("Date-Time for t1 = %d/%d/%d:%d.%d.%d\n", utc.tm_year, utc.tm_mon + 1, utc.tm_mday, utc.tm_hour, utc.tm_min, utc.tm_sec));

    t1 = GetTimeInSec(utc.tm_year + 1900, utc.tm_mon + 1, utc.tm_mday, utc.tm_hour, utc.tm_min, utc.tm_sec);

    DEBUG_LOG_PRINT_LEV2(("t1 = %Lu\n", t1 ));


    t2 = GetTimeInSec(1980, 1, 6, 0, 0, 0);

    DEBUG_LOG_PRINT_LEV2(("t2 = %Lu\n", t2 ));

    *gpsTime =  (uint32_t)(t1- t2);

    DEBUG_LOG_PRINT_LEV2(("Gps Time = %u\n", *gpsTime ));

    DEBUG_LOG_PRINT_LEV2(("SbeeRequestCurrentGpsTime: exit\n" ));
}


e_agps_Status SbeeInitialize()
{
    t_sbee_ServerConfiguration configuration;
    static int isInitialized = 0;

    if(isInitialized == 0)
    {
        sbee_cb.v_RequestConnectionCallback = SbeeConnectionRequest;
        sbee_cb.v_NotifyConnectionCallback = SbeeNotifyConnection;
        sbee_cb.v_CancelConnectionCallback = SbeeCloseConnection;
        sbee_cb.v_SendDataCallback = SbeeSendData;
        sbee_cb.v_CurrentGpsTimeCallback = SbeeRequestCurrentGpsTime;
        MC_SBEE_INITIALIZE(&sbee_cb);

        isInitialized = 1;
    }
    return SOCKET_AGPS_NO_ERROR;
}

e_agps_Status SbeeStart(uint16_t startTime)
{
  DEBUG_LOG_PRINT_LEV2(("Calling MC_SBEE_START_EXTENDED_EPHEMERIS from SbeeStart\n"));
  MC_SBEE_START_EXTENDED_EPHEMERIS(startTime);

    return SOCKET_AGPS_NO_ERROR;
}


e_agps_Status SbeeStop()
{
    MC_SBEE_ABORT_EXTENDED_EPHEMERIS();
    return SOCKET_AGPS_NO_ERROR;
}

void SendReceivedDataToSbee(t_sbee_si_SbeeDataParams *sbeeData)
{
   MC_SBEE_RECV_DATA((void *)sbeeData->v_handle, (unsigned char *)sbeeData->a_SbeeData.p_Pointer, (unsigned int)sbeeData->a_SbeeData.v_Length);
}


void SbeeUserConfiguration(t_sbee_si_UserConfiguration *userConfiguration)
{
    MC_SBEE_SET_USER_CONFIGURATION((t_sbee_UserConfiguration *)userConfiguration);
}

void SbeeServerConfiguration(t_sbee_si_ServerConfiguration *serverConfiguration)
{
  int index;
    MC_SBEE_SET_SERVER_CONFIGURATION((t_sbee_ServerConfiguration*)serverConfiguration);
  for(index = 0; index < serverConfiguration->v_NumSbeeServers; ++index)
  {
    if(serverConfiguration->v_SbeeServers[index] != NULL)
    {
        free( serverConfiguration->v_SbeeServers[index] );
        serverConfiguration->v_SbeeServers[index] = NULL;
    }
  }
}


void SbeeDataConnectionStatus(uint32_t handle, e_sbee_si_DataConnectionStatus status)
{
    DEBUG_LOG_PRINT_LEV2(("sending data connection status to sbee for handle %u is %d", handle, status));
  MC_SBEE_SET_DATA_CONNECTION_STATUS((void *)handle, (e_sbee_DataConnectionStatus)status);

}

void SbeeDeviceChargingStatus(BOOL status)
{
  MC_SBEE_SET_DEVICE_CHARGING_STATUS(status);
}

void SbeeDeviceBearerStatus(e_sbee_si_BearerStatus status)
{
  MC_SBEE_SET_DEVICE_BEARER_STATUS(status);
}

void SbeeNetworkStatus(e_sbee_si_NetworkStatus status)
{
  MC_SBEE_SET_NETWORK_STATUS(status);
}

void SbeeUserResponse(uint32_t handle,  e_sbee_si_UserResponseOption response)
{
  MC_SBEE_SET_USER_RESPONSE((void*)handle,response);
}

#endif /* AGPS_SBEE_FTR  */


/**
*
*/
e_agps_Status SetGpsConfiguration(t_agps_Configuration *gpsConfiguration)
{

    t_cgps_Config config;
    config.v_ConfigMaskValidity = (uint32_t)gpsConfiguration->configMaskValid;
    config.v_ConfigMask = (uint32_t)gpsConfiguration->configMask;
    config.v_Config2Mask = (uint32_t)gpsConfiguration->config2Valid;
    config.v_PowerPref = gpsConfiguration->powerPref;
    config.v_SensMode = gpsConfiguration->sensMod;
    config.v_SensTtff = gpsConfiguration->sensTTFF;
    config.v_ColdStart = gpsConfiguration->coldStart;

    config.v_ServerConfig.v_PortNum  = gpsConfiguration->slpAddress.portNumber;
    config.v_ServerConfig.v_AddrLen  = strlen( (const char*)gpsConfiguration->slpAddress.address );
    config.v_ServerConfig.v_AddrType = gpsConfiguration->slpAddress.type;
    config.v_ServerConfig.v_Addr     = malloc( config.v_ServerConfig.v_AddrLen + 1 );
    memcpy( config.v_ServerConfig.v_Addr ,
        gpsConfiguration->slpAddress.address ,
        config.v_ServerConfig.v_AddrLen );


    DEBUG_LOG_PRINT_LEV2(("config.v_ConfigMaskValidity = %d\n", config.v_ConfigMaskValidity));
    DEBUG_LOG_PRINT_LEV2(("config.v_ConfigMask = %d\n", config.v_ConfigMask));
    DEBUG_LOG_PRINT_LEV2(("config.v_Config2Mask = %d\n", config.v_Config2Mask));
    DEBUG_LOG_PRINT_LEV2(("config.v_PowerPref = %d\n", config.v_PowerPref));
    DEBUG_LOG_PRINT_LEV2(("config.v_SensMode = %d\n", config.v_SensMode));
    DEBUG_LOG_PRINT_LEV2(("config.v_SensTtff = %d\n", config.v_SensTtff));
    DEBUG_LOG_PRINT_LEV2(("config.v_ColdStart = %d\n",config.v_ColdStart ));

    DEBUG_LOG_PRINT_LEV2(("Sending configuration parameters to CGPS\n"));
    return MC_CGPS_UPDATE_USER_CONFIGURATION(&config);
}


e_agps_Status SetGpsLoggingConfiguration(t_agps_LoggingConfiguration *gpsLoggingConfiguration)
{
    t_cgps_ModuleTraceConfig vl_Config;

    vl_Config.v_Source      = gpsLoggingConfiguration->v_Moduletype;
    vl_Config.v_Filesize    = gpsLoggingConfiguration->v_LogFileSize;
    vl_Config.v_Severity    = gpsLoggingConfiguration->v_LogOpt;
    vl_Config.v_Destination = gpsLoggingConfiguration->v_LogType;

    DEBUG_LOG_PRINT_LEV2(("LogConfig Module=%d FileSize=%d Severity=%d Destn=%d",
                            vl_Config.v_Source,     \
                            vl_Config.v_Filesize,   \
                            vl_Config.v_Severity,   \
                            vl_Config.v_Destination));

    return MC_CGPS_UPDATE_TRACE_CONFIGURATION(&vl_Config);
}

e_agps_Status SetGpsPlatformConfiguration(t_agps_PlatformConfiguration *gpsPlatformConfiguration)
{
    t_cgps_PlatformConfig config;

    config.v_Config1MaskValid   =    (uint32_t)gpsPlatformConfiguration->v_Config1MaskValid;
    config.v_Config1Mask        =    (uint32_t)gpsPlatformConfiguration->v_Config1Mask;
    config.v_Config2Mask        =    gpsPlatformConfiguration->v_Config2Mask;
    config.v_MsaMandatoryMask   =    gpsPlatformConfiguration->v_MsaMandatoryMask;
    config.v_MsbMandatoryMask   =    gpsPlatformConfiguration->v_MsbMandatoryMask;
    config.v_SleepTimerDuration =    gpsPlatformConfiguration->v_SleepTimerDuration;
    config.v_ComaTimerDuration  =    gpsPlatformConfiguration->v_ComaTimerDuration;
    config.v_DeepComaDuration   =    gpsPlatformConfiguration->v_DeepComaDuration;

    DEBUG_LOG_PRINT_LEV2(("Sending Platform configuration parameters to CGPS\n"));

    return MC_CGPS_UPDATE_PLATFORM_CONFIGURATION(&config);
}



/* + PROD_TEST */
#ifdef CGPS_CNO_VALUE_FTR

/* Define the CGPS Production Test Callback here */
/* In this callback, you will need to send data to the client */

/* @todo : Dnt know what is the data here. Replace void* data by whatever you will use */
void CgpsProductionTestCallback( uint16_t  v_CnoValue, bool v_CheckLimit )
{
    if( ProductionTestFd != 0 )
    {
        t_agps_ProductionTestData v_Data;

        v_Data.v_CheckLimit = v_CheckLimit;
        v_Data.v_CNoValue = v_CnoValue;

        SendCommand(SOCKET_AGPS_PRODUCTION_TEST_DATA,&v_Data,sizeof(v_Data),ProductionTestFd);
    }
}
#endif
/* - PROD_TEST */
#endif /* #ifdef AGPS_GENERIC_SOLUTION_FTR */

/* +LMSqc19820 -Anil */
/**
*
*/
char * GetServerVersion()
{
    return AGPS_SERVER_VERSION;
}
/* -LMSqc19820 -Anil */

void GpsEventNotificationCallback (e_cgps_GpsStatus vp_Result)
{
    void *ResultType;
    DEBUG_LOG_PRINT_LEV2(("GpsEventNotificationCallback. Callback done! vp_Result %d \n",vp_Result));
    ResultType = (void *) ( vp_Result);
    SendCommandToClientCapability(SOCKET_AGPS_NI_NOTIFICATION,
                                      ResultType,
                                      1,
                                      K_AGPS_REGISTER_NI_CLIENT);

}



/*--------------------------------------------------------*/
/*     COMMAND PROCESSOR                                  */
/*--------------------------------------------------------*/

void ProcessCommandX(t_agps_Command *command , int fd )
{
    e_agps_Status agpsStatus = SOCKET_AGPS_NO_ERROR;

    DEBUG_LOG_PRINT_LEV2(("Recvd Command %s\n" , GetEnumAsString(command->type,a_SocketCommandEnumAsString)));

    DebugLogFd = fd;
    if( command->type == SOCKET_AGPS_STATUS )
    {
        // We do nothing. Ignore it
        return;
    }

    switch( command->type )
    {
        case SOCKET_AGPS_SERVICE_START:
        {
            int index = MapCallbackFunction( fd );

            if( index >= 0 )
            {
                DEBUG_LOG_PRINT_LEV2(("Calling Service Start\n"));

                agpsStatus =  MC_AGPS_SERVICE_START_REQUEST ( &(gpsClientCallbackMap[index].handle) );


                DEBUG_LOG_PRINT_LEV2(("ServiceStart returned %d\n", agpsStatus ));
            }
            else
            {
                DEBUG_LOG_PRINT_LEV2(("No Handle available\n"));
                agpsStatus = SOCKET_AGPS_NO_FREE_HANDLE;
            }
            break;
        }


        case SOCKET_AGPS_APPLICATIONID_START:
        {
            t_cgps_ApplicationID_Info *pCommand = (t_cgps_ApplicationID_Info *)((t_agps_Packet *)(command->data))->pData;
            int index = MapCallbackFunction( fd );

            if( index >= 0 )
            {
                DEBUG_LOG_PRINT_LEV2(("Calling Application ID Start\n"));

                agpsStatus =  MC_AGPS_SERVICE_START_REQUEST_2  ( &(gpsClientCallbackMap[index].handle),pCommand );

                DEBUG_LOG_PRINT_LEV2(("Application ID returned %d\n", agpsStatus ));
            }
            else
            {
                DEBUG_LOG_PRINT_LEV2(("No Handle available\n"));
                agpsStatus = SOCKET_AGPS_NO_FREE_HANDLE;
            }
            break;
        }


        case SOCKET_AGPS_SERVICE_STOP:
        {
            int index = FindMappedFd( fd );
            // t_agps_Client  *client = FindClient( fd );
            if( index >= 0 )
            {
                DEBUG_LOG_PRINT_LEV2(("Calling Service Stop\n"));

                agpsStatus = MC_AGPS_SERVICE_STOP_REQUEST ( gpsClientCallbackMap[index].handle);

                UnMapCallbackFunction( fd );

                DEBUG_LOG_PRINT_LEV2(("ServiceStop returned %d\n", agpsStatus ));
            }
            else
            {
                DEBUG_LOG_PRINT_LEV2(("Service not started\n"));
                agpsStatus = SOCKET_AGPS_SERVICE_NOT_STARTED;
            }
            break;
        }

        case SOCKET_AGPS_SWITCH_OFF_GPS:
        {
            t_agps_Client  *client = FindClient( fd );
            DEBUG_LOG_PRINT_LEV2(("Calling Switch off GPS\n"));
            MC_AGPS_SWITCH_OFF_GPS();
            if( client != NULL)
                    client->toBeClosed = 1;
            DEBUG_LOG_PRINT_LEV2(("Switch off GPS returned %d\n", agpsStatus ));
        }
        break;

        case SOCKET_AGPS_PERIODIC_FIX:
        {
            t_agps_PeriodicFixCommand *pCommand = (t_agps_PeriodicFixCommand *)((t_agps_Packet *)(command->data))->pData;
            int index = FindMappedFd( fd );
            if( index >= 0)
            {
                DEBUG_LOG_PRINT_LEV2(("Calling PeriodicFix\n"));

                agpsStatus = MC_AGPS_PERIODIC_FIX_REQUEST
                                                    ( gpsClientCallbackMap[index].handle,
                                                      (t_cgps_NavigationCallback)callbackFunctions[index],
                                                      pCommand->outputType,
                                                      pCommand->nmeaMask,
                                                      pCommand->fixRate
                                                    );

                DEBUG_LOG_PRINT_LEV2(("PeriodicFix returned %d\n", agpsStatus ));

            }
            else
            {
                DEBUG_LOG_PRINT_LEV2(("Service not started\n"));
                agpsStatus = SOCKET_AGPS_SERVICE_NOT_STARTED;
            }
            break;
        }

        case SOCKET_AGPS_SINGLE_SHOT_FIX:
        {
            t_agps_SingleShotFixCommand *pCommand = (t_agps_SingleShotFixCommand *)((t_agps_Packet *)(command->data))->pData;
            int index = FindMappedFd( fd );
            if( index >= 0)
            {
                agpsStatus = MC_AGPS_SINGLESHOT_FIX_REQUEST
                                                     (  gpsClientCallbackMap[index].handle,
                                                        (t_cgps_NavigationCallback)callbackFunctions[index],
                                                        pCommand->outputType,
                                                        pCommand->nmeaMask,
                                                        pCommand->timeout,
                                                        pCommand->horizAccuracy,
                                                        pCommand->vertAccuracy,
                                                        pCommand->ageLimit
                                                     );
            }
            else
            {
                agpsStatus = SOCKET_AGPS_SERVICE_NOT_STARTED;
            }
        }
        break;

        case SOCKET_AGPS_FENCE_GEOGRAPHICAL_AREA:
        {
            t_agps_FenceGeographicalAreaCommand *pCommand = (t_agps_FenceGeographicalAreaCommand *)((t_agps_Packet *)(command->data))->pData;
            int index = FindMappedFd( fd );
            if( index >= 0)
            {
                agpsStatus = MC_AGPS_REGISTER_FENCE_GEOGRAPHICAL_AREA
                                                     (  gpsClientCallbackMap[index].handle,
                                                        (t_cgps_NavigationCallback)callbackFunctions[index],
                                                        pCommand->v_OutputType,
                                                        pCommand->v_NmeaMask,
                                                        &pCommand->v_PositioningParams,
                                                        &pCommand->v_TargetArea,
                                                        pCommand->v_SpeedThreshold,
                                                        pCommand->v_AreaEventType
                                                     );
            }
            else
            {
                agpsStatus = SOCKET_AGPS_SERVICE_NOT_STARTED;
            }
        }
        break;
        case SOCKET_AGPS_LOCATION_FORWARD:
        {
             uint32_t tp_id=0;
             uint8_t* ch_ptr=NULL;
#if defined AGPS_FTR || defined AGPS_UP_FTR

            t_agps_LocationForwardCommand *pCommand = (t_agps_LocationForwardCommand *)((t_agps_Packet *)(command->data))->pData;
            tp_id  = pCommand->clientInfo.ThirdPartyInfo.v_ThirdPartyId;
            ch_ptr = pCommand->clientInfo.ThirdPartyInfo.a_ThirdPartyName;
            DEBUG_LOG_PRINT_LEV2(("SOCKET_AGPS_LOCATION_FORWARD SERVER: third party id %d" , tp_id));
            DEBUG_LOG_PRINT_LEV2(("SOCKET_AGPS_LOCATION_FORWARD SERVER: third party name %s" , ch_ptr));

            int index = FindMappedFd( fd );
            if( index >= 0)
            {
                agpsStatus = MC_AGPS_LOCATION_FORWARD_REQUEST
                                                     (  gpsClientCallbackMap[index].handle,
                                                        pCommand );

            }
            else
            {
                agpsStatus = SOCKET_AGPS_SERVICE_NOT_STARTED;
            }
#endif
            break;
        }
        case SOCKET_AGPS_LOCATION_RETRIEVAL:
        {
#if defined AGPS_UP_FTR

            t_agps_LocationRetrieveCommand *pCommand = (t_agps_LocationRetrieveCommand *)((t_agps_Packet *)(command->data))->pData;
            int index = FindMappedFd( fd );
            if( index >= 0)
            {
                agpsStatus = MC_AGPS_LOCATION_RETRIEVAL_REQUEST
                                                        (  gpsClientCallbackMap[index].handle,
                                                           callbackFunctions[index],
                                                           pCommand);

            }
            else
            {
                agpsStatus = SOCKET_AGPS_SERVICE_NOT_STARTED;
            }
#endif
            break;
        }

        case SOCKET_AGPS_DELETE_AIDING_DATA:
        {
            /* Expect uint32_t field */
            uint32_t fields;
            // t_agps_Client  *client = FindClient( fd );

            agpsStatus = SOCKET_AGPS_NO_ERROR;

            fields = *((uint32_t*)((t_agps_Packet *)(command->data))->pData);

            DEBUG_LOG_PRINT_LEV2(("SOCKET_AGPS_DELETE_AIDING_DATA : Fields to Delte %0x" , fields));

            MC_AGPS_DELETE_NV_STORE_FIELD( fields );


        }
        break;

#if defined (AGPS_UP_FTR)  || defined (AGPS_FTR)
        case SOCKET_AGPS_NOTIFICATION_USER_RESPONSE:
        {
            t_agps_NotificationRsp* notifResp = (t_agps_NotificationRsp *)((t_agps_Packet *)(command->data))->pData;

            DEBUG_LOG_PRINT_LEV2(("UserResponse, Notification Handle: %d, Answer: %d\n",notifResp->v_Handle, notifResp->v_Answer));
            MC_AGPS_SEND_USER_RESPONSE(notifResp->v_Handle, notifResp->v_Answer);

            break;
        }
#endif

        case SOCKET_AGPS_SET_USER_CONFIG:
        {
            t_agps_Configuration *gpsConfiguration = (t_agps_Configuration *)((t_agps_Packet *)(command->data))->pData;
            agpsStatus = MC_AGPS_SET_GPS_CONFIGURATION( gpsConfiguration );
            break;
        }

        case SOCKET_AGPS_SET_LOGGING_CONFIG:
        {
            t_agps_LoggingConfiguration *gpsLoggingConfiguration = (t_agps_LoggingConfiguration *)((t_agps_Packet *)(command->data))->pData;
            agpsStatus = MC_AGPS_SET_GPS_LOGGING_CONFIGURATION( gpsLoggingConfiguration );
            break;
        }

        case SOCKET_AGPS_SET_PLATFORM_CONFIG:
        {
            t_agps_PlatformConfiguration *gpsPlatformConfiguration = (t_agps_PlatformConfiguration *)((t_agps_Packet *)(command->data))->pData;
            agpsStatus = MC_AGPS_SET_GPS_PLATFORM_CONFIGURATION( gpsPlatformConfiguration );
            break;
        }

/* +LMSqc19820 -Anil */
        case SOCKET_AGPS_GET_VERSION:
        {
            t_agps_Version v_Send;
            char* string = MC_AGPS_GET_SERVER_VERSION();
            if( NULL != string )
            {
                DEBUG_LOG_PRINT_LEV2(("Processing command SOCKET_AGPS_GET_VERSION : %s\n" , string ));
                memcpy( v_Send.data , string , strlen((const char*)string)+1);
            }
            else
            {
                memcpy( v_Send.data , "Could Not Get Version" , strlen("Could Not Get Version")+1);
            }
            /* @todo : Currently both this and status message are returned. Ideally only one of them should go through */
            SendCommand(SOCKET_AGPS_VERSION,&v_Send,sizeof(v_Send),fd);
        }
        break;
#ifdef AGPS_SBEE_FTR
        case SOCKET_AGPS_SBEE_RECV_DATA:
        {
            t_sbee_si_SbeeDataParams *sbeeData = (t_sbee_si_SbeeDataParams *)((t_agps_Packet *)(command->data))->pData;
      MC_SBEE_RECV_DATA((void *)sbeeData->v_handle, (unsigned char *)sbeeData->a_SbeeData.p_Pointer, (unsigned int)sbeeData->a_SbeeData.v_Length);
            break;
        }

        case SOCKET_AGPS_SBEE_ABORT_EXTENDED_EPHEMERIS:
        {
            MC_SBEE_ABORT_EXTENDED_EPHEMERIS();
            break;
        }

        case SOCKET_AGPS_SBEE_INITIALIZE:
        {
            //MC_SBEE_INITIALIZE();
            break;
        }

        case SOCKET_AGPS_SBEE_START_EXTENDED_EPHEMERIS:
        {
            t_sbee_si_SbeeStart *sbeeStart = (t_sbee_si_SbeeStart *)((t_agps_Packet *)(command->data))->pData;
            MC_SBEE_START_EXTENDED_EPHEMERIS(sbeeStart->startTime);
            break;
        }
        case SOCKET_AGPS_SBEE_SET_USER_CONFIGURATION:
        {
            t_sbee_si_UserConfiguration *userConfiguration = (t_sbee_si_UserConfiguration*)((t_agps_Packet *)(command->data))->pData;
            MC_SBEE_SET_USER_CONFIGURATION(userConfiguration);
            break;
        }
        case SOCKET_AGPS_SBEE_SET_SERVER_CONFIGURATION:
        {
            int i;
            t_sbee_si_ServerConfiguration *serverConfiguration = (t_sbee_si_ServerConfiguration*)((t_agps_Packet *)(command->data))->pData;
            DEBUG_LOG_PRINT_LEV2(("Calling SBEE Server configuration\n"));
            MC_SBEE_SET_SERVER_CONFIGURATION(serverConfiguration);

            for(i = 0; i < serverConfiguration->v_NumSbeeServers; ++i)
            {
                if(serverConfiguration->v_SbeeServers[i] != NULL)
                {
                    free( serverConfiguration->v_SbeeServers[i] );
                    serverConfiguration->v_SbeeServers[i] = NULL;
                }
            }

            DEBUG_LOG_PRINT_LEV2(("Calling SBEE Server configuration : done"));
            break;
        }

    case SOCKET_AGPS_SBEE_SET_DATA_CONNECTION_STATUS:
    {
      t_sbee_si_status_message_with_handle* statusMessage = (t_sbee_si_status_message_with_handle*)((t_agps_Packet *)(command->data))->pData;
      MC_SBEE_SET_DATA_CONNECTION_STATUS(statusMessage->handle, (e_sbee_si_DataConnectionStatus)statusMessage->status);
      break;
    }

    case SOCKET_AGPS_SBEE_SET_DEVICE_CHARGING_STATUS:
    {
      t_sbee_si_status_message* statusMessage = (t_sbee_si_status_message*)((t_agps_Packet *)(command->data))->pData;
      MC_SBEE_SET_DEVICE_CHARGING_STATUS((e_sbee_si_ChargingModeOption)statusMessage->status);
      break;
    }

    case SOCKET_AGPS_SBEE_SET_DEVICE_BEARER_STATUS:
    {
      t_sbee_si_status_message* statusMessage = (t_sbee_si_status_message*)((t_agps_Packet *)(command->data))->pData;
      MC_SBEE_SET_DEVICE_BEARER_STATUS((e_sbee_si_BearerStatus)statusMessage->status);
      break;
    }

    case SOCKET_AGPS_SBEE_SET_NETWORK_STATUS:
    {
      t_sbee_si_status_message* statusMessage = (t_sbee_si_status_message*)((t_agps_Packet *)(command->data))->pData;
      MC_SBEE_SET_NETWORK_STATUS((e_sbee_si_NetworkStatus)statusMessage->status);
      break;
    }

    case SOCKET_AGPS_SBEE_SET_USER_RESPONSE:
    {
      t_sbee_si_status_message_with_handle* statusMessage = (t_sbee_si_status_message_with_handle*)((t_agps_Packet *)(command->data))->pData;
      MC_SBEE_SET_USER_RESPONSE(statusMessage->handle, (e_sbee_si_UserResponseOption)statusMessage->status);
      break;
    }


#endif /*  AGPS_SBEE_FTR */
/* -LMSqc19820 -Anil */

#ifdef AGPS_UP_FTR

/* +LMSqc19265 -Anil  */
#ifdef AGPS_GENERIC_SOLUTION_FTR
/* -LMSqc19265 -Anil */


        case SOCKET_AGPS_USER_CONFIG_REQ:
            DEBUG_LOG_PRINT_LEV2(("Processing command SOCKET_AGPS_USER_CONFIG_REQ\n"));
            agpsStatus = MC_AGPS_USER_CONFIG_REQ();
            break;

        case SOCKET_AGPS_START_BEARER_CNF:
            DEBUG_LOG_PRINT_LEV2(("Processing command SOCKET_AGPS_START_BEARER_CNF\n"));
            agpsStatus = MC_AGPS_START_BEARER_CNF();
            break;

        case SOCKET_AGPS_START_BEARER_ERR:
            DEBUG_LOG_PRINT_LEV2(("Processing command SOCKET_AGPS_START_BEARER_ERR\n"));
            agpsStatus = MC_AGPS_START_BEARER_ERR();
            break;

        case SOCKET_AGPS_CLOSE_BEARER_IND:
            DEBUG_LOG_PRINT_LEV2(("Processing command SOCKET_AGPS_CLOSE_BEARER_IND\n"));
            agpsStatus = MC_AGPS_CLOSE_BEARER_IND();
            break;

        case SOCKET_AGPS_CLOSE_BEARER_CNF:
            DEBUG_LOG_PRINT_LEV2(("Processing command SOCKET_AGPS_CLOSE_BEARER_CNF\n"));
            agpsStatus = MC_AGPS_CLOSE_BEARER_CNF();
            break;

        case SOCKET_AGPS_CLOSE_BEARER_ERR:
            DEBUG_LOG_PRINT_LEV2(("Processing command SOCKET_AGPS_CLOSE_BEARER_ERR\n"));
            agpsStatus = MC_AGPS_CLOSE_BEARER_ERR();
            break;

        case SOCKET_AGPS_SUPL_TCPIP_CONNECT_CNF:  /** \ref t_agps_TcpIpConnectRsp */
            {
                t_agps_TcpIpConnectCnf *tcpIpConnectCnf =
                    (t_agps_TcpIpConnectCnf *)((t_agps_Packet *)(command->data))->pData;
                DEBUG_LOG_PRINT_LEV2(("Processing command SOCKET_AGPS_SUPL_TCPIP_CONNECT_CNF\n"));
                agpsStatus = MC_AGPS_SUPL_TCPIP_CONNECT_CNF(tcpIpConnectCnf);
            }
            break;

        case SOCKET_AGPS_SUPL_TCPIP_CONNECT_ERR:  /** \ref t_agps_TcpIpConnectRsp */
            {
                t_agps_TcpIpConnectErr *tcpIpConnectErr =
                    (t_agps_TcpIpConnectErr *)((t_agps_Packet *)(command->data))->pData;
                DEBUG_LOG_PRINT_LEV2(("Processing command SOCKET_AGPS_SUPL_TCPIP_CONNECT_ERR\n"));
                agpsStatus = MC_AGPS_SUPL_TCPIP_CONNECT_ERR(tcpIpConnectErr);
            }
            break;

        case SOCKET_AGPS_SUPL_TCPIP_DISCONNECT_IND: /** \ref t_agps_TcpIpDisconnectInd */
            {
                t_agps_TcpIpDisconnectInd *tcpIpDisconnectInd =
                    (t_agps_TcpIpDisconnectInd *)((t_agps_Packet *)(command->data))->pData;
                DEBUG_LOG_PRINT_LEV2(("Processing command SOCKET_AGPS_SUPL_TCPIP_DISCONNECT_IND\n"));
                agpsStatus = MC_AGPS_SUPL_TCPIP_DISCONNECT_IND(tcpIpDisconnectInd);
            }
            break;

        case SOCKET_AGPS_SUPL_TCPIP_DISCONNECT_CNF: /** \ref t_agps_TcpIpDisconnectCnf */
            {
                t_agps_TcpIpDisconnectCnf *tcpIpDisconnectCnf =
                    (t_agps_TcpIpDisconnectCnf *)((t_agps_Packet *)(command->data))->pData;
                DEBUG_LOG_PRINT_LEV2(("Processing command SOCKET_AGPS_SUPL_TCPIP_DISCONNECT_CNF\n"));
                agpsStatus = MC_AGPS_SUPL_TCPIP_DISCONNECT_CNF(tcpIpDisconnectCnf);
            }
            break;
        case SOCKET_AGPS_SUPL_TCPIP_DISCONNECT_ERR: /** \ref t_agps_TcpIpDisconnectRsp */
            {
                t_agps_TcpIpDisconnectErr *tcpIpDisconnectErr =
                    (t_agps_TcpIpDisconnectErr *)((t_agps_Packet *)(command->data))->pData;
                DEBUG_LOG_PRINT_LEV2(("Processing command SOCKET_AGPS_SUPL_TCPIP_DISCONNECT_ERR\n"));
                agpsStatus = MC_AGPS_SUPL_TCPIP_DISCONNECT_ERR(tcpIpDisconnectErr);
            }
            break;

        case SOCKET_AGPS_SUPL_MOBILE_INFO_IND: /** \ref t_agps_MobileInfoInd */
            {
                t_agps_MobileInfoInd *mobileInfoInd =
                    (t_agps_MobileInfoInd *)((t_agps_Packet *)(command->data))->pData;
                DEBUG_LOG_PRINT_LEV2(("Processing command SOCKET_AGPS_SUPL_MOBILE_INFO_IND\n"));
                agpsStatus = MC_AGPS_SUPL_MOBILE_INFO_IND(mobileInfoInd);
            }
            break;

        case SOCKET_AGPS_WIRELESS_NETWORK_INFO_IND: /** \ref t_agps_WLANInfoInd */
            {
                t_agps_WLANInfoInd *wlanInfoInd =
                    (t_agps_WLANInfoInd *)((t_agps_Packet *)(command->data))->pData;
                DEBUG_LOG_PRINT_LEV2(("Processing command SOCKET_AGPS_WIRELESS_NETWORK_INFO_IND\n"));
                agpsStatus = MC_AGPS_WLAN_INFO_IND(wlanInfoInd);
            }
            break;

/* +LMSqc19265 -Anil  */
#endif /* #ifdef AGPS_GENERIC_SOLUTION_FTR  */
/* -LMSqc19265 -Anil */

        case SOCKET_AGPS_SUPL_SMS_PUSH_IND: /** \ref t_agps_SmsPushInd */
            {
                t_agps_SmsPushInd * smsPushInd = (t_agps_SmsPushInd *)((t_agps_Packet *)command->data)->pData;

                DEBUG_LOG_PRINT_LEV2(("Processing command SOCKET_AGPS_SUPL_SMS_PUSH_IND\n"));
                agpsStatus = MC_AGPS_SUPL_SMS_PUSH_IND(smsPushInd);
            }
            break;
/* +LMSqc19265 -Anil  */
#ifdef AGPS_GENERIC_SOLUTION_FTR
/* -LMSqc19265 -Anil */

        case SOCKET_AGPS_SUPL_SEND_DATA_CNF: /** \ref t_agps_SuplSendDataCnf */
            {
                t_agps_SuplSendDataCnf *sendDataCnfm =
                    (t_agps_SuplSendDataCnf *)((t_agps_Packet *)(command->data))->pData;
                DEBUG_LOG_PRINT_LEV2(("Processing command SOCKET_AGPS_SUPL_SEND_DATA_CNF\n"));
                agpsStatus = MC_AGPS_SUPL_SEND_DATA_CNF(sendDataCnfm);
            }
            break;

        case SOCKET_AGPS_SUPL_RECEIVE_DATA_IND: /** \ref t_agps_SuplData */
            {
                t_agps_SuplData * suplData = (t_agps_SuplData *)((t_agps_Packet *)command->data)->pData;

                DEBUG_LOG_PRINT_LEV2(("Processing command SOCKET_AGPS_SUPL_RECEIVE_DATA_IND\n"));
                agpsStatus = MC_AGPS_SUPL_RECEIVE_DATA_IND(suplData);
                suplData->v_Data.p_Pointer = NULL ;
            }
            break;
/* +LMSqc19265 -Anil  */
#endif /* #ifdef AGPS_GENERIC_SOLUTION_FTR  */
/* -LMSqc19265 -Anil */

#endif /* #ifdef AGPS_UP_FTR */




#if defined (AGPS_UP_FTR)  || defined (AGPS_FTR)
        case SOCKET_AGPS_REGISTER_CLIENT:
            {
                t_agps_Client  *client = FindClient( fd );

                t_agps_RegisterClientMask *pMask = (t_agps_RegisterClientMask *)((t_agps_Packet *)command->data)->pData;
                DEBUG_LOG_PRINT_LEV2(("Processing command SOCKET_AGPS_REGISTER_CLIENT , Mask : %x\n" , *pMask ));

                  client->capabilityMask = *pMask;

                if( client->capabilityMask & K_AGPS_REGISTER_NOTIFICATION_CLIENT )
                {
                    MC_AGPS_REGISTER_NOTIFICATION(AgpsNotificationCallback);
                }

#ifdef AGPS_SBEE_FTR
        SbeeInitialize();
#endif /*#ifdef AGPS_SBEE_FTR*/

                if( client->capabilityMask & K_AGPS_REGISTER_MOBILE_INFO_CLIENT )
                {
                        SendCommandToClientCapability(
                            SOCKET_AGPS_MOBILE_INFO_REQ,
                            NULL ,
                            0 ,
                            K_AGPS_REGISTER_MOBILE_INFO_CLIENT);
                }


                if( client->capabilityMask & K_AGPS_REGISTER_WIRELESS_NETWORK_CLIENT )
                {
                    SendCommandToClientCapability(
                         SOCKET_AGPS_WIRELESS_NETWORK_INFO_REQ,
                         NULL ,
                         0 ,
                         K_AGPS_REGISTER_WIRELESS_NETWORK_CLIENT);
                }

                if( client->capabilityMask & K_AGPS_REGISTER_NI_CLIENT )
                {
                     MC_CGPS_EVENT_NOTIFICATION_CB_REGISTER( GpsEventNotificationCallback );
                }
            }
            break;
#endif /* #ifdef AGPS_UP_FTR  || AGPS_FTR*/

        /* + PROD_TEST */
#ifdef CGPS_CNO_VALUE_FTR
        case SOCKET_AGPS_PRODUCTION_TEST_START:
            {
                t_CGPS_CnoCriteria *CNoCriteria;

                /* We save the fd so that in the callback we know who to send the production test data to */
                ProductionTestFd = fd;

                /* Register the CGPS Production Test Callback here */
                MC_CGPS_PRODUCTION_TEST_CB_REGISTER( CgpsProductionTestCallback);

                /* Now call the CGPS Production Test Function here */
                /* t_Agps_CnoCriteria and t_CGPS_CnoCriteria are same */
                CNoCriteria = (t_CGPS_CnoCriteria *)((t_agps_Packet *)command->data)->pData;
                MC_CGPS_GET_CNO_VALUE( *CNoCriteria );
            }
        break;
#endif
        /* - PROD_TEST */
         case SOCKET_AGPS_DEBUG_LOG_START:
             {
                 DEBUG_LOG_PRINT_LEV2(("received SOCKET_AGPS_DEBUG_LOG_START\n"));

                /* We save the fd so that in the callback we know who to send the production test data to */
                /* Register the CGPS Production Test Callback here */
                agpsStatus = MC_CGPS_TEST_REGISTER_CALLBACK_FOR_ALL(CallbackDebugLog,K_CGPS_NMEA,0xffff );

            }
                break;
    case SOCKET_AGPS_PRODUCTION_TEST_START_REQ:
         {
            DEBUG_LOG_PRINT(("Processing command SOCKET_AGPS_PRODUCTION_TEST_START_REQ\n"));

            t_agps_TestRequest    *gpsTREQ_Data;
            t_CGPS_TestRequest    cgpsTREQ_Data;

            memset(&cgpsTREQ_Data, 0, sizeof(cgpsTREQ_Data));

            gpsTREQ_Data = (t_agps_TestRequest *)((t_agps_Packet *)(command->data))->pData;

            cgpsTREQ_Data.v_InPinValue       = gpsTREQ_Data->v_InPinValue;
            cgpsTREQ_Data.v_InPinMask        = gpsTREQ_Data->v_InPinMask;
            cgpsTREQ_Data.v_OutPinValue      = gpsTREQ_Data->v_OutPinValue;
            cgpsTREQ_Data.v_OutPinMask       = gpsTREQ_Data->v_OutPinMask;
            cgpsTREQ_Data.v_SpectrumTestType = gpsTREQ_Data->v_SpectrumTestType;
            cgpsTREQ_Data.v_FileLogResult    = gpsTREQ_Data->v_FileLogResult;
            strncpy(cgpsTREQ_Data.v_InputSigFreq, gpsTREQ_Data->v_InputSigFreq, CGPS_MAX_TREQ_FREQ_LEN);
            strncpy(cgpsTREQ_Data.v_RefClockFreq, gpsTREQ_Data->v_RefClockFreq, CGPS_MAX_TREQ_FREQ_LEN);
            strncpy(cgpsTREQ_Data.v_PgaValues,    gpsTREQ_Data->v_PgaValues,    CGPS_MAX_PGA_VALUE_LEN);

            MC_CGPS_PRODUCTION_TEST_START((uint16_t)gpsTREQ_Data->v_TestReqMask, &cgpsTREQ_Data);
            DEBUG_LOG_PRINT(("SOCKET_AGPS_PRODUCTION_TEST_START_REQ\n"));
        }
        break;
    case SOCKET_AGPS_PRODUCTION_TEST_STOP_IND:
         {
            DEBUG_LOG_PRINT(("Processing command SOCKET_AGPS_PRODUCTION_TEST_STOP_IND\n"));

            t_agps_StopRequest    *gpsTREQ_Data;

            gpsTREQ_Data = (t_agps_StopRequest *)((t_agps_Packet *)(command->data))->pData;

            DEBUG_LOG_PRINT(("v_TestReqMask %d \n", gpsTREQ_Data->v_TestReqMask));
            DEBUG_LOG_PRINT(("v_State %d \n", gpsTREQ_Data->v_State));

            MC_CGPS_PRODUCTION_TEST_STOP((uint16_t)gpsTREQ_Data->v_TestReqMask, (uint8_t)gpsTREQ_Data->v_State);
         }
         break;

#ifdef AGPS_EE_FTR

        case SOCKET_AGPS_REGISTER_EE_CLIENT_CAPABILITIES:
        {
            t_agps_Client  *client = FindClient( fd );
            DEBUG_LOG_PRINT(("File descriptor for EEClient : %d\n" , fd));

            t_agps_RegisterClientMask *pMask = (t_agps_RegisterClientMask *)((t_agps_Packet *)command->data)->pData;
            DEBUG_LOG_PRINT(("Processing command SOCKET_AGPS_REGISTER_EE_CLIENT_CAPABILITIES , Mask : %x\n" , *pMask ));
            client->capabilityMask = *pMask;

            /*NOTE:-
                        AS of now MSL  is registring with the  the Mask : 1664  i.e 11010000000
                        it means:-
                        K_AGPS_REGISTER_REFPOS_CLIENT                          Set
                        K_AGPS_REGISTER_EXTENDED_EPHEMERIS_CLIENT   unset
                        K_AGPS_REGISTER_SAEE_CLIENT                              Set
                        K_AGPS_REGISTER_BCE_CLIENT                               Set

                        This is to be corrected from MSL side.
                        Till this correction is received, we will keep the capability as K_AGPS_REGISTER_BCE_CLIENT
                         SendCommandToClientCapability at EEClientCallback
                        To be changed to K_AGPS_REGISTER_EXTENDED_EPHEMERIS_CLIENT later on
                    */

            if(( client->capabilityMask & K_AGPS_REGISTER_EXTENDED_EPHEMERIS_CLIENT) || (client->capabilityMask & K_AGPS_REGISTER_BCE_CLIENT))
            {
                MC_AGPS_REGISTER_EXTENDED_EPH(EEClientCallback);

            }

            /*XYBRID Integration :194997*/
            if( client->capabilityMask & K_AGPS_REGISTER_REFPOS_CLIENT )
            {
                MC_AGPS_REGISTER_REF_LOCATION(EEClientGetRefLocCallback);

            }
            /*XYBRID Integration :194997*/


            /*Reference Time :194997*/

            MC_AGPS_REGISTER_REF_TIME(EEClientGetRefTimeCallback);

            /*Reference Time :194997*/
            MC_AGPS_REGISTER_DELETE_SEED(EEClientDeleteSeedCallback);
            MC_AGPS_REGISTER_BCE_DATA(EEClientBceDataCallback);
        }
        break;

        case SOCKET_AGPS_EE_GET_EPHEMERIS_RSP:
        {
            t_agps_NavDataList * NavDataList = (t_agps_NavDataList *)((t_agps_Packet *)command->data)->pData;

            DEBUG_LOG_PRINT_LEV2(("Processing command SOCKET_AGPS_EE_GET_EPHEMERIS_RSP\n"));

            agpsStatus = MC_AGPS_EE_GET_EPHEMERIS_RSP(NavDataList);

        }
        break;

        case SOCKET_AGPS_EE_GET_REFLOCATION_RSP:
        {
            t_agps_RefPosition * RefPosition = (t_agps_RefPosition *)((t_agps_Packet *)command->data)->pData;

            DEBUG_LOG_PRINT_LEV2(("received SOCKET_AGPS_EE_GET_REFLOCATION_RSP on AGPSServer \n"));

            agpsStatus = MC_AGPS_EE_GET_REFLOCATION_RSP(RefPosition);

        }
        break;

        case SOCKET_AGPS_SET_REFERENCE_TIME_IND:
        {
            s_gnsCP_RefTime *pRefTime = (s_gnsCP_RefTime*)MC_RTK_GET_MEMORY (sizeof(*pRefTime));

             t_agps_RefTimeData * agpsRefTime = (t_agps_RefTimeData *)((t_agps_Packet *)command->data)->pData;

            pRefTime->v_GpsWeek = agpsRefTime->weekNum;
            pRefTime->v_GpsTow = agpsRefTime->TOWmSec;
            pRefTime->v_GpsTowUnc = (agpsRefTime->TAccmSec * 1000);
            pRefTime->v_RefDeltaCellTime = K_gnsUNKNOWN_S32;
            pRefTime->v_RefCellTimeUnc = K_gnsUNKNOWN_U32;
            pRefTime->v_UtranGpsDriftRate = K_gnsUNKNOWN_S8;
            pRefTime->v_NumTowAssist = 0;
            pRefTime->v_CellTimeType = E_gns_RAT_TYPE_NONE;

            DEBUG_LOG_PRINT_LEV2(("received SOCKET_AGPS_SET_REFERENCE_TIME_IND on AGPSServer \n"));

            GNS_CpRefTimeWithSrcInd(E_gns_ASSIST_SRC_APPLI,pRefTime);
        }
        break;

#endif

        default :
            DEBUG_LOG_PRINT_LEV2(("ERROR : Unknown command! : %d" , command->type ));
    }

    /* We send a status report back for all messages except SOCKET_ACK and SOCKET_STATUS */
    SendCommand( SOCKET_AGPS_STATUS , &agpsStatus , sizeof(agpsStatus) , fd );

}

void ProcessCommand( char *pData, int fd, int bytesRead )
{
    int byteDecoded = 0;

    /* Ack every received command and then go ahead and process all the received commands! */
    /* @todo : Currently we ack , execute , ack , execute etc. This is not good! */
    while(byteDecoded < bytesRead)
    {
        t_agps_Command      command;
        t_Buffer            v_ReadBuffer;
/* +ER:270717 */
        uint32_t            decodedLen = 0;
/* -ER:270717 */
        v_ReadBuffer.p_Buffer = pData + byteDecoded;
        v_ReadBuffer.v_Length = bytesRead - byteDecoded;
/* +ER:270717 */
        decodedLen   = agpscodec_DecodeCommand(&command, v_ReadBuffer );
        byteDecoded += decodedLen;

        DEBUG_LOG_PRINT_LEV2(("Bytes Decoded = %d, Total Bytes Decoded=%d Bytes read =%d\n", decodedLen, byteDecoded, bytesRead ));

        if( decodedLen )
        {
/* -ER:270717 */
            ProcessCommandX( &command , fd );

            if(command.data->pData != NULL)
            {
                free( command.data->pData );
                command.data->pData = NULL;
            }


            if(command.data != NULL)
            {
                free( command.data );
                command.data = NULL;
            }

/* +ER:270717 */
        }
        else
        {
             break;
        }
/* -ER:270717 */
    }
}




/*--------------------------------------------------------*/
/*  FILE DESCRIPTOR HANDLERS                              */
/*--------------------------------------------------------*/

//Sets the given fd (file descriptor) as non blocking
//Parameter: fd :- File descriptor
static void FdSetNonBlock(int  fd)
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



//Main loop which handles the events from all the clients and sends data to clients
void FdHandler()
{
    int numEvents;
    int index;
    int childFd;

    for( ; ExitSocketServer == 0 ; )
    {

        OSA_Sleep(1);
        //Waiting for receiving events on EpollFd which is linked Main socket and all the clients
        //will return Events from all the clients.
        numEvents = epoll_wait(EpollFd, Events, MAX_GPS_CLIENT, -1);
        for(index = 0; index < numEvents; index++)
        {
            if(Events[index].data.fd == ListenerFd)
            {
                DEBUG_LOG_PRINT_LEV2(("Connection Event occured\n"));
                childFd = accept(ListenerFd, NULL, NULL);
                DEBUG_LOG_PRINT_LEV2(("Connection Accepted, childFd = %d\n", childFd));
/* +ER:270717 */
                if(childFd >= 0)
/* -ER:270717 */
                {
                    //Set childFd non-blocking
                    FdSetNonBlock(childFd);
                    EpollEvent.events = EPOLLIN | EPOLLET;
                    EpollEvent.data.fd = childFd;

                    if(epoll_ctl(EpollFd, EPOLL_CTL_ADD, childFd, &EpollEvent) < 0)
                    {
                        DEBUG_LOG_PRINT_LEV2(("Error: epoll_ctl ADD returned negative value\n"));
                        //exit(1);
                    }
                    else
                    {
                        //Add a new client to the clients list with childFd
                        AddClient(childFd);
                        DEBUG_LOG_PRINT_LEV2(("Client is added to the client list\n"));
                    }
                }
                else
                {
                    DEBUG_LOG_PRINT_LEV2(("Invalid client connection\n"));
                }
            }
            else {
                //Search Client list for a client with the incoming fd
                t_agps_Client  *client = FindClient(Events[index].data.fd);
                DEBUG_LOG_PRINT_LEV2(("Activity on connection\n"));
                //Process the event only if client with the incoming fd is present in the client list
                if(client != NULL)
                {
                    DEBUG_LOG_PRINT_LEV2(("Events on handle %d - <0x%X>" , Events[index].data.fd , Events[index].events ));
                    if( Events[index].events & EPOLLERR )
                    {
                        //DEBUG_LOG_PRINT_LEV2("agpsserver.c: Cliet closed the connection or Error happend on the file descriptor, Error: %d\n", errno);
                        DEBUG_LOG_PRINT_LEV2(("EPOLLERR on handle %d" , Events[index].data.fd));
                    }
                    if( Events[index].events & EPOLLHUP )
                    {
                        DEBUG_LOG_PRINT_LEV2(("EPOLLHUP on handle %d" , Events[index].data.fd));
                        //Stop listening to the File Descriptor
                        epoll_ctl(EpollFd, EPOLL_CTL_DEL, Events[index].data.fd, NULL);
                        //Delete the client from the clent list
                        DeleteClient(Events[index].data.fd);
                        close(Events[index].data.fd);
                    }
                    if( Events[index].events & EPOLLIN )
                    {
                        /*
                            1) Read BUFFER_SIZE bytes. If all buffer size bytes is read, there is a possibility that more data exists.
                            Read a further buffer size of data. Continue this till there is no more data to be read.
                            2) Check if there is a fucntion to peek and see the amount of data available on the socket. Read exactly
                            that much data.
                            In any case, this constant allocation of buffer memory for each client is not good
                        */
                        char *p_DataBuffer;
                        int  v_DataLen = 0;
                        int done = FALSE;
                        int bytesRead = 0;

                        p_DataBuffer = (char *)malloc(BUFFER_SIZE);

                            do
                            {

                                bytesRead = recv(Events[index].data.fd, p_DataBuffer + v_DataLen, BUFFER_SIZE, 0);


                                if(bytesRead == 0)
                                {
                                    DEBUG_LOG_PRINT_LEV2(("%d closed connection\n", Events[index].data.fd));
                                    epoll_ctl(EpollFd, EPOLL_CTL_DEL, Events[index].data.fd, NULL);
                                    DeleteClient(Events[index].data.fd);
                                    close(Events[index].data.fd);

                                    if(p_DataBuffer != NULL)
                                    {
                                        free( p_DataBuffer );
                                        p_DataBuffer = NULL;
                                    }

                                    v_DataLen = 0;
                                    done = TRUE;
                                }
                                else if(bytesRead < 0 && errno != EAGAIN)
                                {
                                    DEBUG_LOG_PRINT_LEV2(("bytesRead < 0, error occured on fd %d, errno: %d\n",
                                            Events[index].data.fd, errno));

                                    if(p_DataBuffer != NULL)
                                    {
                                        free( p_DataBuffer );
                                        p_DataBuffer = NULL;
                                    }

                                    v_DataLen = 0;
                                    done = TRUE;
                                }
                                else if( bytesRead == BUFFER_SIZE )
                                {
                                    /* This implies there is further data to be read. Go around for another try */
                                    char *p_Tmp;

                                    /* Update current data size */
                                    v_DataLen += bytesRead;

                                    p_Tmp = realloc( p_DataBuffer , v_DataLen + BUFFER_SIZE );

                                    if( p_Tmp != NULL )
                                    {
                                        p_DataBuffer = p_Tmp;
                                    }
                                    else
                                    {
                                        DEBUG_LOG_PRINT_LEV2(("Realloc failed!!!"));
                                        done = TRUE;
                                    }
                                }
                                else
                                {
                                    /* Actual length of data is hence whatever was previously read + current read data */
                                    v_DataLen += bytesRead;
                                    done = TRUE;
                                }
                            }while( !done );

                            if( v_DataLen > 0 )
                            {
                                DEBUG_LOG_PRINT_LEV2(("%d Bytes Read\n", v_DataLen));

                                ProcessCommand( p_DataBuffer ,  Events[index].data.fd, v_DataLen );

                                DEBUG_LOG_PRINT_LEV2(("Command processed. \n"));

                        }
                        if(p_DataBuffer != NULL)
                        {
                            free( p_DataBuffer );
                            p_DataBuffer = NULL;
                        }
                    }
                }
            }
        }
    }
}


int  InitSocketServer( void )
{
    int bindStatus;

    u_int yes = 1;

    DEBUG_LOG_PRINT_LEV2(("Entering to the program\n"));
#ifndef LBS_LINUX_UBUNTU_FTR

    DEBUG_LOG_PRINT_LEV2(("Thread ID : %d\n",gettid()));
#endif




    if( (ListenerFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    //if( (ListenerFd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        DEBUG_LOG_PRINT_LEV2(("Error: sockfd\n"));
        return 1;
    }
    DEBUG_LOG_PRINT_LEV2(("Socket Created, fd : %d\n", ListenerFd));


    bzero(&srv, sizeof(srv));

    //srv.sun_family = AF_UNIX;
    //strncpy(srv.sun_path, GNS_SOCKET_PATH, sizeof(srv.sun_path) - 1);

    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = htonl(INADDR_SERVER_HOST);
    srv.sin_port = htons(PORT);

    if (setsockopt(ListenerFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
       DEBUG_LOG_PRINT_LEV2(("Reusing ADDR failed"));
       return 1;
    }


    bindStatus = bind(ListenerFd, (struct sockaddr *) &srv, sizeof(srv));
    if( bindStatus < 0)
    {
        DEBUG_LOG_PRINT_LEV2(("Error: bind %d\n", errno));
        return 1;
    }


    DEBUG_LOG_PRINT_LEV2(("Socket bound\n"));

    if (listen(ListenerFd, MAX_GPS_CLIENT) == -1 )
    {
        DEBUG_LOG_PRINT_LEV2(("Listening failed on fd : %d\n", ListenerFd));
        return 1;
    }

    DEBUG_LOG_PRINT_LEV2(("Listening started on fd : %d\n", ListenerFd));

    EpollFd = epoll_create(MAX_GPS_CLIENT);
    if(EpollFd < 0)
    {
        DEBUG_LOG_PRINT_LEV2(("Error: epoll_create\n"));
        return 1;
    }

    DEBUG_LOG_PRINT_LEV2(("Epoll Created EpollFd: %d\n", EpollFd));

    EpollEvent.events = EPOLLIN | EPOLLERR | EPOLLHUP;
    EpollEvent.data.fd = ListenerFd;
    if(epoll_ctl(EpollFd, EPOLL_CTL_ADD, ListenerFd, &EpollEvent) < 0)
    {
        DEBUG_LOG_PRINT_LEV2(("epoll_ctl, adding ListenerFd\n"));
        return 1;
    }
    DEBUG_LOG_PRINT_LEV2(("epoll_ctl added for ListenerFd\n"));

    /* Change to restrictive user. */
    DEBUG_LOG_PRINT(("Changing to restrictive uid in socket server thread\n"));
    setuid(1001);

    DEBUG_LOG_PRINT_LEV2(("Calling FdHandler for looping\n"));

    FdHandler();

    return 0;
}


void InitAgps()
{
#ifdef AGPS_GENERIC_SOLUTION_FTR
    GNS_Initialize();
#ifdef AGPS_UP_FTR
    GNS_SuplInitialize(__GnsSuplCallback);
    GNS_WirelessInitialize(__GnsWirelessInfoCallback);
#endif /*#ifdef AGPS_UP_FTR*/
#ifdef GNS_CELL_INFO_FTR
    GNS_CellInfoInitialize(__GnsCellInfoCallback);
#endif /*#ifdef GNS_CELL_INFO_FTR*/

#ifdef AMIL_LCS_FTR
#ifdef AGPS_FTR
    GNS_CpInitialize(__GnsCpCallback);
#endif /*#ifdef AGPS_FTR*/
#ifdef GPS_FREQ_AID_FTR
    GNS_FaInitialize(__GnsFaCallback);
#endif
#ifdef AGPS_TIME_SYNCH_FTR
    GNS_FtaInitialize(__GnsFtaCallback);
#endif
#endif /* AMIL_LCS_FTR */

#endif /*#ifdef AGPS_GENERIC_SOLUTION_FTR*/

}
