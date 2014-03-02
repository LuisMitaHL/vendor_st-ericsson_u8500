/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) 2009 ST-Ericsson.
 *  All rights reserved
 *  Date: 03-12-2009
 *  Author: Rahul Ranade
 *  Email : rahul.ranade@stericsson.com
 *****************************************************************************/
/*!
 * \file     gns.h
 * \date     4/04/2009
 * \version  0.1
 *
 * <B>Compiler:</B> \n
 *
 * <B>Description:</B> AGPS Interface Definitions.\n
 */

/*! \defgroup   Location_gnsGeneral   General APIs.
 *  \brief      Apis common to SUPL / CP and autonomous mode
 */
/*! \defgroup   Location_gnsSUPL  SUPL (Secure User Plane Location) A-GPS Session API.
 *  \brief      Secure User Plane Location (SUPL) A-GPS Session API.
 */
/*! \defgroup   Location_gnsCP    CP (Control Plane) A-GPS Session API.
 *  \brief      Control Plane (CP) A-GPS Session API.
 */
 /*! \defgroup   Location_gnsEE    EE (Extended Ephemeris),Wi-Fi Reference Position Position, SNTP Reference.
 *  \brief      EE, Wi-Fi Reference Position,SNTP Reference Time API.
 */


/*! \addtogroup Location_gnsGeneral
 *      \details
 *      These APIs are used for Init/Deinit of GNS module
 *
 */
 /*@{*/
     /*!\defgroup Location_gnsGeneral_Functions     gnsGeneral_API */
     /*!\defgroup Location_gnsGeneral_Structures    gnsGeneral_Structures */
     /*!\defgroup Location_gnsGeneral_Constants     gnsGeneral_Constants*/
 /*@}*/


/*! \addtogroup Location_gnsCP
 *      \details
 *      The Control Plane Session API is used to deliver assistance data and position requests
 *      which originate over a UEs air interface.
 *
 *      The parameters and sequences supported by this API are oriented around Control Plane
 *      Location orientated Location Protocols.
 */
    /*@{*/
        /*!\defgroup Location_gnsCP_Functions       gnsCP_API */
        /*!\defgroup Location_gnsCP_Structures      gnsCP_Structures */
        /*!\defgroup Location_gnsCP_Constants       gnsCP_Constants*/
    /*@}*/


/*! \addtogroup Location_gnsSUPL
 *      \details
 *      The Secure User Plane Location (SUPL) API is used to initiate a User Plane connection
 *      to a SUPL server to allow a SUPL based positioning exchange.
 */
    /*@{*/
        /*!\defgroup Location_gnsSUPL_Functions         gnsSUPL_API */
        /*!\defgroup Location_gnsSUPL_Structures        gnsSUPL_Structures */
        /*!\defgroup Location_gnsSUPL_Constants         gnsSUPL_Constants*/
    /*@}*/
    
/*! \addtogroup Location_gnsEE
 *      \details
 *      These API are used to get assitance in the form of Extended Ephemeris,Wi-FI based Refrence Position and SNTP based Reference Time.
 */
    /*@{*/
        /*!\defgroup Location_gnsEE_Functions         gnsEE */
        /*!\defgroup Location_gnsEE_Structures        gnsEE_Structures */
        /*!\defgroup Location_gnsEE_Constants         gnsEE_Constants*/
    /*@}*/


#ifndef GNS_H
#define GNS_H

#ifdef __RTK_E__
#include "master.h"
#else
#include "agpsosa.h"
#endif

/*! \addtogroup Location_gnsGeneral_Constants */
/*@{*/

/*! \anchor Type_of_UNKNOWN
 *  \name   Type_of_UNKNOWN
 *  \brief  Predefined Invalid values for different data types.
 */
/*@{*/
/*Type of UNKNOWN*/
#define K_gnsUNKNOWN_S8   0x7F              /*!< Value for indicating a signed 8 bit value is not present. */
#define K_gnsUNKNOWN_U8   0xFF              /*!< Value for indicating an unsigned 8 bit value is not present. */
#define K_gnsUNKNOWN_S16  0x7FFF            /*!< Value for indicating a signed 16 bit value is not present. */
#define K_gnsUNKNOWN_U16  0xFFFF            /*!< Value for indicating an unsigned 16 bit value is not present. */
#define K_gnsUNKNOWN_S32  0x7FFFFFFF        /*!< Value for indicating a signed 32 bit value is not present. */
#define K_gnsUNKNOWN_U32  0xFFFFFFFF        /*!< Value for indicating an unsigned 32 bit value is not present. */
/*@}*/
/*!
 *  \enum   e_gns_RATType
 *  \brief  Type of RAT or Cell information.
 */

typedef enum{

    E_gns_RAT_TYPE_NONE = 0,   /*!< RAT Type is not defined*/                    
    E_gns_RAT_TYPE_GSM,        /*!< RAT Type is GSM*/                      
    E_gns_RAT_TYPE_WCDMA_FDD,  /*!< RAT Type is WCDMA - FDD*/                                               
    E_gns_RAT_TYPE_WCDMA_TDD   /*!< RAT Type is WCDMA- TDD*/ 

} e_gns_RATType;

/*@}*/
/*!
 *  \enum   e_gns_DelayType
 *  \brief  Type of Delay.
 */
typedef enum
{
    E_gns_LOW_DELAY ,    /*<!When set to this it implies that fulfillment of the response time requirement has precedence over fulfillment of the accuracy requirement*/
    E_gns_DELAY_TOLERANT /*<!When set to this it implies that fulfillment of the accuracy requirement takes precedence over fulfillment of the response time requirement*/
} e_gns_DelayType;     /*<!The delay field has 2 options according to 3GPP TS 29.002 version 10.5.0 Release 10 */

/*@}*/

/*! \addtogroup Location_gnsGeneral_Functions */
/*@{*/

/*------------------------------------*/
/*!
* \brief  Initialises the GNS Module
* \details This API MUST be called before any of the GPS functionality can be exercised by OemLbs
         
* \return             A flag to indicate whether the Initialisation was successful
* \retval TRUE        Init Successful
* \retval FALSE     Init Failure
*/
bool GNS_Initialize( void );

/*@}*/

#endif
