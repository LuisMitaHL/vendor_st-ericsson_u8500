/*
* =====================================================================================
* 
*        Filename:  tatl00iqt.c
* 
*     Description:  
* 
*         Version:  1.0
*         Created:  27/08/2010 
*        Revision:  none
*        Compiler:  
* 
*          Author: Jean-Michel Simon 
*         � Copyright ST-Ericsson, 2010. All Rights Reserved
* 
* =====================================================================================
*/
 
//#define PIPE_VERSION
 
#define TATLIQT_C
#include "dthiqt.h"
#include "tatliqt.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
   
   
#undef  TATLIQT_C



/*Init default value for VIDEO */
/****************************/
int dth_init_service()
{	 

static char *ViewFinder_SavePrefix_Path = "savedframes/test20101013_";
static char *ViewFinder_SDcardDirectory_Path = "/mnt/UserFS2/";
static char *StillPicture_SavePrefix_Path = "savedframes/test20101014_";
static char *StillPicture_SDcardDirectory_Path = "/mnt/UserFS2/";
static char *Video_SavePrefix_Path = "savedframes/test20101015_";
static char *Video_HREncodedSavePrefix_Path = "savedframes/test20101016_";
static char *Video_SDcardDirectory_Path = "/mnt/UserFS2/";
 
int result = 0;
 
	SYSLOG(LOG_ERR,"IQ tuning DTH mounting\n");
	result = tatliqtuning_register_dth_struct();
	SYSLOG(LOG_ERR,"Page Element DTH mounting\n");
	result = tatlm1_115PeInitAction();
	SYSLOG(LOG_ERR,"All IQ tuning DTH mounted\n");	

	/* Default Ram Disk size = 64 Mbytes */
	IQT_param_array [AcquisitionControl_RamDiskSize]=64;

	IQT_param_array [ViewFinder_Resolution]=ViewFinder_Resolution_VGA;
	IQT_param_array [ViewFinder_Width]=640;
	IQT_param_array [ViewFinder_Height]=480;
	IQT_param_array [ViewFinder_DisplayActivation]=ViewFinder_DisplayActivation_OFF;
	IQT_param_array [ViewFinder_DisplayResolution]=ViewFinder_LowResolution;
	IQT_param_array [ViewFinder_SaveActivation]=ViewFinder_SaveActivation_OFF;
	IQT_param_array [ViewFinder_SaveResolutionLR]=ViewFinder_SaveResolutionLR_OFF;
	IQT_param_array [ViewFinder_FrameRate]=22;

        IQT_string_array[ViewFinder_SavePrefix]=(int8_t*)calloc(strlen(ViewFinder_SavePrefix_Path)+1,sizeof(int8_t));
        strncpy((char*)IQT_string_array[ViewFinder_SavePrefix],(const char*)ViewFinder_SavePrefix_Path,strlen(ViewFinder_SavePrefix_Path));

	IQT_param_array [ViewFinder_SaveFirstFrame]=1;
	IQT_param_array [ViewFinder_SaveFrameCount]=10;
	IQT_param_array [ViewFinder_SaveMode]=ViewFinder_FIFO;

        IQT_string_array[ViewFinder_SDcardDirectory]=(int8_t*)calloc(strlen(ViewFinder_SDcardDirectory_Path)+1,sizeof(int8_t));
        strncpy((char*)IQT_string_array[ViewFinder_SDcardDirectory],(const char*)ViewFinder_SDcardDirectory_Path,strlen(ViewFinder_SDcardDirectory_Path));

	IQT_param_array [ViewFinder_FrameSkippingNumber]=5;

	IQT_param_array [StillPicture_Resolution]=StillPicture_Resolution_VGA;
	IQT_param_array [StillPicture_Width]=640;
	IQT_param_array [StillPicture_Height]=480;
	IQT_param_array [StillPicture_Mode]=StillPicture_Single;
	IQT_param_array [StillPicture_BurstFrameCount]=5;

	IQT_param_array [StillPicture_DisplayActivation]=StillPicture_DisplayActivation_OFF;
	IQT_param_array [StillPicture_DisplayResolution]=StillPicture_LowResolution;
	IQT_param_array [StillPicture_SaveActivation]=StillPicture_SaveActivation_OFF;
	IQT_param_array [StillPicture_SaveResolutionLR]=StillPicture_SaveResolutionLR_OFF;
	IQT_param_array [StillPicture_SaveResolutionHR]=StillPicture_SaveResolutionHR_OFF;
	IQT_param_array [StillPicture_SaveResolutionRAW]=StillPicture_SaveResolutionRAW_OFF;

	IQT_param_array [StillPicture_FrameRate]=22;

        IQT_string_array[StillPicture_SavePrefix]=(int8_t*)calloc(strlen(StillPicture_SavePrefix_Path)+1,sizeof(int8_t));
        strncpy((char*)IQT_string_array[StillPicture_SavePrefix],(const char*)StillPicture_SavePrefix_Path,strlen(StillPicture_SavePrefix_Path));

	IQT_param_array [StillPicture_SaveFirstFrame]=1;
	IQT_param_array [StillPicture_SaveFrameCount]=5;
	IQT_param_array [StillPicture_SaveMode]=StillPicture_FIFO;

        IQT_string_array[StillPicture_SDcardDirectory]=(int8_t*)calloc(strlen(StillPicture_SDcardDirectory_Path)+1,sizeof(int8_t));
        strncpy((char*)IQT_string_array[StillPicture_SDcardDirectory],(const char*)StillPicture_SDcardDirectory_Path,strlen(ViewFinder_SDcardDirectory_Path));


	IQT_param_array [StillPicture_FrameSkippingNumber]=5;

	IQT_param_array [Video_Resolution]=Video_Resolution_VGA;
	IQT_param_array [Video_Width]=640;
	IQT_param_array [Video_Height]=480;

	IQT_param_array [Video_DisplayActivation]=Video_DisplayActivation_OFF;
	IQT_param_array [Video_DisplayResolution]=Video_LowResolution;
	IQT_param_array [Video_SaveActivation]=Video_SaveActivation_OFF;
	IQT_param_array [Video_HREncodedSaveActivation]=Video_HREncodedSaveActivation_OFF;
 
        IQT_string_array[Video_HREncodedSavePrefix]=(int8_t*)calloc(strlen(Video_HREncodedSavePrefix_Path)+1,sizeof(int8_t));
        strncpy((char*)IQT_string_array[Video_HREncodedSavePrefix],(const char*)Video_HREncodedSavePrefix_Path,strlen(Video_HREncodedSavePrefix_Path));

	IQT_param_array [Video_SaveResolutionLR]=Video_SaveResolutionLR_OFF;
	IQT_param_array [Video_SaveResolutionHR]=Video_SaveResolutionHR_OFF;
 
	IQT_param_array [Video_FrameRate]=22;

        IQT_string_array[Video_SavePrefix]=(int8_t*)calloc(strlen(Video_SavePrefix_Path)+1,sizeof(int8_t));
        strncpy((char*)IQT_string_array[Video_SavePrefix],(const char*)Video_SavePrefix_Path,strlen(Video_SavePrefix_Path));

	IQT_param_array [Video_SaveFirstFrame]=1;
	IQT_param_array [Video_SaveFrameCount]=10;
	IQT_param_array [Video_SaveMode]=Video_FIFO ;

        IQT_string_array[Video_SDcardDirectory]=(int8_t*)calloc(strlen(Video_SDcardDirectory_Path)+1,sizeof(int8_t));
        strncpy((char*)IQT_string_array[Video_SDcardDirectory],(const char*)Video_SDcardDirectory_Path,strlen(Video_SDcardDirectory_Path));

	IQT_param_array [Video_FrameSkippingNumber]=5;
	return result ;
}

int Dth_ActIQT_exec(struct dth_element *elem)
{
        return (tatln1_00ActExec(elem)) ;	
}

int Dth_ActIQTParam_Set(struct dth_element *elem, void *value)
{
       return (tatln1_02ActSet(elem,value)); 	
}

int Dth_ActIQTParam_Get(struct dth_element *elem, void *value)
{
       return (tatln1_03ActGet(elem,value));	
}

int Dth_ElemIQTParam_Set(struct dth_element *elem, void *value)
{	
        return (tatln1_04ActSetDynamic(elem,value));	
}

int Dth_ActIQTPreActionFile_GetVF(struct dth_element *elem, void *value)
{
		if (elem == NULL)
			return -1;

        return(tatlm1_16PreTreatmentActionVF(value));
}

int Dth_ActIQTPreActionFile_GetStill(struct dth_element *elem, void *value)
{
		if (elem == NULL)
			return -1;

        return(tatlm1_19PreTreatmentActionStill(value));
}
int Dth_ActIQTPreActionFile_GetVideo(struct dth_element *elem, void *value)
{
		if (elem == NULL)
			return -1;

        return(tatlm1_20PreTreatmentActionVideo(value));
}
int Dth_ActIQTPreActionFile_GetRawStill(struct dth_element *elem, void *value)
{	
        return(tatlm1_116PreTreatmentActionRawStill(elem,value));	
}
int Dth_ActIQTPreActionFile_GetHrStill(struct dth_element *elem, void *value)
{	
        return(tatlm1_117PreTreatmentActionHrStill(elem,value));	
}
