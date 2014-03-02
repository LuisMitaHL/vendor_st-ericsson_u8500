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
#include <stdio.h>			/* printf ... */
#include <linux/fb.h>		/* fb struct & var */
#include <fcntl.h>			/* O_RDWR */
#include <sys/mman.h>		/* mmap() ...*/
#include <sys/stat.h>       /* mknod .. */
#include <unistd.h>			/* fork ... */
#include <string.h>			/* memcpy ..*/
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdlib.h>		/* malloc(), free() */	

#include <signal.h> /*kill ... */

#include <sys/ioctl.h>

#include "IqtServices.h"
#include <errno.h>
 
//#define debug_printf(...) printf(__VA_ARGS__)
#define debug_printf(...)

#define INIT "do /usr/share/mm-valid/imaging/init_iqt.ite\n"
#define SCRIPTS_PATH "do /usr/share/mm-valid/imaging/ite_omx_use_cases_for_IQT/imaging_scripts/intern_usecases/"
#define INIT_TO_IDLE  	        SCRIPTS_PATH"start_init_to_idle.ite\n"
#define IDLE_TO_INIT  		SCRIPTS_PATH"start_idle_to_init.ite\n"
#define PREVIEW 		SCRIPTS_PATH"start_preview.ite\n"
#define EXE_TO_IDLE  		SCRIPTS_PATH"start_executing_to_idle.ite\n"
#define PAUSE              	SCRIPTS_PATH"start_pause.ite\n"
#define RESUME 			SCRIPTS_PATH"start_running.ite\n"
#define STILL_PICTURE		SCRIPTS_PATH"still_picture_capture.ite\n"
#define START_VIDEO		SCRIPTS_PATH"start_record.ite\n"
#define STOP_VIDEO  		SCRIPTS_PATH"stop_record.ite\n"
#define START_STILL_BURST	SCRIPTS_PATH"start_still_capture.ite\n"
#define STOP_STILL_BURST  	SCRIPTS_PATH"stop_still_capture.ite\n"
#define STOP_PREVIEW  	        SCRIPTS_PATH"stop_preview.ite\n"

#define READ_PAGE_ELEMENT  	SCRIPTS_PATH"read_page_element.ite"
#define WRITE_PAGE_ELEMENT 	SCRIPTS_PATH"write_page_element.ite"

uint32_t fd_output_iqt_pipe=0;
uint32_t fd_input_iqt_pipe=0;
uint32_t fd_iqt_output_MMTE_task_pipe=0;
//FILE *fd_popen_MmteFilter=NULL;

uint32_t IqtServ_InitPipe(char *input_iqt_pipe,char *output_iqt_pipe,char *iqt_output_MMTE_task_pipe,int *fd_ip,int *fd_op,int *fd_op_mmte_task,int pts)
{
int32_t vl_error=IQTSERV_NO_ERROR; 
int32_t fd_output;
int32_t fd_input;
int32_t fd_output_MMTE_task_pipe;
char command[100];
FILE *fd_popen;
 
  errno=0;
  if(mkfifo (input_iqt_pipe, 0666) ==0) /* creation tube avec droit lecture/ecriture*/
  {
    printf("ok");
  }
  else 
  {
    if(errno == EEXIST) printf("this file (%s) exists already \n",input_iqt_pipe);		
    else printf("Issue for the creation of %s",input_iqt_pipe);
  }
  fd_input=open(input_iqt_pipe,O_RDWR | O_NDELAY);
  if(fd_input<0)
  {
    printf("error for the opening: %s\n",input_iqt_pipe);
    return(IQTSERV_FAILED );	
  }
  else
  {
    fd_input_iqt_pipe=*fd_ip=fd_input;
  }
  errno=0;
  if(mkfifo (output_iqt_pipe, 0666) ==0) /* creation tube avec droit lecture/ecriture*/
  {
    printf("ok");
  }
  else 
  {
    if(errno == EEXIST) printf("this file (%s) exists already \n",output_iqt_pipe);		
    else printf("Issue for the creation of %s",output_iqt_pipe);
  }
  errno=0;
  fd_output=open(output_iqt_pipe,O_RDWR | O_NDELAY);
  if(fd_output<0)
  {
    printf("error for the opening: %s\n",output_iqt_pipe);
    return(IQTSERV_FAILED );
  }
  else
  {
    fd_output_iqt_pipe=*fd_op=fd_output;
  }
  if(mkfifo (iqt_output_MMTE_task_pipe, 0666) ==0) /* creation tube avec droit lecture/ecriture*/
  {
    printf("ok");
  }
  else 
  {
    if(errno == EEXIST) printf("this file (%s) exists already \n",iqt_output_MMTE_task_pipe);		
    else printf("Issue for the creation of %s",iqt_output_MMTE_task_pipe);
  }
  errno=0;
  fd_output_MMTE_task_pipe=open(iqt_output_MMTE_task_pipe,O_RDWR | O_NDELAY);
  if(fd_output_MMTE_task_pipe<0)
  {
    printf("error for the opening: %s",iqt_output_MMTE_task_pipe);
    return(IQTSERV_FAILED );	
  }
  else 
  {
    fd_iqt_output_MMTE_task_pipe=*fd_op_mmte_task=fd_output_MMTE_task_pipe;
  }
  printf("Initialization for the iqt pipe finished\n");
 
  //sprintf(command,"strace -f -c MmteOutputConsoleManagement %d",fd_output);
  sprintf(command,"MmteOutputConsoleManagement %d %d %s %d 1>%s 2>&1",fd_output,fd_output_MMTE_task_pipe,output_iqt_pipe,pts,iqt_output_MMTE_task_pipe);

  fd_popen = popen(command, "w");

  //fd_popen_MmteFilter = fd_popen;

  printf("MmteOutputConsoleManagement started\n");

  return vl_error; 
}

void IqtServ_WaitAcknowledge(int fd,char *trigger,int timeout)
{
char string[1024];
ssize_t size=0;
 
fd_set rfds;
struct timeval tv;
int retval;
int vl_timeout;	 

 usleep(30000); 

 FD_ZERO(&rfds);
 FD_SET(fd, &rfds);
 
 /* Wait up to timeout/1000 seconds. */
 tv.tv_sec = timeout/1000;
 tv.tv_usec = 0;
 memset(string,0,sizeof(string));
 retval = select((fd+1), &rfds, NULL, NULL, &tv);
 
 vl_timeout=((int)(tv.tv_sec) * 1000)+((int)(tv.tv_usec)/1000) ;
 printf("timeout = %d/%d\n",vl_timeout,timeout);

 if (retval == -1)
   perror("select()");
 else if (retval)
 {
   size=read(fd, string,34);
   if(strncmp(string,trigger,strlen(trigger)) == 0)
   {
     printf("mmte command/script executed: %s\n",trigger);
   }
   else if(strncmp(string,"Error MMTE script detected ",strlen("Error MMTE script detected ")) == 0)
   {
     printf("Error MMTE script detected\n");
     
     IqtServ_WaitAcknowledge(fd,trigger,vl_timeout);
   }
   else if(strncmp(string,"Error MMTE command detected",strlen("Error MMTE command detected")) == 0)
   {
     printf("Error MMTE command detected\n");
     IqtServ_WaitAcknowledge(fd,trigger,vl_timeout);
   }
   else if(strncmp(string,"Timeout event detected     ",strlen("Timeout event detected     ")) == 0)
   {
     printf("Timeout event detected\n");     
     IqtServ_WaitAcknowledge(fd,trigger,vl_timeout);
   }
   else 
   {
     printf("Unexpected Acknowledge: %s\n",string);
     IqtServ_WaitAcknowledge(fd,trigger,vl_timeout);
   }
 }
 else
 {
   printf("Mmte command timeout\n");
 }
 usleep(30000);
}

uint32_t IqtServ_SetMmteAlias(int fd_in,int fd_out,char *mmte_command,int timeout)
{
uint32_t vl_error=IQTSERV_NO_ERROR; 
char chaine[200];

 memset(chaine,0,sizeof(chaine));
 strcpy(chaine, mmte_command);
 printf("Write a buffer of %d bytes with text %s\n", strlen(chaine), chaine);
 write(fd_in, chaine, strlen(chaine));
 IqtServ_WaitAcknowledge(fd_out,"Command executed",timeout);
 return vl_error; 
}

uint32_t IqtServ_InitMmte(int fd_in,int fd_out,char *trigger,int timeout)
{
uint32_t vl_error=IQTSERV_NO_ERROR; 
char chaine[200];

 memset(chaine,0,sizeof(chaine));
  strcpy(chaine, INIT);
  printf("Write a buffer of %d bytes with text %s\n", strlen(chaine), chaine);
  write(fd_in, chaine, strlen(chaine));
  IqtServ_WaitAcknowledge(fd_out,trigger,timeout);
  return vl_error; 
}
 
uint32_t IqtServ_StartPreview(int fd_in,int fd_out,char *trigger,int timeout)
{
uint32_t vl_error=IQTSERV_NO_ERROR; 
char chaine[200];

 memset(chaine,0,sizeof(chaine));
  strcpy(chaine,PREVIEW );
  printf("Write a buffer of %d bytes with text %s\n", strlen(chaine), chaine);
  write(fd_in, chaine, strlen(chaine));
  IqtServ_WaitAcknowledge(fd_out,trigger,timeout);
  return vl_error; 
}

uint32_t IqtServ_InitToIdle(int fd_in,int fd_out,char *trigger,int timeout)
{
uint32_t vl_error=IQTSERV_NO_ERROR; 
char chaine[200];

 memset(chaine,0,sizeof(chaine));
  strcpy(chaine,INIT_TO_IDLE);
  printf("Write a buffer of %d bytes with text %s\n", strlen(chaine), chaine);
  write(fd_in, chaine, strlen(chaine));
  IqtServ_WaitAcknowledge(fd_out,trigger,timeout);
  return vl_error; 
}

uint32_t IqtServ_ExecutingToIdle(int fd_in,int fd_out,char *trigger,int timeout)
{
uint32_t vl_error=IQTSERV_NO_ERROR; 
char chaine[200];

 memset(chaine,0,sizeof(chaine));
  strcpy(chaine, EXE_TO_IDLE );
  printf("Write a buffer of %d bytes with text %s\n", strlen(chaine), chaine);
  write(fd_in, chaine, strlen(chaine));
  IqtServ_WaitAcknowledge(fd_out,trigger,timeout);
  return vl_error; 
}

uint32_t IqtServ_IdleToInit(int fd_in,int fd_out,char *trigger,int timeout)
{
uint32_t vl_error=IQTSERV_NO_ERROR; 
char chaine[200];

 memset(chaine,0,sizeof(chaine));
  strcpy(chaine,IDLE_TO_INIT);
  printf("Write a buffer of %d bytes with text %s\n", strlen(chaine), chaine);
  write(fd_in, chaine, strlen(chaine));
  IqtServ_WaitAcknowledge(fd_out,trigger,timeout);
  return vl_error; 
}
uint32_t IqtServ_StartPause(int fd_in,int fd_out,char *trigger,int timeout)
{
uint32_t vl_error=IQTSERV_NO_ERROR; 
char chaine[200];

 memset(chaine,0,sizeof(chaine));
  strcpy(chaine, PAUSE );
  printf("Write a buffer of %d bytes with text %s\n", strlen(chaine), chaine);
  write(fd_in, chaine, strlen(chaine));
  IqtServ_WaitAcknowledge(fd_out,trigger,timeout);
  return vl_error; 
}

uint32_t IqtServ_StartResume(int fd_in,int fd_out,char *trigger,int timeout)
{
uint32_t vl_error=IQTSERV_NO_ERROR; 
char chaine[200];

 memset(chaine,0,sizeof(chaine));
  strcpy(chaine, RESUME );
  printf("Write a buffer of %d bytes with text %s\n", strlen(chaine), chaine);
  write(fd_in, chaine, strlen(chaine));
  IqtServ_WaitAcknowledge(fd_out,trigger,timeout);
  return vl_error; 
}

uint32_t IqtServ_StartStillPictureCapture(int fd_in,int fd_out,char *trigger,int timeout)
{
uint32_t vl_error=IQTSERV_NO_ERROR; 
char chaine[200];

 memset(chaine,0,sizeof(chaine));
  strcpy(chaine, STILL_PICTURE);
  printf("Write a buffer of %d bytes with text %s\n", strlen(chaine), chaine);
  write(fd_in, chaine, strlen(chaine));
  IqtServ_WaitAcknowledge(fd_out,trigger,timeout);
  return vl_error; 
}

uint32_t IqtServ_StartRecord(int fd_in,int fd_out,char *trigger,int timeout)
{
uint32_t vl_error=IQTSERV_NO_ERROR; 
char chaine[200];

 memset(chaine,0,sizeof(chaine));
  strcpy(chaine, START_VIDEO);
  printf("Write a buffer of %d bytes with text %s\n", strlen(chaine), chaine);
  write(fd_in, chaine, strlen(chaine));
  IqtServ_WaitAcknowledge(fd_out,trigger,timeout);
  return vl_error; 
}

uint32_t IqtServ_StopRecord(int fd_in,int fd_out,char *trigger,int timeout)
{
uint32_t vl_error=IQTSERV_NO_ERROR; 
char chaine[200];

 memset(chaine,0,sizeof(chaine));
  strcpy(chaine, STOP_VIDEO);
  printf("Write a buffer of %d bytes with text %s\n", strlen(chaine), chaine);
  write(fd_in, chaine, strlen(chaine));
  IqtServ_WaitAcknowledge(fd_out,trigger,timeout);
  return vl_error; 
}

uint32_t IqtServ_StartStillBurstPictureCapture(int fd_in,int fd_out,char *trigger,int timeout)
{
uint32_t vl_error=IQTSERV_NO_ERROR; 
char chaine[200];

 memset(chaine,0,sizeof(chaine));
  strcpy(chaine, START_STILL_BURST);
  printf("Write a buffer of %d bytes with text %s\n", strlen(chaine), chaine);
  write(fd_in, chaine, strlen(chaine));
  IqtServ_WaitAcknowledge(fd_out,trigger,timeout);
  return vl_error; 
}

uint32_t IqtServ_StopStillBurstPictureCapture(int fd_in,int fd_out,char *trigger,int timeout)
{
uint32_t vl_error=IQTSERV_NO_ERROR; 
char chaine[200];

 memset(chaine,0,sizeof(chaine));
  strcpy(chaine, STOP_STILL_BURST);
  printf("Write a buffer of %d bytes with text %s\n", strlen(chaine), chaine);
  write(fd_in, chaine, strlen(chaine));
  IqtServ_WaitAcknowledge(fd_out,trigger,timeout);
  return vl_error; 
}

uint32_t IqtServ_StopPreview(int fd_in,int fd_out,char *trigger,int timeout)
{
uint32_t vl_error=IQTSERV_NO_ERROR; 
char chaine[200];

 memset(chaine,0,sizeof(chaine));
  strcpy(chaine,STOP_PREVIEW );
  printf("Write a buffer of %d bytes with text %s\n", strlen(chaine), chaine);
  write(fd_in, chaine, strlen(chaine));
  IqtServ_WaitAcknowledge(fd_out,trigger,timeout);
  return vl_error; 
}

uint32_t IqtServ_QuitMmte(int fd_in,int fd_out,char *trigger,int timeout)
{
uint32_t vl_error=IQTSERV_NO_ERROR; 
char chaine[200];

 memset(chaine,0,sizeof(chaine));
  strcpy(chaine, "quit\n");
  printf("Write a buffer of %d bytes with text %s\n", strlen(chaine), chaine);
  write(fd_in, chaine, strlen(chaine));
  IqtServ_WaitAcknowledge(fd_out,trigger,timeout);
  return vl_error; 
}

uint32_t IqtServ_StartMmte(char *pipe_input,char *pipe_output,int fd_out,char *trigger,int timeout)
{
uint32_t vl_error=IQTSERV_NO_ERROR; 
char command[200];

  printf("Starting MMTE\n");

  system("rm -f /tmp/mmte_in_background.sh");
  system("echo \"export OMX_BELLAGIO_REGISTRY=/system/omxcomponents\" > /tmp/mmte_in_background.sh"); 
  sprintf(command,"echo \"mmte_bellagio 0<%s 1>%s 2>&1\" >> /tmp/mmte_in_background.sh",pipe_input,pipe_output);
  system(command);	
  system("chmod 777 /tmp/mmte_in_background.sh &");
  system("/tmp/mmte_in_background.sh &");

  IqtServ_WaitAcknowledge(fd_out,trigger,timeout);
  //pclose(fd_popen_MmteFilter);
  return vl_error;  
}

uint32_t IqtServ_CloseFileDescriptors(void)
{
uint32_t vl_error=IQTSERV_NO_ERROR; 

  close(fd_output_iqt_pipe);
  close(fd_input_iqt_pipe);
  close(fd_iqt_output_MMTE_task_pipe);
  return vl_error; 
}

uint32_t IqtServ_ReadPageElement(int fd,int fd_output,int index,int* value,int timeout)
{
uint32_t vl_error=IQTSERV_NO_ERROR; 
char chaine[200];

 memset(chaine,0,sizeof(chaine));
 sprintf(chaine,"%s 0x%04x\n",READ_PAGE_ELEMENT,index);
 printf("Write a buffer of %d bytes with text %s\n", strlen(chaine), chaine);
 write(fd, chaine, strlen(chaine));

 IqtServ_AckReadPageElement(fd_output,timeout,index,value);

 return(vl_error);
}

void IqtServ_AckReadPageElement(int fd,int timeout,int index,int* value)
{
char string[1024];
ssize_t size=0;


char ref_string1[28];
char ref_string2[18];
 
fd_set rfds;
struct timeval tv;
int retval;
int vl_timeout;

 memset(ref_string1,0,sizeof(ref_string1));
 memset(ref_string2,0,sizeof(ref_string2));	

 sprintf(ref_string1,"+nPEList_addr[0]:0x%08x",index);
 sprintf(ref_string2,"+nPEList_data[0]:"); 

 FD_ZERO(&rfds);
 FD_SET(fd, &rfds);
 
 /* Wait up to timeout/1000 seconds. */
 tv.tv_sec = timeout/1000;
 tv.tv_usec = 0;
 memset(string,0,sizeof(string));
 retval = select((fd+1), &rfds, NULL, NULL, &tv);
 
 vl_timeout=((int)(tv.tv_sec) * 1000)+((int)(tv.tv_usec)/1000) ;
 printf("timeout = %d/%d\n",vl_timeout,timeout);

 if (retval == -1)
   perror("select()");
 else if (retval)
 {
   size=read(fd, string,34);

   if(strncmp(string,ref_string1,strlen(ref_string1)) == 0)
   {
     printf("Checking for the address = %s\n",ref_string1);
     IqtServ_AckReadPageElement(fd,vl_timeout,index,value);
   }
   else if(strncmp(string,ref_string2,strlen(ref_string2)) == 0)
   {
     debug_printf("__debug_Ref_string = %s\n",ref_string2);
     debug_printf("__debug_PageElement_index = %s\n",string);
     debug_printf("__PageElement_index = %s\n",&string[17]);
     sscanf(&string[17],"%x",value);
     printf("PageElement_index0x%04x = 0x%x\n",index,*value);

   }
   else if(strncmp(string,"Error MMTE script detected ",strlen("Error MMTE script detected ")) == 0)
   {
     printf("Error MMTE script detected\n");
     
     IqtServ_AckReadPageElement(fd,vl_timeout,index,value);
   }
   else if(strncmp(string,"Error MMTE command detected",strlen("Error MMTE command detected")) == 0)
   {
     printf("Error MMTE command detected\n");
     IqtServ_AckReadPageElement(fd,vl_timeout,index,value);
   }
   else if(strncmp(string,"Timeout event detected     ",strlen("Timeout event detected     ")) == 0)
   {
     printf("Timeout event detected\n");     
     IqtServ_AckReadPageElement(fd,vl_timeout,index,value);
   }
   else 
   {
     printf("Unexpected Acknowledge: %s\n",string);
     IqtServ_AckReadPageElement(fd,vl_timeout,index,value);
   }
 }
 else
 {
   printf("Mmte command timeout for PE\n");
 }
}

uint32_t IqtServ_WritePageElement(int fd_in,int fd_out,int index,int value,char *trigger,int timeout)
{
uint32_t vl_error=IQTSERV_NO_ERROR; 
char chaine[200];

  memset(chaine,0,sizeof(chaine));
  sprintf(chaine,"%s 0x%04x 0x%08x\n",WRITE_PAGE_ELEMENT,index,value);	
  printf("Write a buffer of %d bytes with text %s\n", strlen(chaine), chaine);
  write(fd_in, chaine, strlen(chaine));
  IqtServ_WaitAcknowledge(fd_out,trigger,timeout);
  return vl_error; 
}

uint32_t IqtServ_ReadCurrentState(int fd_in,int fd_out,int timeout,int* State)
{
uint32_t vl_error=IQTSERV_NO_ERROR;
char chaine[200];

  memset(chaine,0,sizeof(chaine));
  sprintf(chaine,"echo Current_State:$(IQT_CURRENT_STATE)\n");
  printf("Write a buffer of %d bytes with text %s\n", strlen(chaine), chaine);
  write(fd_in, chaine, strlen(chaine));
  IqtServ_AckReadCurrentState(fd_out,timeout,State);
  return vl_error;
}

void IqtServ_AckReadCurrentState(int fd,int timeout,int* State)
{
char string[1024];
ssize_t size=0;
int vl_timeout;
fd_set rfds;
struct timeval tv;
int retval;

  FD_ZERO(&rfds);
  FD_SET(fd, &rfds);
 
  /* Wait up to timeout/1000 seconds. */
  tv.tv_sec = timeout/1000;
  tv.tv_usec = 0;
  memset(string,0,sizeof(string));
  retval = select((fd+1), &rfds, NULL, NULL, &tv);
 
  vl_timeout=((int)(tv.tv_sec) * 1000)+((int)(tv.tv_usec)/1000) ;
  printf("timeout = %d/%d\n",vl_timeout,timeout);

  if (retval == -1)
    perror("select()");
  else if (retval)
  {
    size=read(fd, string,34);
    if(strncmp(string,"Current_State:",strlen("Current_State:")) == 0)
    {
      printf("__debug_Current_State= %s\n",string);
      printf("__debug_Current_State= %s\n",&string[14]);
      printf(string);
      sscanf(&string[14],"%d",State);
      printf("State = %d\n",*State);
    } 
    else if(strncmp(string,"Error MMTE script detected ",strlen("Error MMTE script detected ")) == 0)
    {
      printf("Error MMTE script detected\n");
      IqtServ_AckReadCurrentState(fd,vl_timeout,State);
    }
    else if(strncmp(string,"Error MMTE command detected",strlen("Error MMTE command detected")) == 0)
    {
      printf("Error MMTE command detected\n");
      IqtServ_AckReadCurrentState(fd,vl_timeout,State);
    }
    else if(strncmp(string,"Timeout event detected     ",strlen("Timeout event detected     ")) == 0)
    {
      printf("Timeout event detected\n");
      IqtServ_AckReadCurrentState(fd,vl_timeout,State);
    }
    else if(strncmp(string,"Trigger detected",strlen("Trigger detected")) == 0)
    {
      printf("Unexpected Acknowledge %s\n",string);
      IqtServ_AckReadCurrentState(fd,vl_timeout,State);
    }
    else if(strncmp(string,"Command executed",strlen("Command executed")) == 0)
    {
      printf("Unexpected Acknowledge %s\n",string);
      IqtServ_AckReadCurrentState(fd,vl_timeout,State);
    }
    else
    {
      printf("Mmte command timeout %s\n",string);
    }	
  } 
}



