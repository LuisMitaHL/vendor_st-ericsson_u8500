/*
 * =====================================================================================
 * 
 * 	Filename:  wavconvexe.c
 * 
 *     	Description:  source code of wave file convert application
 * 
 *      © Copyright ST-Ericsson, 2009. All Rights Reserved
 * 
 * =====================================================================================
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "hats_audio.h"
#include "hats_audio.hi"

int main(int argc, char * argv[])
{
  e_HatsAudioDevice_t Device = NB_OF_AUDIO_DEVICE;

	if((argc != 3)&& (argc != 5))
  {
  	goto syntax;
  }
	if (argc == 5)
  {
 	if(( argv[1][0] == '-') && (argv[1][1] == 'd'))
    {
      	/* device name detected */
        if (strncmp(argv[2], "default", sizeof("default" )) == 0)
        {
        	Device = AUDIO_DEVICE_0;
  				printf("\n device default found\n");
        }
        else if(strncmp(argv[2], "hdmi", sizeof("hdmi" )) == 0)
        {
        	Device = AUDIO_DEVICE_HDMI;
  				printf("\n device hdmi found\n");
        }
		}
    if (Device != NB_OF_AUDIO_DEVICE)
    {
    	return ConvWaveFile(Device,argv[3], argv[4]);
	  }
  }
  else
  {
   	return ConvWaveFile(AUDIO_DEVICE_0,argv[1], argv[2]);
  }
  
syntax:
  	printf("\nsyntax %s (-d) <input file> <output file>\n", argv[0]);
  	printf("\n\t -d <device name> if option not used default device selected\n");
  	printf("\t\t device list: default,hdmi\n");
  	if(argc != 1)
    	return -1;
    else
    	return 0;
    
}
