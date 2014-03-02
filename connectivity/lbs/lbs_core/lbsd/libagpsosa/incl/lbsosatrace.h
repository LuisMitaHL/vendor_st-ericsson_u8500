/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) 2012 ST-Ericsson.
 *  All rights reserved
 *  Date: 28-08-2012
 *  Author: Dhinu Thomas
 *  Email : Dhinu.Thomas@stericsson.com
 *****************************************************************************/

#ifndef _LBS_OSA_TRACE_H_
#define _LBS_OSA_TRACE_H_

#ifdef __LBSOSATRACE_C__
#define GLOBAL
#else
#define GLOBAL extern
#endif

#include "agpsosa.h"

#define LBSOSATRACE_PRINT            LbsOsaTrace_Print


/**
 * \enum e_lbsosatrace_LogSeverity
 * \brief Num to provide module log
 */
typedef enum
{
    K_LBSOSATRACE_LOG_LEV_ERR  = (1<<0),  /**< Error */
    K_LBSOSATRACE_LOG_LEV_WRN  = (1<<1),  /**< Warning*/
    K_LBSOSATRACE_LOG_LEV_INF  = (1<<2),  /**< Information*/
    K_LBSOSATRACE_LOG_LEV_DBG  = (1<<3),  /**< Debug*/
    K_LBSOSATRACE_LOG_LEV_FNC  = (1<<4),  /**< Function*/
    K_LBSOSATRACE_LOG_LEV_TME  = (1<<5)   /**< Time*/
} e_lbsosatrace_LogSeverity;


/**
 * \enum e_lbsosatrace_LogType
 * \brief Type of Log
 */
typedef enum
{
    K_LBSOSATRACE_LOG_TYPE_APPL,  /**< Logging into Application or Platform Trace*/
    K_LBSOSATRACE_LOG_TYPE_FILE,   /**< Logging into File*/
    K_LBSOSATRACE_LOG_TYPE_NONE  /**<No Logging */
} e_lbsosatrace_LogType;


/**
 * \enum e_lbsosatrace_ModuleType
 * \brief Num to provide module log
 */
typedef enum
{
    K_LBSOSATRACE_FSM,      /**< FSM Logging Configuration*/
    K_LBSOSATRACE_CGPS,     /**< CGPS Logging Configuration*/
    K_LBSOSATRACE_HALGPS,   /**< HALGPS Logging Configuration*/
    K_LBSOSATRACE_CMCC,     /**< CMCC Logging Configuration*/
    K_LBSOSATRACE_LSIM,     /**< LSIM Logging Configuration*/
    K_LBSOSATRACE_SBEE,     /**< SBEE Logging Configuration*/
    K_LBSOSATRACE_SUPL,     /**< SUPL Logging Configuration*/
    K_LBSOSATRACE_CHIP_LOG1,/**< CHIP LOG 1 Logging Configuration*/
    K_LBSOSATRACE_CHIP_LOG2,/**< CHIP LOG 2 Logging Configuration*/
    K_LBSOSATRACE_CHIP_LOG3,/**< CHIP LOG 3 Logging Configuration*/
    K_LBSOSATRACE_CHIP_LOG4,/**< CHIP LOG 4 Logging Configuration*/

    K_LBSOSATRACE_MODULE_MAX   /**< This should be last in the enum*/
} e_lbsosatrace_ModuleType;



/**
 * \struct t_lbsosatrace_Config read
 * \brief Structure which provide the Logging configuration. This should be
 *          in sync with t_lbscfg_Logging from lbscfg module*/

typedef struct{
    uint8_t  v_Version[50];            /**< Logging Configuration file version*/
    uint8_t  v_ChipLog1Type;           /**< GNB Message Type - Application,File,None*/
    uint16_t v_ChipLog1MaxFilesize;    /**< GNB Maximum File size*/
    uint8_t  v_ChipLog1Opt;            /**< GNB Source Type -E-Error,W-Warnings,I-Information,D-Debug,F-Function,T-Timestamp  */
    uint8_t  v_ChipLog2Type;           /**< NMEA Message Type - Application,File,None*/
    uint16_t v_ChipLog2MaxFilesize;    /**< NMEA Maximum File size*/
    uint8_t  v_ChipLog2Opt;            /**< NMEA Source Type -E-Error,W-Warnings,I-Information,D-Debug,F-Function,T-Timestamp  */
    uint8_t  v_ChipLog3Type;           /**< NAV Message Type - Application,File,None*/
    uint16_t v_ChipLog3MaxFilesize;    /**< NAV Maximum File size*/
    uint8_t  v_ChipLog3Opt;            /**< NAV Source Type -E-Error,W-Warnings,I-Information,D-Debug,F-Function,T-Timestamp  */
    uint8_t  v_ChipLog4Type;           /**< EVENT Message Type - Application,File,None*/
    uint16_t v_ChipLog4MaxFilesize;    /**< EVENT Maximum File size*/
    uint8_t  v_ChipLog4Opt;            /**< EVENT Source Type -E-Error,W-Warnings,I-Information,D-Debug,F-Function,T-Timestamp  */
    uint8_t  v_SuplType;               /**< SUPL Message Type - Application,File,None*/
    uint16_t v_SuplMaxFilesize;        /**< SUPL Maximum File size*/
    uint8_t  v_SuplOpt;                /**< SUPL Source Type -E-Error,W-Warnings,I-Information,D-Debug,F-Function,T-Timestamp  */
    uint8_t  v_CmccType;               /**< CMCC Message Type - Application,File,None*/
    uint16_t v_CmccMaxFilesize;        /**< CMCC Maximum File size*/
    uint8_t  v_CmccOpt;                /**< CMCC Source Type -E-Error,W-Warnings,I-Information,D-Debug,F-Function,T-Timestamp  */
    uint8_t  v_CgpsType;               /**< CGPS Message Type - Application,File,None*/
    uint16_t v_CgpsMaxFilesize;        /**< CGPS Maximum File size*/
    uint8_t  v_CgpsOpt;                /**< CGPS Source Type -E-Error,W-Warnings,I-Information,D-Debug,F-Function,T-Timestamp  */
    uint8_t  v_FsmType;                /**< FSM Message Type - Application,File,None*/
    uint16_t v_FsmMaxFilesize;         /**< FSM Maximum File size*/
    uint8_t  v_FsmOpt;                 /**< FSM Source Type -E-Error,W-Warnings,I-Information,D-Debug,F-Function,T-Timestamp  */

}t_lbsosatrace_LogConfig;


/**
 * \brief
 *         Not completely implemented. This is used for setting
 *         the default logging config.
 * \details
 *      This is called before the config file is red. For the time being,
 *      the default logging is set as file logging.
 * \param
 * \returns
 */
extern void LbsOsaTrace_Init();

/**
 * \brief
 *      This function is called for info traces with variable parameters.
 * \details
 * \param lbs_module - Module type
 * \param variable number or args of type char*
 * \returns return the number of bytes written.
 */
extern uint16_t LbsOsaTrace_Print(e_lbsosatrace_ModuleType, e_lbsosatrace_LogSeverity, char *, ...);

/**
 * \brief
 *      This function is used for upadating the LbsOsaTrace configuration.
 * \details
 *      This function should be called whenever there is any chnage
 *      in the logging configuration. The parameter passing shold be
 *      in the form which can be directly mapped to t_lbsosatrace_LogConfig.
 * \param vp_config pointer to structure t_lbsosatrace_LogConfig
 * \returns
 */
void LbsOsaTrace_UpdateConfiguration(const t_lbsosatrace_LogConfig *);


/**
 * \brief
 *      Function called to flush and close all the log files.
 * \details
 * \param
 * \returns
 */
void LbsOsaTrace_CloseLogFiles();

#undef GLOBAL

#endif /* _LBS_OSA_TRACE_H_ */

