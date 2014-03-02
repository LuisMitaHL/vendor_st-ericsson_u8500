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

class ext_stab : public Component, public ext_stabTemplate
{

public:

    ext_stab();

	/* splitter.api.inform interface methods */
	virtual void setConfig(enum e_grabctlPortID port_idx,  t_uint8 capture_started);

	virtual void setParams(struct s_grabParams GrabParams, t_uint32 input_port);

    /* grabctl.api.reportcurves interface methods */
	virtual void reportCurves(t_uint32 hCurveAddr, t_uint32 vCurveAddr, t_uint32 timestamp);

    /* stab.api.configure interface methods */
    virtual void informBuffers(t_uint32 hCurvePhysical, t_sint32 * hCurveLogical,
                               t_uint32 vCurvePhysical, t_sint32 * vCurveLogical,
                               t_uint32 x_size, t_uint32 y_size);

    virtual void activateStab(t_bool enable);
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
	virtual void processEvent(void)			  { Component::processEvent() ; }

	/* stab.conf emptythisbuffer interface emptyThisBuffer method, interface on input ports! */
	virtual void emptyThisBuffer(OMX_BUFFERHEADERTYPE_p buffer, t_uint8 index);
	/* stab.conf fillthisbuffer interface fillThisBuffer, interface on output ports! */
	virtual void fillThisBuffer(OMX_BUFFERHEADERTYPE_p buffer, t_uint8 index);

private:
	OMX_BUFFERHEADERTYPE *mBuf;
	Port mPorts[6];
    int mNbCurves;
    struct s_stabCurveMapping curveMapping;
    int mCurveIdx;
    int mCurveTimestamp;
    bool mStabEnabled;
    int x_size;
    int y_size;
    bool mStabFirstFrame;

	enum e_capturing
	{
		GRBCTL_CAPT_NOTCAPTURING = 0,
		GRBCTL_CAPT_PENDING_EOS = 1,
		GRBCTL_CAPT_CAPTURING = 2,
		GRBCTL_CAPT_ONE_SHOT = 3
	};
	e_capturing mCapturing[3];

	t_uint32 buffers_proc[6];
	t_uint32 srcPort, destPort, convPort,src_index,dest_index;
	t_uint32 FrameWidth[3], FrameHeight[3];
	enum e_grabFormat format[3];

	void stabilize(OMX_BUFFERHEADERTYPE *buf);
    OMX_ERRORTYPE addExtraData(OMX_BUFFERHEADERTYPE *buf, t_sint16 vect_x, t_sint16 vect_y);
    void extraDataInit(OMX_OTHER_EXTRADATATYPE *pExtraData);

	virtual int calcFilledLength(enum e_grabFormat format, t_uint16 xsize, t_uint16 ysize);
};
