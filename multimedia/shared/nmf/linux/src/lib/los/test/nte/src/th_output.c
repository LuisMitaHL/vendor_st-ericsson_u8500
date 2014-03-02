/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#ifndef TH_NO_FILEOUT



#define TH_DEBUG	0
/*********************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cm/inc/cm.h>

#include <los/api/los_api.h>
#ifndef LINUX
#include <ilos/api/irq.h>
#endif

/*********************************************************************/
/* File name */
#if TH_DEBUG==1
static char th_FileName[100];
#endif
/* File pointer */
static FILE* th_pFileHandle ;

/* Number of threads accessing the file
 * One access is considered as done, when file is closed by thread
 */
static t_uint8 th_fileAccessNumber = 0;

/* Flag to display debug traces on screen */
static t_uint8 th_debugDisplayFlag = 0;

static char delayedPrint[4096];

/*********************************************************************/
// initialization of th_debugDisplayFlag
void th_config(t_sint8 flag) {
	th_debugDisplayFlag = flag;
}

// open file if first thread
void th_fopen(const char*fileName) {
	if( (th_fileAccessNumber++)==0 ) {
	    if (th_pFileHandle == 0)
    	    th_pFileHandle = fopen(fileName, "w+");
    }
	if (th_pFileHandle == 0) {
		LOS_Log( "Error at %s:%d : could not open file: %s\n", __FILE__, __LINE__, fileName);
#ifdef LINUX
		exit(1);
#else
		while(1);
#endif
	}
#if TH_DEBUG==1
   	fclose(th_pFileHandle);
	strcpy(th_FileName,fileName);
#endif
    delayedPrint[0] = 0;
}

// write message to file
void th_fprintf(const char*pStringMessage) {
#ifndef LINUX
    if(ILOS_Interrupt_context() != 0) {
        sprintf(delayedPrint + strlen(delayedPrint), "%s\n", pStringMessage);
     } else
#endif /* LINUX */
    {
         if(delayedPrint[0] != 0) {
         #if TH_DEBUG==1
         	 th_pFileHandle = fopen(th_FileName, "a");
         #endif
                 fprintf(th_pFileHandle, "%s", delayedPrint);
         #if TH_DEBUG==1
         	 fclose(th_pFileHandle);
         #endif
             delayedPrint[0] = 0;
         }

     #if TH_DEBUG==1
       	 th_pFileHandle = fopen(th_FileName, "a");
     #endif
         if (th_pFileHandle == 0) {
             LOS_Log( "Error at %s:%d : log file was not opened\n", __FILE__, __LINE__);
#ifdef LINUX
		exit(1);
#else
		while(1);
#endif
         }
         fprintf( th_pFileHandle, "%s\n", pStringMessage );
         fflush( th_pFileHandle );
     #if TH_DEBUG==1
      	 fclose(th_pFileHandle);
     #endif

     }
    if(th_debugDisplayFlag) {
        if(strstr(pStringMessage, ":FAILED:"))
            LOS_Log( "Error:%s\n", (void*)pStringMessage);
        else
            LOS_Log( "%s\n", (void*)pStringMessage);
    }
}

// close file if last thread
void th_fclose(void) {
    if(delayedPrint[0] != 0) {
    #if TH_DEBUG==1
    	th_pFileHandle = fopen(th_FileName, "a");
    #endif
        fprintf(th_pFileHandle, "%s", delayedPrint);
    #if TH_DEBUG==1
    	fclose(th_pFileHandle);
    #endif
        delayedPrint[0] = 0;
    }

	if( (--th_fileAccessNumber)==0 ) {
    #if TH_DEBUG==0
		fclose(th_pFileHandle);
		th_pFileHandle = 0;
    #endif
	}

	//LOS_Exit(); //TODO, elsewhere
}



#endif /* ndef TH_NO_FILEOUT */
