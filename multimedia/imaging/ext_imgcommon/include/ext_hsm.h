/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _EXT_HSM_H_
#define _EXT_HSM_H_

#include "ext_qhsm.h"
#include "ext_qf_os.h"
#include "ext_qevent.h"
#include "ext_ispctl_communication_manager.h"
#include "ext_deferredeventmgr.h"
#include "ext_trace.h"
#include "ENS_Component.h"
#include "ext_ispctl_component_manager.h"
#include "debug_trace.h"
#include "ext_omx_state_manager.h"
#include <cm/inc/cm.hpp>

#define ENTRY
#define EXIT
#define INIT

/*Configuring ISP pipe related */
typedef enum {
    ConfiguringISPPipe_DisaRecordPipe=0,
    ConfiguringISPPipe_EnaRecordPipe,
    ConfiguringISPPipe_DisaCapturePipe,
    ConfiguringISPPipe_EnaCapturePipe,
    ConfiguringISPPipe_ResolutionOnly
}t_configuring_isp_pipe;


class COM_SM : public CScfStateMachine {

public:
    COM_SM(ENS_Component &enscomp);
    virtual ~COM_SM();


    OMX_BOOL isExpectedState() {
        if STATE_IS_ACTIVE(&COM_SM::WaitEvent)
            return OMX_TRUE;
        else return OMX_FALSE;
    }



protected:
    virtual void timeoutAction (void);

    SCF_STATE  initial(s_scf_event const *e);
    SCF_STATE  final(s_scf_event const *e);


    /* shared state hierarchy between imaging components */
    /* --------------------------------------------------*/
    SCF_STATE OMX_Executing(s_scf_event const *e);
    SCF_STATE Streaming(s_scf_event const *e);
    SCF_STATE WaitEvent(s_scf_event const *e);
    virtual SCF_STATE ProcessPendingEvents(s_scf_event const *e);


    /* pure virtual allowing to start the state machine : this is the entry point */
    /* ---------------------------------------------------------------------------*/
    virtual SCF_STATE Booting(s_scf_event const *e)=0;

    /* pure virtual allowing to describe behaviour when ports are asked to be disabled or enabled*/
    /* ------------------------------------------------------------------------------------------*/
    virtual SCF_STATE EnablePortInIdleExecutingState(s_scf_event const *e) = 0;

    /* state handler allowing to describe the operating mode transition : somehow the Booting state handler will have to connect to them */
    /* ----------------------------------------------------------------------------------------------------------------------------------*/
    SCF_STATE OpModeTransition(s_scf_event const *e);
    SCF_STATE EnteringOpMode(s_scf_event const *e);
    SCF_STATE ExitingOpMode(s_scf_event const *e);
    virtual SCF_STATE EnteringPreview(s_scf_event const *e) = 0;
    virtual SCF_STATE PreparingGrabConfig(s_scf_event const *e) =0;

    /* common qstates */
    /* ---------------*/
    SCF_STATE SHARED_Error_handling(s_scf_event const *e);

public :
    SCF_PSTATE whatNext;


    /* CAUTION!!! */
    t_uint8 enabledPort;

    CDefferedEventMgr * pDeferredEventMgr;

    CExtIspctlCom * pExtIspctlCom;
    COmxStateMgr *pOmxStateMgr;

    ENS_Component& mENSComponent;
};

#endif  /* _EXT_HSM_H_*/

