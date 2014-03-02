/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides services for power services module
* \author  ST-Ericsson
*/
/*****************************************************************************/
#define TATL01SELFTEST_C
#include "tatlgps.h"
#undef TATL01SELFTEST_C

#include <pn_const_modem_ext.h>	/* ISI_common_lib/NWM_include */
#include <modem_test_msg_ext.h>	/* modem_lib/NWM_include */

int v_tatlgps_state;

static void tatl01_06SetSelfTestReq(TATLGPS_SELF_TEST_RUN_REQ *req)
{

	tat_modem_marsh8(&req->trans_id, ISI_MARSH);
	tat_modem_marsh8(&req->message_id, ISI_MARSH);
	tat_modem_marsh16(&req->group_id, ISI_MARSH);
	tat_modem_marsh16(&req->test_id, ISI_MARSH);
	tat_modem_marsh16(&req->case_ID, ISI_MARSH);
	tat_modem_marsh16((u16 *) req->fillbyte, ISI_MARSH);
	tat_modem_marsh16(&req->inp_data_length, ISI_MARSH);
}

static void tatl01_07GetSelfTestResp(TATLGPS_SELF_TEST_RUN_RESP *resp)
{

	tat_modem_marsh8(&resp->trans_id, ISI_UNMARSH);
	tat_modem_marsh8(&resp->message_id, ISI_UNMARSH);
	tat_modem_marsh16(&resp->group_id, ISI_UNMARSH);
	tat_modem_marsh16(&resp->test_id, ISI_UNMARSH);
	tat_modem_marsh16(&resp->case_ID, ISI_UNMARSH);
	tat_modem_marsh8(&resp->fillbyte, ISI_UNMARSH);
	tat_modem_marsh8(&resp->status, ISI_UNMARSH);
	tat_modem_marsh16(&resp->out_data_length, ISI_UNMARSH);
}

int tatl01_00SelfTest_exec(struct dth_element *elem)
{
	int vl_Error = 0;
	TATLGPS_SELF_TEST_RUN_REQ vl_req;
	TATLGPS_SELF_TEST_RUN_RESP vl_resp;
	size_t vl_req_len = sizeof(vl_req);
	size_t vl_resp_len = sizeof(vl_resp);
	uint32_t vl_resourceid;

	memset(&vl_req, 0, vl_req_len);
	memset(&vl_resp, 0, vl_resp_len);

	switch (elem->user_data) {
		/* use stored arguments values to execute action */
	case ACT_GPS_SELF_TEST_TX:

            switch (v_tatlgps_tx_state)
            {
            case TATLGPS_ON:
                vl_req.group_id = MODEM_TEST_GRP_SELFTEST_L1;
                vl_req.test_id = MODEM_TEST_ST_GPS_TX_ON;
                vl_req.case_ID = 0;
                break;

		case TATLGPS_OFF:
			vl_req.group_id = MODEM_TEST_GRP_SELFTEST_L1;
			vl_req.test_id = MODEM_TEST_ST_GPS_TX_OFF;
			vl_req.case_ID = 0;
			break;

		default:
			vl_Error = EINVAL;
		}
		break;

	case ACT_GPS_SELF_TEST_TIMEMARK:

            switch (v_tatlgps_time_state)
            {
            case TATLGPS_ON:
                vl_req.group_id = MODEM_TEST_GRP_SELFTEST_L1;
                vl_req.test_id = MODEM_TEST_ST_GPS_TIMEMARK_ON;
                vl_req.case_ID = 0;
                break;

		case TATLGPS_OFF:
			vl_req.group_id = MODEM_TEST_GRP_SELFTEST_L1;
			vl_req.test_id = MODEM_TEST_ST_GPS_TIMEMARK_OFF;
			vl_req.case_ID = 0;
			break;

		default:
			vl_Error = EINVAL;
		}
		break;

	default:
		vl_Error = EBADRQC;
	}

	if (vl_Error == 0) {
		isi_common_add_isi_header((uint8 *) &vl_req, vl_req_len,
					  PN_MODEM_TEST);
		vl_req.trans_id = TATLGPS_SELFTEST_TRANS_ID;
		vl_req.message_id = MODEM_TEST_RUN_REQ;
		vl_req.inp_data_length = 0;	/* no GPS self test request has data */

#ifdef _DEBUG
		tatl01_04DumpSelfTestReq(&vl_req);
#endif /* _DEBUG */
		tatl01_06SetSelfTestReq(&vl_req);

		//vl_Error = tatl03_01SendAndReceive(&vl_req, vl_req_len, &vl_resp, &vl_resp_len);
		vl_Error = tat_modem_send(TATGPS_FEATURE, (u8*)&vl_req, vl_req_len, &vl_resourceid);
		if ( 0 == vl_Error )
		{
		    vl_resp_len = sizeof(TATLGPS_SELF_TEST_RUN_RESP);
            vl_Error = tat_modem_read(TATGPS_FEATURE, vl_resourceid, &vl_resp, &vl_resp_len);
            if (0 == vl_Error)
            {
			    tatl01_07GetSelfTestResp(&vl_resp);
#ifdef _DEBUG
				tatl01_05DumpSelfTestResp(&vl_resp);
#endif /* _DEBUG */
			}
		}
	    if(vl_Error!=0)
	   {
		switch(elem->user_data)
  		{
        		/* use stored arguments values to execute action */
  		case ACT_GPS_SELF_TEST_TX:
 			v_tatlgps_tx_error=1;
		break;
		case ACT_GPS_SELF_TEST_TIMEMARK:
			v_tatlgps_time_error=1;
		break;
		}
           }
	
    }

    return 0;
}

int tatl01_01SelfTest_get(struct dth_element *elem, void *value)
{
    
   
    switch(elem->user_data)
    {
	case TX_STATUS:
		
		if(v_tatlgps_tx_error==1)
		{
			*((int*)value)=TATLGPS_TX_STATUS_ERROR;
				
		}
		else
		{
			*((int*)value)=v_tatlgps_tx_state;
		}	
	break;
	case TIME_STATUS:
		
		if(v_tatlgps_time_error==1)
		{
			*((uint8_t*)value)=TATLGPS_TIME_STATUS_ERROR;
				
		}
		else
		{
			*((uint8_t*)value)=v_tatlgps_time_state;
		}
	break;
  	default:
	    *((uint8_t*)value)=TATLGPS_TIME_STATUS_ERROR;
    }
    return 0;
}

int tatl01_02SelfTest_set(struct dth_element *elem, void *value)
{
    int vl_Error = 0;

  	switch(elem->user_data)
  	{
    	case ACT_GPS_SELF_TEST_TX:
    	/* these actions have the same enum arguments */
        switch (*((u32*)value))
        {
        case TATLGPS_ON:
            v_tatlgps_tx_state = TATLGPS_TX_STATUS_ON;
			printf("TATLGPS : TATLGPS_TX_STATUS_ON");
            break;

        case TATLGPS_OFF:
            v_tatlgps_tx_state = TATLGPS_TX_STATUS_OFF;
			printf("TATLGPS : TATLGPS_TX_STATUS_OFF");
            break;

        default:
		v_tatlgps_tx_error=1;
            vl_Error = EBADRQC;
        }
        break;

 	case ACT_GPS_SELF_TEST_TIMEMARK:

        /* these actions have the same enum arguments */
        switch (*((u32*)value))
        {
        case TATLGPS_ON:
            v_tatlgps_time_state = TATLGPS_TIME_STATUS_ON;
			printf("TATLGPS : TATLGPS_TIME_STATUS_ON");
            break;

        case TATLGPS_OFF:
            v_tatlgps_time_state = TATLGPS_TIME_STATUS_OFF;
			printf("TATLGPS : TATLGPS_TIME_STATUS_OFF");
            break;
        default:
	    v_tatlgps_time_error=1;
            vl_Error = EBADRQC;
        }
        break;

    default:
	v_tatlgps_tx_error=1;
	v_tatlgps_tx_error=1;
        vl_Error = EINVAL;
    }

    return vl_Error;
}

#ifdef _DEBUG

void tatl01_04DumpSelfTestReq(TATLGPS_SELF_TEST_RUN_REQ *req)
{
    fprintf(stdout, "\n++ TATLGPS_SELF_TEST_RUN_REQ\n");
    tatl03_02DumpIsiHeader(&req->hdr);
    fprintf(stdout, "trans_id=0x%X, message_id=0x%X, group_id=0x%X, test_id=0x%X, case_ID=0x%X\n",
    	req->trans_id,
    	req->message_id,
    	req->group_id,
    	req->test_id,
    	req->case_ID);
    fprintf(stdout, "-- TATLGPS_SELF_TEST_RUN_REQ\n\n");
}

void tatl01_05DumpSelfTestResp(TATLGPS_SELF_TEST_RUN_RESP *resp)
{
    fprintf(stdout, "\n++ TATLGPS_SELF_TEST_RUN_RESP\n");
    tatl03_02DumpIsiHeader(&resp->hdr);
    fprintf(stdout, "trans_id=0x%X, message_id=0x%X, group_id=0x%X, test_id=0x%X, case_ID=0x%X, status=0x%X, out_data_length=%u\n",
    	resp->trans_id,
    	resp->message_id,
    	resp->group_id,
    	resp->test_id,
    	resp->case_ID,
    	resp->status,
    	resp->out_data_length);
    fprintf(stdout, "-- TATLGPS_SELF_TEST_RUN_RESP\n\n");
}

#endif /* _DEBUG */

