/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __ite_main_h
#define __ite_main_h

#ifdef _ITE_MAIN_
#define ITE_MAIN_EXTERN
#else
#define ITE_MAIN_EXTERN extern
#endif

/// <Hem> As discussed with Atul, compile time flag is to be preferred over command based mechanism for
/// enable/disable of glace and bitmap store functionality in BMS nreg case.
#define STORE_GLACE_STATS_AND_BITMAP (0)


#include <inc/type.h>
#include <cm/inc/cm.h>
#include <los/api/los_api.h>
#include <ite_testenv_utils.h>
#include <assert.h>

#include "cli.h"
#include "grab_types.idt.h"
#include "ispctl_types.idt.h"
#include "ite_host2sensor.h"

#ifdef __ARM_SYMBIAN
#define IMAGING_PATH "F:/imaging"
#define FW_LLA_ISP_ROOT_DIR "F:/imaging"
#define IMAGING_FS_PATH  "F:/imaging/ite_nmf/script/setup_sdcard"
#endif //__ARM_SYMBIAN

#ifdef ANDROID
#define IMAGING_SETUP_SCRIPT ". ./system/usr/share/mm-valid/imaging/ite_nmf/script/setup_sdcard"
#endif

#ifndef UNUSED
#define UNUSED(a) ((void) (a))
#endif /* UNUSED */


#   define ITE_NMF_ASSERT(flag)                                                             \
    do                                                                                      \
    {                                                                                       \
        if (!flag)                                                                          \
        {                                                                                   \
            LOS_Log("Assert at %s, Line = %d function = %s", __FILE__, __LINE__, __func__); \
            while (1)                                                                       \
                ;                                                                           \
        }                                                                                   \
    } while (0)

#ifdef __ARM_LINUX
    int DisplayStartScreen(int fbindex);
#endif

#ifdef ANDROID
#ifdef LOS_Log
#undef LOS_Log
#endif
#define LOS_Log(format...)  CLI_disp_msg((char *) format)
#endif

//percentage deviation of max_exposure time applied from theoretical maximum exposure time
#define K_TolerancePercentage (0.5)

//Max exposure time is deviated from applied maximum exposure time by K_Tolerancelines
#define K_Tolerancelines (6)

#define DEFAULT_FPS 10.0

#define  TIMELOG //mle
enum e_sequencemode {
  MODE_UNDEFINED=0x0,
  STILLPREVIEW=0x1,
  STILL=0x2,
  VIDEOPREVIEW=0x3,
  VIDEO=0x4,
  BMS=0x5,
  BML=0x6
  };

enum e_sequencestate {
  STATE_FREE=0x0,
  STATE_PREPARED=0x1,
  STATE_RUNNING=0x2
};

enum e_bmsmode {
  BMSSTILL=0x0,
  BMSVIDEO=0x1
};

enum e_rawBPP {
  RAW8BPP=8,
  RAW12BPP=12,
  RGB30BPP=30
};

enum e_openpipe {
  BAYERSTORE0=0x0,
  BAYERSTORE1=0x1,  // not usable for V1 (not connected)
  BAYERSTORE2=0x2,
  BAYERLOAD1=0x3,
  BAYERLOAD2=0x4,
  RGBLOAD=0x5
};

typedef struct _sia_usecase_ {
   t_uint32         sensor;
   t_uint32         LR_XSize;
   t_uint32     LR_YSize;
   e_resolution     LR_resolution;
   enum e_grabFormat    LR_GrbFormat;
   t_uint32         HR_XSize;
   t_uint32         HR_YSize;
   e_resolution     HR_resolution;
   enum e_grabFormat    HR_GrbFormat;
   t_uint32         BMS_XSize;
   t_uint32         BMS_YSize;
   e_resolution     BMS_resolution;
   enum e_grabFormat    BMS_GrbFormat;
   enum e_bmsmode   BMS_Mode;
   enum e_openpipe  BMS_output;
   enum e_openpipe  BML_input;
   enum e_grabFormat    BML_GrbFormat;
   t_uint32         flag_backup_fw;
   t_uint32         flag_nosensor_testmode;
   t_cm_memory_handle esramGrabCacheMemHandle;
   t_los_memory_handle FwTuning_handler;
   enum e_sequencemode  mode;
   enum e_sequencestate state;
   float        MaxDZ;
   float        CurrentDZ;
   e_resolution     LR_Min_resolution;
   e_resolution     HR_Min_resolution;
   e_resolution     LR_Max_resolution;
   e_resolution     HR_Video_Max_resolution;
   e_resolution     HR_Still_Max_resolution;
   e_sensormode     sensormode;
   t_uint8      HorizPos;
   t_uint32    BMS_woi_X; 
   t_uint32    BMS_woi_Y;   
   t_uint32    framerate_x100;   
   t_uint32    data_format;   
   t_uint32    smiapp_powerUp_sequence;
   t_uint32    stripeInUse;
   t_uint32    stripeIndex;
}ts_sia_usecase, *tps_sia_usecase;

extern ts_sia_usecase usecase;

/* For Non regression test selection */
typedef struct _sia_nonreglist_ {
 t_uint8  nonreg1;
 t_uint8  nonreg2;
 t_uint8  nonreg3;
 t_uint8  nonreg4;
 t_uint8  nonreg5;
}ts_sia_nonreglist, *tps_sia_nonreglist;

typedef struct _sia_nonregselect_ {
 ts_sia_nonreglist  lr;
 ts_sia_nonreglist  hr;
 ts_sia_nonreglist  lrhr;
 ts_sia_nonreglist  bms;
 ts_sia_nonreglist  bml;
 ts_sia_nonreglist  zoom;
 ts_sia_nonreglist  pan_tilt;
}ts_sia_nonregselect, *tps_sia_nonregselect;

#define KlogDirectoryLentgh  1024
ITE_MAIN_EXTERN char g_out_path[KlogDirectoryLentgh];
ITE_MAIN_EXTERN char g_trace_path[KlogDirectoryLentgh];
ITE_MAIN_EXTERN unsigned long long g_time_trace[32];
ITE_MAIN_EXTERN t_uint32 g_test_perf_status;

#ifdef TIMELOG
#define SIA_EVENT_TIME_SIZE 60

typedef struct _sia_eventtime_ {
t_uint8 index;
unsigned long long time[SIA_EVENT_TIME_SIZE];
unsigned long long timestamp[SIA_EVENT_TIME_SIZE];
enum e_ispctlInfo infoid[SIA_EVENT_TIME_SIZE];
}ts_sia_eventtime, *pts_sia_eventtime;

typedef struct _sia_eventtimelog_ {
ts_sia_eventtime grabLRinfo;
ts_sia_eventtime grabLRerror;
ts_sia_eventtime grabLRdebug;
ts_sia_eventtime grabHRinfo;
ts_sia_eventtime grabHRerror;
ts_sia_eventtime grabHRdebug;
ts_sia_eventtime grabBMSinfo;
ts_sia_eventtime grabBMSerror;
ts_sia_eventtime grabBMSdebug;
ts_sia_eventtime grabBMLinfo;
ts_sia_eventtime grabBMLerror;
ts_sia_eventtime grabBMLdebug;
ts_sia_eventtime ispctlinfo;
ts_sia_eventtime ispctlerror;
ts_sia_eventtime isphosttosensacc;
ts_sia_eventtime ispzoominfo;
ts_sia_eventtime ispstatinfo;
ts_sia_eventtime ispframeinfo;
ts_sia_eventtime ispfocusinfo;
ts_sia_eventtime ispnvminfo;
ts_sia_eventtime ispcdccinfo;
ts_sia_eventtime isppowerinfo;
ts_sia_eventtime ispsmsinfo;
ts_sia_eventtime ispsensopmodeinfo;
ts_sia_eventtime ispsenscommitinfo;
ts_sia_eventtime ispispcommitinfo;
ts_sia_eventtime ispfctlrw;
}ts_sia_eventtimelog, *pts_sia_eventtimelog;
#endif

int ite_main(int argc, char **argv);
void init_test_results_nreg (char test_result_foldername[256]);
void UnExpectedSignalHandler(int );
void Init_UnExpectedSignalHandling();
#ifdef __ARM_SYMBIAN

#ifdef __cplusplus
extern "C"
{
#endif
IMPORT_SHARED void ite_sia_init_redirection(void);
IMPORT_SHARED void ite_sia_deinit_redirection(void);
IMPORT_SHARED TInt mapInstallActiveScheduler();
IMPORT_SHARED TInt mapDeleteActiveScheduler();

IMPORT_SHARED t_los_process_id		LOS_ThreadCreate( void (*start_routine) (void *),
											  void* arg,
											  int stacksize,
											  t_los_priority priority,
											  const char* name );
#ifdef __cplusplus
}
#endif

#endif


#endif // __ite_main_h
