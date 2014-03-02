/*
* ============================================================================
*
*		Filename:  tatl00test.c
*
*		Description:
*
*       Version:  1.0
*       Created:  MAY 2011
*       Revision:  none
*       Compiler:
*
*       Author:  VDY
*       Copyright ST-Ericsson, 2011. All Rights Reserved
*
* ============================================================================
*/
#define TATLTEST_C
#include "dthtest.h"
#include "tatltest.h"
#undef  TATLTEST_C

/*Init default value for TEST */
/****************************/
int dth_init_service()
{
	int result = 0;
	int i = 0;
	int j = 0;
	result = tatltest_register_dth_struct();
	if (result == 0) {
		/* asynch init */
		if (init_asynchLib() == -1)
			result = -1;
		/* Warning must be freed on an uninit function.*/
		for(i = 0; i<STRINGNB; i++) {
			TString[i] = (char*)calloc(TStringSize[i], sizeof(char));
			for(j=0; j < (TStringSize[i]-1); j++)
			{
				(TString[i])[j] = 'a'+ j%26;
			}
		}
	}
	return result;
}

int Dth_Test_Exec(struct dth_element *elem)
{
	return tatl_TestParam_Exec(elem);
}

int Dth_Test_Set(struct dth_element *elem, void *value)
{
	return tatl_TestParam_Set(elem, value);
}

int Dth_Test_Get(struct dth_element *elem, void *value)
{
	return tatl_TestParam_Get(elem, value);
}

int DthDBAction_exec(struct dth_element *elem)
{
	int result = 0;
	result = tatl1_00ActDBAction_exec(elem);
	return result;
}

int DthDBAction_set(struct dth_element *elem, void *value)
{
	int result = 0;
	result = tatl1_02ActDBAction_Set(elem, value);
	return result;
}

int DthDBAction_get(struct dth_element *elem, void *value)
{
	int result = 0;
	result = tatl1_03ActDBAction_Get(elem, value);
	return result;
}

int DthABAction_exec(struct dth_element *elem)
{
	int result = 0;
	result = tatl1_00ActABAction_exec(elem);
	return result;
}

int DthABAction_set(struct dth_element *elem, void *value)
{
	int result = 0;
	result = tatl1_02ActABAction_Set(elem, value);
	return result;
}

int DthABAction_get(struct dth_element *elem, void *value)
{
	int result = 0;
	result = tatl1_03ActABAction_Get(elem, value);
	return result;
}

int Dth_Test_ExecError(struct dth_element *elem)
{
	int vl_error = 0;
	if(elem->user_data != ERROR_CASE)
		vl_error = Dth_Test_Exec(elem);
	else
		vl_error = EBADRQC;
	return vl_error;
}

int Dth_Test_SetError(struct dth_element *elem, void *value)
{
	int vl_error = 0;
	if(elem->user_data != ERROR_CASE)
		vl_error = Dth_Test_Set(elem, value);
	else
		vl_error = EBADRQC;
	return vl_error;
}

int Dth_Test_GetError(struct dth_element *elem, void *value)
{
	int vl_error = 0;
	if(elem->user_data != ERROR_CASE)
		vl_error = Dth_Test_Get(elem, value);
	else
		vl_error = EBADRQC;
	return vl_error;
}

