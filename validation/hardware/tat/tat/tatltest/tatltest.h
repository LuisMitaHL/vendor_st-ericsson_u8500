/*
* ============================================================================
*
*       Filename:  tatltest.h
*
*		Description:
*
*       Version:  1.0
*       Created:  27/08/2009
*       Revision:  none
*       Compiler:
*
*       Author: GVO,
*       Copyright ST-Ericsson, 2009. All Rights Reserved
*
* ============================================================================
*/

#ifndef TATLTEST_H_
#define TATLTEST_H_

#include "tatl02testgen.h"

#include "tatl01testfunc.h"
#include "tatasync.h"
#define SYSLOG					TEST_SYSLOG
#define SYSLOGLOC				TEST_SYSLOGLOC
#define SYSLOGSTR				TEST_SYSLOGSTR

typedef struct {
	uint8_t value; /**< Task to be done by the process. */
	uint8_t action;	/**< State of the process. */
	int address; /**< Task to be done by the process. */
} AB_ACTION_REGISTER;

typedef struct {
	uint32_t value;	/**< Task to be done by the process. */
	uint8_t action;	/**< State of the process. */
	int address; /**< Task to be done by the process. */
} DB_ACTION_REGISTER;

int tatl1_00ActABAction_exec(struct dth_element *elem);
int tatl1_02ActABAction_Set(struct dth_element *elem, void *Value);
int tatl1_03ActABAction_Get(struct dth_element *elem, void *Value);

int tatl1_00ActDBAction_exec(struct dth_element *elem);
int tatl1_02ActDBAction_Set(struct dth_element *elem, void *Value);
int tatl1_03ActDBAction_Get(struct dth_element *elem, void *Value);

#endif /* TATLTEST_H_ */
