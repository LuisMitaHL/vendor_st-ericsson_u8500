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

char *TvOutSelectionError = "Unable to select AB8500 TVOUT" ;
char *InitDisplayError = "Unable to init display" ;
char *PwrModeSetError = "Unable to set power mode value" ;
char *PwrModeExecError = "Unable to exec power mode" ;
char *FilePathError = "Unable to file path" ;
char *WidthPictureError = "Unable to set width for the picture" ;
char *HeigthPictureError = "Unable to set height for the picture" ;
char *PictureFormatError = "Unable to set picture format" ;
char *LoopModeError = "Unable to set loop mode" ;
char *LoopRateError = "Unable to set loop rate" ;
char *OrderSetError = "Unable to set order" ;
char *OrderExecError = "Unable to exec order" ;
char *FrequencyError = "Unable to set display frequency" ;
char *ScanModeError = "Unable to set scan mode" ;


/**
 *  Manage main and sub displays to generate traffic on DSI0 and DSI2 buses.
 *  @param[in] displayId id of the display.
 *  @param[in] pwrModeId id of power mode.
 *  @param[in] data data needed to manage display.
 *  @retval 0 success.
 *  @retval errorCode if an error occures while processing.
 */
static int tatl03_04Manage_Display(int displayId, int pwrModeId, DTH_DISPLAY_DATA *data)
{
	int vl_error ;
	struct dth_element elem ;

	vl_error = 0;
	elem.user_data = 0 ;
	elem.type = DTH_TYPE_U32 ;

	if ((data->order == START) && (data->state == DEACTIVATED)) {
		/* Init display */
		if (data->displayId == ACT_CVBSDISPLAY_INIT) {
			u32 tvOutSelection = 0 ; /* TV-OUT CVBS via AB8500 */
			vl_error = tatl01_01Set_Video_Parameter(IN_TVODISPLAY_ID, DTH_TYPE_U32, ((void *)&tvOutSelection));
			if (vl_error != 0) {
				data->error = TvOutSelectionError ;
				goto error ;
			}
		}
		elem.user_data = data->displayId ;
		vl_error = tatlm1_00ActExec(&elem) ;
		if (vl_error != 0) {
			data->error = InitDisplayError ;
			goto error ;
		}

		/* Set power mode ON. */
		vl_error = tatl01_01Set_Video_Parameter(pwrModeId, DTH_TYPE_U32, ((void *)&data->order));
		if (vl_error != 0) {
			data->error = OrderSetError ;
			goto error ;
		}
		elem.user_data = pwrModeId ;
		vl_error = tatlm1_00ActExec(&elem) ;
		if (vl_error != 0) {
			data->error = OrderExecError ;
			goto error ;
		}

		/* Set picture properties. */
		vl_error = tatl01_01Set_Video_Parameter(IN_DISPLAY_PATH, DTH_TYPE_STRING, ((void *)data->filePath));
		if (vl_error != 0) {
			data->error = FilePathError ;
			goto error ;
		}
		vl_error = tatl01_01Set_Video_Parameter(IN_PICTDISPLAY_WIDTH, DTH_TYPE_U32, ((void *)&data->width));
		if (vl_error != 0) {
			data->error = WidthPictureError ;
			goto error ;
		}
		vl_error = tatl01_01Set_Video_Parameter(IN_PICTDISPLAY_HEIGHT, DTH_TYPE_U32, ((void *)&data->height));
		if (vl_error != 0) {
			data->error = HeigthPictureError ;
			goto error ;
		}
		vl_error = tatl01_01Set_Video_Parameter(IN_PICTDISPLAY_FORMAT, DTH_TYPE_U32, ((void *)&data->pictureFormat));
		if (vl_error != 0) {
			data->error = PictureFormatError ;
			goto error ;
		}
		vl_error = tatl01_01Set_Video_Parameter(IN_LOOPMODE, DTH_TYPE_U32, ((void *)&data->loopMode));
		if (vl_error != 0) {
			data->error = LoopModeError ;
			goto error ;
		}
		vl_error = tatl01_01Set_Video_Parameter(IN_PICTDISPLAY_DURATION, DTH_TYPE_U32, ((void *)&data->loopRate));
		if (vl_error != 0) {
			data->error = LoopRateError ;
			goto error ;
		}

		/* Set other properties for CVBS and HDMI */
		if ((data->displayId == ACT_CVBSDISPLAY_INIT) || (data->displayId == ACT_HDMIDISPLAY_INIT)) {
		    vl_error = tatl01_01Set_Video_Parameter(IN_TVODISPLAY_FREQUENCY, DTH_TYPE_U32, ((void *)&data->frequency));
		    if (vl_error != 0) {
			    data->error = FrequencyError ;
			    goto error ;
		    }
		    vl_error = tatl01_01Set_Video_Parameter(IN_SCANMODE, DTH_TYPE_U32, ((void *)&data->scanMode));
			if (vl_error != 0) {
				data->error = ScanModeError ;
			    goto error ;
		    }
		}

		/* Start display picture. */
		vl_error = tatl01_01Set_Video_Parameter(displayId, DTH_TYPE_U32, ((void *)&data->order));
		if (vl_error != 0) {
			data->error = OrderSetError ;
			goto error ;
		}
		elem.user_data = displayId ;
		vl_error = tatlm1_00ActExec(&elem) ;
		if (vl_error != 0) {
			data->error = OrderExecError ;
			goto error ;
		}

		/* Update data. */
		data->state = ACTIVATED ;
		data->error = NO_ERROR ;
	} else if ((data->order == STOP) && (data->state == ACTIVATED)) {
		/* Stop display picture. */
		vl_error = tatl01_01Set_Video_Parameter(displayId, DTH_TYPE_U32, ((void *)&data->order));
		if (vl_error != 0) {
			data->error = OrderSetError ;
			goto error ;
		}
		elem.user_data = displayId ;
		vl_error = tatlm1_00ActExec(&elem) ;
		if (vl_error != 0) {
			data->error = OrderExecError ;
			goto error ;
		}

		/* Set power mode OFF. */
		vl_error = tatl01_01Set_Video_Parameter(pwrModeId, DTH_TYPE_U32, ((void *)&data->order));
		if (vl_error != 0) {
			data->error = OrderSetError ;
			goto error ;
		}
		elem.user_data = pwrModeId ;
		vl_error = tatlm1_00ActExec(&elem) ;
		if (vl_error != 0) {
			data->error = OrderExecError ;
			goto error ;
		}

		/* Update data. */
		data->state = DEACTIVATED ;
		data->error = NO_ERROR ;
	}

error:
	return vl_error ;
}

int tatl03_00Display_Exec(struct dth_element *elem)
{
    int vl_error = 0;

	switch (elem->user_data) {
	case ACT_DSI0:
			vl_error = tatl03_04Manage_Display(ACT_MAINDISPLAYPICTURE, ACT_SET_MAINDISPWRMODE, &v_tatcoex_dsi0_data) ;
		break ;

	case ACT_DSI2:
			vl_error = tatl03_04Manage_Display(ACT_HDMIDISPLAYPICTURE, ACT_SET_HDMIDISPWRMODE, &v_tatcoex_dsi2_data) ;
		break ;

	default:
			vl_error = EBADRQC;
		break ;
	}

	return vl_error ;
}


int tatl03_01Display_Get(struct dth_element *elem, void *value)
{
    int vl_error ;

	vl_error = 0;

	switch (elem->user_data) {
	case ACT_DSI0_STATE:
	{
		*((u8 *)value) = v_tatcoex_dsi0_data.state ;
		SYSLOG(LOG_DEBUG, "Get DSI0 state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *)value));
	}
	break ;

	case ACT_DSI0_ERROR:
	{
		strncpy((char *)value, v_tatcoex_dsi0_data.error, MAX_STRNG_SIZE);
		SYSLOG(LOG_DEBUG, "Get DSI0 error string: %s\n", (char *) value);
	}
	break ;

	case ACT_DSI2_STATE:
	{
		*((u8 *)value) = v_tatcoex_dsi2_data.state ;
		SYSLOG(LOG_DEBUG, "Get DSI2 state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *)value));
	}
	break ;

	case ACT_DSI2_ERROR:
	{
		strncpy((char *)value, v_tatcoex_dsi2_data.error, MAX_STRNG_SIZE);
		SYSLOG(LOG_DEBUG, "Get DSI2 error string: %s\n", (char *) value);
	}
	break ;

	default:
		vl_error = EBADRQC;
		break ;
	}

	return vl_error ;

}


int tatl03_02Display_Set(struct dth_element *elem, void *value)
{
    int vl_error = 0;

	switch (elem->user_data) {
	case ACT_DSI0:
	{
		v_tatcoex_dsi0_data.order = *((u8 *)value);
		SYSLOG(LOG_DEBUG, "Set DSI0 order START(0) STOP(1): %i\n", v_tatcoex_dsi0_data.order);
	}
	break;

	case ACT_DSI2:
	{
		v_tatcoex_dsi2_data.order = *((u8 *)value);
		SYSLOG(LOG_DEBUG, "Set DSI2 order START(0) STOP(1): %i\n", v_tatcoex_dsi2_data.order);
	}
	break;

	default:
		vl_error = EBADRQC;
	break;
	}

	return vl_error ;

}

void tatl03_03Init_Dsi_Ycbcr_Data(DTH_DISPLAY_DATA *data, u32 displayId, u32 powerMode, char *filePath, u32 width, u32 height, u32 pictureFormat, u32 loopMode, u32 loopRate, u32 frequency, u32 scanMode)
{
	data->order = STOP ;
	data->state = DEACTIVATED ;
	data->error = NO_ERROR ;
	data->displayId = displayId ;
	data->powerMode = powerMode ;
	data->filePath = filePath ;
	data->width = width ;
	data->height = height ;
	data->pictureFormat = pictureFormat ;
	data->loopMode = loopMode ;
	data->loopRate = loopRate ;
	data->frequency = frequency ;
	data->scanMode = scanMode ;
}

