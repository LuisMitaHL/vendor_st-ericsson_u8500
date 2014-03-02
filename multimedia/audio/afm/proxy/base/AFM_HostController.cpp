/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   AFM_HostController.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "AFM_HostController.h"
#include "ENS_Component.h"
#include "ENS_Port.h"
#include "NmfHost_ProcessingComponent.h"
#include "ENS_Nmf.h"

OMX_ERRORTYPE
AFM_HostController::instantiate(OMX_U32 priority) {
    t_nmf_error   nmf_error;

    // instantiate the controller
    mNmfCtrl = hst_composite_controllerCreate();
    if (!mNmfCtrl){
      NMF_LOG("Error - Instantiation Error\n") ;
      return OMX_ErrorInsufficientResources;
    }
    ((hst_composite_controller*)mNmfCtrl)->priority = priority;

    if (mNmfCtrl->construct() != NMF_OK) {
      NMF_LOG("Error -  Construct Error\n") ;
      return OMX_ErrorInsufficientResources;
    }

    nmf_error = mNmfCtrl->bindFromUser("sendcommand" ,NB_MAX_PORTS, mHostProcessingComp.getNmfSendCommandItf());
    if (nmf_error != NMF_OK) {
      NMF_LOG("Error: unable to bind controller sendcommand!...[%d]\n", nmf_error);
      return OMX_ErrorInsufficientResources;
    }

    nmf_error = mNmfCtrl->bindFromUser("fsminit" ,1, mHostProcessingComp.getNmfFsmInitItf());
    if (nmf_error != NMF_OK) {
      NMF_LOG("Error: unable to bind controller fsminit!...[%d]\n", nmf_error);
      return OMX_ErrorInsufficientResources;
    }

    nmf_error = mNmfCtrl->bindFromUser("configure" ,1, &mIconfigure);
    if (nmf_error != NMF_OK) {
      NMF_LOG("Error: unable to bind controller configuration!...[%d]\n", nmf_error);
      return OMX_ErrorInsufficientResources;
    }

    nmf_error = EnsWrapper_bindToUser(mENSComponent.getOMXHandle(), mNmfCtrl,"proxy",mHostProcessingComp.getEventHandlerCB(), NB_MAX_PORTS) ;
    if (nmf_error != NMF_OK) {
      NMF_LOG("Error: unable to bind controller event handler!...[%d]\n", nmf_error);
      return OMX_ErrorInsufficientResources;
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE
AFM_HostController::deInstantiate(void) {
    t_nmf_error   nmf_error;

    nmf_error = EnsWrapper_unbindToUser(mENSComponent.getOMXHandle(), mNmfCtrl, "proxy");
    if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to unbind controller proxy!...[%d]\n", nmf_error); return OMX_ErrorInsufficientResources; }

    nmf_error = mNmfCtrl->unbindFromUser("configure");
    if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to unbind controller configure!...[%d]\n", nmf_error); return OMX_ErrorInsufficientResources; }
    
    nmf_error = mNmfCtrl->unbindFromUser("fsminit");
    if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to unbind ctrl fsminit!...[%d]\n", nmf_error); return OMX_ErrorInsufficientResources; }

    nmf_error = mNmfCtrl->unbindFromUser("sendcommand");
    if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to unbind ctrl sendcommand!...[%d]\n", nmf_error); return OMX_ErrorInsufficientResources; }

    if (mNmfCtrl->destroy() != NMF_OK) return OMX_ErrorInsufficientResources;

    hst_composite_controllerDestroy((hst_composite_controller*&)mNmfCtrl);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE
AFM_HostController::start(void) {

    mNmfCtrl->start();

    return OMX_ErrorNone;
}

OMX_ERRORTYPE
AFM_HostController::stop(void) {

  mNmfCtrl->stop_flush();
  
  return OMX_ErrorNone;
}

OMX_ERRORTYPE
AFM_HostController::configure(void) {
    int portDirections = 0;
    int portTypes      = 0;

    for(int i = 0; i < (int) mENSComponent.getPortCount(); i++) {
        if (mENSComponent.getPort(i)->getDirection() == OMX_DirOutput) {
            portDirections |= 1 << i;
        }
        if (mENSComponent.getPort(i)->isMpc()) {
            portTypes |= 1 << i;
        }

    }

    mIconfigure.setParameter(portDirections, portTypes);

    return OMX_ErrorNone;
}

static const char* bindingItfNameHost[NB_MAX_PORTS]=
{
    "binding[0]",
    "binding[1]",
    "binding[2]",
    "binding[3]",
    "binding[4]",
    "binding[5]",
    "binding[6]",
    "binding[7]",
    "binding[8]",
    "binding[9]"
};

static const char* bindingInitItfNameHost[NB_MAX_PORTS]=
{
    "bindinginit[0]",
    "bindinginit[1]",
    "bindinginit[2]",
    "bindinginit[3]",
    "bindinginit[4]",
    "bindinginit[5]",
    "bindinginit[6]",
    "bindinginit[7]",
    "bindinginit[8]",
    "bindinginit[9]"
};

static const char* ctrlItfNameHost[NB_MAX_PORTS]=
{
    "eventhandlers[0]",
    "eventhandlers[1]",
    "eventhandlers[2]",
    "eventhandlers[3]",
    "eventhandlers[4]",
    "eventhandlers[5]",
    "eventhandlers[6]",
    "eventhandlers[7]",
    "eventhandlers[8]",
    "eventhandlers[9]"
};


OMX_ERRORTYPE
AFM_HostController::bindMain(NMF::Composite * mainComp) {
    t_nmf_error   nmf_error;

    nmf_error = mNmfCtrl->bindAsynchronous("maincomponent", 2, mainComp, "sendcommand");
    if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to bind controller slaves[0]!...[%d]\n", nmf_error); return OMX_ErrorInsufficientResources; }

    nmf_error = mNmfCtrl->bindAsynchronous("maincompinit", 2, mainComp, "fsminit");
    if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to bind controller inits[0]!...[%d]\n", nmf_error); return OMX_ErrorInsufficientResources; }

    nmf_error = mainComp->bindAsynchronous("proxy", 4, mNmfCtrl, "maineventhandler");
    if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to bind controller proxy!...[%d]\n", nmf_error); return OMX_ErrorInsufficientResources; }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE
AFM_HostController::unbindMain(NMF::Composite * mainComp) {
    t_nmf_error   nmf_error;

    nmf_error = mNmfCtrl->unbindAsynchronous("maincomponent", mainComp, "sendcommand");
    if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to unbind controller slaves[0]!...[%d]\n", nmf_error); return OMX_ErrorInsufficientResources; }

    nmf_error = mNmfCtrl->unbindAsynchronous("maincompinit", mainComp, "fsminit");
    if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to unbind controller inits[0]!...[%d]\n", nmf_error); return OMX_ErrorInsufficientResources; }

    nmf_error = mainComp->unbindAsynchronous("proxy", mNmfCtrl, "maineventhandler");
    if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to unbind controller proxy!...[%d]\n", nmf_error); return OMX_ErrorInsufficientResources; }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE
AFM_HostController::bindBinding(NMF::Composite* bindingComp, int portIdx) {
  t_nmf_error   nmf_error;

  DBC_ASSERT(portIdx < NB_MAX_PORTS);

  // the portIdx s checked by the assert above
  //coverity[overrun-local]
  nmf_error = mNmfCtrl->bindAsynchronous(bindingItfNameHost[portIdx], 2, bindingComp, "sendcommand");
  if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to bind controller %s!...[%d]\n", bindingItfNameHost[portIdx], nmf_error); return OMX_ErrorInsufficientResources; }
  
  nmf_error = mNmfCtrl->bindAsynchronous(bindingInitItfNameHost[portIdx], 2, bindingComp, "fsminit");
  if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to bind controller %s!...[%d]\n", bindingInitItfNameHost[portIdx], nmf_error); return OMX_ErrorInsufficientResources; }
  
  nmf_error = bindingComp->bindAsynchronous("proxy", 4, mNmfCtrl, ctrlItfNameHost[portIdx]);
  if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to bind controller proxy!...[%d]\n", nmf_error); return OMX_ErrorInsufficientResources; }

  return OMX_ErrorNone;
}


OMX_ERRORTYPE
AFM_HostController::unbindBinding(NMF::Composite * bindingComp, int portIdx) {
  t_nmf_error   nmf_error;

  nmf_error = mNmfCtrl->unbindAsynchronous(bindingItfNameHost[portIdx], bindingComp, "sendcommand");
  if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to unbind controller %s!...[%d]\n", bindingItfNameHost[portIdx], nmf_error); return OMX_ErrorInsufficientResources; }
  
  nmf_error = mNmfCtrl->unbindAsynchronous(bindingInitItfNameHost[portIdx], bindingComp, "fsminit");
  if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to unbind controller %s!...[%d]\n", bindingInitItfNameHost[portIdx], nmf_error); return OMX_ErrorInsufficientResources; }
  
  nmf_error = bindingComp->unbindAsynchronous("proxy", mNmfCtrl, ctrlItfNameHost[portIdx]);
  if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to unbind controller proxy!...[%d]\n", nmf_error); return OMX_ErrorInsufficientResources; }
  
  return OMX_ErrorNone;
}


#ifdef HOST_ONLY

OMX_ERRORTYPE
AFM_HostController::bindBinding(t_cm_instance_handle bindingComp, int portIdx) {
  DBC_ASSERT(0);
  return OMX_ErrorNotImplemented;
}

OMX_ERRORTYPE
AFM_HostController::unbindBinding(t_cm_instance_handle bindingComp, int portIdx) {
  DBC_ASSERT(0);
  return OMX_ErrorNotImplemented;
}

#else

OMX_ERRORTYPE
AFM_HostController::bindBinding(t_cm_instance_handle bindingComp, int portIdx) {
  t_nmf_error			 nmf_error;

  DBC_ASSERT(portIdx < NB_MAX_PORTS);

  // the portIdx s checked by the assert above
  //coverity[overrun-local]
  nmf_error = mNmfCtrl->bindToMPC(bindingItfNameHost[portIdx], bindingComp, "sendcommand",2);
  if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to bind controller %s!...[%d]\n", bindingItfNameHost[portIdx], nmf_error); return OMX_ErrorInsufficientResources; }
  
  nmf_error = mNmfCtrl->bindToMPC(bindingInitItfNameHost[portIdx], bindingComp, "fsminit", 2);
  if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to bind controller %s!...[%d]\n", bindingInitItfNameHost[portIdx], nmf_error); return OMX_ErrorInsufficientResources; }
  
  nmf_error = mNmfCtrl->bindFromMPC(bindingComp, "proxy", ctrlItfNameHost[portIdx], 4);
  if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to bind controller proxy!...[%d]\n", nmf_error); return OMX_ErrorInsufficientResources; }
  
  return OMX_ErrorNone;
}

OMX_ERRORTYPE
AFM_HostController::unbindBinding(t_cm_instance_handle bindingComp, int portIdx) {
  t_nmf_error   nmf_error;

  nmf_error = mNmfCtrl->unbindToMPC(bindingItfNameHost[portIdx], bindingComp, "sendcommand");
  if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to unbind controller %s!...[%d]\n", bindingItfNameHost[portIdx], nmf_error); return OMX_ErrorInsufficientResources; }
  
  nmf_error = mNmfCtrl->unbindToMPC(bindingInitItfNameHost[portIdx], bindingComp, "fsminit");
  if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to unbind controller %s!...[%d]\n", bindingInitItfNameHost[portIdx], nmf_error); return OMX_ErrorInsufficientResources; }
  
  nmf_error = mNmfCtrl->unbindFromMPC(bindingComp, "proxy", ctrlItfNameHost[portIdx]);
  if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to unbind controller proxy!...[%d]\n", nmf_error); return OMX_ErrorInsufficientResources; }
  
  return OMX_ErrorNone;
}

#endif // HOST_ONLY
