/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides routines to manage ab dth functions
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef DTHAUDIO_H_
#define DTHAUDIO_H_

#include <dthsrvhelper/dthsrvhelper.h> /* for dth_element definition */

#if defined HATS_AB_8500_HW
#include "tatl03param8500.h"
#else
#include "tatl03param8520.h"
#endif

/* DTH common service initialization */
int dth_init_service();

/* DTH common service finalization */
void dth_uninit_service(void);

/* Function to anything that is not an audio parameter */
/*************************************/
int DthAudio_simple_get(struct dth_element *elem, void *value);
int DthAudio_simple_set(struct dth_element *elem, void *Value);
int DthAudio_simple_exec(struct dth_element *elem);

/* Function to access audio parameters */
/************************************/
int DthAudio_param_get(struct dth_element *elem, void *value);
int DthAudio_param_set(struct dth_element *elem, void *value);




#endif /* DTHAUDIO_H_ */
