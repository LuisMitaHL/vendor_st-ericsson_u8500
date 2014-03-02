/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file    tatl03misc.c
* \brief   implementation of SIM general routines and tools
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include <tatlsim.h>
#include <dthsim.h>
#include <tatl03misc.h>
#include <tatl05selftest.h>

#include <isi_driver_lib.h>
#include <tatasync.h>

static tSIMHALTST_HatsExchange v_tatsim_req_args;
static tSIMHALTST_HatsExchange v_tatsim_resp_args;
int v_tatsim_activity = TATSIM_SIM_DEACTIVATED;
static int v_tatsim_activate = TATSIM_DEACTIVATE;
static u8 v_tatsim_atr[TATLSIM_ATR_LENGTH];
static u8 v_tatsim_apdu_data[TATLSIM_NB_APDU_DATA];
static u8 v_tatsim_apdu_result[TATLSIM_NB_APDU_RESULT];
static int v_tatsim_SimClockState = ENUM_SMC_CLOCK_STOPPED;
static u8 v_tatsim_clockactivation = ENUM_SMC_START_CLOCK;

extern int v_tatsim_doingSelfTest;	/* from tatl05selftest.c */

void tatl3_initModule(void)
{
	/* raw inits */
	memset(&v_tatsim_req_args, 0, sizeof(tSIMHALTST_HatsExchange));
	memset(&v_tatsim_resp_args, 0, sizeof(tSIMHALTST_HatsExchange));
	memset(v_tatsim_atr, 0xFF, TATLSIM_ATR_LENGTH * sizeof(u8));
	memset(v_tatsim_apdu_data, 0, TATLSIM_NB_APDU_DATA * sizeof(u8));
	memset(v_tatsim_apdu_result, 0, TATLSIM_NB_APDU_RESULT * sizeof(u8));

	/* set defaults */
	v_tatsim_req_args.Voltage = ENUM_SMC_ACTIVATE_1_8V;
	v_tatsim_req_args.CardClkFreq = ENUM_SMC_CLOCK_3_84MHZ;
	v_tatsim_req_args.ClkStopLevel = ENUM_SMC_STOP_CLOCK_HIGH;
}

/*Display MODEM Request on console screen*/
/*****************************************/
void tatl3_printIsiHeader(t_isi_header * hdr)
{
	SYSLOGSTR(LOG_DEBUG, "[isi header]\n");
	SYSLOGSTR(LOG_DEBUG, "Media Id        : 0x%.02x\n", hdr->mediaID);
	SYSLOGSTR(LOG_DEBUG, "RxDev           : 0x%.02x\n", hdr->rxDev);
	SYSLOGSTR(LOG_DEBUG, "TxDev           : 0x%.02x\n", hdr->txDev);
	SYSLOGSTR(LOG_DEBUG, "Ressource Id    : 0x%.02x\n", hdr->resourceID);
	SYSLOGSTR(LOG_DEBUG, "length          : %i\n", hdr->length);
	SYSLOGSTR(LOG_DEBUG, "rxObj           : 0x%.02x\n", hdr->rxObj);
	SYSLOGSTR(LOG_DEBUG, "txObj           : 0x%.02x\n\n", hdr->txObj);
}

void tatl3_printModemReq(t_ModemTest_RunReq * req)
{
	SYSLOGSTR(LOG_DEBUG, "[modem run req]\n");
	SYSLOGSTR(LOG_DEBUG, "transaction id  : 0x%.02x\n", req->trans_id);
	SYSLOGSTR(LOG_DEBUG, "message id      : 0x%.02x\n", req->message_id);
	SYSLOGSTR(LOG_DEBUG, "group id        : 0x%.02x\n", req->group_id);
	SYSLOGSTR(LOG_DEBUG, "test id         : 0x%.02x\n", req->test_id);
	SYSLOGSTR(LOG_DEBUG, "case id         : 0x%.02x\n", req->case_ID);
	SYSLOGSTR(LOG_DEBUG, "attrib          : 0x%x\n", req->attrib);
	SYSLOGSTR(LOG_DEBUG, "filler          : 0x%x\n", req->fillbyte);
	SYSLOGSTR(LOG_DEBUG, "data length     : %u\n", req->inp_data_length);
}

void tatl3_printModemResp(t_ModemTest_RunResp * resp)
{
	SYSLOGSTR(LOG_DEBUG, "[modem run resp]\n");
	SYSLOGSTR(LOG_DEBUG, "transaction id  : 0x%.02x\n", resp->trans_id);
	SYSLOGSTR(LOG_DEBUG, "message id      : 0x%.02x\n", resp->message_id);
	SYSLOGSTR(LOG_DEBUG, "group id        : 0x%.02x\n", resp->group_id);
	SYSLOGSTR(LOG_DEBUG, "test id         : 0x%.02x\n", resp->test_id);
	SYSLOGSTR(LOG_DEBUG, "case id         : 0x%.02x\n", resp->case_ID);
	SYSLOGSTR(LOG_DEBUG, "status          : 0x%.02x\n", resp->status);
	SYSLOGSTR(LOG_DEBUG, "data length     : %u\n", resp->out_data_length);
}

void tatl3_printHatsExchange(tSIMHALTST_HatsExchange * ex)
{
	int i;
	SYSLOGSTR(LOG_DEBUG, "[HATS exchange]\n");
	SYSLOGSTR(LOG_DEBUG, "Action          : 0x%.02x\n", ex->Action);
	SYSLOGSTR(LOG_DEBUG, "Voltage         : 0x%.02x\n", ex->Voltage);
	SYSLOGSTR(LOG_DEBUG, "Card clk freq.  : 0x%.02x\n", ex->CardClkFreq);
	SYSLOGSTR(LOG_DEBUG, "Clk stop level  : 0x%.02x\n", ex->ClkStopLevel);
	SYSLOGSTR(LOG_DEBUG, "Fi / Di factors : 0x%.02x\n", ex->FD_Factors);
	SYSLOGSTR(LOG_DEBUG, "Protocol T=0/1  : 0x%.02x\n", ex->Protocol);
	SYSLOGSTR(LOG_DEBUG, "WI              : 0x%.02x\n", ex->WI);
	SYSLOGSTR(LOG_DEBUG, "Class           : 0x%.02x\n", ex->Class);
	SYSLOGSTR(LOG_DEBUG, "Ins             : 0x%.02x\n", ex->Ins);
	SYSLOGSTR(LOG_DEBUG, "P1              : 0x%.02x\n", ex->P1);
	SYSLOGSTR(LOG_DEBUG, "P2              : 0x%.02x\n", ex->P2);
	SYSLOGSTR(LOG_DEBUG, "Lc              : 0x%.02x\n", ex->Lc);
	SYSLOGSTR(LOG_DEBUG, "LeLSB           : 0x%.02x\n", ex->LeLSB);
	SYSLOGSTR(LOG_DEBUG, "LeMSB           : 0x%.02x\n", ex->LeMSB);
	SYSLOGSTR(LOG_DEBUG, "Sw1             : 0x%.02x\n", ex->Sw1);
	SYSLOGSTR(LOG_DEBUG, "Sw2             : 0x%.02x\n", ex->Sw2);
	SYSLOGSTR(LOG_DEBUG, "data            :\n");

	for (i = 0; i < 256; i++) {
		if ((i > 0) && (i % 16) == 0) {
			SYSLOGSTR(LOG_DEBUG, "\n");
		}
		SYSLOGSTR(LOG_DEBUG, "0x%.02x ", ex->Data[i]);
	}
	SYSLOGSTR(LOG_DEBUG, "\n");
}

void tatl3_dumpSimReq(uint8_t * req)
{
	SYSLOGLOC(LOG_DEBUG);
	SYSLOGSTR(LOG_DEBUG, "\n=============================\n");
	SYSLOGSTR(LOG_DEBUG, "Request info :\n");
	SYSLOGSTR(LOG_DEBUG, "=============================\n");

	tatl3_printIsiHeader((t_isi_header *) req);
	req += sizeof(t_isi_header);

	tatl3_printModemReq((t_ModemTest_RunReq *) req);
	req += MODEM_TEST_RUN_REQ_LEN;

	tatl3_printHatsExchange((tSIMHALTST_HatsExchange *) req);
}

/*Display MODEM Response on console screen*/
/******************************************/
void tatl3_dumpSimResp(u8 * resp)
{
	SYSLOGLOC(LOG_DEBUG);
	SYSLOGSTR(LOG_DEBUG, "\n=============================\n");
	SYSLOGSTR(LOG_DEBUG, "Response info :\n");
	SYSLOGSTR(LOG_DEBUG, "=============================\n");

	tatl3_printIsiHeader((t_isi_header *) resp);
	resp += sizeof(t_isi_header);

	tatl3_printModemResp((t_ModemTest_RunResp *) resp);
	resp += MODEM_TEST_RUN_RESP_LEN;

	tatl3_printHatsExchange((tSIMHALTST_HatsExchange *) resp);
}

static int talt3_08IsActionPermitted(struct dth_element *elem)
{
	int vl_res = 0;

	int vl_mode = TAT_MODEM_MODE_UNKNOWN;
	vl_res = tat_modem_get_mode(TAT_MODEM_SRCE_AUTO, &vl_mode);
	if (0 == vl_res) {
		switch (vl_mode) {
		case TAT_MODEM_MODE_LOCAL:

			switch (elem->user_data) {
			case ACT_ACTIVATION:

				/* SIM activation/deactivation not available during self test */
				if (0 == v_tatsim_doingSelfTest)
					vl_res = 1;
				else
					SYSLOG(LOG_ERR,
					       "error: SIM (de)activation is not available during self test");
				break;

			case ACT_PPS:

				/* must be invoked immediatly after the SIM was activated */
				if (TATSIM_SIM_ACTIVATED != v_tatsim_activity)
					SYSLOG(LOG_ERR,
					       "error: activate SIM first");
				else if ((v_tatsim_testsMask &
					  TATSIM_SIM_TEST_MASK) != 0)
					SYSLOG(LOG_ERR,
					       "error: SIM PPS can only be performed after the SIM was activated and before calling any of the SIM test");
				else
					vl_res = 1;
				break;

			case ACT_SET_CLOCK:
			case ACT_CLOCK:
			case ACT_SEND_APDU:

				/* SIM must be activated */
				if (TATSIM_SIM_ACTIVATED != v_tatsim_activity)
					SYSLOG(LOG_ERR,
					       "error: activate SIM first");
				else
					vl_res = 1;
				break;

			case ACT_DUMP_LOOP_START:
			case ACT_DUMP_LOOP_STOP:

				vl_res = 1;
				break;

			default:
				SYSLOG(LOG_ERR, "not a valid action code: %d",
				       elem->user_data);
			}

			break;

		case TAT_MODEM_MODE_NORMAL:

			SYSLOG(LOG_ERR,
			       "SIM interface is not available in normal mode");
			break;

		default:
			SYSLOG(LOG_ERR,
			       "modem is not available or initialized.");
		}
	}

	return vl_res;
}

void fillSimReqCommon(t_isi_header * hdr, t_ModemTest_RunReq * req, size_t len)
{
	/* set header for SIM HATS request */
	isi_common_add_isi_header((u8 *) hdr, len, PN_DSP_SIM_TEST);

	/* set MODEM_RUN_REQ */
	req->message_id = MODEM_TEST_RUN_REQ;
	req->trans_id = 0x77;
	req->group_id = MODEM_TEST_GRP_LICENSEE_L23;
	req->test_id = SIM_HAL_TEST_HANDLER;
	req->case_ID = SIMHALTST_HATS_REQUEST;
	req->attrib = MODEM_TEST_ATTR_NONE;
	req->fillbyte = 0xAA;
	req->inp_data_length = SIMHALTST_HATS_EXCHANGE_LEN;
}

int fillSimReqData(struct dth_element *elem, tSIMHALTST_HatsExchange * data,
		   uint16_t * data_length)
{
	int code = 0;
	*data_length = SIMHALTST_HATS_EXCHANGE_LEN;

	/* The command is NO MORE equal to elem->user_data for SIM request */

	switch (elem->user_data) {
	case ACT_ACTIVATION:

		switch (v_tatsim_activate) {
		case TATSIM_ACTIVATE:	/* SIM activation */

			data->Action = HATS_ACTION_ACTIVATION;
			data->Voltage = v_tatsim_req_args.Voltage;
			break;

		case TATSIM_DEACTIVATE:	/* SIM deactivation */

			data->Action = HATS_ACTION_DEACTIVATION;
			break;

		default:	/* not a valid SIM activation arg */
			SYSLOG(LOG_ERR,
			       "Error: SIM activation illegal argument!");
			code = EINVAL;
		}
		break;

	case ACT_PPS:

		data->Action = HATS_ACTION_PPS;
		data->FD_Factors = v_tatsim_req_args.FD_Factors;
		data->Protocol = v_tatsim_req_args.Protocol;
		data->WI = v_tatsim_req_args.WI;
		break;

	case ACT_SET_CLOCK:

		data->Action = HATS_ACTION_SET_FREQUENCY;
		data->CardClkFreq = v_tatsim_req_args.CardClkFreq;
		break;

	case ACT_CLOCK:

		switch (v_tatsim_clockactivation) {
		case ENUM_SMC_START_CLOCK:
			/* valid action with no attribute */
			data->Action = HATS_ACTION_CARD_CLOCK_START;
			break;

		case ENUM_SMC_STOP_CLOCK:
			data->Action = HATS_ACTION_CARD_CLOCK_STOP;
			data->ClkStopLevel = v_tatsim_req_args.ClkStopLevel;
			break;

		default:
			SYSLOG(LOG_ERR, "stop clock illegal argument");
			code = EINVAL;
		}
		break;

	case ACT_SEND_APDU:

		data->Action = HATS_ACTION_SEND_CMD;
		data->Class = v_tatsim_req_args.Class;
		data->Ins = v_tatsim_req_args.Ins;
		data->P1 = v_tatsim_req_args.P1;
		data->P2 = v_tatsim_req_args.P2;
		data->Lc = v_tatsim_req_args.Lc;
		data->LeLSB = v_tatsim_req_args.LeLSB;
		data->LeMSB = v_tatsim_req_args.LeMSB;
		memset(data->Data, 0xFF, 256);
		memcpy(data->Data, v_tatsim_apdu_data, TATLSIM_NB_APDU_DATA);
		break;

	case ACT_DUMP_LOOP_START:
		data->Action = HATS_ACTION_DUMP_LOOP_START;
		*data_length = 1;
		break;

	case ACT_DUMP_LOOP_STOP:
		data->Action = HATS_ACTION_DUMP_LOOP_STOP;
		*data_length = 1;
		break;

	default:
		/* Error: invalid action code */
		SYSLOG(LOG_ERR, "Error: illegal SIM action!");
		code = EBADRQC;
	}

	v_tatsim_req_args.Action = data->Action;

	return code;
}

int tatlsim_decode_atr(uint8_t * atr, uint32_t size)
{
	/* byte position in atr ( < size ) */
	uint32_t b = 0;
	uint8_t byte = 0;

	/* first ATR byte is the initial character (TS) */
	if (b >= size)
		goto end_of_atr;
	byte = atr[++b];

	SYSLOGLOC(LOG_DEBUG);
	SYSLOGSTR(LOG_DEBUG, "\nATR: T0 = 0x%X\n", byte);

	/* second is the format character T0, codes Y(1) and K. This could be considered as TD(0) */
	int i, j;
	i = j = 0;
	uint8_t y, t;
	y = t = 0;
	do {
		/* read TD(i), indicator of presence of interface bytes, TA(i+1) present if byte 5=1, TB(i+1) present if byte 6=1, etc. */
		t = 0x0F & byte;
		SYSLOGSTR(LOG_DEBUG, "ATR: T(%d) = %u\n", i, t);

		i++;

		y = (0xF0 & byte) >> 4;
		SYSLOGSTR(LOG_DEBUG, "ATR: Y(%d) = 0x%X -> ", i, y);

		for (j = 3; j >= 0; j--) {
			if ((y & (0x01 << j)) != 0)
				SYSLOGSTR(LOG_DEBUG, "%c", 'A' + j);
			else
				SYSLOGSTR(LOG_DEBUG, ".");
		}
		SYSLOGSTR(LOG_DEBUG, "\n\n");

		if (b >= size)
			goto end_of_atr;

		byte = atr[++b];

		/* j=0 -> TA, j=1 -> TB, j=2 -> TC, j=3 -> TD */
		for (j = 0; j < 4; j++) {
			if ((y & (0x01 << j)) != 0) {
				switch (j) {
					/* TA(i) present */
				case 0:
					SYSLOGSTR(LOG_DEBUG,
						  "ATR: TA(%d) = 0x%02X\n", i,
						  byte);
					switch (i) {
						/* TA(1) : high=FI, low=DI (see IEC-1816-3 6.5.2) */
					case 1:
						v_tatsim_req_args.FD_Factors =
						    byte;
						libtatasync_msg_send(VALUE_CHANGED, ARG_PPS_F_PATH);
						libtatasync_msg_send(VALUE_CHANGED, ARG_PPS_D_PATH);
						SYSLOGSTR(LOG_INFO,
							  "ATR: FD factors Fi=%u, Di=%u\n",
							  PPS_FI_FACTOR(byte),
							  PPS_DI_FACTOR(byte));
						break;
						/* TA(2) : specific mode byte (see IEC-1816-3 6.5.7 and 6.6) */
					case 2:
						break;

					default:
						;
					}
					break;

					/* TB(i) present */
				case 1:
					SYSLOGSTR(LOG_DEBUG,
						  "ATR: TB(%d) = 0x%02X\n", i,
						  byte);
/*                    switch (i) {*/
/*                        |+ TB(1) : b8 = 0 codes II (b7 b6) and PI1 (b5 to b1) (see IEC-1816-3 6.5.4) +|*/
/*                    case 1:*/
/*                        break;*/

/*                        |+ TB(2) : PI2, alternative to PI1 (see IEC-1816-3 6.5.4) +|*/
/*                    case 2:*/
/*                        break;*/

/*                    default:*/
/*                        ;*/
/*                    }*/
/*                    break;*/

					/* TC(i) present */
				case 2:
					SYSLOGSTR(LOG_DEBUG,
						  "ATR: TC(%d) = 0x%02X\n", i,
						  byte);
					switch (i) {
						/* TC(1) : extra guard time (see IEC-1816-3 6.5.3) */
					case 1:
						break;

						/* The specific interface byte TC(2) codes the integer value
						 * WI over the eight bits; the null value is reserved for future
						 * use. If no TC(2) appears in the Answer-to-Reset, then the
						 * default value is WI = 10. */
					case 2:
						v_tatsim_req_args.WI = byte;
						SYSLOGSTR(LOG_INFO,
							  "ATR: WI = %u\n",
							  v_tatsim_req_args.WI);
						break;

					default:
						;
					}
					break;

				case 3:
					SYSLOGSTR(LOG_DEBUG,
						  "ATR: TD(%d) = 0x%02X\n", i,
						  byte);
					break;
				}

				/* read next ATR byte */
				if (j < 3) {
					if (b >= size)
						goto end_of_atr;
					byte = atr[++b];
				}
			} else if (j == 3) {
				/* no TD */
				SYSLOGSTR(LOG_DEBUG, "ATR: no TD(%d)\n", i);
				y = 0;
			}
		}
	} while (y);

	goto end_ok;

end_of_atr:
	SYSLOG(LOG_ERR, "ATR is missformed!");

end_ok:

	SYSLOGSTR(LOG_DEBUG, "\n");
	return b;
}

void tatl3_marshModemRunReq(t_ModemTest_RunReq * req, int marsh)
{
	IsiMarsh16(&req->group_id, marsh);
	IsiMarsh16(&req->test_id, marsh);
	IsiMarsh16(&req->case_ID, marsh);
	IsiMarsh16(&req->inp_data_length, marsh);
}

void tatl3_marshModemRunResp(t_ModemTest_RunResp * resp, int marsh)
{
	IsiMarsh16(&resp->group_id, marsh);
	IsiMarsh16(&resp->test_id, marsh);
	IsiMarsh16(&resp->case_ID, marsh);
	IsiMarsh16(&resp->out_data_length, marsh);
}

int tatl3_00GetSimInfo_exec(struct dth_element *elem)
{
	uint8_t vl_isi[ISI_DRIVER_MAX_MESSAGE_LENGTH];
	t_isi_header *pl_hdr;
	t_ModemTest_RunReq *pl_req;
	t_ModemTest_RunResp *pl_resp = NULL;
	tSIMHALTST_HatsExchange *pl_hats_ex;
	size_t vl_req_len, vl_resp_len = 0;
	int vl_Error = TAT_ERROR_OFF;
	uint32_t vl_resourceid = 0U;

	SYSLOG(LOG_DEBUG, "Executing %s",
	       elem == NULL ? "an unreferenced element" : elem->path);
	memset(vl_isi, 0, ISI_DRIVER_MAX_MESSAGE_LENGTH);

	/* Construct the SIM message to be sent */
	pl_hdr = (t_isi_header *) vl_isi;
	vl_req_len = sizeof(t_isi_header);

	pl_req = (t_ModemTest_RunReq *) (vl_isi + vl_req_len);
	vl_req_len += MODEM_TEST_RUN_REQ_LEN;

	pl_hats_ex = (tSIMHALTST_HatsExchange *) (vl_isi + vl_req_len);
	vl_req_len += SIMHALTST_HATS_EXCHANGE_LEN;

	fillSimReqCommon(pl_hdr, pl_req, vl_req_len);

	if (!talt3_08IsActionPermitted(elem)) {
		vl_Error = EPERM;
	} else if (fillSimReqData(elem, pl_hats_ex, &pl_req->inp_data_length) ==
		   0) {
		/* OK */
		tatl3_dumpSimReq(vl_isi);

		tatl3_marshModemRunReq(pl_req, ISI_MARSH);

		vl_Error = tat_modem_send(TATSIM_FEATURE, vl_isi, vl_req_len,
					  &vl_resourceid);
		if (0 == vl_Error) {
			/* Message successfully sent. Waiting for response */

			/* Read ISI message until RFIC Read Response Message is received - Read in blocking mode */
			vl_resp_len =
			    sizeof(t_isi_header) + MODEM_TEST_RUN_RESP_LEN +
			    SIMHALTST_HATS_EXCHANGE_LEN;
			memset(vl_isi, 0, ISI_DRIVER_MAX_MESSAGE_LENGTH);
			vl_Error =
			    tat_modem_read(TATSIM_FEATURE, vl_resourceid,
					   vl_isi, &vl_resp_len);

			/* Decoding of the SIM HAL response */
			if (0 == vl_Error) {
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

				switch (pl_hats_ex->Action) {
					/* nominals */
				case HATS_ACTION_SUCCESSFUL:

					/* Recover specific data for the request executed */
					switch (v_tatsim_req_args.Action) {
						/* Copy ATR if request was ACTIVATION or DEACTIVATION */
					case HATS_ACTION_ACTIVATION:

						SYSLOG(LOG_DEBUG,
						       "SIM was successfuly activated.");
						memcpy(v_tatsim_atr,
						       pl_hats_ex->Data,
						       TATLSIM_ATR_LENGTH);

						v_tatsim_req_args.WI = 10;	/* see comment for TC(2) in tatlsim_decode_atr() */
						tatlsim_decode_atr(v_tatsim_atr,
								   pl_hats_ex->
								   LeMSB * 256 +
								   pl_hats_ex->
								   LeLSB);
						v_tatsim_activity =
						    TATSIM_SIM_ACTIVATED;
						v_tatsim_testsMask &=
						    TATSIM_SELFTEST_MASK;
						v_tatsim_testsMask |=
						    TATSIM_TEST_ACTIVATION;
						break;

					case HATS_ACTION_DEACTIVATION:

						SYSLOG(LOG_DEBUG,
						       "SIM was successfuly deactivated.");
						memcpy(v_tatsim_atr,
						       pl_hats_ex->Data,
						       TATLSIM_ATR_LENGTH);
						v_tatsim_activity =
						    TATSIM_SIM_DEACTIVATED;
						v_tatsim_testsMask |=
						    TATSIM_TEST_DEACTIVATION;
						break;

					case HATS_ACTION_SEND_CMD:

						/* Copy Data, LeMSB, LeLSB, SW1 and SW2 */
						memcpy(v_tatsim_apdu_result,
						       pl_hats_ex->Data,
						       TATLSIM_NB_APDU_RESULT);
						v_tatsim_resp_args.LeMSB =
						    pl_hats_ex->LeMSB;
						v_tatsim_resp_args.LeLSB =
						    pl_hats_ex->LeLSB;
						v_tatsim_resp_args.Sw1 =
						    pl_hats_ex->Sw1;
						v_tatsim_resp_args.Sw2 =
						    pl_hats_ex->Sw2;
						v_tatsim_testsMask |=
						    TATSIM_TEST_SEND_CMD;
						break;

					case HATS_ACTION_PPS:

						v_tatsim_resp_args.FD_Factors =
						    pl_hats_ex->FD_Factors;
						v_tatsim_testsMask |=
						    TATSIM_TEST_PPS;
						break;

					case HATS_ACTION_SET_FREQUENCY:

						v_tatsim_testsMask |=
						    TATSIM_TEST_SET_FREQUENCY;
						break;

					case HATS_ACTION_CARD_CLOCK_STOP:

						v_tatsim_SimClockState = 1;

						v_tatsim_testsMask |=
						    TATSIM_TEST_CLOCK_STOP;
						break;

					case HATS_ACTION_CARD_CLOCK_START:

						v_tatsim_SimClockState = 0;
						v_tatsim_testsMask |=
						    TATSIM_TEST_CLOCK_START;
						break;

						/* Nothing to be done in default */
					}

					break;	/* case HATS_ACTION_SUCCESSFUL */

					/* alternatives */
				case HATS_ACTION_MODIFIED:
					/* The request was executed but its result is not the one expected.
					 * This error can be produced in response of a PPS request as SIM HAL has change protocl speed by himself. */
					SYSLOG(LOG_ERR,
					       "Unpredictable SIM HAL behavior.");
					vl_Error = ENOTRECOVERABLE;
					break;

					/* Unrecoverable error cases */
				case HATS_ACTION_FAILED:
				case HATS_ACTION_UNKNOWN:
				case HATS_ACTION_NO_REPLY:
				case HATS_ACTION_NO_CARD:
				case HATS_ACTION_FRAME_ERROR:

					SYSLOG(LOG_ERR,
					       "An unrecoverable SIM HAL error occurred!");
					vl_Error = ENOTRECOVERABLE;
					break;

					/* Other shoud be considered as an error case */
				default:

					/* SIM HAL new return code? This code may need an update. */
					SYSLOG(LOG_ERR,
					       "Unknown SIM HAL result code received!");
					vl_Error = ENOTRECOVERABLE;
				}
			} else {
				/* No message of desired type */
				SYSLOG(LOG_ERR,
				       "Listening for response ended with code %d",
				       vl_Error);
				vl_Error = ENOMSG;
			}
		} else {
			SYSLOG(LOG_ERR,
			       "Sending SIM message failed with code %d",
			       vl_Error);
			vl_Error = EPROTO;
		}
	} else {
		SYSLOG(LOG_ERR, "Invalid SIM ISI action");
		vl_Error = ENOTRECOVERABLE;
	}

	SYSLOG(LOG_DEBUG, "Leaving tatl3_00GetSimInfo_exec with code %d",
	       vl_Error);

	return vl_Error;
}

int tatl3_02GetSimInfoParam_get(struct dth_element *elem, void *Value)
{
	int vl_Error = TAT_ERROR_OFF, i = 0;
	int vl_cell = 0;

	SYSLOG(LOG_DEBUG, "get %s...", elem->path);

	switch (elem->user_data) {
	case ACT_ACTIVATION:	/* out */
		{
			u8 *u8ptr = (u8 *) Value;
			*u8ptr = v_tatsim_activate;
			break;
		}
	case ARG_ACTIVATION_VOLTAGE:	/* in/out */
		{
			u8 *u8ptr = (u8 *) Value;
			*u8ptr = v_tatsim_req_args.Voltage;
			break;
		}
	case ARG_ACTIVATION_ATR:	/* out */
		{
			struct dth_array *ValTab = NULL;
			u8 *pl_DataOut = NULL;

			ValTab = (struct dth_array *)Value;

			vl_cell =
			    talt3_07IsCellArray(elem, ValTab->row, ValTab->col);
			switch (vl_cell) {
			case TATLSIM_CELL:
				{
					int index =
					    (ValTab->row * elem->cols) +
					    ValTab->col;
					pl_DataOut = (u8 *) ValTab->array;
					*pl_DataOut = v_tatsim_atr[index];
					SYSLOG(LOG_DEBUG, "Get ATR[%d] = 0x%X",
					       index, *pl_DataOut);
					break;
				}
			case TATLSIM_ARRAY:
				{
					pl_DataOut = (u8 *) ValTab->array;
					for (i = 0; i < TATLSIM_ATR_LENGTH; i++) {
						*pl_DataOut = v_tatsim_atr[i];
						pl_DataOut++;
					}
					/* Dump ATR first 32 bytes */
					pl_DataOut = (u8 *) ValTab->array;
					SYSLOGSTR(LOG_DEBUG, "ATR :\n");
					for (i = 0; i < 32; i++)
						SYSLOGSTR(LOG_DEBUG, " 0x%.02X",
							  pl_DataOut[i]);
					SYSLOGSTR(LOG_DEBUG, "\n\n");
					break;
				}
			default:
				vl_Error = EINVAL;
			}
			break;
		}
	case ARG_SET_CLOCK_FREQ:	/* in/out */
		{
			u8 *u8ptr = (u8 *) Value;
			*u8ptr = v_tatsim_req_args.CardClkFreq;
			break;
		}
	case ARG_PPS_F:	/* in/out */
		{
			u8 *u8ptr = (u8 *) Value;
			*u8ptr = PPS_FI_FACTOR(v_tatsim_req_args.FD_Factors);
			break;
		}
	case ARG_PPS_D:	/* in/out */
		{
			u8 *u8ptr = (u8 *) Value;
			*u8ptr = PPS_DI_FACTOR(v_tatsim_req_args.FD_Factors);
			break;
		}
	case ARG_PPS_STATUS:	/* in/out */
		{
			u8 *u8ptr = (u8 *) Value;
			*u8ptr = v_tatsim_resp_args.FD_Factors;
			break;
		}
	case ARG_STOP_CLOCK_LEVEL:	/* in/out */
		{
			u8 *u8ptr = (u8 *) Value;
			*u8ptr = v_tatsim_req_args.ClkStopLevel;
			break;
		}
	case ARG_SEND_APDU_CLASS:	/* in/out */
		{
			u8 *u8ptr = (u8 *) Value;
			*u8ptr = v_tatsim_req_args.Class;
			break;
		}
	case ARG_SEND_APDU_INS:	/* in/out */
		{
			u8 *u8ptr = (u8 *) Value;
			*u8ptr = v_tatsim_req_args.Ins;
			break;
		}
	case ARG_SEND_APDU_P1:	/* in/out */
		{
			u8 *u8ptr = (u8 *) Value;
			*u8ptr = v_tatsim_req_args.P1;
			break;
		}
	case ARG_SEND_APDU_P2:	/* in/out */
		{
			u8 *u8ptr = (u8 *) Value;
			*u8ptr = v_tatsim_req_args.P2;
			break;
		}
	case ARG_SEND_APDU_LC:	/* in/out */
		{
			u8 *u8ptr = (u8 *) Value;
			*u8ptr = v_tatsim_req_args.Lc;
			break;
		}
	case ARG_SEND_APDU_LELSB:	/* in/out */
		{
			u8 *u8ptr = (u8 *) Value;
			*u8ptr = v_tatsim_req_args.LeLSB;
			break;
		}
	case ARG_SEND_APDU_LEMSB:	/* in/out */
		{
			u8 *u8ptr = (u8 *) Value;
			*u8ptr = v_tatsim_req_args.LeMSB;
			break;
		}
	case ARG_SEND_APDU_DATAOUT:	/* out */
		{
			struct dth_array *ValTab = NULL;
			u8 *pl_DataOut = NULL;
			ValTab = (struct dth_array *)Value;

			vl_cell =
			    talt3_07IsCellArray(elem, ValTab->row, ValTab->col);
			switch (vl_cell) {
			case TATLSIM_CELL:
				{
					int index =
					    (ValTab->row * elem->cols) +
					    ValTab->col;
					pl_DataOut = (u8 *) ValTab->array;
					*pl_DataOut =
					    v_tatsim_apdu_result[index];
					SYSLOG(LOG_DEBUG,
					       "Get APDU OUT[%d] = 0x%X", index,
					       *pl_DataOut);
					break;
				}
			case TATLSIM_ARRAY:
				{
					pl_DataOut = (u8 *) ValTab->array;
					for (i = 0; i < TATLSIM_NB_APDU_RESULT;
					     i++) {
						*pl_DataOut =
						    v_tatsim_apdu_result[i];
						pl_DataOut++;
					}

					SYSLOGLOC(LOG_DEBUG);
					SYSLOGSTR(LOG_DEBUG, "\nAPDU OUT :\n");
					pl_DataOut = (u8 *) ValTab->array;
					for (i = 0; i < 25; i++) {
						SYSLOGSTR(LOG_DEBUG, " 0x%.02X",
							  pl_DataOut[i]);
					}
					SYSLOGSTR(LOG_DEBUG, "\n\n");
					break;
				}
			default:
				vl_Error = EINVAL;
			}
			break;
		}
	case ARG_SEND_APDU_SW1:	/* out */
		{
			u8 *u8ptr = (u8 *) Value;
			*u8ptr = v_tatsim_resp_args.Sw1;
			SYSLOG(LOG_DEBUG, "Sw1 = %02X", v_tatsim_resp_args.Sw1);
			break;
		}
	case ARG_SEND_APDU_SW2:	/* out */
		{
			u8 *u8ptr = (u8 *) Value;
			*u8ptr = v_tatsim_resp_args.Sw2;
			SYSLOG(LOG_DEBUG, "Sw2 = %02X", v_tatsim_resp_args.Sw2);
			break;
		}
	case ARG_CLOCK_STATE:	/* out */
		{
			u8 *u8ptr = (u8 *) Value;
			*u8ptr = v_tatsim_SimClockState;
			break;
		}
	default:
		vl_Error = EBADRQC;
	}

	SYSLOG(LOG_DEBUG, "Getting done (%d)", vl_Error);

	return vl_Error;
}

int tatl3_03GetSimInfoParam_set(struct dth_element *elem, void *Value)
{
	int vl_Error = TAT_ERROR_OFF, i;
	int vl_cell;

	SYSLOG(LOG_DEBUG, "Setting %s", elem->path);
	switch (elem->user_data) {
	case ACT_ACTIVATION:
		{
			u8 *u8ptr = (u8 *) Value;
			v_tatsim_activate = *u8ptr;
			break;
		}
	case ARG_ACTIVATION_VOLTAGE:
		{
			u8 *u8ptr = (u8 *) Value;
			v_tatsim_req_args.Voltage = *u8ptr;
			break;
		}
	case ARG_SET_CLOCK_FREQ:
		{
			u8 *u8ptr = (u8 *) Value;
			v_tatsim_req_args.CardClkFreq = *u8ptr;
			break;
		}
	case ARG_PPS_T:
		{
			u8 *u8ptr = (u8 *) Value;
			v_tatsim_req_args.Protocol = *u8ptr;
			break;
		}
	case ARG_PPS_F:
		{
			u8 *u8ptr = (u8 *) Value;
			v_tatsim_req_args.FD_Factors |= ((*u8ptr & 0x0F) << 4);
			break;
		}
	case ARG_PPS_D:
		{
			u8 *u8ptr = (u8 *) Value;
			v_tatsim_req_args.FD_Factors |= (*u8ptr & 0x0F);
			break;
		}
	case ACT_CLOCK:
		{
			u8 *u8ptr = (u8 *) Value;
			v_tatsim_clockactivation = *u8ptr;
			break;
		}
	case ARG_STOP_CLOCK_LEVEL:
		{
			u8 *u8ptr = (u8 *) Value;
			v_tatsim_req_args.ClkStopLevel = *u8ptr;
			break;
		}
	case ARG_SEND_APDU_CLASS:
		{
			u8 *u8ptr = (u8 *) Value;
			v_tatsim_req_args.Class = *u8ptr;
			break;
		}
	case ARG_SEND_APDU_INS:
		{
			u8 *u8ptr = (u8 *) Value;
			v_tatsim_req_args.Ins = *u8ptr;
			break;
		}
	case ARG_SEND_APDU_P1:
		{
			u8 *u8ptr = (u8 *) Value;
			v_tatsim_req_args.P1 = *u8ptr;
			break;
		}
	case ARG_SEND_APDU_P2:
		{
			u8 *u8ptr = (u8 *) Value;
			v_tatsim_req_args.P2 = *u8ptr;
			break;
		}
	case ARG_SEND_APDU_LC:
		{
			u8 *u8ptr = (u8 *) Value;
			v_tatsim_req_args.Lc = *u8ptr;
			break;
		}
	case ARG_SEND_APDU_LELSB:
		{
			u8 *u8ptr = (u8 *) Value;
			v_tatsim_req_args.LeLSB = *u8ptr;
			break;
		}
	case ARG_SEND_APDU_LEMSB:
		{
			u8 *u8ptr = (u8 *) Value;
			v_tatsim_req_args.LeMSB = *u8ptr;
			break;
		}
	case ARG_SEND_APDU_DATAIN:
		{
			struct dth_array *ValTab = NULL;
			u8 *pl_DataIn = NULL;
			ValTab = (struct dth_array *)Value;

			vl_cell =
			    talt3_07IsCellArray(elem, ValTab->row, ValTab->col);
			switch (vl_cell) {
			case TATLSIM_CELL:
				{
					int index =
					    (ValTab->row * elem->cols) +
					    ValTab->col;
					pl_DataIn = (u8 *) ValTab->array;
					v_tatsim_apdu_data[index] = *pl_DataIn;
					SYSLOG(LOG_DEBUG,
					       "Set SIM DATA[%d], 0x%X", index,
					       *pl_DataIn);
					break;
				}
			case TATLSIM_ARRAY:
				{
					pl_DataIn = (u8 *) ValTab->array;
					for (i = 0; i < TATLSIM_NB_APDU_DATA;
					     i++) {
						v_tatsim_apdu_data[i] =
						    *pl_DataIn;
						pl_DataIn++;
					}
					SYSLOGLOC(LOG_DEBUG);
					SYSLOGSTR(LOG_DEBUG, "\nAPDU IN:\n");
					for (i = 0; i < 25; i++) {
						SYSLOGSTR(LOG_DEBUG, " 0x%.02X",
							  v_tatsim_apdu_data
							  [i]);
					}

					SYSLOGSTR(LOG_DEBUG, "\n\n");
					break;
				}
			default:
				vl_Error = EINVAL;
			}
			break;
		}
	default:
		vl_Error = EBADRQC;
	}
	SYSLOG(LOG_DEBUG, "Setting done (%i)", vl_Error);
	return vl_Error;
}

int tatl3_04StopSimActivity(void)
{
	int vl_error = TAT_ERROR_OFF;

	SYSLOG(LOG_INFO, "explicit SIM deactivation");

	/* proceeding this way is not very safe but it works without rewritting the
	   shutdown code */
	struct dth_element vl_elt;
	memset(&vl_elt, 0, sizeof(vl_elt));

	v_tatsim_activate = TATSIM_DEACTIVATE;
	vl_elt.user_data = ACT_ACTIVATION;
	vl_error = tatl3_00GetSimInfo_exec(&vl_elt);

	return vl_error;
}

int talt3_07IsCellArray(struct dth_element *elem, int row, int col)
{
	int vl_res = TATLSIM_OUT_OF_ARRAY;

	if ((elem->cols == col) && (elem->rows == row))
		vl_res = TATLSIM_ARRAY;
	else if ((0 <= col) && (elem->cols > col) && (0 <= row)
		 && (elem->rows > row))
		vl_res = TATLSIM_CELL;

	return vl_res;
}
