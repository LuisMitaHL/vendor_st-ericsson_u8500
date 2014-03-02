/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include "ite_pipeinfo.h"
#include "ite_sensorinfo.h"
#include "ite_vpip.h"
#include "ispctl_types.idt.h"
#include "ite_sia_init.h"

#include <los/api/los_api.h>

//ported on 8500 V1
ts_PipeControl OutputPipeHRInfo;
ts_PipeControl OutputPipeLRInfo;

ts_InputPipe  InputPipeInfo;
ts_BMSPipe  BMSPipeInfo;

extern ts_sensInfo SensorsInfo[2];

/********************************************************/
/* 		Private functions declaration 		*/
/********************************************************/
void ITE_InitPipeControl(enum e_grabPipeID pipe);
void ITE_InitPipeSetupBank(enum e_grabPipeID pipe, 
                           OutputFormat_te format, 
                           t_uint32 SizeX, 
                           t_uint32 SizeY, 
                           t_uint32 StreamLength);
void ITE_SendPipeControlConfig(enum e_grabPipeID pipe);
void ITE_SendPipeSetupBankConfig(enum e_grabPipeID pipe);
/*******************************************************/


/********************************************************/
/* 		"Public" functions 	 		*/
/********************************************************/


/**************************************/
/* ITE_SetActiveSensor(char sensor)   */
/* Set wich sensor will be active     */
/*   <1> Sensor_0		      */
/*   <2> Sensor_1		      */
/**************************************/
void ITE_SetActiveSensor(char sensor){
   InputPipeInfo.bActiveSensor = sensor;
}

/**************************************/
/* ITE_GetActiveSensor(char sensor)   */
/* Get wich sensor is active          */
/*   <1> Sensor_0		      */
/*   <2> Sensor_1		      */
/**************************************/
char ITE_GetActiveSensor(void){
   return InputPipeInfo.bActiveSensor;
}

/**************************************/
/* ITE_ActivePipe(enum e_grabPipeID pipe,     */
/*  		  int enable)	      */
/* enable or disable pipe HR or LR    */
/**************************************/
//ported on 8500 V1
void ITE_ActivePipe(enum e_grabPipeID pipe, 
                    t_uint32 enable){
	
//ts_OutputPipe* MyPointer = NULL;
   
if(pipe == GRBPID_PIPE_HR)
{
   //MyPointer = &OutputPipeHRInfo;
   //MyPointer->PipeControl.bPipeControl = enable;
   ITE_writePE(DataPathControl_e_Flag_Pipe0Enable_Byte0, enable);
   
}
else
{
   //MyPointer = &OutputPipeLRInfo;
   //MyPointer->PipeControl.bPipeControl = enable;
   ITE_writePE(DataPathControl_e_Flag_Pipe1Enable_Byte0, enable);

}


}



/**************************************/
/* ITE_InitInputPipe() 		      */
/* Initialize InputPipeInfo structure */
/**************************************/
void ITE_InitInputPipe(void){

   int IndexAcitveSensor = (int) ITE_GetActiveSensor() - 1; //__NO_WARNING__
   
   InputPipeInfo.uwInputImageSize_X = (t_uint32) (SensorsInfo[IndexAcitveSensor].config.inputImageSizeX);
   InputPipeInfo.uwInputImageSize_Y = (t_uint32) SensorsInfo[IndexAcitveSensor].config.inputImageSizeY;

   InputPipeInfo.fLowPowerStreaming = (t_bool)FALSE;
   InputPipeInfo.fBayerBypass = (t_bool)FALSE;
   InputPipeInfo.bTestMode = 0;
/* TODO : Initialisé dans le futur si besoin */
/*   InputPipeInfo.bNumberOfStatusLines;
   InputPipeInfo.bNumberOfDarkLines;
   InputPipeInfo.bNumberOfBlackLines;
   InputPipeInfo.uwNumberOfInterLinePixelClocks;
   InputPipeInfo.uwNumberOfInterFrameLines;
   InputPipeInfo.bNumberOfDummyColumns;
   InputPipeInfo.bInputImageSource;
   InputPipeInfo.bOutputImageDestination;
*/
}

/**************************************/
/* ITE_SendInputPipeConfig() 	      */
/* Send InputPipeInfo structure into  */
/* corresponding PEs		      */
/**************************************/
/*
void ITE_SendInputPipeConfig(void){

   int count = 0;
   ts_PageElement tab_pe[ISPCTL_SIZE_TAB_PE] = {0,0};
   
   tab_pe[count].pe_addr   =  ModeSetupBank0_uwInputImageSize_X_MSByte;
   tab_pe[count++].pe_data =  InputPipeInfo.uwInputImageSize_X;
   tab_pe[count].pe_addr   =  ModeSetupBank0_uwInputImageSize_Y_MSByte;
   tab_pe[count++].pe_data =  InputPipeInfo.uwInputImageSize_Y;
   tab_pe[count].pe_addr   =  ModeSetupBank0_fLowPowerStreaming;
   tab_pe[count++].pe_data =  InputPipeInfo.fLowPowerStreaming;
   tab_pe[count].pe_addr   =  ModeSetupBank0_fBayerBypass;
   tab_pe[count++].pe_data =  InputPipeInfo.fBayerBypass;
   tab_pe[count].pe_addr   =  ModeSetupBank0_bTestMode;
   tab_pe[count++].pe_data =  InputPipeInfo.bTestMode;
  
   
   tab_pe[count].pe_addr   =  ModeSetupBank0_bActiveSensor;
   tab_pe[count++].pe_data =  ITE_GetActiveSensor();
   
   ITE_writeListPE(tab_pe, count);
  
}
*/

/****************************************/
/* ITE_InitOutputPipe(...) 	      	*/
/* Initialize Output Pipe structure   	*/
/* for pipe HR or LR  		      	*/
/* according to output format	      	*/
/* 		output size x and y   	*/
/*		streamlength(0:infiny)	*/
/*					*/
/****************************************/
/*
void ITE_InitOutputPipe(enum e_grabPipeID pipe, 
                        PipeOutputFormat_e format, 
                        t_uint32 SizeX, 
                        t_uint32 SizeY, 
                        t_uint32 StreamLength){

if (pipe == GRBPID_PIPE_RAW_OUT) // BMS Settings
   LOS_Log("\nERROR function ITE_InitOutputPipe could not be used for BMS \n",NULL,NULL,NULL,NULL,NULL,NULL,NULL);
else {				 // HR and LR
   ITE_InitPipeControl(pipe);
   ITE_InitPipeSetupBank(pipe, format, SizeX, SizeY, StreamLength);
	}
}
*/


/**************************************/
/* ITE_SendOutputPipeConfig(pipe)     */
/* Send PipeControl & PipeSetupBank   */
/* structure into corresponding PEs   */
/**************************************/
/*
void ITE_SendOutputPipeConfig(enum e_grabPipeID pipe){

if (pipe == GRBPID_PIPE_RAW_OUT) // BMS Settings
   LOS_Log("\nERROR function ITE_SendOutputPipeConfig could not be used for BMS \n",NULL,NULL,NULL,NULL,NULL,NULL,NULL);
else {				 // HR and LR
	ITE_SendPipeControlConfig(pipe);
	ITE_SendPipeSetupBankConfig(pipe);	
	}
}
*/

/****************************************/
/* ITE_InitBMSPipe(...) 	      	*/
/* Initialize BMSPipe structure   	*/
/* for BMS	 		      	*/
/* according to output format	      	*/
/* 		output size x and y   	*/
/*		streamlength(0:infiny)	*/
/*					*/
/****************************************/
void ITE_InitBMSPipe(   t_uint32 SizeX, 
                        t_uint32 SizeY, 
                        t_uint32 StreamLength){

BMSPipeInfo.uwInputImageSize_X = SizeX;
BMSPipeInfo.uwInputImageSize_Y = SizeY;
BMSPipeInfo.bStreamLength = StreamLength;
// Not sure we need following PEs
BMSPipeInfo.uwPipeOutputSize_X = SizeX;
BMSPipeInfo.uwPipeOutputSize_Y = SizeY;
}


/****************************************/
/* ITE_SendBMSPipeConfig(pipe)    	*/
/* Send BMS			   	*/
/* structure into corresponding PEs   	*/
/****************************************/
/*
void ITE_SendBMSPipeConfig(void)
{
   int count = 0;
   ts_PageElement tab_pe[ISPCTL_SIZE_TAB_PE] = {0,0};
   tab_pe[count].pe_addr   =  ModeSetupBank0_uwInputImageSize_X_MSByte;
   tab_pe[count++].pe_data =  BMSPipeInfo.uwInputImageSize_X;
   tab_pe[count].pe_addr   =  ModeSetupBank0_uwInputImageSize_Y_MSByte;
   tab_pe[count++].pe_data =  BMSPipeInfo.uwInputImageSize_Y;
   tab_pe[count].pe_addr   =  Pipe0SetupBank_uwPipeOutputSize_X_MSByte;
   tab_pe[count++].pe_data =  BMSPipeInfo.uwPipeOutputSize_X;
   tab_pe[count].pe_addr   =  Pipe0SetupBank_uwPipeOutputSize_Y_MSByte;
   tab_pe[count++].pe_data =  BMSPipeInfo.uwPipeOutputSize_Y;
   tab_pe[count].pe_addr   =  RunModeControl_bStreamLength;
   tab_pe[count++].pe_data =  BMSPipeInfo.bStreamLength;
   
   ITE_writeListPE(tab_pe, count);

}

*/

/********************************************************/
/* 		"Private" functions 	 		*/
/********************************************************/

/**************************************/
/* ITE_InitPipeControl(pipe) 	      */
/* Initialize PipeControl structure   */
/* for pipe HR or LR  		      */
/**************************************/
/*
void ITE_InitPipeControl(enum e_grabPipeID pipe){

   ts_OutputPipe* MyPointer = NULL;
   
   if(pipe == GRBPID_PIPE_HR){
      MyPointer = &OutputPipeHRInfo;
   }else{
      MyPointer = &OutputPipeLRInfo;
   }
#if !(defined(__PEPS8500_SIA) || defined(_SVP_))
   MyPointer->PipeControl.bPipeControl = PipeControl_Enable;
   MyPointer->PipeControl.fSfxSolariseEnabled = (t_bool)FALSE;
   MyPointer->PipeControl.fSfxNegativeEnabled = (t_bool)FALSE;
#endif
*/   
/*
   MyPointer->PipeControl.bEmboss ;  //??
   MyPointer->PipeControl.bGreyBack ;  //??
   MyPointer->PipeControl.bFlipper ;  //??
   MyPointer->PipeControl.ReplaceRedChannel ;  //??
   MyPointer->PipeControl.ReplaceGreenChannel ;  //??
   MyPointer->PipeControl.ReplaceBlueChannel ;  //??
*/
//}

/****************************************/
/* ITE_InitPipeSetupBank(...) 	        */
/* Initialize PipeSetupBank structure   */
/* for pipe HR or LR  		        */
/* according to output format	        */
/* 		output size x and y     */
/*		streamlength(255:infiny)*/
/****************************************/
/*
void ITE_InitPipeSetupBank(enum e_grabPipeID pipe, 
                           OutputFormat_te format, 
                           t_uint32 SizeX, 
                           t_uint32 SizeY, 
                           t_uint32 StreamLength){

t_uint32 PipeStreamLength;
ts_OutputPipe* MyPointer = NULL;
   
   if (StreamLength == INFINITY)
      PipeStreamLength = 0;    // for Pipe stram infinity = 0
   else 
      PipeStreamLength = StreamLength;
   if(pipe == GRBPID_PIPE_HR){
      MyPointer = &OutputPipeHRInfo;
   }else{
      MyPointer = &OutputPipeLRInfo;
   }

   MyPointer->SetupBank.uwPipeOutputSize_X = SizeX;
   MyPointer->SetupBank.uwPipeOutputSize_Y = SizeY;
   MyPointer->SetupBank.bPipeOutputFormat = format;
   MyPointer->SetupBank.bPipeStreamLength = PipeStreamLength;
  // MyPointer->SetupBank.fSystemFirmwareSpecificSetup ; // ??
   MyPointer->SetupBank.bPixValidLineTypes = 0x20;
   MyPointer->SetupBank.fCb_Cr_Flip = (t_bool)FALSE;
   MyPointer->SetupBank.fY_CbCr_Flip = (t_bool)FALSE;
   MyPointer->SetupBank.fTogglePixValid = (t_bool)FALSE;

}
*/

/**************************************/
/* ITE_SendPipeControlConfig(pipe)    */
/* Send PipeControl structure         */
/* into corresponding PEs   	      */
/**************************************/
/*
void ITE_SendPipeControlConfig(enum e_grabPipeID pipe){

int count = 0;
ts_PageElement tab_pe[ISPCTL_SIZE_TAB_PE] = {0,0}; 
#if !(defined(__PEPS8500_SIA) || defined(_SVP_))
 if (pipe == GRBPID_PIPE_HR) {  
   tab_pe[count].pe_addr   =  Pipe0Control_bPipeControl;
   tab_pe[count++].pe_data =  OutputPipeHRInfo.PipeControl.bPipeControl;
   tab_pe[count].pe_addr   =  Pipe0Control_fSfxSolariseEnabled;
   tab_pe[count++].pe_data =  OutputPipeHRInfo.PipeControl.fSfxSolariseEnabled;
   tab_pe[count].pe_addr   =  Pipe0Control_fSfxNegativeEnabled;
   tab_pe[count++].pe_data =  OutputPipeHRInfo.PipeControl.fSfxNegativeEnabled;
   
   tab_pe[count].pe_addr   =  Pipe0Control_bEmboss;
   tab_pe[count++].pe_data =  OutputPipeHRInfo.PipeControl.bEmboss;
   tab_pe[count].pe_addr   =  Pipe0Control_bGreyBack;
   tab_pe[count++].pe_data =  OutputPipeHRInfo.PipeControl.bGreyBack;
   tab_pe[count].pe_addr   =  Pipe0Control_bFlipper;
   tab_pe[count++].pe_data =  OutputPipeHRInfo.PipeControl.bFlipper;
   tab_pe[count].pe_addr   =  Pipe0Control_ReplaceRedChannel;
   tab_pe[count++].pe_data =  OutputPipeHRInfo.PipeControl.ReplaceRedChannel;
   tab_pe[count].pe_addr   =  Pipe0Control_ReplaceGreenChannel;
   tab_pe[count++].pe_data =  OutputPipeHRInfo.PipeControl.ReplaceGreenChannel;
   tab_pe[count].pe_addr   =  Pipe0Control_ReplaceBlueChannel;
   tab_pe[count++].pe_data =  OutputPipeHRInfo.PipeControl.ReplaceBlueChannel;
   

    }
 else { // PIPE LR
   tab_pe[count].pe_addr   =  Pipe1Control_bPipeControl;
   tab_pe[count++].pe_data =  OutputPipeLRInfo.PipeControl.bPipeControl;
   tab_pe[count].pe_addr   =  Pipe1Control_fSfxSolariseEnabled;
   tab_pe[count++].pe_data =  OutputPipeLRInfo.PipeControl.fSfxSolariseEnabled;
   tab_pe[count].pe_addr   =  Pipe1Control_fSfxNegativeEnabled;
   tab_pe[count++].pe_data =  OutputPipeLRInfo.PipeControl.fSfxNegativeEnabled;
   
   tab_pe[count].pe_addr   =  Pipe1Control_bEmboss;
   tab_pe[count++].pe_data =  OutputPipeLRInfo.PipeControl.bEmboss;
   tab_pe[count].pe_addr   =  Pipe1Control_bGreyBack;
   tab_pe[count++].pe_data =  OutputPipeLRInfo.PipeControl.bGreyBack;
   tab_pe[count].pe_addr   =  Pipe1Control_bFlipper;
   tab_pe[count++].pe_data =  OutputPipeLRInfo.PipeControl.bFlipper;
   tab_pe[count].pe_addr   =  Pipe1Control_ReplaceRedChannel;
   tab_pe[count++].pe_data =  OutputPipeLRInfo.PipeControl.ReplaceRedChannel;
   tab_pe[count].pe_addr   =  Pipe1Control_ReplaceGreenChannel;
   tab_pe[count++].pe_data =  OutputPipeLRInfo.PipeControl.ReplaceGreenChannel;
   tab_pe[count].pe_addr   =  Pipe1Control_ReplaceBlueChannel;
   tab_pe[count++].pe_data =  OutputPipeLRInfo.PipeControl.ReplaceBlueChannel;
       }
 ITE_writeListPE(tab_pe, count);
 #endif
}
*/

/**************************************/
/* ITE_SendPipeSetupBankConfig(pipe)  */
/* Send PipeSetupBank structure       */
/* into corresponding PEs   	      */
/**************************************/
/*
void ITE_SendPipeSetupBankConfig(enum e_grabPipeID pipe){

int count = 0;
ts_PageElement tab_pe[ISPCTL_SIZE_TAB_PE] = {0,0}; 
#if !(defined(__PEPS8500_SIA) || defined(_SVP_))
 if (pipe == GRBPID_PIPE_HR) {  
   tab_pe[count].pe_addr   =  Pipe0SetupBank_uwPipeOutputSize_X_MSByte;
   tab_pe[count++].pe_data =  OutputPipeHRInfo.SetupBank.uwPipeOutputSize_X;
   tab_pe[count].pe_addr   =  Pipe0SetupBank_uwPipeOutputSize_Y_MSByte;
   tab_pe[count++].pe_data =  OutputPipeHRInfo.SetupBank.uwPipeOutputSize_Y;
   tab_pe[count].pe_addr   =  Pipe0SetupBank_bPipeOutputFormat;
   tab_pe[count++].pe_data =  OutputPipeHRInfo.SetupBank.bPipeOutputFormat;
   tab_pe[count].pe_addr   =  Pipe0SetupBank_bPipeStreamLength;
   tab_pe[count++].pe_data =  OutputPipeHRInfo.SetupBank.bPipeStreamLength;
   tab_pe[count].pe_addr   =  Pipe0SetupBank_fSystemFirmwareSpecificSetup;
   tab_pe[count++].pe_data =  OutputPipeHRInfo.SetupBank.fSystemFirmwareSpecificSetup;
   tab_pe[count].pe_addr   =  Pipe0SetupBank_bPixValidLineTypes;
   tab_pe[count++].pe_data =  OutputPipeHRInfo.SetupBank.bPixValidLineTypes;
   tab_pe[count].pe_addr   =  Pipe0SetupBank_fCb_Cr_Flip;
   tab_pe[count++].pe_data =  OutputPipeHRInfo.SetupBank.fCb_Cr_Flip;
   tab_pe[count].pe_addr   =  Pipe0SetupBank_fY_CbCr_Flip;
   tab_pe[count++].pe_data =  OutputPipeHRInfo.SetupBank.fY_CbCr_Flip;
   tab_pe[count].pe_addr   =  Pipe0SetupBank_fTogglePixValid;
   tab_pe[count++].pe_data =  OutputPipeHRInfo.SetupBank.fTogglePixValid;
   }
 else { // PIPE LR
   tab_pe[count].pe_addr   =  Pipe1SetupBank_uwPipeOutputSize_X_MSByte;
   tab_pe[count++].pe_data =  OutputPipeLRInfo.SetupBank.uwPipeOutputSize_X;
   tab_pe[count].pe_addr   =  Pipe1SetupBank_uwPipeOutputSize_Y_MSByte;
   tab_pe[count++].pe_data =  OutputPipeLRInfo.SetupBank.uwPipeOutputSize_Y;
   tab_pe[count].pe_addr   =  Pipe1SetupBank_bPipeOutputFormat;
   tab_pe[count++].pe_data =  OutputPipeLRInfo.SetupBank.bPipeOutputFormat;
   tab_pe[count].pe_addr   =  Pipe1SetupBank_bPipeStreamLength;
   tab_pe[count++].pe_data =  OutputPipeLRInfo.SetupBank.bPipeStreamLength;
   tab_pe[count].pe_addr   =  Pipe1SetupBank_fSystemFirmwareSpecificSetup;
   tab_pe[count++].pe_data =  OutputPipeLRInfo.SetupBank.fSystemFirmwareSpecificSetup;
   tab_pe[count].pe_addr   =  Pipe1SetupBank_bPixValidLineTypes;
   tab_pe[count++].pe_data =  OutputPipeLRInfo.SetupBank.bPixValidLineTypes;
   tab_pe[count].pe_addr   =  Pipe1SetupBank_fCb_Cr_Flip;
   tab_pe[count++].pe_data =  OutputPipeLRInfo.SetupBank.fCb_Cr_Flip;
   tab_pe[count].pe_addr   =  Pipe1SetupBank_fY_CbCr_Flip;
   tab_pe[count++].pe_data =  OutputPipeLRInfo.SetupBank.fY_CbCr_Flip;
   tab_pe[count].pe_addr   =  Pipe1SetupBank_fTogglePixValid;
   tab_pe[count++].pe_data =  OutputPipeLRInfo.SetupBank.fTogglePixValid;
   }
 ITE_writeListPE(tab_pe, count);
 #endif
}
*/

/****************************************/
/* ITE_InitializeOutputPipe(...) 	        */
/* Initialize g_Pipe[x] structure   */
/* for pipe HR or LR  		        */
/* according to output format	        */
/* 		output size x and y     */
/****************************************/
//New for 8500 v1
void ITE_InitializeOutputPipe(enum e_grabPipeID pipe, 
                           OutputFormat_te format, 
                           t_uint32 SizeX, 
                           t_uint32 SizeY)
{

ts_PipeControl* MyPointer = NULL;
   
   if(pipe == GRBPID_PIPE_HR){
      MyPointer = &OutputPipeHRInfo;
   }else{
      MyPointer = &OutputPipeLRInfo;
   }

   MyPointer->PipeOutputSize_X = SizeX;
   MyPointer->PipeOutputSize_Y = SizeY;
   MyPointer->OutputFormat_Pipe = format;
   MyPointer->Flag_TogglePixValid = (t_bool)FALSE;
   //MyPointer->PixValidLineTypes = (t_bool)FALSE;
   MyPointer->Flag_Flip_Cb_Cr = (t_bool)FALSE;
   MyPointer->Flag_Flip_Y_CbCr = (t_bool)FALSE;

}

/**************************************/
/* ITE_SendPipeControlConfig(pipe)    */
/* Send PipeControl structure         */
/* into corresponding PEs   	      */
/**************************************/
//ported on 8500 V1
void ITE_SendOutputPipeConfig(enum e_grabPipeID pipe){

int count = 0,array_size;
ts_PageElement tab_pe[ISPCTL_SIZE_TAB_PE/* 32 */];

//__NO_WARNING__
for(array_size =0 ; array_size<=32 ; array_size++)
 {
  tab_pe[array_size].pe_addr = 0;
  tab_pe[array_size].pe_data = 0;
 }

 if (pipe == GRBPID_PIPE_HR) {  
   tab_pe[count].pe_addr   =  Pipe_0_u16_X_size_Byte0;
   tab_pe[count++].pe_data =  OutputPipeHRInfo.PipeOutputSize_X;
   tab_pe[count].pe_addr   =  Pipe_0_u16_Y_size_Byte0;
   tab_pe[count++].pe_data =  OutputPipeHRInfo.PipeOutputSize_Y;
   tab_pe[count].pe_addr   =  Pipe_0_e_OutputFormat_Pipe_Byte0;
   tab_pe[count++].pe_data =  OutputPipeHRInfo.OutputFormat_Pipe;
   tab_pe[count].pe_addr   =  Pipe_0_e_Flag_TogglePixValid_Byte0;
   tab_pe[count++].pe_data =  OutputPipeHRInfo.Flag_TogglePixValid;
   /*
   tab_pe[count].pe_addr   =  Pipe_0_u8_PixValidLineTypes_Byte0;
   tab_pe[count++].pe_data =  OutputPipeHRInfo.PixValidLineTypes;
   */
   tab_pe[count].pe_addr   =  Pipe_0_e_Flag_Flip_Cb_Cr_Byte0;
   tab_pe[count++].pe_data =  OutputPipeHRInfo.Flag_Flip_Cb_Cr;
   tab_pe[count].pe_addr   =  Pipe_0_e_Flag_Flip_Y_CbCr_Byte0;
   tab_pe[count++].pe_data =  OutputPipeHRInfo.Flag_Flip_Y_CbCr;
   

    }
 else { // PIPE LR
   tab_pe[count].pe_addr   =  Pipe_1_u16_X_size_Byte0;
   tab_pe[count++].pe_data =  OutputPipeLRInfo.PipeOutputSize_X;
   tab_pe[count].pe_addr   =  Pipe_1_u16_Y_size_Byte0;
   tab_pe[count++].pe_data =  OutputPipeLRInfo.PipeOutputSize_Y;
   tab_pe[count].pe_addr   =  Pipe_1_e_OutputFormat_Pipe_Byte0;
   tab_pe[count++].pe_data =  OutputPipeLRInfo.OutputFormat_Pipe;
   tab_pe[count].pe_addr   =  Pipe_1_e_Flag_TogglePixValid_Byte0;
   tab_pe[count++].pe_data =  OutputPipeLRInfo.Flag_TogglePixValid;
   /*
   tab_pe[count].pe_addr   =  Pipe_1_u8_PixValidLineTypes_Byte0;
   tab_pe[count++].pe_data =  OutputPipeLRInfo.PixValidLineTypes;
   */
   tab_pe[count].pe_addr   =  Pipe_1_e_Flag_Flip_Cb_Cr_Byte0;
   tab_pe[count++].pe_data =  OutputPipeLRInfo.Flag_Flip_Cb_Cr;
   tab_pe[count].pe_addr   =  Pipe_1_e_Flag_Flip_Y_CbCr_Byte0;
   tab_pe[count++].pe_data =  OutputPipeLRInfo.Flag_Flip_Y_CbCr;
    }
 ITE_writeListPE(tab_pe, count);
}
