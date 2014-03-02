/*
 * STELP Test App
 *
 * test_sbee.c
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

#include <agpsosa.h>

#include <strings.h>

//#include <gps.h>
//#include <gps_ni.h>

#include <gpsclient.h>
#include <agps_client.h>
#include <sbeesocketinterface.h>
#include <sbeeclient.h>

#include <stepl_utility.h>
#include <test_engine_int.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "LBSSTELP"
#define SEND(a) TSTENG_execute_command(-1,a, NULL)


/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Generic Test Commands

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

static void TEST_SBEE_START(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static void TEST_SBEE_STOP(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static void TEST_SBEE_USER_CONFIGURATION( TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found );
static void TEST_SBEE_SERVER_CONFIGURATION( TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found );

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Unit Test Commands

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

static int TEST_SBEE_0001(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_SBEE_0002(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_SBEE_0003(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_SBEE_0004(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_SBEE_0005(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_SBEE_0006(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_SBEE_0007(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_SBEE_0008(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_SBEE_0009(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_SBEE_0010(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_SBEE_0011(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_SBEE_0012(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_SBEE_0013(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);



/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Test description table

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

TSTENG_TEST_TABLE_BEGIN(SBEE)

/*Generic Test commands*/

/*
Format   lbsstelp SBEE STARTSBEE P1
Example  lbsstelp SBEE STARTSBEE 2
Parameter Field name  Description                   Value
P1 Start-time Relative start time, to start PGPS  0 - n
*/

TSTENG_TEST_TABLE_CMD("SBEESTART", TEST_SBEE_START, "LBSSTELP : Start SBEE")

/*
Format  lbsstelp SBEE STOPSBEE
Example lbsstelp SBEE STOPSBEE
Parameter: None

*/


TSTENG_TEST_TABLE_CMD("SBEESTOP", TEST_SBEE_STOP, "LBSSTELP : Stop SBEE")

/*
Format  lbsstelp SBEE SBEEUSRCFG P1 P2 P3 P4 P5 P6 P7
Example lbsstelp SBEE SBEEUSRCFG 2 0 1 2 42 4 65535

Parameter Field name        Description         Value

P1  ServerConnectionOption  Connection option 0 SBEE_SEED_NEVER
                                                                     1 SBEE_SEED_MANUAL
                                                                     2 SBEE_SEED_AUTO_ALWAYS
                                                                     3 SBEE_SEED_AUTO_ONLY_ON_HOME_NETWORK
                                                                     4 SBEE_SEED_AUTO_ONLY_ON_ROAMING
                                                                     5 SBEE_SEED_AUTO_ALWAYS_ASK

P2  PrefBearerOption  Preferred bearer option 0 SBEE_PREF_BEARER_ANY
                                                                   1 SBEE_PREF_BEARER_CELLULAR
                                                                   2 SBEE_PREF_BEARER_WIFI

P3  ChargingModeOption  Whether to predict if not charging  0 SBEE_PREDICTION_PREF_CHARGING_ONLY
                                                                                       1 SBEE_PREDICTION_PREF_NONE

P4  EEOptionMask  Bit mask for Seed & Broadcast Ephemeris 1             E_SBEE_BROADCAST_EPHEMERIS
                                                                                        (1<<1)    E_SBEE_PREDICTED_EPHEMERIS
                                                                                        (1<<2)    E_SBEE_SELF_PREDICTED_EPHEMERIS

P5  EESeedDownloadFreq  Prediction Duration                      Number of 4 hours blocks. For example, to predict for 1 day, enter 6 (24/4)

P6  EEMaxAllowedSeedDownload  Number of times seed can be downloaded a week (1..n).

*/

TSTENG_TEST_TABLE_CMD("SBEEUSRCFG", TEST_SBEE_USER_CONFIGURATION, "LBSSTELP : Set User Configuration")
  /*
  Format  lbsstelp SBEE SBEESERVCFG P1 P2 P3 P4 P5
  Example lbsstelp SBEE SBEESERVCFG "{009e7bc0-1d39-11dc-b4f8-806d6172696f}" "ST-Ericsson_PGPS_Demo" 0 18 "test1.gpstream.net"
  Parameter Field name  Description                                                                             value
  P1  Device Id Unique identifier of the device                                                                Eg. IMEI number

  P2  IntegratorModelId Identifies the Integrator model                                                   Eg. "ST-Ericsson_PGPS_Demo"

  P3  RetryOption Whether to retry with alternate server if it failed to connect to a server  0 ALLOWED
                                                                                                                                 1 NOT_ALLOWED

  P4  SbeeServersLength Server length                                                                          Length

  P5  ServerAddress Addres to connect to                                                                       Eg. test1.gpstream.net
  */

TSTENG_TEST_TABLE_CMD("SBEESERVCFG", TEST_SBEE_SERVER_CONFIGURATION, "LBSSTELP : Set Server Configuration")

/*Unit test Commands*/
TSTENG_TEST_TABLE_CMD("LBSTID0001", TEST_SBEE_0001, "LBSSTELP : Test Cases LBSTID0001")
TSTENG_TEST_TABLE_CMD("LBSTID0002", TEST_SBEE_0002, "LBSSTELP : Test Cases LBSTID0002")
TSTENG_TEST_TABLE_CMD("LBSTID0003", TEST_SBEE_0003, "LBSSTELP : Test Cases LBSTID0003")
TSTENG_TEST_TABLE_CMD("LBSTID0004", TEST_SBEE_0004, "LBSSTELP : Test Cases LBSTID0004")
TSTENG_TEST_TABLE_CMD("LBSTID0005", TEST_SBEE_0005, "LBSSTELP : Test Cases LBSTID0005")
TSTENG_TEST_TABLE_CMD("LBSTID0006", TEST_SBEE_0006, "LBSSTELP : Test Cases LBSTID0006")
TSTENG_TEST_TABLE_CMD("LBSTID0007", TEST_SBEE_0007, "LBSSTELP : Test Cases LBSTID0007")
TSTENG_TEST_TABLE_CMD("LBSTID0008", TEST_SBEE_0008, "LBSSTELP : Test Cases LBSTID0008")
TSTENG_TEST_TABLE_CMD("LBSTID0009", TEST_SBEE_0009, "LBSSTELP : Test Cases LBSTID0009")
TSTENG_TEST_TABLE_CMD("LBSTID0010", TEST_SBEE_0010, "LBSSTELP : Test Cases LBSTID0010")
TSTENG_TEST_TABLE_CMD("LBSTID0011", TEST_SBEE_0011, "LBSSTELP : Test Cases LBSTID0011")
TSTENG_TEST_TABLE_CMD("LBSTID0012", TEST_SBEE_0012, "LBSSTELP : Test Cases LBSTID0012")
TSTENG_TEST_TABLE_CMD("LBSTID0013", TEST_SBEE_0013, "LBSSTELP : Test Cases LBSTID0013")



TSTENG_TEST_TABLE_END




static t_gpsClient_Callbacks TEST_Sbee_callbacks = {

    .nav_cb = NULL,
    .nmea_cb = NULL,
};


/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Generic Test Commands

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

static void TEST_SBEE_START(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
  int var =0;

  if(args_found < 1)
    {
    var =0;
    TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "LBSSTELP : TEST_SBEE_START - Add Start Time");
    }
  else {

     var = atoi(cmd_buf + arg_index[0]);
    }
  gpsClient_Init( &TEST_Sbee_callbacks );
  sbeeClient_Start( var);
  TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, "LBSSTELP : TEST_SBEE_START - start requested");
  sleep(20);
}

static void TEST_SBEE_STOP(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
  gpsClient_Init( &TEST_Sbee_callbacks );

  sbeeClient_Stop();

  TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, "LBSSTELP : TEST_SBEE_STOP -  stop requested");
}



static void TEST_SBEE_USER_CONFIGURATION( TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found )
{
  uint8_t  v_ServerConnectionOption;
    uint8_t  v_PrefBearerOption;
    uint8_t  v_ChargingModeOption;
    uint8_t  v_EEOptionMask;
    uint8_t  v_EESeedDownloadFreq;
    uint8_t  v_EEMaxAllowedSeedDownload;
    uint16_t v_PrefTimeOfDay;

  if(args_found != 7)
  {
    ALOGD("TEST_SBEE_SERVER_CONFIGURATION : Error, wrong number of arguments, %d\n", args_found);
    return;
  }

  v_ServerConnectionOption = (uint8_t)atoi(cmd_buf + arg_index[0]);
    v_PrefBearerOption = (uint8_t)atoi(cmd_buf + arg_index[1]);
    v_ChargingModeOption = (uint8_t)atoi(cmd_buf + arg_index[2]);
    v_EEOptionMask = (uint8_t)atoi(cmd_buf + arg_index[3]);
    v_EESeedDownloadFreq = (uint8_t)atoi(cmd_buf + arg_index[4]);
    v_EEMaxAllowedSeedDownload = (uint8_t)atoi(cmd_buf + arg_index[5]);
    v_PrefTimeOfDay = (uint16_t)atoi(cmd_buf + arg_index[6]);

  gpsClient_Init( &TEST_Sbee_callbacks );

  sbeeClient_SendUserConfiguration(v_ServerConnectionOption,
                      v_PrefBearerOption,
                      v_ChargingModeOption,
                      v_EEOptionMask,
                                        v_EESeedDownloadFreq,
                                        v_EEMaxAllowedSeedDownload,
                                        v_PrefTimeOfDay);
  sleep(5);

}


static void TEST_SBEE_SERVER_CONFIGURATION( TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found )
{
  int i;
  t_sbee_si_ServerConfiguration sbee_si_ServerConfiguration;

  if(args_found != 5)
  {
    ALOGD("TEST_SBEE_SERVER_CONFIGURATION : Error, wrong number of arguments, %d\n", args_found);
    return;
  }

  strcpy(sbee_si_ServerConfiguration.v_DeviceId, cmd_buf + arg_index[0]);
  strcpy(sbee_si_ServerConfiguration.v_IntegratorModelId, cmd_buf + arg_index[1]);
  sbee_si_ServerConfiguration.v_RetryOption = (uint8_t)atoi(cmd_buf + arg_index[2]);

  sbee_si_ServerConfiguration.v_SbeeServersLength[0] = (uint8_t)atoi(cmd_buf + arg_index[3]);
  ALOGD ("TEST_SBEE_SERVER_CONFIGURATION: The length of the server address is %d\n", sbee_si_ServerConfiguration.v_SbeeServersLength[0]);
  sbee_si_ServerConfiguration.v_SbeeServers[0] = (char*)malloc(sbee_si_ServerConfiguration.v_SbeeServersLength[0] + 1);
  strcpy(sbee_si_ServerConfiguration.v_SbeeServers[0], cmd_buf + arg_index[4]);

  gpsClient_Init( &TEST_Sbee_callbacks );

  sbeeClient_SendServerConfiguration(sbee_si_ServerConfiguration.v_DeviceId,
                      (uint8_t)strlen(sbee_si_ServerConfiguration.v_DeviceId),
                      sbee_si_ServerConfiguration.v_IntegratorModelId,
                      (uint8_t)strlen(sbee_si_ServerConfiguration.v_IntegratorModelId),
                      sbee_si_ServerConfiguration.v_RetryOption,
                      sbee_si_ServerConfiguration.v_SbeeServers[0],
                      sbee_si_ServerConfiguration.v_SbeeServersLength[0]);


  OSA_Free(sbee_si_ServerConfiguration.v_SbeeServers[0]);

  sleep(5);
}










/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Unit Test Commands

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

*/












/*
Test Description: Start Prediction immediately and check predcition file are generated.
                         Boradcast/Prediction: Prediction
                         Prediction                : 7 days
Pass Criteria: EE_*.bin file are generated.
*/

static int TEST_SBEE_0001(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

  SEND("SBEE SBEEUSRCFG 2 0 1 2 42 4 65535");/*2 - Predicted Eph, 42 - (7 days prediction)*/
  SEND("SBEE SBEESERVCFG {009e7bc0-1d39-11dc-b4f8-806d6172696f} ST-Ericsson_PGPS_Demo 0 18 test1.gpstream.net");
  SEND("SBEE SBEESTART 0"); //Start time is 0

 /*Verify File has been created. */
  sleep(180);
  FILE *fp;
  fp=fopen("/data/EE_2.bin" ,"r");

  if(fp==NULL)
  {
    TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "LBSSTELP : TEST_SBEE_08 No file created");
  }
  else
  {
    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, "LBSSTELP : TEST_SBEE_08 - PASS");
  }
  SEND("SBEE SBEESTOP");

  return TSTENG_RESULT_OK;

}





/*
Test Description: Start Prediction and check predcition file are generated.
        Boradcast/Prediction: Prediction
        Prediction                : 7 days
Pass Criteria: EE_*.bin file are generated.
*/

static int TEST_SBEE_0002(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{


  SEND("SBEE SBEEUSRCFG 2 0 1 2 42 4 65535");/*2 - Predicted Eph, 42 - (7 days prediction)*/
  SEND("SBEE SBEESERVCFG {009e7bc0-1d39-11dc-b4f8-806d6172696f} ST-Ericsson_PGPS_Demo 0 18 test1.gpstream.net");
  SEND("SBEE SBEESTART 3");
  sleep(240);

  SEND("SBEE SBEESTOP");

  TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, "LBSSTELP : TEST_SBEE_13 Finished");

  return TSTENG_RESULT_OK;

}


/*
Test Description: Start Prediction and check predcition file are generated.
        Boradcast/Prediction: Prediction
        Prediction                : 7 days
Pass Criteria: EE_*.bin file are generated.
*/
static int TEST_SBEE_0003(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{


  SEND("SBEE SBEEUSRCFG 2 0 1 2 42 4 65535"); /*2 - Predicted Eph, 42 - (7 days prediction)*/
  SEND("SBEE SBEESERVCFG {009e7bc0-1d39-11dc-b4f8-806d6172696f} ST-Ericsson_PGPS_Demo 0 18 test1.gpstream.net");
  SEND("SBEE STARTSBEE 0");
  sleep(60);

  SEND("SBEE SBEESTOP");

  TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, "LBSSTELP : TEST_SBEE_14 Finished");

  return TSTENG_RESULT_OK;

}

/*
Test Description: Start Prediction.
        SeeddownloadOpt: Automatic
        Boradcast/Prediction: Prediction
        Prediction                : 7 days
        Stop the prediction after 60 sec.
Pass Criteria: .
*/

static int TEST_SBEE_0004(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{


  SEND("SBEE SBEEUSRCFG 2 0 1 2 42 4 65535");/*2 - Predicted Eph, 42 - (7 days prediction)*/
  SEND("SBEE SBEESERVCFG {009e7bc0-1d39-11dc-b4f8-806d6172696f} ST-Ericsson_PGPS_Demo 0 18 test1.gpstream.net");
  SEND("SBEE SBEESTART 0");
  sleep(60);

  SEND("SBEE SBEESTOP");

  TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, "LBSSTELP : TEST_SBEE_16 Finished");

  return TSTENG_RESULT_OK;

}


/*
Test Description: Start Prediction and check predcition file are generated.
        SeeddownloadOpt: Never
        Boradcast/Prediction: Prediction
        Prediction                : 7 days
Pass Criteria: EE_*.bin file are generated.
*/

static int TEST_SBEE_0005(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{


  SEND("SBEE SBEEUSRCFG 0 0 1 2 42 4 65535");/*2 - Predicted Eph, 42 - (7 days prediction)*/
  SEND("SBEE SBEESERVCFG {009e7bc0-1d39-11dc-b4f8-806d6172696f} ST-Ericsson_PGPS_Demo 0 18 test1.gpstream.net");
  SEND("SBEE SBEESTART 0");
  sleep(60);

  SEND("SBEE SBEESTOP");

  TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, "LBSSTELP : TEST_SBEE_17 Finished");

  return TSTENG_RESULT_OK;

}


/*
Test Description: Start Prediction and check predcition file are generated.
        SeeddownloadOpt: Never
        Boradcast/Prediction: Prediction
        Prediction                : 4 hr
Pass Criteria: EE_*.bin file are generated.
*/

static int TEST_SBEE_0006(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{


  SEND("SBEE SBEEUSRCFG 0 0 1 2 1 4 65535");/*2 - Predicted Eph, 1 - (4 hr prediction)*/
  SEND("SBEE SBEESERVCFG {009e7bc0-1d39-11dc-b4f8-806d6172696f} ST-Ericsson_PGPS_Demo 0 18 test1.gpstream.net");
  SEND("SBEE SBEESTART 0");
  sleep(60);

  SEND("SBEE SBEESTOP");

  TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, "LBSSTELP : TEST_SBEE_29 Finished");

  return TSTENG_RESULT_OK;

}

/*
Test Description: Start Prediction and check predcition file are generated.
        SeeddownloadOpt: Never
        Boradcast/Prediction: Prediction
        Prediction                : 1 days
Pass Criteria: EE_*.bin file are generated.
*/

static int TEST_SBEE_0007(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{


  SEND("SBEE SBEEUSRCFG 0 0 1 2 6 4 65535");/*2 - Predicted Eph, 6 - (1 days prediction)*/
  SEND("SBEE SBEESERVCFG {009e7bc0-1d39-11dc-b4f8-806d6172696f} ST-Ericsson_PGPS_Demo 0 18 test1.gpstream.net");
  SEND("SBEE SBEESTART 0");
  sleep(60);

  SEND("SBEE SBEESTOP");

  TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, "LBSSTELP : TEST_SBEE_30_1 Finished");

  return TSTENG_RESULT_OK;

}

/*
Test Description: Start Prediction and check predcition file are generated.
        SeeddownloadOpt: Never
        Boradcast/Prediction: Prediction
        Prediction                : 2 days
Pass Criteria: EE_*.bin file are generated.
*/

static int TEST_SBEE_0008(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{


  SEND("SBEE SBEEUSRCFG 0 0 1 2 12 4 65535");/*2 - Predicted Eph, 12 - (2 days prediction)*/
  SEND("SBEE SBEESERVCFG {009e7bc0-1d39-11dc-b4f8-806d6172696f} ST-Ericsson_PGPS_Demo 0 18 test1.gpstream.net");
  SEND("SBEE SBEESTART 0");
  sleep(60);

  SEND("SBEE SBEESTOP");

  TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, "LBSSTELP : TEST_SBEE_30_2 Finished");

  return TSTENG_RESULT_OK;

}

/*
Test Description: Start Prediction and check predcition file are generated.
        SeeddownloadOpt: Never
        Boradcast/Prediction: Prediction
        Prediction                : 3 days
Pass Criteria: EE_*.bin file are generated.
*/

static int TEST_SBEE_0009(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{


  SEND("SBEE SBEEUSRCFG 0 0 1 2 18 4 65535");/*2 - Predicted Eph, 18 - (3 days prediction)*/
  SEND("SBEE SBEESERVCFG {009e7bc0-1d39-11dc-b4f8-806d6172696f} ST-Ericsson_PGPS_Demo 0 18 test1.gpstream.net");
  SEND("SBEE SBEESTART 0");
  sleep(60);

  SEND("SBEE SBEESTOP");

  TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, "LBSSTELP : TEST_SBEE_30_3 Finished");

  return TSTENG_RESULT_OK;

}

/*
Test Description: Start Prediction and check predcition file are generated.
        SeeddownloadOpt: Never
        Boradcast/Prediction: Prediction
        Prediction                : 5 days
Pass Criteria: EE_*.bin file are generated.
*/

static int TEST_SBEE_0010(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{


  SEND("SBEE SBEEUSRCFG 0 0 1 2 30 4 65535");/*2 - Predicted Eph, 30 - (5 days prediction)*/
  SEND("SBEE SBEESERVCFG {009e7bc0-1d39-11dc-b4f8-806d6172696f} ST-Ericsson_PGPS_Demo 0 18 test1.gpstream.net");
  SEND("SBEE SBEESTART 0");
  sleep(60);

  SEND("SBEE SBEESTOP");

  TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, "LBSSTELP : TEST_SBEE_30_5 Finished");

  return TSTENG_RESULT_OK;

}

/*
Test Description: Start Prediction and check predcition file are generated.
        SeeddownloadOpt: Never
        Boradcast/Prediction: Prediction
        Prediction                : 7 days
Pass Criteria: EE_*.bin file are generated.
*/

static int TEST_SBEE_0011(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{


  SEND("SBEE SBEEUSRCFG 0 0 1 2 42 4 65535"); /*2 - Predicted Eph, 42 - (7 days prediction)*/
  SEND("SBEE SBEESERVCFG {009e7bc0-1d39-11dc-b4f8-806d6172696f} ST-Ericsson_PGPS_Demo 0 18 test1.gpstream.net");
  SEND("SBEE SBEESTART 0");
  sleep(60);

  SEND("SBEE SBEESTOP");

  TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, "LBSSTELP : TEST_SBEE_30_7 Finished");

  return TSTENG_RESULT_OK;

}

/*
Test Description: Start Prediction and check predcition file are generated.
        SeeddownloadOpt: Never
        Boradcast/Prediction: Prediction
        Prediction                : 4 hr
Pass Criteria: EE_*.bin file are generated.
*/

static int TEST_SBEE_0012(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{


  SEND("SBEE SBEEUSRCFG 0 0 1 2 1 4 65535");/*2 - Predicted Eph, 1 - (4 hr prediction)*/
  SEND("SBEE SBEESERVCFG {009e7bc0-1d39-11dc-b4f8-806d6172696f} ST-Ericsson_PGPS_Demo 0 18 test1.gpstream.net");
  SEND("SBEE SBEESTART 0");
  sleep(180);
  SEND("NAF LBSTID0000");

  SEND("SBEE SBEESTOP");

  TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, "LBSSTELP : TEST_SBEE_34 Finished");

  return TSTENG_RESULT_OK;

}


/*
Test Description: Start Prediction and check predcition file are generated.
        SeeddownloadOpt: Never
        Boradcast/Prediction: Prediction
        Prediction                : 1 days
Pass Criteria: EE_*.bin file are generated.
*/

static int TEST_SBEE_0013(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{


  SEND("SBEE SBEEUSRCFG 0 0 1 2 6 4 65535"); /*2 - Predicted Eph, 6 - (1 days prediction)*/
  SEND("SBEE SBEESERVCFG {009e7bc0-1d39-11dc-b4f8-806d6172696f} ST-Ericsson_PGPS_Demo 0 18 test1.gpstream.net");
  SEND("SBEE SBEESTART 0");
  sleep(180);
  SEND("NAF LBSTID0000");

  SEND("SBEE SBEESTOP");

  TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, "LBSSTELP : TEST_SBEE_34 Finished");

  return TSTENG_RESULT_OK;

}






