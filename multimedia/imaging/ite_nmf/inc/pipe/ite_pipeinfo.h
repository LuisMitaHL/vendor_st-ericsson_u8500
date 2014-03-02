/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef ITE_PIPEINFO_H_
#define ITE_PIPEINFO_H_

//For Linux
#include <inc/type.h>

//#include <share/inc/type.h>
#include "grab_types.idt.h"
#include "VhcElementDefs.h"

//ported on 8500 V1
typedef struct _PipeControl_ {
   t_uint16 PipeOutputSize_X;
   t_uint16 PipeOutputSize_Y;
   t_uint8 OutputFormat_Pipe;
   t_uint8 Flag_TogglePixValid;
   t_uint8 PixValidLineTypes;
   t_uint8 Flag_Flip_Cb_Cr;
   t_uint8 Flag_Flip_Y_CbCr;
}ts_PipeControl;

typedef struct _SetupBank_ {
   t_uint32 uwPipeOutputSize_X;
   t_uint32 uwPipeOutputSize_Y;
   t_uint8  bPipeOutputFormat;
   t_uint8  bPipeStreamLength;
   t_bool   fSystemFirmwareSpecificSetup;
   t_uint8  bPixValidLineTypes;
   t_bool   fCb_Cr_Flip;
   t_bool   fY_CbCr_Flip;
   t_bool   fTogglePixValid;
}ts_SetupBank;

typedef struct _OutputPipe_ {
   ts_SetupBank   SetupBank;
   ts_PipeControl PipeControl;
}ts_OutputPipe, tps_OutputPipe;

typedef struct _InputPipe_ {
   t_uint32 uwInputImageSize_X;
   t_uint32 uwInputImageSize_Y;
   t_uint8  bActiveSensor;
   t_bool   fLowPowerStreaming;
   t_bool   fBayerBypass;
   t_uint8  bTestMode;
   t_uint8  bNumberOfStatusLines;
   t_uint8  bNumberOfDarkLines;
   t_uint8  bNumberOfBlackLines;
   t_uint32 uwNumberOfInterLinePixelClocks;
   t_uint32 uwNumberOfInterFrameLines;
   t_uint8  bNumberOfDummyColumns;
   t_uint8  bInputImageSource;
   t_uint8  bOutputImageDestination;
}ts_InputPipe;



typedef struct _BMSPipe_ {
   t_uint32 uwInputImageSize_X;
   t_uint32 uwInputImageSize_Y;
   t_uint32 uwPipeOutputSize_X;
   t_uint32 uwPipeOutputSize_Y;
   t_uint8  bStreamLength;
}ts_BMSPipe;


#ifdef __cplusplus
extern "C"
{
#endif

void ITE_SetActiveSensor(char sensor);
char ITE_GetActiveSensor(void);

void ITE_InitInputPipe(void);
void ITE_SendInputPipeConfig(void);

void ITE_InitOutputPipe(enum e_grabPipeID pipe, 
                        OutputFormat_te format, 
                        t_uint32 SizeX, 
                        t_uint32 SizeY, 
                        t_uint32 StreamLength);
void ITE_SendOutputPipeConfig(enum e_grabPipeID pipe);
void ITE_InitBMSPipe(t_uint32 SizeX, 
                     t_uint32 SizeY, 
                     t_uint32 StreamLength);
void ITE_SendBMSPipeConfig(void);

void ITE_InitializeOutputPipe(enum e_grabPipeID pipe, 
                           OutputFormat_te format, 
                           t_uint32 SizeX, 
                           t_uint32 SizeY);
						   
void ITE_SendOutputPipeConfig(enum e_grabPipeID pipe);						 

void ITE_SendDatapath(void);

void ITE_ActivePipe(enum e_grabPipeID pipe,t_uint32 enable);

#ifdef __cplusplus
}
#endif

#endif /* ITE_PIPEINFO_H_ */
