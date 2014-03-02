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

/*---------------------------------------------------------------------------*
 * Procedure    : tatl1_00ActSYS_exec
 *---------------------------------------------------------------------------*
 * Description  : Execute the Cmd Action sent by DTH
 *---------------------------------------------------------------------------*
 * Parameter(s) : dth_element *, the  DTH element
 *---------------------------------------------------------------------------*
 * Return Value : u32, TAT error
 *---------------------------------------------------------------------------*/
int tatl1_00ActSYS_exec(struct dth_element *elem)
{
	int vl_Error = TAT_ERROR_OFF;

	switch (elem->user_data) {

	case ACT_POWEROFF:
	  {
    char command[100];

    memset(command,0,sizeof(command));
    sprintf(command,"poweroff -d %d &\n", SYS_DELAY);
    printf(command);
    system(command);	
    }  
	  break;
    
	case ACT_REBOOT:
	  {
    char command[100];

    memset(command,0,sizeof(command));
    sprintf(command,"reboot -d %d &\n", SYS_DELAY);
    printf(command);
    system(command);	 	
    }
		break;

	default:
		printf("tatl1_00ActSYS_exec USERDATA not AVAILABLE\n");
		vl_Error = TAT_ERROR_CASE;
		break;
	}
	return vl_Error;
}

/*----------------------------------------------------------------------------*
 * Procedure    : tatl1_02ActSYSParam_Set
 *----------------------------------------------------------------------------*
 * Description  : Set the DTH Element Value Input Parameters
 *----------------------------------------------------------------------------*
 * Parameter(s) : dth_element *, Structure of DTH element
 *		  void *, Value of Dth Element
 *----------------------------------------------------------------------------*
 * Return Value : u32, TAT error
 *---------------------------------------------------------------------------*/
int tatl1_02ActSYSParam_Set(struct dth_element *elem, void *Value)
{
	int vl_Error = TAT_ERROR_OFF;

	return vl_Error;
}

/*----------------------------------------------------------------------------*
 * Procedure    : tatl1_03ActSYSParam_Get
 *----------------------------------------------------------------------------*
 * Description  : Get the DTH Element Value Output Parameters
 *----------------------------------------------------------------------------*
 * Parameter(s) : dth_element *, Structure of DTH element
 *		  void *, Value of Dth Element
 *----------------------------------------------------------------------------*
 * Return Value : u32, TAT error
 *---------------------------------------------------------------------------*/
int tatl1_03ActSYSParam_Get(struct dth_element *elem, void *Value)
{

	int vl_Error = TAT_ERROR_OFF;

	return vl_Error;
}

