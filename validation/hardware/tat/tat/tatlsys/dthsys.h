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

#ifndef DTHSYS_H_
#define DTHSYS_H_

#include <dthsrvhelper/dthsrvhelper.h> /* for dth_element definition */

int dth_init_service();
int Dth_ActSYS_exec(struct dth_element *elem);
int Dth_ActSYSParam_Set(struct dth_element *elem, void *value);
int Dth_ActSYSParam_Get(struct dth_element *elem, void *value);

#endif /* DTHSYS_H_ */
