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

#ifndef DTHADC_H_
#define DTHADC_H_

#include <dthsrvhelper/dthsrvhelper.h> /* for dth_element definition */

int dth_init_service();
int Dth_ActADC_exec(struct dth_element *elem);
int Dth_ActADCParam_Set(struct dth_element *elem, void *value);
int Dth_ActADCParam_Get(struct dth_element *elem, void *value);

int Dth_ActTEST_exec(struct dth_element *elem);
int Dth_ActTESTParam_Set(struct dth_element *elem, void *value);
int Dth_ActTESTParam_Get(struct dth_element *elem, void *value);

#endif /* DTHADC_H_ */
