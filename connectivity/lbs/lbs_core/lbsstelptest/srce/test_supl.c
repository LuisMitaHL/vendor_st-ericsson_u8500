/*
 * STELP Test App
 *
 * test_naf.c
 *
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <TlsHandler_stub.h>
#include <agpsosa.h>

#include <strings.h>

#include <gpsclient.h>
//#include <test_inf.h>


#include <stepl_utility.h>
#include <test_engine_int.h>

//#include <gps.h>
//#include <gps_ni.h>

extern t_TlsConnection *vg_Tlsconnection ;
extern unsigned int    vg_Server_present_Flag;
extern unsigned int  vg_TestHandle ;
extern unsigned int vg_disconnect_Flag;
extern unsigned int vg_TestConnectCnf ;
extern unsigned int vg_TestConnectCnf_Flag;
extern unsigned int vg_Write_Flag ;
extern char *vg_Write_Buffer ;
e_gpsClient_AgpsStatData vg_agpsStatus = 0;
extern int t_flage;

int Supl_pos_flag =0;



int vg_test_result;
static int NumTCPass;
static int NumTCFail;
static int Set_Session_Id;
char buf_SessionId[3];
int powi(int number, int exponent);


#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "LBSSTELP"
#define SEND(a) TSTENG_execute_command(-1,a, NULL)
#define RECV(a) TSTENG_execute_command(-1,a, NULL)

#define MS_IN_SEC (1000)

#define TCBEGIN(handle, str) TSTENG_begin(handle, str)


#define TCEND(handle, str) TSTENG_end(handle, str)

#define TSTENG_BEGIN_STEP(handle, str) \
    ALOGD("LBSSTEP %s STARTED \n", str);\
if(TSTENG_check_info_cb() == TSTENG_RESULT_FAIL)\
    {\
      TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "FAILED to START");\
    return 0;\
}

#define TSTENG_END_STEP(handle, str) \
    ALOGD("LBSSTEP %s END \n", str);\





/*SUPL test*/
static int TEST_SUPL_gen_wait_connect(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_SUPL_gen_recv_data(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_SUPL_gen_connect_cnf(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_SUPL_gen_send_data(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_SUPL_gen_Disconnect_Cnf(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_SUPL_gen_Disconnect_Ind(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_SUPL_gen_Disconnect_Req(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static void TEST_SUPL_Init(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static void TEST_SUPL_Status_cb(e_gpsClient_AgpsStatData *stat);
static int TEST_SUPL_gen_Bearer_Cnf(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_SUPL_gen_Close_Bearer_Cnf(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_SUPL_gen_wait_bearer_req(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_SUPL_gen_wait_close_bearer_req(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int  TEST_SUPL_Wait_Event(uint wait_period);
static int TEST_SUPL_Mobile_info(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_SUPL_WLAN_info(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);

static int TEST_SUPL_Sms_Push(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static void TSTENG_begin(TSTENG_handle_t handle,const char * str);
static void TSTENG_end(TSTENG_handle_t handle,const char * str);
static int TSTENG_check_info_cb(void);


/* Test Case */
static int TEST_SUPL_001(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_SUPL_002(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_SUPL_003(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_SUPL_004(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_SUPL_005(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_SUPL_006(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_SUPL_007(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_SUPL_008(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);


/*ULTS Test Cases */

/* SUPL 1.0 */

static int LBSUPLCON010(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);

static int LBSUPLCON102test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);

static int LBSUPLCON165(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON000(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON023(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON279(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON166(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON168(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON150test1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON150test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON150test3(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON381(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON130test1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON130test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON130test3(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON131test1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON131test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON132(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON106test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON106test3(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON102test1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON103test1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON103test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON104test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON102test3(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON104test1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON100test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON100test1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON060test1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON062(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON063(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON068(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON270test1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON270test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON271(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON272(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON273(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON274(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON275(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON276(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON277(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON278(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON033(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON006test1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON006test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON032(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON279test1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON279test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON279test3(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON013(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON000test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON060(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON066(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON067(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON002test1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON002test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON003test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON003test3(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON004test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON004test3(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON030test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON030test3(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON032test1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON032test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON031test3(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON030test5(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON282(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON281(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSSUPLINT001(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSSUPLINT002(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSSUPLATT300(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);

/* SUPL 2.0 */

static int LBSUPLCON0040a(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON0061(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON0070(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON0074(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON120b(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON135(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON140(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON141(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON0033a(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON0033b(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON0034a(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON0034b(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);

static int LBSUPLCON113test1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON113test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON113test3(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON113test4(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON113test5(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON113test6(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON113test7(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON113test8(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);


static int LBSUPLCON111test1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON111test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON111test3(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON111test4(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON111test5(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON111test6(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON111test7(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON111test8(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON111test9(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);

/* SUPL 2.0 Area Id  and Geographic Target Area */

/* SUPL 2.0 Geo target area  Network Initiated */

static int LBSUPLCON0050a(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON0050b(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);

/* SUPL 2.0 Geo target area  Set Initiated */

static int LBSUPLCON0130a(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON0130b(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);

/* SUPL 2.0 Area Id  Network Initiated */

static int LBSUPLCON0051a(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON0051b(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON0051c(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON0051d(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON0051e(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON0051f(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON0051g(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);

static int LBSUPLCON0051h(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON0051i(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON0051j(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON0051k(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON0051l(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON0051m(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON0051n(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON0051o(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON0051p(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON0051q(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);

/* SUPL 2.0 Area Id  Network Initiated */

/* SUPL 2.0 Area Id  Set Initiated       */

static int LBSUPLCON0131a(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON0131b(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON0131c(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON0131d(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON0131e(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON0131f(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON0131g(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPLCON0131h(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);

/* SUPL 2.0 Area Id  Set Initiated       */

/* SUPL 2.0 Area Id  and Geographic Target Area */

static int LBSUPL_WLAN_test1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_WLAN_test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_WLAN_test3(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);

static int LBSUPL_WLAN_test4(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_WLAN_test5(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_WLAN_test6(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);

static int LBSUPL_WLAN_test7(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_WLAN_test8(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_WLAN_test9(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);

static int LBSUPL_WLAN_test10(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_WLAN_test11(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_WLAN_test12(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_WLAN_test13(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_WLAN_test14(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_WLAN_test15(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);

static int LBSUPL_WLAN_test16(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);


static int LBSUPL_MLID_001(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_MLID_002(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_MLID_003(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_MLID_004(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_MLID_005(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_MLID_006(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_MLID_007(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_MLID_008(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_MLID_009(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_MLID_010(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_MLID_011(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_MLID_012(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_MLID_013(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_MLID_014(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_MLID_015(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_MLID_016(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_MLID_017(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_MLID_018(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_MLID_019(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_MLID_020(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_MLID_021(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_MLID_022(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_MLID_023(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_MLID_024(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_MLID_025(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_MLID_026(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_MLID_027(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_MLID_028(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_MLID_029(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_MLID_030(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_MLID_031(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSUPL_MLID_032(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);


#define TEST_SUPL_DEFAULT_WAIT 1 /*1 sec*/
#define TEST_SUPL_MAX_WAIT_RETRY 20 /*20 sec*/
#define TEST_SUPL_WAIT_TIMEOUT(X) sleep(X)

TSTENG_TEST_TABLE_BEGIN(SUPL)
    /*SUPL test*/

    TSTENG_TEST_TABLE_CMD("LBSSUPLMobInf",  TEST_SUPL_Mobile_info,               "LBSSTELP : Mobile info ")
    TSTENG_TEST_TABLE_CMD("LBSSUPLWLANInf", TEST_SUPL_WLAN_info,                 "LBSSTELP : WLAN info ")
    TSTENG_TEST_TABLE_CMD("LBSSUPLINIT",    TEST_SUPL_Init,                      "LBSSTELP : Wait for connection request")
    TSTENG_TEST_TABLE_CMD("LBSCONREQ",      TEST_SUPL_gen_wait_connect,          "LBSSTELP : Wait for connection request")
    TSTENG_TEST_TABLE_CMD("LBSCONCNF",      TEST_SUPL_gen_connect_cnf,           "LBSSTELP : Wait for connection request")
    TSTENG_TEST_TABLE_CMD("LBSDATARECV",    TEST_SUPL_gen_recv_data,             "LBSSTELP : Wait for recever data")
    TSTENG_TEST_TABLE_CMD("LBSDATASEND",    TEST_SUPL_gen_send_data,             "LBSSTELP : Wait for sending data")
    TSTENG_TEST_TABLE_CMD("LBSDISREQ",      TEST_SUPL_gen_Disconnect_Req,        "LBSSTELP : disconnect Request")
    TSTENG_TEST_TABLE_CMD("LBSDISCNF",      TEST_SUPL_gen_Disconnect_Cnf,        "LBSSTELP : disconnect Confirm")
    TSTENG_TEST_TABLE_CMD("LBSDISIND",      TEST_SUPL_gen_Disconnect_Ind,        "LBSSTELP : disconnect send")
    TSTENG_TEST_TABLE_CMD("LBSPDPREQ",      TEST_SUPL_gen_wait_bearer_req,       "LBSSTELP : Bearer Request")
    TSTENG_TEST_TABLE_CMD("LBSPDPCNF",      TEST_SUPL_gen_Bearer_Cnf,            "LBSSTELP : Bearer Confirmation")
    TSTENG_TEST_TABLE_CMD("LBSCLOSEPDPREQ", TEST_SUPL_gen_wait_close_bearer_req, "LBSSTELP : Close Bearer Request")
    TSTENG_TEST_TABLE_CMD("LBSCLOSEPDPCNF", TEST_SUPL_gen_Close_Bearer_Cnf,      "LBSSTELP : Close Bearer Confirmation")
    TSTENG_TEST_TABLE_CMD("LBSSmsPush",     TEST_SUPL_Sms_Push,                  "LBSSTELP : Push SMS")

/*Test Case   */
    TSTENG_TEST_TABLE_CMD("LBSSUPL001",     TEST_SUPL_001,          "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSSUPL002",     TEST_SUPL_002,          "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSSUPL003",     TEST_SUPL_003,          "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSSUPL004",     TEST_SUPL_004,          "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSSUPL005",     TEST_SUPL_005,          "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSSUPL006",     TEST_SUPL_006,          "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSSUPL007",     TEST_SUPL_007,          "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSSUPL008",     TEST_SUPL_008,          "LBSSTELP : Start SUPL test")

 /*ULTS Test Cases */
    TSTENG_TEST_TABLE_CMD("LBSUPLCON010",      LBSUPLCON010,         "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON102test2", LBSUPLCON102test2,    "LBSSTELP : Start SUPL test")


    TSTENG_TEST_TABLE_CMD("LBSUPLCON113test1", LBSUPLCON113test1,           "LBSSTELP : Start SUPL test")

    TSTENG_TEST_TABLE_CMD("LBSUPLCON113test2", LBSUPLCON113test2,           "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON113test3", LBSUPLCON113test3,           "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON113test4", LBSUPLCON113test4,           "LBSSTELP : Start SUPL test")
    
    TSTENG_TEST_TABLE_CMD("LBSUPLCON113test5", LBSUPLCON113test5,           "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON113test6", LBSUPLCON113test6,           "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON113test7", LBSUPLCON113test7,           "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON113test8", LBSUPLCON113test8,           "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON111test1",  LBSUPLCON111test1,            "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON111test2",  LBSUPLCON111test2,            "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON111test3",  LBSUPLCON111test3,            "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON111test4",  LBSUPLCON111test4,            "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON111test5",  LBSUPLCON111test5,            "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON111test6",  LBSUPLCON111test6,            "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON111test7",  LBSUPLCON111test7,            "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON111test8",  LBSUPLCON111test8,            "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON111test9",  LBSUPLCON111test9,            "LBSSTELP : Start SUPL test")

    TSTENG_TEST_TABLE_CMD("LBSUPLCON165",      LBSUPLCON165,         "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON000",      LBSUPLCON000,         "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON023",      LBSUPLCON023,         "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON279",      LBSUPLCON279,         "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON166",      LBSUPLCON166,         "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON168",      LBSUPLCON168,         "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON150test1", LBSUPLCON150test1,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON150test2", LBSUPLCON150test2,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON150test3", LBSUPLCON150test3,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON381",      LBSUPLCON381,         "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON130test1", LBSUPLCON130test1,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON130test2", LBSUPLCON130test2,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON130test3", LBSUPLCON130test3,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON131test1", LBSUPLCON131test1,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON131test2", LBSUPLCON131test2,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON132",      LBSUPLCON132,         "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON106test2", LBSUPLCON106test2,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON106test3", LBSUPLCON106test3,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON102test1", LBSUPLCON102test1,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON103test1", LBSUPLCON103test1,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON103test2", LBSUPLCON103test2,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON104test2", LBSUPLCON104test2,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON102test3", LBSUPLCON102test3,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON104test1", LBSUPLCON104test1,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON100test2", LBSUPLCON100test2,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON100test1", LBSUPLCON100test1,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON060test1", LBSUPLCON060test1,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON062",      LBSUPLCON062,         "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON063",      LBSUPLCON063,         "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON068",      LBSUPLCON068,         "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON270test1", LBSUPLCON270test1,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON270test2", LBSUPLCON270test2,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON271",      LBSUPLCON271,         "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON272",      LBSUPLCON272,         "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON273",      LBSUPLCON273,         "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON274",      LBSUPLCON274,         "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON275",      LBSUPLCON275,         "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON276",      LBSUPLCON276,         "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON277",      LBSUPLCON277,         "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON278",      LBSUPLCON278,         "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON033",      LBSUPLCON033,         "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON006test1", LBSUPLCON006test1,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON006test2", LBSUPLCON006test2,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON032",      LBSUPLCON032,         "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON279test1", LBSUPLCON279test1,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON279test2", LBSUPLCON279test2,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON279test3", LBSUPLCON279test3,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON013",      LBSUPLCON013,         "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON000test2", LBSUPLCON000test2,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON060",      LBSUPLCON060,         "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON066",      LBSUPLCON066,         "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON067",      LBSUPLCON067,         "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON002test1", LBSUPLCON002test1,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON002test2", LBSUPLCON002test2,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON003test2", LBSUPLCON003test2,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON003test3", LBSUPLCON003test3,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON004test2", LBSUPLCON004test2,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON004test3", LBSUPLCON004test3,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON030test2", LBSUPLCON030test2,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON030test3", LBSUPLCON030test3,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON032test1", LBSUPLCON032test1,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON032test2", LBSUPLCON032test2,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON031test3", LBSUPLCON031test3,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON030test5", LBSUPLCON030test5,    "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON282",      LBSUPLCON282,         "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON281",      LBSUPLCON281,         "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSSUPLTESTABORT",  LBSSUPLINT001,        "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSSUPLTESTSTARTSTOP", LBSSUPLINT002,     "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSSUPLATT300",     LBSSUPLATT300,        "LBSSTELP : Start SUPL test")

/* SUPL 2.0 */
    TSTENG_TEST_TABLE_CMD("LBSUPLCON040a",     LBSUPLCON0040a,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON061",      LBSUPLCON0061,        "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON070",      LBSUPLCON0070,        "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON074",      LBSUPLCON0074,        "LBSSTELP : Start SUPL test")

    TSTENG_TEST_TABLE_CMD("LBSUPLCON120b",     LBSUPLCON120b,        "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON135",      LBSUPLCON135,         "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON140",      LBSUPLCON140,         "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON141",      LBSUPLCON141,         "LBSSTELP : Start SUPL test")

    TSTENG_TEST_TABLE_CMD("LBSUPLCON0033a",    LBSUPLCON0033a,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON0033b",    LBSUPLCON0033b,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON0034a",    LBSUPLCON0034a,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON0034b",    LBSUPLCON0034b,       "LBSSTELP : Start SUPL test")

/* SUPL 2.0 Area Id  and Geographic Target Area */

    TSTENG_TEST_TABLE_CMD("LBSUPLCON0051a",    LBSUPLCON0051a,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON0051b",    LBSUPLCON0051b,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON0051c",    LBSUPLCON0051c,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON0051d",    LBSUPLCON0051d,       "LBSSTELP : Start SUPL test") 
    TSTENG_TEST_TABLE_CMD("LBSUPLCON0051e",    LBSUPLCON0051e,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON0051f",    LBSUPLCON0051f,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON0051g",    LBSUPLCON0051g,       "LBSSTELP : Start SUPL test")
    
    TSTENG_TEST_TABLE_CMD("LBSUPLCON0051h",    LBSUPLCON0051h,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON0051i",    LBSUPLCON0051i,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON0051j",    LBSUPLCON0051j,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON0051k",    LBSUPLCON0051k,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON0051l",    LBSUPLCON0051l,       "LBSSTELP : Start SUPL test") 
    TSTENG_TEST_TABLE_CMD("LBSUPLCON0051m",    LBSUPLCON0051m,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON0051n",    LBSUPLCON0051n,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON0051o",    LBSUPLCON0051o,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON0051p",    LBSUPLCON0051p,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON0051q",    LBSUPLCON0051q,       "LBSSTELP : Start SUPL test")

    TSTENG_TEST_TABLE_CMD("LBSUPLCON0131a",    LBSUPLCON0131a,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON0131b",    LBSUPLCON0131b,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON0131c",    LBSUPLCON0131c,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON0131d",    LBSUPLCON0131d,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON0131e",    LBSUPLCON0131e,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON0131f",    LBSUPLCON0131f,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON0131g",    LBSUPLCON0131g,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON0131h",    LBSUPLCON0131h,       "LBSSTELP : Start SUPL test")

    TSTENG_TEST_TABLE_CMD("LBSUPLCON0050a",    LBSUPLCON0050a,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON0050b",    LBSUPLCON0050b,       "LBSSTELP : Start SUPL test")

    TSTENG_TEST_TABLE_CMD("LBSUPLCON0130a",    LBSUPLCON0130a,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPLCON0130b",    LBSUPLCON0130b,       "LBSSTELP : Start SUPL test")
    
    TSTENG_TEST_TABLE_CMD("LBSUPL_WLAN_test1",  LBSUPL_WLAN_test1,   "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_WLAN_test2",  LBSUPL_WLAN_test2,   "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_WLAN_test3",  LBSUPL_WLAN_test3,   "LBSSTELP : Start SUPL test")

    TSTENG_TEST_TABLE_CMD("LBSUPL_WLAN_test4",  LBSUPL_WLAN_test4, "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_WLAN_test5",  LBSUPL_WLAN_test5, "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_WLAN_test6",  LBSUPL_WLAN_test6, "LBSSTELP : Start SUPL test")

    TSTENG_TEST_TABLE_CMD("LBSUPL_WLAN_test7",  LBSUPL_WLAN_test7, "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_WLAN_test8",  LBSUPL_WLAN_test8, "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_WLAN_test9",  LBSUPL_WLAN_test9, "LBSSTELP : Start SUPL test")

    TSTENG_TEST_TABLE_CMD("LBSUPL_WLAN_test10",  LBSUPL_WLAN_test10,   "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_WLAN_test11",  LBSUPL_WLAN_test11,   "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_WLAN_test12",  LBSUPL_WLAN_test12,   "LBSSTELP : Start SUPL test")

    TSTENG_TEST_TABLE_CMD("LBSUPL_WLAN_test13",  LBSUPL_WLAN_test13,   "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_WLAN_test14",  LBSUPL_WLAN_test14,   "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_WLAN_test15",  LBSUPL_WLAN_test15,   "LBSSTELP : Start SUPL test")

    TSTENG_TEST_TABLE_CMD("LBSUPL_WLAN_test16",  LBSUPL_WLAN_test16,   "LBSSTELP : Start SUPL test")

    TSTENG_TEST_TABLE_CMD("LBSUPL_MLID_001",    LBSUPL_MLID_001,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_MLID_002",    LBSUPL_MLID_002,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_MLID_003",    LBSUPL_MLID_003,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_MLID_004",    LBSUPL_MLID_004,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_MLID_005",    LBSUPL_MLID_005,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_MLID_006",    LBSUPL_MLID_006,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_MLID_007",    LBSUPL_MLID_007,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_MLID_008",    LBSUPL_MLID_008,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_MLID_009",    LBSUPL_MLID_009,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_MLID_010",    LBSUPL_MLID_010,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_MLID_011",    LBSUPL_MLID_011,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_MLID_012",    LBSUPL_MLID_012,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_MLID_013",    LBSUPL_MLID_013,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_MLID_014",    LBSUPL_MLID_014,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_MLID_015",    LBSUPL_MLID_015,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_MLID_016",    LBSUPL_MLID_016,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_MLID_017",    LBSUPL_MLID_017,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_MLID_018",    LBSUPL_MLID_018,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_MLID_019",    LBSUPL_MLID_019,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_MLID_020",    LBSUPL_MLID_020,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_MLID_021",    LBSUPL_MLID_021,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_MLID_022",    LBSUPL_MLID_022,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_MLID_023",    LBSUPL_MLID_023,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_MLID_024",    LBSUPL_MLID_024,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_MLID_025",    LBSUPL_MLID_025,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_MLID_026",    LBSUPL_MLID_026,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_MLID_027",    LBSUPL_MLID_027,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_MLID_028",    LBSUPL_MLID_028,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_MLID_029",    LBSUPL_MLID_029,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_MLID_030",    LBSUPL_MLID_030,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_MLID_031",    LBSUPL_MLID_031,       "LBSSTELP : Start SUPL test")
    TSTENG_TEST_TABLE_CMD("LBSUPL_MLID_032",    LBSUPL_MLID_032,       "LBSSTELP : Start SUPL test")

TSTENG_TEST_TABLE_END

LBSSTELP_reporting_criteria_t rc;

/*---------------------------------------------------------------------------------------*/
/*-----------------------          Test Generic Command     -------------------------------------*/
/*---------------------------------------------------------------------------------------*/

static const t_gpsClient_agpsCallbacks TEST_SUPL_callbacks = {

    .agpsstat_cb = TEST_SUPL_Status_cb
};

static void TEST_SUPL_Status_cb(e_gpsClient_AgpsStatData *p_Agpsstat)
{
    ALOGD("LBSSUPL : TEST_SUPL_Status_cb Enter\n");

    vg_agpsStatus = *p_Agpsstat;
    vg_Server_present_Flag = TRUE ;

    ALOGD("LBSSUPL : TEST_SUPL_Status_cb Exit vg_Server_present_Flag = %d ,vg_agpsStatus = %d\n",vg_Server_present_Flag ,vg_agpsStatus);

}
static int TEST_SUPL_Wait_Event(uint wait_Period)
{
    int retVal = FALSE;
    int retry = 0;

    ALOGD("LBSSUPL : TEST_SUPL_Wait_Event Enter vg_Server_present_Flag  = %d\n",vg_Server_present_Flag);
    while ((vg_Server_present_Flag == FALSE) && (vg_disconnect_Flag == FALSE)&&(retry < wait_Period))
    {
        sleep(TEST_SUPL_DEFAULT_WAIT);
        retry++;

    ALOGD("LBSSUPL : TEST_SUPL_Wait_Event inside while loop retry = %d\n",retry);

    }
    ALOGD("LBSSUPL : TEST_SUPL_Wait_Event exit vg_Server_present_Flag  = %d\n",vg_Server_present_Flag);

    vg_Server_present_Flag = FALSE ;

    if(retry < TEST_SUPL_MAX_WAIT_RETRY)
        retVal = TRUE;


return retVal;
}





static void TEST_SUPL_Init(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    TSTENG_BEGIN_STEP(handle, __func__);
    ALOGD("LBSSUPL : TEST_SUPL_Init\n");
    gpsClient_AgpsInit(&TEST_SUPL_callbacks);

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, " SUPL INIT done");
    TSTENG_END_STEP(handle, __func__);
}


static int TEST_SUPL_gen_Bearer_Cnf(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    TSTENG_BEGIN_STEP(handle, __func__);
    int v_result = atoi(cmd_buf + arg_index[0]);
    if(v_result == 1)
    {

        ALOGD("LBSSUPL : TEST_SUPL_gen_Bearer_Cnf Yes\n");
        char *v_apn = malloc (sizeof(char)*50);
        v_apn = cmd_buf + arg_index[1];
        gpsclient_DataConnOpen((char*)v_apn);
        TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, " Bearer cnf send");
    }
    else
    {

        ALOGD("LBSSUPL : TEST_SUPL_gen_Bearer_Cnf No\n");
        gpsclient_DataConnFailed();
        TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, " Bearer Error send");
    }

    TSTENG_END_STEP(handle, __func__);
    return TSTENG_RESULT_OK;
}

static int TEST_SUPL_gen_Close_Bearer_Cnf(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
     TSTENG_BEGIN_STEP(handle, __func__);
     int v_result = atoi(cmd_buf + arg_index[0]);

    if(v_result == 1)
    {

        ALOGD("LBSSUPL : TEST_SUPL_gen_Close_Bearer_Cnf Yes\n");
        gpsclient_DataConnClosed();
        TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, " Bearer close cnf send");
    }
    else
    {

        ALOGD("LBSSUPL : TEST_SUPL_gen_Close_Bearer_Cnf No \n");
        gpsclient_DataConnCloseFailed();
        TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, " Bearer Error send");
    }
    TSTENG_END_STEP(handle, __func__);
    return TSTENG_RESULT_OK;
}


static int TEST_SUPL_gen_connect_cnf(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    TSTENG_BEGIN_STEP(handle, __func__);
    int v_handle = atoi(cmd_buf + arg_index[0]);
    int v_result = atoi(cmd_buf + arg_index[1]);

    if(v_result == 1)
    {

        ALOGD("LBSSUPL : TEST_SUPL_gen_connect_cnf yes \n");
        TlsConnectCnf(v_handle);
        TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, " connetct cnf send");
    }
    else
    {

        ALOGD("LBSSUPL : TEST_SUPL_gen_connect_cnf No \n");
        int v_cause = atoi(cmd_buf + arg_index[2]);
        TlsConnectErr(v_handle,v_cause);
        TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, " connetct cnf send");
    }

    //TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, "LBSSTELP : connetct exit");
    TSTENG_END_STEP(handle, __func__);
    return TSTENG_RESULT_OK;
}

static int TEST_SUPL_gen_recv_data(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)

{
    TSTENG_BEGIN_STEP(handle, __func__);
    int v_handle = atoi(cmd_buf + arg_index[0]);
    int v_length = atoi(cmd_buf + arg_index[2]);
    int v_compareLen = atoi(cmd_buf + arg_index[3]);
    uint v_waitPeriod = atoi(cmd_buf + arg_index[4]);
    int i,j;
    int index;
    char *output;
    output = (char *)malloc( v_length+1 );
    ALOGD("LBSSUPL : TEST_SUPL_gen_recv_data enter\n");
 
    /* Copy input data */
    char *vl_recv_data = malloc((v_length*2)+1);

    if( v_waitPeriod == 0 )
        v_waitPeriod = TEST_SUPL_MAX_WAIT_RETRY;

    memcpy(vl_recv_data ,(cmd_buf + arg_index[1]),(v_length*2));
    vl_recv_data[(v_length*2)] = '\0';

    for( index = 0 ; index < v_length ; index++  )
    {
        char byte[3];
        int inputIndex = index*2;
        int asnValue = 0;
        unsigned int high = 0, low = 0;

        high = vl_recv_data[inputIndex++];
        low  = vl_recv_data[inputIndex];


        if( high > 0x39  )
        {
            high = high - 0x37;
        }
        else
        {
            high = high - 0x30;
        }

        if( low > 0x39  )
        {
            low = low - 0x37;
        }
        else
        {
            low = low - 0x30;
        }

        asnValue = (high * 0x10) + low;
        output[index] = asnValue;

    }
    output[index] = '\0';

    if(TEST_SUPL_Wait_Event(v_waitPeriod) == FALSE)
    {
        TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL,
        "LBSSTELP : TEST_SUPL_gen_recv_data\n");
        return TSTENG_RESULT_FAIL;
    }

    /*while (t_flage == FALSE)
    {
        sleep(TEST_SUPL_DEFAULT_WAIT);

    }
    t_flage = FALSE;*/


    for(i=6,j=0;i<8;i++,j++)
    {
        buf_SessionId[j]=vg_Write_Buffer[i];
    }
    buf_SessionId[j] ='\0';

    if ((strncmp(output ,vg_Write_Buffer,v_compareLen)) == 0)
    {
        TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, " Received data and input data is same");
    }
    else
    {
        TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, " Received data and input data is different");
    }

    ALOGD("LBSSUPL : TEST_SUPL_gen_recv_data exit\n");

    OSA_Free(output);
    TSTENG_END_STEP(handle, __func__);
    return TSTENG_RESULT_OK;

}


static int TEST_SUPL_gen_send_data(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)

{
    TSTENG_BEGIN_STEP(handle, __func__);
    ALOGD("LBSSUPL : TEST_SUPL_gen_send_data enter\n");
    unsigned int v_handle = atoi(cmd_buf + arg_index[0]);
    int v_length = atoi(cmd_buf + arg_index[2]);
    int i,j;

    /* Archana */

    int index;
    char *output;
    output = (char *)malloc( v_length+1 );

    /* Copy input data */
    char *vl_Read_Buffer = malloc((v_length*2)+1);
    memcpy(vl_Read_Buffer ,(cmd_buf + arg_index[1]),(v_length*2));
    vl_Read_Buffer[(v_length*2)] = '\0';

    for( index = 0 ; index < v_length ; index++  )
    {
        char byte[3];
        int inputIndex = index*2;
        int asnValue = 0;
        unsigned int high = 0, low = 0;

        high = vl_Read_Buffer[inputIndex++];
        low  = vl_Read_Buffer[inputIndex];


        if( high > 0x39  )
        {
            high = high - 0x37;
        }
        else
        {
            high = high - 0x30;
        }

        if( low > 0x39  )
        {
            low = low - 0x37;
        }
        else
        {
            low = low - 0x30;
        }

        asnValue = (high * 0x10) + low;
        output[index] = asnValue;
        //ALOGD("LBSSUPL: TEST_SUPL_gen_send_data SUPL asnValue = %x", output[index]);
    }

    output[index] = '\0';
    
    for(i=6,j=0;i<8;i++,j++)
    {
        output[i]=buf_SessionId[j];
    }        

    /* Archana */
    ALOGD("LBSSUPL : TEST_SUPL_gen_send_data, vl_Read_Buffer = %s ,v_length = %d\n",output,v_length);
    TlsReadInd(v_handle , output , v_length );

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, " Data sent");

    if( output != NULL )
    free(output);
    output = NULL;

    TSTENG_END_STEP(handle, __func__);
    return TSTENG_RESULT_OK;
}

static int TEST_SUPL_gen_Disconnect_Cnf(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)

{
    TSTENG_BEGIN_STEP(handle, __func__);
    unsigned int v_handle = atoi(cmd_buf + arg_index[0]);
    ALOGD("LBSSUPL: TEST_SUPL_gen_Disconnect_Cnf handler = %d", v_handle);

    TlsDisconnectCnf(v_handle );
    vg_disconnect_Flag = FALSE;

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, " Disconnect Cnf send");
    TSTENG_END_STEP(handle, __func__);
    return TSTENG_RESULT_OK;
}



static int TEST_SUPL_gen_Disconnect_Req(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)

{
     TSTENG_BEGIN_STEP(handle, __func__);
     int v_handle = atoi(cmd_buf + arg_index[0]);

    if(TEST_SUPL_Wait_Event(TEST_SUPL_MAX_WAIT_RETRY) == FALSE)
    {
        TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL,
                            " TEST_SUPL_gen_Disconnect_Req\n");
        return TSTENG_RESULT_FAIL;
    }

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, " Disconnect Request recevied");
    TSTENG_END_STEP(handle, __func__);
    return TSTENG_RESULT_OK;
}


static int TEST_SUPL_gen_Disconnect_Ind(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)

{
    TSTENG_BEGIN_STEP(handle, __func__);

    int v_handle = atoi(cmd_buf + arg_index[0]);

    //TlsDisconnectInd(handle );
    /*This functionality is missing from software in TLS*/

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, " Disconnectind Cnf send");
    TSTENG_END_STEP(handle, __func__);
    return TSTENG_RESULT_OK;
}



//LBSWAITCONN  handle  addr  port .

static int TEST_SUPL_gen_wait_connect(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TSTENG_BEGIN_STEP(handle, __func__);
    ALOGD("LBSSUPL : TEST_SUPL_gen_wait_connect Enter\n");

    unsigned int testHandle = atoi(cmd_buf + arg_index[0]);
    char *vg_server_addr = malloc (sizeof(char)*256);
    int v_length = strlen((cmd_buf + arg_index[1]));
    memcpy(vg_server_addr,(cmd_buf + arg_index[1]),v_length) ;
    vg_server_addr[v_length] = '\0';
    unsigned short vg_port  = atoi(cmd_buf + arg_index[2]);

    if(TEST_SUPL_Wait_Event(TEST_SUPL_MAX_WAIT_RETRY) == FALSE)
    {
        TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL,
        " TEST_SUPL_gen_wait_connect\n");
        return TSTENG_RESULT_FAIL;
    }
/*
    if(vg_TestHandle != testHandle)
    {
        TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "Test Handle wrong");
        return TSTENG_RESULT_FAIL;
    }
*/
    if(strncmp((vg_Tlsconnection->server.a_Addr) , vg_server_addr ,19) != 0 )
    {
        //ALOGD("LBSSUPL : TEST_SUPL_gen_wait_connect vg_Tlsconnection->server.a_Addr =%s\n",vg_Tlsconnection->server.a_Addr);

        TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "Test server address wrong recv");
        return TSTENG_RESULT_FAIL;
    }

    if((vg_Tlsconnection->server.v_Port)!= vg_port)
    {

        TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "Test server port wrong");
        return TSTENG_RESULT_FAIL;
    }
    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
    " Received connect request\n");
    TSTENG_END_STEP(handle, __func__);
    return TSTENG_RESULT_OK;


}

static int TEST_SUPL_gen_wait_bearer_req(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    TSTENG_BEGIN_STEP(handle, __func__);
    ALOGD("LBSSUPL : TEST_SUPL_gen_wait_bearer_req Enter\n");

    if(TEST_SUPL_Wait_Event(TEST_SUPL_MAX_WAIT_RETRY) == FALSE)
    {
        TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL,
        " TEST_SUPL_gen_wait_bearer_req\n");

        return TSTENG_RESULT_FAIL;
    }



    if(vg_agpsStatus != GPSCLIENT_REQUEST_AGPS_DATA_CONN)
    {
            TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL,
        " TEST_SUPL_gen_wait_bearer_req\n");

        vg_agpsStatus = 0;

    return TSTENG_RESULT_FAIL;
    }
    ALOGD("LBSSUPL : TEST_SUPL_gen_wait_bearer_req done\n");

    vg_agpsStatus = 0;
    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
        " TEST_SUPL_gen_wait_bearer_req\n");
    TSTENG_END_STEP(handle, __func__);
    return TSTENG_RESULT_OK;

}

static int TEST_SUPL_gen_wait_close_bearer_req(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TSTENG_BEGIN_STEP(handle, __func__);
    ALOGD("LBSSUPL : TEST_SUPL_gen_wait_close_bearer_req Enter\n");

    if(TEST_SUPL_Wait_Event(TEST_SUPL_MAX_WAIT_RETRY) == FALSE)
    {
        TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL,
        " TEST_SUPL_gen_wait_close_bearer_req\n");

        return TSTENG_RESULT_FAIL;
    }

    vg_Server_present_Flag = FALSE ;
    ALOGD("LBSSUPL : TEST_SUPL_gen_wait_bearer_req vg_agpsStatus = %d\n",vg_agpsStatus);

    if(vg_agpsStatus != GPSCLIENT_RELEASE_AGPS_DATA_CONN)
    {

        TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL,
        " TEST_SUPL_gen_wait_close_bearer_req\n");
        ALOGD("LBSSUPL : TEST_SUPL_gen_wait_close_bearer_req not done\n");

        vg_agpsStatus = 0;

    return TSTENG_RESULT_FAIL;
    }
    vg_agpsStatus = 0;
    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
        " TEST_SUPL_gen_wait_close_bearer_req\n");
    TSTENG_END_STEP(handle, __func__);
    return TSTENG_RESULT_OK;

}

static int TEST_SUPL_Mobile_info(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)

{
    TSTENG_BEGIN_STEP(handle, __func__);

    char v_imsi[64];
    int v_handle = atoi(cmd_buf + arg_index[0]);
    int v_mcc = atoi(cmd_buf + arg_index[1]);
    int v_mnc = atoi(cmd_buf + arg_index[2]);
    int v_cellId = atoi(cmd_buf + arg_index[3]);
    int v_Lac = atoi(cmd_buf + arg_index[4]);
    int v_celltype = atoi(cmd_buf + arg_index[5]);
    strcpy(v_imsi,(cmd_buf + arg_index[6]));
    //strcpy(v_msisdn,(cmd_buf + arg_index[7]));

    ALOGD("LBSSUPL : TEST_SUPL_Mobile_info enter\n");

    gpsClient_Mobile_Info(v_mcc,v_mnc,v_cellId,v_Lac,v_celltype,v_imsi,NULL);

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, " gpsClient_Mobile_Info send");
    TSTENG_END_STEP(handle, __func__);
    return TSTENG_RESULT_OK;


}
static int TEST_SUPL_WLAN_info(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)

{
    TSTENG_BEGIN_STEP(handle, __func__);
    t_gpsClient_WlanInfo v_wlanInfo;

    v_wlanInfo.v_WLAN_AP_MS_Addr                                       = atoi(cmd_buf + arg_index[0]);
    v_wlanInfo.v_WLAN_AP_LS_Addr                                       = atoi(cmd_buf + arg_index[1]);
    v_wlanInfo.v_WLAN_OptionalInfo = (t_gpsClient_WlanOptinalInfo *)malloc( sizeof(t_gpsClient_WlanOptinalInfo) );
    if(v_wlanInfo.v_WLAN_OptionalInfo != NULL)
    {

        v_wlanInfo.v_WLAN_OptionalInfo->v_WLAN_AP_TransmitPower        = atoi(cmd_buf + arg_index[2]);
        v_wlanInfo.v_WLAN_OptionalInfo->v_WLAN_AP_AntennaGain          = atoi(cmd_buf + arg_index[3]);
        v_wlanInfo.v_WLAN_OptionalInfo->v_WLAN_AP_SNR                  = atoi(cmd_buf + arg_index[4]);
        v_wlanInfo.v_WLAN_OptionalInfo->v_WLAN_AP_DeviceType           = atoi(cmd_buf + arg_index[5]);
        v_wlanInfo.v_WLAN_OptionalInfo->v_WLAN_AP_SignalStrength       = atoi(cmd_buf + arg_index[6]);
        v_wlanInfo.v_WLAN_OptionalInfo->v_WLAN_AP_Channel              = atoi(cmd_buf + arg_index[7]);
        v_wlanInfo.v_WLAN_OptionalInfo->v_WLAN_AP_RTDValue             = atoi(cmd_buf + arg_index[8]);
        v_wlanInfo.v_WLAN_OptionalInfo->v_WLAN_AP_RTDUnits             = atoi(cmd_buf + arg_index[9]);
        v_wlanInfo.v_WLAN_OptionalInfo->v_WLAN_AP_Accuracy             = atoi(cmd_buf + arg_index[10]);
        v_wlanInfo.v_WLAN_OptionalInfo->v_WLAN_AP_SETTransmitPower     = atoi(cmd_buf + arg_index[11]);
        v_wlanInfo.v_WLAN_OptionalInfo->v_WLAN_AP_SETAntennaGain       = atoi(cmd_buf + arg_index[12]);
        v_wlanInfo.v_WLAN_OptionalInfo->v_WLAN_AP_SETSNR               = atoi(cmd_buf + arg_index[13]);
        v_wlanInfo.v_WLAN_OptionalInfo->v_WLAN_AP_SETSignalStrength    = atoi(cmd_buf + arg_index[14]);

        
        ALOGD("TEST_SUPL_WLAN_info : v_AP_tx_power =%d",v_wlanInfo.v_WLAN_OptionalInfo->v_WLAN_AP_TransmitPower);
        ALOGD("TEST_SUPL_WLAN_info : v_AP_antenna_gain =%d",v_wlanInfo.v_WLAN_OptionalInfo->v_WLAN_AP_AntennaGain);
        ALOGD("TEST_SUPL_WLAN_info : v_AP_SNR =%d",v_wlanInfo.v_WLAN_OptionalInfo->v_WLAN_AP_SNR);
        
        ALOGD("TEST_SUPL_WLAN_info : v_AP_Device_type =%d",v_wlanInfo.v_WLAN_OptionalInfo->v_WLAN_AP_DeviceType);
        ALOGD("TEST_SUPL_WLAN_info : v_AP_signal_strength =%d",v_wlanInfo.v_WLAN_OptionalInfo->v_WLAN_AP_SignalStrength);
        ALOGD("TEST_SUPL_WLAN_info : v_AP_channel =%d",v_wlanInfo.v_WLAN_OptionalInfo->v_WLAN_AP_Channel);
        
        ALOGD("TEST_SUPL_WLAN_info : v_RTD_Value =%d",v_wlanInfo.v_WLAN_OptionalInfo->v_WLAN_AP_RTDValue);
        ALOGD("TEST_SUPL_WLAN_info : v_RTD_units =%d",v_wlanInfo.v_WLAN_OptionalInfo->v_WLAN_AP_RTDUnits);
        ALOGD("TEST_SUPL_WLAN_info : v_RTD_accuracy =%d",v_wlanInfo.v_WLAN_OptionalInfo->v_WLAN_AP_Accuracy);
        
        ALOGD("TEST_SUPL_WLAN_info : v_SET_Transmit_power =%d",v_wlanInfo.v_WLAN_OptionalInfo->v_WLAN_AP_SETTransmitPower);
        ALOGD("TEST_SUPL_WLAN_info : v_SET_antenna_gain =%d",v_wlanInfo.v_WLAN_OptionalInfo->v_WLAN_AP_SETAntennaGain);
        ALOGD("TEST_SUPL_WLAN_info : v_SET_SNR =%d",v_wlanInfo.v_WLAN_OptionalInfo->v_WLAN_AP_SETSNR);
        
        ALOGD("TEST_SUPL_WLAN_info : v_SET_signal_strength =%d",v_wlanInfo.v_WLAN_OptionalInfo->v_WLAN_AP_SETSignalStrength);
    }


    ALOGD("LBSSUPL : TEST_SUPL_WLAN_info enter\n");
    
    ALOGD("TEST_SUPL_WLAN_info : v_WLAN_AP_MS_Addr =%x",v_wlanInfo.v_WLAN_AP_MS_Addr);
    ALOGD("TEST_SUPL_WLAN_info : v_WLAN_AP_LS_Addr =%x",v_wlanInfo.v_WLAN_AP_LS_Addr);

    gpsClient_WLAN_Info(&v_wlanInfo);

    free(v_wlanInfo.v_WLAN_OptionalInfo);
    if(v_wlanInfo.v_WLAN_OptionalInfo != NULL)
    {
        v_wlanInfo.v_WLAN_OptionalInfo = NULL;
    }


    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, " gpsClient_WLAN_Info send");
    TSTENG_END_STEP(handle, __func__);
    return TSTENG_RESULT_OK;


}


static int TEST_SUPL_Sms_Push(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)

{

    TSTENG_BEGIN_STEP(handle, __func__);

    ALOGD("LBSSUPL : TEST_SUPL_Sms_Push enter\n");


    int v_handle = atoi(cmd_buf + arg_index[0]);
    int v_length = atoi(cmd_buf + arg_index[2]);
    int result = 0;
    int index;
    char *output;
    output = (char *)malloc( v_length+1 );
    /* Copy input data */
    char *vl_Read_Buffer = malloc((v_length*2)+1);
    memcpy(vl_Read_Buffer ,(cmd_buf + arg_index[1]),(v_length*2));
    vl_Read_Buffer[(v_length*2)] = '\0';

    for( index = 0 ; index < v_length ; index++  )
    {
        char byte[3];
        int inputIndex = index*2;
        int asnValue = 0;
        unsigned int high = 0, low = 0;

        high = vl_Read_Buffer[inputIndex++];
        low  = vl_Read_Buffer[inputIndex];


        if( high > 0x39  )
        {
            high = high - 0x37;
        }
        else
        {
            high = high - 0x30;
        }

        if( low > 0x39  )
        {
            low = low - 0x37;
        }
        else
        {
            low = low - 0x30;
        }

        asnValue = (high * 0x10) + low;
        output[index] = asnValue;
        //ALOGD("LBSSUPL: TEST_SUPL_gen_send_data SUPL asnValue = %x", output[index]);
    }

    output[index] = '\0';
    result = agpsClient_SmsPush( output , v_length , NULL, 0);

    if(result)
    {
        TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL,
        " TEST_SUPL_Sms_Push\n");
        return TSTENG_RESULT_FAIL;
    }


    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, " TEST_SUPL_Sms_Push send");
    TSTENG_END_STEP(handle, __func__);
    return TSTENG_RESULT_OK;


}




static int TSTENG_check_info_cb()
{

  return vg_test_result;
}
static void TSTENG_begin(TSTENG_handle_t handle,const char * str)
{
    ALOGD("LBSTC %s STARTED \n", str);

    vg_test_result = TSTENG_RESULT_OK;
}


static void TSTENG_end(TSTENG_handle_t handle,const char * str)
{
    ALOGD("LBSSTELP : TSTENG_end begin");
    FILE  *filePtr;

    filePtr = fopen("/data/LbsSUPLTestResults.txt","a");

    if(filePtr == NULL)
    {
      ALOGD("Unable to open Result File...");
      return;
    }

    ALOGD("LBSTC %s STOPPED \n", str);
    TSTENG_test_info_cb(handle, vg_test_result, str);

    if(vg_test_result == TSTENG_RESULT_OK)
    {
       NumTCPass++;
       ALOGD("LBSSTELP : TSTENG_end NumTCPass =%d",NumTCPass);
    }
    else
    {

       NumTCFail++;
       ALOGD("LBSSTELP : TSTENG_end NumTCFail =%d",NumTCFail);
    }

    fprintf(filePtr, "%s, %s\r\n", str, ((vg_test_result == TSTENG_RESULT_OK) ? "PASS" : "FAIL"));

    vg_test_result = TSTENG_RESULT_FAIL;

    fclose(filePtr);

}


/*---------------------------------------------------------------------------------------*/
/*-----------------------          Test Case------------- -------------------------------------*/
/*---------------------------------------------------------------------------------------*/


static int TEST_SUPL_001(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    SEND("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    SEND("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    SEND("SUPL LBSDATARECV 1 002F02000080004C000404850D961FE45F80082200070A024020EA4800095A02108059415B0DB88DEC00B945E000B0 46 10");//SUPL_START   handler , PDU, LENGTH, COMP Lenght
    SEND("SUPL LBSDATASEND 1 001A010000C0004C000404850D961FE400000004605400119000 26");//SUPL_RESP
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSDATARECV 1 001A010000C0004C000404850D961FE40000000460540011A800 26");//SUPL_END
    SEND("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    SEND("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");

    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}


static int TEST_SUPL_002(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 0 0") ;//ERR
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");

    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;


}

static int TEST_SUPL_003(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    /*Test Description:
          SUPL session is disconnected after SUPL_START is received.
    */

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275"); //err
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 002F02000080004C000404850D961FE45F80082200070A024020EA4800095A02108059415B0DB88DEC00B945E000B0 46 10");//SUPL_START
    SEND("SUPL LBSDISIND 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");

    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}

static int TEST_SUPL_004(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    /*Test Description:
          After receiving SUPL_START, TS doesnot  send SUPL response.
          Upon expiry of UT1 timer, SUPL session will need to be terminated.
          TS will receive SUPL END message followed by disconnect request
    */

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSBEARERREQ");
    SEND("SUPL LBSBEARERCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 002F02000080004C000404850D961FE45F80082200070A024020EA4800095A02108059415B0DB88DEC00B945E000B0 46 10");//SUPL_START
    TEST_SUPL_WAIT_TIMEOUT(10); //UT1 timer must expire
    RECV("SUPL LBSDATARECV 1 001A010000C0004C000404850D961FE40000000460540011A800 26 10");//SUPL_END
    RECV("SUPL LBSDISREQ 1");
    SEND("SUPL LBSDISCNF 1");
    SEND("NAF LBSSTOP 1");


    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}

static int TEST_SUPL_005(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    SEND("SUPL LBSDATASEND 1 001A010000C0004C000404850D961FE400000004605400119000 26");//SUPL_RESP
    RECV("SUPL LBSDATARECV 1 0028010000C0004C000404850D961FE400000004605400119A0E093DF00000000800200800000090 40 10");//SUPL_POSINT
    SEND("SUPL LBSDATASEND 1 003301FFFFC0004C000404850D961FE40000000460540011AA01AC19B072C593262CD9307369CCB01938D86140000000003600 51");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}




static int TEST_SUPL_006(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 002e02000080004c000404850d961fe45f80082200070a024020ea4800095a02108200080200000025ca2f000580 46");//SUPL_START
    SEND("SUPL LBSDATASEND 1 003301FFFFC0004C000404850D961FE40000000460540011AA01AC19B072C593262CD9307369CCB01938D86140000000003600 51");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}

static int TEST_SUPL_007(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    /*Test Description:
          SUPL session is disconnected after SUPL_START is received.
    */

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}

static int TEST_SUPL_008(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{


    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 00100100004000000046054001180010 16");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 0030010000C0004C000404850D961FE400000004605400119A4E093DF00000000800200800000114564810562527D260 48 10");//SUPL_POSINIT
    SEND("SUPL LBSDATASEND 1 003301FFFFC0004C000404850D961FE40000000460540011AA01AC19B072C593262CD9307369CCB01938D86140000000003600 51");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}


/*ULTS Tet Cases */

static int LBSUPLCON102test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 001F01000080004C000404850D961FE453824200080200000025CCAF000AE0 36 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A010000C0004C000404850D961FE400000004605400119004 26");//SUPL_RESP
    RECV("SUPL LBSDATARECV 1 0028010000C0004C000404850D961FE400000004605400119A0E093DF00000000800200800000090 40 10");//SUPL_POSINT
    TEST_SUPL_WAIT_TIMEOUT(10);
    RECV("SUPL LBSDATARECV 1 001A010000C0004C000404850D961FE40000000460540011A800 36 10");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}


static int LBSUPLCON113test1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{/* Sending SUPL End as soon as SUPL start*/
/* Normal Third Party Location Forward case where SUPL START message has the third aprty information*/
/*SUPL_END is sent after SUPL_POSINIT message from SET*/
    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");

    SEND("NAF LBSLOCFWD 1 1 30 60 100 5 1 portalnmms");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003a02000080004c000404850d961fe47ba240216b6800095a3a04a05021080200080200000025ccaf000ae0218400009e1bf974c3b376ddbcc0 58 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A020000C0004C000404850D961FE400000004605400119004 26");//SUPL_RESP                             
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e890085ada0002568e812814084202df00000000800200800000090 53 10");//SUPL_POSINT
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}

static int LBSUPLCON113test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
/* Normal Third Party Location Forward case where SUPL START message with third party id and No Third Party Name present*/
    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSLOCFWD 1 1 30 60 100 5 1 0");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}
static int LBSUPLCON113test3(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
/* Normal Third Party Location Forward case where SUPL START message has the third aprty information*/
/*Invalid Third Party ID- 50*/
    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSLOCFWD 1 1 30 60 100 5 50 portalnmms");/*Invalid Third Party ID- 50*/
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}

static int LBSUPLCON113test4(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
/* Sending SUPL End as soon as SUPL start*/
/*When SUPL_END is received the SET should disconnect and close the SUPL transaction*/

    TCBEGIN(handle, __func__);
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSLOCFWD 1 1 30 60 100 5 1 portalnmms");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    SEND("SUPL LBSDATARECV 1 003a02000080004c000404850d961fe47ba240216b6800095a3a04a05021080200080200000025ccaf000ae0218400009e1bf974c3b376ddbcc0 58 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}

static int LBSUPLCON113test5(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
/* Normal Third Party Location Forward case where SUPL START message has the third aprty information*/
/*SUPL_END is sent after SUPL_POSINIT message from SET*/
/*10 sec wait after SUPL_END*/
    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSLOCFWD 1 1 30 60 100 5 1 portalnmms");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003a02000080004c000404850d961fe47ba240216b6800095a3a04a05021080200080200000025ccaf000ae0218400009e1bf974c3b376ddbcc0 58 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A020000C0004C000404850D961FE400000004605400119004 26");//SUPL_RESP                             
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e890085ada0002568e812814084202df00000000800200800000090 53 10");//SUPL_POSINT
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");//SUPL_END
    TEST_SUPL_WAIT_TIMEOUT(10);
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}


static int LBSUPLCON113test6(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
/* Normal Third Party Location Forward case where SUPL START message has the third aprty information*/
/*SUPL_END is sent after SUPL_POSINIT message from SET*/
/*sending SUPL_STOP after SUPL_START*/
    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSLOCFWD 1 1 30 60 100 5 1 portalnmms");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003a02000080004c000404850d961fe47ba240216b6800095a3a04a05021080200080200000025ccaf000ae0218400009e1bf974c3b376ddbcc0 58 10");//SUPL_START
    SEND("NAF LBSSTOP 1");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}

static int LBSUPLCON113test7(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
/* Normal Third Party Location Forward case where SUPL START message has the third aprty information*/
/*Here the in SUPL_POSINIT the preferred is agpsSETassistedPreferred in the SET Capabilities
/*SUPL_END is sent after SUPL_POSINIT message from SET*/
    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSLOCFWD 1 1 30 60 100 5 1 portalnmms");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003a02000080004c000404850d961fe47ba240216b6800095a3a04a05021080200080200000025ccaf000ae0218400009e1bf974c3b376ddbcc0 58 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A020000C0004C000404850D961FE400000004605400119004 26");//SUPL_RESP                             
    RECV("SUPL LBSDATARECV 1 0035020000C0004C000404850D961FE400000004605400119A2E810085ADA0002568E812814084202DF00000000800200800000090 53 10");//SUPL_POSINT
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}


static int LBSUPLCON113test8(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    /* straight test case as per expected behaviour triggered start*/
    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSLOCFWD 1 1 30 60 100 5 1 portalnmms");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    
    RECV("SUPL LBSDATARECV 1 0049020000C0004EAAEF3374004488CC02F8B1E0055817084017258BFA4020EA4801095A02108200080200000026DC6D34378E974D900002786FE5D30ECDDB76F3000000240000EC00 73 15");//SUPL_TRIGGERED_START
    //TEST_SUPL_WAIT_MAX_TIMEOUT;
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}

static int LBSUPLCON111test1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    /*to test the third party retrieval with valid third party id, valid name, valid QoP */
    TCBEGIN(handle, __func__);
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSLOCRET 1 1 30 60 100 5 2 AABBCCDD");//third party id mdn- and name is AABBCCDD00112233
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    SEND("SUPL LBSDATARECV 1 001B02000080004C00404048D159E27E14240555DE66E889119A20 27 10");//SUPL_SET_INIT
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


static int LBSUPLCON111test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    /*to test the third party retrieval with invalid third party id, valid name, valid QoP */
    TCBEGIN(handle, __func__);
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSLOCRET 1 1 30 60 100 5 25 AABBCCDD");//third party id invalid- and name is AABBCCDD
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

static int LBSUPLCON111test3(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    /*to test the third party retrieval with valid third party id, invalid name, valid QoP */
    TCBEGIN(handle, __func__);
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSLOCRET 1 1 30 60 100 5 2 0");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

static int LBSUPLCON111test4(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    /*to test the third party retrieval with valid third party id and valid character name, valid QoP but SUPL_END is sent after 65secs*/
    /*Expected result is SUPL_END should be sent by SET at expiry of 60 secs*/
    TCBEGIN(handle, __func__);
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSLOCRET 1 1 30 60 100 5 2 AABBCCDD");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    SEND("SUPL LBSDATARECV 1 001B02000080004C00404048D159E27E14240555DE66E889119A20 27 10");//SUPL_SET_INIT
    //After SUPL_SET_INIT message SET starts 60 sec UT9 timer and waits for SUPL_END from SLP
    TEST_SUPL_WAIT_TIMEOUT(61);
    SEND("SUPL LBSDATARECV 1 001202000080098c000404850d961fe54860 19 10");//SUPL_END
    RECV("SUPL LBSDISREQ 1");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

static int LBSUPLCON111test5(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    /*Valid Third Party ID, valid Third party Name, valid QoP */
    TCBEGIN(handle, __func__);
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSLOCRET 1 1 30 60 100 5 2 AABBCCDD");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    SEND("SUPL LBSDATARECV 1 004002000080004c000404850d961fe614b9820a0a12121a1a22238e4400c81bf85bbf6e15b87870d9a71e1e9a77ee75d76705dd709aa72d3cfa61e4992e6600 64 10");//SUPL_SET_INIT
    //After SUPL_SET_INIT message SET starts 60 sec UT9 timer and waits for SUPL_END from SLP
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


static int LBSUPLCON111test6(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    /*Valid Third Party ID, valid Third party Name and QoP valid*/
    TCBEGIN(handle, __func__);
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSLOCRET 1 1 30 60 100 5 2 AABBCCDD");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    SEND("SUPL LBSDATARECV 1 004002000080004c000404850d961fe614b9820a0a12121a1a22238e4400c81bf85bbf6e15b87870d9a71e1e9a77ee75d76705dd709aa72d3cfa61e4992e6600 64 10");//SUPL_SET_INIT
    //After SUPL_SET_INIT message SET starts 60 sec UT9 timer and waits for SUPL_END from SLP
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


static int LBSUPLCON111test7(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    /*Valid Third Party ID, valid Third party Name and QoP invalid*/
    TCBEGIN(handle, __func__);
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSLOCRET 1 10000 100000 100000 100000 5 2 AABBCCDD");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    SEND("SUPL LBSDATARECV 1 004002000080004c000404850d961fe614b9820a0a12121a1a22238e4400c81bf85bbf6e15b87870d9a71e1e9a77ee75d76705dd709aa72d3cfa61e4992e6600 64 10");//SUPL_SET_INIT
    //After SUPL_SET_INIT message SET starts 60 sec UT9 timer and waits for SUPL_END from SLP
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


static int LBSUPLCON111test8(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    /*Valid Third Party ID, valid Third party Name and QoP NOT present*/
    TCBEGIN(handle, __func__);
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSLOCRET 1 1 -1 -1 -1 -1 2 AABBCCDD");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    SEND("SUPL LBSDATARECV 1 004002000080004c000404850d961fe614b9820a0a12121a1a22238e4400c81bf85bbf6e15b87870d9a71e1e9a77ee75d76705dd709aa72d3cfa61e4992e6600 64 10");//SUPL_SET_INIT
    //After SUPL_SET_INIT message SET starts 60 sec UT9 timer and waits for SUPL_END from SLP
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

static int LBSUPLCON111test9(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    /*No Third Party id, No third party Name, No Qop */
    TCBEGIN(handle, __func__);
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSLOCRET 1 0 0 0 0 0 -1 0 ");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

static int LBSUPLCON010(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 00100100004000000046054001180010 16");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 0030010000C0004C000404850D961FE400000004605400119A4E093DF00000000800200800000114564810562527D260 48 10");//SUPL_POSINIT
    SEND("SUPL LBSDATASEND 1 003301FFFFC0004C000404850D961FE40000000460540011AA01AC19B072C593262CD9307369CCB01938D86140000000003600 51");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}




/* MOLR */
static int LBSUPLCON165(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 001F01000080004C000404850D961FE453824200080200000045CCAF000AE0 36 10");//SUPL_START
    TEST_SUPL_WAIT_TIMEOUT(10);
    RECV("SUPL LBSDATARECV 1 001201000080004C000404850D961FE54860 18 5");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}


static int LBSUPLCON000(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 001001FFFF4000000046054001180000 16");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 0030010000C0004C000404850D961FE400000004605400119A4E092DF0000000080020080000011E33CAA066CB970610 48 10");//SUPL_POSINIT
    SEND("SUPL LBSDATASEND 1 003301FFFFC0004C000404850D961FE40000000460540011AA01AC19B072C593262CD8B46D69CCB01938D86140000000003100 51");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");


    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}





static int LBSUPLCON279(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 00700100004000000206054001182007C45BFDFE00A600E000D200E400CA00DC00E80040008600DE00DA00DA00EA00DC00D200C600C200E800D200DE00DC00E6F6FF7F802980380034803900328037003A00100021803780368036803A8037003480318030803A0034803780370039B8 112");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 0031010000C0020C000404850D961FE400000020605400119A4E892DF000000009002008000001000AC9546840C571C868 49 10");//SUPL_POSINIT
    SEND("SUPL LBSDATASEND 1 0033010000C0020C000404850D961FE40000002060540011AA01AC19B072C593262CDBB07169CCB019B8D86140000000002C00 51");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}


static int LBSUPLCON166(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 002001000080004C000404850D961FE45382404010040004000000B995E0015C 32 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A010000C0004C000404850D961FE400000004605400119000 26");//SUPL_RESP
    RECV("SUPL LBSDATARECV 1 0029010000C0004C000404850D961FE400000004605400119A0E092DF0000000010040100010000002 41 10");//SUPL_POSINT
    TEST_SUPL_WAIT_TIMEOUT(10);
    RECV("SUPL LBSDATARECV 1 001A010000C0004C000404850D961FE40000000460540011A90C 26 5");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");


    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}

static int LBSUPLCON168(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 002001000080004C000404850D961FE45382404010040008000000B995E0015C 32 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A010000C0004C000404850D961FE400000004605400119000 26");//SUPL_RESP
    RECV("SUPL LBSDATARECV 1 0029010000C0004C000404850D961FE400000004605400119A0E092DF0000000010040100020000002 41 10");//SUPL_POSINT
    TEST_SUPL_WAIT_TIMEOUT(10);
    RECV("SUPL LBSDATARECV 1 001A010000C0004C000404850D961FE40000000460540011A90C 26 5");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}


static int LBSUPLCON150test1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 002001000080004C000404850D961FE45382404010040008000000B995E0015C 32 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A010000C0004C000404850D961FE400000004605400119000 26");//SUPL_RESP
    RECV("SUPL LBSDATARECV 1 0029010000C0004C000404850D961FE400000004605400119A0E092DF0000000010040100010000002 41 10");//SUPL_POSINT
    SEND("SUPL LBSDATASEND 1 0033010000C0004C000404850D961FE40000000460540011AA01AC19B072C593262CD8B36D69CCB01938D86140000000003300 51");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");


     TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}

static int LBSUPLCON150test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 002001000080004C000404850D961FE45382404010040008000000B995E0015C 32 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A010000C0004C000404850D961FE400000004605400119000 26");//SUPL_RESP
    RECV("SUPL LBSDATARECV 1 0029010000C0004C000404850D961FE400000004605400119A0E092DF0000000010040100020000002 41 10");//SUPL_POSINT
    SEND("SUPL LBSDATASEND 1 0035010000C0004C000404850D961FE40000000460540011AA21AC19B072C593262CDCB565698CB018F8D861000000008010000000 51");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");


    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}


static int LBSUPLCON150test3(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 002001000080004C000404850D961FE45382404010040004000000B995E0015C 32 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A010000C0008C000404850D961FE400000004605400119000 26");//SUPL_RESP
    RECV("SUPL LBSDATARECV 1 0029010000C0008C000404850D961FE400000004605400119A0E092DF0000000010040100010000002 41 10");//SUPL_POSINT
    SEND("SUPL LBSDATASEND 1 003A010000C0008C000404850D961FE40000000460540011AA21AC19B072C593262CD8B26569CCB01938D86140000000003200D0080000000000 58");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}


static int LBSUPLCON381(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 002001000080004C000404850D961FE45382404010040008000000B995E0015C 32 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 0033010000C0004C000404850D961FE40000000060540011AA01AC19B072C593262CD8306B69CCB01938D861400000B4803200 51");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}


static int LBSUPLCON130test1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 002001000080004C000404850D961FE45382404010040004000000B995E0015C 32 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A010000C0004C000404850D961FE400000004605400119000 26");//SUPL_RESP
    RECV("SUPL LBSDATARECV 1 0029010000C0004C000404850D961FE400000004605400119A0E092DF0000000010040100010000002 41 10");//SUPL_POSINT
    SEND("SUPL LBSDATASEND 1 0033010000C0004C000404850D961FE40000000460540011AA01AC19B072C593262CD8B56369CCB01A38D86180000000003800 51");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}


static int LBSUPLCON130test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 002001000080004C000404850D961FE45382404010040004000000B995E0015C 32 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A010000C0004C000404850D961FE400000004605400119004 26");//SUPL_RESP
    RECV("SUPL LBSDATARECV 1 0029010000C0004C000404850D961FE400000004605400119A0E092DF0000000010040100010000002 41 10");//SUPL_POSINT
    TEST_SUPL_WAIT_TIMEOUT(10) ;
    RECV("SUPL LBSDATARECV 1 001A010000C0004C000404850D961FE40000000460540011A800 26 5");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}


static int LBSUPLCON130test3(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__);
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 002001000080004C000404850D961FE45382404010040008000000B995E0015C 32 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A010000C0004C000404850D961FE400000004605400119010 26");//SUPL_RESP
    RECV("SUPL LBSDATARECV 1 0029010000C0004C000404850D961FE400000004605400119A0E092DF0000000010040100020000002 41 10");//SUPL_POSINT
    TEST_SUPL_WAIT_TIMEOUT(10) ;
    RECV("SUPL LBSDATARECV 1 001A010000C0004C000404850D961FE40000000460540011A800 26 5");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}



static int LBSUPLCON131test1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 002001000080004C000404850D961FE45382404010040004000000B995E0015C 32 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 0033010000C0004C000404850D961FE40000000460540011AA01AC19B072C593262CD9336369CCAE75B8E38E5E3C00B40064C8 51");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}


static int LBSUPLCON131test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 002001000080004C000404850D961FE45382404010040004000000B995E0015C 32 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 0033010000C0004C000404850D961FE40000000460540011AA01AC19B072C593262CD9336369CCAE75B8E38E5E3C00B40064C8 51");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}


static int LBSUPLCON132(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 002001000080004C000404850D961FE45382404010040008000000B995E0015C 32 10");//SUPL_START
    TEST_SUPL_WAIT_TIMEOUT(10);
    RECV("SUPL LBSDATARECV 1 001A010000C0004C000404850D961FE40000000460540011A90E 26 5");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}


static int LBSUPLCON106test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 002001000080004C000404850D961FE45382404010040004000000B995E0015C 32 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A010000C0004C000404850D961FE400000004605400119000 26");//SUPL_RESP
    RECV("SUPL LBSDATARECV 1 0029010000C0004C000404850D961FE400000004605400119A0E092DF0000000010040100010000002 41 10");//SUPL_POSINT
    SEND("SUPL LBSDATASEND 1 0033010000C0004C000404850D961FE40000000460540011AA01AC19B072C593262CD8B16969CCB01978D86140000000003700 51");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}


static int LBSUPLCON106test3(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 002001000080004C000404850D961FE45382404010040008000000B995E0015C 32 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A010000C0004C000404850D961FE400000004605400119000 26");//SUPL_RESP
    RECV("SUPL LBSDATARECV 1 0029010000C0004C000404850D961FE400000004605400119A0E092DF0000000010040100020000002 41 10");//SUPL_POSINT
    SEND("SUPL LBSDATASEND 1 0037010000C0004C000404850D961FE40000000460540011AA21AC19B072C593262CD83563698CB018F8D86180000000D0080000000000 55");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}


static int LBSUPLCON102test1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 002001000080004C000404850D961FE45382404010040004000000B995E0015C 32 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A010000C0004C000404850D961FE400000004605400119004 26");//SUPL_RESP
    RECV("SUPL LBSDATARECV 1 0029010000C0004C000404850D961FE400000004605400119A0E092DF0000000010040100010000002 41 10");//SUPL_POSINT
    TEST_SUPL_WAIT_TIMEOUT(10);
    RECV("SUPL LBSDATARECV 1 001A010000C0004C000404850D961FE40000000460540011A800 26 5");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}




static int LBSUPLCON103test1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 002001000080004C000404850D961FE45382404010040004000000B995E0015C 32 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A010000C0004C000404850D961FE400000004605400119000 26");//SUPL_RESP
    TEST_SUPL_WAIT_TIMEOUT(10);
    RECV("SUPL LBSDATARECV 1 001A010000C0004C000404850D961FE40000000460540011A918 26 5");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}



static int LBSUPLCON103test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 002001000080004C000404850D961FE45382404010040008000000B995E0015C 32 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A010000C0004C000404850D961FE400000004605400119000 26");//SUPL_RESP
    TEST_SUPL_WAIT_TIMEOUT(10);
    RECV("SUPL LBSDATARECV 1 001A010000C0004C000404850D961FE40000000460540011A918 26 5");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}



static int LBSUPLCON104test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 002001000080004C000404850D961FE45382404010040008000000B995E0015C 32 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A010000C0004C000404850D961FE400000004605400119000 26");//SUPL_RESP
    RECV("SUPL LBSDATARECV 1 0029010000C0004C000404850D961FE400000004605400119A0E092DF0000000010040100020000002 41 10");//SUPL_POSINT
    TEST_SUPL_WAIT_TIMEOUT(10);
    RECV("SUPL LBSDATARECV 1 001A010000C0004C000404850D961FE40000000460540001A918 26 5");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}

static int LBSUPLCON102test3(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 002001000080004C000404850D961FE45382404010040004000000B995E0015C 32 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 0025010000C0004C000404850D961FE40000000512F3CF01E35BB7AB3E40C68E01A34C8802 37");//SUPL_RESP
    RECV("SUPL LBSDATARECV 1 0034010000C0004C000404850D961FE40000000512F3CF01E35BB7AB3E40C68E01A34C8D070496F8000000008020080008000001 52 10");//SUPL_POSINT
    SEND("SUPL LBSDATASEND 1 0025010000C0004C000404850D961FE40000000512F3CF01E35BB7AB3E40C68E01A34C9400 37");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}



static int LBSUPLCON104test1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 002001000080004C000404850D961FE45382404010040008000000B995E0015C 32 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A010000C0004C000404850D961FE400000004605400119000 26");//SUPL_RESP
    RECV("SUPL LBSDATARECV 1 0029010000C0004C000404850D961FE400000004605400119A0E092DF0000000010040100020000002 41 10");//SUPL_POSINT
    TEST_SUPL_WAIT_TIMEOUT(10);
    RECV("SUPL LBSDATARECV 1 001A010000C0004C000404850D961FE40000000860540011A918 26 5");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}


static int LBSUPLCON100test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 002001000080004C000404850D961FE45382404010040008000000B995E0015C 32 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A010000C0004C000404850D961FE400000004605400119000 26");//SUPL_RESP
    RECV("SUPL LBSDATARECV 1 0029010000C0004C000404850D961FE400000004605400119A0E092DF0000000010040100020000002 41 10");//SUPL_POSINT
    SEND("SUPL LBSDATASEND 1 0033010000C0004C000404850D961FE40000000460540011AA01AC19B072C593262CD8356169CCB01978D86100000000003000 51");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}


static int LBSUPLCON100test1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 002001000080004C000404850D961FE45382404010040004000000B995E0015C 32 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A01FFFFC0004C000404850D961FE400000004605400119000 26");//SUPL_RESP
    RECV("SUPL LBSDATARECV 1 0029010000C0004C000404850D961FE400000004605400119A0E092DF0000000010040100010000002 41 10");//SUPL_POSINT
    SEND("SUPL LBSDATASEND 1 003301FFFFC0004C000404850D961FE40000000460540011AA01AC19B072C593262CD8B06569CCB01978D86140000000003200 51");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}


static int LBSUPLCON060test1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 000F0100004000000046054001180000 17");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    TEST_SUPL_WAIT_TIMEOUT(10);
    RECV("SUPL LBSDATARECV 1 0022010000C0008C000408850D961FE40000000460540011A9877782FFBEFA0BAB1C 34 10");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}


static int LBSUPLCON062(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 001001000040000000460540011800A0 16");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSDATASEND 1 0022010000C0008C000408850D961FE40000000460540011A9877782FFBEFA0BAB1C 34");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}


static int LBSUPLCON063(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 00100100004000000046054001180000 16");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 0031010000C0004C000404850D961FE400000004605400119A4E892DF000000009002008000001000DC74D7A986C3F2F28 49 10");//SUPL_POSINIT
    SEND("SUPL LBSDATASEND 1 001A010000C0004C000404850D961FE400000004605400119000 26");//SUPL_RESP
    RECV("SUPL LBSDATARECV 1 001A010000C0004C000404850D961FE40000000460540011A904 26 10");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}



static int LBSUPLCON068(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 00100100004000000046054001180000 16");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 0031010000C0004C000404850D961FE400000004605400119A4E892DF000000009002008000000800DC74D7A986C3F2F28 49 10");//SUPL_POSINIT
    TEST_SUPL_WAIT_TIMEOUT(10);
    RECV("SUPL LBSDATARECV 1 001A010000C0004C000404850D961FE40000000460540011A90C 26 10");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}



static int LBSUPLCON270test1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 00100100004000000046054001180000 17");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 0031010000C0004C000404850D961FE400000004605400119A4E892DF000000009002008000001000DC74D7A986C3F2F28 49 10");//SUPL_POSINIT
    SEND("SUPL LBSDATASEND 1 0033010000C0004C000404850D961FE40000000460540011AA01AC19B072C593262CD9326569CCB019B8D86100000000002D00 51");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}


static int LBSUPLCON270test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 0011010000400000004605400118200000 17");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003D010000C0008C000408850D961FE400000004605400119A4E892DF4D38F31942000D000C0028004801600120040100000010019C08A1AF1D2057490 61 10");//SUPL_POSINIT
    SEND("SUPL LBSDATASEND 1 0033010000C0008C000408850D961FE40000000460540011AA01AC19B072C593262CD9307169CCB01938D86140000000003000 51");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}



static int LBSUPLCON271(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 0011010000400000004605400118200004 17");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003D010000C000CC000408850D961FE400000004605400119A4E892DF4D38F31942000D000C002800B00090012004010000002001164F8DAD7B8D34EB0 61 10");//SUPL_POSINIT
    SEND("SUPL LBSDATASEND 1 0033010000C000CC000408850D961FE40000000460540011AA01AC19B072C593262CD8B46169CCB01978D86100000000002D00 51");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}




static int LBSUPLCON272(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 0011010000400000004605400118200008 17");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 0031010000C0004C000404850D961FE400000004605400119A4E892DF0000000090020080000008009E51E9FC506F191C8 49 10");//SUPL_POSINIT
    SEND("SUPL LBSDATASEND 1 0033010000C0004C000404850D961FE40000000460540011AA01AC19B072C593262CDA346369CCB01938D86140000000002F00 51");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}



static int LBSUPLCON273(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 0011010000400000004605400118200008 17");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    SEND("SUPL LBSDATASEND 1 0022010000C0004C000404850D961FE40000000460540011A9A47947A7F141BC6472 34");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}



static int LBSUPLCON274(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 0011010000400000004605400118200008 17");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 0031010000C0008C000408850D961FE400000004605400119A4E892DF0000000090020080000008009E51E9FC506F191C8 49 10");//SUPL_POSINIT
    SEND("SUPL LBSDATASEND 1 0033010000C0008C000408850D961FE40000000460540011AA01AC19B072C593262CD8B36F69CCB01938D86180000000003100 51");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}




static int LBSUPLCON275(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 001101000040000000460540011820000C 17");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003D010000C000CC000408850D961FE400000004605400119A4E892DF4D38F31942000D000C0028004801600120040100000020014EE6C2904D1096080 61 10");//SUPL_POSINIT
    SEND("SUPL LBSDATASEND 1 0033010000C000CC000408850D961FE40000000460540011AA01AC19B072C593262CD8B27369CCB019B8D86140000000003000 51");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}




static int LBSUPLCON276(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 001101000040000000460540011820000C 17");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSDATASEND 1 0022010000C0004C000404850D961FE40000000460540011A9A49DCD85209A212C10 34");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}


static int LBSUPLCON277(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 0011010000400000004605400118200010 17");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSDATASEND 1 0022010000C0004C000404850D961FE40000000460540011A9A49DCD85209A212C10 34");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}



static int LBSUPLCON278(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 0011010000400000004605400118200010 17");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 0031010000C0004C000404850D961FE400000004605400119A4E892DF000000009002008000000800CC98312E3A92B3ED8 49 10");//SUPL_POSINIT
    SEND("SUPL LBSDATASEND 1 0033010000C0004C000404850D961FE40000000460540011AA01AC19B072C593262CD8B46569CCB01938D86140000000002E00 51");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}



static int LBSUPLCON033(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 0011010000400000004605400118209004 17");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    SEND("SUPL LBSDATASEND 1 0022010000C0004C000404850D961FE40000000460540011A8D8F6F3B0E8C70CC000 34");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}



static int LBSUPLCON006test1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 00700100004000000046054001182007C45BFFFCA600E000D200E400CA00DC00E80040008600DE00DA00DA00EA00DC00D200C600C200E800D200DE00DC00E60016FFFF2980380034803900328037003A00100021803780368036803A8037003480318030803A00348037803700398000 112");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 0031010000C0004C000404850D961FE400000004605400119A4E892DF000000009002008000001000ECFBB1F272B3682E0 49 10");//SUPL_POSINIT
    SEND("SUPL LBSDATASEND 1 0033010000C0004C000404850D961FE40000000460540011AA01AC19B072C593262CD9B07169CCB018B8D86100000000003A00 51");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}



static int LBSUPLCON006test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 001501000040000000460540011808071428002A80 21");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003D010000C0008C000408850D961FE400000004605400119A4E892DF4D38F31942000D000C0028004801600120040100000010014EEC5716E7CB34290 61 10");//SUPL_POSINIT
    SEND("SUPL LBSDATASEND 1 0033010000C0008C000408850D961FE40000000460540011AA01AC19B072C593262CD8B36B69CCB018F8D86140000000003400 51");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}






static int LBSUPLCON279test1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 00700100004000000206054001182007C45BFDFE00A600E000D200E400CA00DC00E80040008600DE00DA00DA00EA00DC00D200C600C200E800D200DE00DC00E6F6FF7F802980380034803900328037003A00100021803780368036803A8037003480318030803A0034803780370039B8 112");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 0031010000C0020C000408850D961FE400000020605400119A4E892DF000000009002008000000800AC9546840C571C868 49 10");//SUPL_POSINIT
    SEND("SUPL LBSDATASEND 1 0033010000C0020C000408850D961FE40000002060540011AA01AC19B072C593262CDB316169CCB01938D86140000000003600 51");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}


static int LBSUPLCON279test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 003C0100004000000206054001182007C4A6A6F0B4BCEDA68386EF76BAEF4F8EC2F5F5DB3C06E9A9BC2D2F3B69A0E1BBDDAEBBD3E3B0BD7D76CF01B8 60");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 0031010000C0020C000408850D961FE400000020605400119A4E892DF000000009002008000000800B714041EDE02779F0 49 10");//SUPL_POSINIT
    SEND("SUPL LBSDATASEND 1 0033010000C0020C000408850D961FE40000002060540011AA01AC19B072C593262CDAB56769CCB01938D86180000000002D00 51");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}


static int LBSUPLCON279test3(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 00400100004000000206054001182007C52AA6E0D2E4CADCE84086DEDADAEADCD2C6C2E8D2DEDCE6EAA9B834B932B73A1021B7B6B6BAB734B1B0BA34B7B739B8 64");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 0031010000C0020C000408850D961FE400000020605400119A4E892DF0000000090020080000010008F4F225F298A0E4C8 49 10");//SUPL_POSINIT
    SEND("SUPL LBSDATASEND 1 0033010000C0020C000408850D961FE40000002060540011AA01AC19B072C593262CDAB56769CCB018F8D86180000000003200 51");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}

static int LBSUPLCON023(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 00100100004000000046054001180000 16");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 0031010000C0004C000404850D961FE400000004605400119A4E892DF000000009002008000001000DC74D7A986C3F2F28 49 10");//SUPL_POSINIT
    SEND("SUPL LBSDATASEND 1 0033010000C0004C000404850D961FE40000000460540011AA01AC19B072C593262CD8B56169CCB01938D86100000000003100 51");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}


static int LBSUPLCON032(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 00100100004000000046054001180050 16");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 0031010000C0004C000404850D961FE400000004605400119A4E092DF000000009002008000000800A51ADD9EB7495F0D0 49 10");//SUPL_POSINIT
    SEND("SUPL LBSDATASEND 1 0033010000C0004C000404850D961FE40000000460540011AA01AC19B072C593262CD9B06B69CCB018F8D86100000000003500 51");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;


    return TSTENG_RESULT_OK;

}


//------------------------------------------------------------------//


static int LBSUPLCON013(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 00100100004000000046054001180010 16");//SUPL_INIT
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


static int LBSUPLCON000test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 00100100004000000046054001180000 16");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003B010000C0020C000408850D961FE400000004605400119A4E892DF000000009802008000000990708B54345BB018C8D3E371D35EA61B0FCBCA0 59 10");//SUPL_POSINIT
    sleep(2);
    SEND("SUPL LBSDATASEND 1 0033010000C0020C000408850D961FE40000000460540011AA01AC19B072C593262CD8356F69CCB01938D86140000000003300 51");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


static int LBSUPLCON060(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 000F0100004000000046054001180000  16");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    sleep(3);
    RECV("SUPL LBSDATARECV 1 0022010000C0028C000408850D961FE40000000460540011A9869DAE0ED4331AB9CC 34 10");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;


    return TSTENG_RESULT_OK;
}


static int LBSUPLCON066(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 00100100004000000046054001180000 16");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003B010000C0034C000408850D961FE400000004605400119A4E892DF000000009802008000001194708B54345BB018CAD3E371D35EA61B0FCBCA0 59 10");//SUPL_POSINIT
    TEST_SUPL_WAIT_TIMEOUT(10);
    RECV("SUPL LBSDATARECV 1 001A010000C0034C000408850D961FE40000000460540011A90C 26 8");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


static int LBSUPLCON067(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 00100100004000000046054001180060 16");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003B010000C0038C000408850D961FE400000004605400119A4E892DF000000009802008000000990708B54345BB018C8D3E260101F387E1464520 59 10");//SUPL_POSINIT
    TEST_SUPL_WAIT_TIMEOUT(10);
    RECV("SUPL LBSDATARECV 1 001A010000C0038C000408850D961FE40000000460540011A90C 26 8");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;


    return TSTENG_RESULT_OK;
}



static int LBSUPLCON002test1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 00100100004000000046054001180000 16");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003B010000C004CC000408850D961FE400000004605400119A4E892DF000000009802008000001194708B54345BB018CAD3E371D35EA61B0FCBCA0 59 10");//SUPL_POSINIT
    sleep(3);
    SEND("SUPL LBSDATASEND 1 0033010000C004CC000408850D961FE40000000460540011AA01AC19B072C593262CD8B26969CCB01938D86180000000002F00 51");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;


    return TSTENG_RESULT_OK;

}

static int LBSUPLCON002test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 001B01000040000000512F3CF01E35BB7AB3E40C68E01A34C80000 27");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 0046010000C0050C000408850D961FE40000000512F3CF01E35BB7AB3E40C68E01A34C8D274496F800000004C010040000004C83845AA1A2DD80C6469F1C0F61B8532C615B50 70 10");//SUPL_POSINIT
    sleep(2);
    SEND("SUPL LBSDATASEND 1 003E010000C0050C000408850D961FE40000000512F3CF01E35BB7AB3E40C68E01A34C9500D60CD83962C993166C1AB7B4E6580C9C6C30A0000000001880 62");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;


    return TSTENG_RESULT_OK;
}




static int LBSUPLCON003test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 00100100004000000046054001180000 16");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003B010000C0058C000408850D961FE400000004605400119A4E892DF000000009802008000000990708B54345BB018C8D3E371D35EA61B0FCBCA0 59 10");//SUPL_POSINIT
    TEST_SUPL_WAIT_TIMEOUT(10);
    RECV("SUPL LBSDATARECV 1 001A010000C005CC000408850D961FE40000000460540011A918 26 10");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;
}


static int LBSUPLCON003test3(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 00100100004000000046054001180000 16");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003B010000C005CC000408850D961FE400000004605400119A4E892DF000000009802008000001194708B54345BB018CACBE371D35EA61B0FCBCA0 59 10");//SUPL_POSINIT
    TEST_SUPL_WAIT_TIMEOUT(10);
    RECV("SUPL LBSDATARECV 1 001A010000C005CC000008850D961FE40000000460540011A918 26 10");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;
}



static int LBSUPLCON004test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 00100100004000000046054001180000 16");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003B010000C0064C000408850D961FE400000004605400119A4E892DF000000009802008000001194708B54345BB018CACBE371D35EA61B0FCBCA0 59 10");//SUPL_POSINIT
    TEST_SUPL_WAIT_TIMEOUT(10);
    RECV("SUPL LBSDATARECV 1 001A010000C0064C000408850D961FE40000000860540011A918 26 10");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}

static int LBSUPLCON004test3(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 00100100004000000046054001180000 16");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003B010000C0068C000408850D961FE400000004605400119A4E892DF000000009802008000000990708B54345BB018C8D3E371D35EA61B0FCBCA0 59 10");//SUPL_POSINIT
    TEST_SUPL_WAIT_TIMEOUT(10);
    RECV("SUPL LBSDATARECV 1 001A010000C0068C000408850D961FE40000000460540001A918 26 10");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}



static int LBSUPLCON030test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 00100100004000000046054001180010 16");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003B010000C0024C000408850D961FE400000004605400119A4E892DF000000009802008000001194708B54345BB018CAD3E28AC9020AC4A4FA4C0 59 10");//SUPL_POSINIT
    TEST_SUPL_WAIT_TIMEOUT(10);
    RECV("SUPL LBSDATARECV 1 001A010000C0024C000408850D961FE40000000460540011A800 26 10");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}


static int LBSUPLCON030test3(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 00100100004000000046054001180040 16");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003B010000C0028C000408850D961FE400000004605400119A4E892DF000000009802008000000990708B54345BA018C8CBD351DAFDD0CF8D8CAE0 59 10");//SUPL_POSINIT
    SEND("SUPL LBSDATASEND 1 0025010000C0028C000408850D961FE40000000460540011A08013C2024DF0282010001C00 37");//SUPL_pos
    TEST_SUPL_WAIT_TIMEOUT(10);
    RECV("SUPL LBSDATARECV 1 001A010000C0028C000408850D961FE40000000460540011A800 26 10");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}


static int LBSUPLCON032test1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 00100100004000000046054001180050 16");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003B010000C002CC000408850D961FE400000004605400119A4E892DF000000009802008000000990708B54345BB018C8CBE2946B767ADD257C340 59 10");//SUPL_POSINIT
    sleep(2);
    SEND("SUPL LBSDATASEND 1 0033010000C002CC000408850D961FE40000000460540011AA01AC19B072C593262CD9306F69CCB018B8D86140000000003400 51");//SUPL_pos
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}


static int LBSUPLCON032test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 00100100004000000046054001180000 16");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003B010000C003CC000408850D961FE400000004605400119A4E892DF000000009802008000001194708B54345BB018CAD3E371D35EA61B0FCBCA0 59 10");//SUPL_POSINIT
    sleep(5);
    RECV("SUPL LBSDATARECV 1 001A010000C003CC000408850D961FE40000000460540011A90E 26 10");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}


static int LBSUPLCON031test3(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 00100100004000000046054001180020 16");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003B010000C0044C000408850D961FE400000004605400119A4E892DF000000009802008000001194708B54345BB018CAD3E35BAF6924F48F078A0 59 10");//SUPL_POSINIT
    sleep(2);
    SEND("SUPL LBSDATASEND 1 0033010000C0044C000408850D961FE40000000460540011AA01AC19B072C593262CD8346969CCB01938D86100000000003100 51");//SUPL_pos
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}


static int LBSUPLCON030test5(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 00100100004000000046054001180060 16");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003B010000C0040C000408850D961FE400000004605400119A4E892DF000000009802008000000990708B54345BB018C8CBE260101F387E1464520 59 10");//SUPL_POSINIT
    sleep(2);
    SEND("SUPL LBSDATASEND 1 003B010000C0040C000408850D961FE40000000460540011AA81AC19B072C593262CD8346B69CCAE75B8E38E5E3C00B40064C860101F387E146452 59");//SUPL_pos
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}

static int LBSUPLCON282(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 002A0100008000CC000408850D961FE453A2426008020000004651C22D50D16EC0632B4F8B995E0015C0 42 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A010000C000CC000408850D961FE400000004605400119018 26");//SUPL_RESP
    RECV("SUPL LBSDATARECV 1 0033010000C000CC000408850D961FE40000000460540011AA0E892DF000000009802008000001194708B54345BB018CAD3E20 51 10");//SUPL_POSINT
    sleep(2);
    SEND("SUPL LBSDATASEND 1 0033010000C000CC000408850D961FE40000000460540011AA01AC19B072C593262CD8356B69CCAE75B8E38E5E3C00B40064C8 51");//SUPL_SEND
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}



static int LBSUPLCON281(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSmsPush 1 001001FFFF4000000046054001180010 16");//SUPL_INIT
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003B010000C0010C000408850D961FE400000004605400119A4E892DF000000009802008000000990708B54345B9018C8D3C2A127B88F73D141BE0 59 10");//SUPL_POSINIT
    sleep(2);
    SEND("SUPL LBSDATASEND 1 003301FFFFC0010C000408850D961FE40000000460540011AA01AC19B072C593262CD8356169CCB01938D86143064A86003114 51");//SUPL_pos
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}



/*====================================================================================================================================================================================
                                                                               SUPL 2.0 tests
  ====================================================================================================================================================================================*/

/************************************
Network Initiated Periodic Reporting - Real
 ************************************/

static int LBSUPLCON0040a(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSmsPush 1 0014020000400000004605400118401008108000 20");//SUPL_INIT
    SEND("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    SEND("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    /* SUPL TRIGGERED START */
    SEND("SUPL LBSDATARECV 1 003B020000C0004EAAEF3374004488CC02F8B1E0055817084010212BFA4020EA4801095A02108200080200000026DC6D34378E974D9040084AD000 59 10");
    /* SUPL TRIGGERED RESPONSE */
    SEND("SUPL LBSDATASEND 1 0026020000C0004C000404850D961FE40000000460540011C085A00220000120000760000F00 38");

    TEST_SUPL_WAIT_TIMEOUT(30);
    /* SUPL POS INIT */
    SEND("SUPL LBSDATARECV 1 0031010000C0004C000404850D961FE400000004605400119A4E892DF000000009002008000001000DC74D7A986C3F2F28 49 10");
    /* SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


static int LBSUPLCON0061(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSmsPush 1 0014020000402F8B1E0055817080401008108000 20");//SUPL_INIT
    SEND("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    SEND("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    /* SUPL TRIGGERED START */
    SEND("SUPL LBSDATARECV 1 003B020000C0004EAAEF3374004488CC02F8B1E0055817084010212BFA4020EA4801095A02108200080200000026DC6D34378E974D9040084AD000 59 10");
    /* SUPL TRIGGERED RESPONSE - MSB, 10 reports, start time = 30 sec, interval = 60 sec,  Actual no of fixes according to test specification is 50, but setting it to 10 */
    SEND("SUPL LBSDATASEND 1 0026020000C0004EAAEF3374004488CC02F8B1E0055817084085A00220000120000760000F00 38");
    /* SUPL TRIGGERED STOP*/
    SEND("SUPL LBSDATASEND 1 001D020000C0004EAAEF3374004488CC02F8B1E0055817084101308000 29");
    /* SUPL END */
    SEND("SUPL LBSDATARECV 1 0033010000C000CC000408850D961FE40000000460540011AA01AC19B072C593262CD8B46169CCB01978D86100000000002D00 51 10");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


static int LBSUPLCON0070(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    TCBEGIN(handle, __func__) ;
    /* This is to ensure that SUPL POS INIT is sent in the first report */
    SEND("NAF LBSCFGDEL 0xFFFF");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSmsPush 1 0014020000400000004605400118401008108000 20");//SUPL_INIT
    SEND("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    SEND("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    /* SUPL TRIGGERED START */
    SEND("SUPL LBSDATARECV 1 003B020000C0004EAAEF3374004488CC02F8B1E0055817084010212BFA4020EA4801095A02108200080200000026DC6D34378E974D9040084AD000 59 10");
    /* SUPL TRIGGERED RESPONSE - MSB, 10 reports, start time = 10 sec,(Spec says 30 secs) This is to ensure we get SUPL POS INIT in the first report,
        interval = 60 sec,  Actual no of fixes according to test specification is 50, but setting it to 10 */
    SEND("SUPL LBSDATASEND 1 0026020000C0004C000404850D961FE40000000460540011C085A00220000120000760000F00 38");
    /* SUPL POS INIT after 30 seconds*/
    TEST_SUPL_WAIT_TIMEOUT(30);
    SEND("SUPL LBSDATARECV 1 0031010000C0004C000404850D961FE400000004605400119A4E892DF000000009002008000001000DC74D7A986C3F2F28 49 10");
    /* SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


static int LBSUPLCON0074(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSmsPush 1 0014020000400000004605400118401008108000 20");//SUPL_INIT
    SEND("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    SEND("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    /* SUPL TRIGGERED START */
    SEND("SUPL LBSDATARECV 1 003B020000C0004EAAEF3374004488CC02F8B1E0055817084010212BFA4020EA4801095A02108200080200000026DC6D34378E974D9040084AD000 59 10");
    /* SUPL TRIGGERED RESPONSE  */
    SEND("SUPL LBSDATASEND 1 0026020000C0004C000404850D961FE40000000460540011C085A00220000120000760000F00 38");
    /* SUPL REPORT - First report to received after 30 seconds */
    TEST_SUPL_WAIT_TIMEOUT(30);
    SEND("SUPL LBSDATARECV 1 001E020000C0004EAAEF3374004488CC02F8B1E005581708430188000000 30 10 30");
    /* SUPL END sent by SET */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


/************************************
Set Initiated Periodic Reporting - Real
 ************************************/

static int LBSUPLCON120b(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    /* Periodic fix - C struct, NMEA Mask - 0, 60000 ms interval, 10 fixes (specification says - 50 */
    SEND("NAF LBSSTARTPF 1 0 60000 10");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    
    /* SUPL TRIGGERED START 10 reports, interval = 60 sec*/
    SEND("SUPL LBSDATARECV 1 003E020000C0004EAAEF3374004488CC02F8B1E0055817084011A18BFA4020EA4801095A02108200080200000026DC6D34378E974D9000000090000003E020000C0004EAAEF3374004488CC02F8B1E0055817084011A18BFA4020EA4801095A02108200080200000026DC6D34378E974D9000000090000 62 10");
    /* SUPL TRIGGERED RESPONSE - MSB*/
    SEND("SUPL LBSDATASEND 1 001D020000C0004C000404850D961FE40000000460540011C081000200 29");
    /* SUPL POS INIT */
    SEND("SUPL LBSDATARECV 1 0031010000C0004C000404850D961FE400000004605400119A4E892DF000000009002008000001000DC74D7A986C3F2F28 49 10");
    /* SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


static int LBSUPLCON135(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    /* Periodic fix - C struct, NMEA Mask - 0, 60000 ms interval, 10 fixes (specification says - 50 */
    SEND("NAF LBSSTARTPF 1 0 60000 10");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    
    /* SUPL TRIGGERED START 10 reports, interval = 60 sec*/
    SEND("SUPL LBSDATARECV 1 003E020000C0004EAAEF3374004488CC02F8B1E0055817084011A18BFA4020EA4801095A02108200080200000026DC6D34378E974D9000000090000003E020000C0004EAAEF3374004488CC02F8B1E0055817084011A18BFA4020EA4801095A02108200080200000026DC6D34378E974D9000000090000 62 10");
    /* SUPL TRIGGERED RESPONSE - MSB */
    SEND("SUPL LBSDATASEND 1 001D020000C0004EAAEF3374004488CC02F8B1E0055817084081000200 29");

    /* SUPL POS INIT */
    SEND("SUPL LBSDATARECV 1 0031010000C0004C000404850D961FE400000004605400119A4E892DF000000009002008000001000DC74D7A986C3F2F28 49 10");

    /* SUPL TRIGGERED STOP*/
    SEND("SUPL LBSDATASEND 1 001D020000C0004EAAEF3374004488CC02F8B1E0055817084101308000 29");
    /* SUPL END */
    SEND("SUPL LBSDATARECV 1 0033010000C000CC000408850D961FE40000000460540011AA01AC19B072C593262CD8B46169CCB01978D86100000000002D00 51 10");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


static int LBSUPLCON140(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    /* Periodic fix - C struct, NMEA Mask - 0, 60000 ms interval, 10 fixes (specification says - 50 */
    SEND("NAF LBSSTARTPF 1 0 60000 10");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    
    /* SUPL TRIGGERED START 10 reports, interval = 60 sec*/
    SEND("SUPL LBSDATARECV 1 003E020000C0004EAAEF3374004488CC02F8B1E0055817084011A18BFA4020EA4801095A02108200080200000026DC6D34378E974D9000000090000003E020000C0004EAAEF3374004488CC02F8B1E0055817084011A18BFA4020EA4801095A02108200080200000026DC6D34378E974D9000000090000 62 10");

    /* UT1 */
    TEST_SUPL_WAIT_TIMEOUT(11);
    /* SUPL END sent by SET */
    SEND("SUPL LBSDATARECV 1 0033010000C000CC000408850D961FE40000000460540011AA01AC19B072C593262CD8B46169CCB01978D86100000000002D00 51 10");
    RECV("SUPL LBSDISREQ 1");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


static int LBSUPLCON141(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    /* Periodic fix - C struct, NMEA Mask - 0, 60000 ms interval, 10 fixes (specification says - 50 */
    SEND("NAF LBSSTARTPF 1 0 60000 10");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    
    /* SUPL TRIGGERED START 10 reports, interval = 60 sec*/
    SEND("SUPL LBSDATARECV 1 003E020000C0004EAAEF3374004488CC02F8B1E0055817084011A18BFA4020EA4801095A02108200080200000026DC6D34378E974D9000000090000003E020000C0004EAAEF3374004488CC02F8B1E0055817084011A18BFA4020EA4801095A02108200080200000026DC6D34378E974D9000000090000 62 10");
    /* SUPL TRIGGERED RESPONSE - MSA */
    SEND("SUPL LBSDATASEND 1 001D020000C0004EAAEF3374004488CC02F8B1E0055817084081000200 29");

    /* SUPL POS INIT */
    SEND("SUPL LBSDATARECV 1 0031010000C0004C000404850D961FE400000004605400119A4E892DF000000009002008000001000DC74D7A986C3F2F28 49 10");
    /* UT2 timer expires after 11 seconds but session continues with next reports*/
    TEST_SUPL_WAIT_TIMEOUT(60);
    SEND("SUPL LBSDATARECV 1 0031010000C0004C000404850D961FE400000004605400119A4E892DF000000009002008000001000DC74D7A986C3F2F28 49 10");
    TEST_SUPL_WAIT_TIMEOUT(60);
    SEND("SUPL LBSDATARECV 1 0031010000C0004C000404850D961FE400000004605400119A4E892DF000000009002008000001000DC74D7A986C3F2F28 49 10");
    TEST_SUPL_WAIT_TIMEOUT(60);
    SEND("SUPL LBSDATARECV 1 0031010000C0004C000404850D961FE400000004605400119A4E892DF000000009002008000001000DC74D7A986C3F2F28 49 10");
    TEST_SUPL_WAIT_TIMEOUT(60);
    SEND("SUPL LBSDATARECV 1 0031010000C0004C000404850D961FE400000004605400119A4E892DF000000009002008000001000DC74D7A986C3F2F28 49 10");
    TEST_SUPL_WAIT_TIMEOUT(60);
    SEND("SUPL LBSDATARECV 1 0031010000C0004C000404850D961FE400000004605400119A4E892DF000000009002008000001000DC74D7A986C3F2F28 49 10");
    TEST_SUPL_WAIT_TIMEOUT(60);
    SEND("SUPL LBSDATARECV 1 0031010000C0004C000404850D961FE400000004605400119A4E892DF000000009002008000001000DC74D7A986C3F2F28 49 10");
    TEST_SUPL_WAIT_TIMEOUT(60);
    SEND("SUPL LBSDATARECV 1 0031010000C0004C000404850D961FE400000004605400119A4E892DF000000009002008000001000DC74D7A986C3F2F28 49 10");
    TEST_SUPL_WAIT_TIMEOUT(60);
    SEND("SUPL LBSDATARECV 1 0031010000C0004C000404850D961FE400000004605400119A4E892DF000000009002008000001000DC74D7A986C3F2F28 49 10");
    TEST_SUPL_WAIT_TIMEOUT(60);
    SEND("SUPL LBSDATARECV 1 0031010000C0004C000404850D961FE400000004605400119A4E892DF000000009002008000001000DC74D7A986C3F2F28 49 10");
    TEST_SUPL_WAIT_TIMEOUT(60);
    SEND("SUPL LBSDATARECV 1 0031010000C0004C000404850D961FE400000004605400119A4E892DF000000009002008000001000DC74D7A986C3F2F28 49 10");

    /* SUPL END sent by SET */
    SEND("SUPL LBSDATARECV 1 0033010000C000CC000408850D961FE40000000460540011AA01AC19B072C593262CD8B46169CCB01978D86100000000002D00 51 10");
    RECV("SUPL LBSDISREQ 1");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


/***********************************
Emergency
************************************/
    
static int LBSUPLCON0033a(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    /* SUPL INIT  - Emergency session, Notification Only, ESLP - www.emer-spirent-lcs.com */
    SEND("SUPL LBSSmsPush 1  002B020000402F8B1E0055817080601804040500022A1045FCF3C06ACAADC0E35BB7AB3E40C68E01A34C80 43");


    SEND("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    SEND("SUPL LBSCONREQ 1 www.emer-spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* SUPL POS INIT */
    SEND("SUPL LBSDATARECV 1 0030010000C0004C000404850D961FE400000004605400119A4E093DF00000000800200800000114564810562527D260 48 10");
    /* SUPL END */
    SEND("SUPL LBSDATASEND 1 003301FFFFC0004C000404850D961FE40000000460540011AA01AC19B072C593262CD9307369CCB01938D86140000000003600 51");
    SEND("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    SEND("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


static int LBSUPLCON0033b(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    /* SUPL INIT  - Emergency session, Notification Only, ESLP not provided */
    SEND("SUPL LBSSmsPush 1 0018020000402F8B1E005581708060180404050002042000 24");


    SEND("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    SEND("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* SUPL POS INIT */
    SEND("SUPL LBSDATARECV 1 0030010000C0004C000404850D961FE400000004605400119A4E093DF00000000800200800000114564810562527D260 48 10");
    /* SUPL END */
    SEND("SUPL LBSDATASEND 1 003301FFFFC0004C000404850D961FE40000000460540011AA01AC19B072C593262CD9307369CCB01938D86140000000003600 51");
    SEND("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    SEND("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


static int LBSUPLCON0034a(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    /* SUPL INIT  - Session ID :  00 BE 2C 00*/
    SEND("SUPL LBSSmsPush 1 0011020000402F8B000055817080201008 17");


    SEND("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    SEND("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* SUPL INIT  - Emergency session, Notification Only, ESLP not provided, Session ID 00 BE 2C 78 */
    SEND("SUPL LBSSmsPush 1 0018020000402F8B1E005581708060180404050002042000 24");
    SEND("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    SEND("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* SUPL POS INIT */
    SEND("SUPL LBSDATARECV 1 0030010000C0004C000404850D961FE400000004605400119A4E093DF00000000800200800000114564810562527D260 48 10");
    /* SUPL END */
    SEND("SUPL LBSDATASEND 1 003301FFFFC0004C000404850D961FE40000000460540011AA01AC19B072C593262CD9307369CCB01938D86140000000003600 51");
    SEND("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    SEND("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


static int LBSUPLCON0034b(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");

    /* SUPL INIT  - Emergency session, Notification Only, ESLP not provided, Session ID 00 BE 2C 78 */
    SEND("SUPL LBSSmsPush 1 0018020000402F8B1E005581708060180404050002042000 24");
    SEND("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    SEND("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* SUPL INIT  - Session ID :  00 BE 2C 00*/
    SEND("SUPL LBSSmsPush 1 0011020000402F8B000055817080201008 17");

    /* SUPL POS INIT */
    SEND("SUPL LBSDATARECV 1 0030010000C0004C000404850D961FE400000004605400119A4E093DF00000000800200800000114564810562527D260 48 10");

    /* SUPL END */
    SEND("SUPL LBSDATASEND 1 003301FFFFC0004C000404850D961FE40000000460540011AA01AC19B072C593262CD9307369CCB01938D86140000000003600 51");
    SEND("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    SEND("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

/* SUPL 2.0 Area Id  and Geographic Target Area ,all Periodic */

/***********************************************
Network Initiated Area Id inside Based periodic
 ***********************************************/

static int LBSUPLCON0051a(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 0014020000400000004605400118401008108100 20");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 003b020000c0004c000404850d961fe40000000460540011c010210b8240216b6800095a3a04a05021080200080200000027e8b69505a28139ad00 59 10");

    /* Send SUPL TRIGGERED RESPONSE - MSB , start time = 60 sec, interval = 60 sec*/
    SEND("SUPL LBSDATASEND 1 002F020000C0004C000404850D961FE40000000460540011C08A2002BA60003B0140003C0000960000210000001000 47");
    TEST_SUPL_WAIT_TIMEOUT(60);
    /* Receive SUPL_POSINIT */
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e090085ada0002568ec12814084202df00000000800200800000090 53 10");
    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

/************************************
Network Initiated Area Id outside Based periodic
 ************************************/

static int LBSUPLCON0051b(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 0014020000400000004605400118401008108100 20");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 003b020000c0004c000404850d961fe40000000460540011c010210b8240216b6800095a3a04a05021080200080200000027e8b69505a28139ad00 59 10");

    /* Send SUPL TRIGGERED RESPONSE - MSB , start time = 30 sec, interval = 60 sec*/
    SEND("SUPL LBSDATASEND 1 003C020000C0004C000404850D961FE40000000460540011C090A002BAA0003B0140001E00007808002F00401000100000020001E008020002000200 60");
    TEST_SUPL_WAIT_TIMEOUT(30);
    /* Receive SUPL_POSINIT */
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e090085ada0002568ec12814084202df00000000800200800000090 53 10");
    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

/************************************
Network Initiated Area Id inside Assisted periodic
 ************************************/

static int LBSUPLCON0051c(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 0014020000400000004605400118400008108100 20");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 003b020000c0004c000404850d961fe40000000460540011c010210b8240216b6800095a3a04a05021080200080200000027e8b69505a28139ad00 59 10");

    /* Send SUPL TRIGGERED RESPONSE - MSB , start time = 60 sec, interval = 60 sec*/
    SEND("SUPL LBSDATASEND 1 004C020000C0004C000404850D961FE40000000460540011C098A000BA60003B0140003C00007808002F00401000100000010085E00802000200000020F00401000100012700010002000200 76");
    TEST_SUPL_WAIT_TIMEOUT(60);
    /* Receive SUPL_POSINIT */
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e090085ada0002568ec12814084202df00000000800200800000090 53 10");
    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

/************************************
Network Initiated Area Id outside Based periodic
 ************************************/

static int LBSUPLCON0051d(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 0014020000400000004605400118401008108100 20");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 003b020000c0004c000404850d961fe40000000460540011c010210b8240216b6800095a3a04a05021080200080200000027e8b69505a28139ad00 59 10");

    /* Send SUPL TRIGGERED RESPONSE - MSB , start time = 60 sec, interval = 60 sec*/
    SEND("SUPL LBSDATASEND 1 0042020000C0004C000404850D961FE40000000460540011C093A002BAA0003B0140003C00007800042F004040001000000107804010001000113800100020002000 66");
    TEST_SUPL_WAIT_TIMEOUT(60);
    /* Receive SUPL_POSINIT */
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e090085ada0002568ec12814084202df00000000800200800000090 53 10");
    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

/************************************
Network Initiated Area Id outside Assisted periodic
 ************************************/

static int LBSUPLCON0051e(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 0014020000400000004605400118400008108100 20");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 003b020000c0004c000404850d961fe40000000460540011c010210b8240216b6800095a3a04a05021080200080200000027e8b69505a28139ad00 59 10");

    /* Send SUPL TRIGGERED RESPONSE - MSB , start time = 60 sec, interval = 60 sec*/
    SEND("SUPL LBSDATASEND 1 0042020000C0004C000404850D961FE40000000460540011C093A000BAA0003B0140003C00007800042FF9FE703E9000012D078134D00A6800B93800300070008000 66");
    TEST_SUPL_WAIT_TIMEOUT(60);
    /* Receive SUPL_POSINIT */
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e090085ada0002568ec12814084202df00000000800200800000090 53 10");
    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


/************************************
Network Initiated Area Id entering Assisted periodic
 ************************************/

static int LBSUPLCON0051f(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 0014020000400000004605400118400008108100 20");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 003b020000c0004c000404850d961fe40000000460540011c010210b8240216b6800095a3a04a05021080200080200000027e8b69505a28139ad00 59 10");

    /* Send SUPL TRIGGERED RESPONSE - MSB , start time = 60 sec, interval = 60 sec*/
    SEND("SUPL LBSDATASEND 1 0042020000C0004C000404850D961FE40000000460540011C093A000BA20003B0140003C00007800042F004010001000000107802008000800093800080010001000 66");
    TEST_SUPL_WAIT_TIMEOUT(60);
    /* Receive SUPL_POSINIT */
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e090085ada0002568ec12814084202df00000000800200800000090 53 10");
    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


/***********************************************
Network Initiated Area Id inside Based periodic
 ***********************************************/

static int LBSUPLCON0051g(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 0014020000400000004605400118401008108100 20");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 003b020000c0004c000404850d961fe40000000460540011c010210b8240216b6800095a3a04a05021080200080200000027e8b69505a28139ad00 59 10");

    /* Send SUPL TRIGGERED RESPONSE - MSB , start time = 60 sec, interval = 60 sec*/
    SEND("SUPL LBSDATASEND 1 003E020000C0004C000404850D961FE40000000460540011C091A002BA60003B0140003C0000960004201780200800080000010BC0100400040000004000 62");
    TEST_SUPL_WAIT_TIMEOUT(60);
    /* Receive SUPL_POSINIT */
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e090085ada0002568ec12814084202df00000000800200800000090 53 10");
    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

/***********************************************
Network Initiated Area Id, inside event
 ***********************************************/
static int LBSUPLCON0051h(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 0 001010123456789");// Cell id is GSM
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 0014020000400000004605400118401008108100 20");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 003b020000c0004c000404850d961fe40000000460540011c010210b8240216b6800095a3a04a05021080200080200000027e8b69505a28139ad00 59 10");
    /* Send SUPL TRIGGERED RESPONSE - MSB , start time = 30 sec, interval = 60 sec*/
    SEND("SUPL LBSDATASEND 1 002D020000C0004C000404850D961FE40000000460540011C0892002BA60003B0140001E000078000001000100 45");
    TEST_SUPL_WAIT_TIMEOUT(30);
    /* Receive SUPL_POSINIT */
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e090085ada0002568ec12814084202df00000000800200800000090 53 10");
    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

/************************************
Network Initiated Area Id, OUTSIDE event
 ************************************/

static int LBSUPLCON0051i(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 0 001010123456789");// Cell id is GSM
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 0014020000400000004605400118401008108100 20");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 003b020000c0004c000404850d961fe40000000460540011c010210b8240216b6800095a3a04a05021080200080200000027e8b69505a28139ad00 59 10");
    /* Send SUPL TRIGGERED RESPONSE - MSB , start time = 30 sec, interval = 60 sec*/
    SEND("SUPL LBSDATASEND 1 002F020000C0004C000404850D961FE40000000460540011C08A2002BAA0003B0140001E0000780000030002000100 47");
    TEST_SUPL_WAIT_TIMEOUT(30);
    /* Receive SUPL_POSINIT */
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e090085ada0002568ec12814084202df00000000800200800000090 53 10");
    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

/************************************
Network Initiated Area Id, OUTSIDE event
 ************************************/
static int LBSUPLCON0051j(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 0 001010123456789");// Cell id is GSM
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 0014020000400000004605400118401008108100 20");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 003b020000c0004c000404850d961fe40000000460540011c010210b8240216b6800095a3a04a05021080200080200000027e8b69505a28139ad00 59 10");
    /* Send SUPL TRIGGERED RESPONSE - MSB , start time = 30 sec, interval = 60 sec*/
    SEND("SUPL LBSDATASEND 1 0031020000C0004C000404850D961FE40000000460540011C08B2002BAA0003B0140001E00007800000700400040008000 49");
    TEST_SUPL_WAIT_TIMEOUT(30);
    /* Receive SUPL_POSINIT */
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e090085ada0002568ec12814084202df00000000800200800000090 53 10");
    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

/************************************************************************
Network Initiated Area Id, UC(mandatory parameter) not present,Only LAC present
 ************************************************************************/
static int LBSUPLCON0051k(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 0 001010123456789");// Cell id is GSM
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 0014020000400000004605400118401008108100 20");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 003b020000c0004c000404850d961fe40000000460540011c010210b8240216b6800095a3a04a05021080200080200000027e8b69505a28139ad00 59 10");
    /* Send SUPL TRIGGERED RESPONSE - MSB , start time = 30 sec, interval = 60 sec*/
    SEND("SUPL LBSDATASEND 1 002D020000C0004C000404850D961FE40000000460540011C0892002BAA0003B0140001E000078000002000100 45");
    /* Receive SUPL END */
    RECV("SUPL LBSDATARECV 1 001a020000c0004c000404850d961fe40000000460540011a908 26 10");//SUPL_END as CI not present
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");

    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

/************************************************************************
Network Initiated Area Id, UC(mandatory parameter) not present,Only MNC,MNC present
 ************************************************************************/

static int LBSUPLCON0051l(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 0 001010123456789");// Cell id is GSM
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 0014020000400000004605400118401008108100 20");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 003b020000c0004c000404850d961fe40000000460540011c010210b8240216b6800095a3a04a05021080200080200000027e8b69505a28139ad00 59 10");
    /* Send SUPL TRIGGERED RESPONSE - MSB , start time = 30 sec, interval = 60 sec*/
    SEND("SUPL LBSDATASEND 1 002E020000C0004C000404850D961FE40000000460540011C089A002BAA0003B0140001E00007800000C00401000 46");
    /* Receive SUPL END */
    RECV("SUPL LBSDATARECV 1 001A010000C0004C000404850D961FE40000000460540011A800 26");//SUPL_END as CI not present
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");

    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

/********************************************************************
Network Initiated Area Id, UC(mandatory parameter) not present,Only MNC present
 ********************************************************************/

static int LBSUPLCON0051m(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 0 001010123456789");// Cell id is GSM
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 0014020000400000004605400118401008108100 20");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    
    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 003b020000c0004c000404850d961fe40000000460540011c010210b8240216b6800095a3a04a05021080200080200000027e8b69505a28139ad00 59 10");

    /* Send SUPL TRIGGERED RESPONSE - MSB , start time = 30 sec, interval = 60 sec*/
    SEND("SUPL LBSDATASEND 1 002D020000C0004C000404850D961FE40000000460540011C0892002BAA0003B0140001E000078000004004000 45");
    
    /* Receive SUPL END */
    RECV("SUPL LBSDATARECV 1 001A010000C0004C000404850D961FE40000000460540011A800 26 10");//SUPL_END as CI not present
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");

    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

/********************************************************************
Network Initiated Area Id, UC(mandatory parameter) not present,Only LAC present
 ********************************************************************/

static int LBSUPLCON0051n(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;

    Supl_pos_flag = 1;

    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 0014020000400000004605400118401008108100 20");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    
    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 003b020000c0004c000404850d961fe40000000460540011c010210b8240216b6800095a3a04a05021080200080200000027e8b69505a28139ad00 59 10");

    /* Send SUPL TRIGGERED RESPONSE - MSB , start time = 30 sec, interval = 60 sec*/
    SEND("SUPL LBSDATASEND 1 002D020000C0004C000404850D961FE40000000460540011C0892002BAA0003B0140001E000078000022000100 45");
    
    /* Receive SUPL END */
    RECV("SUPL LBSDATARECV 1 001A010000C0004C000404850D961FE40000000460540011A800 26 10");//SUPL_END as Uc not present
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");

    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

/********************************************************************
Network Initiated Area Id, UC(mandatory parameter) not present,Only MCC present
 ********************************************************************/

static int LBSUPLCON0051o(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 0014020000400000004605400118401008108100 20");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 003b020000c0004c000404850d961fe40000000460540011c010210b8240216b6800095a3a04a05021080200080200000027e8b69505a28139ad00 59 10");

    /* Send SUPL TRIGGERED RESPONSE - MSB , start time = 30 sec, interval = 60 sec*/
    SEND("SUPL LBSDATASEND 1 002D020000C0004C000404850D961FE40000000460540011C0892002BAA0003B0140001E000078000028004000 45");
    
    /* Receive SUPL END */
    RECV("SUPL LBSDATARECV 1 001A010000C0004C000404850D961FE40000000460540011A800 26 10");//SUPL_END as Uc not present
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");

    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

/********************************************************************
Network Initiated Area Id, UC(mandatory parameter) not present,Only MNC present
 ********************************************************************/

static int LBSUPLCON0051p(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 0014020000400000004605400118401008108100 20");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    
    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 003b020000c0004c000404850d961fe40000000460540011c010210b8240216b6800095a3a04a05021080200080200000027e8b69505a28139ad00 59 10");

    /* Send SUPL TRIGGERED RESPONSE - MSB , start time = 30 sec, interval = 60 sec*/
    SEND("SUPL LBSDATASEND 1 002D020000C0004C000404850D961FE40000000460540011C0892002BAA0003B0140001E000078000024004000 45");
    
    /* Receive SUPL END */
    RECV("SUPL LBSDATARECV 1 001A010000C0004C000404850D961FE40000000460540011A800 26 10");//SUPL_END as Uc not present
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");

    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

/***********************************************
Network Initiated Area Id, Cell id not supported.
 ***********************************************/

static int LBSUPLCON0051q(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 0 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 0014020000400000004605400118401008108100 20");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    
    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 003b020000c0004c000404850d961fe40000000460540011c010210b8240216b6800095a3a04a05021080200080200000027e8b69505a28139ad00 59 10");

    /* Send SUPL TRIGGERED RESPONSE - MSB , start time = 30 sec, interval = 60 sec*/
    SEND("SUPL LBSDATASEND 1 0033020000C0004C000404850D961FE40000000460540011C08C2002BAA0003B0140001E00007800002F004010001000000200 51");
    
    TEST_SUPL_WAIT_TIMEOUT(120);
    /* Receive SUPL END */
    RECV("SUPL LBSDATARECV 1 001A010000C0004C000404850D961FE40000000460540011A800 26 10");//SUPL_END as same cell id type not present
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");

    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


/************************************
Set Initiated Area Id inside Based periodic
 ************************************/

static int LBSUPLCON0131a(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF  LBSGEOFENCE 0 6 30 120 60000 100 150 8 1 0 500 3324374 0 6512822 2 2 ");
    SEND("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    SEND("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    /* SUPL TRIGGERED START */
    SEND("SUPL LBSDATARECV 1 004402000080004c000404850d961fe600c80c5c12010b5b40004ad1d02502810840100040100000012af10000ec630000780003c00332b9d6e360b6001f300001000008 68 10");
    /* SUPL TRIGGERED RESPONSE - MSB, no reports, start time = 30 sec, interval = 60 sec */
    SEND("SUPL LBSDATASEND 1 002B020000C0004C000404850D961FE40000000460540011C08820029A60001E00004B0000108000000800 43");

    TEST_SUPL_WAIT_TIMEOUT(30);
    /* Receive SUPL_POSINIT */
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e090085ada0002568ec12814084202df00000000800200800000090 53 10");
    /* SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

/************************************
Set Initiated Area Id outside Based periodic
 ************************************/

static int LBSUPLCON0131b(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSGEOFENCE 0 6 30 120 60000 100 150 8 2 0 500  1209470 0 3621150 2 2 ");
    SEND("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    SEND("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    /* SUPL TRIGGERED START */
    SEND("SUPL LBSDATARECV 1 004402000080004c000404850d961fe600c80c5c12010b5b40004ad1d02502810840100040100000012af10000ec630000780003c00332b9d6e360b6001f300001000008 68 10");
    /* SUPL TRIGGERED RESPONSE - MSB, no reports, start time = 30 sec, interval = 60 sec */
    SEND("SUPL LBSDATASEND 1 0038020000C0004C000404850D961FE40000000460540011C08EA0029AA0001E00003C04001780200800080000010000F004010001000100 56");
    TEST_SUPL_WAIT_TIMEOUT(30);
    /* Receive SUPL_POSINIT */
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e090085ada0002568ec12814084202df00000000800200800000090 53 10");
    /* SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

/************************************
Set Initiated Area Id inside Assisted periodic
 ************************************/

static int LBSUPLCON0131c(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSGEOFENCE 0 6 30 120 60000 100 150 8 1 0 500 3324374 0 6512822 2 2 ");
    SEND("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    SEND("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    /* SUPL TRIGGERED START */
    SEND("SUPL LBSDATARECV 1 004402000080004c000404850d961fe600c80c5c12010b5b40004ad1d02502810840100040100000012af10000ec630000780003c00332b9d6e360b6001f300001000008 68 10");
    /* SUPL TRIGGERED RESPONSE - MSB, no reports, start time = 30 sec, interval = 60 sec */
    SEND("SUPL LBSDATASEND 1 0048020000C0004C000404850D961FE40000000460540011C096A0009A60001E00003C04001780200800080000008042F00401000100000010780200800080009380008001000100 72");
    TEST_SUPL_WAIT_TIMEOUT(30);
    /* Receive SUPL_POSINIT */
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e090085ada0002568ec12814084202df00000000800200800000090 53 10");
    /* SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

/************************************
Set Initiated Area Id outside Based periodic
 ************************************/

static int LBSUPLCON0131d(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSGEOFENCE 0 6 30 120 60000 100 150 8 2 0 500  1209470 0 3621150 2 2 ");
    SEND("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    SEND("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    /* SUPL TRIGGERED START */
    SEND("SUPL LBSDATARECV 1 004402000080004c000404850d961fe600c80c5c12010b5b40004ad1d02502810840100040100000012af10000ec630000780003c00332b9d6e360b6001f300001000008 68 10");
    /* SUPL TRIGGERED RESPONSE - MSB, no reports, start time = 30 sec, interval = 60 sec */
    SEND("SUPL LBSDATASEND 1 003E020000C0004C000404850D961FE40000000460540011C091A0029AA0001E00003C000217814008000800000083C02008000800089C00080010001000 62");
    TEST_SUPL_WAIT_TIMEOUT(30);
    /* Receive SUPL_POSINIT */
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e090085ada0002568ec12814084202df00000000800200800000090 53 10");
    /* SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

/************************************
Set Initiated Area Id outside Assisted periodic
 ************************************/

static int LBSUPLCON0131e(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSGEOFENCE 0 6 30 120 60000 100 150 8 2 0 500  1209470 0 3621150 2 2 ");
    SEND("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    SEND("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    /* SUPL TRIGGERED START */
    SEND("SUPL LBSDATARECV 1 004402000080004c000404850d961fe600c80c5c12010b5b40004ad1d02502810840100040100000012af10000ec630000780003c00332b9d6e360b6001f300001000008 68 10");
    /* SUPL TRIGGERED RESPONSE - MSB, no reports, start time = 30 sec, interval = 60 sec */
    SEND("SUPL LBSDATASEND 1 003E020000C0004C000404850D961FE40000000460540011C091A0009AA0001E00003C000217FCFF381F4800000E83C09A680534005C9C00180038004000 62");
    TEST_SUPL_WAIT_TIMEOUT(30);
    /* Receive SUPL_POSINIT */
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e090085ada0002568ec12814084202df00000000800200800000090 53 10");
    /* SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

/************************************
Set Initiated Area Id entering Assisted periodic
 ************************************/

static int LBSUPLCON0131f(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSGEOFENCE 0 6 30 240 60000 100 150 8 0 0 500 3324374 0 6512822 2 2 ");
    SEND("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    SEND("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    /* SUPL TRIGGERED START */
    SEND("SUPL LBSDATARECV 1 004402000080004c000404850d961fe600c80c5c12010b5b40004ad1d02502810840100040100000012af10000ec630000780003c00332b9d6e360b6001f300001000008 68 10");
    /* SUPL TRIGGERED RESPONSE - MSB, no reports, start time = 30 sec, interval = 60 sec */
    SEND("SUPL LBSDATASEND 1 003E020000C0004C000404850D961FE40000000460540011C091A0009A20001E00003C000217802008000800000083C01004000400049C00040008000800 62");
    TEST_SUPL_WAIT_TIMEOUT(30);
    /* Receive SUPL_POSINIT */
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e090085ada0002568ec12814084202df00000000800200800000090 53 10");
    /* SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

/********************************************
Set Initiated Area Id inside Based periodic(no params)
 ********************************************/

static int LBSUPLCON0131g(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSGEOFENCE 0 6 30 120 60000 100 150 8 1 0 500 3324374 0 6512822 2 2 ");
    SEND("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    SEND("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    /* SUPL TRIGGERED START */
    SEND("SUPL LBSDATARECV 1 004402000080004c000404850d961fe600c80c5c12010b5b40004ad1d02502810840100040100000012af10000ec630000780003c00332b9d6e360b6001f300001000008 68 10");
    /* Send SUPL TRIGGERED RESPONSE - MSB , no trigger params*/
    SEND("SUPL LBSDATASEND 1 001D020000C0004C000404850D961FE40000000460540011C081000200 29");
    TEST_SUPL_WAIT_TIMEOUT(30);
    /* Receive SUPL_POSINIT */
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e090085ada0002568ec12814084202df00000000800200800000090 53 10");
    /* SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");

    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

/****************************************************************************
Set Initiated Area Id inside Based periodic(start time greater than stop time)
 ****************************************************************************/

static int LBSUPLCON0131h(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSGEOFENCE 0 6 60 30 60000 100 150 8 1 0 500 3324374 0 6512822 2 2 ");

    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

/**************************************************
Network Initiated Geographic target area(circle) outside Based periodic
 **************************************************/

static int LBSUPLCON0050a(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 0014020000400000004605400118401008108100 20");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 003b020000c0004c000404850d961fe40000000460540011c010210b8240216b6800095a3a04a05021080200080200000027e8b69505a28139ad00 59 10");

    /* Send SUPL TRIGGERED RESPONSE - MSB , start time = 30 sec, interval = 60 sec*/
    SEND("SUPL LBSDATASEND 1 0032020000C0004C000404850D961FE40000000460540011C08BA000BCA0003B0140001E0000F0000CAE75B8D916800F9C00 50");
    TEST_SUPL_WAIT_TIMEOUT(30);
    /* Receive SUPL_POSINIT */
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e090085ada0002568ec12814084202df00000000800200800000090 53 10");
    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

/****************************************************
Network Initiated Geographic target area(circle) inside Based periodic
 ****************************************************/

static int LBSUPLCON0050b(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 0014020000400000004605400118401008108100 20");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 003b020000c0004c000404850d961fe40000000460540011c010210b8240216b6800095a3a04a05021080200080200000027e8b69505a28139ad00 59 10");

    /* Send SUPL TRIGGERED RESPONSE - MSB , start time = 30 sec, interval = 60 sec*/
    SEND("SUPL LBSDATASEND 1 0032020000C0004C000404850D961FE40000000460540011C08BA002BC60003B0140001E0000F000200005A00008400F9C00 50");
    TEST_SUPL_WAIT_TIMEOUT(30);
    /* Receive SUPL_POSINIT */
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e090085ada0002568ec12814084202df00000000800200800000090 53 10");
    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

/*********************************************************
Set Initiated Area Geographic target(circle) Area inside Based periodic
 *********************************************************/

static int LBSUPLCON0130a(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF  LBSGEOFENCE 0 6 30 240 60000 100 150 8 1 0 500 3324374 0 6512822 2 2 ");
    SEND("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    SEND("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    /* SUPL TRIGGERED START */
    SEND("SUPL LBSDATARECV 1 004402000080004c000404850d961fe600c80c5c12010b5b40004ad1d02502810840100040100000012af10000ec630000780003c00332b9d6e360b6001f300001000008 68 10");
    /* SUPL TRIGGERED RESPONSE - MSB, no reports, start time = 30 sec, interval = 60 sec */
    SEND("SUPL LBSDATASEND 1 001D020000C0004C000404850D961FE40000000460540011C081000200 29");

    TEST_SUPL_WAIT_TIMEOUT(30);
    /* Receive SUPL_POSINIT */
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e090085ada0002568ec12814084202df00000000800200800000090 53 10");
    /* SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

/**************************************************
Set Initiated geographic target outside(circle) Based periodic
 **************************************************/

static int LBSUPLCON0130b(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSGEOFENCE 0 6 30 240 60000 100 150 8 2 0 500  1209470 0 3621150 2 2 ");
    SEND("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    SEND("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    /* SUPL TRIGGERED START */
    SEND("SUPL LBSDATARECV 1 004402000080004c000404850d961fe600c80c5c12010b5b40004ad1d02502810840100040100000012af10000ec630000780003c00332b9d6e360b6001f300001000008 68 10");
    /* SUPL TRIGGERED RESPONSE - MSB, no reports, start time = 30 sec, interval = 60 sec */
    SEND("SUPL LBSDATASEND 1 001D020000C0004C000404850D961FE40000000460540011C081000200 29");
    TEST_SUPL_WAIT_TIMEOUT(30);

    /* Receive SUPL_POSINIT */
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e090085ada0002568ec12814084202df00000000800200800000090 53 10");
    /* SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

static int LBSSUPLINT001(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    /*Test Description:
          SUPL session is disconnected after SUPL_START is received.
          GPS Start -> TCPIP Connect Req -> GPS Stop -> TCPIP Connect Cnf -> Check that TCPIP Disconnect is received
    */

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("NAF LBSSTOP 1");
    ALOGD("LBSSUPLINT001:  Check LBSD is STOPPED or NOT");
    sleep(1);
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");

    SEND("NAF LBSSTOP 1");


    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}


static int LBSSUPLINT002(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    /*Test Description:
          GPS Start 1 -> TCPIP Connect Req 1 -> GPS Stop 1 -> GPS Start 2 -> TCPIP Connect Req 2 ->
          TCPIP Connect Cnf 1 -> Check that TCPIP Disconnect 1 is received

    */

    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 1");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("NAF LBSSTOP 1");
    ALOGD("LBSSUPLINT002:  Check LBSD is STOPPED or NOT");
    sleep(2);
    SEND("NAF LBSSTARTSF 1 0 30 60 100 5");
    sleep(1);
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");

    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}

static int LBSSUPLATT300(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
      /*Test Description:
        Simulate the ATT 300 iteration test. This test basically can be used to check possible memory
        related issues inside the SUPL module. Also the successsive iteration using a different set
        session ID. This test case cannot be used to check the position fix.
       */

   int count, i, j;
   char buf_id[6] = "C0004";
   char * q = buf_id;
   int num;


   TCBEGIN(handle, __func__) ;


    for (i=4, j=0; i >=0; i--,j++)
    {
      if (q[i] >=48 && q[i] <= 57 )
         num = q[i] - 48;
      else if (q[i] >= 65 && q[i] <= 70 )
         num = (q[i] - 65) + 10;
      Set_Session_Id += num * powi(16,j) ;
    }


   Supl_pos_flag =1;

   for (count=0; count< 299; count++)
   {
      sprintf (buf_SessionId, "%X", Set_Session_Id);
      SEND("SUPL LBSSUPLINIT");
      SEND("NAF LBSCFGMOLR 1");
      SEND("NAF LBSCFGSUPL 1");
      SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
      SEND("SUPL LBSSmsPush 1 00700100004000000046054001182013D2DFFFE530070006900720065006E007400200043006F006D006D0075006E00690063006100740069006F006E0073000B7FFF94C01C001A401C8019401B801D00080010C01BC01B401B401D401B801A4018C018401D001A401BC01B801CC0000 112");  //SUPL INIT

      RECV("SUPL LBSPDPREQ");
      SEND("SUPL LBSPDPCNF 1 bsnl.com");
      RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
      SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
      sleep(3);
      RECV("SUPL LBSDATARECV 1 0031010000C0004C000404850D961FE400000004605400119A4E092DF000000009002008000000800E4ACB897B6AFB4588 49 10");  //SUPL POS init
      sleep(1);
      SEND("SUPL LBSDATASEND 1 02EF010000C0004C000404850D961FE40000000460540011A085A642224D9DD0F49054676C81A4FA4D49F4A00321E1E00142D4041000002AAAAAAAAAAAAAAAAAAAAA804F1A80801E877AD21B5ED4B36284844E1784048680025C32843618013C69F7FE2574CB54A7FE2A62229049321975FDB007FA7606B00D080000155555555555555555555540278D404000381000E0A15D2742CF3DE4E520027BA0012AB9421AAA009E34FC009EC825953C00053A548824B89367ED4803FD50041E270400000AAAAAAAAAAAAAAAAAAAAAA013C6A0200822581D8DC0ACA1AAAD5B5F8B98042EC0008974A10D55004F1A7DFF921AC5535DFE469FB2DF2E96EF1A567401FE9B822F2402000005555555555555555555555009E350101C160464C65156A13EA13FCB458A036B0000441BD086C100278D3F00C710A57AC2FFD14FFF5C254E4FFF815400FF4A8117EA21000002AAAAAAAAAAAAAAAAAAAAA804F1A808000800001DA4ACDD98DC03D45DF5C0485000210BE843488013C69F7FF4B08C9D948002A728F6A8AA94EE05A5007FA3407CAD4080000155555555555555555555540278D404000400000EDD55E443A72899AEF9811DBA0010667421A16009E34FC02F57693016BFD8D3ECBBACD6660E9E4F803FD3A041ECB0400000AAAAAAAAAAAAAAAAAAAAAA013C6A0200020980076D180002A2FDA94788907EA4000839BA10AAD004F1A7E0192C22F553600129C71C6EEAFC0D7099C01FEA202000702000005555555555555555555555009E35010041A480EB7C456E9E78265EFB8DE022308004A7A50879080278D3F0013B05E711AFF334EA719A72F4A68B09E00FF5601081BC1000002AAAAAAAAAAAAAAAAAAAAA804F1A80802087C076159ECBFAD7BE9FEE133014E880025E36843590013C69F7F9C57EDD7118007A67DBA2B93A0A356D0007FA72066E61609FDFE9E11FDEA000002000000022D9E01960A00F4904A00A495D930A1BA68FE1050E706A9182882D7D32A14459EFD758A29371C2A05165328C0C28E6BFE940147AF98369EA05040200008000 751");
      sleep(3);
      RECV("SUPL LBSDATARECV 1 0031010000C0004C000404850D961FE40000000460540011A0802A4422000072B2B16C81A4AE8549F4A803484838887A18 49 10");  //SUPL POS
      sleep(1);
      SEND("SUPL LBSDATASEND 1 0033010000C0004C000404850D961FE40000000460540011AA01AC18B070C5AB16EC19346369CD25740A4FA54487118600691E 51");  //SUPL END
      sleep(3);
      RECV("SUPL LBSDISREQ 1 ");
      SEND("SUPL LBSDISCNF 1");
      sleep(3);
      RECV("SUPL LBSCLOSEPDPREQ");
      sleep(1);
      SEND("SUPL LBSCLOSEPDPCNF 1");
      SEND("NAF LBSSTOP 1");
      sleep (3);

      Set_Session_Id = Set_Session_Id + 4;

   }
   Supl_pos_flag =0;
   TCEND(handle, __func__) ;

   return TSTENG_RESULT_OK;

}

static int LBSUPL_WLAN_test1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
/*Wlan Info is given after the Mob Info request cmd Valid values in all the fields*/
/*MS_Addr =0, MS_Addr =0, TransmitPower=0,AntennaGain =0,SNR = 0, DeviceType = 802.11a device, SignalStrength = 0, Channel = 0, RTDValue = 0, RTDUnits =0(In micro seconds),
   Accuracy =0, SETTransmitPower =0, SETAntennaGain  =0 ,SETSNR =0, SETSignalStrength =0 */
    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSUPLWLANInf 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSLOCFWD 1 1 30 60 100 5 1 portalnmms");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 004C02000080004C000404850D961FE47BA240216B6800095A3A04A05021080801737FF0000000000003FBFBF87F0020000000007F7F7F7F02E65780057010C200004F0DFCBA61D9BB6EDE60 76 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A020000C0004C000404850D961FE400000004605400119004 26");//SUPL_RESP                             
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e890085ada0002568e812814084202df00000000800200800000090 53 10");//SUPL_POSINT
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}


static int LBSUPL_WLAN_test2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    /*Wlan Info is given after the Mob Info request cmd only device type is changed to 82.11b deice*/
    /*MS_Addr =0, MS_Addr =0, TransmitPower=0,AntennaGain =0,SNR = 0, DeviceType = 802.11b device, SignalStrength = 0, Channel = 0, RTDValue = 0, RTDUnits =2(In10 nano seconds),
       Accuracy =0, SETTransmitPower =0, SETAntennaGain  =0 ,SETSNR =0, SETSignalStrength =0 */
    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSUPLWLANInf 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSLOCFWD 1 1 30 60 100 5 1 portalnmms");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003a02000080004c000404850d961fe47ba240216b6800095a3a04a05021080200080200000025ccaf000ae0218400009e1bf974c3b376ddbcc0 58 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A020000C0004C000404850D961FE400000004605400119004 26");//SUPL_RESP                             
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e890085ada0002568e812814084202df00000000800200800000090 53 10");//SUPL_POSINT
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}

static int LBSUPL_WLAN_test3(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    /*Wlan Info is given after the Mob Info request cmd Long Address more than 6 bytes(LS+MS)*/
    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSUPLWLANInf 17465664421 2458342749837782 3 4 5 0 7 8 9 0 11 12 13 14 15");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSLOCFWD 1 1 30 60 100 5 1 portalnmms");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003a02000080004c000404850d961fe47ba240216b6800095a3a04a05021080200080200000025ccaf000ae0218400009e1bf974c3b376ddbcc0 58 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A020000C0004C000404850D961FE400000004605400119004 26");//SUPL_RESP                             
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e890085ada0002568e812814084202df00000000800200800000090 53 10");//SUPL_POSINT
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}

static int LBSUPL_WLAN_test4(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    /*Wlan Info is given after the Mob Info request cmd Access Point TransmitPower Invalid -130*/
    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSUPLWLANInf 1 1 130 0 0 0 0 0 0 0 0 0 0 0 0");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSLOCFWD 1 1 30 60 100 5 1 portalnmms");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003a02000080004c000404850d961fe47ba240216b6800095a3a04a05021080200080200000025ccaf000ae0218400009e1bf974c3b376ddbcc0 58 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A020000C0004C000404850D961FE400000004605400119004 26");//SUPL_RESP                             
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e890085ada0002568e812814084202df00000000800200800000090 53 10");//SUPL_POSINT
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}
static int LBSUPL_WLAN_test5(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    /*Wlan Info is given after the Mob Info request cmd Access Point Antenna Gain Invalid 130*/
    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSUPLWLANInf 1 1 0 130 0 0 0 0 0 0 0 0 0 0 0");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSLOCFWD 1 1 30 60 100 5 1 portalnmms");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003a02000080004c000404850d961fe47ba240216b6800095a3a04a05021080200080200000025ccaf000ae0218400009e1bf974c3b376ddbcc0 58 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A020000C0004C000404850D961FE400000004605400119004 26");//SUPL_RESP                             
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e890085ada0002568e812814084202df00000000800200800000090 53 10");//SUPL_POSINT
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}

static int LBSUPL_WLAN_test6(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    /*Wlan Info is given after the Mob Info request cmd Access Point SNR Invalid 130*/
    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSUPLWLANInf 1 1 0 0 130 0 0 0 0 0 0 0 0 0 0");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSLOCFWD 1 1 30 60 100 5 1 portalnmms");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003a02000080004c000404850d961fe47ba240216b6800095a3a04a05021080200080200000025ccaf000ae0218400009e1bf974c3b376ddbcc0 58 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A020000C0004C000404850D961FE400000004605400119004 26");//SUPL_RESP                             
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e890085ada0002568e812814084202df00000000800200800000090 53 10");//SUPL_POSINT
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}

static int LBSUPL_WLAN_test7(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    /*Wlan Info is given after the Mob Info request cmd Access Point Signal Strength Invalid 130*/
    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSUPLWLANInf 1 1 0 0 0 1 130 0 0 0 0 0 0 0 0");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSLOCFWD 1 1 30 60 100 5 1 portalnmms");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003a02000080004c000404850d961fe47ba240216b6800095a3a04a05021080200080200000025ccaf000ae0218400009e1bf974c3b376ddbcc0 58 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A020000C0004C000404850D961FE400000004605400119004 26");//SUPL_RESP                             
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e890085ada0002568e812814084202df00000000800200800000090 53 10");//SUPL_POSINT
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}

static int LBSUPL_WLAN_test8(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    /*Wlan Info is given after the Mob Info request cmd SET Transmit power Invalid 130*/
    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSUPLWLANInf 1 1 0 0 0 1 0 0 0 0 0 130 0 0 0");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSLOCFWD 1 1 30 60 100 5 1 portalnmms");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003a02000080004c000404850d961fe47ba240216b6800095a3a04a05021080200080200000025ccaf000ae0218400009e1bf974c3b376ddbcc0 58 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A020000C0004C000404850D961FE400000004605400119004 26");//SUPL_RESP                             
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e890085ada0002568e812814084202df00000000800200800000090 53 10");//SUPL_POSINT
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}

static int LBSUPL_WLAN_test9(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    /*Wlan Info is given after the Mob Info request cmd SET Antenna Gain Invalid 130*/
    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSUPLWLANInf 1 1 0 0 0 1 0 0 0 0 0 0 130 0 0");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSLOCFWD 1 1 30 60 100 5 1 portalnmms");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003a02000080004c000404850d961fe47ba240216b6800095a3a04a05021080200080200000025ccaf000ae0218400009e1bf974c3b376ddbcc0 58 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A020000C0004C000404850D961FE400000004605400119004 26");//SUPL_RESP                             
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e890085ada0002568e812814084202df00000000800200800000090 53 10");//SUPL_POSINT
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}

static int LBSUPL_WLAN_test10(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    /*Wlan Info is given after the Mob Info request cmd SET SNR Invalid 130*/
    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSUPLWLANInf 1 1 0 0 0 1 0 0 0 0 0 0 0 130 0");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSLOCFWD 1 1 30 60 100 5 1 portalnmms");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003a02000080004c000404850d961fe47ba240216b6800095a3a04a05021080200080200000025ccaf000ae0218400009e1bf974c3b376ddbcc0 58 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A020000C0004C000404850D961FE400000004605400119004 26");//SUPL_RESP                             
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e890085ada0002568e812814084202df00000000800200800000090 53 10");//SUPL_POSINT
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}

static int LBSUPL_WLAN_test11(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    /*Wlan Info is given after the Mob Info request cmd SET Signal Strength Invalid 130*/
    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSUPLWLANInf 1 1 0 0 0 1 0 0 0 0 0 0 0 0 130");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSLOCFWD 1 1 30 60 100 5 1 portalnmms");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003a02000080004c000404850d961fe47ba240216b6800095a3a04a05021080200080200000025ccaf000ae0218400009e1bf974c3b376ddbcc0 58 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A020000C0004C000404850D961FE400000004605400119004 26");//SUPL_RESP                             
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e890085ada0002568e812814084202df00000000800200800000090 53 10");//SUPL_POSINT
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}

static int LBSUPL_WLAN_test12(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    /*Wlan Info is given after the Mob Info request cmd Device Type Invalid  4*/
    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSUPLWLANInf 1 1 0 0 0 4 0 0 0 0 0 0 0 0 0");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSLOCFWD 1 1 30 60 100 5 1 portalnmms");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003a02000080004c000404850d961fe47ba240216b6800095a3a04a05021080200080200000025ccaf000ae0218400009e1bf974c3b376ddbcc0 58 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A020000C0004C000404850D961FE400000004605400119004 26");//SUPL_RESP                             
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e890085ada0002568e812814084202df00000000800200800000090 53 10");//SUPL_POSINT
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}

static int LBSUPL_WLAN_test13(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    /*Wlan Info is given after the Mob Info request cmd RTD Accuracy Invalid 300*/
    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSUPLWLANInf 1 1 0 0 0 0 0 0 0 0 300 0 0 0 0");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSLOCFWD 1 1 30 60 100 5 1 portalnmms");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003a02000080004c000404850d961fe47ba240216b6800095a3a04a05021080200080200000025ccaf000ae0218400009e1bf974c3b376ddbcc0 58 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A020000C0004C000404850D961FE400000004605400119004 26");//SUPL_RESP                             
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e890085ada0002568e812814084202df00000000800200800000090 53 10");//SUPL_POSINT
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}
static int LBSUPL_WLAN_test14(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    /*Wlan Info is given after the Mob Info request cmd RTD Value -1*/
    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSUPLWLANInf 1 1 0 0 0 0 0 0 -1 0 0 0 0 0 0");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSLOCFWD 1 1 30 60 100 5 1 portalnmms");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003a02000080004c000404850d961fe47ba240216b6800095a3a04a05021080200080200000025ccaf000ae0218400009e1bf974c3b376ddbcc0 58 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A020000C0004C000404850D961FE400000004605400119004 26");//SUPL_RESP                             
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e890085ada0002568e812814084202df00000000800200800000090 53 10");//SUPL_POSINT
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}

static int LBSUPL_WLAN_test15(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    /*Wlan Info is given after the Mob Info request cmd RTDunits Invalid 5*/
    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSUPLWLANInf 1 1 0 0 0 0 0 0 0 5 0 0 0 0 0");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSLOCFWD 1 1 30 60 100 5 1 portalnmms");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003a02000080004c000404850d961fe47ba240216b6800095a3a04a05021080200080200000025ccaf000ae0218400009e1bf974c3b376ddbcc0 58 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A020000C0004C000404850D961FE400000004605400119004 26");//SUPL_RESP                             
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e890085ada0002568e812814084202df00000000800200800000090 53 10");//SUPL_POSINT
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}


static int LBSUPL_WLAN_test16(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    /*Wlan Info is given after the Mob Info request cmd Access Point Channel Frequency Invalid 300*/
    TCBEGIN(handle, __func__) ;
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSUPLWLANInf 1 1 0 0 0 0 0 300 0 0 0 0 0 0 0");
    /* MLID not supported for this test case */
    SEND("NAF LBSCFGDEL 16384");
    SEND("NAF LBSLOCFWD 1 1 30 60 100 5 1 portalnmms");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 003a02000080004c000404850d961fe47ba240216b6800095a3a04a05021080200080200000025ccaf000ae0218400009e1bf974c3b376ddbcc0 58 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A020000C0004C000404850D961FE400000004605400119004 26");//SUPL_RESP                             
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e890085ada0002568e812814084202df00000000800200800000090 53 10");//SUPL_POSINT
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}


/**********************************************************************
Multiple Location ID : WCDMA Network is supported , SUPL Triggred Start , 2 WCDMA cell Info
 **********************************************************************/
static int LBSUPL_MLID_001(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    /* This is to ensure that Previous Mobile Network Informationgets deleted */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");

    SEND("SUPL LBSSUPLMobInf 1 23 27 145 1 1 001010123456790");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 17 19 123 1 1 001010123456789");

    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 001602000040000000460540011840100821804BFF40 22");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 004E020000C0004C000404850D961FE40000000460540011C019A90B8240216B6800095A3B04A050210802008826000001644BC95967E28B3F9810401104C00001ECD1005C1B0000001007D04000 78 48");

    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


/***************************************************************
Multiple Location ID : WCDMA Network is supported , SUPL POS INIT , 2 WCDMA cell Info
 ****************************************************************/
static int LBSUPL_MLID_002(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    /* This is to ensure that Previous Mobile Network Informationgets deleted */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");

    SEND("SUPL LBSSUPLMobInf 1 23 27 145 1 1 001010123456790");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 17 19 123 1 1 001010123456789");

    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 001602000040000000460540011840100821004BFF00 22");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* Receive SUPL POS INIT */
    RECV("SUPL LBSDATARECV 1 0052020000C0004C000404850D961FE400000004605400119E6E090085ADA0002568EC12814084202DF00000000802209800000593DB5FC0EBA58D32401134041004413000007B34401706C000000401F400 82 10");

    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


/*********************************************************************
Multiple Location ID : WCDMA Network is supported , SUPL Triggred Start , 2 WCDMA and 1 GSM cell Info
 **********************************************************************/
static int LBSUPL_MLID_003(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    /* This is to ensure that Previous Mobile Network Informationgets deleted */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");

    SEND("SUPL LBSSUPLMobInf 1 23 27 145 1 1 001010123456790");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 22 22 222 1 0 001010123456789");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 17 19 123 1 1 001010123456789");

    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 001602000040000000460540011840100821804BFF40 22");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 004E020000C0004C000404850D961FE40000000460540011C019A90B8240216B6800095A3B04A050210802008826000001646355A977CB6A4A1410401104C00001ECD1005C1B000000100FA04000 78 10");

    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


/****************************************************************
Multiple Location ID : WCDMA Network is supported , SUPL POS INIT , 2 WCDMA and 1 WLAN cell Info
 ****************************************************************/
static int LBSUPL_MLID_004(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    /* This is to ensure that Previous Mobile Network Informationgets deleted */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");

    SEND("SUPL LBSSUPLMobInf 1 23 27 145 1 1 001010123456790");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLWLANInf 12345 22222222 1 1 1 0 1 1 1 0 11 11 11 11 11");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 17 19 123 1 1 001010123456789");

    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 001602000040000000460540011840100821004BFF00 22");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* Receive SUPL POS INIT */
    RECV("SUPL LBSDATARECV 1 0052020000C0004C000404850D961FE400000004605400119E6E090085ADA0002568EC12814084202DF00000000802209800000595067BFC53A83A26D01134041004413000007B34401706C000000483E800 82 10");

    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


/*****************************************************************
Multiple Location ID : GSM Network is supported , SUPL Triggred Start , 3 GSM cell Info
 ******************************************************************/
static int LBSUPL_MLID_005(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    /* This is to ensure that Previous Mobile Network Informationgets deleted */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");

    SEND("SUPL LBSSUPLMobInf 1 11 11 111 1 0 001010123456790");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 22 22 222 1 0 001010123456789");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 33 33 333 1 0 00123456789");

    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 0016020000400000004605400118401008218017FD00 22");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 005D020000C0004C00850D961FE7FFFC0000000460540011C021290B8240216B6800095A3B04A0502108004210840004053400AD1AB022DB6EB9368401084210001014D0034082C0B0000806F00003E82040B02C000401BC0003E81000 93 10");

    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


/*********************************************************
Multiple Location ID : GSM Network is supported , SUPL POS INIT , 3 GSM cell Info
 **********************************************************/
static int LBSUPL_MLID_006(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    /* This is to ensure that Previous Mobile Network Informationgets deleted */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");

    SEND("SUPL LBSSUPLMobInf 1 11 11 111 1 0 001010123456790");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 22 22 222 1 0 001010123456789");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 33 33 333 1 0 00123456789");

    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 0016020000400000004605400118401008210017FC00 22");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* Receive SUPL POS INIT */
    RECV("SUPL LBSDATARECV 1 0061020000C0004C00850D961FE7FFFC00000004605400119E6E090085ADA0002568EC12814084202DF000000001084210001014D003EA1F6C3319C23E58024281004210840004053400D020B02C000201BC0000FA08102C0B0001006F0000FA00 97 10");

    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


/**************************************************************************
Multiple Location ID : GSM Network is supported , SUPL Triggred Start , 2 GSM and 1 WLAN cell Info
 ***************************************************************************/
static int LBSUPL_MLID_007(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    /* This is to ensure that Previous Mobile Network Informationgets deleted */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");

    SEND("SUPL LBSSUPLMobInf 1 11 11 111 1 0 001010123456790");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLWLANInf 12345 22222222 1 1 1 0 1 1 1 0 11 11 11 11 11");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 33 33 333 1 0 00123456789");

    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 0016020000400000004605400118401008218017FD00 22");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 005D020000C0004C00850D961FE7FFFC0000000460540011C021290B8240216B6800095A3B04A0502108004210840004053400B4EDA4F2FD0578148401084210001014D0034082C0B0000806F00003E82040B02C000401BC0003E81000 93 10");

    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


/**********************************************************************
Multiple Location ID : GSM Network is supported , SUPL POS INIT , 2 GSM and 1 WCDMA cell Info
 ***********************************************************************/
static int LBSUPL_MLID_008(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    /* This is to ensure that Previous Mobile Network Informationgets deleted */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");

    SEND("SUPL LBSSUPLMobInf 1 11 11 111 1 0 001010123456790");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 22 22 222 1 1 001010123456789");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 33 33 333 1 0 00123456789");

    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 0016020000400000004605400118401008210017FC00 22");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* Receive SUPL POS INIT */
    RECV("SUPL LBSDATARECV 1 0056020000C0004C00850D961FE7FFFC00000004605400119E6E090085ADA0002568EC12814084202DF000000001084210001014D002BF0B30B4E6CB0774022C80804210840004053400D0205816000200DE0001F400 86 10");

    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


/*****************************************************************
Multiple Location ID : WLAN Network is supported , SUPL Triggred Start , 3 WLANcell Info
 ******************************************************************/
static int LBSUPL_MLID_009(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    /* This is to ensure that Previous Mobile Network Informationgets deleted */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");

    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSUPLWLANInf 65535 11111111 3 4 5 0 7 8 9 0 11 12 13 14 15");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLWLANInf 12345 22222222 1 1 1 0 1 1 1 0 11 11 11 11 11");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLWLANInf 19991 33333333 73 74 70 0 77 70 79 0 71 72 73 74 75");

    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 001702000040000000460540011840100829812FFF3FE8 23");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 00A1020000C0004C000404850D961FE40000000460540011C04042A90B8240216B6800095A3B04A05021080801737FF2AD07E008B806464E28CC232000001047C7C97FCA038CB75DB21E083C6410801737FF2AD07E008B806464E28CC232000001047C7C97FCA035002E6FFE8E15530139008080801000140000020171514FF14040FA0A005CDFFD8F155201FE0105070821810800000402E2E35FE38083E81000 161 10");

    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


/*********************************************************
Multiple Location ID : WLAN Network is supported , SUPL POS INIT , 3 WLAN cell Info
 **********************************************************/
static int LBSUPL_MLID_010(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    /* This is to ensure that Previous Mobile Network Informationgets deleted */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");

    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSUPLWLANInf 65535 11111111 3 4 5 0 7 8 9 0 11 12 13 14 15");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLWLANInf 12345 22222222 1 1 1 0 1 1 1 0 11 11 11 11 11");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLWLANInf 19991 33333333 73 74 70 0 77 70 79 0 71 72 73 74 75");

    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 001702000040000000460540011840100829012FFF3FE0 23");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* Receive SUPL POS INIT */
    RECV("SUPL LBSDATARECV 1 00A4020000C0004C000404850D961FE400000004605400119E6E090085ADA0002568EC12814084202DF00000002005CDFFCAB41F8022E0191938A3308C800000411F1F25FF2809CF897B26865932780A9A042005CDFFCAB41F8022E0191938A3308C800000411F1F25FF280D400B9BFFA38554C04E40202020040005000000805C5453FC50103E82801737FF63C554807F804141C208604200000100B8B8D7F8E020FA00 164 10");

    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


/*******************************************************************************
Multiple Location ID : WLAN Network is supported , SUPL Triggred Start , 2 WLAN and 2 WCDMA cell Info
 *******************************************************************************/
static int LBSUPL_MLID_011(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    /* This is to ensure that Previous Mobile Network Informationgets deleted */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");

    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSUPLWLANInf 65535 11111111 3 4 5 0 7 8 9 0 11 12 13 14 15");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 2 2 2 2 2 1 002220222456789");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLWLANInf 19991 33333333 73 74 70 0 77 70 79 0 71 72 73 74 75");

    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 001702000040000000460540011840100829812FFF3FE8 23");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 0084020000C000CC0088088909961FE40000000460540011C034A90B8240216B6800095A3B04A05021080801737FF2AD07E008B806464E28CC232000001047C7C97FCA02D730B2B9469BE0DA08801737FF2AD07E008B806464E28CC232000001047C7C97FCA035002E6FFEC78AA900FF0082838410C084000002017171AFF1C0020D0800 132 10");

    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


/*******************************************************************************
Multiple Location ID : WLAN Network is supported , SUPL POS INIT, 2 WLAN, 1 WCDMA and 1 GSM cell Info
 *******************************************************************************/
static int LBSUPL_MLID_012(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    /* This is to ensure that Previous Mobile Network Informationgets deleted */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");

    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("SUPL LBSSUPLWLANInf 65535 11111111 3 4 5 0 7 8 9 0 11 12 13 14 15");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 2 2 2 2 2 0 002220222456789");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLWLANInf 19991 33333333 73 74 70 0 77 70 79 0 71 72 73 74 75");

    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 001702000040000000460540011840100829012FFF3FE0 23");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* Receive SUPL POS INIT */
    RECV("SUPL LBSDATARECV 1 0088020000c0004c0088088909961fe400000004605400119e6e090085ada0002568ec12814084202df00000002005cdffcab41f8022e0191938a3308c800002411f1f25ff280921596628b83f443809ba022005cdffcab41f8022e0191938a3308c800002411f1f25ff280d400b9bffb1e2aa403fc020a0e1043021000000805c5c6bfc70008980 136 10");

    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


/***************************************************************************************
Multiple Location ID : GSM and WCDMA Network is supported , SUPL Triggred Start , 2 GSM and 1 WCDMA cell Info
 ***************************************************************************************/
static int LBSUPL_MLID_013(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    /* This is to ensure that Previous Mobile Network Informationgets deleted */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");

    SEND("SUPL LBSSUPLMobInf 1 11 11 111 1 0 001010123456790");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 22 22 222 1 1 001010123456789");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 33 33 333 1 0 00123456789");

    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 001602000040000000460540011840100821805BFF40 22");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 005b020000c0004c00850d961fe7fffc0000000460540011c020290b8240216b6800095a3b04a0502108004210840004053400b7a8b7346ccba7b40401084210001014d003440160580000378022610205816000200de00020d080 91 10");

    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


/*******************************************************************************
Multiple Location ID : GSM and WCDMA Network is supported , SUPL POS INIT , 1 GSM and 2 WCDMA cell Info
 ********************************************************************************/
static int LBSUPL_MLID_014(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    /* This is to ensure that Previous Mobile Network Informationgets deleted */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");

    SEND("SUPL LBSSUPLMobInf 1 11 11 111 1 1 001010123456790");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 22 22 222 1 0 001010123456789");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 33 33 333 1 1 00123456789");

    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 001602000040000000460540011840100821005BFF00 22");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* Receive SUPL POS INIT */
    RECV("SUPL LBSDATARECV 1 005d020000c0004c00850d961fe7fffc00000004605400119e6e090085ada0002568ec12814084202df0000000080421080000a69251a8d1051bf60d1011e4081008421000014d34082c0b0000806f00004b02200581600000de022600 93 10");

    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


/********************************************************************************************
Multiple Location ID : GSM and WCDMA Network is supported , SUPL Triggred Start , 2 GSM, 2 WLAN and 2 WCDMA cell Info
 *********************************************************************************************/
static int LBSUPL_MLID_015(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    /* This is to ensure that Previous Mobile Network Informationgets deleted */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");

    SEND("SUPL LBSSUPLWLANInf 11111 11111111 11 11 11 0 11 11 11 0 11 11 11 11 11");
    TEST_SUPL_WAIT_TIMEOUT(40);
    SEND("SUPL LBSSUPLMobInf 1 11 11 111 1 0 001010123456790");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 22 22 222 1 1 001010123456789");
    TEST_SUPL_WAIT_TIMEOUT(20);
    SEND("SUPL LBSSUPLWLANInf 19991 33333333 73 74 70 0 77 70 79 0 71 72 73 74 75");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 44 44 444 1 1 004444444444444");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 33 33 333 1 0 00123456789");

    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 001602000040000000460540011840100821805BFF40 22");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 0065020000c0004c00850d961fe7fffc0000000460540011c025290b8240216b6800095a3b04a05021080042108400040534009a4cfed234d5e4610601084210001014d0034402c0b000006f002581100581600000de220d04081605800080378001450200 101 10");

    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


/**************************************************************************************
Multiple Location ID : GSM and WCDMA Network is supported , SUPL POS INIT , 2 GSM, 2 WLAN and 2 WCDMA cell Info
 ***************************************************************************************/
static int LBSUPL_MLID_016(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    /* This is to ensure that Previous Mobile Network Informationgets deleted */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");

    SEND("SUPL LBSSUPLWLANInf 11111 11111111 11 11 11 0 11 11 11 0 11 11 11 11 11");
    TEST_SUPL_WAIT_TIMEOUT(40);
    SEND("SUPL LBSSUPLMobInf 1 11 11 111 1 1 001010123456790");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 22 22 222 1 0 001010123456789");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 44 44 444 1 0 004444444444444");
    TEST_SUPL_WAIT_TIMEOUT(20);
    SEND("SUPL LBSSUPLWLANInf 19991 33333333 73 74 70 0 77 70 79 0 71 72 73 74 75");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 33 33 333 1 1 00123456789");

    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 001602000040000000460540011840100821005BFF00 22");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* Receive SUPL POS INIT */
    RECV("SUPL LBSDATARECV 1 0068020000c0004c00850d961fe7fffc00000004605400119e6e090085ada0002568ec12814084202df0000000080421080000a69d7ae2eaecbab7230012940c1008421000014d34085816000080de0010c1c204160580004037800080211002c0b000006f027d80 104 10");

    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


/***************************************************************************************
Multiple Location ID : WLAN and WCDMA Network is supported , SUPL Triggred Start , 2 WLAN and 1 WCDMA cell Info
 ***************************************************************************************/
static int LBSUPL_MLID_017(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    /* This is to ensure that Previous Mobile Network Informationgets deleted */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");

    SEND("SUPL LBSSUPLWLANInf 65535 11111111 3 4 5 0 7 8 9 0 11 12 13 14 15");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 22 22 222 1 1 001010123456789");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLWLANInf 12345 22222222 1 1 1 0 1 1 1 0 11 11 11 11 11");

    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 001802000040000000460540011840100831816FFF5FFA00 24");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 008e020000c0004c000404850d961fe40000000460540011c039a90b8240216b6800095a3b04a05021080801737ff470aa9809c8040404008000a00000100b8a8a7f8a030693f6c873b0702410801737ff470aa9809c8040404008000a00000100b8a8a7f8a0344016058000037882581400b9bffb1e2aa403fc020a0e1043021000000805c5c6bfc70008982000 142 10");

    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


/*******************************************************************************
Multiple Location ID : WLAN and WCDMA Network is supported ,SUPL POS INIT, 1 WLAN and 2 WCDMA cell Info
 ********************************************************************************/
static int LBSUPL_MLID_018(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    /* This is to ensure that Previous Mobile Network Informationgets deleted */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");

    SEND("SUPL LBSSUPLMobInf 1 11 11 111 1 1 001010123456790");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLWLANInf 12345 22222222 1 1 1 0 1 1 1 0 11 11 11 11 11");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 33 33 333 1 1 00123456789");

    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 001702000040000000460540011840100829016FFF5FF8 23");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* Receive SUPL POS INIT */
    RECV("SUPL LBSDATARECV 1 006e020000c0004c00850d961fe7fffc00000004605400119e6e090085ada0002568ec12814084202df0000000080421080000a693b618eb81acf4d5f012f4081008421000014d35002e6ffe8e15530139008080801000140000020171514ff14001130880160580000379083400 110 10");

    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


/********************************************************************************************
Multiple Location ID : WLAN and WCDMA Network is supported , SUPL Triggred Start , 2 GSM, 2 WLAN and 2 WCDMA cell Info
 *********************************************************************************************/
static int LBSUPL_MLID_019(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    /* This is to ensure that Previous Mobile Network Informationgets deleted */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");

    SEND("SUPL LBSSUPLWLANInf 11111 11111111 11 11 11 0 11 11 11 0 11 11 11 11 11");
    TEST_SUPL_WAIT_TIMEOUT(40);
    SEND("SUPL LBSSUPLMobInf 1 11 11 111 1 0 001010123456790");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 22 22 222 1 1 001010123456789");
    TEST_SUPL_WAIT_TIMEOUT(20);
    SEND("SUPL LBSSUPLWLANInf 19991 33333333 73 74 70 0 77 70 79 0 71 72 73 74 75");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 44 44 444 1 1 004444444444444");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 33 33 333 1 0 00123456789");

    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 001802000040000000460540011840100831816FFF5FFA00 24");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 0089020000c0004c00850d961fe7fffc0000000460540011c037290b8240216b6800095a3b04a0502108004210840004053400f7f8fb61e5236e9006880581600000de004b02801737ff2ad07e008b806464e28cc232000009047c7c97fca00113044016058000037888341400b9bffb1e2aa4019c022a2a284502d000000805c5453fc50023f02000 137 10");

    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


/**************************************************************************************
Multiple Location ID : WLAN and WCDMA Network is supported , SUPL POS INIT , 2 GSM, 2 WLAN and 2 WCDMA cell Info
 ***************************************************************************************/
static int LBSUPL_MLID_020(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    /* This is to ensure that Previous Mobile Network Informationgets deleted */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");

    SEND("SUPL LBSSUPLWLANInf 11111 11111111 11 11 11 0 11 11 11 0 11 11 11 11 11");
    TEST_SUPL_WAIT_TIMEOUT(30);
    SEND("SUPL LBSSUPLMobInf 1 11 11 111 1 1 001010123456790");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 22 22 222 1 0 001010123456789");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 44 44 444 1 0 004444444444444");
    TEST_SUPL_WAIT_TIMEOUT(20);
    SEND("SUPL LBSSUPLWLANInf 19991 33333333 73 74 70 0 77 70 79 0 71 72 73 74 75");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 33 33 333 1 1 00123456789");

    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 001702000040000000460540011840100829016FFF5FF8 23");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* Receive SUPL POS INIT */
    RECV("SUPL LBSDATARECV 1 008a020000c0004c00850d961fe7fffc00000004605400119e6e090085ada0002568ec12814084202df0000000080421080000a6916fa7c6a83e5b0b2014b40c1008421000014d35002e6ffe55a0fc011700c8c9c519846400001208f8f92ff940012c088016058000037814502801737ff63c55480338045454508a05a00000100b8a8a7f8a00401000 138 10");

    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


/***************************************************************************************
Multiple Location ID : WLAN and GSM Network is supported , SUPL Triggred Start , 2 WLAN and 1 GSM cell Info
 ***************************************************************************************/
static int LBSUPL_MLID_021(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    /* This is to ensure that Previous Mobile Network Informationgets deleted */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");

    SEND("SUPL LBSSUPLWLANInf 65535 11111111 3 4 5 0 7 8 9 0 11 12 13 14 15");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 22 22 222 1 0 001010123456789");
    TEST_SUPL_WAIT_TIMEOUT(35);
    SEND("SUPL LBSSUPLWLANInf 12345 22222222 1 1 1 0 1 1 1 0 11 11 11 11 11");

    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 001702000040000000460540011840100829812FFFBFE8 23");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 008f020000c0004c000404850d961fe40000000460540011c03a290b8240216b6800095a3b04a05021080801737ff470aa9809c8040404008000a00000100b8a8a7f8a02e6c3209f7054912210801737ff470aa9809c8040404008000a00000100b8a8a7f8a034082c0b0000806f0010e742801737ff63c554807f804141c208604200000100b8b8d7f8e0024b8400 143 10");

    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


/*******************************************************************************
Multiple Location ID : WLAN and GSM Network is supported , SUPL POS INIT , 1 WLAN and 2 GSM cell Info
 ********************************************************************************/
static int LBSUPL_MLID_022(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    /* This is to ensure that Previous Mobile Network Informationgets deleted */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");

    SEND("SUPL LBSSUPLMobInf 1 11 11 111 1 0 001010123456790");
    TEST_SUPL_WAIT_TIMEOUT(20);
    SEND("SUPL LBSSUPLWLANInf 12345 22222222 1 1 1 0 1 1 1 0 11 11 11 11 11");
    TEST_SUPL_WAIT_TIMEOUT(15);
    SEND("SUPL LBSSUPLMobInf 1 33 33 333 1 0 00123456789");

    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 001702000040000000460540011840100829012FFFBFE0 23");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* Receive SUPL POS INIT */
    RECV("SUPL LBSDATARECV 1 0072020000c0004c00850d961fe7fffc00000004605400119e6e090085ada0002568ec12814084202df000000001084210001014d002b984963dad9ff2c0026481004210840004053400d400b9bffa38554c04e40202020040005000000805c5453fc50006a42040b02c000401bc00873a00 114 10");

    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


/********************************************************************************************
Multiple Location ID : WLAN and GSM Network is supported , SUPL Triggred Start , 2 GSM, 2 WLAN and 2 WCDMA cell Info
 *********************************************************************************************/
static int LBSUPL_MLID_023(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    /* This is to ensure that Previous Mobile Network Informationgets deleted */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");

    SEND("SUPL LBSSUPLWLANInf 11111 11111111 11 11 11 0 11 11 11 0 11 11 11 11 11");
    TEST_SUPL_WAIT_TIMEOUT(40);
    SEND("SUPL LBSSUPLMobInf 1 11 11 111 1 0 001010123456790");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 22 22 222 1 1 001010123456789");
    TEST_SUPL_WAIT_TIMEOUT(20);
    SEND("SUPL LBSSUPLWLANInf 19991 33333333 73 74 70 0 77 70 79 0 71 72 73 74 75");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 44 44 444 1 1 004444444444444");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 33 33 333 1 0 00123456789");

    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 001702000040000000460540011840100829812FFFBFE8 23");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 0089020000c0004c00850d961fe7fffc0000000460540011c037290b8240216b6800095a3b04a0502108004210840004053400f4fe4d03b8dbe4100601084210001014d0035002e6ffe55a0fc011700c8c9c519846400001208f8f92ff940020d08102c0b0001006f00027d85002e6ffec78aa90067008a8a8a1140b40000020171514ff14008e3080 137 10");

    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


/**************************************************************************************
Multiple Location ID : WLAN and GSM Network is supported , SUPL POS INIT , 2 GSM, 2 WLAN and 2 WCDMA cell Info
 ***************************************************************************************/
static int LBSUPL_MLID_024(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    /* This is to ensure that Previous Mobile Network Informationgets deleted */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");

    SEND("SUPL LBSSUPLWLANInf 11111 11111111 11 11 11 0 11 11 11 0 11 11 11 11 11");
    TEST_SUPL_WAIT_TIMEOUT(30);
    SEND("SUPL LBSSUPLMobInf 1 11 11 111 1 1 001010123456790");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 22 22 222 1 0 001010123456789");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 44 44 444 1 0 004444444444444");
    TEST_SUPL_WAIT_TIMEOUT(20);
    SEND("SUPL LBSSUPLWLANInf 19991 33333333 73 74 70 0 77 70 79 0 71 72 73 74 75");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 33 33 333 1 1 00123456789");

    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 001702000040000000460540011840100829012FFFBFE0 23");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* Receive SUPL POS INIT */
    RECV("SUPL LBSDATARECV 1 008f020000c0004c00850d961fe7fffc00000004605400119e6e090085ada0002568ec12814084202df0000000080421080000a695cc24b1ed6cff960015040d400b9bff95683f0045c0323271466119000004823e3e4bfe50004b02042c0b0000406f00086401020b02c000201bc00041a0a005cdffd8f155200ce0115151422816800000402e2a29fe2801004000 143 10");

    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


/***********************************************************************************
Multiple Location ID : All Networks are supported , SUPL Triggred Start , 2 GSM, 2 WLAN and 2 WCDMA cell Info
 ************************************************************************************/
static int LBSUPL_MLID_025(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    /* This is to ensure that Previous Mobile Network Informationgets deleted */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");

    SEND("SUPL LBSSUPLWLANInf 11111 11111111 11 11 11 0 11 11 11 0 11 11 11 11 11");
    TEST_SUPL_WAIT_TIMEOUT(30);
    SEND("SUPL LBSSUPLMobInf 1 11 11 111 1 1 001010123456790");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 22 22 222 1 0 001010123456789");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 44 44 444 1 0 004444444444444");
    TEST_SUPL_WAIT_TIMEOUT(20);
    SEND("SUPL LBSSUPLWLANInf 19991 33333333 73 74 70 0 77 70 79 0 71 72 73 74 75");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 33 33 333 1 1 00123456789");

    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 001802000040000000460540011840100831816FFFDFFA00 24");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 009d020000c0004c00850d961fe7fffc0000000460540011c04040a90b8240216b6800095a3b04a050210802010842000029a6805c57dfa01606e45040210840000534d400b9bff95683f0045c0323271466119000004823e3e4bfe500044c2042c0b0000406f000860e1020b02c000201bc000401088016058000037813ec2801737ff63c55480338045454508a05a00000100b8a8a7f8a003f484000 157 10");

    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


/****************************************************************************
Multiple Location ID : All Networks are supported , SUPL POS INIT , 2 GSM, 2 WLAN and 2 WCDMA cell Info
 *****************************************************************************/
static int LBSUPL_MLID_026(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    /* This is to ensure that Previous Mobile Network Informationgets deleted */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");

    SEND("SUPL LBSSUPLWLANInf 11111 11111111 11 11 11 0 11 11 11 0 11 11 11 11 11");
    TEST_SUPL_WAIT_TIMEOUT(30);
    SEND("SUPL LBSSUPLMobInf 1 11 11 111 1 1 001010123456790");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 22 22 222 1 0 001010123456789");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 44 44 444 1 0 004444444444444");
    TEST_SUPL_WAIT_TIMEOUT(20);
    SEND("SUPL LBSSUPLWLANInf 19991 33333333 73 74 70 0 77 70 79 0 71 72 73 74 75");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 33 33 333 1 1 00123456789");

    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 001702000040000000460540011840100829016FFFDFF8 23");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* Receive SUPL POS INIT */
    RECV("SUPL LBSDATARECV 1 00a0020000c0004c00850d961fe7fffc00000004605400119e6e090085ada0002568ec12814084202df0000000080421080000a697bf7e6c3e03ddb9901614141008421000014d35002e6ffe55a0fc011700c8c9c519846400001208f8f92ff940012c0810b02c000101bc00219004082c0b0000806f00010682200581600000de05140a005cdffd8f155200ce0115151422816800000402e2a29fe280100400 160 10");

    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

/********************************************************************************************
Multiple Location ID : No Network is supported , SUPL Triggred Start , 1 GSM, 1 WLAN and 1 WCDMA cell Info
 *********************************************************************************************/
static int LBSUPL_MLID_027(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    /* This is to ensure that Previous Mobile Network Informationgets deleted */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");

    SEND("SUPL LBSSUPLWLANInf 11111 11111111 11 11 11 0 11 11 11 0 11 11 11 11 11");
    TEST_SUPL_WAIT_TIMEOUT(40);
    SEND("SUPL LBSSUPLMobInf 1 11 11 111 1 0 001010123456790");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 22 22 222 1 1 001010123456789");

    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 0016020000400000004605400118401008218003FD00 22");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 003b020000c0004c000404850d961fe40000000460540011c010210b8240216b6800095a3b04a05021080200b02c00001bc4ca8d19b9466814b900 59 10");

    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}


/**************************************************************************************
Multiple Location ID : No Network is supported , SUPL POS INIT , 1 GSM, 1 WLAN and 1 WCDMA cell Info
 ***************************************************************************************/
static int LBSUPL_MLID_028(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    /* This is to ensure that Previous Mobile Network Informationgets deleted */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");

    SEND("SUPL LBSSUPLWLANInf 11111 11111111 11 11 11 0 11 11 11 0 11 11 11 11 11");
    TEST_SUPL_WAIT_TIMEOUT(30);
    SEND("SUPL LBSSUPLMobInf 1 11 11 111 1 1 001010123456790");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 22 22 222 1 0 001010123456789");

    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 0016020000400000004605400118401008210003FC00 22");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* Receive SUPL POS INIT */
    RECV("SUPL LBSDATARECV 1 0041020000c0008c000404850d961fe400000004605400119e6e090085ada0002568ec12814084202df00000000105816000100de003ad8a1c5f24502dd2020200 65 10");

    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

/***************************************************************************************
Multiple Location ID : WLAN and WCDMA Network is supported , SUPL Triggred Start , 2 WLAN and 1 WCDMA cell Info
 with time diffrence more than 11 mins
 ***************************************************************************************/
static int LBSUPL_MLID_029(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    /* This is to ensure that Previous Mobile Network Informationgets deleted */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");

    SEND("SUPL LBSSUPLWLANInf 65535 11111111 3 4 5 0 7 8 9 0 11 12 13 14 15");
    TEST_SUPL_WAIT_TIMEOUT(660);
    SEND("SUPL LBSSUPLMobInf 1 22 22 222 1 1 001010123456789");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLWLANInf 12345 22222222 1 1 1 0 1 1 1 0 11 11 11 11 11");
    TEST_SUPL_WAIT_TIMEOUT(35);
    SEND("SUPL LBSSUPLMobInf 1 33 33 333 1 1 001010122226789");
    TEST_SUPL_WAIT_TIMEOUT(20);
    SEND("SUPL LBSSUPLMobInf 1 44 44 444 1 1 00123499989");

    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 001802000040000000460540011840100831816FFF5FFA00 24");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* Receive SUPL TRIGGERED START */
    RECV("SUPL LBSDATARECV 1 0073020000c0004c00850e6627e7fffc0000000460540011c02c290b8240216b6800095a3b04a05021080201605800003787bfc7db0f291b748030402c0b000006f0d1008421000014d011305002e6ffe8e15530139008080801000140000020171514ff140059108802c0b000006f11a2c200 115 10");

    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

/*********************************************************
Multiple Location ID : GSM Network is supported , SUPL POS INIT , 1 GSM and 2 WCDMA cell Info
with time diffrence more than 11 mins
 **********************************************************/
static int LBSUPL_MLID_030(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TCBEGIN(handle, __func__) ;
    /* This is to ensure that Previous Mobile Network Informationgets deleted */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");

    SEND("SUPL LBSSUPLMobInf 1 11 11 111 1 1 001010123456790");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 22 22 222 1 0 001010123456789");
    TEST_SUPL_WAIT_TIMEOUT(660);
    SEND("SUPL LBSSUPLMobInf 1 33 33 333 1 0 001010122226789");
    TEST_SUPL_WAIT_TIMEOUT(80);
    SEND("SUPL LBSSUPLMobInf 1 44 44 444 1 1 00123499989");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 55 55 555 1 1 00123488889");

    /* Send SUPL_INIT */
    SEND("SUPL LBSSmsPush 1 0016020000400000004605400118401008210017FC00 22");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    /* Receive SUPL POS INIT */
    RECV("SUPL LBSDATARECV 1 004c020000c0004c00850e2223e7fffc00000004605400119e6e090085ada0002568ec12814084202df00000000806e1b80001159223c72c862674f64010d40102108420002029a0008e3000 76 10");

    /* Send SUPL END */
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF  LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;
}

/****************************************************************************
Multiple Location ID : GSM is supported , SUPL START 2 WCDMA cell Info
 *****************************************************************************/
static int LBSUPL_MLID_031(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    TCBEGIN(handle, __func__) ;
    /* This is to ensure that Previous Mobile Network Informationgets deleted */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 0 001010123456789");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 2 2 2 2 2 1 001010123456779");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 3 3 3 3 3 0 001010123456669");

    SEND("NAF  LBSLOCFWD 1 1 30 60 100 5 1 portalnmms");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success
    RECV("SUPL LBSDATARECV 1 004d0200008006cc000404850d961fe47b8240216b6800095a3b04a05021080200080200000025ccaf000ae023ec04100040100000013440010040000004025800002786fe5d30ecddb76f3000 77 10");//SUPL_START
    SEND("SUPL LBSDATASEND 1 001A020000C0004C000404850D961FE400000004605400119004 26");//SUPL_RESP                             
    RECV("SUPL LBSDATARECV 1 0035020000c0004c000404850d961fe400000004605400119a2e890085ada0002568e812814084202df00000000800200800000090 53 10");//SUPL_POSINT
    SEND("SUPL LBSDATASEND 1 001A020000C000CC000408850D961FE402F8B1E0005F163C2800 26");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}

/****************************************************************************
Multiple Location ID : All Networks are supported , SUPL TRIGGERED START 2 WCDMA cell Info
 *****************************************************************************/
static int LBSUPL_MLID_032(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    /* straight test case as per expected behaviour triggered start*/
    TCBEGIN(handle, __func__) ;
    /* This is to ensure that Previous Mobile Network Informationgets deleted */
    SEND("NAF LBSCFGDEL 16384");
    SEND("SUPL LBSSUPLINIT");
    SEND("NAF LBSCFGMOLR 1");
    SEND("NAF LBSCFGSUPL 2");
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    TEST_SUPL_WAIT_TIMEOUT(10);
    SEND("SUPL LBSSUPLMobInf 1 1 1 1 1 1 001010123456789");
    SEND("NAF LBSLOCFWD 1 1 30 60 100 5 1 portalnmms");
    RECV("SUPL LBSPDPREQ");
    SEND("SUPL LBSPDPCNF 1 bsnl.com");
    RECV("SUPL LBSCONREQ 1 www.spirent-lcs.com 7275");
    SEND("SUPL LBSCONCNF 1 1 0"); //CNF Success

    RECV("SUPL LBSDATARECV 1 0049020000C0004EAAEF3374004488CC02F8B1E0055817084017258BFA4020EA4801095A02108200080200000026DC6D34378E974D900002786FE5D30ECDDB76F3000000240000EC00 73 15");//SUPL_TRIGGERED_START
    //TEST_SUPL_WAIT_MAX_TIMEOUT;
    SEND("SUPL LBSDATASEND 1 001A010000C000CC000408850D961FE402F8B1E0005F163C2800 26");//SUPL_END
    RECV("SUPL LBSDISREQ 1 ");
    SEND("SUPL LBSDISCNF 1");
    RECV("SUPL LBSCLOSEPDPREQ");
    SEND("SUPL LBSCLOSEPDPCNF 1");
    SEND("NAF LBSSTOP 1");
    TCEND(handle, __func__) ;

    return TSTENG_RESULT_OK;

}

int powi(int number, int exponent)
{
   int i, product = 1;
   for (i = 0; i < exponent; i++)
     product *= number;

   return product;
}

