/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef ITE_NMF_STATISTIC_FUNCTIONS_H_
#define ITE_NMF_STATISTIC_FUNCTIONS_H_



#include "VhcElementDefs.h"
#include "ite_testenv_utils.h"
#include "ite_sia_buffer.h"
#include "grab_types.idt.h"
#include "ite_main.h"
#include "cli.h"

#include "ite_framerate.h"


typedef struct
{
    /// Memory address where the device will dump actual sensor exposure and pipe DG parameters
    t_uint32 *ptru32_SensorParametersTargetAddress;

    /// Used exposure time in microseconds
    t_uint32 u32_ExposureTime_us;

    /// Used analogue gain as multiplier (units: gain x 256, e.g. 1536)
    t_uint32 u32_AnalogGain_x256;

    /// Current Red  channel gain
    t_uint32 u32_RedGain_x1000;

    /// Current Green  channel gain
    t_uint32 u32_GreenGain_x1000;

    /// Current Blue channel gain
    t_uint32 u32_BlueGain_x1000;

    /// count of frame (1-256)
    t_uint32 u32_frame_counter;

    /// frame rate
    t_uint32 u32_frameRate_x100;

    /// flash-lit frame indicator
    /// Value = 1 indicate it is flash lit frame
    /// Value = 0 indicate it is normal frame
    t_uint32 u32_flash_fired;

    t_uint32    u32_NDFilter_Transparency_x100;

    t_uint32    u32_Flag_NDFilter;

    t_uint32    u32_ExposureQuantizationStep_us;

    t_uint32    u32_ActiveData_ReadoutTime_us;

    t_uint32    u32_SensorExposureTimeMin_us;

    t_uint32    u32_SensorExposureTimeMax_us;

    t_uint32    u32_applied_f_number_x_100;

    t_uint32    u32_SensorParametersAnalogGainMin_x256;

    t_uint32    u32_SensorParametersAnalogGainMax_x256;

    t_uint32    u32_SensorParametersAnalogGainStep_x256;
} ts_frameParamStatus, *tps_frameParamStatus;


typedef struct {
    t_uint8  gridWidth;
    t_uint8  gridHeight;
    t_uint32 HSizeFraction;     // value 1 for 1% to 100 for 100%
    t_uint32 VSizeFraction; // value 1 for 1% to 100 for 100%
    t_uint32 HROIFraction;  // value 1 for 1% to 100 for 100%
    t_uint32 VROIFraction;  // value 1 for 1% to 100 for 100%
    t_uint8  RedSatLevel;
    t_uint8  GreenSatLevel;
    t_uint8  BlueSatLevel;
    float    RedManualGain;
    float    GreenManualGain;
    float    BlueManualGain;
    t_uint32 mode;      // 0 for sensormode=COLORBAR
                    // other for senormode NORMAL MODE
}ts_stat_test_usecase, *tps_stat_test_usecase;




void ITE_NMF_GlaceStatsTest(char *);
//void ITE_NMF_GlaceStatsTestStop(char *pipe);
void ITE_NMF_GlaceStatsTestRequestAndWait(void);
void ITE_NMF_DumpGlaceBuffer(void);
void ITE_NMF_DumpFrameParamStatusBuffer(void) ;
void ITE_NMF_HistoStatsTest(char *);
void ITE_NMF_HistoStatsTestStop(char *pipe);
void ITE_NMF_HistoStatsTestRequestAndWait(void);
void ITE_NMF_DumpHistoBuffer(void);

CMD_COMPLETION C_ite_dbg_createstatbuff_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_glacestattest_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_SetGlaceBlockFraction_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_SetHistoBlockFraction_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_SetGlaceGrid_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_SetGlaceSatLevel_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_SetStatWBManualMode_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_GlaceDump_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_glacehelp_cmd(int a_nb_args, char ** ap_args);

CMD_COMPLETION C_ite_dbg_histostattest_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_HistoDump_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_histohelp_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_CheckHisto_cmd(int a_nb_args, char ** ap_args);

CMD_COMPLETION C_ite_dbg_glacehistostattest_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_updategaintest_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_glacehistoexp_cmd(int a_nb_args, char ** ap_args);

CMD_COMPLETION C_ite_dbg_test_glace_and_focus_stats_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_test_glace_and_focus_stats_robustness_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_GlaceTimeCheck_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_Update_Exposure_Time_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_test_SensorParams_cmd(int     a_nb_args, char    **ap_args);
CMD_COMPLETION C_ite_dbg_test_ISP_Params_cmd(int     a_nb_args,char    **ap_args);

void Init_statistic_ITECmdList(void);

unsigned long long  ITE_NMF_WaitStatReady(enum e_ispctlInfo ispctlInfo);
void ITE_NMF_SetGlaceBlockFraction(t_uint8 HSizeFraction,t_uint8 VSizeFraction,t_uint8 HROIFraction,t_uint8 VROIFraction);
void ITE_NMF_SetHistoBlockFraction(t_uint8 HSizeFraction,t_uint8 VSizeFraction,t_uint8 HROIFraction,t_uint8 VROIFraction);
void ITE_NMF_SetStatWBManualMode(float RedManualGain,float GreenManualGain,float BlueManualGain);
void ITE_NMF_SetGlaceGrid(t_uint8 gridWidth,t_uint8 gridHeight);
void ITE_NMF_SetGlaceSatLevel(t_uint8 RedSatLevel,t_uint8 GreenSatLevel,t_uint8 BlueSatLevel);

unsigned long long ITE_NMF_GlaceReqNoWait(void);  // use Glace Control coin
unsigned long long ITE_NMF_HistoReqNoWait(void);  // use Histo Control coin
unsigned long long ITE_NMF_GlaceAndHistoReqNoWait(void);  // use Glace and Histo Control coin
unsigned long long ITE_NMF_GlaceAndHistoExpReqNoWait(void);
void ITE_NMF_ResetISPBufferPointers(void);
void ITE_NMF_DumpHistoBuffer(void);
void ITE_NMF_CheckHistoBuffer(void);

void ITE_NMF_UpdateGain(void);
void ITE_NMF_UpdateAnalogGain(t_uint32 AGain);
unsigned long long  ITE_NMF_UpdateExposureTime(t_uint32 timeus);
void ITE_NMF_UpdateAnalogGainAndExposureTime(t_uint16 AGain, t_uint32 timeus);
void ITE_NMF_test_glacerobustness(void);
int ITE_NMF_test_glacefocusstats(char * fps);
int ITE_NMF_test_glacefocusstats_robustness(char * fps);
void ITE_NMF_UpdateGlaceParam(void);
void ITE_NMF_UpdateHistoParam(void);
void ITE_NMF_FocusStatsPrepare(void);
void ITE_NMF_GlaceTimeCheck(char *ap_pipe);
void ITE_NMF_Print_frameparamstatus_buffer(void);
void ITE_NMF_UpdateExposureTime_cmd(char *exptime);
void ITE_NMF_UpdateDigitalGain (float DGain);
void ITE_NMF_test_SensorParams(char* maxfps);
void ITE_DisplaySensorParams (void);
void ITE_DigitalGainCheck (char* loop_count);
void ITE_NMF_Continous_Glace_Test(char *ap_pipe);
#endif /*ITE_NMF_STATISTIC_FUNCTIONS_H_ */

