/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) 2009 ST-Ericsson.
 *  All rights reserved
 *
 *****************************************************************************/

#ifndef GNS_FTA_API_H
#define GNS_FTA_API_H

#include "gns.h"
#include "gnsCP_Typedefs.h"
#include "gnsFTA_Typedefs.h"


/*! \addtogroup Location_gnsFTA_Functions */
/*@{*/

/* Callback which is executed to request for platform functionalities.  */
/*!
* \brief    Callback registered with \ref GNS_FtaInitialize
* \details This callback is executed with messages and data which indicate
   type of Platform calls need to be executed.
* \param Type of Request \ref  e_gnsFTA_MsgType
* \param Length of data  uint32_t
* \param Data associated with Request \ref  u_gnsFTA_MsgDataOut
*/

typedef void (*t_gnsFTA_Callback)( e_gnsFTA_MsgType , uint32_t , u_gnsFTA_MsgDataOut* );


/*------------------------------------*/
/*!
* \brief  Initialises the GNS Fine time assistance Module
* \details This API MUST be called before any of the Fine time assistance functionality can be exercised

* \param v_FtaCallback of type \ref t_gnsFTA_Callback
* \return             A flag to indicate whether the Initialisation was successful
* \retval TRUE        Initialize Successful
* \retval FALSE     Initialize Failure
*/

bool GNS_FtaInitialize( t_gnsFTA_Callback v_FtaCallback );


/*!
* \brief  Provide Time Assistance based on FTA
* \details if the input is NULL pointer, Indicate that FTA pulse has been generate but there is no valid Time Assistance
* \param p_PulseTime \ref s_gnsFTA_PulseCnf
* \return None
* \retval None
*/

void GNS_FtaPulseCnf( s_gnsFTA_PulseCnf *p_PulseTime );

/*@}*/



#endif /* GNS_FTA_API_H*/

