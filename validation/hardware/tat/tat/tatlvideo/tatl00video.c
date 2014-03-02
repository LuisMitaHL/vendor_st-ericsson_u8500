/*
* =====================================================================================
* 
*        Filename:  tatl00video.c
* 
*     Description:  
* 
*         Version:  1.0
*         Created:  27/08/2009 
*        Revision:  none
*        Compiler:  
* 
*          Author: GVO, 
*         � Copyright ST-Ericsson, 2009. All Rights Reserved
* 
* =====================================================================================
*/

#define TATLVIDEO_C
#include "dthvideo.h"
#include "tatlvideo.h"
#include "CameraServices.h"
#undef  TATLVIDEO_C

/*Init default value for VIDEO */
/****************************/
int dth_init_service()
{	    
static char *default_pathname = "/mnt/UserFS2/";
static char *default_pict1path = "/usr/share/multimediaservices/videoservices/pattern8888.rgb";
static char *default_pict2path = "/usr/share/multimediaservices/videoservices/st-ericsson_864x480.png";
int result = 0;
 
	result = tatlvideo_register_dth_struct();

  /* CAMERA */
	VIDEO_param_array [IN_SENSOR_ID]=0;

	VIDEO_param_array [Set_Video_Resolution]=Set_Video_Resolution_VGA;
	VIDEO_param_array [IN_VIDEO_WIDTH]=640;
	VIDEO_param_array [IN_VIDEO_HEIGHT]=480;
	VIDEO_param_array [IN_VIDEO_FRAMERATE]=22;
 
	VIDEO_param_array [Set_Preview_Resolution]=Set_Preview_Resolution_WVGAp;
	VIDEO_param_array [IN_PREVIEW_ID]=0;
	VIDEO_param_array [IN_PREVIEW_WIDTH]=480;
	VIDEO_param_array [IN_PREVIEW_HEIGHT]=848;
	VIDEO_param_array [IN_PREVIEW_FRAMERATE]=22;

	VIDEO_param_array [IN_PREVIEW_SCAN_MODE]=0;
	VIDEO_param_array [IN_PREVIEW_DISPLAY_WIDTH]=720;
	VIDEO_param_array [IN_PREVIEW_DISPLAY_HEIGHT]=576;
	VIDEO_param_array [IN_PREVIEW_DISPLAY_FRAMERATE]=50;

	VIDEO_param_array [Set_Picture_Resolution]=Set_Picture_Resolution_VGA;
	VIDEO_param_array [IN_PICTURE_FORMAT]=0;
	VIDEO_param_array [IN_PICTURE_WIDTH]=640;
	VIDEO_param_array [IN_PICTURE_HEIGHT]=480;
	VIDEO_param_array [IN_PICTURE_FRAMERATE]=22;

	VIDEO_param_array [IN_PICTURE_FINITEBURSTFRAME]=2;
        VIDEO_string_array[IN_PICTURE_PATH]=(int8_t*)calloc(strlen(default_pathname)+1,sizeof(int8_t));
        strncpy((char*)VIDEO_string_array[IN_PICTURE_PATH],(const char*)default_pathname,strlen(default_pathname));

	VIDEO_param_array[CURRENT_STATE]=CURRENT_STATE_INIT;
	
  /*DISPLAY*/
  
  VIDEO_string_array[IN_DISPLAY_PATH]=(int8_t*)calloc(strlen(default_pict1path)+1,sizeof(int8_t));
  strncpy((char*)VIDEO_string_array[IN_DISPLAY_PATH],(const char*)default_pict1path,strlen(default_pict1path));  
  VIDEO_param_array [IN_PICTDISPLAY_FORMAT]=DTHVIDEO_ARGB8888;
  VIDEO_param_array [IN_PICTDISPLAY_WIDTH]=480;
  VIDEO_param_array [IN_PICTDISPLAY_HEIGHT]=864;
  VIDEO_param_array [IN_PICTDISPLAY_ROTATION]=DTHVIDEO_ROTATE_CW;
  VIDEO_string_array [IN_DISPLAY2_PATH]=(int8_t*)calloc(strlen(default_pict2path)+1,sizeof(int8_t));
  strncpy((char*)VIDEO_string_array[IN_DISPLAY2_PATH],(const char*)default_pict2path,strlen(default_pict2path));  
  VIDEO_param_array [IN_PICTDISPLAY2_FORMAT]=DTHVIDEO_PNG;
  VIDEO_param_array [IN_PICTDISPLAY2_WIDTH]=864;
  VIDEO_param_array [IN_PICTDISPLAY2_HEIGHT]=480;
  VIDEO_param_array [IN_PICTDISPLAY2_ROTATION]=DTHVIDEO_ROTATE_UR;
  VIDEO_param_array [IN_PATTERNSIZE]=20;
  VIDEO_param_array [IN_PATTERNROTATION]=DTHVIDEO_ROTATE_CW;
  VIDEO_param_array [IN_PATTERN_WIDTH]=480;
  VIDEO_param_array [IN_PATTERN_HEIGHT]=864;

	return result ;
}


int Dth_ActVIDEO_exec(struct dth_element *elem)
{
        return (tatlm1_00ActExec(elem)) ;	
}

int Dth_ActVIDEOParam_Set(struct dth_element *elem, void *value)
{
       return (tatlm1_02ActSet(elem,value)); 	
}

int Dth_ActVIDEOParam_Get(struct dth_element *elem, void *value)
{
       return (tatlm1_03ActGet(elem,value));	
}

int Dth_ElemVIDEOParam_Set(struct dth_element *elem, void *value)
{	
        return (tatlm1_04ActSetDynamic(elem,value));	
}



