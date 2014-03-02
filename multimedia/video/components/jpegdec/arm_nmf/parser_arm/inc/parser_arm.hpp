/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef PARSER_ARM_HPP_
#define PARSER_ARM_HPP_

//#include <jpegdec/arm_nmf/parser_arm.nmf>

#include "JPEGDec_ARMNMF_Parser.h"
#include "JPEGDec_ARMNMF_ParamAndConfig.h"
#include "Component.h"

//#include "t1xhv_vdc.idt"
//#include "t1xhv_vdc_jpeg.idt"
//#include "decoder.idt"

#include "arm_nmf/share/vfm_vdc_jpeg.idt"
#include "armnmf_vdc_jpeg.idt"
#include "OMX_Component.h"
#include "jpeg_dynamic.idt"

#include <stdio.h>
#define VFM_HEADER_FIFO_COUNT 4
//#define ENDIANESS_CONVERSION(number) ((t_uint32)(number)>>16 | ((t_uint32)(number) & 0xffff)<<16)
#define ENDIANESS_CONVERSION(number) (number)

//structures for pay load management
// Image buffer list
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
/////////////////////////////
typedef struct {
	ts_ddep_sdc_jpeg_scan_desc *pHeader;
	OMX_BOOL isFree;
	OMX_BOOL isRequestedToFree ;
	t_uint8  sequenceNumber;
}armnmf_headerDesc;


typedef struct{
t_uint32 frame_width;
t_uint32 frame_height;
t_uint32 crop_window_width;
t_uint32 crop_window_height;
t_uint8  downSamplingFactor;
}armnmf_dynamicConfiguration;

class JPEGDec_ARMNMF_Parser_CLASS;
class jpegdec_arm_nmf_parser_arm: public Component, public jpegdec_arm_nmf_parser_armTemplate
{
private:
	typedef struct
{
		t_uint32  buffer_p;
		t_uint32  bitOffset;
		t_uint32  encodedDataSize;
		t_uint32  physicalAddressParamIn;
		t_uint32  headerBufferMpcAddress;
		t_uint16  isFakeHeader;
		t_uint16  lastHeader;
} ts_sdc_jpeg_scan_desc;
public:

    jpegdec_arm_nmf_parser_arm();
    virtual ~jpegdec_arm_nmf_parser_arm();

    //Component virtual functions
    virtual void process();

    //Constructor interface from component.type
    virtual void fsmInit(fsmInit_t initFSM);
	virtual void reset();
    virtual void disablePortIndication(t_uint32 portIdx);
    virtual void enablePortIndication(t_uint32 portIdx);
    virtual void flushPortIndication(t_uint32 portIdx);
	virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled);
    virtual void destroy(void) ;

	virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param);

    virtual void processEvent(void)
	{
		//printf("\nARM NMF processevent");
        Component::processEvent() ;
	}
    virtual void emptyThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer)
	{
#ifdef DBG_PRINT
		printf("\nARM NMF empty this buffer = %x\n",pBuffer);
#endif
		Component::deliverBuffer(0, pBuffer);
	}
    virtual void fillThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer)
	{
#ifdef DBG_PRINT
		printf("\nARM NMF fill this buffer = %x\n",pBuffer);
#endif
        Component::deliverBuffer(1, pBuffer);
	}

    virtual t_nmf_error construct(void);
    void releaseBtstBuffer(OMX_U8 btstContent,OMX_BUFFERHEADERTYPE *pBuffer);
    t_djpegErrorCode fillHeader(OMX_BUFFERHEADERTYPE *pBuffer);
    OMX_BUFFERHEADERTYPE* provideBitstreamBuffer();
	virtual void endAlgo(t_t1xhv_status status, t_t1xhv_decoder_info info, t_uint32 durationInTicks);

	virtual void setParam(void* portDef1, void* portDef2);
    private:
    void set_pJdcIn_parameters(ts_t1xhv_vdc_jpeg_param_in *pJdcIn_parameters);
    void setScanParams(ts_sdc_jpeg_scan_desc *pJdcScanParams);
	t_uint16 processBuffer(OMX_BUFFERHEADERTYPE *ptrOmxBuffer, ts_sdc_jpeg_scan_desc *ptrHeaderBuffer);
    void initHeaderDescriptors();
    void clearAllPendingRequestsToFreeHeader();
    ts_ddep_sdc_jpeg_scan_desc* getFreeHeaderDesc();
	void freeHeaderDesc(t_uint32 pBuffer);
	t_uint16 download_parameters();
	void return_InputBuffers();
	void return_OutputBuffers(OMX_U32);


private:
    Port mPorts[2];
    OMX_U32 subSamplingType;
    JPEGDec_ARMNMF_ParamAndConfig_CLASS mParam;
    JPEGDec_ARMNMF_Parser_CLASS mParser;
    OMX_U32 bufferSizeReq,allocatedBufferSize;
    OMX_COLOR_FORMATTYPE formatType;
	//FIXME make them dynamic
	//OMX_BUFFERHEADERTYPE mBufIn[4] ;
	//OMX_BUFFERHEADERTYPE mBufOut[4];
	OMX_BOOL isOutputBufferDequeue,isPortSetting,isEOSTrue,isInputBufferDequeue;
	OMX_BOOL processActualExecution;

    //Dynamic Configuration
	t_uint16 last_read_conf_no;
	ts_ddep_sdc_jpeg_dynamic_params armnmf_component_params_set;
	ts_ddep_sdc_jpeg_dynamic_configuration *ps_ddep_sdc_jpeg_dynamic_configuration;

    ts_ddep_buffer_descriptor mParamBufferDesc;
    ts_ddep_buffer_descriptor mLineBufferDesc;
    ts_ddep_buffer_descriptor mHuffMemDesc;

	OMX_PARAM_PORTDEFINITIONTYPE portDef1,portDef2;

	OMX_U32	downsamplingFactor;
	OMX_U32 colourMode;
	OMX_U32 windowWidth;
	OMX_U32 windowHeight;


	OMX_BOOL PARSER_WAITING_FOR_BUFFERS;

	//armnmf_dynamicConfiguration currentDynamicConfiguration;
    //virtual void endScanParsing(armnmf_error status, t_uint32 ptrOmxBuffer, t_uint32 ptrHeaderBuffer);
	/*************************************************************************************
	SW JPEG Decode code
	*************************************************************************************/
	ENS_List inputBufferFifo;
	ENS_List inputHeaderFifo;
	OMX_BOOL inputDependencyResolved;
	OMX_BOOL outputDependencyResolved;
	OMX_BOOL headerDependencyResolved;
	OMX_BOOL firstBufferWithBitstream;
	OMX_BOOL bufferUpdateNeeded;
	void ResolveDependencies();

    ts_bitstream_buf_link_and_header *bufferLinkStart;
   //functions for bitstream manaement
	t_uint16 addBufferToBtsLinkList(OMX_BUFFERHEADERTYPE *pBuffer,ts_bitstream_buf_link_and_header *currentBufferLink,t_ahb_address *return_addr);
	t_uint16 removeBufferFromBtsLinkList(ts_bitstream_buf_link_and_header *BufferLinkToRemove);
	t_uint16 readLastBtsBufferfromLinkList(ts_bitstream_buf_link_and_header *currentBufferLink);


	ts_image_buf_link_and_header *imageBufferLinkStart;
	//functions for image buffer management
	t_uint16 addBufferToImageLinkList(OMX_BUFFERHEADERTYPE *pBuffer,ts_image_buf_link_and_header *currentBufferLink);
	t_uint16 removeBufferfromImageLinkList(ts_image_buf_link_and_header *BufferLinkToRemove);
	t_uint16 readFirstImageBufferfromLinkList(ts_image_buf_link_and_header *currentBufferLink);

	typedef struct
	{
		t_uint16 index_to_next_not_required_bitstream_buffer;
		t_uint16 current_bitstream_buffer_index;
		t_uint16 last_index_ps_ddep_bitstream_buf_link_and_header;
		t_ahb_address physical_ps_ddep_bitstream_buf_link_and_header;
		t_uint32 ps_ddep_bitstream_buf_link_and_header;
	} t_ddep_bitstream_mgmt;

	t_ddep_bitstream_mgmt bsm;
	ts_sdc_jpeg_scan_desc sdc_jpeg_scan_desc;

	typedef struct
	{
		ts_t1xhv_vdc_frame_buf_in		s_in_frame_buffer; //addr_in_frame_buffer
		ts_t1xhv_vdc_frame_buf_out      s_out_frame_buffer;//addr_out_frame_buffer
		ts_t1xhv_vdc_internal_buf       s_internal_buffer;//addr_internal_buffer
		ts_t1xhv_bitstream_buf_pos      s_in_out_bitstream_buffer; //addr_in_bitstream_buffer
		ts_t1xhv_bitstream_buf_pos      s_out_bitstream_buffer;//addr_out_frame_buffer
		ts_t1xhv_vdc_jpeg_param_in     	s_in_parameters;    //addr_in_parameters
		ts_t1xhv_vdc_jpeg_param_out    	s_out_parameters;   //addr_out_parameters
		ts_t1xhv_vdc_jpeg_param_inout  	s_in_out_frame_parameters; //addr_in_frame_parameters
		ts_t1xhv_vdc_jpeg_param_inout  	s_out_frame_parameters;//addr_out_grame_parameters
		//ts_ddep_sdc_jpeg_scan_desc      s_ddep_sdc_jpeg_scan_desc;
	} ts_ddep_sdc_jpeg_param_desc;


	ts_ddep_sdc_jpeg_param_desc sdc_jpeg_param_desc;
    OMX_U32 savedFlags;
	OMX_U64 savedTimeStamp;
	t_uint16 current_frame_width;
	t_uint16 current_frame_height;
	//required for cease
	OMX_U32 minOutputBufferSize;
	OMX_BOOL ceaseEnabled;
	//void setParam(void* pComponentConfigStructure);
	//void setConfig(t_uint32 index,void* pComponentParameterStructure);
	virtual void setConfig(t_uint32 nParamIndex,ts_ddep_sdc_jpeg_dynamic_params config);
	void updateParamAndConfigCrop(ts_ddep_sdc_jpeg_dynamic_params *pDynamicConfig);
	void updateParamAndConfigScale(ts_ddep_sdc_jpeg_dynamic_params *pDynamicConfig);
	void updateDynamicConfigurationStructure();
	OMX_ERRORTYPE configureCoeffAndLineBuffer();
	OMX_ERRORTYPE checkConfAndParams();
	void set_pJdcInOut_parameters();
	void set_pJdcOut_parameters();
	virtual void memoryRequirements(t_uint16 mChannelId,ts_ddep_buffer_descriptor paramBufferDesc,
							  ts_ddep_buffer_descriptor linkListBufferDesc,ts_ddep_buffer_descriptor debugBufferDesc,
							  ts_ddep_buffer_descriptor lineBufferDesc);
	virtual void huffMemory(ts_ddep_buffer_descriptor HuffMemoryDesc);

};


#endif /* PARSER_ARM_HPP_ */
