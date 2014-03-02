/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   isidump.c
* \brief   routines to print ISI messages and subblocks in a human readable way
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "rfdef.h"
#include "isidump.h"
#include "misc.h"
#include "rftuning.h"

void tatl17_dump_sb_common(void *pp_block, int vp_prio)
{
	u16 pl_sb_id = DEREF_PTR(pp_block, u16);

	SYSLOGSTR(vp_prio, "\n");
	switch (pl_sb_id) {
    case C_TEST_SB_PA_INFO:
        {
            C_TEST_SB_PA_INFO_STR *pl_sb = (C_TEST_SB_PA_INFO_STR *)pp_block;
            SYSLOGSTR(vp_prio, "C_TEST_SB_PA_INFO:\n");
            SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
            SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
            SYSLOGSTR(vp_prio, "status = %u\n", pl_sb->status);
            SYSLOGSTR(vp_prio, "system_info = 0x%X\n", pl_sb->system_info);
            SYSLOGSTR(vp_prio, "status = %u\n", pl_sb->version);
            /* Manufacturer of PA in ASCII string */
            SYSLOGSTR(vp_prio, "manufacturer PA = %s\n",
                (const char *)pl_sb + SIZE_C_TEST_SB_PA_INFO_STR);
        }
        break;

    case C_TEST_SB_PA_BIAS_INFO:
        {
            C_TEST_SB_PA_BIAS_INFO_STR *pl_sb =
                (C_TEST_SB_PA_BIAS_INFO_STR *)pp_block;
            SYSLOGSTR(vp_prio, "C_TEST_SB_PA_BIAS_INFO:\n");
            SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
            SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
            SYSLOGSTR(vp_prio, "status = %u\n", pl_sb->status);
            SYSLOGSTR(vp_prio, "system_info = 0x%X\n", pl_sb->system_info);
            SYSLOGSTR(vp_prio, "status = %u\n", pl_sb->bias1_const);
            SYSLOGSTR(vp_prio, "status = %u\n", pl_sb->bias1_current);
            SYSLOGSTR(vp_prio, "status = %u\n", pl_sb->bias2_default_dac);
            SYSLOGSTR(vp_prio, "status = %u\n", pl_sb->target_value);
        }
        break;

	case C_TEST_SB_GSM_RX_CONTROL:
		{
			C_TEST_SB_GSM_RX_CONTROL_STR *pl_sb =
			    (C_TEST_SB_GSM_RX_CONTROL_STR *) pp_block;
			SYSLOGSTR(vp_prio, "C_TEST_SB_GSM_RX_CONTROL:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
			SYSLOGSTR(vp_prio, "mode = %u\n", pl_sb->mode);
			SYSLOGSTR(vp_prio, "rf_band = 0x%lX\n", pl_sb->rf_band);
			SYSLOGSTR(vp_prio, "channel = %u\n", pl_sb->channel);
			SYSLOGSTR(vp_prio, "mon_ch = %u\n", pl_sb->mon_ch);
			SYSLOGSTR(vp_prio, "afc = %d\n", pl_sb->afc);
			SYSLOGSTR(vp_prio, "agc = %u\n", pl_sb->agc);
			SYSLOGSTR(vp_prio, "operationMode = %u\n",
				  pl_sb->operationMode);
		}
		break;

	case C_TEST_SB_GSM_RX_CONTROL_RESP:
		{
			C_TEST_SB_GSM_RX_CONTROL_RESP_STR *pl_sb =
			    (C_TEST_SB_GSM_RX_CONTROL_RESP_STR *) pp_block;
			SYSLOGSTR(vp_prio, "C_TEST_SB_GSM_RX_CONTROL_RESP:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
			SYSLOGSTR(vp_prio, "status = %u\n", pl_sb->status);
		}
		break;

    case C_TEST_SB_AFC_MEAS_AND_ADJUST:
        {
            C_TEST_SB_AFC_MEAS_AND_ADJUST_STR *pl_sb = (C_TEST_SB_AFC_MEAS_AND_ADJUST_STR *)pp_block;
            SYSLOGSTR(vp_prio, "C_TEST_SB_AFC_MEAS_AND_ADJUST:\n");
            SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
            SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
            SYSLOGSTR(vp_prio, "measurement mode = %u\n",
                pl_sb->measurementMode);
            SYSLOGSTR(vp_prio, "afc = %u\n", pl_sb->afcValue);
            SYSLOGSTR(vp_prio, "coarse = %u\n", pl_sb->coarseValue);
            SYSLOGSTR(vp_prio, "timeout = %u\n", pl_sb->adjustTimeout);
            SYSLOGSTR(vp_prio, "accuracy (th radian) = %u (Q11)\n",
                pl_sb->adjustAccuracy);
        }
        break;

    case C_TEST_SB_AFC_MEAS_AND_ADJUST_RESULTS:
        {
            C_TEST_SB_AFC_MEAS_AND_ADJUST_RESULTS_STR *pl_sb = (C_TEST_SB_AFC_MEAS_AND_ADJUST_RESULTS_STR *)pp_block;
            SYSLOGSTR(vp_prio, "C_TEST_SB_AFC_MEAS_AND_ADJUST_RESULTS:\n");
            SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
            SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
            SYSLOGSTR(vp_prio, "status = %02X\n", pl_sb->status);
            SYSLOGSTR(vp_prio, "freq. error (th radian) = %d (Q11)\n",
                pl_sb->freqErrorRad);
            SYSLOGSTR(vp_prio, "used AFC = %u\n", pl_sb->usedAfc);
            SYSLOGSTR(vp_prio, "usec coarse = %u\n", pl_sb->usedCoarse);
            SYSLOGSTR(vp_prio, "remaining timeout = %u\n",
                pl_sb->remainingTimeout);
        }
        break;

	case C_TEST_SB_LINKO_AFC_TUNING:
		{
			C_TEST_SB_LINKO_AFC_TUNING_STR *pl_sb =
			    (C_TEST_SB_LINKO_AFC_TUNING_STR *) pp_block;
			SYSLOGSTR(vp_prio, "C_TEST_SB_LINKO_AFC_TUNING:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
			SYSLOGSTR(vp_prio, "afc = %u\n", pl_sb->defaultAfc);
			SYSLOGSTR(vp_prio, "coarse = %u\n",
				  pl_sb->defaultCoarse);
		}
		break;

	case C_TEST_SB_LINKO_AFC_TUNING_RESULTS:
		{
			C_TEST_SB_LINKO_AFC_TUNING_RESULTS_STR *pl_sb =
			    (C_TEST_SB_LINKO_AFC_TUNING_RESULTS_STR *) pp_block;
			SYSLOGSTR(vp_prio,
				  "C_TEST_SB_LINKO_AFC_TUNING_RESULTS:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
			SYSLOGSTR(vp_prio, "status = %u\n", pl_sb->status);
			SYSLOGSTR(vp_prio, "c_coarse = %u\n", pl_sb->c_coarse);
			SYSLOGSTR(vp_prio, "i_bias_core = %u\n",
				  pl_sb->i_bias_core);
			SYSLOGSTR(vp_prio, "afc0_value = %d\n",
				  pl_sb->afc0_value);
			SYSLOGSTR(vp_prio, "temperature = %u\n",
				  pl_sb->temperature);
			SYSLOGSTR(vp_prio, "c_temp_k = %d\n", pl_sb->c_temp_k);
			SYSLOGSTR(vp_prio, "c_temp_b = %d\n", pl_sb->c_temp_b);
			SYSLOGSTR(vp_prio, "afc1_reading = %d\n",
				  pl_sb->afc1_reading);
			SYSLOGSTR(vp_prio, "afc2_reading = %d\n",
				  pl_sb->afc2_reading);
			SYSLOGSTR(vp_prio, "afc3_reading = %d\n",
				  pl_sb->afc3_reading);
			SYSLOGSTR(vp_prio, "afc4_reading = %d\n",
				  pl_sb->afc4_reading);
			SYSLOGSTR(vp_prio, "afc5_reading = %d\n",
				  pl_sb->afc5_reading);
			SYSLOGSTR(vp_prio, "afc1_value = %u\n",
				  pl_sb->afc1_value);
			SYSLOGSTR(vp_prio, "afc2_value = %u\n",
				  pl_sb->afc2_value);
			SYSLOGSTR(vp_prio, "afc3_value = %u\n",
				  pl_sb->afc3_value);
			SYSLOGSTR(vp_prio, "afc4_value = %u\n",
				  pl_sb->afc4_value);
			SYSLOGSTR(vp_prio, "afc5_value = %u\n",
				  pl_sb->afc5_value);
		}
		break;

	case C_HAL_SB_GSM_RX_CONTROL:
		{
			C_HAL_SB_GSM_RX_CONTROL_STR *pl_sb =
			    (C_HAL_SB_GSM_RX_CONTROL_STR *) pp_block;
			SYSLOGSTR(vp_prio, "C_HAL_SB_GSM_RX_CONTROL_STR:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
			SYSLOGSTR(vp_prio, "gsm_band_info = %lX\n",
				  pl_sb->gsm_band_info);
			SYSLOGSTR(vp_prio, "rx_channel = %u\n",
				  pl_sb->rx_channel);
			SYSLOGSTR(vp_prio, "mon_channel = %u\n",
				  pl_sb->mon_channel);
			SYSLOGSTR(vp_prio, "rx_slot_mask_afc_table = 0x%08X\n",
				  pl_sb->rx_slot_mask_afc_table);
			SYSLOGSTR(vp_prio, "manualAfc = %lu\n",
				  pl_sb->manualAfc);
			SYSLOGSTR(vp_prio, "agc_mode_fill2 = 0x%08X\n",
				  pl_sb->agc_mode_fill2);
			SYSLOGSTR(vp_prio, "rx_level = %u\n", pl_sb->rx_level);
		}
		break;

	case C_HAL_SB_GSM_TX_CONTROL:
		{
			C_HAL_SB_GSM_TX_CONTROL_STR *pl_sb =
			    (C_HAL_SB_GSM_TX_CONTROL_STR *) pp_block;
			SYSLOGSTR(vp_prio, "C_HAL_SB_GSM_TX_CONTROL_STR:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
			SYSLOGSTR(vp_prio, "gsm_band_info = %lX\n",
				  pl_sb->gsm_band_info);
			SYSLOGSTR(vp_prio, "tx_channel = %u\n",
				  pl_sb->tx_channel);
			SYSLOGSTR(vp_prio, "tx_slot_mask_afc_table = 0x%08X\n",
				  pl_sb->tx_slot_mask_afc_table);
			SYSLOGSTR(vp_prio, "manualAfc = %lu\n",
				  pl_sb->manualAfc);
			SYSLOGSTR(vp_prio, "tx_8psk_mask_data_type = 0x%08X\n",
				  pl_sb->tx_8psk_mask_data_type);
			SYSLOGSTR(vp_prio,
				  "tx_rach_mask_tx_power_unit = 0x%08X\n",
				  pl_sb->tx_rach_mask_tx_power_unit);
			SYSLOGSTR(vp_prio, "power_level = %u\n",
				  pl_sb->power_level);
		}
		break;

	case C_HAL_SB_WCDMA_RX_CONTROL:
		{
			C_HAL_SB_WCDMA_RX_CONTROL_STR *pl_sb =
			    (C_HAL_SB_WCDMA_RX_CONTROL_STR *) pp_block;
			SYSLOGSTR(vp_prio, "C_HAL_SB_WCDMA_RX_CONTROL_STR:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
			SYSLOGSTR(vp_prio, "band_info = %lX\n",
				  pl_sb->band_info);
			SYSLOGSTR(vp_prio, "dl_uarfcn = %u\n",
				  pl_sb->dl_uarfcn);
			SYSLOGSTR(vp_prio, "afc_table_fill1 = 0x%02X\n",
				  pl_sb->afc_table_fill1);
			SYSLOGSTR(vp_prio, "manualAfc = %lu\n",
				  pl_sb->manualAfc);
			SYSLOGSTR(vp_prio, "agc_mode_fill2 = 0x%02X\n",
				  pl_sb->agc_mode_fill2);
			SYSLOGSTR(vp_prio, "rx_level = %d\n", pl_sb->rx_level);
		}
		break;

	case C_HAL_SB_WCDMA_TX_CONTROL:
		{
			C_HAL_SB_WCDMA_TX_CONTROL_STR *pl_sb =
			    (C_HAL_SB_WCDMA_TX_CONTROL_STR *) pp_block;
			SYSLOGSTR(vp_prio, "C_HAL_SB_WCDMA_TX_CONTROL_STR:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
			SYSLOGSTR(vp_prio, "band_info = %lX\n",
				  pl_sb->band_info);
			SYSLOGSTR(vp_prio, "ul_uarfcn = %u\n",
				  pl_sb->ul_uarfcn);
			SYSLOGSTR(vp_prio, "power_level = %d\n",
				  pl_sb->power_level);
			SYSLOGSTR(vp_prio, "afc_table_fill1 = 0x%02X\n",
				  pl_sb->afc_table_fill1);
			SYSLOGSTR(vp_prio, "manualAfc = %u\n",
				  pl_sb->manualAfc);
			SYSLOGSTR(vp_prio,
				  "dpdch_enable_acknack_enable = 0x%02X\n",
				  pl_sb->dpdch_enable_acknack_enable);
			SYSLOGSTR(vp_prio,
				  "cqi_enable_ul_modulation = 0x%02X\n",
				  pl_sb->cqi_enable_ul_modulation);
			SYSLOGSTR(vp_prio, "num_edpdch_edpdch_tti = 0x%02X\n",
				  pl_sb->num_edpdch_edpdch_tti);
			SYSLOGSTR(vp_prio, "beta_c_beta_d = 0x%02X\n",
				  pl_sb->beta_c_beta_d);
			SYSLOGSTR(vp_prio, "beta_hs = %u\n", pl_sb->beta_hs);
			SYSLOGSTR(vp_prio, "beta_ec = %u\n", pl_sb->beta_ec);
			SYSLOGSTR(vp_prio, "beta_ed1 = %u\n", pl_sb->beta_ed1);
			SYSLOGSTR(vp_prio, "beta_ed2 = %u\n", pl_sb->beta_ed2);
			SYSLOGSTR(vp_prio, "beta_ed3 = %u\n", pl_sb->beta_ed3);
			SYSLOGSTR(vp_prio, "beta_ed4 = %u\n", pl_sb->beta_ed4);
			SYSLOGSTR(vp_prio, "scrambling_code = %lu\n",
				  pl_sb->scrambling_code);
			SYSLOGSTR(vp_prio, "dpdch_sf = %u\n", pl_sb->dpdch_sf);
			SYSLOGSTR(vp_prio, "edpdch_sf = %u\n",
				  pl_sb->edpdch_sf);
			SYSLOGSTR(vp_prio, "hsdpa_delay_fill3 = 0x%02X\n",
				  pl_sb->hsdpa_delay_fill3);
			SYSLOGSTR(vp_prio, "fill4 = %u\n", pl_sb->fill4);
		}
		break;

	case C_HAL_SB_GSM_RXTX_CONTROL:
		{
			C_HAL_SB_GSM_RXTX_CONTROL_STR *pl_sb =
			    (C_HAL_SB_GSM_RXTX_CONTROL_STR *) pp_block;
			SYSLOGSTR(vp_prio, "C_HAL_SB_GSM_RXTX_CONTROL:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
			SYSLOGSTR(vp_prio, "gsm_band_info = %lu\n",
				  pl_sb->gsm_band_info);
			SYSLOGSTR(vp_prio, "rf_channel = %u\n",
				  pl_sb->rf_channel);
			SYSLOGSTR(vp_prio, "rx_slot_mask = 0x%02X\n",
				  HIGHBYTE(pl_sb->rx_slot_mask_tx_slot_mask));
			SYSLOGSTR(vp_prio, "tx_slot_mask = 0x%02X\n",
				  LOWBYTE(pl_sb->rx_slot_mask_tx_slot_mask));
			SYSLOGSTR(vp_prio, "tx_8psk_mask = 0x%02X\n",
				  HIGHBYTE(pl_sb->tx_8psk_mask_mon_rach_mask));
			SYSLOGSTR(vp_prio, "mon_rach_mask = 0x%02X\n",
				  LOWBYTE(pl_sb->tx_8psk_mask_mon_rach_mask));
			SYSLOGSTR(vp_prio, "data_type = %u\n",
				  HIGHBYTE(pl_sb->data_type_afc_type));
			SYSLOGSTR(vp_prio, "afc_type = %u\n",
				  LOWBYTE(pl_sb->data_type_afc_type));
			SYSLOGSTR(vp_prio, "manualAfc = %lu\n",
				  pl_sb->manualAfc);
			SYSLOGSTR(vp_prio, "tx_timing_advance = %u\n",
				  pl_sb->tx_timing_advance);
			SYSLOGSTR(vp_prio, "agc_mode = %u\n",
				  HIGHBYTE(pl_sb->agc_mode_tx_power_unit));
			SYSLOGSTR(vp_prio, "tx_power_unit = %u\n",
				  LOWBYTE(pl_sb->agc_mode_tx_power_unit));

			int vl_slot, vl_nb_slots =
			    sizeof(pl_sb->slot_level) /
			    sizeof(pl_sb->slot_level[0]);
			for (vl_slot = 0; vl_slot < vl_nb_slots; vl_slot++) {
				SYSLOGSTR(vp_prio, "slot %d level = %d\n",
					  vl_slot, pl_sb->slot_level[vl_slot]);
			}
		}
		break;

	case C_HAL_RF_SB_TUNING_VALUES_GET_REQ:
		{
			C_HAL_RF_SB_TUNING_VALUES_GET_REQ_STR *pl_sb =
			    (C_HAL_RF_SB_TUNING_VALUES_GET_REQ_STR *) pp_block;
			SYSLOGSTR(vp_prio,
				  "C_HAL_RF_SB_TUNING_VALUES_GET_REQ:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
			SYSLOGSTR(vp_prio, "tuning id = %u\n",
				  pl_sb->tuning_id);
		}
		break;

	case C_HAL_RF_SB_TUNING_VALUES_GET_RESP:
		{
			C_HAL_RF_SB_TUNING_VALUES_GET_RESP_STR *pl_sb =
			    (C_HAL_RF_SB_TUNING_VALUES_GET_RESP_STR *) pp_block;
			SYSLOGSTR(vp_prio,
				  "C_HAL_RF_SB_TUNING_VALUES_GET_RESP:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
			SYSLOGSTR(vp_prio,
				  "tuning id = %u\nstatus = 0x%X\nnbr of words = %u\n",
				  pl_sb->tuning_id,
				  LOWBYTE(pl_sb->fill1_status),
				  pl_sb->nbr_of_words);
			SYSLOGSTR(vp_prio, "data = ");
			PrintBinary(pl_sb->data,
				    pl_sb->nbr_of_words * sizeof(uint16));
			SYSLOGSTR(vp_prio, "\n");
		}
		break;

	case C_HAL_RF_SB_TUNING_VALUES_SET_REQ:
		{
			C_HAL_RF_SB_TUNING_VALUES_SET_REQ_STR *pl_sb =
			    (C_HAL_RF_SB_TUNING_VALUES_SET_REQ_STR *) pp_block;
			SYSLOGSTR(vp_prio,
				  "C_HAL_RF_SB_TUNING_VALUES_SET_REQ:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
			SYSLOGSTR(vp_prio,
				  "tuning id = %u\nnbr of words = %u\n",
				  pl_sb->tuning_id, pl_sb->nbr_of_words);
			SYSLOGSTR(vp_prio, "data = ");
			PrintBinary(pl_sb->data,
				    pl_sb->nbr_of_words * sizeof(uint16));
			SYSLOGSTR(vp_prio, "\n");
		}
		break;

	case C_HAL_RF_SB_TUNING_VALUES_SET_RESP:
		{
			C_HAL_RF_SB_TUNING_VALUES_SET_RESP_STR *pl_sb =
			    (C_HAL_RF_SB_TUNING_VALUES_SET_RESP_STR *) pp_block;
			SYSLOGSTR(vp_prio,
				  "C_HAL_RF_SB_TUNING_VALUES_SET_RESP:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
			SYSLOGSTR(vp_prio, "tuning id = %u\nstatus = 0x%X\n",
				  pl_sb->tuning_id,
				  LOWBYTE(pl_sb->fill1_status));
		}
		break;

	case C_TEST_SB_PWR_SEQ_MEASURE:
		{
			C_TEST_SB_PWR_SEQ_MEASURE_STR *pl_sb =
			    (C_TEST_SB_PWR_SEQ_MEASURE_STR *) pp_block;
			union tx_pwr_seq_data vl_pwr_seq_data;
			vl_pwr_seq_data.wcdma = pl_sb->system_dep_data;

			SYSLOGSTR(vp_prio, "C_TEST_SB_PWR_SEQ_MEASURE:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
			SYSLOGSTR(vp_prio, "system_info = %u\n",
				  pl_sb->system_info);
			SYSLOGSTR(vp_prio,
				  "rx_attenuation_mode_and_fill1 = 0x%02X\n",
				  pl_sb->rx_attenuation_mode_and_fill1);

			switch (pl_sb->system_info) {
			case C_TEST_GSM:
				{
					SYSLOGSTR(vp_prio, "++ GSM data ++\n");
					SYSLOGSTR(vp_prio,
						  "rf_band = 0x%04lX\n",
						  vl_pwr_seq_data.gsm.rf_band);
					SYSLOGSTR(vp_prio,
						  "pwr_level_nbr = %u\n",
						  vl_pwr_seq_data.gsm.
						  pwr_level_nbr);
					SYSLOGSTR(vp_prio, "-- GSM data --\n");
				}
				break;

			case C_TEST_WCDMA:
				{
					SYSLOGSTR(vp_prio,
						  "++ WCDMA data ++\n");
					SYSLOGSTR(vp_prio,
						  "rf_band = 0x%04lX\n",
						  vl_pwr_seq_data.wcdma.
						  rf_band);
					SYSLOGSTR(vp_prio,
						  "-- WCDMA data --\n");
				}
				break;

			default:
				SYSLOG(LOG_ERR, "unknown mode");
			}

			SYSLOGSTR(vp_prio, "channel_nbr = %u\n",
				  pl_sb->channel_nbr);
			SYSLOGSTR(vp_prio, "pwr_ctrl_value = %u\n",
				  pl_sb->pwr_ctrl_value);
			SYSLOGSTR(vp_prio, "rf_rx_attenuation_value = %u\n",
				  pl_sb->rf_rx_attenuation_value);
			SYSLOGSTR(vp_prio, "lna_state = 0x%X\n",
				  pl_sb->lna_state);
			SYSLOGSTR(vp_prio, "rf_lna_step_size = %u\n",
				  pl_sb->rf_lna_step_size);
			SYSLOGSTR(vp_prio, "nbr_empty_slots = %u\n",
				  pl_sb->nbr_empty_slots);
		}
		break;

	case C_TEST_SB_PWR_SEQ_MEAS_RESULTS:
		{
			C_TEST_SB_PWR_SEQ_MEAS_RESULTS_STR *pl_sb =
			    (C_TEST_SB_PWR_SEQ_MEAS_RESULTS_STR *) pp_block;
			SYSLOGSTR(vp_prio, "C_TEST_SB_PWR_SEQ_MEAS_RESULTS:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
			SYSLOGSTR(vp_prio, "rx_attenuation_mode = 0x%X\n",
				  HIGHBYTE
				  (pl_sb->rx_attenuation_mode_and_status));
			SYSLOGSTR(vp_prio, "status = 0x%X\n",
				  LOWBYTE
				  (pl_sb->rx_attenuation_mode_and_status));
			SYSLOGSTR(vp_prio, "system_info = %u\n",
				  pl_sb->system_info);
			SYSLOGSTR(vp_prio, "rf_band = 0x%lX\n", pl_sb->rf_band);
			SYSLOGSTR(vp_prio, "channel_nbr = %u\n",
				  pl_sb->channel_nbr);
			SYSLOGSTR(vp_prio, "int_measured_pwr1 (Q6) = %d\n",
				  pl_sb->int_measured_pwr1);
			SYSLOGSTR(vp_prio, "int_measured_pwr2 (Q6) = %d\n",
				  pl_sb->int_measured_pwr2);
			SYSLOGSTR(vp_prio, "lna_state = %u\n",
				  pl_sb->lna_state);
		}
		break;

	case C_TEST_SB_FREQ_RESP_CALIBRATE:
		{
			C_TEST_SB_FREQ_RESP_CALIBRATE_STR *pl_sb =
			    (C_TEST_SB_FREQ_RESP_CALIBRATE_STR *) pp_block;
			SYSLOGSTR(vp_prio, "C_TEST_SB_FREQ_RESP_CALIBRATE:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
			SYSLOGSTR(vp_prio, "system_info = 0x%X\n",
				  pl_sb->system_info);
			SYSLOGSTR(vp_prio, "fill1 = %u\n", pl_sb->fill1);
			SYSLOGSTR(vp_prio, "rf_band = 0x%lX\n", pl_sb->rf_band);
			SYSLOGSTR(vp_prio, "fill2 = %u\n", pl_sb->fill2);
			SYSLOGSTR(vp_prio, "nbr_of_data = %u\n",
				  pl_sb->nbr_of_data);

			int i;
			for (i = 0; i < pl_sb->nbr_of_data; i++) {
				C_TEST_FREQ_RESP_DATA_STR *data =
				    pl_sb->freq_resp_data + i;
				SYSLOGSTR(vp_prio, "freq_resp_data %d:\n\n", i);
				SYSLOGSTR(vp_prio, "channel_nbr = %u\n",
					  data->channel_nbr);
				SYSLOGSTR(vp_prio, "ext_data = %d\n",
					  data->ext_data);
				SYSLOGSTR(vp_prio, "int_data = %d\n",
					  data->int_data);
				SYSLOGSTR(vp_prio, "fill1 = %u\n\n",
					  data->fill1);
			}
		}
		break;

	case C_TEST_SB_LINKO2_FREQ_RESP_CAL_RESULTS:
		{
			C_TEST_SB_LINKO2_FREQ_RESP_CAL_RESULTS_STR *pl_sb =
			    (C_TEST_SB_LINKO2_FREQ_RESP_CAL_RESULTS_STR *)
			    pp_block;
			SYSLOGSTR(vp_prio,
				  "C_TEST_SB_LINKO2_FREQ_RESP_CAL_RESULTS:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
			SYSLOGSTR(vp_prio, "status = 0x%X\n", pl_sb->status);
			SYSLOGSTR(vp_prio, "system_info = 0x%X\n",
				  pl_sb->system_info);
			SYSLOGSTR(vp_prio, "rf_band = 0x%lX\n", pl_sb->rf_band);
			SYSLOGSTR(vp_prio, "slope(Q3) = %d\n", pl_sb->slope);
			SYSLOGSTR(vp_prio, "coeff_b(Q0) = %d\n",
				  pl_sb->coeff_b);
			SYSLOGSTR(vp_prio, "coeff_c(Q6) = %d\n",
				  pl_sb->coeff_c);

			int i;
			for (i = 0; i < pl_sb->nbr_of_data; i++) {
				C_TEST_FREQ_RESP_DATA_STR *data =
				    pl_sb->freq_resp_data + i;
				SYSLOGSTR(vp_prio, "freq_resp_data %d:\n", i);
				SYSLOGSTR(vp_prio, "channel_nbr = %u\n",
					  data->channel_nbr);
				SYSLOGSTR(vp_prio, "ext_data = %d\n",
					  data->ext_data);
				SYSLOGSTR(vp_prio, "int_data = %d\n",
					  data->int_data);
				SYSLOGSTR(vp_prio, "fill1 = %u\n", data->fill1);
			}
		}
		break;

	case C_TEST_SB_GSM_TX_PWR_TUNING:
		{
			C_TEST_SB_GSM_TX_PWR_TUNING_STR *pl_sb =
			    (C_TEST_SB_GSM_TX_PWR_TUNING_STR *) pp_block;
			SYSLOGSTR(vp_prio, "C_TEST_SB_GSM_TX_PWR_TUNING:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
			SYSLOGSTR(vp_prio,
				  "RF Band: 0x%lX\nTx channel number: 0x%X\nFiller: 0x%X\nEdge mode: 0x%X\nAmount of tuning ctrl data: 0x%X\n",
				  pl_sb->rf_band, pl_sb->channel_nbr,
				  pl_sb->fill1, pl_sb->edge_on,
				  pl_sb->nbr_of_ctrl_data);

			int i;
			for (i = 0; i < pl_sb->nbr_of_ctrl_data; i++) {
				GSM_TX_PWR_SELFTUNING_CTRL_DATA_STR *pl_data =
				    (GSM_TX_PWR_SELFTUNING_CTRL_DATA_STR *)
				    pl_sb->selftun_ctrl + i;
				SYSLOGSTR(vp_prio, "TUNING CTRL DATA %i ++ \n",
					  i);
				SYSLOGSTR(vp_prio,
					  "GSM pwr level number: 0x%X\n"
					  "Target power: 0x%X\n"
					  "Filler: 0x%X\n"
					  "PA ctrl data: 0x%X\n"
					  "PA Voltage ctrl value:0x%X\n"
					  "PA bias ctrl value:0x%X\n",
					  pl_data->gsm_pwr_level,
					  pl_data->target_dbm, pl_data->fill1,
					  pl_data->pa_data_valid,
					  pl_data->pa_ctrl_data.pa_voltage_ctrl,
					  pl_data->pa_ctrl_data.pa_ictrl);
				SYSLOGSTR(vp_prio,
					  "-- TUNING CTRL DATA %i --\n", i);
			}
		}
		break;

	case C_TEST_SB_WCDMA_TX_TUNING:
		{
			C_TEST_SB_WCDMA_TX_TUNING_STR *pl_sb =
			    (C_TEST_SB_WCDMA_TX_TUNING_STR *) pp_block;
			SYSLOGSTR(vp_prio, "C_TEST_SB_WCDMA_TX_TUNING:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);

			SYSLOGSTR(vp_prio, "Step duration: 0x%X\n"
				  "Tuning Control options: 0x%X\n"
				  "RF Band: 0x%lX\n"
				  "Tx channel number: 0x%X\n"
				  "Target power start: 0x%X\n"
				  "Power metter attenuation: 0x%X\n"
				  "Power control start: 0x%X\n"
				  "Power control step: 0x%X\n"
				  "DPDCH channel weight: 0x%X\n"
				  "DPCCH channel weight: 0x%X\n"
				  "Amount of tuning ctrl data: 0x%X\n",
				  pl_sb->step_length,
				  pl_sb->tuning_ctrl_opt,
				  pl_sb->rf_band,
				  pl_sb->channel_nbr,
				  pl_sb->tuning_target,
				  pl_sb->pwr_meter_att,
				  pl_sb->pwr_ctrl_value,
				  pl_sb->pwr_ctrl_step,
				  pl_sb->dpdch_weight,
				  pl_sb->dpcch_weight,
				  pl_sb->pa_ctrl_data_size);

			int i;
			for (i = 0; i < pl_sb->pa_ctrl_data_size; i++) {
				WCDMA_TX_PWR_CTRL_DATA_STR *pl_data =
				    (WCDMA_TX_PWR_CTRL_DATA_STR *)
				    pl_sb->pa_ctrl + i;
				SYSLOGSTR(vp_prio,
					  "++ TUNING CTRL DATA %i ++ \n", i);
				SYSLOGSTR(vp_prio,
					  "PA Voltage ctrl value:0x%X\n"
					  "PA bias ctrl value:0x%X\n",
					  pl_data->pa_voltage_ctrl,
					  pl_data->pa_ictrl);
				SYSLOGSTR(vp_prio,
					  "-- TUNING CTRL DATA %i --\n", i);
				pl_data++;
			}
		}
		break;

	case C_TEST_SB_TX_TUNING_RESULTS:
		{
			C_TEST_SB_TX_TUNING_RESULTS_STR *pl_sb =
			    (C_TEST_SB_TX_TUNING_RESULTS_STR *) pp_block;
			SYSLOGSTR(vp_prio, "C_TEST_SB_TX_TUNING_RESULTS:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);

			SYSLOGSTR(vp_prio,
				  "Filler_Status: 0x%X\n"
				  "System info: 0x%X\n"
				  "System dep data: 0x%lX\n"
				  "Measured max Power: 0x%X\n"
				  "Amount of returned values: 0x%X\n",
				  pl_sb->status,
				  pl_sb->system_info,
				  pl_sb->system_dep_data,
				  pl_sb->max_pwr, pl_sb->amount_of_values);

			int i;
			for (i = 0; i < pl_sb->amount_of_values; i++) {
				TX_PWR_TUNING_DATA_STR *pl_data =
				    (TX_PWR_TUNING_DATA_STR *)
				    pl_sb->measured_pwr + i;
				SYSLOGSTR(vp_prio,
					  "TUNING RETURNED VALUE %i ++ \n", i);
				SYSLOGSTR(vp_prio,
					  "Measured power: 0x%X\n"
					  "Power control DAC value: 0x%X\n",
					  pl_data->measured_pwr,
					  pl_data->pwr_ctrl);
				SYSLOGSTR(vp_prio,
					  "-- TUNING RETURNED VALUE %i -- \n",
					  i);
				pl_data++;
			}
		}
		break;

	case C_TEST_SB_GSM_TX_IQ_SETUP:
		{
			C_TEST_SB_GSM_TX_IQ_SETUP_STR *pl_sb =
			    (C_TEST_SB_GSM_TX_IQ_SETUP_STR *) pp_block;
			SYSLOGSTR(vp_prio, "C_TEST_SB_GSM_TX_IQ_SETUP:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
			SYSLOG(vp_prio, "Band: 0x%X\n"
			       "Power control loop mode: 0x%X\n"
			       "Tx power level: 0x%X\n"
			       "Tx channel number: 0x%X\n"
			       "I DC offset value: 0x%X\n"
			       "Q DC offset value: 0x%X\n"
			       "Amplitude offset value: 0x%X\n"
			       "Filler: 0x%X\n",
			       pl_sb->band,
			       pl_sb->pa_loop_mode,
			       pl_sb->power_level,
			       pl_sb->channel_nbr,
			       pl_sb->dc_offset_i,
			       pl_sb->dc_offset_q,
			       pl_sb->ampl_offset, pl_sb->fill1);
		}
		break;

	case C_TEST_SB_WCDMA_TX_IQ_SETUP:
		{
			C_TEST_SB_WCDMA_TX_IQ_SETUP_STR *pl_sb =
			    (C_TEST_SB_WCDMA_TX_IQ_SETUP_STR *) pp_block;
			SYSLOGSTR(vp_prio, "C_TEST_SB_WCDMA_TX_IQ_SETUP_STR:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
			SYSLOG(vp_prio, "Band: 0x%X\n"
			        "Tx channel number: 0x%X\n"
			        "Filler: 0x%X\n",
			        pl_sb->band_info,
			        pl_sb->ul_uarfcn, pl_sb->fill1);
		}
		break;

	case C_TEST_SB_DC_OFFSET_RESULT:
		{
			C_TEST_SB_DC_OFFSET_RESULT_STR *pl_sb =
			    (C_TEST_SB_DC_OFFSET_RESULT_STR *) pp_block;
			SYSLOGSTR(vp_prio, "C_TEST_SB_DC_OFFSET_RESULT:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);

			SYSLOGSTR(vp_prio, "Status: 0x%X\n"
				  "I branch value: 0x%X\n"
				  "I branch pwr difference: 0x%X\n"
				  "Q branch value: 0x%X\n"
				  "Q branch pwr difference: 0x%X\n"
				  "Filler: 0x%X\n\n",
				  pl_sb->status,
				  pl_sb->i_tuning_value,
				  pl_sb->i_result,
				  pl_sb->q_tuning_value,
				  pl_sb->q_result, pl_sb->fill1);
		}
		break;

	case C_TEST_SB_AMPLITUDE_RESULT:
		{
			C_TEST_SB_AMPLITUDE_RESULT_STR *pl_sb =
			    (C_TEST_SB_AMPLITUDE_RESULT_STR *) pp_block;
			SYSLOGSTR(vp_prio, "C_TEST_SB_AMPLITUDE_RESULT:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);

			SYSLOGSTR(vp_prio, "Status: 0x%X\n"
				  "Ampl tuning value: 0x%X\n"
				  "Ampl best tuning value: 0x%X\n"
				  "Ampl tuning vector length: 0x%X\n",
				  pl_sb->status,
				  pl_sb->best_tuning_value,
				  pl_sb->best_result, pl_sb->vector_length);
		}
		break;

	case C_TEST_SB_PHASE_RESULT:
		{
			C_TEST_SB_PHASE_RESULT_STR *pl_sb =
			    (C_TEST_SB_PHASE_RESULT_STR *) pp_block;
			SYSLOGSTR(vp_prio, "C_TEST_SB_PHASE_RESULT:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);

			SYSLOGSTR(vp_prio, "Status: 0x%X\n"
				  "Phase tuning value: 0x%X\n"
				  "Phase best tuning value: 0x%X\n"
				  "Phase tuning vector length: 0x%X\n",
				  pl_sb->status,
				  pl_sb->best_tuning_value,
				  pl_sb->best_result, pl_sb->vector_length);
		}
		break;

	case C_TEST_SB_GSM_PA_COMPRESSION_TUNING:
		{
			C_TEST_SB_GSM_PA_COMPRESSION_TUNING_STR *pl_sb =
			    (C_TEST_SB_GSM_PA_COMPRESSION_TUNING_STR *)
			    pp_block;
			SYSLOGSTR(vp_prio,
				  "C_TEST_SB_GSM_PA_COMPRESSION_TUNING:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);

			SYSLOGSTR(vp_prio, "RF band: 0x%lX\n"
				  "Tx channel number: 0x%X\n"
				  "Control word: 0x%X\n"
				  "Tx target power: 0x%X\n"
				  "PA compression level: 0x%X\n",
				  pl_sb->rf_band,
				  pl_sb->channel_nbr,
				  pl_sb->control_word,
				  pl_sb->target_power_dBm_q6,
				  pl_sb->compression_level_dB_q6);
		}
		break;

	case C_TEST_SB_GSM_PA_COMPRESSION_TUNING_RESULTS:
		{
			C_TEST_SB_GSM_PA_COMPRESSION_TUNING_RESULTS_STR *pl_sb =
			    (C_TEST_SB_GSM_PA_COMPRESSION_TUNING_RESULTS_STR *)
			    pp_block;
			SYSLOGSTR(vp_prio,
				  "C_TEST_SB_GSM_PA_COMPRESSION_TUNING_RESULTS:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);

			SYSLOGSTR(vp_prio, "Status: 0x%X\n"
				  "Filler1: 0x%X\n"
				  "RF band: 0x%lX\n"
				  "PA Compensation: 0x%X\n"
				  "Final Tx power: 0x%X\n"
				  "Iteration count: 0x%X\n"
				  "Filler2: 0x%X\n",
				  pl_sb->status,
				  pl_sb->fill1,
				  pl_sb->rf_band,
				  pl_sb->pa_compensation_dB_q6,
				  pl_sb->final_tx_power_dBm_q6,
				  pl_sb->iteration_count, pl_sb->fill2);
		}
		break;

	case C_TEST_SB_TUNING_ERROR:
		{
			C_TEST_SB_TUNING_ERROR_STR *pl_sb =
			    (C_TEST_SB_TUNING_ERROR_STR *) pp_block;
			SYSLOGSTR(vp_prio, "C_TEST_SB_TUNING_ERROR:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
			SYSLOGSTR(vp_prio, "errorCode = %u\n",
				  pl_sb->errorCode);
			/* xxxxxxxx--------  Id of failed operation
			   --------xxxxxxxx  First char of message
			 */
			SYSLOGSTR(vp_prio, "status = %X\n",
				  *((u8 *) (&pl_sb->errorCode +
					    sizeof(uint16))));
			SYSLOGSTR(vp_prio, "message = %s\n",
				  (char *)&pl_sb->errorCode + sizeof(uint16) +
				  1);
			/* 0 to 3 bytes: filler */
		}
		break;

	case C_TEST_SB_ANTENNA_TEST:
		{
			C_TEST_SB_ANTENNA_TEST_STR *pl_sb =
			    (C_TEST_SB_ANTENNA_TEST_STR *) pp_block;
			SYSLOGSTR(vp_prio, "C_TEST_SB_ANTENNA_TEST:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
			SYSLOGSTR(vp_prio, "Sub Block Sys Info: 0x%X\n"
				  "Sub Block Band: 0x%ld\n"
				  "Sub Block Channel: 0x%X\n"
				  "Sub Block Power Level: %X\n"
				  "Sub Block Attenuation value: 0x%X\n",
				  pl_sb->system_info,
				  pl_sb->system_dep_data,
				  pl_sb->channel_nbr,
				  pl_sb->pwr_level, pl_sb->att_value);
		}
		break;

	case C_TEST_SB_ANTENNA_TEST_RESULT:
		{
			C_TEST_SB_ANTENNA_TEST_RESULT_STR *pl_sb =
			    (C_TEST_SB_ANTENNA_TEST_RESULT_STR *) pp_block;
			SYSLOGSTR(vp_prio, "C_TEST_SB_ANTENNA_TEST_RESULT:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
			SYSLOGSTR(vp_prio, "Status = 0x%X\n"
				  "VSWR value = %u\n",
				  LOWBYTE(pl_sb->fill1_substatus),
				  pl_sb->vswr_value);
		}
		break;

	case C_TEST_SB_READ_RX_IQ_SAMPLES:
		{
			C_TEST_SB_READ_RX_IQ_SAMPLES_STR *pl_sb =
			    (C_TEST_SB_READ_RX_IQ_SAMPLES_STR *) pp_block;
			SYSLOGSTR(vp_prio, "C_TEST_SB_READ_RX_IQ_SAMPLES:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
			SYSLOGSTR(vp_prio, "System info = 0x%X\nRx path = %u\n",
				  pl_sb->system_info,
				  LOWBYTE(pl_sb->is_new_samples_rx_path));
		}
		break;

	case C_TEST_SB_RECEIVED_RX_IQ_SAMPLES:
		{
			C_TEST_SB_RECEIVED_RX_IQ_SAMPLES_STR *pl_sb =
			    (C_TEST_SB_RECEIVED_RX_IQ_SAMPLES_STR *) pp_block;
			SYSLOGSTR(vp_prio,
				  "C_TEST_SB_RECEIVED_RX_IQ_SAMPLES:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);

			SYSLOGSTR(vp_prio,
				  "Status = 0x%X\nSystem info = 0x%X\nRx path = 0x%X\nSample freq. = %u\nRx gain = %u\nNbr of data = %u\n",
				  LOWBYTE(pl_sb->status), pl_sb->system_info,
				  LOWBYTE(pl_sb->fill2_rx_path),
				  pl_sb->sample_freq, pl_sb->rx_gain,
				  pl_sb->nbr_of_data);

			C_TEST_IQ_PAIR_DATA_STR *pl_data;
			int i;

			for (i = 0; i < pl_sb->nbr_of_data; i++) {
				pl_data = pl_sb->rx_iq_data + i;
				SYSLOGSTR(vp_prio, "Data %d: i=%d, q=%d\n", i,
					  pl_data->i_data, pl_data->q_data);
			}
		}
		break;

	case C_TEST_SB_RESERVED_FOR_LICENSEE_REQ1:
		{
			C_TEST_SB_RESERVED_FOR_LICENSEE_REQ1_STR *pl_sb =
			    (C_TEST_SB_RESERVED_FOR_LICENSEE_REQ1_STR *)
			    pp_block;

			GET_INFO_CTRL_DATA_STR *pl_get_info =
			    (GET_INFO_CTRL_DATA_STR *) ((u8 *) pl_sb +
							SIZE_C_TEST_SB_RESERVED_FOR_LICENSEE_REQ1_STR);

			SYSLOGSTR(vp_prio,
				  "C_TEST_SB_RESERVED_FOR_LICENSEE_REQ1_STR:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
			SYSLOGSTR(vp_prio, "Requested action = 0x%X\n"
				  "index = %u\n",
				  pl_get_info->requested_action,
				  LOWBYTE(pl_get_info->fill_index));
		}
		break;

	case C_TEST_SB_RESERVED_FOR_LICENSEE_RESP1:
		{
			C_TEST_SB_RESERVED_FOR_LICENSEE_RESP1_STR *pl_sb =
			    (C_TEST_SB_RESERVED_FOR_LICENSEE_RESP1_STR *)
			    pp_block;

			RETURN_INFO_PARAM_RESP_STR *pl_ret_info =
			    (RETURN_INFO_PARAM_RESP_STR *) ((u8 *) pl_sb +
							    SIZE_C_TEST_SB_RESERVED_FOR_LICENSEE_RESP1_STR);
			SYSLOGSTR(vp_prio,
				  "C_TEST_SB_RESERVED_FOR_LICENSEE_RESP1_STR:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
			SYSLOGSTR(vp_prio, "Requested action = 0x%X\n"
				  "Status = %u\n"
				  "Nb of param = %u\n"
				  "Index = %u\n",
				  pl_ret_info->requested_action,
				  pl_ret_info->action_status,
				  pl_ret_info->nb_of_param, pl_ret_info->index);

			char vl_strParam[68];
			strncpy(vl_strParam,
				(char *)pl_ret_info->return_info_param.string,
				68);
			vl_strParam[67] = 0;
			SYSLOGSTR(vp_prio, "Param name = %s\n", vl_strParam);
		}
		break;

	case C_TEST_SB_GSM_SINGLE_BER:
		{
			C_TEST_SB_GSM_SINGLE_BER_STR *pl_sb =
			    (C_TEST_SB_GSM_SINGLE_BER_STR *) pp_block;
			SYSLOGSTR(vp_prio, "C_TEST_SB_GSM_SINGLE_BER:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);

			SYSLOGSTR(vp_prio, "rx_slot_tx_slot = 0x%04X\n",
				  pl_sb->rx_slot_pattern_tx_slot_pattern);
			SYSLOGSTR(vp_prio, "8psk_slots_channel_type = 0x%04X\n",
				  pl_sb->psk_slots_channel_type);
			SYSLOGSTR(vp_prio, "codec = %u\n",
				  LOWBYTE(pl_sb->fill2_codec));
			SYSLOGSTR(vp_prio, "arfcn = %u\n", pl_sb->ARFCN_val);
			SYSLOGSTR(vp_prio, "coded mode = %u\n",
				  pl_sb->coded_mode);
			SYSLOGSTR(vp_prio, "nbr tested bits = %u\n",
				  pl_sb->num_of_tested_bits);
			SYSLOGSTR(vp_prio, "data type = %u\n",
				  LOWBYTE(pl_sb->fill3_transmit_data_type));

			int vl_nb_data = pl_sb->num_of_freq;
			int i, j;
			GSM_SBER_INFO_PER_FREQ_STR *pl_data;
			for (i = 0; i < vl_nb_data; i++) {
				pl_data =
				    (GSM_SBER_INFO_PER_FREQ_STR *)
				    pl_sb->array_of_freq_info;
				pl_data += i;

				SYSLOGSTR(vp_prio,
					  "\nGSM_SBER_INFO_PER_FREQ[%d]\n", i);
				SYSLOGSTR(vp_prio, "levels :\n");
				for (j = 0; j < 8; j++) {
					SYSLOGSTR(vp_prio, "%d: Tx=%u PCL", j,
						  pl_data->
						  tx_level_pcl[j].tx_level);
					SYSLOGSTR(vp_prio, ", Rx=%d dBm (Q8)\n",
						  pl_data->
						  init_rx_level
						  [j].rx_pwr_level);
				}
				SYSLOGSTR(vp_prio, "\n");
				SYSLOGSTR(vp_prio, "arfcn = %u\n",
					  pl_data->arfcn);
				SYSLOGSTR(vp_prio, "rf band = %u\n",
					  pl_data->rf_band);
			}
		}
		break;

	case C_TEST_SB_GSM_SINGLE_BER_SYNC:
		{
			C_TEST_SB_GSM_SINGLE_BER_SYNC_STR *pl_sb =
			    (C_TEST_SB_GSM_SINGLE_BER_SYNC_STR *) pp_block;
			SYSLOGSTR(vp_prio, "C_TEST_SB_GSM_SINGLE_BER_SYNC:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
			SYSLOGSTR(vp_prio, "rf band = 0x%4X\n", pl_sb->rf_band);
			SYSLOGSTR(vp_prio, "tester = %u\n", pl_sb->tester_tool);
			SYSLOGSTR(vp_prio, "arfcn_bcch = %u\n",
				  pl_sb->arfcn_bcch);
			SYSLOGSTR(vp_prio, "data type = %u\n",
				  LOWBYTE(pl_sb->fill1_data_type));
			SYSLOGSTR(vp_prio, "arfcn_tch = %u\n",
				  pl_sb->arfcn_tch);
			SYSLOGSTR(vp_prio, "FN sync = %u\n",
				  pl_sb->FN_end_of_sync);
			SYSLOGSTR(vp_prio, "channel type = %u\n",
				  HIGHBYTE(pl_sb->chan_type_codec));
			SYSLOGSTR(vp_prio, "codec = %u\n",
				  LOWBYTE(pl_sb->chan_type_codec));
			SYSLOGSTR(vp_prio, "slot pattern = 0x%02X\n",
				  LOWBYTE(pl_sb->fill2_time_slot_pat));
			SYSLOGSTR(vp_prio, "coded mode = %u\n",
				  pl_sb->coded_mode);
			SYSLOGSTR(vp_prio, "init_rx_level = %d (Q8)\n",
				  pl_sb->init_rx_level);
		}
		break;

	case C_TEST_SB_GSM_SINGLE_BER_RESULTS:
		{
			C_TEST_SB_GSM_SINGLE_BER_RESULTS_STR *pl_sb =
			    (C_TEST_SB_GSM_SINGLE_BER_RESULTS_STR *) pp_block;
			SYSLOGSTR(vp_prio,
				  "C_TEST_SB_GSM_SINGLE_BER_RESULTS:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
			SYSLOGSTR(vp_prio, "status = 0x%X\n",
				  LOWBYTE(pl_sb->fill1_status));

			int i;
			GSM_BER_RES_INFO_STR *pl_data;
			for (i = 0; i < pl_sb->num_of_res; i++) {
				pl_data =
				    (GSM_BER_RES_INFO_STR *)
				    pl_sb->array_of_results;
				pl_data += i;
				SYSLOGSTR(vp_prio, "\nGSM_BER_RES_INFO[%d]\n",
					  i);

				SYSLOGSTR(vp_prio, "Ib RBER bits = %u\n",
					  pl_data->num_of_rber_Ib_coded_bits);
				SYSLOGSTR(vp_prio, "II RBER bits = %u\n",
					  pl_data->num_of_rber_II_coded_bits);
				SYSLOGSTR(vp_prio, "BER bits = %u\n",
					  pl_data->num_of_ber_uncoded_bits);
				SYSLOGSTR(vp_prio, "Ib RBER error = %u\n",
					  pl_data->rber_Ib_coded_error_bits);
				SYSLOGSTR(vp_prio, "II RBER error = %u\n",
					  pl_data->rber_II_coded_error_bits);
				SYSLOGSTR(vp_prio, "BER error = %u\n",
					  pl_data->ber_uncoded_error_bits);
				SYSLOGSTR(vp_prio, "nb blocks = %u\n",
					  pl_data->meas_blocks);
				SYSLOGSTR(vp_prio, "nb blocks error = %u\n",
					  pl_data->false_blocks);
				SYSLOGSTR(vp_prio, "Ave. SNR(Q) = %d\n",
					  pl_data->avg_snr);
				SYSLOGSTR(vp_prio, "Ave. pwr(Q) = %d\n",
					  pl_data->avg_pwr);
			}
		}
		break;

	case C_TEST_SB_WCDMA_SINGLE_BER:
		{
			C_TEST_SB_WCDMA_SINGLE_BER_STR *pl_sb =
			    (C_TEST_SB_WCDMA_SINGLE_BER_STR *) pp_block;
			SYSLOGSTR(vp_prio, "C_TEST_SB_WCDMA_SINGLE_BER:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
			SYSLOGSTR(vp_prio, "no_bits_to_be_tested = %u\n",
				  pl_sb->no_bits_to_be_tested);
			SYSLOGSTR(vp_prio, "num of freq = %u\n",
				  pl_sb->num_of_freq);

			int i;
			WCDMA_SBER_INFO_PER_FREQ_STR *pl_data;
			for (i = 0; i < pl_sb->num_of_freq; i++) {
				pl_data =
				    (WCDMA_SBER_INFO_PER_FREQ_STR *)
				    pl_sb->array_of_freq_info;
				pl_data += i;
				SYSLOGSTR(vp_prio,
					  "\nWCDMA_SBER_INFO_PER_FREQ[%d]\n",
					  i);

				SYSLOGSTR(vp_prio, "tx level = %d\n",
					  pl_data->tx_level);
				SYSLOGSTR(vp_prio, "DL uarfcn = %u\n",
					  pl_data->dl_uarfcn);
				SYSLOGSTR(vp_prio, "band = %u\n",
					  pl_data->rf_band);
			}
		}
		break;

	case C_TEST_SB_WCDMA_SINGLE_BER_SYNC:
		{
			C_TEST_SB_WCDMA_SINGLE_BER_SYNC_STR *pl_sb =
			    (C_TEST_SB_WCDMA_SINGLE_BER_SYNC_STR *) pp_block;
			SYSLOGSTR(vp_prio,
				  "C_TEST_SB_WCDMA_SINGLE_BER_SYNC:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
			SYSLOGSTR(vp_prio, "band = %u\n", pl_sb->rf_band);
			SYSLOGSTR(vp_prio, "DL uarfcn = %u\n",
				  pl_sb->dl_uarfcn);
			SYSLOGSTR(vp_prio, "tester = %u\n", pl_sb->tester_tool);
			SYSLOGSTR(vp_prio, "channelization code = %u\n",
				  LOWBYTE(pl_sb->fill1_2_channelization_code));
			SYSLOGSTR(vp_prio, "UL scramble code = %u\n",
				  pl_sb->ul_scrambling_code);
			SYSLOGSTR(vp_prio, "pri scramble code = %u\n",
				  pl_sb->pri_scrambling_code);
			SYSLOGSTR(vp_prio, "frame number = %u\n",
				  pl_sb->frame_no);
			SYSLOGSTR(vp_prio, "RX data type = %u\n",
				  LOWBYTE(pl_sb->fill2_rx_data_type));
			SYSLOGSTR(vp_prio, "TX level = %d\n",
				  pl_sb->tx_pwr_level);
		}
		break;

	case C_TEST_SB_WCDMA_SINGLE_BER_RESULTS:
		{
			C_TEST_SB_WCDMA_SINGLE_BER_RESULTS_STR *pl_sb =
			    (C_TEST_SB_WCDMA_SINGLE_BER_RESULTS_STR *) pp_block;
			SYSLOGSTR(vp_prio,
				  "C_TEST_SB_WCDMA_SINGLE_BER_RESULTS:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
			SYSLOGSTR(vp_prio, "status = 0x%X\n",
				  LOWBYTE(pl_sb->fill1_status));
			SYSLOGSTR(vp_prio, "nb results = %u\n",
				  pl_sb->num_of_result);

			int i;
			WCDMA_SBER_RES_INFO_STR *pl_data;
			for (i = 0; i < pl_sb->num_of_result; i++) {
				pl_data =
				    (WCDMA_SBER_RES_INFO_STR *)
				    pl_sb->array_of_results;
				pl_data += i;
				SYSLOGSTR(vp_prio,
					  "\nWCDMA_SBER_RES_INFO[%d]\n", i);

				SYSLOGSTR(vp_prio, "total bits = %u\n",
					  pl_data->num_bits_meas);
				SYSLOGSTR(vp_prio, "error bits = %u\n",
					  pl_data->num_err_bits);
				SYSLOGSTR(vp_prio, "Ave. pwr(Q) = %d\n",
					  pl_data->avg_rcvd_pwr);
				SYSLOGSTR(vp_prio, "Rscp(Q) = %d\n",
					  pl_data->avg_rscp_res);
			}
		}
		break;

	case C_HAL_SB_WCDMA_TX_SWEEP_TEST_CONTROL:
		{
			C_HAL_SB_WCDMA_TX_SWEEP_TEST_CONTROL_STR *pl_sb =
			    (C_HAL_SB_WCDMA_TX_SWEEP_TEST_CONTROL_STR *) pp_block;
			SYSLOGSTR(vp_prio,
				  "C_HAL_SB_WCDMA_TX_SWEEP_TEST_CONTROL:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb->sb_len);
			SYSLOGSTR(vp_prio, "number_of_channels = %u\n",
				  pl_sb->number_of_channels);
			SYSLOGSTR(vp_prio, "duration = %u\n",
				  pl_sb->duration);

			int i,j;
			WCDMA_TX_SWEEP_TEST_CHANNEL_STR *pl_data1;
			int16 *pl_data2;
			for (i = 0; i < pl_sb->number_of_channels; i++) {
				pl_data1 = (WCDMA_TX_SWEEP_TEST_CHANNEL_STR *)pl_sb->channel_subblock;
				pl_data1 += i;
				SYSLOGSTR(vp_prio,
					  "\nWCDMA_TX_SWEEP_TEST_CHANNEL_STR[%d]\n", i);
				SYSLOGSTR(vp_prio, "band_info = %u\n",
					  pl_data1->band_info);
				SYSLOGSTR(vp_prio, "ul_uarfcn = %u\n",
					  pl_data1->ul_uarfcn);
				SYSLOGSTR(vp_prio, "number_of_steps = %u\n",
					  pl_data1->number_of_steps);

				for (j = 0; j < pl_data1->number_of_steps; j++) {
					pl_data2 = (int16 *)pl_data1->power_level;
					pl_data2 += j;
					SYSLOGSTR(vp_prio,
					  "power_level[%d] = %d\n", j, *pl_data2);
				}
			}
		}
		break;

	case C_TEST_SB_RESERVED_FOR_LICENSEE_REQ4:
		{
			C_TEST_SB_RESERVED_FOR_LICENSEE_REQ4_STR *pl_sb_hdr =
			    (C_TEST_SB_RESERVED_FOR_LICENSEE_REQ4_STR *) pp_block;

			C_TEST_SB_RESERVED_FOR_LICENSEE_REQ4_SEQ *pl_sb =
			    (C_TEST_SB_RESERVED_FOR_LICENSEE_REQ4_SEQ *) ((u8 *) pl_sb_hdr +
				SIZE_C_TEST_SB_RESERVED_FOR_LICENSEE_REQ4_STR);

			SYSLOGSTR(vp_prio,
				  "C_TEST_SB_RESERVED_FOR_LICENSEE_REQ4_STR:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb_hdr->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb_hdr->sb_len);

			SYSLOGSTR(vp_prio,
				  "C_TEST_SB_RESERVED_FOR_LICENSEE_RESP4_SEQ:\n");
			SYSLOGSTR(vp_prio, "band = 0x%X\n", pl_sb->band);
			SYSLOGSTR(vp_prio, "channel = %u\n", pl_sb->channel);
			SYSLOGSTR(vp_prio, "control = %u\n", pl_sb->control);
			SYSLOGSTR(vp_prio, "power = %d\n", pl_sb->power);

			SYSLOGSTR(vp_prio, "filler x 20 = %u\n", pl_sb->filler01);
 		}
 		break;

	case C_TEST_SB_RESERVED_FOR_LICENSEE_RESP4:
		{			
			C_TEST_SB_RESERVED_FOR_LICENSEE_RESP4_STR *pl_sb_hdr =
			    (C_TEST_SB_RESERVED_FOR_LICENSEE_RESP4_STR *) pp_block;

			C_TEST_SB_RESERVED_FOR_LICENSEE_RESP4_SEQ *pl_sb =
			    (C_TEST_SB_RESERVED_FOR_LICENSEE_RESP4_SEQ *) ((u8 *) pl_sb_hdr +
				SIZE_C_TEST_SB_RESERVED_FOR_LICENSEE_RESP4_STR);

			SYSLOGSTR(vp_prio,
				  "C_TEST_SB_RESERVED_FOR_LICENSEE_RESP4_STR:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb_hdr->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb_hdr->sb_len);

			SYSLOGSTR(vp_prio,
				  "C_TEST_SB_RESERVED_FOR_LICENSEE_RESP4_SEQ:\n");
			SYSLOGSTR(vp_prio, "band = 0x%X\n", pl_sb->band);
			SYSLOGSTR(vp_prio, "channel = %u\n", pl_sb->channel);
			SYSLOGSTR(vp_prio, "status = 0x%02X\n", pl_sb->status);
			SYSLOGSTR(vp_prio, "power_in_test = %d\n", pl_sb->power_in_test);
			SYSLOGSTR(vp_prio, "power_last_request = %d\n", pl_sb->power_last_request);
			SYSLOGSTR(vp_prio, "power_last_report = %d\n", pl_sb->power_last_report);
			SYSLOGSTR(vp_prio, "power_delta = %d\n", pl_sb->power_delta);
			SYSLOGSTR(vp_prio, "filler1 = %u\n", pl_sb->filler1);
			SYSLOGSTR(vp_prio, "filler2 = %u\n", pl_sb->filler2);
 		}
 		break;

	case C_TEST_SB_RESERVED_FOR_LICENSEE_REQ2:
		{
			C_TEST_SB_RESERVED_FOR_LICENSEE_REQ2_STR *pl_sb_hdr =
			    (C_TEST_SB_RESERVED_FOR_LICENSEE_REQ2_STR *) pp_block;

			C_TEST_SB_RESERVED_FOR_LICENSEE_REQ2_SEQ *pl_sb =
			    (C_TEST_SB_RESERVED_FOR_LICENSEE_REQ2_SEQ *) ((u8 *) pl_sb_hdr +
				SIZE_C_TEST_SB_RESERVED_FOR_LICENSEE_REQ2_STR);

			SYSLOGSTR(vp_prio,
				  "C_TEST_SB_RESERVED_FOR_LICENSEE_REQ2:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb_hdr->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb_hdr->sb_len);

			SYSLOGSTR(vp_prio,
				  "C_TEST_SB_RESERVED_FOR_LICENSEE_REQ2_WCDMA_SEQ:\n");
			SYSLOGSTR(vp_prio, "system = %u\n", pl_sb->system);
			SYSLOGSTR(vp_prio, "band_MSB-band_LSB = %04X-%04X\n", pl_sb->band_MSB, pl_sb->band_LSB);
			SYSLOGSTR(vp_prio, "powerlevel = %d\n", pl_sb->powerlevel);
			SYSLOGSTR(vp_prio, "mode_filler = %d\n", HIGHBYTE(pl_sb->mode_filler));
			SYSLOGSTR(vp_prio, "channel = %u\n", pl_sb->channel);
		}
		break;

	case C_TEST_SB_RESERVED_FOR_LICENSEE_RESP2:
		{
			C_TEST_SB_RESERVED_FOR_LICENSEE_RESP2_STR *pl_sb_hdr =
			    (C_TEST_SB_RESERVED_FOR_LICENSEE_RESP2_STR *) pp_block;

			C_TEST_SB_RESERVED_FOR_LICENSEE_RESP2_SEQ *pl_sb =
			    (C_TEST_SB_RESERVED_FOR_LICENSEE_RESP2_SEQ *) ((u8 *) pl_sb_hdr +
				SIZE_C_TEST_SB_RESERVED_FOR_LICENSEE_RESP2_STR);

			SYSLOGSTR(vp_prio,
				  "C_TEST_SB_RESERVED_FOR_LICENSEE_RESP2:\n");
			SYSLOGSTR(vp_prio, "sb_id = 0x%02X\n", pl_sb_hdr->sb_id);
			SYSLOGSTR(vp_prio, "sb_len = %u\n", pl_sb_hdr->sb_len);

			SYSLOGSTR(vp_prio,
				  "C_TEST_SB_RESERVED_FOR_LICENSEE_RESP2_WCDMA_SEQ:\n");
			SYSLOGSTR(vp_prio, "status = 0x%X\n",
				  LOWBYTE(pl_sb->filler_status));
			SYSLOGSTR(vp_prio, "transpower = %d\n",
				  pl_sb->transpower);
			SYSLOGSTR(vp_prio, "transpower_inraw = %u\n",
				  pl_sb->transpower_inraw);
			SYSLOGSTR(vp_prio, "recvpower = %d\n",
				  pl_sb->recvpower);
			SYSLOGSTR(vp_prio, "recvpower_inraw = %u\n",
				  pl_sb->recvpower_inraw);
			SYSLOGSTR(vp_prio, "filler = %u\n",
				  pl_sb->filler);
		}
		break;

	default:
		SYSLOG(LOG_WARNING, "Unhandled subblock with id 0x%X",
		       pl_sb_id);
	}

	SYSLOG(vp_prio, "\n");
}

void tatl17_dump_header(t_isi_header *pp_hdr, int vp_prio)
{
	SYSLOGSTR(vp_prio, "ISI header:\n");
	SYSLOGSTR(vp_prio, "mediaID = %u, ", pp_hdr->mediaID);
	SYSLOGSTR(vp_prio, "rxDev = %u, ", pp_hdr->rxDev);
	SYSLOGSTR(vp_prio, "txDev = %u, ", pp_hdr->txDev);
	SYSLOGSTR(vp_prio, "resourceID = %u, ", pp_hdr->resourceID);
	SYSLOGSTR(vp_prio, "length = %u, ", pp_hdr->length);
	SYSLOGSTR(vp_prio, "rxObj = %u, ", pp_hdr->rxObj);
	SYSLOGSTR(vp_prio, "txObj = %u.\n", pp_hdr->txObj);
}

void tatl17_dump_msg_wcdma(struct tatrf_isi_msg_t *pp_st, int vp_prio)
{
	u8 vl_trans, vl_msg_id;
	tatl17_11get_id(pp_st, &vl_trans, &vl_msg_id);

	void *pl_msg = tatrf_isi_msg_header(pp_st);

	switch (vl_msg_id) {
	case W_TEST_AGC_LNA_ALIGN_REQ:
		{
			W_TEST_AGC_LNA_ALIGN_REQ_STR *req =
			    (W_TEST_AGC_LNA_ALIGN_REQ_STR *) pl_msg;
			SYSLOGSTR(vp_prio, "W_TEST_AGC_LNA_ALIGN_REQ_STR:\n");
			SYSLOGSTR(vp_prio, "filler1 = %u\n",
				  HIGHBYTE(req->filler1_agc_step_length));
			SYSLOGSTR(vp_prio, "agc_step_length = %u\n",
				  LOWBYTE(req->filler1_agc_step_length));
			SYSLOGSTR(vp_prio, "mid_chan = %u\n", req->mid_chan);
			SYSLOGSTR(vp_prio, "afc_dac_value = %u\n",
				  req->afc_dac_value);
			SYSLOGSTR(vp_prio, "rx_chain_gain = %2X\n",
				  req->rx_chain_gain);
			SYSLOGSTR(vp_prio, "mid_gain1 = %2X\n", req->mid_gain1);
			SYSLOGSTR(vp_prio, "mid_gain2 = %2X\n", req->mid_gain2);
			SYSLOGSTR(vp_prio, "low_gain1 = %2X\n", req->low_gain1);
			SYSLOGSTR(vp_prio, "low_gain2 = %2X\n", req->low_gain2);
			SYSLOGSTR(vp_prio, "agc_slope_align1 = %2X\n",
				  req->agc_slope_align1);
			SYSLOGSTR(vp_prio, "agc_slope_align2 = %2X\n",
				  req->agc_slope_align2);
			SYSLOGSTR(vp_prio, "filler2 = %u\n",
				  HIGHBYTE(req->filler2_rx_align_ctrl));
			SYSLOGSTR(vp_prio, "rx_align_ctrl = %u\n",
				  LOWBYTE(req->filler2_rx_align_ctrl));
			SYSLOGSTR(vp_prio, "low_chan = %u\n", req->low_chan);
			SYSLOGSTR(vp_prio, "high_chan = %u\n", req->high_chan);
			SYSLOGSTR(vp_prio, "\n");
		}
		break;

	case W_TEST_AGC_LNA_ALIGN_RESP:
		{
			W_TEST_AGC_LNA_ALIGN_RESP_STR *req =
			    (W_TEST_AGC_LNA_ALIGN_RESP_STR *) pl_msg;
			SYSLOGSTR(vp_prio, "W_TEST_AGC_LNA_ALIGN_RESP:\n");
			SYSLOGSTR(vp_prio, "filler1 = %u\n",
				  HIGHBYTE(req->filler1_align_status));
			SYSLOGSTR(vp_prio, "align_status = %X\n",
				  LOWBYTE(req->filler1_align_status));
			SYSLOGSTR(vp_prio, "rx_chain_gain = %u\n",
				  req->rx_chain_gain);
			SYSLOGSTR(vp_prio, "low_gain_error = %u\n",
				  req->mid_gain_error);
			SYSLOGSTR(vp_prio, "low_gain_error = %u\n",
				  req->low_gain_error);
			SYSLOGSTR(vp_prio, "agc_gain_error_slope = %u\n",
				  req->agc_gain_error_slope);
			SYSLOGSTR(vp_prio, "bias_error = %u\n",
				  req->bias_error);
			SYSLOGSTR(vp_prio, "low_freq_comp = %u\n",
				  req->low_freq_comp);
			SYSLOGSTR(vp_prio, "high_freq_comp = %u\n",
				  req->high_freq_comp);
			SYSLOGSTR(vp_prio, "\n");
		}
		break;

	default:
		SYSLOG(LOG_WARNING, "No method to dump message 0x%X",
		       vl_msg_id);
	}

	if ((NULL != pp_st->nb_sb) && (pp_st->encoding == client)) {
		/* nb_sb_id */
		u16 vl_num_sb = DEREF_PTR(pp_st->nb_sb, u16);

		u8 *pl_ptr8 = (u8 *) pp_st->nb_sb + 2;

		SYSLOGSTR(vp_prio, "\n%u subblocks:\n\n", vl_num_sb);

		/* dump subblocks */
		u16 vl_sb_len, vl_sb_ndx;
		for (vl_sb_ndx = 0; vl_sb_ndx < vl_num_sb; vl_sb_ndx++) {
			vl_sb_len = DEREF_PTR(pl_ptr8 + 2, u16);
			/*tatl17_dump_sb_wcdma(pl_ptr8); */
			pl_ptr8 += vl_sb_len;
		}
	}
}

void tatl17_dump_msg_common(struct tatrf_isi_msg_t *pp_st, int vp_prio)
{
	u8 vl_trans, vl_msg_id;
	tatl17_11get_id(pp_st, &vl_trans, &vl_msg_id);

	void *pl_msg = tatrf_isi_msg_header(pp_st);

	switch (vl_msg_id) {
    case C_TEST_PA_INFO_REQ:
        {
            C_TEST_PA_INFO_REQ_STR *pl_req = (C_TEST_PA_INFO_REQ_STR *)pl_msg;
            SYSLOGSTR(vp_prio, "C_TEST_PA_INFO_REQ:\n");
            SYSLOGSTR(vp_prio, "system_info = 0x%X\n", pl_req->system_info);
            SYSLOGSTR(vp_prio, "update_pa_type_to_pm = %u\n",
                pl_req->update_pa_type_to_pm);
        }
        break;

    case C_TEST_PA_INFO_RESP:
        {
            C_TEST_PA_INFO_RESP_STR *pl_resp =
                (C_TEST_PA_INFO_RESP_STR *)pl_msg;
            SYSLOGSTR(vp_prio, "C_TEST_TUNING_RESP:\n");
            SYSLOGSTR(vp_prio, "status = 0x%X\n", pl_resp->status);
        }
        break;

	case C_TEST_TUNING_REQ:

		SYSLOGSTR(vp_prio, "C_TEST_TUNING_REQ:\n");
		/* only fillers */
		SYSLOGSTR(vp_prio, "\n");
		break;

	case C_TEST_TUNING_RESP:
		{
			C_TEST_TUNING_RESP_STR *pl_resp =
			    (C_TEST_TUNING_RESP_STR *) pl_msg;
			SYSLOGSTR(vp_prio, "C_TEST_TUNING_RESP:\n");
			SYSLOGSTR(vp_prio, "status = 0x%X\n",
				  LOWBYTE(pl_resp->status));
			SYSLOGSTR(vp_prio, "\n");
		}
		break;

	case C_TEST_IQ_SELF_TUNING_REQ:
		{
			C_TEST_IQ_SELF_TUNING_REQ_STR *pl_req =
			    (C_TEST_IQ_SELF_TUNING_REQ_STR *) pl_msg;
			SYSLOGSTR(vp_prio, "C_TEST_TUNING_REQ:\n");
			SYSLOGSTR(vp_prio, "Operation mode: 0x%X\n"
				  "Filler: 0x%X\n\n",
				  pl_req->tx_iq_mode, pl_req->fill1);
		}
		break;

	case C_TEST_IQ_SELF_TUNING_RESP:

		/* this message has no practical member */
		SYSLOGSTR(vp_prio, "C_TEST_IQ_SELF_TUNING_RESP: (empty)\n\n");
		break;

	case C_HAL_RF_TEST_INFO_REQ:
		{
			C_HAL_RF_TEST_INFO_REQ_STR *pl_req =
			    (C_HAL_RF_TEST_INFO_REQ_STR *) pl_msg;
			SYSLOGSTR(vp_prio, "C_HAL_RF_TEST_INFO_REQ:\n");
			SYSLOGSTR(vp_prio, "info_type = 0x%u\n",
				  pl_req->info_type);
			SYSLOGSTR(vp_prio, "\n");
		}
		break;

	case C_HAL_RF_TEST_INFO_RESP:
		{
			C_HAL_RF_TEST_INFO_RESP_STR *pl_resp =
			    (C_HAL_RF_TEST_INFO_RESP_STR *) pl_msg;
			SYSLOGSTR(vp_prio, "C_HAL_RF_TEST_INFO_RESP:\n");
			SYSLOGSTR(vp_prio, "info_type = 0x%u\n",
				  pl_resp->info_type);
			SYSLOGSTR(vp_prio, "data = 0x%u\n", pl_resp->data);
			SYSLOGSTR(vp_prio, "\n");
		}
		break;

	case C_HAL_RF_TEST_CONTROL_REQ:

		SYSLOGSTR(vp_prio, "C_HAL_RF_TEST_CONTROL_REQ:\n");
		/* this message has no filler ++ */
		SYSLOGSTR(vp_prio, "\n");
		break;

	case C_HAL_RF_TEST_CONTROL_RESP:
		{
			C_HAL_RF_TEST_CONTROL_RESP_STR *pl_resp =
			    (C_HAL_RF_TEST_CONTROL_RESP_STR *) pl_msg;
			SYSLOGSTR(vp_prio, "C_HAL_RF_TEST_CONTROL_RESP:\n");
			/* xxxxxxxx--------  Filler
			   --------xxxxxxxx  RF control response with status
			 */
			SYSLOGSTR(vp_prio, "status = 0x%X\n",
				  LOWBYTE(pl_resp->status));
			SYSLOGSTR(vp_prio, "\n");
		}
		break;

	case C_HAL_RF_TEST_CONTROL_STOP_REQ:

		SYSLOGSTR(vp_prio, "C_HAL_RF_TEST_CONTROL_STOP_REQ:\n");
		/* only fillers */
		SYSLOGSTR(vp_prio, "\n");
		break;

	case C_HAL_RF_TEST_CONTROL_STOP_RESP:

		SYSLOGSTR(vp_prio, "C_HAL_RF_TEST_CONTROL_STOP_RESP:\n");
		/* only fillers */
		SYSLOGSTR(vp_prio, "\n");
		break;

	case C_HAL_RF_TUNING_VALUES_GET_REQ:
		{
			C_HAL_RF_TUNING_VALUES_GET_REQ_STR *pl_req =
			    (C_HAL_RF_TUNING_VALUES_GET_REQ_STR *) pl_msg;
			SYSLOGSTR(vp_prio,
				  "C_HAL_RF_TUNING_VALUES_GET_REQ:\nsource: 0x%2X\n",
				  pl_req->source);
		}
		break;

	case C_HAL_RF_TUNING_VALUES_GET_RESP:
		{
			C_HAL_RF_TUNING_VALUES_GET_RESP_STR *pl_req =
			    (C_HAL_RF_TUNING_VALUES_GET_RESP_STR *) pl_msg;
			SYSLOGSTR(vp_prio,
				  "C_HAL_RF_TUNING_VALUES_GET_RESP:\nstatus: 0x%X\n",
				  LOWBYTE(pl_req->fill1_status));
		}
		break;

	case C_HAL_RF_TUNING_VALUES_SET_REQ:
		{
			C_HAL_RF_TUNING_VALUES_SET_REQ_STR *pl_req =
			    (C_HAL_RF_TUNING_VALUES_SET_REQ_STR *) pl_msg;
			SYSLOGSTR(vp_prio,
				  "C_HAL_RF_TUNING_VALUES_SET_REQ:\ndestination: 0x%2X\n",
				  pl_req->destination);
		}
		break;

	case C_HAL_RF_TUNING_VALUES_SET_RESP:
		{
			C_HAL_RF_TUNING_VALUES_SET_RESP_STR *pl_resp =
			    (C_HAL_RF_TUNING_VALUES_SET_RESP_STR *) pl_msg;
			SYSLOGSTR(vp_prio,
				  "C_HAL_RF_TUNING_VALUES_SET_RESP:\nstatus: 0x%X\n",
				  LOWBYTE(pl_resp->fill1_status));
		}
		break;

	case C_TEST_RFIC_READ_REQ:
		{
			C_TEST_RFIC_READ_REQ_STR *pl_req =
			    (C_TEST_RFIC_READ_REQ_STR *) pl_msg;
			SYSLOGSTR(vp_prio,
				  "C_TEST_RFIC_READ_REQ:\nACCESS_INCR : 0x%X\n ADDRESS MSW : 0x%X\n ADDRESS LSW : 0x%X\n DATA LENGTH : 0x%X\n FILLER :  0x%X\n",
				  pl_req->accs_incr, pl_req->address_msw,
				  pl_req->address_lsw, pl_req->data_length,
				  pl_req->fill1_fill1);
		}
		break;

	case C_TEST_RFIC_READ_RESP:
		{
			C_TEST_RFIC_READ_RESP_STR *pl_resp =
			    (C_TEST_RFIC_READ_RESP_STR *) pl_msg;
			SYSLOGSTR(vp_prio,
				  "C_TEST_RFIC_READ_RESP:\nMODE STATUS: 0x%X\n DATA LENGTH: 0x%X\n",
				  pl_resp->mode_status, pl_resp->data_length);
			PrintBinary(pl_resp->data,
				    pl_resp->data_length * sizeof(u16));
		}
		break;

	case C_TEST_RFIC_WRITE_REQ:
		{
			C_TEST_RFIC_WRITE_REQ_STR *pl_req =
			    (C_TEST_RFIC_WRITE_REQ_STR *) pl_msg;
			SYSLOGSTR(vp_prio,
				  "C_TEST_RFIC_WRITE_REQ:\nACCESS_INCR : 0x%X\n ADDRESS MSW : 0x%X\n ADDRESS LSW : 0x%X\n DATA LENGTH : 0x%X\n",
				  pl_req->accs_incr, pl_req->address_msw,
				  pl_req->address_lsw, pl_req->data_length);
			PrintBinary(pl_req->data,
				    pl_req->data_length * sizeof(u16));
		}
		break;

	case C_TEST_RFIC_WRITE_RESP:
		{
			C_TEST_RFIC_WRITE_RESP_STR *pl_resp =
			    (C_TEST_RFIC_WRITE_RESP_STR *) pl_msg;
			SYSLOGSTR(vp_prio,
				  "C_TEST_RFIC_WRITE_RESP\nMODE_STATUS: 0x%X\n",
				  pl_resp->mode_status);
		}
		break;

	case C_TEST_ANTENNA_CONTROL_REQ:
		{
			C_TEST_ANTENNA_CONTROL_REQ_STR *pl_req =
			    (C_TEST_ANTENNA_CONTROL_REQ_STR *) pl_msg;
			SYSLOGSTR(vp_prio,
				  "C_TEST_ANTENNA_CONTROL_REQ:\n"
				  "Operation Mode: 0x%X\n",
				  pl_req->ant_ctrl_mode);
		}
		break;

	case C_TEST_ANTENNA_CONTROL_RESP:
		{
			C_TEST_ANTENNA_CONTROL_RESP_STR *pl_resp =
			    (C_TEST_ANTENNA_CONTROL_RESP_STR *) pl_msg;
			SYSLOGSTR(vp_prio,
				  "C_TEST_ANTENNA_CONTROL_RESP:\n"
				  "Status: 0x%X\n",
				  LOWBYTE(pl_resp->fill2_status));
		}
		break;

	case STORE_COMMAND_LIST_REQ:
		{
			STORE_COMMAND_LIST_REQ_STR *pl_req =
			    (STORE_COMMAND_LIST_REQ_STR *) pl_msg;
			SYSLOGSTR(vp_prio,
				  "STORE_COMMAND_LIST_REQ:\n"
				  "Nb remaining sb: 0x%u\n",
				  pl_req->num_rem_sub_blocks);
		}
		break;

	case STORE_COMMAND_LIST_RESP:
		{
			STORE_COMMAND_LIST_RESP_STR *pl_resp =
			    (STORE_COMMAND_LIST_RESP_STR *) pl_msg;
			SYSLOGSTR(vp_prio,
				  "STORE_COMMAND_LIST_RESP:\n" "Status: 0x%X\n",
				  LOWBYTE(pl_resp->fill1_status));
		}
		break;

	case START_COMMAND_LIST_REQ:

		SYSLOGSTR(vp_prio, "START_COMMAND_LIST_REQ:\n");
		/* only fillers */
		SYSLOGSTR(vp_prio, "\n");
		break;

	case START_COMMAND_LIST_RESP:
		{
			START_COMMAND_LIST_RESP_STR *pl_resp =
			    (START_COMMAND_LIST_RESP_STR *) pl_msg;
			SYSLOGSTR(vp_prio,
				  "START_COMMAND_LIST_RESP:\n" "Status: 0x%X\n",
				  LOWBYTE(pl_resp->fill1_status));
		}
		break;

	case STOP_COMMAND_LIST_REQ:

		SYSLOGSTR(vp_prio, "STOP_COMMAND_LIST_REQ:\n");
		/* only fillers */
		SYSLOGSTR(vp_prio, "\n");
		break;

	case STOP_COMMAND_LIST_RESP:
		{
			STOP_COMMAND_LIST_RESP_STR *pl_resp =
			    (STOP_COMMAND_LIST_RESP_STR *) pl_msg;
			SYSLOGSTR(vp_prio,
				  "STOP_COMMAND_LIST_RESP:\n" "Status: 0x%X\n",
				  LOWBYTE(pl_resp->fill1_status));
		}
		break;

	case GET_STATUS_REQ:

		SYSLOGSTR(vp_prio, "GET_STATUS_REQ:\n");
		/* only fillers */
		SYSLOGSTR(vp_prio, "\n");
		break;

	case GET_STATUS_RESP:
		{
			GET_STATUS_RESP_STR *pl_resp =
			    (GET_STATUS_RESP_STR *) pl_msg;
			SYSLOGSTR(vp_prio,
				  "GET_STATUS_RESP:\n" "Status: 0x%X\n",
				  LOWBYTE(pl_resp->fill1_status));
		}
		break;

	case GET_RESULTS_REQ:

		SYSLOGSTR(vp_prio, "GET_RESULTS_REQ:\n");
		/* only fillers */
		SYSLOGSTR(vp_prio, "\n");
		break;

	case GET_RESULTS_RESP:
		{
			GET_RESULTS_RESP_STR *pl_resp =
			    (GET_RESULTS_RESP_STR *) pl_msg;
			SYSLOGSTR(vp_prio,
				  "GET_RESULTS_RESP:\n" "Status: 0x%X\n"
				  "More sb: 0x%X\n",
				  LOWBYTE(pl_resp->fill1_status),
				  LOWBYTE(pl_resp->fill2_is_remaining_sb));
		}
		break;

	case SEQUENCER_RESET_REQ:

		SYSLOGSTR(vp_prio, "SEQUENCE_RESET_REQ:\n");
		/* only fillers */
		SYSLOGSTR(vp_prio, "\n");
		break;

	case SEQUENCER_RESET_RESP:
		{
			SEQUENCER_RESET_RESP_STR *pl_resp =
			    (SEQUENCER_RESET_RESP_STR *) pl_msg;
			SYSLOGSTR(vp_prio,
				  "SEQUENCER_RESET_RESP:\n" "Status: 0x%X\n",
				  LOWBYTE(pl_resp->fill1_status));
		}
		break;

	default:
		SYSLOG(LOG_WARNING, "No method to dump message 0x%X",
		       vl_msg_id);
	}

	if ((NULL != pp_st->nb_sb) && (client == pp_st->encoding)) {
		/* nb_sb_id */
		u16 vl_num_sb = DEREF_PTR(pp_st->nb_sb, u16);

		u8 *pl_ptr8 = ((u8 *) pp_st->nb_sb) + 2;

		SYSLOGSTR(vp_prio, "\n%u subblocks:\n", vl_num_sb);

		/* dump subblocks */
		u16 vl_sb_len, vl_sb_ndx;
		for (vl_sb_ndx = 0; vl_sb_ndx < vl_num_sb; vl_sb_ndx++) {
			vl_sb_len = DEREF_PTR(pl_ptr8 + 2, u16);
			tatl17_dump_sb_common(pl_ptr8, vp_prio);
			pl_ptr8 += vl_sb_len;
		}
	}
}

void tatl17_dump_msg_modem(struct tatrf_isi_msg_t *pp_st, int vp_prio)
{
	u8 vl_trans, vl_msg_id;
	tatl17_11get_id(pp_st, &vl_trans, &vl_msg_id);

	void *pl_msg = tatrf_isi_msg_header(pp_st);

	switch (vl_msg_id) {
	case MODEM_TEST_RUN_REQ:
		{
			MODEM_TEST_RUN_REQ_STR *pl_req =
			    (MODEM_TEST_RUN_REQ_STR *) pl_msg;
			SYSLOGSTR(vp_prio,
				  "MODEM_TEST_RUN_REQ:\n"
				  "Test Group Id: 0x%X\n"
				  "Test Handler Id: 0x%X\n"
				  "Test Case Id: 0x%X\n" "Attrib:  0x%X\n"
				  "Data Lenght: 0x%X\n\n", pl_req->group_id,
				  pl_req->test_id, pl_req->case_ID,
				  pl_req->attrib, pl_req->inp_data_length);
		}
		break;

	case MODEM_TEST_RUN_RESP:
		{
			MODEM_TEST_RUN_RESP_STR *pl_resp =
			    (MODEM_TEST_RUN_RESP_STR *) pl_msg;
			SYSLOGSTR(vp_prio,
				  "MODEM_TEST_RUN_RESP:\n"
				  "Test Group Id: 0x%X\n"
				  "Test Handler Id: 0x%X\n"
				  "Test Case Id: 0x%X\n" "Status:  0x%X\n"
				  "Data Lenght: 0x%X\n\n", pl_resp->group_id,
				  pl_resp->test_id, pl_resp->case_ID,
				  pl_resp->status, pl_resp->out_data_length);
		}
		break;

	default:
		SYSLOG(LOG_WARNING, "No method to dump MODEM message 0x%X",
		       vl_msg_id);
	}
}

void tatl17_10dump(struct tatrf_isi_msg_t *pp_st, const char *pp_name,
		   int vp_prio)
{
	tat_modem_print_isi((uint8_t *) pp_st->msg,
			    tatrf_isi_total_length(pp_st), vp_prio);

	if (pp_name && strlen(pp_name)) {
		SYSLOGSTR(vp_prio, "\nMESSAGE \"%s\":\n\n", pp_name);
	}

	if (client != pp_st->encoding) {
		SYSLOG(LOG_WARNING, "Not client encoded");
		return;
	}

	/* Dump ISI header */
	t_isi_header *pl_hdr = (t_isi_header *) pp_st->msg;
	tatl17_dump_header(pl_hdr, vp_prio);

	u8 vl_trans, vl_msg_id;
	tatl17_11get_id(pp_st, &vl_trans, &vl_msg_id);
	SYSLOGSTR(vp_prio, "trans id = %u\nmessage id = 0x%X\n", vl_trans,
		  vl_msg_id);

	/* delegate to appropriate handler according to resource ID */
	if (pp_st != NULL) {
		u8 vl_resource_id = ((t_isi_header *) pp_st->msg)->resourceID;
		switch (vl_resource_id) {
		case PN_DSP_COMMON_TEST:
			tatl17_dump_msg_common(pp_st, vp_prio);
			break;

		case PN_DSP_WCDMA_TEST:
			tatl17_dump_msg_wcdma(pp_st, vp_prio);
			break;

		case PN_MODEM_TEST:
			tatl17_dump_msg_modem(pp_st, vp_prio);
			break;

		default:
			SYSLOG(LOG_WARNING,
			       "No method available to dump message with resource ID of 0x%X",
			       vl_resource_id);
		}
	}
}
