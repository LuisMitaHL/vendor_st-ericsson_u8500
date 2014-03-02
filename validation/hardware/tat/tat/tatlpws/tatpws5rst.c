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

static uint8_t tatpws_Reset_Mode;
static uint8_t tatpws_Modem_Reset_Status;
static uint8_t tatpws_APE_Reset_Status;

/* Main Function of the thread which manage the sw AP reset execution	*/
/************************************************************************/
static void tatpws5_Async_Reset_Fct()
{
	PWS_SYSLOG(LOG_ERR, "-->> New Thread For RESET: sleep 2s for response PC panel......");
	sleep(2);

	PWS_SYSLOG(LOG_ERR, "-->> New Thread For RESET: ready to execute \"pwm_reset_ap(%u)\"", tatpws_Reset_Mode);
	tatpws_APE_Reset_Status = pwm_reset_ap(tatpws_Reset_Mode);

	pthread_exit(NULL);
}

/* Execution of the action "ACTION_RESET_AP", "ACTION_RESET_MODEM" */
/*******************************************************************/
int tatpws5_Reset_Execute(struct dth_element *elem)
{
	int vl_Error = TATPWS_NO_ERROR;

	pthread_t tatpws_sw_APE_thread;
	pthread_attr_t thread_attr;
	vl_Error = pthread_attr_init(&thread_attr);

	if (vl_Error != 0) {
		PWS_SYSLOG(LOG_ERR, "pthread_attr_init: %s\n", strerror(vl_Error));
		return vl_Error ;
	}

	vl_Error = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	if (vl_Error != 0)  {
		PWS_SYSLOG(LOG_ERR, "pthread_attr_setdetachstate: %s\n", strerror(vl_Error));
		return vl_Error ;
	}

	switch(elem->user_data){

	case ACTION_RESET_AP:

		/* there will no chance to feedback OK to PC if RESET is OK */
		/* so we set OK firstly */
		tatpws_APE_Reset_Status = AP_RESET_OK;

		/* for SW_APE_RESET, the software will stop & never
		*  resume, as the A9 cannot release the reset.
		*  So , in order to have the acknowledge of the DTH
		*  action execution, launch a thread to launch a Sw APE Reset
		*  if the flag "tatpws_sw_APE_Reset" is set */
		vl_Error = pthread_create(&tatpws_sw_APE_thread,
					&thread_attr,
					(void *)tatpws5_Async_Reset_Fct,
					NULL);

		if (vl_Error) {
			PWS_SYSLOG(LOG_DEBUG, "pthread_create error=%d", vl_Error);
			vl_Error = TATPWS_ERROR_EXEC;
		}
	
		break;

	case ACTION_RESET_MODEM:

		tatpws_Modem_Reset_Status = pwm_reset_modem(tatpws_Reset_Mode);
		
		PWS_SYSLOG(LOG_DEBUG, "pwm_reset_modem(%d)=> %d",
			tatpws_Reset_Mode, tatpws_Modem_Reset_Status);
	
		break;

	default:
		/* Invalid parameter */
		vl_Error = TATPWS_ERROR_BAD_VALUE;
		PWS_SYSLOG(LOG_DEBUG, "TATPWS_ERROR_BAD_VALUE");
		break;
	}

	return vl_Error;
}

/* Set Callback for Mode Parameter of the action "ACTION_RESET_AP",
 * "ACTION_RESET_MODEM"	*/
/*****************************************************************************/
int tatpws5_Set_ResetMode(struct dth_element *elem, void *Value)
{
	int vl_Error = TATPWS_NO_ERROR;
	uint8_t *pl_Mode = (uint8_t *) Value;

	PWS_SYSLOG(LOG_DEBUG, "Reset Mode=%d", *pl_Mode);
	switch (elem->user_data) {
	case ACTION_RESET_AP:
		switch (*pl_Mode) {
		case ENUM_PWS_RESET_ACTIONRESETAP_RESETAP_IN_RESETMODE_SW_APE_RESET:
			tatpws_Reset_Mode = SW_APE_RESET;
			break;
		case ENUM_PWS_RESET_ACTIONRESETAP_RESETAP_IN_RESETMODE_SW_ARM_RESET:
			tatpws_Reset_Mode = SW_ARM_RESET;
			break;
		case ENUM_PWS_RESET_ACTIONRESETAP_RESETAP_IN_RESETMODE_WD_ARM_RESET:
			tatpws_Reset_Mode = WD_ARM_RESET;
			break;
		case ENUM_PWS_RESET_ACTIONRESETAP_RESETAP_IN_RESETMODE_AB8500_WD_RESET:
			tatpws_Reset_Mode = AB8500_WD_RESET;
			break;

		default:
			/* Invalid parameter */
			vl_Error = TATPWS_ERROR_BAD_VALUE;
			PWS_SYSLOG(LOG_DEBUG, "TATPWS_ERROR_BAD_VALUE");
			break;
		}
		break;

	case ACTION_RESET_MODEM:

		switch (*pl_Mode) {
		case ENUM_PWS_RESET_ACTIONRESETMODEM_RESETMODEM_IN_RESETMODE_SW_RESET:
			tatpws_Reset_Mode = MODEM_SW_RESET;
			break;
		case ENUM_PWS_RESET_ACTIONRESETMODEM_RESETMODEM_IN_RESETMODE_WD_RESET:
			tatpws_Reset_Mode = MODEM_WD_RESET;
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

/* Get Callback for Status of the action "ACTION_RESET_AP",
 * "ACTION_RESET_MODEM"*/
/*****************************************************************************/
int tatpws5_Get_Status(struct dth_element *elem, void *Value)
{
	int vl_Error = TATPWS_NO_ERROR;

	switch(elem->user_data){

	case ACTION_RESET_AP:
		*(uint8_t *) Value = tatpws_APE_Reset_Status;
	
		PWS_SYSLOG(LOG_DEBUG, "elem->user_data=%d status=%d",
					elem->user_data,
					tatpws_APE_Reset_Status);
		break;
	
	case ACTION_RESET_MODEM:
		*(uint8_t *) Value = tatpws_Modem_Reset_Status;
	
		PWS_SYSLOG(LOG_DEBUG, "elem->user_data=%d status=%d",
					elem->user_data,
					tatpws_Modem_Reset_Status);
		break;

	default:
		/* Invalid parameter */
		vl_Error = TATPWS_ERROR_BAD_VALUE;
		PWS_SYSLOG(LOG_DEBUG, "TATPWS_ERROR_BAD_VALUE");
		break;
	}

	return vl_Error;
}


