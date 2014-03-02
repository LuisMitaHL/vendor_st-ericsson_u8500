/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file    ste_omxil_test.cpp
* \brief   Test application for OMXIL interface
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "ste_omxil_interface.h"
#include <OMX_Core.h>
#include <OMX_Component.h>
#include <OMX_Types.h>
#include <OMX_Audio.h>

char cName[]="OMX.ST.AFM.decoder.mp3";

OMX_ERRORTYPE audioDecMp3EventHandler(OMX_OUT OMX_HANDLETYPE hComponent,
                                        OMX_OUT OMX_PTR pAppData, OMX_OUT OMX_EVENTTYPE eEvent,
                                        OMX_OUT OMX_U32 Data1, OMX_OUT OMX_U32 Data2,
                                        OMX_OUT OMX_PTR pEventData)
{
   return OMX_ErrorNone;
}

OMX_ERRORTYPE audioDecMp3EmptyBufferDone(
  OMX_OUT OMX_HANDLETYPE hComponent,
  OMX_OUT OMX_PTR pAppData,
  OMX_OUT OMX_BUFFERHEADERTYPE* pBuffer)
{
   return OMX_ErrorNone;
}

OMX_ERRORTYPE audioDecMp3FillBufferDone(
  OMX_OUT OMX_HANDLETYPE hComponent,
  OMX_OUT OMX_PTR pAppData,
  OMX_OUT OMX_BUFFERHEADERTYPE* pBuffer)
{
   return OMX_ErrorNone;
}

OMX_CALLBACKTYPE audioDecMp3Callbacks;

int main(int argc, char* argv[]) {

	OMXCoreInterface* pInterface = NULL;
	OMX_HANDLETYPE hComponentHandle = NULL;
	OMX_ERRORTYPE err;

	audioDecMp3Callbacks.EventHandler    = audioDecMp3EventHandler;
  	audioDecMp3Callbacks.EmptyBufferDone = audioDecMp3EmptyBufferDone;
  	audioDecMp3Callbacks.FillBufferDone  = audioDecMp3FillBufferDone;
		
	pInterface =  GetOMXCoreInterface();

	printf("value for pInterface is %x\n",(unsigned int)pInterface);

	printf("calling OMX_Init()\n");

	err = (pInterface->GetpOMX_Init())();
	if (err != OMX_ErrorNone) {
	    printf("OMX_Init failed\n");
	}
	
	err = (pInterface->GetpOMX_GetHandle())(&hComponentHandle, cName, NULL , &audioDecMp3Callbacks);
	if (err != OMX_ErrorNone) {
	    printf("OMX_GetHandle failed for %s\n", cName);
	    err = (pInterface->GetpOMX_Deinit())();
	    if (err != OMX_ErrorNone) {
		printf("OMX_Deinit failed\n");
	    }
	    return 1;
	}
	printf("value for %s Handle is %p and err is 0x%x\n", cName, (void *) hComponentHandle, err);
	
	// Sample State Change OpenMAX API call 
	//err = OMX_SendCommand(hComponentHandle, OMX_CommandStateSet, OMX_StateIdle, NULL);
	//err = OMX_SendCommand(hComponentHandle, OMX_CommandStateSet, OMX_StateExecuting, NULL);
	
	err = (pInterface->GetpOMX_FreeHandle())(hComponentHandle);
	if (err != OMX_ErrorNone) {
	    printf("OMX_FreeHandle failed for %s\n", cName);
	    err = (pInterface->GetpOMX_Deinit())();
	    if (err != OMX_ErrorNone) {
		printf("OMX_Deinit failed\n");
	    }
	}
	err = (pInterface->GetpOMX_Deinit())();
	if (err != OMX_ErrorNone) {
	    printf("OMX_Deinit failed\n");
	    return 1;
	}	
	
	return 0;
}

 
