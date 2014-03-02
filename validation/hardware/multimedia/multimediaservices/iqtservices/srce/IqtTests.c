/*****************************************************************************/
/**
*  Copyright (C) ST-Ericsson 2009
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Propose Video function for tests (Display (Main, Sub, TV-OUT), Camera)
* \author  ST-Ericsson
*/
/* Not compiled*/
/*****************************************************************************/
 
#include <stdint.h>			/* uint32_t ... */
#include <stdio.h> /* printf ... */
#include <string.h> /* strlen ... */
#include <stdlib.h> /* exit ... */

#include "IqtServices.h"

int32_t main (int32_t argc, char *argv[])
{

char command[100];
int fd_input;
int fd_output;
int fd_popen;
int pts;

FILE *TestMmte;
char MmtePid[20];

  if(argc != 6)
  {
    printf("Usage IqtServices start input_pipe output_pipe output_MMTE_pipe </dev/pts/DEVICE> or IqtServices stop input_pipe output_pipe output_MMTE_pipe </dev/pts/DEVICE>\n");
    exit(0);
  }
 
  pts=atoi(argv[5]);

  if(strncmp(argv[1],"start",strlen("start"))==0)
  {
    IqtServ_InitPipe(argv[2],argv[3],argv[4],&fd_input,&fd_output,&fd_popen,pts);
    IqtServ_StartMmte(argv[2],argv[3],fd_popen,"Trigger detected",20000);
  } 
  else if(strncmp(argv[1],"stop",strlen("stop"))==0) 
  {
    system("pgrep mmte_bellagio> /tmp/test_mmte");
    TestMmte=fopen("/tmp/test_mmte","r");
    if(fgets(MmtePid,20,TestMmte)!=NULL)
    {
      sprintf(command,"echo \"quit\" >%s",argv[2]);
      printf("%s\n",command);
      system(command);
      system("rm /tmp/test_mmte");
    }
    else
    {
    system("rm /tmp/test_mmte");
    printf("mmte_bellagio is not running\n");
    exit(0);
    }
  }
  else
  {
    printf("Usage IqtServices start input_pipe output_pipe output_MMTE_pipe or IqtServices stop input_pipe output_pipe output_MMTE_pipe\n");
    exit(0);
  }
  exit(EXIT_SUCCESS);
}

