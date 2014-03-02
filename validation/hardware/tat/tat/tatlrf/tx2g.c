/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   isimsg.c
* \brief   routines to run GSM/EDGE transceiver
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "rfdef.h"
#include "tx2g.h"

#include "misc.h"
#include "isimsg.h"


/* GLOBALS */

/* variable filled with TX request arguments */
C_HAL_SB_GSM_TX_CONTROL_STR v_tatrf_StartTxControl;
/* Operation (Start or Stop) */
int v_tatrf_StartTxState;
/* Operation status */
int v_tatrf_StartTxStatusValue;


/* Function tide to Start RX 2G5 */
/*********************************/
int DthRf_StartTx2G5_exec(struct dth_element *elem)
{
	return tatl2_00StartTx2G5_exec(elem);
}

int DthRf_StartTx2G5_set(struct dth_element *elem, void *value)
{
	return tatl2_04StartTx2G5_set(elem, value);
}

int DthRf_StartTx2G5Parm_get(struct dth_element *elem, void *value)
{
	return tatl2_02StartTx2G5Parm_get(elem, value);
}

int DthRf_StartTx2G5Parm_set(struct dth_element *elem, void *value)
{
	return tatl2_03StartTx2G5Parm_set(elem, value);
}

int tatl2_startTx2G(void);

int tatl2_00StartTx2G5_exec(struct dth_element *elem)
{
	int vl_Error = TAT_ERROR_CASE;

	switch (elem->user_data) {
	case ACT_TX2G5:

		if (v_tatrf_StartTxState == (ACT_TX2G5_STOP)) {	/* Stop Tx 2G5. */
			vl_Error = tatl3_03StopRfActivity();
		} else if (v_tatrf_StartTxState == (ACT_TX2G5_START)) {	/* Start TX2G5. */
			/* Stop before Start in order simplify the RF test */
			vl_Error = tatl3_03StopRfActivity();
			if (TAT_OK(vl_Error)) {
				vl_Error = tatl2_startTx2G();
			}
		} else {
			/* Invalid request code */
			v_tatrf_StartTxStatusValue = TAT_ERROR_CASE;
			vl_Error = TAT_BAD_REQ;
		}
		break;

	default:

		/* Invalid request code */
		v_tatrf_StartTxStatusValue = TAT_ERROR_CASE;
		vl_Error = TAT_BAD_REQ;
	}

	return vl_Error;
}

int tatl2_03StartTx2G5Parm_set(struct dth_element *elem, void *Value)
{
	int vl_Error = TAT_ERROR_OFF;
	u32 *pl_value = (u32 *) Value;

	switch (elem->user_data) {
		/* Set Band Parameter */
	case ACT_TX2G5_FREQBAND:
		{
			u8 vl_rfBand = (u8) (*pl_value);
			v_tatrf_StartTxControl.gsm_band_info =
			    tatl3_04GetGsmRfBand(vl_rfBand);
			SYSLOG(LOG_DEBUG,
			       "set NO BAND(0), GSM850(1), GSM900(2), DCS1800(3), PCS1900(4): %u\n",
			       (u8) (*pl_value));

			if (INFO_NO_GSM != v_tatrf_StartTxControl.gsm_band_info) {
				SYSLOG(LOG_DEBUG, "set GSM band info to %lu\n",
				       v_tatrf_StartTxControl.gsm_band_info);
			} else {
				vl_Error = TAT_BAD_REQ;
			}
		}
		break;

		/* Set Channel Parameter */
	case ACT_TX2G5_CHANNEL:
		v_tatrf_StartTxControl.tx_channel = (u16) (*pl_value);
		SYSLOG(LOG_DEBUG, "Set CHANNEL parameter: %d\n",
		       (u16) (*pl_value));
		break;

		/* Set Operation mode Parameter */
	case ACT_TX2G5_OPMODE:
		v_tatrf_StartTxControl.tx_slot_mask_afc_table =
		    SetU8InU16(MSB_POSS,
			       v_tatrf_StartTxControl.tx_slot_mask_afc_table,
			       (u8) (*pl_value));
		SYSLOG(LOG_DEBUG, "Set OP MODE parameter: %d\n",
		       (u8) (*pl_value));
		break;

		/* Set AFC table Parameter */
	case ACT_TX2G5_AFCTAB:
		{
			switch ((u8) (*pl_value)) {
			case 0:
				v_tatrf_StartTxControl.tx_slot_mask_afc_table =
				    SetU8InU16(LSB_POSS,
					       v_tatrf_StartTxControl.
					       tx_slot_mask_afc_table,
					       CTRL_AFC_TUNED);
				break;
			case 1:
				v_tatrf_StartTxControl.tx_slot_mask_afc_table =
				    SetU8InU16(LSB_POSS,
					       v_tatrf_StartTxControl.
					       tx_slot_mask_afc_table,
					       CTRL_AFC_FREQ_ABS_MANUAL);
				break;
			case 2:
				v_tatrf_StartTxControl.tx_slot_mask_afc_table =
				    SetU8InU16(LSB_POSS,
					       v_tatrf_StartTxControl.
					       tx_slot_mask_afc_table,
					       CTRL_AFC_FREQ_PPM_MANUAL);
				break;
			default:
				/* Invalid request code */
				vl_Error = TAT_BAD_REQ;
				break;
			}
#if defined (_DEBUG)
			if (vl_Error == TAT_ERROR_OFF) {
				SYSLOG(LOG_DEBUG,
				       "Set AFC TABLE parameter TUNED(0) ABS_MANUAL(1) PMM_MANUAL(2): %d\n",
				       (u8) (*pl_value));
			}
#endif
		}
		break;

	case ACT_TX2G5_AFCVAL:
		{
			vl_Error = 0;
			if ((v_tatrf_StartTxControl.
			     tx_slot_mask_afc_table & 0x00FF) ==
			    CTRL_AFC_FREQ_PPM_MANUAL) {
				u32 temp_value = (u32) (*pl_value);
				(*pl_value) = temp_value * 1024;
				SYSLOG(LOG_DEBUG,
				       "TATLRF => CTRL_AFC_FREQ_PPM_MANUAL:\n value Received: %d\n value Converted: %d\n\n",
				       temp_value, *pl_value);
			} else
			    if ((v_tatrf_StartTxControl.
				 tx_slot_mask_afc_table & 0x00FF) ==
				CTRL_AFC_FREQ_ABS_MANUAL) {
				u32 temp_value = (u32) (*pl_value);
				SYSLOG(LOG_DEBUG,
				       "TATLRF => CTRL_AFC_FREQ_ABS_MANUAL:\n value Received: %d\n value Converted: %d\n\n",
				       temp_value, *pl_value);
			} else
			    if ((v_tatrf_StartTxControl.
				 tx_slot_mask_afc_table & 0x00FF) ==
				CTRL_AFC_TUNED) {
				(*pl_value) = 0;
				u32 temp_value = 0;
				SYSLOG(LOG_DEBUG,
				       "TATLRF => CTRL_AFC_TUNED:\n value Received: %d\n value Converted: %d\n\n",
				       temp_value, *pl_value);
			} else {
				SYSLOG(LOG_ERR,
				       "TATLRF CTRL_AFC => ERROR: TAT_BAD_REQ\n \n");
				(*pl_value) = 0;
				vl_Error = TAT_BAD_REQ;
			}

			v_tatrf_StartTxControl.manualAfc = *pl_value;
			SYSLOG(LOG_DEBUG, "Set AFC VALUE parameter: %d\n",
			       *pl_value);
		}
		break;

	case ACT_TX2G5_MODETYPE:
		{
			v_tatrf_StartTxControl.tx_8psk_mask_data_type =
			    SetU8InU16(MSB_POSS,
				       v_tatrf_StartTxControl.
				       tx_8psk_mask_data_type,
				       (u8) (*pl_value));

			if (vl_Error == TAT_ERROR_OFF) {
				SYSLOG(LOG_DEBUG,
				       "Set MODE TYPE parameter GMSK(0) 8PSK(1..255): %d\n",
				       (u8) (*pl_value));
			}
		}
		break;

	case ACT_TX2G5_DATATYPE:
		{
			switch ((u8) (*pl_value)) {
			case 0:
				v_tatrf_StartTxControl.tx_8psk_mask_data_type =
				    SetU8InU16(LSB_POSS,
					       v_tatrf_StartTxControl.
					       tx_8psk_mask_data_type,
					       C_TEST_DATA_TYPE_0);
				break;
			case 1:
				v_tatrf_StartTxControl.tx_8psk_mask_data_type =
				    SetU8InU16(LSB_POSS,
					       v_tatrf_StartTxControl.
					       tx_8psk_mask_data_type,
					       C_TEST_DATA_TYPE_1);
				break;
			case 2:
				v_tatrf_StartTxControl.tx_8psk_mask_data_type =
				    SetU8InU16(LSB_POSS,
					       v_tatrf_StartTxControl.
					       tx_8psk_mask_data_type,
					       C_TEST_DATA_TYPE_RAND);
				break;
			case 3:
				v_tatrf_StartTxControl.tx_8psk_mask_data_type =
				    SetU8InU16(LSB_POSS,
					       v_tatrf_StartTxControl.
					       tx_8psk_mask_data_type,
					       C_TEST_DATA_TYPE_PN9);
				break;
			case 4:
				v_tatrf_StartTxControl.tx_8psk_mask_data_type =
				    SetU8InU16(LSB_POSS,
					       v_tatrf_StartTxControl.
					       tx_8psk_mask_data_type,
					       C_TEST_DATA_TYPE_DUMMY);
				break;
			default:
				/* Invalid request code */
				vl_Error = TAT_BAD_REQ;
				break;
			}

			if (vl_Error == TAT_ERROR_OFF) {
				SYSLOG(LOG_DEBUG,
				       "Set DATA TYPE parameter 0(0), 1(1), RAND(2), PN9(3), DUMMY(4): %d\n",
				       (u8) (*pl_value));
			}
		}
		break;

	case ACT_TX2G5_PWLEVELUNIT:
		{
			switch ((u8) (*pl_value)) {
			case 0:
				if (v_tatrf_StartTxControl.
				    tx_rach_mask_tx_power_unit == CTRL_TX_DBM) {
					v_tatrf_StartTxControl.power_level =
					    (v_tatrf_StartTxControl.
					     power_level) / 256;
				}

				v_tatrf_StartTxControl.
				    tx_rach_mask_tx_power_unit =
				    SetU8InU16(LSB_POSS,
					       v_tatrf_StartTxControl.
					       tx_rach_mask_tx_power_unit,
					       CTRL_TX_PCL);
				break;

			case 1:

				if (v_tatrf_StartTxControl.
				    tx_rach_mask_tx_power_unit == CTRL_TX_PCL) {
					v_tatrf_StartTxControl.power_level =
					    (v_tatrf_StartTxControl.
					     power_level) * 256;
				}
				v_tatrf_StartTxControl.
				    tx_rach_mask_tx_power_unit =
				    SetU8InU16(LSB_POSS,
					       v_tatrf_StartTxControl.
					       tx_rach_mask_tx_power_unit,
					       CTRL_TX_DBM);
				break;

			default:
				/* Invalid request code */
				vl_Error = TAT_BAD_REQ;
				break;
			}

			v_tatrf_StartTxControl.tx_rach_mask_tx_power_unit =
			    SetU8InU16(MSB_POSS,
				       v_tatrf_StartTxControl.
				       tx_rach_mask_tx_power_unit, 0x00);

			if (vl_Error == TAT_ERROR_OFF) {
				SYSLOG(LOG_DEBUG,
				       "Set POWER LEVEL UNIT parameter PCL(0), DBM(1): %d\n",
				       (u8) (*pl_value));
			}
		}
		break;

	case ACT_TX2G5_PWLEVEL:
		{
			if (v_tatrf_StartTxControl.tx_rach_mask_tx_power_unit ==
			    CTRL_TX_DBM) {
				v_tatrf_StartTxControl.power_level =
				    (s16) (*pl_value) * 256;
			} else {
				v_tatrf_StartTxControl.power_level =
				    (s16) (*pl_value);
			}

			SYSLOG(LOG_DEBUG, "Set POWER LEVEL parameter: %d\n",
			       (s16) (*pl_value));
		}
		break;

	default:

		/* Invalid request code */
		vl_Error = TAT_BAD_REQ;
	}

	return vl_Error;
}

int tatl2_02StartTx2G5Parm_get(struct dth_element *elem, void *value)
{
    (void) elem, (void) value;

	return TAT_ERROR_CASE;
}

int tatl2_04StartTx2G5_set(struct dth_element *elem, void *value)
{
	int vl_Error = TAT_ERROR_OFF;
	u32 *pl_Value = (u32 *) value;

    (void) elem;

	SYSLOG(LOG_DEBUG, "Try to set TX2G5 operation. \n");

	switch (*pl_Value) {
		/* set "Execution" argument value = store it for next execution */
	case 0:		/* Start TX2G5 */

		v_tatrf_StartTxState = ACT_TX2G5_START;
		SYSLOG(LOG_DEBUG, "Operation set => Start TX2G5. \n");
		break;

	case 1:		/* Stop TX3G5 */

		v_tatrf_StartTxState = ACT_TX2G5_STOP;
		SYSLOG(LOG_DEBUG, "Operation set => Stop TX2G5. \n");
		break;

	default:		/* error */

		/* Invalid request code */
		vl_Error = TAT_BAD_REQ;
		SYSLOG(LOG_ERR,
		       "Operation set => Neither StartTX2G5 nor StopTx2G5 \n");
	}

	return (vl_Error);
}

int tatl2_startTx2G(void)
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

	/*Set sub block part of ISI message. */
	C_HAL_SB_GSM_TX_CONTROL_STR *pl_sb =
	    tatrf_isi_msg_add_sb_copy(&vl_msg_info, C_HAL_SB_GSM_TX_CONTROL,
				      &v_tatrf_StartTxControl,
				      sizeof(v_tatrf_StartTxControl));
	if (NULL == pl_sb) {
		return TAT_ISI_HANDLER_ERROR;
	}

	/* Send ISI message - Blocking function. */
	int vl_Error = tatrf_isi_send(&vl_msg_info, "Start TX 2G");
	if (!TAT_OK(vl_Error)) {
		return vl_Error;
	}

	/* message successfully sent. waiting for response */
	C_HAL_RF_TEST_CONTROL_RESP_STR *pl_resp =
	    tatrf_isi_read_simple(&vl_msg_info, C_HAL_RF_TEST_CONTROL_RESP,
				  "Start TX 2G response", &vl_Error);
	if (NULL != pl_resp) {
		/* this what we expected */
		v_tatrf_StartTxStatusValue = pl_resp->status;
	}

	return vl_Error;
}

