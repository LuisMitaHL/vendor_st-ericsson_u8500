/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef PARSER_MPC_HPP_
#define PARSER_MPC_HPP_

#include "JPEGDec_ARMNMF_Parser.h"
#include "JPEGDec_ARMNMF_ParamAndConfig.h"
#include "Component.h"

#include "t1xhv_vdc.idt"
#include "t1xhv_vdc_jpeg.idt"
#include "mpc/share/vfm_vdc_jpeg.idt"
#include "armnmf_vdc_jpeg.idt"
#include "OMX_Component.h"


#define VFM_HEADER_FIFO_COUNT 4
#define ENDIANESS_CONVERSION(number) ((t_uint32)(number)>>16 | ((t_uint32)(number) & 0xffff)<<16)


typedef struct {
	ts_ddep_sdc_jpeg_scan_desc *pHeader;
	OMX_BOOL isFree;
	OMX_BOOL isRequestedToFree ;
	t_uint8  sequenceNumber;
}armnmf_headerDesc;

class JPEGDec_ARMNMF_Parser_CLASS;

class jpegdec_arm_nmf_parser_mpc: public Component, public jpegdec_arm_nmf_parser_mpcTemplate
{
public:

    jpegdec_arm_nmf_parser_mpc();
    virtual ~jpegdec_arm_nmf_parser_mpc();

    virtual void process();
	virtual void processEvent(void);
    virtual void fsmInit(fsmInit_t initFSM);
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

    virtual void sendmemoryContext(void *mpc_vfm_mem_ctxt);

    virtual void ComputeARMLoad(t_bool );
	virtual void memoryRequirements(t_uint16 mChannelId,
									ts_ddep_buffer_descriptor paramBufferDesc,
									ts_ddep_buffer_descriptor linkListBufferDesc,
									ts_ddep_buffer_descriptor debugBufferDesc,
									ts_ddep_buffer_descriptor lineBufferDesc);

virtual void huffMemory(ts_ddep_buffer_descriptor HuffMemoryDesc);
	virtual void setConfig(t_uint32 nParamIndex,ts_ddep_sdc_jpeg_dynamic_params config);

    void releaseBtstBuffer(OMX_U8 btstContent,OMX_BUFFERHEADERTYPE *pBuffer);
    t_djpegErrorCode fillHeader(OMX_BUFFERHEADERTYPE *pBuffer);
    OMX_BUFFERHEADERTYPE* provideBitstreamBuffer();


    private:

    void set_pJdcIn_parameters(ts_t1xhv_vdc_jpeg_param_in *pJdcIn_parameters);
	void set_pJdcInOut_parameters();
	void set_pJdcOut_parameters();
    void setScanParams(ts_ddep_sdc_jpeg_scan_desc *pJdcScanParams);
	OMX_ERRORTYPE checkConfAndParams();

    void processBitstreamBuffer();
    void initHeaderDescriptors();
    void clearAllPendingRequestsToFreeHeader();
    ts_ddep_sdc_jpeg_scan_desc* getFreeHeaderDesc();
    void freeHeaderDesc(t_uint32 pHeader);
	void updateDynamicConfigurationStructure();
	void updateParamAndConfigScale(ts_ddep_sdc_jpeg_dynamic_params *pDynamicConfig);
	void updateParamAndConfigCrop(ts_ddep_sdc_jpeg_dynamic_params *pDynamicConfig);

	OMX_ERRORTYPE configureCoeffAndLineBuffer();


private:
    Port mPorts[2];
    JPEGDec_ARMNMF_ParamAndConfig_CLASS mParam;
    JPEGDec_ARMNMF_Parser_CLASS mParser;
    armnmf_headerDesc mHeaderDesc[VFM_HEADER_FIFO_COUNT];
    OMX_BUFFERHEADERTYPE mBufIn[4] ;
    OMX_BUFFERHEADERTYPE mBufOut[4];
    OMX_BUFFERHEADERTYPE *bufferInput;
	OMX_BOOL isInputBufferDequeued;

    OMX_U8 PARSER_WAITING_FOR_BUFFERS;
    OMX_STATETYPE compState;

    //Memory Requirements for MPC
    ts_ddep_buffer_descriptor mParamBufferDesc;
    ts_ddep_buffer_descriptor mLinkListBufferDesc;
    ts_ddep_buffer_descriptor mDebugBufferDesc;
    ts_ddep_buffer_descriptor mLineBufferDesc;
    ts_ddep_buffer_descriptor mHuffMemDesc;

    //Dynamic Configuration
    ts_ddep_sdc_jpeg_dynamic_configuration *m_ps_ddep_sdc_jpeg_dynamic_configuration;

    void *save_memory_context;
};
#endif /* PARSER_MPC_HPP_ */
