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
#include <arm_codec_type.idt>
#include <host_sei.h>
#include <host_decoder.h>
#include "omxilosalservices.h"

class h264dec_arm_nmf_ddep : public h264dec_arm_nmf_ddepTemplate, public VFM_nmf_ddep
{
public:

    h264dec_arm_nmf_ddep();
    virtual ~h264dec_arm_nmf_ddep();

    //Component virtual functions
    virtual void process() ;

    //Constructor interface from component.type
    virtual void fsmInit(fsmInit_t init);
    virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled)     { VFM_nmf_ddep::setTunnelStatus(portIdx, isTunneled); };
    virtual void start(void) ;
    virtual void stop(void) ;
    virtual void destroy(void) ;

    virtual void processEvent(void)                              { Component::processEvent() ; }
    virtual void emptyThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer) { Component::deliverBuffer(0, pBuffer); } 
    virtual void fillThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer)  { Component::deliverBuffer(1, pBuffer); }
    virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param) { VFM_nmf_ddep::sendCommand(cmd, param); }
    virtual void stateChangeIndication(OMX_STATETYPE oldState, OMX_STATETYPE newState);
    virtual void disablePortIndication(t_uint32 portIdx)    { /* reset of the codec once disabling is performed */ } ;
    virtual void enablePortIndication(t_uint32 portIdx)     { /* reset of the codec once enabling is performed */ } ;
    virtual void flushPortIndication(t_uint32 portIdx)      { /* reset of the codec once flush is performed */ } ;

public:
    virtual void nal_handshake(t_handshake mode, t_bit_buffer *p);
    virtual void picture(t_picture_output valid, t_uint16 *p_fb, t_uint32 nFlags, t_frameinfo info);
    virtual void set_config(t_uint32 domainDdrId, t_uint32 domainEsramId, t_frameinfo frame_info);
	virtual void set_configuration(void *omx_ptr);
    virtual void flush_command_to_codec();
    virtual void pause_command_to_codec();
    virtual void send_vfm_memory(void *memory_ctxt);
    virtual void sendProxyEvent(OMX_IN OMX_EVENTTYPE eEvent,  OMX_IN OMX_U32 nData1, OMX_IN OMX_U32 nData2);
    virtual void pendingCommandAck(t_ack_error_codes error);

protected:
    IMPORT_C virtual void process_inputbuffer(OMX_BUFFERHEADERTYPE *pBuffer, OMX_BOOL isInternal);
    IMPORT_C virtual void process_special_inputbuffer(OMX_BUFFERHEADERTYPE *pBuffer);
    IMPORT_C virtual OMX_BOOL specificActivateInputFastPath(OMX_BUFFERHEADERTYPE *pBuffer) const;
    IMPORT_C virtual OMX_BOOL isSpecialCase(OMX_BUFFERHEADERTYPE *pBuffer) const;
    IMPORT_C virtual void process_outputbuffer(OMX_BUFFERHEADERTYPE *pBuffer, OMX_BOOL isInternal);
    
    void updatePower();
    void codec_ack_flushingspec(OMX_U32 nPortIndex);
    
    void delayed_fsmInit(fsmInit_t initFSM,bool invalid);
    void addPendingCommand();
    void error_recovery();
    void fatal_decoding_error(t_uint16 error_code);
    void h264dec_assert(int condition, int line, OMX_BOOL isFatal, int param1=0, int param2=0);

private:
    Port                    mPorts[2];

    // These are user functions
private:
    // Max number of buffers
    // These numbers must be the same in function H264Dec_Proxy::getMaxBuffers()
    static const int mMaxInput=128, mMaxOutput=128;
    bool mNeedMoreNal;
    t_SEI    mSei;
    
    void copyAndSend(VFM_DDepInternalBuffer *pInternal);
    OMX_BOOL activateFastPath(OMX_BUFFERHEADERTYPE *pBuffer) const;
    void copyAllInput();
    
    void check_picinfo(t_frameinfo info);
    
    bool mSendLastNal;
    OMX_U32 mLastNalFlag;
    void dumpMemoryStatus(void) const;
    
    // ------------ Members that are specific to H264dec
    
    /// @brief The DDep parameters, that are used to send data to the proxy, like the profile / level
    /// or the AVC parameters.
    t_frameinfo mFrameInfo;
    
    bool mFsmInitCalled;
    bool mPendingSendConfig;    
    bool mPendingPicSize;
// +CR324558 CHANGE START FOR
	bool mPendingDPBSize;
// -CR324558 CHANGE END OF
    OMX_U32 mH264DecMaskFlushingAck;

    OMX_U32 mNbPendingCommands;
    omxilosalservices::OmxILOsalMutex* mMutexPendingCommand;
    OMX_S64 mLastTimestamp;
	void *pOMXHandle;
	fsmInit_t mInitFSM;		//Delayed fsm_init
	bool mFsmInitDelayed;
};

#endif
