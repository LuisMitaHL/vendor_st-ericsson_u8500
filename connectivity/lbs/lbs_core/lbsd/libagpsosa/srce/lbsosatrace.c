/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) 2012 ST-Ericsson.
 *  All rights reserved
 *  Date: 28-08-2012
 *  Author: Dhinu Thomas
 *  Email : Dhinu.Thomas@stericsson.com
 *****************************************************************************/

#define __LBSOSATRACE_C__

#include <string.h>
#include <stdarg.h>

#include "lbsosatraceinternal.h"
#include "lbsosatrace.h"

/*
 *   This static memory is not thread safe.
 *   Used for storing only config parameters.
 */
static t_lbsosatrace_Config  v_LbsOsaTrace_Config[K_LBSOSATRACE_MODULE_MAX];


/**
 * \brief
 *      Used for getting the file path corresponding to the module.
 * \details
 * \param vp_Module : The module type
 * \returns
 */
static char* LbsOsaTrace_GetFilePath(e_lbsosatrace_ModuleType vp_Module)
{
    switch(vp_Module)
    {
        case K_LBSOSATRACE_FSM:
            return  K_LBSOSATRACE_FSM_FILE;
        case K_LBSOSATRACE_CGPS:
            return  K_LBSOSATRACE_CGPS_FILE;
        case K_LBSOSATRACE_SUPL:
            return  K_LBSOSATRACE_SUPL_FILE;
        case K_LBSOSATRACE_CHIP_LOG1:
            return K_LBSOSATRACE_GNB_FILE;
        case K_LBSOSATRACE_CHIP_LOG2:
            return  K_LBSOSATRACE_NMEA_FILE;
        case K_LBSOSATRACE_CHIP_LOG3:
            return  K_LBSOSATRACE_NAV_FILE;
        case K_LBSOSATRACE_CHIP_LOG4:
            return  K_LBSOSATRACE_EVENT_FILE;
#ifdef  CMCC_LOGGING_ENABLE
        case K_LBSOSATRACE_CMCC:
            return  K_LBSOSATRACE_CMCC_FILE;
#endif
        default:
            return "";
    }
}


/**
 * \brief
 *      Formats the traces and handls printing according to conf.
 * \details
 * \param vp_Module - vp_Module from which the trace is printed
 * \param config - configuration
 * \param pp_Buffer - Contains the buffer to print
 * \returns
 */
static int32_t LbsOsaTrace_WriteLog(e_lbsosatrace_ModuleType vp_Module, int8_t *pp_Buffer)
{
    int32_t vl_Retval = 0;

    /*Writing to Console*/
    if( K_LBSOSATRACE_LOG_TYPE_APPL == v_LbsOsaTrace_Config[vp_Module].v_Type )
    {
        OSA_Print(("%s", pp_Buffer));
    }
    else if( v_LbsOsaTrace_Config[vp_Module].v_Type == K_LBSOSATRACE_LOG_TYPE_FILE )
    {
        struct stat vl_Stat;
        int32_t     vl_StatRet;

        /*If the log file was closed earlier, open it for writing*/
        if(NULL == v_LbsOsaTrace_Config[vp_Module].p_FileDesc
                && NULL == (v_LbsOsaTrace_Config[vp_Module].p_FileDesc = (t_gfl_FILE *)MC_GFL_FOPEN((char *)LbsOsaTrace_GetFilePath(vp_Module),(const uint8_t*)"a+")))
        {
            return 0;
        }

        /*If the current file size exceeds the max file size from conf, do rollover */
        if(v_LbsOsaTrace_Config[vp_Module].v_Filesize)
        {
            uint32_t vl_FileSize = 0;


            /* Get file size*/
            vl_StatRet = MC_GFL_FSTAT(fileno(v_LbsOsaTrace_Config[vp_Module].p_FileDesc), &vl_Stat);
            vl_FileSize = (vl_StatRet==0) ? vl_Stat.st_size :0;


            /* Condition 1: To check if the file size exceeds max permitted size.
                      Condition 2: To check if the file position indicator has reached at the end of the file.     */
            if(((uint32_t)(v_LbsOsaTrace_Config[vp_Module].v_Filesize*1024*1024) < (vl_FileSize+999))
                    && MC_GFL_FTELL(v_LbsOsaTrace_Config[vp_Module].p_FileDesc) >= vl_FileSize)
            {
                /*Close the file and open in 'r+' mode to set the file position indicator to the beginning of the file. */

                MC_GFL_FCLOSE(v_LbsOsaTrace_Config[vp_Module].p_FileDesc);
                if(NULL != (v_LbsOsaTrace_Config[vp_Module].p_FileDesc = (t_gfl_FILE *)MC_GFL_FOPEN((char *)LbsOsaTrace_GetFilePath(vp_Module),(const uint8_t*)"r+")))
                {
                    MC_GFL_FWRITE( "************************ LOG ROLLOVER ************************\n", 1, 64, v_LbsOsaTrace_Config[vp_Module].p_FileDesc);
                }
            }

        }

        uint16_t buflen = strlen((char*)(pp_Buffer));

#if 0
        /*
           This is for checking if the file is deleted due to some reason.
           For the normal logging, it is disabled since it is an expensive
           system call.
         */
        vl_StatRet = MC_GFL_STAT(LbsOsaTrace_GetFilePath(vp_Module), &vl_Stat);
#else
        vl_StatRet = 0;
#endif
        if(0 == vl_StatRet)
            vl_Retval = MC_GFL_FWRITE(pp_Buffer, 1, buflen, v_LbsOsaTrace_Config[vp_Module].p_FileDesc);
        else
        {
            /*This is fail safe condition that in case the file was closed
             *for eg: The SD card is unmounted.
             *In this case open the file again and update the p_FileDesc
             */
            /*close fd if possible*/
            MC_GFL_FCLOSE(v_LbsOsaTrace_Config[vp_Module].p_FileDesc);
            v_LbsOsaTrace_Config[vp_Module].p_FileDesc = NULL;

            /*If opened, write the trace to fd*/
            if(NULL !=(v_LbsOsaTrace_Config[vp_Module].p_FileDesc = (t_gfl_FILE *)MC_GFL_FOPEN((char *)LbsOsaTrace_GetFilePath(vp_Module),(const uint8_t*)"a+")))
            {
                uint8_t *tempbuf = "**************** Coudn't stat the file. Reopening ****************\n";
                MC_GFL_FWRITE(tempbuf, 1, strlen((char*)(tempbuf)), v_LbsOsaTrace_Config[vp_Module].p_FileDesc);
                vl_Retval = MC_GFL_FWRITE(pp_Buffer, 1, buflen, v_LbsOsaTrace_Config[vp_Module].p_FileDesc);
            }
        }
    }
    return vl_Retval;
}


/**
 * \brief
 *      The implementation may change later. This is used for setting
 *         the default logging config.
 * \details
 *      This is called before the config file is red. For the time being,
 *      the default logging is set as file logging.
 * \param
 * \returns
 */
void LbsOsaTrace_Init()
{
    /*Setting the default conf -- FSM, CGPS and SUPL to Application
      PE logs to filesystem, CMCC logs are disabled.
     */
    v_LbsOsaTrace_Config[K_LBSOSATRACE_FSM].v_Type     = K_LBSOSATRACE_LOG_TYPE_APPL;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_FSM].v_Filesize = 0;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_FSM].v_Severity = 0;
    if(NULL == (v_LbsOsaTrace_Config[K_LBSOSATRACE_FSM].p_FileDesc = (t_gfl_FILE *)MC_GFL_FOPEN(K_LBSOSATRACE_FSM_FILE,(const uint8_t*)"a+")))
    {
        OSA_Print(("ERROR : Unable to Open file %s.\n", K_LBSOSATRACE_FSM_FILE));
    }

    v_LbsOsaTrace_Config[K_LBSOSATRACE_CGPS].v_Type     = K_LBSOSATRACE_LOG_TYPE_APPL;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_CGPS].v_Filesize = 0;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_CGPS].v_Severity = K_LBSOSATRACE_LOG_LEV_INF;
    if(NULL == (v_LbsOsaTrace_Config[K_LBSOSATRACE_CGPS].p_FileDesc = (t_gfl_FILE *)MC_GFL_FOPEN(K_LBSOSATRACE_CGPS_FILE,(const uint8_t*)"a+")))
    {
        OSA_Print(("ERROR : Unable to Open file %s.\n", K_LBSOSATRACE_CGPS_FILE));
    }

    v_LbsOsaTrace_Config[K_LBSOSATRACE_CMCC].v_Type     = K_LBSOSATRACE_LOG_TYPE_NONE;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_CMCC].v_Filesize = 0;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_CMCC].v_Severity = 0;
#ifdef CMCC_LOGGING_ENABLE
    if(NULL == (v_LbsOsaTrace_Config[K_LBSOSATRACE_CMCC].p_FileDesc = (t_gfl_FILE *)MC_GFL_FOPEN(K_LBSOSATRACE_CMCC_FILE,(const uint8_t*)"a+")))
    {
        OSA_Print(("ERROR : Unable to Open file %s.\n", K_LBSOSATRACE_CMCC_FILE));
    }
#endif

    v_LbsOsaTrace_Config[K_LBSOSATRACE_SUPL].v_Type     = K_LBSOSATRACE_LOG_TYPE_APPL;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_SUPL].v_Filesize = 0;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_SUPL].v_Severity = K_LBSOSATRACE_LOG_LEV_ERR | K_LBSOSATRACE_LOG_LEV_WRN | K_LBSOSATRACE_LOG_LEV_INF | K_LBSOSATRACE_LOG_LEV_DBG | K_LBSOSATRACE_LOG_LEV_FNC;;
    if(NULL == (v_LbsOsaTrace_Config[K_LBSOSATRACE_SUPL].p_FileDesc = (t_gfl_FILE *)MC_GFL_FOPEN(K_LBSOSATRACE_SUPL_FILE,(const uint8_t*)"a+")))
    {
        OSA_Print(("ERROR : Unable to Open file %s.\n", K_LBSOSATRACE_SUPL_FILE));
    }

    v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG1].v_Type     = K_LBSOSATRACE_LOG_TYPE_FILE;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG1].v_Filesize = 0;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG1].v_Severity = K_LBSOSATRACE_LOG_LEV_INF;
    if(NULL == (v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG1].p_FileDesc = (t_gfl_FILE *)MC_GFL_FOPEN(K_LBSOSATRACE_GNB_FILE,(const uint8_t*)"a+")))
    {
        OSA_Print(("ERROR : Unable to Open file %s.\n", K_LBSOSATRACE_GNB_FILE));
    }

    v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG2].v_Type     = K_LBSOSATRACE_LOG_TYPE_FILE;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG2].v_Filesize = 0;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG2].v_Severity = K_LBSOSATRACE_LOG_LEV_INF;
    if(NULL == (v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG2].p_FileDesc = (t_gfl_FILE *)MC_GFL_FOPEN(K_LBSOSATRACE_NMEA_FILE,(const uint8_t*)"a+")))
    {
        OSA_Print(("ERROR : Unable to Open file %s.\n", K_LBSOSATRACE_NMEA_FILE));
    }

    v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG3].v_Type     = K_LBSOSATRACE_LOG_TYPE_FILE;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG3].v_Filesize = 0;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG3].v_Severity = K_LBSOSATRACE_LOG_LEV_INF;
    if(NULL == (v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG3].p_FileDesc = (t_gfl_FILE *)MC_GFL_FOPEN(K_LBSOSATRACE_NAV_FILE,(const uint8_t*)"a+")))
    {
        OSA_Print(("ERROR : Unable to Open file %s.\n", K_LBSOSATRACE_NAV_FILE));
    }

    v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG4].v_Type     = K_LBSOSATRACE_LOG_TYPE_FILE;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG4].v_Filesize = 0;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG4].v_Severity = K_LBSOSATRACE_LOG_LEV_INF;
    if(NULL == (v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG4].p_FileDesc = (t_gfl_FILE *)MC_GFL_FOPEN(K_LBSOSATRACE_EVENT_FILE,(const uint8_t*)"a+")))
    {
        OSA_Print(("ERROR : Unable to Open file %s.\n", K_LBSOSATRACE_EVENT_FILE));
    }
}



/**
 * \brief
 *      This function is used for upadating the LbsOsaTrace configuration.
 * \details
 *      This function should be called whenever there is any chnage
 *      in the logging configuration. The parameter passing shold be
 *      in the form which can be directly mapped to t_lbsosatrace_LogConfig.
 * \param pp_LogConfig pointer to structure t_lbsosatrace_LogConfig
 * \returns
 */
void LbsOsaTrace_UpdateConfiguration(const t_lbsosatrace_LogConfig *pp_LogConfig)
{
    v_LbsOsaTrace_Config[K_LBSOSATRACE_FSM].v_Type     = pp_LogConfig->v_FsmType;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_FSM].v_Filesize = pp_LogConfig->v_FsmMaxFilesize;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_FSM].v_Severity = pp_LogConfig->v_FsmOpt;
    /*If file logging, open the file only if it was not opened earlier*/
    if(K_LBSOSATRACE_LOG_TYPE_FILE == pp_LogConfig->v_FsmType && NULL == v_LbsOsaTrace_Config[K_LBSOSATRACE_FSM].p_FileDesc)
    {
        v_LbsOsaTrace_Config[K_LBSOSATRACE_FSM].p_FileDesc = (t_gfl_FILE *)MC_GFL_FOPEN(K_LBSOSATRACE_FSM_FILE,(const uint8_t*)"a+");
    }
    /*If Application logging, flush and close the file only if it was opened earlier*/
    else if (K_LBSOSATRACE_LOG_TYPE_APPL == pp_LogConfig->v_FsmType && NULL != v_LbsOsaTrace_Config[K_LBSOSATRACE_FSM].p_FileDesc) /*Close the file if it is already open*/
    {
        MC_GFL_FFLUSH(v_LbsOsaTrace_Config[K_LBSOSATRACE_FSM].p_FileDesc);
        MC_GFL_FCLOSE(v_LbsOsaTrace_Config[K_LBSOSATRACE_FSM].p_FileDesc);
        v_LbsOsaTrace_Config[K_LBSOSATRACE_FSM].p_FileDesc = NULL;
    }

    v_LbsOsaTrace_Config[K_LBSOSATRACE_CGPS].v_Type      = pp_LogConfig->v_CgpsType;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_CGPS].v_Filesize  = pp_LogConfig->v_CgpsMaxFilesize;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_CGPS].v_Severity  = pp_LogConfig->v_CgpsOpt;
    /*If file logging, open the file only if it was not opened earlier*/
    if(K_LBSOSATRACE_LOG_TYPE_FILE == pp_LogConfig->v_CgpsType && NULL == v_LbsOsaTrace_Config[K_LBSOSATRACE_CGPS].p_FileDesc)
    {
        v_LbsOsaTrace_Config[K_LBSOSATRACE_CGPS].p_FileDesc = (t_gfl_FILE *)MC_GFL_FOPEN(K_LBSOSATRACE_CGPS_FILE,(const uint8_t*)"a+");
    }
    /*If Application logging, flush and close the file only if it was opened earlier*/
    else if (K_LBSOSATRACE_LOG_TYPE_APPL == pp_LogConfig->v_CgpsType && NULL != v_LbsOsaTrace_Config[K_LBSOSATRACE_CGPS].p_FileDesc) /*Close the file if it is already open*/
    {
        MC_GFL_FFLUSH(v_LbsOsaTrace_Config[K_LBSOSATRACE_CGPS].p_FileDesc);
        MC_GFL_FCLOSE(v_LbsOsaTrace_Config[K_LBSOSATRACE_CGPS].p_FileDesc);
        v_LbsOsaTrace_Config[K_LBSOSATRACE_CGPS].p_FileDesc = NULL;
    }

#ifdef CMCC_LOGGING_ENABLE
    v_LbsOsaTrace_Config[K_LBSOSATRACE_CMCC].v_Type     = pp_LogConfig->v_CmccType;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_CMCC].v_Filesize = pp_LogConfig->v_CmccMaxFilesize;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_CMCC].v_Severity      = pp_LogConfig->v_CmccOpt;
    /*If file logging, open the file only if it was not opened earlier*/
    if(K_LBSOSATRACE_LOG_TYPE_FILE == pp_LogConfig->v_CmccType && NULL == v_LbsOsaTrace_Config[K_LBSOSATRACE_CMCC].p_FileDesc)
    {
        v_LbsOsaTrace_Config[K_LBSOSATRACE_CMCC].p_FileDesc = (t_gfl_FILE *)MC_GFL_FOPEN(K_LBSOSATRACE_CMCC_FILE,(const uint8_t*)"a+");
    }
    /*If Application logging, flush and close the file only if it was opened earlier*/
    else if (K_LBSOSATRACE_LOG_TYPE_APPL == pp_LogConfig->v_CmccType && NULL != v_LbsOsaTrace_Config[K_LBSOSATRACE_CMCC].p_FileDesc) /*Close the file if it is already open*/
    {
        MC_GFL_FFLUSH(v_LbsOsaTrace_Config[K_LBSOSATRACE_CMCC].p_FileDesc);
        MC_GFL_FCLOSE(v_LbsOsaTrace_Config[K_LBSOSATRACE_CMCC].p_FileDesc);
        v_LbsOsaTrace_Config[K_LBSOSATRACE_CMCC].p_FileDesc = NULL;
    }
#endif

    v_LbsOsaTrace_Config[K_LBSOSATRACE_SUPL].v_Type     = pp_LogConfig->v_SuplType;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_SUPL].v_Filesize = pp_LogConfig->v_SuplMaxFilesize;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_SUPL].v_Severity = pp_LogConfig->v_SuplOpt;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_SUPL].p_FileDesc = NULL;
    /*If file logging, open the file only if it was not opened earlier*/
    if(K_LBSOSATRACE_LOG_TYPE_FILE == pp_LogConfig->v_SuplType && NULL == v_LbsOsaTrace_Config[K_LBSOSATRACE_SUPL].p_FileDesc)
    {
        v_LbsOsaTrace_Config[K_LBSOSATRACE_SUPL].p_FileDesc = (t_gfl_FILE *)MC_GFL_FOPEN(K_LBSOSATRACE_SUPL_FILE,(const uint8_t*)"a+");
    }
    /*If Application logging, flush and close the file only if it was opened earlier*/
    else if (K_LBSOSATRACE_LOG_TYPE_APPL == pp_LogConfig->v_SuplType && NULL != v_LbsOsaTrace_Config[K_LBSOSATRACE_SUPL].p_FileDesc) /*Close the file if it is already open*/
    {
        MC_GFL_FFLUSH(v_LbsOsaTrace_Config[K_LBSOSATRACE_SUPL].p_FileDesc);
        MC_GFL_FCLOSE(v_LbsOsaTrace_Config[K_LBSOSATRACE_SUPL].p_FileDesc);
        v_LbsOsaTrace_Config[K_LBSOSATRACE_SUPL].p_FileDesc = NULL;
    }

    v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG1].v_Type     = pp_LogConfig->v_ChipLog1Type;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG1].v_Filesize = pp_LogConfig->v_ChipLog1MaxFilesize;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG1].v_Severity = pp_LogConfig->v_ChipLog1Opt;
    /*If file logging, open the file only if it was not opened earlier*/
    if(K_LBSOSATRACE_LOG_TYPE_FILE == pp_LogConfig->v_ChipLog1Type && NULL == v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG1].p_FileDesc)
    {
        v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG1].p_FileDesc = (t_gfl_FILE *)MC_GFL_FOPEN(K_LBSOSATRACE_GNB_FILE,(const uint8_t*)"a+");
    }
    /*If Application logging, flush and close the file only if it was opened earlier*/
    else if (K_LBSOSATRACE_LOG_TYPE_APPL == pp_LogConfig->v_ChipLog1Type && NULL != v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG1].p_FileDesc) /*Close the file if it is already open*/
    {
        MC_GFL_FFLUSH(v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG1].p_FileDesc);
        MC_GFL_FCLOSE(v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG1].p_FileDesc);
        v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG1].p_FileDesc = NULL;
    }

    v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG2].v_Type     = pp_LogConfig->v_ChipLog2Type;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG2].v_Filesize = pp_LogConfig->v_ChipLog2MaxFilesize;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG2].v_Severity = pp_LogConfig->v_ChipLog2Opt;
    /*If file logging, open the file only if it was not opened earlier*/
    if(K_LBSOSATRACE_LOG_TYPE_FILE == pp_LogConfig->v_ChipLog2Type && NULL == v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG2].p_FileDesc)
    {
        v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG2].p_FileDesc = (t_gfl_FILE *)MC_GFL_FOPEN(K_LBSOSATRACE_NMEA_FILE,(const uint8_t*)"a+");
    }
    /*If Application logging, flush and close the file only if it was opened earlier*/
    else if (K_LBSOSATRACE_LOG_TYPE_APPL == pp_LogConfig->v_ChipLog2Type && NULL != v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG2].p_FileDesc) /*Close the file if it is already open*/
    {
        MC_GFL_FFLUSH(v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG2].p_FileDesc);
        MC_GFL_FCLOSE(v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG2].p_FileDesc);
        v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG2].p_FileDesc = NULL;
    }

    v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG3].v_Type     = pp_LogConfig->v_ChipLog3Type;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG3].v_Filesize = pp_LogConfig->v_ChipLog3MaxFilesize;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG3].v_Severity = pp_LogConfig->v_ChipLog3Opt;
    /*If file logging, open the file only if it was not opened earlier*/
    if(K_LBSOSATRACE_LOG_TYPE_FILE == pp_LogConfig->v_ChipLog3Type && NULL == v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG3].p_FileDesc)
    {
        v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG3].p_FileDesc = (t_gfl_FILE *)MC_GFL_FOPEN(K_LBSOSATRACE_NAV_FILE,(const uint8_t*)"a+");
    }
    /*If Application logging, flush and close the file only if it was opened earlier*/
    else if (K_LBSOSATRACE_LOG_TYPE_APPL == pp_LogConfig->v_ChipLog3Type && NULL != v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG3].p_FileDesc) /*Close the file if it is already open*/
    {
        MC_GFL_FFLUSH(v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG3].p_FileDesc);
        MC_GFL_FCLOSE(v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG3].p_FileDesc);
        v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG3].p_FileDesc = NULL;
    }

    v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG4].v_Type     = pp_LogConfig->v_ChipLog4Type;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG4].v_Filesize = pp_LogConfig->v_ChipLog4MaxFilesize;
    v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG4].v_Severity = pp_LogConfig->v_ChipLog4Opt;
    /*If file logging, open the file only if it was not opened earlier*/
    if(K_LBSOSATRACE_LOG_TYPE_FILE == pp_LogConfig->v_ChipLog4Type && NULL == v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG4].p_FileDesc)
    {
        v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG4].p_FileDesc = (t_gfl_FILE *)MC_GFL_FOPEN(K_LBSOSATRACE_EVENT_FILE,(const uint8_t*)"a+");
    }
    /*If Application logging, flush and close the file only if it was opened earlier*/
    else if (K_LBSOSATRACE_LOG_TYPE_APPL == pp_LogConfig->v_ChipLog4Type && NULL != v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG4].p_FileDesc) /*Close the file if it is already open*/
    {
        MC_GFL_FFLUSH(v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG4].p_FileDesc);
        MC_GFL_FCLOSE(v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG4].p_FileDesc);
        v_LbsOsaTrace_Config[K_LBSOSATRACE_CHIP_LOG4].p_FileDesc = NULL;
    }
}


/**
 * \brief
 *      This function is called for info traces with variable parameters.
 * \details
 * \param lbs_module - vp_Module type
 * \param variable number or args of type char*
 * \returns return the number of bytes written.
 */
uint16_t LbsOsaTrace_Print(e_lbsosatrace_ModuleType vp_Module, e_lbsosatrace_LogSeverity vp_Severity, char *pp_Format, ...)
{
    if( v_LbsOsaTrace_Config[vp_Module].v_Type != K_LBSOSATRACE_LOG_TYPE_NONE
            /*For PE logs, the logging severity is handled inside the PE*/
            && ( (v_LbsOsaTrace_Config[vp_Module].v_Severity & vp_Severity) || ( vp_Module >= K_LBSOSATRACE_CHIP_LOG1) )
      )
    {
        /**
         * \int8_t* pl_TraceLevel
         * \brief strings to provide Severity in the trace
         */
        char* pl_TraceLevel[] =
        {
            "",        /**< 0*/
            "ERR",     /**< 1 - Error*/
            "WRN",     /**< 2 - Warning*/
            "",        /**< 3 */
            "INF",     /**< 4 - Information */
            "",        /**< 5*/
            "",        /**< 6*/
            "",        /**< 7*/
            "DBG",     /**< 8 - Debug*/
            "",        /**< 9*/
            "",        /**< 10*/
            "",        /**< 11*/
            "",        /**< 12*/
            "",        /**< 13*/
            "",        /**< 14*/
            "",        /**< 15*/
            "FNC",     /**< 16 - Function*/
        };

        /**
         * \int8_t* LogModuleType
         * \brief Strings to provide module header in the trace
         */
        char* pl_TraceModule[] =
        {
            "LBS_FSMX",     /**< FSM Logging Configuration*/
            "LBS_CGPS",     /**< CGPS Logging Configuration*/
            "LBS_HALX",     /**< HALGPS Logging Configuration*/
            "LBS_CMCC",     /**< CMCC Logging Configuration*/
            "LBS_LSIM",     /**< LSIM Logging Configuration*/
            "LBS_SBEE",     /**< SBEE Logging Configuration*/
            "LBS_SUPL",     /**< SUPL Logging Configuration*/
            "LBS_GNBD",     /**< CHIP LOG 1 Logging Configuration*/
            "LBS_NMEA",     /**< CHIP LOG 2 Logging Configuration*/
            "LBS_NAVD",     /**< CHIP LOG 3 Logging Configuration*/
            "LBS_EVNT"
        };

        int8_t pl_InputTrace[K_LBSOSATRACE_TIME_FORMATTED_SIZE];
        int8_t pl_FormattedTrace[K_LBSOSATRACE_TIME_FORMATTED_SIZE];

        memset(pl_InputTrace, 0, K_LBSOSATRACE_TIME_FORMATTED_SIZE);

        va_list Marker;
        va_start(Marker, pp_Format);

        /*Fill the str buffer from the place it finished writing before.*/
        vsnprintf((pl_InputTrace), (K_LBSOSATRACE_SIZE), pp_Format, Marker);
        va_end(Marker);

        /*Timestamp is required.*/
        if( v_LbsOsaTrace_Config[vp_Module].v_Type == K_LBSOSATRACE_LOG_TYPE_FILE && (v_LbsOsaTrace_Config[vp_Module].v_Severity & K_LBSOSATRACE_LOG_LEV_TME))
        {
            snprintf( pl_FormattedTrace, K_LBSOSATRACE_TIME_FORMATTED_SIZE, "%12d %s-%s:%s\n", GN_GPS_Get_OS_Time_ms(), pl_TraceModule[vp_Module], pl_TraceLevel[vp_Severity], pl_InputTrace);

            /*Add the new line at the end in case the buffer has
             *gone beyond K_LBSOSATRACE_TIME_FORMATTED_SIZE
             */
            pl_FormattedTrace[K_LBSOSATRACE_TIME_FORMATTED_SIZE - 2] = (int8_t)'\n';
        }
        else
        {
            snprintf( pl_FormattedTrace, K_LBSOSATRACE_FORMATTED_SIZE, "%s-%s:%s\n", pl_TraceModule[vp_Module], pl_TraceLevel[vp_Severity], pl_InputTrace);

            /*Add the new line at the end in case the buffer has
             *gone beyond K_LBSOSATRACE_FORMATTED_SIZE
             */
            pl_FormattedTrace[K_LBSOSATRACE_FORMATTED_SIZE - 2] = (int8_t)'\n';
        }

        return  LbsOsaTrace_WriteLog(vp_Module, pl_FormattedTrace);
    }
    else
    {
        return 0;
    }
}


/**
 * \brief
 *      Function called to flush and close all the log files.
 * \details
 * \param
 * \returns
 */
void LbsOsaTrace_CloseLogFiles()
{
    int vl_Index;

    for( vl_Index = 0; vl_Index < K_LBSOSATRACE_MODULE_MAX; vl_Index++)
    {
        if(NULL != v_LbsOsaTrace_Config[vl_Index].p_FileDesc)
        {
            MC_GFL_FFLUSH(v_LbsOsaTrace_Config[vl_Index].p_FileDesc);
            MC_GFL_FCLOSE(v_LbsOsaTrace_Config[vl_Index].p_FileDesc);
            v_LbsOsaTrace_Config[vl_Index].p_FileDesc = NULL;
        }
    }
}


#undef __LBSOSATRACE_C__
