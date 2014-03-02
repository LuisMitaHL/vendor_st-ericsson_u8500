/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   tx3goutpwrmeas.c
* \brief   routines to manage WCDMA output power measurement
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "rfdef.h"
#include "tx3goutpwrmeas.h"

#include "misc.h"
#include "isimsg.h"

/* Global */

C_TEST_SB_RESERVED_FOR_LICENSEE_REQ4_STR v_tatrf_outpwrmeas_wcdma_sb;

C_TEST_SB_RESERVED_FOR_LICENSEE_REQ4_SEQ v_tatrf_outpwrmeas_wcdma_req_seq =
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

C_TEST_SB_RESERVED_FOR_LICENSEE_RESP4_SEQ v_tatrf_outpwrmeas_wcdma_resp_seq =
{0, 0, 0, 0, 0, 0, 0, 0, 0};


/* Function tide to WCDMA internal power measurement */
/*****************************************************/
int DthRf_OutPwrMeasure3G_get(struct dth_element *elem, void *value)
{
	return tatl22_31Tx3G_OutputPowerMeasure_Get(elem, value);
}

int DthRf_OutPwrMeasure3G_set(struct dth_element *elem, void *value)
{
	return tatl22_32Tx3G_OutputPowerMeasure_Set(elem, value);
}

int DthRf_OutPwrMeasure3G_exec(struct dth_element *elem)
{
	return tatl22_33Tx3G_OutputPowerMeasure_Exec(elem);
}

int tatl22_31Tx3G_OutputPowerMeasure_Get(struct dth_element *elem, void *value)
{
	int vl_Error = TAT_ERROR_OFF;

	switch (elem->user_data) {
		/* REQ */
		case ACT_TX3G_OUTPWRMEASURE_BAND:

			switch(v_tatrf_outpwrmeas_wcdma_req_seq.band){
				case INFO_NO_WCDMA: DEREF_PTR_SET(value, u32, 0); break;
				case INFO_WCDMA_FDD_BAND1: DEREF_PTR_SET(value, u32, 1); break;
				case INFO_WCDMA_FDD_BAND2: DEREF_PTR_SET(value, u32, 2); break;
				case INFO_WCDMA_FDD_BAND3: DEREF_PTR_SET(value, u32, 3); break;
				case INFO_WCDMA_FDD_BAND4: DEREF_PTR_SET(value, u32, 4); break;
				case INFO_WCDMA_FDD_BAND5: DEREF_PTR_SET(value, u32, 5); break;
				case INFO_WCDMA_FDD_BAND6: DEREF_PTR_SET(value, u32, 6); break;
				case INFO_WCDMA_FDD_BAND7: DEREF_PTR_SET(value, u32, 7); break;
				case INFO_WCDMA_FDD_BAND8: DEREF_PTR_SET(value, u32, 8); break;
				case INFO_WCDMA_FDD_BAND9: DEREF_PTR_SET(value, u32, 9); break;
				case INFO_WCDMA_FDD_BAND10: DEREF_PTR_SET(value, u32, 10); break;
				default: vl_Error = TAT_BAD_REQ; break;
			}
			break;

		case ACT_TX3G_OUTPWRMEASURE_CHANNEL:
			DEREF_PTR_SET(value, u16, v_tatrf_outpwrmeas_wcdma_req_seq.channel);
			break;

		case ACT_TX3G_OUTPWRMEASURE_CONTROL:
			DEREF_PTR_SET(value, u16, v_tatrf_outpwrmeas_wcdma_req_seq.control);
			break;

		case ACT_TX3G_OUTPWRMEASURE_PWRLEVEL:
			DEREF_PTR_SET(value, float,
			      RF_UNQ(DEREF_PTR(&v_tatrf_outpwrmeas_wcdma_req_seq.power, int16), 8));
			break;

		/* RESP */
		case ACT_TX3G_OUTPWRMEASURE_STATUS:
			DEREF_PTR_SET(value, u16, v_tatrf_outpwrmeas_wcdma_resp_seq.status);
			break;

		case ACT_TX3G_OUTPWRMEASURE_POWER_IN_TEST:
			DEREF_PTR_SET(value, float,
			      RF_UNQ(DEREF_PTR(&v_tatrf_outpwrmeas_wcdma_resp_seq.power_in_test, int16), 8));
			break;

		case ACT_TX3G_OUTPWRMEASURE_POWER_LAST_REQ:
			DEREF_PTR_SET(value, float,
			      RF_UNQ(DEREF_PTR(&v_tatrf_outpwrmeas_wcdma_resp_seq.power_last_request, int16), 8));
			break;

		case ACT_TX3G_OUTPWRMEASURE_POWER_LAST_REP:
			DEREF_PTR_SET(value, float,
			      RF_UNQ(DEREF_PTR(&v_tatrf_outpwrmeas_wcdma_resp_seq.power_last_report, int16), 8));
			break;

		case ACT_TX3G_OUTPWRMEASURE_POWER_DELTA:
			DEREF_PTR_SET(value, float,
			      RF_UNQ(DEREF_PTR(&v_tatrf_outpwrmeas_wcdma_resp_seq.power_delta, int16), 8));
			break;

		default:
			vl_Error = TAT_BAD_REQ;
			break;
	}

	return vl_Error;
}

int tatl22_32Tx3G_OutputPowerMeasure_Set(struct dth_element *elem, void *value)
{
	int vl_Error = TAT_ERROR_OFF;

	switch (elem->user_data) {

		case ACT_TX3G_OUTPWRMEASURE_BAND:
			switch(*((uint32 *)value)){
				case 0: v_tatrf_outpwrmeas_wcdma_req_seq.band = INFO_NO_WCDMA; break;
				case 1: v_tatrf_outpwrmeas_wcdma_req_seq.band = INFO_WCDMA_FDD_BAND1; break;
				case 2: v_tatrf_outpwrmeas_wcdma_req_seq.band = INFO_WCDMA_FDD_BAND2; break;
				case 3: v_tatrf_outpwrmeas_wcdma_req_seq.band = INFO_WCDMA_FDD_BAND3; break;
				case 4: v_tatrf_outpwrmeas_wcdma_req_seq.band = INFO_WCDMA_FDD_BAND4; break;
				case 5: v_tatrf_outpwrmeas_wcdma_req_seq.band = INFO_WCDMA_FDD_BAND5; break;
				case 6: v_tatrf_outpwrmeas_wcdma_req_seq.band = INFO_WCDMA_FDD_BAND6; break;
				case 7: v_tatrf_outpwrmeas_wcdma_req_seq.band = INFO_WCDMA_FDD_BAND7; break;
				case 8: v_tatrf_outpwrmeas_wcdma_req_seq.band = INFO_WCDMA_FDD_BAND8; break;
				case 9: v_tatrf_outpwrmeas_wcdma_req_seq.band = INFO_WCDMA_FDD_BAND9; break;
				case 10: v_tatrf_outpwrmeas_wcdma_req_seq.band = INFO_WCDMA_FDD_BAND10; break;
				default: vl_Error = TAT_BAD_REQ; break;
			}

			break;

		case ACT_TX3G_OUTPWRMEASURE_CHANNEL:
			DEREF_PTR_SET(&v_tatrf_outpwrmeas_wcdma_req_seq.channel, u16, *((uint16 *)value));
			break;

		case ACT_TX3G_OUTPWRMEASURE_CONTROL:
			DEREF_PTR_SET(&v_tatrf_outpwrmeas_wcdma_req_seq.control, u16, *((uint16 *)value));
			break;

		case ACT_TX3G_OUTPWRMEASURE_PWRLEVEL:
			DEREF_PTR_SET(&v_tatrf_outpwrmeas_wcdma_req_seq.power, int16,
			      RF_Q(DEREF_PTR(value, float), 8));
			break;

		default:
			vl_Error = TAT_BAD_REQ;
			break;
	}

	return vl_Error;
}

int tatl22_33Tx3G_OutputPowerMeasure_Exec(struct dth_element *elem)
{
	int vl_Error = TAT_ERROR_OFF;

	switch( elem->user_data ){
	
		case ACT_TX3G_INTPWRMEASURE:
			vl_Error = tatl22_34Tx3G_OutputPowerMeasure_Action();
			break;

		default:
			vl_Error = TAT_BAD_REQ;
			break;
	}

	return vl_Error;
}

int tatl22_34Tx3G_OutputPowerMeasure_Action()
{
	int vl_Error = TAT_ERROR_OFF;

	memset(&v_tatrf_outpwrmeas_wcdma_sb, 0, SIZE_C_TEST_SB_RESERVED_FOR_LICENSEE_REQ4_STR);

	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	C_TEST_TUNING_REQ_STR *vl_req;
	vl_req = tatrf_isi_msg_begin(&vl_msg_info, PN_DSP_COMMON_TEST,
					C_TEST_TUNING_REQ, numberOfSubBlocks);
	if (NULL == vl_req)
	{
		return TAT_ISI_HANDLER_ERROR;
	}

	C_TEST_SB_RESERVED_FOR_LICENSEE_REQ4_STR *pl_sb =
	tatrf_isi_msg_add_sb_copy(&vl_msg_info,
				C_TEST_SB_RESERVED_FOR_LICENSEE_REQ4,
				&v_tatrf_outpwrmeas_wcdma_sb,
				SIZE_C_TEST_SB_RESERVED_FOR_LICENSEE_REQ4_STR);
	if (NULL == pl_sb)
	{
		return TAT_ISI_HANDLER_ERROR;
	}

	vl_Error = tatrf_isi_append_data(&vl_msg_info, (u16 *)(&v_tatrf_outpwrmeas_wcdma_req_seq),
					SIZE_C_TEST_SB_RESERVED_FOR_LICENSEE_REQ4_SEQ / sizeof(u16));
	if (!TAT_OK(vl_Error))
	{
		return vl_Error;
	}

	/* send ISI message - blocking function */
	vl_Error = tatrf_isi_send(&vl_msg_info, "Start WCDMA Output Power Measure Request");
	if (!TAT_OK(vl_Error))
	{
		return vl_Error;
	}

	memset(&v_tatrf_outpwrmeas_wcdma_resp_seq, 0, SIZE_C_TEST_SB_RESERVED_FOR_LICENSEE_RESP4_SEQ);

	/* message successfully sent. waiting for response */
	C_TEST_TUNING_RESP_STR *pl_resp = tatrf_isi_read(&vl_msg_info, C_TEST_TUNING_RESP, numerOfSubBlocks,
							"Start WCDMA Output Power Measure Response", &vl_Error);
	if (NULL == pl_resp)
		return TAT_ISI_HANDLER_ERROR;

	C_TEST_SB_RESERVED_FOR_LICENSEE_RESP4_STR *pl_sbr_hdr =
			tatrf_isi_find_sb(&vl_msg_info, C_TEST_SB_RESERVED_FOR_LICENSEE_RESP4, NULL);

	if (NULL == pl_sbr_hdr)
	{
		return TAT_ERROR_NOT_MATCHING_MSG;
	}

	u8 *pl_ptrby = (u8 *)pl_sbr_hdr;
	C_TEST_SB_RESERVED_FOR_LICENSEE_RESP4_SEQ *pl_sbr =
		(C_TEST_SB_RESERVED_FOR_LICENSEE_RESP4_SEQ *)((u8 *)pl_sbr_hdr + SIZE_C_TEST_SB_RESERVED_FOR_LICENSEE_RESP4_STR);

	memcpy(&v_tatrf_outpwrmeas_wcdma_resp_seq, pl_sbr, SIZE_C_TEST_SB_RESERVED_FOR_LICENSEE_RESP4_SEQ);

	return vl_Error;
}