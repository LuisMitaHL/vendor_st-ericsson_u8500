/*****************************************************************************/
/**
*  Copyright (C) ST-Ericsson 2009
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Propose Video function for tests (Display (Main, Sub, TV-OUT), Camera)
* \author  ST-Ericsson
*/
/*****************************************************************************/
 
#include <stdint.h>			/* uint32_t ... */
#include <stdio.h> /* printf ... */
#include <string.h> /* strlen ... */
#include <stdlib.h> /* exit ... */

#include "../incl/VideoServices.h"

/*
	Service allowing to compare beginning of argument arg with a string s1
	return 	
		=0 if equal
*/
int32_t compareArgString(char *s1, char *arg)
{
	return strncmp(s1,arg,strlen(s1));
}

/*
	Print help
*/
void printHelp()
{					
	printf("-h, --help:		help\n");
	printf("-m=#:  mode of test\n\
	(#=0): Display Mode");
	printf("-d=#:  display Id\n\
	(#=%d): Main LCD\n\
	(#=%d): Sub LCD\n\
	(#=%d): Analog TV-OUT (AV8100)\n\
	(#=%d): Analog TV-OUT (AV8100)\n\
	(#=%d): HDMI TV-OUT\n", DISPLAY_MAIN, DISPLAY_SUB, DISPLAY_TV_OUT_CVBS_AB8500, DISPLAY_TV_OUT_CVBS_AV8100, DISPLAY_TV_OUT_HDMI);
	printf("-dinit: Initialise the display\n");
	printf("-dplug: print 1 if display is plugged, else 0\n");
	printf("-dpwr=#: display power mode to apply\n\
	(#=%d): POWER OFF\n\
	(#=%d): POWER ON\n\
	(#=%d): LOW POWER\n", POWER_OFF, POWER_ON, LOW_POWER);							
	printf("-dprop=WxHxFxM: set display properties, WidthxHeightxFrequencyxScanMode\n\
	ScanMode:\n\
	(#=P): PROGRESSIVE\n\
	(#=I): INTERLACED\n");
	printf("-dgprop: get display properties => print Width, Height, ScanMode\n");
	printf("-dpwm=#: pwm to apply of display backlight (Max is 1024)\n");
	printf("-dgpwm: get pwm value of display backlight\n");
	printf("-davconn=#: Select the Av connector source\n\
	(#=%d): VIDEO\n\
	(#=%d): AUDIO\n", VIDEO_SOURCE, AUDIO_SOURCE);
	printf("-davformat=#: Select the av8100 input format\n\
	(#=%d): RGB\n\
	(#=%d): YUV\n", VID_AV8100_INPUT_PIX_RGB888, VID_AV8100_INPUT_PIX_YCBCR422);
	printf("-dpict=#: display picture, #=picture File path\n");							
	printf("-dpict2=#: for TOGGLE loop display picture 2, #=picture 2 File path\n");							
	printf("-dploop=MxD1xD2: set display picture loop ModexInterval (no loop if not used), to be used with -dpict\n\
	Mode:\n\
	(#=%d): Fixed Rate\n\
	(#=%d): Max speed\n\
	(#=%d): TOGGLE\n\
	D1: Duration of Image1\n\
    D2: Duration of Image2 (value should be 0 if loop mode not TOGGLE)\n", FIXED_RATE, MAX_SPEED, TOGGLE_LOOP);
	printf("-dsploop: stop the display picture loop\n");	
	printf("-dgals: Get ALS Value\n");

	exit(EXIT_SUCCESS);
}

int32_t main (int32_t argc, char *argv[])
{
	uint32_t vl_mode=0xFF;
	e_VidServ_DisplayId_t vl_displayId=0xFF;
	e_VidServ_PowerMode_t vl_displayPowerMode=0xFF;
	t_VidServ_DisplayResolution sl_displayProp={0xFF,0xFF,0xFF,0xFF,0xFF};
	char al_DisplayFilePath[512]="\0";
	char al_DisplayFilePath2[512]="\0";
	e_VidServ_LoopMode_t vl_LoopMode=NO_LOOP;
	uint32_t vl_LoopDuration1=0;
	uint32_t vl_LoopDuration2=0;
	uint32_t vl_initDisplayRequested=0xFF;
	uint32_t vl_GetDisplayPropRequested=0xFF;
	uint32_t vl_PwmToApply=0;
	uint32_t vl_PwmValueToApply=0xFF;
	uint32_t vl_DiplayIsPlugRequested=0xFF;
	uint32_t vl_StopPictureLoopRequested=0xFF;
	uint32_t vl_GetPwmValue=0xFF;
	uint32_t vl_GetAlsValue=0xFF;
	uint32_t vl_AlsValue=0xFF;
	uint32_t vl_AvConnectorSourceValue=0xFF;
	uint32_t vl_AV8100InputVideoFormat=0xFF;

  /* A - parse parameters */
	if(argc > 0){
        int32_t i;

        for(i=1; i<argc; i++)
            {
               if ( (compareArgString("-h",argv[i]) == 0) || (compareArgString("--help",argv[i]) == 0) )
                    {
						printHelp();
						continue;
					}
               if (compareArgString("-m=",argv[i]) == 0)
                    {
                        if(sscanf(argv[i], "-m=%d", (int32_t*)&vl_mode)==1)
						{
							continue;
						}
                    }
               if (compareArgString("-d=",argv[i]) == 0)
                    {
                        if(sscanf(argv[i], "-d=%d", (int32_t*)&vl_displayId)==1)
						{
							continue;
						}
                    }
               if (compareArgString("-davconn=",argv[i]) == 0)
                    {
                        if(sscanf(argv[i], "-davconn=%d", (int32_t*)&vl_AvConnectorSourceValue)==1)
						{
							continue;
						}
                    }
               if (compareArgString("-davformat=",argv[i]) == 0)
                    {
                        if(sscanf(argv[i], "-davformat=%d", (int32_t*)&vl_AV8100InputVideoFormat)==1)
						{
							continue;
						}
                    }
               if (compareArgString("-dinit",argv[i]) == 0)
                    {
						vl_initDisplayRequested=1;
						continue;
                    }	
               if (compareArgString("-dplug",argv[i]) == 0)
                    {
						vl_DiplayIsPlugRequested=1;
						continue;
                    }						
               if (compareArgString("-dpwr=",argv[i]) == 0)
                    {
                        if(sscanf(argv[i], "-dpwr=%d", (int32_t*)&vl_displayPowerMode)==1)
						{
							continue;
						}
                    }									
               if (compareArgString("-dprop=",argv[i]) == 0)
                    {
						if(sscanf(argv[i], "-dprop=%dx%dx%dx%c", (int32_t*)&(sl_displayProp.Width),(int32_t*)&(sl_displayProp.Height),(int32_t*)&(sl_displayProp.Frequency),(int8_t*)&(sl_displayProp.ScanMode))==4)
						{
							switch (sl_displayProp.ScanMode){
							case 0x50 /* P */: case 0x70 /* p */:												
								sl_displayProp.ScanMode=PROGRESSIVE;
							break;
							case 0x49 /* I */: case 0x69 /* i */:							
								sl_displayProp.ScanMode=INTERLACED;
							break;
							default:
								printf("Invalid scan mode\n");
								exit(EXIT_FAILURE);
							break;	
							}	/*switch (vl_selection)	*/											
							continue;
						}
                    }
               if (compareArgString("-dgprop",argv[i]) == 0)
                    {
						vl_GetDisplayPropRequested=1;
						continue;
                    }		
               if (compareArgString("-dpwm=",argv[i]) == 0)
                    {
						vl_PwmToApply=1;
						if(sscanf(argv[i], "-dpwm=%d", &vl_PwmValueToApply)==1)
						continue;
                    }			
               if (compareArgString("-dgpwm",argv[i]) == 0)
                    {
						vl_GetPwmValue=1;
						continue;
                    }																		
               if (compareArgString("-dpict=",argv[i]) == 0)
                    {
                        if(sscanf(argv[i], "-dpict=%s", &al_DisplayFilePath[0])==1)
						{
							continue;
						}
                    }
               if (compareArgString("-dpict2=",argv[i]) == 0)
                    {
                        if(sscanf(argv[i], "-dpict2=%s", &al_DisplayFilePath2[0])==1)
						{
							continue;
						}
                    }
               if (compareArgString("-dploop=",argv[i]) == 0)
                    {
						if(sscanf(argv[i], "-dploop=%dx%dx%d", (int32_t*)&(vl_LoopMode),(int32_t*)&(vl_LoopDuration1),(int32_t*)&(vl_LoopDuration2))==3)
						{										
							continue;
						}
                    }
               if (compareArgString("-dsploop",argv[i]) == 0)
                    {
						vl_StopPictureLoopRequested=1;
						continue;
                    }																			
               if (compareArgString("-dgals",argv[i]) == 0)
                    {
						vl_GetAlsValue=1;
						continue;
                    }

				/* parameter error */
				printf("parameter error: %s", argv[i]);
				exit(EXIT_FAILURE);								
            }
    } /* if(argc > 0) */


  /* B - Call related Video services functions */	
  /* Display Mode */
  if(vl_mode==0)
  {
    /* Manage AV connector source selection */
  	if(vl_AvConnectorSourceValue!=0xFF)
		{
		    if(VidServ_AVConnectorSourceSelection(vl_AvConnectorSourceValue)!=0) 
			{
				printf("Error during AV connector source selection\n");
				exit(EXIT_FAILURE);
			}						
		}
    

  	/* Initialise the display */
  	if(vl_initDisplayRequested!=0xFF)
		{
			if(vl_displayId!=0xFF)
			{
				if(VidServ_DisplayInit(vl_displayId)!=0) 
				{
					printf("Error during display plug status execution\n");
					exit(EXIT_FAILURE);
				}						
			}
			else
			{
				printf("Error: Display Id should be precised for dpwr\n");
				exit(EXIT_FAILURE);		
			}
		}

  	/* Get display plug status */
  	if(vl_DiplayIsPlugRequested!=0xFF)
		{
			uint32_t vl_Status=0xFF;
			if(vl_displayId!=0xFF)
			{
				if(VidServ_DisplayIsPlugged(vl_displayId,&vl_Status)!=0) 
				{
					printf("Error during display plug status execution\n");
					exit(EXIT_FAILURE);
				}
				switch (vl_Status){
				case 1:
					printf("Display plug status %d",vl_Status);
					printf(" => Display is plugged\n");
				break;
				case 0:
					printf("Display plug status %d",vl_Status);
					printf(" => Display is not plugged\n");
				break;
				default:
					printf("Error during display plug status execution\n");					
				break;	
				} /* switch (vl_Status)	*/							
			}
			else
			{
				printf("Error: Display Id should be precised for dpwr\n");
				exit(EXIT_FAILURE);		
			}
		}
  
  	/* Set Power Mode Management */
  	if(vl_displayPowerMode!=0xFF)
		{
			if(vl_displayId!=0xFF)
			{
				if(VidServ_DisplaySetPowerMode(vl_displayId, vl_displayPowerMode)!=0)
				{
					printf("Error during Set Display Power Mode execution\n");
					exit(EXIT_FAILURE);
				}
				else
				{
					printf("Set Power Mode Done\n");
				}				
			}
			else
			{
				printf("Error: Display Id should be precised for dpwr\n");
				exit(EXIT_FAILURE);		
			}
		}
		
	/* Set AV8100 Input Pixel Format */
	if(vl_AV8100InputVideoFormat!=0xFF)
	{
		if(VidServ_DisplayAV8100InputVideoFormat(vl_AV8100InputVideoFormat)!=0)
		{
			printf("Error during setting AV8100 Input Format\n");
			exit(EXIT_FAILURE);
		}
		else
		{
			printf("Set AV8100 Input Format Done\n");
		}
	}

	/* Set Display Properties Management */
  	if(sl_displayProp.ScanMode!=0xFF)
		{
			if(vl_displayId!=0xFF)
			{
				if(VidServ_DisplaySetProperties(vl_displayId, &sl_displayProp)!=0)
				{
					printf("Error during Set Display Properties execution\n");
					exit(EXIT_FAILURE);
				}
				else
				{
					printf("Set Display Properties Done\n");
				}				
			}
			else
			{
				printf("Error: Display Id should be precised for dprop\n");
				exit(EXIT_FAILURE);		
			}
		}

	/* Get Display Properties Management */
  	if(vl_GetDisplayPropRequested==1)
		{
            char ScanMode[50]="\0";

			if(vl_displayId!=0xFF)
			{
				t_VidServ_DisplayResolution sl_GetDisplayResolution;
				if(VidServ_DisplayGetProperties(vl_displayId, &sl_GetDisplayResolution)!=0)
				{
					printf("Error during Get Display Properties execution\n");
					exit(EXIT_FAILURE);
				}
				else
				{
				  switch (sl_GetDisplayResolution.ScanMode){
				  case NONE:
                     strcpy((char*)ScanMode, "None");
				  break;
				  case PROGRESSIVE:
                     strcpy((char*)ScanMode, "Progressive");
				  break;
				  case INTERLACED:
                     strcpy((char*)ScanMode, "Interlaced");
				  break;
				  default:
					printf("Scan Mode Error\n");					
				break;	
				} /* switch (vl_Status)	*/		                    
					printf("Display Id=%d Width=%d, Height=%d, ScanMode=%s\n", vl_displayId, sl_GetDisplayResolution.Width, sl_GetDisplayResolution.Height, ScanMode);
				}				
			}
			else
			{
				printf("Error: Display Id should be precised for dgprop\n");
				exit(EXIT_FAILURE);		
			}
		}	
		
	/* Set PWM of display backlight Management */
  	if(vl_PwmToApply==1)
		{
			if(vl_displayId!=0xFF)
			{
				if(VidServ_BacklightSetPWMValue(vl_displayId, vl_PwmValueToApply)!=0)
				{
					printf("Error during Set PWM of display backlight execution\n");
					exit(EXIT_FAILURE);
				}
				else
				{
					printf("Set PWM of display backlight Done\n");
				}				
			}
			else
			{
				printf("Error: Display Id should be precised for dpwm\n");
				exit(EXIT_FAILURE);		
			}
		}	

	/* Get PWM of display backlight Management */
	if(vl_GetPwmValue==1)
		{			
			if(vl_displayId!=0xFF)
			{
				if(VidServ_BacklightGetPWMValue(vl_displayId, &vl_PwmValueToApply)!=0)
				{
					printf("Error during Get PWM of display backlight execution\n");
					exit(EXIT_FAILURE);
				}
				else
				{
					printf("Get PWM of display backlight Done, value of PWM on display %d is %d/1024\n",vl_displayId,vl_PwmValueToApply);
				}				
			}
			else
			{
				printf("Error: Display Id should be precised for dgpwm\n");
				exit(EXIT_FAILURE);		
			}		
		}					
		
	/* Display Picture Management */
  	if(al_DisplayFilePath[0]!=0x00)
		{
			t_VidServ_ImageProperties vl_ImageProperties, vl_ImageProperties2;
			char *pl_FileExtension;
			
			/* look for last "." character */
			pl_FileExtension=strrchr((const char*)&al_DisplayFilePath[0], 0x2e);
            if(pl_FileExtension!=NULL)
            {
			  pl_FileExtension++;
            }
            else
            {
				printf("File extension is not supported\n");
				exit(EXIT_FAILURE);
            }
			
			if(strcmp(pl_FileExtension,"png")==0) vl_ImageProperties.InputPictureFormat=PNG;
			else if( (strcmp(pl_FileExtension,"raw")==0) || (strcmp(pl_FileExtension,"rgb")==0) ) vl_ImageProperties.InputPictureFormat=RGB888;
			else if( (strcmp(pl_FileExtension,"jpg")==0) || (strcmp(pl_FileExtension,"jpeg")==0) ) vl_ImageProperties.InputPictureFormat=JPEG;
			else 
			{
				printf("File extension is not supported\n");
				exit(EXIT_FAILURE);
			}
			
			vl_ImageProperties.FromFile=1;
			vl_ImageProperties.BufferAddressOrFilePath=(uint8_t *)&al_DisplayFilePath;

            /* for toggle loop, get picture2 data */
            if(al_DisplayFilePath2[0]!=0x00)	
            {
			  /* look for last "." character */
			  pl_FileExtension=strrchr((const char*)&al_DisplayFilePath2[0], 0x2e);
              if(pl_FileExtension!=NULL)
              {
			    pl_FileExtension++;
              }
              else
              {
				printf("File extension is not supported\n");
				exit(EXIT_FAILURE);
              }
			
			  if(strcmp(pl_FileExtension,"png")==0) vl_ImageProperties2.InputPictureFormat=PNG;
			  else if( (strcmp(pl_FileExtension,"raw")==0) || (strcmp(pl_FileExtension,"rgb")==0) ) vl_ImageProperties2.InputPictureFormat=RGB888;
			  else if( (strcmp(pl_FileExtension,"jpg")==0) || (strcmp(pl_FileExtension,"jpeg")==0) ) vl_ImageProperties2.InputPictureFormat=JPEG;
			  else 
			  {
				printf("File extension is not supported\n");
				exit(EXIT_FAILURE);
			  }
			
			  vl_ImageProperties2.FromFile=1;
			  vl_ImageProperties2.BufferAddressOrFilePath=(uint8_t *)&al_DisplayFilePath2;
              
            }	
						
            /* for toggle loop, get picture2 data */
            if(al_DisplayFilePath2[0]!=0x00)	
            {
			  /* look for last "." character */
			  pl_FileExtension=strrchr((const char*)&al_DisplayFilePath2[0], 0x2e);
              if(pl_FileExtension!=NULL)
              {
			    pl_FileExtension++;
              }
              else
              {
				printf("File extension is not supported\n");
				exit(EXIT_FAILURE);
              }
			
			  if(strcmp(pl_FileExtension,"png")==0) vl_ImageProperties2.InputPictureFormat=PNG;
			  else if( (strcmp(pl_FileExtension,"raw")==0) || (strcmp(pl_FileExtension,"rgb")==0) ) vl_ImageProperties2.InputPictureFormat=RGB888;
			  else if( (strcmp(pl_FileExtension,"jpg")==0) || (strcmp(pl_FileExtension,"jpeg")==0) ) vl_ImageProperties2.InputPictureFormat=JPEG;
			  else 
			  {
				printf("File extension is not supported\n");
				exit(EXIT_FAILURE);
			  }
			
			  vl_ImageProperties2.FromFile=1;
			  vl_ImageProperties2.BufferAddressOrFilePath=(uint8_t *)&al_DisplayFilePath2;
              
            }	
						
			if(vl_displayId!=0xFF)
			{
				if(VidServ_DisplayPicture(vl_displayId, &vl_ImageProperties, vl_LoopMode, vl_LoopDuration1, 0,&vl_ImageProperties2, vl_LoopDuration2,0)!=0)
				{
					printf("Error during Display Picture execution\n");
					exit(EXIT_FAILURE);
				}
				else
				{
					printf("Display Picture Done\n");
				}				
			}
			else
			{
				printf("Error: Display Id should be precised for dprop\n");
				exit(EXIT_FAILURE);		
			}
		}
	/* Stop Display Picture Loop Management */
	if(vl_StopPictureLoopRequested==1)
	{
		if(VidServ_StopDisplayPictureLoop(vl_displayId)!=0)
		{
			printf("Error during Stop Display Picture Loop execution\n");
			exit(EXIT_FAILURE);
		}
		else
		{
			printf("Stop Display Picture Loop Done\n");
		}		
	}

	/* get ALS value Management */
	if(vl_GetAlsValue==1)
	{
		if(VidServ_GetAlsValue(&vl_AlsValue)!=0)
		{
			printf("Error during get ALS value execution\n");
			exit(EXIT_FAILURE);
		}
		else
		{
			printf("Get ALS Value Done, ALS Value is %d\n",vl_AlsValue);
		}		
	}	
	
	
				
  }
  else
  {
    printf("Error: please specify mode\n");
    exit(EXIT_FAILURE);	
  }
	
	exit(EXIT_SUCCESS);
}
