/*****************************************************************************/
/**
*  Copyright (C) ST-Ericsson 2009
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Executable to execute a display loop in a separate process
* \author  ST-Ericsson
*/
/*****************************************************************************/
 
#include <stdint.h>			/* uint32_t ... */
#include <stdio.h>			/* printf ... */
#include <stdlib.h>		/* malloc(), free() */	
#include <unistd.h> 
#include <linux/fb.h>		/* fb struct & var */
#include <sys/time.h> 
#include <unistd.h> 
#include <sys/ioctl.h>
#include <signal.h>

#include "../incl/VideoServices.h"
#include "VideoServices.hi"

uint32_t v_fb_filedesc=0; 


/* handler SIGTERM .... closefb */
/* handler of signals => closefb needed */
void signal_handle(int __attribute__ ((unused)) unused)
{
    if(v_fb_filedesc!=0) 
      {  VidServ_CloseFb(v_fb_filedesc, NULL,0);   v_fb_filedesc = 0;}

    exit(0);	
}


/*
 * VidServ_ExecuteDisplayLoopFromExec - Execute a display in loop - create a child for infinite loop (only interrupt by process kill
 * 
 * @LoopDisplayId: display ID where to apply the loop
 * @LoopMode: mode of display loop
 * @ImageDuration: duration of the image 1
 * @Image2Duration: duration of the image 2 (for TOGGLE loop mode)
 * @return: VIDSERV_NO_ERROR if ok.
 *
 * */
int32_t VidServ_ExecuteDisplayLoopFromExec(uint32_t LoopDisplayId, e_VidServ_LoopMode_t LoopMode, uint32_t ImageDuration, uint32_t Image2Duration)
{
    char al_TmpFile[30];
    FILE *pl_tmp_fd=NULL;
    pid_t vl_ChildPid=0;    
    uint32_t vl_error=VIDSERV_NO_ERROR;
    struct fb_fix_screeninfo sl_fb_fixinfo;
    struct fb_var_screeninfo sl_fb_varinfo;     
    uint32_t vl_LoopCounter;
    uint64_t vl_beginTimeMs;
    int32_t vl_sleepInMs;
    struct timeval sl_TimeValue ={0, 0};
    struct sigaction act = { .sa_handler = signal_handle };

    sigaction(SIGTERM, &act, NULL);    

    switch(fork()){
        case (pid_t)-1:
            printf("Error during child creation for loop mode\n");
            vl_error=VIDSERV_FAILED;
            goto error;    
        break;
        
        case (pid_t)0:    
            /* We are in the Child Thread */
        
            /* Save the PID of the child in a file to be able to kill it when Stop Loop is executed */            
            vl_ChildPid=getpid();
            snprintf(al_TmpFile,22,"/tmp/tmp.displayLoop%d",LoopDisplayId);                
	    
            if((pl_tmp_fd=fopen(al_TmpFile,"w"))!=NULL)
            {
                fwrite(&vl_ChildPid, 1, sizeof(pid_t),pl_tmp_fd);
                fclose(pl_tmp_fd);
            }
            else {vl_error=VIDSERV_FAILED; goto error;}

            /* open an instance of framebuffer and mmap the framebuffer memory */
            if(VidServ_OpenFb(DisplayToDev[LoopDisplayId], &v_fb_filedesc, &sl_fb_fixinfo, &sl_fb_varinfo, NULL)!=0) { vl_error=VIDSERV_FAILED; printf("VidServ_ExecuteDisplayLoop: VidServ_OpenFb KO\n"); goto error;}

	    
	    /* Get time of first display */
	    if(gettimeofday(&sl_TimeValue, NULL)!=0) { vl_error=VIDSERV_FAILED; printf("VidServ_ExecuteDisplayLoop:GetTimeOfday KO\n"); goto out_free;}
	    vl_beginTimeMs=(sl_TimeValue.tv_sec*1000 + sl_TimeValue.tv_usec/1000);
	    vl_LoopCounter=0;

            /* If FIXED_RATE mode then set Image2Duration=ImageDuration to use same code for loop modes */
            if(LoopMode==FIXED_RATE) { Image2Duration=ImageDuration; }
            /* If MAX_SPEED mode then set Image2Duration=ImageDuration=0 to use same code for loop modes */	    
            if(LoopMode==MAX_SPEED) { Image2Duration=0; ImageDuration=0; }
	    	    	    	    
            /* intentional infinite loop, will be stopped by kill execution when VidServ_StopDisplayPictureLoop is called */            
            while(1)
            {            
                  /* Wait Image 2 duration before display Image 1 */
		  if(Image2Duration!=0)
		  { 
		    if(gettimeofday(&sl_TimeValue, NULL)!=0) { vl_error=VIDSERV_FAILED; printf("VidServ_ExecuteDisplayLoop:GetTimeOfday KO\n"); goto out_free;}
		    vl_sleepInMs=(uint64_t)(vl_beginTimeMs+(ImageDuration+Image2Duration)*vl_LoopCounter) - ((uint64_t)(sl_TimeValue.tv_sec*1000 + sl_TimeValue.tv_usec/1000));
		  
		    if(vl_sleepInMs>0) 
		      usleep(vl_sleepInMs*1000);
		  }
  
                  /* Display Image 1 */
		  sl_fb_varinfo.xoffset=0;
	          sl_fb_varinfo.yoffset=0;
                    if (ioctl(v_fb_filedesc, FBIOPAN_DISPLAY, &sl_fb_varinfo) < 0) {
                      printf("VidServ_DisplayPicture: IOCTL  FBIOPAN_DISPLAY KO\n");vl_error=VIDSERV_FAILED;goto out_free;    
                    }

		  /* Wait Image 1 duration before display Image 2 */
		  if(ImageDuration!=0)
		  { 
		    if(gettimeofday(&sl_TimeValue, NULL)!=0) { vl_error=VIDSERV_FAILED; printf("VidServ_ExecuteDisplayLoop:GetTimeOfday KO\n"); goto out_free;}
		    vl_sleepInMs=(uint64_t)(vl_beginTimeMs+(ImageDuration+Image2Duration)*vl_LoopCounter+ImageDuration) - ((uint64_t)(sl_TimeValue.tv_sec*1000 + sl_TimeValue.tv_usec/1000));
		  
		    if(vl_sleepInMs>0) 
		      usleep(vl_sleepInMs*1000);
		  }

		  /* Display Image 2 */  	  
	          sl_fb_varinfo.xoffset=0;
	          sl_fb_varinfo.yoffset=sl_fb_varinfo.yres;
                    if (ioctl(v_fb_filedesc, FBIOPAN_DISPLAY, &sl_fb_varinfo) < 0) {
                      printf("VidServ_DisplayPicture: IOCTL  FBIOPAN_DISPLAY KO\n");vl_error=VIDSERV_FAILED; goto out_free;   
                    }
		vl_LoopCounter++;
            }
        break;
        
        default:
            /* We are in the Parent Thread */                
            return VIDSERV_NO_ERROR;
        break;        
    }    	    

out_free:
  VidServ_CloseFb(v_fb_filedesc, NULL,0);    v_fb_filedesc = 0;

error:
    return vl_error;        
}

/*
 * Main - Execute a display in loop 
 * parameters:
 * param0: Executable name 
 * param1: display ID where to apply the loop
 * param2: mode of display loop
 * param3: duration of the image 1
 * param4: duration of the image 2 (for TOGGLE loop mode)
 *
 * */
int32_t main (int32_t __attribute__ ((unused)) argc, char *argv[])
{
   VidServ_ExecuteDisplayLoopFromExec(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4])); 
   return 0;
}
