/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef MEMGRABCTL_H_
#define MEMGRABCTL_H_

#include "host/memgrabctl/api/configure.hpp"
#include "grabctlcommon.h"
#include "client.h"

#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CMemGrabControl);
#endif
class CMemGrabControl : public CGrabControlCommon {
    public:
        CMemGrabControl(t_sia_client_id clientId,ENS_Component &enscomp):CGrabControlCommon(enscomp),mENSComponent(enscomp) 
        {
            mGrab_client_id = clientId;
            mNmfMemGrabctl = (t_cm_instance_handle)(NULL);
            mNmfGrab = (t_cm_instance_handle)(NULL);
            mNmfLibGeneric = (t_cm_instance_handle)(NULL);
            mNmfLibComponent = (t_cm_instance_handle)(NULL);
            mNmfTrace = (t_cm_instance_handle)(NULL);
        }

        virtual void setEndCapture(t_uint16);
        virtual void setStartCapture(t_uint16) {}
        virtual void setStartOneShotCapture(t_uint16) { }
        void setStartOneShotCaptureAll(t_uint16 main_port_idx, t_uint16 snd_port_idx,t_uint16 third_port_idx,t_uint16 nb);
        virtual void callInterface(t_grab_desc *pGrabDesc, struct s_grabParams grabparams);
        virtual void configureHook(struct s_grabParams &grabParams, t_uint16 port_idx);

        void setOMXHandle(OMX_HANDLETYPE aOSWrapper);

        t_cm_instance_handle getMemControllerHandle(void);

        OMX_ERRORTYPE instantiate();
        OMX_ERRORTYPE deinstantiate();
        OMX_ERRORTYPE forcedDestroy(void);
        OMX_ERRORTYPE start();
        OMX_ERRORTYPE stop();

        t_sia_client_id getClientId(void) {return mGrab_client_id;};
        void setClientId(t_sia_client_id client_id) {mGrab_client_id = client_id;};		


    private:
        OMX_HANDLETYPE mOSWrapper;
        ENS_Component& mENSComponent;
        t_cm_instance_handle mNmfMemGrabctl;
        t_cm_instance_handle mNmfGrab;
        t_cm_instance_handle mNmfLibGeneric; //ENS FSM
        t_cm_instance_handle mNmfLibComponent; //ENS FSM
        t_cm_instance_handle mNmfTrace; //ENS FSM


        Imemgrabctl_api_configure mConfigureControl;

        t_sia_client_id mGrab_client_id;
};

#endif /* MEMGRABCTL_H_ */
