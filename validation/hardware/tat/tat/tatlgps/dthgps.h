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

#ifndef DTHGPS_H_
#define DTHGPS_H_

#include "tatlgpsdth.h"

/* FUNCTIONS (implementation in tatl00dth.c) */

int dth_init_service();

/* Function tide to GPS self tests   */
int DthGps_SelfTest_get(struct dth_element *elem, void *value);
int DthGps_SelfTest_set(struct dth_element *elem, void *value);
int DthGps_SelfTest_exec(struct dth_element *elem);

#endif /* DTHGPS_H_ */
