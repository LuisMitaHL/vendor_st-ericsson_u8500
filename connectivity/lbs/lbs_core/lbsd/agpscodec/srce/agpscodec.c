/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef ARM_LINUX_AGPS_FTR
#include <android_log.h>
#endif
#include "agps_client.h"
#include "agps.h"
#include "agpscodec.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define  LOG_TAG  "agpscodec"
#include "sbeesocketinterface.h"


/*****************************************************************************/
/* agpscodec_GetLengthOfCommand : returns the length of the command                            */
/*****************************************************************************/

static int agpscodec_GetLengthOfPayload( t_agps_Command *command )
{
    int length;

    switch( command->type )
    {
        case SOCKET_AGPS_SERVICE_START:
        {
            length = 0;
            break;
        }

        case SOCKET_AGPS_APPLICATIONID_START:
        {
            t_agps_ApplicationIDInfo *pl_agps_AppInfo=NULL;
            length = sizeof(*pl_agps_AppInfo);
            break;
        }

        case SOCKET_AGPS_SERVICE_STOP:
            length = 0;
            break;

/* +LMSqc19820 -Anil */
        case SOCKET_AGPS_GET_VERSION:
            length = 0;
            break;

        case SOCKET_AGPS_VERSION:
        {

            t_agps_Version *pl_agps_Version=NULL;
            length = sizeof(*pl_agps_Version);
            break;
        }
/* -LMSqc19820 -Anil */

        case SOCKET_AGPS_SWITCH_OFF_GPS:
            length = 0;
            break;

        case SOCKET_AGPS_PERIODIC_FIX:
        {

            t_agps_PeriodicFixCommand *pl_agps_PeriodicFixCommand=NULL;
            length = sizeof(*pl_agps_PeriodicFixCommand);
            break;
        }

        case SOCKET_AGPS_SINGLE_SHOT_FIX:
        {

            t_agps_SingleShotFixCommand *pl_agps_SingleShotFixCommand=NULL;
            length = sizeof(*pl_agps_SingleShotFixCommand);
            break;
        }

        case SOCKET_AGPS_FENCE_GEOGRAPHICAL_AREA:
        {

            t_agps_FenceGeographicalAreaCommand *pl_agps_FenceGeographicalAreaCommand=NULL;
            length = sizeof(*pl_agps_FenceGeographicalAreaCommand);
            break;
        }

        case SOCKET_AGPS_LOCATION_FORWARD:
        {

            t_agps_LocationForwardCommand *pl_agps_LocationForwardCommand=NULL;
            length = sizeof(*pl_agps_LocationForwardCommand);
            break;
        }
        case SOCKET_AGPS_LOCATION_RETRIEVAL:
        {
            t_agps_LocationRetrieveCommand *pl_agps_LocationRetrieveCommand=NULL;
            length = sizeof(*pl_agps_LocationRetrieveCommand);
            break;
        }

        case SOCKET_AGPS_NAVDATA:
        {
            t_agps_CStructNavData *pl_agps_CStructNavData=NULL;
            length = sizeof(*pl_agps_CStructNavData);
            break;
        }

/* +AGPSL-2.4 */
        case SOCKET_AGPS_DELETE_AIDING_DATA:
            length = sizeof(uint32_t);
/* -AGPSL-2.4 */
            break;

        case SOCKET_AGPS_NOTIFICATION:
        {
            t_agps_NotificationInd *pl_agps_NotificationInd=NULL;
            length = sizeof(*pl_agps_NotificationInd);
            break;
        }

        case SOCKET_AGPS_NOTIFICATION_USER_RESPONSE:
        {
            t_agps_NotificationRsp  *pl_agps_NotificationRsp=NULL;
            length = sizeof( *pl_agps_NotificationRsp );
            break;
        }

        case SOCKET_AGPS_SET_USER_CONFIG:
        {
            t_agps_Configuration  *pl_agps_Configuration=NULL;
            length = sizeof( *pl_agps_Configuration );
            break;
        }

        case SOCKET_AGPS_SET_LOGGING_CONFIG:
        {
            t_agps_LoggingConfiguration  *pl_agps_LoggingConfiguration=NULL;
            length = sizeof( *pl_agps_LoggingConfiguration );
            break;
        }

        case SOCKET_AGPS_SET_PLATFORM_CONFIG:
        {
            t_agps_PlatformConfiguration  *pl_agps_PlatformConfiguration=NULL;
            length = sizeof( *pl_agps_PlatformConfiguration );
            break;
        }

        case SOCKET_AGPS_USER_CONFIG_REQ:
            length = 0;
            break;

        case SOCKET_AGPS_USER_CONFIG_RSP:
        {
            t_agps_Configuration *pl_agps_Configuration=NULL;
            length = sizeof( *pl_agps_Configuration );
            break;
        }

        //case SOCKET_AGPS_BEARER_REGISTER_HANDLER:
            //length = 0;
            //break;

        case SOCKET_AGPS_START_BEARER_REQ:
            length = 0;
            break;

        case SOCKET_AGPS_START_BEARER_CNF:
            length = 0;
            break;

        case SOCKET_AGPS_START_BEARER_ERR:
            length = 0;
            break;

        case SOCKET_AGPS_CLOSE_BEARER_REQ:
            length = 0;
            break;

        case SOCKET_AGPS_CLOSE_BEARER_IND:
            length = 0;
            break;

        case SOCKET_AGPS_CLOSE_BEARER_CNF:
            length = 0;
            break;

        case SOCKET_AGPS_CLOSE_BEARER_ERR:
            length = 0;
            break;

        case SOCKET_AGPS_SUPL_TCPIP_CONNECT_REQ:  /** \ref t_agps_TcpIpConnectReq */
        {
            t_agps_TcpIpConnectReq *pl_agps_TcpIpConnectReq=NULL;
            length = sizeof( *pl_agps_TcpIpConnectReq );
            break;
        }

        case SOCKET_AGPS_SUPL_TCPIP_CONNECT_CNF:  /** \ref t_agps_TcpIpConnectRsp */
        {
            t_agps_TcpIpConnectCnf *pl_agps_TcpIpConnectCnf=NULL;
            length = sizeof( *pl_agps_TcpIpConnectCnf );
            break;
        }

        case SOCKET_AGPS_SUPL_TCPIP_CONNECT_ERR:  /** \ref t_agps_TcpIpConnectErr */
        {
            t_agps_TcpIpConnectErr *pl_agps_TcpIpConnectErr=NULL;
            length = sizeof( *pl_agps_TcpIpConnectErr );
            break;
        }

        case SOCKET_AGPS_SUPL_TCPIP_DISCONNECT_REQ: /** \ref t_agps_TcpIpDisconnectReq */
        {
            t_agps_TcpIpDisconnectReq *pl_agps_TcpIpDisconnectReq=NULL;
            length = sizeof( *pl_agps_TcpIpDisconnectReq );
            break;
        }

        case SOCKET_AGPS_SUPL_TCPIP_DISCONNECT_IND: /** \ref t_agps_TcpIpDisconnectInd */
        {
            t_agps_TcpIpDisconnectInd  *pl_agps_TcpIpDisconnectInd=NULL;
            length = sizeof( *pl_agps_TcpIpDisconnectInd );
            break;
        }

        case SOCKET_AGPS_SUPL_TCPIP_DISCONNECT_ERR: /** \ref t_agps_TcpIpDisconnectRsp */
        {
            t_agps_TcpIpDisconnectErr  *pl_agps_TcpIpDisconnectErr=NULL;
            length = sizeof( *pl_agps_TcpIpDisconnectErr);
            break;
        }

        case SOCKET_AGPS_SUPL_TCPIP_DISCONNECT_CNF: /** \ref t_agps_TcpIpDisconnectCnf */
        {
            t_agps_TcpIpDisconnectCnf  *pl_agps_TcpIpDisconnectCnf=NULL;
            length = sizeof( *pl_agps_TcpIpDisconnectCnf );
            break;
        }

        case SOCKET_AGPS_SUPL_MOBILE_INFO_IND: /** \ref t_agps_MobileInfoInd */
        {
            t_agps_MobileInfoInd  *pl_agps_MobileInfoInd=NULL;
            length = sizeof( *pl_agps_MobileInfoInd );
            break;
        }


        case SOCKET_AGPS_WIRELESS_NETWORK_INFO_IND: /** \ref t_agps_MobileInfoInd */
        {
            t_agps_WLANInfoInd  *pl_agps_WlanInfoInd=NULL;
            length = sizeof( *pl_agps_WlanInfoInd );
            break;
        }

        case SOCKET_AGPS_SUPL_SMS_PUSH_IND: /** \ref t_agps_SmsPushInd */
        {
            t_agps_SmsPushInd *p_Payload;
            p_Payload = (t_agps_SmsPushInd *) command->data->pData;

            /* The length of payload will be  ( type + length + data  ) for hash and for sms */
            length = sizeof(int) + sizeof(int) + p_Payload->v_SmsPushInd.v_Length +
                     sizeof(int) + sizeof(int) + p_Payload->v_Hash.v_Length;
        }
        break;

        case SOCKET_AGPS_SUPL_SEND_DATA_REQ: /** \ref t_agps_SuplData */
        {
            t_agps_SuplData *p_Payload;
            p_Payload = (t_agps_SuplData *) command->data->pData;
            length    = sizeof(int) + sizeof(int) + sizeof(uint32_t) +
                        sizeof(int) + sizeof(int) + p_Payload->v_Data.v_Length;
        }
        break;

        case SOCKET_AGPS_SUPL_SEND_DATA_CNF: /** \ref t_agps_SuplSendDataCnf */
        {
            t_agps_SuplSendDataCnf *pl_agps_SuplSendDataCnf=NULL;
            length = sizeof( *pl_agps_SuplSendDataCnf );
            break;
        }

        case SOCKET_AGPS_SUPL_RECEIVE_DATA_IND: /** \ref t_agps_SuplData */
        {
            t_agps_SuplData *p_Payload;
            p_Payload = (t_agps_SuplData *) command->data->pData;
            length = sizeof(int) + sizeof(int) + sizeof(uint32_t) +
                     sizeof(int) + sizeof(int) + p_Payload->v_Data.v_Length;
        }
        break;

        case SOCKET_AGPS_NMEA_DATA:
            length = sizeof(int) + sizeof(int) + command->data->length;
        break;

        case SOCKET_AGPS_DEBUG_LOG_START:
            length = 0;
        break;

        case SOCKET_AGPS_DEBUG_LOG_RESP:
            length = sizeof(int) + sizeof(int) + command->data->length;
            break;
        case SOCKET_AGPS_SBEE_NOTIFY_CONNECTION:
        {
            t_sbee_si_UserNotificationParams *pl_sbee_si_UserNotificationParams;
            length = sizeof( *pl_sbee_si_UserNotificationParams );
            break;
        }

        case SOCKET_AGPS_SBEE_DATA:
        case SOCKET_AGPS_SBEE_RECV_DATA:
        {
            t_sbee_si_SbeeDataParams *p_Payload;
            p_Payload = (t_sbee_si_SbeeDataParams *) command->data->pData;

            length = sizeof(uint32_t) + sizeof(int) + sizeof(int) + sizeof(int) + sizeof(int) + p_Payload->a_SbeeData.v_Length;
        }
        break;

        case SOCKET_AGPS_SBEE_REQUEST_DATA_CONNECTION:
        {
            t_sbee_si_DataConnectionRequestParams *p_Payload;
            p_Payload = (t_sbee_si_DataConnectionRequestParams *) command->data->pData;

            length = sizeof(uint32_t) + sizeof(int) + sizeof(int) + sizeof(int) + sizeof(int) +
                p_Payload->a_ServerAddress.v_Length + sizeof(uint16_t) + sizeof(int) + sizeof(int);
        }
        break;

        case SOCKET_AGPS_SBEE_SET_DATA_CONNECTION_STATUS:
        {
            t_sbee_si_status_message_with_handle *pl_sbee_si_status_message_with_handle;
            length = sizeof( *pl_sbee_si_status_message_with_handle );
            break;
        }

        case SOCKET_AGPS_SBEE_SET_USER_RESPONSE:
        {
            t_sbee_si_status_message_with_handle *pl_sbee_si_status_message_with_handle;
            length = sizeof( *pl_sbee_si_status_message_with_handle );
            break;
        }
        case  SOCKET_AGPS_SBEE_START_EXTENDED_EPHEMERIS:
        {
            t_sbee_si_SbeeStart *pl_sbee_si_SbeeStart;
            length = sizeof( *pl_sbee_si_SbeeStart );
            break;
        }
        case  SOCKET_AGPS_SBEE_ABORT_EXTENDED_EPHEMERIS:
        {
            length = 0;
            break;
        }

        case SOCKET_AGPS_SBEE_SET_USER_CONFIGURATION:
        {
            t_sbee_si_UserConfiguration *pl_sbee_si_UserConfiguration;
            length = sizeof( *pl_sbee_si_UserConfiguration );
            break;
        }
        case SOCKET_AGPS_SBEE_SET_SERVER_CONFIGURATION:
            {
                t_sbee_si_ServerConfiguration *p_Payload;
                int index;
                p_Payload = (t_sbee_si_ServerConfiguration *) command->data->pData;
                length = sizeof(int) + sizeof(int) + MAX_SBEE_DEVICE_ID_LEN +
                         sizeof(int) + sizeof(int) + MAX_SBEE_INTEGRATOR_MODEL_NAME +
                         sizeof(int) + sizeof(int) + sizeof(uint8_t) +
                         sizeof(int) + sizeof(int) + sizeof(uint8_t);
                for(index = 0; index < p_Payload->v_NumSbeeServers; ++index)
                {
                    length += sizeof(int) + sizeof(int) + p_Payload->v_SbeeServersLength[index];
                }
            }
            break;

        case SOCKET_AGPS_SUPL_RECEIVE_DATA_CNF: /** \ref t_agps_SuplReceiveDataCnf */
        {
            t_agps_SuplReceiveDataCnf *pl_agps_SuplReceiveDataCnf=NULL;
            length = sizeof( *pl_agps_SuplReceiveDataCnf );
            break;
        }

        case SOCKET_AGPS_STATUS:
        {
            e_agps_Status  *pl_agps_Status=NULL;
            length = sizeof(*pl_agps_Status);
            break;
        }

        case SOCKET_AGPS_REGISTER_CLIENT:
        {
            t_agps_RegisterClientMask *pl_agps_RegisterClientMask=NULL;
            length = sizeof(*pl_agps_RegisterClientMask);
            break;
        }

        case SOCKET_AGPS_MOBILE_INFO_REQ:
            length = 0;
             break;

        case SOCKET_AGPS_WIRELESS_NETWORK_INFO_REQ:
            length =0;
            break;

        /* + PROD_TEST */
        case SOCKET_AGPS_PRODUCTION_TEST_START:
        {
            t_agps_CnoCriteria  *pl_agps_CnoCriteria=NULL;
            length = sizeof(*pl_agps_CnoCriteria);
        break;
        }

        case SOCKET_AGPS_PRODUCTION_TEST_DATA:
        {
            t_agps_ProductionTestData  *pl_agps_ProductionTestData=NULL;
            length = sizeof(*pl_agps_ProductionTestData);
        break;
        }
        /* - PROD_TEST */

        case SOCKET_AGPS_NI_NOTIFICATION:
        {

            length = 1;
            DEBUG_LOG_PRINT_LEV2(("SOCKET_AGPS_NI_NOTIFICATION pData %d\n",command->data->pData));
        }
        break;

        case SOCKET_AGPS_PRODUCTION_TEST_START_REQ:
        {

            t_agps_TestRequest *p_Payload = NULL;
            length = sizeof (*p_Payload);
            DEBUG_LOG_PRINT_LEV2(("SOCKET_AGPS_PRODUCTION_TEST_START_REQ length %d\n",length));
        }
        break;

        case SOCKET_AGPS_PRODUCTION_TEST_STOP_IND:
        {
            t_agps_StopRequest *p_Payload = NULL;
            length = sizeof(p_Payload);
            DEBUG_LOG_PRINT_LEV2(("SOCKET_AGPS_PRODUCTION_TEST_STOP_IND length %d\n",length));
        }
        break;

        /*Begin EEClient Development        194997 18 Jan 2012*/
        
        case SOCKET_AGPS_EE_GET_EPHEMERIS_IND:
        {
            t_agps_ExtendedEphInd  *pl_agps_ExtendedEphInd=NULL;
            length = sizeof(*pl_agps_ExtendedEphInd);
        }
        break;        
        case SOCKET_AGPS_REGISTER_EE_CLIENT_CAPABILITIES:
        {
            t_agps_RegisterClientMask *pl_agps_RegisterEEClientMask=NULL;
            length = sizeof(*pl_agps_RegisterEEClientMask);
            DEBUG_LOG_PRINT_LEV2(("SOCKET_AGPS_REGISTER_EE_CLIENT_CAPABILITIES length= %d\n",length));
        }
        break;        
        case SOCKET_AGPS_EE_GET_EPHEMERIS_RSP:
        {
            t_agps_NavDataList *pl_agps_NavDataList=NULL;
            length = sizeof(*pl_agps_NavDataList);
            DEBUG_LOG_PRINT_LEV2(("SOCKET_AGPS_EE_GET_EPHEMERIS_RSP length= %d\n",length));
        }
        break;
        
        /*End EEClient Development        194997 18 Jan 2012*/

        /*XYBRID Integration :194997*/
        case  SOCKET_AGPS_EE_GET_REFLOCATION_IND:
        {
            length = 0;
        }
        break;

        case SOCKET_AGPS_EE_GET_REFLOCATION_RSP:
        {
            t_agps_RefPosition *pl_agps_RefPositionList=NULL;
            length = sizeof(*pl_agps_RefPositionList);
            DEBUG_LOG_PRINT_LEV2(("SOCKET_AGPS_EE_GET_REFLOCATION_RSP length= %d\n",length));
        }
        break;        
        /*XYBRID Integration :194997*/
        case SOCKET_AGPS_SET_REFERENCE_TIME_IND:
        {
            t_agps_RefTimeData *pl_agps_RefTime=NULL;
            length = sizeof(*pl_agps_RefTime);
            DEBUG_LOG_PRINT_LEV2(("SOCKET_AGPS_SET_REFERENCE_TIME_IND length= %d\n",length));
        }
        break;
        case  SOCKET_AGPS_GET_REFERENCE_TIME_REQ:
        {
            length = 0;
        }
        break;

        case  SOCKET_AGPS_DELETE_SEED_REQ:
        {
            length = 0;
        }
        break;
        case  SOCKET_AGPS_EE_SET_BCEDATA_IND:
        {
            t_agps_NavDataBCE *pl_agps_NavDataBCE=NULL;
            length = sizeof(*pl_agps_NavDataBCE);
            DEBUG_LOG_PRINT_LEV2(("SOCKET_AGPS_EE_SET_BCEDATA_IND length= %d\n",length));
        }
        break;
        default :
            DEBUG_LOG_PRINT_LEV2(("Uknown command type %d\n",command->type ));
            length = -1;
            break;
    }
    return length;
}

int agpscodec_EncodeData(void *data, void * buffer, e_agps_DataTypes dataType, int length)
{
    int sizeTill = 0;
    int type = dataType;

    //copy type
    memcpy((uint8_t *)buffer + sizeTill, (void*)&type, sizeof(int));
    sizeTill += sizeof(int);

    //copy length
    memcpy((uint8_t *)buffer + sizeTill, (void*)&length, sizeof(int));
    sizeTill += sizeof(int);

    //copy data
    memcpy((uint8_t *)buffer + sizeTill, data, length);
    sizeTill += length;

    return sizeTill;
}

static int agpscodec_EncodeU8Array(t_agps_U8Array *u8Array, void * buffer)
{
    int sizeTill = 0;

    sizeTill = agpscodec_EncodeData((void *)u8Array->p_Pointer, buffer, AGPS_U8_ARRAY_TYPE, (int)u8Array->v_Length);
    return sizeTill;
}


/* +LMSqc26074 -Anil */
static int agpscodec_EncodeNmeaData(t_agps_Command *command, void *buffer)
{
    t_agps_U8Array tmpArray;
    int sizeTill = 0;
    tmpArray.p_Pointer = (uint8_t *)command->data->pData;

    tmpArray.v_Length = command->data->length;
    sizeTill += agpscodec_EncodeU8Array(&tmpArray, (uint8_t *)buffer + sizeTill);

    return sizeTill;
}
/* -LMSqc26074 -Anil */


static int agpscodec_EncodeSuplData(t_agps_Command *command, void *buffer)
{
    t_agps_SuplData *suplData;
    int sizeTill = 0;

    suplData = (t_agps_SuplData *)command->data->pData;

    sizeTill += agpscodec_EncodeData((void *)&(suplData->v_ConnectionHandle), (uint8_t *)buffer + sizeTill, AGPS_U32_TYPE, sizeof(uint32_t));

    sizeTill += agpscodec_EncodeU8Array(&suplData->v_Data, (uint8_t *)buffer + sizeTill);

    return sizeTill;
}


static int agpscodec_EncodeSmsPushInd(t_agps_Command *command, void *buffer)
{
    t_agps_SmsPushInd *smsPushInd;
    int sizeTill = 0;

    smsPushInd = (t_agps_SmsPushInd *)command->data->pData;

    sizeTill += agpscodec_EncodeU8Array(&smsPushInd->v_SmsPushInd, (uint8_t *)buffer + sizeTill);

    sizeTill += agpscodec_EncodeU8Array(&smsPushInd->v_Hash, (uint8_t *)buffer + sizeTill);

    return sizeTill;
}
static int agpscodec_EncodeSbeeConnectionRequest(t_agps_Command *command, void *buffer)
{
    int sizeTill = 0;
    t_sbee_si_DataConnectionRequestParams *connectionRequest;

    connectionRequest = (t_sbee_si_DataConnectionRequestParams *)command->data->pData;

    sizeTill += agpscodec_EncodeData((void *)&(connectionRequest->v_handle), buffer + sizeTill, AGPS_U32_TYPE, sizeof(uint32_t));
    sizeTill += agpscodec_EncodeU8Array(&connectionRequest->a_ServerAddress, buffer + sizeTill);
    sizeTill += agpscodec_EncodeData((void *)&(connectionRequest->v_PortNumber), buffer + sizeTill, AGPS_U16_TYPE, sizeof(uint16_t));

    return sizeTill;
}

static int agpscodec_EncodeSbeeData(t_agps_Command *command, void *buffer)
{
    int sizeTill = 0;
    t_sbee_si_SbeeDataParams *sendData;

    sendData = (t_sbee_si_SbeeDataParams *)command->data->pData;

    sizeTill += agpscodec_EncodeData((void *)&(sendData->v_handle), buffer + sizeTill, AGPS_U32_TYPE, sizeof(uint32_t));
    sizeTill += agpscodec_EncodeU8Array(&sendData->a_SbeeData, buffer + sizeTill);

    return sizeTill;
}

static int agpscodec_EncodeSbeeServerConfiguration(t_agps_Command *command, void *buffer)
{
    int sizeTill = 0, i;
    t_agps_U8Array tmpArray;
    uint8_t tmpValue;

    t_sbee_si_ServerConfiguration *serverConfiguration;
    serverConfiguration = (t_sbee_si_ServerConfiguration *)command->data->pData;

    tmpArray.p_Pointer = (uint8_t *)serverConfiguration->v_DeviceId;
    tmpArray.v_Length = MAX_SBEE_DEVICE_ID_LEN;
    sizeTill += agpscodec_EncodeU8Array(&tmpArray, buffer + sizeTill);

    tmpArray.p_Pointer = (uint8_t *)serverConfiguration->v_IntegratorModelId;
    tmpArray.v_Length = MAX_SBEE_INTEGRATOR_MODEL_NAME;
    sizeTill += agpscodec_EncodeU8Array(&tmpArray, buffer + sizeTill);

    tmpValue = serverConfiguration->v_RetryOption;
    sizeTill += agpscodec_EncodeData((void *)&(tmpValue), buffer + sizeTill, AGPS_U8_TYPE, sizeof(uint8_t));

    tmpValue = serverConfiguration->v_NumSbeeServers;
    sizeTill += agpscodec_EncodeData((void *)&(tmpValue), buffer + sizeTill, AGPS_U8_TYPE, sizeof(uint8_t));

    for(i = 0; i < serverConfiguration->v_NumSbeeServers; ++i)
    {
        tmpArray.p_Pointer = (uint8_t *)serverConfiguration->v_SbeeServers[i];
        tmpArray.v_Length = serverConfiguration->v_SbeeServersLength[i];
        sizeTill += agpscodec_EncodeU8Array(&tmpArray, buffer + sizeTill);
    }
    return sizeTill;
}

/* +ER:270717 */
static void agpscodec_AddCheckSum(void *buffer, int position)
{
    char* p_Buffer = (char*)buffer;
    uint8_t    checkSum = 0;
    int   index    = 0;

    for( index = 0; index < position ; index++)
    {
        checkSum ^= p_Buffer[index];
    }

    memcpy(p_Buffer+position, &checkSum, AGPS_CODEC_CS_SIZE);

#ifndef AGPS_QT_FTR

#ifdef DEBUG_LOG_LEV2
    {
        int i=0;
        char* buff = NULL;
        int count = 0;
        int sizeTill = position+1;

        buff = (char *)malloc( 2*sizeTill + 1 );

        for( i=0 ; i < sizeTill ; i++ )
        {
            count += sprintf( buff + count , "%02X" , *((char *)p_Buffer+i) );
        }

        buff[ 2*sizeTill ] = '\0';

        DEBUG_LOG_PRINT_LEV2(("Data : %s" , buff));

        free( buff );
        buff = NULL;
    }
#endif
#endif /* AGPS_QT_FTR */
}
/* -ER:270717 */

/*****************************************************************************/
/* codec_EncodeCommand : Encode the Commad                                   */
/*****************************************************************************/

int agpscodec_EncodeCommand(t_agps_Command *command , t_Buffer *p_RetBuffer )
{
    int type, sizeTill = 0,payLoadLength;
/* +ER:270717 */
    unsigned int header = AGPS_CODEC_HEADER;
/* -ER:270717 */
    char *p_Buffer;
    uint8_t *niNotify;

    DEBUG_LOG_PRINT_LEV2(("Entering agpscodec_EncodeCommand\n"));

    if( ( command == NULL ) || ( p_RetBuffer == NULL ) )
    {
        DEBUG_LOG_PRINT_LEV2(("Invalid parameter : buffer %p , command %p\n" , p_RetBuffer , command ));
        return 0;
    }

    type = command->type;

    payLoadLength = agpscodec_GetLengthOfPayload( command );

    DEBUG_LOG_PRINT_LEV2(("agpscodec_EncodeCommand : %d , payload : %d bytes\n" , type , payLoadLength));
/* +ER:270717 */
    /* Now allocate sufficient memory for payload length + command type + overall length */
    p_Buffer = (char *) malloc( payLoadLength +sizeof(type) + sizeof(payLoadLength) + AGPS_CODEC_HEADER_N_CS_SIZE );

    memset( p_Buffer, 0, payLoadLength +sizeof(type) + sizeof(payLoadLength) + AGPS_CODEC_HEADER_N_CS_SIZE);
    memcpy( p_Buffer, &header,  AGPS_CODEC_HEADER_SIZE);

    memcpy( p_Buffer+AGPS_CODEC_HEADER_SIZE, (void*)&type, sizeof(int));

    DEBUG_LOG_PRINT_LEV2(("type is copied to the buffer\n"));

    sizeTill += sizeof(int)+AGPS_CODEC_HEADER_SIZE;
/* -ER:270717 */
    if(payLoadLength != -1)
    {
        DEBUG_LOG_PRINT_LEV2(("Copying data length\n"));
        memcpy(p_Buffer + sizeTill, (void*)&payLoadLength, sizeof(payLoadLength));
        sizeTill += sizeof(payLoadLength);

        if( payLoadLength > 0 )
        {
            if( command->type == SOCKET_AGPS_SUPL_SMS_PUSH_IND )
            {

                sizeTill += agpscodec_EncodeSmsPushInd(command, p_Buffer + sizeTill);

            }
            else if( command->type == SOCKET_AGPS_SUPL_SEND_DATA_REQ ||
                command->type == SOCKET_AGPS_SUPL_RECEIVE_DATA_IND )
            {
                sizeTill += agpscodec_EncodeSuplData(command, p_Buffer + sizeTill);
            }
            else if( command->type == SOCKET_AGPS_SBEE_DATA || command->type == SOCKET_AGPS_SBEE_RECV_DATA )
            {
                DEBUG_LOG_PRINT_LEV2(("SOCKET_AGPS_SBEE_DATA"));
                sizeTill += agpscodec_EncodeSbeeData(command, p_Buffer + sizeTill);
            }
            else if( command->type == SOCKET_AGPS_SBEE_REQUEST_DATA_CONNECTION )
            {
                DEBUG_LOG_PRINT_LEV2(("SOCKET_AGPS_SBEE_REQUEST_DATA_CONNECTION"));
                sizeTill += agpscodec_EncodeSbeeConnectionRequest(command, p_Buffer + sizeTill );
            }
            else if( command->type == SOCKET_AGPS_SBEE_SET_SERVER_CONFIGURATION )
            {
                DEBUG_LOG_PRINT_LEV2(("SOCKET_AGPS_SBEE_SET_SERVER_CONFIGURATION"));
                sizeTill += agpscodec_EncodeSbeeServerConfiguration(command, p_Buffer + sizeTill);
                DEBUG_LOG_PRINT_LEV2(("agpscodec_EncodeCommand: number of bytes encoded %d", sizeTill));
            }
            /* +LMSqc26074 -Anil */
            else if( command->type == SOCKET_AGPS_NMEA_DATA )
            {
                sizeTill += agpscodec_EncodeNmeaData(command, p_Buffer + sizeTill);
            }
            /* -LMSqc26074 -Anil */
            else if( command->type == SOCKET_AGPS_DEBUG_LOG_RESP )
            {
                DEBUG_LOG_PRINT_LEV2(("encoding SOCKET_AGPS_DEBUG_LOG_RESP data"));
                sizeTill += agpscodec_EncodeNmeaData(command, p_Buffer + sizeTill);

            }
            else if(command->type == SOCKET_AGPS_NI_NOTIFICATION )
            {
                DEBUG_LOG_PRINT_LEV2(("encoding SOCKET_AGPS_NI_NOTIFICATION"));
                niNotify = (uint8_t *)&command->data->pData;
                memcpy(p_Buffer + sizeTill, (void*)niNotify, sizeof(uint8_t));
                sizeTill += sizeof(uint8_t);
            }
            else if (command->type == SOCKET_AGPS_PRODUCTION_TEST_START_REQ)
            {
                DEBUG_LOG_PRINT_LEV2(("encoding SOCKET_AGPS_PRODUCTION_TEST_START_REQ sizeTill %d",sizeTill));
                t_agps_TestRequest *pgpsTREQData;
                pgpsTREQData = (t_agps_TestRequest *)command->data->pData;
                memcpy(p_Buffer + sizeTill, (void*)pgpsTREQData, sizeof(t_agps_TestRequest));
                sizeTill += sizeof(t_agps_TestRequest);
                DEBUG_LOG_PRINT_LEV2(("encoding SOCKET_AGPS_PRODUCTION_TEST_START_REQ sizeTill %d",sizeTill));
            }

            else if (command->type == SOCKET_AGPS_PRODUCTION_TEST_STOP_IND)
            {
                DEBUG_LOG_PRINT_LEV2(("encoding SOCKET_AGPS_PRODUCTION_TEST_STOP_IND sizeTill %d",sizeTill));
                t_agps_StopRequest *sendData;
                sendData = (t_agps_StopRequest *)command->data->pData;
                memcpy(p_Buffer + sizeTill, (void*)sendData, sizeof(t_agps_StopRequest));
                sizeTill += sizeof(t_agps_StopRequest);
                DEBUG_LOG_PRINT_LEV2(("encoding SOCKET_AGPS_PRODUCTION_TEST_STOP_IND sizeTill %d",sizeTill));
            }
            else
            {
                DEBUG_LOG_PRINT_LEV2(("Copying data\n"));
                memcpy(p_Buffer + sizeTill, (void*)((t_agps_Packet *)(command->data))->pData, payLoadLength);
                sizeTill += payLoadLength;
            }
        }
/* +ER:270717 */
        agpscodec_AddCheckSum( p_Buffer, sizeTill );
        sizeTill += AGPS_CODEC_CS_SIZE;
/* -ER:270717 */
    }
    else
    {
        DEBUG_LOG_PRINT_LEV2(("ERROR! Invalid value for length!"));
    }

    DEBUG_LOG_PRINT_LEV2(("Command encoded.\n"));
#ifndef AGPS_QT_FTR
#ifdef DEBUG_LOG_LEV2
    {
        int i=0;
        char* buff = NULL;
        int count = 0;

        buff = (char *)malloc( 2*sizeTill + 1 );

        for( i=0 ; i < sizeTill ; i++ )
        {
            count += sprintf( buff + count , "%02X" , *((char *)p_Buffer+i) );
        }

        buff[ 2*sizeTill ] = '\0';

        DEBUG_LOG_PRINT_LEV2(("Data : %s" , buff));

        free( buff );
        buff = NULL;
    }
#endif
#endif /*AGPS_QT_FTR*/

    if( payLoadLength >= 0 )
    {
        p_RetBuffer->p_Buffer = p_Buffer;
        p_RetBuffer->v_Length = sizeTill;
        return sizeTill;
    }
    else
    {
        free( p_Buffer );
        p_Buffer = NULL;
        p_RetBuffer->p_Buffer = NULL;
        p_RetBuffer->v_Length = 0;
        return 0;
    }

}

static int agpscodec_DecodeData(void *data, char *buffer, e_agps_DataTypes *dataType, int *length)
{
    int type;
    int sizeTill = 0;


    memcpy(&type, buffer + sizeTill, sizeof(int));
    sizeTill += sizeof(int);
    *dataType = (e_agps_DataTypes)type;

    memcpy(length, buffer + sizeTill, sizeof(int));
    sizeTill += sizeof(int);

    memcpy(data, buffer + sizeTill, *length);
    sizeTill += *length;

    return sizeTill;
}

static int agpscodec_DecodeData_Alloc(void **data, char *buffer, e_agps_DataTypes *dataType, int *length)
{
    int type;
    int sizeTill = 0;

    memcpy(&type, buffer + sizeTill, sizeof(int));
    sizeTill += sizeof(int);
    *dataType = (e_agps_DataTypes)type;

    memcpy(length, buffer + sizeTill, sizeof(int));
    sizeTill += sizeof(int);

    *data = malloc(*length);

    memcpy(*data, buffer + sizeTill, *length);
    sizeTill += *length;

    return sizeTill;
}

static int agpscodec_DecodeU8Array(t_agps_U8Array *u8Array, char *buffer)
{
    int length;
    e_agps_DataTypes type;
    int sizeTill = 0;

    sizeTill += agpscodec_DecodeData_Alloc((void *)&u8Array->p_Pointer, buffer, &type, &length);
    u8Array->v_Length = length;

    return sizeTill;
}

/* + LMSqc26074 -Anil */
static int agpscodec_DecodeNmeaData(t_agps_Command *command, char *buffer)
{
    int sizeTill = 0;
    t_agps_U8Array tmpArray;
    sizeTill += agpscodec_DecodeU8Array(&tmpArray, buffer + sizeTill);
    command->data->pData = tmpArray.p_Pointer;
    command->data->length = tmpArray.v_Length;
    return sizeTill;
}
/* -LMSqc26074 - Anil */

static int agpscodec_DecodeSmsPushInd(t_agps_Command *command, char *buffer)
{
    int sizeTill = 0;
    t_agps_SmsPushInd *smsPushInd;
    smsPushInd = (t_agps_SmsPushInd *)malloc( sizeof( *smsPushInd ) );
    sizeTill += agpscodec_DecodeU8Array(&smsPushInd->v_SmsPushInd, buffer + sizeTill);
    sizeTill += agpscodec_DecodeU8Array(&smsPushInd->v_Hash, buffer + sizeTill);
    command->data->length = smsPushInd->v_Hash.v_Length + sizeof(smsPushInd->v_Hash.v_Length) +
                                  smsPushInd->v_SmsPushInd.v_Length + sizeof(smsPushInd->v_SmsPushInd.v_Length);
    command->data->pData = smsPushInd;
    return sizeTill;
}


static int agpscodec_DecodeSuplData(t_agps_Command *command, char *buffer)
{
    int sizeTill = 0;
    t_agps_SuplData *suplData;
    suplData = (t_agps_SuplData *)malloc( sizeof( *suplData ) );

    //to skip type and size (both are size of integers)
//    buffer += sizeof(int) * 2;
    sizeTill += sizeof(int) * 2;

    memcpy(&suplData->v_ConnectionHandle, buffer + sizeTill, sizeof( suplData->v_ConnectionHandle ));
    sizeTill += sizeof( suplData->v_ConnectionHandle );

    sizeTill += agpscodec_DecodeU8Array(&suplData->v_Data, buffer + sizeTill);

    command->data->length = suplData->v_Data.v_Length + sizeof(suplData->v_Data.v_Length) + sizeof(int);

    command->data->pData = suplData;

     DEBUG_LOG_PRINT_LEV2(("agpscodec_DecodeSuplData : handle : %d , length : %d",suplData->v_ConnectionHandle,suplData->v_Data.v_Length));

    return sizeTill;
}
static int agpscodec_DecodeSbeeConnectionRequest(t_agps_Command *command, char *buffer)
{
    int sizeTill = 0;
    e_agps_DataTypes dataType;
    int length;

    t_sbee_si_DataConnectionRequestParams *connectionReq;

    connectionReq = (t_sbee_si_DataConnectionRequestParams *)malloc( sizeof(t_sbee_si_DataConnectionRequestParams) );

    sizeTill += agpscodec_DecodeData( (void *)&connectionReq->v_handle,buffer + sizeTill, &dataType, &length);

    sizeTill += agpscodec_DecodeU8Array(&connectionReq->a_ServerAddress, buffer + sizeTill);

    sizeTill += agpscodec_DecodeData( (void *)&connectionReq->v_PortNumber, buffer + sizeTill, &dataType, &length);

    command->data->length = connectionReq->a_ServerAddress.v_Length + sizeof(connectionReq->a_ServerAddress.v_Length) + sizeof(connectionReq->v_PortNumber) + sizeof(int);

    command->data->pData = connectionReq;

    return sizeTill;
}

static int agpscodec_DecodeSbeeData(t_agps_Command *command, char *buffer)
{
    int sizeTill = 0;
    e_agps_DataTypes dataType;
    int length;

    t_sbee_si_SbeeDataParams *data;
    data = (t_sbee_si_SbeeDataParams *)malloc( sizeof(t_sbee_si_SbeeDataParams) );

    sizeTill += agpscodec_DecodeData( (void *)&data->v_handle, buffer + sizeTill, &dataType, &length);

    sizeTill += agpscodec_DecodeU8Array(&data->a_SbeeData, buffer + sizeTill);

    command->data->length = data->a_SbeeData.v_Length + sizeof(data->a_SbeeData.v_Length) + sizeof(int);

    command->data->pData = data;

    return sizeTill;
}

static int agpscodec_DecodeSbeeServerConfiguration(t_agps_Command *command, char *buffer)
{
    int sizeTill = 0, i;
    t_agps_U8Array tmpArray;
    e_agps_DataTypes dataType;
    int length;

    t_sbee_si_ServerConfiguration *serverConfiguration;
    serverConfiguration = (t_sbee_si_ServerConfiguration *)malloc(sizeof(*serverConfiguration));

    memset( serverConfiguration , 0 , sizeof(t_sbee_si_ServerConfiguration) );
    sizeTill += agpscodec_DecodeU8Array( (void *)&tmpArray, buffer + sizeTill);
    memcpy(serverConfiguration->v_DeviceId, tmpArray.p_Pointer, tmpArray.v_Length);
    DEBUG_LOG_PRINT_LEV2(("agpscodec_DecodeSbeeServerConfiguration: device Id is %s", serverConfiguration->v_DeviceId));

    free( tmpArray.p_Pointer );
    tmpArray.p_Pointer = NULL;

    sizeTill += agpscodec_DecodeU8Array( (void *)&tmpArray, buffer + sizeTill);
    memcpy(serverConfiguration->v_IntegratorModelId, tmpArray.p_Pointer, tmpArray.v_Length);
    DEBUG_LOG_PRINT_LEV2(("agpscodec_DecodeSbeeServerConfiguration: integrator model Id is %s", serverConfiguration->v_IntegratorModelId));

    free( tmpArray.p_Pointer );
    tmpArray.p_Pointer = NULL;

    sizeTill += agpscodec_DecodeData((void *)&serverConfiguration->v_RetryOption, buffer + sizeTill, &dataType, &length);

    DEBUG_LOG_PRINT_LEV2(("agpscodec_DecodeSbeeServerConfiguration: Retry option is %d", serverConfiguration->v_RetryOption));

    sizeTill += agpscodec_DecodeData((void *)&serverConfiguration->v_NumSbeeServers, buffer + sizeTill, &dataType, &length);
    DEBUG_LOG_PRINT_LEV2(("agpscodec_DecodeSbeeServerConfiguration: Number of servers is %d", serverConfiguration->v_NumSbeeServers));


    for(i = 0; i < serverConfiguration->v_NumSbeeServers; ++i)
    {
        sizeTill += agpscodec_DecodeU8Array( (void *)&tmpArray, buffer + sizeTill);
        serverConfiguration->v_SbeeServers[i] = (int8_t *)tmpArray.p_Pointer;
        serverConfiguration->v_SbeeServersLength[i] = tmpArray.v_Length;
        DEBUG_LOG_PRINT_LEV2(("agpscodec_DecodeSbeeServerConfiguration: Server Address[%d] is %s", i, serverConfiguration->v_SbeeServers[i]));
    }

    command->data->length = sizeof(t_sbee_si_ServerConfiguration) + serverConfiguration->v_SbeeServersLength[0] + serverConfiguration->v_SbeeServersLength[1] + serverConfiguration->v_SbeeServersLength[2];

    command->data->pData = serverConfiguration;

    return sizeTill;

}

/* +ER:270717 */
static bool agpscodec_IsDataValid(char *buffer, int length)
{
    char* p_Buffer  = (char*)buffer;
    unsigned char recdCS;
    uint8_t    checkSum  = 0;
    int   index     = 0;
    unsigned int   header    = 0;
    bool  returnVal = FALSE;

    memcpy(&header, p_Buffer, sizeof(header));

    if( header == (unsigned int)AGPS_CODEC_HEADER )
    {
        for( index = 0; index < (int)(length+AGPS_CODEC_HEADER_SIZE+AGPS_CODEC_LENGTH_N_TYPE_SIZE) ; index++)
        {
            checkSum ^= p_Buffer[index];
        }

        DEBUG_LOG_PRINT_LEV2(("DBG : agpscodec_IsDataValid - CheckSum : %x", checkSum));

        recdCS = p_Buffer[length+AGPS_CODEC_HEADER_SIZE+AGPS_CODEC_LENGTH_N_TYPE_SIZE];
        if( checkSum == recdCS)
        {
            returnVal = TRUE;
        }
    }
    else
    {
        DEBUG_LOG_PRINT_LEV2(("ERR : agpscodec_IsDataValid - Header : %x", header));
    }

    return returnVal;
}
static bool agpscodec_IsHeaderPresent(char *buffer)
{
    char* p_Buffer  = (char*)buffer;
    unsigned int   header    = 0;
    bool  returnVal = FALSE;

    memcpy(&header, p_Buffer, sizeof(header));

    if( header == AGPS_CODEC_HEADER )
    {
        returnVal = TRUE;
    }
    else
    {
        DEBUG_LOG_PRINT_LEV2(("ERR : agpscodec_IsHeaderPresent - Header : %x", header));
    }

    return returnVal;
}
/* -ER:270717 */

uint32_t agpscodec_DecodeCommand(t_agps_Command *command, t_Buffer v_SubmitBuffer)
{

    int32_t length, type;
    uint32_t sizeTill = 0;
    t_agps_Packet *packet;
    packet = malloc(sizeof(*packet));
    char *buffer;
/* +ER:270717 */
    bool isDataValid     = FALSE;
    bool isHeaderPresent = FALSE;
/* -ER:270717 */

    buffer =  v_SubmitBuffer.p_Buffer;

/* +ER:270717 */
    isHeaderPresent = agpscodec_IsHeaderPresent(buffer);

    if ( isHeaderPresent )
    {

        sizeTill = AGPS_CODEC_HEADER_SIZE;
/* -ER:270717 */
        command->data = packet;
        DEBUG_LOG_PRINT_LEV2(("Entering agpscodec_DecodeCommand\n"));
/* +ER:270717 */
        memcpy(&type, buffer+AGPS_CODEC_HEADER_SIZE, sizeof(int));
/* -ER:270717 */
        command->type = type;

        sizeTill += sizeof(int);

        DEBUG_LOG_PRINT_LEV2(("Decoding Length\n"));
        memcpy(&((t_agps_Packet *)(command->data))->length, buffer + sizeTill, sizeof(length));
        sizeTill += sizeof(length);

        length = ((t_agps_Packet *)(command->data))->length;

/* +ER:270717 */
#ifndef AGPS_QT_FTR
#ifdef DEBUG_LOG_LEV2
        {
            int i=0;
            char* buff = NULL;
            int count = 0;
            int printlength = length+AGPS_CODEC_HEADER_N_CS_SIZE+2*sizeof(int);

            buff = (char *)malloc( 2*printlength + 1 );

            for( i=0 ; i < printlength ; i++ )
            {
                count += sprintf( buff + count , "%02X" , *((char *)buffer+i) );
            }

            buff[ 2*printlength ] = '\0';

            DEBUG_LOG_PRINT_LEV2(("Decode Data : %s" , buff));

            free( buff );
            buff = NULL;
        }

#endif
#endif /* AGPS_QT_FTR */

        isDataValid = agpscodec_IsDataValid(buffer, length);

        if( isDataValid )
        {
/* -ER:270717 */
            if( length > 0 )
            {
                DEBUG_LOG_PRINT_LEV2(("Decoding data\n"));
                if( command->type == SOCKET_AGPS_SUPL_SMS_PUSH_IND )
                {
                    sizeTill += agpscodec_DecodeSmsPushInd(command, buffer + sizeTill);
                }
                else if( command->type == SOCKET_AGPS_SUPL_SEND_DATA_REQ ||
                    command->type == SOCKET_AGPS_SUPL_RECEIVE_DATA_IND )
                {
                    sizeTill += agpscodec_DecodeSuplData(command, buffer + sizeTill);
                }
                /* +LMSqc26074 -Anil */
                else if( command->type == SOCKET_AGPS_NMEA_DATA )
                {
                    sizeTill += agpscodec_DecodeNmeaData(command, buffer + sizeTill);
                }
                else if( command->type == SOCKET_AGPS_SBEE_DATA || command->type == SOCKET_AGPS_SBEE_RECV_DATA )
                {
                    DEBUG_LOG_PRINT_LEV2(("Decoding SOCKET_AGPS_SBEE_DATA\n"));
                    sizeTill += agpscodec_DecodeSbeeData(command, buffer + sizeTill);
                }
                else if( command->type == SOCKET_AGPS_SBEE_REQUEST_DATA_CONNECTION )
                {
                    DEBUG_LOG_PRINT_LEV2(("Decoding SOCKET_AGPS_SBEE_REQUEST_DATA_CONNECTION\n"));
                    sizeTill += agpscodec_DecodeSbeeConnectionRequest(command, buffer + sizeTill );
                }
                else if( command->type == SOCKET_AGPS_SBEE_SET_SERVER_CONFIGURATION )
                {
                    DEBUG_LOG_PRINT_LEV2(("Decoding SOCKET_AGPS_SBEE_SET_SERVER_CONFIGURATION\n"));
                    sizeTill += agpscodec_DecodeSbeeServerConfiguration(command, buffer + sizeTill);
                }
                /* -LMSqc26074 -Anil */
                else if( command->type == SOCKET_AGPS_DEBUG_LOG_RESP )
                {
                    DEBUG_LOG_PRINT_LEV2(("Decoding SOCKET_AGPS_DEBUG_LOG_RESP\n"));
                    sizeTill += agpscodec_DecodeNmeaData(command, buffer + sizeTill);
                }
                else if(command->type == SOCKET_AGPS_NI_NOTIFICATION )
                {
                    DEBUG_LOG_PRINT_LEV2(("Decoding SOCKET_AGPS_NI_NOTIFICATION"));
                    ((t_agps_Packet *)(command->data))->pData = malloc( length );
                    memset(((t_agps_Packet *)(command->data))->pData,0,length);
                    memcpy((void *)command->data->pData, buffer + sizeTill, sizeof(uint8_t));
                    sizeTill += length;
                }
                else if (command->type == SOCKET_AGPS_PRODUCTION_TEST_START_REQ)
                {
                    DEBUG_LOG_PRINT_LEV2(("Decoding SOCKET_AGPS_PRODUCTION_TEST_START_REQ"));
                    ((t_agps_Packet *)(command->data))->pData = malloc( length );
                    memset(((t_agps_Packet *)(command->data))->pData,0,length);
                    memcpy((void *)command->data->pData, buffer + sizeTill, sizeof(t_agps_TestRequest));
                    sizeTill += length;
                }
                else if (command->type == SOCKET_AGPS_PRODUCTION_TEST_STOP_IND)
                {
                    DEBUG_LOG_PRINT_LEV2(("Decoding SOCKET_AGPS_PRODUCTION_TEST_STOP_IND"));
                    ((t_agps_Packet *)(command->data))->pData = malloc( length );
                    memset(((t_agps_Packet *)(command->data))->pData,0,length);
                    memcpy((void *)command->data->pData, buffer + sizeTill, sizeof(t_agps_StopRequest));
                    sizeTill += length;
                }
                else
                {
                    ((t_agps_Packet *)(command->data))->pData = malloc( length );
                    memcpy(((t_agps_Packet *)(command->data))->pData, (buffer + sizeTill), length );
                    sizeTill += length;
                }
            }
            else
            {
                DEBUG_LOG_PRINT_LEV2(("Data length is %d\n", length ));
                ((t_agps_Packet *)(command->data))->pData = NULL;
            }

/* +ER:270717 */
            sizeTill += AGPS_CODEC_CS_SIZE;
/* -ER:270717 */
            DEBUG_LOG_PRINT_LEV2(("Command decoded\n" ));
/* +ER:270717 */
        }
        else
        {
            DEBUG_LOG_PRINT_LEV2(("Command not decoded, IsDataValid %u", isDataValid ));
            ((t_agps_Packet *)(command->data))->pData = NULL;

            free( packet );
            packet = NULL;
            command->data = NULL;
            sizeTill = 0;
        }
    }
    else
    {
        DEBUG_LOG_PRINT_LEV2(("Command not decoded, isHeaderPresent %u", isHeaderPresent ));

        free( packet );
        packet = NULL;
        sizeTill = 0;
    }
/* -ER:270717 */
    return sizeTill;
}

