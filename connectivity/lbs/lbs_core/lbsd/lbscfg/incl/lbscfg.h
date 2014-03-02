#ifndef _LBSCFG_H_
#define _LBSCFG_H_
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


/*
 **************************************************************************
 * Defines and type declarations
 **************************************************************************
*/

/**
* \enum e_lbscfg_ConfigType
* \brief Provides the configuration tyoe
*/


typedef enum
{
   K_LBSCFG_USERCONFIG_TYPE,  /**< User Configuration*/
   K_LBSCFG_PLATFORMCONFIG_TYPE,  /**< Platform Configuration*/
   K_LBSCFG_LOGGINGCONFIG_TYPE,  /**< Log Configuration*/
} e_lbscfg_ConfigType;


/**
* \enum e_lbscfg_LogSeverity
* \brief Num to provide module log
*/

typedef enum
{
   K_LBSCFG_LOG_LEV_ERR  = (1<<0),  /**<ERROR */
   K_LBSCFG_LOG_LEV_WRN  = (1<<1),  /**< WARNING*/
   K_LBSCFG_LOG_LEV_INF  = (1<<2),  /**< Information*/
   K_LBSCFG_LOG_LEV_DBG  = (1<<3),  /**< Debug*/
   K_LBSCFG_LOG_LEV_FNC  = (1<<4),  /**< Function*/
   K_LBSCFG_LOG_LEV_TME  = (1<<5)   /**< Time*/
} e_lbsconfig_LogSeverity;

/**
* \enum e_lbscfg_LogType
* \brief Type of Log
*/

typedef enum
{
   K_LBSCFG_LOG_TYPE_APPL,  /**< Logging into Application or Platform Trace*/
   K_LBSCFG_LOG_TYPE_FILE,   /**< Logging into File*/
   K_LBSCFG_LOG_TYPE_NONE,  /**<No Logging */
} e_lbsconfig_LogType;


/**
* \enum e_lbscfg_LogModuleType
* \brief Num to provide module log
*/

typedef enum
{
   K_LBSCFG_FSM,      /**< FSM Logging Configuration*/
   K_LBSCFG_CGPS,     /**< CGPS Logging Configuration*/
   K_LBSCFG_HALGPS,   /**< HALGPS Logging Configuration*/
   K_LBSCFG_CMCC,     /**< CMCC Logging Configuration*/
   K_LBSCFG_LSIM,     /**< LSIM Logging Configuration*/
   K_LBSCFG_SBEE,     /**< SBEE Logging Configuration*/
   K_LBSCFG_SUPL,     /**< SUPL Logging Configuration*/
   K_LBSCFG_CHIP_LOG1,/**< CHIP LOG 1 Logging Configuration*/
   K_LBSCFG_CHIP_LOG2,/**< CHIP LOG 2 Logging Configuration*/
   K_LBSCFG_CHIP_LOG3,/**< CHIP LOG 3 Logging Configuration*/
   K_LBSCFG_CHIP_LOG4,/**< CHIP LOG 4 Logging Configuration*/

   K_LBSCFG_MODULE_MAX   /**< This should be last in the enum*/
} e_lbscfg_LogModuleType;


/**
* \struct t_lbscfg_LogParam
* \brief Structure which provide the user configuration from file to LBS Subsystem
*/
typedef struct{
    uint8_t v_Type;        /**< Message Type - Application,File,None*/
    uint16_t v_Filesize;   /**< Maximum File size*/
    uint8_t v_Opt;        /**< Source Type -E-Error,W-Warnings,I-Information,D-Debug,F-Function,T-Timestamp  \ref e_lbsconfig_LogSeverity*/
}t_lbscfg_LogParam;


/**
* \brief
*      Interface for write to the Logging configuration file.
* \returns
*      None
*/

void LBSCFG_LogWriteConfiguration();

/**
* \brief
*      Interface for read from Logging configuration file.
* \returns
*      None
*/

void LBSCFG_LogReadConfiguration();

/**
* \brief
*      Interface for set a specific log configuration in to the platform.
* \details
*      This function is used by Users of LBSCFG to update the configuration obtained
*      from outside.
* \param v_Type [in] Type log expected.
* \param Config_data [out] Pointer to the memory for data structure to be filled.
* \returns
*      TRUE : Success in getting the values.
*      FALSE : Module logging is not configurable.
*/

uint8_t LBSCFG_LogGetConfiguration(e_lbscfg_LogModuleType v_Type, t_lbscfg_LogParam *Config_data);

/**
* \brief
*      Interface for set a specific log configuration in to the platform.
* \details
*      This function is used by Users of LBSCFG to update the configuration obtained
*      from outside.
* \param v_Type [in] Type log expected.
* \param Config_data [out] Pointer to the memory for data structure to be filled.
* \returns
*      TRUE : Success in getting the values.
*      FALSE : Module logging is not configurable.
*/

uint8_t LBSCFG_LogSetConfiguration(e_lbscfg_LogModuleType v_Type, t_lbscfg_LogParam *Config_data);




/**
* \brief
*      Interface for write to the User configuration file.
* \returns
*      None
*/

void LBSCFG_UserWriteConfiguration();

/**
* \brief
*      Interface for read from User configuration file.
* \returns
*      None
*/

void LBSCFG_UserReadConfiguration();


/**
* \brief
*      Interface for write to the Platform configuration file.
* \returns
*      None
*/

void LBSCFG_PlatformWriteConfiguration();

/**
* \brief
*      Interface for read from Platform configuration file.
* \returns
*      None
*/

void LBSCFG_PlatformReadConfiguration();

/**
* \brief
*      Interface for set a specific user configuration in to the platform.
* \details
*      This function is used by Users of LBSCFG to update the configuration obtained
*      from outside.
* \param v_Type [in] Type log expected.
* \param Config_data [out] Pointer to the memory for data structure to be filled.
* \returns
*      TRUE : Success in getting the values.
*      FALSE : Module logging is not configurable.
*/

uint8_t LBSCFG_UserGetConfiguration();


/**
* \brief
*      Interface for set a specific user configuration in to the platform.
* \details
*      This function is used by Users of LBSCFG to update the configuration obtained
*      from outside.
* \param v_Type [in] Type log expected.
* \param Config_data [out] Pointer to the memory for data structure to be filled.
* \returns
*      TRUE : Success in getting the values.
*      FALSE : Module logging is not configurable.
*/

uint8_t LBSCFG_UserSetConfiguration();


/**
* \brief
*      Interface for set a specific platform configuration in to the platform.
* \details
*      This function is used by Users of LBSCFG to update the configuration obtained
*      from outside.
* \param v_Type [in] Type log expected.
* \param Config_data [out] Pointer to the memory for data structure to be filled.
* \returns
*      TRUE : Success in getting the values.
*      FALSE : Module logging is not configurable.
*/

uint8_t LBSCFG_PlatformGetConfiguration();


/**
* \brief
*      Interface for set a specific platform configuration in to the platform.
* \details
*      This function is used by Users of LBSCFG to update the configuration obtained
*      from outside.
* \param v_Type [in] Type log expected.
* \param Config_data [out] Pointer to the memory for data structure to be filled.
* \returns
*      TRUE : Success in getting the values.
*      FALSE : Module logging is not configurable.
*/

uint8_t LBSCFG_PlatformSetConfiguration();
#endif


