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

#ifndef TATL01CHARGEFUNC_H_
#define TATL01CHARGEFUNC_H_

#define CHARGE_SYSFS_PATH "/sys/ab8500_chargalg/chargalg"

enum{
	MT_BATTOKFALLING,
	MT_OTPBATTOKF,
	MT_STR_OTP,
	MT_AUTOMAINCHCURR,
	MT_AUTOVBUSCHCURR,
	MT_USBLINKSTATUS,
	MT_CHVOLTLEVEL,
	MT_ALGOSTATUS,
};

typedef struct {
	char   regvalue;
	float  value;
} Match_Table_float;

typedef struct {
	char   regvalue;
	char *str;
} Match_Table_string;

int tatl1_00ActCHARGE_exec(struct dth_element *elem);
int tatl1_02ActCHARGEParam_Set(struct dth_element *elem, void *Value);
int tatl1_03ActCHARGEParam_Get(struct dth_element *elem, void *Value);

#endif /* TATL01CHARGEFUNC_H_ */
