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
#define PIPE_VERSION

#include <stdint.h>			/* uint32_t ... */
#include <stdio.h> /* printf ... */
#include <string.h> /* strlen ... */
#include <stdlib.h> /* exit ... */
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
 
#define NB_BLOCK 8

static int write_checked ( int fd, void const * buffer, size_t length )
{
      char const * ptr = buffer;
      size_t       len = length;
      ssize_t            wsize;

      errno=0;
      while (( len != 0 ) && (wsize = write ( fd, ptr, len )) != 0)
      {
            if ( wsize == -1 )
            {
                  if (errno != EINTR)
   		  {
                  	perror ( "write");
			printf("error=%d",errno);
			fflush(stdout);
			break;
		  }
		  else
		  {
			perror("ernno = EINTR");
		  }     
            }
	    else
	    {		
              ptr += wsize;
              len -= wsize;
	    }
      }
      return len == 0;
}

int main(int argc, char **argv)
{
  FILE *stream;
  FILE *log, *console;
  char *chaine_read=NULL;
  char *chaine_read_copy=NULL;

  int result=-1;
  int result2=-1;
  int result3=-1;
  int result4=-1;
  int result5=-1;
  int result6=-1;
  int result7=-1;
  int result8=-1;
  int result9=-1;
  int result10=-1;
  int result11=-1;
  int v_script_error=0;
  int v_trigger_detected=0;
  int pipe_output=0;

  int fd_output_MMTE_task_pipe;
 
  int MmteAlive=1;

  fd_set rfds;
  struct timeval tv;
  int retval;
  char *FileName;
  char *ConsoleName;
  char *command;
  int i;
  int device;

	pipe_output=atoi(argv[1]);
	fd_output_MMTE_task_pipe=atoi(argv[2]);

	chaine_read=(char*)malloc(1024*NB_BLOCK);
	chaine_read_copy=(char*)malloc(34);

	stream = fdopen (pipe_output, "r");

	for(i=0;i<strlen(argv[3]);i++)
	{
	  if(argv[3][i]=='/')
	     argv[3][i]='_';
	}
 
	device=atoi(argv[4]);

	FileName=(char*)malloc(strlen(argv[3])+strlen("/tmp/trace_mmte.txt")+1);
	memset(FileName,0,strlen(argv[3])+strlen("/tmp/trace_mmte.txt")+1);
 
	sprintf(FileName,"/tmp/trace_mmte%s.txt",argv[3]);
	log=fopen(FileName,"w");

	ConsoleName=(char*)malloc(strlen(argv[4])+strlen("/dev/pts/")+1);
	memset(ConsoleName,0,strlen(argv[4])+strlen("/dev/pts/")+1);

	sprintf(ConsoleName,"/dev/pts/%d",device);
	console = fopen(ConsoleName,"w");

	system("rm -f /tmp/trace_mmte.txt");

	command=(char*)malloc(strlen("ln -s  ")+strlen(FileName)+strlen("/tmp/trace_mmte.txt")+1);	
	memset(command,0,strlen("ln -s  ")+strlen(FileName)+strlen("/tmp/trace_mmte.txt")+1);
	sprintf(command,"ln -s %s /tmp/trace_mmte.txt",FileName);
	system(command);

	while(MmteAlive)
	{	  
          /* Watch stdin (fd 0) to see when it has input. */
          FD_ZERO(&rfds);
          FD_SET(pipe_output, &rfds);

          /* Wait up to 30 seconds. */
          tv.tv_sec = 30;
          tv.tv_usec = 0;
	  memset(chaine_read,0,1024*NB_BLOCK);
          retval = select((pipe_output+1), &rfds, NULL, NULL, &tv);

          if (retval == -1)
            perror("select()");
          else if (retval)
	  {
	    while(fgets(chaine_read,1024*NB_BLOCK,stream)!=NULL)
	    {

	      fprintf(log,chaine_read);
	      fflush(log);
		  if( console != NULL) {
			fprintf(console,chaine_read);
			fflush(console);
		  }

	      result=strncmp(chaine_read,"File_saved",10);
	      result2=strncmp(chaine_read,"Entering vte_Init",17);
	      result3=strncmp(chaine_read,"Could NOT open command file",27);
	      result5=strncmp(chaine_read,"Could not load the script!",26);
	      result4=strncmp(chaine_read,"Unknown command",15);
	      result6=strncmp(chaine_read,"Command executed",16);
	      result7=strncmp(chaine_read,"bye!",4);
	      result8=strncmp(chaine_read,"EventExpected timeout",21);
	      //result9=strncmp(chaine_read,"PageElement_index",17);
	      result9=strncmp(chaine_read,"+nPEList_addr[0]:",17);
	      result11=strncmp(chaine_read,"+nPEList_data[0]:",17);
	      result10=strncmp(chaine_read,"Current_State:",14);
 
	      if(result10==0)
	      {
		memset(chaine_read_copy,0,34);
	 	strncpy(chaine_read_copy,chaine_read,24);
	        write_checked(fd_output_MMTE_task_pipe,chaine_read_copy,34);	
	      }

	      if((result11==0) || (result9==0))
	      {
		memset(chaine_read_copy,0,34);
	 	strncpy(chaine_read_copy,chaine_read,27);
	        write_checked(fd_output_MMTE_task_pipe,chaine_read_copy,34);	
	      }

	      if(strncmp(chaine_read,"do ",3)==0)
	      {
	        v_trigger_detected=0;
	      }

	      if((result==0)||(result2==0)||(result6==0))
	      {
	        if(v_script_error==0)
	        {
		  if((result==0)||(result2==0))
		  {
		    write_checked(fd_output_MMTE_task_pipe,"Trigger detected                  ",34);
		  }
		  else
		  {
		    write_checked(fd_output_MMTE_task_pipe,"Command executed                  ",34);
		  }			
		  v_trigger_detected=1;
	        }
	        else
	          v_script_error=0;
	      }
	      else if((result3==0)||(result5==0))
	      {
		write_checked(fd_output_MMTE_task_pipe,"Error MMTE script detected        ",34);
	        //v_script_error=1;
	      }
	      if(result4==0)
	      {
	        write_checked(fd_output_MMTE_task_pipe,"Error MMTE command detected       ",34);
	      }
	      if(result8==0)
	      {
		write_checked(fd_output_MMTE_task_pipe,"Timeout event detected            ",34);
	      }
	      if(result7==0)
	      {
	        MmteAlive=0;
	      }			
	}
      }
      else
      {
	//printf("Warnig:No Data from mmte within 60s");
	//fflush(stdout);
      }
     }
     write_checked(fd_output_MMTE_task_pipe,"Command executed                  ",34);
     fclose(log);
	 fclose(console);
     free(chaine_read);
     free(FileName);
     free(command);
     if(chaine_read_copy!=NULL)
     	free(chaine_read_copy);
     exit(0);
}

