/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/* brief   This file contains the methods needed to instantiate the EXT_ISPCTL DSP NMF component. It depends on NMF Component Manager API
 *       It :
 *           - builds the NMF network to communicate with EXT_ISPCTL
 *           - provides the interfaces needed to communicate with EXT_ISPCTL
 *           - implements the callback functions binded on EXTISPTCL Alert/infos/debug functions.
 */

#undef DBGT_LAYER
#define DBGT_LAYER 3
#undef DBGT_PREFIX
#define DBGT_PREFIX "ISPC"

#include "ext_ispctl_component_manager.h"
#include "MMIO_Camera.h"

#include "ENS_Nmf.h"
#include "debug_trace.h"
#include "ext_qhsm.h"
#include "ext_qevent.h"
//#include "hw_buffer_lib.h"

#include "ext_error.h"

/* for traces */
#undef OMX_TRACE_UID
#define OMX_TRACE_UID 0x8


void CExtIspctlComponentManager::setOMXHandle(OMX_HANDLETYPE aOSWrapper) {
    mOSWrapper = aOSWrapper;
}
void CExtIspctlComponentManager::setSM(CScfStateMachine * aSM) {
    mSM = aSM;
}

Iext_ispctl_api_cmd CExtIspctlComponentManager::getExtIspctlCmd(void) {
    return mExtIspctlCommand;
}

Iext_ispctl_api_cfg CExtIspctlComponentManager::getExtIspctlCfg(void) {
    return mExtIspctlConfigure;
}

OMX_ERRORTYPE CExtIspctlComponentManager::instantiateNMF()
{

    OMX_ERRORTYPE error = OMX_ErrorNone;
    DBGT_PTRACE("Instantiate  ExtIspCtl");

    OMX_U32 domainId = mENSComponent.getNMFDomainHandle(RM_NMFD_PROCSIA);

    /* Instantiate EXT_ISPCTL Component */
    t_cm_error cm_error = CM_InstantiateComponent(
            "ext_ispctl",
            domainId,
            NMF_SCHED_NORMAL,
            "ext_ispctl",
            &mNmfExtISPCtl);
    if ( cm_error != CM_OK){
        DBGT_PTRACE( "An error occured while instantiating ext_ispctl : %d", cm_error);
        return OMX_ErrorHardware;
    }

    DBGT_PTRACE("Bind  ExtIspCtl");
    error = ENS::bindComponentFromHost( mNmfExtISPCtl
            ,"iConfigure"
            ,(NMF::InterfaceReference *)&mExtIspctlConfigure
            , EXTICTL_FIFO_COMS_SIZE
            );

    if ( error != OMX_ErrorNone)
    {
        DBGT_PTRACE("Error : Bind Host->ext_ispctl (iConfigure) 0x%x",error);
        return error;
    }

    error = ENS::bindComponentFromHost( mNmfExtISPCtl
            ,"iCommand"
            ,(NMF::InterfaceReference *)&mExtIspctlCommand
            ,EXTICTL_FIFO_COMS_SIZE
            );

    if (error != OMX_ErrorNone)
    {
        DBGT_PTRACE("Error : Bind Host->ext_ispctl (iCommand) 0x%x",error);
        return error;
    }

    const char *iAlertItfNamesArray[] = {"iAlert[0]", "iAlert[1]"};

    DBC_ASSERT(mOSWrapper !=0);
    error = ENS::bindComponentToHost(
            mOSWrapper,
            mNmfExtISPCtl,
            iAlertItfNamesArray[mSia_client_id],
            (NMF::InterfaceDescriptor *)this,
            EXTICTL_FIFO_COMS_SIZE);

    if(error != OMX_ErrorNone)
    {
        /*  Fatal error : Return directly */
        DBGT_PTRACE("Error : Bind ext_ispctl->Host (iAlert) 0x%x",error);
        return error;
    }


    return OMX_ErrorNone;

}

OMX_ERRORTYPE CExtIspctlComponentManager::configureNMF()
{
    DBGT_PTRACE("Configure  ExtIspCtl OK");
    OMX_ERRORTYPE omxerr=OMX_ErrorNone;
    return omxerr;
}

OMX_ERRORTYPE CExtIspctlComponentManager::stopNMF()
{
    OMX_ERRORTYPE error = OMX_ErrorNone;

    DBGT_PTRACE("Stop ExtIspCtl ...");
    error = ENS::stopNMFComponent(mNmfExtISPCtl);
    if(error != OMX_ErrorNone)
    {
        DBGT_PTRACE("Error : Fail to stop ext_ispctl component  (error =0x%x)",error);
        return error;
    }
    DBGT_PTRACE("Stop ExtIspCtl OK");

    return error;
}

OMX_ERRORTYPE CExtIspctlComponentManager::deinstantiateNMF()
{
    OMX_ERRORTYPE error = OMX_ErrorNone;
    DBGT_PTRACE("Deinstantiate  EXTISPTCL ...");
    error = ENS::unbindComponentFromHost( (NMF::InterfaceReference *)&mExtIspctlCommand );
    if (error != OMX_ErrorNone)
    {
        DBGT_PTRACE("Error : Unbind Host->ext_ispctl (ext_ispctlCommand) (error =0x%x)",error);
        return error;
    }
    error = ENS::unbindComponentFromHost( (NMF::InterfaceReference *)&mExtIspctlConfigure );
    if (error != OMX_ErrorNone)
    {
        DBGT_PTRACE("Error : Unbind Host->ext_ispctl (ext_ispctlConfigure)   (error =0x%x)",error);
        return error;
    }
    const char *iAlertItfNamesArray[] = {"iAlert[0]", "iAlert[1]"};
    error = ENS::unbindComponentToHost( mOSWrapper, mNmfExtISPCtl, iAlertItfNamesArray[mSia_client_id]);
    if (error != OMX_ErrorNone)
    {
        DBGT_PTRACE("Error : ext_ispctl->sia_resource_manager (iAlert)  (error =0x%x)",error);
        return error;
    }
    /* Destroy EXTISPTCL component */
    error = ENS::destroyNMFComponent( mNmfExtISPCtl );
    if (error != OMX_ErrorNone)
    {
        DBGT_PTRACE("Error : Destroy ext_ispctl (error =0x%x)",error);
        return error;
    }
    DBGT_PTRACE("Deinstantiate  ExtIspCtl OK");
    return OMX_ErrorNone;
}

void CExtIspctlComponentManager::forcedStopNMF()
{
    ENS::stopNMFComponent( mNmfExtISPCtl );
}

OMX_ERRORTYPE CExtIspctlComponentManager::startNMF()
{

    OMX_ERRORTYPE error = OMX_ErrorNone;
    DBGT_PTRACE( "Start ExtIspCtl");
    error = ENS::startNMFComponent(mNmfExtISPCtl);
    if (error != OMX_ErrorNone){
        DBGT_PTRACE ("Error : Fail to start mNmfExtISPCtl component (error = %d)", error );
        return error;
    }
    return OMX_ErrorNone;
}

void CExtIspctlComponentManager::info( enum e_ispctlInfo info_id, t_uint32 value, t_uint32 timestamp)
{
    DBGT_PROLOG();

    s_scf_event event;
    event.sig = EVT_ISPCTL_INFO_SIG;
    event.type.ispctlInfo.info_id = info_id;
    event.type.ispctlInfo.value = value;
    event.type.ispctlInfo.timestamp = timestamp;
    event.type.ispctlInfo.number_of_pe = 0;

    DBGT_PTRACE("CExtIspctlComponentManager::Callback_ext_ispctlInfo,event.type.ispctlInfo.info_id 0x%x (%s)", event.type.ispctlInfo.info_id, CError::stringIspctlInfo(info_id));
    mSM->ProcessEvent(&event);

    DBGT_EPILOG();
}

void CExtIspctlComponentManager::error( enum e_ispctlError error_id, t_uint32 value, t_uint32 timestamp)
{
    DBGT_PROLOG();
    s_scf_event event;
    event.sig = EVT_ISPCTL_ERROR_SIG;
    event.type.ispctlError.error_id = error_id;
    event.type.ispctlError.last_PE_data_polled = value; //depends on error_id,  last_PE_data_polled or number_of_pe is the value
    event.type.ispctlError.number_of_pe = value;
    event.type.ispctlError.timestamp = timestamp;

    DBGT_PTRACE("  CExtIspctlComponentManager::Callback_ext_ispctlError,event.type.ispctlError.error_id 0x%x (%s), last_PE_data_polled =0x%x", (int)event.type.ispctlError.error_id, CError::stringIspctlError(error_id),(int)value);

    mSM->ProcessEvent(&event);
    DBGT_EPILOG();
}

void CExtIspctlComponentManager::debug(enum e_ispctlDebug debug_id, t_uint16 data1, t_uint16 data2, t_uint32 timestamp)
{
}

