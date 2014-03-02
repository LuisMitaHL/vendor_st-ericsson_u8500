/*
 * Lbs Config Module
 *
 * lbsconfig.c
 *
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include <strings.h>

#include <lbscfg.h>
#include <lbscfg_internal.h>

#include "lbsosatrace.h"


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 0

/**
* \brief
*      Interface for write to the Logging configuration file.
* \returns
*      None
*/

void LBSCFG_LogWriteConfiguration()
{

    t_gfl_FILE * LogConfigFILEUpdate;

    if ((LogConfigFILEUpdate = MC_GFL_FOPEN(LBSCFG_LOGGING_CONFIGURATION_FILE_NAME,(const uint8_t*)"wb")) == NULL)
    {
      printf("ERR : LbsConfig.cfg File Not Found for Updating");
    }
    else
    {
        printf("INF : LbsConfig.cfg File Found for Updating");
        MC_GFL_FWRITE(&vg_lbscfg.v_log, 1, sizeof(t_lbscfg_User), LogConfigFILEUpdate);
        MC_GFL_FCLOSE(LogConfigFILEUpdate);
    }


}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 1

/**
* \brief
*      Interface for read from Logging configuration file.
* \returns
*      None
*/

void LBSCFG_LogReadConfiguration()
{
    t_gfl_FILE * LogConfigFILE;

    vg_lbscfg.v_LogFileReadStatus = FALSE;

    if ((LogConfigFILE = MC_GFL_FOPEN(LBSCFG_LOGGING_CONFIGURATION_FILE_NAME,(const uint8_t*)"rb")) == NULL)
    {
      printf("ERR : LbsLogConfig.cfg File Not Found");
    }
    else
    {
        printf("INF : LbsLogConfig.cfg File Found");
        MC_GFL_FREAD(&vg_lbscfg.v_log, sizeof(char),sizeof( t_lbscfg_Logging ), LogConfigFILE);
        vg_lbscfg.v_LogFileReadStatus = TRUE;
        MC_GFL_FCLOSE(LogConfigFILE);
        LbsOsaTrace_UpdateConfiguration((t_lbsosatrace_LogConfig *)&vg_lbscfg.v_log);
    }
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 2

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


uint8_t LBSCFG_LogGetConfiguration(e_lbscfg_LogModuleType v_Type, t_lbscfg_LogParam *Config_data)
{
  uint8_t retval = TRUE;

  if(vg_lbscfg.v_LogFileReadStatus == FALSE)
  {
      printf("ERR : LbsLogConfig.cfg File was Not Found using Default config values \n");
      Config_data->v_Type     = K_LBSCFG_LOG_TYPE_APPL;
      Config_data->v_Filesize = 0;
      Config_data->v_Opt      = 15;
      return FALSE;
  }

  if(Config_data == NULL) return FALSE;

  switch(v_Type)
  {
    case K_LBSCFG_FSM:
        Config_data->v_Type     = vg_lbscfg.v_log.v_FsmType;
        Config_data->v_Filesize = vg_lbscfg.v_log.v_FsmMaxFilesize;
        Config_data->v_Opt      = vg_lbscfg.v_log.v_FsmOpt;
    break;
    case K_LBSCFG_CGPS:
        Config_data->v_Type     = vg_lbscfg.v_log.v_CgpsType;
        Config_data->v_Filesize = vg_lbscfg.v_log.v_CgpsMaxFilesize;
        Config_data->v_Opt      = vg_lbscfg.v_log.v_CgpsOpt;
    break;
    case K_LBSCFG_CMCC:
        Config_data->v_Type     = vg_lbscfg.v_log.v_CmccType;
        Config_data->v_Filesize = vg_lbscfg.v_log.v_CmccMaxFilesize;
        Config_data->v_Opt      = vg_lbscfg.v_log.v_CmccOpt;
    break;
    case K_LBSCFG_SUPL:
        Config_data->v_Type     = vg_lbscfg.v_log.v_SuplType;
        Config_data->v_Filesize = vg_lbscfg.v_log.v_SuplMaxFilesize;
        Config_data->v_Opt      = vg_lbscfg.v_log.v_SuplOpt;
    break;
    case K_LBSCFG_CHIP_LOG1:
        Config_data->v_Type     = vg_lbscfg.v_log.v_ChipLog1Type;
        Config_data->v_Filesize = vg_lbscfg.v_log.v_ChipLog1MaxFilesize;
        Config_data->v_Opt      = vg_lbscfg.v_log.v_ChipLog1Opt;
    break;
    case K_LBSCFG_CHIP_LOG2:
        Config_data->v_Type     = vg_lbscfg.v_log.v_ChipLog2Type;
        Config_data->v_Filesize = vg_lbscfg.v_log.v_ChipLog2MaxFilesize;
        Config_data->v_Opt      = vg_lbscfg.v_log.v_ChipLog2Opt;
    break;
    case K_LBSCFG_CHIP_LOG3:
        Config_data->v_Type     = vg_lbscfg.v_log.v_ChipLog3Type;
        Config_data->v_Filesize = vg_lbscfg.v_log.v_ChipLog3MaxFilesize;
        Config_data->v_Opt      = vg_lbscfg.v_log.v_ChipLog3Opt;
    break;
    case K_LBSCFG_CHIP_LOG4:
        Config_data->v_Type     = vg_lbscfg.v_log.v_ChipLog4Type;
        Config_data->v_Filesize = vg_lbscfg.v_log.v_ChipLog4MaxFilesize;
        Config_data->v_Opt      = vg_lbscfg.v_log.v_ChipLog4Opt;
    break;
    case K_LBSCFG_LSIM:
    case K_LBSCFG_SBEE:
    case K_LBSCFG_HALGPS:
    default:
    retval = FALSE;
    break;

  }
  return retval;

}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 3

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

uint8_t LBSCFG_LogSetConfiguration(e_lbscfg_LogModuleType v_Type, t_lbscfg_LogParam *Config_data)
{
  uint8_t retval = TRUE;

  if(vg_lbscfg.v_LogFileReadStatus == FALSE) return FALSE;

  if(Config_data == NULL) return FALSE;

  switch(v_Type)
  {
    case K_LBSCFG_FSM:
        vg_lbscfg.v_log.v_FsmType        = Config_data->v_Type;
        vg_lbscfg.v_log.v_FsmMaxFilesize = Config_data->v_Filesize;
        vg_lbscfg.v_log.v_FsmOpt         = Config_data->v_Opt;
    break;
    case K_LBSCFG_CGPS:
        vg_lbscfg.v_log.v_CgpsType        = Config_data->v_Type;
        vg_lbscfg.v_log.v_CgpsMaxFilesize = Config_data->v_Filesize;
        vg_lbscfg.v_log.v_CgpsOpt         = Config_data->v_Opt;
    break;
    case K_LBSCFG_CMCC:
        vg_lbscfg.v_log.v_CmccType        = Config_data->v_Type;
        vg_lbscfg.v_log.v_CmccMaxFilesize = Config_data->v_Filesize;
        vg_lbscfg.v_log.v_CmccOpt         = Config_data->v_Opt;
    break;
    case K_LBSCFG_SUPL:
        vg_lbscfg.v_log.v_SuplType        = Config_data->v_Type;
        vg_lbscfg.v_log.v_SuplMaxFilesize = Config_data->v_Filesize;
        vg_lbscfg.v_log.v_SuplOpt         = Config_data->v_Opt;
    break;
    case K_LBSCFG_CHIP_LOG1:
        vg_lbscfg.v_log.v_ChipLog1Type        = Config_data->v_Type;
        vg_lbscfg.v_log.v_ChipLog1MaxFilesize = Config_data->v_Filesize;
        vg_lbscfg.v_log.v_ChipLog1Opt         = Config_data->v_Opt;
    break;
    case K_LBSCFG_CHIP_LOG2:
        vg_lbscfg.v_log.v_ChipLog2Type        = Config_data->v_Type;
        vg_lbscfg.v_log.v_ChipLog2MaxFilesize = Config_data->v_Filesize;
        vg_lbscfg.v_log.v_ChipLog2Opt         = Config_data->v_Opt;
    break;
    case K_LBSCFG_CHIP_LOG3:
        vg_lbscfg.v_log.v_ChipLog3Type        = Config_data->v_Type;
        vg_lbscfg.v_log.v_ChipLog3MaxFilesize = Config_data->v_Filesize;
        vg_lbscfg.v_log.v_ChipLog3Opt         = Config_data->v_Opt;
    break;
    case K_LBSCFG_CHIP_LOG4:
        vg_lbscfg.v_log.v_ChipLog4Type        = Config_data->v_Type;
        vg_lbscfg.v_log.v_ChipLog4MaxFilesize = Config_data->v_Filesize;
        vg_lbscfg.v_log.v_ChipLog4Opt         = Config_data->v_Opt;
    break;
    case K_LBSCFG_LSIM:
    case K_LBSCFG_HALGPS:
    case K_LBSCFG_SBEE:
    default:
    retval = FALSE;
    break;

  }
  LbsOsaTrace_UpdateConfiguration((t_lbsosatrace_LogConfig *)&vg_lbscfg.v_log);
  return retval;
}



