/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __CGPS_H__
#define __CGPS_H__

/**
* \file cgps.h
*
* <B>Description:</B> This is the main header file for CGPS that needs to be included\n
*
*/


/*! \defgroup   CGPS_AppliSession   Positioning.
 *  \brief      APIs related to start and stop GPS service.
 */
/*! \defgroup   CGPS_AppliAGPS  A-GNSS.
 *  \brief      Control Plane/ SUPL related.
 */
/*! \defgroup   CGPS_AppliConfig    Configuration.
 *  \brief      Configuration settings related.
 */
/*! \defgroup   CGPS_AppliGeneral  General.
 *  \brief      General / Others.
 */
/*! \defgroup	CGPS_AppliTest  Test.
 *	\brief	Test APIs.
 */



/*! \addtogroup CGPS_AppliSession
 *      \details
 *      These APIs are used for Start/Stop of GPS service
 *
 */
/*@{*/
    /*!\defgroup CGPS_AppliSession_Functions     APIs */
    /*!\defgroup CGPS_AppliSession_Structures   Structures*/
    /*!\defgroup CGPS_AppliSession_Constants    Constants*/
/*@}*/


/*! \addtogroup CGPS_AppliAGPS
 *      \details
 *      The Control Plane / SUPL Session related API
 *
 */
/*@{*/
    /*!\defgroup CGPS_AppliAGPS_Functions      APIs */
    /*!\defgroup CGPS_AppliAGPS_Structures    Structures*/
    /*!\defgroup CGPS_AppliAGPS_Constants     Constants*/
/*@}*/


/*! \addtogroup CGPS_AppliConfig
 *      \details
 *      The API related to configuration settings
 */
/*@{*/
    /*!\defgroup CGPS_AppliConfig_Functions       APIs */
    /*!\defgroup CGPS_AppliConfig_Structures     Structures */
    /*!\defgroup CGPS_AppliConfig_Constants      Constants*/
/*@}*/

/*! \addtogroup CGPS_AppliGeneral
 *      \details
 *      The APIs are general and valid for Autonomous / AGPS positioning sessions.
 */
/*@{*/
    /*!\defgroup CGPS_AppliGeneral_Functions         APIs */
    /*!\defgroup CGPS_AppliGeneral_Structures       Structures */
    /*!\defgroup CGPS_AppliGeneral_Constants        Constants*/
/*@}*/


/*! \addtogroup CGPS_AppliTest
 *      \details
 *      The APIs are for test only.
 */
/*@{*/
    /*!\defgroup CGPS_AppliTest_Functions         APIs */
/*@}*/



/*==========================================================================*/
/*==========================================================================*/
/*=                          PROVIDED INTERFACE                            =*/
/*==========================================================================*/
/*==========================================================================*/
#ifndef __RTK_E__
/* + LMSqc32518 */
#include "gnsInternal.h"
/* - LMSqc32518 */
#endif /*__RTK_E__ */
#include "cgps.hec"
#include "cgps.hem"
#include "cgps.het"
#include "cgps.hep"

/* + LMSqc32518 */
#ifdef AGPS_TEST_MODE_FTR
#include "cgpsTestMode.hem"
#include "cgpsTestMode.hec"
#include "cgpsTestMode.het"
#include "cgpsTestMode.hep"
#include "cgpsTestMode.hev"
#endif /*AGPS_TEST_MODE_FTR*/
/* - LMSqc32518 */
/**
*\}
*/
#endif  /* __CGPS_H__ */
