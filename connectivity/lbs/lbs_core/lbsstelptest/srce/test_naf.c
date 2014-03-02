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
#include <strings.h>

#include "math.h"
#include "agpsosa.h"
#include "gpsclient.h"
#include "EEClient.h"
#include "stepl_utility.h"
#include "test_engine_int.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "LBSSTELP"

#define MS_IN_SEC (1000)

static LBSSTELP_reporting_criteria_t TEST_NAF_default_rc = {
  .accuracy_vertical_m = 100,
  .accuracy_horizontal_m = 100,
  .reporting_rate = 1 * MS_IN_SEC,
  .status_rate_ms = 1 * MS_IN_SEC,
  .no_count_periodic = 50,
  .configMaskValid = 1,
  .configMask =1,
  .config2Valid =1,
  .sensMod = 1,
  .sensTTFF = 1,
  .powerPref = 1,
  .coldStart = 1,
  .aidingDataType = 255,
  .mt_testcases = 0,
  .time_to_first_fix_ms = 200000,
  .age_limit_ms = 100000,
  .IsFixAvailable = FALSE,
  .errorStatus = GPSCLIENT_NO_ERROR,

};

static t_stelp_PlatformConfiguration TEST_NAF_DefaultPlatformConfig_rc ={
    .v_Config1MaskValid = 65535,
    .v_Config1Mask =1,
    .v_Config2Mask =7,
    .v_MsaMandatoryMask = 192,    /**<This field is a bit mask for the Mandatory Assistance data in case of MSA*/
    .v_MsbMandatoryMask = 104,    /**<This field is a bit mask for the Mandatory Assistance data in case of MSB*/
    .v_SleepTimerDuration = 60, /**<This field is used to specify the Sleep timer duration of GPS*/
    .v_ComaTimerDuration = 5,   /**<This field is used to specify the Coma timer duration of GPS*/
    .v_DeepComaDuration = 6*24*60*60,/**<This field is used to specify the DeepComa timer duration of GPS*/
};

t_stelp_PlatformConfiguration PlatformConfig;


static t_OsaSemHandle NAFSM_TEST_END_CB;
static int TEST_NAF_periodic_count = 0;
static int periodic_count = 0;
static int flage = 0;
static int notification_count = 0;
static int TEST_NAF_gen_stop_service(TSTENG_handle_t handle);
static int TEST_Mobile_information(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_NAF_gen_location_forward(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_NAF_gen_location_Retrieve(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_NAF_gen_fence_geo_area(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_NAF_gen_Application_ID(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);

static int TEST_NAF_gen_cfg_agps(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_NAF_gen_cfg_mo_pref_uplane(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_NAF_gen_cfg_msa(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_NAF_gen_cfg_locationmode(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_NAF_gen_cfg_user(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_NAF_gen_cfg_platform(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);

static int TEST_NAF_gen_get_user_cfg(TSTENG_handle_t handle);
static int TEST_NAF_gen_get_platform_cfg(TSTENG_handle_t handle);


static int TEST_NAF_gen_cfg_delete_aid(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_NAF_gen_cfg_tls(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_NAF_gen_cfg_supl(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_NAF_gen_cfg_start_mode(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_NAF_gen_cfg_Freq_Aiding(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_NAF_gen_cfg_FTA(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);


static int TEST_NAF_gen_Logging_Configuration(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);


static int TEST_NAF_gen_Config_GPS_Glonass_SBAS(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);

static int TEST_NAF_gen_Config_EEGPS_EEGlonass_WIFIPOS(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_NAF_gen_Config_SAGPS_SAGlonass(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_NAF_gen_Config_SNTP_Ref_Time(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);




static int TEST_NAF_autonomous_periodic(TSTENG_handle_t handle);

static int TEST_NAF_autonomous_singleshot(TSTENG_handle_t handle);

static int TEST_NAF_autonomous_periodic_singleshot(TSTENG_handle_t handle);
static int TEST_NAF_autonomous_singleshot_periodic(TSTENG_handle_t handle);


static int TEST_NAF_user_configuration(TSTENG_handle_t handle);


static int TEST_NAF_autonomous_periodic_sleep_periodic(TSTENG_handle_t handle);
static int TEST_NAF_autonomous_periodic_sleep_singleshot(TSTENG_handle_t handle);

static int TEST_NAF_autonomous_periodic_outputtype0(TSTENG_handle_t handle);
static int TEST_NAF_autonomous_periodic_outputtype1(TSTENG_handle_t handle);

static int TEST_NAF_autonomous_singleshot_outputtype0(TSTENG_handle_t handle);
static int TEST_NAF_autonomous_singleshot_outputtype1(TSTENG_handle_t handle);



static int TEST_NAF_autonomous_periodic_hot(TSTENG_handle_t handle);
static int TEST_NAF_autonomous_periodic_cold(TSTENG_handle_t handle);
static int TEST_NAF_autonomous_periodic_warm(TSTENG_handle_t handle);

static int TEST_NAF_autonomous_singleshot_hot(TSTENG_handle_t handle);
static int TEST_NAF_autonomous_singleshot_cold(TSTENG_handle_t handle);
static int TEST_NAF_autonomous_singleshot_warm(TSTENG_handle_t handle);

static int TEST_NAF_MO1_user_configuration(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TEST_NAF_MO2_user_configuration(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);

static int TEST_NAF_MT1(TSTENG_handle_t handle);
static int TEST_NAF_MT2(TSTENG_handle_t handle);
static int TEST_NAF_MT3(TSTENG_handle_t handle);
static int TEST_NAF_MT4(TSTENG_handle_t handle);
static int TEST_NAF_MT5(TSTENG_handle_t handle);
static int TEST_NAF_MT6(TSTENG_handle_t handle);
static int TEST_NAF_MT7(TSTENG_handle_t handle);
static int TEST_NAF_MT8(TSTENG_handle_t handle);
static int TEST_NAF_MT9(TSTENG_handle_t handle);
static int TEST_NAF_MT10(TSTENG_handle_t handle);
static int TEST_NAF_MT11(TSTENG_handle_t handle);
LBSSTELP_reporting_criteria_t rc;


/*Begin  EEClient Development        194997 18 Jan 2012*/
static int TEST_NAF_EEClient_Init(TSTENG_handle_t handle);
static int TEST_NAF_EEClient_GetChipVer(TSTENG_handle_t handle);
static int TEST_NAF_EEClient_SetCapabilities(TSTENG_handle_t handle);
static int TEST_NAF_EEClient_SetEphemerisData(TSTENG_handle_t handle);
static int TEST_NAF_EEClient_EEColdStart(TSTENG_handle_t handle);


/*End  EEClient Development        194997 18 Jan 2012*/


/* Callbacks */
static void TEST_NAF_gen_navigation_data_cb(t_gpsClient_NavData * location);
static void TEST_NAF_gen_nmea_data_cb(t_gpsClient_NmeaData *nmea);

static void TEST_NAF_navigation_data_cb(t_gpsClient_NavData * location);
static void TEST_NAF_nmea_data_cb(t_gpsClient_NmeaData *nmea);
static void TEST_NAF_notification_cb(t_gpsClient_NotifyData *notification);
/* decoding fuction*/

/*Unused Code.Tobe deleted*/
#if 0
static void TEST_NAF_EncToAscii(t_gpsClient_NotifyData * ninotify );
#endif

char * TEST_NAF_UCS2(char *n_var, int vl_strlen);
char * TEST_NAF_UTF8(char *n_var, int vl_strlen);
static void NotifyData_Display(t_gpsClient_NotifyData * ninotify );
static void NavData_Display( t_gpsClient_NavData * Navigation );


char * Convert_UCS2_to_ASCII(char *n_var, int vl_strlen);
char * Convert_UTF8_to_ASCII(char *n_var, int vl_strlen);

static t_gpsClient_Callbacks TEST_NAF_gen_callbacks = {

    .nav_cb = TEST_NAF_gen_navigation_data_cb,
    .nmea_cb = TEST_NAF_gen_nmea_data_cb,

};






static t_gpsClient_Callbacks TEST_NAF_callbacks = {

    .nav_cb = TEST_NAF_navigation_data_cb,
    .nmea_cb = TEST_NAF_nmea_data_cb,

};

static  t_gpsClient_NiCallbacks TEST_NAF_Notification_callbacks = {

    .notify_cb = TEST_NAF_notification_cb,

};
/*Begin  EEClient Development        194997 18 Jan 2012*/

void TEST_EEClient_getEphemeris_callback(t_EEClient_EphemerisData *GetEphemeris);
void TEST_EEClient_getRefLocation_callback(t_EEClient_RefLocationData *GetRefLocationData);
void TEST_EEClient_getBroadcastEphemeris_callback(e_EEClient_Status Status);
void TEST_EEClient_setBroadcastEphemeris_callback(t_EEClient_NavDataBCE *BCEData);
void TEST_EEClient_getGpsTime_callback(e_EEClient_Status Status);
void TEST_EEClient_setGpsTime_callback(t_EEClient_GpsTime *SetGpsTime);
void TEST_EEClient_setChipVer_callback(t_EEClient_ChipVer *SetChipVer);




static  t_EEClient_Callbacks TEST_EE_callbacks = {
    .getephemerisdata_cb = TEST_EEClient_getEphemeris_callback,
    .getrefLocationdata_cb = TEST_EEClient_getRefLocation_callback,
    .getBroadcastephemerisdata_cb = TEST_EEClient_getBroadcastEphemeris_callback,
    .setBroadcastephemerisdata_cb = TEST_EEClient_setBroadcastEphemeris_callback,
    .getgpstime_cb = TEST_EEClient_getGpsTime_callback,
    .setgpstime_cb = TEST_EEClient_setGpsTime_callback,
    .setChipVer_cb = TEST_EEClient_setChipVer_callback,
};

/*End EEClient Development        194997 18 Jan 2012*/
/*************************************************************************************************************/
/*                                               Declarations for UNIT TEST Case BEGINS                                                                                  */
/*************************************************************************************************************/

/******************************************************/
/*      GLOBAL VARIABLES FOR UPDATING TEST RESULTS           */
/******************************************************/

static int NumTCPass;
static int NumTCFail;
static int TotalTc;
int vg_test_result = TSTENG_RESULT_OK;
static t_gpsClient_Callbacks *TEST_NAF_callbacks_auto = &TEST_NAF_gen_callbacks;

/******************************************************/
/*     FUNCTIONS FOR UPDATING TEST RESULTS                        */
/******************************************************/

static void TSTENG_begin(const char * str);
static void TSTENG_end(TSTENG_handle_t handle,const char * str);
static int  TSTENG_check_info_cb(void);

/*Unused code,to be deleted*/
#if 0
static void TSTENG_UpdatePassFail(TSTENG_handle_t handle,TSTENG_result_t result);
#endif

static void TSTENG_SummaryBegin(TSTENG_handle_t handle);
static void TSTENG_SummaryEnd(TSTENG_handle_t handle);


/*************************************************************************************/
/*     FUNCTIONS TO WAIT FOR THE FIX AND VERIFY FIX FOR REQUIRED PARMETERS                        */
/************************************************************************************/

static int TEST_NAF_Wait_For_Fix(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);

/*************************************************************************************/
/*     FUNCTIONS TO CHECK ERROR STATUS FROM GPSCLIENT API'S                          */
/************************************************************************************/

static int LBSSTATUS(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);

/*************************************************************************************/
/*     FUNCTIONS TO CHECK PSTE NMEA Messages                          */
/************************************************************************************/

static int LBSNMEA(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);


/*************************************************************************************/
/*     FUNCTIONS TO READ LIST OF TEST CASES FROM A FILE AND EXECUTE THEM                            */
/************************************************************************************/

static int TEST_NAF_Read_And_Execute_TestList(TSTENG_handle_t handle);


/*************************************************************************************/
/*     FUNCTIONS FOR MAKING  SINGLESHOT AND PERIODIC FIX REQUESTS                                     */
/************************************************************************************/

static int TEST_NAF_gen_periodic_fix_auto(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);

static int TEST_NAF_gen_single_shot_fix_auto(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);

/*************************************************************************************/
/*                                CALLBACK FUNCTIONS TO BE REGISTERED                                                  */
/************************************************************************************/

static void TEST_NAF_gen_navigation_data_cb_auto(t_gpsClient_NavData * location);
static void TEST_NAF_gen_nmea_data_cb_auto(t_gpsClient_NmeaData *nmea);

/*************************************************************************************/
/*                                INTIALIZING CALLBACKS                                                                            */
/************************************************************************************/


static t_gpsClient_Callbacks TEST_NAF_gen_callbacks_auto = {

    .nav_cb = TEST_NAF_gen_navigation_data_cb_auto,
    .nmea_cb = TEST_NAF_gen_nmea_data_cb_auto,

};

/*************************************************************************************/
/*                                MACRO DEFINATIONS                                                                                */
/************************************************************************************/

#define TEST_NAF_CONVERT_RMSVAL_TO_HORIZONTAL_ACC(x,y) sqrt((x * x) + (y * y))
#define TCBEGIN(handle, str) TSTENG_begin(str)
#define SUBEGIN(handle, str) TSTENG_SummaryBegin(handle)
#define SUBEND(handle, str) TSTENG_SummaryEnd(handle)
#define TCEND(handle, str) TSTENG_end(handle, str)

#define TSTENG_BEGIN_STEP(handle, str) \
    ALOGD("LBSSTEP %s STARTED \n", str);\
if(TSTENG_check_info_cb() == TSTENG_RESULT_FAIL)\
    {\
      TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "FAILED to START");\
      return TSTENG_RESULT_FAIL;\
}

#define TSTENG_END_STEP(handle, str) \
    ALOGD("LBSSTEP %s END \n", str);\


#define SEND(a) TSTENG_execute_command(-1,a, NULL) /* Used for Issuing stelp Commands (To Start a testcase)*/
#define RUN(a)  TSTENG_execute_command(-1,a, NULL)
#define RECV(a) TSTENG_execute_command(-1,a, NULL) /* Used for Receiving the error status from the command executed by SEND()*/
#ifdef LBS_LINUX_UBUNTU_FTR
#define ALOGD  printf
#endif


/*************************************************************************************/
/*                                                          TEST CASES BEGINS                                                        */
/************************************************************************************/
                /******************************************************/
                /*                               EVENT TRIGGER TC's                             */
                /******************************************************/
static int LBSNAFGEO001(TSTENG_handle_t handle);
static int LBSNAFGEO002(TSTENG_handle_t handle);
static int LBSNAFGEO003(TSTENG_handle_t handle);
static int LBSNAFGEO004(TSTENG_handle_t handle);
static int LBSNAFGEO005(TSTENG_handle_t handle);
static int LBSNAFGEO006(TSTENG_handle_t handle);
static int LBSNAFGEO007(TSTENG_handle_t handle);
static int LBSNAFGEO008(TSTENG_handle_t handle);
static int LBSNAFGEO009(TSTENG_handle_t handle);
static int LBSNAFGEO010(TSTENG_handle_t handle);
static int LBSNAFGEO011(TSTENG_handle_t handle);
static int LBSNAFGEO012(TSTENG_handle_t handle);
static int LBSNAFGEO013(TSTENG_handle_t handle);
static int LBSNAFGEO014(TSTENG_handle_t handle);
static int LBSNAFGEO015(TSTENG_handle_t handle);
static int LBSNAFGEO016(TSTENG_handle_t handle);
static int LBSNAFGEO017(TSTENG_handle_t handle);
static int LBSNAFGEO018(TSTENG_handle_t handle);
static int LBSNAFGEO019(TSTENG_handle_t handle);
static int LBSNAFGEO020(TSTENG_handle_t handle);

static int LBSNAFGEO021(TSTENG_handle_t handle);
static int LBSNAFGEO022(TSTENG_handle_t handle);
static int LBSNAFGEO023(TSTENG_handle_t handle);
static int LBSNAFGEO024(TSTENG_handle_t handle);
static int LBSNAFGEO025(TSTENG_handle_t handle);
static int LBSNAFGEO026(TSTENG_handle_t handle);
static int LBSNAFGEO027(TSTENG_handle_t handle);

/*Area Id Test Case*/
static int LBSNAFGEO028(TSTENG_handle_t handle);
static int LBSNAFGEO029(TSTENG_handle_t handle);
static int LBSNAFGEO030(TSTENG_handle_t handle);
static int LBSNAFGEO031(TSTENG_handle_t handle);


                /******************************************************/
                /*                               EVENT TRIGGER TC's ENDs*/
                /******************************************************/


static int LBSNAFINT0001(TSTENG_handle_t handle);
static int LBSNAFINT0002(TSTENG_handle_t handle);
static int LBSNAFINT0003(TSTENG_handle_t handle);
static int LBSNAFINT0004(TSTENG_handle_t handle);
static int LBSNAFINT0005(TSTENG_handle_t handle);
static int LBSNAFINT0010(TSTENG_handle_t handle);
static int LBSNAFINT0011(TSTENG_handle_t handle);
static int LBSNAFINT0012(TSTENG_handle_t handle);
static int LBSNAFINT0013(TSTENG_handle_t handle);
static int LBSNAFINT0014(TSTENG_handle_t handle);
static int LBSNAFINT0015(TSTENG_handle_t handle);
static int LBSNAFINT0016(TSTENG_handle_t handle);
static int LBSNAFINT0017(TSTENG_handle_t handle);
static int LBSNAFINT0018(TSTENG_handle_t handle);
static int LBSNAFINT0019(TSTENG_handle_t handle);
static int LBSNAFINT0021(TSTENG_handle_t handle);
static int LBSNAFINT0022(TSTENG_handle_t handle);
static int LBSNAFINT0023(TSTENG_handle_t handle);
static int LBSNAFINT0024(TSTENG_handle_t handle);
static int LBSNAFINT0025(TSTENG_handle_t handle);
static int LBSNAFINT0026(TSTENG_handle_t handle);
static int LBSNAFINT0027(TSTENG_handle_t handle);
static int LBSNAFINT0028(TSTENG_handle_t handle);
static int LBSNAFINT0029(TSTENG_handle_t handle);
static int LBSNAFINT0030(TSTENG_handle_t handle);
static int LBSNAFINT0031(TSTENG_handle_t handle);
static int LBSNAFINT0032(TSTENG_handle_t handle);
static int LBSNAFINT0033(TSTENG_handle_t handle);
static int LBSNAFINT0034(TSTENG_handle_t handle);
static int LBSNAFINT0035(TSTENG_handle_t handle);
static int LBSNAFINT0036(TSTENG_handle_t handle);
static int LBSNAFINT0038(TSTENG_handle_t handle);
static int LBSNAFINT0039(TSTENG_handle_t handle);
static int LBSNAFINT0040(TSTENG_handle_t handle);
static int LBSNAFINT0041(TSTENG_handle_t handle);
static int LBSNAFINT0042(TSTENG_handle_t handle);
static int LBSNAFINT0043(TSTENG_handle_t handle);

static int LBSNAFEE0001(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0002(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0003(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0004(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0005(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0006(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0007(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0008(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0009(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0010(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0011(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0012(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0013(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0014(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0015(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0021(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0022(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0023(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0024(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0025(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0026(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0027(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0028(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0031(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0032(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0033(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0034(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0035(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0036(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0037(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0038(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0039(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0041(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0042(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int LBSNAFEE0043(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);

/*************************************************************************************/
/*                                                          TEST CASES ENDS                                                           */
/************************************************************************************/
/*************************************************************************************************************/
/*                                               Declartions for UNIT TEST Case ENDS                                                                                  */
/*************************************************************************************************************/

/*************************************************************************************/
/*                                                          INTIALIZING NAF TABLE                                                  */
/************************************************************************************/


TSTENG_TEST_TABLE_BEGIN(NAF)
TSTENG_TEST_TABLE_CMD("LBSSTARTSF",   TEST_NAF_gen_single_shot_fix_auto, "LBSSTELP : Start Single Shot Fix with User define QOP")
TSTENG_TEST_TABLE_CMD("LBSSTARTPF",   TEST_NAF_gen_periodic_fix_auto,    "LBSSTELP : Start Periodic Fix")
TSTENG_TEST_TABLE_CMD("LBSSTOP",      TEST_NAF_gen_stop_service,         "LBSSTELP : Stop Service")
TSTENG_TEST_TABLE_CMD("LBSLOCFWD",    TEST_NAF_gen_location_forward,     "LBSSTELP : Request for Location Forward")
TSTENG_TEST_TABLE_CMD("LBSLOCRET",    TEST_NAF_gen_location_Retrieve,     "LBSSTELP : Request for Location Retrieve")
TSTENG_TEST_TABLE_CMD("LBSGEOFENCE",  TEST_NAF_gen_fence_geo_area,       "LBSSTELP : Request for Geofence")
TSTENG_TEST_TABLE_CMD("LBSAPPID",     TEST_NAF_gen_Application_ID,       "LBSSTELP : Registering Application ID")

TSTENG_TEST_TABLE_CMD("LBSCFGUSR",    TEST_NAF_gen_cfg_user,             "LBSSTELP : Set User Configuration")
TSTENG_TEST_TABLE_CMD("LBSCFGTEST",   TEST_NAF_gen_cfg_platform,         "LBSSTELP : Set Platform Configuration")

TSTENG_TEST_TABLE_CMD("LBSGETUSRCFG", TEST_NAF_gen_get_user_cfg,         "LBSSTELP : Geet User Configuration")
TSTENG_TEST_TABLE_CMD("LBSGETPLTCFG", TEST_NAF_gen_get_platform_cfg,     "LBSSTELP : Set Platform Configuration")


TSTENG_TEST_TABLE_CMD("LBSCFGAGPS", TEST_NAF_gen_cfg_agps,             "LBSSTELP : Set Enable or Disable AGPS")
TSTENG_TEST_TABLE_CMD("LBSCFGMOLR", TEST_NAF_gen_cfg_mo_pref_uplane, "LBSSTELP : Set MO Supl or MO CP")
TSTENG_TEST_TABLE_CMD("LBSCFGDEL", TEST_NAF_gen_cfg_delete_aid,       "LBSSTELP : Set Delete Aiding Data")
TSTENG_TEST_TABLE_CMD("LBSCFGCOLD", TEST_NAF_gen_cfg_start_mode,       "LBSSTELP : Set Start Mode Cold or Warm or Hot")
TSTENG_TEST_TABLE_CMD("LBSCFGMSA", TEST_NAF_gen_cfg_msa,              "LBSSTELP : Set Start Mode to MSA or MBS")
TSTENG_TEST_TABLE_CMD("LBSCFGLOCMODE", TEST_NAF_gen_cfg_locationmode,             "LBSSTELP : Set Semi Auto and Basic Self Location Mode")
TSTENG_TEST_TABLE_CMD("LBSCFGNOTLS", TEST_NAF_gen_cfg_tls,      "LBSSTELP : Set TLS/NonTls mode")
TSTENG_TEST_TABLE_CMD("LBSCFGSUPL", TEST_NAF_gen_cfg_supl,              "LBSSTELP : SUPL support 0-No Supl, 1-Suplv1, 2- Suplv2")
TSTENG_TEST_TABLE_CMD("LBSCFGFREQAID", TEST_NAF_gen_cfg_Freq_Aiding,  "LBSSTELP : Disable or Enable Frequency Aiding")
TSTENG_TEST_TABLE_CMD("LBSCFGFTA", TEST_NAF_gen_cfg_FTA,  "LBSSTELP : Disable or Enable FineTime Aiding")
TSTENG_TEST_TABLE_CMD("LBSSUMBIG", TSTENG_SummaryBegin,              "LBSSUMBIG : summary begin")
TSTENG_TEST_TABLE_CMD("LBSSUMEND", TSTENG_SummaryEnd,               "LBSSUMEND : Summary end")
TSTENG_TEST_TABLE_CMD("LBSMOBINF", TEST_Mobile_information,               "LBSMOBINF : Mobile Information")

TSTENG_TEST_TABLE_CMD("LBSCFGLOG", TEST_NAF_gen_Logging_Configuration, "LBSCFGLOG : Log Configuration")


TSTENG_TEST_TABLE_CMD("LBSCFGSAT", TEST_NAF_gen_Config_GPS_Glonass_SBAS,             "LBSSTELP : GPS GLONASS SBAS Enable/Disable Config")
TSTENG_TEST_TABLE_CMD("LBSCFGEEWIFI", TEST_NAF_gen_Config_EEGPS_EEGlonass_WIFIPOS,             "LBSSTELP : EE-GPS EE-GLONASS WIFI Positioning Enable/Disable Config")
TSTENG_TEST_TABLE_CMD("LBSCFGSELFASSIST", TEST_NAF_gen_Config_SAGPS_SAGlonass,             "LBSSTELP : SA GPS/GLONASS  Enable/Disable Config")
TSTENG_TEST_TABLE_CMD("LBSCFGSNTPTIME", TEST_NAF_gen_Config_SNTP_Ref_Time,             "LBSSTELP : SNTP Reference Time Enable/Disable Config")



TSTENG_TEST_TABLE_CMD("LBSTID0000", TEST_NAF_autonomous_periodic, "LBSSTELP : Start a autonomous periodic fix")
TSTENG_TEST_TABLE_CMD("LBSTID0001", TEST_NAF_autonomous_singleshot, "LBSSTELP : Start a autonomous singleshot fix")
TSTENG_TEST_TABLE_CMD("LBSTID0002", TEST_NAF_autonomous_periodic_singleshot, "LBSSTELP : Start a autonomous perodic fix , register singleshot fix")
TSTENG_TEST_TABLE_CMD("LBSTID0003", TEST_NAF_autonomous_singleshot_periodic, "LBSSTELP : Start a autonomous singleshot fix, register for perodic fix")
TSTENG_TEST_TABLE_CMD("LBSTID0004", TEST_NAF_user_configuration, "LBSSTELP : User Configuration")
TSTENG_TEST_TABLE_CMD("LBSTID0005", TEST_NAF_autonomous_periodic_sleep_periodic, "LBSSTELP : Start a autonomous perodic fix twice for sleep mode")
TSTENG_TEST_TABLE_CMD("LBSTID0006", TEST_NAF_autonomous_periodic_sleep_singleshot, "LBSSTELP : Start a autonomous perodic fix go to sleep mode and register in singleshot")
TSTENG_TEST_TABLE_CMD("LBSTID0007", TEST_NAF_autonomous_periodic_outputtype0, "LBSSTELP : Start a autonomous perodic fix with output type 0,All other parameters valid")
TSTENG_TEST_TABLE_CMD("LBSTID0008", TEST_NAF_autonomous_periodic_outputtype1, "LBSSTELP : Start a autonomous perodic fix with output type 1,All other parameters valid")
TSTENG_TEST_TABLE_CMD("LBSTID0009", TEST_NAF_autonomous_singleshot_outputtype0, "LBSSTELP : Start a autonomous singleshot fix with output type 0,All other parameters valid")
TSTENG_TEST_TABLE_CMD("LBSTID0010", TEST_NAF_autonomous_singleshot_outputtype1, "LBSSTELP : Start a autonomous singleshot fix with output type 1,All other parameters valid")
TSTENG_TEST_TABLE_CMD("LBSTID0011", TEST_NAF_autonomous_periodic_hot, "LBSSTELP : Start a autonomous perodic fix ,hot")
TSTENG_TEST_TABLE_CMD("LBSTID0012", TEST_NAF_autonomous_periodic_cold, "LBSSTELP : Start a autonomous perodic fix ,cold")
TSTENG_TEST_TABLE_CMD("LBSTID0013", TEST_NAF_autonomous_periodic_warm, "LBSSTELP : Start a autonomous perodic fix ,warm")
TSTENG_TEST_TABLE_CMD("LBSTID0014", TEST_NAF_autonomous_singleshot_hot, "LBSSTELP : Start a autonomous singleshot fix ,hot")
TSTENG_TEST_TABLE_CMD("LBSTID0015", TEST_NAF_autonomous_singleshot_cold, "LBSSTELP : Start a autonomous singleshot fix ,cold")
TSTENG_TEST_TABLE_CMD("LBSTID0016", TEST_NAF_autonomous_singleshot_warm, "LBSSTELP : Start a autonomous singleshot fix ,warm")


TSTENG_TEST_TABLE_CMD("LBSTID2000", TEST_NAF_MO1_user_configuration, "LBSSTELP : Start a Mobile Originated session 1")
TSTENG_TEST_TABLE_CMD("LBSTID2001", TEST_NAF_MO2_user_configuration, "LBSSTELP : Start a Mobile Originated session 2")


TSTENG_TEST_TABLE_CMD("LBSTID3000", TEST_NAF_MT1, "LBSSTELP : UE Assisted LCS MT-LR, Privacy Verifaction, Location Not Allowed if No Response")
TSTENG_TEST_TABLE_CMD("LBSTID3001", TEST_NAF_MT2, "LBSSTELP : UE Assisted LCS MT-LR, Privacy Verification, Location Allowed if No Response")
TSTENG_TEST_TABLE_CMD("LBSTID3002", TEST_NAF_MT3, "LBSSTELP : UE Assisted LCS MT-LR, Request for additional assistance data, Success")
TSTENG_TEST_TABLE_CMD("LBSTID3003", TEST_NAF_MT4, "LBSSTELP : UE Assisted LCS MT-LR, Success")
TSTENG_TEST_TABLE_CMD("LBSTID3004", TEST_NAF_MT5, "LBSSTELP : UE Assisted LCS MT-LR, Configuration Incomplete")
TSTENG_TEST_TABLE_CMD("LBSTID3005", TEST_NAF_MT6, "LBSSTELP : UE Based LCS MT-LR, Privacy Verifaction, Location Not Allowed if No Response")
TSTENG_TEST_TABLE_CMD("LBSTID3006", TEST_NAF_MT7, "LBSSTELP : UE Based LCS MT-LR, Privacy Verification, Location Allowed if No Response")
TSTENG_TEST_TABLE_CMD("LBSTID3007", TEST_NAF_MT8, "LBSSTELP : UE Based LCS MT-LR, Not Enough GPS Satellites")
TSTENG_TEST_TABLE_CMD("LBSTID3008", TEST_NAF_MT9, "LBSSTELP : UE Based LCS MT-LR, Request for additional assistance data, Success")
TSTENG_TEST_TABLE_CMD("LBSTID3009", TEST_NAF_MT10, "LBSSTELP : UE Based LCS MT-LR, Configuration Incomplete")
TSTENG_TEST_TABLE_CMD("LBSTID3010", TEST_NAF_MT11, "LBSSTELP : UE Based LCS MT-LR, Success")
/*************************************************************************************/
/*             FUCTIONS FOR NAF UNIT TEST IN NAF COMMAND TABLE BEGINS                  */
/************************************************************************************/

TSTENG_TEST_TABLE_CMD("LBSNAFGEO001", LBSNAFGEO001, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFGEO002", LBSNAFGEO002, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFGEO003", LBSNAFGEO003, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFGEO004", LBSNAFGEO004, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFGEO005", LBSNAFGEO005, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFGEO006", LBSNAFGEO006, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFGEO007", LBSNAFGEO007, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFGEO008", LBSNAFGEO008, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFGEO009", LBSNAFGEO009, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFGEO010", LBSNAFGEO010, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFGEO011", LBSNAFGEO011, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFGEO012", LBSNAFGEO012, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFGEO013", LBSNAFGEO013, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFGEO014", LBSNAFGEO014, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFGEO015", LBSNAFGEO015, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFGEO016", LBSNAFGEO016, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFGEO017", LBSNAFGEO017, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFGEO018", LBSNAFGEO018, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFGEO019", LBSNAFGEO019, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFGEO020", LBSNAFGEO020, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFGEO021", LBSNAFGEO021, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFGEO022", LBSNAFGEO022, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFGEO023", LBSNAFGEO023, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFGEO024", LBSNAFGEO024, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFGEO025", LBSNAFGEO025, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFGEO026", LBSNAFGEO026, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFGEO027", LBSNAFGEO027, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFGEO028", LBSNAFGEO028, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFGEO029", LBSNAFGEO029, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFGEO030", LBSNAFGEO030, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFGEO031", LBSNAFGEO031, "LBSSTELP : Start ananoums test")


/*************************************************************************************/
/*             FUCTIONS FOR NAF UNIT TEST IN NAF COMMAND TABLE BEGINS                  */
/************************************************************************************/



TSTENG_TEST_TABLE_CMD("LBSSTARTPFA", TEST_NAF_gen_periodic_fix_auto, "LBSSTELP : Start Periodic Fix")
TSTENG_TEST_TABLE_CMD("LBSSTARTSFA", TEST_NAF_gen_single_shot_fix_auto,"LBSSTELP : Start Single Shot Fix with User define QOP")
TSTENG_TEST_TABLE_CMD("LBSNAFTESTALL", TEST_NAF_Read_And_Execute_TestList, "LBSSTELP : Start executing Autonomous test case list")
TSTENG_TEST_TABLE_CMD("LBSPOS", TEST_NAF_Wait_For_Fix, "LBSSTELP : Wait for Autonomous fix")
TSTENG_TEST_TABLE_CMD("LBSSTATUS", LBSSTATUS, "LBSSTELP : Check LBS error status")

TSTENG_TEST_TABLE_CMD("LBSNMEA", LBSNMEA, "LBSSTELP : Check for PSTE NMEA Messages")


TSTENG_TEST_TABLE_CMD("LBSNAFINT0001", LBSNAFINT0001, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0002", LBSNAFINT0002, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0003", LBSNAFINT0003, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0004", LBSNAFINT0004, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0005", LBSNAFINT0005, "LBSSTELP : Start ananoums test")

TSTENG_TEST_TABLE_CMD("LBSNAFINT0010", LBSNAFINT0010, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0011", LBSNAFINT0011, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0012", LBSNAFINT0012, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0013", LBSNAFINT0013, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0014", LBSNAFINT0014, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0015", LBSNAFINT0015, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0016", LBSNAFINT0016, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0017", LBSNAFINT0017, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0018", LBSNAFINT0018, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0019", LBSNAFINT0019, "LBSSTELP : Start ananoums test")

TSTENG_TEST_TABLE_CMD("LBSNAFINT0021", LBSNAFINT0021, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0022", LBSNAFINT0022, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0023", LBSNAFINT0023, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0024", LBSNAFINT0024, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0025", LBSNAFINT0025, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0026", LBSNAFINT0026, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0027", LBSNAFINT0027, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0028", LBSNAFINT0028, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0029", LBSNAFINT0029, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0030", LBSNAFINT0030, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0031", LBSNAFINT0031, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0032", LBSNAFINT0032, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0033", LBSNAFINT0033, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0034", LBSNAFINT0034, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0035", LBSNAFINT0035, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0036", LBSNAFINT0036, "LBSSTELP : Start ananoums test")

TSTENG_TEST_TABLE_CMD("LBSTID038", LBSNAFINT0038, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0039", LBSNAFINT0039, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0040", LBSNAFINT0040, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0041", LBSNAFINT0041, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0042", LBSNAFINT0042, "LBSSTELP : Start ananoums test")
TSTENG_TEST_TABLE_CMD("LBSNAFINT0043", LBSNAFINT0043, "LBSSTELP : Start ananoums test")

TSTENG_TEST_TABLE_CMD("LBSNAFEE0001", LBSNAFEE0001, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0002", LBSNAFEE0002, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0003", LBSNAFEE0003, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0004", LBSNAFEE0004, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0005", LBSNAFEE0005, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0006", LBSNAFEE0006, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0007", LBSNAFEE0007, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0008", LBSNAFEE0008, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0009", LBSNAFEE0009, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0010", LBSNAFEE0010, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0011", LBSNAFEE0011, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0012", LBSNAFEE0012, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0013", LBSNAFEE0013, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0014", LBSNAFEE0014, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0015", LBSNAFEE0015, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0021", LBSNAFEE0021, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0022", LBSNAFEE0022, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0023", LBSNAFEE0023, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0024", LBSNAFEE0024, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0025", LBSNAFEE0025, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0026", LBSNAFEE0026, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0027", LBSNAFEE0027, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0028", LBSNAFEE0028, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0031", LBSNAFEE0031, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0032", LBSNAFEE0032, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0033", LBSNAFEE0033, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0034", LBSNAFEE0034, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0035", LBSNAFEE0035, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0036", LBSNAFEE0036, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0037", LBSNAFEE0037, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0038", LBSNAFEE0038, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0039", LBSNAFEE0039, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0041", LBSNAFEE0041, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0042", LBSNAFEE0042, "LBSSTELP : EE Tests")
TSTENG_TEST_TABLE_CMD("LBSNAFEE0043", LBSNAFEE0043, "LBSSTELP : EE Tests")
/*Begin  EEClient Development        194997 18 Jan 2012*/

TSTENG_TEST_TABLE_CMD("EECLIENTINIT",      TEST_NAF_EEClient_Init,         "LBSSTELP :EE Client Init")
TSTENG_TEST_TABLE_CMD("EECLIENTGETVERSION",      TEST_NAF_EEClient_GetChipVer,         "LBSSTELP : EE Client get version")
TSTENG_TEST_TABLE_CMD("EECLIENTSETCAPAB",      TEST_NAF_EEClient_SetCapabilities,         "LBSSTELP : EE Client set capabilities")
TSTENG_TEST_TABLE_CMD("EECLIENTSETEPHEMERIS",      TEST_NAF_EEClient_SetEphemerisData,         "LBSSTELP : EE Client set ephemeris")
TSTENG_TEST_TABLE_CMD("EECLIENTCOLD",      TEST_NAF_EEClient_EEColdStart,         "LBSSTELP : EE Client set ephemeris")



/*End  EEClient Development        194997 18 Jan 2012*/

/*************************************************************************************/
/*             ADDING ALL FUCTIONS FOR NAF UNIT TEST IN NAF COMMAND TABLE ENDS                      */
/************************************************************************************/







TSTENG_TEST_TABLE_END

/* Autonomous UNIT test case ID's : END*/


static void TSTENG_begin(const char * str)
{
    ALOGD("LBSNAF %s STARTED \n", str);

    vg_test_result = TSTENG_RESULT_OK;
    rc.IsFixAvailable = FALSE;
    rc.errorStatus = GPSCLIENT_NO_ERROR;
    TEST_NAF_callbacks_auto =  &TEST_NAF_gen_callbacks_auto;

}


static void TSTENG_SummaryEnd(TSTENG_handle_t handle)
{
   FILE  *filePtr;
   handle = handle; /*handle not used.To avoid Warning for known fact*/
   filePtr = fopen("/data/LbsNafTestResults.txt","a+");

   if(filePtr == NULL)
   {
      ALOGD("UNABLE TO OPEN RESULT FILE EXITING....");
      return;
   }

   TotalTc = NumTCFail + NumTCPass;
   fprintf(filePtr, "TEST SUITE ENDED\n");
   fprintf(filePtr, "PASS - %d  FAIL - %d  TOTAL - %d  PASSPercentage- %d  \n", NumTCPass, NumTCFail, TotalTc, (NumTCPass*100)/TotalTc);


   fclose(filePtr);

}


static void TSTENG_SummaryBegin(TSTENG_handle_t handle)
{
   handle = handle; /*handle not used.To avoid Warning for known fact*/
   ALOGD("LBSSTELP : TSTENG_SummaryBegin");
   NumTCPass = 0;
   NumTCFail = 0;
}


/*Unused code,to be deleted*/
#if 0
static void TSTENG_UpdatePassFail(TSTENG_handle_t handle,TSTENG_result_t result)
{
   handle = handle; /*handle not used.To avoid Warning for known fact*/
   if(result == TSTENG_RESULT_OK)
        NumTCPass++;
   else
        NumTCFail++;
}
#endif



static void TSTENG_end(TSTENG_handle_t handle,const char * str)
{
    ALOGD("LBSSTELP : TSTENG_end begin");
    FILE  *filePtr;

    filePtr = fopen("/data/LbsNafTestResults.txt","a");

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


static int TSTENG_check_info_cb()
{
  return vg_test_result;
}


/*
*register for Logging Configuration
*/
static int TEST_NAF_gen_Logging_Configuration(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    int ModuleType = atoi(cmd_buf + arg_index[0]);
    int LogType = atoi(cmd_buf + arg_index[1]);
    int LogOpt = atoi(cmd_buf + arg_index[2]);
    int LogFileSize = atoi(cmd_buf + arg_index[3]);
    args_found = args_found; /*Parameter not used.This is to avoid warning for th known fact*/
    gpsClient_Init( (t_gpsClient_Callbacks *) &TEST_NAF_callbacks );

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSCFGLOG : Logging Configuration\n");

    gpsClient_Set_LoggingConfiguration(ModuleType,LogType,LogOpt,LogFileSize);


    return TSTENG_RESULT_OK;

}
static int TEST_Mobile_information(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)

{

    char v_imsi[64];
    int v_mcc = atoi(cmd_buf + arg_index[1]);
    int v_mnc = atoi(cmd_buf + arg_index[2]);
    int v_cellId = atoi(cmd_buf + arg_index[3]);
    int v_Lac = atoi(cmd_buf + arg_index[4]);
    int v_celltype = atoi(cmd_buf + arg_index[5]);
    args_found = args_found; /*Parameter not used.This is to avoid warning for th known fact*/
    strcpy(v_imsi,(cmd_buf + arg_index[6]));

    ALOGD("LBSNAF : TEST_Mobile_info enter\n");

    gpsClient_Mobile_Info(v_mcc,v_mnc,v_cellId,v_Lac,v_celltype,v_imsi,NULL);

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, " gpsClient_Mobile_Info send");

    return TSTENG_RESULT_OK;


}


static int TEST_NAF_gen_fence_geo_area(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    TSTENG_BEGIN_STEP(handle, __func__);

    int indeex;

    int paramno=11;
    int outputType = 1;

    args_found = args_found; /*Parameter not used.This is to avoid warning for th known fact*/

    TEST_NAF_periodic_count = 0;   /*if not periodic*/

    periodic_count = 0;

    rc = TEST_NAF_default_rc;

    rc.NMEA = atoi(cmd_buf + arg_index[0]);

    rc.no_count_periodic = atoi(cmd_buf + arg_index[1]);

    if (rc.no_count_periodic-1)
       TEST_NAF_periodic_count = 1;   /*if  periodic*/

    gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_callbacks );

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,"LBSSTELP : Data callbacks successfully registered\n");

    rc.positioningParams.v_NumberOfFixes = rc.no_count_periodic;

    if(rc.no_count_periodic > 1)
     {
      rc.positioningParams.v_StartTime   = atoi(cmd_buf + arg_index[2]);
      rc.positioningParams.v_StopTime    = atoi(cmd_buf + arg_index[3]);
      rc.positioningParams.v_FixInterval = atoi(cmd_buf + arg_index[4]);
     }

    else
     {
      rc.positioningParams.v_Timeout  = atoi(cmd_buf + arg_index[2]);
      rc.positioningParams.v_AgeLimit = atoi(cmd_buf + arg_index[3]);

     }

    rc.positioningParams.v_HorizAccuracy = atoi(cmd_buf + arg_index[5]);
    rc.positioningParams.v_VertAccuracy  = atoi(cmd_buf + arg_index[6]);

    rc.speedThreshold = atoi(cmd_buf + arg_index[7]);
    rc.areaEventType  = atoi(cmd_buf + arg_index[8]);

    rc.targetArea.v_ShapeType = atoi(cmd_buf + arg_index[9]);

    if(rc.targetArea.v_ShapeType == 0)         /*circle*/

     {

        rc.targetArea.v_CircularRadius    = atoi(cmd_buf + arg_index[10]);

        *rc.targetArea.v_LatSign          = atoi(cmd_buf + arg_index[12]);
        *rc.targetArea.v_Latitude         = atoi(cmd_buf + arg_index[11]);
        *rc.targetArea.v_Longitude        = atoi(cmd_buf + arg_index[13]);


        rc.targetArea.v_CircularRadiusMax = atoi(cmd_buf + arg_index[14]);


        rc.targetArea.v_CircularRadiusMin = atoi(cmd_buf + arg_index[15]);

      }

     if(rc.targetArea.v_ShapeType == 1)       /*ellipse*/

       {

        rc.targetArea.v_EllipSemiMinorAngle = atoi(cmd_buf + arg_index[10]);
        rc.targetArea.v_EllipSemiMajor      = atoi(cmd_buf + arg_index[11]);
        rc.targetArea.v_EllipSemiMinor      = atoi(cmd_buf + arg_index[12]);
        *rc.targetArea.v_Latitude           = atoi(cmd_buf + arg_index[13]);
        *rc.targetArea.v_LatSign            = atoi(cmd_buf + arg_index[14]);
        *rc.targetArea.v_Longitude          = atoi(cmd_buf + arg_index[15]);
        rc.targetArea.v_EllipSemiMajorMax   = atoi(cmd_buf + arg_index[16]);
        rc.targetArea.v_EllipSemiMajorMin   = atoi(cmd_buf + arg_index[17]);
        rc.targetArea.v_EllipSemiMinorMax   = atoi(cmd_buf + arg_index[18]);
        rc.targetArea.v_EllipSemiMinorMin   = atoi(cmd_buf + arg_index[19]);

        }


     if(rc.targetArea.v_ShapeType == 2)      /*polygon*/

      {

        rc.targetArea.v_NumberOfPoints= atoi(cmd_buf + arg_index[10]);


        for(indeex=0;indeex<rc.targetArea.v_NumberOfPoints;indeex++)
        {
             rc.targetArea.v_Latitude[indeex]  = atoi(cmd_buf + arg_index[paramno]);
             paramno=paramno+1;

        }


        for(indeex=0;indeex<rc.targetArea.v_NumberOfPoints;indeex++)
        {
             rc.targetArea.v_LatSign[indeex]   = atoi(cmd_buf + arg_index[paramno]);
             paramno=paramno+1;
        }


        for(indeex=0;indeex<rc.targetArea.v_NumberOfPoints;indeex++)
        {
              rc.targetArea.v_Longitude[indeex] = atoi(cmd_buf + arg_index[paramno]);
              paramno=paramno+1;
        }


        rc.targetArea.v_PolygonHysteresis= atoi(cmd_buf + arg_index[paramno]);


       }

    gpsClient_ServiceStart();

    rc.errorStatus=gpsClient_FenceGeographicalArea(outputType,rc.NMEA,rc.positioningParams,&rc.targetArea,rc.speedThreshold,rc.areaEventType);

    (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
    (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);

    return TSTENG_RESULT_OK;

}
/*
*register for forwarding location to third party
*/

static int TEST_NAF_gen_location_forward(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    if(args_found < 8)
    {
          ALOGD("LBSSTELP : ERR Location Forward - Expected 6 parameter");
       TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "LBSSTELP : Error in test arguments - Test requires 6 parameters");
       return TSTENG_RESULT_FAIL;
    }

    TEST_NAF_periodic_count = 0;

    rc = TEST_NAF_default_rc;

    gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_gen_callbacks );

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Data callbacks successfully registered\n");


    rc.serviceType = atoi(cmd_buf + arg_index[0]);
    rc.time_to_first_fix_ms = atoi(cmd_buf + arg_index[1]) * 1000;
    rc.accuracy_horizontal_m = atoi(cmd_buf + arg_index[2]);
    rc.accuracy_vertical_m = atoi(cmd_buf + arg_index[3]);
    rc.age_limit_ms = atoi(cmd_buf + arg_index[4]) * 1000;
    ALOGD("LBSSTELP : QOP Argument found - %d Arguments - TTFF %d HA %d VA %d Age %d \n",args_found,atoi(cmd_buf + arg_index[1]),atoi(cmd_buf + arg_index[2]),atoi(cmd_buf + arg_index[3]),atoi(cmd_buf + arg_index[4]));
    rc.clientType = atoi(cmd_buf + arg_index[5]);
    rc.clientTonNpi = atoi(cmd_buf + arg_index[6]);
    strcpy(rc.clientNumber,(cmd_buf + arg_index[7]));
    ALOGD("LBSSTELP : QOP Argument found - %d Arguments - ClientType %d Npi %d Num %s \n",args_found,atoi(cmd_buf + arg_index[5]),atoi(cmd_buf + arg_index[6]),(cmd_buf + arg_index[7]));

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, "LBSSTELP : Applying command line parameters");


    gpsClient_ServiceStart();

    t_gpsClient_Qop qop;
    qop.vp_AgeLimit = rc.age_limit_ms;
    qop.vp_HorizAccuracy = rc.accuracy_horizontal_m;
    qop.vp_Timeout = rc.time_to_first_fix_ms;
    qop.vp_VertAccuracy = rc.accuracy_vertical_m;
    t_gpsClient_ThirdClientInfo thirdclient;
    memset (&thirdclient, 0, sizeof (thirdclient));

    switch (rc.clientType)
    {
      case 1:
        thirdclient.v_ClientIdTonNpi = rc.clientTonNpi;
        thirdclient.v_ClientIdConfig = 1;
        thirdclient.v_ClientExtIdLen = strlen (rc.clientNumber);
        if(thirdclient.v_ClientExtIdLen >= 40)
            thirdclient.v_ClientExtIdLen = 40 -1;
        strcpy((char*)thirdclient.a_ClientExtId, rc.clientNumber);        /**< Requestor Id string */


          break;

      case 2:
          thirdclient.v_MlcNumTonNpi = rc.clientTonNpi;
          thirdclient.v_MlcNumConfig = 1;
          thirdclient.v_ClientExtIdLen = strlen (rc.clientNumber);
          if(thirdclient.v_MlcNumberLen >= 40)
              thirdclient.v_MlcNumberLen = 40 -1;
          strcpy((char*)thirdclient.a_MlcNumber, rc.clientNumber);         /**< Requestor Id string */

          break;
      case 0:
          /*No need to add anything*/
      default:
      break;
    }
    thirdclient.v_ThirdPartyInfo.v_ThirdPartyId = atoi(cmd_buf + arg_index[6]);

    strcpy((char*)thirdclient.v_ThirdPartyInfo.a_ThirdPartyName , (cmd_buf + arg_index[7]));

    ALOGD("LBSSTELP : Third Party Info Id - %d Name %s \n",thirdclient.v_ThirdPartyInfo.v_ThirdPartyId ,thirdclient.v_ThirdPartyInfo.a_ThirdPartyName);

    gpsClient_LocationForward(&qop, &thirdclient,rc.serviceType );

    return TSTENG_RESULT_OK;

}

static int TEST_NAF_gen_location_Retrieve(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    if(args_found < 8)
    {
       ALOGD("LBSSTELP : ERR Location Forward - Expected 8 parameter");
       TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "LBSSTELP : Error in test arguments - Test requires 8 parameters");
       return TSTENG_RESULT_FAIL;
    }

    TEST_NAF_periodic_count = 0;

    rc = TEST_NAF_default_rc;

    gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_gen_callbacks );

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Data callbacks successfully registered\n");


    rc.serviceType = atoi(cmd_buf + arg_index[0]);
    rc.time_to_first_fix_ms = atoi(cmd_buf + arg_index[1]) * 1000;
    rc.accuracy_horizontal_m = atoi(cmd_buf + arg_index[2]);
    rc.accuracy_vertical_m = atoi(cmd_buf + arg_index[3]);
    rc.age_limit_ms = atoi(cmd_buf + arg_index[4]) * 1000;
    ALOGD("LBSSTELP : QOP Argument found - %d Arguments - TTFF %d HA %d VA %d Age %d \n",args_found,atoi(cmd_buf + arg_index[1]),atoi(cmd_buf + arg_index[2]),atoi(cmd_buf + arg_index[3]),atoi(cmd_buf + arg_index[4]));
    rc.clientType = atoi(cmd_buf + arg_index[5]);
    rc.clientTonNpi = atoi(cmd_buf + arg_index[6]);
    strcpy(rc.clientNumber,(cmd_buf + arg_index[7]));
    ALOGD("LBSSTELP : QOP Argument found - %d Arguments - ClientType %d Npi %d Num %s  \n",args_found,atoi(cmd_buf + arg_index[5]),atoi(cmd_buf + arg_index[6]),(cmd_buf + arg_index[7]));

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, "LBSSTELP : Applying command line parameters");


    gpsClient_ServiceStart();

    t_gpsClient_Qop qop;
    qop.vp_AgeLimit = rc.age_limit_ms;
    qop.vp_HorizAccuracy = rc.accuracy_horizontal_m;
    qop.vp_Timeout = rc.time_to_first_fix_ms;
    qop.vp_VertAccuracy = rc.accuracy_vertical_m;
    t_gpsClient_ThirdClientInfo thirdclient;
    memset (&thirdclient, 0, sizeof (thirdclient));

    switch (rc.clientType)
    {
      case 1:
        thirdclient.v_ClientIdTonNpi = rc.clientTonNpi;
        thirdclient.v_ClientIdConfig = 1;
        thirdclient.v_ClientExtIdLen = strlen (rc.clientNumber);
        if(thirdclient.v_ClientExtIdLen >= 40)
            thirdclient.v_ClientExtIdLen = 40 -1;
        strcpy((char*)thirdclient.a_ClientExtId, rc.clientNumber);        /**< Requestor Id string */


          break;

      case 2:
          thirdclient.v_MlcNumTonNpi = rc.clientTonNpi;
          thirdclient.v_MlcNumConfig = 1;
          thirdclient.v_ClientExtIdLen = strlen (rc.clientNumber);
          if(thirdclient.v_MlcNumberLen >= 40)
              thirdclient.v_MlcNumberLen = 40 -1;
          strcpy((char*)thirdclient.a_MlcNumber, rc.clientNumber);         /**< Requestor Id string */

          break;
      case 0:
          /*No need to add anything*/
      default:
      break;
    }
    thirdclient.v_ThirdPartyInfo.v_ThirdPartyId = atoi(cmd_buf + arg_index[6]);

    strcpy((char*)thirdclient.v_ThirdPartyInfo.a_ThirdPartyName , (cmd_buf + arg_index[7]));

    ALOGD("LBSSTELP : Third Party Info Id - %d Name %s \n",thirdclient.v_ThirdPartyInfo.v_ThirdPartyId ,thirdclient.v_ThirdPartyInfo.a_ThirdPartyName);

    gpsClient_LocationRetrieval(&qop, &thirdclient );

    return TSTENG_RESULT_OK;

}

/*
*register for Application ID Info
*/
static int TEST_NAF_gen_Application_ID(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    int cmd=0;
    t_gpsClient_ApplicationIDInfo    application_id_info;

    ALOGD("LBSSTELP : Entering the Application ID - from STELP");

    if(args_found > 1)
    {
       ALOGD("LBSSTELP : ERR Application ID - Expected only 1 parameter");
       TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "LBSSTELP : Error in test arguments - Test requires 1 parameter");
       return TSTENG_RESULT_FAIL;
    }

    TEST_NAF_periodic_count = 0;

    rc = TEST_NAF_default_rc;

    gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_gen_callbacks );

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Data callbacks successfully registered\n");


    cmd = atoi(cmd_buf + arg_index[0]);

    switch(cmd)
    {
       case 1://All parameters are right. Straight case
           application_id_info.v_ApplicationIDInfoPresent = 1;
           strcpy( (char*)application_id_info.v_ApplicationProvider, "x-oma-application:ulp.ua");//Application provider
           strcpy( (char*)application_id_info.v_ApplicationName,     "Tristar");//Application Name
           strcpy( (char*)application_id_info.v_ApplicationVersion,  "2.3");//Application version

           ALOGD("LBSSTELP : Application ID - AppProvider %s",  application_id_info.v_ApplicationProvider);
           ALOGD("LBSSTELP : Application ID - AppProvider %s", application_id_info.v_ApplicationName);
           ALOGD("LBSSTELP : Application ID - AppProvider %s", application_id_info.v_ApplicationVersion);
       break;

       case 2://Application if info not present but valid values in app provider and app name fields
           application_id_info.v_ApplicationIDInfoPresent = 1;
           strcpy((char*)application_id_info.v_ApplicationProvider, "0");//Application provider
           strcpy( (char*)application_id_info.v_ApplicationName,     "0");//Application Name
           strcpy( (char*)application_id_info.v_ApplicationVersion,  "0");//Application version
       break;

       case 3://Application if info NOT present and  NULL values in app provider and app name fields
           application_id_info.v_ApplicationIDInfoPresent = 1;
           strcpy( (char*)application_id_info.v_ApplicationProvider, "");//Application provider
           strcpy( (char*)application_id_info.v_ApplicationName,     "");//Application Name
           strcpy( (char*)application_id_info.v_ApplicationVersion,  "");//Application version
       break;

       case 4://Application if info id present but the app provider and app name are valid and app version invalid
           //As per ULP specification App version is an optional field and can be invalid
           application_id_info.v_ApplicationIDInfoPresent = 1;
           strcpy( (char*)application_id_info.v_ApplicationProvider, "x-oma-application:ulp.ua");//Application provider
           strcpy( (char*)application_id_info.v_ApplicationName,     "Tristar");//Application Name
           strcpy( (char*)application_id_info.v_ApplicationVersion,  "");//Application version
       break;

       default:
       break;
    }

    gpsClient_ServiceStart2(&application_id_info);

    return TSTENG_RESULT_OK;

}

/*
*Stop single shot
*/

static int TEST_NAF_gen_stop_service(TSTENG_handle_t handle)
{
    handle = handle; /*handle not used.To avoid Warning for known fact*/
    gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_callbacks );
    gpsClient_Stop();

    TEST_NAF_periodic_count = 0;
    periodic_count = 0;
    sleep(2);
    free(rc.NmeaData.pData);

    return TSTENG_RESULT_OK;

}

void LOCAL_NAF_set_config_mask(TSTENG_handle_t handle, uint32_t maskValid, uint32_t mask)
{
    rc = TEST_NAF_default_rc;

    gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_callbacks );

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Setup TLS- 0=TLS, 1-NoTLS\n");

    rc.configMaskValid = maskValid;
    rc.configMask = mask;
    rc.config2Valid = 0; //Bit to indicate Cold Start
    rc.sensMod = 0;
    rc.sensTTFF = 0;
    rc.powerPref = 0;
    rc.coldStart = 0;

    gpsClient_Set_Configuration( rc.configMaskValid, rc.configMask,rc.config2Valid,rc.sensMod,rc.sensTTFF,rc.powerPref, rc.coldStart);



}

/*
*Setting user configuration
*/

static int TEST_NAF_gen_cfg_user(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    rc = TEST_NAF_default_rc;

    if(args_found < 7)
    {
          ALOGD("LBSSTELP : ERR Set User Cfg - Expected 7 parameter");
       TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "LBSSTELP : Error in test arguments - Test requires 7 parameters");
       return TSTENG_RESULT_FAIL;
    }

    gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_callbacks );

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Gen Configuration set\n");


    rc.configMaskValid = atoi(cmd_buf + arg_index[0]);
    rc.configMask = atoi(cmd_buf + arg_index[1]);
    rc.config2Valid = atoi(cmd_buf + arg_index[2]);
    rc.sensMod = atoi(cmd_buf + arg_index[3]);
    rc.sensTTFF = atoi(cmd_buf + arg_index[4]);
    rc.powerPref = atoi(cmd_buf + arg_index[5]);
    rc.coldStart = atoi(cmd_buf + arg_index[6]);



    gpsClient_Set_Configuration( rc.configMaskValid, rc.configMask,rc.config2Valid,rc.sensMod,rc.sensTTFF,rc.powerPref, rc.coldStart);

    sleep(2);

    return TSTENG_RESULT_OK;

}

/*
*Delete Aiding data
*/

static int TEST_NAF_gen_cfg_delete_aid(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    rc = TEST_NAF_default_rc;
    if(args_found < 1)
    {
          ALOGD("LBSSTELP : ERR Set Delete Aid - Expected 1 parameter");
       TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "LBSSTELP : Error in test arguments - Test requires 1 parameters");
       return TSTENG_RESULT_FAIL;
    }
    gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_callbacks );

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Gen Configuration set\n");


    rc.aidingDataType = atoi(cmd_buf + arg_index[0]);


    gpsClient_DeleteAidingData(rc.aidingDataType);
    sleep(2);


    return TSTENG_RESULT_OK;

}


/*
*Start a Periodic fix
*/

/*
*Disable/enable agps
*/

static int TEST_NAF_gen_cfg_agps(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    rc = TEST_NAF_default_rc;
    uint8_t mode=0;
    uint32_t mask=0;

    if(args_found < 1)
    {
          ALOGD("LBSSTELP : ERR Set AGPS Mode - Expected 1 parameter");
       TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "LBSSTELP : Error in test arguments - Test requires 1 parameters");
       return TSTENG_RESULT_FAIL;
    }


    mode = atoi(cmd_buf + arg_index[0]);
    switch(mode)
    {
        case 0 : mask = K_CGPS_CONFIG_MS_POS_TECH_AUTONOMOUS; break;
        case 1:
        default:
            mask = K_CGPS_CONFIG_MS_POS_TECH_MSBASED    |
                        K_CGPS_CONFIG_MS_POS_TECH_MSASSISTED |
                        K_CGPS_CONFIG_MS_POS_TECH_AUTONOMOUS |
                        K_CGPS_CONFIG_AGPS_TRANSPORT_UPLANE      |
                        K_CGPS_CONFIG_AGPS_TRANSPORT_CPLANE      |
                        K_CGPS_CONFIG_AGPS_TRANSPORT_PREF_UPLANE |
                        K_CGPS_CONFIG_AGPS_SESSION_MO_ALLOWED     |
                        K_CGPS_CONFIG_AGPS_SESSION_MT_ALLOWED;

       break;
    }
    LOCAL_NAF_set_config_mask(handle, 255, mask);
    sleep(2);

    return TSTENG_RESULT_OK;

}

/*
*Disable/enable MSA, MSB
*/

static int TEST_NAF_gen_cfg_mo_pref_uplane(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    uint32_t mode=0;
    uint32_t mask=0;

    if(args_found < 1)
    {
          ALOGD("LBSSTELP : ERR Set MOLR Pref - Expected 1 parameter  : 0=MSB, 1=MSA 2=MSAMSB");
       TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "LBSSTELP : Error in test arguments - Test requires 1 parameters");
       return TSTENG_RESULT_FAIL;
    }

    mode = atoi(cmd_buf + arg_index[0]);
    if(mode >= 1)
    {
             mode =  K_CGPS_CONFIG_AGPS_TRANSPORT_PREF_UPLANE| K_CGPS_CONFIG_AGPS_TRANSPORT_UPLANE ;
             mask =  K_CGPS_CONFIG_AGPS_TRANSPORT_PREF_UPLANE| K_CGPS_CONFIG_AGPS_TRANSPORT_UPLANE ;
    }
    else
    {
             mode =  K_CGPS_CONFIG_AGPS_TRANSPORT_CPLANE;
             mask =  K_CGPS_CONFIG_AGPS_TRANSPORT_PREF_UPLANE| K_CGPS_CONFIG_AGPS_TRANSPORT_CPLANE ;
    }


    LOCAL_NAF_set_config_mask(handle, mask, mode);
    sleep(2);

    return TSTENG_RESULT_OK;

}

/*
*Disable/enable MSA, MSB
*/

static int TEST_NAF_gen_cfg_msa(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    rc = TEST_NAF_default_rc;
    uint8_t mode=0;
    uint32_t maskValid=0;
    uint32_t mask=0;

    if(args_found < 1)
    {
          ALOGD("LBSSTELP : ERR Set MSA MSB - Expected 1 parameter");
       TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "LBSSTELP : Error in test arguments - Test requires 1 parameters");
       return TSTENG_RESULT_FAIL;
    }

    mode = atoi(cmd_buf + arg_index[0]);
    switch (mode)
    {
      case 0: mask = K_CGPS_CONFIG_MS_POS_TECH_MSBASED; break;
      case 1: mask = K_CGPS_CONFIG_MS_POS_TECH_MSASSISTED; break;
      case 2: mask = K_CGPS_CONFIG_MS_POS_TECH_MSBASED     | K_CGPS_CONFIG_MS_POS_TECH_MSASSISTED; break;
      default:mask = K_CGPS_CONFIG_MS_POS_TECH_MSBASED     | K_CGPS_CONFIG_MS_POS_TECH_MSASSISTED; break;
    }
    maskValid = K_CGPS_CONFIG_MS_POS_TECH_MSBASED     | K_CGPS_CONFIG_MS_POS_TECH_MSASSISTED; //Only first 2 bits are being changed. Rest all remain what has been set before
    LOCAL_NAF_set_config_mask(handle, maskValid, mask);

    sleep(2);

    return TSTENG_RESULT_OK;

}

/*
*Semi Auto and Basic Self Location Mode
*/

static int TEST_NAF_gen_cfg_locationmode(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    rc = TEST_NAF_default_rc;
    uint8_t mode=0;
    uint32_t maskValid=0;
    uint32_t mask=0;

    if(args_found < 1)
    {
          ALOGD("LBSSTELP : ERR Set Semi Auto and Basic Self Mode - Expected 1 parameter");
       TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "LBSSTELP : Error in test arguments - Test requires 1 parameter");
       return TSTENG_RESULT_FAIL;
    }

    mode = atoi(cmd_buf + arg_index[0]);
    switch (mode)
    {
      case 0: mask = K_CGPS_CONFIG_SEMIAUTO; break;
      default:mask = K_CGPS_CONFIG_MS_POS_TECH_MSBASED     | K_CGPS_CONFIG_MS_POS_TECH_MSASSISTED; break;
    }
    maskValid = K_CGPS_CONFIG_SEMIAUTO;
    LOCAL_NAF_set_config_mask(handle, maskValid, mask);
    sleep(2);

    return TSTENG_RESULT_OK;

}


/*NOTE: These values MUST be in sync with the enum values of e_cgps_ConfigBitMask in the file cgps.hec*/
enum SAT
{
    CGPS_CONFIG_GPS_ENABLED                         = (1<<16),  /**< Mask for Enabling GPS. */
    CGPS_CONFIG_GLONASS_ENABLED                     = (1<<17),  /**< Mask for Enabling GLONASS */
    CGPS_CONFIG_SBAS_ENABLED                        = (1<<18),  /**< Mask for Enabling SBAS */
    CGPS_CONFIG_EXTENDED_EPHEMERIS_GPS_ENABLED      = (1<<19),  /**< Mask for Enabling EE for GPS */
    CGPS_CONFIG_EXTENDED_EPHEMERIS_GLONASS_ENABLED  = (1<<20),  /**< Mask for Enabling EE for Glonass */
    CGPS_CONFIG_WIFI_POS_ENABLED                    = (1<<21),   /**< Mask for Enabling Wi-Fi Positioning */
    K_CGPS_CONFIG_QZSS_ENABLED                      = (1<<22),   /**< Mask for Enabling Wi-Fi Positioning */ 
    K_CGPS_CONFIG_SNTP_REF_TIME_ENABLED             = (1<<23),   /**< Mask for Enabling Wi-Fi Positioning */
    CGPS_CONFIG_SELF_ASSISTED_GPS_ENABLED           = (1<<24),  /**< Mask for Enabling EE for GPS */
    CGPS_CONFIG_SELF_ASSISTED_GLONASS_ENABLED       = (1<<25),  /**< Mask for Enabling EE for Glonass*/

};

static int TEST_NAF_gen_Config_GPS_Glonass_SBAS(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    rc = TEST_NAF_default_rc;
    uint8_t gps=0,glonass=0,sbas=0;
    uint32_t maskValid=0;
    uint32_t mask=0;


    ALOGD("LBSSTELP : Entered TEST_NAF_gen_Config_GPS_Glonass_SBAS");

    if(args_found < 1)
    {
        ALOGD("LBSSTELP : ERR GPS/GLONASS/SBS Enable Disable Expected 1 parameter");
        TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "LBSSTELP : Error in test arguments - Test requires 1 parameter");
        return TSTENG_RESULT_FAIL;
    }

    gps = atoi(cmd_buf + arg_index[0]);
    glonass = atoi(cmd_buf + arg_index[1]);
    sbas = atoi(cmd_buf + arg_index[2]);

    if(gps)
        mask |= CGPS_CONFIG_GPS_ENABLED;

    if(glonass)
        mask |= CGPS_CONFIG_GLONASS_ENABLED;

    if(sbas)
        mask |= CGPS_CONFIG_SBAS_ENABLED;


    maskValid = CGPS_CONFIG_GPS_ENABLED | CGPS_CONFIG_GLONASS_ENABLED | CGPS_CONFIG_SBAS_ENABLED;

    ALOGD("LBSSTELP : Value of mask = %d ",mask);
    ALOGD("LBSSTELP : Value of maskValid = %d ",maskValid);

    LOCAL_NAF_set_config_mask(handle, maskValid, mask);


    return TSTENG_RESULT_OK;

}



/*
*Disable/enable  Extended Ephemeris -GPS , Extended Ephemeris -Glonass , Wi-Fi Positioning.
*/


static int TEST_NAF_gen_Config_EEGPS_EEGlonass_WIFIPOS(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    rc = TEST_NAF_default_rc;
    uint8_t EEgps=0,EEglonass=0,wifi=0;
    uint32_t maskValid=0;
    uint32_t mask=0;


    ALOGD("LBSSTELP : Entered TEST_NAF_gen_Config_EEGPS_EEGlonass_WIFIPOS");

    if(args_found < 1)
    {
        ALOGD("LBSSTELP : ERR EEGPS/EEGLONASS/Wifi Enable Disable Expected 1 parameter");
        TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "LBSSTELP : Error in test arguments - Test requires 1 parameter");
        return TSTENG_RESULT_FAIL;
    }

    EEgps = atoi(cmd_buf + arg_index[0]);
    EEglonass = atoi(cmd_buf + arg_index[1]);
    wifi = atoi(cmd_buf + arg_index[2]);

    if(EEgps)
        mask |= CGPS_CONFIG_EXTENDED_EPHEMERIS_GPS_ENABLED;

    if(EEglonass)
        mask |= CGPS_CONFIG_EXTENDED_EPHEMERIS_GLONASS_ENABLED;

    if(wifi)
        mask |= CGPS_CONFIG_WIFI_POS_ENABLED;


    maskValid = CGPS_CONFIG_EXTENDED_EPHEMERIS_GPS_ENABLED | CGPS_CONFIG_EXTENDED_EPHEMERIS_GLONASS_ENABLED | CGPS_CONFIG_WIFI_POS_ENABLED;

    ALOGD("LBSSTELP : Value of mask = %d ",mask);
    ALOGD("LBSSTELP : Value of maskValid = %d ",maskValid);

    LOCAL_NAF_set_config_mask(handle, maskValid, mask);


    return TSTENG_RESULT_OK;

}


/*
*Disable/enable  SNTP Reference Time.
*/


static int TEST_NAF_gen_Config_SNTP_Ref_Time(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    rc = TEST_NAF_default_rc;
    uint8_t RefTime=0;
    uint32_t maskValid=0;
    uint32_t mask=0;


    ALOGD("LBSSTELP : Entered TEST_NAF_gen_Config_SNTP_Ref_Time");

    if(args_found < 1)
    {
        ALOGD("LBSSTELP : ERR Ref Time Enable Disable Expected 1 parameter");
        TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "LBSSTELP : Error in test arguments - Test requires 1 parameter");
        return TSTENG_RESULT_FAIL;
    }

    RefTime = atoi(cmd_buf + arg_index[0]);


    if(RefTime)
        mask |= K_CGPS_CONFIG_SNTP_REF_TIME_ENABLED;

    maskValid = K_CGPS_CONFIG_SNTP_REF_TIME_ENABLED;

    ALOGD("LBSSTELP : Value of mask = %d ",mask);
    ALOGD("LBSSTELP : Value of maskValid = %d ",maskValid);

    LOCAL_NAF_set_config_mask(handle, maskValid, mask);


    return TSTENG_RESULT_OK;

}


/*
*Disable/enable  SAGPS /SAGlonass
*/

static int TEST_NAF_gen_Config_SAGPS_SAGlonass(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    rc = TEST_NAF_default_rc;
    uint8_t SAgps=0,SAglonass=0;
    uint32_t maskValid=0;
    uint32_t mask=0;


    ALOGD("LBSSTELP : Entered TEST_NAF_gen_Config_SAGPS_SAGlonass");

    if(args_found < 1)
    {
        ALOGD("LBSSTELP : ERR SAGPS/SAGLonass Enable Disable Expected 1 parameter");
        TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "LBSSTELP : Error in test arguments - Test requires 1 parameter");
        return TSTENG_RESULT_FAIL;
    }

    SAgps = atoi(cmd_buf + arg_index[0]);
    SAglonass = atoi(cmd_buf + arg_index[1]);


    if(SAgps)
        mask |= CGPS_CONFIG_SELF_ASSISTED_GPS_ENABLED;

    if(SAglonass)
        mask |= CGPS_CONFIG_SELF_ASSISTED_GLONASS_ENABLED;

    maskValid = CGPS_CONFIG_SELF_ASSISTED_GPS_ENABLED | CGPS_CONFIG_SELF_ASSISTED_GLONASS_ENABLED;

    ALOGD("LBSSTELP : Value of mask = %d ",mask);
    ALOGD("LBSSTELP : Value of maskValid = %d ",maskValid);

    LOCAL_NAF_set_config_mask(handle, maskValid, mask);


    return TSTENG_RESULT_OK;

}


/*
*Disable/enable TLS
*/

static int TEST_NAF_gen_cfg_tls(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    uint32_t mode=0;

    if(args_found < 1)
    {
          ALOGD("LBSSTELP : ERR Set NoTLS - Expected 1 parameter");
       TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "LBSSTELP : Error in test arguments - Test requires 1 parameters");
       return TSTENG_RESULT_FAIL;
    }

    mode = atoi(cmd_buf + arg_index[0]);
    if(mode >= 1)
        mode = 0XFFFF & K_CGPS_CONFIG_TLS_SUPPORTED;

    LOCAL_NAF_set_config_mask(handle, K_CGPS_CONFIG_TLS_SUPPORTED, mode);
    sleep(2);

    return TSTENG_RESULT_OK;

}

static int TEST_NAF_gen_cfg_supl(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    uint32_t mode=0;
    uint32_t local_mode_mask = 0; /* This is used to indicate valid masks */

    if(args_found < 1)
    {
          ALOGD("LBSSTELP : ERR Set Supl V2 - Expected 1 parameter");
       TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "LBSSTELP : Error in test arguments - Test requires 1 parameters");
       return TSTENG_RESULT_FAIL;
    }
    mode = atoi(cmd_buf + arg_index[0]);
    ALOGD("LBSSTELP : TEST_NAF_gen_cfg_supl mode =%d ",mode);

    switch(mode)
    {
      case 0:
          LOCAL_NAF_set_config_mask(handle, K_CGPS_CONFIG_AGPS_TRANSPORT_UPLANE, 0);
          break;
      case 2:
            local_mode_mask = K_CGPS_CONFIG_SUPLV2_SUPPORTED | K_CGPS_CONFIG_AGPS_TRANSPORT_UPLANE;
          LOCAL_NAF_set_config_mask(handle, local_mode_mask ,(local_mode_mask &0xFFFF));
          break;
      case 1:
      default:
            local_mode_mask = K_CGPS_CONFIG_SUPLV2_SUPPORTED | K_CGPS_CONFIG_AGPS_TRANSPORT_UPLANE;
          LOCAL_NAF_set_config_mask(handle, local_mode_mask ,(K_CGPS_CONFIG_AGPS_TRANSPORT_UPLANE & 0xFFFF));
          break;

    }

    return TSTENG_RESULT_OK;

}



/*
*Disable/enable Start Mode
*/

static int TEST_NAF_gen_cfg_start_mode(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    TSTENG_BEGIN_STEP(handle, __func__);
    rc = TEST_NAF_default_rc;
    uint8_t mode=0;

    if(args_found < 1)
    {
          ALOGD("LBSSTELP : ERR Set Start Mode - Expected 1 parameter");
       TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "LBSSTELP : Error in test arguments - Test requires 1 parameters");
       return TSTENG_RESULT_FAIL;
    }


    gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_callbacks );

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Setup StartMode- 0=Quick, 1-Cold, 2-ColdAll, 3-WarmAll, 4-HotAll\n");

    mode = atoi(cmd_buf + arg_index[0]);
    rc.configMask = 0;
    rc.configMaskValid = 0;
    rc.config2Valid = 16; //Bit to indicate Cold Start
    rc.sensMod = 0;
    rc.sensTTFF = 0;
    rc.powerPref = 0;
    if(mode > 4) mode = 0; //Set to Quick Start or Default
    rc.coldStart = mode;

    gpsClient_Set_Configuration( rc.configMaskValid, rc.configMask,rc.config2Valid,rc.sensMod,rc.sensTTFF,rc.powerPref, rc.coldStart);
    sleep(2);
    TSTENG_END_STEP(handle, __func__);


    return TSTENG_RESULT_OK;

}
/*
*Disable/enable Frequeyncy Aiding
*/
static int TEST_NAF_gen_cfg_Freq_Aiding(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    uint32_t mode=0;

    if(args_found < 1)
    {
       ALOGD("LBSSTELP : Disable Frequency aiding command received No parameters");
       TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "LBSSTELP : Error in test arguments - Test requires 1 parameters");
       return TSTENG_RESULT_FAIL;
    }

    mode = atoi(cmd_buf + arg_index[0]);
    if(mode >= 1)
        mode = 0XFFFF & K_CGPS_CONFIG_FREQ_AIDING;
    else
        mode = 0;

    ALOGD("LBSSTELP : Configure Frequency aiding command received %d", mode);
    LOCAL_NAF_set_config_mask(handle, K_CGPS_CONFIG_FREQ_AIDING, mode);

    return TSTENG_RESULT_OK;

}


/*
*Disable/enable Fine Time Assistance
*/
static int TEST_NAF_gen_cfg_FTA(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    uint32_t mode=0;

    if(args_found < 1)
    {
       ALOGD("LBSSTELP : Configure FineTime aiding command received No parameters");
       TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "LBSSTELP : Error in test arguments - Test requires 1 parameters");
       return TSTENG_RESULT_FAIL;
    }

    mode = atoi(cmd_buf + arg_index[0]);
    if(mode >= 1)
        mode = 0XFFFF & K_CGPS_CONFIG_AGPS_FTA;
    else
        mode = 0;

    ALOGD("LBSSTELP : Configure Fine Time Aiding command received %d",mode);
    LOCAL_NAF_set_config_mask(handle, K_CGPS_CONFIG_AGPS_FTA, mode);

    return TSTENG_RESULT_OK;
}

/* Autonomous Periodic Test */
static int TEST_NAF_autonomous_periodic(TSTENG_handle_t handle)
{

    int outputType = 1;
    
    TEST_NAF_periodic_count = 1;
    rc = TEST_NAF_default_rc;


    gpsClient_Init( (t_gpsClient_Callbacks *) &TEST_NAF_callbacks );

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Data callbacks successfully registered\n");



    rc.NMEA = 0;
    rc.reporting_rate = 1000;
    rc.no_count_periodic = 10;

    gpsClient_ServiceStart();

    gpsClient_PeriodicFix( outputType, rc.NMEA, rc.reporting_rate );

    (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
    (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);


    return TSTENG_RESULT_OK;
}


/* Autonomous SingleShot Test */

static int TEST_NAF_autonomous_singleshot(TSTENG_handle_t handle)
{


    int outputType = 1;
    TEST_NAF_periodic_count = 0;

    rc = TEST_NAF_default_rc;


    gpsClient_Init( (t_gpsClient_Callbacks *) &TEST_NAF_callbacks );

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Data callbacks successfully registered\n");


    rc.NMEA = 0;
    rc.time_to_first_fix_ms = 200000;
    rc.accuracy_horizontal_m = 100;
    rc.accuracy_vertical_m = 100;
    rc.age_limit_ms = 100;

    gpsClient_ServiceStart();


    gpsClient_SingleShotFix( outputType, rc.NMEA, rc.time_to_first_fix_ms, rc.accuracy_horizontal_m, rc.accuracy_vertical_m, rc.age_limit_ms);

    (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
    (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);


    return TSTENG_RESULT_OK;
}



/**
  Register for Periodic Fix with period of 1s and wait for a fix,
  register for single shot fix
  Periodic fix call back should stop after registering Singleshot fix.
**/

static int TEST_NAF_autonomous_periodic_singleshot(TSTENG_handle_t handle)
{
     flage=1;
    int outputType = 1;
    TEST_NAF_periodic_count = 1;

    periodic_count = 0;

    rc = TEST_NAF_default_rc;


    gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_callbacks );

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Data callbacks successfully registered\n");



    rc.NMEA = 0;
    rc.reporting_rate = 1000;
    rc.no_count_periodic = 10;

    gpsClient_ServiceStart();

    gpsClient_PeriodicFix( outputType, rc.NMEA, rc.reporting_rate );

    (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
    (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);



    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Data callbacks successfully registered\n");


    rc.NMEA = 0;
    rc.time_to_first_fix_ms = 200000;
    rc.accuracy_horizontal_m = 100;
    rc.accuracy_vertical_m = 100;
    rc.age_limit_ms = 100;

    gpsClient_ServiceStart();


    gpsClient_SingleShotFix( outputType, rc.NMEA, rc.time_to_first_fix_ms, rc.accuracy_horizontal_m, rc.accuracy_vertical_m, rc.age_limit_ms);

    (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
    (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);


    return TSTENG_RESULT_OK;


}


/*
*register for single shot fix
* after getting the fix register for perodic fix
*Periodic fix callback should start after  getting a singleshot fix or timeout
*/

static int TEST_NAF_autonomous_singleshot_periodic(TSTENG_handle_t handle)
{


    int outputType = 1;
    TEST_NAF_periodic_count = 0;

    rc = TEST_NAF_default_rc;

    gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_callbacks );

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Data callbacks successfully registered\n");


    rc.NMEA = 0;
    rc.time_to_first_fix_ms = 200000;
    rc.accuracy_horizontal_m = 100;
    rc.accuracy_vertical_m = 100;
    rc.age_limit_ms = 100;

    gpsClient_ServiceStart();


    gpsClient_SingleShotFix( outputType, rc.NMEA, rc.time_to_first_fix_ms, rc.accuracy_horizontal_m, rc.accuracy_vertical_m, rc.age_limit_ms);

    (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
    (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);


    TEST_NAF_periodic_count = 1;

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Data callbacks successfully registered\n");



    rc.NMEA = 0;
    rc.reporting_rate = 1000;
    rc.no_count_periodic = 10;

    gpsClient_ServiceStart();

    gpsClient_PeriodicFix( outputType, rc.NMEA, rc.reporting_rate );

    (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
    (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);


    return TSTENG_RESULT_OK;

}

/*
*Setting user configuration
*Start a Periodic fix
*/

static int TEST_NAF_user_configuration(TSTENG_handle_t handle)
{

    int outputType = 1;
    TEST_NAF_periodic_count = 1;

    rc = TEST_NAF_default_rc;


    gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_callbacks );

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Data callbacks successfully registered\n");


    rc.configMaskValid = 255;
    rc.configMask = K_CGPS_CONFIG_MS_POS_TECH_MSBASED | K_CGPS_CONFIG_MS_POS_TECH_AUTONOMOUS |
        K_CGPS_CONFIG_AGPS_TRANSPORT_CPLANE | K_CGPS_CONFIG_AGPS_SESSION_MO_ALLOWED | K_CGPS_CONFIG_AGPS_SESSION_MT_ALLOWED;
    rc.config2Valid = 16;
    rc.sensMod = 0;
    rc.sensTTFF = 0;
    rc.powerPref = 0;
    rc.coldStart = 0;



    gpsClient_Set_Configuration( rc.configMaskValid, rc.configMask,rc.config2Valid,rc.sensMod,rc.sensTTFF,rc.powerPref, rc.coldStart);



    rc.NMEA = 0;
    rc.reporting_rate = 1000;
    rc.no_count_periodic = 10;

    gpsClient_ServiceStart();

    gpsClient_PeriodicFix( outputType, rc.NMEA, rc.reporting_rate );

    (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
    (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);


    return TSTENG_RESULT_OK;

}

/*
*Setting test configuration
*Start a Periodic fix
*/

static int TEST_NAF_gen_cfg_platform(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    PlatformConfig = TEST_NAF_DefaultPlatformConfig_rc;

     if(args_found < 8)
     {
           ALOGD("LBSSTELP : ERR Set Platform Cfg - Expected 8 parameter");
        TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "LBSSTELP : Error in test arguments - Test requires 8 parameters");
        return TSTENG_RESULT_FAIL;
     }

     gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_callbacks );

     TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                         "LBSSTELP :Platform Configuration set\n");

    PlatformConfig.v_Config1MaskValid = atoi(cmd_buf + arg_index[0]);
    PlatformConfig.v_Config1Mask = atoi(cmd_buf + arg_index[1]);
    PlatformConfig.v_Config2Mask = atoi(cmd_buf + arg_index[2]);
    PlatformConfig.v_MsaMandatoryMask = atoi(cmd_buf + arg_index[3]);
    PlatformConfig.v_MsbMandatoryMask = atoi(cmd_buf + arg_index[4]);
    PlatformConfig.v_SleepTimerDuration= atoi(cmd_buf + arg_index[5]);
    PlatformConfig.v_ComaTimerDuration= atoi(cmd_buf + arg_index[6]);
    PlatformConfig.v_DeepComaDuration = atoi(cmd_buf + arg_index[7]);


    gpsClient_Set_PlatformConfiguration( PlatformConfig.v_Config1MaskValid,PlatformConfig.v_Config1Mask,PlatformConfig.v_Config2Mask, \
        PlatformConfig.v_MsaMandatoryMask,PlatformConfig.v_MsbMandatoryMask,\
         PlatformConfig.v_SleepTimerDuration,PlatformConfig.v_ComaTimerDuration,PlatformConfig.v_DeepComaDuration);

    sleep(2);

     return TSTENG_RESULT_OK;

}


/*
*Getting User configuration
*/

static int TEST_NAF_gen_get_user_cfg(TSTENG_handle_t handle)
{
   
   handle = handle; /*handle not used.To avoid Warning for known fact*/
   /* To be Supported later */
   return TSTENG_RESULT_OK;

}


/*
*Getting Platform configuration
*/

static int TEST_NAF_gen_get_platform_cfg(TSTENG_handle_t handle)
{
    
    handle = handle; /*handle not used.To avoid Warning for known fact*/
    /* To be Supported later */
    return TSTENG_RESULT_OK;


}



/*
*Register for a periodic fix and wait for a fix.
* stop CGPS, wait till it goes to Sleep mode.
* Once it went to sleep mode register for a periodic fix and Wait for a fix.
*It should get a fix after coming back from Sleep mode

*/

static int TEST_NAF_autonomous_periodic_sleep_periodic(TSTENG_handle_t handle)
{

    int outputType = 1;
    TEST_NAF_periodic_count = 1;

    rc = TEST_NAF_default_rc;


    gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_callbacks );

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Data callbacks successfully registered\n");



    rc.NMEA = 0;
    rc.reporting_rate = 1000;
    rc.no_count_periodic = 10;

    gpsClient_ServiceStart();

    gpsClient_PeriodicFix( outputType, rc.NMEA, rc.reporting_rate );

    (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
    (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);

    sleep(60);
    TEST_NAF_periodic_count = 1;


    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Data callbacks successfully registered\n");



    rc.NMEA = 0;
    rc.reporting_rate = 1000;
    rc.no_count_periodic = 10;

    gpsClient_ServiceStart();

    gpsClient_PeriodicFix( outputType, rc.NMEA, rc.reporting_rate );

    (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
    (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);



    return TSTENG_RESULT_OK;


}




/*
*Register for a periodic fix and wait for a fix.
* stop CGPS, wait till it goes to Sleep mode.
* Once it went to sleep mode register for a singleshot fix and Wait for a fix.
*It should get a fix after coming back from Sleep mode

*/

static int TEST_NAF_autonomous_periodic_sleep_singleshot(TSTENG_handle_t handle)
{
    flage=1;
    int outputType = 1;
    TEST_NAF_periodic_count = 1;

    periodic_count = 0;

    rc = TEST_NAF_default_rc;

    gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_callbacks );

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Data callbacks successfully registered\n");



    rc.NMEA = 0;
    rc.reporting_rate = 1000;
    rc.no_count_periodic = 10;

    gpsClient_ServiceStart();

    gpsClient_PeriodicFix( outputType, rc.NMEA, rc.reporting_rate );

    (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
    (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);

    sleep(60);


    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Data callbacks successfully registered\n");


    rc.NMEA = 0;
    rc.time_to_first_fix_ms = 200000;
    rc.accuracy_horizontal_m = 100;
    rc.accuracy_vertical_m = 100;
    rc.age_limit_ms = 100;

    gpsClient_ServiceStart();


    gpsClient_SingleShotFix( outputType, rc.NMEA, rc.time_to_first_fix_ms, rc.accuracy_horizontal_m, rc.accuracy_vertical_m, rc.age_limit_ms);

    (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
    (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);


    return TSTENG_RESULT_OK;

}



/*
*Call CGPS_Register_Periodic_Fix with valid Handle
*Output_Type = 0
*All other parameters valid
*return should indicate success , NMEA mode is used as output.
*/

static int TEST_NAF_autonomous_periodic_outputtype0(TSTENG_handle_t handle)
{

     int outputType = 0;
     TEST_NAF_periodic_count = 1;

     rc = TEST_NAF_default_rc;


     gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_callbacks );

     TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                         "LBSSTELP : Data callbacks successfully registered\n");



     rc.NMEA = 0x3ff; /* Enable all NMEA Sentences */
     rc.reporting_rate = 1000;
     rc.no_count_periodic = 10;

     gpsClient_ServiceStart();

     gpsClient_PeriodicFix( outputType, rc.NMEA, rc.reporting_rate );

     (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
     (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);


     return TSTENG_RESULT_OK;




}



/*
*Call CGPS_Register_Periodic_Fix with valid Handle
*Output_Type = 1
*All other parameters valid
*return should indicate success , C structure used to provide fix info
*/

static int TEST_NAF_autonomous_periodic_outputtype1(TSTENG_handle_t handle)
{

    int outputType = 1;
    TEST_NAF_periodic_count = 1;

    rc = TEST_NAF_default_rc;

    gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_callbacks );

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Data callbacks successfully registered\n");



    rc.NMEA = 0;
    rc.reporting_rate = 1000;
    rc.no_count_periodic = 10;

    gpsClient_ServiceStart();

    gpsClient_PeriodicFix( outputType, rc.NMEA, rc.reporting_rate );

    (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
    (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);


    return TSTENG_RESULT_OK;


}


/*
*Call CGPS_Register_Single_Shot_Fix with valid Handle
*Output_Type = 0
*All other parameters valid
* return should indicate success , NMEA mode is used as output.
*/

static int TEST_NAF_autonomous_singleshot_outputtype0(TSTENG_handle_t handle)
{

    int outputType = 0;
    TEST_NAF_periodic_count = 0;

    rc = TEST_NAF_default_rc;

    gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_callbacks );

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Data callbacks successfully registered\n");


    rc.NMEA = 0x3ff; /* Enable all NMEA Sentences */
    rc.time_to_first_fix_ms = 200000;
    rc.accuracy_horizontal_m = 100;
    rc.accuracy_vertical_m = 100;
    rc.age_limit_ms = 100;

    gpsClient_ServiceStart();


    gpsClient_SingleShotFix( outputType, rc.NMEA, rc.time_to_first_fix_ms, rc.accuracy_horizontal_m, rc.accuracy_vertical_m, rc.age_limit_ms);

    (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
    (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);


    return TSTENG_RESULT_OK;

}



/*
*Call CGPS_Register_Single_Shot_Fix with valid Handle
*Output_Type = 1
*All other parameters valid
* return should indicate success , C structure used to provide fix info
*/

static int TEST_NAF_autonomous_singleshot_outputtype1(TSTENG_handle_t handle)
{

    int outputType = 1;
    TEST_NAF_periodic_count = 0;

    rc = TEST_NAF_default_rc;

    gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_callbacks );

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Data callbacks successfully registered\n");


    rc.NMEA = 0;
    rc.time_to_first_fix_ms = 200000;
    rc.accuracy_horizontal_m = 100;
    rc.accuracy_vertical_m = 100;
    rc.age_limit_ms = 100;

    gpsClient_ServiceStart();


    gpsClient_SingleShotFix( outputType, rc.NMEA, rc.time_to_first_fix_ms, rc.accuracy_horizontal_m, rc.accuracy_vertical_m, rc.age_limit_ms);

    (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
    (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);



    return TSTENG_RESULT_OK;


}



/*
*Autonomous Hot start Periodic report test
*/

static int TEST_NAF_autonomous_periodic_hot(TSTENG_handle_t handle)
{

    int outputType = 1;
    TEST_NAF_periodic_count = 1;

    rc = TEST_NAF_default_rc;


    gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_callbacks );

    rc.aidingDataType = 0;
    gpsClient_DeleteAidingData( rc.aidingDataType );
    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Data callbacks successfully registered\n");



    rc.NMEA = 0;
    rc.reporting_rate = 1000;
    rc.no_count_periodic = 10;

    gpsClient_ServiceStart();

    gpsClient_PeriodicFix( outputType, rc.NMEA, rc.reporting_rate );

    (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
    (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);


    return TSTENG_RESULT_OK;


}



/*
*Autonomous Cold start Periodic report test
*/

static int TEST_NAF_autonomous_periodic_cold(TSTENG_handle_t handle)
{

    int outputType = 1;
    TEST_NAF_periodic_count = 1;

    rc = TEST_NAF_default_rc;


    gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_callbacks );

    rc.aidingDataType = GPSCLIENT_ASSIST_DELETE_ALMANAC | GPSCLIENT_ASSIST_DELETE_EPHEMERIS | GPSCLIENT_ASSIST_DELETE_POSITION |\
                        GPSCLIENT_ASSIST_DELETE_TIME | GPSCLIENT_ASSIST_DELETE_UTC | GPSCLIENT_ASSIST_DELETE_HEALTH;

    gpsClient_DeleteAidingData( rc.aidingDataType );
    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Data callbacks successfully registered\n");



    rc.NMEA = 0;
    rc.reporting_rate = 1000;
    rc.no_count_periodic = 10;

    gpsClient_ServiceStart();

    gpsClient_PeriodicFix( outputType, rc.NMEA, rc.reporting_rate );

    (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
    (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);


    return TSTENG_RESULT_OK;

}



/*
*Autonomous Warm start Periodic report test
*/

static int TEST_NAF_autonomous_periodic_warm(TSTENG_handle_t handle)
{

    int outputType = 1;
    TEST_NAF_periodic_count = 1;


    rc = TEST_NAF_default_rc;

    gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_callbacks );
    rc.aidingDataType = GPSCLIENT_ASSIST_DELETE_EPHEMERIS;
    gpsClient_DeleteAidingData( rc.aidingDataType );
    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Data callbacks successfully registered\n");



    rc.NMEA = 0;
    rc.reporting_rate = 1000;
    rc.no_count_periodic = 10;

    gpsClient_ServiceStart();

    gpsClient_PeriodicFix( outputType, rc.NMEA, rc.reporting_rate );

    (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
    (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);


    return TSTENG_RESULT_OK;

}



/*
*Autonomous Hot start Singleshot test
*/

static int TEST_NAF_autonomous_singleshot_hot(TSTENG_handle_t handle)
{

    int outputType = 1;
    TEST_NAF_periodic_count = 0;

    rc = TEST_NAF_default_rc;


    gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_callbacks );
    rc.aidingDataType = 0;
    gpsClient_DeleteAidingData( rc.aidingDataType );

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Data callbacks successfully registered\n");


    rc.NMEA = 0;
    rc.time_to_first_fix_ms = 200000;
    rc.accuracy_horizontal_m = 100;
    rc.accuracy_vertical_m = 100;
    rc.age_limit_ms = 100;

    gpsClient_ServiceStart();


    gpsClient_SingleShotFix( outputType, rc.NMEA, rc.time_to_first_fix_ms, rc.accuracy_horizontal_m, rc.accuracy_vertical_m, rc.age_limit_ms);

    (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
    (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);


    return TSTENG_RESULT_OK;


}



/*
*Autonomous Cold start Singleshot test
*/

static int TEST_NAF_autonomous_singleshot_cold(TSTENG_handle_t handle)
{

    int outputType = 1;
    TEST_NAF_periodic_count = 0;

    rc = TEST_NAF_default_rc;


    gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_callbacks );
    rc.aidingDataType = GPSCLIENT_ASSIST_DELETE_ALMANAC | GPSCLIENT_ASSIST_DELETE_EPHEMERIS | GPSCLIENT_ASSIST_DELETE_POSITION |\
                        GPSCLIENT_ASSIST_DELETE_TIME | GPSCLIENT_ASSIST_DELETE_UTC | GPSCLIENT_ASSIST_DELETE_HEALTH;

    gpsClient_DeleteAidingData( rc.aidingDataType );

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Data callbacks successfully registered\n");




    rc.NMEA = 0;
    rc.time_to_first_fix_ms = 200000;
    rc.accuracy_horizontal_m = 100;
    rc.accuracy_vertical_m = 100;
    rc.age_limit_ms = 100;

    gpsClient_ServiceStart();


    gpsClient_SingleShotFix( outputType, rc.NMEA, rc.time_to_first_fix_ms, rc.accuracy_horizontal_m, rc.accuracy_vertical_m, rc.age_limit_ms);

    (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
    (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);


    return TSTENG_RESULT_OK;

}



/*
*Autonomous Warm start Singleshot test
*/

static int TEST_NAF_autonomous_singleshot_warm(TSTENG_handle_t handle)
{

    int outputType = 1;
    TEST_NAF_periodic_count = 0;

    rc = TEST_NAF_default_rc;


    gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_callbacks );
    rc.aidingDataType = GPSCLIENT_ASSIST_DELETE_EPHEMERIS;

    gpsClient_DeleteAidingData( rc.aidingDataType );

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Data callbacks successfully registered\n");




    rc.NMEA = 0;
    rc.time_to_first_fix_ms = 200000;
    rc.accuracy_horizontal_m = 100;
    rc.accuracy_vertical_m = 100;
    rc.age_limit_ms = 100;

    gpsClient_ServiceStart();


    gpsClient_SingleShotFix( outputType, rc.NMEA, rc.time_to_first_fix_ms, rc.accuracy_horizontal_m,    rc.accuracy_vertical_m, rc.age_limit_ms);

    (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
    (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);


    return TSTENG_RESULT_OK;



}



/*
*Mobile Originated User configuration test for MS Based
*/

static int TEST_NAF_MO1_user_configuration(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    int outputType = 1;
    TEST_NAF_periodic_count = 0;

    rc = TEST_NAF_default_rc;
    switch (args_found)
    {
        case 4:
            rc.time_to_first_fix_ms = atoi(cmd_buf + arg_index[0]) * 1000;
            rc.accuracy_horizontal_m = atoi(cmd_buf + arg_index[1]);
            rc.accuracy_vertical_m = atoi(cmd_buf + arg_index[2]);
            rc.age_limit_ms = atoi(cmd_buf + arg_index[3]) * 1000;
            ALOGD("LBSSTELP : QOP Argument found - %d Arguments - TTFF %d HA %d VA %d Age %d \n",args_found,atoi(cmd_buf + arg_index[0]),atoi(cmd_buf + arg_index[1]),atoi(cmd_buf + arg_index[2]),atoi(cmd_buf + arg_index[3]));
            TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, "LBSSTELP : Applying command line parameters");
        break;

        case 0:
            TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, "LBSSTELP : Applying Default parameters");
        break;

        default:
            TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "LBSSTELP : Error in test arguments - Test requires 4 parameters");
            return TSTENG_RESULT_FAIL;

    };



    gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_callbacks );


    rc.configMaskValid = 65535;
    rc.configMask = K_CGPS_CONFIG_MS_POS_TECH_MSBASED | K_CGPS_CONFIG_MS_POS_TECH_AUTONOMOUS |
        K_CGPS_CONFIG_AGPS_TRANSPORT_CPLANE | K_CGPS_CONFIG_AGPS_SESSION_MO_ALLOWED | K_CGPS_CONFIG_AGPS_SESSION_MT_ALLOWED|K_CGPS_CONFIG_SEMIAUTO;
    rc.config2Valid = 16;
    rc.sensMod = 0;
    rc.sensTTFF = 0;
    rc.powerPref = 0;
    rc.coldStart = 0;

    gpsClient_Set_Configuration( rc.configMaskValid, rc.configMask,rc.config2Valid,rc.sensMod,rc.sensTTFF,rc.powerPref, rc.coldStart);
    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Data callbacks successfully registered\n");




    rc.NMEA = 0;
    rc.time_to_first_fix_ms = 200000;
    rc.accuracy_horizontal_m = 100;
    rc.accuracy_vertical_m = 100;
    rc.age_limit_ms = 100;

    gpsClient_ServiceStart();


    ALOGD("LBSSTELP : QOP Argument injected - TTFF %lu HA %d VA %d Age %lu \n",rc.time_to_first_fix_ms,rc.accuracy_horizontal_m,rc.accuracy_vertical_m,rc.age_limit_ms);

    gpsClient_SingleShotFix( outputType, rc.NMEA, rc.time_to_first_fix_ms, rc.accuracy_horizontal_m, rc.accuracy_vertical_m, rc.age_limit_ms);


    (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
    (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);



    return TSTENG_RESULT_OK;


}


/*
*Mobile Originated User configuration test for MS Assisted
*/

static int TEST_NAF_MO2_user_configuration(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    int outputType = 1;
    TEST_NAF_periodic_count = 0;


    rc = TEST_NAF_default_rc;


    switch (args_found)
    {
        case 4:
            rc.time_to_first_fix_ms = atoi(cmd_buf + arg_index[0]) * 1000;
            rc.accuracy_horizontal_m = atoi(cmd_buf + arg_index[1]);
            rc.accuracy_vertical_m = atoi(cmd_buf + arg_index[2]);
            rc.age_limit_ms = atoi(cmd_buf + arg_index[3]) * 1000;
            ALOGD("LBSSTELP : QOP Argument found - %d Agrguments - TTFF %d HA %d VA %d Age %d \n",args_found,atoi(cmd_buf + arg_index[0]),atoi(cmd_buf + arg_index[1]),atoi(cmd_buf + arg_index[2]),atoi(cmd_buf + arg_index[3]));
            TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, "LBSSTELP : Applying command line parameters");
        break;

        case 0:
            TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, "LBSSTELP : Applying Default parameters");
        break;

        default:
            TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "LBSSTELP : Error in test arguments - Test requires 4 parameters");
            return TSTENG_RESULT_FAIL;

    };


    gpsClient_Init((t_gpsClient_Callbacks *)&TEST_NAF_callbacks );

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Data callbacks successfully registered\n");

    rc.configMaskValid = 65535;
    rc.configMask = K_CGPS_CONFIG_MS_POS_TECH_MSASSISTED | K_CGPS_CONFIG_MS_POS_TECH_AUTONOMOUS |
        K_CGPS_CONFIG_AGPS_TRANSPORT_CPLANE | K_CGPS_CONFIG_AGPS_SESSION_MO_ALLOWED | K_CGPS_CONFIG_AGPS_SESSION_MT_ALLOWED;
    rc.config2Valid = 1;
    rc.sensMod = 0;
    rc.sensTTFF = 0;
    rc.powerPref = 0;
    rc.coldStart = 0;

    gpsClient_Set_Configuration( rc.configMaskValid, rc.configMask,rc.config2Valid,rc.sensMod,rc.sensTTFF,rc.powerPref, rc.coldStart);


    rc.NMEA = 0;

    gpsClient_ServiceStart();


    ALOGD("LBSSTELP : QOP Argument injected - TTFF %lu HA %d VA %d Age %lu \n",rc.time_to_first_fix_ms,rc.accuracy_horizontal_m,rc.accuracy_vertical_m,rc.age_limit_ms);

    gpsClient_SingleShotFix( outputType, rc.NMEA, rc.time_to_first_fix_ms, rc.accuracy_horizontal_m, rc.accuracy_vertical_m, rc.age_limit_ms);

    (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
    (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);


    return TSTENG_RESULT_OK;


}


static int TEST_NAF_MT1(TSTENG_handle_t handle)
{

     TEST_NAF_periodic_count = 0;
     rc.mt_testcases = 0;

     ALOGD("LBSSTELP :TEST_NAF_MT1\n");

     gpsClient_Init( &TEST_NAF_callbacks );

     gpsclient_NotificationInit(&TEST_NAF_Notification_callbacks);


     TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                         "LBSSTELP : Data callbacks successfully registered\n");



     gpsClient_ServiceStart();

     (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
     (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);


     return TSTENG_RESULT_OK;

}

static int TEST_NAF_MT2(TSTENG_handle_t handle)
{

     TEST_NAF_periodic_count = 0;
     rc.mt_testcases = 1;

     ALOGD("LBSSTELP :TEST_NAF_MT2\n");

     gpsClient_Init( &TEST_NAF_callbacks );

     gpsclient_NotificationInit(&TEST_NAF_Notification_callbacks);


     TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                         "LBSSTELP : Data callbacks successfully registered\n");



     gpsClient_ServiceStart();

     (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
     (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);


     return TSTENG_RESULT_OK;

}

static int TEST_NAF_MT3(TSTENG_handle_t handle)
{

     TEST_NAF_periodic_count = 0;
     rc.mt_testcases = 2;

     ALOGD("LBSSTELP :TEST_NAF_MT3\n");

     gpsClient_Init( &TEST_NAF_callbacks );

     gpsclient_NotificationInit(&TEST_NAF_Notification_callbacks);


     TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                         "LBSSTELP : Data callbacks successfully registered\n");



     gpsClient_ServiceStart();

     (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
     (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);


     return TSTENG_RESULT_OK;

}

static int TEST_NAF_MT4(TSTENG_handle_t handle)
{

     TEST_NAF_periodic_count = 0;
     rc.mt_testcases = 3;

     ALOGD("LBSSTELP :TEST_NAF_MT4\n");

     gpsClient_Init( &TEST_NAF_callbacks );

     gpsclient_NotificationInit(&TEST_NAF_Notification_callbacks);


     TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                         "LBSSTELP : Data callbacks successfully registered\n");



     gpsClient_ServiceStart();

     (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
     (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);


     return TSTENG_RESULT_OK;

}

static int TEST_NAF_MT5(TSTENG_handle_t handle)
{

     TEST_NAF_periodic_count = 0;
     rc.mt_testcases = 4;

     ALOGD("LBSSTELP :TEST_NAF_MT5\n");

     gpsClient_Init( &TEST_NAF_callbacks );

     gpsclient_NotificationInit(&TEST_NAF_Notification_callbacks);


     TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                         "LBSSTELP : Data callbacks successfully registered\n");



     gpsClient_ServiceStart();

     (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
     (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);


     return TSTENG_RESULT_OK;

}

static int TEST_NAF_MT6(TSTENG_handle_t handle)
{

     TEST_NAF_periodic_count = 0;
     rc.mt_testcases = 5;

     ALOGD("LBSSTELP :TEST_NAF_MT6\n");

     gpsClient_Init( &TEST_NAF_callbacks );

     gpsclient_NotificationInit(&TEST_NAF_Notification_callbacks);


     TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                         "LBSSTELP : Data callbacks successfully registered\n");



     gpsClient_ServiceStart();

     (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
     (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);


     return TSTENG_RESULT_OK;

}

static int TEST_NAF_MT7(TSTENG_handle_t handle)
{

     TEST_NAF_periodic_count = 0;
     rc.mt_testcases = 6;

     ALOGD("LBSSTELP :TEST_NAF_MT7\n");

     gpsClient_Init( &TEST_NAF_callbacks );

     gpsclient_NotificationInit(&TEST_NAF_Notification_callbacks);


     TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                         "LBSSTELP : Data callbacks successfully registered\n");



     gpsClient_ServiceStart();

     (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
     (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);


     return TSTENG_RESULT_OK;

}

static int TEST_NAF_MT8(TSTENG_handle_t handle)
{

     TEST_NAF_periodic_count = 0;
     rc.mt_testcases = 7;

     ALOGD("LBSSTELP :TEST_NAF_MT8\n");

     gpsClient_Init( &TEST_NAF_callbacks );

     gpsclient_NotificationInit(&TEST_NAF_Notification_callbacks);


     TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                         "LBSSTELP : Data callbacks successfully registered\n");



     gpsClient_ServiceStart();

     (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
     (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);


     return TSTENG_RESULT_OK;

}


static int TEST_NAF_MT9(TSTENG_handle_t handle)
{

     TEST_NAF_periodic_count = 0;
     rc.mt_testcases = 8;

     ALOGD("LBSSTELP :TEST_NAF_MT9\n");

     gpsClient_Init( &TEST_NAF_callbacks );

     gpsclient_NotificationInit(&TEST_NAF_Notification_callbacks);


     TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                         "LBSSTELP : Data callbacks successfully registered\n");



     gpsClient_ServiceStart();

     (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
     (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);


     return TSTENG_RESULT_OK;

}


static int TEST_NAF_MT10(TSTENG_handle_t handle)
{

     TEST_NAF_periodic_count = 0;
     rc.mt_testcases = 9;

     ALOGD("LBSSTELP :TEST_NAF_MT10\n");

     gpsClient_Init( &TEST_NAF_callbacks );

     gpsclient_NotificationInit(&TEST_NAF_Notification_callbacks);


     TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                         "LBSSTELP : Data callbacks successfully registered\n");



     gpsClient_ServiceStart();

     (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
     (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);


     return TSTENG_RESULT_OK;

}


static int TEST_NAF_MT11(TSTENG_handle_t handle)
{

     TEST_NAF_periodic_count = 0;
     rc.mt_testcases = 10;

     ALOGD("LBSSTELP :TEST_NAF_MT11\n");

     gpsClient_Init( &TEST_NAF_callbacks );

     gpsclient_NotificationInit(&TEST_NAF_Notification_callbacks);


     TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                         "LBSSTELP : Data callbacks successfully registered\n");



     gpsClient_ServiceStart();

     (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
     (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);


     return TSTENG_RESULT_OK;

}



/*
*Navigation Data Callback
*/

static void TEST_NAF_navigation_data_cb(t_gpsClient_NavData * location)
{
    ALOGD("LBSSTELP : TEST_NAF_navigation_data_cb\n");
    NavData_Display(location);

    if (TEST_NAF_periodic_count)
    {
       periodic_count++;
       ALOGD("LBSSTELP : Location fix obtained for Periodic\n");

       if (rc.no_count_periodic == periodic_count)
       {
         TEST_NAF_periodic_count = 0;
         periodic_count = 0;
         ALOGD("Location fix obtained for Periodic\n");
         sleep(5);
         gpsClient_Stop();
         (void) OSA_SemCount(NAFSM_TEST_END_CB);
       }
    }
    else
    {
         ALOGD("LBSSTELP : Location fix obtained for SingleShot\n");
         sleep(5);
         gpsClient_Stop();
         (void) OSA_SemCount(NAFSM_TEST_END_CB);
    }


}


/*
*Navigation Data Callback
*/

static void TEST_NAF_gen_navigation_data_cb(t_gpsClient_NavData * location)
{
    ALOGD("LBSSTELP : TEST_NAF_gen_navigation_data_cb\n");
    location = location; /*Parameter not used.This is to avoid warning for th known fact*/
    if (TEST_NAF_periodic_count)
    {
       periodic_count++;

       if (rc.no_count_periodic == periodic_count)
       {
         TEST_NAF_periodic_count = 0;
         periodic_count = 0;
         ALOGD("Location fix obtained for Periodic\n");
         sleep(5);
         gpsClient_Stop();
       }
    }
    else
    {
         ALOGD("LBSSTELP : Location fix obtained for SingleShot\n");
         sleep(5);
    }


}

static void TEST_NAF_gen_nmea_data_cb(t_gpsClient_NmeaData *nmea)
{
   nmea = nmea;/*Parameter not used.This is to avoid warning for th known fact*/
    ALOGD("LBSSTELP : TEST_NAF_gen_nmea_data_cb\n");
   if (TEST_NAF_periodic_count)
   {
     periodic_count++;
     if (rc.no_count_periodic == periodic_count)
     {
        TEST_NAF_periodic_count = 0;
        periodic_count = 0;
        ALOGD("Location fix obtained for Periodic\n");
        sleep(5);
        gpsClient_Stop();
     }
   }
     else
     {
        ALOGD("LBSSTELP : Location fix obtained for SingleShot\n");
        sleep(5);
     }



}


/*
*NMEA Data Callback
*/

static void TEST_NAF_nmea_data_cb(t_gpsClient_NmeaData *nmea)
{
    
   nmea = nmea;/*Parameter not used.This is to avoid warning for th known fact*/
    ALOGD("LBSSTELP : TEST_NAF_nmea_data_cb\n");
     if (TEST_NAF_periodic_count)
     {
        periodic_count++;

        if (rc.no_count_periodic == periodic_count)
        {
           TEST_NAF_periodic_count = 0;
           periodic_count = 0;
           ALOGD("Location fix obtained for Periodic\n");
           sleep(5);
           gpsClient_Stop();
           (void) OSA_SemCount(NAFSM_TEST_END_CB);
         }
     }
     else
     {
          ALOGD("LBSSTELP : Location fix obtained for SingleShot\n");
          sleep(5);
          gpsClient_Stop();
          (void) OSA_SemCount(NAFSM_TEST_END_CB);
     }



}

/*
*Notification Callback
*/

static void TEST_NAF_notification_cb(t_gpsClient_NotifyData *ninotify)
{

    ALOGD("LBSSTELP :TEST_NAF_notification_cb\n");

    notification_count++;

    switch(rc.mt_testcases)
    {
        case 0:
        {
            if (notification_count == 1)
            {
                gpsclient_NotificationResponse(ninotify->v_Handle,GPSCLIENT_NI_RESPONSE_ACCEPT);
            }
            else if(notification_count == 2)
            {
                gpsclient_NotificationResponse(ninotify->v_Handle,GPSCLIENT_NI_RESPONSE_DENY);

            }
            else if(notification_count == 3)
            {
                gpsclient_NotificationResponse(ninotify->v_Handle,GPSCLIENT_NI_RESPONSE_NORESP);
                notification_count = 0;
                (void) OSA_SemCount(NAFSM_TEST_END_CB);

            }
        }

        break;

        case 1:
        {
            if (notification_count == 1)
            {
                gpsclient_NotificationResponse(ninotify->v_Handle,GPSCLIENT_NI_RESPONSE_ACCEPT);
            }
            else if(notification_count == 2)
            {
                gpsclient_NotificationResponse(ninotify->v_Handle,GPSCLIENT_NI_RESPONSE_DENY);

            }
            else if(notification_count == 3)
            {
                gpsclient_NotificationResponse(ninotify->v_Handle,GPSCLIENT_NI_RESPONSE_NORESP);
                notification_count = 0;
                (void) OSA_SemCount(NAFSM_TEST_END_CB);

            }

        }

        break;

        case 2:
        {
            if (notification_count == 1)
            {
                gpsclient_NotificationResponse(ninotify->v_Handle,GPSCLIENT_NI_RESPONSE_ACCEPT);
                notification_count = 0;
                (void) OSA_SemCount(NAFSM_TEST_END_CB);
            }

        }

        break;

        case 3:
        {
            if (notification_count == 1)
            {
                gpsclient_NotificationResponse(ninotify->v_Handle,GPSCLIENT_NI_RESPONSE_ACCEPT);
                notification_count = 0;
                (void) OSA_SemCount(NAFSM_TEST_END_CB);
            }

        }

        break;

        case 4:
        {
            if (notification_count == 1)
            {
                gpsclient_NotificationResponse(ninotify->v_Handle,GPSCLIENT_NI_RESPONSE_ACCEPT);
                notification_count = 0;
                (void) OSA_SemCount(NAFSM_TEST_END_CB);
            }

        }

        break;

        case 5:
        {
            if (notification_count == 1)
            {
                gpsclient_NotificationResponse(ninotify->v_Handle,GPSCLIENT_NI_RESPONSE_ACCEPT);
            }
            else if(notification_count == 2)
            {
                gpsclient_NotificationResponse(ninotify->v_Handle,GPSCLIENT_NI_RESPONSE_DENY);

            }
            else if(notification_count == 3)
            {
                gpsclient_NotificationResponse(ninotify->v_Handle,GPSCLIENT_NI_RESPONSE_NORESP);
                notification_count = 0;
                (void) OSA_SemCount(NAFSM_TEST_END_CB);

            }

        }

        break;

        case 6:
        {
            if (notification_count == 1)
            {
                gpsclient_NotificationResponse(ninotify->v_Handle,GPSCLIENT_NI_RESPONSE_ACCEPT);
            }
            else if(notification_count == 2)
            {
                gpsclient_NotificationResponse(ninotify->v_Handle,GPSCLIENT_NI_RESPONSE_DENY);

            }
            else if(notification_count == 3)
            {
                gpsclient_NotificationResponse(ninotify->v_Handle,GPSCLIENT_NI_RESPONSE_NORESP);
                notification_count = 0;
                (void) OSA_SemCount(NAFSM_TEST_END_CB);

            }

        }

        break;

        case 7:
        {
            if (notification_count == 1)
            {
                gpsclient_NotificationResponse(ninotify->v_Handle,GPSCLIENT_NI_RESPONSE_ACCEPT);
                notification_count = 0;
                (void) OSA_SemCount(NAFSM_TEST_END_CB);
            }


        }

        break;

        case 8:
        {
            if (notification_count == 1)
            {
                gpsclient_NotificationResponse(ninotify->v_Handle,GPSCLIENT_NI_RESPONSE_ACCEPT);
                notification_count = 0;
                (void) OSA_SemCount(NAFSM_TEST_END_CB);
            }


        }

        break;

        case 9:
        {
            if (notification_count == 1)
            {
                gpsclient_NotificationResponse(ninotify->v_Handle,GPSCLIENT_NI_RESPONSE_ACCEPT);
                notification_count = 0;
                (void) OSA_SemCount(NAFSM_TEST_END_CB);
            }


        }

        break;

        case 10:
        {
            if (notification_count == 1)
            {
                gpsclient_NotificationResponse(ninotify->v_Handle,GPSCLIENT_NI_RESPONSE_ACCEPT);
                notification_count = 0;
                (void) OSA_SemCount(NAFSM_TEST_END_CB);
            }


        }

        break;

        default:
        break;

    }
    NotifyData_Display(ninotify);



}
static void NavData_Display( t_gpsClient_NavData * Navigation )

{
    ALOGD("IN NavData_Display ");

    ALOGD("LBSSTELP : Latitude: %f ",Navigation->v_Latitude);
    ALOGD("LBSSTELP : Longitude: %f ",Navigation->v_Longitude);
    ALOGD("LBSSTELP : Altitude: %f ",Navigation->v_AltitudeMSL);
    ALOGD("LBSSTELP : UTCms: %d ",Navigation->v_Milliseconds);
    ALOGD("LBSSTELP : GPS Week No: %d ",Navigation->v_GpsWeekNo);
    ALOGD("LBSSTELP : GPS TOW: %f ",Navigation->v_GpsTOW);
    ALOGD("LBSSTELP : Northing RMS: %f ",Navigation->v_NAccEst);
    ALOGD("LBSSTELP : Easting  RMS: %f ",Navigation->v_EAccEst);
    ALOGD("LBSSTELP : Vertical RMS: %f ",Navigation->v_VAccEst);
    ALOGD("LBSSTELP : Semi-major axis: %f ",Navigation->v_HAccMaj);
    ALOGD("LBSSTELP : Semi-minor axis: %f ",Navigation->v_HAccMin);
    ALOGD("LBSSTELP : Bearing: %f ",Navigation->v_HAccMajBrg);
    ALOGD("LBSSTELP : Horizontal Velocity: %f ",Navigation->v_HVelAccEst);
    ALOGD("LBSSTELP : Vertical Velocity: %f ",Navigation->v_VVelAccEst);
    ALOGD("LBSSTELP : PRResRMS: %f ",Navigation->v_PRResRMS);
    ALOGD("LBSSTELP : HDOP: %f ",Navigation->v_HDOP);
    ALOGD("LBSSTELP : VDOP: %f ",Navigation->v_VDOP);
    ALOGD("LBSSTELP : PDOP: %f ",Navigation->v_PDOP);
    ALOGD("LBSSTELP : FixType: %d ",Navigation->v_FixType);

}

static void NotifyData_Display( t_gpsClient_NotifyData * ninotify )

{
    ALOGD("IN NotifyData_Display ");
    int vl_strlen1 = 0;
    int vl_strlen2 = 0;
    int vl_strlen3 = 0;
    char *pl_buff1 = NULL;
    char *pl_buff2 = NULL;
    char *pl_buff3 = NULL;
    vl_strlen1 = sizeof(ninotify->a_RequestorId);
    vl_strlen2 = sizeof(ninotify->a_ClientName);
    vl_strlen3 = sizeof(ninotify->a_CodeWord);


    ALOGD("LBSSTELP : RequestorIdEncodingType = %d", ninotify->v_RequestorIdEncodingType);
    ALOGD("LBSSTELP : CodeWordEncodingType = %d", ninotify->v_CodeWordEncodingType);
       ALOGD("LBSSTELP : ClientNameEncodingType = %d", ninotify->v_ClientNameEncodingType);

    switch(ninotify->v_RequestorIdEncodingType)
        {
            case GPSCLIENT_UCS2:
            {
                pl_buff1 = Convert_UCS2_to_ASCII((char*)ninotify->a_RequestorId,vl_strlen1);
            }
            break;
            case GPSCLIENT_UTF8:
            {
                pl_buff1 = Convert_UTF8_to_ASCII((char*)ninotify->a_RequestorId,vl_strlen1);
            }
            break;

            default:
            break;
        }

    switch(ninotify->v_ClientNameEncodingType)
        {
            case GPSCLIENT_UCS2:
            {
                pl_buff2 = Convert_UCS2_to_ASCII((char*)ninotify->a_ClientName,vl_strlen2);
            }
            break;

            case GPSCLIENT_UTF8:
            {

                pl_buff2 = Convert_UTF8_to_ASCII((char*)ninotify->a_ClientName,vl_strlen2);
            }
            break;

            default:
            break;
        }

    switch(ninotify->v_CodeWordEncodingType)
        {
            case GPSCLIENT_UCS2:
            {
                pl_buff3 = Convert_UCS2_to_ASCII((char*)ninotify->a_CodeWord,vl_strlen3);
            }
            break;

            case GPSCLIENT_UTF8:
            {
                pl_buff3 = Convert_UTF8_to_ASCII((char*)ninotify->a_CodeWord,vl_strlen3);
            }
            break;

            default:
            break;
        }
    if ((pl_buff1!=NULL) && (pl_buff2!=NULL) && (pl_buff3!=NULL))
        ALOGD("LBSSTELP : TEST_NAF_notification_cb - a_RequestorId = %s , a_ClientName = %s ,a_CodeWord = %s , a_PhoneNumber = %s ",pl_buff1,pl_buff2,pl_buff3,ninotify->a_PhoneNumber);

    if (pl_buff1!=NULL)  OSA_Free(pl_buff1);
    if (pl_buff2!=NULL)  OSA_Free(pl_buff2);
    if (pl_buff3!=NULL)  OSA_Free(pl_buff3);
}

char * Convert_UCS2_to_ASCII(char *pl_var, int vl_strlen)
        {

            ALOGD("IN Convert_UCS2_to_ASCII ");
            int i = 0;
            int v_Length = vl_strlen / 2;

            char *pl_buff=NULL;

            pl_buff = MC_RTK_GET_MEMORY( v_Length );

            if(pl_buff!=NULL)
                {

            for( i=0 ; i < (v_Length - 1) ; i++ )
                {
                    pl_buff[i] = pl_var[ 1 + 2*i ];

                }
            pl_buff[i] = '\0';
                }
            ALOGD("UCS2 Buffer decoded  %s",pl_buff);
            return pl_buff;
        }
char * Convert_UTF8_to_ASCII(char *pl_var, int vl_strlen)
    {

        ALOGD("IN Convert_UTF8_to_ASCII ");
        char *pl_buff;

        if( '\0' != pl_var[0] )
            {
               pl_buff = MC_RTK_GET_MEMORY( vl_strlen + 1);
               memcpy( pl_buff , pl_var , vl_strlen );
               /* Null terminate the string */
               pl_buff[vl_strlen] = 0x00;
             }
        else
            {
                pl_buff = NULL;
            }
        return pl_buff;


}


/*Unused Code.Tobe deleted*/
#if 0
static void TEST_NAF_EncToAscii( t_gpsClient_NotifyData * ninotify )

{
    ALOGD("IN TEST_NAF_EncToAscii ");

    int vl_strlen1 = 0;
    int vl_strlen2 = 0;
    int vl_strlen3 = 0;
    char *pl_buff1 = NULL;
    char *pl_buff2 = NULL;
    char *pl_buff3 = NULL;
    vl_strlen1 = sizeof(ninotify->a_RequestorId);
    vl_strlen2 = sizeof(ninotify->a_ClientName);
    vl_strlen3 = sizeof(ninotify->a_CodeWord);


    ALOGD("ninotify->v_RequestorIdEncodingType = %d", ninotify->v_RequestorIdEncodingType);

    ALOGD("ninotify->v_CodeWordEncodingType = %d", ninotify->v_CodeWordEncodingType);

    switch(ninotify->v_RequestorIdEncodingType)
        {
            ALOGD("IN SWITCH TEST_NAF_EncToAscii_req");
            case GPSCLIENT_UCS2:
                {
                    ALOGD("IN SWITCH K_GPSCLIENT_UCS2_req");
                    pl_buff1 = TEST_NAF_UCS2((char*)ninotify->a_RequestorId,vl_strlen1);

                }
            break;
            case GPSCLIENT_UTF8:
                {
                    ALOGD("IN SWITCH K_GPSCLIENT_UTF8_req");
                    pl_buff1 = TEST_NAF_UTF8((char*)ninotify->a_RequestorId,vl_strlen1);

                }
            break;

            default:
            break;
        }

    switch(ninotify->v_ClientNameEncodingType)
        {
            ALOGD("IN SWITCH TEST_NAF_EncToAscii_clin");
            case GPSCLIENT_UCS2:
                {
                    ALOGD("IN SWITCH K_GPSCLIENT_UCS2_clin");
                    pl_buff2 = TEST_NAF_UCS2((char*)ninotify->a_ClientName,vl_strlen2);
                }
                break;

            case GPSCLIENT_UTF8:
                {

                    ALOGD("IN SWITCH K_GPSCLIENT_UTF8_clin");
                    pl_buff2 = TEST_NAF_UTF8((char*)ninotify->a_ClientName,vl_strlen2);
                }
                break;

            default:
            break;
        }

    switch(ninotify->v_CodeWordEncodingType)
        {
            ALOGD("IN SWITCH TEST_NAF_EncToAscii_code");
            case GPSCLIENT_UCS2:
                {
                    ALOGD("IN SWITCH K_GPSCLIENT_UCS2_code");
                    pl_buff3 = TEST_NAF_UCS2((char*)ninotify->a_CodeWord,vl_strlen3);
                }
                break;

            case GPSCLIENT_UTF8:
                {
                    ALOGD("IN SWITCH K_GPSCLIENT_UTF8_code");
                    pl_buff3 = TEST_NAF_UTF8((char*)ninotify->a_CodeWord,vl_strlen3);
                }
                break;

            default:
            break;
        }

     if ((pl_buff1!=NULL) && (pl_buff2!=NULL) && (pl_buff3!=NULL)) //Coverity
        ALOGD("LBSSTELP : TEST_NAF_notification_cb - a_RequestorId = %s , a_ClientName = %s ,a_CodeWord = %s , a_PhoneNumber = %s ",pl_buff1,pl_buff2,pl_buff3,ninotify->a_PhoneNumber);

    if (pl_buff1!=NULL)  OSA_Free(pl_buff1);
    if (pl_buff2!=NULL)  OSA_Free(pl_buff2);
    if (pl_buff3!=NULL)  OSA_Free(pl_buff3);

}

#endif

char * TEST_NAF_UCS2(char *pl_var, int vl_strlen)
{

    ALOGD("IN TEST_NAF_UCS2 ");
    int i = 0;
    int v_Length = vl_strlen / 2;

    char *pl_buff=NULL;

    pl_buff = MC_RTK_GET_MEMORY( v_Length );

    if(pl_buff!=NULL)
    {

        for( i=0 ; i < (v_Length - 1) ; i++ )
        {
            pl_buff[i] = pl_var[ 1 + 2*i ];

        }
        pl_buff[i] = '\0';
    }
    ALOGD("UCS2 Buffer decoded  %s",pl_buff);
    return pl_buff;
}

char * TEST_NAF_UTF8(char *pl_var, int vl_strlen) //Coverity
{
    char *pl_buff = NULL;
    ALOGD("IN TEST_NAF_UTF8 ");

    if( '\0' != pl_var[0] )
    {
       pl_buff = MC_RTK_GET_MEMORY( vl_strlen + 1);
       if(pl_buff!=NULL)
       {
           memcpy( pl_buff , pl_var , vl_strlen );
           /* Null terminate the string */
           pl_buff[vl_strlen] = 0x00;
       }
    }
    return pl_buff;
}
static int TEST_NAF_gen_single_shot_fix_auto(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TSTENG_BEGIN_STEP(handle, __func__);

    int outputType = 1;

    if(args_found < 6)
    {
       ALOGD("LBSSTELP : ERR Single Shot Fix - Expected 6 parameter");
       TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "LBSSTELP : Error in test arguments - Test requires 6 parameters");
       return TSTENG_RESULT_FAIL;
    }

    TEST_NAF_periodic_count = 0;

    rc = TEST_NAF_default_rc;

    gpsClient_Init( (t_gpsClient_Callbacks *)TEST_NAF_callbacks_auto );

    outputType = atoi(cmd_buf + arg_index[0]);
    rc.NMEA = atoi(cmd_buf + arg_index[1]);
    rc.time_to_first_fix_ms = atoi(cmd_buf + arg_index[2]) * 1000;
    rc.accuracy_horizontal_m = atoi(cmd_buf + arg_index[3]);
    rc.accuracy_vertical_m = atoi(cmd_buf + arg_index[4]);
    rc.age_limit_ms = atoi(cmd_buf + arg_index[5]) * 1000;

    ALOGD("LBSSTELP : QOP Argument found - %d Arguments - TTFF %d HA %d VA %d Age %d \n",args_found,atoi(cmd_buf + arg_index[2]),atoi(cmd_buf + arg_index[3]),atoi(cmd_buf + arg_index[4]),atoi(cmd_buf + arg_index[5]));
    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, "LBSSTELP : Applying command line parameters");


    rc.errorStatus = gpsClient_ServiceStart();
    if(rc.errorStatus!=GPSCLIENT_NO_ERROR)
    {
        ALOGD("StarusFrom gpsClient_ServiceStart%d\n",rc.errorStatus);
        TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL,
                        "LBSSTELP : StarusFrom gpsClient_SingleShotFix\n");
        return rc.errorStatus;
    }

    rc.NmeaData.pData = malloc (1000);
    rc.errorStatus = gpsClient_SingleShotFix( outputType, rc.NMEA, rc.time_to_first_fix_ms, rc.accuracy_horizontal_m, rc.accuracy_vertical_m, rc.age_limit_ms);
    sleep(2);
    if(rc.errorStatus!=GPSCLIENT_NO_ERROR)
    {
        ALOGD("StarusFrom gpsClient_SingleShotFix%d\n",rc.errorStatus);
        TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL,
                        "LBSSTELP : StarusFrom gpsClient_SingleShotFix\n");
        return rc.errorStatus;
    }
    else
    {
       TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : StarusFrom gpsClient_SingleShotFix\n");
       TSTENG_END_STEP(handle, __func__);
       return TSTENG_RESULT_OK;
    }
}


static int TEST_NAF_gen_periodic_fix_auto(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TSTENG_BEGIN_STEP(handle, __func__);

    int outputType = 1;
    args_found=args_found;/*Parameter not used.This is to avoid warning for th known fact*/
    TEST_NAF_periodic_count = 1;
    rc = TEST_NAF_default_rc;

    ALOGD("Inside of the TEST_NAF_gen_periodic_fix_auto\n");
    gpsClient_Init( (t_gpsClient_Callbacks *)TEST_NAF_callbacks_auto );

    outputType = atoi(cmd_buf + arg_index[0]);
    rc.NMEA = atoi(cmd_buf + arg_index[1]);
    rc.reporting_rate = atoi(cmd_buf + arg_index[2]);
    rc.no_count_periodic = atoi(cmd_buf + arg_index[3]);

    rc.errorStatus = gpsClient_ServiceStart();

    ALOGD("rc.errorstatus after the gpsClient_ServiceStart %d\n",rc.errorStatus);

    if(rc.errorStatus!=GPSCLIENT_NO_ERROR)
    {
       ALOGD("StarusFrom gpsClient_ServiceStart%d\n",rc.errorStatus);
       TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL,
                        "LBSSTELP : StarusFrom gpsClient_ServiceStar\n");
       return rc.errorStatus;
    }

    rc.NmeaData.pData = malloc (1000);

    rc.errorStatus = gpsClient_PeriodicFix( outputType, rc.NMEA, rc.reporting_rate );

    sleep(2);
    if(rc.errorStatus!=GPSCLIENT_NO_ERROR)
    {
        ALOGD("StarusFrom gpsClient_ServiceStart%d\n",rc.errorStatus);
        TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL,
                        "LBSSTELP : StarusFrom gpsClient_ServiceStar\n");
        return rc.errorStatus;
    }
    else
    {

        ALOGD("normal exit  TEST_NAF_gen_periodic_fix_auto\n");
        TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : StarusFrom gpsClient_ServiceStar\n");
        TSTENG_END_STEP(handle, __func__);
        return TSTENG_RESULT_OK;
    }
}


/* -------------------------------- callbaack for auto ------------------------------------------*/




static void TEST_NAF_gen_navigation_data_cb_auto(t_gpsClient_NavData * location)
{
    ALOGD("LBSSTELP : TEST_NAF_gen_navigation_data_cb_auto\n");
    NavData_Display(location);

    memcpy(&rc.NavData, location, sizeof (*location));

    ALOGD("LBSSTELP : 2d_fix = %d , 3d_fix =%d , fix mode= %d",location->v_Valid2DFix,location->v_Valid3DFix,location->v_FixMode);
    ALOGD("LBSSTELP : latitude = %f , longitude =%f ",location->v_Latitude,location->v_Longitude);
    rc.IsFixAvailable = TRUE;
}



//int fistFix = 0;
static void TEST_NAF_gen_nmea_data_cb_auto(t_gpsClient_NmeaData *nmea)
{
   ALOGD("LBSSTELP : TEST_NAF_gen_nmea_data_cb_auto\n");

   strcpy(rc.NmeaData.pData,nmea->pData);
   rc.NmeaData.length = nmea->length;
   rc.IsFixAvailable = TRUE;
}


static int TEST_NAF_Read_And_Execute_TestList(TSTENG_handle_t handle)
{
    SUBEGIN(handle,__func__);
    FILE *fptr = NULL;
    char TestCaseID[20],Command[30] ;
    int NumCommandsExecuted = 0;
    fptr = fopen("/etc/LbsNafTestCaseList.txt","r");

    if(fptr == NULL)
    {
       perror("Unable to Read Test case list Exiting..");
       return TSTENG_RESULT_FAIL;
    }

    ALOGD("TEST CASE LIST EXECUTION STARTED");
    while(!feof(fptr))
    {
       ++NumCommandsExecuted;
       fscanf(fptr, "%s", TestCaseID);
       ALOGD("Executing %s ",TestCaseID);
       sprintf(Command,"%s %s","NAF",TestCaseID);
       ALOGD("Sending Command %s ",Command);
       SEND(Command);

       memset(TestCaseID, 0, sizeof(TestCaseID));
       memset(Command, 0, sizeof(Command));
       vg_test_result = TSTENG_RESULT_OK;
       SEND ("NAF LBSCFGCOLD 1");
       sleep(10);
    }

    ALOGD("TEST CASE LIST EXECUTION FINISHED: Number of test executed are %d",NumCommandsExecuted);
    fclose(fptr);
    SUBEND(handle,__func__);
    return TSTENG_RESULT_OK;

}

static int TEST_NAF_Wait_For_Fix(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    TSTENG_BEGIN_STEP(handle, __func__);
    
    if(args_found < 1)
    {
        ALOGD("LBSSTELP : ERR  Expected 1 parameter");
        TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "LBSSTELP : Error in test arguments - Test requires 1 parameters");
        return TSTENG_RESULT_FAIL;
    }
    
    int vl_periodic = atoi(cmd_buf + arg_index[0]);
    int vl_max_time = atoi(cmd_buf + arg_index[1]);
    int vl_fix_type = atoi(cmd_buf + arg_index[2]);
    
    int vl_ret = TSTENG_RESULT_OK;
    int vl_recHorAcc = 0;
    int vl_reportingPeriod = vl_max_time;

    ALOGD("LBSSTELP : TEST_NAF_Wait_For_Fix Enter: vl_max_time = %d vl_periodic = %d vl_fix_type = %d,IsFixAvailable %d", \
           vl_max_time, \
           vl_periodic, \
           vl_fix_type,\
           rc.IsFixAvailable);

    while(vl_max_time > 3)
    {
        sleep(1);

        ALOGD("LBSSTELP : TEST_NAF_Wait_For_Fix : insideloop  vl_max_time =%d",vl_max_time);
        vl_max_time = vl_max_time -1 ;

    }

    if(vl_max_time == 0)
    {
        ALOGD("LBSSTELP : TEST_NAF_Wait_For_Fix : Maximum time expired ");

        TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL,
                         "LBSSTELP : Fix callbacks failure\n");

        if((vl_fix_type!= 0) || (vl_periodic == 1))
        {
            vl_ret = TSTENG_RESULT_FAIL;
            goto End ;
        }

    }
    if(vl_periodic == 1)//periodic
    {
          ALOGD("LBSSTELP : TEST_NAF_Wait_For_Fix : insideloop  vl_max_time =%d rc.reporting_rate = %ld",vl_max_time,rc.reporting_rate);
          if(((rc.reporting_rate/1000) - (vl_reportingPeriod - vl_max_time)) <= 2)
          {
             ALOGD("LBSSTELP : TEST_NAF_Wait_For_Fix : Obtained Periodoc Fix ");
             TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                              "LBSSTELP : Fix callbacks success\n");
          }
          else
          {
             ALOGD("LBSSTELP : TEST_NAF_Wait_For_Fix : Obtained periodic Fix but at incorrect reporting rate");
               TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL,
                                "LBSSTELP : Fix callbacks success\n");
               vl_ret = TSTENG_RESULT_FAIL;

          }

    }
    else //Single shot fix case only
    {

        if(vl_fix_type == 0)
        {
            //case where fix was not required and we have not got a fix and timout has happned
            TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                                "LBSSTELP : Fix callbacks success\n");
            goto End;
        }

        if(rc.NavData.v_FixType == vl_fix_type)   /**< Position Fix Type 0=No fix, 1=Estimated, 2=2D, 3=2D Differential, 4=3D, 5=3D Differential */
        {

            vl_recHorAcc = TEST_NAF_CONVERT_RMSVAL_TO_HORIZONTAL_ACC(rc.NavData.v_NAccEst,rc.NavData.v_EAccEst);
            ALOGD("Received Horizontal and vertical accuracy is %d %f",vl_recHorAcc,rc.NavData.v_VAccEst);
            if(vl_recHorAcc <= rc.accuracy_horizontal_m && rc.NavData.v_VAccEst <= rc.accuracy_vertical_m)
            {
               ALOGD("LBSSTELP : TEST_NAF_Wait_For_Fix : Obtained single shot Fix ");
               TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                                "LBSSTELP : Fix callbacks success\n");
            }
            else
            {
               ALOGD("LBSSTELP : TEST_NAF_Wait_For_Fix : Obtained single shot Fix but not with required accuracy");
               TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL,
                                "LBSSTELP : Fix callbacks success\n");
               vl_ret = TSTENG_RESULT_FAIL;

            }

        }
        else
        {
            ALOGD("LBSSTELP : TEST_NAF_Wait_For_Fix : Fix not obtained for single shot ");

            TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL,
                             "LBSSTELP : Fix callbacks failure\n");
            vl_ret = TSTENG_RESULT_FAIL;

        }

    }

    End:
    rc.IsFixAvailable = FALSE ;
    ALOGD("LBSSTELP : TEST_NAF_Wait_For_Fix Exit ");
    TSTENG_END_STEP(handle, __func__);
    return vl_ret;

}


/* Function use to check error status GPS client or from status callback from CGPS */

static int LBSSTATUS(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    uint8_t status =0;

    if(args_found < 1)
    {
        ALOGD("LBSSTELP : ERR Set Start Mode - Expected 1 parameter");
        TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL, "LBSSTELP : Error in test arguments - Test requires 1 parameters");
        return TSTENG_RESULT_FAIL;
    }

    status = atoi(cmd_buf + arg_index[0]);

    ALOGD("LBSSTELP : inside LBSSTATUS rc.errorStatus= %d",rc.errorStatus);

    if(rc.errorStatus != status)
    {

        ALOGD("LBSSTELP : inside LBSSTATUS as TSTENG_RESULT_FAIL");
        TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL,
                        "LBSSTELP : LBSSTATUS\n");
        return TSTENG_RESULT_FAIL;
    }
    else
    {

       ALOGD("LBSSTELP : inside LBSSTATUS as TSTENG_RESULT_OK");
       TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : LBSSTATUS \n");
       return TSTENG_RESULT_OK;
    }

}


/*-------------------------------------------------------------------------------------------
* Description: Handle PSTE Messages
*
*--------------------------------------------------------------------------------------------*/

static int LBSNMEA(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    TSTENG_BEGIN_STEP(handle, __func__);
    int vl_max_time = atoi(cmd_buf + arg_index[0]);
    char *vl_PSTE = cmd_buf + arg_index[1];
    int vl_PSTECommand = atoi(cmd_buf + arg_index[2]);
    int vl_ret = TSTENG_RESULT_OK;
    char vl_PSTEMessage[10];
    bool vl_PSTEAvailable = FALSE;
    int compare;
    
    args_found=args_found;/*Parameter not used.This is to avoid warning for th known fact*/
    sprintf(vl_PSTEMessage, "%s,%d",vl_PSTE,vl_PSTECommand);
    vl_max_time *= 5;

    while(vl_max_time)
    {
        usleep(200000);
        vl_max_time--;
        if((rc.NmeaData.length > 0) && (rc.IsFixAvailable == TRUE) )
        {
            if ((compare = strncmp(rc.NmeaData.pData ,vl_PSTEMessage,7)) == 0)
            {
                ALOGD("PSTE Data %s\n",(char*)rc.NmeaData.pData);
                vl_PSTEAvailable = TRUE;
                break;
            }
        }
    }

    if(vl_PSTEAvailable)
    {
        vl_ret = TSTENG_RESULT_OK;
    }
    else
    {
        vl_ret = TSTENG_RESULT_FAIL;
    }


    if(vl_ret == TSTENG_RESULT_FAIL)
    {

        ALOGD("LBSSTELP : inside LBSNMEA as TSTENG_RESULT_FAIL");
        TSTENG_test_info_cb(handle, TSTENG_RESULT_FAIL,
                        "LBSSTELP : LBSNMEA\n");
        TSTENG_END_STEP(handle, __func__);
        return TSTENG_RESULT_FAIL;
    }
    else
    {

       ALOGD("LBSSTELP : inside LBSNMEA as TSTENG_RESULT_OK");
       TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : LBSNMEA \n");
       TSTENG_END_STEP(handle, __func__);
       return TSTENG_RESULT_OK;
    }
    TSTENG_END_STEP(handle, __func__);
    return TSTENG_RESULT_OK;
}




/*-------------------------------------------------------------------------------------------
*Test Description:circle, single shot fix,position inside,longitude out of range.
*
*Pass/Fail Criteria:  pass  status: Invalid parameters.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFGEO001(TSTENG_handle_t handle)
    {

       TCBEGIN(handle, __func__) ;


       SEND("NAF LBSGEOFENCE 0 1 60000 0 0 100 150 8 1 0 500 1209661 0 36208840 2 2 ");
       RECV("NAF LBSSTATUS 1");
       SEND("NAF LBSSTOP");


       TCEND(handle, __func__) ;
       return TSTENG_RESULT_OK;

    }

/*-------------------------------------------------------------------------------------------
*Test Description:circle, periodic fix,position inside,longitude out of range.
*
*Pass/Fail Criteria:  pass  status: Invalid parameters.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFGEO002(TSTENG_handle_t handle)
    {

        TCBEGIN(handle, __func__) ;

        SEND("NAF LBSGEOFENCE 0 6 30 240 60000 100 150 8 1 0 500 1209661 0 36208840 2 2 ");
        RECV("NAF LBSSTATUS 1");
        SEND("NAF LBSSTOP");


        TCEND(handle, __func__) ;
        return TSTENG_RESULT_OK;

    }

/*-------------------------------------------------------------------------------------------
*Test Description: circle,single shot fix,position inside.
*
*Pass/Fail Criteria:  pass  status: reference point inside.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFGEO003(TSTENG_handle_t handle)
    {

        TCBEGIN(handle, __func__) ;


        SEND("NAF LBSGEOFENCE 0 1 60000 0 0 100 150 8 1 0 500 1209661 0 3620884 2 2 ");
        RECV("NAF LBSSTATUS 0");
        SEND ("NAF LBSPOS 0 90 4");
        SEND("NAF LBSSTOP");


        TCEND(handle, __func__) ;
        return TSTENG_RESULT_OK;

    }


/*-------------------------------------------------------------------------------------------
*Test Description: circle,periodic fix,position inside.
*
*Pass/Fail Criteria:  pass  status: reference point inside.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFGEO004(TSTENG_handle_t handle)
    {

        TCBEGIN(handle, __func__) ;

        SEND("NAF LBSGEOFENCE 0 6 30 240 60000 100 150 8 1 0 500 3324374 0 6512822 2 2 ");
        RECV("NAF LBSSTATUS 0");
        SEND("NAF LBSSTOP");

        TCEND(handle, __func__) ;
        return TSTENG_RESULT_OK;

    }


/*-------------------------------------------------------------------------------------------
*Test Description: circle,periodic fix,position outside.
*
*Pass/Fail Criteria:  pass  status: reference point outside.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFGEO005(TSTENG_handle_t handle)
    {

        TCBEGIN(handle, __func__) ;

        SEND("NAF LBSGEOFENCE 0 6 30 240 60000 100 150 8 2 0 500  1209470 0 3621150 2 2 ");
        RECV("NAF LBSSTATUS 0");
        SEND("NAF LBSSTOP");

        TCEND(handle, __func__) ;
        return TSTENG_RESULT_OK;

    }

/*-------------------------------------------------------------------------------------------
*Test Description: circle,single shotfix,position outside.
*
*Pass/Fail Criteria:  pass  status: reference point outside.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFGEO006(TSTENG_handle_t handle)
    {

        TCBEGIN(handle, __func__) ;

        SEND("NAF LBSGEOFENCE 0 1 60000 0 0 100 150 8 2 0 500 1209470 0 3621150 2 2 ");
        RECV("NAF LBSSTATUS 0");
        SEND ("NAF LBSPOS 0 90 4");
        SEND("NAF LBSSTOP");

        TCEND(handle, __func__) ;
        return TSTENG_RESULT_OK;

    }

/*-------------------------------------------------------------------------------------------
*Test Description: circle,periodic fix,position boundary/inside.
*
*Pass/Fail Criteria:  pass  status: reference point inside.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFGEO007(TSTENG_handle_t handle)
    {

        TCBEGIN(handle, __func__) ;

        SEND("NAF LBSGEOFENCE 0 6 30 240 60000 100 150 8 1 0 500 1209727 0 3620578 2 2 ");
        RECV("NAF LBSSTATUS 0");
        SEND("NAF LBSSTOP");

        TCEND(handle, __func__) ;
        return TSTENG_RESULT_OK;

    }


/*-------------------------------------------------------------------------------------------
*Test Description: polygon,periodic fix,position outside.
*
*Pass/Fail Criteria:  pass  status:no error.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFGEO008(TSTENG_handle_t handle)
    {

        TCBEGIN(handle, __func__) ;


        SEND("NAF LBSGEOFENCE 0 6 30 240 60000 100 150 8 2 2 8 1210040 1210016 1209913 1210035 1209745 1209764 1209922 1209706 0 0 0 0 0 0 0 0 3620562 3620817 3620866 3620888 3620872 3620736 3620692 3620671 1 ");
        RECV("NAF LBSSTATUS 0");
        SEND("NAF LBSSTOP");

        TCEND(handle, __func__) ;
        return TSTENG_RESULT_OK;

    }

/*-------------------------------------------------------------------------------------------
*Test Description: polygon,periodic fix,longitude out of range.
*
*Pass/Fail Criteria:  pass  status:invalid parameters.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFGEO009(TSTENG_handle_t handle)
    {

        TCBEGIN(handle, __func__) ;


        SEND("NAF LBSGEOFENCE 0 6 30 240 60000 100 150 8 2 2 8 1210040 1210015 1209912 1210035 1209744 1209764 1209922 1209706 0  0 0 0   0   0 0 0 36205611 36208161 3620865 3620888 3620871 3620736 3620692 3620670 1 ");
        RECV("NAF LBSSTATUS 1");
        SEND("NAF LBSSTOP");

        TCEND(handle, __func__) ;
        return TSTENG_RESULT_OK;

    }



/*-------------------------------------------------------------------------------------------
*Test Description: polygon,periodic fix,point inside.
*
*Pass/Fail Criteria:  pass  status:reference point inside polygon.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFGEO010(TSTENG_handle_t handle)
    {

        TCBEGIN(handle, __func__) ;


        SEND("NAF LBSGEOFENCE 0 6 30 240 60000 100 150 8 1 2 8 1210040 1210016 1209913 1210035  1209513 1209764 1209922 1209706 0 0 0 0   0   0 0 0 3620562 3620817 3620865 3620888  3620860 3620736 3620692 3620670 1 ");
        RECV("NAF LBSSTATUS 0");
        SEND("NAF LBSSTOP");

        TCEND(handle, __func__) ;
        return TSTENG_RESULT_OK;

    }


/*-------------------------------------------------------------------------------------------
*Test Description: polygon,periodic fix,boundary/in.
*
*Pass/Fail Criteria:  pass  status:reference point inside polygon.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFGEO011(TSTENG_handle_t handle)
    {

        TCBEGIN(handle, __func__) ;


        SEND("NAF LBSGEOFENCE 0 6 30 240 60000 100 150 8 1 2 8 1210039 1210015 1209912 1210035 1209685 1209764 1209922 1209705 0 0 0 0   0   0 0 0 3620561 3620816 3620865 3620887 3620855 3620736 3620692 3620670 1 ");
        RECV("NAF LBSSTATUS 0");
        SEND("NAF LBSSTOP");

        TCEND(handle, __func__) ;
        return TSTENG_RESULT_OK;

    }



/*-------------------------------------------------------------------------------------------
*Test Description: polygon,single shot fix,position outside.
*
*Pass/Fail Criteria:  pass  status:reference point outside polygon.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFGEO012(TSTENG_handle_t handle)
    {

        TCBEGIN(handle, __func__) ;


        SEND("NAF LBSGEOFENCE 0 1 60000 0 0 100 150 8 2 2 8 1210040 1210016 1209913 1210035 1209745 1209764 1209922 1209706 0 0 0 0 0 0 0 0 3620562 3620817 3620866 3620888 3620872 3620736 3620692 3620671 1 ");
        RECV("NAF LBSSTATUS 0");
        SEND ("NAF LBSPOS 0 90 4");
        SEND("NAF LBSSTOP");

        TCEND(handle, __func__) ;
        return TSTENG_RESULT_OK;

    }

/*-------------------------------------------------------------------------------------------
*Test Description: polygon,single shot fix,longitude out of range.
*
*Pass/Fail Criteria:  pass  status:invalid parameters.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFGEO013(TSTENG_handle_t handle)
    {

        TCBEGIN(handle, __func__) ;


        SEND("NAF LBSGEOFENCE 0 1 60000 0 0 100 150 8 1 2 8 1210039 1210015 1209912 1210035 1209744 1209764 1209922 1209705 0  0 0 0   0   0 0 0 3620561 3620816 3620865 36208871 3620871 3620736 3620692 3620670 1 ");
        RECV("NAF LBSSTATUS 1");
        SEND("NAF LBSSTOP");

        TCEND(handle, __func__) ;
        return TSTENG_RESULT_OK;

    }

/*-------------------------------------------------------------------------------------------
*Test Description: polygon,single shot fix,inside.
*
*Pass/Fail Criteria:  pass  status:reference point inside polygon.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFGEO014(TSTENG_handle_t handle)
    {

        TCBEGIN(handle, __func__) ;


        SEND("NAF LBSGEOFENCE 0 1 60000 0 0 100 150 8 1 2 8 1210040 1210016 1209913 1210035  1209513 1209764 1209922 1209706 0 0 0 0   0   0 0 0 3620562 3620817 3620865 3620888  3620860 3620736 3620692 3620670 1 ");
        RECV("NAF LBSSTATUS 0");
        SEND("NAF LBSPOS 0 90 4");
        SEND("NAF LBSSTOP");

        TCEND(handle, __func__) ;
        return TSTENG_RESULT_OK;

    }


/*-------------------------------------------------------------------------------------------
*Test Description: polygon,single shot fix,boundary/in.
*
*Pass/Fail Criteria:  pass  status:reference point inside polygon..
*--------------------------------------------------------------------------------------------*/


static int LBSNAFGEO015(TSTENG_handle_t handle)
    {

        TCBEGIN(handle, __func__) ;


        SEND("NAF LBSGEOFENCE 0 1 60000 0 0 100 150 8 1 2 8 1210039 1210015 1209912 1210035 1209684 1209764 1209922 1209705 0 0 0 0   0   0 0 0 3620561 3620816 3620865 3620887  3620854 3620736 3620692 3620670 1 ");
        RECV("NAF LBSSTATUS 0");
        SEND ("NAF LBSPOS 0 90 4");
        SEND("NAF LBSSTOP");

        TCEND(handle, __func__) ;
        return TSTENG_RESULT_OK;

    }

/*-------------------------------------------------------------------------------------------
*Test Description: polygon,periodic fix, 2 points.
*
*Pass/Fail Criteria:  pass  status:invalid parameter.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFGEO016(TSTENG_handle_t handle)
    {

        TCBEGIN(handle, __func__) ;


       SEND("NAF LBSGEOFENCE 0 6 30 240 60000 100 150 8 2 2 2 1210039 1210015 0 0  3620561 3620816 1 ");
       RECV("NAF LBSSTATUS 1");

       SEND("NAF LBSSTOP");


        TCEND(handle, __func__) ;
        return TSTENG_RESULT_OK;

    }
/*-------------------------------------------------------------------------------------------
*Test Description: polygon,periodic fix,inside,5 points.
*
*Pass/Fail Criteria:  pass  status:reference point inside polygon.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFGEO017(TSTENG_handle_t handle)
    {

        TCBEGIN(handle, __func__) ;


       SEND("NAF LBSGEOFENCE 0 6 30 240 60000 100 150 8 1 2 5 1210040 1210015 1209912   1209512 1209705 0 0 0 0   0  3620561 3620816 3620865   3620860  3620670 1 ");
       RECV("NAF LBSSTATUS 0");

       SEND("NAF LBSSTOP");


        TCEND(handle, __func__) ;
        return TSTENG_RESULT_OK;

    }


/*-------------------------------------------------------------------------------------------
*Test Description: polygon,single shot fix,2 points.
*
*Pass/Fail Criteria:  pass  status: invalid parameter.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFGEO018(TSTENG_handle_t handle)
    {

        TCBEGIN(handle, __func__) ;


       SEND("NAF LBSGEOFENCE 0 1 60000 0 0 100 150 8 2 2 2 1210040 1210015  0 0  3620561 3620816 1 ");
       RECV("NAF LBSSTATUS 1");
       SEND("NAF LBSSTOP");



        TCEND(handle, __func__) ;
        return TSTENG_RESULT_OK;

    }

/*-------------------------------------------------------------------------------------------
*Test Description: polygon,single shot fix,inside,5 points.
*
*Pass/Fail Criteria:  pass  status:reference point inside polygon.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFGEO019(TSTENG_handle_t handle)
    {

        TCBEGIN(handle, __func__) ;


       SEND("NAF LBSGEOFENCE 0 1 60000 0 0 100 150 8 1 2 5 1210039 1210015 1209912   1209512 1209705 0 0 0 0  0 3620561 3620816 3620865   3620860  3620670 1");
       RECV("NAF LBSSTATUS 0");
       SEND ("NAF LBSPOS 0 90 4");
       SEND("NAF LBSSTOP");


        TCEND(handle, __func__) ;
        return TSTENG_RESULT_OK;

    }

/*-------------------------------------------------------------------------------------------
*Test Description: polygon,single shot fix,outside,5 points.
*
*Pass/Fail Criteria:  pass  status:reference point outside polygon.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFGEO020(TSTENG_handle_t handle)
    {

      TCBEGIN(handle, __func__) ;
      SEND("NAF LBSGEOFENCE 0 1 60000 0 0 100 150 8 2 2 5 1210040 1210016  1209764 1209922 1209705 0 0 0 0 0 3620562 3620816  3620736 3620692 3620670 1");
      RECV("NAF LBSSTATUS 0");
      SEND ("NAF LBSPOS 0 90 4");
      SEND("NAF LBSSTOP");


        TCEND(handle, __func__) ;
        return TSTENG_RESULT_OK;
    }

    /*-------------------------------------------------------------------------------------------
    *Test Description: polygon,single shot fix,position outside.
    *
    *Pass/Fail Criteria:  pass  status:reference point outside polygon.
    *--------------------------------------------------------------------------------------------*/

static int LBSNAFGEO021(TSTENG_handle_t handle)
        {

            TCBEGIN(handle, __func__) ;


           SEND("NAF LBSGEOFENCE 0 1 60000 0 0 100 150 8 2 2 8 1210040 1210016 1209913 1210035 1209513 1209829 1209922 1209706 0 0 0 0 0 0 0 0 3620562 3620817 3620866 3620888 3620860 3620745 3620692 3620671 1 ");
           RECV("NAF LBSSTATUS 0");
           SEND ("NAF LBSPOS 0 90 4");
           SEND("NAF LBSSTOP");


            TCEND(handle, __func__) ;
            return TSTENG_RESULT_OK;

        }


/*-------------------------------------------------------------------------------------------
*Test Description: circle,periodic fix,position outside/boundary.
*
*Pass/Fail Criteria:  pass  status: reference point outside.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFGEO022(TSTENG_handle_t handle)
    {

        TCBEGIN(handle, __func__) ;



       SEND("NAF LBSGEOFENCE 0 6 30 240 60000 100 150 8 2 0 500  1209525 0 3620994 2 2 ");
       RECV("NAF LBSSTATUS 0");

       SEND("NAF LBSSTOP");


        TCEND(handle, __func__) ;
        return TSTENG_RESULT_OK;

    }

/*-------------------------------------------------------------------------------------------
*Test Description: circle,periodic fix,position inside.
*
*Pass/Fail Criteria:  pass  status: reference point inside.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFGEO023(TSTENG_handle_t handle)
    {

        TCBEGIN(handle, __func__) ;



       SEND("NAF LBSGEOFENCE 0 6 30 240 60000 100 150 8 1 0 1000  1209525 0 3620994 2 2 ");
       RECV("NAF LBSSTATUS 0");

       SEND("NAF LBSSTOP");


        TCEND(handle, __func__) ;
        return TSTENG_RESULT_OK;

    }

/*-------------------------------------------------------------------------------------------
*Test Description: circle,single shotfix,position outside.
*
*Pass/Fail Criteria:  pass  status: reference point outside.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFGEO024(TSTENG_handle_t handle)
    {

        TCBEGIN(handle, __func__) ;


       SEND("NAF LBSGEOFENCE 0 1 60000 0 0 100 150 8 2 0 1000 1209223 0 3621168 2 2 ");
       RECV("NAF LBSSTATUS 0");
       SEND ("NAF LBSPOS 0 90 4");
       SEND("NAF LBSSTOP");


        TCEND(handle, __func__) ;
        return TSTENG_RESULT_OK;

    }

/*-------------------------------------------------------------------------------------------
*Test Description: polygon,single shot fix,inside,15 points.
*
*Pass/Fail Criteria:  pass  status:reference point inside polygon.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFGEO025(TSTENG_handle_t handle)
    {

      TCBEGIN(handle, __func__) ;
      SEND("NAF LBSGEOFENCE 0 1 60000 0 0 100 150 8 1 2 15 1202367 1202833  1203299 1203765 1204231 1204697  1205163 1205629 1209824 1209824 1205163  1204697 1204231 1203765 1203299   0        0        0        0       0       0       0       0       0       0       0       0         0       0      0   3616888 3616422  3616888 3616422 3616888 3616422 3616888  3616422 3616888 3621548 3621315  3621548 3621315 3621548 3621315  1");
      RECV("NAF LBSSTATUS 0");

      SEND ("NAF LBSPOS 0 90 4");
      SEND("NAF LBSSTOP");


        TCEND(handle, __func__) ;
        return TSTENG_RESULT_OK;
    }


/*-------------------------------------------------------------------------------------------
*Test Description: ellipse,single shotfix,position inside.
*
*Pass/Fail Criteria:  pass  status: reference point inside(at center).
*--------------------------------------------------------------------------------------------*/


static int LBSNAFGEO026(TSTENG_handle_t handle)
    {

        TCBEGIN(handle, __func__) ;


       SEND("NAF LBSGEOFENCE 0 1 60000 0 0 100 150 8 1 1 60 500 250 1209731 0 3620790 0 0 0 0");
       RECV("NAF LBSSTATUS 0");

       SEND("NAF LBSSTOP");


        TCEND(handle, __func__) ;
        return TSTENG_RESULT_OK;

    }

/*-------------------------------------------------------------------------------------------
*Test Description: ellipse,single shotfix,position inside.
*
*Pass/Fail Criteria:  pass  status: reference point inside(at foci).
*--------------------------------------------------------------------------------------------*/


static int LBSNAFGEO027(TSTENG_handle_t handle)
    {

        TCBEGIN(handle, __func__) ;


       SEND("NAF LBSGEOFENCE 0 1 60000 0 0 100 150 8 1 1 60 500 250 1209911 0 3620949 0 0 0 0");
       RECV("NAF LBSSTATUS 0");

       SEND("NAF LBSSTOP");


        TCEND(handle, __func__) ;
        return TSTENG_RESULT_OK;

    }

/*-------------------------------------------------------------------------------------------
*Test Description: circle,periodic fix,position entering.
*
*Pass/Fail Criteria:  pass  status: reference point inside.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFGEO028(TSTENG_handle_t handle)
    {

        TCBEGIN(handle, __func__) ;

        SEND("NAF LBSGEOFENCE 0 6 30 240 60000 100 150 8 0 0 500 3324374 0 6512822 2 2 ");
        RECV("NAF LBSSTATUS 0");
        SEND("NAF LBSSTOP");

        TCEND(handle, __func__) ;
        return TSTENG_RESULT_OK;

    }


/*-------------------------------------------------------------------------------------------
*Test Description: circle,periodic fix,position inside.
*
*Pass/Fail Criteria:  pass  status: reference point inside.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFGEO030(TSTENG_handle_t handle)
    {

        TCBEGIN(handle, __func__) ;

        SEND("NAF LBSGEOFENCE 0 6 30 120 60000 100 150 8 1 0 500 3324374 0 6512822 2 2 ");
        RECV("NAF LBSSTATUS 0");
        SEND("NAF LBSSTOP");

        TCEND(handle, __func__) ;
        return TSTENG_RESULT_OK;

    }


/*-------------------------------------------------------------------------------------------
*Test Description: circle,periodic fix,position outside.
*
*Pass/Fail Criteria:  pass  status: reference point outside.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFGEO029(TSTENG_handle_t handle)
    {

        TCBEGIN(handle, __func__) ;

        SEND("NAF LBSGEOFENCE 0 6 30 120 60000 100 150 8 2 0 500  1209470 0 3621150 2 2 ");
        RECV("NAF LBSSTATUS 0");
        SEND("NAF LBSSTOP");

        TCEND(handle, __func__) ;
        return TSTENG_RESULT_OK;

    }

/*-------------------------------------------------------------------------------------------
*Test Description: circle,periodic fix,position leaving.
*
*Pass/Fail Criteria:  pass  status: reference point outside.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFGEO031(TSTENG_handle_t handle)
    {

        TCBEGIN(handle, __func__) ;

        SEND("NAF LBSGEOFENCE 0 6 30 240 60000 100 150 8 3 0 500  1209470 0 3621150 2 2 ");
        RECV("NAF LBSSTATUS 0");
        SEND("NAF LBSSTOP");

        TCEND(handle, __func__) ;
        return TSTENG_RESULT_OK;

    }



/*-------------------------------------------------------------------------------------------
*Test Description: Start and stop GPS 1000 times.
*
*Pass/Fail Criteria:  pass   Service must not crash. and fix should be obtained.
*--------------------------------------------------------------------------------------------*/

static int LBSNAFINT0001(TSTENG_handle_t handle)
{
    int i;

    TCBEGIN(handle, __func__) ;

    for(i=0;i<1000;i++)
    {

       SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
       RECV("NAF LBSSTATUS 0");

       SEND ("NAF LBSPOS 0 52 4");
       SEND("NAF LBSSTOP");
       sleep(2);
       if(vg_test_result == TSTENG_RESULT_FAIL)
       {
          ALOGD("Session number %d failed, resetting result flag..",i+1);
          vg_test_result = TSTENG_RESULT_OK;
       }

    }

    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}


/*-------------------------------------------------------------------------------------------
*Test Description: Start and stop GPS 1000 times.
*
*Pass/Fail Criteria:  pass  Service must not crash. whether fix is obtained or not.
*--------------------------------------------------------------------------------------------*/

static int LBSNAFINT0002(TSTENG_handle_t handle)
{
    int i;

    TCBEGIN(handle, __func__) ;

    for(i=0;i<1000;i++)
    {

       SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
       RECV("NAF LBSSTATUS 0");

       SEND ("NAF LBSPOS 0 10 0");
       SEND("NAF LBSSTOP");
       sleep(2);
       if(vg_test_result == TSTENG_RESULT_FAIL)
       {
          ALOGD("Session number %d failed, resetting result flag..",i+1);
          vg_test_result = TSTENG_RESULT_OK;
       }

    }

    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}



/*-------------------------------------------------------------------------------------------
*Test Description: 100 cold start sessions.
*
*Pass/Fail Criteria:  pass  Service must not crash. Fix is obtained for every iteration.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFINT0003(TSTENG_handle_t handle)
{

   int i;

   TCBEGIN(handle, __func__) ;
   for(i=0;i<100;i++)
   {

      SEND ("NAF LBSCFGCOLD 1");

      ALOGD("LBSSTELP : Cold start mode\n");
      sleep(5);
      SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
      RECV("NAF LBSSTATUS 0");

      SEND ("NAF LBSPOS 0 60 4");
      SEND("NAF LBSSTOP");
      sleep(2);
      if(vg_test_result == TSTENG_RESULT_FAIL)
      {
         ALOGD("Session number %d failed, resetting result flag..",i+1);
         vg_test_result = TSTENG_RESULT_OK;
      }

   }
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;

}




/*-------------------------------------------------------------------------------------------
*Test Description: 100 Warm start sessions.
*
*Pass/Fail Criteria:  pass  Service must not crash. Fix is obtained for every iteration.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFINT0004(TSTENG_handle_t handle)
{


   int i;

   TCBEGIN(handle, __func__) ;
   for(i=0;i<100;i++)
   {

      SEND ("NAF LBSCFGCOLD 3");

      ALOGD("LBSSTELP : Warm start mode\n");
      sleep(5);
      SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
      RECV("NAF LBSSTATUS 0");

      SEND ("NAF LBSPOS 0 60 4");
      SEND("NAF LBSSTOP");
      sleep(2);
      if(vg_test_result == TSTENG_RESULT_FAIL)
      {
         ALOGD("Session number %d failed, resetting result flag..",i+1);
         vg_test_result = TSTENG_RESULT_OK;
      }

   }
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;


}





/*-------------------------------------------------------------------------------------------
*Test Description: 100 Hot start sessions.
*
*Pass/Fail Criteria:  pass  Service must not crash. Fix is obtained for every iteration.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFINT0005(TSTENG_handle_t handle)
{


   int i;

   TCBEGIN(handle, __func__) ;
   for(i=0;i<100;i++)
   {

      SEND ("NAF LBSCFGCOLD 4");

      ALOGD("LBSSTELP : Warm start mode\n");
      sleep(5);
      SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
      RECV("NAF LBSSTATUS 0");

      SEND ("NAF LBSPOS 0 60 4");
      SEND("NAF LBSSTOP");
      sleep(2);
      if(vg_test_result == TSTENG_RESULT_FAIL)
      {
         ALOGD("Session number %d failed, resetting result flag..",i+1);
         vg_test_result = TSTENG_RESULT_OK;
      }

   }
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;


}




/*-------------------------------------------------------------------------------------------
*Test Description: Register for a periodic fix with Output Type = 0 and all other parameters set to platform default.
*
*Pass/Fail : Periodic fix must be returned and fix information should be in NMEA format.
*--------------------------------------------------------------------------------------------*/

static int LBSNAFINT0010(TSTENG_handle_t handle)
{


    TCBEGIN(handle, __func__) ;
    SEND("NAF LBSSTARTPFA 0 1023 3000 5");
    RECV("NAF LBSSTATUS 0") ;
    SEND ("NAF LBSPOS 1 4 0");
    SEND ("NAF LBSPOS 1 4 0");
    SEND ("NAF LBSPOS 1 4 0");
    SEND ("NAF LBSPOS 1 4 0");
    SEND ("NAF LBSPOS 1 4 0");

    SEND("NAF LBSSTOP");
    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}



/*-------------------------------------------------------------------------------------------
*Test Description: Register for a periodic fix with Output Type = 1 and all other parameters set to platform default.
*
*Pass/Fail : Periodic fix must be returned and fix information should be in C struct format.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFINT0011(TSTENG_handle_t handle)
{

    TCBEGIN(handle, __func__) ;
    SEND("NAF LBSSTARTPFA 1 0 3000 5");
    RECV("NAF LBSSTATUS 0") ;
    SEND ("NAF LBSPOS 1 4 0");
    SEND ("NAF LBSPOS 1 4 0");
    SEND ("NAF LBSPOS 1 4 0");
    SEND ("NAF LBSPOS 1 4 0");
    SEND ("NAF LBSPOS 1 4 0");

    SEND("NAF LBSSTOP");
    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}

/*-------------------------------------------------------------------------------------------
*Test Description: Register for a periodic fix with Output Type = 255 and all other parameters set to platform default.
*
*Pass/Fail : Periodic fix must be returned and fix information should be in C struct format.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFINT0012(TSTENG_handle_t handle)
{

    TCBEGIN(handle, __func__) ;
    SEND("NAF LBSSTARTPFA 255 0 3000 5");
    RECV("NAF LBSSTATUS 1") ;

    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;
}


/*-------------------------------------------------------------------------------------------
*Test Description: Register for a periodic fix with Output Type = 0,Fix Rate  = 0 and all other parameters
*set to platform default.
*
*Pass/Fail : Periodic fix must be returned at platform default rate,
*--------------------------------------------------------------------------------------------*/


static int LBSNAFINT0013(TSTENG_handle_t handle)
{


    TCBEGIN(handle, __func__) ;
    SEND("NAF LBSSTARTPFA 0 2000 0 5");
    RECV("NAF LBSSTATUS 0") ;
    SEND ("NAF LBSPOS 1 2 0");
    SEND ("NAF LBSPOS 1 2 0");
    SEND ("NAF LBSPOS 1 2 0");
    SEND ("NAF LBSPOS 1 2 0");
    SEND ("NAF LBSPOS 1 2 0");

    SEND("NAF LBSSTOP");
    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}



/*-------------------------------------------------------------------------------------------
* Test Description:Register for a periodic fix
*with Output Type = 0,Fix Rate  = 1
*and all other parameters set to platform default.
*
*Pass/Fail : Periodic fix must be returned at every 1 second
*--------------------------------------------------------------------------------------------*/

static int LBSNAFINT0014(TSTENG_handle_t handle)
{

    TCBEGIN(handle, __func__) ;
    SEND("NAF LBSSTARTPFA 0 1023 1000 5");
    RECV("NAF LBSSTATUS 0") ;
    SEND ("NAF LBSPOS 1 2 0");
    SEND ("NAF LBSPOS 1 2 0");
    SEND ("NAF LBSPOS 1 2 0");
    SEND ("NAF LBSPOS 1 2 0");
    SEND ("NAF LBSPOS 1 2 0");

    SEND("NAF LBSSTOP");
    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}



/*-------------------------------------------------------------------------------------------
*Test Description:Register for a periodic fix
*with Output Type = 1,Fix Rate  = 5
*and all other parameters set to platform default.
*
*Pass/Fail Criteria:Periodic fix must be returned at every 5 second
*--------------------------------------------------------------------------------------------*/

static int LBSNAFINT0015(TSTENG_handle_t handle)
{

    TCBEGIN(handle, __func__) ;
    SEND("NAF LBSSTARTPFA 1 0 5000 5");
    RECV("NAF LBSSTATUS 0") ;
    SEND ("NAF LBSPOS 1 6 0");
    SEND ("NAF LBSPOS 1 6 0");
    SEND ("NAF LBSPOS 1 6 0");
    SEND ("NAF LBSPOS 1 6 0");
    SEND ("NAF LBSPOS 1 6 0");

    SEND("NAF LBSSTOP");
    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}



/*-------------------------------------------------------------------------------------------
*Test Description: Register for a periodic fix
*with Output Type = 1,Fix Rate  = 255
*and all other parameters set to platform default.
*Pass/Fail Criteria:Periodic fix must be returned at every 255 second
*--------------------------------------------------------------------------------------------*/


static int LBSNAFINT0016(TSTENG_handle_t handle)
{


    TCBEGIN(handle, __func__) ;
    SEND("NAF LBSSTARTPFA 1 0 255000 5");
    RECV("NAF LBSSTATUS 0") ;
    SEND ("NAF LBSPOS 1 256 0");
    SEND ("NAF LBSPOS 1 256 0");
    SEND ("NAF LBSPOS 1 256 0");
    SEND ("NAF LBSPOS 1 256 0");
    SEND ("NAF LBSPOS 1 256 0");

    SEND("NAF LBSSTOP");
    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}



/*-------------------------------------------------------------------------------------------
*Test Description:Register for a periodic fix
*with Output Type = 0, Specific NMEA Maks set and all other parameters set to platform default.
*
*Pass/Fail Criteria:Periodic fix must be returned and fix information should  have only requested NMEA data strings.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFINT0017(TSTENG_handle_t handle)
{

    TCBEGIN(handle, __func__) ;
    SEND("NAF LBSSTARTPFA 0 255 2000 5");
    RECV("NAF LBSSTATUS 0") ;
    SEND ("NAF LBSPOS 1 3 0");
    SEND ("NAF LBSPOS 1 3 0");
    SEND ("NAF LBSPOS 1 3 0");
    SEND ("NAF LBSPOS 1 3 0");
    SEND ("NAF LBSPOS 1 3 0");

    SEND("NAF LBSSTOP");
    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}


/*-------------------------------------------------------------------------------------------
*Test Description: Register for a periodic fix
*with Output Type = 1, Specific NMEA Masks set and all other parameters set to platform default.
*
*Pass/Fail Criteria:Periodic fix must be returned and fix information should be in C struct format.
*No effect should be seen because of set NMEA mask.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFINT0018(TSTENG_handle_t handle)
{

    TCBEGIN(handle, __func__) ;
    SEND("NAF LBSSTARTPFA 1 255 2000 5");
    RECV("NAF LBSSTATUS 0") ;
    SEND ("NAF LBSPOS 1 3 0");
    SEND ("NAF LBSPOS 1 3 0");
    SEND ("NAF LBSPOS 1 3 0");
    SEND ("NAF LBSPOS 1 3 0");
    SEND ("NAF LBSPOS 1 3 0");

    SEND("NAF LBSSTOP");
    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;
}


/*-------------------------------------------------------------------------------------------
*Test Description: Register for a Single shot position fix
*and all other parameters set to platform default.
*  All other parameters valid
*
*Pass/Fail Criteria:  Singleshot fix must be returned within platform default timeout.
*--------------------------------------------------------------------------------------------*/

static int LBSNAFINT0019(TSTENG_handle_t handle)
{

    TCBEGIN(handle, __func__) ;
    SEND("NAF LBSSTARTSFA 1 0 50 70 70 5");
    RECV("NAF LBSSTATUS 0") ;
    SEND ("NAF LBSPOS 0 52 4");

    SEND("NAF LBSSTOP");
    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}



/*-------------------------------------------------------------------------------------------
*Test Description: Register for a Single shot position fix
*with Output Type = 0 and all other parameters set to platform default.
*
*Pass/Fail Criteria: Single Shot position fix  must be returned and fix information should be in NMEA format.
*--------------------------------------------------------------------------------------------*/

static int LBSNAFINT0021(TSTENG_handle_t handle)
{


    TCBEGIN(handle, __func__) ;
    SEND("NAF LBSSTARTSFA 0 1023 50 70 70 5");
    RECV("NAF LBSSTATUS 0");
    SEND ("NAF LBSPOS 0 52 4");

    SEND("NAF LBSSTOP");
    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}

/*-------------------------------------------------------------------------------------------
*Test Description:  Register for a Single shot position fix
*with Output Type = 1 and all other parameters set to platform default.
*
*Pass/Fail Criteria: Single Shot position fix  must be returned and fix information should be in C struct form,.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFINT0022(TSTENG_handle_t handle)
{

    TCBEGIN(handle, __func__) ;
    SEND("NAF LBSSTARTSFA 1 0 50 70 70 5");
    RECV("NAF LBSSTATUS 0");
    SEND ("NAF LBSPOS 0 52 4");

    SEND("NAF LBSSTOP");
    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}

/*-------------------------------------------------------------------------------------------
*Test Description:  Register for a Single shot position fix
*with Output Type = 250 and all other parameters set to platform default.
*
*Pass/Fail Criteria:  GPSCLIENT_INVALID_PARAMETER  error must be  returned and
*no position fixshould be obtained,.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFINT0023(TSTENG_handle_t handle)
{


    TCBEGIN(handle, __func__) ;
    SEND("NAF LBSSTARTSFA 250 0 50 70 70 5");
    RECV("NAF LBSSTATUS 1");

    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;
}

/*-------------------------------------------------------------------------------------------
* Test Description:Register for a Single shot position fix
*with Output Type = 0, Specific NMEA Masks set and all other parameters set to platform default.
*
*Pass/Fail Criteria:Single Shot position fix must be returned and fix information should
*have only requested NMEA data strings.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFINT0024(TSTENG_handle_t handle)
{

    TCBEGIN(handle, __func__) ;
    SEND("NAF LBSSTARTSFA 0 255 50 70 70 5");
    RECV("NAF LBSSTATUS 0");
    SEND ("NAF LBSPOS 0 52 4");

    SEND("NAF LBSSTOP");
    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}

/*-------------------------------------------------------------------------------------------
* Test Description:Register for a Single shot position fix
* with Output Type = 1, Specific NMEA Masks set and all other parameters set to platform default.
*
* Pass/Fail Criteria:Single Shot position fix must be returned and fix
*information should be in C struct format. No effect should be seen because of set NMEA mask.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFINT0025(TSTENG_handle_t handle)
{


    TCBEGIN(handle, __func__) ;
    SEND("NAF LBSSTARTSFA 1 255 50 70 70 5");
    RECV("NAF LBSSTATUS 0");
    SEND ("NAF LBSPOS 0 52 4");

    SEND("NAF LBSSTOP");
    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}



/*-------------------------------------------------------------------------------------------
* Test Description:Register for a Single shot position fix
*with Timeout  = 0 and all other parameters set to platform default.
*Do not connect antenna , Clear all previous Assistance data.
*
*Pass/Fail Criteria: Single Shot position fix with valid position or invalid position should be returned within platform default timeout.
*--------------------------------------------------------------------------------------------*/

static int LBSNAFINT0026(TSTENG_handle_t handle)
{


    TCBEGIN(handle, __func__) ;
    SEND ("NAF LBSCFGCOLD 1");

    ALOGD("LBSSTELP : Disconnect the GPS antenna\n");
    sleep(5);
    SEND("NAF LBSSTARTSFA 1 0 0 70 70 5");
    RECV("NAF LBSSTATUS 0");
    SEND ("NAF LBSPOS 0 52 0");

    SEND("NAF LBSSTOP");
    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;


}

/*-------------------------------------------------------------------------------------------
* Test Description:Register for a Single shot position fix
* with Timeout  = 1 and all other parameters set to platform default.
* Do not connect antenna , Clear all previous Assistance data.
*
*Pass/Fail Criteria:  Single Shot position fix with valid position or invalid position should be returned within1 sec.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFINT0027(TSTENG_handle_t handle)
{


    TCBEGIN(handle, __func__) ;
    SEND ("NAF LBSCFGCOLD 1");

    ALOGD("LBSSTELP : Disconnect the GPS antenna\n");
    sleep(5);
    SEND("NAF LBSSTARTSFA 1 0 1 70 70 5");
    RECV("NAF LBSSTATUS 0");
    SEND ("NAF LBSPOS 0 2 0");

    SEND("NAF LBSSTOP");
    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;


}

/*-------------------------------------------------------------------------------------------
*Test Description: Register for a Single shot position fix
* with Timeout  = 255 and all other parameters set to platform default.
* Do not connect antenna , Clear all previous Assistance data.
*
*Pass/Fail Criteria:  Single Shot position fix with valid position or invalid position should be returned within 255 sec.
*--------------------------------------------------------------------------------------------*/

static int LBSNAFINT0028(TSTENG_handle_t handle)
{

   TCBEGIN(handle, __func__) ;
   SEND ("NAF LBSCFGCOLD 1");

   ALOGD("LBSSTELP : Disconnect the GPS antenna\n");
   sleep(5);
   SEND("NAF LBSSTARTSFA 1 0 255 70 70 5");
   RECV("NAF LBSSTATUS 0");
   SEND ("NAF LBSPOS 0 256 0");

   SEND("NAF LBSSTOP");
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;


}


/*-------------------------------------------------------------------------------------------
*Test Description: Register for a Single shot position fix
* with Vertical accuracy = 0 and all other parameters set to platform default.
*
*Pass/Fail Criteria:  Single Shot position fix should be reported either 0 vertical accuracy or
*With default vertical accuracy at Timeout,
*--------------------------------------------------------------------------------------------*/

static int LBSNAFINT0029(TSTENG_handle_t handle)
{

    TCBEGIN(handle, __func__) ;
    SEND("NAF LBSSTARTSFA 1 0 50 70 0 5");
    RECV("NAF LBSSTATUS 0");
    SEND ("NAF LBSPOS 0 52 0");

    SEND("NAF LBSSTOP");
    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}
/*-------------------------------------------------------------------------------------------
*Test Description: Register for a Single shot position fix
* with Vertical accuracy = 1 and all other parameters set to platform default.
*
*Pass/Fail Criteria:  Single Shot position fix should be reported either 1 vertical accuracy or
*With default vertical accuracy at Timeout,
*--------------------------------------------------------------------------------------------*/
static int LBSNAFINT0030(TSTENG_handle_t handle)
{

    TCBEGIN(handle, __func__) ;
    SEND("NAF LBSSTARTSFA 1 0 50 70 1 5");
    RECV("NAF LBSSTATUS 0");
    SEND ("NAF LBSPOS 0 52 0");

    SEND("NAF LBSSTOP");
    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}

/*-------------------------------------------------------------------------------------------
*Test Description:  Register for a Single shot position fix
*with Horizontal Accuracy = 0 and all other parameters set to platform default.
*
*Pass/Fail Criteria: Single Shot position fix should be reported either 0 horizontal  accuracy or
*With default horizontal accuracy at Timeout,
*--------------------------------------------------------------------------------------------*/

static int LBSNAFINT0031(TSTENG_handle_t handle)
{


    TCBEGIN(handle, __func__) ;
    SEND("NAF LBSSTARTSFA 1 0 50 0 70 5");
    RECV("NAF LBSSTATUS 0");
    SEND ("NAF LBSPOS 0 52 0");

    SEND("NAF LBSSTOP");
    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}



/*-------------------------------------------------------------------------------------------
*Test Description: Register for a Single shot position fix
* with Horizontal Accuracy = 1 and all other parameters set to platform default.
*
*Pass/Fail Criteria: Single Shot position fix should be reported either 1 horizontal  accuracy or
*With default horizontal accuracy at Timeout.
*--------------------------------------------------------------------------------------------*/

static int LBSNAFINT0032(TSTENG_handle_t handle)
{


    TCBEGIN(handle, __func__) ;
    SEND("NAF LBSSTARTSFA 1 0 50 1 70 5");
    RECV("NAF LBSSTATUS 0");
    SEND ("NAF LBSPOS 0 52 0");

    SEND("NAF LBSSTOP");
    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}

/*-------------------------------------------------------------------------------------------
*Test Description: Register for a Single shot position fix
*with Age Limit = 0 and all other parameters set to platform default..
*
*Pass/Fail Criteria:  Singleshot fix must be returned within platform default timeout.
*--------------------------------------------------------------------------------------------*/

static int LBSNAFINT0033(TSTENG_handle_t handle)
{


    TCBEGIN(handle, __func__) ;
    SEND("NAF LBSSTARTSFA 1 0 50 70 70 0");
    RECV("NAF LBSSTATUS 0");
    SEND ("NAF LBSPOS 0 52 4");

    SEND("NAF LBSSTOP");
    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}


/*-------------------------------------------------------------------------------------------
*Test Description: Register for a Single shot position fix
* Once a fix has been received , Disconnect GPS signal, wait for 5 seconds.
*Again Register for a Single shot position fix with Age Limit = 20 , and all other parameters set to platform default.

*
*Pass/Fail Criteria:
*1) Singleshot position fix must be returned.
*2) Same postion fix as obtained in first should be reported as previous fix obtained is within age limit of current request.
*--------------------------------------------------------------------------------------------*/

static int LBSNAFINT0034(TSTENG_handle_t handle)
{

    TCBEGIN(handle, __func__) ;

    SEND ("NAF LBSCFGCOLD 1");

    sleep(5);
    ALOGD("COLD START MODE");
    SEND("NAF LBSSTARTSFA 1 0 50 100 100 5");
    RECV("NAF LBSSTATUS 0");

    SEND ("NAF LBSPOS 0 52 4");

    ALOGD("LBSSTELP : Disconnect the GPS antenna\n");
    sleep(10);

    SEND("NAF LBSSTARTSFA 1 0 50 100 100 20");
    RECV("NAF LBSSTATUS 0");
    SEND ("NAF LBSPOS 0 10 4");

    SEND("NAF LBSSTOP");

    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}



/*-------------------------------------------------------------------------------------------
*Test Description: Register for a Single shot position fix
* Once a fix has been received , Disconnect GPS signal, wait for 100 seconds.
*Again Register for a Single shot position fix with Age Limit = 120 , and all other parameters set to platform default.

*
*Pass/Fail Criteria:
*1) Singleshot position fix must be returned.
*2) Same postion fix as obtained in first should be reported as previous fix obtained is within age limit of current request,
*--------------------------------------------------------------------------------------------*/

static int LBSNAFINT0035(TSTENG_handle_t handle)
{

    TCBEGIN(handle, __func__) ;

    SEND ("NAF LBSCFGCOLD 1");\

    sleep(5);
    ALOGD("COLD START MODE");
    SEND("NAF LBSSTARTSFA 1 0 50 100 100 5");
    RECV("NAF LBSSTATUS 0");

    SEND ("NAF LBSPOS 0 52 4");

    ALOGD("LBSSTELP : Disconnect the GPS antenna\n");
    sleep(100);


    SEND("NAF LBSSTARTSFA 1 0 50 100 100 120");
    RECV("NAF LBSSTATUS 0");
    SEND ("NAF LBSPOS 0 10 4");

    SEND("NAF LBSSTOP");

    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;


}



/*-------------------------------------------------------------------------------------------
*Test Description: Register for a Single shot position fix
* Once a fix has been received , Disconnect GPS signal,, wait for 100 seconds.
*Again Register for a Single shot position fix with Age Limit = 60 , and all other parameters set to platform default.

*
*Pass/Fail Criteria:
*1) Singleshot position fix must be returned.
*2) Position fix should not be obtained for second request as previous fix is not in
*    age limit of the current request made and GPS signal is not connected.
*--------------------------------------------------------------------------------------------*/


static int LBSNAFINT0036(TSTENG_handle_t handle)
{

    TCBEGIN(handle, __func__) ;

    SEND ("NAF LBSCFGCOLD 1");

    sleep(5);
    ALOGD("COLD START MODE");
    SEND("NAF LBSSTARTSFA 1 0 50 100 100 5");
    RECV("NAF LBSSTATUS 0");

    SEND ("NAF LBSPOS 0 52 4");

    ALOGD("LBSSTELP : Disconnect the GPS antenna\n");
    sleep(120);

    SEND("NAF LBSSTARTSFA 1 0 50 100 100 60");
    RECV("NAF LBSSTATUS 0");
    SEND ("NAF LBSPOS 0 10 0");// checking for 10 sec that fix should not come.


    SEND("NAF LBSSTOP");

    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}




/*-------------------------------------------------------------------------------------------
*Test Description: Register Periodic fix with Output Type = 0, Fix Rate  = 1s.
*Again register for periodic fix with Fix Rate as  2s , 5s and 10s.
*
*Pass/Fail Criteria:  : Periodic fix must be returned at every 1, 2, 5, 10 seconds and fix information should be in NMEA format.
*--------------------------------------------------------------------------------------------*/

static int LBSNAFINT0038(TSTENG_handle_t handle)
{


    TCBEGIN(handle, __func__) ;
    SEND("NAF LBSSTARTPFA  0 1023 1000 5");
    RECV("NAF LBSSTATUS 0");
    SEND ("NAF LBSPOS 1 2 0");
    SEND ("NAF LBSPOS 1 2 0");
    SEND ("NAF LBSPOS 1 2 0");
    SEND ("NAF LBSPOS 1 2 0");
    SEND ("NAF LBSPOS 1 2 0");

    sleep(2);
    SEND("NAF LBSSTARTPFA  0 1023 2000 5");
    RECV("NAF LBSSTATUS 0");
    SEND ("NAF LBSPOS 1 3 0");
    SEND ("NAF LBSPOS 1 3 0");
    SEND ("NAF LBSPOS 1 3 0");
    SEND ("NAF LBSPOS 1 3 0");
    SEND ("NAF LBSPOS 1 3 0");

    sleep(2);
    SEND("NAF LBSSTARTPFA  0 1023 5000 5");
    RECV("NAF LBSSTATUS 0");
    SEND ("NAF LBSPOS 1 6 0");
    SEND ("NAF LBSPOS 1 6 0");
    SEND ("NAF LBSPOS 1 6 0");
    SEND ("NAF LBSPOS 1 6 0");
    SEND ("NAF LBSPOS 1 6 0");

    sleep(2);
    SEND("NAF LBSSTARTPFA  0 1023 10000 3");
    RECV("NAF LBSSTATUS 0");
    SEND ("NAF LBSPOS 1 11 0");
    SEND ("NAF LBSPOS 1 11 0");
    SEND ("NAF LBSPOS 1 11 0");

    SEND("NAF LBSSTOP");
    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}

/*-------------------------------------------------------------------------------------------
*Test Description: Register for a Periodic fix with Output Type = 1, Fix Rate  = 1s.
*Again Register for Periodic fix with Fix Rate as  2s, 5s and 10s.
*
*Pass/Fail Criteria:  : Periodic fix must be returned at every 1, 2, 5, 10 seconds and
*fix information should be in C- struct format.
*--------------------------------------------------------------------------------------------*/



static int LBSNAFINT0039(TSTENG_handle_t handle)
{

    TCBEGIN(handle, __func__) ;
    SEND("NAF LBSSTARTPFA  1 0 1000 5");
    RECV("NAF LBSSTATUS 0");
    SEND ("NAF LBSPOS 1 2 0");
    SEND ("NAF LBSPOS 1 2 0");
    SEND ("NAF LBSPOS 1 2 0");
    SEND ("NAF LBSPOS 1 2 0");
    SEND ("NAF LBSPOS 1 2 0");

    sleep(2);
    SEND("NAF LBSSTARTPFA 1 0 2000 5");
    RECV("NAF LBSSTATUS 0");
    SEND ("NAF LBSPOS 1 3 0");
    SEND ("NAF LBSPOS 1 3 0");
    SEND ("NAF LBSPOS 1 3 0");
    SEND ("NAF LBSPOS 1 3 0");
    SEND ("NAF LBSPOS 1 3 0");

    sleep(2);
    SEND("NAF LBSSTARTPFA  1 0 5000 5");
    RECV("NAF LBSSTATUS 0");
    SEND ("NAF LBSPOS 1 6 0");
    SEND ("NAF LBSPOS 1 6 0");
    SEND ("NAF LBSPOS 1 6 0");
    SEND ("NAF LBSPOS 1 6 0");
    SEND ("NAF LBSPOS 1 6 0");

    sleep(2);
    SEND("NAF LBSSTARTPFA  1 0 10000 3");
    RECV("NAF LBSSTATUS 0");
    SEND ("NAF LBSPOS 1 11 0");
    SEND ("NAF LBSPOS 1 11 0");
    SEND ("NAF LBSPOS 1 11 0");

    SEND("NAF LBSSTOP");
    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}


/*-------------------------------------------------------------------------------------------
*Test Description: Register Periodic fix with Output Type = 0 ,Fix Rate  = 1s.
*Again Register for a Periodic with Fix Rate as  2s , 5s and 10s and with Output Type = 1
*
*Pass/Fail Criteria:  : Periodic fix must be returned at every 1 second with fix information in NMEA format
*and in every 2, 5, 10 seconds with fix information in C- struct format.
*--------------------------------------------------------------------------------------------*/



static int LBSNAFINT0040(TSTENG_handle_t handle)
{


    TCBEGIN(handle, __func__) ;
    SEND("NAF LBSSTARTPFA  0 1023 1000 5");
    RECV("NAF LBSSTATUS 0");
    SEND ("NAF LBSPOS 1 2 0");
    SEND ("NAF LBSPOS 1 2 0");
    SEND ("NAF LBSPOS 1 2 0");
    SEND ("NAF LBSPOS 1 2 0");
    SEND ("NAF LBSPOS 1 2 0");

    sleep(2);
    SEND("NAF LBSSTARTPFA  1 0 2000 5");
    RECV("NAF LBSSTATUS 0");

    SEND ("NAF LBSPOS 1 3 0");
    SEND ("NAF LBSPOS 1 3 0");
    SEND ("NAF LBSPOS 1 3 0");
    SEND ("NAF LBSPOS 1 3 0");
    SEND ("NAF LBSPOS 1 3 0");

    sleep(2);
    SEND("NAF LBSSTARTPFA  1 0 5000 5");
    RECV("NAF LBSSTATUS 0");

    SEND ("NAF LBSPOS 1 6 0");
    SEND ("NAF LBSPOS 1 6 0");
    SEND ("NAF LBSPOS 1 6 0");
    SEND ("NAF LBSPOS 1 6 0");
    SEND ("NAF LBSPOS 1 6 0");

    sleep(2);
    SEND("NAF LBSSTARTPFA  1 0 10000 3");
    RECV("NAF LBSSTATUS 0");

    SEND ("NAF LBSPOS 1 11 0");
    SEND ("NAF LBSPOS 1 11 0");
    SEND ("NAF LBSPOS 1 11 0");

    SEND("NAF LBSSTOP");
    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

    }



/*-------------------------------------------------------------------------------------------
* Test Description: Register Single Shot. Once a fix is obatined Stop GPS
*Disconnect GPS signal
* Wait for 90s and do the following:
*Register for a single shot fix with age limit of 60s
*
*Pass/Fail Criteria:
*1) Singleshot position fix must be returned.
*2) Position fix should not be obtained for second request as previous fix is not in age limit
*    of the current request made and GPS signal is not connected.
*-------------------------------------------------------------------------------------------- */

static int LBSNAFINT0041(TSTENG_handle_t handle)
{

    TCBEGIN(handle, __func__) ;
    SEND("NAF LBSSTARTSFA 1 0 50 70 70 5");
    RECV("NAF LBSSTATUS 0");
    SEND ("NAF LBSPOS 0 52 4");

    SEND("NAF LBSSTOP");

    ALOGD("Dissconnect the simulator\n");
    sleep(90);

    SEND("NAF LBSSTARTSFA 1 0 50 70 70 60");
    RECV("NAF LBSSTATUS 0");
    SEND ("NAF LBSPOS 0 52 0");

    SEND("NAF LBSSTOP");

    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}



/*-------------------------------------------------------------------------------------------
*Test Description: Register for a Single Shot position fix
* Once a fix is obatined Stop GPS
* Disconnect GPS signal
* Wait for 90s and do the following:
*Register for a single shot fix with age limit of 120s
*
*Pass/Fail Criteria:
*1) Singleshot position fix must be returned.
*2) Same postion fix as obtained in first should be reported as previous fix obtained is within age limit of current request.
*-------------------------------------------------------------------------------------------- */


static int LBSNAFINT0042(TSTENG_handle_t handle)
{

   TCBEGIN(handle, __func__) ;
   SEND("NAF LBSSTARTSFA 1 0 50 70 70 5");
   RECV("NAF LBSSTATUS 0");
   SEND ("NAF LBSPOS 0 52 4");

   SEND("NAF LBSSTOP");

   ALOGD("Dissconnect the simulator\n");
   sleep(90);

   SEND("NAF LBSSTARTSFA 1 0 50 70 70 120");
   RECV("NAF LBSSTATUS 0");
   SEND ("NAF LBSPOS 0 20 4");

   SEND("NAF LBSSTOP");

   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;

}
/*-------------------------------------------------------------------------------------------
*Test Description: Register for a Single Shot position fix
* Check for PSTE 1 Message on Session Start
* Check for PSTE 4 Message on getting Fix
* Check for PSTE 2 Message on Session Stop
* Check for PSTE 3 Message on Session Abort
*
*Pass/Fail Criteria:
*1) Test 1 will PASS if Fix is obtained
*2) Test 2 will FAIL as Horizontal and vertical Accuracy are set to 1m
*-------------------------------------------------------------------------------------------- */


static int LBSNAFINT0043(TSTENG_handle_t handle)
{

   TCBEGIN(handle, __func__) ;

   ALOGD("LBSSTELP LBSNAFINT0043 Test1 \n");
   SEND ("NAF LBSCFGCOLD 1");
   ALOGD("COLD START MODE");
   sleep(5);
   SEND("NAF LBSSTARTSFA 0 1023 50 70 70 5");
   RECV("NAF LBSSTATUS 0");
   RECV("NAF LBSNMEA 25 $PSTE 1");
   RECV("NAF LBSNMEA 52 $PSTE 4");
   SEND("NAF LBSSTOP");
   RECV("NAF LBSNMEA 25 $PSTE 2");

   sleep(5);

   ALOGD("LBSSTELP LBSNAFINT0043 Test2 \n");
   SEND ("NAF LBSCFGCOLD 1");
   ALOGD("COLD START MODE");
   sleep(5);
   SEND("NAF LBSSTARTSFA 0 1023 50 1 1 5");
   RECV("NAF LBSSTATUS 0");
   RECV("NAF LBSNMEA 25 $PSTE 1");
   RECV("NAF LBSNMEA 52 $PSTE 4");

   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;

}

/******************************Extended Ephemeris,Xybrid,SNTP,SAGPS TEST CASES*******************************/




#define PGPS_MODE 0
#define PGLONASS_MODE 1
#define PGNSS_MODE 2


/********LBSNAFEE0001***************
a] Test:- PGPS->PGLonass->PGNSS WARM starts for N number of iterations
    PGPS|PGLON|PGNSS| PGPS|PGLON|PGNSS| PGPS|PGLON|PGNSS| PGPS|PGLON|PGNSS| PGPS|PGLON|PGNSS|

    Mode:  WARM

b] Preconditions:- 
    Acquired Almanac
    Atleast one GPS session should have happened prior to this testcase to make it warm start.
    MSL Seed should be downloaded or SIM with data connection enabled must be present to download seed.


c] Details:-
    Warm start sessions with the setup PGPS->PGLonass->PGNSS will go on continously for the N number of iterations.
    Total Iteration number is given as input

d] Usage:-
    lbsstelp NAF LBSNAFEE0001 ##

e] Example:-

i)  lbsstelp NAF LBSNAFEE0001       //Number not provided, by default 300 iterations are considered.100 -PGPS,100-PGLONASS and 100-PGNSS
    
ii) lbsstelp NAF LBSNAFEE0001  600     //Number provided. (N/3) -PGPS, (N/3)-PGLONASS and (N/3)-PGNSS .in example, 200 iterations each.


************************************/

static int LBSNAFEE0001(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    int i;
    int Nextmode = 0;
    int vl_warmCounts = 300; /*Default 300*/

    if(args_found >= 1)
    {
        vl_warmCounts = atoi(cmd_buf + arg_index[0]);
        
    }
    
    ALOGD("LBSSTELP : LBSNAFEE0001 Count = %d ",vl_warmCounts);
    SEND ("NAF LBSCFGCOLD 3");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 0 0");
          
    for(i=0;i<vl_warmCounts;i++)
    {
        ALOGD("Session number %d",i);
        TCBEGIN(handle, __func__);
        switch(Nextmode)
        {
            case PGPS_MODE:
            {
                SEND("NAF LBSCFGSAT 1 0 0");
                SEND("NAF LBSCFGEEWIFI 1 0 0");
                Nextmode = PGLONASS_MODE;
            }
            break;

            
            case PGLONASS_MODE:
            {
                SEND("NAF LBSCFGSAT 0 1 0");
                SEND("NAF LBSCFGEEWIFI 0 1 0");
                Nextmode = PGNSS_MODE;
            }
            break;

            case PGNSS_MODE:
            {
                SEND("NAF LBSCFGSAT 1 1 0");
                SEND("NAF LBSCFGEEWIFI 1 1 0");
                Nextmode = PGPS_MODE;
            }
            break;
                        

        }
        
        rc.IsFixAvailable = FALSE;

        sleep(2);
        SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
        RECV("NAF LBSSTATUS 0");

        SEND ("NAF LBSPOS 0 60 4");
        SEND("NAF LBSSTOP");
        sleep(2);
        
        if(vg_test_result == TSTENG_RESULT_FAIL)
        {
             ALOGD("Session number %d failed, resetting result flag..",i+1);
             vg_test_result = TSTENG_RESULT_OK;
        }

   }
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;


}


/********LBSNAFEE0002***************
a] Test:- PGNSS 24 hours Warm start tests
     Mode:  WARM

b] Preconditions:- 
    Acquired Almanac
    Atleast one GPS session should have happened prior to this testcase to make it warm start.
    MSL Seed should be downloaded or SIM with data connection enabled must be present to download seed.

c] Details:-
    GNSS + PGNSS Warm starts
    24 Hours Test with a session triggered for every 10 minutes

d] Usage:-
    lbsstelp NAF LBSNAFEE0002

e] Example:-
    lbsstelp NAF LBSNAFEE0002

************************************/


static int LBSNAFEE0002(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    int i;
    int vl_timeElapsed = 0; /*24 hours  in seconds ,  24 * 60 * 60  = 86400s*/
    int v1_maxtime = 86400;

    SEND ("NAF LBSCFGCOLD 3");
    SEND("NAF LBSCFGSAT 1 1 0");
    SEND("NAF LBSCFGEEWIFI 1 1 0");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 0 0");
    sleep(5);
    
    while(vl_timeElapsed <= v1_maxtime)
    {
        ALOGD("Time Elapsed number %d",vl_timeElapsed);
        TCBEGIN(handle, __func__);
        rc.IsFixAvailable = FALSE;

        SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
        RECV("NAF LBSSTATUS 0");

        SEND ("NAF LBSPOS 0 60 4");
        SEND("NAF LBSSTOP");
        sleep(600);  //sleep for 10 minutes
        vl_timeElapsed += 600;
        
        if(vg_test_result == TSTENG_RESULT_FAIL)
        {
             ALOGD("Session number %d failed, resetting result flag..",i+1);
             vg_test_result = TSTENG_RESULT_OK;
        }

   }
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;


}


/********LBSNAFEE0003***************
a] Test:- GPS + PGPS  WARM for N number of iterations
    Mode:  WARM

b] Preconditions:- 
    Acquired Almanac
    Atleast one GPS session should have happened prior to this testcase to make it warm start.
    MSL Seed should be downloaded or SIM with data connection enabled must be present to download seed.

c] Details:-
    Warm start sessions with the setup PGPS will go on continously for the N number of iterations.
    Total Iteration number is given as input

d] Usage:-
    lbsstelp NAF LBSNAFEE0003 ##

e] Example:-

i)
    lbsstelp NAF LBSNAFEE0003       //Number not provided, by default 100 iterations are considered.100 -PGPS.
    
ii) lbsstelp NAF LBSNAFEE0003  10     //Number provided. N -PGPS, in example, 10 iterations.


************************************/



static int LBSNAFEE0003(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    int i;
    int vl_warmCounts = 100; /*Default 100*/

    if(args_found >= 1)
    {
        vl_warmCounts = atoi(cmd_buf + arg_index[0]);
    }
    
    ALOGD("LBSSTELP : LBSNAFEE0003 Count = %d ",vl_warmCounts);
    SEND ("NAF LBSCFGCOLD 3");
    SEND("NAF LBSCFGSAT 1 0 0");
    SEND("NAF LBSCFGEEWIFI 1 0 0");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 0 0");
    sleep(5);
          
    for(i=0;i<vl_warmCounts;i++)
    {
        ALOGD("Session number %d",i);
        TCBEGIN(handle, __func__);
        rc.IsFixAvailable = FALSE;

        SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
        RECV("NAF LBSSTATUS 0");

        SEND ("NAF LBSPOS 0 60 4");
        SEND("NAF LBSSTOP");
        sleep(2);
        
        if(vg_test_result == TSTENG_RESULT_FAIL)
        {
             ALOGD("Session number %d failed, resetting result flag..",i+1);
             vg_test_result = TSTENG_RESULT_OK;
        }

   }
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;


}


/********LBSNAFEE0004***************

a] Test:- Glonass + PGlonass  WARM for N number of iterations
    Mode:  WARM

b] Preconditions:- 
    Acquired Almanac
    Atleast one GPS session should have happened prior to this testcase to make it warm start.
    MSL Seed should be downloaded or SIM with data connection enabled must be present to download seed.
                            


c] Details:-
    Warm start sessions with the setup PGlonass will go on continously for the N number of iterations.
    Total Iteration number is given as input

d] Usage:-
    lbsstelp NAF LBSNAFEE0004 ##

e] Example:-

i)
    lbsstelp NAF LBSNAFEE0004       //Number not provided, by default 100 iterations are considered.100 -PGlonass.
    
ii) lbsstelp NAF LBSNAFEE0004  10     //Number provided. N -PGlonass, in example, 10 iterations.


************************************/



static int LBSNAFEE0004(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    int i;
    int vl_warmCounts = 100; /*Default 100*/

    if(args_found >= 1)
    {
        vl_warmCounts = atoi(cmd_buf + arg_index[0]);
    }
    
    ALOGD("LBSSTELP : LBSNAFEE0004 Count = %d ",vl_warmCounts);
    SEND ("NAF LBSCFGCOLD 3");
    SEND("NAF LBSCFGSAT 0 1 0");
    SEND("NAF LBSCFGEEWIFI 0 1 0");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 0 0");
    sleep(5);
          
    for(i=0;i<vl_warmCounts;i++)
    {
        
        ALOGD("Session number %d",i);
        TCBEGIN(handle, __func__);
        rc.IsFixAvailable = FALSE;
        
        SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
        RECV("NAF LBSSTATUS 0");

        SEND ("NAF LBSPOS 0 60 4");
        SEND("NAF LBSSTOP");
        sleep(2);
        
        if(vg_test_result == TSTENG_RESULT_FAIL)
        {
             ALOGD("Session number %d failed, resetting result flag..",i+1);
             vg_test_result = TSTENG_RESULT_OK;
        }

   }
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;


}


/********LBSNAFEE0005***************

a] Test:- GNSS + PGNSS  WARM for N number of iterations
    Mode:  WARM

b] Preconditions:- 
    Acquired Almanac
    Atleast one GPS session should have happened prior to this testcase to make it warm start.
    MSL Seed should be downloaded or SIM with data connection enabled must be present to download seed.
                            

c] Details:-
    Warm start sessions with the setup PGNSS will go on continously for the N number of iterations.
    Total Iteration number is given as input

d] Usage:-
    lbsstelp NAF LBSNAFEE0005 ##

e] Example:-

i)
    lbsstelp NAF LBSNAFEE0005       //Number not provided, by default 100 iterations are considered.100 -PGNSS.
    
ii) lbsstelp NAF LBSNAFEE0005  10     //Number provided. N -PGNSS, in example, 10 iterations.


************************************/



static int LBSNAFEE0005(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    int i;
    int vl_warmCounts = 100; /*Default 100*/

    if(args_found >= 1)
    {
        vl_warmCounts = atoi(cmd_buf + arg_index[0]);
    }
    

    ALOGD("LBSSTELP : LBSNAFEE0005 Count = %d ",vl_warmCounts);

    SEND ("NAF LBSCFGCOLD 3");
    SEND("NAF LBSCFGSAT 1 1 0");
    SEND("NAF LBSCFGEEWIFI 1 1 0");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 0 0");
    sleep(5);
    
    for(i=0;i<vl_warmCounts;i++)
    {
        
        ALOGD("Session number %d",i);
        TCBEGIN(handle, __func__);
        rc.IsFixAvailable = FALSE;
        
        SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
        RECV("NAF LBSSTATUS 0");

        SEND ("NAF LBSPOS 0 60 4");
        SEND("NAF LBSSTOP");
        sleep(2);
        
        if(vg_test_result == TSTENG_RESULT_FAIL)
        {
             ALOGD("Session number %d failed, resetting result flag..",i+1);
             vg_test_result = TSTENG_RESULT_OK;
        }

   }
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;


}


/********LBSNAFEE0006***************

a] Test:- GPS Only + Disable Prediction, WARM for N number of iterations
    Mode:  WARM

b] Preconditions:- 
    Acquired Almanac
    Atleast one GPS session should have happened prior to this testcase to make it warm start.
                            

c] Details:-
    Warm start sessions with the setup GPS Only + Disable Prediction, will go on continously for the N number of iterations.
    Total Iteration number is given as input

d] Usage:-
    lbsstelp NAF LBSNAFEE0006 ##

e] Example:-

i)
    lbsstelp NAF LBSNAFEE0006       //Number not provided, by default 100 iterations are considered.100 -GPS.
    
ii) lbsstelp NAF LBSNAFEE0006  10     //Number provided. N -GPS, in example, 10 iterations.


************************************/



static int LBSNAFEE0006(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    int i;
    int vl_warmCounts = 100; /*Default 100*/

    if(args_found >= 1)
    {
        vl_warmCounts = atoi(cmd_buf + arg_index[0]);
    }
    

    ALOGD("LBSSTELP : LBSNAFEE0006 Count = %d ",vl_warmCounts);

    SEND ("NAF LBSCFGCOLD 3");
    SEND("NAF LBSCFGSAT 1 0 0");
    SEND("NAF LBSCFGEEWIFI 0 0 0");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 0 0");
    sleep(5);
    
    for(i=0;i<vl_warmCounts;i++)
    {
        
        ALOGD("Session number %d",i);
        TCBEGIN(handle, __func__);
        rc.IsFixAvailable = FALSE;
        
        SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
        RECV("NAF LBSSTATUS 0");

        SEND ("NAF LBSPOS 0 60 4");
        SEND("NAF LBSSTOP");
        sleep(2);
        
        if(vg_test_result == TSTENG_RESULT_FAIL)
        {
             ALOGD("Session number %d failed, resetting result flag..",i+1);
             vg_test_result = TSTENG_RESULT_OK;
        }

   }
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;


}


/********LBSNAFEE0007***************

a] Test:- Glonass Only + Disable Prediction, WARM for N number of iterations
    Mode:  WARM

b] Preconditions:- 
    Acquired Almanac 
    Atleast one GPS session should have happened prior to this testcase to make it warm start.
                            

c] Details:-
    Warm start sessions with the setup Glonass Only + Disable Prediction, will go on continously for the N number of iterations.
    Total Iteration number is given as input

d] Usage:-
    lbsstelp NAF LBSNAFEE0007 ##

e] Example:-

i)
    lbsstelp NAF LBSNAFEE0007       //Number not provided, by default 100 iterations are considered.100 -Glonass.
    
ii) lbsstelp NAF LBSNAFEE0007  10     //Number provided. N -Glonass, in example, 10 iterations.


************************************/



static int LBSNAFEE0007(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    int i;
    int vl_warmCounts = 100; /*Default 100*/

    if(args_found >= 1)
    {
        vl_warmCounts = atoi(cmd_buf + arg_index[0]);
    }
    

    ALOGD("LBSSTELP : LBSNAFEE0007 Count = %d ",vl_warmCounts);

    SEND ("NAF LBSCFGCOLD 3");
    SEND("NAF LBSCFGSAT 0 1 0");
    SEND("NAF LBSCFGEEWIFI 0 0 0");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 0 0");
    sleep(5);
    
    for(i=0;i<vl_warmCounts;i++)
    {
        
        ALOGD("Session number %d",i);
        TCBEGIN(handle, __func__);
        rc.IsFixAvailable = FALSE;
        
        SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
        RECV("NAF LBSSTATUS 0");

        SEND ("NAF LBSPOS 0 60 4");
        SEND("NAF LBSSTOP");
        sleep(2);
        
        if(vg_test_result == TSTENG_RESULT_FAIL)
        {
             ALOGD("Session number %d failed, resetting result flag..",i+1);
             vg_test_result = TSTENG_RESULT_OK;
        }

   }
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;


}


/********LBSNAFEE0008**************

a] Test:- GNSS + Disable Prediction, WARM for N number of iterations
    Mode:  WARM

b] Preconditions:-
    Acquired Almanac
    Atleast one GPS session should have happened prior to this testcase to make it warm start.
                            

c] Details:-
    Warm start sessions with the setup GNSS Only + Disable Prediction, will go on continously for the N number of iterations.
    Total Iteration number is given as input

d] Usage:-
    lbsstelp NAF LBSNAFEE0008 ##

e] Example:-

i)
    lbsstelp NAF LBSNAFEE0008       //Number not provided, by default 100 iterations are considered.100 -GNSS.
    
ii) lbsstelp NAF LBSNAFEE0008  10     //Number provided. N -GNSS, in example, 10 iterations.


************************************/



static int LBSNAFEE0008(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    int i;
    int vl_warmCounts = 100; /*Default 100*/

    if(args_found >= 1)
    {
        vl_warmCounts = atoi(cmd_buf + arg_index[0]);
    }
    

    ALOGD("LBSSTELP : LBSNAFEE0008 Count = %d ",vl_warmCounts);

    SEND ("NAF LBSCFGCOLD 3");
    SEND("NAF LBSCFGSAT 1 1 0");
    SEND("NAF LBSCFGEEWIFI 0 0 0");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 0 0");
    sleep(5);
    
    for(i=0;i<vl_warmCounts;i++)
    {
        
        ALOGD("Session number %d",i);
        TCBEGIN(handle, __func__);
        rc.IsFixAvailable = FALSE;
        
        SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
        RECV("NAF LBSSTATUS 0");

        SEND ("NAF LBSPOS 0 60 4");
        SEND("NAF LBSSTOP");
        sleep(2);
        
        if(vg_test_result == TSTENG_RESULT_FAIL)
        {
             ALOGD("Session number %d failed, resetting result flag..",i+1);
             vg_test_result = TSTENG_RESULT_OK;
        }

   }
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;


}




/********LBSNAFEE0009**************

a] Test:- PGNSS,WARM + Tracking for N number of seconds
    Mode:  WARM

b] Preconditions:- 
    Acquired Almanac
    Atleast one GPS session should have happened prior to this testcase to make it warm start.
    MSL Seed should be downloaded or SIM with data connection enabled must be present to download seed.
                            

c] Details:-
    Warm start periodic sessions with the setup PGNSS, will go on continously for the N number of seconds.
    reporting rate set to evry 1 second
    Total periodic Iteration number is given as input.By default its 1200 i.e 20 minutes + time taken for first fix.

d] Usage:-
    lbsstelp NAF LBSNAFEE0009 ##

e] Example:-

i)
    lbsstelp NAF LBSNAFEE0009       //Number not provided, by default 1200 (20 Minutes - 20*60 seconds) iterations are considered.
    
ii) lbsstelp NAF LBSNAFEE0009  1500    //Number provided. N -GNSS, in example, 10 iterations.


************************************/



static int LBSNAFEE0009(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    
    int outputType = 1;
    int vl_warmCounts = 1200; /*Default 1200 -20 minutes + time taken for first fix.*/
    
    if(args_found >= 1)
    {
        vl_warmCounts = atoi(cmd_buf + arg_index[0]);
    }

    ALOGD("LBSSTELP : LBSNAFEE0009 seconds = %d ",vl_warmCounts);
    
    TCBEGIN(handle, __func__);
    SEND("NAF LBSCFGSAT 1 1 0");
    SEND("NAF LBSCFGEEWIFI 1 1 0");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 0 0");

    TEST_NAF_periodic_count = 1;
    rc = TEST_NAF_default_rc;
    

    gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_callbacks );
    rc.aidingDataType = GPSCLIENT_ASSIST_DELETE_EPHEMERIS;
    gpsClient_DeleteAidingData( rc.aidingDataType );
    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Data callbacks successfully registered\n");

    rc.NMEA = 0;
    rc.reporting_rate = 1000;
    rc.no_count_periodic = vl_warmCounts;

    gpsClient_ServiceStart();

    gpsClient_PeriodicFix( outputType, rc.NMEA, rc.reporting_rate );

    (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
    (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);

    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}


/********LBSNAFEE0010**************

a] Test:- PGNSS,WARM + Tracking for N number of seconds
    Mode:  WARM

b] Preconditions:- 
    Acquired Almanac
    Atleast one GPS session should have happened prior to this testcase to make it warm start.
    MSL Seed should be downloaded or SIM with data connection enabled must be present to download seed.
                            

c] Details:-
    Warm start periodic sessions with the setup PGNSS, will go on continously for the N number of seconds.
    reporting rate set to evry 1 second
    Total periodic Iteration number is given as input.By default its 1200 i.e 20 minutes + time taken for first fix.

d] Usage:-
    lbsstelp NAF LBSNAFEE0010 ##

e] Example:-

i)
    lbsstelp NAF LBSNAFEE0010       //Number not provided, by default 1200 (20 Minutes - 20*60 seconds) iterations are considered.
    
ii) lbsstelp NAF LBSNAFEE0010  1500    //Number provided. N -GNSS, in example, 10 iterations.


************************************/



static int LBSNAFEE0010(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    
    int outputType = 1;
    int vl_warmCounts = 1200; /*Default 1200 -20 minutes + time taken for first fix.*/
    
    if(args_found >= 1)
    {
        vl_warmCounts = atoi(cmd_buf + arg_index[0]);
    }

    ALOGD("LBSSTELP : LBSNAFEE0010 seconds = %d ",vl_warmCounts);
    TCBEGIN(handle, __func__);

    SEND("NAF LBSCFGSAT 1 1 0");
    SEND("NAF LBSCFGEEWIFI 1 1 0");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 0 0");

    TEST_NAF_periodic_count = 1;
    rc = TEST_NAF_default_rc;
    

    gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_callbacks );
    rc.aidingDataType = GPSCLIENT_ASSIST_DELETE_EPHEMERIS;
    gpsClient_DeleteAidingData( rc.aidingDataType );
    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Data callbacks successfully registered\n");

    rc.NMEA = 0;
    rc.reporting_rate = 1000;
    rc.no_count_periodic = vl_warmCounts;

    gpsClient_ServiceStart();

    gpsClient_PeriodicFix( outputType, rc.NMEA, rc.reporting_rate );

    (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
    (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);

    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}



/********LBSNAFEE0011**************

a] Test:- PGPS,WARM + Tracking for N number of seconds
    Mode:  WARM

b] Preconditions:-
    Acquired Almanac
    Atleast one GPS session should have happened prior to this testcase to make it warm start.
    MSL Seed should be downloaded or SIM with data connection enabled must be present to download seed.
                            

c] Details:-
    Warm start periodic sessions with the setup PGPS, will go on continously for the N number of seconds.
    reporting rate set to evry 1 second
    Total periodic Iteration number is given as input.By default its 1200 i.e 20 minutes + time taken for first fix.

d] Usage:-
    lbsstelp NAF LBSNAFEE0011 ##

e] Example:-

i)
    lbsstelp NAF LBSNAFEE0011       //Number not provided, by default 1200 (20 Minutes - 20*60 seconds) iterations are considered.
    
ii) lbsstelp NAF LBSNAFEE0011  1500    //Number provided. N -PGPS, in example, 10 iterations.


************************************/



static int LBSNAFEE0011(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    
    int outputType = 1;
    int vl_warmCounts = 1200; /*Default 1200 s -20 minutes + time taken for first fix.*/
    
    if(args_found >= 1)
    {
        vl_warmCounts = atoi(cmd_buf + arg_index[0]);
    }

    
    ALOGD("LBSSTELP : LBSNAFEE0011 seconds = %d ",vl_warmCounts);
    TCBEGIN(handle, __func__);

    SEND("NAF LBSCFGSAT 1 0 0");
    SEND("NAF LBSCFGEEWIFI 1 0 0");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 0 0");

    TEST_NAF_periodic_count = 1;
    rc = TEST_NAF_default_rc;
    

    gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_callbacks );
    rc.aidingDataType = GPSCLIENT_ASSIST_DELETE_EPHEMERIS;
    gpsClient_DeleteAidingData( rc.aidingDataType );
    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Data callbacks successfully registered\n");

    rc.NMEA = 0;
    rc.reporting_rate = 1000;
    rc.no_count_periodic = vl_warmCounts;

    gpsClient_ServiceStart();

    gpsClient_PeriodicFix( outputType, rc.NMEA, rc.reporting_rate );

    (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
    (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);

    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}


/********LBSNAFEE0012*************

a] Test:- PGlonass,WARM + Tracking for N number of seconds
    Mode:  WARM

b] Preconditions:- 
    Acquired Almanac
    Atleast one GPS session should have happened prior to this testcase to make it warm start.
    MSL Seed should be downloaded or SIM with data connection enabled must be present to download seed.
                            

c] Details:-
    Warm start periodic sessions with the setup PGlonass, will go on continously for the N number of seconds.
    reporting rate set to evry 1 second
    Total periodic Iteration number is given as input.By default its 1200 i.e 20 minutes + time taken for first fix.

d] Usage:-
    lbsstelp NAF LBSNAFEE0012 ##

e] Example:-

i)
    lbsstelp NAF LBSNAFEE0012       //Number not provided, by default 1200s (20 Minutes - 20*60 seconds) iterations are considered.
    
ii) lbsstelp NAF LBSNAFEE0012  1500    //Number provided. N -PGlonass, in example, 10 iterations.


************************************/



static int LBSNAFEE0012(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    
    int outputType = 1;
    int vl_warmCounts = 1200; /*Default 1200 -20 minutes + time taken for first fix.*/
    
    if(args_found >= 1)
    {
        vl_warmCounts = atoi(cmd_buf + arg_index[0]);
    }

    
    ALOGD("LBSSTELP : LBSNAFEE0012 seconds = %d ",vl_warmCounts);
    TCBEGIN(handle, __func__);

    SEND("NAF LBSCFGSAT 0 1 0");
    SEND("NAF LBSCFGEEWIFI 0 1 0");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 0 0");

    TEST_NAF_periodic_count = 1;
    rc = TEST_NAF_default_rc;
    

    gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_callbacks );
    rc.aidingDataType = GPSCLIENT_ASSIST_DELETE_EPHEMERIS;
    gpsClient_DeleteAidingData( rc.aidingDataType );
    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Data callbacks successfully registered\n");

    rc.NMEA = 0;
    rc.reporting_rate = 1000;
    rc.no_count_periodic = vl_warmCounts;

    gpsClient_ServiceStart();

    gpsClient_PeriodicFix( outputType, rc.NMEA, rc.reporting_rate );

    (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
    (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);

    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}


/********LBSNAFEE0013*************

a] Test:- GNSS +  Disable Prediction + Tracking for N number of seconds
    Mode:  WARM

b] Preconditions:-
    Acquired Almanac
    Atleast one GPS session should have happened prior to this testcase to make it warm start.
                            

c] Details:-
    Warm start periodic sessions with the setup GNSS +  Disable Prediction , will go on continously for the N number of seconds.
    reporting rate set to evry 1 second
    Total periodic Iteration number is given as input.By default its 1200 i.e 20 minutes + time taken for first fix.

d] Usage:-
    lbsstelp NAF LBSNAFEE0013 ##

e] Example:-

i)
    lbsstelp NAF LBSNAFEE0013       //Number not provided, by default 1200 (20 Minutes - 20*60 seconds) iterations are considered.
    
ii) lbsstelp NAF LBSNAFEE0013  1500    //Number provided. N -GNSS +  Disable Prediction , in example, 10 iterations.


************************************/



static int LBSNAFEE0013(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    
    int outputType = 1;
    int vl_warmCounts = 1200; /*Default 1200 -20 minutes + time taken for first fix.*/
    
    if(args_found >= 1)
    {
        vl_warmCounts = atoi(cmd_buf + arg_index[0]);
    }
    
    ALOGD("LBSSTELP : LBSNAFEE0013 seconds = %d ",vl_warmCounts);
    TCBEGIN(handle, __func__);

    SEND("NAF LBSCFGSAT 1 1 0");
    SEND("NAF LBSCFGEEWIFI 0 0 0");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 0 0");

    TEST_NAF_periodic_count = 1;
    rc = TEST_NAF_default_rc;
    

    gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_callbacks );
    rc.aidingDataType = GPSCLIENT_ASSIST_DELETE_EPHEMERIS;
    gpsClient_DeleteAidingData( rc.aidingDataType );
    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Data callbacks successfully registered\n");

    rc.NMEA = 0;
    rc.reporting_rate = 1000;
    rc.no_count_periodic = vl_warmCounts;

    gpsClient_ServiceStart();

    gpsClient_PeriodicFix( outputType, rc.NMEA, rc.reporting_rate );

    (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
    (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);

    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}


/********LBSNAFEE0014*************

a] Test:- GPS + Disable Prediction + Tracking for N number of seconds
    Mode:  WARM

b] Preconditions:-
    Acquired Almanac
    Atleast one GPS session should have happened prior to this testcase to make it warm start.
                            

c] Details:-
    Warm start periodic sessions with the setup GPS +  Disable Prediction , will go on continously for the N number of seconds.
    reporting rate set to evry 1 second
    Total periodic Iteration number is given as input.By default its 1200 i.e 20 minutes + time taken for first fix.

d] Usage:-
    lbsstelp NAF LBSNAFEE0014 ##

e] Example:-

i)
    lbsstelp NAF LBSNAFEE0014       //Number not provided, by default 1200 (20 Minutes - 20*60 seconds) iterations are considered.
    
ii) lbsstelp NAF LBSNAFEE0014  1500    //Number provided. N -GPS +  Disable Prediction , in example, 10 iterations.


************************************/



static int LBSNAFEE0014(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    
    int outputType = 1;
    int vl_warmCounts = 1200; /*Default 1200 -20 minutes + time taken for first fix.*/
    
    if(args_found >= 1)
    {
        vl_warmCounts = atoi(cmd_buf + arg_index[0]);
    }

    
    ALOGD("LBSSTELP : LBSNAFEE0014 seconds = %d ",vl_warmCounts);
    TCBEGIN(handle, __func__);

    SEND("NAF LBSCFGSAT 1 0 0");
    SEND("NAF LBSCFGEEWIFI 0 0 0");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 0 0");

    TEST_NAF_periodic_count = 1;
    rc = TEST_NAF_default_rc;
    

    gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_callbacks );
    rc.aidingDataType = GPSCLIENT_ASSIST_DELETE_EPHEMERIS;
    gpsClient_DeleteAidingData( rc.aidingDataType );
    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Data callbacks successfully registered\n");

    rc.NMEA = 0;
    rc.reporting_rate = 1000;
    rc.no_count_periodic = vl_warmCounts;

    gpsClient_ServiceStart();

    gpsClient_PeriodicFix( outputType, rc.NMEA, rc.reporting_rate );

    (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
    (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);

    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}


/********LBSNAFEE0015*************

a] Test:- Glonass  + Disable Prediction + Tracking for N number of seconds
    Mode:  WARM

b] Preconditions:-
    Acquired Almanac
    Atleast one GPS session should have happened prior to this testcase to make it warm start.
                            

c] Details:-
    Warm start periodic sessions with the setup Glonass +  Disable Prediction , will go on continously for the N number of seconds.
    reporting rate set to every 1 second
    Total periodic Iteration number is given as input.By default its 1200 i.e 20 minutes + time taken for first fix.

d] Usage:-
    lbsstelp NAF LBSNAFEE0015 ##

e] Example:-

i)
    lbsstelp NAF LBSNAFEE0015       //Number not provided, by default 1200 (20 Minutes - 20*60 seconds) iterations are considered.
    
ii) lbsstelp NAF LBSNAFEE0015  1500    //Number provided. N -Glonass +  Disable Prediction , in example, 10 iterations.


************************************/



static int LBSNAFEE0015(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    
    int outputType = 1;
    int vl_warmCounts = 1200; /*Default 1200 -20 minutes + time taken for first fix.*/
    
    if(args_found >= 1)
    {
        vl_warmCounts = atoi(cmd_buf + arg_index[0]);
    }

    ALOGD("LBSSTELP : LBSNAFEE0015 seconds = %d ",vl_warmCounts);

    TCBEGIN(handle, __func__);

    SEND("NAF LBSCFGSAT 0 1 0");
    SEND("NAF LBSCFGEEWIFI 0 0 0");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 0 0");

    TEST_NAF_periodic_count = 1;
    rc = TEST_NAF_default_rc;
    

    gpsClient_Init( (t_gpsClient_Callbacks *)&TEST_NAF_callbacks );
    rc.aidingDataType = GPSCLIENT_ASSIST_DELETE_EPHEMERIS;
    gpsClient_DeleteAidingData( rc.aidingDataType );
    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK,
                        "LBSSTELP : Data callbacks successfully registered\n");

    rc.NMEA = 0;
    rc.reporting_rate = 1000;
    rc.no_count_periodic = vl_warmCounts;

    gpsClient_ServiceStart();

    gpsClient_PeriodicFix( outputType, rc.NMEA, rc.reporting_rate );

    (void) OSA_InitCountingSemA(NAFSM_TEST_END_CB, 0);
    (void) OSA_WaitForSemSignalled(NAFSM_TEST_END_CB);

    TCEND(handle, __func__) ;
    return TSTENG_RESULT_OK;

}


/********************************Cold Starts**********************/

/********LBSNAFEE0021***************
a] Test:- PGPS->PGLonass->PGNSS cold starts for N number of iterations
    PGPS|PGLON|PGNSS| PGPS|PGLON|PGNSS| PGPS|PGLON|PGNSS| PGPS|PGLON|PGNSS| PGPS|PGLON|PGNSS|

    Mode:  cold

b] Preconditions:- 
     MSL Seed should be downloaded or SIM with data connection enabled must be present to download seed.


c] Details:-
    cold start sessions with the setup PGPS->PGLonass->PGNSS will go on continously for the N number of iterations.
    Total Iteration number is given as input

d] Usage:-
    lbsstelp NAF LBSNAFEE0021 ##

e] Example:-

i)  lbsstelp NAF LBSNAFEE0021       //Number not provided, by default 300 iterations are considered.100 -PGPS,100-PGLONASS and 100-PGNSS
    
ii) lbsstelp NAF LBSNAFEE0021  600     //Number provided. (N/3) -PGPS, (N/3)-PGLONASS and (N/3)-PGNSS .in example, 200 iterations each.


************************************/
 

static int LBSNAFEE0021(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    int i;
    int Nextmode = 0;
    int vl_coldCounts = 300; /*Default 300*/

    if(args_found >= 1)
    {
        vl_coldCounts = atoi(cmd_buf + arg_index[0]);
    }
    

    ALOGD("LBSSTELP : LBSNAFEE0021 Count = %d ",vl_coldCounts);
    SEND ("NAF LBSCFGCOLD 2");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 0 0");
          
    for(i=0;i<vl_coldCounts;i++)
    {
        TCBEGIN(handle, __func__);
        switch(Nextmode)
        {
            case PGPS_MODE:
            {
                SEND("NAF LBSCFGSAT 1 0 0");
                SEND("NAF LBSCFGEEWIFI 1 0 0");
                Nextmode = PGLONASS_MODE;
            }
            break;

            
            case PGLONASS_MODE:
            {
                SEND("NAF LBSCFGSAT 0 1 0");
                SEND("NAF LBSCFGEEWIFI 0 1 0");
                Nextmode = PGNSS_MODE;
            }
            break;

            case PGNSS_MODE:
            {
                SEND("NAF LBSCFGSAT 1 1 0");
                SEND("NAF LBSCFGEEWIFI 1 1 0");
                Nextmode = PGPS_MODE;
            }
            break;
                        

        }
        
        rc.IsFixAvailable = FALSE;

        
        ALOGD("LBSSTELP : cold start mode\n");
        sleep(2);
        SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
        RECV("NAF LBSSTATUS 0");

        SEND ("NAF LBSPOS 0 60 4");
        SEND("NAF LBSSTOP");
        sleep(2);
        
        if(vg_test_result == TSTENG_RESULT_FAIL)
        {
             ALOGD("Session number %d failed, resetting result flag..",i+1);
             vg_test_result = TSTENG_RESULT_OK;
        }

   }
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;


}


/********LBSNAFEE0022***************
a] Test:- PGNSS 24 hours cold start tests
     Mode:  cold

b] Preconditions:- 
     MSL Seed should be downloaded or SIM with data connection enabled must be present to download seed.

c] Details:-
    GNSS + PGNSS cold starts
    24 Hours Test with a session triggered for every 10 minutes

d] Usage:-
    lbsstelp NAF LBSNAFEE0022

e] Example:-
    lbsstelp NAF LBSNAFEE0022

************************************/


static int LBSNAFEE0022(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    int i;
    int vl_timeElapsed = 0; /*24 hours  in seconds ,  24 * 60 * 60  = 86400s*/
    int v1_maxtime = 86400;


    SEND ("NAF LBSCFGCOLD 2");
    SEND("NAF LBSCFGSAT 1 1 0");
    SEND("NAF LBSCFGEEWIFI 1 1 0");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 0 0");
    sleep(5);
    while(vl_timeElapsed <= v1_maxtime)
    {
        
        ALOGD("LBSSTELP : LBSNAFEE0022 vl_timeElapsed = %d ",vl_timeElapsed);
        
        TCBEGIN(handle, __func__);
        rc.IsFixAvailable = FALSE;
        
        
        SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
        RECV("NAF LBSSTATUS 0");

        SEND ("NAF LBSPOS 0 60 4");
        SEND("NAF LBSSTOP");
        sleep(600);  //sleep for 10 minutes
        vl_timeElapsed += 600;
        
        if(vg_test_result == TSTENG_RESULT_FAIL)
        {
             ALOGD("Session number %d failed, resetting result flag..",i+1);
             vg_test_result = TSTENG_RESULT_OK;
        }

   }
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;


}


/********LBSNAFEE0023***************
a] Test:- PGPS  cold for N number of iterations
    Mode:  cold

b] Preconditions:- 
     MSL Seed should be downloaded or SIM with data connection enabled must be present to download seed.
                            


c] Details:-
    cold start sessions with the setup PGPS will go on continously for the N number of iterations.
    Total Iteration number is given as input

d] Usage:-
    lbsstelp NAF LBSNAFEE0023 ##

e] Example:-

i)
    lbsstelp NAF LBSNAFEE0023       //Number not provided, by default 100 iterations are considered.100 -PGPS.
    
ii) lbsstelp NAF LBSNAFEE0023  10     //Number provided. N -PGPS, in example, 10 iterations.


************************************/



static int LBSNAFEE0023(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    int i;
    int vl_coldCounts = 100; /*Default 100*/

    if(args_found >= 1)
    {
        vl_coldCounts = atoi(cmd_buf + arg_index[0]);
    }
    

    ALOGD("LBSSTELP : LBSNAFEE0023 Count = %d ",vl_coldCounts);
    SEND ("NAF LBSCFGCOLD 2");
    SEND("NAF LBSCFGSAT 1 0 0");
    SEND("NAF LBSCFGEEWIFI 1 0 0");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 0 0");
    sleep(5);
    
    for(i=0;i<vl_coldCounts;i++)
    {
        
        ALOGD("Session number %d",i);
        TCBEGIN(handle, __func__);
        rc.IsFixAvailable = FALSE;
        
        SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
        RECV("NAF LBSSTATUS 0");

        SEND ("NAF LBSPOS 0 60 4");
        SEND("NAF LBSSTOP");
        sleep(2);
        
        if(vg_test_result == TSTENG_RESULT_FAIL)
        {
             ALOGD("Session number %d failed, resetting result flag..",i+1);
             vg_test_result = TSTENG_RESULT_OK;
        }

   }
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;


}


/********LBSNAFEE0024***************

a] Test:- PGlonass  cold for N number of iterations
    Mode:  cold

b] Preconditions:- 
     MSL Seed should be downloaded or SIM with data connection enabled must be present to download seed.
                            


c] Details:-
    cold start sessions with the setup PGlonass will go on continously for the N number of iterations.
    Total Iteration number is given as input

d] Usage:-
    lbsstelp NAF LBSNAFEE0024 ##

e] Example:-

i)
    lbsstelp NAF LBSNAFEE0024       //Number not provided, by default 100 iterations are considered.100 -PGlonass.
    
ii) lbsstelp NAF LBSNAFEE0024  10     //Number provided. N -PGlonass, in example, 10 iterations.


************************************/



static int LBSNAFEE0024(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    int i;
    int vl_coldCounts = 100; /*Default 100*/

    if(args_found >= 1)
    {
        vl_coldCounts = atoi(cmd_buf + arg_index[0]);
    }
    

    ALOGD("LBSSTELP : LBSNAFEE0024 Count = %d ",vl_coldCounts);

    SEND ("NAF LBSCFGCOLD 2");
    SEND("NAF LBSCFGSAT 0 1 0");
    SEND("NAF LBSCFGEEWIFI 0 1 0");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 0 0");
    sleep(5);
          
    for(i=0;i<vl_coldCounts;i++)
    {
        ALOGD("Session number %d",i);
        TCBEGIN(handle, __func__);
        rc.IsFixAvailable = FALSE;
        
        SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
        RECV("NAF LBSSTATUS 0");

        SEND ("NAF LBSPOS 0 60 4");
        SEND("NAF LBSSTOP");
        sleep(2);
        
        if(vg_test_result == TSTENG_RESULT_FAIL)
        {
             ALOGD("Session number %d failed, resetting result flag..",i+1);
             vg_test_result = TSTENG_RESULT_OK;
        }

   }
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;


}


/********LBSNAFEE0025***************

a] Test:- PGNSS  cold for N number of iterations
    Mode:  cold

b] Preconditions:- 
     MSL Seed should be downloaded or SIM with data connection enabled must be present to download seed.
                            

c] Details:-
    cold start sessions with the setup PGNSS will go on continously for the N number of iterations.
    Total Iteration number is given as input

d] Usage:-
    lbsstelp NAF LBSNAFEE0025 ##

e] Example:-

i)
    lbsstelp NAF LBSNAFEE0025       //Number not provided, by default 100 iterations are considered.100 -PGNSS.
    
ii) lbsstelp NAF LBSNAFEE0025  10     //Number provided. N -PGNSS, in example, 10 iterations.


************************************/



static int LBSNAFEE0025(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    int i;
    int vl_coldCounts = 100; /*Default 100*/

    if(args_found >= 1)
    {
        vl_coldCounts = atoi(cmd_buf + arg_index[0]);
    }
    

    ALOGD("LBSSTELP : LBSNAFEE0025 Count = %d ",vl_coldCounts);

    SEND ("NAF LBSCFGCOLD 2");
    SEND("NAF LBSCFGSAT 1 1 0");
    SEND("NAF LBSCFGEEWIFI 1 1 0");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 0 0");
    sleep(5);
    
    for(i=0;i<vl_coldCounts;i++)
    {
        ALOGD("Session number %d",i);
        TCBEGIN(handle, __func__);
        rc.IsFixAvailable = FALSE;
        
        SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
        RECV("NAF LBSSTATUS 0");

        SEND ("NAF LBSPOS 0 60 4");
        SEND("NAF LBSSTOP");
        sleep(2);
        
        if(vg_test_result == TSTENG_RESULT_FAIL)
        {
             ALOGD("Session number %d failed, resetting result flag..",i+1);
             vg_test_result = TSTENG_RESULT_OK;
        }

   }
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;


}


/********LBSNAFEE0026***************

a] Test:- GPS Only + Disable Prediction, cold for N number of iterations
    Mode:  cold

b] Preconditions:- 
                    None                            

c] Details:-
    cold start sessions with the setup GPS Only + Disable Prediction, will go on continously for the N number of iterations.
    Total Iteration number is given as input

d] Usage:-
    lbsstelp NAF LBSNAFEE0026 ##

e] Example:-

i)
    lbsstelp NAF LBSNAFEE0026       //Number not provided, by default 100 iterations are considered.100 -GPS.
    
ii) lbsstelp NAF LBSNAFEE0026  10     //Number provided. N -GPS, in example, 10 iterations.


************************************/



static int LBSNAFEE0026(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    int i;
    int vl_coldCounts = 100; /*Default 100*/

    if(args_found >= 1)
    {
        vl_coldCounts = atoi(cmd_buf + arg_index[0]);
    }
    

    ALOGD("LBSSTELP : LBSNAFEE0026 Count = %d ",vl_coldCounts);

    SEND ("NAF LBSCFGCOLD 2");
    SEND("NAF LBSCFGSAT 1 0 0");
    SEND("NAF LBSCFGEEWIFI 0 0 0");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 0 0");
    sleep(5);
          
    for(i=0;i<vl_coldCounts;i++)
    {
        ALOGD("Session number %d",i);
        TCBEGIN(handle, __func__);
        rc.IsFixAvailable = FALSE;
        
        SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
        RECV("NAF LBSSTATUS 0");

        SEND ("NAF LBSPOS 0 60 4");
        SEND("NAF LBSSTOP");
        sleep(2);
        
        if(vg_test_result == TSTENG_RESULT_FAIL)
        {
             ALOGD("Session number %d failed, resetting result flag..",i+1);
             vg_test_result = TSTENG_RESULT_OK;
        }

   }
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;


}


/********LBSNAFEE0027***************

a] Test:- Glonass Only + Disable Prediction, cold for N number of iterations
    Mode:  cold

b] Preconditions:- 
                    None                            

c] Details:-
    cold start sessions with the setup Glonass Only + Disable Prediction, will go on continously for the N number of iterations.
    Total Iteration number is given as input

d] Usage:-
    lbsstelp NAF LBSNAFEE0027 ##

e] Example:-

i)
    lbsstelp NAF LBSNAFEE0027       //Number not provided, by default 100 iterations are considered.100 -Glonass.
    
ii) lbsstelp NAF LBSNAFEE0027  10     //Number provided. N -Glonass, in example, 10 iterations.


************************************/



static int LBSNAFEE0027(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    int i;
    int vl_coldCounts = 100; /*Default 100*/

    if(args_found >= 1)
    {
        vl_coldCounts = atoi(cmd_buf + arg_index[0]);
    }
    

    ALOGD("LBSSTELP : LBSNAFEE0027 Count = %d ",vl_coldCounts);

    SEND ("NAF LBSCFGCOLD 2");
    SEND("NAF LBSCFGSAT 0 1 0");
    SEND("NAF LBSCFGEEWIFI 0 0 0");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 0 0");
    sleep(5);
    
    for(i=0;i<vl_coldCounts;i++)
    {
        ALOGD("Session number %d",i);
        TCBEGIN(handle, __func__);
        rc.IsFixAvailable = FALSE;
        
        SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
        RECV("NAF LBSSTATUS 0");

        SEND ("NAF LBSPOS 0 60 4");
        SEND("NAF LBSSTOP");
        sleep(2);
        
        if(vg_test_result == TSTENG_RESULT_FAIL)
        {
             ALOGD("Session number %d failed, resetting result flag..",i+1);
             vg_test_result = TSTENG_RESULT_OK;
        }

   }
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;


}


/********LBSNAFEE0028**************

a] Test:- GNSS + Disable Prediction, cold for N number of iterations
    Mode:  cold

b] Preconditions:- 
     None                            
                            

c] Details:-
    cold start sessions with the setup GNSS Only + Disable Prediction, will go on continously for the N number of iterations.
    Total Iteration number is given as input

d] Usage:-
    lbsstelp NAF LBSNAFEE0028 ##

e] Example:-

i)
    lbsstelp NAF LBSNAFEE0028       //Number not provided, by default 100 iterations are considered.100 -GNSS.
    
ii) lbsstelp NAF LBSNAFEE0028  10     //Number provided. N -GNSS, in example, 10 iterations.


************************************/



static int LBSNAFEE0028(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    int i;
    int vl_coldCounts = 100; /*Default 100*/

    if(args_found >= 1)
    {
        vl_coldCounts = atoi(cmd_buf + arg_index[0]);
    }
    

    ALOGD("LBSSTELP : LBSNAFEE0028 Count = %d ",vl_coldCounts);

    SEND ("NAF LBSCFGCOLD 2");
    SEND("NAF LBSCFGSAT 1 1 0");
    SEND("NAF LBSCFGEEWIFI 0 0 0");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 0 0");
    sleep(5);
    
    for(i=0;i<vl_coldCounts;i++)
    {
        ALOGD("Session number %d",i);
        TCBEGIN(handle, __func__);
        rc.IsFixAvailable = FALSE;
        
        SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
        RECV("NAF LBSSTATUS 0");

        SEND ("NAF LBSPOS 0 60 4");
        SEND("NAF LBSSTOP");
        sleep(2);
        
        if(vg_test_result == TSTENG_RESULT_FAIL)
        {
             ALOGD("Session number %d failed, resetting result flag..",i+1);
             vg_test_result = TSTENG_RESULT_OK;
        }

   }
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;


}


/********************************Xybrid & SNTP***************/
/********LBSNAFEE0031**************

a] Test:- GNSS + Disable Prediction + Xybrid, cold for N number of iterations
    Mode:  cold

b] Preconditions:- 
             Wi-Fi Module should be enabled.
             Handset is in a position to do a data Connection
             There are RXN registered Wi-Fi hot-spots available in the scan list of wi-fi hotspots
             
                            

c] Details:-
    cold start sessions with the setup GNSS Only + Disable Prediction + Xybrid, will go on continously for the N number of iterations.
    Total Iteration number is given as input

d] Usage:-
    lbsstelp NAF LBSNAFEE0031 ##

e] Example:-

i)
    lbsstelp NAF LBSNAFEE0031       //Number not provided, by default 100 iterations are considered.
    
ii) lbsstelp NAF LBSNAFEE0031  10     //Number provided. N  10 iterations.


************************************/

static int LBSNAFEE0031(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    int i;
    int vl_coldCounts = 100; /*Default 100*/

    if(args_found >= 1)
    {
        vl_coldCounts = atoi(cmd_buf + arg_index[0]);
    }
    
    ALOGD("LBSSTELP : LBSNAFEE0031 count =%d",vl_coldCounts);
    SEND("NAF LBSCFGSAT 1 1 0");
    SEND("NAF LBSCFGEEWIFI 0 0 1");
    SEND ("NAF LBSCFGCOLD 2");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 0 0");
    sleep(5);
    
    for(i=0;i<vl_coldCounts;i++)
    {
        
        ALOGD("Session number %d",i);
        TCBEGIN(handle, __func__);
        rc.IsFixAvailable = FALSE;

        SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
        RECV("NAF LBSSTATUS 0");

        SEND ("NAF LBSPOS 0 60 4");
        SEND("NAF LBSSTOP");
        sleep(2);
        
        if(vg_test_result == TSTENG_RESULT_FAIL)
        {
             ALOGD("Session number %d failed, resetting result flag..",i+1);
             vg_test_result = TSTENG_RESULT_OK;
        }

   }
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;


}

/********LBSNAFEE0032**************

a] Test:- GPS + PGPS + Xybrid, cold for N number of iterations
    Mode:  cold

b] Preconditions:- 
             Wi-Fi Module should be enabled.
             Handset is in a position to do a data Connection
             There are RXN registered Wi-Fi hot-spots available in the scan list of wi-fi hotspots
             
                            

c] Details:-
    cold start sessions with the setup GPS Only + PGPS + Xybrid, will go on continously for the N number of iterations.
    Total Iteration number is given as input

d] Usage:-
    lbsstelp NAF LBSNAFEE0032 ##

e] Example:-

i)
    lbsstelp NAF LBSNAFEE0032       //Number not provided, by default 100 iterations are considered.
    
ii) lbsstelp NAF LBSNAFEE0032  10     //Number provided. N  10 iterations.


************************************/

static int LBSNAFEE0032(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    int i;
    int vl_coldCounts = 100; /*Default 100*/

    if(args_found >= 1)
    {
        vl_coldCounts = atoi(cmd_buf + arg_index[0]);
    }
    
    ALOGD("LBSSTELP : LBSNAFEE0032 coun =%d",vl_coldCounts);
    SEND("NAF LBSCFGSAT 1 0 0");
    SEND("NAF LBSCFGEEWIFI 1 0 1");
    SEND ("NAF LBSCFGCOLD 2");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 0 0");
    sleep(5);
    
    for(i=0;i<vl_coldCounts;i++)
    {
        
        ALOGD("Session number %d",i);
        TCBEGIN(handle, __func__);
        rc.IsFixAvailable = FALSE;

        SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
        RECV("NAF LBSSTATUS 0");

        SEND ("NAF LBSPOS 0 60 4");
        SEND("NAF LBSSTOP");
        sleep(2);
        
        if(vg_test_result == TSTENG_RESULT_FAIL)
        {
             ALOGD("Session number %d failed, resetting result flag..",i+1);
             vg_test_result = TSTENG_RESULT_OK;
        }

   }
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;


}

/********LBSNAFEE0033**************

a] Test:- Glonass + PGlonass + Xybrid, cold for N number of iterations
    Mode:  cold

b] Preconditions:- 
             Wi-Fi Module should be enabled.
             Handset is in a position to do a data Connection
             There are RXN registered Wi-Fi hot-spots available in the scan list of wi-fi hotspots
             
                            

c] Details:-
    cold start sessions with the setup Glonass + PGlonass + Xybrid, will go on continously for the N number of iterations.
    Total Iteration number is given as input

d] Usage:-
    lbsstelp NAF LBSNAFEE0033 ##

e] Example:-

i)
    lbsstelp NAF LBSNAFEE0033       //Number not provided, by default 100 iterations are considered.
    
ii) lbsstelp NAF LBSNAFEE0033  10     //Number provided. N - iterations.


************************************/

static int LBSNAFEE0033(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    int i;
    int vl_coldCounts = 100; /*Default 100*/

    if(args_found >= 1)
    {
        vl_coldCounts = atoi(cmd_buf + arg_index[0]);
    }
    
    ALOGD("LBSSTELP : LBSNAFEE0033 coun =%d",vl_coldCounts);
    SEND("NAF LBSCFGSAT 0 1 0");
    SEND("NAF LBSCFGEEWIFI 0 1 1");
    SEND ("NAF LBSCFGCOLD 2");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 0 0");
    sleep(5);
    
    for(i=0;i<vl_coldCounts;i++)
    {
        
        ALOGD("Session number %d",i);
        TCBEGIN(handle, __func__);
        rc.IsFixAvailable = FALSE;

        SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
        RECV("NAF LBSSTATUS 0");

        SEND ("NAF LBSPOS 0 60 4");
        SEND("NAF LBSSTOP");
        sleep(2);
        
        if(vg_test_result == TSTENG_RESULT_FAIL)
        {
             ALOGD("Session number %d failed, resetting result flag..",i+1);
             vg_test_result = TSTENG_RESULT_OK;
        }

   }
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;


}


/********LBSNAFEE0034**************

a] Test:- GNSS+ PGNSS + Xybrid, cold for N number of iterations
    Mode:  cold

b] Preconditions:- 
              Wi-Fi Module should be enabled.
             Handset is in a position to do a data Connection
             There are RXN registered Wi-Fi hot-spots available in the scan list of wi-fi hotspots
             
                            

c] Details:-
    cold start sessions with the setup GNSS+ PGNSS + Xybrid, will go on continously for the N number of iterations.
    Total Iteration number is given as input

d] Usage:-
    lbsstelp NAF LBSNAFEE0034 ##

e] Example:-

i)
    lbsstelp NAF LBSNAFEE0034       //Number not provided, by default 100 iterations are considered.
    
ii) lbsstelp NAF LBSNAFEE0034  10     //Number provided. N - iterations.


************************************/

static int LBSNAFEE0034(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    int i;
    int vl_coldCounts = 100; /*Default 100*/

    if(args_found >= 1)
    {
        vl_coldCounts = atoi(cmd_buf + arg_index[0]);
    }
    
    ALOGD("LBSSTELP : LBSNAFEE0034 coun =%d",vl_coldCounts);
    SEND("NAF LBSCFGSAT 1 1 0");
    SEND("NAF LBSCFGEEWIFI 1 1 1");
    SEND ("NAF LBSCFGCOLD 2");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 0 0");
    sleep(5);
    
    for(i=0;i<vl_coldCounts;i++)
    {
        
        ALOGD("Session number %d",i);
        TCBEGIN(handle, __func__);
        rc.IsFixAvailable = FALSE;

        SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
        RECV("NAF LBSSTATUS 0");

        SEND ("NAF LBSPOS 0 60 4");
        SEND("NAF LBSSTOP");
        sleep(2);
        
        if(vg_test_result == TSTENG_RESULT_FAIL)
        {
             ALOGD("Session number %d failed, resetting result flag..",i+1);
             vg_test_result = TSTENG_RESULT_OK;
        }

   }
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;


}

/********LBSNAFEE0035**************

a] Test:- GNSS + Disable Prediction + Xybrid, warm for N number of iterations
    Mode:  warm

b] Preconditions:- 
             Atleast one session should have happened prior to this testcase to make it warm start.
              Wi-Fi Module should be enabled.
             Handset is in a position to do a data Connection
             There are RXN registered Wi-Fi hot-spots available in the scan list of wi-fi hotspots
             
                            

c] Details:-
    warm start sessions with the setup GNSS Only + Disable Prediction + Xybrid, will go on continously for the N number of iterations.
    Total Iteration number is given as input

d] Usage:-
    lbsstelp NAF LBSNAFEE0035 ##

e] Example:-

i)
    lbsstelp NAF LBSNAFEE0035       //Number not provided, by default 100 iterations are considered.
    
ii) lbsstelp NAF LBSNAFEE0035  10     //Number provided. N  10 iterations.


************************************/

static int LBSNAFEE0035(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    int i;
    int vl_warmCounts = 100; /*Default 100*/

    if(args_found >= 1)
    {
        vl_warmCounts = atoi(cmd_buf + arg_index[0]);
    }
    
    ALOGD("LBSSTELP : LBSNAFEE0035 count =%d",vl_warmCounts);
    SEND("NAF LBSCFGSAT 1 1 0");
    SEND("NAF LBSCFGEEWIFI 0 0 1");
    SEND ("NAF LBSCFGCOLD 3");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 0 0");
    sleep(5);
    
    for(i=0;i<vl_warmCounts;i++)
    {
        
        ALOGD("Session number %d",i);
        TCBEGIN(handle, __func__);
        rc.IsFixAvailable = FALSE;

        SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
        RECV("NAF LBSSTATUS 0");

        SEND ("NAF LBSPOS 0 60 4");
        SEND("NAF LBSSTOP");
        sleep(2);
        
        if(vg_test_result == TSTENG_RESULT_FAIL)
        {
             ALOGD("Session number %d failed, resetting result flag..",i+1);
             vg_test_result = TSTENG_RESULT_OK;
        }

   }
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;


}

/********LBSNAFEE0036**************

a] Test:- GPS + PGPS + Xybrid, warm for N number of iterations
    Mode:  warm

b] Preconditions:- 
             Atleast one session should have happened prior to this testcase to make it warm start.
              Wi-Fi Module should be enabled.
             Handset is in a position to do a data Connection
             There are RXN registered Wi-Fi hot-spots available in the scan list of wi-fi hotspots
             
                            

c] Details:-
    warm start sessions with the setup GPS Only + PGPS + Xybrid, will go on continously for the N number of iterations.
    Total Iteration number is given as input

d] Usage:-
    lbsstelp NAF LBSNAFEE0036 ##

e] Example:-

i)
    lbsstelp NAF LBSNAFEE0036       //Number not provided, by default 100 iterations are considered.
    
ii) lbsstelp NAF LBSNAFEE0036  10     //Number provided. N  10 iterations.


************************************/

static int LBSNAFEE0036(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    int i;
    int vl_warmCounts = 100; /*Default 100*/

    if(args_found >= 1)
    {
        vl_warmCounts = atoi(cmd_buf + arg_index[0]);
    }
    
    ALOGD("LBSSTELP : LBSNAFEE0036 coun =%d",vl_warmCounts);
    SEND("NAF LBSCFGSAT 1 0 0");
    SEND("NAF LBSCFGEEWIFI 1 0 1");
    SEND ("NAF LBSCFGCOLD 3");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 0 0");
    sleep(5);
    
    for(i=0;i<vl_warmCounts;i++)
    {
        
        ALOGD("Session number %d",i);
        TCBEGIN(handle, __func__);
        rc.IsFixAvailable = FALSE;

        SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
        RECV("NAF LBSSTATUS 0");

        SEND ("NAF LBSPOS 0 60 4");
        SEND("NAF LBSSTOP");
        sleep(2);
        
        if(vg_test_result == TSTENG_RESULT_FAIL)
        {
             ALOGD("Session number %d failed, resetting result flag..",i+1);
             vg_test_result = TSTENG_RESULT_OK;
        }

   }
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;


}

/********LBSNAFEE0037**************

a] Test:- Glonass + PGlonass + Xybrid, warm for N number of iterations
    Mode:  warm

b] Preconditions:- 
             Atleast one session should have happened prior to this testcase to make it warm start.
              Wi-Fi Module should be enabled.
             Handset is in a position to do a data Connection
             There are RXN registered Wi-Fi hot-spots available in the scan list of wi-fi hotspots
             
                            

c] Details:-
    warm start sessions with the setup Glonass + PGlonass + Xybrid, will go on continously for the N number of iterations.
    Total Iteration number is given as input

d] Usage:-
    lbsstelp NAF LBSNAFEE0037 ##

e] Example:-

i)
    lbsstelp NAF LBSNAFEE0037       //Number not provided, by default 100 iterations are considered.
    
ii) lbsstelp NAF LBSNAFEE0037  10     //Number provided. N - iterations.


************************************/

static int LBSNAFEE0037(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    int i;
    int vl_warmCounts = 100; /*Default 100*/

    if(args_found >= 1)
    {
        vl_warmCounts = atoi(cmd_buf + arg_index[0]);
    }
    
    ALOGD("LBSSTELP : LBSNAFEE0037 coun =%d",vl_warmCounts);
    SEND("NAF LBSCFGSAT 0 1 0");
    SEND("NAF LBSCFGEEWIFI 0 1 1");
    SEND ("NAF LBSCFGCOLD 3");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 0 0");
    sleep(5);
    
    for(i=0;i<vl_warmCounts;i++)
    {
        
        ALOGD("Session number %d",i);
        TCBEGIN(handle, __func__);
        rc.IsFixAvailable = FALSE;

        SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
        RECV("NAF LBSSTATUS 0");

        SEND ("NAF LBSPOS 0 60 4");
        SEND("NAF LBSSTOP");
        sleep(2);
        
        if(vg_test_result == TSTENG_RESULT_FAIL)
        {
             ALOGD("Session number %d failed, resetting result flag..",i+1);
             vg_test_result = TSTENG_RESULT_OK;
        }

   }
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;


}


/********LBSNAFEE0038**************

a] Test:- GNSS+ PGNSS + Xybrid, warm for N number of iterations
    Mode:  warm

b] Preconditions:- 
             Atleast one session should have happened prior to this testcase to make it warm start.
              Wi-Fi Module should be enabled.
             Handset is in a position to do a data Connection
             There are RXN registered Wi-Fi hot-spots available in the scan list of wi-fi hotspots
             
                            

c] Details:-
    warm start sessions with the setup GNSS+ PGNSS + Xybrid, will go on continously for the N number of iterations.
    Total Iteration number is given as input

d] Usage:-
    lbsstelp NAF LBSNAFEE0038 ##

e] Example:-

i)
    lbsstelp NAF LBSNAFEE0038       //Number not provided, by default 100 iterations are considered.
    
ii) lbsstelp NAF LBSNAFEE0038  10     //Number provided. N - iterations.


************************************/

static int LBSNAFEE0038(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    int i;
    int vl_warmCounts = 100; /*Default 100*/

    if(args_found >= 1)
    {
        vl_warmCounts = atoi(cmd_buf + arg_index[0]);
    }
    
    ALOGD("LBSSTELP : LBSNAFEE0038 coun =%d",vl_warmCounts);
    SEND("NAF LBSCFGSAT 1 1 0");
    SEND("NAF LBSCFGEEWIFI 1 1 1");
    SEND ("NAF LBSCFGCOLD 3");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 0 0");
    sleep(5);
    
    for(i=0;i<vl_warmCounts;i++)
    {
        
        ALOGD("Session number %d",i);
        TCBEGIN(handle, __func__);
        rc.IsFixAvailable = FALSE;

        SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
        RECV("NAF LBSSTATUS 0");

        SEND ("NAF LBSPOS 0 60 4");
        SEND("NAF LBSSTOP");
        sleep(2);
        
        if(vg_test_result == TSTENG_RESULT_FAIL)
        {
             ALOGD("Session number %d failed, resetting result flag..",i+1);
             vg_test_result = TSTENG_RESULT_OK;
        }

   }
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;


}


/********LBSNAFEE0039**************

a] Test:- GNSS+ PGNSS + Xybrid + SNTP, cold for N number of iterations
    Mode:  cold

b] Preconditions:- 
              Wi-Fi Module should be enabled.
             Handset is in a position to do a data Connection
             There are RXN registered Wi-Fi hot-spots available in the scan list of wi-fi hotspots
             
                            

c] Details:-
    cold start sessions with the setup GNSS+ PGNSS + Xybrid + SNTP, will go on continously for the N number of iterations.
    Total Iteration number is given as input

d] Usage:-
    lbsstelp NAF LBSNAFEE0039 ##

e] Example:-

i)
    lbsstelp NAF LBSNAFEE0039       //Number not provided, by default 100 iterations are considered.
    
ii) lbsstelp NAF LBSNAFEE0039  10     //Number provided. N - iterations.


************************************/

static int LBSNAFEE0039(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    int i;
    int vl_coldCounts = 100; /*Default 100*/

    if(args_found >= 1)
    {
        vl_coldCounts = atoi(cmd_buf + arg_index[0]);
    }
    
    ALOGD("LBSSTELP : LBSNAFEE0039 coun =%d",vl_coldCounts);
    SEND("NAF LBSCFGSAT 1 1 0");
    SEND("NAF LBSCFGEEWIFI 1 1 1");
    SEND ("NAF LBSCFGCOLD 2");
    
    SEND ("NAF LBSCFGSNTPTIME 1");
    SEND ("NAF LBSCFGSELFASSIST 0 0");

     sleep(5);
    
    for(i=0;i<vl_coldCounts;i++)
    {
        
        ALOGD("Session number %d",i);
        TCBEGIN(handle, __func__);
        rc.IsFixAvailable = FALSE;

        SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
        RECV("NAF LBSSTATUS 0");

        SEND ("NAF LBSPOS 0 60 4");
        SEND("NAF LBSSTOP");
        sleep(2);
        
        if(vg_test_result == TSTENG_RESULT_FAIL)
        {
             ALOGD("Session number %d failed, resetting result flag..",i+1);
             vg_test_result = TSTENG_RESULT_OK;
        }

   }
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;


}
/********************************Xybrid & SNTP***************/


/*******************************SAGPS**********************/
/********LBSNAFEE0041***************
a] Test:- GPS + SAGPS  WARM for N number of iterations
    Mode:  WARM

b] Preconditions:- 
    Acquired Almanac
    Atleast one GPS session should have happened prior to this testcase to make it warm start.
    MSL Seed should be downloaded or SIM with data connection enabled must be present to download seed.

c] Details:-
    Warm start sessions with the setup SAGPS will go on continously for the N number of iterations.
    Total Iteration number is given as input

d] Usage:-
    lbsstelp NAF LBSNAFEE0041 ##

e] Example:-

i)
    lbsstelp NAF LBSNAFEE0041       //Number not provided, by default 100 iterations are considered.
    
ii) lbsstelp NAF LBSNAFEE0041  10     //Number provided. N  iterations.


************************************/



static int LBSNAFEE0041(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    int i;
    int vl_warmCounts = 100; /*Default 100*/

    if(args_found >= 1)
    {
        vl_warmCounts = atoi(cmd_buf + arg_index[0]);
    }
    
    ALOGD("LBSSTELP : LBSNAFEE0041 Count = %d ",vl_warmCounts);
    SEND ("NAF LBSCFGCOLD 3");
    SEND("NAF LBSCFGSAT 1 0 0");
    SEND("NAF LBSCFGEEWIFI 0 0 0");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 1 0");
    sleep(5);
          
    for(i=0;i<vl_warmCounts;i++)
    {
        ALOGD("Session number %d",i);
        TCBEGIN(handle, __func__);
        rc.IsFixAvailable = FALSE;

        SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
        RECV("NAF LBSSTATUS 0");

        SEND ("NAF LBSPOS 0 60 4");
        SEND("NAF LBSSTOP");
        sleep(2);
        
        if(vg_test_result == TSTENG_RESULT_FAIL)
        {
             ALOGD("Session number %d failed, resetting result flag..",i+1);
             vg_test_result = TSTENG_RESULT_OK;
        }

   }
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;


}


/********LBSNAFEE0042***************

a] Test:- Glonass + SAGlonass  WARM for N number of iterations
    Mode:  WARM

b] Preconditions:- 
    Acquired Almanac
    Atleast one GPS session should have happened prior to this testcase to make it warm start.
    MSL Seed should be downloaded or SIM with data connection enabled must be present to download seed.
                            


c] Details:-
    Warm start sessions with the setup SAGlonass will go on continously for the N number of iterations.
    Total Iteration number is given as input

d] Usage:-
    lbsstelp NAF LBSNAFEE0042 ##

e] Example:-

i)
    lbsstelp NAF LBSNAFEE0042       //Number not provided, by default 100 iterations are considered.100 -SAGlonass.
    
ii) lbsstelp NAF LBSNAFEE0042  10     //Number provided. N -SAGlonass, in example, 10 iterations.


************************************/



static int LBSNAFEE0042(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    int i;
    int vl_warmCounts = 100; /*Default 100*/

    if(args_found >= 1)
    {
        vl_warmCounts = atoi(cmd_buf + arg_index[0]);
    }
    
    ALOGD("LBSSTELP : LBSNAFEE0042 Count = %d ",vl_warmCounts);
    SEND ("NAF LBSCFGCOLD 3");
    SEND("NAF LBSCFGSAT 0 1 0");
    SEND("NAF LBSCFGEEWIFI 0 0 0");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 0 1");
    sleep(5);
          
    for(i=0;i<vl_warmCounts;i++)
    {
        
        ALOGD("Session number %d",i);
        TCBEGIN(handle, __func__);
        rc.IsFixAvailable = FALSE;
        
        SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
        RECV("NAF LBSSTATUS 0");

        SEND ("NAF LBSPOS 0 60 4");
        SEND("NAF LBSSTOP");
        sleep(2);
        
        if(vg_test_result == TSTENG_RESULT_FAIL)
        {
             ALOGD("Session number %d failed, resetting result flag..",i+1);
             vg_test_result = TSTENG_RESULT_OK;
        }

   }
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;


}


/********LBSNAFEE0043***************

a] Test:- GNSS + SAGNSS  WARM for N number of iterations
    Mode:  WARM

b] Preconditions:- 
    Acquired Almanac
    Atleast one GPS session should have happened prior to this testcase to make it warm start.
    MSL Seed should be downloaded or SIM with data connection enabled must be present to download seed.
                            

c] Details:-
    Warm start sessions with the setup SAGNSS will go on continously for the N number of iterations.
    Total Iteration number is given as input

d] Usage:-
    lbsstelp NAF LBSNAFEE0043 ##

e] Example:-

i)
    lbsstelp NAF LBSNAFEE0043       //Number not provided, by default 100 iterations are considered.100 -SAGNSS.
    
ii) lbsstelp NAF LBSNAFEE0043  10     //Number provided. N -SAGNSS, in example, 10 iterations.


************************************/



static int LBSNAFEE0043(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    int i;
    int vl_warmCounts = 100; /*Default 100*/

    if(args_found >= 1)
    {
        vl_warmCounts = atoi(cmd_buf + arg_index[0]);
    }
    

    ALOGD("LBSSTELP : LBSNAFEE0043 Count = %d ",vl_warmCounts);

    SEND ("NAF LBSCFGCOLD 3");
    SEND("NAF LBSCFGSAT 1 1 0");
    SEND("NAF LBSCFGEEWIFI 0 0 0");
    
    SEND ("NAF LBSCFGSNTPTIME 0");
    SEND ("NAF LBSCFGSELFASSIST 1 1");
    sleep(5);
    
    for(i=0;i<vl_warmCounts;i++)
    {
        
        ALOGD("Session number %d",i);
        TCBEGIN(handle, __func__);
        rc.IsFixAvailable = FALSE;
        
        SEND("NAF LBSSTARTSFA 1 0 50 30 70 2");
        RECV("NAF LBSSTATUS 0");

        SEND ("NAF LBSPOS 0 60 4");
        SEND("NAF LBSSTOP");
        sleep(2);
        
        if(vg_test_result == TSTENG_RESULT_FAIL)
        {
             ALOGD("Session number %d failed, resetting result flag..",i+1);
             vg_test_result = TSTENG_RESULT_OK;
        }

   }
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;


}
/*******************************SAGPS*********************/
/******************************Extended Ephemeris,Xybrid,SNTP,SAGPS TEST CASES*******************************/
void TEST_EEClient_getEphemeris_callback(t_EEClient_EphemerisData *GetEphemeris)
{
    ALOGD("\n TEST_EEClient_getEphemeris_callback Entry");
    ALOGD("\n TEST_EEClient_getEphemeris_callback v_PrnBitMask=%u",GetEphemeris->v_PrnBitMask);
    ALOGD("\n TEST_EEClient_getEphemeris_callback v_ConstelType=%d",GetEphemeris->v_ConstelType);
    ALOGD("\n TEST_EEClient_getEphemeris_callback v_EERefGPSTime=%d",GetEphemeris->v_EERefGPSTime);
}


/**
*  @brief Callback with Get Reference Location Indication. This shall be sent as an Indication for Reference Location Data from the MSL.
*/
void TEST_EEClient_getRefLocation_callback(t_EEClient_RefLocationData *GetRefLocationData)
{
    GetRefLocationData=GetRefLocationData;/*Parameter not used.This is to avoid warning for th known fact*/

}


/**
*  @brief Callback with Get Broadcast Ephemeris. This shall be sent as a confirmation for Broadcast Ephemeris Request from the MSL.
*/
void TEST_EEClient_getBroadcastEphemeris_callback(e_EEClient_Status Status)
{
    
    Status=Status;/*Parameter not used.This is to avoid warning for th known fact*/
}


/**
*  @brief Callback with Set Broadcast Ephemeris Indication. This shall be sent as a Indication for Broadcast Ephemeris from the MSL.
*/
void TEST_EEClient_setBroadcastEphemeris_callback(t_EEClient_NavDataBCE *BCEData)
{
    
    BCEData=BCEData;/*Parameter not used.This is to avoid warning for th known fact*/
}



/**
*  @brief Callback with Get GPS Time. This shall be sent as a confirmation for GPS Time request from the MSL .
*/
void TEST_EEClient_getGpsTime_callback(e_EEClient_Status Status)
{
    
    Status=Status;/*Parameter not used.This is to avoid warning for th known fact*/
}


/**
*  @brief Callback with Set GPS Time. This shall be sent as a Indication for GPS Time Request from the MSL .
*/
void TEST_EEClient_setGpsTime_callback(t_EEClient_GpsTime *SetGpsTime)
{
    SetGpsTime=SetGpsTime;/*Parameter not used.This is to avoid warning for th known fact*/

}



/**
*  @brief Callback with GPS Firmware and Hardware version. This shall be sent as a confirmation to a request for the GPS Firmware and Hardware version .
*/
void TEST_EEClient_setChipVer_callback(t_EEClient_ChipVer *SetChipVer)
{

    ALOGD("\n TEST_EEClient_setChipVer_callback Entry");
    ALOGD("\n TEST_EEClient_setChipVer_callback Software Version = %s",SetChipVer->v_Version);
    ALOGD("\n TEST_EEClient_setChipVer_callback Exit");

}


/*************************EEClient Test Cases*************************************************/

static int TEST_NAF_EEClient_Init(TSTENG_handle_t handle)
{

    int result=0;
    handle = handle; /*handle not used.To avoid Warning for known fact*/
    
    ALOGD("TEST_NAF_EEClientInit: Calling  EEClient_Init\n");
    result= EEClient_Init(&TEST_EE_callbacks);
    ALOGD("TEST_NAF_EEClientInit: EEClient_Init returned = %d",result);

    return TSTENG_RESULT_OK;

}

static int TEST_NAF_EEClient_GetChipVer(TSTENG_handle_t handle)
{

    int result=0;
    handle = handle; /*handle not used.To avoid Warning for known fact*/
    
    ALOGD("TEST_NAF_EEClient_GetChipVer: Entry\n");
    result= EEClient_Init(&TEST_EE_callbacks);
    ALOGD("TEST_NAF_EEClient_GetChipVer: EEClient_Init returned = %d",result);
    result=  EEClient_GetChipVer();
    ALOGD("TEST_NAF_EEClient_GetChipVer: EEClient_GetChipVer returned = %d",result);

    return TSTENG_RESULT_OK;

}

static int TEST_NAF_EEClient_SetCapabilities(TSTENG_handle_t handle)
{

    int result=0;
    handle = handle; /*handle not used.To avoid Warning for known fact*/

    ALOGD("TEST_NAF_EEClientInit: Calling  EEClient_Init\n");
    result= EEClient_Init(&TEST_EE_callbacks);
    ALOGD("TEST_NAF_EEClientInit: EEClient_Init returned = %d",result);
    
    ALOGD("TEST_NAF_EEClient_SetCapabilities Entry");
    result=  EEClient_SetCapabilities(EEClient_LBS_BCE);
    ALOGD("TEST_NAF_EEClient_SetCapabilities: EEClient_SetCapabilities returned = %d",result);

    return TSTENG_RESULT_OK;

}

static int TEST_NAF_EEClient_SetEphemerisData(TSTENG_handle_t handle)
{
    int result=0;
    t_EEClient_NavData ephemerisData = {0};;
    
    handle = handle; /*handle not used.To avoid Warning for known fact*/

    ALOGD("TEST_NAF_EEClient_SetEphemerisData: Entry\n");
    result= EEClient_Init(&TEST_EE_callbacks);
    ALOGD("TEST_NAF_EEClient_SetEphemerisData: EEClient_Init returned = %d",result);
    


    /***Fill EphemerisData with Dummy Data****/
    ephemerisData.v_numEntriesGPS =2;
    ephemerisData.v_numEntriesGlonass=2;
    /**GPS1**/
    ephemerisData.v_FullEphGPS[0].prn = 1 ;
    ephemerisData.v_FullEphGPS[0].CAOrPOnL2 = 2;
    ephemerisData.v_FullEphGPS[0].ura = 3;
    ephemerisData.v_FullEphGPS[0].health = 4;
    ephemerisData.v_FullEphGPS[0].L2PData = 5;
    ephemerisData.v_FullEphGPS[0].ure = 6;
    ephemerisData.v_FullEphGPS[0].AODO = 7;
    ephemerisData.v_FullEphGPS[0].TGD = 8;
    ephemerisData.v_FullEphGPS[0].af2 = 9;
    ephemerisData.v_FullEphGPS[0].ephem_fit = 10;
    ephemerisData.v_FullEphGPS[0].gps_week = 11;
    ephemerisData.v_FullEphGPS[0].iodc =12;
    ephemerisData.v_FullEphGPS[0].toc =13;
    ephemerisData.v_FullEphGPS[0].toe =14;
    ephemerisData.v_FullEphGPS[0].iode =15;
    ephemerisData.v_FullEphGPS[0].af1 =16;
    ephemerisData.v_FullEphGPS[0].crs =17;
    ephemerisData.v_FullEphGPS[0].delta_n =18;
    ephemerisData.v_FullEphGPS[0].cuc =19;
    ephemerisData.v_FullEphGPS[0].cus =20;
    ephemerisData.v_FullEphGPS[0].cic =21;
    ephemerisData.v_FullEphGPS[0].cis =22;
    ephemerisData.v_FullEphGPS[0].crc =23;
    ephemerisData.v_FullEphGPS[0].i_dot =24;
    ephemerisData.v_FullEphGPS[0].e =25;
    ephemerisData.v_FullEphGPS[0].sqrt_a =26;
    ephemerisData.v_FullEphGPS[0].af0 =27;
    ephemerisData.v_FullEphGPS[0].m0 =28;
    ephemerisData.v_FullEphGPS[0].omega0 =29;
    ephemerisData.v_FullEphGPS[0].i0 =30;
    ephemerisData.v_FullEphGPS[0].w =31;
    ephemerisData.v_FullEphGPS[0].omega_dot =32;
    
    /**GPS1**/

    
    /**GPS2**/
    ephemerisData.v_FullEphGPS[1].prn = 41 ;
    ephemerisData.v_FullEphGPS[1].CAOrPOnL2 = 42;
    ephemerisData.v_FullEphGPS[1].ura = 43;
    ephemerisData.v_FullEphGPS[1].health = 44;
    ephemerisData.v_FullEphGPS[1].L2PData = 45;
    ephemerisData.v_FullEphGPS[1].ure = 46;
    ephemerisData.v_FullEphGPS[1].AODO = 47;
    ephemerisData.v_FullEphGPS[1].TGD = 48;
    ephemerisData.v_FullEphGPS[1].af2 = 49;
    ephemerisData.v_FullEphGPS[1].ephem_fit = 50;
    ephemerisData.v_FullEphGPS[1].gps_week = 51;
    ephemerisData.v_FullEphGPS[1].iodc =52;
    ephemerisData.v_FullEphGPS[1].toc =53;
    ephemerisData.v_FullEphGPS[1].toe =54;
    ephemerisData.v_FullEphGPS[1].iode =55;
    ephemerisData.v_FullEphGPS[1].af1 =56;
    ephemerisData.v_FullEphGPS[1].crs =57;
    ephemerisData.v_FullEphGPS[1].delta_n =58;
    ephemerisData.v_FullEphGPS[1].cuc =59;
    ephemerisData.v_FullEphGPS[1].cus =60;
    ephemerisData.v_FullEphGPS[1].cic =61;
    ephemerisData.v_FullEphGPS[1].cis =62;
    ephemerisData.v_FullEphGPS[1].crc =63;
    ephemerisData.v_FullEphGPS[1].i_dot =64;
    ephemerisData.v_FullEphGPS[1].e =65;
    ephemerisData.v_FullEphGPS[1].sqrt_a =66;
    ephemerisData.v_FullEphGPS[1].af0 =67;
    ephemerisData.v_FullEphGPS[1].m0 =68;
    ephemerisData.v_FullEphGPS[1].omega0 =69;
    ephemerisData.v_FullEphGPS[1].i0 =70;
    ephemerisData.v_FullEphGPS[1].w =71;
    ephemerisData.v_FullEphGPS[1].omega_dot =72;
    
    /**GPS2**/

    /**GLONASS1***/
    ephemerisData.v_FullEphGlonass[0].slot = 81;
    ephemerisData.v_FullEphGlonass[0].FT = 82;
    ephemerisData.v_FullEphGlonass[0].M = 83;
    ephemerisData.v_FullEphGlonass[0].Bn = 84;
    ephemerisData.v_FullEphGlonass[0].utc_offset = 85;
    ephemerisData.v_FullEphGlonass[0].freqChannel = 86;
    ephemerisData.v_FullEphGlonass[0].gamma = 87;
    ephemerisData.v_FullEphGlonass[0].lsx =88;
    ephemerisData.v_FullEphGlonass[0].lsy =89;
    ephemerisData.v_FullEphGlonass[0].lsz = 90;
    ephemerisData.v_FullEphGlonass[0].gloSec = 91;
    ephemerisData.v_FullEphGlonass[0].tauN = 92;
    ephemerisData.v_FullEphGlonass[0].x = 93;
    ephemerisData.v_FullEphGlonass[0].y = 94;
    ephemerisData.v_FullEphGlonass[0].z =95;
    ephemerisData.v_FullEphGlonass[0].vx =96;
    ephemerisData.v_FullEphGlonass[0].vy =97;
    ephemerisData.v_FullEphGlonass[0].vz =98;
    /**GLONASS1***/

    /**GLONASS2***/
    ephemerisData.v_FullEphGlonass[1].slot = 21;
    ephemerisData.v_FullEphGlonass[1].FT = 22;
    ephemerisData.v_FullEphGlonass[1].M = 23;
    ephemerisData.v_FullEphGlonass[1].Bn = 24;
    ephemerisData.v_FullEphGlonass[1].utc_offset = 25;
    ephemerisData.v_FullEphGlonass[1].freqChannel = 26;
    ephemerisData.v_FullEphGlonass[1].gamma = 27;
    ephemerisData.v_FullEphGlonass[1].lsx =28;
    ephemerisData.v_FullEphGlonass[1].lsy =29;
    ephemerisData.v_FullEphGlonass[1].lsz = 20;
    ephemerisData.v_FullEphGlonass[1].gloSec = 31;
    ephemerisData.v_FullEphGlonass[1].tauN = 32;
    ephemerisData.v_FullEphGlonass[1].x = 33;
    ephemerisData.v_FullEphGlonass[1].y = 34;
    ephemerisData.v_FullEphGlonass[1].z =35;
    ephemerisData.v_FullEphGlonass[1].vx =36;
    ephemerisData.v_FullEphGlonass[1].vy =37;
    ephemerisData.v_FullEphGlonass[1].vz =38;
    /**GLONASS2***/
    
    
    /***Fill EphemerisData with Dummy Data****/

    ALOGD("Calling EEClient_SetEphemerisData\n");
    ALOGD("Address of ephemerisData=%p\n",(&ephemerisData));
    
    result= EEClient_SetEphemerisData((&ephemerisData));
    ALOGD("TEST_NAF_EEClient_SetEphemerisData: EEClient_SetEphemerisData returned = %d",result);
    return TSTENG_RESULT_OK;

}



static int TEST_NAF_EEClient_EEColdStart(TSTENG_handle_t handle)
{

   TCBEGIN(handle, __func__) ;
      SEND ("NAF LBSCFGDEL 255");
      sleep(5);
      SEND ("NAF EECLIENTSETCAPAB");
      sleep(5);
      SEND ("NAF LBSSTARTSF 1 0 200 100 100 1");
      sleep(50);
      SEND("NAF LBSSTOP");
      
   
   TCEND(handle, __func__) ;
   return TSTENG_RESULT_OK;


}



/*End EEClient Development        194997 18 Jan 2012*/
