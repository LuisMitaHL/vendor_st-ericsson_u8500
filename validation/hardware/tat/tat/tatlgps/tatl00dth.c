/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides services for power services module
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "tatlgps.h"

int dth_init_service()
{
    int vl_result = 0;

    /*Register DTH tree */
	vl_result = tatlgps_register_dth_struct();

    return vl_result;
}

/* Function tide to GPS self tests */
int DthGps_SelfTest_exec(struct dth_element *elem)
{
  return tatl01_00SelfTest_exec(elem);
}

int DthGps_SelfTest_get(struct dth_element *elem, void *value)
{
  return tatl01_01SelfTest_get(elem, value);
}

int DthGps_SelfTest_set(struct dth_element *elem, void *value)
{
  return tatl01_02SelfTest_set(elem, value);
}
