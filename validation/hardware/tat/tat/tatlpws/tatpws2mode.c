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

#include "tatpws.h"

static uint8_t tatpws_Mode;
static uint8_t tatpws_Acc_Name;
static uint8_t tatpws_StatusMode;
static uint8_t tatpws_Mode_Exit_Condition;

extern GpioReg a_GPIO_Config_For_PWS_Mode[TATPWS_NB_GPIO];
extern GpioReg a_GPIO_Config_Initial[TATPWS_NB_GPIO];
extern GpioReg a_AB_GPIO_Config_Initial[TATPWS_NB_AB_GPIOS];
extern GpioReg a_AB_GPIO_Config_For_PWS_Mode[TATPWS_NB_AB_GPIOS];

/* Execution of the action "ACTION_SET_AP_MODE"	,
 * "ACTION_SET_HW_ACC_MODE", "ACTION_SET_MODEM_MODE"*/
/*****************************************************************************/
int tatpws2_SetMode_Execute(struct dth_element *elem)
{
	int vl_Error = TATPWS_NO_ERROR;

	switch (elem->user_data) {
	case ACTION_SET_AP_MODE:
		/* read current states of GPIO except those with PreventReading=1*/
		vl_Error = tatpws1_GPIO_Read(a_GPIO_Config_Initial, TATPWS_NB_GPIO);
		/* Write new states of GPIO (except for DAT & DIR registers wich
		 * are modified by DATS/DATC resp DIRS/DIRC*/
		if (vl_Error == TATPWS_NO_ERROR)
			vl_Error = tatpws1_GPIO_Write(a_GPIO_Config_For_PWS_Mode, TATPWS_NB_GPIO);
		/* read current states of AB8500 GPIO*/
		if (vl_Error == TATPWS_NO_ERROR)
			vl_Error = tatpws4_ABGPIO_Read(a_AB_GPIO_Config_Initial, TATPWS_NB_AB_GPIOS);
		/* Write new states of AB8500 GPIO*/
		if (vl_Error == TATPWS_NO_ERROR)
			vl_Error = tatpws4_ABGPIO_Write(a_AB_GPIO_Config_For_PWS_Mode, TATPWS_NB_AB_GPIOS);
		/* execute power test */
		tatpws_StatusMode = (uint8_t) pwm_set_ap_mode(tatpws_Mode, tatpws_Mode_Exit_Condition);
		PWS_SYSLOG(LOG_DEBUG, "pwm_set_ap_mode(%d, %d)=> %d", tatpws_Mode, tatpws_Mode_Exit_Condition, tatpws_StatusMode);
		/* write back the original states of GPIO
		 * (except for DATS/DATC and DIRS/DIRC) */
		if (vl_Error == TATPWS_NO_ERROR)
			vl_Error = tatpws1_GPIO_Write(a_GPIO_Config_Initial, TATPWS_NB_GPIO);
		/* write back the original states of AB8500 GPIO */
		if (vl_Error == TATPWS_NO_ERROR)
			vl_Error = tatpws4_ABGPIO_Write(a_AB_GPIO_Config_Initial, TATPWS_NB_AB_GPIOS);
		break;

	case ACTION_SET_HW_ACC_MODE:
		tatpws_StatusMode = (uint8_t) pwm_set_hwacc_mode(tatpws_Acc_Name, tatpws_Mode);
		PWS_SYSLOG(LOG_DEBUG, "pwm_set_hwacc_mode(%d, %d)=> %d", tatpws_Acc_Name, tatpws_Mode, tatpws_StatusMode);
		break;

	case ACTION_SET_MODEM_MODE:
		tatpws_StatusMode = (uint8_t) pwm_set_modem_mode(tatpws_Mode);
		PWS_SYSLOG(LOG_DEBUG, "pwm_set_modem_mode(%d)=> %d", tatpws_Mode, tatpws_StatusMode);
		break;

	default:
		vl_Error = TATPWS_ERROR_BAD_REQ;
		PWS_SYSLOG(LOG_DEBUG, "TATPWS_ERROR_BAD_REQ");
		break;
	}
  return vl_Error;
}

/* Result of the action "ACTION_SET_AP_MODE",
 * "ACTION_SET_HW_ACC_MODE", "ACTION_SET_MODEM_MODE"*/
/*****************************************************************************/
int tatpws2_SetMode_Result(struct dth_element *elem, void *Value)
{

	int vl_Error = TATPWS_NO_ERROR;
if (Value == NULL)
		return 0;

	/*Execute the action Action */
	vl_Error = tatpws2_SetMode_Execute(elem);
	PWS_SYSLOG(LOG_DEBUG, "tatpws2_SetMode_Execute has returned=%d", vl_Error);

	return vl_Error;
}

/* Set Callback for Mode Parameter of the action "ACTION_SET_AP_MODE",
 * "ACTION_SET_HW_ACC_MODE", "ACTION_SET_MODEM_MODE"*/
/*****************************************************************************/
int tatpws2_Set_Mode(struct dth_element *elem, void *Value)
{
	int vl_Error = TATPWS_NO_ERROR;
	uint8_t *pl_Mode = (uint8_t *)Value;
if (Value == NULL)
		return 0;

	PWS_SYSLOG(LOG_DEBUG, "Mode=%d", *pl_Mode);
	switch (elem->user_data) {
	case ACTION_SET_AP_MODE:
		switch (*pl_Mode) {
		case ENUM_AP_MODE_NORMAL:
			tatpws_Mode = AP_NORMAL;
			break;
		case ENUM_AP_MODE_EXECUTE:
			tatpws_Mode = AP_EXECUTE;
			break;
		case ENUM_AP_MODE_RUNNING:
			tatpws_Mode = AP_RUNNING;
			break;
		case ENUM_PWS_POWERDOWN_ACTIONSSETAPMODE_SETAPMODE_IN_APMODE_IDLE:
			tatpws_Mode = AP_IDLE;
			break;
		case ENUM_PWS_POWERDOWN_ACTIONSSETAPMODE_SETAPMODE_IN_APMODE_SLEEP:
			tatpws_Mode = AP_SLEEP;
			break;
		case ENUM_PWS_POWERDOWN_ACTIONSSETAPMODE_SETAPMODE_IN_APMODE_DEEP_SLEEP_ESRAM0_RET:
			tatpws_Mode = AP_DEEP_SLEEP_ESRAM0_RET;
			break;
		case ENUM_PWS_POWERDOWN_ACTIONSSETAPMODE_SETAPMODE_IN_APMODE_DEEP_SLEEP_ESRAM0_OFF:
			tatpws_Mode = AP_DEEP_SLEEP_ESRAM0_OFF;
			break;
		default:
			/* Invalid parameter */
			vl_Error = TATPWS_ERROR_BAD_VALUE;
			PWS_SYSLOG(LOG_DEBUG, "TATPWS_ERROR_BAD_VALUE, with ACTION_SET_AP_MODE");
			break;
		}
		break;

	case ACTION_SET_AP_EXIT_CONDITION:
		switch (*pl_Mode) {
		case AP_EXIT_CONDITION_GPIO:
			tatpws_Mode_Exit_Condition = AP_EXIT_BY_GPIO;
			break;
		case AP_EXIT_CONDITION_TIMER:
			tatpws_Mode_Exit_Condition = AP_EXIT_BY_TIMER;
			break;
		default:
			/* Invalid parameter */
			vl_Error = TATPWS_ERROR_BAD_VALUE;
			PWS_SYSLOG(LOG_DEBUG, "TATPWS_ERROR_BAD_VALUE, with ACTION_SET_AP_EXIT_CONDITION");
			break;
		}
		break;

	case ACTION_SET_HW_ACC_MODE:
		switch (*pl_Mode) {
		case ENUM_PWS_POWERDOWN_ACTIONSSETHWACCMODE_SETHWACCMODE_IN_HWACCMODE_ON:
			tatpws_Mode = HWACC_ON;
			break;
		case ENUM_PWS_POWERDOWN_ACTIONSSETHWACCMODE_SETHWACCMODE_IN_HWACCMODE_OFF:
			tatpws_Mode = HWACC_OFF;
			break;
		case ENUM_PWS_POWERDOWN_ACTIONSSETHWACCMODE_SETHWACCMODE_IN_HWACCMODE_RETENTION:
			tatpws_Mode = HWACC_RETENTION;
			break;
		default:
			/* Invalid parameter */
			vl_Error = TATPWS_ERROR_BAD_VALUE;
			PWS_SYSLOG(LOG_DEBUG, "TATPWS_ERROR_BAD_VALUE, with ACTION_HW_ACC_MODE");
			break;
		}
		break;

	case ACTION_SET_MODEM_MODE:
		switch (*pl_Mode) {
		case ENUM_SET_MODEM_MODE_LP_DISABLE:
			tatpws_Mode = MODEM_LP_DISABLED;
			break;
		case ENUM_SET_MODEM_MODE_LP_ENABLE:
			tatpws_Mode = MODEM_LP_ENABLED;
			break;
		default:
			/* Invalid parameter */
			vl_Error = TATPWS_ERROR_BAD_VALUE;
			PWS_SYSLOG(LOG_DEBUG, "TATPWS_ERROR_BAD_VALUE, with ACTION_SET_MODEM_MODE");
			break;
		}
		break;

	default:
		/* Invalid parameter */
		PWS_SYSLOG(LOG_DEBUG, "TATPWS_ERROR_BAD_REQ");
		vl_Error = TATPWS_ERROR_BAD_REQ;
	break;
	}

	return vl_Error;
}

/* Set Callback for HwAccName of the action "ACTION_SET_HW_ACC_MODE"		 */
/*****************************************************************************/
int tatpws2_Set_HwAcc(struct dth_element *elem, void *Value)
{
	int vl_Error = TATPWS_NO_ERROR;
	uint8_t *pl_Name = (uint8_t *)Value;


	PWS_SYSLOG(LOG_DEBUG, "Acc_Name=%d", *pl_Name);
	if ((elem->user_data) != ACTION_SET_HW_ACC_MODE) {
		/* Invalid parameter */
		vl_Error = TATPWS_ERROR_BAD_REQ;
		PWS_SYSLOG(LOG_DEBUG, "TATPWS_ERROR_BAD_REQ");
	} else {

		switch (*pl_Name) {
		case ENUM_PWS_POWERDOWN_ACTIONSSETHWACCMODE_SETHWACCMODE_IN_HWACCNAME_SIAMMDSP:
			tatpws_Acc_Name = SIAMMDSP;
			break;
		case ENUM_PWS_POWERDOWN_ACTIONSSETHWACCMODE_SETHWACCMODE_IN_HWACCNAME_SIAPIPE:
			tatpws_Acc_Name = SIAPIPE;
			break;
		case ENUM_PWS_POWERDOWN_ACTIONSSETHWACCMODE_SETHWACCMODE_IN_HWACCNAME_SVAMMDSP:
			tatpws_Acc_Name = SVAMMDSP;
			break;
		case ENUM_PWS_POWERDOWN_ACTIONSSETHWACCMODE_SETHWACCMODE_IN_HWACCNAME_SVAPIPE:
			tatpws_Acc_Name = SVAPIPE;
			break;
		case ENUM_PWS_POWERDOWN_ACTIONSSETHWACCMODE_SETHWACCMODE_IN_HWACCNAME_SGA:
			tatpws_Acc_Name = SGA;
			break;
		case ENUM_PWS_POWERDOWN_ACTIONSSETHWACCMODE_SETHWACCMODE_IN_HWACCNAME_ESRAM1:
			tatpws_Acc_Name = ESRAM1;
			break;
		case ENUM_PWS_POWERDOWN_ACTIONSSETHWACCMODE_SETHWACCMODE_IN_HWACCNAME_ESRAM2:
			tatpws_Acc_Name = ESRAM2;
			break;
		case ENUM_PWS_POWERDOWN_ACTIONSSETHWACCMODE_SETHWACCMODE_IN_HWACCNAME_ESRAM3:
			tatpws_Acc_Name = ESRAM3;
			break;
		case ENUM_PWS_POWERDOWN_ACTIONSSETHWACCMODE_SETHWACCMODE_IN_HWACCNAME_ESRAM4:
			tatpws_Acc_Name = ESRAM4;
			break;
		case ENUM_PWS_POWERDOWN_ACTIONSSETHWACCMODE_SETHWACCMODE_IN_HWACCNAME_B2R2MCDE:
			tatpws_Acc_Name = B2R2MCDE;
			break;

		default:
			/* Invalid parameter */
			vl_Error = TATPWS_ERROR_BAD_VALUE;
			PWS_SYSLOG(LOG_DEBUG, "TATPWS_ERROR_BAD_VALUE");
			break;
		}
	}

	return vl_Error;
}

/* Get Callback for Status of the action "ACTION_SET_AP_MODE",
 * "ACTION_SET_HW_ACC_MODE", "ACTION_SET_MODEM_MODE"*/
/*****************************************************************************/
int tatpws2_Get_Status(struct dth_element *elem, void *Value)
{
	int vl_Error = TATPWS_NO_ERROR;

	PWS_SYSLOG(LOG_DEBUG, "elem->user_data=%d", elem->user_data);

	switch (elem->user_data) {
	case ACTION_SET_AP_MODE:
	case ACTION_SET_HW_ACC_MODE:
	case ACTION_SET_MODEM_MODE:
		*(uint8_t *)Value = tatpws_StatusMode;
		break;

	default:
		/* Invalid parameter */
		vl_Error = TATPWS_ERROR_BAD_REQ;
		PWS_SYSLOG(LOG_DEBUG, " TATPWS_ERROR_BAD_REQ");
		break;
	}

	return vl_Error;
}




