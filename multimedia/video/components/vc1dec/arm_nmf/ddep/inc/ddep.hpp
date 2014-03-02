/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief   Include file for Data Dependency layer for vc1 decoder.
 * \author  ST-Ericsson
 */


#ifndef __DDEP_HPP
#define __DDEP_HPP

#ifndef __SYMBIAN32__
#define IMPORT_C
#define EXPORT_C
#endif

#include <VFM_DDepUtility.h>

class vc1dec_arm_nmf_ddep : public vc1dec_arm_nmf_ddepTemplate, public VFM_nmf_ddep
{
public:

    vc1dec_arm_nmf_ddep();
    virtual ~vc1dec_arm_nmf_ddep();

    //Component virtual functions
    virtual void process() ;

    //Constructor interface from component.type
    virtual void fsmInit(fsmInit_t init);
    virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled) { /* FIXME: to be implemented */ }
	virtual void reset() ;
    virtual void destroy(void) ;
	    virtual void start(void) ;
	virtual void stop(void) ;


    virtual void processEvent(void)                              { Component::processEvent() ; }
    virtual void emptyThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer) { Component::deliverBuffer(0, pBuffer); }
    virtual void fillThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer)  { Component::deliverBuffer(1, pBuffer); }
    virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param) { VFM_nmf_ddep::sendCommand(cmd, param); }
    virtual void disablePortIndication(t_uint32 portIdx)    { /* reset of the codec once disabling is performed */ } ;
    virtual void enablePortIndication(t_uint32 portIdx)     { /* reset of the codec once enabling is performed */ } ;
    virtual void flushPortIndication(t_uint32 portIdx)      { /* reset of the codec once flush is performed */ } ;

public:
    virtual void nal_handshake(t_handshake mode, t_bit_buffer *p);
    virtual void flush(void);  /* +ER 341788 CHANGE START FOR */
    virtual void picture(t_picture_output valid, t_uint8 *p_fb, t_uint32 nFlags, t_frameinfo info);
    virtual void set_config(t_uint32 domainDdrId, t_uint32 domainEsramId, t_frameinfo frame_info);
    virtual void set_configuration(void *ptr);
    virtual void flush_command_to_codec();
    virtual void pause_command_to_codec();
    virtual void send_vfm_memory(void *memory_ctxt);
    virtual void sendProxyEvent(OMX_IN OMX_EVENTTYPE eEvent,  OMX_IN OMX_U32 nData1, OMX_IN OMX_U32 nData2);

protected:
    IMPORT_C virtual void process_inputbuffer(OMX_BUFFERHEADERTYPE *pBuffer, OMX_BOOL isInternal);
    IMPORT_C virtual void process_special_inputbuffer(OMX_BUFFERHEADERTYPE *pBuffer);
    IMPORT_C virtual OMX_BOOL specificActivateInputFastPath(OMX_BUFFERHEADERTYPE *pBuffer) const;
    IMPORT_C virtual OMX_BOOL isSpecialCase(OMX_BUFFERHEADERTYPE *pBuffer) const;

    IMPORT_C virtual void process_outputbuffer(OMX_BUFFERHEADERTYPE *pBuffer, OMX_BOOL isInternal);

    void updateDVFS();
//+ ER335583
    void updateDDR();
//- ER335583

private:
    Port                    mPorts[2];

    // These are user functions
private:
    static const int mMaxInput=200, mMaxOutput=64;      // FIXME: how to have something dynamic?
    bool mNeedMoreNal;

    void checkInputSize();
    void copyAllInput();

    void check_picinfo(const t_frameinfo &info);

    bool mSendLastNal;
    OMX_U32 mLastNalFlag;

    t_frameinfo mFrameInfo;
	t_uint16 pic_width;
	t_uint16 pic_height;
	void *ptrOMXHandle;
};

#endif
