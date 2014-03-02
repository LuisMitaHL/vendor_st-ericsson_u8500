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

#ifndef TATL01SYSFUNC_H_
#define TATL01SYSFUNC_H_

#define SYS_DELAY 1

int tatl1_00ActSYS_exec(struct dth_element *elem);
int tatl1_02ActSYSParam_Set(struct dth_element *elem, void *Value);
int tatl1_03ActSYSParam_Get(struct dth_element *elem, void *Value);

#endif /* TATL01SYSFUNC_H_ */
