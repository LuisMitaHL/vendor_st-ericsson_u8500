/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file    tatl05selftest.c
* \brief   implementation of HATS SIM selftest routines
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "tatlsim.h"
#include "tatl05selftest.h"
#include "tatl03misc.h"

#if defined(SET_AB8500_PBT_MODE_FOR_UICC)
#include "tat-pmu-lib.h"
#endif
#include <isi_driver_lib.h>

#include <test_handler_modem_ext.h>
#include <modemi2c_test_handler.h>

/*  UICC SELF TEST INTERFACE */

/** UICC Self test status. Value is one of the action execution result code. */
u8 v_tatlsim_uicc_selftest_status = TATLSIM_DEFAULT_SELFTEST_RESULT;

/** I2C Self test status. Value is one of the action execution result code. */
u8 v_tatlsim_i2c_selftest_status = TATLSIM_DEFAULT_SELFTEST_RESULT;

int v_tatsim_doingSelfTest = 0;

/**
 * Table containing unitary self test results.
 * Each value can be:
 * SLFTST_CLK if self test is successfull
 * SLFTST_SHORT_TO_GND if line is short-circuited with GND
 * SLFTST_SHORT_TO_SUPPLY if line is short-circuited with power
 * SLFTST_SHORT_TO_OTHERS if line is short-circuited with another line (not GND or power)
 */
u8 v_tatlsim_selftest_uicc_results[TATLSIM_NB_SELFTEST_LINES];

static int tatl5_selftest_I2C(void);
static int tatl5_selftest_UICC(void);

static void tatl3_dumpSimSelfTestReq(uint8_t * req)
{
	SYSLOGLOC(LOG_DEBUG);
	SYSLOGSTR(LOG_DEBUG, "\n=============================\n");
	SYSLOGSTR(LOG_DEBUG, "Self Test Request :\n");
	SYSLOGSTR(LOG_DEBUG, "=============================\n");

	tatl3_printIsiHeader((t_isi_header *) req);
	req += sizeof(t_isi_header);

	tatl3_printModemReq((t_ModemTest_RunReq *) req);
}

static void tatl3_dumpSimSelfTestResp(uint8_t * resp)
{
	SYSLOGLOC(LOG_DEBUG);
	SYSLOGSTR(LOG_DEBUG, "\n=============================\n");
	SYSLOGSTR(LOG_DEBUG, "Self Test Response :\n");
	SYSLOGSTR(LOG_DEBUG, "=============================\n");

	tatl3_printIsiHeader((t_isi_header *) resp);
	resp += sizeof(t_isi_header);

	tatl3_printModemResp((t_ModemTest_RunResp *) resp);
}

static int talt5_06IsActionPermitted(struct dth_element *elem)
{
	int vl_res = 0;

	/* Self tests can be done as long as the SIM is not activated */
	if ((v_tatsim_testsMask & TATSIM_TEST_ACTIVATION) == 0) {
		switch (elem->user_data) {
		case ACT_SELFTEST_UICC:

			/* available */
			vl_res = 1;
			break;

		case ACT_SELFTEST_I2C:
			/* UICC self test must NOT be done yet */
			if ((v_tatsim_testsMask & TATSIM_TEST_UICC_SELFTEST) ==
			    0)
				vl_res = 1;
			else
				SYSLOG(LOG_ERR,
				       "error: I2C self test should be first.");
			break;
		}
	} else {
		SYSLOG(LOG_ERR,
		       "error: self tests are not available after the SIM is activated.");
	}

	return vl_res;
}

int tatl5_01SelfTest_exec(struct dth_element *elem)
{
	int vl_error = TAT_ERROR_OFF;
	SYSLOG(LOG_DEBUG, "Executing %s...", elem->path);

	if (!talt5_06IsActionPermitted(elem))
		return EPERM;
	switch (elem->user_data) {
	case ACT_SELFTEST_UICC:
		v_tatsim_doingSelfTest = 1;
		vl_error = tatl5_selftest_UICC();
		if (vl_error)
			v_tatlsim_uicc_selftest_status =
			    TATLSIM_DEFAULT_SELFTEST_ERROR;
		v_tatsim_doingSelfTest = 0;
		break;
	case ACT_SELFTEST_I2C:
		v_tatsim_doingSelfTest = 1;
		vl_error = tatl5_selftest_I2C();
		if (vl_error)
			v_tatlsim_i2c_selftest_status =
			    TATLSIM_DEFAULT_SELFTEST_ERROR;
		v_tatsim_doingSelfTest = 0;
		break;

	default:
		SYSLOG(LOG_ERR, "Invalid action");
		vl_error = EBADRQC;
	}

	return vl_error;
}

int tatl5_selftest_I2C(void)
{
	int vl_error = TAT_ERROR_OFF;
	uint8_t vl_isi[ISI_DRIVER_MAX_MESSAGE_LENGTH];
	t_isi_header *pl_hdr;
	t_ModemTest_RunReq *pl_req;
	size_t vl_req_len, vl_resp_len = 0;
	t_ModemTest_RunResp *pl_resp = NULL;
	uint32_t vl_rsc_id = 0U;

	SYSLOG(LOG_DEBUG, "SIM I2C Self Test");
	memset(vl_isi, 0, ISI_DRIVER_MAX_MESSAGE_LENGTH);

	/* Construct the SIM message to be sent */
	pl_hdr = (t_isi_header *) vl_isi;
	vl_req_len = sizeof(t_isi_header);

	pl_req = (t_ModemTest_RunReq *) (vl_isi + vl_req_len);
	vl_req_len += MODEM_TEST_RUN_REQ_LEN;

	pl_req->trans_id = 0x77;
	pl_req->message_id = MODEM_TEST_RUN_REQ;
	pl_req->group_id = MODEM_TEST_GRP_SELFTEST_L23;
	pl_req->test_id = I2C_HAL_SELFTEST_HANDLER;
	pl_req->case_ID = 0;	/* not significant for self test */
	pl_req->attrib = MODEM_TEST_ATTR_NONE;
	pl_req->fillbyte = 0xAA;
	pl_req->inp_data_length = 0;	/* no payload */

	isi_common_add_isi_header((uint8 *) pl_hdr, vl_req_len,
				  PN_DSP_SIM_TEST);

	tatl3_dumpSimSelfTestReq(vl_isi);
	tatl3_marshModemRunReq(pl_req, ISI_MARSH);

	vl_error =
	    tat_modem_send(TATSIM_FEATURE, vl_isi, vl_req_len, &vl_rsc_id);
	if (0 == vl_error) {
		/* Message successfully sent. Waiting for response */

		/* Read ISI message until RFIC Read Response Message is received - Read in blocking mode */
		memset(vl_isi, 0, ISI_DRIVER_MAX_MESSAGE_LENGTH);
		vl_resp_len = sizeof(t_isi_header) + MODEM_TEST_RUN_RESP_LEN;
		vl_error =
		    tat_modem_read(TATSIM_FEATURE, vl_rsc_id, vl_isi,
				   &vl_resp_len);

		/* Decoding of the SIM HAL response */
		if (0 == vl_error) {
			pl_resp =
			    (t_ModemTest_RunResp *) (vl_isi +
						     sizeof(t_isi_header));
			tatl3_marshModemRunResp(pl_resp, ISI_UNMARSH);
			tatl3_dumpSimSelfTestResp(vl_isi);

			/* update global Self test status */
			v_tatlsim_i2c_selftest_status = pl_resp->status;
			v_tatsim_testsMask |= TATSIM_TEST_I2C_SELFTEST;
		} else {
			/* No message of desired type */
			SYSLOG(LOG_DEBUG,
			       "Listening for response ended with code %d",
			       vl_error);
			vl_error = ENOMSG;
		}
	} else {
		SYSLOG(LOG_DEBUG, "Sending SIM message failed with code %d",
		       vl_error);
		vl_error = EPROTO;
	}

	return vl_error;
}

int tatl5_selftest_UICC(void)
{
	int vl_error = TAT_ERROR_OFF;
	uint8_t vl_isi[ISI_DRIVER_MAX_MESSAGE_LENGTH];
	t_isi_header *pl_hdr;
	t_ModemTest_RunReq *pl_req;
	size_t vl_req_len, vl_resp_len = 0;
	t_ModemTest_RunResp *pl_resp = NULL;
	tSIMHALTST_HatsExchange *pl_hats_ex;
	uint32_t vl_rsrc_id = 0U;
#if defined(SET_AB8500_PBT_MODE_FOR_UICC)
	u8 vl_pmu_testmode = 0U;	/* save AB8500 test mode before UICC self test to restore it later */
	int vl_err_pmu = 0;	/* used for UICC self test */
#endif

	SYSLOG(LOG_DEBUG, "SIM UICC Self Test");
	memset(vl_isi, 0, ISI_DRIVER_MAX_MESSAGE_LENGTH);

	/* Construct the SIM message to be sent */
	pl_hdr = (t_isi_header *) vl_isi;
	vl_req_len = sizeof(t_isi_header);

	pl_req = (t_ModemTest_RunReq *) (vl_isi + vl_req_len);
	vl_req_len += MODEM_TEST_RUN_REQ_LEN;

	pl_req->trans_id = 0x77;
	pl_req->message_id = MODEM_TEST_RUN_REQ;
	pl_req->group_id = MODEM_TEST_GRP_LICENSEE_L23;
	pl_req->test_id = SIM_HAL_TEST_HANDLER;
	pl_req->case_ID = SIMHALTST_HATS_REQUEST;	/* not significant for self test */
	pl_req->attrib = MODEM_TEST_ATTR_NONE;
	pl_req->fillbyte = 0xAA;
	pl_req->inp_data_length = 1;	/* no payload */

	tSIMHALTST_HatsExchange *pl_args =
	    (tSIMHALTST_HatsExchange *) (vl_isi + vl_req_len);
	vl_req_len += SIMHALTST_HATS_EXCHANGE_LEN;
	pl_args->Action = HATS_ACTION_SELFTEST;

	isi_common_add_isi_header((uint8 *) pl_hdr, vl_req_len,
				  PN_DSP_SIM_TEST);

#if defined(SET_AB8500_PBT_MODE_FOR_UICC)
	/* set AB8500 to PBT mode */
	SYSLOG(LOG_DEBUG, "Checking AB8500 test mode...");
	vl_pmu_testmode = 0x00;
	vl_err_pmu = abxxxx_read(0x1100, &vl_pmu_testmode);
	if (vl_err_pmu < 0) {
		SYSLOG(LOG_ERR,
		       "UICC self test cannot be run: fail to set AB8500 in test mode");
		vl_error = EIO;
	} else if (vl_pmu_testmode != 0x01) {
		SYSLOG(LOG_DEBUG, "Setting AB8500 in test mode...");
		vl_err_pmu = abxxxx_write(0x1100, 0x01);
		if (vl_err_pmu < 0) {
			SYSLOG(LOG_ERR,
			       "UICC self test cannot be run: fail to set AB8500 in test mode");
			vl_error = EIO;
		} else {
			SYSLOG(LOG_DEBUG, "AB8500 is in test mode.");
		}
	}
#endif

	if (TAT_ERROR_OFF == vl_error) {

		tatl3_dumpSimReq(vl_isi);
		tatl3_marshModemRunReq(pl_req, ISI_MARSH);

		/* send self test request */
		vl_error =
		    tat_modem_send(TATSIM_FEATURE, vl_isi, vl_req_len,
				   &vl_rsrc_id);
		if (0 == vl_error) {
			/* Message successfully sent. Waiting for response */

			/* Read ISI message until RFIC Read Response Message is received - Read in blocking mode */
			vl_resp_len =
			    sizeof(t_isi_header) + MODEM_TEST_RUN_RESP_LEN +
			    SIMHALTST_HATS_EXCHANGE_LEN;
			memset(vl_isi, 0, ISI_DRIVER_MAX_MESSAGE_LENGTH);
			vl_error =
			    tat_modem_read(TATSIM_FEATURE, vl_rsrc_id, vl_isi,
					   &vl_resp_len);

			/* Decoding of the SIM HAL response */
			if (0 == vl_error) {
				/* reverse endianess of response */
				pl_resp =
				    (t_ModemTest_RunResp *) (vl_isi +
							     sizeof
							     (t_isi_header));
				pl_hats_ex =
				    (tSIMHALTST_HatsExchange *) ((u8 *) pl_resp
								 +
								 MODEM_TEST_RUN_RESP_LEN);
				tatl3_marshModemRunResp(pl_resp, ISI_UNMARSH);

				tatl3_dumpSimResp(vl_isi);

				/* extract response data */
				v_tatlsim_uicc_selftest_status =
				    pl_hats_ex->Action;
				memset(v_tatlsim_selftest_uicc_results,
				       TATLSIM_DEFAULT_SELFTEST_RESULT,
				       sizeof(v_tatlsim_selftest_uicc_results));
				/* extract the 4 lines selftest results */
				memcpy(v_tatlsim_selftest_uicc_results,
				       pl_hats_ex->Data,
				       sizeof(v_tatlsim_selftest_uicc_results));

				v_tatsim_testsMask |= TATSIM_TEST_UICC_SELFTEST;
			} else {
				/* No message of desired type */
				SYSLOG(LOG_DEBUG,
				       "Listening for response ended with code %d",
				       vl_error);
				vl_error = ENOMSG;
			}
		} else {
			SYSLOG(LOG_DEBUG,
			       "Sending SIM message failed with code %d",
			       vl_error);
			vl_error = EPROTO;
		}
	}
#if defined(SET_AB8500_PBT_MODE_FOR_UICC)
	/* Restore AB8500 PBT mode */
	SYSLOG(LOG_DEBUG, "Restore AB8500 test mode");
	abxxxx_write(0x1100, 0x01);
#endif

	return vl_error;
}

int tatl5_04SelfTest_get(struct dth_element *elem, void *value)
{
	int vl_error = TAT_ERROR_OFF, i;
	int vl_cell;

	switch (elem->user_data) {
	case ARG_SELFTEST_UICC_STATUS:
		{
			u8 *u8ptr = (u8 *) value;
			*u8ptr = v_tatlsim_uicc_selftest_status;
			break;
		}
	case ARG_SELFTEST_UICC_RESULT:
		{
			struct dth_array *ValTab = NULL;
			u16 *pl_DataOut = NULL;
			ValTab = (struct dth_array *)value;
			vl_cell =
			    talt3_07IsCellArray(elem, ValTab->row, ValTab->col);
			switch (vl_cell) {
			case TATLSIM_CELL:
				{
					int index =
					    (ValTab->row * elem->cols) +
					    ValTab->col;
					if (index >= 0
					    && index <
					    TATLSIM_NB_SELFTEST_LINES) {
						pl_DataOut =
						    (u16 *) ValTab->array;
						*pl_DataOut =
						    (u16)
						    v_tatlsim_selftest_uicc_results
						    [index];
						SYSLOG(LOG_DEBUG,
						       "Get Self test result [%d] = 0x%02X",
						       index, *pl_DataOut);
					} else {
						vl_error = EINVAL;
					}
					break;
				}
			case TATLSIM_ARRAY:
				pl_DataOut = (u16 *) ValTab->array;
				for (i = 0; i < TATLSIM_NB_SELFTEST_LINES; i++) {
					*pl_DataOut =
					    (u16)
					    v_tatlsim_selftest_uicc_results[i];
					SYSLOG(LOG_DEBUG,
					       "Get Self test result [%d] = 0x%02X",
					       i, *pl_DataOut);
					pl_DataOut++;
				}
				break;
			default:
				vl_error = EINVAL;
			}
			break;
		}
	case ARG_SELFTEST_I2C_STATUS:
		{
			u8 *u8ptr = (u8 *) value;
			*u8ptr = v_tatlsim_i2c_selftest_status;
			break;
		}
	default:
		vl_error = EBADRQC;
	}

	SYSLOG(LOG_DEBUG, "Getting done (%d)", vl_error);

	return vl_error;
}

int tatl5_05SelfTest_set(struct dth_element *elem, void *value)
{
	int vl_error = EBADRQC;

	if ((elem == NULL) && (value == NULL))
		return vl_error;

	return vl_error;
}
