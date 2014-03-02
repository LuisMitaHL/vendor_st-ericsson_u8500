/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   tx2g5intpwrmeas.c
* \brief   routines to manage GSM/EDGE internal power measurement
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "rfdef.h"
#include "tx2g5intpwrmeas.h"

#include "misc.h"
#include "isimsg.h"

/* Global */
uint32 v_tatrf_intpwrmeas_gsm_band = 0;

C_TEST_SB_RESERVED_FOR_LICENSEE_REQ2_STR v_tatrf_intpwrmeas_gsm_sb;

C_TEST_SB_RESERVED_FOR_LICENSEE_REQ2_SEQ v_tatrf_intpwrmeas_gsm_req_seq = {0, 0, 0, 0, 0, 0};

C_TEST_SB_RESERVED_FOR_LICENSEE_RESP2_SEQ v_tatrf_intpwrmeas_gsm_resp_seq = {0, 0, 0, 0, 0, 0};

/* Function tide to GSM/EDGE internal power measurement */
/********************************************************/
int DthRf_IntPwrMeasure2G5_get(struct dth_element *elem, void *value)
{
	return tatl22_01Tx2G5_InternalPowerMeasure_Get(elem, value);
}

int DthRf_IntPwrMeasure2G5_set(struct dth_element *elem, void *value)
{
	return tatl22_02Tx2G5_InternalPowerMeasure_Set(elem, value);
}

int DthRf_IntPwrMeasure2G5_exec(struct dth_element *elem)
{
	return tatl22_03Tx2G5_InternalPowerMeasure_Exec(elem);
}

int tatl22_01Tx2G5_InternalPowerMeasure_Get(struct dth_element *elem, void *value)
{
	int vl_Error = TAT_ERROR_OFF;

	switch (elem->user_data) {
		case ACT_TX2G5_INTPWRMEASURE_BAND:

			v_tatrf_intpwrmeas_gsm_band = SetU16InU32(MSB_POSS,
								v_tatrf_intpwrmeas_gsm_band,
								v_tatrf_intpwrmeas_gsm_req_seq.band_MSB);
			v_tatrf_intpwrmeas_gsm_band = SetU16InU32(LSB_POSS,
								v_tatrf_intpwrmeas_gsm_band,
								v_tatrf_intpwrmeas_gsm_req_seq.band_LSB);

			switch(v_tatrf_intpwrmeas_gsm_band){
				case INFO_NO_GSM: DEREF_PTR_SET(value, u32, 0); break;
				case INFO_GSM850: DEREF_PTR_SET(value, u32, 1); break;
				case INFO_GSM900: DEREF_PTR_SET(value, u32, 2); break;
				case INFO_GSM1800: DEREF_PTR_SET(value, u32, 3); break;
				case INFO_GSM1900: DEREF_PTR_SET(value, u32, 4); break;
				default: vl_Error = TAT_BAD_REQ; break;
			}
			break;

		case ACT_TX2G5_INTPWRMEASURE_CHANNEL:
			DEREF_PTR_SET(value, u16, v_tatrf_intpwrmeas_gsm_req_seq.channel);
			break;

		case ACT_TX2G5_INTPWRMEASURE_MODE:
			DEREF_PTR_SET(value, u8, HIGHBYTE(v_tatrf_intpwrmeas_gsm_req_seq.mode_filler));
			break;

		case ACT_TX2G5_INTPWRMEASURE_PCL:
			DEREF_PTR_SET(value, u16, v_tatrf_intpwrmeas_gsm_req_seq.powerlevel);
			break;

		case ACT_TX2G5_INTPWRMEASURE_FWDPOWER:
			DEREF_PTR_SET(value, float,
			      RF_UNQ(DEREF_PTR(&v_tatrf_intpwrmeas_gsm_resp_seq.transpower, int16), 6));
			break;

		case ACT_TX2G5_INTPWRMEASURE_REFPOWER:
			DEREF_PTR_SET(value, float,
			      RF_UNQ(DEREF_PTR(&v_tatrf_intpwrmeas_gsm_resp_seq.recvpower, int16), 6));
			break;

		case ACT_TX2G5_INTPWRMEASURE_FWDADC:
			DEREF_PTR_SET(value, u16, v_tatrf_intpwrmeas_gsm_resp_seq.transpower_inraw);
			break;

		case ACT_TX2G5_INTPWRMEASURE_REFADC:
			DEREF_PTR_SET(value, u16, v_tatrf_intpwrmeas_gsm_resp_seq.recvpower_inraw);
			break;

		case ACT_TX2G5_INTPWRMEASURE_STATUS:
			DEREF_PTR_SET(value, u8, LOWBYTE(v_tatrf_intpwrmeas_gsm_resp_seq.filler_status));
			break;

		default:
			vl_Error = TAT_BAD_REQ;
			break;
	}

	return vl_Error;
}

int tatl22_02Tx2G5_InternalPowerMeasure_Set(struct dth_element *elem, void *value)
{
	int vl_Error = TAT_ERROR_OFF;

	switch (elem->user_data) {

		case ACT_TX2G5_INTPWRMEASURE_BAND:
			switch(*((uint32 *)value)){
				case 0: v_tatrf_intpwrmeas_gsm_band = INFO_NO_GSM; break;
				case 1: v_tatrf_intpwrmeas_gsm_band = INFO_GSM850; break;
				case 2: v_tatrf_intpwrmeas_gsm_band = INFO_GSM900; break;
				case 3: v_tatrf_intpwrmeas_gsm_band = INFO_GSM1800; break;
				case 4: v_tatrf_intpwrmeas_gsm_band = INFO_GSM1900; break;
				default: vl_Error = TAT_BAD_REQ; break;
			}

			v_tatrf_intpwrmeas_gsm_req_seq.band_MSB =
				GetU16InU32(MSB_POSS, v_tatrf_intpwrmeas_gsm_band);
			v_tatrf_intpwrmeas_gsm_req_seq.band_LSB =
				GetU16InU32(LSB_POSS, v_tatrf_intpwrmeas_gsm_band);

			break;

		case ACT_TX2G5_INTPWRMEASURE_CHANNEL:
			DEREF_PTR_SET(&v_tatrf_intpwrmeas_gsm_req_seq.channel, u16, *((uint16*)value));
			break;

		case ACT_TX2G5_INTPWRMEASURE_MODE:
			switch(*((uint32 *)value)){
				case 0:
					v_tatrf_intpwrmeas_gsm_req_seq.mode_filler =
						SetU8InU16(MSB_POSS,
						v_tatrf_intpwrmeas_gsm_req_seq.mode_filler,
						C_TEST_WCDMA_TX_CW_SIGNAL);
					break;
				case 1:
					v_tatrf_intpwrmeas_gsm_req_seq.mode_filler =
						SetU8InU16(MSB_POSS,
						v_tatrf_intpwrmeas_gsm_req_seq.mode_filler,
						C_TEST_WCDMA_TX_WCDMA_SIGNAL);
					break;
				default:
					vl_Error = TAT_BAD_REQ;
					break;
			}
			break;

		case ACT_TX2G5_INTPWRMEASURE_PCL:
			DEREF_PTR_SET(&v_tatrf_intpwrmeas_gsm_req_seq.powerlevel, u16, *((uint16 *)value));
			break;

		default:
			vl_Error = TAT_BAD_REQ;
			break;
	}

	return vl_Error;
}

int tatl22_03Tx2G5_InternalPowerMeasure_Exec(struct dth_element *elem)
{
	int vl_Error = TAT_ERROR_OFF;

	switch( elem->user_data ){

		case ACT_TX2G5_INTPWRMEASURE:
			vl_Error = tatl22_04Tx2G5_InternalPowerMeasure_Action();
			break;

		default:
			vl_Error = TAT_BAD_REQ;
			break;
	}

	return vl_Error;
}

int tatl22_04Tx2G5_InternalPowerMeasure_Action()
{
	int vl_Error = TAT_ERROR_OFF;

	memset(&v_tatrf_intpwrmeas_gsm_sb, 0, SIZE_C_TEST_SB_RESERVED_FOR_LICENSEE_REQ2_STR);
	v_tatrf_intpwrmeas_gsm_req_seq.system = C_TEST_GSM;

	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	C_TEST_TUNING_REQ_STR *vl_req;
	vl_req = tatrf_isi_msg_begin(&vl_msg_info, PN_DSP_COMMON_TEST,
					C_TEST_TUNING_REQ, numberOfSubBlocks);
	if (NULL == vl_req)
	{
		return TAT_ISI_HANDLER_ERROR;
	}

	C_TEST_SB_RESERVED_FOR_LICENSEE_REQ2_STR *pl_sb =
	tatrf_isi_msg_add_sb_copy(&vl_msg_info,
				C_TEST_SB_RESERVED_FOR_LICENSEE_REQ2,
				&v_tatrf_intpwrmeas_gsm_sb,
				SIZE_C_TEST_SB_RESERVED_FOR_LICENSEE_REQ2_STR);
	if (NULL == pl_sb)
	{
		return TAT_ISI_HANDLER_ERROR;
	}

	vl_Error = tatrf_isi_append_data(&vl_msg_info, (u16 *)(&v_tatrf_intpwrmeas_gsm_req_seq),
					SIZE_C_TEST_SB_RESERVED_FOR_LICENSEE_REQ2_SEQ / sizeof(u16));
	if (!TAT_OK(vl_Error))
	{
		return vl_Error;
	}

	/* send ISI message - blocking function */
	vl_Error = tatrf_isi_send(&vl_msg_info, "Start GSM Internal Power Measure Request");
	if (!TAT_OK(vl_Error))
	{
		return vl_Error;
	}

	memset(&v_tatrf_intpwrmeas_gsm_resp_seq, 0, SIZE_C_TEST_SB_RESERVED_FOR_LICENSEE_RESP2_SEQ);

	/* message successfully sent. waiting for response */
	C_TEST_TUNING_RESP_STR *pl_resp = tatrf_isi_read(&vl_msg_info, C_TEST_TUNING_RESP, numerOfSubBlocks,
							"Start GSM internal power measure Response", &vl_Error);
	if (NULL == pl_resp)
		return TAT_ISI_HANDLER_ERROR;

	C_TEST_SB_RESERVED_FOR_LICENSEE_RESP2_STR *pl_sbr_hdr = tatrf_isi_find_sb(
							&vl_msg_info, C_TEST_SB_RESERVED_FOR_LICENSEE_RESP2, NULL);
	if (NULL == pl_sbr_hdr)
	{
		return TAT_ERROR_NOT_MATCHING_MSG;
	}

	u8 *pl_ptrby = (u8 *)pl_sbr_hdr;
	C_TEST_SB_RESERVED_FOR_LICENSEE_RESP2_SEQ *pl_sbr =
		(C_TEST_SB_RESERVED_FOR_LICENSEE_RESP2_SEQ *)(pl_ptrby + SIZE_C_TEST_SB_RESERVED_FOR_LICENSEE_RESP2_STR);

	memcpy(&v_tatrf_intpwrmeas_gsm_resp_seq, pl_sbr, SIZE_C_TEST_SB_RESERVED_FOR_LICENSEE_RESP2_SEQ);

	return vl_Error;
}