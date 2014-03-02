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

char *ApeI2CError = "Unable to use APEI2C";

/**
 *  Process to START/STOP APEI2C activity.
 */
static void tatl10_04ApeI2C_Process(void *threadData)
{
	DTH_APEI2C_DATA *data = (DTH_APEI2C_DATA *) threadData;
	u8 value = 0;

	data->state = ACTIVATED;
	data->error = NO_ERROR;

	while (data->order == START) {
		if (abxxxx_read(AB8500_REVISION_REGISTER, &value) < 0) {
			data->state = DEACTIVATED;
			data->error = ApeI2CError;
			pthread_exit(NULL);
		}
	}

	data->state = DEACTIVATED;
	data->error = NO_ERROR;
	pthread_exit(NULL);
}

int tatl10_00ApeI2C_Exec(struct dth_element *elem)
{
	int vl_error = 0;

	switch (elem->user_data) {
	case ACT_APEI2C:
		vl_error = tatl01_00Manage_Processes(&v_tatcoex_apeI2C_thread, (void *)&v_tatcoex_apeI2C_data,
				(void *)tatl10_04ApeI2C_Process, v_tatcoex_apeI2C_data.order, v_tatcoex_apeI2C_data.state);
		break;
	}

	return vl_error;
}

int tatl10_01ApeI2C_Get(struct dth_element *elem, void *value)
{
	int vl_error = 0;

	switch (elem->user_data) {
	case ACT_APEI2C_STATE:
		*((u8 *) value) = v_tatcoex_apeI2C_data.state;
		SYSLOG(LOG_DEBUG, "Get APEI2C state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		break;

	case ACT_APEI2C_ERROR:
		strncpy((char *)value, v_tatcoex_apeI2C_data.error, MAX_STRNG_SIZE);
		SYSLOG(LOG_DEBUG, "Get APEI2C error string: %s\n", (char *)value);
		break;

	default:
		vl_error = EBADRQC;
		break;
	}

	return vl_error;

}

int tatl10_02ApeI2C_Set(struct dth_element *elem, void *value)
{
	int vl_error = 0;

	switch (elem->user_data) {
	case ACT_APEI2C:
		v_tatcoex_apeI2C_data.order = *((u8 *) value);
		SYSLOG(LOG_DEBUG, "Set APEI2C order START(0) STOP(1): %i\n", v_tatcoex_apeI2C_data.order);
		break;

	default:
		vl_error = EBADRQC;
		break;
	}

	return vl_error;

}

void tatl10_03Init_ApeI2C_Data()
{
	v_tatcoex_apeI2C_data.state = DEACTIVATED;
	v_tatcoex_apeI2C_data.order = STOP;
	v_tatcoex_apeI2C_data.error = NO_ERROR;
}


