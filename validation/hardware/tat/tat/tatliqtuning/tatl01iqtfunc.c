/*
* =====================================================================================
* 
*        Filename:  tatl01iqtfunc.c
* 
*     Description:  
* 
*         Version:  1.0
*         Created:  27/08/2010 
*        Revision:  none
*        Compiler:  
* 
*          Author: jean-michel simon 
*         � Copyright ST-Ericsson, 2010. All Rights Reserved
* 
* =====================================================================================
*/


#define TATL01IQTFUNC_C
#include"tatliqt.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mount.h>
#include "tatl01iqtfunc.h"
#undef TATL01IQTFUNC_C

static int32_t PeDthElementCounter;

//#define debug_printf(...) printf(__VA_ARGS__)
#define debug_printf(...)


//#define free(aa) {printf("[%s][ligne %d] Liberation bloc %s a %p\n",__FILE__,__LINE__,#aa,aa);free(aa);}

/*---------------------------------------------------------------------------------*
* Procedure    : tatlm1_00ActExec
*---------------------------------------------------------------------------------*
* Description  : Execute the Cmd Action sent by DTH 
*---------------------------------------------------------------------------------*
* Parameter(s) : dth_element *, the  DTH element
*---------------------------------------------------------------------------------*
* Return Value : u32, TAT error 
*---------------------------------------------------------------------------------*/
int32_t tatln1_00ActExec(struct dth_element *elem)
{
  int32_t vl_Error=TAT_ERROR_OFF;
  int32_t vl_TimeOut = 0;

  switch(elem->user_data)
  {	
  case ViewFinder:
	switch(IQT_param_array [elem->user_data])
	{
		case ViewFinder_Launch:
			switch(IQT_param_array[AcquisitionControl_CurrentState])
			{
				case acquisitionControl_CurrentState_ViewFinder:
				{
					ViewFinderDisplayActivationON=0;
					RawStillDisplayActivationON=0;
					HrStillDisplayActivationON=0;
					IqtServ_ExecutingToIdle(fd_input,fd_popen,"Trigger detected",MMTE_TIMEOUT);
					IQT_param_array[AcquisitionControl_CurrentState]=acquisitionControl_CurrentState_Idle;
					tatlm1_05InternalIqtState("IDLE");

			 		if(IQT_param_array [ViewFinder_DisplayActivation ]==ViewFinder_DisplayActivation_ON )
			  		{
						tatlm1_18Remove(ViewFinder,IQT_param_array [ViewFinder]);
			  		}
					tatlm1_113AclManagementForTheState(acquisitionControl_CurrentState_Idle);	
				}
				break;

				case acquisitionControl_CurrentState_Idle:
				{
			  		if(IQT_param_array [ViewFinder_DisplayActivation ]==ViewFinder_DisplayActivation_ON )
			  		{
						tatlm1_18Remove(ViewFinder,IQT_param_array [ViewFinder]);
			  		}
			  		tatlm1_14MmteStringManagement("FAKE_SINK_PATH_BACKUP_IMAGE",RAM_DISK_PATH_VIEWFINDER_LR);
			  		tatlm1_14MmteStringManagement("SAVING_FRAME_BY_FRAME","OMX_TRUE");
			  		tatlm1_14MmteStringManagement("RAW_ENABLE","OMX_TRUE");

			  		if(IQT_param_array [ViewFinder_DisplayActivation]==ViewFinder_DisplayActivation_ON)
						tatlm1_14MmteStringManagement("VF_DISPLAY_ACTIVATION","OMX_TRUE");
			  		else
						tatlm1_14MmteStringManagement("VF_DISPLAY_ACTIVATION","OMX_FALSE");

			  		tatlm1_15MmteValueManagement("VF_DISPLAY_RESOLUTION",IQT_param_array [ViewFinder_DisplayResolution]);

			  		if(IQT_param_array [ViewFinder_SaveActivation]==ViewFinder_SaveActivation_ON)
						tatlm1_14MmteStringManagement("VF_SAVE_ACTIVATION","OMX_TRUE");
			  		else
						tatlm1_14MmteStringManagement("VF_SAVE_ACTIVATION","OMX_FALSE");

			  		if(IQT_param_array [ViewFinder_SaveResolutionLR]==ViewFinder_SaveResolutionLR_ON )
						tatlm1_14MmteStringManagement("VF_SAVE_RESOLUTION_LR","OMX_TRUE");
			  		else
						tatlm1_14MmteStringManagement("VF_SAVE_RESOLUTION_LR","OMX_FALSE");

			  		tatlm1_15MmteValueManagement("VF_SAVE_FIRST_FRAME",IQT_param_array [ViewFinder_SaveFirstFrame]);
			  		tatlm1_15MmteValueManagement("VF_SAVE_FRAME_COUNT",IQT_param_array [ViewFinder_SaveFrameCount]);
			  		tatlm1_15MmteValueManagement("VF_SAVE_MODE",IQT_param_array [ViewFinder_SaveMode]);

			  		tatlm1_15MmteValueManagement("VF_FRAME_SKIPPING_NUMBER",IQT_param_array [ViewFinder_FrameSkippingNumber]);

			  		IqtServ_StartResume(fd_input,fd_popen,"Trigger detected",MMTE_TIMEOUT);
			  		IQT_param_array[AcquisitionControl_CurrentState]=acquisitionControl_CurrentState_ViewFinder;
			  		tatlm1_05InternalIqtState("VIEWFINDER");
			  		DisplayFrameNumber=1;
			  		if(IQT_param_array [ViewFinder_DisplayActivation ]==ViewFinder_DisplayActivation_ON )
			  		{
						ViewFinderDisplayActivationON=1;
			  		}
			  		else
			  		{
						ViewFinderDisplayActivationON=0;
			  		}
			  		tatlm1_113AclManagementForTheState(acquisitionControl_CurrentState_ViewFinder);
				}
				break;

				default:
				{
			  		SYSLOG(LOG_ERR, "You are not in the correct state.\nOnly the transitions Idle => Viewfinder or Viewfinder => Idle are authorized.");
			  		return(TAT_ERROR_MISC);
				}
				break;
			}
		break; /* ViewFinder_Launch */
		case ViewFinder_SaveToSDcard:
			tatlm1_09SaveToSDcardManagement(elem->user_data);
		break;
		case ViewFinder_LsFrame:
			tatlm1_17List(elem->user_data);
		break;
		case ViewFinder_RemoveLr:
			tatlm1_18Remove(elem->user_data,IQT_param_array [elem->user_data]);
		break;
		default:
		  SYSLOG(LOG_ERR, "Case forbidden.");
		  return(TAT_ERROR_MISC);
		break;
	}
    break;    
  case StillPicture:
	switch(IQT_param_array [elem->user_data])
	{
		case StillPicture_Launch:

			switch(IQT_param_array[AcquisitionControl_CurrentState])
			{
				case acquisitionControl_CurrentState_StillPicture:
				{
					tatlm1_118StopStillPicture();
				}				
				break;

				case acquisitionControl_CurrentState_ViewFinder:
				{
			  		if(IQT_param_array [StillPicture_DisplayActivation ]==StillPicture_DisplayActivation_ON_LR )
			  		{
						tatlm1_18Remove(StillPicture,StillPicture_RemoveLr);
			  		}
					ViewFinderDisplayActivationON=0;
/* IMGFILTER */
					tatlm1_14MmteStringManagement("IMG_FILTER_PATH_BACKUP_IMAGE",RAM_DISK_PATH_STILL_RAW);
					tatlm1_14MmteStringManagement("IMG_FILTER_PATH_STORE_IMAGE",RAM_DISK_PATH_STILL_HR);
/* IMGFILTER */
					tatlm1_14MmteStringManagement("FAKE_SINK_PATH_BACKUP_IMAGE",RAM_DISK_PATH_STILL_RAW);
					tatlm1_14MmteStringManagement("SAVING_FRAME_BY_FRAME","OMX_TRUE");
					tatlm1_14MmteStringManagement("RAW_ENABLE","OMX_TRUE");

					if(IQT_param_array [AcquisitionControl_OmxGraphUsed] == acquisitionControl_OmxGraphUsed_IQT_debug_still_display )
					  tatlm1_14MmteStringManagement("DEBUG_STILLPICTURE_DISPLAY","OMX_TRUE");
					else
					  tatlm1_14MmteStringManagement("DEBUG_STILLPICTURE_DISPLAY","OMX_FALSE");

					if(IQT_param_array [StillPicture_SaveActivation]==StillPicture_SaveActivation_ON)
						tatlm1_14MmteStringManagement("STILL_SAVE_ACTIVATION","OMX_TRUE");
					else
						tatlm1_14MmteStringManagement("STILL_SAVE_ACTIVATION","OMX_FALSE");

					switch(IQT_param_array[StillPicture_DisplayActivation])
					{
					  case StillPicture_DisplayActivation_ON_LR_RAW_HR:
					    if(IQT_param_array [StillPicture_Mode]==StillPicture_Single)
					    {
					      tatlm1_14MmteStringManagement("STILL_DISPLAY_ACTIVATION","OMX_TRUE");
					      tatlm1_14MmteStringManagement("RAW_STILL_DISPLAY_ACTIVATION","OMX_TRUE");
					      tatlm1_14MmteStringManagement("HR_STILL_DISPLAY_ACTIVATION","OMX_TRUE");
					    }
					    else
					    {

					      tatlm1_14MmteStringManagement("STILL_SAVE_ACTIVATION","OMX_FALSE");
					      IQT_param_array [StillPicture_SaveActivation]=StillPicture_SaveActivation_OFF;
 					      IQT_param_array[StillPicture_DisplayActivation]=StillPicture_DisplayActivation_ON_LR;

					      StillActionForbidden = 1;

					    }
					    break;
					  case StillPicture_DisplayActivation_ON_RAW_HR:
					    if(IQT_param_array [StillPicture_Mode]==StillPicture_Single)
					    {
					      tatlm1_14MmteStringManagement("STILL_DISPLAY_ACTIVATION","OMX_FALSE");
					      tatlm1_14MmteStringManagement("RAW_STILL_DISPLAY_ACTIVATION","OMX_TRUE");
					      tatlm1_14MmteStringManagement("HR_STILL_DISPLAY_ACTIVATION","OMX_TRUE");
					    }
					    else
					    {
					      tatlm1_14MmteStringManagement("STILL_SAVE_ACTIVATION","OMX_FALSE");
					      IQT_param_array [StillPicture_SaveActivation]=StillPicture_SaveActivation_OFF;
 					      IQT_param_array[StillPicture_DisplayActivation]=StillPicture_DisplayActivation_OFF;
					      StillActionForbidden = 1;
					    }
					    break;
					  case StillPicture_DisplayActivation_ON_LR:
					    tatlm1_14MmteStringManagement("STILL_DISPLAY_ACTIVATION","OMX_TRUE");
					    tatlm1_14MmteStringManagement("RAW_STILL_DISPLAY_ACTIVATION","OMX_FALSE");
					    tatlm1_14MmteStringManagement("HR_STILL_DISPLAY_ACTIVATION","OMX_FALSE");
					    break;
					  case StillPicture_DisplayActivation_OFF:
					    tatlm1_14MmteStringManagement("STILL_DISPLAY_ACTIVATION","OMX_FALSE");
					    tatlm1_14MmteStringManagement("RAW_STILL_DISPLAY_ACTIVATION","OMX_FALSE");
					    tatlm1_14MmteStringManagement("HR_STILL_DISPLAY_ACTIVATION","OMX_FALSE");
					    break;
					default:
					  break;
					}

					tatlm1_15MmteValueManagement("STILL_DISPLAY_RESOLUTION",IQT_param_array [StillPicture_DisplayResolution]);

					if(IQT_param_array [StillPicture_SaveResolutionLR]==StillPicture_SaveResolutionLR_ON )
						tatlm1_14MmteStringManagement("STILL_SAVE_RESOLUTION_LR","OMX_TRUE");
					else
						tatlm1_14MmteStringManagement("STILL_SAVE_RESOLUTION_LR","OMX_FALSE");

					if(IQT_param_array [StillPicture_SaveResolutionHR]==StillPicture_SaveResolutionHR_ON )
						tatlm1_14MmteStringManagement("STILL_SAVE_RESOLUTION_HR","OMX_TRUE");
					else
						tatlm1_14MmteStringManagement("STILL_SAVE_RESOLUTION_HR","OMX_FALSE");

					if(IQT_param_array [StillPicture_SaveResolutionRAW]==StillPicture_SaveResolutionRAW_ON )
						tatlm1_14MmteStringManagement("STILL_SAVE_RESOLUTION_RAW","OMX_TRUE");
					else
						tatlm1_14MmteStringManagement("STILL_SAVE_RESOLUTION_RAW","OMX_FALSE");

					tatlm1_15MmteValueManagement("STILL_SAVE_FIRST_FRAME",IQT_param_array [StillPicture_SaveFirstFrame]);
					tatlm1_15MmteValueManagement("STILL_SAVE_FRAME_COUNT",IQT_param_array [StillPicture_SaveFrameCount]);
					tatlm1_15MmteValueManagement("STILL_SAVE_MODE",IQT_param_array [StillPicture_SaveMode]);
					
 					tatlm1_15MmteValueManagement("STILL_FRAME_SKIPPING_NUMBER",IQT_param_array [StillPicture_FrameSkippingNumber]);

					IQT_param_array[AcquisitionControl_CurrentState]=acquisitionControl_CurrentState_StillPicture;
					tatlm1_05InternalIqtState("STILL");

					tatlm1_15MmteValueManagement("STILL_MODE",IQT_param_array [StillPicture_Mode]);
					tatlm1_15MmteValueManagement("STILL_BURST_FRAME_COUNT",IQT_param_array [StillPicture_BurstFrameCount]);

					switch(IQT_param_array [StillPicture_Mode])
					{
						case StillPicture_Single:					  
							vl_TimeOut=MMTE_TIMEOUT;
						break;
						case StillPicture_Burst:
							vl_TimeOut=IQT_param_array[StillPicture_BurstFrameCount]*MMTE_TIMEOUT_FOR_STILL;
							printf("Timeout for still burst = %d\n",vl_TimeOut);
						break;
						case StillPicture_Continuous:
							vl_TimeOut=MMTE_TIMEOUT;
						break;
						default:
						  SYSLOG(LOG_ERR, "Case forbidden.");
						break;
					}

					CurrentStillMode=IQT_param_array [StillPicture_Mode];
					IqtServ_StartStillBurstPictureCapture(fd_input,fd_popen,"Trigger detected",vl_TimeOut);
			  		DisplayFrameNumber=1;	

					switch(IQT_param_array[StillPicture_DisplayActivation])
					{
					  case StillPicture_DisplayActivation_ON_LR_RAW_HR:
					    RawStillDisplayActivationON=1;
					    HrStillDisplayActivationON=1;
					    StillDisplayActivationON=1;
					    break;
					  case StillPicture_DisplayActivation_ON_RAW_HR:
					    RawStillDisplayActivationON=1;
					    HrStillDisplayActivationON=1;
					    StillDisplayActivationON=0;
					    break;
					  case StillPicture_DisplayActivation_ON_LR:
					    RawStillDisplayActivationON=0;
					    HrStillDisplayActivationON=0;
					    StillDisplayActivationON=1;
					    break;
					  case StillPicture_DisplayActivation_OFF:
					    RawStillDisplayActivationON=0;
					    HrStillDisplayActivationON=0;
					    StillDisplayActivationON=0;
					    break;
					default:
					  break;
					}

			 		if(IQT_param_array [ViewFinder_DisplayActivation ]==ViewFinder_DisplayActivation_ON )
			  		{
						tatlm1_18Remove(ViewFinder,ViewFinder);
			  		}	
			  		tatlm1_113AclManagementForTheState(acquisitionControl_CurrentState_StillPicture);
				        if(StillActionForbidden)
					{ 
					  StillActionForbidden = 0;
					  tatlm1_118StopStillPicture();
					  SYSLOG(LOG_ERR, "The display of the raw bayer and HR pictures\n is available only in single mode.\nBe carefull: the SaveActivation for Still Picture has been disabled");
					  return(TAT_ERROR_MISC);
				       }		
				}				
				break;
				default:
				{
			  		SYSLOG(LOG_ERR, "You are not in the correct state.\nOnly the transitions from Viewfinder or Still Picture are authorized.");
			  		return(TAT_ERROR_MISC);					
				}
				break;
			}
		break; /* case StillPicture_Launch */
		case StillPicture_SaveToSDcard:
			tatlm1_09SaveToSDcardManagement(elem->user_data);
		break;
		case StillPicture_LsFrame:
			tatlm1_17List(elem->user_data);
		break;
		case StillPicture_RemoveLr:
			tatlm1_18Remove(elem->user_data,IQT_param_array [elem->user_data]);
		break;
		case StillPicture_RemoveHr:
			tatlm1_18Remove(elem->user_data,IQT_param_array [elem->user_data]);
		break;
		case StillPicture_RemoveRaw:
			tatlm1_18Remove(elem->user_data,IQT_param_array [elem->user_data]);
		break;
		default:
		  SYSLOG(LOG_ERR, "Case forbidden.");
		  return(TAT_ERROR_MISC);
		break;
	}
    break;
  case Video:
	switch(IQT_param_array [elem->user_data])
	{
		case Video_Launch:
			switch(IQT_param_array[AcquisitionControl_CurrentState])
			{
				case acquisitionControl_CurrentState_Video:
				{
					if(IQT_param_array [ViewFinder_DisplayActivation ]==ViewFinder_DisplayActivation_ON )
			  		{
						ViewFinderDisplayActivationON=1;
						DisplayFrameNumber=1;
						tatlm1_18Remove(ViewFinder,IQT_param_array [ViewFinder]);
					}
			  		VideoDisplayActivationON=0;

			  		if(IQT_param_array [ViewFinder_DisplayActivation]==ViewFinder_DisplayActivation_ON)
						tatlm1_14MmteStringManagement("VF_DISPLAY_ACTIVATION","OMX_TRUE");
			  		else
						tatlm1_14MmteStringManagement("VF_DISPLAY_ACTIVATION","OMX_FALSE");

			  		if(IQT_param_array [ViewFinder_SaveActivation]==ViewFinder_SaveActivation_ON)
						tatlm1_14MmteStringManagement("VF_SAVE_ACTIVATION","OMX_TRUE");
			  		else
						tatlm1_14MmteStringManagement("VF_SAVE_ACTIVATION","OMX_FALSE");

			  		if(IQT_param_array [ViewFinder_SaveResolutionLR]==ViewFinder_SaveResolutionLR_ON )
						tatlm1_14MmteStringManagement("VF_SAVE_RESOLUTION_LR","OMX_TRUE");
			  		else
						tatlm1_14MmteStringManagement("VF_SAVE_RESOLUTION_LR","OMX_FALSE");

			  		tatlm1_15MmteValueManagement("VF_SAVE_FIRST_FRAME",IQT_param_array [ViewFinder_SaveFirstFrame]);
			  		tatlm1_15MmteValueManagement("VF_SAVE_FRAME_COUNT",IQT_param_array [ViewFinder_SaveFrameCount]);
			  		tatlm1_15MmteValueManagement("VF_SAVE_MODE",IQT_param_array [ViewFinder_SaveMode]);

			  		IqtServ_StopRecord(fd_input,fd_popen,"Trigger detected",MMTE_TIMEOUT);

			  		IQT_param_array[AcquisitionControl_CurrentState]=acquisitionControl_CurrentState_ViewFinder;
					tatlm1_117WorkAround();

			 	tatlm1_15MmteValueManagement("VF_FRAME_SKIPPING_NUMBER",IQT_param_array [ViewFinder_FrameSkippingNumber]);
			 	tatlm1_05InternalIqtState("VIEWFINDER");
			 	if(IQT_param_array [Video_DisplayActivation ]==Video_DisplayActivation_ON )
			 	{
					tatlm1_18Remove(Video,Video_RemoveLr);
			 	}
			 	tatlm1_113AclManagementForTheState(acquisitionControl_CurrentState_ViewFinder);

				}
				break;
				case acquisitionControl_CurrentState_ViewFinder:
				{

			  		if(IQT_param_array [Video_DisplayActivation ]==Video_DisplayActivation_ON )
			  		{
						tatlm1_18Remove(Video,Video_RemoveLr);
			  		}
			  		ViewFinderDisplayActivationON=0;
					RawStillDisplayActivationON=0;
					HrStillDisplayActivationON=0;
			  		tatlm1_14MmteStringManagement("FAKE_SINK_PATH_BACKUP_IMAGE",RAM_DISK_PATH_VIDEO_LR);
			  		tatlm1_14MmteStringManagement("SAVING_FRAME_BY_FRAME","OMX_TRUE");
			  		tatlm1_14MmteStringManagement("RAW_ENABLE","OMX_TRUE");

			  		if(IQT_param_array [Video_DisplayActivation ]==Video_DisplayActivation_ON )
			  		{
						tatlm1_14MmteStringManagement("VIDEO_DISPLAY_ACTIVATION","OMX_TRUE");
			  		}
			  		else
			  		{
						tatlm1_14MmteStringManagement("VIDEO_DISPLAY_ACTIVATION","OMX_FALSE");
			  		}

			  		tatlm1_15MmteValueManagement("VIDEO_DISPLAY_RESOLUTION",IQT_param_array [Video_DisplayResolution]);

			  		if(IQT_param_array [Video_HREncodedSaveActivation]==Video_HREncodedSaveActivation_ON)
						tatlm1_14MmteStringManagement("VIDEO_ENCODED_SAVE_ACTIVATION","OMX_TRUE");
			  		else
						tatlm1_14MmteStringManagement("VIDEO_ENCODED_SAVE_ACTIVATION","OMX_FALSE");

			  		if(IQT_param_array [Video_SaveActivation]==Video_SaveActivation_ON)
						tatlm1_14MmteStringManagement("VIDEO_SAVE_ACTIVATION","OMX_TRUE");
			  		else
						tatlm1_14MmteStringManagement("VIDEO_SAVE_ACTIVATION","OMX_FALSE");

			  		if(IQT_param_array [Video_SaveResolutionLR]==Video_SaveResolutionLR_ON )
						tatlm1_14MmteStringManagement("VIDEO_SAVE_RESOLUTION_LR","OMX_TRUE");
			  		else
						tatlm1_14MmteStringManagement("VIDEO_SAVE_RESOLUTION_LR","OMX_FALSE");

			  		if(IQT_param_array [Video_SaveResolutionHR]==Video_SaveResolutionHR_ON )
						tatlm1_14MmteStringManagement("VIDEO_SAVE_RESOLUTION_HR","OMX_TRUE");
			  		else
						tatlm1_14MmteStringManagement("VIDEO_SAVE_RESOLUTION_HR","OMX_FALSE");

			  		tatlm1_15MmteValueManagement("VIDEO_SAVE_FIRST_FRAME",IQT_param_array [Video_SaveFirstFrame]);
			  		tatlm1_15MmteValueManagement("VIDEO_SAVE_FRAME_COUNT",IQT_param_array [Video_SaveFrameCount ]);
			  		tatlm1_15MmteValueManagement("VIDEO_SAVE_MODE",IQT_param_array [Video_SaveMode]);

			  		tatlm1_15MmteValueManagement("VIDEO_FRAME_SKIPPING_NUMBER",IQT_param_array [Video_FrameSkippingNumber]);

			  		IQT_param_array[AcquisitionControl_CurrentState]=acquisitionControl_CurrentState_Video ;
			  		tatlm1_05InternalIqtState("VIDEO");
			  		IqtServ_StartRecord(fd_input,fd_popen,"Trigger detected",MMTE_TIMEOUT);
			  		DisplayFrameNumber=1;
			  		if(IQT_param_array [Video_DisplayActivation ]==Video_DisplayActivation_ON )
			  		{
						VideoDisplayActivationON=1;
			  		}
			  		else
			  		{
						VideoDisplayActivationON=0;
			  		}	
			  		if(IQT_param_array [ViewFinder_DisplayActivation ]==ViewFinder_DisplayActivation_ON )
			  		{
						tatlm1_18Remove(ViewFinder,ViewFinder);
			  		}	
			  		tatlm1_113AclManagementForTheState(acquisitionControl_CurrentState_Video);	  
				}
				break;
				default:
				{
			  		SYSLOG(LOG_ERR, "You are not in the correct state.\nOnly the transitions from Viewfinder or VIDEO are authorized.");		
			  		return(TAT_ERROR_MISC);			
				}
				break;
			}
		break;/* Video_Launch */
		case Video_SaveToSDcard:
			tatlm1_09SaveToSDcardManagement(elem->user_data);
		break;
		case Video_LsFrame:
			tatlm1_17List(elem->user_data);
		break;
		case Video_RemoveLr:
			tatlm1_18Remove(elem->user_data,IQT_param_array [elem->user_data]);
		break; 
		case Video_RemoveHr:
			tatlm1_18Remove(elem->user_data,IQT_param_array [elem->user_data]);
		break;
		default:
		  SYSLOG(LOG_ERR, "Case forbidden.");
		  return(TAT_ERROR_MISC);
		break;
	}
    break;    
  case AcquisitionControl:
    switch(IQT_param_array[AcquisitionControl])
    {  
    case AcquisitionControl_About:
     SYSLOG(LOG_ERR, "Tool built thanks to the contribution from:\nJean-Michel Hautbois\nEmmanuel Grandin\nAlain Crouzet\nVincent Delaunay\nBertrand Colau\nMatthieu Laurent\nand\nJean-Michel Simon.\n");
      return(TAT_ERROR_MISC);
    break;
    case AcquisitionControl_GoToIdle:
      switch(IQT_param_array[AcquisitionControl_CurrentState])
      {  
	case acquisitionControl_CurrentState_Init:
	{ 

	  tatlm1_116StartInitToIdle();

	  if(PeDthElementCounter==0)
	  {
	  	tatlm1_108register_pe_dth_struct(MAP_FILE);
	        tatlm1_114DthPeInitAclManagement(ACTION_DISABLED,INIT_PE_DTH_PATHNAME);
		/* Send an asynchrounous event for the update of the IQ tuning register panel */
	  }

	  tatlm1_113AclManagementForTheState(acquisitionControl_CurrentState_Idle);
	  break;
	}
	case acquisitionControl_CurrentState_ViewFinder:
	{
	  ViewFinderDisplayActivationON=0;
	  IqtServ_ExecutingToIdle(fd_input,fd_popen,"Trigger detected",MMTE_TIMEOUT);	
	  IQT_param_array[AcquisitionControl_CurrentState]=acquisitionControl_CurrentState_Idle;
	  tatlm1_05InternalIqtState("IDLE");	

	  if(IQT_param_array [ViewFinder_DisplayActivation ]==ViewFinder_DisplayActivation_ON )
	  {
	  	tatlm1_18Remove(ViewFinder,IQT_param_array [ViewFinder]);
	  }
	  tatlm1_113AclManagementForTheState(acquisitionControl_CurrentState_Idle);
	  break;
	}
	case acquisitionControl_CurrentState_Idle:
	{
	  char command[100];

	  umount("/iqt_rd");
	  IqtServ_IdleToInit(fd_input,fd_popen,"Trigger detected",MMTE_TIMEOUT);	
	  IqtServ_QuitMmte(fd_input,fd_popen,"Command executed",MMTE_TIMEOUT);

	  memset(command,0,sizeof(command));
	  sprintf(command,"umount %s\n",DEFAULT_SDCARD_DIRECTORY_PATH);
	  printf(command);
	  system(command);

	  sleep(1);
	  IqtServ_CloseFileDescriptors();
	  IQT_param_array[AcquisitionControl_CurrentState]=acquisitionControl_CurrentState_Init;
	  tatlm1_113AclManagementForTheState(acquisitionControl_CurrentState_Init);

	  /* To do: Remove the dth tree for page element if it exists.It is for the FIDO 319105 */
	  if(PeDthElementCounter!=0)
	  {
	    /* Remove the page element dth tree  */
	    /* Send an asynchrounous event for the update of the IQ tuning register panel */
	  }
	  break;	
	}
	default:
      	  SYSLOG(LOG_ERR, "Transition forbidden.");
          return(TAT_ERROR_MISC);
	break;
      }
    break; 
    case AcquisitionControl_Pause:
	switch(IQT_param_array[AcquisitionControl_CurrentState])
	{
	case acquisitionControl_CurrentState_ViewFinder:
	case acquisitionControl_CurrentState_Video:
	{
	  IqtServ_StartPause(fd_input,fd_popen,"Trigger detected",MMTE_TIMEOUT);
	  v_backup_IqtState=IQT_param_array[AcquisitionControl_CurrentState];
	  IQT_param_array[AcquisitionControl_CurrentState]=acquisitionControl_CurrentState_Pause;
	  tatlm1_05InternalIqtState("PAUSE");
	  tatlm1_113AclManagementForTheState(acquisitionControl_CurrentState_Pause);
	break;
	}
	case acquisitionControl_CurrentState_StillPicture:
	{
	  SYSLOG(LOG_ERR, "Transition Still Picture => Pause forbidden.");
	  return(TAT_ERROR_MISC);
	break;
	}
	case acquisitionControl_CurrentState_Pause:
	{
	  SYSLOG(LOG_ERR, "Transition Pause => Pause forbidden.");
	  return(TAT_ERROR_MISC);
	break;
	}
	case acquisitionControl_CurrentState_Idle:
	{
	  SYSLOG(LOG_ERR, "Transition Idle => Pause forbidden.");
          return(TAT_ERROR_MISC);
	break;
	}
	default:
	  SYSLOG(LOG_ERR, "Transition Unknown forbidden.");
          return(TAT_ERROR_MISC);
	break;
	}
    break;
    case AcquisitionControl_Resume:
	switch(v_backup_IqtState)
	{  
	  case acquisitionControl_CurrentState_ViewFinder:
	  {
	    v_backup_IqtState=acquisitionControl_CurrentState_Init;
	    IqtServ_StartResume(fd_input,fd_popen,"Trigger detected",MMTE_TIMEOUT);
	    IQT_param_array[AcquisitionControl_CurrentState]=acquisitionControl_CurrentState_ViewFinder;
	    tatlm1_05InternalIqtState("VIEWFINDER");
	    tatlm1_113AclManagementForTheState(acquisitionControl_CurrentState_ViewFinder);
	    break;
	  }
	  case acquisitionControl_CurrentState_Video:
	  {
	    v_backup_IqtState=acquisitionControl_CurrentState_Init;
	    IqtServ_StartResume(fd_input,fd_popen,"Trigger detected",MMTE_TIMEOUT);
	    IQT_param_array[AcquisitionControl_CurrentState]=acquisitionControl_CurrentState_Video ;
	    tatlm1_05InternalIqtState("VIDEO");
	    tatlm1_113AclManagementForTheState(acquisitionControl_CurrentState_Video);
	    break;
	  } 
  	  default: 
      	    SYSLOG(LOG_ERR, "Transition Executing => Executing forbidden.");
            return(TAT_ERROR_MISC);
    	  break;
	}
    break;
    case AcquisitionControl_PageElementDth:
	  if(PeDthElementCounter==0)	
	  	tatlm1_108register_pe_dth_struct(MAP_FILE);
    break;
    case AcquisitionControl_MmteTrace:
      {
	char command[100];

    	sprintf(command,"cat /tmp/trace_mmte.txt\n");
	printf(command);
	system(command);
      }
    break;
    case AcquisitionControl_GetCurrentState:
      {
	int vl_State;

	IqtServ_ReadCurrentState(fd_input,fd_popen,MMTE_TIMEOUT,&vl_State);
      }
    break;
    case AcquisitionControl_ResolutionUpdate:
      {
	tatlm1_119ActSetResolutionVF(IQT_param_array [ViewFinder_Resolution]);
	tatlm1_120ActSetResolutionStill(IQT_param_array [StillPicture_Resolution]);
	tatlm1_121ActSetResolutionVideo(IQT_param_array [Video_Resolution]);
      }
    break;
    case AcquisitionControl_Reboot:
      {
	system("rm -f /tmp/reboot.sh");
	system("echo \"sleep 4\" > /tmp/reboot.sh");
	system("echo \"reboot\" >> /tmp/reboot.sh");
	system("chmod 777 /tmp/reboot.sh&");
	printf("Reboot of the target in 4s\n");
	system("/tmp/reboot.sh&");
      }
      break;
    case AcquisitionControl_MemoryAvailable:
      {
	char command[100];

	sprintf(command,"df /iqt_rd&\n");
	printf(command);
	system(command);
      }
    break;

    default:
      SYSLOG(LOG_ERR, "Case forbidden.");
      return(TAT_ERROR_MISC);
    break;
    }
  break;
  default:
    SYSLOG(LOG_ERR, "Case forbidden.");
    return(TAT_ERROR_MISC);
  break;
  }
  if(vl_Error!=TAT_ERROR_OFF)
  {
  printf ("tatlm1_00ActExec  user data:%d  vl_Error:%d\n",elem->user_data,vl_Error);
  }
  return vl_Error ;
}	


/*---------------------------------------------------------------------------------*
* Procedure    : tatlm1_02ActSet
*---------------------------------------------------------------------------------*
* Description  : Set the DTH Element Value Input Parameters 
*---------------------------------------------------------------------------------*
* Parameter(s) : dth_element *, Structure of DTH element
*		  void *, Value of Dth Element						   
*---------------------------------------------------------------------------------*
* Return Value : u32, TAT error
*---------------------------------------------------------------------------------*/
int32_t tatln1_02ActSet (struct dth_element *elem, void *Value)
{

  int32_t vl_Error=TAT_ERROR_OFF;    	
  switch (elem->type){

  case DTH_TYPE_U8:
    IQT_param_array [elem->user_data]= *(uint8_t*)Value;
    break;
  case DTH_TYPE_S8:
    IQT_param_array [elem->user_data]= *(int8_t*)Value;
    break;
  case DTH_TYPE_U16:
    IQT_param_array [elem->user_data]= *(uint16_t*)Value;
    break;
  case DTH_TYPE_S16:
    IQT_param_array [elem->user_data]= *(int16_t*)Value;
    break;
  case DTH_TYPE_U32:
    IQT_param_array [elem->user_data]= *(uint32_t*)Value;
    break;
  case DTH_TYPE_S32:
    IQT_param_array [elem->user_data]= *(int32_t*)Value;
    break;	
  case DTH_TYPE_U64:
    IQT_param_array [elem->user_data]= *(uint32_t*)Value;
    break;
  case DTH_TYPE_S64:
    IQT_param_array [elem->user_data]= *(int32_t*)Value;
    break;
  case DTH_TYPE_STRING:
    if ((int8_t*)Value!=NULL)
    {
      uint32_t len =strlen((const char*)Value);
      if (IQT_string_array[elem->user_data]!=NULL)
      { 
        free(IQT_string_array[elem->user_data]);
      }
      IQT_string_array[elem->user_data]=(int8_t*)calloc(len+1,sizeof(int8_t));
      strncpy((char*)IQT_string_array[elem->user_data],(const char*)Value,len);
    }
    else
    {
      printf (" tatl1_02ActADCParam_Set (int8_t*)Value) ==NULL  \n");	 	
    }
    break;		
  default:
    SYSLOG(LOG_ERR, "tatl1_02ActVIDEOParam_Set problem with TYPE.");
    vl_Error=TAT_ERROR_CASE;
    break;	
  }		
  return vl_Error ;
}	


/*---------------------------------------------------------------------------------*
* Procedure    : tatlm1_03ActGet
*---------------------------------------------------------------------------------*
* Description  : Get the DTH ELement Value Output Parameters 
*---------------------------------------------------------------------------------*
* Parameter(s) : dth_element *, Structure of DTH element
*		  void *, Value of Dth Element						   
*---------------------------------------------------------------------------------*
* Return Value : u32, TAT error
*---------------------------------------------------------------------------------*/
int32_t tatln1_03ActGet (struct dth_element *elem, void *Value)
{
  int32_t vl_Error=TAT_ERROR_OFF;

  switch (elem->type){

  case DTH_TYPE_U8:
    *((uint8_t*)Value)= IQT_param_array [elem->user_data];
    break;
  case DTH_TYPE_S8:
    *((int8_t*)Value)= IQT_param_array [elem->user_data];
    break;
  case DTH_TYPE_U16:
    *((uint16_t*)Value)= IQT_param_array [elem->user_data];
    break;
  case DTH_TYPE_S16:
    *((int16_t*)Value)= IQT_param_array [elem->user_data];
    break;
  case DTH_TYPE_U32:
    *((uint32_t*)Value)= IQT_param_array [elem->user_data];
    break;
  case DTH_TYPE_S32:
    *((int32_t*)Value)= IQT_param_array [elem->user_data];
    break;	
  case DTH_TYPE_U64:
    *((uint64_t*)Value)= IQT_param_array [elem->user_data];
    break;
  case DTH_TYPE_S64:
    *((int64_t*)Value)= IQT_param_array [elem->user_data];
    break;
  case DTH_TYPE_STRING:
    if (IQT_string_array[elem->user_data]!=NULL)
    {
      strncpy((char*)Value,(const char*)IQT_string_array[elem->user_data],strlen((char*)IQT_string_array[elem->user_data]));
    }
    else 
    {
      strncpy((char*)Value,"NULL",5);			
    }
    break;		
  default:
    SYSLOG(LOG_ERR, "tatl1_03ActVIDEOParam_Get TYPE not available.");
    vl_Error=TAT_ERROR_CASE; 
    break;	
  }  
  return vl_Error ;

}	

/*---------------------------------------------------------------------------------*
* Procedure    : tatlm1_04ActSetDynamic
*---------------------------------------------------------------------------------*
* Description  : Set the DTH Element Value Input Parameters and Execute Action
*---------------------------------------------------------------------------------*
* Parameter(s) : dth_element *, Structure of DTH element
*		  void *, Value of Dth Element	
*---------------------------------------------------------------------------------*
* Return Value : u32, TAT error
*---------------------------------------------------------------------------------*/
int32_t tatln1_04ActSetDynamic (struct dth_element *elem, void *Value)
{
  int32_t vl_Error=TAT_ERROR_OFF;
  	

  switch (elem->type){

  case DTH_TYPE_U8:
    IQT_param_array [elem->user_data]= *(uint8_t*)Value;
    break;
  case DTH_TYPE_S8:
    IQT_param_array [elem->user_data]= *(int8_t*)Value;
    break;
  case DTH_TYPE_U16:
    IQT_param_array [elem->user_data]= *(uint16_t*)Value;
    break;
  case DTH_TYPE_S16:
    IQT_param_array [elem->user_data]= *(int16_t*)Value;
    break;
  case DTH_TYPE_U32:
    IQT_param_array [elem->user_data]= *(uint32_t*)Value;
    break;
  case DTH_TYPE_S32:
    IQT_param_array [elem->user_data]= *(int32_t*)Value;
    break;	
  case DTH_TYPE_U64:
    IQT_param_array [elem->user_data]= *(uint32_t*)Value;
    break;
  case DTH_TYPE_S64:
    IQT_param_array [elem->user_data]= *(int32_t*)Value;
    break;
  case DTH_TYPE_STRING:
    if ((int8_t*)Value!=NULL)
    {
      uint32_t len =strlen((const char*)Value);
      if (IQT_string_array[elem->user_data]!=NULL)
      { 
        free(IQT_string_array[elem->user_data]);
      }
      IQT_string_array[elem->user_data]=(int8_t*)calloc(len+1,sizeof(int8_t));
      strncpy((char*)IQT_string_array[elem->user_data],(const char*)Value,len);
    }
    else
    {
      printf (" tatlm1_04ActSetDynamic (int8_t*)Value) ==NULL  \n");	 	
    }
    break;		
  default:
    SYSLOG(LOG_ERR, "tatlm1_04ActSetDynamic problem with TYPE.");
    vl_Error=TAT_ERROR_CASE;
    break;
  }
  return vl_Error ;
}

int32_t tatlm1_05InternalIqtState(char *IqtState)
{
uint32_t vl_error=TAT_ERROR_OFF; 

	vl_error=tatlm1_14MmteStringManagement("IQT_CURRENT_STATE",IqtState);
   return vl_error; 
}

int32_t tatlm1_06MkdirManagement(char *pathname)
{
uint32_t vl_error=TAT_ERROR_OFF; 
	errno = 0;
	printf("mkdir %s\n",pathname);
	if(mkdir(pathname,0777)==0)
	{
	printf("mkdir operation successfull\n");
	} 
	else
	{
	if(errno == EEXIST) printf("this file (%s) exists already \n",pathname);		
	else printf("Error during mkdir operation:%d\n",errno);
	}

   return vl_error; 
}

int32_t tatlm1_07RamDiskManagement(void)
{
uint32_t vl_error=TAT_ERROR_OFF; 
//int32_t v_RamDiskSize=64;
char option[100];

  errno = 0;

  vl_error=tatlm1_06MkdirManagement("/iqt_rd");
  memset(option,0,sizeof(option));
  sprintf(option,"size=%dm",IQT_param_array [AcquisitionControl_RamDiskSize]);
  /*example: mount -t tmpfs -o size=64m tmpfs /iqt_rd */
  if(mount("tmpfs","/iqt_rd","tmpfs",MS_MGC_VAL,option)==0)
  {
    printf("mount operation successfull: Ram Disk %s mounted\n","/iqt_rd");
  } 
  else
  {
    if(errno == EBUSY) printf("Source %s already mounted \n","/iqt_rd");		
    else printf("Error during mount operation:%d\n",errno);
  }

  vl_error=tatlm1_13RamDiskTreeManagement(RAM_DISK_PATH_VIEWFINDER_LR);
  vl_error=tatlm1_13RamDiskTreeManagement(RAM_DISK_PATH_STILL_LR);
  vl_error=tatlm1_13RamDiskTreeManagement(RAM_DISK_PATH_STILL_HR);
  vl_error=tatlm1_13RamDiskTreeManagement(RAM_DISK_PATH_STILL_RAW);
  vl_error=tatlm1_13RamDiskTreeManagement(RAM_DISK_PATH_VIDEO_LR);
  vl_error=tatlm1_13RamDiskTreeManagement(RAM_DISK_PATH_VIDEO_HR);
  return vl_error;  	
}

int32_t tatlm1_08SavePrefixManagement(void)
{
uint32_t vl_error=TAT_ERROR_OFF; 
static char *PrefixPathname="savedframes/test20101105_";

  IQT_string_array[ViewFinder_SavePrefix]=(int8_t*)malloc(strlen(PrefixPathname)+1);
  strcpy((char*)IQT_string_array[ViewFinder_SavePrefix],PrefixPathname);
  IQT_string_array[StillPicture_SavePrefix]=(int8_t*)malloc(strlen(PrefixPathname)+1);
  strcpy((char*)IQT_string_array[StillPicture_SavePrefix],PrefixPathname);
  IQT_string_array[Video_SavePrefix]=(int8_t*)malloc(strlen(PrefixPathname)+1);
  strcpy((char*)IQT_string_array[Video_SavePrefix],PrefixPathname);
  return vl_error; 
}

int32_t tatlm1_10MkdirPrefixManagement(int32_t UseCase)
{
uint32_t vl_error=TAT_ERROR_OFF; 
char *SavePrefix=NULL;
char *SDcardDirectory=NULL;
char pathname[100];
uint32_t i=0;

  switch(UseCase)
  {
  case ViewFinder:
    SavePrefix=(char*)IQT_string_array[ViewFinder_SavePrefix];
    SDcardDirectory=(char*)IQT_string_array[ViewFinder_SDcardDirectory];
    break;
  case StillPicture:
    SavePrefix=(char*)IQT_string_array[StillPicture_SavePrefix];
    SDcardDirectory=(char*)IQT_string_array[StillPicture_SDcardDirectory];
    break;
  case Video:
    SavePrefix=(char*)IQT_string_array[Video_SavePrefix];
    SDcardDirectory=(char*)IQT_string_array[Video_SDcardDirectory];
    break;
  default:
    SYSLOG(LOG_ERR, "Case forbidden.");
    return(TAT_ERROR_MISC);
    break;
  }
  if(SavePrefix==NULL)
  {
    vl_error=TAT_MISC_ERR;
    printf("SavePrefix=NULL\n");
    return vl_error; 
  }
  if(SDcardDirectory==NULL)
  {
    vl_error=TAT_MISC_ERR;
    printf("SDcardDirectory=NULL\n");
    return vl_error; 
  }
  while(SavePrefix[i]!='\0')
  {
    if(SavePrefix[i]=='/')
    {
      strcpy(pathname,"");
      strncat(pathname,SDcardDirectory,strlen(SDcardDirectory));
      strncat(pathname,SavePrefix,i);
      vl_error=tatlm1_06MkdirManagement(pathname);
    }
    i++;
  }
  return vl_error; 
}

int32_t tatlm1_11SDcardDirectoryManagement(char *pathname,int32_t UseCase)
{
uint32_t vl_error=TAT_ERROR_OFF; 
char command[200];
char Resolution[10];
FILE *FrameList;
char frame_name[20];
char frame_index[20];

  sprintf(command,"ls %s |grep f >/tmp/frame_list",pathname);
  system(command);
  FrameList=fopen("/tmp/frame_list","r");
  memset(frame_index,0,sizeof(frame_index));
  while(fgets(frame_name,1024,FrameList)!=NULL)
  {	
    memset(frame_index,0,sizeof(frame_name));
    strncpy(frame_index,&frame_name[1],strlen(frame_name)-2);
    memset(command,0,sizeof(command));
    sprintf(command,"cp %s",pathname);
    strncat(command,frame_name,strlen(frame_name)-1);
    strcat(command," ");
    switch(UseCase)
    {
    case ViewFinder:
      strncat(command,(char*)IQT_string_array[ViewFinder_SDcardDirectory],strlen((char*)IQT_string_array[ViewFinder_SDcardDirectory]));
      strncat(command,(char*)IQT_string_array[ViewFinder_SavePrefix],strlen((char*)IQT_string_array[ViewFinder_SavePrefix]));
      break;
    case StillPicture:
      strncat(command,(char*)IQT_string_array[StillPicture_SDcardDirectory],strlen((char*)IQT_string_array[StillPicture_SDcardDirectory]));
      strncat(command,(char*)IQT_string_array[StillPicture_SavePrefix],strlen((char*)IQT_string_array[StillPicture_SavePrefix]));
      break;
    case Video:
      strncat(command,(char*)IQT_string_array[Video_SDcardDirectory],strlen((char*)IQT_string_array[Video_SDcardDirectory]));
      strncat(command,(char*)IQT_string_array[Video_SavePrefix],strlen((char*)IQT_string_array[Video_SavePrefix]));
      break;
    default:
      SYSLOG(LOG_ERR, "Case forbidden.");
      return(TAT_ERROR_MISC);
      break;
    }
    memset(Resolution,0,sizeof(Resolution));
    vl_error=tatlm1_12ResolutionPrefixManagement(pathname,Resolution);
    strcat(command,Resolution);
    strncat(command,frame_index,strlen(frame_index));
    printf("%s\n",command);
    system(command);
  }
  fclose(FrameList);
  system("rm /tmp/frame_list");
  return vl_error; 
}

int32_t tatlm1_09SaveToSDcardManagement(int32_t UseCase)
{
uint32_t vl_error=TAT_ERROR_OFF; 

  tatlm1_10MkdirPrefixManagement(UseCase);
  switch(UseCase)
  {
  case ViewFinder:
    vl_error=tatlm1_11SDcardDirectoryManagement(RAM_DISK_PATH_VIEWFINDER_LR,UseCase);
    break;
  case StillPicture:
    vl_error=tatlm1_11SDcardDirectoryManagement(RAM_DISK_PATH_STILL_LR,UseCase);
    vl_error=tatlm1_11SDcardDirectoryManagement(RAM_DISK_PATH_STILL_HR,UseCase);
    vl_error=tatlm1_11SDcardDirectoryManagement(RAM_DISK_PATH_STILL_RAW,UseCase);
    break;
  case Video:
    vl_error=tatlm1_11SDcardDirectoryManagement(RAM_DISK_PATH_VIDEO_LR,UseCase);
    vl_error=tatlm1_11SDcardDirectoryManagement(RAM_DISK_PATH_VIDEO_HR,UseCase);
    break;
  }
  return vl_error; 
}

int32_t tatlm1_12ResolutionPrefixManagement(char *pathname,char *Resolution)
{
uint32_t vl_error=TAT_MISC_ERR; 
uint32_t length=0;

  length=strlen(pathname); 
  if(strncmp(&pathname[length-4],"/LR/",4)==0 || strncmp(&pathname[length-4],"/lr/",4)==0 || strncmp(&pathname[length-4],"/Lr/",4)==0)
  {
    strncpy(Resolution,&pathname[length-3],2);
    vl_error=TAT_ERROR_OFF;
  }
  if(strncmp(&pathname[length-4],"/HR/",4)==0 || strncmp(&pathname[length-4],"/hr/",4)==0 || strncmp(&pathname[length-4],"/Hr/",4)==0)
  {
    strncpy(Resolution,&pathname[length-3],2);
    vl_error=TAT_ERROR_OFF;
  }
  if(strncmp(&pathname[length-5],"/RAW/",5)==0 || strncmp(&pathname[length-5],"/raw/",5)==0 || strncmp(&pathname[length-5],"/Raw/",5)==0)
  {
    strncpy(Resolution,&pathname[length-4],3);
    vl_error=TAT_ERROR_OFF;
  }
  return vl_error; 
}
 
int32_t tatlm1_13RamDiskTreeManagement(char *Pathname)
{
uint32_t vl_error=TAT_ERROR_OFF; 
char Directory[100];
uint32_t i=1;

 while(Pathname[i]!='\0')
 {
   if(Pathname[i]=='/')
   {
     strcpy(Directory,"");
     strncat(Directory,Pathname,i);
     vl_error=tatlm1_06MkdirManagement(Directory);
   }
   i++;
 }
 return vl_error; 
}

int32_t tatlm1_14MmteStringManagement(char *alias,char *status)
{
int32_t vl_Error=TAT_ERROR_OFF;
char command[100];

  sprintf(command,"alias %s %s\n",alias,status);	
  IqtServ_SetMmteAlias(fd_input,fd_popen,command,MMTE_TIMEOUT);
  return vl_Error ;
}

int32_t tatlm1_15MmteValueManagement(char *alias,int value)
{
int32_t vl_Error=TAT_ERROR_OFF;
char command[100];

  sprintf(command,"alias %s %d\n",alias,value);
  IqtServ_SetMmteAlias(fd_input,fd_popen,command,MMTE_TIMEOUT);	
  return vl_Error ;
} 

int32_t tatlm1_16PreTreatmentActionVF(void *Value)
{
int32_t vl_Error=TAT_ERROR_OFF;
struct dth_file_data *myFile;
uint32_t i=0;
uint32_t timeout=100;
char Path[100];
char PathName[100];
char command[100];
FILE *fd;

  if( (ViewFinderDisplayActivationON==1) && (IQT_param_array[AcquisitionControl_CurrentState]!=acquisitionControl_CurrentState_Pause))
  {	
    printf("PreTreatment\n");
    myFile=(struct dth_file_data *)Value;
    printf("pathname=%s\n",myFile->pathname);
    printf("offset=%d\n",myFile->offset);
    printf("count=%d\n",myFile->count);

    i=strlen(myFile->pathname)-1;
    while(i!=0)
    {
      if(myFile->pathname[i]=='/')
      {
	break;	
      }
      i--;
    } 
    memset(Path,0,sizeof(Path));
    strncpy(Path,myFile->pathname,i);
    printf("path=%s\n",Path);
    memset(PathName,0,sizeof(PathName));

    sprintf(PathName,"%s/f%03d",Path,DisplayFrameNumber);
    while(((fd=fopen(PathName,"r"))==NULL) && timeout)
    {
      printf("wait=%s\n",PathName);
      usleep(1000);
      timeout--;
    }
    if(!timeout)
    {
      printf("timeout\n");
    }
    else
      fclose(fd);

    memset(command,0,sizeof(command));
    sprintf(command,"mv %s/f%03d %s/current",Path,DisplayFrameNumber++,Path);
    printf("%s\n",command);
    system(command);

    tatlm1_110FrameSkipping(Path);
  }
  else if( (ViewFinderDisplayActivationON==1) &&(IQT_param_array[AcquisitionControl_CurrentState]==acquisitionControl_CurrentState_Pause))
  {
    /* do nothing */
    /* The last current frame is always read */
  }
  else
  {
    /* mandatory to avoid cyclic error when the streaming control panel is opened */
    /* because this function is cyclicly called when the streaming control panel is opened !!!!*/
    vl_Error=EACCES;
  }
  return vl_Error ;
}

int32_t tatlm1_17List(int32_t UseCase)
{
int32_t vl_error=TAT_ERROR_OFF;
char command[100];

	
  switch(UseCase)
  {
    case ViewFinder:
      memset(command,0,sizeof(command));
      sprintf(command,"ls -al %s",RAM_DISK_PATH_VIEWFINDER_LR );
      printf("%s\n",command);
      system(command);
      break;
  case StillPicture:
    memset(command,0,sizeof(command));
    sprintf(command,"ls -al %s",RAM_DISK_PATH_STILL_LR);
    printf("%s\n",command);
    system(command);

    memset(command,0,sizeof(command));
    sprintf(command,"ls -al %s",RAM_DISK_PATH_STILL_HR);
    printf("%s\n",command);
    system(command);

    memset(command,0,sizeof(command));
    sprintf(command,"ls -al %s",RAM_DISK_PATH_STILL_RAW);
    printf("%s\n",command);
    system(command);

    break;
  case Video:
    memset(command,0,sizeof(command));
    sprintf(command,"ls -al %s",RAM_DISK_PATH_VIDEO_LR);
    printf("%s\n",command);
    system(command);

    memset(command,0,sizeof(command));
    sprintf(command,"ls -al %s",RAM_DISK_PATH_VIDEO_HR);
    printf("%s\n",command);
    system(command);

    break;
  }
  return vl_error; 
}

int32_t tatlm1_18Remove(int32_t UseCase,int32_t Resolution)
{
int32_t vl_error=TAT_ERROR_OFF;
char command[100];
	 
 switch(UseCase)
 { 
   case ViewFinder:
     memset(command,0,sizeof(command));
     sprintf(command,"rm %s*",RAM_DISK_PATH_VIEWFINDER_LR );
     system(command);
     printf("%s\n",command);
     break;
 case StillPicture:
   memset(command,0,sizeof(command));
   if(Resolution==StillPicture_RemoveLr)
     sprintf(command,"rm %s*",RAM_DISK_PATH_STILL_LR);
   if(Resolution==StillPicture_RemoveHr)
     sprintf(command,"rm %s*",RAM_DISK_PATH_STILL_HR);
   if(Resolution==StillPicture_RemoveRaw)
     sprintf(command,"rm %s*",RAM_DISK_PATH_STILL_RAW);
   system(command);
   printf("%s\n",command);
   break;
 case Video:
   memset(command,0,sizeof(command));
   if(Resolution==Video_RemoveLr)
     sprintf(command,"rm %s*",RAM_DISK_PATH_VIDEO_LR);
   if(Resolution==Video_RemoveHr)
     sprintf(command,"rm %s*",RAM_DISK_PATH_VIDEO_HR);
   system(command);
   printf("%s\n",command);
   break;
 }
 return vl_error; 
}

int32_t tatlm1_19PreTreatmentActionStill(void *Value)
{
int32_t vl_Error=TAT_ERROR_OFF;
struct dth_file_data *myFile;
uint32_t i=0;
uint32_t timeout=100;
char Path[100];
char PathName[100];
char command[100];
FILE *fd;
 
  if( (StillDisplayActivationON==1) && (IQT_param_array[AcquisitionControl_CurrentState]!=acquisitionControl_CurrentState_Pause))
  {	
    printf("PreTreatment\n");
    myFile=(struct dth_file_data *)Value;
    printf("pathname=%s\n",myFile->pathname);
    printf("offset=%d\n",myFile->offset);
    printf("count=%d\n",myFile->count);

    i=strlen(myFile->pathname)-1;
    while(i!=0)
    {
      if(myFile->pathname[i]=='/')
	{
	  break;	
	}
      i--;
    } 
    memset(Path,0,sizeof(Path));
    strncpy(Path,myFile->pathname,i);
    printf("path=%s\n",Path);
    memset(PathName,0,sizeof(PathName));
    sprintf(PathName,"%s/f%03d",Path,DisplayFrameNumber);
    while(((fd=fopen(PathName,"r"))==NULL) && timeout)
    {
      printf("wait=%s\n",PathName);
      usleep(1000);
      timeout--;
		}
    if(!timeout)
    {
      printf("timeout\n");
    }
    else
      fclose(fd);

    memset(command,0,sizeof(command));
    sprintf(command,"mv %s/f%03d %s/current",Path,DisplayFrameNumber++,Path);
    printf("%s\n",command);
    system(command);;

    tatlm1_110FrameSkipping(Path);

  }
  else if( (StillDisplayActivationON==1) &&(IQT_param_array[AcquisitionControl_CurrentState]==acquisitionControl_CurrentState_Pause))
  {
    /* do nothing */
    /* The last current frame is always read */
  }
  else
  {
    /* mandatory to avoid cyclic error when the streaming control panel is opened */
    /* because this function is cyclicly called when the streaming control panel is opened !!!!*/
    vl_Error=EACCES;
  }
  return vl_Error ;
}
int32_t tatlm1_20PreTreatmentActionVideo(void *Value)
{
int32_t vl_Error=TAT_ERROR_OFF;
struct dth_file_data *myFile;
uint32_t i=0;
uint32_t timeout=200;
char Path[100];
char PathName[100];
char command[100];
FILE *fd;
 
  if( (VideoDisplayActivationON==1) &&(IQT_param_array[AcquisitionControl_CurrentState]!=acquisitionControl_CurrentState_Pause))
  {	
    printf("PreTreatment\n");
    myFile=(struct dth_file_data *)Value;
    printf("pathname=%s\n",myFile->pathname);
    printf("offset=%d\n",myFile->offset);
    printf("count=%d\n",myFile->count);

    i=strlen(myFile->pathname)-1;
    while(i!=0)
    {
      if(myFile->pathname[i]=='/')
      {
	break;	
      }
      i--;
    } 
    memset(Path,0,sizeof(Path));
    strncpy(Path,myFile->pathname,i);
    printf("path=%s\n",Path);
    memset(PathName,0,sizeof(PathName));
    sprintf(PathName,"%s/f%03d",Path,DisplayFrameNumber);
    while(((fd=fopen(PathName,"r"))==NULL) && timeout)
    {
      printf("wait=%s\n",PathName);
      usleep(1000);
      timeout--;
    }
    if(!timeout)
    {
      printf("timeout\n");
    }
    else
      fclose(fd);

    memset(command,0,sizeof(command));
    sprintf(command,"mv %s/f%03d %s/current",Path,DisplayFrameNumber++,Path);
    printf("%s\n",command);
    system(command);

    tatlm1_110FrameSkipping(Path);
  }
  else if( (VideoDisplayActivationON==1) &&(IQT_param_array[AcquisitionControl_CurrentState]==acquisitionControl_CurrentState_Pause))
  {
    /* do nothing */
    /* The last current frame is always read */
  }
  else
  {
    /* mandatory to avoid cyclic error when the streaming control panel is opened */
    /* because this function is cyclicly called when the streaming control panel is opened !!!!*/
    vl_Error=EACCES;
  }
  return vl_Error ;
}

 
int Dth_ActPe_exec( struct dth_element *elem)
{ 
  /* Only one action to manage for Page Element */

	if (elem == NULL)
		return -1;

  if(PeDthElementCounter==0)
  {
    tatlm1_116StartInitToIdle();

    tatlm1_108register_pe_dth_struct(MAP_FILE);

    tatlm1_113AclManagementForTheState(acquisitionControl_CurrentState_Idle);

    tatlm1_114DthPeInitAclManagement(ACTION_DISABLED,INIT_PE_DTH_PATHNAME);
  }
  else
    printf("Page Element DTH already mounted\n");

  //Request for a new discover => by asynchrounous event
  return (TAT_ERROR_OFF) ;	 
}

int Dth_ActPeParam_Set(struct dth_element *elem, void *Value)
{
  int32_t vl_Error=TAT_ERROR_OFF;    	
  switch (elem->type){

  case DTH_TYPE_U8:
    PE_param_array [((struct Pe_UserData*)(elem->user_data))->DataIndex]= *(uint8_t*)Value;
    break;
  case DTH_TYPE_S8:
    PE_param_array [((struct Pe_UserData*)(elem->user_data))->DataIndex]= *(int8_t*)Value;
    break;
  case DTH_TYPE_U16:
    PE_param_array [((struct Pe_UserData*)(elem->user_data))->DataIndex]= *(uint16_t*)Value;
    break;
  case DTH_TYPE_S16:
    PE_param_array [((struct Pe_UserData*)(elem->user_data))->DataIndex]= *(int16_t*)Value;
    break;
  case DTH_TYPE_U32:
    PE_param_array [((struct Pe_UserData*)(elem->user_data))->DataIndex]= *(uint32_t*)Value;
    break;
  case DTH_TYPE_S32:
    PE_param_array [((struct Pe_UserData*)(elem->user_data))->DataIndex]= *(int32_t*)Value;
    break;	
  case DTH_TYPE_U64: 
    PE_param_array [((struct Pe_UserData*)(elem->user_data))->DataIndex]= *(uint32_t*)Value;
    break;
  case DTH_TYPE_S64:
    PE_param_array [((struct Pe_UserData*)(elem->user_data))->DataIndex]= *(int32_t*)Value;
    break; 
  case DTH_TYPE_FLOAT:
    PE_param_array [((struct Pe_UserData*)(elem->user_data))->DataIndex]= *(float*)Value;
    break;
  case DTH_TYPE_STRING:
    if ((int8_t*)Value!=NULL)
    {
      uint32_t len =strlen((const char*)Value);
      if (PE_string_array[((struct Pe_UserData*)(elem->user_data))->DataIndex]!=NULL)
      { 
        free(PE_string_array[((struct Pe_UserData*)(elem->user_data))->DataIndex]);
      }
      PE_string_array[((struct Pe_UserData*)(elem->user_data))->DataIndex]=(int8_t*)calloc(len+1,sizeof(int8_t));
      strncpy((char*)PE_string_array[((struct Pe_UserData*)(elem->user_data))->DataIndex],(const char*)Value,len);
    }
    else
    {
      printf (" tatl1_02ActADCParam_Set (int8_t*)Value) ==NULL  \n");	 	
    }
    break;		
  default: 
    SYSLOG(LOG_ERR, "tatl1_02ActVIDEOParam_Set problem with TYPE.");
    vl_Error=TAT_ERROR_CASE;
    break;	
  }
  IqtServ_WritePageElement(fd_input,fd_popen,((struct Pe_UserData*)(elem->user_data))->Index,PE_param_array [((struct Pe_UserData*)(elem->user_data))->DataIndex],"Trigger detected",MMTE_TIMEOUT);

  debug_printf ("Set Page Element: NumBytes=%d index=0x%x value=0x%x DataIndex=%d\n",((struct Pe_UserData*)(elem->user_data))->NumBytes,((struct Pe_UserData*)(elem->user_data))->Index,PE_param_array [((struct Pe_UserData*)(elem->user_data))->DataIndex],((struct Pe_UserData*)(elem->user_data))->DataIndex);		
  return vl_Error ;	
}

int Dth_ActPeParam_Get(struct dth_element *elem, void *Value)
{
  int32_t vl_Error=TAT_ERROR_OFF;

  IqtServ_ReadPageElement(fd_input,fd_popen,((struct Pe_UserData*)(elem->user_data))->Index,&(PE_param_array [((struct Pe_UserData*)(elem->user_data))->DataIndex]),MMTE_TIMEOUT);

  switch (elem->type){

  case DTH_TYPE_U8:
    *((uint8_t*)Value)= PE_param_array [((struct Pe_UserData*)(elem->user_data))->DataIndex];
    break;
  case DTH_TYPE_S8:
    *((int8_t*)Value)= PE_param_array [((struct Pe_UserData*)(elem->user_data))->DataIndex];
    break;
  case DTH_TYPE_U16:
    *((uint16_t*)Value)= PE_param_array [((struct Pe_UserData*)(elem->user_data))->DataIndex];
    break;
  case DTH_TYPE_S16:
    *((int16_t*)Value)= PE_param_array [((struct Pe_UserData*)(elem->user_data))->DataIndex];
    break;
  case DTH_TYPE_U32:
    *((uint32_t*)Value)= PE_param_array [((struct Pe_UserData*)(elem->user_data))->DataIndex];
    break;
  case DTH_TYPE_S32:
    *((int32_t*)Value)= PE_param_array [((struct Pe_UserData*)(elem->user_data))->DataIndex];
    break;	
  case DTH_TYPE_U64:
    *((uint64_t*)Value)= PE_param_array [((struct Pe_UserData*)(elem->user_data))->DataIndex];
    break;
  case DTH_TYPE_S64:
    *((int64_t*)Value)= PE_param_array [((struct Pe_UserData*)(elem->user_data))->DataIndex];
    break;
  case DTH_TYPE_FLOAT:
    *((float*)Value)= PE_param_array [((struct Pe_UserData*)(elem->user_data))->DataIndex];
    break;
  case DTH_TYPE_STRING:
    if (PE_string_array[((struct Pe_UserData*)(elem->user_data))->DataIndex]!=NULL)
    {
      strncpy((char*)Value,(const char*)PE_string_array[((struct Pe_UserData*)(elem->user_data))->DataIndex],strlen((char*)PE_string_array[((struct Pe_UserData*)(elem->user_data))->DataIndex]));
    }
    else 
    {
      strncpy((char*)Value,"NULL",5);			
    }
    break;		
  default:
    SYSLOG(LOG_ERR, "tatl1_03ActVIDEOParam_Get TYPE not available.");
    vl_Error=TAT_ERROR_CASE; 
    break;	
  }  
  debug_printf ("Get Page Element: NumBytes=%d index=0x%x DataIndex=%d\n",((struct Pe_UserData*)(elem->user_data))->NumBytes,((struct Pe_UserData*)(elem->user_data))->Index,((struct Pe_UserData*)(elem->user_data))->DataIndex);	
  return vl_Error ;
}


t_EnumTypeElt* p_iqt_EnumTypeElt=NULL;
t_EnumTypeElt* p_iqt_FirstEnumTypeElt=NULL;
t_ValEnumElt* p_first_enum_list=NULL;
t_ValEnumElt* p_enum_list=NULL;


int32_t tatlm1_100EnumManagement(char *mapfile)
{ 
  int32_t vl_error = 0;
  FILE *file_handle;
  char string_read[100];
  int vl_Entries;
  int step;
  int k=0;
  int p=0;
  char CurrentAttr[100];


  file_handle=fopen(mapfile,"r");
  if(file_handle==NULL)
  {
    printf("The map file is missing\n");	
    return vl_error;
  }

  step=STEP1;

  memset(string_read,0,sizeof(string_read));
  while(fgets(string_read,4,file_handle)!=NULL)
  {
    if(strncmp(string_read,"End",3)==0)
    {
      break;
    }
  }
  printf("End the map file found for the enum management\n");

  memset(string_read,0,sizeof(string_read));
  while(fgets(string_read,sizeof(string_read),file_handle)!=NULL)
  {
    if(strncmp(string_read,"[",1) == 0)
    {
      char TypeEnum[100];
			
      k=0;
      memset(TypeEnum,0,sizeof(TypeEnum));
      do
      {
	TypeEnum[k]=(&string_read[1])[k];
	k++;
      }while((&string_read[1])[k]!=']');
      debug_printf("TypeEnum=%s\n",TypeEnum);

      if(p_iqt_EnumTypeElt!=NULL)
      {
	p_iqt_EnumTypeElt->p_Next=(t_EnumTypeElt*)malloc(sizeof(t_EnumTypeElt));
	p_iqt_EnumTypeElt=p_iqt_EnumTypeElt->p_Next;
      }
      else
	p_iqt_EnumTypeElt=(t_EnumTypeElt*)malloc(sizeof(t_EnumTypeElt));

      if(p_iqt_FirstEnumTypeElt==NULL)
	p_iqt_FirstEnumTypeElt=p_iqt_EnumTypeElt;

      p_iqt_EnumTypeElt->EnumType=(char*)malloc(strlen(TypeEnum)+1);
      memset(p_iqt_EnumTypeElt->EnumType,0,strlen(TypeEnum)+1);
      strncpy(p_iqt_EnumTypeElt->EnumType,TypeEnum,strlen(TypeEnum));
      //strncpy(p_iqt_EnumTypeElt->EnumType,TypeEnum);
      p_iqt_EnumTypeElt->p_Next=NULL;
    }

    if(strncmp(string_read,"entries = ",10) == 0)
    {
      char Entries[4];
      char AttrNb[3];
      int vl_AttrNb,vl_AttrIndex;

      memset(Entries,0,sizeof(Entries));
      strcpy(Entries,&string_read[10]);
      sscanf(Entries,"%d",&vl_Entries);
      debug_printf("Entries = %d\n",vl_Entries);
		       	
      p_iqt_EnumTypeElt->EnumEntries=vl_Entries;
      p_iqt_EnumTypeElt->ValEnumElt=(t_ValEnumElt*)malloc(vl_Entries*sizeof(t_ValEnumElt));
      for(k=0;k<vl_Entries;k++)
      {
	if(fgets(string_read,100,file_handle)!=NULL)
	{
	  memset(AttrNb,0,sizeof(AttrNb));
	  strncpy(AttrNb,&string_read[4],2);
	  sscanf(AttrNb,"%d",&vl_AttrNb);
	  if(vl_AttrNb>9)
	    vl_AttrIndex=9;
	  else
	    vl_AttrIndex=8;

	  debug_printf("Attr%d = %s\n",k,&string_read[vl_AttrIndex]);
	  p_iqt_EnumTypeElt->ValEnumElt[k].EnumAttr=(char*)malloc(strlen(&string_read[vl_AttrIndex]));
	  memset(p_iqt_EnumTypeElt->ValEnumElt[k].EnumAttr,0,strlen(&string_read[vl_AttrIndex]));
	  strncpy(p_iqt_EnumTypeElt->ValEnumElt[k].EnumAttr,&string_read[vl_AttrIndex],strlen(&string_read[vl_AttrIndex])-1);
	  memset(CurrentAttr,0,sizeof(CurrentAttr));
	  strncpy(CurrentAttr,p_iqt_EnumTypeElt->ValEnumElt[k].EnumAttr,strlen(p_iqt_EnumTypeElt->ValEnumElt[k].EnumAttr));
	}
	else
	{
	  debug_printf("Syntax error in the map file\n");
	}
	if(fgets(string_read,100,file_handle)!=NULL)
	{
	  char Value[8];
	  int  vl_Value;

	  memset(Value,0,sizeof(Value));
	  //Value[0]=string_read[7];
	  strncpy(Value,&string_read[7],strlen(&string_read[7]));
	  sscanf(Value,"%d",&vl_Value);

	  debug_printf("_val%d =%s",k,&string_read[7]);
	  debug_printf("__val%d =%d\n",k,vl_Value);
	  p_iqt_EnumTypeElt->ValEnumElt[k].EnumVal=vl_Value;

	  if(k>0)
	    if(p_iqt_EnumTypeElt->ValEnumElt[k-1].EnumVal>p_iqt_EnumTypeElt->ValEnumElt[k].EnumVal)
	    {
	      printf("sorting for %s\n",CurrentAttr);
	      p=k;
	      do
	      {		
		if(p_iqt_EnumTypeElt->ValEnumElt[p-1].EnumVal>p_iqt_EnumTypeElt->ValEnumElt[p].EnumVal)
		{
		  p_iqt_EnumTypeElt->ValEnumElt[p].EnumVal=p_iqt_EnumTypeElt->ValEnumElt[p-1].EnumVal;
		  free(p_iqt_EnumTypeElt->ValEnumElt[p].EnumAttr);
		  p_iqt_EnumTypeElt->ValEnumElt[p].EnumAttr=(char*)malloc(strlen(p_iqt_EnumTypeElt->ValEnumElt[p-1].EnumAttr)+1);
		  memset(p_iqt_EnumTypeElt->ValEnumElt[p].EnumAttr,0,strlen(p_iqt_EnumTypeElt->ValEnumElt[p-1].EnumAttr)+1);
		  strncpy(p_iqt_EnumTypeElt->ValEnumElt[p].EnumAttr,p_iqt_EnumTypeElt->ValEnumElt[p-1].EnumAttr,strlen(p_iqt_EnumTypeElt->ValEnumElt[p-1].EnumAttr));

		  p_iqt_EnumTypeElt->ValEnumElt[p-1].EnumVal=vl_Value;
		  free(p_iqt_EnumTypeElt->ValEnumElt[p-1].EnumAttr);
		  p_iqt_EnumTypeElt->ValEnumElt[p-1].EnumAttr=(char*)malloc(strlen(CurrentAttr)+1);
		  memset(p_iqt_EnumTypeElt->ValEnumElt[p-1].EnumAttr,0,strlen(CurrentAttr)+1);
		  strncpy(p_iqt_EnumTypeElt->ValEnumElt[p-1].EnumAttr,CurrentAttr,strlen(CurrentAttr));
		}
		else
		  break;
		p--;
	      }								
	      while(p!=0);
	    }
	}
	else
	{
	  printf("Syntax error in the map file\n");
	}
      }
    }
  }
  fclose(file_handle);
  return(vl_error);
}

int32_t tatlm1_101EnumManagementDebug(void)
{
int k=0;
t_EnumTypeElt* p_iqt_EnumTypeCurrentElt=NULL;

  p_iqt_EnumTypeCurrentElt = p_iqt_FirstEnumTypeElt;
	
  while(p_iqt_EnumTypeCurrentElt!=NULL)
  {
    printf("EnumType=%s EnumEntries=%d\n",p_iqt_EnumTypeCurrentElt->EnumType,p_iqt_EnumTypeCurrentElt->EnumEntries);
    for(k=0;k<p_iqt_EnumTypeCurrentElt->EnumEntries;k++)
    {
      printf("Attr%d=%s\n Val%d=%d\n",k,p_iqt_EnumTypeCurrentElt->ValEnumElt[k].EnumAttr,k,p_iqt_EnumTypeCurrentElt->ValEnumElt[k].EnumVal);
    }
    p_iqt_EnumTypeCurrentElt=p_iqt_EnumTypeCurrentElt->p_Next;
  }
  return(0);
}

int32_t tatlm1_102DeviceVersionManagement(char* StringOfTheMapFile)
{
int32_t vl_error = 0;
struct dth_element *vl_elt = NULL;
char Version[100];
char *pl_VersionStart,*pl_VersionStop;

  pl_VersionStart=strstr(StringOfTheMapFile,"\"");
  pl_VersionStop=strstr(&pl_VersionStart[1],"\"");
	
  if((pl_VersionStart==NULL) || (pl_VersionStop==NULL))
  {
    printf("Syntax error: version not compliant\n");
    return(-1);
  }

  if((unsigned int) (((int)pl_VersionStop-(int)pl_VersionStart)-2)>sizeof(Version) )
  {
    printf("Size error: the string for the version is too long\n");
    return(-1);
  }
  printf("Version found\n");
  memset(Version,0,sizeof(Version));
  strncpy(Version,&pl_VersionStart[1],((int)pl_VersionStop-(int)pl_VersionStart)-1);

  vl_elt = (struct dth_element *)malloc(sizeof(struct dth_element));
  vl_elt->path = (char*)malloc(strlen(FW_VERSION_DTH_PATHNAME)+1);
  memset(vl_elt->path,0,strlen(FW_VERSION_DTH_PATHNAME)+1);	
  strncpy(vl_elt->path,FW_VERSION_DTH_PATHNAME,strlen(FW_VERSION_DTH_PATHNAME));

  vl_elt->type=DTH_TYPE_STRING;
  vl_elt->cols=0;
  vl_elt->rows=0;
  vl_elt->enumString=NO_ENUM;
  vl_elt->info=NO_INFO;
  vl_elt->min=0;
  vl_elt->max=0;
  vl_elt->get=NO_GET;
  vl_elt->set=NO_SET;
  vl_elt->exec=NO_EXEC;
  vl_elt->user_data=FW_VERSION;
	
  vl_elt->default_value = (char*)malloc(strlen(Version)+1);
  memset((char*)(vl_elt->default_value),0,strlen(Version)+1);
  strcpy((char*)(vl_elt->default_value),Version);
  printf("default_value = %s\n",(char*)(vl_elt->default_value));

  vl_error = dth_register_element(vl_elt);
  return(vl_error);
}

int32_t tatlm1_103GroupItemManagement(char* StringOfTheMapFile,char* GroupItem,char* GroupItemAccessRights)
{
int i=0;
int j=0;
char GroupItemAccessRightsNull[50];

 strncpy(GroupItemAccessRightsNull,GroupItemAccessRights,sizeof(GroupItemAccessRightsNull));
 do
 {
   GroupItem[i]=(&StringOfTheMapFile[GROUP_ITEM_INDEX])[i];
   i++;
 }while(((&StringOfTheMapFile[GROUP_ITEM_INDEX])[i]!=' ') && ((&StringOfTheMapFile[GROUP_ITEM_INDEX])[i]!='"'));
 debug_printf("%s\n",GroupItem);

 strncpy(GroupItemAccessRights,"read write",strlen("read write")+1);
 if(((&StringOfTheMapFile[GROUP_ITEM_INDEX])[i+1]=='['))
 {
   strncpy(GroupItemAccessRights,GroupItemAccessRightsNull,sizeof(GroupItemAccessRightsNull));
   do
   {
     GroupItemAccessRights[j]=(&StringOfTheMapFile[GROUP_ITEM_INDEX])[i+2+j];
     j++;
   }while((&StringOfTheMapFile[GROUP_ITEM_INDEX])[i+2+j]!=']');
 }
 debug_printf("AccessRights=%s\n",GroupItemAccessRights);

 tatlm1_111BracketManagement(GroupItem);

 return(0);
}

int32_t tatlm1_104PrefixAndItemPreManagement(char* StringOfTheMapFile,char* Item,char* PeItem,char* PrefixItem)
{
int vl_error = 0;
int i;

  i=0;
  do
  {
    PeItem[i]=(&StringOfTheMapFile[ITEM_INDEX])[i];
    i++;
  }while((&StringOfTheMapFile[ITEM_INDEX])[i]!='"');
  debug_printf("Pe Item full %s \n",&StringOfTheMapFile[ITEM_INDEX]);

  i=0;

  do
  {
    PrefixItem[i]=PeItem[i];
    i++;
  }while(PeItem[i]!='_');	

  strcpy(Item,PeItem);
  debug_printf("len=%d\n",strlen(&StringOfTheMapFile[ITEM_INDEX]));

  vl_error = tatlm1_111BracketManagement(Item);	
  return(vl_error);
}

int32_t tatlm1_105EnumItemManagement(char* StringOfTheMapFile,struct dth_element* pp_DthElt,int vp_NumBytes,int vp_index)
{
int vl_error = 0;
char Enum[100];
t_EnumTypeElt* p_iqt_EnumTypeCurrentElt=NULL;
t_ValEnumElt* p_current_enum_list=NULL;
int k=0;
int length=0;
char* pl_enumString=NULL;


  switch(vp_NumBytes)
  {
    case 1:
    {
      pp_DthElt->type=DTH_TYPE_U8;
    }
    break;
    case 2:
    {
      pp_DthElt->type=DTH_TYPE_U16;
    }
    break;
    case 4:
    {
      pp_DthElt->type=DTH_TYPE_U32;
    }
    break;
    default:
      SYSLOG(LOG_ERR, "Case forbidden.");
      return(TAT_ERROR_MISC);
    break;
  }
  memset(Enum,0,sizeof(Enum));
  strcpy(Enum,&(&StringOfTheMapFile[ITEM_INDEX])[FIX_ENUM_INDEX+vp_index]);
  debug_printf("enum detected %d = %s\n",vp_index,Enum);
					
  p_iqt_EnumTypeCurrentElt = p_iqt_FirstEnumTypeElt;
  while(p_iqt_EnumTypeCurrentElt!=NULL)
  {
    if(strncmp(p_iqt_EnumTypeCurrentElt->EnumType,Enum,strlen(p_iqt_EnumTypeCurrentElt->EnumType)) == 0)
    {
      debug_printf("EnumType %s EnumEntries %d\n",p_iqt_EnumTypeCurrentElt->EnumType,p_iqt_EnumTypeCurrentElt->EnumEntries);
      break;
    }
    else
      p_iqt_EnumTypeCurrentElt=p_iqt_EnumTypeCurrentElt->p_Next;
		//debug_printf("next=%x\n",p_iqt_EnumTypeCurrentElt);
  }
  debug_printf("flag1\n");

  /* for each entries in enum table */
  for(k=0;k<p_iqt_EnumTypeCurrentElt->EnumEntries;k++) {

    debug_printf("Attr%d %s Val%d %d\n",k,p_iqt_EnumTypeCurrentElt->ValEnumElt[k].EnumAttr,k,p_iqt_EnumTypeCurrentElt->ValEnumElt[k].EnumVal);

    /* length = length of enum attribute + spacer caracteres ( \n, \t, " ") +
     * 3 caracteres for 3 digits */
    length = strlen(" ") + strlen(p_iqt_EnumTypeCurrentElt->ValEnumElt[k].EnumAttr) + 3 + strlen("\t\n");

    /* add length to previous length for memory reallocation */
    if (pl_enumString != NULL)
      length = length+strlen(pl_enumString);

    pl_enumString = (char*)realloc(pl_enumString,length);
    if (pl_enumString == NULL) 
    {
      printf("error realloc pl_enumString0\n");						
      vl_error = -1;
      free(pl_enumString);
      return vl_error;
    }
    /* string concatenation
     * string format => enumAttr0\tvalueAttr0\nenumAttr1\tvalueAttr1\n...*/
    if(k>0)
      sprintf(pl_enumString,"%s %s\t%d\n",pl_enumString,p_iqt_EnumTypeCurrentElt->ValEnumElt[k].EnumAttr,p_iqt_EnumTypeCurrentElt->ValEnumElt[k].EnumVal);
    else
      sprintf(pl_enumString," %s\t%d\n",p_iqt_EnumTypeCurrentElt->ValEnumElt[k].EnumAttr,p_iqt_EnumTypeCurrentElt->ValEnumElt[k].EnumVal);
  }

  debug_printf("test_enum =\n%s\n",pl_enumString);
  pp_DthElt->enumString=(char*)((char*)malloc(strlen(pl_enumString)+1));
  memset((char*)pp_DthElt->enumString,0,strlen(pl_enumString)+1);
  strncpy((char*)pp_DthElt->enumString,pl_enumString,strlen(pl_enumString));

  free(pl_enumString);
  free(p_first_enum_list);
  p_first_enum_list=p_current_enum_list=NULL;

  return(vl_error);
}

int32_t tatlm1_106TypeItemManagement(struct dth_element* pp_DthElt,int vp_NumBytes,char* PrefixItem)
{
	int vl_error = 0;

  if((strncmp(PrefixItem,"u32",3) == 0) && (vp_NumBytes==4))
  {
    pp_DthElt->type=DTH_TYPE_U32;
    pp_DthElt->min=0;
    pp_DthElt->max=0xFFFFFFFF;
  }
  if((strncmp(PrefixItem,"u16",3) == 0) && (vp_NumBytes==2))
  {
    pp_DthElt->type=DTH_TYPE_U16;
    pp_DthElt->min=0;
    pp_DthElt->max=0xFFFF;
  }
  if((strncmp(PrefixItem,"u8",2) == 0)  && (vp_NumBytes==1))
  {
    pp_DthElt->type=DTH_TYPE_U8;
    pp_DthElt->min=0;
    pp_DthElt->max=0xFF;
  }
  if((strncmp(PrefixItem,"s32",3) == 0) && (vp_NumBytes==4))
  {
    pp_DthElt->type=DTH_TYPE_S32;
    pp_DthElt->min=(long)0x80000000;
    pp_DthElt->max=0x7FFFFFFF;
  }
  if((strncmp(PrefixItem,"s16",3) == 0) && (vp_NumBytes==2))
  {
    pp_DthElt->type=DTH_TYPE_S16;
    pp_DthElt->min=-32768;
    pp_DthElt->max=0x7FFF;
  }
  if((strncmp(PrefixItem,"s8",2) == 0)  && (vp_NumBytes==1))
  {
    pp_DthElt->type=DTH_TYPE_S8;
    pp_DthElt->min=-128;
    pp_DthElt->max=0x7F;
  }
  if(strncmp(PrefixItem,"bo",2) == 0)
  {
    pp_DthElt->type=DTH_TYPE_U8;
    pp_DthElt->min=0;
    pp_DthElt->max=1;
  }
  if((strncmp(PrefixItem,"f",1) == 0) && (vp_NumBytes==4))
  {
    pp_DthElt->type=DTH_TYPE_FLOAT;
    pp_DthElt->min=(long)0x80000000;
    pp_DthElt->max=0x7FFFFFFF;
  }
  if((strncmp(PrefixItem,"u32",3) != 0) && (strncmp(PrefixItem,"s32",3) != 0) && (strncmp(PrefixItem,"f",1) != 0)  && (vp_NumBytes==4))
  {
    pp_DthElt->type=DTH_TYPE_U32;
    pp_DthElt->min=0;
    pp_DthElt->max=0xFFFFFFFF;
  }
  return(vl_error);
}

int32_t tatlm1_107ItemManagement(char* StringOfTheMapFile,char* GroupItem,char* GroupItemAccessRights)
{
int32_t vl_error = 0;
struct dth_element *vl_elt = NULL;
unsigned int j;
char PeItem[150];
char PrefixItem[150];
char Item[150];
char PeDthPathItem[500];
char al_info[300];
int vl_Index;
int vl_NumBytes;
int vl_ToggleEndian = 0;
char Index[7];
char NumBytes[2];
char vl_temp1,vl_temp2;
int result=-1;


  memset(Item,0,sizeof(Item));
  memset(PeItem,0,sizeof(PeItem));
  memset(PrefixItem,0,sizeof(PrefixItem));

  tatlm1_104PrefixAndItemPreManagement(StringOfTheMapFile,Item,PeItem,PrefixItem);
  debug_printf("Pe Item  %s detected for %s prefix=%s item=%s\n",PeItem,GroupItem,PrefixItem,Item);

  result=strncmp(GroupItem,"Reserved",8);
  if(result!=0)
  {
    memset(PeDthPathItem,0,sizeof(PeDthPathItem));
    sprintf(PeDthPathItem,"%s%s/%s.%s",IQT_PE_DTH_PATHNAME,GroupItem,GroupItem,Item);
    vl_elt = (struct dth_element *)malloc(sizeof(struct dth_element));
    vl_elt->path = (char*)malloc(strlen(PeDthPathItem)+1);
    memset(vl_elt->path,0,strlen(PeDthPathItem)+1);	
    strncpy(vl_elt->path,PeDthPathItem,strlen(PeDthPathItem));
 
    vl_elt->type=DTH_TYPE_U32;
    vl_elt->enumString=NO_ENUM;
    vl_elt->cols=0;
    vl_elt->rows=0;
    vl_elt->min=0;
    vl_elt->get=NO_GET;
    vl_elt->set=NO_SET;
    debug_printf("AccessRights=%s\n",GroupItemAccessRights);
    if((strncmp(GroupItemAccessRights,"read only",strlen("read only"))==0) || (strncmp(GroupItemAccessRights,"read write",strlen("read write"))==0))
      vl_elt->get=Dth_ActPeParam_Get;
    if((strncmp(GroupItemAccessRights,"write only",strlen("write only"))==0) || (strncmp(GroupItemAccessRights,"read write",strlen("read write"))==0))
      vl_elt->set=Dth_ActPeParam_Set;
    vl_elt->exec=NO_EXEC;

    for(j=0;j<strlen(&StringOfTheMapFile[ITEM_INDEX]);j++)
    {
      if(((&StringOfTheMapFile[ITEM_INDEX])[0+j]==' ') &&((&StringOfTheMapFile[ITEM_INDEX])[1+j]=='I') && ((&StringOfTheMapFile[ITEM_INDEX])[2+j]=='n') && ((&StringOfTheMapFile[ITEM_INDEX])[3+j]=='d') && ((&StringOfTheMapFile[ITEM_INDEX])[4+j]=='e') && ((&StringOfTheMapFile[ITEM_INDEX])[5+j]=='x') && ((&StringOfTheMapFile[ITEM_INDEX])[6+j]==' '))
      { 
	vl_ToggleEndian=0;
	memset(Index,0,sizeof(Index));
	memset(NumBytes,0,sizeof(NumBytes));
	strncpy(Index,&(&StringOfTheMapFile[ITEM_INDEX])[REGISTER_INDEX+j],6);
	sscanf(Index,"%c%c%x",&vl_temp1,&vl_temp2,&vl_Index);
	strncpy(NumBytes,&(&StringOfTheMapFile[ITEM_INDEX])[TYPE_INDEX+j],1);
	sscanf(NumBytes,"%d",&vl_NumBytes);

	if(strncmp(&(&StringOfTheMapFile[ITEM_INDEX])[ENDIAN_INDEX+j],"ToggleEndian",12)==0)
	  vl_ToggleEndian=1;
	debug_printf("index=0x%04x NumBytes=%d Endianess=%d j=%d\n",vl_Index,vl_NumBytes,vl_ToggleEndian,j);				
	if(strncmp(PrefixItem,"e",1) == 0)
	{
	  vl_error=tatlm1_105EnumItemManagement(StringOfTheMapFile,vl_elt,vl_NumBytes,j);
	}
	break;		
      }
    }
    tatlm1_106TypeItemManagement(vl_elt,vl_NumBytes,PrefixItem);

    vl_elt->user_data=(int)((struct Pe_UserData*)malloc(sizeof(struct Pe_UserData)));
    ((struct Pe_UserData*)(vl_elt->user_data))->Index=vl_Index;
    ((struct Pe_UserData*)(vl_elt->user_data))->NumBytes=vl_NumBytes;
    ((struct Pe_UserData*)(vl_elt->user_data))->ToggleEndian=vl_ToggleEndian;
    ((struct Pe_UserData*)(vl_elt->user_data))->DataIndex=PeDthElementCounter;
 
    sprintf(al_info,"index=0x%04x ToggleEndian=%d DataIndex=%d",vl_Index,vl_ToggleEndian,PeDthElementCounter++);
    vl_elt->info=(char*)((char*)malloc(strlen(al_info)+1));
    strncpy((char*)vl_elt->info,al_info,strlen(al_info)+1);

    vl_elt->default_value = NULL;
    vl_error = dth_register_element(vl_elt);
    //printf("register elt status = %d\n",vl_error);
    debug_printf("path for the elt %s\n",vl_elt->path);
  }
  else
    printf("%s filtered out\n",GroupItem);

  return(vl_error);	
}

int32_t tatlm1_108register_pe_dth_struct(char *mapfile)
{ 
  int32_t vl_error = 0;
  FILE *file_handle;
  char string_read[200];
  char GroupItem[50];
  char GroupItemAccessRights[50];

  tatlm1_100EnumManagement(mapfile);
  //tatlm1_101EnumManagementDebug();

  file_handle=fopen(mapfile,"r");
  if(file_handle==NULL)
  {
    printf("The map file is missing:\n");	
    return vl_error;
  }
  memset(string_read,0,sizeof(string_read));
  PeDthElementCounter=0;

  while(fgets(string_read,sizeof(string_read),file_handle)!=NULL)
  {
    if(strncmp(string_read,"Device ",7) == 0)
    {
      tatlm1_102DeviceVersionManagement(string_read);
    }
    if((strncmp(string_read,"\tGroup",6) == 0) && (strncmp(string_read,"\tGroup End",10) != 0))
    {
      memset(GroupItem,0,sizeof(GroupItem));
      memset(GroupItemAccessRights,0,sizeof(GroupItemAccessRights));
      tatlm1_103GroupItemManagement(string_read,GroupItem,GroupItemAccessRights);
      printf("%s\n",GroupItem);
    }
    if(strncmp(string_read,"\t\tItem",6) == 0)
    {
      tatlm1_107ItemManagement(string_read,GroupItem,GroupItemAccessRights);
      debug_printf("Number of Page Element = %d\n",PeDthElementCounter);
    }
    if(strncmp(string_read,"End",3) == 0)
    {
      printf("Reading of the map file for ISP finished\n");
      printf("Number of Page Element = %d\n",PeDthElementCounter);
      PE_param_array=(int32_t*)calloc(PeDthElementCounter,sizeof(int32_t));
      PE_string_array=(int8_t**)calloc(PeDthElementCounter,sizeof(int32_t));
    }	  
  }
  fclose(file_handle);
  return vl_error;
}

uint32_t  tatlm1_109InitFileDescriptors(int camera_fd_input,int camera_fd_popen)
{
int32_t vl_error = 0;

  fd_input=camera_fd_input;
  fd_popen=camera_fd_popen;

  return vl_error;
}

int32_t tatlm1_110FrameSkipping(char *Path)
{
char command[100];
FILE *ListOfCurrentFrames;
char FileName[100];;
uint32_t ListOfCurrentFramesNumber=0;
char vl_temp1;
uint32_t vl_Index;
uint32_t j=0;
uint32_t i=0;
int32_t vl_error = 0;

  memset(command,0,sizeof(command));
  sprintf(command,"ls %s/* >/tmp/ListOfCurrentFrames",Path);
  debug_printf("%s\n",command);
  system(command);		
	
  memset(FileName,0,sizeof(FileName));
  ListOfCurrentFrames=fopen("/tmp/ListOfCurrentFrames","r");
  while(fgets(FileName,sizeof(FileName),ListOfCurrentFrames)!=NULL)
  {
    ListOfCurrentFramesNumber++;
  }
  debug_printf("%s %d\n",FileName,ListOfCurrentFramesNumber);
	
  fclose(ListOfCurrentFrames);

  i=0;
  if(ListOfCurrentFramesNumber>0)
    i=strlen(FileName);
  while(i!=0)
  {
    if(FileName[i]=='/')
    {
      break;	
    }
    i--;
  } 
  if(FileName[i+1]=='f')
  {
    sscanf(&FileName[i+1],"%c%d",&vl_temp1,&vl_Index);
    debug_printf("file=%s %d\n",&FileName[i+1],vl_Index);

    for(j=DisplayFrameNumber;j<vl_Index;j++)
    {
      memset(command,0,sizeof(command));
      sprintf(command,"rm -f %s/f%03d",Path,j);
      debug_printf("%s\n",command);
      system(command);		
    }
    printf("Frames skipped:%d\n",vl_Index-DisplayFrameNumber);
    DisplayFrameNumber=vl_Index;

  }
  else
    printf("Frames skipped: 0\n");

  return vl_error;
}

int32_t tatlm1_111BracketManagement(char *Item)
{
int EndOfStringIndex;
int k=0;
int32_t vl_error = 0;

  EndOfStringIndex=strlen(Item)-1;	
  if(Item[EndOfStringIndex]==']')
  {
    k=EndOfStringIndex-1;
    while(Item[k]!='[')
    {
      k--;
    }
    Item[k]='_';
    Item[EndOfStringIndex]=0;
  }
  return vl_error;
}

int32_t tatlm1_112DthAclManagement(char *pp_ActionName,int vp_enable)
{
char command[100];
int32_t vl_error = 0;
 
  memset(command,0,sizeof(command));
  if(vp_enable==1)
  {
    sprintf(command,"chmod +x /mnt/9p/DTH/iq_tunning/streaming_control/%s/value &",pp_ActionName);	
  }
  else
  {
    sprintf(command,"chmod -x /mnt/9p/DTH/iq_tunning/streaming_control/%s/value &",pp_ActionName);	
  }
  printf("%s\n",command);
#if defined(DTH_ACTION_ACL_MGT)
  system(command);
#endif	

  return vl_error;
}

int32_t tatlm1_113AclManagementForTheState(int State)
{
int32_t vl_error = 0;

  switch(State)
  {
    case acquisitionControl_CurrentState_Init:
      tatlm1_112DthAclManagement("AcquisitionControl",ACTION_ENABLED);
      tatlm1_112DthAclManagement("ViewFinder",ACTION_DISABLED);
      tatlm1_112DthAclManagement("StillPicture",ACTION_DISABLED);
      tatlm1_112DthAclManagement("Video",ACTION_DISABLED);
      //tatlm1_112DthAclManagement("GoToIdle",ACTION_ENABLED);
      //tatlm1_112DthAclManagement("Pause",ACTION_DISABLED);
      //tatlm1_112DthAclManagement("Resume",ACTION_DISABLED);
      break;
  case acquisitionControl_CurrentState_Idle:
    tatlm1_112DthAclManagement("AcquisitionControl",ACTION_ENABLED);
    tatlm1_112DthAclManagement("ViewFinder",ACTION_ENABLED);
    tatlm1_112DthAclManagement("StillPicture",ACTION_DISABLED);
    tatlm1_112DthAclManagement("Video",ACTION_DISABLED);
    //tatlm1_112DthAclManagement("GoToIdle",ACTION_ENABLED);
    //tatlm1_112DthAclManagement("Pause",ACTION_DISABLED);
    //tatlm1_112DthAclManagement("Resume",ACTION_DISABLED);
    break;
  case acquisitionControl_CurrentState_ViewFinder:
    tatlm1_112DthAclManagement("AcquisitionControl",ACTION_ENABLED);
    tatlm1_112DthAclManagement("ViewFinder",ACTION_ENABLED);
    tatlm1_112DthAclManagement("StillPicture",ACTION_ENABLED);
    tatlm1_112DthAclManagement("Video",ACTION_ENABLED);
    //tatlm1_112DthAclManagement("GoToIdle",ACTION_ENABLED);
    //tatlm1_112DthAclManagement("Pause",ACTION_ENABLED);
    //tatlm1_112DthAclManagement("Resume",ACTION_DISABLED);
    break;
  case acquisitionControl_CurrentState_StillPicture:
    tatlm1_112DthAclManagement("AcquisitionControl",ACTION_DISABLED);
    tatlm1_112DthAclManagement("ViewFinder",ACTION_DISABLED);
    tatlm1_112DthAclManagement("StillPicture",ACTION_ENABLED);
    tatlm1_112DthAclManagement("Video",ACTION_DISABLED);
    //tatlm1_112DthAclManagement("GoToIdle",ACTION_DISABLED);
    //tatlm1_112DthAclManagement("Pause",ACTION_DISABLED);
    //tatlm1_112DthAclManagement("Resume",ACTION_DISABLED);
    break;
  case acquisitionControl_CurrentState_Video:
    tatlm1_112DthAclManagement("AcquisitionControl",ACTION_ENABLED);
    tatlm1_112DthAclManagement("ViewFinder",ACTION_DISABLED);
    tatlm1_112DthAclManagement("StillPicture",ACTION_DISABLED);
    tatlm1_112DthAclManagement("Video",ACTION_ENABLED);
    //tatlm1_112DthAclManagement("GoToIdle",ACTION_DISABLED);
    //tatlm1_112DthAclManagement("Pause",ACTION_ENABLED);
    //tatlm1_112DthAclManagement("Resume",ACTION_DISABLED);
    break;
  case acquisitionControl_CurrentState_Pause:
    tatlm1_112DthAclManagement("AcquisitionControl",ACTION_ENABLED);
    tatlm1_112DthAclManagement("ViewFinder",ACTION_DISABLED);
    tatlm1_112DthAclManagement("StillPicture",ACTION_DISABLED);
    tatlm1_112DthAclManagement("Video",ACTION_DISABLED);
    //tatlm1_112DthAclManagement("GoToIdle",ACTION_DISABLED);
    //tatlm1_112DthAclManagement("Pause",ACTION_DISABLED);
    //tatlm1_112DthAclManagement("Resume",ACTION_ENABLED);
    break;
  }
  return vl_error;
}

int32_t tatlm1_114DthPeInitAclManagement(int vp_enable,char* pp_DthActionPathName)
{
char command[100];
int32_t vl_error = 0;
 
  memset(command,0,sizeof(command));
  if(vp_enable==1)
  {
    sprintf(command,"chmod +x /mnt/9p%s/value &",pp_DthActionPathName);	
  }
  else
  {
    sprintf(command,"chmod -x /mnt/9p%s/value &",pp_DthActionPathName);	
  }
  printf("%s\n",command);
  system(command);
	
  return vl_error;
}


int32_t tatlm1_115PeInitAction(void)
{
int32_t vl_error = 0;
struct dth_element *vl_elt = NULL;
static char *pl_info = "9ptimeout:15000";

  vl_elt = (struct dth_element *)malloc(sizeof(struct dth_element));
  vl_elt->path = (char*)malloc(strlen(INIT_PE_DTH_PATHNAME)+1);
  memset(vl_elt->path,0,strlen(INIT_PE_DTH_PATHNAME)+1);	
  strncpy(vl_elt->path,INIT_PE_DTH_PATHNAME,strlen(INIT_PE_DTH_PATHNAME));

  vl_elt->info=(char*)((char*)malloc(strlen(pl_info)+1));
  strncpy((char*)vl_elt->info,pl_info,strlen(pl_info)+1);

  vl_elt->type=DTH_TYPE_U8;
  vl_elt->cols=0;
  vl_elt->rows=0;
  vl_elt->enumString=NO_ENUM;
  vl_elt->min=0;
  vl_elt->max=0;
  vl_elt->get=NO_GET;
  vl_elt->set=NO_SET;
  vl_elt->exec=Dth_ActPe_exec;
  vl_elt->user_data=INIT_PE;
  vl_elt->default_value = NULL;

  vl_error = dth_register_element(vl_elt);
  return(vl_error);
}

void tatlm1_116StartInitToIdle(void)
{
  char command[100];

  tatlm1_07RamDiskManagement();

  IqtServ_InitPipe(input_iqt_pipe,output_iqt_pipe,iqt_output_MMTE_task_pipe,&fd_input,&fd_output,&fd_popen,0);

  memset(command,0,sizeof(command));
  sprintf(command,"mount %s\n",DEFAULT_SDCARD_DIRECTORY_PATH);
  printf(command);
  system(command);

  IqtServ_StartMmte(input_iqt_pipe,output_iqt_pipe,fd_popen,"Trigger detected",MMTE_TIMEOUT);
  if(IQT_param_array [AcquisitionControl_OmxGraphUsed]==acquisitionControl_OmxGraphUsed_IQT)
  { 
    tatlm1_14MmteStringManagement("NOT_OMX_GRAPH_IQT","OMX_FALSE");
  }
  else
  {
    tatlm1_14MmteStringManagement("NOT_OMX_GRAPH_IQT","OMX_TRUE");
  }

  if(IQT_param_array [ViewFinder_On_Display]==ViewFinder_OnDisplayActivation_ON)
    tatlm1_14MmteStringManagement("VF_ON_DISPLAY_ACTIVATION","OMX_TRUE");
  else
    tatlm1_14MmteStringManagement("VF_ON_DISPLAY_ACTIVATION","OMX_FALSE");

  IqtServ_InitMmte(fd_input,fd_popen,"Trigger detected",MMTE_TIMEOUT);
  IQT_param_array[AcquisitionControl_CurrentState]=acquisitionControl_CurrentState_Init;
  tatlm1_05InternalIqtState("INIT");

  if(IQT_param_array [AcquisitionControl_OmxGraphUsed]==acquisitionControl_OmxGraphUsed_IQT)
  { 
    tatlm1_14MmteStringManagement("NETWORK_FRAMEWORK_WITH_IMGFILTER","OMX_FALSE");
    tatlm1_14MmteStringManagement("NETWORK_FRAMEWORK_WITH_SPLITTER","OMX_TRUE");
  }
  else
  {
    tatlm1_14MmteStringManagement("NETWORK_FRAMEWORK_WITH_IMGFILTER","OMX_TRUE");
    tatlm1_14MmteStringManagement("NETWORK_FRAMEWORK_WITH_SPLITTER","OMX_FALSE");
  }

  tatlm1_15MmteValueManagement("DEFAULT_VPB0_WIDTH",IQT_param_array[ViewFinder_Width]);
  tatlm1_15MmteValueManagement("DEFAULT_VPB0_HEIGHT",IQT_param_array[ViewFinder_Height]);

  tatlm1_15MmteValueManagement("DEFAULT_VPB1_1_WIDTH_WITH_ISPPROC",IQT_param_array[StillPicture_Width]);
  tatlm1_15MmteValueManagement("DEFAULT_VPB1_1_HEIGHT_WITH_ISPPROC",IQT_param_array[StillPicture_Height]);
 
  tatlm1_15MmteValueManagement("DEFAULT_FRAMERATE_VPB0",IQT_param_array[ViewFinder_FrameRate]);
  tatlm1_15MmteValueManagement("DEFAULT_FRIMG_FILTER_PATH_STORE_IMAGEAMERATE_VPB1",IQT_param_array[StillPicture_FrameRate]);
  tatlm1_15MmteValueManagement("DEFAULT_FRAMERATE_VPB2",IQT_param_array[Video_FrameRate]);

  tatlm1_15MmteValueManagement("DEFAULT_VPB2_WIDTH__UC_STILL",IQT_param_array[Video_Width]);
  tatlm1_15MmteValueManagement("DEFAULT_VPB2_HEIGHT__UC_STILL",IQT_param_array[Video_Height]);
 
  IqtServ_InitToIdle(fd_input,fd_popen,"Trigger detected",MMTE_TIMEOUT);
  IQT_param_array[AcquisitionControl_CurrentState]=acquisitionControl_CurrentState_Idle;

  tatlm1_117WorkAround();

  tatlm1_05InternalIqtState("IDLE");
}

void tatlm1_117WorkAround(void)
{
char command[100];

  sprintf(command,"echo flag1\n");
  IqtServ_SetMmteAlias(fd_input,fd_popen,command,MMTE_TIMEOUT);
}


int32_t tatlm1_118GetPeDthElementCounter (void)
{
	return PeDthElementCounter;
}

int32_t tatlm1_116PreTreatmentActionRawStill(struct dth_element *elem, void *Value)
{
int32_t vl_Error=TAT_ERROR_OFF;

  Value =Value;
  elem =elem;
  if(RawStillDisplayActivationON==1)
  {	
    printf("PreTreatmentRawStill\nDownload of the Raw image (5 MBytes) in progress\n");
  }
  else
  {
    /* mandatory to avoid cyclic error when the streaming control panel is opened */
    /* because this function is cyclicly called when the streaming control panel is opened !!!!*/
    vl_Error=EACCES;
  }
  return vl_Error ;
}

int32_t tatlm1_117PreTreatmentActionHrStill(struct dth_element *elem, void *Value)
{
int32_t vl_Error=TAT_ERROR_OFF;

  Value =Value;
  elem =elem;
  if(HrStillDisplayActivationON==1)
  {	
    printf("Download finished\nPreTreatmentHRStill\nDownload finished\n");
  }
  else
  {
    /* mandatory to avoid cyclic error when the streaming control panel is opened */
    /* because this function is cyclicly called when the streaming control panel is opened !!!!*/
    vl_Error=EACCES;
  }
  return vl_Error ;
}

void tatlm1_118StopStillPicture(void) 
{
  if(IQT_param_array [ViewFinder_DisplayActivation ]==ViewFinder_DisplayActivation_ON )
  {
    ViewFinderDisplayActivationON=1;
    DisplayFrameNumber=1;
    tatlm1_18Remove(ViewFinder,IQT_param_array [ViewFinder]);
  }
  StillDisplayActivationON=0;
  RawStillDisplayActivationON=0;
  HrStillDisplayActivationON=0;

  if(IQT_param_array [ViewFinder_DisplayActivation]==ViewFinder_DisplayActivation_ON)
    tatlm1_14MmteStringManagement("VF_DISPLAY_ACTIVATION","OMX_TRUE");
  else
    tatlm1_14MmteStringManagement("VF_DISPLAY_ACTIVATION","OMX_FALSE");

  if(IQT_param_array [ViewFinder_SaveActivation]==ViewFinder_SaveActivation_ON)
    tatlm1_14MmteStringManagement("VF_SAVE_ACTIVATION","OMX_TRUE");
  else
    tatlm1_14MmteStringManagement("VF_SAVE_ACTIVATION","OMX_FALSE");

  if(IQT_param_array [ViewFinder_SaveResolutionLR]==ViewFinder_SaveResolutionLR_ON )
    tatlm1_14MmteStringManagement("VF_SAVE_RESOLUTION_LR","OMX_TRUE");
  else
    tatlm1_14MmteStringManagement("VF_SAVE_RESOLUTION_LR","OMX_FALSE");

  tatlm1_15MmteValueManagement("VF_SAVE_FIRST_FRAME",IQT_param_array [ViewFinder_SaveFirstFrame]);
  tatlm1_15MmteValueManagement("VF_SAVE_FRAME_COUNT",IQT_param_array [ViewFinder_SaveFrameCount]);
  tatlm1_15MmteValueManagement("VF_SAVE_MODE",IQT_param_array [ViewFinder_SaveMode]);
				
  IqtServ_StopStillBurstPictureCapture(fd_input,fd_popen,"Trigger detected",MMTE_TIMEOUT);
  IQT_param_array[AcquisitionControl_CurrentState]=acquisitionControl_CurrentState_ViewFinder;
					
  tatlm1_117WorkAround();

  tatlm1_15MmteValueManagement("VF_FRAME_SKIPPING_NUMBER",IQT_param_array [ViewFinder_FrameSkippingNumber]);

  tatlm1_05InternalIqtState("VIEWFINDER");

  if(IQT_param_array [StillPicture_DisplayActivation ]==StillPicture_DisplayActivation_ON_LR )
  {
    tatlm1_18Remove(StillPicture,StillPicture_RemoveLr);
  }
  tatlm1_113AclManagementForTheState(acquisitionControl_CurrentState_ViewFinder);
}

int32_t tatlm1_119ActSetResolutionVF(int Value)
{

  int32_t vl_Error=TAT_ERROR_OFF;  
  	
  switch (Value){

  case ViewFinder_Resolution_SQCIF:
	IQT_param_array [ViewFinder_Width]=128;
	IQT_param_array [ViewFinder_Height]=96;
    break;
  case ViewFinder_Resolution_QQVGA:
	IQT_param_array [ViewFinder_Width]=160;
	IQT_param_array [ViewFinder_Height]=120;
    break;
  case ViewFinder_Resolution_QCIF:
	IQT_param_array [ViewFinder_Width]=176;
	IQT_param_array [ViewFinder_Height]=144;
    break;
  case ViewFinder_Resolution_QVGA:
	IQT_param_array [ViewFinder_Width]=320;
	IQT_param_array [ViewFinder_Height]=240;
    break;
  case ViewFinder_Resolution_QVGAp:
	IQT_param_array [ViewFinder_Width]=240;
	IQT_param_array [ViewFinder_Height]=320;
    break;
  case ViewFinder_Resolution_WQVGA:
	IQT_param_array [ViewFinder_Width]=400;
	IQT_param_array [ViewFinder_Height]=240;
    break;	
  case ViewFinder_Resolution_CIF:
	IQT_param_array [ViewFinder_Width]=352;
	IQT_param_array [ViewFinder_Height]=288;
    break;
  case ViewFinder_Resolution_HVGA:
	IQT_param_array [ViewFinder_Width]=480;
	IQT_param_array [ViewFinder_Height]=320;
    break;
  case ViewFinder_Resolution_nHD:
	IQT_param_array [ViewFinder_Width]=640;
	IQT_param_array [ViewFinder_Height]=360;
    break;
  case ViewFinder_Resolution_VGA:
	IQT_param_array [ViewFinder_Width]=640;
	IQT_param_array [ViewFinder_Height]=480;
    break;
  case ViewFinder_Resolution_D1:
	IQT_param_array [ViewFinder_Width]=720;
	IQT_param_array [ViewFinder_Height]=576;
    break;
  case ViewFinder_Resolution_SVGA:
	IQT_param_array [ViewFinder_Width]=800;
	IQT_param_array [ViewFinder_Height]=600;
    break;
  case ViewFinder_Resolution_WVGA:
	IQT_param_array [ViewFinder_Width]=848;
	IQT_param_array [ViewFinder_Height]=480;
    break;
  case ViewFinder_Resolution_qHD:
	IQT_param_array [ViewFinder_Width]=960;
	IQT_param_array [ViewFinder_Height]=540;
    break;
  case  ViewFinder_Resolution_XVGA:
	IQT_param_array [ViewFinder_Width]=1024;
	IQT_param_array [ViewFinder_Height]=768;
    break;
  case ViewFinder_Resolution_SXGA:
	IQT_param_array [ViewFinder_Width]=1280;
	IQT_param_array [ViewFinder_Height]=1024;
    break;
  case ViewFinder_Resolution_UXGA:
	IQT_param_array [ViewFinder_Width]=1600;
	IQT_param_array [ViewFinder_Height]=1200;
    break;
  case ViewFinder_Resolution_QXGA:
	IQT_param_array [ViewFinder_Width]=2048;
	IQT_param_array [ViewFinder_Height]=1536;
    break;
  case ViewFinder_Resolution_5MP:
	IQT_param_array [ViewFinder_Width]=2608;
	IQT_param_array [ViewFinder_Height]=1952;
    break;
		
  default:
    SYSLOG(LOG_ERR, "Resolution not supported");
    vl_Error=TAT_ERROR_CASE;
    break;	
  }		
  return vl_Error ;
}	
int32_t tatlm1_120ActSetResolutionStill(int Value)
{

  int32_t vl_Error=TAT_ERROR_OFF;  
  	
  switch (Value){

  case StillPicture_Resolution_SQCIF:
	IQT_param_array [StillPicture_Width]=128;
	IQT_param_array [StillPicture_Height]=96;
    break;
  case StillPicture_Resolution_QQVGA:
	IQT_param_array [StillPicture_Width]=160;
	IQT_param_array [StillPicture_Height]=120;
    break;
  case StillPicture_Resolution_QCIF:
	IQT_param_array [StillPicture_Width]=176;
	IQT_param_array [StillPicture_Height]=144;
    break;
  case StillPicture_Resolution_QVGA:
	IQT_param_array [StillPicture_Width]=320;
	IQT_param_array [StillPicture_Height]=240;
    break;
  case StillPicture_Resolution_QVGAp:
	IQT_param_array [StillPicture_Width]=240;
	IQT_param_array [StillPicture_Height]=320;
    break;
  case StillPicture_Resolution_WQVGA:
	IQT_param_array [StillPicture_Width]=400;
	IQT_param_array [StillPicture_Height]=240;
    break;	
  case StillPicture_Resolution_CIF:
	IQT_param_array [StillPicture_Width]=352;
	IQT_param_array [StillPicture_Height]=288;
    break;
  case StillPicture_Resolution_HVGA:
	IQT_param_array [StillPicture_Width]=480;
	IQT_param_array [StillPicture_Height]=320;
    break;
  case StillPicture_Resolution_nHD:
	IQT_param_array [StillPicture_Width]=640;
	IQT_param_array [StillPicture_Height]=360;
    break;
  case StillPicture_Resolution_VGA:
	IQT_param_array [StillPicture_Width]=640;
	IQT_param_array [StillPicture_Height]=480;
    break;
  case StillPicture_Resolution_D1:
	IQT_param_array [StillPicture_Width]=720;
	IQT_param_array [StillPicture_Height]=576;
    break;
  case StillPicture_Resolution_SVGA:
	IQT_param_array [StillPicture_Width]=800;
	IQT_param_array [StillPicture_Height]=600;
    break;
  case StillPicture_Resolution_WVGA:
	IQT_param_array [StillPicture_Width]=848;
	IQT_param_array [StillPicture_Height]=480;
    break;
  case StillPicture_Resolution_qHD:
	IQT_param_array [StillPicture_Width]=960;
	IQT_param_array [StillPicture_Height]=540;
    break;
  case  StillPicture_Resolution_XVGA:
	IQT_param_array [StillPicture_Width]=1024;
	IQT_param_array [StillPicture_Height]=768;
    break;
  case StillPicture_Resolution_SXGA:
	IQT_param_array [StillPicture_Width]=1280;
	IQT_param_array [StillPicture_Height]=1024;
    break;
  case StillPicture_Resolution_UXGA:
	IQT_param_array [StillPicture_Width]=1600;
	IQT_param_array [StillPicture_Height]=1200;
    break;
  case StillPicture_Resolution_QXGA:
	IQT_param_array [StillPicture_Width]=2048;
	IQT_param_array [StillPicture_Height]=1536;
    break;
  case StillPicture_Resolution_5MP:
	IQT_param_array [StillPicture_Width]=2608;
	IQT_param_array [StillPicture_Height]=1952;
    break;
		
  default:
    SYSLOG(LOG_ERR, "Resolution not supported");
    vl_Error=TAT_ERROR_CASE;
    break;	
  }		
  return vl_Error ;
}	

int32_t tatlm1_121ActSetResolutionVideo(int Value)
{

  int32_t vl_Error=TAT_ERROR_OFF;  
  	
  switch (Value){

  case Video_Resolution_SQCIF:
	IQT_param_array [Video_Width]=128;
	IQT_param_array [Video_Height]=96;
    break;
  case Video_Resolution_QQVGA:
	IQT_param_array [Video_Width]=160;
	IQT_param_array [Video_Height]=120;
    break;
  case Video_Resolution_QCIF:
	IQT_param_array [Video_Width]=176;
	IQT_param_array [Video_Height]=144;
    break;
  case Video_Resolution_QVGA:
	IQT_param_array [Video_Width]=320;
	IQT_param_array [Video_Height]=240;
    break;
  case Video_Resolution_QVGAp:
	IQT_param_array [Video_Width]=240;
	IQT_param_array [Video_Height]=320;
    break;
  case Video_Resolution_WQVGA:
	IQT_param_array [Video_Width]=400;
	IQT_param_array [Video_Height]=240;
    break;	
  case Video_Resolution_CIF:
	IQT_param_array [Video_Width]=352;
	IQT_param_array [Video_Height]=288;
    break;
  case Video_Resolution_HVGA:
	IQT_param_array [Video_Width]=480;
	IQT_param_array [Video_Height]=320;
    break;
  case Video_Resolution_nHD:
	IQT_param_array [Video_Width]=640;
	IQT_param_array [Video_Height]=360;
    break;
  case Video_Resolution_VGA:
	IQT_param_array [Video_Width]=640;
	IQT_param_array [Video_Height]=480;
    break;
  case Video_Resolution_D1:
	IQT_param_array [Video_Width]=720;
	IQT_param_array [Video_Height]=576;
    break;
  case Video_Resolution_SVGA:
	IQT_param_array [Video_Width]=800;
	IQT_param_array [Video_Height]=600;
    break;
  case Video_Resolution_WVGA:
	IQT_param_array [Video_Width]=848;
	IQT_param_array [Video_Height]=480;
    break;
  case Video_Resolution_qHD:
	IQT_param_array [Video_Width]=960;
	IQT_param_array [Video_Height]=540;
    break;
  case  Video_Resolution_XVGA:
	IQT_param_array [Video_Width]=1024;
	IQT_param_array [Video_Height]=768;
    break;
  case Video_Resolution_SXGA:
	IQT_param_array [Video_Width]=1280;
	IQT_param_array [Video_Height]=1024;
    break;
  case Video_Resolution_UXGA:
	IQT_param_array [Video_Width]=1600;
	IQT_param_array [Video_Height]=1200;
    break;
  case Video_Resolution_QXGA:
	IQT_param_array [Video_Width]=2048;
	IQT_param_array [Video_Height]=1536;
    break;
  case Video_Resolution_5MP:
	IQT_param_array [Video_Width]=2608;
	IQT_param_array [Video_Height]=1952;
    break;
		
  default:
    SYSLOG(LOG_ERR, "Resolution not supported");
    vl_Error=TAT_ERROR_CASE;
    break;	
  }		
  return vl_Error ;
}	
