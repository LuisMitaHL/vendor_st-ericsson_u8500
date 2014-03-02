/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   tunepwr.c
* \brief   routines to self-tune internal power metter
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "rfdef.h"
#include "tunepwr.h"

#include "misc.h"
#include "pmmacces.h"
#include "pmmidmap.h"
#include "isimsg.h"

/*----------------------------------------------------------------------------*/
/* Constants                                                                  */
/*----------------------------------------------------------------------------*/

/* QUOTIENT constants. */
#define QUOT_TARGET_POWER			    6
#define QUOT_WCDMA_TARGET_POWER	        3
#define QUOT_TX_RFIC_AGC_INDEX          2
#define NO_QUOTIENT                     0

/*----------------------------------------------------------------------------*/
/* Global variables															  */
/*----------------------------------------------------------------------------*/

/* ex MSG_C_TEST_TUNING_REQ_GSM_TX_PWR_TUNING */
C_TEST_TUNING_REQ_STR v_tatrf_tx_power_req = {
    .id = C_TEST_TUNING_REQ,
    .fill1 = 0xAA55,
    .fill2 = 0xAA55,
    .numberOfSubBlocks = 0
};
C_TEST_SB_GSM_TX_PWR_TUNING_STR v_tatrf_tx_power_req_sb = {
    .sb_id = C_TEST_SB_GSM_TX_PWR_TUNING,
    .sb_len = SIZE_C_TEST_SB_GSM_TX_PWR_TUNING_STR,
    .rf_band = INFO_NO_GSM,
    .channel_nbr = 0u,
    .fill1 = 0xAA55,
    .edge_on = 0u,
    .nbr_of_ctrl_data = 0u
};

GSM_TX_PWR_SELFTUNING_CTRL_DATA_STR v_tatrf_tx_power_req_sb_data[MAX_TUNING_DATA] = {
    { .gsm_pwr_level = 0u, .target_dbm = 0, .fill1 = 0xAA55, .pa_data_valid = 0u },
    { .gsm_pwr_level = 0u, .target_dbm = 0, .fill1 = 0xAA55, .pa_data_valid = 0u },
    { .gsm_pwr_level = 0u, .target_dbm = 0, .fill1 = 0xAA55, .pa_data_valid = 0u },
    { .gsm_pwr_level = 0u, .target_dbm = 0, .fill1 = 0xAA55, .pa_data_valid = 0u },
    { .gsm_pwr_level = 0u, .target_dbm = 0, .fill1 = 0xAA55, .pa_data_valid = 0u },
    { .gsm_pwr_level = 0u, .target_dbm = 0, .fill1 = 0xAA55, .pa_data_valid = 0u },
    { .gsm_pwr_level = 0u, .target_dbm = 0, .fill1 = 0xAA55, .pa_data_valid = 0u },
    { .gsm_pwr_level = 0u, .target_dbm = 0, .fill1 = 0xAA55, .pa_data_valid = 0u },
    { .gsm_pwr_level = 0u, .target_dbm = 0, .fill1 = 0xAA55, .pa_data_valid = 0u },
    { .gsm_pwr_level = 0u, .target_dbm = 0, .fill1 = 0xAA55, .pa_data_valid = 0u },
    { .gsm_pwr_level = 0u, .target_dbm = 0, .fill1 = 0xAA55, .pa_data_valid = 0u },
    { .gsm_pwr_level = 0u, .target_dbm = 0, .fill1 = 0xAA55, .pa_data_valid = 0u },
    { .gsm_pwr_level = 0u, .target_dbm = 0, .fill1 = 0xAA55, .pa_data_valid = 0u },
    { .gsm_pwr_level = 0u, .target_dbm = 0, .fill1 = 0xAA55, .pa_data_valid = 0u },
    { .gsm_pwr_level = 0u, .target_dbm = 0, .fill1 = 0xAA55, .pa_data_valid = 0u },
    { .gsm_pwr_level = 0u, .target_dbm = 0, .fill1 = 0xAA55, .pa_data_valid = 0u },
    { .gsm_pwr_level = 0u, .target_dbm = 0, .fill1 = 0xAA55, .pa_data_valid = 0u },
    { .gsm_pwr_level = 0u, .target_dbm = 0, .fill1 = 0xAA55, .pa_data_valid = 0u },
    { .gsm_pwr_level = 0u, .target_dbm = 0, .fill1 = 0xAA55, .pa_data_valid = 0u },
    { .gsm_pwr_level = 0u, .target_dbm = 0, .fill1 = 0xAA55, .pa_data_valid = 0u }
};

/* ex MSG_C_TEST_TUNING_REQ_WCDMA_TX_PWR_TUNING */
C_TEST_SB_WCDMA_TX_TUNING_STR v_tatrf_wcdma_tx_power_req_sb = {
    .sb_id = C_TEST_SB_WCDMA_TX_TUNING,
    .sb_len = SIZE_C_TEST_SB_WCDMA_TX_TUNING_STR,
    .step_length = 0u,
    .tuning_ctrl_opt = 0,
    .rf_band = INFO_NO_WCDMA,
    .channel_nbr = 0u,
    .tuning_target = 0u,
    .pwr_meter_att = 0u,
    .pwr_ctrl_value = 0u,
    .pwr_ctrl_step = 0,
    .dpdch_weight = 0u,
    .dpcch_weight = 0u,
    .pa_ctrl_data_size = 0u
};

WCDMA_TX_PWR_CTRL_DATA_STR v_tatrf_wcdma_tx_power_req_sb_data[MAX_WCDMA_TUNING_DATA];

/* ex MSG_C_TEST_TUNING_RESP_GSM_TX_TUNING_RESULTS */
C_TEST_TUNING_RESP_STR v_tatrf_tx_power_resp = {
    .id = C_TEST_TUNING_RESP,
    .status = MAKE16(ISIFILLER8, C_TEST_OK),
    .fill2 = 0xAA55,
    .numerOfSubBlocks = 0u
};

C_TEST_SB_TX_TUNING_RESULTS_STR v_tatrf_tx_power_resp_results = {
    .sb_id = C_TEST_SB_TX_TUNING_RESULTS,
    .sb_len = SIZE_C_TEST_SB_TX_TUNING_RESULTS_STR,
    .status = MAKE16(ISIFILLER8, C_TEST_OK),
    .system_info = C_TEST_GSM,
    .system_dep_data = 0u,
    .max_pwr = 0,
    .amount_of_values = 0u
};

TX_PWR_TUNING_DATA_STR v_tatrf_tx_power_resp_results_data[MAX_WCDMA_TUNING_DATA] = {
    { .measured_pwr = 0, .pwr_ctrl = 0 },   /* 0 */
    { .measured_pwr = 0, .pwr_ctrl = 0 },
    { .measured_pwr = 0, .pwr_ctrl = 0 },
    { .measured_pwr = 0, .pwr_ctrl = 0 },
    { .measured_pwr = 0, .pwr_ctrl = 0 },
    { .measured_pwr = 0, .pwr_ctrl = 0 },
    { .measured_pwr = 0, .pwr_ctrl = 0 },
    { .measured_pwr = 0, .pwr_ctrl = 0 },
    { .measured_pwr = 0, .pwr_ctrl = 0 },
    { .measured_pwr = 0, .pwr_ctrl = 0 },
    { .measured_pwr = 0, .pwr_ctrl = 0 },   /* 10 */
    { .measured_pwr = 0, .pwr_ctrl = 0 },
    { .measured_pwr = 0, .pwr_ctrl = 0 },
    { .measured_pwr = 0, .pwr_ctrl = 0 },
    { .measured_pwr = 0, .pwr_ctrl = 0 },
    { .measured_pwr = 0, .pwr_ctrl = 0 },
    { .measured_pwr = 0, .pwr_ctrl = 0 },
    { .measured_pwr = 0, .pwr_ctrl = 0 },
    { .measured_pwr = 0, .pwr_ctrl = 0 },
    { .measured_pwr = 0, .pwr_ctrl = 0 },
    { .measured_pwr = 0, .pwr_ctrl = 0 },   /* 20 */
    { .measured_pwr = 0, .pwr_ctrl = 0 },
    { .measured_pwr = 0, .pwr_ctrl = 0 },
    { .measured_pwr = 0, .pwr_ctrl = 0 },
    { .measured_pwr = 0, .pwr_ctrl = 0 },
    { .measured_pwr = 0, .pwr_ctrl = 0 },
    { .measured_pwr = 0, .pwr_ctrl = 0 },
    { .measured_pwr = 0, .pwr_ctrl = 0 },
    { .measured_pwr = 0, .pwr_ctrl = 0 },
    { .measured_pwr = 0, .pwr_ctrl = 0 },
    { .measured_pwr = 0, .pwr_ctrl = 0 },   /* 30 */
    { .measured_pwr = 0, .pwr_ctrl = 0 }
};

float v_tatrf_input_target_power[GSM_EGDE_PANEL_TUNING_DATA] = {
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
};

s16 v_tatrf_input_pa_vcc[GSM_EGDE_PANEL_TUNING_DATA] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

s16 v_tatrf_input_pa_bias[GSM_EGDE_PANEL_TUNING_DATA] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

u8 v_tatrf_shift = 0u;
u16 v_tatrf_pa_data_valid = 0x0000 ;
u32 v_tatrf_band = 0x00000000 ;

/* ----------------------------------------------------------------------- */
/* Module private functions                                                */
/* ----------------------------------------------------------------------- */

int tatl9_04SwapPwrSelftuningCtrlData(int vp_nb_tuning_data, GSM_TX_PWR_SELFTUNING_CTRL_DATA_STR *pp_data);

/**
 * Read data from the PMM.
 * @param[in] elem		DTH element.
 * @param[in] value		memory area where data read will be stored.
 * @param[in] id 		identifiant of the paremeter to read.
 * @param[in] quotient 	quotient used by the modem software.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl9_11ReadDataFromPmm(struct dth_element *elem, void *value, int id,
    int quotient);

/* Function tide to Start GSM Tx power level       */
/**************************************************/
int DthRf_Power_Tx_Get(struct dth_element *elem, void *value)
{
	return tatl9_02Pwr_SelfTuning_Get(elem, value);
}

int DthRf_Power_Tx_Set(struct dth_element *elem, void *value)
{
	return tatl9_03Pwr_SelfTuning_Set(elem, value);
}

int DthRf_Power_Tx_Exec(struct dth_element *elem)
{
	return tatl9_00Pwr_SelfTuning_Exec(elem);
}

int tatl9_selftuning_tx(int vp_mode);

int tatl9_00Pwr_SelfTuning_Exec(struct dth_element *elem)
{
	int vl_Error = TAT_ERROR_OFF;

	switch (elem->user_data) {
	case ACT_8PSK_TX:
	case ACT_GMSK_TX:	/* Lauch 8PSK/GMSK power level test. */

		vl_Error = tatl9_selftuning_tx(0);
		break;

	case ACT_WCDMA_TX:	/* Lauch WCDMA power level test. */

		vl_Error = tatl9_selftuning_tx(1);
		break;

	default:
		vl_Error = TAT_BAD_REQ;
	}

	return vl_Error;
}

int tatl9_02Pwr_SelfTuning_Get(struct dth_element *elem, void *value)
{
	int vl_error;
	size_t vl_position = 0;
	int16 id = 0;

	vl_error = TAT_ERROR_OFF;

	switch (elem->user_data) {
	case ACT_8PSK_TX_ERROR_CODE:
	case ACT_WCDMA_TX_ERROR_CODE:
	case ACT_GMSK_TX_ERROR_CODE:
		{
			u8 *vl_Value;
			vl_Value = (u8 *) value;
			if (LOWBYTE(v_tatrf_tx_power_resp.id) ==
			    C_TEST_TUNING_RESP) {
				*vl_Value =
				    v_tatrf_tx_power_resp_results.status;
				SYSLOG(LOG_DEBUG,
				       "Get GMSK Tx power level request status: %d",
				       *vl_Value);
			}
		}
		break;

	case ACT_8PSK_TX_MAX_POWER:
	case ACT_WCDMA_TX_MAX_POWER:
	case ACT_GMSK_TX_MAX_POWER:
		{
			float *pMaxPwr;
			pMaxPwr = (float *)value;
			if (LOWBYTE(v_tatrf_tx_power_resp.id) ==
			    C_TEST_TUNING_RESP) {
				*pMaxPwr =
				    RF_UNQ(v_tatrf_tx_power_resp_results.
					   max_pwr, QUOT_TARGET_POWER);
				SYSLOG(LOG_DEBUG, "Get max power value: %lf",
				       *pMaxPwr);
			}
		}
		break;

	case ACT_8PSK_TX_MEASURED_POWER:
	case ACT_WCDMA_TX_MEASURED_POWER:
	case ACT_GMSK_TX_MEASURED_POWER:
		{
			TX_PWR_TUNING_DATA_STR *vl_tuning_data;
			struct dth_array *vl_array;
			float vl_measured_pwr[WCDMA_PANEL_TUNING_DATA];
			int vl_nb_tuning_data;
			int i;

			vl_array = (struct dth_array *)value;
			memset(vl_measured_pwr, 0, sizeof(vl_measured_pwr));
			vl_tuning_data = NULL;
			vl_nb_tuning_data = 0;
			i = 0;

			SYSLOG(LOG_DEBUG, "Get measured power :");

			if (LOWBYTE(v_tatrf_tx_power_resp.id) ==
			    C_TEST_TUNING_RESP) {
				vl_tuning_data =
				    v_tatrf_tx_power_resp_results_data;
				vl_nb_tuning_data =
				    v_tatrf_tx_power_resp_results.
				    amount_of_values;

				if (vl_nb_tuning_data <=
				    WCDMA_PANEL_TUNING_DATA) {
					for (i = 0; i < vl_nb_tuning_data; i++) {
						vl_measured_pwr[i +
								v_tatrf_shift] =
						    RF_UNQ((vl_tuning_data +
							    i)->measured_pwr,
							   QUOT_TARGET_POWER);
					}

					if ((elem->user_data ==
					     ACT_8PSK_TX_MEASURED_POWER)
					    || (elem->user_data ==
						ACT_GMSK_TX_MEASURED_POWER)) {
						GetDthArray(elem, vl_array,
							    sizeof(float),
							    vl_measured_pwr,
							    sizeof(float),
							    GSM_EGDE_PANEL_TUNING_DATA);
					} else {
						GetDthArray(elem, vl_array,
							    sizeof(float),
							    vl_measured_pwr,
							    sizeof(float),
							    WCDMA_PANEL_TUNING_DATA);
					}

					if (vl_array->col == elem->cols
					    && vl_array->row == elem->rows) {
						for (i = 0;
						     i <
						     WCDMA_PANEL_TUNING_DATA;
						     i++) {
							SYSLOGSTR(LOG_DEBUG,
								  "%d = %lf", i,
								  vl_measured_pwr
								  [i]);
						}
					} else {
						i = vl_array->col;
						SYSLOGSTR(LOG_DEBUG, "%d = %lf",
							  i,
							  vl_measured_pwr[i]);
					}
				}
			}
		}
		break;

	case ACT_8PSK_TX_RFIC_AGC_INDEX:
	case ACT_WCDMA_TX_RFIC_AGC_INDEX:
	case ACT_GMSK_TX_RFIC_AGC_INDEX:
		{
			TX_PWR_TUNING_DATA_STR *vl_tuning_data;
			struct dth_array *vl_array;
			s16 vl_agc_index[WCDMA_PANEL_TUNING_DATA];
			float vl_agc_index_f[WCDMA_PANEL_TUNING_DATA];
			int vl_nb_tuning_data, vl_max_nb_data = 0;
			int i;

			vl_array = (struct dth_array *)value;
			memset(vl_agc_index, 0, sizeof(vl_agc_index));
			vl_tuning_data = NULL;
			vl_nb_tuning_data = 0;
			i = 0;

			SYSLOG(LOG_DEBUG, "Get all Power control DAC values :");

			if (LOWBYTE(v_tatrf_tx_power_resp.id) ==
			    C_TEST_TUNING_RESP) {
				vl_tuning_data =
				    v_tatrf_tx_power_resp_results_data;
				vl_nb_tuning_data =
				    v_tatrf_tx_power_resp_results.
				    amount_of_values;

				if (vl_nb_tuning_data <=
				    WCDMA_PANEL_TUNING_DATA) {
					if (ACT_WCDMA_TX_RFIC_AGC_INDEX ==
					    elem->user_data) {
						vl_max_nb_data =
						    WCDMA_PANEL_TUNING_DATA;
					} else {
						vl_max_nb_data =
						    GSM_EGDE_PANEL_TUNING_DATA;
					}

					if ((ACT_8PSK_TX_RFIC_AGC_INDEX ==
					     elem->user_data)
					    || (ACT_WCDMA_TX_RFIC_AGC_INDEX ==
						elem->user_data)) {
						/* case high precision AGC gain index */
						for (i = 0;
						     i < vl_nb_tuning_data;
						     i++) {
							vl_agc_index_f[i +
								       v_tatrf_shift]
							    =
							    RF_UNQ((vl_tuning_data + i)->pwr_ctrl, QUOT_TX_RFIC_AGC_INDEX);
						}

						GetDthArray(elem, vl_array,
							    sizeof(float),
							    vl_agc_index_f,
							    sizeof(float),
							    vl_max_nb_data);

						if (vl_array->col == elem->cols
						    && vl_array->row ==
						    elem->rows) {
							for (i = 0;
							     i < vl_max_nb_data;
							     i++) {
								SYSLOGSTR
								    (LOG_DEBUG,
								     " %d = %lf\n",
								     i,
								     vl_agc_index_f
								     [i]);
							}
						} else {
							i = vl_array->col;
							SYSLOG(LOG_DEBUG,
							       "%d = %lf", i,
							       vl_agc_index_f
							       [i]);
						}
					} else {
						/* case low precision AGC gain index */
						for (i = 0;
						     i < vl_nb_tuning_data;
						     i++) {
							vl_agc_index[i +
								     v_tatrf_shift]
							    =
							    (vl_tuning_data +
							     i)->pwr_ctrl;
						}

						GetDthArray(elem, vl_array,
							    sizeof(s16),
							    vl_agc_index,
							    sizeof(s16),
							    vl_max_nb_data);

						if (vl_array->col == elem->cols
						    && vl_array->row ==
						    elem->rows) {
							for (i = 0;
							     i < vl_max_nb_data;
							     i++) {
								SYSLOGSTR
								    (LOG_DEBUG,
								     " %d = %d\n",
								     i,
								     vl_agc_index
								     [i]);
							}
						} else {
							i = vl_array->col;
							SYSLOG(LOG_DEBUG,
							       "%d = %d", i,
							       vl_agc_index[i]);
						}
					}
				}

				/* The memory area used for the response must be re-initialized for the next response
				   when the last output parameter is sent to the PC part. */
				memset(&v_tatrf_tx_power_resp, 0,
				       sizeof(v_tatrf_tx_power_resp));
				memset(&v_tatrf_tx_power_resp_results, 0,
				       sizeof(v_tatrf_tx_power_resp_results));
				memset(v_tatrf_tx_power_resp_results_data, 0,
				       sizeof
				       (v_tatrf_tx_power_resp_results_data));
			}
		}
		break;

	case ACT_GMSK_TX_TARGET_POWER:
		{
			switch (v_tatrf_band) {
			case INFO_GSM850:
				{
					if ((id =
					     tatl14_06GetRfParamId
					     ("GMSK_GSM850_TX_TARGET_POWER",
					      &vl_position)) < 0) {
						SYSLOG(LOG_ERR,
						       "GMSK_GSM850_TX_TARGET_POWER parameter does not exist");
					} else {
						vl_error =
						    tatl9_11ReadDataFromPmm
						    (elem, value, id,
						     QUOT_TARGET_POWER);
					}
				}
				break;

			case INFO_GSM900:
				{
					if ((id =
					     tatl14_06GetRfParamId
					     ("GMSK_GSM900_TX_TARGET_POWER",
					      &vl_position)) < 0) {
						SYSLOG(LOG_ERR,
						       "GMSK_GSM900_TX_TARGET_POWER parameter does not exist");
					} else {
						vl_error =
						    tatl9_11ReadDataFromPmm
						    (elem, value, id,
						     QUOT_TARGET_POWER);
					}
				}
				break;

			case INFO_GSM1800:
				{
					if ((id =
					     tatl14_06GetRfParamId
					     ("GMSK_GSM1800_TX_TARGET_POWER",
					      &vl_position)) < 0) {
						SYSLOG(LOG_ERR,
						       "GMSK_GSM1800_TX_TARGET_POWER parameter does not exist");
					} else {
						vl_error =
						    tatl9_11ReadDataFromPmm
						    (elem, value, id,
						     QUOT_TARGET_POWER);
					}
				}
				break;

			case INFO_GSM1900:
				{
					if ((id =
					     tatl14_06GetRfParamId
					     ("GMSK_GSM1900_TX_TARGET_POWER",
					      &vl_position)) < 0) {
						SYSLOG(LOG_ERR,
						       "GMSK_GSM1900_TX_TARGET_POWER parameter does not exist");
					} else {
						vl_error =
						    tatl9_11ReadDataFromPmm
						    (elem, value, id,
						     QUOT_TARGET_POWER);
					}
				}
				break;

			default:
				{
					if ((id =
					     tatl14_06GetRfParamId
					     ("GMSK_GSM850_TX_TARGET_POWER",
					      &vl_position)) < 0) {
						SYSLOG(LOG_ERR,
						       "GMSK_GSM850_TX_TARGET_POWER parameter does not exist");
					} else {
						vl_error =
						    tatl9_11ReadDataFromPmm
						    (elem, value, id,
						     QUOT_TARGET_POWER);
					}
				}
				break;
			}
		}
		break;

	case ACT_GMSK_TX_PA_VCC:
		{
			/* Not necessary for the moment. 
			   switch (v_tatrf_band)
			   {
			   case INFO_GSM850:
			   {
			   if( (id = tatl14_02GetRfParamId("GMSK_GSM850_TX_PWR_COEFF", &vl_position)) < 0)
			   {
			   SYSLOG(LOG_ERR, "GMSK_GSM850_TX_PWR_COEFF parameter does not exist"); 
			   }
			   else
			   {
			   vl_error = tatl9_11ReadDataFromPmm(elem, value, id, NO_QUOTIENT);
			   }
			   }
			   break;

			   case INFO_GSM900:
			   {
			   if( (id = tatl14_02GetRfParamId("GMSK_GSM900_TX_PWR_COEFF", &vl_position)) < 0)
			   {
			   SYSLOG(LOG_ERR, "GMSK_GSM900_TX_PWR_COEFF parameter does not exist"); 
			   }
			   else
			   {
			   vl_error = tatl9_11ReadDataFromPmm(elem, value, id, NO_QUOTIENT);
			   }
			   }
			   break;

			   case INFO_GSM1800:
			   {
			   if( (id = tatl14_02GetRfParamId("GMSK_GSM1800_TX_PWR_COEFF", &vl_position)) < 0)
			   {
			   SYSLOG(LOG_ERR, "GMSK_GSM1800_TX_PWR_COEFF parameter does not exist"); 
			   }
			   else
			   {
			   vl_error = tatl9_11ReadDataFromPmm(elem, value, id, NO_QUOTIENT);
			   }
			   }
			   break;

			   case INFO_GSM1900:
			   {
			   if( (id = tatl14_02GetRfParamId("GMSK_GSM1900_TX_PWR_COEFF", &vl_position)) < 0)
			   {
			   SYSLOG(LOG_ERR, "GMSK_GSM1900_TX_PWR_COEFF parameter does not exist"); 
			   }
			   else
			   {
			   vl_error = tatl9_11ReadDataFromPmm(elem, value, id, NO_QUOTIENT);
			   }
			   }
			   break;

			   default:
			   {
			   if( (id = tatl14_02GetRfParamId("GMSK_GSM850_TX_PWR_COEFF", &vl_position)) < 0)
			   {
			   SYSLOG(LOG_ERR, "GMSK_GSM850_TX_PWR_COEFF parameter does not exist"); 
			   }
			   else
			   {
			   vl_error = tatl9_11ReadDataFromPmm(elem, value, id, NO_QUOTIENT);
			   }
			   }        
			   break;
			   } */
		}
		break;

	case ACT_GMSK_TX_PA_BIAS:
		{
			/* Not necessary for the moment.
			   switch (v_tatrf_band)
			   {
			   case INFO_GSM850:
			   {
			   if( (id = tatl14_02GetRfParamId("GSM850_GERAN_GSM_TX_BIAS_TABLE", &vl_position)) < 0)
			   {
			   SYSLOG(LOG_ERR, "GSM850_GERAN_GSM_TX_BIAS_TABLE parameter does not exist"); 
			   }
			   else
			   {
			   vl_error = tatl9_11ReadDataFromPmm(elem, value, id, NO_QUOTIENT);
			   }
			   }
			   break;

			   case INFO_GSM900:
			   {
			   if( (id = tatl14_02GetRfParamId("GSM900_GERAN_GSM_TX_BIAS_TABLE", &vl_position)) < 0)
			   {
			   SYSLOG(LOG_ERR, "GSM900_GERAN_GSM_TX_BIAS_TABLE parameter does not exist"); 
			   }
			   else
			   {
			   vl_error = tatl9_11ReadDataFromPmm(elem, value, id, NO_QUOTIENT);
			   }
			   }
			   break;

			   case INFO_GSM1800:
			   {
			   if( (id = tatl14_02GetRfParamId("GSM1800_GERAN_GSM_TX_BIAS_TABLE", &vl_position)) < 0)
			   {
			   SYSLOG(LOG_ERR, "GSM1800_GERAN_GSM_TX_BIAS_TABLE parameter does not exist"); 
			   }
			   else
			   {
			   vl_error = tatl9_11ReadDataFromPmm(elem, value, id, NO_QUOTIENT);
			   }
			   }
			   break;

			   case INFO_GSM1900:
			   {
			   if( (id = tatl14_02GetRfParamId("GSM1900_GERAN_GSM_TX_BIAS_TABLE", &vl_position)) < 0)
			   {
			   SYSLOG(LOG_ERR, "GSM1900_GERAN_GSM_TX_BIAS_TABLE parameter does not exist"); 
			   }
			   else
			   {
			   vl_error = tatl9_11ReadDataFromPmm(elem, value, id, NO_QUOTIENT);
			   }
			   }
			   break;

			   default:
			   {
			   if( (id = tatl14_02GetRfParamId("GSM850_GERAN_GSM_TX_BIAS_TABLE", &vl_position)) < 0)
			   {
			   SYSLOG(LOG_ERR, "GSM850_GERAN_GSM_TX_BIAS_TABLE parameter does not exist"); 
			   }
			   else
			   {
			   vl_error = tatl9_11ReadDataFromPmm(elem, value, id, NO_QUOTIENT);
			   }
			   }        
			   break;
			   } */
		}
		break;

	case ACT_8PSK_TX_TARGET_POWER:
		{
			switch (v_tatrf_band) {
			case INFO_GSM850:
				{
					if ((id =
					     tatl14_06GetRfParamId
					     ("8PSK_GSM850_TX_TARGET_POWER",
					      &vl_position)) < 0) {
						SYSLOG(LOG_ERR,
						       "8PSK_GSM850_TX_TARGET_POWER parameter does not exist");
					} else {
						vl_error =
						    tatl9_11ReadDataFromPmm
						    (elem, value, id,
						     QUOT_TARGET_POWER);
					}
				}
				break;

			case INFO_GSM900:
				{
					if ((id =
					     tatl14_06GetRfParamId
					     ("8PSK_GSM900_TX_TARGET_POWER",
					      &vl_position)) < 0) {
						SYSLOG(LOG_ERR,
						       "8PSK_GSM900_TX_TARGET_POWER parameter does not exist");
					} else {
						vl_error =
						    tatl9_11ReadDataFromPmm
						    (elem, value, id,
						     QUOT_TARGET_POWER);
					}
				}
				break;

			case INFO_GSM1800:
				{
					if ((id =
					     tatl14_06GetRfParamId
					     ("8PSK_GSM1800_TX_TARGET_POWER",
					      &vl_position)) < 0) {
						SYSLOG(LOG_ERR,
						       "8PSK_GSM1800_TX_TARGET_POWER parameter does not exist");
					} else {
						vl_error =
						    tatl9_11ReadDataFromPmm
						    (elem, value, id,
						     QUOT_TARGET_POWER);
					}
				}
				break;

			case INFO_GSM1900:
				{
					if ((id =
					     tatl14_06GetRfParamId
					     ("8PSK_GSM1900_TX_TARGET_POWER",
					      &vl_position)) < 0) {
						SYSLOG(LOG_ERR,
						       "8PSK_GSM1900_TX_TARGET_POWER parameter does not exist");
					} else {
						vl_error =
						    tatl9_11ReadDataFromPmm
						    (elem, value, id,
						     QUOT_TARGET_POWER);
					}
				}
				break;

			default:
				{
					if ((id =
					     tatl14_06GetRfParamId
					     ("8PSK_GSM850_TX_TARGET_POWER",
					      &vl_position)) < 0) {
						SYSLOG(LOG_ERR,
						       "8PSK_GSM850_TX_TARGET_POWER parameter does not exist");
					} else {
						vl_error =
						    tatl9_11ReadDataFromPmm
						    (elem, value, id,
						     QUOT_TARGET_POWER);
					}
				}
				break;
			}
		}
		break;

	case ACT_8PSK_TX_PA_VCC:
		{
			/* Not necessary for the moment.
			   switch (v_tatrf_band)
			   {
			   case INFO_GSM850:
			   {
			   if( (id = tatl14_06GetRfParamId("8PSK_GSM850_PA_VOLT", &vl_position)) < 0)
			   {
			   SYSLOG(LOG_ERR, "8PSK_GSM850_PA_VOLT parameter does not exist"); 
			   }
			   else
			   {
			   vl_error = tatl9_11ReadDataFromPmm(elem, value, id, NO_QUOTIENT);
			   }
			   }
			   break;

			   case INFO_GSM900:
			   {
			   if( (id = tatl14_02GetRfParamId("8PSK_GSM900_PA_VOLT", &vl_position)) < 0)
			   {
			   SYSLOG(LOG_ERR, "8PSK_GSM900_PA_VOLT parameter does not exist"); 
			   }
			   else
			   {
			   vl_error = tatl9_11ReadDataFromPmm(elem, value, id, NO_QUOTIENT);
			   }
			   }
			   break;

			   case INFO_GSM1800:
			   {
			   if( (id = tatl14_02GetRfParamId("8PSK_GSM1800_PA_VOLT", &vl_position)) < 0)
			   {
			   SYSLOG(LOG_ERR, "8PSK_GSM1800_PA_VOLT parameter does not exist"); 
			   }
			   else
			   {
			   vl_error = tatl9_11ReadDataFromPmm(elem, value, id, NO_QUOTIENT);
			   }
			   }
			   break;

			   case INFO_GSM1900:
			   {
			   if( (id = tatl14_02GetRfParamId("8PSK_GSM1900_PA_VOLT", &vl_position)) < 0)
			   {
			   SYSLOG(LOG_ERR, "8PSK_GSM1900_PA_VOLT parameter does not exist"); 
			   }
			   else
			   {
			   vl_error = tatl9_11ReadDataFromPmm(elem, value, id, NO_QUOTIENT);
			   }
			   }
			   break;

			   default:
			   {
			   if( (id = tatl14_02GetRfParamId("8PSK_GSM850_PA_VOLT", &vl_position)) < 0)
			   {
			   SYSLOG(LOG_ERR, "8PSK_GSM850_PA_VOLT parameter does not exist"); 
			   }
			   else
			   {
			   vl_error = tatl9_11ReadDataFromPmm(elem, value, id, NO_QUOTIENT);
			   }
			   }        
			   break;
			   } */
		}
		break;

	case ACT_8PSK_TX_PA_BIAS:
		{
			switch (v_tatrf_band) {
				/* No PMM id defined for 8PSK_PA_VCC
				   PMM_8PSK_GSM850_PA_BIAS
				   case INFO_GSM850:
				   {
				   if( (id = tatl14_02GetRfParamId("8PSK_GSM850_PA_BIAST", &vl_position)) < 0)
				   {
				   SYSLOG(LOG_ERR, "8PSK_GSM850_PA_BIAS parameter does not exist"); 
				   }
				   else
				   {
				   vl_error = tatl9_11ReadDataFromPmm(elem, value, id, NO_QUOTIENT);
				   }
				   }
				   break;

				   case INFO_GSM900:
				   {
				   if( (id = tatl14_02GetRfParamId("8PSK_GSM850_PA_BIAS", &vl_position)) < 0)
				   {
				   SYSLOG(LOG_ERR, "8PSK_GSM850_PA_BIAS parameter does not exist"); 
				   }
				   else
				   {
				   vl_error = tatl9_11ReadDataFromPmm(elem, value, id, NO_QUOTIENT);
				   }
				   }
				   break;

				   case INFO_GSM1800:
				   {
				   if( (id = tatl14_02GetRfParamId("8PSK_GSM850_PA_BIAS", &vl_position)) < 0)
				   {
				   SYSLOG(LOG_ERR, "8PSK_GSM850_PA_BIAS parameter does not exist"); 
				   }
				   else
				   {
				   vl_error = tatl9_11ReadDataFromPmm(elem, value, id, NO_QUOTIENT);
				   }
				   }
				   break;

				   case INFO_GSM1900:
				   {
				   if( (id = tatl14_02GetRfParamId("8PSK_GSM850_PA_BIAS", &vl_position)) < 0)
				   {
				   SYSLOG(LOG_ERR, "8PSK_GSM850_PA_BIAS parameter does not exist"); 
				   }
				   else
				   {
				   vl_error = tatl9_11ReadDataFromPmm(elem, value, id, NO_QUOTIENT);
				   }
				   }
				   break;

				   default:
				   {
				   if( (id = tatl14_02GetRfParamId("8PSK_GSM850_PA_BIAS", &vl_position)) < 0)
				   {
				   SYSLOG(LOG_ERR, "8PSK_GSM850_PA_BIAS parameter does not exist"); 
				   }
				   else
				   {
				   vl_error = tatl9_11ReadDataFromPmm(elem, value, id, NO_QUOTIENT);
				   }
				   }    
				   break; */
			}
		}
		break;

	case ACT_WCDMA_TX_PA_VCC:
		{
			switch (v_tatrf_band) {
			case INFO_WCDMA_FDD_BAND1:
				{
					if ((id =
					     tatl14_06GetRfParamId
					     ("UTRAN_PA_VOLT_A",
					      &vl_position)) < 0) {
						SYSLOG(LOG_ERR,
						       "UTRAN_PA_VOLT_A parameter does not exist");
					} else {
						vl_error =
						    tatl9_11ReadDataFromPmm
						    (elem, value, id,
						     NO_QUOTIENT);
					}
				}
				break;

			case INFO_WCDMA_FDD_BAND2:
				{
					if ((id =
					     tatl14_06GetRfParamId
					     ("UTRAN_PA_VOLT_B",
					      &vl_position)) < 0) {
						SYSLOG(LOG_ERR,
						       "UTRAN_PA_VOLT_B parameter does not exist");
					} else {
						vl_error =
						    tatl9_11ReadDataFromPmm
						    (elem, value, id,
						     NO_QUOTIENT);
					}
				}
				break;

			case INFO_WCDMA_FDD_BAND3:
				{
					if ((id =
					     tatl14_06GetRfParamId
					     ("UTRAN_PA_VOLT_C",
					      &vl_position)) < 0) {
						SYSLOG(LOG_ERR,
						       "UTRAN_PA_VOLT_C parameter does not exist");
					} else {
						vl_error =
						    tatl9_11ReadDataFromPmm
						    (elem, value, id,
						     NO_QUOTIENT);
					}
				}
				break;

			case INFO_WCDMA_FDD_BAND4:
				{
					if ((id =
					     tatl14_06GetRfParamId
					     ("UTRAN_PA_VOLT_D",
					      &vl_position)) < 0) {
						SYSLOG(LOG_ERR,
						       "UTRAN_PA_VOLT_D parameter does not exist");
					} else {
						vl_error =
						    tatl9_11ReadDataFromPmm
						    (elem, value, id,
						     NO_QUOTIENT);
					}
				}
				break;

			case INFO_WCDMA_FDD_BAND5:
				{
					if ((id =
					     tatl14_06GetRfParamId
					     ("UTRAN_PA_VOLT_E",
					      &vl_position)) < 0) {
						SYSLOG(LOG_ERR,
						       "UTRAN_PA_VOLT_E parameter does not exist");
					} else {
						vl_error =
						    tatl9_11ReadDataFromPmm
						    (elem, value, id,
						     NO_QUOTIENT);
					}
				}
				break;

			default:
				{
					if ((id =
					     tatl14_06GetRfParamId
					     ("UTRAN_PA_VOLT_A",
					      &vl_position)) < 0) {
						SYSLOG(LOG_ERR,
						       "UTRAN_PA_VOLT_A parameter does not exist");
					} else {
						vl_error =
						    tatl9_11ReadDataFromPmm
						    (elem, value, id,
						     NO_QUOTIENT);
					}
				}
				break;
			}
		}
		break;

	case ACT_WCDMA_TX_PA_BIAS:
		{
			switch (v_tatrf_band) {
			case INFO_WCDMA_FDD_BAND1:
				{
					if ((id =
					     tatl14_06GetRfParamId
					     ("UTRAN_PA_BIAS_A",
					      &vl_position)) < 0) {
						SYSLOG(LOG_ERR,
						       "UTRAN_PA_BIAS_A parameter does not exist");
					} else {
						vl_error =
						    tatl9_11ReadDataFromPmm
						    (elem, value, id,
						     NO_QUOTIENT);
					}
				}
				break;

			case INFO_WCDMA_FDD_BAND2:
				{
					if ((id =
					     tatl14_06GetRfParamId
					     ("UTRAN_PA_BIAS_B",
					      &vl_position)) < 0) {
						SYSLOG(LOG_ERR,
						       "UTRAN_PA_BIAS_B parameter does not exist");
					} else {
						vl_error =
						    tatl9_11ReadDataFromPmm
						    (elem, value, id,
						     NO_QUOTIENT);
					}
				}
				break;

			case INFO_WCDMA_FDD_BAND3:
				{
					if ((id =
					     tatl14_06GetRfParamId
					     ("UTRAN_PA_BIAS_C",
					      &vl_position)) < 0) {
						SYSLOG(LOG_ERR,
						       "UTRAN_PA_BIAS_C parameter does not exist");
					} else {
						vl_error =
						    tatl9_11ReadDataFromPmm
						    (elem, value, id,
						     NO_QUOTIENT);
					}
				}
				break;

			case INFO_WCDMA_FDD_BAND4:
				{
					if ((id =
					     tatl14_06GetRfParamId
					     ("UTRAN_PA_BIAS_D",
					      &vl_position)) < 0) {
						SYSLOG(LOG_ERR,
						       "UTRAN_PA_BIAS_D parameter does not exist");
					} else {
						vl_error =
						    tatl9_11ReadDataFromPmm
						    (elem, value, id,
						     NO_QUOTIENT);
					}
				}
				break;

			case INFO_WCDMA_FDD_BAND5:
				{
					if ((id =
					     tatl14_06GetRfParamId
					     ("UTRAN_PA_BIAS_E",
					      &vl_position)) < 0) {
						SYSLOG(LOG_ERR,
						       "UTRAN_PA_BIAS_E parameter does not exist");
					} else {
						vl_error =
						    tatl9_11ReadDataFromPmm
						    (elem, value, id,
						     NO_QUOTIENT);
					}
				}
				break;

			default:
				{
					if ((id =
					     tatl14_06GetRfParamId
					     ("UTRAN_PA_BIAS_A",
					      &vl_position)) < 0) {
						SYSLOG(LOG_ERR,
						       "UTRAN_PA_BIAS_A parameter does not exist");
					} else {
						vl_error =
						    tatl9_11ReadDataFromPmm
						    (elem, value, id,
						     NO_QUOTIENT);
					}
				}
				break;
			}
		}
		break;

	default:

		/* Invalid request code */
		vl_error = TAT_BAD_REQ;
	}

	return vl_error;
}

int tatl9_03Pwr_SelfTuning_Set(struct dth_element *elem, void *value)
{
	int vl_error;

	vl_error = TAT_ERROR_OFF;

	switch (elem->user_data) {
	case ACT_GMSK_TX:
		{
			v_tatrf_tx_power_req_sb.edge_on = EDGE_OFF;
			SYSLOG(LOG_DEBUG, "Set Tx power level => edge_on=0");
		}
		break;

	case ACT_8PSK_TX:
		{
			v_tatrf_tx_power_req_sb.edge_on = EDGE_ON;
			SYSLOG(LOG_DEBUG, "Set Tx power level => edge_on=1");
		}
		break;

	case ACT_8PSK_TX_BAND:
	case ACT_GMSK_TX_BAND:
		{
			u16 *vl_Value;
			vl_Value = (u16 *) value;

			v_tatrf_tx_power_req_sb.rf_band =
			    tatl3_04GetGsmRfBand(*vl_Value);
			v_tatrf_band = v_tatrf_tx_power_req_sb.rf_band;

			if (v_tatrf_tx_power_req_sb.rf_band != INFO_NO_GSM) {
				SYSLOG(LOG_DEBUG,
				       "set NO BAND(0), GSM850(1), GSM900(2), DCS1800(3), PCS1900(4): %u",
				       *vl_Value);
				SYSLOG(LOG_DEBUG, "set GSM band info to %lu",
				       v_tatrf_tx_power_req_sb.rf_band);
			} else {
				vl_error = TAT_BAD_REQ;
			}
		}
		break;

	case ACT_8PSK_TX_CHANNEL:
	case ACT_GMSK_TX_CHANNEL:
		{
			u16 *vl_Value;
			vl_Value = (u16 *) value;

			v_tatrf_tx_power_req_sb.channel_nbr = *vl_Value;
			SYSLOG(LOG_DEBUG, "Set Tx Channel parameter: %d",
			       v_tatrf_tx_power_req_sb.channel_nbr);
		}
		break;

	case ACT_GMSK_TX_PA_DATA_VALID:
	case ACT_8PSK_TX_PA_DATA_VALID:
		{
			u16 *vl_Value;
			vl_Value = (u16 *) value;

			v_tatrf_pa_data_valid = *vl_Value;
			SYSLOG(LOG_DEBUG,
			       "Set Tx data valid INTERNAL(0) EXTERNAL(1) : %d",
			       v_tatrf_pa_data_valid);
		}
		break;

	case ACT_8PSK_TX_TARGET_POWER:
	case ACT_GMSK_TX_TARGET_POWER:
		{
			struct dth_array *vl_array;
			int i;

			vl_array = (struct dth_array *)value;
			i = 0;

			SetDthArray(elem, vl_array, sizeof(float),
				    v_tatrf_input_target_power, sizeof(float),
				    GSM_EGDE_PANEL_TUNING_DATA);

			SYSLOG(LOG_DEBUG,
			       "Set Target Power table parameter (dBm):");
			if (vl_array->col == elem->cols
			    && vl_array->row == elem->rows) {
				for (i = 0; i < GSM_EGDE_PANEL_TUNING_DATA; i++) {
					SYSLOGSTR(LOG_DEBUG, " %d = %lf", i,
						  v_tatrf_input_target_power
						  [i]);
				}
			} else {
				i = vl_array->col;
				SYSLOG(LOG_DEBUG, " %d = %lf", i,
				       v_tatrf_input_target_power[i]);
			}
		}
		break;

	case ACT_8PSK_TX_PA_VCC:
	case ACT_GMSK_TX_PA_VCC:
		{
			struct dth_array *vl_array;
			int i;

			vl_array = (struct dth_array *)value;
			i = 0;

			SetDthArray(elem, vl_array, sizeof(s16),
				    v_tatrf_input_pa_vcc, sizeof(s16),
				    GSM_EGDE_PANEL_TUNING_DATA);

			SYSLOG(LOG_DEBUG, "Set PA VCC table parameter:");
			if (vl_array->col == elem->cols
			    && vl_array->row == elem->rows) {
				for (i = 0; i < GSM_EGDE_PANEL_TUNING_DATA; i++) {
					SYSLOGSTR(LOG_DEBUG, " %d = %d", i,
						  v_tatrf_input_pa_vcc[i]);
				}
			} else {
				i = vl_array->col;
				SYSLOGSTR(LOG_DEBUG, " %d = %d", i,
					  v_tatrf_input_pa_vcc[i]);
			}
		}
		break;

	case ACT_GMSK_TX_PA_BIAS:
		{
			struct dth_array *vl_array;
			int i;

			vl_array = (struct dth_array *)value;
			i = 0;

			SetDthArray(elem, vl_array, sizeof(s16),
				    v_tatrf_input_pa_bias, sizeof(s16),
				    GSM_EGDE_PANEL_TUNING_DATA);

			SYSLOG(LOG_DEBUG, "Set PA bias table parameter:");
			if (vl_array->col == elem->cols
			    && vl_array->row == elem->rows) {
				for (i = 0; i < GSM_EGDE_PANEL_TUNING_DATA; i++) {
					SYSLOGSTR(LOG_DEBUG, " %d = %d", i,
						  v_tatrf_input_pa_bias[i]);
				}
			} else {
				i = vl_array->col;
				SYSLOGSTR(LOG_DEBUG, " %d = %d", i,
					  v_tatrf_input_pa_bias[i]);
			}
		}
		break;

	case ACT_8PSK_TX_PA_BIAS:
		{
			v_tatrf_input_pa_bias[0] = *((s16 *) (value));
			SYSLOG(LOG_DEBUG, "Set PA bias parameter: %d",
			       v_tatrf_input_pa_bias[0]);
		}
		break;

	case ACT_WCDMA_TX_CHANNEL:
		{
			u16 *vl_Value;
			vl_Value = (u16 *) value;

			v_tatrf_wcdma_tx_power_req_sb.channel_nbr = *vl_Value;
			SYSLOG(LOG_DEBUG, "Set WCDMA Tx Channel parameter: %d",
			       v_tatrf_wcdma_tx_power_req_sb.channel_nbr);
		}
		break;

	case ACT_WCDMA_TX_TARGET_PWR_START:
		{
			float *vl_Value;
			vl_Value = (float *)value;

			SYSLOG(LOG_DEBUG,
			       "Set WCDMA Max tuned power level parameter: %f",
			       *vl_Value);
			v_tatrf_wcdma_tx_power_req_sb.tuning_target =
			    RF_Q(*vl_Value, QUOT_WCDMA_TARGET_POWER);

		}
		break;

	case ACT_WCDMA_TX_PWR_CTRL_STEP:
		{
			s16 *vl_Value;
			vl_Value = (s16 *) value;

			v_tatrf_wcdma_tx_power_req_sb.pwr_ctrl_step = *vl_Value;
			SYSLOG(LOG_DEBUG, "Set index step size: %d",
			       v_tatrf_wcdma_tx_power_req_sb.pwr_ctrl_step);
		}
		break;

	case ACT_WCDMA_TX_AMOUNT_CTRL_DATA:
		{
			u16 *vl_Value;
			vl_Value = (u16 *) value;

			v_tatrf_wcdma_tx_power_req_sb.pa_ctrl_data_size =
			    *vl_Value;
			SYSLOG(LOG_DEBUG,
			       "Set WCDMA amount of control data: %d",
			       v_tatrf_wcdma_tx_power_req_sb.pa_ctrl_data_size);
		}
		break;

	case ACT_WCDMA_TX_TUNING_OPTIONS:
		{
			u16 *vl_Value;
			vl_Value = (u16 *) value;

			v_tatrf_wcdma_tx_power_req_sb.tuning_ctrl_opt =
			    *vl_Value;
			SYSLOG(LOG_DEBUG, "Set tuning ctrl opt: %d",
			       v_tatrf_wcdma_tx_power_req_sb.tuning_ctrl_opt);
		}
		break;

	case ACT_WCDMA_TX_STEP_DURATION:
		{
			u16 *vl_Value;
			vl_Value = (u16 *) value;

			v_tatrf_wcdma_tx_power_req_sb.step_length = *vl_Value;
			SYSLOG(LOG_DEBUG,
			       "Set WCDMA step duration parameter: %d",
			       v_tatrf_wcdma_tx_power_req_sb.step_length);
		}
		break;

	case ACT_WCDMA_TX_PA_VCC:
		{
			WCDMA_TX_PWR_CTRL_DATA_STR *vl_tuning_data;
			struct dth_array *vl_array;
			s16 *vl_pa_vcc;
			int vl_nb_tuning_data;
			int i;

			vl_tuning_data = v_tatrf_wcdma_tx_power_req_sb_data;
			vl_array = (struct dth_array *)value;
			vl_pa_vcc = (s16 *) vl_array->array;
			vl_nb_tuning_data =
			    v_tatrf_wcdma_tx_power_req_sb.pa_ctrl_data_size;
			i = 0;

			SYSLOG(LOG_DEBUG, "Set WCDMA PA VCC table parameter:");
			for (i = 0; i < vl_nb_tuning_data; i++) {
				(vl_tuning_data + i)->pa_voltage_ctrl =
				    *(vl_pa_vcc + i);
				SYSLOGSTR(LOG_DEBUG, " %d = %d \n ", i,
					  (vl_tuning_data +
					   i)->pa_voltage_ctrl);
			}
		}
		break;

	case ACT_WCDMA_TX_PA_BIAS:
		{
			WCDMA_TX_PWR_CTRL_DATA_STR *vl_tuning_data;
			struct dth_array *vl_array;
			s16 *vl_pa_bias;
			int vl_nb_tuning_data;
			int i;

			vl_tuning_data = v_tatrf_wcdma_tx_power_req_sb_data;
			vl_array = (struct dth_array *)value;
			vl_pa_bias = (s16 *) vl_array->array;
			vl_nb_tuning_data =
			    v_tatrf_wcdma_tx_power_req_sb.pa_ctrl_data_size;
			i = 0;

			SYSLOG(LOG_DEBUG, "Set WCDMA PA bias table parameter:");
			for (i = 0; i < vl_nb_tuning_data; i++) {
				(vl_tuning_data + i)->pa_ictrl =
				    *(vl_pa_bias + i);
				SYSLOGSTR(LOG_DEBUG, " %d = %d \n ", i,
					  (vl_tuning_data + i)->pa_ictrl);
			}
		}
		break;

	case ACT_WCDMA_TX_PWR_CTRL_START_VALUE:
		{
			u16 *vl_Value;
			vl_Value = (u16 *) value;

			v_tatrf_wcdma_tx_power_req_sb.pwr_ctrl_value =
			    *vl_Value;
			SYSLOG(LOG_DEBUG,
			       "Set WCDMA Tx Power control start value parameter: %u ",
			       v_tatrf_wcdma_tx_power_req_sb.pwr_ctrl_value);
		}
		break;

	case ACT_WCDMA_TX_BAND:
		{
			u32 *vl_Value;
			vl_Value = (u32 *) value;

			v_tatrf_wcdma_tx_power_req_sb.rf_band =
			    tatl3_06GetWcdmaRfBand(*vl_Value);
			v_tatrf_band = v_tatrf_wcdma_tx_power_req_sb.rf_band;
			SYSLOG(LOG_DEBUG,
			       "set NO BAND(0), BAND1(1), BAND2(2), BAND3(3), BAND4(4), BAND5(5), BAND6(6), BAND7(7), BAND8(8), BAND9(9): %u",
			       *vl_Value);
			SYSLOG(LOG_DEBUG, "set WCDMA band info to %lu",
			       v_tatrf_wcdma_tx_power_req_sb.rf_band);
		}
		break;

	default:
		/* Invalid request code */
		vl_error = TAT_BAD_REQ;
	}

	return vl_error;
}

int tatl9_11ReadDataFromPmm(struct dth_element *elem, void *value, int id,
			    int quotient)
{
	u32 vl_old_mem_type;
	u32 vl_mem_type;
	int vl_error;
	struct dth_element vl_pmm_elem;

	vl_old_mem_type = 0;
	vl_mem_type = 1;	/* PMM memory. */
	vl_error = 0;
	memset(&vl_pmm_elem, 0, sizeof(vl_pmm_elem));

	/* Get the old PMM memory type. */
	vl_pmm_elem.user_data = ACT_PMM_TYPE;
	if ((vl_error =
	     tatl5_01Pmm_GetAndResult(&vl_pmm_elem, &vl_old_mem_type)) == 0) {
		/* Set the new PMM memory type. */
		if ((vl_error =
		     tatl5_02Pmm_Set(&vl_pmm_elem, &vl_mem_type)) == 0) {
			/* Read data from PMM memory. */
			vl_pmm_elem.path = "dummy";
			vl_pmm_elem.user_data = id;
			vl_pmm_elem.cols = elem->cols;
			vl_pmm_elem.rows = elem->rows;
			vl_pmm_elem.type = elem->type;
			if ((vl_error =
			     tatl5_03Pmm_Read(&vl_pmm_elem, value,
					      quotient)) == 0) {
				/* Set the old PMM memory type. */
				vl_pmm_elem.user_data = ACT_PMM_TYPE;
				vl_error =
				    tatl5_02Pmm_Set(&vl_pmm_elem,
						    &vl_old_mem_type);
			}
		}
	}

	return vl_error;
}

int tatl9_getTx2GData(struct tatrf_isi_msg_t *pp_st)
{
	/* @precond: pp_st->sb is a non-null C_TEST_SB_GSM_TX_PWR_TUNING_STR* */
	/* TODO: check precondition. */

	C_TEST_SB_GSM_TX_PWR_TUNING_STR *pl_sb =
	    (C_TEST_SB_GSM_TX_PWR_TUNING_STR *) pp_st->sb;
	int vl_nb_tuning_data = 0;
	int vl_pwr_level_nb = 0;
	int i = 0;

	int vl_Error = TAT_ERROR_OFF;
	switch (v_tatrf_tx_power_req_sb.rf_band) {
		/* low bands */
	case INFO_GSM850:
	case INFO_GSM900:

		/* GSM or EDGE? */
		switch (v_tatrf_tx_power_req_sb.edge_on) {
		case EDGE_OFF:

			vl_nb_tuning_data = GSM_LOW_BAND_TUNING_DATA;
			vl_pwr_level_nb = GSM_LOW_BAND_PWR_LEVEL_START;
			v_tatrf_shift = GSM_LOW_BAND_SHIFT;
			break;

		case EDGE_ON:

			vl_nb_tuning_data = EDGE_LOW_BAND_TUNING_DATA;
			vl_pwr_level_nb = EDGE_LOW_BAND_PWR_LEVEL_START;
			v_tatrf_shift = EDGE_LOW_BAND_SHIFT;
			break;

		default:
			vl_Error = TAT_BAD_REQ;
		}

		break;		/* case low bands */

	case INFO_GSM1800:
	case INFO_GSM1900:

		/* GSM or EDGE? */
		switch (v_tatrf_tx_power_req_sb.edge_on) {
		case EDGE_OFF:

			vl_nb_tuning_data = GSM_HIGH_BAND_TUNING_DATA;
			vl_pwr_level_nb = GSM_HIGH_BAND_PWR_LEVEL_START;
			v_tatrf_shift = GSM_HIGH_BAND_SHIFT;
			break;

		case EDGE_ON:

			vl_nb_tuning_data = EDGE_HIGH_BAND_TUNING_DATA;
			vl_pwr_level_nb = EDGE_HIGH_BAND_PWR_LEVEL_START;
			v_tatrf_shift = EDGE_HIGH_BAND_SHIFT;
			break;

		default:
			vl_Error = TAT_BAD_REQ;
		}

		break;		/* case high bands */

	default:
		vl_Error = TAT_BAD_REQ;
	}

	if (!TAT_OK(vl_Error))
		return vl_Error;

	/* @precond not too many tuning data */
	if (MAX_TUNING_DATA < vl_nb_tuning_data) {
		return TAT_MISC_ERR;
	}

	/* fill subblock members */
	pl_sb->nbr_of_ctrl_data = vl_nb_tuning_data;

	/* append one GSM_TX_PWR_SELFTUNING_CTRL_DATA_STR structure per data */
	GSM_TX_PWR_SELFTUNING_CTRL_DATA_STR *pl_tuning_data =
	    v_tatrf_tx_power_req_sb_data;
	for (i = 0; TAT_OK(vl_Error) && (i < vl_nb_tuning_data); i++) {
		pl_tuning_data->gsm_pwr_level = vl_pwr_level_nb;
		pl_tuning_data->fill1 = 0x0000;
		pl_tuning_data->pa_data_valid = v_tatrf_pa_data_valid;
		pl_tuning_data->target_dbm =
		    RF_Q(v_tatrf_input_target_power[i + v_tatrf_shift],
			 QUOT_TARGET_POWER);
		if (v_tatrf_pa_data_valid == 0) {	/* INTERNAL */
			pl_tuning_data->pa_ctrl_data.pa_voltage_ctrl = 0x0000;
			pl_tuning_data->pa_ctrl_data.pa_ictrl = 0x0000;
		} else if (v_tatrf_pa_data_valid == 1) {	/* EXTERNAL */
			pl_tuning_data->pa_ctrl_data.pa_voltage_ctrl =
			    v_tatrf_input_pa_vcc[i + v_tatrf_shift];

			if (EDGE_ON == v_tatrf_tx_power_req_sb.edge_on) {
				/* EDGE: apply the unique PA bias entered */
				pl_tuning_data->pa_ctrl_data.pa_ictrl =
				    v_tatrf_input_pa_bias[0];
			} else {
				/* GSM */
				pl_tuning_data->pa_ctrl_data.pa_ictrl =
				    v_tatrf_input_pa_bias[i + v_tatrf_shift];
			}
		}

		vl_Error =
		    tatrf_isi_append_data(pp_st, (u16 *) pl_tuning_data,
					  SIZE_GSM_TX_PWR_SELFTUNING_CTRL_DATA_STR
					  / sizeof(u16));

		vl_pwr_level_nb++;
		pl_tuning_data++;
	}

	/* @postcond: subblock len is correct */
	if (pl_sb->sb_len !=
	    vl_nb_tuning_data * SIZE_GSM_TX_PWR_SELFTUNING_CTRL_DATA_STR +
	    SIZE_C_TEST_SB_GSM_TX_PWR_TUNING_STR) {
		SYSLOG(LOG_WARNING, "subblock length not correct!");
	}

	return TAT_ERROR_OFF;
}

int tatl9_selftuning_tx(int vp_mode)
{
	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	C_TEST_TUNING_REQ_STR *pl_req;
	pl_req =
	    tatrf_isi_msg_begin(&vl_msg_info, PN_DSP_COMMON_TEST,
				C_TEST_TUNING_REQ, numberOfSubBlocks);
	if (NULL == pl_req) {
		return TAT_ISI_HANDLER_ERROR;
	}

	/* add a subblock */
	int vl_Error = TAT_ERROR_CASE;
	if (vp_mode == 0) {
		/* TX 2G */
		C_TEST_SB_GSM_TX_PWR_TUNING_STR *pl_sb =
		    tatrf_isi_msg_add_sb_copy(&vl_msg_info,
					      C_TEST_SB_GSM_TX_PWR_TUNING,
					      &v_tatrf_tx_power_req_sb,
					      sizeof(v_tatrf_tx_power_req_sb));
		if (NULL == pl_sb) {
			return TAT_ISI_HANDLER_ERROR;
		}

		/* push data into subblock */
		vl_Error = tatl9_getTx2GData(&vl_msg_info);
		if (!TAT_OK(vl_Error)) {
			return vl_Error;
		}

		/* send ISI message - blocking function */
		vl_Error = tatrf_isi_send(&vl_msg_info, "Self tuning TX 2G");
		if (!TAT_OK(vl_Error)) {
			return vl_Error;
		}
	} else {
		/* TX Wcdma */
		v_tatrf_shift = 0;	/* no output shift in WCDMA */

		C_TEST_SB_WCDMA_TX_TUNING_STR *pl_sb =
		    tatrf_isi_msg_add_sb_copy(&vl_msg_info,
					      C_TEST_SB_WCDMA_TX_TUNING,
					      &v_tatrf_wcdma_tx_power_req_sb,
					      sizeof
					      (v_tatrf_wcdma_tx_power_req_sb));
		if (NULL == pl_sb) {
			return TAT_ISI_HANDLER_ERROR;
		}
		pl_sb->pwr_meter_att = PWR_METER_ATT_START;
		pl_sb->dpdch_weight = DPDCH_CHANNEL_WEIGHT;
		pl_sb->dpcch_weight = DPCCH_CHANNEL_WEIGHT;

		/* pl_sb->pa_ctrl: push data into subblock */
		vl_Error =
		    tatrf_isi_append_data(&vl_msg_info,
					  (u16 *)
					  v_tatrf_wcdma_tx_power_req_sb_data,
					  sizeof
					  (v_tatrf_wcdma_tx_power_req_sb_data) /
					  sizeof(u16));
		if (!TAT_OK(vl_Error)) {
			return vl_Error;
		}

		/* @postcond: subblock len is correct */
		if (pl_sb->sb_len !=
		    pl_sb->pa_ctrl_data_size * SIZE_TX_PWR_TUNING_DATA_STR +
		    SIZE_C_TEST_SB_WCDMA_TX_TUNING_STR) {
			SYSLOG(LOG_WARNING, "subblock length not correct!");
		}

		/* send ISI message - blocking function */
		vl_Error = tatrf_isi_send(&vl_msg_info, "Self tuning TX WCDMA");
		if (!TAT_OK(vl_Error)) {
			return vl_Error;
		}
	}

	memset(&v_tatrf_tx_power_resp, 0, sizeof(v_tatrf_tx_power_resp));
	memset(&v_tatrf_tx_power_resp_results, 0,
	       sizeof(v_tatrf_tx_power_resp_results));
	memset(v_tatrf_tx_power_resp_results_data, 0,
	       sizeof(v_tatrf_tx_power_resp_results_data));

	/* message successfully sent. waiting for response */
	C_TEST_TUNING_RESP_STR *pl_resp =
	    tatrf_isi_read(&vl_msg_info, C_TEST_TUNING_RESP, numerOfSubBlocks,
			   "Self tuning TX response", &vl_Error);
	if (NULL != pl_resp) {
		/* this what we expected */
		v_tatrf_tx_power_resp = *pl_resp;

		/* response should have returned a C_TEST_SB_TX_TUNING_RESULTS subblock */
		C_TEST_SB_TX_TUNING_RESULTS_STR *pl_sbr =
		    tatrf_isi_find_sb(&vl_msg_info, C_TEST_SB_TX_TUNING_RESULTS,
				      NULL);
		if (NULL != pl_sbr) {
			v_tatrf_tx_power_resp_results = *pl_sbr;

			/* TODO: check that buffer has enough capacity to receive data */
			memcpy(v_tatrf_tx_power_resp_results_data,
			       pl_sbr->measured_pwr,
			       pl_sbr->amount_of_values *
			       SIZE_TX_PWR_TUNING_DATA_STR);
			vl_Error = TAT_ERROR_OFF;
		} else {
			vl_Error = TAT_ERROR_NOT_MATCHING_MSG;
		}
	}

	return vl_Error;
}

