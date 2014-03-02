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

#include "tatlsys.h"

int dth_init_service()
{

	int result = TAT_ERROR_OFF;

	result = tatlsys_register_dth_struct();

	return result ;
}

int Dth_ActSYS_exec(struct dth_element *elem)
{
	return tatl1_00ActSYS_exec(elem) ;
}

int Dth_ActSYSParam_Set(struct dth_element *elem, void *value)
{
	return tatl1_02ActSYSParam_Set(elem, value);
}

int Dth_ActSYSParam_Get(struct dth_element *elem, void *value)
{
	return tatl1_03ActSYSParam_Get(elem, value) ;
}

