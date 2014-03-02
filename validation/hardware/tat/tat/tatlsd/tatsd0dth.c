/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides services for power services module
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "dthsd.h"
#include "tatsddth.h"
#include <libab.h>

int dth_init_service()
{

	int result = 0;

	STORAGE_SYSLOG(LOG_DEBUG,"dth_init_service of tatsd");			

	mySDCardState.CardInserted=0;
	mySDCardState.PowerSupplyValue=0;
	mySDCardState.RegulatorValue=0;
	mySDCardState.ControlerStatus=0;
	mySDCardState.ChecksumCheck=0;
	mySDCardState.CardMounted=0;
	mySDCardState.FileCopied=0;

	
	/*Register Action arborescence */
	tatlsd_register_dth_struct();
	
	return result ;

}

int Dth_ActSDPresence_exec(struct dth_element *elem)
{
	int vl_value=0,vl_temp=0;
	FILE * pl_file=NULL;
	char commandLineDetection[MAX_LOCAL_STRING_SIZE]="\0";
	char outputFileDetection[MAX_LOCAL_STRING_SIZE]="\0";
	char registerDetectionValue[MAX_LOCAL_STRING_SIZE]="0x8000E000";

	u8 reg_PowerSupplyVal=0;
	uint16_t reg_PowerSupplyAddress=0x421;

	uint16_t reg_RegulatorAddress=0x40A;
	u8 reg_RegulatorVal=0;

	char commandControlerLine[MAX_LOCAL_STRING_SIZE]="\0";
	char outputControlerFile[MAX_LOCAL_STRING_SIZE]="\0";
	char registerControlerValue[MAX_LOCAL_STRING_SIZE]="0x80126000";

	char commandClockLine[MAX_LOCAL_STRING_SIZE]="\0";
	char outputClockFile[MAX_LOCAL_STRING_SIZE]="\0";
	char registerClockValue[MAX_LOCAL_STRING_SIZE]="0x80126004";

	STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDPresence_exec of tatsd");

	if (elem == NULL)
		return -1;

	/*initialization*/
	mySDCardState.CardInserted=0;
	mySDCardState.PowerSupplyValue=0;
	mySDCardState.RegulatorValue=0;
	mySDCardState.ControlerStatus=0;
	  /*SD card Detection*/
	sprintf(outputFileDetection,"%s%s",FILE_PATH,OUTPUT_PRESENCE_FILE);
	sprintf(commandLineDetection,"sh %s%s %s %s",FILE_PATH,INPUT_SCRIPT_PRESENCE_FILE,outputFileDetection,registerDetectionValue);
	system( commandLineDetection);

	STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDPresence_exec outputFileDetection : %s",outputFileDetection);
	STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDPresence_exec commandLineDetection : %s",commandLineDetection);
 
	pl_file=fopen(outputFileDetection,"r");
	if(pl_file!=NULL)
	{
		fscanf(pl_file,"%x",&vl_value);
		vl_temp=vl_value&0x80000000;
		vl_temp=(vl_temp>>31)&1;
		mySDCardState.CardInserted=vl_temp;
		STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDPresence_exec CardInserted : %d",vl_temp);
		fclose(pl_file);
	}
	

	/*Voltage detection*/
	abxxxx_read(reg_PowerSupplyAddress,&reg_PowerSupplyVal);
	vl_temp=reg_PowerSupplyVal&7;
	mySDCardState.PowerSupplyValue=vl_temp;
	STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDPresence_exec PowerSupplyValue : %d",vl_temp);

	/*Regulator detection*/
	abxxxx_read(reg_RegulatorAddress,&reg_RegulatorVal);
	vl_temp=reg_RegulatorVal&1;
	mySDCardState.RegulatorValue=vl_temp;
	STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDPresence_exec RegulatorValue : %d",vl_temp);
	/*Controler Status*/
	sprintf(outputControlerFile,"%s%s",FILE_PATH,OUTPUT_PRESENCE_FILE);
	sprintf(commandControlerLine,"sh %s%s %s %s",FILE_PATH,INPUT_SCRIPT_PRESENCE_FILE,outputControlerFile,registerControlerValue);
	system( commandControlerLine);

	pl_file=fopen(outputControlerFile,"r");
	if(pl_file!=NULL)
	{
		fscanf(pl_file,"%x",&vl_value);
		vl_temp=vl_value&0x03;
		mySDCardState.ControlerStatus=vl_temp;
		STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDParamControler_Get (correct=3 / not Correct!=3 ) %x : ",vl_temp);
		fclose(pl_file);
	}
	

	/*Clock Status*/
	sprintf(outputClockFile,"%s%s",FILE_PATH,OUTPUT_PRESENCE_FILE);
	sprintf(commandClockLine,"sh %s%s %s %s",FILE_PATH,INPUT_SCRIPT_PRESENCE_FILE,outputClockFile,registerClockValue);

	STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDPresence_exec outputClockFile : %s",outputClockFile);
	STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDPresence_exec commandClockLine : %s",commandClockLine);
	system( commandClockLine);

	pl_file=fopen(outputClockFile,"r");
	if(pl_file!=NULL)
	{
		fscanf(pl_file,"%x",&vl_value);
		vl_temp=vl_value&0x00000100;
		vl_temp=(vl_temp>>8)&1;
		mySDCardState.ClockStatus=vl_temp;
		STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDPresence_exec ClockStatus : %d",vl_temp);
		fclose(pl_file);
	}
	



	STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDPresence_exec of tatsd");


	return 0;
}

int Dth_ActSDParamDetection_Get(struct dth_element *elem, void *value)
{
	if ((elem == NULL) && (value == NULL))
		return -1;

	STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDParamDetection_Get CardInserted : %d",mySDCardState.CardInserted);
	*((u8 *)value) =(u8) mySDCardState.CardInserted;
	return 0;
}
int Dth_ActSDParamVoltage_Get(struct dth_element *elem, void *value)
{
	if ((elem == NULL) && (value == NULL))
		return -1;

	STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDParamVoltage_Get PowerSupplyValue : %d",mySDCardState.PowerSupplyValue);
	*((u8 *)value) =(u8) mySDCardState.PowerSupplyValue;
	return 0;
}

int Dth_ActSDParamRegulator_Get(struct dth_element *elem, void *value)
{
	if ((elem == NULL) && (value == NULL))
		return -1;

	STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDParamRegulator_Get RegulatorValue : %d",mySDCardState.RegulatorValue);

	*((u8 *)value) =(u8) mySDCardState.RegulatorValue;
	return 0;
}
int Dth_ActSDParamControler_Get(struct dth_element *elem, void *value)
{
	if ((elem == NULL) && (value == NULL))
		return -1;

	STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDParamControler_Get ControlerStatus : %d",mySDCardState.ControlerStatus);
	*((u8 *)value) =(u8) mySDCardState.ControlerStatus;
	return 0;
}
int Dth_ActSDParamClock_Get(struct dth_element *elem, void *value)
{
	if ((elem == NULL) && (value == NULL))
		return -1;

	STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDParamClock_Get ClockStatus : %d",mySDCardState.ClockStatus);
	*((u8 *)value) =(u8) mySDCardState.ClockStatus;
	return 0;
}

int Dth_ActSDWriteFile_exec(struct dth_element *elem)
{
	int vl_value=0;
	FILE * pl_file=NULL;
	char errorTextMsg[MAX_LOCAL_STRING_SIZE]="\0";


	char commandMountedLine[MAX_LOCAL_STRING_SIZE]="\0";
	char outputMountedFile[MAX_LOCAL_STRING_SIZE]="\0";


	char commandLineCopyFile[MAX_LOCAL_STRING_SIZE]="\0";
	char inputSourceCopyFile[MAX_LOCAL_STRING_SIZE]="\0";
	char inputTargetCopyFile[MAX_LOCAL_STRING_SIZE]="\0";
	char outputTextCopyFile[MAX_LOCAL_STRING_SIZE]="\0";

	char commandLineChecksumSource[MAX_LOCAL_STRING_SIZE]="\0";
	char outputSourceChecksumFile[MAX_LOCAL_STRING_SIZE]="\0";
	char inputSourceChecksumFile[MAX_LOCAL_STRING_SIZE]="\0";
	char checksumSourceFile[MAX_LOCAL_STRING_SIZE]="\0";

	char commandLineChecksumTarget[MAX_LOCAL_STRING_SIZE]="\0";
	char outputTargetChecksumFile[MAX_LOCAL_STRING_SIZE]="\0";
	char inputTargetChecksumFile[MAX_LOCAL_STRING_SIZE]="\0";
	char checksumTargetFile[MAX_LOCAL_STRING_SIZE]="\0";

	if (elem == NULL)
		return -1;

	STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDWriteFile_exec of tatsd");
	/*initialization*/
	mySDCardState.ChecksumCheck=0;
	mySDCardState.CardMounted=0;
	mySDCardState.FileCopied=0;

	/*Mount SDCard*/

	sprintf(outputMountedFile,"%s%s",FILE_PATH,OUTPUT_MOUNT_FILE);
	sprintf(commandMountedLine,"sh %s%s %s",FILE_PATH,INPUT_SCRIPT_MOUNTSD_FILE,outputMountedFile);
	system( commandMountedLine);
	STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDWriteFile_exec outputMountedFile : %s",outputMountedFile);
	STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDWriteFile_exec commandLine : %s",commandMountedLine);
	STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDWriteFile_exec of tatsd");
	pl_file=fopen(outputMountedFile,"r");
	if(pl_file!=NULL)
	{
		fscanf(pl_file,"%s",errorTextMsg);

		STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDWriteFile_exec %s ",errorTextMsg);

		if(strcmp(errorTextMsg,ERROR_MESSAGE))
		{
			STORAGE_SYSLOG(LOG_DEBUG,"dans strcmp Dth_ActSDWriteFile_exec %s ",errorTextMsg);	
			vl_value=1;
		}
		else
		{
			STORAGE_SYSLOG(LOG_DEBUG,"else strcmp Dth_ActSDWriteFile_exec %s ",errorTextMsg);	
			vl_value=0;
		}
		mySDCardState.CardMounted=vl_value;
		fclose(pl_file);
	}
	
        if(vl_value!=0) /*if the SD card is mounted we continue*/
	{
		/*Copy File*/
		vl_value=0;
		sprintf(inputSourceCopyFile,"%s%s",FILE_PATH,INPUT_FILE_TO_COPY);
		sprintf(inputTargetCopyFile,"%s%s",SDCARD_MOUNT_PATH,INPUT_FILE_TO_COPY);
		sprintf(outputTextCopyFile,"%s%s",FILE_PATH,OUTPUT_COPY_FILE);

		sprintf(commandLineCopyFile,"sh %s%s %s %s %s",FILE_PATH,INPUT_SCRIPT_COPY_FILE,inputSourceCopyFile,inputTargetCopyFile,outputTextCopyFile);
		system(commandLineCopyFile);
		STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDWriteFile_exect of tatsd commandLineCopyFile : %s",commandLineCopyFile);


		pl_file=fopen(outputTextCopyFile,"r");
		if(pl_file!=NULL)
		{
			fscanf(pl_file,"%s",errorTextMsg);

			STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDWriteFile_exec errorTextMsg %s ",errorTextMsg);

			if(!strcmp(errorTextMsg,"ok"))
			{
				STORAGE_SYSLOG(LOG_DEBUG,"dans strcmp OK Dth_ActSDWriteFile_exec %s ",errorTextMsg);	
				vl_value=1;
			}
			else
			{
				STORAGE_SYSLOG(LOG_DEBUG,"else strcmp KO Dth_ActSDWriteFile_exec %s ",errorTextMsg);	
				vl_value=0;
			}
			mySDCardState.FileCopied=vl_value;
			fclose(pl_file);
		}
		
 		if(vl_value!=0) /*if the exists on SDcard we continue*/
		{
			/*verify checksum*/
			vl_value=0;
			sprintf(outputSourceChecksumFile,"%s%s",FILE_PATH,OUTPUT_CHECKSUM_SOURCE_FILE);
			sprintf(inputSourceChecksumFile,"%s%s",FILE_PATH,INPUT_FILE_TO_COPY);
			sprintf(commandLineChecksumSource,"sh %s%s %s %s",FILE_PATH,INPUT_SCRIPT_CHECKSUM_FILE,inputSourceChecksumFile,outputSourceChecksumFile);


			system( commandLineChecksumSource);
			STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDWriteFile_exec Checksum : %s",outputSourceChecksumFile);
			STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDWriteFile_exec commandLine : %s",commandLineChecksumSource);
			STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDFileChecksum_Get of tatsd");
			pl_file=fopen(outputSourceChecksumFile,"r");
			if(pl_file!=NULL)
			{
				fscanf(pl_file,"%s",checksumSourceFile);

				STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDWriteFile_exec checksumSourceFile %s ",checksumSourceFile);
				fclose(pl_file);
			}
			
			sprintf(outputTargetChecksumFile,"%s%s",FILE_PATH,OUTPUT_CHECKSUM_TARGET_FILE);
			sprintf(inputTargetChecksumFile,"%s%s",SDCARD_MOUNT_PATH,INPUT_FILE_TO_COPY);
			sprintf(commandLineChecksumTarget,"sh %s%s %s %s",FILE_PATH,INPUT_SCRIPT_CHECKSUM_FILE,inputTargetChecksumFile,outputTargetChecksumFile);
			STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDWriteFile_exec   commandLineChecksumTarget %s ",commandLineChecksumTarget);
			system(commandLineChecksumTarget);
			pl_file=fopen(outputTargetChecksumFile,"r");
			if(pl_file!=NULL)
			{
				fscanf(pl_file,"%s",checksumTargetFile);

				STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDWriteFile_exec dans if target %s ",checksumTargetFile);
				fclose(pl_file);
			}
			STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDWriteFile_exec before compare target %s source %s",checksumTargetFile,checksumSourceFile);
			
			if(!strcmp(checksumTargetFile,checksumSourceFile))
			{
				STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDWriteFile_exec dans if");
				vl_value=1;
			}
			else
			{
				STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDWriteFile_exec dans else");
				vl_value=0;
			}
			mySDCardState.ChecksumCheck=vl_value;

			/*unmount SD Card"*/
			system("umount /mnt/SDCard");
		}
	}
	return 0;
}
int Dth_ActSDMounted_Get(struct dth_element *elem, void *value)
{
	if ((elem == NULL) && (value == NULL))
		return -1;

	STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDMounted_Get mySDCardState.CardMounted : %d",mySDCardState.CardMounted);
	*((u8 *)value) =(u8) mySDCardState.CardMounted;
	return 0;

}

int Dth_ActSDFileCopied_Get(struct dth_element *elem, void *value)
{
	if ((elem == NULL) && (value == NULL))
		return -1;

 	STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDFileCopied_Get FileCopied %d ",mySDCardState.FileCopied);
	*((u8 *)value) =(u8) mySDCardState.FileCopied;

	return 0;
}

int Dth_ActSDFileChecksum_Get(struct dth_element *elem, void *value)
{
	if ((elem == NULL) && (value == NULL))
		return -1;

	STORAGE_SYSLOG(LOG_DEBUG,"Dth_ActSDFileChecksum_Get SDCardState.ChecksumCheck %d ",mySDCardState.ChecksumCheck);
	*((u8 *)value) =(u8) mySDCardState.ChecksumCheck;
	return 0;

}

