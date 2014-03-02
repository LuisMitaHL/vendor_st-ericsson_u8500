/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file    tatl00dthsim.c
* \brief   implementation of HATS SIM service routines
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include <dthsim.h>
#include <tatlsim.h>
#include <tatl03misc.h>
#include <tatl05selftest.h>
#include <tatasync.h>

u32 v_tatsim_testsMask = TATSIM_INITIAL_TEST;

extern int v_tatsim_activity;   /* from tatl03misc.c */

/*Init default value for SIM */
/****************************/
int dth_init_service()
{
	int result = 0;

	result = tatlsim_register_dth_struct();

	tatl3_initModule();

	/* make use of tatasync for PPS output update after activation */
	init_asynchLib();

	return result ;

}

void dth_uninit_service(void)
{
    /* deactivate SIM interface if still activated */
    if (TATSIM_SIM_ACTIVATED == v_tatsim_activity)
		tatl3_04StopSimActivity();
}

/* Function tide to SIM Management */
/*************************************/
int DthSim_Simple_set(struct dth_element *elem, void *Value)
{
	return tatl3_03GetSimInfoParam_set(elem, Value);
}

int DthSim_Simple_get(struct dth_element *elem, void *Value)
{
	return tatl3_02GetSimInfoParam_get(elem, Value);
}

int DthSim_Simple_Exec(struct dth_element *elem)
{
	return tatl3_00GetSimInfo_exec(elem);
}

int DthSim_SelfTest_Exec(struct dth_element *elem)
{
	return tatl5_01SelfTest_exec(elem);
}

int DthSim_SelfTest_get(struct dth_element *elem, void *Value)
{
	return tatl5_04SelfTest_get(elem, Value);
}

int DthSim_SelfTest_set(struct dth_element *elem, void *Value)
{
	return tatl5_05SelfTest_set(elem, Value);
}

