/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   check9p.c
* \brief   This module provides API to manage ISI link and modem
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "rfdef.h"
#include "check9p.h"

/* GLOBALS */

/* check 9p server input */
u32 v_tatrf_Check9pServeur = 0U;

/* Function used to check 9P Server */
/************************************/
int DthRf_Check9pServer_get(struct dth_element *elem, void *value)
{
	return tatl4_04Check9pServer_get(elem, value);
}

int DthRf_Check9pServer_set(struct dth_element *elem, void *value)
{
	return tatl4_05Check9pServer_Set(elem, value);
}

/******************************************************************/
/* 9p server check function                                       */
/******************************************************************/
int tatl4_03Check9pServer_exec(struct dth_element *elem)
{
    (void) elem;

	return TAT_ERROR_OFF;
}

int tatl4_04Check9pServer_get(struct dth_element *elem, void *Value)
{
	u32 vl_Error = TAT_ERROR_OFF;
	u32 *pl_Value = (u32 *) Value;

	vl_Error = tatl4_03Check9pServer_exec(elem);
	if (vl_Error == TAT_ERROR_OFF)
		*pl_Value = v_tatrf_Check9pServeur;

	return vl_Error;
}

int tatl4_05Check9pServer_Set(struct dth_element *elem, void *Value)
{
	u32 vl_Error = TAT_ERROR_OFF;
	u32 *pl_Value = (u32 *) Value;

    (void) elem;

	v_tatrf_Check9pServeur = *pl_Value;

	return vl_Error;
}

