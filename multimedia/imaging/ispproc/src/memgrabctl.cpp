/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "memgrabctl.h"
#include "ENS_DBC.h"
#include "osi_trace.h" /* for IN0() / OUT0() */
#include "ENS_Nmf.h"
//#include <cm/inc/cm_macros.h>
#include <host/grabctl/api/error.hpp>

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "ispproc_src_memgrabctlTraces.h"
#endif

//static void CBmemgrabctlError(void *ctx, enum e_grabError error_id, t_uint16 data, enum e_grabPipeID pipe_id);

void CMemGrabControl::setEndCapture(t_uint16 port_idx){
	IN0("\n");
	OstTraceFiltStatic0(TRACE_FLOW, "Entry CMemGrabControl::setEndCapture", (&mENSComponent));
	//NMFCALL (mConfigureControl, setConfig)((e_grabctlPortID)port_idx,GRABCTL_CFG_CAPTURING,0);
	OUTR(" ",0);
	OstTraceFiltStatic0(TRACE_FLOW, "Exit CMemGrabControl::setEndCapture", (&mENSComponent));
}

void CMemGrabControl::configureHook(struct s_grabParams &grabParams, t_uint16 port_idx){

	int xsize, ysize;
	xsize = grabParams.x_window_size;
	ysize = grabParams.y_window_size;

	if (grabParams.rotation_cfg ==0)
	{
	   	grabParams.x_window_size = xsize;
	   	grabParams.y_window_size = ysize;
	}
	else
	{
		grabParams.y_window_size = xsize;
		grabParams.x_window_size = ysize;
	}
}


void CMemGrabControl::setStartOneShotCaptureAll(t_uint16 main_port_idx, t_uint16 snd_port_idx,t_uint16 third_port_idx,t_uint16 nb){
	IN0("\n");
	OstTraceFiltStatic0(TRACE_FLOW, "Entry CMemGrabControl::setStartOneShotCaptureAll", (&mENSComponent));
	mConfigureControl.setConfig((e_grabctlPortID)main_port_idx,(e_grabctlPortID)snd_port_idx,(e_grabctlPortID)third_port_idx,nb,GRABCTL_CFG_CAPTURING,2);
	OUTR(" ",0);
	OstTraceFiltStatic0(TRACE_FLOW, "Exit CMemGrabControl::setStartOneShotCaptureAll", (&mENSComponent));
}

void CMemGrabControl::callInterface(t_grab_desc *pGrabDesc, struct s_grabParams grabparams) {
	IN0("\n");
	OstTraceFiltStatic0(TRACE_FLOW, "Entry CMemGrabControl::callInterface", (&mENSComponent));
	mConfigureControl.setParams(grabparams,(e_grabctlPortID)pGrabDesc->port_idx,pGrabDesc->bufferCount);
	OUTR(" ", 0);
	OstTraceFiltStatic0(TRACE_FLOW, "Exit CMemGrabControl::callInterface", (&mENSComponent));
}

void CMemGrabControl::setOMXHandle(OMX_HANDLETYPE aOSWrapper)
{
	mOSWrapper = aOSWrapper;
}

t_cm_instance_handle CMemGrabControl::getMemControllerHandle(void)
{
	return mNmfMemGrabctl;
}

OMX_ERRORTYPE CMemGrabControl::instantiate()
{
	OMX_ERRORTYPE error = OMX_ErrorNone;

	MSG0("Instantiate MemGrabctl & Grab...\n");
	OstTraceFiltStatic0 (TRACE_DEBUG, "Instantiate MemGrabctl & Grab...", (&mENSComponent));
    error = ENS::instantiateNMFComponent(
    		mENSComponent.getNMFDomainHandle(),
    		"memgrabctl",
    		"memgrabctl",
    		&mNmfMemGrabctl);
    if ( error != OMX_ErrorNone ) {
       DBGT_ERROR("An error occurred while instantiating MemGrabctl : %d\n", error);
       OstTraceFiltStatic1 (TRACE_ERROR, "An error occurred while instantiating MemGrabctl : %d", (&mENSComponent), error);
       return error;
    }

    error = ENS::instantiateNMFComponent(
    		mENSComponent.getNMFDomainHandle(),
        		"grab",
        		"grab",
        		&mNmfGrab);
	if ( error != OMX_ErrorNone ) {
	   DBGT_ERROR("An error occurred while instantiating MemGrabctl : %d\n", error);
	   OstTraceFiltStatic1 (TRACE_ERROR, "An error occurred while instantiating MemGrabctl : %d", (&mENSComponent), error);
	   return error;
	}


	error = ENS::instantiateNMFComponent(
			 mENSComponent.getNMFDomainHandle(),
	    		"fsm.generic"
	    		,"fsm.generic"
	    		,&mNmfLibGeneric);
	if ( error != OMX_ErrorNone ){
		DBGT_ERROR("An error occured while instantiating generic : %d\n", error);
		OstTraceFiltStatic1 (TRACE_ERROR, "An error occured while instantiating generic : %d", (&mENSComponent), error);
	   // OUTR(" ",error);
    	//OstTraceFiltStatic1(TRACE_FLOW, "Exit CMemGrabControl::instantiate %d", (&mENSComponent),error);
		return error;
	}

	error = ENS::instantiateNMFComponent(
			mENSComponent.getNMFDomainHandle()
			,"fsm.component"
			,"fsm.component"
			,&mNmfLibComponent);
	if ( error != OMX_ErrorNone ){
		DBGT_ERROR("An error occured while instantiating component : %d\n", error);
		OstTraceFiltStatic1 (TRACE_ERROR, "An error occured while instantiating component : %d", (&mENSComponent), error);
		//OUTR(" ",error);
    	//OstTraceFiltStatic1(TRACE_FLOW, "Exit CMemGrabControl::instantiate %d", (&mENSComponent),error);
		return error;
	}

	MSG0("Bind MemGrabctl...\n");
	OstTraceFiltStatic0 (TRACE_DEBUG, "Bind MemGrabctl...", (&mENSComponent));
    error = ENS::bindComponentFromHost(
    		mNmfMemGrabctl,
    		"configure",
    		(NMF::InterfaceReference  *)&mConfigureControl,
    		2);
    if ( error != OMX_ErrorNone) {
        DBGT_ERROR("Error : Bind Host->MemGrabctl (configure) 0x%x\n",error);
        OstTraceFiltStatic1 (TRACE_ERROR, "Error : Bind Host->MemGrabctl (configure) 0x%x", (&mENSComponent),error);
        return error;
    }


    error = ENS::bindComponent(
    		mNmfMemGrabctl,
    		"grab_interface",
    		mNmfGrab,
    		"iCommand");
    if(error != OMX_ErrorNone) {
            DBGT_ERROR("Error : Bind MemGrabctl->Grab (execute) 0x%x\n",error);
            OstTraceFiltStatic1 (TRACE_ERROR, "Error : Bind MemGrabctl->Grab (execute) 0x%x", (&mENSComponent),error);
            return error;
	}

    const char *iAlertItfNamesArray[] = {"iAlert[0]", "iAlert[1]", "iAlert[2]", "iAlert[3]"};
    error = ENS::bindComponentAsynchronous(
				mNmfGrab,
				iAlertItfNamesArray[mGrab_client_id],
        		mNmfMemGrabctl,
        		"iGRABAlert",
        		50);
	if(error != OMX_ErrorNone) {
			DBGT_ERROR("Error : Bind Grab->MemGrabctl (alert) 0x%x\n",error);
			OstTraceFiltStatic1 (TRACE_ERROR, "Error : Bind Grab->MemGrabctl (alert) 0x%x", (&mENSComponent),error);
			return error;
	}


	 error = ENS::bindComponent( mNmfMemGrabctl, "genericfsm" , mNmfLibGeneric, "genericfsm");
    if(error != OMX_ErrorNone){
        DBGT_ERROR("Error : Error : grabctrl (genericfsm) -> genericfsm (error = %d)\n", error );
        OstTraceFiltStatic1 (TRACE_ERROR, "Error : Error : grabctrl (genericfsm) -> genericfsm (error = %d)", (&mENSComponent), error );
        //OUTR(" ",error);
    	//OstTraceFiltStatic1(TRACE_FLOW, "Exit CMemGrabControl::instantiate %d", (&mENSComponent),error);
        return error;
    }

    error = ENS::bindComponent( mNmfMemGrabctl, "componentfsm" , mNmfLibComponent, "componentfsm");
    if(error != OMX_ErrorNone){
        DBGT_ERROR("Error : Error : grabctrl (componentfsm) -> componentfsm (error = %d)\n", error );
        OstTraceFiltStatic1 (TRACE_ERROR, "Error : Error : grabctrl (componentfsm) -> componentfsm (error = %d)", (&mENSComponent), error );
        //OUTR(" ",error);
    	//OstTraceFiltStatic1(TRACE_FLOW, "Exit CMemGrabControl::instantiate %d", (&mENSComponent),error);
        return error;
    }


    error = ENS::bindComponent( mNmfLibComponent, "genericfsm" , mNmfLibGeneric, "genericfsm");
    if(error != OMX_ErrorNone){
        DBGT_ERROR("Error : Error : mNmfLibComponent (genericfsm) -> mNmfLibGeneric (genericfsm) (error = %d)\n", error );
        OstTraceFiltStatic1 (TRACE_ERROR, "Error : Error : mNmfLibComponent (genericfsm) -> mNmfLibGeneric (genericfsm) (error = %d)", (&mENSComponent), error );
        //OUTR(" ",error);
    	//OstTraceFiltStatic1(TRACE_FLOW, "Exit CMemGrabControl::instantiate %d", (&mENSComponent),error);
        return error;
    }

    /* do not bind ENS-related interfaces. Those are handled in ispproc.cpp */

    MSG0("Bind MemGrabctl, grab OK \n");
    OstTraceFiltStatic0 (TRACE_DEBUG, "Bind MemGrabctl, grab OK ", (&mENSComponent));
    return OMX_ErrorNone;
}

OMX_ERRORTYPE CMemGrabControl::deinstantiate() {
    //IN0("\n");
	//OstTraceFiltStatic0(TRACE_FLOW, "Entry CMemGrabControl::deinstantiate", (&mENSComponent));
    OMX_ERRORTYPE error = OMX_ErrorNone;

	MSG0("Deinstantiate MemGrabCtl ...\n");
	OstTraceFiltStatic0 (TRACE_DEBUG, "Deinstantiate MemGrabCtl ...", (&mENSComponent));

    error = ENS::unbindComponentFromHost( (NMF::InterfaceReference  *)&mConfigureControl );
    if (error != OMX_ErrorNone)
    {
        DBGT_ERROR("Error : Unbind Host->MemGrabCtl (mConfigureControl) (error =0x%x)\n",error);
        OstTraceFiltStatic1 (TRACE_ERROR, "Error : Unbind Host->MemGrabCtl (mConfigureControl) (error =0x%x)", (&mENSComponent),error);
        //OUTR(" ",error);
    	//OstTraceFiltStatic1(TRACE_FLOW, "Exit CMemGrabControl::deinstantiate %d", (&mENSComponent),error);
        return error;
    }

    error = ENS::unbindComponent( mNmfMemGrabctl,"grab_interface");
    if (error != OMX_ErrorNone)
    {
        DBGT_ERROR("Error : Unbind Host->MemGrabCtl (ispctlCommand) (error =0x%x)\n",error);
        OstTraceFiltStatic1 (TRACE_ERROR, "Error : Unbind Host->MemGrabCtl (ispctlCommand) (error =0x%x)", (&mENSComponent),error);
        //OUTR(" ",error);
    	//OstTraceFiltStatic1(TRACE_FLOW, "Exit CMemGrabControl::deinstantiate %d", (&mENSComponent),error);
        return error;
    }

    const char *iAlertItfNamesArray[] = {"iAlert[0]", "iAlert[1]", "iAlert[2]", "iAlert[3]"};
    error = ENS::unbindComponentAsynchronous(mNmfGrab, iAlertItfNamesArray[mGrab_client_id]);
    if (error != OMX_ErrorNone)
    {
        DBGT_ERROR("Error : ispctl->sia_resource_manager (iAlert)  (error =0x%x)\n",error);
        OstTraceFiltStatic1 (TRACE_ERROR, "Error : ispctl->sia_resource_manager (iAlert)  (error =0x%x)", (&mENSComponent),error);
        return error;
    }

    error = ENS::unbindComponent ( mNmfMemGrabctl , "componentfsm");
    if (error != OMX_ErrorNone){
        DBGT_ERROR("Error : UnBind grabcontroler->componentfsm (componentfsm)   (error = %d)\n", error );
        OstTraceFiltStatic1 (TRACE_ERROR, "Error : UnBind grabcontroler->componentfsm (componentfsm)   (error = %d)", (&mENSComponent), error );
        //OUTR(" ",error);
    	//OstTraceFiltStatic1(TRACE_FLOW, "Exit CMemGrabControl::deinstantiate %d", (&mENSComponent),error);
        return error;
    }

    error = ENS::unbindComponent ( mNmfMemGrabctl , "genericfsm");
    if (error != OMX_ErrorNone){
        DBGT_ERROR("Error : UnBind grabcontroler->genericfsm (genericfsm)   (error = %d)\n", error );
        OstTraceFiltStatic1 (TRACE_ERROR, "Error : UnBind grabcontroler->genericfsm (genericfsm)   (error = %d)", (&mENSComponent), error );
        //OUTR(" ",error);
    	//OstTraceFiltStatic1(TRACE_FLOW, "Exit CMemGrabControl::deinstantiate %d", (&mENSComponent),error);
        return error;
    }

    error = ENS::unbindComponent ( mNmfLibComponent , "genericfsm");
    if (error != OMX_ErrorNone){
        DBGT_ERROR("Error : UnBind mNmfLibComponent -> genericfsm (genericfsm)   (error = %d)\n", error );
        OstTraceFiltStatic1 (TRACE_ERROR, "Error : UnBind mNmfLibComponent -> genericfsm (genericfsm)   (error = %d)", (&mENSComponent), error );
        //OUTR(" ",error);
    	//OstTraceFiltStatic1(TRACE_FLOW, "Exit CMemGrabControl::deinstantiate %d", (&mENSComponent),error);
        return error;
    }

    error = ENS::destroyNMFComponent(mNmfMemGrabctl);
    if (error != OMX_ErrorNone) {
        DBGT_ERROR("Error : Destroy MemGrabctl (error =0x%x)\n",error);
        OstTraceFiltStatic1 (TRACE_ERROR, "Error : Destroy MemGrabctl (error =0x%x)", (&mENSComponent),error);
        return error;
    }

    error = ENS::destroyNMFComponent(mNmfGrab);
    if (error != OMX_ErrorNone) {
        DBGT_ERROR("Error : Destroy Grab (error =0x%x)\n",error);
        OstTraceFiltStatic1 (TRACE_ERROR, "Error : Destroy Grab (error =0x%x)", (&mENSComponent),error);
        return error;
    }

    error = ENS::destroyNMFComponent( mNmfLibGeneric );
    if (error != OMX_ErrorNone){
        DBGT_ERROR("Error : Destroy genericfsm  (error = %d)\n", error );
        OstTraceFiltStatic1 (TRACE_ERROR, "Error : Destroy genericfsm  (error = %d)", (&mENSComponent), error );
        //OUTR(" ",error);
    	//OstTraceFiltStatic1(TRACE_FLOW, "Exit CMemGrabControl::deinstantiate %d", (&mENSComponent),error);
        return error;
    }

    error = ENS::destroyNMFComponent( mNmfLibComponent );
    if (error != OMX_ErrorNone){
        DBGT_ERROR("Error : Destroy componentfsm  (error = %d)\n", error );
        OstTraceFiltStatic1 (TRACE_ERROR, "Error : Destroy componentfsm  (error = %d)", (&mENSComponent), error );
        //OUTR(" ",error);
    	//OstTraceFiltStatic1(TRACE_FLOW, "Exit CMemGrabControl::deinstantiate %d", (&mENSComponent),error);
        return error;
    }

    MSG0("Deinstantiate  MemGrabctl OK \n");
    OstTraceFiltStatic0 (TRACE_DEBUG, "Deinstantiate  MemGrabctl OK ", (&mENSComponent));
    return OMX_ErrorNone;
}

/* 
  called from COmxIspProc::NmfPanicCallback in case of NMF panic on SIA.
  Brutally destroy the whole NMF network on DSP side
  Note that in this function we FORCE all components to stop and destroy 
    whatever the error code returned by NMF functions !
*/
OMX_ERRORTYPE CMemGrabControl::forcedDestroy() 
{
    OMX_ERRORTYPE error = OMX_ErrorUndefined;

    error = ENS::unbindComponentFromHost( (NMF::InterfaceReference  *)&mConfigureControl );
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("CMemGrabControl::forcedDestroy : ENS::unbindComponentFromHost mConfigureControl error =0x%x\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "CMemGrabControl::forcedDestroy : ENS::unbindComponentFromHost mConfigureControl error =0x%x", (&mENSComponent),error);
    }
    error = ENS::unbindComponent( mNmfMemGrabctl,"grab_interface");
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("CMemGrabControl::forcedDestroy : unbindComponent grab_interface error =0x%x\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "CMemGrabControl::forcedDestroy : unbindComponent grab_interface error =0x%x", (&mENSComponent),error);
    }

    const char *iAlertItfNamesArray[] = {"iAlert[0]", "iAlert[1]", "iAlert[2]", "iAlert[3]"};
    error = ENS::unbindComponentAsynchronous(mNmfGrab, iAlertItfNamesArray[mGrab_client_id]);
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("CMemGrabControl::forcedDestroy : ENS::unbindComponentAsynchronous iAlertItfNamesArray error =0x%x\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "CMemGrabControl::forcedDestroy : ENS::unbindComponentAsynchronous iAlertItfNamesArray error =0x%x", (&mENSComponent),error);
    }

    error = ENS::unbindComponent ( mNmfMemGrabctl , "componentfsm");
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("CMemGrabControl::forcedDestroy : ENS::unbindComponent componentfsm error =0x%x\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "CMemGrabControl::forcedDestroy : ENS::unbindComponent componentfsm error =0x%x", (&mENSComponent),error);
    }
    error = ENS::unbindComponent ( mNmfMemGrabctl , "genericfsm");
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("CMemGrabControl::forcedDestroy : ENS::unbindComponent mNmfMemGrabctl error =0x%x\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "CMemGrabControl::forcedDestroy : ENS::unbindComponent mNmfMemGrabctl error =0x%x", (&mENSComponent),error);
    }
    error = ENS::unbindComponent ( mNmfLibComponent , "genericfsm");
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("CMemGrabControl::forcedDestroy : ENS::unbindComponent mNmfLibComponent error =0x%x\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "CMemGrabControl::forcedDestroy : unbindComponent mNmfLibComponent error =0x%x", (&mENSComponent),error);
    }
    error = ENS::destroyNMFComponent(mNmfMemGrabctl);
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("CMemGrabControl::forcedDestroy : ENS::destroyNMFComponent mNmfMemGrabctl error =0x%x\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "CMemGrabControl::forcedDestroy : ENS::destroyNMFComponent mNmfMemGrabctl error =0x%x", (&mENSComponent),error);
    }
    error = ENS::destroyNMFComponent(mNmfGrab);
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("CMemGrabControl::forcedDestroy : ENS::destroyNMFComponent mNmfGrab error =0x%x\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "CMemGrabControl::forcedDestroy : ENS::destroyNMFComponent mNmfGrab error =0x%x", (&mENSComponent),error);
    }
    error = ENS::destroyNMFComponent( mNmfLibGeneric );
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("CMemGrabControl::forcedDestroy : ENS::destroyNMFComponent mNmfLibGeneric error =0x%x\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "CMemGrabControl::forcedDestroy : ENS::destroyNMFComponent mNmfLibGeneric error =0x%x", (&mENSComponent),error);
    }
    error = ENS::destroyNMFComponent( mNmfLibComponent );
    if (OMX_ErrorNone != error) {
        DBGT_ERROR("CMemGrabControl::forcedDestroy : ENS::destroyNMFComponent mNmfLibComponent error =0x%x\n",error);
        OstTraceFiltStatic1(TRACE_ERROR, "CMemGrabControl::forcedDestroy : ENS::destroyNMFComponent mNmfLibComponent error =0x%x", (&mENSComponent),error);
    }

    return OMX_ErrorNone;
}


OMX_ERRORTYPE CMemGrabControl::start()
{
    OMX_ERRORTYPE error = OMX_ErrorNone;

    MSG0("Starting MemGrabctl\n");
    OstTraceFiltStatic0 (TRACE_DEBUG, "Starting MemGrabctl", (&mENSComponent));
    error = ENS::startNMFComponent(mNmfMemGrabctl);
    if (error != OMX_ErrorNone) {
        DBGT_ERROR("Error : Fail to start MemGrabctl component (error = %d)\n", error );
        OstTraceFiltStatic1 (TRACE_ERROR, "Error : Fail to start MemGrabctl component (error = %d)", (&mENSComponent), error );
        return error;
    }

    mConfigureControl.setClientID(mGrab_client_id);


    return OMX_ErrorNone;
}

OMX_ERRORTYPE CMemGrabControl::stop()
{
	OMX_ERRORTYPE error = OMX_ErrorNone;

	MSG0("Stop MemGrabctl ...\n");
	OstTraceFiltStatic0 (TRACE_DEBUG, "Stop MemGrabctl ...", (&mENSComponent));
    error = ENS::stopNMFComponent(mNmfMemGrabctl);
    if(error != OMX_ErrorNone) {
        DBGT_ERROR("Error : Failed to stop MemGrabctl component (error =0x%x)\n",error);
        OstTraceFiltStatic1 (TRACE_ERROR, "Error : Failed to stop MemGrabctl component (error =0x%x)", (&mENSComponent),error);
        return error;
    }
    MSG0("Stop MemGrabctl OK\n");
    OstTraceFiltStatic0 (TRACE_DEBUG, "Stop MemGrabctl OK", (&mENSComponent));

    return OMX_ErrorNone;
}


