/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file    dthsim.h
* \brief   declaration of HATS SIM service callbacks for DTH
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef DTHSIM_H_
#define DTHSIM_H_

#include <dthsrvhelper/dthsrvhelper.h> /* for dth_element definition */
#include "hatssim.h"

/* TAT common service initialization */
int dth_init_service();

/* TAT common service finalization */
void dth_uninit_service(void);

/* Function used to check 9P Server */
/************************************/
int DthSim_Check9pServer_get(struct dth_element *elem, void *value);
int DthSim_Check9pServer_set(struct dth_element *elem, void *value);

int DthSim_Get_Parameter(struct dth_element *elem, void *value);
int DthSim_Set_Parameter(struct dth_element *elem, void *value);

/* Function tide to SIM Management */
/*************************************/
int DthSim_Simple_set(struct dth_element *elem, void *Value);
int DthSim_Simple_get(struct dth_element *elem, void *Value);
int DthSim_Simple_Exec(struct dth_element *elem);

/* DTH interface for I2C & UICC Self Test */
/************************************/
int DthSim_SelfTest_set(struct dth_element *elem, void *Value);
int DthSim_SelfTest_get(struct dth_element *elem, void *Value);
int DthSim_SelfTest_Exec(struct dth_element *elem);

enum{
ACT_DUMP_LOOP_START = SMC_ELEMENT_COUNT,
ACT_DUMP_LOOP_STOP
};

#endif /* DTHSIM_H_ */
