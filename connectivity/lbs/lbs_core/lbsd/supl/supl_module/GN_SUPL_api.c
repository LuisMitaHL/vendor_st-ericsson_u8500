
//****************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
//
// Filename  GN_SUPL_api.c
//
// $Header: X:/MKS Projects/prototype/prototype/supl_module/rcs/GN_SUPL_api.c 1.70 2009/01/14 14:27:49Z grahama Rel $
// $Locker: $
//****************************************************************************

#ifdef __cplusplus
   extern "C" {
#endif

//****************************************************************************
// File level documentation
/// \file
/// \brief
///      GloNav SUPL Internally Implemented Interface API Functions
//
//-----------------------------------------------------------------------------
/// \ingroup    GloNav_SUPL
//
/// \defgroup   GN_SUPL_api_I  GloNav SUPL Library - Internally Implemented SUPL API.
//
/// \brief
///      GloNav SUPL Library - SUPL API definitions for functions implemented internally.
//
/// \details
///      GloNav SUPL Library - GPS SUPL API definitions for the enumerated
///      data types structures and functions provided in the library and can be
///      called by the host software (ie inward called).
/// \addtogroup GN_SUPL_api_I
//
//-----------------------------------------------------------------------------
/// \ingroup      GloNav_SUPL
//
/// \defgroup     GN_SUPL_api_H  GloNav SUPL Library - Host Implemented SUPL API.
//
/// \brief
///      GloNav SUPL Library - SUPL API definitions for functions implemented by the host.
//
/// \details
///      GloNav SUPL Library - SUPL API definitions for the enumerated data
///      types structures and functions called by the library and must be
///      implemented by the host software to suit the target platform OS and
///      hardware configuration (ie outward called).
///      All of these functions must be implemented in order to link a final
///      solution, even if only with a stub "{ return( 0 ); }" so say that the
///      particular action requested by the library is not supported.
/// \note
///      The functions in this group must be implemented in the
///      supporting code and linked with the library for the GPS to function
///      correctly.
/// \addtogroup GN_SUPL_api_H
//
//*****************************************************************************

#include <stdio.h>
#include <string.h>

#include "os_assert.h"

#include "gps_ptypes.h"
#include "GN_Status.h"
#include "GN_SUPL_api.h"
#include "GN_RRLP_api.h"

#include "supl_interface.h"
#include "supl_config.h"
#include "supl_helper.h"
#include "GAD_Conversions.h"
#include "supl_log.h"
#include "supl_hmac.h"
#include "time.h"


#define SUPL_API_LOGGING

//*****************************************************************************
/// \addtogroup GN_SUPL_api_I
/// \{


//*****************************************************************************
/// \brief
///      Initialises the SUPL_Handler.
/// \details
///      Sets up queues, instance data and memory allocations required.
///      <p> Called once at startup by the host to initialise the SUPL handler.
/// \returns
///      Nothing.
void GN_SUPL_Handler_Init( void )
{
   GN_SUPL_Log( "+GN_SUPL_Handler_Init:  %s  %s.", __DATE__, __TIME__ );
   GN_RRLP_Handler_Init();
   SUPL_SubSystem_Init();
   SUPL_Segmented_PDU_Buffer_Init();
   GN_SUPL_Log( "-GN_SUPL_Handler_Init:" );
}
//*****************************************************************************
/// \brief
///      SUPL Delete_LocationID_Data.
/// \details
///      SUPL Delete_LocationID_Data.
/// \returns
///      Flag to indicate success or failure of the occurence of event.
/// \retval #TRUE Flag indicating Area Event has occured
/// \retval #FALSE Flag indicatingArea Event has not occured and reporting is not required.

BL GN_SUPL_Delete_LocationID_Data_Ind_In( void )
{

   GN_SUPL_Log( "+GN_SUPL_Delete_LocationID_Data_Ind_In:" );
   
   SUPL_Send_Delete_Location_id_Info(NULL, NULL );

   GN_SUPL_Log( "-GN_SUPL_Delete_LocationID_Data_Ind_In:" );

   return TRUE;
}


//*****************************************************************************
/// \brief
///      DeInitialises the SUPL_Handler.
/// \details
///      Cleans up queues, instance data and memory allocations used.
///      <p> Called once at shutdown by the host to deinitialise the SUPL handler.
/// \note
///      #GN_SUPL_Handler_DeInit() will not attempt to close down any
///      connections to prevent the risk of deadlock.
/// \returns
///      Nothing.
void GN_SUPL_Handler_DeInit( void )
{
   GN_SUPL_Log( "+GN_SUPL_Handler_DeInit:" );
   GN_RRLP_Handler_DeInit();
   SUPL_SubSystem_DeInit();
   SUPL_Segmented_PDU_Buffer_Init();
   GN_SUPL_Log( "-GN_SUPL_Handler_DeInit:" );
}


//*****************************************************************************
/// \brief
///      SUPL_Handler process block.
/// \details
///      Called in the main processing loop to perform the SUPL related
///      processing functions.
///      <p> This module does the main SUPL processing is exercised to
///      enable the SUPL Handler to process SUPL related events.
/// \returns
///      Nothing.
void GN_SUPL_Handler( void )
{
   SUPL_Message_Handler();
   GN_RRLP_Handler();
}

//*****************************************************************************
/// \brief
///      Mobile Info indication.
/// \details
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval #TRUE Flag indicating success, Status will be set to True.
/// \retval #FALSE Flag indicating failure , Status will be set to False.

BL GN_SUPL_Mobile_Info_Ind_In(
     s_LocationId*  p_SUPL_LocationId   ///< [in] Location ID required for the SUPL exchange.
)
{
   BL Status = TRUE;

   GN_SUPL_Log( "+GN_SUPL_Mobile_Info_Ind_In:" );
   GN_SUPL_Log( "GN_SUPL_api_call:    SUPL<=Host [label=\"GN_SUPL_Mobile_Info_Ind_In\"];" );

   if(p_SUPL_LocationId != NULL)
   {
      SUPL_Send_Mobile_Info_Received(NULL, NULL, p_SUPL_LocationId);
   }

   GN_SUPL_Log( "-GN_SUPL_Mobile_Info_Ind_In:" );
   return Status;
}


//*****************************************************************************
/// \brief
///      SUPL incoming connection indication.
/// \details
///      Confirms a connection for SUPL to an external network entity.
///      <p> This function is called in response to a #GN_SUPL_Connect_Req_Out()
///      the #GN_SUPL_Connect_Ind_In()::Handle must be set to the
///      value provided in #GN_SUPL_Connect_Req_Out()::Handle.
///      <H3>p_Status values passed in:</H3>
///      <ul>
///         <li><var>#GN_SUCCESS \copydoc GN_SUCCESS</var>\n
///            p_Status should be set to this value when the #GN_SUPL_Connect_Req_Out()
///            was successful.</li>
///         <li><var>#GN_ERR_CONN_REJECTED \copydoc GN_ERR_CONN_REJECTED</var>\n
///            p_Status should be set to this value when the connection
///            associated with the Handle was rejected.</li>
///         <li><var>#GN_ERR_CONN_TIMEOUT \copydoc GN_ERR_CONN_TIMEOUT</var>\n
///            p_Status should be set to this value when the server could not be found.</li>
///         <li><var>#GN_ERR_CONN_REJECTED \copydoc GN_ERR_CONN_REJECTED</var>\n
///            p_Status should be set to this value when the connection to the server was refused.
///               Example : when the TLS Handshake failed.</li>
///      </ul>
///      <H3>p_Status values returned:</H3>
///      <ul>
///         <li><var>#GN_SUCCESS \copydoc GN_SUCCESS</var>\n
///            p_Status should be set to this value when the return value
///            indicated success.</li>
///         <li><var>#GN_ERR_HANDLE_INVALID \copydoc GN_ERR_HANDLE_INVALID</var>\n
///            p_Status should be set to this value when the Handle
///            passed was not recognised by the library software.</li>
///         <li><var>#GN_ERR_NO_RESOURCE \copydoc GN_ERR_NO_RESOURCE</var>\n
///            p_Status should be set to this value when there are not
///            enough resources on the host to perform the requested function.</li>
///      </ul>
/// \attention
///      The caller of #GN_SUPL_Connect_Ind_In() must manage (e.g. alloc and free)
///      the memory referenced by p_Status and p_LocationId.
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval #TRUE Flag indicating success, *p_Status will be set to GN_SUCCESS.
/// \retval #FALSE Flag indicating failure and that the status should be checked.
BL GN_SUPL_Connect_Ind_In(
   void*          Handle,        ///< [in] Opaque Handle used to coordinate requests.
   e_GN_Status*   p_Status,      ///< [in/out] Status of disconnect and to be checked when return flag indicates failure.
   s_LocationId*  p_LocationId   ///< [in] Location ID required for the SUPL exchange.
)
{
   s_SUPL_Instance *p_SUPL_Instance;
   //e_GN_Status GN_Status = GN_SUCCESS;
   BL Status = TRUE;
   U1 i;

   GN_SUPL_Log( "+GN_SUPL_Connect_Ind_In:" );
   GN_SUPL_Log( "GN_SUPL_api_call:    SUPL<=Host [label=\"GN_SUPL_Connect_Ind_In: Handle=%p\"];", Handle );

   if ( *p_Status == GN_SUCCESS )
   {
      *p_Status = GN_SUCCESS; // Set the GN_Status value to success unless an error occurs.

      p_SUPL_Instance = SUPL_Instance_From_Handle(Handle);
      if ( p_SUPL_Instance != NULL )
      {
         GN_SUPL_Log_ServerConnected( Handle );

         supl_log_LocationId( "GN_SUPL_Connect_Ind_In:", p_LocationId );
         if(p_LocationId->Type == CIT_AccessPoint_WLAN)
         {
             s_GN_SUPL_V2_WLANAPInfo *p_WLANAPInfo;
             /*WLAN is the Wireless Network and Access point Information present is that of WLAN's*/
             /*so copy the WLAN Access Point Info on to the SUPL Instance*/
             p_SUPL_Instance->p_SUPL->LocationId.Type   = p_LocationId->Type;
             p_SUPL_Instance->p_SUPL->LocationId.Status = p_LocationId->Status;
             p_WLANAPInfo = &(p_SUPL_Instance->p_SUPL->LocationId.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo);

             if(((sizeof(p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_MS_Addr))+
                (sizeof(p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_LS_Addr)))<= 6)
             {
                 p_WLANAPInfo->v_MS_Addr = p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_MS_Addr;
                 p_WLANAPInfo->v_LS_Addr = p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_LS_Addr;

 
                 if((p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_TransmitPower >= -127)&&(p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_TransmitPower <= 128))
                 {
                    p_WLANAPInfo->v_TransmitPower = p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_TransmitPower;
                 }
                 else
                 {
                    /*Since apart from AP MAC address all other parameters are optional need to know what can they be filled with if they are found to be out of range*/
                    p_WLANAPInfo->v_TransmitPower = 0;
                 }
                 if((p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_AntennaGain >= -127)&&(p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_AntennaGain <= 128))
                 {
                    p_WLANAPInfo->v_AntennaGain = p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_AntennaGain;
                 }
                 else
                 {
                    /*Since apart from AP MAC address all other parameters are optional need to know what can they be filled with if they are found to be out of range*/
                    p_WLANAPInfo->v_AntennaGain = 0;
                 }

                 if((p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SNR >= -127)&&(p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SNR <= 128))
                 {
                    p_WLANAPInfo->v_SNR = p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SNR;
                 }
                 else
                 {
                    /*Since apart from AP MAC address all other parameters are optional need to know what can they be filled with if they are found to be out of range*/
                    p_WLANAPInfo->v_SNR = 0;
                 }

                 if((p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SignalStrength >= -127)&&(p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SignalStrength <= 128))
                 {
                    p_WLANAPInfo->v_SignalStrength = p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SignalStrength;
                 }
                 else
                 {
                    /*Since apart from AP MAC address all other parameters are optional need to know what can they be filled with if they are found to be out of range*/
                    p_WLANAPInfo->v_SignalStrength = 0;
                 }

                 if((p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETTransmitPower >= -127)&&(p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETTransmitPower <= 128))
                 {
                    p_WLANAPInfo->v_SETTransmitPower = p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETTransmitPower;
                 }
                 else
                 {
                    /*Since apart from AP MAC address all other parameters are optional need to know what can they be filled with if they are found to be out of range*/
                    p_WLANAPInfo->v_SETTransmitPower = 0;
                 }

                 if((p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETAntennaGain >= -127)&&(p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETAntennaGain <= 128))
                 {
                    p_WLANAPInfo->v_SETAntennaGain = p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETAntennaGain;
                 }
                 else
                 {
                    /*Since apart from AP MAC address all other parameters are optional need to know what can they be filled with if they are found to be out of range*/
                    p_WLANAPInfo->v_SETAntennaGain = 0;
                 }
                 if((p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETSNR >= -127)&&(p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETSNR <= 128))
                 {
                    p_WLANAPInfo->v_SETSNR = p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETSNR;
                 }
                 else
                 {
                    /*Since apart from AP MAC address all other parameters are optional need to know what can they be filled with if they are found to be out of range*/
                    p_WLANAPInfo->v_SETSNR = 0;
                 }

                 if((p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETSignalStrength >= -127)&&(p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETSignalStrength <= 128))
                 {
                    p_WLANAPInfo->v_SETSignalStrength = p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETSignalStrength;
                 }
                 else
                 {
                    /*Since apart from AP MAC address all other parameters are optional need to know what can they be filled with if they are found to be out of range*/
                    p_WLANAPInfo->v_SETSignalStrength = 0;
                 }

                 if( p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_Channel <= 256 )
                 {
                    p_WLANAPInfo->v_Channel = p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_Channel;
                 }
                 else
                 {
                    /*Since apart from AP MAC address all other parameters are optional need to know what can they be filled with if they are found to be out of range*/
                    p_WLANAPInfo->v_Channel = 0;
                 }

                 if( p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_Accuracy <= 255 )
                 {
                    p_WLANAPInfo->v_Accuracy = p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_Accuracy;
                 }
                 else
                 {
                    /*Since apart from AP MAC address all other parameters are optional need to know what can they be filled with if they are found to be out of range*/
                    p_WLANAPInfo->v_Accuracy = 0;
                 }

                 if( p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_RTDValue <= 1677216 )
                 {
                    p_WLANAPInfo->v_RTDValue = p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_RTDValue;
                 }
                 else
                 {
                    /*Since apart from AP MAC address all other parameters are optional need to know what can they be filled with if they are found to be out of range*/
                    p_WLANAPInfo->v_RTDValue = 0;
                 }
                 if( p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_DeviceType <= 2 )
                 {
                    p_WLANAPInfo->v_DeviceType = p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_DeviceType; 
                 }
                 else
                 {
                 /*Since apart from AP MAC address all other parameters are optional need to know what can they be filled with if they are found to be out of range*/
                    p_WLANAPInfo->v_DeviceType = 0;
                 }

                 if( p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_RTDUnits <= 4 )
                 {
                     p_WLANAPInfo->v_RTDUnits = p_LocationId->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_RTDUnits;
                 }
                 else
                 {
                    /*Since apart from AP MAC address all other parameters are optional need to know what can they be filled with if they are found to be out of range*/
                     p_WLANAPInfo->v_RTDUnits = 0;
                 }
                 GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: WlanAccessPointInfo.v_MS_Addr %d", 
                               p_SUPL_Instance->p_SUPL->LocationId.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_MS_Addr);
                 GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: WlanAccessPointInfo.v_LS_Addr %d", 
                               p_SUPL_Instance->p_SUPL->LocationId.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_LS_Addr);
                 GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: WlanAccessPointInfo.v_Accuracy %d", 
                               p_SUPL_Instance->p_SUPL->LocationId.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_Accuracy);
                 GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: WlanAccessPointInfo.v_AntennaGain %d", 
                               p_SUPL_Instance->p_SUPL->LocationId.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_AntennaGain);
                 GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: WlanAccessPointInfo.v_Channel %d", 
                               p_SUPL_Instance->p_SUPL->LocationId.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_Channel);
                 GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: WlanAccessPointInfo.v_DeviceType %d", 
                               p_SUPL_Instance->p_SUPL->LocationId.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_DeviceType);
                 GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: WlanAccessPointInfo.v_RTDUnits %d", 
                               p_SUPL_Instance->p_SUPL->LocationId.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_RTDUnits);
                 GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: WlanAccessPointInfo.v_RTDValue %d", 
                               p_SUPL_Instance->p_SUPL->LocationId.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_RTDValue);
                 GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: WlanAccessPointInfo.v_SETAntennaGain %d", 
                               p_SUPL_Instance->p_SUPL->LocationId.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETAntennaGain);
                 GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: WlanAccessPointInfo.v_SETSignalStrength %d", 
                               p_SUPL_Instance->p_SUPL->LocationId.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETSignalStrength);
                 GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: WlanAccessPointInfo.v_SETSNR %d", 
                               p_SUPL_Instance->p_SUPL->LocationId.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETSNR);
                 GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: WlanAccessPointInfo.v_SETTransmitPower %d", 
                               p_SUPL_Instance->p_SUPL->LocationId.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETTransmitPower);
                 GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: WlanAccessPointInfo.v_SignalStrength %d", 
                               p_SUPL_Instance->p_SUPL->LocationId.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SignalStrength);
                 GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: WlanAccessPointInfo.v_SNR %d", 
                               p_SUPL_Instance->p_SUPL->LocationId.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SNR);
                 GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: WlanAccessPointInfo.v_TransmitPower %d", 
                               p_SUPL_Instance->p_SUPL->LocationId.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_TransmitPower);
             }
             else
             {
                 GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: WLAN AP MAC Address Invalid" );
                Status = FALSE;
             }
         }
         else if(p_LocationId->Type == CIT_AccessPoint_WIMAX)
         {
             /*This Wimax Acces Point Info is present so copy on to the SUPL_Instance*/
         }
         else
         {
             p_SUPL_Instance->p_SUPL->LocationId.Status = p_LocationId->Status;
             switch ( p_LocationId->Type )
             {
             case CIT_gsmCell:       ///< Cell information is from a GSM network.
                p_SUPL_Instance->p_SUPL->LocationId.Type = p_LocationId->Type;
                {
                   s_gsmCellInfo  *p_gsmCellInfo = &p_SUPL_Instance->p_SUPL->LocationId.of_type.gsmCellInfo;
                   // INTEGER(0..999), -- Mobile Country Code
                   if ( /* p_LocationId->of_type.gsmCellInfo.refMCC >= 0 && */ p_LocationId->of_type.gsmCellInfo.refMCC <= 999 )
                   {
                      p_gsmCellInfo->refMCC  = p_LocationId->of_type.gsmCellInfo.refMCC;
                   }
                   else
                   {
                      GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Cell Information == gsmCell: MCC %d out of range 0..999",
                                   p_LocationId->of_type.gsmCellInfo.refMCC);
                      ///\todo Log at the moment and decide if we need to handle differently later.
                      //Status = FALSE;
                      //*p_Status = GN_ERR_PARAMETER_INVALID;
                   }
                   // INTEGER(0..999), -- Mobile Network Code
                   if ( /* p_LocationId->of_type.gsmCellInfo.refMNC >= 0 && */ p_LocationId->of_type.gsmCellInfo.refMNC <= 999 )
                   {
                      p_gsmCellInfo->refMNC  = p_LocationId->of_type.gsmCellInfo.refMNC;
                   }
                   else
                   {
                      GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Cell Information == gsmCell: MNC %d out of range 0..999",
                                   p_LocationId->of_type.gsmCellInfo.refMNC );
                      ///\todo Log at the moment and decide if we need to handle differently later.
                      //Status = FALSE;
                      //*p_Status = GN_ERR_PARAMETER_INVALID;
                   }
                   // INTEGER(0..65535), -- Location area code
                   //if (p_LocationId->of_type.gsmCellInfo.refLAC >= 0 && p_LocationId->of_type.gsmCellInfo.refLAC <= 65535)
                   //{
                      p_gsmCellInfo->refLAC  = p_LocationId->of_type.gsmCellInfo.refLAC;
                   //}
                   //else
                   //{
                   //   GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Cell Information == gsmCell: LAC %d out of range 0..65535",
                   //                p_LocationId->of_type.gsmCellInfo.refLAC);
                   //   ///\todo Log at the moment and decide if we need to handle differently later.
                   //   //Status = FALSE;
                   //   //*p_Status = GN_ERR_PARAMETER_INVALID;
                   //}
                   // INTEGER(0..65535), -- Cell identity
                   //if (p_LocationId->of_type.gsmCellInfo.refLAC >= 0 && p_LocationId->of_type.gsmCellInfo.refLAC <= 65535)
                   //{
                      p_gsmCellInfo->refCI   = p_LocationId->of_type.gsmCellInfo.refCI;
                   //}
                   //else
                   //{
                   //   GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Cell Information == gsmCell: CI %d out of range 0..65535",
                   //                p_LocationId->of_type.gsmCellInfo.refCI );
                   //   ///\todo Log at the moment and decide if we need to handle differently later.
                   //   //Status = FALSE;
                   //   //*p_Status = GN_ERR_PARAMETER_INVALID;
                   //}
                   // INTEGER(0..255) OPTIONAL, --Timing Advance. (-1 = Not present).
                   if ( p_LocationId->of_type.gsmCellInfo.tA >= -1 && p_LocationId->of_type.gsmCellInfo.tA <= 250 )
                   {
                      p_gsmCellInfo->tA      = p_LocationId->of_type.gsmCellInfo.tA;
                   }
                   else
                   {
                      GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Cell Information == gsmCell: tA %d out of range 0..250",
                                   p_LocationId->of_type.gsmCellInfo.tA );
                      ///\todo Log at the moment and decide if we need to handle differently later.
                      //Status = FALSE;
                      //*p_Status = GN_ERR_PARAMETER_INVALID;
                   }
                   p_gsmCellInfo->NMRCount = p_LocationId->of_type.gsmCellInfo.NMRCount;
                   if ( p_gsmCellInfo->NMRCount != 0 )
                   {
                      s_NMRElement *p_NMRDestination, *p_NMRSource;

                      p_gsmCellInfo->p_NMRList = GN_Calloc( p_gsmCellInfo->NMRCount, sizeof( s_NMRElement ) );
                      p_NMRDestination = p_gsmCellInfo->p_NMRList;
                      p_NMRSource = p_LocationId->of_type.gsmCellInfo.p_NMRList;
                      for ( i = 0 ; i < p_gsmCellInfo->NMRCount ; i++ )
                      {
                         // INTEGER(0..1023),
                         if ( /* p_NMRSource->aRFCN >= 0 && */ p_NMRSource->aRFCN <= 1023 )
                         {
                            p_NMRDestination->aRFCN = p_NMRSource->aRFCN;
                         }
                         else
                         {
                            GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Cell Information == gsmCell: NMR %u of %u : aRFCN %d out of range 0..1023",
                                         i + 1,
                                         p_gsmCellInfo->NMRCount,
                                         p_NMRSource->aRFCN );
                            ///\todo Log at the moment and decide if we need to handle differently later.
                            //Status = FALSE;
                            //*p_Status = GN_ERR_PARAMETER_INVALID;
                         }
                         // INTEGER(0..63),
                         if ( /* p_NMRSource->bSIC >= 0 && */ p_NMRSource->bSIC <= 63 )
                         {
                            p_NMRDestination->bSIC = p_NMRSource->bSIC;
                         }
                         else
                         {
                            GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Cell Information == gsmCell: NMR %u of %u : bSIC %d out of range 0..63",
                                         i + 1,
                                         p_gsmCellInfo->NMRCount,
                                         p_NMRSource->bSIC );
                            ///\todo Log at the moment and decide if we need to handle differently later.
                            //Status = FALSE;
                            //*p_Status = GN_ERR_PARAMETER_INVALID;
                         }
                         // INTEGER(0..63),
                         if ( /* p_NMRSource->rxLev >= 0 && */ p_NMRSource->rxLev <= 63 )
                         {
                            p_NMRDestination->rxLev = p_NMRSource->rxLev;
                         }
                         else
                         {
                            GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Cell Information == gsmCell: NMR %u of %u : rxLev %d out of range 0..63",
                                         i + 1,
                                         p_gsmCellInfo->NMRCount,
                                         p_NMRSource->rxLev );
                            ///\todo Log at the moment and decide if we need to handle differently later.
                            //Status = FALSE;
                            //*p_Status = GN_ERR_PARAMETER_INVALID;
                         }
                         p_NMRDestination++;
                         p_NMRSource++;
                      }
                   }
                }
                break;
             case CIT_cdmaCell:         ///< Cell information is from a CDMA network.
                p_SUPL_Instance->p_SUPL->LocationId.Type = p_LocationId->Type;
                {
                   s_cdmaCellInfo  *p_cdmaCellInfo = &p_SUPL_Instance->p_SUPL->LocationId.of_type.cdmaCellInfo;

                   // INTEGER(0..65535), -- Network Id
                   //if ( p_LocationId->of_type.cdmaCellInfo.refNID >= 0 && p_LocationId->of_type.cdmaCellInfo.refNID <= 65535 )
                   //{
                      p_cdmaCellInfo->refNID        = p_LocationId->of_type.cdmaCellInfo.refNID;          // INTEGER(0..65535), -- Network Id
                   //}
                   //else
                   //{
                   //   GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Cell Information == cdmaCell: refNID %u out of range 0..65535",
                   //                p_LocationId->of_type.cdmaCellInfo.refNID );
                   //   ///\todo Log at the moment and decide if we need to handle differently later.
                   //   //Status = FALSE;
                   //   //*p_Status = GN_ERR_PARAMETER_INVALID;
                   //}
                   // INTEGER(0..32767), -- System Id
                   if ( /* p_LocationId->of_type.cdmaCellInfo.refSID >= 0 && */ p_LocationId->of_type.cdmaCellInfo.refSID <= 32767 )
                   {
                      p_cdmaCellInfo->refSID        = p_LocationId->of_type.cdmaCellInfo.refSID;          // INTEGER(0..32767), -- System Id
                   }
                   else
                   {
                      GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Cell Information == cdmaCell: refSID %u out of range 0..32767",
                                   p_LocationId->of_type.cdmaCellInfo.refSID );
                      ///\todo Log at the moment and decide if we need to handle differently later.
                      //Status = FALSE;
                      //*p_Status = GN_ERR_PARAMETER_INVALID;
                   }
                   // INTEGER(0..65535), -- Base Station Id
                   //if ( p_LocationId->of_type.cdmaCellInfo.refBASEID >= 0 && p_LocationId->of_type.cdmaCellInfo.refBASEID <= 65535 )
                   //{
                      p_cdmaCellInfo->refBASEID     = p_LocationId->of_type.cdmaCellInfo.refBASEID;       // INTEGER(0..65535), -- Base Station Id
                   //}
                   //else
                   //{
                   //   GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Cell Information == cdmaCell: refSID %u out of range 0..65535",
                   //                p_LocationId->of_type.cdmaCellInfo.refBASEID );
                   //   ///\todo Log at the moment and decide if we need to handle differently later.
                   //   //Status = FALSE;
                   //   //*p_Status = GN_ERR_PARAMETER_INVALID;
                   //}
                   // INTEGER(0..4194303), -- Base Station Latitude
                   if ( /* p_LocationId->of_type.cdmaCellInfo.refBASELAT >= 0 && */ p_LocationId->of_type.cdmaCellInfo.refBASELAT <= 4194303 )
                   {
                      p_cdmaCellInfo->refBASELAT    = p_LocationId->of_type.cdmaCellInfo.refBASELAT;      // INTEGER(0..4194303), -- Base Station Latitude
                   }
                   else
                   {
                      GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Cell Information == cdmaCell: refBASELAT %u out of range 0..4194303",
                                   p_LocationId->of_type.cdmaCellInfo.refBASELAT );
                      ///\todo Log at the moment and decide if we need to handle differently later.
                      //Status = FALSE;
                      //*p_Status = GN_ERR_PARAMETER_INVALID;
                   }
                   // INTEGER(0..8388607), -- Base Station Longitude
                   if ( /* p_LocationId->of_type.cdmaCellInfo.reBASELONG >= 0 && */ p_LocationId->of_type.cdmaCellInfo.reBASELONG <= 8388607 )
                   {
                      p_cdmaCellInfo->reBASELONG    = p_LocationId->of_type.cdmaCellInfo.reBASELONG;      // INTEGER(0..8388607), -- Base Station Longitude
                   }
                   else
                   {
                      GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Cell Information == cdmaCell: reBASELONG %u out of range 0..8388607",
                                   p_LocationId->of_type.cdmaCellInfo.reBASELONG );
                      ///\todo Log at the moment and decide if we need to handle differently later.
                      //Status = FALSE;
                      //*p_Status = GN_ERR_PARAMETER_INVALID;
                   }
                   // INTEGER(0..511), -- Base Station PN Code
                   if ( /* p_LocationId->of_type.cdmaCellInfo.refREFPN >= 0 && */ p_LocationId->of_type.cdmaCellInfo.refREFPN <= 511 )
                   {
                      p_cdmaCellInfo->refREFPN      = p_LocationId->of_type.cdmaCellInfo.refREFPN;        // INTEGER(0..511), -- Base Station PN Code
                   }
                   else
                   {
                      GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Cell Information == cdmaCell: refREFPN %u out of range 0..511",
                                   p_LocationId->of_type.cdmaCellInfo.refREFPN );
                      ///\todo Log at the moment and decide if we need to handle differently later.
                      //Status = FALSE;
                      //*p_Status = GN_ERR_PARAMETER_INVALID;
                   }
                   // INTEGER(0..65535), -- GPS Week Number
                   //if ( p_LocationId->of_type.cdmaCellInfo.refWeekNumber >= 0 && p_LocationId->of_type.cdmaCellInfo.refWeekNumber <= 65535 )
                   //{
                      p_cdmaCellInfo->refWeekNumber = p_LocationId->of_type.cdmaCellInfo.refWeekNumber;   // INTEGER(0..65535), -- GPS Week Number
                   //}
                   //else
                   //{
                   //   GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Cell Information == cdmaCell: refWeekNumber %u out of range 0..65535",
                   //                p_LocationId->of_type.cdmaCellInfo.refWeekNumber );
                   //   ///\todo Log at the moment and decide if we need to handle differently later.
                   //   //Status = FALSE;
                   //   //*p_Status = GN_ERR_PARAMETER_INVALID;
                   //}
                   // INTEGER(0..4194303),-- GPS Seconds
                   if ( /* p_LocationId->of_type.cdmaCellInfo.refSeconds >= 0 && */ p_LocationId->of_type.cdmaCellInfo.refSeconds <= 4194303 )
                   {
                      p_cdmaCellInfo->refSeconds = p_LocationId->of_type.cdmaCellInfo.refSeconds;   // INTEGER(0..65535), -- GPS Week Number
                   }
                   else
                   {
                      GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Cell Information == cdmaCell: refSeconds %u out of range 0..4194303",
                                   p_LocationId->of_type.cdmaCellInfo.refSeconds );
                      ///\todo Log at the moment and decide if we need to handle differently later.
                      //Status = FALSE;
                      //*p_Status = GN_ERR_PARAMETER_INVALID;
                   }
                }
                break;
             case CIT_wcdmaCell:        // Cell information is from a WCDMA network.
                p_SUPL_Instance->p_SUPL->LocationId.Type = p_LocationId->Type;
                Status = supl_Copy_wcdmaCellInfo( &p_SUPL_Instance->p_SUPL->LocationId.of_type.wcdmaCellInfo, &p_LocationId->of_type.wcdmaCellInfo );
                if ( ! Status )
                {
                   *p_Status = GN_ERR_PARAMETER_INVALID;
                }
                break;
             case CIT_AccessPoint_WLAN:
                break;
             case CIT_AccessPoint_UNKNOWN:
                break;
             case CIT_AccessPoint_WIMAX:
                break;
             default:
                break;
             } 
                 
         }
         
         switch ( p_SUPL_Instance->p_SUPL->SetId.type )
         {
         case GN_SETId_PR_msisdn:
            GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Set Id configured with msisdn." );
            break;
         case GN_SETId_PR_imsi:
            GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Set Id configured with imsi." );
            break;
         case GN_SETId_PR_mdn:
            GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Set Id configured with imsi." );
            break;
         case GN_SETId_PR_min:
            GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Set Id unsupported configuration min." );
            ///\todo Add GN_ERR_SET_NOT_CONFIGURED error.
            Status = FALSE;
            *p_Status = GN_ERR_PARAMETER_INVALID;
            break;
         case GN_SETId_PR_nai:
            GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Set Id unsupported configuration nai." );
            ///\todo Add GN_ERR_SET_NOT_CONFIGURED error.
            Status = FALSE;
            *p_Status = GN_ERR_PARAMETER_INVALID;
            break;
         case GN_SETId_PR_iPAddressV4:
            GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Set Id unsupported configuration ip Address (V4)." );
            ///\todo Add GN_ERR_SET_NOT_CONFIGURED error.
            Status = FALSE;
            *p_Status = GN_ERR_PARAMETER_INVALID;
            break;
         case GN_SETId_PR_iPAddressV6:
            GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Set Id unsupported configuration ip Address (V6)." );
            ///\todo Add GN_ERR_SET_NOT_CONFIGURED error.
            Status = FALSE;
            *p_Status = GN_ERR_PARAMETER_INVALID;
            break;
         default:
            /* Check if there is a new set id configured. If there is, use it */
            /* GN_SETId_PR_NOTHING:  handled in default case.*/
            {
               e_SetIdType SetIdType = supl_config_get_SUPL_SetIdType();

               GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: No Set Id configured. Checking for fresh Information." );

               if ( SetIdType != GN_SETId_PR_NOTHING )
               {
                  p_SUPL_Instance->p_SUPL->SetId.type = SetIdType;
                  switch ( SetIdType )
                  {
                  case GN_SETId_PR_msisdn:
                     GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Set Id configured with msisdn." );
                     memcpy(  p_SUPL_Instance->p_SUPL->SetId.u.msisdn,
                              supl_config_get_SUPL_msisdn(),
                              sizeof( p_SUPL_Instance->p_SUPL->SetId.u.msisdn ) );
                     break;
                  case GN_SETId_PR_mdn:
                     memcpy(  p_SUPL_Instance->p_SUPL->SetId.u.mdn,
                              supl_config_get_SUPL_mdn(),
                              sizeof( p_SUPL_Instance->p_SUPL->SetId.u.mdn ) );
                     break;
                  case GN_SETId_PR_min:
                     //memcpy(  p_SUPL_Instance->p_SUPL->SetId.u.min,
                     //         supl_config_get_SUPL_min(),
                     //         sizeof( p_SUPL_Instance->p_SUPL->SetId.u.min ) );
                     GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Set Id unsupported configuration min." );
                     ///\todo Add GN_ERR_SET_NOT_CONFIGURED error.
                     Status = FALSE;
                     *p_Status = GN_ERR_PARAMETER_INVALID;
                     break;
                  case GN_SETId_PR_imsi:
                     GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Set Id configured with imsi." );
                     memcpy(  p_SUPL_Instance->p_SUPL->SetId.u.imsi,
                              supl_config_get_SUPL_imsi(),
                              sizeof( p_SUPL_Instance->p_SUPL->SetId.u.imsi ) );
                     break;
                  case GN_SETId_PR_nai:
                     GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Set Id unsupported configuration nai." );
                     ///\todo Add GN_ERR_SET_NOT_CONFIGURED error.
                     Status = FALSE;
                     *p_Status = GN_ERR_PARAMETER_INVALID;
                     break;
                  case GN_SETId_PR_iPAddressV4:
                     GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Set Id unsupported configuration ip Address (V4)." );
                     ///\todo Add GN_ERR_SET_NOT_CONFIGURED error.
                     Status = FALSE;
                     *p_Status = GN_ERR_PARAMETER_INVALID;
                     break;
                  case GN_SETId_PR_iPAddressV6:
                     GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Set Id unsupported configuration ip Address (V6)." );
                     ///\todo Add GN_ERR_SET_NOT_CONFIGURED error.
                     Status = FALSE;
                     *p_Status = GN_ERR_PARAMETER_INVALID;
                     break;
                  default:
                  // case GN_SETId_PR_NOTHING:  Not handled.
                     break;
                  }
               }
               else
               {
                  GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Set Id not configured." );
                  ///\todo Add GN_ERR_SET_NOT_CONFIGURED error.
                  Status = FALSE;
                  *p_Status = GN_ERR_PARAMETER_INVALID;
               }
            }
            break;
         }
         if (Status)
         {
            supl_log_LocationId( "GN_SUPL_Connect_Ind_In:",  &p_SUPL_Instance->p_SUPL->LocationId );
            SUPL_Send_Comms_Open_Success(
               NULL,
               NULL,
               p_SUPL_Instance->Handle );
         }
         else
         {
            CH *ErrorText;

            ErrorText = GN_SUPL_StatusDesc_From_Status( *p_Status );

            GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Connection Failed: GN_Status %x %s", *p_Status, ErrorText );
            SUPL_Send_Comms_Open_Failure(
               NULL,
               NULL,
               p_SUPL_Instance->Handle );
            return Status;
         }
      }
      else
      {
         Status = FALSE;
         *p_Status = GN_ERR_HANDLE_INVALID;
      }
   }
   else
   {
      p_SUPL_Instance = SUPL_Instance_From_Handle( Handle );
      if ( p_SUPL_Instance != NULL )
      {
         CH *ErrorText;

         ErrorText = GN_SUPL_StatusDesc_From_Status( *p_Status );

         switch( *p_Status )
         {
            case GN_ERR_CONN_REJECTED:
                GN_SUPL_Log_ServerDisconnected( Handle );
                break;
            case GN_ERR_NO_RESOURCE:
                GN_SUPL_Log_NoNetworkResource( Handle );
                break;
            case GN_ERR_CONN_TIMEOUT:
                GN_SUPL_Log_ServerNotFound( Handle );
                break;
            /* Below one are just to avoid warnings*/
         case GN_SUCCESS:
         case GN_ERR_HANDLE_INVALID:
         case GN_ERR_POINTER_INVALID:
         case GN_ERR_PARAMETER_INVALID:
         case GN_ERR_CONN_SHUTDOWN:
         case GN_WRN_NOT_READY:
         case GN_WRN_NO_POSITION:
            GN_SUPL_Log( "Not Handling in these cases" );
            break;
         }

         GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Connection rejected by HOST: GN_Status %x %s", *p_Status, ErrorText );
         *p_Status = GN_SUCCESS;
         SUPL_Send_Comms_Open_Failure(
            NULL,
            NULL,
            p_SUPL_Instance->Handle );
         return Status;
      }
      else
      {
         Status = FALSE;
         *p_Status = GN_ERR_HANDLE_INVALID;
      }
   }
   GN_SUPL_Log( "-GN_SUPL_Connect_Ind_In:" );
   return Status;
}


//*****************************************************************************
/// \brief
///      SUPL incoming disconnection indication.
/// \details
///      Confirms disconnection for an existing SUPL connection.
///      <p> This function is called in response to a
///      #GN_SUPL_Disconnect_Req_Out() and the Handle is set to the Handle
///      from the #GN_SUPL_Disconnect_Req_Out() call or is called when a
///      connection spontaneously terminates when the Handle is set to the
///      Handle in the #GN_SUPL_Connect_Req_Out() request.
///      <H3>p_Status values passed in:</H3>
///      <ul>
///         <li><var>#GN_SUCCESS \copydoc GN_SUCCESS</var>\n
///            p_Status should be set to this value when the #GN_SUPL_Disconnect_Req_Out()
///            was successful.</li>
///         <li><var>#GN_ERR_CONN_SHUTDOWN \copydoc GN_ERR_CONN_SHUTDOWN</var>\n
///            p_Status should be set to this value when the connection
///            associated with the Handle was spontaneously shut down.</li>
///      </ul>
///      <H3>p_Status values returned:</H3>
///      <ul>
///         <li><var>#GN_SUCCESS \copydoc GN_SUCCESS</var>\n
///            p_Status should be set to this value when the return value
///            indicated success.</li>
///         <li><var>#GN_ERR_HANDLE_INVALID \copydoc GN_ERR_HANDLE_INVALID</var>\n
///            p_Status should be set to this value when the Handle
///            passed was not recognised by the library software.</li>
///         <li><var>#GN_ERR_NO_RESOURCE \copydoc GN_ERR_NO_RESOURCE</var>\n
///            p_Status should be set to this value when there are not
///            enough resources on the host to perform the requested function.</li>
///      </ul>
/// \attention
///      The caller of #GN_SUPL_Disconnect_Ind_In() must manage (e.g. alloc and free)
///      the memory referenced by p_Status.
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval #TRUE Flag indicating success, *p_Status will be set to GN_SUCCESS.
/// \retval #FALSE Flag indicating failure and that the status should be checked.
BL GN_SUPL_Disconnect_Ind_In(
   void*          Handle,     ///< [in] Opaque Handle used to coordinate requests.
   e_GN_Status*   p_Status    ///< [in/out] Status of disconnect and to be checked when return flag indicates failure.
)
{
   BL Return_Status = FALSE;
   CH* StatusDesc;

   GN_SUPL_Log( "+GN_SUPL_Disconnect_Ind_In:" );
   GN_SUPL_Log( "GN_SUPL_api_call:    SUPL<=Host [label=\"GN_SUPL_Disconnect_Ind_In: Handle=%p\"];", Handle );

   if ( p_Status == NULL )
   {
      GN_SUPL_Log( "GN_SUPL_api_call: Invalid status" );
   }
   else
   {
      switch ( *p_Status )
      {
      case GN_SUCCESS:           // GN Success Status.
         SUPL_Send_Comms_Close(
            NULL,
            NULL,
            Handle );
         *p_Status = GN_SUCCESS;
         Return_Status = TRUE;
         break;

      case GN_ERR_NO_RESOURCE:   // GN Error Status - No resources available to perform action.
      case GN_ERR_CONN_SHUTDOWN: // GN Error Status - Connection was shutdown spontaneously.
      case GN_ERR_CONN_REJECTED: // GN Error Status - Connection attempt was rejected.
      case GN_ERR_CONN_TIMEOUT:  // GN Error Status - Connection attempt timed out.
         StatusDesc = GN_SUPL_StatusDesc_From_Status( *p_Status );
         GN_SUPL_Log( "GN_SUPL_Disconnect_Ind_In: Status Returned: GN_Status %x %s", *p_Status, StatusDesc );
         SUPL_Send_Comms_Open_Failure(
            NULL,
            NULL,
            Handle );
         *p_Status = GN_SUCCESS;
         Return_Status = TRUE;
         break;

      default:
         break;
      }
   }

   GN_SUPL_Log( "-GN_SUPL_Disconnect_Ind_In:" );
   return Return_Status;
}


//*****************************************************************************
/// \brief
///      SUPL Push delivery in.
/// \details
///      Delivers a SUPL Push from an SMS or MMS.
///      <p> This function is called in response to a SUPL-INIT being received
///      via an SMS or MMS and is then delivered to the SUPL_Handler.
///      <p> A Handle will be created by the SUPL subsystem and placed
///      in the location pointed at by p_NewHandle.
///      <H3>p_Status values returned:</H3>
///      <ul>
///         <li><var>#GN_SUCCESS \copydoc GN_SUCCESS</var>\n
///            p_Status should be set to this value when the return value
///            indicated success.</li>
///         <li><var>#GN_ERR_NO_RESOURCE \copydoc GN_ERR_NO_RESOURCE</var>\n
///            p_Status should be set to this value when there are not
///            enough resources on the host to perform the requested function.</li>
///      </ul>
///      \anchor VER_Generation
///      <H3>VER Hash Generation</H3>
///      <p> For Proxy mode SUPL-POS-INIT must contain a verification field (VER) which
///      is an HMAC (Hash Message Authentication Code) which is generated as below:
///      <ul>
///         <li> VER = H(H-SLP XOR opad, H(H-SLP XOR ipad, SUPL INIT)) </li>
///         <li> H = Hash function (SHA-1). </li>
///         <li> H-SLP = FQDN of the SLP. </li>
///         <li> SUPL-INIT = SUPL-INIT received in the PUSH. </li>
///         <li> IPAD (Inner PAD) = Array of 0x36, length of H-SLP. </li>
///         <li> OPAD (Outer PAD) = Array of 0x5c, length or H-SLP. </li>
///      </ul>
/// \attention
///      The caller of #GN_SUPL_Push_Delivery_In() must manage (e.g. alloc and free)
///      the memory referenced by p_NewHandle, p_Status, p_PDU and p_VER.
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval #TRUE Flag indicating success, *p_Status will be set to GN_SUCCESS.
/// \retval #FALSE Flag indicating failure and that the status should be checked.
BL GN_SUPL_Push_Delivery_In(
   void**         p_NewHandle,///< [out] Opaque Handle Filled in by SUPL.
   e_GN_Status*   p_Status,   ///< [out] Status of disconnect and to be checked when return flag indicates failure.
   U2             PDU_Size,   ///< [in] Size of data at p_PDU in bytes.
   U1*            p_PDU,      ///< [in] Pointer to data.
   U2             VER_Size,   ///< [in] Size of \ref VER_Generation "hash" at p_VER in bytes.
   U1*            p_VER       ///< [in] Pointer to \ref VER_Generation "hash".
)
{
   s_SUPL_Instance *p_SUPL_Instance = NULL;
   U4 OS_TimeStamp;
   BL Status = TRUE;

   GN_SUPL_Log( "+GN_SUPL_Push_Delivery_In:" );
   GN_SUPL_Log( "GN_SUPL_api_call:    SUPL<=Host [label=\"GN_SUPL_Push_Delivery_In:\"];" );

   *p_Status = GN_SUCCESS; // Set the GN_Status value to success unless an error occurs.

   // Need to generate a handle.
   p_SUPL_Instance = SUPL_Instance_Request_New();
   if ( p_SUPL_Instance == NULL )
   {
      Status = FALSE;
   }
   else
   {
      *p_NewHandle = p_SUPL_Instance->Handle;

      p_SUPL_Instance->p_SUPL->State = state_SUPL_Push_Received;

      /*This also handles cases where SUPL PDU was not succesfully decoded. 
              In these cases it is assumed that session is a SUPL 1.0 transaction.    */

      p_SUPL_Instance->p_SUPL->p_VER = GN_Calloc( 1, SIZE_SHA1_HASH_IN_BYTES );

      if ( p_SUPL_Instance->p_SUPL->p_VER == NULL )
      {
         Status = FALSE;
      }
      else
      {
         if( VER_Size == 0 || p_VER == NULL)
         {
            int error_Sha;
            char server[256] ;
            s_SUPL_IpAddress *slp_address = supl_config_get_SUPL_SLP_address();
   
            strcpy(server,slp_address->Address) ;
#ifdef SUPL_API_LOGGING
            {
               U1 i;
    
               for ( i = 0 ;i < PDU_Size ; i++ )
               {
                  GN_SUPL_Log( "GN_SUPL_Push_Delivery_In: supl_init_Size == %d, supl_pdu_string = %x", PDU_Size, p_PDU[i]);
               }
    
            }
#endif
            p_SUPL_Instance->p_SUPL->p_VER = GN_Calloc( 1, SIZE_SHA1_HASH_IN_BYTES );
            
            error_Sha = GN_SUPL_GetHmac_sha1 ( (U1 *)server,
                                               p_PDU ,
                                               PDU_Size ,
                                               pHashedKeysha1
                                             );
            
            GN_SUPL_Log ( "Hashing with SHA1 returned with server address %s", server );
            
            memcpy (p_SUPL_Instance->p_SUPL->p_VER , pHashedKeysha1, SIZE_SHA1_HASH_IN_BYTES );
            
#ifdef SUPL_API_LOGGING
            {
               CH VER_String_sha[17];
               U1 i;
    
               for ( i = 0 ; i < 8 && i < SIZE_SHA1_HASH_IN_BYTES ; i++ ) // Ver ::= BIT STRING(SIZE (64)) i.e. 8 octets
               {
                  sprintf( &VER_String_sha[i*2], "%02x", pHashedKeysha1[i] );
               }
    
               GN_SUPL_Log( "GN_SUPL_Push_Delivery_In: VER_Size == %d, VER_SHA1 = 0x\"%s\"", SIZE_SHA1_HASH_IN_BYTES, VER_String_sha);
            }
#endif
            GN_SUPL_Log( "GN_SUPL_Push_Delivery_In:SUPL1.0 Hash Ver generated internally");

         }
         else
         {
            if ( p_SUPL_Instance->p_SUPL->p_VER != NULL )
            GN_Free( p_SUPL_Instance->p_SUPL->p_VER );
            p_SUPL_Instance->p_SUPL->p_VER = GN_Calloc( 1, VER_Size );
            memcpy (p_SUPL_Instance->p_SUPL->p_VER  , p_VER , VER_Size );
         }
      }

      s_PDU_Buffer_Store  *p_PDU_Buf = asn1_PDU_Buffer_Store_Alloc( PDU_Size );
 
      if ( p_PDU_Buf != NULL )
      {
             /*        @TODO : Side effect of removing this assignment. This code uses a snapshot of the configuration at the time of the request
       SUPL 2.0 requires that we always use updated configuration */
             /*
         p_SUPL_Instance->p_SUPL->SessionCfg.SUPL_PT_Supported.PT_agpsSETAssisted = supl_config_get_SUPL_agpsSETassisted();
         p_SUPL_Instance->p_SUPL->SessionCfg.SUPL_PT_Supported.PT_agpsSETBased = supl_config_get_SUPL_agpsSETBased();
         p_SUPL_Instance->p_SUPL->SessionCfg.SUPL_PT_Supported.PT_autonomousGPS = supl_config_get_SUPL_autonomousGPS();
         p_SUPL_Instance->p_SUPL->SessionCfg.SUPL_PT_Supported.PT_eCID = supl_config_get_SUPL_eCID(); */
 
         memcpy( p_PDU_Buf->PDU_Encoded.p_PDU_Data, p_PDU, PDU_Size );
         p_PDU_Buf->PDU_Encoded.Length = PDU_Size;
         OS_TimeStamp = GN_GPS_Get_OS_Time_ms();
         if ( OS_TimeStamp == 0 ) OS_TimeStamp++; // Make sure this is never 0.
         SUPL_Send_PDU_Received(
            NULL,
            NULL,
            p_PDU_Buf,
            OS_TimeStamp,
            p_SUPL_Instance->Handle );
      }
 
   }


   if ( Status == FALSE )
   {
      CH *StatusDesc;
      // Clear up instance and any allocated data.
      SUPL_Instance_Delete( p_SUPL_Instance );
      GN_SUPL_Log( "GN_SUPL_api_call:    SUPL>>Host [label=\"GN_SUPL_Push_Delivery_In: Failed\"];" );
      StatusDesc = GN_SUPL_StatusDesc_From_Status( *p_Status );
      GN_SUPL_Log( "GN_SUPL_Push_Delivery_In: Status Returned: GN_Status %x %s", *p_Status, StatusDesc );

      *p_Status = GN_ERR_NO_RESOURCE;
   }
   else
   {
      GN_SUPL_Log( "GN_SUPL_api_call:    SUPL>>Host [label=\"GN_SUPL_Push_Delivery_In: Handle=%p\"];", *p_NewHandle );
   }
   GN_SUPL_Log( "-GN_SUPL_Push_Delivery_In:" );
   return Status;
}


//*****************************************************************************
/// \brief
///      SUPL PDU delivery in.
/// \details
///      Delivers a SUPL PDU from an existing network connection.
///      <p> This function is called in response to data being sent over a
///      TCP/IP connection set up by a #GN_SUPL_Connect_Req_Out()
///      function call.
///      <p> The Handle is set to the Handle from the
///      #GN_SUPL_Connect_Req_Out() call.
///      <H3>p_Status values returned:</H3>
///      <ul>
///         <li><var>#GN_SUCCESS \copydoc GN_SUCCESS</var>\n
///            p_Status should be set to this value when the return value
///            indicated success.</li>
///         <li><var>#GN_ERR_HANDLE_INVALID \copydoc GN_ERR_HANDLE_INVALID</var>\n
///            p_Status should be set to this value when the Handle
///            passed was not recognised by the library software.</li>
///         <li><var>#GN_ERR_NO_RESOURCE \copydoc GN_ERR_NO_RESOURCE</var>\n
///            p_Status should be set to this value when there are not
///            enough resources on the host to perform the requested function.</li>
///      </ul>
/// \attention
///      The caller of #GN_SUPL_PDU_Delivery_In() must manage (e.g. alloc and free)
///      the memory referenced by p_Status and p_PDU.
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval #TRUE Flag indicating success, *p_Status will be set to GN_SUCCESS.
/// \retval #FALSE Flag indicating failure and that the status should be checked.
BL GN_SUPL_PDU_Delivery_In(
   void*          Handle,     ///< [in] Opaque Handle used to coordinate requests.
   e_GN_Status*   p_Status,   ///< [out] Status of PDU delivery and to be checked when return flag indicates failure.
   U2             PDU_Size,   ///< [in] Size of data at p_PDU in bytes.
   U1*            p_PDU       ///< [in] Pointer to data.
)
{
   BL Status;
   s_PDU_Buffer_Store  *p_PDU_Buf = asn1_PDU_Buffer_Store_Alloc( PDU_Size );
   U4 OS_TimeStamp;

   GN_SUPL_Log( "+GN_SUPL_PDU_Delivery_In:" );
   GN_SUPL_Log( "GN_SUPL_api_call:    SUPL<=Host [label=\"GN_SUPL_PDU_Delivery_In: Handle=%p\"];", Handle );

   Status = TRUE;
   *p_Status = GN_SUCCESS; // Set the GN_Status value to success unless an error occurs.

   if ( p_PDU_Buf != NULL )
   {
      memcpy( p_PDU_Buf->PDU_Encoded.p_PDU_Data, p_PDU, PDU_Size );
      p_PDU_Buf->PDU_Encoded.Length = PDU_Size;

      OS_TimeStamp = GN_GPS_Get_OS_Time_ms();
      if ( OS_TimeStamp == 0 ) OS_TimeStamp++; // Make sure this is never 0.

      SUPL_Send_PDU_Received(
         NULL,
         NULL,
         p_PDU_Buf,
         OS_TimeStamp,
         Handle );
   }
   else
   {
      CH *StatusDesc;

      Status = FALSE;
      *p_Status = GN_ERR_NO_RESOURCE;
      StatusDesc = GN_SUPL_StatusDesc_From_Status( *p_Status );
      GN_SUPL_Log( "GN_SUPL_PDU_Delivery_In: Status Returned: GN_Status %x %s", *p_Status, StatusDesc );
   }

   GN_SUPL_Log( "-GN_SUPL_PDU_Delivery_In:" );
   return Status;
}


//*****************************************************************************
/// \brief
///      SUPL notification response in.
/// \details
///      Provides a response from the user notification.
///      <p> This function is called in response to a notification request
///      sent in #GN_SUPL_Notification_Req_Out() required by the SUPL
///      exchange.
///      <p> The Handle is set to the Handle from the
///      #GN_SUPL_Notification_Req_Out() call.
///      <H3>p_Status values passed in:</H3>
///      <ul>
///         <li><var>#GN_SUCCESS \copydoc GN_SUCCESS</var>\n
///            p_Status should be set to this value when the #GN_SUPL_Disconnect_Req_Out()
///            was successful.</li>
///         <li><var>#GN_ERR_NO_RESOURCE \copydoc GN_ERR_NO_RESOURCE</var>\n
///            p_Status should be set to this value when there are not
///            enough resources on the host to perform the requested function.</li>
///      </ul>
///      <H3>p_Status values returned:</H3>
///      <ul>
///         <li><var>#GN_SUCCESS \copydoc GN_SUCCESS</var>\n
///            p_Status should be set to this value when the return value
///            indicated success.</li>
///         <li><var>#GN_ERR_HANDLE_INVALID \copydoc GN_ERR_HANDLE_INVALID</var>\n
///            p_Status should be set to this value when the Handle
///            passed was not recognised by the library software.</li>
///         <li><var>#GN_ERR_NO_RESOURCE \copydoc GN_ERR_NO_RESOURCE</var>\n
///            p_Status should be set to this value when there are not
///            enough resources on the host to perform the requested function.</li>
///      </ul>
/// \attention
///      The caller of #GN_SUPL_Notification_Rsp_In() must manage (e.g. alloc and free)
///      the memory referenced by p_Status.
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval #TRUE Flag indicating success, *p_Status will be set to GN_SUCCESS.
/// \retval #FALSE Flag indicating failure and that the status should be checked.
BL GN_SUPL_Notification_Rsp_In(
   void*          Handle,        ///< [in] Opaque Handle used to coordinate requests.
   e_GN_Status*   p_Status,      ///< [in/out] Status of notification and to be checked when return flag indicates failure.
   BL    Notification_Accepted   ///< [in] Flag to indicate the user response to the indication.
                                 /// <ul>
                                 ///   <li>#TRUE = Accepted by user.</li>
                                 ///   <li>#FALSE = Rejected by user.</li>
                                 /// </ul>
)
{
   GN_SUPL_Log( "+GN_SUPL_Notification_Rsp_In:" );
   GN_SUPL_Log( "GN_SUPL_api_call:    SUPL<=Host [label=\"GN_SUPL_Notification_Rsp_In: Handle=%p\"];", Handle );
   *p_Status = GN_SUCCESS; // Set the GN_Status value to success unless an error occurs.

   GN_SUPL_Log( "GN_SUPL_Notification_Rsp_In: Notification: %s", Notification_Accepted ? "Accepted" : "Rejected" );

   SUPL_Send_Notification_Response( 0, 0, Notification_Accepted, Handle );
   GN_SUPL_Log( "-GN_SUPL_Notification_Rsp_In:" );
   return TRUE;
}

//*****************************************************************************
/// \brief
///      SUPL position request in.
/// \details
///      Requests a position from the SUPL subsystem.
///      <p> This function is called to obtain a position from the SUPL
///      subsystem and is likely to initiate a Mobile Originated supl
///      sequence depending on QoP parameters.
///      <p> A Handle will be created by the SUPL subsystem and placed
///      in the location pointed at by p_NewHandle.
///      <H3>p_Status values returned:</H3>
///      <ul>
///         <li><var>#GN_SUCCESS \copydoc GN_SUCCESS</var>\n
///            p_Status should be set to this value when the return value
///            indicated success.</li>
///         <li><var>#GN_ERR_NO_RESOURCE \copydoc GN_ERR_NO_RESOURCE</var>\n
///            p_Status should be set to this value when there are not
///            enough resources on the host to perform the requested function.</li>
///      </ul>
/// \attention
///      The caller of #GN_SUPL_Position_Req_In() must manage (e.g. alloc and free)
///      the memory referenced by p_NewHandle, p_Status and p_SUPL_QoP.
/// \todo
///      Reject out of bound parameters with #GN_ERR_PARAMETER_INVALID.
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval #TRUE Flag indicating success, *p_Status will be set to #GN_SUCCESS.
/// \retval #FALSE Flag indicating failure and that the status should be checked.
BL GN_SUPL_Position_Req_In(
   void**         p_NewHandle,   ///< [out] Opaque Handle Filled in by SUPL.
   e_GN_Status*   p_Status,      ///< [out] Status of notification and to be checked when return flag indicates failure.
   s_GN_SUPL_QoP* p_SUPL_QoP     ///< [in] QoP criteria. NULL pointer if no QoP criteria set.
)
{
/* @todo : Internal SUPL functionality is accessed here from external call flow. Should be corrected */
   BL Status = TRUE;
   s_SUPL_Instance *p_SUPL_Instance;
   s_SUPL_IpAddress *p_CFG_IpAddress;

   s_GN_SUPL_Connect_Req_Parms SUPL_Connect_Req_Parms;

   GN_SUPL_Log( "+GN_SUPL_Position_Req_In:" );
   GN_SUPL_Log( "GN_SUPL_api_call:    SUPL<=Host [label=\"GN_SUPL_Position_Req_In\"];" );

   *p_Status = GN_SUCCESS; // Set the GN_Status value to success unless an error occurs.

   p_SUPL_Instance = SUPL_Instance_Request_New();

   if ( p_SUPL_Instance != NULL )
   {
      *p_NewHandle = p_SUPL_Instance->Handle;
      p_SUPL_Instance->p_SUPL->MO_Request = TRUE;
   }
   else
   {
      Status = FALSE;
      *p_Status = GN_ERR_NO_RESOURCE;
   }
   if ( Status )
   {
      if ( p_SUPL_QoP != NULL )
      {
         s_GN_QoP *p_QoP;

         p_SUPL_Instance->p_SUPL->p_GN_QoP = GN_Calloc( 1, sizeof( s_GN_QoP ) );

         p_QoP = p_SUPL_Instance->p_SUPL->p_GN_QoP;
         if ( p_QoP == NULL )
         {
             Status = FALSE;
             *p_Status = GN_ERR_NO_RESOURCE;
         }

         else
         {
            if ( p_SUPL_QoP->horacc <= 127 )
            {
               // Horizontal accuracy       INTEGER (0..127).
               // if QoP is present horacc is mandatory.
               p_QoP->horacc = p_SUPL_QoP->horacc;
               p_QoP->p_horacc = &p_QoP->horacc;
               #ifdef SUPL_API_LOGGING
                  GN_SUPL_Log( "GN_SUPL_Position_Req_In: QoP horacc ==  %u",
                               p_QoP->horacc );
               #endif
            }
            else
            {
               Status = FALSE;
               *p_Status = GN_ERR_PARAMETER_INVALID;
               GN_SUPL_Log( "GN_SUPL_Position_Req_In: QoP horacc ==  %u out of range 0..127",
                            p_SUPL_QoP->horacc );
            }


            // Vertical accuracy         INTEGER (0..127)   OPTIONAL (-1 = Not present).
             if ( p_SUPL_QoP->veracc != -1 )
            {
               if ( p_SUPL_QoP->veracc >= 0 /*&& p_SUPL_QoP->veracc <= 127 */ )
               {
                  p_QoP->veracc = (U1) p_SUPL_QoP->veracc;
                  p_QoP->p_veracc = &p_QoP->veracc;
#ifdef SUPL_API_LOGGING
                  GN_SUPL_Log( "GN_SUPL_Position_Req_In: QoP veracc ==  %u",
                               p_QoP->veracc );
#endif
               }
               else
               {
                  GN_SUPL_Log( "GN_SUPL_Position_Req_In: QoP veracc ==  %d out of range 0..127",
                                p_SUPL_QoP->veracc );
                  Status = FALSE;
                  *p_Status = GN_ERR_PARAMETER_INVALID;
               }
            }

            // Maximum age of location   INTEGER (0..65535) OPTIONAL (-1 = Not present).
            if ( p_SUPL_QoP->maxLocAge != -1 )
            {
               if ( p_SUPL_QoP->maxLocAge >= 0 || p_SUPL_QoP->maxLocAge <= 65535 )
               {
                   p_QoP->maxLocAge = (U2) p_SUPL_QoP->maxLocAge;
                   p_QoP->p_maxLocAge = &p_QoP->maxLocAge;
#ifdef SUPL_API_LOGGING
                   GN_SUPL_Log( "GN_SUPL_Position_Req_In: QoP maxLocAge ==  %u",
                                  p_QoP->maxLocAge );
#endif
               }
               else
               {
                   GN_SUPL_Log( "GN_SUPL_Position_Req_In: QoP maxLocAge ==  %d out of range 0..65535",
                               p_SUPL_QoP->maxLocAge );
                   Status = FALSE;
                  *p_Status = GN_ERR_PARAMETER_INVALID;
               }
            }

            // Maximum permissible delay INTEGER (0..7)     OPTIONAL (-1 = Not present).
            if ( p_SUPL_QoP->delay != -1 )
            {
               if ( p_SUPL_QoP->delay >= 0 && p_SUPL_QoP->delay <= 7 )
               {
                  ///\todo Add code to cope with 0 for a delay meaning return immediately with last known position.
                  p_QoP->delay = (U1) p_SUPL_QoP->delay;
                  p_QoP->p_delay = &p_QoP->delay;
#ifdef SUPL_API_LOGGING
                     GN_SUPL_Log( "GN_SUPL_Position_Req_In: QoP delay ==  %u",
                                  p_QoP->delay );
#endif
               }
               else
               {
                  GN_SUPL_Log( "GN_SUPL_Position_Req_In: QoP delay ==  %d out of range 0..7",
                               p_SUPL_QoP->delay );
                  Status = FALSE;
                  *p_Status = GN_ERR_PARAMETER_INVALID;
               }
            }
         }
      }
      if (Status)
      {
         p_CFG_IpAddress = supl_config_get_SUPL_SLP_address();
         SUPL_Connect_Req_Parms.TcpIp_AddressType = p_CFG_IpAddress->TcpIp_AddressType;
         SUPL_Connect_Req_Parms.p_TcpIp_Address = p_CFG_IpAddress->Address;
         SUPL_Connect_Req_Parms.Port = p_CFG_IpAddress->Port;
         #ifdef SUPL_API_LOGGING
            if ( SUPL_Connect_Req_Parms.TcpIp_AddressType != IP_None )
            {
               GN_SUPL_Log( "GN_SUPL_Position_Req_In: SLP IPAddress == %s, Port == %u",
                  SUPL_Connect_Req_Parms.p_TcpIp_Address,
                  SUPL_Connect_Req_Parms.Port );
            }
         #endif
/*        @TODO : Side effect of removing this assignment. This code uses a snapshot of the configuration at the time of the request
          SUPL 2.0 requires that we always use updated configuration */
/*
         p_SUPL_Instance->p_SUPL->SessionCfg.SUPL_PT_Supported.PT_agpsSETAssisted = supl_config_get_SUPL_agpsSETassisted();
         p_SUPL_Instance->p_SUPL->SessionCfg.SUPL_PT_Supported.PT_agpsSETBased = supl_config_get_SUPL_agpsSETBased();
         p_SUPL_Instance->p_SUPL->SessionCfg.SUPL_PT_Supported.PT_autonomousGPS = supl_config_get_SUPL_autonomousGPS();
         p_SUPL_Instance->p_SUPL->SessionCfg.SUPL_PT_Supported.PT_eCID = supl_config_get_SUPL_eCID(); */

         SUPL_Send_MO_Position_Requested(
            p_SUPL_Instance,
            p_SUPL_Instance,
            p_SUPL_Instance->Handle,
            &SUPL_Connect_Req_Parms );

         SUPL_Instance_State_Transition(  p_SUPL_Instance,
                                          state_SUPL_Comms_Open_Sent,
                                          "event_SUPL_MO_Position_Requested" );
      }
   }
   if ( Status )
   {
      GN_SUPL_Log( "GN_SUPL_api_call:    SUPL>>Host [label=\"GN_SUPL_Position_Req_In: Handle=%p\"];", *p_NewHandle );
   }
   GN_SUPL_Log( "-GN_SUPL_Position_Req_In:" );
   return Status;
}


//*****************************************************************************
/// \brief
///      GN SUPL API Function to Set or change the SUPL Library Configuration data.
/// \details
///      GN SUPL API Function to Set or change the SUPL Library Configuration data.
///      <p> Typically this function is called the once after #GN_SUPL_Handler_Init()
///      and before entering the main #GN_SUPL_Handler() processing loop.
/// \returns
///      Flag to indicate whether the new configuration settings were accepted.
/// \retval #TRUE if accepted.
/// \retval #FALSE if not accepted.
BL GN_SUPL_Set_Config(
   s_GN_SUPL_Config* p_Config    ///< [in] Pointer to where the SUPL Library can get the Configuration data from.
)
{
   GN_SUPL_Log( "+GN_SUPL_Set_Config:" );
   GN_SUPL_Log( "GN_SUPL_api_call:    SUPL<=Host [label=\"GN_SUPL_Set_Config\"];" );
#ifdef SUPL_API_LOGGING
      if ( p_Config->SLP_IP_Type != IP_None )
      {
         GN_SUPL_Log( "GN_SUPL_Set_Config: SLP IPAddress == %s, Port == %u",
            p_Config->SLP_IP_Address,
            p_Config->SLP_IP_Port );
      }
      if ( p_Config->SET_IP_Type != IP_None )
      {
         GN_SUPL_Log( "GN_SUPL_Set_Config: SET IPAddress == %s",
            p_Config->SET_IP_Address );
      }
      if ( p_Config->p_SET_imsi != NULL )
      {
         GN_SUPL_Log( "GN_SUPL_Set_Config: SET IMSI == %s", p_Config->p_SET_imsi );
      }
      if ( p_Config->p_SET_mdn != NULL )
      {
         GN_SUPL_Log( "GN_SUPL_Set_Config: SET mdn == %s", p_Config->p_SET_mdn );
      }
      if ( p_Config->p_SET_min != NULL )
      {
         GN_SUPL_Log( "GN_SUPL_Set_Config: SET min == %s", p_Config->p_SET_min );
      }
      if ( p_Config->p_SET_msisdn != NULL )
      {
         GN_SUPL_Log( "GN_SUPL_Set_Config: SET msisdn == %s", p_Config->p_SET_msisdn );
      }
      if ( p_Config->p_SET_nai != NULL )
      {
         GN_SUPL_Log( "GN_SUPL_Set_Config: SET nai == %s", p_Config->p_SET_nai );
      }
      GN_SUPL_Log( "GN_SUPL_Set_Config: User Timer 1 == %u", p_Config->User_Timer_1_Duration );
      GN_SUPL_Log( "GN_SUPL_Set_Config: User Timer 2 == %u", p_Config->User_Timer_2_Duration );
      GN_SUPL_Log( "GN_SUPL_Set_Config: User Timer 3 == %u", p_Config->User_Timer_3_Duration );

      if( p_Config->p_PlatfromVersion != NULL )
      {
         GN_SUPL_Log( "GN_SUPL_Set_Config: SET Platform Version == %s", p_Config->p_PlatfromVersion );
      }

#endif
   // e_TcpIp_AddressType SLP_IP_Type; // Type of IP address in SLP_IP_Address.
   // CH *SLP_IP_Address;              // SLP IP Address.
   // U2 SLP_IP_Port;                  // SLP IP Port (default should be 7275).
   supl_config_set_SUPL_SLP_address(  p_Config->SLP_IP_Type, p_Config->SLP_IP_Address, p_Config->SLP_IP_Port );
   // U2 User_Timer_1_Duration;        // SUPL UT1 value in seconds (default should be 10).
   supl_config_set_SUPL_UT1(             p_Config->User_Timer_1_Duration * 1000 );
   // U2 User_Timer_2_Duration;        // SUPL UT2 value in seconds (default should be 10).
   supl_config_set_SUPL_UT2(             p_Config->User_Timer_2_Duration * 1000 );
   // U2 User_Timer_3_Duration;        // SUPL UT3 value in seconds (default should be 10).
   supl_config_set_SUPL_UT3(             p_Config->User_Timer_3_Duration * 1000 );
   // CH *p_SET_msisdn;                // Optional MSISDN.
   supl_config_set_SUPL_msisdn(          p_Config->p_SET_msisdn );
   // CH *p_SET_mdn;                   // Optional MDN.
   supl_config_set_SUPL_mdn(             p_Config->p_SET_mdn );
   // CH *p_SET_min;                   // Optional MIN.
   // CH *p_SET_imsi;                  // Optional IMSI.
   supl_config_set_SUPL_imsi(            p_Config->p_SET_imsi );
   // CH *p_SET_nai;                   // Optional NAI.
   // e_TcpIp_AddressType SET_IP_Type; // Type of IP address in SET_IP_Address.
   // CH *SET_IP_Address;              // SET IP Address.
   // supl_config_set_SUPL_SET_address(p_Config->SET_IP_Address, p_Config->SET_IP_Port);
   GN_SUPL_Log( "GN_SUPL_Set_Config: agpsSETAssisted %s", p_Config->PT_agpsSETAssisted ? "    supported" : "not supported" );
   supl_config_set_SUPL_agpsSETassisted( p_Config->PT_agpsSETAssisted );
   GN_SUPL_Log( "GN_SUPL_Set_Config:    agpsSETBased %s", p_Config->PT_agpsSETBased    ? "    supported" : "not supported" );
   supl_config_set_SUPL_agpsSETBased(    p_Config->PT_agpsSETBased );
   GN_SUPL_Log( "GN_SUPL_Set_Config:   autonomousGPS %s", p_Config->PT_autonomousGPS   ? "    supported" : "not supported" );
   supl_config_set_SUPL_autonomousGPS(   p_Config->PT_autonomousGPS );
   GN_SUPL_Log( "GN_SUPL_Set_Config:            eCID %s", p_Config->PT_eCID            ? "    supported" : "not supported" );
   supl_config_set_SUPL_eCID(            p_Config->PT_eCID );

   supl_config_set_platform_version( p_Config->p_PlatfromVersion );

   /* Addition to force default client config to SUPLv1.0 */
   {
      U1 MajorVersion;

      supl_config_get_SUPL_version(&MajorVersion , NULL , NULL);

      if( 0 == MajorVersion )
      {
         /* This is possible only in default case where the configuration has not been overwritten */
         supl_config_set_SUPL_version(1,0,0);
      }
   }

   GN_SUPL_Log( "-GN_SUPL_Set_Config:" );
   return TRUE;
}


//*****************************************************************************
/// \brief
///      GN SUPL API Function to Set or change the SUPL Library Extended Configuration data.
/// \details
///      GN SUPL API Function to Set or change the SUPL Library Extended Configuration data.
///      <p> Typically this function is called the once after #GN_SUPL_Handler_Init()
///      and before entering the main #GN_SUPL_Handler() processing loop.
///      This configuration will fail if the major version requested is not supported by
///      supl library
/// \returns
///      Flag to indicate whether the new configuration settings were accepted.
/// \retval #TRUE if accepted.
/// \retval #FALSE if not accepted.

BL GN_SUPL_Extd_Set_Config(
   s_GN_SUPL_Extd_Config* p_Config    ///< [in] Pointer to where the SUPL Library can get the Configuration data from.
)
{
   U1 vl_Index;

   GN_SUPL_Log("+GN_SUPL_Extd_Set_Config:");

   if(  p_Config->Major_Version == 1 || p_Config->Major_Version == 2 )
   {
      GN_SUPL_Log( "GN_SUPL_Extd_Set_Config:   Version                          %d.0.0", p_Config->Major_Version );
      supl_config_set_SUPL_version(p_Config->Major_Version,0,0);
   }
   else
   {
      GN_SUPL_Log( "GN_SUPL_Extd_Set_Config:   Invalid MajorVersion             %d", p_Config->Major_Version );
      return FALSE;
   }
   /* Beginning of SUPLv2.0 specific configuration */
   
   // U2 ESLP_IP_Port;                  // ESLP IP Port (default should be 7275).
   supl_config_set_SUPL_ESLP_address(  p_Config->ESLP_IP_Type, p_Config->ESLP_IP_Address, p_Config->ESLP_IP_Port );

   GN_SUPL_Log( "GN_SUPL_Extd_Set_Config:   Periodic Trigger               %s", p_Config->Periodic_Trigger ? "    supported" : "not supported" );
   supl_config_set_Periodic_Trigger(p_Config->Periodic_Trigger);

   GN_SUPL_Log( "GN_SUPL_Extd_Set_Config:   Area Event Trigger             %s", p_Config->Area_Event_Trigger? "    supported" : "not supported" );
   supl_config_set_AreaEvent_Trigger(p_Config->Area_Event_Trigger);

   GN_SUPL_Log( "GN_SUPL_Extd_Set_Config:   GeographicArea Ellipse         %s", p_Config->Geographic_Area_Ellipse ? "    supported" : "not supported" );
   supl_config_set_Geographic_Area_Ellipse(p_Config->Geographic_Area_Ellipse);

   GN_SUPL_Log( "GN_SUPL_Extd_Set_Config:   GeographicArea Polygon         %s", p_Config->Geographic_Area_Polygon ? "    supported" : "not supported" );
   supl_config_set_Geographic_Area_Polygon(p_Config->Geographic_Area_Polygon);

   GN_SUPL_Log( "GN_SUPL_Extd_Set_Config:   Max_Geographic_Areas           %u", p_Config->Max_Geographic_Areas);
   supl_config_set_Max_Geographic_Areas(p_Config->Max_Geographic_Areas);

   GN_SUPL_Log( "GN_SUPL_Extd_Set_Config:   Max_Area_ID_Lists              %u", p_Config->Max_Area_ID_Lists);
   supl_config_set_Max_Area_ID_Lists(p_Config->Max_Area_ID_Lists);

   GN_SUPL_Log( "GN_SUPL_Extd_Set_Config:   Max_Area_ID_Per_List           %u", p_Config->Max_Area_ID_Per_List);
   supl_config_set_Max_Area_ID_Per_List(p_Config->Max_Area_ID_Per_List);

   GN_SUPL_Log( "GN_SUPL_Extd_Set_Config:   Max_Num_Session                %u", p_Config->Max_Num_Session);
   supl_config_set_Max_Num_Session(p_Config->Max_Num_Session);

   GN_SUPL_Log( "GN_SUPL_Extd_Set_Config:   Reporting_Mode_Real_Time       %s", p_Config->Reporting_Mode_Real_Time ? "    supported" : "not supported" );
   supl_config_set_Reporting_Mode_Real_Time(p_Config->Reporting_Mode_Real_Time);

   GN_SUPL_Log( "GN_SUPL_Extd_Set_Config:   Reporting_Mode_Quasi_Real_Time %s", p_Config->Reporting_Mode_Quasi_Real_Time ? "    supported" : "not supported" );
   supl_config_set_Reporting_Mode_Quasi_Real_Time(p_Config->Reporting_Mode_Quasi_Real_Time);

   GN_SUPL_Log( "GN_SUPL_Extd_Set_Config:   Reporting_Mode_Batch           %s", p_Config->Reporting_Mode_Batch ? "    supported" : "not supported" );
   supl_config_set_Reporting_Mode_Batch(p_Config->Reporting_Mode_Batch);

   GN_SUPL_Log( "GN_SUPL_Extd_Set_Config:   Batch_Report_Position          %s", p_Config->Batch_Report_Position ? "    supported" : "not supported" );
   supl_config_set_Batch_Report_Position(p_Config->Batch_Report_Position);

   GN_SUPL_Log( "GN_SUPL_Extd_Set_Config:   Batch_Report_Measurements      %s", p_Config->Batch_Report_Measurements ? "    supported" : "not supported" );
   supl_config_set_Batch_Report_Measurements(p_Config->Batch_Report_Measurements);

   GN_SUPL_Log( "GN_SUPL_Extd_Set_Config:   Min_Report_Interval            %u", p_Config->Min_Report_Interval);
   supl_config_set_Min_Report_Interval(p_Config->Min_Report_Interval);

   GN_SUPL_Log( "GN_SUPL_Extd_Set_Config:   Max_Report_Interval            %u", p_Config->Max_Report_Interval);
   supl_config_set_Max_Report_Interval(p_Config->Max_Report_Interval);

   GN_SUPL_Log( "GN_SUPL_Extd_Set_Config:   Max_Positions_In_Batch         %u", p_Config->Max_Positions_In_Batch);
   supl_config_set_Max_Positions_In_Batch(p_Config->Max_Positions_In_Batch);

   GN_SUPL_Log( "GN_SUPL_Extd_Set_Config:   Max_Measurement_In_Batch       %u", p_Config->Max_Measurement_In_Batch);
   supl_config_set_Max_Measurement_In_Batch(p_Config->Max_Measurement_In_Batch);

   GN_SUPL_Log( "GN_SUPL_Set_Config: User Timer 5 == %u", p_Config->User_Timer_5_Duration );
   supl_config_set_SUPL_UT5(             p_Config->User_Timer_5_Duration * 1000 );

   GN_SUPL_Log( "GN_SUPL_Set_Config: User Timer 6 == %u", p_Config->User_Timer_6_Duration );
   supl_config_set_SUPL_UT6(             p_Config->User_Timer_6_Duration * 1000 );

   GN_SUPL_Log( "GN_SUPL_Set_Config: User Timer 7 == %u", p_Config->User_Timer_7_Duration );
   supl_config_set_SUPL_UT7(             p_Config->User_Timer_7_Duration * 1000 );

   GN_SUPL_Log( "GN_SUPL_Set_Config: User Timer 8 == %u", p_Config->User_Timer_8_Duration );
   supl_config_set_SUPL_UT8(             p_Config->User_Timer_8_Duration * 1000 );

   GN_SUPL_Log( "GN_SUPL_Set_Config: User Timer 9 == %u", p_Config->User_Timer_9_Duration );
   supl_config_set_SUPL_UT9(             p_Config->User_Timer_9_Duration * 1000 );

   GN_SUPL_Log( "GN_SUPL_Set_Config: User Timer 10 == %u", p_Config->User_Timer_10_Duration );
   supl_config_set_SUPL_UT10(            p_Config->User_Timer_10_Duration * 1000 );
    
   GN_SUPL_Log( "GN_SUPL_Set_Config: supl_config_set_SUPL_eslp_whitelist_entry");

   for(vl_Index=0; vl_Index < MAX_ESLP_ADDRESS ; vl_Index++)
   {
      supl_config_set_SUPL_eslp_whitelist_entry( p_Config->p_ESlp_WhiteList[vl_Index] , vl_Index );
   }
   
   GN_SUPL_Log("-GN_SUPL_Extd_Set_Config:");

   return TRUE;
}

/// \}

//*****************************************************************************
/// \addtogroup GN_SUPL_api_H
/// \{

//*****************************************************************************
/// \brief
///      SUPL outgoing connection request.
/// \details
///      Requests a connection for SUPL to an external network entity.
///      <p> The Handle is set by the SUPL Handler or is set to Handle
///      provided by the #GN_SUPL_Push_Delivery_In() call.
///      <p> Following the #GN_SUPL_Connect_Req_Out() the SUPL subsystem waits
///      for a #GN_SUPL_Connect_Ind_In() when the connection to the SLP is
///      established.
///      <H3>p_Status values returned:</H3>
///      <ul>
///         <li><var>#GN_SUCCESS \copydoc GN_SUCCESS</var>\n
///            p_Status should be set to this value when the return value
///            indicated success.</li>
///         <li><var>#GN_ERR_NO_RESOURCE \copydoc GN_ERR_NO_RESOURCE</var>\n
///            p_Status should be set to this value when there are not
///            enough resources on the host to perform the requested function.</li>
///      </ul>
/// \note
///      This function must be implemented by the Host software and will be
///      called by functions from within the GloNav Library.
/// \attention
///      The caller of #GN_SUPL_Connect_Req_Out() must manage (e.g. alloc and free)
///      the memory referenced by p_Status and p_SUPL_Connect_Req_Parms.
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval #TRUE Flag indicating success, *p_Status will be set to GN_SUCCESS.
/// \retval #FALSE Flag indicating failure and that the status should be checked.
BL GN_SUPL_Connect_Req_Out_Wrapper(
   void*          Handle,        ///< [in] Opaque Handle used to coordinate requests.
   e_GN_Status*   p_Status,      ///< [out] Status of connection request and to be checked when return flag indicates failure.
   s_GN_SUPL_Connect_Req_Parms*
      p_SUPL_Connect_Req_Parms   ///< [in] Details of connection.
)
{
   BL Status;
   CH *StatusDesc;

   GN_SUPL_Log( "+GN_SUPL_Connect_Req_Out:" );
   GN_SUPL_Log( "GN_SUPL_api_call:    SUPL=>Host [label=\"GN_SUPL_Connect_Req_Out: Handle=%p\"];", Handle );
   Status = GN_SUPL_Connect_Req_Out( Handle, p_Status, p_SUPL_Connect_Req_Parms );
   if (!Status)
   {
      StatusDesc = GN_SUPL_StatusDesc_From_Status( *p_Status );
      GN_SUPL_Log( "GN_SUPL_Connect_Req_Out: Status Returned: GN_Status %x %s", *p_Status, StatusDesc );
   }
   GN_SUPL_Log( "-GN_SUPL_Connect_Req_Out:" );
   return Status;
}


//*****************************************************************************
/// \brief
///      SUPL disconnection request.
/// \details
///      Requests a disconnection for an existing SUPL connection.
///      <p> The Handle is set to the Handle from the
///      #GN_SUPL_Connect_Req_Out() call.
///      <H3>p_Status values passed in:</H3>
///      <ul>
///         <li><var>#GN_SUCCESS \copydoc GN_SUCCESS</var>\n
///            p_Status should be set to this value when the #GN_SUPL_Disconnect_Req_Out()
///            was successful.</li>
///         <li><var>#GN_ERR_CONN_SHUTDOWN \copydoc GN_ERR_CONN_SHUTDOWN</var>\n
///            p_Status should be set to this value when the connection
///            associated with the Handle was spontaneously shut down.</li>
///      </ul>
///      <H3>p_Status values returned:</H3>
///      <ul>
///         <li><var>#GN_SUCCESS \copydoc GN_SUCCESS</var>\n
///            p_Status should be set to this value when the return value
///            indicated success.</li>
///         <li><var>#GN_ERR_HANDLE_INVALID \copydoc GN_ERR_HANDLE_INVALID</var>\n
///            p_Status should be set to this value when the Handle
///            passed was not recognised by the host software.</li>
///         <li><var>#GN_ERR_NO_RESOURCE \copydoc GN_ERR_NO_RESOURCE</var>\n
///            p_Status should be set to this value when there are not
///            enough resources on the host to perform the requested function.</li>
///         <li><var>#GN_ERR_CONN_SHUTDOWN \copydoc GN_ERR_CONN_SHUTDOWN</var>\n
///            p_Status should be set to this value when the connection
///            associated with the handle is no longer available.</li>
///      </ul>
/// \note
///      This function must be implemented by the Host software and will be
///      called by functions from within the GloNav Library.
/// \attention
///      The caller of #GN_SUPL_Disconnect_Req_Out() must manage (e.g. alloc and free)
///      the memory referenced by p_Status.
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval #TRUE Flag indicating success, *p_Status will be set to GN_SUCCESS.
/// \retval #FALSE Flag indicating failure and that the status should be checked.
BL GN_SUPL_Disconnect_Req_Out_Wrapper(
   void*          Handle,     ///< [in] Opaque Handle used to coordinate requests.
   e_GN_Status*   p_Status    ///< [in/out] Status of disconnect and to be checked when return flag indicates failure.
)
{
   BL Status;
   CH *StatusDesc;

   GN_SUPL_Log( "+GN_SUPL_Disconnect_Req_Out:" );
   GN_SUPL_Log( "GN_SUPL_api_call:    SUPL=>Host [label=\"GN_SUPL_Disconnect_Req_Out: Handle=%p\"];", Handle );
   if ( *p_Status != GN_SUCCESS )
   {
      StatusDesc = GN_SUPL_StatusDesc_From_Status( *p_Status );
      GN_SUPL_Log( "GN_SUPL_Disconnect_Req_Out: Status Passed In: GN_Status %x %s", *p_Status, StatusDesc );
   }
   Status = GN_SUPL_Disconnect_Req_Out( Handle, p_Status );
   if ( ! Status )
   {
      StatusDesc = GN_SUPL_StatusDesc_From_Status( *p_Status );
      GN_SUPL_Log( "GN_SUPL_Disconnect_Req_Out: Status Returned: GN_Status %x %s", *p_Status, StatusDesc );
   }
   GN_SUPL_Log( "-GN_SUPL_Disconnect_Req_Out:" );
   return Status;
}


//*****************************************************************************
/// \brief
///      SUPL PDU delivery out.
/// \details
///      Delivers a SUPL PDU to an existing network connection.
///      <p> The Handle is set to the Handle from the
///      #GN_SUPL_Connect_Req_Out() call.
///      <H3>p_Status values returned:</H3>
///      <ul>
///         <li><var>#GN_SUCCESS \copydoc GN_SUCCESS</var>\n
///            p_Status should be set to this value when the return value
///            indicated success.</li>
///         <li><var>#GN_ERR_HANDLE_INVALID \copydoc GN_ERR_HANDLE_INVALID</var>\n
///            p_Status should be set to this value when the Handle
///            passed was not recognised by the host software.</li>
///         <li><var>#GN_ERR_POINTER_INVALID \copydoc GN_ERR_POINTER_INVALID</var>\n
///            p_Status should be set to this value when the p_PDU is NULL.</li>
///         <li><var>#GN_ERR_PARAMETER_INVALID \copydoc GN_ERR_PARAMETER_INVALID</var>\n
///            p_Status should be set to this value when the PDU_Size is 0.</li>
///         <li><var>#GN_ERR_NO_RESOURCE \copydoc GN_ERR_NO_RESOURCE</var>\n
///            p_Status should be set to this value when there are not
///            enough resources on the host to perform the requested function.</li>
///         <li><var>#GN_ERR_CONN_SHUTDOWN \copydoc GN_ERR_CONN_SHUTDOWN</var>\n
///            p_Status should be set to this value when the connection
///            associated with the handle is no longer available.</li>
///      </ul>
/// \note
///      This function must be implemented by the Host software and will be
///      called by functions from within the GloNav Library.
/// \attention
///      The caller of #GN_SUPL_PDU_Delivery_Out() must manage (e.g. alloc and free)
///      the memory referenced by p_Status and p_PDU.
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval #TRUE Flag indicating success, *p_Status will be set to GN_SUCCESS.
/// \retval #FALSE Flag indicating failure and that the status should be checked.
BL GN_SUPL_PDU_Delivery_Out_Wrapper(
   void*          Handle,     ///< [in] Opaque Handle used to coordinate requests.
   e_GN_Status*   p_Status,   ///< [out] Status of PDU delivery and to be checked when return flag indicates failure.
   U2             PDU_Size,   ///< [in] Size of data at p_PDU in bytes.
   U1*            p_PDU       ///< [in] Pointer to data.
)
{
   BL Status;
   CH *StatusDesc;

   GN_SUPL_Log( "+GN_SUPL_PDU_Delivery_Out:" );
   GN_SUPL_Log( "GN_SUPL_api_call:    SUPL=>Host [label=\"GN_SUPL_PDU_Delivery_Out: Handle=%p\"];", Handle );
   Status = GN_SUPL_PDU_Delivery_Out(Handle, p_Status, PDU_Size, p_PDU );
   if ( ! Status )
   {
      StatusDesc = GN_SUPL_StatusDesc_From_Status( *p_Status );
      GN_SUPL_Log( "GN_SUPL_PDU_Delivery_Out: Status Returned: GN_Status %x %s", *p_Status, StatusDesc );
   }
   GN_SUPL_Log( "-GN_SUPL_PDU_Delivery_Out:" );
   return Status;
}


//*****************************************************************************
/// \brief
///      SUPL notification request out.
/// \details
///      Indicates the handset user should be notified of a location request.
///      Depending on the parameters in the SUPL exchange the user may be
///      given the option to reject the request.
///      <p> The Handle is set to the Handle from the
///      #GN_SUPL_Push_Delivery_In() call as a notification will only originate from
///      a SUPL-INIT.
///      <p> The host will respond with #GN_SUPL_Notification_Rsp_In() indicating
///      whether the positioning sequence may proceed. If no response is available
///      from the user the Host will respond after a suitable time has elapsed.
///      <H3>p_Status values returned:</H3>
///      <ul>
///         <li><var>#GN_SUCCESS \copydoc GN_SUCCESS</var>\n
///            p_Status should be set to this value when the return value
///            indicated success.</li>
///         <li><var>#GN_ERR_HANDLE_INVALID \copydoc GN_ERR_HANDLE_INVALID</var>\n
///            p_Status should be set to this value when the Handle
///            passed was not recognised by the host software.</li>
///         <li><var>#GN_ERR_POINTER_INVALID \copydoc GN_ERR_POINTER_INVALID</var>\n
///            p_Status should be set to this value when the p_SUPL_Notification_Parms
///            is NULL.</li>
///         <li><var>#GN_ERR_PARAMETER_INVALID \copydoc GN_ERR_PARAMETER_INVALID</var>\n
///            p_Status should be set to this value when the PDU_Size is 0.</li>
///         <li><var>#GN_ERR_NO_RESOURCE \copydoc GN_ERR_NO_RESOURCE</var>\n
///            p_Status should be set to this value when there are not
///            enough resources on the host to perform the requested function.</li>
///      </ul>
/// \note
///      This function must be implemented by the Host software and will be
///      called by functions from within the GloNav Library.
/// \attention
///      The caller of #GN_SUPL_Notification_Req_Out() must manage (e.g. alloc and free)
///      the memory referenced by p_Status and p_SUPL_Notification_Parms.
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval #TRUE Flag indicating success, *p_Status will be set to GN_SUCCESS.
/// \retval #FALSE Flag indicating failure and that the status should be checked.
BL GN_SUPL_Notification_Req_Out_Wrapper(
   void*          Handle,           ///< [in] Opaque Handle used to coordinate requests.
   e_GN_Status*   p_Status,         ///< [out] Status of disconnect and to be checked when return flag indicates failure.
   s_GN_SUPL_Notification_Parms*
      p_SUPL_Notification_Parms     ///< [in] Pointer to a set of notification parameters.
)
{
// Notification ::= SEQUENCE {
//    notificationType  NotificationType,
//    encodingType      EncodingType OPTIONAL,
//    requestorId       OCTET STRING(SIZE (1..maxReqLength)) OPTIONAL,
//    requestorIdType   FormatIndicator OPTIONAL,
//    clientName        OCTET STRING(SIZE (1..maxClientLength)) OPTIONAL,
//    clientNameType    FormatIndicator OPTIONAL,
//    ...}
// NotificationType ::= ENUMERATED {
//    noNotificationNoVerification(0), notificationOnly(1),
//    notificationAndVerficationAllowedNA(2),
//    notificationAndVerficationDeniedNA(3), privacyOverride(4), ...
//    }
//
// EncodingType ::= ENUMERATED {ucs2(0), gsmDefault(1), utf8(2), ...
//                             }
//
// maxReqLength INTEGER ::= 50
//
// maxClientLength INTEGER ::= 50
//
// FormatIndicator ::= ENUMERATED {
//    logicalName(0), e-mailAddress(1), msisdn(2), url(3), sipUrl(4), min(5),
//    mdn(6), imsPublicIdentity(7), ...
//    }
   BL Status;
   CH *StatusDesc;
   CH *EncodingType = "";

   GN_SUPL_Log( "+GN_SUPL_Notification_Req_Out:" );
   GN_SUPL_Log( "GN_SUPL_api_call:    SUPL=>Host [label=\"GN_SUPL_Notification_Req_Out: Handle=%p\"];", Handle );
   #ifdef SUPL_API_LOGGING
      switch ( p_SUPL_Notification_Parms->GN_SUPL_Notify_Type )
      {
      case NOTIFY_NONE:                // Provide no notification to the user but indicate permission or rejection of request.
         GN_SUPL_Log( "GN_SUPL_Notification_Req_Out: Notify Type = NOTIFY_NONE" );
         break;
      case NOTIFY_ONLY:                // Provide notification to the user.
         GN_SUPL_Log( "GN_SUPL_Notification_Req_Out: Notify Type = NOTIFY_ONLY" );
         break;
      case NOTIFY_ALLOWED_ON_TIMEOUT:  // Provide confirmation request which will default to being allowed if no response.
         GN_SUPL_Log( "GN_SUPL_Notification_Req_Out: Notify Type = NOTIFY_ALLOWED_ON_TIMEOUT" );
         break;
      case NOTIFY_DENIED_ON_TIMEOUT:   // Provide confirmation request which will default to being disallowed if no response.
         GN_SUPL_Log( "GN_SUPL_Notification_Req_Out: Notify Type = NOTIFY_DENIED_ON_TIMEOUT" );
         break;
      case NOTIFY_PRIVACY_OVERRIDE:    // Provide notification that a privacy overide supl request has been received and no logging should take place.
         GN_SUPL_Log( "GN_SUPL_Notification_Req_Out: Notify Type = NOTIFY_PRIVACY_OVERRIDE" );
         break;
      }
      switch ( p_SUPL_Notification_Parms->GN_SUPL_Encoding_Type )
      {
      case ENCODING_NONE:        EncodingType = "NONE" ;       break ; // No encoding type specified in SUPL-INIT.
      case ENCODING_ucs2:        EncodingType = "ucs2" ;       break ; // Notification identifier encoded using UCS2.
      case ENCODING_gsmDefault:  EncodingType = "gsmDefault" ; break ; // Notification identifier encoded using GSM Default.
      case ENCODING_utf8:        EncodingType = "utf8" ;       break ; // Notification identifier encoded using UTF8.
      }
      GN_SUPL_Log( "GN_SUPL_Notification_Req_Out: Encoding type:\"%s\"",
                   EncodingType );

      if ( p_SUPL_Notification_Parms->ClientNameLen > 0 )
      {
         CH *Notify_Fmt_Ind = "" ;

         switch ( p_SUPL_Notification_Parms->ClientNameType )
         {
         case NOTIFY_noFormat:            Notify_Fmt_Ind = "Unspecified"         ; break ; // No format specified.
         case NOTIFY_logicalName:         Notify_Fmt_Ind = "Name"                ; break ; // Identifier contains a Name.
         case NOTIFY_e_mailAddress:       Notify_Fmt_Ind = "Email Address"       ; break ; // Identifier contains an Email Address.
         case NOTIFY_msisdn:              Notify_Fmt_Ind = "MSISDN"              ; break ; // Identifier contains an MSISDN.
         case NOTIFY_url:                 Notify_Fmt_Ind = "URL"                 ; break ; // Identifier contains a URL.
         case NOTIFY_sipUrl:              Notify_Fmt_Ind = "SIP URL"             ; break ; // Identifier contains a SIP URL.
         case NOTIFY_min:                 Notify_Fmt_Ind = "MIN"                 ; break ; // Identifier contains an MIN.
         case NOTIFY_mdn:                 Notify_Fmt_Ind = "MDN"                 ; break ; // Identifier contains an MDN.
         case NOTIFY_imsPublicIdentity:   Notify_Fmt_Ind = "IMS Public Identity" ; break ; // Identifier contains an IMS Public Identity.
         }
         GN_SUPL_Log( "GN_SUPL_Notification_Req_Out: Client Name Notification format indicator: \"%s\"",
                      Notify_Fmt_Ind );
         {
            CH ClientName_String[ 50 * 2 + 1 ]; // maxClientLength INTEGER ::= 50
            U1 i;

            ClientName_String[0] = '\0'; // Set it to an empty string.
            for ( i = 0 ; i < 50 && i < p_SUPL_Notification_Parms->ClientNameLen ; i++ ) // maxClientLength INTEGER ::= 50
            {
               sprintf( &ClientName_String[i*2], "%02x", p_SUPL_Notification_Parms->ClientName[i] );
            }

            GN_SUPL_Log( "GN_SUPL_Notification_Req_Out: Client Name Length == %d, Client Name = 0x\"%s\"",
                         p_SUPL_Notification_Parms->ClientNameLen,
                         ClientName_String );
         }
      }
      if ( p_SUPL_Notification_Parms->RequestorIdLen > 0 )
      {
         CH *Notify_Fmt_Ind = "" ;

         switch ( p_SUPL_Notification_Parms->RequestorIdType )
         {
         case NOTIFY_noFormat:            Notify_Fmt_Ind = "Unspecified"         ; break ; // No format specified.
         case NOTIFY_logicalName:         Notify_Fmt_Ind = "Name"                ; break ; // Identifier contains a Name.
         case NOTIFY_e_mailAddress:       Notify_Fmt_Ind = "Email Address"       ; break ; // Identifier contains an Email Address.
         case NOTIFY_msisdn:              Notify_Fmt_Ind = "MSISDN"              ; break ; // Identifier contains an MSISDN.
         case NOTIFY_url:                 Notify_Fmt_Ind = "URL"                 ; break ; // Identifier contains a URL.
         case NOTIFY_sipUrl:              Notify_Fmt_Ind = "SIP URL"             ; break ; // Identifier contains a SIP URL.
         case NOTIFY_min:                 Notify_Fmt_Ind = "MIN"                 ; break ; // Identifier contains an MIN.
         case NOTIFY_mdn:                 Notify_Fmt_Ind = "MDN"                 ; break ; // Identifier contains an MDN.
         case NOTIFY_imsPublicIdentity:   Notify_Fmt_Ind = "IMS Public Identity" ; break ; // Identifier contains an IMS Public Identity.
         }
         GN_SUPL_Log( "GN_SUPL_Notification_Req_Out: Requestor ID Notification format indicator: \"%s\"",
                      Notify_Fmt_Ind );
         {
            CH RequestorId_String[ 50 * 2 + 1 ]; // maxReqLength INTEGER ::= 50
            U1 i;

            RequestorId_String[0] = '\0'; // Set it to an empty string.
            for ( i = 0 ; i < 50 && i < p_SUPL_Notification_Parms->RequestorIdLen ; i++ ) // maxReqLength INTEGER ::= 50
            {
               sprintf( &RequestorId_String[i*2], "%02x", p_SUPL_Notification_Parms->RequestorId[i] );
            }

            GN_SUPL_Log( "GN_SUPL_Notification_Req_Out: Requestor ID Length == %d, Requestor ID = 0x\"%s\"",
                         p_SUPL_Notification_Parms->RequestorIdLen,
                         RequestorId_String );
         }
      }
   #endif
   Status = GN_SUPL_Notification_Req_Out( Handle, p_Status, p_SUPL_Notification_Parms );
   if ( ! Status )
   {
      StatusDesc = GN_SUPL_StatusDesc_From_Status( *p_Status );
      GN_SUPL_Log( "GN_SUPL_Notification_Req_Out: Status Returned: GN_Status %x %s", *p_Status, StatusDesc );
   }
   GN_SUPL_Log( "-GN_SUPL_Notification_Req_Out:" );
   return Status;
}


//*****************************************************************************
/// \brief
///      SUPL position response out.
/// \details
///      Provides the Position calculated by the SUPL subsystem to the host.
///      <p> The Handle is set to the Handle from the
///      #GN_SUPL_Push_Delivery_In() or a #GN_SUPL_Position_Req_In().
///      <H3>p_Status values returned:</H3>
///      <ul>
///         <li><var>#GN_SUCCESS \copydoc GN_SUCCESS</var>\n
///            p_Status should be set to this value when the return value
///            indicated success.</li>
///         <li><var>#GN_ERR_HANDLE_INVALID \copydoc GN_ERR_HANDLE_INVALID</var>\n
///            p_Status should be set to this value when the Handle
///            passed was not recognised by the host software.</li>
///         <li><var>#GN_ERR_POINTER_INVALID \copydoc GN_ERR_POINTER_INVALID</var>\n
///            p_Status should be set to this value when the p_GN_AGPS_GAD_Data
///            is NULL.</li>
///         <li><var>#GN_ERR_NO_RESOURCE \copydoc GN_ERR_NO_RESOURCE</var>\n
///            p_Status should be set to this value when there are not
///            enough resources on the host to perform the requested function.</li>
///      </ul>
/// \note
///      This function must be implemented by the Host software and will be
///      called by functions from within the GloNav Library.
/// \attention
///      The caller of #GN_SUPL_Position_Resp_Out() must manage (e.g. alloc and free)
///      the memory referenced by p_Status and p_GN_AGPS_GAD_Data.
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval #TRUE Flag indicating success, *p_Status will be set to GN_SUCCESS.
/// \retval #FALSE Flag indicating failure and that the status should be checked.
BL GN_SUPL_Position_Resp_Out_Wrapper(
   void*          Handle,     ///< [in] Opaque Handle used to coordinate requests.
   e_GN_Status*   p_Status,   ///< [out] Status of disconnect and to be checked when return flag indicates failure.
   s_GN_SUPL_Pos_Data*
      p_GN_SUPL_Pos_Data      ///< [in] Data structure populated containing the position solution.
)
{
   BL Status;
   CH *StatusDesc;

   GN_SUPL_Log( "+GN_SUPL_Position_Resp_Out:" );
   GN_SUPL_Log( "GN_SUPL_api_call:    SUPL=>Host [label=\"GN_SUPL_Position_Resp_Out: Handle=%p\"];", Handle );
   #ifdef SUPL_API_LOGGING
      if ( *p_Status == GN_WRN_NO_POSITION )
      {
         GN_SUPL_Log( "GN_SUPL_Position_Resp_Out: No position reported." );
      }
      else if ( *p_Status == GN_SUCCESS )
      {
         GN_SUPL_Log( "******************************************************************************************" );

         GN_SUPL_Log( "GN_SUPL_Position_Resp_Out: Timestamp: %4u/%02u/%02u %02u:%02u:%02u.%03u",
            p_GN_SUPL_Pos_Data->Year,
            p_GN_SUPL_Pos_Data->Month,
            p_GN_SUPL_Pos_Data->Day,
            p_GN_SUPL_Pos_Data->Hours,
            p_GN_SUPL_Pos_Data->Minutes,
            p_GN_SUPL_Pos_Data->Seconds,
            p_GN_SUPL_Pos_Data->Milliseconds );

         GN_SUPL_Log( "GN_SUPL_Position_Resp_Out: Latitude             %10.6f degrees",
                              p_GN_SUPL_Pos_Data->Latitude );

         GN_SUPL_Log( "GN_SUPL_Position_Resp_Out: Longitude           %11.6f degrees",
                              p_GN_SUPL_Pos_Data->Longitude );

         if ( p_GN_SUPL_Pos_Data->H_Acc_Valid )          // Horizontal Uncertainty fields valid: H_AccMaj, H_AccMin and H_AccMajBrg.
         {
            GN_SUPL_Log( "GN_SUPL_Position_Resp_Out: H_AccMaj (AccEst)    %10.6f metres",
                                 p_GN_SUPL_Pos_Data->H_AccMaj );
            GN_SUPL_Log( "GN_SUPL_Position_Resp_Out: H_AccMin (AccEst)    %10.6f metres",
                                 p_GN_SUPL_Pos_Data->H_AccMin );
            GN_SUPL_Log( "GN_SUPL_Position_Resp_Out: H_AccMajBrg         %11.6f degrees",
                                 p_GN_SUPL_Pos_Data->H_AccMajBrg );
         }
         else
         {
            GN_SUPL_Log( "GN_SUPL_Position_Resp_Out: Horizontal Uncertainty fields not valid: H_AccMaj, H_AccMin and H_AccMajBrg." );
         }

         if ( p_GN_SUPL_Pos_Data->AltitudeInfo_Valid )   // Altitude fields valid: Altitude and V_AccEst.
         {
            GN_SUPL_Log( "GN_SUPL_Position_Resp_Out: Altitude             %10.6f metres",
                                 p_GN_SUPL_Pos_Data->Altitude );
            GN_SUPL_Log( "GN_SUPL_Position_Resp_Out: Altitude (AccEst)    %10.6f metres",
                                 p_GN_SUPL_Pos_Data->V_AccEst );
         }
         else
         {
            GN_SUPL_Log( "GN_SUPL_Position_Resp_Out: Altitude fields not valid: Altitude and V_AccEst." );
         }

         if ( p_GN_SUPL_Pos_Data->HorizontalVel_Valid )    // Horizontal Velocity fields valid: HorizontalVel and Bearing.
         {
            GN_SUPL_Log( "GN_SUPL_Position_Resp_Out: HorizontalVel        %10.6f metres/second",
                                 p_GN_SUPL_Pos_Data->HorizontalVel );
            GN_SUPL_Log( "GN_SUPL_Position_Resp_Out: Bearing             %11.6f degrees",
                                 p_GN_SUPL_Pos_Data->Bearing );
         }
         else
         {
            GN_SUPL_Log( "GN_SUPL_Position_Resp_Out: Horizontal Velocity fields not valid: HorizontalVel and Bearing." );
         }

         if ( p_GN_SUPL_Pos_Data->VerticalVel_Valid )      // Vertical Velocity field valid: VerticalVel.
         {
            GN_SUPL_Log( "GN_SUPL_Position_Resp_Out: VerticalVel          %10.6f metres/second",
                                 p_GN_SUPL_Pos_Data->VerticalVel );
         }
         else
         {
            GN_SUPL_Log( "GN_SUPL_Position_Resp_Out: Vertical Velocity field not valid: VerticalVel" );
         }

         if ( p_GN_SUPL_Pos_Data->HVel_AccEst_Valid )      // Horizontal Velocity uncertainty field valid: HVel_AccEst.
         {
            GN_SUPL_Log( "GN_SUPL_Position_Resp_Out: HVel_AccEst (AccEst) %10.6f metres/second",
                                 p_GN_SUPL_Pos_Data->HVel_AccEst );
         }
         else
         {
            GN_SUPL_Log( "GN_SUPL_Position_Resp_Out: Horizontal Velocity uncertainty field not valid: HVel_AccEst." );
         }
         if ( p_GN_SUPL_Pos_Data->VVel_AccEst_Valid )      // Vertical Velocity uncertainty field valid: VVel_AccEst.
         {
            GN_SUPL_Log( "GN_SUPL_Position_Resp_Out: VVel_AccEst (AccEst) %10.6f metres/second",
                                 p_GN_SUPL_Pos_Data->VVel_AccEst );
         }
         else
         {
            GN_SUPL_Log( "GN_SUPL_Position_Resp_Out: Vertical Velocity uncertainty field not valid: VVel_AccEst." );
         }

         if ( p_GN_SUPL_Pos_Data->Confidence_Valid )      // Confidence field valid: Confidence.
         {
            GN_SUPL_Log( "GN_SUPL_Position_Resp_Out: Confidence (Percent) %3u",
                                 p_GN_SUPL_Pos_Data->Confidence );
         }
         else
         {
            GN_SUPL_Log( "GN_SUPL_Position_Resp_Out: Confidence field not valid: Confidence." );
         }

         GN_SUPL_Log( "******************************************************************************************" );
      }
   #endif
   Status = GN_SUPL_Position_Resp_Out( Handle, p_Status, p_GN_SUPL_Pos_Data );
   if ( ! Status )
   {
      StatusDesc = GN_SUPL_StatusDesc_From_Status( *p_Status );
      GN_SUPL_Log( "GN_SUPL_Position_Resp_Out: Status Returned: GN_Status %x %s", *p_Status, StatusDesc );
   }
   GN_SUPL_Log( "-GN_SUPL_Position_Resp_Out:" );
   return Status;
}

//*****************************************************************************
/// \brief
///      Third Party Position response out.
/// \details
///      Provides the Position calculated by the SUPL subsystem to the host.
///      <p> The Handle is set to the Handle from the
///      #GN_SUPL_Push_Delivery_In() or a #GN_SUPL_Position_Req_In().
///      <H3>p_Status values returned:</H3>
///      <ul>
///         <li><var>#GN_SUCCESS \copydoc GN_SUCCESS</var>\n
///            p_Status should be set to this value when the return value
///            indicated success.</li>
///         <li><var>#GN_ERR_HANDLE_INVALID \copydoc GN_ERR_HANDLE_INVALID</var>\n
///            p_Status should be set to this value when the Handle
///            passed was not recognised by the host software.</li>
///         <li><var>#GN_ERR_POINTER_INVALID \copydoc GN_ERR_POINTER_INVALID</var>\n
///            p_Status should be set to this value when the p_GN_AGPS_GAD_Data
///            is NULL.</li>
///         <li><var>#GN_ERR_NO_RESOURCE \copydoc GN_ERR_NO_RESOURCE</var>\n
///            p_Status should be set to this value when there are not
///            enough resources on the host to perform the requested function.</li>
///      </ul>
/// \note
///      This function must be implemented by the Host software and will be
///      called by functions from within the GloNav Library.
/// \attention
///      The caller of #GN_SUPL_Position_Resp_Out() must manage (e.g. alloc and free)
///      the memory referenced by p_Status and p_GN_AGPS_GAD_Data.
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval #TRUE Flag indicating success, *p_Status will be set to GN_SUCCESS.
/// \retval #FALSE Flag indicating failure and that the status should be checked.
BL GN_SUPL_ThirdPartyPosition_Resp_Out_Wrapper(
   void*          Handle,     ///< [in] Opaque Handle used to coordinate requests.
   e_GN_Status*   p_Status,   ///< [out] Status of disconnect and to be checked when return flag indicates failure.
   s_GN_SUPL_Pos_Data*
      p_GN_SUPL_Pos_Data      ///< [in] Data structure populated containing the position solution.
)
{
   BL Status;
   CH *StatusDesc;

   GN_SUPL_Log( "+GN_SUPL_ThirdPartyPosition_Resp_Out:" );
   GN_SUPL_Log( "GN_SUPL_api_call:    SUPL=>Host [label=\"GN_SUPL_ThirdPartyPosition_Resp_Out: Handle=%p\"];", Handle );
   #ifdef SUPL_API_LOGGING
      if ( *p_Status == GN_WRN_NO_POSITION )
      {
         GN_SUPL_Log( "GN_SUPL_ThirdPartyPosition_Resp_Out: No position reported." );
      }
      else if ( *p_Status == GN_SUCCESS )
      {
         GN_SUPL_Log( "******************************************************************************************" );

         GN_SUPL_Log( "GN_SUPL_ThirdPartyPosition_Resp_Out: Timestamp: %4u/%02u/%02u %02u:%02u:%02u.%03u",
            p_GN_SUPL_Pos_Data->Year,
            p_GN_SUPL_Pos_Data->Month,
            p_GN_SUPL_Pos_Data->Day,
            p_GN_SUPL_Pos_Data->Hours,
            p_GN_SUPL_Pos_Data->Minutes,
            p_GN_SUPL_Pos_Data->Seconds,
            p_GN_SUPL_Pos_Data->Milliseconds );

         GN_SUPL_Log( "GN_SUPL_ThirdPartyPosition_Resp_Out: Latitude             %10.6f degrees",
                              p_GN_SUPL_Pos_Data->Latitude );

         GN_SUPL_Log( "GN_SUPL_ThirdPartyPosition_Resp_Out: Longitude           %11.6f degrees",
                              p_GN_SUPL_Pos_Data->Longitude );

         if ( p_GN_SUPL_Pos_Data->H_Acc_Valid )          // Horizontal Uncertainty fields valid: H_AccMaj, H_AccMin and H_AccMajBrg.
         {
            GN_SUPL_Log( "GN_SUPL_ThirdPartyPosition_Resp_Out: H_AccMaj (AccEst)    %10.6f metres",
                                 p_GN_SUPL_Pos_Data->H_AccMaj );
            GN_SUPL_Log( "GN_SUPL_ThirdPartyPosition_Resp_Out: H_AccMin (AccEst)    %10.6f metres",
                                 p_GN_SUPL_Pos_Data->H_AccMin );
            GN_SUPL_Log( "GN_SUPL_ThirdPartyPosition_Resp_Out: H_AccMajBrg         %11.6f degrees",
                                 p_GN_SUPL_Pos_Data->H_AccMajBrg );
         }
         else
         {
            GN_SUPL_Log( "GN_SUPL_ThirdPartyPosition_Resp_Out: Horizontal Uncertainty fields not valid: H_AccMaj, H_AccMin and H_AccMajBrg." );
         }

         if ( p_GN_SUPL_Pos_Data->AltitudeInfo_Valid )   // Altitude fields valid: Altitude and V_AccEst.
         {
            GN_SUPL_Log( "GN_SUPL_ThirdPartyPosition_Resp_Out: Altitude             %10.6f metres",
                                 p_GN_SUPL_Pos_Data->Altitude );
            GN_SUPL_Log( "GN_SUPL_ThirdPartyPosition_Resp_Out: Altitude (AccEst)    %10.6f metres",
                                 p_GN_SUPL_Pos_Data->V_AccEst );
         }
         else
         {
            GN_SUPL_Log( "GN_SUPL_ThirdPartyPosition_Resp_Out: Altitude fields not valid: Altitude and V_AccEst." );
         }

         if ( p_GN_SUPL_Pos_Data->HorizontalVel_Valid )    // Horizontal Velocity fields valid: HorizontalVel and Bearing.
         {
            GN_SUPL_Log( "GN_SUPL_ThirdPartyPosition_Resp_Out: HorizontalVel        %10.6f metres/second",
                                 p_GN_SUPL_Pos_Data->HorizontalVel );
            GN_SUPL_Log( "GN_SUPL_ThirdPartyPosition_Resp_Out: Bearing             %11.6f degrees",
                                 p_GN_SUPL_Pos_Data->Bearing );
         }
         else
         {
            GN_SUPL_Log( "GN_SUPL_ThirdPartyPosition_Resp_Out: Horizontal Velocity fields not valid: HorizontalVel and Bearing." );
         }

         if ( p_GN_SUPL_Pos_Data->VerticalVel_Valid )      // Vertical Velocity field valid: VerticalVel.
         {
            GN_SUPL_Log( "GN_SUPL_ThirdPartyPosition_Resp_Out: VerticalVel          %10.6f metres/second",
                                 p_GN_SUPL_Pos_Data->VerticalVel );
         }
         else
         {
            GN_SUPL_Log( "GN_SUPL_ThirdPartyPosition_Resp_Out: Vertical Velocity field not valid: VerticalVel" );
         }

         if ( p_GN_SUPL_Pos_Data->HVel_AccEst_Valid )      // Horizontal Velocity uncertainty field valid: HVel_AccEst.
         {
            GN_SUPL_Log( "GN_SUPL_ThirdPartyPosition_Resp_Out: HVel_AccEst (AccEst) %10.6f metres/second",
                                 p_GN_SUPL_Pos_Data->HVel_AccEst );
         }
         else
         {
            GN_SUPL_Log( "GN_SUPL_ThirdPartyPosition_Resp_Out: Horizontal Velocity uncertainty field not valid: HVel_AccEst." );
         }
         if ( p_GN_SUPL_Pos_Data->VVel_AccEst_Valid )      // Vertical Velocity uncertainty field valid: VVel_AccEst.
         {
            GN_SUPL_Log( "GN_SUPL_ThirdPartyPosition_Resp_Out: VVel_AccEst (AccEst) %10.6f metres/second",
                                 p_GN_SUPL_Pos_Data->VVel_AccEst );
         }
         else
         {
            GN_SUPL_Log( "GN_SUPL_ThirdPartyPosition_Resp_Out: Vertical Velocity uncertainty field not valid: VVel_AccEst." );
         }

         if ( p_GN_SUPL_Pos_Data->Confidence_Valid )      // Confidence field valid: Confidence.
         {
            GN_SUPL_Log( "GN_SUPL_ThirdPartyPosition_Resp_Out: Confidence (Percent) %3u",
                                 p_GN_SUPL_Pos_Data->Confidence );
         }
         else
         {
            GN_SUPL_Log( "GN_SUPL_ThirdPartyPosition_Resp_Out: Confidence field not valid: Confidence." );
         }

         GN_SUPL_Log( "******************************************************************************************" );
      }
   #endif
   Status = GN_SUPL_ThirdPartyPosition_Resp_Out( Handle, p_Status, p_GN_SUPL_Pos_Data );
   if ( ! Status )
   {
      StatusDesc = GN_SUPL_StatusDesc_From_Status( *p_Status );
      GN_SUPL_Log( "GN_SUPL_ThirdPartyPosition_Resp_Out: Status Returned: GN_Status %x %s", *p_Status, StatusDesc );
   }
   GN_SUPL_Log( "-GN_SUPL_ThirdPartyPosition_Resp_Out:" );
   return Status;
}
/* +LMSqc38060 */


/// \brief
///      GN_SUPL_Abort - Aborting the supl session as part of GPS stop request
/// \details
/// \note
/// \attention
/// \returns
///      Flag to indicate success or failure of the posting of the request.

void GN_SUPL_Abort(void* Handle )
{
    s_SUPL_Instance   *p_SUPL_Instance;

    GN_SUPL_Log( "+GN_SUPL_Abort:" );

    p_SUPL_Instance = SUPL_Instance_From_Handle(Handle);

    if( NULL == p_SUPL_Instance )
    {
       GN_SUPL_Log("GN_SUPL_Abort: Invalid Handle value :  %p" , Handle );
       return;
    }
    else
    {
        SUPL_Send_Abort_Request(
            NULL,
            NULL,
            Handle
            );
    }
    GN_SUPL_Log( "-GN_SUPL_Abort:" );
}
/* +LMSqc38060 */


/// \brief
///      GN_SUPL_Trigger_End_Ind_In - End the Trieggered supl session when the stop time is reached or when the maximum
///     number of reports has been reached
/// \details
/// \note
/// \attention
/// \returns
///      Flag to indicate success or failure of the posting of the request.

void GN_SUPL_Trigger_End_Ind_In(void* Handle )
{
    s_SUPL_Instance   *p_SUPL_Instance;

    GN_SUPL_Log( "+GN_SUPL_Trigger_End_Ind_In:" );

    p_SUPL_Instance = SUPL_Instance_From_Handle(Handle);

    if( NULL == p_SUPL_Instance )
    {
       GN_SUPL_Log("GN_SUPL_Trigger_End_Ind_In: Invalid Handle value :  %p" , Handle );
       return;
    }
    else
    {
        SUPL_Send_Trigger_End_Request(
            NULL,
            NULL,
            Handle
            );
    }
    GN_SUPL_Log( "-GN_SUPL_Trigger_End_Ind_In:" );
}




/// \brief
///      GN_SUPL_Trigger_Ind_In
/// \details
///      This function is used to indicate to SUPL library that a trigger has occured for a particular session.
///      Following this indication, SUPL library will start a positioning session with the SUPL server to report
///      either the event or the position based on the request from the server.
/// \note
/// \attention
/// \returns
///      Flag to indicate success or failure of the posting of the request.

BL GN_SUPL_Trigger_Ind_In(
   void*          Handle,       ///< [in] Opaque Handle used to coordinate requests.
   e_GN_Status*   p_Status      ///< [out] Status of notification and to be checked when return flag indicates failure.
)
{
   s_SUPL_Instance   *p_SUPL_Instance;

   GN_SUPL_Log( "+GN_SUPL_Trigger_Ind_In:" );

   p_SUPL_Instance = SUPL_Instance_From_Handle(Handle);

   if( NULL == p_SUPL_Instance )
   {
      GN_SUPL_Log("GN_SUPL_Trigger_Ind_In: Invalid Handle value :  %p" , Handle );\
      *p_Status = GN_ERR_HANDLE_INVALID;
      return FALSE;
   }
   else
   {
      SUPL_Send_Trigger(
            p_SUPL_Instance,
            p_SUPL_Instance,
            Handle
            );
   }

   GN_SUPL_Log( "-GN_SUPL_Trigger_Ind_In:" );

   *p_Status = GN_SUCCESS;
   return TRUE;
}




/// \brief
///      GN_SUPL_ApplicationID_Info_In
/// \details
///      This function is used to populate Application Id Info on to the SUPL instance for Set Initiated SUPL session.
/// \note
/// \attention
/// \returns
///      Flag to indicate success or failure of the posting of the request.

BL GN_SUPL_ApplicationID_Info_In(
    void*           Handle,       ///< [in] Opaque Handle used to coordinate requests.
    s_GN_SUPL_V2_ApplicationInfo  *p_SUPL_ApplicationIDInfo,       ///< [in] Pointer to Application ID info.
    e_GN_Status*   p_Status   ///< [out] Status of PDU delivery and to be checked when return flag indicates failure.

)
{
  BL RetVal=TRUE;
  s_SUPL_Instance     *p_SUPL_Instance;

  GN_SUPL_Log( "+GN_SUPL_ApplicationID_Info_In" );
  p_SUPL_Instance = SUPL_Instance_From_Handle(Handle);

   if( NULL == p_SUPL_Instance )
  {
      GN_SUPL_Log("GN_SUPL_ApplicationID_Info_In: Invalid Handle value :  %p" , Handle );\
     *p_Status = GN_ERR_HANDLE_INVALID;
      //      return FALSE;
           RetVal = FALSE;
  }
   else
   {
       SUPL_Send_ApplicationId_Info(
           p_SUPL_Instance,
           p_SUPL_Instance,
           Handle,
           p_SUPL_ApplicationIDInfo
           );
       
       *p_Status = GN_SUCCESS;
   }

   GN_SUPL_Log( "-GN_SUPL_ApplicationID_Info_In:" );

  *p_Status = GN_SUCCESS;


  return RetVal;
}


/// \brief
///      GN_SUPL_Start_ThirdParty_Location_Transfer
/// \details
///      This function is used to populate Third Party Info on to the SUPL instance for Set Initiated SUPL session.
/// \note
/// \attention
/// \returns
///      Flag to indicate success or failure of the posting of the request.

BL GN_SUPL_Start_ThirdParty_Location_Transfer(
    void*           Handle,       ///< [in] Opaque Handle used to coordinate requests.
    s_GN_SUPL_V2_ThirdPartyInfo *p_Third_Party_Info,       ///< [in] Pointer to Third Party Info.
    e_GN_Status*   p_Status   ///< [out] Status of PDU delivery and to be checked when return flag indicates failure.

)
{
  BL RetVal=TRUE;
  s_SUPL_Instance     *p_SUPL_Instance;

  GN_SUPL_Log( "+GN_SUPL_Start_ThirdParty_Location_Transfer" );
  p_SUPL_Instance = SUPL_Instance_From_Handle(Handle);


   if( NULL == p_SUPL_Instance )
  {
      GN_SUPL_Log("GN_SUPL_Start_ThirdParty_Location_Transfer: Invalid Handle value :  %p" , Handle );\
     *p_Status = GN_ERR_HANDLE_INVALID;
//      return FALSE;
     RetVal = FALSE;
  }
   else
   {

      GN_SUPL_Log("GN_SUPL_Start_ThirdParty_Location_Transfer: Thid party Name  %s" , p_Third_Party_Info->thirdPartyIdName );

       SUPL_Send_ThirdParty_Info(
           p_SUPL_Instance,
           p_SUPL_Instance,
           Handle,
           p_Third_Party_Info
           );
       *p_Status = GN_SUCCESS;
   }

   GN_SUPL_Log( "-GN_SUPL_Start_ThirdParty_Location_Transfer:" );




  return RetVal;
}
/// \brief
///      GN_SUPL_Start_ThirdPartyPosition_Req_In
/// \details
///      This function is used to populate the targer SET's Info on to the SUPL instance for Set Initiated SUPL session.
/// \note
/// \attention
/// \returns
///      Flag to indicate success or failure of the posting of the request.

BL GN_SUPL_ThirdPartyPosition_Req_In(
    void**         p_NewHandle,             ///< [out] Opaque Handle Filled in by SUPL.
    e_GN_Status*   p_Status,                ///< [out] Status of notification and to be checked when return flag indicates failure.
    s_GN_SUPL_QoP* p_SUPL_QoP,              ///< [in] QoP criteria. NULL pointer if no QoP criteria set.
    s_GN_SUPL_V2_ThirdPartyInfo  *tp_info   ///<[in]. Target SET i.e third party's info
)
{
    /* @todo : Internal SUPL functionality is accessed here from external call flow. Should be corrected */
       BL Status = TRUE;
       s_SUPL_Instance *p_SUPL_Instance;
       s_SUPL_IpAddress *p_CFG_IpAddress;

       s_GN_SUPL_Connect_Req_Parms SUPL_Connect_Req_Parms;

       GN_SUPL_Log( "+GN_SUPL_ThirdPartyPosition_Req_In_1:" );
       GN_SUPL_Log( "GN_SUPL_api_call:    SUPL<=Host [label=\"GN_SUPL_ThirdPartyPosition_Req_In_1\"];" );

       *p_Status = GN_SUCCESS; // Set the GN_Status value to success unless an error occurs.

       p_SUPL_Instance = SUPL_Instance_Request_New();

       if ( p_SUPL_Instance != NULL )
       {
          *p_NewHandle = p_SUPL_Instance->Handle;
          p_SUPL_Instance->p_SUPL->MO_Request = TRUE;
          //We know that this SUPL instance is created only to retrieve the third party/targer SET Info. Hence mark it so that we can refer to it later
          p_SUPL_Instance->p_SUPL->V2_Data.ThirdParty_Retrieve = TRUE;
       }
       else
       {
          Status = FALSE;
          *p_Status = GN_ERR_NO_RESOURCE;
       }
       if ( Status )
       {
          if ( p_SUPL_QoP != NULL )
          {
             s_GN_QoP *p_QoP;

             p_SUPL_Instance->p_SUPL->p_GN_QoP = GN_Calloc( 1, sizeof( s_GN_QoP ) );

             p_QoP = p_SUPL_Instance->p_SUPL->p_GN_QoP;
             if ( p_QoP == NULL )
             {
                 Status = FALSE;
                 *p_Status = GN_ERR_NO_RESOURCE;
             }

             else
             {
                if ( p_SUPL_QoP->horacc <= 127 )
                {
                   // Horizontal accuracy       INTEGER (0..127).
                   // if QoP is present horacc is mandatory.
                   p_QoP->horacc = p_SUPL_QoP->horacc;
                   p_QoP->p_horacc = &p_QoP->horacc;
               #ifdef SUPL_API_LOGGING
                      GN_SUPL_Log( "GN_SUPL_ThirdPartyPosition_Req_In_1: QoP horacc ==  %u",
                                   p_QoP->horacc );
               #endif
                }
                else
                {
                   Status = FALSE;
                   *p_Status = GN_ERR_PARAMETER_INVALID;
                   GN_SUPL_Log( "GN_SUPL_ThirdPartyPosition_Req_In_1: QoP horacc ==  %u out of range 0..127",
                                p_SUPL_QoP->horacc );
                }


                // Vertical accuracy         INTEGER (0..127)   OPTIONAL (-1 = Not present).
                 if ( p_SUPL_QoP->veracc != -1 )
                {
                   if ( p_SUPL_QoP->veracc >= 0 /*&& p_SUPL_QoP->veracc <= 127 */ )
                   {
                      p_QoP->veracc = (U1) p_SUPL_QoP->veracc;
                      p_QoP->p_veracc = &p_QoP->veracc;
#ifdef SUPL_API_LOGGING
                      GN_SUPL_Log( "GN_SUPL_ThirdPartyPosition_Req_In_1: QoP veracc ==  %u",
                                   p_QoP->veracc );
#endif
                   }
                   else
                   {
                      GN_SUPL_Log( "GN_SUPL_ThirdPartyPosition_Req_In_1: QoP veracc ==  %d out of range 0..127",
                                    p_SUPL_QoP->veracc );
                      Status = FALSE;
                      *p_Status = GN_ERR_PARAMETER_INVALID;
                   }
                }

                // Maximum age of location   INTEGER (0..65535) OPTIONAL (-1 = Not present).
                if ( p_SUPL_QoP->maxLocAge != -1 )
                {
                   if ( p_SUPL_QoP->maxLocAge >= 0 || p_SUPL_QoP->maxLocAge <= 65535 )
                   {
                       p_QoP->maxLocAge = (U2) p_SUPL_QoP->maxLocAge;
                       p_QoP->p_maxLocAge = &p_QoP->maxLocAge;
#ifdef SUPL_API_LOGGING
                       GN_SUPL_Log( "GN_SUPL_ThirdPartyPosition_Req_In_1: QoP maxLocAge ==  %u",
                                      p_QoP->maxLocAge );
#endif
                   }
                   else
                   {
                       GN_SUPL_Log( "GN_SUPL_ThirdPartyPosition_Req_In_1: QoP maxLocAge ==  %d out of range 0..65535",
                                   p_SUPL_QoP->maxLocAge );
                       Status = FALSE;
                      *p_Status = GN_ERR_PARAMETER_INVALID;
                   }
                }

                // Maximum permissible delay INTEGER (0..7)     OPTIONAL (-1 = Not present).
                if ( p_SUPL_QoP->delay != -1 )
                {
                   if ( p_SUPL_QoP->delay >= 0 && p_SUPL_QoP->delay <= 7 )
                   {
                      ///\todo Add code to cope with 0 for a delay meaning return immediately with last known position.
                      p_QoP->delay = (U1) p_SUPL_QoP->delay;
                      p_QoP->p_delay = &p_QoP->delay;
#ifdef SUPL_API_LOGGING
                         GN_SUPL_Log( "GN_SUPL_ThirdPartyPosition_Req_In_1: QoP delay ==  %u",
                                      p_QoP->delay );
#endif
                   }
                   else
                   {
                      GN_SUPL_Log( "GN_SUPL_ThirdPartyPosition_Req_In_1: QoP delay ==  %d out of range 0..7",
                                   p_SUPL_QoP->delay );
                      Status = FALSE;
                      *p_Status = GN_ERR_PARAMETER_INVALID;
                   }
                }
             }
          }
          if (Status)
          {
             p_CFG_IpAddress = supl_config_get_SUPL_SLP_address();
             SUPL_Connect_Req_Parms.TcpIp_AddressType = p_CFG_IpAddress->TcpIp_AddressType;
             SUPL_Connect_Req_Parms.p_TcpIp_Address = p_CFG_IpAddress->Address;
             SUPL_Connect_Req_Parms.Port = p_CFG_IpAddress->Port;
         #ifdef SUPL_API_LOGGING
                if ( SUPL_Connect_Req_Parms.TcpIp_AddressType != IP_None )
                {
                   GN_SUPL_Log( "GN_SUPL_ThirdPartyPosition_Req_In_1: SLP IPAddress == %s, Port == %u",
                      SUPL_Connect_Req_Parms.p_TcpIp_Address,
                      SUPL_Connect_Req_Parms.Port );
                }
         #endif
    /*        @TODO : Side effect of removing this assignment. This code uses a snapshot of the configuration at the time of the request
              SUPL 2.0 requires that we always use updated configuration */
    /*
             p_SUPL_Instance->p_SUPL->SessionCfg.SUPL_PT_Supported.PT_agpsSETAssisted = supl_config_get_SUPL_agpsSETassisted();
             p_SUPL_Instance->p_SUPL->SessionCfg.SUPL_PT_Supported.PT_agpsSETBased = supl_config_get_SUPL_agpsSETBased();
             p_SUPL_Instance->p_SUPL->SessionCfg.SUPL_PT_Supported.PT_autonomousGPS = supl_config_get_SUPL_autonomousGPS();
             p_SUPL_Instance->p_SUPL->SessionCfg.SUPL_PT_Supported.PT_eCID = supl_config_get_SUPL_eCID(); */

             SUPL_Send_MO_Position_Requested(
                p_SUPL_Instance,
                p_SUPL_Instance,
                p_SUPL_Instance->Handle,
                &SUPL_Connect_Req_Parms );

             SUPL_Instance_State_Transition(  p_SUPL_Instance,
                                              state_SUPL_Comms_Open_Sent,
                                              "event_SUPL_MO_Position_Requested" );
          }
       }
        if(Status)
        {

            s_SUPL_Instance     *p_SUPL_Instance;
            p_SUPL_Instance = SUPL_Instance_From_Handle(*p_NewHandle);


            if( NULL == p_SUPL_Instance )
            {
                 GN_SUPL_Log("GN_SUPL_ThirdPartyPosition_Req_In_2: Invalid Handle value :  %p" , p_NewHandle );\
                 *p_Status = GN_ERR_HANDLE_INVALID;
                 Status = FALSE;
            }
            else
            {

                 GN_SUPL_Log("GN_SUPL_ThirdPartyPosition_Req_In_2: Target ID Name   %s" , tp_info->thirdPartyIdName );

                 SUPL_Send_TargetSet_Info(
                       p_SUPL_Instance,
                       p_SUPL_Instance,
                       p_NewHandle,
                       tp_info
                       );
                *p_Status = GN_SUCCESS;
            }

        }

       if ( Status )
       {
          GN_SUPL_Log( "GN_SUPL_api_call:    SUPL>>Host [label=\"GN_SUPL_Start_ThirdPartyPosition_Req_In: Handle=%p\"];", *p_NewHandle );
       }
       return Status;
}
/// \brief
///      GN_SUPL_TriggerPeriodic_Req_In
/// \details
///      This function is used to trigger periodic session for Set Initiated SUPL session.
/// \note
/// \attention
/// \returns
///      Flag to indicate success or failure of the posting of the request.

BL GN_SUPL_TriggerPeriodic_Req_In(
    void*           Handle,       ///< [in] Opaque Handle used to coordinate requests.
    s_GN_SUPL_V2_Triggers* p_Trigger,       ///< [in] Pointer to Trigger Events.
    e_GN_Status*   p_Status   ///< [out] Status of PDU delivery and to be checked when return flag indicates failure.

)
{
  BL RetVal=FALSE;
  s_SUPL_Instance     *p_SUPL_Instance;

  GN_SUPL_Log( "+GN_SUPL_TriggerPeriodic_Req_In" );
  GN_SUPL_Log( "+GN_SUPL_TriggerPeriodic_Req_In =%d", p_Trigger->triggeredMode);
  p_SUPL_Instance = SUPL_Instance_From_Handle(Handle);

   if( NULL == p_SUPL_Instance )
  {
      GN_SUPL_Log("GN_SUPL_Trigger_Ind_In: Invalid Handle value :  %p" , Handle );\
     *p_Status = GN_ERR_HANDLE_INVALID;
      return FALSE;
  }
   else
   {
       SUPL_Send_TriggerPeriodic_Request(
           p_SUPL_Instance,
           p_SUPL_Instance,
           Handle,
           p_Trigger
           );
   }

   GN_SUPL_Log( "-GN_SUPL_Trigger_Ind_In:" );

  *p_Status = GN_SUCCESS;


  return RetVal;
}



//*****************************************************************************
/// \brief
///      SUPL Check if Area event occured.
/// \details
///      Delivers position that needs to be checked for occurence of event  that is pre-defined by conditions
///      <p> The Handle is set to the Handle from the
///      #GN_SUPL_Connect_Req_Out() call.
///      <p> The p_GN_AGPS_GAD_Data is pointer to position computed of user for which area event occurence needs to be checked.
/// \note
///      This function must be implemented by the Host software and will be
///      called by functions from within the SUPL Library.
/// \returns
///      Flag to indicate success or failure of the occurence of event.
/// \retval #TRUE Flag indicating Area Event has occured
/// \retval #FALSE Flag indicatingArea Event has not occured and reporting is not required.
BL GN_SUPL_Check_AreaEvent_Wrapper
(
    void*          Handle,           ///< [in] Opaque Handle used to coordinate requests.
    s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data    ///< [in] GN A-GPS GAD (Geographical Area Description)
)
{
   BL Status;
   s_GN_SUPL_V2_Coordinate CurrentPos;

   CurrentPos.latitude     = p_GN_AGPS_GAD_Data->latitude;
   CurrentPos.latitudeSign = p_GN_AGPS_GAD_Data->latitudeSign;
   CurrentPos.longitude    = p_GN_AGPS_GAD_Data->longitude;

   GN_SUPL_Log( "+GN_SUPL_Check_AreaEvent:" );
   GN_SUPL_Log( "GN_SUPL_api_call:    SUPL=>Host [label=\"GN_SUPL_Check_AreaEvent : Handle=%p\"];", Handle );

   Status = GN_SUPL_Check_AreaEvent( Handle, &CurrentPos);

   GN_SUPL_Log( "GN_SUPL_Check_AreaEvent: Status Returned : %s", (Status)?"Event Occured":"Event Not Occured" );

   GN_SUPL_Log( "-GN_SUPL_Check_AreaEvent:" );

   return Status;
}

#ifdef __cplusplus
   }     // extern "C"
#endif
