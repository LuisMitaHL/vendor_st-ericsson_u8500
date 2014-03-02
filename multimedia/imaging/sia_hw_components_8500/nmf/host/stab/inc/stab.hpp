/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \brief   Stab ARM-NMF component class header
*/
#include "Component.h" /* because we NMF-extend fsm.component.component */

#ifdef __SYMBIAN32__
#include "openmax/il/shai/OMX_Symbian_IVCommonExt.h" /* for stab extradata type */
#else
#include "OMX_Symbian_IVCommonExt_Ste.h" /* for stab extradata type */
#endif

#define TIMESTAMP_TICKS 90000 /* DSP timestamps are from a 90khz clock */

enum e_stabPortID {
	PORT_IN_LR,
	PORT_OUT_LR,
    PORT_IN_BMS,
    PORT_OUT_BMS,
	PORT_IN_HR,
	PORT_OUT_HR,
	PORT_MAX
};

struct s_stabCurveMapping {
    t_uint32 hCurvePhysical[2];
    t_sint32 * hCurveLogical[2];
    t_uint32 vCurvePhysical[2];
    t_sint32 * vCurveLogical[2];
};
struct s_stabVector{
    t_sint16 vect_x;
    t_sint16 vect_y;
    t_uint32 timestamp;
};

enum e_stabApplyVector {
    STAB_VECTOR_APPLIED,
    STAB_VECTOR_NOTAPPLIED
};


#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(stab);
#endif
class stab : public Component, public stabTemplate
{

public:

    stab();
    ~stab();

    /* grabctl.api.reportcurves interface methods */
	virtual void reportCurves(t_uint32 hCurveAddr, t_uint32 vCurveAddr, t_uint32 timestamp);

    /* stab.api.configure interface methods */
    virtual void informBuffers(t_uint32 hCurvePhysical, t_sint32 * hCurveLogical,
                               t_uint32 vCurvePhysical, t_sint32 * vCurveLogical,
                               t_uint32 lr_x_size, t_uint32 lr_y_size,
                               t_uint32 hr_x_size, t_uint32 hr_y_size,
                               t_uint32 lr_x_origin, t_uint32 lr_y_origin,
                               t_uint32 hr_x_origin, t_uint32 hr_y_origin,
                               t_uint32 max_lr_x_size, t_uint32 max_lr_y_size,
                               t_uint32 max_hr_x_size, t_uint32 max_hr_y_size,t_sint32 rotation);

    virtual void activateStab(t_bool enable, t_bool lr_only, t_uint32 framerate);
	/* void functions from Component.h */
	virtual void reset();
    virtual void disablePortIndication(t_uint32 portIdx);
    virtual void enablePortIndication(t_uint32 portIdx);
    virtual void flushPortIndication(t_uint32 portIdx);
	virtual void process();


	/* fsminit interface from component.type */
	virtual void fsmInit(fsmInit_t inits);
    virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled);

	/* fsm.component.component.type interface sendcommand method sendCommand */
	virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param);
	/* fsm.component.component.type interface postevent method processEvent */
	virtual void processEvent(void)  			  { Component::processEvent() ; }

	/* stab.conf emptythisbuffer interface emptyThisBuffer method, interface on input ports! */
	virtual void emptyThisBuffer(OMX_BUFFERHEADERTYPE_p buffer, t_uint8 index);
	/* stab.conf fillthisbuffer interface fillThisBuffer, interface on output ports! */
	virtual void fillThisBuffer(OMX_BUFFERHEADERTYPE_p buffer, t_uint8 index);



private:
	Port mPorts[6];
    int mNbCurves;
    struct s_stabCurveMapping mCurveMapping;
    struct s_stabVector mLastVector;
    int mCurveIdx;
    int mCurveTimestamp;
    bool mStabEnabled;
    int mFramerate;
    int lr_x_size, lr_y_size, hr_x_size, hr_y_size;
    int lr_x_origin, lr_y_origin, hr_x_origin, hr_y_origin;
    int max_lr_x_size, max_lr_y_size, max_hr_x_size, max_hr_y_size;
    bool mStabFirstFrame;
    bool mLROnly;
    t_sint32 mDelta, mMaxDelta;
    t_sint32 mRotation;

	void stabilize(OMX_BUFFERHEADERTYPE *buf);
    enum e_stabApplyVector applyHRVector(OMX_BUFFERHEADERTYPE *buf);
    OMX_ERRORTYPE initExtraData(OMX_BUFFERHEADERTYPE* pOmxBufHdr, enum e_stabPortID port_id);
    OMX_ERRORTYPE clearAllExtraData(OMX_BUFFERHEADERTYPE* pOmxBufHdr);
    OMX_ERRORTYPE setExtraDataField(OMX_U32 nExtradataType, OMX_BUFFERHEADERTYPE* pOmxBufHdr,
						void* pExtradataData, OMX_U32 nExtradataDataSize, OMX_VERSIONTYPE version);
    OMX_ERRORTYPE setExtraDataField(OMX_BUFFERHEADERTYPE* pOmxBufHdr, OMX_SYMBIAN_DIGITALVIDEOSTABTYPE* pExtradataData, OMX_VERSIONTYPE version);						

};
