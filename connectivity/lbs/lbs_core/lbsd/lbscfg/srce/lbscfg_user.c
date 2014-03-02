/*
 * Lbs Config Module
 *
 * lbscfg_user.c
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
*      Interface for write to the User configuration file.
* \returns
*      None
*/

void LBSCFG_UserWriteConfiguration()
{

    t_gfl_FILE * UserConfigFILEUpdate;

    if ((UserConfigFILEUpdate = MC_GFL_FOPEN(LBSCFG_USER_CONFIGURATION_FILE_NAME,(const uint8_t*)"wb")) == NULL)
    {
      //MC_CGPS_TRACE(("ERR : LbsConfig.cfg File Not Found for Updating"));
    }
    else
    {
        //MC_CGPS_TRACE(("INF : LbsConfig.cfg File Found for Updating"));
        MC_GFL_FWRITE(&vg_lbscfg.v_user, 1, sizeof(t_lbscfg_User), UserConfigFILEUpdate);
        MC_GFL_FCLOSE(UserConfigFILEUpdate);
    }

}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 1

/**
* \brief
*      Interface for read from User configuration file.
* \returns
*      None
*/

void LBSCFG_UserReadConfiguration()
{
    t_gfl_FILE * UserConfigFILE;
    vg_lbscfg.v_UserFileReadStatus = FALSE;

    if ((UserConfigFILE = MC_GFL_FOPEN(LBSCFG_USER_CONFIGURATION_FILE_NAME,(const uint8_t*)"rb")) == NULL)
    {
       //MC_CGPS_TRACE(("ERR : LbsConfig.cfg File Not Found"));
    }
    else
    {   
       
       //MC_CGPS_TRACE(("INF : LbsConfig.cfg File Found"));
       MC_GFL_FREAD(&vg_lbscfg.v_user, sizeof(char), sizeof( t_lbscfg_User ), UserConfigFILE);  
       vg_lbscfg.v_UserFileReadStatus = TRUE;
       MC_GFL_FCLOSE(UserConfigFILE);

    }


}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 2

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

uint8_t LBSCFG_UserGetConfiguration()
{

    uint8_t retval = TRUE;

    return retval;


}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 3

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

uint8_t LBSCFG_UserSetConfiguration()
{
    uint8_t retval = TRUE;

    return retval;

}


