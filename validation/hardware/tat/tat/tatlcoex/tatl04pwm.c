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

#include "tatlcoex.h"

char *PwmError = "Enable to generate traffic on PWM bus" ;

/**
 *  Set dynamically input parameter from tatlvideo library.
 *  @param[in] id id of the parameter to be set.
 *  @param[in] type DTH type of the parameter to be set.
 *  @param[in] value new value of the parameter.
 *  @retval 0 success.
 *  @retval errorCode if an error occures while processing.
 */
static int tatl04_04Set_Video_Dynamic_Parameter(int id, int type, void *value)
{
	int vl_error = 0;
	struct dth_element elem ;

	elem.type = type ;
	elem.user_data = id ;

	vl_error = Dth_ElemVIDEOParam_Set(&elem, value);

	return  vl_error ;
}

/**
 *  Perform continuous writing accesses on backlights via PWM buses.
 *	@param[in] threadData data needed for PWM processing.
 */
static void tatl04_05Pwm_Process(void *threadData)
{
	DTH_PWM_PROCESS_DATA *pwmData ;
	int vl_error = 0;
	u32 value = 0;

	pwmData = (DTH_PWM_PROCESS_DATA *) threadData ;

	pwmData->state = ACTIVATED ;
	while (pwmData->order == START) {
		/* Set backlight value to 255. */
		value = 255 ;
		vl_error = tatl04_04Set_Video_Dynamic_Parameter(pwmData->pwmId, DTH_TYPE_U32, ((void *)&value));
		if (vl_error != 0) {
			pwmData->error = PwmError ;
			pwmData->state = DEACTIVATED ;
			pthread_exit(NULL);
		}

		usleep(500);

		/* Set backlight value to 0. */
		value = 0 ;
		vl_error = tatl04_04Set_Video_Dynamic_Parameter(pwmData->pwmId, DTH_TYPE_U32, ((void *)&value));
		if (vl_error != 0) {
			pwmData->error = PwmError ;
			pwmData->state = DEACTIVATED ;
			pthread_exit(NULL);
		}
		pwmData->error = NO_ERROR ;
	}

	/* Set backlight value to 0. */
    value = 0 ;
	vl_error = tatl04_04Set_Video_Dynamic_Parameter(pwmData->pwmId, DTH_TYPE_U32, ((void *)&value));
	if (vl_error != 0) {
		pwmData->error = PwmError ;
		pwmData->state = DEACTIVATED ;
		pthread_exit(NULL);
	}

	pwmData->error = NO_ERROR ;
	pwmData->state = DEACTIVATED ;
	pthread_exit(NULL);
}

int tatl04_00Pwm_Exec(struct dth_element *elem)
{
    int vl_error = 0;

	switch (elem->user_data) {
	case ACT_PWM1:
		vl_error = tatl01_00Manage_Processes(&v_tatcoex_pwm1_thread, (void *)&v_tatcoex_pwm1_data, (void*)tatl04_05Pwm_Process, v_tatcoex_pwm1_data.order, v_tatcoex_pwm1_data.state);
	break ;

	default:
		vl_error = EBADRQC;
	break ;
	}

	return vl_error ;
}

int tatl04_01Pwm_Get(struct dth_element *elem, void *value)
{
    int vl_error = 0 ;

	switch (elem->user_data) {
	case ACT_PWM1_STATE:
	{
		*((u8 *)value) = v_tatcoex_pwm1_data.state ;
		SYSLOG(LOG_DEBUG, "Get PWM1 state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *)value));
	}
	break ;

	case ACT_PWM1_ERROR:
	{
		strncpy((char *)value, v_tatcoex_pwm1_data.error, MAX_STRNG_SIZE);
		SYSLOG(LOG_DEBUG, "Get PWM1 error string: %s\n", (char *) value);
	}
	break ;

	default:
		vl_error = EBADRQC;
	break ;
	}

	return vl_error ;

}


int tatl04_02Pwm_Set(struct dth_element *elem, void *value)
{
    int vl_error = 0;

	switch (elem->user_data) {
	case ACT_PWM1:
	{
		v_tatcoex_pwm1_data.order = *((u8 *)value);
		SYSLOG(LOG_DEBUG, "Set PWM1 order START(0) STOP(1): %i\n", v_tatcoex_pwm1_data.order);
	}
	break ;

	default:
		vl_error = EBADRQC;
	break ;
	}

	return vl_error ;

}

void tatl04_03Init_Pwm_Data(DTH_PWM_PROCESS_DATA *data, u32 pwmId)
{
	data->order = STOP ;
	data->state = DEACTIVATED ;
	data->error = NO_ERROR ;
	data->pwmId = pwmId ;
}

