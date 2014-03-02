/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   rx3g.c
* \brief   routines to run WCDMA receiver
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "rfdef.h"
#include "rx3g.h"

#include "misc.h"
#include "isimsg.h"

/*Global variables */
C_HAL_SB_WCDMA_RX_CONTROL_STR v_tatrf_startRx3GControl = {
    .sb_id = C_HAL_SB_WCDMA_RX_CONTROL,
    .sb_len = SIZE_C_HAL_SB_WCDMA_RX_CONTROL_STR,
    .band_info = INFO_NO_WCDMA,
    .dl_uarfcn = 0U,
    .afc_table_fill1 = MAKE16(CTRL_AFC_TUNED, ISIFILLER8),
    .manualAfc = 0U,
    .agc_mode_fill2 = MAKE16(CTRL_RX_AGC_MANUAL, ISIFILLER8),
    .rx_level = 0
};

int v_tatrf_startRx3GState = ACT_RX3G_STOP;

/* Function tide to Start RX 3G */
/*********************************/
int DthRf_StartRx3G_exec(struct dth_element *elem)
{
	return tatl6_00StartRx3G_exec(elem);
}

int DthRf_StartRx3G_set(struct dth_element *elem, void *value)
{
	return tatl6_04StartRx3G_set(elem, value);
}

int DthRf_StartRx3G_Parm_get(struct dth_element *elem, void *value)
{
	return tatl6_02StartRx3G_Parm_get(elem, value);
}

int DthRf_StartRx3G_Parm_set(struct dth_element *elem, void *value)
{
	return tatl6_03StartRx3G_Parm_set(elem, value);
}

int tatl6_startRx3G(void);

int tatl6_00StartRx3G_exec(struct dth_element *elem)
{
	/* elem->path gives the (sub-element) path */
	/* elem->user_data gives the user_data, e.g. an ID used in a switch/case */
	int vl_Error = TAT_ERROR_CASE;

	switch (elem->user_data) {
	case ACT_RX3G:
		if (v_tatrf_startRx3GState == ACT_RX3G_STOP) {
			vl_Error = tatl3_03StopRfActivity();
		} else if (v_tatrf_startRx3GState == ACT_RX3G_START) {
			/* Stop before Start in order simplify the RF test */
			vl_Error = tatl3_03StopRfActivity();
			if (TAT_OK(vl_Error)) {
				vl_Error = tatl6_startRx3G();
			}
		} else {
			/* Invalid request code */
			vl_Error = TAT_BAD_REQ;
		}
		break;

	default:
		/* Invalid request code */
		vl_Error = TAT_BAD_REQ;
	}

	return vl_Error;
}

int tatl6_02StartRx3G_Parm_get(struct dth_element *elem, void *value)
{
    (void) elem, (void) value;

	return TAT_ERROR_OFF;
}

int tatl6_03StartRx3G_Parm_set(struct dth_element *elem, void *Value)
{
	int vl_Error = TAT_ERROR_OFF;
	u32 *vl_Value;
	vl_Value = (u32 *) Value;

	switch (elem->user_data) {
	case ACT_RX3G_FREQBAND:

		v_tatrf_startRx3GControl.band_info =
		    tatl3_06GetWcdmaRfBand(*vl_Value);
		SYSLOG(LOG_DEBUG,
		       "set NO BAND(0), BAND1(1), BAND2(2), BAND3(3), BAND4(4), BAND5(5), BAND6(6), BAND7(7), BAND8(8): %u\n",
		       *vl_Value);
		SYSLOG(LOG_DEBUG, "set WCDMA band info to %lu\n",
		       v_tatrf_startRx3GControl.band_info);
		break;

	case ACT_RX3G_UARFCN:
		v_tatrf_startRx3GControl.dl_uarfcn = (u16) (*vl_Value);
		break;

	case ACT_RX3G_AFC_TABLE:
		switch ((u8) (*vl_Value)) {
		case 0:
			v_tatrf_startRx3GControl.afc_table_fill1 =
			    SetU8InU16(MSB_POSS,
				       v_tatrf_startRx3GControl.afc_table_fill1,
				       CTRL_AFC_TUNED);
			break;
		case 1:
			v_tatrf_startRx3GControl.afc_table_fill1 =
			    SetU8InU16(MSB_POSS,
				       v_tatrf_startRx3GControl.afc_table_fill1,
				       CTRL_AFC_FREQ_ABS_MANUAL);
			break;
		case 2:
			v_tatrf_startRx3GControl.afc_table_fill1 =
			    SetU8InU16(MSB_POSS,
				       v_tatrf_startRx3GControl.afc_table_fill1,
				       CTRL_AFC_FREQ_PPM_MANUAL);
			break;
		default:
			/* Invalid request code */
			vl_Error = TAT_BAD_REQ;
			break;
		}
		v_tatrf_startRx3GControl.afc_table_fill1 = SetU8InU16(LSB_POSS,
								      v_tatrf_startRx3GControl.
								      afc_table_fill1,
								      ISIFILLER8);
		break;

	case ACT_RX3G_MANUAL_AFC:
		vl_Error = 0;
		if ((v_tatrf_startRx3GControl.afc_table_fill1 >> 8) ==
		    CTRL_AFC_FREQ_PPM_MANUAL) {
			u32 temp_value = (u32) (*vl_Value);
			(*vl_Value) = temp_value * 1024;
			SYSLOG(LOG_DEBUG,
			       "CTRL_AFC_FREQ_PPM_MANUAL:\n value Received: %d\n value Converted: %d\n\n",
			       temp_value, *vl_Value);
		} else if ((v_tatrf_startRx3GControl.afc_table_fill1 >> 8) ==
			   CTRL_AFC_FREQ_ABS_MANUAL) {
			u32 temp_value = (u32) (*vl_Value);
			SYSLOG(LOG_DEBUG,
			       "CTRL_AFC_FREQ_ABS_MANUAL:\n value Received: %d\n value Converted: %d\n\n",
			       temp_value, *vl_Value);
		} else if ((v_tatrf_startRx3GControl.afc_table_fill1 >> 8) ==
			   CTRL_AFC_TUNED) {
			(*vl_Value) = 0;
			u32 temp_value = 0;
			SYSLOG(LOG_DEBUG,
			       "CTRL_AFC_TUNED:\n value Received: %d\n value Converted: %d\n\n",
			       temp_value, *vl_Value);
		} else {
			SYSLOG(LOG_ERR, "CTRL_AFC => ERROR: TAT_BAD_REQ\n");
			(*vl_Value) = 0;
			vl_Error = TAT_BAD_REQ;
		}

		v_tatrf_startRx3GControl.manualAfc = (u32) (*vl_Value);
		break;

	case ACT_RX3G_AGCMODE:
		switch ((u8) (*vl_Value)) {
		case 0:
			v_tatrf_startRx3GControl.agc_mode_fill2 =
			    SetU8InU16(MSB_POSS,
				       v_tatrf_startRx3GControl.agc_mode_fill2,
				       CTRL_RX_AGC_MANUAL);
			SYSLOG(LOG_DEBUG,
			       "Set RX3G AGCMODE -> CTRL_RX_AGC_MANUAL: v_tatrf_startRx3GControl.agc_mode_fill2 %d\n\n",
			       v_tatrf_startRx3GControl.agc_mode_fill2);
			break;
		case 1:
			v_tatrf_startRx3GControl.agc_mode_fill2 =
			    SetU8InU16(MSB_POSS,
				       v_tatrf_startRx3GControl.agc_mode_fill2,
				       CTRL_RX_AGC_AUTOMATIC);
			SYSLOG(LOG_DEBUG,
			       "Set RX3G AGCMODE -> CTRL_RX_AGC_AUTOMATIC: v_tatrf_startRx3GControl.agc_mode_fill2 %d\n\n",
			       v_tatrf_startRx3GControl.agc_mode_fill2);
			break;
		default:
			/* Invalid request code */
			SYSLOG(LOG_ERR,
			       "Set RX3G AGCMODE -> TAT_BAD_REQ: v_tatrf_startRx3GControl.agc_mode_fill2 %d\n\n",
			       *vl_Value);
			vl_Error = TAT_BAD_REQ;
			break;
		}

		v_tatrf_startRx3GControl.agc_mode_fill2 = SetU8InU16(LSB_POSS,
								     v_tatrf_startRx3GControl.
								     agc_mode_fill2,
								     ISIFILLER8);
		SYSLOG(LOG_DEBUG,
		       "Set RX3G AGCMODE : v_tatrf_startRx3GControl.agc_mode_fill2 %d\n\n",
		       v_tatrf_startRx3GControl.agc_mode_fill2);
		break;

	case ACT_RX3G_PATH:
		{
			u8 vl_agc_mode =
			    GetU8InU16(MSB_POSS,
				       v_tatrf_startRx3GControl.agc_mode_fill2);
			switch (*((u8 *) (vl_Value))) {
			case ACT_RX3G_PATH1:
				/* RF HAL handles 0x00 for MAIN but we SHOULD respect the ISI 
				   specification which expects we pass the MAIN_ENABLED(0x04) flag */
				v_tatrf_startRx3GControl.agc_mode_fill2 =
				    SetU8InU16(MSB_POSS,
					       v_tatrf_startRx3GControl.
					       agc_mode_fill2,
					       vl_agc_mode +
					       CTRL_RX_AGC_MAIN_PATH_ENABLED);
				break;

			case ACT_RX3G_PATH2:
				/* for diveristy both MAIN and DIVERSITY flags must be set */
				v_tatrf_startRx3GControl.agc_mode_fill2 =
				    SetU8InU16(MSB_POSS,
					       v_tatrf_startRx3GControl.
					       agc_mode_fill2,
					       vl_agc_mode +
					       CTRL_RX_AGC_MAIN_PATH_ENABLED +
					       CTRL_RX_AGC_DIVERSITY_PATH_ENABLED);
				break;

			default:
				vl_Error = TAT_BAD_REQ;
			}
		}

	case ACT_RX3G_RX_LEVEL:
		v_tatrf_startRx3GControl.rx_level = (u16) ((*vl_Value) * 256);
		break;

	default:
		/* Invalid request code */
		vl_Error = TAT_BAD_REQ;
	}

	return vl_Error;
}

int tatl6_04StartRx3G_set(struct dth_element *elem, void *value)
{
	int vl_Error = TAT_ERROR_OFF;
	u32 *vl_Value;
	vl_Value = (u32 *) value;

    (void) elem;

	switch (*vl_Value) {
	case 0:
		v_tatrf_startRx3GState = ACT_RX3G_START;
		SYSLOG(LOG_DEBUG,
		       "TATLRF => Set RX3G Start: v_tatrf_startRx3GState %d\n\n",
		       v_tatrf_startRx3GState);
		break;

	case 1:
		v_tatrf_startRx3GState = ACT_RX3G_STOP;
		SYSLOG(LOG_DEBUG,
		       "Set RX3G Stop: v_tatrf_startRx3GState %d\n\n",
		       v_tatrf_startRx3GState);
		break;

	default:
		/* Invalid request code */
		SYSLOG(LOG_ERR, "Set RX3G error case: vl_Value %d\n\n",
		       *vl_Value);
		vl_Error = TAT_BAD_REQ;
	}

	return vl_Error;
}

const C_HAL_SB_WCDMA_RX_CONTROL_STR *tatl1_06GetRx3GInfo(void)
{
	const C_HAL_SB_WCDMA_RX_CONTROL_STR *pl_data = NULL;
	if (ACT_RX3G_START == v_tatrf_startRx3GState) {
		pl_data = &v_tatrf_startRx3GControl;
	}

	return pl_data;
}

int tatl6_startRx3G(void)
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

	C_HAL_SB_WCDMA_RX_CONTROL_STR *pl_sb =
	    tatrf_isi_msg_add_sb_copy(&vl_msg_info, C_HAL_SB_WCDMA_RX_CONTROL,
				      &v_tatrf_startRx3GControl,
				      sizeof(v_tatrf_startRx3GControl));
	if (NULL == pl_sb) {
		return TAT_ISI_HANDLER_ERROR;
	}

	/*Send ISI message - Blocking function */
	int vl_Error = tatrf_isi_send(&vl_msg_info, "Start RX 3G");
	if (!TAT_OK(vl_Error)) {
		return vl_Error;
	}

	/* message successfully sent. waiting for response */
	C_HAL_RF_TEST_CONTROL_RESP_STR *pl_resp =
	    tatrf_isi_read_simple(&vl_msg_info, C_HAL_RF_TEST_CONTROL_RESP,
				  "Start RX 3G response", &vl_Error);
	if (NULL != pl_resp) {
		/* this what we expected */
	}

	return vl_Error;
}

