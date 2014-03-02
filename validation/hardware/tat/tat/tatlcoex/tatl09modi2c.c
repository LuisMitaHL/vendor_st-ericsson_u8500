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

char *StartModI2CError = "Unable to start MODI2C actvity";
char *StopModI2CError = "Unable to stop MODI2C actvity";

/**
 *  Process to START/STOP MODI2C activity.
 */
static void tatl09_04ModI2C_Process()
{
	struct dth_element elem;

	elem.user_data = 0;
	elem.type = DTH_TYPE_U32;
	elem.path = "Start/Stop MODI2C activity";

	if (v_tatcoex_modI2C_data.order == START) {
		if (v_tatcoex_modI2C_data.state == DEACTIVATED) {
			elem.user_data = ACT_DUMP_LOOP_START;
			if (DthSim_Simple_Exec(&elem) != 0) {
				v_tatcoex_modI2C_data.error = StartModI2CError;
			} else {
				v_tatcoex_modI2C_data.error = NO_ERROR;
				v_tatcoex_modI2C_data.state = ACTIVATED;
			}
		}
	} else if (v_tatcoex_modI2C_data.order == STOP) {
		if (v_tatcoex_modI2C_data.state == ACTIVATED) {
			elem.user_data = ACT_DUMP_LOOP_STOP;
			if (DthSim_Simple_Exec(&elem) != 0) {
				v_tatcoex_modI2C_data.error = StopModI2CError;
			} else {
				v_tatcoex_modI2C_data.error = NO_ERROR;
				v_tatcoex_modI2C_data.state = DEACTIVATED;
			}
		}
	}
}

int tatl09_00ModI2C_Exec(struct dth_element *elem)
{
	int vl_error = 0;

	switch (elem->user_data) {
	case ACT_MODI2C:
		tatl09_04ModI2C_Process();
		break;
	}

	return vl_error;
}

int tatl09_01ModI2C_Get(struct dth_element *elem, void *value)
{
	int vl_error = 0;

	switch (elem->user_data) {
	case ACT_MODI2C_STATE:
		*((u8 *) value) = v_tatcoex_modI2C_data.state;
		SYSLOG(LOG_DEBUG, "Get MODI2C state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		break;

	case ACT_MODI2C_ERROR:
		strncpy((char *)value, v_tatcoex_modI2C_data.error, MAX_STRNG_SIZE);
		SYSLOG(LOG_DEBUG, "Get MODI2C error string: %s\n", (char *)value);
		break;

	default:
		vl_error = EBADRQC;
		break;
	}

	return vl_error;

}

int tatl09_02ModI2C_Set(struct dth_element *elem, void *value)
{
	int vl_error;

	vl_error = 0;

	switch (elem->user_data) {
	case ACT_MODI2C:
		v_tatcoex_modI2C_data.order = *((u8 *) value);
		SYSLOG(LOG_DEBUG, "Set MODI2C order START(0) STOP(1): %i\n", v_tatcoex_modI2C_data.order);
		break;

	default:
		vl_error = EBADRQC;
		break;
	}

	return vl_error;

}

void tatl09_03Init_ModI2C_Data()
{
	v_tatcoex_modI2C_data.state = DEACTIVATED;
	v_tatcoex_modI2C_data.order = STOP;
	v_tatcoex_modI2C_data.error = NO_ERROR;
}


