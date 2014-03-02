/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   rx2g.c
* \brief   routines to run GSM/EDGE receiver
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "rfdef.h"
#include "rx2g.h"

#include "misc.h"
#include "isimsg.h"


/* GLOBALS */

/* variable filled with RX request arguments */
C_HAL_SB_GSM_RX_CONTROL_STR v_tatrf_StartRxControl;

/* operation: Start or Stop */
int v_tatrf_StartRxState;

/* operation status */
int v_tatrf_StartRxStatusValue;


/* Function tide to Start RX 2G5 */
/*********************************/
int DthRf_StartRx2G5_exec(struct dth_element *elem)
{
	return tatl1_00StartRx2G5_exec(elem);
}

int DthRf_StartRx2G5Parm_get(struct dth_element *elem, void *value)
{
    (void) elem, (void)value;

	return TAT_BAD_REQ;
}

int DthRf_StartRx2G5Parm_set(struct dth_element *elem, void *value)
{
	return tatl1_03StartRx2G5Parm_set(elem, value);
}

int DthRf_StartRx2G5_set(struct dth_element *elem, void *value)
{
	return tatl1_04StartRx2G5_set(elem, value);
}

int tatl1_startRx2G(void);

int tatl1_00StartRx2G5_exec(struct dth_element *elem)
{
	int vl_Error = 0;

	switch (elem->user_data) {
	case ACT_RX2G5:	/* use stored arguments values to execute action */

		switch (v_tatrf_StartRxState) {
		case ACT_RX2G5_STOP:

			vl_Error = tatl3_03StopRfActivity();
			break;

		case ACT_RX2G5_START:

			/* Stop before Start in order simplify the RF test */
			vl_Error = tatl3_03StopRfActivity();
			if (TAT_OK(vl_Error)) {
				vl_Error = tatl1_startRx2G();
			}
			break;

		default:

			v_tatrf_StartRxStatusValue = TAT_ERROR_CASE;
			vl_Error = TAT_BAD_REQ;
		}

		break;

	default:
		/* Invalid request code */
		v_tatrf_StartRxStatusValue = TAT_ERROR_CASE;
		vl_Error = TAT_BAD_REQ;
	}

	return vl_Error;
}

int tatl1_03StartRx2G5Parm_set(struct dth_element *elem, void *Value)
{
	int vl_Error = TAT_ERROR_OFF;
	u32 *vl_Value;
	vl_Value = (u32 *) Value;

	switch (elem->user_data) {
	case ACT_RX2G5_FREQBAND:

		v_tatrf_StartRxControl.gsm_band_info =
		    tatl3_04GetGsmRfBand(*vl_Value);
		SYSLOG(LOG_DEBUG,
		       "tatlrf: set NO BAND(0), GSM850(1), GSM900(2), DCS1800(3), PCS1900(4): %u\n",
		       (u8) (*vl_Value));
		SYSLOG(LOG_DEBUG, "tatlrf: set GSM band info to %lu\n",
		       v_tatrf_StartRxControl.gsm_band_info);
		break;

	case ACT_RX2G5_CHANNEL:

		v_tatrf_StartRxControl.rx_channel = (u16) (*vl_Value);
		SYSLOG(LOG_DEBUG, "tatlrf: Set CHANNEL parameter: %d\n",
		       (u16) (*vl_Value));
		break;

	case ACT_RX2G5_MONITOR:

		v_tatrf_StartRxControl.mon_channel = (u16) (*vl_Value);
		SYSLOG(LOG_DEBUG, "tatlrf: Set MONITOR parameter: %d\n",
		       (u16) (*vl_Value));
		break;

	case ACT_RX2G5_OPMODE:

		v_tatrf_StartRxControl.rx_slot_mask_afc_table =
		    SetU8InU16(MSB_POSS,
			       v_tatrf_StartRxControl.rx_slot_mask_afc_table,
			       (u8) (*vl_Value));
		SYSLOG(LOG_DEBUG, "tatlrf: Set OPERATION MODE parameter: %d\n",
		       (u16) (*vl_Value));
		break;

	case ACT_RX2G5_AFCTAB:
		{
			switch ((u8) (*vl_Value)) {
			case 0:

				v_tatrf_StartRxControl.rx_slot_mask_afc_table =
				    SetU8InU16(LSB_POSS,
					       v_tatrf_StartRxControl.
					       rx_slot_mask_afc_table,
					       CTRL_AFC_TUNED);
				break;

			case 1:

				v_tatrf_StartRxControl.rx_slot_mask_afc_table =
				    SetU8InU16(LSB_POSS,
					       v_tatrf_StartRxControl.
					       rx_slot_mask_afc_table,
					       CTRL_AFC_FREQ_ABS_MANUAL);
				break;

			case 2:

				v_tatrf_StartRxControl.rx_slot_mask_afc_table =
				    SetU8InU16(LSB_POSS,
					       v_tatrf_StartRxControl.
					       rx_slot_mask_afc_table,
					       CTRL_AFC_FREQ_PPM_MANUAL);
				break;

			default:

				/* Invalid request code */
				vl_Error = TAT_BAD_REQ;
			}
			SYSLOG(LOG_DEBUG,
			       "tatlrf: Set AFC TABLE parameter TUNED(0) ABS_MANUAL(1) PMM_MANUAL(2): %d\n",
			       (u8) (*vl_Value));
		}
		break;

	case ACT_RX2G5_AFCVAL:

		vl_Error = TAT_ERROR_OFF;
		if ((v_tatrf_StartRxControl.rx_slot_mask_afc_table & 0x00FF) ==
		    CTRL_AFC_FREQ_PPM_MANUAL) {
			u32 temp_value = (u32) (*vl_Value);
			(*vl_Value) = temp_value * 1024;
			SYSLOG(LOG_DEBUG,
			       "TATLRF => CTRL_AFC_FREQ_PPM_MANUAL:\n value Received: %d\n value Converted: %d\n\n",
			       temp_value, *vl_Value);
		} else
		    if ((v_tatrf_StartRxControl.
			 rx_slot_mask_afc_table & 0x00FF) ==
			CTRL_AFC_FREQ_ABS_MANUAL) {
			u32 temp_value = (u32) (*vl_Value);
			SYSLOG(LOG_DEBUG,
			       "TATLRF => CTRL_AFC_FREQ_ABS_MANUAL:\n value Received: %d\n value Converted: %d\n\n",
			       temp_value, *vl_Value);
		} else
		    if ((v_tatrf_StartRxControl.
			 rx_slot_mask_afc_table & 0x00FF) == CTRL_AFC_TUNED) {
			(*vl_Value) = 0;
			SYSLOG(LOG_DEBUG,
			       "TATLRF => CTRL_AFC_TUNED:\n value Received: %d\n value Converted: %d\n\n",
			       0, *vl_Value);
		} else {
			SYSLOG(LOG_ERR,
			       "TATLRF CTRL_AFC => ERROR:  TAT_BAD_REQ\n \n");
			(*vl_Value) = 0;
			vl_Error = TAT_BAD_REQ;
		}

		v_tatrf_StartRxControl.manualAfc = (u32) (*vl_Value);
		SYSLOG(LOG_DEBUG, "tatlrf: Set AFC VALUE parameter: %d\n",
		       (u32) (*vl_Value));
		break;

	case ACT_RX2G5_AGCMODE:
		{
			switch ((u8) (*vl_Value)) {
			case 0:

				v_tatrf_StartRxControl.agc_mode_fill2 =
				    SetU8InU16(MSB_POSS,
					       v_tatrf_StartRxControl.
					       agc_mode_fill2,
					       CTRL_RX_AGC_MANUAL);
				break;

			case 1:

				v_tatrf_StartRxControl.agc_mode_fill2 =
				    SetU8InU16(MSB_POSS,
					       v_tatrf_StartRxControl.
					       agc_mode_fill2,
					       CTRL_RX_AGC_AUTOMATIC);
				break;

			default:

				/* Invalid request code */
				vl_Error = TAT_BAD_REQ;
			}

			v_tatrf_StartRxControl.agc_mode_fill2 =
			    SetU8InU16(LSB_POSS,
				       v_tatrf_StartRxControl.agc_mode_fill2,
				       ISIFILLER8);
			SYSLOG(LOG_DEBUG,
			       "tatlrf: Set AGC MODE parameter AUTOMATIC(0) MANUAL(1): %d\n",
			       (u8) (*vl_Value));
		}
		break;

	case ACT_RX2G5_EXPECTPW:
		v_tatrf_StartRxControl.rx_level = (s16) (*vl_Value) * 256;
		SYSLOG(LOG_DEBUG,
		       "tatlrf: Set EXPECT POWER parameter: %d\nQ8 value: %d\n",
		       (s16) (*vl_Value), v_tatrf_StartRxControl.rx_level);
		break;

	default:

		/* Invalid request code */
		vl_Error = TAT_BAD_REQ;
	}

	return vl_Error;
}

int tatl1_04StartRx2G5_set(struct dth_element *elem, void *value)
{
	int vl_Error = TAT_ERROR_OFF;
	u32 *vl_Value = (u32 *) value;

    switch (elem->user_data)
    {
    case ACT_RX2G5:

	    v_tatrf_StartRxState = *vl_Value;
		break;

    default:
	    vl_Error = TAT_BAD_REQ;
	}

	return vl_Error;
}

const C_HAL_SB_GSM_RX_CONTROL_STR *tatl1_06GetRx2GInfo(void)
{
	const C_HAL_SB_GSM_RX_CONTROL_STR *pl_data = NULL;
	if (ACT_RX2G5_START == v_tatrf_StartRxState) {
		pl_data = &v_tatrf_StartRxControl;
	}

	return pl_data;
}

int tatl1_startRx2G(void)
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

	C_HAL_SB_GSM_RX_CONTROL_STR *pl_sb =
	    tatrf_isi_msg_add_sb_copy(&vl_msg_info, C_HAL_SB_GSM_RX_CONTROL,
				      &v_tatrf_StartRxControl,
				      sizeof(v_tatrf_StartRxControl));
	if (NULL == pl_sb) {
		return TAT_ISI_HANDLER_ERROR;
	}

	/* send ISI message - blocking function */
	int vl_Error = tatrf_isi_send(&vl_msg_info, "Start RX 2G");
	if (!TAT_OK(vl_Error)) {
		return vl_Error;
	}

	/* message successfully sent. waiting for response */
	C_HAL_RF_TEST_CONTROL_RESP_STR *pl_resp =
	    tatrf_isi_read_simple(&vl_msg_info, C_HAL_RF_TEST_CONTROL_RESP,
				  "Start RX 2G response", &vl_Error);
	if (NULL != pl_resp) {
		/* this what we expected */
		v_tatrf_StartRxStatusValue = pl_resp->status;
	}

	return vl_Error;
}

