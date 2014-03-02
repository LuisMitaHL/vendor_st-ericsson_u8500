/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _EXT_RESOURCE_SHARER_MANAGER_H_
#define _EXT_RESOURCE_SHARER_MANAGER_H_
#include "ext_client.h"

#include <cm/inc/cm.hpp> /* needed for t_nmf_core_id */
#include "OMX_Core.h" /*needed for error types*/
#include "resource_sharer/api/rs_alert.hpp"
#include "resource_sharer/api/free.hpp"
#include "resource_sharerWrapped.hpp"


class CScfStateMachine;

#define RS_FIFO_COMS_SIZE  16


typedef enum{
	CAMERA_RAW,
    ISPPROC_SINGLE_STILL,
    ISPPROC_SINGLE_SNAP,
	ISPPROC_BURST,
	CAMERA_VF,
	REQUESTER_MAX
}t_requesterID;



#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CResourceSharerManager);
#endif
class CResourceSharerManager :public resource_sharer_api_rs_alertDescriptor
{

	public :
	CResourceSharerManager(t_sia_client_id clientId) {sia_client_id=clientId;};
	OMX_ERRORTYPE instantiateNMF();
	OMX_ERRORTYPE bindNMF() ;
	OMX_ERRORTYPE configureNMF();
	OMX_ERRORTYPE startNMF() ;
	OMX_ERRORTYPE stopNMF() ;
	OMX_ERRORTYPE deinstantiateNMF() ;

    void setOMXHandle(OMX_HANDLETYPE aOSWrapper);
    void setSM(CScfStateMachine * sm);


    virtual void freed(void);
	virtual void pong(void);
    virtual void accepted(t_uint16 onGoing, t_uint16 requesterId);
	virtual void denied(t_uint16 requesterId);
	virtual void released(t_uint16 requesterId);

    OMX_HANDLETYPE  mOSWrapper;
    CScfStateMachine *     mSM;
    Iresource_sharer_api_free mRSFree;
	Iresource_sharer_api_ping mRSPing;
    Iresource_sharer_api_lock mRSLock;
	Iresource_sharer_api_release mRSRelease;
	t_sia_client_id getClientId(void) {return sia_client_id;};
	void setClientId(t_sia_client_id client_id) {sia_client_id = client_id;};
	protected :

	private :

	resource_sharerWrapped * rs;
    t_sia_client_id sia_client_id;

};



#endif /*_EXT_RESOURCE_SHARER_MANAGER_H_*/
