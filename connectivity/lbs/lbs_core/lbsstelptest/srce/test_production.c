/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) ST-Ericsson SA 2009. All rights reserved.
 *  This code is ST-Ericsson proprietary and confidential.
 *  Any use of the code for whatever purpose is subject to
 *  specific written permission of ST-Ericsson.
 *
 *****************************************************************************/

#define LOG_TAG "LBSSTELP"


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

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

#if (X86_64_UBUNTU_FTR || AGPS_LINUX_BARE_FTR)
#define ALOGD printf
#endif

//t_treq_ProductionTestReqs ProductionTestReq;

int TREQ_TC_result = TSTENG_RESULT_OK;

LBSSTELP_reporting_criteria_t rc;

static int  TSTENG_check_info_cb(void);
static void TREQ_navigation_data_cb(t_gpsClient_NavData * location);
static void TREQ_nmea_data_cb(t_gpsClient_NmeaData *nmea);

static  t_gpsClient_Callbacks TREQ_callbacks = {
    .nav_cb = TREQ_navigation_data_cb,
    .nmea_cb = TREQ_nmea_data_cb,
};

#define TSTENG_BEGIN_STEP(handle, str) \
    ALOGD("LBSSTEP %s STARTED \n", str);\
if(TSTENG_check_info_cb() == TSTENG_RESULT_FAIL)\
    {\
      TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "FAILED to START");\
      return TSTENG_RESULT_FAIL;\
}

#define TSTENG_END_STEP(handle, str) \
    ALOGD("LBSSTEP %s END \n", str);\



static int TEST_START_SPEC_ANALYZER(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_STOP_SPEC_ANALYZER(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);


TSTENG_TEST_TABLE_BEGIN(TREQ)

/*Test Case   */
    TSTENG_TEST_TABLE_CMD("LBSSTARTSAT", TEST_START_SPEC_ANALYZER,          "LBSSTELP : Start Spectrum Analyzer test")
    TSTENG_TEST_TABLE_CMD("LBSSTOPSAT",  TEST_STOP_SPEC_ANALYZER,          "LBSSTELP : Stop Spectrum Analyzer test")

TSTENG_TEST_TABLE_END

static void TREQ_navigation_data_cb(t_gpsClient_NavData * location)
{
    location=location;/*Parameter not used.This is to avoid warning for th known fact*/
    ALOGD("LBSSTELP : TREQ_navigation_data_cb\n");

}

static void TREQ_nmea_data_cb(t_gpsClient_NmeaData *nmea)
{
    
    nmea=nmea;/*Parameter not used.This is to avoid warning for th known fact*/
    ALOGD("LBSSTELP : TREQ_nmea_data_cb\n");

}


static int TSTENG_check_info_cb()
{
  return TREQ_TC_result;
}


static int TEST_START_SPEC_ANALYZER(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TSTENG_BEGIN_STEP(handle, __func__);

    TREQ_TC_result = TSTENG_RESULT_OK;
    int SpectrumTestType = 0;

    t_gpsClient_TestRequest gpsClient_TestReq;
    memset(&gpsClient_TestReq, 0, sizeof(gpsClient_TestReq));

    ALOGD("1 LBSSTELP : args_found = %d", args_found);

    // Commands to READ PGA/AGC Values & Reset RF have only 1 parameter
    if(args_found < 1)
    {
       ALOGD("LBSSTELP : ERR Start Spectrum Analyzer test  - Expected 1 parameter");
       TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "LBSSTELP : Error in test Expected 1 parameter");
       return TSTENG_RESULT_FAIL;
    }

    gpsClient_Init( (t_gpsClient_Callbacks *) &TREQ_callbacks );

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, "LBSSTELP : Start Spectrum Analyzer Test\n");

    gpsClient_TestReq.v_TestReqMask = GPSCLIENT_TREQ_SPECANALYZ;
    ALOGD("LBSSTELP : TEST_START_SPEC_ANALYZER TestMask %d", gpsClient_TestReq.v_TestReqMask);

    SpectrumTestType = atoi(cmd_buf + arg_index[0]);
    gpsClient_TestReq.v_SpectrumTestType = SpectrumTestType;
    ALOGD("LBSSTELP : TEST_START_SPEC_ANALYZER SpectrumTestType %d", gpsClient_TestReq.v_SpectrumTestType);

    if((gpsClient_TestReq.v_SpectrumTestType == GPSCLIENT_GPS_WIDEBAND_SCAN) ||
        (gpsClient_TestReq.v_SpectrumTestType == GPSCLIENT_GPS_NARROWBAND_SCAN) ||
        (gpsClient_TestReq.v_SpectrumTestType == GPSCLIENT_GLONASS_WIDEBAND_SCAN) ||
        (gpsClient_TestReq.v_SpectrumTestType == GPSCLIENT_GLONASS_NARROWBAND_SCAN))
    {
        strncpy(gpsClient_TestReq.v_InputSigFreq, cmd_buf + arg_index[1], GPSCLIENT_MAX_TREQ_FREQ_LEN);
    }
    else if(gpsClient_TestReq.v_SpectrumTestType == GPSCLIENT_SET_PGA_AGC_VALUE)
    {
        int index = 1;
        while(index != args_found)
        {
            strcat(gpsClient_TestReq.v_PgaValues, cmd_buf + arg_index[index]);
            strcat(gpsClient_TestReq.v_PgaValues, " ");
            ALOGD("index=%d",index);
            index++;
        }

    }
    else
        {ALOGD("LBSSTELP : TEST_START_SPEC_ANALYZER Read values or RESET RF test. ");}



    ALOGD("LBSSTELP : Start Spectrum Analyzer test Mask %d, SpectrumTestType %d, Frequency %s PGA value %s",\
        gpsClient_TestReq.v_TestReqMask, gpsClient_TestReq.v_SpectrumTestType, gpsClient_TestReq.v_InputSigFreq, gpsClient_TestReq.v_PgaValues);

    gpsClient_ProductionTestStartReq(gpsClient_TestReq);

    TSTENG_END_STEP(handle, __func__);

    return TREQ_TC_result;
}

static int TEST_STOP_SPEC_ANALYZER(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    TSTENG_BEGIN_STEP(handle, __func__);

    TREQ_TC_result = TSTENG_RESULT_OK;
    t_gpsClient_TestStop v_TestStop;
    
    cmd_buf=cmd_buf;/*Parameter not used.This is to avoid warning for th known fact*/
    arg_index=arg_index; /*Parameter not used.This is to avoid warning for th known fact*/

    if(args_found != 0)
    {
       ALOGD("LBSSTELP : ERR Stop Spectrum Analyzer test  - Expected No parameter");
       TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "LBSSTELP : Error in test Expected No parameter");
       return TSTENG_RESULT_FAIL;
    }

    gpsClient_Init( (t_gpsClient_Callbacks *) &TREQ_callbacks );

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                                "LBSSTELP : Stop Spectrum Analyzer Test\n");

    v_TestStop.v_TestMask = GPSCLIENT_TREQ_SPECANALYZ;
    v_TestStop.v_State = 1;

    gpsClient_ProductionTestStopReq(v_TestStop);
    TSTENG_END_STEP(handle, __func__);

    gpsClient_Stop();

    return TREQ_TC_result;
}



