/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __CGPSUTILS_HIC__
#define __CGPSUTILS_HIC__
/**
* \file cgpsutils.hic
* \date 07/08/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all constant used by cgps4utils.c.\n
* 
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 07.04.08 </TD><TD> M.BELOU </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

/** 
* \enum e_cgps_cp_old_session_mask
*
* define mask use to analyse the old session bit fiel in t_CGPSOperatorRules
*
*/
typedef enum 
{
    CGPS_OLD_SESSION_MASK_AUTONOMOUS_VS_AUTONOMOUS_RESTRICTION = 0x0001,
    CGPS_OLD_SESSION_MASK_AUTONOMOUS_VS_SUPL_RESTRICTION = 0x0002,
    CGPS_OLD_SESSION_MASK_AUTONOMOUS_VS_CPMO_RESTRICTION = 0x0004,
    CGPS_OLD_SESSION_MASK_AUTONOMOUS_VS_CPNI_RESTRICTION = 0x0008,
    CGPS_OLD_SESSION_MASK_SUPL_VS_AUTONOMOUS_RESTRICTION = 0x0010,
    CGPS_OLD_SESSION_MASK_SUPL_VS_SUPL_RESTRICTION = 0x0020,
    CGPS_OLD_SESSION_MASK_SUPL_VS_CPMO_RESTRICTION = 0x0040,
    CGPS_OLD_SESSION_MASK_SUPL_VS_CPNI_RESTRICTION = 0x0080,
    CGPS_OLD_SESSION_MASK_CPMO_VS_AUTONOMOUS_RESTRICTION = 0x0100,
    CGPS_OLD_SESSION_MASK_CPMO_VS_SUPL_RESTRICTION = 0x0200,
    CGPS_OLD_SESSION_MASK_CPMO_VS_CPMO_RESTRICTION = 0x0400,
    CGPS_OLD_SESSION_MASK_CPMO_VS_CPNI_RESTRICTION = 0x0800,
    CGPS_OLD_SESSION_MASK_CPNI_VS_AUTONOMOUS_RESTRICTION = 0x1000,
    CGPS_OLD_SESSION_MASK_CPNI_VS_SUPL_RESTRICTION = 0x2000,
    CGPS_OLD_SESSION_MASK_CPNI_VS_CPMO_RESTRICTION = 0x4000,
    CGPS_OLD_SESSION_MASK_CPNI_VS_CPNI_RESTRICTION = 0x8000,
} e_cgps_cp_old_session_mask;


/** 
* \enum e_cgps_cp_new_session_mask
*
* define mask use to analyse the old session bit fiel in t_CGPSOperatorRules
*
*/
typedef enum 
{
    CGPS_NEW_SESSION_MASK_AUTONOMOUS_VS_AUTONOMOUS_RESTRICTION = 0x0001,
    CGPS_NEW_SESSION_MASK_AUTONOMOUS_VS_SUPL_RESTRICTION = 0x0002,
    CGPS_NEW_SESSION_MASK_AUTONOMOUS_VS_CPMO_RESTRICTION = 0x0004,
    CGPS_NEW_SESSION_MASK_AUTONOMOUS_VS_CPNI_RESTRICTION = 0x0008,
    CGPS_NEW_SESSION_MASK_SUPL_VS_AUTONOMOUS_RESTRICTION = 0x0010,
    CGPS_NEW_SESSION_MASK_SUPL_VS_SUPL_RESTRICTION = 0x0020,
    CGPS_NEW_SESSION_MASK_SUPL_VS_CPMO_RESTRICTION = 0x0040,
    CGPS_NEW_SESSION_MASK_SUPL_VS_CPNI_RESTRICTION = 0x0080,
    CGPS_NEW_SESSION_MASK_CPMO_VS_AUTONOMOUS_RESTRICTION = 0x0100,
    CGPS_NEW_SESSION_MASK_CPMO_VS_SUPL_RESTRICTION = 0x0200,
    CGPS_NEW_SESSION_MASK_CPMO_VS_CPMO_RESTRICTION = 0x0400,
    CGPS_NEW_SESSION_MASK_CPMO_VS_CPNI_RESTRICTION = 0x0800,
    CGPS_NEW_SESSION_MASK_CPNI_VS_AUTONOMOUS_RESTRICTION = 0x1000,
    CGPS_NEW_SESSION_MASK_CPNI_VS_SUPL_RESTRICTION = 0x2000,
    CGPS_NEW_SESSION_MASK_CPNI_VS_CPMO_RESTRICTION = 0x4000,
    CGPS_NEW_SESSION_MASK_CPNI_VS_CPNI_RESTRICTION = 0x8000,
} e_cgps_cp_new_session_mask;

/** 
* \enum e_cgps_cp_application_id_bitfield
*
* define Application ID used in the operator rules table
*
*/
typedef enum
{
    CGPS_AUTONOMOUS_APPLICATION_ID = 0x01,
    CGPS_SUPL_APPLICATION_ID = 0x02,
    CGPS_CP_MO_APPLICATION_ID = 0x04,
    CGPS_CP_NI_APPLICATION_ID = 0x08,
    CGPS_EMERGENCY_APPLICATION_ID = 0x10,
    CGPS_CP_MAX_VALUE_APPLICATION_ID = 0x7FFFFFFF
}e_cgps_cp_application_id;

/**
* \enum  e_cgps_PlatformConfigBitMask
* this enum is used to check the Platform Config2Mask
*/
typedef enum
{
   K_CGPS_PLATFORMCONFIG_MANDATORY_MSA        = (1<<0),   /**< Mask for Mandatory MSA Assistance Data */
   K_CGPS_PLATFORMCONFIG_MANDATORY_MSB        = (1<<1),   /**< Mask for Mandatory MSB Assistance Data */
   K_CGPS_PLATFORMCONFIG_GPS_POWER_INTERVAL   = (1<<2),   /**<Mask for GPS Power Interval,which enables 
                                                           assignment of different timer values for Sleep,Coma 
                                                           and DeepComa Modes*/
} e_cgps_PlatformConfigBitMask;


/**
* \def CGPS_OPERATOR_RULE_TAB_SIZE
*
* Define the max number of operator in the rules table 
*/
#define CGPS_MAX_OPERATOR_IN_RULE_TAB_SIZE 1 /*number of operator in the operator rule table*/


/**
* \def CGPS_DEFAULT_OPERATOR_INDEX
*
* Define the default operator rules of the system
*/
#define CGPS_DEFAULT_OPERATOR_INDEX 0


/**
* \def CGPS_MAX_PRIORITY_VALUE
*
* Define the Max value for suspended priority value
*/
#define CGPS_MAX_PRIORITY_VALUE 0xFF


/**
* \def CGPS_MIN_PRIORITY_VALUE
*
* Define the Min value for suspended priority value
*/
#define CGPS_MIN_PRIORITY_VALUE 0x01


/**
* \def CGPS_CGPS_SESSION_NOT_SUSPENDED
*
* Define the Max value for suspended priority value
*/
#define CGPS_SESSION_NOT_SUSPENDED 0x00


/*+LMSqc38060_2*/
/**
* \def CGPS_SESSION_SUSPENDED
*
* Define the value for suspended state of session
*/
#define CGPS_SESSION_SUSPENDED 0x01
/*-LMSqc38060_2*/


/*+SYScs46505*/
/**
* \def CGPS_MIN_TIME_BETWEEN_SUPL
*
* Define Minimum time between two SUPL session, when assistance data is required in tracking mode
*/
#define K_CGPS_MIN_TIME_BETWEEN_SUPL                         300000 //(5 min)


/**
* \def CGPS_INVALID_TIMESTAMP
*
* Define Invalida Timestamp which can be used in all SUPL sessions
*/
#define K_CGPS_INVALID_TIMESTAMP                                 0


/**
* \def CGPS_ASSIST_TRACK_MAX_NUM_FAIL_ATTEMPT
*
* Define the Maximum Fail attemp for Assistance data in a cell
*/
#define K_CGPS_ASSIST_TRACK_MAX_NUM_FAIL_ATTEMPT               100  //Note this number shall be reduced when Cell is updated upon change.


/**
* \def CGPS_ASSIST_TRACK_PERIOD
*
* Define the Perdiod when Assistance data can be tracked
*/
#define K_CGPS_ASSIST_TRACK_PERIOD                            1000  //1   


/*-SYScs46505*/
/**
* \def CGPS_USER_CONFIGURATION_FILE_NAME
*
* Define the User Configuration File Name
*/
#ifndef ARM_LINUX_AGPS_FTR
#define CGPS_USER_CONFIGURATION_FILE_NAME  "/etc/LbsConfig.cfg"
#else
#define CGPS_USER_CONFIGURATION_FILE_NAME  "/system/etc/LbsConfig.cfg"
#endif
/**
* \def CGPS_LOGGING_CONFIGURATION_FILE_NAME
*
* Define the Logging Configuration File Name
*/
#ifndef ARM_LINUX_AGPS_FTR
#define CGPS_LOGGING_CONFIGURATION_FILE_NAME  "/etc/LbsLogConfig.cfg"
#else
#define CGPS_LOGGING_CONFIGURATION_FILE_NAME  "/system/etc/LbsLogConfig.cfg"
#endif
/**
* \def CGPS_PLATFORM_CONFIGURATION_FILE_NAME
*
* Define the Platform Configuration File Name
*/
#ifndef ARM_LINUX_AGPS_FTR
#define CGPS_PLATFORM_CONFIGURATION_FILE_NAME  "/etc/LbsPltConfig.cfg"
#else
#define CGPS_PLATFORM_CONFIGURATION_FILE_NAME  "/system/etc/LbsPltConfig.cfg"
#endif
/**
* \def CGPS_MAX_ESLP_ADDR_LENGTH
*
* Define the Maximum length of ESLP Address
*/
#define CGPS_MAX_ESLP_ADDR_LENGTH  256

/**
* \def CGPS_MAX_ESLP_ADDRESS
*
* Define the Maximum number of ESLP Addresses
*/
#define CGPS_MAX_ESLP_ADDRESS  10


/**
* \def K_CGPS_MARGIN_LONG_DEG
*
* Define the margin of longitude degrees under consideration to check if point lies on edge.
*/
#define K_CGPS_MARGIN_LONG_DEG         (.00006)


/**
* \def K_CGPS_MARGIN_LAT_DEG
*
* Define the margin of latitude degrees under consideration to check if point lies on edge.
*/
#define K_CGPS_MARGIN_LAT_DEG         (.00006)
/**
* \def K_CGPS_LAT_2_KM
*
* Define the conversion from latitude degrees to meters.
*/
#define K_CGPS_LAT_to_METERS         (111100)


/**
* \def K_CGPS_EARTH_RADIUS_METERS
*
* Define the approximate earth radius in meters
*/
#define K_CGPS_EARTH_RADIUS_METERS  6378000


/**
* \def CONFIG_VER
*
* Define the Version of config tool
*/


#define CONFIG_VER "v.1.2.10"

#define K_CGPS_PATCHUPLOAD_FS_PATH  "/etc/lbs/"

#define K_CGPS_MAX_FS_PATCH_RETRY   2
#define K_CGPS_MAX_CHARS_PER_LINE   260 //256 chars per line is the limit for a single line patch message

#define     CGPS_ASSIST_MASK_ALM_MODEL      0x0001
#define     CGPS_ASSIST_MASK_UTC_MODEL      0x0002
#define     CGPS_ASSIST_MASK_IONO_MODEL     0x0004
#define     CGPS_ASSIST_MASK_EPHEMERIS      0x0008
#define     CGPS_ASSIST_MASK_DGPS_CORR      0x0010
#define     CGPS_ASSIST_MASK_REF_POSITION   0x0020
#define     CGPS_ASSIST_MASK_REF_TIME       0x0040
#define     CGPS_ASSIST_MASK_ACQ_ASSIST     0x0080
#define     CGPS_ASSIST_MASK_REQ_RTI        0x0100



#endif /* __CGPSUTILS_HIC__ */
