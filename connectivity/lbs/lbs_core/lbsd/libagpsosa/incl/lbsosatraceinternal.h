/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) 2012 ST-Ericsson.
 *  All rights reserved
 *  Date: 28-08-2012
 *  Author: Dhinu Thomas
 *  Email : Dhinu.Thomas@stericsson.com
 *****************************************************************************/

#ifndef _LBS_OSA_TRACE_INTERNAL_H_
#define _LBS_OSA_TRACE_INTERNAL_H_

#include "agpsosa.h"

#define MC_LBSOSATRACE_APPL(trace_string)  OSA_Print(("%s", trace_string))


#define K_LBSOSATRACE_SIZE     980

/*Length of header. Ex: LBS_FSM -, LBS_CGPS-, LBS_LSIM-, LBS_NMEA-*/
#define K_LBSOSATRACE_HEADER_SIZE                  9
/*Length of message. Ex: INF:, WRN:, ERR:*/
#define K_LBSOSATRACE_SEVERITY_SIZE                4
#define K_LBSOSATRACE_TIMESTAMP_SIZE              13

#define K_LBSOSATRACE_FORMATTED_SIZE              (K_LBSOSATRACE_SIZE + K_LBSOSATRACE_HEADER_SIZE + K_LBSOSATRACE_SEVERITY_SIZE)

#define K_LBSOSATRACE_TIME_FORMATTED_SIZE         (K_LBSOSATRACE_FORMATTED_SIZE + K_LBSOSATRACE_TIMESTAMP_SIZE)


#if defined (AGPS_LINUX_FTR)
#define K_LBSOSATRACE_ROOT_PATH "/data"
#elif defined(MMC_FTR)
#define K_LBSOSATRACE_ROOT_PATH "/rem1"
#else
#define K_LBSOSATRACE_ROOT_PATH "/share1"
#endif

/**
 * \def K_LBSOSATRACE_NMEA_FILE
 *
 * Define the full path file name, where are stored the NMEA log
 */
#define K_LBSOSATRACE_NMEA_FILE  K_LBSOSATRACE_ROOT_PATH "/LBS_NMEA.txt"

/**
 * \def K_LBSOSATRACE_GNB_FILE
 *
 * Define the full path file name, where are stored the GNB debug informations
 */
#define K_LBSOSATRACE_GNB_FILE  K_LBSOSATRACE_ROOT_PATH "/LBS_GNBD.txt"

/**
 * \def K_LBSOSATRACE_NAV_FILE
 *
 * Define the full path file name, where are stored the Navigation debug information
 */
#define K_LBSOSATRACE_NAV_FILE  K_LBSOSATRACE_ROOT_PATH "/LBS_NAVD.txt"

/**
 * \def K_LBSOSATRACE_EVENT_FILE
 *
 * Define the full path file name, where are stored the event log informations
 */
#define K_LBSOSATRACE_EVENT_FILE  K_LBSOSATRACE_ROOT_PATH "/LBS_EVNT.txt"

/**
 * \def K_LBSOSATRACE_SUPL_FILE
 *
 * Define the full path file name, where are stored the SUPL event log informations
 */
#define K_LBSOSATRACE_SUPL_FILE  K_LBSOSATRACE_ROOT_PATH "/LBS_SUPL.txt"


/**
 * \def K_LBSOSATRACE_FSM_FILE
 *
 * Define the full path file name, where are stored the CGPS log data information
 */
#define K_LBSOSATRACE_FSM_FILE   K_LBSOSATRACE_ROOT_PATH "/LBS_FSMX.txt"

/**
 * \def K_LBSOSATRACE_CGPS_FILE
 *
 * Define the full path file name, where are stored the CGPS log data information
 */
#define K_LBSOSATRACE_CGPS_FILE   K_LBSOSATRACE_ROOT_PATH "/LBS_CGPS.txt"


#ifdef CMCC_LOGGING_ENABLE
/**
 * \def K_LBSOSATRACE_CMCC_FILE
 *
 * Define the full path file name, where are stored the CMCC log data information
 */
#define K_LBSOSATRACE_CMCC_FILE   K_LBSOSATRACE_ROOT_PATH "/LBS_CMCC.txt"
#endif /* #ifdef CMCC_LOGGING_ENABLE */


/**
 * \struct t_lbsosatrace_Config
 * \brief Structure which stores the Logging configuration.
 */
typedef struct{
    uint8_t     v_Type;       /**< Message Type - Application,File,None*/
    uint16_t    v_Filesize;   /**< Maximum File size*/
    uint8_t     v_Severity;   /**< Trace Type -E-Error,W-Warnings,I-Information,D-Debug,F-Function,T-Timestamp  \ref e_lbsconfig_LogSeverity*/
    t_gfl_FILE* p_FileDesc;   /** <File pointer to the opened logfiles */
}t_lbsosatrace_Config;



#endif /* _LBS_OSA_TRACE_INTERNAL_H_ */

