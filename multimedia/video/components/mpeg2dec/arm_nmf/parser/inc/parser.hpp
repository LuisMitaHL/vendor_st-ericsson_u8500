/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef PARSER_HPP_
#define PARSER_HPP_

#include "Component.h"

#include "t1xhv_vdc.idt"
#include "t1xhv_vdc_mpeg2.idt"
#include "mpc/share/vfm_vdc_mpeg2.idt"
#include "armnmf_vdc_mpeg2.idt"
#include "OMX_Component.h"
#include "host_m2d_parser.h"

#define INPUT_BUFFER_COUNT 16
#define OUTPUT_BUFFER_COUNT 16
#define VFM_HEADER_FIFO_COUNT COUNT_HEADER_BUFFERS

#define ENDIANESS_CONVERSION(number) ((t_uint32)(number)>>16 | ((t_uint32)(number) & 0xffff)<<16)


typedef struct {
	ts_ddep_vdc_mpeg2_header_desc *pHeader;
	OMX_BOOL isFree;
	OMX_BOOL isRequestedToFree ;
	t_uint8  sequenceNumber;
}armnmf_headerDesc;

class mpeg2dec_arm_nmf_parser: public Component, public mpeg2dec_arm_nmf_parserTemplate
{
public:

    mpeg2dec_arm_nmf_parser();
    virtual ~mpeg2dec_arm_nmf_parser();

    virtual void process();
	virtual void processEvent(void);
    virtual void fsmInit(fsmInit_t init);
	virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled);
	virtual void reset();
    virtual void disablePortIndication(t_uint32 portIdx);
    virtual void enablePortIndication(t_uint32 portIdx);
    virtual void flushPortIndication(t_uint32 portIdx);
    virtual void destroy(void) ;
    

	virtual t_nmf_error construct(void);
	virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param);

    virtual void fromProcComp_emptyThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer);
    virtual void fromProcComp_fillThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer);

    virtual void fromSharedBufferComp_emptyThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer);
    virtual void fromSharedBufferComp_fillThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer);
    
    virtual void fillThisHeader(Buffer_p buffer);
	virtual void computeARMLoad(t_bool disable); 
	virtual void memoryRequirements(t_uint16 mChannelId,
									ts_ddep_buffer_descriptor paramBufferDesc,
									ts_ddep_buffer_descriptor linkListBufferDesc,
									ts_ddep_buffer_descriptor debugBufferDesc,
									t_uint32 ouputPortBufferCountActual,
									t_uint8 errorMapFlag,t_uint16 *proxyErrorMapBuffer);
	virtual void sendportSettings(t_uint32 width,t_uint32 height, t_uint32 concealment_flag);

    void releaseBtstBuffer(OMX_U8 btstContent,OMX_BUFFERHEADERTYPE *pBuffer);
    t_dmpeg2ErrorCode fillHeader(OMX_BUFFERHEADERTYPE *pBuffer,t_bit_buffer *pBtstBuffer);
    OMX_BUFFERHEADERTYPE* provideBitstreamBuffer();
	

    private:
		
	void setBitOffset(ts_ddep_vdc_mpeg2_header_desc *pHeader,t_bit_buffer *pBtstBuffer);
    void set_pJdcIn_parameters(ts_t1xhv_vdc_mpeg2_param_in *pIn_parameters, ts_mp2 *btst_mp2);
	void set_pJdcInOut_parameters();
	void set_pJdcOut_parameters();
    void setScanParams(ts_ddep_vdc_mpeg2_header_desc *pJdcScanParams);
	OMX_ERRORTYPE checkConfAndParams();

    void processBitstreamBuffer();
    void initHeaderDescriptors();
    void clearAllPendingRequestsToFreeHeader();
    ts_ddep_vdc_mpeg2_header_desc* getFreeHeaderDesc();
    void freeHeaderDesc(t_uint32 pHeader);
	void detectPortSettingsAndNotify();


private:
    Port mPorts[2];
    armnmf_headerDesc mHeaderDesc[VFM_HEADER_FIFO_COUNT];
    OMX_BUFFERHEADERTYPE mBufIn[INPUT_BUFFER_COUNT] ;
    OMX_BUFFERHEADERTYPE mBufOut[OUTPUT_BUFFER_COUNT];

	ts_mp2 mpeg2StreamParams;
	mpeg2decParser mParser;

    OMX_STATETYPE compState;
	OMX_BOOL updateData;
	t_uint32 frame_info_width;
	t_uint32 frame_info_height;
	t_uint32 config_concealmentflag;

    //Memory Requirements for MPC 
    ts_ddep_buffer_descriptor mParamBufferDesc;
    ts_ddep_buffer_descriptor mLinkListBufferDesc;
    ts_ddep_buffer_descriptor mDebugBufferDesc;

	//ErrorMap

    t_uint8 errorMapReportingFlag;
	t_uint16 *mProxyErrorMapBuffer;
};
#endif /* PARSER_HPP_ */
