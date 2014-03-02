/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   sglber3g.c
* \brief   routines to run Single Ended Bit Error Rate for WCDMA interface
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "rfdef.h"
#include "sglber3g.h"
#include "sglber.h"

#include "isimsg.h"
#ifdef STUB_SINGLE_BER
#include "isidump.h"
#endif
#include "misc.h"

/* Function tide to Single Ended BER 3G           */
/**************************************************/
int DthRf_SingleBer3G_get(struct dth_element *elem, void *value)
{
	return tatl19_02SingleBer3G_get(elem, value);
}

int DthRf_SingleBer3G_set(struct dth_element *elem, void *value)
{
	return tatl19_01SingleBer3G_set(elem, value);
}

int DthRf_SingleBer3G_exec(struct dth_element *elem)
{
	return tatl19_00SingleBer3G_exec(elem);
}

/* INTERNAL GLOBALS */

/* Single BER 3G command code */
u32 v_tatrf_sber3g_run = ACT_BER3G_CMD_RESET;
u8 v_tatrf_sber3g_dtu_status = C_TEST_OFF;
u8 v_tatrf_sber3g_cmd_status = 0xFF;
u32 v_tatrf_sber3g_msg_format = ACT_BER_ONE_SB_PER_FREQ;

C_TEST_SB_WCDMA_SINGLE_BER_SYNC_STR v_tatrf_sber3g_sync;
C_TEST_SB_WCDMA_SINGLE_BER_STR v_tatrf_sber3g[SBER3G_NB_RESULTS];
WCDMA_SBER_INFO_PER_FREQ_STR v_tatrf_sber3g_data[SBER3G_NB_RESULTS];
C_TEST_SB_WCDMA_SINGLE_BER_RESULTS_STR v_tatrf_sber3g_results[SBER3G_NB_RESULTS];
WCDMA_SBER_RES_INFO_STR v_tatrf_sber3g_results_data[SBER3G_NB_RESULTS];

/* Initialize single BER 3G globals */
void tatl19_initMembers(int force);
int tatl19_setInputs(struct dth_element *elem, void *value);
int tatl19_getResults(struct dth_element *elem, void *value);

int tatl19_store(void);
int tatl19_refreshResults(void);

C_TEST_SB_WCDMA_SINGLE_BER_STR* tatl19_getWcdmaSber(struct dth_element *elem);
WCDMA_SBER_INFO_PER_FREQ_STR* tatl19_getWcdmaSberData(struct dth_element *elem);
C_TEST_SB_WCDMA_SINGLE_BER_RESULTS_STR* tatl19_getWcdmaSberResults(struct dth_element *elem);
WCDMA_SBER_RES_INFO_STR* tatl19_getWcdmaSberResultsData(struct dth_element *elem);

/* PUBLIC INTERFACE */

int tatl19_00SingleBer3G_exec(struct dth_element *elem)
{
    int vl_result = 0;

    tatl19_initMembers(0);

    SYSLOG(LOG_DEBUG, "enter SingleBer3G_exec(%s)", elem->path);

    switch ( elem->user_data )
    {
    case ACT_BER_3G:

        switch (v_tatrf_sber3g_run)
        {
        case ACT_BER3G_CMD_STORE:

            vl_result = tatl19_store();
            break;

        case ACT_BER3G_CMD_START:

            vl_result = tatl20_01start(&v_tatrf_sber3g_cmd_status);
            break;

        case ACT_BER3G_CMD_STOP:

            vl_result = tatl20_02stop(&v_tatrf_sber3g_cmd_status);
            break;

        case ACT_BER3G_CMD_RESULT:

            vl_result = tatl19_refreshResults();
            break;

        case ACT_BER3G_CMD_RESET:
  
            tatl19_initMembers(1);
            vl_result = tatl20_04clear(&v_tatrf_sber3g_cmd_status);
            break;

        default:
            SYSLOG(LOG_ERR, "Bad BER_3G command code %u", v_tatrf_sber3g_run);
        }
        break;

    case ACT_BER_3G_GET_STATUS:

        vl_result = tatl20_05refreshStatus(&v_tatrf_sber3g_dtu_status);
        break;

    default:
        vl_result = TAT_BAD_REQ;
    }

    SYSLOG(LOG_DEBUG, "exit SingleBer3G_exec(%s) with code %d",
        elem->path, vl_result);

    return vl_result;
}

int tatl19_01SingleBer3G_set(struct dth_element *elem, void *value)
{
    int vl_result = 0;
    u32 * pl_value= (u32*) value;
    tatl19_initMembers(0);

    SYSLOG(LOG_DEBUG, "enter SingleBer3G_set(%s)", elem->path);

    switch ( elem->user_data )
    {
    /* Commands */
    case ACT_BER_3G:
        
        DTH_SET_UNSIGNED(elem, value, v_tatrf_sber3g_run, vl_result);
        break;

    case ACT_BER_3G_IN_SEQUENCE_TYPE:

        DTH_SET_UNSIGNED(elem, value, v_tatrf_sber3g_msg_format, vl_result);
        break;

    /* Synchronization */
    case ACT_BER_3G_SYNC_IN_TESTER:

        DTH_SET_UNSIGNED(elem, value, v_tatrf_sber3g_sync.tester_tool, vl_result);
        break;

    case ACT_BER_3G_SYNC_IN_BAND:

        DTH_SET_UNSIGNED(elem, value, v_tatrf_sber3g_sync.rf_band, vl_result);
        break;

    case ACT_BER_3G_SYNC_IN_ARFCN:

        DTH_SET_UNSIGNED(elem, value, v_tatrf_sber3g_sync.dl_uarfcn, vl_result);
        break;

    case ACT_BER_3G_SYNC_IN_OVSF:

        DTH_SET_L8(elem, value, v_tatrf_sber3g_sync.fill1_2_channelization_code, vl_result);
        break;

    case ACT_BER_3G_SYNC_IN_UL_SCRAMBLING_CODE:

        DTH_SET_UNSIGNED(elem, value, v_tatrf_sber3g_sync.ul_scrambling_code, vl_result);
        break;

    case ACT_BER_3G_SYNC_IN_PRI_SCRAMBLING_CODE:

        DTH_SET_UNSIGNED(elem, value, v_tatrf_sber3g_sync.pri_scrambling_code, vl_result);
        break;

    case ACT_BER_3G_SYNC_IN_FRAME_NO:

        DTH_SET_UNSIGNED(elem, value, v_tatrf_sber3g_sync.frame_no, vl_result);
        break;

    case ACT_BER_3G_SYNC_DATA_TYPE:

        DTH_SET_L8(elem, value, v_tatrf_sber3g_sync.fill2_rx_data_type, vl_result);
        break;

    case ACT_BER_3G_SYNC_IN_TX_LEVEL:

        DTH_SET_QUOT(
            elem,
            value,
            vl_result,
            v_tatrf_sber3g_sync.tx_pwr_level,
            SBER3G_QUOT_TX_LEVEL
            );
        break;
    case ACT_BER_3G_SYNC_IN_RF_RX_PATH:

	switch (*(u32 *) value) {
		case 0:
			*pl_value=(u32)CTRL_RF_PATH1;
			SYSLOG(LOG_INFO, "path=main");
			DTH_SET_L8(elem, pl_value, v_tatrf_sber3g_sync.fill3_2_rf_rx_path, vl_result);
			break;

		case 1:
			*pl_value=(u32)CTRL_RF_PATH2;
			SYSLOG(LOG_INFO, "path=diversity");
			DTH_SET_L8(elem,pl_value,v_tatrf_sber3g_sync.fill3_2_rf_rx_path,vl_result);		
			break;

		default:
			SYSLOG(LOG_ERR, "invalid WCDMA path code");
			 vl_result = TAT_BAD_REQ;
		break;
		}

	break;
    default:
        vl_result = tatl19_setInputs(elem, value);
    }

    SYSLOG(LOG_DEBUG, "exit SingleBer3G_set(%s) with code %d",
        elem->path, vl_result);

    return vl_result;
}

int tatl19_02SingleBer3G_get(struct dth_element *elem, void *value)
{
    int vl_result = 0;

    tatl19_initMembers(0);

    SYSLOG(LOG_DEBUG, "enter SingleBer3G_get(%s)", elem->path);

    switch ( elem->user_data )
    {
    case ACT_BER_3G:

        DTH_GET_UNSIGNED(v_tatrf_sber3g_run, elem, value, vl_result);
        break;

    case ACT_BER_3G_IN_SEQUENCE_TYPE:

        DTH_GET_UNSIGNED(v_tatrf_sber3g_msg_format, elem, value, vl_result);
        break;

    case ACT_BER_3G_OUT_CMD_STATUS:

        DTH_GET_UNSIGNED(v_tatrf_sber3g_cmd_status, elem, value, vl_result);
        break;

    case ACT_BER_3G_OUT_DUT_STATUS:

        DTH_GET_UNSIGNED(v_tatrf_sber3g_dtu_status, elem, value, vl_result);
        break;

    default:
        vl_result = tatl19_getResults(elem, value);
    }

    SYSLOG(LOG_DEBUG, "exit SingleBer3G_get(%s) with code %d",
        elem->path, vl_result);

    return vl_result;
}

int tatl19_getResults(struct dth_element *elem, void *value)
{
    int vl_result = 0;

    C_TEST_SB_WCDMA_SINGLE_BER_RESULTS_STR *pl_results =
        tatl19_getWcdmaSberResults(elem);
    if ( NULL == pl_results )
    {
        return TAT_ERROR_ON;
    }

    WCDMA_SBER_RES_INFO_STR *pl_info = tatl19_getWcdmaSberResultsData(elem);
    if ( NULL == pl_info )
    {
        return TAT_ERROR_ON;
    }

    switch ( elem->user_data )
    {
    /* BER results */
    case ACT_BER_3G_OUT_BER_ERR:

        DTH_GET_UNSIGNED(pl_info->num_err_bits, elem, value, vl_result);
        break;

    case ACT_BER_3G_OUT_BER_BLOCK:

        DTH_GET_UNSIGNED(pl_info->num_bits_meas, elem, value, vl_result);
        break;

    case ACT_BER_3G_OUT_BER:
        
        DTH_GET_FLOAT(
            tatl20_03divide(
                pl_info->num_err_bits, 
                pl_info->num_bits_meas,
                0.0
            ),
            elem,
            value,
            vl_result
        );
        break;

    /* globals */
    case ACT_BER_3G_OUT_RX_AVERAGE_POWER:

        DTH_GET_QUOT(
            elem,
            value,
            pl_info->avg_rcvd_pwr,
            SBER3G_QUOT_AVERAGE_RX_PWR,
            vl_result
        );
        break;

    case ACT_BER_3G_OUT_AVERAGE_RSCP:

        DTH_GET_QUOT(
            elem,
            value,
            pl_info->avg_rscp_res,
            SBER3G_QUOT_AVERAGE_RSCP,
            vl_result
        );
        break;

    default:
        vl_result = TAT_BAD_REQ;
    }

    SYSLOG(LOG_DEBUG, "exit SingleBer3G_get(%s) with code %d",
        elem->path, vl_result);

    return vl_result;
}

void tatl19_initMembers(int force)
{
    static int v_tatrf_sber3g_initialized = 0;

    if ( !force && v_tatrf_sber3g_initialized ) {
        /* already done */
        return;
    }

    if ( 0 == force )
    {
        memset(&v_tatrf_sber3g_sync, 0, sizeof(C_TEST_SB_WCDMA_SINGLE_BER_SYNC_STR));
        memset(v_tatrf_sber3g, 0, sizeof(C_TEST_SB_WCDMA_SINGLE_BER_STR) * SBER3G_NB_RESULTS);
        memset(v_tatrf_sber3g_data, 0, sizeof(WCDMA_SBER_INFO_PER_FREQ_STR) * SBER3G_NB_RESULTS);
    }

    memset(v_tatrf_sber3g_results, 0, sizeof(C_TEST_SB_WCDMA_SINGLE_BER_RESULTS_STR) * SBER3G_NB_RESULTS);
    memset(v_tatrf_sber3g_results_data, 0, sizeof(WCDMA_SBER_RES_INFO_STR) * SBER3G_NB_RESULTS);

    /* now initialized */
    v_tatrf_sber3g_initialized = 1;
}

int tatl19_setInputs(struct dth_element *elem, void *value)
{
    int vl_result = 0;

    C_TEST_SB_WCDMA_SINGLE_BER_STR *pl_sb = tatl19_getWcdmaSber(elem);
    if ( 0 == pl_sb ) {
        return TAT_ERROR_ON;
    }

    WCDMA_SBER_INFO_PER_FREQ_STR *pl_data = tatl19_getWcdmaSberData(elem);
    if ( 0 == pl_data ) {
        return TAT_ERROR_ON;
    }

    switch ( elem->user_data )
    {
    /* DL channel */
    case ACT_BER_3G_IN_BAND:

         DTH_SET_UNSIGNED(elem, value, pl_data->rf_band, vl_result);
	 break;

    case ACT_BER_3G_IN_DL_UARFCN:

        DTH_SET_UNSIGNED(elem, value, pl_data->dl_uarfcn, vl_result);
        break;

    case ACT_BER_3G_IN_NBR_BITS:

        DTH_SET_UNSIGNED(elem, value, pl_sb->no_bits_to_be_tested, vl_result);
        break;

    /* UL channel */
    case ACT_BER_3G_IN_TX_LEVEL:

        DTH_SET_QUOT(
            elem,
            value,
            vl_result,
            pl_data->tx_level,
            SBER3G_QUOT_TX_LEVEL
            );
        break;

    default:
        vl_result = TAT_BAD_REQ;
    }

    return vl_result;
}

C_TEST_SB_WCDMA_SINGLE_BER_STR* tatl19_getWcdmaSber(struct dth_element *elem)
{
    C_TEST_SB_WCDMA_SINGLE_BER_STR* pl_sb = 0;
    int page = tatl20_06pageNumber(elem);

    if ( (0 <= page) && (SBER3G_NB_RESULTS > page) )
    {
        pl_sb = v_tatrf_sber3g + page;
    }
    else
    {
        SYSLOG(LOG_ERR, "index out of bounds");
    }

    return pl_sb;
}

C_TEST_SB_WCDMA_SINGLE_BER_RESULTS_STR* tatl19_getWcdmaSberResults(struct dth_element *elem)
{
    C_TEST_SB_WCDMA_SINGLE_BER_RESULTS_STR* pl_result = 0;
    int page = tatl20_06pageNumber(elem);

    if ( (0 <= page) && (SBER3G_NB_RESULTS > page) )
    {
        pl_result = v_tatrf_sber3g_results + page;
    }
    else
    {
        SYSLOG(LOG_ERR, "index out of bounds");
    }

    return pl_result;
}

WCDMA_SBER_INFO_PER_FREQ_STR* tatl19_getWcdmaSberData(struct dth_element *elem)
{
    WCDMA_SBER_INFO_PER_FREQ_STR *pl_info = 0;
    int page = tatl20_06pageNumber(elem);

    if ( (0 <= page) && (SBER3G_NB_RESULTS > page) )
    {
        pl_info = v_tatrf_sber3g_data + page;
    }
    else
    {
        SYSLOG(LOG_ERR, "index out of bounds");
    }

    return pl_info;
}

WCDMA_SBER_RES_INFO_STR* tatl19_getWcdmaSberResultsData(struct dth_element *elem)
{
    WCDMA_SBER_RES_INFO_STR *pl_result = 0;
    int page = tatl20_06pageNumber(elem);

    if ( (0 <= page) && (SBER3G_NB_RESULTS > page) )
    {
        pl_result = v_tatrf_sber3g_results_data + page;
    }
    else
    {
        SYSLOG(LOG_ERR, "index out of bounds");
    }

    return pl_result;
}

int tatl19_store(void)
{
    struct tatrf_isi_msg_t vl_msg_info;
    tatrf_isi_init(&vl_msg_info);

    STORE_COMMAND_LIST_REQ_STR* pl_req = tatrf_isi_msg_begin(&vl_msg_info,
        PN_DSP_COMMON_TEST, STORE_COMMAND_LIST_REQ, num_sub_blocks);
    if ( NULL == pl_req )
    {
        return TAT_ISI_HANDLER_ERROR;
    }

    /* pass all subblocks in one call */
    pl_req->num_rem_sub_blocks = 0;
    pl_req->fill1 = 0x55AA;

    /* add SYNC subblock first */
    C_TEST_SB_WCDMA_SINGLE_BER_SYNC_STR *pl_sync = tatrf_isi_msg_add_sb_copy(&vl_msg_info,
        C_TEST_SB_WCDMA_SINGLE_BER_SYNC, &v_tatrf_sber3g_sync, SIZE_C_TEST_SB_WCDMA_SINGLE_BER_SYNC_STR);
    if ( NULL == pl_sync )
    {
        return TAT_ISI_HANDLER_ERROR;
    }

    /* add one SINGLE_BER subblock per page where the RF band is set */
    C_TEST_SB_WCDMA_SINGLE_BER_STR *pl_ber = v_tatrf_sber3g, *pl_isi_ber=NULL;
    int i = 0;
    int vl_Error = 0;
    for ( i = 0; (i < SBER3G_NB_RESULTS) && (0 == vl_Error); i++ )
    {
        pl_ber = v_tatrf_sber3g + i;
        pl_ber->num_of_freq = 0;

        if ( v_tatrf_sber3g_data[i].rf_band != INFO_NO_WCDMA )
        {
            if ( (ACT_BER_ONE_SB_PER_FREQ == v_tatrf_sber3g_msg_format) ||
                 (NULL == pl_isi_ber) )
            {
		/* append SINGLE_BER subblock */
                pl_isi_ber = tatrf_isi_msg_add_sb_copy(&vl_msg_info,
                    C_TEST_SB_WCDMA_SINGLE_BER, pl_ber,
                    SIZE_C_TEST_SB_WCDMA_SINGLE_BER_STR);
            }

            if ( NULL != pl_isi_ber )
            {
                pl_isi_ber->num_of_freq ++;

                tatrf_isi_append_data(&vl_msg_info, (u16*)(v_tatrf_sber3g_data + i),
                    SIZE_WCDMA_SBER_INFO_PER_FREQ_STR / sizeof(uint16));
            }
            else
            {
                vl_Error = TAT_ISI_HANDLER_ERROR;
            }
        }
    }

    STORE_COMMAND_LIST_RESP_STR *pl_resp = NULL;
    if ( 0 == vl_Error )
    {
#ifdef STUB_SINGLE_BER
        tatrf_isi_dump(&vl_msg_info, "Store BER 3G (stubbed)", LOG_INFO);

        /* build response from scratch */
        tatrf_isi_init(&vl_msg_info);

        pl_resp = tatrf_isi_msg_begin_no_sb(&vl_msg_info, PN_DSP_COMMON_TEST,
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

        tatrf_isi_dump(&vl_msg_info, "Store BER 3G response (stubbed)", LOG_INFO);
#else
        vl_Error = tatrf_isi_send(&vl_msg_info, "Store BER 3G");
        if ( TAT_OK(vl_Error) )
       	{
            /* message successfully sent. waiting for response */
            pl_resp = tatrf_isi_read_simple(&vl_msg_info, 
                STORE_COMMAND_LIST_RESP, "Store BER 3G response", &vl_Error);
        }
#endif

        if ( NULL != pl_resp )
        {
            v_tatrf_sber3g_cmd_status = LOWBYTE(pl_resp->fill1_status);
        }
    }

    return vl_Error;
}

int tatl19_readResultSubblocks(struct tatrf_isi_msg_t *pp_msg_info)
{
    int vl_Error = 0;

    /* extract each BER_RESULT and copy to local results.
       skip pages where the band was not defined in subblock of request.
     */
    C_TEST_SB_WCDMA_SINGLE_BER_RESULTS_STR *pl_ber_res = NULL;
    int index_page = 0, index_res = 0;
    while ( (index_page < SBER3G_NB_RESULTS) && (0 == vl_Error) )
    {
        while ( (v_tatrf_sber3g_data[index_page].rf_band == INFO_NO_WCDMA)
            && (index_page < SBER3G_NB_RESULTS) )
        {
            index_page++;
        }

        if ( index_page < SBER3G_NB_RESULTS )
        {
            SYSLOGSTR(LOG_DEBUG, "extracting results for page %d\n", index_page+1);

            if ( (ACT_BER_ONE_SB_PER_FREQ == v_tatrf_sber3g_msg_format) ||
                 (NULL == pl_ber_res) )
            {
                pl_ber_res = tatrf_isi_find_sb(pp_msg_info,
                    C_TEST_SB_WCDMA_SINGLE_BER_RESULTS, pl_ber_res);

                SYSLOGSTR(LOG_DEBUG, "read one more SB_WCDMA_SINGLE_BER_RESULTS=%p\n", pl_ber_res);
                index_res=0;
            }

            if ( NULL != pl_ber_res )
            {
                if ( ACT_BER_ONE_SB_PER_FREQ == v_tatrf_sber3g_msg_format )
                {
                    /* case one subblock with one result */
                    memcpy(v_tatrf_sber3g_results + index_page, pl_ber_res,
                        SIZE_C_TEST_SB_WCDMA_SINGLE_BER_RESULTS_STR);

                    if ( pl_ber_res->num_of_result == 1 )
                    {
                        memcpy(v_tatrf_sber3g_results_data + index_page,
                            (WCDMA_SBER_RES_INFO_STR*)pl_ber_res->array_of_results,
                            SIZE_WCDMA_SBER_RES_INFO_STR);
                    }
                    else
                    {
                        SYSLOG(LOG_ERR, "SB_WCDMA_SINGLE_BER_RESULTS should have one and only one result");
                        vl_Error = TAT_ERROR_NOT_MATCHING_MSG;
                    }
                }
                else
                {
                    /* case a unique subblock with many results */
                    SYSLOGSTR(LOG_DEBUG, "extract subblock result at index=%d/%u\n",
                        index_res+1, pl_ber_res->num_of_result);

                    if ( pl_ber_res->num_of_result > index_res )
                    {
                        memcpy(v_tatrf_sber3g_results_data + index_page,
                            ((WCDMA_SBER_RES_INFO_STR*)pl_ber_res->array_of_results) + index_res,
                            SIZE_WCDMA_SBER_RES_INFO_STR);

                        index_res++;
                    }
                    else
                    {
                        SYSLOG(LOG_ERR, "not enough WCDMA_SBER_RES_INFO");
                        vl_Error = TAT_ERROR_NOT_MATCHING_MSG;
                    }
                }

                index_page++;
            }
            else
            {
                SYSLOG(LOG_ERR, "not enough SB_WCDMA_SINGLE_BER_RESULTS or missing");
                vl_Error = TAT_ERROR_NOT_MATCHING_MSG;
            }
        }
    }

    return vl_Error;
}

#ifdef STUB_SINGLE_BER

int tatl19_askResults(struct tatrf_isi_msg_t *pp_msg_info)
{
    tatrf_isi_dump(pp_msg_info, "Get BER 3G results (stubbed)", LOG_INFO);

    /* build response from scratch */
    tatrf_isi_init(pp_msg_info);

    GET_RESULTS_RESP_STR *pl_msg = tatrf_isi_msg_begin(pp_msg_info,
        PN_DSP_COMMON_TEST, GET_RESULTS_RESP, numberOfSubBlocks);

    u16 vl_dutState = tatl20_07getDutState();
    if ( vl_dutState == C_TEST_READY )
    {
        if ( tatl20_08getStatus() > 0 )
            pl_msg->fill1_status = MAKE16(0xAA, C_TEST_OK);
        else
            pl_msg->fill1_status = MAKE16(0xAA, C_TEST_FAILED);
    }
    else
    {
        pl_msg->fill1_status = MAKE16(0xAA, C_TEST_WRONG_STATE);
    }

    pl_msg->fill2_is_remaining_sb = 0;

    int vl_Error = 0;
    if ( LOWBYTE(pl_msg->fill1_status) == C_TEST_OK )
    {
        C_TEST_SB_WCDMA_SINGLE_BER_RESULTS_STR *pl_sb = NULL;
        WCDMA_SBER_RES_INFO_STR vl_sberResult;
        int index_page = -1;
        while ( (index_page < SBER3G_NB_RESULTS) && (0 == vl_Error) )
        {
            do
            {
                index_page++;
            }
            while ( (v_tatrf_sber3g_data[index_page].rf_band == INFO_NO_WCDMA)
                && (index_page < SBER3G_NB_RESULTS) );

            if ( index_page >= SBER3G_NB_RESULTS )
                break;

            if ( (ACT_BER_ONE_SB_PER_FREQ == v_tatrf_sber3g_msg_format) ||
                 (NULL == pl_sb) )
            {
                pl_sb = tatrf_isi_msg_add_sb(pp_msg_info,
                    C_TEST_SB_WCDMA_SINGLE_BER_RESULTS);
                pl_sb->fill1_status = 0xAA00; /* C_TEST_OK */
                pl_sb->num_of_result = 0;
            }

            pl_sb->num_of_result++;

            memset(&vl_sberResult, 0, SIZE_WCDMA_SBER_RES_INFO_STR);

            /* arbitrary assign different values per page */
            switch (index_page % SBER3G_NB_RESULTS)
            {
            case 0:
                vl_sberResult.num_bits_meas = 1046;
                vl_sberResult.num_err_bits = 20;
                vl_sberResult.avg_rcvd_pwr = TATRF_Q(14.6, SBER3G_QUOT_AVERAGE_RX_PWR);
                vl_sberResult.avg_rscp_res = TATRF_Q(-4.6, SBER3G_QUOT_AVERAGE_RSCP);
                break;

            case 1:
                vl_sberResult.num_bits_meas = 3165;
                vl_sberResult.num_err_bits = 104;
                vl_sberResult.avg_rcvd_pwr = TATRF_Q(7.9, SBER3G_QUOT_AVERAGE_RX_PWR);
                vl_sberResult.avg_rscp_res = TATRF_Q(2.45, SBER3G_QUOT_AVERAGE_RSCP);
                break;

            case 2:
                vl_sberResult.num_bits_meas = 760;
                vl_sberResult.num_err_bits = 36;
                vl_sberResult.avg_rcvd_pwr = TATRF_Q(-4.98, SBER3G_QUOT_AVERAGE_RX_PWR);
                vl_sberResult.avg_rscp_res = TATRF_Q(-1.6, SBER3G_QUOT_AVERAGE_RSCP);
                break;

            case 3:
                vl_sberResult.num_bits_meas = 9046;
                vl_sberResult.num_err_bits = 92;
                vl_sberResult.avg_rcvd_pwr = TATRF_Q(6.4, SBER3G_QUOT_AVERAGE_RX_PWR);
                vl_sberResult.avg_rscp_res = TATRF_Q(7, SBER3G_QUOT_AVERAGE_RSCP);
                break;

            case 4:
                vl_sberResult.num_bits_meas = 406;
                vl_sberResult.num_err_bits = 14;
                vl_sberResult.avg_rcvd_pwr = TATRF_Q(9.3, SBER3G_QUOT_AVERAGE_RX_PWR);
                vl_sberResult.avg_rscp_res = TATRF_Q(16.48, SBER3G_QUOT_AVERAGE_RSCP);
                break;
            }

            vl_Error = tatrf_isi_append_data(pp_msg_info, (u16*)&vl_sberResult,
                SIZE_WCDMA_SBER_RES_INFO_STR / sizeof(uint16));
        }
    }

    if ( !TAT_OK(vl_Error) )
        return vl_Error;

    tatl17_12marsh_msg(pp_msg_info, ISI_MARSH);
    tatl17_12marsh_msg(pp_msg_info, ISI_UNMARSH);

    tatrf_isi_dump(pp_msg_info, "Get BER 3G results response (stubbed)", LOG_INFO);

    /* parse pseudo response */
    v_tatrf_sber3g_cmd_status = LOWBYTE(pl_msg->fill1_status);

    if ( C_TEST_OK == v_tatrf_sber3g_cmd_status )
    {
        vl_Error = tatl19_readResultSubblocks(pp_msg_info);
    }

    return vl_Error;
}

#else

int tatl19_askResults(struct tatrf_isi_msg_t *pp_msg_info)
{
    /* send request */
    int vl_Error = tatrf_isi_send(pp_msg_info, "Get BER 3G results");
    if ( !TAT_OK(vl_Error) )
   	{
        return vl_Error;
    }

    /* message successfully sent. read response */
    GET_RESULTS_RESP_STR *pl_resp = tatrf_isi_read(pp_msg_info, 
        GET_RESULTS_RESP, numberOfSubBlocks, "Get BER 3G results response",
        &vl_Error);
    if ( NULL != pl_resp )
    {
        v_tatrf_sber3g_cmd_status = LOWBYTE(pl_resp->fill1_status);

        if ( C_TEST_OK == v_tatrf_sber3g_cmd_status )
        {
            vl_Error = tatl19_readResultSubblocks(pp_msg_info);
        }
    }

    return vl_Error;
}

#endif

int tatl19_refreshResults(void)
{
    struct tatrf_isi_msg_t vl_msg_info;
    tatrf_isi_init(&vl_msg_info);

    GET_RESULTS_REQ_STR* pl_req = tatrf_isi_msg_begin_no_sb(&vl_msg_info,
        PN_DSP_COMMON_TEST, GET_RESULTS_REQ);
    if ( NULL == pl_req )
    {
        return TAT_ISI_HANDLER_ERROR;
    }

    int vl_Error = tatl19_askResults(&vl_msg_info);

    return vl_Error;
}
