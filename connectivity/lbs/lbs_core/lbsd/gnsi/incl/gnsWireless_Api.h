/*****************************************************************************
  *  ST-Ericsson GPS Technology
  *  Copyright (C) 2009 ST-Ericsson.
  *  All rights reserved
  *
  *****************************************************************************/
/*!
 * \file     gnsWirelessInfo_Api.h
 * \date     01/02/2012
 * \version  0.1
*/
#ifndef GNS_WIRELESS_NW_INFO_API_H
#define GNS_WIRELESS_NW_INFO_API_H

#include "gns.h"


/*@{*/


/* Callback which is executed to request for platform functionalities. */
/*!
* \brief»       Callback registered with \ref GNS_Initialize
* \details This callback is executed with messages and data which indicates WLAN information, Wimax id
* \param Wireless Network Information  \ref  e_gnsWIRELESS_MsgType - Tells Wireless NetowrkInfo type
* \param Length of data \ref  uint32_t
* \param Data associated with Request \ref     u_gnsWIRELESS_MsgData
*/
typedef void (*t_GnsWirelessCallback)(e_gnsWIRELESS_MsgType , uint32_t , u_gnsWIRELESS_MsgData* );

/*!
* \brief   Initialises the Wireless NetowrkInfo functionality of GNS Module
* \details

* \param  v_Callback of type \ref t_GnsWirelessCallback
* \return    A flag to indicate whether the Initialisation was successful
* \retval TRUE         Successful
* \retval FALSE         Failure
*/
bool GNS_WirelessInitialize(t_GnsWirelessCallback v_Callback );

/*------------------------------------*/
/*!
* \brief  Client informsWireless Access Point Info to AGPS subsystem
* \details 
* \param pp_WirelessAPInfo    structure of type \ref s_gnsWIRELESS_NetworkInfo
* \return             A flag to indicate whether the function call was successful
* \retval TRUE        function call Successful
* \retval FALSE     function call fail
*/
void GNS_WirelessNetworkInfoInd(s_gnsWIRELESS_NetworkInfo* pp_WirelessAPInfo);

#endif /* GNS_WIRELESS_NW_INFO_API_H */

