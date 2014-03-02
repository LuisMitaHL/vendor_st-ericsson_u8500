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

#include "tatlcharge.h"

int dth_init_service()
{

	int result = TAT_ERROR_OFF;

	result = tatlcharge_register_dth_struct();

	return result ;
}

int Dth_ActCHARGE_exec(struct dth_element *elem)
{
	return tatl1_00ActCHARGE_exec(elem) ;
}

int Dth_ActCHARGEParam_Set(struct dth_element *elem, void *value)
{
	return tatl1_02ActCHARGEParam_Set(elem, value);
}

int Dth_ActCHARGEParam_Get(struct dth_element *elem, void *value)
{
	return tatl1_03ActCHARGEParam_Get(elem, value) ;
}

