/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   tx3g.c
* \brief   routines to run WCDMA transceiver
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "rfdef.h"
#include "tx3g.h"

#include "misc.h"
#include "isimsg.h"

C_HAL_SB_WCDMA_TX_CONTROL_STR v_tatrf_StartTx3GControl = {
	/* Subblock ID */
	C_HAL_SB_WCDMA_TX_CONTROL,
	/* Subblock length in bytes */
	SIZE_C_HAL_SB_WCDMA_TX_CONTROL_STR,
	/* By default, no band selected */
	INFO_NO_WCDMA,
	/* UARFCN (highest value are 200KHz per step,  lowest values have
	   xxxx100KHz offset)
	 */
	0,
	/* Power level */
	0,
	/* xxxxxxxx--------  AFC table selection
	   --------xxxxxxxx  Filler */
	0,
	/* Filler according to specification but used for "Maximum power
	 * limitation" */
	ACT_TX3G_MAX_POWER_LIMIT_OFF,
	/* manual AFC */
	.manualAfc = 0U,
	/* xxxxxxxx--------  DPDCH enabled
	   --------xxxxxxxx  ACK/NACK enabled
	 */
	MAKE16(1, 0),
	/* xxxxxxxx--------  CQI enabled
	   --------xxxxxxxx  UL modulation
	 */
	0,			/* CQI off, modulation QPSK */
	/* xxxxxxxx--------  E-DPDCH number
	   --------xxxxxxxx  E-DPDCH_TTI
	 */
	0,			/* E-DPDCH number: 0, TTI: 2 ms */
	/* xxxxxxxx--------  Beta c
	   --------xxxxxxxx  Beta d
	 */
	MAKE16(120, 225),
	/* Gain factor for HS-DPCCH.
	   Actual value is beta_hs/225
	   beta hs for both Ack/Nack and CQI defined in the same parameter
	 */
	5,
	/* Gain factor for E-DPCCH.
	   Actual value is beta_ec/225.
	 */
	5,
	/* Gain factor for E-DPDCH1.
	   Actual value is beta_ed1/225.
	 */
	0,
	/* Gain factor for E-DPDCH2.
	   Actual value is beta_ed2/225.
	 */
	0,
	/* Gain factor for E-DPDCH3.
	   Actual value is beta_ed3/225.
	 */
	8,
	/* Gain factor for E-DPDCH4.
	   Actual value is beta_ed4/225.
	 */
	8,
	/* Scrambling code */
	0,
	/* Used spreading factor for DPDCH */
	256,
	/* Used spreading factor for E-DPDCH */
	256,
	/* xxxxxxxx--------  HSDPA activation delay
	   --------xxxxxxxx  Filler
	 */
	0,
	/* Filler */
	0xAA55
};

int v_tatrf_StartTx3GState = ACT_TX3G_STOP;

/* Function tide to Start RX 3G */
/*********************************/
int DthRf_StartTx3G_exec(struct dth_element *elem)
{
	return tatl7_00StartTx3G_exec(elem);
}

int DthRf_StartTx3G_set(struct dth_element *elem, void *value)
{
	return tatl7_04StartTx3G_set(elem, value);
}

int DthRf_StartTx3G_Parm_get(struct dth_element *elem, void *value)
{
	return tatl7_02StartTx3G_Parm_get(elem, value);
}

int DthRf_StartTx3G_Parm_set(struct dth_element *elem, void *value)
{
	return tatl7_03StartTx3G_Parm_set(elem, value);
}

static int tatl7_startTx3G(void);

int tatl7_00StartTx3G_exec(struct dth_element *elem)
{
	/* elem->path gives the (sub-element) path */
	/* elem->user_data gives the user_data, e.g. an ID used in a
	 * switch/case */
	int vl_Error = TAT_ERROR_OFF;

	switch (elem->user_data) {
	case ACT_TX3G:
		if (v_tatrf_StartTx3GState == ACT_TX3G_STOP)
			vl_Error = tatl3_03StopRfActivity();
		else if (v_tatrf_StartTx3GState == ACT_TX3G_START)
			vl_Error = tatl7_startTx3G();
		else
			vl_Error = TAT_BAD_REQ;
		break;

	default:
		vl_Error = TAT_BAD_REQ;
	}

	return vl_Error;
}

int tatl7_03StartTx3G_Parm_set(struct dth_element *elem, void *Value)
{
	int vl_Error = TAT_ERROR_OFF;

	switch (elem->user_data) {
	case ACT_TX3G_FREQBAND:

		v_tatrf_StartTx3GControl.band_info =
		    tatl3_06GetWcdmaRfBand(DEREF_PTR(Value, u32));
		SYSLOG(LOG_DEBUG,
		       "set NO BAND(0), BAND1(1), BAND2(2), BAND3(3), BAND4(4), BAND5(5), BAND6(6), BAND7(7), BAND8(8): %u\n",
		       DEREF_PTR(Value, u32));
		SYSLOG(LOG_DEBUG, "set WCDMA band info to %lu\n",
		       v_tatrf_StartTx3GControl.band_info);
		break;

	case ACT_TX3G_UARFCN:
		v_tatrf_StartTx3GControl.ul_uarfcn = DEREF_PTR(Value, u16);
		break;

	case ACT_TX3G_PWLEVEL:

		/* warning: is coded as uint16 in C_HAL_SB_WCDMA_TX_CONTROL_STR
		 * but is really a signed attribute */
		DEREF_PTR_SET(&v_tatrf_StartTx3GControl.power_level, int16,
			      RF_Q(DEREF_PTR(Value, float), 8));
		SYSLOG(LOG_INFO, "Tx power level set to %lf (%u as u16)\n",
		       DEREF_PTR(Value, float),
		       v_tatrf_StartTx3GControl.power_level);
		break;

	case ACT_TX3G_AFC_TABLE:
		switch (DEREF_PTR(Value, u8)) {
		case 0:
			v_tatrf_StartTx3GControl.afc_table_fill1 =
			    SetU8InU16(MSB_POSS,
				       v_tatrf_StartTx3GControl.afc_table_fill1,
				       CTRL_AFC_TUNED);
			break;
		case 1:
			v_tatrf_StartTx3GControl.afc_table_fill1 =
			    SetU8InU16(MSB_POSS,
				       v_tatrf_StartTx3GControl.afc_table_fill1,
				       CTRL_AFC_FREQ_ABS_MANUAL);
			break;
		case 2:
			v_tatrf_StartTx3GControl.afc_table_fill1 =
			    SetU8InU16(MSB_POSS,
				       v_tatrf_StartTx3GControl.afc_table_fill1,
				       CTRL_AFC_FREQ_PPM_MANUAL);
			break;
		default:
			/* Invalid request code */
			vl_Error = TAT_BAD_REQ;
			break;
		}
		v_tatrf_StartTx3GControl.afc_table_fill1 = SetU8InU16(LSB_POSS,
			v_tatrf_StartTx3GControl.afc_table_fill1, ISIFILLER8);
		break;

	case ACT_TX3G_MAX_POWER_LIMIT:

		DTH_SET_UNSIGNED(elem, Value, v_tatrf_StartTx3GControl.fill2,
				 vl_Error);
		break;

	case ACT_TX3G_MANUAL_AFC:

		{
			u32 vl_afc = DEREF_PTR(Value, u32);
			if ((v_tatrf_StartTx3GControl.afc_table_fill1 >> 8) ==
			    CTRL_AFC_FREQ_PPM_MANUAL) {
				vl_afc *= 1024;
				SYSLOG(LOG_DEBUG,
				       "=> CTRL_AFC_FREQ_PPM_MANUAL:\n value Received: %d\n value Converted: %d\n",
				       DEREF_PTR(Value, u32), vl_afc);
			} else
			    if ((v_tatrf_StartTx3GControl.afc_table_fill1 >> 8)
				== CTRL_AFC_FREQ_ABS_MANUAL) {
				SYSLOG(LOG_DEBUG,
				       "=> CTRL_AFC_FREQ_ABS_MANUAL:\n value Received: %d\n value Converted: %d\n",
				       DEREF_PTR(Value, u32), vl_afc);
			} else
			    if ((v_tatrf_StartTx3GControl.afc_table_fill1 >> 8)
				== CTRL_AFC_TUNED) {
				vl_afc = 0;
				SYSLOG(LOG_DEBUG,
				       "=> CTRL_AFC_TUNED:\n value Received: %d\n",
				       DEREF_PTR(Value, u32));
			} else {
				SYSLOG(LOG_ERR,
				       "CTRL_AFC => ERROR: TAT_BAD_REQ\n");

				vl_afc = 0;
				vl_Error = TAT_BAD_REQ;
			}

			v_tatrf_StartTx3GControl.manualAfc = vl_afc;
		}

		break;

	case ACT_TX3G_DPDCH_ON:
		ASSERT(elem->type == DTH_TYPE_U32,
		       "unexpected type for dth element ACT_TX3G_DPDCH_ON");
		switch (DEREF_PTR(Value, u32)) {
		case RF_STARTTX3G_DPDCH_OFF:
			v_tatrf_StartTx3GControl.dpdch_enable_acknack_enable =
			    SetU8InU16(MSB_POSS,
				       v_tatrf_StartTx3GControl.dpdch_enable_acknack_enable,
				       0);
			break;
		case RF_STARTTX3G_DPDCH_ON:
			v_tatrf_StartTx3GControl.dpdch_enable_acknack_enable =
			    SetU8InU16(MSB_POSS,
				       v_tatrf_StartTx3GControl.dpdch_enable_acknack_enable,
				       1);
			break;

		default:
			SYSLOG(LOG_ERR, "Invalid DPDCH state");
			vl_Error = TAT_BAD_REQ;
		}
		break;

	case ACT_TX3G_ACKNACK_ON:
		ASSERT(elem->type == DTH_TYPE_U32,
		       "unexpected type for dth element ACT_TX3G_ACKNACK_ON");
		switch (DEREF_PTR(Value, u32)) {
		case RF_STARTTX3G_ACKNACK_OFF:
			v_tatrf_StartTx3GControl.dpdch_enable_acknack_enable =
			    SetU8InU16(LSB_POSS,
				       v_tatrf_StartTx3GControl.dpdch_enable_acknack_enable,
				       0);
			break;
		case RF_STARTTX3G_ACKNACK_ON:
			v_tatrf_StartTx3GControl.dpdch_enable_acknack_enable =
			    SetU8InU16(LSB_POSS,
				       v_tatrf_StartTx3GControl.dpdch_enable_acknack_enable,
				       1);
			break;

		default:
			SYSLOG(LOG_ERR, "Invalid ACK/NACK state");
			vl_Error = TAT_BAD_REQ;
		}
		break;

	case ACT_TX3G_CQI_ON:
		ASSERT(elem->type == DTH_TYPE_U32,
		       "unexpected type for dth element ACT_TX3G_CQI_ON");
		switch (DEREF_PTR(Value, u32)) {
		case RF_STARTTX3G_CQI_OFF:
			v_tatrf_StartTx3GControl.cqi_enable_ul_modulation =
			    SetU8InU16(MSB_POSS,
				       v_tatrf_StartTx3GControl.cqi_enable_ul_modulation,
				       0);
			break;
		case RF_STARTTX3G_CQI_ON:
			v_tatrf_StartTx3GControl.cqi_enable_ul_modulation =
			    SetU8InU16(MSB_POSS,
				       v_tatrf_StartTx3GControl.cqi_enable_ul_modulation,
				       1);
			break;

		default:
			SYSLOG(LOG_ERR, "Invalid CQI state");
			vl_Error = TAT_BAD_REQ;
		}
		break;

	case ACT_TX3G_MODULATION:
		ASSERT(elem->type == DTH_TYPE_U32,
		       "unexpected type for dth element ACT_TX3G_MODULATION");
		/* note: QPSK/16QAM constant values should be extern! */
		switch (DEREF_PTR(Value, u32)) {
		case RF_STARTTX3G_QPSK:
			v_tatrf_StartTx3GControl.cqi_enable_ul_modulation =
			    SetU8InU16(LSB_POSS,
				       v_tatrf_StartTx3GControl.cqi_enable_ul_modulation,
				       0);
			break;

		case RF_STARTTX3G_16QAM:
			v_tatrf_StartTx3GControl.cqi_enable_ul_modulation =
			    SetU8InU16(LSB_POSS,
				       v_tatrf_StartTx3GControl.cqi_enable_ul_modulation,
				       1);
			break;

		default:
			SYSLOG(LOG_ERR, "Invalid modulation option");
			vl_Error = TAT_BAD_REQ;
		}
		break;

	case ACT_TX3G_E_DPDCH_NUMBER:
		ASSERT(elem->type == DTH_TYPE_U8,
		       "unexpected type for dth element ACT_TX3G_E_DPDCH_NUMBER");
		v_tatrf_StartTx3GControl.num_edpdch_edpdch_tti =
		    SetU8InU16(MSB_POSS,
			       v_tatrf_StartTx3GControl.num_edpdch_edpdch_tti,
			       DEREF_PTR(Value, u8));
		break;

	case ACT_TX3G_E_DPDCH_TTI:
		ASSERT(elem->type == DTH_TYPE_U8,
		       "unexpected type for dth element ACT_TX3G_E_DPDCH_TTI");
		/* note: DPDCH TTI constant values should be extern! */
		switch (DEREF_PTR(Value, u8)) {
		case RF_STARTTX3G_DPDCH_TTI_2MS:
			v_tatrf_StartTx3GControl.num_edpdch_edpdch_tti =
			    SetU8InU16(LSB_POSS,
				       v_tatrf_StartTx3GControl.num_edpdch_edpdch_tti,
				       0);
			break;

		case RF_STARTTX3G_DPDCH_TTI_10MS:
			v_tatrf_StartTx3GControl.num_edpdch_edpdch_tti =
			    SetU8InU16(LSB_POSS,
				       v_tatrf_StartTx3GControl.num_edpdch_edpdch_tti,
				       1);
			break;

		default:
			SYSLOG(LOG_ERR, "Invalid DPDCH TTI");
			vl_Error = TAT_BAD_REQ;
		}
		break;

		/* Gain factor for DPCCH. Actual value is beta_c/225. */
	case ACT_TX3G_BETA_C:
		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TX3G_BETA_C");
		v_tatrf_StartTx3GControl.beta_c_beta_d =
		    SetU8InU16(MSB_POSS, v_tatrf_StartTx3GControl.beta_c_beta_d,
			       DEREF_PTR(Value, u8));
		break;

		/* Gain factor for DPDCH. Actual value is beta_d/225. */
	case ACT_TX3G_BETA_D:
		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TX3G_BETA_D");
		v_tatrf_StartTx3GControl.beta_c_beta_d =
		    SetU8InU16(LSB_POSS, v_tatrf_StartTx3GControl.beta_c_beta_d,
			       DEREF_PTR(Value, u8));
		break;

		/* Gain factor for HS-DPCCH. Actual value is beta_hs/225.
		   beta hs for both Ack/Nack and CQI defined in the same parameter */
	case ACT_TX3G_BETA_HS:
		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TX3G_BETA_HS");
		v_tatrf_StartTx3GControl.beta_hs = DEREF_PTR(Value, u16);
		break;

		/* Gain factor for E-DPCCH. Actual value is beta_ec/225 */
	case ACT_TX3G_BETA_EC:
		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TX3G_BETA_EC");
		v_tatrf_StartTx3GControl.beta_ec = DEREF_PTR(Value, u16);
		break;

		/* Gain factor for E-DPDCH1. Actual value is beta_ed1/225 */
	case ACT_TX3G_BETA_ED1:
		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TX3G_BETA_ED1");
		v_tatrf_StartTx3GControl.beta_ed1 = DEREF_PTR(Value, u16);
		break;

		/* Gain factor for E-DPDCH2. Actual value is beta_ed2/225 */
	case ACT_TX3G_BETA_ED2:
		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TX3G_BETA_ED2");
		v_tatrf_StartTx3GControl.beta_ed2 = DEREF_PTR(Value, u16);
		break;

		/* Gain factor for E-DPDCH3. Actual value is beta_ed3/225 */
	case ACT_TX3G_BETA_ED3:
		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TX3G_BETA_ED3");
		v_tatrf_StartTx3GControl.beta_ed3 = DEREF_PTR(Value, u16);
		break;

		/* Gain factor for E-DPDCH4. Actual value is beta_ed4/225 */
	case ACT_TX3G_BETA_ED4:
		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TX3G_BETA_ED4");
		v_tatrf_StartTx3GControl.beta_ed4 = DEREF_PTR(Value, u16);
		break;

	case ACT_TX3G_SCRAMBLING_CODE:
		ASSERT(elem->type == DTH_TYPE_U32,
		       "unexpected type for dth element ACT_TX3G_SCRAMBLING_CODE");
		v_tatrf_StartTx3GControl.scrambling_code =
		    DEREF_PTR(Value, u32);
		break;

	case ACT_TX3G_DPDCH_SPREAD_FACTOR:
		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TX3G_DPDCH_SPREAD_FACTOR");
		v_tatrf_StartTx3GControl.dpdch_sf = DEREF_PTR(Value, u16);
		break;

	case ACT_TX3G_EDPDCH_SPREAD_FACTOR:
		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TX3G_EDPDCH_SPREAD_FACTOR");
		v_tatrf_StartTx3GControl.edpdch_sf = DEREF_PTR(Value, u16);
		break;

	case ACT_TX3G_HSDPA_ACTIVATION_DELAY:
		ASSERT(elem->type == DTH_TYPE_U8,
		       "unexpected type for dth element ACT_TX3G_HSDPA_ACTIVATION_DELAY");
		v_tatrf_StartTx3GControl.hsdpa_delay_fill3 =
		    MAKE16(DEREF_PTR(Value, u8), 0xAA);
		break;

	default:
		/* Invalid request code */
		vl_Error = TAT_BAD_REQ;
		break;
	}

	return vl_Error;
}

int tatl7_02StartTx3G_Parm_get(struct dth_element *elem, void *value)
{
	int vl_Error = TAT_ERROR_OFF;

	switch (elem->user_data) {

	case ACT_TX3G_MAX_POWER_LIMIT:

		DTH_GET_UNSIGNED(v_tatrf_StartTx3GControl.fill2, elem, value,
				 vl_Error);
		break;

	case ACT_TX3G_DPDCH_ON:
		ASSERT(elem->type == DTH_TYPE_U32,
		       "unexpected type for dth element ACT_TX3G_DPDCH_ON");
		switch (HIGHBYTE
			(v_tatrf_StartTx3GControl.dpdch_enable_acknack_enable)) {
		case 0:
			DEREF_PTR_SET(value, u32, RF_STARTTX3G_DPDCH_OFF);
			break;

		case 1:
			DEREF_PTR_SET(value, u32, RF_STARTTX3G_DPDCH_ON);
			break;

		default:
			SYSLOG(LOG_ERR, "Invalid DPDCH state");
			vl_Error = TAT_BAD_REQ;
		}
		break;

	case ACT_TX3G_ACKNACK_ON:
		ASSERT(elem->type == DTH_TYPE_U32,
		       "unexpected type for dth element ACT_TX3G_ACKNACK_ON");
		switch (LOWBYTE
			(v_tatrf_StartTx3GControl.dpdch_enable_acknack_enable))	{
		case 0:
			DEREF_PTR_SET(value, u32, RF_STARTTX3G_ACKNACK_OFF);
			break;

		case 1:
			DEREF_PTR_SET(value, u32, RF_STARTTX3G_ACKNACK_ON);
			break;

		default:
			SYSLOG(LOG_ERR, "Invalid ACK/NACK state");
			vl_Error = TAT_BAD_REQ;
		}
		break;

	case ACT_TX3G_CQI_ON:
		ASSERT(elem->type == DTH_TYPE_U32,
		       "unexpected type for dth element ACT_TX3G_CQI_ON");
		switch (HIGHBYTE
			(v_tatrf_StartTx3GControl.cqi_enable_ul_modulation)) {
		case 0:
			DEREF_PTR_SET(value, u32, RF_STARTTX3G_CQI_OFF);
			break;

		case 1:
			DEREF_PTR_SET(value, u32, RF_STARTTX3G_CQI_ON);
			break;

		default:
			SYSLOG(LOG_ERR, "Invalid CQI state");
			vl_Error = TAT_BAD_REQ;
		}
		break;

	case ACT_TX3G_MODULATION:
		ASSERT(elem->type == DTH_TYPE_U32,
		       "unexpected type for dth element ACT_TX3G_MODULATION");
		/* note: QPSK/16QAM constant values should be extern! */
		switch (LOWBYTE
			(v_tatrf_StartTx3GControl.cqi_enable_ul_modulation)) {
		case 0:
			DEREF_PTR_SET(value, u32, RF_STARTTX3G_QPSK);
			break;

		case 1:
			DEREF_PTR_SET(value, u32, RF_STARTTX3G_16QAM);
			break;

		default:
			SYSLOG(LOG_ERR, "Invalid modulation option");
			vl_Error = TAT_BAD_REQ;
		}
		break;

	case ACT_TX3G_E_DPDCH_NUMBER:
		ASSERT(elem->type == DTH_TYPE_U8,
		       "unexpected type for dth element ACT_TX3G_E_DPDCH_NUMBER");
		DEREF_PTR_SET(value, u8,
			      HIGHBYTE
			      (v_tatrf_StartTx3GControl.num_edpdch_edpdch_tti));
		break;

	case ACT_TX3G_E_DPDCH_TTI:
		ASSERT(elem->type == DTH_TYPE_U8,
		       "unexpected type for dth element ACT_TX3G_E_DPDCH_TTI");
		/* note: DPDCH TTI constant values should be extern! */
		switch (LOWBYTE(v_tatrf_StartTx3GControl.num_edpdch_edpdch_tti)) {
		case 0:
			DEREF_PTR_SET(value, u8, RF_STARTTX3G_DPDCH_TTI_2MS);
			break;

		case 1:
			DEREF_PTR_SET(value, u8, RF_STARTTX3G_DPDCH_TTI_10MS);
			break;

		default:
			SYSLOG(LOG_ERR, "Invalid DPDCH TTI");
			vl_Error = TAT_BAD_REQ;
		}
		break;

		/* Gain factor for DPCCH. Actual value is beta_c/225. */
	case ACT_TX3G_BETA_C:
		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TX3G_BETA_C");
		DEREF_PTR_SET(value, u16,
			      HIGHBYTE(v_tatrf_StartTx3GControl.beta_c_beta_d));
		break;

		/* Gain factor for DPDCH. Actual value is beta_d/225. */
	case ACT_TX3G_BETA_D:
		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TX3G_BETA_D");
		DEREF_PTR_SET(value, u16,
			      LOWBYTE(v_tatrf_StartTx3GControl.beta_c_beta_d));
		break;

		/* Gain factor for HS-DPCCH. Actual value is beta_hs/225.
		   beta hs for both Ack/Nack and CQI defined in the same parameter */
	case ACT_TX3G_BETA_HS:
		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TX3G_BETA_HS");
		DEREF_PTR_SET(value, u16, v_tatrf_StartTx3GControl.beta_hs);
		break;

		/* Gain factor for E-DPCCH. Actual value is beta_ec/225 */
	case ACT_TX3G_BETA_EC:
		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TX3G_BETA_EC");
		DEREF_PTR_SET(value, u16, v_tatrf_StartTx3GControl.beta_ec);
		break;

		/* Gain factor for E-DPDCH1. Actual value is beta_ed1/225 */
	case ACT_TX3G_BETA_ED1:
		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TX3G_BETA_ED1");
		DEREF_PTR_SET(value, u16, v_tatrf_StartTx3GControl.beta_ed1);
		break;

		/* Gain factor for E-DPDCH2. Actual value is beta_ed2/225 */
	case ACT_TX3G_BETA_ED2:
		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TX3G_BETA_ED2");
		DEREF_PTR_SET(value, u16, v_tatrf_StartTx3GControl.beta_ed2);
		break;

		/* Gain factor for E-DPDCH3. Actual value is beta_ed3/225 */
	case ACT_TX3G_BETA_ED3:
		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TX3G_BETA_ED3");
		DEREF_PTR_SET(value, u16, v_tatrf_StartTx3GControl.beta_ed3);
		break;

		/* Gain factor for E-DPDCH4. Actual value is beta_ed4/225 */
	case ACT_TX3G_BETA_ED4:
		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TX3G_BETA_ED4");
		DEREF_PTR_SET(value, u16, v_tatrf_StartTx3GControl.beta_ed4);
		break;

	case ACT_TX3G_SCRAMBLING_CODE:
		ASSERT(elem->type == DTH_TYPE_U32,
		       "unexpected type for dth element ACT_TX3G_SCRAMBLING_CODE");
		DEREF_PTR_SET(value, u32,
			      v_tatrf_StartTx3GControl.scrambling_code);
		break;

	case ACT_TX3G_DPDCH_SPREAD_FACTOR:
		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TX3G_DPDCH_SPREAD_FACTOR");
		DEREF_PTR_SET(value, u16, v_tatrf_StartTx3GControl.dpdch_sf);
		break;

	case ACT_TX3G_EDPDCH_SPREAD_FACTOR:
		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TX3G_EDPDCH_SPREAD_FACTOR");
		DEREF_PTR_SET(value, u16, v_tatrf_StartTx3GControl.edpdch_sf);
		break;

	case ACT_TX3G_HSDPA_ACTIVATION_DELAY:
		ASSERT(elem->type == DTH_TYPE_U8,
		       "unexpected type for dth element ACT_TX3G_HSDPA_ACTIVATION_DELAY");
		DEREF_PTR_SET(value, u8,
			      HIGHBYTE
			      (v_tatrf_StartTx3GControl.hsdpa_delay_fill3));
		break;
	}

	return vl_Error;
}

int tatl7_04StartTx3G_set(struct dth_element *elem, void *value)
{
	int vl_Error = TAT_ERROR_OFF;

	if (ACT_TX3G == elem->user_data)
		v_tatrf_StartTx3GState = DEREF_PTR(value, u32);
	else
		vl_Error = TAT_BAD_REQ;

	return vl_Error;
}

/* Start TX 3G with the current settings */
int tatl7_startTx3G(void)
{
	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	C_HAL_RF_TEST_CONTROL_REQ_STR *vl_req;
	vl_req =
	    tatrf_isi_msg_begin(&vl_msg_info, PN_DSP_COMMON_TEST,
				C_HAL_RF_TEST_CONTROL_REQ, num_sb_id);
	if (NULL == vl_req)
		return TAT_ISI_HANDLER_ERROR;

	v_tatrf_StartTx3GControl.fill4 = 0xAA00;

	C_HAL_SB_WCDMA_TX_CONTROL_STR *pl_sb =
	    tatrf_isi_msg_add_sb_copy(&vl_msg_info, C_HAL_SB_WCDMA_TX_CONTROL,
				      &v_tatrf_StartTx3GControl,
				      sizeof(v_tatrf_StartTx3GControl));
	if (NULL == pl_sb)
		return TAT_ISI_HANDLER_ERROR;

	/* send ISI message - blocking function */
	int vl_Error = tatrf_isi_send(&vl_msg_info, "Start TX 3G");
	if (!TAT_OK(vl_Error))
		return vl_Error;

	/* message successfully sent. waiting for response */
	tatl17_06read(&vl_msg_info, C_HAL_RF_TEST_CONTROL_RESP, NULL,
		"Start TX 3G response", &vl_Error);

	return vl_Error;
}
