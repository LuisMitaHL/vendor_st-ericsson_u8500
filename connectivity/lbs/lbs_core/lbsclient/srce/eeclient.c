/*
* Copyright (C) ST-Ericsson 2009
*
* EEclient.c
* Date: 17-01-2012
* Author: Mohan Babu
* Email : mohanbabu.n@stericsson.com
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
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "agps_client.h"
#include "clientutils.h"
#include "EEClient.h"



#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define  LOG_TAG  "EEclient"


#define  SOFTWARE_VER  "V5.20"  /*Note: This Value not increase 254 characters*/


static t_EEClient_Callbacks     *eeClientCallbacks = NULL;
t_clientUtils_GpsState eeClientState;
time_t Requestedtime = 0;

#define CUT_OFF_TIME    (10)  /*10 Seconds*/


void EEClient_getEphemeris(t_EEClient_EphemerisData *GetEphemeris);
void EEClient_getRefLocation(void);
void EEClient_getRefTime(void);
void EEClient_DeleteSeed(void);
void EEClient_setBroadcastEphemeris(t_EEClient_NavDataBCE *pBCEData);




/**
  *  @brief  This function is for initializing the lbs EE client. Thread is created in application process.
  *  @param callbacks          Structure which has pointer to various call back functions
  *  @return                   returns 0 on success
  */
/*
  * Register the Call Backs
  *
  *   
  */

void EEClient_EventHandler(t_agps_Command command)
{
    DEBUG_LOG_PRINT_LEV2(("EEClient_EventHandler : revcd commands %d\n", command.type));
    switch( command.type )
    {
    
        case SOCKET_AGPS_EE_GET_EPHEMERIS_IND:
        {
            EEClient_getEphemeris((t_EEClient_EphemerisData*)((t_agps_Packet *)(command.data))->pData);
            
        }
        break;
        
        case SOCKET_AGPS_EE_GET_REFLOCATION_IND:
        {
/*XYBRID Integration :194997*/
            EEClient_getRefLocation();
/*XYBRID Integration :194997*/
        }
        break;

        case SOCKET_AGPS_GET_REFERENCE_TIME_REQ:
        {
            EEClient_getRefTime();
        }
        break;
        
        case SOCKET_AGPS_DELETE_SEED_REQ:
        {
            EEClient_DeleteSeed();
        }
        break;

        case SOCKET_AGPS_EE_SET_BCEDATA_IND:
        {
            EEClient_setBroadcastEphemeris((t_EEClient_NavDataBCE*)((t_agps_Packet *)(command.data))->pData);
        }
        break;

        case SOCKET_AGPS_STATUS:
        {
            clientUtils_ProcessStatus( &eeClientState, (e_agps_Status*)((t_agps_Packet *)(command.data))->pData );
        }
        break;
        

        default:
        {
            DEBUG_LOG_PRINT_LEV2(("EEClient_EventHandler Default Hit"));
        }
        break;    
    
    
    }


}


int EEClient_Init( t_EEClient_Callbacks *callbacks )
{
    int retVal;
    DEBUG_LOG_PRINT_LEV2(("EEClient_Init Called"));

    if(NULL == callbacks)
    {
        DEBUG_LOG_PRINT_LEV2(("EEClient_Init returned EEClient_FAILURE"));
        return EEClient_FAILURE;
    }

    eeClientCallbacks = callbacks;
    eeClientState.callback = EEClient_EventHandler;
    
    retVal = clientUtils_InitEEState( &eeClientState );

    return EEClient_SUCESS;
}




void EEClient_SendCommandToServer( t_agps_Command *command )
{
    clientUtils_SendCommandToServer( &eeClientState, command );
}


/**
  *  @brief  This function is used to Inject Ephemeris data. This is triggered on request for Ephemeris data Indication.
  *  @see EEClient_getEphemeris_callback for confirming the request
  *  @return                   returns 0 on success
  */
int EEClient_SetEphemerisData(t_EEClient_NavData *ephemerisData)
{
    t_agps_Command command;
    t_agps_Packet packet;
    uint8_t v_Index = 0;
    /*
        Frequency Channel is a work around till RXN support this features.
        The frequency channels mapping is obtianed from the website http://www.glonass-center.ru/en/CUSGLONASS/index.php
        Please Note:-
        Array indexing is from Zero, but satellite numbering is from 1, so while using this frequency index, Slot number  -1 should be used as index.
    */
    /*
        Checked agianst the website: http://www.glonass-center.ru/en/CUSGLONASS/index.php
        15-June 2012
        05-July  2012
        13-July  2012
        19-July  2012
        22-Aug  2012
        11-Sep  2012
        */
    int8_t  a_FreqIndex[24] = {1,-4,5,6,1,-4,5,6,-2,-7,0,-1,-2,-7,0,-1,4,-3,3,2,4,-3,3,2};

    for(v_Index = 0 ; v_Index < ephemerisData->v_numEntriesGlonass ; v_Index++)
    {
        ephemerisData->v_FullEphGlonass[v_Index].freqChannel = a_FreqIndex[ephemerisData->v_FullEphGlonass[v_Index].slot - 1];
    }

    packet.length = sizeof(t_EEClient_NavData);
    packet.pData = (void *)ephemerisData;
    
    DEBUG_LOG_PRINT_LEV2(("EEClient_SetEphemerisData packet.length=%d",packet.length));
    
    command.type = SOCKET_AGPS_EE_GET_EPHEMERIS_RSP;
    command.data = (void *) ( &packet );


    EEClient_SendCommandToServer( &command );

    return EEClient_SUCESS;

}


/**
  *  @brief  This function is used to Inject Position data. This is triggered on request for Position data Indication.
  *  @see EEClient_getRefLocation_callback for confirming the request
  *  @return                   returns 0 on success
  */
int EEClient_SetRefPositionData(t_EEClient_RefPosition *refPositionData)
{
    t_agps_Command command;
    t_agps_Packet packet;

    Requestedtime = 0;
    DEBUG_LOG_PRINT_LEV2(("EEClient_SetRefPositionData Called"));
    packet.length = sizeof(t_EEClient_RefPosition);
    packet.pData = (void *)refPositionData;

    command.type = SOCKET_AGPS_EE_GET_REFLOCATION_RSP;
    command.data = (void *) ( &packet );


    EEClient_SendCommandToServer( &command );

    return EEClient_SUCESS;

}


/**
  *  @brief  This function is used to Inject Time data. This is triggered on request for Position data Indication.
  *  @see EEClient_getRefLocation_callback for confirming the request
  *  @return                   returns 0 on success
  */

int EEClient_SetRefTimeData(t_EEClient_RefTimeData *refTimeData)
{
    t_agps_Command command;
    t_agps_Packet packet;
    packet.length = sizeof(t_EEClient_RefTimeData);
    packet.pData = (void *)refTimeData;
    command.type = SOCKET_AGPS_SET_REFERENCE_TIME_IND;
    command.data = (void *) ( &packet );
    EEClient_SendCommandToServer( &command );

    DEBUG_LOG_PRINT_LEV2(("Reference Time from MSL: %u %u %u %u %u",refTimeData->weekNum,refTimeData->TOWmSec,refTimeData->TOWnSec,refTimeData->TAccmSec,refTimeData->TAccnSec));

    
    return EEClient_SUCESS;
}



/**
  *  @brief  This function is used to request for Broadcast Ephemeris from the Chip.
  *  @see EEClient_getBroadcastEphemeris_callback for confirming the request
  *  @return                   returns 0 on success
  */
int EEClient_GetBroadcastEphemeris(e_EEClient_ConstelType constelType)
{
    t_agps_Command command;
    t_agps_Packet packet;

    packet.length = sizeof(e_EEClient_ConstelType);
    DEBUG_LOG_PRINT_LEV2(("EEClient_GetBroadcastEphemeris Enum Length=%d",packet.length));
    packet.pData = (void *)constelType;

    command.type = SOCKET_AGPS_EE_WRITE_BROADCASTEPHEMERIS_REQ;
    command.data = (void *) ( &packet );


    EEClient_SendCommandToServer( &command );
    return EEClient_SUCESS;

}


/**
  *  @brief  This function is used to request the GPS Time from the chip.
  *  @see EEClient_getGpsTime_callback for confirming the request
  *  @return                   returns 0 on success
  */
int EEClient_GetGpsTime()
{
    t_agps_Command command;
    t_agps_Packet packet;

    packet.length = 0;
    packet.pData = NULL;
    command.type = SOCKET_AGPS_EE_MSL_SET_GPSTIME_REQ;
    command.data = (void *) ( &packet );
    EEClient_SendCommandToServer( &command );

    return EEClient_SUCESS;

}


/**
  *  @brief  This function is used to indicate the Capablities EEClient is registered for.
  *  @return                   returns 0 on success
  */


int EEClient_SetCapabilities(e_EEClient_SetCapabilitiesInd setCapabilities)
{
    t_agps_Command command;
    t_agps_Packet packet;

    DEBUG_LOG_PRINT_LEV2(("EEClient_SetCapabilities setCapabilities=%d",setCapabilities));

    packet.length = sizeof(e_EEClient_SetCapabilitiesInd);
    DEBUG_LOG_PRINT_LEV2(("EEClient_SetCapabilities packet.length=%d",packet.length));
    packet.pData = (void *)(&setCapabilities);

    command.type = SOCKET_AGPS_REGISTER_EE_CLIENT_CAPABILITIES;
    command.data = (void *) ( &packet );
    EEClient_SendCommandToServer( &command );

    return EEClient_SUCESS;

}





/**
*  @brief  Get Reference Location Indication.
*/
/*XYBRID Integration :194997*/
void EEClient_getRefLocation(void)
{
    t_EEClient_RefLocationData GetRefLocationData;
    struct timeval current;

    gettimeofday( &current , NULL );
    DEBUG_LOG_PRINT_LEV2(("EEClient_getRefLocation current.tv_sec =%d,Requestedtime=%d",current.tv_sec,Requestedtime));
    if((current.tv_sec - Requestedtime) > CUT_OFF_TIME)
    {
        
        DEBUG_LOG_PRINT_LEV2(("getrefLocationdata_cb Called"));
        Requestedtime = current.tv_sec;
        eeClientCallbacks->getrefLocationdata_cb(&GetRefLocationData);
    }
    else
    {
         DEBUG_LOG_PRINT_LEV2(("Request already Done.Still waiting for response"));
    }
    

}


void EEClient_getRefTime(void)
{
    DEBUG_LOG_PRINT_LEV2(("EEClient_getRefTime"));
    eeClientCallbacks->getReferenceTime_cb();
}
void EEClient_DeleteSeed(void)
{
    DEBUG_LOG_PRINT_LEV2(("EEClient_DeleteSeed"));
    eeClientCallbacks->deleteSeed_cb();

}



/**
*  @brief  Get Ephemeris Request. 
*/

void EEClient_getEphemeris(t_EEClient_EphemerisData *GetEphemeris)
{

    DEBUG_LOG_PRINT_LEV2(("EEClient_getEphemeris v_PrnBitMask = %u,v_ConstelType=%d,v_EERefGPSTime=%d",GetEphemeris->v_PrnBitMask,GetEphemeris->v_ConstelType,GetEphemeris->v_EERefGPSTime));

    eeClientCallbacks->getephemerisdata_cb(GetEphemeris);

}


/*XYBRID Integration :194997*/



/**
*  @brief  Get Broadcast Ephemeris.
*/
void EEClient_getBroadcastEphemeris(e_EEClient_Status Status)
{
    DEBUG_LOG_PRINT_LEV2(("getBroadcastephemerisdata_cb Called"));

    eeClientCallbacks->getBroadcastephemerisdata_cb(Status);

}


/**
*  @brief Set Broadcast Ephemeris Indication.
*/
void EEClient_setBroadcastEphemeris(t_EEClient_NavDataBCE *pBCEData)
{
    DEBUG_LOG_PRINT_LEV2(("setBroadcastephemerisdata_cb Called"));

    eeClientCallbacks->setBroadcastephemerisdata_cb(pBCEData);

}



/**
*  @brief Set GPS Time.
*/
void EEClient_setGpsTime(t_EEClient_GpsTime *SetGpsTime)
{
    DEBUG_LOG_PRINT_LEV2(("setgpstime_cb Called"));

    eeClientCallbacks->setgpstime_cb(SetGpsTime);

}



/**
*  @brief GPS Firmware and Hardware version.
*/
void EEClient_setChipVer(t_EEClient_ChipVer *SetChipVer)
{
    DEBUG_LOG_PRINT_LEV2(("setChipVer_cb Callback Called"));


    /*Note, Caller should not free the memory*/
    eeClientCallbacks->setChipVer_cb(SetChipVer);

}

/**
  *  @brief  This function is used to request the Chip Firmware and Hardware version
  *  @see EEClient_setChipVer_callback for confirming the request
  *  @return                   returns 0 on success
  */
int EEClient_GetChipVer()
{

    t_EEClient_ChipVer softwareVersion;

    
    DEBUG_LOG_PRINT_LEV2(("EEClient_GetChipVer Called"));
    strcpy((char*)softwareVersion.v_Version,SOFTWARE_VER);

    EEClient_setChipVer(&softwareVersion);
    
    DEBUG_LOG_PRINT_LEV2(("EEClient_GetChipVer Exit"));
    return EEClient_SUCESS;

}





