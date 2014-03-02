/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define _CNAME_ Vfm_mpeg4enc
#include "osi_trace.h"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_mpeg4enc_proxy_src_MPEG4Enc_ProcessingComponentTraces.h"
#endif //for OST_TRACE_COMPILER_IN_USE


#include "VFM_Port.h"
#include "VFM_Memory.h"
#include "MPEG4Enc_Proxy.h"
#include <cm/inc/cm_macros.h>
#include "host/eventhandler.hpp"
#include "SharedBuffer.h"
#include "MPEG4Enc_ProcessingComponent.h"

#ifdef MMDSP_PERF
	#include "osi_perf.h"
#endif

#define DEBUG_MEM_SIZE (64*1024 - 1)
#define BUFFER_LINKLIST_SIZE (1024)
#define SEARCH_WINDOW_SIZE_IN_BYTES (0xa000)
#define HEADER_BUFFER_SIZE (0x38)

#define MAX_VBV_OCCUPANCY (63488)

#ifndef CIF_WIDTH
	#define CIF_WIDTH 352
#endif
#ifndef CIF_HEIGHT
	#define CIF_HEIGHT 288
#endif

#ifndef QCIF_WIDTH
	#define QCIF_WIDTH 176
#endif
#ifndef QCIF_HEIGHT
	#define QCIF_HEIGHT 144
#endif

#ifndef VGA_WIDTH
	#define VGA_WIDTH 640
#endif
#ifndef VGA_HEIGHT
	#define VGA_HEIGHT 480
#endif


#define ONE_BIT 1
#define TWO_BITS 2
#define THREE_BITS 3
#define FOUR_BITS 4
#define EIGHT_BITS 8
#define ELEVEN_BITS 11
#define THIRTEEN_BITS 13
#define FIFTEEN_BITS 15
#define SIXTEEN_BITS 16
#define THIRTY_TWO_BITS 32


#define VOS_START_CODE 0x000001b0
#define VO_START_CODE 0x000001b5
#define VO_CODE 0x00000100
#define VOL_CODE 0x00000120
#define VOP_CODE 0x000001b6
#define VOS_END_CODE 0x000001b1

#define SVA_RTYPE_MODE_CONSTANT_ZERO 0
#define SVA_RTYPE_MODE_CONSTANT_ONE 1

typedef enum
{
	SVA_SPATIAL_QUALITY_NONE,
	SVA_SPATIAL_QUALITY_LOW,
	SVA_SPATIAL_QUALITY_MEDIUM,
	SVA_SPATIAL_QUALITY_HIGH
} t_sva_brc_spatial_quality;

typedef enum
{
	SVA_COLOR_PRIMARY_UNKNOWN,
	SVA_COLOR_PRIMARY_BT601_FULLRANGE,
	SVA_COLOR_PRIMARY_BT601_LIMITEDRANGE,
	SVA_COLOR_PRIMARY_BT709_FULLRANGE,
	SVA_COLOR_PRIMARY_BT709_LIMITEDRANGE
} t_sva_ddep_color_primary;


typedef struct
{
	t_uint64 buffer;
	t_uint32 nbBitsValid;
	t_uint8 *currBuffer;/*destination buffer*/
	t_uint8 *endbuffer;
	t_uint32 totalBitsWritten;/*total bits written*/
	//FILE *fout;
}VFM_WRITESTREAM_HEADER;

MPEG4Enc_ProcessingComponent::MPEG4Enc_ProcessingComponent()
{
	mBufferingModel = SVA_BUFFERING_NONE;

	mSwisBufSize = 0;
	mVbvBufferSize = 0;
	mVbvOccupancy = 0;
}

static OMX_ERRORTYPE WriteBits(VFM_WRITESTREAM_HEADER *pStream, t_uint32 data, t_uint32 nbBits)
{
	t_uint64 tmp;

	RETURN_XXX_IF_WRONG(nbBits > 0, OMX_ErrorBadParameter)

	/*write in scratch buffer first*/
	if(nbBits!=32)
	{
		tmp=((data)&((1<<nbBits)-1));
	}//NOTE : can precompute mask table to accelerate
	else
	{
		tmp=data;
	}
	tmp=(tmp<<(64-pStream->nbBitsValid-nbBits));
	pStream->buffer+=tmp;
	pStream->nbBitsValid+=nbBits;
	pStream->totalBitsWritten+=nbBits;

	/*try to write data into destination buffer*/
	while(pStream->nbBitsValid>=8)
	{
		tmp=((pStream->buffer>>56)&0xff);
		*pStream->currBuffer++= (t_uint8)tmp;
		pStream->buffer=(pStream->buffer<<8);
		pStream->nbBitsValid-=8;
	}
	return OMX_ErrorNone;
}

void flushWriteStream(VFM_WRITESTREAM_HEADER *pStream)
{

	if(pStream->nbBitsValid!=0)
	{
		t_uint32 nbStuffBits=8-pStream->nbBitsValid;

		WriteBits(pStream,0,nbStuffBits);
		/*correct pStream->totalBitsWritten so it doesn't take into account padding bits*/
		pStream->totalBitsWritten-=nbStuffBits;
	}
}

t_uint32 MPEG4Enc_ProcessingComponent::Write_VOS_VO_VOL(MPEG4Enc_Proxy *mpeg4enc, t_uint8* addr_pointer)
{
	OstTraceFiltStatic0(TRACE_API, "=> MPEG4Enc_ProcessingComponent::Write_VOS_VO_VOL()", mpeg4enc);

	OMX_VIDEO_PARAM_MPEG4TYPE *pMpeg4Param = &(mpeg4enc->mParam.m_enc_param);
	OMX_VIDEO_PARAM_BITRATETYPE *pBrcType = &(mpeg4enc->mParam.m_bitrate);
	OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE *pErrCorr = &(mpeg4enc->mParam.m_err_corr);

	/* +Change start for CR343589 Rotation */
	OMX_CONFIG_ROTATIONTYPE *pRotation = &(mpeg4enc->mParam.m_rotation);
	/* -Change end for CR343589 Rotation */

	OMX_COLORPRIMARYTYPE pColorPrimary = mpeg4enc->mParam.m_color_primary;
	OMX_PARAM_PIXELASPECTRATIO *pPixelAspectRatio = &(mpeg4enc->mParam.m_pixel_aspect_ratio);

	/* +Change start for CR338066 MPEG4Enc DCI */
	VFM_PARAM_MPEG4SHORTDCITYPE *pMpeg4ShortDCI = &(mpeg4enc->mParam.m_short_dci);
	/* -Change end for CR338066 MPEG4Enc DCI */
	
	VFM_WRITESTREAM_HEADER writeStream;
//< 	t_uint32 nbBits=0;
	t_uint32 bitRate;
	OMX_U32 height = 0, width = 0;
	//< t_uint32 vbvBufferSize;
	//< t_uint32 vbvOccupancy;
	//< t_uint32 swisBuffer;


	bitRate = pBrcType->nTargetBitrate;    //< bitrate in terms of bytes
	//< swisBuffer = bitRate;					//< swis buffer size is usually set equal to bitrate size
	//< vbvOccupancy = bitRate;					//<
	//< vbvBufferSize = swisBuffer;				//< vbvbuffersize is usually swisbuf/16384

	//< ####### experimental changes FIXME
	//< vbvOccupancy = 4000<<6;
	//< swisBuffer = 4000000;					//< swis buffer size is usually set equal to bitrate size
	//< vbvBufferSize =swisBuffer;
	//< ####### experimental changes FIXME

	writeStream.buffer = 0;
	writeStream.currBuffer= addr_pointer;
	writeStream.endbuffer = addr_pointer+HEADER_BUFFER_SIZE;
	writeStream.nbBitsValid=0;
	writeStream.totalBitsWritten=0;

	memset(addr_pointer, 0, HEADER_BUFFER_SIZE);

	/*write VOS+VO+VOL for first frame*/
	WriteBits(&writeStream,VOS_START_CODE/*0x000001b0*/,THIRTY_TWO_BITS/*32*/);

	switch(pMpeg4Param->eProfile)
	{
		case(OMX_VIDEO_MPEG4ProfileSimple):
			switch(pMpeg4Param->eLevel)
			{
				case(OMX_VIDEO_MPEG4Level0):
					WriteBits(&writeStream,0x08,EIGHT_BITS/*8*/);
					OstTraceFiltStatic0(TRACE_DEBUG, "MPEG4ENC_Processing: SP Level0 written in header \n",mpeg4enc);
					break;
					/*case(OMX_VIDEO_MPEG4Level0b):
						WriteBits(&writeStream,0x01,8);
						break;*/
				case(OMX_VIDEO_MPEG4Level1):
					WriteBits(&writeStream,0x01,EIGHT_BITS/*8*/);
					OstTraceFiltStatic0(TRACE_DEBUG, "MPEG4ENC_Processing: SP Level1 written in header \n",mpeg4enc);
					break;
				case(OMX_VIDEO_MPEG4Level2):
					WriteBits(&writeStream,0x02,EIGHT_BITS/*8*/);
					OstTraceFiltStatic0(TRACE_DEBUG, "MPEG4ENC_Processing: SP Level2 written in header \n",mpeg4enc);
					break;
				case(OMX_VIDEO_MPEG4Level3):
					WriteBits(&writeStream,0x03,EIGHT_BITS/*8*/);
					OstTraceFiltStatic0(TRACE_DEBUG, "MPEG4ENC_Processing: SP Level3 written in header \n",mpeg4enc);
					break;
				case(OMX_VIDEO_MPEG4Level4a):
					WriteBits(&writeStream,0x04,EIGHT_BITS/*8*/);
					OstTraceFiltStatic0(TRACE_DEBUG, "MPEG4ENC_Processing: SP Level4a written in header \n",mpeg4enc);
					break;
				case(OMX_VIDEO_MPEG4Level5):
					WriteBits(&writeStream,0x05,EIGHT_BITS/*8*/);
					OstTraceFiltStatic0(TRACE_DEBUG, "MPEG4ENC_Processing: SP Level6 written in header \n",mpeg4enc);
					break;
				default:
					// if user sets OMX_LevelMax then write L6 related bit in header. This is because currently
					// there is no support for L6 in omx, user will set the level as max if he wants it to be L6
					WriteBits(&writeStream,0x06,EIGHT_BITS/*8*/);	//choose default L6
					OstTraceFiltStatic0(TRACE_DEBUG, "MPEG4ENC_Processing: SP Level6 written in header \n",mpeg4enc);
					break;
			}
			break;
		default:
			WriteBits(&writeStream,0x06,EIGHT_BITS/*8*/);		   //by defualt we choose SP@L6
			break;
	}
	WriteBits(&writeStream,VO_START_CODE/*0x000001b5*/,THIRTY_TWO_BITS/*32*/);
	WriteBits(&writeStream,0,ONE_BIT/*1*/);
	WriteBits(&writeStream,1,FOUR_BITS/*4*/);	// Visual Object Type: 0001: Video ID


	if( pColorPrimary == OMX_ColorPrimaryUnknown)
	{
		OstTraceFiltStatic0(TRACE_DEBUG, "MPEG4ENC_Processing: pColorPrimary == OMX_ColorPrimaryUnknown",mpeg4enc);
		WriteBits(&writeStream,0,ONE_BIT/*1*/);		// video_signal_type
		WriteBits(&writeStream,1,TWO_BITS/*2*/);	// stuffing_bits
	}
	else
	{
		OstTraceFiltStatic1(TRACE_DEBUG, "MPEG4ENC_Processing: pColorPrimary = %d", mpeg4enc, pColorPrimary);
		WriteBits(&writeStream,1,ONE_BIT/*1*/);		// video_signal_type
		WriteBits(&writeStream,0x5,THREE_BITS/*3*/);		// video_format

		if(pColorPrimary == OMX_ColorPrimaryBT601LimitedRange || pColorPrimary == OMX_ColorPrimaryBT709LimitedRange)
		{
			WriteBits(&writeStream,0,ONE_BIT/*1*/);		// video_range is 0
		}
		else if(pColorPrimary == OMX_ColorPrimaryBT601FullRange || pColorPrimary == OMX_ColorPrimaryBT709FullRange)
		{
			WriteBits(&writeStream,1,ONE_BIT/*1*/);		// video_range is 1
		}

		WriteBits(&writeStream,1,ONE_BIT/*1*/);		// colour_description

		if(pColorPrimary == OMX_ColorPrimaryBT709FullRange || pColorPrimary == OMX_ColorPrimaryBT709LimitedRange)
		{
			WriteBits(&writeStream,0x1,EIGHT_BITS/*8*/);		// color_primaries
			WriteBits(&writeStream,0x1,EIGHT_BITS/*8*/);		// transfer_characteristics
			WriteBits(&writeStream,0x1,EIGHT_BITS/*8*/);		// matrix_coefficients
		}
		else if(pColorPrimary == OMX_ColorPrimaryBT601FullRange || pColorPrimary == OMX_ColorPrimaryBT601LimitedRange)
		{
			WriteBits(&writeStream,0x4,EIGHT_BITS/*8*/);		// color_primaries
			WriteBits(&writeStream,0x4,EIGHT_BITS/*8*/);		// transfer_characteristics
			WriteBits(&writeStream,0x4,EIGHT_BITS/*8*/);		// matrix_coefficients
		}

		if(writeStream.totalBitsWritten %8 !=0)
		{
			WriteBits(&writeStream,0,ONE_BIT/*1*/);
			while(writeStream.totalBitsWritten %8 !=0)
			{
				WriteBits(&writeStream,1,ONE_BIT/*1*/);
			}
		}
	}


	//write video object
	WriteBits(&writeStream,VO_CODE/*0x00000100*/,THIRTY_TWO_BITS/*32*/);
	//write vol
	WriteBits(&writeStream,VOL_CODE/*0x00000120*/,THIRTY_TWO_BITS/*32*/);
	WriteBits(&writeStream,0,ONE_BIT/*1*/);
	WriteBits(&writeStream,1,EIGHT_BITS/*8*/);
	WriteBits(&writeStream,0,ONE_BIT/*1*/);


	if(pPixelAspectRatio->nHorizontal == 1 && pPixelAspectRatio->nVertical == 1)
	{
		OstTraceFiltStatic0(TRACE_DEBUG, "MPEG4ENC_Processing: PAR 1:1 Square written in header",mpeg4enc);
		WriteBits(&writeStream,1,FOUR_BITS/*4*/);	// Aspect Ratio	= Square
	}
	else if(pPixelAspectRatio->nHorizontal == 4 && pPixelAspectRatio->nVertical == 3)
	{
		OstTraceFiltStatic0(TRACE_DEBUG, "MPEG4ENC_Processing: PAR 4:3 written in header",mpeg4enc);
		WriteBits(&writeStream,0x2,FOUR_BITS/*4*/);	// Aspect Ratio	= 4:3
	}
	else if(pPixelAspectRatio->nHorizontal == 16 && pPixelAspectRatio->nVertical == 9)
	{
		OstTraceFiltStatic0(TRACE_DEBUG, "MPEG4ENC_Processing: PAR 16:9 written in header",mpeg4enc);
		WriteBits(&writeStream,0x4,FOUR_BITS/*4*/);	// Aspect Ratio	= 16:9
	}
	else
	{
		OstTraceFiltStatic2(TRACE_DEBUG, "MPEG4ENC_Processing: Custom PAR horizontal : %d Vertical :%d written in header",mpeg4enc,pPixelAspectRatio->nHorizontal,pPixelAspectRatio->nVertical);
		WriteBits(&writeStream,0xF,FOUR_BITS/*4*/);	// Use Extended_PAR
		WriteBits(&writeStream,pPixelAspectRatio->nHorizontal,EIGHT_BITS/*8*/);	//Horizontal Resolution
		WriteBits(&writeStream,pPixelAspectRatio->nVertical,EIGHT_BITS/*8*/);	//Vertical Resolution
	}

	/*in case of no bufferring we don't write vbv info*/
	if((mBufferingModel == SVA_BUFFERING_NONE) || (pMpeg4ShortDCI->bEnableShortDCI == OMX_TRUE))	//FIXME define index/structs for buffering model
	{
		OstTraceFiltStatic0(TRACE_DEBUG, "MPEG4ENC_Processing: Writing buffering information in header",mpeg4enc);
		WriteBits(&writeStream,0,ONE_BIT/*1*/);//no vbv
	}
	else
	{
		OstTraceFiltStatic0(TRACE_DEBUG, "MPEG4ENC_Processing: NOT Writing buffering information in header",mpeg4enc);
		WriteBits(&writeStream,1,ONE_BIT/*1*/);//vol control param
		WriteBits(&writeStream,1,TWO_BITS/*2*/);//chroma format
		WriteBits(&writeStream,1,ONE_BIT/*1*/);//low delay
		WriteBits(&writeStream,1,ONE_BIT/*1*/);//vbv_param
		WriteBits(&writeStream,0,FIFTEEN_BITS/*15*/);//first_half_bit_rate
		WriteBits(&writeStream,1,ONE_BIT/*1*/);//marker_bit
		WriteBits(&writeStream,bitRate/400,FIFTEEN_BITS/*15*/);//latter_half_bit_rate //< FIXME correction required, (bitrate+399)/400
		WriteBits(&writeStream,1,ONE_BIT/*1*/);//marker_bit

		//< if(swisBuffer%16384)
		//<		WriteBits(&writeStream,(vbvBufferSize/16384+1)>>3,FIFTEEN_BITS/*15*/);//first_half_vbv_buffer_size
		//< else
		//< 	WriteBits(&writeStream,(vbvBufferSize/16384)>>3,FIFTEEN_BITS/*15*/);//first_half_vbv_buffer_size

		WriteBits(&writeStream,mVbvBufferSize>>3,FIFTEEN_BITS/*15*/);//first_half_vbv_buffer_size


		WriteBits(&writeStream,1,ONE_BIT/*1*/);//marker_bit

		//< if(swisBuffer%16384)
		//< 	WriteBits(&writeStream,(vbvBufferSize/16384+1)&7,THREE_BITS/*3*/);//latter_half_vbv_buffer_size
		//< else
		//< 	WriteBits(&writeStream,(vbvBufferSize/16384)&7,THREE_BITS/*3*/);//latter_half_vbv_buffer_size

		WriteBits(&writeStream,mVbvBufferSize & 0x7,THREE_BITS/*3*/);//latter_half_vbv_buffer_size

		//< WriteBits(&writeStream,0,ELEVEN_BITS/*11*/);//first_half_vbv_occupancy
		WriteBits(&writeStream,mVbvOccupancy>>15,ELEVEN_BITS/*11*/);//first_half_vbv_occupancy
		WriteBits(&writeStream,1,ONE_BIT/*1*/);//marker_bit
		//< WriteBits(&writeStream,vbvOccupancy/64,FIFTEEN_BITS/*15*/);//latter_half_vbv_occupancy
		WriteBits(&writeStream,mVbvOccupancy & 0x7FFF,FIFTEEN_BITS/*15*/);//latter_half_vbv_occupancy
		WriteBits(&writeStream,1,ONE_BIT/*1*/);//marker_bit
	}
	WriteBits(&writeStream,0,TWO_BITS/*2*/);//rectangular
	WriteBits(&writeStream,1,ONE_BIT/*1*/);
	WriteBits(&writeStream,mpeg4enc->mParam.m_enc_param.nTimeIncRes,SIXTEEN_BITS/*16*/);
	WriteBits(&writeStream,1,ONE_BIT/*1*/);
	WriteBits(&writeStream,0,ONE_BIT/*1*/);//fix_vop_rated
	//WriteBits(&writeStream,pMp4Conf->vopTimeIncrement,nbBits);
	WriteBits(&writeStream,1,ONE_BIT/*1*/);

	/* +Change start for CR343589 Rotation */
	switch(pRotation->nRotation)
	{
		case 90   :
		case -90  :
		case 270  :
		case -270 :
			width = mpeg4enc->getFrameHeight(1);
			height = mpeg4enc->getFrameWidth(1);
			break;

		default:
			width = mpeg4enc->getFrameWidth(1);
			height = mpeg4enc->getFrameHeight(1);
			break;
	}
	/* -Change end for CR343589 Rotation */

	if (width ==0)
	{
		OstTraceInt1(TRACE_ERROR, "MPEG4ENC_ProcessingComponent : Writing ZERO width (Write_VOS_VO_VOL) at line no %d ",__LINE__);
	}

	WriteBits(&writeStream,width,THIRTEEN_BITS/*13*/);
	WriteBits(&writeStream,1,ONE_BIT/*1*/);

	if (height ==0)
	{
		OstTraceInt1(TRACE_ERROR, "MPEG4ENC_ProcessingComponent : Writing ZERO height (Write_VOS_VO_VOL) at line no %d ",__LINE__);
	}

	WriteBits(&writeStream,height,THIRTEEN_BITS/*13*/);
	WriteBits(&writeStream,1,ONE_BIT/*1*/);
	WriteBits(&writeStream,0,ONE_BIT/*1*/);//interlaced
	WriteBits(&writeStream,1,ONE_BIT/*1*/); //OBMC Disable
	WriteBits(&writeStream,0,ONE_BIT/*1*/);//sprite enable
	WriteBits(&writeStream,0,ONE_BIT/*1*/); //NOT_8_BIT
	WriteBits(&writeStream,0,ONE_BIT/*1*/); //QUANT_TYPE
	WriteBits(&writeStream,1,ONE_BIT/*1*/);//complexity disable
	if(pErrCorr->bEnableDataPartitioning==OMX_TRUE)
	{
		OstTraceFiltStatic0(TRACE_DEBUG, "MPEG4ENC_Processing: Writing Data Partitioning information in header",mpeg4enc);
		WriteBits(&writeStream,0,ONE_BIT/*1*/);	// resync marker disable
		WriteBits(&writeStream,1,ONE_BIT/*1*/);//partitionned data
		if(pErrCorr->bEnableRVLC==OMX_TRUE)
		{
			WriteBits(&writeStream,1,ONE_BIT/*1*/);//reversible code
		}
		else
		{
			WriteBits(&writeStream,0,ONE_BIT/*1*/);//no reversible code
		}
	}
	else
	{
		OstTraceFiltStatic0(TRACE_DEBUG, "MPEG4ENC_Processing: NOT Writing Data Partitioning information in header",mpeg4enc);
		WriteBits(&writeStream,1,ONE_BIT/*1*/);
		WriteBits(&writeStream,0,ONE_BIT/*1*/);//no partitionned data
	}

	WriteBits(&writeStream,0,ONE_BIT/*1*/);//scalability

	/*padd to aligned*/
	if(writeStream.totalBitsWritten %8 !=0)
	{
		WriteBits(&writeStream,0,ONE_BIT/*1*/);
		while(writeStream.totalBitsWritten %8 !=0)
		{
			WriteBits(&writeStream,1,ONE_BIT/*1*/);
		}
	}
	
	OstTraceFiltStatic1(TRACE_FLOW, "MPEG4Enc_ProcessingComponent::Write_VOS_VO_VOL(): VOS_VO_VOL header of size % bits written", mpeg4enc, writeStream.totalBitsWritten);
	
	OstTraceFiltStatic0(TRACE_API, "<= MPEG4Enc_ProcessingComponent::Write_VOS_VO_VOL()", mpeg4enc);

	return writeStream.totalBitsWritten;
}

void MPEG4Enc_ProcessingComponent::set_pMecIn_parameters(MPEG4Enc_Proxy *pProxyComponent, ts_t1xhv_vec_mpeg4_param_in *pMecIn_parameters, ts_ddep_vec_mpeg4_custom_param_in *pMecIn_custom_parameters)
{
	OstTraceFiltStatic0(TRACE_API, "=> MPEG4Enc_ProcessingComponent::set_pMecIn_parameters()", pProxyComponent);

	t_uint16 mb_num=0;

	t_uint32 mb_per_gob = 0, first_mb_offset = 0;

	pMecIn_parameters->picture_coding_type             = 0/*OMX_VIDEO_PictureTypeI*/;
	if(pProxyComponent->mParam.m_enc_param.bSVH ==OMX_TRUE)
		pMecIn_parameters->flag_short_header               = 1;
	else
		pMecIn_parameters->flag_short_header			   = 0;

	OstTraceFiltStatic1(TRACE_FLOW, "set_pMecIn_parameters(): flag_short_header = 0x%x", pProxyComponent, pMecIn_parameters->flag_short_header);

	/* +Change start for CR343589 Rotation */
	switch(pProxyComponent->mParam.m_rotation.nRotation)
	{
		case 90   :
		case -90  :
		case 270  :
		case -270 :
			//Swapping width and height values for rotation of 90 and 270 degrees
			pMecIn_parameters->frame_width                 = pProxyComponent->getFrameHeight(0);
			pMecIn_parameters->frame_height                = pProxyComponent->getFrameWidth(0);
			pMecIn_parameters->window_width                = pProxyComponent->getFrameHeight(1);
			pMecIn_parameters->window_height               = pProxyComponent->getFrameWidth(1);

			break;
		default:
			//Not swapping width and height values
			pMecIn_parameters->frame_width                 = pProxyComponent->getFrameWidth(0);
			pMecIn_parameters->frame_height                = pProxyComponent->getFrameHeight(0);
			pMecIn_parameters->window_width                = pProxyComponent->getFrameWidth(1);
			pMecIn_parameters->window_height               = pProxyComponent->getFrameHeight(1);

			break;
	}
	/* -Change end for CR343589 Rotation */
	
	pMecIn_parameters->window_horizontal_offset        = pProxyComponent->mParam.m_crop_param.nLeft;
	pMecIn_parameters->window_vertical_offset          = pProxyComponent->mParam.m_crop_param.nTop;

	OstTraceFiltStatic2(TRACE_FLOW, "set_pMecIn_parameters(): Frame Width = %d  Frame Height = %d \n", pProxyComponent, pMecIn_parameters->frame_width, pMecIn_parameters->frame_height);
	OstTraceFiltStatic2(TRACE_FLOW, "set_pMecIn_parameters(): Window Width = %d  Window Height = %d \n", pProxyComponent, pMecIn_parameters->window_width, pMecIn_parameters->window_height);
	OstTraceFiltStatic2(TRACE_FLOW, "set_pMecIn_parameters(): Horiz Offset = %d  Vert Offset = %d \n", pProxyComponent, pMecIn_parameters->window_horizontal_offset, pMecIn_parameters->window_vertical_offset);

	if(pMecIn_parameters->flag_short_header ==1)
	{
		//>//> OstTrace1(TRACE_NORMAL,MPEG4ENC_NMF_SETMECIN_GOBHEADERFREQ, "Gob Header Freq = %d\n", pProxyComponent->mParam.m_enc_h263_param.nGOBHeaderInterval);

		pMecIn_parameters->gob_header_freq             = pProxyComponent->mParam.m_enc_h263_param.nGOBHeaderInterval;
		// is set in brc component, cycled between 0-3 for consecutive frames
		pMecIn_parameters->gob_frame_id                = 0x0;
	}
	else
	{
		pMecIn_parameters->gob_header_freq             = 0x0000;	//only for sh??
		pMecIn_parameters->gob_frame_id                = 0x0;	//only for sh??
	}


	if( pProxyComponent->getColorFormat(0)== OMX_COLOR_FormatYUV420Planar)
	{
	    pMecIn_parameters->yuvformat = 0 ;
	}
	else if ( pProxyComponent->getColorFormat(0)== OMX_COLOR_FormatYUV420SemiPlanar)
	{
		pMecIn_parameters->yuvformat = 1 ;
	}
	else
	{
		pMecIn_parameters->yuvformat = 0x256; //MPC case setting random value, not required for MPC
	}
	if(pProxyComponent->mParam.m_err_corr.bEnableDataPartitioning == OMX_TRUE)
		pMecIn_parameters->data_partitioned            = 1;
	else
		pMecIn_parameters->data_partitioned			   = 0;

	//rvlc is only valid for sp and dp case
	if((pMecIn_parameters->flag_short_header ==0) && (pMecIn_parameters->data_partitioned ==1))
	{
		//> OstTrace1(TRACE_NORMAL,MPEG4ENC_NMF_SETMECIN_ENABLE_RVLC, "bReversibleVLC = %d\n", pProxyComponent->mParam.m_enc_param.bReversibleVLC);

		pMecIn_parameters->reversible_vlc   = ((pProxyComponent->mParam.m_enc_param.bReversibleVLC == OMX_TRUE)?1:0);
	}


	if(pMecIn_parameters->flag_short_header ==1)  // disable for SH
	{
		pMecIn_parameters->hec_freq                    = 0x0000;
	}
	else
	{
		if(pMecIn_parameters->data_partitioned == 1)
		{
			//> OstTrace1(TRACE_NORMAL,MPEG4ENC_NMF_SETMECIN_NHEADEREXTENSION, "nHeaderExtension = %d\n", pProxyComponent->mParam.m_enc_param.nHeaderExtension);
			pMecIn_parameters->hec_freq                    = pProxyComponent->mParam.m_enc_param.nHeaderExtension;	//< FIXME should bEnableHECCodes be chechked?
		}
		else
			pMecIn_parameters->hec_freq					   = 0x0000;
	}

	pMecIn_parameters->modulo_time_base            = 0x0000;

	if(pMecIn_parameters->flag_short_header ==1)  // disable for SH
	{

		pMecIn_parameters->vp_size_type                 = 0x0000;
		pMecIn_parameters->vp_size_max                  = 0x0800;
		pMecIn_parameters->vp_bit_size                  = 0x0000;
		pMecIn_parameters->vp_mb_size                   = 0x0000;
	}
	else
	{
		//Changing vp_size_type to 0 from 3, so that algo takes only
		// vp_bit_size into consideration
		pMecIn_parameters->vp_size_type                 = 0x0000;

		// packet size are constaraints by level setting(refer mpeg4 standard)
		switch(pProxyComponent->mParam.m_enc_param.eLevel)
		{
			case(OMX_VIDEO_MPEG4Level0):
				pMecIn_parameters->vp_size_max  = 2048;
				break;
			case(OMX_VIDEO_MPEG4Level1):
				pMecIn_parameters->vp_size_max  = 2048;
				break;
			case(OMX_VIDEO_MPEG4Level2):
				pMecIn_parameters->vp_size_max  = 4096;
				break;
			case(OMX_VIDEO_MPEG4Level3):
				pMecIn_parameters->vp_size_max  = 8192;
				break;
			case(OMX_VIDEO_MPEG4Level4a):
				pMecIn_parameters->vp_size_max  = 8192;
				break;
			case(OMX_VIDEO_MPEG4Level5):
				pMecIn_parameters->vp_size_max  = 8192;
				break;
			default:
				pMecIn_parameters->vp_size_max  = 8192;
				break;
		}

		if(pMecIn_parameters->data_partitioned == 1)
		{
			//> OstTrace1(TRACE_NORMAL,MPEG4ENC_NMF_SETMECIN_NMAXPACKETSIZE, "nMaxPacketSize = %d\n", pProxyComponent->mParam.m_enc_param.nMaxPacketSize);

			//nMaxPacketSize is in bytes, vp_bit_size is in bits
			pMecIn_parameters->vp_bit_size                  = 8*pProxyComponent->mParam.m_enc_param.nMaxPacketSize;
			//0x14 value was based on FW team inputs
			pMecIn_parameters->vp_mb_size                   = 0x0014;

		}
		else
		{
			pMecIn_parameters->vp_bit_size                  = 0x0000;
			pMecIn_parameters->vp_mb_size                   = 0x0000;

		}
	}
	if(pMecIn_parameters->flag_short_header == 0)
	{
		//> OstTrace1(TRACE_NORMAL,MPEG4ENC_NMF_SETMECIN_NTIMEINCRES, "nTimeIncRes = %d\n", pProxyComponent->mParam.m_enc_param.nTimeIncRes);
		//> OstTrace1(TRACE_NORMAL,MPEG4ENC_NMF_SETMECIN_XENCODEFRAMERATE, "xEncodeFramerate = %x\n", pProxyComponent->mParam.m_framerate.xEncodeFramerate);

		pMecIn_parameters->vop_time_increment_resolution    = pProxyComponent->mParam.m_enc_param.nTimeIncRes;
		//< FIXME experimental change
		//< pMecIn_parameters->vop_time_increment               = (pProxyComponent->mParam.m_enc_param.nTimeIncRes/(pProxyComponent->mParam.m_framerate.xEncodeFramerate>>16));
		//< pMecIn_parameters->fixed_vop_time_increment         = 0x0001;
		pMecIn_parameters->vop_time_increment               = 0x0;
		pMecIn_parameters->fixed_vop_time_increment         = (pProxyComponent->mParam.m_enc_param.nTimeIncRes/(pProxyComponent->mParam.m_framerate.xEncodeFramerate>>16));
	}
	else
	{
		pMecIn_parameters->vop_time_increment_resolution    = 30000;
		pMecIn_parameters->vop_time_increment               = 1001;
		pMecIn_parameters->fixed_vop_time_increment         = 1001;
	}

	OstTraceFiltStatic1(TRACE_FLOW, "MPEG4ENC_Processing: pMecIn_parameters->vop_time_increment_resolution = %d", pProxyComponent, pMecIn_parameters->vop_time_increment_resolution);
	OstTraceFiltStatic1(TRACE_FLOW, "MPEG4ENC_Processing: pMecIn_parameters->vop_time_increment = %d", pProxyComponent, pMecIn_parameters->vop_time_increment);
	OstTraceFiltStatic1(TRACE_FLOW, "MPEG4ENC_Processing: pMecIn_parameters->fixed_vop_time_increment = %d", pProxyComponent, pMecIn_parameters->fixed_vop_time_increment);

	/* +Change start for CR343589 Rotation */
	pMecIn_parameters->me_type                 = 0x0;
	/* -Change end for CR343589 Rotation */

	pMecIn_parameters->init_me                  = 0x1;
	pMecIn_parameters->vop_fcode_forward        = 0x0001;


	//FIMXE check RTYPE before setting this field
	pMecIn_parameters->rounding_type               = 0;

	mb_num = (t_uint16)((pMecIn_parameters->frame_width)/16) * ((pMecIn_parameters->frame_height)/16);

	// if user sets OMX_VIDEO_IntraRefreshBoth and sets either of nAirMBs=0 or nCirMBs=0, then
	// the corresponding IR scheme is disabled explicity. i.e. if user sets IntraRefreshBoth and set nAirMBs to 0,
	// then the fw is programmed only as IntraRefreshCyclic. Similar is case with IntraRefreshBoth and nCirMBs =0
	// This understanding was reached based on validation inputs
	if(pProxyComponent->mParam.m_intra_ref.eRefreshMode == OMX_VIDEO_IntraRefreshAdaptive)
	{
		//> OstTrace1(TRACE_NORMAL,MPEG4ENC_NMF_SETMECIN_NAIRMBS_1, "nAirMBs = %d\n", pProxyComponent->mParam.m_intra_ref.nAirMBs);

		pMecIn_parameters->intra_refresh_type           = 1;
		if(pProxyComponent->mParam.m_intra_ref.nCirMBs)
			pMecIn_parameters->air_mb_num                   = pProxyComponent->mParam.m_intra_ref.nAirMBs;
		else	//nMB is 0 i.e. air is disabled
		{
			pMecIn_parameters->intra_refresh_type           = 0;
			pMecIn_parameters->air_mb_num                   = 0;
		}
		pMecIn_parameters->cir_period_max               = 0;
	}
	else if(pProxyComponent->mParam.m_intra_ref.eRefreshMode == OMX_VIDEO_IntraRefreshCyclic)
	{
		//> OstTrace1(TRACE_NORMAL,MPEG4ENC_NMF_SETMECIN_NCIRMBS_1, "nCirMBs = %d\n", pProxyComponent->mParam.m_intra_ref.nCirMBs);

		pMecIn_parameters->intra_refresh_type           = 2;
		pMecIn_parameters->air_mb_num                   = 0;
		if(pProxyComponent->mParam.m_intra_ref.nCirMBs)
			pMecIn_parameters->cir_period_max               = (mb_num/(pProxyComponent->mParam.m_intra_ref.nCirMBs));
		else	//nMB is 0 i.e. cir is disabled
		{
			pMecIn_parameters->intra_refresh_type           = 0;
			pMecIn_parameters->cir_period_max               = 0;
		}
	}
	else if(pProxyComponent->mParam.m_intra_ref.eRefreshMode == OMX_VIDEO_IntraRefreshBoth)
	{
		//> OstTrace1(TRACE_NORMAL,MPEG4ENC_NMF_SETMECIN_NAIRMBS_2, "nAirMBs = %d\n", pProxyComponent->mParam.m_intra_ref.nAirMBs);
		//> OstTrace1(TRACE_NORMAL,MPEG4ENC_NMF_SETMECIN_NCIRMBS_2, "nCirMBs = %d\n", pProxyComponent->mParam.m_intra_ref.nCirMBs);

		if((pProxyComponent->mParam.m_intra_ref.nAirMBs!=0) && (pProxyComponent->mParam.m_intra_ref.nCirMBs !=0))
		{	//both
			pMecIn_parameters->intra_refresh_type           = 3;
			pMecIn_parameters->air_mb_num                   = pProxyComponent->mParam.m_intra_ref.nAirMBs;
			pMecIn_parameters->cir_period_max               = (mb_num/(pProxyComponent->mParam.m_intra_ref.nCirMBs));
		}
		else if((pProxyComponent->mParam.m_intra_ref.nAirMBs==0) && (pProxyComponent->mParam.m_intra_ref.nCirMBs !=0))
		{	//cyclic only
			pMecIn_parameters->intra_refresh_type           = 2;
			pMecIn_parameters->air_mb_num                   = 0;
			pMecIn_parameters->cir_period_max               = (mb_num/(pProxyComponent->mParam.m_intra_ref.nCirMBs));
		}
		else if((pProxyComponent->mParam.m_intra_ref.nAirMBs!=0) && (pProxyComponent->mParam.m_intra_ref.nCirMBs ==0))
		{	//adaptive only
			pMecIn_parameters->intra_refresh_type           = 1;
			pMecIn_parameters->air_mb_num                   = pProxyComponent->mParam.m_intra_ref.nAirMBs;
			pMecIn_parameters->cir_period_max               = 0;	//should have been set as infinity
		}
		else if((pProxyComponent->mParam.m_intra_ref.nAirMBs==0) && (pProxyComponent->mParam.m_intra_ref.nCirMBs ==0))
		{	//none
			pMecIn_parameters->intra_refresh_type           = 0;
			pMecIn_parameters->air_mb_num                   = 0;
			pMecIn_parameters->cir_period_max               = 0;	//should have been set as infinity
		}
	}
	else
	{
		pMecIn_parameters->intra_refresh_type           = 0;
		pMecIn_parameters->air_mb_num                   = 0;
		pMecIn_parameters->cir_period_max               = 0;
	}

	OstTraceFiltStatic1(TRACE_DEBUG, "MPEG4ENC_Processing: pMecIn_parameters->intra_refresh_type = %d", pProxyComponent, pMecIn_parameters->intra_refresh_type);
	OstTraceFiltStatic1(TRACE_DEBUG, "MPEG4ENC_Processing: pMecIn_parameters->air_mb_num = %d", pProxyComponent, pMecIn_parameters->air_mb_num);
	OstTraceFiltStatic1(TRACE_DEBUG, "MPEG4ENC_Processing: pMecIn_parameters->cir_period_max = %d", pProxyComponent, pMecIn_parameters->cir_period_max);
	//> OstTrace1(TRACE_NORMAL,MPEG4ENC_NMF_SETMECIN_QUANT, "nQp = %d\n", pProxyComponent->mParam.m_Qp.nQpI);

	if(pMecIn_parameters->picture_coding_type  == 0/*OMX_VIDEO_PictureTypeI*/)
		pMecIn_parameters->quant                    = pProxyComponent->mParam.m_Qp.nQpI;
	else	/*picture coding type == P_FRAME */
		pMecIn_parameters->quant					= pProxyComponent->mParam.m_Qp.nQpP;

	//> OstTraceExt2(TRACE_NORMAL,MPEG4ENC_NMF_SETMECIN_ECONTROLRATE, "eControlRate = %d, Bitrate = %d \n", pProxyComponent->mParam.m_bitrate.eControlRate, pProxyComponent->mParam.m_bitrate.nTargetBitrate);

	// OMX_Video_ControlRateVariableSkipFrames and OMX_Video_ControlRateConstantSkipFrames are included just to take care
	// incase user sets them by mistake.
	// by default VBR and CBR involve frame skipping
	if(pProxyComponent->mParam.m_bitrate.eControlRate == OMX_Video_ControlRateDisable)
		pMecIn_parameters->brc_type                 = 0;
	else if((pProxyComponent->mParam.m_bitrate.eControlRate == OMX_Video_ControlRateVariable) || (pProxyComponent->mParam.m_bitrate.eControlRate == OMX_Video_ControlRateVariableSkipFrames))
		pMecIn_parameters->brc_type                 = 3;
	else if((pProxyComponent->mParam.m_bitrate.eControlRate == OMX_Video_ControlRateConstant) || (pProxyComponent->mParam.m_bitrate.eControlRate == OMX_Video_ControlRateConstantSkipFrames))
		pMecIn_parameters->brc_type                 = 2;



	pMecIn_parameters->brc_frame_target             =0x00000000;	//only used if brc type == 1 CBR
	pMecIn_parameters->brc_target_min_pred          =0x00000000;	//only used if brc type == 2/3/4/5/6/7
	pMecIn_parameters->brc_target_max_pred      =0x00000000;	//only used if brc type == 2/3/4/5/6/7
	pMecIn_parameters->skip_count                   =0x00000000;	//only used if brc type == 2/3/4/5/6/7


	if(pProxyComponent->isMpcObject)
	{
		pMecIn_parameters->bit_rate                 = ENDIANESS_CONVERSION(pProxyComponent->mParam.m_bitrate.nTargetBitrate);
	}
	else
	{
		pMecIn_parameters->bit_rate                 = (pProxyComponent->mParam.m_bitrate.nTargetBitrate);
	}

	pMecIn_parameters->framerate                =  (pProxyComponent->mParam.m_framerate.xEncodeFramerate>>16)<<10;
	pMecIn_parameters->ts_modulo                =0x0000;
	pMecIn_parameters->ts_seconds               =0x0000;
	//< #define A_THR(fr)		(27-(((fr)*39322)>>16))		/* This formula gives 18 if frame rate is 15 Hz, and 9 if 30 Hz */
	//< pMecIn_parameters->air_thr                  =0x0012;
	//< don't hardcode air_th to 12 (as done before, it is dependant on framerate)
	pMecIn_parameters->air_thr                  = (27-(((pProxyComponent->mParam.m_framerate.xEncodeFramerate>>16)*39322)>>16));

	pMecIn_parameters->delta_target             =0x00000000;
	pMecIn_parameters->minQp                    =0x0002;
	pMecIn_parameters->maxQp                    =0x001F;

	#if 0	//< set in brc module, no need to set it here
	mb_num = (pMecIn_parameters->frame_width)/16 * (pMecIn_parameters->frame_height)/16;

	if(mb_num <= 99)
		pMecIn_parameters->Smax                    = ENDIANESS_CONVERSION(64*1024);
	else if(mb_num <= 396)
		pMecIn_parameters->Smax                    = ENDIANESS_CONVERSION(256*1024);
	else if(mb_num <= 1584)
		pMecIn_parameters->Smax                    = ENDIANESS_CONVERSION(512*1024);
	else
		pMecIn_parameters->Smax					   = ENDIANESS_CONVERSION(1024*1024);

	#endif

	pMecIn_parameters->min_base_quality            =0x0000;
	pMecIn_parameters->min_framerate               =10;//>15;
	pMecIn_parameters->max_buff_level              =0x0000;

	pMecIn_parameters->first_I_skipped_flag            =0x0000;
	pMecIn_parameters->init_ts_modulo_old              =0xFFFF;


	//Setting slice_loss_first_mb and ..._mb_num using OMX_VIDEO_FASTUPDATETYPE set by user.
	if(pMecIn_parameters->flag_short_header == 1)
	{
		//Short Header - we calculate no. of MBs per GOB - see H.263 spec
		if (pMecIn_parameters->frame_height <= 400) {
			mb_per_gob = (pMecIn_parameters->frame_width)/16;	//implies 1 row makes 1 GOB
		}
		else {
			mb_per_gob = (pMecIn_parameters->frame_width)/8;	//implies 2 rows make 1 GOB; => No. of MBs per GOB is doubled
		}

		first_mb_offset = mb_per_gob * pProxyComponent->mParam.m_fast_update.nFirstGOB;
	}
	else
	{
		//For SP, GOB is interpreted as video packet.
		//In case of SP, we ignore the nFirstGOB field and take the nFirstMb as the number of first video packet
		first_mb_offset = 0;
	}

	if(pProxyComponent->mParam.m_fast_update.bEnableVFU == OMX_TRUE)
	{
		pMecIn_parameters->slice_loss_first_mb[0]          = first_mb_offset + pProxyComponent->mParam.m_fast_update.nFirstMB;
		pMecIn_parameters->slice_loss_mb_num[0]            = pProxyComponent->mParam.m_fast_update.nNumMBs;
	}
	else
	{
		pMecIn_parameters->slice_loss_first_mb[0]          =0;
		pMecIn_parameters->slice_loss_mb_num[0]            =0;
	}

	for(t_uint16 count = 1; count<=7; count++)
	{
		pMecIn_parameters->slice_loss_first_mb[count]          =0;
		pMecIn_parameters->slice_loss_mb_num[count]            =0;
	}

	pMecIn_custom_parameters->profile = (pProxyComponent->mParam.m_enc_param.eProfile == OMX_VIDEO_MPEG4ProfileSimple)?0:1;
	switch(pProxyComponent->mParam.m_enc_param.eLevel)
	{
		case(OMX_VIDEO_MPEG4Level0):
			pMecIn_custom_parameters->level = 0;
			break;
			/*case(OMX_VIDEO_MPEG4Level0b):
					WriteBits(&writeStream,0x01,8);
					break;*/
		case(OMX_VIDEO_MPEG4Level1):
			pMecIn_custom_parameters->level = 1;
			break;
		case(OMX_VIDEO_MPEG4Level2):
			pMecIn_custom_parameters->level = 2;
			break;
		case(OMX_VIDEO_MPEG4Level3):
			pMecIn_custom_parameters->level = 3;
			break;
		case(OMX_VIDEO_MPEG4Level4a):
			pMecIn_custom_parameters->level = 4;
			break;
		case(OMX_VIDEO_MPEG4Level5):
			pMecIn_custom_parameters->level = 5;
			break;
		default:
			pMecIn_custom_parameters->level = 6;	//< by default L6
			break;
	}
	if(pMecIn_parameters->flag_short_header == 1 && pProxyComponent->mParam.m_enc_h263_param.eProfile == OMX_VIDEO_H263ProfileISWV2)
	{
		//This is where all H263 profile 3 related in_param fields are filled (Annexes I, J, K and T)

		pMecIn_parameters->h263_p3 			= 1;	//H263 Profile 3 features enable
		pMecIn_parameters->adv_intra_coding = 1;	//Annex I
		pMecIn_parameters->mod_quant_mode	= 1;	//Annex T	


		//Annex J
		if(pProxyComponent->mParam.m_deblocking.bDeblocking == OMX_TRUE) {
			pMecIn_parameters->deblock_filter	= 1;	//Annex J
		}
		else {
			pMecIn_parameters->deblock_filter	= 0;	//Annex J
		}

		//Now for Annex K -- Slice Structured Coding 
		if(pProxyComponent->mParam.m_enc_param.nMaxPacketSize != 0 && pProxyComponent->mParam.m_enc_param.nSliceHeaderSpacing == 0) {
			//so Vp is != 0 and Vm is 0
			pMecIn_parameters->slice_mode 		= 1;	//Annex K
		}
		else if(pProxyComponent->mParam.m_enc_param.nMaxPacketSize == 0 && pProxyComponent->mParam.m_enc_param.nSliceHeaderSpacing != 0) {
			//so Vp is 0 and Vm != 0
			pMecIn_parameters->slice_mode 		= 2;	//Annex K
		}
		else if(pProxyComponent->mParam.m_enc_param.nMaxPacketSize != 0 && pProxyComponent->mParam.m_enc_param.nSliceHeaderSpacing == 0) {
			//so Vp and Vm both != 0
			pMecIn_parameters->slice_mode 		= 3;	//Annex K
		}
		else {
			//so Vp and Vm both = 0
			pMecIn_parameters->slice_mode 		= 0;
		}
	}
	else
	{
		pMecIn_parameters->h263_p3 			= 0;	//H263 Profile 3 features enable
		pMecIn_parameters->adv_intra_coding = 0;	//Annex I
		pMecIn_parameters->slice_mode 		= 0;	//Annex K
		pMecIn_parameters->deblock_filter	= 0;	//Annex J
		pMecIn_parameters->mod_quant_mode	= 0;	//Annex T
	}

	//Pixel Aspect Ratio info
	pMecIn_custom_parameters->par_horizontal 	= (t_uint32) pProxyComponent->mParam.m_pixel_aspect_ratio.nHorizontal;
	pMecIn_custom_parameters->par_vertical 		= (t_uint32) pProxyComponent->mParam.m_pixel_aspect_ratio.nVertical;

	//Color Primary Info
	if(pProxyComponent->mParam.m_color_primary == OMX_ColorPrimaryBT709FullRange)
	{
		pMecIn_custom_parameters->color_primary = SVA_COLOR_PRIMARY_BT709_FULLRANGE;
	}
	else if(pProxyComponent->mParam.m_color_primary == OMX_ColorPrimaryBT709LimitedRange)
	{
		pMecIn_custom_parameters->color_primary = SVA_COLOR_PRIMARY_BT709_LIMITEDRANGE;
	}
	else if(pProxyComponent->mParam.m_color_primary == OMX_ColorPrimaryBT601FullRange)
	{
		pMecIn_custom_parameters->color_primary = SVA_COLOR_PRIMARY_BT601_FULLRANGE;
	}
	else if(pProxyComponent->mParam.m_color_primary == OMX_ColorPrimaryBT601LimitedRange)
	{
		pMecIn_custom_parameters->color_primary = SVA_COLOR_PRIMARY_BT601_LIMITEDRANGE;
	}
	else
	{
		pMecIn_custom_parameters->color_primary = SVA_COLOR_PRIMARY_UNKNOWN;
	}

	//custom parameters
	if(pProxyComponent->isMpcObject)
	{
		if(pMecIn_parameters->flag_short_header==0)
		{
			pMecIn_custom_parameters->no_of_p_frames = ENDIANESS_CONVERSION(pProxyComponent->mParam.m_enc_param.nPFrames);
		}
		else
		{
			pMecIn_custom_parameters->no_of_p_frames = ENDIANESS_CONVERSION(pProxyComponent->mParam.m_enc_h263_param.nPFrames);
		}
	}
	else
	{
		if(pMecIn_parameters->flag_short_header==0)
		{
			pMecIn_custom_parameters->no_of_p_frames = (pProxyComponent->mParam.m_enc_param.nPFrames);
		}
		else
		{
			pMecIn_custom_parameters->no_of_p_frames = (pProxyComponent->mParam.m_enc_h263_param.nPFrames);
		}
	}

	// SP: CBR-CustomVBV, VBR-VBV, ConstantQp-None
	// SH: CBR-HRD, VBR-AnnexG, ConstantQp-None

#if 1		//FIXME check
	if((pMecIn_parameters->flag_short_header == 0))
	{
		if(pMecIn_parameters->brc_type == 2)	//CBR
		{
			pMecIn_custom_parameters->buffering_model = SVA_BUFFERING_VBV;	//FIXME custom VBV here
		}
		else if(pMecIn_parameters->brc_type == 3)	//VBR
		{
			pMecIn_custom_parameters->buffering_model = SVA_BUFFERING_VBV;
		}
		else
		{
			pMecIn_custom_parameters->buffering_model = SVA_BUFFERING_NONE;
		}
	}
	else if((pMecIn_parameters->flag_short_header == 1))
	{
		if(pMecIn_parameters->brc_type == 2)	//CBR
		{
			pMecIn_custom_parameters->buffering_model = SVA_BUFFERING_HRD;
		}
		else if(pMecIn_parameters->brc_type == 3)	//VBR
		{
			pMecIn_custom_parameters->buffering_model = SVA_BUFFERING_ANNEXG;
		}
		else
		{
			pMecIn_custom_parameters->buffering_model = SVA_BUFFERING_NONE;
		}
	}
#endif

	/* +Change start for CR338066 MPEG4Enc DCI */
	pMecIn_custom_parameters->short_dci_enable = ((pProxyComponent->mParam.m_short_dci.bEnableShortDCI == OMX_TRUE) ? 1 : 0 );
	/* -Change end for CR338066 MPEG4Enc DCI */

	//< pMecIn_custom_parameters->buffering_model = SVA_BUFFERING_NONE ;
	mBufferingModel  = (t_sva_brc_buffering_model)pMecIn_custom_parameters->buffering_model;
	//< bitrate already endianess converted, so reconvert it back for proper value
	if(pProxyComponent->isMpcObject)
	{
		mSwisBufSize = ENDIANESS_CONVERSION(pMecIn_parameters->bit_rate);
	}
	else
	{
		mSwisBufSize = (pMecIn_parameters->bit_rate);
	}

	if(mSwisBufSize%16384)
		mVbvBufferSize = mSwisBufSize/16384+1;
	else
		mVbvBufferSize = mSwisBufSize/16384;

	//< bitrate already endianess converted, so reconvert it back for proper value
	if(pProxyComponent->isMpcObject)
	{
		mVbvOccupancy = (ENDIANESS_CONVERSION(pMecIn_parameters->bit_rate)>>6)*2/3;
	}
	else
	{
		mVbvOccupancy = ((pMecIn_parameters->bit_rate)>>6)*2/3;
	}

	if( mVbvOccupancy > MAX_VBV_OCCUPANCY ) {
		mVbvOccupancy = MAX_VBV_OCCUPANCY;
		//The reference encoder puts a check on the parameter -Ro passed to it
		//and gives an error, if simply (bitrate>>6) is passed as -Ro,
		//since it may exceed the maximum valid VBV_OCCUPANCY of 63488.
		//This check here corresponds to the same.
	}

	//< limit the swisbuf, vbvbuf and vbvocc based on profile level limitations
	switch(pProxyComponent->mParam.m_enc_param.eLevel)
	{
		case(OMX_VIDEO_MPEG4Level0):
			if(mVbvBufferSize > 10)
				mVbvBufferSize = 10;
			break;
			/*case(OMX_VIDEO_MPEG4Level0b):
			if(mVbvBufferSize > 10)
				mVbvBufferSize = 10;

					break;*/
		case(OMX_VIDEO_MPEG4Level1):
			if(mVbvBufferSize > 10)
				mVbvBufferSize = 10;
			break;
		case(OMX_VIDEO_MPEG4Level2):
			if(mVbvBufferSize > 40)
				mVbvBufferSize = 40;
			break;
		case(OMX_VIDEO_MPEG4Level3):
			if(mVbvBufferSize > 40)
				mVbvBufferSize = 40;
			break;
		case(OMX_VIDEO_MPEG4Level4a):
			if(mVbvBufferSize > 80)
				mVbvBufferSize = 80;
			break;
		case(OMX_VIDEO_MPEG4Level5):
			if(mVbvBufferSize > 112)
				mVbvBufferSize = 112;
			break;
		default:	//< by default L6
			if(mVbvBufferSize > 248)
				mVbvBufferSize = 248;
			break;
	}

	if((mVbvOccupancy *64) > (mVbvBufferSize*16384))
		mVbvOccupancy	= 171 * mVbvBufferSize;		//< taken from ref code

	if(pProxyComponent->isMpcObject)
	{
		pMecIn_custom_parameters->swis_buffer 		= ENDIANESS_CONVERSION(mSwisBufSize);	//< endianess not converted, so convert again
		pMecIn_custom_parameters->vbv_occupancy 	= ENDIANESS_CONVERSION(mVbvOccupancy);
		pMecIn_custom_parameters->vbv_buffer_size 	= ENDIANESS_CONVERSION((mVbvBufferSize*16384));	// The algo requires that the value passed to it is NOT already divided (see BRC variable vbvBufferSizeIn16384BitsUnit)
	}
	else
	{
		pMecIn_custom_parameters->swis_buffer 		= (mSwisBufSize);	//< endianess not converted, so convert again
		pMecIn_custom_parameters->vbv_occupancy 	= (mVbvOccupancy);
		pMecIn_custom_parameters->vbv_buffer_size 	= ((mVbvBufferSize*16384));	// The algo requires that the value passed to it is NOT already divided (see BRC variable vbvBufferSizeIn16384BitsUnit)
	}

	OstTraceFiltStatic1(TRACE_DEBUG, "MPEG4ENC_Processing: mSwisBufSize = %d", pProxyComponent, mSwisBufSize);
	OstTraceFiltStatic1(TRACE_DEBUG, "MPEG4ENC_Processing: mVbvOccupancy = %d", pProxyComponent, mVbvOccupancy);
	OstTraceFiltStatic1(TRACE_DEBUG, "MPEG4ENC_Processing: mVbvBufferSize*16384 = %d", pProxyComponent, mVbvBufferSize*16384);

	pMecIn_custom_parameters->IPictureQp = pProxyComponent->mParam.m_Qp.nQpI;
	pMecIn_custom_parameters->PPictureQp = pProxyComponent->mParam.m_Qp.nQpP;
	pMecIn_custom_parameters->spatial_quality   = SVA_SPATIAL_QUALITY_NONE;	//< FIXME

	//> OstTrace1(TRACE_NORMAL,MPEG4ENC_NMF_SETMECIN_INTRAREFRESHVOP, "IntraRefreshVOP = %d\n", pProxyComponent->mParam.m_force_intra.IntraRefreshVOP);

	OstTraceFiltStatic1(TRACE_FLOW, "MPEG4ENC_Processing: pProxyComponent->mParam.m_force_intra.IntraRefreshVOP = %d \n",pProxyComponent,pProxyComponent->mParam.m_force_intra.IntraRefreshVOP);
	// dynamic config
/* +Change start for ER 348538 I-frame request is not applied to correct frame */
	
	//>Now we don't pass force_intra information in the parameter buffer. Instead, 
	//> this information is passed with the next input buffer received through a flag. This
	//> is to ensure that the I-frame request does not get applied to the next UNPROCESSED 
	//> buffer (held in queue) instead of the next PUSHED buffer.
	
	//>pMecIn_custom_parameters->force_intra = (pProxyComponent->mParam.m_force_intra.IntraRefreshVOP == OMX_TRUE)?1:0;
	// reset the force intra member variable after setting is passed to fw param,
	// this is to take care that, next frame is not forced intra again, unless user explicitly sets it again
	//>pProxyComponent->mParam.m_force_intra.IntraRefreshVOP = OMX_FALSE;
/* -Change end for ER 348538 I-frame request is not applied to correct frame */
	pMecIn_parameters->framerate = (pProxyComponent->mParam.m_framerate.xEncodeFramerate>>16)<<10;

	if(pProxyComponent->isMpcObject)
	{
		pMecIn_parameters->bit_rate = ENDIANESS_CONVERSION(pProxyComponent->mParam.m_bitrate.nTargetBitrate);
	}
	else
	{
		pMecIn_parameters->bit_rate = (pProxyComponent->mParam.m_bitrate.nTargetBitrate);
	}


	if(pProxyComponent->isMpcObject)
	{
			OstTraceFiltStatic2(TRACE_FLOW, "MPEG4ENC_Processing: FrameRate = %d  BitRate = %d \n", pProxyComponent, (pMecIn_parameters->framerate)>>10, ENDIANESS_CONVERSION(pMecIn_parameters->bit_rate));
	}
	else
	{
			OstTraceFiltStatic2(TRACE_FLOW, "MPEG4ENC_Processing: FrameRate = %d  BitRate = %d \n", pProxyComponent, (pMecIn_parameters->framerate)>>10, pMecIn_parameters->bit_rate);
	}



	//< other configs, not implemented currently //FIXME

	// pMecIn_parameters->min_base_quality =
	// pMecIn_parameters->min_framerate =
	// pMecIn_custom_parameters->no_of_p_frames =
	// pMecIn_parameters->gob_header_freq =
	// pMecIn_parameters->hec_freq =
	// pMecIn_parameters->air_mb_num =
	// pMecIn_parameters->cir_period_max =
	// packet size and info is also dynamically configurable

	OstTraceFiltStatic0(TRACE_API, "<= MPEG4Enc_ProcessingComponent::set_pMecIn_parameters()", pProxyComponent);
}

