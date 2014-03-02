/*
 * Lbs Config Module
 *
 * lbscfg_platform.c
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

#include <lbscfg_internal.h>
#include <lbscfg.h>


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 0


/**
* \brief
*      Interface for write to the Platform configuration file.
* \returns
*      None
*/

void LBSCFG_PlatformWriteConfiguration()
{
    t_gfl_FILE * PlatformConfigFILEUpdate;

    if ((PlatformConfigFILEUpdate = MC_GFL_FOPEN(LBSCFG_PLATFORM_CONFIGURATION_FILE_NAME,(const uint8_t*)"wb")) == NULL)
    {
      //MC_CGPS_TRACE(("ERR : LbsPltConfig.cfg File Not Found for Updating"));
    }
    else
    {
        //MC_CGPS_TRACE(("INF : LbsPltConfig.cfg File Found for Updating"));
        MC_GFL_FWRITE(&vg_lbscfg.v_plt, 1, sizeof(t_lbscfg_Platform), PlatformConfigFILEUpdate);
        MC_GFL_FCLOSE(PlatformConfigFILEUpdate);
    }


}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 1

/**
* \brief
*      Interface for read from Platform configuration file.
* \returns
*      None
*/

void LBSCFG_PlatformReadConfiguration()
{
    t_gfl_FILE * PlatformConfigFILE;
    vg_lbscfg.v_PlatformFileReadStatus = FALSE;

    if ((PlatformConfigFILE = MC_GFL_FOPEN(LBSCFG_PLATFORM_CONFIGURATION_FILE_NAME,(const uint8_t*)"rb")) == NULL)
    {
     // MC_CGPS_TRACE(("ERR : LbsPltConfig.cfg File Not Found"));
    }
    else
    {
        // MC_CGPS_TRACE(("INF : LbsPltConfig.cfg File Found"));
        MC_GFL_FREAD(&vg_lbscfg.v_plt, sizeof(char),sizeof( t_lbscfg_Platform ), PlatformConfigFILE);
        vg_lbscfg.v_PlatformFileReadStatus = TRUE;
        MC_GFL_FCLOSE(PlatformConfigFILE);
    }


}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 2

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

uint8_t LBSCFG_PlatformGetConfiguration()
{

    uint8_t retval = TRUE;

    return retval;

}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 3

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

uint8_t LBSCFG_PlatformSetConfiguration()
{

    uint8_t retval = TRUE;

    return retval;


}


