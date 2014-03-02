/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   sglber2g.c
* \brief   routines to run Single Ended Bit Error Rate for GSM/EDGE interface
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "rfdef.h"
#include "sglber2g.h"
#include "sglber.h"

#include "isimsg.h"
#ifdef STUB_SINGLE_BER
#include "isidump.h"
#endif
#include "misc.h"

/* Function tide to Single Ended BER 2G           */
/**************************************************/
int DthRf_SingleBer2G_get(struct dth_element *elem, void *value)
{
	return tatl18_02SingleBer2G_get(elem, value);
}

int DthRf_SingleBer2G_set(struct dth_element *elem, void *value)
{
	return tatl18_01SingleBer2G_set(elem, value);
}

int DthRf_SingleBer2G_exec(struct dth_element *elem)
{
	return tatl18_00SingleBer2G_exec(elem);
}

/* INTERNAL GLOBALS */

/* Single BER 2G command code */
u32 v_tatrf_sber2g_run = ACT_BER_2G_CMD_RESET;
u8 v_tatrf_sber2g_dtu_status = C_TEST_OFF;
u8 v_tatrf_sber2g_cmd_status = 0xFF;
u32 v_tatrf_sber2g_msg_format = ACT_BER_ONE_SB_PER_FREQ;

C_TEST_SB_GSM_SINGLE_BER_SYNC_STR v_tatrf_sber2g_sync;
C_TEST_SB_GSM_SINGLE_BER_STR v_tatrf_sber2g[SBER2G_NB_RESULTS];
float v_tatrf_sber2g_rx_levels[SBER2G_NB_RESULTS][8];
uint16 v_tatrf_sber2g_tx_levels[SBER2G_NB_RESULTS][8];
GSM_SBER_INFO_PER_FREQ_STR v_tatrf_sber2g_data[SBER2G_NB_RESULTS];
C_TEST_SB_GSM_SINGLE_BER_RESULTS_STR v_tatrf_sber2g_results[SBER2G_NB_RESULTS];
GSM_BER_RES_INFO_STR v_tatrf_sber2g_results_data[SBER2G_NB_RESULTS];

/* initialize single BER 2G globals */
void tatl18_initMembers(int force);

int tatl18_setInputs(struct dth_element *elem, void *value);
int tatl18_getResults(struct dth_element *elem, void *value);

int tatl18_store(void);
int tatl18_refreshResults(void);
int tatl18_setGsmSberInfo(int vp_index, int vp_slot,
			  C_TEST_SB_GSM_SINGLE_BER_STR * pl_sb,
    GSM_SBER_INFO_PER_FREQ_STR *pl_sber_info, float *pl_rx_lvl,
    uint16 vp_tx_lvl);

C_TEST_SB_GSM_SINGLE_BER_STR *tatl18_getGsmSber(struct dth_element *elem);
GSM_SBER_INFO_PER_FREQ_STR *tatl18_getGsmSberData(struct dth_element *elem);
C_TEST_SB_GSM_SINGLE_BER_RESULTS_STR* tatl18_getGsmSberResults(struct
							       dth_element
							       *elem);
GSM_BER_RES_INFO_STR *tatl18_getGsmSberResultsData(struct dth_element *elem);

/* PUBLIC INTERFACE */

int tatl18_00SingleBer2G_exec(struct dth_element *elem)
{
	int vl_result = 0;

	tatl18_initMembers(0);

//	SYSLOG(LOG_DEBUG, "enter SingleBer2G_exec(%s)", elem->path);

	switch (elem->user_data) {
	case ACT_BER_2G:

		switch (v_tatrf_sber2g_run) {
		case ACT_BER_2G_CMD_STORE:

			vl_result = tatl18_store();
			break;

		case ACT_BER_2G_CMD_START:

			vl_result = tatl20_01start(&v_tatrf_sber2g_cmd_status);
			break;

		case ACT_BER_2G_CMD_STOP:

			vl_result = tatl20_02stop(&v_tatrf_sber2g_cmd_status);
			break;

		case ACT_BER_2G_CMD_RESULT:

			vl_result = tatl18_refreshResults();
			break;

		case ACT_BER_2G_CMD_RESET:

			tatl18_initMembers(1);
			vl_result = tatl20_04clear(&v_tatrf_sber2g_cmd_status);
			break;


		default:
			SYSLOG(LOG_ERR, "Bad BER_2G command code %u",
			       v_tatrf_sber2g_run);
			vl_result = TAT_BAD_REQ;
		}
        break;

    case ACT_BER_2G_GET_STATUS:

        vl_result = tatl20_05refreshStatus(&v_tatrf_sber2g_dtu_status);
		break;

	default:
		/* Invalid request code */
		vl_result = TAT_BAD_REQ;
	}

	SYSLOG(LOG_DEBUG, "exit SingleBer2G_exec(%s) with code %d",
	       elem->path, vl_result);

	return vl_result;
}

int tatl18_01SingleBer2G_set(struct dth_element *elem, void *value)
{
	int vl_result = 0;

	tatl18_initMembers(0);

//	SYSLOG(LOG_DEBUG, "enter SingleBer2G_set(%s)", elem->path);

	switch (elem->user_data) {
		/* Commands */
	case ACT_BER_2G:

		DTH_SET_UNSIGNED(elem, value, v_tatrf_sber2g_run, vl_result);
		break;

	case ACT_BER_2G_IN_SEQUENCE_TYPE:

		DTH_SET_UNSIGNED(elem, value, v_tatrf_sber2g_msg_format,
				 vl_result);
		break;

		/* Synchronization */
	case ACT_BER_2G_SYNC_IN_TESTER:

		DTH_SET_UNSIGNED(elem, value, v_tatrf_sber2g_sync.tester_tool,
				 vl_result);
		break;

	case ACT_BER_2G_SYNC_IN_BAND:

		DTH_SET_UNSIGNED(elem, value, v_tatrf_sber2g_sync.rf_band,
				 vl_result);
		break;

	case ACT_BER_2G_SYNC_IN_BCCH_ARFCN:

		DTH_SET_UNSIGNED(elem, value, v_tatrf_sber2g_sync.arfcn_bcch,
				 vl_result);
		break;

	case ACT_BER_2G_SYNC_IN_TCH_ARFCN:

		DTH_SET_UNSIGNED(elem, value, v_tatrf_sber2g_sync.arfcn_tch,
				 vl_result);
		break;

	case ACT_BER_2G_SYNC_IN_TX_DATA_TYPE:

		DTH_SET_L8(elem, value, v_tatrf_sber2g_sync.fill1_data_type,
			   vl_result);
		break;

	case ACT_BER_2G_SYNC_IN_FN_SYNC:

		DTH_SET_UNSIGNED(elem, value,
				 v_tatrf_sber2g_sync.FN_end_of_sync, vl_result);
		break;

	case ACT_BER_2G_SYNC_IN_CHANNEL_TYPE:

		DTH_SET_H8(elem, value, v_tatrf_sber2g_sync.chan_type_codec,
			   vl_result);
		break;

	case ACT_BER_2G_SYNC_IN_CODEC:

		DTH_SET_L8(elem, value, v_tatrf_sber2g_sync.chan_type_codec,
			   vl_result);
		break;

	case ACT_BER_2G_SYNC_IN_TS_PATTERN:

		DTH_SET_L8(elem, value, v_tatrf_sber2g_sync.fill2_time_slot_pat,
			   vl_result);
		break;

	case ACT_BER_2G_SYNC_IN_CODED_MODE:

		DTH_SET_UNSIGNED(elem, value, v_tatrf_sber2g_sync.coded_mode,
				 vl_result);
		break;

    case ACT_BER_2G_SYNC_IN_INIT_RX_LEVEL:

        DTH_SET_QUOT(elem, value, vl_result, v_tatrf_sber2g_sync.init_rx_level,
            SBER2G_QUOT_INIT_RX_LEVEL);
        break;

	default:
		/* Invalid request code */
		vl_result = tatl18_setInputs(elem, value);
	}

//	SYSLOG(LOG_DEBUG, "exit SingleBer2G_set(%s) with code %d",
//	       elem->path, vl_result);

	return vl_result;
}

int tatl18_02SingleBer2G_get(struct dth_element *elem, void *value)
{
    int vl_result = 0, vl_page = -1;
    struct dth_array *pl_array = NULL;

	tatl18_initMembers(0);

//	SYSLOG(LOG_DEBUG, "enter SingleBer2G_get(%s)", elem->path);

	switch (elem->user_data) {
	case ACT_BER_2G:

		DTH_GET_UNSIGNED(v_tatrf_sber2g_run, elem, value, vl_result);
		break;

	case ACT_BER_2G_IN_SEQUENCE_TYPE:

		DTH_GET_UNSIGNED(v_tatrf_sber2g_msg_format, elem, value,
				 vl_result);
		break;

	case ACT_BER_2G_OUT_CMD_STATUS:

		DTH_GET_UNSIGNED(v_tatrf_sber2g_cmd_status, elem, value,
				 vl_result);
		break;

	case ACT_BER_2G_OUT_DUT_STATUS:

        DTH_GET_UNSIGNED(v_tatrf_sber2g_dtu_status, elem, value, vl_result);
        break;

    case ACT_BER_2G_IN_RX_LEVEL:

			pl_array = (struct dth_array *)value;
        vl_page = tatl20_06pageNumber(elem);
        if ( (vl_page >= 0) && (vl_page < SBER2G_NB_RESULTS) )
        {
				GetDthArray(elem, pl_array, sizeof(float),
                v_tatrf_sber2g_rx_levels[vl_page], sizeof(float), 8);
        }
        else
        {
				vl_result = TAT_ERROR_MISC;
        }
        break;

    case ACT_BER_2G_IN_TX_LEVEL:

        pl_array = (struct dth_array*)value;
        vl_page = tatl20_06pageNumber(elem);
        if ( (vl_page >= 0) && (vl_page < SBER2G_NB_RESULTS) )
        {
            GetDthArray(elem, pl_array, sizeof(uint16),
                v_tatrf_sber2g_tx_levels[vl_page], sizeof(uint16), 8);
        }
        else
        {
            vl_result = TAT_ERROR_MISC;
        }
        break;

    case ACT_BER_2G_SYNC_IN_CODED_MODE:

        DTH_GET_UNSIGNED(v_tatrf_sber2g_sync.coded_mode, elem, value, vl_result);
        break;

    case ACT_BER_2G_IN_CODED_MODE:
        {
            C_TEST_SB_GSM_SINGLE_BER_STR *pl_sb = tatl18_getGsmSber(elem);
            if ( 0 != pl_sb )
            {
                DTH_GET_UNSIGNED(pl_sb->coded_mode, elem, value, vl_result);
            }
            else
            {
                vl_result = TAT_ERROR_ON;
			}
		}
		break;

	default:
		vl_result = tatl18_getResults(elem, value);
	}

//	SYSLOG(LOG_DEBUG, "exit SingleBer2G_get(%s) with code %d",
//	       elem->path, vl_result);

	return vl_result;
}

/* PRIVATE */

void tatl18_initMembers(int force)
{
	static int v_tatrf_sber2g_initialized = 0;
    int i = 0;

	if (!force && v_tatrf_sber2g_initialized) {
		/* already done */
		return;
	}

    if ( 0 == force )
    {
		SYSLOG(LOG_INFO, "Reset sync and slots");
        memset(&v_tatrf_sber2g_sync, 0, sizeof(C_TEST_SB_GSM_SINGLE_BER_SYNC_STR));
        v_tatrf_sber2g_sync.coded_mode = C_TEST_TRUE;

        memset(v_tatrf_sber2g, 0, sizeof(C_TEST_SB_GSM_SINGLE_BER_STR) * SBER2G_NB_RESULTS);
        for ( i = 0; i < SBER2G_NB_RESULTS; i++ )
        {
            v_tatrf_sber2g[i].coded_mode = C_TEST_TRUE;
	}

        memset(v_tatrf_sber2g_rx_levels, 0, SBER2G_NB_RESULTS * 8 * sizeof(float));
        memset(v_tatrf_sber2g_tx_levels, 0, SBER2G_NB_RESULTS * 8 * sizeof(uint16));
        memset(v_tatrf_sber2g_data, 0, sizeof(GSM_SBER_INFO_PER_FREQ_STR) * SBER2G_NB_RESULTS);
    }

	SYSLOG(LOG_INFO, "Reset results");
    memset(v_tatrf_sber2g_results, 0, sizeof(C_TEST_SB_GSM_SINGLE_BER_RESULTS_STR) * SBER2G_NB_RESULTS);
    memset(v_tatrf_sber2g_results_data, 0, sizeof(GSM_BER_RES_INFO_STR) * SBER2G_NB_RESULTS);

	/* now initialized */
	v_tatrf_sber2g_initialized = 1;
}

int tatl18_setInputs(struct dth_element *elem, void *value)
{
    int vl_result = 0, vl_page;
	struct dth_array *pl_array;

	C_TEST_SB_GSM_SINGLE_BER_STR *pl_sb = tatl18_getGsmSber(elem);
	if (0 == pl_sb) {
		return TAT_ERROR_ON;
	}

	GSM_SBER_INFO_PER_FREQ_STR *pl_data = tatl18_getGsmSberData(elem);
	if (0 == pl_data) {
		return TAT_ERROR_ON;
	}

	switch (elem->user_data) {
	case ACT_BER_2G_IN_CHANNEL_TYPE:

		DTH_SET_L8(elem, value, pl_sb->psk_slots_channel_type,
			   vl_result);
		break;

	case ACT_BER_2G_IN_RX_SLOT_PATTERN:

		DTH_SET_H8(elem, value, pl_sb->rx_slot_pattern_tx_slot_pattern,
			   vl_result);
		break;

	case ACT_BER_2G_IN_CODED_MODE:

		DTH_SET_UNSIGNED(elem, value, pl_sb->coded_mode, vl_result);
		break;

	case ACT_BER_2G_IN_CODEC:

		DTH_SET_L8(elem, value, pl_sb->fill2_codec, vl_result);
		break;

	case ACT_BER_2G_IN_NBR_BLOCK_BITS:

		DTH_SET_UNSIGNED(elem, value, pl_sb->num_of_tested_bits,
				 vl_result);
		break;

	case ACT_BER_2G_IN_TX_SLOT_PATTERN:

		DTH_SET_L8(elem, value, pl_sb->rx_slot_pattern_tx_slot_pattern,
			   vl_result);
		break;

	case ACT_BER_2G_IN_MODULATION:

		DTH_SET_H8(elem, value, pl_sb->psk_slots_channel_type,
			   vl_result);
		break;

	case ACT_BER_2G_IN_TX_DATA_TYPE:

        DTH_SET_L8(elem, value, pl_sb->fill3_transmit_data_type, vl_result);
        break;

    case ACT_BER_2G_IN_RX_LEVEL:

			pl_array = (struct dth_array *)value;
        vl_page = tatl20_06pageNumber(elem);
        if ( (vl_page >= 0) && (vl_page < SBER2G_NB_RESULTS) )
        {
				SetDthArray(elem, pl_array, sizeof(float),
                v_tatrf_sber2g_rx_levels[vl_page], sizeof(float), 8);
        }
        else
        {
				vl_result = TAT_ERROR_MISC;
			}
        break;

    case ACT_BER_2G_IN_TX_LEVEL:

        pl_array = (struct dth_array*)value;
        vl_page = tatl20_06pageNumber(elem);
        if ( (vl_page >= 0) && (vl_page < SBER2G_NB_RESULTS) )
        {
            SetDthArray(elem, pl_array, sizeof(uint16),
                v_tatrf_sber2g_tx_levels[vl_page], sizeof(uint16), 8);
        }
        else
        {
            vl_result = TAT_ERROR_MISC;
		}
		break;

	case ACT_BER_2G_IN_BAND:

		DTH_SET_UNSIGNED(elem, value, pl_data->rf_band, vl_result);
		break;

	case ACT_BER_2G_IN_ARFCN:

		DTH_SET_UNSIGNED(elem, value, pl_sb->ARFCN_val, vl_result);
        break;
    case ACT_BER_2G_IN_TCH_ARFCN:

		DTH_SET_UNSIGNED(elem, value, pl_data->arfcn, vl_result);
		break;

	default:
		/* Invalid request code */
		SYSLOG(LOG_ERR, "not a BER 2G input: %d", elem->user_data);
		vl_result = TAT_BAD_REQ;
	}

	return vl_result;
}

int tatl18_setGsmSberInfo(int vp_index, int vp_slot,
			  C_TEST_SB_GSM_SINGLE_BER_STR * pl_sb,
    GSM_SBER_INFO_PER_FREQ_STR *pl_sber_info, float *pl_rx_lvl,
    uint16 vp_tx_lvl)
{
	int vl_result = 0;
	ASSERT((vp_slot >= 0)
	       && (vp_slot < 8), "ber gsm slot number out of range");

	u8 rx_slot =
	    GetU8InU16(MSB_POSS, pl_sb->rx_slot_pattern_tx_slot_pattern);
	u8 tx_slot =
	    GetU8InU16(LSB_POSS, pl_sb->rx_slot_pattern_tx_slot_pattern);
	u8 mask = 1 << vp_slot;

	pl_sber_info->init_rx_level[vp_slot].rx_pwr_level = 0;
	pl_sber_info->tx_level_pcl[vp_slot].tx_level = 0U;

    if ( (rx_slot & mask) != 0 )
    {
		SYSLOGSTR(LOG_INFO, "set rx level[%d] to %f dBm\n", vp_slot,
            *pl_rx_lvl);
		pl_sber_info->init_rx_level[vp_slot].rx_pwr_level =
            RF_Q(*pl_rx_lvl, SBER2G_QUOT_RX_LEVEL);

        *pl_rx_lvl = RF_UNQ(
            pl_sber_info->init_rx_level[vp_slot].rx_pwr_level,
			   SBER2G_QUOT_RX_LEVEL);
    }
    
    if ( (tx_slot & mask) != 0 )
    {
        SYSLOGSTR(LOG_INFO, "set tx level[%d] to %u PCL\n", vp_slot,
            vp_tx_lvl);
        pl_sber_info->tx_level_pcl[vp_slot].tx_level = vp_tx_lvl;
	}

	return vl_result;
}

int tatl18_getResults(struct dth_element *elem, void *value)
{
	int vl_result = 0;

	C_TEST_SB_GSM_SINGLE_BER_RESULTS_STR *pl_results =
	    tatl18_getGsmSberResults(elem);
	if (NULL == pl_results) {
		return TAT_ERROR_ON;
	}

	GSM_BER_RES_INFO_STR *pl_data = tatl18_getGsmSberResultsData(elem);
	if (NULL == pl_data) {
		return TAT_ERROR_ON;
	}

	switch (elem->user_data) {
	case ACT_BER_2G_OUT_CLASS_IB_ERR:

		DTH_GET_UNSIGNED(pl_data->rber_Ib_coded_error_bits, elem, value,
				 vl_result);
		break;

	case ACT_BER_2G_OUT_CLASS_IB_BITS:

		DTH_GET_UNSIGNED(pl_data->num_of_rber_Ib_coded_bits, elem,
				 value, vl_result);
		break;

	case ACT_BER_2G_OUT_CLASS_IB_RBER:

		DTH_GET_FLOAT(tatl20_03divide(pl_data->rber_Ib_coded_error_bits,
					      pl_data->
					      num_of_rber_Ib_coded_bits, 0.0),
			      elem, value, vl_result);
		break;

	case ACT_BER_2G_OUT_CLASS_II_ERR:

		DTH_GET_UNSIGNED(pl_data->rber_II_coded_error_bits, elem,
				 value, vl_result);
		break;

	case ACT_BER_2G_OUT_CLASS_II_BITS:

		DTH_GET_UNSIGNED(pl_data->num_of_rber_II_coded_bits, elem,
				 value, vl_result);
		break;

	case ACT_BER_2G_OUT_CLASS_II_RBER:

		DTH_GET_FLOAT(tatl20_03divide(pl_data->rber_II_coded_error_bits,
					      pl_data->
					      num_of_rber_II_coded_bits, 0.0),
			      elem, value, vl_result);
		break;

	case ACT_BER_2G_OUT_BLER_ERR:

		DTH_GET_UNSIGNED(pl_data->false_blocks, elem, value, vl_result);
		break;

	case ACT_BER_2G_OUT_BLER_BLOCK:

		DTH_GET_UNSIGNED(pl_data->meas_blocks, elem, value, vl_result);
		break;

	case ACT_BER_2G_OUT_BLER:

		DTH_GET_FLOAT(tatl20_03divide(pl_data->false_blocks,
					      pl_data->meas_blocks,
					      0.0), elem, value, vl_result);
		break;

	case ACT_BER_2G_OUT_BER_ERR:

		DTH_GET_UNSIGNED(pl_data->ber_uncoded_error_bits, elem, value,
				 vl_result);
		break;

	case ACT_BER_2G_OUT_BER_BITS:

		DTH_GET_UNSIGNED(pl_data->num_of_ber_uncoded_bits, elem, value,
				 vl_result);
		break;

	case ACT_BER_2G_OUT_BER:

		DTH_GET_FLOAT(tatl20_03divide(pl_data->ber_uncoded_error_bits,
					      pl_data->num_of_ber_uncoded_bits,
					      0.0), elem, value, vl_result);
		break;

	case ACT_BER_2G_OUT_STATUS:

		DTH_GET_L8(pl_results->fill1_status, elem, value, vl_result);
		break;

	case ACT_BER_2G_OUT_RX_AVERAGE_POWER:

		DTH_GET_QUOT(elem,
			     value,
			     pl_data->avg_pwr,
			     SBER2G_QUOT_RX_AVERAGE_POWER, vl_result);
		break;

	case ACT_BER_2G_OUT_SNR:

		DTH_GET_SIGNED(pl_data->avg_snr, elem, value, vl_result);
	   break;

	default:
		/* Invalid request code */
		vl_result = TAT_BAD_REQ;
	}

	return vl_result;
}

C_TEST_SB_GSM_SINGLE_BER_STR *tatl18_getGsmSber(struct dth_element * elem)
{
	C_TEST_SB_GSM_SINGLE_BER_STR *pl_sb = NULL;
	int page = tatl20_06pageNumber(elem);

	if ((0 <= page) && (SBER2G_NB_RESULTS > page)) {
		pl_sb = &v_tatrf_sber2g[page];
	} else {
		SYSLOG(LOG_ERR, "index out of bounds: %d", page);
	}

	return pl_sb;
}

GSM_SBER_INFO_PER_FREQ_STR *tatl18_getGsmSberData(struct dth_element * elem)
{
	GSM_SBER_INFO_PER_FREQ_STR *pl_data = NULL;

	int page = tatl20_06pageNumber(elem);

	if ((0 <= page) && (SBER2G_NB_RESULTS > page)) {
		pl_data = &v_tatrf_sber2g_data[page];
	} else {
		SYSLOG(LOG_ERR, "index out of bounds: %d", page);
	}

	return pl_data;
}

C_TEST_SB_GSM_SINGLE_BER_RESULTS_STR *tatl18_getGsmSberResults(struct
							       dth_element *
							       elem)
{
	C_TEST_SB_GSM_SINGLE_BER_RESULTS_STR *pl_result = NULL;

	int page = tatl20_06pageNumber(elem);

	if ((0 <= page) && (SBER2G_NB_RESULTS > page)) {
		pl_result = &v_tatrf_sber2g_results[page];
	} else {
		SYSLOG(LOG_ERR, "index out of bounds: %d", page);
	}

	return pl_result;
}

GSM_BER_RES_INFO_STR *tatl18_getGsmSberResultsData(struct dth_element * elem)
{
	GSM_BER_RES_INFO_STR *pl_result = NULL;

	int page = tatl20_06pageNumber(elem);

	if ((0 <= page) && (SBER2G_NB_RESULTS > page)) {
		pl_result = &v_tatrf_sber2g_results_data[page];
	} else {
		SYSLOG(LOG_ERR, "index out of bounds: %d", page);
	}

	return pl_result;
}

int tatl18_store(void)
{
	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	SYSLOG(LOG_INFO, "Store SBER 2G");

	STORE_COMMAND_LIST_REQ_STR *pl_req = tatrf_isi_msg_begin(&vl_msg_info,
								 PN_DSP_COMMON_TEST,
								 STORE_COMMAND_LIST_REQ,
								 num_sub_blocks);
	if (NULL == pl_req) {
		return TAT_ISI_HANDLER_ERROR;
	}

	/* pass all subblocks in one call */
	pl_req->num_rem_sub_blocks = 0;
	pl_req->fill1 = 0x55AA;

	/* add SYNC subblock first */
	C_TEST_SB_GSM_SINGLE_BER_SYNC_STR *pl_sync =
	    tatrf_isi_msg_add_sb_copy(&vl_msg_info,
				      C_TEST_SB_GSM_SINGLE_BER_SYNC,
				      &v_tatrf_sber2g_sync,
				      SIZE_C_TEST_SB_GSM_SINGLE_BER_SYNC_STR);
	if (NULL == pl_sync) {
		return TAT_ISI_HANDLER_ERROR;
	}

	/* add one SINGLE_BER subblock per page where the RF band is set */
	C_TEST_SB_GSM_SINGLE_BER_STR *pl_ber = v_tatrf_sber2g, *pl_isi_ber =
	    NULL;
	int i, slot;
	int vl_Error = 0;
	for (i = 0; (i < SBER2G_NB_RESULTS) && (0 == vl_Error); i++) {
		pl_ber = v_tatrf_sber2g + i;
		pl_ber->num_of_freq = 0;

        if ( v_tatrf_sber2g_data[i].rf_band != INFO_NO_GSM )
        {
			SYSLOG(LOG_INFO, "Store slot %d", i + 1);
            for (slot = 0; (slot < 8) && (0 == vl_Error); slot++)
            {
                vl_Error = tatl18_setGsmSberInfo(i, slot, pl_ber,
                    v_tatrf_sber2g_data + i,
                    &v_tatrf_sber2g_rx_levels[i][slot],
                    v_tatrf_sber2g_tx_levels[i][slot]);
            }

            if ( 0 == vl_Error )
            {
                if ( (ACT_BER_ONE_SB_PER_FREQ == v_tatrf_sber2g_msg_format) ||
                     (NULL == pl_isi_ber) )
                {
					/* append SINGLE_BER subblock */
					pl_isi_ber =
					    tatrf_isi_msg_add_sb_copy
					    (&vl_msg_info,
					     C_TEST_SB_GSM_SINGLE_BER, pl_ber,
					     SIZE_C_TEST_SB_GSM_SINGLE_BER_STR);
				}

				if (NULL != pl_isi_ber) {
					pl_isi_ber->num_of_freq++;

					tatrf_isi_append_data(&vl_msg_info,
							      (u16
							       *)
							      (v_tatrf_sber2g_data
							       + i),
							      SIZE_GSM_SBER_INFO_PER_FREQ_STR
							      / sizeof(uint16));
				} else {
					vl_Error = TAT_ISI_HANDLER_ERROR;
				}
			}
		}
	}

	STORE_COMMAND_LIST_RESP_STR *pl_resp = NULL;
	if (0 == vl_Error) {
#ifdef STUB_SINGLE_BER
		tatrf_isi_dump(&vl_msg_info, "Store BER 2G (stubbed)",
			       LOG_INFO);

		/* build response from scratch */
		tatrf_isi_init(&vl_msg_info);

		pl_resp =
		    tatrf_isi_msg_begin_no_sb(&vl_msg_info, PN_DSP_COMMON_TEST,
					      STORE_COMMAND_LIST_RESP);

        u16 vl_dutState = tatl20_07getDutState();
        if ( vl_dutState == C_TEST_OFF )
        {
			pl_resp->fill1_status = MAKE16(0xAA, C_TEST_OK);
            tatl20_09setDutState(C_TEST_READY);
        }
        else
        {
            pl_resp->fill1_status = MAKE16(0xAA, C_TEST_WRONG_STATE);
		}

		tatrf_isi_dump(&vl_msg_info, "Store BER 2G response (stubbed)",
			       LOG_INFO);
#else
		vl_Error = tatrf_isi_send(&vl_msg_info, "Store BER 2G");
		if (TAT_OK(vl_Error)) {
			/* message successfully sent. waiting for response */
			pl_resp = tatrf_isi_read_simple(&vl_msg_info,
							STORE_COMMAND_LIST_RESP,
							"Store BER 2G response",
							&vl_Error);
		}
#endif

		if (NULL != pl_resp) {
			v_tatrf_sber2g_cmd_status =
			    LOWBYTE(pl_resp->fill1_status);
		}
	}

	return vl_Error;
}

int tatl18_readResultSubblocks(struct tatrf_isi_msg_t *pp_msg_info)
{
	int vl_Error = 0;

	/* extract each BER_RESULT and copy to local results.
	   skip pages where the band was not defined in subblock of request.
	 */
	C_TEST_SB_GSM_SINGLE_BER_RESULTS_STR *pl_ber_res = NULL;
	int index_page = 0, index_res = 0;
	while ((index_page < SBER2G_NB_RESULTS) && (0 == vl_Error)) {
		while ((v_tatrf_sber2g_data[index_page].rf_band == INFO_NO_GSM)
		       && (index_page < SBER2G_NB_RESULTS)) {
			index_page++;
		}

		if (index_page < SBER2G_NB_RESULTS) {
			SYSLOGSTR(LOG_DEBUG, "extracting results for page %d\n",
				  index_page + 1);

			if ((ACT_BER_ONE_SB_PER_FREQ ==
			     v_tatrf_sber2g_msg_format)
			    || (NULL == pl_ber_res)) {
				pl_ber_res =
				    tatrf_isi_find_sb(pp_msg_info,
						      C_TEST_SB_GSM_SINGLE_BER_RESULTS,
						      pl_ber_res);

				SYSLOGSTR(LOG_DEBUG,
					  "read one more SB_GSM_SINGLE_BER_RESULTS=%p\n",
					  pl_ber_res);
				index_res = 0;
			}

			if (NULL != pl_ber_res) {
				if (ACT_BER_ONE_SB_PER_FREQ ==
				    v_tatrf_sber2g_msg_format) {
					/* case one subblock with one result */
					memcpy(v_tatrf_sber2g_results +
					       index_page, pl_ber_res,
					       SIZE_C_TEST_SB_GSM_SINGLE_BER_RESULTS_STR);

					if (pl_ber_res->num_of_res == 1) {
						memcpy
						    (v_tatrf_sber2g_results_data
						     + index_page,
						     (GSM_BER_RES_INFO_STR *)
						     pl_ber_res->
						     array_of_results,
						     SIZE_GSM_BER_RES_INFO_STR);
					} else {
						SYSLOG(LOG_ERR,
						       "SB_GSM_SINGLE_BER_RESULTS should have one and only one result");
						vl_Error =
						    TAT_ERROR_NOT_MATCHING_MSG;
					}
				} else {
					/* case a unique subblock with many results */
					SYSLOGSTR(LOG_DEBUG,
						  "extract subblock result at index=%d/%u\n",
						  index_res + 1,
						  pl_ber_res->num_of_res);

					if (pl_ber_res->num_of_res > index_res) {
						memcpy
						    (v_tatrf_sber2g_results_data
						     + index_page,
						     ((GSM_BER_RES_INFO_STR *)
						      pl_ber_res->
						      array_of_results) +
						     index_res,
						     SIZE_GSM_BER_RES_INFO_STR);

						index_res++;
					} else {
						SYSLOG(LOG_ERR,
						       "not enough GSM_BER_RES_INFO");
						vl_Error =
						    TAT_ERROR_NOT_MATCHING_MSG;
					}
				}

				index_page++;
			} else {
				SYSLOG(LOG_ERR,
				       "not enough SB_GSM_SINGLE_BER_RESULTS or missing");
				vl_Error = TAT_ERROR_NOT_MATCHING_MSG;
			}
		}
	}

	return vl_Error;
}

#ifdef STUB_SINGLE_BER

int tatl18_askResults(struct tatrf_isi_msg_t *pp_msg_info)
{
	tatrf_isi_dump(pp_msg_info, "Get BER 2G results (stubbed)", LOG_INFO);

	/* build response from scratch */
	tatrf_isi_init(pp_msg_info);

	GET_RESULTS_RESP_STR *pl_msg = tatrf_isi_msg_begin(pp_msg_info,
        PN_DSP_COMMON_TEST, GET_RESULTS_RESP, numberOfSubBlocks);

    u16 vl_dutState = tatl20_07getDutState();
    if (vl_dutState == C_TEST_READY)
    {
        if ( tatl20_08getStatus() > 0 )
			pl_msg->fill1_status = MAKE16(0xAA, C_TEST_OK);
		else
			pl_msg->fill1_status = MAKE16(0xAA, C_TEST_FAILED);
	} else {
		pl_msg->fill1_status = MAKE16(0xAA, C_TEST_WRONG_STATE);
	}

	pl_msg->fill2_is_remaining_sb = 0;

	int vl_Error = 0;
	if (LOWBYTE(pl_msg->fill1_status) == C_TEST_OK) {
		C_TEST_SB_GSM_SINGLE_BER_RESULTS_STR *pl_sb = NULL;
		GSM_BER_RES_INFO_STR vl_sberResult;
		int index_page = -1;
		while ((index_page < SBER2G_NB_RESULTS) && (0 == vl_Error)) {
			do {
				index_page++;
			}
			while ((v_tatrf_sber2g_data[index_page].rf_band ==
				INFO_NO_GSM)
			       && (index_page < SBER2G_NB_RESULTS));

			if (index_page >= SBER2G_NB_RESULTS)
				break;

			if ((ACT_BER_ONE_SB_PER_FREQ ==
			     v_tatrf_sber2g_msg_format) || (NULL == pl_sb)) {
				pl_sb =
				    tatrf_isi_msg_add_sb(pp_msg_info,
							 C_TEST_SB_GSM_SINGLE_BER_RESULTS);
				pl_sb->fill1_status = 0xAA00;	/* C_TEST_OK */
				pl_sb->num_of_res = 0;
			}

			pl_sb->num_of_res++;

			memset(&vl_sberResult, 0, SIZE_GSM_BER_RES_INFO_STR);

			/* arbitrary assign different values per page */
			switch (index_page % SBER2G_NB_RESULTS) {
			case 0:
				vl_sberResult.num_of_rber_Ib_coded_bits = 2048;
				vl_sberResult.num_of_rber_II_coded_bits = 1065;
				vl_sberResult.num_of_ber_uncoded_bits = 745;
				vl_sberResult.rber_Ib_coded_error_bits = 106;
				vl_sberResult.rber_II_coded_error_bits = 204;
				vl_sberResult.ber_uncoded_error_bits = 46;
				vl_sberResult.meas_blocks = 189;
				vl_sberResult.false_blocks = 34;
                vl_sberResult.avg_snr = -12;
                vl_sberResult.avg_pwr = RF_Q(5.4, SBER2G_QUOT_RX_AVERAGE_POWER);
				break;

			case 1:
				vl_sberResult.num_of_rber_Ib_coded_bits = 104;
				vl_sberResult.num_of_rber_II_coded_bits = 365;
				vl_sberResult.num_of_ber_uncoded_bits = 984;
				vl_sberResult.rber_Ib_coded_error_bits = 10;
				vl_sberResult.rber_II_coded_error_bits = 40;
				vl_sberResult.ber_uncoded_error_bits = 36;
				vl_sberResult.meas_blocks = 69;
				vl_sberResult.false_blocks = 7;
                vl_sberResult.avg_snr = 2;
                vl_sberResult.avg_pwr = RF_Q(6.78, SBER2G_QUOT_RX_AVERAGE_POWER);
				break;

			case 2:
				vl_sberResult.num_of_rber_Ib_coded_bits = 4068;
				vl_sberResult.num_of_rber_II_coded_bits = 8049;
				vl_sberResult.num_of_ber_uncoded_bits = 2064;
				vl_sberResult.rber_Ib_coded_error_bits = 105;
				vl_sberResult.rber_II_coded_error_bits = 307;
				vl_sberResult.ber_uncoded_error_bits = 608;
				vl_sberResult.meas_blocks = 1047;
				vl_sberResult.false_blocks = 145;
                vl_sberResult.avg_snr = -15;
                vl_sberResult.avg_pwr = RF_Q(12.8, SBER2G_QUOT_RX_AVERAGE_POWER);
				break;

			case 3:
				vl_sberResult.num_of_rber_Ib_coded_bits = 3697;
				vl_sberResult.num_of_rber_II_coded_bits = 7640;
				vl_sberResult.num_of_ber_uncoded_bits = 9405;
				vl_sberResult.rber_Ib_coded_error_bits = 1043;
				vl_sberResult.rber_II_coded_error_bits = 450;
				vl_sberResult.ber_uncoded_error_bits = 973;
				vl_sberResult.meas_blocks = 104;
				vl_sberResult.false_blocks = 24;
                vl_sberResult.avg_snr = 7;
                vl_sberResult.avg_pwr = RF_Q(14.9, SBER2G_QUOT_RX_AVERAGE_POWER);
				break;
			}

			vl_Error =
			    tatrf_isi_append_data(pp_msg_info,
						  (u16 *) & vl_sberResult,
						  SIZE_GSM_BER_RES_INFO_STR /
						  sizeof(uint16));
		}
	}

	if (!TAT_OK(vl_Error))
		return vl_Error;

	tatl17_12marsh_msg(pp_msg_info, ISI_MARSH);
	tatl17_12marsh_msg(pp_msg_info, ISI_UNMARSH);

	tatrf_isi_dump(pp_msg_info, "Get BER 2G results response (stubbed)",
		       LOG_INFO);

	/* parse pseudo response */
	v_tatrf_sber2g_cmd_status = LOWBYTE(pl_msg->fill1_status);

	if (C_TEST_OK == v_tatrf_sber2g_cmd_status) {
		vl_Error = tatl18_readResultSubblocks(pp_msg_info);
	}

	return vl_Error;
}

#else

int tatl18_askResults(struct tatrf_isi_msg_t *pp_msg_info)
{
	/* send request */
	int vl_Error = tatrf_isi_send(pp_msg_info, "Get BER 2G results");
	if (!TAT_OK(vl_Error)) {
		return vl_Error;
	}

	/* message successfully sent. read response */
	GET_RESULTS_RESP_STR *pl_resp = tatrf_isi_read(pp_msg_info,
						       GET_RESULTS_RESP,
						       numberOfSubBlocks,
						       "Get BER 2G results response",
						       &vl_Error);
	if (NULL != pl_resp) {
		v_tatrf_sber2g_cmd_status = LOWBYTE(pl_resp->fill1_status);

		if (C_TEST_OK == v_tatrf_sber2g_cmd_status) {
			vl_Error = tatl18_readResultSubblocks(pp_msg_info);
		}
	}

	return vl_Error;
}

#endif

int tatl18_refreshResults(void)
{
	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	GET_RESULTS_REQ_STR *pl_req = tatrf_isi_msg_begin_no_sb(&vl_msg_info,
								PN_DSP_COMMON_TEST,
								GET_RESULTS_REQ);
	if (NULL == pl_req) {
		return TAT_ISI_HANDLER_ERROR;
	}

	int vl_Error = tatl18_askResults(&vl_msg_info);

	return vl_Error;
}

