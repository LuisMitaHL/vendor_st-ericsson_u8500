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

#include "dthhdmi.h"
#include "tatlhdmi.h"

int dth_init_service()
{

  int result = TAT_ERROR_OFF;

  result = tatlhdmi_register_dth_struct();

  return result;

}

int Dth_ActHDMI_exec(struct dth_element *elem)
{
  return tatlx1_00ActExec(elem);
}

int Dth_ActHDMIParam_Set(struct dth_element *elem, void *value)
{
  return tatlx1_01ActSet(elem, value);
}

int Dth_ActHDMIParam_Get(struct dth_element *elem, void *value)
{
  return tatlx1_02ActGet(elem, value);
}

