/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   sglber.c
* \brief   routines common to Single Ended Bit Error Rate test interfaces
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "rfdef.h"

#include "misc.h"
#include "isimsg.h"
#ifdef STUB_SINGLE_BER
#include "isidump.h"
#endif

float tatl20_03divide(float num, float div, float if_zero)
{
	float res = if_zero;

	if (0.0 != div)
		res = num / div * 100.0;

	return res;
}

int tatl20_06pageNumber(struct dth_element *pp_elem)
{
	int vl_page = -1;
	static const char *BER_SLOT_PATTERN = "/SLOT/SLOT";

	char *pl_substr = strstr(pp_elem->path, BER_SLOT_PATTERN);
	if (0 != pl_substr) {
		vl_page = atoi(pl_substr + strlen(BER_SLOT_PATTERN));
		if (vl_page != 0) {
			vl_page--;	/* 1 based index but returned 0 based index */
			/*SYSLOG(LOG_INFO, "page for %s equals %d", pp_elem->path, vl_page); */
		}
	}

	return vl_page;
}

#ifndef STUB_SINGLE_BER

/* STANDARD IMPLEMENTATION */

int tatl20_01start(u8 * pp_status)
{
	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	START_COMMAND_LIST_REQ_STR *pl_req =
	    tatrf_isi_msg_begin_no_sb(&vl_msg_info,
				      PN_DSP_COMMON_TEST,
				      START_COMMAND_LIST_REQ);
	if (NULL == pl_req) {
		return TAT_ISI_HANDLER_ERROR;
	}

	START_COMMAND_LIST_RESP_STR *pl_resp = NULL;

	/* send ISI message - blocking function */
	int vl_Error = tatrf_isi_send(&vl_msg_info, "Start BER");
	if (!TAT_OK(vl_Error)) {
		return vl_Error;
	}

	/* message successfully sent. waiting for response */
	pl_resp = tatrf_isi_read_simple(&vl_msg_info, START_COMMAND_LIST_RESP,
					"Start BER response", &vl_Error);

	if (NULL != pl_resp) {
		*pp_status = LOWBYTE(pl_resp->fill1_status);
	}

	return vl_Error;
}

int tatl20_02stop(u8 * pp_status)
{
	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	STOP_COMMAND_LIST_REQ_STR *pl_req =
	    tatrf_isi_msg_begin_no_sb(&vl_msg_info,
				      PN_DSP_COMMON_TEST,
				      STOP_COMMAND_LIST_REQ);
	if (NULL == pl_req) {
		return TAT_ISI_HANDLER_ERROR;
	}

	STOP_COMMAND_LIST_RESP_STR *pl_resp = NULL;

	/* send ISI message - blocking function */
	int vl_Error = tatrf_isi_send(&vl_msg_info, "Stop BER");
	if (!TAT_OK(vl_Error)) {
		return vl_Error;
	}

	/* message successfully sent. waiting for response */
	pl_resp = tatrf_isi_read_simple(&vl_msg_info, STOP_COMMAND_LIST_RESP,
					"Stop BER response", &vl_Error);

	if (NULL != pl_resp) {
		*pp_status = LOWBYTE(pl_resp->fill1_status);
	}

	return vl_Error;
}

int tatl20_04clear(u8 * pp_status)
{
	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	SEQUENCER_RESET_REQ_STR *pl_req =
	    tatrf_isi_msg_begin_no_sb(&vl_msg_info,
				      PN_DSP_COMMON_TEST, SEQUENCER_RESET_REQ);
	if (NULL == pl_req) {
		return TAT_ISI_HANDLER_ERROR;
	}

	SEQUENCER_RESET_RESP_STR *pl_resp = NULL;

	/* send ISI message - blocking function */
	int vl_Error = tatrf_isi_send(&vl_msg_info, "Sequencer reset");
	if (!TAT_OK(vl_Error)) {
		return vl_Error;
	}

	/* message successfully sent. waiting for response */
	pl_resp = tatrf_isi_read_simple(&vl_msg_info, SEQUENCER_RESET_RESP,
					"Sequencer reset response", &vl_Error);

	if (NULL != pl_resp) {
		*pp_status = LOWBYTE(pl_resp->fill1_status);
	}

	return vl_Error;
}

int tatl20_05refreshStatus(u8 * pp_status)
{
	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	GET_STATUS_REQ_STR *pl_req = tatrf_isi_msg_begin_no_sb(&vl_msg_info,
							       PN_DSP_COMMON_TEST,
							       GET_STATUS_REQ);
	if (NULL == pl_req) {
		return TAT_ISI_HANDLER_ERROR;
	}

	/* send ISI message - blocking function */
	int vl_Error = tatrf_isi_send(&vl_msg_info, "Get DTU status");
	if (!TAT_OK(vl_Error)) {
		return vl_Error;
	}

	/* message successfully sent. waiting for response */
	GET_STATUS_RESP_STR *pl_resp = tatrf_isi_read_simple(&vl_msg_info,
							     GET_STATUS_RESP,
							     "Get DTU status response",
							     &vl_Error);

	if (NULL != pl_resp) {
		*pp_status = LOWBYTE(pl_resp->fill1_status);
	}

	return vl_Error;
}

#else

/* STUB IMPLEMENTATION */
u16 v_tatrf_sber_stub_dut_state = C_TEST_OFF;

/* incremented each time a GET_RESULTS_REQ has been triggered in RUN state. 
 * This is used to simulate end of measurements: measurement ends on X call to 
 * GET_RESULTS_REQ then returning back to READY state */
int v_tatrf_sber_stub_get_status = 0;

u16 tatl20_07getDutState()
{
    return v_tatrf_sber_stub_dut_state;
}

int tatl20_08getStatus()
{
    return v_tatrf_sber_stub_get_status;
}

void tatl20_09setDutState(u16 vp_state)
{
    if ((C_TEST_OFF <= vp_state) && (C_TEST_RUN >= vp_state))
    {
        v_tatrf_sber_stub_dut_state = vp_state;
    }
}

int tatl20_01start(u8 *pp_status)
{
    /* build legitimate request */
    struct tatrf_isi_msg_t vl_msg_info;
    tatrf_isi_init(&vl_msg_info);

    START_COMMAND_LIST_REQ_STR* pl_req = tatrf_isi_msg_begin_no_sb(&vl_msg_info,
        PN_DSP_COMMON_TEST, START_COMMAND_LIST_REQ);
    if ( NULL == pl_req )
    {
        return TAT_ISI_HANDLER_ERROR;
    }

    tatrf_isi_dump(&vl_msg_info, "Start BER 2G (stubbed)", LOG_INFO);

    /* build fake response */
    START_COMMAND_LIST_RESP_STR *pl_resp = NULL;
    tatrf_isi_init(&vl_msg_info);

    int vl_Error = 0;
    pl_resp = tatrf_isi_msg_begin_no_sb(&vl_msg_info, PN_DSP_COMMON_TEST,
        START_COMMAND_LIST_RESP);
    if ( NULL == pl_resp )
    {
        return TAT_ISI_HANDLER_ERROR;
    }

    if ( v_tatrf_sber_stub_dut_state == C_TEST_READY )
    {
        pl_resp->fill1_status = MAKE16(0xAA, C_TEST_OK);
    }
    else
    {
        pl_resp->fill1_status = MAKE16(0xAA, C_TEST_WRONG_STATE);
    }

    tatrf_isi_dump(&vl_msg_info, "Start BER 2G response (stubbed)", LOG_INFO);

    /* process fake response */
    *pp_status = LOWBYTE(pl_resp->fill1_status);
    if ( *pp_status == C_TEST_OK )
    {
        v_tatrf_sber_stub_dut_state = C_TEST_RUN;
        v_tatrf_sber_stub_get_status = 0;
    }

    return vl_Error;
}

int tatl20_02stop(u8 *pp_status)
{
    /* build legitimate request */
    struct tatrf_isi_msg_t vl_msg_info;
    tatrf_isi_init(&vl_msg_info);

    STOP_COMMAND_LIST_REQ_STR* pl_req = tatrf_isi_msg_begin_no_sb(&vl_msg_info,
        PN_DSP_COMMON_TEST, STOP_COMMAND_LIST_REQ);
    if ( NULL == pl_req )
    {
        return TAT_ISI_HANDLER_ERROR;
    }

    STOP_COMMAND_LIST_RESP_STR *pl_resp = NULL;

    tatrf_isi_dump(&vl_msg_info, "Stop BER 2G (stubbed)", LOG_INFO);

    /* build fake response */
    tatrf_isi_init(&vl_msg_info);

    int vl_Error = 0;
    pl_resp = tatrf_isi_msg_begin_no_sb(&vl_msg_info, PN_DSP_COMMON_TEST,
        STOP_COMMAND_LIST_RESP);
    if ( NULL == pl_resp )
    {
        return TAT_ISI_HANDLER_ERROR;
    }

    if ( v_tatrf_sber_stub_dut_state == C_TEST_RUN )
    {
        pl_resp->fill1_status = MAKE16(0xAA, C_TEST_OK);
    }
    else
    {
        pl_resp->fill1_status = MAKE16(0xAA, C_TEST_WRONG_STATE);
    }

    tatrf_isi_dump(&vl_msg_info, "Stop BER 2G response (stubbed)", LOG_INFO);

    /* process fake response */
    *pp_status = LOWBYTE(pl_resp->fill1_status);

    if ( *pp_status == C_TEST_OK )
    {
        v_tatrf_sber_stub_dut_state = C_TEST_READY;
        v_tatrf_sber_stub_get_status = 0;
    }

    return vl_Error;
}

int tatl20_04clear(u8 *pp_status)
{
    /* build legitimate request */
    struct tatrf_isi_msg_t vl_msg_info;
    tatrf_isi_init(&vl_msg_info);

    SEQUENCER_RESET_REQ_STR* pl_req = tatrf_isi_msg_begin_no_sb(&vl_msg_info,
        PN_DSP_COMMON_TEST, SEQUENCER_RESET_REQ);
    if ( NULL == pl_req )
    {
        return TAT_ISI_HANDLER_ERROR;
    }

    SEQUENCER_RESET_RESP_STR *pl_resp = NULL;

    tatrf_isi_dump(&vl_msg_info, "Sequencer reset (stubbed)", LOG_INFO);

    /* build fake response */
    tatrf_isi_init(&vl_msg_info);

    int vl_Error = 0;
    pl_resp = tatrf_isi_msg_begin_no_sb(&vl_msg_info, PN_DSP_COMMON_TEST,
        SEQUENCER_RESET_RESP);
    if ( NULL == pl_resp )
    {
        return TAT_ISI_HANDLER_ERROR;
    }

    pl_resp->fill1_status = MAKE16(0xAA, C_TEST_OK);

    tatrf_isi_dump(&vl_msg_info, "Sequencer reset response (stubbed)", LOG_INFO);

    /* process fake response */
    *pp_status = LOWBYTE(pl_resp->fill1_status);

    v_tatrf_sber_stub_dut_state = C_TEST_OFF;
    v_tatrf_sber_stub_get_status = 0;

    return vl_Error;
}

int tatl20_05refreshStatus(u8 *pp_status)
{
    /* build legitimate request */
    struct tatrf_isi_msg_t vl_msg_info;
    tatrf_isi_init(&vl_msg_info);

    GET_STATUS_REQ_STR* pl_req = tatrf_isi_msg_begin_no_sb(&vl_msg_info,
        PN_DSP_COMMON_TEST, GET_STATUS_REQ);
    if ( NULL == pl_req )
    {
        return TAT_ISI_HANDLER_ERROR;
    }

    tatrf_isi_dump(&vl_msg_info, "Get DTU status (stubbed)", LOG_INFO);

    /* build fake response */
    GET_STATUS_RESP_STR *pl_resp = NULL;
    tatrf_isi_init(&vl_msg_info);

    int vl_Error = 0;
    pl_resp = tatrf_isi_msg_begin_no_sb(&vl_msg_info, PN_DSP_COMMON_TEST,
        GET_STATUS_RESP);
    if ( NULL == pl_resp )
    {
        return TAT_ISI_HANDLER_ERROR;
    }

    if ( v_tatrf_sber_stub_dut_state == C_TEST_RUN )
    {
        v_tatrf_sber_stub_get_status ++;
        if ( 2 < v_tatrf_sber_stub_get_status )
        {
            v_tatrf_sber_stub_dut_state = C_TEST_READY;
        }
    }

    pl_resp->fill1_status = MAKE16(0xAA, v_tatrf_sber_stub_dut_state);

    tatrf_isi_dump(&vl_msg_info, "Get DTU status response (stubbed)", LOG_INFO);

    /* process fake response */
    *pp_status = LOWBYTE(pl_resp->fill1_status);

    return vl_Error;
}

#endif /* STUB_SINGLE_BER */
