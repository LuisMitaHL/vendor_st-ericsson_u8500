/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   tx3gpwrswp.c
* \brief   routines to manage WCDMA TX power sweep
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "rfdef.h"
#include "tx3gpwrswp.h"

#include "misc.h"
#include "isimsg.h"

/* CONSTS */

/* C_STATE POWER SWEEP table */
#define C_RF_STATE_POWERSWEEP_INACTIVE		0
#define C_RF_STATE_POWERSWEEP_RUNNING		1
#define C_RF_STATE_POWERSWEEP_STOPPED		2

/* NUMBER OF CHANNELS*/
#define C_RF_WCDMA_TX_POWERSWEEP_NUMBER_OF_CHANNELS	1

/* STEP NUMBER */
#define C_RF_WCDMA_TX_POWERSWEEP_STEP_NUMBER	100

/* GLOBALS */

/* tx3g power sweep operation request message subblock channel parameter */
WCDMA_TX_SWEEP_TEST_CHANNEL_STR *p_tatrf_wcdma_powersweep_channels = NULL;

/* tx3g power sweep operation request message subblock */
C_HAL_SB_WCDMA_TX_SWEEP_TEST_CONTROL_STR *p_tatrf_wcdma_powersweep_sb = NULL;

/* tx3g power sweep operation request message subblock parameters */
uint16 v_tatrf_wcdma_powersweep_setup = 1;
uint32 v_tatrf_wcdma_powersweep_steplength;
uint16 v_tatrf_wcdma_powersweep_numberofchannels = C_RF_WCDMA_TX_POWERSWEEP_NUMBER_OF_CHANNELS;
uint32 v_tatrf_wcdma_powersweep_band;
uint16 v_tatrf_wcdma_powersweep_uluarfcn;
uint16 v_tatrf_wcdma_powersweep_numberofsteps = 1;
int16 v_tatrf_wcdma_powersweep_powerlevel[C_RF_WCDMA_TX_POWERSWEEP_STEP_NUMBER]={0};

/* Function tide to WCDMA TX power sweep          */
/**************************************************/
int DthRf_Tx3G_PowerSweep_Get(struct dth_element *elem, void *value)
{
	return tatl21_01Tx3G_PowerSweep_Get(elem, value);
}

int DthRf_Tx3G_PowerSweep_Set(struct dth_element *elem, void *value)
{
	return tatl21_02Tx3G_PowerSweep_Set(elem, value);
}

int DthRf_Tx3G_PowerSweep_Exec(struct dth_element *elem)
{
	return tatl21_03Tx3G_PowerSweep_Exec(elem);
}

int tatl21_04Tx3G_PowerSweep_Action()
{
	int vl_Error = TAT_ERROR_OFF;

	p_tatrf_wcdma_powersweep_channels = malloc(SIZE_WCDMA_TX_SWEEP_TEST_CHANNEL_STR
					+ (sizeof(int16) * C_RF_WCDMA_TX_POWERSWEEP_STEP_NUMBER));

	p_tatrf_wcdma_powersweep_sb = malloc(SIZE_C_HAL_SB_WCDMA_TX_SWEEP_TEST_CONTROL_STR
					+ (SIZE_WCDMA_TX_SWEEP_TEST_CHANNEL_STR	+ (sizeof(int16) * C_RF_WCDMA_TX_POWERSWEEP_STEP_NUMBER))
					* C_RF_WCDMA_TX_POWERSWEEP_NUMBER_OF_CHANNELS);

	if (NULL == p_tatrf_wcdma_powersweep_channels ||
		NULL == p_tatrf_wcdma_powersweep_sb)
	{
		SYSLOG(LOG_ERR, "malloc failed for WCDMA_TX_SWEEP_TEST_CHANNEL_STR / C_HAL_SB_WCDMA_TX_SWEEP_TEST_CONTROL_STR");
		return TAT_ISI_HANDLER_ERROR;
	}

	/* Init power sweep data */
	memset(p_tatrf_wcdma_powersweep_channels,
		0,
		SIZE_WCDMA_TX_SWEEP_TEST_CHANNEL_STR
		+ sizeof(int16) * C_RF_WCDMA_TX_POWERSWEEP_STEP_NUMBER);

	memset(p_tatrf_wcdma_powersweep_sb,
		0,
		SIZE_C_HAL_SB_WCDMA_TX_SWEEP_TEST_CONTROL_STR
		+ (SIZE_WCDMA_TX_SWEEP_TEST_CHANNEL_STR	+ (sizeof(int16) * C_RF_WCDMA_TX_POWERSWEEP_STEP_NUMBER))
		* C_RF_WCDMA_TX_POWERSWEEP_NUMBER_OF_CHANNELS);

	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	C_HAL_RF_TEST_CONTROL_REQ_STR *vl_req;
	vl_req = tatrf_isi_msg_begin(&vl_msg_info, PN_DSP_COMMON_TEST,
					C_HAL_RF_TEST_CONTROL_REQ, num_sb_id);
	if (NULL == vl_req)
	{
		return TAT_ISI_HANDLER_ERROR;
	}

	p_tatrf_wcdma_powersweep_channels->band_info = v_tatrf_wcdma_powersweep_band;
	p_tatrf_wcdma_powersweep_channels->ul_uarfcn = v_tatrf_wcdma_powersweep_uluarfcn;
	p_tatrf_wcdma_powersweep_channels->number_of_steps = v_tatrf_wcdma_powersweep_numberofsteps;
	memcpy(p_tatrf_wcdma_powersweep_channels->power_level,
		v_tatrf_wcdma_powersweep_powerlevel,
		sizeof(int16) * C_RF_WCDMA_TX_POWERSWEEP_STEP_NUMBER);

	p_tatrf_wcdma_powersweep_sb->number_of_channels = v_tatrf_wcdma_powersweep_numberofchannels;
	p_tatrf_wcdma_powersweep_sb->tx_setup = v_tatrf_wcdma_powersweep_setup;
	p_tatrf_wcdma_powersweep_sb->duration = v_tatrf_wcdma_powersweep_steplength;

	C_HAL_SB_WCDMA_TX_CONTROL_STR *pl_sb =
	tatrf_isi_msg_add_sb_copy(&vl_msg_info,
				C_HAL_SB_WCDMA_TX_SWEEP_TEST_CONTROL,
				p_tatrf_wcdma_powersweep_sb,
				SIZE_C_HAL_SB_WCDMA_TX_SWEEP_TEST_CONTROL_STR
				+ (SIZE_WCDMA_TX_SWEEP_TEST_CHANNEL_STR	+ (sizeof(int16) * C_RF_WCDMA_TX_POWERSWEEP_STEP_NUMBER))
				* C_RF_WCDMA_TX_POWERSWEEP_NUMBER_OF_CHANNELS);

	if (NULL == pl_sb)
	{
		return TAT_ISI_HANDLER_ERROR;
	}

	vl_Error = tatrf_isi_append_data(&vl_msg_info, (u16 *)p_tatrf_wcdma_powersweep_channels,
					SIZE_WCDMA_TX_SWEEP_TEST_CHANNEL_STR / sizeof(u16)
					+ C_RF_WCDMA_TX_POWERSWEEP_STEP_NUMBER);

	if (!TAT_OK(vl_Error))
	{
		return vl_Error;
	}

	/* send ISI message - blocking function */
	vl_Error = tatrf_isi_send(&vl_msg_info, "Start TX 3G Power Sweep Request");
	if (!TAT_OK(vl_Error))
	{
		return vl_Error;
	}

	/* message successfully sent. waiting for response */
	tatl17_06read(&vl_msg_info, C_HAL_RF_TEST_CONTROL_RESP, NULL,
		"Start TX 3G Power Sweep Response", &vl_Error);


	free(p_tatrf_wcdma_powersweep_channels);
	p_tatrf_wcdma_powersweep_channels = NULL;
	free(p_tatrf_wcdma_powersweep_sb);
	p_tatrf_wcdma_powersweep_sb = NULL;

	return vl_Error;
}

int tatl21_01Tx3G_PowerSweep_Get(struct dth_element *elem, void *Value)
{
	int vl_Error = TAT_ERROR_OFF;	/* errno return code */
	int number;

	/* Switch on elem user data and fill Value with the corresponding current value */
	switch (elem->user_data) {

	case ACT_TX3G_POWER_SWEEP_SETUP:
	{
		*((uint16*)Value) = v_tatrf_wcdma_powersweep_setup;
		SYSLOG(LOG_DEBUG, "get: power sweep setup = %u",
			v_tatrf_wcdma_powersweep_setup);
		break;
	}
	case ACT_TX3G_POWER_SWEEP_STEP_LENGTH:
	{
		*((uint16*)Value) = v_tatrf_wcdma_powersweep_steplength;
		SYSLOG(LOG_DEBUG, "get: power sweep step length = %u",
			v_tatrf_wcdma_powersweep_steplength);
		break;
	}
	case ACT_TX3G_POWER_SWEEP_NUMBER_OF_CHANNEL:
	{
		*((uint16*)Value) = v_tatrf_wcdma_powersweep_numberofchannels;
		SYSLOG(LOG_DEBUG, "get: power sweep number of channels = %u",
			v_tatrf_wcdma_powersweep_numberofchannels);
		break;
	}
	case ACT_TX3G_POWER_SWEEP_BAND:
	{
		*((uint16*)Value) = v_tatrf_wcdma_powersweep_band;
		SYSLOG(LOG_DEBUG, "get: power sweep band = %u",
			v_tatrf_wcdma_powersweep_band);
		break;
	}
	case ACT_TX3G_POWER_SWEEP_UARFCN:
	{
		*((uint16*)Value) = v_tatrf_wcdma_powersweep_uluarfcn;
		SYSLOG(LOG_DEBUG, "get: power sweep uluarfcn = %u",
			v_tatrf_wcdma_powersweep_uluarfcn);
		break;
	}
	case ACT_TX3G_POWER_SWEEP_STEP_NUMBER:
	{
		*((uint16*)Value) = v_tatrf_wcdma_powersweep_numberofsteps;
		SYSLOG(LOG_DEBUG, "get: power sweep step number = %u",
			v_tatrf_wcdma_powersweep_numberofsteps);
		break;
	}
	/* Raise error if elem user data is not an valid output */
	default:
	{
		if(elem->user_data >= ACT_TX3G_POWER_SWEEP_POWER_LEVEL_001 &&
			elem->user_data <= ACT_TX3G_POWER_SWEEP_POWER_LEVEL_100)
		{
			number = elem->user_data - ACT_TX3G_POWER_SWEEP_POWER_LEVEL_001;
			DEREF_PTR_SET(Value, float,
			RF_UNQ(DEREF_PTR(v_tatrf_wcdma_powersweep_powerlevel + number, int16), 8));
//			SYSLOG(LOG_DEBUG, "get: power sweep power level table[%d] = %d",
//			number,
//			*(v_tatrf_wcdma_powersweep_powerlevel + number));
		}
		else
		{
			SYSLOG(LOG_ERR, "invalid output for get:%d", elem->user_data);
			vl_Error = TAT_BAD_REQ;
		}
		break;
	}
	}

	return vl_Error;
}

int tatl21_02Tx3G_PowerSweep_Set(struct dth_element *elem, void *Value)
{
	int vl_Error = TAT_ERROR_OFF;	/* errno return code */
	int number;

	/* Switch on elem user data, cast Value and fill the corresponding internal value */
	switch (elem->user_data) {

	case ACT_TX3G_POWER_SWEEP_SETUP:
	{
		v_tatrf_wcdma_powersweep_setup = *((uint16*)Value);
		SYSLOG(LOG_DEBUG, "set: power sweep setup = %u",
			v_tatrf_wcdma_powersweep_setup);
		break;
	}
	case ACT_TX3G_POWER_SWEEP_STEP_LENGTH:
	{
		v_tatrf_wcdma_powersweep_steplength = *((uint16*)Value);
		SYSLOG(LOG_DEBUG, "set: power sweep step length = %u",
			v_tatrf_wcdma_powersweep_steplength);
		break;
	}
	case ACT_TX3G_POWER_SWEEP_NUMBER_OF_CHANNEL:
	{
		v_tatrf_wcdma_powersweep_numberofchannels = *((uint16*)Value);
		SYSLOG(LOG_DEBUG, "set: power sweep number of channels = %u",
			v_tatrf_wcdma_powersweep_numberofchannels);
		break;
	}
	case ACT_TX3G_POWER_SWEEP_BAND:
	{
		v_tatrf_wcdma_powersweep_band = *((uint16*)Value);
		SYSLOG(LOG_DEBUG, "set: power sweep band = %u",
			v_tatrf_wcdma_powersweep_band);
		break;
	}
	case ACT_TX3G_POWER_SWEEP_UARFCN:
	{
		v_tatrf_wcdma_powersweep_uluarfcn = *((uint16*)Value);
		SYSLOG(LOG_DEBUG, "set: power sweep uluarfcn = %u",
			v_tatrf_wcdma_powersweep_uluarfcn);
		break;
	}
	case ACT_TX3G_POWER_SWEEP_STEP_NUMBER:
	{
		v_tatrf_wcdma_powersweep_numberofsteps = *((uint16*)Value);
		SYSLOG(LOG_DEBUG, "set: power sweep step number = %u",
			v_tatrf_wcdma_powersweep_numberofsteps);
		break;
	}
	/* Raise error if elem user data is not an valid output */
	default:
	{

		if(elem->user_data >= ACT_TX3G_POWER_SWEEP_POWER_LEVEL_001 &&
			elem->user_data <= ACT_TX3G_POWER_SWEEP_POWER_LEVEL_100)
		{
			number = elem->user_data - ACT_TX3G_POWER_SWEEP_POWER_LEVEL_001;
			DEREF_PTR_SET(v_tatrf_wcdma_powersweep_powerlevel + number, int16,
			RF_Q(DEREF_PTR(Value, float), 8));
//			SYSLOG(LOG_DEBUG, "set: power sweep power level table[%d] = %d",
//			number,
//			*(v_tatrf_wcdma_powersweep_powerlevel + number));
		}
		else
		{
			SYSLOG(LOG_ERR, "invalid output for set:%d", elem->user_data);
			vl_Error = TAT_BAD_REQ;
		}
		break;
	}
	}

	return vl_Error;
}

int tatl21_03Tx3G_PowerSweep_Exec(struct dth_element *elem)
{
	int vl_Error = TAT_ERROR_OFF;

	switch (elem->user_data) {

	case ACT_TX3G_POWER_SWEEP:
	{
		vl_Error = tatl21_04Tx3G_PowerSweep_Action();
		break;
	}
	default:
	{
		SYSLOG(LOG_ERR, "invalid action code: %d", elem->user_data);
		vl_Error = TAT_BAD_REQ;
		break;
	}
	}

	return vl_Error;
}
