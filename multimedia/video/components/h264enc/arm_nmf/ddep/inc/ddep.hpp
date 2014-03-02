#include "omxilosalservices.h"
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __DDEP_HPP
#define __DDEP_HPP

#ifndef __SYMBIAN32__
#define IMPORT_C
#define EXPORT_C
#endif

#include <VFM_DDepUtility.h>
#include "omxilosalservices.h"

#ifndef NULL
#define NULL 0x0
#endif

#ifdef STAGEFRIGHT_SUPPORT // for stagefright output SPS-PPS in 1 buffer
#define H24ENC_SPS_PPS_IN_ONE_BUFFER
#endif

class h264enc_arm_nmf_ddep : public h264enc_arm_nmf_ddepTemplate, public VFM_nmf_ddep
{
public:

    h264enc_arm_nmf_ddep();
    virtual ~h264enc_arm_nmf_ddep();

    //Component virtual functions
    virtual void process() ;

    //Constructor interface from component.type
    virtual void fsmInit(fsmInit_t init);
    virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled)     { VFM_nmf_ddep::setTunnelStatus(portIdx, isTunneled); };
    virtual void reset() ;
    virtual void start(void) ;
    virtual void stop(void) ;
    virtual void destroy(void) ;


    virtual void processEvent(void)                              { Component::processEvent() ; }
    virtual void emptyThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer) { Component::deliverBuffer(0, pBuffer); }
    virtual void fillThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer)  { Component::deliverBuffer(1, pBuffer); }
    virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param) { VFM_nmf_ddep::sendCommand(cmd, param); }

    virtual void disablePortIndication(t_uint32 portIdx)    { /* FIXME: reset of the codec once disabling is performed */ } ;
    virtual void enablePortIndication(t_uint32 portIdx)     { /* reset of the codec once enabling is performed */ } ;
    virtual void flushPortIndication(t_uint32 portIdx)      { /* FIXME: reset of the codec once flush is performed */ } ;

public: //inherited from VFM_nmf_ddep
    virtual void flush_command_to_codec();
    virtual void pause_command_to_codec();
    virtual void send_vfm_memory(void *memory_ctxt);
    virtual void sendProxyEvent(OMX_IN OMX_EVENTTYPE eEvent,  OMX_IN OMX_U32 nData1, OMX_IN OMX_U32 nData2);

public:
    virtual void notify_input_buffer(void *p_b, t_uint32 size, t_valid_output valid, t_uint32 nTimeStampH, t_uint32 nTimeStampL, t_uint32 nFlags);
    virtual void notify_output_buffer(void *p_b, t_uint32 offset, t_uint32 size, t_valid_output valid, t_uint32 nTimeStampH, t_uint32 nTimeStampL, t_uint32 nFlags);
    virtual void set_config(t_uint32 domainDdrId, t_uint32 domainEsramId, t_frameinfo frame_info);
    virtual void pendingCommandAck(t_uint32 error);
	virtual void set_VideoStabParam(OMX_U32 OSWidth, OMX_U32 OSHeight);
        /* +Change start for CR 399938 */
	void set_param(void *ptr);
	void set_param(void) {}
        /* -Change end for CR 399938 */

private:
    void flushInput();
    void flushOutput();

    void updateDVFS();
//+ ER335583
    void updateDDR();
//- ER335583

    void processExtraData(OMX_BUFFERHEADERTYPE_p pBufferHdr,t_uint32 flag_tbd);

    void PrintParams();

private:
    bool check_start();
    void addPendingCommand();
    void waitForCommandCompletion();
    void nextState();
    void goToInit2();
    void goToInit3();
    void goToLoaded();
    void fill_sps_pps_buffers_request();
    void fill_sps_pps_buffers();

private:
    static const int mMaxInput=20, mMaxOutput=20;      // FIXME: how to have something dynamic?
    void ScheduleNextTaskIfNeeded();
    Port  mPorts[2];
    int mEncodingStatus;    // 0b00 all buffers free     - 0b10 input buffer in use
                            // 0b01 output buffer in use - 0b11 both buffers in use
    bool mIsFlushRequested;
    bool mIsPauseRequested;
    bool mGenerateSpsPps;
#ifdef H24ENC_SPS_PPS_IN_ONE_BUFFER // for stagefright output SPS-PPS in 1 buffer
    OMX_BUFFERHEADERTYPE* mOutputBufSpsPps;
    t_uint16 mSizeSpsPps;
#else
    OMX_BUFFERHEADERTYPE* mOutputBufSps;
    OMX_BUFFERHEADERTYPE* mOutputBufPps;
    t_uint16 mSizeSps;
    t_uint16 mSizePps;
#endif
	OMX_U32 osWidth,osHeight;
    t_frameinfo mFrameInfo;
    t_sps_pps mSequenceHeader;

    int mFrameNb;
    t_h264enc_state mState;

    OMX_U32 mNbPendingCommands;
    omxilosalservices::OmxILOsalMutex* mMutexPendingCommand;

    bool mOk;
    t_uint16 mPortsDisabled;
    t_uint16 mPortsStdTunneled;

    void *ptrParam;

};

#endif //__DDEP_HPP
