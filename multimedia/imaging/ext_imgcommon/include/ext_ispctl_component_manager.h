/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _EXT_ISPCTL_COMPONENT_MANAGER_H_
#define _EXT_ISPCTL_COMPONENT_MANAGER_H_

#include "OMX_Core.h" /*needed for error types*/

#include "ext_client.h"
#include "ext_deferredeventmgr.h"
#include "host/ext_ispctl/api/cmd.hpp"
#include "host/ext_ispctl/api/cfg.hpp"
#include "host/ext_ispctl/api/alert.hpp"

#include "host/api/set_debug.hpp"

#include "ENS_Component.h"
#include "ext_error.h"
class CScfStateMachine;

#define EXTICTL_FIFO_COMS_SIZE  16

#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CExtIspctlComponentManager);
#endif
class CExtIspctlComponentManager:public ext_ispctl_api_alertDescriptor
{

public :
   CExtIspctlComponentManager(t_sia_client_id clientId, ENS_Component &enscomp):mENSComponent(enscomp) { mSia_client_id =clientId;};
	OMX_ERRORTYPE instantiateNMF();
	OMX_ERRORTYPE configureNMF();
	OMX_ERRORTYPE startNMF() ;
	OMX_ERRORTYPE stopNMF() ;
	OMX_ERRORTYPE deinstantiateNMF() ;
    /* +353212 */
    void forcedStopNMF() ;
    /* -353212 */

	Iext_ispctl_api_cmd getExtIspctlCmd(void);
	Iext_ispctl_api_cfg getExtIspctlCfg(void);

    void setOMXHandle(OMX_HANDLETYPE aOSWrapper);
    void setSM(CScfStateMachine * sm);

	
    virtual void info(enum e_ispctlInfo info, t_uint32 value, t_uint32 timestamp) ;
    virtual void error(enum e_ispctlError error_id, t_uint32 data, t_uint32 timestamp);
    virtual void debug(enum e_ispctlDebug debug_id, t_uint16 data1, t_uint16 data2, t_uint32 timestamp);
	
	
protected :


private :
	t_sia_client_id mSia_client_id;

   OMX_HANDLETYPE   mOSWrapper;
   ENS_Component&   mENSComponent;
    CScfStateMachine * mSM;

   t_cm_instance_handle		mNmfExtISPCtl;

   Iext_ispctl_api_cfg mExtIspctlConfigure;
	Iext_ispctl_api_cmd mExtIspctlCommand;
	Iapi_set_debug mExtIspctlDebug;
};



#endif /**/
