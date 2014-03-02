/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) 2009 ST-Ericsson.
 *  All rights reserved
 *  Date: 03-07-2012
 *  Author: Mohan Babu N
 *  Email : mohanbabu.n@stericsson.com
 *****************************************************************************/
/*!
 * \file     gnsEE.h
 * \date     03/07/2012
 * \version  0.1
*/

#ifndef GNS_EE_H
#define GNS_EE_H

#include "gns.h"
#include "gnsEE_Typedefs.h"

/*! \addtogroup Location_gnsEE_Functions */
/*@{*/


/**
* \brief This Callback function to request Extended Ephemeris
* \param pp_EERequest \ref  t_gnsEE_ExtendedEphInd
* \return     None
*/
typedef void (* t_GnsEEClientCallback)(t_gnsEE_ExtendedEphInd *pp_EERequest);

/**
* \brief This Callback function to request Reference Location
* \return     None
*/


typedef void (* t_GnsEERefLocationCallback)();


/**
* \brief This Callback function to request SNTP Reference Time
* \return     None
*/


typedef void (* t_GnsEERefTimeCallback)();

/**
* \brief This Callback function to request seed deletion.
* \return     None
*/

typedef void (* t_GnsEEDeleteSeedCallback)();


/**
* \brief This Callback function is to provide Broadcast Ephemeris to MSL
* \param pp_BCE \ref  t_gnsEE_NavDataBCE
* \return     None
*/

typedef void (* t_GnsEEBCEDataCallback)(t_gnsEE_NavDataBCE *pp_BCE);



/*!
* \brief  Registers CallBack function to request Extended Ephemeris
* \details Registers CallBack function to request Extended Ephemeris
* \param pp_NavDataList \ref s_gnsEE_NavDataList
* \return     None
*/

void GNS_EEGetEphemerisRsp(s_gnsEE_NavDataList* pp_NavDataList);

/*!
* \brief  Registers CallBack function to request Extended Ephemeris
* \details Registers CallBack function to request Extended Ephemeris
* \param pp_RefPosition \ref t_gnsEE_RefPosition
* \return     None
*/

void GNS_EEGetRefLocationRsp(t_gnsEE_RefPosition* pp_RefPosition);


/*!
* \brief  Registers CallBack function to Delete Seed
* \details Registers CallBack function to Delete Seed 
* \param v_Callback \ref t_GnsEEClientCallback
* \return     TRUE/FALSE
*/
bool GNS_DeleteSeedRegister( t_GnsEEDeleteSeedCallback   v_Callback );


/*!
* \brief  Registers CallBack function to request Extended Ephemeris
* \details Registers CallBack function to request Extended Ephemeris
* \param v_Callback \ref t_GnsEEClientCallback
* \return     TRUE/FALSE
*/
bool GNS_ExtendedEphemerisRegister( t_GnsEEClientCallback v_Callback );


/*!
* \brief  Registers CallBack function to request Reference Position
* \details Registers CallBack function to request Reference Position
* \param v_Callback \ref t_GnsEERefLocationCallback
* \return     TRUE/FALSE
*/
bool GNS_ReferencePositionRegister( t_GnsEERefLocationCallback   v_Callback );


/*!
* \brief  Registers CallBack function to request Reference Time
* \details Registers CallBack function to request Reference Time
* \param v_Callback \ref t_GnsEERefTimeCallback
* \return     TRUE/FALSE
*/

bool GNS_ReferenceTimeRegister( t_GnsEERefTimeCallback   v_Callback );

/*!
* \brief  Registers CallBack function to request BCE data
* \details Registers CallBack function to request BCE Data
* \param v_Callback \ref t_GnsEERefTimeCallback
* \return     TRUE/FALSE
*/
bool GNS_BCEDataRegister( t_GnsEEBCEDataCallback   v_Callback );


/*!
* \brief  Requests Extended Ephemeris
* \details Requests Extended Ephemeris
* \param pp_EERequest \ref t_gnsEE_ExtendedEphInd
* \return     None
*/
void GNS_EEGetEphemerisRequest(t_gnsEE_ExtendedEphInd *pp_EERequest);

/*!
* \brief  Requests Reference Position
* \details Requests Reference Position
* \return     None
*/

void GNS_EEGetRefLocationRequest();

/*!
* \brief  Requests Reference Time
* \details Requests Reference Time
* \return     None
*/

void GNS_EEGetRefTimeRequest();

/*!
* \brief  Requests Seed Deletion
* \details Requests Seed Deletion
* \return     None
*/

void GNS_EEDeleteSeedRequest();


/*!
* \brief  Provides Broadcast Ephemeris Data
* \details Provides Broadcast Ephemeris Data
* \param pp_BCE \ref t_gnsEE_NavDataBCE
* \return     None
*/
void GNS_EEFeedBCEInd(t_gnsEE_NavDataBCE *pp_BCE);


/*@}*/


#endif /*GNS_EE_H */
