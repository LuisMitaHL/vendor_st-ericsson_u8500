/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   sftune2g.c
* \brief   routines to self-tune 2G and perform PA compression test
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "rfdef.h"
#include "sftune2g.h"

#include "misc.h"
#include "isimsg.h"


#define CONTROL_WORD	0x0001

/* struct C_TEST_SB_GSM_PA_COMPRESSION_TUNING_STR quotients */
#define QUOT_SB_PA_COMPRESSION_TARGET_POWER			 6
#define QUOT_SB_PA_COMPRESSION_COMPR_LEVEL			 6

/* struct C_TEST_SB_GSM_PA_COMPRESSION_TUNING_RESULTS_STR quotients */
#define QUOT_SB_PA_COMPRESSION_RESULTS_COMP_LEVEL    6
#define QUOT_SB_PA_COMPRESSION_RESULTS_TX_POWER      6

/*-----------------------------------------------------------------------------
Global variables
-----------------------------------------------------------------------------*/

/* ex MSG_C_TEST_IQ_SELF_TUNING_REQ: ISI message for GSM IQ SelfTuning request */
/* Values from the bitmask table C_TEST_TX_IQ_TUNING_MODE */
uint16 v_tatrf11_iq_tx_iq_mode = 0u;

C_TEST_SB_GSM_TX_IQ_SETUP_STR v_tatrf11_iq_setup = {
    .sb_id = C_TEST_SB_GSM_TX_IQ_SETUP,
    .sb_len = SIZE_C_TEST_SB_GSM_TX_IQ_SETUP_STR,
    .band = G_TEST_UNINITIALIZED_GSM_BAND,
    .pa_loop_mode = G_TEST_PA_MODE_NONE,
    .power_level = 0u,
    .channel_nbr = 0u,
    .dc_offset_i = 0,
    .dc_offset_q = 0,
    .ampl_offset = 0,
    .fill1 = 0xAA55
};

/* ex MSG_C_TEST_IQ_SELF_TUNING_RESP: ISI message for GSM IQ SelfTuning response */
C_TEST_IQ_SELF_TUNING_RESP_STR v_tatrf11_iq_resp = {
    MAKE16(0x77, C_TEST_IQ_SELF_TUNING_RESP),
    .sub_block_count = 0u
};

C_TEST_SB_DC_OFFSET_RESULT_STR v_tatrf11_iq_offset_result = {
    .sb_id = C_TEST_SB_DC_OFFSET_RESULT,
    .sb_len = SIZE_C_TEST_SB_DC_OFFSET_RESULT_STR,
    .status = MAKE16(CTRL_RF_PATH1, C_TEST_OK),
    .i_tuning_value = 0,
    .i_result = 0,
    .q_tuning_value = 0,
    .q_result = 0,
    .fill1 = 0xAA55
};

C_TEST_SB_AMPLITUDE_RESULT_STR v_tatrf11_iq_ampl_result = {
    .sb_id = C_TEST_SB_AMPLITUDE_RESULT,
    .sb_len = SIZE_C_TEST_SB_AMPLITUDE_RESULT_STR,
    .status = MAKE16(CTRL_RF_PATH1, C_TEST_OK),
    .best_tuning_value = 0,
    .best_result = 0,
    .vector_length = 0u
};

C_TEST_SB_PHASE_RESULT_STR v_tatrf11_iq_phase_result = {
    .sb_id = C_TEST_SB_PHASE_RESULT,
    .sb_len = SIZE_C_TEST_SB_PHASE_RESULT_STR,
    .status = MAKE16(CTRL_RF_PATH1, C_TEST_OK),
    .best_tuning_value = 0,
    .best_result = 0,
    .vector_length = 0u
};

/* ex MSG_C_TEST_PA_COMPRESSION_REQ: ISI message for PA compression SelfTuning request. */
/* Values from the bitmask table C_TEST_TX_IQ_TUNING_MODE */
C_TEST_SB_GSM_PA_COMPRESSION_TUNING_STR v_tatrf11_pa_compression;

/* ex MSG_C_TEST_PA_COMPRESSION_RESP: ISI message for PA compression SelfTuning response */
C_TEST_TUNING_RESP_STR v_tatrf11_pa_resp = {
    .id = C_TEST_TUNING_RESP,
    .status = MAKE16(ISIFILLER8, C_TEST_OK),
    .fill2 = 0xAA55,
    .numerOfSubBlocks = 0u
};

C_TEST_SB_GSM_PA_COMPRESSION_TUNING_RESULTS_STR v_tatrf11_pa_results = {
    .sb_id = C_TEST_SB_GSM_PA_COMPRESSION_TUNING_RESULTS,
    .sb_len = SIZE_C_TEST_SB_GSM_PA_COMPRESSION_TUNING_RESULTS_STR,
    .status = MAKE16(ISIFILLER8, C_TEST_OK),
    .fill1 = 0xAA55,
    .rf_band = INFO_NO_GSM,
    .pa_compensation_dB_q6 = 0,
    .final_tx_power_dBm_q6 = 0,
    .iteration_count = 0,
    .fill2 = 0xAA55
};

/* Function tide to Start GSM Tx IQ                */
/**************************************************/
int DthRf_Gsm_Tx_Iq_Exec(struct dth_element *elem)
{
	return tatl11_00GsmTx_SelfTuning_Exec(elem);
}

int DthRf_Gsm_Tx_Iq_Get(struct dth_element *elem, void *value)
{
	return tatl11_01GsmTx_SelfTuning_Get(elem, value);
}

int DthRf_Gsm_Tx_Iq_Set(struct dth_element *elem, void *value)
{
	return tatl11_02GsmTx_SelfTuning_Set(elem, value);
}

/* Function tide to Start GSM PA                  */
/**************************************************/
int DthRf_Gsm_Pa_Exec(struct dth_element *elem)
{
	return tatl11_00GsmTx_SelfTuning_Exec(elem);
}

int DthRf_Gsm_Pa_Get(struct dth_element *elem, void *value)
{
	return tatl11_01GsmTx_SelfTuning_Get(elem, value);
}

int DthRf_Gsm_Pa_Set(struct dth_element *elem, void *value)
{
	return tatl11_02GsmTx_SelfTuning_Set(elem, value);
}

int tatl11_gsmTxIqSelfTuning(void);
int tatl11_gsmPaCompressionSelfTuning(void);

int tatl11_00GsmTx_SelfTuning_Exec(struct dth_element *elem)
{
	int vl_Error = TAT_ERROR_CASE;

	switch (elem->user_data) {
	case ACT_GSM_TX_IQ:	/* Run GSM TX IQ Self Tuning. */

		vl_Error = tatl11_gsmTxIqSelfTuning();
		break;

	case ACT_GSM_PA:	/* Run GSM PA Self Tuning. */

		vl_Error = tatl11_gsmPaCompressionSelfTuning();
		break;

	default:
		vl_Error = TAT_BAD_REQ;
	}

	return vl_Error;

}

int tatl11_01GsmTx_SelfTuning_Get(struct dth_element *elem, void *value)
{
	int vl_error = TAT_ERROR_OFF;

	switch (elem->user_data) {
		/* Get GSM PA iteration count. */
	case ACT_GSM_PA_ITER_COUNT:
		{
			if ((LOWBYTE(v_tatrf11_pa_resp.id)) == C_TEST_TUNING_RESP) {
				u32 *vl_Value;
				vl_Value = (u32 *) value;
				*vl_Value = v_tatrf11_pa_results.iteration_count;
				SYSLOG(LOG_DEBUG,
				       "Get GSM PA iteration count: %d",
				       *vl_Value);
			}
		}
		break;

		/* Get GSM PA error code. */
	case ACT_GSM_PA_ERROR_CODE:
		{
			if ((LOWBYTE(v_tatrf11_pa_resp.id)) == C_TEST_TUNING_RESP) {
				u8 *vl_Value;
				vl_Value = (u8 *) value;
				*vl_Value = v_tatrf11_pa_results.status;
				SYSLOG(LOG_DEBUG, "Get GSM PA error code: %d",
				       *vl_Value);
			}
		}
		break;

		/* Get GSM PA compensation. */
	case ACT_GSM_PA_COMPENSATION_LEVEL:
		{
			if ((LOWBYTE(v_tatrf11_pa_resp.id)) == C_TEST_TUNING_RESP) {
				float *vl_Value;
				vl_Value = (float *)value;
				*vl_Value =
				    RF_UNQ(v_tatrf11_pa_results.
					   pa_compensation_dB_q6,
					   QUOT_SB_PA_COMPRESSION_RESULTS_COMP_LEVEL);
				SYSLOG(LOG_DEBUG,
				       "Get GSM PA compensation: %lf",
				       *vl_Value);
			}
		}
		break;

		/* Get GSM PA Tx power level. */
	case ACT_GSM_PA_TX_POWER_LEVEL:
		{
			if ((LOWBYTE(v_tatrf11_pa_resp.id)) == C_TEST_TUNING_RESP) {
				float *vl_Value;
				vl_Value = (float *)value;
				*vl_Value =
				RF_UNQ(v_tatrf11_pa_results.
					   final_tx_power_dBm_q6,
					   QUOT_SB_PA_COMPRESSION_RESULTS_TX_POWER);
				SYSLOG(LOG_DEBUG,
				       "Get GSM PA Tx power level: %f",
				       *vl_Value);
			}
		}
		break;

		/* Get GMSK TX IQ DC offset error code. */
	case ACT_GSM_TX_IQ_DC_OFFSET_ERROR_CODE:
		{
			if ((LOWBYTE(v_tatrf11_iq_resp.id)) ==
			    C_TEST_IQ_SELF_TUNING_RESP) {
				u8 *vl_Value;
				vl_Value = (u8 *) value;
				*vl_Value = v_tatrf11_iq_offset_result.status;
				SYSLOG(LOG_DEBUG,
				       "Get GMSK TX IQ DC offset error code: %d",
				       *vl_Value);
			}
		}
		break;

		/* Get GMSK TX IQ I branch value. */
	case ACT_GSM_TX_IQ_I_BRANCH_VALUE:
		{
			if ((LOWBYTE(v_tatrf11_iq_resp.id)) ==
			    C_TEST_IQ_SELF_TUNING_RESP) {
				s16 *vl_Value;
				vl_Value = (s16 *) value;
				*vl_Value =
				v_tatrf11_iq_offset_result.i_tuning_value;
				SYSLOG(LOG_DEBUG,
				       "Get GMSK TX IQ DC I branch value: %d",
				       *vl_Value);
			}
		}
		break;

		/* Get GMSK TX IQ I branch pwr difference. */
	case ACT_GSM_TX_IQ_I_BRANCH_PWR_DIFF:
		{
			if ((LOWBYTE(v_tatrf11_iq_resp.id)) ==
			    C_TEST_IQ_SELF_TUNING_RESP) {
				s16 *vl_Value;
				vl_Value = (s16 *) value;
				*vl_Value = v_tatrf11_iq_offset_result.i_result;
				SYSLOG(LOG_DEBUG,
				       "Get GMSK TX IQ DC I branch pwr difference: %d",
				       *vl_Value);
			}
		}
		break;

		/* Get GMSK TX IQ Q branch value. */
	case ACT_GSM_TX_IQ_Q_BRANCH_VALUE:
		{
			if ((LOWBYTE(v_tatrf11_iq_resp.id)) ==
			    C_TEST_IQ_SELF_TUNING_RESP) {
				s16 *vl_Value;
				vl_Value = (s16 *) value;
				*vl_Value =
				    v_tatrf11_iq_offset_result.q_tuning_value;
				SYSLOG(LOG_DEBUG,
				       "Get GMSK TX IQ DC Q branch value: %d",
				       *vl_Value);
			}
		}
		break;

		/* Get GMSK TX IQ Q branch pwr difference. */
	case ACT_GSM_TX_IQ_Q_BRANCH_PWR_DIFF:
		{
			if ((LOWBYTE(v_tatrf11_iq_resp.id)) ==
			    C_TEST_IQ_SELF_TUNING_RESP) {
				s16 *vl_Value;
				vl_Value = (s16 *) value;
				*vl_Value = v_tatrf11_iq_offset_result.q_result;
				SYSLOG(LOG_DEBUG,
				       "Get GMSK TX IQ DC Q branch pwr difference: %d",
				       *vl_Value);
			}
		}
		break;

		/* Get GMSK TX IQ amplitude error code. */
	case ACT_GSM_TX_IQ_AMPL_ERROR_CODE:
		{
			if ((LOWBYTE(v_tatrf11_iq_resp.id)) ==
			    C_TEST_IQ_SELF_TUNING_RESP) {
				u8 *vl_Value;
				vl_Value = (u8 *) value;
				*vl_Value = v_tatrf11_iq_ampl_result.status;
				SYSLOG(LOG_DEBUG,
				       "Get GMSK TX IQ amplitude error code: %d",
				       *vl_Value);
			}
		}
		break;

		/* Get GMSK TX IQ amplitude tuning value. */
	case ACT_GSM_TX_IQ_AMPL_TUNING_VALUE:
		{
			if ((LOWBYTE(v_tatrf11_iq_resp.id)) ==
			    C_TEST_IQ_SELF_TUNING_RESP) {
				s16 *vl_Value;
				vl_Value = (s16 *) value;
				*vl_Value = v_tatrf11_iq_ampl_result.best_tuning_value;
				SYSLOG(LOG_DEBUG,
				       "Get GMSK TX IQ amplitude tuning value: %d",
				       *vl_Value);
			}
		}
		break;

		/* Get GMSK TX IQ amplitude best tuning value. */
	case ACT_GSM_TX_IQ_AMPL_BEST_TUNING_VALUE:
		{
			if ((LOWBYTE(v_tatrf11_iq_resp.id)) ==
			    C_TEST_IQ_SELF_TUNING_RESP) {
				s16 *vl_Value;
				vl_Value = (s16 *) value;
				*vl_Value =
				v_tatrf11_iq_ampl_result.best_result;
				SYSLOG(LOG_DEBUG,
				       "Get GMSK TX IQ amplitude best tuning value: %d",
				       *vl_Value);
			}
		}
		break;

		/* Get GMSK TX IQ phase error code. */
	case ACT_GSM_TX_IQ_PHASE_ERROR_CODE:
		{
			if ((LOWBYTE(v_tatrf11_iq_resp.id)) ==
			    C_TEST_IQ_SELF_TUNING_RESP) {
				u8 *vl_Value;
				vl_Value = (u8 *) value;
				*vl_Value = v_tatrf11_iq_phase_result.status;
				SYSLOG(LOG_DEBUG,
				       "Get GMSK TX IQ phase error code: %d",
				       *vl_Value);
			}
		}
		break;

		/* Get GMSK TX IQ phase tuning value. */
	case ACT_GSM_TX_IQ_PHASE_TUNING_VALUE:
		{
			if ((LOWBYTE(v_tatrf11_iq_resp.id)) ==
			    C_TEST_IQ_SELF_TUNING_RESP) {
				s16 *vl_Value;
				vl_Value = (s16 *) value;
				*vl_Value = v_tatrf11_iq_phase_result.best_result;
				SYSLOG(LOG_DEBUG,
				       "Get GMSK TX IQ phase tuning value: %d",
				       *vl_Value);
			}
		}
		break;

		/* Get GMSK TX IQ phase best tuning value. */
	case ACT_GSM_TX_IQ_PHASE_BEST_TUNING_VALUE:
		{
			if ((LOWBYTE(v_tatrf11_iq_resp.id)) ==
			    C_TEST_IQ_SELF_TUNING_RESP) {
				s16 *vl_Value;
				vl_Value = (s16 *) value;
				*vl_Value =
				    v_tatrf11_iq_phase_result.best_tuning_value;
				SYSLOG(LOG_DEBUG,
				       "Get GMSK TX IQ phase best tuning value: %d",
				       *vl_Value);
			}
		}
		break;

	default:
		/* Invalid request code */
		vl_error = TAT_BAD_REQ;
	}

	return vl_error;
}

int tatl11_02GsmTx_SelfTuning_Set(struct dth_element *elem, void *value)
{
	int vl_error = TAT_ERROR_OFF;

	switch (elem->user_data) {

	case ACT_GSM_PA:	/* GSM PA Self Tuning: only one action available */
		/* the important point is to return TAT_ERROR_OFF. */
		break;

		/* Set channel value. */
	case ACT_GSM_PA_TARGET_POWER:
		{
			float *vl_Value;
			vl_Value = (float *)value;

			SYSLOG(LOG_DEBUG, "Set GSM PA target power: %lf",
			       *vl_Value);
			v_tatrf11_pa_compression.target_power_dBm_q6 =
			    RF_Q(*vl_Value,
				 QUOT_SB_PA_COMPRESSION_TARGET_POWER);
		}
		break;

		/* Set channel value. */
	case ACT_GSM_PA_COMPRESSION_LEVEL:
		{
			float *vl_Value;
			vl_Value = (float *)value;

			SYSLOG(LOG_DEBUG, "Set GSM PA compression level: %lf",
			       *vl_Value);
			v_tatrf11_pa_compression.compression_level_dB_q6 =
			    RF_Q(*vl_Value, QUOT_SB_PA_COMPRESSION_COMPR_LEVEL);
		}
		break;

		/* Set GSM PA Tx Band Parameter */
	case ACT_GSM_PA_TX_BAND:
		{
			u16 *vl_Value;
			vl_Value = (u16 *) value;

			v_tatrf11_pa_compression.rf_band =
			    tatl3_04GetGsmRfBand(*vl_Value);
			SYSLOG(LOG_DEBUG,
			       "set NO BAND(0), GSM850(1), GSM900(2), DCS1800(3), PCS1900(4): %u",
			       *vl_Value);
			if (INFO_NO_GSM != v_tatrf11_pa_compression.rf_band) {
				SYSLOG(LOG_DEBUG, "set GSM band info to %lu",
				    v_tatrf11_pa_compression.rf_band);
			} else {
				vl_error = TAT_BAD_REQ;
			}
		}
		break;

		/* Set channel value. */
	case ACT_GSM_PA_TX_CHANNEL:
		{
			u16 *vl_Value;
			vl_Value = (u16 *) value;

			v_tatrf11_pa_compression.channel_nbr = *vl_Value;
			SYSLOG(LOG_DEBUG, "Set GSM PA channel parameter: %d",
			    v_tatrf11_pa_compression.channel_nbr);
		}
		break;

	case ACT_GSM_TX_IQ:	/* GSM TX IQ Self Tuning: only one action available */
		/* the important point is to return TAT_ERROR_OFF. */
		break;

		/*Set GMSK Tx power level task : Start or Stop */
	case ACT_GSM_TX_IQ_TX_OP_MODE:
		{
			u16 *vl_Value;
			vl_Value = (u16 *) value;
			switch (*vl_Value) {
			case 0:
				v_tatrf11_iq_tx_iq_mode = DC_OFFSET;
				break;

			case 1:
				v_tatrf11_iq_tx_iq_mode = AMPLITUDE;
				break;

			case 2:
				v_tatrf11_iq_tx_iq_mode = DC_OFFSET | AMPLITUDE;
				break;

			case 3:
				v_tatrf11_iq_tx_iq_mode = IQ_PHASE;
				break;

			case 4:
				v_tatrf11_iq_tx_iq_mode = DC_OFFSET | IQ_PHASE;
				break;

			case 5:
				v_tatrf11_iq_tx_iq_mode = AMPLITUDE | IQ_PHASE;
				break;

			case 6:
				v_tatrf11_iq_tx_iq_mode =
				    DC_OFFSET | AMPLITUDE | IQ_PHASE;
				break;

			default:
				vl_error = TAT_BAD_REQ;
			}

#if defined (_DEBUG)
			if (TAT_OK(vl_error)) {
				SYSLOG(LOG_DEBUG,
				       "Set GSM TX IQ operation mode: %d",
				       v_tatrf_iq_tx_iq_mode);
			}
#endif
		}
		break;

		/* Set GSM TX IQ Band Parameter */
	case ACT_GSM_TX_IQ_TX_BAND:
		{
			u16 *vl_Value;
			vl_Value = (u16 *) value;
			switch (*vl_Value) {
			case 0:
				v_tatrf11_iq_setup.band =
				    G_TEST_UNINITIALIZED_GSM_BAND;
				break;

			case 1:
				v_tatrf11_iq_setup.band = G_TEST_GSM850;
				break;

			case 2:
				v_tatrf11_iq_setup.band = G_TEST_GSM900;
				break;

			case 3:
				v_tatrf11_iq_setup.band = G_TEST_GSM1800;
				break;

			case 4:
				v_tatrf11_iq_setup.band = G_TEST_GSM1900;
				break;

			default:
				/* Invalid request code */
				vl_error = TAT_BAD_REQ;
			}

			if (TAT_OK(vl_error)) {
				SYSLOG(LOG_DEBUG,
				       "Set GSM TX IQ band parameter: %d",
				       v_tatrf11_iq_setup.band);
			}
		}
		break;

	case ACT_GSM_TX_IQ_TX_MODULATION_TYPE:
		{
			/* RF band modifier: RF band should be set first. */
			switch (*((u32 *) value)) {
			case OPT_GSM_TX_IQ_TX_EDGE:

				/* set EDGE flag */
				v_tatrf11_iq_setup.band |= G_EDGE_ON;
				break;

			case OPT_GSM_TX_IQ_TX_GSM:
				v_tatrf11_iq_setup.band &= ~G_EDGE_ON;
				/* unset EDGE flag */
				break;

			default:
				/* problem here */
				vl_error = TAT_BAD_REQ;
			}

			if (TAT_OK(vl_error)) {
				SYSLOG(LOG_DEBUG,
				       "Set GSM/EDGE TX IQ band parameter: 0x%X",
				       v_tatrf11_iq_setup.band);
			}
		}
		break;

		/* Set power level value. */
	case ACT_GSM_TX_IQ_TX_POWER_LEVEL:
		{
			u16 *vl_Value;
			vl_Value = (u16 *) value;

			v_tatrf11_iq_setup.power_level = *vl_Value;
			SYSLOG(LOG_DEBUG,
			       "Set GSM TX IQ power level parameter: %d",
			       v_tatrf11_iq_setup.power_level);
		}
		break;

		/* Set channel value. */
	case ACT_GSM_TX_IQ_TX_CHANNEL:
		{
			u16 *vl_Value;
			vl_Value = (u16 *) value;

			v_tatrf11_iq_setup.channel_nbr = *vl_Value;
			SYSLOG(LOG_DEBUG, "Set GSM TX IQ channel parameter: %d",
			       v_tatrf11_iq_setup.channel_nbr);
		}
		break;

		/* Set I DC offset value. */
	case ACT_GSM_TX_IQ_I_DC_OFFSET:
		{
			s16 *vl_Value;
			vl_Value = (s16 *) value;

			v_tatrf11_iq_setup.dc_offset_i = *vl_Value;
			SYSLOG(LOG_DEBUG, "Set GSM TX IQ I DC offset value: %d",
			       v_tatrf11_iq_setup.dc_offset_i);
		}
		break;

		/* Set Q DC offset value. */
	case ACT_GSM_TX_IQ_Q_DC_OFFSET:
		{
			s16 *vl_Value;
			vl_Value = (s16 *) value;

			v_tatrf11_iq_setup.dc_offset_q = *vl_Value;
			SYSLOG(LOG_DEBUG, "Set GSM TX IQ Q DC offset value: %d",
			       v_tatrf11_iq_setup.dc_offset_q);
		}
		break;

		/* Set amplitude offset value. */
	case ACT_GSM_TX_IQ_AMPL_OFFSET:
		{
			s16 *vl_Value;
			vl_Value = (s16 *) value;

			v_tatrf11_iq_setup.ampl_offset = *vl_Value;
			SYSLOG(LOG_DEBUG,
			       "Set GSM TX IQ amplitude offset value: %d",
			       v_tatrf11_iq_setup.ampl_offset);
		}
		break;

	default:

		/* Invalid request code */
		vl_error = TAT_BAD_REQ;
	}

	return vl_error;
}

int tatl11_gsmTxIqSelfTuning(void)
{
	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	C_TEST_IQ_SELF_TUNING_REQ_STR *pl_req;
	pl_req =
	    tatrf_isi_msg_begin(&vl_msg_info, PN_DSP_COMMON_TEST,
	            C_TEST_IQ_SELF_TUNING_REQ, sub_block_count);
	if (NULL == pl_req) {
		return TAT_ISI_HANDLER_ERROR;
	}
	pl_req->tx_iq_mode = v_tatrf11_iq_tx_iq_mode;
	pl_req->fill1 = 0x0000;

	/* add GSM TX IQ subblock. */
	C_TEST_SB_GSM_TX_IQ_SETUP_STR *pl_sb_iq_setup =
	    tatrf_isi_msg_add_sb_copy(&vl_msg_info, C_TEST_SB_GSM_TX_IQ_SETUP,
	                  &v_tatrf11_iq_setup,
	                  sizeof(v_tatrf11_iq_setup));
	if (NULL == pl_sb_iq_setup) {
		return TAT_ISI_HANDLER_ERROR;
	}
	pl_sb_iq_setup->pa_loop_mode = G_TEST_PA_MODE_NONE;
	pl_sb_iq_setup->fill1 = 0xAA55;

	/* send ISI message - blocking function */
	int vl_Error = tatrf_isi_send(&vl_msg_info, "GSM TX IQ Self tuning");
	if (!TAT_OK(vl_Error)) {
		return vl_Error;
	}

	/* message successfully sent. waiting for response */
	memset(&v_tatrf11_iq_resp, 0, sizeof(v_tatrf11_iq_resp));
	memset(&v_tatrf11_iq_offset_result, 0, sizeof(v_tatrf11_iq_offset_result));
	memset(&v_tatrf11_iq_ampl_result, 0, sizeof(v_tatrf11_iq_ampl_result));
	memset(&v_tatrf11_iq_phase_result, 0, sizeof(v_tatrf11_iq_phase_result));

	C_TEST_IQ_SELF_TUNING_RESP_STR *pl_resp =
	    tatrf_isi_read(&vl_msg_info, C_TEST_IQ_SELF_TUNING_RESP,
	           sub_block_count, "GSM TX IQ Self tuning response",
	           &vl_Error);
	if (NULL != pl_resp) {
		/* this what we expected */
		v_tatrf11_iq_resp = *pl_resp;

		/* copy returned C_TEST_SB_DC_OFFSET_RESULT subblock */
		C_TEST_SB_DC_OFFSET_RESULT_STR *pl_sbr_offset_result =
		    tatrf_isi_find_sb(&vl_msg_info, C_TEST_SB_DC_OFFSET_RESULT,
		              NULL);
		if (NULL != pl_sbr_offset_result) {
			v_tatrf11_iq_offset_result = *pl_sbr_offset_result;
		}

		/* copy returned C_TEST_SB_AMPLITUDE_RESULT subblock */
		C_TEST_SB_AMPLITUDE_RESULT_STR *pl_sbr_ampl_result =
		    tatrf_isi_find_sb(&vl_msg_info, C_TEST_SB_AMPLITUDE_RESULT,
				      NULL);
		if (NULL != pl_sbr_ampl_result) {
			v_tatrf11_iq_ampl_result = *pl_sbr_ampl_result;
		}

		/* copy returned C_TEST_SB_PHASE_RESULT subblock */
		C_TEST_SB_PHASE_RESULT_STR *pl_sbr_phase_result =
		    tatrf_isi_find_sb(&vl_msg_info, C_TEST_SB_PHASE_RESULT,
				      NULL);
		if (NULL != pl_sbr_phase_result) {
			v_tatrf11_iq_phase_result = *pl_sbr_phase_result;
		}
	}

	return vl_Error;
}

int tatl11_gsmPaCompressionSelfTuning(void)
{
	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	C_TEST_TUNING_REQ_STR *pl_req =
	    tatrf_isi_msg_begin(&vl_msg_info, PN_DSP_COMMON_TEST,
				C_TEST_TUNING_REQ, numberOfSubBlocks);
	if (NULL == pl_req) {
		return TAT_ISI_HANDLER_ERROR;
	}

	/* add GSM PA subblock. */
	C_TEST_SB_GSM_PA_COMPRESSION_TUNING_STR *pl_sb_pa =
	    tatrf_isi_msg_add_sb_copy(&vl_msg_info,
				      C_TEST_SB_GSM_PA_COMPRESSION_TUNING,
				      &v_tatrf11_pa_compression,
				      sizeof(v_tatrf11_pa_compression));
	if (NULL == pl_sb_pa) {
		return TAT_ISI_HANDLER_ERROR;
	}
	pl_sb_pa->control_word = CONTROL_WORD;

	/* send ISI message - blocking function */
	int vl_Error =
	    tatrf_isi_send(&vl_msg_info, "GSM PA compression Self tuning");
	if (!TAT_OK(vl_Error)) {
		return vl_Error;
	}

	/* message successfully sent. waiting for response */
	memset(&v_tatrf11_pa_resp, 0, sizeof(v_tatrf11_pa_resp));
	memset(&v_tatrf11_pa_results, 0, sizeof(v_tatrf11_pa_results));

	C_TEST_TUNING_RESP_STR *pl_resp =
	    tatrf_isi_read(&vl_msg_info, C_TEST_TUNING_RESP, numerOfSubBlocks,
			   "GSM PA compression Self tuning response",
			   &vl_Error);
	if (NULL != pl_resp) {
		/* this what we expected */
		v_tatrf11_pa_resp = *pl_resp;

		/* copy returned C_TEST_SB_GSM_PA_COMPRESSION_TUNING_RESULTS subblock */
		C_TEST_SB_GSM_PA_COMPRESSION_TUNING_RESULTS_STR
		    *pl_sbr_pa_result =
		    tatrf_isi_find_sb(&vl_msg_info,
				      C_TEST_SB_GSM_PA_COMPRESSION_TUNING_RESULTS,
				      NULL);
		if (NULL != pl_sbr_pa_result) {
		    v_tatrf11_pa_results = *pl_sbr_pa_result;
		} else {
			vl_Error = TAT_ERROR_NOT_MATCHING_MSG;
		}
	}

	return vl_Error;
}

