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

char *StartAudioError = "Unable to start audio service";
char *StopAudioError = "Unable to stop audio service";
char *InitAudioError = "Unable to init audio service";
char *DeInitAudioError = "Unable to deinit audio service";

/**
 *  Initialize audio service for coexistence.
 *  @retval 0 success.
 *  @retval -1 if an error occured while processing.
 */
static int tatl08_05Init_Audio_Service()
{
	int vl_error;

	vl_error = 0;

	/* Init is only done if all audio buses are deactivated. */
	if ((v_tatcoex_vib_data.state == DEACTIVATED) && (v_tatcoex_mic_data.state == DEACTIVATED)
	    && (v_tatcoex_msp1_data.state == DEACTIVATED) && (v_tatcoex_msp2_data.state == DEACTIVATED)) {
		system(INIT_AUDIO);
		vl_error = 0;

		/* if(vl_error == -1)
		   {
		   SYSLOG(LOG_ERR, "%s\n", InitAudioError);
		   } */
	}
	return vl_error;
}

/**
 *  DeInitialize audio service for coexistence.
 *  @retval 0 success.
 *  @retval -1 if an error occured while processing.
 */
static int tatl08_06DeInit_Audio_Data()
{
	int vl_error;

	vl_error = 0;

	/* De-init is only done if all audio buses are deactivated. */
	if ((v_tatcoex_vib_data.state == DEACTIVATED) && (v_tatcoex_mic_data.state == DEACTIVATED)
	    && (v_tatcoex_msp1_data.state == DEACTIVATED) && (v_tatcoex_msp2_data.state == DEACTIVATED)) {
		system(DEINIT_AUDIO);
		vl_error = 0;

		/*if(vl_error == -1)
		   {
		   SYSLOG(LOG_ERR, "%s\n", DeInitAudioError);
		   } */
	}

	return vl_error;
}

/**
 *  Manage all audio services for TAT coexistence
 *  @param[in] data needed to manage audio
 */
static void tatl08_04Manage_Audio_Services(DTH_AUDIO_DATA *data)
{

	if ((data->order == START) && (data->state == DEACTIVATED)) {
		/* Init audio if needed. */
		if (tatl08_05Init_Audio_Service() < 0) {
			data->error = InitAudioError;
			data->state = DEACTIVATED;
		} else {
			/*
			   //Launch service
			   if( system(data->startService) < 0)
			   {
			   SYSLOG(LOG_ERR, "%s\n", StartAudioError);
			   data->error = StartAudioError ;
			   data->state = DEACTIVATED ;
			   }
			   else
			   {
			   data->error = NO_ERROR ;
			   data->state = ACTIVATED ;

			   //DeInit audio if needed
			   if (tatl08_06DeInit_Audio_Data () < 0)
			   {
			   data->error = DeInitAudioError ;
			   data->state = DEACTIVATED ;
			   }
			   else
			   {
			   data->error = NO_ERROR ;
			   }
			   }
			 */
			system(data->startService);
			data->error = NO_ERROR;
			data->state = ACTIVATED;
		}
	} else if ((data->order == STOP) && (data->state == ACTIVATED)) {
		/*if( system(data->stopService) < 0)
		   {
		   SYSLOG(LOG_ERR, "%s\n", StopAudioError);
		   data->error = StopAudioError ;
		   }
		   else
		   {
		   data->error = NO_ERROR ;
		   data->state = DEACTIVATED ;
		   } */
		system(data->stopService);
		data->state = DEACTIVATED;

		/* DeInit audio if needed. */
		if (tatl08_06DeInit_Audio_Data() < 0)
			data->error = DeInitAudioError;
		else
			data->error = NO_ERROR;
	}
}

int tatl08_00Audio_Exec(struct dth_element *elem)
{
	int vl_error = 0;

	switch (elem->user_data) {
	case ACT_VIB:
		{
			tatl08_04Manage_Audio_Services(&v_tatcoex_vib_data);
		}
		break;

	case ACT_DMIC:
		{
			tatl08_04Manage_Audio_Services(&v_tatcoex_mic_data);
		}
		break;

	case ACT_MSP1:
		{
			tatl08_04Manage_Audio_Services(&v_tatcoex_msp1_data);
		}
		break;

	case ACT_MSP2:
		{
			tatl08_04Manage_Audio_Services(&v_tatcoex_msp2_data);
		}
		break;

	default:
		vl_error = EBADRQC;
		break;
	}

	return vl_error;
}

int tatl08_01Audio_Get(struct dth_element *elem, void *value)
{
	int vl_error;

	vl_error = 0;

	switch (elem->user_data) {
	case ACT_DMIC_STATE:
		{
			*((u8 *) value) = v_tatcoex_mic_data.state;
			SYSLOG(LOG_DEBUG, "Get DMIC state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_DMIC_ERROR:
		{
			strncpy((char *)value, v_tatcoex_mic_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get DMIC error string: %s\n", (char *)value);
		}
		break;

	case ACT_VIB_STATE:
		{
			*((u8 *) value) = v_tatcoex_vib_data.state;
			SYSLOG(LOG_DEBUG, "Get VIB state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_VIB_ERROR:
		{
			strncpy((char *)value, v_tatcoex_vib_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get VIB error string: %s\n", (char *)value);
		}
		break;

	case ACT_MSP1_STATE:
		{
			*((u8 *) value) = v_tatcoex_msp1_data.state;
			SYSLOG(LOG_DEBUG, "Get MSP1 state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_MSP1_ERROR:
		{
			strncpy((char *)value, v_tatcoex_msp1_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get MSP1 error string: %s\n", (char *)value);
		}
		break;

	case ACT_MSP2_STATE:
		{
			*((u8 *) value) = v_tatcoex_msp2_data.state;
			SYSLOG(LOG_DEBUG, "Get MSP1 state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_MSP2_ERROR:
		{
			strncpy((char *)value, v_tatcoex_msp2_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get MSP1 error string: %s\n", (char *)value);
		}
		break;

	default:
		vl_error = EBADRQC;
		break;
	}

	return vl_error;

}

int tatl08_02Audio_Set(struct dth_element *elem, void *value)
{
	int vl_error;

	vl_error = 0;

	switch (elem->user_data) {
	case ACT_VIB:
		{
			v_tatcoex_vib_data.order = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set VIB order START(0) STOP(1): %i\n", v_tatcoex_vib_data.order);
		}
		break;

	case ACT_DMIC:
		{
			v_tatcoex_mic_data.order = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set DMIC order START(0) STOP(1): %i\n", v_tatcoex_mic_data.order);
		}
		break;

	case ACT_MSP1:
		{
			v_tatcoex_msp1_data.order = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set MSP1 order START(0) STOP(1): %i\n", v_tatcoex_msp1_data.order);
		}
		break;

	case ACT_MSP2:
		{
			v_tatcoex_msp2_data.order = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set MSP1 order START(0) STOP(1): %i\n", v_tatcoex_msp2_data.order);
		}
		break;

	default:
		vl_error = EBADRQC;
		break;
	}

	return vl_error;

}

void tatl08_03Init_Audio_Data(DTH_AUDIO_DATA *data, char *start, char *stop)
{
	data->state = DEACTIVATED;
	data->order = STOP;
	data->error = NO_ERROR;
	data->startService = start;
	data->stopService = stop;
	v_tatcoex_init_audio_done = 0;
}

