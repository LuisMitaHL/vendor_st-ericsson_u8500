/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file
* \brief   This module provides routines to manage to manage db dth functions
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef TATL01DBFUNC_H_
#define TATL01DBFUNC_H_

#include "dthdb.h"

typedef struct{
	uint32_t value; /**< Task to be done by the process. */
	uint8_t action; /**< State of the process. */
} DB_REGISTER ;

int tatl1_00ActDB_exec(struct dth_element *elem);
int tatl1_02ActDB_Set (struct dth_element *elem, void *Value);
int tatl1_03ActDB_Get (struct dth_element *elem, void *Value);

#endif /* TATL01DBFUNC_H_ */

