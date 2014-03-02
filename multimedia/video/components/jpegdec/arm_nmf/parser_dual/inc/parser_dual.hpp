/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef PARSER_DUAL_HPP_
#define PARSER_DUAL_HPP_

#include "JPEGDec_ARMNMF_Parser.h"
#include "JPEGDec_ARMNMF_ParamAndConfig.h"
#include "Component.h"
#include "VFM_Memory.h"

//#include "t1xhv_vdc.idt"
//#include "t1xhv_vdc_jpeg.idt"
#include "mpc/share/vfm_vdc_jpeg.idt"
//#include "armnmf_vdc_jpeg.idt"
#include "OMX_Component.h"


#define VFM_HEADER_FIFO_COUNT 4
#define ENDIANESS_CONVERSION(number) ((t_uint32)(number)>>16 | ((t_uint32)(number) & 0xffff)<<16)


typedef struct
{
		t_uint32  buffer_p;
		t_uint32  bitOffset;
		t_uint32  encodedDataSize;
		t_uint32  physicalAddressParamIn;
		t_uint32  headerBufferMpcAddress;
} ts_sdc_jpeg_scan_desc;



typedef struct ts_image_buf_link_and_header {
		ts_t1xhv_vdc_frame_buf_out bufferLink;
		OMX_BUFFERHEADERTYPE *pBuffer;
		struct ts_image_buf_link_and_header * next;
		struct ts_image_buf_link_and_header * prev;
}ts_image_buf_link_and_header;

//bitstream buffer list
typedef struct {
	ts_t1xhv_bitstream_buf_link bufferLink;
	OMX_BUFFERHEADERTYPE *pBuffer;
} ts_bitstream_buf_link_and_header;




typedef struct {
	ts_ddep_sdc_jpeg_scan_desc *pHeader;
	OMX_BOOL isFree;
	OMX_BOOL isRequestedToFree ;
	t_uint8  sequenceNumber;
}armnmf_headerDesc;

class JPEGDec_ARMNMF_Parser_CLASS;

class jpegdec_arm_nmf_parser_dual: public Component, public jpegdec_arm_nmf_parser_dualTemplate
{
public:

    jpegdec_arm_nmf_parser_dual();
    virtual ~jpegdec_arm_nmf_parser_dual();

    void ResolveDependencies();
    void return_OutputBuffers(OMX_U32 output_buffSize);
    void return_InputBuffers();

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

    virtual void ComputeARMLoad(t_bool);

	virtual void memoryRequirements(t_uint16 mChannelId,
									ts_ddep_buffer_descriptor paramBufferDesc,
									ts_ddep_buffer_descriptor linkListBufferDesc,
									ts_ddep_buffer_descriptor debugBufferDesc,
									ts_ddep_buffer_descriptor lineBufferDesc);
virtual void huffMemory(ts_ddep_buffer_descriptor HuffMemoryDesc);

 virtual void sendmemoryContext(void *mpc_vfm_mem_ctxt);

	virtual void setConfig(t_uint32 nParamIndex,ts_ddep_sdc_jpeg_dynamic_params config);

    void releaseBtstBuffer(OMX_U8 btstContent,OMX_BUFFERHEADERTYPE *pBuffer);
    t_djpegErrorCode fillHeader(OMX_BUFFERHEADERTYPE *pBuffer);
    OMX_BUFFERHEADERTYPE* provideBitstreamBuffer();


    private:

    void set_pJdcIn_parameters(ts_t1xhv_vdc_jpeg_param_in *pJdcIn_parameters);
	void set_pJdcInOut_parameters();
	void set_pJdcOut_parameters();
    void setScanParams(ts_ddep_sdc_jpeg_scan_desc *pJdcScanParams);
    OMX_BOOL compare_hw_huff_tables(ts_t1xhv_vdc_jpeg_param_in *pJdcIn_values);
	OMX_ERRORTYPE checkConfAndParams();

    void processBitstreamBuffer();
    void initHeaderDescriptors();
    void clearAllPendingRequestsToFreeHeader();
    ts_ddep_sdc_jpeg_scan_desc* getFreeHeaderDesc();
    void freeHeaderDesc(t_uint32 pHeader);
	void updateDynamicConfigurationStructure();
	void updateParamAndConfigScale(ts_ddep_sdc_jpeg_dynamic_params *pDynamicConfig);
	void updateParamAndConfigCrop(ts_ddep_sdc_jpeg_dynamic_params *pDynamicConfig);
	virtual void endAlgo(t_t1xhv_status status, t_t1xhv_decoder_info info, t_uint32 durationInTicks);

	t_uint16 addBufferToBtsLinkList(OMX_BUFFERHEADERTYPE *pBuffer,ts_bitstream_buf_link_and_header *currentBufferLink,t_ahb_address *return_addr);
	t_uint16 removeBufferFromBtsLinkList(ts_bitstream_buf_link_and_header *BufferLinkToRemove);
	t_uint16 readLastBtsBufferfromLinkList(ts_bitstream_buf_link_and_header *currentBufferLink);


	ts_image_buf_link_and_header *imageBufferLinkStart;
	//functions for image buffer management
	t_uint16 addBufferToImageLinkList(OMX_BUFFERHEADERTYPE *pBuffer,ts_image_buf_link_and_header *currentBufferLink);
	t_uint16 removeBufferfromImageLinkList(ts_image_buf_link_and_header *BufferLinkToRemove);
	t_uint16 readFirstImageBufferfromLinkList(ts_image_buf_link_and_header *currentBufferLink);
	void setParam(void* portDef1, void* portDef2);

	OMX_ERRORTYPE configureCoeffAndLineBuffer();


private:
    Port mPorts[2];
    OMX_BOOL isSWDecoderReq;
    OMX_U8 subSamplingType;
	OMX_U8 colourMode;
	OMX_BOOL bitstreamBufferProcessed;
    JPEGDec_ARMNMF_ParamAndConfig_CLASS mParam;
    JPEGDec_ARMNMF_Parser_CLASS mParser;
    armnmf_headerDesc mHeaderDesc[VFM_HEADER_FIFO_COUNT];
    OMX_BUFFERHEADERTYPE mBufIn[4] ;
    OMX_BUFFERHEADERTYPE mBufOut[4];
    OMX_BUFFERHEADERTYPE *bufferInput;
	OMX_BOOL MPC_isInputBufferDequeued;
	OMX_BOOL isEOSTrue;
	OMX_BOOL processActualExecution;

	OMX_PARAM_PORTDEFINITIONTYPE portDef1,portDef2;
	ts_bitstream_buf_link_and_header *bufferLinkStart;
	t_uint16 current_frame_width;
	t_uint16 current_frame_height;
	OMX_BOOL firstBufferWithBitstream;
	//ts_ddep_sdc_jpeg_scan_desc *headerBuf;

	ts_sdc_jpeg_scan_desc sdc_jpeg_scan_desc;
	ts_ddep_sdc_jpeg_param_desc sdc_jpeg_param_desc;

	ENS_List inputBufferFifo;
	ENS_List inputHeaderFifo;
	OMX_BOOL inputDependencyResolved;
	OMX_BOOL outputDependencyResolved;
	OMX_BOOL headerDependencyResolved;
	OMX_BOOL isOutputBufferDequeue,isPortSetting;


    OMX_BOOL isInputBufferDequeue;
    OMX_BOOL bufferUpdateNeeded;
    OMX_U32 bufferSizeReq,allocatedBufferSize;
    OMX_COLOR_FORMATTYPE formatType;

    OMX_U8 PARSER_WAITING_FOR_BUFFERS;
    OMX_STATETYPE compState;

	OMX_U32	downsamplingFactor;
	OMX_U32 windowWidth;
	OMX_U32 windowHeight;

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
#endif /* PARSER_DUAL_HPP_ */
