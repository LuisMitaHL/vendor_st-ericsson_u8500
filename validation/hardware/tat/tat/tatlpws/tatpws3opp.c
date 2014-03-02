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

static uint8_t tatpws_Opp;
static uint8_t tatpws_StatusOpp;

/* Execution of the action "ACTION_SET_APE_OPP","ACTION_SET_ARM_OPP",
 * "ACTION_SET_DDR_OPP", "ACTION_SET_MODEM_OPP"*/
/******************************************************************************/
int tatpws3_SetOpp_Execute(struct dth_element *elem)
{
	int vl_Error = TATPWS_NO_ERROR;

	switch (elem->user_data) {
	case ACTION_SET_APE_OPP:
		tatpws_StatusOpp = (uint8_t) pwm_set_ape_opp(tatpws_Opp);
		PWS_SYSLOG(LOG_DEBUG,
				"pwm_set_ape_opp(%d)=> %d", tatpws_Opp, tatpws_StatusOpp);
		break;

	case ACTION_SET_ARM_OPP:
		tatpws_StatusOpp = (uint8_t) pwm_set_arm_opp(tatpws_Opp);
		PWS_SYSLOG(LOG_DEBUG,
					"pwm_set_arm_opp(%d)=> %d", tatpws_Opp, tatpws_StatusOpp);
		break;

	case ACTION_SET_DDR_OPP:
		tatpws_StatusOpp = (uint8_t) pwm_set_ddr_opp(tatpws_Opp);
		PWS_SYSLOG(LOG_DEBUG,
				"pwm_set_ddr_opp(%d)=> %d", tatpws_Opp, tatpws_StatusOpp);
		break;

	case ACTION_SET_MODEM_OPP:
		tatpws_StatusOpp = (uint8_t) pwm_set_modem_opp(tatpws_Opp);
		PWS_SYSLOG(LOG_DEBUG,
				"pwm_set_modem_opp(%d)=> %d", tatpws_Opp, tatpws_StatusOpp);
		break;

	default:
		vl_Error = TATPWS_ERROR_BAD_REQ;
		PWS_SYSLOG(LOG_DEBUG, "TATPWS_ERROR_BAD_REQ");
		break;
	}
  return vl_Error;
}

/* Result of the action "ACTION_SET_APE_OPP","ACTION_SET_ARM_OPP",
 *"ACTION_SET_DDR_OPP", "ACTION_SET_MODEM_OPP"*/
/*****************************************************************************/
int tatpws3_SetOpp_Result(struct dth_element *elem, void *Value)
{

	int vl_Error;
if (Value == NULL)
		return 0;

	vl_Error = TATPWS_NO_ERROR;

	/*Execute the action Action */
	vl_Error = tatpws3_SetOpp_Execute(elem);
	PWS_SYSLOG(LOG_DEBUG, "tatpws3_SetOpp_Execute has returned=%d", vl_Error);

	return vl_Error;
}

/* Set Callback for Opp Parameter of the action "ACTION_SET_APE_OPP",
 * "ACTION_SET_ARM_OPP", "ACTION_SET_DDR_OPP", "ACTION_SET_MODEM_OPP"*/
/*****************************************************************************/
int tatpws3_Set_Opp(struct dth_element *elem, void *Value)
{
	int vl_Error = TATPWS_NO_ERROR;
	uint8_t *pl_Mode = (uint8_t *)Value;

	PWS_SYSLOG(LOG_DEBUG, "Operating Point=%d", *pl_Mode);
	switch (elem->user_data) {
	case ACTION_SET_APE_OPP:
		switch (*pl_Mode) {
		case ENUM_PWS_50_IF_USB_UNPLUGGED:
			tatpws_Opp = APE_OPP50;
			break;
		case ENUM_PWS_POWERDOWN_ACTIONSSETAPEOPP_SETAPEOPERATINGPOINT_IN_OPP_100_:
			tatpws_Opp = APE_OPP100;
			break;
		default:
			/* Invalid parameter */
			vl_Error = TATPWS_ERROR_BAD_VALUE;
			PWS_SYSLOG(LOG_DEBUG, "TATPWS_ERROR_BAD_VALUE");
			break;
		}
		break;
	case ACTION_SET_ARM_OPP:
		switch (*pl_Mode) {
		case ENUM_PWS_POWERDOWN_ACTIONSSETARMOPP_SETARMOPERATINGPOINT_IN_OPP_30_:
			tatpws_Opp = ARM_OPP30;
			break;
		case ENUM_PWS_POWERDOWN_ACTIONSSETARMOPP_SETARMOPERATINGPOINT_IN_OPP_50_:
			tatpws_Opp = ARM_OPP50;
			break;
		case ENUM_PWS_POWERDOWN_ACTIONSSETARMOPP_SETARMOPERATINGPOINT_IN_OPP_100_:
			tatpws_Opp = ARM_OPP100;
			break;
		case ENUM_PWS_POWERDOWN_ACTIONSSETARMOPP_SETARMOPERATINGPOINT_IN_OPP_125_:
			tatpws_Opp = ARM_OPP125;
			break;
		default:
			/* Invalid parameter */
			vl_Error = TATPWS_ERROR_BAD_VALUE;
			PWS_SYSLOG(LOG_DEBUG, "TATPWS_ERROR_BAD_VALUE");
			break;
		}
		break;
	case ACTION_SET_DDR_OPP:
		switch (*pl_Mode) {
		case ENUM_PWS_POWERDOWN_ACTIONSSETDDROPP_SETDDROPERATINGPOINT_IN_OPP_25_:
			tatpws_Opp = DDR_OPP25;
			break;
		case ENUM_PWS_POWERDOWN_ACTIONSSETDDROPP_SETDDROPERATINGPOINT_IN_OPP_50_:
			tatpws_Opp = DDR_OPP50;
			break;
		case ENUM_PWS_POWERDOWN_ACTIONSSETDDROPP_SETDDROPERATINGPOINT_IN_OPP_100_:
			tatpws_Opp = DDR_OPP100;
			break;
		default:
			/* Invalid parameter */
			vl_Error = TATPWS_ERROR_BAD_VALUE;
			PWS_SYSLOG(LOG_DEBUG, "TATPWS_ERROR_BAD_VALUE");
			break;
		}
		break;

	case ACTION_SET_MODEM_OPP:
		switch (*pl_Mode) {
		case ENUM_PWS_POWERDOWN_ACTIONSSETMODEMOPP_SETMODEMOPERATINGPOINT_IN_OPP_50_:
			tatpws_Opp = MODEM_OPP50;
			break;
		case ENUM_PWS_POWERDOWN_ACTIONSSETMODEMOPP_SETMODEMOPERATINGPOINT_IN_OPP_100_:
			tatpws_Opp = MODEM_OPP100;
			break;
		default:
			/* Invalid parameter */
			vl_Error = TATPWS_ERROR_BAD_VALUE;
			PWS_SYSLOG(LOG_DEBUG, "TATPWS_ERROR_BAD_VALUE");
			break;
		}
		break;
	default:
		/* Invalid parameter */
		vl_Error = TATPWS_ERROR_BAD_REQ;
		PWS_SYSLOG(LOG_DEBUG, "TATPWS_ERROR_BAD_REQ");
		break;
	}

	return vl_Error;
}

/* Get Callback for Status of the action "ACTION_SET_APE_OPP",
 * "ACTION_SET_ARM_OPP", "ACTION_SET_DDR_OPP", "ACTION_SET_MODEM_OPP"*/
/*****************************************************************************/
int tatpws3_Get_Status(struct dth_element *elem, void *Value)
{
	int vl_Error = TATPWS_NO_ERROR;

	PWS_SYSLOG(LOG_DEBUG, "elem->user_data=%d", elem->user_data);

	switch (elem->user_data) {
	case ACTION_SET_APE_OPP:
	case ACTION_SET_ARM_OPP:
	case ACTION_SET_DDR_OPP:
	case ACTION_SET_MODEM_OPP:
		*(uint8_t *)Value = tatpws_StatusOpp;
	break;

	default:
		/* Invalid parameter */
		vl_Error = TATPWS_ERROR_BAD_REQ;
		PWS_SYSLOG(LOG_DEBUG, "TATPWS_ERROR_BAD_REQ");
		break;
	}

	return vl_Error;
}

