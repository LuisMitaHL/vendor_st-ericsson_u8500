/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef ITE_NMF_STANDARD_FUNCTIONS_H_
#define ITE_NMF_STANDARD_FUNCTIONS_H_

#include "ite_testenv_utils.h"
#include "ite_sia_buffer.h"
#include "grab_types.idt.h"
#include "ite_main.h"
#include "ite_framerate.h"

typedef enum e_result 
{
  SUCCESS=0,
  FAILURE=1
}RESULT_te;

typedef struct _bmlinfo_ {
   t_uint32   s16_CenterOffsetX;
   t_uint32   s16_CenterOffsetY;
   t_uint32   f_SetFOVX;
   t_uint32   Previous_f_SetFOVX;
   t_uint32   u16_MaximumUsableSensorFOVX;
   t_uint32   u16_MaximumUsableSensorFOVY;
   t_uint32   u16_MaxOPXOutputSize;
   t_uint32   u16_MaxOPYOutputSize;
   t_uint32   u16_VTXAddrMin;
   t_uint32   u16_VTYAddrMin;
   t_uint32   u16_VTXAddrMax;
   t_uint32   u16_VTYAddrMax;
   t_uint32   f_PreScaleFactor;
   t_uint32   u16_VTXAddrStart;
   t_uint32   u16_VTYAddrStart;
   t_uint32   u16_VTXAddrEnd;
   t_uint32   u16_VTYAddrEnd;
   t_uint32   u16_NumberofNonActiveLinesAtTopEdge;
} ts_bmlinfo;



#define IS_TOSH_SENSOR_USED (ite_sensorselect==0)
#define IS_558_SENSOR_USED (ite_sensorselect==1)


void ITE_testFirmwareRevision();
void ITE_InitUseCase(char * ap_test_id, char * ap_grabvpip_options,tps_sia_usecase p_sia_usecase);
void ITE_Start_Env(tps_sia_usecase p_sia_usecase, char * ap_grabvpip_options, int smiapp_power_sequence, int sensor_tunning_before_boot);
void ITE_Stop_Env(void);
void ITE_Clear_Env(char *args);
void ITE_Load_FW(tps_sia_usecase p_sia_usecase);
void ITE_StoreCurrentFOVX(void);
void ITE_ConfigureLRPipe(tps_siapicturebuffer p_Buffer);
void ITE_ConfigureHRPipe(tps_siapicturebuffer p_Buffer);
void ITE_ConfigureBMSPipe(tps_siapicturebuffer p_Buffer, t_uint32 nbFrames);
void ITE_StillPreview_Prepare(tps_sia_usecase p_sia_usecase);
void ITE_StillPreview_Start(t_uint32 nbFrames);
void ITE_StillPreview_Stop(void);
void ITE_StillPreview_Free(void);
void ITE_Still_Prepare(tps_sia_usecase p_sia_usecase, t_uint32 nbFrames);
int  ITE_Still_Start(t_uint32 nbFrames);
void ITE_Still_Stop(void);
void ITE_Still_Free(void);
void ITE_BMS_Prepare(tps_sia_usecase p_sia_usecase, t_uint32 nbFrames);
int  ITE_BMS_Start(t_uint32 nbFrames);
int  ITE_BMS_Start_Extention(t_uint32 nbFrames ,char* wait_param ,char* toggle_param);
void ITE_BMS_Stop(void);
void ITE_BMS_Free(void);
void ITE_BML_Prepare(tps_sia_usecase p_sia_usecase, t_uint32 nbFrames);
void ITE_BML_stripe_Prepare(tps_sia_usecase p_sia_usecase, t_uint32 nbFrames);
int  ITE_BML_Start(t_uint32 nbFrames);
int  ITE_BML_stripe_Start(t_uint32 nbFrames,t_uint32 stripe_count);
void ITE_BML_Stop(void);
void ITE_BMS_Abort(void);
void ITE_BML_Free(void);
void ITE_VideoPreview_Prepare(tps_sia_usecase p_sia_usecase);
void ITE_VideoPreview_Start(t_uint32 nbFrames);
void ITE_VideoPreview_Stop(void);
void ITE_VideoPreview_Free(void);
void ITE_Video_Prepare(tps_sia_usecase p_sia_usecase, t_uint32 nbFrames);
void ITE_Video_Start(t_uint32 nbFrames);
void ITE_Video_Stop(void);
void ITE_Video_Free(void);
void ITE_DisplayBuffer(tps_siapicturebuffer p_Buffer);
void ITE_DisplaySingleBuffer(tps_siapicturebuffer p_Buffer);
void ITE_modecheck(void);
void ITE_InitBmlInfo(void);
void ITE_SetGammaCurve(char *curve);

//void ITE_Shutter(IrisCmd_te action);




void ITE_LRHR_Prepare(tps_sia_usecase p_sia_usecase, t_uint32 nbFrames);
int ITE_LRHR_Start(t_uint32 nbFrames);
void ITE_LRHR_Stop(void);
void ITE_LRHR_Abort(void);
void ITE_LRHR_Free(void);
void ITE_HR_Prepare(tps_sia_usecase p_sia_usecase, t_uint32 nbFrames);
int  ITE_HR_Start(t_uint32 nbFrames);
void ITE_HR_Stop(void);
void ITE_HR_Abort(void);
void ITE_HR_Free(void);
void ITE_LR_Prepare(tps_sia_usecase p_sia_usecase, t_uint32 nbFrames);
int  ITE_LR_Start(t_uint32 nbFrames);
void ITE_LR_Stop(void);
void ITE_LR_Abort(void);
void ITE_LR_Free(void);
void ITE_LRHR_Prepare(tps_sia_usecase p_sia_usecase, t_uint32 nbFrames);
int  ITE_LRHR_Start(t_uint32 nbFrames);
void ITE_LRHR_Stop(void);
void ITE_LRHR_Free(void);
void ITE_stop_sia(void );
void ITE_patchStopStartRxforCSI(void);
void ITE_postBootPE_PEPS(void);
void ITE_postBoot(void);
#if defined(_SVP_)
void ITE_postBootPE_TLM(void);
#endif
/*
void ITE_ConfigureLRPipe(t_uint32 FrameXSize, t_uint32 FrameYSize, t_uint32 frameRate, enum e_grabFormat grabFormat, t_uint32 nbFrames);
void ITE_ResetLRPipe();
void ITE_StartLRPipe(t_uint32 nbFrames);
void ITE_StopLRPipe(void);
void ITE_ConfigureHRPipe(t_uint32 FrameXSize, t_uint32 FrameYSize, t_uint32 frameRate, enum e_grabFormat grabFormat, t_uint32 nbFrames);
void ITE_ResetHRPipe();
void ITE_StartHRPipe(t_uint32 nbFrames);
void ITE_StopHRPipe(void);

void ITE_StillPreview(t_uint32 FrameXSize, t_uint32 FrameYSize, t_uint32 frameRate, enum e_grabFormat grabFormat, t_uint32 nbFrames);
void ITE_StillCapture(t_uint32 FrameXSize, t_uint32 FrameYSize, t_uint32 frameRate, enum e_grabFormat grabFormat, t_uint32 nbFrames);
void ITE_VideoPreview(t_uint32 FrameXSize, t_uint32 FrameYSize, t_uint32 frameRate, enum e_grabFormat grabFormat, t_uint32 nbFrames);
void ITE_VideoCapture(t_uint32 FrameXSize, t_uint32 FrameYSize, t_uint32 frameRate, enum e_grabFormat grabFormat, t_uint32 nbFrames);
*/

t_uint16 IsFormatCompatibleWithMCDE(enum e_grabFormat format);
void ITE_ConvertBufferFormatwithB2R2(tps_siapicturebuffer p_inputBuffer,tps_siapicturebuffer p_outputBuffer);
void ITE_WriteRGB16Buffer2GamFile(ts_siapicturebuffer gamBuffer, t_uint32 xSnapshot, t_uint32 ySnapshot, char *fileName);
void ITE_WaitUntilKeyPressed(void);
void ITE_TestValidFrameFlag(void);

#endif /*ITE_NMF_STANDARD_FUNCTIONS_H_ */
