/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file
* \brief   This module provides routines to manage to manage ab dth functions
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef TATL01ABFUNC_H_
#define TATL01ABFUNC_H_

#include "dthab.h"

typedef struct{
	uint8_t value; /**< Task to be done by the process. */
	uint8_t action; /**< State of the process. */
} AB_REGISTER ;

int tatl1_00ActAB_exec(struct dth_element *elem);
int tatl1_02ActAB_Set (struct dth_element *elem, void *Value);
int tatl1_03ActAB_Get (struct dth_element *elem, void *Value);

#endif /* TATL01ABFUNC_H_ */

