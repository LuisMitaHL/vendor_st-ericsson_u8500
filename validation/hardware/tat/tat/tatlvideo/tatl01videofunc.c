/*
* =====================================================================================
* 
*        Filename:  tatl01videofunc.c
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
  
#define TATL01VIDEOFUNC_C
#include"tatlvideo.h"
#include "CameraServices.h"
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#undef TATL01VIDEOFUNC_C
  
#define MMTE_TIMEOUT 10000
#define MMTE_TIMEOUT_FOR_STILL 4200

extern int32_t tatlm1_108register_pe_dth_struct(char *mapfile);
extern int32_t tatlm1_118GetPeDthElementCounter (void);
extern uint32_t  tatlm1_109InitFileDescriptors(int camera_fd_input,int camera_fd_popen);
#define MAP_FILE			"/usr/share/mm-valid/imaging/isp_firmware/isp8500_firmware_IMX072/device_params/V2WReg.map"
/*----------------------------------------------------------------------*
* Procedure    : tatlm1_SetColorBuffer
*----------------------------------------------------------------------*
* Description  : Generate the RGB888_32 bits buffer for the color pattern 
*----------------------------------------------------------------------*
* Parameter(s) : s_DisplayPattern *, the DisplayPattern structure
*                u32, the number of pixels for the pattern
*----------------------------------------------------------------------*
* Return Value : None
*----------------------------------------------------------------------*/
void tatlm1_SetColorBuffer (t_tatvideo_DisplayPattern* pp_DisplayPattern, uint32_t vp_NbPixels ) /* RGB888 on 32 bits XXRRGGBB */ 
{
  uint32_t i; 

  for (i = 0;i<vp_NbPixels;i++)
  {    
    pp_DisplayPattern->p_RGB32BitBuffer[i]=pp_DisplayPattern->v_ColorDef1;
  }   
}


/*----------------------------------------------------------------------*
* Procedure    : tatm1_SetChessBuffer
*----------------------------------------------------------------------*
* Description  : Generate the RGB888_32 bits buffer for the chess pattern 
*----------------------------------------------------------------------*
* Parameter(s) : s_DisplayPattern *, the DisplayPattern structure
*----------------------------------------------------------------------*
* Return Value : None
*----------------------------------------------------------------------*/
void tatm1_SetChessBuffer ( t_tatvideo_DisplayPattern* pp_DisplayPattern)
{
  uint32_t i;  
  uint32_t j;
  uint32_t vl_ColorOdd ;
  uint32_t vl_ColorEven ;

  for (i = 0 ; i < pp_DisplayPattern->v_Lines ; i ++)
  {
    if (((i / pp_DisplayPattern->v_DisplayParam)% 2) == 1)
    {
      vl_ColorOdd = pp_DisplayPattern->v_ColorDef1;
      vl_ColorEven = pp_DisplayPattern->v_ColorDef2;
    }
    else
    {
      vl_ColorOdd = pp_DisplayPattern->v_ColorDef2;
      vl_ColorEven = pp_DisplayPattern->v_ColorDef1;
    }
    for (j = 0 ; j < pp_DisplayPattern->v_Columns ; j ++)
    {
      if (((j / pp_DisplayPattern->v_DisplayParam)% 2) == 1)
      {
        pp_DisplayPattern->p_RGB32BitBuffer[ ((i * pp_DisplayPattern->v_Columns) + j)] = vl_ColorOdd;
      }
      else
      {
        pp_DisplayPattern->p_RGB32BitBuffer[ ((i * pp_DisplayPattern->v_Columns) + j)]=vl_ColorEven;

      }
    }
  }
}


/*---------------------------------------------------------------------------------*
* Procedure    : tatm1_SetGrayScaleBuffer
*---------------------------------------------------------------------------------*
* Description  : Generate the RGB888_32 bits buffer for the gray scale pattern 
*---------------------------------------------------------------------------------*
* Parameter(s) : s_DisplayPattern *, the DisplayPattern structure
*---------------------------------------------------------------------------------*
* Return Value : None
*---------------------------------------------------------------------------------*/
void tatm1_SetGrayScaleBuffer (t_tatvideo_DisplayPattern* pp_DisplayPattern)
{  
  uint32_t i;  
  uint32_t j;
  uint32_t vl_GrayColor = 0xFF000000U ;
  uint32_t vl_GrayScaleColumn = 0, vl_FirstColumn = 0, vl_Step;

  vl_Step = pp_DisplayPattern->v_Columns/8;

  for (i = 0;i<pp_DisplayPattern->v_Lines;i++)
  {
    /* for 8 columns of gray */
    for (vl_GrayScaleColumn = 0 ; vl_GrayScaleColumn< 8 ; vl_GrayScaleColumn++)
    {
      /* For each column of gray */
      for (j = vl_FirstColumn ; j<(vl_FirstColumn+vl_Step) ; j++)
      {
        pp_DisplayPattern->p_RGB32BitBuffer[  ((i * pp_DisplayPattern->v_Columns) + j)] = vl_GrayColor;
      }
      /* update for the next column of gray */
      if (vl_GrayScaleColumn == 7)
      {
        vl_GrayColor = 0xFFFFFFFFU;    /* last column is white */	
      }
      else
      {
        vl_GrayColor = vl_GrayColor + 0x00202020U;
      }
      vl_FirstColumn = vl_FirstColumn + vl_Step;
    }
    /* init for the next line */
    vl_GrayColor = 0xFF000000U;
    vl_FirstColumn = 0;     	  
  }
}


/*---------------------------------------------------------------------------------*
* Procedure    : tatm1_SetCrossBuffer
*---------------------------------------------------------------------------------*
* Description  : Generate the RGB888_32 bits buffer for the cross pattern 
*---------------------------------------------------------------------------------*
* Parameter(s) : s_DisplayPattern *, the DisplayPattern structure
*---------------------------------------------------------------------------------*
* Return Value : None
*---------------------------------------------------------------------------------*/
void tatm1_SetCrossBuffer ( t_tatvideo_DisplayPattern* pp_DisplayPattern)  
{
  uint32_t i;  
  uint32_t j;
  uint16_t vl_YCross = pp_DisplayPattern->v_Lines/2, vl_XCross = pp_DisplayPattern->v_Columns/2 ;

  for (i = 0;i<pp_DisplayPattern->v_Lines;i++)
  {
    for (j = 0;j<pp_DisplayPattern->v_Columns;j++)
    {
      pp_DisplayPattern->p_RGB32BitBuffer[(i*pp_DisplayPattern->v_Columns)+j] = pp_DisplayPattern->v_ColorDef1 ; /* background */
    }
  }

  for (j = 0;j<pp_DisplayPattern->v_Columns;j++)
  {
    pp_DisplayPattern->p_RGB32BitBuffer[(vl_YCross*pp_DisplayPattern->v_Columns)+j] = pp_DisplayPattern->v_ColorDef2; /* horizontal axis  */
  }


  for (i = 0;i<pp_DisplayPattern->v_Lines;i++)
  {
    pp_DisplayPattern->p_RGB32BitBuffer[(i*pp_DisplayPattern->v_Columns)+vl_XCross] = pp_DisplayPattern->v_ColorDef2; /* vertical axis */ 
  }

}


/*------------------------------------------------------------------------------------*
* Procedure    : tatm1_SetAlignBuffer
*------------------------------------------------------------------------------------*
* Description  : Generate the RGB888_32 bits buffer for the Alignment pattern 
*------------------------------------------------------------------------------------*
* Parameter(s) : s_DisplayPattern *, the DisplayPattern structure
*------------------------------------------------------------------------------------*
* Return Value : None
*------------------------------------------------------------------------------------*/
void tatm1_SetAlignBuffer (t_tatvideo_DisplayPattern* pp_DisplayPattern) 
{
  uint32_t i;  
  uint32_t j;
  uint16_t vl_Lines = pp_DisplayPattern->v_Lines, vl_Columns = pp_DisplayPattern->v_Columns ;
  uint16_t vl_VisibleLines = pp_DisplayPattern->v_Lines, vl_VisibleColumns = pp_DisplayPattern->v_Columns;
  uint32_t vp_BorderSize = pp_DisplayPattern->v_DisplayParam;
  uint32_t vp_Color1 = pp_DisplayPattern->v_ColorDef1;
  uint32_t vp_Color2 = pp_DisplayPattern->v_ColorDef2;

  for (i = 0;i<vl_Lines;i++)
  {
    for (j = 0;j<vl_Columns;j++)
    {

      if ((i<vp_BorderSize)  /* "vp_BorderSize" first lines */
        ||( (((uint32_t)(vl_VisibleLines-1-vp_BorderSize))<i) && (i<=(((uint32_t)vl_VisibleLines-1)))) /* "vp_BorderSize" last lines */ )
      {		  
        pp_DisplayPattern->p_RGB32BitBuffer[((i * vl_Columns) + j)]=vp_Color1;
      }
      else
      {
        if ((j<vp_BorderSize) /* "vp_BorderSize" first columns */
          ||( (((uint32_t)(vl_VisibleColumns-1-vp_BorderSize))<j) && (j<=((uint32_t)(vl_VisibleColumns-1)))) /* "vp_BorderSize" last columns */)
        {
          pp_DisplayPattern->p_RGB32BitBuffer[((i * vl_Columns) + j)]=vp_Color1;
        }
        else
        {  /* others pixels */
          pp_DisplayPattern->p_RGB32BitBuffer[ ((i * vl_Columns) + j)]=vp_Color2;
        }		
      }
    }
  }
}

/*------------------------------------------------------------------------------------*
* Procedure    : tatm1_SetVerticalColorBarBuffer
*------------------------------------------------------------------------------------*
* Description  : Generate the RGB888_32 bits buffer for the VerticalColor Bar pattern 
*------------------------------------------------------------------------------------*
* Parameter(s) : s_DisplayPattern *, the DisplayPattern structure
*------------------------------------------------------------------------------------*
* Return Value : None
*------------------------------------------------------------------------------------*/
void tatm1_SetVerticalColorBarBuffer (t_tatvideo_DisplayPattern* pp_DisplayPattern) 
{
  uint32_t i;  
  uint32_t j;
  uint16_t vl_Lines =pp_DisplayPattern->v_Lines, vl_Columns = pp_DisplayPattern->v_Columns ;
  uint32_t vl_White=0x00ffffff;
  uint32_t vl_Yellow=0x00ffff00;
  uint32_t vl_Cyan=0x0000ffff;
  uint32_t vl_Green=0x0000ff00;
  uint32_t vl_Magenta=0x00ff00ff;
  uint32_t vl_Red=0x00ff0000;
  uint32_t vl_Blue=0x000000ff;
  uint32_t vl_Black=0x00000000;

  for (i=0;i<vl_Lines;i++)
  {
    for (j=0;j<vl_Columns;j++)
    {
      if (j<(vl_Columns/8))
      {
        pp_DisplayPattern->p_RGB32BitBuffer[((i * vl_Columns) + j)]=vl_White;
      }
      else if (((vl_Columns/8)<=j)&&(j<((vl_Columns/8)*2)))
      {
        pp_DisplayPattern->p_RGB32BitBuffer[((i * vl_Columns) + j)]=vl_Yellow;
      }
      else if ((((vl_Columns/8)*2)<=j)&&(j<((vl_Columns/8)*3)))
      {
        pp_DisplayPattern->p_RGB32BitBuffer[((i * vl_Columns) + j)]=vl_Cyan;
      }
      else if ((((vl_Columns/8)*3)<=j)&&(j<((vl_Columns/8)*4)))
      {
        pp_DisplayPattern->p_RGB32BitBuffer[((i * vl_Columns) + j)]=vl_Green;
      }		
      else if ((((vl_Columns/8)*4)<=j)&&(j<((vl_Columns/8)*5)))
      {
        pp_DisplayPattern->p_RGB32BitBuffer[((i * vl_Columns) + j)]=vl_Magenta;
      }		
      else if ((((vl_Columns/8)*5)<=j)&&(j<((vl_Columns/8)*6)))
      {
        pp_DisplayPattern->p_RGB32BitBuffer[((i * vl_Columns) + j)]=vl_Red;
      }
      else if ((((vl_Columns/8)*6)<=j)&&(j<((vl_Columns/8)*7)))
      {
        pp_DisplayPattern->p_RGB32BitBuffer[((i * vl_Columns) + j)]=vl_Blue;
      }		
      else 
      {
        pp_DisplayPattern->p_RGB32BitBuffer[((i * vl_Columns) + j)]=vl_Black;
      }		
    }	
  }
}

/*---------------------------------------------------------------------------------*
* Procedure    : tatlm1_CreatePatternImage
*---------------------------------------------------------------------------------*
* Description  : fill the RGB888_32 bits buffer of the DisplayPattern structure
*---------------------------------------------------------------------------------*
* Parameter(s) : s_DisplayPattern *, the DisplayPattern structure
*---------------------------------------------------------------------------------*
* Return Value : u32, TAT error
*---------------------------------------------------------------------------------*/
int32_t tatlm1_CreatePatternImage(t_tatvideo_DisplayPattern* pp_DisplayPattern)
{
  int32_t vl_Error = TAT_ERROR_OFF;
  uint32_t vl_NbPixels = 0;
  uint32_t i = 0;
  uint8_t vl_alpha = 0;
  uint8_t vl_red = 0;
  uint8_t vl_grenn = 0;
  uint8_t vl_blue = 0;
  FILE * pFile;

  printf ("tatlm1_06CreatePatternImage  v_type%d v_ColorDef1 0x%x v_ColorDef2 0x%x v_Lines%d v_Columns%d v_DisplayParam%d\n",pp_DisplayPattern->v_type,pp_DisplayPattern->v_ColorDef1,pp_DisplayPattern->v_ColorDef2,pp_DisplayPattern->v_Lines,pp_DisplayPattern->v_Columns,pp_DisplayPattern->v_DisplayParam);
  vl_NbPixels = pp_DisplayPattern->v_Lines*pp_DisplayPattern->v_Columns;
  if (vl_NbPixels == 0)
  {
    printf ("tatlm1_06CreatePatternImage TYPE vl_NbPixels=0 check WIDTH and HEIGH of picture\n");
    return TAT_BAD_REQ; 
  }

  pp_DisplayPattern->p_RGB32BitBuffer = (uint32_t*)calloc(vl_NbPixels,sizeof(uint32_t));
  pp_DisplayPattern->p_RGB8BitBuffer = (uint8_t*)calloc(vl_NbPixels*4,sizeof(uint8_t));
  
  pp_DisplayPattern->v_BufferSize = vl_NbPixels;

  switch (pp_DisplayPattern->v_type)
  {         
  case TATVIDEO_COLOR:	 
    tatlm1_SetColorBuffer(pp_DisplayPattern, vl_NbPixels);	      
    break;		
  case TATVIDEO_CHESS:
    if (pp_DisplayPattern->v_DisplayParam == 0)
    {
      printf ("TAT Display Pattern :  size couldn't be 0 \n");
      vl_Error = TAT_BAD_REQ; 
    }
    else
    {
      tatm1_SetChessBuffer (pp_DisplayPattern);
    }
    break;  
  case TATVIDEO_GRAY_SCALE:
    tatm1_SetGrayScaleBuffer (pp_DisplayPattern);
    break; 
  case TATVIDEO_CENTER:
    tatm1_SetCrossBuffer (pp_DisplayPattern);
    break; 
  case TATVIDEO_ALIGN:
    if (pp_DisplayPattern->v_DisplayParam==0)
    {
      printf ("TAT Display Pattern :  size couldn't be 0 \n");
      vl_Error= TAT_BAD_REQ; 
    }
    else
    {
      tatm1_SetAlignBuffer (pp_DisplayPattern);
    }
    break; 
  case TATVIDEO_VERTCOLORBAR:
    tatm1_SetVerticalColorBarBuffer(pp_DisplayPattern);
    break; 	
  default:
    free(pp_DisplayPattern->p_RGB8BitBuffer);
    free(pp_DisplayPattern->p_RGB32BitBuffer);
    vl_Error = TAT_ERROR_CASE;
    break;
  }

  if(vl_Error==TAT_ERROR_OFF)
  {
    /*RGB888PICTURE*/
    pFile = fopen( "/usr/share/multimediaservices/videoservices/pattern8888.rgb","wb");
    if (pFile!=NULL)
    {
      for (i=0;i<vl_NbPixels;i++)
      {
        
        vl_alpha=(uint8_t)(pp_DisplayPattern->p_RGB32BitBuffer[i]>>24);
        vl_red=(uint8_t)(pp_DisplayPattern->p_RGB32BitBuffer[i]>>16);
        vl_grenn=(uint8_t)(pp_DisplayPattern->p_RGB32BitBuffer[i]>>8);
        vl_blue=(uint8_t)(pp_DisplayPattern->p_RGB32BitBuffer[i]);
        
        pp_DisplayPattern->p_RGB8BitBuffer[(i*4)+3]=vl_alpha;
        pp_DisplayPattern->p_RGB8BitBuffer[(i*4)+2]=vl_red;
        pp_DisplayPattern->p_RGB8BitBuffer[(i*4)+1]=vl_grenn;
        pp_DisplayPattern->p_RGB8BitBuffer[(i*4)+0]=vl_blue;
        
        fwrite(&pp_DisplayPattern->p_RGB8BitBuffer[(i*4)+0], 1, 1, pFile); //A
        fwrite(&pp_DisplayPattern->p_RGB8BitBuffer[(i*4)+1], 1, 1, pFile); //R
        fwrite(&pp_DisplayPattern->p_RGB8BitBuffer[(i*4)+2], 1, 1, pFile); //G
        fwrite(&pp_DisplayPattern->p_RGB8BitBuffer[(i*4)+3], 1, 1, pFile); //B
      }		
      pp_DisplayPattern->v_BufferSize=4*vl_NbPixels;			
      fclose(pFile);
    }
  }

  /*RGB565PICTURE OK*/
  /*pFile = fopen( "/share/multimediaservices/videoservices/pattern565.rgb","wb");
  for (i=0;i<vl_NbPixels;i++)
  {
  vl_red5bits=(uint8_t)((pp_DisplayPattern->p_RGB32BitBuffer[i]>>16)&0x1f);
  vl_grenn6bits=(uint8_t)((pp_DisplayPattern->p_RGB32BitBuffer[i]>>8)&0x3f);
  vl_blue5bits=(uint8_t)(pp_DisplayPattern->p_RGB32BitBuffer[i]&0x1f);
  //RGB + SWAP
  pp_DisplayPattern->p_RGB8BitBuffer[(i*2)+1]=(vl_red5bits<<3)|(vl_grenn6bits>>3);
  fwrite(&pp_DisplayPattern->p_RGB8BitBuffer[(i*2)+1], 1, 1, pFile);
  pp_DisplayPattern->p_RGB8BitBuffer[(i*2)]=(vl_grenn6bits<<5)|vl_blue5bits;
  fwrite(&pp_DisplayPattern->p_RGB8BitBuffer[(i*2)], 1, 1, pFile);
  }		
  pp_DisplayPattern->v_BufferSize=2*vl_NbPixels;		
  fclose(pFile);
  */


  return vl_Error ;
}

/*---------------------------------------------------------------------------------*
* Procedure    : tatlm1_ManageDisplay
*---------------------------------------------------------------------------------*
* Description  : call videoservices functions
*---------------------------------------------------------------------------------*
* Parameter(s) : u32, vp_userdata: Action from DTH 
*---------------------------------------------------------------------------------*
* Return Value : u8, TAT error
*---------------------------------------------------------------------------------*/
int32_t tatlm1_ManageDisplay (int32_t vp_userdata,t_tatvideo_DisplayManagement sp_DisplayManagement)
{
  int32_t vl_Error = TAT_ERROR_OFF;
  e_VidServ_PowerMode_t v_PowerMode;
  uint32_t v_IsPlugged=0;
  t_VidServ_DisplayResolution s_DisplayResolution; 
  
  t_VidServ_ImageProperties s_Pict1Properties,s_Pict2Properties; 
  t_VidServ_PictureResolution  s_InputPictureResolution, s_InputPictureResolution2; 
  uint32_t v_Pict1Duration=0;
  uint32_t v_Pict2Duration=0;
  e_VidServ_Rotation_t v_Pict1Rotation,v_Pict2Rotation;
  e_VidServ_Rotation_t v_PatternRotation;
  e_VidServ_LoopMode_t v_LoopMode;  

  t_tatvideo_DisplayPattern s_DisplayPattern; 

  e_VidServ_DisplayId_t v_DisplayId=sp_DisplayManagement.v_DisplayID;

  switch(vp_userdata) { 	
  case ACT_MAINDISPLAY_INIT:
  case ACT_CVBSDISPLAY_INIT:
  case ACT_HDMIDISPLAY_INIT:
    vl_Error = VidServ_DisplayInit (v_DisplayId);
      if (vl_Error != VIDSERV_NO_ERROR) {
    	  printf ("Display init VidServ_DisplayInit  vl_Error: %d\n", vl_Error);	 
      }
      else {
    	  vl_Error = VidServ_DisplayGetProperties (v_DisplayId, &s_DisplayResolution); 
	      if (vl_Error != VIDSERV_NO_ERROR) {
    		  printf ("Display init VidServ_DisplayGetProperties  vl_Error: %d\n", vl_Error);	 
    	  }
    	  else {
		      VIDEO_param_array [OUT_DISPLAY_WIDTH]=s_DisplayResolution.Width;
	        VIDEO_param_array [OUT_DISPLAY_HEIGHT]=s_DisplayResolution.Height;
	        VIDEO_param_array [OUT_DISPLAY_DEPTH]=s_DisplayResolution.Depth;
	      }
      }
      break;

  case ACT_SET_MAINDISPWRMODE:
  case ACT_SET_CVBSDISPWRMODE:
  case ACT_SET_HDMIDISPWRMODE:
    v_PowerMode = sp_DisplayManagement.v_Option;
    if(v_PowerMode==DTHVIDEO_OFF) v_PowerMode=POWER_OFF;
    else v_PowerMode=POWER_ON;
    vl_Error = VidServ_DisplaySetPowerMode (v_DisplayId, v_PowerMode);
    if (vl_Error != VIDSERV_NO_ERROR)
    {
    	printf ("Display init VidServ_DisplaySetPowerMode vl_Error: %d\n", vl_Error);	 
    }
    break;

  case ACT_SET_CVBSDISPROP:
  case ACT_SET_HDMIDISPROP:
    s_DisplayResolution.Width = VIDEO_param_array [IN_TVODISPLAY_WIDTH];
    s_DisplayResolution.Height = VIDEO_param_array [IN_TVODISPLAY_HEIGHT];
    s_DisplayResolution.Frequency = VIDEO_param_array [IN_TVODISPLAY_FREQUENCY];
    s_DisplayResolution.ScanMode = VIDEO_param_array [IN_SCANMODE];
    vl_Error = VidServ_DisplaySetProperties (v_DisplayId, &s_DisplayResolution);
    if (vl_Error != VIDSERV_NO_ERROR)
    {
    	printf ("Display init VidServ_DisplaySetProperties vl_Error: %d\n", vl_Error);	 
    }
    break;

  case ACT_MAINDISPLAYPICTURE:
  case ACT_CVBSDISPLAYPICTURE:
  case ACT_HDMIDISPLAYPICTURE:	
    if(VIDEO_param_array[vp_userdata] == DTHSTARTDISPLAY)
    {		
      /*Only picture from embedded can be selected, not from PC side*/
      s_Pict1Properties.FromFile = 1;
      s_Pict1Properties.BufferSize = 0;
      s_Pict1Properties.BufferAddressOrFilePath = (uint8_t*)VIDEO_string_array[IN_DISPLAY_PATH];
      s_Pict1Properties.InputPictureFormat = VIDEO_param_array [IN_PICTDISPLAY_FORMAT];
      
      s_InputPictureResolution.Width = VIDEO_param_array [IN_PICTDISPLAY_WIDTH];
      s_InputPictureResolution.Height = VIDEO_param_array [IN_PICTDISPLAY_HEIGHT];
      
      s_Pict1Properties.InputPictureResolution = s_InputPictureResolution;
      v_Pict1Duration = VIDEO_param_array [IN_PICTDISPLAY_DURATION];
      v_Pict1Rotation = VIDEO_param_array [IN_PICTDISPLAY_ROTATION];

      s_Pict2Properties.FromFile = 1;
      s_Pict2Properties.BufferSize = 0;
      s_Pict2Properties.BufferAddressOrFilePath = (uint8_t*)VIDEO_string_array[IN_DISPLAY2_PATH];
      s_Pict2Properties.InputPictureFormat = VIDEO_param_array [IN_PICTDISPLAY2_FORMAT];
      
      s_InputPictureResolution2.Width = VIDEO_param_array [IN_PICTDISPLAY2_WIDTH];
      s_InputPictureResolution2.Height = VIDEO_param_array [IN_PICTDISPLAY2_HEIGHT];
      
      s_Pict2Properties.InputPictureResolution = s_InputPictureResolution2;  
      v_Pict2Duration = VIDEO_param_array [IN_PICTDISPLAY2_DURATION];
      v_Pict2Rotation = VIDEO_param_array [IN_PICTDISPLAY2_ROTATION];

      v_LoopMode = VIDEO_param_array [IN_LOOPMODE];

      vl_Error = VidServ_DisplayPicture(v_DisplayId, &s_Pict1Properties, v_LoopMode, v_Pict1Duration, v_Pict1Rotation, &s_Pict2Properties, v_Pict2Duration,v_Pict2Rotation);
      if (vl_Error != VIDSERV_NO_ERROR)
      {
        printf ("Display picture VidServ_DisplayPicture  vl_Error: %d\n", vl_Error);	 
      }
    }
    else
    {
	vl_Error = VidServ_StopDisplayPictureLoop(v_DisplayId);
        if(vl_Error != VIDSERV_NO_ERROR)
      	{
		if(0==VIDEO_param_array [IN_LOOPMODE])
		{
			vl_Error=VIDSERV_NO_ERROR;
		}
      		printf ("Display picture VidServ_StopDisplayPictureLoop  vl_Error:: %d\n", vl_Error);	 
    	}
    }
    break;
  case ACT_MAINDISPLAYPATTERN:
  case ACT_CVBSDISPLAYPATTERN:
  case ACT_HDMIDISPLAYPATTERN:
    v_PatternRotation = VIDEO_param_array[IN_PATTERNROTATION];
    s_DisplayPattern.v_type = sp_DisplayManagement.v_Option;
    
    /*
    vl_Error = VidServ_DisplayGetProperties (v_DisplayId, &s_DisplayResolution);
    if(vl_Error != VIDSERV_NO_ERROR)
    {
    	printf ("DISPLAYPATTERN VidServ_StopDisplayPictureLoop  vl_Error: %d\n", vl_Error);	 
    }	 
    else
	{
	    s_DisplayPattern.v_Columns = s_DisplayResolution.Width;
	    s_DisplayPattern.v_Lines = s_DisplayResolution.Height;
	    */
	    s_DisplayPattern.v_Columns = VIDEO_param_array[IN_PATTERN_WIDTH];
	    s_DisplayPattern.v_Lines = VIDEO_param_array[IN_PATTERN_HEIGHT];
	    
	    /* RGB888 on 32 bits XXRRGGBB */ 
	    s_DisplayPattern.v_ColorDef1 = ((VIDEO_param_array[IN_PATTERNALPHA1]<<24)|(VIDEO_param_array[IN_PATTERNRED1]<<16)|(VIDEO_param_array[IN_PATTERNGREEN1]<<8)|(VIDEO_param_array[IN_PATTERNBLUE1]));
	    s_DisplayPattern.v_ColorDef2 = ((VIDEO_param_array[IN_PATTERNALPHA2]<<24)|(VIDEO_param_array[IN_PATTERNRED2]<<16)|(VIDEO_param_array[IN_PATTERNGREEN2]<<8)|(VIDEO_param_array[IN_PATTERNBLUE2]));
	    s_DisplayPattern.v_DisplayParam = VIDEO_param_array [IN_PATTERNSIZE];

	    vl_Error = tatlm1_CreatePatternImage(&s_DisplayPattern);
	    if (vl_Error == TAT_ERROR_OFF)
	    {
	      s_Pict1Properties.FromFile = 0;
	      s_Pict1Properties.BufferSize = s_DisplayPattern.v_BufferSize;
	      s_Pict1Properties.BufferAddressOrFilePath = s_DisplayPattern.p_RGB8BitBuffer;
	      s_Pict1Properties.InputPictureFormat = ARGB8888;
	      s_InputPictureResolution.Width = s_DisplayResolution.Width;
	      s_InputPictureResolution.Height = s_DisplayResolution.Height;
	      s_Pict1Properties.InputPictureResolution = s_InputPictureResolution;
	      v_LoopMode = NO_LOOP ;
	      vl_Error = VidServ_DisplayPicture(v_DisplayId, &s_Pict1Properties, v_LoopMode,0, v_PatternRotation, &s_Pict2Properties,0, v_PatternRotation);
	      if (vl_Error != VIDSERV_NO_ERROR)
	      {
		printf ("DISPLAYPATTERN VidServ_DisplayPicture  vl_Error: %d\n", vl_Error);	 
	      }
	      free(s_DisplayPattern.p_RGB8BitBuffer);
	      free(s_DisplayPattern.p_RGB32BitBuffer);
	    }
	//}
    break;	
  case ACT_CVBS_DETECT:
  case ACT_HDMI_DETECT:
    vl_Error = VidServ_DisplayIsPlugged (v_DisplayId, &v_IsPlugged);
    if (vl_Error != VIDSERV_NO_ERROR)
    {
	printf ("Display detect VidServ_DisplayIsPlugged  vl_Error: %d\n", vl_Error);	 
    }
    else 
    {
	VIDEO_param_array [OUT_TVODETECT_STATE]=v_IsPlugged;
    }
    break;	
  default:
    vl_Error = TAT_BAD_REQ;
    printf("tatlm1_ManageDisplay BAD userdata vl_Error: %d\n", vp_userdata);
    break;
  }

  return vl_Error ;
}

/*---------------------------------------------------------------------------------*
* Procedure    : tatlm1_00ActExec
*---------------------------------------------------------------------------------*
* Description  : Execute the Cmd Action sent by DTH 
*---------------------------------------------------------------------------------*
* Parameter(s) : dth_element *, the  DTH element
*---------------------------------------------------------------------------------*
* Return Value : u32, TAT error 
*---------------------------------------------------------------------------------*/
int32_t tatlm1_00ActExec(struct dth_element *elem)
{
  int32_t vl_Error=TAT_ERROR_OFF;
  uint32_t   vl_AlsValue=0;
  t_tatvideo_DisplayManagement s_DisplayManagement;
  uint32_t vl_TimeOut;

  char command[100];
  int32_t vl_PEDthElementNumber=0;

  switch(elem->user_data)
  {
    /*CAMERA ACTION*/
  case ACT_CAMERA_INIT:
	switch(VIDEO_param_array[ACT_CAMERA_INIT])
	{ 
	  case ACT_CAMERA_INIT_InitSensor:
	  {
	    if(v_MMTE_started==1)
	    {
	      switch(VIDEO_param_array [IN_SENSOR_ID])
	      {
	        case ENUM_VIDEO_CAMERA_STATICCONF_INIT_IN_SENSORID_PRIMARY:
		  CameraServ_InitMmte(fd_input,fd_popen,"Trigger detected",MMTE_TIMEOUT);
		  break;
	        case ENUM_VIDEO_CAMERA_STATICCONF_INIT_IN_SENSORID_SECONDARY:
		  CameraServ_Init2Mmte(fd_input,fd_popen,"Trigger detected",MMTE_TIMEOUT);
		  break;
	      }
	    }
	    else
	    {
	      SYSLOG(LOG_ERR,"MMTE is not started.");
	      return(TAT_ERROR_MISC);
	    }
	  }
	  break;
	  case ACT_CAMERA_INIT_PageElementDth:
	  {
		  vl_PEDthElementNumber=tatlm1_118GetPeDthElementCounter();

		  if(vl_PEDthElementNumber==0)
	      tatlm1_108register_pe_dth_struct(MAP_FILE);
	    else
	    {	
	      SYSLOG(LOG_ERR, "Page Element DTH  for ISP registers is already mounted.");
	      //return(TAT_ERROR_MISC);
	    }
	  }
	  break;
	  case ACT_CAMERA_INIT_PageElementStub:
	  {
	    if(v_PE_Stub==0)
	    {
	      CameraServ_SetMmteAlias(fd_input,fd_popen,"alias PAGE_ELEMENT_STUB 1\n",MMTE_TIMEOUT);
	      v_PE_Stub=1;
	      SYSLOG(LOG_ERR, "Page Element Stub enabled.");
	      //return(TAT_ERROR_MISC);
	    }
	    else
	    {
	      CameraServ_SetMmteAlias(fd_input,fd_popen,"alias PAGE_ELEMENT_STUB 0\n",MMTE_TIMEOUT);
	      v_PE_Stub=0;
	      SYSLOG(LOG_ERR, "Page Element Stub disabled: you go to use the real ISP registers.");
	      //return(TAT_ERROR_MISC);
	    }
	  }
	  break;
	}
    break;

  case ACT_SET_VIDEO:

    switch(VIDEO_param_array[ACT_SET_VIDEO])
      {
      case ACT_SET_VIDEO_PARAM: 
  	if(v_MMTE_started==1)
	{
	  sprintf(command,"alias DEFAULT_VPB2_WIDTH__UC_STILL %d\n",VIDEO_param_array [IN_VIDEO_WIDTH]);
	  CameraServ_SetMmteAlias(fd_input,fd_popen,command,MMTE_TIMEOUT);

	  sprintf(command,"alias DEFAULT_VPB2_HEIGHT__UC_STILL %d\n",VIDEO_param_array [IN_VIDEO_HEIGHT]);
	  CameraServ_SetMmteAlias(fd_input,fd_popen,command,MMTE_TIMEOUT);

	  sprintf(command,"alias DEFAULT_FRAMERATE_VPB2 %d\n",VIDEO_param_array [IN_VIDEO_FRAMERATE]);
	  CameraServ_SetMmteAlias(fd_input,fd_popen,command,MMTE_TIMEOUT);
	}
	else
	{
	  SYSLOG(LOG_ERR, "MMTE is not started.");
	  //return(TAT_ERROR_MISC);
	}
	break;

      case ACT_SET_VIDEO_RESOLUTION_UPDATE:
	tatlm1_12ActSetResolutionVideo(VIDEO_param_array[Set_Video_Resolution]);
	break;
      case ACT_SET_VIDEO_1ST_SETTINGS:
	  VIDEO_param_array [IN_SENSOR_ID]=0;;
	  VIDEO_param_array [IN_VIDEO_WIDTH]=640;
	  VIDEO_param_array [IN_VIDEO_HEIGHT]=480;
	  VIDEO_param_array [IN_VIDEO_FRAMERATE]=22;
	  VIDEO_param_array [Set_Video_Resolution]=Set_Video_Resolution_VGA;

	break;
      case ACT_SET_VIDEO_2ND_SETTINGS:
	  VIDEO_param_array [IN_SENSOR_ID]=1;
	  VIDEO_param_array [IN_VIDEO_WIDTH]=176;
	  VIDEO_param_array [IN_VIDEO_HEIGHT]=144;
	  VIDEO_param_array [IN_VIDEO_FRAMERATE]=22;
	  VIDEO_param_array [Set_Video_Resolution]=Set_Video_Resolution_QCIF;

	break;
      }
    break;

  case ACT_SET_PREVIEW:
  {
    char v_ScanMode=0;
    switch(VIDEO_param_array[ACT_SET_PREVIEW])
      {
      case ACT_SET_PREVIEW_PARAM:
	{
	

    switch(VIDEO_param_array[IN_PREVIEW_SCAN_MODE])
    {
	case ENUM_VIDEO_CAMERA_STATICCONF_SET_PREVIEW_IN_SCANMODE_INTERLACED:
		v_ScanMode='I';
	break;
	case ENUM_VIDEO_CAMERA_STATICCONF_SET_PREVIEW_IN_SCANMODE_PROGRESSIVE:
		v_ScanMode='P';
	break;
	default:
	break;
    }

    switch(VIDEO_param_array[IN_PREVIEW_ID])
    {
	case ENUM_VIDEO_CAMERA_STATICCONF_SET_PREVIEW_IN_DISPLAYID_MAIN:
	{
	  char command[100];

	  FILE * fd_fbo_LCD1=NULL;
	  FILE * fd_fbo_LCD2=NULL;
	  FILE * fd_fbo_LCD3=NULL;
	  FILE * fd_fbo_LCD4=NULL;

	  fd_fbo_LCD1=fopen("/dev/graphics/fb0_LCD1","r");
	  fd_fbo_LCD2=fopen("/dev/graphics/fb0_LCD2","r");
	  fd_fbo_LCD3=fopen("/dev/graphics/fb0_LCD3","r");
	  fd_fbo_LCD4=fopen("/dev/graphics/fb0_LCD4","r");

	  if((fd_fbo_LCD1!=NULL) || (fd_fbo_LCD2!=NULL) || (fd_fbo_LCD3!=NULL) || (fd_fbo_LCD4!=NULL))
	  {
	    memset(command,0,sizeof(command));
	    sprintf(command,"rm -f /dev/graphics/fb0\n");
	    printf(command);
	    printf("\n");
	    system(command);
	  }
	  if(fd_fbo_LCD1!=NULL)
	  {
	    fclose(fd_fbo_LCD1);
	    memset(command,0,sizeof(command));
	    sprintf(command,"mv /dev/graphics/fb0_LCD1 /dev/graphics/fb0\n");
	    printf(command);
	    printf("\n");
	    system(command);
	  }
	  if(fd_fbo_LCD2!=NULL)
	  {
	    fclose(fd_fbo_LCD2);
	    memset(command,0,sizeof(command));
	    sprintf(command,"mv /dev/graphics/fb0_LCD2 /dev/graphics/fb0\n");
	    printf(command);
	    printf("\n");
	    system(command);
	  }
	  if(fd_fbo_LCD3!=NULL)
	  {
	    fclose(fd_fbo_LCD3);
	    memset(command,0,sizeof(command));
	    sprintf(command,"mv /dev/graphics/fb0_LCD3 /dev/graphics/fb0\n");
	    printf(command);
	    printf("\n");
	    system(command);
	  }
	  if(fd_fbo_LCD4!=NULL)
	  {
	    fclose(fd_fbo_LCD4);
	    memset(command,0,sizeof(command));
	    sprintf(command,"mv /dev/graphics/fb0_LCD4 /dev/graphics/fb0\n");
	    printf(command);
	    printf("\n");
	    system(command);
	  }

	  memset(command,0,sizeof(command));
	  sprintf(command,"VideoTests -m=0 -d=%d -dgprop\n",VIDEO_param_array[IN_PREVIEW_ID]);
	  printf(command);
	  system(command);
	  printf("\n");
	}
	break;
	case ENUM_VIDEO_CAMERA_STATICCONF_SET_PREVIEW_IN_DISPLAYID_TVO_CVBS_AV8100:
    	{
	  char command[100];
	  FILE * fd_fbo_LCD1=NULL;
	  FILE * fd_fbo_LCD2=NULL;
	  FILE * fd_fbo_LCD3=NULL;
	  FILE * fd_fbo_LCD4=NULL;

	  if(InitTvoCvbsAV8100DisplayDone==0)
	  {
	    memset(command,0,sizeof(command));
	    sprintf(command,"VideoTests -m=0 -d=%d -dinit\n",VIDEO_param_array[IN_PREVIEW_ID]);
	    printf(command);
	    system(command);
	    printf("\n");

	    memset(command,0,sizeof(command));
	    sprintf(command,"VideoTests -m=0 -davconn=0\n");
	    printf(command);
	    system(command);
	    printf("\n");

	    InitTvoCvbsAV8100DisplayDone=1;
	  }
		
	  memset(command,0,sizeof(command));
	  sprintf(command,"VideoTests -m=0 -d=%d -dplug\n",VIDEO_param_array[IN_PREVIEW_ID]);
	  printf(command);
	  system(command);
	  printf("\n");
		
	  memset(command,0,sizeof(command));
	  sprintf(command,"VideoTests -m=0 -d=%d -dprop=%dx%dx%dx%c\n",VIDEO_param_array[IN_PREVIEW_ID],VIDEO_param_array[IN_PREVIEW_DISPLAY_WIDTH ],VIDEO_param_array[IN_PREVIEW_DISPLAY_HEIGHT],VIDEO_param_array[IN_PREVIEW_DISPLAY_FRAMERATE],v_ScanMode);
	  printf(command);
	  system(command);
	  printf("\n");

	  memset(command,0,sizeof(command));
	  sprintf(command,"VideoTests -m=0 -d=%d -dgprop\n",VIDEO_param_array[IN_PREVIEW_ID]);
	  printf(command);
	  system(command);
	  printf("\n");

	  fd_fbo_LCD3=fopen("/dev/graphics/fb0_LCD3","r");

	  if(fd_fbo_LCD3==NULL)
	  {
	    fd_fbo_LCD1=fopen("/dev/graphics/fb0_LCD1","r");
	    fd_fbo_LCD2=fopen("/dev/graphics/fb0_LCD2","r");
	    fd_fbo_LCD4=fopen("/dev/graphics/fb0_LCD4","r");

	    if(fd_fbo_LCD1!=NULL)
	    {
	      fclose(fd_fbo_LCD1);
	      memset(command,0,sizeof(command));
	      sprintf(command,"mv /dev/graphics/fb0_LCD1 /dev/graphics/fb0_LCD3\n");
	      printf(command);
	      printf("\n");
	      system(command);
	    }
	    else if(fd_fbo_LCD2!=NULL)
	    {
	      fclose(fd_fbo_LCD2);
	      memset(command,0,sizeof(command));
	      sprintf(command,"mv /dev/graphics/fb0_LCD2 /dev/graphics/fb0_LCD3\n");
	      printf(command);
	      printf("\n");
	      system(command);
	    }
	    else if(fd_fbo_LCD4!=NULL)
	    {
	      fclose(fd_fbo_LCD4);
	      memset(command,0,sizeof(command));
	      sprintf(command,"mv /dev/graphics/fb0_LCD4 /dev/graphics/fb0_LCD3\n");
	      printf(command);
	      printf("\n");
	      system(command);
	    }
	    else
	    {
	      memset(command,0,sizeof(command));
	      sprintf(command,"mv /dev/graphics/fb0 /dev/graphics/fb0_LCD3\n");
	      printf(command);
	      printf("\n");
	      system(command);
	    }

	    memset(command,0,sizeof(command));
	    sprintf(command,"rm -f /dev/graphics/fb0\n");
	    printf(command);
	    printf("\n");
	    system(command);

	    memset(command,0,sizeof(command));
	    sprintf(command,"ln -s /dev/fb3 /dev/graphics/fb0\n");
	    printf(command);
	    printf("\n");
	    system(command);	
	  }
	  else
	  {
	    fclose(fd_fbo_LCD3);
	  }
	}
	break;
	case ENUM_VIDEO_CAMERA_STATICCONF_SET_PREVIEW_IN_DISPLAYID_TVO_HDMI:
    	{
	  char command[100];
	  FILE * fd_fbo_LCD1=NULL;
	  FILE * fd_fbo_LCD2=NULL;
	  FILE * fd_fbo_LCD3=NULL;
	  FILE * fd_fbo_LCD4=NULL;

	  if(InitTvoHdmiDisplayDone==0)
	  {
	    memset(command,0,sizeof(command));
	    sprintf(command,"VideoTests -m=0 -d=%d -dinit\n",VIDEO_param_array[IN_PREVIEW_ID]);
	    printf(command);
	    system(command);
	    printf("\n");
	    
	    InitTvoHdmiDisplayDone=1;
	  }
		
	  memset(command,0,sizeof(command));
	  sprintf(command,"VideoTests -m=0 -d=%d -dplug\n",VIDEO_param_array[IN_PREVIEW_ID]);
	  printf(command);
	  system(command);
	  printf("\n");
		
	  memset(command,0,sizeof(command));
	  sprintf(command,"VideoTests -m=0 -d=%d -dprop=%dx%dx%dx%c\n",VIDEO_param_array[IN_PREVIEW_ID],VIDEO_param_array[IN_PREVIEW_DISPLAY_WIDTH],VIDEO_param_array[IN_PREVIEW_DISPLAY_HEIGHT],VIDEO_param_array[IN_PREVIEW_DISPLAY_FRAMERATE],v_ScanMode);
	  printf(command);
	  system(command);
	  printf("\n");

	  memset(command,0,sizeof(command));
	  sprintf(command,"VideoTests -m=0 -d=%d -dgprop\n",VIDEO_param_array[IN_PREVIEW_ID]);
	  printf(command);
	  system(command);
	  printf("\n");

	  fd_fbo_LCD4=fopen("/dev/graphics/fb0_LCD4","r");

	  if(fd_fbo_LCD4==NULL)
	  {
	    fd_fbo_LCD1=fopen("/dev/graphics/fb0_LCD1","r");
	    fd_fbo_LCD2=fopen("/dev/graphics/fb0_LCD2","r");
	    fd_fbo_LCD3=fopen("/dev/graphics/fb0_LCD3","r");

	    if(fd_fbo_LCD1!=NULL)
	    {
	      fclose(fd_fbo_LCD1);
	      memset(command,0,sizeof(command));
	      sprintf(command,"mv /dev/graphics/fb0_LCD1 /dev/graphics/fb0_LCD4\n");
	      printf(command);
	      printf("\n");
	      system(command);
	    }
	    else if(fd_fbo_LCD2!=NULL)
	    {
	      fclose(fd_fbo_LCD2);
	      memset(command,0,sizeof(command));
	      sprintf(command,"mv /dev/graphics/fb0_LCD2 /dev/graphics/fb0_LCD4\n");
	      printf(command);
	      printf("\n");
	      system(command);
	    }
	    else if(fd_fbo_LCD3!=NULL)
	    {
	      fclose(fd_fbo_LCD3);
	      memset(command,0,sizeof(command));
	      sprintf(command,"mv /dev/graphics/fb0_LCD3 /dev/graphics/fb0_LCD4\n");
	      printf(command);
	      printf("\n");
	      system(command);
	    }
	    else
	    {
	      memset(command,0,sizeof(command));
	      sprintf(command,"mv /dev/graphics/fb0 /dev/graphics/fb0_LCD4\n");
	      printf(command);
	      printf("\n");
	      system(command);
	    }

	    memset(command,0,sizeof(command));
	    sprintf(command,"rm -f /dev/graphics/fb0\n");
	    printf(command);
	    printf("\n");
	    system(command);

	    memset(command,0,sizeof(command));
	    sprintf(command,"ln -s /dev/fb3 /dev/graphics/fb0\n");
	    printf(command);
	    printf("\n");
	    system(command);	
	  }
	  else
	  {
	    fclose(fd_fbo_LCD4);
	  }
	}
	break;
	default:
	break;
    }
    if(v_MMTE_started==1)
    {
      sprintf(command,"alias DEFAULT_VPB0_WIDTH %d\n",VIDEO_param_array [IN_PREVIEW_WIDTH]);
      CameraServ_SetMmteAlias(fd_input,fd_popen,command,MMTE_TIMEOUT);

      sprintf(command,"alias DEFAULT_VPB0_HEIGHT %d\n",VIDEO_param_array [IN_PREVIEW_HEIGHT]);
      CameraServ_SetMmteAlias(fd_input,fd_popen,command,MMTE_TIMEOUT);	

      sprintf(command,"alias DEFAULT_FRAMERATE_VPB0 %d\n",VIDEO_param_array [IN_PREVIEW_FRAMERATE]);
      CameraServ_SetMmteAlias(fd_input,fd_popen,command,MMTE_TIMEOUT);	
    }
    else
    {
      SYSLOG(LOG_ERR, "MMTE is not started.");
      //return(TAT_ERROR_MISC);
    }
	}
    break;

      case ACT_SET_PREVIEW_RESOLUTION_UPDATE:
	tatlm1_10ActSetResolutionVF(VIDEO_param_array[Set_Preview_Resolution]);
	break;
      case ACT_SET_PREVIEW_1ST_SETTINGS:
	  VIDEO_param_array [IN_PREVIEW_WIDTH]=480;
	  VIDEO_param_array [IN_PREVIEW_HEIGHT]=848;
	  VIDEO_param_array [IN_PREVIEW_FRAMERATE]=22;
	  VIDEO_param_array [Set_Preview_Resolution]=Set_Preview_Resolution_WVGAp;

	break;
      case ACT_SET_PREVIEW_2ND_SETTINGS:
	  VIDEO_param_array [IN_PREVIEW_WIDTH]=480;
	  VIDEO_param_array [IN_PREVIEW_HEIGHT]=640;
	  VIDEO_param_array [IN_PREVIEW_FRAMERATE]=22;
	  VIDEO_param_array [Set_Preview_Resolution]=Set_Preview_Resolution_VGA;
	break;
      }
  }
  break; 

  case ACT_SET_PICTURE:

    switch(VIDEO_param_array[ACT_SET_PICTURE])
    {
      case ACT_SET_PICTURE_PARAM:
      {
	if(v_MMTE_started==1)
	{
	  sprintf(command,"alias IMG_FILTER_PATH_BACKUP_IMAGE %s\n",VIDEO_string_array[IN_PICTURE_PATH]);
	  CameraServ_SetMmteAlias(fd_input,fd_popen,command,MMTE_TIMEOUT);	

	  sprintf(command,"alias IMG_FILTER_PATH_STORE_IMAGE %s\n",VIDEO_string_array[IN_PICTURE_PATH]);
	  CameraServ_SetMmteAlias(fd_input,fd_popen,command,MMTE_TIMEOUT);	

	  sprintf(command,"alias IMG_FILTER_PATH_CONFIG_FILE_METRIC %s\n",VIDEO_string_array[IN_PICTURE_PATH]);
	  CameraServ_SetMmteAlias(fd_input,fd_popen,command,MMTE_TIMEOUT);	
 
	  sprintf(command,"alias FAKE_SINK_PATH_BACKUP_IMAGE %s\n",VIDEO_string_array[IN_PICTURE_PATH]);
	  CameraServ_SetMmteAlias(fd_input,fd_popen,command,MMTE_TIMEOUT);	

	  sprintf(command,"alias DEFAULT_VPB1_0_WIDTH_WITH_ISPPROC %d\n",VIDEO_param_array [IN_PICTURE_WIDTH]);
	  CameraServ_SetMmteAlias(fd_input,fd_popen,command,MMTE_TIMEOUT);

	  sprintf(command,"alias DEFAULT_VPB1_0_HEIGHT_WITH_ISPPROC %d\n",VIDEO_param_array [IN_PICTURE_HEIGHT]);
	  CameraServ_SetMmteAlias(fd_input,fd_popen,command,MMTE_TIMEOUT);	

	  sprintf(command,"alias DEFAULT_VPB1_1_WIDTH_WITH_ISPPROC %d\n",VIDEO_param_array [IN_PICTURE_WIDTH]);
	  CameraServ_SetMmteAlias(fd_input,fd_popen,command,MMTE_TIMEOUT);

	  sprintf(command,"alias DEFAULT_VPB1_1_HEIGHT_WITH_ISPPROC %d\n",VIDEO_param_array [IN_PICTURE_HEIGHT]);
	  CameraServ_SetMmteAlias(fd_input,fd_popen,command,MMTE_TIMEOUT);	

	  sprintf(command,"alias DEFAULT_FRAMERATE_VPB1 %d\n",VIDEO_param_array [IN_PREVIEW_FRAMERATE]);
	  CameraServ_SetMmteAlias(fd_input,fd_popen,command,MMTE_TIMEOUT);	

	  sprintf(command,"alias SAVING_FRAME_BY_FRAME %s\n","OMX_TRUE");
	  CameraServ_SetMmteAlias(fd_input,fd_popen,command,MMTE_TIMEOUT);	

	  tatlm1_05RawSavingManagement();
	}
	else
	{
	  SYSLOG(LOG_ERR, "MMTE is not started.");
	  //return(TAT_ERROR_MISC);
	}
      }
      break;

      case ACT_SET_PICTURE_RESOLUTION_UPDATE:
      {
	tatlm1_11ActSetResolutionStill(VIDEO_param_array[Set_Picture_Resolution]);
      }
      break;
      case ACT_SET_PICTURE_1ST_SETTINGS:
	  VIDEO_param_array [IN_PICTURE_WIDTH]=1600;
	  VIDEO_param_array [IN_PICTURE_HEIGHT]=1200;
	  VIDEO_param_array [IN_PICTURE_FRAMERATE]=22;
	  VIDEO_param_array [Set_Picture_Resolution]=Set_Picture_Resolution_UXGA;

	break;
      case ACT_SET_PICTURE_2ND_SETTINGS:
	  VIDEO_param_array [IN_PICTURE_WIDTH]=480;
	  VIDEO_param_array [IN_PICTURE_HEIGHT]=640;
	  VIDEO_param_array [IN_PICTURE_FRAMERATE]=22;
	  VIDEO_param_array [Set_Picture_Resolution]=Set_Picture_Resolution_VGA;
	break;
    case ACT_SET_PICTURE_SD_CONFIG:
	  {
	    static char *pathname = "/mnt/SDCard//";

	    free(VIDEO_string_array[IN_PICTURE_PATH]);
	    VIDEO_string_array[IN_PICTURE_PATH]=(int8_t*)calloc(strlen(pathname)+1,sizeof(int8_t));
	    strncpy((char*)VIDEO_string_array[IN_PICTURE_PATH],(const char*)pathname,strlen(pathname));

	  }
    }
    break;

  case ACT_TAKE_PICTURE:
    switch(VIDEO_param_array [ACT_TAKE_PICTURE])
    {
	case ENUM_VIDEO_CAMERA_STATICCONF_SHOT_START_IDLE:
 
	  if((VIDEO_param_array[CURRENT_STATE]==CURRENT_STATE_INIT) && (v_MMTE_started==1))
	  {
	    CameraServ_StartIdle(fd_input,fd_popen,"Trigger detected",MMTE_TIMEOUT);
	    VIDEO_param_array[CURRENT_STATE]=CURRENT_STATE_IDLE;
	    tatlm1_07InternalIqtState("IDLE");
	    tatlm1_09AclManagementForTheState(CURRENT_STATE_IDLE);
	  }
	  else
	  {
	    switch(v_MMTE_started)
	    {
	    case 1:
	    {	
	      SYSLOG(LOG_ERR, "Only the transition from Init to Idle is allowed\nYou are not in the correct state.");
	    }
	    break;
	    case 0:
	    {
	      SYSLOG(LOG_ERR, "MMTE is not started.");
	    }
	    break;
	    }
	    //return(TAT_ERROR_MISC);
	  }
	  break;
	case ENUM_VIDEO_CAMERA_STATICCONF_SHOT_START_PREVIEW:
	  if(VIDEO_param_array[CURRENT_STATE]==CURRENT_STATE_IDLE)
	    {
	      CameraServ_StartResume(fd_input,fd_popen,"Trigger detected",MMTE_TIMEOUT);
	      VIDEO_param_array[CURRENT_STATE]=CURRENT_STATE_VIEWFINDER;
	      tatlm1_07InternalIqtState("VIEWFINDER");
	      tatlm1_09AclManagementForTheState(CURRENT_STATE_VIEWFINDER);
	    }
	  else
	  {
	    SYSLOG(LOG_ERR, "Only the transition from Idle to ViewFinder is allowed\nYou are not in the correct state.");
	    //return(TAT_ERROR_MISC);
	  }		
	break;
	case ENUM_VIDEO_CAMERA_STATICCONF_SHOT_TAKE_PICTURE:
	{
	  if(VIDEO_param_array [IN_PICTURE_SHOTMODE ]==ENUM_VIDEO_CAMERA_STATICCONF_SHOT_SHOTMODE_SINGLE_SHOT)
	  {
	    if(VIDEO_param_array[CURRENT_STATE]==CURRENT_STATE_VIEWFINDER)
	    {
	      VIDEO_param_array[CURRENT_STATE]=CURRENT_STATE_STILLPICTURE;
	      tatlm1_07InternalIqtState("STILL");
	      tatlm1_09AclManagementForTheState(CURRENT_STATE_STILLPICTURE);			

	      CameraServ_StartStillPictureCapture(fd_input,fd_popen,"Trigger detected",MMTE_TIMEOUT);

	      VIDEO_param_array[CURRENT_STATE]=CURRENT_STATE_VIEWFINDER;
	      tatlm1_07InternalIqtState("VIEWFINDER");
	      tatlm1_09AclManagementForTheState(CURRENT_STATE_VIEWFINDER);

	      {
		char command[500];
		char FileName[500];
		uint32_t ListOfCurrentFramesNumber=0;
		FILE *ListOfSingleStillPictures;

		memset(command,0,sizeof(command));
		sprintf(command,"ls %sframe*.bmp >/tmp/ListOfSingleStillPictures",VIDEO_string_array[IN_PICTURE_PATH]);
		printf("%s\n",command);
		system(command);

		memset(FileName,0,sizeof(FileName));
		ListOfSingleStillPictures=fopen("/tmp/ListOfSingleStillPictures","r");
		while(fgets(FileName,sizeof(FileName),ListOfSingleStillPictures)!=NULL)
		{
		  ListOfCurrentFramesNumber++;
		}

	
		fclose(ListOfSingleStillPictures);

		FileName[strlen(FileName)-1]='\0';

		memset(command,0,sizeof(command));
		sprintf(command,"cp %s %spicture.bmp\n",FileName,VIDEO_string_array[IN_PICTURE_PATH]);
		printf(command);
		system(command);

		system("rm -f /tmp/picture.bmp");

		memset(command,0,sizeof(command));
		sprintf(command,"ln -s %s /tmp/picture.bmp\n",FileName);
		printf(command);
		system(command);
	      }
	    }
	    else
	    {
	      SYSLOG(LOG_ERR, "Only the transition from ViewFinder to StillPicture is allowed\nYou are not in the correct state.");
	      //return(TAT_ERROR_MISC);
	    }
	  }
	  else
	  {
	    sprintf(command,"alias S_NB_FRAME_LIMIT  %d\n",VIDEO_param_array [IN_PICTURE_FINITEBURSTFRAME]);
	    CameraServ_SetMmteAlias(fd_input,fd_popen,command,MMTE_TIMEOUT);

	    if(VIDEO_param_array [IN_PICTURE_BURSTMODE ]==ENUM_VIDEO_CAMERA_STATICCONF_SHOT_BURSTMODE_INFINITE_BURST)
	    {
	      sprintf(command,"alias S_IS_LIMIT OMX_FALSE\n");
	      vl_TimeOut=MMTE_TIMEOUT;
	    }
	    if(VIDEO_param_array [IN_PICTURE_BURSTMODE ]==ENUM_VIDEO_CAMERA_STATICCONF_SHOT_BURSTMODE_FINITE_BURST)
	    {
	      sprintf(command,"alias S_IS_LIMIT OMX_TRUE\n");
	      vl_TimeOut=VIDEO_param_array [IN_PICTURE_FINITEBURSTFRAME]*MMTE_TIMEOUT_FOR_STILL;
	      printf("Timeout for still burst = %d\n",vl_TimeOut);
	    }
	    CameraServ_SetMmteAlias(fd_input,fd_popen,command,MMTE_TIMEOUT);	

	    if(VIDEO_param_array[CURRENT_STATE]==CURRENT_STATE_VIEWFINDER)
	    {
	      VIDEO_param_array[CURRENT_STATE]=CURRENT_STATE_STILLPICTURE;
	      tatlm1_07InternalIqtState("STILL");
	      tatlm1_09AclManagementForTheState(CURRENT_STATE_STILLPICTURE);

	      CameraServ_StartStillBurstPictureCapture(fd_input,fd_popen,"Trigger detected",vl_TimeOut);
	      if(VIDEO_param_array [IN_PICTURE_BURSTMODE ]==ENUM_VIDEO_CAMERA_STATICCONF_SHOT_BURSTMODE_FINITE_BURST)
	      {			
		VIDEO_param_array[CURRENT_STATE]=CURRENT_STATE_VIEWFINDER;
		tatlm1_07InternalIqtState("VIEWFINDER");
		tatlm1_09AclManagementForTheState(CURRENT_STATE_VIEWFINDER);
	      }
	    }
	    else
	    {
	      SYSLOG(LOG_ERR, "Only the transition from ViewFinder to StillPicture is allowed\nYou are not in the correct state.");
	      //return(TAT_ERROR_MISC);
	    }
	  }
	}
	break;

	case ENUM_VIDEO_CAMERA_STATICCONF_SHOT_STOP_PREVIEW:
	  if(VIDEO_param_array[CURRENT_STATE]==CURRENT_STATE_VIEWFINDER)
	  {
	    CameraServ_StopPreview(fd_input,fd_popen,"Trigger detected",MMTE_TIMEOUT);

	    VIDEO_param_array[CURRENT_STATE]=CURRENT_STATE_IDLE;
	    tatlm1_07InternalIqtState("IDLE");
	    tatlm1_09AclManagementForTheState(CURRENT_STATE_IDLE);
	  }
	  else
	  {	
	    SYSLOG(LOG_ERR, "Only the transition from ViewFinder to Idle is allowed\nYou are not in the correct state.");
	    //return(TAT_ERROR_MISC);
	  }
	break;

	case ENUM_VIDEO_CAMERA_STATICCONF_SHOT_STOP_IDLE:

	  if(VIDEO_param_array[CURRENT_STATE]==CURRENT_STATE_IDLE)
	  {
	    CameraServ_StopIdle(fd_input,fd_popen,"Trigger detected",MMTE_TIMEOUT);
	    VIDEO_param_array[CURRENT_STATE]=CURRENT_STATE_INIT;
	    tatlm1_07InternalIqtState("INIT");
	    tatlm1_09AclManagementForTheState(CURRENT_STATE_INIT);
	  }
	  else
	  {	
	    SYSLOG(LOG_ERR, "Only the transition from Idle to Init is allowed\nYou are not in the correct state.");
	    //return(TAT_ERROR_MISC);
	  }
	break;

	case ENUM_VIDEO_CAMERA_STATICCONF_SHOT_PAUSE :

	  if((VIDEO_param_array[CURRENT_STATE]==CURRENT_STATE_VIEWFINDER) || (VIDEO_param_array[CURRENT_STATE]==CURRENT_STATE_VIDEO))
	  {
	    CameraServ_StartPause(fd_input,fd_popen,"Trigger detected",MMTE_TIMEOUT);
	    v_backup_CameraState=VIDEO_param_array[CURRENT_STATE];
	    VIDEO_param_array[CURRENT_STATE]=CURRENT_STATE_PAUSE;
	    tatlm1_07InternalIqtState("PAUSE");
	    tatlm1_09AclManagementForTheState(CURRENT_STATE_PAUSE);
	  }
	  else
	  {
	    SYSLOG(LOG_ERR, "Only the transition from ViewFinder or Video to Pause is allowed\nYou are not in the correct state.");
	    //return(TAT_ERROR_MISC);
	  }
	break;
	case ENUM_VIDEO_CAMERA_STATICCONF_SHOT_RESUME:
	  switch(v_backup_CameraState)
	  {  
	    case CURRENT_STATE_VIEWFINDER:
	    {
	      v_backup_CameraState=CURRENT_STATE_INIT;
	      CameraServ_StartResume(fd_input,fd_popen,"Trigger detected",MMTE_TIMEOUT);
	      VIDEO_param_array[CURRENT_STATE]=CURRENT_STATE_VIEWFINDER;
	      tatlm1_07InternalIqtState("VIEWFINDER");
	      tatlm1_09AclManagementForTheState(CURRENT_STATE_VIEWFINDER);
	    break;
	  }
	    case CURRENT_STATE_VIDEO:
	    {
	      v_backup_CameraState=CURRENT_STATE_INIT;
	      CameraServ_StartResume(fd_input,fd_popen,"Trigger detected",MMTE_TIMEOUT);
	      VIDEO_param_array[CURRENT_STATE]=CURRENT_STATE_VIDEO ;
	      tatlm1_07InternalIqtState("VIDEO");
	      tatlm1_09AclManagementForTheState(CURRENT_STATE_VIDEO);
	      break;
	    } 
  	    default: 
	      SYSLOG(LOG_ERR, "Transition Executing => Executing forbidden.");
	      //return(TAT_ERROR_MISC);
	      break;
	}
	break;
	case ENUM_VIDEO_CAMERA_STATICCONF_SHOT_STOP_MMTE:
	  if(v_MMTE_started==1)
	  {
	    char command[100];

	    CameraServ_StopMmte(fd_input,fd_popen,"Command executed",MMTE_TIMEOUT);
	    memset(command,0,sizeof(command));
	    sprintf(command,"umount %s\n",VIDEO_string_array[IN_PICTURE_PATH]);
	    printf(command);
	    system(command);

	    sleep(1);
	    CameraServ_CloseFileDescriptors();

	    VIDEO_param_array[CURRENT_STATE]=CURRENT_STATE_INIT;
	    v_MMTE_started = 0;
	  }
	  else
	  {	
	    SYSLOG(LOG_ERR, "MMTE is not yet started.");
	    //return(TAT_ERROR_MISC);
	  }
	break;
	case ENUM_VIDEO_CAMERA_STATICCONF_SHOT_START_MMTE:
	{

	  if(v_MMTE_started==0)
	  {
	    char command[100];
		DIR *dir;
		struct dirent *inputdir;
		unsigned int i=0;
		char *temp;

		/* Folder /dev/pts contains all device to export MMTE console */
		/* when a new telnet console is opened, a new device is created */
		/* we need to get the latest to redirect console */
		/* and maybe trace MMTE on this 'expected' telnet */
		dir = opendir("/dev/pts");
		if(dir == NULL) {
			printf("tatlvideo: cannot find /dev/pts !\n");
			return TAT_ERROR_NOT_FOUND;
		}

		while((inputdir = readdir(dir)) != NULL) {
			strtoul(inputdir->d_name, &temp, 10);
			if((*inputdir->d_name != '\0') && *temp == '\0') {
				i++;
			}
		}
		printf("/dev/pts directory found : %d\n", i);
		closedir(dir);
		if( i<1 ) {
			printf("Error no pts files have been found\n");
			return TAT_ERROR_NOT_FOUND;
		}

		CameraServ_InitPipe("/tmp/input_camera_pipe","/tmp/output_camera_pipe","/tmp/camera_output_MMTE_task_pipe",&fd_input,&fd_output,&fd_popen,i-1);
	    tatlm1_109InitFileDescriptors(fd_input,fd_popen);

	    memset(command,0,sizeof(command));
	    sprintf(command,"mount %s\n",VIDEO_string_array[IN_PICTURE_PATH]);
	    printf(command);
	    system(command); 

	    CameraServ_StartMmte("/tmp/input_camera_pipe","/tmp/output_camera_pipe",fd_popen,"Trigger detected",MMTE_TIMEOUT);

	    VIDEO_param_array[CURRENT_STATE]=CURRENT_STATE_INIT;
	    tatlm1_07InternalIqtState("INIT");
	    tatlm1_09AclManagementForTheState(CURRENT_STATE_INIT);
	    v_MMTE_started = 1;
	  }
	  else
	  {	
	    SYSLOG(LOG_ERR, "MMTE is already started.");
	    //return(TAT_ERROR_MISC);
	  }
	}
	break;
	case ENUM_VIDEO_CAMERA_STATICCONF_SHOT_START_VIDEO:
	{
	  if(VIDEO_param_array[CURRENT_STATE]==CURRENT_STATE_VIEWFINDER)
	  {
	    CameraServ_StartRecord(fd_input,fd_popen,"Trigger detected",MMTE_TIMEOUT);
	    VIDEO_param_array[CURRENT_STATE]=CURRENT_STATE_VIDEO;
	    tatlm1_07InternalIqtState("VIDEO");
	    tatlm1_09AclManagementForTheState(CURRENT_STATE_VIDEO);
	  }
	  else
	  {	
	    SYSLOG(LOG_ERR, "Only the transition from ViewFinder to Video is allowed\nYou are not in the correct state.");
	    //return(TAT_ERROR_MISC);
	  }
	}
	break;
	case ENUM_VIDEO_CAMERA_STATICCONF_SHOT_STOP_VIDEO:

	  if(VIDEO_param_array[CURRENT_STATE]==CURRENT_STATE_VIDEO)
	  {
	    CameraServ_StopRecord(fd_input,fd_popen,"Trigger detected",MMTE_TIMEOUT);
	    VIDEO_param_array[CURRENT_STATE]=CURRENT_STATE_VIEWFINDER;
	    tatlm1_07InternalIqtState("VIEWFINDER");
	    tatlm1_09AclManagementForTheState(CURRENT_STATE_VIEWFINDER);
	  }
	  else
	  {
	    SYSLOG(LOG_ERR, "Only the transition from Video to ViewFinder is allowed\nYou are not in the correct state.");
	    //return(TAT_ERROR_MISC);	
	  }
	break;
	case ENUM_VIDEO_CAMERA_STATICCONF_SHOT_STOP_BURST:
	  if(VIDEO_param_array[CURRENT_STATE]==CURRENT_STATE_STILLPICTURE)
	  {
	    CameraServ_StopStillBurstPictureCapture(fd_input,fd_popen,"Trigger detected",MMTE_TIMEOUT);
	    VIDEO_param_array[CURRENT_STATE]=CURRENT_STATE_VIEWFINDER;
	    tatlm1_07InternalIqtState("VIEWFINDER");
	    tatlm1_09AclManagementForTheState(CURRENT_STATE_VIEWFINDER);
	  }
	  else
	  {	
	    SYSLOG(LOG_ERR, "Only the transition from StillPicture to ViewFinder is allowed\nYou are not in the correct state.");
	    //return(TAT_ERROR_MISC);
	  } 
	break;
	case ENUM_VIDEO_CAMERA_STATICCONF_SHOT_LS_STORAGE:
	  printf("ls -al %s\n",VIDEO_string_array[IN_PICTURE_PATH]);
	  sprintf(command,"ls -al %s",VIDEO_string_array[IN_PICTURE_PATH]);
	  system(command);

	break;    
	case ENUM_VIDEO_CAMERA_STATICCONF_SHOT_RM_STORAGE:
	{ 
	  char TmpPath[50];
		   
	  memset(TmpPath,0,50);
	  strncpy(TmpPath,(char *)(VIDEO_string_array[IN_PICTURE_PATH]),strlen((char *)(VIDEO_string_array[IN_PICTURE_PATH])));
	  sprintf(command,"rm %s*",TmpPath);
	  printf(command);
	  printf("\n");
	  system(command);
	}
	break;
	case ENUM_VIDEO_CAMERA_STATICCONF_SHOT_MMTE_TRACE:
	  sprintf(command,"cat /tmp/trace_mmte.txt\n");
	  printf(command);
	  system(command);
	break;
	case ENUM_VIDEO_CAMERA_STATICCONF_SHOT_PAL_SETTINGS:
	  VIDEO_param_array [IN_PREVIEW_SCAN_MODE]=0;
	  VIDEO_param_array [IN_PREVIEW_DISPLAY_WIDTH]=720;
	  VIDEO_param_array [IN_PREVIEW_DISPLAY_HEIGHT]=576;
	  VIDEO_param_array [IN_PREVIEW_DISPLAY_FRAMERATE]=50;

	break;
	case ENUM_VIDEO_CAMERA_STATICCONF_SHOT_NTSC_SETTINGS:
	  VIDEO_param_array [IN_PREVIEW_SCAN_MODE]=0;
	  VIDEO_param_array [IN_PREVIEW_DISPLAY_WIDTH]=720;
	  VIDEO_param_array [IN_PREVIEW_DISPLAY_HEIGHT]=480;
	  VIDEO_param_array [IN_PREVIEW_DISPLAY_FRAMERATE]=60;
	break;
	case ENUM_VIDEO_CAMERA_STATICCONF_SHOT_REBOOT:
	  sprintf(command,"reboot\n");
	  printf(command);
	  system(command);
	break;

	default:
	break;
    }
    break;

    /*DISPLAY MAIN ACTION*/		
  case ACT_MAINDISPLAY_INIT:
  case ACT_SET_MAINDISPWRMODE:
  case ACT_MAINDISPLAYPICTURE:
  case ACT_MAINDISPLAYPATTERN:
    s_DisplayManagement.v_DisplayID=DISPLAY_MAIN;
    s_DisplayManagement.v_Option=VIDEO_param_array [elem->user_data];
    vl_Error=tatlm1_ManageDisplay (elem->user_data,s_DisplayManagement);
    break;
   /*DISPLAY CVBS ACTION*/			
  case ACT_CVBSDISPLAY_INIT:
  case ACT_SET_CVBSDISPROP:		
  case ACT_SET_CVBSDISPWRMODE:
  case ACT_CVBSDISPLAYPICTURE:
  case ACT_CVBSDISPLAYPATTERN:
  case ACT_CVBS_DETECT:
    if (VIDEO_param_array [IN_TVODISPLAY_ID]==0)
    {
      s_DisplayManagement.v_DisplayID=DISPLAY_TV_OUT_CVBS_4500;
    }
    else
    {
      s_DisplayManagement.v_DisplayID=DISPLAY_TV_OUT_CVBS_5810;
    }	
    s_DisplayManagement.v_Option=VIDEO_param_array [elem->user_data];
    vl_Error=tatlm1_ManageDisplay (elem->user_data,s_DisplayManagement);
    break;	
  case ACT_CVBSCONNECTOR:
    if (VIDEO_param_array [IN_TVOCONNECTOR_ID] == 0)
    {
      vl_Error = VidServ_AVConnectorSourceSelection(VIDEO_SOURCE);
      if (vl_Error != VIDSERV_NO_ERROR)
      {
	printf ("Video source selection VidServ_AVConnectorSourceSelection  vl_Error: %d\n", vl_Error);	 
	vl_Error = TAT_BAD_REQ;
      }
    }
    else
    {
        vl_Error = VidServ_AVConnectorSourceSelection(AUDIO_SOURCE);
	if (vl_Error != VIDSERV_NO_ERROR)
        {
          printf ("Audio source selection VidServ_AVConnectorSourceSelection  vl_Error: %d\n", vl_Error);
          vl_Error = TAT_BAD_REQ;
        }
    }
    break;	
      /*DISPLAY HDMI ACTION*/	
  case ACT_HDMIDISPLAY_INIT:
  case ACT_SET_HDMIDISPROP:
  case ACT_SET_HDMIDISPWRMODE:
  case ACT_HDMIDISPLAYPICTURE:
  case ACT_HDMIDISPLAYPATTERN:
  case ACT_HDMI_DETECT:	
    s_DisplayManagement.v_DisplayID=DISPLAY_TV_OUT_HDMI;
    s_DisplayManagement.v_Option=VIDEO_param_array [elem->user_data];
    vl_Error=tatlm1_ManageDisplay (elem->user_data,s_DisplayManagement);
    break;
  case ACT_GET_ALS:
    vl_Error=VidServ_GetAlsValue (&vl_AlsValue);
    if (vl_Error != VIDSERV_NO_ERROR)
    {
	printf ("Get ALS VidServ_GetAlsValue  vl_Error: %d, vl_AlsValue %d\n",vl_Error, vl_AlsValue);
	vl_Error = TAT_BAD_REQ;
    }
    else
    {
      VIDEO_param_array [OUT_ALS_VALUE]=(int32_t)vl_AlsValue;
    }
    break;
 /* case ACT_SET_CVBSAV8100INFORMAT:
    AV8100InputVideoFormat=VIDEO_param_array[IN_AV8100INFORMAT];
    vl_Error=VidServ_DisplayAV8100InputVideoFormat(AV8100InputVideoFormat);    
    break;
 */   
  default:
    vl_Error = TAT_BAD_REQ;
    printf ("tatlm1_00ActExec ERRORBAD user data: %d\n",elem->user_data);
    break;
  }
  if(vl_Error != TAT_ERROR_OFF)
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
int32_t tatlm1_02ActSet (struct dth_element *elem, void *Value)
{
  //VIDEO_SYSLOG(LOG_INFO, "setting elem %s", elem->path);

  int32_t vl_Error=TAT_ERROR_OFF;    	
  switch (elem->type){

  case DTH_TYPE_U8:
    VIDEO_param_array [elem->user_data]= *(uint8_t*)Value;
    break;
  case DTH_TYPE_S8:
    VIDEO_param_array [elem->user_data]= *(int8_t*)Value;
    break;
  case DTH_TYPE_U16:
    VIDEO_param_array [elem->user_data]= *(uint16_t*)Value;
    break;
  case DTH_TYPE_S16:
    VIDEO_param_array [elem->user_data]= *(int16_t*)Value;
    break;
  case DTH_TYPE_U32:
    VIDEO_param_array [elem->user_data]= *(uint32_t*)Value;
    break;
  case DTH_TYPE_S32:
    VIDEO_param_array [elem->user_data]= *(int32_t*)Value;
    break;	
  case DTH_TYPE_U64:
    VIDEO_param_array [elem->user_data]= *(uint32_t*)Value;
    break;
  case DTH_TYPE_S64:
    VIDEO_param_array [elem->user_data]= *(int32_t*)Value;
    break;
  case DTH_TYPE_STRING:
    if ((int8_t*)Value!=NULL)
    {
      uint32_t len =strlen((const char*)Value);
      if (VIDEO_string_array[elem->user_data]!=NULL)
      { 
        free(VIDEO_string_array[elem->user_data]);
      }
      VIDEO_string_array[elem->user_data]=(int8_t*)calloc(len+1,sizeof(int8_t));
      strncpy((char*)VIDEO_string_array[elem->user_data],(const char*)Value,len);
    }
    else
    {
      printf (" tatl1_02ActADCParam_Set (int8_t*)Value) ==NULL  \n");	 	
    }
    break;		
  default:
    printf ("tatl1_02ActVIDEOParam_Set problem with TYPE\n"); 
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
int32_t tatlm1_03ActGet (struct dth_element *elem, void *Value)
{
  int32_t vl_Error=TAT_ERROR_OFF;

  switch (elem->type){

  case DTH_TYPE_U8:
    *((uint8_t*)Value)= VIDEO_param_array [elem->user_data];
    break;
  case DTH_TYPE_S8:
    *((int8_t*)Value)= VIDEO_param_array [elem->user_data];
    break;
  case DTH_TYPE_U16:
    *((uint16_t*)Value)= VIDEO_param_array [elem->user_data];
    break;
  case DTH_TYPE_S16:
    *((int16_t*)Value)= VIDEO_param_array [elem->user_data];
    break;
  case DTH_TYPE_U32:
    *((uint32_t*)Value)= VIDEO_param_array [elem->user_data];
    break;
  case DTH_TYPE_S32:
    *((int32_t*)Value)= VIDEO_param_array [elem->user_data];
    break;	
  case DTH_TYPE_U64:
    *((uint64_t*)Value)= VIDEO_param_array [elem->user_data];
    break;
  case DTH_TYPE_S64:
    *((int64_t*)Value)= VIDEO_param_array [elem->user_data];
    break;
  case DTH_TYPE_STRING:
    if (VIDEO_string_array[elem->user_data]!=NULL)
    {
      strncpy((char*)Value,(const char*)VIDEO_string_array[elem->user_data],strlen((char*)VIDEO_string_array[elem->user_data]));
    }
    else 
    {
      strncpy((char*)Value,"NULL",5);			
    }
    break;		
  default:
    printf ("tatl1_03ActVIDEOParam_Get TYPE not available\n");
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
int32_t tatlm1_04ActSetDynamic (struct dth_element *elem, void *Value)
{
  int32_t vl_Error=TAT_ERROR_OFF;
  	
  /*CAMERA*/
  t_CameraServ_SensorProperties  s_SensorProperties; 
  t_CameraServ_ZoomProperties    s_ZoomProperties;
  t_CameraServ_FlashProperties   s_FlashProperties;
  e_CameraServ_SensorId_t vl_SensorId; 
  e_CameraServ_FocusMode_t vl_FocusMode;
  uint32_t   vl_FrameRate; 
 /* e_VidServ_WhiteBalance_t vl_WhiteBalance;
  int32_t    vl_Contrast;
  int32_t    vl_Saturation;
  uint32_t   vl_Brightness;
*/
  
  /*DISPLAY*/
  e_VidServ_DisplayId_t vl_DisplayId;
  uint32_t vl_PwmValue;


  switch (elem->type){

  case DTH_TYPE_U8:
    VIDEO_param_array [elem->user_data]= *(uint8_t*)Value;
    break;
  case DTH_TYPE_S8:
    VIDEO_param_array [elem->user_data]= *(int8_t*)Value;
    break;
  case DTH_TYPE_U16:
    VIDEO_param_array [elem->user_data]= *(uint16_t*)Value;
    break;
  case DTH_TYPE_S16:
    VIDEO_param_array [elem->user_data]= *(int16_t*)Value;
    break;
  case DTH_TYPE_U32:
    VIDEO_param_array [elem->user_data]= *(uint32_t*)Value;
    break;
  case DTH_TYPE_S32:
    VIDEO_param_array [elem->user_data]= *(int32_t*)Value;
    break;	
  case DTH_TYPE_U64:
    VIDEO_param_array [elem->user_data]= *(uint32_t*)Value;
    break;
  case DTH_TYPE_S64:
    VIDEO_param_array [elem->user_data]= *(int32_t*)Value;
    break;
  case DTH_TYPE_STRING:
    if ((int8_t*)Value!=NULL)
    {
      uint32_t len =strlen((const char*)Value);
      if (VIDEO_string_array[elem->user_data]!=NULL)
      { 
        free(VIDEO_string_array[elem->user_data]);
      }
      VIDEO_string_array[elem->user_data]=(int8_t*)calloc(len+1,sizeof(int8_t));
      strncpy((char*)VIDEO_string_array[elem->user_data],(const char*)Value,len);
    }
    else
    {
      printf (" tatlm1_04ActSetDynamic (int8_t*)Value) ==NULL  \n");	 	
    }
    break;		
  default:
    printf ("tatlm1_04ActSetDynamic problem with TYPE\n"); 
    vl_Error=TAT_ERROR_CASE;
    break;
  }


  /* Used for dynamic panel*/
  switch(elem->user_data)
  {	
    /*CAMERA Dynamic Panel*/
  case IN_FOCUS_MODE_1:
  case IN_FLASH_TYPE_1:
  case IN_FLASH_MODE_1:
  case IN_FRAME_RATE_1:
  case IN_ZOOM_TYPE_1:
  case IN_ZOOM_VALUE_1:
 /* case IN_DYNWHITEBALANCE_1:
  case IN_DYNSATURATION_1:
  case IN_DYNCONTRAST_1:
  case IN_DYNBRIGHTNESS_1:*/
    vl_SensorId= SENSOR_PRIMARY;
    vl_FocusMode=VIDEO_param_array [IN_FOCUS_MODE_1];
    vl_FrameRate= VIDEO_param_array [IN_FRAME_RATE_1];
    /*vl_Contrast=VIDEO_param_array [IN_DYNCONTRAST_1];
    vl_Saturation=VIDEO_param_array [IN_DYNSATURATION_1];
    vl_Brightness=VIDEO_param_array [IN_DYNBRIGHTNESS_1];
    vl_WhiteBalance=VIDEO_param_array [IN_DYNWHITEBALANCE_1];
    */
    s_FlashProperties.FlashType=VIDEO_param_array [IN_FLASH_TYPE_1];
    s_FlashProperties.FlashMode=VIDEO_param_array [IN_FLASH_MODE_1];
    
    s_SensorProperties.FocusMode= &vl_FocusMode; 
    /*s_SensorProperties.WhiteBalance= &vl_WhiteBalance; 
    s_SensorProperties.Brightness=&vl_Brightness; 
    s_SensorProperties.Saturation=&vl_Saturation; 
    s_SensorProperties.Contrast=&vl_Contrast; */
    s_SensorProperties.FlashProperties=&s_FlashProperties;
    s_SensorProperties.FrameRate= &vl_FrameRate;
    s_ZoomProperties.ZoomValue= VIDEO_param_array [IN_ZOOM_VALUE_1];
    s_ZoomProperties.ZoomType= VIDEO_param_array [IN_ZOOM_TYPE_1];

    /*Zoom not taken into account*/
    vl_Error=CameraServ_CameraSetSensorProp (&vl_SensorId, &s_SensorProperties);
    if (vl_Error != CAMERASERV_NO_ERROR)
    {
	printf ("Set camera properties CameraServ_CameraSetSensorProp  vl_Error: %d\n", vl_Error);
    }
    else
    {
	vl_Error=CameraServ_CameraSetSensorZoom (&s_ZoomProperties);
	if (vl_Error != CAMERASERV_NO_ERROR)
	{
	    printf ("Set zoom properties CameraServ_CameraSetSensorProp  vl_Error: %d\n", vl_Error);
	}
    }
    break;
  case IN_FOCUS_MODE_2:
  case IN_FLASH_TYPE_2:
  case IN_FLASH_MODE_2:
  case IN_FRAME_RATE_2:
  case IN_ZOOM_TYPE_2:
  case IN_ZOOM_VALUE_2:
 /* case IN_DYNWHITEBALANCE_2:
  case IN_DYNSATURATION_2:
  case IN_DYNCONTRAST_2:
  case IN_DYNBRIGHTNESS_2:  */
    vl_SensorId= SENSOR_SECONDARY;
    vl_FocusMode=VIDEO_param_array [IN_FOCUS_MODE_2];
    vl_FrameRate= VIDEO_param_array [IN_FRAME_RATE_2];
   /* vl_Contrast=VIDEO_param_array [IN_DYNCONTRAST_2];
    vl_Saturation=VIDEO_param_array [IN_DYNSATURATION_2];
    vl_Brightness=VIDEO_param_array [IN_DYNBRIGHTNESS_2];
    vl_WhiteBalance=VIDEO_param_array [IN_DYNWHITEBALANCE_2];*/
    
    s_FlashProperties.FlashType=VIDEO_param_array [IN_FLASH_TYPE_2];
    s_FlashProperties.FlashMode=VIDEO_param_array [IN_FLASH_MODE_2];
    
    s_SensorProperties.FocusMode= &vl_FocusMode; 
    /*s_SensorProperties.WhiteBalance= &vl_WhiteBalance; 
    s_SensorProperties.Brightness=&vl_Brightness; 
    s_SensorProperties.Saturation=&vl_Saturation; 
    s_SensorProperties.Contrast=&vl_Contrast; */
    s_SensorProperties.FlashProperties=&s_FlashProperties;
    s_SensorProperties.FrameRate= &vl_FrameRate;
    s_ZoomProperties.ZoomValue= VIDEO_param_array [IN_ZOOM_VALUE_2];
    s_ZoomProperties.ZoomType= VIDEO_param_array [IN_ZOOM_TYPE_2];

    /*Zoom not taken into account*/
    vl_Error=CameraServ_CameraSetSensorProp (&vl_SensorId, &s_SensorProperties);
    if (vl_Error != CAMERASERV_NO_ERROR)
    {
	printf ("Set camera properties CameraServ_CameraSetSensorProp  vl_Error: %d\n", vl_Error);
    }
    else
    {
    	vl_Error=CameraServ_CameraSetSensorZoom (&s_ZoomProperties);
	if (vl_Error != CAMERASERV_NO_ERROR)
	{
	    printf ("Set zoom properties CameraServ_CameraSetSensorProp  vl_Error: %d\n", vl_Error);
	}
    }
    break;
    /*DISPLAY Dynamic Panel*/
  case IN_MAINDIS_BLPWMVAL:
    vl_DisplayId=DISPLAY_MAIN;  
    vl_PwmValue= VIDEO_param_array [IN_MAINDIS_BLPWMVAL];
    vl_Error=VidServ_BacklightSetPWMValue (vl_DisplayId, vl_PwmValue);
    if (vl_Error != VIDSERV_NO_ERROR)
    {
	printf ("Back Light VidServ_BacklightSetPWMValue  vl_Error: %d\n", vl_Error);
    }
    break;
  }

  return vl_Error ;
}

//int32_t tatlm1_05RawSavingManagement(char *FakeSinkInstance)
int32_t tatlm1_05RawSavingManagement(void)
{
int32_t vl_Error=TAT_ERROR_OFF;
char command[100];
char RawFlag[20];

  if(VIDEO_param_array [IN_PICTURE_FORMAT]==ENUM_VIDEO_CAMERA_STATICCONF_SET_PICTURE_IN_PICTUREFORMAT_RAW)
  {
    strcpy(RawFlag,"OMX_TRUE");
  }
  else
  {
    strcpy(RawFlag,"OMX_FALSE");
  }
  sprintf(command,"alias RAW_ENABLE %s\n",RawFlag);
  CameraServ_SetMmteAlias(fd_input,fd_popen,command,MMTE_TIMEOUT);	

  return vl_Error ;
}

int32_t tatlm1_06MmteStringManagement(char *alias,char *status)
{
int32_t vl_Error=TAT_ERROR_OFF;
char command[100];

  sprintf(command,"alias %s %s\n",alias,status);
  CameraServ_SetMmteAlias(fd_input,fd_popen,command,MMTE_TIMEOUT);	

  return vl_Error ;
}

int32_t tatlm1_07InternalIqtState(char *IqtState)
{
uint32_t vl_error=TAT_ERROR_OFF; 

  vl_error=tatlm1_06MmteStringManagement("IQT_CURRENT_STATE",IqtState);
  return vl_error; 
}


int32_t tatlm1_08DthAclManagement(char *pp_ActionName,int vp_enable)
{
char command[100];
int32_t vl_error = 0;
 
  memset(command,0,sizeof(command));
  if(vp_enable==1)
  {
    sprintf(command,"chmod +x /mnt/9p/DTH/VIDEO/Camera/Staticconf/%s/value &",pp_ActionName);	
  }
  else
  {
    sprintf(command,"chmod -x /mnt/9p/DTH/VIDEO/Camera/Staticconf/%s/value &",pp_ActionName);	
  }
  printf("%s\n",command);
#if defined(DTH_ACTION_ACL_MGT)
  system(command);
#endif

  return vl_error;
}

int32_t tatlm1_09AclManagementForTheState(int State)
{
int32_t vl_error = 0;

  switch(State)
  {
    case CURRENT_STATE_INIT:
      tatlm1_08DthAclManagement("Shot",ENABLED);
      //tatlm1_08DthAclManagement("START_MMTE",DISABLED);
      //tatlm1_08DthAclManagement("STOP_MMTE",ENABLED);
      //tatlm1_08DthAclManagement("TAKE_PICTURE",DISABLED);
      //tatlm1_08DthAclManagement("STOP_BURST",DISABLED);
      //tatlm1_08DthAclManagement("START_PREVIEW",DISABLED);
      //tatlm1_08DthAclManagement("STOP_PREVIEW",DISABLED);
      //tatlm1_08DthAclManagement("START_VIDEO",DISABLED);
      //tatlm1_08DthAclManagement("STOP_VIDEO",DISABLED);
      //tatlm1_08DthAclManagement("START_IDLE",ENABLED);
      //tatlm1_08DthAclManagement("STOP_IDLE",DISABLED);
      //tatlm1_08DthAclManagement("Pause",DISABLED);
      //tatlm1_08DthAclManagement("Resume",DISABLED);

      //tatlm1_08DthAclManagement("Init_Sensor",ENABLED);
      //tatlm1_08DthAclManagement("Set_Video",ENABLED);
      //tatlm1_08DthAclManagement("Set_Preview",ENABLED);
      //tatlm1_08DthAclManagement("Set_Picture",ENABLED);
      break;
    case CURRENT_STATE_IDLE:
      //tatlm1_08DthAclManagement("START_MMTE",DISABLED);
      //tatlm1_08DthAclManagement("STOP_MMTE",ENABLED);
      //tatlm1_08DthAclManagement("TAKE_PICTURE",DISABLED);
      //tatlm1_08DthAclManagement("STOP_BURST",DISABLED);
      //tatlm1_08DthAclManagement("START_PREVIEW",ENABLED);
      //tatlm1_08DthAclManagement("STOP_PREVIEW",DISABLED);
      //tatlm1_08DthAclManagement("START_VIDEO",DISABLED);
      //tatlm1_08DthAclManagement("STOP_VIDEO",DISABLED);
      //tatlm1_08DthAclManagement("START_IDLE",DISABLED);
      //tatlm1_08DthAclManagement("STOP_IDLE",DISABLED);
      //tatlm1_08DthAclManagement("Pause",DISABLED);
      //tatlm1_08DthAclManagement("Resume",DISABLED);

      //tatlm1_08DthAclManagement("Init_Sensor",ENABLED);
      //tatlm1_08DthAclManagement("Set_Video",ENABLED);
      //tatlm1_08DthAclManagement("Set_Preview",ENABLED);
      //tatlm1_08DthAclManagement("Set_Picture",ENABLED);
      break;
    case CURRENT_STATE_VIEWFINDER:
      //tatlm1_08DthAclManagement("START_MMTE",DISABLED);
      //tatlm1_08DthAclManagement("STOP_MMTE",ENABLED);
      //tatlm1_08DthAclManagement("TAKE_PICTURE",ENABLED);
      //tatlm1_08DthAclManagement("STOP_BURST",DISABLED);
      //tatlm1_08DthAclManagement("START_PREVIEW",DISABLED);
      //tatlm1_08DthAclManagement("STOP_PREVIEW",ENABLED);
      //tatlm1_08DthAclManagement("START_VIDEO",ENABLED);
      //tatlm1_08DthAclManagement("STOP_VIDEO",DISABLED);
      //tatlm1_08DthAclManagement("START_IDLE",DISABLED);
      //tatlm1_08DthAclManagement("STOP_IDLE",DISABLED);
      //tatlm1_08DthAclManagement("Pause",ENABLED);
      //tatlm1_08DthAclManagement("Resume",DISABLED);

      //tatlm1_08DthAclManagement("Init_Sensor",ENABLED);
      //tatlm1_08DthAclManagement("Set_Video",ENABLED);
      //tatlm1_08DthAclManagement("Set_Preview",ENABLED);
      //tatlm1_08DthAclManagement("Set_Picture",ENABLED);
      break;
    case CURRENT_STATE_STILLPICTURE:
      //tatlm1_08DthAclManagement("START_MMTE",DISABLED);
      //tatlm1_08DthAclManagement("STOP_MMTE",ENABLED);
      //tatlm1_08DthAclManagement("TAKE_PICTURE",DISABLED);
      //tatlm1_08DthAclManagement("STOP_BURST",ENABLED);
      //tatlm1_08DthAclManagement("START_PREVIEW",DISABLED);
      //tatlm1_08DthAclManagement("STOP_PREVIEW",DISABLED);
      //tatlm1_08DthAclManagement("START_VIDEO",DISABLED);
      //tatlm1_08DthAclManagement("STOP_VIDEO",DISABLED);
      //tatlm1_08DthAclManagement("START_IDLE",DISABLED);
      //tatlm1_08DthAclManagement("STOP_IDLE",DISABLED);
      //tatlm1_08DthAclManagement("Pause",DISABLED);
      //tatlm1_08DthAclManagement("Resume",DISABLED);

      //tatlm1_08DthAclManagement("Init_Sensor",ENABLED);
      //tatlm1_08DthAclManagement("Set_Video",ENABLED);
      //tatlm1_08DthAclManagement("Set_Preview",ENABLED);
      //tatlm1_08DthAclManagement("Set_Picture",ENABLED);
      break;
    case CURRENT_STATE_VIDEO:
      //tatlm1_08DthAclManagement("START_MMTE",DISABLED);
      //tatlm1_08DthAclManagement("STOP_MMTE",ENABLED);
      //tatlm1_08DthAclManagement("TAKE_PICTURE",DISABLED);
      //tatlm1_08DthAclManagement("STOP_BURST",DISABLED);
      //tatlm1_08DthAclManagement("START_PREVIEW",DISABLED);
      //tatlm1_08DthAclManagement("STOP_PREVIEW",DISABLED);
      //tatlm1_08DthAclManagement("START_VIDEO",DISABLED);
      //tatlm1_08DthAclManagement("STOP_VIDEO",ENABLED);
      //tatlm1_08DthAclManagement("START_IDLE",DISABLED);
      //tatlm1_08DthAclManagement("STOP_IDLE",DISABLED);
      //tatlm1_08DthAclManagement("Pause",ENABLED);
      //tatlm1_08DthAclManagement("Resume",DISABLED);

      //tatlm1_08DthAclManagement("Init_Sensor",ENABLED);
      //tatlm1_08DthAclManagement("Set_Video",ENABLED);
      //tatlm1_08DthAclManagement("Set_Preview",ENABLED);
      //tatlm1_08DthAclManagement("Set_Picture",ENABLED);
      break;
  case CURRENT_STATE_PAUSE :
    //tatlm1_08DthAclManagement("START_MMTE",DISABLED);
    //tatlm1_08DthAclManagement("STOP_MMTE",ENABLED);
    //tatlm1_08DthAclManagement("TAKE_PICTURE",DISABLED);
    //tatlm1_08DthAclManagement("STOP_BURST",DISABLED);
    //tatlm1_08DthAclManagement("START_PREVIEW",DISABLED);
    //tatlm1_08DthAclManagement("STOP_PREVIEW",DISABLED);
    //tatlm1_08DthAclManagement("START_VIDEO",DISABLED);
    //tatlm1_08DthAclManagement("STOP_VIDEO",DISABLED);
    //tatlm1_08DthAclManagement("START_IDLE",DISABLED);
    //tatlm1_08DthAclManagement("STOP_IDLE",DISABLED);
    //tatlm1_08DthAclManagement("Pause",DISABLED);
    //tatlm1_08DthAclManagement("Resume",ENABLED);

    //tatlm1_08DthAclManagement("Init_Sensor",ENABLED);
    //tatlm1_08DthAclManagement("Set_Video",ENABLED);
    //tatlm1_08DthAclManagement("Set_Preview",ENABLED);
    //tatlm1_08DthAclManagement("Set_Picture",ENABLED);
    break;
  default:
    /* for start mmte */
    //tatlm1_08DthAclManagement("START_MMTE",ENABLED);
    //tatlm1_08DthAclManagement("STOP_MMTE",DISABLED);
    //tatlm1_08DthAclManagement("TAKE_PICTURE",DISABLED);
    //tatlm1_08DthAclManagement("STOP_BURST",DISABLED);
    //tatlm1_08DthAclManagement("START_PREVIEW",DISABLED);
    //tatlm1_08DthAclManagement("STOP_PREVIEW",DISABLED);
    //tatlm1_08DthAclManagement("START_VIDEO",DISABLED);
    //tatlm1_08DthAclManagement("STOP_VIDEO",DISABLED);
    //tatlm1_08DthAclManagement("START_IDLE",DISABLED);
    //tatlm1_08DthAclManagement("STOP_IDLE",DISABLED);
    //tatlm1_08DthAclManagement("Pause",DISABLED);
    //tatlm1_08DthAclManagement("Resume",DISABLED);

    //tatlm1_08DthAclManagement("Init_Sensor",DISABLED);
    //tatlm1_08DthAclManagement("Set_Video",DISABLED);
    //tatlm1_08DthAclManagement("Set_Preview",DISABLED);
    //tatlm1_08DthAclManagement("Set_Picture",DISABLED);
    break;
  }
  return vl_error;
}

int32_t tatlm1_10ActSetResolutionVF(int Value)
{

  int32_t vl_Error=TAT_ERROR_OFF;  
  	
  switch (Value){

  case Set_Preview_Resolution_SQCIF:
	VIDEO_param_array [IN_PREVIEW_WIDTH]=128;
	VIDEO_param_array [IN_PREVIEW_HEIGHT]=96;
    break;
  case Set_Preview_Resolution_QQVGA:
	VIDEO_param_array [IN_PREVIEW_WIDTH]=160;
	VIDEO_param_array [IN_PREVIEW_HEIGHT]=120;
    break;
  case Set_Preview_Resolution_QCIF:
	VIDEO_param_array [IN_PREVIEW_WIDTH]=176;
	VIDEO_param_array [IN_PREVIEW_HEIGHT]=144;
    break;
  case Set_Preview_Resolution_QVGA:
	VIDEO_param_array [IN_PREVIEW_WIDTH]=320;
	VIDEO_param_array [IN_PREVIEW_HEIGHT]=240;
    break;
  case Set_Preview_Resolution_QVGAp:
	VIDEO_param_array [IN_PREVIEW_WIDTH]=240;
	VIDEO_param_array [IN_PREVIEW_HEIGHT]=320;
    break;
  case Set_Preview_Resolution_WQVGA:
	VIDEO_param_array [IN_PREVIEW_WIDTH]=400;
	VIDEO_param_array [IN_PREVIEW_HEIGHT]=240;
    break;	
  case Set_Preview_Resolution_CIF:
	VIDEO_param_array [IN_PREVIEW_WIDTH]=352;
	VIDEO_param_array [IN_PREVIEW_HEIGHT]=288;
    break;
  case Set_Preview_Resolution_HVGA:
	VIDEO_param_array [IN_PREVIEW_WIDTH]=480;
	VIDEO_param_array [IN_PREVIEW_HEIGHT]=320;
    break;
  case Set_Preview_Resolution_nHD:
	VIDEO_param_array [IN_PREVIEW_WIDTH]=640;
	VIDEO_param_array [IN_PREVIEW_HEIGHT]=360;
    break;
  case Set_Preview_Resolution_VGA:
	VIDEO_param_array [IN_PREVIEW_WIDTH]=640;
	VIDEO_param_array [IN_PREVIEW_HEIGHT]=480;
    break;
  case Set_Preview_Resolution_D1:
	VIDEO_param_array [IN_PREVIEW_WIDTH]=720;
	VIDEO_param_array [IN_PREVIEW_HEIGHT]=576;
    break;
  case Set_Preview_Resolution_SVGA:
	VIDEO_param_array [IN_PREVIEW_WIDTH]=800;
	VIDEO_param_array [IN_PREVIEW_HEIGHT]=600;
    break;
  case Set_Preview_Resolution_WVGA:
	VIDEO_param_array [IN_PREVIEW_WIDTH]=848;
	VIDEO_param_array [IN_PREVIEW_HEIGHT]=480;
    break;
  case Set_Preview_Resolution_WVGAp:
	VIDEO_param_array [IN_PREVIEW_WIDTH]=480;
	VIDEO_param_array [IN_PREVIEW_HEIGHT]=848;
    break;
  case Set_Preview_Resolution_qHD:
	VIDEO_param_array [IN_PREVIEW_WIDTH]=960;
	VIDEO_param_array [IN_PREVIEW_HEIGHT]=540;
    break;
  case  Set_Preview_Resolution_XVGA:
	VIDEO_param_array [IN_PREVIEW_WIDTH]=1024;
	VIDEO_param_array [IN_PREVIEW_HEIGHT]=768;
    break;
  case Set_Preview_Resolution_SXGA:
	VIDEO_param_array [IN_PREVIEW_WIDTH]=1280;
	VIDEO_param_array [IN_PREVIEW_HEIGHT]=1024;
    break;
  case Set_Preview_Resolution_UXGA:
	VIDEO_param_array [IN_PREVIEW_WIDTH]=1600;
	VIDEO_param_array [IN_PREVIEW_HEIGHT]=1200;
    break;
  case Set_Preview_Resolution_QXGA:
	VIDEO_param_array [IN_PREVIEW_WIDTH]=2048;
	VIDEO_param_array [IN_PREVIEW_HEIGHT]=1536;
    break;
  case Set_Preview_Resolution_5MP:
	VIDEO_param_array [IN_PREVIEW_WIDTH]=2608;
	VIDEO_param_array [IN_PREVIEW_HEIGHT]=1952;
    break;
		
  default:
    SYSLOG(LOG_ERR, "Resolution not supported");
    vl_Error=TAT_ERROR_CASE;
    break;	
  }		
  return vl_Error ;
}	
int32_t tatlm1_11ActSetResolutionStill(int Value)
{

  int32_t vl_Error=TAT_ERROR_OFF;  
  	
  switch (Value){

  case Set_Picture_Resolution_SQCIF:
	VIDEO_param_array [IN_PICTURE_WIDTH]=128;
	VIDEO_param_array [IN_PICTURE_HEIGHT]=96;
    break;
  case Set_Picture_Resolution_QQVGA:
	VIDEO_param_array [IN_PICTURE_WIDTH]=160;
	VIDEO_param_array [IN_PICTURE_HEIGHT]=120;
    break;
  case Set_Picture_Resolution_QCIF:
	VIDEO_param_array [IN_PICTURE_WIDTH]=176;
	VIDEO_param_array [IN_PICTURE_HEIGHT]=144;
    break;
  case Set_Picture_Resolution_QVGA:
	VIDEO_param_array [IN_PICTURE_WIDTH]=320;
	VIDEO_param_array [IN_PICTURE_HEIGHT]=240;
    break;
  case Set_Picture_Resolution_QVGAp:
	VIDEO_param_array [IN_PICTURE_WIDTH]=240;
	VIDEO_param_array [IN_PICTURE_HEIGHT]=320;
    break;
  case Set_Picture_Resolution_WQVGA:
	VIDEO_param_array [IN_PICTURE_WIDTH]=400;
	VIDEO_param_array [IN_PICTURE_HEIGHT]=240;
    break;	
  case Set_Picture_Resolution_CIF:
	VIDEO_param_array [IN_PICTURE_WIDTH]=352;
	VIDEO_param_array [IN_PICTURE_HEIGHT]=288;
    break;
  case Set_Picture_Resolution_HVGA:
	VIDEO_param_array [IN_PICTURE_WIDTH]=480;
	VIDEO_param_array [IN_PICTURE_HEIGHT]=320;
    break;
  case Set_Picture_Resolution_nHD:
	VIDEO_param_array [IN_PICTURE_WIDTH]=640;
	VIDEO_param_array [IN_PICTURE_HEIGHT]=360;
    break;
  case Set_Picture_Resolution_VGA:
	VIDEO_param_array [IN_PICTURE_WIDTH]=640;
	VIDEO_param_array [IN_PICTURE_HEIGHT]=480;
    break;
  case Set_Picture_Resolution_D1:
	VIDEO_param_array [IN_PICTURE_WIDTH]=720;
	VIDEO_param_array [IN_PICTURE_HEIGHT]=576;
    break;
  case Set_Picture_Resolution_SVGA:
	VIDEO_param_array [IN_PICTURE_WIDTH]=800;
	VIDEO_param_array [IN_PICTURE_HEIGHT]=600;
    break;
  case Set_Picture_Resolution_WVGA:
	VIDEO_param_array [IN_PICTURE_WIDTH]=848;
	VIDEO_param_array [IN_PICTURE_HEIGHT]=480;
    break;
  case Set_Picture_Resolution_qHD:
	VIDEO_param_array [IN_PICTURE_WIDTH]=960;
	VIDEO_param_array [IN_PICTURE_HEIGHT]=540;
    break;
  case  Set_Picture_Resolution_XVGA:
	VIDEO_param_array [IN_PICTURE_WIDTH]=1024;
	VIDEO_param_array [IN_PICTURE_HEIGHT]=768;
    break;
  case Set_Picture_Resolution_SXGA:
	VIDEO_param_array [IN_PICTURE_WIDTH]=1280;
	VIDEO_param_array [IN_PICTURE_HEIGHT]=1024;
    break;
  case Set_Picture_Resolution_UXGA:
	VIDEO_param_array [IN_PICTURE_WIDTH]=1600;
	VIDEO_param_array [IN_PICTURE_HEIGHT]=1200;
    break;
  case Set_Picture_Resolution_QXGA:
	VIDEO_param_array [IN_PICTURE_WIDTH]=2048;
	VIDEO_param_array [IN_PICTURE_HEIGHT]=1536;
    break;
  case Set_Picture_Resolution_5MP:
	VIDEO_param_array [IN_PICTURE_WIDTH]=2608;
	VIDEO_param_array [IN_PICTURE_HEIGHT]=1952;
    break;
		
  default:
    SYSLOG(LOG_ERR, "Resolution not supported");
    vl_Error=TAT_ERROR_CASE;
    break;	
  }		
  return vl_Error ;
}	

int32_t tatlm1_12ActSetResolutionVideo(int Value)
{

  int32_t vl_Error=TAT_ERROR_OFF;  
  	
  switch (Value){

  case Set_Video_Resolution_SQCIF:
	VIDEO_param_array [IN_VIDEO_WIDTH]=128;
	VIDEO_param_array [IN_VIDEO_HEIGHT]=96;
    break;
  case Set_Video_Resolution_QQVGA:
	VIDEO_param_array [IN_VIDEO_WIDTH]=160;
	VIDEO_param_array [IN_VIDEO_HEIGHT]=120;
    break;
  case Set_Video_Resolution_QCIF:
	VIDEO_param_array [IN_VIDEO_WIDTH]=176;
	VIDEO_param_array [IN_VIDEO_HEIGHT]=144;
    break;
  case Set_Video_Resolution_QVGA:
	VIDEO_param_array [IN_VIDEO_WIDTH]=320;
	VIDEO_param_array [IN_VIDEO_HEIGHT]=240;
    break;
  case Set_Video_Resolution_QVGAp:
	VIDEO_param_array [IN_VIDEO_WIDTH]=240;
	VIDEO_param_array [IN_VIDEO_HEIGHT]=320;
    break;
  case Set_Video_Resolution_WQVGA:
	VIDEO_param_array [IN_VIDEO_WIDTH]=400;
	VIDEO_param_array [IN_VIDEO_HEIGHT]=240;
    break;	
  case Set_Video_Resolution_CIF:
	VIDEO_param_array [IN_VIDEO_WIDTH]=352;
	VIDEO_param_array [IN_VIDEO_HEIGHT]=288;
    break;
  case Set_Video_Resolution_HVGA:
	VIDEO_param_array [IN_VIDEO_WIDTH]=480;
	VIDEO_param_array [IN_VIDEO_HEIGHT]=320;
    break;
  case Set_Video_Resolution_nHD:
	VIDEO_param_array [IN_VIDEO_WIDTH]=640;
	VIDEO_param_array [IN_VIDEO_HEIGHT]=360;
    break;
  case Set_Video_Resolution_VGA:
	VIDEO_param_array [IN_VIDEO_WIDTH]=640;
	VIDEO_param_array [IN_VIDEO_HEIGHT]=480;
    break;
  case Set_Video_Resolution_D1:
	VIDEO_param_array [IN_VIDEO_WIDTH]=720;
	VIDEO_param_array [IN_VIDEO_HEIGHT]=576;
    break;
  case Set_Video_Resolution_SVGA:
	VIDEO_param_array [IN_VIDEO_WIDTH]=800;
	VIDEO_param_array [IN_VIDEO_HEIGHT]=600;
    break;
  case Set_Video_Resolution_WVGA:
	VIDEO_param_array [IN_VIDEO_WIDTH]=848;
	VIDEO_param_array [IN_VIDEO_HEIGHT]=480;
    break;
  case Set_Video_Resolution_qHD:
	VIDEO_param_array [IN_VIDEO_WIDTH]=960;
	VIDEO_param_array [IN_VIDEO_HEIGHT]=540;
    break;
  case  Set_Video_Resolution_XVGA:
	VIDEO_param_array [IN_VIDEO_WIDTH]=1024;
	VIDEO_param_array [IN_VIDEO_HEIGHT]=768;
    break;
  case Set_Video_Resolution_SXGA:
	VIDEO_param_array [IN_VIDEO_WIDTH]=1280;
	VIDEO_param_array [IN_VIDEO_HEIGHT]=1024;
    break;
  case Set_Video_Resolution_UXGA:
	VIDEO_param_array [IN_VIDEO_WIDTH]=1600;
	VIDEO_param_array [IN_VIDEO_HEIGHT]=1200;
    break;
  case Set_Video_Resolution_QXGA:
	VIDEO_param_array [IN_VIDEO_WIDTH]=2048;
	VIDEO_param_array [IN_VIDEO_HEIGHT]=1536;
    break;
  case Set_Video_Resolution_5MP:
	VIDEO_param_array [IN_VIDEO_WIDTH]=2608;
	VIDEO_param_array [IN_VIDEO_HEIGHT]=1952;
    break;
		
  default:
    SYSLOG(LOG_ERR, "Resolution not supported");
    vl_Error=TAT_ERROR_CASE;
    break;	
  }		
  return vl_Error ;
}	

