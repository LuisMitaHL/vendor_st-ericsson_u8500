/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_jpegdec_arm_nmf_parser_arm_src_parser_armTraces.h"
#endif



#include "jpegdec/arm_nmf/parser_arm.nmf"
//#include "parser_arm.hpp"
#include <string.h>
#include "SharedBuffer.h"



//#include "mpc/common/buffer.idt"
//#define BTSLNKLST_DBG_PRNT
#define EXT_BIT       1

#define BUFFERFLAG_EOS OMX_BUFFERFLAG_EOS

t_uint16 jpegdec_arm_nmf_parser_arm::addBufferToBtsLinkList(OMX_BUFFERHEADERTYPE *pBuffer,ts_bitstream_buf_link_and_header *currentBufferLink,t_ahb_address *return_addr)
{
	OstTraceFiltInst1(TRACE_API, "PARSER_ARM: Inside addBufferToBtsLinkList for buffer : 0x%x \n",(OMX_U32)pBuffer);
#ifdef BTSLNKLST_DBG_PRNT
printf("\n addBufferToBtsLinkList\n");
#endif
			     if(bufferLinkStart == 0)
				 {
    OstTraceFiltInst0(TRACE_FLOW, "PARSER_ARM: Inside addBufferToBtsLinkList bufferLinkStart == 0");
					ts_bitstream_buf_link_and_header *tmp = new ts_bitstream_buf_link_and_header;
					if(tmp == 0) return 1;

					bufferLinkStart = tmp;
					//bufferLinkStart->bufferLink = (tmp->bufferLink);
					tmp->bufferLink.addr_next_buf_link = 0;
					tmp->bufferLink.addr_prev_buf_link = 0;
					tmp->bufferLink.addr_buffer_start = (t_ahb_address)pBuffer->pBuffer + (t_ahb_address)pBuffer->nOffset;

					// <CAUTION> Important : ALGO requires extra WORD at end for processing so intentionally 64 bytes have been added
					tmp->bufferLink.addr_buffer_end = (t_ahb_address)pBuffer->pBuffer + (t_ahb_address)pBuffer->nOffset + (t_ahb_address)pBuffer->nFilledLen + 64;
					tmp->pBuffer = pBuffer;
                    *currentBufferLink = *tmp;
					*return_addr = (t_ahb_address)&tmp->bufferLink;
#ifdef BTSLNKLST_DBG_PRNT
printf("\n &tmp->bufferLink:0x%X \n tmp->bufferLink.addr_next_buf_link: 0x%X \n tmp->bufferLink.addr_prev_buf_link: 0x%X \n tmp->bufferLink.addr_buffer_start: 0x%X \n tmp->bufferLink.addr_buffer_end: 0x%X",
															&tmp->bufferLink,
															tmp->bufferLink.addr_next_buf_link,
															tmp->bufferLink.addr_prev_buf_link,
															tmp->bufferLink.addr_buffer_start,
															tmp->bufferLink.addr_buffer_end);
#endif

				}
				else
				{
                    OstTraceFiltInst0(TRACE_FLOW, "PARSER_ARM: Inside addBufferToBtsLinkList in else bufferLinkStart == 0");
					ts_bitstream_buf_link_and_header *lastElement = bufferLinkStart;
					ts_bitstream_buf_link_and_header *tmp = new ts_bitstream_buf_link_and_header;
#ifdef BTSLNKLST_DBG_PRNT
printf("\nAdded &tmp->bufferLink:0x%X in to the buffer struct\n",&tmp->bufferLink);
ts_bitstream_buf_link_and_header *tmp2;
#endif

					if(tmp == 0) return 1;

					//Add to the end of list
					while(lastElement->bufferLink.addr_next_buf_link)
					{
                    lastElement = (ts_bitstream_buf_link_and_header *)lastElement->bufferLink.addr_next_buf_link;
                    }

					lastElement->bufferLink.addr_next_buf_link = (t_ahb_address) &(tmp->bufferLink);
					tmp->bufferLink.addr_next_buf_link = (t_ahb_address)0;
					tmp->bufferLink.addr_prev_buf_link = (t_ahb_address)&(lastElement->bufferLink) ;

					tmp->bufferLink.addr_buffer_start = (t_ahb_address)pBuffer->pBuffer + (t_ahb_address)pBuffer->nOffset;
					tmp->bufferLink.addr_buffer_end = (t_ahb_address)pBuffer->pBuffer + (t_ahb_address)pBuffer->nOffset + (t_ahb_address)pBuffer->nFilledLen;
					tmp->pBuffer = pBuffer;
					*currentBufferLink = *tmp;
					*return_addr = (t_ahb_address)&tmp->bufferLink;
#ifdef BTSLNKLST_DBG_PRNT
//> debug prints
lastElement = bufferLinkStart;
                    //> print total (links - 1)
while(lastElement->bufferLink.addr_next_buf_link)
{
	tmp2=lastElement;
	printf("\n &tmp2->bufferLink:0x%X \n tmp2->bufferLink.addr_next_buf_link: 0x%X \n tmp2->bufferLink.addr_prev_buf_link: 0x%X \n tmp2->bufferLink.addr_buffer_start: 0x%X \n tmp2->bufferLink.addr_buffer_end: 0x%X",
																&tmp2->bufferLink,
																tmp2->bufferLink.addr_next_buf_link,
																tmp2->bufferLink.addr_prev_buf_link,
																tmp2->bufferLink.addr_buffer_start,
																tmp2->bufferLink.addr_buffer_end);

	lastElement = (ts_bitstream_buf_link_and_header *)lastElement->bufferLink.addr_next_buf_link;
}
					//print last element
	tmp2=lastElement;
	printf("\n &tmp2->bufferLink:0x%X \n tmp2->bufferLink.addr_next_buf_link: 0x%X \n tmp2->bufferLink.addr_prev_buf_link: 0x%X \n tmp2->bufferLink.addr_buffer_start: 0x%X \n tmp2->bufferLink.addr_buffer_end: 0x%X",
																&tmp2->bufferLink,
																tmp2->bufferLink.addr_next_buf_link,
																tmp2->bufferLink.addr_prev_buf_link,
																tmp2->bufferLink.addr_buffer_start,
																tmp2->bufferLink.addr_buffer_end);
#endif
				}

	return 0;
}

t_uint16 jpegdec_arm_nmf_parser_arm::removeBufferFromBtsLinkList(ts_bitstream_buf_link_and_header *BufferLinkToRemove)
{
	OstTraceFiltInst1(TRACE_API, "PARSER_ARM: Inside removeBufferFromBtsLinkList -%x\n",(OMX_U32)BufferLinkToRemove);
#ifdef BTSLNKLST_DBG_PRNT
printf("\nremoveBufferFromBtsLinkList bufferLinkStart->pBuffer: 0x%X \n",bufferLinkStart->pBuffer);
ts_bitstream_buf_link_and_header *tmp2;
tmp2=bufferLinkStart;
while(tmp2->bufferLink.addr_next_buf_link)
	{
		printf("\n &tmp2->bufferLink:0x%X \n tmp2->bufferLink.addr_next_buf_link: 0x%X \n tmp2->bufferLink.addr_prev_buf_link: 0x%X \n tmp2->bufferLink.addr_buffer_start: 0x%X \n tmp2->bufferLink.addr_buffer_end: 0x%X",
																&tmp2->bufferLink,
																tmp2->bufferLink.addr_next_buf_link,
																tmp2->bufferLink.addr_prev_buf_link,
																tmp2->bufferLink.addr_buffer_start,
																tmp2->bufferLink.addr_buffer_end);

		tmp2 = (ts_bitstream_buf_link_and_header *)tmp2->bufferLink.addr_next_buf_link;
     }
	//print last element
    printf("\n &tmp2->bufferLink:0x%X \n tmp2->bufferLink.addr_next_buf_link: 0x%X \n tmp2->bufferLink.addr_prev_buf_link: 0x%X \n tmp2->bufferLink.addr_buffer_start: 0x%X \n tmp2->bufferLink.addr_buffer_end: 0x%X",
																&tmp2->bufferLink,
																tmp2->bufferLink.addr_next_buf_link,
																tmp2->bufferLink.addr_prev_buf_link,
																tmp2->bufferLink.addr_buffer_start,
																tmp2->bufferLink.addr_buffer_end);
#endif
	//remove the first element of list
	ts_bitstream_buf_link_and_header *element = bufferLinkStart;

	if(bufferLinkStart!=BufferLinkToRemove)
    {
        OstTraceFiltInst0(TRACE_ERROR, "PARSER_ARM: bufferLinkStart!=BufferLinkToRemove \n");
		return 1;
    }

	if(bufferLinkStart->bufferLink.addr_next_buf_link)
	{
		bufferLinkStart = (ts_bitstream_buf_link_and_header*)bufferLinkStart->bufferLink.addr_next_buf_link;
		bufferLinkStart->bufferLink.addr_prev_buf_link = 0;
	}
	else
	{
       //bufferLinkStart = (ts_bitstream_buf_link_and_header*);
		bufferLinkStart->bufferLink.addr_prev_buf_link = 0;
		bufferLinkStart->bufferLink.addr_next_buf_link = 0;
		bufferLinkStart = 0;
	}

	if(bufferLinkStart != 0)
    delete element;

	return 0;
}

t_uint16 jpegdec_arm_nmf_parser_arm::readLastBtsBufferfromLinkList(ts_bitstream_buf_link_and_header *currentBufferLink)
{
	if(bufferLinkStart == 0)
    {
       OstTraceFiltInst0(TRACE_ERROR, "PARSER_ARM: bufferLinkStart == 0 \n");
		return 1;
    }
	else
	{
		ts_bitstream_buf_link_and_header *tmp2;
		tmp2=bufferLinkStart;
		while(tmp2->bufferLink.addr_next_buf_link)
		{
			tmp2 = (ts_bitstream_buf_link_and_header *)tmp2->bufferLink.addr_next_buf_link;
		}


		*currentBufferLink = *tmp2;
	}
	return 0;
}

t_uint16 jpegdec_arm_nmf_parser_arm::addBufferToImageLinkList(OMX_BUFFERHEADERTYPE *pBuffer,ts_image_buf_link_and_header *currentBufferLink)
{
	OstTraceFiltInst1(TRACE_API, "PARSER_ARM: Inside addBufferToImageLinkList for buffer : 0x%x \n",(OMX_U32)pBuffer);
	if(imageBufferLinkStart==0)
	{
        OstTraceFiltInst0(TRACE_FLOW, "PARSER_ARM: Inside addBufferToImageLinkList imageBufferLinkStart==0");
		ts_image_buf_link_and_header *tmp = new ts_image_buf_link_and_header;
		imageBufferLinkStart = tmp;
		//maintian list
		tmp->next = 0;
		tmp->prev = 0;
		tmp->bufferLink.addr_dest_buffer = (t_ahb_address)pBuffer->pBuffer;
		*currentBufferLink = *tmp;
		tmp->pBuffer = pBuffer;
	}
	else
	{
        OstTraceFiltInst0(TRACE_FLOW, "PARSER_ARM: Inside addBufferToImageLinkList else imageBufferLinkStart==0");
		ts_image_buf_link_and_header *tmp = new ts_image_buf_link_and_header;
		ts_image_buf_link_and_header *LastElement = imageBufferLinkStart;
		//go to last element & add new element
		while(LastElement->next != 0 )
			LastElement = LastElement->next;

		LastElement->next = tmp;
		tmp->prev = LastElement;
		tmp->next = 0;
		tmp->bufferLink.addr_dest_buffer = (t_ahb_address)pBuffer->pBuffer;
		*currentBufferLink = *tmp;
		tmp->pBuffer = pBuffer;
	}

	return 0;
}

t_uint16 jpegdec_arm_nmf_parser_arm::removeBufferfromImageLinkList(ts_image_buf_link_and_header *BufferLinkToRemove)
{
	ts_image_buf_link_and_header *element = imageBufferLinkStart;
    OstTraceFiltInst1(TRACE_API, "PARSER_ARM: Inside removeBufferfromImageLinkList for buffer : 0x%x \n",(OMX_U32)BufferLinkToRemove);
	if(BufferLinkToRemove !=imageBufferLinkStart)
    {
      OstTraceFiltInst0(TRACE_ERROR, "PARSER_ARM: BufferLinkToRemove !=imageBufferLinkStart\n");
		return 1;
    }

	if(imageBufferLinkStart->next)
	{
		imageBufferLinkStart = imageBufferLinkStart->next;
		imageBufferLinkStart->prev = 0;
	}
	else
	{
		imageBufferLinkStart = 0;
	}

	delete element;
	return 0;
}

t_uint16 jpegdec_arm_nmf_parser_arm::readFirstImageBufferfromLinkList(ts_image_buf_link_and_header *currentBufferLink)
{
	if(imageBufferLinkStart == 0)
    {
        OstTraceFiltInst0(TRACE_ERROR, "PARSER_ARM: imageBufferLinkStart == 0\n");
		return 1;
    }
	else
	{
		*currentBufferLink = *imageBufferLinkStart;
	}
	return 0;
}

jpegdec_arm_nmf_parser_arm::jpegdec_arm_nmf_parser_arm()
{
	PARSER_WAITING_FOR_BUFFERS  =OMX_FALSE;
	inputDependencyResolved		=OMX_FALSE;
	outputDependencyResolved	=OMX_FALSE;
	headerDependencyResolved	=OMX_FALSE;
	bufferUpdateNeeded			= OMX_TRUE;
	firstBufferWithBitstream    =OMX_TRUE;
	current_frame_width = 0;
	current_frame_height = 0;
	inputBufferFifo.clear();
	inputHeaderFifo.clear();
	bufferLinkStart = 0;
    imageBufferLinkStart = 0;
    ceaseEnabled = OMX_FALSE;
    isOutputBufferDequeue = OMX_FALSE;
    isInputBufferDequeue = OMX_FALSE;
    allocatedBufferSize = 0;
    isEOSTrue = OMX_FALSE;
    processActualExecution = OMX_FALSE;

	armnmf_component_params_set.downsampling_factor = 0;
	armnmf_component_params_set.horizontal_offset = 0;
	armnmf_component_params_set.vertical_offset = 0;
	armnmf_component_params_set.window_height = 0;
	armnmf_component_params_set.window_width = 0;
	bsm.current_bitstream_buffer_index = 0;
	bsm.index_to_next_not_required_bitstream_buffer = 0;
	bsm.last_index_ps_ddep_bitstream_buf_link_and_header = 0;
	bsm.physical_ps_ddep_bitstream_buf_link_and_header = 0;
	bsm.ps_ddep_bitstream_buf_link_and_header = 0;
	portDef1.bBuffersContiguous = OMX_FALSE;
	portDef1.bEnabled = OMX_FALSE;
	portDef1.bPopulated = OMX_FALSE;
	portDef1.eDir = (OMX_DIRTYPE)0;
	portDef1.eDomain = (OMX_PORTDOMAINTYPE)0;
	portDef1.nBufferAlignment = 0;
	portDef1.nBufferCountActual = 0;
	portDef1.nBufferCountMin = 0;
	portDef1.nBufferSize = 0;
	portDef1.nPortIndex = 0;
	portDef1.nSize = 0;

	isPortSetting = OMX_FALSE;

	portDef2.bBuffersContiguous = OMX_FALSE;
	portDef2.bEnabled = OMX_FALSE;
	portDef2.bPopulated = OMX_FALSE;
	portDef2.eDir = (OMX_DIRTYPE)0;
	portDef2.eDomain = (OMX_PORTDOMAINTYPE)0;
	portDef2.nBufferAlignment = 0;
	portDef2.nBufferCountActual = 0;
	portDef2.nBufferCountMin = 0;
	portDef2.nBufferSize = 0;
	portDef2.nPortIndex = 0;
	portDef2.nSize = 0;

	sdc_jpeg_scan_desc.bitOffset = 0;
	sdc_jpeg_scan_desc.buffer_p = 0;
	sdc_jpeg_scan_desc.encodedDataSize = 0;
	sdc_jpeg_scan_desc.headerBufferMpcAddress = 0;
	sdc_jpeg_scan_desc.physicalAddressParamIn = 0;
    sdc_jpeg_scan_desc.isFakeHeader = 0;
    sdc_jpeg_scan_desc.lastHeader = 0;
	last_read_conf_no = 0;
	minOutputBufferSize = 0;
	savedFlags = 0;
	savedTimeStamp = 0;

	downsamplingFactor = 0;
	colourMode = 0;

	windowWidth = 0;
	windowHeight = 0;

	subSamplingType = 0;/* default if YUV 420 */
	bufferSizeReq = 0;

	mParamBufferDesc.nSize = 0;
	mParamBufferDesc.nMpcAddress = 0;
	mParamBufferDesc.nLogicalAddress = 0;
	mParamBufferDesc.nPhysicalAddress = 0;

	ps_ddep_sdc_jpeg_dynamic_configuration = 0;

	mLineBufferDesc.nLogicalAddress = 0;
	mLineBufferDesc.nMpcAddress  = 0;
	mLineBufferDesc.nPhysicalAddress  = 0;
	mLineBufferDesc.nSize  = 0;

	formatType = OMX_COLOR_FormatYUV420Planar; // YUV 420

	mHuffMemDesc.nLogicalAddress = 0;
	mHuffMemDesc.nMpcAddress = 0;
	mHuffMemDesc.nPhysicalAddress = 0;
	mHuffMemDesc.nSize = 0;

	memset(&sdc_jpeg_param_desc,0,sizeof(sdc_jpeg_param_desc)); //Fixme

//	OstTraceFiltInst0(TRACE_API, "PARSER_ARM: Inside constructor for parser_arm \n");
}

jpegdec_arm_nmf_parser_arm::~jpegdec_arm_nmf_parser_arm()
{

}

t_nmf_error METH(construct)(void)
{
#ifdef DBG_PRINT
	printf("construct called\n");
#endif

	mParser.initializeReferences(this,&mParam);
	return NMF_OK;
}

void METH(destroy)()
{

}

void METH(fsmInit)(fsmInit_t initFSM)
{
    t_uint16 portsDisabled = initFSM.portsDisabled;
    t_uint16 portsTunneled = initFSM.portsTunneled;
    OstTraceInt0(TRACE_API, "PARSER_ARM: Inside fsmInit  \n");
    if (initFSM.traceInfoAddr)
    {
        setTraceInfo(initFSM.traceInfoAddr, initFSM.id1);
	}

   /* OMX_BUFFERHEADERTYPE * mBufIn_list[4] ;
	OMX_BUFFERHEADERTYPE * mBufOut_list[4];

    for(int i=0;i<4;i++)
	{
		mBufIn_list[i] = &mBufIn[i] ;
		mBufOut_list[i] = &mBufOut[i] ;
	}*/

	//Input port can keep buffers till complete data is not available
	mPorts[0].init(InputPort, /* port direction */
								 false, /* isBufferSupplier */
								 true, /* isHWPort */
								 0, /* port we're sharing with */
								 0, /* initialBuffers ptrFIXME: mBufIn_list*/
								 4, /* bufferCount */
								 &inputport, /* return interface */
								 0, /* portIdx */
								 portsDisabled&1, /* isDisabled */
								 portsTunneled&1,
								 (Component *)this /* componentOwner */
								 );

	mPorts[1].init(OutputPort, /* port direction */
								 false, /* isBufferSupplier */
								 true, /* isHWPort */
								 0, /* port we're sharing with */
								 0, /* initialBuffers ptr FIXME: mBufOut_list*/
								 4, /* bufferCount */
								 &outputport, /* return interface */
								 1, /* portIdx */
								 (portsDisabled >> 1)&1, /* isDisabled */
								 (portsTunneled>>1)&1,
								 (Component *)this /* componentOwner */
								 );
	//mPorts[0].init(InputPort,  false, true, NULL, mBufIn_list, 4, &inputport, 0, portsDisabled &1, this);
    //mPorts[1].init(OutputPort, false, true, NULL, mBufOut_list, 4, &outputport, 1, (portsDisabled >> 1)&1, this);

	//mPorts[2].init(OutputPort, false, true, 0, 0, mMaxOutput, &headerPort, 2, portsDisabled, this);
    init(2, mPorts, &proxy, &me, 0); //FIXME:invalid parameter is to be initilaized properly.


}

void METH(disablePortIndication)(t_uint32 portIdx)
{
#ifdef DBG_PRINT
	printf("disablePortIndication\n");
#endif
}
void METH(enablePortIndication)(t_uint32 portIdx)
{
#ifdef DBG_PRINT
	printf("enablePortIndication\n");
#endif
}
void METH(flushPortIndication)(t_uint32 portIdx)
{
#ifdef DBG_PRINT
	printf("flushPortIndication\n");
#endif
}
void METH(reset)()
{
#ifdef DBG_PRINT
	printf("reset\n");
#endif
}


void METH(setTunnelStatus)(t_sint16 portIdx, t_uint16 isTunneled)
{
        #ifdef DBG_PRINT
        	printf("setTunnelStatus\n");
        #endif
    OstTraceFiltInst0(TRACE_API, "PARSER_ARM: setTunnelStatus\n ");
    if (portIdx == -1)
    {
        for (int i=0; i<2; i++)
        {
            if (isTunneled & (1<<i))
            {
                mPorts[i].setTunnelStatus(true);
            }
            else
            {
                mPorts[i].setTunnelStatus(false);
            }
        }
    }
    else
    {
        if (isTunneled & (1<<portIdx))
        {
            mPorts[portIdx].setTunnelStatus(true);
        }
        else
        {
            mPorts[portIdx].setTunnelStatus(false);
        }
    }
}



void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword parameter)
{
    #ifdef DBG_PRINT
        printf("sendCommand received\n");
    #endif
    OstTraceFiltInst2(TRACE_API, "PARSER_ARM: sendCommand cmd -%d parameter -%d\n ",cmd,parameter);
	switch (cmd)
	{
		case OMX_CommandStateSet:
		{
			switch (parameter)
			{
				case OMX_StatePause:
				case OMX_StateExecuting: break;
				case OMX_StateIdle:
						if(!processActualExecution)
						{
                            OstTraceFiltInst0(TRACE_FLOW, "PARSER_ARM: sendComand !processActualExecution\n ");
							mParam.initializeParamAndConfig();
							mParser.parserReset();
							mParam.completeImageProcessed=OMX_FALSE;
							return_InputBuffers();
							inputDependencyResolved		=OMX_FALSE;
							headerDependencyResolved	=OMX_FALSE;
							return_OutputBuffers(0);
							outputDependencyResolved	=OMX_FALSE;
						}
						break;
			}
		}
		break;

		case OMX_CommandFlush:
		case OMX_CommandPortDisable:
		{
			switch (parameter)
			{
				case 0:
				{
					mParam.initializeParamAndConfig();
					mParser.parserReset();
					mParam.completeImageProcessed=OMX_FALSE;
				}
				return_InputBuffers();
				inputDependencyResolved		=OMX_FALSE;
				headerDependencyResolved	=OMX_FALSE;
				break;

				case 1:
						return_OutputBuffers(0);
						outputDependencyResolved	=OMX_FALSE;

					break;
			}
		}
		break;

		case OMX_CommandPortEnable:
			break;
		default:
			//To remove warning in linux
			break;

	}

	Component::sendCommand(cmd, parameter) ;
}

void METH(process)()
{
    OstTraceFiltInst0(TRACE_API, "PARSER_ARM: Inside process\n");
    ResolveDependencies();
	t_uint16 result = processBuffer(0,0);
	if(result)
	{
		//FIX ME :send error to proxy saying NOT supported
		while (1);
	}

}


t_uint16 jpegdec_arm_nmf_parser_arm:: processBuffer(OMX_BUFFERHEADERTYPE* ptrOmxBuffer, ts_sdc_jpeg_scan_desc *ptrParamInHeaderBuffer)
{
	t_djpegErrorCode btstState;
    OstTraceFiltInst0(TRACE_API, "PARSER_ARM: Inside processBuffer\n");
	if(mPorts[0].queuedBufferCount())
	{
		OstTraceFiltInst0(TRACE_API, "PARSER_ARM: Inside processBuffer and started Processing \n");
		if(mParam.completeImageProcessed==OMX_TRUE)
		{
		   // processingInit();
		   mParam.initializeParamAndConfig();
		   //mParser.btparInit();
		   mParam.completeImageProcessed=OMX_FALSE;
		}
		btstState = mParser.jpegParser();
		switch(btstState)
		{
		case DJPEG_END_OF_DATA:
			PARSER_WAITING_FOR_BUFFERS=OMX_TRUE;
			break;
		case DJPEG_UNSUPPORTED_MODE:
			proxy.eventHandler(OMX_EventError,(OMX_U32)OMX_ErrorStreamCorrupt,0);
            OstTraceFiltInst0(TRACE_ERROR, "PARSER_ARM: ProcessBuffer Unsupported Mode \n");
			//return_InputBuffers();
			inputDependencyResolved		=OMX_FALSE;

            break;


		case DJPEG_NO_ERROR:
			PARSER_WAITING_FOR_BUFFERS=OMX_FALSE;
			break;
			default:
#ifdef DBG_PRINT
                printf("\n Error in Parsing Error Code: %d\n",btstState);
				printf("Non fatal error: Ignoring Error Code: %d\n",btstState);
#endif
				break;
		}
	}
	return 0;
}

void jpegdec_arm_nmf_parser_arm::set_pJdcIn_parameters(ts_t1xhv_vdc_jpeg_param_in *pJdcIn_parameters)
{
    OstTraceFiltInst0(TRACE_API, "PARSER_ARM: Inside set_pJdcIn_parameters\n");
	OMX_U8 cnt;
    {
		//Zero memory
		t_uint32 count;
        for (count = 0; count<sizeof(ts_t1xhv_vdc_jpeg_param_in);count++)
			((t_uint8*)pJdcIn_parameters)[count]=0;
    }
	pJdcIn_parameters->frame_width = mParam.frameWidth;
	pJdcIn_parameters->frame_height = mParam.frameHeight;
    pJdcIn_parameters->window_width = mParam.frameWidth;
	pJdcIn_parameters->window_height= mParam.frameHeight;
	pJdcIn_parameters->window_horizontal_offset    = 0;		// x is in width
	pJdcIn_parameters->window_vertical_offset	   =0;


	if(mParam.isCroppingEnabled)
	{
		pJdcIn_parameters->window_width = mParam.cropWindowWidth;
		pJdcIn_parameters->window_height= mParam.cropWindowHeight;

	}
	else
	{
		pJdcIn_parameters->window_width = mParam.frameWidth;
		pJdcIn_parameters->window_height= mParam.frameHeight;
	}

	windowWidth = pJdcIn_parameters->window_width;
	windowHeight = pJdcIn_parameters->window_height;

	pJdcIn_parameters->window_horizontal_offset    = mParam.horizontal_offset;
	pJdcIn_parameters->window_vertical_offset	   = mParam.vertical_offset;
    pJdcIn_parameters->downsampling_factor = mParam.downsamplingFactor;

	// Data Dependency considers validity of Dynamic configuration in all cases(whether setconfig or not)
	// So if there is no or one time setconfig call is there following code takes care of that.

	//if(m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set_atomic_exit==0){
	if(!mParam.isCroppingEnabled)
	{
		ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.window_width = mParam.frameWidth;
		ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.window_height =mParam.frameHeight;

		ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.vertical_offset = 0;
		ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.horizontal_offset =0;
	}

	if(!mParam.isDownScalingEnabled)
	{
		ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.downsampling_factor = mParam.downsamplingFactor;
	}


	/*if(jpegdec->mParam.isCroppingEnabled)
	{
		pJdcIn_parameters->window_width = (mParam.cropWindowWidth+ 0xf)&0xfff0;
		pJdcIn_parameters->window_height= (mParam.cropWindowHeight + 0xf)&0xfff0;
	}
	else
	{
		pJdcIn_parameters->window_width = (mParam.frameWidth+0xf)& 0xfff0;
		pJdcIn_parameters->window_height= (mParam.frameHeight+0xf) &0xfff0;
	}*/

	//pJdcIn_parameters->window_horizontal_offset    = 0;		// x is in width
	//pJdcIn_parameters->window_vertical_offset	   =0;
	pJdcIn_parameters->nb_components = 		mParam.nbComponents;

	for(cnt =0; cnt<MAX_NUMBER_COMPONENTS;cnt++)
	{
		if(mParam.components[cnt].componentId == mParam.IdY)
		{
			OMX_U8 DChuffmanDestSel= mParam.components[cnt].DCHuffmanTableDestinationSelector;
			OMX_U8 AChuffmanDestSel= mParam.components[cnt].ACHuffmanTableDestinationSelector;
			OMX_U8 quantDestSel= mParam.components[cnt].quantizationTableDestinationSelector;

			pJdcIn_parameters->h_sampling_factor_y = mParam.samplingFactors[cnt].hSamplingFactor;
			pJdcIn_parameters->v_sampling_factor_y = mParam.samplingFactors[cnt].vSamplingFactor;

			pJdcIn_parameters->component_selector_y = mParam.components[cnt].componentSelector;
                /////DC HUFFMAN TABLES
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_y_dc_table.huffman_bits_dc,
				   mParam.DCHuffmanTable[DChuffmanDestSel].DCHuffmanBits,
				   HUFF_BITS_TABLE_SIZE*sizeof(OMX_U16));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_y_dc_table.huffman_val_dc,
				   mParam.DCHuffmanTable[DChuffmanDestSel].DCHuffmanVal,
				   DC_HUFFVAL_TABLE_SIZE*sizeof(OMX_U16));
                ////New Members
             memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_y_dc_table.maxcode,
				   mParam.DCHuffmanTable[DChuffmanDestSel].MaxCode,
				   18*sizeof(OMX_S32));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_y_dc_table.valoffset,
				   mParam.DCHuffmanTable[DChuffmanDestSel].ValOffset,
				   17*sizeof(OMX_S32));
             memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_y_dc_table.look_nbits,
				   mParam.DCHuffmanTable[DChuffmanDestSel].Look_Nbits,
				   256*sizeof(OMX_S16));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_y_dc_table.look_sym,
				   mParam.DCHuffmanTable[DChuffmanDestSel].Look_Sym,
				   256*sizeof(OMX_U16));
                /////AC HUFFMAN TABLES
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_y_ac_table.huffman_bits_ac,
				   mParam.ACHuffmanTable[AChuffmanDestSel].ACHuffmanBits,
				   HUFF_BITS_TABLE_SIZE*sizeof(OMX_U16));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_y_ac_table.huffman_val_ac,
				   mParam.ACHuffmanTable[AChuffmanDestSel].ACHuffmanVal,
				   AC_HUFFVAL_TABLE_SIZE*sizeof(OMX_U16));
                /////New Members
             memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_y_ac_table.maxcode,
				   mParam.ACHuffmanTable[AChuffmanDestSel].MaxCode,
				   18*sizeof(OMX_S32));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_y_ac_table.valoffset,
				   mParam.ACHuffmanTable[AChuffmanDestSel].ValOffset,
				   17*sizeof(OMX_S32));
             memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_y_ac_table.look_nbits,
				   mParam.ACHuffmanTable[AChuffmanDestSel].Look_Nbits,
				   256*sizeof(OMX_S16));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_y_ac_table.look_sym,
				   mParam.ACHuffmanTable[AChuffmanDestSel].Look_Sym,
				   256*sizeof(OMX_U16));
                /////QUANTIZATION TABLE
			 memcpy(pJdcIn_parameters->quant_y,
				   mParam.quantizationTables[quantDestSel].QuantizationTable,
				   QUANT_TABLE_SIZE*sizeof(OMX_U16));

		}

		if(mParam.components[cnt].componentId == mParam.IdCb)
		{
			OMX_U8 DChuffmanDestSel= mParam.components[cnt].DCHuffmanTableDestinationSelector;
			OMX_U8 AChuffmanDestSel= mParam.components[cnt].ACHuffmanTableDestinationSelector;
			OMX_U8 quantDestSel= mParam.components[cnt].quantizationTableDestinationSelector;

			pJdcIn_parameters->h_sampling_factor_cb = mParam.samplingFactors[cnt].hSamplingFactor;
			pJdcIn_parameters->v_sampling_factor_cb = mParam.samplingFactors[cnt].vSamplingFactor;

			pJdcIn_parameters->component_selector_cb = mParam.components[cnt].componentSelector;

			   /////DC HUFFMAN TABLES
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cb_dc_table.huffman_bits_dc,
				   mParam.DCHuffmanTable[DChuffmanDestSel].DCHuffmanBits,
				   HUFF_BITS_TABLE_SIZE*sizeof(OMX_U16));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cb_dc_table.huffman_val_dc,
				   mParam.DCHuffmanTable[DChuffmanDestSel].DCHuffmanVal,
				   DC_HUFFVAL_TABLE_SIZE*sizeof(OMX_U16));
             ////New Members
             memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cb_dc_table.maxcode,
				   mParam.DCHuffmanTable[DChuffmanDestSel].MaxCode,
				   18*sizeof(OMX_S32));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cb_dc_table.valoffset,
				   mParam.DCHuffmanTable[DChuffmanDestSel].ValOffset,
				   17*sizeof(OMX_S32));
             memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cb_dc_table.look_nbits,
				   mParam.DCHuffmanTable[DChuffmanDestSel].Look_Nbits,
				   256*sizeof(OMX_S16));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cb_dc_table.look_sym,
				   mParam.DCHuffmanTable[DChuffmanDestSel].Look_Sym,
				   256*sizeof(OMX_U16));
                /////AC HUFFMAN TABLES
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cb_ac_table.huffman_bits_ac,
				   mParam.ACHuffmanTable[AChuffmanDestSel].ACHuffmanBits,
				   HUFF_BITS_TABLE_SIZE*sizeof(OMX_U16));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cb_ac_table.huffman_val_ac,
				   mParam.ACHuffmanTable[AChuffmanDestSel].ACHuffmanVal,
				   AC_HUFFVAL_TABLE_SIZE*sizeof(OMX_U16));
             //////New Members
             memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cb_ac_table.maxcode,
				   mParam.ACHuffmanTable[AChuffmanDestSel].MaxCode,
				   18*sizeof(OMX_S32));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cb_ac_table.valoffset,
				   mParam.ACHuffmanTable[AChuffmanDestSel].ValOffset,
				   17*sizeof(OMX_S32));
             memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cb_ac_table.look_nbits,
				   mParam.ACHuffmanTable[AChuffmanDestSel].Look_Nbits,
				   256*sizeof(OMX_S16));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cb_ac_table.look_sym,
				   mParam.ACHuffmanTable[AChuffmanDestSel].Look_Sym,
				   256*sizeof(OMX_U16));
             /////QUANTIZATION TABLE
			 memcpy(pJdcIn_parameters->quant_cb,
				   mParam.quantizationTables[quantDestSel].QuantizationTable,
				   QUANT_TABLE_SIZE*sizeof(OMX_U16));

		}

		if(mParam.components[cnt].componentId == mParam.IdCr)
		{
			OMX_U8 DChuffmanDestSel= mParam.components[cnt].DCHuffmanTableDestinationSelector;
			OMX_U8 AChuffmanDestSel= mParam.components[cnt].ACHuffmanTableDestinationSelector;
			OMX_U8 quantDestSel= mParam.components[cnt].quantizationTableDestinationSelector;

			pJdcIn_parameters->h_sampling_factor_cr = mParam.samplingFactors[cnt].hSamplingFactor;
			pJdcIn_parameters->v_sampling_factor_cr = mParam.samplingFactors[cnt].vSamplingFactor;

			pJdcIn_parameters->component_selector_cr = mParam.components[cnt].componentSelector;

			   /////DC HUFFMAN TABLES
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cr_dc_table.huffman_bits_dc,
				   mParam.DCHuffmanTable[DChuffmanDestSel].DCHuffmanBits,
				   HUFF_BITS_TABLE_SIZE*sizeof(OMX_U16));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cr_dc_table.huffman_val_dc,
				   mParam.DCHuffmanTable[DChuffmanDestSel].DCHuffmanVal,
				   DC_HUFFVAL_TABLE_SIZE*sizeof(OMX_U16));
             ////New Members
             memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cr_dc_table.maxcode,
				   mParam.DCHuffmanTable[DChuffmanDestSel].MaxCode,
				   18*sizeof(OMX_S32));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cr_dc_table.valoffset,
				   mParam.DCHuffmanTable[DChuffmanDestSel].ValOffset,
				   17*sizeof(OMX_S32));
             memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cr_dc_table.look_nbits,
				   mParam.DCHuffmanTable[DChuffmanDestSel].Look_Nbits,
				   256*sizeof(OMX_S16));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cr_dc_table.look_sym,
				   mParam.DCHuffmanTable[DChuffmanDestSel].Look_Sym,
				   256*sizeof(OMX_U16));
                /////AC HUFFMAN TABLES
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cr_ac_table.huffman_bits_ac,
				   mParam.ACHuffmanTable[AChuffmanDestSel].ACHuffmanBits,
				   HUFF_BITS_TABLE_SIZE*sizeof(OMX_U16));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cr_ac_table.huffman_val_ac,
				   mParam.ACHuffmanTable[AChuffmanDestSel].ACHuffmanVal,
				   AC_HUFFVAL_TABLE_SIZE*sizeof(OMX_U16));
             //////New Members
             memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cr_ac_table.maxcode,
				   mParam.ACHuffmanTable[AChuffmanDestSel].MaxCode,
				   18*sizeof(OMX_S32));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cr_ac_table.valoffset,
				   mParam.ACHuffmanTable[AChuffmanDestSel].ValOffset,
				   17*sizeof(OMX_S32));
             memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cr_ac_table.look_nbits,
				   mParam.ACHuffmanTable[AChuffmanDestSel].Look_Nbits,
				   256*sizeof(OMX_S16));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cr_ac_table.look_sym,
				   mParam.ACHuffmanTable[AChuffmanDestSel].Look_Sym,
				   256*sizeof(OMX_U16));
             //////QUANTIZATION TABLE
			 memcpy(pJdcIn_parameters->quant_cr,
				   mParam.quantizationTables[quantDestSel].QuantizationTable,
				   QUANT_TABLE_SIZE*sizeof(OMX_U16));

		}
	}
	pJdcIn_parameters->downsampling_factor = mParam.downsamplingFactor;
	pJdcIn_parameters->ace_enable = 0;
    pJdcIn_parameters->ace_strength = 0;
	pJdcIn_parameters->nb_scan_components = mParam.nbScanComponents;
	pJdcIn_parameters->progressive_mode =  mParam.mode;
	pJdcIn_parameters->successive_approx_position = mParam.successiveApproxPosition;
	pJdcIn_parameters->start_spectral_selection = mParam.startSpectralSelection;
	pJdcIn_parameters->end_spectral_selection = mParam.endSpectralSelection;
	pJdcIn_parameters->restart_interval = mParam.restartInterval;
    ///////////Assigning the memory address of the software huffman table
    pJdcIn_parameters->huff_table.sw_huff_table = ((mHuffMemDesc.nLogicalAddress));
   //pJdcIn_parameters->huff_table.sw_huff_table.msb = (t_uint16)((mHuffMemDesc.nLogicalAddress) & 0xFFFF0000 );

	downsamplingFactor = mParam.downsamplingFactor;

	OstTraceFiltInst1(TRACE_FLOW, "PARSER_ARM: Inside set_pJdcIn_parameters,  downsamplingFactor : %d \n",downsamplingFactor);

	colourMode = mParam.nbComponents;


		//code segment for setting the values of color mode
		{
				//convert cropping and downsampling
				t_uint32 mb_width = 0;
				t_uint32 mb_height = 0;
				t_uint16 blocks_per_line =0,blocks_per_col=0;


				blocks_per_line=windowWidth/8;
				if((windowWidth%8)!=0) blocks_per_line++;

				blocks_per_col=windowHeight/8;
				if((windowHeight%8)!=0) blocks_per_col++;

				mb_width = (blocks_per_line*(8/downsamplingFactor)) / 16;
				if  ((blocks_per_line*(8/downsamplingFactor)) % 16 != 0) mb_width ++;

				mb_height = (blocks_per_col*(8/downsamplingFactor)) / 16;
				if  ((blocks_per_col*(8/downsamplingFactor)) % 16 != 0) mb_height ++;

				if(colourMode==1)
				{
					OstTraceFiltInst0(TRACE_FLOW, "PARSER_ARM: Monochrome Mode \n");
					#ifdef DBG_PRINT
						printf("\nMonochrome Mode.");
					#endif
					bufferSizeReq = (mb_width*16*mb_height*16*1);
				}
				else if(colourMode==3)
				{
					if (subSamplingType == 0)
					{
						OstTraceFiltInst0(TRACE_FLOW, "PARSER_ARM: Color Mode and YUV 420. \n");
						#ifdef DBG_PRINT
							printf("\nColor Mode and YUV 420.");
						#endif
						bufferSizeReq = ((mb_width*16*mb_height*16*3)/2);
					}
					if (subSamplingType == 1)
					{
						OstTraceFiltInst0(TRACE_FLOW, "PARSER_ARM: Color Mode and YUV 422 \n");
						#ifdef DBG_PRINT
							printf("\nColor Mode and YUV 422.");
						#endif
						bufferSizeReq = ((mb_width*16*mb_height*16*2));
					}
					if (subSamplingType == 2)
					{
						OstTraceFiltInst0(TRACE_FLOW, "PARSER_ARM: Color Mode and YUV 444 \n");
						#ifdef DBG_PRINT
							printf("\nColor Mode and YUV 444.");
						#endif
						bufferSizeReq = ((mb_width*16*mb_height*16*3));
					}

					OstTraceFiltInst1(TRACE_FLOW, "PARSER_ARM: bufferSizeReq : %d \n",bufferSizeReq);


				}
				else
				{
					#ifdef DBG_PRINT
						printf("\nErratic Color Mode.");
					#endif
				}
		}



}

void jpegdec_arm_nmf_parser_arm::setScanParams(ts_sdc_jpeg_scan_desc *pJdcScanParams)
{
    OstTraceFiltInst0(TRACE_API, "PARSER_ARM: setScanParams\n ");
   pJdcScanParams->bitOffset = ENDIANESS_CONVERSION(mParam.scan.bitstreamOffset);
   pJdcScanParams->encodedDataSize = ENDIANESS_CONVERSION(mParam.scan.encodedDataSize);
}

t_uint16 jpegdec_arm_nmf_parser_arm::download_parameters()
{
	t_uint16 retry = 0;
    t_uint16 l_atomic_current_conf;
	ts_ddep_sdc_jpeg_dynamic_params tmpDynamicConfiguration;
	ts_ddep_sdc_jpeg_dynamic_params *ps_ddep_sdc_jpeg_dynamic_params_tmp_set;
	ts_ddep_sdc_jpeg_dynamic_params *ps_ddep_sdc_jpeg_dynamic_params_armnmf_set;
	ts_ddep_sdc_jpeg_dynamic_params *ps_ddep_sdc_jpeg_dynamic_params_host_set;

	ps_ddep_sdc_jpeg_dynamic_params_tmp_set =&tmpDynamicConfiguration;
	ps_ddep_sdc_jpeg_dynamic_params_armnmf_set=&armnmf_component_params_set;
	ps_ddep_sdc_jpeg_dynamic_params_host_set=&(ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set);

	printf("\ndownload_parameters enter");
	OstTraceFiltInst0(TRACE_API, "PARSER_ARM: download_parameters enter \n");

	//if (!set_param_done) return 0;

	l_atomic_current_conf = ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set_atomic_exit;

	if (l_atomic_current_conf == last_read_conf_no) //already using latest configuration
		return 1;

	while(1)
	{
		t_uint16  *src;
		t_uint16  *dest;
		t_uint16 count;
		t_uint16 atomic_entry_exit;

        retry++;
		if (retry > 10) {/*raise error event here*/ //printf("\n Returned here.");
		return 0;}

		src = (t_uint16 *)ps_ddep_sdc_jpeg_dynamic_params_host_set;
		dest = (t_uint16 *)ps_ddep_sdc_jpeg_dynamic_params_tmp_set;
		count = sizeof(ts_ddep_sdc_jpeg_dynamic_params);
		atomic_entry_exit = ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set_atomic_exit;
		while(count--) *dest++=*src++;
		if (atomic_entry_exit == ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set_atomic_entry)
		{
			//atomically copied from host to tmp, now copy it to main mpc_param_set
			src = (t_uint16 *)ps_ddep_sdc_jpeg_dynamic_params_tmp_set;
			dest= (t_uint16 *)ps_ddep_sdc_jpeg_dynamic_params_armnmf_set;
			count = sizeof(ts_ddep_sdc_jpeg_dynamic_params);
			while(count--) *dest++=*src++;
			/*while(count--)
			{
                if (*dest != *src)
				{
					printf("count=%d, %X %X", count, *dest, *src);
					break;
				}
				dest++;
				src++;
			}*/
			last_read_conf_no = atomic_entry_exit;
			//printf("\nnew setting captured in %d attempts", retry);
			break;
		}
			// else retry
	}
	//printf("\ndownload_parameters exit");
	return 1;
}

OMX_BUFFERHEADERTYPE* jpegdec_arm_nmf_parser_arm::provideBitstreamBuffer()
{
	OMX_BUFFERHEADERTYPE *pBuffer;
    OstTraceFiltInst0(TRACE_API, "PARSER_ARM: Inside provideBitstreamBuffer \n");
	while(mPorts[0].queuedBufferCount())//&&mPorts[1].queuedBufferCount())
	{
		pBuffer = mPorts[0].dequeueBuffer();
		isInputBufferDequeue = OMX_TRUE;
#ifdef DBG_PRINT
		printf("Taking Input buffer \n");
#endif
		if(!pBuffer->nFilledLen)
		{
			OstTraceFiltInst0(TRACE_FLOW, "PARSER_ARM: Case for input buffer with size ZERO \n");
			//mPorts[0].returnBuffer(pBuffer);
            inputBufferFifo.pushBack((void*)pBuffer);
           // emptythisbufferSharedBufIn.emptyThisBuffer(pBuffer); 
			//isInputBufferDequeue = OMX_FALSE;
            sdc_jpeg_scan_desc.buffer_p= 0; //setting to NULL
				//Temporarily set to configuration for a single frame
				sdc_jpeg_scan_desc.lastHeader=1;
				sdc_jpeg_scan_desc.isFakeHeader=1;
				//emptythisheader.emptyThisHeader((Buffer_p)(headerBuf->headerBufferMpcAddress)); 
                inputHeaderFifo.pushBack((void*)&sdc_jpeg_scan_desc);
                ResolveDependencies();
#ifdef DBG_PRINT
			printf("Returning Input buffer \n");
#endif
		}
		else
		{
			OstTraceFiltInst2(TRACE_FLOW, "PARSER_ARM: Propoer buffer : 0x%x with Size : %d \n",(OMX_U32)pBuffer,pBuffer->nFilledLen);
			return pBuffer;
		}
	}

   return NULL;
}


void jpegdec_arm_nmf_parser_arm::set_pJdcInOut_parameters()
{
	t_uint8 *ptr;
	ts_t1xhv_vdc_jpeg_param_inout *pJdcInOut_parameters;
	ts_ddep_sdc_jpeg_param_desc *ps_ddep_sdc_jpeg_param_desc;
    OstTraceFiltInst0(TRACE_API, "PARSER_ARM: set_pJdcInOut_parameters()\n ");
	ptr = (t_uint8*)mParamBufferDesc.nLogicalAddress;
	ps_ddep_sdc_jpeg_param_desc = (ts_ddep_sdc_jpeg_param_desc *) ptr;
	pJdcInOut_parameters = &(ps_ddep_sdc_jpeg_param_desc->s_in_out_frame_parameters);

	pJdcInOut_parameters->mcu_index = 0;
	pJdcInOut_parameters->end_of_band_run = 0;
	pJdcInOut_parameters->dc_predictor_y = 0;
	pJdcInOut_parameters->dc_predictor_cb = 0;
	pJdcInOut_parameters->dc_predictor_cr = 0;
	pJdcInOut_parameters->ace_count0 = 0;
	pJdcInOut_parameters->ace_count1 = 0;
	pJdcInOut_parameters->ace_count2 = 0;
	pJdcInOut_parameters->ace_count3 = 0;
	pJdcInOut_parameters->crop_mcu_index = 0;
	pJdcInOut_parameters->crop_mcu_index_in_row = 0;
	pJdcInOut_parameters->slice_pos = 0;
	pJdcInOut_parameters->mcu_index_rst = 0;
	pJdcInOut_parameters->rst_value = 0;


}



void jpegdec_arm_nmf_parser_arm::set_pJdcOut_parameters()
{
	t_uint8 *ptr;
	ts_t1xhv_vdc_jpeg_param_out *pJdcOut_parameters;
	ts_ddep_sdc_jpeg_param_desc *ps_ddep_sdc_jpeg_param_desc;
    OstTraceFiltInst0(TRACE_API, "PARSER_ARM: set_pJdcOut_parameters()\n ");
	ptr = (t_uint8*)mParamBufferDesc.nLogicalAddress;
	ps_ddep_sdc_jpeg_param_desc = (ts_ddep_sdc_jpeg_param_desc *) ptr;
	pJdcOut_parameters = &(ps_ddep_sdc_jpeg_param_desc->s_out_parameters);

	pJdcOut_parameters->error_type = 0;
	pJdcOut_parameters->ace_offset0 = 0;
	pJdcOut_parameters->ace_offset1 = 0;
	pJdcOut_parameters->ace_offset2 = 0;
	pJdcOut_parameters->ace_offset3 = 0;
}


t_djpegErrorCode jpegdec_arm_nmf_parser_arm::fillHeader(OMX_BUFFERHEADERTYPE *pBuffer)
{
    OstTraceFiltInst0(TRACE_API, "PARSER_ARM: fillHeader()\n ");
	OMX_ERRORTYPE error = OMX_ErrorUndefined;
	firstBufferWithBitstream = OMX_TRUE;
	if (!mParam.updateData)
	{
        OstTraceFiltInst0(TRACE_FLOW, "PARSER_ARM: fillHeader() mParam.updateData\n ");
		sdc_jpeg_scan_desc.buffer_p= (t_uint32)pBuffer; //fixme

		error = checkConfAndParams();
		if(error)
		{
			//freeHeaderDesc((t_uint32)&sdc_jpeg_scan_desc);
            OstTraceFiltInst0(TRACE_ERROR, "PARSER_ARM: fillHeader() error sending EventError\n ");
			proxy.eventHandler(OMX_EventError,(OMX_U32)error,0);
		}
		error = configureCoeffAndLineBuffer();
		if(error)
		{
				//freeHeaderDesc((t_uint32)&sdc_jpeg_scan_desc);
            OstTraceFiltInst0(TRACE_ERROR, "PARSER_ARM: fillHeader() error sending EventError\n ");
				proxy.eventHandler(OMX_EventError,(OMX_U32)error,0);
		}
		set_pJdcInOut_parameters();
		set_pJdcOut_parameters();
		set_pJdcIn_parameters(&(sdc_jpeg_param_desc.s_in_parameters));
		setScanParams(&sdc_jpeg_scan_desc);
		inputHeaderFifo.pushBack((void*)&sdc_jpeg_scan_desc);
		//change for buffer payload
		//inputBufferFifo.pushBack((void*)pBuffer);
		mParam.updateData = 1;

	}
	//Check for next dependency resolution
	ResolveDependencies();

	return DJPEG_NO_ERROR;
}




void jpegdec_arm_nmf_parser_arm::releaseBtstBuffer(OMX_U8 btstContent,OMX_BUFFERHEADERTYPE *pBuffer)
{
	//inputBufferFifo.pushBack((void*)pBuffer); //fixme

	if(btstContent == 0x00 || btstContent == 0x01 )//i.e. either header or marker no bitstream
    {
		if(isDispatching())
		{
			mPorts[0].returnBuffer(pBuffer);
		}
		else
		{
			//printf("return from Asyn - input \n");
			Component::returnBufferAsync(0,pBuffer);
		}
		isInputBufferDequeue = OMX_FALSE;
        OstTraceFiltInst1(TRACE_FLOW, "PARSER_ARM: releaseBtstBuffer header present pBuffer -%x\n ",(OMX_U32)pBuffer);
//		mPorts[0].returnBuffer(pBuffer);		//Pure Header,No Bitstream
#ifdef DBG_PRINT
		printf("Returning Input buffer \n");
#endif
	}
	else
	{
        OstTraceFiltInst1(TRACE_FLOW, "PARSER_ARM: releaseBtstBuffer bitstream present pBuffer -%x\n ",(OMX_U32)pBuffer);
		inputBufferFifo.pushBack((void*)pBuffer);
		//add this buffer to list of bitstream buffer
		ResolveDependencies();
	}
}

/*************************************************************************************
SW JPEG Decode code
*************************************************************************************/
void jpegdec_arm_nmf_parser_arm::ResolveDependencies()
{
    OstTraceFiltInst0(TRACE_API, "PARSER_ARM: Inside ResolveDependencies \n");
	isEOSTrue = OMX_FALSE;
	if(!outputDependencyResolved)
	{
        OstTraceFiltInst0(TRACE_FLOW, "PARSER_ARM: ResolveDependencies !outputDependencyResolved\n");
		if(mPorts[1].queuedBufferCount())
		{
			outputDependencyResolved = OMX_TRUE;
            OstTraceFiltInst0(TRACE_FLOW, "PARSER_ARM: ResolveDependencies outputDependencyResolved = true\n");
		}

	}

	if(!headerDependencyResolved)
	{
		//check for header buffer
        OstTraceFiltInst0(TRACE_FLOW, "PARSER_ARM: ResolveDependencies !headerDependencyResolved\n");
		if(inputHeaderFifo.getSize())
		{
			headerDependencyResolved = OMX_TRUE;
            OstTraceFiltInst0(TRACE_FLOW, "PARSER_ARM: ResolveDependencies headerDependencyResolved = true\n");
		}
	}//end of headerdepndency check

	if(inputBufferFifo.getSize() && headerDependencyResolved)
	{
		inputDependencyResolved = OMX_TRUE;
        OstTraceFiltInst0(TRACE_FLOW, "PARSER_ARM: ResolveDependencies inputDependencyResolved = true\n");
		//taking out input buffer
		{
				OMX_BUFFERHEADERTYPE *pBuffer = (OMX_BUFFERHEADERTYPE *)inputBufferFifo.popFront();
				//Update link list with this buffer fixme
				while(pBuffer)
				{
                   //CHECK For EOS
					if(pBuffer->nFlags & BUFFERFLAG_EOS)
					{
						isEOSTrue = OMX_TRUE;
                        OstTraceFiltInst0(TRACE_FLOW, "PARSER_ARM: ResolveDependencies EOS = true\n");
					}
					// check if filled length non zero
					if(pBuffer->nFilledLen == 0)
					{
#if 0
						#ifdef BTSLNKLST_DBG_PRNT
						printf("\n Filled length is zero for pBuffer: 0x%X",pBuffer);
						#endif
#endif
						if(isEOSTrue)
						{
                        	#ifdef BTSLNKLST_DBG_PRNT
								printf("\n Filled length is zero & EOS FLAG set : 0x%X",pBuffer);
                        	#endif
							//> FIX ME: return all input & output buffers
////							endAlgo(STATUS_JOB_COMPLETE,VDC_ERT_END_OF_BITSREAM_REACHED,0);
						}
						else
////#endif
						{
							mPorts[0].returnBuffer(pBuffer);
                            OstTraceFiltInst0(TRACE_FLOW, "PARSER_ARM: ResolveDependencies returning inputbuffer\n");
							isInputBufferDequeue = OMX_FALSE;
							#ifdef DBG_PRINT
							printf("Returning Input buffer \n");
							#endif
						}
					}
////////////////////////else
					{
						#ifdef BTSLNKLST_DBG_PRNT
						printf("\n ++++++++adding to bitstream list buffer 0x%X",pBuffer);
						#endif
                        OstTraceFiltInst1(TRACE_FLOW, "PARSER_ARM:  ++++++++adding to bitstream list buffer 0x%X",(OMX_U32)pBuffer);
						ts_bitstream_buf_link_and_header currentBufferLink;
						t_ahb_address buf_struct_addr;
						t_uint16 result = addBufferToBtsLinkList(pBuffer,&currentBufferLink,&buf_struct_addr);
						if(result)
						{
								DBC_ASSERT(0);
								//while(1);
								OstTraceFiltInst1(TRACE_ERROR, "PARSER_ARM: Incorrect result at line no : %d ",__LINE__);
						}

						if((bufferUpdateNeeded == OMX_TRUE))
						{
							#ifdef BTSLNKLST_DBG_PRNT
							printf("\n||||||||||Programmed Buf struct = 0x%X\n",buf_struct_addr);
							#endif
							sdc_jpeg_param_desc.s_in_out_bitstream_buffer.addr_bitstream_buf_struct = (t_ahb_address)buf_struct_addr;//&currentBufferLink->bufferLink ;
							sdc_jpeg_param_desc.s_in_out_bitstream_buffer.addr_bitstream_start = (t_ahb_address)(currentBufferLink.pBuffer->pBuffer) + (t_ahb_address)(currentBufferLink.pBuffer->nOffset);
							bufferUpdateNeeded = OMX_FALSE;
                            OstTraceFiltInst0(TRACE_FLOW, "PARSER_ARM:bufferUpdateNeeded == OMX_TRUE");
						}
                   }//end of nFilled length check's else
				   pBuffer = (OMX_BUFFERHEADERTYPE *)inputBufferFifo.popFront();
				}//end of while
		}

    	//generate EventPortSettings changed if their is a change in width or height
		if (current_frame_width != sdc_jpeg_param_desc.s_in_parameters.frame_width || current_frame_height != sdc_jpeg_param_desc.s_in_parameters.frame_height)
    	{
			#ifdef DBG_PRINT
				printf("\n DO th needful for Event Port0 Setting Changed for Width and Hieght\n");
			#endif
                OstTraceFiltInst0(TRACE_ERROR, "PARSER_ARM DO th needful for Event Port0 Setting Changed for Width and Hieght\n");
			portDef1.format.image.nFrameHeight = sdc_jpeg_param_desc.s_in_parameters.frame_height;
			portDef1.format.image.nFrameWidth = sdc_jpeg_param_desc.s_in_parameters.frame_width;
			//portDef1.format.image.eColorFormat = formatType;
			changePortSettingsSignal.portSettings((void*)&portDef1);
			isPortSetting = OMX_TRUE;
		}

		if (portDef2.format.image.eColorFormat != formatType)
    	{
			#ifdef DBG_PRINT
				printf("\n DO th needful for Event Port Setting Changed at Port1 for FORMAT\n");
			#endif
            OstTraceFiltInst0(TRACE_ERROR, "PARSER_ARM DO th needful for Event Port1 Setting Changed for Width and Hieght\n");
			portDef2.format.image.nFrameHeight = sdc_jpeg_param_desc.s_in_parameters.frame_height;
			portDef2.format.image.nFrameWidth = sdc_jpeg_param_desc.s_in_parameters.frame_width;
			portDef2.nBufferSize = bufferSizeReq;
			portDef2.format.image.eColorFormat = formatType;
			changePortSettingsSignal.portSettings((void*)&portDef2);
	     }
	}

	if(inputDependencyResolved && outputDependencyResolved && headerDependencyResolved)
	{
		//for taking out output buffer

		    OMX_BUFFERHEADERTYPE *pBuffer;
			pBuffer = mPorts[1].dequeueBuffer();

        #ifdef DBG_PRINT
        			printf("Taking Output buffer \n");
        #endif
            OstTraceFiltInst0(TRACE_FLOW, "PARSER_ARM Taking Output buffer \n");
			ts_image_buf_link_and_header currentBufferLink;
			t_uint16 result = addBufferToImageLinkList(pBuffer,&currentBufferLink);
			if(result)
			{
				 DBC_ASSERT(0);
				 //while(1);
				 OstTraceFiltInst1(TRACE_ERROR, "PARSER_ARM: Incorrect result at line no : %d ",__LINE__);
			}

                        //program sdc_jpeg_param_desc
                        sdc_jpeg_param_desc.s_out_frame_buffer.addr_dest_buffer = (t_ahb_address)currentBufferLink.bufferLink.addr_dest_buffer ;
                        isOutputBufferDequeue = OMX_TRUE;
                        allocatedBufferSize = pBuffer->nAllocLen;


      if(!sdc_jpeg_scan_desc.buffer_p)//////buffer_p set to 0 in provideBitstreamBuffer for fakebuffers used to handle the case of filledlenth =0
            {
                OstTraceFiltInst0(TRACE_FLOW, "PARSER_ARM buffer_p set to 0 in provideBitstreamBuffer for fakebuffers used to handle the case of filledlenth =0 \n");
                            return_InputBuffers();  /////freeing all the buffers
							return_OutputBuffers(bufferSizeReq);

					        //check for EOS & Generate event if required
							if(isEOSTrue)
							{
								proxy.eventHandler(OMX_EventBufferFlag, 1, BUFFERFLAG_EOS);
								isEOSTrue = OMX_FALSE;
							}

							//reset frame_width & frame_height for image mode
							//for payload management do not reset frame width or height
							// will be set by set Paramter or update sttings in case of port sttings
							//current_frame_width = 0;
							//current_frame_height = 0;

							inputDependencyResolved		=OMX_FALSE;
							outputDependencyResolved	=OMX_FALSE;
							headerDependencyResolved	=OMX_FALSE;
            }
       else
            {
                    {
                        ts_sdc_jpeg_scan_desc *headerBuf = (ts_sdc_jpeg_scan_desc *)inputHeaderFifo.popFront();
                        //> TO DO: copy header data into Param structures or have two copy of param to optimize
                        //headerDependencyResolved = OMX_TRUE;
                        //copy buffer offset

                        if (headerBuf)
                        {
                            sdc_jpeg_param_desc.s_in_out_bitstream_buffer.bitstream_offset = headerBuf->bitOffset;
                        }
                        OstTraceFiltInst0(TRACE_API, "PARSER_ARM Taking out headerBuffer\n");
                        //store frame width & frame height to calculate nFilledLength & further checking for port settings
                        current_frame_width = sdc_jpeg_param_desc.s_in_parameters.frame_width;
                        current_frame_height = sdc_jpeg_param_desc.s_in_parameters.frame_height;
                    }//end of header buffer FIFO size check

                    isPortSetting = OMX_FALSE;

                    //generate EventPortSettings changed if their is a change in width or height
                    if (current_frame_width != sdc_jpeg_param_desc.s_in_parameters.frame_width || current_frame_height != sdc_jpeg_param_desc.s_in_parameters.frame_height)
                    {
                        #ifdef DBG_PRINT
                            printf("\n DO th needful for Event Port Setting Changed \n");
                        #endif
                        //proxy.eventHandler(OMX_EventPortSettingsChanged, 1, NULL);
                            OstTraceFiltInst0(TRACE_ERROR, "PARSER_ARM in resolve dep DO th needful for Event Port Setting Changed \n");
                        portDef1.format.image.nFrameHeight = sdc_jpeg_param_desc.s_in_parameters.frame_height;
                        portDef1.format.image.nFrameWidth = sdc_jpeg_param_desc.s_in_parameters.frame_width;
                        portDef1.format.image.eColorFormat = formatType;
                        changePortSettingsSignal.portSettings((void*)&portDef1);
                        isPortSetting = OMX_TRUE;
                    }

                    if (allocatedBufferSize < bufferSizeReq || portDef2.format.image.eColorFormat != formatType)
                    {
                        #ifdef DBG_PRINT
                            printf("\n DO th needful for Event Port Setting Changed at Port1\n");
                        #endif
                        OstTraceFiltInst0(TRACE_ERROR, "PARSER_ARM in resolve dep DO th needful for Event Port1 Setting Changed \n");
                        portDef2.format.image.nFrameHeight = sdc_jpeg_param_desc.s_in_parameters.frame_height;
                        portDef2.format.image.nFrameWidth = sdc_jpeg_param_desc.s_in_parameters.frame_width;
                        portDef2.nBufferSize = bufferSizeReq;
                        portDef2.format.image.eColorFormat = formatType;
                        changePortSettingsSignal.portSettings((void*)&portDef2);
                    }

                    if (allocatedBufferSize < bufferSizeReq)
                    {
                        //return the outputbuffer is allocated
                        isPortSetting = OMX_TRUE;
                        OstTraceFiltInst0(TRACE_ERROR, "PARSER_ARM in resolve dep allocatedBufferSize < bufferSizeReq \n");
                    }

                    if (isPortSetting)
                    {
                        isPortSetting = OMX_FALSE;
                        if (isOutputBufferDequeue)
                        {
                            return_OutputBuffers(0);
                            outputDependencyResolved    =OMX_FALSE;
                        }
                        return;
                    }

                    processActualExecution = OMX_TRUE;
                    //Read from Image buffer List & Bts Buffer list and copy Buffer Flags
                    {
                        ts_bitstream_buf_link_and_header btsBuffer;
                        ts_image_buf_link_and_header imageBuffer;

                        memset(&btsBuffer,0x0,sizeof(ts_bitstream_buf_link_and_header)); //to fix coverity

                        t_uint16 result =  readLastBtsBufferfromLinkList(&btsBuffer);
                        if(result)
                        {
                            DBC_ASSERT(0);
                            //while(1);
                            OstTraceFiltInst1(TRACE_ERROR, "PARSER_ARM: Incorrect result at line no : %d ",__LINE__);
                        }

                        result = readFirstImageBufferfromLinkList(&imageBuffer);
                        if(result)
                        {
                            DBC_ASSERT(0);
                            //while(1);
                            OstTraceFiltInst1(TRACE_ERROR, "PARSER_ARM: Incorrect result at line no : %d ",__LINE__);
                        }
            #ifdef DBG_PRINT
                        printf("\nbtsBuffer.pBuffer: 0x%X",btsBuffer.pBuffer);
            #endif
                        imageBuffer.pBuffer->nFlags = btsBuffer.pBuffer->nFlags;
                        imageBuffer.pBuffer->nTimeStamp = btsBuffer.pBuffer->nTimeStamp;
                    }

            #ifdef DBG_PRINT
                    printf("Value of downsamplingFactor %d and colormode : %d \n",downsamplingFactor,colourMode);
            #endif
                    OstTraceFiltInst2(TRACE_FLOW, "PARSER_ARM:Value of downsamplingFactor %d and colormode : %d \n",downsamplingFactor,colourMode);
                    //configureAlgo
            #ifdef DBG_PRINT
                    printf("All dependencies resolved!! Lets enjoy!! \n");
            #endif
                    if(firstBufferWithBitstream == OMX_TRUE)
                    {
                        OstTraceFiltInst0(TRACE_FLOW, "PARSER_ARM  configure Algo\n");
                    iAlgo.configureAlgo((t_uint32)  &sdc_jpeg_param_desc.s_in_frame_buffer,
                                        (t_uint32)  &sdc_jpeg_param_desc.s_out_frame_buffer,
                                        (t_uint32)  &sdc_jpeg_param_desc.s_internal_buffer,
                                        (t_uint32)  &sdc_jpeg_param_desc.s_in_out_bitstream_buffer,
                                        (t_uint32)  &sdc_jpeg_param_desc.s_out_bitstream_buffer,
                                        (t_uint32)  &sdc_jpeg_param_desc.s_in_parameters,
                                        (t_uint32)  &sdc_jpeg_param_desc.s_out_parameters,
                                        (t_uint32)  &sdc_jpeg_param_desc.s_in_out_frame_parameters,
                                        (t_uint32)  &sdc_jpeg_param_desc.s_out_frame_parameters);

                    //controlAlgo
            #ifdef DBG_PRINT
                    printf("\n control algo: CMD_START\n");
            #endif
                    OstTraceFiltInst0(TRACE_FLOW, "PARSER_ARM  control Algo\n");
                    firstBufferWithBitstream = OMX_FALSE;
                    iAlgo.controlAlgo(CMD_START,0);
                    processActualExecution = OMX_FALSE;
                    }
                    else
                    {
                       //update buffer list if buffer is needed !
                       //falgs update
                        ts_bitstream_buf_link_and_header btsBuffer;
                        ts_image_buf_link_and_header imageBuffer;

                        memset(&btsBuffer,0x0,sizeof(ts_bitstream_buf_link_and_header)); //to fix coverity

                        t_uint16 result =  readLastBtsBufferfromLinkList(&btsBuffer);
                        if(result)
                        {
                            DBC_ASSERT(0);
                            //while(1);
                            OstTraceFiltInst1(TRACE_ERROR, "PARSER_ARM: Incorrect result at line no : %d ",__LINE__);
                        }

                        result = readFirstImageBufferfromLinkList(&imageBuffer);
                        if(result)
                        {
                            DBC_ASSERT(0);
                            //while(1);
                            OstTraceFiltInst1(TRACE_ERROR, "PARSER_ARM: Incorrect result at line no : %d ",__LINE__);
                        }
            #ifdef DBG_PRINT
                        printf("\nbtsBuffer.pBuffer: 0x%X",btsBuffer.pBuffer);
            #endif
                        imageBuffer.pBuffer->nFlags = btsBuffer.pBuffer->nFlags;
                        imageBuffer.pBuffer->nTimeStamp = btsBuffer.pBuffer->nTimeStamp;

                        //call update.Algo
            #ifdef DBG_PRINT
                        printf("\n control algo: CMD_UPDATE_BUFFER\n");
            #endif
            OstTraceFiltInst0(TRACE_FLOW, "PARSER_ARM  update Algo\n");
                        iAlgo.updateAlgo(CMD_UPDATE_BUFFER,0,0);
                        processActualExecution = OMX_FALSE;
                    }
             }
    }
}


void jpegdec_arm_nmf_parser_arm::return_InputBuffers()
{
    OstTraceFiltInst0(TRACE_API, "PARSER_ARM  return_InputBuffers\n");
	t_uint16 result = 0;
	if (isInputBufferDequeue)
	{
		bufferUpdateNeeded = OMX_TRUE;
		//Return Input Buffers
        //FIX ME: check for last buffer contents whether completely consumed or not
		if((bufferLinkStart->bufferLink.addr_next_buf_link ==0) && (bufferLinkStart->bufferLink.addr_next_buf_link == 0))
		{
			if(bufferLinkStart->pBuffer->nFlags & BUFFERFLAG_EOS)
			{
				isEOSTrue = OMX_TRUE;
			}
			if(isDispatching())
			{
				mPorts[0].returnBuffer(bufferLinkStart->pBuffer);
			}
			else
			{
				//printf("return from Asyn - input \n");
				Component::returnBufferAsync(0,bufferLinkStart->pBuffer);
			}
			isInputBufferDequeue = OMX_FALSE;
			//mPorts[0].returnBuffer(bufferLinkStart->pBuffer);
			#ifdef DBG_PRINT
				printf("Returning Input buffer \n");
			#endif
			result = removeBufferFromBtsLinkList(bufferLinkStart);
			if(result)
			{
				//FIX ME !
				//while(1);
				OstTraceFiltInst1(TRACE_ERROR, "PARSER_ARM: Incorrect result at line no : %d ",__LINE__);
			}
		}
		else
		{
			while(bufferLinkStart->bufferLink.addr_next_buf_link)
			{
				//check with firmware output fixme
				if(isDispatching())
				{
					mPorts[0].returnBuffer(bufferLinkStart->pBuffer);
				}
				else
				{
					//printf("return from Asyn - input \n");
					Component::returnBufferAsync(0,bufferLinkStart->pBuffer);
				}
				isInputBufferDequeue = OMX_FALSE;
				//	mPorts[0].returnBuffer(bufferLinkStart->pBuffer);
				#ifdef DBG_PRINT
					printf("Returning Input buffer \n");
				#endif
				if(bufferLinkStart->pBuffer->nFlags & BUFFERFLAG_EOS)
				{
					isEOSTrue = OMX_TRUE;
				}
				//bufferLinkStart = (ts_bitstream_buf_link_and_header *)bufferLinkStart->bufferLink.addr_next_buf_link;
				result = removeBufferFromBtsLinkList(bufferLinkStart);
				if(result)
				{
					//FIX ME !
					//while(1);
					OstTraceFiltInst1(TRACE_ERROR, "PARSER_ARM: Incorrect result at line no : %d ",__LINE__);
				}
			}
			//return the last buffer
			if(isDispatching())
			{
				mPorts[0].returnBuffer(bufferLinkStart->pBuffer);
			}
			else
			{
				//printf("return from Asyn - input \n");
				Component::returnBufferAsync(0,bufferLinkStart->pBuffer);
			}
			isInputBufferDequeue = OMX_FALSE;
	 		//  mPorts[0].returnBuffer(bufferLinkStart->pBuffer);
			#ifdef DBG_PRINT
			    printf("Returning Input buffer \n");
			#endif
			if(bufferLinkStart->pBuffer->nFlags & BUFFERFLAG_EOS)
			{
				isEOSTrue = OMX_TRUE;
			}
			result = removeBufferFromBtsLinkList(bufferLinkStart);
			if(result)
			{
				//FIX ME !
				//while(1);
				OstTraceFiltInst1(TRACE_ERROR, "PARSER_ARM: Incorrect result at line no : %d ",__LINE__);
			}
		}
	}
}


void jpegdec_arm_nmf_parser_arm::return_OutputBuffers(OMX_U32 output_buffSize)
{
	t_uint16 result = 0;
    OstTraceFiltInst0(TRACE_API, "PARSER_ARM  return_OutputBuffers\n");
	if (isOutputBufferDequeue)
	{
		bufferUpdateNeeded = OMX_TRUE;
		//Return output Buffers
		if ((imageBufferLinkStart->next == 0) && (imageBufferLinkStart->prev ==0) )
		{
			if(isEOSTrue)
			{
				imageBufferLinkStart->pBuffer->nFlags |= BUFFERFLAG_EOS;
			}
			// FIX ME : correct nFilled length
			imageBufferLinkStart->pBuffer->nFilledLen = output_buffSize;
			if(isDispatching())
			{
				mPorts[1].returnBuffer(imageBufferLinkStart->pBuffer);
			}
			else
			{
				//printf("return from Asyn - input \n");
				Component::returnBufferAsync(1,imageBufferLinkStart->pBuffer);
			}

			//mPorts[1].returnBuffer(imageBufferLinkStart->pBuffer);
			isOutputBufferDequeue = OMX_FALSE;
			#ifdef DBG_PRINT
				printf("Returning Ouput buffer \n");
			#endif
			result =  removeBufferfromImageLinkList(imageBufferLinkStart);
			if(result)
			{
				//FIX ME !
				//while(1);
				OstTraceFiltInst1(TRACE_ERROR, "PARSER_ARM: Incorrect result at line no : %d ",__LINE__);
			}
		}
		else
		{
			while(imageBufferLinkStart->next)
            {
			    if(isEOSTrue)
			   	{
			   		imageBufferLinkStart->pBuffer->nFlags |= BUFFERFLAG_EOS;
			   	}

				imageBufferLinkStart->pBuffer->nFilledLen = output_buffSize;
				if(isDispatching())
				{
					mPorts[1].returnBuffer(imageBufferLinkStart->pBuffer);
				}
				else
				{
					//printf("return from Asyn - input \n");
					Component::returnBufferAsync(1,imageBufferLinkStart->pBuffer);
				}

				//mPorts[1].returnBuffer(imageBufferLinkStart->pBuffer);
				isOutputBufferDequeue = OMX_FALSE;
				#ifdef DBG_PRINT
					printf("Returning Ouput buffer \n");
				#endif
				//imageBufferLinkStart->next;
				result =  removeBufferfromImageLinkList(imageBufferLinkStart);
				if(result)
				{
					//FIX ME !
					//while(1);
					OstTraceFiltInst1(TRACE_ERROR, "PARSER_ARM: Incorrect result at line no : %d ",__LINE__);
				}
				// remove from the list
			}
		}
	}
}

void jpegdec_arm_nmf_parser_arm::endAlgo(t_t1xhv_status status, t_t1xhv_decoder_info info, t_uint32 durationInTicks)
{
	//#ifdef DBG_PRINT
		printf("\n endAlgo called with status =%d\n",status);

	OstTraceFiltInst1(TRACE_API, "PARSER_ARM: In endAlgo with status : 0x%x \n",status);
	//#endif
	//OMX_BOOL isEOSTrue = OMX_FALSE;
	t_uint16 result = 0;
//	OMX_U32 filledlength;
	switch(status)
	{
		case STATUS_JOB_COMPLETE:
							return_InputBuffers();
							return_OutputBuffers(bufferSizeReq);
                            OstTraceFiltInst0(TRACE_FLOW, "PARSER_ARM  returned OUtput_InputBuffers\n");
					        //check for EOS & Generate event if required
							if(isEOSTrue)
							{
								proxy.eventHandler(OMX_EventBufferFlag, 1, BUFFERFLAG_EOS);
                                OstTraceFiltInst0(TRACE_FLOW, "PARSER_ARM sending EOS in endalgo\n");
								isEOSTrue = OMX_FALSE;
							}

							//reset frame_width & frame_height for image mode
							//for payload management do not reset frame width or height
							// will be set by set Paramter or update sttings in case of port sttings
							//current_frame_width = 0;
							//current_frame_height = 0;

							inputDependencyResolved		=OMX_FALSE;
							outputDependencyResolved	=OMX_FALSE;
							headerDependencyResolved	=OMX_FALSE;

							//areFlagsAndTimestampSaved   = OMX_FALSE;
					        ResolveDependencies();
							break;

	   case STATUS_BUFFER_NEEDED:

							   //firstBufferWithBitstream  =OMX_TRUE;
								#ifdef DBG_PRINT
									   printf("\n >>>>>>>>>>STATUS BUFFER NEEDED<<<<<<<<<<<");
								#endif
							   //return used up buffers
					            while(bufferLinkStart->bufferLink.addr_next_buf_link)
								{
									//check with firmware output fixme
									//printf("returning buffer bufferLinkStart->pBuffer: 0x%X",bufferLinkStart->pBuffer);
									mPorts[0].returnBuffer(bufferLinkStart->pBuffer);
									#ifdef DBG_PRINT
									printf("Returning Input buffer \n");
									#endif
									//bufferLinkStart = (ts_bitstream_buf_link_and_header *)bufferLinkStart->bufferLink.addr_next_buf_link;
									result = removeBufferFromBtsLinkList(bufferLinkStart);
									if(result)
									{
										//FIX ME !
										//while(1);
										OstTraceFiltInst1(TRACE_ERROR, "PARSER_ARM: Incorrect result at line no : %d ",__LINE__);
									}
								}
                                OstTraceFiltInst0(TRACE_FLOW, "PARSER_ARM status buffer needed removed buffers form linked list\n");
								//return the last buffer in the list
								#ifdef DBG_PRINT
									printf("returning buffer bufferLinkStart->pBuffer: 0x%X",bufferLinkStart->pBuffer);
								#endif
		    					mPorts[0].returnBuffer(bufferLinkStart->pBuffer);
								#ifdef DBG_PRINT
								    printf("Returning Input buffer \n");
								#endif
								result = removeBufferFromBtsLinkList(bufferLinkStart);
								if(result)
								{
									//FIX ME !
									//while(1);
									OstTraceFiltInst1(TRACE_ERROR, "PARSER_ARM: Incorrect result at line no : %d ",__LINE__);
								}

							   bufferUpdateNeeded = OMX_TRUE;
							   //> FIX ME
							   //ResolveDependencies();
							   //return the bitstream / Image buffer buffer FIX ME
							   break;
        default:
                                OstTraceFiltInst0(TRACE_FLOW, "PARSER_ARM: Error in ALGO \n");
								break;
	}
	//fixme
}

void jpegdec_arm_nmf_parser_arm::setParam(void* portDef1, void* portDef2)
{
    OstTraceInt0(TRACE_API, "PARSER_ARM setParam\n");
    //printf(" \njpegdec_arm_nmf_parser_arm::setParam  \n");
	//OMX_PARAM_PORTDEFINITIONTYPE *tmpPortDef = (OMX_PARAM_PORTDEFINITIONTYPE*)portDef;
	OMX_IMAGE_PORTDEFINITIONTYPE *tmpFormat = (OMX_IMAGE_PORTDEFINITIONTYPE*)portDef1;

	// lets see what is necessary
	this->portDef1.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
	this->portDef1.nVersion.s.nVersionMajor    = 1;
    this->portDef1.nVersion.s.nVersionMinor    = 1;
    this->portDef1.nVersion.s.nRevision	= 1;
    this->portDef1.nVersion.s.nStep		= 0;
	this->portDef1.nPortIndex = 0;

	//copy format
    this->portDef1.format.image.bFlagErrorConcealment = tmpFormat->bFlagErrorConcealment;
	this->portDef1.format.image.cMIMEType = tmpFormat->cMIMEType;
	this->portDef1.format.image.eColorFormat = tmpFormat->eColorFormat;
	this->portDef1.format.image.eCompressionFormat = tmpFormat->eCompressionFormat;
//	this->portDef1.format.image.nBitrate = tmpFormat->nBitrate;
	this->portDef1.format.image.nFrameHeight = tmpFormat->nFrameHeight;
	this->portDef1.format.image.nFrameWidth = tmpFormat->nFrameWidth;
	this->portDef1.format.image.nSliceHeight = tmpFormat->nSliceHeight;
	this->portDef1.format.image.nStride = tmpFormat->nStride;
	this->portDef1.format.image.pNativeRender = tmpFormat->pNativeRender;
	this->portDef1.format.image.pNativeWindow = tmpFormat->pNativeWindow;
//	this->portDef.format.image.xFramerate = tmpFormat->xFramerate;
	//set current_frame_width & height
	current_frame_height = tmpFormat->nFrameHeight;
	current_frame_width = tmpFormat->nFrameWidth;

	tmpFormat = (OMX_IMAGE_PORTDEFINITIONTYPE*)portDef2;

	// lets see what is necessary
	this->portDef2.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
	this->portDef2.nVersion.s.nVersionMajor    = 1;
    this->portDef2.nVersion.s.nVersionMinor    = 1;
    this->portDef2.nVersion.s.nRevision	= 1;
    this->portDef2.nVersion.s.nStep		= 0;
	this->portDef2.nPortIndex = 1;

	//copy format
    this->portDef2.format.image.bFlagErrorConcealment = tmpFormat->bFlagErrorConcealment;
	this->portDef2.format.image.cMIMEType = tmpFormat->cMIMEType;
	this->portDef2.format.image.eColorFormat = tmpFormat->eColorFormat;
	this->portDef2.format.image.eCompressionFormat = tmpFormat->eCompressionFormat;
//	this->portDef2.format.image.nBitrate = tmpFormat->nBitrate;
	this->portDef2.format.image.nFrameHeight = tmpFormat->nFrameHeight;
	this->portDef2.format.image.nFrameWidth = tmpFormat->nFrameWidth;
	this->portDef2.format.image.nSliceHeight = tmpFormat->nSliceHeight;
	this->portDef2.format.image.nStride = tmpFormat->nStride;
	this->portDef2.format.image.pNativeRender = tmpFormat->pNativeRender;
	this->portDef2.format.image.pNativeWindow = tmpFormat->pNativeWindow;
//	this->portDef.format.image.xFramerate = tmpFormat->xFramerate;

}

void jpegdec_arm_nmf_parser_arm::updateParamAndConfigScale(ts_ddep_sdc_jpeg_dynamic_params *pDynamicConfig)
{
    OstTraceFiltInst0(TRACE_API, "PARSER_ARM  updateParamAndConfigScale\n");
	mParam.downsamplingFactor = (JPEGDec_Proxy_Downsampling_Factor)pDynamicConfig->downsampling_factor;
}


void jpegdec_arm_nmf_parser_arm::updateParamAndConfigCrop(ts_ddep_sdc_jpeg_dynamic_params *pDynamicConfig)
{
    OstTraceFiltInst0(TRACE_API, "PARSER_ARM updateParamAndConfigCrop\n");
	mParam.cropWindowWidth   = pDynamicConfig->window_width;
	mParam.cropWindowHeight  = pDynamicConfig->window_height;
    mParam.horizontal_offset = pDynamicConfig->horizontal_offset;
    mParam.vertical_offset   = pDynamicConfig->vertical_offset;
}

void jpegdec_arm_nmf_parser_arm::updateDynamicConfigurationStructure()
{
	t_uint16 atomic_write;
    OstTraceFiltInst0(TRACE_API, "PARSER_ARM uupdateDynamicConfigurationStructure\n");
	atomic_write = ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set_atomic_entry + 1;
	ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set_atomic_entry = atomic_write;

	ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.window_width = mParam.cropWindowWidth;
	ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.window_height = mParam.cropWindowHeight;

	ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.vertical_offset = mParam.vertical_offset;
	ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.horizontal_offset =mParam.horizontal_offset;
	ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.downsampling_factor =mParam.downsamplingFactor;

	ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set_atomic_exit = atomic_write;
}



void jpegdec_arm_nmf_parser_arm::setConfig(t_uint32 nParamIndex,ts_ddep_sdc_jpeg_dynamic_params config)
{
#ifdef DBG_PRINT
	printf("setConfig enter \n");
#endif
    OstTraceFiltInst0(TRACE_API, "PARSER_ARM setConfig\n");
	switch(nParamIndex)
	{
		case OMX_IndexConfigCommonScale:
			updateParamAndConfigScale(&config);
			mParam.isDownScalingEnabled = 1;
			break;
		case OMX_IndexConfigCommonOutputCrop:
			updateParamAndConfigCrop(&config);
			mParam.isCroppingEnabled = 1;
			break;
		default:
			break;
	}

	updateDynamicConfigurationStructure();

}


OMX_ERRORTYPE jpegdec_arm_nmf_parser_arm::checkConfAndParams(){
	if(!(mParam.frameWidth>0 && mParam.frameWidth<=8176))
    {
        OstTraceFiltInst0(TRACE_ERROR, "PARSER_ARM checkConfAndParams OMX_ErrorUnsupportedSetting (!(mParam.frameWidth>0 && mParam.frameWidth<=8176)) \n");
		return OMX_ErrorUnsupportedSetting;
    }

	if(!(mParam.frameHeight>0 && mParam.frameHeight<=8176))
    {
        OstTraceFiltInst0(TRACE_ERROR, "PARSER_ARM checkConfAndParams OMX_ErrorUnsupportedSetting !(mParam.frameHeight>0 && mParam.frameHeight<=8176) \n");
		return OMX_ErrorUnsupportedSetting;
    }

    if(!(mParam.downsamplingFactor ==1
	   || mParam.downsamplingFactor ==2
	   || mParam.downsamplingFactor== 4
	   || mParam.downsamplingFactor == 8))
	{
        OstTraceFiltInst0(TRACE_ERROR, "PARSER_ARM checkConfAndParams OMX_ErrorUnsupportedSetting unsupported downscale \n");
		 return OMX_ErrorUnsupportedSetting;
	}

	if(mParam.nbComponents>MAX_NUMBER_COMPONENTS)
    {
        OstTraceFiltInst0(TRACE_ERROR, "PARSER_ARM checkConfAndParams OMX_ErrorUnsupportedSetting Unsupported componetnss \n");
	   return OMX_ErrorUnsupportedSetting;
    }

   if(mParam.nbScanComponents>MAX_NUMBER_COMPONENTS)
   {
       OstTraceFiltInst0(TRACE_ERROR, "PARSER_ARM checkConfAndParams OMX_ErrorUnsupportedSetting Unsupported scan componetnss \n");
	   return OMX_ErrorUnsupportedSetting;
   }

   for(t_uint32 cnt =0;cnt<mParam.nbComponents;cnt++ )
   {
		t_uint32 h_factor, v_factor;
		h_factor = mParam.samplingFactors[cnt].hSamplingFactor;
		v_factor = mParam.samplingFactors[cnt].vSamplingFactor;

		if(h_factor<1 || h_factor >4 || h_factor==3){
            OstTraceFiltInst0(TRACE_ERROR, "PARSER_ARM checkConfAndParams OMX_ErrorUnsupportedSetting Unsupported h_factor  \n");
			return OMX_ErrorUnsupportedSetting;
        }

		if(v_factor<1 || v_factor >4 || v_factor==3){
            OstTraceFiltInst0(TRACE_ERROR, "PARSER_ARM checkConfAndParams OMX_ErrorUnsupportedSetting Unsupported v_factor  \n");
			return OMX_ErrorUnsupportedSetting;
        }
    }

   if(mParam.mode ==1){
       OstTraceFiltInst0(TRACE_ERROR, "PARSER_ARM checkConfAndParams OMX_ErrorUnsupportedSetting Unsupported mode  \n");
		return OMX_ErrorUnsupportedSetting;
   }

	if(mParam.mode==1){
        if(mParam.startSpectralSelection>63){
            OstTraceFiltInst0(TRACE_ERROR, "PARSER_ARM checkConfAndParams OMX_ErrorUnsupportedSetting Unsupported startSpectralSelection  \n");
			return OMX_ErrorUnsupportedSetting;
        }
		if(mParam.endSpectralSelection!=63){
            OstTraceFiltInst0(TRACE_ERROR, "PARSER_ARM checkConfAndParams OMX_ErrorUnsupportedSetting Unsupported endSpectralSelection  \n");
			return OMX_ErrorUnsupportedSetting;
        }
	}
	else{
		if(mParam.startSpectralSelection!=0){
            OstTraceFiltInst0(TRACE_ERROR, "PARSER_ARM checkConfAndParams OMX_ErrorUnsupportedSetting Unsupported startSpectralSelection  \n");
			return OMX_ErrorUnsupportedSetting;
    }
		if(mParam.endSpectralSelection!=63){
            OstTraceFiltInst0(TRACE_ERROR, "PARSER_ARM checkConfAndParams OMX_ErrorUnsupportedSetting Unsupported endSpectralSelection  \n");
			return OMX_ErrorUnsupportedSetting;
        }
	}
	return OMX_ErrorNone;
}
void jpegdec_arm_nmf_parser_arm::huffMemory(ts_ddep_buffer_descriptor HuffMemoryDesc)
{
    mHuffMemDesc = HuffMemoryDesc;
    return;
}

void jpegdec_arm_nmf_parser_arm::memoryRequirements(t_uint16 mChannelId,ts_ddep_buffer_descriptor paramBufferDesc,
							  ts_ddep_buffer_descriptor linkListBufferDesc,ts_ddep_buffer_descriptor debugBufferDesc,
							  ts_ddep_buffer_descriptor lineBufferDesc)
{
	//Store Memory Descriptors and initialize pointers
    OstTraceInt0(TRACE_API, "PARSER_ARM memoryRequirements  \n");
	mParamBufferDesc = paramBufferDesc;
    mLineBufferDesc = lineBufferDesc;

	//Initilaize Dynamic configuration memory pointers
	ps_ddep_sdc_jpeg_dynamic_configuration = (ts_ddep_sdc_jpeg_dynamic_configuration*)(paramBufferDesc.nLogicalAddress
											   +sizeof(ts_ddep_sdc_jpeg_param_desc));
	ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set_atomic_entry=0;
	ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set_atomic_exit=0;

	//Provide memories to MPC
//    iDdep.setNeeds(mChannelId,mLinkListBufferDesc,mDebugBufferDesc);
//	iDdep.setParameter(mChannelId,ID_SDC_JPEG,mParamBufferDesc);
}


OMX_ERRORTYPE jpegdec_arm_nmf_parser_arm::configureCoeffAndLineBuffer(){
	t_uint8 *ptr;
	t_uint32 lineBufferSize=0,coeffBufferSize=0;
	ts_t1xhv_vdc_frame_buf_out *p_out_frame_buffer;
	ts_ddep_sdc_jpeg_param_desc *ps_ddep_sdc_jpeg_param_desc;
	t_uint16 width,height;

	width =	(mParam.frameWidth+0xf)&0xfff0;
	height = (mParam.frameHeight+0xf)&0xfff0;
	ptr = (t_uint8*)mParamBufferDesc.nLogicalAddress;
	ps_ddep_sdc_jpeg_param_desc = (ts_ddep_sdc_jpeg_param_desc *) ptr;
	p_out_frame_buffer = &(ps_ddep_sdc_jpeg_param_desc->s_out_frame_buffer);

	if(!mParam.samplingFactors[1].hSamplingFactor)
	{
		formatType = OMX_COLOR_FormatMonochrome;
	}
	else if ((mParam.samplingFactors[0].hSamplingFactor==1) && (mParam.samplingFactors[0].vSamplingFactor==1) &&
		 	 (mParam.samplingFactors[1].hSamplingFactor==1) && (mParam.samplingFactors[1].vSamplingFactor==1) &&
			 (mParam.samplingFactors[2].hSamplingFactor==1) && (mParam.samplingFactors[2].vSamplingFactor==1))
	{
        OstTraceFiltInst0(TRACE_ERROR, "PARSER_ARM configureCoeffAndLineBuffer YUV444  \n");
		formatType = OMX_COLOR_FormatYUV444Interleaved; // YUV 420
		subSamplingType = 2; // YUV 444
	}
	else if ((mParam.samplingFactors[0].hSamplingFactor==2) && (mParam.samplingFactors[0].vSamplingFactor==1) &&
			 (mParam.samplingFactors[1].hSamplingFactor==1) && (mParam.samplingFactors[1].vSamplingFactor==1) &&
			 (mParam.samplingFactors[2].hSamplingFactor==1) && (mParam.samplingFactors[2].vSamplingFactor==1))
	{
        OstTraceFiltInst0(TRACE_ERROR, "PARSER_ARM configureCoeffAndLineBuffer YUV422  \n");
		formatType = OMX_COLOR_FormatYUV422Planar; // YUV 420
		subSamplingType = 1; // YUV 422
	}
	else if ((mParam.samplingFactors[0].hSamplingFactor==2) && (mParam.samplingFactors[0].vSamplingFactor==2) &&
			 (mParam.samplingFactors[1].hSamplingFactor==1) && (mParam.samplingFactors[1].vSamplingFactor==1) &&
			 (mParam.samplingFactors[2].hSamplingFactor==1) && (mParam.samplingFactors[2].vSamplingFactor==1))
	{
        OstTraceFiltInst0(TRACE_ERROR, "PARSER_ARM configureCoeffAndLineBuffer YUV420  \n");
		formatType = OMX_COLOR_FormatYUV420Planar; // YUV 420
		subSamplingType = 0; // YUV 420
	}
	else
	{
        OstTraceFiltInst0(TRACE_ERROR, "PARSER_ARM configureCoeffAndLineBuffer exotic YUV444  \n");
		formatType = OMX_COLOR_FormatYUV444Interleaved; // YUV 420
		subSamplingType = 2; // YUV 444
	}




#ifdef DBG_PRINT
	printf("subSamplingType = %d \n",subSamplingType);
#endif

	//Allocate Line Buffer
	if(mParam.samplingFactors[0].vSamplingFactor==1 &&mParam.samplingFactors[1].vSamplingFactor==1 &&
	   mParam.samplingFactors[2].vSamplingFactor==1 &&mParam.downsamplingFactor==DOWNSAMPLING_FACTOR_8)
	{
		lineBufferSize = (width/2 + 0xf)&0xfffffff0;
	}
	else
	{
		lineBufferSize = 0;
	}

	if(lineBufferSize)
	{
			ptr = (t_uint8*)mLineBufferDesc.nLogicalAddress;
			p_out_frame_buffer->addr_jpeg_line_buffer = ENDIANESS_CONVERSION((OMX_U32)ptr);
	}
	else
	{
		p_out_frame_buffer->addr_jpeg_line_buffer = 0;
	}

	//Allocate memory for coefficient buffer
	if(mParam.mode == PROGRESSIVE_JPEG){
		t_uint16 maxhSampFactor,maxvSampFactor;
		maxhSampFactor = mParam.samplingFactors[0].hSamplingFactor;
		maxhSampFactor = (maxhSampFactor > mParam.samplingFactors[1].hSamplingFactor) ? maxhSampFactor: mParam.samplingFactors[1].hSamplingFactor;
		maxhSampFactor = (maxhSampFactor > mParam.samplingFactors[2].hSamplingFactor) ? maxhSampFactor: mParam.samplingFactors[2].hSamplingFactor;

		maxvSampFactor = mParam.samplingFactors[0].vSamplingFactor;
		maxvSampFactor = (maxvSampFactor > mParam.samplingFactors[1].vSamplingFactor) ? maxvSampFactor: mParam.samplingFactors[1].vSamplingFactor;
		maxvSampFactor = (maxvSampFactor > mParam.samplingFactors[2].vSamplingFactor) ? maxvSampFactor: mParam.samplingFactors[2].vSamplingFactor;

		coeffBufferSize = ((height)*(width)*2*
						   (mParam.samplingFactors[0].hSamplingFactor * mParam.samplingFactors[0].vSamplingFactor +
							mParam.samplingFactors[1].hSamplingFactor * mParam.samplingFactors[1].vSamplingFactor +
							mParam.samplingFactors[2].hSamplingFactor * mParam.samplingFactors[2].vSamplingFactor))/
							(maxhSampFactor*maxvSampFactor);

		coeffBufferSize = (coeffBufferSize + 0xf)&0xfffffff0;
	  }
	  else
	  {
		  coeffBufferSize =0;
	  }

	  if(coeffBufferSize)
	  {
		  //TODO:Allocate memory for coefficient buffer here
		  return OMX_ErrorUndefined;
	  }
	  else
	  {
		  p_out_frame_buffer->addr_jpeg_coef_buffer = 0;
	  }

	return OMX_ErrorNone;
}

