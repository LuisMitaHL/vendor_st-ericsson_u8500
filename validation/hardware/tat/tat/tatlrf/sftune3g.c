/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   sftune3g.c
* \brief   routines to self-tune 3G
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "rfdef.h"
#include "sftune3g.h"

#include "misc.h"
#include "isimsg.h"

/*-----------------------------------------------------------------------------
Global variables
-----------------------------------------------------------------------------*/

/* ex MSG_C_TEST_IQ_SELF_TUNING_REQ: ISI message for WCDMA IQ SelfTuning request */
/* Values from the bitmask table C_TEST_TX_IQ_TUNING_MODE */
uint16 v_tatrf15_iq_tx_iq_mode = 0u;

C_TEST_SB_WCDMA_TX_IQ_SETUP_STR v_tatrf15_iq_setup = {
	.sb_id = C_TEST_SB_WCDMA_TX_IQ_SETUP,
	.sb_len = SIZE_C_TEST_SB_WCDMA_TX_IQ_SETUP_STR,
	.band_info = INFO_NO_WCDMA,
	.ul_uarfcn = 0u,
	.fill1 = 0x0000
};

/* ex MSG_C_TEST_IQ_SELF_TUNING_RESP: ISI message for GSM IQ SelfTuning response */
C_TEST_IQ_SELF_TUNING_RESP_STR v_tatrf15_iq_resp = {
	MAKE16(0x77, C_TEST_IQ_SELF_TUNING_RESP),
	.sub_block_count = 0u
};

C_TEST_SB_DC_OFFSET_RESULT_STR v_tatrf15_iq_offset_result = {
	.sb_id = C_TEST_SB_DC_OFFSET_RESULT,
	.sb_len = SIZE_C_TEST_SB_DC_OFFSET_RESULT_STR,
	.status = MAKE16(CTRL_RF_PATH1, C_TEST_OK),
	.i_tuning_value = 0,
	.i_result = 0,
	.q_tuning_value = 0,
	.q_result = 0,
	.fill1 = 0xAA55
};

C_TEST_SB_AMPLITUDE_RESULT_STR v_tatrf15_iq_ampl_result = {
	.sb_id = C_TEST_SB_AMPLITUDE_RESULT,
	.sb_len = SIZE_C_TEST_SB_AMPLITUDE_RESULT_STR,
	.status = MAKE16(CTRL_RF_PATH1, C_TEST_OK),
	.best_tuning_value = 0,
	.best_result = 0,
	.vector_length = 0u
};

C_TEST_SB_PHASE_RESULT_STR v_tatrf15_iq_phase_result = {
	.sb_id = C_TEST_SB_PHASE_RESULT,
	.sb_len = SIZE_C_TEST_SB_PHASE_RESULT_STR,
	.status = MAKE16(CTRL_RF_PATH1, C_TEST_OK),
	.best_tuning_value = 0,
	.best_result = 0,
	.vector_length = 0u
};

/* Prototypes                          */
/***************************************/
int tatl15_setBand(u16 value);
int tatl15_setOPMode(u16 value);
int tatl15_setChannel(u16 value);
int tatl15_wcdmaTxIqSelfTuning(void);


/* Function tide to start WCDMA Tx IQ */
/**************************************/
int DthRf_Wcdma_Tx_Iq_Exec(struct dth_element *elem)
{
	return tatl15_00WcdmaTx_SelfTuning_Exec(elem);
}

int DthRf_Wcdma_Tx_Iq_Get(struct dth_element *elem, void *value)
{
	return tatl15_01WcdmaTx_SelfTuning_Get(elem, value);
}

int DthRf_Wcdma_Tx_Iq_Set(struct dth_element *elem, void *value)
{
	return tatl15_02WcdmaTx_SelfTuning_Set(elem, value);
}


int tatl15_00WcdmaTx_SelfTuning_Exec(struct dth_element *elem)
{
	int vl_Error = TAT_ERROR_CASE;
	switch(elem->user_data)
	{
	case ACT_WCDMA_TX_IQ: /* Run WCDMA TX self tuning. */
		vl_Error = tatl15_wcdmaTxIqSelfTuning();
		break;
	default:
		vl_Error = TAT_BAD_REQ;
		break;
	}
	return vl_Error;
}

int tatl15_01WcdmaTx_SelfTuning_Get(struct dth_element *elem, void *value)
{
	int vl_error = TAT_ERROR_OFF;

	switch(elem->user_data)
	{
	/* OFFSET Result fields*/
	case ACT_WCDMA_TX_IQ_DC_OFFSET_ERROR_CODE:
	{
		if ((LOWBYTE(v_tatrf15_iq_resp.id)) == C_TEST_IQ_SELF_TUNING_RESP)
		{
			u8 *vl_Value;
			vl_Value = (u8 *) value;
			*vl_Value = v_tatrf15_iq_offset_result.status;
			SYSLOG(LOG_DEBUG,
				   "Get WCDMA TX IQ DC offset error code: %d",
				   *vl_Value);
		}
	}
		break;
	case ACT_WCDMA_TX_IQ_DC_OFFSET_HW_PATH:
	{
		if ((LOWBYTE(v_tatrf15_iq_resp.id)) == C_TEST_IQ_SELF_TUNING_RESP)
		{
			u8 *vl_Value;
			vl_Value = (u8 *) value;

			uint16 hw_path = 0x0;
#if ((COMMON_DSP_TEST_ISI_VERSION_Z == 0 && COMMON_DSP_TEST_ISI_VERSION_Y >= 128) || (COMMON_DSP_TEST_ISI_VERSION_Z > 0))
			hw_path = v_tatrf15_iq_offset_result.status;
			hw_path = hw_path>>8;
			hw_path = (hw_path>0x02)? 0:hw_path;
#endif

			*vl_Value = hw_path;
			SYSLOG(LOG_DEBUG,
				   "Get WCDMA TX IQ DC offset HW Path: %d",
				   *vl_Value);
		}
	}
		break;
	case ACT_WCDMA_TX_IQ_I_BRANCH_VALUE:
	{
		if ((LOWBYTE(v_tatrf15_iq_resp.id)) == C_TEST_IQ_SELF_TUNING_RESP)
		{
			s16 *vl_Value;
			vl_Value = (s16 *) value;
			*vl_Value =
					v_tatrf15_iq_offset_result.i_tuning_value;
			SYSLOG(LOG_DEBUG,
				   "Get WCDMA TX IQ DC I branch value: %d",
				   *vl_Value);
		}
	}
		break;
	case ACT_WCDMA_TX_IQ_I_BRANCH_PWR_DIFF:
	{
		if ((LOWBYTE(v_tatrf15_iq_resp.id)) == C_TEST_IQ_SELF_TUNING_RESP)
		{
			s16 *vl_Value;
			vl_Value = (s16 *) value;
			*vl_Value = v_tatrf15_iq_offset_result.i_result;
			SYSLOG(LOG_DEBUG,
				   "Get WCDMA TX IQ DC I branch pwr difference: %d",
				   *vl_Value);
		}
	}
		break;
	case ACT_WCDMA_TX_IQ_Q_BRANCH_VALUE:
	{
		if ((LOWBYTE(v_tatrf15_iq_resp.id)) == C_TEST_IQ_SELF_TUNING_RESP)
		{
			s16 *vl_Value;
			vl_Value = (s16 *) value;
			*vl_Value =
					v_tatrf15_iq_offset_result.q_tuning_value;
			SYSLOG(LOG_DEBUG,
				   "Get WCDMA TX IQ DC Q branch value: %d",
				   *vl_Value);
		}
	}
		break;
	case ACT_WCDMA_TX_IQ_Q_BRANCH_PWR_DIFF:
	{
		if ((LOWBYTE(v_tatrf15_iq_resp.id)) == C_TEST_IQ_SELF_TUNING_RESP)
		{
			s16 *vl_Value;
			vl_Value = (s16 *) value;
			*vl_Value = v_tatrf15_iq_offset_result.q_result;
			SYSLOG(LOG_DEBUG,
				   "Get WCDMA TX IQ DC Q branch pwr difference: %d",
				   *vl_Value);
		}
	}
		break;

		/* PHASE Result fields*/
	case ACT_WCDMA_TX_IQ_PHASE_ERROR_CODE:
	{
		if ((LOWBYTE(v_tatrf15_iq_resp.id)) == C_TEST_IQ_SELF_TUNING_RESP)
		{
			u8 *vl_Value;
			vl_Value = (u8 *) value;
			*vl_Value = v_tatrf15_iq_phase_result.status;
			SYSLOG(LOG_DEBUG,
				   "Get WCDMA TX IQ phase error code: %d",
				   *vl_Value);
		}
	}
		break;

	case ACT_WCDMA_TX_IQ_PHASE_HW_PATH:
	{
		if ((LOWBYTE(v_tatrf15_iq_resp.id)) == C_TEST_IQ_SELF_TUNING_RESP)
		{
			u8 *vl_Value;
			vl_Value = (u8 *) value;

			uint16 hw_path = 0x0;
#if ((COMMON_DSP_TEST_ISI_VERSION_Z == 0 && COMMON_DSP_TEST_ISI_VERSION_Y >= 128) || (COMMON_DSP_TEST_ISI_VERSION_Z > 0))
			hw_path = v_tatrf15_iq_phase_result.status;
			hw_path = hw_path>>8;
			hw_path = (hw_path>0x02)? 0:hw_path;
#endif

			*vl_Value = hw_path;
			SYSLOG(LOG_DEBUG,
				   "Get WCDMA TX IQ phase HW Path: %d",
				   *vl_Value);
		}
	}
		break;

	case ACT_WCDMA_TX_IQ_PHASE_TUNING_VALUE:
	{
		if ((LOWBYTE(v_tatrf15_iq_resp.id)) == C_TEST_IQ_SELF_TUNING_RESP)
		{
			s16 *vl_Value;
			vl_Value = (s16 *) value;
			*vl_Value = v_tatrf15_iq_phase_result.best_tuning_value;
			SYSLOG(LOG_DEBUG,
				   "Get WCDMA TX IQ phase tuning value: %d",
				   *vl_Value);
		}
	}
		break;

	case ACT_WCDMA_TX_IQ_PHASE_BEST_TUNING_RESULT:
	{
		if ((LOWBYTE(v_tatrf15_iq_resp.id)) == C_TEST_IQ_SELF_TUNING_RESP)
		{
			s16 *vl_Value;
			vl_Value = (s16 *) value;
			*vl_Value =
					v_tatrf15_iq_phase_result.best_result;
			SYSLOG(LOG_DEBUG,
				   "Get WCDMA TX IQ phase best tuning value: %d",
				   *vl_Value);
		}
	}
		break;

	case ACT_WCDMA_TX_IQ_PHASE_TUNING_VEC_LEN:
	{
		if ((LOWBYTE(v_tatrf15_iq_resp.id)) == C_TEST_IQ_SELF_TUNING_RESP)
		{
			s16 *vl_Value;
			vl_Value = (s16 *) value;
			*vl_Value =
					v_tatrf15_iq_phase_result.vector_length;
			SYSLOG(LOG_DEBUG,
				   "Get WCDMA TX IQ phase vector length: %d",
				   *vl_Value);
		}
	}
		break;

		/* AMPLITUDE Result fields*/
	case ACT_WCDMA_TX_IQ_AMPL_ERROR_CODE:
	{
		if ((LOWBYTE(v_tatrf15_iq_resp.id)) == C_TEST_IQ_SELF_TUNING_RESP)
		{
			u8 *vl_Value;
			vl_Value = (u8 *) value;
			*vl_Value = v_tatrf15_iq_ampl_result.status;
			SYSLOG(LOG_DEBUG,
				   "Get WCDMA TX IQ amplitude error code: %d",
				   *vl_Value);
		}
	}
		break;

	case ACT_WCDMA_TX_IQ_AMPL_HW_PATH:
	{
		if ((LOWBYTE(v_tatrf15_iq_resp.id)) == C_TEST_IQ_SELF_TUNING_RESP)
		{
			u8 *vl_Value;
			vl_Value = (u8 *) value;

			uint16 hw_path = 0x0;
#if ((COMMON_DSP_TEST_ISI_VERSION_Z == 0 && COMMON_DSP_TEST_ISI_VERSION_Y >= 128) || (COMMON_DSP_TEST_ISI_VERSION_Z > 0))
			hw_path = v_tatrf15_iq_ampl_result.status;
			hw_path = hw_path>>8;
			hw_path = (hw_path>0x02)? 0:hw_path;
#endif

			*vl_Value = hw_path;
			SYSLOG(LOG_DEBUG,
				   "Get WCDMA TX IQ amplitude HW Path: %d",
				   *vl_Value);
		}
	}
		break;

	case ACT_WCDMA_TX_IQ_AMPL_BEST_TUNING_VALUE:
	{
		if ((LOWBYTE(v_tatrf15_iq_resp.id)) == C_TEST_IQ_SELF_TUNING_RESP)
		{
			s16 *vl_Value;
			vl_Value = (s16 *) value;
			*vl_Value = v_tatrf15_iq_ampl_result.best_tuning_value;
			SYSLOG(LOG_DEBUG,
				   "Get WCDMA TX IQ amplitude best tuning value: %d",
				   *vl_Value);
		}
	}
		break;

	case ACT_WCDMA_TX_IQ_AMPL_BEST_TUNING_RESULT:
	{
		if ((LOWBYTE(v_tatrf15_iq_resp.id)) == C_TEST_IQ_SELF_TUNING_RESP)
		{
			s16 *vl_Value;
			vl_Value = (s16 *) value;
			*vl_Value =
					v_tatrf15_iq_ampl_result.best_result;
			SYSLOG(LOG_DEBUG,
				   "Get WCDMA TX IQ amplitude best tuning result: %d",
				   *vl_Value);
		}
	}
		break;

	case ACT_WCDMA_TX_IQ_AMPL_TUNING_VEC_LEN:
	{
		if ((LOWBYTE(v_tatrf15_iq_resp.id)) == C_TEST_IQ_SELF_TUNING_RESP)
		{
			s16 *vl_Value;
			vl_Value = (s16 *) value;
			*vl_Value =
					v_tatrf15_iq_ampl_result.vector_length;
			SYSLOG(LOG_DEBUG,
				   "Get WCDMA TX IQ amplitude vector length: %d",
				   *vl_Value);
		}
	}
		break;

	default:
		vl_error = TAT_BAD_REQ;
	}

	return vl_error;
}

int tatl15_02WcdmaTx_SelfTuning_Set(struct dth_element *elem, void *value)
{
	int vl_error = TAT_ERROR_OFF;

	u16 *vl_Value;
	vl_Value = (u16 *) value;

	switch(elem->user_data)
	{
	case ACT_WCDMA_TX_IQ_TX_OP_MODE:
		vl_error = tatl15_setOPMode(*vl_Value);
		break;
	case ACT_WCDMA_TX_IQ_TX_BAND:
		vl_error = tatl15_setBand(*vl_Value);
		break;
	case ACT_WCDMA_TX_IQ_TX_CHANNEL:
		vl_error = tatl15_setChannel(*vl_Value);
		break;
	}

	return vl_error;
}

int tatl15_wcdmaTxIqSelfTuning(void)
{
	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	C_TEST_IQ_SELF_TUNING_REQ_STR *pl_req;
	pl_req =
			tatrf_isi_msg_begin(&vl_msg_info, PN_DSP_COMMON_TEST,
								C_TEST_IQ_SELF_TUNING_REQ, sub_block_count);
	if (NULL == pl_req)
		return TAT_ISI_HANDLER_ERROR;

	pl_req->tx_iq_mode = v_tatrf15_iq_tx_iq_mode;
	pl_req->fill1 = 0x0000;

	/* add WCDMA TX IQ subblock. */
	C_TEST_SB_WCDMA_TX_IQ_SETUP_STR *pl_sb_iq_setup =
			tatrf_isi_msg_add_sb_copy(&vl_msg_info, C_TEST_SB_WCDMA_TX_IQ_SETUP,
									  &v_tatrf15_iq_setup,
									  sizeof(v_tatrf15_iq_setup));
	if (NULL == pl_sb_iq_setup) {
		return TAT_ISI_HANDLER_ERROR;
	}

	/* send ISI message - blocking function */
	int vl_Error = tatrf_isi_send(&vl_msg_info, "WCDMA TX IQ Self tuning");
	if (!TAT_OK(vl_Error))
		return vl_Error;

	/* message successfully sent. waiting for response */
	memset(&v_tatrf15_iq_resp, 0, sizeof(v_tatrf15_iq_resp));
	memset(&v_tatrf15_iq_offset_result, 0, sizeof(v_tatrf15_iq_offset_result));
	memset(&v_tatrf15_iq_ampl_result, 0, sizeof(v_tatrf15_iq_ampl_result));
	memset(&v_tatrf15_iq_phase_result, 0, sizeof(v_tatrf15_iq_phase_result));

	C_TEST_IQ_SELF_TUNING_RESP_STR *pl_resp =
			tatrf_isi_read(&vl_msg_info, C_TEST_IQ_SELF_TUNING_RESP,
						   sub_block_count, "WCDMA TX IQ Self tuning response",
						   &vl_Error);
	if (NULL != pl_resp) {
		/* this what we expected */
		v_tatrf15_iq_resp = *pl_resp;

		/* copy returned C_TEST_SB_DC_OFFSET_RESULT subblock */
		C_TEST_SB_DC_OFFSET_RESULT_STR *pl_sbr_offset_result =
				tatrf_isi_find_sb(&vl_msg_info, C_TEST_SB_DC_OFFSET_RESULT,
								  NULL);
		if (NULL != pl_sbr_offset_result) {
			v_tatrf15_iq_offset_result = *pl_sbr_offset_result;
		}

		/* copy returned C_TEST_SB_AMPLITUDE_RESULT subblock */
		C_TEST_SB_AMPLITUDE_RESULT_STR *pl_sbr_ampl_result =
				tatrf_isi_find_sb(&vl_msg_info, C_TEST_SB_AMPLITUDE_RESULT,
								  NULL);
		if (NULL != pl_sbr_ampl_result) {
			v_tatrf15_iq_ampl_result = *pl_sbr_ampl_result;
		}

		/* copy returned C_TEST_SB_PHASE_RESULT subblock */
		C_TEST_SB_PHASE_RESULT_STR *pl_sbr_phase_result =
				tatrf_isi_find_sb(&vl_msg_info, C_TEST_SB_PHASE_RESULT,
								  NULL);
		if (NULL != pl_sbr_phase_result) {
			v_tatrf15_iq_phase_result = *pl_sbr_phase_result;
		}
	}

	return vl_Error;
}

/**
 * @brief tatl15_setOPMode Sets the operation mode according to the value passed from TAT client
 * @param value Value from TAT client
 * @return TAT error code
 */
int tatl15_setOPMode(u16 value)
{
	int vl_error = TAT_ERROR_OFF;

	switch (value) {
	case 0:
		v_tatrf15_iq_tx_iq_mode = DC_OFFSET;
		break;
	case 1:
		v_tatrf15_iq_tx_iq_mode = AMPLITUDE;
		break;
	case 2:
		v_tatrf15_iq_tx_iq_mode = DC_OFFSET | AMPLITUDE;
		break;
	case 3:
		v_tatrf15_iq_tx_iq_mode = IQ_PHASE;
		break;
	case 4:
		v_tatrf15_iq_tx_iq_mode = DC_OFFSET | IQ_PHASE;
		break;
	case 5:
		v_tatrf15_iq_tx_iq_mode = AMPLITUDE | IQ_PHASE;
		break;
	case 6:
		v_tatrf15_iq_tx_iq_mode =
				DC_OFFSET | AMPLITUDE | IQ_PHASE;
		break;

	default:
		vl_error = TAT_BAD_REQ;
	}

	SYSLOG(LOG_DEBUG,
		   "Set WCDMA TX IQ OP mode %d",
		   v_tatrf15_iq_tx_iq_mode);

	return vl_error;
}

/**
 * @brief tatl15_setBand Sets the band according to the value passed from TAT client
 * @param value Value from TAT client
 * @return TAT error code
 */
int tatl15_setBand(u16 value)
{
	int vl_error = TAT_ERROR_OFF;
	int decal = (int)(value-1);

	uint32 band = 0x00000000;
	switch(value)
	{
	case 0:
		band = 0x00000000;
		break;
	case 1:
		band = 0x00000001;
		break;
	default:
		band = 0x00000001;
		band = band<<decal;
		break;
	}

	v_tatrf15_iq_setup.band_info = band;

	SYSLOG(LOG_DEBUG,
		   "Set WCDMA TX IQ band parameter %u",
		   v_tatrf15_iq_setup.band_info);

	return vl_error;
}

/**
 * @brief tatl15_setChannel Sets the channel according to the value passed from TAT client
 * @param value Value from TAT client
 * @return TAT error code
 */
int tatl15_setChannel(u16 value)
{
	int vl_error = TAT_ERROR_OFF;

	v_tatrf15_iq_setup.ul_uarfcn = value;

	SYSLOG(LOG_DEBUG,
		   "Set WCDMA TX IQ channel parameter %d",
		   v_tatrf15_iq_setup.ul_uarfcn);

	return vl_error;
}

