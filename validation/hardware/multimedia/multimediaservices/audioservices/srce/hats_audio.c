/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
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
#include <dirent.h>
#include <unistd.h>
#include <assert.h>
#include "hats_audio.h"
#include "hats_audio.hi"

e_HatsAudioInput_t In_Organ = CODEC_SRC_LINEIN;
e_HatsAudioOutput_t Out_Organ = CODEC_DEST_HEADSET;

uint32_t StartCommand(char* pcommand)
{
    int32_t status = HATS_AUDIO_NO_ERROR;
    char command[MAX_LGTH_COMMAND] = "";
    uint32_t PID;
    MC_TRACE_DBG(printf("AUDIOSERVICES StartCommand: file=%s line=%d \n", __FILE__, __LINE__));

    snprintf(command, MAX_LGTH_COMMAND, "%s &", pcommand);
    system(command);
    printf("AUDIOSERVICES CMD:%s\n", command);

    /* Check PID*/
    sleep(1);
    GetPID(pcommand, &PID);
    if (PID == 0) {
        /* no ID found means error during playback processing*/
        printf("\nAUDIOSERVICES ERROR: error during audio services command\n");
        status = HATS_AUDIO_FAILED;
    }

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
uint32_t ExecuteCommand(char* pcommand, hats_alsa_Device_t * pDevice)
{
	int32_t status = HATS_AUDIO_NO_ERROR;
	char command[200]="";
	char option[200]="-q";
	
	MC_TRACE_DBG(snprintf(option,200, "Trace_debug"));
	
	/* mixer command */
	if (strncmp(pcommand,"amixer",6)==0)
	{
		 /* delete status file */
		system("rm -f /tmp/mixerstatus.txt");

		if(pDevice->device !=HATS_AUDIO_NO_DEVICE)
			{
				if(pDevice->subdevice !=HATS_AUDIO_NO_DEVICE)
				{	
					snprintf(command,200, "%s ,device=%d,subdevice=%d %s || echo \"ERROR\" >/tmp/mixerstatus.txt", pcommand, pDevice->device,pDevice->subdevice,option);
				}
				else
				{
					snprintf(command,200, "%s ,device=%d %s || echo \"ERROR\" >/tmp/mixerstatus.txt", pcommand, pDevice->device,option);
				}
		}
		else
		{
				snprintf(command,200, "%s  %s || echo \"ERROR\" >/tmp/mixerstatus.txt", pcommand,option);
		}
	}
	else
	{
		if(pDevice->device !=HATS_AUDIO_NO_DEVICE)
		{
			if(pDevice->subdevice !=HATS_AUDIO_NO_DEVICE)
			{	
				snprintf(command,200, "%s ,device=%d,subdevice=%d", pcommand, pDevice->device,pDevice->subdevice);
			}
			else
			{
				snprintf(command,200, "%s ,device=%d", pcommand, pDevice->device);
			}
		}
		else
		{
				snprintf(command,200, "%s", pcommand);
		}
		printf("\n%s\n",command);

	}
			
	system(command);

	/* check command status */
	if (strncmp(pcommand,"amixer",6)==0)
	{
		status = checkstatus("/tmp/mixerstatus.txt");
	}
	
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
   
  /* Check action */
  if(  (Device >= NB_OF_AUDIO_DEVICE)
     ||(Type > CODEC_CAPTURE)
     ||(Mode >= NB_OF_DATA_MODE))
  {
     printf(" one parameter is out of range : DEVICE = %d  TYPE = %d  MODE = %d\n",Device,Type,Mode );        
     return HATS_AUDIO_INVALID_PARAMETER;   
  }   
  /* clear the status error file */
MC_TRACE_DBG(printf("%s %s %d Device=%d Type=%d Mode=%d\n", __FILE__,__func__,__LINE__,Device,Type,Mode));
  system("rm -f /tmp/playbackstatus.txt");
  
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
  char command[100];
  hats_alsa_Device_t AlsaDevice;
  /* check action */
  if(  (Device >= NB_OF_AUDIO_DEVICE)
     ||(Type > CODEC_CAPTURE))
  {
     printf("\none parameter is out of range : DEVICE = %d  TYPE = %d\n",Device,Type );        
     return HATS_AUDIO_INVALID_PARAMETER;   
  }
  
  /* prepare command */                                                 
  memset(command,0,sizeof(command));                                                
  if((Type == CODEC_PLAYBACK) && (hats_audio_contex[Device].PlaybackMode != HATS_AUDIO_UNDEFINED_MODE))
  {
    if(hats_audio_contex[Device].PlaybackMode == DIGITAL_LOOPBACK_MODE)
    {
     GetAlsaDevice(Device,CODEC_PLAYBACK,&AlsaDevice);
    	/* disable power */
      AlsaDevice.subdevice = 1;
    	SetMixerCtrl(&AlsaDevice,HATS_AUDIO_CAPTURE_POWER, "\'DISABLE\'");
      AlsaDevice.subdevice = 0;
    	SetMixerCtrl(&AlsaDevice,HATS_AUDIO_PLAYBACK_POWER, "\'DISABLE\'");
      /* deactive digital loop */
      AlsaDevice.subdevice = HATS_AUDIO_NO_DEVICE;
      SetMixerCtrl(&AlsaDevice,HATS_AUDIO_DIGITAL_LOOPBACK,  "\'DISABLE\'");
	  SetMixerCtrl(&AlsaDevice,HATS_AUDIO_PCM_CAPTURE_MODE,  "\'DISABLE\'");
	  SetMixerCtrl(&AlsaDevice,HATS_AUDIO_PCM_RENDERING_MODE,  " \'DISABLE\' \'DISABLE\' \'DISABLE\'");
    }
#ifdef CR_CAP_73_001_STW4500_AUDIOLOOP
    else if(hats_audio_contex[Device].PlaybackMode == AB_DIGITAL_LOOPBACK_MODE)
    {
      stw400audioloop(HATS_STW4500_LOOP_OFF);
	  
    }
#endif /* CR_CAP_73_001_STW4500_AUDIOLOOP */
    else
    {
    	if(hats_audio_contex[Device].PlaybackMode !=FM_PLAYBACK_MODE)
		{
				/* kill the aplay process to stop the playback */
      		GetAlsaDevice(Device,CODEC_PLAYBACK,&AlsaDevice);
        	snprintf(command,200,"%s -Dhw:%d,%d",PCM_PLAYER,AlsaDevice.device,AlsaDevice.subdevice);         
      		KillPID(command);
		}
      
      /* deactivate power */
    	if(Device ==AUDIO_DEVICE_0)
  		{
    		/* disable power */
    		SetMixerCtrl(&AlsaDevice,HATS_AUDIO_PLAYBACK_POWER, "\'DISABLE\'");
    		/* disable rendering modes */
  	    	memset(command,0,sizeof(command));                                            
  			snprintf(command,sizeof(command)," \'DISABLE\' \'DISABLE\' \'DISABLE\'");          	 
			SetMixerCtrl(&AlsaDevice,HATS_AUDIO_PCM_RENDERING_MODE,  command);
  		}
  		else if(Device ==AUDIO_DEVICE_HDMI)
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
      GetAlsaDevice(Device,CODEC_PLAYBACK,&AlsaDevice);
    	/* disable power */
      AlsaDevice.subdevice = 1;
    	SetMixerCtrl(&AlsaDevice,HATS_AUDIO_CAPTURE_POWER, "\'DISABLE\'");
      AlsaDevice.subdevice = 0;
    	SetMixerCtrl(&AlsaDevice,HATS_AUDIO_PLAYBACK_POWER, "\'DISABLE\'");
      /* deactive analog loop */
      AlsaDevice.subdevice = HATS_AUDIO_NO_DEVICE;
      SetMixerCtrl(&AlsaDevice,HATS_AUDIO_ANALOG_LOOPBACK, "\'DISABLE\'");
    }
    else
    {
      GetAlsaDevice(Device,CODEC_PLAYBACK,&AlsaDevice);
      //snprintf(command,50,"arecord -Dhw:%d,%d",AlsaDevice.device,AlsaDevice.subdevice);         
      snprintf(command,50,"arecord -r 48000 -f S16");         
      KillPID(command);
      /* deactivate power */
    	if(Device ==AUDIO_DEVICE_0)
  		{
    		/* disable power */
#if 0  /* code deactivate until correctio of CAP_714_001 */
    		SetMixerCtrl(&AlsaDevice,HATS_AUDIO_CAPTURE_POWER, "\'DISABLE\'");
				/* reset capture mode*/
  	    memset(command,0,sizeof(command));                                            
  			snprintf(command,200," \'DISABLE\' \'DISABLE\' \'DISABLE\'");          	 
				SetMixerCtrl(&AlsaDevice,HATS_AUDIO_PCM_CAPTURE_MODE, command);	 
#endif  /* code deactivate until correctio of CAP_714_001 */
  		}
    }
    hats_audio_contex[Device].CaptureMode= HATS_AUDIO_UNDEFINED_MODE;     
  }
  else
  {
    printf("ERROR: invalid command codec type = %d, current mode = %d\n",Type,hats_audio_contex[Device].PlaybackMode);
    return HATS_AUDIO_INVALID_PARAMETER;
  }
  return HATS_AUDIO_NO_ERROR;
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

	MC_TRACE_DBG(printf("%s line=%d: Device=%d Output=%d \n", __func__, __LINE__ , Device, Output));
  /* Check action */
  if(  (Device != AUDIO_DEVICE_0)
     ||(Output >= CODEC_NB_OF_DEST))
  {
     printf("\none parameter is out of range : DEVICE = %d  Output = %d\n",Device,Output );        
     status =  HATS_AUDIO_INVALID_PARAMETER;
  }
  else
  {
		Out_Organ = Output;
		GetAlsaDevice(Device,CODEC_PLAYBACK,&AlsaDevice);

		/* Change the sink organ */
		status = ExecuteScript(hats_audio8520_output[Output].script, "/tmp/Sink.txt");

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
  uint32_t status = HATS_AUDIO_NO_ERROR;
  hats_alsa_Device_t AlsaDevice;
  /* check action */
  if(Device != AUDIO_DEVICE_0)
  {
     printf("\none device is out of range : DEVICE = %d\n",Device);        
     status = HATS_AUDIO_INVALID_PARAMETER;   
  }
  else
  {
    /* Change the sink organ */
       GetAlsaDevice(Device,CODEC_PLAYBACK,&AlsaDevice);
    status = GetMixerCtrl(&AlsaDevice,HATS_AUDIO_PCM_SINK,  p_Output);
  }
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

	MC_TRACE_DBG(printf("%s line=%d: Device=%d Input=%d \n", __func__, __LINE__ , Device, Input));
  /* Check action */
  if((Device != AUDIO_DEVICE_0) ||(Input > CODEC_NB_OF_SRC))
  {
     printf("\none parameter is out of range : DEVICE = %d  Input = %d\n",Device,Input );        
     status = HATS_AUDIO_INVALID_PARAMETER;
  }
  else
  { 
		In_Organ = Input;
		GetAlsaDevice(Device,CODEC_CAPTURE,&AlsaDevice);

		/* Change the source organ */
		status =ExecuteScript(hats_audio8520_input[Input].script, "/tmp/Src.txt");

		/* force FM mode in case of FM_TX sink */
		for(i= 0; i <HATS_AUDIO_NB_PB_MODE;i++)
		{
			if(DIGITAL_LOOPBACK_MODE == hats_audio_PB_mode[i].ID)
			{
				    if(Input == CODEC_SRC_FM_RX)
					{
						for(index=0; index< HATS_AUDIO_NB_CP_MODE; index++)
						if(hats_audio_CP_mode[index].ID == FM_CAPTURE_MODE)
							break;
						hats_audio_CP_mode[index].alsa_mode[HATS_ALSA_CP_FM_MODE] = 1;
					}
					else
					{
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
  uint32_t status = HATS_AUDIO_NO_ERROR;
  hats_alsa_Device_t AlsaDevice;
  /* check action */
  if(Device != AUDIO_DEVICE_0) 
  {
     printf("\none parameter is out of range : DEVICE = %d\n",Device );
     status =  HATS_AUDIO_INVALID_PARAMETER;
  }
  else
  {
    /* Change the sink organ */
       GetAlsaDevice(Device,CODEC_CAPTURE,&AlsaDevice);
    status = GetMixerCtrl(&AlsaDevice,HATS_AUDIO_PCM_SRCE,  p_input);
  }
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
  char volume[50];
  hats_alsa_Device_t AlsaDevice;
  float LVol=0, RVol=0;

  /* check action */
  if(  (Device >= NB_OF_AUDIO_DEVICE)
     ||(Type >= NB_OF_CODEC_TYPE)
     ||(LeftVolume > 100)
     ||(RightVolume> 100))
  {
     printf("\none parameter is out of range : DEVICE = %d  TYPE = %d LeftVolume = %d  RightVolume = %d\n",Device,Type,LeftVolume,RightVolume);        
     status =  HATS_AUDIO_INVALID_PARAMETER;   
  }
  else
  { 
    /* patch kbo : VolMax=90 for Capture*/
	if(Type == CODEC_CAPTURE)
    {
		if (LeftVolume>90)
				LeftVolume=90;
		if (RightVolume>90)
				RightVolume=90;
	}
	
	/* 0..100 to 0..63*/
	 LVol=63.0*((float)LeftVolume/100);
	 RVol=63.0*((float)RightVolume/100);
	 snprintf(volume,50,"%d,%d",(uint32_t)LVol,(uint32_t)RVol);
	 
      /* Change the current volume*/
    if(Type == CODEC_CAPTURE)
    {
       GetAlsaDevice(Device,CODEC_CAPTURE,&AlsaDevice);
        status = SetMixerCtrl(&AlsaDevice,HATS_AUDIO_CAPTURE_VOL,  volume);
    }
    else if(Type == CODEC_PLAYBACK)
    {
       GetAlsaDevice(Device,CODEC_PLAYBACK,&AlsaDevice);
        status = SetMixerCtrl(&AlsaDevice,HATS_AUDIO_PLAYBACK_VOL,  volume);
    }
		else
    {
       GetAlsaDevice(Device,CODEC_MASTER,&AlsaDevice);
        status = SetMixerCtrl(&AlsaDevice,HATS_AUDIO_MASTER_VOL,  volume);
    }
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
  uint32_t status = HATS_AUDIO_NO_ERROR;
  uint32_t Volume[2];
  hats_alsa_Device_t AlsaDevice;
  if(  (Device >= NB_OF_AUDIO_DEVICE) ||(Type >= NB_OF_CODEC_TYPE))
  {
     printf("\none parameter is out of range : DEVICE = %d  TYPE = %d\n",Device,Type );        
     status =  HATS_AUDIO_INVALID_PARAMETER;   
  }
  else
  {
    /* get the current volumes */
    if(Type == CODEC_PLAYBACK)
    {
       GetAlsaDevice(Device,CODEC_PLAYBACK,&AlsaDevice);
       status = GetMixerCtrl(&AlsaDevice,HATS_AUDIO_PLAYBACK_VOL,  Volume);
    }
    else if(Type == CODEC_CAPTURE)
    {
       GetAlsaDevice(Device,CODEC_CAPTURE,&AlsaDevice);
       status = GetMixerCtrl(&AlsaDevice,HATS_AUDIO_CAPTURE_VOL,  Volume);
    }
		else
    {
       GetAlsaDevice(Device,CODEC_MASTER,&AlsaDevice);
        status = GetMixerCtrl(&AlsaDevice,HATS_AUDIO_MASTER_VOL,  Volume);
    }
    *p_LeftVolume = Volume[0];
    *p_RightVolume = Volume[1];
  }
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
  uint32_t status = HATS_AUDIO_NO_ERROR;
  char Mute_State[3];
  hats_alsa_Device_t AlsaDevice;
  
  /* check action */
  if(  (Device >= NB_OF_AUDIO_DEVICE)
     ||(Type >= NB_OF_CODEC_TYPE)
     ||(State > STREAM_UNMUTED))
  {
     printf(" one parameter is out of range : DEVICE = %d  TYPE = %d  State = %d\n",Device,Type,State );        
     status= HATS_AUDIO_INVALID_PARAMETER;   
  }   
  else
  { 
    /* Change the current mute state*/
    snprintf(Mute_State,3,"%d",State);
    if(Type == CODEC_PLAYBACK)
    {
       GetAlsaDevice(Device,CODEC_PLAYBACK,&AlsaDevice);
         status = SetMixerCtrl(&AlsaDevice,HATS_AUDIO_PLAYBACK_MUTE,  Mute_State);
    }
    else if(Type == CODEC_CAPTURE)
    {
        GetAlsaDevice(Device,CODEC_CAPTURE,&AlsaDevice);
        status = SetMixerCtrl(&AlsaDevice,HATS_AUDIO_CAPTURE_MUTE,  Mute_State);
    }
		else
    {
       GetAlsaDevice(Device,CODEC_MASTER,&AlsaDevice);
        status = SetMixerCtrl(&AlsaDevice,HATS_AUDIO_MASTER_MUTE,  Mute_State);
    }
  }
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
  uint32_t status = HATS_AUDIO_NO_ERROR;
  hats_alsa_Device_t AlsaDevice;
  
  if(  (Device >= NB_OF_AUDIO_DEVICE) ||(Type >= NB_OF_CODEC_TYPE))
  {
     printf("\none parameter is out of range : DEVICE = %d  TYPE = %d\n",Device,Type );        
     status =  HATS_AUDIO_INVALID_PARAMETER;   
  }
  else
  {
    /* get the current volumes */
    if(Type == CODEC_PLAYBACK)
    {
        GetAlsaDevice(Device,CODEC_PLAYBACK,&AlsaDevice);
        status = GetMixerCtrl(&AlsaDevice,HATS_AUDIO_PLAYBACK_MUTE, p_State);
    }
    else if(Type == CODEC_CAPTURE)
    {
        GetAlsaDevice(Device,CODEC_CAPTURE,&AlsaDevice);
        status = GetMixerCtrl(&AlsaDevice,HATS_AUDIO_CAPTURE_MUTE,  p_State);
    }
		else
    {
       GetAlsaDevice(Device,CODEC_MASTER,&AlsaDevice);
        status = GetMixerCtrl(&AlsaDevice,HATS_AUDIO_MASTER_MUTE,  p_State);
    }
  }
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
  char unix_command[200];
  FILE *file_ptr;

  *PID = 0;
  /* clear the status error file */
  system("rm -f /tmp/aplayPID.txt");

/*  snprintf(unix_command,200,"ps -f |grep -E \"%s\" | grep -v grep | grep -v sh| busybox awk -F\' \' \'{print $2}\'>/tmp/aplayPID.txt",Command);*/
  snprintf(unix_command,200,"ps |grep -E \"%s\" | grep -v grep | grep -v sh| busybox awk -F\' \' \'{print $1}\'>/tmp/aplayPID.txt",Command);
  system(unix_command);
  file_ptr = fopen("/tmp/aplayPID.txt","r");
  if(file_ptr != NULL)
  {
    fscanf(file_ptr,"%d\n",PID);
   // printf("\nPID : %d\n",*PID); 
  }
  /* clear the temp file */
  system("rm -f /tmp/aplayPID.txt");
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
  char unix_command[200];

  snprintf(unix_command,200,"ps |grep -E \"%s\" | grep -v grep | grep -v sh| busybox awk -F\' \' \'{print $1}\'|xargs kill -2",Command);
/*  snprintf(unix_command,200,"ps -f |grep -E \"%s\" | grep -v grep | grep -v sh| busybox awk -F\' \' \'{print $2}\'|xargs kill -2",Command);*/
/*  snprintf(unix_command,200,"pkill -2 -f \"%s\"",Command);*/
//  printf("\n%s\n", unix_command); /* only for debug */
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
  char command[200];
  int32_t status = HATS_AUDIO_NO_ERROR;
  uint32_t PID;
  hats_alsa_Device_t AlsaDevice;
  uint32_t i;

  /* check resource by verifying that no PID associated to command is already running */                                                 
MC_TRACE_DBG(printf("%s %s %d Device=%d Mode=%d\n", __FILE__,__func__,__LINE__,Device,Mode));
  memset(command,0,sizeof(command));
  GetAlsaDevice(Device,CODEC_PLAYBACK,&AlsaDevice);
// snprintf(command,200,"aplay -Dhw:%d,%d",AlsaDevice.device,AlsaDevice.subdevice);
  snprintf(command,200,"%s",PCM_PLAYER);
  GetPID(command,&PID);

  if (PID != 0)
  {
    /*Error: ressource already used*/
    printf( "\nERROR: device %d already in used\n",Device);
    return HATS_AUDIO_DEVICE_ALREADY_USED;
  }

  /* Check mode */
  for(i= 0; i <HATS_AUDIO_NB_PB_MODE;i++)
  {
    if(Mode == hats_audio_PB_mode[i].ID)
    {
      if(hats_audio_PB_mode[i].Status != HATS_AUDIO_NO_ERROR)
      {
        printf("\nERROR: %s not implemented at this time\n", hats_audio_PB_mode[i].name);
        status = HATS_AUDIO_NOT_AVAILABLE;
      }
      hats_audio_contex[Device].PlaybackMode = Mode;
      break;
    }
  }
  if(i == HATS_AUDIO_NB_PB_MODE)
  {
     printf("\nERROR: invalid playback mode %d\n", Mode);
     status = HATS_AUDIO_INVALID_PARAMETER;
  }
  if( status == HATS_AUDIO_NO_ERROR)
  {
       /*=> enable to 8 channel*/
			status = SetMultichannelMode(AUDIO_DEVICE_0,MULTI_CHANNEL_ENABLE);
  }
  if(( status == HATS_AUDIO_NO_ERROR) &&(Device ==AUDIO_DEVICE_0))
  {

#ifdef CR_CAP_73_001_STW4500_AUDIOLOOP
    if( Mode == AB_DIGITAL_LOOPBACK_MODE)
    {
MC_TRACE_DBG(printf("%s %s %d Out_Organ=%d In_Organ=%d\n", __FILE__,__func__,__LINE__,Out_Organ,In_Organ));
		status = ExecuteScript(Dloop_configs[Out_Organ][In_Organ], NULL);
		return status;
    }
    else
#endif /* CR_CAP_73_001_STW4500_AUDIOLOOP */
    { 
      memset(command,0,sizeof(command));
		  for(i = HATS_ALSA_PB_FIFO_MODE;i<NB_ALSA_PB_MODE;i++)
		  {
			  if(hats_audio_PB_mode[Mode].alsa_mode[i] != 0)
			  {
          strncat(command,"\'ENABLE\' ",200);
			  }
        else
			  {
          strncat(command,"\'DISABLE\' ",200);
			  }
		  }
	    status |= SetMixerCtrl(&AlsaDevice,HATS_AUDIO_PCM_RENDERING_MODE,  command);
    }
  }
  if(( status == HATS_AUDIO_NO_ERROR) &&(Device ==AUDIO_DEVICE_0))
  {
#if 0  /* code deactivate until correctio of CAP_714_001 */
    status = SetMixerCtrl(&AlsaDevice,HATS_AUDIO_PLAYBACK_POWER, "\'ENABLE\'");
#endif /* if 0*/
  }
  else if(Device ==AUDIO_DEVICE_HDMI)
  {
				PowerHDMIaudioInput(HATS_HDMI_AUDIO_ON);
  }
  if( status == HATS_AUDIO_NO_ERROR)
  {
  switch(Mode)
  {
    case NORMAL_PLAYBACK_MODE:
    case FM_TX_MODE:
      /* Prepare command */
 //     snprintf(command,200,"aplay -Dhw:%d,%d %s&",AlsaDevice.device,AlsaDevice.subdevice,FilePath);
	  snprintf(command,200,"%s -Dhw:%d,%d %s&",PCM_PLAYER,AlsaDevice.device,AlsaDevice.subdevice,FilePath);

      /* Start the playback */
      printf( "\nCOMMAND: %s\n",command);
      system(command);
	  status = ExecuteScript(hats_audio8520_output[Out_Organ].script, NULL);
      /* GetPID*/
      //snprintf(command,20,"aplay -Dhw:%d,%d",AlsaDevice.device,AlsaDevice.subdevice);
	  snprintf(command,200,"%s",PCM_PLAYER);
      sleep(1);
      GetPID(command,&PID);
      if(PID == 0)
      {
         /* no ID found means error during playback processing*/
         printf( "\nERROR: error during playback processing\n");
          status = HATS_AUDIO_FAILED;
      }
      break;
    case DIGITAL_LOOPBACK_MODE:
            /* check resource by verifying that no PID associated to arecord command is already running */
            snprintf(command, MAX_LGTH_COMMAND, "%s", ARECORD_FOR_DIGITAL_LOOPBACK);
            GetPID(command, &PID);

            if (PID != 0) {
                /*Error: ressource already used*/
                printf("AUDIOSERVICES ERROR: device %s already in used\n", AlsaDevice.name);
                return HATS_AUDIO_DEVICE_ALREADY_USED;
            } else {
                snprintf(command, MAX_LGTH_COMMAND, "%s", APLAY_FOR_DIGITAL_LOOPBACK);
                GetPID(command, &PID);

                if (PID != 0) {
                    /*Error: ressource already used*/
                    printf("AUDIOSERVICES ERROR: device %s already in used\n", AlsaDevice.name);
                    return HATS_AUDIO_DEVICE_ALREADY_USED;
                }
            }

            snprintf(command, MAX_LGTH_COMMAND, "%s|%s", ARECORD_FOR_DIGITAL_LOOPBACK, APLAY_FOR_DIGITAL_LOOPBACK);
            status = StartCommand(command);
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
  char command[200];
  int32_t status = HATS_AUDIO_NO_ERROR;
  uint32_t PID;
  uint32_t i;
  hats_alsa_Device_t AlsaDevice;
  
  /* check resource by verifying that no PID associated to command is already running */                                                 
  memset(command,0,sizeof(command));                                                
  GetAlsaDevice(Device,CODEC_CAPTURE,&AlsaDevice);
  //snprintf(command,50,"arecord -Dhw:%d,%d",AlsaDevice.device,AlsaDevice.subdevice);        
  snprintf(command,50,"arecord -r 48000 -f S16");         
  GetPID(command,&PID);               

  if (PID != 0)
  {
    /*Error: ressource already used*/
    printf( "\nERROR: device %d already in used\n",AlsaDevice.device);
    return HATS_AUDIO_DEVICE_ALREADY_USED;
  }
  /* Check mode */
  for(i= 0; i <HATS_AUDIO_NB_CP_MODE;i++)
  {
    if(Mode == hats_audio_CP_mode[i].ID)
    {
      if(hats_audio_CP_mode[i].Status != HATS_AUDIO_NO_ERROR)
      {
        printf("\nERROR: %s not implemented at this time\n", hats_audio_CP_mode[i].name);
        status = HATS_AUDIO_NOT_AVAILABLE;
      }
      hats_audio_contex[Device].CaptureMode = Mode;
      break;
    }
  }
  if(i == HATS_AUDIO_NB_CP_MODE)
  {
        printf("\nERROR: invalid capture mode %d\n", Mode);
        status = HATS_AUDIO_INVALID_PARAMETER;
  }
  if(( status == HATS_AUDIO_NO_ERROR)&&(Device ==AUDIO_DEVICE_0))
  {
    memset(command,0,sizeof(command));
		for(i = HATS_ALSA_CP_FM_MODE;i<NB_ALSA_CP_MODE;i++)
		{
			if(hats_audio_CP_mode[Mode-HATS_ALSA_CP_FM_MODE].alsa_mode[i] != 0)
			{
        strncat(command,"\'ENABLE\' ",200);
			}
      else
			{
        strncat(command,"\'DISABLE\' ",200);
			}
		}
#if 0  /* code deactivate until correctio of CAP_714_001 */
		status |= SetMixerCtrl(&AlsaDevice,HATS_AUDIO_PCM_CAPTURE_MODE,  command);
#endif /*if 0*/
  }

  if(( status == HATS_AUDIO_NO_ERROR) &&(Device ==AUDIO_DEVICE_0))
  {
#if 0  /* code deactivate until correctio of CAP_714_001 */
    status = SetMixerCtrl(&AlsaDevice,HATS_AUDIO_CAPTURE_POWER, "\'ENABLE\'");
#endif /*if 0*/
  }

  if( status == HATS_AUDIO_NO_ERROR)
  {
    /* Prepare command */                                                 
    memset(command,0,100);                                                
//    snprintf(command,200,"arecord -Dhw:%d,%d %s&",AlsaDevice.device,AlsaDevice.subdevice,FilePath);         
      snprintf(command,50,"arecord -r 48000 -f S16 %s&",FilePath);         
                                                                         
    switch(Mode)
    {
      case NORMAL_CAPTURE_MODE:
      case FM_CAPTURE_MODE:
        /* Start the record */                                              
      printf( "\nCOMMAND: %s\n",command);
        system(command);
        
        /* GetPID*/                                                           
     // snprintf(command,50,"arecord -Dhw:%d,%d",AlsaDevice.device,AlsaDevice.subdevice);         
      snprintf(command,50,"arecord -r 48000 -f S16");         
        sleep(1);
	GetPID(command,&PID);               
        if(PID == 0)
        {
            /* no ID found means error during capture processing*/
           printf( "\nERROR: error during capture processing\n");
         status = HATS_AUDIO_FAILED;
        }
        break;
      case ANALOG_LOOPBACK_MODE:
        AlsaDevice.subdevice = 0;
#if 0  /* code deactivate until correctio of CAP_714_001 */
    		status = SetMixerCtrl(&AlsaDevice,HATS_AUDIO_PLAYBACK_POWER, "\'ENABLE\'");
#endif /* code deactivate until correctio of CAP_714_001 */
        AlsaDevice.subdevice = HATS_AUDIO_NO_DEVICE;
      	status = SetMixerCtrl(&AlsaDevice,HATS_AUDIO_ANALOG_LOOPBACK,  "\'ENABLE\'");
        break;
			default:
			  break;
    }
  }
  return status;
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
	int32_t status = HATS_AUDIO_NO_ERROR;
	char command[200]="";
	MC_TRACE_DBG(printf("Entering %s device=%d subdev=%d Ctrl=%d decod_Ctrl=%s Param=%s\n",
		__func__, pDevice->device,pDevice->subdevice, Ctrl,hats_audio_mixer_ctrl[Ctrl],p_param ));
	switch(Ctrl)
	{	
		case HATS_AUDIO_PCM_SRCE:
		
		MC_TRACE_DBG(printf("SetMixerCtrl device=%d subdev=%d Ctrl=%s Param=%s\n"
		, pDevice->device,pDevice->subdevice, hats_audio_mixer_ctrl[Ctrl],p_param )); 

		status = ExecuteCommand("amixer cset iface=MIXER,name='LineIn Right' Disabled", pDevice);
		status = ExecuteCommand("amixer cset iface=MIXER,name='LineIn Left' Disabled", pDevice);
		status = ExecuteCommand("amixer cset iface=MIXER,name='Mic 1' Disabled", pDevice);
		status = ExecuteCommand("amixer cset iface=MIXER,name='Mic 2' Disabled", pDevice);
		status = ExecuteCommand("amixer cset iface=MIXER,name='DMic 1' Disabled", pDevice);
		status = ExecuteCommand("amixer cset iface=MIXER,name='DMic 2' Disabled", pDevice);
		status = ExecuteCommand("amixer cset iface=MIXER,name='DMic 3' Disabled", pDevice);
		status = ExecuteCommand("amixer cset iface=MIXER,name='DMic 4' Disabled", pDevice);
		status = ExecuteCommand("amixer cset iface=MIXER,name='DMic 5' Disabled", pDevice);
		status = ExecuteCommand("amixer cset iface=MIXER,name='DMic 6' Disabled", pDevice);
		status = ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface AD 1 Loopback Switch' 'Disabled'", pDevice);
		status = ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface AD 2 Loopback Switch' 'Disabled'", pDevice);


		if (strcmp(p_param, "\'CODEC_SRC_LINEIN\'")==0)
		{		
			status = ExecuteCommand("amixer cset iface=MIXER,name='LineIn Right' Enabled", pDevice);
			status = ExecuteCommand("amixer cset iface=MIXER,name='LineIn Left' Enabled", pDevice);
			status = ExecuteCommand("amixer cset iface=MIXER,name='Mic 2 or LINR Select Capture Route' 'LineIn Right'", pDevice);
			status = ExecuteCommand("amixer cset iface=MIXER,name='AD 1 Select Capture Route' 'LineIn Left'", pDevice);
			status = ExecuteCommand("amixer cset iface=MIXER,name='AD 2 Select Capture Route' 'LineIn Right'", pDevice);
			status = ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface AD To Slot 0 Map' 'AD_OUT1'", pDevice);
			status = ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface AD To Slot 1 Map' 'AD_OUT2'", pDevice);

		}
		else if (strcmp(p_param, "\'CODEC_SRC_MICROPHONE_1A\'")==0)
		{
			status = ExecuteCommand("amixer cset iface=MIXER,name='Mic 1' Enabled", pDevice);
			status = ExecuteCommand("amixer cset iface=MIXER,name='Mic 1A or 1B Select Capture Route' 'Mic 1A'", pDevice);
			status = ExecuteCommand("amixer cset iface=MIXER,name='AD 3 Select Capture Route' 'Mic 1'", pDevice);
			status = ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface AD To Slot 0 Map' 'AD_OUT3'", pDevice);
			status = ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface AD To Slot 1 Map' 'AD_OUT3'", pDevice);
		}
		else if (strcmp(p_param, "\'CODEC_SRC_MICROPHONE_1B\'")==0)
		{		
			status = ExecuteCommand("amixer cset iface=MIXER,name='Mic 1' Enabled", pDevice);
			status = ExecuteCommand("amixer cset iface=MIXER,name='Mic 1A or 1B Select Capture Route' 'Mic 1B'", pDevice);
			status = ExecuteCommand("amixer cset iface=MIXER,name='AD 3 Select Capture Route' 'Mic 1'", pDevice);
			status = ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface AD To Slot 0 Map' 'AD_OUT3'", pDevice);
			status = ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface AD To Slot 1 Map' 'AD_OUT3'", pDevice);
		}
		else if (strcmp(p_param, "\'CODEC_SRC_MICROPHONE_2\'")==0)
		{
			status = ExecuteCommand("amixer cset iface=MIXER,name='Mic 2' on", pDevice);
			status = ExecuteCommand("amixer cset iface=MIXER,name='Mic 2 or LINR Select Capture Route' 'Mic 2'", pDevice);
			status = ExecuteCommand("amixer cset iface=MIXER,name='AD 5 Select Capture Route' 'Mic 2'", pDevice);
			status = ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface AD To Slot 0 Map' 'AD_OUT5'", pDevice);
			status = ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface AD To Slot 1 Map' 'AD_OUT5'", pDevice);
		}
		else if (strcmp(p_param, "\'CODEC_SRC_D_MICROPHONE_12\'")==0)
		{			
			status = ExecuteCommand("amixer cset iface=MIXER,name='DMic 1' Enabled", pDevice);
			status = ExecuteCommand("amixer cset iface=MIXER,name='DMic 2' Enabled", pDevice);
			status = ExecuteCommand("amixer cset iface=MIXER,name='AD 1 Select Capture Route' 'DMic 1'", pDevice);
			status = ExecuteCommand("amixer cset iface=MIXER,name='AD 2 Select Capture Route' 'DMic 2'", pDevice);
			status = ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface AD To Slot 0 Map' 'AD_OUT1'", pDevice);
			status = ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface AD To Slot 1 Map' 'AD_OUT2'", pDevice);
		}
		else if (strcmp(p_param, "\'CODEC_SRC_D_MICROPHONE_34\'")==0)
		{		
			status = ExecuteCommand("amixer cset iface=MIXER,name='DMic 3' Enabled", pDevice);
			status = ExecuteCommand("amixer cset iface=MIXER,name='DMic 4' Enabled", pDevice);
			status = ExecuteCommand("amixer cset iface=MIXER,name='AD 3 Select Capture Route' 'DMic 3'", pDevice);
			status = ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface AD To Slot 0 Map' 'AD_OUT3'", pDevice);
			status = ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface AD To Slot 1 Map' 'AD_OUT4'", pDevice);
		}
		else if (strcmp(p_param, "\'CODEC_SRC_D_MICROPHONE_56\'")==0)
		{		
			status = ExecuteCommand("amixer cset iface=MIXER,name='DMic 5' Enabled", pDevice);
			status = ExecuteCommand("amixer cset iface=MIXER,name='DMic 6' Enabled", pDevice);
			status = ExecuteCommand("amixer cset iface=MIXER,name='AD 5 Select Capture Route' 'DMic 5'", pDevice);
			status = ExecuteCommand("amixer cset iface=MIXER,name='AD 6 Select Capture Route' 'DMic 6'", pDevice);
			status = ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface AD To Slot 0 Map' 'AD_OUT5'", pDevice);
			status = ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface AD To Slot 1 Map' 'AD_OUT6'", pDevice);
		}
		else if (strcmp(p_param, "\'CODEC_SRC_FM_RX\'")==0)
		{
			/* pDevice should be NO_DEVICE */
			pDevice->device =HATS_AUDIO_NO_DEVICE;
			pDevice->subdevice =HATS_AUDIO_NO_DEVICE;

			ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface DA 7 From Slot Map' 'SLOT24'", pDevice);
			ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface DA 8 From Slot Map' 'SLOT25'", pDevice);

			ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface AD 1 Loopback Switch' 'Enabled'", pDevice);
			ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface AD 2 Loopback Switch' 'Enabled'", pDevice);

			ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface AD To Slot 0 Map' 'AD_OUT1'", pDevice);
			ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface AD To Slot 1 Map' 'AD_OUT2'", pDevice);

		}			
		break;
		
		case HATS_AUDIO_PCM_SINK:
		MC_TRACE_DBG(printf("SetMixerCtrl device=%d subdev=%d Ctrl=%s Param=%s\n"
		, pDevice->device,pDevice->subdevice, hats_audio_mixer_ctrl[Ctrl],p_param ));  		

		status = ExecuteCommand("amixer cset iface=MIXER,name='Headset Left' Disabled", pDevice);
		status = ExecuteCommand("amixer cset iface=MIXER,name='Headset Right' Disabled", pDevice);
		status = ExecuteCommand("amixer cset iface=MIXER,name='Earpiece' Disabled", pDevice);
		status = ExecuteCommand("amixer cset iface=MIXER,name='Vibra 1' 'Disabled'", pDevice);
		status = ExecuteCommand("amixer cset iface=MIXER,name='Vibra 2' 'Disabled'", pDevice);
		status = ExecuteCommand("amixer cset iface=MIXER,name='IHF Left' 'Disabled'", pDevice);
		status = ExecuteCommand("amixer cset iface=MIXER,name='IHF Right' 'Disabled'", pDevice);

		
		if (strcmp(p_param, "\'CODEC_DEST_HEADSET\'")==0)
		{	
				status = ExecuteCommand("amixer cset iface=MIXER,name='Headset Left' Enabled", pDevice);
				status = ExecuteCommand("amixer cset iface=MIXER,name='Headset Right' Enabled", pDevice);
				status = ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface DA 1 From Slot Map' 'SLOT9'", pDevice);
				status = ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface DA 2 From Slot Map' 'SLOT11'", pDevice);
		}
		else if (strcmp(p_param, "\'CODEC_DEST_EARPIECE\'")==0)
		{
				status = ExecuteCommand("amixer cset iface=MIXER,name='IHF Left Source Playback Route' 'Audio Path'", pDevice);
				status = ExecuteCommand("amixer cset iface=MIXER,name='Earpiece or LineOut Mono Source' 'IHF Left'", pDevice);
				status = ExecuteCommand("amixer cset iface=MIXER,name='Earpiece' Enabled", pDevice);
				status = ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface DA 3 From Slot Map' 'SLOT11'", pDevice);
		}
		else if (strcmp(p_param, "\'CODEC_DEST_HANDSFREE\'")==0)
		{
				status = ExecuteCommand("amixer cset iface=MIXER,name='IHF Left Source Playback Route' 'Audio Path'", pDevice);
				status = ExecuteCommand("amixer cset iface=MIXER,name='IHF Right Source Playback Route' 'Audio Path'", pDevice);
				status = ExecuteCommand("amixer cset iface=MIXER,name='IHF Left' 'Enabled'", pDevice);
				status = ExecuteCommand("amixer cset iface=MIXER,name='IHF Right' 'Enabled'", pDevice);
				status = ExecuteCommand("amixer cset iface=MIXER,name='IHF or LineOut Select' 'IHF'", pDevice);
				status = ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface DA 3 From Slot Map' 'SLOT9'", pDevice);
				status = ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface DA 4 From Slot Map' 'SLOT11'", pDevice);
		}
		else if (strcmp(p_param, "\'CODEC_DEST_VIBRATOR1\'")==0)
		{
				status = ExecuteCommand("amixer cset iface=MIXER,name='Vibra 1' 'Enabled'", pDevice);
				status = ExecuteCommand("amixer cset iface=MIXER,name='Vibra 1 Controller Playback Route' 'Audio Path'", pDevice);
				status = ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface DA 5 From Slot Map' 'SLOT9'", pDevice);
		}
		else if (strcmp(p_param, "\'CODEC_DEST_VIBRATOR2\'")==0)
		{
				status = ExecuteCommand("amixer cset iface=MIXER,name='Vibra 2' 'Enabled'", pDevice);
				status = ExecuteCommand("amixer cset iface=MIXER,name='Vibra 2 Controller Playback Route' 'Audio Path'", pDevice);
				status = ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface DA 6 From Slot Map' 'SLOT9'", pDevice);
		}
		else if (strcmp(p_param, "\'CODEC_DEST_FM_TX\'")==0)
		{
				/* pDevice should be NO_DEVICE */
				pDevice->device =HATS_AUDIO_NO_DEVICE;
				pDevice->subdevice =HATS_AUDIO_NO_DEVICE;

				ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface DA 7 From Slot Map' 'SLOT9'", pDevice);
				ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface DA 8 From Slot Map' 'SLOT11'", pDevice);

				ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface AD 1 Loopback Switch' 'Enabled'", pDevice);
				ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface AD 2 Loopback Switch' 'Enabled'", pDevice);

				ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface AD To Slot 16 Map' 'AD_OUT1'", pDevice);
				ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface AD To Slot 17 Map' 'AD_OUT2'", pDevice);

		}
		break;
		
		case HATS_AUDIO_CAPTURE_VOL: 
		MC_TRACE_DBG(printf("SetMixerCtrl device=%d subdev=%d Ctrl=%s Param=%s\n"
		, pDevice->device,pDevice->subdevice, hats_audio_mixer_ctrl[Ctrl],p_param ));  		
		/* 0..63*/
		snprintf(command,200,"amixer cset iface=MIXER,name='LineIn Master Gain Capture Volume' %s",p_param);
		ExecuteCommand(command, pDevice);
		snprintf(command,200,"amixer cset iface=MIXER,name='Mic Master Gain Capture Volume' %s",p_param);
		ExecuteCommand(command, pDevice);
		snprintf(command,200,"amixer cset iface=MIXER,name='HD Mic Master Gain Capture Volume' %s",p_param);
		ExecuteCommand(command, pDevice);
		
		/* dft values for others volumes*/
		ExecuteCommand("amixer cset iface=MIXER,name='Mic 1 Capture Volume' 10,10", pDevice);
		ExecuteCommand("amixer cset iface=MIXER,name='Mic 2 Capture Volume' 10,10", pDevice);
		ExecuteCommand("amixer cset iface=MIXER,name='LineIn Capture Volume' 10,10", pDevice);
		ExecuteCommand("amixer cset iface=MIXER,name='LineIn to Headset Bypass Playback Volume' 10,10", pDevice);
		break;

		case HATS_AUDIO_PLAYBACK_VOL: 
		MC_TRACE_DBG(printf("SetMixerCtrl device=%d subdev=%d Ctrl=%s Param=%s\n"
		, pDevice->device,pDevice->subdevice, hats_audio_mixer_ctrl[Ctrl],p_param ));  		
		/* 0..63*/
		snprintf(command,200,"amixer cset iface=MIXER,name='Headset Master Gain Playback Volume' %s",p_param);
		ExecuteCommand(command, pDevice);
		snprintf(command,200,"amixer cset iface=MIXER,name='IHF Master Gain Playback Volume' %s",p_param);
		ExecuteCommand(command, pDevice);
		snprintf(command,200,"amixer cset iface=MIXER,name='Vibra Master Gain Playback Volume' %s",p_param);
		ExecuteCommand(command, pDevice);
		/* dft values for others volumes*/
		ExecuteCommand("amixer cset iface=MIXER,name='Headset Digital Gain Playback Volume' 9,9", pDevice);
		ExecuteCommand("amixer cset iface=MIXER,name='Headset Gain Playback Volume' 10,10", pDevice);
		
		break;
		
		case HATS_AUDIO_DIGITAL_LOOPBACK: 
		MC_TRACE_DBG(printf("SetMixerCtrl device=%d subdev=%d Ctrl=%s Param=%s %d\n"
		, pDevice->device,pDevice->subdevice, hats_audio_mixer_ctrl[Ctrl],p_param,__LINE__ ));
/*		ExecuteCommand("amixer cset iface=MIXER,name='Digital Interface 0 FIFO Enable Switch' Enabled ", pDevice);*/

		/* pDevice should be NO_DEVICE */
		pDevice->device =HATS_AUDIO_NO_DEVICE;
		pDevice->subdevice =HATS_AUDIO_NO_DEVICE;
		if (strcmp(p_param,"\'ENABLE\'" )==0)
		{
			ExecuteCommand(DIGITAL_LOOPBACK_DB_COMMAND, pDevice);
		}
		else if (strcmp(p_param,"\'DISABLE\'" )==0)
		{
			KillPID("arecord");
			KillPID("aplay");			
		}	
			
		break;
		
		case HATS_AUDIO_ANALOG_LOOPBACK: 
		MC_TRACE_DBG(printf("SetMixerCtrl device=%d subdev=%d Ctrl=%s Param=%s\n"
		, pDevice->device,pDevice->subdevice, hats_audio_mixer_ctrl[Ctrl],p_param ));  	

		/* should better to do this in PCMStartCapture fct because no mixer control with this call to stw400audioloop fct*/
		if (strcmp(p_param,"\'ENABLE\'" )==0)
		{
			stw400audioloop(HATS_STW4500_ANALOGLOOP);
		}
		else if (strcmp(p_param,"\'DISABLE\'" )==0)
		{
			stw400audioloop(HATS_STW4500_LOOP_OFF);
		}
		break;
		
		default:
			/* only for debug*/ 
			MC_TRACE_DBG(printf("SetMixerCtrl stubed device=%d subdev=%d Ctrl=%s Param=%s\n"
				, pDevice->device,pDevice->subdevice, hats_audio_mixer_ctrl[Ctrl],p_param ));
		return HATS_AUDIO_NO_ERROR;
		break;	
	}   
                                         
	return HATS_AUDIO_NO_ERROR;
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
  char command[200];
  int32_t status = HATS_AUDIO_NO_ERROR;
  uint32_t  volume[2] = {0,0};

    /* delete status file */
    system("rm -r /tmp/mixerstatus.txt");
    /* Prepare command */
    memset(command,0,sizeof(command));                                                
    if(pDevice->device !=HATS_AUDIO_NO_DEVICE)
		{
			if(pDevice->subdevice !=HATS_AUDIO_NO_DEVICE)
			{
				snprintf(command,200,"amixer -c 0 cget iface=MIXER,name=\'%s\',device=%d,subdevice=%d || echo \"ERROR\" >/tmp/mixerstatus.txt",hats_audio_mixer_ctrl[Ctrl],pDevice->device,pDevice->subdevice);
			}
			else
			{
				snprintf(command,200,"amixer -c 0 cget iface=MIXER,name=\'%s\',device=%d || echo \"ERROR\" >/tmp/mixerstatus.txt",hats_audio_mixer_ctrl[Ctrl],pDevice->device);
			}
    }
		else
		{
			snprintf(command,200,"amixer -c 0 cget iface=MIXER,name=\'%s\' || echo \"ERROR\" >/tmp/mixerstatus.txt",hats_audio_mixer_ctrl[Ctrl]);
		}
    
    /*set mixer control*/                                              
    //printf("\n%s\n",command); /* only for debug*/                                              
    system(command);                                             

    /* treatment to retreive parameter value */
    switch( Ctrl)
    {
      case HATS_AUDIO_PCM_SRCE:
        /* TO BE COMPLETED */
        *p_param = CODEC_NB_OF_SRC;
          status = HATS_AUDIO_NOT_AVAILABLE;
        break;
      case HATS_AUDIO_PCM_SINK:
        /* TO BE COMPLETED */
        *p_param = CODEC_NB_OF_DEST;
          status = HATS_AUDIO_NOT_AVAILABLE;
        break;
      case HATS_AUDIO_CAPTURE_VOL:
        /* TO BE COMPLETED */
        *p_param = volume[0] |(volume[1]<<8);
          status = HATS_AUDIO_NOT_AVAILABLE;
        break;
      case HATS_AUDIO_PLAYBACK_VOL:
        /* TO BE COMPLETED */
        ((uint32_t*)p_param)[0] = volume[0] ;
        ((uint32_t*)p_param)[1] = volume[1] ;
         status = HATS_AUDIO_NOT_AVAILABLE;
        break;
      case HATS_AUDIO_CAPTURE_MUTE:
        /* TO BE COMPLETED */
       *p_param = STREAM_UNMUTED;
       status = HATS_AUDIO_NOT_AVAILABLE;
        break;
      case HATS_AUDIO_PLAYBACK_MUTE:
        /* TO BE COMPLETED */
       *p_param = STREAM_UNMUTED;
          status = HATS_AUDIO_NOT_AVAILABLE;
        break;
       default:
         status = HATS_AUDIO_INVALID_PARAMETER;
    }
  return status;
}
/*--------------------------------------------------------------------------------------*/
/*!   \ingroup internal_audio_services
  \fn void GetAlsaDevice(e_HatsAudioDevice_t device, e_HatsAudioMode_t Mode, hats_alsa_Device_t* pDevice)
  \brief Get Alsa device a  nd subdevice
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
    
    if( Device == AUDIO_DEVICE_0)  /* playback or capture on standard organs */
    {
     if( Type == CODEC_PLAYBACK)
     {
       pAlsaDevice->device = 0;
       pAlsaDevice->subdevice = 1;
     }
     else if( Type == CODEC_CAPTURE)
     {
       pAlsaDevice->device = 0;
       pAlsaDevice->subdevice = 2;
     }
     else
     {
       pAlsaDevice->device = 0;
     }
    }
    if( Device == AUDIO_DEVICE_BT) /* playback or capture on Bluetooth */
    {
       pAlsaDevice->device = 0;
       pAlsaDevice->subdevice = 3;
    }
    if( Device == AUDIO_DEVICE_HDMI) /* playback on HDMI */
    {
       pAlsaDevice->device = 0;
       pAlsaDevice->subdevice = 0;
    }
}

/*--------------------------------------------------------------------------------------*/
/*! \ingroup audio_services
	\fn uint32_t SetMultichannelMode(e_HatsAudioChannelMode_t Mode);
	\brief Service allowing to retrieve the mute state of the specified PCM transfer
	\param [in] Mode Alsa behaviour mode \ref e_HatsAudioChannelMode_t
	\return 	
		0 if no error
*/
/*--------------------------------------------------------------------------------------*/
uint32_t SetMultichannelMode(e_HatsAudioDevice_t Device,e_HatsAudioChannelMode_t Mode)
{

  uint32_t status = HATS_AUDIO_NO_ERROR;
  hats_alsa_Device_t AlsaDevice;
  /* Check action */
  if((Device != AUDIO_DEVICE_0)||(Mode >= NB_MULTICHANNEL_MODE))
  {
     printf("\none parameter is out of range : DEVICE = %d  MODE = %d\n",Device,Mode );        
     status = HATS_AUDIO_INVALID_PARAMETER;   
  }
  else
  { 
     /* Change the sink organ */
    GetAlsaDevice(Device,CODEC_MASTER,&AlsaDevice);
    if( Mode == MULTI_CHANNEL_DISABLE)
		{
			status = SetMixerCtrl(&AlsaDevice,HATS_AUDIO_MULTI_CHANNEL_MODE,"\'DISABLE\'");
		}
		else
		{
			status = SetMixerCtrl(&AlsaDevice,HATS_AUDIO_MULTI_CHANNEL_MODE,"\'ENABLE\'");
		}
  }
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
  char command[200];
  if((Device != AUDIO_DEVICE_0))
  {
     printf("\none parameter is out of range : DEVICE = %d\n",Device);        
     status = HATS_AUDIO_INVALID_PARAMETER;   
  }
  else
  { 
		snprintf(command,200,"MMTE -f %s/StartCSCall.ate",HATS_AUDIO_MMTE_SCRIPT_PATH);
    //printf("\n%s\n",command); /* only for debug*/                                              
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
  char command[200];
  
  if((Device != AUDIO_DEVICE_0))
  {
     printf("\none parameter is out of range : DEVICE = %d\n",Device);        
     status = HATS_AUDIO_INVALID_PARAMETER;   
  }
  else
  { 
		snprintf(command,200,"MMTE -f %s/StopCSCall.ate",HATS_AUDIO_MMTE_SCRIPT_PATH);
    //printf("\n%s\n",command); /* only for debug*/                                              
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
    
   /* 1- open hdmi system file   */
    fdesc = fopen (HATS_AUDIO_HDMI_AUDIOCFG_SYSFS,"w");
 	if(( fdesc == NULL) )
    { 
    	printf("error when try to open file: %s\n", HATS_AUDIO_HDMI_AUDIOCFG_SYSFS);
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
	  	printf("error when try to access file: %s\n", HATS_AUDIO_HDMI_AUDIOCFG_SYSFS);
        vl_error=HATS_AUDIO_FAILED;
      }
      fclose(fdesc);     
    }
    /* 3- close hdmi system file */
  	return vl_error;	
}
