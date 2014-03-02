/*
 * lbs Configuration
 *
 * lbsconfig.h
 *
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */


/**************************************************************************
 *
 * $Copyright Ericsson AB 2009 $
 *
 **************************************************************************
 *
 *  Module : Configuration
 *
 *  Brief  : LBS Configuration Management function
 *
 **************************************************************************
 *
 * $Workfile: lbsconfig.h $
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

#include <agpsosa.h>


/**
* \struct t_lbscfg_User
* \brief Structure which provide the user configuration from file to LBS Subsystem
*/


typedef struct{
    int8_t  v_Version[50];        /**< User Configuration file version*/
    uint16_t v_ConfigMask;            /**< This element bit in this field shall be set if the corresponding bit is set in v_ValidityBitmap
                                                       1- Field is enabled, 0 - Field is disabled*/
    uint16_t v_Config2Mask;        /**< This field shall based on which all optional element are updated. */
    uint16_t v_SensMode;            /**< This field shall be used if corresponding bit in v_Config2Mask is set*/
    uint16_t v_SensTtff;            /**< This field shall be used if corresponding bit in v_Config2Mask is set*/
    uint16_t v_PowerPref;            /**< This field shall be used if corresponding bit in v_Config2Mask is set*/
    uint16_t v_ColdStart;            /**< This field shall be used if corresponding bit in v_Config2Mask is set*/
    int8_t  v_Slp_Address[50];    /**< Pointer to SLP address to be used for SUPL Sessions*/
    uint8_t  v_Slp_Address_Type;    /**< This information shall define the type of SLP address provided in Slp_Address*/
    int16_t v_Port_Num;            /**< Port number to be used for SUPL Sessions, 0XFFF sets the port number to default value*/
    int16_t v_Slp_AddrLen;        /**< Length of octet used to carry SLP address*/
    int8_t v_Apn[50];            /**< Pointer to Access Point Name string, NULL if APN has to be set to default value*/
    int8_t v_Login[50];            /**< Pointer to the Login string, NULL if unused*/
    int8_t v_Password[50];        /**< Pointer to the passowrd string, NULL if unused*/
    int8_t v_ESlp_Address[2560];        /**< Pointer to the passowrd string, NULL if unused*/


}t_lbscfg_User;



/**
* \struct t_lbscfg_Logging
* \brief Structure which provide the Logging configuration from file to LBS Subsystem
*/

typedef struct{
    int8_t v_Version[50];        /**< Logging Configuration file version*/
    uint8_t v_ChipLog1Type;        /**< GNB Message Type - Application,File,None*/
    uint16_t v_ChipLog1MaxFilesize;        /**< GNB Maximum File size*/
    uint8_t v_ChipLog1Opt;        /**< GNB Source Type -E-Error,W-Warnings,I-Information,D-Debug,F-Function,T-Timestamp  */
    uint8_t v_ChipLog2Type;        /**< NMEA Message Type - Application,File,None*/
    uint16_t v_ChipLog2MaxFilesize;        /**< NMEA Maximum File size*/
    uint8_t v_ChipLog2Opt;    /**< NMEA Source Type -E-Error,W-Warnings,I-Information,D-Debug,F-Function,T-Timestamp  */
    uint8_t v_ChipLog3Type;        /**< NAV Message Type - Application,File,None*/
    uint16_t v_ChipLog3MaxFilesize;        /**< NAV Maximum File size*/
    uint8_t v_ChipLog3Opt;        /**< NAV Source Type -E-Error,W-Warnings,I-Information,D-Debug,F-Function,T-Timestamp  */
    uint8_t v_ChipLog4Type;        /**< EVENT Message Type - Application,File,None*/
    uint16_t v_ChipLog4MaxFilesize;        /**< EVENT Maximum File size*/
    uint8_t v_ChipLog4Opt;    /**< EVENT Source Type -E-Error,W-Warnings,I-Information,D-Debug,F-Function,T-Timestamp  */
    uint8_t v_SuplType;        /**< SUPL Message Type - Application,File,None*/
    uint16_t v_SuplMaxFilesize;        /**< SUPL Maximum File size*/
    uint8_t v_SuplOpt;    /**< SUPL Source Type -E-Error,W-Warnings,I-Information,D-Debug,F-Function,T-Timestamp  */
    uint8_t v_CmccType;        /**< CMCC Message Type - Application,File,None*/
    uint16_t v_CmccMaxFilesize;        /**< CMCC Maximum File size*/
    uint8_t v_CmccOpt;    /**< CMCC Source Type -E-Error,W-Warnings,I-Information,D-Debug,F-Function,T-Timestamp  */
    uint8_t v_CgpsType;        /**< CGPS Message Type - Application,File,None*/
    uint16_t v_CgpsMaxFilesize;        /**< CGPS Maximum File size*/
    uint8_t v_CgpsOpt;    /**< CGPS Source Type -E-Error,W-Warnings,I-Information,D-Debug,F-Function,T-Timestamp  */
    uint8_t v_FsmType;        /**< FSM Message Type - Application,File,None*/
    uint16_t v_FsmMaxFilesize;        /**< FSM Maximum File size*/
    uint8_t v_FsmOpt;        /**< FSM Source Type -E-Error,W-Warnings,I-Information,D-Debug,F-Function,T-Timestamp  */

}t_lbscfg_Logging;

/**
* \struct t_lbscfg_Platform
* \brief Structure which provide the Platform configuration from Configurationfile to LBS Subsystem
*/


typedef struct{
    int8_t v_Version[50];        /**<PlatformConfiguration file version*/
    uint16_t v_Config1Mask;      /** The element bit in this field shall be set if the corresponding bit is set in v_Config1MaskValid*/
    uint16_t v_Config2Mask;    /**<This field shall based on which all optional element are updated*/
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

}t_lbscfg_Platform;



/**
* \struct t_lbscfg_Context
* \brief Structure which provide the user configuration from file to LBS Subsystem
*/
typedef struct{
    uint8_t v_LogFileReadStatus; /**<Log file read status - True if read*/
    uint8_t v_UserFileReadStatus; /**<User file read status - True if read*/
    uint8_t v_PlatformFileReadStatus; /**<Platform file read status - True if read*/
    t_lbscfg_Logging v_log;/**<Log structure - Valid only if #v_LogFileReadStatus is TRUE*/
    t_lbscfg_User v_user;/**<Log structure - Valid only if #v_UserFileReadStatus is TRUE*/
    t_lbscfg_Platform v_plt;/**<Log structure - Valid only if #v_PlatformFileReadStatus is TRUE*/
}t_lbscfg_Context;



/**
*\var vg_lbscfg
*\brief It is the configuration each NAF session would take on Single Shot / Periodice fix request.
*/
t_lbscfg_Context vg_lbscfg;


/*-SYScs46505*/
/**
* \def LBSCFG_USER_CONFIGURATION_FILE_NAME
*
* Define the User Configuration File Name
*/
#ifndef ARM_LINUX_AGPS_FTR
#define LBSCFG_USER_CONFIGURATION_FILE_NAME  "/etc/LbsConfig.cfg"
#else
#define LBSCFG_USER_CONFIGURATION_FILE_NAME  "/system/etc/LbsConfig.cfg"
#endif
/**
* \def LBSCFG_LOGGING_CONFIGURATION_FILE_NAME
*
* Define the Logging Configuration File Name
*/
#ifndef ARM_LINUX_AGPS_FTR
#define LBSCFG_LOGGING_CONFIGURATION_FILE_NAME  "/etc/LbsLogConfig.cfg"
#else
#define LBSCFG_LOGGING_CONFIGURATION_FILE_NAME  "/system/etc/LbsLogConfig.cfg"
#endif

/**
* \def LBSCFG_PLATFORM_CONFIGURATION_FILE_NAME
*
* Define the Platform Configuration File Name
*/
#ifndef ARM_LINUX_AGPS_FTR
#define LBSCFG_PLATFORM_CONFIGURATION_FILE_NAME  "/etc/LbsPltConfig.cfg"
#else
#define LBSCFG_PLATFORM_CONFIGURATION_FILE_NAME  "/system/etc/LbsPltConfig.cfg"
#endif



