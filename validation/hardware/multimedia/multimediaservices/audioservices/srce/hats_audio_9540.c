/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides routines to manage audio via alsa framework
* \author  ST-Ericsson
*/
/*****************************************************************************/
#define HATS_AUDIO_C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>
#include "hats_audio_9540.h"
#include "hats_audio_9540.hi"

e_HatsAudioInput_t In_Organ = CODEC_SRC_LINEIN;
e_HatsAudioOutput_t Out_Organ = CODEC_DEST_HEADSET;
 
/*--------------------------------------------------------------------------------------*/
/*! audio_services internal functions
*/
/*--------------------------------------------------------------------------------------*/

uint32_t StartCommand(char* pcommand)
{
	int32_t status = HATS_AUDIO_NO_ERROR;
	char command[MAX_LGTH_COMMAND]="";
	uint32_t PID;
	MC_TRACE_DBG(printf("AUDIOSERVICES StartCommand: file=%s line=%d \n", __FILE__, __LINE__));

	snprintf(command,MAX_LGTH_COMMAND, "%s &", pcommand);	
	system(command);
	printf("AUDIOSERVICES CMD:%s\n",command);
	
	/* Check PID*/
	sleep(1);
	GetPID(pcommand,&PID);               
	if(PID == 0)
	{
		/* no ID found means error during playback processing*/
		printf( "\nAUDIOSERVICES ERROR: error during audio services command\n");
		status = HATS_AUDIO_FAILED;
	}
	
	return status;
}


uint32_t StopCommand(char* pcommand)
{
	int32_t status = HATS_AUDIO_NO_ERROR;
	char command[MAX_LGTH_COMMAND]="";
	MC_TRACE_DBG(printf("AUDIOSERVICES StopCommand: file=%s line=%d \n", __FILE__, __LINE__));

	snprintf(command,MAX_LGTH_COMMAND,"%s",pcommand);         
	KillPID(command);
	printf ("AUDIOSERVICES: KillPID %s \n", command);
	
	return status;
}

uint32_t ExecuteScript(const char* ScriptPath, char * ResultPath)
{
	int32_t status = HATS_AUDIO_NO_ERROR;
	char command[MAX_LGTH_COMMAND]="";
	MC_TRACE_DBG(printf("AUDIOSERVICES ExecuteScript: file=%s line=%d \n", __FILE__, __LINE__));

	/* delete result file*/
	if (ResultPath!=NULL)
	{
		snprintf(command,MAX_LGTH_COMMAND, " rm -f %s", ResultPath);
		system(command);
	}
	
	/* execute script and log in the result file */
	if (strlen(ScriptPath)!=0)
	{
		if (ResultPath!=NULL)
			snprintf(command,MAX_LGTH_COMMAND, "%s > %s", ScriptPath, ResultPath);
		else
			snprintf(command,MAX_LGTH_COMMAND, "%s", ScriptPath);		
		system(command);
		printf ("AUDIOSERVICES SCRIPT:%s \n", command);
	}
	else
	{
		printf ("AUDIOSERVICES ERROR: no script to execute!\n");
		status = HATS_AUDIO_NOT_AVAILABLE;
	}

	/* check the status of the script execution*/
	/* status = checkstatus(ResultPath);*/
	
	return status;
}

/*--------------------------------------------------------------------------------------*/
/*! \ingroup audio_services
\fn uint32_t StartPCMTransfer(e_HatsAudioDevice_t Device,e_HatsAudioCodecType_t Type,e_HatsAudioMode_t Mode,char * FilePath)
\brief Service allowing to initiate a 48 kHz PCM transfer : playback or capture
*/
/*--------------------------------------------------------------------------------------*/
uint32_t StartPCMTransfer(e_HatsAudioDevice_t Device,e_HatsAudioCodecType_t Type,e_HatsAudioMode_t Mode,char * FilePath)
{
	int32_t status = HATS_AUDIO_NO_ERROR;
	MC_TRACE_DBG(printf("AUDIOSERVICES StartPCMTransfer: file=%s line=%d \n", __FILE__, __LINE__));

	/* Check action */
	if(  (Device >= NB_OF_AUDIO_DEVICE)
			||(Type > CODEC_CAPTURE)
			||(Mode >= NB_OF_DATA_MODE))
	{
		printf("AUDIOSERVICES ERROR: one parameter is out of range : DEVICE = %d  TYPE = %d  MODE = %d\n",Device,Type,Mode );        
		return HATS_AUDIO_INVALID_PARAMETER;   
	}   

	if(Type == CODEC_PLAYBACK)
	{
		status = StartPCMPlayback(Device,Mode,FilePath);
	}
	else
	{
		status = StartPCMCapture(Device,Mode,FilePath);
	}
	return status;
}


/*--------------------------------------------------------------------------------------*/
/*! \ingroup audio_services
\fn uint32_t StopPCMTransfer(e_HatsAudioDevice_t Device,, e_HatsAudioCodecType_t Type)
\brief Service allowing to stop a PCM transfer : playback or capture
*/
/*--------------------------------------------------------------------------------------*/
uint32_t StopPCMTransfer(e_HatsAudioDevice_t Device, e_HatsAudioCodecType_t Type)
{	
	int32_t status = HATS_AUDIO_NO_ERROR;
	MC_TRACE_DBG(printf("AUDIOSERVICES StopPCMTransfer: file=%s line=%d \n", __FILE__, __LINE__));

	/* check action */
	if(  (Device >= NB_OF_AUDIO_DEVICE)
			||(Type > CODEC_CAPTURE))
	{
		printf("AUDIOSERVICES ERROR: one parameter is out of range : DEVICE = %d  TYPE = %d\n",Device,Type );        
		return HATS_AUDIO_INVALID_PARAMETER;   
	}

	
	if((Type == CODEC_PLAYBACK) && (hats_audio_contex[Device].PlaybackMode != HATS_AUDIO_UNDEFINED_MODE))
	{
		if(hats_audio_contex[Device].PlaybackMode == DIGITAL_LOOPBACK_MODE)
		{
			StopCommand("aplay");
			StopCommand("arecord");
		}
		else if(hats_audio_contex[Device].PlaybackMode == AB_DIGITAL_LOOPBACK_MODE)
		{
			status= ExecuteScript(HATS_AB_DIGITAL_LOOP_STOP_FILE, "/tmp/AB_DIGITAL_LOOPBACK_MODE.txt");			
		}
		else
		{
			if(hats_audio_contex[Device].PlaybackMode !=FM_PLAYBACK_MODE)  /* ?? */
			{
				StopCommand("aplay");
			}
			
			/* deactivate power */
			if(Device ==AUDIO_DEVICE_HDMI)
			{
				PowerHDMIaudioInput(HATS_HDMI_AUDIO_OFF);      
			}
			
		}   
		hats_audio_contex[Device].PlaybackMode= HATS_AUDIO_UNDEFINED_MODE;       
	}

	else if((Type == CODEC_CAPTURE) && (hats_audio_contex[Device].CaptureMode!= HATS_AUDIO_UNDEFINED_MODE))
	{
		if(hats_audio_contex[Device].CaptureMode== ANALOG_LOOPBACK_MODE)
		{	  
			status= ExecuteScript(HATS_AB_ANALOG_LOOP_STOP_FILE, "/tmp/ANALOG_LOOPBACK_MODE.txt");			

		}
		else
		{
			status= StopCommand("arecord");
		}
		hats_audio_contex[Device].CaptureMode= HATS_AUDIO_UNDEFINED_MODE;     
	}

	else
	{
		printf("AUDIOSERVICES ERROR: invalid command codec type = %d, or invalid context\n",Type);
		return HATS_AUDIO_INVALID_PARAMETER;
	}


	return status;
}


/*--------------------------------------------------------------------------------------*/
/*!   \ingroup internal_audio_services
\fn uint32_t StartPCMPlayback(e_HatsAudioDevice_t Device,e_HatsAudioMode_t Mode,char * FilePath)
\brief start a PCM playback 
\param [in] Device Id \ref e_HatsAudioDevice_t
\param [in] Mode refer to \ref e_HatsAudioMode_t
\param [in] FilePath string : file location
\return   
	HATS_AUDIO_NO_ERROR if no error
*/
/*--------------------------------------------------------------------------------------*/
uint32_t StartPCMPlayback(e_HatsAudioDevice_t Device,e_HatsAudioMode_t Mode,char * FilePath)
{
	char command[MAX_LGTH_COMMAND]="";
	int32_t status = HATS_AUDIO_NO_ERROR;
	uint32_t PID;
	hats_alsa_Device_t AlsaDevice;
	uint32_t i;
	MC_TRACE_DBG(printf("AUDIOSERVICES StartPCMPlayback: file=%s line=%d \n", __FILE__, __LINE__));

	/* Get Device numbers */
	GetAlsaDevice(Device,CODEC_PLAYBACK,&AlsaDevice);

	/* Check mode */
	for(i= 0; i <HATS_AUDIO_NB_PB_MODE;i++)
	{
		if(Mode == hats_audio_PB_mode[i].ID)
		{
			if(hats_audio_PB_mode[i].Status != HATS_AUDIO_NO_ERROR)
			{
				printf("AUDIOSERVICES ERROR: %s not implemented at this time\n", hats_audio_PB_mode[i].name);
				status = HATS_AUDIO_NOT_AVAILABLE;
			}
			hats_audio_contex[Device].PlaybackMode = Mode;
			snprintf(hats_audio_contex[Device].PlaybackFile, MAX_LGTH_COMMAND, FilePath);
			break;
		}
	}
	if(i == HATS_AUDIO_NB_PB_MODE)
	{
		printf("AUDIOSERVICES ERROR: invalid playback mode %d\n", Mode);
		status = HATS_AUDIO_INVALID_PARAMETER;
	}

	if( status == HATS_AUDIO_NO_ERROR)
	{
		switch(Mode)
		{
		case NORMAL_PLAYBACK_MODE:
		case FM_TX_MODE:
		
			/* check resource by verifying that no PID associated to command is already running */                                                 
			snprintf(command,MAX_LGTH_COMMAND,"%s -D%s",PCM_PLAYER, AlsaDevice.name);         
			GetPID(command,&PID);
	
			if (PID != 0)
			{
				/*Error: ressource already used*/
				printf( "AUDIOSERVICES ERROR: device %s already in used\n",AlsaDevice.name);
				return HATS_AUDIO_DEVICE_ALREADY_USED;
			}
					
			if(Device ==AUDIO_DEVICE_HDMI)
			{
				PowerHDMIaudioInput(HATS_HDMI_AUDIO_ON);      
			}
			
			snprintf(command,MAX_LGTH_COMMAND,"%s -D%s %s",PCM_PLAYER, AlsaDevice.name, FilePath);    	
			status=StartCommand(command);	
			
			break;
			
		case DIGITAL_LOOPBACK_MODE:
	
			/* check resource by verifying that no PID associated to command is already running */                                                 
			snprintf(command,MAX_LGTH_COMMAND,"%s",ARECORD_FOR_DIGITAL_LOOPBACK);         
			GetPID(command,&PID);
	
			if (PID != 0)
			{
				/*Error: ressource already used*/
				printf( "AUDIOSERVICES ERROR: device %s already in used\n",AlsaDevice.name);
				return HATS_AUDIO_DEVICE_ALREADY_USED;
			}
			else
			{
				snprintf(command,MAX_LGTH_COMMAND,"%s",APLAY_FOR_DIGITAL_LOOPBACK);         
				GetPID(command,&PID);
	
				if (PID != 0)
				{
					/*Error: ressource already used*/
					printf( "AUDIOSERVICES ERROR: device %s already in used\n",AlsaDevice.name);
					return HATS_AUDIO_DEVICE_ALREADY_USED;
				}
			}
			
			snprintf(command,MAX_LGTH_COMMAND,"%s|%s",ARECORD_FOR_DIGITAL_LOOPBACK, APLAY_FOR_DIGITAL_LOOPBACK);         
			status=StartCommand(command);	
			break;
			
		case AB_DIGITAL_LOOPBACK_MODE:
			status= ExecuteScript(Dloop_configs[Out_Organ][In_Organ], "/tmp/AB_DIGITAL_LOOPBACK_MODE.txt");				
			break;
			
		default:
			break;
		}
	}
	return status;
}

/*--------------------------------------------------------------------------------------*/
/*!   \ingroup internal_audio_services
\fn uint32_t StartPCMCapture(e_HatsAudioDevice_t Device,e_HatsAudioMode_t Mode,char * FilePath)
\brief start a PCM capture
\param [in] Device Id \ref e_HatsAudioDevice_t
\param [in] Mode refer to \ref e_HatsAudioMode_t
\param [in] FilePath string : file location
\return   
	HATS_AUDIO_NO_ERROR if no error
*/
/*--------------------------------------------------------------------------------------*/
uint32_t StartPCMCapture(e_HatsAudioDevice_t Device,e_HatsAudioMode_t Mode,char * FilePath)
{
	char command[MAX_LGTH_COMMAND];
	int32_t status = HATS_AUDIO_NO_ERROR;
	uint32_t PID;
	uint32_t i;
	hats_alsa_Device_t AlsaDevice;
	MC_TRACE_DBG(printf("AUDIOSERVICES StartPCMCapture: file=%s line=%d \n", __FILE__, __LINE__));

	/* check resource by verifying that no PID associated to command is already running */                                                 
	memset(command,0,sizeof(command));                                                
	GetAlsaDevice(Device,CODEC_CAPTURE,&AlsaDevice);
	snprintf(command,MAX_LGTH_COMMAND,"%s -D%s",PCM_RECORDER, AlsaDevice.name);         
	GetPID(command,&PID);               

	if (PID != 0)
	{
		/*Error: ressource already used*/
		printf( "AUDIOSERVICES ERROR: device %d already in used\n",AlsaDevice.device);
		return HATS_AUDIO_DEVICE_ALREADY_USED;
	}
	
	/* Check mode */
	for(i= 0; i <HATS_AUDIO_NB_CP_MODE;i++)
	{
		if(Mode == hats_audio_CP_mode[i].ID)
		{
			if(hats_audio_CP_mode[i].Status != HATS_AUDIO_NO_ERROR)
			{
				printf("AUDIOSERVICES ERROR: %s not implemented at this time\n", hats_audio_CP_mode[i].name);
				status = HATS_AUDIO_NOT_AVAILABLE;
			}
			hats_audio_contex[Device].CaptureMode = Mode;
			snprintf(hats_audio_contex[Device].RecordFile, MAX_LGTH_COMMAND, FilePath);

			break;
		}
	}

	if(i == HATS_AUDIO_NB_CP_MODE)
	{
		printf("AUDIOSERVICES ERROR: invalid capture mode %d\n", Mode);
		status = HATS_AUDIO_INVALID_PARAMETER;
	}

	if( status == HATS_AUDIO_NO_ERROR)
	{	
		switch(Mode)
		{
		case NORMAL_CAPTURE_MODE:
		case FM_CAPTURE_MODE:
			snprintf(command,MAX_LGTH_COMMAND,"%s -D%s %s",PCM_RECORDER, AlsaDevice.name, FilePath);         
			status=StartCommand(command);
			break;
		case ANALOG_LOOPBACK_MODE:
			status= ExecuteScript(HATS_AB_ANALOG_LOOP_START_FILE, "/tmp/ANALOG_LOOPBACK_MODE.txt");			
			break;
			
		default:
			break;
		}
	}
	return status;
}



/*--------------------------------------------------------------------------------------*/
/*! \ingroup audio_services
\fn uint32_t SetSink(e_HatsAudioDevice_t Device,e_HatsAudioOutput_t Output)
\brief Service allowing to choose the output of the loop or of the playback PCM transfer
*/
/*--------------------------------------------------------------------------------------*/
uint32_t SetSink(e_HatsAudioDevice_t Device,e_HatsAudioOutput_t Output)
{
	uint32_t status = HATS_AUDIO_NO_ERROR;
	hats_alsa_Device_t AlsaDevice;
	uint32_t i;
	MC_TRACE_DBG(printf("AUDIOSERVICES SetSink: file=%s line=%d \n", __FILE__, __LINE__));

	/* Check action */
	if(  (Device != AUDIO_DEVICE_0)
			||(Output >= CODEC_NB_OF_DEST))
	{
		printf("AUDIOSERVICES ERROR: one parameter is out of range : DEVICE = %d  Output = %d\n",Device,Output );        
		status =  HATS_AUDIO_INVALID_PARAMETER;   
	}
	else
	{	
		Out_Organ = Output;
		GetAlsaDevice(Device,CODEC_PLAYBACK,&AlsaDevice);
			
		/* Change the sink organ */
		status = ExecuteScript(hats_audio_output[Output].script, "/tmp/Sink.txt");
			
		/* force FM mode in case of FM_TX sink */
		for(i= 0; i <HATS_AUDIO_NB_PB_MODE;i++)
		{
			if(DIGITAL_LOOPBACK_MODE == hats_audio_PB_mode[i].ID)
			{
				if(Output == CODEC_DEST_FM_TX)
				hats_audio_PB_mode[i].alsa_mode[HATS_ALSA_PB_FM_TX_MODE] = 1;
				else
				hats_audio_PB_mode[i].alsa_mode[HATS_ALSA_PB_FM_TX_MODE] = 0;
				
				break;
			}
		}
		
	}
	return status;
}




/*--------------------------------------------------------------------------------------*/
/*! \ingroup audio_services
\fn uint32_t GetSink(e_HatsAudioDevice_t Device,e_HatsAudioOutput_t* p_Output)
\brief Service allowing to retrieve current selected output
*/
/*--------------------------------------------------------------------------------------*/
uint32_t GetSink(e_HatsAudioDevice_t Device,e_HatsAudioOutput_t* p_Output)
{
	uint32_t status = HATS_AUDIO_NOT_AVAILABLE;
	printf("AUDIOSERVICES: %d %d", Device,*p_Output);        
	printf("AUDIOSERVICES: GetSink: Warning not supported function!\n");        

	return status;
}


/*--------------------------------------------------------------------------------------*/
/*! \ingroup audio_services
\fn uint32_t SetSrc(e_HatsAudioDevice_t Device,e_HatsAudioInput_t Input)
\brief Service allowing to choose the input of the loop or of the capture PCM transfer
*/
/*--------------------------------------------------------------------------------------*/
uint32_t SetSrc(e_HatsAudioDevice_t Device,e_HatsAudioInput_t Input)
{
	uint32_t status = HATS_AUDIO_NO_ERROR;
	hats_alsa_Device_t AlsaDevice;
	uint32_t i;
	uint32_t index;
	MC_TRACE_DBG(printf("AUDIOSERVICES SetSrc: file=%s line=%d \n", __FILE__, __LINE__));
	
	/* Check action */
	if((Device != AUDIO_DEVICE_0) ||(Input > CODEC_NB_OF_SRC))
	{
		printf("AUDIOSERVICES ERROR: one parameter is out of range : DEVICE = %d  Input = %d\n",Device,Input );        
		status = HATS_AUDIO_INVALID_PARAMETER;   
	}
	else
	{ 			
		In_Organ = Input;
		GetAlsaDevice(Device,CODEC_CAPTURE,&AlsaDevice);
				
		/* Change the source organ */
		status =ExecuteScript(hats_audio_input[Input].script, "/tmp/Src.txt");

		/* force FM mode in case of FM_TX sink */
		for(i= 0; i <HATS_AUDIO_NB_PB_MODE;i++)
		{
			if(DIGITAL_LOOPBACK_MODE == hats_audio_PB_mode[i].ID)
			{if(Input == CODEC_SRC_FM_RX) {
					for(index=0; index< HATS_AUDIO_NB_CP_MODE; index++)
					if(hats_audio_CP_mode[index].ID == FM_CAPTURE_MODE)
					break;
					hats_audio_CP_mode[index].alsa_mode[HATS_ALSA_CP_FM_MODE] = 1;

				} else {
					for(index=0; index< HATS_AUDIO_NB_CP_MODE; index++)
					if(hats_audio_CP_mode[index].ID == FM_CAPTURE_MODE)
					break;
					hats_audio_CP_mode[index].alsa_mode[HATS_ALSA_CP_FM_MODE] = 0;
				}
				break;
			}
		}
		
	}
	return status;
}

/*--------------------------------------------------------------------------------------*/
/*! \ingroup audio_services
\fn uint32_t GetSrc(e_HatsAudioDevice_t Device, e_HatsAudioInput_t* p_input)
\brief Service allowing to retrieve current selected input
*/
/*--------------------------------------------------------------------------------------*/
uint32_t GetSrc(e_HatsAudioDevice_t Device, e_HatsAudioInput_t* p_input)
{
	uint32_t status = HATS_AUDIO_NOT_AVAILABLE;

	printf("AUDIOSERVICES:%d %d", Device,*p_input);        
	printf("AUDIOSERVICES:GetSrc: Warning not supported function!\n");        

	return status;
}

/*--------------------------------------------------------------------------------------*/
/*! \ingroup audio_services
\fn uint32_t SetVolume(e_HatsAudioDevice_t Device,e_HatsAudioCodecType_t Type, uint32_t LeftVolume, uint32_t RightVolume);
\brief Service allowing to set the volume level of the specified PCM transfer
*/
/*--------------------------------------------------------------------------------------*/
uint32_t SetVolume(e_HatsAudioDevice_t Device,e_HatsAudioCodecType_t Type, uint32_t LeftVolume, uint32_t RightVolume)
{
	uint32_t status = HATS_AUDIO_NO_ERROR;
	MC_TRACE_DBG(printf("AUDIOSERVICES SetVolume: file=%s line=%d \n", __FILE__, __LINE__));

	if ((LeftVolume==100)||(RightVolume==100))
	{
		if(  (Device >= NB_OF_AUDIO_DEVICE)||(Type >= NB_OF_CODEC_TYPE)	)
 		{
 			printf("AUDIOSERVICES:%d %d %d %d", Device, Type, LeftVolume, RightVolume);        
			printf("AUDIOSERVICES:SetVolume: invalid parameter!\n");  
   			status =  HATS_AUDIO_INVALID_PARAMETER;   
		}
		else if(Type == CODEC_CAPTURE)
    		{
			status= ExecuteScript(HATS_AB_IN_VOLMAX_FILE, "/tmp/Vol.txt");			
		}
		else if(Type == CODEC_PLAYBACK)
    		{
			status= ExecuteScript(HATS_AB_OUT_VOLMAX_FILE, "/tmp/Vol.txt");			
		}
	
  	}
  	else
 	{ 
		printf("AUDIOSERVICES:%d %d %d %d", Device, Type, LeftVolume, RightVolume);        
		printf("AUDIOSERVICES:SetVolume: Warning not supported!\n");  
		status = HATS_AUDIO_NOT_AVAILABLE;
	}

	return status;
}

/*--------------------------------------------------------------------------------------*/
/*! \ingroup audio_services
\fn uint32_t GetVolume(e_HatsAudioDevice_t Device,e_HatsAudioCodecType_t Type, uint32_t * p_LeftVolume, uint32_t * p_RightVolume);
\brief Service allowing to retrieve the volume level of the specified PCM transfer
*/
/*--------------------------------------------------------------------------------------*/
uint32_t GetVolume(e_HatsAudioDevice_t Device,e_HatsAudioCodecType_t Type, uint32_t * p_LeftVolume, uint32_t * p_RightVolume)
{
	uint32_t status = HATS_AUDIO_NOT_AVAILABLE;

	printf("AUDIOSERVICES:%d %d %d %d", Device, Type, *p_LeftVolume, *p_RightVolume);        
	printf("AUDIOSERVICES: GetVolume: Warning not supported function!\n");        

	return status;
}


/*--------------------------------------------------------------------------------------*/
/*! \ingroup audio_services
\fn uint32_t SetMute(e_HatsAudioDevice_t Device,e_HatsAudioCodecType_t Type, e_HatsAudioMute_t State);
\brief Service allowing to mute/unmute the specified PCM transfer
*/
/*--------------------------------------------------------------------------------------*/
uint32_t SetMute(e_HatsAudioDevice_t Device,e_HatsAudioCodecType_t Type, e_HatsAudioMute_t State)
{
	uint32_t status = HATS_AUDIO_NOT_AVAILABLE;

	printf("AUDIOSERVICES:%d %d %d", Device, Type, State);        
	printf("AUDIOSERVICES:SetMute : Warning not supported function!\n");        

	return status;
}

/*--------------------------------------------------------------------------------------*/
/*! \ingroup audio_services
\fn uint32_t GetMute(e_HatsAudioDevice_t Device,e_HatsAudioCodecType_t Type,e_HatsAudioMute_t* p_State);
\brief Service allowing to retrieve the mute state of the specified PCM transfer
*/
/*--------------------------------------------------------------------------------------*/
uint32_t GetMute(e_HatsAudioDevice_t Device,e_HatsAudioCodecType_t Type,e_HatsAudioMute_t*p_State)
{
	uint32_t status = HATS_AUDIO_NOT_AVAILABLE;

	printf("AUDIOSERVICES:%d %d %d", Device, Type, *p_State);        
	printf("AUDIOSERVICES:GetMute : Warning not supported function!\n");        

	return status;
}

/*--------------------------------------------------------------------------------------*/
/*!   \ingroup internal_audio_services
\fn uint32_t GetPID(uint8_t* Command, uint32_t* PID)
\brief get the process ID of the specified command
\param [in] Command string containing the system command 
\param [out] PID process ID associated to the command
\return   
	0 if no error
*/
/*--------------------------------------------------------------------------------------*/
uint32_t GetPID(const char* Command, uint32_t* PID)
{
	char *unix_command;
	char commandGetPID[MAX_LGTH_COMMAND];
	char commandtmp[MAX_LGTH_COMMAND];
	FILE *file_ptr;
	MC_TRACE_DBG(printf("AUDIOSERVICES GetPID: file=%s line=%d \n", __FILE__, __LINE__));

	*PID = 0;
	/* clear the status error file */
	system("rm -f /tmp/PID.txt");

	/* manage pipes*/
	snprintf(commandtmp,MAX_LGTH_COMMAND,"%s",Command);
	unix_command = strtok( commandtmp, "|" );
	if (unix_command!=NULL)
	{
		snprintf(commandGetPID,MAX_LGTH_COMMAND,"ps w |grep -E \"%s\" | grep -v grep | grep -v sh| busybox awk -F\' \' \'{print $1}\'>/tmp/PID.txt",unix_command);
	}
	else
	{
		snprintf(commandGetPID,MAX_LGTH_COMMAND,"ps w |grep -E \"%s\" | grep -v grep | grep -v sh| busybox awk -F\' \' \'{print $1}\'>/tmp/PID.txt",Command);
	}
	
	system(commandGetPID);
	printf("AUDIOSERVICES: %s\n",commandGetPID);
	file_ptr = fopen("/tmp/PID.txt","r");
	if(file_ptr != NULL)
	{
		fscanf(file_ptr,"%d\n",PID);
	}
	/* clear the temp file */
	system("rm -f /tmp/PID.txt");
	return 0;
}

/*--------------------------------------------------------------------------------------*/
/*!   \ingroup internal_audio_services
\fn uint32_t KillPID(uint8_t* Command)
\brief kill the process associated to the command
\param [in] Command string containing the system command 
\return   
	0 if no error
*/
/*--------------------------------------------------------------------------------------*/
uint32_t KillPID(const char* Command)
{
	char unix_command[MAX_LGTH_COMMAND];
	MC_TRACE_DBG(printf("AUDIOSERVICES KillPID: file=%s line=%d \n", __FILE__, __LINE__));

	snprintf(unix_command,MAX_LGTH_COMMAND,"ps w |grep -E \"%s\" | grep -v grep | grep -v sh| busybox awk -F\' \' \'{print $1}\'|xargs kill -2",Command);
	system(unix_command);
	return 0;
}

/*--------------------------------------------------------------------------------------*/
/*!   \ingroup internal_audio_services
\fn uint32_t checkstatus(uint8_t* FileName)
\brief check status of a command
\param [in] FileName file name containing status 
\return   
	HATS_AUDIO_NO_ERROR if status is ok else HATS_AUDIO_FAILED
*/
/*--------------------------------------------------------------------------------------*/
uint32_t checkstatus(const char* FileName)
{
	FILE *file_ptr;
	uint8_t buffer[128];
	int32_t nb_attempt = 5;
	MC_TRACE_DBG(printf("AUDIOSERVICES checkstatus: file=%s line=%d \n", __FILE__, __LINE__));
	while(((file_ptr = fopen(FileName,"r")) == NULL)&&(nb_attempt-- !=0));
	
	if(file_ptr ==NULL)
	{
		/* no file means no error */
		//printf("\nFILE %s not found : %x\n",FileName, file_ptr); 
		return HATS_AUDIO_NO_ERROR;
	}
	else
	{
		while(fscanf(file_ptr,"%s\n",buffer) != EOF)
		{
			printf("\nFILE: %s\n", buffer); 
		}
		fclose(file_ptr);  
		return HATS_AUDIO_FAILED;
	}
}


/*--------------------------------------------------------------------------------------*/
/*!   \ingroup internal_audio_services
\fn uint32_t SetMixerCtrl(e_HatsAudioDevice_t Device, hats_audio_mixer_Ctrl_t Ctrl,  uint8_t* p_param)
\brief set mixer control and return associated status
\param [in] Device Id \ref e_HatsAudioDevice_t
\param [in] Ctrl mixer control refer to \ref hats_audio_mixer_Ctrl_t
\param [in] p_param parameters associated to ctrl
\return   
	HATS_AUDIO_NO_ERROR if no error
*/
/*--------------------------------------------------------------------------------------*/
uint32_t SetMixerCtrl(hats_alsa_Device_t* pDevice, hats_audio_mixer_Ctrl_t Ctrl,  const char* p_param)
{
	uint32_t status = HATS_AUDIO_NOT_AVAILABLE;

	printf("AUDIOSERVICES:%d %d %d", (int)pDevice, (int)Ctrl, *p_param);        
	printf("AUDIOSERVICES:SetMixerCtrl : Warning not supported function!\n");        

	return status;
}

/*--------------------------------------------------------------------------------------*/
/*!   \ingroup internal_audio_services
\fn uint32_t GetMixerCtrl(e_HatsAudioDevice_t Device, hats_audio_mixer_Ctrl_t Ctrl,  uint32_t* p_param)
\brief get mixer control state
\brief start a PCM capture
\param [in] Device Id \ref e_HatsAudioDevice_t
\param [in] Ctrl mixer control refer to \ref hats_audio_mixer_Ctrl_t
\param [out] p_param parameters associated to ctrl
\return   
	HATS_AUDIO_NO_ERROR if no error
*/
/*--------------------------------------------------------------------------------------*/
uint32_t GetMixerCtrl(hats_alsa_Device_t* pDevice, hats_audio_mixer_Ctrl_t Ctrl,  uint32_t* p_param)
{
	uint32_t status = HATS_AUDIO_NO_ERROR;

	printf("AUDIOSERVICES:%d %d %d", (int)pDevice, (int)Ctrl, *p_param);        
	printf("AUDIOSERVICES:GetMixerCtrl : Warning not supported function!\n");        

	return status;
}
/*--------------------------------------------------------------------------------------*/
/*!   \ingroup internal_audio_services
\fn void GetAlsaDevice(e_HatsAudioDevice_t device, e_HatsAudioMode_t Mode, hats_alsa_Device_t* pDevice, char* pDeviceName)
\brief Get Alsa device and subdevice
\param [in] Device Id \ref e_HatsAudioDevice_t
\param [in] Mode  playback or capture
\param [out] pAlsaDevice Alsa device
\return   
	HATS_AUDIO_NO_ERROR if no error
*/
/*--------------------------------------------------------------------------------------*/
void GetAlsaDevice(e_HatsAudioDevice_t Device, e_HatsAudioCodecType_t Type, hats_alsa_Device_t* pAlsaDevice)
{	
	/* default value */
	pAlsaDevice->device = HATS_AUDIO_NO_DEVICE;
	pAlsaDevice->subdevice = HATS_AUDIO_NO_DEVICE;
	snprintf( pAlsaDevice->name, MAX_LGTH_COMMAND, "default");
	MC_TRACE_DBG(printf("AUDIOSERVICES GetAlsaDevice: file=%s line=%d \n", __FILE__, __LINE__));
	
	if( Device == AUDIO_DEVICE_0)  /* playback or capture on standard organs */
	{
		if( Type == CODEC_PLAYBACK)
		{
			pAlsaDevice->device = 0;
			pAlsaDevice->subdevice = 1;
			snprintf( pAlsaDevice->name, MAX_LGTH_COMMAND, "hw:0,1");
		}
		else if( Type == CODEC_CAPTURE)
		{
			pAlsaDevice->device = 0;
			pAlsaDevice->subdevice = 2;
			snprintf( pAlsaDevice->name, MAX_LGTH_COMMAND, "hw:0,2");
		}
	}
	if( Device == AUDIO_DEVICE_BT) /* playback or capture on Bluetooth */
	{
		pAlsaDevice->device = 0;
		pAlsaDevice->subdevice = 3;
		snprintf( pAlsaDevice->name, MAX_LGTH_COMMAND, "hw:0,3");
	}
	if( Device == AUDIO_DEVICE_HDMI) /* playback on HDMI */
	{
		pAlsaDevice->device = 0;
		pAlsaDevice->subdevice = 0;
		snprintf( pAlsaDevice->name, 100, "hw:0,0");
	}
}

/*--------------------------------------------------------------------------------------*/
/*! \ingroup audio_services
	\fn uint32_t SetMultichannelMode(e_HatsAudioChannelMode_t Mode);
	\brief Service allowing to Set Multichannel Mode
	\param [in] Mode Alsa behaviour mode \ref e_HatsAudioChannelMode_t
	\return 	
		0 if no error
*/
/*--------------------------------------------------------------------------------------*/
uint32_t SetMultichannelMode(e_HatsAudioDevice_t Device,e_HatsAudioChannelMode_t Mode)
{
	uint32_t status = HATS_AUDIO_NOT_AVAILABLE;

	printf("AUDIOSERVICES:%d %d", Device, Mode);        
	printf("AUDIOSERVICES:SetMultichannelMode : Warning not supported function!\n");        

	return status;
}

/*--------------------------------------------------------------------------------------*/
/*! \ingroup audio_services
	\fn uint32_t StartSpeechCall(e_HatsAudioDevice_t Device);
	\brief Service allowing to start a 2G or 3G speech call ( audio part only)
	\param [in] Device device Id \ref e_HatsAudioDevice_t
	\return 	
		0 if no error
*/
/*--------------------------------------------------------------------------------------*/
uint32_t StartSpeechCall(e_HatsAudioDevice_t Device)
{
	uint32_t status = HATS_AUDIO_NO_ERROR;
	char command[MAX_LGTH_COMMAND];
	MC_TRACE_DBG(printf("AUDIOSERVICES StartSpeechCall: file=%s line=%d \n", __FILE__, __LINE__));

	if((Device != AUDIO_DEVICE_0))
	{
		printf("AUDIOSERVICES ERROR: one parameter is out of range : DEVICE = %d\n",Device);        
		status = HATS_AUDIO_INVALID_PARAMETER;   
	}
	else
	{ 
		snprintf(command,MAX_LGTH_COMMAND,"MMTE -f %s/StartCSCall.ate",HATS_AUDIO_MMTE_SCRIPT_PATH);    //printf("\n%s\n",command); /* only for debug*/                                              
		system(command);
	}                                             
	return status;
}
/*--------------------------------------------------------------------------------------*/
/*! \ingroup audio_services
	\fn uint32_t StopSpeechCall(e_HatsAudioDevice_t Device);
	\brief Service allowing to stop a 2G or 3G speech call ( audio part only)
	\param [in] Device device Id \ref e_HatsAudioDevice_t
	\return 	
		0 if no error
*/
/*--------------------------------------------------------------------------------------*/
uint32_t StopSpeechCall(e_HatsAudioDevice_t Device)
{
	uint32_t status = HATS_AUDIO_NO_ERROR;
	char command[MAX_LGTH_COMMAND];
	MC_TRACE_DBG(printf("AUDIOSERVICES StopSpeechCall: file=%s line=%d \n", __FILE__, __LINE__));

	if((Device != AUDIO_DEVICE_0))
	{
		printf("AUDIOSERVICES ERROR: one parameter is out of range : DEVICE = %d\n",Device);        
		status = HATS_AUDIO_INVALID_PARAMETER;   
	}
	else
	{ 
		snprintf(command,MAX_LGTH_COMMAND,"MMTE -f %s/StopCSCall.ate",HATS_AUDIO_MMTE_SCRIPT_PATH);
		system(command);                                             
	}
	return status;
}
/*--------------------------------------------------------------------------------------*/
/*! \ingroup audio_services
	\fn uint32_t GetCsCallCapability(HatsAudioScCallCodec_t* pList);
	\brief Service allowing to retreive the list of supported audio codec
	\param [in] pList pointer to the list of audio codec ref audiocodec_t
	\return 	
		0 if no error
*/
/*--------------------------------------------------------------------------------------*/
uint32_t GetCsCallCapability(HatsAudioScCallCodec_t* pList)
{

	MC_TRACE_DBG(printf("AUDIOSERVICES GetCsCallCapability: file=%s line=%d \n", __FILE__, __LINE__));
	memcpy(pList,&ScCallCapabilities,sizeof(HatsAudioScCallCodec_t));
	return HATS_AUDIO_NO_ERROR;
}
/*--------------------------------------------------------------------------------------*/
/*! \ingroup audio_services
	\fn int32_t PowerHDMIaudioInput(hats_hdmi_audio_state state);
	\brief Service allowing to enable and disable the audio input for HDMI
	\param [in] state power state ref hats_hdmi_audio_state
	\return 	
		0 if no error
*/
/*--------------------------------------------------------------------------------------*/
int32_t PowerHDMIaudioInput(hats_hdmi_audio_state state)
{
	FILE * fdesc=0;
	uint32_t vl_error=HATS_AUDIO_NO_ERROR;
	char If_mode;
	MC_TRACE_DBG(printf("AUDIOSERVICES PowerHDMIaudioInput: file=%s line=%d \n", __FILE__, __LINE__));
	
	/* 1- open hdmi system file   */
	fdesc = fopen (HATS_AUDIO_HDMI_AUDIOCFG_SYSFS,"w");
	if(( fdesc == NULL) )
	{ 
		printf("AUDIOSERVICES ERROR: error when try to open file: %s\n", HATS_AUDIO_HDMI_AUDIOCFG_SYSFS);
		vl_error=HATS_AUDIO_FAILED;
	}  

	else
	{
		/* 2- Send the command */
		if(state == HATS_HDMI_AUDIO_OFF)
		{
			If_mode = AV8100_AUDIO_SLAVE;
		}
		else
		{
			If_mode = AV8100_AUDIO_MASTER;
		}
		if( fprintf(fdesc,"%02x%02x%02x%02x%02x%02x%02x\n"
					,AV8100_AUDIO_TDM_MODE      /* if format */
					,4                          /* Nb of I2S entrie */
					,AV8100_AUDIO_FREQ_48KHZ    /* Frequency */
					,AV8100_AUDIO_20BITS        /* Sample lenght*/
					,AV8100_AUDIO_LPCM_MODE     /* sample format */
					,If_mode         			/* if format */
					,AV8100_AUDIO_MUTE_DISABLE  /* mute */
					)<0)
		{
			/* error during Write execution */
			printf("AUDIOSERVICES ERROR: error when try to access file: %s\n", HATS_AUDIO_HDMI_AUDIOCFG_SYSFS);
			vl_error=HATS_AUDIO_FAILED;
		}
		fclose(fdesc);     
	}
	/* 3- close hdmi system file */
	return vl_error;	
}
