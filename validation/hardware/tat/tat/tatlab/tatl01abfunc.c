/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file
* \brief   This module provides routines to manage ab dth functions
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "tatlab.h"

#define AB_READ 0

AB_REGISTER register_data={0, 0};

/*---------------------------------------------------------------------------------*
 * Procedure    : tatl1_00ActAB_exec
 *----------------------------------------------------------------------------*
 * Description  : Execute the Cmd Action sent by DTH
 *----------------------------------------------------------------------------*
 * Parameter(s) : dth_element *, the  DTH element
 *----------------------------------------------------------------------------*
 * Return Value : u32, TAT error
 *---------------------------------------------------------------------------*/
int tatl1_00ActAB_exec(struct dth_element *elem)
{
	int vl_Error = 0;
	int reg_addr = 0, reg_value = 0;
	if (((uint32_t)elem->user_data) <= 0xFFFFFFFF) {
		if (register_data.action == AB_READ) {
			/* Read register */
			reg_addr = elem->user_data;
			printf("Will read 0x%04x register\n", reg_addr);
			vl_Error = abxxxx_read(reg_addr, &register_data.value);
			/* Get output value */
			printf("Read : 0x%02x in 0x%04x\n", register_data.value, reg_addr);
		} else {
			/* Write register */
			reg_addr = elem->user_data;
			reg_value = register_data.value;
			printf("Write 0x%02x in 0x%04x\n", reg_value, reg_addr);
			vl_Error = abxxxx_write(reg_addr, register_data.value);
		}
	} else {
		printf("tatl1_00ActAB_exec USERDATA not AVAILABLE\n");
		vl_Error = 1;
	}
	return vl_Error;
}

/*----------------------------------------------------------------------------*
 * Procedure    : tatl1_02ActABParam_Set
 *----------------------------------------------------------------------------*
 * Description  : Set the DTH Element Value Input Parameters
 *----------------------------------------------------------------------------*
 * Parameter(s) : dth_element *, Structure of DTH element
 *		  void *, Value of Dth Element
 *----------------------------------------------------------------------------*
 * Return Value : u32, TAT error
 *---------------------------------------------------------------------------*/
int tatl1_02ActAB_Set(struct dth_element *elem, void *Value)
{
	int vl_Error = 0;
	switch (elem->type) {
	case DTH_TYPE_U8:
		register_data.action = *(uint8_t *) Value;
		/*printf("Action Value = %i\n", *(uint8_t *) Value);*/
		break;
	case DTH_TYPE_U8B:
		register_data.value = *(uint8_t *) Value;
		/*printf("Bitfield Value = 0x%x\n", *(uint8_t *) Value);*/
		break;
	default:
		printf("tatl1_02ActABParam_Set TYPE not AVAILABLE\n");
		vl_Error = -6;
		break;
	}
	return vl_Error;

}

/*----------------------------------------------------------------------------*
 * Procedure    : tatl1_03ActABParam_Get
 *----------------------------------------------------------------------------*
 * Description  : Get the DTH ELement Value Output Parameters
 *----------------------------------------------------------------------------*
 * Parameter(s) : dth_element *, Structure of DTH element
 *		  void *, Value of Dth Element
 *----------------------------------------------------------------------------*
 * Return Value : u32, TAT error
 *---------------------------------------------------------------------------*/
int tatl1_03ActAB_Get(struct dth_element *elem, void *Value)
{
	int vl_Error = 0;

	switch (elem->type) {
	case DTH_TYPE_U8B:
		*((uint8_t *) Value) = register_data.value;
		/*printf("Bitfield Value = 0x%x\n", *(uint8_t *) Value);*/
		break;
	default:
		printf("tatl1_03ActABParam_Get TYPE not AVAILABLE\n");
		vl_Error = -8;
		break;
	}
	return vl_Error;
}
