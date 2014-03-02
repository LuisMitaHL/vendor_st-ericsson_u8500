/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) 2009 ST-Ericsson.
 *  All rights reserved
 *
 *****************************************************************************/
/*!
 * \file     gnsCellInfo_Api.h
 * \date     06/01/2012
 * \version  0.1
*/
#ifndef GNS_CELL_INFO_API_H
#define GNS_CELL_INFO_API_H

/*@{*/

/*!
 *  \enum   e_gnsCellInfo_MsgType
 *      List of constant used to define type of message
 */
typedef enum
{
    E_gnsCellInfo_MOBILE_INFO_REQ  /*!< Mobile Info Updates Requested. Data defined by \ref e_gnsCellInfo_MsgType */
}e_gnsCellInfo_MsgType;

/*! \struct  s_gnsCellInfo_MobileInfoReq
 *  \brief  Structure which defines the data associated with \ref E_gnsSUPL_RECEIVE_DATA_CNF
 */
typedef struct
{
    uint32_t v_RequestedFields;    /* \ref  e_gnsSUPL_MobileInfoValidity*/
}s_gnsCellInfo_MobileInfoReq;

/*! \union  u_gnsCellInfo_MsgData
 *  \brief  Union which defines the data associated with \ref e_gnsCellInfo_MsgType
 */
typedef union
{
    s_gnsCellInfo_MobileInfoReq        v_GnsCellInfo_MobileInfoReq; /*!< Corresponds to message type #E_gnsCellInfo_MOBILE_INFO_REQ */
}u_gnsCellInfo_MsgData;

/* Callback which is executed to request for platform functionalities.  */
/*!
* \brief    Callback registered with \ref GNS_Initialize
* \details This callback is executed with messages and data which indicates cell information, Enhanced Cell id
* \param Cell Information  \ref  e_gnsCellInfo_MsgType - Tells CellInfo type
* \param Length of data \ref  uint32_t
* \param Data associated with Request \ref  u_GnsCellInfo_MsgData
*/
typedef void (*t_GnsCellInfoCallback)(e_gnsCellInfo_MsgType , uint32_t , u_gnsCellInfo_MsgData* );

/*!
* \brief   Initialises the CellInfo functionality of GNS Module
* \details
* \param  v_Callback of type \ref t_GnsCellInfoCallback
* \return             A flag to indicate whether the Initialisation was successful
* \retval TRUE        Successful
* \retval FALSE       Failure
*/
bool GNS_CellInfoInitialize(t_GnsCellInfoCallback v_Callback );

#endif /* GNS_CELL_INFO_API_H */
