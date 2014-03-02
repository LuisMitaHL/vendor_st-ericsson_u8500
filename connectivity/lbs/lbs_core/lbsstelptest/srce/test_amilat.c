/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) ST-Ericsson SA 2009. All rights reserved.
 *  This code is ST-Ericsson proprietary and confidential.
 *  Any use of the code for whatever purpose is subject to
 *  specific written permission of ST-Ericsson.
 *
 *  Author: Anshuman Pandey
 *  Email : anshuman.pandey@stericsson.com
 *****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <agpsosa.h>

#include <strings.h>
#include <gpsclient.h>

#include <stepl_utility.h>
#include <test_engine_int.h>
#include <test_stelp_socket.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "LBSSTELP"
#define SEND(a) TSTENG_execute_command(-1,a, NULL)
#define RECV(a) TSTENG_execute_command(-1,a, NULL)

//#define TCBEGIN(handle, str) TSTENG_begin(handle, str)


//#define TCEND(handle, str) TSTENG_end(handle, str)


static int TEST_AMILAT_001(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_AMILAT_002(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_AMILAT_003(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);

static int TEST_AMILAT_004(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_AMILAT_005(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_AMILAT_006(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);

/*static int TEST_AMILAT_007(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_AMILAT_008(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_AMILAT_009(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_AMILAT_010(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_AMILAT_011(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_AMILAT_012(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_AMILAT_013(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_AMILAT_014(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
*/

TSTENG_TEST_TABLE_BEGIN(AMILAT)

/*Test Case   */
    TSTENG_TEST_TABLE_CMD("LBSAMILAT001", TEST_AMILAT_001,          "LBSSTELP : Start AMILAT test")
    TSTENG_TEST_TABLE_CMD("LBSAMILAT002", TEST_AMILAT_002,          "LBSSTELP : Start AMILAT test")
    TSTENG_TEST_TABLE_CMD("LBSAMILAT003", TEST_AMILAT_003,          "LBSSTELP : Start AMILAT test")
    TSTENG_TEST_TABLE_CMD("LBSAMILAT004", TEST_AMILAT_004,          "LBSSTELP : Start AMILAT test")
    TSTENG_TEST_TABLE_CMD("LBSAMILAT005", TEST_AMILAT_005,          "LBSSTELP : Start AMILAT test")
    TSTENG_TEST_TABLE_CMD("LBSAMILAT006", TEST_AMILAT_006,          "LBSSTELP : Start AMILAT test")
/*    TSTENG_TEST_TABLE_CMD("LBSAMILAT007", TEST_AMILAT_007,          "LBSSTELP : Start AMILAT test")
    TSTENG_TEST_TABLE_CMD("LBSAMILAT008", TEST_AMILAT_008,          "LBSSTELP : Start AMILAT test")
    TSTENG_TEST_TABLE_CMD("LBSAMILAT009", TEST_AMILAT_009,          "LBSSTELP : Start AMILAT test")
    TSTENG_TEST_TABLE_CMD("LBSAMILAT010", TEST_AMILAT_010,          "LBSSTELP : Start AMILAT test")
    TSTENG_TEST_TABLE_CMD("LBSAMILAT011", TEST_AMILAT_011,          "LBSSTELP : Start AMILAT test")
    TSTENG_TEST_TABLE_CMD("LBSAMILAT012", TEST_AMILAT_012,          "LBSSTELP : Start AMILAT test")
    TSTENG_TEST_TABLE_CMD("LBSAMILAT013", TEST_AMILAT_013,          "LBSSTELP : Start AMILAT test")
    TSTENG_TEST_TABLE_CMD("LBSAMILAT014", TEST_AMILAT_014,          "LBSSTELP : Start AMILAT test")
*/

TSTENG_TEST_TABLE_END

LBSSTELP_reporting_criteria_t rc;
TSTENG_result_t AMILAT_TC_result = TSTENG_RESULT_OK;

#define K_NUM_RETRY_SOCKET_CONN 4
#define K_WAIT_FOR_STELP_SOCKET_CONN_SEC 2
#define K_WAIT_BEFORE_SEND_SEC 1
#define K_RECEIVE_DATA_TIMEOUT 60


uint8_t AMILAT_findsubstring(char *string,char *pattern)
{
    uint32_t i=0;

    for (i=0;i<strlen(string);i++)
    {
        if((strlen(string)-i)>=strlen(pattern))
        {
            if((string[i]==(*pattern)))
            {
                if(!strncmp(string+i,pattern,strlen(pattern)))
                    return 0;
            }
        }
    }

    return -1;
}


static int AMILAT_SendCommand( void* p_Data  )
{
    if (TSTENG_RESULT_OK == AMILAT_TC_result )
    {
        sleep(K_WAIT_BEFORE_SEND_SEC);
        ALOGD("Sending data over Socket - %s \n", p_Data);
        send( socket_conn.fd_stelp_socket, p_Data, strlen(p_Data), 0);
        return TRUE;
    }
    else
    {
        ALOGD ("DEBUG AMILAT_SendCommand : Test already failed. return Error");
        return FALSE;
    }

}

void AMILAT_SendOK(void)
{
    if (TSTENG_RESULT_OK == AMILAT_TC_result )
    {
        char string1[7];
        sprintf(string1,"\nOK%c",13); // OK received from modem is in this format. <newline>OK<verticalTab>
        sleep(K_WAIT_BEFORE_SEND_SEC);
        ALOGD("Sending OK over Socket - %s\n", string1);
        send( socket_conn.fd_stelp_socket, string1, strlen(string1), 0);
        return;

    }
    else
    {
        ALOGD ("DEBUG AMILAT_SendOK : Test already failed. return Error");
        return;
    }
}

static int AMILAT_ForceSendCommand( void* p_Data  )
{
    sleep(K_WAIT_BEFORE_SEND_SEC);
    ALOGD("Force Send data over Socket - %s \n", p_Data);
    send( socket_conn.fd_stelp_socket, p_Data, strlen(p_Data), 0);
    return TRUE;
}


static int AMILAT_ReceiveData( int8_t vp_timeout)
{

  /* Compare the string msg.testcase_str and p_Data */
    if (TSTENG_RESULT_OK == AMILAT_TC_result )
    {
        for (; vp_timeout >= 0; vp_timeout -= 5)
        {

            ALOGD("DEBUG vp_timeout = %d", vp_timeout);
            if (0 == (socket_conn.stelp_socket_data_read))
            {
                ALOGD ("DEBUG AMILAT_ReceiveData : No data in socket Sleep for 5 seconds");
                sleep(5);
            }
            else
            {
                ALOGD("msg.testcase_str = %s \n",msg.testcase_str);
                return TRUE;
            }
        }
        AMILAT_TC_result = TSTENG_RESULT_FAIL;
        ALOGD ("DEBUG AMILAT_ReceiveData : NoData recieved in time mentioned. return Error");
        return FALSE;
    }
    else
    {
        ALOGD ("DEBUG AMILAT_ReceiveData : Test already failed. return Error");
        return FALSE;
    }
}



void AMILAT_ClearReceivedData( )
{

  /* Compare the string msg.testcase_str and p_Data */
    if (TSTENG_RESULT_OK == AMILAT_TC_result )
    {
        ALOGD ("DEBUG 1. AMILAT_ClearReceivedData socket_conn.stelp_socket_data_read = %d  ", socket_conn.stelp_socket_data_read);
        socket_conn.stelp_socket_data_read = 0;
        ALOGD ("DEBUG 2. AMILAT_ClearReceivedData socket_conn.stelp_socket_data_read = %d  ", socket_conn.stelp_socket_data_read);

    }
    else
    {
        ALOGD ("DEBUG AMILAT_ClearReceivedData : Test already failed. return Error");
    }
}


static int AMILAT_CheckDataContains( void* pp_Data )
{


  /* Compare the string msg.testcase_str and p_Data */
    if (TSTENG_RESULT_OK == AMILAT_TC_result )
    {
        if (!AMILAT_findsubstring(msg.testcase_str,pp_Data))
        {
            ALOGD ("DEBUG AMILAT_CheckDataContains : %s match",pp_Data);
            return TRUE;
        }
        else
        {
            AMILAT_TC_result = TSTENG_RESULT_FAIL;
            ALOGD ("DEBUG AMILAT_CheckDataContains : %s not received",pp_Data);
            return FALSE;
        }
    }
    else
    {
        ALOGD ("DEBUG AMILAT_CheckDataContains : Test already failed. return Error");
        return FALSE;
    }
}


int AMILAT_TEST_START()
{
    uint8_t v_retry = 0;

    for (v_retry = 0; v_retry < K_NUM_RETRY_SOCKET_CONN; v_retry++)
    {
        if (socket_conn.fd_stelp_socket)
        {
            AMILAT_TC_result = TSTENG_RESULT_OK;
            ALOGD ("DEBUG AMILAT_TEST_START : TSTENG_RESULT_OK %d",v_retry);
            break;
        }
        else
        {
            AMILAT_TC_result = TSTENG_RESULT_FAIL;
            ALOGD ("DEBUG AMILAT_TEST_START : TSTENG_RESULT_FAIL %d",v_retry);
            sleep(K_WAIT_FOR_STELP_SOCKET_CONN_SEC);
        }
    }
    return AMILAT_TC_result;

}

void AMILAT_TEST_END()
{
    if (TSTENG_RESULT_OK == AMILAT_TC_result )
        ALOGD("\n>>>>>>>>> TESTCASE PASS <<<<<<<<");
    else
        ALOGD("\n>>>>>>>>> TESTCASE FAIL <<<<<<<<");

    AMILAT_ForceSendCommand("STELPTESTEND");

    LBSSTELP_socket_close_down();

}


void AMILAT_SEND_MSB_POSINST(void)
{

    ALOGD("MSB ra1 ril32");

    AMILAT_SendCommand(\
"AT+CPOSR: <?xml version=\"1.0\" ?>\n\
AT+CPOSR: <pos>\n\
AT+CPOSR: <pos_meas>\n\
AT+CPOSR: <RRC_meas>\n\
AT+CPOSR: <rep_quant addl_assist_data_req=\"true\">\n\
AT+CPOSR: <RRC_method_type literal=\"ue_based\"/>\n\
AT+CPOSR: <RRC_method literal=\"gps\"/>\n\
AT+CPOSR: <hor_acc>16</hor_acc>\n\
AT+CPOSR: <vert_acc>10</vert_acc>\n\
AT+CPOSR: </rep_quant>\n\
AT+CPOSR: <rep_crit>\n\
AT+CPOSR: <period_rep_crit>\n\
AT+CPOSR: <rep_amount literal=\"ra1\"/>\n\
AT+CPOSR: <rep_interval_long literal=\"ril32\"/>\n\
AT+CPOSR: </period_rep_crit>\n\
AT+CPOSR: </rep_crit>\n\
AT+CPOSR: </RRC_meas>\n\
AT+CPOSR: </pos_meas>\n\
AT+CPOSR: </pos>");

    return;

}

void AMILAT_SEND_MSA_POSINST(void)
{

        ALOGD("MSA ra1 ril32");

    AMILAT_SendCommand(\
"AT+CPOSR: <?xml version=\"1.0\" ?>\n\
AT+CPOSR: <pos>\n\
AT+CPOSR: <pos_meas>\n\
AT+CPOSR: <RRC_meas>\n\
AT+CPOSR: <rep_quant addl_assist_data_req=\"true\">\n\
AT+CPOSR: <RRC_method_type literal=\"ue_assisted\"/>\n\
AT+CPOSR: <RRC_method literal=\"gps\"/>\n\
AT+CPOSR: <hor_acc>16</hor_acc>\n\
AT+CPOSR: <vert_acc>10</vert_acc>\n\
AT+CPOSR: </rep_quant>\n\
AT+CPOSR: <rep_crit>\n\
AT+CPOSR: <period_rep_crit>\n\
AT+CPOSR: <rep_amount literal=\"ra1\"/>\n\
AT+CPOSR: <rep_interval_long literal=\"ril32\"/>\n\
AT+CPOSR: </period_rep_crit>\n\
AT+CPOSR: </rep_crit>\n\
AT+CPOSR: </RRC_meas>\n\
AT+CPOSR: </pos_meas>\n\
AT+CPOSR: </pos>");

    return;

}

void AMILAT_SEND_MSB_POSINST2(void)
{

    ALOGD("MSB ra8 ril1");

    AMILAT_SendCommand(\
"AT+CPOSR: <?xml version=\"1.0\" ?>\n\
AT+CPOSR: <pos>\n\
AT+CPOSR: <pos_meas>\n\
AT+CPOSR: <RRC_meas>\n\
AT+CPOSR: <rep_quant addl_assist_data_req=\"true\">\n\
AT+CPOSR: <RRC_method_type literal=\"ue_based\"/>\n\
AT+CPOSR: <RRC_method literal=\"gps\"/>\n\
AT+CPOSR: <hor_acc>16</hor_acc>\n\
AT+CPOSR: <vert_acc>10</vert_acc>\n\
AT+CPOSR: </rep_quant>\n\
AT+CPOSR: <rep_crit>\n\
AT+CPOSR: <period_rep_crit>\n\
AT+CPOSR: <rep_amount literal=\"ra8\"/>\n\
AT+CPOSR: <rep_interval_long literal=\"ril1\"/>\n\
AT+CPOSR: </period_rep_crit>\n\
AT+CPOSR: </rep_crit>\n\
AT+CPOSR: </RRC_meas>\n\
AT+CPOSR: </pos_meas>\n\
AT+CPOSR: </pos>");

    return;

}

void AMILAT_SEND_MSA_POSINST2(void)
{

    ALOGD("MSA ra8 ril1");

    AMILAT_SendCommand(\
"AT+CPOSR: <?xml version=\"1.0\" ?>\n\
AT+CPOSR: <pos>\n\
AT+CPOSR: <pos_meas>\n\
AT+CPOSR: <RRC_meas>\n\
AT+CPOSR: <rep_quant addl_assist_data_req=\"true\">\n\
AT+CPOSR: <RRC_method_type literal=\"ue_assisted\"/>\n\
AT+CPOSR: <RRC_method literal=\"gps\"/>\n\
AT+CPOSR: <hor_acc>16</hor_acc>\n\
AT+CPOSR: <vert_acc>10</vert_acc>\n\
AT+CPOSR: </rep_quant>\n\
AT+CPOSR: <rep_crit>\n\
AT+CPOSR: <period_rep_crit>\n\
AT+CPOSR: <rep_amount literal=\"ra8\"/>\n\
AT+CPOSR: <rep_interval_long literal=\"ril1\"/>\n\
AT+CPOSR: </period_rep_crit>\n\
AT+CPOSR: </rep_crit>\n\
AT+CPOSR: </RRC_meas>\n\
AT+CPOSR: </pos_meas>\n\
AT+CPOSR: </pos>");

    return;

}


void AMILAT_SEND_REF_TIME(void)
{
    AMILAT_SendCommand(\
"AT+CPOSR: <?xml version=\"1.0\" ?>\n\
AT+CPOSR: <pos>\n\
AT+CPOSR: <assist_data>\n\
AT+CPOSR: <GPS_assist>\n\
AT+CPOSR: <ref_time>\n\
AT+CPOSR: <GPS_time>\n\
AT+CPOSR: <GPS_TOW_23b>346217190</GPS_TOW_23b>\n\
AT+CPOSR: <GPS_week>230</GPS_week>\n\
AT+CPOSR: </GPS_time>\n\
AT+CPOSR: <GPS_TOW_assist>\n\
AT+CPOSR: <sat_id>59</sat_id>\n\
AT+CPOSR: <tlm_word>18432</tlm_word>\n\
AT+CPOSR: <anti_sp>0</anti_sp>\n\
AT+CPOSR: <alert>0</alert>\n\
AT+CPOSR: <tlm_res>0</tlm_res>\n\
AT+CPOSR: </GPS_TOW_assist>\n\
AT+CPOSR: <GPS_TOW_assist>\n\
AT+CPOSR: <sat_id>62</sat_id>\n\
AT+CPOSR: <tlm_word>18432</tlm_word>\n\
AT+CPOSR: <anti_sp>128</anti_sp>\n\
AT+CPOSR: <alert>60</alert>\n\
AT+CPOSR: <tlm_res>163</tlm_res>\n\
AT+CPOSR: </GPS_TOW_assist>\n\
AT+CPOSR: </ref_time>\n\
AT+CPOSR: </GPS_assist>\n\
AT+CPOSR: </assist_data>\n\
AT+CPOSR: </pos>");

}

void AMILAT_RECEIVE_MSA_ASSTDATA_REQ(void)
{
    if (AMILAT_ReceiveData(K_RECEIVE_DATA_TIMEOUT))
    {
        AMILAT_CheckDataContains("AT+CPOS: <?xml version=\"1.0\" ?>");
        AMILAT_CheckDataContains("AT+CPOS: <pos>");
        AMILAT_CheckDataContains("AT+CPOS: <pos_err>");
        AMILAT_CheckDataContains("AT+CPOS: <err_reason literal=\"gps_assist_data_missing\"/>");
        AMILAT_CheckDataContains("AT+CPOS: <GPS_assist_req>");
        AMILAT_CheckDataContains("AT+CPOS: <alm_req literal=\"false\"/>");
        AMILAT_CheckDataContains("AT+CPOS: <UTC_model_req literal=\"false\"/>");
        AMILAT_CheckDataContains("AT+CPOS: <nav_model_req literal=\"false\"/>");
        AMILAT_CheckDataContains("AT+CPOS: <DGPS_corr_req literal=\"false\"/>");
        AMILAT_CheckDataContains("AT+CPOS: <ref_loc_req literal=\"false\"/>");
        AMILAT_CheckDataContains("AT+CPOS: <ref_time_req literal=\"true\"/>");
        AMILAT_CheckDataContains("AT+CPOS: <aqu_assist_req literal=\"true\"/>");
        AMILAT_CheckDataContains("AT+CPOS: <rt_integr_req literal=\"false\"/>");
        AMILAT_CheckDataContains("AT+CPOS: <nav_addl_data>");
        AMILAT_CheckDataContains("AT+CPOS: <GPS_week>");
        AMILAT_CheckDataContains("AT+CPOS: <GPS_toe>");
        AMILAT_CheckDataContains("AT+CPOS: <ttoe_limit>");
        AMILAT_CheckDataContains("AT+CPOS: <sat_data>");
        AMILAT_CheckDataContains("AT+CPOS: </sat_data>");
        AMILAT_CheckDataContains("AT+CPOS: </nav_addl_data>");
        AMILAT_CheckDataContains("AT+CPOS: </GPS_assist_req>");
        AMILAT_CheckDataContains("AT+CPOS: </pos_err>");
        AMILAT_CheckDataContains("AT+CPOS: </pos>");
        AMILAT_ClearReceivedData();
    }

    return;

}

void AMILAT_RECEIVE_MSB_ASSTDATA_REQ(void)
{
    if (AMILAT_ReceiveData(K_RECEIVE_DATA_TIMEOUT))
    {
        AMILAT_CheckDataContains("AT+CPOS: <?xml version=\"1.0\" ?>");
        AMILAT_CheckDataContains("AT+CPOS: <pos>");
        AMILAT_CheckDataContains("AT+CPOS: <pos_err>");
        AMILAT_CheckDataContains("AT+CPOS: <err_reason literal=\"gps_assist_data_missing\"/>");
        AMILAT_CheckDataContains("AT+CPOS: <GPS_assist_req>");
        AMILAT_CheckDataContains("AT+CPOS: <alm_req literal=\"false\"/>");
        AMILAT_CheckDataContains("AT+CPOS: <UTC_model_req literal=\"false\"/>");
        AMILAT_CheckDataContains("AT+CPOS: <nav_model_req literal=\"true\"/>");
        AMILAT_CheckDataContains("AT+CPOS: <DGPS_corr_req literal=\"false\"/>");
        AMILAT_CheckDataContains("AT+CPOS: <ref_loc_req literal=\"true\"/>");
        AMILAT_CheckDataContains("AT+CPOS: <ref_time_req literal=\"true\"/>");
        AMILAT_CheckDataContains("AT+CPOS: <aqu_assist_req literal=\"false\"/>");
        AMILAT_CheckDataContains("AT+CPOS: <rt_integr_req literal=\"false\"/>");
        AMILAT_CheckDataContains("AT+CPOS: <nav_addl_data>");
        AMILAT_CheckDataContains("AT+CPOS: <GPS_week>");
        AMILAT_CheckDataContains("AT+CPOS: <GPS_toe>");
        AMILAT_CheckDataContains("AT+CPOS: <ttoe_limit>");
        AMILAT_CheckDataContains("AT+CPOS: <sat_data>");
        AMILAT_CheckDataContains("AT+CPOS: </sat_data>");
        AMILAT_CheckDataContains("AT+CPOS: </nav_addl_data>");
        AMILAT_CheckDataContains("AT+CPOS: </GPS_assist_req>");
        AMILAT_CheckDataContains("AT+CPOS: </pos_err>");
        AMILAT_CheckDataContains("AT+CPOS: </pos>");
        AMILAT_ClearReceivedData();
    }

}

void AMILAT_RECEIVE_POS_ERROR()
{
    if (AMILAT_ReceiveData(K_RECEIVE_DATA_TIMEOUT))
    {
        AMILAT_CheckDataContains("AT+CPOS: <?xml version=\"1.0\" ?>");
        AMILAT_CheckDataContains("AT+CPOS: <pos>");
        AMILAT_CheckDataContains("AT+CPOS: <pos_err>");
        AMILAT_CheckDataContains("AT+CPOS: <err_reason literal=\"not_enough_gps_satellites\"/>");
        AMILAT_CheckDataContains("AT+CPOS: </pos_err>");
        AMILAT_CheckDataContains("AT+CPOS: </pos>");
        AMILAT_ClearReceivedData();
    }
}


static int TEST_AMILAT_001(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    AMILAT_TC_result = TSTENG_RESULT_OK;

    AMILAT_TEST_START();

    AMILAT_SEND_MSA_POSINST();

    AMILAT_RECEIVE_MSA_ASSTDATA_REQ();

    AMILAT_SendOK();

    AMILAT_TEST_END();

    return AMILAT_TC_result;

}



static int TEST_AMILAT_002(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    AMILAT_TC_result = TSTENG_RESULT_OK;

    AMILAT_TEST_START();

    AMILAT_SEND_MSB_POSINST();

    AMILAT_RECEIVE_MSB_ASSTDATA_REQ();

    AMILAT_SendOK();

    AMILAT_TEST_END();

    return AMILAT_TC_result;

}



static int TEST_AMILAT_003(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    AMILAT_TC_result = TSTENG_RESULT_OK;

    AMILAT_TEST_START();

    AMILAT_SEND_MSB_POSINST();

    AMILAT_RECEIVE_MSB_ASSTDATA_REQ();

    AMILAT_SendOK();

    AMILAT_SEND_REF_TIME();

    AMILAT_RECEIVE_POS_ERROR();

    AMILAT_SendOK();


    AMILAT_TEST_END();

    return AMILAT_TC_result;

}


static int TEST_AMILAT_004(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    uint8_t index=8;
    AMILAT_TC_result = TSTENG_RESULT_OK;

    AMILAT_TEST_START();

    AMILAT_SEND_MSB_POSINST2();

    AMILAT_RECEIVE_MSB_ASSTDATA_REQ();

    AMILAT_SendOK();

    AMILAT_SEND_REF_TIME();

    while(index--)
    {
        ALOGD("TEST_AMILAT_004 1");
        AMILAT_RECEIVE_POS_ERROR();
        AMILAT_SendOK();
    }



    AMILAT_TEST_END();

    return AMILAT_TC_result;

}

static int TEST_AMILAT_005(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    AMILAT_TC_result = TSTENG_RESULT_OK;

    AMILAT_TEST_START();

    AMILAT_SEND_MSA_POSINST();

    AMILAT_RECEIVE_MSA_ASSTDATA_REQ();

    AMILAT_SendOK();

    AMILAT_SEND_REF_TIME();

    AMILAT_RECEIVE_POS_ERROR();

    AMILAT_SendOK();


    AMILAT_TEST_END();

    return AMILAT_TC_result;

}

static int TEST_AMILAT_006(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    uint8_t index=8;

    AMILAT_TC_result = TSTENG_RESULT_OK;

    AMILAT_TEST_START();

    AMILAT_SEND_MSA_POSINST2();

    AMILAT_RECEIVE_MSA_ASSTDATA_REQ();

    AMILAT_SendOK();

    AMILAT_SEND_REF_TIME();

    while(index--)
    {
        AMILAT_RECEIVE_POS_ERROR();
        AMILAT_SendOK();
    }
    AMILAT_TEST_END();

    return AMILAT_TC_result;

}



