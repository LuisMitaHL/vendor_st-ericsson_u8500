/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __ITE_RECORDSYSTEM_TIME_H
#define __ITE_RECORDSYSTEM_TIME_H

/* For recording system time :
  1. EVENT_REFRESH_EVENTCOUNT macro is used before a coin is toggled, We give the name of the PE along with the 
      * maximum threshold time in milliseceonds for the PE to set
  2. EVENT_GET_EVENTCOUNT macro is used as soon as  a coin is toggled (pe is set),We give the name of the PE,
       *Address of the variable which returns the number of events received since refresh along with the maximum 
       *threshold time in milliseceonds for the PE to set 
  3. RECORD_SYSTEM_TIME macro can be used for any event/function/statement.We give the following parameters
     * type -> This informs the macro if the record system time should start or stop so the parameter  passed is "START" or "STOP".
     * usecase ->  This parameter gives the usecase EG."HR_PIPE_START" etc ...
     * comment -> Here we pass the comment that is to be appended in the report.
     * threshold -> This parameter gives the threshold time in milliseceonds for the particular operation
 * PS: All parameters for the macro RECORD_SYSTEM_TIME are in char format EXCEPT threshold which is in unsigned long long format
 */

#include <inc/type.h>
#include <test/api/test.h>

/* Macro to enable/disable performance Testing */
#ifndef __ARM_SYMBIAN
//#define TEST_PERFORMANCE
#endif
//#define DUMP_PERF_DATA_TO_FILE

/* Threshold values in microseconds */
#define MAX_HR_START_TIME       500000000
#define MAX_LR_START_TIME       500000000
#define MAX_BMS_START_TIME      500000000
#define MAX_BML_START_TIME      500000000
#define MAX_ISP_START_STREAM    500000000
#define MAX_SENSOR_START_STREAM 500000000

#define MAX_HR_STOP_TIME        500000000
#define MAX_LR_STOP_TIME        500000000
#define MAX_BMS_STOP_TIME       500000000
#define MAX_BML_STOP_TIME       500000000
#define MAX_ISP_STOP_STREAM     500000000
#define MAX_SENSOR_STOP_STREAM  500000000
#define MAX_ISP_BOOT_COMPLETE   500000000

/* other defines */
#define MAX_CHAR                256
#define MAX_RECORDINGS          32
#define PERFORMANCE_SKIPPED     256


#ifdef __cplusplus
extern "C"
{
#endif

typedef struct _time_keeper_{
   unsigned long long       cur_time;
   char                     tag[MAX_CHAR];
}ts_time_keeper;

#ifdef __cplusplus
}
#endif

#ifdef DUMP_PERF_DATA_TO_FILE
extern t_los_file                  *g_trace_file;
#endif

extern t_uint32                    g_current_record_index;
extern volatile ts_time_keeper     g_time_keeper[MAX_RECORDINGS];
extern t_uint32                    g_test_perf_status;
extern char                        g_record_msg[256];
extern volatile unsigned long long g_time_diff ,g_time ;
extern t_uint32                    g_time_keeper_index  ;

//volatile unsigned long long g_time_isp_start,g_time_isp_stop,g_time_sensor_start,g_time_sensor_stop,g_time_vpip_start,g_time_vpip_stop;

#ifdef DUMP_PERF_DATA_TO_FILE

#define FWRITE(data)           LOS_fwrite(data, sizeof(data), 1, g_trace_file)

#else

#define FWRITE(data)

#endif


#ifdef TEST_PERFORMANCE

#define CALCULATE_TIME_TAKEN(usecase,comment,threshold)        \
                                                               for (g_time_keeper_index=0;g_time_keeper_index<MAX_RECORDINGS;g_time_keeper_index++) \
                                                               {\
                                                                if(0 == strcmp(g_time_keeper[g_time_keeper_index].tag,comment)) \
                                                                {\
                                                                 g_time_diff = g_time - g_time_keeper[g_time_keeper_index].cur_time;\
                                                                 g_time_keeper_index = MAX_RECORDINGS + 1;\
                                                                }\
                                                               }\
                                                               if (g_time_keeper_index == MAX_RECORDINGS) \
                                                               {\
                                                                snprintf(g_record_msg,sizeof(g_record_msg),"Incorrect parameters set in RECORD_SYSTEM_TIME\n"); \
                                                                snprintf(g_record_msg,sizeof(g_record_msg)," \n"); \
                                                               }\
                                                               else \
                                                               {\
                                                                snprintf(g_record_msg,sizeof(g_record_msg),"[ITE_NMF_8500][%s]Recording STOP Time For [%s] (in microseconds) = %llu \n",usecase,comment,g_time); \
                                                                FWRITE(g_record_msg);  if (g_out_path[0] != '\0') { mmte_testComment(g_record_msg);}    \
                                                                snprintf(g_record_msg,sizeof(g_record_msg),"[ITE_NMF_8500]Time Taken For [%s] (in microseconds) = %llu \n",comment,g_time_diff); \
                                                                FWRITE(g_record_msg);  if (g_out_path[0] != '\0') { mmte_testComment(g_record_msg);}   \
                                                                snprintf(g_record_msg,sizeof(g_record_msg),"[ITE_NMF_8500]THRESHOLD Time For [%s] (in microseconds) = %llu \n",comment,(unsigned long long)threshold); \
                                                                FWRITE(g_record_msg);   if (g_out_path[0] != '\0') { mmte_testComment(g_record_msg);}   \
                                                                if (g_time_diff > (unsigned long long)threshold)\
                                                                {  g_test_perf_status = FALSE;\
                                                                   if ( g_out_path[0] != '\0' ) {\
                                                                   snprintf(g_record_msg,sizeof(g_record_msg),"Test Failed in Performance\n");\
                                                                   mmte_testComment(g_record_msg);}\
                                                                } \
                                                                else {g_test_perf_status = TRUE;} \
                                                                } \
                                                                g_time_keeper[g_time_keeper_index].cur_time = 0; \
                                                                memset ( g_time_keeper[g_time_keeper_index].tag, 0, MAX_CHAR*sizeof(char) ); 
                                                               
                                                              


#define RECORD_SYSTEM_TIME(STATE,usecase,comment,threshold)     \
                                                               g_time=LOS_getSystemTime(); \
                                                               if (0 == strcmp(STATE,"START")) { \
                                                               snprintf(g_record_msg ,sizeof(g_record_msg),"[ITE_NMF_8500][%s]Recording START Time For [%s] (in microseconds) = %llu \n",usecase,comment,g_time);\
                                                               FWRITE(g_record_msg); \
                                                               g_time_keeper[g_current_record_index].cur_time = g_time;         \
                                                               strcpy(g_time_keeper[g_current_record_index].tag,comment);       \
                                                               if ( g_out_path[0] != '\0' ) {mmte_testComment(g_record_msg);} \
                                                               g_current_record_index++; \
                                                               if (g_current_record_index == (MAX_RECORDINGS)) {g_current_record_index = 0;} } \
                                                               else if (0 == strcmp(STATE,"STOP")) {\
                                                               CALCULATE_TIME_TAKEN(usecase,comment,threshold);}
                                                               /*do {} while(FALSE)*/

 

#define PERFORMANCE_RESULT_FAILED()                            \
                                                                if(PERFORMANCE_SKIPPED == g_test_perf_status)\
                                                                {LOS_Log("\nTest FAILED Performance SKIPPED\n");\
                                                                mmte_testResult(TEST_FAILED);}\
                                                                else if ((FALSE == g_test_perf_status) && (g_out_path[0] != '\0'))                            \
                                                                {LOS_Log("\nTEST FAILED,Test FAILED in Performance\n");\
                                                                 mmte_testComment("TEST FAILED,Test FAILED in Performance\n");\
                                                                 mmte_testResult(TEST_FAILED);}                 \
                                                                else if ((TRUE == g_test_perf_status)&& (g_out_path[0] != '\0'))\
                                                                {LOS_Log("\nTEST FAILED,Test PASSED in Performance\n");\
                                                                 mmte_testComment("TEST FAILED,Test PASSED in Performance\n");\
                                                                 mmte_testResult(TEST_FAILED);}\
                                                                 else if ((TRUE == g_test_perf_status)&& (g_out_path[0] == '\0'))\
                                                                 {LOS_Log("\nTest FAILED,Performance PASSED\n");}\
                                                                 else\
                                                                 {LOS_Log("\nTEST FAILED,Test FAILED in Performance\n");}
                                                                /*do {} while(FALSE)*/

                                                                
#define PERFORMANCE_RESULT_PASSED()                            \
                                                                if(PERFORMANCE_SKIPPED == g_test_perf_status)\
                                                                {LOS_Log("\nTest PASSED Performance SKIPPED\n");\
                                                                mmte_testResult(TEST_PASSED);}\
                                                                else if ((FALSE == g_test_perf_status) && (g_out_path[0] != '\0'))                            \
                                                                {LOS_Log("\nTEST PASSED,Test FAILED in Performance\n");\
                                                                 mmte_testComment("TEST PASSED,Test FAILED in Performance\n");\
                                                                 mmte_testResult(TEST_PASSED);}                 \
                                                                else if ((TRUE == g_test_perf_status)&& (g_out_path[0] != '\0'))\
                                                                {LOS_Log("\nTEST PASSED,Test PASSED in Performance\n");\
                                                                 mmte_testComment("TEST PASSED,Test PASSED in Performance\n");\
                                                                 mmte_testResult(TEST_PASSED);}\
                                                                else if ((TRUE == g_test_perf_status) && (g_out_path[0] == '\0'))                            \
                                                                 {LOS_Log("\nTest PASSED,Performance PASSED\n");}\
                                                                else\
                                                                 {LOS_Log("\nTEST PASSED,Test FAILED in Performance\n");}
                                                                 /*do {} while(FALSE)*/
                                                                
                                                                 
#define EVENT_REFRESH_EVENTCOUNT(name,threshold)      \
                                            ITE_RefreshEventCount(name##_Byte0); \
                                            RECORD_SYSTEM_TIME("START","EVENTS",#name,threshold)



#define EVENT_GET_EVENTCOUNT(name,threshold)    \
                                               ITE_GetEventCount(name##_Byte0); \
                                            RECORD_SYSTEM_TIME("STOP","EVENTS",#name,threshold); \
                                               LOS_Log("1 %s EVENT(S) RECEIVED.\n",#name)

                                                               

                                                                                                                        


                                            
#else /* TEST_PERFORMANCE */

#define CALCULATE_TIME_TAKEN(time,usecase,comment,threshold)
#define RECORD_SYSTEM_TIME(type,usecase,comment,threshold)
#define EVENT_REFRESH_EVENTCOUNT(name,threshold)     
#define EVENT_GET_EVENTCOUNT(name, number_of_events, threshold)  

#define PERFORMANCE_RESULT_FAILED()                              \
                                                                LOS_Log("TEST FAILED\n");                                  \
                                                                mmte_testResult(TEST_FAILED)

#define PERFORMANCE_RESULT_PASSED()                             \
                                                                LOS_Log("TEST PASSED\n");                                  \
                                                                mmte_testResult(TEST_PASSED)

#endif  /* TEST_PERFORMANCE */

                                                                                                                         



#endif  /*__ITE_RECORDSYSTEM_TIME_H */                                                            
