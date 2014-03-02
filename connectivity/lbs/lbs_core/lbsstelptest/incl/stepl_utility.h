/*
 * Positioning Manager
 *
 * possupl_utility.h
 *
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

#ifndef INCLUSION_GUARD_STEPLSUPL_UTILITY_H
#define INCLUSION_GUARD_STEPLSUPL_UTILITY_H
/**************************************************************************
 *
 * $Copyright Ericsson AB 2009 $
 *
 **************************************************************************
 *
 *  Module : Positioning (SUPL)
 *
 *  Brief  : POSITIONING MANAGER SUPL Utility Functions
 *
 **************************************************************************
 *
 * $Workfile: steplsupl_utility.h $
 *
 **************************************************************************
*/

/*
 **************************************************************************
 * Includes
 **************************************************************************
*/

//standard files
#include <string.h>
#include <stdlib.h>
#include <wchar.h>

//basic system files
//#include "r_debug.h"



/*
 **************************************************************************
 * Defines and type declarations
 **************************************************************************
*/






typedef struct {

   /**
     * @brief Preferred NMEA Mask.
     */
    unsigned short NMEA;
    unsigned short speedThreshold;

    /**
     * @brief Vertical Accuracy expected in meters.
     */

    unsigned short accuracy_vertical_m;
   /**
     * @brief Horizontal Accuracy expected in meters.
     */

    unsigned short accuracy_horizontal_m;

    /**
      * @brief Preferred NMEA Mask.
     */

    /**
     * @brief Criteria for using historical locations
     */
    unsigned long age_limit_ms;

    /**
     * @brief Time to first fix (ignored by periodic sessions)
     */
    unsigned long time_to_first_fix_ms;

    /**
     * @brief Satelite status reporting rate (ignored by transfer to third party)
     */
    unsigned long status_rate_ms;

    /**
     * @brief Reporting rate (ignoned by singleshot sessions)
     */
    unsigned long reporting_rate;


    /** @brief Number of periodic reports
     */
    int no_count_periodic;

    /* For User Configuration */
    int configMaskValid;

    int configMask;

    int config2Valid;

    unsigned char sensMod;

    unsigned char sensTTFF;

    unsigned char powerPref;

    unsigned char coldStart;

    /* For Aiding data */
    unsigned int aidingDataType;

    unsigned int mt_testcases;
    unsigned char clientType; /*0 - No number, 1 - ClientName, 2 - MLC Number*/
    unsigned char clientTonNpi;
    char clientNumber[40];
    unsigned char serviceType;
    int IsFixAvailable;
    /** @brief Error status
     */
    int errorStatus;
    t_gpsClient_NavData NavData;
    t_gpsClient_NmeaData NmeaData;

    e_gpsClient_AreaEventType areaEventType;

    t_gpsClient_PositioningParams positioningParams;
    t_gpsClient_GeographicTargetArea targetArea;

    t_gpsClient_ApplicationIDInfo     application_id_info;

} LBSSTELP_reporting_criteria_t;

typedef struct{
    uint8_t v_Version[50];        /**<PlatformConfiguration file version*/
    uint16_t v_Config1MaskValid;   /** This field shall be set for all elements whose information is to be updated in v_Config1Mask */
    uint16_t v_Config1Mask;       /** The element bit in this field shall be set if the corresponding bit is set in v_Config1MaskValid*/
    uint16_t v_Config2Mask;    /**<This field shall based on which all optional element are updated.*/
    uint16_t v_MsaMandatoryMask;    /**<This field defines the Mandatory Assistance Data Mask for MSA.This field will
                                                            be set only if Mandatory MSA Assistance Data bit is set in v_Config2Mask.*/
    uint16_t v_MsbMandatoryMask;    /**<This field defines the Mandatory Assistance Data Mask for MSB.This field will
                                                            be set only if Mandatory MSB Assistance Data bit is set in v_Config2Mask.*/
    uint32_t v_SleepTimerDuration;  /**<This field shall set the Sleep Timer duration of GPS. This field will
                                                                be set only if GPS Power saving Int. bit is set in v_Config2Mask.*/
    uint32_t v_ComaTimerDuration;   /**<This field shall set the Coma Timer duration of GPS. This field will
                                                            be set only if GPS Power saving Int. bit is set in v_Config2Mask.*/
    uint32_t v_DeepComaDuration;    /**<This field shall set the DeepComa Timer duration of GPS. This field will
                                                                be set only if GPS Power saving Int. bit is set in v_Config2Mask.*/

}t_stelp_PlatformConfiguration;


typedef enum
{
   K_CGPS_CONFIG_MS_POS_TECH_MSASSISTED    = (1<<0),   /**< Mask for POS TECHNOLOGY-MS ASSISTED */
   K_CGPS_CONFIG_MS_POS_TECH_MSBASED       = (1<<1),   /**< Mask for POS TECHNOLOGY-MS BASED */
   K_CGPS_CONFIG_MS_POS_TECH_AUTONOMOUS    = (1<<2),   /**< Mask for POS TECHNOLOGY-AUTONOMOUS */
   K_CGPS_CONFIG_MS_POS_TECH_ECID          = (1<<3),   /**< Mask for POS TECHNOLOGY-Enhanced Cell Id */
   K_CGPS_CONFIG_AGPS_TRANSPORT_UPLANE     = (1<<4),   /**< Mask for User Plane Support */
   K_CGPS_CONFIG_AGPS_TRANSPORT_CPLANE     = (1<<5),   /**< Mask for Control Plane Support */
   K_CGPS_CONFIG_AGPS_TRANSPORT_PREF_UPLANE= (1<<6),   /**< Mask for Preference to User Plane over Control Plane */
   K_CGPS_CONFIG_AGPS_SESSION_MO_ALLOWED   = (1<<7),   /**< Mask for Mobile Originated Location Session */
   K_CGPS_CONFIG_AGPS_SESSION_MT_ALLOWED   = (1<<8),   /**< Mask for Mobile Terminated Location Session */
   K_CGPS_CONFIG_SUPL_MULTI_SESSION        = (1<<9),   /**< Mask for SUPL multi session */
/* ++ LMSqc00847    ANP 14/07/2009 */
/*This is provided to identify if Position reports need to be sent as soon as coarse position
is available irrespective of QoP. GPS position need to check for QoP*/
   K_CGPS_CONFIG_NBP_SUPPORTED             = (1<<10),  /**< Mask for Network Based Positioning Support. */
/* -- LMSqc00847    ANP 14/07/2009 */
   K_CGPS_CONFIG_TLS_SUPPORTED             = (1<<11),  /**< Mask for Supl Secured Connection. */
   K_CGPS_CONFIG_SUPLV2_SUPPORTED          = (1<<12),  /**< Mask for Supl Version 2.0 Support. */
   K_CGPS_CONFIG_AGPS_FTA             = (1<<13),  /**< Mask for Fine Time Aiding Support. */
   K_CGPS_CONFIG_FREQ_AIDING              = (1<<14),  /**< Mask for Frequency Aiding Support. */
   K_CGPS_CONFIG_SEMIAUTO               = (1<<15),  /**< Mask for Semi Auto Location Mode. */
} e_cgps_ConfigBitMask;



#endif /* INCLUSION_GUARD_POSSUPL_UTILITY_H */
