/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     speechprocFactoryMethod.cpp
 * \brief    factory method for Speech Proc proxy
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#include "SpeechProcComponent.h"
#include "SpeechProcNmf_ProcessingComp.h"
#include "speech_proc_config.h"

OMX_ERRORTYPE speechprocFactoryMethod(ENS_Component_p * ppENSComponent) {
  OMX_ERRORTYPE error  = OMX_ErrorNone;

  SpeechProcComponent *speechproc = new SpeechProcComponent;
  if (speechproc == 0) {
    return OMX_ErrorInsufficientResources;
  }

  SpeechProcNmf_ProcessingComp *speechprocProcessingCpt =
    new SpeechProcNmf_ProcessingComp ((ENS_Component&)*speechproc);
  if (speechprocProcessingCpt == 0) {
	delete speechproc;
    return OMX_ErrorInsufficientResources;
  }

  error = speechproc->construct();

  if(error != OMX_ErrorNone){
    delete speechproc;
    delete speechprocProcessingCpt;
  }
  else{
    *ppENSComponent = speechproc;
    speechproc->setProcessingComponent(speechprocProcessingCpt);
    speechprocProcessingCpt->setLibrary(speechproc->getLibrary());
  }

  return error;
}
