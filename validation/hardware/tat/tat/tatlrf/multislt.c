/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   multislt.c
* \brief   declare DTH routines for 2G RX/TX multislots
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "rfdef.h"
#include "multislt.h"

#include "misc.h"
#include "isimsg.h"


/* This structure is used to maintain RX/TX multi slots input data */
C_HAL_SB_GSM_RXTX_CONTROL_STR v_tatlrf_rxtxmultislot = {
    .sb_id = C_HAL_SB_GSM_RXTX_CONTROL,
    .sb_len = SIZE_C_HAL_SB_GSM_RXTX_CONTROL_STR,
    .gsm_band_info = INFO_NO_GSM,
    .rf_channel = 0u,
    .rx_slot_mask_tx_slot_mask = 0u,
    .tx_8psk_mask_mon_rach_mask = 0u,
    .data_type_afc_type = 0u,
    .manualAfc = 0u,
    .tx_timing_advance = 0u,
    .agc_mode_tx_power_unit = 0u,
    .slot_level = { 0, 0, 0, 0, 0, 0, 0, 0 }
};

u8 v_tatlrf_rxtxmultislot_status = 0xFF;

enum {
	ACT_RX_TX_MULTISLOT_START = 0,
	ACT_RX_TX_MULTISLOT_STOP
};

int v_tatlrf_rxtxmultislot_action = ACT_RX_TX_MULTISLOT_STOP;

typedef enum {
	/* Zero data */
	MULTISLOT_DATA_TYPE_ZERO,
	/* One data */
	MULTISLOT_DATA_TYPE_ONE,
	/* PN9 data for BB5.0, GSMK dummy burst for DCT4 */
	MULTISLOT_DATA_TYPE_PN9_BB5_0,
	/* PN9 data */
	MULTISLOT_DATA_TYPE_PN9,
	/* GSMK dummy burst */
	MULTISLOT_DATA_TYPE_GSMK
} MULTISLOT_DATA_TYPE;

typedef enum {
	MULTISLOT_AFC_TUNED,
	MULTISLOT_AFC_FREQ_ABS_MANUAL,
	MULTISLOT_AFC_FREQ_PPM_MANUAL
} MULTISLOT_AFC_TYPE;

typedef enum {
	/* Use MON for AGC adjust */
	MULTISLOT_AGC_AUTOMATIC,
	/* Use given AGC value */
	MULTISLOT_AGC_MANUAL
} MULTISLOT_AGC_MODE;

typedef enum {
	/* Tx power use PCL level */
	MULTISLOT_PWR_UNIT_PCL,
	/* Tx power use dBm level */
	MULTISLOT_PWR_UNIT_DBM
} MULTISLOT_PWR_UNIT;

/* Function tide to RX/TX multi slots             */
/**************************************************/
int DthRf_RxTxMultiSlot_Get(struct dth_element *elem, void *value)
{
	return tatl13_01RxTxMultiSlots_Get(elem, value);
}

int DthRf_RxTxMultiSlot_Set(struct dth_element *elem, void *value)
{
	return tatl13_02RxTxMultiSlots_Set(elem, value);
}

int DthRf_RxTxMultiSlot_Exec(struct dth_element *elem)
{
    (void) elem;

	return tatl13_00RxTxMultiSlots_Exec();
}

int tatl13_startRxTxMultiSlots(void);
int tatl13_encodeLevel(float level, u8 slot, int16 * encoded);
int tatl13_decodeLevel(int16 value, u8 slot, float *level);

int tatl13_00RxTxMultiSlots_Exec(void)
{
	int vl_Error = TAT_BAD_REQ;	/* TAT error code */

	switch (v_tatlrf_rxtxmultislot_action) {
	case ACT_RX_TX_MULTISLOT_START:
		{
			/* stop RX/TX multislot if already running before restart */
			SYSLOG(LOG_NOTICE, "Stopping RX/TX multislots...");
			vl_Error = tatl3_03StopRfActivity();

			if (TAT_OK(vl_Error)) {
				/* start RX/TX multislots */
				SYSLOG(LOG_NOTICE,
				       "Starting RX/TX multislots...");
				vl_Error = tatl13_startRxTxMultiSlots();
			}
		}
		break;

	case ACT_RX_TX_MULTISLOT_STOP:

		/* stop RX/TX multislot */
		SYSLOG(LOG_NOTICE, "Stopping RX/TX multislots...");
		vl_Error = tatl3_03StopRfActivity();
		break;
	}

	return vl_Error;
}

int tatl13_01RxTxMultiSlots_Get(struct dth_element *elem, void *value)
{
	int vl_Error = TAT_ERROR_OFF;	/* errno return code */

	/* Switch on elem user data, cast value and fill the corresponding internal value */
	switch (elem->user_data) {
	case ACT_RXTX_MULTISLOT_STATUS:	/* TYPE U8 */

		ASSERT(elem->type == DTH_TYPE_U8,
		       "unexpected type for dth element ACT_RXTX_MULTISLOT_STATUS");
		DEREF_PTR_SET(value, u8, v_tatlrf_rxtxmultislot_status);
		SYSLOG(LOG_DEBUG, "get: RX/TX multi slots status = %u",
		       DEREF_PTR(value, u8));
		break;

	case ACT_RXTX_MULTISLOT_LEVELS:
		{
			ASSERT(elem->type == DTH_TYPE_FLOAT,
			       "unexpected type for dth element ACT_RXTX_MULTISLOT_LEVELS");
			int slot;
			float vl_level;
			struct dth_array *vl_array = (struct dth_array *)value;
			float *vl_dst = (float *)vl_array->array;

			if ((vl_array->col == elem->cols)
			    && (vl_array->row == elem->rows)) {
				/* copy all elements from array to data */
				vl_Error = TAT_ERROR_OFF;
				for (slot = 0; (slot < 8) && TAT_OK(vl_Error);
				     slot++) {
					vl_Error =
					    tatl13_decodeLevel
					    (v_tatlrf_rxtxmultislot.
					     slot_level[slot], slot, &vl_level);
					if (TAT_OK(vl_Error)) {
						SYSLOGSTR(LOG_DEBUG,
							  "get: RX/TX multi slots level %d : %lf\n",
							  slot, vl_level);
						DEREF_PTR_SET(vl_dst + slot,
							      float, vl_level);
					}
				}
			} else if ((vl_array->col < elem->cols)
				   && (vl_array->row < elem->rows)) {
				slot = vl_array->col;
				vl_Error =
				    tatl13_decodeLevel(v_tatlrf_rxtxmultislot.
						       slot_level[slot], slot,
						       &vl_level);
				if (TAT_OK(vl_Error)) {
					SYSLOGSTR(LOG_DEBUG,
						  "get: RX/TX multi slots level %d : %lf\n",
						  slot, vl_level);
					DEREF_PTR_SET(vl_dst + slot, float,
						      vl_level);
				}
			} else {
				/* error: array col or/and row are out of range. */
				SYSLOG(LOG_ERR,
				       "cell (c:%d, r:%d) is out of range!",
				       vl_array->col, vl_array->row);
			}
		}
		break;

		/* Raise error if elem user data is not an valid output */
	default:

		SYSLOG(LOG_ERR, "invalid output: %d", elem->user_data);
		vl_Error = TAT_BAD_REQ;
	}

	return vl_Error;
}

int tatl13_02RxTxMultiSlots_Set(struct dth_element *elem, void *value)
{
	int vl_Error = TAT_ERROR_OFF;	/* TAT error code */
	struct dth_array *vl_array = NULL;

	/* Switch on elem user data, cast value and fill the corresponding internal value */
	switch (elem->user_data) {
	case ACT_RXTX_MULTISLOT:	/* TYPE U32 */

		ASSERT(elem->type == DTH_TYPE_U32,
		       "unexpected type for dth element ACT_RXTX_MULTISLOT");
		switch (DEREF_PTR(value, u32)) {
		case ACT_RX_TX_MULTISLOT_START:

			SYSLOG(LOG_DEBUG, "set: starting RX/TX multislots");
			v_tatlrf_rxtxmultislot_action =
			    ACT_RX_TX_MULTISLOT_START;
			break;

		case ACT_RX_TX_MULTISLOT_STOP:

			SYSLOG(LOG_DEBUG, "set: stopping RX/TX multislots");
			v_tatlrf_rxtxmultislot_action =
			    ACT_RX_TX_MULTISLOT_STOP;
			break;

		default:
			SYSLOG(LOG_ERR,
			       "set: invalid ACT_RXTX_MULTISLOT value");
			vl_Error = TAT_BAD_REQ;
		}
		break;

	case ACT_RXTX_MULTISLOT_BAND:	/* TYPE U32 */

		ASSERT(elem->type == DTH_TYPE_U32,
		       "unexpected type for dth element ACT_RXTX_MULTISLOT_BAND");
		v_tatlrf_rxtxmultislot.gsm_band_info =
		    tatl3_04GetGsmRfBand(DEREF_PTR(value, u32));
		SYSLOG(LOG_DEBUG, "set: RX/TX multi slots RF band to %lu\n",
		       v_tatlrf_rxtxmultislot.gsm_band_info);
		break;

	case ACT_RXTX_MULTISLOT_CHANNEL:	/* TYPE U16 */

		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_RXTX_MULTISLOT_CHANNEL");
		v_tatlrf_rxtxmultislot.rf_channel = DEREF_PTR(value, u16);
		SYSLOG(LOG_DEBUG, "set: RX/TX multi slots channel to %u\n",
		       v_tatlrf_rxtxmultislot.rf_channel);
		break;

	case ACT_RXTX_MULTISLOT_SLOTS:	/* TYPE U8[C:4] */

		ASSERT(elem->type == DTH_TYPE_U8,
		       "unexpected type for dth element ACT_RXTX_MULTISLOT_SLOTS");
		vl_array = (struct dth_array *)value;
		SYSLOG(LOG_DEBUG, "set: RX/TX multi slots slots...\n");
		/* this code works because ACT_RXTX_MULTISLOT_SLOTS slots are ordered 
         * exactly as the mask bytes in the structure are */
		SetDthArray(elem, vl_array, sizeof(u8),
			    &v_tatlrf_rxtxmultislot.rx_slot_mask_tx_slot_mask,
			    sizeof(u8), TAT_RF_RX_TX_MULTISLOTS_NB_SLOTS);
		SYSLOG(LOG_DEBUG, "set: RX/TX multi slots RX and TX to 0x%04X",
		       v_tatlrf_rxtxmultislot.rx_slot_mask_tx_slot_mask);
		SYSLOG(LOG_DEBUG,
		       "set: RX/TX multi slots 8PSK and Mon/Rach to 0x%04X",
		       v_tatlrf_rxtxmultislot.tx_8psk_mask_mon_rach_mask);
		break;

	case ACT_RXTX_MULTISLOT_DATA_TYPE:	/* TYPE U8 */
		{
			ASSERT(elem->type == DTH_TYPE_U8,
			       "unexpected type for dth element ACT_RXTX_MULTISLOT_DATA_TYPE");
			u8 dth_data_type = DEREF_PTR(value, u8);

			switch (dth_data_type) {
			case MULTISLOT_DATA_TYPE_ZERO:

				v_tatlrf_rxtxmultislot.data_type_afc_type =
				    SetU8InU16(MSB_POSS,
					       v_tatlrf_rxtxmultislot.
					       data_type_afc_type,
					       C_TEST_DATA_TYPE_0);
				SYSLOG(LOG_DEBUG,
				       "RX/TX type set to Zero data");
				break;

			case MULTISLOT_DATA_TYPE_ONE:

				v_tatlrf_rxtxmultislot.data_type_afc_type =
				    SetU8InU16(MSB_POSS,
					       v_tatlrf_rxtxmultislot.
					       data_type_afc_type,
					       C_TEST_DATA_TYPE_1);
				SYSLOG(LOG_DEBUG, "RX/TX type set to One data");
				break;

			case MULTISLOT_DATA_TYPE_PN9_BB5_0:

				v_tatlrf_rxtxmultislot.data_type_afc_type =
				    SetU8InU16(MSB_POSS,
					       v_tatlrf_rxtxmultislot.
					       data_type_afc_type,
					       C_TEST_DATA_TYPE_RAND);
				SYSLOG(LOG_DEBUG,
				       "RX/TX type set to PN9 data for BB5.0, GMSK dummy burst for DCT4");
				break;

			case MULTISLOT_DATA_TYPE_PN9:

				v_tatlrf_rxtxmultislot.data_type_afc_type =
				    SetU8InU16(MSB_POSS,
					       v_tatlrf_rxtxmultislot.
					       data_type_afc_type,
					       C_TEST_DATA_TYPE_PN9);
				SYSLOG(LOG_DEBUG, "RX/TX type set to PN9 data");
				break;

			case MULTISLOT_DATA_TYPE_GSMK:

				v_tatlrf_rxtxmultislot.data_type_afc_type =
				    SetU8InU16(MSB_POSS,
					       v_tatlrf_rxtxmultislot.
					       data_type_afc_type,
					       C_TEST_DATA_TYPE_DUMMY);
				SYSLOG(LOG_DEBUG,
				       "RX/TX type set to GMSK Dummy burst");
				break;

			default:
				vl_Error = TAT_BAD_REQ;
			}
		}
		break;

	case ACT_RXTX_MULTISLOT_AFC_MODE:	/* TYPE U8 */
		{
			ASSERT(elem->type == DTH_TYPE_U8,
			       "unexpected type for dth element ACT_RXTX_MULTISLOT_AFC_MODE");
			u8 dth_afc_type = DEREF_PTR(value, u8);

			switch (dth_afc_type) {
			case MULTISLOT_AFC_TUNED:

				v_tatlrf_rxtxmultislot.data_type_afc_type =
				    SetU8InU16(LSB_POSS,
					       v_tatlrf_rxtxmultislot.
					       data_type_afc_type,
					       CTRL_AFC_TUNED);
				SYSLOG(LOG_DEBUG,
				       "RX/TX AFC type set to TUNED");
				break;

			case MULTISLOT_AFC_FREQ_ABS_MANUAL:

				v_tatlrf_rxtxmultislot.data_type_afc_type =
				    SetU8InU16(LSB_POSS,
					       v_tatlrf_rxtxmultislot.
					       data_type_afc_type,
					       CTRL_AFC_FREQ_ABS_MANUAL);
				SYSLOG(LOG_DEBUG,
				       "RX/TX AFC type set to ABS MANUAL");
				break;

			case MULTISLOT_AFC_FREQ_PPM_MANUAL:

				v_tatlrf_rxtxmultislot.data_type_afc_type =
				    SetU8InU16(LSB_POSS,
					       v_tatlrf_rxtxmultislot.
					       data_type_afc_type,
					       CTRL_AFC_FREQ_PPM_MANUAL);
				SYSLOG(LOG_DEBUG,
				       "RX/TX AFC type set to PPM MANUAL");
				break;

			default:
				vl_Error = TAT_BAD_REQ;
			}
		}
		break;

	case ACT_RXTX_MULTISLOT_AFC_PPM:	/* TYPE FLOAT */
		{
			if (CTRL_AFC_FREQ_PPM_MANUAL ==
			    GetU8InU16(LSB_POSS,
				       v_tatlrf_rxtxmultislot.
				       data_type_afc_type)) {
				ASSERT(elem->type == DTH_TYPE_FLOAT,
				       "unexpected type for dth element ACT_RXTX_MULTISLOT_AFC_PPM");
				float dth_afc_ppm = DEREF_PTR(value, float);
				v_tatlrf_rxtxmultislot.manualAfc =
				    (uint32) RF_Q(dth_afc_ppm,
						  TAT_RF_MULTISLOT_QUOT_AFC_PPM);
				SYSLOG(LOG_DEBUG,
				       "set: RX/TX multi slots manual AFC to %f (Q10 conversion: %lu)",
				       dth_afc_ppm,
				       v_tatlrf_rxtxmultislot.manualAfc);
			}
		}
		break;

	case ACT_RXTX_MULTISLOT_AFC_OFFSET:	/* TYPE U32 */

		if (CTRL_AFC_FREQ_ABS_MANUAL ==
		    GetU8InU16(LSB_POSS,
			       v_tatlrf_rxtxmultislot.data_type_afc_type)) {
			ASSERT(elem->type == DTH_TYPE_U32,
			       "unexpected type for dth element ACT_RXTX_MULTISLOT_AFC_OFFSET");
			v_tatlrf_rxtxmultislot.manualAfc =
			    DEREF_PTR(value, u32);
			SYSLOG(LOG_DEBUG,
			       "set: RX/TX multi slots manual AFC to %lu",
			       v_tatlrf_rxtxmultislot.manualAfc);
		}
		break;

	case ACT_RXTX_MULTISLOT_TX_TIMING_ADVANCE:	/* TYPE U8 */

		ASSERT(elem->type == DTH_TYPE_U8,
		       "unexpected type for dth element ACT_RXTX_MULTISLOT_TX_TIMING_ADVANCE");
		v_tatlrf_rxtxmultislot.tx_timing_advance = DEREF_PTR(value, u8);
		SYSLOG(LOG_DEBUG,
		       "set: RX/TX multi slots Tx timing advance to %u",
		       v_tatlrf_rxtxmultislot.tx_timing_advance);
		break;

	case ACT_RXTX_MULTISLOT_AGC_MODE:	/* TYPE U8 */
		{
			ASSERT(elem->type == DTH_TYPE_U8,
			       "unexpected type for dth element ACT_RXTX_MULTISLOT_AGC_MODE");
			u8 dth_agc_mode = DEREF_PTR(value, u8);

			switch (dth_agc_mode) {
			case MULTISLOT_AGC_AUTOMATIC:

				v_tatlrf_rxtxmultislot.agc_mode_tx_power_unit =
				    SetU8InU16(MSB_POSS,
					       v_tatlrf_rxtxmultislot.
					       agc_mode_tx_power_unit,
					       CTRL_RX_AGC_AUTOMATIC);
				SYSLOG(LOG_DEBUG,
				       "RX/TX AGC mode set to AUTOMATIC");
				break;

			case MULTISLOT_AGC_MANUAL:

				v_tatlrf_rxtxmultislot.agc_mode_tx_power_unit =
				    SetU8InU16(MSB_POSS,
					       v_tatlrf_rxtxmultislot.
					       agc_mode_tx_power_unit,
					       CTRL_RX_AGC_MANUAL);
				SYSLOG(LOG_DEBUG,
				       "RX/TX AGC mode set to MANUAL");
				break;

			default:
				vl_Error = TAT_BAD_REQ;
			}
		}
		break;

	case ACT_RXTX_MULTISLOT_TX_PWR_LEVEL_UNIT:	/* TYPE U8 */
		{
			ASSERT(elem->type == DTH_TYPE_U8,
			       "unexpected type for dth element ACT_RXTX_MULTISLOT_TX_PWR_LEVEL_UNIT");
			u8 dth_pwr_unit = DEREF_PTR(value, u8);
			switch (dth_pwr_unit) {
			case MULTISLOT_PWR_UNIT_PCL:

				v_tatlrf_rxtxmultislot.agc_mode_tx_power_unit =
				    SetU8InU16(LSB_POSS,
					       v_tatlrf_rxtxmultislot.
					       agc_mode_tx_power_unit,
					       CTRL_TX_PCL);
				SYSLOG(LOG_DEBUG,
				       "RX/TX power level unit set to PCL");
				break;

			case MULTISLOT_PWR_UNIT_DBM:

				v_tatlrf_rxtxmultislot.agc_mode_tx_power_unit =
				    SetU8InU16(LSB_POSS,
					       v_tatlrf_rxtxmultislot.
					       agc_mode_tx_power_unit,
					       CTRL_TX_DBM);
				SYSLOG(LOG_DEBUG,
				       "RX/TX power level unit set to DBm");
				break;

			default:
				vl_Error = TAT_BAD_REQ;
			}
		}
		break;

	case ACT_RXTX_MULTISLOT_LEVELS:
		{
			ASSERT(elem->type == DTH_TYPE_FLOAT,
			       "unexpected type for dth element ACT_RXTX_MULTISLOT_LEVELS");
			int slot;
			struct dth_array *vl_array = (struct dth_array *)value;
			float *vl_src = (float *)vl_array->array;

			if ((vl_array->col == elem->cols)
			    && (vl_array->row == elem->rows)) {
				/* copy all elements from array to data */
				for (slot = 0; (slot < 8) && TAT_OK(vl_Error);
				     slot++) {
					vl_Error =
					    tatl13_encodeLevel(vl_src[slot],
							       slot,
							       &v_tatlrf_rxtxmultislot.
							       slot_level
							       [slot]);
					SYSLOGSTR(LOG_DEBUG,
						  "set RX/TX multi slots level %d to %u\n",
						  slot,
						  v_tatlrf_rxtxmultislot.
						  slot_level[slot]);
				}
			} else if ((vl_array->col < elem->cols)
				   && (vl_array->row < elem->rows)) {
				slot = vl_array->col;
				vl_Error =
				    tatl13_encodeLevel(vl_src[slot], slot,
						       &v_tatlrf_rxtxmultislot.
						       slot_level[slot]);
				SYSLOGSTR(LOG_DEBUG,
					  "set RX/TX multi slots level %d to %u\n",
					  slot,
					  v_tatlrf_rxtxmultislot.
					  slot_level[slot]);
			} else {
				/* error: array col or/and row are out of range. */
				SYSLOG(LOG_ERR,
				       "cell (c:%d, r:%d) is out of range!",
				       vl_array->col, vl_array->row);
			}
		}
		break;

		/* Raise error if elem user data is not an valid input */
	default:
		SYSLOG(LOG_ERR, "invalid input: %d", elem->user_data);
		vl_Error = TAT_BAD_REQ;
	}

	return vl_Error;
}

/*
TX / RX level value meaning table

Rx(n) | Tx(n) | 8PSK(n) | Mon/RACH(n)   | AGC mode | unit | Rx/Tx level
------+-------+---------+---------------+----------+------+------------
0     | 0     | NA      | no effect	    | NA       | NA   | ERROR
------+-------+---------+---------------+----------+------+------------
1     | 0     | NA      | 0=normal Rx   | manual   | NA   | dBm (Q8)
      |       |         | 1=monitoring  | auto     |      | none
------+-------+---------+---------------+----------+------+------------
0     | 1     | 0       | 0=normal Tx   | NA       | PCL  | 0-19 (Q0)
      |       |         | 1=access burst|          | dBm  | dBm (Q8)
------+-------+---------+---------------+----------+------+------------
1     | 1     | NA      | ?     	    | ?        | ?    | ERROR
*/
int tatl13_encodeLevel(float level, u8 slot, int16 * encoded)
{
	int vl_Error = TAT_ERROR_OFF;
	u16 rx_slot, tx_slot;

	if (slot < 8) {
		rx_slot =
		    GetU8InU16(MSB_POSS,
			       v_tatlrf_rxtxmultislot.
			       rx_slot_mask_tx_slot_mask) & (0x01 << slot);
		tx_slot =
		    GetU8InU16(LSB_POSS,
			       v_tatlrf_rxtxmultislot.
			       rx_slot_mask_tx_slot_mask) & (0x01 << slot);

		if ((0 != rx_slot) && (tx_slot == 0)) {
			/* RX */
			if (GetU8InU16
			    (MSB_POSS,
			     v_tatlrf_rxtxmultislot.agc_mode_tx_power_unit) ==
			    CTRL_RX_AGC_MANUAL) {
				/* RX in manual AGC mode */
				*encoded =
				    RF_Q(level,
					 TAT_RF_MULTISLOT_QUOT_LEVEL_DBM);
			} else {
				/* level is unrelevant */
				*encoded = 0;
			}
		} else if ((0 != tx_slot) && (0 == rx_slot)) {
			/* TX */
			switch (GetU8InU16
				(LSB_POSS,
				 v_tatlrf_rxtxmultislot.
				 agc_mode_tx_power_unit)) {
				/* Tx value in PCL */
			case CTRL_TX_PCL:

				*encoded = (u16) level;
				break;

				/* TX value in dBm */
			case CTRL_TX_DBM:

				*encoded =
				    RF_Q(level,
					 TAT_RF_MULTISLOT_QUOT_LEVEL_DBM);
				break;

			default:
				vl_Error = TAT_BAD_REQ;
			}
		} else if ((0 != tx_slot) && (0 != rx_slot)) {
			/* both TX and RX: invalid configuration! */
			SYSLOG(LOG_ERR,
			       "slot %d must have TX or RX but not both at the same time.",
			       slot);
			vl_Error = TAT_BAD_REQ;
		}
		/* else rx_slot = 0 and tx_slot = 0 means slot is not used */
		else {
			*encoded = 0;
		}
	} else {
		/* error: invalid slot number */
		vl_Error = TAT_BAD_REQ;
	}

	return vl_Error;
}

int tatl13_decodeLevel(int16 value, u8 slot, float *level)
{
	int vl_Error = TAT_ERROR_OFF;
	u16 rx_slot, tx_slot;

	if (slot < 8) {
		rx_slot =
		    GetU8InU16(MSB_POSS,
			       v_tatlrf_rxtxmultislot.
			       rx_slot_mask_tx_slot_mask) & (0x01 << slot);
		tx_slot =
		    GetU8InU16(LSB_POSS,
			       v_tatlrf_rxtxmultislot.
			       rx_slot_mask_tx_slot_mask) & (0x01 << slot);

		if ((0 != rx_slot) && (tx_slot == 0)) {
			/* RX */
			if (GetU8InU16
			    (MSB_POSS,
			     v_tatlrf_rxtxmultislot.agc_mode_tx_power_unit) ==
			    CTRL_RX_AGC_MANUAL) {
				/* RX in manual AGC mode */
				*level =
				    RF_UNQ(value,
					   TAT_RF_MULTISLOT_QUOT_LEVEL_DBM);
			} else {
				/* level is unrelevant */
				*level = value;
			}
		} else if ((0 != tx_slot) && (0 == rx_slot)) {
			/* TX */
			switch (GetU8InU16
				(LSB_POSS,
				 v_tatlrf_rxtxmultislot.
				 agc_mode_tx_power_unit)) {
				/* Tx value in PCL */
			case CTRL_TX_PCL:

				*level = value;
				break;

				/* TX value in dBm */
			case CTRL_TX_DBM:

				*level =
				    RF_UNQ(value,
					   TAT_RF_MULTISLOT_QUOT_LEVEL_DBM);
				break;

			default:
				vl_Error = TAT_BAD_REQ;
			}
		} else if ((0 != tx_slot) && (0 != rx_slot)) {
			/* both TX and RX: invalid configuration! */
			SYSLOG(LOG_ERR,
			       "slot %d must have TX or RX but not both at the same time.",
			       slot);
			vl_Error = TAT_BAD_REQ;
		}
		/* else rx_slot = 0 and tx_slot = 0 means slot is not used */
		else {
			*level = value;
		}
	} else {
		/* error: invalid slot number */
		vl_Error = TAT_BAD_REQ;
	}

	return vl_Error;
}

int tatl13_startRxTxMultiSlots(void)
{
	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	C_HAL_RF_TEST_CONTROL_REQ_STR *pl_req;
	pl_req =
	    tatrf_isi_msg_begin(&vl_msg_info, PN_DSP_COMMON_TEST,
				C_HAL_RF_TEST_CONTROL_REQ, num_sb_id);
	if (NULL == pl_req) {
		return TAT_ISI_HANDLER_ERROR;
	}

	C_HAL_SB_GSM_RXTX_CONTROL_STR *pl_sb =
	    tatrf_isi_msg_add_sb_copy(&vl_msg_info, C_HAL_SB_GSM_RXTX_CONTROL,
				      &v_tatlrf_rxtxmultislot,
				      sizeof(v_tatlrf_rxtxmultislot));
	if (NULL == pl_sb) {
		return TAT_ISI_HANDLER_ERROR;
	}

	/* send ISI message - blocking function */
	int vl_Error = tatrf_isi_send(&vl_msg_info, "RX/TX multislots");
	if (!TAT_OK(vl_Error)) {
		return vl_Error;
	}

	/* message successfully sent. waiting for response */
	v_tatlrf_rxtxmultislot_status = 0xFF;

	C_HAL_RF_TEST_CONTROL_RESP_STR *pl_resp =
	    tatrf_isi_read_simple(&vl_msg_info, C_HAL_RF_TEST_CONTROL_RESP,
				  "RX/TX multislots response", &vl_Error);
	if (NULL != pl_resp) {
		/* this what we expected */
		v_tatlrf_rxtxmultislot_status =
		    GetU8InU16(LSB_POSS, pl_resp->status);
	}

	return vl_Error;
}
