/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) 2009 ST-Ericsson.
 *  All rights reserved
 *  Date: 03-12-2009
 *  Author: Rahul Ranade
 *  Email : rahul.ranade@stericsson.com
 *****************************************************************************/
#ifndef GNS_SUPL_API_H
#define GNS_SUPL_API_H

#include "gns.h"
#include "gnsSUPL_Typedefs.h"
#include "gnsWireless_Typedef.h"

/*! \addtogroup Location_gnsSUPL_Functions */
/*@{*/

/* Callback which is executed to request for platform functionalities.  */
/*!
* \brief    Callback registered with \ref GNS_Initialize
* \details This callback is executed with messages and data which indicate 
   what type of Platform calls need to be executed.
* \param Type of Request \ref  e_gnsSUPL_MsgType
* \param Length of data  uint32_t                
* \param Data associated with Request \ref  u_gnsSUPL_MsgData    
*/
typedef void (*t_GnsSuplCallback)( e_gnsSUPL_MsgType , uint32_t , u_gnsSUPL_MsgData* );

/*!
* \brief  Initialises the SUPL functionality of GNS Module
* \details This API MUST be called before any of the SUPL functionality can be exercised by OemLbs
         
* \param v_Callback of type \ref t_GnsSuplCallback
* \return             A flag to indicate whether the Initialisation was successful
* \retval TRUE        Init Successful
* \retval FALSE     Init Failure
*/
bool GNS_SuplInitialize(t_GnsSuplCallback v_Callback );



void GNS_SuplTcpIpConnectCnf( t_GnsConnectionHandle vp_ConnectionHandle );

void GNS_SuplTcpIpConnectErr( t_GnsConnectionHandle vp_ConnectionHandle,int v_ErrorType );

void GNS_SuplTcpIpDisconnectCnf( t_GnsConnectionHandle vp_ConnectionHandle );

void GNS_SuplTcpIpDisconnectInd( t_GnsConnectionHandle vp_ConnectionHandle );

void GNS_SuplEstablishBearerCnf();

void GNS_SuplEstablishBearerErr();

void GNS_SuplCloseBearerInd();

void GNS_SuplCloseBearerCnf();

void GNS_SuplCloseBearerErr();


/*------------------------------------*/
/*!
* \brief  SUPL pdu sent to the network
* \details Indication to the GNS module that SUPL pdu has been sent to the network
* \param vp_ConnectionHandle  \ref t_GnsConnectionHandle
* \return             None
*/
void GNS_SuplSendDataCnf( t_GnsConnectionHandle vp_ConnectionHandle );

/*!
* \brief  Process SUPL pdu from Network
* \details 
* \param vp_ConnectionHandle
* \param pp_Pdu                Requested Aiding data from network
* \param vp_PduLen                   Requested Aiding data length
* \return             None
*/
void GNS_SuplReceiveDataInd( t_GnsConnectionHandle vp_ConnectionHandle , uint8_t* pp_Pdu , uint32_t vp_PduLen );

/*------------------------------------*/
/*!
* \brief  Deliver WAP PUSH message to GNS module
* \details Client calls this API to process the WAP PUSH recieved
* \param pp_Data        Message Data. This should be only the payload( SUPLINIT ) and must not contain any WAP headers
* \param vp_DataLen    Message Data Length
* \param pp_Hash        Hash of Data and Server Addr.
* \param vp_HashLen    Hash Length. Generally 8 bytes
* \return             A flag to indicate whether the function call was successful
* \retval TRUE        function call Successful
* \retval FALSE     function call fail
*/
void GNS_SuplSmsPushInd( uint8_t* pp_Data , uint32_t vp_DataLen , uint8_t* pp_Hash ,  uint32_t vp_HashLen );


/*------------------------------------*/
/*!
* \brief  Client informs Mobile Info Change to AGPS subsystem
* \details 
* \param pp_MobileInfo    structure of type \ref s_gnsSUPL_MobileInfo
* \return             A flag to indicate whether the function call was successful
* \retval TRUE        function call Successful
* \retval FALSE     function call fail
*/
void GNS_SuplMobileInfoInd(s_gnsSUPL_MobileInfo* pp_MobileInfo);



/*@}*/

#endif /* GNS_SUPL_API_H */

