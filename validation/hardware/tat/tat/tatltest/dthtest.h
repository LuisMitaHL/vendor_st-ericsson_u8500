/*
* ==================================
*
*        Filename:  dthtest.h
*
*     Description:
*
*         Version:  1.0
*         Created:  5 may 2011
*        Revision:  none
*        Compiler:
*
*          Author:  VDY,
*         © Copyright ST-Ericsson, 2011. All Rights Reserved
*
* ===================================
*/

#ifndef DTHTEST_H_
#define DTHTEST_H_

#include <dthsrvhelper/dthsrvhelper.h>	/* for dth_element definition */

#if defined(TATLTEST_C)
#define GLOBAL
#else
#define GLOBAL extern
#endif

GLOBAL int dth_init_service();
GLOBAL int Dth_Test_Exec(struct dth_element *elem);
GLOBAL int Dth_Test_Set(struct dth_element *elem, void *value);
GLOBAL int Dth_Test_Get(struct dth_element *elem, void *value);

GLOBAL int DthDBAction_exec(struct dth_element *elem);
GLOBAL int DthDBAction_set(struct dth_element *elem, void *value);
GLOBAL int DthDBAction_get(struct dth_element *elem, void *value);

GLOBAL int DthABAction_exec(struct dth_element *elem);
GLOBAL int DthABAction_set(struct dth_element *elem, void *value);
GLOBAL int DthABAction_get(struct dth_element *elem, void *value);

GLOBAL int Dth_Test_ExecError(struct dth_element *elem);
GLOBAL int Dth_Test_SetError(struct dth_element *elem, void *value);
GLOBAL int Dth_Test_GetError(struct dth_element *elem, void *value);
#undef GLOBAL

#endif /* DTHTEST_H_ */
