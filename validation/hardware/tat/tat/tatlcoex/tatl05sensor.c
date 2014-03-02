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

#define TATL05SENSOR_C
#include "tatlcoex.h"
#undef  TATL05SENSOR_C

char *SensorIdInitError = "Unable to set sensor id" ;
char *SensorInitError = "Unable to init sensor" ;
char *StartPreviewError = "Unable to start preview" ;
char *StopPreviewError = "Unable to stop preview" ;
char *StartMMTEError = "Unable to start MMTE" ;
char *StopMMTEError = "Unable to stop MMTE" ;


/**
 *  Manage primary and secondary sensor to generate traffic on CSI0 and CSI1 buses.
 *  @param[in] displayId id of the display.
 *  @param[in] pwrModeId id of power mode.
 *  @param[in] data data needed to manage display.
 *  @retval 0 success.
 *  @retval errorCode if an error occures while processing.
 */
static int tatl05_04Manage_Sensor(DTH_SENSOR_DATA *data)
{
	int vl_error = 0;
	struct dth_element elem ;

	elem.user_data = 0 ;
	elem.type = DTH_TYPE_U32 ;
	u32 start_mmte = ENUM_VIDEO_CAMERA_STATICCONF_SHOT_START_MMTE;
	u32 stop_mmte = ENUM_VIDEO_CAMERA_STATICCONF_SHOT_STOP_MMTE;
	u32 start_preview = ENUM_VIDEO_CAMERA_STATICCONF_SHOT_START_PREVIEW;
	u32 stop_preview = ENUM_VIDEO_CAMERA_STATICCONF_SHOT_STOP_PREVIEW;

	if ((data->order == START) && (data->state == DEACTIVATED)) {
		/* Set sensor id. */
		vl_error = tatl01_01Set_Video_Parameter(IN_SENSOR_ID, DTH_TYPE_U32, ((void *)&data->sensorId));
		if (vl_error != 0) {
			data->error = SensorIdInitError ;
			goto error ;
		}

		/* Start MMTE */
		vl_error = tatl01_01Set_Video_Parameter(ACT_TAKE_PICTURE, DTH_TYPE_U32, ((void *)&start_mmte));
		if (vl_error != 0) {
			data->error = StartMMTEError ;
			goto error ;
		}
		elem.user_data = ACT_TAKE_PICTURE ;
		vl_error = tatlm1_00ActExec(&elem) ;
		if (vl_error != 0) {
			data->error = StartMMTEError ;
			goto error ;
		}

		/* Init camera */
		elem.user_data = ACT_CAMERA_INIT ;
		vl_error = tatlm1_00ActExec(&elem) ;
		if (vl_error != 0) {
			data->error = SensorIdInitError ;
			goto error ;
		}

		/* Start preview. */
		elem.user_data = ACT_TAKE_PICTURE ;
		vl_error = tatl01_01Set_Video_Parameter(ACT_TAKE_PICTURE, DTH_TYPE_U32, (void *)&start_preview);
		vl_error = tatlm1_00ActExec(&elem) ;
		if (vl_error != 0) {
			data->error = StartPreviewError ;
			goto error ;
		}

		/* Update data. */
		data->state = ACTIVATED ;
		data->error = NO_ERROR ;
	} else if ((data->order == STOP) && (data->state == ACTIVATED)) {
		/* Stop preview. */
		elem.user_data = ACT_TAKE_PICTURE ;
		vl_error = tatl01_01Set_Video_Parameter(ACT_TAKE_PICTURE, DTH_TYPE_U32, (void *)&stop_preview);
		if (vl_error != 0) {
			data->error = StopPreviewError ;
			goto error ;
		}
		vl_error = tatlm1_00ActExec(&elem) ;
		if (vl_error != 0) {
			data->error = StopPreviewError ;
			goto error ;
		}

		/* Stop MMTE */
		elem.user_data = ACT_TAKE_PICTURE ;
		vl_error = tatl01_01Set_Video_Parameter(ACT_TAKE_PICTURE, DTH_TYPE_U32, (void *)&stop_mmte);
		if (vl_error != 0) {
			data->error = StopMMTEError ;
			goto error ;
		}
		vl_error = tatlm1_00ActExec(&elem) ;
		if (vl_error != 0) {
			data->error = StopMMTEError ;
			goto error ;
		}

		/* Update data. */
		data->state = DEACTIVATED ;
		data->error = NO_ERROR ;
	}

error:
	return vl_error ;
}

int tatl05_00Sensor_Exec(struct dth_element *elem)
{
    int vl_error = 0;

	switch (elem->user_data) {
	case ACT_CSI0:
		vl_error =  tatl05_04Manage_Sensor(&v_tatcoex_csi0_data) ;
	break ;

	case ACT_CSI1:
		vl_error =  tatl05_04Manage_Sensor(&v_tatcoex_csi1_data) ;
	break ;

	default:
		vl_error = EBADRQC;
	break ;
	}

	return vl_error ;
}

int tatl05_01Sensor_Get(struct dth_element *elem, void *value)
{
    int vl_error = 0;


	switch (elem->user_data) {
	case ACT_CSI0_STATE:
	{
		*((u8 *)value) = v_tatcoex_csi0_data.state ;
		SYSLOG(LOG_DEBUG, "Get CSI0 state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *)value));
	}
	break ;

	case ACT_CSI0_ERROR:
	{
		strncpy((char *)value, v_tatcoex_csi0_data.error, MAX_STRNG_SIZE);
		SYSLOG(LOG_DEBUG, "Get CSI0 error string: %s\n", (char *) value);
	}
	break ;

	case ACT_CSI1_STATE:
	{
		*((u8 *)value) = v_tatcoex_csi1_data.state ;
		SYSLOG(LOG_DEBUG, "Get CSI1 state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *)value));
	}
	break ;

	case ACT_CSI1_ERROR:
	{
		strncpy((char *)value, v_tatcoex_csi1_data.error, MAX_STRNG_SIZE);
		SYSLOG(LOG_DEBUG, "Get CSI1 error string: %s\n", (char *) value);
	}
	break ;

	default:
		vl_error = EBADRQC;
	break ;
	}

	return vl_error ;

}

int tatl05_02Sensor_Set(struct dth_element *elem, void *value)
{
    int vl_error = 0;

	switch (elem->user_data) {
	case ACT_CSI0:
	{
	    u8 data = *((u8 *)value);
		if (data == START) {
			if ((v_tatcoex_csi1_data.order == STOP) && (v_tatcoex_csi1_data.state == DEACTIVATED))
				v_tatcoex_csi0_data.order = data ;
		} else {
			v_tatcoex_csi0_data.order = data ;
		}
		SYSLOG(LOG_DEBUG, "Set CSI0 order START(0) STOP(1): %i\n", v_tatcoex_csi0_data.order);
	}
	break ;

	case ACT_CSI1:
	{
	    u8 data = *((u8 *)value);
		if (data == START) {
			if ((v_tatcoex_csi0_data.order == STOP) && (v_tatcoex_csi0_data.state == DEACTIVATED))
				v_tatcoex_csi1_data.order = data ;
		} else {
			v_tatcoex_csi1_data.order = data ;
		}
		SYSLOG(LOG_DEBUG, "Set CSI1 order START(0) STOP(1): %i\n", v_tatcoex_csi1_data.order);
	}
	break ;

	default:
		vl_error = EBADRQC;
	break ;
	}

	return vl_error ;

}

void tatl05_03Init_Csi_Data(DTH_SENSOR_DATA *data, u32 sensorId)
{
	data->order = STOP ;
	data->state = DEACTIVATED ;
	data->error = NO_ERROR ;
	data->sensorId = sensorId ;
}

