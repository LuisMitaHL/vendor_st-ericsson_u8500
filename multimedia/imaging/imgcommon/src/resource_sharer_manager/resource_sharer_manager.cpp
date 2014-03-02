/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define OMXCOMPONENT "RESOURCE_SHARER_MANAGER"
#include "resource_sharer_manager.h"


#include "osi_trace.h"
#include "qhsm.h"  //rename_me scf_api.h
#include "qevent.h"
#include "error.h"


/* for traces */
#undef OMX_TRACE_UID
#define OMX_TRACE_UID 0x8

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "imgcommon_src_resource_sharer_manager_resource_sharer_managerTraces.h"
#endif



/*
 * Both following functions are used as setter for the handler and the COM_SM that are needed in this resource sharer manager
 */
void CResourceSharerManager::setOMXHandle(OMX_HANDLETYPE aOSWrapper) {
	mOSWrapper = aOSWrapper;
}
void CResourceSharerManager::setSM(CScfStateMachine * aSM) {
	mSM = aSM;
}



/*
 * This function instantiates the resource sharer
 */
OMX_ERRORTYPE CResourceSharerManager::instantiateNMF() {
    t_nmf_error nmf_err = NMF_OK;
    MSG0("Instantiate  CResourceSharerManager ...\n");
    OstTraceFiltStatic0(TRACE_DEBUG, "Instantiate  CResourceSharerManager ...", (mTraceObject));
    rs = resource_sharerWrappedCreate();
    if (NULL == rs) {
        DBGT_ERROR("CResourceSharerManager::instantiateNMF, error in resource_sharerWrappedCreate\n");
        OstTraceFiltStatic0(TRACE_ERROR, "CResourceSharerManager::instantiateNMF, error in resource_sharerWrappedCreate", (mTraceObject));
        return OMX_ErrorHardware;
    }
    else {
        nmf_err = rs->construct();
        if (nmf_err != NMF_OK) {
            DBGT_ERROR("Error: rs->construct() 0x%X\n", nmf_err);
            OstTraceFiltStatic1(TRACE_ERROR, "Error: rs->construct() 0x%X", (mTraceObject), nmf_err);
            return OMX_ErrorHardware;
        }
    }
    return OMX_ErrorNone;
}

/*
 * This function binds the IF, it is based on a clientId which is an attribute to the class,
 * whose value is depending on client choice (today, "1" for ISPPROC, and "0" for CAMERA).
 */
OMX_ERRORTYPE CResourceSharerManager::bindNMF() {
	t_nmf_error nmf_err = NMF_OK;
	MSG0("Bind  CResourceSharerManager ...\n");
	OstTraceFiltStatic0(TRACE_DEBUG, "Bind  CResourceSharerManager ...", (mTraceObject));

	const char *iFreeNamesArray[] = {"free[0]", "free[1]", "free[2]","free[3]"};
	const char *iPingNamesArray[] = {"ping[0]", "ping[1]", "ping[2]", "ping[3]"};

	nmf_err = rs->bindFromUser(iPingNamesArray[sia_client_id], 4, &mRSPing);
	nmf_err |= rs->bindFromUser(iFreeNamesArray[sia_client_id], 4, &mRSFree);
	if (nmf_err != NMF_OK) {
		DBGT_ERROR("Error: RS bind() 0x%X\n", nmf_err);
		OstTraceFiltStatic1(TRACE_ERROR, "Error: RS bind() 0x%X", (mTraceObject), nmf_err);
		return OMX_ErrorHardware;
	}

	const char *iLockNamesArray[] = {"lock[0]", "lock[1]", "lock[2]", "lock[3]"};
	const char *iReleaseNamesArray[] = {"release[0]", "release[1]", "release[2]", "release[3]"};
	nmf_err = rs->bindFromUser(iLockNamesArray[sia_client_id], 4, &mRSLock);
	nmf_err |= rs->bindFromUser(iReleaseNamesArray[sia_client_id], 4, &mRSRelease);
	if (nmf_err != NMF_OK) {
		DBGT_ERROR("Error: RS bind() 0x%X\n", nmf_err);
		OstTraceFiltStatic1(TRACE_ERROR, "Error: RS bind() 0x%X", (mTraceObject), nmf_err);
		return OMX_ErrorHardware;
	}

	const char *iAlertNamesArray[] = {"alert[0]", "alert[1]", "alert[2]", "alert[3]"};

	nmf_err = rs->bindFromUser(iFreeNamesArray[sia_client_id], 1, &mRSFree);
	nmf_err |= EnsWrapper_bindToUser(mOSWrapper, rs, iAlertNamesArray[sia_client_id],(resource_sharer_api_rs_alertDescriptor *)this, 2);

	if (nmf_err != NMF_OK) {
		DBGT_ERROR("Error: RS bind() 0x%X\n", nmf_err);
		OstTraceFiltStatic1(TRACE_ERROR, "Error: RS bind() 0x%X", (mTraceObject), nmf_err);
		return OMX_ErrorHardware;
	}





    return OMX_ErrorNone;
}

/*
 * This function is empty
 */
OMX_ERRORTYPE CResourceSharerManager::configureNMF() {
    MSG0("Configure  CResourceSharerManager OK\n");
    OstTraceFiltStatic0(TRACE_DEBUG, "Configure  CResourceSharerManager OK", (mTraceObject));
    return OMX_ErrorNone;
}

/*
 * This function stops the resource sharer
 */
OMX_ERRORTYPE CResourceSharerManager::stopNMF() {
	MSG0("Stop  CResourceSharerManager ...\n");
	OstTraceFiltStatic0(TRACE_DEBUG, "Stop  CResourceSharerManager ...", (mTraceObject));
	rs->stop_flush();
    return OMX_ErrorNone;

}

/*
 * This function de-instantiates and un-binds the IF of the resource sharer
 */
OMX_ERRORTYPE CResourceSharerManager::deinstantiateNMF() {
	t_nmf_error nmf_err = NMF_OK;
	MSG0("Deinstantiate  CResourceSharerManager ...\n");
	OstTraceFiltStatic0(TRACE_DEBUG, "Deinstantiate  CResourceSharerManager ...", (mTraceObject));
	const char *iFreeNamesArray[] = {"free[0]", "free[1]", "free[2]", "free[3]"};
	const char *iAlertNamesArray[] = {"alert[0]", "alert[1]", "alert[2]", "alert[3]"};
	const char *iPingNamesArray[] = {"ping[0]", "ping[1]", "ping[2]", "ping[3]"};
	nmf_err = rs->unbindFromUser(iFreeNamesArray[sia_client_id]);
	if (nmf_err != NMF_OK) {
		DBGT_ERROR("Error: RS Unbind() 0x%X\n", nmf_err);
		OstTraceFiltStatic1(TRACE_ERROR, "Error: RS Unbind() 0x%X", (mTraceObject), nmf_err);
		return OMX_ErrorHardware;
	}
	nmf_err = rs->unbindFromUser(iPingNamesArray[sia_client_id]);
	if (nmf_err != NMF_OK) {
		DBGT_ERROR("Error: RS Unbind() 0x%X\n", nmf_err);
		OstTraceFiltStatic1(TRACE_ERROR, "Error: RS Unbind() 0x%X", (mTraceObject), nmf_err);
		return OMX_ErrorHardware;
	}

	const char *iLockNamesArray[] = {"lock[0]", "lock[1]", "lock[2]", "lock[3]"};
	const char *iReleaseNamesArray[] = {"release[0]", "release[1]", "release[2]", "release[3]"};
	nmf_err = rs->unbindFromUser(iLockNamesArray[sia_client_id]);
	if (nmf_err != NMF_OK) {
		DBGT_ERROR("Error: RS Unbind() 0x%X\n", nmf_err);
		OstTraceFiltStatic1(TRACE_ERROR, "Error: RS Unbind() 0x%X", (mTraceObject), nmf_err);
		return OMX_ErrorHardware;
	}
	nmf_err = rs->unbindFromUser(iReleaseNamesArray[sia_client_id]);
	if (nmf_err != NMF_OK) {
		DBGT_ERROR("Error: RS Unbind() 0x%X\n", nmf_err);
		OstTraceFiltStatic1(TRACE_ERROR, "Error: RS Unbind() 0x%X", (mTraceObject), nmf_err);
		return OMX_ErrorHardware;
	}

	nmf_err = EnsWrapper_unbindToUser(mOSWrapper, rs, iAlertNamesArray[sia_client_id]);
	if (nmf_err != NMF_OK) {
		DBGT_ERROR("Error: RS UNbind() 0x%X\n", nmf_err);
		OstTraceFiltStatic1(TRACE_ERROR, "Error: RS UNbind() 0x%X", (mTraceObject), nmf_err);
		return OMX_ErrorHardware;
	}
	rs->destroy();
	resource_sharerWrappedDestroy(rs);

    return OMX_ErrorNone;
}

/*
 * This function starts the resource sharer
 */
OMX_ERRORTYPE CResourceSharerManager::startNMF() {

	MSG0("Start  CResourceSharerManager ...\n");
	OstTraceFiltStatic0(TRACE_DEBUG, "Start  CResourceSharerManager ...", (mTraceObject));
	rs->start();
    return OMX_ErrorNone;
}




void CResourceSharerManager::freed(void ){
    ASYNC_IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CResourceSharerManager::freed", (mTraceObject));
    MSG0("CResourceSharerManager::freed ...\n");
    OstTraceFiltStatic0(TRACE_DEBUG, "CResourceSharerManager::freed ...", (mTraceObject));
	DBC_ASSERT(0); //OBSOLETE!!!
    ASYNC_OUT0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Exit CResourceSharerManager::freed", (mTraceObject));
}

void CResourceSharerManager::pong(void ){
    ASYNC_IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CResourceSharerManager::pong", (mTraceObject));
    MSG0("CResourceSharerManager::pong ...\n");
    OstTraceFiltStatic0(TRACE_DEBUG, "CResourceSharerManager::pong ...", (mTraceObject));
    s_scf_event event;
    event.sig = Q_PONG_SIG;
	mSM->ProcessEvent(&event);
    ASYNC_OUT0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Exit CResourceSharerManager::pong", (mTraceObject));
}

void CResourceSharerManager::accepted(t_uint16 onGoing, t_uint16 requesterId ){
    ASYNC_IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CResourceSharerManager::accepted", (mTraceObject));
    MSG2("CResourceSharerManager::accepted, onGoing = %d (%s)\n",onGoing,CError::stringRSRequesterId((t_requesterID)onGoing));
    OstTraceFiltStatic1(TRACE_DEBUG, "CResourceSharerManager::accepted, onGoing = %d)", (mTraceObject),onGoing);

	s_scf_event event;
	event.sig = Q_LOCK_OK_SIG;
	mSM->ProcessEvent(&event);

    ASYNC_OUT0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Exit CResourceSharerManager::accepted", (mTraceObject));
}

void CResourceSharerManager::denied(t_uint16 requesterId  ){
    ASYNC_IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CResourceSharerManager::denied", (mTraceObject));
    MSG2("CResourceSharerManager::denied, requesterId = %d (%s)\n", requesterId,CError::stringRSRequesterId((t_requesterID)requesterId));
    OstTraceFiltStatic1(TRACE_DEBUG, "CResourceSharerManager::denied, requesterId = %d", (mTraceObject), requesterId);

    s_scf_event event;
    event.sig = Q_LOCK_KO_SIG;
	mSM->ProcessEvent(&event);
    ASYNC_OUT0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Exit CResourceSharerManager::denied", (mTraceObject));
}


void CResourceSharerManager::released(t_uint16 requesterId  ){
    ASYNC_IN0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Entry CResourceSharerManager::released", (mTraceObject));
    MSG2("CResourceSharerManager::released, requesterId = %d (%s)\n",requesterId, CError::stringRSRequesterId((t_requesterID)requesterId));
    OstTraceFiltStatic1(TRACE_DEBUG, "CResourceSharerManager::released, requesterId = %d", (mTraceObject),requesterId);
    s_scf_event event;
    event.sig = Q_RELEASED_SIG;
	mSM->ProcessEvent(&event);
    ASYNC_OUT0("\n");
    OstTraceFiltStatic0(TRACE_FLOW, "Exit CResourceSharerManager::released", (mTraceObject));
}

